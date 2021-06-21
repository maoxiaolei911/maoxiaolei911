/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include <stdlib.h>
#include <QByteArray>
#include <QDateTime>
#include "XWPSError.h"
#include "XWPSState.h"
#include "XWPSContextState.h"

static long gs_serialnumber = 42;

#define MAX_DEPTH 10

class XWPSRef2
{
public:
	XWPSRef2() {}
	
public:
	XWPSRef proc1, proc2;
};

int XWPSContextState::eexecParam(XWPSRef * op, ushort * pcstate)
{
	int npop = 1;

  if (op->hasType(XWPSRef::Dictionary))
  {
		++npop; 
		--op;
	}
    
  int code = op->checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
  	
  *pcstate = op->value.intval;
  if (op->value.intval != *pcstate)
		return (int)(XWPSError::RangeCheck);	
  return npop;
}

int XWPSContextState::setLanguageLevel(int new_level)
{
	int old_level = language_level;
  XWPSRef *pgdict =	dict_stack.index(dict_stack.count() - 2);
  XWPSRef *level2dict;
  int code = 0;

  if (new_level < 1 || 
  	new_level > (dict_stack.getSystemDict()->dictFindString(this, "ll3dict", &level2dict) > 0 ? 3 : 2))
	{
		return (int)(XWPSError::RangeCheck);	
	}
    
  if (dict_stack.getSystemDict()->dictFindString(this, "level2dict", &level2dict) <= 0)
		return (int)(XWPSError::Undefined);
	
  while (new_level != old_level) 
  {
		switch (old_level) 
		{
	    case 1: 
	    	{	
					XWPSRef *pdict;
					code = level2dict->dictFindString(this, "globaldict", &pdict);
					if (code > 0) 
					{
		    		if (!pdict->hasType(XWPSRef::Dictionary))
							return (int)(XWPSError::TypeCheck);
		    		pgdict->assign(pdict);
					}
					dict_auto_expand = true;
				}
				code = swapLevelDict("level2dict");
				if (code < 0)
		    	return code;
				++old_level;
				continue;
				
	    case 3:	
				code = swapLevelDict("ll3dict");
				if (code < 0)
		    	return code;
				--old_level;
				continue;
				
	    default:	
				break;
		}
		switch (new_level) 
		{
	    case 1: 
	    	{	
					int index = pgdict->dictFirst();
					XWPSRef elt[2];

					while ((index = pgdict->dictNext(this, index, &elt[0])) >= 0)
		    		if (elt[0].hasType(XWPSRef::Name))
							nameInvalidateValueCache(&elt[0]);
					pgdict->assign(dict_stack.getSystemDict());
					dict_auto_expand = false;
				}
				code = swapLevelDict("level2dict");
				break;
				
	    case 3:	
				code = swapLevelDict("ll3dict");
				break;
				
	    default:
				return (int)(XWPSError::Fatal);
		}
		break;
  }
  dict_stack.setTop();	
  return code;
}

int XWPSContextState::swapEntry(XWPSRef elt[2], XWPSRef * pdict, XWPSRef * pdict2)
{
	XWPSRef *pvalue;
  XWPSRef old_value;
  int found = pdict->dictFind(this, &elt[0], &pvalue);

  switch (found) 
  {
		default:
		case 0:	
	    old_value.makeNull();
	    break;
	    
		case 1:		/* present */
	    old_value.assign(pvalue);
  }
  
  {
		uint space2 = pdict2->space();
		int code;

		pdict2->setSpace(PS_AVM_LOCAL);
		dictPut(pdict2, &elt[0], &old_value);
		if (elt[1].hasType(XWPSRef::Null)) 
		{
	    code = dictUndef(pdict, &elt[0]);
	    if (code == XWPSError::Undefined &&	old_value.hasType(XWPSRef::Null))
				code = 0;
		} 
		else 
		{
	    uint space = pdict->space();

	    pdict->setSpace(PS_AVM_LOCAL);
	    code = dictPut(pdict, &elt[0], &elt[1]);
	    pdict->setSpace(space);
		}
		pdict2->setSpace(space2);
		return code;
  }
}

int XWPSContextState::swapLevelDict(const char *dict_name)
{
	XWPSRef *pleveldict;
  XWPSRef rleveldict;
  int index;
  XWPSRef elt[2];	
  XWPSRef *psubdict;

  if (dict_stack.getSystemDict()->dictFindString(this, dict_name, &pleveldict) <= 0)
		return (int)(XWPSError::Undefined);
  rleveldict.assign(pleveldict);
  index = rleveldict.dictFirst();
  while ((index = rleveldict.dictNext(this, index, &elt[0])) >= 0)
		if (elt[1].hasType(XWPSRef::Dictionary) &&
	    	elt[1].dictFind(this, &elt[0], &psubdict) > 0 &&
	    	elt[1].objEq(this, psubdict)) 
	  {
	    int isub = elt[1].dictFirst();
	    XWPSRef subelt[2];
	    int found = dict_stack.getSystemDict()->dictFind(this, &elt[0], &psubdict);
	    XWPSRef rsubdict;

	    if (found <= 0)
				continue;
	    rsubdict.assign(psubdict);
	    while ((isub = elt[1].dictNext(this, isub, &subelt[0])) >= 0)
				if (!subelt[0].objEq(this, &elt[0])) 
				{
		    	int code = swapEntry(subelt, &rsubdict, &elt[1]);

		    	if (code < 0)
						return code;
				}
		} 
		else 
		{
	    int code = swapEntry(elt, dict_stack.getSystemDict(), &rleveldict);

	    if (code < 0)
			return code;
		}
  return 0;
}

int XWPSContextState::type1Crypt(int (*proc)(uchar *, const uchar *, uint, ushort *))
{
	XWPSRef * op = op_stack.getCurrentTop();
  ushort state;
  uint ssize;

  int code = op[-2].checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
  	
  state = op[-2].value.intval;
  if (op[-2].value.intval != state)
		return (int)(XWPSError::RangeCheck);	
  
  code = op[-1].checkReadType(XWPSRef::String);
  if (code < 0)
  	return code;
  	
  code = op->checkWriteType(XWPSRef::String);
  if (code < 0)
  	return code;
  	
  ssize = op[-1].size();
  if (op->size() < ssize)
		return (int)(XWPSError::RangeCheck);	
  (*proc)(op->getBytes(), op[-1].getBytes(), ssize,   &state);
  op[-2].value.intval = state;
  op[-1].assign(op);
  op[-1].setSize(ssize);
  pop(1);
  return 0;
}

int XWPSContextState::zbind()
{
	XWPSRef * op = op_stack.getCurrentTop();
  uint depth = 1;
  XWPSRef defn;
  XWPSRef * bsp;

  switch (op->type()) 
  {
		case XWPSRef::Array:			
		case XWPSRef::MixedArray:
		case XWPSRef::ShortArray:
	    defn.assign(op);
	    break;
	    
		case XWPSRef::OpArray:
	    defn.assign(op->getArray());
	    break;
	    
		default:
	    return op->checkTypeFailed();
  }
  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->assign(&defn);
  bsp = op;
  
  while (depth) 
  {
		while (bsp->size()) 
		{
	    ushort *const tpp = bsp->getPacked();

	    bsp->decSize(1);
	    if (r_is_packed(tpp)) 
	    {
				ushort elt = *tpp;

				if (r_packed_is_exec_name(&elt)) 
				{
		    	XWPSRef nref;
		    	XWPSRef *pvalue;

		    	nameIndexRef(packed_name_index(&elt), &nref);
		    	if ((pvalue = dictFindName(&nref)) != 0 &&	pvalue->isExOper()) 
		    	{
						bsp->storeCheckDest(pvalue);
						*tpp = pt_tag(pt_executable_operator) + opIndex(pvalue);
		    	}
				}
				bsp->setPackedPtr(tpp + 1);
	    } 
	    else 
	    {
				XWPSRef * tp = bsp->getArray();
				bsp->setArrayPtr(tp + 1);

				switch (tp->type()) 
				{
		    	case XWPSRef::Name:
						if (tp->hasAttr(PS_A_EXECUTABLE)) 
						{
			    		XWPSRef *pvalue;

			    		if ((pvalue = dictFindName(tp)) != 0 &&	pvalue->isExOper()) 
			    		{
								bsp->storeCheckDest(pvalue);
								tp->assign(pvalue);
			    		}
						}
						break;
						
		    	case XWPSRef::Array:	
						if (!tp->hasAttr(PS_A_WRITE))
			    		break;
			    	
		    	case XWPSRef::MixedArray:
		    	case XWPSRef::ShortArray:
						if (tp->hasAttr(PS_A_EXECUTABLE)) 
						{
			    		tp->clearAttrs(PS_A_WRITE);
			    		if (bsp >= op_stack.getTop()) 
			    		{
								XWPSRef temp;
								int code;

								temp.assign(tp);
								op_stack.setCurrentTop(bsp);
								code = op_stack.push(1);
								if (code < 0) 
								{
				    			op_stack.pop(depth);
				    			return code;
								}
								bsp = op_stack.getCurrentTop();
								bsp->assign(&temp);
			    		} 
			    		else
			    		{
			    			++bsp;
								bsp->assign(tp);
							}
			    		depth++;
						}
						
					default:
						break;
				}
	  	}
		}
		bsp--;
		depth--;
		if (bsp < op_stack.getBottom()) 
		{
	  	op_stack.setCurrentTop(bsp);
	  	op_stack.popBlock();
	 		bsp = op_stack.getCurrentTop();
		}
  }
  op_stack.setCurrentTop(bsp);
  return 0;
}

int XWPSContextState::zclipRestore()
{
	return pgs->clipRestore();
}

int XWPSContextState::zclipSave()
{
	return pgs->clipSave();
}

int XWPSContextState::zeqproc()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef2 stack[MAX_DEPTH + 1];
  XWPSRef2 *top = stack;

  stack[0].proc1.makeArray(0, 1, op - 1);
  stack[0].proc2.makeArray(0, 1, op);
  for (;;) 
  {
		long i;

		if (top->proc1.size() == 0) 
		{
	    if (top == stack) 
	    {
				op[-1].makeTrue();
				pop(1);
				return 0;
	    }
	    --top;
	    continue;
		}
		
		i = top->proc1.size() - 1;
		top->proc1.arrayGet(this, i, &top[1].proc1);
		top->proc2.arrayGet(this, i, &top[1].proc2);
		top->proc1.decSize(1);
		++top;
		
		if (top->proc1.objEq(this, &top->proc2)) 
		{
	    if (top->proc1.type() != top->proc2.type() &&
					(top->proc1.type() == XWPSRef::Name ||
		 				top->proc2.type() == XWPSRef::Name))
				break;
	    --top;
	    continue;
		}
		if (top->proc1.isArray() && top->proc2.isArray() &&
	    	top->proc1.size() == top->proc2.size() && top < stack + (MAX_DEPTH - 1) ) 
	  {
	    continue;
		}
		break;
  }
  op[-1].makeFalse();
  pop(1);
  return 0;
}

int XWPSContextState::zexD()
{
	XWPSRef * op = op_stack.getCurrentTop();
  PSStreamexDState * state = new PSStreamexDState;
  int code;

  (*s_exD_template.set_defaults)((PSStreamState *)state);
  if (op->hasType(XWPSRef::Dictionary)) 
  {
		uint cstate;

		code = op->checkDictRead();
		if (code < 0)
		{
			if (state)
				delete state;
			return code;
		}
		if ((code = op->dictUintParam(this, "seed", 0, 0xffff, 0x10000,    &cstate)) < 0 ||
	    	(code = op->dictIntParam(this, "lenIV", 0, max_int, 4,  &state->lenIV)) < 0 )
	  {
	  	if (state)
				delete state;
	    return code;
	  }
		state->cstate = cstate;
		code = 1;
  } 
  else 
  {
		code = eexecParam(op, &state->cstate);
  }
  if (code < 0)
  {
  	if (state)
			delete state;
		return code;
	}
		
  if (op[-1].hasType(XWPSRef::File)) 
  {
		XWPSStream *s = (op - 1)->getStream();

		if (s->state != 0 && s->state->templat == &s_PFBD_template) 
		{
	    PSStreamPFBDState *pss = (PSStreamPFBDState *)s->state;

	    state->pfb_state = pss;
	    if (pss->record_type == 2) 
	    {
				if (pss->binary_to_hex && s->bufAvailable() > 0) 
				{
		    	state->binary = 0;	
		    	state->hex_left = s->bufAvailable();
				} 
				else 
		    	state->binary = 1;
				pss->binary_to_hex = 0;
	    }
	    state->record_left = pss->record_left;
		}
  }
  return filterRead(code, &s_exD_template, (PSStreamState *)state, 0);
}

int XWPSContextState::zexE()
{
	XWPSRef * op = op_stack.getCurrentTop();
  PSStreamexEState *state = new PSStreamexEState;
  int code = eexecParam(op, &state->cstate);

  if (code < 0)
  {
  	if (state)
  		delete state;
		return code;
	}
  return filterWrite(code, &s_exE_template, (PSStreamState *)state, 0);
}

int XWPSContextState::zgetEnv()
{
	XWPSRef * op = op_stack.getCurrentTop();
  char *str;
//  uchar *value;
//  int len = 0;

  int code = op->checkReadType(XWPSRef::String);
  if (code < 0)
  	return code;
  	
 	QByteArray ba;
 	str = new char[op->size() + 1];
 	memcpy(str, op->getBytes(), op->size());
 	str[op->size()] = 0;
 	ba = qgetenv(str);
 	delete [] str;
 	if (ba.size() <= 0)
  {	
		op->makeFalse();
		return 0;
  }
  
  code = push(&op, 1);
  if (code < 0)
		return code;
  op[-1].makeString(idmemory()->iallocSpace() | PS_A_ALL, (ushort)ba.size(), (uchar*)(ba.data()));
  op->makeTrue();
  return 0;
}

int XWPSContextState::zlanguageLevel()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op,1);
  if (code < 0)
		return code;
  op->makeInt(language_level);
  return 0;
}

int XWPSContextState::zmakeOperator()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSOpArrayTable *opt;
  uint count;
  XWPSRef *tab;

  int code = op[-1].checkType(XWPSRef::Name);
  if (code < 0)
  	return code;
  	
  op->checkProc();
  if (code < 0)
  	return code;
  	
  switch (op->space()) 
  {
		case PS_AVM_GLOBAL:
	    opt = &op_array_table_global;
	    break;
	    
		case PS_AVM_LOCAL:
	    opt = &op_array_table_local;
	    break;
	    
		default:
	    return (int)(XWPSError::InvalidAccess);
  }
  
  count = opt->count;
  tab = opt->table.getArray();
  while (count > 0 && tab[count - 1].hasType(XWPSRef::Null))
		--count;
  if (count == opt->table.size())
		return (int)(XWPSError::LimitCheck);
  tab[count].assign(op);
  opt->nx_table[count] = nameIndex(op - 1);
  opIndexRef(opt->base_index + count, op - 1);
  opt->count = count + 1;
  pop(1);
  return 0;
}

int XWPSContextState::zoserrno()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->makeInt(errno);
  return 0;
}

int XWPSContextState::zoserrorString()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code;

  code = op->checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;  	
  
	op->makeFalse();
	return 0;
}

int XWPSContextState::zrealTime()
{
	XWPSRef * op = op_stack.getCurrentTop();
  uint t = QDateTime::currentDateTimeUtc().toTime_t();
  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->makeInt(t);
  return 0;
}

int XWPSContextState::zserialNumber()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->makeInt(gs_serialnumber);
  return 0;
}

int XWPSContextState::zsetDebug()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code = op[-1].checkReadType(XWPSRef::String);
  if (code < 0)
  	return code;
  	
  code = op->checkType(XWPSRef::Boolean);
  if (code < 0)
  	return code;
  	
  pop(2);
  return 0;
}

int XWPSContextState::zsetLanguageLevel()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code = 0;

  code = op->checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
  	
  if (op->value.intval != language_level) 
  {
		code = setLanguageLevel((int)op->value.intval);
		if (code < 0)
	    return code;
  }
  language_level = op->value.intval;
  pop(1);
  return code;
}

int XWPSContextState::zsetOSErrno()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = op->checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
  	
  errno = op->value.intval;
  pop(1);
  return 0;
}

int XWPSContextState::zsetSubstituteColorSpace()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int index, code;

  code = op->checkType(XWPSRef::Boolean);
  if (code < 0)
  	return code;
  	
  code = op[-1].checkIntLEU(2);
  if (code < 0)
  	return code;
  	
  index = (int)op[-1].value.intval;
  code = pgs->setSubstituteColorSpace((PSColorSpaceIndex)index, (op->value.boolval ? pgs->currentColorSpace() : NULL));
  if (code >= 0)
		pop(2);
  return code;
}

int XWPSContextState::ztype1Decrypt()
{
	return type1Crypt(ps_type1_decrypt);
}

int XWPSContextState::ztype1Encrypt()
{
	return type1Crypt(ps_type1_encrypt);
}

int XWPSContextState::zuserTime()
{
	XWPSRef * op = op_stack.getCurrentTop();
  uint t = QDateTime::currentDateTime().toTime_t();
  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->makeInt(t);
  return 0;
}
