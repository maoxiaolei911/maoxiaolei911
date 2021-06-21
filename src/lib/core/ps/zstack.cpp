/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include "XWPSError.h"
#include "XWPSContextState.h"

int XWPSContextState::zclearStack()
{
	op_stack.pop(op_stack.count());
  return 0;
}

int XWPSContextState::zclearToMark()
{
	uint count = op_stack.countToMark();
  if (count == 0)
		return (int)(XWPSError::UnmatchedMark);
	
  op_stack.pop(count);
  return 0;
}

int XWPSContextState::zcount()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->makeInt(op_stack.count() - 1);
  return 0;
}

int XWPSContextState::zcountToMark()
{
	XWPSRef * op = op_stack.getCurrentTop();
  uint count = op_stack.countToMark();

  if (count == 0)
		return (int)(XWPSError::UnmatchedMark);
			
  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->makeInt(count - 1);
  return 0;
}

int XWPSContextState::zdup()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = checkOp(op, 1);
  if (code < 0)
  	return code;
  	
  code = push(&op, 1);
  if (code < 0)
		return code;
  op->assign(op - 1);
  return 0;
}

int XWPSContextState::zexch()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef next;

  int code = checkOp(op, 2);
  if (code < 0)
  	return code;
  	
  next.assign(op - 1);
  op[-1].assign(op);
  op->assign(&next);
  return 0;
}

int XWPSContextState::zindex()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * opn;

  int code = op->checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
  	
  if (op->value.intval >= (op - op_stack.getBottom()))
  {
		XWPSRef *elt;

		if (op->value.intval < 0)
			return (int)(XWPSError::RangeCheck);
	  
		elt = op_stack.index(op->value.intval + 1);
		if (elt == 0)
			return (int)(XWPSError::RangeCheck);
		op->assign(elt);
		return 0;
  }
  opn = op + ~(int)op->value.intval;
  op->assign(opn);
  return 0;
}

int XWPSContextState::zmark()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->makeMark();
  return 0;
}

int XWPSContextState::zpop()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = checkOp(op, 1);
  if (code < 0)
  	return code;
  	
  pop(1);
  return 0;
}

int  XWPSContextState::zroll()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * op1 = op - 1;
  
  int code = op1->checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
  	
  code = op->checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
  	
  if (op1->getInt() > (op1 - op_stack.getBottom()))
  {
  	if (op1->getInt() < 0 || ((op1->getInt() + 2) > (int)op_stack.count()))
	    return (int)(XWPSError::RangeCheck);
	    	
		int count = op1->getInt();
		if (count <= 1) 
		{
	    pop(2);
	    return 0;
	  }
	  
	  int mod = op->getInt();
		if (mod >= count)
	    mod %= count;
		else if (mod < 0) 
		{
	    mod %= count;
	    if (mod < 0)
				mod += count;
		}
		
		int left = count;
		for (int i = 0; left; i++)
		{
			XWPSRef *elt = op_stack.index(i + 2);
	    XWPSRef save;
	    save.assign(elt);
	    XWPSRef * next;
	    
	    int j, k;
	    for (j = i, left--;; j = k, elt = next, left--) 
	    {
				k = (j + mod) % count;
				if (k == i)
		    	break;
				next = op_stack.index(k + 2);
				elt->assign(next);
	    }
	    elt->assign(&save);
		}
		
		pop(2);
		return 0;
  }
  
  int count = op1->getInt();
  if (count <= 1) 
  {
		pop(2);
		return 0;
  }
  
  int mod = op->getInt();
  switch (mod)
  {
  	case 1:
  		pop(2);
  		op -= 2;
  		{
  			XWPSRef top;
				top.assign(op);
				XWPSRef * from = op;
				for (int n = count; --n; from--)
		    	from->assign(from - 1);
				from->assign(&top);
  		}
  		return 0;
  		
  	case -1:
  		pop(2);
  		op -= 2;
  		{
				XWPSRef bot;
				XWPSRef * to = op - count + 1;
				bot.assign(to);
				for (int n = count; --n; to++)
		   		to->assign(to + 1);
				to->assign(&bot);
	    }
	    return 0;
	    
	  default:
	   	break;
  }
  
  if (mod < 0)
  {
  	mod += count;
		if (mod < 0) 
		{
	    mod %= count;
	    if (mod < 0)
				mod += count;
		}
  }
  else if (mod >= count)
		mod %= count;
		
	if (mod <= count >> 1)
	{
		if (mod >= op_stack.getTop() - op) 
		{
	    op_stack.setRequested(mod);
	    return (int)(XWPSError::StackOverflow);
		}
		pop(2);
		op -= 2;
		XWPSRef * to = op + mod;
		XWPSRef * from = op;
		for (int n = count; n--; to--, from--)
	    to->assign(from);
	    
	  memcpyRef(from + 1, op + 1, mod);
	}
	else
	{
		mod = count - mod;
		if (mod >= (op_stack.getTop() - op)) 
		{
	    op_stack.setRequested(mod);
	    return (int)(XWPSError::StackOverflow);
		}
		pop(2);
		op -= 2;
		XWPSRef * to = op - count + 1;
		memcpyRef(op + 1, to, mod);
		
		XWPSRef * from = to + mod;
		for (int n = count; n--; to++, from++)
	    to->assign(from);
	}
	
	return 0;
}
