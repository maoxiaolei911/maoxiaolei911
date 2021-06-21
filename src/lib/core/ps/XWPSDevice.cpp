/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <string.h>
#include <stdio.h>
#include "XWApplication.h"
#include "psbittable.h"
#include "psscantable.h"
#include "XWPSError.h"
#include "XWPSBitmap.h"
#include "XWPSParam.h"
#include "XWPSPath.h"
#include "XWPSColor.h"
#include "XWPSColorSpace.h"
#include "XWPSComposite.h"
#include "XWPSContextState.h"
#include "XWPSStream.h"
#include "XWPSText.h"
#include "XWPSState.h"
#include "XWPSStreamState.h"
#include "XWPSCosObject.h"
#include "XWPSDeviceBbox.h"
#include "XWPSDeviceClip.h"
#include "XWPSDeviceMem.h"
#include "XWPSDevicePDF.h"
#include "XWPSDeviceDVI.h"
#include "XWPSDevice.h"

#define LINE_ACCUM_COPY(line, bpp, xo, xe, raster, y)\
	if ( (xe) > l_xprev ) {\
	    int code;\
	    LINE_ACCUM_STORE(bpp);\
	    code = copyColor\
	      (line, l_xprev - (xo), raster,\
	       ps_no_bitmap_id, l_xprev, y, (xe) - l_xprev, 1);\
	    if ( code < 0 )\
	      return code;\
	}

#define FILL_SCAN_LINES
#define FILL_CURVES
#define FILL_TRAPEZOIDS

typedef int (XWPSDevice::*stroke_line_proc_t)(XWPSPath * ,
	                                            int,
	                                            XWPSPartialLine * ,
	                                            XWPSPartialLine * ,
	                                            XWPSDeviceColor * ,
	                                            XWPSImagerState * ,
	                                            XWPSStrokeParams * ,
	                                            XWPSFixedRect * ,
	                                            int ,
	                                            PSLineJoin);


#define OPTIMIZE_ORIENTATION

#define FILL_STROKE_PATH(thin)\
  if(to_path==&stroke_path_body && !stroke_path_body.isVoid()) {\
    fill_params.adjust.x = STROKE_ADJUSTMENT(thin, pis, x);\
    fill_params.adjust.y = STROKE_ADJUSTMENT(thin, pis, y);\
    code = fillPath(pis, to_path, &fill_params, pdevc, NULL);\
    if ( code < 0 ) goto exit;\
  }


static const char *const pcmsa[] =
{
    "", "DeviceGray", "", "DeviceRGB", "DeviceCMYK"
};

XWPSDevice::XWPSDevice(QObject * parent)
	:QObject(parent),
	 refCount(1)
{
	params_size = 0;
	
	procs.open__.open_ = &XWPSDevice::openDefault;
	procs.get_initial_matrix_.get_initial_matrix = &XWPSDevice::getInitialMatrixDefault;
	procs.sync_output_.sync_output = &XWPSDevice::syncOutputDefault;
	procs.output_page_.output_page = &XWPSDevice::outputPageDefault;
	procs.close__.close_ = &XWPSDevice::closeDefault;
	procs.map_rgb_color_.map_rgb_color = &XWPSDevice::mapRGBColorDefault;
	procs.map_color_rgb_.map_color_rgb = &XWPSDevice::mapColorRGBDefault;
	procs.fill_rectangle_.fill_rectangle = &XWPSDevice::fillRectangleNo;
	procs.tile_rectangle_.tile_rectangle = &XWPSDevice::tileRectangleDefault;
	procs.copy_mono_.copy_mono = &XWPSDevice::copyMonoDefault;
	procs.copy_color_.copy_color = &XWPSDevice::copyColorDefault;
	procs.draw_line_.draw_line = &XWPSDevice::drawLineDefault;
	procs.get_bits_.get_bits = &XWPSDevice::getBitsDefault;
	procs.get_params_.get_params = &XWPSDevice::getParamsDefault;
	procs.put_params_.put_params = &XWPSDevice::putParamsDefault;
	procs.map_cmyk_color_.map_cmyk_color = &XWPSDevice::mapCMYKColorDefault;
	procs.get_xfont_device_.get_xfont_device = &XWPSDevice::getXFontDeviceDefault;
	procs.map_rgb_alpha_color_.map_rgb_alpha_color = &XWPSDevice::mapRGBAlphaColorDefault;
	procs.get_page_device_.get_page_device = &XWPSDevice::getPageDeviceDefault;
	procs.get_alpha_bits_.get_alpha_bits = &XWPSDevice::getAlphaBitsDefault;
	procs.copy_alpha_.copy_alpha = &XWPSDevice::copyAlphaDefault;
	procs.get_band_.get_band = &XWPSDevice::getBandDefault;
	procs.copy_rop_.copy_rop = &XWPSDevice::copyRopNo;
	procs.fill_path_.fill_path = &XWPSDevice::fillPathDefault;
	procs.stroke_path_.stroke_path = &XWPSDevice::strokePathDefault;
	procs.fill_mask_.fill_mask = &XWPSDevice::fillMaskDefault;
	procs.fill_trapezoid_.fill_trapezoid = &XWPSDevice::fillTrapezoidDefault;
	procs.fill_parallelogram_.fill_parallelogram = &XWPSDevice::fillParallelogramDefault;
	procs.fill_triangle_.fill_triangle = &XWPSDevice::fillTriangleDefault;
	procs.draw_thin_line_.draw_thin_line = &XWPSDevice::drawThinLineDefault;
	procs.begin_image_.begin_image = &XWPSDevice::beginImageDefault;
	procs.image_data_.image_data = &XWPSDevice::imageDataDefault;
	procs.end_image_.end_image = &XWPSDevice::endImageDefault;
	procs.strip_tile_rectangle_.strip_tile_rectangle = &XWPSDevice::stripTileRectangleDefault;
	procs.strip_copy_rop_.strip_copy_rop = &XWPSDevice::stripCopyRopNo;
	procs.get_clipping_box_.get_clipping_box = &XWPSDevice::getClipingBoxDefault;
	procs.begin_typed_image_.begin_typed_image = &XWPSDevice::beginTypedImageDefault;
	procs.get_bits_rectangle_.get_bits_rectangle = &XWPSDevice::getBitsRectangleDefault;
	procs.map_color_rgb_alpha_.map_color_rgb_alpha = &XWPSDevice::mapColorRGBAlphaDefault;
	procs.create_compositor_.create_compositor = &XWPSDevice::createCompositorDefault;
	procs.get_hardware_params_.get_hardware_params = &XWPSDevice::getHardwareParamsDefault;
	procs.text_begin_.text_begin = &XWPSDevice::textBeginDefault;
		
	page_procs.install = &XWPSDevice::installDefault;
	page_procs.begin_page = &XWPSDevice::beginPageDefault;
	page_procs.end_page = &XWPSDevice::endPageDefault;
	
	dname = 0;
	retained = false;
	is_open = false;
	max_fill_band = 0;
	color_info.num_components = 1;
	color_info.depth = 1;
	color_info.max_gray = 1;
	color_info.max_color = 0;
	color_info.dither_grays = 2;
	color_info.dither_colors = 0;
	cached_colors.black = ps_no_color_index;
	cached_colors.white = ps_no_color_index;
	width = 1;
	height = 1;
	MediaSize[0] = 0;
	MediaSize[1] = 0;
	ImagingBBox[0] = 0.0;
	ImagingBBox[1] = 0.0;
	ImagingBBox[2] = 0.0;
	ImagingBBox[3] = 0.0;
	ImagingBBox_set = false;
	HWResolution[0] = 72;
	HWResolution[1] = 72;
	MarginsHWResolution[0] = 72;
	MarginsHWResolution[1] = 72;
	Margins[0] = 0.0;
	Margins[1] = 0.0;
	HWMargins[0] = 0.0;
	HWMargins[1] = 0.0;
	HWMargins[2] = 0.0;
	HWMargins[3] = 0.0;
	PageCount = 0;
	ShowpageCount = 0;
	NumCopies = 1;
	NumCopies_set = false;
	IgnoreNumCopies = false;
	UseCIEColor = false;
	context_state = 0;
}

XWPSDevice::XWPSDevice(const char * devname,
	                     int w,
	                     int h,
	                     float xdpi,
	                     float ydpi,
	                     int nc,
	                     int depth,
	                     ushort mg, 
	                     ushort mc,
	                     ushort dg,
	                     ushort dc,
	                     QObject * parent)
	:QObject(parent),
	 refCount(1)
{
	params_size = 0;
	
	procs.open__.open_ = &XWPSDevice::openDefault;
	procs.get_initial_matrix_.get_initial_matrix = &XWPSDevice::getInitialMatrixDefault;
	procs.sync_output_.sync_output = &XWPSDevice::syncOutputDefault;
	procs.output_page_.output_page = &XWPSDevice::outputPageDefault;
	procs.close__.close_ = &XWPSDevice::closeDefault;
	procs.map_rgb_color_.map_rgb_color = &XWPSDevice::mapRGBColorDefault;
	procs.map_color_rgb_.map_color_rgb = &XWPSDevice::mapColorRGBDefault;
	procs.fill_rectangle_.fill_rectangle = &XWPSDevice::fillRectangleNo;
	procs.tile_rectangle_.tile_rectangle = &XWPSDevice::tileRectangleDefault;
	procs.copy_mono_.copy_mono = &XWPSDevice::copyMonoDefault;
	procs.copy_color_.copy_color = &XWPSDevice::copyColorDefault;
	procs.draw_line_.draw_line = &XWPSDevice::drawLineDefault;
	procs.get_bits_.get_bits = &XWPSDevice::getBitsDefault;
	procs.get_params_.get_params = &XWPSDevice::getParamsDefault;
	procs.put_params_.put_params = &XWPSDevice::putParamsDefault;
	procs.map_cmyk_color_.map_cmyk_color = &XWPSDevice::mapCMYKColorDefault;
	procs.get_xfont_device_.get_xfont_device = &XWPSDevice::getXFontDeviceDefault;
	procs.map_rgb_alpha_color_.map_rgb_alpha_color = &XWPSDevice::mapRGBAlphaColorDefault;
	procs.get_page_device_.get_page_device = &XWPSDevice::getPageDeviceDefault;
	procs.get_alpha_bits_.get_alpha_bits = &XWPSDevice::getAlphaBitsDefault;
	procs.copy_alpha_.copy_alpha = &XWPSDevice::copyAlphaDefault;
	procs.get_band_.get_band = &XWPSDevice::getBandDefault;
	procs.copy_rop_.copy_rop = &XWPSDevice::copyRopNo;
	procs.fill_path_.fill_path = &XWPSDevice::fillPathDefault;
	procs.stroke_path_.stroke_path = &XWPSDevice::strokePathDefault;
	procs.fill_mask_.fill_mask = &XWPSDevice::fillMaskDefault;
	procs.fill_trapezoid_.fill_trapezoid = &XWPSDevice::fillTrapezoidDefault;
	procs.fill_parallelogram_.fill_parallelogram = &XWPSDevice::fillParallelogramDefault;
	procs.fill_triangle_.fill_triangle = &XWPSDevice::fillTriangleDefault;
	procs.draw_thin_line_.draw_thin_line = &XWPSDevice::drawThinLineDefault;
	procs.begin_image_.begin_image = &XWPSDevice::beginImageDefault;
	procs.image_data_.image_data = &XWPSDevice::imageDataDefault;
	procs.end_image_.end_image = &XWPSDevice::endImageDefault;
	procs.strip_tile_rectangle_.strip_tile_rectangle = &XWPSDevice::stripTileRectangleDefault;
	procs.strip_copy_rop_.strip_copy_rop = &XWPSDevice::stripCopyRopNo;
	procs.get_clipping_box_.get_clipping_box = &XWPSDevice::getClipingBoxDefault;
	procs.begin_typed_image_.begin_typed_image = &XWPSDevice::beginTypedImageDefault;
	procs.get_bits_rectangle_.get_bits_rectangle = &XWPSDevice::getBitsRectangleDefault;
	procs.map_color_rgb_alpha_.map_color_rgb_alpha = &XWPSDevice::mapColorRGBAlphaDefault;
	procs.create_compositor_.create_compositor = &XWPSDevice::createCompositorDefault;
	procs.get_hardware_params_.get_hardware_params = &XWPSDevice::getHardwareParamsDefault;
	procs.text_begin_.text_begin = &XWPSDevice::textBeginDefault;
		
	page_procs.install = &XWPSDevice::installDefault;
	page_procs.begin_page = &XWPSDevice::beginPageDefault;
	page_procs.end_page = &XWPSDevice::endPageDefault;
	
	dname = devname;
	retained = false;
	is_open = false;
	max_fill_band = 0;
	color_info.num_components = nc;
	color_info.depth = depth;
	color_info.max_gray = mg;
	color_info.max_color = mc;
	color_info.dither_grays = dg;
	color_info.dither_colors = dc;
	cached_colors.black = ps_no_color_index;
	cached_colors.white = ps_no_color_index;
	width = w;
	height = h;
	MediaSize[0] = (((width) * 72.0 + 0.5) - 0.5) / (xdpi);
	MediaSize[1] = (((height) * 72.0 + 0.5) - 0.5) / (ydpi);
	ImagingBBox[0] = 0.0;
	ImagingBBox[1] = 0.0;
	ImagingBBox[2] = 0.0;
	ImagingBBox[3] = 0.0;
	ImagingBBox_set = false;
	HWResolution[0] = xdpi;
	HWResolution[1] = ydpi;
	MarginsHWResolution[0] = xdpi;
	MarginsHWResolution[1] = ydpi;
	Margins[0] = 0.0;
	Margins[1] = 0.0;
	HWMargins[0] = 0.0;
	HWMargins[1] = 0.0;
	HWMargins[2] = 0.0;
	HWMargins[3] = 0.0;
	PageCount = 0;
	ShowpageCount = 0;
	NumCopies = 1;
	NumCopies_set = false;
	IgnoreNumCopies = false;
	UseCIEColor = false;
	context_state = 0;
}

XWPSDevice::~XWPSDevice()
{
}

int XWPSDevice::beginImage(XWPSImagerState *pis, 
	                       XWPSImage1 *pim,
	                       PSImageFormat format, 
	                       XWPSIntRect *prect,
	                       XWPSDeviceColor *pdcolor, 
	                       XWPSClipPath *pcpath,
	                       XWPSImageEnumCommon **pinfo)
{
//	*pinfo = 0;
	return (this->*(procs.begin_image_.begin_image))(pis, pim, format, prect, pdcolor,pcpath, pinfo);
}

int XWPSDevice::beginImageDefault(XWPSImagerState *pis, 
	                       XWPSImage1 *pim,
	                       PSImageFormat format, 
	                       XWPSIntRect *prect,
	                       XWPSDeviceColor *pdcolor, 
	                       XWPSClipPath *pcpath,
	                       XWPSImageEnumCommon **pinfo)
{
	int (XWPSDevice::*save_begin_image)(XWPSImagerState *, 
	                               XWPSImage1 *,
	                               PSImageFormat, 
	                               XWPSIntRect *,
	                               XWPSDeviceColor *, 
	                               XWPSClipPath *,
	                               XWPSImageEnumCommon **) = procs.begin_image_.begin_image;
	                               
	XWPSImage1 image;
  XWPSImage1 *ptim;
  int code;
  
  procs.begin_image_.begin_image = &XWPSDevice::beginImageNo;
  if (pim->format == format)
		ptim = pim;
  else 
  {
		image = *pim;
		image.format = format;
		ptim = &image;
  }
  code = beginTypedImage(pis, NULL, ptim, prect, pdcolor, pcpath, pinfo);
  procs.begin_image_.begin_image = save_begin_image;
  return code;
}

int XWPSDevice::beginPage(XWPSState *pgs)
{
	return (this->*(page_procs.begin_page))(pgs);
}

int XWPSDevice::beginTypedImage(XWPSImagerState * pis,
			                        XWPSMatrix *pmat, 
			                        XWPSImageCommon *pic,
			   											XWPSIntRect * prect,
			                        XWPSDeviceColor * pdcolor,
			                        XWPSClipPath * pcpath, 
			                        XWPSImageEnumCommon ** pinfo)
{
//	*pinfo = 0;
	return (this->*(procs.begin_typed_image_.begin_typed_image))(pis, pmat, pic, prect, pdcolor,pcpath, pinfo);
}

int XWPSDevice::beginTypedImageDefault(XWPSImagerState * pis,
			                        XWPSMatrix *pmat, 
			                        XWPSImageCommon *pic,
			   											XWPSIntRect * prect,
			                        XWPSDeviceColor * pdcolor,
			                        XWPSClipPath * pcpath, 
			                        XWPSImageEnumCommon ** pinfo)
{
	if (pic->type.begin_typed_image_.begin_typed_image1 == &XWPSImage1::beginImage1)
	{
		XWPSMatrix ctm;
		(pis)->ctmOnly(&ctm);
		XWPSImage1 *pim = (XWPSImage1 *)pic;
		if (pmat == 0 || (pis != 0 && !memcmp(pmat, &ctm, sizeof(*pmat)))) 
		{
	    int code = beginImage(pis, pim, pim->format, prect, pdcolor, pcpath, pinfo);
	    if (code >= 0)
				return code;
		}
	}
	
	return pic->beginTypedImage(this, pis, pmat, prect, pdcolor, pcpath, pinfo);
}

ulong XWPSDevice::getBlack()
{
	if (cached_colors.black == ps_no_color_index)
		cached_colors.black = mapRGBColor(0, 0, 0);
		
	return cached_colors.black;
}

int XWPSDevice::close()
{
	int code = 0;
	if (is_open)
	{
		code = (this->*(procs.close__.close_))();
		if (code < 0)
	    return code;
		is_open = false;
	}
	
	return code;
}

int XWPSDevice::copyAlpha(const uchar *data, 
	                      int data_x,
	                      int raster, 
	                      ulong id, 
	                      int x, 
	                      int y, 
	                      int widthA, 
	                      int heightA,
	                      ulong color, 
	                      int depthA)
{
	return (this->*(procs.copy_alpha_.copy_alpha))(data, data_x, raster, id, x, y, widthA, heightA, color, depthA);
}

int XWPSDevice::copyAlphaDefault(const uchar *data, 
	                      int data_x,
	                      int raster, 
	                      ulong id, 
	                      int x, 
	                      int y, 
	                      int widthA, 
	                      int heightA,
	                      ulong color, 
	                      int depth)
{
	if (depth == 1)
		return copyMono(data, data_x, raster, id, x, y, widthA, heightA, ps_no_color_index, color);
		
	{
		const uchar *row;
		
		int bpp = color_info.depth;
		uint in_size = getRaster(false);
		uchar *lin;
		uint out_size;
		uchar *lout;
		int code = 0;
		ushort color_rgb[3];
		int ry;

		fit_copy(data, data_x, raster, id, x, y, widthA, heightA);
		row = data;
		out_size = bitmap_raster(widthA * bpp);
		lin = new uchar[in_size];
		lout = new uchar[out_size];
		mapColorRGB(color, color_rgb);
		for (ry = y; ry < y + heightA; row += raster, ++ry)
		{
			uchar *line;
	    int sx, rx;
	    
	    DECLARE_LINE_ACCUM_COPY(lout, bpp, x);

	    code = getBits(ry, lin, &line);
	    if (code < 0)
				break;
				
			for (sx = data_x, rx = x; sx < data_x + widthA; ++sx, ++rx)
			{
				ulong previous = ps_no_color_index;
				ulong composite;
				int alpha2, alpha;
				
				if (depth == 2)	
		    	alpha = ((row[sx >> 2] >> ((3 - (sx & 3)) << 1)) & 3) * 5;
				else
		    	alpha2 = row[sx >> 1],alpha = (sx & 1 ? alpha2 & 0xf : alpha2 >> 4);
		    		
blend:
				if (alpha == 15)
					composite = color;
				else
				{
					if (previous == ps_no_color_index)
					{
						if (bpp < 8) 
						{
			    		const uint bit = rx * bpp;
			    		const uchar *src = line + (bit >> 3);

			    		previous = (*src >> (8 - (bit + bpp))) & ((1 << bpp) - 1);
						}
						else
						{
							const uchar *src = line + (rx * (bpp >> 3));

			    		previous = 0;
			    		switch (bpp >> 3) 
			    		{
								case 4:
				    			previous += (ulong) * src++ << 24;
								case 3:
				    			previous += (ulong) * src++ << 16;
								case 2:
				    			previous += (ulong) * src++ << 8;
								case 1:
				    			previous += *src++;
			    		}
						}
					}
					
					if (alpha == 0)
						composite = previous;
					else
					{
						ushort rgb[3];
						mapColorRGB(previous, rgb);
#if arch_ints_are_short
#  define b_int long
#else
#  define b_int int
#endif
#define make_shade(old, clr, alpha, amax) \
  (old) + (((b_int)(clr) - (b_int)(old)) * (alpha) / (amax))
			rgb[0] = make_shade(rgb[0], color_rgb[0], alpha, 15);
			rgb[1] = make_shade(rgb[1], color_rgb[1], alpha, 15);
			rgb[2] = make_shade(rgb[2], color_rgb[2], alpha, 15);
#undef b_int
#undef make_shade
						composite = mapRGBColor(rgb[0], rgb[1], rgb[2]);
						if (composite == ps_no_color_index)
						{
							if (alpha == 7)
								++alpha;
			    		alpha = (alpha & 8) | (alpha >> 1);
			    		goto blend;
						}
					}
				}
				LINE_ACCUM(composite, bpp);
			}
			LINE_ACCUM_COPY(lout, bpp, x, rx, raster, ry);
		}
		
//out:
		if (lout)
			delete [] lout;
			
		if (lin)
			delete [] lin;
			
		return code;
	}
}

int XWPSDevice::copyColor(const uchar *data, 
	                      int dx, 
	                      int raster, 
	                      ulong id,
	                      int x, 
	                      int y, 
	                      int w, 
	                      int h)
{
	return (this->*(procs.copy_color_.copy_color))(data, dx, raster, id, x, y, w, h);
}

int XWPSDevice::copyColorDefault(const uchar *data, 
	                      int dx, 
	                      int raster, 
	                      ulong id,
	                      int x, 
	                      int y, 
	                      int w, 
	                      int h)
{
	int depth = color_info.depth;
  uchar mask;

  const uchar *row;
  int iy;

  if (depth == 1)
		return copyMono(data, dx, raster, id, x, y, w, h, 0, 1);
		
  fit_copy(data, dx, raster, id, x, y, w, h);
  mask = (uchar) ((1 << depth) - 1);
  for (row = data, iy = 0; iy < h; row += raster, ++iy) 
  {
		int ix;
		ulong c0 = ps_no_color_index;
		const uchar *ptr = row + ((dx * depth) >> 3);
		int i0;

		for (i0 = ix = 0; ix < w; ++ix) 
		{
	    ulong color;

	    if (depth >= 8) 
	    {
				color = *ptr++;
				switch (depth) 
				{
		    	case 32:
						color = (color << 8) + *ptr++;
		    	case 24:
						color = (color << 8) + *ptr++;
		    	case 16:
						color = (color << 8) + *ptr++;
				}
	    } 
	    else 
	    {
				uint dbit = (-(ix + dx + 1) * depth) & 7;

				color = (*ptr >> dbit) & mask;
				if (dbit == 0)
		    	ptr++;
	    }
	    if (color != c0) 
	    {
				if (ix > i0) 
				{
		    	int code = fillRectangle(i0 + x, iy + y, ix - i0, 1, c0);

		    	if (code < 0)
						return code;
				}
				c0 = color;
				i0 = ix;
	    }
		}
		if (ix > i0) 
		{
	    int code = fillRectangle(i0 + x, iy + y, ix - i0, 1, c0);

	    if (code < 0)
				return code;
		}
  }
  return 0;
}

void XWPSDevice::copyColorParams(XWPSDevice * proto)
{
	color_info = proto->color_info;
	cached_colors = proto->cached_colors;
	copyColorProcs(proto);
}

void XWPSDevice::copyColorProcs(XWPSDevice *targetA)
{
	ulong (XWPSDevice::*from_cmyk)(ushort, ushort, ushort, ushort) = procs.map_cmyk_color_.map_cmyk_color;
	ulong (XWPSDevice::*from_rgb)(ushort, ushort, ushort) = procs.map_rgb_color_.map_rgb_color;
	int (XWPSDevice::*to_rgb)(ulong color, ushort *) = procs.map_color_rgb_.map_color_rgb;
	
	if (procs.map_cmyk_color_.map_cmyk_colorfw == &XWPSDeviceForward::mapCMYKColorForward ||
			from_cmyk == &XWPSDevice::mapCMYKColorCMYK1Bit ||
			from_cmyk == &XWPSDevice::mapCMYKColorCMYK8Bit)
	{
		from_cmyk = targetA->procs.map_cmyk_color_.map_cmyk_color;
		if ((from_cmyk == &XWPSDevice::mapCMYKColorCMYK1Bit || from_cmyk == &XWPSDevice::mapCMYKColorCMYK8Bit))
			procs.map_cmyk_color_.map_cmyk_color = from_cmyk;
		else
			procs.map_cmyk_color_.map_cmyk_colorfw = &XWPSDeviceForward::mapCMYKColorForward;
	}
	
	if (procs.map_rgb_color_.map_rgb_colorfw == &XWPSDeviceForward::mapRGBColorForward ||
			from_rgb == &XWPSDevice::mapRGBColorRGBDefault) 
	{
		from_rgb = targetA->procs.map_rgb_color_.map_rgb_color;
		if (from_rgb == &XWPSDevice::mapRGBColorRGBDefault)
			procs.map_rgb_color_.map_rgb_color = from_rgb;
		else
			procs.map_rgb_color_.map_rgb_colorfw = &XWPSDeviceForward::mapRGBColorForward;
  }
  
  if (procs.map_color_rgb_.map_color_rgbfw == &XWPSDeviceForward::mapColorRGBForward ||
			to_rgb == &XWPSDevice::mapColorRGBCMYK1Bit ||
			procs.map_color_rgb_.map_color_rgbfw == &XWPSDevice::mapColorRGBCMYK8Bit) 
	{
		to_rgb = targetA->procs.map_color_rgb_.map_color_rgb;
		if (to_rgb == &XWPSDevice::mapColorRGBCMYK1Bit || to_rgb == &XWPSDevice::mapColorRGBCMYK8Bit)
			procs.map_color_rgb_.map_color_rgb = to_rgb;
		else
			procs.map_color_rgb_.map_color_rgbfw = &XWPSDeviceForward::mapColorRGBForward;
  }
}

int XWPSDevice::copyDevice(XWPSDevice **pnew)
{
	XWPSDevice * ret = new XWPSDevice(dname, width, height,HWResolution[0],HWResolution[1],color_info.num_components,color_info.depth,color_info.max_gray,color_info.max_color,color_info.dither_grays,color_info.dither_colors);
	ret->copyDeviceParam(this);
	*pnew = ret;
	return 0;
}

int  XWPSDevice::copyDevice2(XWPSDevice ** pnew_dev, 
					                 bool keep_open)
{
	int code = copyDevice(pnew_dev);
	if (code < 0)
		return code;
		
	if (keep_open)
		(*pnew_dev)->open();
		
	return 0;
}

void XWPSDevice::copyDeviceParam(XWPSDevice * proto)
{
	params_size = proto->params_size;
	dname = proto->dname;
	
	max_fill_band = proto->max_fill_band;
	color_info = proto->color_info;
	cached_colors = proto->cached_colors;
	width = proto->width;
	height = proto->height;
	MediaSize[0] = proto->MediaSize[0];
	MediaSize[1] = proto->MediaSize[1];
	for (int i = 0; i < 4; i++)
		ImagingBBox[i] = proto->ImagingBBox[i];
	ImagingBBox_set = proto->ImagingBBox_set;
	HWResolution[0] = proto->HWResolution[0];
	HWResolution[1] = proto->HWResolution[1];
	MarginsHWResolution[0] = proto->MarginsHWResolution[0];
	MarginsHWResolution[1] = proto->MarginsHWResolution[1];
	Margins[0] = proto->Margins[0];
	Margins[1] = proto->Margins[1];
	for (int i = 0; i < 4; i++)
		HWMargins[i] = proto->HWMargins[i];
		
	PageCount = proto->PageCount;
	ShowpageCount = proto->ShowpageCount;
	NumCopies = proto->NumCopies;
	NumCopies_set = proto->NumCopies_set;
	IgnoreNumCopies = proto->IgnoreNumCopies;
	UseCIEColor = proto->UseCIEColor;	
	context_state = proto->context_state;
}

void XWPSDevice::copyDeviceProc(XWPSDevice * proto)
{
	procs.open__.open_ = proto->procs.open__.open_;
	procs.get_initial_matrix_.get_initial_matrix = proto->procs.get_initial_matrix_.get_initial_matrix;
	procs.sync_output_.sync_output = proto->procs.sync_output_.sync_output;
	procs.output_page_.output_page = proto->procs.output_page_.output_page;
	procs.close__.close_ = proto->procs.close__.close_;
	procs.map_rgb_color_.map_rgb_color = proto->procs.map_rgb_color_.map_rgb_color;
	procs.map_color_rgb_.map_color_rgb = proto->procs.map_color_rgb_.map_color_rgb;
	procs.fill_rectangle_.fill_rectangle = proto->procs.fill_rectangle_.fill_rectangle;
	procs.tile_rectangle_.tile_rectangle = proto->procs.tile_rectangle_.tile_rectangle;
	procs.copy_mono_.copy_mono = proto->procs.copy_mono_.copy_mono;
	procs.copy_color_.copy_color = proto->procs.copy_color_.copy_color;
	procs.draw_line_.draw_line = proto->procs.draw_line_.draw_line;
	procs.get_bits_.get_bits = proto->procs.get_bits_.get_bits;
	procs.get_params_.get_params = proto->procs.get_params_.get_params;
	procs.put_params_.put_params = proto->procs.put_params_.put_params;
	procs.map_cmyk_color_.map_cmyk_color = proto->procs.map_cmyk_color_.map_cmyk_color;
	procs.get_xfont_device_.get_xfont_device = proto->procs.get_xfont_device_.get_xfont_device;
	procs.map_rgb_alpha_color_.map_rgb_alpha_color = proto->procs.map_rgb_alpha_color_.map_rgb_alpha_color;
	procs.get_page_device_.get_page_device = proto->procs.get_page_device_.get_page_device;
	procs.get_alpha_bits_.get_alpha_bits = proto->procs.get_alpha_bits_.get_alpha_bits;
	procs.copy_alpha_.copy_alpha = proto->procs.copy_alpha_.copy_alpha;
	procs.get_band_.get_band = proto->procs.get_band_.get_band;
	procs.copy_rop_.copy_rop = proto->procs.copy_rop_.copy_rop;
	procs.fill_path_.fill_path = proto->procs.fill_path_.fill_path;
	procs.stroke_path_.stroke_path = proto->procs.stroke_path_.stroke_path;
	procs.fill_mask_.fill_mask = proto->procs.fill_mask_.fill_mask;
	procs.fill_trapezoid_.fill_trapezoid = proto->procs.fill_trapezoid_.fill_trapezoid;
	procs.fill_parallelogram_.fill_parallelogram = proto->procs.fill_parallelogram_.fill_parallelogram;
	procs.fill_triangle_.fill_triangle = proto->procs.fill_triangle_.fill_triangle;
	procs.draw_thin_line_.draw_thin_line = proto->procs.draw_thin_line_.draw_thin_line;
	procs.begin_image_.begin_image = proto->procs.begin_image_.begin_image;
	procs.image_data_.image_data = proto->procs.image_data_.image_data;
	procs.end_image_.end_image = proto->procs.end_image_.end_image;
	procs.strip_tile_rectangle_.strip_tile_rectangle = proto->procs.strip_tile_rectangle_.strip_tile_rectangle;
	procs.strip_copy_rop_.strip_copy_rop = proto->procs.strip_copy_rop_.strip_copy_rop;
	procs.get_clipping_box_.get_clipping_box = proto->procs.get_clipping_box_.get_clipping_box;
	procs.begin_typed_image_.begin_typed_image = proto->procs.begin_typed_image_.begin_typed_image;
	procs.get_bits_rectangle_.get_bits_rectangle = proto->procs.get_bits_rectangle_.get_bits_rectangle;
	procs.map_color_rgb_alpha_.map_color_rgb_alpha = proto->procs.map_color_rgb_alpha_.map_color_rgb_alpha;
	procs.create_compositor_.create_compositor = proto->procs.create_compositor_.create_compositor;
	procs.get_hardware_params_.get_hardware_params = proto->procs.get_hardware_params_.get_hardware_params;
	procs.text_begin_.text_begin = proto->procs.text_begin_.text_begin;
	page_procs = proto->page_procs;
}

int XWPSDevice::copyMono(const uchar *data, 
	                     int dx, 
	                     int raster, 
	                     ulong id,
	                     int x, 
	                     int y, 
	                     int w, 
	                     int h,
	                     ulong zero, 
	                     ulong one)
{
	return (this->*(procs.copy_mono_.copy_mono))(data, dx, raster, id, x, y, w, h, zero, one);
}

int XWPSDevice::copyMonoUnaligned(const uchar * data,
	    								  int dx, 
	    								  int raster, 
	    								  ulong id, 
	    								  int x, 
	    								  int y, 
	    								  int w, 
	    								  int h,
		       							ulong zero, 
		       							ulong one)
{
	uint offset = ALIGNMENT_MOD(data, align_bitmap_mod);
  int step = raster & (align_bitmap_mod - 1);

  data -= offset;
  dx += offset << 3;

  if (!step) 
		return copyMono(data, dx, raster, id,  x, y, w, h, zero, one);
  
  {
		const uchar *p = data;
		int d = dx;
		int code = 0;
		int i;

		for (i = 0; i < h && code >= 0;  ++i, p += raster - step, d += step << 3 )
	    code = copyMono(p, d, raster, ps_no_bitmap_id, x, y + i, w, 1, zero, one);
		return code;
  }
}

int XWPSDevice::copyMonoDefault(const uchar *data, 
	                     int dx, 
	                     int raster, 
	                     ulong id,
	                     int x, 
	                     int y, 
	                     int w, 
	                     int h,
	                     ulong zero, 
	                     ulong one)
{
	bool invert;
  ulong color;
  XWPSDeviceColor devc;

  fit_copy(data, dx, raster, id, x, y, w, h);
  if (one != ps_no_color_index) 
  {
		invert = false;
		color = one;
		if (zero != ps_no_color_index) 
		{
	    int code = fillRectangle(x, y, w, h, zero);

	    if (code < 0)
				return code;
		}
  } 
  else 
  {
		invert = true;
		color = zero;
  }
  devc.setPure(color);
  return devc.defaultFillMaskedDC(data, dx, raster, id, x, y, w, h, this, rop3_T, invert);
}

void XWPSDevice::copyParams(XWPSDevice * proto)
{
	width = proto->width;
	height = proto->height;
	MediaSize[0] = proto->MediaSize[0];
	MediaSize[1] = proto->MediaSize[1];
	for (int i = 0; i < 4; i++)
		ImagingBBox[i] = proto->ImagingBBox[i];
	ImagingBBox_set = proto->ImagingBBox_set;
	HWResolution[0] = proto->HWResolution[0];
	HWResolution[1] = proto->HWResolution[1];
	MarginsHWResolution[0] = proto->MarginsHWResolution[0];
	MarginsHWResolution[1] = proto->MarginsHWResolution[1];
	Margins[0] = proto->Margins[0];
	Margins[1] = proto->Margins[1];
	for (int i = 0; i < 4; i++)
		HWMargins[i] = proto->HWMargins[i];
		
	PageCount = proto->PageCount;
	copyColorParams(proto);
}

int XWPSDevice::copyRop(const uchar * sdata, 
	                    int sourcex, 
	                    uint sraster, 
	                    ulong id,
		                  const ulong * scolors,
	                    XWPSTileBitmap * texture, 
	                    const ulong * tcolors,
		                  int x, 
		                  int y, 
		                  int widthA, 
		                  int heightA,
		                  int phase_x, 
		                  int phase_y, 
		                  ulong lop)
{
	return (this->*(procs.copy_rop_.copy_rop))(sdata, sourcex, sraster, id, scolors, texture, tcolors, x, y, widthA, heightA, phase_x, phase_y, lop);
}

int XWPSDevice::copyRopDefault(const uchar * sdata, 
	                    int sourcex, 
	                    uint sraster, 
	                    ulong id,
		                  const ulong * scolors,
	                    XWPSTileBitmap * texture, 
	                    const ulong * tcolors,
		                  int x, 
		                  int y, 
		                  int widthA, 
		                  int heightA,
		                  int phase_x, 
		                  int phase_y, 
		                  ulong lop)
{
	XWPSStripBitmap *textures;
  XWPSStripBitmap tiles;

  if (texture == 0)
		textures = 0;
  else 
  {
		tiles.data = texture->data;
		tiles.raster = texture->raster;
		tiles.size = texture->size;
		tiles.id = texture->id;
		tiles.rep_width = texture->rep_width;
		tiles.rep_height = texture->rep_height;
		textures = &tiles;
  }
  
  return stripCopyRop(sdata, sourcex, sraster, id, scolors, textures, tcolors, x, y, widthA, heightA, phase_x, phase_y, lop);
}

int XWPSDevice::copyRopNo(const uchar * , 
	                    int , 
	                    uint , 
	                    ulong ,
		                  const ulong * ,
	                    XWPSTileBitmap * , 
	                    const ulong * ,
		                  int , 
		                  int , 
		                  int , 
		                  int ,
		                  int , 
		                  int , 
		                  ulong )
{
	return (int)(XWPSError::Unknown);
}

void XWPSDevice::decacheColors()
{
	cached_colors.black = cached_colors.white = ps_no_color_index;
}

int XWPSDevice::createCompositor(XWPSDevice **pcdev, 
		                   XWPSComposite *pcte,
		                   XWPSImagerState *pis)
{
	return (this->*(procs.create_compositor_.create_compositor))(pcdev, pcte, pis);
}

int XWPSDevice::createCompositorDefault(XWPSDevice **pcdev, 
		                   XWPSComposite *pcte,
		                   XWPSImagerState *pis)
{
	return ((XWPSComposite*)pcte)->createDefaultCompositor(pcdev, this, pis);
}

int XWPSDevice::createCompositorNo(XWPSDevice **, 
		                   XWPSComposite *,
		                   XWPSImagerState *)
{
	return (int)(XWPSError::Unknown);
}

int XWPSDevice::createCompositorNull(XWPSDevice **pcdev, 
		                   XWPSComposite *,
		                   XWPSImagerState *)
{
	*pcdev = this;
  return 0;
}

int XWPSDevice::drawLine(int x0, 
	                     int y0, 
	                     int x1, 
	                     int y1, 
	                     ulong color)
{
	return (this->*(procs.draw_line_.draw_line))(x0, y0, x1, y1, color);
}

int XWPSDevice::drawThinLine(long fx0, 
                           long fy0, 
                           long fx1, 
                           long fy1,
                           XWPSDeviceColor *pdevc, 
                           ulong lop)
{
	return (this->*(procs.draw_thin_line_.draw_thin_line))(fx0, fy0, fx1, fy1, pdevc, lop);
}

int XWPSDevice::drawThinLineDefault(long fx0, 
                           long fy0, 
                           long fx1, 
                           long fy1,
                           XWPSDeviceColor *pdevc, 
                           ulong lop)
{
	int ix = fixed2int_var(fx0);
  int iy = fixed2int_var(fy0);
  int itox = fixed2int_var(fx1);
  int itoy = fixed2int_var(fy1);
  
  if (itoy == iy)
  {
  	return (ix <= itox ? pdevc->fillRectangle(ix, iy, itox - ix + 1, 1, this, lop, NULL) :
						pdevc->fillRectangle(itox, iy, ix - itox + 1, 1, this, lop, NULL));
  }
  
  if (itox == ix)
  {
  	return (iy <= itoy ? pdevc->fillRectangle(ix, iy, 1, itoy - iy + 1, this, lop, NULL) :
		        pdevc->fillRectangle(ix, itoy, 1, iy - itoy + 1, this, lop, NULL));
  }
  
  {
  	long h = fy1 - fy0;
		long w = fx1 - fx0;
		long tf;
		bool swap_axes;
		XWPSFixedEdge left, right;
		
		if ((w < 0 ? -w : w) <= (h < 0 ? -h : h)) 
		{
	    if (h < 0)
				SWAP(fx0, fx1, tf), SWAP(fy0, fy1, tf),  h = -h;
	    right.start.x = (left.start.x = fx0 - fixed_half) + fixed_1;
	    right.end.x = (left.end.x = fx1 - fixed_half) + fixed_1;
	    left.start.y = right.start.y = fy0;
	    left.end.y = right.end.y = fy1;
	    swap_axes = false;
		} 
		else 
		{
	    if (w < 0)
				SWAP(fx0, fx1, tf), SWAP(fy0, fy1, tf), w = -w;
	    right.start.x = (left.start.x = fy0 - fixed_half) + fixed_1;
	    right.end.x = (left.end.x = fy1 - fixed_half) + fixed_1;
	    left.start.y = right.start.y = fx0;
	    left.end.y = right.end.y = fx1;
	    swap_axes = true;
		}
		return fillTrapezoid(&left, &right, left.start.y, left.end.y, swap_axes, pdevc, lop);
  }
}

int XWPSDevice::endImage(XWPSImageEnumCommon * info, bool draw_last)
{
	int code = (this->*(procs.end_image_.end_image))(info, draw_last);
	return code;
}

int XWPSDevice::endImageDefault(XWPSImageEnumCommon * info, bool draw_last)
{
	return info->end(draw_last);
}

int XWPSDevice::endPage(int reason, XWPSState * pgs)
{
	return (this->*(page_procs.end_page))(reason, pgs);
}

int XWPSDevice::endPageDefault(int reason, XWPSState * )
{
	return (reason != 2 ? 1 : 0);
}

int  XWPSDevice::fillMask(const uchar * data, 
	                      int sourcex, 
	                      int raster, 
	                      ulong id,
		   									int x, 
		   									int y, 
		   									int w, 
		   									int h,
		   									XWPSDeviceColor * pdcolor, 
		   									int depth,
		   									ulong lop, 
		   									XWPSClipPath *pcpath)
{
	return (this->*(procs.fill_mask_.fill_mask))(data, sourcex, raster, id, x, y, w, h, pdcolor, depth, lop, pcpath);
}

int  XWPSDevice::fillMaskDefault(const uchar * data, 
	                      int dx, 
	                      int raster, 
	                      ulong id,
		   									int x, 
		   									int y, 
		   									int w, 
		   									int h,
		   									XWPSDeviceColor * pdcolor, 
		   									int depth,
		   									ulong lop, 
		   									XWPSClipPath *pcpath)
{
	XWPSDevice *dev;
  XWPSDeviceClip cdev;
  cdev.incRef();
  cdev.context_state = context_state;
  ulong colors[2];
  XWPSStripBitmap *tile;
  
  if (pdcolor->isPure()) 
  {
		tile = 0;
		colors[0] = ps_no_color_index;
		colors[1] = pdcolor->pureColor();
  } 
  else if (pdcolor->isBinaryHalftone()) 
  {
		tile = pdcolor->binaryTile();
		colors[0] = pdcolor->binaryColor(0);
		colors[1] = pdcolor->binaryColor(1);
  } 
  else
		return (int)(XWPSError::Unknown);
			
	if (pcpath != 0)
	{
		dev = &cdev;
	  cdev.translate(pcpath->getList(), 0, 0);
	  cdev.setTarget(this);
	  cdev.max_fill_band = max_fill_band;
	  cdev.open();
	}
	else
		dev = this;
		
	if (depth > 1)
		return dev->copyAlpha(data, dx, raster, id, x, y, w, h, colors[1], depth);
	
	if (lop != lop_default)
	{
		ulong scolors[2];

		scolors[0] = dev->getWhite();
		scolors[1] = dev->getBlack();
		if (tile == 0)
	    colors[0] = colors[1];
	    
		return dev->stripCopyRop(data, dx, raster, id, scolors, tile, colors, x, y, w, h,
	     											 pdcolor->getPhase()->x, pdcolor->getPhase()->y,
	     											 lop | (rop3_S | lop_S_transparent));
	}
	
	if (tile == 0) 
		return dev->copyMono(data, dx, raster, id, x, y, w, h, ps_no_color_index, colors[1]);
  
  fit_copy(data, dx, raster, id, x, y, w, h);
  {
  	const uchar *row = data + (dx >> 3);
		int dx_bit = dx & 7;
		int wdx = w + dx_bit;
		int iy;

		for (row = data, iy = 0; iy < h; row += raster, iy++)
		{
			for (int ix = dx_bit; ix < wdx;)
			{
				int i0;
				uint b;
				uint len;
				int code;
				
				b = row[ix >> 3];
				len = byte_bit_run_length[ix & 7][b ^ 0xff];
				if (len) 
				{
		    	ix += ((len - 1) & 7) + 1;
		    	continue;
				}
				i0 = ix;
				for (;;)
				{
					b = row[ix >> 3];
		    	len = byte_bit_run_length[ix & 7][b];
		    	if (!len)
						break;
		    	ix += ((len - 1) & 7) + 1;
		    	if (ix >= wdx) 
		    	{
						ix = wdx;
						break;
		    	}
		    	if (len < 8)
						break;
				}
				code = dev->stripTileRectangle(tile, i0 - dx_bit + x, iy + y, ix - i0, 1, colors[0], colors[1],
		     															 pdcolor->getPhase()->x, pdcolor->getPhase()->y);
				if (code < 0)
		    	return code;
			}
		}
  }
  
  return 0;
}

int  XWPSDevice::fillParallelogram(long px, 
	                               long py, 
	                               long ax, 
	                               long ay, 
	                               long bx, 
	                               long by,
	                               XWPSDeviceColor *pdevc, 
	                               ulong lop)
{
	return (this->*(procs.fill_parallelogram_.fill_parallelogram))(px, py, ax, ay, bx, by, pdevc, lop);
}

int XWPSDevice::fillParallelogramDefault(long px, 
	                               long py, 
	                               long ax, 
	                               long ay, 
	                               long bx, 
	                               long by,
	                               XWPSDeviceColor *pdevc, 
	                               ulong lop)
{
	long t;
  long qx, qy, ym;
  XWPSFixedEdge left, right;
  int code;
  
  if (PARALLELOGRAM_IS_RECT(ax, ay, bx, by)) 
  {
		XWPSIntRect r;

		INT_RECT_FROM_PARALLELOGRAM(&r, px, py, ax, ay, bx, by);
		return ((XWPSDeviceColor*)pdevc)->fillRectangle(r.p.x, r.p.y, r.q.x - r.p.x, r.q.y - r.p.y, this, lop, NULL);
  }
  
  if (ay < 0)
		px += ax, py += ay, ax = -ax, ay = -ay;
  if (by < 0)
		px += bx, py += by, bx = -bx, by = -by;
  qx = px + ax + bx;
  if ((ax ^ bx) < 0) 
  {
		if (ax > bx)
	    SWAP(ax, bx, t), SWAP(ay, by, t);
  }
  else
  {
  	if ((double)ay * bx < (double)by * ax)
	    SWAP(ax, bx, t), SWAP(ay, by, t);
  }
  
  qy = py + ay + by;
  left.start.x = right.start.x = px;
  left.start.y = right.start.y = py;
  left.end.x = px + ax;
  left.end.y = py + ay;
  right.end.x = px + bx;
  right.end.y = py + by;
  
#define ROUNDED_SAME(p1, p2)\
  (fixed_pixround(p1) == fixed_pixround(p2))
  
  if (ay < by)
  {
  	if (!ROUNDED_SAME(py, left.end.y)) 
  	{
	    code = fillTrapezoid(&left, &right, py, left.end.y, false, pdevc, lop);
	    if (code < 0)
				return code;
		}
		
		left.start = left.end;
		left.end.x = qx, left.end.y = qy;
		ym = right.end.y;
		if (!ROUNDED_SAME(left.start.y, ym)) 
		{
	    code = fillTrapezoid(&left, &right, left.start.y, ym, false, pdevc, lop);
	    if (code < 0)
				return code;
		}
		right.start = right.end;
		right.end.x = qx, right.end.y = qy;
  }
  else
  {
  	if (!ROUNDED_SAME(py, right.end.y)) 
  	{
	    code = fillTrapezoid(&left, &right, py, right.end.y, false, pdevc, lop);
	    if (code < 0)
				return code;
		}
		right.start = right.end;
		right.end.x = qx, right.end.y = qy;
		ym = left.end.y;
		if (!ROUNDED_SAME(right.start.y, ym)) 
		{
	    code = fillTrapezoid(&left, &right, right.start.y, ym, false, pdevc, lop);
	  	if (code < 0)
				return code;
		}
		left.start = left.end;
		left.end.x = qx, left.end.y = qy;
  }
  
  if (!ROUNDED_SAME(ym, qy))
		return fillTrapezoid(&left, &right, ym, qy, false, pdevc, lop);
  else
		return 0;
#undef ROUNDED_SAME
}

int  XWPSDevice::fillPath(XWPSImagerState * pis, 
	                      XWPSPath * ppath,
		                    XWPSFillParams * params,
	                      XWPSDeviceColor * pdevc, 
	                      XWPSClipPath * pcpath)
{
	return (this->*(procs.fill_path_.fill_path))(pis, ppath, params, pdevc, pcpath);
}

int  XWPSDevice::fillPathDefault(XWPSImagerState * pis, 
	                      XWPSPath * ppath,
		                    XWPSFillParams * params,
	                      XWPSDeviceColor * pdevc, 
	                      XWPSClipPath * pcpath)
{
	XWPSDevice * dev = this;
	XWPSFixedPoint adjust;

  ulong lop = pis->log_op;
  XWPSFixedRect ibox, bbox;
  XWPSDeviceClip cdev;
  cdev.incRef();
  cdev.context_state = context_state;
  XWPSPath ffpath;
  XWPSPath *pfpath;
  int code;
  long adjust_left, adjust_right, adjust_below, adjust_above;

#define no_band_mask ((long)(-1) << (sizeof(long) * 8 - 1))
  bool fill_by_trapezoids;
  XWPSLineList lst;

  adjust = params->adjust;
  ppath->getBbox(&ibox);
  if (params->fill_zero_width)
		adjust.adjustIfEmpty(&ibox);
		
	if (pcpath)
		pcpath->innerBox(&bbox);
  else
		getClipingBox(&bbox);
		
	if (!ibox.within(bbox))
	{
		if (pcpath)
	    pcpath->outerBox(&bbox);
	    
	  ibox.intersect(bbox);
	  if (ibox.p.x - adjust.x >= ibox.q.x + adjust.x ||
	    ibox.p.y - adjust.y >= ibox.q.y + adjust.y)
	    return 0;
	    
	  if (pcpath)
	  {
	  	dev = &cdev;
	  	cdev.translate(pcpath->getList(), 0, 0);
	  	cdev.setTarget(this);
	  	cdev.max_fill_band = max_fill_band;
	  	cdev.open();
	  }
	}
	
	if (adjust.x == fixed_half)
		adjust_left = fixed_half - fixed_epsilon, adjust_right = fixed_half;
	else
		adjust_left = adjust_right = adjust.x;
  if (adjust.y == fixed_half)
		adjust_below = fixed_half - fixed_epsilon,  adjust_above = fixed_half;
	else
		adjust_below = adjust_above = adjust.y;
		
	fill_by_trapezoids =	((adjust_below | adjust_above) != 0 || !ppath->hasCurves() || params->flatness >= 1.0);
	if (fill_by_trapezoids && !lop_is_idempotent(lop))
	{
		XWPSFixedRect rbox;
		if (ppath->isRectangular(&rbox)) 
		{
	    int x0 = fixed2int_pixround(rbox.p.x - adjust_left);
	    int y0 = fixed2int_pixround(rbox.p.y - adjust_below);
	    int x1 = fixed2int_pixround(rbox.q.x + adjust_right);
	    int y1 = fixed2int_pixround(rbox.q.y + adjust_above);

	    return pdevc->fillRectangle(x0, y0, x1 - x0, y1 - y0, dev, lop, 0);
		}
		fill_by_trapezoids = false;
	}
	
	if (!ppath->hasCurves())
		pfpath = ppath;
	else if (fill_by_trapezoids)
	{
		code = ffpath.addFlattenedAccurate(ppath, params->flatness, pis->accurate_curves);
		if (code < 0)
	    return code;
		pfpath = &ffpath;
	}
	else if (ppath->isMonotonic())
		pfpath = ppath;
	else
	{
		code = ffpath.addMonotonized(ppath);
		if (code < 0)
	    return code;
		pfpath = &ffpath;
	}
	
	if ((code = lst.add_y_list(pfpath, adjust_below, adjust_above, &ibox)) < 0)
		goto nope;
		
	if (fill_by_trapezoids)
		code = fillLoopByTrapezoids(&lst, params, pdevc, lop, &ibox, adjust_left, adjust_right, adjust_below, adjust_above,
	                              (max_fill_band == 0 ? no_band_mask : int2fixed(-max_fill_band)));
	else
		code = dev->fillLoopByScanLines(&lst, params, pdevc, lop, &ibox, adjust_left, adjust_right, adjust_below, adjust_above,
	                             (max_fill_band == 0 ? no_band_mask : int2fixed(-max_fill_band)));
	                             	
nope:
	if (lst.close_count != 0)
		pfpath->unclose(lst.close_count);
		
	return code;
}

int XWPSDevice::fillRectangle(int x, int y, int w, int h, ulong color)
{
	return (this->*(procs.fill_rectangle_.fill_rectangle))(x, y, w, h, color);
}

int XWPSDevice::fillTrapezoid(XWPSFixedEdge *left, 
	                          XWPSFixedEdge *right,
	                          long ybot, 
	                          long ytop, 
	                          bool swap_axes,
	                          XWPSDeviceColor *pdevc, 
	                          ulong lop)
{
	return (this->*(procs.fill_trapezoid_.fill_trapezoid))(left, right, ybot, ytop, swap_axes, pdevc, lop);
}

int XWPSDevice::fillTrapezoidDefault(XWPSFixedEdge *left, 
	                          XWPSFixedEdge *right,
	                          long ybot, 
	                          long ytop, 
	                          bool swap_axes,
	                          XWPSDeviceColor *pdevc, 
	                          ulong lop)
{
	const long ymin = fixed_pixround(ybot) + fixed_half;
  const long ymax = fixed_pixround(ytop);
  
  if (ymin >= ymax)
		return 0;
		
	{
		int iy = fixed2int_var(ymin);
		const int iy1 = fixed2int_var(ymax);
		XWPSTrapLine l, r;
		int rxl, rxr, ry;
		const long  x0l = left->start.x, x1l = left->end.x, x0r = right->start.x,
	    					x1r = right->end.x, dxl = x1l - x0l, dxr = x1r - x0r;
		const long	ysl = ymin - left->start.y, ysr = ymin - right->start.y;
		long fxl;
		bool fill_direct = pdevc->writePure(lop);
		int max_rect_height = 1;
		int code;
		
		l.h = left->end.y - left->start.y;
		r.h = right->end.y - right->start.y;
		l.x = x0l + (fixed_half - fixed_epsilon);
		r.x = x0r + (fixed_half - fixed_epsilon);
		ry = iy;
		
#define FILL_TRAP_RECT(x,y,w,h)\
  (swap_axes ? pdevc->fillRectangle(y, x, h, w, this, lop, NULL) :\
   pdevc->fillRectangle(x, y, w, h, this, lop, NULL))
#define FILL_TRAP_RECT_DIRECT(x,y,w,h)\
  (swap_axes ? fillRectangle(y, x, h, w, cindex) :\
   fillRectangle(x, y, w, h, cindex))
   
#define YMULT_QUO(ys, tl)\
  (ys < fixed_1 && tl.df < YMULT_LIMIT ? ys * tl.df / tl.h :\
   fixed_mult_quo(ys, tl.df, tl.h))
   
    if (fixed_floor(l.x) == fixed_pixround(x1l)) 
    {
	    l.di = 0, l.df = 0;
	    fxl = 0;
		} 
		else 
		{
	    l.compute_dx(dxl, ysl);
	    fxl = YMULT_QUO(ysl, l);
	    l.x += fxl;
		}
		
		if (fixed_floor(r.x) == fixed_pixround(x1r)) 
		{
	    if (l.di == 0 && l.df == 0)
				max_rect_height = max_int;
	    else
				r.di = 0, r.df = 0;
		}
		else if (dxr == dxl && fxl != 0) 
		{
	    if (l.di == 0)
				r.di = 0, r.df = l.df;
	    else
				r.compute_dx(dxr, ysr);
	    if (ysr == ysl && r.h == l.h)
				r.x += fxl;
	    else
				r.x += YMULT_QUO(ysr, r);
		} 
		else 
		{
	    r.compute_dx(dxr, ysr);
	    r.x += YMULT_QUO(ysr, r);
		}
		rxl = fixed2int_var(l.x);
		rxr = fixed2int_var(r.x);
		
		if (iy1 - iy <= max_rect_height) 
		{
	    iy = iy1 - 1;
		} 
		else 
		{
	    l.compute_ldx(ysl);
	    if (dxr == dxl && ysr == ysl && r.h == l.h)
				r.ldi = l.ldi, r.ldf = l.ldf, r.xf = l.xf;
	    else
				r.compute_ldx(ysr);
		}
		
#define STEP_LINE(ix, tl)\
  tl.x += tl.ldi;\
  if ( (tl.xf += tl.ldf) >= 0 ) tl.xf -= tl.h, tl.x++;\
  ix = fixed2int_var(tl.x)
  
    if (fill_direct) 
    {
	    ulong cindex = pdevc->pureColor();
	    while (++iy != iy1) 
	    {
				int ixl, ixr;

				STEP_LINE(ixl, l);
				STEP_LINE(ixr, r);
				if (ixl != rxl || ixr != rxr) 
				{
		    	code = FILL_TRAP_RECT_DIRECT(rxl, ry, rxr - rxl, iy - ry);
		    	if (code < 0)
						goto xit;
		    	rxl = ixl, rxr = ixr, ry = iy;
				}
	    }
	    code = FILL_TRAP_RECT_DIRECT(rxl, ry, rxr - rxl, iy - ry);
		} 
		else 
		{
	    while (++iy != iy1) 
	    {
				int ixl, ixr;

				STEP_LINE(ixl, l);
				STEP_LINE(ixr, r);
				if (ixl != rxl || ixr != rxr) 
				{
		    	code = FILL_TRAP_RECT(rxl, ry, rxr - rxl, iy - ry);
		    	if (code < 0)
						goto xit;
		    	rxl = ixl, rxr = ixr, ry = iy;
				}
	    }
	    code = FILL_TRAP_RECT(rxl, ry, rxr - rxl, iy - ry);
		}
#undef STEP_LINE

xit:	
		if (code < 0 && fill_direct)
	    return code;
	    
		return code;
	}
}

int XWPSDevice::fillTriangle(long px, 
	                         long py, 
	                         long ax, 
	                         long ay, 
	                         long bx, 
	                         long by,
	                         XWPSDeviceColor *pdevc, 
	                         ulong lop)
{
	return (this->*(procs.fill_triangle_.fill_triangle))(px, py, ax, ay, bx, by, pdevc, lop);
}

int XWPSDevice::fillTriangleDefault(long px, 
	                         long py, 
	                         long ax, 
	                         long ay, 
	                         long bx, 
	                         long by,
	                         XWPSDeviceColor *pdevc, 
	                         ulong lop)
{
	long t;
  long ym;
  XWPSFixedEdge left, right;
  int code;
  
  if (ay < 0)
		px += ax, py += ay, bx -= ax, by -= ay, ax = -ax, ay = -ay;
  if (by < 0)
		px += bx, py += by, ax -= bx, ay -= by, bx = -bx, by = -by;
  if (ay > by)
		SWAP(ax, bx, t), SWAP(ay, by, t);
		
	left.start.x = right.start.x = px;
  left.start.y = right.start.y = py;
  if (ay == 0)
  {
  	if (ax < 0)
	    left.start.x = px + ax;
		else
	    right.start.x = px + ax;
		left.end.x = right.end.x = px + bx;
		left.end.y = right.end.y = py + by;
		ym = py;
  }
  else if (ay == by)
  {
  	if (ax < bx)
	    left.end.x = px + ax, right.end.x = px + bx;
		else
	    left.end.x = px + bx, right.end.x = px + ax;
		left.end.y = right.end.y = py + by;
		ym = py;
  }
  else
  {
  	ym = py + ay;
		if (fixed_mult_quo(bx, ay, by) < ax) 
		{
	    left.end.x = px + bx, left.end.y = py + by;
	    right.end.x = px + ax, right.end.y = py + ay;
	    code = fillTrapezoid(&left, &right, py, ym, false, pdevc, lop);
	    right.start = right.end;
	    right.end = left.end;
		} 
		else 
		{
	    left.end.x = px + ax, left.end.y = py + ay;
	    right.end.x = px + bx, right.end.y = py + by;
	    code = fillTrapezoid(&left, &right, py, ym, false, pdevc, lop);
	    left.start = left.end;
	    left.end = right.end;
		}
		if (code < 0)
	    return code;
  }
  
  return fillTrapezoid(&left, &right, ym, right.end.y, false, pdevc, lop);
}

int XWPSDevice::getAlphaBits(PSGraphicsObjectType type)
{
	return (this->*(procs.get_alpha_bits_.get_alpha_bits))(type);
}

int XWPSDevice::getAlphaBitsDefault(PSGraphicsObjectType type)
{
	return (type == go_text ? color_info.anti_alias.text_bits : color_info.anti_alias.graphics_bits);
}

int XWPSDevice::getBand(int y, int *band_start)
{
	return (this->*(procs.get_band_.get_band))(y, band_start);
}

int XWPSDevice::getBits(int y, uchar * data, uchar ** actual_data)
{
	return (this->*(procs.get_bits_.get_bits))(y, data, actual_data);
}

int XWPSDevice::getBitsDefault(int y, uchar * data, uchar ** actual_data)
{
	int (XWPSDevice::*save_get_bits)(int, uchar *, uchar **)  = procs.get_bits_.get_bits;
	XWPSIntRect rect;
  XWPSGetBitsParams params;
  
  rect.p.y = y;
  rect.q.x = width, rect.q.y = y + 1;
  params.options =	(actual_data ? GB_RETURN_POINTER : 0) | GB_RETURN_COPY |
	                  (GB_ALIGN_STANDARD | GB_OFFSET_0 | GB_RASTER_STANDARD |
	                  GB_PACKING_CHUNKY | GB_COLORS_NATIVE | GB_ALPHA_NONE);
  
  params.raster = bitmap_raster(width * color_info.depth);
  params.data[0] = data;
  procs.get_bits_.get_bits = &XWPSDevice::getBitsNo;
  int code = getBitsRectangle(&rect, &params, NULL);
  if (actual_data)
		*actual_data = params.data[0];
	procs.get_bits_.get_bits = save_get_bits;
  return code;
}

int XWPSDevice::getBitsCopy(int x, 
	                        int w, 
	                        int h,
		                      XWPSGetBitsParams * params,
		                      XWPSGetBitsParams *stored,
		                      const uchar * src_base, 
		                      uint dev_raster)
{
	ulong options = params->options;
  ulong stored_options = stored->options;
  int x_offset = (options & GB_OFFSET_0 ? 0 : params->x_offset);
  int depth = color_info.depth;
  int bit_x = x * depth;
  const uchar *src = src_base;
  
  bool direct_copy = requestedIncludesStored(params, stored);
  int code = 0;
  
  if ((~options & GB_RETURN_COPY) ||
			!(options & (GB_OFFSET_0 | GB_OFFSET_SPECIFIED)) ||
			!(options & (GB_RASTER_STANDARD | GB_RASTER_SPECIFIED)))
		return (int)(XWPSError::RangeCheck);
			
	if (options & GB_PACKING_CHUNKY)
	{
		uchar *data = params->data[0];
		int end_bit = (x_offset + w) * depth;
		uint std_raster = (options & GB_ALIGN_STANDARD ? bitmap_raster(end_bit) : (end_bit + 7) >> 3);
		uint raster =  (options & GB_RASTER_STANDARD ? std_raster : params->raster);
		int dest_bit_x = x_offset * depth;
		int skew = bit_x - dest_bit_x;
		
		if (!(skew & 7) && direct_copy)
		{
			int bit_w = w * depth;

	    bytes_copy_rectangle(data + (dest_bit_x >> 3), raster,
				 										src + (bit_x >> 3), dev_raster, ((bit_x + bit_w + 7) >> 3) - (bit_x >> 3), h);
		}
		else if (direct_copy)
		{
			XWPSDeviceMem tdev;
			tdev.incRef();
	    uchar *line_ptr = data;
			tdev.makeMemMonoDevice(this);
	    tdev.line_ptrs = &tdev.base;
	    for (; h > 0; line_ptr += raster, src += dev_raster, --h) 
	    {
				int align = ALIGNMENT_MOD(line_ptr, align_bitmap_mod);

				tdev.base = line_ptr - align;
				tdev.copyMono(src, bit_x, dev_raster, ps_no_bitmap_id, dest_bit_x + (align << 3), 0, w, 1, 0, 1);
	    }
		}
		else if (options & ~stored_options & GB_COLORS_NATIVE)
		{
			code = getBitsStdToNative(x, w, h, params, stored, src_base, dev_raster, x_offset, raster);
	    options = params->options;
		}
		else
		{
			code = getBitsNativeToStd(x, w, h, params, stored, src_base, dev_raster, x_offset, raster, std_raster);
	    options = params->options;
		}
		params->options = (options & (GB_COLORS_ALL | GB_ALPHA_ALL)) | GB_PACKING_CHUNKY |
	    								(options & GB_COLORS_NATIVE ? 0 : options & GB_DEPTH_ALL) |
	    								(options & GB_ALIGN_STANDARD ? GB_ALIGN_STANDARD : GB_ALIGN_ANY) |
	    								GB_RETURN_COPY | (x_offset == 0 ? GB_OFFSET_0 : GB_OFFSET_SPECIFIED) |
	    								(raster == std_raster ? GB_RASTER_STANDARD : GB_RASTER_SPECIFIED);
	}
	else if (!(~options & (GB_PACKING_PLANAR | GB_SELECT_PLANES | GB_ALIGN_STANDARD)) &&
	       (stored_options & GB_PACKING_CHUNKY) &&
	       ((options & stored_options) & GB_COLORS_NATIVE))
	{
		int num_planes = color_info.num_components;
		int dest_depth = depth / num_planes;
		XWPSBitsPlane source, dest;
		int plane = -1;
		for (int i = 0; i < num_planes; ++i)
	    if (params->data[i] != 0) 
	    {
				if (plane >= 0)
		    	return (int)(XWPSError::RangeCheck);
				plane = i;
	    }
		source.data.read = src_base;
		source.raster = dev_raster;
		source.depth = depth;
		source.x = x;
		dest.data.write = params->data[plane];
		dest.raster = (options & GB_RASTER_STANDARD ?  bitmap_raster((x_offset + w) * dest_depth) : params->raster);
		dest.depth = dest_depth;
		dest.x = x_offset;
		return dest.bitsExtractPlane(&source, (num_planes - 1 - plane) * dest_depth, w, h);
	}
	else
		return (int)(XWPSError::RangeCheck);
  return code;
}

int XWPSDevice::getBitsRectangle(XWPSIntRect *prect,
	                             XWPSGetBitsParams *params, 
	                             XWPSIntRect **unread)
{
	return (this->*(procs.get_bits_rectangle_.get_bits_rectangle))(prect, params, unread);
}

int XWPSDevice::getBitsRectangleDefault(XWPSIntRect *prect,
	                             XWPSGetBitsParams *params, 
	                             XWPSIntRect **unread)
{
	int  (XWPSDevice::*save_get_bits_rectangle)(XWPSIntRect *,
	                                       XWPSGetBitsParams *, 
	                                       XWPSIntRect **) = procs.get_bits_rectangle_.get_bits_rectangle;
	                                       
	int depth = color_info.depth;
  uint min_raster = (width * depth + 7) >> 3;
  ulong options = params->options;
  int code;
  
  procs.get_bits_rectangle_.get_bits_rectangle = &XWPSDevice::getBitsRectangleNo;
  
  if (prect->q.y == prect->p.y + 1 &&
			!(~options & (GB_RETURN_COPY | GB_PACKING_CHUNKY | GB_COLORS_NATIVE)) &&
			(options & (GB_ALIGN_STANDARD | GB_ALIGN_ANY)) &&
			((options & (GB_OFFSET_0 | GB_OFFSET_ANY)) ||
	 		((options & GB_OFFSET_SPECIFIED) && params->x_offset == 0)) &&
			((options & (GB_RASTER_STANDARD | GB_RASTER_ANY)) ||
	 		((options & GB_RASTER_SPECIFIED) &&
	  		params->raster >= min_raster)) &&	unread == NULL)
	{
		uchar *data = params->data[0];
		uchar *row = data;
		
		if (!(prect->p.x == 0 && prect->q.x == width))
			row = new uchar[min_raster];
			
		code = getBits(prect->p.y, row, &params->data[0]);
		if (code >= 0)
		{
			if (row != data)
			{
				if (prect->p.x == 0 && params->data[0] != row)
				{
					;
				}
				else
				{
					int width_bits = (prect->q.x - prect->p.x) * depth;
		    	XWPSDeviceMem tdev;
		    	tdev.incRef();
					tdev.makeMemMonoDevice(this);
		    	tdev.width = width_bits;
		    	tdev.height = 1;
		    	tdev.line_ptrs = &tdev.base;
		    	tdev.base = data;
		    	code = tdev.copyMono(params->data[0], prect->p.x * depth, min_raster, ps_no_bitmap_id, 0, 0, width_bits, 1, 0, 1);
		    	params->data[0] = data;
				}
				
				
			}
			if (row)
			{
				if (row != data)
					delete [] row;
			}
			params->options =	GB_ALIGN_STANDARD | GB_OFFSET_0 | GB_PACKING_CHUNKY |
												GB_ALPHA_NONE | GB_COLORS_NATIVE | GB_RASTER_STANDARD |
												(params->data[0] == data ? GB_RETURN_COPY : GB_RETURN_POINTER);
	    goto ret;
		}
	}
	
	{
		int x = prect->p.x, w = prect->q.x - x;
		int bits_per_pixel = depth;
		uchar *row;

		if (options & GB_COLORS_STANDARD_ALL)
		{
			int bpc = GB_OPTIONS_MAX_DEPTH(options);
	    int nc = (options & GB_COLORS_CMYK ? 4 :
	     					options & GB_COLORS_RGB ? 3 : 1) +
	    					(options & (GB_ALPHA_ALL - GB_ALPHA_NONE) ? 1 : 0);
	    int bpp = bpc * nc;

	    if (bpp > bits_per_pixel)
				bits_per_pixel = bpp;
		}
		
		row = new uchar[(bits_per_pixel * w + 7) >> 3];
		
		uint dev_raster = getRaster(true);
	  uint raster = (options & GB_RASTER_SPECIFIED ? params->raster :
	     						options & GB_ALIGN_STANDARD ? bitmap_raster(depth * w) :
	     						(depth * w + 7) >> 3);
	  XWPSIntRect rect;
	  XWPSGetBitsParams copy_params;
	  ulong copy_options = (GB_ALIGN_STANDARD | GB_ALIGN_ANY) |
													(GB_RETURN_COPY | GB_RETURN_POINTER) |
													(GB_OFFSET_0 | GB_OFFSET_ANY) |
													(GB_RASTER_STANDARD | GB_RASTER_ANY) | GB_PACKING_CHUNKY |
													GB_COLORS_NATIVE | (options & (GB_DEPTH_ALL | GB_COLORS_ALL)) |
													GB_ALPHA_ALL;
	  uchar *dest = params->data[0];
	  int y;
	  rect.p.x = x, rect.q.x = x + w;
	  code = 0;
	  for (y = prect->p.y; y < prect->q.y; ++y)
	  {
	  	rect.p.y = y, rect.q.y = y + 1;
			copy_params.options = copy_options;
			copy_params.data[0] = row;
			code = getBitsRectangle(&rect, &copy_params, NULL);
			if (code < 0)
		    break;
			if (copy_params.options & GB_OFFSET_0)
		    copy_params.x_offset = 0;
			params->data[0] = dest + (y - prect->p.y) * raster;
			code = getBitsCopy(copy_params.x_offset, w, 1,params, &copy_params,	copy_params.data[0], dev_raster);
			if (code < 0)
		    break;
	  }
	  
	  if (row)
	  	delete [] row;
	  	
	  params->data[0] = dest;
	}	
	
ret:
	procs.get_bits_rectangle_.get_bits_rectangle = save_get_bits_rectangle;
	return (code < 0 ? code : 0);
}

int XWPSDevice::getBitsReturnPointer(int x, 
					                          int h,
			                              XWPSGetBitsParams *params,
			                              XWPSGetBitsParams *stored,
			                              uchar * stored_base)
{
	ulong options = params->options;
  ulong both = options & stored->options;

  if (!(options & GB_RETURN_POINTER) ||	
  	  !requestedIncludesStored(params, stored))
		return -1;
		
	{
		int depth = color_info.depth;
		uint dev_raster = (both & GB_PACKING_CHUNKY ?	getRaster(true) :
	     								both & GB_PACKING_PLANAR ?
	       							bitmap_raster(color_info.depth / color_info.num_components * width) :
	     								both & GB_PACKING_BIT_PLANAR ?  bitmap_raster(width) : 0);
		uint raster = (options & (GB_RASTER_STANDARD | GB_RASTER_ANY) ? dev_raster : params->raster);
		uchar *base;
		
		if (h <= 1 || raster == dev_raster)
		{
			int x_offset = (options & GB_OFFSET_ANY ? x : options & GB_OFFSET_0 ? 0 : params->x_offset);
			if (x_offset == x)
			{
				base = stored_base;
				params->x_offset = x;
			}
			else
			{
				uint align_mod = (options & GB_ALIGN_ANY ? 8 : align_bitmap_mod * 8);
				int bit_offset = x - x_offset;
				int bytes;

				if (bit_offset & (align_mod - 1))
		    	return -1;
				if (depth & (depth - 1)) 
				{
		    	int step = depth / igcd(depth, align_mod) * align_mod;

		    	bytes = bit_offset / step * step;
				} 
				else 
		    	bytes = bit_offset & (-depth & -align_mod);
		    	
				base = stored_base + arith_rshift(bytes, 3);
				params->x_offset = (bit_offset - bytes) / depth;
			}
			
			params->options =	GB_ALIGN_STANDARD | GB_RETURN_POINTER | GB_RASTER_STANDARD |
												(stored->options & ~GB_PACKING_ALL) |	(params->x_offset == 0 ? GB_OFFSET_0 : GB_OFFSET_SPECIFIED);
			if (both & GB_PACKING_CHUNKY)
			{
				params->options |= GB_PACKING_CHUNKY;
				params->data[0] = base;
			}
			else
			{
				int n = (stored->options & GB_PACKING_BIT_PLANAR ? (params->options |= GB_PACKING_BIT_PLANAR,
								 color_info.depth) :  (params->options |= GB_PACKING_PLANAR, color_info.num_components));

				for (int i = 0; i < n; ++i)
		    	if (!(both & GB_SELECT_PLANES) || stored->data[i] != 0) 
		    	{
						params->data[i] = base;
						base += dev_raster * height;
		    	}
			}
			
			return 0;
		}
	}
	
	return -1;
}

void XWPSDevice::getClipingBox(XWPSFixedRect*pbox)
{
	(this->*(procs.get_clipping_box_.get_clipping_box))(pbox);
}

void XWPSDevice::getClipingBoxDefault(XWPSFixedRect*pbox)
{
	pbox->p.x = 0;
  pbox->p.y = 0;
  pbox->q.x = int2fixed(width);
  pbox->q.y = int2fixed(height);
}

void XWPSDevice::getClipingBoxLarge(XWPSFixedRect*pbox)
{
	pbox->p.x = min_fixed;
  pbox->p.y = min_fixed;
  pbox->q.x = max_fixed;
  pbox->q.y = max_fixed;
}

int  XWPSDevice::getDeviceOrHWParams(XWPSParamList * plist, bool is_hardware)
{
  int code = (is_hardware ? getHardwareParams(context_state, plist) : getParams(context_state, plist));
	
  return code;
}

int  XWPSDevice::getHardwareParams(XWPSContextState * ctx, XWPSParamList *plist)
{
	return (this->*(procs.get_hardware_params_.get_hardware_params))(ctx, plist);
}

void XWPSDevice::getInitialMatrix(XWPSMatrix *pmat)
{
	(this->*(procs.get_initial_matrix_.get_initial_matrix))(pmat);
}

void XWPSDevice::getInitialMatrixDefault(XWPSMatrix *pmat)
{
	pmat->xx = HWResolution[0] / 72.0;
  pmat->xy = 0;
  pmat->yx = 0;
  pmat->yy = HWResolution[1] / -72.0;
  pmat->tx = 0;
  pmat->ty = height;
}

void XWPSDevice::getInitialMatrixUpright(XWPSMatrix *pmat)
{
	pmat->xx = HWResolution[0] / 72.0;
  pmat->xy = 0;
  pmat->yx = 0;
  pmat->yy = HWResolution[1] / 72.0;
  pmat->tx = 0;
  pmat->ty = 0;
}

XWPSDevice * XWPSDevice::getPageDevice()
{
	return (this->*(procs.get_page_device_.get_page_device))();
}

int  XWPSDevice::getParams(XWPSContextState * ctx, XWPSParamList *plist)
{
	return (this->*(procs.get_params_.get_params))(ctx, plist);
}

int  XWPSDevice::getParamsDefault(XWPSContextState * ctx,XWPSParamList *plist)
{
	int code;
	int mns = 1;
  bool seprs = false;
  PSParamString dns, pcms;
  PSParamFloatArray msa, ibba, hwra, ma;
  PSParamStringArray scna;
  
#define set_param_array(a, d, s)\
  (a.data = d, a.size = s, a.persistent = true);
  
  int colors = color_info.num_components;
  int depth = color_info.depth;
  int GrayValues = color_info.max_gray + 1;
  int HWSize[2];
  PSParamIntArray hwsa;
  PSParamFloatArray hwma, mhwra;
  dns.fromString(dname);
  
  {
  	const char *cms = pcmsa[colors];
  	if (*cms != 0)
	    pcms.fromString(cms);
  }
  
  set_param_array(hwra, HWResolution, 2);
  set_param_array(msa, MediaSize, 2);
  set_param_array(ibba, ImagingBBox, 4);
  set_param_array(ma, Margins, 2);
  set_param_array(scna, NULL, 0);
  
  HWSize[0] = width;
  HWSize[1] = height;
  set_param_array(hwsa, HWSize, 2);
  set_param_array(hwma, HWMargins, 4);
  set_param_array(mhwra, MarginsHWResolution, 2);
  if ((code = plist->writeName(ctx,"OutputDevice", &dns)) < 0 ||
  	   (code = plist->writeFloatArray(ctx,"PageSize", &msa)) < 0 ||
  	   (code = (pcms.data == 0 ? 0 : plist->writeName(ctx,"ProcessColorModel", &pcms))) < 0 ||
			 (code = plist->writeFloatArray(ctx,"HWResolution", &hwra)) < 0 ||
			 (code = (ImagingBBox_set ? plist->writeFloatArray(ctx,"ImagingBBox", &ibba) :plist->writeNull(ctx,"ImagingBBox"))) < 0 ||
			 (code = plist->writeFloatArray(ctx,"Margins", &ma)) < 0 ||
			 (code = plist->writeInt(ctx,"MaxSeparations", &mns)) < 0 ||
			 (code = (NumCopies_set < 0 || getPageDevice() == 0 ? 0: NumCopies_set ?
		 						plist->writeInt(ctx,"NumCopies", &NumCopies) :
		 						plist->writeNull(ctx,"NumCopies"))) < 0 ||
			 (code = plist->writeNameArray(ctx,"SeparationColorNames", &scna)) < 0 ||
			 (code = plist->writeBool(ctx,"Separations", &seprs)) < 0 ||
			 (code = plist->writeBool(ctx,"UseCIEColor", &UseCIEColor)) < 0 ||
			 (code = plist->writeIntArray(ctx,"HWSize", &hwsa)) < 0 ||
			 (code = plist->writeFloatArray(ctx,".HWMargins", &hwma)) < 0 ||
			 (code = plist->writeFloatArray(ctx,".MarginsHWResolution", &mhwra)) < 0 ||
			 (code = plist->writeFloatArray(ctx,".MediaSize", &msa)) < 0 ||
			 (code = plist->writeString(ctx,"Name", &dns)) < 0 ||
			 (code = plist->writeInt(ctx,"Colors", &colors)) < 0 ||
			 (code = plist->writeInt(ctx,"BitsPerPixel", &depth)) < 0 ||
			 (code = plist->writeInt(ctx,"GrayValues", &GrayValues)) < 0 ||
			 (code = plist->writeLong(ctx,"PageCount", &PageCount)) < 0 ||
			 (code = plist->writeBool(ctx,".IgnoreNumCopies", &IgnoreNumCopies)) < 0 ||
			 (code = plist->writeInt(ctx,"TextAlphaBits", &color_info.anti_alias.text_bits)) < 0 ||
			 (code = plist->writeInt(ctx,"GraphicsAlphaBits",	&color_info.anti_alias.graphics_bits)) < 0)
		return code;
		
	if (colors > 1) 
	{
		int RGBValues = color_info.max_color + 1;
		long ColorValues = 1L << depth;

		if ((code = plist->writeInt(ctx,"RedValues", &RGBValues)) < 0 ||
	      (code = plist->writeInt(ctx,"GreenValues", &RGBValues)) < 0 ||
	      (code = plist->writeInt(ctx,"BlueValues", &RGBValues)) < 0 ||
	      (code = plist->writeLong(ctx,"ColorValues", &ColorValues)) < 0)
	      return code;
  }
 if (plist->requested(ctx,"HWColorMap")) 
 {
		uchar palette[3 << 8];

		if (paramHWColorMap(palette)) 
		{
	    PSParamString hwcms;
	    hwcms.data = palette, hwcms.size = colors << depth;
	    if ((code = plist->writeString(ctx,"HWColorMap", &hwcms)) < 0)
				return code;
		}
  }

  return 0;
}

uint XWPSDevice::getRaster(bool pad)
{
	ulong bits = (ulong) width * color_info.depth;
  return (pad ? bitmap_raster(bits) : (uint) ((bits + 7) >> 3));
}

XWPSDevice * XWPSDevice::getXFontDevice()
{
	return (this->*(procs.get_xfont_device_.get_xfont_device))();
}

int  XWPSDevice::imageData(XWPSImageEnumCommon * info,
		      							 const uchar ** plane_data,
		      							 int data_x, 
		      							 uint raster, 
		      							 int heightA)
{
	return (this->*(procs.image_data_.image_data))(info, plane_data, data_x, raster, heightA);
}

int  XWPSDevice::imageDataDefault(XWPSImageEnumCommon * info,
		      							   const uchar ** plane_data,
		      							   int data_x, 
		      							   uint raster, 
		      							   int heightA)
{
	return info->imageData(plane_data, data_x, raster, heightA);
}

int XWPSDevice::install(XWPSState * pgs)
{
	return (this->*(page_procs.install))(pgs);
}

bool XWPSDevice::isTrueColor()
{
	int ncomp = color_info.num_components;
  int depth = color_info.depth;
  int i, max_v;
    
#define CV(i) (ushort)((ulong)ps_max_color_value * i / max_v)
#define CV0 ((ushort)0)

  switch (ncomp) 
  {
		case 1:	
	    max_v = color_info.max_gray;
	    if (max_v != (1 << depth) - 1)
				return 0;
	    for (i = 0; i <= max_v; ++i) 
	    {
				ushort v = CV(i);

				if (mapRGBColor(v, v, v) != i)
		    	return 0;
	    }
	    return true;
	    
		case 3:	
	    max_v = color_info.max_color;
	    if (depth % 3 != 0 || max_v != (1 << (depth / 3)) - 1)
				return false;
	    {
				const int gs = depth / 3, rs = gs * 2;

				for (i = 0; i <= max_v; ++i) 
				{
		    	ushort v = CV(i);

		    		if (mapRGBColor(v, CV0, CV0) !=i << rs ||
								mapRGBColor(CV0, v, CV0) !=i << gs ||
								mapRGBColor(CV0, CV0, v) !=	i)
							return 0;
				}
	    }
	    return true;
	    
		case 4:	
	    max_v = color_info.max_color;
	    if ((depth & 3) != 0 || max_v != (1 << (depth / 4)) - 1)
				return false;
	    {
				const int ys = depth / 4, ms = ys * 2, cs = ys * 3;

				for (i = 0; i <= max_v; ++i) 
				{
		    	ushort v = CV(i);

		    	if (mapCMYKColor(v, CV0, CV0, CV0) !=i << cs ||
							mapCMYKColor(CV0, v, CV0, CV0) !=i << ms ||
							mapCMYKColor(CV0, CV0, v, CV0) !=	i << ys ||
							mapCMYKColor(CV0, CV0, CV0, v) !=	i	)
						return 0;
				}
	    }
	    return 1;
	    
			default:
	    	return 0;		/* DeviceN */
  }
#undef CV
#undef CV0
}

ulong XWPSDevice::mapCMYKColor(ushort c, ushort m, ushort y, ushort k)
{
	return (this->*(procs.map_cmyk_color_.map_cmyk_color))(c, m, y, k);
}

ulong XWPSDevice::mapCMYKColorCMYK1Bit(ushort c, ushort m, ushort y, ushort k)
{
#define CV_BIT(v) ((v) >> (ps_color_value_bits - 1))
    return (ulong)
	(CV_BIT(k) + (CV_BIT(y) << 1) + (CV_BIT(m) << 2) + (CV_BIT(c) << 3));
#undef CV_BIT
}

ulong XWPSDevice::mapCMYKColorCMYK8Bit(ushort c, ushort m, ushort y, ushort k)
{
	ulong color =	ps_color_value_to_byte(k) +	((uint)ps_color_value_to_byte(y) << 8) +
								((uint)ps_color_value_to_byte(m) << 16) +	((uint)ps_color_value_to_byte(c) << 24);

  return (color == ps_no_color_index ? color ^ 1 : color);
}

ulong XWPSDevice::mapCMYKColorDefault(ushort c, ushort m, ushort y, ushort k)
{
	ushort rgb[3];
  XWPSImagerState s;
  s.i_ctx_p = context_state;
  s.colorCMYKToRgb(cv2frac(c), cv2frac(m), cv2frac(y), cv2frac(k), (short*)rgb);
  return mapRGBColor(frac2cv(rgb[0]), frac2cv(rgb[1]), frac2cv(rgb[2]));
}

int  XWPSDevice::mapColorRGB(ulong color, ushort* prgb)
{
	return (this->*(procs.map_color_rgb_.map_color_rgb))(color, prgb);
}

int XWPSDevice::mapColorRGBDefault(ulong color, ushort* prgb)
{
	prgb[0] = prgb[1] = prgb[2] = -(ushort)color;
  return 0;
}

int XWPSDevice::mapColorRGBAlpha(ulong color, ushort * prgba)
{
	return (this->*(procs.map_color_rgb_alpha_.map_color_rgb_alpha))(color, prgba);
}

int XWPSDevice::mapColorRGBAlphaDefault(ulong color, ushort * prgba)
{
	prgba[3] = ps_max_color_value;
  return mapColorRGB(color, prgba);
}

int XWPSDevice::mapColorRGBCMYK1Bit(ulong color, ushort* prgb)
{
	if (color & 1)
		prgb[0] = prgb[1] = prgb[2] = 0;
  else 
  {
		prgb[0] = (color & 8 ? 0 : ps_max_color_value);
		prgb[1] = (color & 4 ? 0 : ps_max_color_value);
		prgb[2] = (color & 2 ? 0 : ps_max_color_value);
  }
  return 0;
}

int XWPSDevice::mapColorRGBCMYK8Bit(ulong color, ushort* prgb)
{
	int	not_k = ~color & 0xff,	r = not_k - (color >> 24),	g = not_k - ((color >> 16) & 0xff),	b = not_k - ((color >> 8) & 0xff); 

  prgb[0] = (r < 0 ? 0 : ps_color_value_from_byte(r));
  prgb[1] = (g < 0 ? 0 : ps_color_value_from_byte(g));
  prgb[2] = (b < 0 ? 0 : ps_color_value_from_byte(b));
  return 0;
}

int XWPSDevice::mapColorRGBGrayDefault(ulong color, ushort* prgb)
{
	ushort gray = color * ps_max_color_value / color_info.max_gray;

  prgb[0] = gray;
  prgb[1] = gray;
  prgb[2] = gray;
  return 0;
}

int XWPSDevice::mapColorRGBRGBDefault(ulong color, ushort* prgb)
{
	if (color_info.depth == 24) 
	{
		prgb[0] = ps_color_value_from_byte(color >> 16);
		prgb[1] = ps_color_value_from_byte((color >> 8) & 0xff);
		prgb[2] = ps_color_value_from_byte(color & 0xff);
  } 
  else 
  {
		uint bits_per_color = color_info.depth / 3;
		uint color_mask = (1 << bits_per_color) - 1;

		prgb[0] = ((color >> (bits_per_color * 2)) & color_mask) * (ulong) ps_max_color_value / color_mask;
		prgb[1] = ((color >> (bits_per_color)) & color_mask) *  (ulong) ps_max_color_value / color_mask;
		prgb[2] = (color & color_mask) * (ulong) ps_max_color_value / color_mask;
  }
  return 0;
}

ulong XWPSDevice::mapRGBAlphaColor(ushort r, ushort g, ushort b, ushort a)
{
	return (this->*(procs.map_rgb_alpha_color_.map_rgb_alpha_color))(r, g, b, a);
}

ulong XWPSDevice::mapRGBAlphaColorDefault(ushort r, ushort g, ushort b, ushort )
{
	return mapRGBColor(r, g, b);
}

ulong XWPSDevice::mapRGBColor(ushort r, ushort g, ushort b)
{
	return (this->*(procs.map_rgb_color_.map_rgb_color))(r, g, b);
}

ulong XWPSDevice::mapRGBColorDefault(ushort r, ushort g, ushort b)
{
	return ((r | g | b) > ps_max_color_value / 2 ?  1 : 0);
}

ulong XWPSDevice::mapRGBColorGrayDefault(ushort r, ushort g, ushort b)
{
	ulong gray =  (((r * (ulong) lum_red_weight) +  (g * (ulong) lum_green_weight) +
      						(b * (ulong) lum_blue_weight) + (lum_all_weights / 2)) / lum_all_weights
     							* color_info.max_gray +  (ps_max_color_value / 2)) / ps_max_color_value;

  return gray;
}

ulong XWPSDevice::mapRGBColorRGBDefault(ushort r, ushort g, ushort b)
{
	if (color_info.depth == 24)
		return ps_color_value_to_byte(b) + ((uint) ps_color_value_to_byte(g) << 8) + ((ulong) ps_color_value_to_byte(r) << 16);
  else 
  {
		int bpc = color_info.depth / 3;
		int drop = sizeof(ushort) * 8 - bpc;
		return ((((r >> drop) << bpc) + (g >> drop)) << bpc) + (b >> drop);
  }
}

int XWPSDevice::open()
{
	if (is_open)
		return 0;
		
	int code = (this->*(procs.open__.open_))();
	if (code < 0)
	  return code;
	  
	is_open = true;
	return 1;
}

int XWPSDevice::outputPage(int num_copies, int flush)
{
	return (this->*(procs.output_page_.output_page))(num_copies, flush);
}

int XWPSDevice::outputPageDefault(int num_copies, int flush)
{
	int code = syncOutput();
	if (code >= 0)
		code = finishOutputPage(num_copies, flush);
		
	return code;
}

int XWPSDevice::putDeviceParams(XWPSContextState * ctx, XWPSParamList * plist)
{
	bool was_open = is_open;
  int code;

  code = putParams(ctx, plist);
  return (code < 0 ? code : was_open && !is_open ? 1 : code);
}

int XWPSDevice::putParams(XWPSContextState * ctx, XWPSParamList *plist)
{
	return (this->*(procs.put_params_.put_params))(ctx, plist);
}

int XWPSDevice::putParamsDefault(XWPSContextState * ctx, XWPSParamList *plist)
{
	int ecode = 0;
  int code;
  PSParamFloatArray hwra;
  PSParamIntArray hwsa;
  PSParamFloatArray msa;
  PSParamFloatArray ma;
  PSParamFloatArray hwma;
  PSParamFloatArray mhwra;
  PSParamStringArray scna;
  int nci = NumCopies;
  int ncset = NumCopies_set;
  bool ignc = IgnoreNumCopies;
  bool ucc = UseCIEColor;
  PSParamFloatArray ibba;
  bool ibbnull = false;
  int colors = color_info.num_components;
  int depth = color_info.depth;
  int GrayValues = color_info.max_gray + 1;
  int RGBValues = color_info.max_color + 1;
  long ColorValues = 1L << depth;
  int tab = color_info.anti_alias.text_bits;
  int gab = color_info.anti_alias.graphics_bits;
  PSParamString cms;
  
  switch (code = plist->readFloatArray(ctx, "HWResolution", &hwra))
  {
  	case 0:
			if (hwra.size != 2)
			{
				ecode = XWPSError::RangeCheck;
			}
			else
			{
				if (hwra.data[0] <= 0 || hwra.data[1] <= 0)
	    		ecode = XWPSError::RangeCheck;
				else
	    		break;
			}
			goto hwre;
			
		default:
			ecode = code;
			
hwre:	
	    plist->signalError(ctx, "HWResolution", ecode);
	    
	  case 1:
	  	break;
  }
  
  switch (code = plist->readIntArray(ctx, "HWSize", &hwsa))
  {
  	case 0:
  		if (hwra.size != 2)
			{
				ecode = XWPSError::RangeCheck;
			}
			else
			{
				if ((hwsa.data[0] <= 0 && hwsa.data[0] != width) ||
	    			(hwsa.data[1] <= 0 && hwsa.data[1] != height))
	    		ecode = XWPSError::RangeCheck;
#define max_coord (max_fixed / fixed_1)
#if max_coord < max_int
				else if (hwsa.data[0] > max_coord || hwsa.data[1] > max_coord)
	    		ecode = XWPSError::LimitCheck;
#endif
#undef max_coord
				else
	    		break;	    		
			}
			goto hwse;
			
		default:
			ecode = code;
			
hwse:	
	    plist->signalError(ctx, "HWSize", ecode);
	    
	  case 1:
	  	break;
  }
  
  {
  	const float *res = (hwra.data == 0 ? HWResolution : hwra.data);
  	code = plist->mediaSize(ctx, "PageSize", res, &msa);
		if (code < 0)
	    ecode = code;
	    
	  const float * data = msa.data;
		code = plist->mediaSize(ctx, ".MediaSize", res, &msa);
		if (code < 0)
	    ecode = code;
		else if (msa.data == 0)
	    msa.data = (float*)data;
  }
  
  switch (code = plist->readFloatArray(ctx, "Margins", &ma))
  {
  	case 0:
			if (ma.size != 2)
			{
				ecode = XWPSError::RangeCheck;
			}
			else
				break;				
			goto me;
			
		default:
			ecode = code;
			
me:	
	    plist->signalError(ctx, "Margins", ecode);
	    
	  case 1:
	  	break;
  }
  
  switch (code = plist->readFloatArray(ctx, ".HWMargins", &hwma))
  {
  	case 0:
			if (hwma.size != 4)
			{
				ecode = XWPSError::RangeCheck;
			}
			else
				break;
			goto hwme;
			
		default:
			ecode = code;
			
hwme:	
	    plist->signalError(ctx, ".HWMargins", ecode);
	    
	  case 1:
	  	break;
  }
  
  switch (code = plist->readFloatArray(ctx, ".MarginsHWResolution", &mhwra))
  {
  	case 0:
			if (mhwra.size != 2)
			{
				ecode = XWPSError::RangeCheck;
			}
			else
			{
				if (mhwra.data[0] != MarginsHWResolution[0] ||  mhwra.data[1] != MarginsHWResolution[1])
	    		ecode = XWPSError::RangeCheck;
	    	else
	    		break;
			}
			goto mhwre;
			
		default:
			ecode = code;
			
mhwre:	
	    plist->signalError(ctx, ".MarginsHWResolution", ecode);
	    
	  case 1:
	  	break;
  }
  
  switch (code = plist->readBool(ctx, ".IgnoreNumCopies", &ignc))
  {
  	default:
	    ecode = code;
	    plist->signalError(ctx, ".IgnoreNumCopies", ecode);
		case 0:
		case 1:
	    break;
  }
  
  if (NumCopies_set >= 0 &&	getPageDevice() != 0)
  {
		switch (code = plist->readInt(ctx, "NumCopies", &nci)) 
		{
			case 0:
	    	if (nci < 0)
					ecode = XWPSError::RangeCheck;
	    	else 
	    	{
					ncset = 1;
					break;
	    	}
	    	goto nce;
			default:
	    	if ((code = plist->readNull(ctx, "NumCopies")) == 0) 
	    	{
					ncset = 0;
					break;
	    	}
	    	ecode = code;
nce:
	    	plist->signalError(ctx, "NumCopies", ecode);
	    	
			case 1:
	    	break;
    }
  }
  if ((code = plist->readBool(ctx, "UseCIEColor", &ucc)) < 0) 
  {
		ecode = code;
		plist->signalError(ctx, "UseCIEColor", ecode);
  }
  if ((code = plist->antiAliasBits(ctx, "TextAlphaBits", &tab)) < 0)
		ecode = code;
  if ((code = plist->antiAliasBits(ctx, "GraphicsAlphaBits", &gab)) < 0)
		ecode = code;
		
#define IGNORE_INT_PARAM(pname)\
  { int igni;\
    switch ( code = plist->readInt(ctx, pname, &igni) )\
      { default:\
	  ecode = code;\
	  plist->signalError(ctx, pname, ecode);\
	case 0:\
	case 1:\
	  break;\
      }\
  }
  
  IGNORE_INT_PARAM("%MediaSource")
	IGNORE_INT_PARAM("%MediaDestination")
	
	switch (code = plist->readFloatArray(ctx, "ImagingBBox", &ibba)) 
	{
		case 0:
	    if (ibba.size != 4 ||	ibba.data[2] < ibba.data[0] || ibba.data[3] < ibba.data[1])
				ecode = XWPSError::RangeCheck;
	    else
				break;
	    goto ibbe;
	    
		default:
	    if ((code = plist->readNull(ctx, "ImagingBBox")) == 0) 
	    {
				ibbnull = true;
				break;
	    }
	    ecode = code;
	    
ibbe:
			plist->signalError(ctx, "ImagingBBox", ecode);
			
		case 1:
	    break;
  }
  
  if ((code = plist->checkBytes(ctx, "OutputDevice", (const uchar*)dname, strlen(dname), true)) < 0)
		ecode = code;
  if ((code = plist->checkBytes(ctx, "ProcessColorModel", (const uchar*)pcmsa[colors], strlen(pcmsa[colors]), colors != 0)) < 0)
		ecode = code;
  if ((code = plist->checkLong(ctx, "MaxSeparations", 1, true)) < 0)
		ecode = code;
  if ((code = plist->checkBool(ctx, "Separations", false, true)) < 0)
		ecode = code;
		
	switch (code = plist->readNameArray(ctx, "SeparationColorNames", &scna))
	{
		case 0:
			if (scna.size != 0)
			{
				ecode = XWPSError::RangeCheck;
			}
			else
				break;
			goto scne;
			
		default:
			ecode = code;
			
scne:	
	    plist->signalError(ctx, "SeparationColorNames", ecode);
	    
	  case 1:
	  	break;				
	}
	
	if ((code = plist->checkBytes(ctx, "Name", (const uchar *)dname, strlen(dname), true)) < 0)
		ecode = code;
  if ((code = plist->checkLong(ctx, "Colors", colors, true)) < 0)
		ecode = code;
  if ((code = plist->checkLong(ctx, "BitsPerPixel", depth, true)) < 0)
		ecode = code;
  if ((code = plist->checkLong(ctx, "GrayValues", GrayValues, true)) < 0)
		ecode = code;
  if ((code = plist->checkLong(ctx, "PageCount", PageCount, true)) < 0)
		ecode = code;
  if ((code = plist->checkLong(ctx, "RedValues", RGBValues, colors > 1)) < 0)
		ecode = code;
  if ((code = plist->checkLong(ctx, "GreenValues", RGBValues, colors > 1)) < 0)
		ecode = code;
  if ((code = plist->checkLong(ctx, "BlueValues", RGBValues, colors > 1)) < 0)
		ecode = code;
  if ((code = plist->checkLong(ctx, "ColorValues", ColorValues, colors > 1)) < 0)
		ecode = code;
  if (plist->readString(ctx, "HWColorMap", &cms) != 1) 
  {
		uchar palette[3 << 8];

		if (paramHWColorMap(palette))
	    code = plist->checkBytes(ctx, "HWColorMap", palette, colors << depth, true);
		else
	    code = plist->checkBytes(ctx, "HWColorMap", 0, 0, false);
		if (code < 0)
	    ecode = code;
  }
  
  code = plist->commit();
  if (ecode < 0)
		return ecode;
  if (code < 0)
		return code;
		
	if (hwra.data != 0 &&
			(HWResolution[0] != hwra.data[0] ||
	 		HWResolution[1] != hwra.data[1])) 
	{
		if (is_open)
	    close();
		setResolution(hwra.data[0], hwra.data[1]);
  }
  if (hwsa.data != 0 &&
			(width != hwsa.data[0] ||
	 		height != hwsa.data[1])) 
	{
		if (is_open)
	    close();
		setWidthHeight(hwsa.data[0], hwsa.data[1]);
  }
  if (msa.data != 0 &&
			(MediaSize[0] != msa.data[0] ||
	 		MediaSize[1] != msa.data[1])) 
	{
		if (is_open)
	    close();
		setMediaSize(msa.data[0], msa.data[1]);
  }
  if (ma.data != 0) 
  {
		Margins[0] = ma.data[0];
		Margins[1] = ma.data[1];
  }
  if (hwma.data != 0) 
  {
		HWMargins[0] = hwma.data[0];
		HWMargins[1] = hwma.data[1];
		HWMargins[2] = hwma.data[2];
		HWMargins[3] = hwma.data[3];
  }
  NumCopies = nci;
  NumCopies_set = ncset;
  IgnoreNumCopies = ignc;
  if (ibba.data != 0) 
  {
		ImagingBBox[0] = ibba.data[0];
		ImagingBBox[1] = ibba.data[1];
		ImagingBBox[2] = ibba.data[2];
		ImagingBBox[3] = ibba.data[3];
		ImagingBBox_set = true;
  } 
  else if (ibbnull) 
  {
		ImagingBBox_set = false;
  }
  UseCIEColor = ucc;
  color_info.anti_alias.text_bits = tab;
  color_info.anti_alias.graphics_bits = gab;
  decacheColors();
  return 0;
}

void XWPSDevice::setMediaSize(float media_width, float media_height)
{
	MediaSize[0] = media_width;
  MediaSize[1] = media_height;
  width = media_width * HWResolution[0] / 72.0 + 0.499;
  height = media_height * HWResolution[1] / 72.0 + 0.499;
}

void XWPSDevice::setPaperSize(int w, int h)
{
	width = w;
	height = h;
	MediaSize[0] = (((width) * 72.0 + 0.5) - 0.5) / (X_DPI);
	MediaSize[1] = (((height) * 72.0 + 0.5) - 0.5) / (Y_DPI);
	HWResolution[0] = Y_DPI;
	HWResolution[1] = Y_DPI;
	MarginsHWResolution[0] = Y_DPI;
	MarginsHWResolution[1] = Y_DPI;
}

void XWPSDevice::setResolution(float x_dpi, float y_dpi)
{
	HWResolution[0] = x_dpi;
  HWResolution[1] = y_dpi;
  width = MediaSize[0] * x_dpi / 72.0 + 0.5;
  height = MediaSize[1] * y_dpi / 72.0 + 0.5;
}

void XWPSDevice::setWidthHeight(int widthA, int heightA)
{
	width = widthA;
  height = heightA;
  MediaSize[0] = widthA * 72.0 / HWResolution[0];
  MediaSize[1] = heightA * 72.0 / HWResolution[1];
}

int XWPSDevice::stripCopyRop(const uchar *sdata, 
	                         int sourcex, 
	                         uint sraster, 
	                         ulong id,
	                         const ulong *scolors,
	                         XWPSStripBitmap *textures, 
	                         const ulong *tcolors,
	                         int x, 
	                         int y, 
	                         int widthA, 
	                         int heightA,
	                         int phase_x, 
	                         int phase_y, 
	                         ulong lop)
{
	return (this->*(procs.strip_copy_rop_.strip_copy_rop))(sdata, sourcex, sraster, id,scolors, textures, tcolors, x,y,widthA,heightA,phase_x,phase_y,lop);
}

int XWPSDevice::stripTileRectangle(XWPSStripBitmap * tiles,
			      										 int x, 
			      										 int y, 
			      										 int w, 
			      										 int h,
			      										 ulong color0, 
			      										 ulong color1,
			      										 int px, 
			      										 int py)
{
	return (this->*(procs.strip_tile_rectangle_.strip_tile_rectangle))(tiles, x, y, w,h, color0, color1, px,py);
}

int XWPSDevice::stripTileRectangleDefault(XWPSStripBitmap * tiles,
			      										 int x, 
			      										 int y, 
			      										 int w, 
			      										 int h,
			      										 ulong color0, 
			      										 ulong color1,
			      										 int px, 
			      										 int py)
{
	int widthA = tiles->size.x;
  int heightA = tiles->size.y;
  int raster = tiles->raster;
  int rwidth = tiles->rep_width;
  int rheight = tiles->rep_height;
  int shift = tiles->shift;
  ulong tile_id = tiles->id;

  fit_fill_xy(x, y, w, h);
  
  if (procs.tile_rectangle_.tile_rectangle != &XWPSDevice::tileRectangleDefault)
  {
  	if (shift == 0)
  	{
  		int (XWPSDevice::*tile_proc)(XWPSTileBitmap *, 
	                                  int, 
	                                  int, 
	                                  int, 
	                                  int,
                                    ulong, 
                                    ulong,
                                    int, 
                                    int) = procs.tile_rectangle_.tile_rectangle;
			procs.tile_rectangle_.tile_rectangle = &XWPSDevice::tileRectangleDefault;
			int code = tileRectangle(tiles, x, y, w, h,	 color0, color1, px, py);
	    procs.tile_rectangle_.tile_rectangle = tile_proc;
	    return code;
  	}
  }
  
  {
  	int xoff = (shift == 0 ? px :	 px + (y + py) / rheight * tiles->rep_shift);
		int irx = ((rwidth & (rwidth - 1)) == 0 ? (x + xoff) & (rwidth - 1) : (x + xoff) % rwidth);
		int ry = ((rheight & (rheight - 1)) == 0 ?  (y + py) & (rheight - 1) : (y + py) % rheight);
		int icw = widthA - irx;
		int ch = heightA - ry;
		uchar *row = tiles->data + ry * raster;
		
		int code;

#define real_copy_tile(srcx, tx, ty, tw, th, id)\
  code =\
    ((color0 == ps_no_color_index && color1 == ps_no_color_index) ?\
     copyColor(row, srcx, raster, id, tx, ty, tw, th) :\
     copyMono(row, srcx, raster, id, tx, ty, tw, th, color0, color1));\
  		if (code < 0) return (code);
  			
#define copy_tile(srcx, tx, ty, tw, th, id)\
  real_copy_tile(srcx, tx, ty, tw, th, id)
  
  	if (ch >= h)
  	{
  		if (icw >= w)
  		{
  			copy_tile(irx, x, y, w, h, (w == widthA && h == heightA ? tile_id : ps_no_bitmap_id));
  		}
  		else
  		{
  			int ex = x + w;
				int fex = ex - widthA;
				int cx = x + icw;
				ulong id = (h == heightA ? tile_id : ps_no_bitmap_id);

				copy_tile(irx, x, y, icw, h, ps_no_bitmap_id);
				while (cx <= fex) 
				{
		    	copy_tile(0, cx, y, widthA, h, id);
		    	cx += widthA;
				}
				if (cx < ex) 
				{
		    	copy_tile(0, cx, y, ex - cx, h, ps_no_bitmap_id);
				}
  		}
  	}
  	else if (icw >= w && shift == 0)
  	{
  		int ey = y + h;
	    int fey = ey - heightA;
	    int cy = y + ch;
	    ulong id = (w == widthA ? tile_id : ps_no_bitmap_id);

	    copy_tile(irx, x, y, w, ch, (ch == heightA ? id : ps_no_bitmap_id));
	    row = tiles->data;
	    do 
	   	{
				ch = (cy > fey ? ey - cy : heightA);
				copy_tile(irx, x, cy, w, ch, (ch == heightA ? id : ps_no_bitmap_id));
	    } while ((cy += ch) < ey);
  	}
  	else
  	{
  		int ex = x + w, ey = y + h;
	    int fex = ex - widthA, fey = ey - heightA;
	    int cx, cy;

	    for (cy = y;;) 
	    {
				ulong id = (ch == heightA ? tile_id : ps_no_bitmap_id);

				if (icw >= w) 
				{
		    	copy_tile(irx, x, cy, w, ch, (w == widthA ? id : ps_no_bitmap_id));
				} 
				else 
				{
		    	copy_tile(irx, x, cy, icw, ch, ps_no_bitmap_id);
		    	cx = x + icw;
		    	while (cx <= fex) 
		    	{
						copy_tile(0, cx, cy, widthA, ch, id);
						cx += widthA;
		    	}
		    	if (cx < ex) 
		    	{
						copy_tile(0, cx, cy, ex - cx, ch, ps_no_bitmap_id);
		    	}
				}
				if ((cy += ch) >= ey)
		    	break;
				ch = (cy > fey ? ey - cy : heightA);
				if ((irx += shift) >= rwidth)
		    	irx -= rwidth;
				icw = widthA - irx;
				row = tiles->data;
	    }
  	}
  	
#undef copy_tile
#undef real_copy_tile
  }
  
  return 0;
}

int XWPSDevice::strokePath(XWPSImagerState * pis, 
	                       XWPSPath * ppath,
		                     XWPSStrokeParams * params,
	                       XWPSDeviceColor * pdcolor, 
	                       XWPSClipPath *pcpath)
{
	return (this->*(procs.stroke_path_.stroke_path))(pis, ppath, params, pdcolor,pcpath);
}

int XWPSDevice::strokePathDefault(XWPSImagerState * pis, 
	                       XWPSPath * ppath,
		                     XWPSStrokeParams * params,
	                       XWPSDeviceColor * pdcolor, 
	                       XWPSClipPath *pcpath)
{
	return strokePathOnly(ppath, 0, pis, params, pdcolor, pcpath);
}

int XWPSDevice::strokePathOnly(XWPSPath * ppath,
					                   XWPSPath * to_path, 
					                   XWPSImagerState * pis, 
		                         XWPSStrokeParams * params,
	                           XWPSDeviceColor * pdevc, 
	                           XWPSClipPath *pcpath)
{
	XWPSFixedRect ibox, cbox;
  XWPSDeviceClip cdev;
  cdev.incRef();
  cdev.context_state = context_state;
  XWPSDevice *dev = this;
  int code = 0;
  XWPSFillParams fill_params;
  XWPSLineParams *pgs_lp = pis->currentLineParams();
  int dash_count = pgs_lp->dash.pattern_size;
  XWPSPath fpath, dpath;
  XWPSPath stroke_path_body;
  const XWPSPath *spath;
  float xx = pis->ctm.xx, xy = pis->ctm.xy;
  float yx = pis->ctm.yx, yy = pis->ctm.yy;
  
  int uniform = 0;
  bool reflected = xy * yx > xx * yy;
  PSOrientation orient = orient_other;
  
  const PSSegmentNotes not_first = sn_not_first;
  
  PSLineJoin curve_join =	(pgs_lp->curve_join >= 0 ? (PSLineJoin)pgs_lp->curve_join :
	 												pgs_lp->join == ps_join_none ? ps_join_bevel : pgs_lp->join);
  float line_width = pgs_lp->half_width;
  bool always_thin;
  double line_width_and_scale, device_line_width_scale;
  double device_dot_length = pgs_lp->dot_length * fixed_1;
  const XWPSSubpath *psub;
  
  ppath->getBbox(&ibox);
  
  {
  	XWPSFixedPoint expansion;
  	if (pis->strokePathExpansion(ppath, &expansion) < 0)
  	{
  		ibox.p.x = ibox.p.y = min_fixed;
	    ibox.q.x = ibox.q.y = max_fixed;
  	}
  	else
  	{
  		expansion.x += pis->fill_adjust.x;
	    expansion.y += pis->fill_adjust.y;
	    ibox.p.x = (ibox.p.x < min_fixed + expansion.x ? min_fixed :
			ibox.p.x - expansion.x);
	    ibox.p.y = (ibox.p.y < min_fixed + expansion.y ? min_fixed :
			ibox.p.y - expansion.y);
	    ibox.q.x = (ibox.q.x > max_fixed - expansion.x ? max_fixed :
			ibox.q.x + expansion.x);
	    ibox.q.y = (ibox.q.y > max_fixed - expansion.y ? max_fixed :
			ibox.q.y + expansion.y);
  	}
  }
  
  if (pcpath)
		pcpath->innerBox(&cbox);
  else if (pdevc)
		getClipingBox(&cbox);
  else 
		cbox = ibox;
  
  if (!ibox.within(cbox))
  {
  	XWPSFixedRect bbox;  	
  	if (pcpath) 
  	{
	    pcpath->outerBox(&bbox);
	    ibox.intersect(bbox);
		} 
		else
	    ibox.intersect(cbox);
		if (ibox.p.x >= ibox.q.x || ibox.p.y >= ibox.q.y) 
	    return 0;
		
		if (pcpath) 
		{
	    dev = &cdev;
	  	cdev.translate(pcpath->getList(), 0, 0);
	  	cdev.setTarget(this);
	  	cdev.max_fill_band = max_fill_band;
	  	cdev.open();
		}
  }
  
  fill_params.rule = ps_rule_winding_number;
  fill_params.flatness = pis->flatness;
#ifdef USE_FILL_ADJUSTMENT
  fill_params.fill_zero_width = (pis->fill_adjust.x | pis->fill_adjust.y) != 0;
#else
  fill_params.fill_zero_width = false;
#endif

	if (line_width < 0)
		line_width = -line_width;
  line_width_and_scale = line_width * (double)int2fixed(1);
  if (is_fzero(line_width))
		always_thin = true;
  else 
  {
		float xa, ya;
		
		switch (orient)
		{
			case orient_portrait:
				xa = xx, ya = yy;
				goto sat;
				
	    case orient_landscape:
				xa = xy, ya = yx;
	      
sat:
				if (xa < 0)
		    	xa = -xa;
				if (ya < 0)
		    	ya = -ya;
				always_thin = (qMax(xa, ya) * line_width < 0.5);
				if (!always_thin && uniform) 
		    	device_line_width_scale = line_width_and_scale * xa;
				break;
				
	    default:
				{
		    	double xsq = xx * xx + xy * xy;
		    	double ysq = yx * yx + yy * yy;
		    	double cross = xx * yx + xy * yy;

		    	if (cross < 0)
						cross = 0;
		    	always_thin =	((qMax(xsq, ysq) + cross) * line_width * line_width < 0.25);
				}
		}
	}
	
	if (device_dot_length != 0)
	{
		XWPSMatrix mat;
		if (pgs_lp->dot_length_absolute) 
	    getInitialMatrix(&mat);
		else
			pis->ctmOnly(&mat);
	  
		device_dot_length *= fabs(mat.xy) + fabs(mat.yy);
	}
	
	if (!ppath->hasCurves())
	{
		if (!ppath->firstSubpath())
	    return 0;
		spath = ppath;
	}
	else
	{
		if ((code = fpath.copyReducing(ppath, float2fixed(params->flatness), pis,
			                             (PSPathCopyOptions)(pis->accurate_curves ? pco_accurate | pco_for_stroke : pco_for_stroke))) < 0)
	    return code;
		spath = &fpath;
	}
	
	if (dash_count) 
	{
		code = dpath.addDashExpansion((XWPSPath*)spath, pis);
		if (code < 0)
	    goto exf;
		spath = &dpath;
  }
  
  if (to_path == 0)
  	to_path = &stroke_path_body;
  
  for (psub = ((XWPSPath*)spath)->firstSubpath(); psub != 0;)
  {
  	int index = 0;
		const XWPSSegment *pseg = psub;
		long x = pseg->pt.x;
		long y = pseg->pt.y;
		bool is_closed = ((const XWPSSubpath *)pseg)->is_closed;
		XWPSPartialLine pl, pl_prev, pl_first;
		
		while ((pseg = pseg->next) != 0 &&
	       pseg->type != s_start)
	  {
	  	long sx = pseg->pt.x, udx = sx - x;
	    long sy = pseg->pt.y, udy = sy - y;

	    pl.o.p.x = x, pl.o.p.y = y;
d:
			pl.e.p.x = sx, pl.e.p.y = sy;
	    if (!(udx | udy))
	    {
	    	if (index != 0)
		    	continue;
		    	
		    while ((pseg = pseg->next) != 0 && pseg->type != s_start) 
		    {
		    	sx = pseg->pt.x, udx = sx - x;
		    	sy = pseg->pt.y, udy = sy - y;
		    	if (udx | udy)
						goto d;
				}
				
				if (!(pgs_lp->cap == ps_cap_round || pgs_lp->dot_length != 0))
		    	break;
		    	
		    {
		    	const XWPSSegment *end = psub->prev;

		    	if (end != 0 && (end->pt.x != x || end->pt.y != y))
						sx = end->pt.x, sy = end->pt.y;
		    	else if (pseg != 0 && (pseg->pt.x != x || pseg->pt.y != y))
						sx = pseg->pt.x, sy = pseg->pt.y;
				}
				
				udx = sx - x, udy = sy - y;
				if ((udx | udy) == 0) 
				{
		    	if (is_fzero(pgs_lp->dot_orientation.xy)) 
						udx = fixed_1;
		    	else 
						udy = fixed_1;
				}
				
				{
		    	double scale = device_dot_length /  hypot((double)udx, (double)udy);
		    	if (pgs_lp->cap == ps_cap_butt) 
		    	{
						long dmax = qMax(qAbs(udx), qAbs(udy));
						if (dmax * scale < fixed_1)
			    		scale = (float)fixed_1 / dmax;
		    	}
		    	udx = (long) (udx * scale);
		    	udy = (long) (udy * scale);
		    	if ((udx | udy) == 0)
						udy = fixed_epsilon;
		    	sx = x + udx;
		    	sy = y + udy;
				}
				
				pseg = (pseg != 0 ? pseg->prev : psub->last);
				goto d;
	    }
	    
	    if (always_thin)
	    {
	    	pl.e.cdelta.x = pl.e.cdelta.y = 0;
				pl.width.x = pl.width.y = 0;
				pl.thin = true;
	    }
	    else
	    {
	    	if (uniform != 0)
	    	{
	    		float dpx = udx, dpy = udy;
		    	float wl = device_line_width_scale /  hypot(dpx, dpy);
	
		    	pl.e.cdelta.x = (long) (dpx * wl);
		    	pl.e.cdelta.y = (long) (dpy * wl);
		    	pl.width.x = -pl.e.cdelta.y,
					pl.width.y = pl.e.cdelta.x;
		    	pl.thin = false;
	    	}
	    	else
	    	{
	    		XWPSPoint dpt;
	    		float wl;
		    	pis->idtransform((float)udx, (float)udy, &dpt); 
		    	wl = line_width_and_scale /hypot(dpt.x, dpt.y);
		    	dpt.x *= wl;
		    	dpt.y *= wl;
		    	
		    	pl.e.cdelta.x = (long) (dpt.x * xx);
		    	pl.e.cdelta.y = (long) (dpt.y * yy);
		    	if (orient != orient_portrait)
						pl.e.cdelta.x += (long) (dpt.y * yx),  pl.e.cdelta.y += (long) (dpt.x * xy);
		    	if (!reflected)
						dpt.x = -dpt.x, dpt.y = -dpt.y;
		    	pl.width.x = (long) (dpt.y * xx), pl.width.y = -(long) (dpt.x * yy);
		    	if (orient != orient_portrait)
						pl.width.x -= (long) (dpt.x * yx), pl.width.y += (long) (dpt.y * xy);
		    	pl.thin = pl.width_is_thin();
	    	}
	    	
	    	if (!pl.thin)
	    	{
	    		pl.adjust_stroke(pis, false);
		    	pl.compute_caps();
	    	}
	    }
	    
	    if (index++)
	    {
	    	PSLineJoin join = (pseg->notes & not_first ? curve_join : pgs_lp->join);
				int first;
				XWPSPartialLine * lptr;

				if (join == ps_join_none) 
				{
		    	first = 0;
		    	lptr = 0;
		    	index = 1;
				} 
				else 
				{
		    	first = (is_closed ? 1 : index - 2);
		    	lptr = &pl;
				}
				if (to_path == 0)
					code = dev->strokeFill(to_path, first, &pl_prev, lptr,  pdevc, pis, params, &cbox, uniform, join);
				else
					code = dev->strokeAdd(to_path, first, &pl_prev, lptr,  pdevc, pis, params, &cbox, uniform, join);
				if (code < 0)
		    	goto exit;
				FILL_STROKE_PATH(always_thin);
	    }
	    else
	    	pl_first = pl;
	    	
	    pl_prev = pl;
	    x = sx, y = sy;
	  }
	  
	  if (index)
	  {
	  	PSLineJoin join =	((pseg == 0 ? ((XWPSPath*)spath)->firstSubpath() : pseg)->notes & not_first ? curve_join : pgs_lp->join);	  		
	    XWPSPartialLine * lptr = (!is_closed || join == ps_join_none ? 0 : &pl_first);
	    if (to_path == 0)
	    	code = dev->strokeFill(to_path, index - 1, &pl_prev, lptr, pdevc, pis, params, &cbox, uniform, join);
	    else
	    	code = dev->strokeAdd(to_path, index - 1, &pl_prev, lptr, pdevc, pis, params, &cbox, uniform, join);
	    if (code < 0)
				goto exit;
	    FILL_STROKE_PATH(always_thin);
	  }
	  
	  psub = (const XWPSSubpath *)pseg;
  }
  
exit:
exf:
	return code;
}

int XWPSDevice::syncOutput()
{
	return (this->*(procs.sync_output_.sync_output))();
}

int XWPSDevice::textBegin(XWPSImagerState * pis,
		    								XWPSTextParams *text, 
		    								XWPSFont * font,
		    								XWPSPath * path, 
		    								XWPSDeviceColor * pdcolor,
		    								XWPSClipPath * pcpath,
		    								XWPSTextEnum ** ppte)
{
	return (this->*(procs.text_begin_.text_begin))(pis, text, font,path, pdcolor, pcpath, ppte);
}

int XWPSDevice::textBeginDefault(XWPSImagerState * pis,
		    								XWPSTextParams *text, 
		    								XWPSFont * font,
		    								XWPSPath * path, 
		    								XWPSDeviceColor * pdcolor,
		    								XWPSClipPath * pcpath,
		    								XWPSTextEnum ** ppte)
{
	uint operation = text->operation;
  bool propagate_charpath = (operation & TEXT_DO_DRAW) != 0;
  int code;
  
  XWPSState *pgs = (XWPSState*)pis;
  XWPSShowEnum *penum = new XWPSShowEnum(pgs);
  code = penum->init(this, pis, text, font, path, pdcolor, pcpath);
  penum->auto_release = false; 
  penum->level = pgs->level;
  if (operation & TEXT_DO_ANY_CHARPATH)
			penum->charpath_flag = (operation & TEXT_DO_FALSE_CHARPATH ? cpm_false_charpath :
	     												operation & TEXT_DO_TRUE_CHARPATH ? cpm_true_charpath :
	     												operation & TEXT_DO_FALSE_CHARBOXPATH ? cpm_false_charboxpath :
	     												operation & TEXT_DO_TRUE_CHARBOXPATH ? cpm_true_charboxpath :
	     												operation & TEXT_DO_CHARWIDTH ? cpm_charwidth : cpm_show);
  else
		penum->charpath_flag =  (propagate_charpath ? pgs->in_charpath : cpm_show);
			
	switch (penum->charpath_flag) 
	{
    case cpm_false_charpath: 
    case cpm_true_charpath:
			penum->can_cache = -1; 
			break;
			
    case cpm_false_charboxpath: 
    case cpm_true_charboxpath:
			penum->can_cache = 0; 
			break;
			
    case cpm_charwidth:
    default:
			penum->can_cache = 1; 
			break;
  }
  
  code = penum->showStateSetup();
  if (code < 0)
		return code;
		
	penum->show_gstate =	(propagate_charpath && (pgs->in_charpath != 0) ? pgs->show_gstate : pgs);
	if (!(~operation & (TEXT_DO_NONE | TEXT_RETURN_WIDTH)))
	{
		XWPSDeviceNull *dev_null = new XWPSDeviceNull;
		dev_null->context_state = context_state;
		if ((code = pgs->save()) < 0)
	    return code;
		penum->level = pgs->level;
		dev_null->makeNullDevice(pgs->currentDevice());
		pgs->ctm_default_set = false;
		penum->dev_null = dev_null;
		pgs->setDeviceNoInit(dev_null);
		pgs->newPath();
		pgs->translateToFixed(fixed_0, fixed_0);
		code = pgs->path->addPoint(fixed_0, fixed_0);
		if (code < 0)
	    return code;
	}
	
	*ppte = penum;
  return 0;
}

int XWPSDevice::textBeginGX(XWPSImagerState * pis,
		    								XWPSTextParams *text, 
		    								XWPSFont * font,
		    								XWPSPath * path, 
		    								XWPSDeviceColor * pdcolor,
		    								XWPSClipPath * pcpath,
		    								XWPSTextEnum ** ppte)
{
	if (TEXT_PARAMS_ARE_INVALID(text))
		return (int)(XWPSError::RangeCheck);
  
  {
		XWPSPath *tpath = ((text->operation & TEXT_DO_NONE) &&
	     				!(text->operation & TEXT_RETURN_WIDTH) ? 0 : path);
		XWPSDeviceColor *tcolor = (text->operation & TEXT_DO_DRAW ? pdcolor : 0);
		XWPSClipPath *tcpath =   (text->operation & TEXT_DO_DRAW ? pcpath : 0);
		return textBegin(pis, text, font, tpath, tcolor, tcpath, ppte);
  }
}

int XWPSDevice::tileRectangle(XWPSTileBitmap *tile, 
	                          int x, 
	                          int y, 
	                          int widthA, 
	                          int heightA,
                            ulong color0, 
                            ulong color1,
                            int phase_x, 
                            int phase_y)
{
	return (this->*(procs.tile_rectangle_.tile_rectangle))(tile, x, y,widthA, heightA, color0, color1, phase_x, phase_y);
}

int XWPSDevice::tileRectangleDefault(XWPSTileBitmap *tile, 
	                            int x, 
	                            int y, 
	                            int w, 
	                            int h,
                              ulong color0, 
                              ulong color1,
                              int phase_x, 
                              int phase_y)
{
	XWPSStripBitmap tiles;
	tiles.data = tile->data;
	tiles.raster = tile->raster;
	tiles.size = tile->size;
	tiles.id = tile->id;
	tiles.rep_width = tile->rep_width;
	tiles.rep_height = tile->rep_height;
  return stripTileRectangle(&tiles, x, y, w, h, color0, color1, phase_x, phase_y);
}

ulong XWPSDevice::getWhite()
{
	if (cached_colors.white == ps_no_color_index)
		cached_colors.white = mapRGBColor(ps_max_color_value, ps_max_color_value, ps_max_color_value);
	return cached_colors.white;
}

#define al_x_at_y(alp, yv)\
  ((yv) == (alp)->end.y ? (alp)->end.x :\
   ((yv) <= (alp)->y_fast_max ?\
    ADD_NUM_ADJUST(((yv) - (alp)->start.y) * al_dx(alp), alp) / al_dy(alp) :\
    (\
     fixed_mult_quo(al_dx(alp), (yv) - (alp)->start.y, al_dy(alp)))) +\
   (alp)->start.x)
   
#define al_dx(alp) ((alp)->diff.x)
#define al_dy(alp) ((alp)->diff.y)

#define loop_fill_rectangle(x, y, w, h)\
  pdevc->fillRectangle(x, y, w, h, this, lop,0)
#define loop_fill_rectangle_direct(x, y, w, h)\
  (fill_direct ?\
   fillRectangle(x, y, w, h, cindex) :\
   pdevc->fillRectangle(x, y, w, h , this, lop,0))
   
#define loop_fill_trapezoid_fixed(fx0, fw0, fy0, fx1, fw1, fh)\
  loopFillTrap(fx0, fw0, fy0, fx1, fw1, fh, pbox, pdevc, lop)
   
int XWPSDevice::fillLoopByScanLines(XWPSLineList* ll, 
	       									XWPSFillParams * params, 
	       									XWPSDeviceColor * pdevc,
		     									ulong lop, 
		     									XWPSFixedRect * pbox,
													long adjust_left, 
													long adjust_right,
		                      long adjust_below, 
		                      long adjust_above, 
		                      long )
{
	int rule = params->rule;
  long fixed_flat = float2fixed(params->flatness);
  bool fill_direct = pdevc->writePure(lop);
  ulong cindex;
  
  XWPSActiveLine *yll = ll->y_list;
  long y_limit = pbox->q.y;
  long y;
  
  long look_below =  (adjust_above == fixed_0 ? fixed_0 : adjust_above - fixed_epsilon);
  long look_above =  adjust_below + fixed_epsilon;
  long look_height = look_above + look_below;
  bool do_adjust = look_height > fixed_epsilon;
  
  if (yll == 0)
  	return 0;
  	
  if (fill_direct)
		cindex = pdevc->pureColor();
		
#define next_pixel_center(y)\
  (fixed_pixround(y) + fixed_half)
  
  y = next_pixel_center(yll->start.y) - look_below;
  ll->x_head.next = 0;
  ll->x_head.x_current = min_fixed;
  while (1)
  {
  	XWPSActiveLine *alp, *nlp;
		long x;
		long ya = y + look_height;
		while (yll != 0 && yll->start.y < ya)
		{
			XWPSActiveLine *ynext = yll->next;
			if (yll->direction == dir_horizontal)
			{}
			else
			{
				ll->insertXNew(yll);
				yll->setScanLinePoints(fixed_flat);
			}
			yll = ynext;
		}
		
		if (y >= y_limit)
	    break;
		if (ll->x_head.next == 0)
		{
			if (yll == 0)
				break;
	    y = next_pixel_center(yll->start.y) - look_below;
	    continue;
		}
		
		x = min_fixed;
		for (alp = ll->x_head.next; alp != 0; alp = nlp)
		{
			long nx;

	    nlp = alp->next;
e:
	    if (alp->end.y <= y) 
	    {
		    if (alp->end_x_line())
		      continue;
		    alp->setScanLinePoints(fixed_flat);
		    goto e;
	    }
	    
	    nx = alp->x_current =	(alp->start.y >= y ? alp->start.x : alp->curve_k < 0 ? al_x_at_y(alp, y) : alp->cursor.curveXAtY(y));
	    if (nx < x)
	    {
	    	XWPSActiveLine *ilp = alp;
	    	while (nx < (ilp = ilp->prev)->x_current);
				alp->prev->next = alp->next;
				if (alp->next)
		    	alp->next->prev = alp->prev;
				if (ilp->next)
		    	ilp->next->prev = alp;
				alp->next = ilp->next;
				ilp->next = alp;
				alp->prev = ilp;
				continue;
	    }
	    x = nx;
		}
		
		{
			int inside = 0;
	    int x1_prev = min_int;
	    
#define inside_path_p() ((inside & rule) != 0)

			for (alp = ll->x_head.next; alp != 0; alp = alp->next)
			{
				int x0 = fixed2int_pixround(alp->x_current - adjust_left);
				if (do_adjust && alp->end.x < alp->start.x) 
				{
		    	long xa = (alp->end.y < ya ? alp->end.x :	alp->curve_k < 0 ?	al_x_at_y(alp, ya) : alp->cursor.curveXAtY(ya));
		    	int x0a = fixed2int_pixround(xa - adjust_left);
		    	if (x0a < x0)
						x0 = x0a;
				}
				
				for (;;)
				{
					inside += alp->direction;
		    	if (!inside_path_p())
						break;
						
					if ((alp = alp->next) == 0)
						goto out;
				}
				
#undef inside_path_p

        if (x0 < x1_prev)
		    	x0 = x1_prev;
				{
		    	int x1 = fixed2int_rounded(alp->x_current + adjust_right);

		    	if (do_adjust && alp->end.x > alp->start.x) 
		    	{
						long xa = (alp->end.y < ya ?  alp->end.x :  alp->curve_k < 0 ?  al_x_at_y(alp, ya) :  alp->cursor.curveXAtY(ya));
						int x1a = fixed2int_rounded(xa + adjust_right);

						if (x1a > x1)
			    		x1 = x1a;
		    	}
		    	if (x1 > x0) 
		    	{
						int code = loop_fill_rectangle_direct(x0, fixed2int_var(y), x1 - x0, 1);
						if (code < 0)
			    		return code;
						x1_prev = x1;
		    	}
				}
			}
			
out:
			;
		}
		y += fixed_1;
  }
  
  return 0;
}

int XWPSDevice::fillLoopByTrapezoids(XWPSLineList * ll,
	       									 XWPSFillParams * params, 
	       									 XWPSDeviceColor * pdevc,
		     									 ulong lop, 
		     									 XWPSFixedRect * pbox,
			                     long adjust_left, 
			                     long adjust_right,
		                       long adjust_below, 
		                       long adjust_above, 
		                       long band_mask)
{
	int rule = params->rule;
  const long y_limit = pbox->q.y;
  XWPSActiveLine *yll = ll->y_list;
  long y;
  int code;
  bool fill_direct = pdevc->writePure(lop);
  ulong cindex;
  
  long y_span_delta = _fixed_pixround_v + adjust_above;
  long y_span_limit = adjust_below + adjust_above;

#define adjusted_y_spans_pixel(y)\
  fixed_fraction((y) + y_span_delta) < y_span_limit

  if (yll == 0)
		return 0;
		
	if (fill_direct)
		cindex = pdevc->pureColor();
		
	y = yll->start.y;
  ll->x_head.next = 0;
  ll->x_head.x_current = min_fixed;
  while (1)
  {
  	long y1;
		XWPSActiveLine *endp, *alp, *stopx;
		long x;
		int draw;
		
		while (yll != 0 && yll->start.y == y)
		{
			XWPSActiveLine *ynext = yll->next;
			if (yll->direction == dir_horizontal)
			{
				int yi = fixed2int_pixround(y - adjust_below);
				int xi, wi;

				if (yll->start.x <= yll->end.x)
		    	xi = fixed2int_pixround(yll->start.x - adjust_left), wi = fixed2int_pixround(yll->end.x + adjust_right) - xi;
				else
		    	xi = fixed2int_pixround(yll->end.x - adjust_left), wi = fixed2int_pixround(yll->start.x + adjust_right) - xi;
				code = pdevc->fillRectangle(xi, yi, wi, 1, this, lop, 0);
				if (code < 0)
		    	return code;
			}
			else
				ll->insertXNew(yll);
				
			yll = ynext;
		}
		
		if (y >= y_limit)
	    break;
	    
		if (ll->x_head.next == 0)
		{
			if (yll == 0)
				break;
	    y = yll->start.y;
	    continue;
		}
		
		y1 = (yll != 0 ? yll->start.y : y_limit);
			
		{
	    long y_band = y | ~band_mask;

	    if (y1 > y_band)
				y1 = y_band + 1;
		}
		for (alp = ll->x_head.next; alp != 0; alp = alp->next)
	    if (alp->end.y < y1)
				y1 = alp->end.y;
				
		x = min_fixed;
#define have_pixels()\
  (fixed_pixround(y - adjust_below) < fixed_pixround(y1 + adjust_above))
		
		draw = (have_pixels()? 1 : -1);
			
		for (alp = stopx = ll->x_head.next; alp != 0; endp = alp, alp = alp->next)
		{
			long nx = al_x_at_y(alp, y1);
	    long dx_old, dx_den;
	    
	    if (nx >= x)
				x = nx;
			else if (draw >= 0 &&
		           (dx_old = alp->x_current - endp->x_current) >= 0 &&
		           (dx_den = dx_old + endp->x_next - nx) > dx_old)
		  {
		  	long dy = y1 - y, y_new;
		  	y_new = ((dy | dx_old) < 1L << (sizeof(long) * 4 - 1) ? dy * dx_old / dx_den :
		     				(fixed_mult_quo(dy, dx_old, dx_den))) + y;
		    stopx = alp;
				if (y_new <= y)
				{
					long ys, xs0, xs1, ye, xe0, xe1, dy, dx0, dx1;
					if (endp->start.y < alp->start.y)
						ys = alp->start.y, xs0 = al_x_at_y(endp, ys), xs1 = alp->start.x;
		    	else
						ys = endp->start.y, xs0 = endp->start.x, xs1 = al_x_at_y(alp, ys);
		    	if (endp->end.y > alp->end.y)
						ye = alp->end.y, xe0 = al_x_at_y(endp, ye), xe1 = alp->end.x;
		    	else
						ye = endp->end.y, xe0 = endp->end.x, xe1 = al_x_at_y(alp, ye);
		    	dy = ye - ys;
		    	dx0 = xe0 - xs0;
		    	dx1 = xe1 - xs1;
		    	if (dx0 == dx1)
		    	{
		    		y_new = y1;
		    	}
		    	else
		    	{
		    		double cross = (double)(xs0 - xs1) / (dx1 - dx0);

						y_new = (long)(ys + cross * dy);
						if (y_new <= y) 
						{
			    		y_new = y + fixed_epsilon;
						}
		    	}
				}
				
				if (y_new < y1) 
				{
		    	y1 = y_new;
		    	nx = al_x_at_y(alp, y1);
		    	draw = 0;
				}
				if (nx > x)
		    	x = nx;
		  }
		  alp->x_next = nx;
		}
		
		for (alp = ll->x_head.next; alp != stopx; alp = alp->next)
	    alp->x_next = al_x_at_y(alp, y1);
	    
	  if (draw > 0 || (draw == 0 && have_pixels()))
	  {
	  	long heightA = y1 - y;
	    long xlbot, xltop;
	    int inside = 0;
	    XWPSActiveLine *nlp;
	    for (x = min_fixed, alp = ll->x_head.next; alp != 0; alp = nlp)
	    {
	    	long xbot = alp->x_current;
				long xtop = alp->x_current = alp->x_next;

#define nx xtop
				long wtop;
				int xi, xli;
				int code;
				nlp = alp->next;
				if (alp->end.y != y1 || !alp->end_x_line()) 
				{
		    	if (nx <= x)
						alp->resortXLine();
		    	else
						x = nx;
				}
#undef nx
        
#define inside_path_p() ((inside & rule) != 0)

        if (!inside_path_p())
        {
        	inside += alp->direction;
		    	if (inside_path_p())
		    		xlbot = xbot, xltop = xtop;
		    	continue;
        }
        
        inside += alp->direction;
				if (inside_path_p())
					continue;
#undef inside_path_p

        wtop = xtop - xltop;
        if (xtop < xltop) 
        {
		    	xtop = xltop += arith_rshift(wtop, 1);
		    	wtop = 0;
				}
				
				if ((adjust_left | adjust_right) != 0) 
				{
		    	xlbot -= adjust_left;
		    	xbot += adjust_right;
		    	xltop -= adjust_left;
		    	xtop += adjust_right;
		    	wtop = xtop - xltop;
				}
				
				if ((xli = fixed2int_var_pixround(xltop)) ==
		    		fixed2int_var_pixround(xlbot) &&
		    		(xi = fixed2int_var_pixround(xtop)) ==
		    		fixed2int_var_pixround(xbot))
		    {
		    	int yi = fixed2int_pixround(y - adjust_below);
		    	int wi = fixed2int_pixround(y1 + adjust_above) - yi;

		    	code = loop_fill_rectangle_direct(xli, yi, xi - xli, wi);
		    }
		    else if ((adjust_below | adjust_above) != 0)
		    {
		    	long wbot = xbot - xlbot;
		    	if (xltop <= xlbot)
		    	{
		    		if (xtop >= xbot)
		    		{
		    			code = loop_fill_trapezoid_fixed(xlbot, wbot, y - adjust_below, xltop, wtop, heightA);
			    		if (adjusted_y_spans_pixel(y1)) 
			    		{
								if (code < 0)
				    			return code;
								code = loop_fill_rectangle_direct(xli, fixed2int_pixround(y1 - adjust_below), fixed2int_var_pixround(xtop) - xli, 1);
			    		}
		    		}
		    		else
		    		{
		    			code = fillSlantAdjust(xlbot, xbot, y, xltop, xtop, heightA, adjust_below, adjust_above, pbox, pdevc, lop);
		    		}
		    	}
		    	else
		    	{
		    		if (xtop <= xbot) 
		    		{
			    		if (adjusted_y_spans_pixel(y)) 
			    		{
								xli = fixed2int_var_pixround(xlbot);
								code = loop_fill_rectangle_direct(xli, fixed2int_pixround(y - adjust_below), fixed2int_var_pixround(xbot) - xli, 1);
								if (code < 0)
				    			return code;
			    		}
			    		code = loop_fill_trapezoid_fixed(xlbot, wbot, y + adjust_above, xltop, wtop, heightA);
						} 
						else 
						{
			    		code = fillSlantAdjust(xlbot, xbot, y, xltop, xtop, heightA, adjust_below, adjust_above, pbox, pdevc, lop);
						}
		    	}
		    }
		    else
		    	code = loop_fill_trapezoid_fixed(xlbot, xbot - xlbot, y, xltop, wtop, heightA);
				if (code < 0)
		    	return code;
	    }
	  }
	  else
	  {
	  	XWPSActiveLine *nlp;

	    for (x = min_fixed, alp = ll->x_head.next; alp != 0; alp = nlp) 
	    {
				long nx = alp->x_current = alp->x_next;

				nlp = alp->next;
				if (alp->end.y == y1) 
				{
		    	if (alp->end_x_line())
						continue;
				}
				if (nx <= x)
		    	alp->resortXLine();
				else
		    	x = nx;
	    }
	  }
	  
	  y = y1;
  }
  
  return 0;
}

int XWPSDevice::fillSlantAdjust(long xlbot, 
	                    long xbot, 
	                    long y,
		                  long xltop, 
		                  long xtop, 
		                  long heightA, 
		                  long adjust_below,
		                  long adjust_above, 
		                  XWPSFixedRect * pbox,
		  							  XWPSDeviceColor * pdevc,
		  							  ulong lop)
{
	long y1 = y + heightA;
	const long yb = y - adjust_below;
  const long ya = y + adjust_above;
  const long y1b = y1 - adjust_below;
  const long y1a = y1 + adjust_above;
  XWPSFixedEdge *plbot;
  XWPSFixedEdge *prbot;
  XWPSFixedEdge *pltop;
  XWPSFixedEdge *prtop;
  XWPSFixedEdge vert_left, slant_left, vert_right, slant_right;
  int code;
  
  if (xlbot < xltop)
  {
  	vert_left.start.x = vert_left.end.x = xlbot;
		vert_left.start.y = yb, vert_left.end.y = ya;
		vert_right.start.x = vert_right.end.x = xtop;
		vert_right.start.y = y1b, vert_right.end.y = y1a;
		slant_left.start.y = ya, slant_left.end.y = y1a;
		slant_right.start.y = yb, slant_right.end.y = y1b;
		plbot = &vert_left, prbot = &slant_right, pltop = &slant_left, prtop = &vert_right;
  }
  else
  {
  	vert_left.start.x = vert_left.end.x = xltop;
		vert_left.start.y = y1b, vert_left.end.y = y1a;
		vert_right.start.x = vert_right.end.x = xbot;
		vert_right.start.y = yb, vert_right.end.y = ya;
		slant_left.start.y = yb, slant_left.end.y = y1b;
		slant_right.start.y = ya, slant_right.end.y = y1a;
		plbot = &slant_left, prbot = &vert_right, pltop = &vert_left, prtop = &slant_right;
  }
  
  slant_left.start.x = xlbot, slant_left.end.x = xltop;
  slant_right.start.x = xbot, slant_right.end.x = xtop;
  
  if (ya >= y1b)
  {
  	int iyb = fixed2int_var_pixround(yb);
		int iya = fixed2int_var_pixround(ya);
		int iy1b = fixed2int_var_pixround(y1b);
		int iy1a = fixed2int_var_pixround(y1a);
		if (iy1b > iyb) 
		{
	    code = fillTrapezoid(plbot, prbot, yb, y1b, false, pdevc, lop);
	    if (code < 0)
				return code;
		}
		if (iya > iy1b) 
		{
	    int ix = fixed2int_var_pixround(vert_left.start.x);
	    int iw = fixed2int_var_pixround(vert_right.start.x) - ix;

	    code = pdevc->fillRectangle(ix, iy1b, iw, iya - iy1b, this, lop, 0);
	    if (code < 0)
				return code;
		}
		if (iy1a > iya)
	    code = fillTrapezoid(pltop, prtop, ya, y1a, false, pdevc, lop);
		else
	    code = 0;
  }
  else
  {
  	long yac;

		if (pbox->p.y < ya) 
		{
	    code = fillTrapezoid(plbot, prbot, yb, ya, false, pdevc, lop);
	    if (code < 0)
				return code;
	    yac = ya;
		} 
		else
	    yac = pbox->p.y;
		if (pbox->q.y > y1b) 
		{
	    code = fillTrapezoid(&slant_left, &slant_right, yac, y1b, false, pdevc, lop);
	    if (code < 0)
				return code;
	    code = fillTrapezoid(pltop, prtop, y1b, y1a, false, pdevc, lop);
		} 
		else
	    code = fillTrapezoid(&slant_left, &slant_right, yac, pbox->q.y, false, pdevc, lop);
  }
  
  return code;
}

int XWPSDevice::finishOutputPage(int num_copies, int )
{
	PageCount += num_copies;
  return 0;
}

void XWPSDevice::getBitsCopyCMYK1Bit(uchar *dest_line, 
	                         uint dest_raster,
			                     const uchar *src_line, 
			                     uint src_raster,
			                     int src_bit, 
			                     int w, 
			                     int h)
{
	for (; h > 0; dest_line += dest_raster, src_line += src_raster, --h) 
	{
		const uchar *src = src_line;
		uchar *dest = dest_line;
		bool hi = (src_bit & 4) != 0; 

		for (int i = w; i > 0; dest += 3, --i) 
		{
	    uint pixel = ((hi = !hi)? *src >> 4 : *src++ & 0xf);

	    if (pixel & 1)
				dest[0] = dest[1] = dest[2] = 0;
	    else 
	    {
				dest[0] = (uchar)((pixel >> 3) - 1);
				dest[1] = (uchar)(((pixel >> 2) & 1) - 1);
				dest[2] = (uchar)(((pixel >> 1) & 1) - 1);
	    }
		}
  }
}

int  XWPSDevice::getBitsNativeToStd(int x, 
	                        int w, 
	                        int h,
			                    XWPSGetBitsParams * params,
			                    XWPSGetBitsParams *,
			                    const uchar * src_base, 
			                    uint dev_raster,
			                    int x_offset, 
			                    uint raster, 
			                    uint std_raster)
{
	int depth = color_info.depth;
  int src_bit_offset = x * depth;
  const uchar *src_line = src_base + (src_bit_offset >> 3);
  ulong options = params->options;
  int ncomp =	(options & (GB_ALPHA_FIRST | GB_ALPHA_LAST) ? 4 : 3);
  uchar *dest_line = params->data[0] + x_offset * ncomp;
  uchar *mapped[16];
  int dest_bytes;
  
  if (!(options & GB_DEPTH_8))
  	return (int)(XWPSError::RangeCheck);
  
  if (options & GB_COLORS_RGB)
		params->options = options &= ~GB_COLORS_STANDARD_ALL | GB_COLORS_RGB, dest_bytes = 3;
  else if (options & GB_COLORS_CMYK)
		params->options = options &= ~GB_COLORS_STANDARD_ALL | GB_COLORS_CMYK, dest_bytes = 4;
  else if (options & GB_COLORS_GRAY)
		params->options = options &= ~GB_COLORS_STANDARD_ALL | GB_COLORS_GRAY, dest_bytes = 1;
  else
		return (int)(XWPSError::RangeCheck);
			
	if (options & GB_RASTER_STANDARD) 
	{
		uint end_byte = (x_offset + w) * dest_bytes;

		raster = std_raster =  (options & GB_ALIGN_STANDARD ?  bitmap_raster(end_byte << 3) : end_byte);
  }
  
  if (options & (GB_ALPHA_FIRST | GB_ALPHA_LAST))
		++dest_bytes;
		
	for (int i = (depth > 4 ? 16 : 1 << depth); --i >= 0; )
		mapped[i] = 0;
  for (; h > 0; dest_line += raster, src_line += dev_raster, --h)
  {
  	sample_load_declare_setup(src, bit, src_line, src_bit_offset & 7, depth);
		uchar *dest = dest_line;
		
		for (int i = 0; i < w; ++i)
		{
			ulong pixel = 0;
	    ushort rgba[4];

	    sample_load_next32(pixel, src, bit, depth);
	    if (pixel < 16)
	    {
	    	if (mapped[pixel]) 
	    	{
		    	memcpy(dest, mapped[pixel], dest_bytes);
		    	dest += dest_bytes;
		    	continue;
				}
				mapped[pixel] = dest;
	    }
	    
	    mapColorRGBAlpha(pixel, rgba);
	    if (options & GB_ALPHA_FIRST)
				*dest++ = ps_color_value_to_byte(rgba[3]);
				
			if (options & GB_COLORS_RGB) 
			{
				dest[0] = ps_color_value_to_byte(rgba[0]);
				dest[1] = ps_color_value_to_byte(rgba[1]);
				dest[2] = ps_color_value_to_byte(rgba[2]);
				dest += 3;
	    } 
	    else if (options & GB_COLORS_CMYK)
	    {
	    	ushort white = qMax(rgba[0], qMax(rgba[1], rgba[2]));

				dest[0] = ps_color_value_to_byte(white - rgba[0]);
				dest[1] = ps_color_value_to_byte(white - rgba[1]);
				dest[2] = ps_color_value_to_byte(white - rgba[2]);
				dest[3] = ps_color_value_to_byte(ps_max_color_value - white);
				dest += 4;
	    }
	    else
	    {
	    	*dest++ = ps_color_value_to_byte(((rgba[0] * (ulong) lum_red_weight) +
				 																	(rgba[1] * (ulong) lum_green_weight) +
				 																	(rgba[2] * (ulong) lum_blue_weight) +
				   																(lum_all_weights / 2))/ lum_all_weights);
	    }
	    
	    if (options & GB_ALPHA_LAST)
				*dest++ = ps_color_value_to_byte(rgba[3]);
		}
  }
  
  return 0;
}

int  XWPSDevice::getBitsStdToNative(int x, 
	                        int w, 
	                        int h,
			                    XWPSGetBitsParams * params,
			                    XWPSGetBitsParams *stored,
			                    const uchar * src_base, 
			                    uint dev_raster,
			                    int x_offset, 
			                    uint raster)
{
	int depth = color_info.depth;
  int dest_bit_offset = x_offset * depth;
  uchar *dest_line = params->data[0] + (dest_bit_offset >> 3);
  int ncolors =	(stored->options & GB_COLORS_RGB ? 3 :
	 							stored->options & GB_COLORS_CMYK ? 4 :
	 							stored->options & GB_COLORS_GRAY ? 1 : -1);
  int ncomp = ncolors +	((stored->options & (GB_ALPHA_FIRST | GB_ALPHA_LAST)) != 0);
  int src_depth = GB_OPTIONS_DEPTH(stored->options);
  int src_bit_offset = x * src_depth * ncomp;
  const uchar *src_line = src_base + (src_bit_offset >> 3);
  ushort src_max = (1 << src_depth) - 1;
  
#define v2cv(value) ((ulong)(value) * ps_max_color_value / src_max)
 
  ushort alpha_default = src_max;

  params->options &= ~GB_COLORS_ALL | GB_COLORS_NATIVE;
  for (; h > 0; dest_line += raster, src_line += dev_raster, --h) 
  {
		sample_load_declare_setup(src, sbit, src_line, src_bit_offset & 7, src_depth);
		sample_store_declare_setup(dest, dbit, dbyte, dest_line, dest_bit_offset & 7, depth);

		for (int i = 0; i < w; ++i) 
		{
	    ushort v[4], va = alpha_default;
	    ulong pixel;

	    if (stored->options & GB_ALPHA_FIRST) 
	    {
				sample_load_next16(va, src, sbit, src_depth);
				va = v2cv(va);
	    }
	    for (int j = 0; j < ncolors; ++j) 
	    {
				ushort vj;

				sample_load_next16(vj, src, sbit, src_depth);
				v[j] = v2cv(vj);
	    }
	    
	    if (stored->options & GB_ALPHA_LAST) 
	    {
				sample_load_next16(va, src, sbit, src_depth);
				va = v2cv(va);
	    }
	    
	    switch (ncolors) 
	    {
	    	case 1:
					v[2] = v[1] = v[0];
	    	case 3:
					pixel = mapRGBAlphaColor(v[0], v[1], v[2], va);
					break;
					
	    	case 4:
					pixel = mapCMYKColor(v[0], v[1], v[2], v[3]);
					break;
					
	    	default:
					return (int)(XWPSError::RangeCheck);
	    }
	    sample_store_next32(pixel, dest, dbit, depth, dbyte);
		}
		sample_store_flush(dest, dbit, depth, dbyte);
  }
  return 0;
}

int XWPSDevice::loopFillTrap(long fx0, 
	                 long fw0, 
	                 long fy0,
	                 long fx1, 
	                 long fw1, 
	                 long fh, 
	                 XWPSFixedRect * pbox,
	                 XWPSDeviceColor * pdevc, 
	                 ulong lop)
{
	long fy1 = fy0 + fh;
  long ybot = qMax(fy0, pbox->p.y);
  long ytop = qMin(fy1, pbox->q.y);
  XWPSFixedEdge left, right;

  if (ybot >= ytop)
		return 0;
  left.start.y = right.start.y = fy0;
  left.end.y = right.end.y = fy1;
  right.start.x = (left.start.x = fx0) + fw0;
  right.end.x = (left.end.x = fx1) + fw1;
  return fillTrapezoid(&left, &right, ybot, ytop, false, pdevc, lop);
}

bool XWPSDevice::paramHWColorMap(uchar * palette)
{
	int depth = color_info.depth;
  int colors = color_info.num_components;

  if (depth <= 8 && colors <= 3) 
  {
		uchar *p = palette;
		ushort rgb[3];

		for (int i = 0; (i >> depth) == 0; i++) 
		{
	    if (mapColorRGB(i, rgb) < 0)
				return false;
	    for (int j = 0; j < colors; j++)
				*p++ = ps_color_value_to_byte(rgb[j]);
		}
		return true;
  }
  return false;
}

bool XWPSDevice::requestedIncludesStored(XWPSGetBitsParams *requested,
			                         XWPSGetBitsParams *stored)
{
	ulong both = requested->options & stored->options;

  if (!(both & GB_PACKING_ALL))
		return false;
  
  if (stored->options & GB_SELECT_PLANES)
  {
  	int n = (stored->options & GB_PACKING_BIT_PLANAR ? color_info.depth : color_info.num_components);

		if (!(requested->options & GB_SELECT_PLANES) || !(both & (GB_PACKING_PLANAR || GB_PACKING_BIT_PLANAR)))
	    return false;
		for (int i = 0; i < n; ++i)
	    if (requested->data[i] && !stored->data[i])
				return false;
  }
  
  if (both & GB_COLORS_NATIVE)
		return true;
		
  if (both & GB_COLORS_STANDARD_ALL) 
  {
		if ((both & GB_ALPHA_ALL) && (both & GB_DEPTH_ALL))
	    return true;
  }
  return false;
}

int XWPSDevice::strokeAdd(XWPSPath * ppath, 
	                        int first, 
	                        XWPSPartialLine * plp, 
	                        XWPSPartialLine * nplp,
	   					            XWPSDeviceColor * ,
	   						          XWPSImagerState * pis, 
	   						          XWPSStrokeParams * ,
	   					            XWPSFixedRect * , 
	   					            int uniform, 
	   					            PSLineJoin join)
{
	const XWPSLineParams *pgs_lp = pis->currentLineParams();
  XWPSFixedPoint points[8];
  int npoints;
  int code;
  bool moveto_first = true;
  if (plp->thin)
  {
  	plp->set_thin_widths();
		plp->adjust_stroke(pis, true);
		plp->compute_caps();
  }
  
  if (first == 0 && pgs_lp->cap == ps_cap_round)
  {
  	if ((code = ppath->addPoint(plp->o.co.x, plp->o.co.y)) < 0 ||
	    	(code = plp->o.add_round_cap(ppath)) < 0)
	    return code;
		npoints = 0;
		moveto_first = false;
  }
  else
  {
  	if ((npoints = plp->o.cap_points((first == 0 ? pgs_lp->cap : ps_cap_butt), points)) < 0)
	    return npoints;
  }
  
  XWPSMatrix ctm;
  pis->ctmOnly(&ctm);
  
  if (nplp == 0)
  {
  	if (pgs_lp->cap == ps_cap_round) 
  	{
  		points[npoints].x = plp->e.co.x;
  		points[npoints].y = plp->e.co.y;
	    ++npoints;
	    if ((code = ppath->addPoints(points, npoints, moveto_first)) < 0)
				return code;
	    code = plp->e.add_round_cap(ppath);
	    goto done;
		}
		code = plp->e.cap_points(pgs_lp->cap, points + npoints);
  }
  else if (join == ps_join_round)
  {
  	points[npoints].x = plp->e.co.x;
  	points[npoints].y = plp->e.co.y;
		++npoints;
		if ((code = ppath->addPoints(points, npoints, moveto_first)) < 0)
	    return code;
		code = plp->e.add_round_cap(ppath);
		goto done;
  }
  else if (nplp->thin)
  	code = plp->e.cap_points(ps_cap_butt, points + npoints);
  else
		code = nplp->line_join_points(pgs_lp, plp, points + npoints,(uniform ? 0 : &ctm),join);
  if (code < 0)
		return code;
  code = ppath->addPoints(points, npoints + code, moveto_first);
  
done:
  if (code < 0)
		return code;
  return ppath->closeSubpathNotes(sn_none);
}

int XWPSDevice::strokeFill(XWPSPath * ppath, 
	               int first, 
	               XWPSPartialLine * plp, 
	               XWPSPartialLine * nplp,
	    					 XWPSDeviceColor * pdevc, 
	    				   XWPSImagerState * pis, 
	    				   XWPSStrokeParams * params,
	    					 XWPSFixedRect * pbbox, 
	    					 int uniform, 
	    					 PSLineJoin join)
{
	const long lix = plp->o.p.x;
  const long liy = plp->o.p.y;
  const long litox = plp->e.p.x;
  const long litoy = plp->e.p.y;
  
  if (plp->thin)
  	return drawThinLine(lix, liy, litox, litoy,	pdevc, pis->log_op);
  
  {
  	XWPSMatrix ctm;
  	pis->ctmOnly(&ctm);
  	XWPSLineParams *pgs_lp = pis->currentLineParams();
		PSLineCap cap = pgs_lp->cap;

		if (!plp->thin && (nplp == 0 || !nplp->thin) 	&& 
			  ((first != 0 && nplp != 0) || cap == ps_cap_butt	|| cap == ps_cap_square)  && 
			  (join == ps_join_bevel || join == ps_join_miter || join == ps_join_none)  && 
			  (pis->fill_adjust.x | pis->fill_adjust.y) == 0  && 
			  lop_is_idempotent(pis->log_op))
		{
			XWPSFixedPoint points[6];
	    int npoints, code;
	    long ax, ay, bx, by;

	    npoints = plp->o.cap_points((first == 0 ? cap : ps_cap_butt), points);
	    if (nplp == 0)
				code = plp->e.cap_points(cap, points + npoints);
	    else
				code = nplp->line_join_points(pgs_lp, plp, points + npoints,(uniform ? 0 : &ctm), join);
	    if (code < 0)
				goto general;
				
#define SUB_OVERFLOWS(r, u, v)\
  (((r = u - v) ^ u) < 0 && (u ^ v) < 0)
	    if (SUB_OVERFLOWS(ax, points[0].x, points[1].x) ||
					SUB_OVERFLOWS(ay, points[0].y, points[1].y) ||
					SUB_OVERFLOWS(bx, points[2].x, points[1].x) ||
					SUB_OVERFLOWS(by, points[2].y, points[1].y))
					goto general;
#undef SUB_OVERFLOWS
			if (nplp != 0)
			{
				if (join == ps_join_miter)
				{
					if (!(points[2].x == plp->e.co.x &&
			  			points[2].y == plp->e.co.y &&
			  			points[5].x == plp->e.ce.x &&
			  			points[5].y == plp->e.ce.y))
						goto fill;
				}
				
				{
					const XWPSFixedPoint *bevel = points + 2;
		    	if (points[3].x == nplp->o.p.x &&	points[3].y == nplp->o.p.y)
						++bevel;
						
		    	code = fillTriangle(bevel->x, bevel->y,
			       									bevel[1].x - bevel->x, bevel[1].y - bevel->y,
			       									bevel[2].x - bevel->x, bevel[2].y - bevel->y,
															pdevc, pis->log_op);
		    	if (code < 0)
						return code;
				}
			}
			
			return fillParallelogram(points[1].x, points[1].y, ax, ay, bx, by, pdevc, pis->log_op);
			
fill:
	    code = ppath->addPoints(points, npoints + code, true);
	    if (code < 0)
				return code;
	    return ppath->closeSubpathNotes(sn_none);
		}
  }
  
general:
  return strokeAdd(ppath, first, plp, nplp, pdevc, pis, params, pbbox, uniform, join);
}

XWPSDeviceHit::XWPSDeviceHit(QObject * parent)
	:XWPSDevice("hit detector", 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, parent)
{
	procs.fill_rectangle_.fill_rectanglehit = &XWPSDeviceHit::fillRectangleHit;
	procs.get_clipping_box_.get_clipping_boxhit = &XWPSDevice::getClipingBoxLarge;
	procs.create_compositor_.create_compositorhit = &XWPSDevice::createCompositorNull;
}

int  XWPSDeviceHit::copyDevice(XWPSDevice **pnew)
{
	*pnew = new XWPSDeviceHit;
	return 0;
}

int XWPSDeviceHit::fillRectangleHit(int , int , int w, int h, ulong )
{
	if (w > 0 && h > 0)
	{
		return (int)(XWPSError::HitDetected);
	}
	
  return 0;
}

XWPSDeviceForward::XWPSDeviceForward(QObject * parent)
	:XWPSDevice(parent),
	target(0)
{
	procs.get_initial_matrix_.get_initial_matrixfw = &XWPSDeviceForward::getInitialMatrixForward;
	procs.sync_output_.sync_outputfw = &XWPSDeviceForward::syncOutputForward;
	procs.output_page_.output_pagefw = &XWPSDeviceForward::outputPageForward;
	procs.map_rgb_color_.map_rgb_colorfw = &XWPSDeviceForward::mapRGBColorForward;
	procs.map_color_rgb_.map_color_rgbfw = &XWPSDeviceForward::mapColorRGBForward;
	procs.get_bits_.get_bitsfw = &XWPSDeviceForward::getBitsForward;
	procs.get_params_.get_paramsfw = &XWPSDeviceForward::getParamsForward;
	procs.put_params_.put_paramsfw = &XWPSDeviceForward::putParamsForward;
	procs.map_cmyk_color_.map_cmyk_colorfw = &XWPSDeviceForward::mapCMYKColorForward;
	procs.get_xfont_device_.get_xfont_devicefw = &XWPSDeviceForward::getXFontDeviceForward;
	procs.map_rgb_alpha_color_.map_rgb_alpha_colorfw = &XWPSDeviceForward::mapRGBAlphaColorForward;
	procs.get_page_device_.get_page_devicefw = &XWPSDeviceForward::getPageDeviceForward;
	procs.get_band_.get_bandfw = &XWPSDeviceForward::getBandForward;
	procs.copy_rop_.copy_ropfw = &XWPSDeviceForward::copyRopForward;
	procs.fill_path_.fill_pathfw = &XWPSDeviceForward::fillPathForward;
	procs.stroke_path_.stroke_pathfw = &XWPSDeviceForward::strokePathForward;
	procs.fill_mask_.fill_maskfw = &XWPSDeviceForward::fillMaskForward;
	procs.fill_trapezoid_.fill_trapezoidfw = &XWPSDeviceForward::fillTrapezoidForward;
	procs.fill_parallelogram_.fill_parallelogramfw = &XWPSDeviceForward::fillParallelogramForward;
	procs.fill_triangle_.fill_trianglefw = &XWPSDeviceForward::fillTriangleForward;
	procs.draw_thin_line_.draw_thin_linefw = &XWPSDeviceForward::drawThinLineForward;
	procs.begin_image_.begin_imagefw = &XWPSDeviceForward::beginImageForward;
	procs.strip_copy_rop_.strip_copy_ropfw = &XWPSDeviceForward::stripCopyRopForward;
	procs.get_clipping_box_.get_clipping_boxfw = &XWPSDeviceForward::getClipingBoxForward;
	procs.begin_typed_image_.begin_typed_imagefw = &XWPSDeviceForward::beginTypedImageForward;
	procs.get_bits_rectangle_.get_bits_rectanglefw = &XWPSDeviceForward::getBitsRectangleForward;
	procs.map_color_rgb_alpha_.map_color_rgb_alphafw = &XWPSDeviceForward::mapColorRGBAlphaForward;
	procs.create_compositor_.create_compositorfw = &XWPSDevice::createCompositorNo;
	procs.get_hardware_params_.get_hardware_paramsfw = &XWPSDeviceForward::getParamsForward;
	procs.text_begin_.text_beginfw = &XWPSDeviceForward::textBeginForward;
}

XWPSDeviceForward::XWPSDeviceForward(const char * devname,
	                                   int w,
	                                   int h,
	                                   float xdpi,
	                                   float ydpi,
	                                   int nc,
	                                   int depth,
	                                   ushort mg, 
	                                   ushort mc,
	                                   ushort dg,
	                                   ushort dc,
	                                   QObject * parent)
	:XWPSDevice(devname, w, h, xdpi, ydpi, nc, depth, mg, mc, dg, dc, parent),
	 target(0)
{
	procs.get_initial_matrix_.get_initial_matrixfw = &XWPSDeviceForward::getInitialMatrixForward;
	procs.sync_output_.sync_outputfw = &XWPSDeviceForward::syncOutputForward;
	procs.output_page_.output_pagefw = &XWPSDeviceForward::outputPageForward;
	procs.map_rgb_color_.map_rgb_colorfw = &XWPSDeviceForward::mapRGBColorForward;
	procs.map_color_rgb_.map_color_rgbfw = &XWPSDeviceForward::mapColorRGBForward;
	procs.get_bits_.get_bitsfw = &XWPSDeviceForward::getBitsForward;
	procs.get_params_.get_paramsfw = &XWPSDeviceForward::getParamsForward;
	procs.put_params_.put_paramsfw = &XWPSDeviceForward::putParamsForward;
	procs.map_cmyk_color_.map_cmyk_colorfw = &XWPSDeviceForward::mapCMYKColorForward;
	procs.get_xfont_device_.get_xfont_devicefw = &XWPSDeviceForward::getXFontDeviceForward;
	procs.map_rgb_alpha_color_.map_rgb_alpha_colorfw = &XWPSDeviceForward::mapRGBAlphaColorForward;
	procs.get_page_device_.get_page_devicefw = &XWPSDeviceForward::getPageDeviceForward;
	procs.get_band_.get_bandfw = &XWPSDeviceForward::getBandForward;
	procs.copy_rop_.copy_ropfw = &XWPSDeviceForward::copyRopForward;
	procs.fill_path_.fill_pathfw = &XWPSDeviceForward::fillPathForward;
	procs.stroke_path_.stroke_pathfw = &XWPSDeviceForward::strokePathForward;
	procs.fill_mask_.fill_maskfw = &XWPSDeviceForward::fillMaskForward;
	procs.fill_trapezoid_.fill_trapezoidfw = &XWPSDeviceForward::fillTrapezoidForward;
	procs.fill_parallelogram_.fill_parallelogramfw = &XWPSDeviceForward::fillParallelogramForward;
	procs.fill_triangle_.fill_trianglefw = &XWPSDeviceForward::fillTriangleForward;
	procs.draw_thin_line_.draw_thin_linefw = &XWPSDeviceForward::drawThinLineForward;
	procs.begin_image_.begin_imagefw = &XWPSDeviceForward::beginImageForward;
	procs.strip_copy_rop_.strip_copy_ropfw = &XWPSDeviceForward::stripCopyRopForward;
	procs.get_clipping_box_.get_clipping_boxfw = &XWPSDeviceForward::getClipingBoxForward;
	procs.begin_typed_image_.begin_typed_imagefw = &XWPSDeviceForward::beginTypedImageForward;
	procs.get_bits_rectangle_.get_bits_rectanglefw = &XWPSDeviceForward::getBitsRectangleForward;
	procs.map_color_rgb_alpha_.map_color_rgb_alphafw = &XWPSDeviceForward::mapColorRGBAlphaForward;
	procs.create_compositor_.create_compositorfw = &XWPSDevice::createCompositorNo;
	procs.get_hardware_params_.get_hardware_paramsfw = &XWPSDeviceForward::getParamsForward;
	procs.text_begin_.text_beginfw = &XWPSDeviceForward::textBeginForward;
}

XWPSDeviceForward::~XWPSDeviceForward()
{
	if (target)
		if (target->decRef() == 0)
			delete target;
}

int XWPSDeviceForward::beginImageForward(XWPSImagerState *pis, 
	                       XWPSImage1 *pim,
	                       PSImageFormat format, 
	                       XWPSIntRect *prect,
	                       XWPSDeviceColor *pdcolor, 
	                       XWPSClipPath *pcpath,
	                       XWPSImageEnumCommon **pinfo)
{
	if (!target)
		return beginImageDefault(pis, pim, format, prect, pdcolor, pcpath, pinfo);
	    
  return target->beginImage(pis, pim, format, prect, pdcolor, pcpath, pinfo);
}

int XWPSDeviceForward::beginTypedImageForward(XWPSImagerState * pis,
			                        XWPSMatrix *pmat, 
			                        XWPSImageCommon *pic,
			   											XWPSIntRect * prect,
			                        XWPSDeviceColor * pdcolor,
			                        XWPSClipPath * pcpath, 
			                        XWPSImageEnumCommon ** pinfo)
{
	if (!target)
		return beginTypedImageDefault(pis, pmat, pic, prect, pdcolor, pcpath,pinfo);
	    
  return target->beginTypedImage(pis, pmat, pic, prect, pdcolor, pcpath,pinfo);
}

int  XWPSDeviceForward::copyDevice(XWPSDevice **pnew)
{
	*pnew = new XWPSDeviceForward(dname, width, height,HWResolution[0],HWResolution[1],color_info.num_components,color_info.depth,color_info.max_gray,color_info.max_color,color_info.dither_grays,color_info.dither_colors);
	((XWPSDeviceForward*)(*pnew))->target = target;
	return 0;
}

int XWPSDeviceForward::copyRopForward(const uchar * sdata, 
	                    int sourcex, 
	                    uint sraster, 
	                    ulong id,
		                  const ulong * scolors,
	                    XWPSTileBitmap * texture, 
	                    const ulong * tcolors,
		                  int x, 
		                  int y, 
		                  int widthA, 
		                  int heightA,
		                  int phase_x, 
		                  int phase_y, 
		                  ulong lop)
{
	if (!target)
		return copyRopDefault(sdata, sourcex, sraster, id, scolors,	texture, tcolors, x, y, widthA, heightA,phase_x, phase_y, lop);
	    
  return target->copyRop(sdata, sourcex, sraster, id, scolors,	texture, tcolors, x, y, widthA, heightA,phase_x, phase_y, lop);
}

int XWPSDeviceForward::drawThinLineForward(long fx0, 
                           long fy0, 
                           long fx1, 
                           long fy1,
                           XWPSDeviceColor *pdevc, 
                           ulong lop)
{
	if (!target)
		return drawThinLineDefault(fx0, fy0, fx1, fy1, pdevc, lop);
	    
  return target->drawThinLine(fx0, fy0, fx1, fy1, pdevc, lop);
}

int XWPSDeviceForward::fillMaskForward(const uchar * data, 
	                      int dx, 
	                      int raster, 
	                      ulong id,
		   									int x, 
		   									int y, 
		   									int w, 
		   									int h,
		   									XWPSDeviceColor * pdcolor, 
		   									int depth,
		   									ulong lop, 
		   									XWPSClipPath *pcpath)
{
	if (!target)
		return fillMaskDefault(data, dx, raster, id, x, y, w, h, pdcolor, depth,lop, pcpath);
	return target->fillMask(data, dx, raster, id, x, y, w, h, pdcolor, depth,lop, pcpath);
}

int  XWPSDeviceForward::fillParallelogramForward(long px, 
	                               long py, 
	                               long ax, 
	                               long ay, 
	                               long bx, 
	                               long by,
	                               XWPSDeviceColor *pdevc, 
	                               ulong lop)
{
	if (!target)
		return fillParallelogramDefault(px, py, ax, ay, bx, by, pdevc, lop);
	return target->fillParallelogram(px, py, ax, ay, bx, by, pdevc, lop);
}

int XWPSDeviceForward::fillPathForward(XWPSImagerState * pis, 
	                      XWPSPath * ppath,
		                    XWPSFillParams * params,
	                      XWPSDeviceColor * pdevc, 
	                      XWPSClipPath * pcpath)
{
	if (!target)
		return fillPathDefault(pis, ppath, params, pdevc, pcpath);
	return target->fillPath(pis, ppath, params, pdevc, pcpath);
}

int XWPSDeviceForward::fillTrapezoidForward(XWPSFixedEdge *left, 
	                          XWPSFixedEdge *right,
	                          long ybot, 
	                          long ytop, 
	                          bool swap_axes,
	                          XWPSDeviceColor *pdevc, 
	                          ulong lop)
{
	if (!target)
		return fillTrapezoidDefault(left, right, ybot, ytop, swap_axes, pdevc, lop);
	return target->fillTrapezoid(left, right, ybot, ytop, swap_axes, pdevc, lop);
}

int XWPSDeviceForward::fillTriangleForward(long px, 
	                         long py, 
	                         long ax, 
	                         long ay, 
	                         long bx, 
	                         long by,
	                         XWPSDeviceColor *pdevc, 
	                         ulong lop)
{
	if (!target)
		return fillTriangleDefault(px, py, ax, ay, bx, by, pdevc, lop);
	return target->fillTriangle(px, py, ax, ay, bx, by, pdevc, lop);
}

void XWPSDeviceForward::forwardColorProcs()
{
	procs.map_rgb_color_.map_rgb_colorfw = &XWPSDeviceForward::mapRGBColorForward;
	procs.map_color_rgb_.map_color_rgbfw = &XWPSDeviceForward::mapColorRGBForward;
	procs.map_cmyk_color_.map_cmyk_colorfw = &XWPSDeviceForward::mapCMYKColorForward;
	procs.map_rgb_alpha_color_.map_rgb_alpha_colorfw = &XWPSDeviceForward::mapRGBAlphaColorForward;
	procs.map_color_rgb_alpha_.map_color_rgb_alphafw = &XWPSDeviceForward::mapColorRGBAlphaForward;
}

int XWPSDeviceForward::getBandForward(int y, int *band_start)
{
	if (!target)
		return getBandDefault(y, band_start);
	
	return target->getBand(y, band_start);
}

int  XWPSDeviceForward::getBitsForward(int y, uchar * data, uchar ** actual_data)
{
	if (!target)
		return getBitsDefault(y, data, actual_data);
	
	return target->getBits(y, data, actual_data);
}

int  XWPSDeviceForward::getBitsRectangleForward(XWPSIntRect *prect,
	                             XWPSGetBitsParams *params, 
	                             XWPSIntRect **unread)
{
	if (!target)
		return getBitsRectangleDefault(prect, params, unread);
	
	return target->getBitsRectangle(prect, params, unread);
}

void XWPSDeviceForward::getClipingBoxForward(XWPSFixedRect*pbox)
{
	if (!target)
		return getClipingBoxDefault(pbox);
	
	return target->getClipingBox(pbox);
}

int XWPSDeviceForward:: getHardwareParamsForward(XWPSContextState * ctx, XWPSParamList *plist)
{
	if (!target)
		return getHardwareParamsDefault(ctx, plist);
			
	return target->getHardwareParams(ctx, plist);
}

void XWPSDeviceForward::getInitialMatrixForward(XWPSMatrix *pmat)
{
	if (!target)
		getInitialMatrixDefault(pmat);
	else
		target->getInitialMatrix(pmat);
}

XWPSDevice * XWPSDeviceForward::getPageDeviceForward()
{
	if (!target)
		return getPageDeviceDefault();
		
	XWPSDevice * ret = target->getPageDevice();
	return (ret == target ? this : ret);
}

int  XWPSDeviceForward::getParamsForward(XWPSContextState * ctx,XWPSParamList *plist)
{
	if (!target)
		return getParamsDefault(ctx,plist);
			
	return target->getParams(ctx,plist);
}

XWPSDevice * XWPSDeviceForward::getXFontDeviceForward()
{
	if (!target)
		return getXFontDeviceDefault();
			
	return target->getXFontDevice();
}

ulong XWPSDeviceForward::mapCMYKColorForward(ushort c, ushort m, ushort y, ushort k)
{
	if (!target)
		return mapCMYKColorDefault(c,m,y,k);
			
	return target->mapCMYKColor(c,m,y,k);
}

int XWPSDeviceForward::mapColorRGBForward(ulong color, ushort* prgb)
{
	if (!target)
		return mapColorRGBDefault(color,prgb);
			
	return target->mapColorRGB(color,prgb);
}

int XWPSDeviceForward::mapColorRGBAlphaForward(ulong color, ushort * prgba)
{
	if (!target)
		return mapColorRGBAlphaDefault(color, prgba);
			
	return target->mapColorRGBAlpha(color, prgba);
}

ulong XWPSDeviceForward::mapRGBAlphaColorForward(ushort r, ushort g, ushort b, ushort alpha)
{
	if (!target)
		return mapRGBAlphaColorDefault(r,g,b,alpha);
			
	return target->mapRGBAlphaColor(r,g,b,alpha);
}

ulong XWPSDeviceForward::mapRGBColorForward(ushort r, ushort g, ushort b)
{
	if (!target)
		return mapRGBColorDefault(r,g,b);
			
	return target->mapRGBColor(r,g,b);
}

int XWPSDeviceForward::outputPageForward(int num_copies, int flush)
{
	int code = 0;
	if (!target)
		code = outputPageDefault(num_copies, flush);
	else
		code = target->outputPage(num_copies, flush);
		
	if (code >= 0 && target != 0)
		PageCount = PageCount;
  return code;
}

int XWPSDeviceForward::putParamsForward(XWPSContextState * ctx,XWPSParamList *plist)
{
	if (!target)
		return putParams(ctx, plist);
			
	int code = target->putParamsDefault(ctx, plist);
  if (code >= 0)
		decacheColors();
  return code;
}

void XWPSDeviceForward::setTarget(XWPSDevice * t)
{
	if (target != t)
	{
		if (target)
			if (target->decRef() == 0)
				delete target;
		target = t;
		if (target)
			target->incRef();
	}
}

int XWPSDeviceForward::strokePathForward(XWPSImagerState * pis, 
	                       XWPSPath * ppath,
		                     XWPSStrokeParams * params,
	                       XWPSDeviceColor * pdcolor, 
	                       XWPSClipPath *pcpath)
{
	if (!target)
		return strokePathDefault(pis, ppath, params, pdcolor, pcpath);
			
	return target->strokePath(pis, ppath, params, pdcolor, pcpath);
}

int XWPSDeviceForward::stripCopyRopForward(const uchar *sdata, 
	                         int sourcex, 
	                         uint sraster, 
	                         ulong id,
	                         const ulong *scolors,
	                         XWPSStripBitmap *textures, 
	                         const ulong *tcolors,
	                         int x, 
	                         int y, 
	                         int widthA, 
	                         int heightA,
	                         int phase_x, 
	                         int phase_y, 
	                         ulong lop)
{
	if (!target)
		return stripCopyRopDefault(sdata, sourcex, sraster, id, scolors,
		                           textures, tcolors, x, y, widthA, heightA,
		                           phase_x, phase_y, lop);
			
	return target->stripCopyRop(sdata, sourcex, sraster, id, scolors,
		                           textures, tcolors, x, y, widthA, heightA,
		                           phase_x, phase_y, lop);
}

int XWPSDeviceForward::syncOutputForward()
{
	if (!target)
		return syncOutputDefault();
			
	return target->syncOutput();
}

int XWPSDeviceForward::textBeginForward(XWPSImagerState * pis,
		    								XWPSTextParams *text, 
		    								XWPSFont * font,
		    								XWPSPath * path, 
		    								XWPSDeviceColor * pdcolor,
		    								XWPSClipPath * pcpath,
		    								XWPSTextEnum ** ppte)
{
	if (!target)
		return textBeginDefault(pis, text, font, path, pdcolor, pcpath, ppte);
			
	return target->textBegin(pis, text, font, path, pdcolor, pcpath, ppte);
}

XWPSClipCallbackData::XWPSClipCallbackData(XWPSDevice * dev)
{
	tdev = dev;
	x = y = w = h = 0;
	color[0] = 0;
	color[1] = 0;
	data = 0;
	sourcex = 0;
	raster = 0;
	depth = 0;
	pdcolor= 0;
	lop = 0;
	pcpath = 0;
	tiles = 0;
	scolors = 0;
	textures = 0;
	tcolors = 0;
}

int XWPSClipCallbackData::copyAlpha(int xc, int yc, int xec, int yec)
{
	return tdev->copyAlpha(data + (yc - y) * raster, sourcex + xc - x, raster, ps_no_bitmap_id,
	                       xc, yc, xec - xc, yec - yc, color[0], depth);
}

int XWPSClipCallbackData::copyColor(int xc, int yc, int xec, int yec)
{
	return tdev->copyColor(data + (yc - y) * raster, sourcex + xc - x, raster, 
	                       ps_no_bitmap_id, xc, yc, xec - xc, yec - yc);
}

int XWPSClipCallbackData::copyMono(int xc, int yc, int xec, int yec)
{
	return tdev->copyMono(data + (yc - y) * raster,	 sourcex + xc - x, raster, 
	                      ps_no_bitmap_id, xc, yc, xec - xc, yec - yc, color[0], color[1]);
}

int XWPSClipCallbackData::fillMask(int xc, int yc, int xec, int yec)
{
	return tdev->fillMask(data + (yc - y) * raster, sourcex + xc - x, raster, ps_no_bitmap_id,
	                      xc, yc, xec - xc, yec - yc, pdcolor, depth, lop, NULL);
}

int XWPSClipCallbackData::fillRectangle(int xc, int yc, int xec, int yec)
{
	return tdev->fillRectangle(xc, yc, xec - xc, yec - yc, color[0]);
}

int XWPSClipCallbackData::stripCopyRop(int xc, int yc, int xec, int yec)
{
	return tdev->stripCopyRop(data + (yc - y) * raster, sourcex + xc - x, raster, ps_no_bitmap_id,
	                          scolors, textures, tcolors, xc, yc, xec - xc, yec - yc, phase.x, phase.y,lop);
}

int XWPSClipCallbackData::stripTileRectangle(int xc, int yc, int xec, int yec)
{
	return tdev->stripTileRectangle(tiles, xc, yc, xec - xc, yec - yc,
	                                color[0], color[1], phase.x, phase.y);
}

XWPSDeviceNull::XWPSDeviceNull(QObject * parent)
	:XWPSDeviceForward("null", 0, 0, 72, 72, 1, 1, 1, 0, 2, 0, parent)
{
	procs.fill_rectangle_.fill_rectanglenull = &XWPSDeviceNull::fillRectangleNull;
	procs.copy_mono_.copy_mononull = &XWPSDeviceNull::copyMonoNull;
	procs.copy_color_.copy_colornull = &XWPSDeviceNull::copyColorNull;
	procs.get_bits_.get_bits = &XWPSDevice::getBitsDefault;
	procs.put_params_.put_paramsnull = &XWPSDeviceNull::putParamsNull;
	procs.get_page_device_.get_page_device = &XWPSDevice::getPageDeviceDefault;
	procs.copy_alpha_.copy_alphanull = &XWPSDeviceNull::copyAlphaNull;
	procs.copy_rop_.copy_ropnull = &XWPSDeviceNull::copyRopNull;
	procs.fill_path_.fill_pathnull = &XWPSDeviceNull::fillPathNull;
	procs.stroke_path_.stroke_pathnull = &XWPSDeviceNull::strokePathNull;
	procs.fill_mask_.fill_mask = &XWPSDevice::fillMaskDefault;
	procs.fill_trapezoid_.fill_trapezoidnull = &XWPSDeviceNull::fillTrapezoidNull;
	procs.fill_parallelogram_.fill_parallelogramnull = &XWPSDeviceNull::fillParallelogramNull;
	procs.fill_triangle_.fill_trianglenull = &XWPSDeviceNull::fillTriangleNull;
	procs.draw_thin_line_.draw_thin_linenull = &XWPSDeviceNull::drawThinLineNull;
	procs.strip_copy_rop_.strip_copy_ropnull = &XWPSDeviceNull::stripCopyRopNull;
	procs.get_clipping_box_.get_clipping_box = &XWPSDevice::getClipingBoxDefault;
	procs.begin_typed_image_.begin_typed_image = &XWPSDevice::beginTypedImageDefault;
	procs.get_bits_rectangle_.get_bits_rectangle = &XWPSDevice::getBitsRectangleDefault;
	procs.text_begin_.text_begin = &XWPSDevice::textBeginDefault;
}

XWPSDeviceNull::~XWPSDeviceNull()
{
}

int XWPSDeviceNull::beginTypedImageDVIMid(XWPSImagerState * pis,
			                     XWPSMatrix *pmat, 
			                     XWPSImageCommon *pic,
			                     XWPSIntRect * prect,
			                     XWPSDeviceColor * pdcolor,
			                     XWPSClipPath * pcpath,
			                     XWPSImageEnumCommon ** pinfo)
{
	XWPSDeviceDVI * pdev = (XWPSDeviceDVI*)target;
	int code = pdev->beginTypedImage(pis, pmat, pic, prect, pdcolor, pcpath, pinfo, PDF_IMAGE_TYPE3_MASK);

  if (code < 0)
		return code;
  if ((*pinfo)->procs.plane_data_.plane_datadvi != &XWPSDVIImageEnum::dviImagePlaneData) 
		return (int)(XWPSError::RangeCheck);
  return code;
}

int XWPSDeviceNull::beginTypedImagePDFMid(XWPSImagerState * pis,
			                     XWPSMatrix *pmat, 
			                     XWPSImageCommon *pic,
			                     XWPSIntRect * prect,
			                     XWPSDeviceColor * pdcolor,
			                     XWPSClipPath * pcpath,
			                     XWPSImageEnumCommon ** pinfo)
{
	XWPSDevicePDF * pdev = (XWPSDevicePDF*)target;
	int code = pdev->beginTypedImage(pis, pmat, pic, prect, pdcolor, pcpath, pinfo, PDF_IMAGE_TYPE3_MASK);

  if (code < 0)
		return code;
  if ((*pinfo)->procs.plane_data_.plane_datapdf != &XWPSPDFImageEnum::pdfImagePlaneData) 
		return (int)(XWPSError::RangeCheck);
  return code;
}

int  XWPSDeviceNull::copyDevice(XWPSDevice **pnew)
{
	*pnew = new XWPSDeviceNull;
	return 0;
}

void XWPSDeviceNull::makeNullDevice(XWPSDevice *dev)
{
	setTarget(dev);
	color_info = dev->color_info;
	cached_colors = dev->cached_colors;
	copyColorProcs(dev);
}

int XWPSDeviceNull::putParamsNull(XWPSContextState * ctx,XWPSParamList *plist)
{
	int code = XWPSDeviceForward::putParams(ctx, plist);
		
	if (code < 0 || getPageDevice() == this)
		return code;
  width = height = 0;
  return code;
}
XWPSPDFTempFile::XWPSPDFTempFile()
{
	file = 0;
	strm = 0;
	strm_buf = 0;
	save_strm = 0;
}

XWPSPDFTempFile::~XWPSPDFTempFile()
{
	if (strm)
	{
		if (strm->decRef() == 0)
			delete strm;
		strm = 0;
	}
	
	if (strm_buf)
	{
		delete [] strm_buf;
		strm_buf = 0;
	}
	
	if (file)
	{
		delete file;
		file = 0;
	}
}

int XWPSPDFTempFile::openFile()
{
	if (file)
	{
		file->close();
		delete file;
		file = 0;
	}
	
	if (file_name.isEmpty())
		file_name = xwApp->getTmpFile();
		
	file = new QFile(file_name);
	if (!file)
		return (int)(XWPSError::InvalidFileAccess);
			
	if (!file->open(QIODevice::ReadWrite))
		return (int)(XWPSError::InvalidFileAccess);
			
	return 0;
}

int XWPSPDFTempFile::openStream()
{
	if (openFile() < 0)
		return (int)(XWPSError::VMError);
	
	if (!strm_buf)
	{
		uint l = 512;
		strm_buf = new uchar[l];
		strm = new XWPSStream;
		strm->writeFile(file, strm_buf, l, false,false);
	}
	
	return 0;
}

XWPSPDFBead::XWPSPDFBead()
{
	id = 0; 
	article_id = 0; 
	prev_id = 0; 
	next_id = 0; 
	page_id = 0;
}

XWPSPDFStdFont::XWPSPDFStdFont()
{
	font = 0;
	pfd = 0;
}

XWPSPDFStdFont::~XWPSPDFStdFont()
{
}

XWPSPDFTextState::XWPSPDFTextState()
{
	character_spacing = 0;
	font = 0;
	size = 0;
	word_spacing = 0;
	leading = 0;
	use_leading = false;
	buffer[0] = 0;
	buffer_count = 0;
}

XWPSPDFTextState::~XWPSPDFTextState()
{
}

void XWPSPDFTextState::reset()
{
	character_spacing = 0;
	font = 0;
	size = 0;
	word_spacing = 0;
	leading = 0;
	use_leading = false;
	matrix.reset();
	line_start.reset();
	current.reset();
	buffer[0] = 0;
	buffer_count = 0;
}

XWPSPDFResourceList::XWPSPDFResourceList()
{
	for (int i = 0; i < NUM_RESOURCE_CHAINS; i++)
		chains[i] = 0;
}

XWPSPDFResourceList::~XWPSPDFResourceList()
{
}

const PSPSDFSetColorCommands psdf_set_fill_color_commands = {
    "g", "rg", "k", "cs", "sc", "scn"
};

const PSPSDFSetColorCommands psdf_set_stroke_color_commands = {
    "G", "RG", "K", "CS", "SC", "SCN"
};

const PSPSDFImageFilterName Poly_filters[] = {
    {"DCTEncode", &s_DCTE_template},
    {"FlateEncode", &s_zlibE_template, psdf_version_ll3},
    {"LZWEncode", &s_LZWE_template},
    {0, 0}
};

const PSPSDFImageFilterName Mono_filters[] = {
    {"CCITTFaxEncode", &s_CFE_template},
    {"FlateEncode", &s_zlibE_template, psdf_version_ll3},
    {"LZWEncode", &s_LZWE_template},
    {"RunLengthEncode", &s_RLE_template},
    {0, 0}
};


#define psdf_image_param_names(acs, aa, af, de, di, ds, dt, dst, dstd, e, f, fns, r)\
    acs, di, dt, dstd, fns, f, {\
      {af, ps_param_type_bool, _OFFSET_OF_(XWPSPSDFImageParams, AutoFilter)},\
      {aa, ps_param_type_bool, _OFFSET_OF_(XWPSPSDFImageParams, AntiAlias)},\
      {de, ps_param_type_int, _OFFSET_OF_(XWPSPSDFImageParams, Depth)},\
      {ds, ps_param_type_bool, _OFFSET_OF_(XWPSPSDFImageParams, Downsample)},\
      {dst, ps_param_type_float, _OFFSET_OF_(XWPSPSDFImageParams, DownsampleThreshold)},\
      {e, ps_param_type_bool, _OFFSET_OF_(XWPSPSDFImageParams, Encode)},\
      {r, ps_param_type_int, _OFFSET_OF_(XWPSPSDFImageParams, Resolution)},\
      {0,0,0}\
    }

const PSPSDFImageParamNames Color_names = {
	psdf_image_param_names(
	"ColorACSImageDict", "AntiAliasColorImages", "AutoFilterColorImages",
	"ColorImageDepth", "ColorImageDict",
	"DownsampleColorImages", "ColorImageDownsampleType",
	"ColorImageDownsampleThreshold", 1.5,
	"EncodeColorImages", "ColorImageFilter", Poly_filters,
	"ColorImageResolution")
};

const PSPSDFImageParamNames Gray_names = {
	psdf_image_param_names(
	"GrayACSImageDict", "AntiAliasGrayImages", "AutoFilterGrayImages",
	"GrayImageDepth", "GrayImageDict",
	"DownsampleGrayImages", "GrayImageDownsampleType",
	"GrayImageDownsampleThreshold", 2.0,
	"EncodeGrayImages", "GrayImageFilter", Poly_filters,
	"GrayImageResolution")
};

const PSPSDFImageParamNames Mono_names = {
	psdf_image_param_names(
	0, "AntiAliasMonoImages", 0,
	"MonoImageDepth", "MonoImageDict",
	"DownsampleMonoImages", "MonoImageDownsampleType",
	"MonoImageDownsampleThreshold", 2.0,
	"EncodeMonoImages", "MonoImageFilter", Mono_filters,
	"MonoImageResolution" )
};


const char *const AutoRotatePages_names[] = {
    "None", "All", "PageByPage", 0
};
const char *const ColorConversionStrategy_names[] = {
    "LeaveColorUnchanged", "UseDeviceDependentColor",
	"UseDeviceIndependentColor", "UseDeviceIndependentColorForImages",
	"sRGB", 0
};
const char *const DownsampleType_names[] = {
    "Average", "Bicubic", "Subsample", 0
};
const char *const Binding_names[] = {
    "Left", "Right", 0
};
const char *const DefaultRenderingIntent_names[] = {
    "Default", "Perceptual", "Saturation", "RelativeColorimetric","AbsoluteColorimetric", 0
};
const char *const TransferFunctionInfo_names[] = {
    "Preserve", "Apply", "Remove", 0
};
const char *const UCRandBGInfo_names[] = {
    "Preserve", "Remove", 0
};
const char *const CannotEmbedFontPolicy_names[] = {
    "OK", "Warning", "Error", 0
};

const PSParamItem psdf_param_items[] = {
	
  {"ASCII85EncodePages", ps_param_type_bool, _OFFSET_OF_(XWPSPSDFDistillerParams,ASCII85EncodePages)},
    
  {"CompressPages", ps_param_type_bool, _OFFSET_OF_(XWPSPSDFDistillerParams,CompressPages)},
    
  {"DetectBlends", ps_param_type_bool, _OFFSET_OF_(XWPSPSDFDistillerParams,DetectBlends)},
  {"DoThumbnails", ps_param_type_bool, _OFFSET_OF_(XWPSPSDFDistillerParams,DoThumbnails)},
  {"ImageMemory", ps_param_type_long, _OFFSET_OF_(XWPSPSDFDistillerParams,ImageMemory)},
    
  {"LZWEncodePages", ps_param_type_bool, _OFFSET_OF_(XWPSPSDFDistillerParams,LZWEncodePages)},
  {"OPM", ps_param_type_int, _OFFSET_OF_(XWPSPSDFDistillerParams,OPM)},
  {"PreserveHalftoneInfo", ps_param_type_bool, _OFFSET_OF_(XWPSPSDFDistillerParams,PreserveHalftoneInfo)},
  {"PreserveOPIComments", ps_param_type_bool, _OFFSET_OF_(XWPSPSDFDistillerParams,PreserveOPIComments)},
  {"PreserveOverprintSettings", ps_param_type_bool, _OFFSET_OF_(XWPSPSDFDistillerParams,PreserveOverprintSettings)},
    
  {"UseFlateCompression", ps_param_type_bool, _OFFSET_OF_(XWPSPSDFDistillerParams,UseFlateCompression)},

  {"ConvertCMYKImagesToRGB", ps_param_type_bool, _OFFSET_OF_(XWPSPSDFDistillerParams,ConvertCMYKImagesToRGB)},
  {"ConvertImagesToIndexed", ps_param_type_bool, _OFFSET_OF_(XWPSPSDFDistillerParams,ConvertImagesToIndexed)},

  {"EmbedAllFonts", ps_param_type_bool, _OFFSET_OF_(XWPSPSDFDistillerParams,EmbedAllFonts)},
  {"MaxSubsetPct", ps_param_type_int, _OFFSET_OF_(XWPSPSDFDistillerParams,MaxSubsetPct)},
  {"SubsetFonts", ps_param_type_bool, _OFFSET_OF_(XWPSPSDFDistillerParams,SubsetFonts)},

    {0,0,0}
};

const char *const resource_names[] = {
    "ColorSpace", "ExtGState", "Pattern", "Shading", "XObject", "Font",
  0, 0, 0
};

const PSPDFImageNames pdf_image_names_full = {
    { "/DeviceCMYK", "/DeviceGray", "/DeviceRGB", "/Indexed" },
    { "/ASCII85Decode", "/ASCIIHexDecode", "/CCITTFaxDecode",
  		"/DCTDecode",  "/DecodeParms", "/Filter", "/FlateDecode",
  		"/LZWDecode", "/RunLengthDecode" },
    "/BitsPerComponent", "/ColorSpace", "/Decode",
    "/Height", "/ImageMask", "/Interpolate", "/Width"
};
const PSPDFImageNames pdf_image_names_short = {
    { "/CMYK", "/G", "/RGB", "/I" },
    { "/A85", "/AHx", "/CCF", "/DCT", "/DP", "/F", "/Fl", "/LZW", "/RL" },
    "/BPC", "/CS", "/D", "/H", "/IM", "/I", "/W"
};


const PSPDFColorSpaceNames pdf_color_space_names = {
    "/DeviceCMYK", "/DeviceGray", "/DeviceRGB", "/Indexed"
};
const PSPDFColorSpaceNames pdf_color_space_names_short = {
    "/CMYK", "/G", "/RGB", "/I"
};

#ifndef M_2PI
#  define M_2PI (2 * M_PI)
#endif

static float ht_EllipseA(float x, float y)
{return (1 - (x * x + 0.9 * y * y));}


static float ht_InvertedEllipseA(float x, float y)
{return (x * x + 0.9 * y * y - 1);}


static float ht_EllipseB(float x, float y)
{return (1 - sqrt(x * x + 0.625 * y * y));}


static float ht_EllipseC(float x, float y)
{return (1 - (0.9 * x * x + y * y));}


static float ht_InvertedEllipseC(float x, float y)
{return (0.9 * x * x + y * y - 1);}


static float ht_Line(float , float y)
{return (-fabs(y));}

static float ht_LineX(float x, float )
{return (x);}


static float ht_LineY(float , float y)
{return (y);}

static float ht_Square(float x, float y)
{return (-qMax(fabs(x), fabs(y)));}


static float ht_Cross(float x, float y)
{return (-qMin(fabs(x), fabs(y)));}


static float ht_Rhomboid(float x, float y)
{return ((0.9 * fabs(x) + fabs(y)) / 2);}


static float ht_DoubleDot(float x, float y)
{return ((sin(x * M_2PI) + sin(y * M_2PI)) / 2);}

static float ht_InvertedDoubleDot(float x, float y)
{return (-(sin(x * M_2PI) + sin(y * M_2PI)) / 2);}


static float ht_SimpleDot(float x, float y)
{return (1 - (x * x + y * y));}

static float ht_InvertedSimpleDot(float x, float y)
{return (x * x + y * y - 1);}

static float ht_CosineDot(float x, float y)
{return ((cos(x * M_PI) + cos(y * M_PI)) / 2);}

static float ht_Double(float x, float y)
{return ((sin(x * M_PI) + sin(y * M_2PI)) / 2);}

static float ht_InvertedDouble(float x, float y)
{return (-(sin(x * M_PI) + sin(y * M_2PI)) / 2);}


const ht_function_t ht_functions[] = {
    {"EllipseA", ht_EllipseA},
    {"InvertedEllipseA", ht_InvertedEllipseA},
    {"EllipseB", ht_EllipseB},
    {"EllipseC", ht_EllipseC},
    {"InvertedEllipseC", ht_InvertedEllipseC},
    {"Line", ht_Line},
    {"LineX", ht_LineX},
    {"LineY", ht_LineY},
    {"Square", ht_Square},
    {"Cross", ht_Cross},
    {"Rhomboid", ht_Rhomboid},
    {"DoubleDot", ht_DoubleDot},
    {"InvertedDoubleDot", ht_InvertedDoubleDot},
    {"SimpleDot", ht_SimpleDot},
    {"InvertedSimpleDot", ht_InvertedSimpleDot},
    {"CosineDot", ht_CosineDot},
    {"Double", ht_Double},
    {"InvertedDouble", ht_InvertedDouble}
};

static bool
coord_between(long start, long mid, long end)
{
    return (start <= end ? start <= mid && mid <= end :
	    start >= mid && mid >= end);
}

XWPSDeviceVector::XWPSDeviceVector(QObject * parent)
	:XWPSDevice(parent)
{
	vec_procs.beginpage_.beginpage = 0;
	vec_procs.setlinewidth_.setlinewidth = 0;
	vec_procs.setlinecap_.setlinecap = 0;
	vec_procs.setlinejoin_.setlinejoin = 0;
	vec_procs.setmiterlimit_.setmiterlimit = 0;
	vec_procs.setdash_.setdash = 0;
	vec_procs.setflat_.setflat = 0;
	vec_procs.setlogop_.setlogop = 0;
	vec_procs.setfillcolor_.setfillcolor = 0;
	vec_procs.setstrokecolor_.setstrokecolor = 0;
	vec_procs.dopath_.dopath = &XWPSDeviceVector::doPathVector;
	vec_procs.dorect_.dorect = 0;
	vec_procs.beginpath_.beginpath = 0;
	vec_procs.moveto_.moveto = 0;
	vec_procs.lineto_.lineto = 0;
	vec_procs.curveto_.curveto = 0;
	vec_procs.closepath_.closepath = 0;
	vec_procs.endpath_.endpath = 0;
	
	strcpy(fname, "xwps");
	
	file = 0;
	strm = 0;
	strmbuf = 0;
	strmbuf_size = 0;
	state = new XWPSImagerState;
	for (int i = 0; i < 11; i++)
		dash_pattern[i] = 0.0;
		
	fill_color = new XWPSDeviceColor(0);
	stroke_color = new XWPSDeviceColor(0);
	fill_color->unset();
	stroke_color->unset();
	no_clip_path_id = ps_no_id;
	clip_path_id = ps_no_id;
	fill_options = ps_path_type_none;
	stroke_options = ps_path_type_none;
	scale.x = X_DPI/72.0;
	scale.y = Y_DPI/72.0;
	in_page = false;
	black = ps_no_color_index;
	white = ps_no_color_index;
	bbox_device = 0;
}

XWPSDeviceVector::XWPSDeviceVector(const char * devname,
	                                 int w,
	                                 int h,
	                                 float xdpi,
	                                 float ydpi,
	                                 int nc,
	                                 int depth,
	                                 ushort mg, 
	                                 ushort mc,
	                                 ushort dg,
	                                 ushort dc,
	                                 QObject * parent)
	:XWPSDevice(devname, w, h, xdpi, ydpi, nc, depth, mg, mc, dg, dc, parent)
{
	vec_procs.beginpage_.beginpage = 0;
	vec_procs.setlinewidth_.setlinewidth = 0;
	vec_procs.setlinecap_.setlinecap = 0;
	vec_procs.setlinejoin_.setlinejoin = 0;
	vec_procs.setmiterlimit_.setmiterlimit = 0;
	vec_procs.setdash_.setdash = 0;
	vec_procs.setflat_.setflat = 0;
	vec_procs.setlogop_.setlogop = 0;
	vec_procs.setfillcolor_.setfillcolor = 0;
	vec_procs.setstrokecolor_.setstrokecolor = 0;
	vec_procs.dopath_.dopath = &XWPSDeviceVector::doPathVector;
	vec_procs.dorect_.dorect = 0;
	vec_procs.beginpath_.beginpath = 0;
	vec_procs.moveto_.moveto = 0;
	vec_procs.lineto_.lineto = 0;
	vec_procs.curveto_.curveto = 0;
	vec_procs.closepath_.closepath = 0;
	vec_procs.endpath_.endpath = 0;
	
	strcpy(fname, "xwps");
	
	file = 0;
	strm = 0;
	strmbuf = 0;
	strmbuf_size = 0;
	state = new XWPSImagerState;
	for (int i = 0; i < 11; i++)
		dash_pattern[i] = 0.0;
		
	fill_color = new XWPSDeviceColor(0);
	stroke_color = new XWPSDeviceColor(0);
	fill_color->unset();
	stroke_color->unset();
	no_clip_path_id = ps_no_id;
	clip_path_id = ps_no_id;
	fill_options = ps_path_type_none;
	stroke_options = ps_path_type_none;
	scale.x = X_DPI/72.0;
	scale.y = Y_DPI/72.0;
	in_page = false;
	black = ps_no_color_index;
	white = ps_no_color_index;
	bbox_device = 0;
}

XWPSDeviceVector::~XWPSDeviceVector()
{	
	if (strm)
	{
		if (!(strm->decRef()))
			delete strm;
		strm = 0;
	}
	
	if (strmbuf)
	{
		delete [] strmbuf;
		strmbuf = 0;
	}
	
	if (fill_color)
	{
		delete fill_color;
		fill_color = 0;
	}
	
	if (stroke_color)
	{
		delete stroke_color;
		stroke_color = 0;
	}
	
	if (state)
	{
		delete state;
		state = 0;
	}
	
	if (bbox_device)
	{
		if (!bbox_device->decRef())
			delete bbox_device;
		bbox_device = 0;
	}
}

int XWPSDeviceVector::beginPage()
{
	return (this->*(vec_procs.beginpage_.beginpage))();
}

int XWPSDeviceVector::beginPath(PSPathType type)
{
	return (this->*(vec_procs.beginpath_.beginpath))(type);
}

int XWPSDeviceVector::closePath(double x0, double y0, double x_start, double y_start, PSPathType type)
{
	return (this->*(vec_procs.closepath_.closepath))(x0, y0, x_start,y_start, type);
}

int  XWPSDeviceVector::copyDevice(XWPSDevice **pnew)
{
	XWPSDeviceVector * ret = new XWPSDeviceVector(dname, width, height,HWResolution[0],HWResolution[1],color_info.num_components,color_info.depth,color_info.max_gray,color_info.max_color,color_info.dither_grays,color_info.dither_colors);
	ret->copyDeviceParamVector(this);
	ret->vec_procs = vec_procs;
	*pnew = ret;
	return 0;
}

void XWPSDeviceVector::copyDeviceParamVector(XWPSDeviceVector * proto)
{
	copyDeviceParam(proto);
	file = proto->file;
	for (int i = 0; i < 11; i++)
		dash_pattern[i] = proto->dash_pattern[i];
	fill_options = proto->fill_options;
	stroke_options = proto->stroke_options;
	scale = proto->scale;
	in_page = proto->in_page;
	black = proto->black;
	white = proto->white;
}

int XWPSDeviceVector::curveTo(double x0, 
	            double y0,
	            double x1, 
	            double y1, 
	            double x2, 
	            double y2,	
	            double x3, 
	            double y3, 
	            PSPathType type)
{
	return (this->*(vec_procs.curveto_.curveto))(x0, y0, x1, y1, x2, y2, x3, y3, type);
}

int XWPSDeviceVector::doPath(XWPSPath *ppath,  PSPathType type, XWPSMatrix *pmat)
{
	return (this->*(vec_procs.dopath_.dopath))(ppath, type, pmat);
}

int XWPSDeviceVector::doPathVector(XWPSPath * ppath, PSPathType type, XWPSMatrix *pmat)
{
	bool do_close =	(type & (ps_path_type_stroke | ps_path_type_always_close)) != 0;
  XWPSFixedRect rbox;
  PSPathRectangularType rtype = ppath->isRectangular(&rbox);
  XWPSPathEnum cenum;
  XWPSDevVectorDoPathState stateA;
  XWPSFixedPoint line_start, line_end;
  bool incomplete_line = false;
  bool need_moveto = false;
  int code;
  
  stateA.init(this, type, pmat);
  if (rtype != prt_none &&
			!((type & ps_path_type_stroke) && rtype == prt_open) &&
			(pmat == 0 || pmat->isXXYY() || pmat->isXYYX()) &&
			(stateA.scale_mat.xx == 1.0 && stateA.scale_mat.yy == 1.0 &&
	 		stateA.scale_mat.isXXYY() &&
	 		is_fzero2(stateA.scale_mat.tx, stateA.scale_mat.ty)))
	{
		XWPSPoint p, q;
		p.transformInverse((float)rbox.p.x, (float)rbox.p.y, &stateA.scale_mat);
		q.transformInverse((float)rbox.q.x, (float)rbox.q.y, &stateA.scale_mat);
		code = doRect((long)p.x, (long)p.y, (long)q.x, (long)q.y, type);
		if (code >= 0)
	    return code;
	}
	
	code = beginPath(type);
  if (code < 0)
		return code;
  cenum.init(ppath);
  for (;;)
  {
  	XWPSFixedPoint vs[3];
		int pe_op = cenum.next(vs);
		
sw:
		if (type & ps_path_type_optimize)
		{
opt:
			if (pe_op == ps_pe_lineto)
			{
				if (!incomplete_line) 
				{
		    	line_end = vs[0];
		    	incomplete_line = true;
		    	continue;
				}
				
				if (vs[0].x == line_end.x) 
				{
		    	if (vs[0].x == line_start.x && coord_between(line_start.y, line_end.y, vs[0].y)) 
		    	{
						line_end.y = vs[0].y;
						continue;
		    	}
				}
				else if (vs[0].y == line_end.y) 
				{
		    	if (vs[0].y == line_start.y && coord_between(line_start.x, line_end.x, vs[0].x)) 
		    	{
						line_end.x = vs[0].x;
						continue;
		    	}
				}
			}
			
			if (incomplete_line)
			{
				if (need_moveto) 
				{
		    	code = stateA.segment(ps_pe_moveto, &line_start);
		    	if (code < 0)
						return code;
		    	need_moveto = false;
				}
				code = stateA.segment(ps_pe_lineto, &line_end);
				if (code < 0)
		    	return code;
				line_start = line_end;
				incomplete_line = false;
				goto opt;
			}
		}
		
		switch (pe_op)
		{
			case 0:
				
done:
				code = endPath(type);
	    	return (code < 0 ? code : 0);
	    		
	    case ps_pe_curveto:
	    	if (need_moveto)
	    	{
	    		code = stateA.segment(ps_pe_moveto, &line_start);
					if (code < 0)
		    		return code;
					need_moveto = false;
	    	}
	    	line_start = vs[2];
	    	goto draw;
	    	
	    case ps_pe_moveto:
	    	line_start = vs[0];
	    	if (!(type & ps_path_type_stroke) && (type & ps_path_type_fill)) 
	    	{
					need_moveto = true;
					continue;
	    	}
	    	goto draw;
	    	
	    case ps_pe_lineto:
	    	if (need_moveto) 
	    	{
					code = stateA.segment(ps_pe_moveto, &line_start);
					if (code < 0)
		    		return code;
					need_moveto = false;
	    	}
	    	line_start = vs[0];
	    	goto draw;
	    	
	    case ps_pe_closepath:
	    	if (need_moveto) 
	    	{
					need_moveto = false;
					continue;
	    	}
	    	if (!do_close) 
	    	{
					pe_op = cenum.next(vs);
					if (pe_op == 0)
		    		goto done;
					code = stateA.segment(ps_pe_closepath, vs);
					if (code < 0)
		    		return code;
					goto sw;
	    	}
	    	
draw:
	    	code = stateA.segment(pe_op, vs);
	    	if (code < 0)
					return code;
		}
		incomplete_line = false;
  }
}

int XWPSDeviceVector::doRect(long x0, long y0, long x1, long y1, PSPathType type)
{
	return (this->*(vec_procs.dorect_.dorect))(x0, y0, x1, y1, type);
}

int XWPSDeviceVector::endPath(PSPathType type)
{
	return (this->*(vec_procs.endpath_.endpath))(type);
}

int XWPSDeviceVector::getParamsVector(XWPSContextState * ctx, XWPSParamList *plist)
{
	int code = getParamsDefault(ctx, plist);
  int ecode;
  PSParamString ofns;

  if (code < 0)
		return code;
		
	ofns.fromString(fname);
  if ((ecode = plist->writeString(ctx, "OutputFile", &ofns)) < 0)
		return ecode;
		
  return code;
}

int XWPSDeviceVector::lineTo(double x0, double y0, double x, double y, PSPathType type)
{
	return (this->*(vec_procs.lineto_.lineto))(x0, y0, x, y, type);
}

int XWPSDeviceVector::moveTo(double x0, double y0,double x, double y, PSPathType type)
{
	return (this->*(vec_procs.moveto_.moveto))(x0, y0, x, y, type);
}

int XWPSDeviceVector::prepareStrokeVector(XWPSImagerState * pis,
	                  XWPSStrokeParams * params, 
	                  XWPSDeviceColor * pdcolor,
			              float scale)
{
	int pattern_size = pis->line_params.dash.pattern_size;
  float dash_offset = pis->line_params.dash.offset * scale;
  float half_width = pis->line_params.half_width * scale;

  if (pattern_size > PS_MAX_DASH)
		return (int)(XWPSError::LimitCheck);
  if (dash_offset != state->line_params.dash.offset ||
			pattern_size != state->line_params.dash.pattern_size ||
			(pattern_size != 0 &&	 !(pis->line_params.dash.dashPatternEq(dash_pattern, scale)))) 
	{
		float pattern[PS_MAX_DASH];
		int i, code;

		for (i = 0; i < pattern_size; ++i)
	    pattern[i] = pis->line_params.dash.pattern[i] * scale;
		code = setDash(pattern, pattern_size, dash_offset);
		if (code < 0)
	    return code;
		memcpy(dash_pattern, pattern, pattern_size * sizeof(float));

		state->line_params.dash.pattern_size = pattern_size;
		state->line_params.dash.offset = dash_offset;
  }
  if (params->flatness != state->flatness) 
  {
		int code = setFlat( params->flatness);

		if (code < 0)
	    return code;
		state->flatness = params->flatness;
  }
  if (half_width != state->line_params.half_width) 
  {
		int code = setLineWidth(half_width * 2);

		if (code < 0)
	    return code;
		state->line_params.half_width = half_width;
  }
  if (pis->line_params.miter_limit != state->line_params.miter_limit) 
  {
		int code = setMiterLimit(pis->line_params.miter_limit);

		if (code < 0)
	    return code;
		state->line_params.setMiterLimit( pis->line_params.miter_limit);
  }
  if (pis->line_params.cap != state->line_params.cap) 
  {
		int code = setLineCap(pis->line_params.cap);

		if (code < 0)
	    return code;
		state->line_params.cap = pis->line_params.cap;
  }
  if (pis->line_params.join != state->line_params.join) 
  {
		int code = setLineJoin(pis->line_params.join);

		if (code < 0)
	    return code;
		state->line_params.join = pis->line_params.join;
  } 
  {
		int code = updateLogOp(pis->log_op);

		if (code < 0)
	    return code;
  }
  if (!drawingColorEq(pdcolor, stroke_color)) 
  {
		int code = setStrokeColor(pdcolor);

		if (code < 0)
	    return code;
		*stroke_color = *pdcolor;
  }
  return 0;
}

int XWPSDeviceVector::putParamsVector(XWPSContextState * ctx, XWPSParamList *plist)
{
	int ecode = 0;
  int code;
  const char * param_name;
  PSParamString ofns;

  switch (code = plist->readString(ctx, (param_name = "OutputFile"), &ofns)) 
  {
		case 0:
	    if (ofns.size > 127)
				ecode = XWPSError::LimitCheck;
	    else if (!bytes_compare(ofns.data, ofns.size, (const uchar *)fname, strlen(fname))) 
				break;
	    else if (is_open && strm != 0 && strm->tell() != 0)
				ecode = XWPSError::RangeCheck;
	    else
				break;
	    goto ofe;
	    
		default:
	    ecode = code;
	    
ofe:
			plist->signalError(ctx, param_name, ecode);
		case 1:
	    break;
  }

  if (ecode < 0)
		return ecode;
  
  {
		bool open = is_open;
		is_open = false;
		code = putParamsDefault(ctx, plist);
		is_open = open;
  }
  if (code < 0)
		return code;

  if (ofns.data != 0) 
  {
		memcpy(fname, ofns.data, ofns.size);
		fname[ofns.size] = 0;
		if (file != 0) 
		{
	    XWPSDeviceBbox *bbdev = bbox_device;

	    bbox_device = 0;
	    code = closeFileVector();
	    bbox_device = bbdev;
	    if (code < 0)
				return code;
	    return openFileBbox(strmbuf_size, bbdev != 0);
		}
  }
  loadCache();
  return 0;
}

int XWPSDeviceVector::reset()
{
	if (!state)
		state = new XWPSImagerState;
	
	state->i_ctx_p = context_state;
	
	if (!fill_color)
		fill_color = new XWPSDeviceColor(0);
		
	if (!stroke_color)
		stroke_color = new XWPSDeviceColor(0);
		
	fill_color->unset();
	stroke_color->unset();
	if (context_state)
	{
		clip_path_id = no_clip_path_id = context_state->nextIDS(1);
	}
	return 0;
}

int XWPSDeviceVector::setDash(const float *pattern, uint count, double offset)
{
	return (this->*(vec_procs.setdash_.setdash))(pattern, count, offset);
}

int XWPSDeviceVector::setFillColor(XWPSDeviceColor * pdc)
{
	return (this->*(vec_procs.setfillcolor_.setfillcolor))(pdc);
}

int XWPSDeviceVector::setFlat(double f)
{
	return (this->*(vec_procs.setflat_.setflat))(f);
}

int XWPSDeviceVector::setLineCap(PSLineCap c)
{
	return (this->*(vec_procs.setlinecap_.setlinecap))(c);
}

int XWPSDeviceVector::setLineJoin(PSLineJoin j)
{
	return (this->*(vec_procs.setlinejoin_.setlinejoin))(j);
}

int XWPSDeviceVector::setLineWidth(double w)
{
	return (this->*(vec_procs.setlinewidth_.setlinewidth))(w);
}

int XWPSDeviceVector::setLogOp(uint lop, uint diff)
{
	return (this->*(vec_procs.setlogop_.setlogop))(lop, diff);
}

int XWPSDeviceVector::setMiterLimit(double l)
{
	return (this->*(vec_procs.setlinewidth_.setlinewidth))(l);
}

int XWPSDeviceVector::setStrokeColor(XWPSDeviceColor * pdc)
{
	return (this->*(vec_procs.setstrokecolor_.setstrokecolor))(pdc);
}

XWPSStream * XWPSDeviceVector::stream()
{
	if (!in_page) 
	{
		beginPage();
		in_page = true;
  }
  return strm;
}

int XWPSDeviceVector::strokeScaling(XWPSImagerState *pis,
			              double *pscale, 
			              XWPSMatrix *pmat)
{
	bool set_ctm = true;
  double scaleA = 1;
  
  if (pis->ctm.isXXYY()) 
  {
		scaleA = fabs(pis->ctm.xx);
		set_ctm = fabs(pis->ctm.yy) != scaleA;
  } 
  else if (((XWPSImagerState*)pis)->ctm.isXYYX()) 
  {
		scaleA = fabs(pis->ctm.xy);
		set_ctm = fabs(pis->ctm.yx) != scaleA;
  } 
  else if ((pis->ctm.xx == pis->ctm.yy && pis->ctm.xy == -pis->ctm.yx) ||
	       (pis->ctm.xx == -pis->ctm.yy && pis->ctm.xy == pis->ctm.yx)) 
	{
		scaleA = hypot(pis->ctm.xx, pis->ctm.xy);
		set_ctm = false;
  }
  if (set_ctm)
  {
  	double  mxx = pis->ctm.xx / scale.x,
	  mxy = pis->ctm.xy / scale.y,
	  myx = pis->ctm.yx / scale.x,
	  myy = pis->ctm.yy / scale.y;

		scaleA = 0.5 * (fabs(mxx) + fabs(mxy) + fabs(myx) + fabs(myy));
		pmat->xx = mxx / scaleA, pmat->xy = mxy / scaleA;
		pmat->yx = myx / scaleA, pmat->yy = myy / scaleA;
		pmat->tx = pmat->ty = 0;
  }
  
  *pscale = scaleA;
  return (int)set_ctm;
}

int XWPSDeviceVector::updateLogOp(uint lop)
{
	ulong diff = lop ^ (state->log_op);

  if (diff != 0) 
  {
		int code = setLogOp(lop, diff);

		if (code < 0)
	    return code;
		state->log_op = lop;
  }
  return 0;
}

int  XWPSDeviceVector::closeFileVector()
{
	if (bbox_device)
	{
		if (!bbox_device->decRef())
			delete bbox_device;
		bbox_device = 0;
	}
	
	if (strm)
	{
		strm->close();
		if (strm->decRef() == 0)
			delete strm;
		strm = 0;
	}
	
	if (strmbuf)
	{
		delete [] strmbuf;
		strmbuf = 0;
	}
	
	return 0;
}

bool XWPSDeviceVector::drawingColorEq(XWPSDeviceColor * pdc1,  XWPSDeviceColor * pdc2)
{
	return (pdc1->isPure() ?  pdc2->isPure() &&
	    		pdc1->pureColor() == pdc2->pureColor() :
	    		pdc1->isNull() ? pdc2->isNull() :  false);
}

bool XWPSDeviceVector::hasSubsetPrefix(const uchar *str, uint size)
{
	if (size < SUBSET_PREFIX_SIZE || str[SUBSET_PREFIX_SIZE - 1] != '+')
		return false;
  for (int i = 0; i < SUBSET_PREFIX_SIZE - 1; ++i)
		if ((uint)(str[i] - 'A') >= 26)
	    return false;
  return true;
}

void XWPSDeviceVector::loadCache()
{
	black = getBlack();
  white = getWhite();
}

bool XWPSDeviceVector::makePathScaling(XWPSPath *ppath, double *pscale)
{
	 XWPSFixedRect bbox;
  double bmin, bmax;

  ppath->getBbox(&bbox);
  bmin = qMin(bbox.p.x / scale.x, bbox.p.y / scale.y);
  bmax = qMax(bbox.q.x / scale.x, bbox.q.y / scale.y);
#define MAX_USER_COORD 32000
  if (bmin <= int2fixed(-MAX_USER_COORD) || bmax > int2fixed(MAX_USER_COORD)) 
  {
		*pscale = qMax(bmin / int2fixed(-MAX_USER_COORD),  bmax / int2fixed(MAX_USER_COORD));
		return true;
  } 
  else 
  {
		*pscale = 1;
		return false;
  }
#undef MAX_USER_COORD
}

void XWPSDeviceVector::makeSubsetPrefix(uchar *str, ulong id)
{
	ulong v = id * 987654321;
  for (int i = 0; i < SUBSET_PREFIX_SIZE - 1; ++i, v /= 26)
		str[i] = 'A' + (v % 26);
  str[SUBSET_PREFIX_SIZE - 1] = '+';
}

bool XWPSDeviceVector::mustPutClipPath( XWPSClipPath * pcpath)
{
	if (pcpath == NULL)
		return clip_path_id != no_clip_path_id;
  if (clip_path_id == pcpath->id)
		return false;
  if (pcpath->includesRectangle(fixed_0, fixed_0, int2fixed(width), int2fixed(height)))
		return clip_path_id != no_clip_path_id;
  return true;
}

int XWPSDeviceVector::openFile(uint strmbuf_sizeA)
{
	return openFileBbox(strmbuf_sizeA, false);
}

int XWPSDeviceVector::openFileBbox(uint strmbuf_sizeA,  bool bbox)
{
	strmbuf = new uchar[strmbuf_sizeA];
	strmbuf_size = strmbuf_sizeA;
	strm = new XWPSStream;
	strm->writeFile(file, strmbuf, strmbuf_size, false, false);
	if (bbox)
	{
		bbox_device = new XWPSDeviceBbox;
		bbox_device->initBbox(0);
		bbox_device->setResolution(HWResolution[0], HWResolution[1]);
		bbox_device->procs.get_initial_matrix_.get_initial_matrix = &XWPSDevice::getInitialMatrixDefault;
		bbox_device->open();
	}
	return 0;
}

int XWPSDeviceVector::scanToken(const uchar **pscan, const uchar * end, const uchar **ptoken)
{
	const uchar *p = *pscan;

  while (p < end && scan_char_decoder[*p] == ctype_space)
		++p;
  *ptoken = p;
  if (p >= end) 
  {
		*pscan = p;
		return 0;
  }
  switch (*p) 
  {
    case '%':
    case ')':
			return (int)(XWPSError::SyntaxError);
    case '(': 
    	{
				uchar buf[50];
				PSStreamCursorRead r;
				PSStreamCursorWrite w;
				PSStreamPSSDState ss;
				int status;
				ss.depth = 0;
				r.ptr = (uchar*)p;
				r.limit = (uchar*)(end - 1);
				w.limit = buf + sizeof(buf) - 1;
				do 
				{
	    		w.ptr = buf;  w.ptr--;
	    		status = (*s_PSSD_template.process)((PSStreamState *)&ss, &r, &w, true);
				}	while (status == 1);
				*pscan = r.ptr + 1;
				return 1;
    	}
    case '<':
			if (end - p < 2)
	    	return (int)(XWPSError::SyntaxError);
			if (p[1] != '<') 
			{
	    	p = (const uchar *)memchr(p + 1, '>', end - p - 1);
	    	if (p == 0)
					return (int)(XWPSError::SyntaxError);
			}
			goto m2;
    case '>':
			if (end - p < 2 || p[1] != '>')
	    	return (int)(XWPSError::SyntaxError);
m2:	
			*pscan = p + 2;
			return 1;
			
    case '[': case ']': case '{': case '}':
			*pscan = p + 1;
			return 1;
			
    case '/':
			++p;
    default:
			break;
  }
  while (p < end && scan_char_decoder[*p] <= ctype_name)
		++p;
  *pscan = p;
  if (p == *ptoken)
		return (int)(XWPSError::SyntaxError);
  return 1;
}

int XWPSDeviceVector::scanTokenComposite(const uchar **pscan, const uchar * end, const uchar **ptoken_orig)
{
	int level = 0;
  const uchar *ignore_token;
  const uchar **ptoken = ptoken_orig;
  int code;

  do 
  {
		code = scanToken(pscan, end, ptoken);
		if (code <= 0)
	    return (code < 0 || level == 0 ? code : (int)(XWPSError::SyntaxError));
		switch (**ptoken) 
		{
			case '<': case '[': case '{':
	    	++level; break;
			case '>': case ']': case '}':
	    	if (level == 0)
					return (int)(XWPSError::SyntaxError);
	    --level; break;
		}
		ptoken = &ignore_token;
  } while (level);
  return code;
}

void  XWPSDeviceVector::setPatternImage(XWPSDataImage *pic, XWPSStripBitmap *tile)
{
	pic->ImageMatrix.xx = pic->Width = tile->rep_width;
  pic->ImageMatrix.yy = pic->Height = tile->rep_height;
}

void XWPSDeviceVector::setProcessColorModel()
{
	switch (color_info.num_components)
	{
		case 1:
			procs.map_rgb_color_.map_rgb_color = &XWPSDevice::mapRGBColorGrayDefault;
			procs.map_color_rgb_.map_color_rgb = &XWPSDevice::mapColorRGBGrayDefault;
			procs.map_cmyk_color_.map_cmyk_color = 0;
			break;
			
    case 3:
    	procs.map_rgb_color_.map_rgb_color = &XWPSDevice::mapRGBColorRGBDefault;
			procs.map_color_rgb_.map_color_rgb = &XWPSDevice::mapColorRGBRGBDefault;
			procs.map_cmyk_color_.map_cmyk_color = 0;
			break;
			
    case 4:
    	procs.map_rgb_color_.map_rgb_color = 0;
			procs.map_color_rgb_.map_color_rgb = &XWPSDevice::mapColorRGBCMYK8Bit;
			procs.map_cmyk_color_.map_cmyk_color = &XWPSDevice::mapCMYKColorCMYK8Bit;
			break;
			
    default:
			break;
	}
}

void XWPSDeviceVector::storeDefaultProducer(char * buf)
{
	sprintf(buf, "(%s %s)", xwApp->getProductName8(), xwApp->getVersion8());
}

XWPSDevVectorDoPathState::XWPSDevVectorDoPathState()
{
	vdev = 0;
	type = ps_path_type_none;
	first = false;
}

XWPSDevVectorDoPathState::~XWPSDevVectorDoPathState()
{
}

void XWPSDevVectorDoPathState::init(XWPSDeviceVector * dev, PSPathType t, XWPSMatrix *pmat)
{
	vdev = dev;
  type = t;
  if (pmat)
  {
  	scale_mat = *pmat;
  	scale_mat.matrixScale(&scale_mat, 1.0 / vdev->scale.x, 1.0 / vdev->scale.y);
  }
  else
  	scale_mat.makeScaling(vdev->scale.x, vdev->scale.y);
  	
  first = true;
}

int XWPSDevVectorDoPathState::segment(int pe_op, XWPSFixedPoint * vs)
{
	XWPSMatrix * pmat = &scale_mat;
  XWPSPoint vp[3];
  int code;

  switch (pe_op) 
  {
		case ps_pe_moveto:
	    vp[0].transformInverse(fixed2float(vs[0].x),fixed2float(vs[0].y), pmat);
	    if (first)
	    {
				start = vp[0]; 
				first = false;
			}
	    code = vdev->moveTo(prev.x, prev.y, vp[0].x, vp[0].y, type);
	    prev = vp[0];
	    break;
	    
		case ps_pe_lineto:
	    vp[0].transformInverse(fixed2float(vs[0].x), fixed2float(vs[0].y), pmat);
	    code = vdev->lineTo(prev.x, prev.y, vp[0].x, vp[0].y, type);
	    prev = vp[0];
	    break;
	    
		case ps_pe_curveto:
	    vp[0].transformInverse(fixed2float(vs[0].x),fixed2float(vs[0].y), pmat);
	    vp[1].transformInverse(fixed2float(vs[1].x),fixed2float(vs[1].y), pmat);
	    vp[2].transformInverse(fixed2float(vs[2].x),fixed2float(vs[2].y), pmat);
	    code = vdev->curveTo(prev.x, prev.y, vp[0].x, vp[0].y, vp[1].x, vp[1].y, vp[2].x, vp[2].y, type);
	    prev = vp[2];
	    break;
	    
		case ps_pe_closepath:
	    code = vdev->closePath(prev.x, prev.y, start.x, start.y, type);
	    prev = start;
	    break;
	    
		default:
	    return -1;
  }
  return code;
}

XWPSPSDFImageParams::XWPSPSDFImageParams()
{
	ACSDict = 0;
	AntiAlias = false;
	AutoFilter = false;
	Depth = -1;
	Dict = 0;
	Downsample = true;
	DownsampleThreshold = 0;
	DownsampleType = ds_Subsample;
	Encode = true;
	Filter = 0;
	Resolution = 0;
	filter_template = 0;
}

XWPSPSDFImageParams::~XWPSPSDFImageParams()
{
}

void XWPSPSDFImageParams::copy(XWPSPSDFImageParams * from)
{
	AntiAlias = from->AntiAlias;
	AutoFilter = from->AutoFilter;
	Depth = from->Depth;
	Downsample = from->Downsample;
	DownsampleThreshold = from->DownsampleThreshold;
	DownsampleType = from->DownsampleType;
	Encode = from->Encode;
	Filter = from->Filter;
	Resolution = from->Resolution;
}

bool XWPSPSDFImageParams::doDownSample(XWPSPixelImage *pim, float resolution)
{
	float factor = (int)(resolution / Resolution);

  return (Downsample && factor >= DownsampleThreshold &&
	    factor <= pim->Width && factor <= pim->Height);
}

void XWPSPSDFImageParams::init(bool af, 
	                             int res, 
	                             float dst, 
	                             const char * f, 
	                             PSStreamTemplate * ft)
{
	AutoFilter = af;
	Resolution = res;
	DownsampleThreshold = dst;
	Filter = f;
	filter_template = ft;
}

XWPSPSDFDistillerParams::XWPSPSDFDistillerParams()
{
	ASCII85EncodePages = false;
	AutoRotatePages = arp_None;
	Binding = binding_Left;
	CompressPages = true;
	DefaultRenderingIntent = ri_Default;
	DetectBlends = true;
	DoThumbnails = false;
	ImageMemory = 500000;
	LockDistillerParams = false;
	LZWEncodePages = false;
	OPM = 1;
	PreserveOPIComments = false;
	UseFlateCompression = true;
	ColorConversionStrategy = ccs_LeaveColorUnchanged;
	PreserveHalftoneInfo = false;
	PreserveOverprintSettings = false;
	TransferFunctionInfo = tfi_Preserve;
	UCRandBGInfo = ucrbg_Remove;
	ConvertCMYKImagesToRGB = false;
	ConvertImagesToIndexed = false;
	CannotEmbedFontPolicy = cefp_Warning;
	EmbedAllFonts = true;
	MaxSubsetPct = 100;
	SubsetFonts = true;
}

XWPSPSDFDistillerParams::~XWPSPSDFDistillerParams()
{
	if (CalCMYKProfile.data)
	{
		delete [] CalCMYKProfile.data;
		CalCMYKProfile.data = 0;
	}
	
	if (CalGrayProfile.data)
	{
		delete [] CalGrayProfile.data;
		CalGrayProfile.data = 0;
	}
	
	if (CalRGBProfile.data)
	{
		delete [] CalRGBProfile.data;
		CalRGBProfile.data = 0;
	}
	
	if (sRGBProfile.data)
	{
		delete [] sRGBProfile.data;
		sRGBProfile.data = 0;
	}
}

void XWPSPSDFDistillerParams::copy(XWPSPSDFDistillerParams * from)
{
	ASCII85EncodePages = from->ASCII85EncodePages;
  AutoRotatePages = from->AutoRotatePages;
  Binding = from->Binding;
  CompressPages = from->CompressPages;
  DefaultRenderingIntent = from->DefaultRenderingIntent;
  DetectBlends = from->DetectBlends;
  DoThumbnails = from->DoThumbnails;
  ImageMemory = from->ImageMemory;
  LockDistillerParams = from->LockDistillerParams;
  LZWEncodePages = from->LZWEncodePages;
  OPM = from->OPM;
  PreserveOPIComments = from->PreserveOPIComments;
  UseFlateCompression = from->UseFlateCompression;
  ColorConversionStrategy = from->ColorConversionStrategy;
  PreserveHalftoneInfo = from->PreserveHalftoneInfo;
  PreserveOverprintSettings = from->PreserveOverprintSettings;
  TransferFunctionInfo = from->TransferFunctionInfo;
  UCRandBGInfo = from->UCRandBGInfo;
  ColorImage.copy(&from->ColorImage);
  ConvertCMYKImagesToRGB = from->ConvertCMYKImagesToRGB;
  ConvertImagesToIndexed = from->ConvertImagesToIndexed;
  GrayImage.copy(&from->GrayImage);
  MonoImage.copy(&from->MonoImage);
  CannotEmbedFontPolicy = from->CannotEmbedFontPolicy;
  EmbedAllFonts = from->EmbedAllFonts;
  MaxSubsetPct = from->MaxSubsetPct;
  SubsetFonts = from->SubsetFonts;
}
