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
#include "XWPSContextState.h"

#define MAX_CHARS 50

int XWPSContextState::darc(double (*afunc)(double))
{
	XWPSRef * op = op_stack.getCurrentTop();
	double num;
	int code = doubleParamsResult(op, 1, &num);
	if (code < 0)
		return code;
	return doubleResult(1, (*afunc)(num) * radians_to_degrees);
}

int XWPSContextState::dcompare(int mask)
{
	XWPSRef * op = op_stack.getCurrentTop();
  double num[2];
  int code = doubleParams(op, 2, num);

  if (code < 0)
		return code;
    
  op[-1].makeBool((mask & (num[0] < num[1] ? 1 : num[0] > num[1] ? 4 : 2)) != 0);
  pop(1);
  return 0;
}

int XWPSContextState::dlog(double (*lfunc)(double))
{
	XWPSRef * op = op_stack.getCurrentTop();
	double num;
	int code = doubleParamsResult(op, 1, &num);
	if (code < 0)
		return code;
		
	if (num <= 0.0)
		return (int)(XWPSError::RangeCheck);
  return doubleResult(1, (*lfunc)(num));
}

int XWPSContextState::doubleParams(XWPSRef * op, int count, double *pval)
{
	pval += count;
  while (--count >= 0)
  {
		switch (op->type()) 
		{
	    case XWPSRef::Real:
				*--pval = op->value.realval;
				break;
				
	    case XWPSRef::Integer:
				*--pval = op->value.intval;
				break;
				
	    case XWPSRef::String:
				if (!op->hasAttr(PS_A_READ) || op->size() != sizeof(double))
		      return (int)(XWPSError::TypeCheck);
				--pval;
				memcpy(pval, op->getBytes(), sizeof(double));
				break;;
				
	    case XWPSRef::Invalid:
	    	return (int)(XWPSError::StackUnderflow);
				
	    default:
		    return (int)(XWPSError::TypeCheck);
		}
		op--;
  }
  return 0;
}

int XWPSContextState::doubleParamsResult(XWPSRef * op, int count, double *pval)
{
	int code = op->checkWriteType(XWPSRef::String);
	if (code < 0)
		return code;
		
  if (op->size() != sizeof(double))
		return (int)(XWPSError::TypeCheck);
    
  return doubleParams(op - 1, count, pval);
}

int XWPSContextState::doubleResult(int count, double result)
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * op1 = op - count;

  op1->assign(op);
  memcpy(op1->getBytes(), &result, sizeof(double));
  pop(count);
  return 0;
}

int XWPSContextState::doubleUnary(double (*func)(double))
{
	XWPSRef * op = op_stack.getCurrentTop();
	double num;
	int code = doubleParamsResult(op, 1, &num);
	if (code < 0)
		return code;
  return doubleResult(1, (*func)(num));
}

int XWPSContextState::zcvd()
{
	XWPSRef * op = op_stack.getCurrentTop();
	double num;
	int code = doubleParamsResult(op, 1, &num);
	if (code < 0)
		return code;
		
	return doubleResult(1, num);
}

int XWPSContextState::zcvsd()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code = doubleParamsResult(op, 0, NULL);
  double num;
  char buf[MAX_CHARS + 2];
  char *str = buf;
  uint len;
  char end;

  if (code < 0)
		return code;
    
  code = op[-1].checkReadType(XWPSRef::String);
  if (code < 0)
		return code;
		
  len = op[-1].size();
  if (len > MAX_CHARS)
		return (int)(XWPSError::LimitCheck);
  memcpy(str, op[-1].getBytes(), len);
  while (len > 0 && isspace(*str))
		++str, --len;
  while (len > 0 && isspace(str[len - 1]))
		--len;
  str[len] = 0;
  if (strspn(str, "0123456789+-.dDeE") != len)
		return (int)(XWPSError::SyntaxError);
  strcat(str, "$");
  if (sscanf(str, "%lf%c", &num, &end) != 2 || end != '$')
		return (int)(XWPSError::SyntaxError);
    
  return doubleResult(1, num);
}

int XWPSContextState::zdabs()
{
	return doubleUnary(fabs);
}

int XWPSContextState::zdadd()
{
	XWPSRef * op = op_stack.getCurrentTop();
	double num[2];
	int code = doubleParamsResult(op, 2, num);
	if (code < 0)
		return code;
	return doubleResult(2, num[0] + num[1]);
}

int XWPSContextState::zdarccos()
{
	return darc(acos);
}

int XWPSContextState::zdarcsin()
{
	return darc(asin);
}

int XWPSContextState::zdatan()
{
	double result;
	XWPSRef * op = op_stack.getCurrentTop();
	double num[2];
	int code = doubleParamsResult(op, 2, num);
	if (code < 0)
		return code;
		
	if (num[0] == 0) 
	{
		if (num[1] == 0)
			return (int)(XWPSError::UndefinedResult);
		result = (num[1] < 0 ? 180 : 0);
  } 
  else 
  {
		result = atan2(num[0], num[1]) * radians_to_degrees;
		if (result < 0)
	    result += 360;
  }
  return doubleResult(2, result);
}

int XWPSContextState::zdceiling()
{
	return doubleUnary(ceil);
}

int XWPSContextState::zdcos()
{
	return doubleUnary(ps_cos_degrees);
}

int XWPSContextState::zdcvi()
{
	XWPSRef * op = op_stack.getCurrentTop();
#define alt_min_long (-1L << (arch_sizeof_long * 8 - 1))
#define alt_max_long (~(alt_min_long))
    
  static const double min_int_real = (alt_min_long * 1.0 - 1);
  static const double max_int_real = (alt_max_long * 1.0 + 1);
  
  double num;
  int code = doubleParams(op, 1, &num);

  if (code < 0)
		return code;

  if (num < min_int_real || num > max_int_real)
		return (int)(XWPSError::RangeCheck);
  op->makeInt((long)num);
  return 0;
}

int XWPSContextState::zdcvr()
{
	XWPSRef * op = op_stack.getCurrentTop();
#define b30 (0x40000000L * 1.0)
#define max_mag (0xffffff * b30 * b30 * b30 * 0x4000)
    
  static const float min_real = -max_mag;
  static const float max_real = max_mag;
  
#undef b30
#undef max_mag
  
  double num;
  int code = doubleParams(op, 1, &num);

  if (code < 0)
		return code;
    
  if (num < min_real || num > max_real)
		return (int)(XWPSError::RangeCheck);
	
  op->makeReal((float)num);
  return 0;
}

int XWPSContextState::zdcvs()
{
	XWPSRef * op = op_stack.getCurrentTop();
  double num;
  int code = doubleParams(op - 1, 1, &num);
  char str[MAX_CHARS + 1];
  int len;

  if (code < 0)
		return code;
    
  code = op->checkWriteType(XWPSRef::String);
  {
		double scanned;

		sprintf(str, "%g", num);
		sscanf(str, "%lf", &scanned);
		if (scanned != num)
	    sprintf(str, "%.16g", num);
  }
  len = strlen(str);
  if (len > op->size())
		return (int)(XWPSError::RangeCheck);
  memcpy(op->getBytes(), str, len);
  op[-1].assign(op);
  op[-1].setSize(len);
  pop(1);
  return 0;
}

int XWPSContextState::zddiv()
{
	XWPSRef * op = op_stack.getCurrentTop();
	double num[2];
	int code = doubleParamsResult(op, 2, num);
	if (code < 0)
		return code;
		
	if (num[1] == 0.0)
		return (int)(XWPSError::UndefinedResult);
   
  return doubleResult(2, num[0] / num[1]);
}

int XWPSContextState::zdeq()
{
	return dcompare(2);
}

int XWPSContextState::zdexp()
{
	double ipart;
	XWPSRef * op = op_stack.getCurrentTop();
	double num[2];
	int code = doubleParamsResult(op, 2, num);
	if (code < 0)
		return code;
		
	if (num[0] == 0.0 && num[1] == 0.0)
		return (int)(XWPSError::UndefinedResult);
	
  if (num[0] < 0.0 && modf(num[1], &ipart) != 0.0)
		return (int)(XWPSError::UndefinedResult);
  return doubleResult(2, pow(num[0], num[1]));
}

int XWPSContextState::zdfloor()
{
	return doubleUnary(floor);
}

int XWPSContextState::zdge()
{
	return dcompare(6);
}

int XWPSContextState::zdgt()
{
	return dcompare(4);
}

int XWPSContextState::zdle()
{
	return dcompare(3);
}

int XWPSContextState::zdln()
{
	return dlog(log);
}

int XWPSContextState::zdlog()
{
	return dlog(log10);
}

int XWPSContextState::zdlt()
{
	return dcompare(1);
}

int XWPSContextState::zdmul()
{
	XWPSRef * op = op_stack.getCurrentTop();
	double num[2];
	int code = doubleParamsResult(op, 2, num);
	if (code < 0)
		return code;
		
	return doubleResult(2, num[0] * num[1]);
}

int XWPSContextState::zdne()
{
	return dcompare(5);
}

int XWPSContextState::zdneg()
{
	XWPSRef * op = op_stack.getCurrentTop();
	double num;
	int code = doubleParamsResult(op, 1, &num);
	if (code < 0)
		return code;
	return doubleResult(1, -num);
}

int XWPSContextState::zdround()
{
	XWPSRef * op = op_stack.getCurrentTop();
	double num;
	int code = doubleParamsResult(op, 1, &num);
	if (code < 0)
		return code;
		
	return doubleResult(1, floor(num + 0.5));
}

int XWPSContextState::zdsin()
{
	return doubleUnary(ps_sin_degrees);
}

int XWPSContextState::zdsqrt()
{
	XWPSRef * op = op_stack.getCurrentTop();
	double num;
	int code = doubleParamsResult(op, 1, &num);
	if (code < 0)
		return code;
		
	if (num < 0.0)
		return (int)(XWPSError::RangeCheck);
  return doubleResult(1, sqrt(num));
}

int XWPSContextState::zdsub()
{
	XWPSRef * op = op_stack.getCurrentTop();
	double num[2];
	int code = doubleParamsResult(op, 2, num);
	if (code < 0)
		return code;
		
	return doubleResult(2, num[0] - num[1]);
}

int XWPSContextState::zdtruncate()
{
	XWPSRef * op = op_stack.getCurrentTop();
	double num;
	int code = doubleParamsResult(op, 1, &num);
	if (code < 0)
		return code;
		
	return doubleResult(1, (num < 0 ? ceil(num) : floor(num)));
}
