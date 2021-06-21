/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPNG_H
#define XWPNG_H

#include <png.h>
#include "XWGlobal.h"

class XWObject;
class XWRef;

#define PDF_TRANS_TYPE_NONE   0
#define PDF_TRANS_TYPE_BINARY 1
#define PDF_TRANS_TYPE_ALPHA  2

XW_DVI_EXPORT XWObject * make_param_Cal(XWRef * xref, 
               png_byte color_type,
		       double G, /* Gamma */
		       double xw, 
		       double yw,
		       double xr, 
		       double yr, 
		       double xg, 
		       double yg, 
		       double xb, 
		       double yb,
		       XWObject *cal_param);

XW_DVI_EXPORT int check_transparency(png_structp png_ptr, 
                       png_infop info_ptr, 
                       int pdf_version);

XW_DVI_EXPORT XWObject * get_rendering_intent (png_structp png_ptr, 
                                 png_infop info_ptr, 
                                 XWObject * intent);

XW_DVI_EXPORT XWObject * create_cspace_sRGB(XWRef * xref, 
                              png_structp png_ptr, 
                              png_infop info_ptr, 
                              XWObject *colorspace);

XW_DVI_EXPORT XWObject * create_cspace_CalRGB(XWRef * xref, 
                     png_structp png_ptr, 
                     png_infop info_ptr, 
                     XWObject *colorspace);

XW_DVI_EXPORT XWObject * create_cspace_CalGray(XWRef * xref, 
                      png_structp png_ptr, 
                      png_infop info_ptr, 
                      XWObject * colorspace);

XW_DVI_EXPORT XWObject * create_ckey_mask(XWRef * xref, 
                 png_structp png_ptr, 
                 png_infop info_ptr, 
                 XWObject * colorkeys);

XW_DVI_EXPORT XWObject * create_soft_mask(XWRef * xref, 
				 png_structp png_ptr, 
				 png_infop info_ptr,
		         png_bytep image_data_ptr, 
		         png_uint_32 width, 
		         png_uint_32 height,
		         XWObject * smask);

XW_DVI_EXPORT XWObject * strip_soft_mask(XWRef * xref, 
                png_structp png_ptr, 
                png_infop info_ptr,
		        png_bytep image_data_ptr, 
		        png_uint_32p rowbytes_ptr,
		        png_uint_32 width, 
		        png_uint_32 height,
		        XWObject * smask);

XW_DVI_EXPORT void read_image_data(png_structp png_ptr, 
                png_infop ,
		 		png_bytep dest_ptr, 
		 		png_uint_32 height, 
		 		png_uint_32 rowbytes);

#endif //XWPNG_H
