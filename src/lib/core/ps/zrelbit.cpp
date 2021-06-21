/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWPSContextState.h"

bool XWPSContextState::objIdentEq(XWPSRef * pref1, XWPSRef * pref2)
{
	if (pref1->type() != pref2->type())
		return false;
		
  if (pref1->hasType(XWPSRef::String))
		return (pref1->getBytes() == pref2->getBytes() &&	pref1->size() == pref2->size());
  return pref1->objEq(this, pref2);
}

int XWPSContextState::objLe(XWPSRef * op1, XWPSRef * op)
{
	switch (op1->type()) 
	{
		case XWPSRef::Integer:
	    switch (op->type()) 
	    {
				case XWPSRef::Integer:
		    	return (op1->value.intval <= op->value.intval);
		    	
				case XWPSRef::Real:
		    	return ((double)op1->value.intval <= op->value.realval);
		    	
				default:
		    	return op->checkTypeFailed();
	    }
	    
		case XWPSRef::Real:
	    switch (op->type()) 
	    {
				case XWPSRef::Real:
		    	return (op1->value.realval <= op->value.realval);
		    	
				case XWPSRef::Integer:
		    	return (op1->value.realval <= (double)op->value.intval);
		    	
				default:
		    	return op->checkTypeFailed();
	    }
	    
		case XWPSRef::String:
	    op1->checkRead();
	    op->checkReadType(XWPSRef::String);
	    return (bytes_compare(op1->getBytes(), op1->size(),  op->getBytes(), op->size()) <= 0);
	    
		default:
	    return op1->checkTypeFailed();
  }
}

int XWPSContextState::zand()
{
	XWPSRef * op = op_stack.getCurrentTop();

  switch (op->type()) 
  {
		case XWPSRef::Boolean:
	    op[-1].checkType(XWPSRef::Boolean);
	    op[-1].value.boolval &= op->value.boolval;
	    break;
	    
		case XWPSRef::Integer:
	    op[-1].checkType(XWPSRef::Integer);
	    op[-1].value.intval &= op->value.intval;
	    break;
	    
		default:
	    return op->checkTypeFailed();
  }
  pop(1);
  return 0;
}

int XWPSContextState::zbitShift()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int shift;

  int code = op->checkType(XWPSRef::Integer);
  if (code < 0)
		return code;
		
  op[-1].checkType(XWPSRef::Integer);
  if (code < 0)
		return code;
		
#define MAX_SHIFT (arch_sizeof_long * 8 - 1)
  if (op->value.intval < -MAX_SHIFT || op->value.intval > MAX_SHIFT)
		op[-1].value.intval = 0;
#undef MAX_SHIFT
    
  else if ((shift = op->value.intval) < 0)
		op[-1].value.intval = ((ulong)(op[-1].value.intval)) >> -shift;
  else
		op[-1].value.intval <<= shift;
  pop(1);
  return 0;
}

int XWPSContextState::zeq()
{
	XWPSRef * op = op_stack.getCurrentTop();
	int code = 0;
	switch (op[-1].type())
	{
		case XWPSRef::String:
			code = op[-1].checkRead();
			break;
			
		default:
			code = checkOp(op, 2);
			break; 
	}
	if (code < 0)
		return code;
		
	switch (op->type())
	{
		case XWPSRef::String:
			code = op->checkRead();
			break;
			
		default:
			break; 
	}
	if (code < 0)
		return code;
	op[-1].makeBool((op[-1].objEq(this, op) ? 1 : 0));
  pop(1);
  return 0;
}

int XWPSContextState::zge()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code = objLe(op, op - 1);

  if (code < 0)
		return code;
    
  op[-1].makeBool(code);
  pop(1);
  return 0;
}

int XWPSContextState::zgt()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code = objLe(op - 1, op);

  if (code < 0)
		return code;
    
  op[-1].makeBool(code ^ 1);
  pop(1);
  return 0;
}

int XWPSContextState::zidenteq()
{
	XWPSRef * op = op_stack.getCurrentTop();
	switch (op[-1].type())
	{
		case XWPSRef::String:
			op[-1].checkRead();
			break;
			
		default:
			checkOp(op, 2);
			break; 
	}
	
	switch (op->type())
	{
		case XWPSRef::String:
			op->checkRead();
			break;
			
		default:
			break; 
	}
	
	op[-1].makeBool(objIdentEq(op - 1, op) ? 1 : 0);
  pop(1);
  return 0;
}

int XWPSContextState::zidentne()
{
	int code = zidenteq();

	XWPSRef * op = op_stack.getCurrentTop();
  if (!code)
		op->value.boolval ^= 1;
  return code;
}

int XWPSContextState::zle()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code = objLe(op - 1, op);

  if (code < 0)
		return code;
    
  op[-1].makeBool(code);
  pop(1);
  return 0;
}

int XWPSContextState::zlt()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code = objLe(op, op - 1);

  if (code < 0)
		return code;
  op[-1].makeBool(code ^ 1);
  pop(1);
  return 0;
}

int XWPSContextState::zmax()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code = objLe(op - 1, op);

  if (code < 0)
		return code;
  if (code) 
  {
		op[-1].assign(op);
  }
  pop(1);
  return 0;
}

int XWPSContextState::zmin()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code = objLe(op - 1, op);

  if (code < 0)
		return code;
  if (!code) 
		op[-1].assign(op);
  pop(1);
  return 0;
}

int XWPSContextState::zne()
{
	int code = zeq();
	XWPSRef * op = op_stack.getCurrentTop();

  if (!code)
		op->value.boolval ^= 1;
  return code;
}

int XWPSContextState::znot()
{
	XWPSRef * op = op_stack.getCurrentTop();

  switch (op->type()) 
  {
		case XWPSRef::Boolean:
	    op->value.boolval = !op->value.boolval;
	    break;
	    
		case XWPSRef::Integer:
	    op->value.intval = ~op->value.intval;
	    break;
	    
		default:
	    return op->checkTypeFailed();
  }
  return 0;
}

int XWPSContextState::zor()
{
	XWPSRef * op = op_stack.getCurrentTop();

  switch (op->type()) 
  {
		case XWPSRef::Boolean:
	    op[-1].checkType(XWPSRef::Boolean);
	    op[-1].value.boolval |= op->value.boolval;
	    break;
	    
		case XWPSRef::Integer:
	    op[-1].checkType(XWPSRef::Integer);
	    op[-1].value.intval |= op->value.intval;
	    break;
	    
		default:
	    return op->checkTypeFailed();
  }
  pop(1);
  return 0;
}

int XWPSContextState::zxor()
{
	XWPSRef * op = op_stack.getCurrentTop();

  switch (op->type()) 
  {
		case XWPSRef::Boolean:
	    op[-1].checkType(XWPSRef::Boolean);
	    op[-1].value.boolval ^= op->value.boolval;
	    break;
	    
		case XWPSRef::Integer:
	    op[-1].checkType(XWPSRef::Integer);
	    op[-1].value.intval ^= op->value.intval;
	    break;
	    
		default:
	    return op->checkTypeFailed();
  }
  pop(1);
  return 0;
}
