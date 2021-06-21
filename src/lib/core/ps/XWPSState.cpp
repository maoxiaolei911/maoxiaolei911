/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <string.h>
#include <math.h>
#include "XWPSError.h"
#include "XWPSDeviceMem.h"
#include "XWPSText.h"
#include "XWPSFont.h"
#include "XWPSHalfTone.h"
#include "XWPSRef.h"
#include "XWPSPath.h"
#include "XWPSColor.h"
#include "XWPSTransferMap.h"
#include "XWPSPattern.h"
#include "XWPSShading.h"
#include "XWPSFunction.h"
#include "XWPSContextState.h"
#include "XWPSState.h"

#define max_coord_fixed (max_fixed - int2fixed(1000))	/* arbitrary */
#define min_coord_fixed (-max_coord_fixed)

#define box_rounding_slop_fixed (fixed_epsilon * 3)

static void
clamp_point(XWPSFixedPoint * ppt, double x, double y)
{
#define clamp_coord(xy)\
  ppt->xy = (xy > fixed2float(max_coord_fixed) ? max_coord_fixed :\
	     xy < fixed2float(min_coord_fixed) ? min_coord_fixed :\
	     float2fixed(xy))
    clamp_coord(x);
    clamp_coord(y);
#undef clamp_coord
}

XWPSStateParts::XWPSStateParts()
{
	path = 0;
	clip_path = 0;
	effective_clip_path = 0;
	color_space = 0;
	ccolor = 0;
	dev_color = 0;
}

void XWPSStateParts::assignParts(const XWPSStateParts * pfrom)
{
	path = pfrom->path; 
	clip_path = pfrom->clip_path;
	effective_clip_path = pfrom->effective_clip_path;
	color_space = pfrom->color_space;
	ccolor = pfrom->ccolor; 
	dev_color = pfrom->dev_color;
}

void XWPSStateParts::assignParts(const XWPSState * pfrom)
{
	path = pfrom->path; 
	clip_path = pfrom->clip_path;
	effective_clip_path = pfrom->effective_clip_path;
	color_space = pfrom->color_space;
	ccolor = pfrom->ccolor; 
	dev_color = pfrom->dev_color;
}

#define force_unit(p) (p < 0.0 ? 0.0 : p > 1.0 ? 1.0 : p)
	
static void
color_rgb_to_hsb(float r, float g, float b, float hsb[3])
{
  ushort red = float2frac(r), green = float2frac(g), blue = float2frac(b);

#define rhue hsb[0]
#define rsat hsb[1]
#define rbri hsb[2]
  
  if (red == green && green == blue) 
  {
		rhue = 0;	
		rsat = 0;
		rbri = r;	
  } 
  else 
  {	
		ushort V, Temp, diff;
		long H;

		V = (red > green ? red : green);
		if (blue > V)
	    V = blue;
		Temp = (red > green ? green : red);
		if (blue < Temp)
	    Temp = blue;
		diff = V - Temp;
		if (V == red)
	    H = (green - blue) * frac_1_long / diff;
		else if (V == green)
	    H = (blue - red) * frac_1_long / diff + 2 * frac_1_long;
		else	
	    H = (red - green) * frac_1_long / diff + 4 * frac_1_long;
		if (H < 0)
	    H += 6 * frac_1_long;
		rhue = H / (frac_1 * 6.0);
		rsat = diff / (float)V;
		rbri = frac2float(V);
  }
#undef rhue
#undef rsat
#undef rbri
}

static void
color_hsb_to_rgb(float hue, float saturation, float brightness, float rgb[3])
{
  if (saturation == 0) 
  {
		rgb[0] = rgb[1] = rgb[2] = brightness;
  } 
  else 
  {	
		float h6 = hue * 6;
		ulong V = float2frac(brightness);
		ushort S = float2frac(saturation);
		int I = (int)h6;
		ulong F = float2frac(h6 - I);	
		ushort M = V * (frac_1_long - S) / frac_1_long;
		ushort N = V * (frac_1_long - S * F / frac_1_long) / frac_1_long;
		ushort K = M - N + V;
		ushort R, G, B;

		switch (I) 
		{
	    default:
				R = V;
				G = K;
				B = M;
				break;
				
	    case 1:
				R = N;
				G = V;
				B = M;
				break;
				
	    case 2:
				R = M;
				G = V;
				B = K;
				break;
				
	    case 3:
				R = M;
				G = N;
				B = V;
				break;
				
	    case 4:
				R = K;
				G = M;
				B = V;
				break;
				
	    case 5:
				R = V;
				G = M;
				B = N;
				break;
				
		}
		rgb[0] = frac2float(R);
		rgb[1] = frac2float(G);
		rgb[2] = frac2float(B);
  }
}

XWPSState::XWPSState(XWPSDevice * dev, XWPSState * pfrom)
	:XWPSImagerState()
{
	device = dev;
	saved = 0;	
	clip_stack = 0;
	path = 0;
	clip_path = 0;
	view_clip = 0;
	color_space = 0;
	ccolor = 0;
	dev_color = 0;
	effective_clip_path = 0;
	font = 0;
	root_font = 0;
	show_gstate = 0;
	device_color_spaces.indexed[0] = 0;
	device_color_spaces.indexed[1] = 0;
	device_color_spaces.indexed[2] = 0;
	if (!pfrom)
	{
		if (device)
			i_ctx_p = device->context_state;
			
		shared = new XWPSImagerStateShared;
		shared->device_color_spaces.named.Gray = new XWPSColorSpace(&ps_color_space_type_DeviceGray);
		shared->device_color_spaces.named.RGB = new XWPSColorSpace(&ps_color_space_type_DeviceRGB);
		shared->device_color_spaces.named.CMYK = new XWPSColorSpace(&ps_color_space_type_DeviceCMYK);
		shared->device_color_spaces.named.Gray->i_ctx_p = i_ctx_p;
		shared->device_color_spaces.named.RGB->i_ctx_p = i_ctx_p;
		shared->device_color_spaces.named.CMYK->i_ctx_p = i_ctx_p;
		
		set_transfer.u.colored.gray = new XWPSTransferMap;
		set_transfer.u.colored.gray->setIdentityTransfer();
		set_transfer.u.colored.red = set_transfer.u.colored.gray;
		set_transfer.u.colored.green = set_transfer.u.colored.gray;
		set_transfer.u.colored.blue = set_transfer.u.colored.gray;
		set_transfer.u.colored.gray->incRef();
		set_transfer.u.colored.gray->incRef();
		set_transfer.u.colored.gray->incRef();
		
		effective_transfer.copy(&set_transfer);
		
		ht_cache = new XWPSHTCache;
		
		setDefault();
		
		path = new XWPSPath(0);
		clip_path = new XWPSClipPath(0);
		view_clip = new XWPSClipPath(0);
		if (i_ctx_p)
		{
			clip_path->id = i_ctx_p->nextIDS(1);
			view_clip->id = i_ctx_p->nextIDS(1);
		}
		
		effective_clip_id = clip_path->id;
		effective_view_clip_id = ps_no_id;
		effective_clip_path = clip_path;
		effective_clip_shared = true;
		
		color_space = new XWPSColorSpace((PSColorSpaceType*)&ps_color_space_type_DeviceGray);
		color_space->i_ctx_p = i_ctx_p;
		if (i_ctx_p)
		{
			shared->device_color_spaces.named.Gray->id = i_ctx_p->nextIDS(1);
			shared->device_color_spaces.named.RGB->id = i_ctx_p->nextIDS(1);
			shared->device_color_spaces.named.CMYK->id = i_ctx_p->nextIDS(1);
			color_space->id = i_ctx_p->nextIDS(1);
			set_transfer.u.colored.gray->id = i_ctx_p->nextIDS(1);
		}
		
		ccolor = new XWPSClientColor;	
		dev_color = new XWPSDeviceColor(0);
		
		setDeviceColor1();
	
		setAlpha(1.0);
		setTransfer(ps_tm_identity);
		setFlat(1.0);
		setFillAdjust(0.25, 0.25);
		setLimitClamp(false);
		setStrokeAdjust(true);	
		initGraphics();
	}
	else
	{
		XWPSClipPath * oldpath = pfrom->view_clip;
		pfrom->view_clip = 0;
		clone(pfrom, copy_for_gstate);
		pfrom->view_clip = oldpath;
		show_gstate =	(pfrom->show_gstate == pfrom ? this : 0);
	}
}

XWPSState::XWPSState(XWPSState * pfrom, PSStateCopyReason reason)
	:XWPSImagerState()
{
	saved = 0;
	path = 0;
	clip_path = 0;
	clip_stack = 0;
	view_clip = 0;
	color_space = 0;
	ccolor = 0;
	dev_color = 0;
	effective_clip_path = 0;
	font = 0;
	root_font = 0;
	show_gstate = 0;
	device = 0;
	saved = 0;
	if (reason == copy_for_gsave || reason == copy_for_gstate)
	{
		XWPSClipPath * oldpath = pfrom->view_clip;
		if (reason == copy_for_gstate)
			pfrom->view_clip = 0;
		
		clone(pfrom, reason);
		pfrom->view_clip = oldpath;
		if (reason == copy_for_gstate)
			if (pfrom->show_gstate == pfrom)
				show_gstate = this;
	}
	else
		copy(pfrom, reason);
}

XWPSState::~XWPSState()
{
	freeContents();
}

int  XWPSState::arc(float xc, float yc, float r, float ang1, float ang2)
{
	return path->imagerArcAdd(this, false, xc, yc, r, ang1, ang2, true);
}

int  XWPSState::arcAdd(bool clockwise, 
	            float axc, 
	            float ayc, 
	            float arad, 
	            float aang1, 
	            float aang2, 
	            bool add_line)
{
	return path->imagerArcAdd(this, clockwise, axc, ayc, arad, aang1, aang2, add_line);
}

int  XWPSState::arcn(float xc, float yc, float r, float ang1, float ang2)
{
	return path->imagerArcAdd(this, true, xc, yc, r, ang1, ang2, true);
}

int  XWPSState::arcTo(float ax1, float ay1, float ax2, float ay2, float arad, float * retxy)
{
	double xt0, yt0, xt2, yt2;
  XWPSPoint up0;
  
  int code = currentPoint(&up0);

  if (code < 0)
		return code;
		
	{
		double dx0 = up0.x - ax1, dy0 = up0.y - ay1;
		double dx2 = ax2 - ax1, dy2 = ay2 - ay1;
		double sql0 = dx0 * dx0 + dy0 * dy0;
		double sql2 = dx2 * dx2 + dy2 * dy2;
		double num = dy0 * dx2 - dy2 * dx0;
		double denom = sqrt(sql0 * sql2) - (dx0 * dx2 + dy0 * dy2);
		if (denom == 0) 
		{
	    code = lineTo(ax1, ay1);
	    xt0 = xt2 = ax1;
	    yt0 = yt2 = ay1;
		}
		else
		{
			double dist = fabs(arad * num / denom);
	    double l0 = dist / sqrt(sql0), l2 = dist / sqrt(sql2);
	    XWPSArcCurveParams arc;

	    arc.ppath = path;
	    arc.pis = this;
	    arc.radius = arad;
	    arc.action = arc_lineto;
	    arc.notes = sn_none;
	    if (arad < 0)
				l0 = -l0, l2 = -l2;
	    arc.p0.x = xt0 = ax1 + dx0 * l0;
	    arc.p0.y = yt0 = ay1 + dy0 * l0;
	    arc.p3.x = xt2 = ax1 + dx2 * l2;
	    arc.p3.y = yt2 = ay1 + dy2 * l2;
	    arc.pt.x = ax1;
	    arc.pt.y = ay1;
	    code = arc.arcAdd(false);
		}
	}
	
	if (retxy != 0) 
	{
		retxy[0] = xt0;
		retxy[1] = yt0;
		retxy[2] = xt2;
		retxy[3] = yt2;
  }
  return code;
}

int XWPSState::ashowBegin(float ax, 
	                float ay, 
	                const uchar * str, 
	                uint size,
	                XWPSTextEnum ** ppte)
{
	XWPSTextParams text;

  text.operation = TEXT_FROM_STRING | TEXT_ADD_TO_ALL_WIDTHS | TEXT_DO_DRAW | TEXT_RETURN_WIDTH;
  text.data.bytes = (uchar*)str, text.size = size;
  text.delta_all.x = ax;
  text.delta_all.y = ay;
  return textBegin(&text, ppte);
}

int  XWPSState::awidthShowBegin(float cx, 
	                     float cy, 
	                     ulong chr,
		                   float ax, 
		                   float ay, 
		                   const uchar * str, uint size,
		                   XWPSTextEnum ** ppte)
{
	XWPSTextParams text;

  text.operation = TEXT_FROM_STRING |	TEXT_ADD_TO_ALL_WIDTHS | TEXT_ADD_TO_SPACE_WIDTH |	TEXT_DO_DRAW | TEXT_RETURN_WIDTH;
  text.data.bytes = (uchar*)str, text.size = size;
  text.delta_space.x = cx;
  text.delta_space.y = cy;
  text.space.s_char = chr;
  text.delta_all.x = ax;
  text.delta_all.y = ay;
  return textBegin(&text, ppte);
}

int XWPSState::beginTransparencyGroup(const XWPSTransparencyGroupParams *, const XWPSRect *)
{
	return pushTransparencyStack(TRANSPARENCY_STATE_Group);
}

int XWPSState::beginTransparencyMask(const XWPSTransparencyMaskParams *,const XWPSRect *)
{
	return pushTransparencyStack(TRANSPARENCY_STATE_Mask);
}

int  XWPSState::charBoxPathBegin(const uchar * str, 
	                      uint size,
		                    bool stroke_path, 
		                    XWPSTextEnum ** ppte)
{
	XWPSTextParams text;

  text.operation = TEXT_FROM_STRING | TEXT_RETURN_WIDTH |	(stroke_path ? TEXT_DO_TRUE_CHARBOXPATH : TEXT_DO_FALSE_CHARBOXPATH);
  text.data.bytes = (uchar*)str, text.size = size;
  return textBegin(&text, ppte);
}

int XWPSState::charPathBegin(const uchar * str, 
	                   uint size, 
	                   bool stroke_path,
		                 XWPSTextEnum ** ppte)
{
	XWPSTextParams text;

  text.operation = TEXT_FROM_STRING | TEXT_RETURN_WIDTH |	(stroke_path ? TEXT_DO_TRUE_CHARPATH : TEXT_DO_FALSE_CHARPATH);
  text.data.bytes = (uchar*)str, text.size = size;
  return textBegin(&text, ppte);
}

XWPSCie * XWPSState::cieCSCommon()
{
	XWPSCieABC *ignore_pabc;

  return color_space->cieCSCommonABC(&ignore_pabc);
}

int  XWPSState::cieCSComplete(bool init)
{
	XWPSCieJointCaches *pjc = currentCieCaches();
	
  pjc->status = (init ? CIE_JC_STATUS_BUILT : CIE_JC_STATUS_INITED);
  return 0;
}

void XWPSState::cieLoadCommonCache(XWPSCie * pcie)
{
	cie_matrix_init(&pcie->common.MatrixLMN);
	
	int j;
	for (j = 0; j < (sizeof(pcie->common.caches.DecodeLMN) / sizeof(pcie->common.caches.DecodeLMN[0])); j++) 
	{
	    PSCieCacheFloats *pcf = &pcie->common.caches.DecodeLMN[j].floats;
	    int i;
	    PSForLoopParams lp;

	    ps_cie_cache_init(&pcf->params, &lp, &pcie->common.RangeLMN.ranges[j]);
	    for (i = 0; i < ps_cie_cache_size; lp.init += lp.step, i++) 
	    {
				pcf->values[i] = (pcie->*(pcie->common.DecodeLMN.procs[j]))(lp.init);
	    }
	    pcf->params.is_identity =	(pcie->common.DecodeLMN.procs[j] == DecodeLMN_default.procs[j]);
	}
}

int  XWPSState::clip()
{
	return commonClip(ps_rule_winding_number);
}

int  XWPSState::clipPath()
{
	XWPSPath cpath;
  int code;
  
  code = clip_path->toPath(&cpath);
  if (code >= 0)
		code = path->assign(&cpath, false);
  
  return code;
}

int  XWPSState::clipRestore()
{
	XWPSClipStack *stack = clip_stack;

  if (stack) 
  {
		XWPSClipStack *next = stack->next;
		XWPSClipPath *pcpath = stack->clip_path;
		int code = 0;

		if (!stack->decRef()) 
		{
			clip_path->assign(pcpath, false);
			delete stack;
		} 
		else 
		{
	    code = clip_path->assign(pcpath);
	    if (code < 0)
				return code;
		}
		clip_stack = next;
		return code;
  } 
  else 
		return clip_path->assign(saved->clip_path);
}

int  XWPSState::clipSave()
{
	XWPSClipPath *copy = new XWPSClipPath(clip_path);
  XWPSClipStack *stack = new XWPSClipStack;
  stack->clip_path = copy;
  stack->next = clip_stack;
  clip_stack = stack;
  return 0;
}

int  XWPSState::clipToPath()
{
	XWPSFixedRect bbox;
  int code;

  if ((code = path->getBbox(&bbox)) < 0 ||
			(code = clipToRectangle(&bbox)) < 0 ||
			(code = clip()) < 0)
		return code;
  
  return 0;
}

int  XWPSState::clipToRectangle(XWPSFixedRect * pbox)
{
	int code = clip_path->fromRectangle(pbox);
  if (code < 0)
		return code;
		
  clip_path->rule = ps_rule_winding_number;
  return 0;
}

int  XWPSState::closePath()
{
  int code = path->closeSubpathNotes(sn_none);

  if (code < 0)
		return code;
    
  if (path->startOutsideRange())
		path->setOutsidePosition();
  return code;
}

int XWPSState::colorLoad()
{
	return dev_color->load(this, device);
}

int XWPSState::colorWritesPure()
{
	return dev_color->writePure(log_op);
}

int XWPSState::concat(const XWPSMatrix * pmat)
{
	XWPSMatrix cmat, c;
	ctmOnly(&c);
  int code = cmat.matrixMultiply((XWPSMatrix*)pmat, &c);

  if (code < 0)
		return code;
    
  updateCTM(cmat.tx, cmat.ty);
  ctm = cmat;
  return code;
}

int XWPSState::copyGState(XWPSState * pfrom)
{
	return copy(pfrom, copy_for_copygstate);
}

int XWPSState::copyPage()
{
	return outputPage(1, 0);
}

int XWPSState::cshowBegin(const uchar * str, uint size, XWPSTextEnum ** ppte)
{
	XWPSTextParams text;

  text.operation = TEXT_FROM_STRING | TEXT_DO_NONE | TEXT_INTERVENE;
  text.data.bytes = (uchar*)str, text.size = size;
  return textBegin(&text, ppte);
}

float XWPSState::currentAlpha()
{
	return (float)alpha / ps_max_color_value;
}

int XWPSState::currentCharMatrix(XWPSMatrix * ptm, bool force)
{
	if (!char_tm_valid) 
	{
		int code;
		if (!force)
	    return (int)(XWPSError::UndefinedResult);
		code = setCharMatrix(&font->FontMatrix);
		if (code < 0)
	    return code;
  }
  if (ptm != NULL)
		*ptm = char_tm;
  return 0;
}

XWPSCieJointCaches * XWPSState::currentCieCaches()
{
	if (!cie_joint_caches)
		cie_joint_caches = new XWPSCieJointCaches;
	return cie_joint_caches;
}

int   XWPSState::currentCMYKColor(float pr4[4])
{
	XWPSClientColor *pcc = ccolor;
  XWPSColorSpace *pcs = color_space;
  XWPSColorSpace *pbcs = pcs;
  PSColorSpaceIndex csi = orig_cspace_index;
  ushort fcc[4];
  XWPSClientColor cc;
  int code;

sw:
	switch (csi) 
	{
		case ps_color_space_index_DeviceGray:
	    pr4[0] = pr4[1] = pr4[2] = 0.0;
	    pr4[3] = 1.0 - pcc->paint.values[0];
	    return 0;
	    
		case ps_color_space_index_DeviceRGB:
	    colorRGBToCMYK(float2frac(pcc->paint.values[0]),
			      float2frac(pcc->paint.values[1]),
			      float2frac(pcc->paint.values[2]),
			      (short*)&fcc[0]);
	    pr4[0] = frac2float(fcc[0]);
	    pr4[1] = frac2float(fcc[1]);
	    pr4[2] = frac2float(fcc[2]);
	    pr4[3] = frac2float(fcc[3]);
	    return 0;
	    
		case ps_color_space_index_DeviceCMYK:
	    pr4[0] = pcc->paint.values[0];
	    pr4[1] = pcc->paint.values[1];
	    pr4[2] = pcc->paint.values[2];
	    pr4[3] = pcc->paint.values[3];
	    return 0;
	    
		case ps_color_space_index_DeviceN:
		case ps_color_space_index_Separation:
ds:
			if (pbcs->concreteSpace(this) == pbcs)
				break;
	    
		case ps_color_space_index_Indexed:
	    pbcs = pbcs->baseSpace();
	    switch (pbcs->type->index) 
	    {
				case ps_color_space_index_DeviceN:
				case ps_color_space_index_Separation:
		    	goto ds;
		    	
				default:
		    	break;
	    }
	    code = pcs->concretizeColor(pcc, fcc, this);
	    if (code < 0)
				return code;
	    cc.paint.values[0] = frac2float(fcc[0]);
	    cc.paint.values[1] = frac2float(fcc[1]);
	    cc.paint.values[2] = frac2float(fcc[2]);
	    cc.paint.values[3] = frac2float(fcc[3]);
	    pcc = &cc;
	    pcs = pbcs;
	    csi = orig_base_cspace_index;
	    goto sw;
	    
		default:
	    break;
  }
  pr4[0] = pr4[1] = pr4[2] = 0.0;
  pr4[3] = 1.0;
  return 0;
}

XWPSColorSpace * XWPSState::currentDeviceCMYKSpace()
{
	XWPSColorSpace *pcs;
  return (!device->UseCIEColor || (pcs = device_color_spaces.deviceCMYK()) == 0 ? shared->deviceCMYK() : pcs);
}

XWPSColorSpace * XWPSState::currentDeviceGraySpace()
{
	XWPSColorSpace *pcs;
  return (!device->UseCIEColor ||  (pcs = device_color_spaces.deviceGray()) == 0 ?  shared->deviceGray() : pcs);
}

XWPSColorSpace * XWPSState::currentDeviceRGBSpace()
{
	XWPSColorSpace *pcs;
  return (!device->UseCIEColor || (pcs = device_color_spaces.deviceRGB()) == 0 ? shared->deviceRGB() : pcs);
}

int XWPSState::currentFillAdjust(XWPSPoint * adjust)
{
	adjust->x = fixed2float(fill_adjust.x);
  adjust->y = fixed2float(fill_adjust.y);
  return 0;
}

int XWPSState::currentGray(float *pg)
{
  switch (orig_cspace_index) 
  {
		case ps_color_space_index_DeviceGray:
	    *pg = ccolor->paint.values[0];
	    break;
	    
		case ps_color_space_index_DeviceRGB:
	    	*pg = frac2float(colorRGBToGray(float2frac(ccolor->paint.values[0]),
												float2frac(ccolor->paint.values[1]),
												float2frac(ccolor->paint.values[2])));
	    break;
	    
		case ps_color_space_index_DeviceCMYK:
	    	*pg = frac2float(colorCMYKToGray(float2frac(ccolor->paint.values[0]),
									float2frac(ccolor->paint.values[1]),
									float2frac(ccolor->paint.values[2]),
									float2frac(ccolor->paint.values[3])));
	    break;
	    
		default:
	    {
				float rgb[3];
				int code = currentRGBColor(rgb);

				if (code < 0)
		    	return code;
				*pg = frac2float(colorRGBToGray( float2frac(rgb[0]), float2frac(rgb[1]),  float2frac(rgb[2])));
	    }
  }
  return 0;
}

int XWPSState::currentGState(XWPSState * pgs)
{
	int code = copy(pgs, copy_for_currentgstate);
	if (code >= 0)
	{
		if ((view_clip == effective_clip_path) || (view_clip == clip_path))
			view_clip = 0;
			
		if (view_clip)
		{
		  delete view_clip;
			view_clip = 0;
		}
	}
		
  return code;
}

int  XWPSState::currentHalftone(XWPSHalftone ** pht)
{
	*pht = halftone;
   return 0;
}

int  XWPSState::currentHSBColor(float *pr3)
{
	float rgb[3];

  currentRGBColor(rgb);
  color_rgb_to_hsb(rgb[0], rgb[1], rgb[2], pr3);
  return 0;
}

int XWPSState::currentPoint(XWPSPoint * ppt)
{
  int code;
  XWPSFixedPoint pt;

  if (path->outsideRange())
		return itransform(path->outside_position.x, path->outside_position.y, ppt);
  code = path->currentPoint(&pt);
  if (code < 0)
		return code;
  return itransform(fixed2float(pt.x), fixed2float(pt.y), ppt);
}

PSRop3 XWPSState::currentRasterOp()
{
	return lop_rop(log_op);
}

int XWPSState::currentRGBColor(float pr3[3])
{
	XWPSClientColor *pcc = ccolor;
  XWPSColorSpace *pcs = color_space;
  XWPSColorSpace *pbcs = pcs;
	PSColorSpaceIndex csi = orig_cspace_index;
  short fcc[4];
  XWPSClientColor cc;
  int code;
  
sw:
	switch (csi) 
	{
		case ps_color_space_index_DeviceGray:
	    pr3[0] = pr3[1] = pr3[2] = pcc->paint.values[0];
	    return 0;
	    
		case ps_color_space_index_DeviceRGB:
	    pr3[0] = pcc->paint.values[0];
	    pr3[1] = pcc->paint.values[1];
	    pr3[2] = pcc->paint.values[2];
	    return 0;
	    
		case ps_color_space_index_DeviceCMYK:
	    colorCMYKToRgb( float2frac(pcc->paint.values[0]),
				 							float2frac(pcc->paint.values[1]),
				 							float2frac(pcc->paint.values[2]),
				 							float2frac(pcc->paint.values[3]), (short*)&fcc[0]);
	    pr3[0] = frac2float(fcc[0]);
	    pr3[1] = frac2float(fcc[1]);
	    pr3[2] = frac2float(fcc[2]);
	    return 0;
	    
		case ps_color_space_index_DeviceN:
		case ps_color_space_index_Separation:
ds:
			if (pbcs->concreteSpace(this) == pbcs)
				break;
				
		case ps_color_space_index_Indexed:
	    pbcs = pbcs->baseSpace();
	    switch (pbcs->type->index) 
	    {
				case ps_color_space_index_DeviceN:
				case ps_color_space_index_Separation:
		    goto ds;
		    
				default:
		    	break;
	    }
	    code = pcs->concretizeColor(pcc, (ushort*)&fcc[0], this);
	    if (code < 0)
				return code;
	    cc.paint.values[0] = frac2float(fcc[0]);
	    cc.paint.values[1] = frac2float(fcc[1]);
	    cc.paint.values[2] = frac2float(fcc[2]);
	    cc.paint.values[3] = frac2float(fcc[3]);
	    pcc = &cc;
	    pcs = pbcs;
	    csi = orig_base_cspace_index;
	    goto sw;
	    
		default:
	    break;
  }
  pr3[0] = pr3[1] = pr3[2] = 0.0;
  return 0;
}

int XWPSState::currentScreen(XWPSScreenHalftone * phsp)
{
	switch (halftone->type) 
	{
		case ht_type_screen:
	    *phsp = *(halftone->params.screen);
	    return 0;
	    
		case ht_type_colorscreen:
	    *phsp = *(halftone->params.colorscreen->screens.colored.gray);
	    return 0;
	    
		default:
	    return (int)(XWPSError::Undefined);
  }
}

int XWPSState::currentScreenLevels()
{
	return dev_ht->order.num_levels;
}

int XWPSState::currentScreenPhase(XWPSIntPoint * pphase, PSColorSelect select)
{
	if (select < 0 || select >= ps_color_select_count)
		return (int)(XWPSError::RangeCheck);
  *pphase = screen_phase[select];
  return 0;
}

bool  XWPSState::currentSourceTransparent()
{
	return (log_op & lop_S_transparent) != 0;
}

bool  XWPSState::currentTextureTransparent()
{
	return (log_op & lop_T_transparent) != 0;
}

int XWPSState::curveTo(float x1, float y1, 
	              float x2, float y2, 
	              float x3, float y3)
{
	XWPSFixedPoint p1, p2, p3;
  int code1 = p1.transform2fixed(&ctm, x1, y1);
  int code2 = p2.transform2fixed(&ctm, x2, y2);
  int code3 = p3.transform2fixed(&ctm, x3, y3);

  if ((code1 | code2 | code3) < 0) 
  {
		if (clamp_coordinates) 
		{
	    XWPSPoint opt1, opt2, opt3;
	    int code;

	    if ((code1 < 0 && code1 != XWPSError::LimitCheck) ||
					(code1 = transform(x1, y1, &opt1)) < 0)
				return code1;
	    if ((code2 < 0 && code2 != XWPSError::LimitCheck) ||
					(code2 = transform(x2, y2, &opt2)) < 0)
				return code2;
	    if ((code3 < 0 && code3 != XWPSError::LimitCheck) ||
					(code3 = transform(x3, y3, &opt3)) < 0)
				return code3;
	    clamp_point(&p1, opt1.x, opt1.y);
	    clamp_point(&p2, opt2.x, opt2.y);
	    clamp_point(&p3, opt3.x, opt3.y);
	    code = path->addCurveNotes(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, sn_none);
	    if (code < 0)
				return code;
	    path->setOutsidePosition(opt3.x, opt3.y);
	    return code;
		} 
		else
	    return (code1 < 0 ? code1 : code2 < 0 ? code2 : code3);
  }
  return path->addCurveNotes(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, sn_none);
}

PSTransparencyStateType XWPSState::currentTransparencyType()
{
	return (PSTransparencyStateType)(transparency_stack == 0 ? 0 : transparency_stack->type);
}

int XWPSState::dashPath()
{
  XWPSPath fpath;
  int code;

  if (currentDashLength() == 0)
		return 0;
  code = flattenPath();
  if (code < 0)
		return code;

  code = fpath.addDashExpansion(path, this);
  if (code < 0) 
		return code;
	
  path->assign(&fpath, false);
  return 0;
}

int XWPSState::defaultClipBox(XWPSFixedRect * pbox)
{
	XWPSRect bbox;
  XWPSMatrix imat;
  if (device->ImagingBBox_set) 
  {  	
		defaultMatrix(&imat);
		bbox.p.x = device->ImagingBBox[0];
		bbox.p.y = device->ImagingBBox[1];
		bbox.q.x = device->ImagingBBox[2];
		bbox.q.y = device->ImagingBBox[3];
  } 
  else 
  {
  	device->getInitialMatrix(&imat);
  	imat.tx += device->Margins[0] * device->HWResolution[0] / device->MarginsHWResolution[0];
		imat.ty += device->Margins[1] * device->HWResolution[1] / device->MarginsHWResolution[1];
		bbox.p.x = device->HWMargins[0];
		bbox.p.y = device->HWMargins[1];
		bbox.q.x = device->MediaSize[0] - device->HWMargins[2];
		bbox.q.y = device->MediaSize[1] - device->HWMargins[3];
  }
  
  int code = bbox.bboxTransform(&imat, &bbox);
  if (code < 0)
		return code;
		
	pbox->p.x = fixed_rounded(float2fixed(bbox.p.x));
  pbox->p.y = fixed_rounded(float2fixed(bbox.p.y));
  pbox->q.x = fixed_rounded(float2fixed(bbox.q.x));
  pbox->q.y = fixed_rounded(float2fixed(bbox.q.y));
  return 0;
}

int XWPSState::defaultMatrix(XWPSMatrix * pmat)
{
	if (ctm_default_set) 
	{
		*pmat = ctm_default;
		return 1;
  }
  
	device->getInitialMatrix(pmat);
	pmat->tx += device->Margins[0] *	device->HWResolution[0] / device->MarginsHWResolution[0];
  pmat->ty += device->Margins[1] *	device->HWResolution[1] / device->MarginsHWResolution[1];
  return 0;
}

int XWPSState::discardTransparencyLayer()
{
	XWPSTransparencyState *pts = transparency_stack;

  if (!pts)
		return (int)(XWPSError::RangeCheck);
  popTransparencyStack();
  return 0;
}

int XWPSState::dtransform(float dx, float dy, XWPSPoint * pt)
{
	XWPSMatrix c;
	ctmOnly(&c);
	return pt->distanceTransform(dx, dy, &c);
}

int XWPSState::effectiveClipPath(XWPSClipPath ** ppcpath)
{
	ulong view_clip_id = (view_clip == 0 || view_clip->rule == 0 ? ps_no_id :	view_clip->id);
	if (device->isMemory()) 
	{
		*ppcpath = clip_path;
		return 0;
  }
  
  if (effective_clip_id == clip_path->id &&	effective_view_clip_id == view_clip_id) 
  {
		*ppcpath = effective_clip_path;
		return 0;
  }
  
  if (view_clip_id == ps_no_id)
  {
  	if (!effective_clip_shared)
  	{
  		if (effective_clip_path)
  			delete effective_clip_path;
	  }
		effective_clip_path = clip_path;
		effective_clip_shared = true;
  }
  else
  {
  	XWPSFixedRect cbox, vcbox;

		clip_path->innerBox(&cbox);
		view_clip->outerBox(&vcbox);
		if (vcbox.within(cbox))
		{
			if (!effective_clip_shared)
			{
				if (effective_clip_path)
  				delete effective_clip_path;
			}
	    effective_clip_path = view_clip;
	    effective_clip_shared = true;
		}
		else
		{
			int code;	    
	    XWPSClipPath *npath = effective_clip_path;
	    if (effective_clip_shared)
	    {
	    	npath = new XWPSClipPath(0);
	    	if (i_ctx_p)
					npath->id = i_ctx_p->nextIDS(1);
	    }
	    	
	    XWPSClipPath ipath;
	    if (i_ctx_p)
				ipath.id = i_ctx_p->nextIDS(1);
			ipath.assign(clip_path);
	    XWPSPath vpath;
	    code = view_clip->toPath(&vpath);
	    if (code < 0)
	    {
	    	if (effective_clip_shared)
	    		delete npath;
				return code;
			}
			code = ipath.intersect(&vpath, ps_rule_winding_number, this);
			if (code < 0)
			{
				if (effective_clip_shared)
	    		delete npath;
				return code;
			}
			npath->assign(&ipath, false);
			effective_clip_path = npath;	    
	    effective_clip_shared = false;
		}
  }
  
  effective_clip_id = clip_path->id;
  effective_view_clip_id = view_clip_id;
  *ppcpath = effective_clip_path;
  return 0;
}

int XWPSState::endTransparencyGroup()
{
	XWPSTransparencyState *pts = transparency_stack;
  if (!pts || pts->type != TRANSPARENCY_STATE_Group)
		return (int)(XWPSError::RangeCheck);
  popTransparencyStack();
  return 0;
}

int XWPSState::endTransparencyMask(PSTransparencyChannelSelector )
{
	XWPSTransparencyState *pts = transparency_stack;

  if (!pts || pts->type != TRANSPARENCY_STATE_Group)
		return (int)(XWPSError::RangeCheck);
  popTransparencyStack();
  return 0;
}

int XWPSState::eoclip()
{
	return commonClip(ps_rule_even_odd);
}

int XWPSState::eofill()
{
	return fillWithRule(ps_rule_even_odd);
}

int XWPSState::eoviewClip()
{
	return commonViewClip(ps_rule_even_odd);
}

int XWPSState::erasePage()
{
	int code;

  if ((code = save()) < 0)
		return code;
  
  if ((code = setGray(1.0)) >= 0) 
		code = fillPage();
  restore();
  return code;
}

int XWPSState::fill()
{
	return fillWithRule(ps_rule_winding_number);
}

int XWPSState::fillPage()
{
	int code;
  uint save_lop;

  setDevColor();
  save_lop = log_op;
  initRop();
  code = dev_color->fillRectangle(0, 0, device->width, device->height, device, log_op, NULL);
  log_op = save_lop;
  if (code < 0)
		return code;
  return device->syncOutput();
}

int XWPSState::fillPath(XWPSPath * ppath, XWPSDeviceColor * pdevc, int rule, long adjust_x, long adjust_y)
{
	XWPSClipPath *pcpath;
  int code = effectiveClipPath(&pcpath);
  XWPSFillParams params;

  if (code < 0)
		return code;
  params.rule = rule;
  params.adjust.x = adjust_x;
  params.adjust.y = adjust_y;
  params.flatness = (in_cachedevice > 1 ? 0.0 : flatness);
  params.fill_zero_width = (adjust_x | adjust_y) != 0;
  return device->fillPath(this, ppath, &params, pdevc, pcpath);
}

int XWPSState::flattenPath()
{
	XWPSPath fpath;
  int code;

  if (!path->hasCurves())
		return 0;
		
  code = fpath.addFlattenedAccurate(path, flatness, accurate_curves);
  if (code < 0) 
		return code;
  path->assign(&fpath, false);
  return 0;
}

int XWPSState::flushPage()
{
	return device->syncOutput();
}

int XWPSState::glyphPathBegin(ulong glyph, 
	                   bool stroke_path,
		                 XWPSTextEnum ** ppte)
{
	XWPSTextParams text;

  text.operation = TEXT_FROM_SINGLE_GLYPH | TEXT_RETURN_WIDTH |	(stroke_path ? TEXT_DO_TRUE_CHARPATH : TEXT_DO_FALSE_CHARPATH);
  text.data.d_glyph = glyph;
  text.size = 1;
  return textBegin(&text, ppte);
}

int XWPSState::glyphShowBegin(ulong glyph, XWPSTextEnum ** ppte)
{
	XWPSTextParams text;

  text.operation = TEXT_FROM_SINGLE_GLYPH | TEXT_DO_DRAW | TEXT_RETURN_WIDTH;
  text.data.d_glyph = glyph;
  text.size = 1;
  return textBegin(&text, ppte);
}

int XWPSState::glyphWidthBegin(ulong glyph, XWPSTextEnum ** ppte)
{
	XWPSTextParams text;

  text.operation = TEXT_FROM_SINGLE_GLYPH | TEXT_DO_NONE | TEXT_RETURN_WIDTH;
  text.data.d_glyph = glyph;
  text.size = 1;
  return textBegin(&text, ppte);
}

int XWPSState::initTransparencyMask(PSTransparencyChannelSelector csel)
{
	XWPSTransparencySource *ptm;
  switch (csel) 
  {
    case TRANSPARENCY_CHANNEL_Opacity: 
    	ptm = &opacity; 
    	break;
    	
    case TRANSPARENCY_CHANNEL_Shape: 
    	ptm = &shape; 
    	break;
    	
    default: 
    	return (int)(XWPSError::RangeCheck);
  }
  return 0;
}

int XWPSState::install(XWPSHalftone * pht, XWPSDeviceHalftone * pdht)
{
	if (pht != halftone)
	{
		if (halftone)
			if (halftone->decRef() == 0)
				delete halftone;
			
		halftone = (XWPSHalftone*)pht;
		if (halftone)
			halftone->incRef();
	}
	
	int code = devhtInstall(pdht, pht->type, device);
	if (code < 0)
		return code;
		
	dev_color->unset(); 
	return 0;
}

int XWPSState::installCieABC(XWPSCieABC *pcie)
{
	cie_matrix_init(&pcie->MatrixABC);
 	
	int j;
	for (j = 0; j < (sizeof(pcie->caches.DecodeABC) / sizeof(pcie->caches.DecodeABC[0])); j++) 
	{
	    PSCieCacheFloats *pcf = &pcie->caches.DecodeABC[j].floats;
	    int i;
	    PSForLoopParams lp;

	    ps_cie_cache_init(&pcf->params, &lp, &pcie->RangeABC.ranges[j]);
	    for (i = 0; i < ps_cie_cache_size; lp.init += lp.step, i++) 
				pcf->values[i] = (pcie->*(pcie->DecodeABC.procs[j]))(lp.init);
	    pcf->params.is_identity =	(pcie->DecodeABC.procs[j] == DecodeABC_default.procs[j]);
	}
 	cieLoadCommonCache(pcie);
  pcie->abccomplete();
  return cieCSComplete(true);
}

int XWPSState::idtransform(float dx, float dy, XWPSPoint * pt)
{
	if (!ctm.isSkewed()) 
	{
		XWPSMatrix c;
		ctmOnly(&c);
		return pt->distanceTransformInverse(dx, dy, &c);
  } 
  else 
  {
  	if (!ctm_inverse_valid)
  	{
  		int code = setInverse();
  		if ( code < 0 ) 
  			return code;
  	}
  	
		return pt->distanceTransform(dx, dy, &ctm_inverse);
  }
}

int XWPSState::imagePath(int width, int height, const uchar * data)
{
	XWPSStatus stat;
  XWPSStatus *out = &stat;
  int code, x, y;
  
  stat.pgs = this;
  stat.data = data;
  stat.width = width;
  stat.height = height;
  stat.raster = (width + 7) / 8;
  
  for (y = height - 1; y >= 0; y--)
  	for (x = width - 1; x >= 0; x--)
  	{
  		if (out->getPixel(x, y) && !out->getPixel(x, y - 1) &&
	      (!out->getPixel(x + 1, y) || out->getPixel(x + 1, y - 1)) &&
					!out->traceFrom(x, y, 1)) 
			{
				stat.count = 0;
				stat.dx = stat.dy = 0;
				if ((code = out->traceFrom(x, y, 0)) < 0)
		    	return code;
				out->addDxDy(0, 0, 1);
				if ((code = closePath()) < 0)
		    	return code;
	    }
  	}
  	
  return 0;
}

int XWPSState::initClip()
{
	XWPSFixedRect box;
  int code = defaultClipBox(&box);
  if (code < 0)
		return code;
  return clipToRectangle(&box);
}

int XWPSState::initGraphics()
{
	int code = initMatrix();
	if (code < 0)
		return code;
		
	code = newPath();
	if (code < 0)
		return code;
		
	code = initClip();
	if (code < 0)
		return code;
		
	setLineWidth(1.0);
	setLineCap(ps_cap_butt);
	setLineJoin(ps_join_miter);
	setCurveJoin(ps_join_bevel);
	code = setDash(0, 0, 0.0);
	if (code < 0)
		return code;
	setDashAdapt(false);
	setDotLength(0.0, false);
	code = setDotOrientation();
	if (code < 0)
		return code;
		
	code = setGray(0.0);
	if (code < 0)
		return code;
		
	setMiterLimit(10.0);
	log_op = lop_default;
	return 0;
}

int XWPSState::initMatrix()
{
	XWPSMatrix imat;
	defaultMatrix(&imat);
	updateCTM(imat.tx, imat.ty);
	ctm = imat;
	return 0;
}

int XWPSState::initViewClip()
{
  if (view_clip != 0 && view_clip->rule != 0) 
  {
		view_clip->reset();
		view_clip->rule = 0;
  }
  return 0;
}

int XWPSState::itransform(float x, float y, XWPSPoint * pt)
{
	if (!ctm.isSkewed()) 
	{
		XWPSMatrix c;
		ctmOnly(&c);
		return pt->transformInverse(x, y, &c);
  } 
  else 
  {
		if (!ctm_inverse_valid)
  	{
  		int code = setInverse();
  		if ( code < 0 ) 
  			return code;
  	}
		return pt->transform(x, y, &ctm_inverse);
  }
}

int XWPSState::kshowBegin(const uchar * str, uint size, XWPSTextEnum ** ppte)
{
	XWPSTextParams text;

  text.operation = TEXT_FROM_STRING | TEXT_DO_DRAW | TEXT_INTERVENE |	TEXT_RETURN_WIDTH;
  text.data.bytes = (uchar*)str, text.size = size;
  return textBegin(&text, ppte);
}

int XWPSState::lineTo(float x, float y)
{
	int code;
  XWPSFixedPoint pt;

  if ((code = pt.transform2fixed(&ctm, x, y)) < 0) 
  {
		if (clamp_coordinates) 
		{
	    XWPSPoint opt;

	    if (code != XWPSError::LimitCheck ||
				(code = opt.transform(x, y, &ctm)) < 0)
				return code;
	    clamp_point(&pt, opt.x, opt.y);
	    code = path->addLineNotes(pt.x, pt.y, sn_none);
	    if (code < 0)
				return code;
	    path->setOutsidePosition(opt.x, opt.y);
		}
		return code;
  }
  return path->addLineNotes(pt.x, pt.y, sn_none);
}

void XWPSState::loadTransferMap(XWPSTransferMap * pmap, float min_value)
{
	ushort *values = ((ushort*)&(pmap->values[0]));
  ushort fmin = float2frac(min_value);
  int i;
  
  for (i = 0; i < transfer_map_size; i++) 
  {
		float fval = pmap->mapping((float)i / (transfer_map_size - 1));

		values[i] = (fval < min_value ? fmin :  fval >= 1.0 ? frac_1 :  float2frac(fval));
  }
}

XWPSCachedFMPair * XWPSState::lookupFMPair(XWPSFont * pfont)
{
	float mxx = char_tm.xx, mxy = char_tm.xy, myx = char_tm.yx,  myy = char_tm.yy;
  XWPSFont *fontA = pfont;
  XWPSFontDir *dir = fontA->dir;
  XWPSCachedFMPair *pair =  dir->fmcache.mdata + dir->fmcache.mnext;
  int count = dir->fmcache.mmax;
  XWPSUid uid;
  if (fontA->FontType == ft_composite || fontA->PaintType != 0)
  	uid.invalid();
  else
  {
  	uid = ((XWPSFontBase *) fontA)->UID;
		if (uid.isValid())
	    fontA = 0;
  }
  
  while (count--)
  {
  	if (pair == dir->fmcache.mdata)
	    pair += dir->fmcache.mmax;
		pair--;
		if (fontA != 0)
		{
			if (pair->font != fontA)
				continue;
		}
		else
		{
			if (!pair->UID.equal(&uid) || pair->FontType != pfont->FontType)
				continue;
		}
		
		if (pair->mxx == mxx && pair->mxy == mxy && pair->myx == myx && pair->myy == myy)
		{
			 if (pair->font == 0) 
					pair->font = pfont;
	    return pair;
		}
  }
  
  return dir->addFMPair(pfont, &uid, this);
}

void XWPSState::lookupXFont(XWPSCachedFMPair * pair, int )
{
	pair->xfont = 0;
}

int XWPSState::moveTo(float x, float y)
{
	XWPSFixedPoint pt;
  int code;

  if ((code = pt.transform2fixed(&ctm, x, y)) < 0) 
  {
		if (clamp_coordinates) 
		{
	    XWPSPoint opt;

	    if (code != XWPSError::LimitCheck ||
					(code = transform(x, y, &opt)) < 0)
				return code;
	    clamp_point(&pt, opt.x, opt.y);
	    code = path->addPoint(pt.x, pt.y);
	    if (code < 0)
				return code;
	    path->setOutsidePosition(opt.x, opt.y);
	    path->outside_start = path->outside_position;
	    path->start_flags = path->state_flags;
		}
		return code;
  }
  return path->addPoint(pt.x, pt.y);
}

int XWPSState::newPath()
{
	return path->newPath();
}

int XWPSState::nullDevice()
{
	if (device == 0 || !device->isNull()) 
	{
	 	XWPSDevice *ndev = new XWPSDeviceNull;
	 	ndev->context_state = i_ctx_p;
		return setDeviceNoErase(ndev);
  }
  return 0;
}

int XWPSState::outputPage(int num_copies, int flush)
{
	if (device->IgnoreNumCopies)
		num_copies = 1;
  return device->outputPage(num_copies, flush);
}

int XWPSState::pathEnumCopyInit(XWPSPathEnum * penum, bool copy)
{
	if (copy)
	{
		XWPSPath *copied_path = new XWPSPath;
		copied_path->copy(path);
		penum->init(copied_path);
		penum->copied_path = copied_path;
	}
	else
		penum->init(path);
		
	currentMatrix(&penum->mat);
  return 0;
}

int XWPSState::pathEnumNext(XWPSPathEnum * penum, XWPSPoint * ppts)
{
	XWPSFixedPoint fpts[3];
  int pe_op = penum->next(fpts);
  int code;

  switch (pe_op) 
  {
		case 0:		/* all done */
		case ps_pe_closepath:
	    break;
	    
		case ps_pe_curveto:
	    if ((code = ppts[1].transformInverse(fixed2float(fpts[1].x), fixed2float(fpts[1].y), &penum->mat)) < 0 ||
					(code = ppts[2].transformInverse(fixed2float(fpts[2].x), fixed2float(fpts[2].y), &penum->mat)) < 0)
				return code;
	    
		case ps_pe_moveto:
		case ps_pe_lineto:
	    if ((code = ppts[0].transformInverse(fixed2float(fpts[0].x), fixed2float(fpts[0].y), &penum->mat)) < 0)
				return code;
		default:
	    break;
  }
  return pe_op;
}

int XWPSState::putDeviceParams(XWPSContextState * ctx, XWPSParamList * plist)
{
	int code = device->putDeviceParams(ctx, plist);

  if (code >= 0)
		updateDevice();
  return code;
}

int XWPSState::rcurveTo(float dx1, float dy1, 
	             float dx2, float dy2, 
	             float dx3, float dy3)
{
	XWPSFixedPoint p1, p2, p3;
  long ptx, pty;
  int code;
  
  if (!path->positionInRange() ||
			(code = p1.distanceTransform2fixed(&ctm, dx1, dy1)) < 0 ||
			(code = p2.distanceTransform2fixed(&ctm, dx2, dy2)) < 0 ||
			(code = p3.distanceTransform2fixed(&ctm, dx3, dy3)) < 0 ||
			(ptx = path->position.x, pty = path->position.y,
	 			code = path->addCurveNotes(ptx + p1.x, pty + p1.y,
				  				ptx + p2.x, pty + p2.y,
				  				ptx + p3.x, pty + p3.y, sn_none)) < 0) 
	{	
		XWPSPoint upt;

		if ((code = currentPoint(&upt)) < 0)
	    return code;
		return curveTo(upt.x + dx1, upt.y + dy1,
			  upt.x + dx2, upt.y + dy2,
			  upt.x + dx3, upt.y + dy3);
  }
  return code;
}

int XWPSState::rectAppend(const XWPSRect * pr, uint count)
{
	for (; count != 0; count--, pr++) 
	{
		float px = pr->p.x, py = pr->p.y, qx = pr->q.x, qy = pr->q.y;
		int code;
		
		if ((qx >= px) != (qy >= py))
	    qx = px, px = pr->q.x;
		if ((code = moveTo(px, py)) < 0 ||
	    	(code = lineTo(qx, py)) < 0 ||
	    	(code = lineTo(qx, qy)) < 0 ||
	    	(code = lineTo(px, qy)) < 0 ||
	    	(code = closePath()) < 0)
	    return code;
  }
  return 0;
}

int XWPSState::rectClip(const XWPSRect * pr, uint count)
{
	int code;
  XWPSPath save;
	
	save.assign(path);
  newPath();
  if ((code = rectAppend(pr, count)) < 0 || (code = clip()) < 0) 
  {
		path->assign(&save, false);
		return code;
  }
   
  newPath();
  return 0;
}

int XWPSState::rectFill(const XWPSRect * pr, uint count)
{
	const XWPSRect *rlist = pr;
  XWPSClipPath *pcpath;
  uint rcount = count;
    int code;

  setDevColor();
  if ((is_fzero2(ctm.xy, ctm.yx) ||
	 			is_fzero2(ctm.xx, ctm.yy)) &&
				effectiveClipPath(&pcpath) >= 0 &&
				pcpath->getList()->isRectangle() &&
				dev_color->load(this, device) >= 0 &&
				device->getAlphaBits(go_graphics)	<= 1) 
	{
		uint i;
		XWPSFixedRect clip_rect;
	
		pcpath->innerBox(&clip_rect);
		for (i = 0; i < count; ++i) 
		{
	    XWPSFixedPoint p, q;
	    XWPSFixedRect draw_rect;
	    int x, y, w, h;

	    if (p.transform2fixed(&ctm, pr[i].p.x, pr[i].p.y) < 0 ||
	    		q.transform2fixed(&ctm, pr[i].q.x, pr[i].q.y) < 0) 
	    {
				goto slow;
	    }
	    draw_rect.p.x = qMin(p.x, q.x) - fill_adjust.x;
	    draw_rect.p.y = qMin(p.y, q.y) - fill_adjust.y;
	    draw_rect.q.x = qMax(p.x, q.x) + fill_adjust.x;
	    draw_rect.q.y = qMax(p.y, q.y) + fill_adjust.y;
	    draw_rect.intersect(clip_rect);
	    x = fixed2int_pixround(draw_rect.p.x);
	    y = fixed2int_pixround(draw_rect.p.y);
	    w = fixed2int_pixround(draw_rect.q.x) - x;
	    h = fixed2int_pixround(draw_rect.q.y) - y;
	    if (w > 0 && h > 0) 
	    {
				if (dev_color->fillRectangle(x, y, w, h,  device, log_op, NULL) < 0)
		    goto slow;
	    }
		}
		return 0;
    
slow:
		rlist = pr + i;
		rcount = count - i;
  } 
  {
		bool do_save = !path->isNull();

		if (do_save) 
		{
	    if ((code = save()) < 0)
				return code;
	    newPath();
		}
		if ((code = rectAppend(rlist, rcount)) < 0 || (code = fill()) < 0)
	    ;
		if (do_save)
	    restore();
		else if (code < 0)
	    newPath();
  }
  return code;
}

int XWPSState::rectStroke(const XWPSRect * pr, uint count, const XWPSMatrix * pmat)
{
	bool do_save = pmat != NULL || !path->isNull();
  int code;

  if (do_save) 
  {
		if ((code = save()) < 0)
	    return code;
		newPath();
  }
  
  if ((code = rectAppend(pr, count)) < 0 ||
			(pmat != NULL && (code = concat(pmat)) < 0) ||
			(code = stroke()) < 0)
		;
  if (do_save)
		restore();
  else if (code < 0)
		newPath();
  return code;
}

int XWPSState::reversePath()
{
	XWPSPath rpath;
  int code;

  code = rpath.copyReversed(path);
  if (code < 0) 
		return code;
    
  path->assign(&rpath, false);
  return 0;
}

int XWPSState::rlineTo(float x, float y)
{
	XWPSFixedPoint dpt;
  long nx, ny;
  int code;

  if (!path->positionInRange() ||
			(code = dpt.distanceTransform2fixed(&ctm, x, y)) < 0 ||
			(((nx = path->position.x + dpt.x) ^ dpt.x) < 0 &&
				(path->position.x ^ dpt.x) >= 0) ||
				(((ny = path->position.y + dpt.y) ^ dpt.y) < 0 &&
				(path->position.y ^ dpt.y) >= 0) ||
				(code = path->addLineNotes(nx, ny, sn_none)) < 0) 
	{
		XWPSPoint upt;

		if ((code = currentPoint(&upt)) < 0)
	   	return code;
		return lineTo(upt.x + x, upt.y + y);
  }
  return code;
}

int XWPSState::remapColor()
{
	XWPSColorSpace *pcs = color_space;
  return pcs->remapColor(this, ccolor, pcs, dev_color, device, ps_color_select_texture);
}

int XWPSState::rmoveTo(float x, float y)
{
	XWPSFixedPoint dpt;
  int code;

  if ((code = dpt.distanceTransform2fixed(&ctm, x, y)) < 0 ||
			(code = path->addRelativePoint(dpt.x, dpt.y)) < 0) 
	{
		XWPSPoint upt;

		if ((code = currentPoint(&upt)) < 0)
	    return code;
		return moveTo(upt.x + x, upt.y + y);
  }
  return code;
}

int XWPSState::restore()
{
	int code;
  if (!saved)
		return save();
		
	code = restoreOnly();
  if (code < 0)
		return code;
		
	if (saved)
		return 0;
  return save();
}

int XWPSState::restoreAll()
{
	if (!saved)
		return save();
		
	while (saved->saved)
	{
		int code = restore();
		if (code < 0)
	    return code;
	}
	
	return restore();
}

int XWPSState::restoreAllForRestore(XWPSState * savedA)
{
	int code;
	while (saved->saved) 
	{
		code = restore();
		if (code < 0)
	    return code;
  }
  
  if (ht_cache)
  	ht_cache->clear();
  	
  if (pattern_cache)
  {
  	if (own_pattern_cache)
  	  delete pattern_cache;
  	pattern_cache = 0;
  	own_pattern_cache = false;
  }
  
  saved->saved = savedA;
  code = restore();
  if (code < 0)
		return code;
	
	return restore();
}

int XWPSState::restoreOnly()
{
	XWPSState *savedA = saved;
  if (!savedA)
		return 1;
	
	void *pdata = client_data;
	client_data = 0;
  void *sdata = savedA->client_data;
  
  if (savedA->pattern_cache == 0)
		savedA->pattern_cache = pattern_cache;
	
	client_data = sdata;
	savedA->client_data = pdata;
	if (pdata != 0 && sdata != 0)
	{
		XWPSIntGState * iigst = (XWPSIntGState*)pdata;
		XWPSIntGState * iigsf = (XWPSIntGState*)sdata;		
		iigst->copy(iigsf);
	}
	
	if (shared != savedA->shared)
	{
		if (shared)
	  {
		  if (!shared->decRef())
			  delete shared;		
	  }
	  
	  shared = savedA->shared;
	  if (shared)
	  	shared->incRef();
	}
	
	line_params.copy(savedA->line_params, false);
	
	ctm = savedA->ctm;
	log_op = savedA->log_op;
	alpha = savedA->alpha;
	blend_mode = savedA->blend_mode;
	opacity.alpha = savedA->opacity.alpha;
	shape.alpha = savedA->shape.alpha;
	text_knockout = savedA->text_knockout;
	overprint = savedA->overprint;
		
	overprint_mode = savedA->overprint_mode;
	flatness = savedA->flatness;
	fill_adjust = savedA->fill_adjust;
	stroke_adjust = savedA->stroke_adjust;
	accurate_curves = savedA->accurate_curves;
	smoothness = savedA->smoothness;
	get_cmap_procs = savedA->get_cmap_procs;
	
	if (halftone != savedA->halftone)
	{
		if (halftone)
		  if (halftone->decRef() == 0)
				delete halftone;
	  
	  halftone = savedA->halftone;
	  if (halftone)
	  	halftone->incRef();
	}
	
	for (int i = 0; i < ps_color_select_count; i++)
		screen_phase[i] = savedA->screen_phase[i];

	if (dev_ht != savedA->dev_ht)
	{
		if (dev_ht)
		  if (dev_ht->decRef() == 0)
		  	delete dev_ht;
	  
	  dev_ht = savedA->dev_ht;
	  if (dev_ht)
	  	dev_ht->incRef();
	}
	
	if (ht_cache != savedA->ht_cache)
	{
		if (ht_cache)
	  {
		  if (!ht_cache->decRef())
		  {
		  	ht_cache->order.cache = 0;
			  delete ht_cache;		
			}
	  }
	  
	  ht_cache = savedA->ht_cache;
	  if (ht_cache)
	  	ht_cache->incRef();
	}
	
	if (cie_render != savedA->cie_render)
	{
		if (cie_render)
		  if (cie_render->decRef() == 0)
				delete cie_render;
	  
	  cie_render = savedA->cie_render;
	  if (cie_render)
	  	cie_render->incRef();
	}
	
	if (black_generation != savedA->black_generation)
	{
		if (black_generation)
	  {
		  if (!black_generation->decRef())
			  delete black_generation;		
	  }
	  
	  black_generation = savedA->black_generation;
	  if (black_generation)
	  	black_generation->incRef();
	}
	
	if (undercolor_removal != savedA->undercolor_removal)
	{
		if (undercolor_removal)
	  {
		  if (!undercolor_removal->decRef())
			  delete undercolor_removal;		
	  }
	  
	  undercolor_removal = savedA->undercolor_removal;
	  if (undercolor_removal)
	  	undercolor_removal->incRef();
	}
	
	set_transfer.copy(&(savedA->set_transfer));
	effective_transfer.copy(&(savedA->effective_transfer));
	
	if (cie_joint_caches != savedA->cie_joint_caches)
	{
		if (cie_joint_caches)
	  {
		  if (!cie_joint_caches->decRef())
			  delete cie_joint_caches;		
	  }
	  
	  cie_joint_caches = savedA->cie_joint_caches;
	  if (cie_joint_caches)
	  	cie_joint_caches->incRef();
	}
	
	cmap_procs = savedA->cmap_procs;
	
	pattern_cache = savedA->pattern_cache;
	
	i_ctx_p = savedA->i_ctx_p;
	
	saved = savedA->saved;
	savedA->saved = 0;
	
	ctm_inverse = savedA->ctm_inverse;
	ctm_inverse_valid = savedA->ctm_inverse_valid;
	ctm_default = savedA->ctm_default;
	ctm_default_set = savedA->ctm_default_set;
	
	if ((view_clip == clip_path) || (view_clip == effective_clip_path))
		view_clip = 0;
		
	if (!effective_clip_shared)
		if (effective_clip_path)
			delete effective_clip_path;
	effective_clip_path = 0;
		
	if (path != savedA->path)
		if (path)
		  delete path;
	path = savedA->path;
	savedA->path = 0;
	if (clip_path != savedA->clip_path)
		if (clip_path)
		 delete clip_path;
	clip_path = savedA->clip_path;
	savedA->clip_path = 0;
	
	if (clip_stack != savedA->clip_stack)
	{
		if (!clip_stack->decRef())
			clearClipStack();
			
		clip_stack = savedA->clip_stack;
	  if (clip_stack)
	  	clip_stack->incRef();
	}
	
	if (view_clip != savedA->view_clip)
		if (view_clip)
		  delete view_clip;
	view_clip = savedA->view_clip;
	savedA->view_clip = 0;
	
	effective_clip_id = savedA->effective_clip_id;
	effective_view_clip_id = savedA->effective_view_clip_id;
	effective_clip_shared = savedA->effective_clip_shared;
	effective_clip_path = savedA->effective_clip_path;
	savedA->effective_clip_path = 0;
	
	clamp_coordinates = savedA->clamp_coordinates;
		
	orig_cspace_index = savedA->orig_cspace_index;
	orig_base_cspace_index = savedA->orig_base_cspace_index;
	
	if (color_space != savedA->color_space)
	{
		if (color_space)
	  {
		  if (!color_space->decRef())
			  delete color_space;		
	  }
	  
	  color_space = savedA->color_space;
	  if (color_space)
	  	color_space->incRef();
	}
	
	if (device_color_spaces.named.Gray != savedA->device_color_spaces.named.Gray)
	{
		if (device_color_spaces.named.Gray)
			if (!device_color_spaces.named.Gray->decRef())
				delete device_color_spaces.named.Gray;
				
		device_color_spaces.named.Gray = savedA->device_color_spaces.named.Gray;
		if (device_color_spaces.named.Gray)
			device_color_spaces.named.Gray->incRef();
	}
	
	if (device_color_spaces.named.RGB != savedA->device_color_spaces.named.RGB)
	{
		if (device_color_spaces.named.RGB)
			if (!device_color_spaces.named.RGB->decRef())
				delete device_color_spaces.named.RGB;
				
		device_color_spaces.named.RGB = savedA->device_color_spaces.named.RGB;
		if (device_color_spaces.named.RGB)
			device_color_spaces.named.RGB->incRef();
	}
	
	if (device_color_spaces.named.CMYK != savedA->device_color_spaces.named.CMYK)
	{
		if (device_color_spaces.named.CMYK)
			if (!device_color_spaces.named.CMYK->decRef())
				delete device_color_spaces.named.CMYK;
				
		device_color_spaces.named.RGB = savedA->device_color_spaces.named.CMYK;
		if (device_color_spaces.named.CMYK)
			device_color_spaces.named.CMYK->incRef();
	}
	
	for (int i = 0; i < 3; i++)
	  if (device_color_spaces.indexed[i] != savedA->device_color_spaces.indexed[i])
	  {
		  if (device_color_spaces.indexed[i])
			  if (!device_color_spaces.indexed[i]->decRef())
				  delete device_color_spaces.indexed[i];
				
		  device_color_spaces.indexed[i] = savedA->device_color_spaces.indexed[i];
		  if (device_color_spaces.indexed[i])
			  device_color_spaces.indexed[i]->incRef();
	  }
	
	if (ccolor != savedA->ccolor)
	{
		if (ccolor)
	  {
		  if (!ccolor->decRef())
			  delete ccolor;		
	  }
	  
	  ccolor = savedA->ccolor;
	  if (ccolor)
	  	ccolor->incRef();
	}
		
	if (dev_color != savedA->dev_color)
		if (dev_color)
		  delete dev_color;
	dev_color = savedA->dev_color;
	savedA->dev_color = 0;
	
	
	if (font != savedA->font)
	{
		if (font)
		  if (font->decRef() == 0)
		  	delete font;
	  
	  font = savedA->font;
	  if (font)
	  	font->incRef();
	}
		
	if (root_font != savedA->root_font)
	{
		if (root_font)
		  if (root_font->decRef() == 0)
		  	delete root_font;
	  
	  root_font = savedA->root_font;
	  if (root_font)
	  	root_font->incRef();
	}
	
	char_tm = savedA->char_tm;
	char_tm_valid = savedA->char_tm_valid;
	in_cachedevice = savedA->in_cachedevice;
	in_charpath = savedA->in_charpath;
	if (show_gstate == savedA)
	  show_gstate =	this;
		
	level = savedA->level;
	
	if (device != savedA->device)
	{
		if (device)
		  if (device->decRef() == 0)
		  	delete device;
	  
	  device = savedA->device;
	  if (device)
	  	device->incRef();
	}
	
	screen_accurate_screens = savedA->screen_accurate_screens;
	screen_min_screen_levels = savedA->screen_accurate_screens;
		
	delete savedA;
	return 0;
}

int XWPSState::rotate(float ang)
{
	XWPSMatrix c;
	ctmOnly(&c);
	int code = c.matrixRotate(&c, ang);
	ctm.xx = c.xx;
	ctm.xy = c.xy;
	ctm.yx = c.yx;
	ctm.yy = c.yy;

  ctm_inverse_valid = false, char_tm_valid = false;
  return code;
}

int  XWPSState::save()
{
	saved = new XWPSState(this, copy_for_gsave);
	saved->clip_stack = 0;
	if (show_gstate == this)
	{
		saved->show_gstate = saved;
		show_gstate = saved;
	}
  level++;
  return 0;
}

int  XWPSState::saveForSave(XWPSState ** psaved)
{
  XWPSClipPath *old_cpath = view_clip;
  XWPSClipPath *new_cpath = 0;
  if (old_cpath)
  	new_cpath = new XWPSClipPath(old_cpath);
  	
  int code = save();
  if (code < 0)
		goto fail;
		
	for (int i = 0; i < 3; ++i)
	{
		XWPSColorSpace *pcs = device_color_spaces.indexed[i];
		if (pcs)
		{
			device_color_spaces.indexed[i] = 0;
			code = setSubstituteColorSpace((PSColorSpaceIndex)i, pcs);
			if (!pcs->decRef())
				delete pcs;
	    if (code < 0)
	    {
	    	if (saved->saved == 0)
		    	saved->saved = this;
				restore();
				if (saved == this)
		    	saved = 0;
				goto fail;
	    }
		}
	}
	
	if (effective_clip_path == view_clip)
		effective_clip_path = new_cpath;
		
	if (new_cpath)
	{
		if (old_cpath)
			if (old_cpath != effective_clip_path)
		    delete old_cpath;
	}
		
	view_clip = new_cpath;
	*psaved = saved;
  saved = 0;
  return code;
  
fail:
  if (new_cpath)
		delete new_cpath;
  return code;
}

int  XWPSState::scale(float sx, float sy)
{
	ctm.xx *= sx;
  ctm.xy *= sx;
  ctm.yx *= sy;
  ctm.yy *= sy;
  ctm_inverse_valid = false, char_tm_valid = false;
  return 0;
}

int  XWPSState::scaleCharMatrix(int sx, int sy)
{
#define scale_cxy(s, vx, vy)\
  if ( s != 1 )\
   {	ctm.vx *= s;\
			ctm.vy *= s;\
			ctm_inverse_valid = false;\
	if ( char_tm_valid )\
	{	char_tm.vx *= s;\
		char_tm.vy *= s;\
	}\
   }
   
  scale_cxy(sx, xx, yx);
  scale_cxy(sy, xy, yy);
#undef scale_cxy
  
  return 0;
}

int  XWPSState::setAlpha(float alphaA)
{
	alpha =	(ushort) (alphaA < 0 ? 0 : alphaA > 1 ? ps_max_color_value : alphaA * ps_max_color_value);
  dev_color->unset();
  return 0;
}

int  XWPSState::setBbox(float llx, float lly, float urx, float ury)
{
	XWPSRect ubox, dbox;
  XWPSFixedRect obox, bbox;
  XWPSPath *ppath = path;
  int code;

  if (llx > urx || lly > ury)
	  return (int)(XWPSError::RangeCheck);
	
  ubox.p.x = llx;
  ubox.p.y = lly;
  ubox.q.x = urx;
  ubox.q.y = ury;
  XWPSMatrix m;
  ctmOnly(&m);
  if ((code = ubox.bboxTransform(&m, &dbox)) < 0)
		return code;
		
  if (dbox.p.x < fixed2float(min_fixed + box_rounding_slop_fixed) ||
			dbox.p.y < fixed2float(min_fixed + box_rounding_slop_fixed) ||
			dbox.q.x >= fixed2float(max_fixed - box_rounding_slop_fixed + fixed_epsilon) ||
			dbox.q.y >= fixed2float(max_fixed - box_rounding_slop_fixed + fixed_epsilon))
	{
		return (int)(XWPSError::LimitCheck);
	}
	
  bbox.p.x = (long) floor(dbox.p.x * fixed_scale) - box_rounding_slop_fixed;
  bbox.p.y = (long) floor(dbox.p.y * fixed_scale) - box_rounding_slop_fixed;
  bbox.q.x =	(long) ceil(dbox.q.x * fixed_scale) + box_rounding_slop_fixed;
  bbox.q.y =	(long) ceil(dbox.q.y * fixed_scale) + box_rounding_slop_fixed;
  if (ppath->getBbox(&obox) >= 0) 
  {	
		ppath->bbox.p.x = qMin(obox.p.x, bbox.p.x);
		ppath->bbox.p.y = qMin(obox.p.y, bbox.p.y);
		ppath->bbox.q.x = qMax(obox.q.x, bbox.q.x);
		ppath->bbox.q.y = qMax(obox.q.y, bbox.q.y);
  } 
  else 
		ppath->bbox = bbox;
  ppath->bbox_set = 1;
  return 0;
}

int XWPSState::setBlackGeneration(PSTransferMapType proc)
{
	return setBlackGenerationRemap(proc, true);
}

int  XWPSState::setBlackGenerationRemap(PSTransferMapType proc, bool remap)
{
	if (!black_generation)
		black_generation = new XWPSTransferMap;
		
	black_generation->id = i_ctx_p->nextIDS(1);
	black_generation->setType(proc);
	if (remap) 
	{
		loadTransferMap(black_generation, 0.0);
		dev_color->unset();
  }
  return 0;
}

int  XWPSState::setBlendMode(PSBlendMode mode)
{
	if (mode < 0 || mode > MAX_BLEND_MODE)
  	return (int)(XWPSError::RangeCheck);
 	blend_mode = mode;
  return 0;
}

int  XWPSState::setCharMatrix(const XWPSMatrix * pmat)
{
	XWPSMatrix cmat, c;
	ctmOnly(&c);
  int code = cmat.matrixMultiply((XWPSMatrix*)pmat, &c);

  if (code < 0)
		return code;
		
  code = char_tm.updateMatrixFixed(cmat.tx, cmat.ty);  
  if (code < 0)
		return code;
		
  char_tm = cmat;
  char_tm_valid = true;
  return 0;
}

int  XWPSState::setCMYKColor(float c, float m, float y, float k)
{
	if (in_cachedevice)
		return (int)(XWPSError::Undefined);
	
  color_space->assign(currentDeviceCMYKSpace());
  orig_cspace_index = orig_base_cspace_index = ps_color_space_index_DeviceCMYK;
  ccolor->paint.values[0] = FORCE_UNIT(c);
  ccolor->paint.values[1] = FORCE_UNIT(m);
  ccolor->paint.values[2] = FORCE_UNIT(y);
  ccolor->paint.values[3] = FORCE_UNIT(k);
  dev_color->unset();
  return 0;
}

int  XWPSState::setColor(XWPSClientColor * pcc)
{
	if (in_cachedevice)
		return (int)(XWPSError::Undefined);
  *ccolor = *pcc;
  color_space->restrictColor(ccolor);
  dev_color->unset();
  return 0;
}

int  XWPSState::setColorRendering(XWPSCieRender * pcrd)
{
	int code = pcrd->complete();
  const XWPSCieRender *pcrd_old = cie_render;
  bool joint_ok;

  if (code < 0)
		return code;
  
  if (pcrd_old != 0 && pcrd->id == pcrd_old->id)
		return 0;
  
  joint_ok =	pcrd_old != 0 &&
  
#define CRD_SAME(elt) !memcmp(&pcrd->elt, &pcrd_old->elt, sizeof(pcrd->elt))
	CRD_SAME(points.WhitePoint) && CRD_SAME(points.BlackPoint) &&
	CRD_SAME(MatrixPQR) && CRD_SAME(RangePQR) &&
	CRD_SAME(TransformPQR);
#undef CRD_SAME

	if (cie_render != pcrd)
	{
		if (cie_render)
			if (cie_render->decRef() == 0)
				delete cie_render;
		cie_render = pcrd;
		if (cie_render)
			cie_render->incRef();
	}
	
  if (!joint_ok)
		code = cieCSComplete(true);
  dev_color->unset();
  return code;
}

int  XWPSState::setColorSpace(XWPSColorSpace * pcs)
{
	int code;
  XWPSColorSpace cs_old;
  cs_old.i_ctx_p = i_ctx_p;
  XWPSClientColor cc_old;

  if (in_cachedevice)
		return (int)(XWPSError::Undefined);
    
  if (pcs->id == color_space->id) 
  {
  	ccolor->pattern = 0;
  	pcs->initColor(ccolor);
		return 0;
  }
  cs_old.copy(color_space);
  cc_old = *ccolor;
  color_space->copy(pcs);
  if ((code = pcs->installCSpace(this)) < 0)
		goto rcs;
		
  ccolor->pattern = 0;
  pcs->initColor(ccolor);
  orig_cspace_index = pcs->type->index;
  {
		XWPSColorSpace *pccs = pcs;
		XWPSColorSpace *pbcs;

		while ((pbcs = pccs->baseSpace()) != 0)
	    pccs = pbcs;
		orig_base_cspace_index = pccs->type->index;
  }
  dev_color->unset();
  return code;
    
rcs:
	color_space->copy(&cs_old);
  return code;
}

int  XWPSState::setColorTransfer(PSTransferMapType red_proc,
		                        PSTransferMapType green_proc, 
		                        PSTransferMapType blue_proc,
		                        PSTransferMapType gray_proc)
{
	return setColorTransferRemap(red_proc, green_proc,  blue_proc, gray_proc, true);
}

int  XWPSState::setColorTransferRemap(PSTransferMapType red_proc,
			  										 PSTransferMapType green_proc,
			  										 PSTransferMapType blue_proc,
			  										 PSTransferMapType gray_proc, 
			  										 bool remap)
{
	XWPSTransferColored *ptran = &set_transfer.u.colored;
  ulong new_ids = 0;
  new_ids = i_ctx_p->nextIDS(4);
  
  ptran->gray->setType(gray_proc);
  ptran->gray->id = new_ids;
  
  if (!(ptran->red))
  	ptran->red = new XWPSTransferMap;
  else if (ptran->red == ptran->gray)
  {
  	ptran->red->decRef();
  	ptran->red = new XWPSTransferMap;
  }
  
  ptran->red->setType(red_proc);
  ptran->red->id = new_ids + 1;
  
  if (!(ptran->green))
  	ptran->green = new XWPSTransferMap;
  else if (ptran->green == ptran->gray)
  {
  	ptran->green->decRef();
  	ptran->green = new XWPSTransferMap;
  }
  
  ptran->green->setType(green_proc);
  ptran->green->id = new_ids + 2;
  
  if (!(ptran->blue))
  	ptran->blue = new XWPSTransferMap;
  else if (ptran->blue == ptran->gray)
  {
  	ptran->blue->decRef();
  	ptran->blue = new XWPSTransferMap;
  }
  
  ptran->blue->setType(blue_proc);
  ptran->blue->id = new_ids + 3;
  if (remap) 
  {
		loadTransferMap(ptran->red, 0.0);
		loadTransferMap(ptran->green, 0.0);
		loadTransferMap(ptran->blue, 0.0);
		loadTransferMap(ptran->gray, 0.0);
		setEffectiveTransfer();
		dev_color->unset();
  }
  return 0;
}

void  XWPSState::setCurrentFont(XWPSFont * pfont)
{
	if (font != pfont)
	{
		if (font)
			if (font->decRef() == 0)
				delete font;
		font = pfont;
		if (font)
			font->incRef();
	}
  char_tm_valid = false;
}

int  XWPSState::setDefaultMatrix(const XWPSMatrix * pmat)
{
	if (pmat == NULL)
		ctm_default_set = false;
  else 
  {
		ctm_default = *((XWPSMatrix*)pmat);
		ctm_default_set = true;
  }
  return 0;
}

int  XWPSState::setDevColor()
{
	if (!dev_color->isSet())
	{
		int code_dc = remapColor();
		if ( code_dc != 0 ) 
			return code_dc;
	}
	
	return 0;
}

void XWPSState::setDeviceColor1()
{
	ccolor->paint.values[0] = 0.0;
  ccolor->pattern = 0;
	dev_color->setPure(1);
	log_op = lop_default;
	color_space->assign(deviceGray());
	orig_cspace_index = orig_base_cspace_index =	ps_color_space_index_DeviceGray;
}

int  XWPSState::setDeviceNoErase(XWPSDevice * dev)
{
	int open_code = 0, code;
	if (!dev->is_open) 
	{
		if (dev->isMemory()) 
		{
	    XWPSDevice *odev = currentDevice();

	    while (odev != 0 && odev->isMemory())
				odev = ((XWPSDeviceMem *)odev)->target;
	    ((XWPSDeviceForward *)dev)->setTarget(odev);
		}
		code = open_code = dev->open();
		if (code < 0)
	    return code;
  }
  setDeviceNoInit(dev);
  ctm_default_set = false;
  if ((code = initMatrix()) < 0 ||
			(code = initClip()) < 0)
		return code;
		
  in_cachedevice = CACHE_DEVICE_NONE;
  in_charpath = cpm_show;
  return open_code;
}

int  XWPSState::setDeviceNoInit(XWPSDevice * dev)
{
	if (device != dev)
	{
		if (device)
			if (device->decRef() == 0)
				delete device;
		
		device = dev;
		if (device)
			device->incRef();
	}
	updateDevice();
	return 0;
}

int  XWPSState::setDeviceOnly(XWPSDevice * dev)
{
	if (device != dev)
	{
		if (device)
			if (device->decRef() == 0)
				delete device;
		
		device = dev;
		if (device)
			device->incRef();
	}
	return 0;
}

int  XWPSState::setDotOrientation()
{
	if (ctm.isXXYY() || ctm.isXYYX())
		return currentMatrix(&(line_params.dot_orientation));
    
  return (int)(XWPSError::RangeCheck);
}

void XWPSState::setEffectiveTransfer()
{
	setEffectiveXFer();
}

int XWPSState::setFont(XWPSFont * pfont)
{
	if (pfont != font)
	{
		if (font)
			if (font->decRef() == 0)
				delete font;
		font = pfont;
		if (font)
			font->incRef();
	}
	
	if (pfont != root_font)
	{
		if (root_font)
			if (root_font->decRef() == 0)
				delete root_font;
		root_font = pfont;
		if (root_font)
			root_font->incRef();
	}
	
  char_tm_valid = false;
  return 0;
}

int  XWPSState::setGray(float gray)
{
	if (in_cachedevice)
		return (int)(XWPSError::Undefined);
			
  color_space->assign(currentDeviceGraySpace());
  orig_cspace_index = orig_base_cspace_index =	ps_color_space_index_DeviceGray;
  ccolor->paint.values[0] = FORCE_UNIT(gray);
  dev_color->unset();
  return 0;
}

int  XWPSState::setGState(XWPSState * pfrom)
{
	XWPSState *saved_show = show_gstate;
  int levelA = level;
  XWPSClipPath * ovcl = view_clip;
  view_clip = 0;
  XWPSTransparencyState *tstack = transparency_stack;
  copy(pfrom, copy_for_setgstate);    
  level = levelA;
  if ((view_clip == clip_path) || (view_clip == effective_clip_path))
  	view_clip = 0;
  if (view_clip != ovcl)
  	if (view_clip)
  		delete view_clip;
  view_clip = ovcl;
  show_gstate =	(show_gstate == pfrom ? this : saved_show);
  transparency_stack = tstack;
  return 0;
}

int  XWPSState::setHalftonePrepare(XWPSHalftone * pht, XWPSDeviceHalftone * pdht)
{
	XWPSHTOrderComponent *pocs = 0;
  int code = 0;

  switch (pht->type)
  {
  	case ht_type_colorscreen:
	    {
				XWPSScreenHalftone *phc =	pht->params.colorscreen->screens.indexed[0];
				static const PSHTSeparationName cnames[4] =
				{
			    ps_ht_separation_Default, ps_ht_separation_Red,
			    ps_ht_separation_Green, ps_ht_separation_Blue
				};
				static const int cindex[4] = {3, 0, 1, 2};
				int i;

				pocs = new XWPSHTOrderComponent[4];
				for (i = 0; i < 4; i++) 
				{
		    	XWPSScreenEnum senum;
		    	int ci = cindex[i];
		    	XWPSHTOrderComponent *poc = &pocs[i];

		    	code = senum.processScreenMemory(this, &phc[ci], currentAccurateScreens());
		    	if (code < 0)
						break;
					
		    	poc->corder = senum.order;
		    	poc->cname = cnames[i];
		    	if (i == 0)	
						pdht->order = senum.order;
		    	else 
		    	{
						uint tile_bytes = senum.order.raster * (senum.order.num_bits / senum.order.width);
						uint num_tiles =  4096 / tile_bytes + 1;
						if (poc->corder.cache)
							if (poc->corder.cache->decRef() == 0)
							{
								poc->corder.cache->order.cache = 0;
							  delete poc->corder.cache;
							}
						poc->corder.cache = new XWPSHTCache(num_tiles, tile_bytes * num_tiles);
						poc->corder.cache->base_id = i_ctx_p->nextIDS(poc->corder.num_levels + 1);
						poc->corder.cache->init(&poc->corder);
		    	}
				}
				if (code < 0)
		    	break;
				pdht->components = pocs;
				pdht->num_comp = 4;
	    }
	    break;
	    
		case ht_type_spot:
	    code = pdht->order.processSpot(this, pht->params.spot);	    
	    if (code < 0)
				return code;
			if (pdht->order.transfer)
				pdht->order.transfer->id = i_ctx_p->nextIDS(1);
	    break;
	    
		case ht_type_threshold:
	    code = pdht->order.processThreshold(this, pht->params.threshold);
	    if (pdht->order.transfer)
				pdht->order.transfer->id = i_ctx_p->nextIDS(1);
	    if (code < 0)
				return code;
	    break;
	    
		case ht_type_threshold2:
	    code = pdht->order.processThreshold2(this, pht->params.threshold2);
	    if (pdht->order.transfer)
				pdht->order.transfer->id = i_ctx_p->nextIDS(1);
	    if (code < 0)
				return code;
	    break;
	    
		case ht_type_client_order:
	    code = pdht->order.processClientOrder(this, pht->params.client_order);
	    if (pdht->order.transfer)
				pdht->order.transfer->id = i_ctx_p->nextIDS(1);
	    if (code < 0)
				return code;				
	    break;
	    
		case ht_type_multiple:
		case ht_type_multiple_colorscreen:
	    {
				uint count = pht->params.multiple->num_comp;
				bool have_Default = false;
				uint i;
				XWPSHalftoneComponent *phc = pht->params.multiple->components;
				XWPSHTOrderComponent *poc_next;
        if (count > 0)
				  pocs = new XWPSHTOrderComponent[count];
				poc_next = pocs + 1;
				for (i = 0; i < count; i++, phc++) 
				{
		    	XWPSHTOrderComponent *poc;

		    	if (phc->cname == ps_ht_separation_Default) 
		    	{
						if (have_Default) 
						{
			    		code = (int)(XWPSError::RangeCheck);
			    		break;
						}
					
						poc = pocs;
						have_Default = true;
		    	} 
		    	else if (i == count - 1 && !have_Default) 
		    	{
			    	code = (int)(XWPSError::RangeCheck);
						break;
		    	} 
		    	else
						poc = poc_next++;
		    	poc->cname = phc->cname;
		    	switch (phc->type) 
		    	{
						case ht_type_spot:
			    		code = poc->corder.processSpot(this, phc->params.spot);
			    		break;
			    		
						case ht_type_threshold:
			    		code = poc->corder.processThreshold(this, phc->params.threshold);
			    		break;
			    			
						case ht_type_threshold2:
			    		code = poc->corder.processThreshold2(this, phc->params.threshold2);
			    		break;
			    		
						case ht_type_client_order:
			    		code = poc->corder.processClientOrder(this, phc->params.client_order);
			    		break;
			    		
						default:
			    		code = (XWPSError::RangeCheck);
			    		break;
		    	}
		    	if (code < 0)
						break;
		    	if (poc != pocs) 
		    	{
						if (poc->corder.cache)
							if (poc->corder.cache->decRef() == 0)
							{
								poc->corder.cache->order.cache = 0;
							  delete poc->corder.cache;
							}
						poc->corder.cache = new XWPSHTCache(4, poc->corder.raster * (poc->corder.num_bits / poc->corder.width) * 4);
						if (i_ctx_p)
							poc->corder.cache->base_id = i_ctx_p->nextIDS(poc->corder.num_levels + 1);
						poc->corder.cache->init(&poc->corder);
		    	}
				}
				if (code < 0)
		    	break;
				pdht->order = pocs[0].corder;
				if (count == 1) 
				{
					delete [] pocs;
		    	pdht->components = 0;
				} 
				else 
				{
		    	pdht->components = pocs;
		    	pdht->num_comp = count;
				}
	    }
	    break;
	    
		default:
	    return (int)(XWPSError::RangeCheck);
  }
  
  if (code < 0)
  {
  	if (pocs)
			delete [] pocs;
	}
  
  return code;
}

int  XWPSState::setHSBColor(float h, float s, float b)
{
	float rgb[3];

  color_hsb_to_rgb(force_unit(h), force_unit(s), force_unit(b), rgb);
  return setRGBColor(rgb[0], rgb[1], rgb[2]);
}

int  XWPSState::setInverse()
{
	XWPSMatrix c;
	ctmOnly(&c);
	int code = ctm_inverse.matrixInvert(&c);
  if (code < 0)
		return code;
  ctm_inverse_valid = true;
  return 0;
}

int  XWPSState::setMatrix(XWPSMatrix * pmat)
{
	updateCTM(pmat->tx, pmat->ty);
	ctm = *pmat;
	return 0;
}

int  XWPSState::setNullColor()
{
	if (in_cachedevice)
		return (int)(XWPSError::Undefined);
  setGray(0.0);
  dev_color->setNull();
  return 0;
}

int  XWPSState::setOpacityAlpha(float alpha)
{
	opacity.alpha = (alpha < 0.0 ? 0.0 : alpha > 1.0 ? 1.0 : alpha);
  return 0;
}

int  XWPSState::setOverprintMode(int mode)
{
	if (mode < 0 || mode > 1)
	 	return (int)(XWPSError::RangeCheck);
  overprint_mode = mode;
  return 0;
}

int  XWPSState::setRasterOp(PSRop3 rop)
{
	if (in_cachedevice)
		return (int)(XWPSError::Undefined);
   
  log_op = (rop & rop3_1) | (log_op & ~rop3_1);
  return 0;
}

int  XWPSState::setRGBColor(float r, float g, float b)
{
	if (in_cachedevice)
		return (int)(XWPSError::Undefined);
	
  color_space->assign(currentDeviceRGBSpace());
  orig_cspace_index = orig_base_cspace_index = ps_color_space_index_DeviceRGB;
  ccolor->paint.values[0] = FORCE_UNIT(r);
  ccolor->paint.values[1] = FORCE_UNIT(g);
  ccolor->paint.values[2] = FORCE_UNIT(b);
  dev_color->unset();
  return 0;
}

int  XWPSState::setScreen(XWPSScreenHalftone * phsp)
{
	XWPSScreenEnum senum;
  int code = senum.processScreenMemory(this, phsp,  currentAccurateScreens());

  if (code < 0)
		return code;
  return senum.install();
}

int  XWPSState::setScreenPhase(int x, int y, PSColorSelect select)
{
	int code = XWPSImagerState::setScreenPhase(x,y,select);
	if (code >= 0 && (select == ps_color_select_texture || select == ps_color_select_all))
		dev_color->unset();
  return code;
}

int  XWPSState::setShapeAlpha(float alpha)
{
	shape.alpha = (alpha < 0.0 ? 0.0 : alpha > 1.0 ? 1.0 : alpha);
  return 0;
}

int XWPSState::setSmoothness(float smoothnessA)
{
	smoothness =	(smoothnessA < 0 ? 0 : smoothnessA > 1 ? 1 : smoothnessA);
  return 0;
}

int  XWPSState::setSourceTransparent(bool transparent)
{
	if (in_cachedevice)
		return (int)(XWPSError::Undefined);
	
  log_op =(transparent ? log_op | lop_S_transparent : log_op & ~lop_S_transparent);
  return 0;
}

int  XWPSState::setSubstituteColorSpace(PSColorSpaceIndex csi, XWPSColorSpace *pcs)
{
	int index = (int)csi;
  static const uint masks[3] = {
		(1 << ps_color_space_index_DeviceGray) |
	  (1 << ps_color_space_index_CIEA),
		(1 << ps_color_space_index_DeviceRGB) |
	  (1 << ps_color_space_index_CIEABC) |
	  (1 << ps_color_space_index_CIEDEF),
		(1 << ps_color_space_index_DeviceCMYK) |
	  (1 << ps_color_space_index_CIEDEFG)};
    
  XWPSColorSpace *pcs_old;
  if (index < 0 || index > 2)
		return (int)(XWPSError::RangeCheck);
			
	if (pcs) 
	{
		if (!masks[index] & (1 << pcs->getIndex()))
	    return (int)(XWPSError::RangeCheck);
  }
  
  pcs_old = device_color_spaces.indexed[index];
  if (pcs_old == 0)
  {
  	if (pcs == 0 || pcs->getIndex() == csi)
	    return 0;
	    
	  XWPSColorSpace *pcs_new = new XWPSColorSpace;
	  pcs_new->i_ctx_p = i_ctx_p;
	  pcs_new->initFrom(pcs);
	  device_color_spaces.indexed[index] = pcs_new;
  }
  else
  	pcs_old->assign(pcs ? pcs : shared->device_color_spaces.indexed[index]);
  		
  return 0;
}

int  XWPSState::setTextKnockout(bool knockout)
{
	text_knockout = knockout;
  return 0;
}

int  XWPSState::setTextureTransparent(bool transparent)
{
	if (in_cachedevice)
		return (int)(XWPSError::Undefined);
	
  log_op =(transparent ? log_op | lop_T_transparent : log_op & ~lop_T_transparent);
  return 0;
}

int  XWPSState::setToCharMatrix()
{
	if (char_tm_valid) 
	{
		ctm = char_tm;
		ctm_inverse_valid = false;
		return 0;
  } 
  else
		return (int)(XWPSError::UndefinedResult);
}

int  XWPSState::setTransfer(int t)
{
	return setTransferRemap(t, true);
}

int  XWPSState::setTransferRemap(int t, bool remap)
{
	set_transfer.u.colored.gray->setType((PSTransferMapType)t);
	set_transfer.u.colored.gray->id = i_ctx_p->nextIDS(1);
	if (set_transfer.u.colored.red != set_transfer.u.colored.gray)
	{
		if (!(set_transfer.u.colored.red->decRef()))
				delete set_transfer.u.colored.red;	
		set_transfer.u.colored.red = set_transfer.u.colored.gray;
		set_transfer.u.colored.red->incRef();
	}
	if (set_transfer.u.colored.green != set_transfer.u.colored.gray)
	{
		if (!(set_transfer.u.colored.green->decRef()))
				delete set_transfer.u.colored.green;	
		set_transfer.u.colored.green = set_transfer.u.colored.gray;
		set_transfer.u.colored.green->incRef();
	}
	if (set_transfer.u.colored.blue != set_transfer.u.colored.gray)
	{
		if (!(set_transfer.u.colored.blue->decRef()))
				delete set_transfer.u.colored.blue;	
		set_transfer.u.colored.blue = set_transfer.u.colored.gray;
		set_transfer.u.colored.blue->incRef();
	}
	if (remap)
	{
		set_transfer.loadTransferMap(0.0);
		setEffectiveTransfer();
		dev_color->unset();
	}
	return 0;
}

int  XWPSState::setUnderColorRemoval(PSTransferMapType proc)
{
	return setUnderColorRemovalRemap(proc, true);
}

int  XWPSState::setUnderColorRemovalRemap(PSTransferMapType proc, bool remap)
{
	if (!undercolor_removal)
		undercolor_removal = new XWPSTransferMap;
		
	undercolor_removal->id = i_ctx_p->nextIDS(1);
	undercolor_removal->setType(proc);
	if (remap) 
	{
		loadTransferMap(undercolor_removal, 0.0);
		dev_color->unset();
  }
  return 0;
}

int  XWPSState::shfill(XWPSShading * psh)
{
	XWPSPattern2 pat;
  XWPSPath cpath;
  XWPSMatrix imat;
  XWPSClientColor cc;
  XWPSColorSpace cs;
  cs.i_ctx_p = i_ctx_p;
  XWPSDeviceColor devc;
  int code;

  pat.Shading = psh;
  code = cc.makePattern(&pat, &imat, this);
  if (code < 0)
		return code;
    
  cs.init(&ps_color_space_type_Pattern);
  cs.params.pattern->has_base_space = false;
  code = cs.remapColor(this, &cc, &cs, &devc, device, ps_color_select_texture);
  if (code >= 0) 
  {
		code = clip_path->toPath(&cpath);
		if (code >= 0)
	    code = fillPath(&cpath, &devc, ps_rule_winding_number,fixed_0, fixed_0);
  }
  return code;
}

int  XWPSState::showBegin(const uchar * str, uint size, XWPSTextEnum ** ppte)
{
	XWPSTextParams text;

  text.operation = TEXT_FROM_STRING | TEXT_DO_DRAW | TEXT_RETURN_WIDTH;
  text.data.bytes = (uchar*)str, text.size = size;
  return textBegin(&text,ppte);
}

int XWPSState::stringWidthBegin(const uchar * str, uint size, XWPSTextEnum ** ppte)
{
	XWPSTextParams text;

  text.operation = TEXT_FROM_STRING | TEXT_DO_NONE | TEXT_RETURN_WIDTH;
  text.data.bytes = (uchar*)str, text.size = size;
  return textBegin(&text, ppte);
}

int  XWPSState::stroke()
{
	int code;
	if (in_charpath)
	{
		if (in_charpath == cpm_true_charpath)
		{
			code = strokePath();
	    if (code < 0)
				return code;
		}
		code = show_gstate->path->addCharPath(path, in_charpath);
	}
	else
	{
		int abits, acode;

		setDevColor();
		code = colorLoad();
		if (code < 0)
	    return code;
		abits = alphaBufferBits();
		if (abits > 1)
		{
			float xxyy = fabs(ctm.xx) + fabs(ctm.yy);
	    float xyyx = fabs(ctm.xy) + fabs(ctm.yx);
	    float scale = 1 << (abits / 2);
	    float orig_width = currentLineWidth();
	    float new_width = orig_width * scale;
	    long extra_adjust =	float2fixed(qMax(xxyy, xyyx) * new_width / 2);
	    float orig_flatness = currentFlat();
	    XWPSPath spath;
	    
	    if (extra_adjust < fixed_1)
				extra_adjust = fixed_1;
	    acode = alphaBufferInit(fill_adjust.x + extra_adjust, fill_adjust.y + extra_adjust, abits);
	    if (acode < 0)
				return acode;
	    setLineWidth(new_width);
	    scaleDashPattern(scale);
	    setFlat(orig_flatness * scale);
	    
	    code = strokeAdd(path, &spath);
	    setLineWidth(orig_width);
	    scaleDashPattern(1.0 / scale);
	    if (code >= 0)
				code = fillPath(&spath, dev_color, ps_rule_winding_number, fill_adjust.x, fill_adjust.y);
	    setFlat(orig_flatness);
	    if (acode > 0)
				alphaBufferRelease(code >= 0);
		}
		else
			code = strokeFill(path);
			
		if (code >= 0)
	    newPath();
	}
	
	return code;
}

int  XWPSState::strokeAdd(XWPSPath * ppath, XWPSPath * to_path)
{
	XWPSStrokeParams params;

  params.flatness = (in_cachedevice > 1 ? 0.0 : flatness);
  return device->strokePathOnly(ppath, to_path, this, &params, NULL, NULL);
}

int  XWPSState::strokeFill(XWPSPath * ppath)
{
	XWPSClipPath *pcpath;
  int code = effectiveClipPath(&pcpath);
  XWPSStrokeParams params;

  if (code < 0)
		return code;
  
  params.flatness = (in_cachedevice > 1 ? 0.0 : flatness);
  return device->strokePath(this, ppath, &params, dev_color, pcpath);
}

int  XWPSState::strokePath()
{
	XWPSPath spath;
  int code;
  
  code = strokeAdd(path, &spath);
  if (code < 0)
		return code;
    
  return path->assign(&spath, false);
}

XWPSState * XWPSState::swapSaved(XWPSState * new_saved)
{
	XWPSState * ret = saved;
	saved = new_saved;
  return ret;
}

int XWPSState::textBegin(XWPSTextParams * text, XWPSTextEnum ** ppte)
{
	XWPSClipPath *pcpath = 0;

  if (text->operation & TEXT_DO_DRAW) 
  {
		int code = effectiveClipPath(&pcpath);

		if (code < 0)
	    return code;
		setDevColor();
		code = dev_color->loadSelect(this, device, ps_color_select_texture);
		if (code < 0)
	    return code;
  }
  return device->textBeginGX(this, text, font, path, dev_color,	pcpath, ppte);
}

int XWPSState::transform(float x, float y, XWPSPoint * pt)
{
	return pt->transform(x, y, &ctm);
}

int XWPSState::translate(float dx, float dy)
{
	XWPSPoint pt;
  int code;

	XWPSMatrix c;
	ctmOnly(&c);
  if ((code = pt.distanceTransform(dx, dy, &c)) < 0)
		return code;
  pt.x += c.tx;
  pt.y += c.ty;
  updateCTM(pt.x, pt.y);
  return 0;
}

int XWPSState::translateToFixed(long px, long py)
{
	double fpx = fixed2float(px);
  double fdx = fpx - ctm.tx;
  double fpy = fixed2float(py);
  double fdy = fpy - ctm.ty;
  long dx, dy;
  int code;
  
  if (ctm.txy_fixed_valid)
  {
  	dx = float2fixed(fdx);
		dy = float2fixed(fdy);
		code = path->translate(dx, dy);
		if (code < 0)
	    return code;
		if (char_tm_valid && char_tm.txy_fixed_valid)
	    char_tm.tx_fixed += dx,
		char_tm.ty_fixed += dy;
  }
  else
  {
  	if (!path->isNull())
	    return (int)(XWPSError::LimitCheck);
  }
  
  ctm.tx = fpx;
  ctm.tx_fixed = px;
  ctm.ty = fpy;
  ctm.ty_fixed = py;
  ctm.txy_fixed_valid = true;
  ctm_inverse_valid = false;
  
  if (char_tm_valid)
  {
  	char_tm.tx += fdx;
		char_tm.ty += fdy;
  }
  
  return 0;
}

void XWPSState::unsetDevColor()
{
	if (dev_color)
		dev_color->unset();
}

void XWPSState::updateCTM(double xt, double yt)
{
	ctm_inverse_valid = false;
	char_tm_valid = false;
	ctm.updateMatrixFixed(xt, yt);
}

void XWPSState::updateDevice()
{
	setCmapProcs(device);
  unsetDevColor();
}

int  XWPSState::upmergePath()
{
	return saved->path->addPath(path);
}

int  XWPSState::upathBbox(XWPSRect * pbox, bool include_moveto)
{
	XWPSFixedRect fbox;
  XWPSRect dbox;
  int code = path->getBbox(&fbox);

  if (code < 0)
		return code;
		
  if (path->lastIsMoveTo() && include_moveto) 
  {
		XWPSFixedPoint pt;

		path->currentPoint(&pt);
		if (pt.x < fbox.p.x)
	    fbox.p.x = pt.x;
		if (pt.y < fbox.p.y)
	    fbox.p.y = pt.y;
		if (pt.x > fbox.q.x)
	    fbox.q.x = pt.x;
		if (pt.y > fbox.q.y)
	    fbox.q.y = pt.y;
  }
  
  dbox.p.x = fixed2float(fbox.p.x);
  dbox.p.y = fixed2float(fbox.p.y);
  dbox.q.x = fixed2float(fbox.q.x);
  dbox.q.y = fixed2float(fbox.q.y);
  XWPSMatrix c;
  ctmOnly(&c);
  return pbox->bboxTransformInverse(&dbox, &c);
}

int XWPSState::viewClip()
{
	return commonViewClip(ps_rule_winding_number);
}

int XWPSState::viewClipPath()
{
	XWPSPath cpath;
  XWPSClipPath *pcpath = view_clip;
  int code;

  if (pcpath == 0 || pcpath->rule == 0) 
  {
		XWPSFixedRect box;
	
		code = defaultClipBox(&box);
		if (code < 0)
	    return code;
		code = cpath.addRectangle(box.p.x, box.p.y, box.q.x, box.q.y);
  } 
  else 
		code = pcpath->toPath(&cpath);
		
  if (code < 0)
		return code;
  return path->assign(&cpath, false);
}

int XWPSState::widthShowBegin(float cx, 
	                   float cy, 
	                   ulong chr,
		                 const uchar * str, 
		                 uint size,
		                 XWPSTextEnum ** ppte)
{
	XWPSTextParams text;

  text.operation = TEXT_FROM_STRING | TEXT_ADD_TO_SPACE_WIDTH |	TEXT_DO_DRAW | TEXT_RETURN_WIDTH;
  text.data.bytes = (uchar*)str, text.size = size;
  text.delta_space.x = cx;
  text.delta_space.y = cy;
  text.space.s_char = chr;
  return textBegin(&text, ppte);
}

int XWPSState::xyshowBegin(const uchar * str, 
	                uint size,
		              const float *x_widths, 
		              const float *y_widths,
		              uint widths_size, 
		              XWPSTextEnum ** ppte)
{
	XWPSTextParams text;

  text.operation = TEXT_FROM_STRING | TEXT_REPLACE_WIDTHS |	TEXT_DO_DRAW | TEXT_RETURN_WIDTH;
  text.data.bytes = (uchar*)str, text.size = size;
  text.x_widths = x_widths;
  text.y_widths = y_widths;
  text.widths_size = widths_size;
  return textBegin(&text, ppte);
}

int XWPSState::alphaBufferBits()
{
	if (!dev_color->isPure())
		return 0;
    
  if (device->isABuf())
		return 0;
	
  return device->getAlphaBits(in_cachedevice ? go_text : go_graphics);
}

int XWPSState::alphaBufferInit(long extra_x, long extra_y, int alpha_bits)
{
	XWPSDevice *dev = device;
  int log2_alpha_bits = ilog2(alpha_bits);
  XWPSFixedRect bbox;
  XWPSIntRect ibox;
  uint width, raster, band_space;
  uint height;
  XWPSLog2ScalePoint log2_scale;
  XWPSDeviceMem *mdev;

  log2_scale.x = log2_scale.y = log2_alpha_bits;
  path->getBbox(&bbox);
  ibox.p.x = fixed2int(bbox.p.x - extra_x) - 1;
  ibox.p.y = fixed2int(bbox.p.y - extra_y) - 1;
  ibox.q.x = fixed2int_ceiling(bbox.q.x + extra_x) + 1;
  ibox.q.y = fixed2int_ceiling(bbox.q.y + extra_y) + 1;
  width = (ibox.q.x - ibox.p.x) << log2_scale.x;
  raster = bitmap_raster(width);
  band_space = raster << log2_scale.y;
  height = (2000 / band_space) << log2_scale.y;
  if (height == 0)
		height = 1 << log2_scale.y;
		
  mdev = new XWPSDeviceMem;
  mdev->context_state = i_ctx_p;
  if (mdev == 0)
		return 0;
		
  mdev->makeMemABufDevice(dev, &log2_scale, alpha_bits, ibox.p.x << log2_scale.x);
  mdev->width = width;
  mdev->height = height;
  if (mdev->open() < 0) 
  {
		delete mdev;
		return 0;
  }
  setDeviceOnly(mdev);
  scalePaths(log2_scale.x, log2_scale.y, true);
  return 1;
}

void XWPSState::alphaBufferRelease(bool newpath)
{
	XWPSDeviceMem *mdev = (XWPSDeviceMem*)device;

  mdev->close();
  scalePaths(-mdev->log2_scale.x, -mdev->log2_scale.y, !(newpath));
  setDeviceOnly(mdev->target);
}

int XWPSState::assignPart(XWPSState * pfrom)
{		
	if (shared != pfrom->shared)
	{
		if (shared)
			if (!shared->decRef())
				delete shared;		
		shared = pfrom->shared;
		if (shared)
			shared->incRef();
	}
	
	ctm = pfrom->ctm;
	log_op = pfrom->log_op;
	alpha = pfrom->alpha;
	blend_mode = pfrom->blend_mode;
	opacity.alpha = pfrom->opacity.alpha;
	shape.alpha = pfrom->shape.alpha;
	text_knockout = pfrom->text_knockout;
	overprint = pfrom->overprint;
	overprint_mode = pfrom->overprint_mode;
	flatness = pfrom->flatness;
	fill_adjust = pfrom->fill_adjust;
	stroke_adjust = pfrom->stroke_adjust;
	accurate_curves = pfrom->accurate_curves;
	smoothness = pfrom->smoothness;
	get_cmap_procs = pfrom->get_cmap_procs;
	
	if (halftone != pfrom->halftone)
	{
		if (halftone)
			if (halftone->decRef() == 0)
				delete halftone;
				
		halftone = pfrom->halftone;
		if (halftone)
			halftone->incRef();
	}
	
	for (int i = 0; i < ps_color_select_count; i++)
		screen_phase[i] = pfrom->screen_phase[i];
		
	if (dev_ht != pfrom->dev_ht)
	{
		if (dev_ht)
			if (dev_ht->decRef() == 0)
				delete dev_ht;
				
		dev_ht = pfrom->dev_ht;
		if (dev_ht)
			dev_ht->incRef();
	}
	
	if (ht_cache != pfrom->ht_cache)
	{
		if (ht_cache)
			if (!ht_cache->decRef())
			{
				ht_cache->order.cache = 0;
				delete ht_cache;
			}
				
		ht_cache = pfrom->ht_cache;
		if (ht_cache)
			ht_cache->incRef();
	}
	
	if (cie_render != pfrom->cie_render)
	{
		if (cie_render)
			if (cie_render->decRef() == 0)
				delete cie_render;
				
		cie_render = pfrom->cie_render;
		if (cie_render)
			cie_render->incRef();
	}
	
	if (black_generation != pfrom->black_generation)
	{
		if (black_generation)
			if (!black_generation->decRef())
				delete black_generation;
				
		black_generation = pfrom->black_generation;
		if (black_generation)
			black_generation->incRef();
	}
	
	if (undercolor_removal != pfrom->undercolor_removal)
	{
		if (undercolor_removal)
			if (!undercolor_removal->decRef())
				delete undercolor_removal;
				
		undercolor_removal = pfrom->undercolor_removal;
		if (undercolor_removal)
			undercolor_removal->incRef();
	}
	
	set_transfer.copy(&(pfrom->set_transfer));
	effective_transfer.copy(&(pfrom->effective_transfer));
	
	if (cie_joint_caches != pfrom->cie_joint_caches)
	{
		if (cie_joint_caches)
			if (!cie_joint_caches->decRef())
				delete cie_joint_caches;
				
		cie_joint_caches = pfrom->cie_joint_caches;
		if (cie_joint_caches)
			cie_joint_caches->incRef();
	}
	
	cmap_procs = pfrom->cmap_procs;
	
	i_ctx_p = pfrom->i_ctx_p;
	
	ctm_inverse = pfrom->ctm_inverse;
	ctm_inverse_valid = pfrom->ctm_inverse_valid;
	ctm_default = pfrom->ctm_default;
	ctm_default_set = pfrom->ctm_default_set;
	
	clamp_coordinates = pfrom->clamp_coordinates;
		
	orig_cspace_index = pfrom->orig_cspace_index;
	orig_base_cspace_index = pfrom->orig_base_cspace_index;
	
	if (clip_stack != pfrom->clip_stack)
	{
		if (clip_stack)
			if (clip_stack->decRef() == 0)
				clearClipStack();
				
		clip_stack = pfrom->clip_stack;
		if (clip_stack)
			clip_stack->incRef();
	}
	
	if (font != pfrom->font)
	{
		if (font)
			if (font->decRef() == 0)
				delete font;
				
		font = pfrom->font;
		if (font)
			font->incRef();
	}
	
	if (root_font != pfrom->root_font)
	{
		if (root_font)
			if (root_font->decRef() == 0)
				delete root_font;
				
		root_font = pfrom->root_font;
		if (root_font)
			root_font->incRef();
	}
	
	char_tm = pfrom->char_tm;
	char_tm_valid = pfrom->char_tm_valid;
	in_cachedevice = pfrom->in_cachedevice;
	in_charpath = pfrom->in_charpath;
	show_gstate =	(pfrom->show_gstate == pfrom ? this : 0);
		
	level = pfrom->level;
	show_gstate = pfrom->show_gstate;
	
	if (device != pfrom->device)
	{
		if (device)
			if (device->decRef() == 0)
				delete device;
				
		device = pfrom->device;
		if (device)
			device->incRef();
	}
	
	screen_accurate_screens = pfrom->screen_accurate_screens;
	screen_min_screen_levels = pfrom->screen_accurate_screens;
	
	return 0;
}

void XWPSState::clearClipStack()
{
	if (clip_stack)
	{
		if (!clip_stack->decRef())
		{
			XWPSClipStack * n = clip_stack;
			while (n)
			{
				XWPSClipStack * t = n->next;
				if (!n->decRef())
					delete n;
					
				n = t;
			}
		}
		
		clip_stack = 0;
	}
}

int XWPSState::clone(XWPSState * pfrom, PSStateCopyReason reason)
{
	if (pfrom->client_data)
	{
		XWPSIntGState * iigst = (XWPSIntGState*)client_data;
		if (!client_data)
		{
			iigst = new XWPSIntGState;
			client_data = iigst;
		}
		
		XWPSIntGState * iigsf = (XWPSIntGState*)pfrom->client_data;		
		iigst->copy(iigsf);
	}
	
	line_params.copy(pfrom->line_params, false);
	if (pfrom->line_params.dash.pattern)
	  setDash(pfrom->line_params.dash.pattern, 
	       	  pfrom->line_params.dash.pattern_size, 
	       	  pfrom->line_params.dash.offset);
	
	pattern_cache = pfrom->pattern_cache;
	
	saved = pfrom->saved;
	assignPart(pfrom);
		
//	if (reason == copy_for_gsave)
//	{
//		path = pfrom->path;
//		clip_path = pfrom->clip_path;
//		view_clip = pfrom->view_clip;
//		effective_clip_path = pfrom->effective_clip_path;
//		pfrom->path = new XWPSPath(path);
//		pfrom->clip_path = new XWPSClipPath(clip_path);
//		if (view_clip)
//			pfrom->view_clip = new XWPSClipPath(view_clip);
//		if (pfrom->effective_clip_shared)
//			pfrom->effective_clip_path = pfrom->clip_path;
//		else
//			pfrom->effective_clip_path = new XWPSClipPath(effective_clip_path);
//	}
//	else
//	{
		path = new XWPSPath(pfrom->path);
		clip_path = new XWPSClipPath(pfrom->clip_path);
		if (pfrom->view_clip)
			view_clip = new XWPSClipPath(pfrom->view_clip);
		if (pfrom->effective_clip_shared)
		  effective_clip_path = clip_path;
	  else
		  effective_clip_path = new XWPSClipPath(pfrom->effective_clip_path);
//	}
		
	effective_clip_id = pfrom->effective_clip_id;
	effective_view_clip_id = pfrom->effective_view_clip_id;
	effective_clip_shared = pfrom->effective_clip_shared;
	
	color_space = new XWPSColorSpace(pfrom->color_space);
	color_space->i_ctx_p = i_ctx_p;
	if (device_color_spaces.named.Gray != pfrom->device_color_spaces.named.Gray)
	{
		if (device_color_spaces.named.Gray)
			if (!device_color_spaces.named.Gray->decRef())
				delete device_color_spaces.named.Gray;
				
		device_color_spaces.named.Gray = pfrom->device_color_spaces.named.Gray;
		if (device_color_spaces.named.Gray)
			device_color_spaces.named.Gray->incRef();
	}
	
	if (device_color_spaces.named.RGB != pfrom->device_color_spaces.named.RGB)
	{
		if (device_color_spaces.named.RGB)
			if (!device_color_spaces.named.RGB->decRef())
				delete device_color_spaces.named.RGB;
				
		device_color_spaces.named.RGB = pfrom->device_color_spaces.named.RGB;
		if (device_color_spaces.named.RGB)
			device_color_spaces.named.RGB->incRef();
	}
	
	if (device_color_spaces.named.CMYK != pfrom->device_color_spaces.named.CMYK)
	{
		if (device_color_spaces.named.CMYK)
			if (!device_color_spaces.named.CMYK->decRef())
				delete device_color_spaces.named.CMYK;
				
		device_color_spaces.named.RGB = pfrom->device_color_spaces.named.CMYK;
		if (device_color_spaces.named.CMYK)
			device_color_spaces.named.CMYK->incRef();
	}
	
	for (int i = 0; i < 3; i++)
	  if (device_color_spaces.indexed[i] != pfrom->device_color_spaces.indexed[i])
	  {
		  if (device_color_spaces.indexed[i])
			  if (!device_color_spaces.indexed[i]->decRef())
				  delete device_color_spaces.indexed[i];
				
		  device_color_spaces.indexed[i] = pfrom->device_color_spaces.indexed[i];
		  if (device_color_spaces.indexed[i])
			  device_color_spaces.indexed[i]->incRef();
	  }
	
	ccolor = new XWPSClientColor;	
	*ccolor = *(pfrom->ccolor);
	dev_color = new XWPSDeviceColor;
	*dev_color = *(pfrom->dev_color);
	
	device_color_spaces = pfrom->device_color_spaces;
	
	return 0;
}

int XWPSState::commonClip(int rule)
{
	int code = clip_path->intersect(path, rule, this);
  if (code < 0)
		return code;
  clip_path->rule = rule;
  return 0;
}

int XWPSState::commonViewClip(int rule)
{
	XWPSFixedRect bbox;
  XWPSClipPath rpath;
  rpath.id = i_ctx_p->nextIDS(1);
  int code;
  XWPSClipPath *pcpath = view_clip;

  if (pcpath == 0) 
  {
		pcpath = new XWPSClipPath(0);
		pcpath->id = i_ctx_p->nextIDS(1);
		view_clip = pcpath;
  }
  
  if ((code =path->getBbox(&bbox)) < 0)
		return code;
    
  code = rpath.fromRectangle(&bbox);
  if (code >= 0)
		code = rpath.intersect(path, rule, this);
  if (code < 0) 
		return code;
    
  rpath.rule = rule;
  pcpath->assign(&rpath, false);
  newPath();
  return 0;
}

int XWPSState::copy(XWPSState * pfrom, PSStateCopyReason)
{
	if (pfrom->client_data)
	{
		XWPSIntGState * iigst = (XWPSIntGState*)client_data;
		if (!client_data)
		{
			iigst = new XWPSIntGState;
			client_data = iigst;
		}
		
		XWPSIntGState * iigsf = (XWPSIntGState*)pfrom->client_data;		
		iigst->copy(iigsf);
	}
	
	line_params.copy(pfrom->line_params, false);
	if (pfrom->line_params.dash.pattern || line_params.dash.pattern)
	  setDash(pfrom->line_params.dash.pattern, 
	       	pfrom->line_params.dash.pattern_size, 
	       	pfrom->line_params.dash.offset);
	
	assignPart(pfrom);
	
	transparency_stack = pfrom->transparency_stack;
	
	if (pfrom->pattern_cache != 0)
		pattern_cache = pfrom->pattern_cache;
	
	if (!effective_clip_shared)
	{
		if (effective_clip_path)
			delete effective_clip_path;
	}
	
	effective_clip_path = 0;
		
	if (!path)
		path = new XWPSPath(0);
	if (pfrom->path)
	  path->assign(pfrom->path);
	
	if (!clip_path)
		clip_path = new XWPSClipPath(0);
	if (pfrom->clip_path)
	  clip_path->assign(pfrom->clip_path);
	  
	if ((view_clip == clip_path) || (view_clip == effective_clip_path))
		view_clip = 0;
	
	if (pfrom->view_clip)
	{
		if (view_clip)
			delete view_clip;
		view_clip = new XWPSClipPath(pfrom->view_clip);
	}
	else
	{
		if (view_clip)
			delete view_clip;
		view_clip = 0;
	}
	
	effective_clip_shared = pfrom->effective_clip_shared;
	if (effective_clip_shared)
	{
		if (pfrom->effective_clip_path == pfrom->view_clip)
			effective_clip_path = view_clip;
		else
			effective_clip_path = clip_path;
	}
	else
	{
		effective_clip_path  = new XWPSClipPath(0);
		if (pfrom->effective_clip_path)
		  effective_clip_path->assign(pfrom->effective_clip_path);
	}
	
	effective_clip_id = pfrom->effective_clip_id;
	effective_view_clip_id = pfrom->effective_view_clip_id;
	
	if (color_space != pfrom->color_space)
	{
		if (color_space)
			if (!color_space->decRef())
				delete color_space;
				
		color_space = pfrom->color_space;
		if (color_space)
			color_space->incRef();
	}
	
	if (device_color_spaces.named.Gray != pfrom->device_color_spaces.named.Gray)
	{
		if (device_color_spaces.named.Gray)
			if (!device_color_spaces.named.Gray->decRef())
				delete device_color_spaces.named.Gray;
				
		device_color_spaces.named.Gray = pfrom->device_color_spaces.named.Gray;
		if (device_color_spaces.named.Gray)
			device_color_spaces.named.Gray->incRef();
	}
	
	if (device_color_spaces.named.RGB != pfrom->device_color_spaces.named.RGB)
	{
		if (device_color_spaces.named.RGB)
			if (!device_color_spaces.named.RGB->decRef())
				delete device_color_spaces.named.RGB;
				
		device_color_spaces.named.RGB = pfrom->device_color_spaces.named.RGB;
		if (device_color_spaces.named.RGB)
			device_color_spaces.named.RGB->incRef();
	}
	
	if (device_color_spaces.named.CMYK != pfrom->device_color_spaces.named.CMYK)
	{
		if (device_color_spaces.named.CMYK)
			if (!device_color_spaces.named.CMYK->decRef())
				delete device_color_spaces.named.CMYK;
				
		device_color_spaces.named.RGB = pfrom->device_color_spaces.named.CMYK;
		if (device_color_spaces.named.CMYK)
			device_color_spaces.named.CMYK->incRef();
	}
	
	for (int i = 0; i < 3; i++)
	  if (device_color_spaces.indexed[i] != pfrom->device_color_spaces.indexed[i])
	  {
		  if (device_color_spaces.indexed[i])
			  if (!device_color_spaces.indexed[i]->decRef())
				  delete device_color_spaces.indexed[i];
				
		  device_color_spaces.indexed[i] = pfrom->device_color_spaces.indexed[i];
		  if (device_color_spaces.indexed[i])
			  device_color_spaces.indexed[i]->incRef();
	  }
	
	if (ccolor != pfrom->ccolor)
	{
		if (ccolor)
			if (!ccolor->decRef())
				delete ccolor;
				
		ccolor = pfrom->ccolor;
		if (ccolor)
			ccolor->incRef();
	}
	
	if (!dev_color)
	  dev_color = new XWPSDeviceColor;
	*dev_color = *(pfrom->dev_color);
	
	show_gstate =	(pfrom->show_gstate == pfrom ? this : 0);
		
	return 0;
}

int XWPSState::fillWithRule(int rule)
{
	int code;
  if (in_charpath)
		code = show_gstate->path->addCharPath(path, in_charpath);
  else 
  {
		int abits, acode;

		setDevColor();
		code = colorLoad();
		if (code < 0)
	    return code;
		abits = alphaBufferBits();
		if (abits > 1) 
		{
	    acode = alphaBufferInit(fill_adjust.x, fill_adjust.y, abits);
	    if (acode < 0)
				return acode;
		} 
		else
	    acode = 0;
		code = fillPath(path, dev_color, rule, fill_adjust.x, fill_adjust.y);
		if (acode > 0)
	    alphaBufferRelease(code >= 0);
		if (code >= 0)
	    newPath();

  }
  return code;
}

void XWPSState::freeContents()
{
	if (path)
	{
		delete path;
		path = 0;
	}
	
	if ((view_clip == clip_path) || (view_clip == effective_clip_path))
		view_clip = 0;
		
	if (effective_clip_shared)
			effective_clip_path = 0;
	
	if (clip_path)
	{
		delete clip_path;
		clip_path = 0;
	}
	
	if (view_clip)
	{
		delete view_clip;
		view_clip = 0;
	}
	
	if (clip_stack)
		clearClipStack();
	
	if (effective_clip_path)
	{
		delete effective_clip_path;
		effective_clip_path = 0;
	}
		
	if (color_space)
	{
		if (!color_space->decRef())
			delete color_space;
		color_space = 0;
	}
	
	if (ccolor)
	{
		if (!ccolor->decRef())
			delete ccolor;
		ccolor = 0;
	}
	
	if (dev_color)
	{
		delete dev_color;
		dev_color = 0;
	}
	
	if (font)
	{
		if (font->decRef() == 0)
			delete font;
		font = 0;
	}
	
	if (root_font)
	{
		if (root_font->decRef() == 0)
			delete root_font;
		root_font = 0;
	}
	
	if (device)
	{
		if (device->decRef() == 0)
			delete device;
		device = 0;
	}
}

void XWPSState::popTransparencyStack()
{
	XWPSTransparencyState *pts = transparency_stack;
	if (!pts)
		return ;
		
  XWPSTransparencyState *saved = pts->saved;
  delete pts;
  transparency_stack = saved;
}

int XWPSState::pushTransparencyStack(PSTransparencyStateType type)
{
	XWPSTransparencyState *pts = new XWPSTransparencyState;
  pts->saved = transparency_stack;
  pts->type = type;
  transparency_stack = pts;
  return 0;
}

void XWPSState::scaleDashPattern(float scale)
{
  for (int i = 0; i < line_params.dash.pattern_size; ++i)
		line_params.dash.pattern[i] *= scale;
    
  line_params.dash.offset *= scale;
  line_params.dash.pattern_length *= scale;
  line_params.dash.init_dist_left *= scale;
  if (line_params.dot_length_absolute)
		line_params.dot_length *= scale;
}

int XWPSState::scalePaths(int log2_scale_x, int log2_scale_y, bool do_path)
{
	const XWPSPathSegments *seg_clip =	(clip_path->path_valid ? clip_path->path.segments : 0);
  const XWPSClipRectList *list_clip = clip_path->rect_list;
  const XWPSPathSegments *seg_view_clip;
  const XWPSClipRectList *list_view_clip;
  const XWPSPathSegments *seg_effective_clip =	(effective_clip_path->path_valid ? effective_clip_path->path.segments : 0);
  const XWPSClipRectList *list_effective_clip = effective_clip_path->rect_list;

  clip_path->scaleExp2Shared(log2_scale_x, log2_scale_y, false, false);
  if (i_ctx_p)
  	clip_path->id = i_ctx_p->nextIDS(1);
  if (view_clip != 0 && view_clip != clip_path) 
  {
		seg_view_clip =  (view_clip->path_valid ? view_clip->path.segments : 0);
		list_view_clip = view_clip->rect_list;
		view_clip->scaleExp2Shared(log2_scale_x, log2_scale_y, list_view_clip == list_clip, seg_view_clip && seg_view_clip == seg_clip);
		if (i_ctx_p)
  		view_clip->id = i_ctx_p->nextIDS(1);
  } 
  else
  {
		seg_view_clip = 0, list_view_clip = 0;
	}
    
  if (effective_clip_path != clip_path &&	effective_clip_path != view_clip)
  {
		effective_clip_path->scaleExp2Shared(log2_scale_x, log2_scale_y, 
		                               list_effective_clip == list_clip || list_effective_clip == list_view_clip,
				   												 seg_effective_clip && (seg_effective_clip == seg_clip ||
				                           seg_effective_clip == seg_view_clip));
		if (i_ctx_p)
  		effective_clip_path->id = i_ctx_p->nextIDS(1);
	}
  if (do_path) 
  {
		const XWPSPathSegments *seg_path = path->segments;

		path->scaleExp2Shared(log2_scale_x, log2_scale_y, 
		               seg_path == seg_clip || seg_path == seg_view_clip ||  seg_path == seg_effective_clip);
  }
  return 0;
}

void XWPSState::setDefault()
{
	ctm_default_set = false;
	ctm_inverse_valid = false;
	effective_clip_shared = false;
	char_tm_valid = false;
	font = 0;
	root_font = 0;
	in_cachedevice = CACHE_DEVICE_NONE;
	in_charpath = cpm_show;
	show_gstate = 0;
	level = 0;
	screen_accurate_screens = false;
	screen_min_screen_levels = 1;
}

XWPSStatus::XWPSStatus()
{
	pgs = 0;
	data = 0;
	width = height = raster = 0;
	dx = dy = 0;
	count = 0;
	outline_scale = 4;
	step = 1;
}

int XWPSStatus::addDxDy(int dxA, int dyA, int countA)
{
	if (countA != 0) 
	{
		if (dxA == dx && dyA == dy)
	    count += countA;
		else 
		{
	    if (count != 0) 
	    {
				int code = pgs->rlineTo(dx * count / (float)outline_scale, dy * count / (float)outline_scale);

				if (code < 0)
		    	return code;
	    }
	    dx = dxA, dy = dyA;
	    count = countA;
		}
  }
  return 0;
}

int XWPSStatus::getPixel(int x, int y)
{
	if (x < 0 || x >= width || y < 0 || y >= height)
		return 0;
  return (data[y * raster + (x >> 3)] >> (~x & 7)) & 1;
}

int XWPSStatus::traceFrom(int x0, int y0, int detect)
{
	int x = x0, y = y0;
  int dxA = -1, dyA = 0;
  int part = 0;
  int code;
  
  if (!detect)
  {
  	part = (getPixel(x + 1, y - 1) ?	outline_scale - step : step);
		code = pgs->moveTo(x + 1 - part / (float)outline_scale, (float)y);
		if (code < 0)
	    return code;
  }
  
  while (1)
  {
  	int tx = dxA - dyA, ty = dyA + dxA;
  	if (getPixel(x + tx, y + ty))
  	{
  		if (!detect)
  		{
  			if (dx == ty && dy == -tx)
  			{
#define half_scale (outline_scale / 2 - step)
		    	count -= half_scale;
		    	code = addDxDy(tx, ty, outline_scale / 2);
		    	if (code < 0)
		    		return code;
#undef half_scale
  			}
  			else
  			{
  				code = addDxDy(dxA, dyA, step - part);
  				if (code < 0)
		    		return code;
		    	code = addDxDy(tx, ty, outline_scale - step);
		    	if (code < 0)
		    		return code;
  			}
  			part = outline_scale - step;
  		}
  		x += tx, y += ty;
	    dxA = -dyA, dyA += tx;
  	}
  	else if (!getPixel(x + dxA, y + dyA))
  	{
  		if (!detect)
  		{
  			code = addDxDy(dxA, dyA, outline_scale - step - part);
  			if (code < 0)
		    		return code;
				code = addDxDy(ty, -tx, step);
				if (code < 0)
		    		return code;
				part = step;
  		}
  		dxA = dyA, dyA -= ty;
  	}
  	else
  	{
  		if (!detect)
  		{
  			code = addDxDy(dxA, dyA, outline_scale);
  			if (code < 0)
		    		return code;
  		}
  		x += dxA, y += dyA;
  	}
  	
  	if (dxA == -step && dyA == 0 && !(tx == -step && ty == -step))
  	{
  		if (x == x0 && y == y0)
				return 0;
	    if (detect && (y > y0 || (y == y0 && x > x0)))
				return 1;
  	}
  }
}
