/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include "XWPSError.h"
#include "XWPSContextState.h"

#define MIN_INTVAL min_long
#define MAX_INTVAL max_long
#define MAX_HALF_INTVAL ((1L << (sizeof(long) * 4 - 1)) - 1)

int XWPSContextState::zabs()
{
	XWPSRef * op = op_stack.getCurrentTop();

  switch (op->type()) 
  {
		default:
	    return op->checkTypeFailed();
	    
		case XWPSRef::Real:
	    if (op->value.realval >= 0)
				return 0;
	    break;
	    
		case XWPSRef::Integer:
	    if (op->value.intval >= 0)
				return 0;
	    break;
  }
  return zneg();
}

int XWPSContextState::zadd()
{
	XWPSRef * op = op_stack.getCurrentTop();
	int code = zopAdd(op);
  if (code == 0) 
		pop(1);
  return code;
}

int XWPSContextState::zbitAdd()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = op->checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
  	
  code = op[-1].checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
  
  op[-1].value.intval += op->value.intval;
  pop(1);
  return 0;
}

int XWPSContextState::zceiling()
{
	XWPSRef * op = op_stack.getCurrentTop();

  switch (op->type()) 
  {
		default:
	    return op->checkTypeFailed();
	    
		case XWPSRef::Real:
	    op->value.realval = ceil(op->value.realval);
	    
		case XWPSRef::Integer:;
  }
  return 0;
}

int XWPSContextState::zdiv()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * op1 = op - 1;
  switch (op->type())
  {
  	default:
			return op->checkTypeFailed();
			
		case XWPSRef::Real:
	    if (op->value.realval == 0)
				return (int)(XWPSError::UndefinedResult);
			switch (op1->type())
			{
				default:
					return op->checkTypeFailed();
					
				case XWPSRef::Real:
				  op1->value.realval /= op->value.realval;
		   		break;
		   		
				case XWPSRef::Integer:
		    	op1->makeReal((float)op1->value.intval / op->value.realval);
			}
			break;
			
		case XWPSRef::Integer:
			if (op->value.intval == 0)
				return (int)(XWPSError::UndefinedResult);
			switch (op1->type())
			{
				default:
					return op->checkTypeFailed();
					
				case XWPSRef::Real:
		    	op1->value.realval /= (double)op->value.intval;
		    	break;
		    	
				case XWPSRef::Integer:
		    	op1->makeReal((float)op1->value.intval / (float)op->value.intval);
			}
  }
  pop(1);
  return 0;
}

int XWPSContextState::zfloor()
{
	XWPSRef * op = op_stack.getCurrentTop();

  switch (op->type()) 
  {
		default:
	    return op->checkTypeFailed();
	    
		case XWPSRef::Real:
	    op->value.realval = floor(op->value.realval);
		case XWPSRef::Integer:
			;
  }
  return 0;
}

int XWPSContextState::zidiv()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = op->checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
  	
  code = op[-1].checkType(XWPSRef::Integer);
  if (op->value.intval == 0)
		return (int)(XWPSError::UndefinedResult);
  
  if ((op[-1].value.intval /= op->value.intval) == MIN_INTVAL && op->value.intval == -1) 
		return (int)(XWPSError::RangeCheck);
  pop(1);
  return 0;
}

int XWPSContextState::zmod()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = op->checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
  	
  code = op[-1].checkType(XWPSRef::Integer);
  if (op->value.intval == 0)
		return (int)(XWPSError::UndefinedResult);
  op[-1].value.intval %= op->value.intval;
  pop(1);
  return 0;
}

int XWPSContextState::zmul()
{
	XWPSRef * op = op_stack.getCurrentTop();
	switch (op->type())
	{
		default:
			return op->checkTypeFailed();
			
		case XWPSRef::Real:
			switch (op[-1].type())
			{
				default:
					return op[-1].checkTypeFailed();
					
				case XWPSRef::Real:
				  op[-1].value.realval *= op->value.realval;
	    		break;
	    		
				case XWPSRef::Integer:
	    	op[-1].makeReal((float)op[-1].value.intval * op->value.realval);
			}
			break;
			
		case XWPSRef::Integer:
			switch (op[-1].type())
			{
				default:
					return op[-1].checkTypeFailed();
					
				case XWPSRef::Real:
	    		op[-1].value.realval *= (float)op->value.intval;
	    		break;
	    		
				case XWPSRef::Integer: 
					{
	    			long int1 = op[-1].value.intval;
	    			long int2 = op->value.intval;
	    			long abs1 = (int1 >= 0 ? int1 : -int1);
	    			long abs2 = (int2 >= 0 ? int2 : -int2);
	    			float fprod;

	    			if ((abs1 > MAX_HALF_INTVAL || abs2 > MAX_HALF_INTVAL) && 
									abs1 != 0 &&
									abs2 > MAX_INTVAL / abs1 && 
									(fprod = (float)int1 * int2, (int1 * int2 != MIN_INTVAL || fprod != (float)MIN_INTVAL)))
							op[-1].makeReal(fprod);
	    			else
							op[-1].value.intval = int1 * int2;
				}
			}
	}
	pop(1);
  return 0;
}

int XWPSContextState::zneg()
{
	XWPSRef * op = op_stack.getCurrentTop();

  switch (op->type()) 
  {
		default:
	    return op->checkTypeFailed();
	    
		case XWPSRef::Real:
	    op->value.realval = -op->value.realval;
	    break;
	    
		case XWPSRef::Integer:
	    if (op->value.intval == MIN_INTVAL)
				op->makeReal(-(float)MIN_INTVAL);
	    else
				op->value.intval = -op->value.intval;
  }
  return 0;
}

int XWPSContextState::zopAdd(XWPSRef * op)
{
	switch (op->type())
	{
		default:
			return op->checkTypeFailed();
			
		case XWPSRef::Real:
			switch (op[-1].type())
			{
				default:
					return op[-1].checkTypeFailed();
					
				case XWPSRef::Real:
					op[-1].value.realval += op->value.realval;
	    		break;
	    		
	    	case XWPSRef::Integer:
	    		op[-1].makeReal((float)op[-1].value.intval + op->value.realval);
			}
			break;
			
		case XWPSRef::Integer:
			switch (op[-1].type())
			{
				default:
					return op[-1].checkTypeFailed();
					
				case XWPSRef::Real:
	    		op[-1].value.realval += (float)op->value.intval;
	    		break;
	    		
	    	case XWPSRef::Integer:
	    		{
	    			long int2 = op->value.intval;

	    			if (((op[-1].value.intval += int2) ^ int2) < 0 &&
								((op[-1].value.intval - int2) ^ int2) >= 0) 
						{
							op[-1].makeReal((float)(op[-1].value.intval - int2) + int2);
	    			}
					}
			}
	}
	return 0;
}

int XWPSContextState::zopSub(XWPSRef * op)
{
	switch (op->type())
	{
		default:
			return op->checkTypeFailed();
			
		case XWPSRef::Real:
			switch (op[-1].type())
			{
				default:
					return op[-1].checkTypeFailed();
					
				case XWPSRef::Real:
	    		op[-1].value.realval -= op->value.realval;	    		
	    		break;
	    		
				case XWPSRef::Integer:
	    		op[-1].makeReal((double)op[-1].value.intval - op->value.realval);
			}
			break;
			
		case XWPSRef::Integer:
			switch (op[-1].type())
			{
				default:
					return op[-1].checkTypeFailed();
					
				case XWPSRef::Real:
	    		op[-1].value.realval -= (double)op->value.intval;
	    		break;
	    		
	    	case XWPSRef::Integer:
	    		{
	    			long int1 = op[-1].value.intval;
	    			if ((int1 ^ (op[-1].value.intval = int1 - op->value.intval)) < 0 &&
								(int1 ^ op->value.intval) < 0	) 
						{
							op[-1].makeReal((float)int1 - op->value.intval);
	    			}
	    		}
			}
	}
	
	return 0;
}

int XWPSContextState::zround()
{
	XWPSRef * op = op_stack.getCurrentTop();

  switch (op->type()) 
  {
		default:
	    return op->checkTypeFailed();
	    
		case XWPSRef::Real:
	    op->value.realval = floor(op->value.realval + 0.5);
		case XWPSRef::Integer:
			;
  }
  return 0;
}

int XWPSContextState::zsub()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code = zopSub(op);

  if (code == 0) 
		pop(1);
  return code;
}

int XWPSContextState::ztruncate()
{
	XWPSRef * op = op_stack.getCurrentTop();

  switch (op->type()) 
  {
		default:
	    return op->checkTypeFailed();
	    
		case XWPSRef::Real:
	    op->value.realval =	(op->value.realval < 0.0 ? ceil(op->value.realval) :	 floor(op->value.realval));
		case XWPSRef::Integer:
			;
  }
  return 0;
}
