/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWPSBitmap.h"
#include "XWPSPath.h"
#include "XWPSDeviceClip.h"

XWPSDeviceClip::XWPSDeviceClip(QObject * parent)
	:XWPSDeviceForward("clipper", 0, 0, 1, 1, 1, 1, 1, 0, 2, 0, parent)
{
	procs.open__.openclip = &XWPSDeviceClip::openClip;
	procs.fill_rectangle_.fill_rectangleclip = &XWPSDeviceClip::fillRectangleClip;
	procs.copy_mono_.copy_monoclip = &XWPSDeviceClip::copyMonoClip;
	procs.copy_color_.copy_colorclip = &XWPSDeviceClip::copyColorClip;
	procs.copy_alpha_.copy_alphaclip = &XWPSDeviceClip::copyAlphaClip;
	procs.copy_rop_.copy_rop = &XWPSDevice::copyRopDefault;
	procs.fill_path_.fill_path = &XWPSDevice::fillPathDefault;
	procs.stroke_path_.stroke_path = &XWPSDevice::strokePathDefault;
	procs.fill_mask_.fill_maskclip = &XWPSDeviceClip::fillMaskClip;
	procs.fill_trapezoid_.fill_trapezoid = &XWPSDevice::fillTrapezoidDefault;
	procs.fill_parallelogram_.fill_parallelogram = &XWPSDevice::fillParallelogramDefault;
	procs.fill_triangle_.fill_triangle = &XWPSDevice::fillTriangleDefault;
	procs.draw_thin_line_.draw_thin_line = &XWPSDevice::drawThinLineDefault;
	procs.begin_image_.begin_image = &XWPSDevice::beginImageDefault;
	procs.strip_tile_rectangle_.strip_tile_rectangleclip = &XWPSDeviceClip::stripTileRectangleClip;
	procs.strip_copy_rop_.strip_copy_ropclip = &XWPSDeviceClip::stripCopyRopClip;
	procs.get_clipping_box_.get_clipping_boxclip = &XWPSDeviceClip::getClipingBoxClip;
	procs.begin_typed_image_.begin_typed_image = &XWPSDevice::beginTypedImageDefault;
	procs.get_bits_rectangle_.get_bits_rectangleclip = &XWPSDeviceClip::getBitsRectangleClip;
	procs.text_begin_.text_begin = &XWPSDevice::textBeginDefault;
	list = 0;
	current = 0;
	clipping_box_set = false;
}

XWPSDeviceClip::~XWPSDeviceClip()
{
}

int XWPSDeviceClip::copyAlphaClip(const uchar *data, 
	                      int sourcex,
	                      int raster, 
	                      ulong , 
	                      int x, 
	                      int y, 
	                      int w, 
	                      int h,
	                      ulong color, 
	                      int depth)
{
	XWPSClipCallbackData ccdata(target);
	ccdata.data = data, ccdata.sourcex = sourcex, ccdata.raster = raster;
  ccdata.color[0] = color, ccdata.depth = depth;
  return clipEnumerate(x, y, w, h, &ccdata, &XWPSClipCallbackData::copyAlpha);
}

int XWPSDeviceClip::copyColorClip(const uchar *data, 
	                      int sourcex, 
	                      int raster, 
	                      ulong ,
	                      int x, 
	                      int y, 
	                      int w, 
	                      int h)
{
	XWPSClipCallbackData ccdata(target);
	ccdata.data = data, ccdata.sourcex = sourcex, ccdata.raster = raster;
  return clipEnumerate(x, y, w, h, &ccdata, &XWPSClipCallbackData::copyColor);
}

int  XWPSDeviceClip::copyDevice(XWPSDevice **pnew)
{
	XWPSDeviceClip * ret = new XWPSDeviceClip;
	ret->copyDeviceParamClip(this);
	*pnew = ret;
	return 0;
}

void XWPSDeviceClip::copyDeviceParamClip(XWPSDeviceClip * proto)
{
	copyDeviceParam(proto);
	translation = proto->translation;
	clipping_box = proto->clipping_box;
	clipping_box_set = proto->clipping_box_set;
}

int XWPSDeviceClip::copyMonoClip(const uchar *data, 
	                     int sourcex, 
	                     int raster, 
	                     ulong id,
	                     int x, 
	                     int y, 
	                     int w, 
	                     int h,
	                     ulong color0, 
	                     ulong color1)
{
	XWPSClipCallbackData ccdata(target);
	XWPSClipRect *rptr = current;
	int xe, ye;

  if (w <= 0 || h <= 0)
		return 0;
  x += translation.x;
  xe = x + w;
  y += translation.y;
  ye = y + h;
  if (y >= rptr->ymin && ye <= rptr->ymax) 
  {
		if (x >= rptr->xmin && xe <= rptr->xmax) 
	    return target->copyMono(data, sourcex, raster, id, x, y, w, h, color0, color1);
  }
  ccdata.data = data, ccdata.sourcex = sourcex, ccdata.raster = raster;
  ccdata.color[0] = color0, ccdata.color[1] = color1;
  return clipEnumerateRest(x, y, xe, ye,&ccdata, &XWPSClipCallbackData::copyMono);
}

int  XWPSDeviceClip::fillMaskClip(const uchar * data, 
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
	XWPSClipCallbackData ccdata;

  if (pcpath != 0)
  	return fillMask(data, sourcex, raster, id, x, y, w, h, pdcolor, depth, lop, pcpath);
  	
  ccdata.data = data, ccdata.sourcex = sourcex, ccdata.raster = raster;
  ccdata.pdcolor = pdcolor, ccdata.depth = depth, ccdata.lop = lop;
  return clipEnumerate(x, y, w, h, &ccdata, &XWPSClipCallbackData::fillMask);
}

int XWPSDeviceClip::fillRectangleClip(int x, int y, int w, int h, ulong color)
{
	XWPSClipCallbackData ccdata(target);
	XWPSClipRect *rptr = current;
	int xe, ye;

  if (w <= 0 || h <= 0)
		return 0;
  x += translation.x;
  xe = x + w;
  y += translation.y;
  ye = y + h;
  
  if ((y >= rptr->ymin && ye <= rptr->ymax) ||
		  ((rptr = rptr->next) != 0 &&
	 	   y >= rptr->ymin && ye <= rptr->ymax)) 
	{
		current = rptr;
		if (x >= rptr->xmin && xe <= rptr->xmax) 
	    return target->fillRectangle(x, y, w, h, color);
		else if ((rptr->prev == 0 || rptr->prev->ymax != rptr->ymax) &&
		 					(rptr->next == 0 || rptr->next->ymax != rptr->ymax)) 
		{
	    if (x < rptr->xmin)
				x = rptr->xmin;
	    if (xe > rptr->xmax)
				xe = rptr->xmax;
	    return	(x >= xe ? 0 : target->fillRectangle(x, y, xe - x, h, color));
		}
  }
  ccdata.color[0] = color;
  return clipEnumerateRest(x, y, xe, ye, &ccdata, &XWPSClipCallbackData::fillRectangle);
}

int XWPSDeviceClip::getBitsRectangleClip(XWPSIntRect *prect,
	                             XWPSGetBitsParams *params, 
	                             XWPSIntRect **unread)
{
	int tx = translation.x, ty = translation.y;
  XWPSIntRect rect;
  int code;

  rect.p.x = prect->p.x - tx, rect.p.y = prect->p.y - ty;
  rect.q.x = prect->q.x - tx, rect.q.y = prect->q.y - ty;
  code = target->getBitsRectangle(&rect, params, unread);
  if (code > 0) 
  {
		XWPSIntRect *listA = *unread;
		for (int i = 0; i < code; ++list, ++i) 
		{
	    listA->p.x += tx, listA->p.y += ty;
	    listA->q.x += tx, listA->q.y += ty;
		}
  }
  return code;
}

void XWPSDeviceClip::getClipingBoxClip(XWPSFixedRect*pbox)
{
	if (!clipping_box_set)
	{
		XWPSFixedRect tbox;
		target->getClipingBox(&tbox);
		if (list->count != 0)
		{
			XWPSFixedRect cbox;

	    if (list->count == 1) 
	    {
				cbox.p.x = int2fixed(list->single.xmin);
				cbox.p.y = int2fixed(list->single.ymin);
				cbox.q.x = int2fixed(list->single.xmax);
				cbox.q.y = int2fixed(list->single.ymax);
	    } 
	    else 
	    {
				cbox.p.x = int2fixed(list->xmin);
				cbox.p.y = int2fixed(list->head->next->ymin);
				cbox.q.x = int2fixed(list->xmax);
				cbox.q.y = int2fixed(list->tail->prev->ymax);
	    }
	    tbox.intersect(cbox);
		}
		
		if (translation.x | translation.y)
		{
			long tx = int2fixed(translation.x),
			ty = int2fixed(translation.y);

	    if (tbox.p.x != min_fixed)
				tbox.p.x -= tx;
	    if (tbox.p.y != min_fixed)
				tbox.p.y -= ty;
	    if (tbox.q.x != max_fixed)
				tbox.q.x -= tx;
	    if (tbox.q.y != max_fixed)
				tbox.q.y -= ty;
		}
		
		clipping_box = tbox;
		clipping_box_set = true;
	}
	
	*pbox = clipping_box;
}

void XWPSDeviceClip::makeClipPathDevice(XWPSClipPath * pcpath)
{
	makeClipTranslateDevice(pcpath->getList(), 0, 0);
}

void XWPSDeviceClip::makeClipTranslateDevice(XWPSClipList * listA, int tx, int ty)
{
	list = listA;
  translation.x = tx;
  translation.y = ty;
}

int XWPSDeviceClip::openClip()
{
	current =	(list->head == 0 ? &list->single : list->head);
  color_info = target->color_info;
  cached_colors = target->cached_colors;
  width = target->width;
  height = target->height;
  clipping_box_set = false;
  return 0;
}

int XWPSDeviceClip::stripCopyRopClip(const uchar *sdata, 
	                         int sourcex, 
	                         uint raster, 
	                         ulong ,
	                         const ulong *scolors,
	                         XWPSStripBitmap *textures, 
	                         const ulong *tcolors,
	                         int x, 
	                         int y, 
	                         int w, 
	                         int h,
	                         int phase_x, 
	                         int phase_y, 
	                         ulong lop)
{
	XWPSClipCallbackData ccdata;

  ccdata.data = sdata, ccdata.sourcex = sourcex, ccdata.raster = raster;
  ccdata.scolors = scolors, ccdata.textures = textures,
	ccdata.tcolors = tcolors;
  ccdata.phase.x = phase_x, ccdata.phase.y = phase_y, ccdata.lop = lop;
  return clipEnumerate(x, y, w, h, &ccdata, &XWPSClipCallbackData::stripCopyRop);
}

int XWPSDeviceClip::stripTileRectangleClip(XWPSStripBitmap * tiles,
			      										 int x, 
			      										 int y, 
			      										 int w, 
			      										 int h,
			      										 ulong color0, 
			      										 ulong color1,
			      										 int phase_x, 
			      										 int phase_y)
{
	XWPSClipCallbackData ccdata;
  ccdata.tiles = tiles;
  ccdata.color[0] = color0, ccdata.color[1] = color1;
  ccdata.phase.x = phase_x, ccdata.phase.y = phase_y;
  return clipEnumerate(x, y, w, h, &ccdata, &XWPSClipCallbackData::stripTileRectangle);
}

void XWPSDeviceClip::translate(XWPSClipList * listA, int tx, int ty)
{
	list = listA;
	translation.x = tx;
  translation.y = ty;
}

int XWPSDeviceClip::clipEnumerate(int x, 
	                                int y, 
	                                int w, 
	                                int h,
	                                XWPSClipCallbackData * pccd,
	                                int (XWPSClipCallbackData::*process)(int xc, int yc, int xec, int yec))
{
	 int xe, ye;
   const XWPSClipRect *rptr = current;

   if (w <= 0 || h <= 0)
			return 0;
   pccd->tdev = target;
   x += translation.x;
   xe = x + w;
   y += translation.y;
   ye = y + h;
   if (y >= rptr->ymin && ye <= rptr->ymax && x >= rptr->xmin && xe <= rptr->xmax) 
   {
			pccd->x = x, pccd->y = y, pccd->w = w, pccd->h = h;
			return (pccd->*process)(x, y, xe, ye);
   }
   return clipEnumerateRest(x, y, xe, ye, pccd, process);
}

int XWPSDeviceClip::clipEnumerateRest(int x, 
	                                    int y, 
	                                    int xe, 
	                                    int ye,
	                                    XWPSClipCallbackData * pccd,
		                                  int (XWPSClipCallbackData::*process)(int xc, int yc, int xec, int yec))
{
	XWPSClipRect *rptr = current;
	int yc;
  int code;
  pccd->x = x, pccd->y = y;
  pccd->w = xe - x, pccd->h = ye - y;
  if (y >= rptr->ymax) 
  {
		if ((rptr = rptr->next) != 0)
	    while (y >= rptr->ymax)
				rptr = rptr->next;
  } 
  else
		while (rptr->prev != 0 && y < rptr->prev->ymax)
	    rptr = rptr->prev;
	    
	if (rptr == 0 || (yc = rptr->ymin) >= ye) 
	{
		if (list->count > 1)
	    current =	(rptr != 0 ? rptr :	 y >= current->ymax ? list->tail : list->head);
		return 0;
  }
  current = rptr;
  if (yc < y)
		yc = y;
		
	do
	{
		const int ymax = rptr->ymax;
		int yec = qMin(ymax, ye);
		do
		{
			int xc = rptr->xmin;
	    int xec = rptr->xmax;

	    if (xc < x)
				xc = x;
	    if (xec > xe)
				xec = xe;
				
			if (xec > xc)
			{
				if (xec - xc == pccd->w)
				{
					while ((rptr = rptr->next) != 0 &&
			            rptr->ymin == yec &&
			            rptr->ymax <= ye &&
			            rptr->xmin <= x &&
			            rptr->xmax >= xe)
			     yec = rptr->ymax;
				}
				else
					rptr = rptr->next;
					
				code = (pccd->*process)(xc, yc, xec, yec);
				if (code < 0)
		    	return code;
	    } 
	    else 
				rptr = rptr->next;
				
	    if (rptr == 0)
				return 0;
		} while(rptr->ymax == ymax);
	} while((yc = rptr->ymin) < ye);
	
	return 0;
}
