/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWPSError.h"
#include "XWPSPattern.h"
#include "XWPSDeviceMem.h"
#include "XWPSState.h"
#include "XWPSDevicePatternAccum.h"

XWPSDevicePatternAccum::XWPSDevicePatternAccum(QObject * parent)
	:XWPSDeviceForward("pattern accumulator", 0, 0, 72, 72, 1, 1, 1, 0, 2, 0, parent)
{
	procs.open__.openpatta = &XWPSDevicePatternAccum::openPatternAccum;
	procs.close__.closepatta = &XWPSDevicePatternAccum::closePatternAccum;
	procs.fill_rectangle_.fill_rectanglepatta = &XWPSDevicePatternAccum::fillRectanglePatternAccum;
	procs.tile_rectangle_.tile_rectangle = &XWPSDevice::tileRectangleDefault;
	procs.copy_mono_.copy_monopatta = &XWPSDevicePatternAccum::copyMonoPatternAccum;
	procs.copy_color_.copy_colorpatta = &XWPSDevicePatternAccum::copyColorPatternAccum;
	procs.get_bits_.get_bits = &XWPSDevice::getBitsDefault;
	procs.copy_alpha_.copy_alpha = &XWPSDevice::copyAlphaDefault;
	procs.copy_rop_.copy_rop = &XWPSDevice::copyRopDefault;
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
	procs.strip_copy_rop_.strip_copy_rop = &XWPSDevice::stripCopyRopDefault;
	procs.get_clipping_box_.get_clipping_box = &XWPSDevice::getClipingBoxLarge;
	procs.begin_typed_image_.begin_typed_image = &XWPSDevice::beginTypedImageDefault;
	procs.get_bits_rectangle_.get_bits_rectanglepatta = &XWPSDevicePatternAccum::getBitsRectanglePatternAccum;
	procs.text_begin_.text_begin = &XWPSDevice::textBeginDefault;
		
	instance = 0;
	bits = 0;
	mask = 0;
}

XWPSDevicePatternAccum::~XWPSDevicePatternAccum()
{
	if (bits)
	{
		if (!bits->decRef())
			delete bits;
			
		bits = 0;
	}
	
	if (mask)
	{
		if (!mask->decRef())
			delete mask;
			
		mask = 0;
	}
}

int XWPSDevicePatternAccum::closePatternAccum()
{
	setTarget(NULL);
  if (bits)
	{
		if (!bits->decRef())
			delete bits;
			
		bits = 0;
	}
	
  if (mask != 0) 
  {
    mask->close();
    delete mask;
    mask = 0;
  }
  return 0;
}

int XWPSDevicePatternAccum::copyColorPatternAccum(const uchar * data, 
	                          int data_x,
		                        int raster, 
		                        ulong id, 
		                        int x, 
		                        int y, 
		                        int w, 
		                        int h)
{
	if (bits)
		target->copyColor(data, data_x, raster, id, x, y, w, h);
  if (mask)
    return mask->fillRectangle(x, y, w, h, 1);
  else
    return 0;
}

int  XWPSDevicePatternAccum::copyDevice(XWPSDevice **pnew)
{
	XWPSDevicePatternAccum * ret = new XWPSDevicePatternAccum;
	ret->copyDeviceParam(this);
	*pnew = ret;
	return 0;
}

int XWPSDevicePatternAccum::copyMonoPatternAccum(const uchar * data, 
	                         int data_x,
		                       int raster, 
		                       ulong id, 
		                       int x, 
		                       int y, 
		                       int w, 
		                       int h,
			                     ulong color0, 
			                     ulong color1)
{
	if (bits)
		target->copyMono(data, data_x, raster, id, x, y, w, h, color0, color1);
  if (mask) 
  {
    if (color0 != ps_no_color_index)
	    color0 = 1;
    if (color1 != ps_no_color_index)
	    color1 = 1;
    if (color0 == 1 && color1 == 1)
	    return mask->fillRectangle(x, y, w, h, 1);
    else
	    return mask->copyMono(data, data_x, raster, id, x, y, w, h, color0, color1);
  } 
  else
    return 0;
}

int XWPSDevicePatternAccum::fillRectanglePatternAccum(int x, int y, int w, int h, ulong color)
{
	if (bits)
		target->fillRectangle(x, y, w, h, color);
  if (mask)
    return mask->fillRectangle(x, y, w, h, 1);
  else
    return 0;
}

int XWPSDevicePatternAccum::getBitsRectanglePatternAccum(XWPSIntRect * prect,
		                               XWPSGetBitsParams * params, 
		                               XWPSIntRect ** unread)
{
	if (bits)
		return target->getBitsRectangle(prect, params, unread);
  return (int)(XWPSError::Fatal);
}

int XWPSDevicePatternAccum::openPatternAccum()
{
	int code = 0;
  bool mask_open = false;
    
	const XWPSPattern1Instance *pinst = instance;
	width = pinst->size.x;
	height = pinst->size.y;
	if (!target)
	{
		target = pinst->saved->currentDevice();
		if (target)
			target->incRef();
	}
		
	HWResolution[0] = target->HWResolution[0];
	HWResolution[1] = target->HWResolution[1];
	color_info = target->color_info;
	
	if (pinst->uses_mask) 
	{
      mask = new XWPSDeviceMem;
      mask->makeMemMonoDevice(0);
      mask->width = pinst->size.x;
			mask->height = pinst->size.y;
			mask->HWResolution[0] = target->HWResolution[0];
			mask->HWResolution[1] = target->HWResolution[1];
      code = mask->open();
      if (code >= 0) 
      {
	    	mask_open = true;
	    	memset(mask->base, 0, mask->raster * mask->height);
			}
  }
  if (code >= 0) 
  {
		switch (pinst->templat.PaintType) 
		{
	    case 2:		/* uncolored */
				setTarget(target);
				break;
				
	    	case 1:
					bits = new XWPSDeviceMem;
		    	bits->makeMemDevice(target->color_info.depth, -1, target);
		    	bits->width = pinst->size.x;
					bits->height = pinst->size.y;
					bits->HWResolution[0] = target->HWResolution[0];
					bits->HWResolution[1] = target->HWResolution[1];
		    
		    	bits->color_info = target->color_info;
		    	code = bits->open();
		    	setTarget(bits);
		}
  }
  
  if (code < 0) 
 	{
		if (bits != 0)
		{
	    delete bits;
	    bits = 0;
	  }
    if (mask != 0) 
    {
	    if (mask_open)
				mask->close();
	    delete mask;
	    mask = 0;
    }
			return code;
  }
  return code;
}
