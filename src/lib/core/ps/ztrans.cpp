/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include "XWPSError.h"
#include "XWPSFunction.h"
#include "XWPSState.h"
#include "XWPSContextState.h"

static const char *const blend_mode_names[] = {
    "Compatible", "Normal", "Multiply", "Screen", "Difference",
  "Darken", "Lighten", "ColorDodge", "ColorBurn", "Exclusion",
  "HardLight", "Overlay", "SoftLight", "Luminosity", "Hue",
  "Saturation", "Color", 0
};

int XWPSContextState::currentFloatValue(float (XWPSState::*current_value)())
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->makeReal((pgs->*current_value)());
  return 0;
}

int XWPSContextState::enumParam(XWPSRef *pnref, const char *const names[])
{
	const char *const *p;
  XWPSRef nsref;

  nameStringRef(pnref, &nsref);
  for (p = names; *p; ++p)
		if (nsref.size() == strlen(*p) && !memcmp(*p, nsref.getBytes(), nsref.size()))
	    return p - names;
	    
  return (int)(XWPSError::RangeCheck);
}

int XWPSContextState::maskDictParam(XWPSRef * op, 
	                  XWPSImageParams *pip_data, 
	                  const char *dict_name,
		                int num_components, 
		                XWPSImage3xMask *pixm)
{
	XWPSRef *pMaskDict;
  XWPSImageParams ip_mask;
  int ignored;
  int code, mcode;

  if (op->dictFindString(this, dict_name, &pMaskDict) <= 0)
		return 1;
  if ((mcode = code = dataImageParams(pMaskDict, &pixm->MaskDict, &ip_mask, false, 1, 12)) < 0 ||
			(code = pMaskDict->dictIntParam(this, "ImageType", 1, 1, 0, &ignored)) < 0 ||
			(code = pMaskDict->dictIntParam(this, "InterleaveType", 1, 3, -1, &pixm->InterleaveType)) < 0 ||
			(code = op->dictFloatsParam(this, "Matte", num_components, pixm->Matte, NULL)) < 0)
		return code;
    
  pixm->has_Matte = code > 0;
  if ((pip_data->MultipleDataSources && pixm->InterleaveType != 3) ||
				ip_mask.MultipleDataSources || mcode != (pixm->InterleaveType != 3))
	{
  	return (int)(XWPSError::RangeCheck);
	}
	
  if (pixm->InterleaveType == 3) 
  {
  	memmoveRef(&pip_data->DataSource[1], &pip_data->DataSource[0], 6);
  	
		pip_data->DataSource[0].assign(&ip_mask.DataSource[0]);
  }
  return 0;
}

int XWPSContextState::maskOp(int (XWPSState::*mask_proc)(PSTransparencyChannelSelector))
{
	int csel;
	XWPSRef * op = op_stack.getCurrentTop();
  int code = op->intParam(1, &csel);

  if (code < 0)
		return code;
  code = (pgs->*mask_proc)((PSTransparencyChannelSelector)csel);
  if (code >= 0)
		pop(1);
  return code;
}

int XWPSContextState::rectParam(XWPSRect *prect, XWPSRef * op)
{
	double coords[4];
  int code = numParams(op, 4, coords);

  if (code < 0)
		return code;
  prect->p.x = coords[0], prect->p.y = coords[1];
  prect->q.x = coords[2], prect->q.y = coords[3];
  return 0;
}

int XWPSContextState::setFloatValue(int (XWPSState::*set_value)(float))
{
	XWPSRef * op = op_stack.getCurrentTop();
  double value;
  int code;

  if (op->realParam(&value) < 0)
		return op->checkTypeFailed();
		
  if ((code = (pgs->*set_value)(value)) < 0)
		return code;
  pop(1);
  return 0;
}

int XWPSContextState::zbeginTransparencyGroup()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * dop = op - 4;
  XWPSTransparencyGroupParams params;
  XWPSRect bbox;
  int code;

  code = dop->checkType(XWPSRef::Dictionary);
  if (code < 0)
  	return code;
  	
  code = dop->checkDictRead();
  if (code < 0)
  	return code;
  	
  if ((code = dop->dictBoolParam(this, "Isolated", false, &params.Isolated)) < 0 ||
			(code = dop->dictBoolParam(this, "Knockout", false, &params.Knockout)) < 0)
		return code;
  code = rectParam(&bbox, op);
  if (code < 0)
		return code;
  params.ColorSpace = pgs->currentColorSpace();
  code = pgs->beginTransparencyGroup(&params, &bbox);
  if (code < 0)
		return code;
  pop(5);
  return code;
}

int XWPSContextState::zbeginTransparencyMask()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * dop = op - 4;
  XWPSTransparencyMaskParams params;
  XWPSRef *pparam;
  XWPSRect bbox;
  int num_components = pgs->currentColorSpace()->numComponents();
  int code;
  
  static const char *const subtype_names[] = {"Alpha", "Luminosity", 0};

  code = dop->checkType(XWPSRef::Dictionary);
  if (code < 0)
		return code;
		
  code = dop->checkDictRead();
  if (code < 0)
		return code;
		
  if (dop->dictFindString(this, "Subtype", &pparam) <= 0)
  	return (int)(XWPSError::RangeCheck);
	
  if ((code = enumParam(pparam, subtype_names)) < 0)
		return code;
    
  if ((code = dop->dictFloatsParam(this, "Background", num_components, params.Background, NULL)) < 0)
		return code;
  else if (code > 0)
		params.has_Background = true;
    
  if (dop->dictFindString(this, "TransferFunction", &pparam) >0) 
  {
		XWPSFunction *pfn = refFunction(pparam);

		if (pfn == 0 || pfn->params->m != 1 || pfn->params->n != 1)
  		return (int)(XWPSError::RangeCheck);
		params.TransferFunction = &XWPSTransparencyMaskParams::tfusingFunction;
		params.TransferFunction_data = pfn;
		pfn->incRef();
  }
  code = rectParam(&bbox, op);
  if (code < 0)
		return code;
  code = pgs->beginTransparencyMask(&params, &bbox);
  if (code < 0)
		return code;
  pop(5);
  return code;
}

int XWPSContextState::zcurrentBlendMode()
{
	XWPSRef * op = op_stack.getCurrentTop();
  const char *mode_name = blend_mode_names[pgs->currentBlendMode()];
  XWPSRef nref;
  int code = nameEnterString(mode_name, &nref);

  if (code < 0)
		return code;
  code = push(&op, 1);
  if (code < 0)
		return code;
  op->assign(&nref);
  return 0;
}

int XWPSContextState::zcurrentOpacityAlpha()
{
	return currentFloatValue(&XWPSState::currentOpacityAlpha);
}

int XWPSContextState::zcurrentRasterOp()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->makeInt((int)pgs->currentRasterOp());
  return 0;
}

int XWPSContextState::zcurrentShapeAlpha()
{
	return currentFloatValue(&XWPSState::currentShapeAlpha);
}

int XWPSContextState::zcurrentSourceTransparent()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->makeBool(pgs->currentSourceTransparent());
  return 0;
}

int XWPSContextState::zcurrentTextKnockout()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->makeBool(pgs->currentTextKnockout());
  return 0;
}

int XWPSContextState::zcurrentTextureTransparent()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->makeBool(pgs->currentTextureTransparent());
  return 0;
}

int XWPSContextState::zdiscardTransparencyGroup()
{
	if (pgs->currentTransparencyType() != TRANSPARENCY_STATE_Group)
  	return (int)(XWPSError::RangeCheck);
  return pgs->discardTransparencyLayer();
}

int XWPSContextState::zdiscardTransparencyMask()
{
	if (pgs->currentTransparencyType() != TRANSPARENCY_STATE_Mask)
  	return (int)(XWPSError::RangeCheck);
  return pgs->discardTransparencyLayer();
}

int XWPSContextState::zendTransparencyGroup()
{
	return pgs->endTransparencyGroup();
}

int XWPSContextState::zendTransparencyMask()
{
	return maskOp(&XWPSState::endTransparencyMask);
}

int XWPSContextState::zimage3x()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSImage3X image;
  XWPSRef *pDataDict;
  XWPSImageParams ip_data;
  int num_components = pgs->currentColorSpace()->numComponents();
  int ignored;
  int code;

  code = op->checkType(XWPSRef::Dictionary);
  if (code < 0)
  	return code;
  code = op->checkDictRead();
  if (code < 0)
  	return code;
  	
  image.image3xInit(NULL);
  if (op->dictFindString(this, "DataDict", &pDataDict) <= 0)
  	return (int)(XWPSError::RangeCheck);
    
  if ((code = pixelImageParams(pDataDict, &image, &ip_data, 12)) < 0 ||
			(code = pDataDict->dictIntParam(this, "ImageType", 1, 1, 0, &ignored)) < 0)
		return code;
		
  if ((code = maskDictParam(op, &ip_data, "ShapeMaskDict", num_components,&image.Shape)) < 0 ||
			(code = maskDictParam(op, &ip_data, "OpacityMaskDict", num_components,	&image.Opacity)) < 0)
		return code;
  return zimageSetup(&image,	&ip_data.DataSource[0],	image.CombineWithColor, 1);
}

int XWPSContextState::zinitTransparencyMask()
{
	return maskOp(&XWPSState::initTransparencyMask);
}

int XWPSContextState::zsetBlendMode()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code;

  code = op->checkType(XWPSRef::Name);
  if (code < 0)
  	return code;
  	
  if ((code = enumParam(op, blend_mode_names)) < 0 ||
		(code = pgs->setBlendMode((PSBlendMode)code)) < 0)
		return code;
  pop(1);
  return 0;
}

int XWPSContextState::zsetOpacityAlpha()
{
	return setFloatValue(&XWPSState::setOpacityAlpha);
}

int XWPSContextState::zsetRasterOp()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int param;
  int code = op->intParam(0xff, &param);

  if (code < 0)
		return code;
  pgs->setRasterOp((PSRop3)param);
  pop(1);
  return 0;
}

int XWPSContextState::zsetShapeAlpha()
{
	return setFloatValue(&XWPSState::setShapeAlpha);
}

int XWPSContextState::zsetSourceTransparent()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = op->checkType(XWPSRef::Boolean);
  if (code < 0)
  	return code;
    
  pgs->setSourceTransparent(op->value.boolval);
  pop(1);
  return 0;
}

int XWPSContextState::zsetTextKnockout()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = op->checkType(XWPSRef::Boolean);
  if (code < 0)
  	return code;
  	
  pgs->setTextKnockout(op->value.boolval);
  pop(1);
  return 0;
}

int XWPSContextState::zsetTextureTransparent()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = op->checkType(XWPSRef::Boolean);
  if (code < 0)
  	return code;
  	
  pgs->setTextureTransparent(op->value.boolval);
  pop(1);
  return 0;
}
