/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <png.h>
#include <QByteArray>
#include <QFile>
#include <QString>
#include "XWNumberUtil.h"
#include "XWObject.h"
#include "XWPNG.h"
#include "XWDVIColorSpaces.h"
#include "XWDVIRef.h"
#include "XWDVICore.h"
#include "XWDVIDev.h"
#include "XWPNGImage.h"

static XWObject *
create_cspace_ICCBased(XWDVICore * core, 
                       png_structp png_ptr, 
                       png_infop info_ptr, 
                       XWObject *colorspace)
{
	png_charp  name;
	int        compression_type;
	png_bytep   profile;
	png_uint_32 proflen;
	colorspace->initNull();
	if (!png_get_valid(png_ptr, info_ptr, PNG_INFO_iCCP) ||
      	!png_get_iCCP(png_ptr, info_ptr, &name, &compression_type, (png_bytepp)(&profile), &proflen))
    	return 0;
    	
    png_byte color_type = png_get_color_type(png_ptr, info_ptr);
    int colortype = 0;
    if (color_type & PNG_COLOR_MASK_COLOR)
    	colortype = PDF_COLORSPACE_TYPE_RGB;
    else
    	colortype = PDF_COLORSPACE_TYPE_GRAY;
    	
    if (XWDVIColorSpaces::iccpCheckColorSpace(colortype, profile, proflen) < 0)
    	colorspace->initNull();
    else
    {
    	int csp_id = core->iccpLoadProfile(name, profile, proflen);
    	if (csp_id < 0) 
      		colorspace->initNull();
    	else 
      		core->getColorSpaceReference(csp_id, colorspace);
    }
    
    return colorspace;
}

#define INVALID_CHRM_VALUE(xw,yw,xr,yr,xg,yg,xb,yb) (\
  (xw) <= 0.0 || (yw) < 1.0e-10 || \
  (xr) < 0.0  || (yr) < 0.0 || (xg) < 0.0 || (yg) < 0.0 || \
  (xb) < 0.0  || (yb) < 0.0)


static XWObject *
create_cspace_Indexed(XWDVICore * core, 
                      XWDVIRef * xref, 
                      png_structp png_ptr, 
                      png_infop info_ptr, 
                      XWObject * colorspace)
{
	png_byte  *data_ptr;
  	png_colorp plte;
  	int        num_plte;
  	if (!png_get_valid(png_ptr, info_ptr, PNG_INFO_PLTE) ||
      	!png_get_PLTE(png_ptr, info_ptr, &plte, &num_plte)) 
    {
    	return 0;
  	}
  	
  	colorspace->initArray(xref);
  	XWObject obj, base;
  	obj.initName("Indexed");
  	colorspace->arrayAdd(&obj);
	base.initNull();
  	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_iCCP))
    	create_cspace_ICCBased(core, png_ptr, info_ptr, &base);
  	else 
  	{
    	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_sRGB))
      		create_cspace_sRGB(xref, png_ptr, info_ptr, &base);
    	else
      		create_cspace_CalRGB(xref, png_ptr, info_ptr, &base);
  	}

  	if (base.isNull())
    	base.initName("DeviceRGB");
    	
    colorspace->arrayAdd(&base);
    obj.initInt(num_plte-1);
  	colorspace->arrayAdd(&obj);
  	data_ptr = (png_byte*)malloc((num_plte*3) * sizeof(png_byte));
  	for (int i = 0; i < num_plte; i++) 
  	{
    	data_ptr[3*i]   = plte[i].red;
    	data_ptr[3*i+1] = plte[i].green;
    	data_ptr[3*i+2] = plte[i].blue;
  	}
  	
  	XWObject lookup;
  	lookup.initString(new XWString((char*)data_ptr, num_plte*3));
  	free(data_ptr);
  	colorspace->arrayAdd(&lookup);

  	return colorspace;
}

XWPNGImage::XWPNGImage(XWDVIRef * xref,
	                   const char *identA, 
	                   int  subtypeA,
	                   int  formatA,
	                   long page_noA, 	           
	                   const QString & filenameA,
	                   XWObject *dictA)
	:XWDVIImage(xref, identA, subtypeA, formatA, page_noA, filenameA, dictA)
{
}

int XWPNGImage::checkForPNG(QIODevice *fp)
{
	fp->seek(0);
	
	unsigned char sigbytes[4];
	if (fp->read ((char*)sigbytes, sizeof(sigbytes)) != sizeof(sigbytes) || 
      				(!png_check_sig(sigbytes, sizeof(sigbytes))))
    	return 0;
  	
  	return 1;
}

int XWPNGImage::getBBox(QIODevice *fp, 
	                   long *width, 
	                   long *height,
	                   double *xdensity, 
	                   double *ydensity)
{
	fp->seek(0);	
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop   png_info_ptr;
  	if (png_ptr == NULL || (png_info_ptr = png_create_info_struct (png_ptr)) == NULL) 
  	{
    	if (png_ptr)
      		png_destroy_read_struct(&png_ptr, NULL, NULL);
    	return -1;
  	}
  	
  	QFile * file = qobject_cast<QFile *>(fp);
  	QString fn = file->fileName();
  	QByteArray ba = QFile::encodeName(fn);
  		
  	FILE * png_file = fopen(ba.constData(), "rb");
  	png_init_io(png_ptr, png_file);
  	png_read_info(png_ptr, png_info_ptr);
  	*width      = png_get_image_width (png_ptr, png_info_ptr);
  	*height     = png_get_image_height(png_ptr, png_info_ptr);
  	png_uint_32 xppm       = png_get_x_pixels_per_meter(png_ptr, png_info_ptr);
  	png_uint_32 yppm       = png_get_y_pixels_per_meter(png_ptr, png_info_ptr);
  	
  	if (png_info_ptr)
    	png_destroy_info_struct(png_ptr, &png_info_ptr);
  	if (png_ptr)
    	png_destroy_read_struct(&png_ptr, NULL, NULL);

  	
    *xdensity = xppm ? 72.0 / 0.0254 / xppm : 1.0;
    *ydensity = yppm ? 72.0 / 0.0254 / yppm : 1.0;

	fclose(png_file);
  	return 0;
}

int XWPNGImage::load(XWDVICore * core, 
	                 XWDVIRef * xref, 
	                 QIODevice *fp)
{
	fp->seek(0);
	
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop   png_info_ptr;
  	if (png_ptr == NULL || (png_info_ptr = png_create_info_struct (png_ptr)) == NULL) 
  	{
    	if (png_ptr)
      		png_destroy_read_struct(&png_ptr, NULL, NULL);
    	return -1;
  	}
  	
  	QFile * file = qobject_cast<QFile *>(fp);
  	QString fn = file->fileName();
  	QByteArray ba = QFile::encodeName(fn);
  		
  	FILE * png_file = fopen(ba.constData(), "rb");
  	png_init_io(png_ptr, png_file);  	
  	png_read_info(png_ptr, png_info_ptr);
  	png_byte color_type = png_get_color_type(png_ptr, png_info_ptr);
  	png_uint_32 width   = png_get_image_width (png_ptr, png_info_ptr);
  	png_uint_32 height  = png_get_image_height(png_ptr, png_info_ptr);
  	png_byte bpc        = png_get_bit_depth   (png_ptr, png_info_ptr);
  	png_uint_32 xppm    = png_get_x_pixels_per_meter(png_ptr, png_info_ptr);
  	png_uint_32 yppm    = png_get_y_pixels_per_meter(png_ptr, png_info_ptr);
  	
  	if (bpc > 8) 
  	{
    	png_set_strip_16(png_ptr);
    	bpc = 8;
  	}
  	
  	int trans_type = check_transparency(png_ptr, png_info_ptr, xref->getVersion());
  	png_read_update_info(png_ptr, png_info_ptr);
  	png_uint_32 rowbytes = png_get_rowbytes(png_ptr, png_info_ptr);
  	
  	XWDVIImageInfo info;
  	info.width  = width;
  	info.height = height;
  	info.bits_per_component = bpc;
  	
  	if (xppm > 0)
      	info.xdensity = 72.0 / 0.0254 / xppm;
    if (yppm > 0)
      	info.ydensity = 72.0 / 0.0254 / yppm;
      	
    XWObject stream;
    stream.initStream(STREAM_COMPRESS, xref);
    
    png_bytep stream_data_ptr = (png_bytep) malloc(rowbytes*height * sizeof(png_byte));
  	read_image_data(png_ptr, png_info_ptr, stream_data_ptr, height, rowbytes);
  	
  	XWObject intent;
  	if (get_rendering_intent(png_ptr, png_info_ptr, &intent))
    	stream.streamGetDict()->add(qstrdup("Intent"), &intent);
    	
    XWObject colorspace, mask;
    colorspace.initNull();
    mask.initNull();
    switch (color_type)
    {
    	case PNG_COLOR_TYPE_PALETTE:
    		 create_cspace_Indexed(core, xref, png_ptr, png_info_ptr, &colorspace);
    		 switch (trans_type)
    		 {
    		 	case PDF_TRANS_TYPE_BINARY:
      				create_ckey_mask(xref, png_ptr, png_info_ptr, &mask);
      				break;
      				
    			case PDF_TRANS_TYPE_ALPHA:
      				create_soft_mask(xref, png_ptr, png_info_ptr, stream_data_ptr, width, height, &mask);
      				break;
      				
    			default:
      				break;
    		 }
    		 break;
    		 
    	case PNG_COLOR_TYPE_RGB:
  		case PNG_COLOR_TYPE_RGB_ALPHA:
    		if (png_get_valid(png_ptr, png_info_ptr, PNG_INFO_iCCP))
      			 create_cspace_ICCBased(core, png_ptr, png_info_ptr, &colorspace);
    		else if (!intent.isNull()) 
      			create_cspace_sRGB(xref, png_ptr, png_info_ptr, &colorspace);
    		else 
      			create_cspace_CalRGB(xref, png_ptr, png_info_ptr, &colorspace);    		
    		if (colorspace.isNull())
      			colorspace.initName("DeviceRGB");

    		switch (trans_type) 
    		{
    			case PDF_TRANS_TYPE_BINARY:
      				create_ckey_mask(xref, png_ptr, png_info_ptr, &mask);
      				break;
      				
    			case PDF_TRANS_TYPE_ALPHA:
      				strip_soft_mask(xref, png_ptr, png_info_ptr, stream_data_ptr, &rowbytes, width, height, &mask);
      				break;
      				
    			default:
      				break;
    		}
    		info.num_components = 3;
    		break;
    		
    	case PNG_COLOR_TYPE_GRAY:
  		case PNG_COLOR_TYPE_GRAY_ALPHA:
    		if (png_get_valid(png_ptr, png_info_ptr, PNG_INFO_iCCP))
      			 create_cspace_ICCBased(core, png_ptr, png_info_ptr, &colorspace);
    		else if (!intent.isNull()) 
      			create_cspace_sRGB(xref, png_ptr, png_info_ptr, &colorspace);
    		else 
      			create_cspace_CalGray(xref, png_ptr, png_info_ptr, &colorspace);
    		if (colorspace.isNull())
      			colorspace.initName("DeviceGray");

    		switch (trans_type) 
    		{
    			case PDF_TRANS_TYPE_BINARY:
      				create_ckey_mask(xref, png_ptr, png_info_ptr, &mask);
      				break;
      				
    			case PDF_TRANS_TYPE_ALPHA:
      				strip_soft_mask(xref, png_ptr, png_info_ptr, stream_data_ptr, &rowbytes, width, height, &mask);
      				break;
      				
    			default:
      				break;
    		}
    		info.num_components = 1;
    		break;

  		default:
    		break;
    }
    
    stream.streamGetDict()->add(qstrdup("ColorSpace"), &colorspace);

  	stream.streamAdd((const char*)stream_data_ptr, rowbytes*height);
  	free(stream_data_ptr);
  	
  	XWObject obj;
  	if (!mask.isNull()) 
  	{
    	if (trans_type == PDF_TRANS_TYPE_BINARY)
      		stream.streamGetDict()->add(qstrdup("Mask"), &mask);
    	else if (trans_type == PDF_TRANS_TYPE_ALPHA) 
    	{
    		xref->refObj(&mask, &obj);
      		stream.streamGetDict()->add(qstrdup("SMask"), &obj);
      		xref->releaseObj(&mask);
    	} 
    	else 
      		mask.free();
  	}

  	png_read_end(png_ptr, NULL);

  	/* Cleanup */
  	if (png_info_ptr)
    	png_destroy_info_struct(png_ptr, &png_info_ptr);
  	if (png_ptr)
    	png_destroy_read_struct(&png_ptr, NULL, NULL);

	fclose(png_file);
  	setImage(xref, &info, &stream);

  	return 0;
}

