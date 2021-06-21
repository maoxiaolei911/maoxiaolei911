/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWNumberUtil.h"
#include "XWObject.h"
#include "XWPNG.h"


XWObject *
make_param_Cal(XWRef * xref, 
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
		       XWObject *cal_param)
{
  	double Xw, Yw, Zw; /* Yw = 1.0 */
  	double Xr, Xg, Xb, Yr, Yb, Yg, Zr, Zg, Zb;

#ifndef ABS
#define ABS(x) ((x) < 0 ? -(x) : (x))
#endif
  	{
    	double zw, zr, zg, zb;
    	double fr, fg, fb;
    	double det;

    	/* WhitePoint */
    	zw = 1 - (xw + yw);
    	zr = 1 - (xr + yr); zg = 1 - (xg + yg); zb = 1 - (xb + yb);
    	Xw = xw / yw; Yw = 1.0; Zw = zw / yw;

    	/* Matrix */
    	det = xr * (yg * zb - zg * yb) - xg * (yr * zb - zr * yb) + xb * (yr * zg - zr * yg);
    	if (ABS(det) < 1.0e-10) 
      		return 0;
      		
    	fr  = (Xw * (yg * zb - zg * yb) - xg * (zb - Zw * yb) + xb * (zg - Zw * yg)) / det;
    	fg  = (xr * (zb - Zw * yb) - Xw * (yr * zb - zr * yb) + xb * (yr * Zw - zr)) / det;
    	fb  = (xr * (yg * Zw - zg) - xg * (yr * Zw - zr) + Xw * (yr * zg - zr * yg)) / det;
    	Xr = fr * xr; Yr = fr * yr; Zr = fr * zr;
    	Xg = fg * xg; Yg = fg * yg; Zg = fg * zg;
    	Xb = fb * xb; Yb = fb * yb; Zb = fb * zb;
  	}

  	if (G < 1.0e-2) 
    	return 0;

  	XWObject white_point, obj;
  	cal_param->initDict(xref);

  	/* White point is always required. */
  	white_point.initArray(xref);
  	obj.initReal(round(Xw, 0.00001));
  	white_point.arrayAdd(&obj);
  	obj.initReal(round(Yw, 0.00001));
  	white_point.arrayAdd(&obj);
  	obj.initReal(round(Zw, 0.00001));
  	white_point.arrayAdd(&obj);  	
  	cal_param->dictAdd(qstrdup("WhitePoint"), &white_point);

  	/* Matrix - default: Identity */ 
  	if (color_type & PNG_COLOR_MASK_COLOR) 
  	{
    	if (G != 1.0) 
    	{
    		XWObject dev_gamma;
      		dev_gamma.initArray(xref);
      		obj.initReal(round(G, 0.00001));
      		dev_gamma.arrayAdd(&obj);
      		obj.initReal(round(G, 0.00001));
      		dev_gamma.arrayAdd(&obj);
      		obj.initReal(round(G, 0.00001));
      		dev_gamma.arrayAdd(&obj);
      		cal_param->dictAdd(qstrdup("Gamma"), &dev_gamma);
    	}

		XWObject matrix;
    	matrix.initArray(xref);
    	obj.initReal(round(Xr, 0.00001));
    	matrix.arrayAdd(&obj);
    	obj.initReal(round(Yr, 0.00001));
    	matrix.arrayAdd(&obj);
    	obj.initReal(round(Zr, 0.00001));
    	matrix.arrayAdd(&obj);
    	obj.initReal(round(Xg, 0.00001));
    	matrix.arrayAdd(&obj);
    	obj.initReal(round(Yg, 0.00001));
    	matrix.arrayAdd(&obj);
    	obj.initReal(round(Zg, 0.00001));
    	matrix.arrayAdd(&obj);
    	obj.initReal(round(Xb, 0.00001));
    	matrix.arrayAdd(&obj);
    	obj.initReal(round(Yb, 0.00001));
    	matrix.arrayAdd(&obj);
    	obj.initReal(round(Zb, 0.00001));
    	matrix.arrayAdd(&obj);
    	cal_param->dictAdd(qstrdup("Matrix"), &matrix);
  	} 
  	else 
  	{ /* Gray */
    	if (G != 1.0)
    	{
    		obj.initReal(round(G, 0.00001));
    		cal_param->dictAdd(qstrdup("Gamma"), &obj);
		}
  	}

  	return cal_param;
}

int
check_transparency(png_structp png_ptr, png_infop info_ptr, int pdf_version)
{
	int trans_type = PDF_TRANS_TYPE_NONE;
	png_color_16p trans_values;
  	png_bytep     trans;
  	int           num_trans;
  	
  	png_byte color_type = png_get_color_type(png_ptr, info_ptr);
  	if (color_type == PNG_COLOR_TYPE_RGB_ALPHA || 
  		color_type == PNG_COLOR_TYPE_GRAY_ALPHA) 
  	{
    	trans_type = PDF_TRANS_TYPE_ALPHA;
  	}
  	else if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS) && 
  		     png_get_tRNS(png_ptr, info_ptr, &trans, &num_trans, &trans_values))
	{
		switch (color_type)
		{
			case PNG_COLOR_TYPE_PALETTE:
      			trans_type = PDF_TRANS_TYPE_BINARY;
      			while (num_trans-- > 0) 
      			{
					if (trans[num_trans] != 0x00 && trans[num_trans] != 0xff) 
					{
	  					trans_type = PDF_TRANS_TYPE_ALPHA;
	  					break;
					}
      			}
      			break;
      			
    		case PNG_COLOR_TYPE_GRAY:
    		case PNG_COLOR_TYPE_RGB:
      			trans_type = PDF_TRANS_TYPE_BINARY;
      			break;
      			
    		default:
      			trans_type = PDF_TRANS_TYPE_NONE;
		}
	}
	
	if (( pdf_version < 3 && trans_type != PDF_TRANS_TYPE_NONE) || 
		( pdf_version < 4 && trans_type == PDF_TRANS_TYPE_ALPHA ))
	{
		png_color_16 bg;
    	bg.red = 255; bg.green = 255; bg.blue  = 255; bg.gray = 255; bg.index = 0;
    	png_set_background(png_ptr, &bg, PNG_BACKGROUND_GAMMA_SCREEN, 0, 1.0);
    	trans_type = PDF_TRANS_TYPE_NONE;
	}
	
	return trans_type;
}

XWObject *
get_rendering_intent (png_structp png_ptr, png_infop info_ptr, XWObject * intent)
{
	int      srgb_intent;
  	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_sRGB) && 
  		png_get_sRGB (png_ptr, info_ptr, &srgb_intent)) 
  	{
    	switch (srgb_intent) 
    	{
    		case PNG_sRGB_INTENT_SATURATION:
      			intent->initName("Saturation");
      			break;
      			
    		case PNG_sRGB_INTENT_PERCEPTUAL:
      			intent->initName("Perceptual");
      			break;
      			
    		case PNG_sRGB_INTENT_ABSOLUTE:
      			intent->initName("AbsoluteColorimetric");
      			break;
      			
    		case PNG_sRGB_INTENT_RELATIVE:
      			intent->initName("RelativeColorimetric");
      			break;
      			
    		default:
      			return 0;
      			break;
    	}
  	} 
  	else
    	return 0;

  	return intent;
}

XWObject *
create_cspace_sRGB(XWRef * xref, png_structp png_ptr, png_infop info_ptr, XWObject *colorspace)
{
	png_byte color_type = png_get_color_type(png_ptr, info_ptr);
	XWObject cal_param;
	if (!make_param_Cal(xref, color_type, 2.2, 0.3127, 0.329,
			       0.64, 0.33, 0.3, 0.6, 0.15, 0.06, &cal_param))
    	return 0;
    	
    colorspace->initArray(xref);
	XWObject obj;
  	switch (color_type) 
  	{
  		case PNG_COLOR_TYPE_RGB:
  		case PNG_COLOR_TYPE_RGB_ALPHA:
  		case PNG_COLOR_TYPE_PALETTE:
  			obj.initName("CalRGB");
    		colorspace->arrayAdd(&obj);
    		break;
    		
  		case PNG_COLOR_TYPE_GRAY:
  		case PNG_COLOR_TYPE_GRAY_ALPHA:
  			obj.initName("CalGray");
    		colorspace->arrayAdd(&obj);
    		break;
  	}
  	colorspace->arrayAdd(&cal_param);

  	return colorspace;
}

XWObject *
create_cspace_CalRGB(XWRef * xref, 
                     png_structp png_ptr, 
                     png_infop info_ptr, 
                     XWObject *colorspace)
{
	colorspace->initNull();
	double   xw, yw, xr, yr, xg, yg, xb, yb;
	if (!png_get_valid(png_ptr, info_ptr, PNG_INFO_cHRM) || 
		!png_get_cHRM(png_ptr, info_ptr, &xw, &yw, &xr, &yr, &xg, &yg, &xb, &yb))
    	return 0;
    	
    if (xw <= 0.0 || yw < 1.0e-10 ||
      	xr < 0.0  || yr < 0.0 || xg < 0.0 || yg < 0.0 || xb < 0.0 || yb < 0.0) 
    {
    	return 0;
  	}
  	
  	double G = 1.0;
  	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_gAMA) && 
  		png_get_gAMA (png_ptr, info_ptr, &G)) 
  	{
    	if (G < 1.0e-2) 
      		return 0;
      		
    	G = 1.0 / G; /* Gamma is inverted. */
  	}
  	
  	XWObject cal_param;
  	if (!make_param_Cal(xref, PNG_COLOR_TYPE_RGB, G, xw, yw, xr, yr, xg, yg, xb, yb, &cal_param))
  		return 0;
  		
  	colorspace->initArray(xref);
  	XWObject obj;
  	obj.initName("CalRGB");
  	colorspace->arrayAdd(&obj);
  	colorspace->arrayAdd(&cal_param);

  	return colorspace;
}

XWObject *
create_cspace_CalGray(XWRef * xref, 
                      png_structp png_ptr, 
                      png_infop info_ptr, 
                      XWObject * colorspace)
{
	colorspace->initNull();
	double   xw, yw, xr, yr, xg, yg, xb, yb;
	if (!png_get_valid(png_ptr, info_ptr, PNG_INFO_cHRM) || 
		!png_get_cHRM(png_ptr, info_ptr, &xw, &yw, &xr, &yr, &xg, &yg, &xb, &yb))
    	return 0;

  	if (xw <= 0.0 || yw < 1.0e-10 || xr < 0.0 
  		|| yr < 0.0 || xg < 0.0 || yg < 0.0 || 
  		xb < 0.0 || yb < 0.0) 
  	{
    	return 0;
  	}
  	
  	double G = 1.0;  	
  	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_gAMA) &&
      	png_get_gAMA (png_ptr, info_ptr, &G)) 
    {
    	if (G < 1.0e-2) 
      		return 0;
    	G = 1.0 / G; /* Gamma is inverted. */
  	}
  	
  	XWObject cal_param;
  	if (!make_param_Cal(xref, PNG_COLOR_TYPE_GRAY, G, xw, yw, xr, yr, xg, yg, xb, yb, &cal_param))
  		return 0;
  		
  	colorspace->initArray(xref);
  	XWObject obj;
  	obj.initName("CalGray");
  	colorspace->arrayAdd(&obj);
  	colorspace->arrayAdd(&cal_param);

  	return colorspace;
}

XWObject *
create_ckey_mask(XWRef * xref, 
                 png_structp png_ptr, 
                 png_infop info_ptr, 
                 XWObject * colorkeys)
{
  	png_bytep trans;
  	int       num_trans;
  	png_color_16p colors;
  	
  	if (!png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS) || 
  		!png_get_tRNS(png_ptr, info_ptr, &trans, &num_trans, &colors)) 
  	{
    	return 0;
  	}

  	png_byte  color_type = png_get_color_type(png_ptr, info_ptr);
  	colorkeys->initArray(xref);
  	XWObject obj;
  	switch (color_type) 
  	{
  		case PNG_COLOR_TYPE_PALETTE:
    		for (int i = 0; i < num_trans; i++) 
    		{
      			if (trans[i] == 0x00) 
      			{
      				obj.initInt(i);
      				colorkeys->arrayAdd(&obj);
      				colorkeys->arrayAdd(&obj);
      			} 
    		}
    		break;
    		
  		case PNG_COLOR_TYPE_RGB:
  			obj.initReal(colors->red);
  			colorkeys->arrayAdd(&obj);
  			colorkeys->arrayAdd(&obj);
  			obj.initReal(colors->green);
  			colorkeys->arrayAdd(&obj);
  			colorkeys->arrayAdd(&obj);
  			obj.initReal(colors->blue);
  			colorkeys->arrayAdd(&obj);
  			colorkeys->arrayAdd(&obj);
    		break;
    		
  		case PNG_COLOR_TYPE_GRAY:
  			obj.initReal(colors->gray);
  			colorkeys->arrayAdd(&obj);
  			colorkeys->arrayAdd(&obj);
    		break;
    		
  		default:
    		colorkeys->free();
    		colorkeys->initNull();
    		break;
  	}

  	return colorkeys;
}

XWObject *
create_soft_mask(XWRef * xref, 
				 png_structp png_ptr, 
				 png_infop info_ptr,
		         png_bytep image_data_ptr, 
		         png_uint_32 width, 
		         png_uint_32 height,
		         XWObject * smask)
{
  	png_bytep   trans;
  	int         num_trans;
  	
  	if (!png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS) || 
  		!png_get_tRNS(png_ptr, info_ptr, &trans, &num_trans, NULL)) 
  	{
    	return 0;
  	}
  	
  	smask->initStream(STREAM_COMPRESS, xref);
  	XWObject obj;  	
  	png_bytep smask_data_ptr = (png_bytep)malloc(width*height * sizeof(png_byte));
  	obj.initName("XObjcect");
  	smask->streamGetDict()->add(qstrdup("Type"), &obj);
  	obj.initName("Image");
  	smask->streamGetDict()->add(qstrdup("Subtype"), &obj);
  	obj.initInt(width);
  	smask->streamGetDict()->add(qstrdup("Width"), &obj);
  	obj.initInt(height);
  	smask->streamGetDict()->add(qstrdup("Height"), &obj);
  	obj.initName("DeviceGray");
  	smask->streamGetDict()->add(qstrdup("ColorSpace"), &obj);
  	obj.initInt(8);
  	smask->streamGetDict()->add(qstrdup("BitsPerComponent"), &obj);
  	
  	for (uint i = 0; i < width*height; i++) 
  	{
    	png_byte idx = image_data_ptr[i];
    	smask_data_ptr[i] = (idx < num_trans) ? trans[idx] : 0xff;
  	}
  	
  	smask->streamAdd((char *)smask_data_ptr, width*height);
  	free(smask_data_ptr);
  	return smask;
}

XWObject *
strip_soft_mask(XWRef * xref, 
                png_structp png_ptr, 
                png_infop info_ptr,
		        png_bytep image_data_ptr, 
		        png_uint_32p rowbytes_ptr,
		        png_uint_32 width, 
		        png_uint_32 height,
		        XWObject * smask)
{
	png_byte color_type = png_get_color_type(png_ptr, info_ptr);
	if (color_type & PNG_COLOR_MASK_COLOR) 
	{
    	if (*rowbytes_ptr != 4*width*sizeof(png_byte)) 
      		return 0;
  	} 
  	else 
  	{
    	if (*rowbytes_ptr != 2*width*sizeof(png_byte)) 
      		return 0;
  	}
  	
  	smask->initStream(STREAM_COMPRESS, xref);
  	XWObject obj;  	
  	obj.initName("XObjcect");
  	smask->streamGetDict()->add(qstrdup("Type"), &obj);
  	obj.initName("Image");
  	smask->streamGetDict()->add(qstrdup("Subtype"), &obj);
  	obj.initInt(width);
  	smask->streamGetDict()->add(qstrdup("Width"), &obj);
  	obj.initInt(height);
  	smask->streamGetDict()->add(qstrdup("Height"), &obj);
  	obj.initName("DeviceGray");
  	smask->streamGetDict()->add(qstrdup("ColorSpace"), &obj);
  	obj.initInt(8);
  	smask->streamGetDict()->add(qstrdup("BitsPerComponent"), &obj);

  	png_bytep smask_data_ptr = (png_bytep)malloc(width*height * sizeof(png_byte));
  	switch (color_type) 
  	{
  		case PNG_COLOR_TYPE_RGB_ALPHA:
    		for (uint i = 0; i < width*height; i++) 
    		{
      			memmove(image_data_ptr+(3*i), image_data_ptr+(4*i), 3);
      			smask_data_ptr[i] = image_data_ptr[4*i+3];
    		}
    		*rowbytes_ptr = 3*width*sizeof(png_byte);
    		break;
    		
  		case PNG_COLOR_TYPE_GRAY_ALPHA:
    		for (uint i = 0; i < width*height; i++) 
    		{
      			image_data_ptr[i] = image_data_ptr[2*i];
      			smask_data_ptr[i] = image_data_ptr[2*i+1];
    		}
    		*rowbytes_ptr = width*sizeof(png_byte);
    		break;
    		
  		default:
    		smask->free();
    		smask->initNull();
    		free(smask_data_ptr);
    		return 0;
  	}

  	smask->streamAdd((char*)smask_data_ptr, width*height);
  	free(smask_data_ptr);
  	return smask;
}

void
read_image_data(png_structp png_ptr, 
                png_infop ,
		 		png_bytep dest_ptr, 
		 		png_uint_32 height, 
		 		png_uint_32 rowbytes)
{
	png_bytepp rows_p = (png_bytepp)malloc(height * sizeof(png_bytep));
  	for (uint i=0; i< height; i++)
    	rows_p[i] = dest_ptr + (rowbytes * i);
  	png_read_image(png_ptr, rows_p);
  	free(rows_p);
}
