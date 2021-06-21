/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "psbittable.h"
#include "XWPSBitmap.h"
#include "XWPSDeviceMem.h"
#include "XWPSDeviceMaskClip.h"

#define DECLARE_MASK_COPY\
	const uchar *sdata;\
	int sx, mx0, my0, mx1, my1
#define FIT_MASK_COPY(data, sourcex, raster, vx, vy, vw, vh)\
	  do{sdata = data, sx = sourcex;\
	  mx0 = vx + phase.x, my0 = vy + phase.y;\
	  mx1 = mx0 + vw, my1 = my0 + vh;\
	  if ( mx0 < 0 )\
	    sx -= mx0, mx0 = 0;\
	  if ( my0 < 0 )\
	    sdata -= my0 * raster, my0 = 0;\
	  if ( mx1 > tiles.size.x )\
	    mx1 = tiles.size.x;\
	  if ( my1 > tiles.size.y )\
	  my1 = tiles.size.y;}while(0)
	    
#define x_offset(ty)\
  (phase.x + (((ty) + phase.y) / tiles.rep_height) *\
   tiles.rep_shift)
   
#define t_next(tx)\
    do{if ( ++cx == tiles.size.x )\
      cx = 0, tp = tile_row, tbit = 0x80;\
    else if ( (tbit >>= 1) == 0 )\
      tp++, tbit = 0x80;\
    tx++;}while(0)
    
#define FOR_RUNS(data_row, tx1, tx, ty)\
	const uchar *data_row = data;\
	int cy = (y + phase.y) % tiles.rep_height;\
	uchar *tile_row = tiles.data + cy * tiles.raster;\
	int ty;\
\
	for ( ty = y; ty < y + h; ty++, data_row += raster ) {\
	  int cx = (x + x_offset(ty)) % tiles.rep_width;\
	  const uchar *tp = tile_row + (cx >> 3);\
	  uchar tbit = 0x80 >> (cx & 7);\
	  int tx;\
\
	  for ( tx = x; tx < x + w; ) {\
	    int tx1;\
\
	    /* Skip a run of 0s. */\
	    while ( tx < x + w && (*tp & tbit) == 0 )\
	      t_next(tx);\
	    if ( tx == x + w )\
	      break;\
	    /* Scan a run of 1s. */\
	    tx1 = tx;\
	    do {\
	      t_next(tx);\
	    } while ( tx < x + w && (*tp & tbit) != 0 );
	    
/* (body goes here) */
#define END_FOR_RUNS()\
	  }\
	  if ( ++cy == tiles.size.y )\
	    cy = 0, tile_row = tiles.data;\
	  else\
	    tile_row += tiles.raster;\
	}

XWPSDeviceMaskClip::XWPSDeviceMaskClip(QObject * parent)
	:XWPSDeviceForward("mask clipper", 0, 0, 1, 1, 1, 1, 1, 0, 2, 0, parent)
{
	procs.fill_rectangle_.fill_rectanglemac = &XWPSDeviceMaskClip::fillRectangleMaskClip;
	procs.copy_mono_.copy_monomac = &XWPSDeviceMaskClip::copyMonoMaskClip;
	procs.copy_color_.copy_colormac = &XWPSDeviceMaskClip::copyColorMaskClip;
	procs.copy_alpha_.copy_alphamac = &XWPSDeviceMaskClip::copyAlphaMaskClip;
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
	procs.strip_tile_rectangle_.strip_tile_rectanglemac = &XWPSDeviceMaskClip::stripTileRectangleMaskClip;
	procs.strip_copy_rop_.strip_copy_ropmac = &XWPSDeviceMaskClip::stripCopyRopMaskClip;
	procs.get_clipping_box_.get_clipping_boxmac = &XWPSDeviceMaskClip::getClipingBoxMaskClip;
	procs.begin_typed_image_.begin_typed_image = &XWPSDevice::beginTypedImageDefault;
	procs.text_begin_.text_begin = &XWPSDevice::textBeginDefault;
		
	mdev = new XWPSDeviceMem;
}

XWPSDeviceMaskClip::~XWPSDeviceMaskClip()
{
	if (mdev)
	{
		if (mdev->base == buffer.bytes)
			mdev->base = 0;
			
		if (!mdev->decRef())
			delete mdev;
		mdev = 0;
	}
}

int XWPSDeviceMaskClip::copyAlphaMaskClip(const uchar * data, 
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
	XWPSClipCallbackData ccdata;

  ccdata.tdev = target;
  ccdata.data = data, ccdata.sourcex = sourcex, ccdata.raster = raster;
  ccdata.x = x, ccdata.y = y, ccdata.w = w, ccdata.h = h;
  ccdata.color[0] = color, ccdata.depth = depth;
  return clipRunsEnumerate(&XWPSClipCallbackData::copyAlpha, &ccdata);
}

int XWPSDeviceMaskClip::copyAlphaTileClip(const uchar * data, 
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
	FOR_RUNS(data_row, txrun, tx, ty) 
	{
		int code = target->copyAlpha(data_row, sourcex + txrun - x, raster, ps_no_bitmap_id, txrun, ty, tx - txrun, 1, color, depth);

		if (code < 0)
	    return code;
  }
  END_FOR_RUNS();
  return 0;
}

int XWPSDeviceMaskClip::copyColorMaskClip(const uchar * data, 
	                      int sourcex, 
	                      int raster, 
	                      ulong ,
		     							  int x, 
		     							  int y, 
		     							  int w, 
		     							  int h)
{
	XWPSClipCallbackData ccdata;

  ccdata.tdev = target;
  ccdata.data = data, ccdata.sourcex = sourcex, ccdata.raster = raster;
  ccdata.x = x, ccdata.y = y, ccdata.w = w, ccdata.h = h;
  return clipRunsEnumerate(&XWPSClipCallbackData::copyColor, &ccdata);
}

int XWPSDeviceMaskClip::copyColorTileClip(const uchar * data, 
	                      int sourcex, 
	                      int raster, 
	                      ulong ,
		     							  int x, 
		     							  int y, 
		     							  int w, 
		     							  int h)
{
	FOR_RUNS(data_row, txrun, tx, ty) 
	{
		int code = target->copyColor(data_row, sourcex + txrun - x, raster,	 ps_no_bitmap_id, txrun, ty, tx - txrun, 1);

		if (code < 0)
	    return code;
  }
  END_FOR_RUNS();
  return 0;
}

int  XWPSDeviceMaskClip::copyDevice(XWPSDevice **pnew)
{
	XWPSDeviceMaskClip * ret = new XWPSDeviceMaskClip;
	ret->copyDeviceParamMaskClip(this);
	ret->copyDeviceProc(this);
	*pnew = ret;
	return 0;
}

void XWPSDeviceMaskClip::copyDeviceParamMaskClip(XWPSDeviceMaskClip * proto)
{
	copyDeviceParam(proto);
	tiles = proto->tiles;
	phase = proto->phase;
}

int XWPSDeviceMaskClip::copyMonoMaskClip(const uchar * data, 
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
	ulong color, mcolor0, mcolor1;

  DECLARE_MASK_COPY;
  int cy, ny;
  int code;

	if (mdev->base == 0 )
		return copyMonoDefault(data, sourcex, raster, id,	x, y, w, h, color0, color1);
	
	if ( color1 != ps_no_color_index )
	{
		if ( color0 != ps_no_color_index )
		{
			code = fillRectangle(x, y, w, h, color0);
	    if ( code < 0 )
				return code;
		}
		
		color = color1;
	  mcolor0 = 0;
	  mcolor1 = ps_no_color_index;
	}
	else if ( color0 != ps_no_color_index)
	{
		color = color0;
	  mcolor0 = ps_no_color_index; 
	  mcolor1 = 0;
	}
	else
		return 0;
		
  FIT_MASK_COPY(data, sourcex, raster, x, y, w, h);
  for (cy = my0; cy < my1; cy += ny)
  {
  	int ty = cy - phase.y;
		int cx, nx;

		ny = my1 - cy;
		if (ny > mdev->height)
	    ny = mdev->height;
		for (cx = mx0; cx < mx1; cx += nx)
		{
			int tx = cx - phase.x;
	    nx = mx1 - cx;
	    memcpy(buffer.bytes, tiles.data + cy * tiles.raster, tiles.raster * ny);
	    mdev->copyMono(sdata + (ty - y) * raster, sx + tx - x, raster, ps_no_bitmap_id, cx, 0, nx, ny, mcolor0, mcolor1);
	    code = target->copyMono(buffer.bytes, cx, tiles.raster, ps_no_bitmap_id, tx, ty, nx, ny, ps_no_color_index, color);
	    if (code < 0)
				return code;
		}
  }
  
  return 0;
}

int XWPSDeviceMaskClip::copyMonoTileClip(const uchar * data, 
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
	ulong color, mcolor0, mcolor1;
  int ty, ny;
  int code;
  
  if (mdev->base == 0 )
		return copyMonoDefault(data, sourcex, raster, id,	x, y, w, h, color0, color1);
	
	if ( color1 != ps_no_color_index )
	{
		if ( color0 != ps_no_color_index )
		{
			code = fillRectangle(x, y, w, h, color0);
	    if ( code < 0 )
				return code;
		}
		
		color = color1;
	  mcolor0 = 0;
	  mcolor1 = ps_no_color_index;
	}
	else if ( color0 != ps_no_color_index)
	{
		color = color0;
	  mcolor0 = ps_no_color_index; 
	  mcolor1 = 0;
	}
	else
		return 0;
		
	for (ty = y; ty < y + h; ty += ny)
	{
		int tx, nx;
		int cy = (ty + phase.y) % tiles.rep_height;
		int xoff = x_offset(ty);

		ny = qMin(y + h - ty, tiles.size.y - cy);
		if (ny > mdev->height)
	    ny = mdev->height;
		for (tx = x; tx < x + w; tx += nx)
		{
			int cx = (tx + xoff) % tiles.rep_width;

	    nx = qMin(x + w - tx, tiles.size.x - cx);
	    memcpy(buffer.bytes, tiles.data + cy * tiles.raster, tiles.raster * ny);
	    mdev->copyMono(data + (ty - y) * raster, sourcex + tx - x, raster, ps_no_bitmap_id, cx, 0, nx, ny, mcolor0, mcolor1);
	    code = target->copyMono(buffer.bytes, cx, tiles.raster, ps_no_bitmap_id, tx, ty, nx, ny, ps_no_color_index, color);
	    if (code < 0)
				return code;
		}
	}
	
	return 0;
}

int XWPSDeviceMaskClip::fillRectangleMaskClip(int x, 
	                          int y, 
	                          int w, 
	                          int h,
			                      ulong color)
{
	int mx0 = x + phase.x, my0 = y + phase.y;
  int mx1 = mx0 + w, my1 = my0 + h;

  if (mx0 < 0)
		mx0 = 0;
  if (my0 < 0)
		my0 = 0;
  if (mx1 > tiles.size.x)
		mx1 = tiles.size.x;
  if (my1 > tiles.size.y)
		my1 = tiles.size.y;
  return target->copyMono(tiles.data + my0 * tiles.raster, mx0,
	 												tiles.raster, tiles.id,
	 												mx0 - phase.x, my0 - phase.y,
	 											  mx1 - mx0, my1 - my0, ps_no_color_index, color);
}

int XWPSDeviceMaskClip::fillRectangleTileClip(int x, 
	                          int y, 
	                          int w, 
	                          int h,
			                      ulong color)
{
	return target->stripTileRectangle(&tiles, x, y, w, h, ps_no_color_index, color, phase.x, phase.y);
}

void XWPSDeviceMaskClip::getClipingBoxMaskClip(XWPSFixedRect * pbox)
{
   XWPSFixedRect tbox;

   target->getClipingBox(&tbox);
   pbox->p.x = tbox.p.x - phase.x;
   pbox->p.y = tbox.p.y - phase.y;
   pbox->q.x = tbox.q.x - phase.x;
   pbox->q.y = tbox.q.y - phase.y;
}

int  XWPSDeviceMaskClip::maskClipInitialize(int proto,
												 XWPSBitmap * bits, 
												 XWPSDevice * tdev,
			                   int tx, 
			                   int ty)
{
	switch (proto)
	{
		case 1:
			makeTileDevice();
			break;
			
		default:
			break;
	}
	
	int buffer_width = bits->size.x;
  int buffer_height =	tile_clip_buffer_size / (bits->raster + sizeof(uchar *));
  
  width = tdev->width;
  height = tdev->height;
  color_info = tdev->color_info;
  setTarget(tdev);
  phase.x = -tx;
  phase.y = -ty;
  if (buffer_height > bits->size.y)
		buffer_height = bits->size.y;
  mdev->makeMemMonoDevice(0);
  for (;;) 
  {
		if (buffer_height <= 0) 
		{
	    mdev->base = 0;
	    return 0;
		}
		mdev->width = buffer_width;
		mdev->height = buffer_height;
		if (mdev->bitmapSize() <= tile_clip_buffer_size)
	    break;
		buffer_height--;
  }
  mdev->base = buffer.bytes;
  return mdev->open();
}

void XWPSDeviceMaskClip::makeTileDevice()
{
	dname = "tile clipper";
	
	procs.fill_rectangle_.fill_rectanglemac = &XWPSDeviceMaskClip::fillRectangleTileClip;
	procs.copy_mono_.copy_monomac = &XWPSDeviceMaskClip::copyMonoTileClip;
	procs.copy_color_.copy_colormac = &XWPSDeviceMaskClip::copyColorTileClip;
	procs.copy_alpha_.copy_alphamac= &XWPSDeviceMaskClip::copyAlphaTileClip;
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
	procs.strip_tile_rectangle_.strip_tile_rectangle = &XWPSDevice::stripTileRectangleDefault;
	procs.strip_copy_rop_.strip_copy_ropmac = &XWPSDeviceMaskClip::stripCopyRopTileClip;
	procs.get_clipping_box_.get_clipping_boxfw = &XWPSDeviceForward::getClipingBoxForward;
	procs.begin_typed_image_.begin_typed_image = &XWPSDevice::beginTypedImageDefault;
	procs.text_begin_.text_begin = &XWPSDevice::textBeginDefault;
}

int XWPSDeviceMaskClip::stripCopyRopMaskClip(const uchar * data, 
	                         int sourcex, 
	                         uint raster, 
	                         ulong ,
			 										 const ulong * scolors,
	   											 XWPSStripBitmap * textures, 
	   											 const ulong * tcolors,
			                     int x, 
			                     int y, 
			                     int w, 
			                     int h,
		                       int phase_x, 
		                       int phase_y, 
		                       ulong lop)
{
	XWPSClipCallbackData ccdata;

  ccdata.tdev = target;
  ccdata.x = x, ccdata.y = y, ccdata.w = w, ccdata.h = h;
  ccdata.data = data, ccdata.sourcex = sourcex, ccdata.raster = raster;
  ccdata.scolors = scolors, ccdata.textures = textures,
	ccdata.tcolors = tcolors;
  ccdata.phase.x = phase_x, ccdata.phase.y = phase_y, ccdata.lop = lop;
  return clipRunsEnumerate(&XWPSClipCallbackData::stripCopyRop, &ccdata);
}

int XWPSDeviceMaskClip::stripCopyRopTileClip(const uchar * data, 
	                         int sourcex, 
	                         uint raster, 
	                         ulong ,
			 										 const ulong * scolors,
	   											 XWPSStripBitmap * textures, 
	   											 const ulong * tcolors,
			                     int x, 
			                     int y, 
			                     int w, 
			                     int h,
		                       int phase_x, 
		                       int phase_y, 
		                       ulong lop)
{
	FOR_RUNS(data_row, txrun, tx, ty) 
	{
		int code = target->stripCopyRop(data_row, sourcex + txrun - x, raster,
	 											ps_no_bitmap_id, scolors, textures, tcolors,
	 											txrun, ty, tx - txrun, 1, phase_x, phase_y, lop);

		if (code < 0)
	    return code;
  }
  END_FOR_RUNS();
  return 0;
}

int XWPSDeviceMaskClip::stripTileRectangleMaskClip(XWPSStripBitmap * tilesA,
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

  ccdata.tdev = target;
  ccdata.x = x, ccdata.y = y, ccdata.w = w, ccdata.h = h;
  ccdata.tiles = tilesA;
  ccdata.color[0] = color0, ccdata.color[1] = color1;
  ccdata.phase.x = phase_x, ccdata.phase.y = phase_y;
  return clipRunsEnumerate(&XWPSClipCallbackData::stripTileRectangle, &ccdata);
}

int XWPSDeviceMaskClip::tileClipInitialize(XWPSStripBitmap * tilesA,
		                     XWPSDevice * tdev, 
		                     int px, 
		                     int py)
{
	int code = maskClipInitialize(1, tilesA,  tdev, 0, 0);

  if (code >= 0) 
  {
		tiles = *tilesA;
		tileClipSetPhase(px, py);
  }
  return code;
}

void XWPSDeviceMaskClip::tileClipSetPhase(int px, int py)
{
	phase.x = px;
  phase.y = py;
}

#undef x_offset

int XWPSDeviceMaskClip::clipRunsEnumerate(int (XWPSClipCallbackData::*process)(int xc, int yc, int xec, int yec),
		    									XWPSClipCallbackData * pccd)
{
	DECLARE_MASK_COPY;
  int cy;
  uchar *tile_row;
  XWPSIntRect prev;
  int code;

  FIT_MASK_COPY(pccd->data, pccd->sourcex, pccd->raster,  pccd->x, pccd->y, pccd->w, pccd->h);
  tile_row = tiles.data + my0 * tiles.raster + (mx0 >> 3);
  prev.p.y = prev.q.y = -1;
  for (cy = my0; cy < my1; cy++)
  {
  	int cx = mx0;
		uchar *tp = tile_row;
		while (cx < mx1)
		{
			int len;
	    int tx1, tx, ty;

	    len = byte_bit_run_length[cx & 7][*tp ^ 0xff];
	    if (len < 8) 
	    {
				cx += len;
				if (cx >= mx1)
		    	break;
	    } 
	    else 
	    {
				cx += len - 8;
				tp++;
				while (cx < mx1 && *tp == 0)
		    	cx += 8, tp++;
				if (cx >= mx1)
		    	break;
				cx += byte_bit_run_length_0[*tp ^ 0xff];
				if (cx >= mx1)
		    	break;
	    }
	    tx1 = cx - phase.x;
	    len = byte_bit_run_length[cx & 7][*tp];
	    if (len < 8) 
	    {
				cx += len;
				if (cx > mx1)
		    	cx = mx1;
	    } 
	    else 
	    {
				cx += len - 8;
				tp++;
				while (cx < mx1 && *tp == 0xff)
		    	cx += 8, tp++;
				if (cx > mx1)
		    	cx = mx1;
				else 
				{
		    	cx += byte_bit_run_length_0[*tp];
		    	if (cx > mx1)
						cx = mx1;
				}
	    }
	    tx = cx - phase.x;
	    ty = cy - phase.y;
	    if (prev.p.x == tx1 && prev.q.x == tx && prev.q.y == ty)
				prev.q.y = ty + 1;
	    else 
	    {
				if (prev.q.y > prev.p.y) 
				{
		    	code = (pccd->*process)(tx1, ty, tx, ty + 1);
		    	if (code < 0)
						return code;
				}
				prev.p.x = tx1;
				prev.p.y = ty;
				prev.q.x = tx;
				prev.q.y = ty + 1;
			}
		}
		tile_row += tiles.raster;
  }
  
  if (prev.q.y > prev.p.y) 
  {
		code = (pccd->*process)(prev.p.x, prev.p.y, prev.q.x, prev.q.y);
		if (code < 0)
	    return code;
  }
  return 0;
}
