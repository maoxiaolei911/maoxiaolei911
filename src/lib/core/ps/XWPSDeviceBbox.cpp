/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWPSError.h"
#include "XWPSText.h"
#include "XWPSParam.h"
#include "XWPSColor.h"
#include "XWPSColorSpace.h"
#include "XWPSPath.h"
#include "XWPSContextState.h"
#include "XWPSDeviceBbox.h"

static const PSDeviceBboxProcs box_procs_default = {
    &XWPSDeviceBbox::initBoxDefault, 
    &XWPSDeviceBbox::getBoxDefault, 
    &XWPSDeviceBbox::addRectDefault,
    &XWPSDeviceBbox::inRectDefault
};
 
static const PSDeviceBboxProcs box_procs_forward = {
   &XWPSDeviceBbox::initBoxForward, 
    &XWPSDeviceBbox::getBoxForward, 
    &XWPSDeviceBbox::addRectForward,
    &XWPSDeviceBbox::inRectForward
};

#define MAX_COORD (max_int_in_fixed - 1000)
#define MAX_RESOLUTION 4000

XWPSDeviceBbox::XWPSDeviceBbox(QObject * parent)
	:XWPSDeviceForward("bbox", MAX_COORD, MAX_COORD, 
		                MAX_RESOLUTION, MAX_RESOLUTION, 
		                1, 8, 255, 0, 256, 1, parent)
{
	procs.open__.openbbox = &XWPSDeviceBbox::openBbox;
	procs.get_initial_matrix_.get_initial_matrix = &XWPSDevice::getInitialMatrixUpright;
	procs.close__.closebbox = &XWPSDeviceBbox::closeBbox;
	procs.map_rgb_color_.map_rgb_color = &XWPSDevice::mapRGBColorGrayDefault;
	procs.map_color_rgb_.map_color_rgb = &XWPSDevice::mapColorRGBGrayDefault;
	procs.fill_rectangle_.fill_rectanglebbox = &XWPSDeviceBbox::fillRectangleBbox;
	procs.copy_mono_.copy_monobbox = &XWPSDeviceBbox::copyMonoBbox;
	procs.copy_color_.copy_colorbbox = &XWPSDeviceBbox::copyColorBbox;
	procs.get_params_.get_paramsbbox = &XWPSDeviceBbox::getParamsBbox;
	procs.put_params_.put_paramsbbox = &XWPSDeviceBbox::putParamsBbox;
	procs.map_cmyk_color_.map_cmyk_color = &XWPSDevice::mapCMYKColorDefault;
	procs.map_rgb_alpha_color_.map_rgb_alpha_color = &XWPSDevice::mapRGBAlphaColorDefault;
	procs.get_page_device_.get_page_device = &XWPSDevice::getPageDevicePageDevice;
	procs.copy_alpha_.copy_alphabbox = &XWPSDeviceBbox::copyAlphaBbox;
	procs.fill_path_.fill_pathbbox = &XWPSDeviceBbox::fillPathBbox;
	procs.stroke_path_.stroke_pathbbox = &XWPSDeviceBbox::strokePathBbox;
	procs.fill_mask_.fill_maskbbox = &XWPSDeviceBbox::fillMaskBbox;
	procs.fill_trapezoid_.fill_trapezoidbbox = &XWPSDeviceBbox::fillTrapezoidBbox;
	procs.fill_parallelogram_.fill_parallelogrambbox = &XWPSDeviceBbox::fillParallelogramBbox;
	procs.fill_triangle_.fill_trianglebbox = &XWPSDeviceBbox::fillTriangleBbox;
	procs.draw_thin_line_.draw_thin_linebbox = &XWPSDeviceBbox::drawThinLineBbox;
	procs.begin_image_.begin_image = &XWPSDevice::beginImageDefault;
	procs.strip_tile_rectangle_.strip_tile_rectanglebbox = &XWPSDeviceBbox::stripTileRectangleBbox;
	procs.strip_copy_rop_.strip_copy_ropbbox = &XWPSDeviceBbox::stripCopyRopBbox;
	procs.begin_typed_image_.begin_typed_imagebbox = &XWPSDeviceBbox::beginTypedImageBbox;
	procs.map_color_rgb_alpha_.map_color_rgb_alpha = &XWPSDevice::mapColorRGBAlphaDefault;
	procs.create_compositor_.create_compositorbbox = &XWPSDeviceBbox::createCompositorBbox;
	procs.text_begin_.text_beginbbox = &XWPSDeviceBbox::textBeginBbox;
		
	box_procs = box_procs_default;
		
	free_standing = false;
	forward_open_close = false;
	box_proc_data = this;
	white_is_opaque = false;
	black = 0;
	white = 0;
	transparent = 0;
}

#undef MAX_COORD
#undef MAX_RESOLUTION

XWPSDeviceBbox::~XWPSDeviceBbox()
{
}

void XWPSDeviceBbox::addIntRect(int x0, int y0, int x1, int y1)
{
	addRect(int2fixed(x0), int2fixed(y0), int2fixed(x1), int2fixed(y1));
}

void XWPSDeviceBbox::addRect(long x0, long y0, long x1, long y1)
{
	XWPSDeviceBbox * bdev = (XWPSDeviceBbox*)box_proc_data;
	return (bdev->*box_procs.add_rect)(x0,y0,x1,y1);
}

void XWPSDeviceBbox::addRectDefault(long x0, long y0, long x1, long y1)
{
	if (x0 < bbox.p.x)
		bbox.p.x = x0;
  if (y0 < bbox.p.y)
		bbox.p.y = y0;
  if (x1 > bbox.q.x)
		bbox.q.x = x1;
  if (y1 > bbox.q.y)
		bbox.q.y = y1;
}

void XWPSDeviceBbox::addRectForward(long x0, long y0, long x1, long y1)
{
	addRect(x0,y0,x1,y1);
}

void XWPSDeviceBbox::bboxBbox(XWPSRect * pbbox)
{
	XWPSFixedRect bboxA;

  getBox(&bboxA);
  if (bboxA.p.x > bboxA.q.x || bboxA.p.y > bboxA.q.y) 
  {
		pbbox->p.x = pbbox->p.y = pbbox->q.x = pbbox->q.y = 0;
  } 
  else 
  {
		XWPSRect dbox;
		XWPSMatrix mat;
	
		dbox.p.x = fixed2float(bboxA.p.x);
		dbox.p.y = fixed2float(bboxA.p.y);
		dbox.q.x = fixed2float(bboxA.q.x);
		dbox.q.y = fixed2float(bboxA.q.y);
		getInitialMatrix(&mat);
		pbbox->bboxTransformInverse(&dbox, &mat);
  }
}

int  XWPSDeviceBbox::beginTypedImageBbox(XWPSImagerState * pis,
			                        XWPSMatrix *pmat, 
			                        XWPSImageCommon *pic,
			   											XWPSIntRect * prect,
			                        XWPSDeviceColor * pdcolor,
			                        XWPSClipPath * pcpath, 
			                        XWPSImageEnumCommon ** pinfo)
{
	XWPSBboxImageEnum *pbe = 0;
	uchar wanted[PS_IMAGE_MAX_PLANES];
	int code = XWPSBboxImageEnum::alloc(pis, pmat, pic, prect, pcpath, &pbe);
	if (code < 0)
		return code;
		
	if (!target)
		code = beginTypedImageDefault(pis, pmat, pic, prect, pdcolor, pcpath, &pbe->target_info);
	else
		code = target->beginTypedImage(pis, pmat, pic, prect, pdcolor, pcpath, &pbe->target_info);
		
	if (code) 
	{
	  pbe->bboxImageEndImage(false);
	  return code;
	}
	
	pbe->commonInit((XWPSDataImage*)pic, this, 0, ps_image_format_chunky);
	
	
	pbe->copyTargetInfo();
	pbe->params_are_const = pbe->target_info->planesWanted(wanted);
	
  *pinfo = pbe;
  return 0;
}

int XWPSDeviceBbox::closeBbox()
{
	if (box_procs.init_box != box_procs_default.init_box) 
	{
		int code = (forward_open_close ? target->close() : 0);
		return code;
  } 
  else 
		return (target && forward_open_close ? target->close() : 0);
}

int  XWPSDeviceBbox::copyAlphaBbox(const uchar *data, 
	                      int data_x,
	                      int raster, 
	                      ulong id, 
	                      int x, 
	                      int y, 
	                      int w, 
	                      int h,
	                      ulong color, 
	                      int depth)
{
	int code = (target == 0 ? 0 : target->copyAlpha(data, data_x, raster, id, x, y, w, h, color, depth));
  addIntRect(x, y, x + w, y + h);
  return code;
}

int XWPSDeviceBbox::copyColorBbox(const uchar *data, 
	                      int dx, 
	                      int raster, 
	                      ulong id,
	                      int x, 
	                      int y, 
	                      int w, 
	                      int h)
{
	int code = (target == 0 ? 0 : target->copyColor(data, dx, raster, id, x, y, w, h));
  addIntRect(x, y, x + w, y + h);
  return code;
}

int  XWPSDeviceBbox::copyDevice(XWPSDevice **pnew)
{
	XWPSDeviceBbox * ret = new XWPSDeviceBbox;
	ret->copyDeviceParamBbox(this);
	ret->copyDeviceProc(this);
	ret->box_procs = box_procs;
	*pnew = ret;
	return 0;
}

void XWPSDeviceBbox::copyDeviceParamBbox(XWPSDeviceBbox * proto)
{
	copyDeviceParam(proto);
	free_standing = proto->free_standing;
	forward_open_close = proto->forward_open_close;
	box_proc_data = proto->box_proc_data;
	white_is_opaque = proto->white_is_opaque;
	bbox = proto->bbox;
	black = proto->black;
	white = proto->white;
	transparent = proto->transparent;
}

int XWPSDeviceBbox::copyMonoBbox(const uchar *data, 
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
	int code = (target == 0 ? 0 : target->copyMono(data, dx, raster, id, x, y, w, h, zero, one));

  if ((one != ps_no_color_index && one != transparent) ||
			(zero != ps_no_color_index && zero != transparent))
		addIntRect(x, y, x + w, y + h);
  return code;
}

void XWPSDeviceBbox::copyParamsBbox(bool remap_colors)
{
	if (target != 0)
		copyParams(target);
  if (remap_colors) 
  {
		black = getBlack();
		white = getWhite();
		transparent = (white_is_opaque ? ps_no_color_index : white);
  }
}

int  XWPSDeviceBbox::createCompositorBbox(XWPSDevice **pcdev, 
		                   XWPSComposite *pcte,
		                   XWPSImagerState *pis)
{
	if (target == 0) 
	{
		*pcdev = this;
		return 0;
  }
  
  {
		XWPSDevice *cdev;
		XWPSDeviceBbox *bbcdev;
		int code = target->createCompositor(&cdev, pcte, pis);

		if (code < 0)
	   return code;
	
		bbcdev = new XWPSDeviceBbox;
		bbcdev->initBbox(target);
		bbcdev->setTarget(cdev);
		bbcdev->box_procs = box_procs_forward;
		bbcdev->box_proc_data = this;
		*pcdev = bbcdev;
		return 0;
  }
}

int XWPSDeviceBbox::drawThinLineBbox(long fx0, 
                           long fy0, 
                           long fx1, 
                           long fy1,
                           XWPSDeviceColor *pdevc, 
                           ulong lop)
{
	int code =(target == 0 ? 0 : target->drawThinLine(fx0, fy0, fx1, fy0, pdevc, lop));

  if (!(pdevc->pureColor() == transparent && pdevc->isPure())) 
  {
		long xmin, ymin, xmax, ymax;

#define SET_MIN_MAX(vmin, vmax, av, bv)\
    if (av < bv)\
	vmin = av, vmax = bv;\
    else\
	vmin = bv, vmax = av;
	
		SET_MIN_MAX(xmin, xmax, fx0, fx1);
		SET_MIN_MAX(ymin, ymax, fy0, fy1);
#undef SET_MIN_MAX
		addRect(xmin, ymin, xmax, ymax);
  }
  return code;
}

int  XWPSDeviceBbox::fillMaskBbox(const uchar * data, 
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
	int code =	(target == 0 ? 0 : target->fillMask(data, dx, raster, id, x, y, w, h, pdcolor, depth, lop, pcpath));

  if (pcpath != NULL &&	!pcpath->includesRectangle(int2fixed(x), int2fixed(y), int2fixed(x + w), int2fixed(y + h))) 
  {
  	XWPSDevice * tdev = target;
		target = NULL;
		fillMaskDefault(data, dx, raster, id, x, y, w, h, pdcolor, depth, lop, pcpath);
		target = tdev;
  } 
  else 
		addIntRect(x, y, x + w, y + h);
  return code;
}

int XWPSDeviceBbox::fillParallelogramBbox(long px, 
	                               long py, 
	                               long ax, 
	                               long ay, 
	                               long bx, 
	                               long by,
	                               XWPSDeviceColor *pdevc, 
	                               ulong lop)
{
	int code = (target == 0 ? 0 :	target->fillParallelogram(px, py, ax, ay, bx, by, pdevc, lop));

  if (!(pdevc->pureColor() == transparent && pdevc->isPure())) 
  {
		long xmin, ymin, xmax, ymax;

#define SET_MIN_MAX(vmin, vmax, av, bv)\
    if (av <= 0) {\
	if (bv <= 0)\
	    vmin = av + bv, vmax = 0;\
	else\
	    vmin = av, vmax = bv;\
    } else if (bv <= 0)\
	vmin = bv, vmax = av;\
    else\
	vmin = 0, vmax = av + bv;
	
		SET_MIN_MAX(xmin, xmax, ax, bx);
		SET_MIN_MAX(ymin, ymax, ay, by);
#undef SET_MIN_MAX
		addRect(px + xmin, py + ymin, px + xmax, py + ymax);
  }
  return code;
}

int  XWPSDeviceBbox::fillPathBbox(XWPSImagerState * pis, 
	                      XWPSPath * ppath,
		                    XWPSFillParams * params,
	                      XWPSDeviceColor * pdevc, 
	                      XWPSClipPath * pcpath)
{
	int code;
	if (!(pdevc->pureColor() == transparent && pdevc->isPure()) && 
		  !ppath->isVoid())
	{
		XWPSFixedRect ibox;
		XWPSFixedPoint adjust;

		if (ppath->getBbox(&ibox) < 0)
	    return 0;
	    
	  adjust = params->adjust;
		if (params->fill_zero_width)
	    adjust.adjustIfEmpty(&ibox);
	  ibox.p.x -= adjust.x;
	  ibox.p.y -= adjust.y;
	  ibox.q.x += adjust.x; 
	  ibox.q.y += adjust.y;
	  
	  if (inRect(&ibox))
	  {
	  	if (!target)
	  		return 0;
	  		
	  	return target->fillPath(pis, ppath, params, pdevc, pcpath);
	  }
	  
	  if (target != 0 && target->procs.fill_path_.fill_path == &XWPSDevice::fillPathDefault)
	  	return fillPath(pis, ppath, params, pdevc, pcpath);
	  	
	  if (!target)
	  	code = 0;
	  else	
	  	code = target->fillPath(pis, ppath, params, pdevc, pcpath);
	  	
	  if (code < 0)
	    return code;
		if (pcpath != NULL && !pcpath->includesRectangle(ibox.p.x, ibox.p.y, ibox.q.x, ibox.q.y))
		{
			XWPSDeviceColor devc;
			XWPSDevice * tdev = target;

	    devc.setPure(black);
	    target = 0;
	    code = fillPathDefault(pis, ppath, params, &devc, pcpath);
	    target = tdev;
		}
		else
			addRect(ibox.p.x, ibox.p.y, ibox.q.x, ibox.q.y);
		return code;
	}
	
	if (!target)
	  return 0;
	  		
	return target->fillPath(pis, ppath, params, pdevc, pcpath);
}

int XWPSDeviceBbox::fillRectangleBbox(int x, int y, int w, int h, ulong color)
{
	int code =(target == 0 ? 0 : target->fillRectangle(x, y, w, h, color));
		
  if (x <= 0 && y <= 0 && x + w >= width && y + h >= height) 
  {
		if (!initBox())
	    return code;
  }
  
  if (color != transparent)
		addIntRect(x, y, x + w, y + h);
  return code;
}

int  XWPSDeviceBbox::fillTriangleBbox(long px, 
	                         long py, 
	                         long ax, 
	                         long ay, 
	                         long bx, 
	                         long by,
	                         XWPSDeviceColor *pdevc, 
	                         ulong lop)
{
	int code = (target == 0 ? 0 : target->fillTriangle(px, py, ax, ay, bx, by, pdevc, lop));

  if (!(pdevc->pureColor() == transparent && pdevc->isPure())) 
  {
		long xmin, ymin, xmax, ymax;

#define SET_MIN_MAX(vmin, vmax, av, bv)\
    if (av <= 0) {\
	if (bv <= 0)\
	    vmin = qMin(av, bv), vmax = 0;\
	else\
	    vmin = av, vmax = bv;\
    } else if (bv <= 0)\
	vmin = bv, vmax = av;\
    else\
	vmin = 0, vmax = qMax(av, bv);
	
		SET_MIN_MAX(xmin, xmax, ax, bx);
		SET_MIN_MAX(ymin, ymax, ay, by);
#undef SET_MIN_MAX
		addRect(px + xmin, py + ymin, px + xmax, py + ymax);
  }
  return code;
}

int  XWPSDeviceBbox::fillTrapezoidBbox(XWPSFixedEdge *left, 
	                          XWPSFixedEdge *right,
	                          long ybot, 
	                          long ytop, 
	                          bool swap_axes,
	                          XWPSDeviceColor *pdevc, 
	                          ulong lop)
{
	int code =	(target == 0 ? 0 : target->fillTrapezoid(left, right, ybot, ytop, swap_axes, pdevc, lop));

  if (!(pdevc->pureColor() == transparent && pdevc->isPure())) 
  {
		long x0l = (left->start.y == ybot ? left->start.x :  left->x_at_y(ybot));
		long x1l = (left->end.y == ytop ? left->end.x : left->x_at_y(ytop));
		long x0r = (right->start.y == ybot ? right->start.x : right->x_at_y(ybot));
		long x1r = (right->end.y == ytop ? right->end.x : right->x_at_y(ytop));
		long xminl = qMin(x0l, x1l), xmaxl = qMax(x0l, x1l);
		long xminr = qMin(x0r, x1r), xmaxr = qMax(x0r, x1r);
		long x0 = qMin(xminl, xminr), x1 = qMax(xmaxl, xmaxr);

		if (swap_axes)
	    addRect(ybot, x0, ytop, x1);
		else
	    addRect(x0, ybot, x1, ytop);
  }
  return code;
}

void XWPSDeviceBbox::getBoxDefault(XWPSFixedRect * pbox)
{
	*pbox = bbox;
}

void XWPSDeviceBbox::getBoxForward(XWPSFixedRect * pbox)
{
	getBox(pbox);
}

int  XWPSDeviceBbox::getParamsBbox(XWPSContextState * ctx, XWPSParamList *plist)
{
	XWPSFixedRect fbox;
  int code = getParamsForward(ctx, plist);
  PSParamFloatArray bba;
  float bboxA[4];

  if (code < 0)
		return code;
		
  if (box_proc_data == 0)
		fbox = bbox;
  else
		getBox(&fbox);
  bboxA[0] = fixed2float(fbox.p.x);
  bboxA[1] = fixed2float(fbox.p.y);
  bboxA[2] = fixed2float(fbox.q.x);
  bboxA[3] = fixed2float(fbox.q.y);
  bba.data = bboxA, bba.size = 4;
  return plist->writeFloatArray(ctx, "PageBoundingBox", &bba);
}

void XWPSDeviceBbox::initBbox(XWPSDevice * targetA)
{
	if (targetA)
	{
		procs.get_initial_matrix_.get_initial_matrixfw = &XWPSDeviceForward::getInitialMatrixForward;
		procs.map_rgb_color_.map_rgb_colorfw = &XWPSDeviceForward::mapRGBColorForward;
		procs.map_color_rgb_.map_color_rgbfw = &XWPSDeviceForward::mapColorRGBForward;
		procs.map_cmyk_color_.map_cmyk_colorfw = &XWPSDeviceForward::mapCMYKColorForward;
		procs.map_rgb_alpha_color_.map_rgb_alpha_colorfw = &XWPSDeviceForward::mapRGBAlphaColorForward;
		procs.map_color_rgb_alpha_.map_color_rgb_alphafw = &XWPSDeviceForward::mapColorRGBAlphaForward;
		setTarget(targetA);
	}
}

bool XWPSDeviceBbox::initBoxDefault()
{
	bbox.p.x = bbox.p.y = max_fixed;
  bbox.q.x = bbox.q.y = min_fixed;
  return white != transparent;
}

bool XWPSDeviceBbox::initBoxForward()
{
	return initBox();
}

bool XWPSDeviceBbox::inRectDefault(XWPSFixedRect * pbox)
{
	return pbox->within(bbox);
}

bool XWPSDeviceBbox::inRectForward(XWPSFixedRect * pbox)
{
	return inRect(pbox);
}

int XWPSDeviceBbox::openBbox()
{
	if (box_procs.init_box == box_procs_default.init_box)
		initBox();
	
	int code = (target && forward_open_close ? target->open() : 0);

	copyParamsBbox(true);
	return code;
}

int  XWPSDeviceBbox::putParamsBbox(XWPSContextState * ctx, XWPSParamList *plist)
{
	int code;
  int ecode = 0;
  const char * param_name;
  PSParamFloatArray bba;

  code = plist->readFloatArray(ctx, (param_name = "PageBoundingBox"), &bba);
  switch (code) 
  {
		case 0:
	    if (bba.size != 4) 
	    {
				ecode = (int)(XWPSError::RangeCheck);
				goto e;
	    }
	    break;
	    
		default:
	    ecode = code;
e:
			plist->signalError(ctx, param_name, ecode);
		case 1:
			;
  }

  code = putParamsForward(ctx, plist);
  if (ecode < 0)
		code = ecode;
  if (code >= 0 && bba.data != 0) 
  {
		initBox();
		addRect(float2fixed(bba.data[0]), float2fixed(bba.data[1]), float2fixed(bba.data[2]), float2fixed(bba.data[3]));
  }
  copyParamsBbox(true);
  return code;
}

int  XWPSDeviceBbox::stripCopyRopBbox(const uchar *sdata, 
	                         int sourcex, 
	                         uint sraster, 
	                         ulong id,
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
	int code = (target == 0 ? 0 : target->stripCopyRop(sdata, sourcex, sraster, id, scolors,
	  					textures, tcolors, x, y, w, h, phase_x, phase_y, lop));

  addIntRect(x, y, x + w, y + h);
  return code;
}

int  XWPSDeviceBbox::stripTileRectangleBbox(XWPSStripBitmap * tiles,
			      										 int x, 
			      										 int y, 
			      										 int w, 
			      										 int h,
			      										 ulong color0, 
			      										 ulong color1,
			      										 int px, 
			      										 int py)
{
	int code = (target == 0 ? 0 : target->stripTileRectangle(tiles, x, y, w, h, color0, color1, px, py));

  if (x <= 0 && y <= 0 && x + w >= width && y + h >= height) 
  {
		if (!initBox())
	    return code;
  }
  addIntRect(x, y, x + w, y + h);
  return code;
}

void XWPSDeviceBbox::setWhiteOpaqueBbox(bool wio)
{
	white_is_opaque = wio;
  transparent = white_is_opaque ? ps_no_color_index : white;
}

int XWPSDeviceBbox::strokePathBbox(XWPSImagerState * pis, 
	                       XWPSPath * ppath,
		                     XWPSStrokeParams * params,
	                       XWPSDeviceColor * pdevc, 
	                       XWPSClipPath *pcpath)
{
	int code =(target == 0 ? 0 : target->strokePath(pis, ppath, params, pdevc, pcpath));
	if (!(pdevc->pureColor() == transparent && pdevc->isPure()))
	{
		XWPSFixedRect ibox;
		XWPSFixedPoint expand;
		if (pis->strokePathExpansion(ppath, &expand) == 0 &&  ppath->getBbox(&ibox) >= 0)
		{
			ibox.p.x -= expand.x;
	  	ibox.p.y -= expand.y;
	  	ibox.q.x += expand.x; 
	  	ibox.q.y += expand.y;
		}
		else
		{
			XWPSPath *spath = new XWPSPath(0);
			if (spath)
				code = pis->strokeAddImager(ppath, spath, this);
	    else
				code = -1;
	    if (code >= 0)
				code = spath->getBbox(&ibox);
	    if (code < 0) 
	    {
				ibox.p.x = ibox.p.y = min_fixed;
				ibox.q.x = ibox.q.y = max_fixed;
	    }
	    if (spath)
				delete spath;
		}
		if (pcpath != NULL && !pcpath->includesRectangle(ibox.p.x, ibox.p.y,ibox.q.x, ibox.q.y))
		{
			XWPSDevice * tdev = target;
			XWPSDeviceColor devc;

	    devc.setPure(black);
	    target = 0;
	    strokePathDefault(pis, ppath, params, &devc, pcpath);
	    target = tdev;
		}
		else
			addRect(ibox.p.x, ibox.p.y, ibox.q.x, ibox.q.y);
	}
	
	return code;
}

int XWPSDeviceBbox::textBeginBbox(XWPSImagerState * pis,
		    								XWPSTextParams *text, 
		    								XWPSFont * font,
		    								XWPSPath * path, 
		    								XWPSDeviceColor * pdcolor,
		    								XWPSClipPath * pcpath,
		    								XWPSTextEnum ** ppte)
{
	XWPSBboxTextEnum *pbte;
  int code;
  
  if (!target)
  	return textBeginDefault(pis, text, font, path, pdcolor, pcpath, ppte);
  	
  pbte = new XWPSBboxTextEnum;
  code = target->textBegin(pis, text, font, path, pdcolor, pcpath, &pbte->target_info);
  if (code < 0) 
  {
		delete pbte;
		return code;
  }
  pbte->copy(pbte->target_info);
  *ppte = pbte;
  return code;
}

void XWPSDeviceBbox::getBox(XWPSFixedRect * pbox)
{
	XWPSDeviceBbox * bdev = (XWPSDeviceBbox*)box_proc_data;
	return (bdev->*(box_procs.get_box))(pbox);
}

bool XWPSDeviceBbox::initBox()
{
	XWPSDeviceBbox * bdev = (XWPSDeviceBbox*)box_proc_data;
	return (bdev->*(box_procs.init_box))();
}

bool XWPSDeviceBbox::inRect(XWPSFixedRect * pbox)
{
	XWPSDeviceBbox * bdev = (XWPSDeviceBbox*)box_proc_data;
	return (bdev->*(box_procs.in_rect))(pbox);
}
