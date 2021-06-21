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

int XWPSContextState::arrayContinue()
{
	XWPSRef * op = op_stack.getCurrentTop();
	XWPSRef * esp = exec_stack.getCurrentTop();
  XWPSRef * obj = esp - 1;

  if (obj->size()) 
  {
		int code = push(&op, 1);
		if (code < 0)
			return code;
		obj->decSize(1);
		op->assign(obj->getArray());
		obj->setArrayPtr(obj->getArray() + 1);
		esp = exec_stack.incCurrentTop(2);
		esp->assign(obj + 1);
		return PS_O_PUSH_ESTACK;
  } 
  else 
  {	
		esp = exec_stack.incCurrentTop(-3);
		return PS_O_POP_ESTACK;
  }
}

int XWPSContextState::copyInterval(XWPSRef * prto, uint index, XWPSRef* prfrom)
{
	int fromtype = prfrom->type();
  uint fromsize = prfrom->size();

  if (!(fromtype == prto->type() ||
	  	((fromtype == XWPSRef::ShortArray || fromtype == XWPSRef::MixedArray) &&
	   		prto->type() == XWPSRef::Array)))
	return prfrom->checkTypeFailed();
		
	int code = prfrom->checkRead();
	if (code < 0)
		return code;
		
  code = prto->checkWrite();
  if (code < 0)
		return code;
		
	if (fromsize > prto->size() - index)
		return (int)(XWPSError::RangeCheck);
  
  switch (fromtype)
  {
  	case XWPSRef::Array:
			return prto->cpyToOld(index, prfrom->getArray(),  fromsize);
				
		case XWPSRef::String:
			memmove(prto->getBytes() + index, prfrom->getBytes(),	fromsize);
	    break;
	    
		case XWPSRef::MixedArray:
		case XWPSRef::ShortArray:
	    {
				uint i;
				ushort *packed = prfrom->getPacked();
				XWPSRef *pdest = prto->getArray() + index;
				XWPSRef elt;

				for (i = 0; i < fromsize; i++, pdest++) 
				{
		    	packedGet(packed, &elt);
		    	pdest->assign(&elt);
		    	packed = packed_next(packed);
				}
	    }
	    break;
  }
  
  return 0;
}

int XWPSContextState::dictContinue()
{
	XWPSRef * op = op_stack.getCurrentTop();
	XWPSRef * esp = exec_stack.getCurrentTop();
  XWPSRef * obj = esp - 2;
  int index = (int)esp->value.intval;

  int code = push(&op, 2);
  if (code < 0)
		return code;
			
  if ((index = obj->dictNext(this, index, op - 1)) >= 0) 
  {
  	esp = exec_stack.getCurrentTop();
		esp->value.intval = index;
		esp = exec_stack.incCurrentTop(2);
		esp->assign(obj + 1);
		return PS_O_PUSH_ESTACK;
  } 
  else 
  {	
		pop(2);	
		esp = exec_stack.incCurrentTop(-4);
		return PS_O_POP_ESTACK;
  }
}

int XWPSContextState::makePackedArray(XWPSRef * parr, XWPSRefStack * pstack, uint size)
{
	uint idest = 0, ishort = 0;
	int skip = 0;
	XWPSRefMemory *imem = idmemory()->current;
	uint space = imem->space;
	XWPSRef * pref;
	int code;
	for (uint i = size; i != 0; i--)
	{
		pref = pstack->index(i - 1);
		switch (pref->btype())
		{
			case XWPSRef::Name:
				if (nameIndex(pref) >= packed_name_max_index)
		    	break;
				idest++;
				continue;
				
			case XWPSRef::Integer:
				if (pref->getInt() < packed_min_intval || pref->getInt() > packed_max_intval)
		    	break;
				idest++;
				continue;
				
			case XWPSRef::OpArray:
				code = pref->storeCheckSpace(space);
				if (code < 0)
					return code;
			case XWPSRef::Operator:
				{
					if (!pref->hasAttr(PS_A_EXECUTABLE))
						break;
		    	uint oidx = pref->size() == 0 ? opFindIndex(pref) : pref->size();
		    	if (oidx == 0 || oidx > packed_int_mask)
						break;
				}
				idest++;
				continue;
				
	    default:
	    	code = pref->storeCheckSpace(space);
				if (code < 0)
					return code;
	    	break;
		}
		
		{
	    int j = (idest - ishort) & (align_packed_per_ref - 1);
	    if (ishort == 0)
				idest += skip = -j & (align_packed_per_ref - 1);
	    else
				idest += (packed_per_ref - 1) * j;
		}
		ishort = idest += packed_per_ref;
	}
	
	int pad = -idest & (packed_per_ref - 1);
	
	uint ksize = (idest + pad) / packed_per_ref;
	parr->clear();
	parr->value.packed = new XWPSPackedArrayRef(ksize, size);
	ushort * pbody = parr->value.packed->ptr;
	ushort * pshort = pbody;
	for (; skip; skip--)
		*pbody++ = pt_tag(pt_integer);
			
  ushort * pdest = pbody;
  for (uint i = size; i != 0; i--)
  {
  	pref = pstack->index(i - 1);
  	switch (pref->btype())
  	{
  		case XWPSRef::Name:
				{
	    		uint nidx = nameIndex(pref);
	    		if (nidx >= packed_name_max_index)
						break;
	    		*pdest++ = nidx + (pref->hasAttr(PS_A_EXECUTABLE) ? pt_tag(pt_executable_name) : pt_tag(pt_literal_name));
				}
				continue;
				
			case XWPSRef::Integer:
				if (pref->getInt() < packed_min_intval || pref->getInt() > packed_max_intval)
	    		break;
				*pdest++ = pt_tag(pt_integer) + ((short)pref->getInt() - packed_min_intval);
				continue;
				
			case XWPSRef::OpArray:
	   	case XWPSRef::Operator:
				{
	    		if (!pref->hasAttr(PS_A_EXECUTABLE))
						break;
	    		uint oidx = pref->size() == 0 ? opFindIndex(pref) : pref->size();
	    		if (oidx == 0 || oidx > packed_int_mask)
						break;
	    		*pdest++ = pt_tag(pt_executable_operator) + oidx;
				}
				continue;
  	}
  	
  	{
	   	int j = (pdest - pshort) & (align_packed_per_ref - 1);
	   	ushort *psrc = pdest;
	   	XWPSRef *pmove = (XWPSRef *) (pdest += (packed_per_ref - 1) * j);
	   	pmove->assign(pref);
	   	while (--j >= 0) 
	   	{
				--psrc;
				--pmove;
				packedGet(psrc, pmove);
	   	}
		}
		
		pshort = pdest += packed_per_ref;
  }
  
  {
		int atype =	(pdest == pbody + size ? XWPSRef::ShortArray : XWPSRef::MixedArray);
		for (; pad; pad--)
	    *pdest++ = pt_tag(pt_integer);
		pstack->pop(size);
		parr->setTypeAttrs((ushort)atype, idmemory()->iallocSpace() | PS_A_READONLY); 
		parr->setSize(size); 
		parr->setPackedPtr(pbody + skip);
  }
  return 0;
}

int XWPSContextState::packedArrayContinue()
{
	XWPSRef * op = op_stack.getCurrentTop();
	XWPSRef * esp = exec_stack.getCurrentTop();
  XWPSRef * obj = esp - 1;

  if (obj->size()) 
  {
		ushort *packed = obj->getPacked();

		obj->decSize(1);
		int code = push(&op, 1);
		if (code < 0)
			return code;
		packedGet(packed, op);
		obj->setPackedPtr(packed_next(packed));
		esp = exec_stack.incCurrentTop(2);
		esp->assign(obj + 1);
		return PS_O_PUSH_ESTACK;
  } 
  else 
  {	
		esp = exec_stack.incCurrentTop(-3);
		return PS_O_POP_ESTACK;
  }
}

int XWPSContextState::stringContinue()
{
	XWPSRef * op = op_stack.getCurrentTop();
	XWPSRef * esp = exec_stack.getCurrentTop();
  XWPSRef * obj = esp - 1;

  if (obj->size()) 
  {
		obj->decSize(1);
		int code = push(&op, 1);
		if (code < 0)
			return code;
		op->makeInt(*(obj->getBytes()));
		obj->setBytesPtr(obj->getBytes() + 1);
		esp = exec_stack.incCurrentTop(2);
		esp->assign(obj + 1);
		return PS_O_PUSH_ESTACK;
  } 
  else 
  {	
		esp = exec_stack.incCurrentTop(-3);
		return PS_O_POP_ESTACK;
  }
}

int XWPSContextState::zaload()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef aref;
  uint asize;

  aref.assign(op);
  if (!aref.isArray())
		return op->checkTypeFailed();
    
  int code = aref.checkRead();
  if (code < 0)
		return code;
		
  asize = aref.size();
  if (asize > op_stack.getTop() - op) 
  {
		int code = op_stack.push(asize);
		uint i;
		if (code < 0)
	    return code;
	    
	  ushort *packed = aref.getPacked();
		for (i = asize; i > 0; i--, packed = packed_next(packed))
	    packedGet(packed, op_stack.index(i));
		op_stack.getCurrentTop()->assign(&aref);
		return 0;
  }
  
  if (aref.hasType(XWPSRef::Array))
  {
  	XWPSRef * s = aref.getArray();
  	memcpyRef(op, s, asize);
	}
  else 
  {
		uint i;
		ushort  * packed = aref.getPacked();
		XWPSRef * pdest = op;

		for (i = 0; i < asize; i++, pdest++, packed = packed_next(packed))
	    packedGet(packed, pdest);
  }
  code = push(&op, asize);
  if (code < 0)
		return code;
  op->assign(&aref);
  return 0;
}

int XWPSContextState::zarray()
{
	XWPSRef * op = op_stack.getCurrentTop();
  uint size;
  int code;

  code = op->checkIntLEU(PS_MAX_ARRAY_SIZE);
  if (code < 0)
		return code;
  size = op->value.intval;
  op->makeArray(currentSpace() | PS_A_ALL, size);
  
  return 0;
}

int XWPSContextState::zastore()
{
	XWPSRef * op = op_stack.getCurrentTop();
  uint size;
  int code;

  code = op->checkWriteType(XWPSRef::Array);
  if (code < 0)
		return code;
  size = op->size();
  if (size > op - op_stack.getBottom()) 
  {
		XWPSRef arr;

		if (size >= op_stack.count())
	    return (int)(XWPSError::StackUnderflow);
		arr.assign(op);
		code = op_stack.store(&arr, size, 1, 0, true);
		if (code < 0)
	    return code;
		op_stack.pop(size);
		op_stack.index(0)->assign(&arr);
  } 
  else 
  {
  	code = op->cpyToOld(0, op - size, size);
  	if (code < 0)
	    return code;
		op[-(int)size].assign(op);
		pop(size);
  }
  return 0;
}

int XWPSContextState::zcopy()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int type = op->type();

  if (type == XWPSRef::Integer)
		return zcopyInteger();
		
  int code = checkOp(op, 2);
  if (code < 0)
	  return code;
    
  switch (type) 
  {
		case XWPSRef::Array:
		case XWPSRef::String:
	    return zcopyInterval();
	    
		case XWPSRef::Dictionary:
	    return zcopyDict();
	    
		default:
	    return op->checkTypeFailed();
  }
}

int XWPSContextState::zcopyInteger()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * op1 = op - 1;
  int count, i;
  int code;

  if ((ulong) op->value.intval > op - op_stack.getBottom()) 
  {
		code = op->checkIntLTU(op_stack.count());
		if (code < 0)
			return code;
		count = op->value.intval;
  } 
  else if (op1 + (count = op->value.intval) <= op_stack.getTop()) 
  {
  	memcpyRef(op, op - count, count);
		code = push(&op, count - 1);
		if (code < 0)
			return code;
		return 0;
  }
  
  code = op_stack.push(count - 1);
  if (code < 0)
		return code;
  for (i = 0; i < count; i++)
		op_stack.index(i)->assign(op_stack.index(i + count));
  return 0;
}

int XWPSContextState::zcopyInterval()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * op1 = op - 1;
  int code = copyInterval(op, 0, op1);

  if (code < 0)
		return code;
  op->setSize(op1->size());
  op1->assign(op);
  pop(1);
  return 0;
}

int XWPSContextState::zcurrentPacking()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->assign(&array_packing);
  return 0;
}

int XWPSContextState::zforall()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * obj = op - 1;
  XWPSRef * ep = exec_stack.getCurrentTop();
  XWPSRef * cproc = ep + 4;

  int code = checkEStack(6);
  if (code < 0)
		return code;
  switch (obj->type()) 
  {
		default:
	    return obj->checkTypeFailed();
	    
		case XWPSRef::Array:
	    code = obj->checkRead();
	    if (code < 0)
				return code;
	    cproc->makeOper(0, &XWPSContextState::arrayContinue);
	    break;
	    
		case XWPSRef::Dictionary:
	    code = obj->checkDictRead();
	    if (code < 0)
				return code;
	   	cproc->makeInt(obj->dictFirst());
	    ++cproc;
	    cproc->makeOper(0, &XWPSContextState::dictContinue);
	    break;
	    
		case XWPSRef::String:
	    code = obj->checkRead();
	    if (code < 0)
				return code;
			cproc->makeOper(0, &XWPSContextState::stringContinue);
	    break;
	    
		case XWPSRef::MixedArray:
		case XWPSRef::ShortArray:
	    code = obj->checkRead();
	    if (code < 0)
				return code;
			cproc->makeOper(0, &XWPSContextState::packedArrayContinue);
	    break;
	    
  }
  code = op->checkProc();
  if (code < 0)
		return code;
  ep[1].makeMarkEStack(PS_ES_FOR, &XWPSContextState::forallCleanup);
  ep[2].assign(obj);
  ep[3].assign(op);
  exec_stack.setCurrentTop(cproc - 1);
  pop(2);
  return cproc->callProc(this);
}

int XWPSContextState::zforcePut()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * op1 = op - 1;
  XWPSRef * op2 = op - 2;
  int code;

  switch (op2->type()) 
  {
    case XWPSRef::Array:
			op1->checkIntLTU(op2->size());
			if (op2->space() > op->space()) 
			{
	    	if (iimemory()->save_level)
					return (int)(XWPSError::InvalidAccess);
			}
			{
	    	XWPSRef *eltp = op2->getArray() + (uint) op1->value.intval;

	    	eltp->assign(op);
			}
			break;
			
    case XWPSRef::Dictionary:
			if (op2->getDict() == dict_stack.getSystemDict()->getDict() ||
				 !(iimemory()->save_level))
	    {
	    	uint space = op2->space();

	    	op2->setSpace(PS_AVM_LOCAL);
	    	code = dictPut(op2, op1, op);
	    	op2->setSpace(space);
			} 
			else
	    	code = dictPut(op2, op1, op);
				if (code < 0)
	    		return code;
			break;
			
    default:
			return (int)(XWPSError::TypeCheck);
  }
  pop(3);
  return 0;
}

int XWPSContextState::zget()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * op1 = op - 1;
  XWPSRef *pvalue;
  uchar * pp;

  switch (op1->type()) 
  {
		case XWPSRef::Dictionary:
	    op1->checkDictRead();
	    if (op1->dictFind(this, op, &pvalue) <= 0)
				return (int)(XWPSError::Undefined);
	    op[-1].assign(pvalue);
	    break;
	    
		case XWPSRef::String:
	    op1->checkRead();
	    op->checkIntLTU(op1->size());
	    pp = op1->getBytes();
	    {
	    	XWPSRef t;
	    	t.makeInt((uint)pp[op->value.intval]);
	    	op1->assign(&t);
	    }
	    break;
	    
		default: 
			{
	    	int code;

	    	code = op->checkType(XWPSRef::Integer);
	    	if (code < 0)
					return code;
					
	    	op1->checkRead();
	    	if (code < 0)
					return code;
					
	    	code = op1->arrayGet(this, op->value.intval, op1);
	    	if (code < 0) 
	    	{
					if (code == XWPSError::TypeCheck)
		    		op1->checkTypeFailed();
					else
		    		return code;
	    	}
			}
  }
  pop(1);
  return 0;
}

int XWPSContextState::zgetInterval()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * op1 = op - 1;
  XWPSRef * op2 = op1 - 1;
  uint index;
  uint count;

  switch (op2->type()) 
  {
		default:
	    return op2->checkTypeFailed();
	    
		case XWPSRef::Array:
		case XWPSRef::String:
		case XWPSRef::MixedArray:
		case XWPSRef::ShortArray:;
  }
  int code = op2->checkRead();
  if (code < 0)
		return code;
		
  code = op1->checkIntLEU(op2->size());
  index = op1->value.intval;
  op->checkIntLEU(op2->size() - index);
  count = op->value.intval;
  switch (op2->type()) 
  {
		case XWPSRef::Array:
	    op2->setArrayPtr(op2->getArray() + index);
	    break;
	    
		case XWPSRef::String:
	    op2->setBytesPtr(op2->getBytes() + index);
	    break;
	    
		case XWPSRef::MixedArray:
			{
	    	ushort *packed = op2->getPacked();

	    	for (; index--;)
					packed = packed_next(packed);
	    	op2->setPackedPtr(packed);
	    	break;
			}
			
		case XWPSRef::ShortArray:
	    op2->setPackedPtr(op2->getPacked() + index);
	    break;
  }
  op2->setSize(count);
  pop(2);
  return 0;
}

int XWPSContextState::zlength()
{
	XWPSRef * op = op_stack.getCurrentTop();
  switch (op->type()) 
  {
		case XWPSRef::Array:
		case XWPSRef::String:
		case XWPSRef::MixedArray:
		case XWPSRef::ShortArray:
	    op->checkRead();
	    op->makeInt(op->size());
	    return 0;
	    
		case XWPSRef::Dictionary:
	    op->checkDictRead();
	    op->makeInt(op->dictLength());
	    return 0;
	    
		case XWPSRef::Name: 
			{
	    	XWPSRef str;

	    	nameStringRef(op, &str);
	    	op->makeInt(str.size());
	    	return 0;
			}
		
		case XWPSRef::AStruct:
	    op->checkRead();
	    {
				const char * tname = op->getTypeName();
				if (strcmp(tname, "bytes"))
					return (int)(XWPSError::TypeCheck);
						
				XWPSBytes * b = (XWPSBytes*)(op->value.pstruct);
	    	op->makeInt(b->length);
	    }
	    return 0;
	    
		default:
	    return op->checkTypeFailed();
  }
}

int XWPSContextState::zpackedArray()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code;
  XWPSRef parr;

  code = op->checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
  	
  if (op->value.intval < 0 ||	((op->value.intval > (op - op_stack.getBottom())) &&
	 		op->value.intval >= op_stack.count()))
	{
		return (int)(XWPSError::RangeCheck);
	}
	
  op_stack.incCurrentTop(-1);
  code = makePackedArray(&parr, op_stack.getStack(), (uint) op->value.intval);
  op_stack.incCurrentTop(1);
  if (code >= 0)
		op_stack.getCurrentTop()->assign(&parr);
  return code;
}

int XWPSContextState::zput()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * op1 = op - 1;
  XWPSRef * op2 = op1 - 1;
  uchar *sdata;
  uint ssize;

  switch (op2->type()) 
  {
		case XWPSRef::Dictionary:
	    op2->checkDictWrite();
	    {
				int code = dictPut(op2, op1, op);

				if (code < 0)
		    	return code;
	    }
	    break;
	    
		case XWPSRef::Array:
	    op2->checkWrite();
	    op1->checkIntLTU(op2->size());
	    op2->storeCheckDest(op);
	    {
				XWPSRef *eltp = op2->getArray() + (uint) op1->value.intval;

				eltp->assign(op);
	    }
	    break;
	    
		case XWPSRef::MixedArray:	
		case XWPSRef::ShortArray:
	    return (int)(XWPSError::InvalidAccess);
	    
		case XWPSRef::String:
	    sdata = op2->getBytes();
	    ssize = op2->size();
str:	    
			op2->checkWrite();
	    op1->checkIntLTU(ssize);
	    op->checkIntLTU(0xff);
	    sdata[(uint)op1->value.intval] = (uchar)op->value.intval;
	    break;
	    
		case XWPSRef::AStruct:
			{
				const char * tname = op->getTypeName();
				if (strcmp(tname, "bytes"))
					return (int)(XWPSError::TypeCheck);
						
				XWPSBytes * b = (XWPSBytes*)op->value.pstruct;
				sdata = b->arr;
	    	ssize = b->length;
			}
	    goto str;
	    
		default:
	    return op2->checkTypeFailed();
  }
  pop(3);
  return 0;
}

int XWPSContextState::zputInterval()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * opindex = op - 1;
  XWPSRef * opto = opindex - 1;
  int code;

  switch (opto->type()) 
  {
		default:
	    return opto->checkTypeFailed();
	    
		case XWPSRef::MixedArray:	
		case XWPSRef::ShortArray:
	    return (int)(XWPSError::InvalidAccess);
	    
		case XWPSRef::Array:
		case XWPSRef::String:
	    code = opto->checkWrite();
	    if (code < 0)
		    return code;
	    code = opindex->checkIntLEU(opto->size());
	    if (code < 0)
		    return code;
	    code = copyInterval(opto, (uint)(opindex->value.intval), op);
	    break;
	    
		case XWPSRef::AStruct:
			{
				const char * tname = opto->getTypeName();
				if (strcmp(tname, "bytes"))
					return (int)(XWPSError::TypeCheck);
						
				XWPSBytes * b = (XWPSBytes*)opto->value.pstruct;
				
	    	uint dsize, ssize, index;

	    	code = opto->checkWrite();
	    	if (code < 0)
		    	return code;
	    	dsize = b->length;
	    	code = opindex->checkIntLEU(dsize);
	    	if (code < 0)
		    	return code;
	    	index = (uint)opindex->value.intval;
	    	code = op->checkReadType(XWPSRef::String);
	    	if (code < 0)
		    	return code;
	    	ssize = op->size();
	    	if (ssize > dsize - index)
					return (int)(XWPSError::RangeCheck);
	    	memcpy(b->arr + index, op->getBytes(), ssize);
	    	code = 0;
	    	break;
	    	
			}
  }
  if (code >= 0)
		pop(3);
  return code;
}

int XWPSContextState::zsetPacking()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = op->checkType(XWPSRef::Boolean);
  if (code < 0)
  	return code;
  	
  array_packing.assign(op);
  pop(1);
  return 0;
}
