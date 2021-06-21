/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <stdio.h>
#include <malloc.h>
#include <QCoreApplication>
#include <QString>
#include "XWApplication.h"
#include "XWDVICrypt.h"
#include "XWDVIType.h"
#include "XWICCP.h"
#include "XWObject.h"
#include "XWDVIRef.h"
#include "XWDVIDev.h"
#include "XWDVIColorSpaces.h"


static unsigned char  nullbytes16[16] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static int
iccp_devClass_allowed(XWDVIDev * devA, iccSig dev_class)
{
  	int colormode = devA->getParam(PDF_DEV_PARAM_COLORMODE);

  	switch (colormode) 
  	{
#if 0
  		case PDF_DEV_COLORMODE_PDFX1:
    		break;
    		
  		case PDF_DEV_COLORMODE_PDFX3:
    		if (dev_class != str2iccSig("prtr")) 
    		{
      			return 0;
    		}
    		break;
#endif

  		default:
    		if (dev_class != str2iccSig("scnr") && 
				dev_class != str2iccSig("mntr") && 
				dev_class != str2iccSig("prtr") && 
				dev_class != str2iccSig("spac")) 
			{
      			return 0;
    		}
    		break;
  		}

  	return 1;
}

class DVIColorSpace
{
public:
	DVIColorSpace();
	~DVIColorSpace();
	
	void clear();
	
	void flush(XWDVIRef * xrefA, bool finished = true);
	
	XWObject * getReference(XWDVIRef * xref, XWObject * ref);
	
public:
	char    *ident;
  	int      subtype;
  	void    *cdata;

  	XWObject resource;
  	XWObject reference;
};

DVIColorSpace::DVIColorSpace()
	:ident(0),
	 subtype(PDF_COLORSPACE_TYPE_INVALID),
	 cdata(0)
{
	resource.initNull();
	reference.initNull();
}

DVIColorSpace::~DVIColorSpace()
{
	if (ident)
		delete [] ident;
		
	if (cdata)
	{
		switch (subtype) 
		{
    		case PDF_COLORSPACE_TYPE_ICCBASED:
      			release_iccbased_cdata((iccbased_cdata*)cdata);
      			break;
      			
      		default:
      			break;
    	}
	}
	
	resource.free();
	reference.free();
}

void DVIColorSpace::flush(XWDVIRef * xrefA, bool finished)
{
	if (finished)
	{
		if (!resource.isNull())
			xrefA->releaseObj(&resource);
		
		if (!reference.isNull())
			xrefA->releaseObj(&reference);
	}
	else
	{
		XWObject obj;
		if (!resource.isNull())
		{
			resource.copy(&obj);
			xrefA->releaseObj(&obj);
		}
		
		if (!reference.isNull())
		{
			reference.copy(&obj);
			xrefA->releaseObj(&obj);
		}
	}
}

XWObject * DVIColorSpace::getReference(XWDVIRef * xref, XWObject * ref)
{
	if (reference.isNull()) 
  		xref->refObj(&resource, &reference);
  		
  	reference.copy(ref);
  	
  	return ref;
}

XWDVIColorSpaces::XWDVIColorSpaces(QObject * parent)
	:QObject(parent),
	 count(0),
	 capacity(0),
	 colorSpaces(0)
{
}

XWDVIColorSpaces::~XWDVIColorSpaces()
{
	if (colorSpaces)
  		free(colorSpaces);
}

void XWDVIColorSpaces::close(XWDVIRef * xref, bool finished)
{
	for (int i = 0; i < count; i++) 
	{
    	DVIColorSpace * colorspace = colorSpaces[i];
    	colorspace->flush(xref);
    	if (finished)
    		delete colorspace;
  	}
  	
  	if (finished)
  	{
  		if (colorSpaces)
  			free(colorSpaces);
  		
  		colorSpaces = 0;
  		count = capacity = 0;
  	}
}

XWObject * XWDVIColorSpaces::getColorSpaceReference(XWDVIRef * xref, int cspc_id, XWObject * ref)
{
	return colorSpaces[cspc_id]->getReference(xref, ref);
}

int XWDVIColorSpaces::iccpCheckColorSpace(int colortype, 
	                                      const void *profile, 
	                                      long proflen)
{
	if (!profile || proflen < 128)
    	return -1;

  	uchar * p = (uchar *) profile;

  	iccSig colorspace = str2iccSig(p + 16);
  	switch (colortype) 
  	{
  		case PDF_COLORSPACE_TYPE_CALRGB:
  		case PDF_COLORSPACE_TYPE_RGB:
    		if (colorspace != str2iccSig("RGB ")) 
      			return -1;
    		break;
    		
  		case PDF_COLORSPACE_TYPE_CALGRAY:
  		case PDF_COLORSPACE_TYPE_GRAY:
    		if (colorspace != str2iccSig("GRAY")) 
      			return -1;
    		break;
    		
  		case PDF_COLORSPACE_TYPE_CMYK:
    		if (colorspace != str2iccSig("CMYK")) 
      			return -1;
    		break;
    		
  		default:
    		return -1;
    		break;
  	}

  	return 0;
}

XWObject * XWDVIColorSpaces::iccpGetRenderingIntent(const void *profile, 
	                                                long proflen,
	                                                XWObject * obj)
{
	if (!profile || proflen < 128)
    	return 0;

  	uchar * p = (uchar *) profile;

  	long intent = (p[64] << 24)|(p[65] << 16)|(p[66] << 8)|p[67];
  	switch (ICC_INTENT_TYPE(intent)) 
  	{
  		case ICC_INTENT_SATURATION:
  			obj->initName("Saturation");
    		break;
    		
  		case ICC_INTENT_PERCEPTUAL:
  			obj->initName("Perceptual");
    		break;
    		
  		case ICC_INTENT_ABSOLUTE:
  			obj->initName("AbsoluteColorimetric");
    		break;
    		
  		case ICC_INTENT_RELATIVE:
  			obj->initName("RelativeColorimetric");
    		break;
    		
  		default:
  			return 0;
  	}

  	return obj;
}

int XWDVIColorSpaces::iccpLoadProfile(XWDVIRef * xref,
	                                  XWDVIDev * devA, 
	                                  const char *ident,
		                              const void *profile, 
		                              long proflen)
{
	iccHeader icch;
	iccp_init_iccHeader(&icch);
  	if (iccp_unpack_header(&icch, profile, proflen, 1) < 0)
  	{ 
  		QString msg = QString(tr("Invalid ICC profile header in \"%1\".\n")).arg(ident);
  		xwApp->warning(msg);
    	return -1;
    }
    	
    if (!iccp_version_supported(xref->getVersion(), (icch.version >> 24) & 0xff,
			      (icch.version >> 16) & 0xff)) 
	{
		QString msg = QString(tr("ICC profile format spec. version %1.%2.%3"
		                         " not supported in current PDF version setting.\n"))
		                      .arg((icch.version >> 24) & 0xff)
		                      .arg((icch.version >> 20) & 0x0f)
		                      .arg((icch.version >> 16) & 0x0f);
    	xwApp->warning(msg);
    	return -1;
  	}
  	
  	if (!iccp_devClass_allowed(devA, icch.devClass)) 
  	{
    	xwApp->warning(tr("unsupported ICC Profile Device Class.\n"));
    	return -1;
  	}
  	
  	int colorspace = 0;
  	if (icch.colorSpace == str2iccSig("RGB ")) 
    	colorspace = PDF_COLORSPACE_TYPE_RGB;
  	else if (icch.colorSpace == str2iccSig("GRAY")) 
    	colorspace = PDF_COLORSPACE_TYPE_GRAY;
  	else if (icch.colorSpace == str2iccSig("CMYK")) 
    	colorspace = PDF_COLORSPACE_TYPE_CMYK;
  	else 
  	{
    	xwApp->warning(tr("unsupported input color space.\n"));
    	return -1;
  	}
  	
  	uchar checksum[16];
  	iccp_get_checksum(checksum, profile, proflen);
  	if (memcmp(icch.ID,  nullbytes16, 16) && memcmp(icch.ID,  checksum, 16)) 
  	{
    	xwApp->warning(tr("invalid ICC profile: Inconsistent checksum.\n"));
    	return -1;
  	}
  	
  	iccbased_cdata * cdata = (iccbased_cdata*)malloc(sizeof(iccbased_cdata));
  	init_iccbased_cdata(cdata);
  	cdata->colorspace = colorspace;
  	memcpy(cdata->checksum, checksum, 16);
  	
  	int cspc_id = findResource(ident, PDF_COLORSPACE_TYPE_ICCBASED, cdata);
  	if (cspc_id >= 0)
  	{
  		release_iccbased_cdata(cdata);
    	return cspc_id;
  	}
  	
  	XWObject resource;
  	resource.initArray(xref);
  	
  	XWObject stream;
  	stream.initStream(STREAM_COMPRESS, xref);
  	
  	XWObject obj;
  	obj.initName("ICCBased");
  	resource.arrayAdd(&obj);
  	xref->refObj(&stream, &obj);
  	resource.arrayAdd(&obj);
  	
  	obj.initInt(get_num_components_iccbased(cdata));
  	XWDict * stream_dict = stream.streamGetDict();
  	stream_dict->add(qstrdup("N"), &obj);
  	stream.streamAdd((const char*)profile, proflen);
  	xref->releaseObj(&stream);
  	cspc_id = defineResource(ident, PDF_COLORSPACE_TYPE_ICCBASED, cdata, &resource);

  	return cspc_id;
}

int XWDVIColorSpaces::defineResource(const char *ident, 
	                                 int subtype, 
	                                 void *cdata, 
	                                 XWObject *resource)
{
	if (count >= capacity) 
	{
    	capacity   += 16;
    	colorSpaces = (DVIColorSpace**)realloc(colorSpaces, capacity * sizeof(DVIColorSpace*));
  	}
  	
  	int cspc_id = count;
  	DVIColorSpace * colorspace = new DVIColorSpace;
  	colorSpaces[cspc_id] = colorspace;
  	if (ident) 
    	colorspace->ident = qstrdup(ident);
    	
  	colorspace->subtype  = subtype;
  	colorspace->cdata    = cdata;
  	colorspace->resource = *resource;
  	
  	count++;
  	return cspc_id;
}

int XWDVIColorSpaces::findResource(const char *ident, int type, const void *cdata)
{
	int cmp = -1;
	for (int cspc_id = 0; cmp && cspc_id < count; cspc_id++) 
	{
    	DVIColorSpace * colorspace = colorSpaces[cspc_id];
    	if (colorspace->subtype != type)
      		continue;

    	switch (colorspace->subtype) 
    	{
    		case PDF_COLORSPACE_TYPE_ICCBASED:
      			cmp = compare_iccbased(ident, 
      			                       (iccbased_cdata*)cdata, 
      			                       colorspace->ident, 
      			                       (iccbased_cdata*)(colorspace->cdata));
      			break;
    	}
    	
    	if (!cmp)
      		return cspc_id;
  	}

  	return -1;
}

