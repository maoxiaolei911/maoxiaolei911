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

int XWPSContextState::zarccos()
{
	XWPSRef * op = op_stack.getCurrentTop();
  double num, result;
  int code = op->realParam(&num);

  if (code < 0)
		return code;
    
  result = acos(num) * radians_to_degrees;
  op->makeReal(result);
  return 0;
}

int XWPSContextState::zarcsin()
{
	XWPSRef * op = op_stack.getCurrentTop();
  double num, result;
  int code = op->realParam(&num);

  if (code < 0)
		return code;
  result = asin(num) * radians_to_degrees;
  op->makeReal(result);
  return 0;
}

int XWPSContextState::zatan()
{
	XWPSRef * op = op_stack.getCurrentTop();
  double args[2];
  double result;
  int code = numParams(op, 2, args);

  if (code < 0)
		return code;
    
  code = ps_atan2_degrees(args[0], args[1], &result);
  if (code < 0)
		return code;
  op[-1].makeReal(result);
  pop(1);
  return 0;
}

int XWPSContextState::zcos()
{
	XWPSRef * op = op_stack.getCurrentTop();
  double angle;
  int code = op->realParam(&angle);

  if (code < 0)
		return code;
    
  op->makeReal(ps_cos_degrees(angle));
  return 0;
}

int XWPSContextState::zexp()
{
	XWPSRef * op = op_stack.getCurrentTop();
  double args[2];
  double result;
  double ipart;
  int code = numParams(op, 2, args);

  if (code < 0)
		return code;
  if (args[0] == 0.0 && args[1] == 0.0)
		return (int)(XWPSError::UndefinedResult);
  
  if (args[0] < 0.0 && modf(args[1], &ipart) != 0.0)
		return (int)(XWPSError::UndefinedResult);
	
  result = pow(args[0], args[1]);
  op[-1].makeReal(result);
  pop(1);
  return 0;
}

int XWPSContextState::zln()
{
	XWPSRef * op = op_stack.getCurrentTop();
  double num;
  int code = op->realParam(&num);

  if (code < 0)
		return code;
    
  if (num <= 0.0)
		return (int)(XWPSError::RangeCheck);
  op->makeReal(log(num));
  return 0;
}

int XWPSContextState::zlog()
{
	XWPSRef * op = op_stack.getCurrentTop();
  double num;
  int code = op->realParam(&num);

  if (code < 0)
		return code;
    
  if (num <= 0.0)
		return (int)(XWPSError::RangeCheck);
  op->makeReal(log10(num));
  return 0;
}

int XWPSContextState::zrand()
{
	XWPSRef * op = op_stack.getCurrentTop();
	
#define A 16807
#define M 0x7fffffff
#define Q 127773		/* M / A */
#define R 2836			/* M % A */
    
  rand_state = A * (rand_state % Q) - R * (rand_state / Q);
  if (rand_state <= 0)
		rand_state += M;
#undef A
#undef M
#undef Q
#undef R
    
  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->makeInt(rand_state);
  return 0;
}

int XWPSContextState::zrrand()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->makeInt(rand_state);
  return 0;
}

int XWPSContextState::zsin()
{
	XWPSRef * op = op_stack.getCurrentTop();
  double angle;
  int code = op->realParam(&angle);

  if (code < 0)
		return code;
  op->makeReal(ps_sin_degrees(angle));
  return 0;
}

int XWPSContextState::zsqrt()
{
	XWPSRef * op = op_stack.getCurrentTop();
  double num;
  int code = op->realParam(&num);

  if (code < 0)
		return code;
  
  if (num < 0.0)
  {
		return (int)(XWPSError::RangeCheck);
	}
  op->makeReal(sqrt(num));
  return 0;
}

int XWPSContextState::zsrand()
{
	XWPSRef * op = op_stack.getCurrentTop();
  long s;

  int code = op->checkType(XWPSRef::Integer);
  if (code < 0)
		return code;
		
  s = op->value.intval;
#if arch_sizeof_long > 4
  s = (int)s;
#endif

  if (s < 1)
		s = -(s % 0x7ffffffe) + 1;
  else if (s > 0x7ffffffe)
		s = 0x7ffffffe;
  rand_state = s;
  pop(1);
  return 0;
}

int XWPSContextState::commonTransform(int (XWPSState::*ptproc)(float, float, XWPSPoint *),
											int (XWPSPoint::*matproc)(double, double, XWPSMatrix *))
{
	XWPSRef * op = op_stack.getCurrentTop();
  double opxy[2];
  XWPSPoint pt;
  int code;

  switch (op->type()) 
  {
		case XWPSRef::Real:
	    opxy[1] = op->value.realval;
	    break;
	    
		case XWPSRef::Integer:
	    opxy[1] = op->value.intval;
	    break;
	    
		case XWPSRef::Array:	
		case XWPSRef::ShortArray:
		case XWPSRef::MixedArray: 
			{
	    	XWPSMatrix mat;
	    	XWPSMatrix *pmat = &mat;

	    	if ((code = op->readMatrix(this, pmat)) < 0 ||
						(code = numParams(op - 1, 2, opxy)) < 0 ||
						(code = (pt.*matproc)(opxy[0], opxy[1], pmat)) < 0) 
				{	
					checkOp(op, 3);
					return code;
	    	}
	    	op--;
	    	pop(1);
	    	goto out;
			}
		default:
	    return op->checkTypeFailed();
  }
  switch (op[-1].type()) 
  {
		case XWPSRef::Real:
	    opxy[0] = (op - 1)->value.realval;
	    break;
	    
		case XWPSRef::Integer:
	    opxy[0] = (op - 1)->value.intval;
	    break;
	    
		default:
	    return op[-1].checkTypeFailed();
  }
  if ((code = (pgs->*ptproc)(opxy[0], opxy[1], &pt)) < 0)
		return code;
out:
  op[-1].makeReal(pt.x);
  op->makeReal(pt.y);
  return 0;
}

int XWPSContextState::zconcat()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSMatrix mat;
  int code = op->readMatrix(this, &mat);

  if (code < 0)
		return code;
  code = pgs->concat(&mat);
  if (code < 0)
		return code;
  pop(1);
  return 0;
}

int XWPSContextState::zconcatMatrix()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSMatrix m1, m2, mp;
  int code;

  if ((code = op[-2].readMatrix(this, &m1)) < 0 ||
			(code = op[-1].readMatrix(this, &m2)) < 0 ||
			(code = mp.matrixMultiply(&m1, &m2)) < 0 ||
			(code = op->writeMatrixIn(&mp)) < 0)
		return code;
  op[-2].assign(op);
  pop(2);
  return code;
}

int XWPSContextState::zcurrentMatrix()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSMatrix mat;
  int code = pgs->currentMatrix(&mat);

  if (code < 0)
		return code;
  push(&op, 6);
  
  XWPSRef * p = op - 5;
  int count = 6;
  double *pval = &mat.xx;
  for (; count--; p++, pval++)
		p->makeReal(*pval);
  return 0;
}

int XWPSContextState::zdefaultMatrix()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSMatrix mat;

  pgs->defaultMatrix(&mat);
  return op->writeMatrixIn(&mat);
}

int XWPSContextState::zdtransform()
{
	return commonTransform(&XWPSState::dtransform, &XWPSPoint::distanceTransform);
}

int XWPSContextState::zidtransform()
{
	return commonTransform(&XWPSState::idtransform, &XWPSPoint::distanceTransformInverse);
}

int XWPSContextState::zinitMatrix()
{
	return pgs->initMatrix();
}

int XWPSContextState::zinvertMatrix()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSMatrix m;
  int code;

  if ((code = op[-1].readMatrix(this, &m)) < 0 ||
			(code = m.matrixInvert(&m)) < 0 ||
			(code = op->writeMatrixIn(&m)) < 0)
		return code;
  op[-1].assign(op);
  pop(1);
  return code;
}

int XWPSContextState::zitransform()
{
	return commonTransform(&XWPSState::itransform, &XWPSPoint::transformInverse);
}

int XWPSContextState::zrotate()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code;
  double ang;

  if ((code = op->realParam(&ang)) >= 0) 
  {
		code = pgs->rotate(ang);
		if (code < 0)
	    return code;
  } 
  else 
  {
		XWPSMatrix mat;
		checkOp(op,1);
		if ((code = numParams(op - 1, 1, &ang)) < 0 ||
	    	(code = mat.makeRotation(ang)) < 0 ||
	    	(code = op->writeMatrixIn(&mat)) < 0) 
	  {	
	    checkOp(op, 2);
	    return code;
		}
		op[-1].assign(op);
  }
  pop(1);
  return code;
}

int XWPSContextState::zscale()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code;
  double scale[2];

  if ((code = numParams(op, 2, scale)) >= 0) 
  {
		code = pgs->scale(scale[0], scale[1]);
		if (code < 0)
	    return code;
  } 
  else 
  {	
		XWPSMatrix mat;
		checkOp(op, 2);
		if ((code = numParams(op - 1, 2, scale)) < 0 ||
	    	(code = mat.makeScaling(scale[0], scale[1])) < 0 ||
	    	(code = op->writeMatrixIn(&mat)) < 0 ) 
	  {
	    checkOp(op, 3);
	    return code;
		}
		op[-2].assign(op);
  }
  pop(2);
  return code;
}

int XWPSContextState::zsetDefaultMatrix()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code;

  if (op->hasType(XWPSRef::Null))
		code = pgs->setDefaultMatrix(NULL);
  else 
  {
		XWPSMatrix mat;

		code = op->readMatrix(this, &mat);
		if (code < 0)
	    return code;
		code = pgs->setDefaultMatrix(&mat);
  }
  if (code < 0)
		return code;
  pop(1);
  return 0;
}

int XWPSContextState::zsetMatrix()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSMatrix mat;
  int code = op->floatMatrix(6, &mat);

  if (code < 0)
		return code;
 	if ((code = pgs->setMatrix(&mat)) < 0)
		return code;
  pop(6);
  return 0;
}

int XWPSContextState::ztranslate()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code;
  double trans[2];

  if ((code = numParams(op, 2, trans)) >= 0) 
  {
		code = pgs->translate(trans[0], trans[1]);
		if (code < 0)
	    return code;
  } 
  else 
  {	
		XWPSMatrix mat;
		checkOp(op, 2);
		if ((code = numParams(op - 1, 2, trans)) < 0 ||
	    	(code = mat.makeTranslation(trans[0], trans[1])) < 0 ||
	    	(code = op->writeMatrixIn(&mat)) < 0 ) 
	  {	
	    checkOp(op, 3);
	    return code;
		}
		op[-2].assign(op);
  }
  pop(2);
  return code;
}

int XWPSContextState::ztransform()
{
	return commonTransform(&XWPSState::transform, &XWPSPoint::transform);
}
