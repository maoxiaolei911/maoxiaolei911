/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QImageReader>
#include <QTextCodec>
#include "XWApplication.h"
#include "XWPictureSea.h"
#include "XWDVICore.h"
#include "XWDVIRef.h"
#include "XWDVIDev.h"
#include "XWPDFFile.h"
#include "XWDVIImage.h"
#include "XWMPost.h"
#include "XWBMPImage.h"
#include "XWJPGImage.h"
#include "XWPNGImage.h"

#ifdef XW_BUILD_PS_LIB
#include "XWPSInterpreter.h"
#endif //XW_BUILD_PS_LIB

#include "XWEPDFImage.h"

XWDVIImageInfo::XWDVIImageInfo()
{
	flags  = 0;
    width  = 0;
  	height = 0;
  	bits_per_component = 0;
  	num_components = 0;
  	min_dpi = 0;
  	xdensity = ydensity = 1.0;
}

XWDVIXFormInfo::XWDVIXFormInfo()
{
	flags    = 0;
  	bbox.llx = 0;
  	bbox.lly = 0;
  	bbox.urx = 0;
  	bbox.ury = 0;
  	matrix.a = 1.0;
  	matrix.b = 0.0;
  	matrix.c = 0.0;
  	matrix.d = 1.0;
  	matrix.e = 0.0;
  	matrix.f = 0.0;
}

XWDVIImage::XWDVIImage(XWDVIRef * xref,
	                   const char *identA, 
	                   int  subtypeA,
	                   int  formatA,
	                   long page_noA, 	           
	                   const QString & filenameA,
	                   XWObject *dictA)
{
	ident = qstrdup(identA);
	subtype  = subtypeA;
	format = formatA;
	page_no = page_noA; 
	filename = filenameA;
	
	page_count = 0;
	memset(res_name, 0, 16);
	
	attr.width = attr.height = 0;
  	attr.xdensity = attr.ydensity = 1.0;
  	attr.bbox.llx = attr.bbox.lly = 0;
  	attr.bbox.urx = attr.bbox.ury = 0;
  	
  	reference.initNull();
  	resource.initDict(xref);
  	attr_dict.initNull();
  	if (!dictA)
  		attr_dict = *dictA;
}

XWDVIImage::~XWDVIImage()
{
	delete [] ident;
		
	reference.free();
  	resource.free();
  	attr_dict.free();
}

void XWDVIImage::clean(bool finish)
{
	if (finish)
	{
		resource.free();
		resource.initNull();
		attr_dict.free();
		attr_dict.initNull();
	}
}

void XWDVIImage::doMPSPage(XWDVICore * core,
	                       XWDVIRef * xref, 
	                       XWDVIDev * dev,
	                       QIODevice *image_file)
{
	XWMPost::doPage(core, xref, dev, image_file);
}

XWObject * XWDVIImage::getReference(XWDVIRef * xref, XWObject * obj)
{
	if (reference.isNull())
		xref->refObj(&resource, &reference);
		
	reference.copy(obj);
	return obj;
}

XWDVIImage * XWDVIImage::loadImage(XWDVICore * core,
	                               XWDVIRef * xref, 
	                               XWDVIDev * dev, 
	                               int id,
	                               const char *identA, 
	                               long page_noA, 
	                               XWObject *dictA)
{
	if (!identA)
		return 0;
		
	QTextCodec * codec = QTextCodec::codecForLocale();
	QString fn = codec->toUnicode(identA);
	XWPictureSea sea;
	bool is_tmp = false;
	
#ifdef XW_BUILD_PS_LIB
	if (fn.endsWith(".ps", Qt::CaseInsensitive) || 
		  fn.endsWith(".eps", Qt::CaseInsensitive))
	{
		XWPSInterpreter interpreter;
		fn = interpreter.toPDF(fn, core->getPageWidth(), core->getPageHeight());
		if (fn.isEmpty())
			return 0;
	}
#endif
	
	QFile * fp = sea.openFile(fn, &is_tmp);
	if (!fp)
	{
		QString msg = QString("fail to find image file \"%1\" or convert it to supported format.\n").arg(fn);
		xwApp->error(msg);
		return 0;
	}
	
	QString filenameA = fp->fileName();	
	
	int formatA = sourceImageType(fp);
	if (formatA == IMAGE_TYPE_UNKNOWN)
	{
		fp->close();
		if (is_tmp)
			fp->remove();
		delete fp;
		return 0;
	}
	
	XWDVIImage * ret = 0;
	XWMPost * mps = 0;
	XWJPGImage * jpg = 0;
	XWPNGImage * png = 0;
	XWBMPImage * bmp = 0;
	XWEPDFImage * pdf = 0;
	switch (formatA)
	{
		case IMAGE_TYPE_MPS:
			mps = new XWMPost(xref, identA, -1, formatA, page_noA, filenameA, dictA);
			ret = mps;
			if (mps->load(core, xref, dev, fp) < 0)
			{
			  delete mps;
			  ret = 0;
			}
			break;
			
		case  IMAGE_TYPE_JPEG:
			jpg = new XWJPGImage(xref, identA, PDF_XOBJECT_TYPE_IMAGE, formatA, page_noA, filenameA, dictA);
			ret = jpg;
			if (xref->isBatch())
			  if (jpg->load(core, xref, fp) < 0)
			  {
				  delete jpg;
				  ret = 0;
			  }
			break;
			
		case  IMAGE_TYPE_PNG:
			png = new XWPNGImage(xref, identA, PDF_XOBJECT_TYPE_IMAGE, formatA, page_noA, filenameA, dictA);
			ret = png;
			if (xref->isBatch())
			  if (png->load(core, xref, fp) < 0)
			  {
				  delete png;
				  ret = 0;
			  }
			break;
			
		case  IMAGE_TYPE_BMP:
			bmp = new XWBMPImage(xref, identA, PDF_XOBJECT_TYPE_IMAGE, formatA, page_noA, filenameA, dictA);
			ret = bmp;
			if (xref->isBatch())
			  if (bmp->load(xref, fp) < 0)
			  {
				  delete bmp;
				  ret = 0;
			  }
			break;
			
		default:
			pdf = new XWEPDFImage(xref, identA, PDF_XOBJECT_TYPE_FORM, formatA, page_noA, filenameA, dictA);
			ret = pdf;
			if (pdf->load(xref, fp) < 0)
			{
			  delete pdf;
			  ret = 0;
			}
			break;
	}
	
	fp->close();
	if (ret)
	{
		XWObject obj;
		ret->getReference(xref, &obj);
	}

	delete fp;
	
	if (ret)
	{
		switch (ret->subtype)
		{
			case PDF_XOBJECT_TYPE_IMAGE:
				sprintf(ret->res_name, "Im%d", id);
				break;
				
			case PDF_XOBJECT_TYPE_FORM:
				sprintf(ret->res_name, "Fm%d", id);
				break;
				
			default:
				break;
		}
	}
	return ret;
}

int  XWDVIImage::scale(PDFTMatrix    *M,
                       PDFRect       *r,
                       TransformInfo *p)
{
	pdf_setmatrix(M, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0);
  	switch (subtype)
  	{
  		case PDF_XOBJECT_TYPE_IMAGE:
    		scaleToFitI(M, p);
    		if (p->flags & INFO_HAS_USER_BBOX) 
    		{
      			r->llx = p->bbox.llx / (attr.width * attr.xdensity);
      			r->lly = p->bbox.lly / (attr.height * attr.ydensity);
      			r->urx = p->bbox.urx / (attr.width * attr.xdensity);
      			r->ury = p->bbox.ury / (attr.height * attr.ydensity);
    		} 
    		else 
    		{
      			r->llx = 0.0;
      			r->lly = 0.0;
      			r->urx = 1.0;
      			r->ury = 1.0;
    		}
    		break;
    		
    	case PDF_XOBJECT_TYPE_FORM:
    		scaleToFitF(M, p);
    		if (p->flags & INFO_HAS_USER_BBOX) 
    		{
      			r->llx = p->bbox.llx;
      			r->lly = p->bbox.lly;
      			r->urx = p->bbox.urx;
      			r->ury = p->bbox.ury;
    		} 
    		else 
    		{
      			r->llx = attr.bbox.llx;
      			r->lly = attr.bbox.lly;
      			r->urx = attr.bbox.urx;
      			r->ury = attr.bbox.ury;
    		}
    		break;
  	}
  	
  	return 0;
}

void XWDVIImage::setAttr(long width, 
	                     long height, 
	                     double xdensity, 
	                     double ydensity, 
	                     double llx, 
	                     double lly, 
	                     double urx, 
	                     double ury)
{
	attr.width = width;
  	attr.height = height;
  	attr.xdensity = xdensity;
  	attr.ydensity = ydensity;
  	attr.bbox.llx = llx;
  	attr.bbox.lly = lly;
  	attr.bbox.urx = urx;
  	attr.bbox.ury = ury;
}

void XWDVIImage::setForm(XWDVIRef * xref, 
	                     void *form_info, 
	                     XWObject *resourceA)
{
	XWDVIXFormInfo * infoA = (XWDVIXFormInfo*)form_info;
	subtype   = PDF_XOBJECT_TYPE_FORM;

  	attr.bbox.llx = infoA->bbox.llx;
  	attr.bbox.lly = infoA->bbox.lly;
  	attr.bbox.urx = infoA->bbox.urx;
  	attr.bbox.ury = infoA->bbox.ury;

	if (reference.isNull())
  		xref->refObj(resourceA, &reference);
  	else
  	{
  		resourceA->label = reference.label;
  		resourceA->generation = reference.generation;
  	}

  	xref->releaseObj(resourceA);
}

void XWDVIImage::setImage(XWDVIRef * xref, 
	                      void *image_info, 
	                      XWObject *resourceA)
{
	if (!resourceA->isStream())
	{
    	xwApp->error("Image XObject must be of stream type.\n");
    	return ;
    }
    
    subtype = PDF_XOBJECT_TYPE_IMAGE;
    XWDVIImageInfo * infoA = (XWDVIImageInfo*)image_info;
    attr.width  = infoA->width;
  	attr.height = infoA->height;
  	attr.xdensity = infoA->xdensity;
  	attr.ydensity = infoA->ydensity;
  	if (reference.isNull())
  		xref->refObj(resourceA, &reference);
  	else
  	{
  		resourceA->label = reference.label;
  		resourceA->generation = reference.generation;
  	}
  	
  	XWObject obj;
  	XWDict * dict = resourceA->getDict();
  	obj.initName("XObject");
  	dict->add(qstrdup("Type"), &obj);
  	obj.initName("Image");
  	dict->add(qstrdup("Subtype"), &obj);
  	obj.initReal(infoA->width);
  	dict->add(qstrdup("Width"), &obj);
  	obj.initReal(infoA->height);
  	dict->add(qstrdup("Height"), &obj);
  	obj.initInt(infoA->bits_per_component);
  	dict->add(qstrdup("BitsPerComponent"), &obj);
  	if (!attr_dict.isNull())
  		dict->merge(attr_dict.getDict());
  		
  	xref->releaseObj(resourceA);
}

void XWDVIImage::setPage(long page_noA, long page_countA)
{
	page_no    = page_noA;
    page_count = page_countA;
}

void XWDVIImage::scaleToFitF(PDFTMatrix *T, TransformInfo *p)
{
	double  s_x, s_y, d_x, d_y;
  	double  wd0, ht0, dp;

  	if (p->flags & INFO_HAS_USER_BBOX) 
  	{
    	wd0 =  p->bbox.urx - p->bbox.llx;
    	ht0 =  p->bbox.ury - p->bbox.lly;
    	d_x = -p->bbox.llx;
    	d_y = -p->bbox.lly;
  	} 
  	else 
  	{
    	wd0 = attr.bbox.urx - attr.bbox.llx;
    	ht0 = attr.bbox.ury - attr.bbox.lly;
    	d_x = 0.0;
    	d_y = 0.0; 
  	}

  	if (wd0 == 0.0) 
    	wd0 = 1.0;
    	
  	if (ht0 == 0.0) 
    	ht0 = 1.0;

  	if ( (p->flags & INFO_HAS_WIDTH ) && (p->flags & INFO_HAS_HEIGHT) ) 
  	{
    	s_x = p->width  / wd0;
    	s_y = (p->height + p->depth) / ht0;
    	dp  = p->depth;
  	} 
  	else if ( p->flags & INFO_HAS_WIDTH ) 
  	{
    	s_x = p->width  / wd0;
    	s_y = s_x;
    	dp  = 0.0;
  	} 
  	else if ( p->flags & INFO_HAS_HEIGHT) 
  	{
    	s_y = (p->height + p->depth) / ht0;
    	s_x = s_y;
    	dp  = p->depth;
  	} 
  	else 
  	{
    	s_x = s_y = 1.0;
    	dp  = 0.0;
  	}

  	T->a = s_x; 
  	T->c = 0.0;
  	T->b = 0.0; 
  	T->d = s_y;
  	T->e = s_x * d_x; 
  	T->f = s_y * d_y - dp;
}

void XWDVIImage::scaleToFitI(PDFTMatrix *T, TransformInfo *p)
{
	double  s_x, s_y, d_x, d_y;
  	double  wd0, ht0, dp, xscale, yscale;
  	if (p->flags & INFO_HAS_USER_BBOX) 
  	{
    	wd0 =  p->bbox.urx - p->bbox.llx;
    	ht0 =  p->bbox.ury - p->bbox.lly;
    	xscale = attr.width * attr.xdensity / wd0;
    	yscale = attr.height * attr.ydensity / ht0;
    	d_x = -p->bbox.llx / wd0;
    	d_y = -p->bbox.lly / ht0;
  	} 
  	else 
  	{
    	wd0 = attr.width * attr.xdensity;
    	ht0 = attr.height * attr.ydensity;
    	xscale = yscale = 1.0;
    	d_x = 0.0;
    	d_y = 0.0; 
  	}

  	if (wd0 == 0.0) 
    	wd0 = 1.0;
    	
  	if (ht0 == 0.0) 
    	ht0 = 1.0;

  	if ( (p->flags & INFO_HAS_WIDTH ) && (p->flags & INFO_HAS_HEIGHT) ) 
  	{
    	s_x = p->width * xscale;
    	s_y = (p->height + p->depth) * yscale;
    	dp  = p->depth * yscale;
  	} 
  	else if ( p->flags & INFO_HAS_WIDTH ) 
  	{
    	s_x = p->width * xscale;
    	s_y = s_x * ((double)attr.height / attr.width);
    	dp  = 0.0;
  	} 
  	else if ( p->flags & INFO_HAS_HEIGHT) 
  	{
    	s_y = (p->height + p->depth) * yscale;
    	s_x = s_y * ((double)attr.width / attr.height);
    	dp  = p->depth * yscale;
  	} 
  	else 
  	{
    	s_x = wd0;
    	s_y = ht0;
    	dp  = 0.0;
  	}
  	
  	T->a = s_x; T->c = 0.0;
  	T->b = 0.0; T->d = s_y;
  	T->e = d_x * s_x / xscale; 
  	T->f = d_y * s_y / yscale - dp;
}

int XWDVIImage::sourceImageType(QIODevice *fp)
{
	int  formatA = IMAGE_TYPE_UNKNOWN;
	if (XWJPGImage::checkForJPEG(fp))
		formatA = IMAGE_TYPE_JPEG;
	else if (XWPNGImage::checkForPNG(fp))
		formatA = IMAGE_TYPE_PNG;
	else if (XWBMPImage::checkForBMP(fp))
		formatA = IMAGE_TYPE_BMP;
	else if (XWPDFFile::checkForPDF(fp))
		formatA = IMAGE_TYPE_PDF;
	else if (XWMPost::checkForMP(fp))
		formatA = IMAGE_TYPE_MPS;
	
	return formatA;
}

#define EBB_DPI 72

XWDVIImageCache::XWDVIImageCache(QObject * parent)
	:QObject(parent),
	 count(0),
	 capacity(0),
	 ximages(0)
{
}

XWDVIImageCache::~XWDVIImageCache()
{
	if (ximages)
	{
		for (int i = 0; i < count; i++)
		{
			if (ximages[i])
				delete ximages[i];
		}
	}
}

void XWDVIImageCache::close(bool finished)
{
	if (ximages)
	{
		for (int i = 0; i < count; i++)
		{
			if (ximages[i])
			{
				ximages[i]->clean(finished);
				delete ximages[i];
			}
		}
		
		if (finished)
		{
			free(ximages);
			ximages = 0;
		}
	}
	
	if (finished)
	{
		count = capacity = 0;
	}
}

int XWDVIImageCache::defineResource(XWDVIRef * xref, 
	                                const char *ident,
			                        int subtype, 
			                        void *info, 
			                        XWObject *resource)
{
	int id = count;
  	if (count >= capacity) 
  	{
    	capacity += 16;
    	ximages   = (XWDVIImage**)realloc(ximages, capacity * sizeof(XWDVIImage*));
  	}
  	
  	QString filename;
  	XWDVIImage* I = new XWDVIImage(xref, ident, subtype, IMAGE_TYPE_UNKNOWN, 0, filename, 0);
  	ximages[id] = I;
  	switch (subtype) 
  	{
  		case PDF_XOBJECT_TYPE_IMAGE:
  			I->setImage(xref, info, resource);
    		sprintf(I->res_name, "Im%d", id);
    		break;
    		
  		case PDF_XOBJECT_TYPE_FORM:
  			I->setForm(xref, info, resource);
    		sprintf(I->res_name, "Fm%d", id);
    		break;
    		
  		default:
    		delete I;
    		ximages[id] = 0;
    		I = 0;
    		id = -1;
    		return -1;
    		break;
  	}
  	
  	count++;

  	return  id;
}

int XWDVIImageCache::findResource(XWDVICore * core,
	                              XWDVIRef * xref, 
	                              XWDVIDev * dev, 
	                              const char *identA, 
	                              long page_noA, 
	                              XWObject *dictA)
{
	int id = 0;
	XWDVIImage * I = 0;
	for (; id < count; id++) 
	{
    	I = ximages[id];
    	if (I->ident && !strcmp(identA, I->ident) && 
    		I->page_no == page_noA + (page_noA < 0 ? I->page_count+1 : 0))
        {
        	if (I->attr_dict.isNull() || (!xref->isScanning()))
      			return  id;
    	}
  	}
  	
  	id = count;
  	I = XWDVIImage::loadImage(core, xref, dev, id, identA, page_noA, dictA);
  	if (!I)
  		return -1;
  		
  	if (count >= capacity) 
  	{
    	capacity += 16;
    	ximages   = (XWDVIImage**)realloc(ximages, capacity * sizeof(XWDVIImage*));
  	}
  	ximages[id] = I;
  	count++;

  	return  id;
}

XWObject * XWDVIImageCache::getReference(XWDVIRef * xref, int id, int page_no, XWObject * obj)
{
	if (id < 0 || id >= count)
		return 0;
		
	XWDVIImage * I = ximages[id];
	PageImages * pimages = 0;
	if (!pageImages.contains(page_no))
	{
		pimages = new PageImages;
		pageImages[page_no] = pimages;
		
		pimages->ids << id;
	}
	else
	{
		pimages = pageImages[page_no];
		if (!pimages->ids.contains(id))
			pimages->ids << id;
	}
		
	return I->getReference(xref, obj);
}

char * XWDVIImageCache::getResName(int id)
{
	if (id < 0 || id >= count)
		return 0;
		
	XWDVIImage * I = ximages[id];
	return I->getResName();
}

int XWDVIImageCache::getSubtype(int id)
{
	if (id < 0 || id >= count)
		return -1;
		
	XWDVIImage * I = ximages[id];
	return I->getSubtype();
}

void XWDVIImageCache::load(XWDVICore * core,
	                       XWDVIRef * xref, 
	                       long page_no)
{
	if (!pageImages.contains(page_no))
		return ;
		
	PageImages * pimages = pageImages[page_no];
	for (int i = 0; i < pimages->ids.size(); i++)
	{
		XWDVIImage * img = ximages[pimages->ids.at(i)];
		if (!img || img->subtype == PDF_XOBJECT_TYPE_FORM || img->filename.isNull())
			continue;
				
		QFile fp(img->filename);
		if (!fp.open(QIODevice::ReadOnly))
			continue;
		switch (img->format)
		{
			case  IMAGE_TYPE_JPEG:
				{
					XWJPGImage * jpg = (XWJPGImage*)img;
					jpg->load(core, xref, &fp);
				}
				break;
			
			case  IMAGE_TYPE_PNG:
				{
					XWPNGImage * png = (XWPNGImage*)img;
					png->load(core, xref, &fp);
				}
				break;
			
			case  IMAGE_TYPE_BMP:
				{
					XWBMPImage * bmp = (XWBMPImage*)img;
					bmp->load(xref, &fp);
				}
				break;
			
			default:
				break;
		}
		
		fp.close();
	}
}

int XWDVIImageCache::scaleImage(int            id,
                                PDFTMatrix    *M,
                                PDFRect       *r,
                                TransformInfo *p)
{
	if (id < 0 || id >= count)
		return -1;
		
	XWDVIImage * I = ximages[id];
	return I->scale(M, r, p);
}

void XWDVIImageCache::setAttr(int id, 
	                          long width, 
	                          long height, 
	                          double xdensity, 
	                          double ydensity, 
	                          double llx, 
	                          double lly, 
	                          double urx, 
	                          double ury)
{
	if (id < 0 || id >= count)
		return;
		
	XWDVIImage * I = ximages[id];
	I->setAttr(width, height, xdensity, ydensity, llx, lly, urx, ury);
}

