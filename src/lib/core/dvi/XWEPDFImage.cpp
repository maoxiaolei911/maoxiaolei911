/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWApplication.h"
#include "XWObject.h"
#include "XWDVIRef.h"
#include "XWPDFFile.h"
#include "XWEPDFImage.h"

XWEPDFImage::XWEPDFImage(XWDVIRef * xref,
	                     const char *identA, 
	                     int  subtypeA,
	                     int  formatA,
	                     long page_noA, 	           
	                     const QString & filenameA,
	                     XWObject *dictA)
	:XWDVIImage(xref, identA, subtypeA, formatA, page_noA, filenameA, dictA)
{
}

int XWEPDFImage::load(XWDVIRef * xref,QIODevice *fp)
{
	XWPDFFile * pf = XWPDFFile::open(xref, ident, fp);
	if (!pf)
    	return -1;
    	
    XWObject pageA, resourcesA, contentsA, tmp, markinfo, tmp1, content_new, dict;
    pageA.initNull();
    resourcesA.initNull();
    contentsA.initNull();
    
    XWDVIXFormInfo infoA;    
    
    if (pf->getVersion() > xref->getVersion())
    	goto too_recent;
    
    long page_noA = page_no;
    long count = 0;
    if (!pf->getPage(&page_noA, &count, &infoA.bbox, &resourcesA, &pageA))
    	goto error_silent;

    XWObject * catalog = pf->getCatalog();
    catalog->dictLookupNF("MarkInfo", &tmp);
    if (XWPDFFile::derefObj(xref, &tmp, &markinfo))
    {
    	tmp.free(); 
    	markinfo.dictLookupNF("Marked", &tmp);
    	tmp1.initNull();
    	XWPDFFile::derefObj(xref, &tmp, &tmp1);
    	tmp.free();    		
    	markinfo.free();
    	if (!tmp1.isBool()) 
    	{
			tmp1.free();
      		goto error;
    	} 
    	else if (tmp.getBool())
      		xwApp->warning("file contains tagged PDF. Ignoring tags.\n");
    	tmp1.free();
  	}
  	else
  		tmp.free();
  	
  	setPage(page_noA, count);
  	pageA.dictLookupNF("Contents", &tmp);
  	XWPDFFile::derefObj(xref, &tmp, &contentsA);
	tmp.free(); 
	pageA.free();
	{
		content_new.initNull();
		if (contentsA.isNull())
			content_new.initStream(0, xref);
		else if (contentsA.isStream())
			XWPDFFile::importObject(xref, &contentsA, &content_new);
		else if (contentsA.isArray())
		{
			int len = contentsA.arrayGetLength();
			content_new.initStream(STREAM_COMPRESS, xref);
			XWObject content_seg;
			content_seg.initNull();
			for (int idx = 0; idx < len; idx++)
			{
				contentsA.arrayGetNF(idx, &tmp);
				XWPDFFile::derefObj(xref, &tmp, &content_seg);
				tmp.free();
				if (!content_seg.isStream() || content_new.streamConcat(&content_seg) < 0)
				{
					 content_seg.free();
	  				 content_new.free();
	  				 goto error;
				}
				content_seg.free();
			}
		}
		else
			goto error;
			
		if (!contentsA.isNull())
      		contentsA.free();
    	contentsA = content_new;
	}
	
	{
		XWObject bbox, matrix;
		tmp.initName("XObject");
		contentsA.streamGetDict()->add(qstrdup("Type"), &tmp);
		tmp.initName("Form");
		contentsA.streamGetDict()->add(qstrdup("Subtype"), &tmp);
		tmp.initReal(1.0);
		contentsA.streamGetDict()->add(qstrdup("FormType"), &tmp);
		tmp.initNull();
		
		bbox.initArray(xref);
		tmp.initReal(infoA.bbox.llx);
		bbox.arrayAdd(&tmp);
		tmp.initReal(infoA.bbox.lly);
		bbox.arrayAdd(&tmp);
		tmp.initReal(infoA.bbox.urx);
		bbox.arrayAdd(&tmp);
		tmp.initReal(infoA.bbox.ury);
		bbox.arrayAdd(&tmp);
		contentsA.streamGetDict()->add(qstrdup("BBox"), &bbox);
		
		matrix.initArray(xref);
		tmp.initReal(1.0);
		matrix.arrayAdd(&tmp);
		tmp.initReal(0.0);
		matrix.arrayAdd(&tmp);
		tmp.initReal(0.0);
		matrix.arrayAdd(&tmp);
		tmp.initReal(1.0);
		matrix.arrayAdd(&tmp);
		tmp.initReal(0.0);
		matrix.arrayAdd(&tmp);
		tmp.initReal(0.0);
		matrix.arrayAdd(&tmp);
		contentsA.streamGetDict()->add(qstrdup("Matrix"), &matrix);
		
		tmp.initNull();
		XWPDFFile::importObject(xref, &resourcesA, &tmp);
		contentsA.streamGetDict()->add(qstrdup("Resources"), &tmp);
		resourcesA.free();
	}
	
	pf->close();

  	setForm(xref, &infoA, &contentsA);

  	return 0;

error:
  	xwApp->warning("cannot parse document. Broken PDF file?\n");
  	
error_silent:	
  	resourcesA.free();
    markinfo.free();
    pageA.free();
    contentsA.free();

  	pf->close();

  	return -1;

too_recent:
  	pf->close();
  	xwApp->warning("PDF version of input file more recent than in output file.\n");
    xwApp->warning("use \"-V\" switch to change output PDF version.\n");
    return -1;
}

