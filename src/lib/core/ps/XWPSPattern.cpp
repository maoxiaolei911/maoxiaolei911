/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include "XWPSError.h"
#include "XWPSDataSource.h"
#include "XWPSColor.h"
#include "XWPSState.h"
#include "XWPSFunction.h"
#include "XWPSShading.h"
#include "XWPSPattern.h"

XWPSPatternInstance::XWPSPatternInstance()
	:XWPSStruct()
{
	saved = 0;
}

XWPSPatternInstance::~XWPSPatternInstance()
{
	if (saved)
	{
		delete saved;
		saved = 0;
	}
}

int XWPSPatternInstance::getLength()
{
	return sizeof(XWPSPatternInstance);
}

const char * XWPSPatternInstance::getTypeName()
{
	return "patterninstance";
}

XWPSPattern * XWPSPatternInstance::getPattern()
{
	return (this->*(type.procs.get_pattern_.get_pattern))();
}

int XWPSPatternInstance::remapColor(XWPSImagerState * pis, 
	               XWPSClientColor * pc, 
	               XWPSColorSpace * pcs,
	               XWPSDeviceColor * pdc, 
	               XWPSDevice * dev,
		             PSColorSelect select)
{
	return (pis->*(type.procs.remap_color))(pc, pcs, pdc, dev, select);
}

bool XWPSPatternInstance::usesBaseSpace()
{
	return getPattern()->usesBaseSpace();
}

XWPSPattern::XWPSPattern()
	:XWPSStruct()
{
	PatternType = 0;
	client_data = 0;
}

XWPSPattern::~XWPSPattern()
{
	if (client_data)
	{
		XWPSStruct * s = (XWPSStruct*)client_data;
		if (!s->decRef())
			delete s;
		client_data = 0;
	}
}

int XWPSPattern::getLength()
{
	return sizeof(XWPSPattern);
}

const char * XWPSPattern::getTypeName()
{
	return "pattern";
}

int XWPSPattern::makePattern(XWPSClientColor *pcc,
			 										const XWPSMatrix * pmat, 
			 										XWPSState * pgs)
{
	return (this->*(type.procs.make_pattern_.make_pattern))(pcc, pmat, pgs);
}

int XWPSPattern::makePatternCommon(XWPSClientColor *pcc,
			 									const XWPSMatrix * pmat, 
			 									XWPSState * pgs,
			 									XWPSPatternInstance * pinst)
{
	pinst->type.procs.uses_base_space_.uses_base_space = type.procs.uses_base_space_.uses_base_space;
	pinst->type.procs.make_pattern_.make_pattern = type.procs.make_pattern_.make_pattern;
	pinst->type.procs.get_pattern_.get_pattern = type.procs.get_pattern_.get_pattern;
	pinst->type.procs.remap_color = type.procs.remap_color;
	
	pinst->saved = new XWPSState(pgs, copy_for_gstate);
	pinst->saved->concat(pmat);
	pcc->pattern = pinst;
  return 0;
}

bool XWPSPattern::usesBaseSpace()
{
	return (this->*(type.procs.uses_base_space_.uses_base_space))();
}

XWPSPattern1::XWPSPattern1()
	:XWPSPattern()
{
	type.procs.uses_base_space_.uses_base_space1 = &XWPSPattern1::usesBaseSpacePattern1;
	type.procs.make_pattern_.make_pattern1 = &XWPSPattern1::makePattern1;
	type.procs.get_pattern_.get_pattern1 = &XWPSPattern1Instance::getPattern1;
	type.procs.remap_color = type.procs.remap_color;
	
	PatternType = type.PatternType;
	PaintType = 0;
	TilingType = 0;
	XStep = 0;
	YStep = 0;
}

int XWPSPattern1::getLength()
{
	return sizeof(XWPSPattern1);
}

const char * XWPSPattern1::getTypeName()
{
	return "pattern1";
}

int XWPSPattern1::makePattern1(XWPSClientColor *pcc,
			 										const XWPSMatrix * pmat, 
			 										XWPSState * pgs)
{
	XWPSPattern1Instance *pinst;
  XWPSState *saved;
  XWPSRect bbox;
  XWPSFixedRect cbox;
  int code;
  
  pinst = new XWPSPattern1Instance;
  makePatternCommon(pcc, pmat, pgs, pinst);
  saved = pinst->saved;
  
  switch (PaintType) 
  {
		case 1:		/* colored */
	    saved->setLogicalOp(lop_default);
	    break;
	    
		case 2:		/* uncolored */
	    saved->setDeviceColor1();
	    break;
	    
		default:
	    code = (int)(XWPSError::RangeCheck);
	    goto fsaved;
  }
  
  pinst->templat = *this;
  client_data = 0;
  code = pinst->computeInstMatrix(&bbox);
  if (code < 0)
		goto fsaved;
		
	{
		float bbw = bbox.q.x - bbox.p.x;
		float bbh = bbox.q.y - bbox.p.y;
		
		pinst->size.x = (int)(bbw + 0.8);	
		pinst->size.y = (int)(bbh + 0.8);

		if (pinst->size.x == 0 || pinst->size.y == 0) 
		{
	    pinst->step_matrix.reset();
	    bbox.p.x = bbox.p.y = bbox.q.x = bbox.q.y = 0;
		} 
		else 
		{
	    if (fabs(pinst->step_matrix.xx * pinst->step_matrix.yy - pinst->step_matrix.xy * pinst->step_matrix.yx) < 1.0e-6) 
	    {
				code = (int)(XWPSError::RangeCheck);
				goto fsaved;
	    }
	    if (pinst->step_matrix.xy == 0 && pinst->step_matrix.yx == 0 &&
					fabs(fabs(pinst->step_matrix.xx) - bbw) < 0.5 &&
					fabs(fabs(pinst->step_matrix.yy) - bbh) < 0.5) 
			{
				saved->scale(fabs(pinst->size.x / pinst->step_matrix.xx), 	fabs(pinst->size.y / pinst->step_matrix.yy));
				code = pinst->computeInstMatrix(&bbox);
				if (code < 0)
		    	goto fsaved;
	    }
		}
  }
  if ((code = pinst->bbox.bboxTransformInverse(&bbox, &(pinst->step_matrix))) < 0)
		goto fsaved;
  pinst->is_simple = (fabs(pinst->step_matrix.xx) == pinst->size.x && pinst->step_matrix.xy == 0 &&
		      pinst->step_matrix.yx == 0 && fabs(pinst->step_matrix.yy) == pinst->size.y);
  pinst->uses_mask = true;
  saved->translateToFixed(float2fixed(pinst->step_matrix.tx - bbox.p.x),  float2fixed(pinst->step_matrix.ty - bbox.p.y));
  pinst->step_matrix.tx = bbox.p.x;
  pinst->step_matrix.ty = bbox.p.y;
  
  cbox.p.x = fixed_0;
  cbox.p.y = fixed_0;
  cbox.q.x = int2fixed(pinst->size.x);
  cbox.q.y = int2fixed(pinst->size.y);
  code = saved->clipToRectangle(&cbox);
  if (code < 0)
		goto fsaved;
  return 0;
    
fsaved:
   return code;
}

bool XWPSPattern1::usesBaseSpacePattern1()
{
	return PaintType == 2;
}

XWPSPattern2::XWPSPattern2()
	:XWPSPattern()
{
	type.procs.uses_base_space_.uses_base_space2 = &XWPSPattern2::usesBaseSpacePattern2;
	type.procs.make_pattern_.make_pattern2 = &XWPSPattern2::makePattern2;
	type.procs.get_pattern_.get_pattern2 = &XWPSPattern2Instance::getPattern2;
	type.procs.remap_color = &XWPSImagerState::remapColorPattern2;
	PatternType = type.PatternType;
	Shading = 0;
}

XWPSPattern2::~XWPSPattern2()
{
	if (Shading)
	{
		if (Shading->decRef() == 0)
			delete Shading;
		Shading = 0;
	}
}

int XWPSPattern2::getLength()
{
	return sizeof(XWPSPattern2);
}

const char * XWPSPattern2::getTypeName()
{
	return "pattern2";
}

int XWPSPattern2::makePattern2(XWPSClientColor *pcc,
			 										const XWPSMatrix * pmat, 
			 										XWPSState * pgs)
{
	XWPSPattern2Instance * pinst = new XWPSPattern2Instance;
	makePatternCommon(pcc, pmat, pgs, pinst);
	pinst->templat = *this;
	if (Shading)
		Shading->incRef();
	client_data = 0;
	return 0;
}

XWPSPattern1Instance::XWPSPattern1Instance()
	:XWPSPatternInstance()
{
	is_simple = false;
	uses_mask = false;
	id = 0;
}

int XWPSPattern1Instance::computeInstMatrix(XWPSRect * pbbox)
{
	double xx = templat.XStep * saved->ctm.xx;
  double xy = templat.XStep * saved->ctm.xy;
  double yx = templat.YStep * saved->ctm.yx;
  double yy = templat.YStep * saved->ctm.yy;

	if (xx == 0 || yy == 0)
	{
		double temp;

		temp = xx, xx = yx, yx = temp;
		temp = xy, xy = yy, yy = temp;
  }
  if (xx < 0)
		xx = -xx, xy = -xy;
  if (yy < 0)
		yx = -yx, yy = -yy;
		
  step_matrix.xx = xx;
  step_matrix.xy = xy;
  step_matrix.yx = yx;
  step_matrix.yy = yy;
  step_matrix.tx = saved->ctm.tx;
  step_matrix.ty = saved->ctm.ty;
  XWPSMatrix ctm;
  saved->ctmOnly(&ctm);
  return templat.BBox.bboxTransform(&ctm, pbbox);
}

int XWPSPattern1Instance::getLength()
{
	return sizeof(XWPSPattern1Instance);
}

const char * XWPSPattern1Instance::getTypeName()
{
	return "pattern1instance";
}

XWPSPattern * XWPSPattern1Instance::getPattern1()
{
	return &templat;
}

XWPSPattern2Instance::XWPSPattern2Instance()
	:XWPSPatternInstance()
{
}

int XWPSPattern2Instance::getLength()
{
	return sizeof(XWPSPattern2Instance);
}

const char * XWPSPattern2Instance::getTypeName()
{
	return "pattern2instance";
}

XWPSPattern * XWPSPattern2Instance::getPattern2()
{
	return &templat;
}
