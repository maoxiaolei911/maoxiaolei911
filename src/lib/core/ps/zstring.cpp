/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWPSContextState.h"

int XWPSContextState::zanchorSearch()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * op1 = op - 1;
  uint size = op->size();

  int code = op1->checkReadType(XWPSRef::String);
  if (code < 0)
  	return code;
  	
  code = op->checkReadType(XWPSRef::String);
  if (size <= op1->size() && !memcmp(op1->getBytes(), op->getBytes(), size)) 
  {
		XWPSRef * op0 = op;

		code = push(&op, 1);
		if (code < 0)
			return code;
		op0->assign(op1);
		op0->setSize(size);
		op1->setBytesPtr(op0->getBytes() + size);
		op1->decSize(size);
		op->makeTrue();
  } 
  else
		op->makeFalse();
  return 0;
}

int XWPSContextState::zbyteString()
{
	XWPSRef * op = op_stack.getCurrentTop();
	
  int code = op->checkIntLEU(max_int);
  if (code < 0)
  	return code;
  	
  uint  size = (uint)op->value.intval;
  op->makeAStruct(idmemory()->iallocSpace() | PS_A_ALL, size);
  return 0;
}

int XWPSContextState::znameString()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = op->checkType(XWPSRef::Name);
  if (code < 0)
  	return code;
  nameStringRef(op, op);
  return 0;
}

int XWPSContextState::zsearch()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * op1 = op - 1;
  uint size = op->size();
  uint count;
  uchar *pat;
  uchar *ptr;
  uchar ch;

  int code = op1->checkReadType(XWPSRef::String);
  if (code < 0)
  	return code;
  	
  code = op->checkReadType(XWPSRef::String);
  if (code < 0)
  	return code;
  if (size > op1->size()) 
  {
		op->makeFalse();
		return 0;
  }
  count = op1->size() - size;
  ptr = op1->getBytes();
  if (size == 0)
		goto found;
  pat = op->getBytes();
  ch = pat[0];
  do 
  {
		if (*ptr == ch && (size == 1 || !memcmp(ptr, pat, size)))
	    goto found;
		ptr++;
  }  while (count--);
  
  op->makeFalse();
  return 0;
  
found:
	op->assign(op1);
	op->value.bytes->ptr = ptr;
  op->setSize(size);
  code = push(&op, 2);
  if (code < 0)
		return code;
  op[-1].assign(op1);
  op[-1].setSize(ptr - op1->getBytes());
  op1->setBytesPtr(ptr + size);
  op1->setSize(count);
  op->makeTrue();
  return 0;
}

int XWPSContextState::zstring()
{
	XWPSRef * op = op_stack.getCurrentTop();
	
  int code = op->checkIntLEU(PS_MAX_STRING_SIZE);
  if (code < 0)
  	return code;
  	
  uint  size = (uint)op->value.intval;
  op->makeString(idmemory()->iallocSpace() | PS_A_ALL, size);
  return 0;
}

int XWPSContextState::zstringMatch()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * op1 = op - 1;
  bool result;
  XWPSStringMatchParams params;

  int code = op->checkReadType(XWPSRef::String);
  if (code < 0)
  	return code;
  switch (op1->type()) 
  {
		case XWPSRef::String:
	    code = op1->checkRead();	    
	    goto cmp;
	    
		case XWPSRef::Name:
	    nameStringRef(op1, op1);	/* can't fail */
cmp:
	    result = params.stringMatch(op1->getBytes(), op1->size(),  op->getBytes(), op->size());
	    break;
	    
		default:
	    result = (op->size() == 1 && *op->getBytes() == '*');
  }
  op1->makeBool(result);
  pop(1);
  return 0;
}
