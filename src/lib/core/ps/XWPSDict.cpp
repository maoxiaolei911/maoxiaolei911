/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include "XWPSError.h"
#include "XWPSContextState.h"
#include "XWPSDict.h"

static const uint dict_max_size = PS_MAX_ARRAY_SIZE - 1;
static bool dict_default_pack = true;

XWPSDict::XWPSDict()
	:refCount(1)
{
	count.makeInt(0);
	maxlength.makeInt(0);
}

XWPSDict::XWPSDict(uint size)
{
	createContents(size, false);
}

XWPSDict::~XWPSDict()
{
}

int XWPSDict::createContents(uint size, bool pack)
{
	uint asize = roundSize((size == 0 ? 1 : size));
	if (asize == 0 || asize > PS_MAX_ARRAY_SIZE - 1)
	{
		return (int)(XWPSError::LimitCheck);
	}
			
	asize++;
	values.makeArray(PS_AVM_FOREIGN | PS_A_ALL, asize);
	if (pack)
	{
		uint ksize = (asize + packed_per_ref - 1) / packed_per_ref;
		keys.makeShortArray(PS_AVM_FOREIGN | PS_A_ALL, ksize, asize);
		ushort * pkp = keys.getPacked();
		ushort * pzp = pkp;
		for (uint i = 0; i < asize || i % packed_per_ref; i++)
		{
			*pzp = packed_key_empty;
			pzp++;
		}	
		*pkp = packed_key_deleted;
	}
	else
	{
		int code = createUnpackedKeys(asize);
		if (code < 0)
			return code;
	}
	count.makeInt(0, 0);
	maxlength.makeInt(0, size);
	return 0;
}

int XWPSDict::createUnpackedKeys(uint asize)
{
	keys.makeArray(PS_AVM_FOREIGN | PS_A_ALL, asize);
	XWPSRef *kp = keys.getArray();
	kp->setAttrs(PS_A_EXECUTABLE);
	return 0;
}

XWPSRef * XWPSDict::getKeys()
{
	return keys.getArray();
}

ushort  * XWPSDict::getKeysPacked()
{
	return keys.getPacked();
}

ushort  * XWPSDict::getKeysWritablePacked()
{
	return keys.getWritablePacked();
}

uint XWPSDict::getLength()
{
	return (uint)(count.getInt());
}

uint XWPSDict::getMaxLength()
{
	return (uint)(maxlength.getInt());
}

uint XWPSDict::getMaxSize()
{
	return dict_max_size;
}

XWPSRef * XWPSDict::getValues()
{
	return values.getArray();
}

ushort  * XWPSDict::getValuesPacked()
{
	return values.getPacked();
}

ushort  * XWPSDict::getValuesWritablePacked()
{
	return values.getWritablePacked();
}

bool XWPSDict::isPacked()
{
	return keys.hasType(XWPSRef::ShortArray);
}

ushort XWPSDict::npairs()
{
	return nslots() - 1;
}

ushort XWPSDict::nslots()
{
	return values.size();
}

uint XWPSDict::roundSize(uint rsize)
{
#if arch_small_memory
    return roundSizeSmall(rsize);
#else
    return roundSizeLarge(rsize);
#endif
}

uint XWPSDict::roundSizeLarge(uint rsize)
{
	if (rsize > dict_max_non_huge)
		return (rsize > dict_max_size ? 0 : rsize);
  while (rsize & (rsize - 1))
		rsize = (rsize | (rsize - 1)) + 1;
  return (rsize <= dict_max_size ? rsize : dict_max_non_huge);
}

uint XWPSDict::roundSizeSmall(uint rsize)
{
	return (rsize > dict_max_size ? 0 : rsize);
}

int XWPSDict::search(XWPSContextState * ctx, 
	                   XWPSRef * pkey,
	                   uint nidx, 
	                   ushort kpack, 
	                   uint hash, 
	                   int ktype, 
	                   XWPSRef ** ppvalue)
{
	uint size = npairs();
	if (isPacked())
	{
		ushort *pslot = 0;
		ushort *kbot = keys.getPacked();
		ushort *kp = kbot + dict_hash_mod(hash, size) + 1;
		for (; ; kp--)
		{
			if (*kp == kpack)
			{
				*ppvalue = values.getArray() + (kp - kbot);
				return 1;
			}
			else if (!r_packed_is_name(kp))
			{
				if ( *kp == packed_key_empty ) 
					goto miss;
					
				if (kp == kbot) 
					break;
				else
				{
					if (pslot == 0) 
						pslot = kp;
				}
			}
		}
		
		for (kp += size; ; kp--)
		{
			if (*kp == kpack)
			{
				*ppvalue = values.getArray() + (kp - kbot);
				return 1;
			}
			else if (!r_packed_is_name(kp))
			{
				if ( *kp == packed_key_empty ) 
					goto miss;
					
				if (kp == kbot) 
					break;
				else
				{
					if (pslot == 0) 
						pslot = kp;
				}
			}
		}
		
		if (pslot == 0 || getLength() == getMaxLength())
	    return (int)(XWPSError::DictFull);
	    	
	  *ppvalue = values.getArray() + (pslot - kbot);
		return 0;
miss:
		if (getLength() == getMaxLength())
	    return (int)(XWPSError::DictFull);
		if (pslot == 0)
	    pslot = kp;
		*ppvalue = values.getArray() + (pslot - kbot);
		return 0;
	}
	else
	{
		XWPSRef *kbot = keys.getArray();		
		XWPSRef *pslot = 0;
		int wrap = 0;
		int etype = 0;
		XWPSRef *kp = kbot + dict_hash_mod(hash, size) + 2;
		for (;;)
		{
			--kp;
			if ((etype = kp->type()) == ktype)
			{
				if (ctx->nameIndex(kp) == nidx)
				{
					*ppvalue = values.getArray() + (kp - kbot);
		    	return 1;
				}
			}
			else if (etype == XWPSRef::Null)
			{
				if (kp == kbot)
				{
					if (wrap++) 
					{
						if (pslot == 0)
			    		return (int)(XWPSError::DictFull);
						break;
		    	}
		    	kp += size + 1;
				}
				else if (kp->hasAttr(PS_A_EXECUTABLE)) 
				{
		    	if (pslot == 0)
						pslot = kp;
				} 
				else
		    	break;
			}
			else
			{
				if (kp->objEq(ctx, pkey)) 
				{
		    	*ppvalue = values.getArray() + (kp - kbot);
		    		return 1;
				}
			}
		}
		if (getLength() == getMaxLength())
	    return (int)(XWPSError::DictFull);
		*ppvalue = values.getArray() + ((pslot != 0 ? pslot : kp) - kbot);
		return 0;
	}
}

void XWPSDict::setMaxLength(uint siz)
{
	maxlength.makeInt((long)siz);
}

int XWPSDict::unpack(XWPSContextState * ctx, XWPSDictStack * pds)
{
	if (!isPacked())
		return 0;
		
	uint c = nslots();
	XWPSPackedArrayRef * kpacked = keys.value.packed;
	ushort * okp = kpacked->ptr;
	
	keys.value.packed = 0;
	int code = createUnpackedKeys(c);
	if (code < 0)
	{
		if (kpacked)
			delete kpacked;
	  return code;
	}
	  
	XWPSRef * nkp = keys.getArray();
	for (; c--; okp++, nkp++)
	{
		if (r_packed_is_name(okp))
			ctx->packedGet(okp, nkp);
		else if (*okp == packed_key_deleted)
			nkp->setAttrs(PS_A_EXECUTABLE);
	}
	
	if (kpacked)
		delete kpacked;
	
	if (pds)
	  pds->setTop();
	return 0;
}

int XWPSDict::valueIndex(XWPSRef * pvalue)
{
	return (int)(pvalue - values.getArray() - 1);
}

static ushort no_packed_keys[2] ={packed_key_deleted, packed_key_empty};

XWPSDictStack::XWPSDictStack(QObject * parent)
	:QObject(parent)
{
	min_size = 0;
	userdict_index = 0;
	def_space = 0;
	top_keys = 0;
	top_npairs = 0;
	top_values = 0;
#define REFS_SIZE_DSTACK DS_REFS_SIZE(MAX_DSTACK)

	XWPSRef stk;
	stk.makeArray(0, REFS_SIZE_DSTACK);
	stack.init(&stk, 0, 0, NULL, NULL);
	stack.setErrorCodes(XWPSError::DictStackUnderflow, XWPSError::DictStackOverflow);
	stack.setMaxCount(MAX_DSTACK);
	
#undef REFS_SIZE_DSTACK
	system_dict.setSpace(PS_AVM_MAX);
}

XWPSDictStack::~XWPSDictStack()
{
}

void XWPSDictStack::allowExpansion(bool expand)
{
	stack.allowExpansion(expand);
}

void XWPSDictStack::cleanup()
{
	stack.cleanup();
}

uint XWPSDictStack::count()
{
	return stack.count();
}

uint XWPSDictStack::countToMark()
{
	return stack.countToMark();
}

int XWPSDictStack::extend(uint request)
{
	return stack.extend(request);
}

bool XWPSDictStack::dictIsPermanent(XWPSRef * pdref)
{
	XWPSDict *pdict = pdref->getDict();
  if (stack.getExtensionSize() == 0) 
  {
		for (int i = 0; i < min_size; ++i)
	    if (stack.getBottomDict(i) == pdict)
				return true;
  } 
  else 
  {
		uint count = stack.count();
		for (int i = count - min_size; i < count; ++i)
	    if (stack.index(i)->getDict() == pdict)
		return true;
  }
  return false;
}

XWPSRef * XWPSDictStack::findNameByIndex(XWPSContextState * ctx, uint nidx)
{
	XWPSRef * pdref = stack.getCurrentTop();
	ushort kpack = packed_name_key(nidx);
	do
	{
		XWPSDict *pdict = pdref->getDict();
		uint size = pdict->npairs();
		if (pdict->isPacked())
		{
			ushort *kbot = pdict->keys.getPacked();
			ushort *kp = kbot + dict_hash_mod(dict_name_index_hash(nidx), size) + 1;
			for (; ; kp--)
			{
				if ( *kp == kpack )
					return (pdict->getValues() + (kp - kbot));
				else if ( !r_packed_is_name(kp) )
				{
					if ( *kp == packed_key_empty ) 
						goto miss;
					
					if (kp == kbot) 
						break;
				}
			}
			
			for (kp += size; ; kp--)
			{
				if ( *kp == kpack )
					return (pdict->getValues() + (kp - kbot));
				else if ( !r_packed_is_name(kp) )
				{
					if ( *kp == packed_key_empty ) 
						break;
					
					if (kp == kbot) 
						break;
				}
			}
miss:
			;
		}
		else
		{
			int wrap = 0;
			XWPSRef *kbot = pdict->getKeys();
			XWPSRef * kp;
			for (kp = kbot + dict_hash_mod(dict_name_index_hash(nidx), size) + 2;;)
			{
				--kp;
				if (kp->hasType(XWPSRef::Name))
				{
					if (ctx->nameIndex(kp) == nidx) 
						return pdict->getValues() + (kp - kbot);
				}
				else if (kp->hasType(XWPSRef::Null))
				{
					if (!kp->hasAttr(PS_A_EXECUTABLE))
						break;
		    	if (kp == kbot) 
		    	{	
						if (wrap++)
			    		break;
						kp += size + 1;
		    	}
				}
			}
		}
	} while (pdref-- > stack.getBottom());
	
	if (!stack.getExtensionSize())
		return 0;
		
	uint i = stack.getCurrentTop() + 1 - stack.getBottom();
	uint size = stack.count();
	XWPSRef *pvalue = 0;
	
	XWPSRef key;
	ctx->nameIndexRef(nidx, &key);
	for (; i < size; i++) 
	{
		XWPSRef * s = stack.index(i);
	  if (s && s->dictFind(ctx, &key, &pvalue) > 0) 
			return pvalue;
	}
	return 0;
}

XWPSRef * XWPSDictStack::findNameByIndex(XWPSContextState * ctx, uint nidx, uint & htemp)
{
	htemp = dict_hash_mod_inline_large(dict_name_index_hash(nidx), top_npairs) + 1;
	if (top_keys[htemp] == pt_tag(pt_literal_name) + (nidx))
		return (top_values + htemp);
		
	return findNameByIndex(ctx, nidx);
}

bool  XWPSDictStack::findNameByIndexTop(uint nidx, uint & htemp, XWPSRef ** pvslot)
{
	htemp = dict_hash_mod_inline_large(dict_name_index_hash(nidx), top_npairs) + 1;
	if (top_keys[htemp] == pt_tag(pt_literal_name) + (nidx))
	{
		*pvslot = (top_values + htemp);
		return true;
	}
	
	*pvslot = 0;
	return false;
}

XWPSDict * XWPSDictStack::getBottomDict(uint i)
{
	return stack.getBottomDict(i);
}

XWPSRef * XWPSDictStack::getUserDict()
{
	return index(count() - 1 - userdict_index);
}

XWPSRef * XWPSDictStack::incCurrentTop(int c)
{
	return stack.incCurrentTop(c);
}

XWPSRef * XWPSDictStack::index(long idx)
{
	return stack.index(idx);
}

void XWPSDictStack::init(XWPSRef *pblock_array,
	                     uint bot_guard, 
	                     uint top_guard, 
	                     XWPSRef *pguard_value,
	                     XWPSRefStackParams *paramsA)
{
	stack.init(pblock_array, bot_guard, top_guard, pguard_value, paramsA);
}

long XWPSDictStack::maxCount()
{
	return stack.maxCount();
}

void XWPSDictStack::pop(uint count)
{
	stack.pop(count);
}

int XWPSDictStack::popBlock()
{
	return stack.popBlock();
}

int  XWPSDictStack::push(uint count)
{
	return stack.push(count);
}

void XWPSDictStack::restoreFixStack(bool is_estack)
{
	stack.restoreFixStack(is_estack);
}

void XWPSDictStack::setErrorCodes(int underflow_errorA, int overflow_errorA)
{
	stack.setErrorCodes(underflow_errorA, overflow_errorA);
}

int XWPSDictStack::setMargin(uint marginA)
{
	return stack.setMargin(marginA);
}

int  XWPSDictStack::setMaxCount(long nmax)
{
	stack.setMaxCount(nmax);return 0;
}

void XWPSDictStack::setSystemDict(XWPSRef * pdict)
{
	system_dict.assign(pdict);
}

void XWPSDictStack::setTop()
{
	XWPSRef * dsp = stack.getCurrentTop();
	XWPSDict *pdict = dsp->getDict();
	if (pdict->isPacked() &&	dsp->dictAccessRef()->hasAttr(PS_A_READ))
	{
		top_keys = pdict->getKeysPacked();
		top_npairs = pdict->npairs();
		top_values = pdict->getValues();
	}
	else
	{
		top_keys = no_packed_keys;
		top_npairs = 1;
	}
	
	if (!dsp->dictAccessRef()->hasAttr(PS_A_WRITE))
		def_space = -1;
  else
		def_space = dsp->space();
}

int  XWPSDictStack::store(XWPSRef *parray, 
	                        uint count,
					                uint skip, 
					                int age, 
					                bool check)
{
	return stack.store(parray, count, skip, age, check);
}

int  XWPSDictStack::storeCheck(XWPSRef *parray, uint count, uint skip)
{
	return stack.storeCheck(parray, count, skip);
}
