/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include "XWPSError.h"
#include "XWPSContextState.h"

int XWPSContextState::zbegin()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = op->checkType(XWPSRef::Dictionary);
  if (code < 0)
  	return code;
  	
  code = op->checkDictRead();
  if (dict_stack.getCurrentTop() == dict_stack.getTop())
		return (int)(XWPSError::DictStackOverflow);
  dict_stack.incCurrentTop(1);
  XWPSRef * dsp = dict_stack.getCurrentTop();
  dsp->assign(op);
  dict_stack.setTop();
  pop(1);
  return 0;
}

int XWPSContextState::zclearDictStack()
{
	while (zend() >= 0)
		;
  return 0;
}

int XWPSContextState::zcopyDict()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * op1 = op - 1;
  int code;

  code = op1->checkType(XWPSRef::Dictionary);
  if (code < 0)
  	return code;
  	
  code = op1->checkDictRead();
  if (code < 0)
  	return code;
  	
  code = op->checkDictWrite();
  if (code < 0)
  	return code;
  	
  if (!dict_auto_expand && 
  	  (op->dictLength() != 0 || 
  	  op->dictMaxLength() < op1->dictLength()))
  {
		return (int)(XWPSError::RangeCheck);
	}
  
  code = dictCopy(op1, op);
  if (code < 0)
		return code;
    
  if (!level2Enabled())
		op->dictAccessRef()->copyAttrs(PS_A_WRITE, op1->dictAccessRef());
    
  op1->assign(op);
  pop(1);
  return 0;
}

int XWPSContextState::zcountDictStack()
{
	XWPSRef * op = op_stack.getCurrentTop();
  uint count = dict_stack.count();

  int code = push(&op, 1);
  if (code < 0)
		return code;
  if (!level2Enabled())
		count--;
  op->makeInt(count);
  return 0;
}

int XWPSContextState::zcurrentDict()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->assign(dict_stack.getCurrentTop());
  return 0;
}

int XWPSContextState::zdef()
{
	int code = zopDef();

  if (code >= 0) 
		pop(2);
  return code;
}

int XWPSContextState::zdict()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = op->checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
  	
#if arch_sizeof_int < arch_sizeof_long
  code = op->checkIntLEU(max_uint);
  if (code < 0)
  	return code;
#else
  if (op->value.intval < 0)
		return (int)(XWPSError::RangeCheck);
#endif

  op->dictAlloc(iimemory()->space, op->value.intval);
  return 0;
}

int XWPSContextState::zdictCopyNew()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * op1 = op - 1;
  int code;

  code = op1->checkType(XWPSRef::Dictionary);
  if (code < 0)
  	return code;
  	
  code = op1->checkDictRead();
  if (code < 0)
  	return code;
  	
  code = op->checkType(XWPSRef::Dictionary);
  if (code < 0)
  	return code;
  	
  code = op->checkDictWrite();
  if (code < 0)
  	return code;
  	
  if (!dict_auto_expand)
		return (int)(XWPSError::Undefined);
    
  code = dictCopyNew(op1, op);
  if (code < 0)
		return code;
    
  op1->assign(op);
  pop(1);
  return 0;
}

int XWPSContextState::zdictStack()
{
	XWPSRef * op = op_stack.getCurrentTop();
  uint count = dict_stack.count();

  int code = op->checkWriteType(XWPSRef::Array);
  if (code < 0)
  	return code;
  	
  if (!level2Enabled())
		count--;
  
  return dict_stack.store(op, count, 0, 0, true);
}

int XWPSContextState::zdictToMark()
{
	uint count2 = op_stack.countToMark();
  XWPSRef rdict;
  int code;
  uint idx;

  if (count2 == 0)
		return (int)(XWPSError::UnmatchedMark);
	
  count2--;
  if ((count2 & 1) != 0)
		return (int)(XWPSError::RangeCheck);
	
  code = rdict.dictAlloc(iimemory()->space, count2 >> 1);
  for (idx = 0; idx < count2; idx += 2) 
  {
		code = dictPut(&rdict, op_stack.index(idx + 1), op_stack.index(idx));
		if (code < 0) 
	    return code;
  }
  
  op_stack.pop(count2);
  op_stack.getCurrentTop()->assign(&rdict);
  return code;
}

int XWPSContextState::zend()
{
	if (dict_stack.count() == dict_stack.getMinSize()) 
		return (int)(XWPSError::DictStackUnderflow);
  
  while (dict_stack.getCurrentTop() == dict_stack.getBottom()) 
  {
		dict_stack.popBlock();
  }
  dict_stack.incCurrentTop(-1);
  dict_stack.setTop();
  return 0;
}

int XWPSContextState::zforceUndef()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = op[-1].checkType(XWPSRef::Dictionary);
  if (code < 0)
  	return code;

  dictUndef(op - 1, op);
  pop(2);
  return 0;
}

int XWPSContextState::zknown()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * op1 = op - 1;
  XWPSRef *pvalue;

  int code = op1->checkType(XWPSRef::Dictionary);
  if (code < 0)
  	return code;
    
  code = op1->checkDictRead();
 	if (code < 0)
  	return code;
  	
  XWPSRef t;
  t.makeBool(op1->dictFind(this, op, &pvalue) > 0 ? 1 : 0);
  op1->assign(&t);
  pop(1);
  return 0;
}

int XWPSContextState::zknownGet()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * op1 = op - 1;
  XWPSRef *pvalue;

  int code = op1->checkType(XWPSRef::Dictionary);
  if (code < 0)
  	return code;
    
  code = op1->checkDictRead();
 	if (code < 0)
  	return code;
  	
  if (op1->dictFind(this, op, &pvalue) <= 0) 
  {
		op1->makeFalse();
		pop(1);
  } 
  else 
  {
		op1->assign(pvalue);
		op->makeTrue();
  }
  return 0;
}

int XWPSContextState::zknownUndef()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * op1 = op - 1;

  int code = op1->checkType(XWPSRef::Dictionary);
  if (code < 0)
  	return code;
    
  code = op1->checkDictWrite();
  if (code < 0)
  	return code;
  	
  code = dictUndef(op1, op);
  op1->makeBool(code == 0);
  pop(1);
  return 0;
}

int XWPSContextState::zload()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef *pvalue;

  switch (op->type()) 
  {
		case XWPSRef::Name:
	    if ((pvalue = dictFindName(op)) == 0)
				return (int)(XWPSError::Undefined);
	    op->assign(pvalue);
	    return 0;
	    
		case XWPSRef::Null:
	    return (int)(XWPSError::TypeCheck);
	    
		case XWPSRef::Invalid:
	    return (int)(XWPSError::StackUnderflow);
	    
		default: 
			{
				uint size = dict_stack.count();
				uint i;

				for (i = 0; i < size; i++) 
				{
		    	XWPSRef *dp = dict_stack.index(i);

		    	int code = dp->checkDictRead();
		    	if (code < 0)
		    		return code;
		    		
		    	if (dp->dictFind(this, op, &pvalue) > 0) 
		    	{
						op->assign(pvalue);
						return 0;
		    	}
				}
				return (int)(XWPSError::Undefined);
	    }
    }
}

int XWPSContextState::zmaxlength()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = op->checkType(XWPSRef::Dictionary);
  if (code < 0)
  	return code;
  	
  code = op->checkDictRead();
 	if (code < 0)
  	return code;
  	
  XWPSRef t;
  t.makeInt(op->dictMaxLength());
  op->assign(&t);
  return 0;
}

int XWPSContextState::zopDef()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * op1 = op - 1;
  XWPSRef *pvslot;
  switch (op1->type())
  {
  	case XWPSRef::Name: 
  		{
	    	uint nidx = nameIndex(op1);
	    	uint htemp;
	    	if (dict_stack.findNameByIndexTop(nidx, htemp, &pvslot))
	    	{
					if (op->space() <= dict_stack.getDefSpace())
		    		goto ra;
	    	}
	    	break;
	    }
	    
		case XWPSRef::Null:
	    return (int)(XWPSError::TypeCheck);
	    	
		case XWPSRef::Invalid:			
	    return (int)(XWPSError::StackUnderflow);
  }
  
  if (!(op->space() <= dict_stack.getDefSpace())) 
  {
		dict_stack.getCurrentTop()->checkDictWrite();
		return (int)(XWPSError::InvalidAccess);
  }
  
  if (dict_stack.getCurrentTop()->dictFind(this, op1, &pvslot) <= 0)
		return dict_stack.getCurrentTop()->dictPut(this, op1, op, &dict_stack);
	
ra:
	pvslot->assign(op);
	return 0;
}

int XWPSContextState::zsetMaxLength()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * op1 = op - 1;
  uint new_size;

  int code = op1->checkType(XWPSRef::Dictionary);
  if (code < 0)
  	return code;
  	
  code = op1->checkDictWrite();
  if (code < 0)
  	return code;
  	
  code = op->checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
#if arch_sizeof_int < arch_sizeof_long
   code = op->checkIntLEU(max_uint);
  if (code < 0)
  	return code;
#else
  if (op->value.intval < 0)
		return (int)(XWPSError::RangeCheck);
#endif
    
  new_size = (uint) op->value.intval;
  if (op[-1].dictLength() > new_size)
		return (int)(XWPSError::DictFull);
	
  code = dictResize(op - 1, new_size);
  if (code >= 0)
		pop(2);
  return code;
}

int XWPSContextState::zundef()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = op[-1].checkType(XWPSRef::Dictionary);
  if (code < 0)
  	return code;
  	
  code = op[-1].checkDictWrite();
  dictUndef(op - 1, op);
  pop(2);
  return 0;
}

int XWPSContextState::zwhere()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRefStackEnum rsenum;

  int code = checkOp(op, 1);
  if (code < 0)
  	return code;
  	
  rsenum.begin(dict_stack.getStack());
  
  do 
  {
		XWPSRef * bot = rsenum.ptr;
		XWPSRef *pdref = bot + rsenum.size;
		XWPSRef *pvalue;

		while (pdref-- > bot) 
		{
	    code = pdref->checkDictRead();
	    if (code < 0)
  			return code;
	    if (pdref->dictFind(this, op, &pvalue) > 0) 
	    {
				code = push(&op, 1);
				if (code < 0)
					return code;
				op[-1].assign(pdref);
				op->makeTrue();
				return 0;
	    }
		}
  } while (rsenum.next());
  
  op->makeFalse();
  return 0;
}
