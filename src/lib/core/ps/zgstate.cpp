/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "XWPSError.h"
#include "XWPSState.h"
#include "XWPSContextState.h"

int XWPSContextState::zcurrentAccurateCurves()
{
	return zcurrentBool(&XWPSState::currentAccurateCurves);
}

int XWPSContextState::zcurrentBool(bool (XWPSState::*current_proc)())
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->makeBool((pgs->*current_proc)());
  return 0;
}

int XWPSContextState::zcurrentCurveJoin()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->makeInt(pgs->currentCurveJoin());
  return 0;
}

int XWPSContextState::zcurrentDash()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op,2);
  if (code < 0)
		return code;
  XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
  op[-1].assign(&iigs->dash_pattern);
  op->makeReal(pgs->currentDashOffset());
  return 0;
}

int XWPSContextState::zcurrentDashAdapt()
{
	return zcurrentBool(&XWPSState::currentDashAdapt);
}

int XWPSContextState::zcurrentDotLength()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op, 2);
  if (code < 0)
		return code;
  op[-1].makeReal(pgs->currentDotLength());
  op->makeBool(pgs->currentDotLengthAbsolute());
  return 0;
}

int XWPSContextState::zcurrentFillAdjust2()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSPoint adjust;

  int code = push(&op, 2);
  if (code < 0)
		return code;
  pgs->currentFillAdjust(&adjust);
  op[-1].makeReal(adjust.x);
  op->makeReal(adjust.y);
  return 0;
}

int XWPSContextState::zcurrentFlat()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->makeReal(pgs->currentFlat());
  return 0;
}

int XWPSContextState::zcurrentLimitClamp()
{
	return zcurrentBool(&XWPSState::currentLimitClamp);
}

int XWPSContextState::zcurrentLineCap()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->makeInt((int)pgs->currentLineCap());
  return 0;
}

int XWPSContextState::zcurrentLineJoin()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->makeInt((int)pgs->currentLineJoin());
  return 0;
}

int XWPSContextState::zcurrentLineWidth()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->makeReal(pgs->currentLineWidth());
  return 0;
}

int XWPSContextState::zcurrentMiterLimit()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->makeReal(pgs->currentMiterLimit());
  return 0;
}

int XWPSContextState::zdotOrientation()
{
	return pgs->dotOrientation();
}

int XWPSContextState::zinitGraphics()
{
	int code = pgs->initGraphics();

  if (code >= 0)
  {
  	XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
		iigs->colorspace.array.makeNull();
	}
  return code;
}

int XWPSContextState::zgrestore()
{
	return pgs->restore();
}

int XWPSContextState::zgrestoreAll()
{
	return pgs->restoreAll();
}

int XWPSContextState::zgsave()
{
	return pgs->save();
}

int XWPSContextState::zsetAccurateCurves()
{
	return zsetBool(&XWPSState::setAccurateCurves);
}

int XWPSContextState::zsetBool(void (XWPSState::*set_proc)(bool))
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = op->checkType(XWPSRef::Boolean);
  if (code < 0)
  	return code;
  	
  (pgs->*set_proc)(op->value.boolval);
  pop(1);
  return 0;
}

int XWPSContextState::zsetCurveJoin()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code;

  code = op->checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
  	
  if (op->value.intval < -1 || op->value.intval > max_int)
		return (int)(XWPSError::RangeCheck);
  
  pgs->setCurveJoin((int)op->value.intval);
  pop(1);
  return 0;
}

int XWPSContextState::zsetDash()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * op1 = op - 1;
  double offset;
  int code = op->realParam(&offset);
  uint i, n;
  float *pattern = 0;

  if (code < 0)
		return op->checkTypeFailed();
  if (!op1->isArray())
		return op1->checkTypeFailed();
  n = op1->size();
  if (n > 0)
    pattern = new float[n];
  for (i = 0, code = 0; i < n && code >= 0; ++i) 
 	{
		XWPSRef element;

		op1->arrayGet(this, (long)i, &element);
		code = element.floatParam(&pattern[i]);
  }
  if (code >= 0)
		code = pgs->setDash(pattern, n, offset);
    
  delete [] pattern;
  if (code < 0)
		return code;
    
  XWPSIntGState * iigs = (XWPSIntGState*)pgs->client_data;
  iigs->dash_pattern.assign(op1);
  pop(2);
  return code;
}

int XWPSContextState::zsetDashAdapt()
{
	return zsetBool(&XWPSState::setDashAdapt);
}

int XWPSContextState::zsetDotLength()
{
	XWPSRef * op = op_stack.getCurrentTop();
  double length;
  int code = op[-1].realParam(&length);

  if (code < 0)
		return code;
  code = op->checkType(XWPSRef::Boolean);
  if (code < 0)
		return code;
		
  code = pgs->setDotLength(length, op->value.boolval);
  if (code < 0)
		return code;
  pop(2);
  return 0;
}

int XWPSContextState::zsetDotOrientation()
{
	return pgs->setDotOrientation();
}

int XWPSContextState::zsetFillAdjust2()
{
	XWPSRef * op = op_stack.getCurrentTop();
  double adjust[2];
  int code = numParams(op, 2, adjust);

  if (code < 0)
		return code;
    
  pgs->setFillAdjust(adjust[0], adjust[1]);
  pop(2);
  return 0;
}

int XWPSContextState::zsetFlat()
{
	return zsetReal(&XWPSState::setFlat);
}

int XWPSContextState::zsetLimitClamp()
{
	return zsetBool(&XWPSState::setLimitClamp);
}

int XWPSContextState::zsetLineCap()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int param;
  int code = op->intParam(max_int, &param);

  if (code < 0)
		return code;
	pgs->setLineCap((PSLineCap) param);
  pop(1);
  return 0;
}

int XWPSContextState::zsetLineJoin()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int param;
  int code = op->intParam(max_int, &param);

  if (code < 0)
		return code;
	pgs->setLineJoin((PSLineJoin) param);
  pop(1);
  return 0;
}

int XWPSContextState::zsetLineWidth()
{
	XWPSRef * op = op_stack.getCurrentTop();
  double width;
  int code = op->realParam(&width);

  if (code < 0)
		return op->checkTypeFailed();
  pgs->setLineWidth(fabs(width));
	pop(1);
  return 0;
}

int XWPSContextState::zsetMiterLimit()
{
	return zsetReal(&XWPSState::setMiterLimit);
}

int XWPSContextState::zsetReal(void (XWPSState::*set_proc)(float))
{
	XWPSRef * op = op_stack.getCurrentTop();
  double param;
  int code = op->realParam(&param);

  if (code < 0)
		return op->checkTypeFailed();
		
  (pgs->*set_proc)((float)param);
	pop(1);
  return 0;
}
