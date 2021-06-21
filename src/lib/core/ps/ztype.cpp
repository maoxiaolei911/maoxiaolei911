/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <string.h>
#include "XWPSError.h"
#include "XWPSFont.h"
#include "XWPSContextState.h"

#define ALT_MIN_LONG (-1L << (arch_sizeof_long * 8 - 1))
#define ALT_MAX_LONG (~(ALT_MIN_LONG))
static const double min_int_real = (ALT_MIN_LONG * 1.0 - 1);
static const double max_int_real = (ALT_MAX_LONG * 1.0 + 1);

int  XWPSContextState::accessCheck(int access, bool modify)
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef *aop;

  switch (op->type()) 
  {
		case XWPSRef::Dictionary:
	    aop = op->dictAccessRef();
	    if (modify) 
	    {
				if (!aop->hasAttrs(access))
		    	return (int)(XWPSError::InvalidAccess);
				aop->clearAttrs(PS_A_ALL);
				aop->setAttrs(access);
				dict_stack.setTop();
				return 0;
	    }
	    break;
	    
		case XWPSRef::Array:
		case XWPSRef::File:
		case XWPSRef::String:
		case XWPSRef::MixedArray:
		case XWPSRef::ShortArray:
		case XWPSRef::AStruct:
		case XWPSRef::Device:;
	    if (modify) 
	    {
				if (!op->hasAttrs(access))
		    	return (int)(XWPSError::InvalidAccess);
				op->clearAttrs(PS_A_ALL);
				op->setAttrs(access);
				return 0;
	    }
	    aop = op;
	    break;
	    
		default:
	    return op->checkTypeFailed();
  }
  return (aop->hasAttrs(access) ? 1 : 0);
}

int  XWPSContextState::convertToString(XWPSRef * op1, XWPSRef * op)
{
	uint len;
  const uchar *pstr = 0;
  int code = objCvs(op1, op->getBytes(), op->size(), &len, &pstr);

  if (code < 0)
  {
  	if (code == XWPSError::RangeCheck)
	    switch (op1->btype()) 
	    {
				case XWPSRef::OpArray:
				case XWPSRef::Operator:
		    	if (pstr != 0)
						switch (*pstr) 
						{
			    		case '%':
			    		case '.':
			    		case '@':
								len = op->size();
								memcpy(op->getBytes(), pstr, len);
								goto ok;
						}
	    }
			return code;
  }
  
ok:
  op1->assign(op);
  op1->setSize(len);
  return 0;
}

void XWPSContextState::ensureDot(char *buf)
{
	if (strchr(buf, '.') == NULL) 
	{
		char *ept = strchr(buf, 'e');

		if (ept == NULL)
	    strcat(buf, ".0");
		else 
		{
	    char buf1[30];

	    strcpy(buf1, ept);
	    strcpy(ept, ".0");
	    strcat(ept, buf1);
		}
  }
}

int XWPSContextState::objCvp(XWPSRef * op, uchar * str, 
	             uint len, uint * prlen,
	             int full_print, uint start_pos)
{
	char buf[50];
  const uchar *data = (const uchar *)buf;
  uint size;
  int code;
  XWPSRef nref;

  if (full_print)
  {
  	static const char * const type_strings[] = { 0,0,"-dict-","-file-",
  			"-array-","-packedarray-","-packedarray-","-array-",	0,0,
  			"-fontID-",0,"-mark-",0,0,	0,0,"-save-","-string-",  "-device-",0 };
  			
		switch (op->btype())
		{
			case XWPSRef::Boolean:
			case XWPSRef::Integer:
	    	break;
	    	
	    case XWPSRef::Real:
	    	{
	    		float value = op->value.realval;
	    		float scanned;

	    		sprintf(buf, "%g", value);
	    		sscanf(buf, "%f", &scanned);
	    		if (scanned != value)
						sprintf(buf, "%.9g", value);
	    		ensureDot(buf);
	    		goto rs;
	    	}
	    	
	    case XWPSRef::Operator:
			case XWPSRef::OpArray:  
	    	code = objCvp(op, (uchar *)buf + 2, sizeof(buf) - 4, &size, 0, 0);
	    	if (code < 0) 
					return code;
	    	buf[0] = buf[1] = buf[size + 2] = buf[size + 3] = '-';
	    	size += 4;
	    	goto nl;
	    	
	    case XWPSRef::Name:
	    	if (op->hasAttr(PS_A_EXECUTABLE))
	    	{
	    		code = objStringData(op, &data, &size);
					if (code < 0)
		    		return code;
					goto nl;
	    	}
	    	if (start_pos > 0)
					return objCvp(op, str, len, prlen, 0, start_pos - 1);
	    	if (len < 1)
					return (int)(XWPSError::RangeCheck);
	    	code = objCvp(op, str + 1, len - 1, prlen, 0, 0);
	    	if (code < 0)
					return code;
	    	str[0] = '/';
	    	++*prlen;
	    	return code;
	    	
	    case XWPSRef::Null:
	    	data = (const uchar *)"null";
	    	goto rs;
	    	
	    case XWPSRef::String:  
	    	if (!op->hasAttr(PS_A_READ))
					goto other;
	    	size = op->size();
	    	{
					bool truncate = (full_print == 1 && size > CVP_MAX_STRING);
					PSStreamCursorRead r;
					PSStreamCursorWrite w;
					uint skip;
					uchar *wstr;
					uint len1;
					int status = 1;

					if (start_pos == 0) 
					{
		    		if (len < 1)
							return (int)(XWPSError::RangeCheck);
		    		str[0] = '(';
		    		skip = 0;
		    		wstr = str + 1;
					} 
					else 
					{
		    		skip = start_pos - 1;
		    		wstr = str;
					}
					len1 = len + (str - wstr);
					r.ptr = op->getBytes() - 1;
					r.limit = r.ptr + (truncate ? CVP_MAX_STRING : size);
					while (skip && status == 1) 
					{
		    		uint written;

		    		w.ptr = (uchar *)buf - 1;
		    		w.limit = w.ptr + qMin(skip + len1, sizeof(buf));
		    		status = s_PSSE_template.process(NULL, &r, &w, false);
		    		written = w.ptr - ((uchar *)buf - 1);
		    		if (written > skip) 
		    		{
							written -= skip;
							memcpy(wstr, buf + skip, written);
							wstr += written;
							skip = 0;
							break;
		    		}
		    		skip -= written;
					}
					w.ptr = wstr - 1;
					w.limit = str - 1 + len;
					if (status == 1)
		    		status = s_PSSE_template.process(NULL, &r, &w, false);
					*prlen = w.ptr - (str - 1);
					if (status != 0)
		    		return 1;
					if (truncate) 
					{
		    		if (len - *prlen < 4 - skip)
							return 1;
		    		memcpy(w.ptr + 1, "...)" + skip, 4 - skip);
		    		*prlen += 4 - skip;
					} 
					else 
					{
		    		if (len - *prlen < 1 - skip)
							return 1;
		    		memcpy(w.ptr + 1, ")" + skip, 1 - skip);
		    		*prlen += 1 - skip;
					}
	    	}
	    	return 0;
	    	
	    case XWPSRef::AStruct:
			case XWPSRef::Struct:
				if (op->isForeign())
				{
					data = (const uchar *)"-foreign-struct-";
					goto rs;
				}
				
				data = (const uchar *)(op->getTypeName());
				size = strlen((const char *)data);
	    	if (size > 4 && !memcmp(data + size - 4, "type", 4))
					size -= 4;
	    	if (size > sizeof(buf) - 2)
					return (int)(XWPSError::RangeCheck);
	    	buf[0] = '-';
	    	memcpy(buf + 1, data, size);
	    	buf[size + 1] = '-';
	    	size += 2;
	    	data = (const uchar *)buf;
	    	goto nl;
				
			default:
other:
				{
					int rtype = op->btype();

					if (rtype > (sizeof(type_strings) / sizeof(type_strings[0])))
						return (int)(XWPSError::RangeCheck);
					data = (const uchar *)type_strings[rtype];
					if (data == 0)
						return (int)(XWPSError::RangeCheck);
	    	}
	    	goto rs;
		}
  }
  
  switch (op->btype())
  {
  	case XWPSRef::Boolean:
			data = (const uchar *)(op->value.boolval ? "true" : "false");
			break;
			
    case XWPSRef::Integer:
			sprintf(buf, "%ld", op->value.intval);
			break;
			
    case XWPSRef::String:
			op->checkRead();
			
		case XWPSRef::Name:
			code = objStringData(op, &data, &size);
			if (code < 0)
	    	return code;
			goto nl;
			
    case XWPSRef::OpArray: 
    	{
				uint index = opIndex(op);
				const XWPSOpArrayTable *opt = opIndexOpArrayTable(index);

				nameIndexRef(opt->nx_table[index - opt->base_index], &nref);
				nameStringRef(&nref, &nref);
				code = objStringData(&nref, &data, &size);
				if (code < 0)
	    		return code;
				goto nl;
    	}
    	
    case XWPSRef::Operator:
    	{
    		uint index = opIndex(op);
    		if (index > 0 && index < op_def_count) 
    		{
	    		data = (const uchar *)(opIndexDef(index)->oname + 1);
	    		break;
				}
				sprintf(buf, "@0x%lx", op->value.opproc);
				break;
    	}
    	
    case XWPSRef::Real:
			sprintf(buf, "%g", op->value.realval);
			ensureDot(buf);
			break;
			
    default:
			data = (const uchar *)"--nostringval--";
  }
  
rs: 
	size = strlen((const char *)data);
nl: 
	if (size < start_pos)
		return (int)(XWPSError::RangeCheck);
	
  size -= start_pos;
  *prlen = qMin(size, len);
  memmove(str, data + start_pos, *prlen);
  return (size > len);
}

int XWPSContextState::objCvs(XWPSRef * op, uchar * str, uint len, uint * prlen, const uchar ** pchars)
{
	int code = objCvp(op, str, len, prlen, 0, 0);

  if (code != 1 && pchars) 
  {
		*pchars = str;
		return code;
  }
  code = objStringData(op, pchars, prlen);
  return (code == XWPSError::TypeCheck ? XWPSError::RangeCheck : code);
}

int XWPSContextState::objEq(XWPSRef * pref1, XWPSRef * pref2)
{
	XWPSRef nref;

  if (pref1->type() != pref2->type())
  {
  	switch (pref1->type())
  	{
  		case XWPSRef::Integer:
				return (pref2->hasType(XWPSRef::Real) &&	pref2->value.realval == pref1->value.intval);
				
	    case XWPSRef::Real:
				return (pref2->hasType(XWPSRef::Integer) &&		pref2->value.intval == pref1->value.realval);
				
	    case XWPSRef::Name:
				if (!pref2->hasType(XWPSRef::String))
		    	return false;
				nameStringRef(pref1, &nref);
				pref1 = &nref;
				break;
				
	    case XWPSRef::String:
				if (!pref2->hasType(XWPSRef::Name))
		    	return false;
				nameStringRef(pref2, &nref);
				pref2 = &nref;
				break;
				
	    default:
				if (pref1->btype() != pref2->btype())
		    	return false;
  	}
  }
  
  switch (pref1->btype())
  {
  	case XWPSRef::Array:
	    return (pref1->getArray() == pref2->getArray() &&  pref1->size() == pref2->size());
	    
		case XWPSRef::MixedArray:
		case XWPSRef::ShortArray:
	    return (pref1->getPacked() == pref2->getPacked() &&  pref1->size() == pref2->size());
	    
		case XWPSRef::Boolean:
	    return (pref1->value.boolval == pref2->value.boolval);
	    
		case XWPSRef::Dictionary:
	    return (pref1->getDict() == pref2->getDict());
	    
		case XWPSRef::File:
	    return (pref1->getStream() == pref2->getStream() &&  pref1->size() == pref2->size());
	    
		case XWPSRef::Integer:
	    return (pref1->value.intval == pref2->value.intval);
	    
		case XWPSRef::Mark:
		case XWPSRef::Null:
	    return true;
	    
		case XWPSRef::Name:
	    return (pref1->getName() == pref2->getName());
	    
		case XWPSRef::OpArray:
		case XWPSRef::Operator:
	    return (opIndex(pref1) == opIndex(pref2));
	    
		case XWPSRef::Real:
	    return (pref1->value.realval == pref2->value.realval);
	    
		case XWPSRef::Save:
	    return (pref2->value.saveid == pref1->value.saveid);
	    
		case XWPSRef::String:
	    return (!bytes_compare(pref1->getBytes(), pref1->size(),  pref2->getBytes(), pref2->size()));
	    
		case XWPSRef::Device:
	    return (pref1->value.pdevice == pref2->value.pdevice);
	    
		case XWPSRef::Struct:
		case XWPSRef::AStruct:
	    return (pref1->getStruct() == pref2->getStruct());
	    
		case XWPSRef::FontID:
	    {
				const XWPSFont *pfont1 = (XWPSFont*)(pref1->getStruct());
				const XWPSFont *pfont2 = (XWPSFont*)(pref2->getStruct());

				while (pfont1->base != pfont1)
		    	pfont1 = pfont1->base;
		    	
				while (pfont2->base != pfont2)
		    	pfont2 = pfont2->base;
				return (pfont1 == pfont2);
	    }
  }
  
  return false;
}

int XWPSContextState::objStringData(XWPSRef *op, const uchar **pchars, uint *plen)
{
	switch (op->type()) 
	{
    case XWPSRef::Name: 
    	{
				XWPSRef nref;

				nameStringRef(op, &nref);
				*pchars = nref.getBytes();
				*plen = nref.size();
				return 0;
    	}
    	
    case XWPSRef::String:
			op->checkRead();
			*pchars = op->getBytes();
			*plen = op->size();
			return 0;
			
    default:
			return (int)(XWPSError::TypeCheck);
  }
}

int XWPSContextState::zcvi()
{
	XWPSRef * op = op_stack.getCurrentTop();
  float fval;

  switch (op->type()) 
  {
		case XWPSRef::Integer:
	    return 0;
	    
		case XWPSRef::Real:
	    fval = op->value.realval;
	    break;
	    
		default:
	    return op->checkTypeFailed();
	    
		case XWPSRef::String:
	    {
				XWPSRef str, token;
				int code;

				str.assign(op);
				code = scanStringToken(&str, &token);
				if (code > 0)	
		    	code = XWPSError::SyntaxError;
				if (code < 0)
		    	return code;
				switch (token.type()) 
				{
		    	case XWPSRef::Integer:
						op->assign(&token);
						return 0;
						
		    	case XWPSRef::Real:
						fval = token.value.realval;
						break;
						
		    	default:
						return op->checkTypeFailed();
				}
	    }
  }
  if (!((fval) > min_int_real && (fval) < max_int_real))
		return (int)(XWPSError::RangeCheck);
  op->makeInt((long)fval);
  return 0;
}

int XWPSContextState::zcvlit()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef *aop;

  int code = checkOp(op, 1);
  if (code < 0)
		return code;
		
  aop = (op->hasType(XWPSRef::Dictionary) ? op->dictAccessRef() : op);
  aop->clearAttrs(PS_A_EXECUTABLE);
  return 0;
}

int XWPSContextState::zcvn()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = op->checkReadType(XWPSRef::String);
  if (code < 0)
		return code;
  return nameFromString(op, op);
}

int XWPSContextState::zcvr()
{
	XWPSRef * op = op_stack.getCurrentTop();

  switch (op->type()) 
  {
		case XWPSRef::Integer:
	    op->makeReal(op->value.intval);
	    
		case XWPSRef::Real:
	    return 0;
	    
		default:
	    return op->checkTypeFailed();
	    
		case XWPSRef::String:
	    {
				XWPSRef str, token;
				int code;

				str.assign(op);
				code = scanStringToken(&str, &token);
				if (code > 0)	
		    	code = XWPSError::SyntaxError;
				if (code < 0)
		    	return code;
				switch (token.type()) 
				{
		    	case XWPSRef::Integer:
						op->makeReal(token.value.intval);
						return 0;
						
		    	case XWPSRef::Real:
						op->assign(&token);
						return 0;
						
		    	default:
						return op->checkTypeFailed();
						
				}
	    }
  }
}

int XWPSContextState::zcvrs()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int radix;

  int code =  op[-1].checkType(XWPSRef::Integer);
  if (code < 0)
		return code;
		
  if (op[-1].value.intval < 2 || op[-1].value.intval > 36)
		return (int)(XWPSError::RangeCheck);
  radix = op[-1].value.intval;
  code = op->checkWriteType(XWPSRef::String);
  if (code < 0)
		return code;
		
  if (radix == 10) 
  {
		switch (op[-2].type()) 
		{
	    case XWPSRef::Integer:
	    case XWPSRef::Real:
				{
		    	code = convertToString(op - 2, op);

		    	if (code < 0)
						return code;
		    	pop(2);
		    	return 0;
				}
				
	    default:
				return op[-2].checkTypeFailed();
		}
  } 
  else 
  {
		ulong ival;
		uchar digits[sizeof(ulong) * 8];
		uchar *endp = &digits[sizeof(digits)/sizeof(digits[0])];
		uchar *dp = endp;

		switch (op[-2].type()) 
		{
	    case XWPSRef::Integer:
				ival = (ulong) op[-2].value.intval;
				break;
				
	    case XWPSRef::Real:
				{
		    	float fval = op[-2].value.realval;

		    	if (!((fval) > min_int_real && (fval) < max_int_real))
						return (int)(XWPSError::RangeCheck);
		    	ival = (ulong) (long)fval;
				} 
				break;
				
	    default:
				return op[-2].checkTypeFailed();
		}
		do 
		{
	    int dit = ival % radix;

	    *--dp = dit + (dit < 10 ? '0' : ('A' - 10));
	    ival /= radix;
		}	while (ival);
		
		if (endp - dp > op->size())
			return (int)(XWPSError::RangeCheck);
		memcpy(op->getBytes(), dp, (uint) (endp - dp));
		op->setSize(endp - dp);
  }
  op[-2].assign(op);
  pop(2);
  return 0;
}

int XWPSContextState::zcvs()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code;

  code = checkOp(op, 2);
  if (code < 0)
		return code;
		
  code = op->checkWriteType(XWPSRef::String);
  if (code < 0)
		return code;
		
  code = convertToString(op - 1, op);
  if (code >= 0)
		pop(1);
  return code;
}

int XWPSContextState::zcvx()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef *aop;
  uint opidx;

  int code = checkOp(op, 1);
  if (code < 0)
		return code;
		
  if (op->hasType(XWPSRef::Operator) &&
			((opidx = opIndex(op)) == 0 ||
	 		op_def_is_internal(opIndexDef(opidx))))
	{
		return (int)(XWPSError::RangeCheck);
	}
  aop = (op->hasType(XWPSRef::Dictionary) ? op->dictAccessRef() : op);
  aop->setAttrs(PS_A_EXECUTABLE);
  return 0;
}

int XWPSContextState::zexecuteOnly()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = checkOp(op, 1);
  if (code < 0)
		return code;
		
  if (op->hasType(XWPSRef::Dictionary))
		return (int)(XWPSError::TypeCheck);
  return accessCheck(PS_A_EXECUTE, true);
}

int XWPSContextState::znoaccess()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = checkOp(op, 1);
  if (code < 0)
		return code;
		
  if (op->hasType(XWPSRef::Dictionary))
  	if (dict_stack.dictIsPermanent(op) || !op->dictAccessRef()->hasAttr(PS_A_WRITE))
	    return (int)(XWPSError::InvalidAccess);
  return accessCheck(0, true);
}

int XWPSContextState::zrcheck()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code = accessCheck(PS_A_READ, false);

  if (code >= 0)
		op->makeBool(code), code = 0;
  return code;
}

int XWPSContextState::zreadOnly()
{
	return accessCheck(PS_A_READONLY, true);
}

int XWPSContextState::ztype()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef tnref;
  int code = op->arrayGet(this, (long)op[-1].btype(), &tnref);

  if (code < 0)
		return code;
    
  if (!tnref.hasType(XWPSRef::Name)) 
  {
		code = checkOp(op, 2);
		if (code < 0)
			return code;
			
		const char *sname = op[-1].getTypeName();
		code = nameRef((uchar*)sname, strlen(sname), op - 1, 0);
		if (code < 0)
			return code;
	
		op[-1].setAttrs(PS_A_EXECUTABLE);
  } 
  else 
		op[-1].assign(&tnref);
  pop(1);
  return 0;
}

int XWPSContextState::ztypeNames()
{
	XWPSRef * op = op_stack.getCurrentTop();
  static const char *const tnames[] = { 0,"booleantype","dicttype","filetype",
           "arraytype","packedarraytype","packedarraytype","arraytype",
           0,0,  "fonttype","integertype","marktype","nametype","nulltype",
           "operatortype","realtype","savetype","stringtype",  "devicetype","operatortype" };
  int i;

  int code = checkOStack(XWPSRef::NextIndex);
  if (code < 0)
		return code;
  for (i = 0; i < XWPSRef::NextIndex; i++) 
  {
		XWPSRef *const rtnp = op + 1 + i;

		if ((i >= (sizeof(tnames) / sizeof(tnames[0]))) || tnames[i] == 0)
	    rtnp->makeNull();
		else 
		{
	    int code = nameEnterString(tnames[i], rtnp);

	    if (code < 0)
				return code;
	    rtnp->setAttrs(PS_A_EXECUTABLE);
		}
  }
  op_stack.incCurrentTop(XWPSRef::NextIndex);
  return 0;
}

int XWPSContextState::zwcheck()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code = accessCheck(PS_A_WRITE, false);

  if (code >= 0)
		op->makeBool(code), code = 0;
  return code;
}

int XWPSContextState::zxcheck()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = checkOp(op, 1);
  if (code < 0)
		return code;
		
	XWPSRef * aop = (op->hasType(XWPSRef::Dictionary) ? op->dictAccessRef() : op);
		
  op->makeBool((aop->hasAttr(PS_A_EXECUTABLE) ? 1 : 0));
  return 0;
}
