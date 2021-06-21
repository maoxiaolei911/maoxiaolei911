/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWPSColorSpace.h"
#include "XWPSComposite.h"
#include "XWPSDeviceCompositeAlpha.h"

XWPSDeviceCompositeAlpha::XWPSDeviceCompositeAlpha(QObject * parent)
	:XWPSDeviceForward("alpha compositor", 0,0,1,1, 1,1,	1,0,2,0, parent)
{
	procs.sync_output_.sync_output = &XWPSDevice::syncOutputDefault;
	procs.output_page_.output_page = &XWPSDevice::outputPageDefault;
	procs.close__.closecra = &XWPSDeviceCompositeAlpha::closeDCA;
	procs.map_rgb_color_.map_rgb_colorcra = &XWPSDeviceCompositeAlpha::mapRGBColorDCA;
	procs.map_color_rgb_.map_color_rgbcra = &XWPSDeviceCompositeAlpha::mapColorRGBDCA;
	procs.fill_rectangle_.fill_rectanglecra = &XWPSDeviceCompositeAlpha::fillRectangleDCA;
	procs.copy_mono_.copy_monocra = &XWPSDeviceCompositeAlpha::copyMonoDCA;
	procs.copy_color_.copy_colorcra = &XWPSDeviceCompositeAlpha::copyColorDCA;
	procs.get_bits_.get_bits = &XWPSDevice::getBitsDefault;
	procs.map_cmyk_color_.map_cmyk_color = &XWPSDevice::mapCMYKColorDefault;
	procs.map_rgb_alpha_color_.map_rgb_alpha_colorcra = &XWPSDeviceCompositeAlpha::mapRGBAlphaColorDCA;
	procs.copy_alpha_.copy_alphacra = &XWPSDeviceCompositeAlpha::copyAlphaDCA;
	procs.copy_rop_.copy_rop = &XWPSDevice::copyRopDefault;
	procs.fill_path_.fill_path = &XWPSDevice::fillPathDefault;
	procs.stroke_path_.stroke_path = &XWPSDevice::strokePathDefault;
	procs.fill_mask_.fill_mask = &XWPSDevice::fillMaskDefault;
	procs.fill_trapezoid_.fill_trapezoid = &XWPSDevice::fillTrapezoidDefault;
	procs.fill_parallelogram_.fill_parallelogram = &XWPSDevice::fillParallelogramDefault;
	procs.fill_triangle_.fill_triangle = &XWPSDevice::fillTriangleDefault;
	procs.draw_thin_line_.draw_thin_line = &XWPSDevice::drawThinLineDefault;
	procs.begin_image_.begin_image = &XWPSDevice::beginImageDefault;
	procs.strip_copy_rop_.strip_copy_rop = &XWPSDevice::stripCopyRopDefault;
	procs.begin_typed_image_.begin_typed_image = &XWPSDevice::beginTypedImageDefault;
	procs.map_color_rgb_alpha_.map_color_rgb_alphacra = &XWPSDeviceCompositeAlpha::mapColorRGBAlphaDCA;
	procs.create_compositor_.create_compositor = &XWPSDevice::createCompositorNo;
}

int XWPSDeviceCompositeAlpha::copyAlphaDCA(const uchar * data, 
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
	return copyAlphaDefault(data, data_x, raster, id, x, y, widthA, heightA, color, depth);
}

int XWPSDeviceCompositeAlpha::copyColorDCA(const uchar * data,
	       					 int dx, 
	       					 int raster, 
	       					 ulong id,
	                 int x, 
	                 int y, 
	                 int w, 
	                 int h)
{
	return copyColorDefault(data, dx, raster, id, x, y, w, h);
}

int  XWPSDeviceCompositeAlpha::copyDevice(XWPSDevice **pnew)
{
	XWPSDeviceCompositeAlpha * ret = new XWPSDeviceCompositeAlpha;
	ret->copyDeviceParamDCA(this);
	*pnew = ret;
	return 0;
}

void XWPSDeviceCompositeAlpha::copyDeviceParamDCA(XWPSDeviceCompositeAlpha * proto)
{
	copyDeviceParam(proto);
	params = proto->params;
}

int XWPSDeviceCompositeAlpha::copyMonoDCA(const uchar * data,
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
	return copyMonoDefault(data, dx, raster, id, x, y, w, h,zero, one);
}

int XWPSDeviceCompositeAlpha::fillRectangleDCA(int x, int y, int w, int h, ulong color)
{
	uchar *std_row;
  uchar *native_row;
  XWPSIntRect rect;
  XWPSGetBitsParams std_params, native_params;
  int code = 0;
  int yi;
  PSCompositeParams cp;
  PSPixelRow source;
  PSPixelRow dest;

  fit_fill(x, y, w, h);
  std_row = new uchar[(color_info.depth * w + 7) >> 3]; 
  native_row = new uchar[(target->color_info.depth * w + 7) >> 3]; 
  rect.p.x = x, rect.q.x = x + w;
  std_params.options =	gbcolorForDevice() |	(GB_ALPHA_LAST | GB_DEPTH_8 | GB_PACKING_CHUNKY | GB_RETURN_COPY | 
  							GB_RETURN_POINTER | GB_ALIGN_ANY | GB_OFFSET_0 | GB_OFFSET_ANY | GB_RASTER_STANDARD | GB_RASTER_ANY);
  cp.cop = params.op;
  if (cp.cop == composite_Dissolve)
		cp.delta = params.delta;
  {
		ushort rgba[4];

		mapColorRGBAlpha(color, rgba);
		cp.source_values[0] = ps_color_value_to_byte(rgba[0]);
		cp.source_values[1] = ps_color_value_to_byte(rgba[1]);
		cp.source_values[2] = ps_color_value_to_byte(rgba[2]);
		cp.source_alpha = ps_color_value_to_byte(rgba[3]);
  }
  
  source.bits_per_value = 8;
  source.alpha = ps_image_alpha_none;
  for (yi = y; yi < y + h; ++yi) 
  {
		rect.p.y = yi, rect.q.y = yi + 1;
		std_params.data[0] = std_row;
		code = target->getBitsRectangle(&rect, &std_params, NULL);
		if (code < 0)
	    break;
	    
		dest.data = std_params.data[0];
		dest.bits_per_value = 8;
		dest.initial_x =  (std_params.options & GB_OFFSET_ANY ? std_params.x_offset : 0);
		dest.alpha =  (std_params.options & GB_ALPHA_FIRST ? ps_image_alpha_first :
	     			std_params.options & GB_ALPHA_LAST ? ps_image_alpha_last : ps_image_alpha_none);
		code = composite_values(&dest, &source, color_info.num_components, w, &cp);
		if (code < 0)
	    break;
		if (std_params.data[0] == std_row) 
		{
	    native_params.options =	(GB_COLORS_NATIVE | GB_PACKING_CHUNKY | GB_RETURN_COPY | GB_OFFSET_0 | GB_RASTER_ALL | GB_ALIGN_STANDARD);
	    native_params.data[0] = native_row;
	    code = target->getBitsCopy(0, w, 1, &native_params,  &std_params, std_row,  0);
	    if (code < 0)
				break;
	    code = target->copyColor(native_row, 0, 0 , ps_no_bitmap_id, x, yi, w, 1);
	    if (code < 0)
				break;
		}
  }
  
//out:
	delete [] native_row;
  delete [] std_row;
  return code;
}

int XWPSDeviceCompositeAlpha::mapColorRGBAlphaDCA(ulong color, ushort * prgba)
{
	prgba[3] = ps_color_value_from_byte((uchar) color);
  return mapColorRGBDCA(color, prgba);
}

int XWPSDeviceCompositeAlpha::mapColorRGBDCA(ulong color, ushort * prgb)
{
	ushort red = ps_color_value_from_byte((uchar) (color >> 24));
  uchar a = (uchar) color;

#define postdiv_(c)\
  (((c) * 0xff + a / 2) / a)
#ifdef PREMULTIPLY_TOWARDS_WHITE
    uchar bias = ~a;

#  define postdiv(c) postdiv_(c - bias)
#else
#  define postdiv(c) postdiv_(c)
#endif

  if (color_info.num_components == 1) 
  {
		if (a != 0xff) 
		{
	    if (a == 0)
				red = 0;
	    else
				red = postdiv(red);
		}
		prgb[0] = prgb[1] = prgb[2] = red;
  } 
  else 
  {
		ushort  green = ps_color_value_from_byte((uchar) (color >> 16)), blue = ps_color_value_from_byte((uchar) (color >> 8));

		if (a != 0xff) 
		{
	    if (a == 0)
				red = green = blue = 0;
	    else 
	    {
				red = postdiv(red);
				green = postdiv(green);
				blue = postdiv(blue);
	    }
		}
		prgb[0] = red, prgb[1] = green, prgb[2] = blue;
  }
#undef postdiv
  return 0;
}

ulong XWPSDeviceCompositeAlpha::mapRGBAlphaColorDCA(ushort red, ushort green, ushort blue, ushort alpha)
{
	uchar a = ps_color_value_to_byte(alpha);

#define premult_(c)\
  (((c) * a + ps_max_color_value / 2) / ps_max_color_value)
#ifdef PREMULTIPLY_TOWARDS_WHITE
    uchar bias = ~a;

#  define premult(c) (premult_(c) + bias)
#else
#  define premult(c) premult_(c)
#endif
    
  ulong color;

  if (color_info.num_components == 1) 
  {
		uint lum =(red * lum_red_weight + green * lum_green_weight + blue * lum_blue_weight + lum_all_weights / 2) /	lum_all_weights;

		if (a == 0xff)
	   	color = ps_color_value_to_byte(lum);
		else
	   	color = premult(lum);
  } 
  else 
  {
		if (a == 0xff)
	   	color =	((uint) ps_color_value_to_byte(red) << 16) +	((uint) ps_color_value_to_byte(green) << 8) + ps_color_value_to_byte(blue);
		else
	   	color =	(premult(red) << 16) + (premult(green) << 8) + premult(blue);
  }
#undef premult
  return (color << 8) + a;
}

ulong XWPSDeviceCompositeAlpha::mapRGBColorDCA(ushort r, ushort g, ushort b)
{
	return mapRGBAlphaColorDCA(r, g, b, ps_max_color_value);
}
