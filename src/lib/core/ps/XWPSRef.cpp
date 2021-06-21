/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <QHash>
#include "XWPSError.h"
#include "XWPSName.h"
#include "XWPSDict.h"
#include "XWPSStream.h"
#include "XWPSFont.h"
#include "XWPSDevice.h"
#include "XWPSContextState.h"
#include "XWPSRef.h"

uchar ref_type_properties[] = {
  0,
  0,
  PS_RTYPE_USES_ACCESS | PS_RTYPE_IS_DICTIONARY,
  PS_RTYPE_USES_ACCESS | PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_ACCESS | PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_ACCESS | PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_ACCESS | PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_ACCESS | PS_RTYPE_USES_SIZE,
  0,	
  PS_RTYPE_USES_ACCESS,
  0,
  0,
  0,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_IS_NULL,
  PS_RTYPE_USES_SIZE,
  0,
  0,
  PS_RTYPE_USES_ACCESS | PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_ACCESS,
  PS_RTYPE_USES_SIZE,	
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE,
  PS_RTYPE_USES_SIZE
};

const uchar enc_num_bytes[] = {
    4, 4, 2, 4, 0, 0, 0, 0,
  4, 4, 2, 4, 0, 0, 0, 0,
  sizeof(XWPSRef)
};
  
static const double binary_scale[32] = {
#define EXPN2(n) (0.5 / (1L << (n-1)))
    1.0, EXPN2(1), EXPN2(2), EXPN2(3),
    EXPN2(4), EXPN2(5), EXPN2(6), EXPN2(7),
    EXPN2(8), EXPN2(9), EXPN2(10), EXPN2(11),
    EXPN2(12), EXPN2(13), EXPN2(14), EXPN2(15),
    EXPN2(16), EXPN2(17), EXPN2(18), EXPN2(19),
    EXPN2(20), EXPN2(21), EXPN2(22), EXPN2(23),
    EXPN2(24), EXPN2(25), EXPN2(26), EXPN2(27),
    EXPN2(28), EXPN2(29), EXPN2(30), EXPN2(31)
#undef EXPN2
};

uint sdecodeushort(const uchar * p, int format)
{
	int a = p[0], b = p[1];

  return (num_is_lsb(format) ? (b << 8) + a : (a << 8) + b);
}

int sdecodeshort(const uchar * p, int format)
{
	int v = (int)sdecodeushort(p, format);

  return (v & 0x7fff) - (v & 0x8000);
}

long sdecodelong(const uchar * p, int format)
{
	int a = p[0], b = p[1], c = p[2], d = p[3];
  long v = (num_is_lsb(format) ?
	      ((long)d << 24) + ((long)c << 16) + (b << 8) + a :
	      ((long)a << 24) + ((long)b << 16) + (c << 8) + d);

#if arch_sizeof_long > 4
  v = (v ^ 0x80000000L) - 0x80000000L;
#endif
  return v;
}

float sdecodefloat(const uchar * p, int format)
{
	quint32 lnum = (quint32) sdecodelong(p, format);
  float fnum;

#if !arch_floats_are_IEEE
  if (format != num_float_native) 
  {
		int sign_expt = lnum >> 23;
		int expt = sign_expt & 0xff;
		long mant = lnum & 0x7fffff;

		if (expt == 0 && mant == 0)
	    fnum = 0;
		else 
		{
	    mant += 0x800000;
	    fnum = (float)ldexp((float)mant, expt - 127 - 23);
		}
		if (sign_expt & 0x100)
	    fnum = -fnum;
   } 
   else
#endif
	fnum = *(float *)&lnum;
  return fnum;
}

int sdecode_number(const uchar * str, int format, XWPSRef * np)
{
	switch (format & 0x170) 
	{
		case num_int32:
		case num_int32 + 16:
	    if ((format & 31) == 0) 
	    {
				np->value.intval = sdecodelong(str, format);
				return XWPSRef::Integer;
	    } 
	    else 
	    {
				np->value.realval =  (double)sdecodelong(str, format) *  binary_scale[format & 31];
				return XWPSRef::Real;
	    }
	    
		case num_int16:
	    if ((format & 15) == 0) 
	    {
				np->value.intval = sdecodeshort(str, format);
				return XWPSRef::Integer;
	    } 
	    else 
	    {
				np->value.realval =  sdecodeshort(str, format) * binary_scale[format & 15];
				return XWPSRef::Real;
	    }
	    
		case num_float:
	    np->value.realval = sdecodefloat(str, format);
	    return XWPSRef::Real;
	    
		default:
	    return (int)(XWPSError::SyntaxError);
  }
}

XWPSRef::XWPSRef()
{
	setTypeAttrs((ushort)Null, 0); 
	setSize(0); 
	value.intval = 0;
}

XWPSRef::~XWPSRef()
{
	clear();
}

int  XWPSRef::arrayGet(XWPSContextState * ctx, long index_long, XWPSRef * pref)
{
	if ((ulong)index_long >= size())
		return (int)(XWPSError::RangeCheck);
	
	switch (type())
	{
		case Array:
	    {
				XWPSRef *pvalue = getArray() + index_long;
				pref->assign(pvalue);
	    }
	    break;
	    
	  case MixedArray:
	    {
				ushort *packed = getPacked();
				uint index = (uint)index_long;
				for (; index--;)
		    	packed = packed_next(packed);
				ctx->packedGet(packed, pref);
	    }
	    break;
	    
	  case ShortArray:
	    {
				ushort *packed = getPacked() + index_long;
				ctx->packedGet(packed, pref);
	    }
	    break;
	    
		default:
	    return (int)(XWPSError::TypeCheck);
	}
	
	return 0;
}

//要特别注意pfrom可能来自自身，特别是Dictionary和Array!!!
void XWPSRef::assign(XWPSRef * pfrom)
{
	if (pfrom == this)
		return ;

  XWPSRef tmp;
  tmp.tas.type_attrs = pfrom->tas.type_attrs;
	tmp.tas.rsize = pfrom->tas.rsize;
	tmp.value = pfrom->value;
	switch (tmp.type())
	{
		case Dictionary:
			if (tmp.value.pdict)
				tmp.value.pdict->incRef();
			clear();
			value.pdict = tmp.value.pdict;
			tmp.value.pdict = 0;
			break;
			
		case Struct:
		case AStruct:
		case FontID:
			if (tmp.value.pstruct)
				tmp.value.pstruct->incRef();
			clear();
			value.pstruct = tmp.value.pstruct;
			tmp.value.pstruct = 0;
			break;
			
		case Device:
			if (tmp.value.pdevice)
				tmp.value.pdevice->incRef();
			clear();
			value.pdevice = tmp.value.pdevice;
			tmp.value.pdevice = 0;
			break;
			
		case File:
			if (tmp.value.pfile)
				tmp.value.pfile->incRef();
			clear();
			value.pfile = tmp.value.pfile;
			tmp.value.pfile = 0;
			break;
			
		case Array:
				if (tmp.value.refs && tmp.value.refs->arr)
					tmp.value.refs->arr->incRef();
				clear();
				value.refs = new XWPSArrayRef;
				value.refs->arr = tmp.value.refs->arr;
				value.refs->ptr = tmp.value.refs->ptr;
				tmp.value.refs = 0;
			break;
			
		case OpArray:
				if (tmp.value.const_refs && tmp.value.const_refs->arr)
					tmp.value.const_refs->arr->incRef();
					clear();
				value.const_refs = new XWPSArrayRef;
				value.const_refs->arr = tmp.value.const_refs->arr;
				value.const_refs->ptr = tmp.value.const_refs->ptr;
				tmp.value.const_refs = 0;
				break;
			
			case MixedArray:
			case ShortArray:
				if (tmp.value.packed && tmp.value.packed->arr)
					tmp.value.packed->arr->incRef();
				clear();
				value.packed = new XWPSPackedArrayRef;
				value.packed->arr = tmp.value.packed->arr;
				value.packed->ptr = tmp.value.packed->ptr;
				tmp.value.packed = 0;
				break;
				
			case String:
				if (tmp.value.bytes && tmp.value.bytes->arr)
				  tmp.value.bytes->arr->incRef();
				clear();
				value.bytes = new XWPSBytesRef;
				value.bytes->arr = tmp.value.bytes->arr;
				value.bytes->ptr = tmp.value.bytes->ptr;
				tmp.value.bytes = 0;
				break;
				
			case Boolean:
				clear();
				value.boolval = tmp.value.boolval;
				break;
				
			case Integer:
				clear();
				value.intval = tmp.value.intval;
				break;
				
			case Name:
				clear();
				value.pname = tmp.value.pname;
				break;
				
			case Operator:
				clear();
				value.opproc = tmp.value.opproc;
				break;
				
			case Real:
				clear();
				value.realval = tmp.value.realval;
				break;
				
			case Save:
				clear();
				value.saveid = tmp.value.saveid;
				break;
				
			default:
				clear();
				value = tmp.value;
				break;
	}
	
	tas.type_attrs = tmp.tas.type_attrs;
	tas.rsize = tmp.tas.rsize;
}

void XWPSRef::assignRef(XWPSRef * pfrom, uint s, uchar * p)
{
	XWPSRef tmp;
  tmp.tas.type_attrs = pfrom->tas.type_attrs;
	clear();
	tas.type_attrs = tmp.tas.type_attrs;
	tas.rsize = s;
	value.bytes = new XWPSBytesRef(p);
}

int XWPSRef::callProc(XWPSContextState * ctx)
{
	if (value.opproc)
		return (ctx->*value.opproc)();
		
	return 0;
}

int XWPSRef::checkAccess(ushort acc1)
{
	if (!hasAttr(acc1)) 
		return (int)(XWPSError::InvalidAccess);
			
	return 0;
}

int XWPSRef::checkArray()
{
	if (!hasType(Array))
		return checkTypeFailed();
			
	return 0;
}

int XWPSRef::checkArrayOnly()
{
	if (!hasType(Array))
		return (int)(XWPSError::TypeCheck);
			
	return 0;
}

int XWPSRef::checkDictRead()
{
	return dictAccessRef()->checkRead();
}

int XWPSRef::checkDictWrite()
{
	return dictAccessRef()->checkWrite();
}

int XWPSRef::checkExecute()
{
	return checkAccess(PS_A_EXECUTE);
}

int XWPSRef::checkFile(XWPSStream ** svar)
{
	int code = checkType(File);
	if (code < 0)
		return code;
		
	*svar = getStream();
	if (((*svar)->read_id | (*svar)->write_id) != size())
	{
		return (int)(XWPSError::InvalidAccess);
	}
	
	return 0;
}

int XWPSRef::checkIntLEU(ulong u)
{
	int code = checkType(Integer);
	if (code == 0)
	{		
		if ((ulong)value.intval > u) 
		{
			code = (int)(XWPSError::RangeCheck);
		}
	}
	
	return code;	
}

int XWPSRef::checkIntLEUOnly(ulong u)
{
	int code = checkTypeOnly(Integer);
	if (code == 0)
	{
		if ((ulong)value.intval > u) 
			code = (int)(XWPSError::RangeCheck);
	}
	
	return code;
}

int XWPSRef::checkIntLTU(ulong u)
{
	int code = checkType(Integer);
	if (code == 0)
	{
		if ((ulong)value.intval >= u) 
			code = (int)(XWPSError::RangeCheck);
	}
	
	return code;
}

int XWPSRef::checkProc()
{
	if (!isProc())
		return checkProcFailed();
		
	return 0;
}

int XWPSRef::checkProcFailed()
{
	return (int)(isArray() ? XWPSError::InvalidAccess : hasType(Invalid) ? XWPSError::StackUnderflow : XWPSError::TypeCheck);
}

int XWPSRef::checkProcOnly()
{
	return checkProc();
}

int XWPSRef::checkRead()
{
	return checkAccess(PS_A_READ);
}

int XWPSRef::checkReadType(XWPSRef::RefType typ)
{
	return checkTypeAccess(typ,PS_A_READ);
}

int XWPSRef::checkReadTypeOnly(XWPSRef::RefType typ)
{
	return checkTypeAccessOnly(typ, PS_A_READ);
}

int XWPSRef::checkType(const char * n)
{
	if (hasType(Struct) || hasType(AStruct) || hasType(FontID))
	{
		if (value.pstruct)
			return value.pstruct->checkType(n);
	}
	
	return checkTypeFailed();
}

int XWPSRef::checkType(XWPSRef::RefType typ)
{
	if (!hasType(typ)) 
		return checkTypeFailed();
		
	return 0;
}

int XWPSRef::checkTypeAccess(XWPSRef::RefType typ, ushort acc1)
{
	if (!hasTypeAttrs(typ,acc1))
	{
		if (!hasType(typ))
			return checkTypeFailed();
			
		return (int)(XWPSError::InvalidAccess);
	}
	
	return 0;
}

int XWPSRef::checkTypeFailed()
{
	return (hasType(Invalid) ? (int)(XWPSError::StackUnderflow) : (int)(XWPSError::TypeCheck));
}

int XWPSRef::checkTypeAccessOnly(XWPSRef::RefType typ, ushort acc1)
{
	if (!hasTypeAttrs(typ,acc1))
		return (int)((!hasType(typ) ? XWPSError::TypeCheck : XWPSError::InvalidAccess));
		
	return 0;
}

int XWPSRef::checkTypeOnly(XWPSRef::RefType typ)
{
	if (!hasType(typ))
		return (int)(XWPSError::TypeCheck);
			
	return 0;
}

int XWPSRef::checkWrite()
{
	return checkAccess(PS_A_WRITE);
}

int XWPSRef::checkWriteType(XWPSRef::RefType typ)
{
	return checkTypeAccess(typ, PS_A_WRITE);
}

int XWPSRef::checkWriteTypeOnly(XWPSRef::RefType typ)
{
	return checkTypeAccessOnly(typ, PS_A_WRITE);
}

void XWPSRef::clear()
{
	switch (type())
	{
		case Dictionary:
			if (value.pdict)
			{
				if (value.pdict->decRef() == 0)
					delete value.pdict;
				value.pdict = 0;
			}
			break;
						
		case Array:
			if (value.refs)
			{
				if (value.refs->decRef() == 0)
				  delete value.refs;
				value.refs = 0;
			}
			break;
			
		case OpArray:
			if (value.const_refs)
			{
				if (value.const_refs->decRef() == 0)
				  delete value.const_refs;
				value.const_refs = 0;
			}
			break;
						
		case MixedArray:	
		case ShortArray:
			if (value.packed)
			{
				if (value.packed->decRef() == 0)
				  delete value.packed;
				value.packed = 0;
			}
			break;
			
		case Struct:
		case AStruct:
		case FontID:
			if (value.pstruct)
			{
				if (value.pstruct->decRef() == 0)
					delete value.pstruct;
				value.pstruct = 0;
			}
			break;
						
		case String:
			if (value.bytes)
			{
				if (value.bytes->decRef() == 0)
				  delete value.bytes;
				value.bytes = 0;
			}
			break;
			
		case Device:
			if (value.pdevice)
			{
				if (value.pdevice->decRef() == 0)
					delete value.pdevice;
				value.pdevice = 0;
			}
			break;
			
		case File:
			if (value.pfile)
			{
				if (value.pfile->decRef() == 0)
					delete value.pfile;
				value.pfile = 0;
			}
			break;
			
		default:
			break;
	}
}

int  XWPSRef::cpyToOld(uint index, XWPSRef * from, uint size)
{
	XWPSRef *to = getArray() + index;
	if (to <= from || from + size <= to)
		while (size--)
		{
			to->assign(from);
			to++;
			from++;
		}
	else
		for (from += size, to += size; size--;)
		{
			from--;
			to--;
			to->assign(from);
		}
		
	return 0;
}

XWPSRef * XWPSRef::dictAccessRef()
{
	return getDict()->accessRef();
}

int  XWPSRef::dictAlloc(uint attr, uint size)
{
	clear();
	setTypeAttrs((ushort)Dictionary, attr | PS_A_ALL);
	value.pdict = new XWPSDict;
	value.pdict->createContents(size, true);
	return 0;
}

int  XWPSRef::dictBoolParam(XWPSContextState * ctx, 
	                         const char *kstr,
		                        bool defaultval, 
		                        bool * pvalue)
{
	XWPSRef *pdval;

  if (dictFindString(ctx, kstr, &pdval) <= 0) 
  {
		*pvalue = defaultval;
		return 1;
  }
  
  if (!pdval->hasType(Boolean))
		return pdval->checkTypeFailed();
  *pvalue = pdval->value.boolval;
  return 0;
}

int  XWPSRef::dictCheckUidParam(XWPSContextState * ctx, XWPSUid * puid)
{
	XWPSRef *puniqueid;

  if (puid->isXUID()) 
  {
		uint size = puid->XUIDSize();
		uint i;

		if (dictFindString(ctx, "XUID", &puniqueid) <= 0)
	    return false;
		if (!puniqueid->hasType(Array) ||  puniqueid->size() != size )
	    return false;
		for (i = 0; i < size; i++) 
		{
	    XWPSRef *pvalue = puniqueid->getArray() + i;

	    if (!pvalue->hasType(Integer))
				return false;
	    if (pvalue->value.intval != puid->XUIDValues()[i])
				return false;
		}
		return true;
  } 
  else 
  {
		if (dictFindString(ctx, "UniqueID", &puniqueid) <= 0)
	    return false;
		return (puniqueid->hasType(Integer) &&	puniqueid->value.intval == puid->id);
  }
}

int XWPSRef::dictCopy(XWPSContextState * ctx, 
	                          XWPSRef * pdrfrom,
	                          XWPSDictStack * pds)
{
	return dictCopyEntries(ctx, pdrfrom, false, pds);
}

int XWPSRef::dictCopyEntries(XWPSContextState * ctx, 
	                           XWPSRef * pdrfrom, 
	                           bool new_only,
	                           XWPSDictStack * pds)
{
	int index = 0;
	XWPSRef elt[2];
	ushort s = space();
	int code = 0;
	XWPSRef *pvslot = 0;
	if (s != PS_AVM_MAX)
	{
		index = pdrfrom->dictFirst();
		while ((index = pdrfrom->dictNext(ctx, index, elt)) >= 0)
		{
	    if (!new_only || dictFind(ctx, &elt[0], &pvslot) <= 0) 
	    {
				code = elt[0].storeCheckSpace(s);
				if (code < 0)
					return code;
					
				code = elt[1].storeCheckSpace(s);
				if (code < 0)
					return code;
			}
		}
	}
	
	index = pdrfrom->dictFirst();
	while ((index = pdrfrom->dictNext(ctx, index, elt)) >= 0) 
	{
		if (new_only && dictFind(ctx, &elt[0], &pvslot) > 0)
	    continue;
		if ((code = dictPut(ctx, &elt[0], &elt[1], pds)) < 0)
	    return code;
  }
  return 0;
}

int  XWPSRef::dictCopyNew(XWPSContextState * ctx, 
	                          XWPSRef * pdrfrom, 
	                          XWPSDictStack * pds)
{
	return dictCopyEntries(ctx, pdrfrom, true, pds);
}

int  XWPSRef::dictCreateContents(uint size, bool pack)
{
	return getDict()->createContents(size, pack);
}

int XWPSRef::dictCreateUnpackedKeys(uint asize)
{
	return getDict()->createUnpackedKeys(asize);
}

int XWPSRef::dictFind(XWPSContextState * ctx, XWPSRef * pkey, XWPSRef ** ppvalue)
{
	uint nidx;
	ushort kpack;
	uint hash;
	int ktype;
	switch (pkey->type())
	{
		case Name:
			nidx = ctx->nameIndex(pkey);
nh:
			hash = dict_name_index_hash(nidx);
			kpack = packed_name_key(nidx);
			ktype = Name;
			break;
			
		case String:
			if (!pkey->hasAttr(PS_A_READ))
				return (int)(XWPSError::InvalidAccess);
			{
				XWPSRef nref;
				int code = ctx->nameRef((const uchar*)(pkey->getBytes()), pkey->size(), &nref, 1);
	    	if (code < 0)
					return code;
	   	 	nidx = ctx->nameIndex(&nref);
	   	 	goto nh;
			}
			break;
			
		case Real:
			{
				int expt = 0;
	    	double mant = frexp(pkey->value.realval, &expt);
	    	if (expt < sizeof(long) * 8 || pkey->value.realval == min_long)
					hash = (uint)(int)pkey->value.realval * 30503;
	    	else
					hash = (uint)(int)(mant * min_long) * 30503;
					
				goto ih;
			}
			break;
			
		case Integer:
			hash = (uint)pkey->value.intval * 30503;
ih:
			kpack = packed_key_impossible;
			ktype = -1;
			nidx = 0;
			break;
			
		case Null:
			return (int)(XWPSError::TypeCheck);
			break;
			
		default:
			hash = pkey->btype() * 99;
			kpack = packed_key_impossible;
			ktype = -1;
			nidx = 0;
			break;
	}
	
	return getDict()->search(ctx, pkey, nidx, kpack, hash, ktype, ppvalue);
}

int XWPSRef::dictFindPassword(XWPSContextState * ctx, const char *kstr, XWPSRef ** ppvalue)
{
	XWPSRef *pvalue;
  if (dictFindString(ctx, kstr, &pvalue) <= 0)
		return (int)(XWPSError::Undefined);
    
  uchar * buf = pvalue->getBytes();
  if (!pvalue->hasType(String) ||
			pvalue->hasAttrs(PS_A_READ) ||
			buf[0] >= pvalue->size())
		return (int)(XWPSError::RangeCheck);
    
  *ppvalue = pvalue;
  return 0;
}

int XWPSRef::dictFindString(XWPSContextState * ctx, const char *kstr, XWPSRef ** ppvalue)
{
	XWPSRef kname;
	int code = ctx->nameRef((const uchar *)kstr, strlen(kstr), &kname, -1);
	if (code < 0)
		return code;
		
	return dictFind(ctx, &kname, ppvalue);
}

int  XWPSRef::dictFirst()
{
	return getDict()->nslots();
}

int  XWPSRef::dictFloatArrayCheckParam(XWPSContextState * ctx, 
	                                   const char *kstr,
			                               uint len, 
			                               float *fvec, 
			                               const float *defaultvec,
			                               int under_error, 
			                               int over_error)
{
	XWPSRef *pdval;
  uint size;
  int code;

  if (dictFindString(ctx, kstr, &pdval) <= 0) 
  {
		if (defaultvec == NULL)
	    return 0;
		memcpy(fvec, defaultvec, len * sizeof(float));

		return len;
  }
  
  if (!pdval->hasType(Array))
		return pdval->checkTypeFailed();
  size = pdval->size();
  if (size > len)
		return (over_error);
		
	XWPSRef * a = pdval->getArray() + size - 1;
  code = ctx->floatParams(a, size, fvec);
  return (code < 0 ? code :  size == len || under_error >= 0 ? size :  (under_error));
}

int  XWPSRef::dictFloatArrayParam(XWPSContextState * ctx, 
	                              const char *kstr,
		                             uint maxlen, 
		                             float *fvec, 
		                             const float *defaultvec)
{
	return dictFloatArrayCheckParam(ctx, kstr, maxlen, fvec,	defaultvec, 0, XWPSError::LimitCheck);
}

int  XWPSRef::dictFloatParam(XWPSContextState * ctx, 
	                         const char *kstr,
		                       float defaultval, 
		                       float *pvalue)
{
	XWPSRef *pdval;

  if (dictFindString(ctx, kstr, &pdval) <= 0) 
  {
		*pvalue = defaultval;
		return 1;
  }
  
  switch (pdval->type()) 
  {
		case Integer:
	    *pvalue = pdval->value.intval;
	    return 0;
	    
		case Real:
	    *pvalue = pdval->value.realval;
	    return 0;
  }
  return checkTypeFailed();
}

int  XWPSRef::dictFloatsParam(XWPSContextState * ctx, 
	                          const char *kstr,
		                        uint maxlen, 
		                        float *fvec, 
		                        const float *defaultvec)
{
	return dictFloatArrayCheckParam(ctx, kstr, maxlen, fvec, defaultvec,
					XWPSError::RangeCheck, XWPSError::RangeCheck);
}

int XWPSRef::dictGrow(XWPSContextState * ctx, XWPSDictStack *pds)
{
	ulong new_size = (ulong) dictLength() * 3 / 2 + 2;
#if arch_sizeof_int < arch_sizeof_long
    if (new_size > max_uint)
	new_size = max_uint;
#endif
	if (new_size > getDict()->npairs())
	{
		int code = dictResize(ctx, (uint) new_size, pds);
		if (code >= 0)
	    return code;
		if (getDict()->npairs() < XWPSDict::getMaxSize()) 
		{
	    code = dictResize(ctx, XWPSDict::getMaxSize(), pds);
	    if (code >= 0)
				return code;
		}
		if (getDict()->npairs() == dictMaxLength()) 
	    return code;
		new_size = getDict()->npairs();
	}

	getDict()->setMaxLength(new_size);
  return 0;
}

int  XWPSRef::dictIntArrayCheckParam(XWPSContextState * ctx, 
	                                 const char *kstr, 
	                                 uint len,
			                             int *ivec, 
			                             int under_error, 
			                             int over_error)
{
	XWPSRef *pdval;
  XWPSRef *pa;
  int *pi = ivec;
  uint size;
  int i;

  if (dictFindString(ctx, kstr, &pdval) <= 0)
		return 0;
  
  if (!pdval->hasType(Array))
		return pdval->checkTypeFailed();
  size = pdval->size();
  if (size > len)
		return (over_error);
  pa = pdval->getArray();
  for (i = 0; i < size; i++, pa++, pi++) 
  {
		switch (pa->type()) 
		{
	    case Integer:
				if (pa->value.intval != (int)pa->value.intval)
				{
	    		return (int)(XWPSError::RangeCheck);
		    }
				*pi = (int)pa->value.intval;
				break;
				
	    case Real:
				if (pa->value.realval < min_int ||
		    		pa->value.realval > max_int ||
		    		pa->value.realval != (int)pa->value.realval)
		    {
	    		return (int)(XWPSError::RangeCheck);
		    }
				*pi = (int)pa->value.realval;
				break;
				
	    default:
				return checkTypeFailed();
		}
  }
  return (size == len || under_error >= 0 ? size : under_error);
}

int XWPSRef::dictIntArrayParam(XWPSContextState * ctx, 
	                            const char *kstr,
		                          uint maxlen, 
		                          int *ivec)
{
	return dictIntArrayCheckParam(ctx, kstr, maxlen, ivec, 0, XWPSError::LimitCheck);
}

int  XWPSRef::dictIntNullParam(XWPSContextState * ctx,
	                           const char *kstr, 
	                           int minval,
		                         int maxval, 
		                         int defaultval, 
		                         int *pvalue)
{
	XWPSRef *pdval;
  int code;
  long ival;

  if (dictFindString(ctx, kstr, &pdval) <= 0) 
  {
		ival = defaultval;
		code = 1;
  } 
  else 
  {
		switch (pdval->type()) 
		{
	    case Integer:
				ival = pdval->value.intval;
				break;
				
	    case Real:
				if (pdval->value.realval < minval || pdval->value.realval > maxval)
				{
	    		return (int)(XWPSError::RangeCheck);
		    }
				ival = (long)pdval->value.realval;
				if (ival != pdval->value.realval)
				{
	    		return (int)(XWPSError::RangeCheck);
		    }
				break;
				
	    case Null:
				return 2;
				
	    default:
				return pdval->checkTypeFailed();
		}
		code = 0;
  }
  if (ival < minval || ival > maxval)
  {
	  return (int)(XWPSError::RangeCheck);
	}
  *pvalue = (int)ival;
  return code;
}

int  XWPSRef::dictIntParam(XWPSContextState * ctx, 
	                        const char *kstr, 
	                        int minval, 
	                        int maxval,
	                        int defaultval, 
	                        int *pvalue)
{
	int code = dictIntNullParam(ctx, kstr, minval, maxval,  defaultval, pvalue);

  return (code == 2 ? checkTypeFailed() : code);
}

int XWPSRef::dictIntsParam(XWPSContextState * ctx, 
	                        const char *kstr,	
	                        uint len, 
	                        int *ivec)
{
	return dictIntArrayCheckParam(ctx, kstr, len, ivec, XWPSError::RangeCheck, XWPSError::RangeCheck);
}

bool XWPSRef::dictIsPacked()
{
	return getDict()->isPacked();
}

uint XWPSRef::dictLength()
{
	return getDict()->getLength();
}

int XWPSRef::dictMatrixParam(XWPSContextState * ctx, 
	                          const char *kstr, 
	                          XWPSMatrix * pmat)
{
	XWPSRef *pdval;

  if (dictFindString(ctx, kstr, &pdval) <= 0)
		return checkTypeFailed();
    
  return pdval->readMatrix(ctx, pmat);
}

uint XWPSRef::dictMaxIndex()
{
	return getDict()->npairs() - 1;
}

uint XWPSRef::dictMaxLength()
{
	return getDict()->getMaxLength();
}

int XWPSRef::dictNext(XWPSContextState * ctx, int index, XWPSRef * eltp)
{
	XWPSRef *vp =getDict()->getValues() + index;
  while (vp--, --index >= 0) 
  {
		getDict()->keys.arrayGet(ctx, (long)index, eltp);
		if (eltp->hasType(Name) || (!(getDict()->isPacked()) && !(eltp->hasType(Null)))) 
		{
	    eltp[1].assign(vp);
	    return index;
		}
  }
  return -1;
}

int  XWPSRef::dictProcParam(XWPSContextState * ctx, 
	                        const char *kstr, 
	                        XWPSRef * pproc,
		                      bool defaultval)
{
	XWPSRef *pdval;

  if (dictFindString(ctx, kstr, &pdval) <= 0) 
  {
		if (defaultval)
	    pproc->makeArray(PS_A_READONLY + PS_A_EXECUTABLE);
		else
	    pproc->makeNull();
		return 1;
  }
  pdval->checkProc();
  pproc->assign(pdval);
  return 0;
}

int  XWPSRef::dictPut(XWPSContextState * ctx, 
	                    XWPSRef * pkey, 
	                    XWPSRef * pvalue,
	                    XWPSDictStack * pds)
{
	int rcode = 0;
	int code = storeCheckDest(pvalue);
	if (code < 0)
		return code;
		
	XWPSRef *pvslot = 0;
	
top:
	code = dictFind(ctx, pkey, &pvslot);
	if (code <= 0)
	{
		switch (code)
		{
			case 0:
				break;
				
			case XWPSError::DictFull:
				if (!ctx->isDictAutoExpand())
		    	return (int)(XWPSError::DictFull);
				code = dictGrow(ctx, pds);
				if (code < 0)
		    	return code;
				goto top;
				break;
				
			default:
				return code;
				break;
		}
		
		uint index = pvslot - getDict()->getValues();
		XWPSRef kname;
		if (pkey->hasType(String))
		{
			if (!pkey->hasAttr(PS_A_READ))
				return (int)(XWPSError::InvalidAccess);
	    code = ctx->nameFromString(pkey, &kname);
	    if (code < 0)
				return code;
	    pkey = &kname;
		}
		
		if (getDict()->isPacked())
		{
			if (!pkey->hasType(Name) ||	ctx->nameIndex(pkey) > packed_name_max_index) 
			{
				code = dictUnpack(ctx, pds);
				if (code < 0)
		    	return code;
				goto top;
	    }
	    ushort * kp = getDict()->getKeysPacked() + index;
	    *kp = pt_tag(pt_literal_name) + ctx->nameIndex(pkey);
		}
		else
		{
			XWPSRef *kp = getDict()->getKeys() + index;
			code = storeCheckDest(pkey);
			if (code < 0)
				return code;
			kp->assign(pkey);
		}
		
		getDict()->count.value.intval++;
		
		if (pkey->hasType(Name))
		{
			XWPSName *pname = pkey->value.pname;
			if (pname->pvalue == 0 &&	pds && pds->dictIsPermanent(this))
				pname->pvalue = pvslot;
			else
				pname->pvalue = (XWPSRef*)1;
		}
		
		rcode = 1;
	}
	
	if (pvslot)
		pvslot->assign(pvalue);
	
	return rcode;
}

int  XWPSRef::dictPutString(XWPSContextState * ctx, 
	                          const char *kstr, 
	                          XWPSRef * pvalue,
		                        XWPSDictStack * pds)
{
	XWPSRef kname;
	int code = ctx->nameRef((const uchar *)kstr, strlen(kstr), &kname, 0);
	if (code < 0)
		return code;
		
	return dictPut(ctx, &kname, pvalue, pds);
}

int XWPSRef::dictReadPassword(XWPSContextState * ctx, const char *pkey, PSPassword * ppass)
{
	XWPSRef *pvalue;
  int code = dictFindPassword(ctx, pkey, &pvalue);
  if (code < 0)
		return code;

	uchar * p = pvalue->getBytes();
  if (p[0] > MAX_PASSWORD)
		return (int)(XWPSError::RangeCheck);
  memcpy(ppass->data, p + 1, (ppass->size = p[0]));
  return 0;
}

int  XWPSRef::dictResize(XWPSContextState * ctx, uint new_size, XWPSDictStack *pds)
{
	if (new_size < dictLength()) 
	{
		if (!ctx->isDictAutoExpand())
	    return (int)(XWPSError::DictFull);
		new_size = dictLength();
  }
  
  XWPSRef drto;  
  XWPSDict * ndict = new XWPSDict;
  drto.makeDict(space() | PS_A_ALL, ndict);
  drto.setSpace(PS_AVM_LOCAL);
  drto.dictCreateContents(new_size, dictIsPacked());  
  drto.dictCopy(ctx, this, pds);
  ndict->setMaxLength(new_size);
  value.pdict->values.assign(&(ndict->values));
  value.pdict->keys.assign(&(ndict->keys));
  value.pdict->count.assign(&(ndict->count));
  value.pdict->maxlength.assign(&(ndict->maxlength));
  if (pds)
		pds->setTop();
		
	return 0;
}

int XWPSRef::dictUidParam(XWPSUid * puid, 
	                       int defaultval,
	                       XWPSContextState *ctx)
{
	XWPSRef *puniqueid;

  if (ctx->level2Enabled() &&	dictFindString(ctx, "XUID", &puniqueid) > 0) 
  {
		long *xvalues;
		uint size, i;

		if (!puniqueid->hasType(Array))
	    return checkTypeFailed();
		size = puniqueid->size();
		if (size == 0)
		{
	    return (int)(XWPSError::RangeCheck);
	  }
		xvalues = new long[size];
		for (i = 0; i < size; i++) 
		{
	    XWPSRef *pvalue = puniqueid->getArray() + i;

	    if (!pvalue->hasType(Integer)) 
	    {
	    	delete [] xvalues;
				return checkTypeFailed();
	    }
	    xvalues[i] = pvalue->value.intval;
		}
		puid->setXUID(xvalues, size);
		return 1;
  }
  
  if (dictFindString(ctx, "UniqueID", &puniqueid) <= 0) 
		return defaultval;
  else 
  {
		if (!puniqueid->hasType(Integer) ||
	    		puniqueid->value.intval < 0 ||
	    		puniqueid->value.intval > 0xffffffL )
	  {
	    return (int)(XWPSError::RangeCheck);
	  }
	  
		if (puniqueid->value.intval == 0) 
		{
	    return defaultval;
		} 
		else
	    puid->setUniqueID(puniqueid->value.intval);
  }
  return 0;
}

int  XWPSRef::dictUintParam(XWPSContextState * ctx, 
	                          const char *kstr,
		                        uint minval, 
		                        uint maxval, 
		                        uint defaultval, 
		                        uint * pvalue)
{
	XWPSRef *pdval;
  int code;
  uint ival;

  if (dictFindString(ctx, kstr, &pdval) <= 0) 
  {
		ival = defaultval;
		code = 1;
  } 
  else 
  {
		code = pdval->checkTypeOnly(Integer);
		if (code < 0)
			return code;
			
		if (pdval->value.intval != (uint) pdval->value.intval)
		{
	    return (int)(XWPSError::RangeCheck);
	  }
		ival = (uint) pdval->value.intval;
		code = 0;
  }
  if (ival < minval || ival > maxval)
  {
	  return (int)(XWPSError::RangeCheck);
	}
  *pvalue = ival;
  return code;
}

int  XWPSRef::dictUndef(XWPSContextState * ctx, XWPSRef * pkey, XWPSDictStack *)
{
	XWPSRef *pvslot = 0;
	if (dictFind(ctx, pkey, &pvslot) <= 0)
	{
		return (int)(XWPSError::Undefined);
	}
			
	uint index = pvslot - getDict()->getValues();
	if (getDict()->isPacked())
	{
		ushort *pkp = getDict()->getKeysPacked() + index;
		if (pkp[-1] == packed_key_empty)
		{
			uint end = getDict()->nslots();
	    *pkp = packed_key_empty;
	    while (++index < end && *++pkp == packed_key_deleted)
				*pkp = packed_key_empty;
		}
		else
			*pkp = packed_key_deleted;
	}
	else
	{
		XWPSRef *kp = getDict()->getKeys() + index;
		kp->makeNull();
		if (!((kp - 1)->hasType(Null)) ||	((kp - 1)->hasAttr(PS_A_EXECUTABLE)))
	    kp->setAttrs(PS_A_EXECUTABLE);
	}
	
	getDict()->count.value.intval--;
	if (pkey->hasType(Name))
	{
		XWPSName *pname = pkey->value.pname;
		if (pname->isValid())
			pname->pvalue = 0;
	}
	
	if (pvslot)
		pvslot->makeNull();
	
	return 0;
}

int  XWPSRef::dictUnpack(XWPSContextState * ctx, XWPSDictStack * pds)
{
	return getDict()->unpack(ctx, pds);
}

int  XWPSRef::dictValueIndex(XWPSRef * pvalue)
{
	return getDict()->valueIndex(pvalue);
}

int XWPSRef::dictWritePassword(XWPSContextState * ctx, const PSPassword * ppass, const char *pkey)
{
	XWPSRef *pvalue;
  int code = dictFindPassword(ctx, pkey, &pvalue);
  if (code < 0)
		return code;
		
  if (ppass->size >= pvalue->size())
		return (int)(XWPSError::RangeCheck);
    
  uchar * p = pvalue->getBytes();
  memcpy(p + 1, ppass->data, (p[0] = ppass->size));
  return 0;
}

int  XWPSRef::fileClose()
{
	XWPSStream *s = getStream();
	if ((s->read_id | s->write_id) == size()) 
	{	
		if (s->close())
		{
	    return (int)(XWPSError::IOError);
	  }
  }
  return 0;
}

bool XWPSRef::fileIsInvalid(XWPSStream ** s)
{
	*s = getStream();
	return ((*s)->read_id | (*s)->write_id) != size();
}

bool XWPSRef::fileIsValid(XWPSStream ** s)
{
	*s = getStream();
	return ((*s)->read_id | (*s)->write_id) == size();
}

int XWPSRef::floatMatrix(int count, XWPSMatrix *pmat)
{
	XWPSRef * op = this;
	double * pval = &(pmat->xx);
	for (pval += count; --count >= 0; --op)
		switch (op->type()) 
		{
	    case Real:
				*--pval = op->value.realval;
				break;
				
	    case Integer:
				*--pval = op->value.intval;
				break;
				
	    case Invalid:
				return (int)(XWPSError::StackUnderflow);
				
	    default:
				return (int)(XWPSError::TypeCheck);
		}
  return 0;
}

int XWPSRef::floatParam(float *pparam)
{
	double dval;
  int code = realParam(&dval);
  if (code >= 0)
		*pparam = (float)dval;
  return code;
}

XWPSRef  * XWPSRef::getArray()
{
	switch (type())
	{
		case Array:
			if (value.refs)
				return value.refs->ptr;
			break;
			
		case OpArray:
			if (value.const_refs)
				return value.const_refs->ptr;
			break;
			
		case MixedArray:
		case ShortArray:
			if (value.packed)
				return (XWPSRef*)(value.packed->ptr);
			break;
			
		default:
			break;
	}
		
	return 0;
}

uchar * XWPSRef::getBytes()
{
	if (value.bytes)
		return value.bytes->ptr;
		
	return 0;
}

ushort  * XWPSRef::getPacked()
{
	switch (type())
	{
		case Array:
			if (value.refs)
				return (ushort*)(value.refs->ptr);
			break;
			
		case OpArray:
			if (value.const_refs)
				return (ushort*)(value.const_refs->ptr);
			break;
			
		case MixedArray:
		case ShortArray:
			if (value.packed)
				return value.packed->ptr;
			break;
			
		default:
			break;
	}
		
	return 0;
}

ushort * XWPSRef::getWritablePacked()
{
	if (value.writable_packed)
		return value.writable_packed->ptr;
		
	return 0;
}

const char * XWPSRef::getTypeName()
{
	switch (type())
	{
		case Dictionary:
			return "dicttype";
			break;
						
		case Array:
		case UnusedArray:
			return "arraytype";
			break;
				
		case OpArray:
		case Operator:
			return "operatortype";
			break;
			
		case MixedArray:
		case ShortArray:
			return "packedarraytype";
			break;
			
		case Struct:
		case AStruct:
			if (value.pstruct)
				return value.pstruct->getTypeName();
			return "struct";
			break;
			
		case FontID:
			return "fonttype";
			break;
						
		case String:
			return "stringtype";
			break;
			
		case Device:
			return "devicetype";
			break;
			
		case File:
			return "filetype";
			break;
				
		case Boolean:
			return "booleantype";
			break;
				
		case Integer:
			return "integertype";
			break;
				
		case Name:
			return "nametype";
			break;
				
		case Real:
			return "realtype";
			break;
				
		case Save:
			return "savetype";
			break;
			
		case Mark:
			return "marktype";
			break;
			
		case Null:
			return "nulltype";
			break;
			
		default:
			break;
	}
	
	return "nulltype";
}

int  XWPSRef::intParam(int max_value, int *pparam)
{
	checkIntLEU(max_value);
  *pparam = (int)value.intval;
  return 0;
}

bool XWPSRef::isForeign()
{
	return space() == PS_AVM_FOREIGN;
}

bool XWPSRef::isLocal()
{
	return space() == PS_AVM_LOCAL;
}

void XWPSRef::makeArray(ushort attrs, ushort size, XWPSRef * elts)
{
	clear();
	value.refs = new XWPSArrayRef(elts);
	setTypeAttrs((ushort)Array, attrs); 
	setSize(size);
}

void XWPSRef::makeArray(ushort attrs, ushort size)
{
	clear();
	setTypeAttrs((ushort)Array, attrs); 
	setSize(size);
	value.refs = new XWPSArrayRef(size);
}

void XWPSRef::makeArray(ushort attrs)
{
	clear();
	setTypeAttrs((ushort)Array, attrs); 
	setSize(0);
	value.refs = new XWPSArrayRef();
}

void XWPSRef::makeAStruct(ushort attrs, XWPSStruct * ptr)
{
	if ((type() != AStruct) || (value.pstruct != ptr))
		clear();
	setTypeAttrs((ushort)AStruct, attrs);
	value.pstruct = ptr;
}

void XWPSRef::makeAStruct(ushort attrs, ushort size)
{
	clear();
	setTypeAttrs((ushort)AStruct, attrs);
	value.pstruct = new XWPSBytes(size);
}

void XWPSRef::makeDevice(ushort attrs, XWPSDevice * ptr)
{
	if ((type() != Device) || (value.pdevice != ptr))
		clear();
	setTypeAttrs((ushort)Device, attrs);
	value.pdevice = ptr;
}

void XWPSRef::makeDict(ushort attrs, XWPSDict * dic)
{
	if ((type() != Dictionary) || (value.pdict != dic))
	  clear();
	setTypeAttrs((ushort)Dictionary, attrs);
	value.pdict = dic;
}

void XWPSRef::makeFile(ushort attrs, ushort id, XWPSStream * s)
{
	if ((type() != File) || (value.pfile != s))
	  clear();
	setTypeAttrs((ushort)File, attrs); 
	setSize(id); 
	value.pfile = s;
}

void XWPSRef::makeFontID(ushort attrs, XWPSStruct * ptr)
{
	if ((type() != FontID) || (value.pstruct != ptr))
	  clear();
	setTypeAttrs((ushort)FontID, attrs); 
	value.pstruct = ptr;
}

void XWPSRef::makeInt(ushort attrs, uint ival)
{
	clear();
	setTypeAttrs((ushort)Integer, attrs); 
	value.intval = ival;
}

void XWPSRef::makeMarkEStack(ushort opidx, op_proc_t proc)
{
	clear();
	setTypeAttrs((ushort)Null, PS_A_EXECUTABLE); 
	setSize(opidx); 
	value.opproc = proc;
}

void XWPSRef::makeName(ushort nidx, XWPSName * pnm)
{
	if ((type() != Name) || (value.pname != pnm))
	  clear();
	setTypeAttrs((ushort)Name, PS_AVM_SYSTEM); 
	setSize(nidx); 
	value.pname = pnm;
}

void XWPSRef::makeOpArray(ushort attrs, ushort size, XWPSRef * elts)
{
	clear();
	value.const_refs = new XWPSArrayRef(elts);
		
	setTypeAttrs((ushort)OpArray, attrs); 
	setSize(size);
}

void XWPSRef::makeOper(ushort opidx, op_proc_t proc)
{
	clear();
	setTypeAttrs((ushort)Operator, PS_A_EXECUTABLE); 
	setSize(opidx); 
	value.opproc = proc;
}

void XWPSRef::makeOper(ushort opidx, ushort attrs, op_proc_t proc)
{
	clear();
	setTypeAttrs((ushort)Operator, attrs); 
	setSize(opidx); 
	value.opproc = proc;
}

void XWPSRef::makeOper(ushort t, ushort opidx, ushort attrs, op_proc_t proc)
{
	clear();
	setTypeAttrs(t, attrs); 
	setSize(opidx); 
	value.opproc = proc;
}

void XWPSRef::makeSave(ushort attrs, ulong sid)
{
	clear();
	setTypeAttrs((ushort)Save, attrs); 
	value.saveid = sid;
}

void XWPSRef::makeShortArray(ushort attrs, uint ksize, uint asize)
{
	clear();
	setTypeAttrs((ushort)ShortArray, attrs); 
	value.packed = new XWPSPackedArrayRef(ksize, asize);
	setSize(asize); 
}

void XWPSRef::makeStreamFile(XWPSStream * s, const char *access)
{
	if ((type() != File) || (value.pfile != s))
		clear();
	value.pfile = s;
	uint attrs = access[1] == '+' ? PS_A_WRITE + PS_A_READ + PS_A_EXECUTE : 0;
	if (access[0] == 'r')
	{
		setTypeAttrs((ushort)File, attrs | (PS_A_READ | PS_A_EXECUTE)); 
	  setSize(s->read_id);
	  s->write_id = 0;
	}
	else
	{
		setTypeAttrs((ushort)File, attrs | PS_A_WRITE); 
	  setSize(s->write_id);
	  s->read_id = 0;
	}
}

void XWPSRef::makeString(ushort attrs, ushort size, uchar * chars)
{
	clear();
	XWPSBytes * b = new XWPSBytes;
	b->arr = (uchar*)malloc((size+1)*sizeof(uchar));
	b->length = size;
	b->self = true;
	memcpy(b->arr, chars,size);
	value.bytes = new XWPSBytesRef();
	value.bytes->arr = b;
	value.bytes->ptr = b->arr;
		
	setTypeAttrs((ushort)String, attrs); 
	setSize(size);
}

void XWPSRef::makeString(ushort attrs, ushort size)
{
	clear();
	setTypeAttrs((ushort)String, attrs); 
	setSize(size);
	value.bytes = new XWPSBytesRef(size);
}

void XWPSRef::makeString(ushort attrs)
{
	clear();
	setTypeAttrs((ushort)String, attrs); 
	setSize(0);
	value.bytes = new XWPSBytesRef;
}

void XWPSRef::makeStringSta(ushort attrs, ushort size, uchar * chars)
{
	value.bytes = new XWPSBytesRef(chars);
	setTypeAttrs((ushort)String, attrs); 
	setSize(size);
}

void XWPSRef::makeStruct(ushort attrs, XWPSStruct * ptr)
{
	if ((type() != Struct) || (value.pstruct != ptr))
	  clear();
	value.pstruct = ptr;
	setTypeAttrs((ushort)Struct, attrs);
}

int XWPSRef::numArrayFormat()
{
	switch (type()) 
	{
		case String:
	    {
				uchar *bp = getBytes();
				int format;

				if (size() < 4 || bp[0] != bt_num_array_value)
		    	return (int)(XWPSError::RangeCheck);
				format = bp[1];
				if (!num_is_valid(format) || 
					  sdecodeshort(bp + 2, format) != (size() - 4) / encoded_number_bytes(format))
				{
		    	return (int)(XWPSError::RangeCheck);
		    }
				return format;
	    }
	    
		case Array:
		case MixedArray:
		case ShortArray:
	    return num_array;
	    
		default:
	    return checkTypeFailed();
  }
}

int XWPSRef::numArrayGet(XWPSContextState * ctx,
	                  int format, 
	                  uint index, 
	                  XWPSRef * np)
{
	if (format == num_array) 
	{
		int code = arrayGet(ctx, (long)index, np);

		if (code < 0)
	    return Null;
		switch (np->type()) 
		{
	    case Integer:
				return Integer;
				
	    case Real:
				return Real;
				
	    default:
				return (int)(XWPSError::RangeCheck);
		}
  } 
  else 
  {
		uint nbytes = encoded_number_bytes(format);

		if (index >= (size() - 4) / nbytes)
	    return Null;
		return sdecode_number(getBytes() + 4 + index * nbytes, format, np);
  }
}

uint XWPSRef::numArraySize(int format)
{
	return (format == num_array ? size() : (size() - 4) / encoded_number_bytes(format));
}

bool XWPSRef::objEq(XWPSContextState * ctx, XWPSRef * pref2)
{
	XWPSRef * pref1 = this;
	XWPSRef nref;
	if (pref1->type() != pref2->type())
	{		
		switch (pref1->type())
		{
			case Integer:
				return (pref2->hasType(Real) &&	pref2->value.realval == pref1->value.intval);
				
	    case Real:
				return (pref2->hasType(Integer) &&	pref2->value.intval == pref1->value.realval);
				
	    case Name:
				if (!pref2->hasType(String))
		    	return false;
				ctx->nameStringRef(pref1, &nref);
				pref1 = &nref;
				break;
				
	    case String:
				if (!pref2->hasType(Name))
		    	return false;
				ctx->nameStringRef(pref2, &nref);
				pref2 = &nref;
				break;
				
	    default:
				if (pref1->btype() != pref2->btype())
		    	return false;
		}
	}
	
	switch (pref1->btype()) 
	{
		case Array:
	    return (pref1->getArray() == pref2->getArray() &&  pref1->size() == pref2->size());
	    
		case MixedArray:
		case ShortArray:
	    return (pref1->getPacked() == pref2->getPacked() && pref1->size() == pref2->size());
	    
		case Boolean:
	    return (pref1->value.boolval == pref2->value.boolval);
	    
		case Dictionary:
	    return (pref1->value.pdict == pref2->value.pdict);
	    
		case File:
	    return (pref1->value.pfile == pref2->value.pfile && pref1->size() == pref2->size());
	    
		case Integer:
	    return (pref1->value.intval == pref2->value.intval);
	    
		case Mark:
		case Null:
	    	return true;
	    	
		case Name:
	    return (pref1->value.pname == pref2->value.pname);
	    
		case OpArray:
		case Operator:
	    return (ctx->opIndex(pref1) == ctx->opIndex(pref2));
	    
		case Real:
	    return (pref1->value.realval == pref2->value.realval);
	    
		case String:
	    return (!bytes_compare(pref1->getBytes(), pref1->size(), pref2->getBytes(), pref2->size()));
	    
		case Device:
	    return (pref1->value.pdevice == pref2->value.pdevice);
	    
		case Struct:
		case AStruct:
	    return (pref1->value.pstruct == pref2->value.pstruct);
	    
		case FontID:
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

int XWPSRef::readMatrix(XWPSContextState * ctx, XWPSMatrix * pmat)
{
	int code;
  XWPSRef values[6];
  XWPSRef *pvalues;

  if (hasType(Array))
		pvalues = getArray();
  else 
  {
		int i;

		for (i = 0; i < 6; ++i) 
		{
	    code = arrayGet(ctx, (long)i, &values[i]);
	    if (code < 0)
				return code;
		}
		pvalues = values;
  }
  checkRead();
  if (code < 0)
  	return code;
  	
  if (size() != 6)
  {
	  return (int)(XWPSError::RangeCheck);
	}
	XWPSRef * a = pvalues + 5;
  code = a->floatMatrix(6, pmat);
  return (code < 0 ? code : 0);
}

int XWPSRef::realParam(double *pparam)
{
	switch (type()) 
	{
		case Integer:
	    *pparam = value.intval;
	    break;
	    
		case Real:
	    *pparam = value.realval;
	    break;
	    
		default:
	    return (int)(XWPSError::TypeCheck);
  }
  return 0;
}

void XWPSRef::setArrayPtr(XWPSRef * a)
{
	switch (type())
	{
		case Array:
			if (value.refs)
				value.refs->ptr = a;
			break;
			
		case OpArray:
			if (value.const_refs)
				value.const_refs->ptr = a;
			break;
			
		case MixedArray:
		case ShortArray:
			if (value.packed)
				value.packed->ptr = (ushort*)a;
			break;
			
		default:
			break;
	}
}

void XWPSRef::setBytesPtr(uchar * p)
{	
	value.bytes->ptr = p;
}

void XWPSRef::setPackedPtr(ushort * p)
{
	switch (type())
	{
		case Array:
			if (value.refs)
				value.refs->ptr = (XWPSRef*)p;
			break;
			
		case OpArray:
			if (value.const_refs)
				value.const_refs->ptr = (XWPSRef*)p;
			break;
			
		case MixedArray:
		case ShortArray:
			if (value.packed)
				value.packed->ptr = p;
			break;
			
		default:
			break;
	}
}

void XWPSRef::setSpace(ushort s)
{
	storeAttrs(PS_A_SPACE, s);
}

ushort XWPSRef::space()
{
	return (typeAttrs() & PS_A_SPACE);
}

int XWPSRef::spaceIndex()
{
	return ((int)space() >> PS_R_SPACE_SHIFT);
}

int XWPSRef::storeCheckDest(XWPSRef * pnew)
{
	return pnew->storeCheckSpace(space());
}

int XWPSRef::storeCheckSpace(ushort s)
{
	if (space() > s)
		return (int)(XWPSError::InvalidAccess);
		
	return 0;
}

uint XWPSRef::subrBias()
{
	return (size() < 1240 ? 107 : size() < 33900 ? 1131 : 32768);
}

int XWPSRef::writeMatrixIn(const XWPSMatrix * pmat)
{
	int code = checkWriteType(Array);
	if (code < 0)
		return code;
		
	if (size() != 6)
	{
		return (int)(XWPSError::RangeCheck);
	}
	
	XWPSRef * aptr = getArray();
	double * pel = (double*)pmat;
	for (int i = 5; i >= 0; i--, aptr++, pel++)
	{
		aptr->makeReal(*pel);
	}
	return 0;
}

XWPSRefMemory::XWPSRefMemory()
{
	stable_memory = 0;
	chunk_size = 0;
	large_size = 0;
	space = 0;
	is_controlled = false;
	limit = 0;
	allocated = 0;
	inherited = 0;
	gc_allocated = 0;
	lost.objects = 0;
	lost.refs = 0;
	lost.strings = 0;
	save_level = 0;
	new_mask = 0;
	test_mask = 0;
	num_contexts = 0;
	total_scanned = 0;
	largest_free_size = 0;
}

XWPSRefMemory::XWPSRefMemory(uint chk_size)
{
	stable_memory = 0;
	chunk_size = chk_size;
	large_size = 0;
	space = 0;
	gc_status.vm_threshold = chunk_size * 3L;
	gc_status.max_vm = max_long;
	is_controlled = false;
	limit = 0;
	allocated = 0;
	inherited = 0;
	gc_allocated = 0;
	lost.objects = 0;
	lost.refs = 0;
	lost.strings = 0;
	save_level = 0;
	new_mask = 0;
	test_mask = ~0;
	num_contexts = 0;
	total_scanned = 0;
	largest_free_size = 0;
}

XWPSRefMemory::~XWPSRefMemory()
{
}

void XWPSRefMemory::setLimit()
{
	ulong max_allocated =  (gc_status.max_vm > previous_status.allocated ?  gc_status.max_vm - previous_status.allocated : 0);

  if (gc_status.enabled) 
  {
		ulong limitA = gc_allocated + gc_status.vm_threshold;

		if (limitA < previous_status.allocated)
	    limit = 0;
		else 
		{
	    limitA -= previous_status.allocated;
	    limit = qMin(limitA, max_allocated);
		}
  } 
  else
		limit = max_allocated;
}

XWPSVMSpace::XWPSVMSpace()
{
	memories.named.foreign = 0;
	memories.named.system = 0;
	memories.named.global = 0;
	memories.named.local = 0;
}

XWPSDualMemory::XWPSDualMemory(uint chunk_size, bool level2)
{
	XWPSRefMemory *ilmem = new XWPSRefMemory(chunk_size);
  XWPSRefMemory *ilmem_stable = new XWPSRefMemory(chunk_size);
  XWPSRefMemory *igmem = 0;
  XWPSRefMemory *igmem_stable = 0;
  XWPSRefMemory *ismem = new XWPSRefMemory(chunk_size);
  
  ilmem->stable_memory = ilmem_stable;
  if (level2)
  {
  	igmem = new XWPSRefMemory(chunk_size);
		igmem_stable = new XWPSRefMemory(chunk_size);
		igmem->stable_memory = igmem_stable;
  }
  else
  {
  	igmem = ilmem; 
  	igmem_stable = ilmem_stable;
  }
  
  spaces.memories.named.local = ilmem;
  spaces.memories.named.global = igmem;
  spaces.memories.named.system = ismem;
  igmem->space = PS_AVM_GLOBAL;
  igmem_stable->space = PS_AVM_GLOBAL;
  ilmem->space = PS_AVM_LOCAL;	
  ilmem_stable->space = PS_AVM_LOCAL;
  ismem->space = PS_AVM_SYSTEM;
  reclaim = 0;
  setSpace(PS_AVM_GLOBAL);
}

XWPSDualMemory::~XWPSDualMemory()
{
	if (spaces.memories.named.local == spaces.memories.named.global)
	{
		if (spaces.memories.named.local)
		{
			if (spaces.memories.named.local->stable_memory)
			{
				delete spaces.memories.named.local->stable_memory;
				spaces.memories.named.local->stable_memory = 0;
			}
			delete spaces.memories.named.local;
			spaces.memories.named.local = 0;
			spaces.memories.named.global = 0;
		}
	}
	else
	{
		if (spaces.memories.named.local)
		{
			if (spaces.memories.named.local->stable_memory)
			{
				delete spaces.memories.named.local->stable_memory;
				spaces.memories.named.local->stable_memory = 0;
			}
			delete spaces.memories.named.local;
			spaces.memories.named.local = 0;
		}
		
		if (spaces.memories.named.global)
		{
			if (spaces.memories.named.global->stable_memory)
			{
				delete spaces.memories.named.global->stable_memory;
				spaces.memories.named.global->stable_memory = 0;
			}
			delete spaces.memories.named.global;
			spaces.memories.named.global = 0;
		}
	}
	
	if (spaces.memories.named.system)
	{
		if (spaces.memories.named.system->stable_memory)
		{
			delete spaces.memories.named.system->stable_memory;
			spaces.memories.named.system->stable_memory = 0;
		}
		delete spaces.memories.named.system;
		spaces.memories.named.system = 0;
	}
}

int XWPSDualMemory::ireclaim(int space)
{
  XWPSRefMemory *mem;

  if (space < 0) 
  {
		XWPSMemoryStatus stats;
		ulong allocated;
		int i;

		mem = spaces.memories.named.global;	
		for (i = 0; i < 4; ++i) 
		{
	    mem = spaces.memories.indexed[i];
	    if (mem == 0)
				continue;
	    if (mem->gc_status.requested > 0 ||	(mem->stable_memory)->gc_status.requested > 0)
				break;
		}
		
		allocated = stats.allocated;
		if (mem->stable_memory != mem) 
		{
	    allocated += stats.allocated;
		}
		if (allocated >= mem->gc_status.max_vm) 
		{
	    return (int)(XWPSError::VMError);
		}
  } 
  else 
  {
		mem = spaces.memories.indexed[space >> PS_R_SPACE_SHIFT];
  }
  
  resetRequested();
	if (mem)
  	mem->setLimit();
  return 0;
}

void XWPSDualMemory::resetRequested()
{
	spaces.memories.named.system->gc_status.requested = 0;
  spaces.memories.named.global->gc_status.requested = 0;
  spaces.memories.named.local->gc_status.requested = 0;
}

void XWPSDualMemory::setInSave()
{
	setMasks(PS_L_NEW, PS_L_NEW);
}

void XWPSDualMemory::setMasks(uint n, uint t)
{
	int i;
  XWPSRefMemory *mem;

  new_mask = n;
  test_mask = t;
  for (i = 0; i < 4; ++i)
		if ((mem = spaces.memories.indexed[i]) != 0) 
		{
	    mem->new_mask = n;
	    mem->test_mask = t;
	    if (mem->stable_memory != mem) 
	    {
				mem = mem->stable_memory;
				if (mem)
				{
					mem->new_mask = n; 
					mem->test_mask = t;
				}
	    }
		}
}

void XWPSDualMemory::setNotInSave()
{
	setMasks(0, ~0);
}

void XWPSDualMemory::setSpace(uint space)
{
	XWPSRefMemory *mem = spaces.memories.indexed[space >> PS_R_SPACE_SHIFT];
	if (mem)
	{
  	current = mem;
  	current_space = mem->space;
  }
}

XWPSIGStateObj::XWPSIGStateObj()
	:XWPSStruct()
{
}

void XWPSIGStateObj::copy(XWPSIGStateObj * other)
{
	gstate.assign(&other->gstate);
}

int XWPSIGStateObj::getLength()
{
	return sizeof(XWPSIGStateObj);
}

const char * XWPSIGStateObj::getTypeName()
{
	return "igstateobj";
}
	
void XWPSRefDeviceNParams::copy(XWPSRefDeviceNParams * other)
{
	layer_names.assign(&other->layer_names);
	tint_transform.assign(&other->tint_transform);
}

void XWPSRefCieProcs::copy(XWPSRefCieProcs * other)
{
	DEFG.assign(&other->DEFG);
	ABC.assign(&other->ABC);
	DecodeLMN.assign(&other->DecodeLMN);
}

void XWPSRefCieRenderProcs::copy(XWPSRefCieRenderProcs * other)
{
	TransformPQR.assign(&other->TransformPQR);
	EncodeLMN.assign(&other->EncodeLMN);
	EncodeABC.assign(&other->EncodeABC);
	RenderTableT.assign(&other->RenderTableT);
}

void XWPSRefSeparationParams::copy(XWPSRefSeparationParams * other)
{
	layer_name.assign(&other->layer_name);
	tint_transform.assign(&other->tint_transform);
}

void XWPSRefColorProcs::copy(XWPSRefColorProcs * other)
{
	cie.copy(&other->cie);
	device_n.copy(&other->device_n);
	separation.copy(&other->separation);
	index_proc.assign(&other->index_proc);
}

void XWPSRefColorSpace::copy(XWPSRefColorSpace * other)
{
	array.assign(&other->array);
	procs.copy(&other->procs);
}

void XWPSRefRgb::copy(XWPSRefRgb * other)
{
	red.assign(&other->red);
	green.assign(&other->green);
	blue.assign(&other->blue);
	gray.assign(&other->gray);
}

void XWPSRefColor::copy(XWPSRefColor * other)
{
	indexed[0].assign(&other->indexed[0]);
	indexed[1].assign(&other->indexed[1]);
	indexed[2].assign(&other->indexed[2]);
	indexed[3].assign(&other->indexed[3]);
	colored.copy(&other->colored);
}

void XWPSRefColorRendering::copy(XWPSRefColorRendering * other)
{
	dict.assign(&other->dict);
	procs.copy(&other->procs);
}

void XWPSIntGState::copy(XWPSIntGState * other)
{
	dash_pattern.assign(&other->dash_pattern);
	screen_procs.copy(&other->screen_procs);
	transfer_procs.copy(&other->transfer_procs);
	black_generation.assign(&other->black_generation);
	undercolor_removal.assign(&other->undercolor_removal);
	colorspace.copy(&other->colorspace);
	pattern.assign(&other->pattern);
	colorrendering.copy(&other->colorrendering);
	halftone.assign(&other->halftone);
	pagedevice.assign(&other->pagedevice);
	remap_color_info.assign(&other->remap_color_info);
	opacity_mask.assign(&other->opacity_mask);
	shape_mask.assign(&other->shape_mask);
}

XWPSIntRemapColorInfo::XWPSIntRemapColorInfo()
	:XWPSStruct()
{
}

int XWPSIntRemapColorInfo::getLength()
{
	return sizeof(XWPSIntRemapColorInfo);
}

const char * XWPSIntRemapColorInfo::getTypeName()
{
	return "intremapcolorinfo";
}
	
XWPSIntPattern::XWPSIntPattern(XWPSRef * op)
	:XWPSStruct()
{
	dict.assign(op);
}

int XWPSIntPattern::getLength()
{
	return sizeof(XWPSIntPattern);
}

const char * XWPSIntPattern::getTypeName()
{
	return "intpattern";
}
	
XWPSImageParams::XWPSImageParams()
	:XWPSStruct()
{
	MultipleDataSources = false;
	pDecode = 0;
}

int XWPSImageParams::getLength()
{
	return sizeof(XWPSImageParams);
}

const char * XWPSImageParams::getTypeName()
{
	return "imageparams";
}
	
XWPSFontData::XWPSFontData()
	:XWPSStruct()
{
	u.type1.OtherSubrs = new XWPSRef;
	u.type1.Subrs = new XWPSRef;
	u.type1.GlobalSubrs = new XWPSRef;
}

XWPSFontData::~XWPSFontData()
{
	if (u.type1.OtherSubrs)
	{
		delete u.type1.OtherSubrs;
		u.type1.OtherSubrs = 0;
	}
	
	if (u.type1.Subrs)
	{
		delete u.type1.Subrs;
		u.type1.Subrs = 0;
	}
	
	if (u.type1.GlobalSubrs)
	{
		delete u.type1.GlobalSubrs;
		u.type1.GlobalSubrs = 0;
	}
}

void XWPSFontData::copy(XWPSFontData * other)
{
	dict.assign(&other->dict);
	BuildChar.assign(&other->BuildChar);
	BuildGlyph.assign(&other->BuildGlyph);
	Encoding.assign(&other->Encoding);
	CharStrings.assign(&other->CharStrings);
	u.type1.OtherSubrs->assign(other->u.type1.OtherSubrs);
	u.type1.Subrs->assign(other->u.type1.Subrs);
	u.type1.GlobalSubrs->assign(other->u.type1.GlobalSubrs);
}

int XWPSFontData::getLength()
{
	return sizeof(XWPSFontData);
}

const char * XWPSFontData::getTypeName()
{
	return "fontdata";
}
	
XWPSCharstringFontRefs::XWPSCharstringFontRefs()
	:XWPSStruct()
{
	Private = 0;
	OtherSubrs = 0;
	Subrs = 0;
	GlobalSubrs = 0;
}

void XWPSCharstringFontRefs::copy(XWPSCharstringFontRefs * other)
{
	Private = other->Private;
	OtherSubrs = other->OtherSubrs;
	Subrs = other->Subrs;
	GlobalSubrs = other->GlobalSubrs;
	no_subrs.assign(&other->no_subrs);
}

int XWPSCharstringFontRefs::getLength()
{
	return sizeof(XWPSCharstringFontRefs);
}

const char * XWPSCharstringFontRefs::getTypeName()
{
	return "charstringfontrefs";
}

XWPSArray::XWPSArray(uint siz)
	:XWPSStruct(),
	 arr(0),
	 ptr(0)
{
	arr = new XWPSRef[siz + 1];
}

XWPSArray::~XWPSArray()
{
	if (arr)
	{
		delete [] arr;
		arr = 0;
	}
}

int XWPSArray::getLength()
{
	return sizeof(XWPSArray);
}

const char * XWPSArray::getTypeName()
{
	return "array";
}

XWPSArrayRef::XWPSArrayRef()
	:XWPSStruct(),
	 arr(0),
	 ptr(0)
{
}

XWPSArrayRef::XWPSArrayRef(XWPSRef * elts)
	:XWPSStruct(),
	 arr(0),
	 ptr(elts)
{
}

XWPSArrayRef::XWPSArrayRef(uint siz)
	:XWPSStruct()
{
	arr = new XWPSArray(siz);
	ptr = arr->arr;
}

XWPSArrayRef::~XWPSArrayRef()
{
	if (arr)
	{
		if (arr->decRef() == 0)
			delete arr;
		arr = 0;
	}
}

XWPSPackedArray::XWPSPackedArray(uint size, uint countA)
	:XWPSStruct(),
	 arr(0),
	 ptr(0),
	 count(countA)
{
	arr = (ushort*)malloc((size + 1) * sizeof(XWPSRef));
	ptr = arr;
}

XWPSPackedArray::~XWPSPackedArray()
{
	if (arr)
	{
		ushort * elt = arr;
		for (uint j = 0; j < count; j++)
		{
			if (r_is_packed(elt))
				elt++;
			else
			{
				XWPSRef * r = (XWPSRef*)elt;
				r->clear();
				elt += packed_per_ref;
			}
		}
		
		free(arr);
	}
}

int XWPSPackedArray::getLength()
{
	return sizeof(XWPSPackedArray);
}

const char * XWPSPackedArray::getTypeName()
{
	return "packedarray";
}

XWPSPackedArrayRef::XWPSPackedArrayRef()
	:XWPSStruct(),
	 arr(0),
	 ptr(0)
{
}

XWPSPackedArrayRef::XWPSPackedArrayRef(ushort * elt)
	:XWPSStruct(),
	 arr(0),
	 ptr(elt)
{
}

XWPSPackedArrayRef::XWPSPackedArrayRef(uint size, uint countA)
	:XWPSStruct()
{
	arr = new XWPSPackedArray(size, countA);
	ptr = arr->arr;
}

XWPSPackedArrayRef::~XWPSPackedArrayRef()
{
	if (arr)
	{
		if (arr->decRef() == 0)
			delete arr;
		arr = 0;
	}
}

//慢!!!!
int memcpyRef(XWPSRef * to, XWPSRef * from, int num)
{
	for (int i = 0; i < num; i++)
	{
		to->assign(from);
		to++;
		from++;
	}
	return 0;
}

//慢!!!!
int memmoveRef(XWPSRef * to, XWPSRef * from, int num)
{
	XWPSRef * tmp = new XWPSRef[num];
	XWPSRef * s = from;
	XWPSRef * d = tmp;
	for (int i = 0; i < num; i++)
	{
		d->assign(s);
		d++;
		s++;
	}
	
	s = tmp;
	d = to;
	for (int i = 0; i < num; i++)
	{
		d->assign(s);
		d++;
		s++;
	}
	delete [] tmp;
	return 0;
}
