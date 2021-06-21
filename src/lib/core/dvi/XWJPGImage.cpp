/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include "XWUtil.h"
#include "XWObject.h"
#include "XWJPG.h"
#include "XWDVIRef.h"
#include "XWDVIDev.h"
#include "XWDVICore.h"
#include "XWDVIColorSpaces.h"
#include "XWJPGImage.h"


XWJPGImage::XWJPGImage(XWDVIRef * xref,
	                   const char *identA, 
	                   int  subtypeA,
	                   int  formatA,
	                   long page_noA, 	           
	                   const QString & filenameA,
	                   XWObject *dictA)
	:XWDVIImage(xref, identA, subtypeA, formatA, page_noA, filenameA, dictA)
{
}

int XWJPGImage::checkForJPEG(QIODevice *fp)
{
	fp->seek(0);
	uchar jpeg_sig[2];
	if (fp->read((char*)jpeg_sig, 2) != 2)
    	return 0;
  	else if (jpeg_sig[0] != 0xff || jpeg_sig[1] != JM_SOI)
    	return 0;

  	return 1;
}

int XWJPGImage::getBBox(QIODevice *fp, 
	                   long *width, 
	                   long *height,
	                   double *xdensity, 
	                   double *ydensity)
{
	JPEG_info j_info;

  	JPEG_info_init(&j_info);

  	if (JPEG_scan_file(&j_info, fp) < 0) 
  	{
    	JPEG_info_clear(&j_info);
    	return -1;
  	}

  	*width  = j_info.width;
  	*height = j_info.height;

  	jpeg_get_density(&j_info, xdensity, ydensity);

  	JPEG_info_clear(&j_info);

  	return 0;
}

int XWJPGImage::load(XWDVICore * core, XWDVIRef * xref, QIODevice *fp)
{
	JPEG_info j_info;
	JPEG_info_init(&j_info);

  	if (JPEG_scan_file(&j_info, fp) < 0) 
  	{
    	JPEG_info_clear(&j_info);
    	return -1;
  	}
  	
  	int colortype = -1;
  	switch (j_info.num_components) 
  	{
  		case 1:
    		colortype = PDF_COLORSPACE_TYPE_GRAY;
    		break;
    		
  		case 3:
    		colortype = PDF_COLORSPACE_TYPE_RGB;
    		break;
    		
  		case 4:
    		colortype = PDF_COLORSPACE_TYPE_CMYK;
    		break;
    		
  		default:
    		JPEG_info_clear(&j_info);
    		return -1;
  	}
  	
  	XWObject stream, obj, colorspace;
  	stream.initStream(0, xref);
  	
  	obj.initName("DCTDecode");
  	stream.streamGetDict()->add(qstrdup("Filter"), &obj);
  	colorspace.initNull();
  	if (j_info.flags & HAVE_APPn_ICC)
  	{
  		XWObject icc_stream;
  		if (JPEG_get_iccp(xref, &j_info, &icc_stream))
  		{
  			char * dptr = icc_stream.streamDataPtr();
  			ulong  len = icc_stream.streamGetLength();
  			if (XWDVIColorSpaces::iccpCheckColorSpace(colortype, dptr, len) == 0)
  			{
  				int cspc_id = core->iccpLoadProfile(0, dptr, len);
  				if (cspc_id >= 0)
				{
					core->getColorSpaceReference(cspc_id, &colorspace);
					XWObject intent;
					intent.initNull();
					XWDVIColorSpaces::iccpGetRenderingIntent(dptr, len, &intent);
	  				if (!intent.isNull())
	    				stream.streamGetDict()->add(qstrdup("Intent"), &intent);
				}
  			}
  		}
  		
  		xref->releaseObj(&icc_stream);
  	}
  	
  	if (colorspace.isNull()) 
  	{
    	switch (colortype) 
    	{
    		case PDF_COLORSPACE_TYPE_GRAY:
      			colorspace.initName("DeviceGray");
      			break;
      			
    		case PDF_COLORSPACE_TYPE_RGB:
      			colorspace.initName("DeviceRGB");
      			break;
      			
    		case PDF_COLORSPACE_TYPE_CMYK:
      			colorspace.initName("DeviceCMYK");
      			break;
    	}
  	}
  	
  	stream.streamGetDict()->add(qstrdup("ColorSpace"), &colorspace);
  	

#define IS_ADOBE_CMYK(j) (((j).flags & HAVE_APPn_ADOBE) && (j).num_components == 4)

  	if (IS_ADOBE_CMYK(j_info))
  	{
  		XWObject decode;
  		decode.initArray(xref);
  		
  		for (int i = 0; i < j_info.num_components; i++) 
  		{
  			obj.initReal(1.0);
  			decode.arrayAdd(&obj);
  			obj.initReal(0.0);
  			decode.arrayAdd(&obj);
  			obj.initReal(1.0);
      		decode.arrayAdd(&obj);
    	}
    	stream.streamGetDict()->add(qstrdup("Decode"), &decode);
  	}
  	
  	JPEG_copy_stream(&j_info, &stream, fp, 0);

  	XWDVIImageInfo infoA;
  	infoA.width              = j_info.width;
  	infoA.height             = j_info.height;
  	infoA.bits_per_component = j_info.bits_per_component;
  	infoA.num_components     = j_info.num_components;

  	jpeg_get_density(&j_info, &infoA.xdensity, &infoA.ydensity);

  	setImage(xref, &infoA, &stream);
  	JPEG_info_clear(&j_info);

  	return 0;
}

