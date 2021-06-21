/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWPSError.h"
#include "XWPSContextState.h"
#include "XWPSRefStack.h"

XWPSRefStackParams::XWPSRefStackParams()
{
	bot_guard = 0;
	top_guard = 0;
	block_size = 0;
	data_size = 0;
	underflow_error = -1;
	overflow_error = -1;
	allow_expansion = true;
}

XWPSRefStackParams::XWPSRefStackParams(XWPSRefStackParams * other)
{
	bot_guard = other->bot_guard;
	top_guard = other->top_guard;
	block_size = other->block_size;
	data_size = other->data_size;
	guard_value.assign(&(other->guard_value));
	underflow_error = other->underflow_error;
	overflow_error = other->overflow_error;
	allow_expansion = other->allow_expansion;
}

XWPSRefStackParams::XWPSRefStackParams(uint bot_guardA,
	                                     uint top_guardA,
	                                     uint block_sizeA,
	                                     uint data_sizeA,
	                                     XWPSRef * pguard_value)
{
	bot_guard = bot_guardA;
	top_guard = top_guardA;
	block_size = block_sizeA;
	data_size = data_sizeA;
	if (pguard_value)
		guard_value.assign(pguard_value);
	underflow_error = -1;
	overflow_error = -1;
	allow_expansion = true;
}

void XWPSRefStackEnum::begin(XWPSRefStack * pstack)
{
	block = (XWPSRefStackBlock *)pstack->current.getArray();
  ptr = pstack->getBottom();
  size = pstack->getCurrentTop() + 1 - pstack->getBottom();
}

bool XWPSRefStackEnum::next()
{
	block = (XWPSRefStackBlock *)(block->next.getArray());
	if (block == 0)
		return false;
  ptr = block->used.getArray();
  size = block->used.size();
  return true;
}


XWPSRefStack::XWPSRefStack(QObject * parent)
	:QObject(parent)
{
	p = 0;
	bot = 0;
	top = 0;
	extension_size = 0;
	extension_used = 0;
	requested = 0;
	margin = 0;
	body_size = 0;
	params = 0;	
}

XWPSRefStack::~XWPSRefStack()
{
	if (params)
  	delete params;
  	
  params = 0;
}

void XWPSRefStack::allowExpansion(bool expand)
{
	params->allow_expansion = expand;
}

void XWPSRefStack::cleanup()
{
	XWPSRefStackBlock *pblock =	(XWPSRefStackBlock *)(current.getArray());
	XWPSRef * t = p + 1;
	uint m = top - p;
	for (; m--; ++t)
		t->makeNull();
	
	if (current.value.refs->arr)
  	current.value.refs->arr->incRef();
  pblock->used.makeArray(0, p + 1 - bot, bot);
  pblock->used.value.refs->arr = current.value.refs->arr;
  pblock->used.tas.type_attrs = current.tas.type_attrs;
  pblock->used.setSize(p + 1 - bot);
}

uint XWPSRefStack::countToMark()
{
	uint scanned = 0;
  XWPSRefStackEnum rsenum;
  rsenum.begin(this);
  do 
  {
		uint count = rsenum.size;
		XWPSRef * t = rsenum.ptr + count - 1;

		for (; count; count--, t--)
		{
	    if (t->hasType(XWPSRef::Mark))
				return scanned + (rsenum.size - count + 1);
		}
		scanned += rsenum.size;
  } while (rsenum.next());
  return 0;
}

int XWPSRefStack::extend(uint request)
{
	uint keep = (top - bot + 1) / 3;
  uint count = p - bot + 1;  
  if (request > params->data_size)
		return (int)(params->overflow_error);
  if (keep + request > body_size)
		keep = body_size - request;
  if (keep > count)
		keep = count;		/* required by ref_stack_push_block */
  return pushBlock(keep, request);
}

XWPSDict * XWPSRefStack::getBottomDict(uint i)
{
	if (bot)
		return bot[i].getDict();
		
	return 0;
}

XWPSRef * XWPSRefStack::incCurrentTop(int c)
{
	p += c;
	return p;
}

XWPSRef * XWPSRefStack::index(long idx)
{
	uint used = p + 1 - bot;

  if (idx < 0)
		return NULL;
  if (idx < used)	
		return p - (uint) idx;
		
  XWPSRefStackBlock * pblock = (XWPSRefStackBlock *)current.getArray();
  do 
  {
		pblock = (XWPSRefStackBlock *) pblock->next.getArray();
		if (pblock == 0)
	    return NULL;
		idx -= used;
		used = pblock->used.size();
  } while (idx >= used);
  return pblock->used.getArray() + (used - 1 - (uint) idx);
}

void XWPSRefStack::init(XWPSRef *pblock_array,
	                      uint bot_guard, 
	                      uint top_guard, 
	                      XWPSRef *pguard_value,
	                      XWPSRefStackParams *paramsA)
{
	uint size = pblock_array->size();
  uint avail = size - (stack_block_refs + bot_guard + top_guard);
  XWPSRefStackBlock *pblock = (XWPSRefStackBlock *)pblock_array->getArray();
  XWPSRef * body = (XWPSRef*)(pblock + 1);
  if (paramsA == 0)
  	paramsA = new XWPSRefStackParams();
  	
  bot = body + bot_guard;
  p = bot - 1;
  top = p + avail;
  current.assign(pblock_array);
  extension_size = 0;
  extension_used = 0;
  max_stack.makeInt(avail);
  requested = 0;
  margin = 0;
  body_size = avail;

  params = paramsA;
  params->bot_guard = bot_guard;
  params->top_guard = top_guard;
  params->block_size = size;
  params->data_size = avail;
  if (pguard_value != 0)
		params->guard_value.assign(pguard_value);
		
	initBlock(pblock_array, 0);
	
	XWPSRef * t = bot;
	for (; avail--; ++t)
		t->makeNull();
	pblock->next.makeArray(0);
}

long XWPSRefStack::maxCount()
{
	return max_stack.value.intval;
}

void XWPSRefStack::restoreFixStack(bool is_estack)
{
	XWPSRefStackEnum rsenum;

  rsenum.begin(this);
  do 
  {
		XWPSRef *stkp = rsenum.ptr;
		uint size = rsenum.size;

		for (; size; stkp++, size--) 
		{
	    stkp->clearAttrs(PS_L_NEW);	
	    if (is_estack) 
	    {
	    	XWPSRef ofile;
	    	ofile.assign(stkp);
	    	switch (stkp->type())
	    	{
	    		case XWPSRef::String:
	    			if (stkp->size() == 0)
	    				break;
	    			else
	    				continue;
	    				
	    		case XWPSRef::File:
	    			break;
	    			
	    		default:
	    			continue;
	    	}
	    	
				stkp->copyAttrs(PS_A_ALL | PS_A_EXECUTABLE,  &ofile);
			}
	  }
  } while (rsenum.next());
}

void XWPSRefStack::pop(uint count)
{
	uint used;
  while ((used = p + 1 - bot) < count) 
  {
		count -= used;
		p = bot - 1;
		popBlock();
  }
  p -= count;
}

int XWPSRefStack::popBlock()
{
	uint countA = p + 1 - bot;
	XWPSRefStackBlock *pcur = (XWPSRefStackBlock *)current.getArray();
  XWPSRefStackBlock *pnext = (XWPSRefStackBlock *)(pcur->next.getArray());
  if (pnext == 0)
		return (int)(params->underflow_error);
		
  uint used = pnext->used.size();
  XWPSRef * body = (XWPSRef *) (pnext + 1) + params->bot_guard;
  XWPSRef next;
  next.assign(&(pcur->next));
  if (used + countA > body_size)
  {
  	uint moved = body_size - countA;
		if (moved == 0)
	    return (int)(XWPSError::Fatal);
	    	
	  memmoveRef(bot + moved, bot, countA);
	  	  
	  uint left = used - moved;
	  XWPSRef * d = bot;
	  XWPSRef * s = body + left;
	  memcpyRef(d, s, moved);
	  
	  uint m = moved;
	  d = body + left;
	  for (; m--; ++d)
			d->makeNull();
		
		pnext->used.decSize(moved);
		p = top;
		extension_used -= moved;
  }
  else
  {
  	XWPSRef * d = body + used;
	 	XWPSRef * s = bot;
	 	memcpyRef(d, s, countA);
	  
	  bot = body;
		top = bot + body_size - 1;
		current.assign(&next);
		p = bot + (used + countA - 1);
		extension_size -= body_size;
		extension_used -= used;
  }
  
  return 0;
}

int  XWPSRefStack::push(uint count)
{
	uint needed = count;
  uint added;
  for (; (added = top - p) < needed; needed -= added) 
  {
		p = top;
		int code = pushBlock((top - bot + 1) / 3, added);
		if (code < 0) 
		{
	    pop(count - needed + added);
	    requested = count;
	    return code;
		}
  }
  p += needed;
  return 0;
}

void XWPSRefStack::setErrorCodes(int underflow_errorA, int overflow_errorA)
{
	params->underflow_error = underflow_errorA;
  params->overflow_error = overflow_errorA;
}

int XWPSRefStack::setMargin(uint marginA)
{
	uint data_size = params->data_size;
	if (marginA <= margin)
	{
		XWPSRef * t = top + 1;
		uint s = margin - marginA;
		for (; s--; ++t)
			t->makeNull();
	}
	else
	{
		if (marginA > data_size >> 1)
		{
	    return (int)(XWPSError::RangeCheck);
	  }
		if ((uint)(top - p) < marginA) 
		{
	   uint used = p + 1 - bot;
	   uint keep = data_size - marginA;
	   int code = pushBlock(keep, used - keep);
	    if (code < 0)
				return code;
		}
	}
	
	margin = marginA;
  body_size = data_size - marginA;
  top = bot + body_size - 1;
  return 0;
}

int  XWPSRefStack::setMaxCount(long nmax)
{
	uint nmin = count();
	if (nmax < nmin)
		nmax = nmin;
  if (nmax > max_uint / sizeof(XWPSRef))
		nmax = max_uint / sizeof(XWPSRef);
  if (!params->allow_expansion) 
  {
		uint ncur = body_size;

		if (nmax > ncur)
	    nmax = ncur;
  }
  max_stack.makeInt((int)nmax);
  return 0;
}

int  XWPSRefStack::store(XWPSRef *parray, 
	                       uint countA,
					               uint skip, 
					               int age, 
					               bool check)
{
	if (countA > count() || countA > parray->size())
	{
		return (int)(XWPSError::RangeCheck);
	}
  if (check)
  {
  	int code = storeCheck(parray, countA, skip);
		if (code < 0)
	    return code;
  }
  
  XWPSRef * to = parray->getArray() + countA;
  uint left = countA, pass = skip;
  XWPSRefStackEnum rsenum;
  rsenum.begin(this);
  do 
  {
		XWPSRef *from = rsenum.ptr;
		uint size = rsenum.size;

		if (size <= pass)
	    pass -= size;
		else 
		{
	    if (pass != 0)
	    {
				size -= pass; 
				pass = 0;
			}
	    from += size;
	    if (size > left)
				size = left;
	    left -= size;
	    switch (age) 
	    {
	    	case -1:	
					while (size--) 
					{
		    		from--; 
		    		to--;
		    		to->assign(from);
					}
					break;
					
	    	case 0:	
					while (size--) 
					{
		    		from--; 
		    		to--;
		    		to->assign(from);
					}
					break;
					
	    	case 1:	
					while (size--) 
					{
		    		from--; 
		    		to--;
		    		to->assign(from);
					}
					break;
	    	}
	    	if (left == 0)
					break;
			}
  } while (rsenum.next());
  parray->setSize(countA);
  return 0;
}

int  XWPSRefStack::storeCheck(XWPSRef *parray, uint count, uint skip)
{
	uint space = parray->space();
  if (space != PS_AVM_LOCAL) 
  {
		uint left = count, pass = skip;
		XWPSRefStackEnum rsenum;
		rsenum.begin(this);
		do 
		{
	    XWPSRef *ptr = rsenum.ptr;
	    uint size = rsenum.size;
	    if (size <= pass)
				pass -= size;
	    else 
	    {
				if (pass != 0)
				{
		    	size -= pass; 
		    	pass = 0;
		    }
				ptr += size;
				if (size > left)
		    	size = left;
				left -= size;
				XWPSRef * b = ptr - size;
				uint s = size;
				for (; s--; b++)
				{
					int code = b->storeCheckSpace(space);
					if (code < 0)
		    		return code;
				}
				if (left == 0)
		    	break;
	    }
		} while (rsenum.next());
  }
  return 0;
}

void XWPSRefStack::initBlock(XWPSRef *psb, uint )
{
	XWPSRef *brefs = psb->getArray();
  XWPSRef *t = brefs + stack_block_refs;
  for (uint i = params->bot_guard; i != 0; i--, t++)
		t->assign(&(params->guard_value));
		
	if (params->top_guard) 
	{
		t = brefs + psb->size();		
		int top_guard = params->top_guard;
		t -= top_guard;
		for (; top_guard--; ++t)
			t->makeNull();
  }
  
  {
		XWPSRefStackBlock * pblock = (XWPSRefStackBlock *) brefs;
		pblock->used.assign(psb);
		pblock->used.setArrayPtr(brefs + stack_block_refs + params->bot_guard);
		pblock->used.setSize(0);
  }
}

int XWPSRefStack::pushBlock(uint keep, uint add)
{
	uint count = p - bot + 1;
  uint move = count - keep;
  XWPSRefStackBlock *pcur = (XWPSRefStackBlock *)(current.getArray());
  if (keep > count)
  {
		return (int)(XWPSError::Fatal);
	}
			
	if (extension_used + (top - bot) + add >=	(uint)max_stack.getInt() ||	!params->allow_expansion)
		return (int)(params->overflow_error);
		
	XWPSRef next;
	next.makeArray(0, (ushort)params->block_size);
	XWPSRefStackBlock * pnext = (XWPSRefStackBlock *)(next.getArray());
  XWPSRef * body = (XWPSRef *) (pnext + 1);
  initBlock(&next, keep);
  body += params->bot_guard;
  XWPSRef * d = body;
	XWPSRef * s = bot + move;
	memcpyRef(d, s, keep);
	
	d = body + keep;	
	uint m = params->data_size - keep;	
	for (; m--; ++d)
		d->makeNull();
	
	d = bot + move;
	m = keep;
	for (; m--; ++d)
		d->makeNull();
	
	pnext->next.assign(&current);
	if (current.value.refs->arr)
  	current.value.refs->arr->incRef();
  pcur->used.makeArray(0, move, bot);
  pcur->used.value.refs->arr = current.value.refs->arr;
  current.assign(&next);
  bot = body;
  top = bot + body_size - 1;
  p = bot + keep - 1;
  extension_size += body_size;
  extension_used += move;
  return 0;
}

XWPSExecStack::XWPSExecStack(QObject * parent)
	:QObject(parent)
{
	current_file = 0;

#define REFS_SIZE_ESTACK ES_REFS_SIZE(MAX_ESTACK)

	XWPSRef stk;	
	stk.makeArray(0, REFS_SIZE_ESTACK);
	XWPSRef euop;
	euop.makeOper(0, &XWPSContextState::estackUnderflow);
	
	init(&stk, OS_GUARD_UNDER, OS_GUARD_OVER, &euop, NULL);
	setMaxCount(MAX_ESTACK);
	
	stack.setErrorCodes(XWPSError::ExecStackUnderflow, XWPSError::ExecstackOverFlow);
	stack.allowExpansion(false);
	
#undef REFS_SIZE_ESTACK
}

XWPSExecStack::~XWPSExecStack()
{
}

void XWPSExecStack::allowExpansion(bool expand)
{
	stack.allowExpansion(expand);
}

void XWPSExecStack::checkCache()
{
	if (stack.p->hasTypeAttrs(XWPSRef::File, PS_A_EXECUTABLE))
		setCache(stack.p);
}

void XWPSExecStack::cleanup()
{
	stack.cleanup();
}

uint XWPSExecStack::count()
{
	return stack.count();
}

uint XWPSExecStack::countToMark()
{
	return stack.countToMark();
}

int XWPSExecStack::extend(uint request)
{
	return stack.extend(request);
}

XWPSDict * XWPSExecStack::getBottomDict(uint i)
{
	return stack.getBottomDict(i);
}

XWPSRef * XWPSExecStack::incCurrentTop(int c)
{
	return stack.incCurrentTop(c);
}

XWPSRef * XWPSExecStack::index(long idx)
{
	return stack.index(idx);
}

void XWPSExecStack::init(XWPSRef *pblock_array,
	                       uint bot_guard, 
	                       uint top_guard, 
	                       XWPSRef *pguard_value,
	                       XWPSRefStackParams *paramsA)
{
	stack.init(pblock_array, bot_guard, top_guard, pguard_value, paramsA);
}

long XWPSExecStack::maxCount()
{
	return stack.maxCount();
}

void XWPSExecStack::pop(uint count)
{
	stack.pop(count);
}

int XWPSExecStack::popBlock()
{
	return stack.popBlock();
}

int  XWPSExecStack::push(uint count)
{
	return stack.push(count);
}

void XWPSExecStack::restoreFixStack(bool is_estack)
{
	stack.restoreFixStack(is_estack);
}

void XWPSExecStack::setErrorCodes(int underflow_errorA, int overflow_errorA)
{
	stack.setErrorCodes(underflow_errorA, overflow_errorA);
}

int XWPSExecStack::setMargin(uint marginA)
{
	return stack.setMargin(marginA);
}

int  XWPSExecStack::setMaxCount(long nmax)
{
	return stack.setMaxCount(nmax);
}

int  XWPSExecStack::store(XWPSRef *parray, 
	                        uint count,
					                uint skip, 
					                int age, 
					                bool check)
{
	return stack.store(parray, count, skip, age, check);
}

int  XWPSExecStack::storeCheck(XWPSRef *parray, uint count, uint skip)
{
	return stack.storeCheck(parray, count, skip);
}

XWPSOpStack::XWPSOpStack(QObject * parent)
	:QObject(parent)
{
#define REFS_SIZE_OSTACK OS_REFS_SIZE(MAX_OSTACK)
	
	XWPSRef stk;
	stk.makeArray(0, REFS_SIZE_OSTACK);
	stack.init(&stk, OS_GUARD_UNDER, OS_GUARD_OVER, NULL, NULL);
	stack.setMaxCount(MAX_OSTACK);
	stack.setErrorCodes(XWPSError::StackUnderflow, XWPSError::StackOverflow);
	
#undef REFS_SIZE_OSTACK
}

XWPSOpStack::~XWPSOpStack()
{
}

void XWPSOpStack::allowExpansion(bool expand)
{
	stack.allowExpansion(expand);
}

void XWPSOpStack::cleanup()
{
	stack.cleanup();
}

uint XWPSOpStack::count()
{
	return stack.count();
}

uint XWPSOpStack::countToMark()
{
	return stack.countToMark();
}

int XWPSOpStack::extend(uint request)
{
	return stack.extend(request);
}

XWPSDict * XWPSOpStack::getBottomDict(uint i)
{
	return stack.getBottomDict(i);
}

XWPSRef * XWPSOpStack::incCurrentTop(int c)
{
	return stack.incCurrentTop(c);
}

XWPSRef * XWPSOpStack::index(long idx)
{
	return stack.index(idx);
}

void XWPSOpStack::init(XWPSRef *pblock_array,
	                     uint bot_guard, 
	                     uint top_guard, 
	                     XWPSRef *pguard_value,
	                     XWPSRefStackParams *paramsA)
{
	stack.init(pblock_array, bot_guard, top_guard, pguard_value, paramsA);
}

long XWPSOpStack::maxCount()
{
	return stack.maxCount();
}

void XWPSOpStack::pop(uint count)
{
	stack.pop(count);
}

int XWPSOpStack::popBlock()
{
	return stack.popBlock();
}

int  XWPSOpStack::push(uint count)
{
	return stack.push(count);
}

void XWPSOpStack::restoreFixStack(bool is_estack)
{
	stack.restoreFixStack(is_estack);
}

void XWPSOpStack::setErrorCodes(int underflow_errorA, int overflow_errorA)
{
	stack.setErrorCodes(underflow_errorA, overflow_errorA);
}

int XWPSOpStack::setMargin(uint marginA)
{
	return stack.setMargin(marginA);
}

int  XWPSOpStack::setMaxCount(long nmax)
{
	return stack.setMaxCount(nmax);
}

int  XWPSOpStack::store(XWPSRef *parray, 
	                        uint count,
					                uint skip, 
					                int age, 
					                bool check)
{
	return stack.store(parray, count, skip, age, check);
}

int  XWPSOpStack::storeCheck(XWPSRef *parray, uint count, uint skip)
{
	return stack.storeCheck(parray, count, skip);
}
