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
#include "XWPSStream.h"
#include "XWPSState.h"
#include "XWPSName.h"
#include "XWPSContextState.h"

class XWPSInterpError 
{
public:
	XWPSInterpError();
	
public:
	int code;
	int line;
	XWPSRef *obj;
	XWPSRef full;
} ;

XWPSInterpError::XWPSInterpError()
{
	code = 0;
	line = -1;
	obj = 0;
}

#define PACKED_SPECIAL_OPS 1

#define PS_R_TYPE_XE(rp)  PS_TYPE_XE_(((ushort *)(rp))[_OFFSET_OF_(XWPSRef, tas.type_attrs) / sizeof(ushort)])

int XWPSContextState::interpret(XWPSRef * pref, XWPSRef * perror_object)
{
	ushort *iref_packed = (ushort *)pref;
#ifdef ALIGNMENT_ALIASING_BUG
  XWPSRef *iref_temp;
#  define IREF (iref_temp = (XWPSRef *)iref_packed, iref_temp)
#else
#  define IREF ((XWPSRef *)iref_packed)
#endif
#define SET_IREF(rp) (iref_packed = (ushort *)(rp))


	int icount = 0;
	XWPSRef * iosp = op_stack.getCurrentTop();
	XWPSRef * iesp = exec_stack.getCurrentTop();
	int code = 0;
	XWPSRef token;
	XWPSRef * pvalue = 0;
	XWPSRef * whichp = 0;
	int ticks_left = 0x7fff;
	XWPSInterpError ierror;
	XWPSIntGState * iigs;
	XWPSIntRemapColorInfo * irci;
	
#define set_error(ecode)\
  { ierror.code = ecode; ierror.line = __LINE__; }
#define return_with_error(ecode, objp)\
  { set_error(ecode); ierror.obj = objp; goto rwe; }
#define return_with_error_iref(ecode)\
  { set_error(ecode); goto rwei; }
#define return_with_code_iref()\
  { ierror.line = __LINE__; goto rweci; }
#define return_with_error_code_op(nargs)\
  return_with_code_iref()
#define return_with_stackoverflow(objp)\
  { op_stack.getStack()->setRequested(1); return_with_error(XWPSError::StackOverflow, objp); }
#define return_with_stackoverflow_iref()\
  { op_stack.getStack()->setRequested(1); return_with_error_iref(XWPSError::StackOverflow); }
	
	
	exec_stack.clearCache();
	
#define IREF_NEXT(ip)\
  ((ushort *)((XWPSRef *)(ip) + 1))
#define IREF_NEXT_EITHER(ip)\
  ( r_is_packed(ip) ? (ip) + 1 : IREF_NEXT(ip) )
#define store_state(ep)\
  ( icount > 0 ? (ep->setArrayPtr(IREF + 1), ep->setSize(icount)) : 0 )
#define store_state_short(ep)\
  ( icount > 0 ? (ep->setPackedPtr(iref_packed + 1), ep->setSize(icount)) : 0 )
#define store_state_either(ep)\
  ( icount > 0 ? (ep->setPackedPtr(IREF_NEXT_EITHER(iref_packed)), ep->setSize(icount)) : 0 )
#define next()\
  if ( --icount > 0 ) { iref_packed = IREF_NEXT(iref_packed); goto top; } else goto out
#define next_short()\
  if ( --icount <= 0 ) { if ( icount < 0 ) goto up; iesp--; }\
  ++iref_packed; goto top
#define next_either()\
  if ( --icount <= 0 ) { if ( icount < 0 ) goto up; iesp--; }\
  iref_packed = IREF_NEXT_EITHER(iref_packed); goto top
  
#if !PACKED_SPECIAL_OPS
#  undef next_either
#  define next_either() next()
#  undef store_state_either
#  define store_state_either(ep) store_state(ep)
#endif
  
	if (iesp >= exec_stack.getTop())
		return_with_error(XWPSError::ExecstackOverFlow, pref);
			
	++iesp;
	iesp->assign(pref);	
	goto bot;
	
top:
	switch (PS_R_TYPE_XE(iref_packed))
	{
		case PS_TYPE_XE_VALUE(XWPSRef::Invalid, 0):
		case PS_TYPE_XE_VALUE(XWPSRef::Invalid, PS_A_EXECUTABLE):
			return_with_error_iref(XWPSError::Fatal);
			break;
			
		case PS_TYPE_XE_VALUE(XWPSRef::Array, PS_A_EXECUTABLE):
		case PS_TYPE_XE_VALUE(XWPSRef::Dictionary, PS_A_EXECUTABLE):
		case PS_TYPE_XE_VALUE(XWPSRef::File, PS_A_EXECUTABLE):
		case PS_TYPE_XE_VALUE(XWPSRef::String, PS_A_EXECUTABLE):
		case PS_TYPE_XE_VALUE(XWPSRef::MixedArray, PS_A_EXECUTABLE):
		case PS_TYPE_XE_VALUE(XWPSRef::ShortArray, PS_A_EXECUTABLE):
			return_with_error_iref(XWPSError::InvalidAccess);
			break;
			
		case PS_TYPE_XE_VALUE(XWPSRef::Array, PS_A_EXECUTE):
		case PS_TYPE_XE_VALUE(XWPSRef::Array, 0):
		case PS_TYPE_XE_VALUE(XWPSRef::Boolean, 0):
		case PS_TYPE_XE_VALUE(XWPSRef::Boolean, PS_A_EXECUTABLE):
		case PS_TYPE_XE_VALUE(XWPSRef::Dictionary, PS_A_EXECUTE):
		case PS_TYPE_XE_VALUE(XWPSRef::Dictionary, 0):
		case PS_TYPE_XE_VALUE(XWPSRef::File, PS_A_EXECUTE):
		case PS_TYPE_XE_VALUE(XWPSRef::File, 0):
		case PS_TYPE_XE_VALUE(XWPSRef::FontID, 0):
		case PS_TYPE_XE_VALUE(XWPSRef::FontID, PS_A_EXECUTABLE):
		case PS_TYPE_XE_VALUE(XWPSRef::Integer, 0):
		case PS_TYPE_XE_VALUE(XWPSRef::Integer, PS_A_EXECUTABLE):
		case PS_TYPE_XE_VALUE(XWPSRef::Mark, 0):
		case PS_TYPE_XE_VALUE(XWPSRef::Mark, PS_A_EXECUTABLE):
		case PS_TYPE_XE_VALUE(XWPSRef::Name, 0):
		case PS_TYPE_XE_VALUE(XWPSRef::Null, 0):
		case PS_TYPE_XE_VALUE(XWPSRef::OpArray, 0):
		case PS_TYPE_XE_VALUE(XWPSRef::Operator, 0):
		case PS_TYPE_XE_VALUE(XWPSRef::Real, 0):
		case PS_TYPE_XE_VALUE(XWPSRef::Real, PS_A_EXECUTABLE):
		case PS_TYPE_XE_VALUE(XWPSRef::Save, 0):
		case PS_TYPE_XE_VALUE(XWPSRef::Save, PS_A_EXECUTABLE):
		case PS_TYPE_XE_VALUE(XWPSRef::String, PS_A_EXECUTE):
		case PS_TYPE_XE_VALUE(XWPSRef::String, 0):
		case PS_TYPE_XE_VALUE(XWPSRef::MixedArray, PS_A_EXECUTE):
		case PS_TYPE_XE_VALUE(XWPSRef::MixedArray, 0):
		case PS_TYPE_XE_VALUE(XWPSRef::ShortArray, PS_A_EXECUTE):
		case PS_TYPE_XE_VALUE(XWPSRef::ShortArray, 0):
		case PS_TYPE_XE_VALUE(XWPSRef::Device, 0):
		case PS_TYPE_XE_VALUE(XWPSRef::Device, PS_A_EXECUTABLE):
		case PS_TYPE_XE_VALUE(XWPSRef::Struct, 0):
		case PS_TYPE_XE_VALUE(XWPSRef::Struct, PS_A_EXECUTABLE):
		case PS_TYPE_XE_VALUE(XWPSRef::AStruct, 0):
		case PS_TYPE_XE_VALUE(XWPSRef::AStruct, PS_A_EXECUTABLE):
			break;
			
		case PS_TYPE_XE_VALUE(XWPSRef::Array, PS_A_EXECUTE + PS_A_EXECUTABLE):
		case PS_TYPE_XE_VALUE(XWPSRef::MixedArray, PS_A_EXECUTE + PS_A_EXECUTABLE):
		case PS_TYPE_XE_VALUE(XWPSRef::ShortArray, PS_A_EXECUTE + PS_A_EXECUTABLE):
			break;
			
		case PS_TYPE_XE_VALUE(tx_op_add, PS_A_EXECUTABLE):

x_add:
			if ((code = zopAdd(iosp)) < 0)
				return_with_error_code_op(2);
			iosp--;
			next_either();
			
		case PS_TYPE_XE_VALUE(tx_op_def, PS_A_EXECUTABLE):
			
x_def:
			op_stack.setCurrentTop(iosp);
			if ((code = zopDef()) < 0)
				return_with_error_code_op(2);
	    iosp -= 2;
	    next_either();
			
		case PS_TYPE_XE_VALUE(tx_op_dup, PS_A_EXECUTABLE):
			
x_dup:
			if (iosp < op_stack.getBottom())
				return_with_error_iref(XWPSError::StackUnderflow);
			
			if (iosp >= op_stack.getTop())
				return_with_stackoverflow_iref();
			
			iosp++;
			iosp->assign(iosp - 1);
			next_either();
			
		case PS_TYPE_XE_VALUE(tx_op_exch, PS_A_EXECUTABLE):
			
x_exch:
			if (iosp <= op_stack.getBottom())
				return_with_error_iref(XWPSError::StackUnderflow);
					
			token.assign(iosp);
			iosp->assign(iosp - 1);
			iosp[-1].assign(&token);
			next_either();
			
		case PS_TYPE_XE_VALUE(tx_op_if, PS_A_EXECUTABLE):
			
x_if:	
			if (!iosp[-1].hasType(XWPSRef::Boolean))
				return_with_error_iref(iosp <= op_stack.getBottom() ? XWPSError::StackUnderflow : XWPSError::TypeCheck);
			
			if (!iosp->isProc())
				return_with_error_iref(iosp->checkProcFailed());
			
			if (!iosp[-1].getBool())
			{
				iosp -= 2;
				next_either();
			}
			
			if (iesp >= exec_stack.getTop())
				return_with_error_iref(XWPSError::ExecstackOverFlow);
					
			store_state_either(iesp);
	    whichp = iosp;
	    iosp -= 2;
	    goto ifup;
	    
	  case PS_TYPE_XE_VALUE(tx_op_ifelse, PS_A_EXECUTABLE):
	  	
x_ifelse:
			if (!iosp[-2].hasType(XWPSRef::Boolean))
				return_with_error_iref((iosp < (op_stack.getBottom() + 2)) ? XWPSError::StackUnderflow : XWPSError::TypeCheck);
			
			if (!iosp[-1].isProc())
				return_with_error_iref(iosp[-1].checkProcFailed());
				
			if (!iosp->isProc())
				return_with_error_iref(iosp->checkProcFailed());
				
			if (iesp >= exec_stack.getTop())
				return_with_error_iref(XWPSError::ExecstackOverFlow);
					
			store_state_either(iesp);
	    whichp = (iosp[-2].value.boolval ? iosp - 1 : iosp);
	    iosp -= 3;
	    
ifup:
			if ((icount = whichp->size() - 1) <= 0)
			{
				if (icount < 0)
		    	goto up;
		    	
		    SET_IREF(whichp->getArray());
		    if (--ticks_left > 0)
		    	goto top;
			}
		    
		  ++iesp;
		  iesp->assign(whichp);
	    SET_IREF(whichp->getArray());
	    if (--ticks_left > 0)
				goto top;
	    goto slice;
		  
		case PS_TYPE_XE_VALUE(tx_op_index, PS_A_EXECUTABLE):
			
x_index: 
			op_stack.setCurrentTop(iosp);
			if ((code = zindex()) < 0)
				return_with_error_code_op(1);
	    next_either();
			
		case PS_TYPE_XE_VALUE(tx_op_pop, PS_A_EXECUTABLE):
			
x_pop:
			if (iosp < op_stack.getBottom())
				return_with_error_iref(XWPSError::StackUnderflow);
			iosp--;
			next_either();
			
		case PS_TYPE_XE_VALUE(tx_op_roll, PS_A_EXECUTABLE):
			
x_roll:
			op_stack.setCurrentTop(iosp);
			if ((code = zroll()) < 0)
				return_with_error_code_op(2);
				
			iosp -= 2;
			next_either();
			
		case PS_TYPE_XE_VALUE(tx_op_sub, PS_A_EXECUTABLE):
			
x_sub:
			if ((code = zopSub(iosp)) < 0)
				return_with_error_code_op(2);
			iosp--;
			next_either();
			
		case PS_TYPE_XE_VALUE(XWPSRef::Null, PS_A_EXECUTABLE):
			goto bot;
			
		case PS_TYPE_XE_VALUE(XWPSRef::OpArray, PS_A_EXECUTABLE):
			pvalue = IREF->getArray();
			
opst:
			store_state(iesp);
				
oppr:
			if (iesp >= (exec_stack.getTop() - 3))
				return_with_error_iref(XWPSError::ExecstackOverFlow);
			iesp += 4;
			op_stack.setCurrentTop(iosp);
			iesp[-3].makeMarkEStack(0, &XWPSContextState::opArrayCleanup);
			iesp[-2].makeInt(op_stack.count());
			iesp[-1].makeInt(dict_stack.count());
			iesp->makeOper(0, &XWPSContextState::opArrayPop);
			goto pr;
		
prst:
			store_state(iesp);
				
pr:	
			if ((icount = pvalue->size() - 1) <= 0)
			{
				if (icount < 0)
		    	goto up;
		    	
		    SET_IREF(pvalue->getArray());	
		    	
		    if (--ticks_left > 0)
		    	goto top;
			}
			
			if (iesp >= exec_stack.getTop())
				return_with_error_iref(XWPSError::ExecstackOverFlow);
					
			++iesp;
			iesp->assign(pvalue);
			SET_IREF(pvalue->getArray());
	    if (--ticks_left > 0)
				goto top;
	    goto slice;
	    
	 	case PS_TYPE_XE_VALUE(XWPSRef::Operator, PS_A_EXECUTABLE):
	 		exec_stack.setCurrentTop(iesp);
	 		op_stack.setCurrentTop(iosp);
	 		switch (code = IREF->callProc(this))
	 		{
	 			case 0:
	 			case 1:
	 				iosp = op_stack.getCurrentTop();
	 				next();
					
				case PS_O_PUSH_ESTACK:
					store_state(iesp);
						
opush:
					iosp = op_stack.getCurrentTop();
					iesp = exec_stack.getCurrentTop();
		    	if (--ticks_left > 0)
						goto up;
		    	goto slice;
		    	
		    case PS_O_POP_ESTACK:

opop:
					iosp = op_stack.getCurrentTop();
		    	if (exec_stack.getCurrentTop() == iesp)
						goto bot;
		    	iesp = exec_stack.getCurrentTop();
		    	goto up;
		    	
		    case PS_O_RESCHEDULE:
		    	store_state(iesp);
		    	goto res;
					
				case XWPSError::RemapColor:
					
oe_remap:
					store_state(iesp);
						
remap:
					if ((iesp + 2) >= exec_stack.getTop())
					{
						exec_stack.setCurrentTop(iesp);
						code = exec_stack.extend(2);
						if (code < 0)
							return_with_error_iref(code);
						iesp = exec_stack.getCurrentTop();
					}
					packedGet(iref_packed, iesp + 1);
					iigs = (XWPSIntGState*)(pgs->client_data);
					irci = (XWPSIntRemapColorInfo*)(iigs->remap_color_info.getStruct());
		    	iesp[2].makeOper(0, irci->proc);
		    	iesp += 2;
		    	goto up;
	 		}
	 		iosp = op_stack.getCurrentTop();
	    iesp = exec_stack.getCurrentTop();
	    return_with_code_iref();
	    
	  case PS_TYPE_XE_VALUE(XWPSRef::Name, PS_A_EXECUTABLE):
	  	pvalue = IREF->getName()->getValue();
	  	if (!((ulong)pvalue > 1))
	  	{
	  		uint nidx = the_name_table->namesIndex(IREF);
				uint htemp;
				pvalue = dict_stack.findNameByIndex(this, nidx, htemp);
				if (pvalue == 0)
					return_with_error_iref(XWPSError::Undefined);
	  	}
	  	
	  	switch (PS_R_TYPE_XE(pvalue))
	  	{
	  		case PS_TYPE_XE_VALUE(XWPSRef::Invalid, 0):
				case PS_TYPE_XE_VALUE(XWPSRef::Invalid, PS_A_EXECUTABLE):
					return_with_error_iref(XWPSError::Fatal);
					
				case PS_TYPE_XE_VALUE(XWPSRef::Array, PS_A_EXECUTABLE):
				case PS_TYPE_XE_VALUE(XWPSRef::Dictionary, PS_A_EXECUTABLE):
				case PS_TYPE_XE_VALUE(XWPSRef::File, PS_A_EXECUTABLE):
				case PS_TYPE_XE_VALUE(XWPSRef::String, PS_A_EXECUTABLE):
				case PS_TYPE_XE_VALUE(XWPSRef::MixedArray, PS_A_EXECUTABLE):
				case PS_TYPE_XE_VALUE(XWPSRef::ShortArray, PS_A_EXECUTABLE):
					return_with_error_iref(XWPSError::InvalidAccess);
					
				case PS_TYPE_XE_VALUE(XWPSRef::Array, PS_A_EXECUTE):
				case PS_TYPE_XE_VALUE(XWPSRef::Array, 0):
				case PS_TYPE_XE_VALUE(XWPSRef::Boolean, 0):
				case PS_TYPE_XE_VALUE(XWPSRef::Boolean, PS_A_EXECUTABLE):
				case PS_TYPE_XE_VALUE(XWPSRef::Dictionary, PS_A_EXECUTE):
				case PS_TYPE_XE_VALUE(XWPSRef::Dictionary, 0):
				case PS_TYPE_XE_VALUE(XWPSRef::File, PS_A_EXECUTE):
				case PS_TYPE_XE_VALUE(XWPSRef::File, 0):
				case PS_TYPE_XE_VALUE(XWPSRef::FontID, 0):
				case PS_TYPE_XE_VALUE(XWPSRef::FontID, PS_A_EXECUTABLE):
				case PS_TYPE_XE_VALUE(XWPSRef::Integer, 0):
				case PS_TYPE_XE_VALUE(XWPSRef::Integer, PS_A_EXECUTABLE):
				case PS_TYPE_XE_VALUE(XWPSRef::Mark, 0):
				case PS_TYPE_XE_VALUE(XWPSRef::Mark, PS_A_EXECUTABLE):
				case PS_TYPE_XE_VALUE(XWPSRef::Name, 0):
				case PS_TYPE_XE_VALUE(XWPSRef::Null, 0):
				case PS_TYPE_XE_VALUE(XWPSRef::OpArray, 0):
				case PS_TYPE_XE_VALUE(XWPSRef::Operator, 0):
				case PS_TYPE_XE_VALUE(XWPSRef::Real, 0):
				case PS_TYPE_XE_VALUE(XWPSRef::Real, PS_A_EXECUTABLE):
				case PS_TYPE_XE_VALUE(XWPSRef::Save, 0):
				case PS_TYPE_XE_VALUE(XWPSRef::Save, PS_A_EXECUTABLE):
				case PS_TYPE_XE_VALUE(XWPSRef::String, PS_A_EXECUTE):
				case PS_TYPE_XE_VALUE(XWPSRef::String, 0):
				case PS_TYPE_XE_VALUE(XWPSRef::MixedArray, PS_A_EXECUTE):
				case PS_TYPE_XE_VALUE(XWPSRef::MixedArray, 0):
				case PS_TYPE_XE_VALUE(XWPSRef::ShortArray, PS_A_EXECUTE):
				case PS_TYPE_XE_VALUE(XWPSRef::ShortArray, 0):
				case PS_TYPE_XE_VALUE(XWPSRef::Device, 0):
				case PS_TYPE_XE_VALUE(XWPSRef::Device, PS_A_EXECUTABLE):
				case PS_TYPE_XE_VALUE(XWPSRef::Struct, 0):
				case PS_TYPE_XE_VALUE(XWPSRef::Struct, PS_A_EXECUTABLE):
				case PS_TYPE_XE_VALUE(XWPSRef::AStruct, 0):
				case PS_TYPE_XE_VALUE(XWPSRef::AStruct, PS_A_EXECUTABLE):
					if (iosp >= op_stack.getTop())
						return_with_stackoverflow(pvalue);
		    	++iosp;
		    	iosp->assign(pvalue);
		    	next();
					
				case PS_TYPE_XE_VALUE(XWPSRef::Array, PS_A_EXECUTE + PS_A_EXECUTABLE):
				case PS_TYPE_XE_VALUE(XWPSRef::MixedArray, PS_A_EXECUTE + PS_A_EXECUTABLE):
				case PS_TYPE_XE_VALUE(XWPSRef::ShortArray, PS_A_EXECUTE + PS_A_EXECUTABLE):
					goto prst;
					
				case PS_TYPE_XE_VALUE(tx_op_add, PS_A_EXECUTABLE):
					goto x_add;
					
				case PS_TYPE_XE_VALUE(tx_op_def, PS_A_EXECUTABLE):
					goto x_def;
					
				case PS_TYPE_XE_VALUE(tx_op_dup, PS_A_EXECUTABLE):
					goto x_dup;
					
				case PS_TYPE_XE_VALUE(tx_op_exch, PS_A_EXECUTABLE):
					goto x_exch;
					
				case PS_TYPE_XE_VALUE(tx_op_if, PS_A_EXECUTABLE):
					goto x_if;
					
				case PS_TYPE_XE_VALUE(tx_op_ifelse, PS_A_EXECUTABLE):
					goto x_ifelse;
					
				case PS_TYPE_XE_VALUE(tx_op_index, PS_A_EXECUTABLE):
					goto x_index;
					
				case PS_TYPE_XE_VALUE(tx_op_pop, PS_A_EXECUTABLE):
					goto x_pop;
					
				case PS_TYPE_XE_VALUE(tx_op_roll, PS_A_EXECUTABLE):
					goto x_roll;
					
				case PS_TYPE_XE_VALUE(tx_op_sub, PS_A_EXECUTABLE):
					goto x_sub;
					
				case PS_TYPE_XE_VALUE(XWPSRef::Null, PS_A_EXECUTABLE):
					goto bot;
					
				case PS_TYPE_XE_VALUE(XWPSRef::OpArray, PS_A_EXECUTABLE):
					pvalue = pvalue->getArray();
					goto opst;
					
				case PS_TYPE_XE_VALUE(XWPSRef::Operator, PS_A_EXECUTABLE):
					{
						exec_stack.setCurrentTop(iesp);
						op_stack.setCurrentTop(iosp);
						switch (code = pvalue->callProc(this))
						{
							case 0:
							case 1:
								iosp = op_stack.getCurrentTop();
								next();
								
							case PS_O_PUSH_ESTACK:
								store_state(iesp);
								goto opush;
								
							case PS_O_POP_ESTACK:
								goto opop;
								
							case PS_O_RESCHEDULE:
								store_state(iesp);
								goto res;
								
							case XWPSError::RemapColor:
								goto oe_remap;
						}
						
						iosp = op_stack.getCurrentTop();
						iesp = exec_stack.getCurrentTop();
						return_with_error(code, pvalue);
					}
					
				case PS_TYPE_XE_VALUE(XWPSRef::Name, PS_A_EXECUTABLE):
				case PS_TYPE_XE_VALUE(XWPSRef::File, PS_A_EXECUTE + PS_A_EXECUTABLE):
				case PS_TYPE_XE_VALUE(XWPSRef::String, PS_A_EXECUTE + PS_A_EXECUTABLE):
				default:
					store_state(iesp);
					icount = 0;
					SET_IREF(pvalue);
					goto top;
	  	}
	  	
	  case PS_TYPE_XE_VALUE(XWPSRef::File, PS_A_EXECUTE + PS_A_EXECUTABLE):
	  	{
	  		XWPSScannerState sstate;
	  		
	  		XWPSStream * s = IREF->getStream();
	  		if (s->read_id != IREF->size())
	  		{
	  			if (s->read_id == 0 && s->write_id == IREF->size())
	  			{
	  				int fcode = fileSwitchToRead(IREF);
	  				if (fcode < 0)
	  					return_with_error_iref(fcode);
	  			}
	  			else
	  				s = &invalid_file_stream;
	  		}	  		

rt:
				if (iosp >= op_stack.getTop())
					return_with_stackoverflow_iref();
					
				op_stack.setCurrentTop(iosp);
				sstate.init(scanner_options);
				
again:
				code = scanToken(s, &token, &sstate);
				iosp = op_stack.getCurrentTop();
				switch (code)
				{
					case 0:
						if (!token.hasAttr(PS_A_EXECUTABLE) ||  token.isArray()) 
						{
			    		iosp++;
			    		iosp->assign(&token);
			    		goto rt;
						}
						store_state(iesp);
						if (iesp >= exec_stack.getTop())
							return_with_error_iref(XWPSError::ExecstackOverFlow);
						exec_stack.setCache(++iesp);
						iesp->assign(IREF);
						SET_IREF(&token);
						icount = 0;
						goto top;
						
					case scan_EOF:
						exec_stack.clearCache();
						goto bot;
						
					case scan_BOS:
						store_state(iesp);
						if (iesp >= exec_stack.getTop())
							return_with_error_iref(XWPSError::ExecstackOverFlow);
						exec_stack.setCache(++iesp);
						iesp->assign(IREF);
						pvalue = &token;
						goto pr;
						
					case scan_Refill:
						store_state(iesp);
						token.assign(IREF);
						if (iesp >= exec_stack.getTop())
							return_with_error_iref(XWPSError::ExecstackOverFlow);
						++iesp;
						iesp->assign(&token);
						exec_stack.setCurrentTop(iesp);
						op_stack.setCurrentTop(iosp);
						code = scanHandleRefill(&token, &sstate, true, true, &XWPSContextState::ztokenExecContinue);
		
scan_cont:
						iosp = op_stack.getCurrentTop();
						iesp = exec_stack.getCurrentTop();
						switch (code)
						{
							case 0:
								iesp--;
								goto again;
								
			    		case PS_O_PUSH_ESTACK:
								exec_stack.clearCache();
								if (--ticks_left > 0)
				    			goto up;
								goto slice;
						}
						iesp--;
						return_with_code_iref();
						
					case scan_Comment:
		    	case scan_DSC_Comment:
		    		{
							XWPSRef file_token;
							store_state(iesp);
							file_token.assign(IREF);
							if (iesp >= exec_stack.getTop())
								return_with_error_iref(XWPSError::ExecstackOverFlow);
							++iesp;
							iesp->assign(&file_token);
							exec_stack.setCurrentTop(iesp);
							op_stack.setCurrentTop(iosp);
							code = ztokenHandleComment(&file_token, &sstate, &token, code, true, true, &XWPSContextState::ztokenExecContinue);
		    		}
		    		goto scan_cont;
		    		
		    	default:
		    		return_with_code_iref();
				}
	  	}
	  	
	  case PS_TYPE_XE_VALUE(XWPSRef::String, PS_A_EXECUTE + PS_A_EXECUTABLE):
	  	{
	  		XWPSStream ss;
				XWPSScannerState sstate;
				sstate.init(SCAN_FROM_STRING);
				ss.readString(IREF->getBytes(), IREF->size(), false);
				ss.cbuf_bytes = IREF->value.bytes->arr;
				if (ss.cbuf_bytes)
					ss.cbuf_bytes->incRef();
				op_stack.setCurrentTop(iosp);
				code = scanToken(&ss, &token, &sstate);
				iosp = op_stack.getCurrentTop();
				switch (code)
				{
					case 0:
		    	case scan_BOS:
		    		store_state(iesp);
						{
							uint size = ss.bufAvailable();
							if (size)
							{
								if (iesp >= exec_stack.getTop())
									return_with_error_iref(XWPSError::ExecstackOverFlow);
								++iesp;
								if (ss.cbuf_bytes)
									ss.cbuf_bytes->incRef();
								iesp->assignRef(IREF, size, ss.bufPtr());
								iesp->value.bytes->arr = ss.cbuf_bytes;								
							}
						}
						
						if (code == 0) 
						{
			    		SET_IREF(&token);
			    		icount = 0;
			    		goto top;
						}
						pvalue = &token;
						goto pr;
						
					case scan_EOF:
						goto bot;
						
					case scan_Refill:
						code = (int)(XWPSError::SyntaxError);
							
					default:
						return_with_code_iref();
				}
	  	}
	  	
	  default:
	  	{
	  		uint index;
	  		switch (*iref_packed >> PS_R_PACKED_TYPE_SHIFT)
	  		{
	  			case pt_full_ref:
		    	case pt_full_ref + 1:
		    		if (iosp >= op_stack.getTop())
		    			return_with_stackoverflow_iref();
		    		++iosp;
		    		iosp->assign(IREF);
		    		next();
						
					case pt_executable_operator:
						index = *iref_packed & packed_value_mask;
						if (!opIndexIsOperator(index))
						{
							store_state_short(iesp);
							index -= op_def_count;
			    		pvalue = (XWPSRef *)(index < op_array_table_global.table.size() ?
			      						(op_array_table_global.table.getArray() + index) :
			       						(op_array_table_local.table.getArray() + (index - op_array_table_global.table.size())));
			    		goto oppr;
						}
						
#if PACKED_SPECIAL_OPS

						switch (index)
						{
							case tx_op_add - tx_op + 1:
								goto x_add;
								
			      	case tx_op_def - tx_op + 1: 
			      		goto x_def;
			      		
			      	case tx_op_dup - tx_op + 1: 
			      		goto x_dup;
			      		
			        case tx_op_exch - tx_op + 1: 
			        	goto x_exch;
			        	
			      	case tx_op_if - tx_op + 1:
			      		goto x_if;
			      		
			      	case tx_op_ifelse - tx_op + 1:
			      		goto x_ifelse;
			      		
			      	case tx_op_index - tx_op + 1:
			      		goto x_index;
			      		
			      	case tx_op_pop - tx_op + 1:
			      		goto x_pop;
			      		
			      	case tx_op_roll - tx_op + 1:
			      		goto x_roll;
			      		
			      	case tx_op_sub - tx_op + 1:
			      		goto x_sub;
			      		
			    		case 0:
			    		default:
			    			break;
						}
#endif
						exec_stack.setCurrentTop(iesp);
						op_stack.setCurrentTop(iosp);
						code = (this->*opIndexProc(index))();
						switch (code)
						{
							case 0:
			    		case 1:
			    			iosp = op_stack.getCurrentTop();
			    			next_short();
			    			
			    		case PS_O_PUSH_ESTACK:
								store_state_short(iesp);
								goto opush;
								
							case PS_O_POP_ESTACK:
								iosp = op_stack.getCurrentTop();
								if (iesp == exec_stack.getCurrentTop()) 
								{
				    			next_short();
								}
								iesp = exec_stack.getCurrentTop();
								goto up;
								
							case PS_O_RESCHEDULE:
								store_state_short(iesp);
								goto res;
								
							 case XWPSError::RemapColor:
									store_state_short(iesp);
									goto remap;
						}
						iosp = op_stack.getCurrentTop();
						iesp = exec_stack.getCurrentTop();
						return_with_code_iref();
						
					case pt_integer:
						if (iosp >= op_stack.getTop())
							return_with_stackoverflow_iref();
						++iosp;
						iosp->makeInt(((int)*iref_packed & packed_int_mask) + packed_min_intval);
						next_short();
						
					case pt_literal_name:
						{
							 uint nidx = *iref_packed & packed_value_mask;
							 if (iosp >= op_stack.getTop())
							 	return_with_stackoverflow_iref();
							 ++iosp;
							 nameIndexRef(nidx, iosp);
							 next_short();
						}
						
					case pt_executable_name:
						{
							uint nidx = *iref_packed & packed_value_mask;
							pvalue = (nameIndexPtr(nidx))->getValue();
							if (!((ulong)pvalue > 1))
							{
								uint htemp = 0;
								pvalue = dict_stack.findNameByIndex(this, nidx, htemp);
								if (pvalue == 0)
								{
									the_name_table->namesIndexRef(nidx,&token);
									return_with_error(XWPSError::Undefined, &token);
								}
							}
							if (pvalue->hasMaskedAttrs(PS_A_EXECUTE, PS_A_EXECUTE + PS_A_EXECUTABLE))
							{
								if (iosp >= op_stack.getTop())
									 return_with_stackoverflow_iref();
								++iosp;
								iosp->assign(pvalue);
								next_short();
							}
							
							if (pvalue->isProc())
							{
								store_state_short(iesp);
								goto pr;
							}
							store_state_short(iesp);
							icount = 0;
							SET_IREF(pvalue);
							goto top;
						}
	  		}
	  	}
	}
	if (iosp >= op_stack.getTop())
		return_with_stackoverflow_iref();
	++iosp;
	iosp->assign(IREF);
	
bot:
	next();
	
out:
	if (!icount)
	{
		iesp--;
		iref_packed = IREF_NEXT(iref_packed);
		goto top;
	}
	
up:
	if (--ticks_left < 0)
		goto slice;
		
	if (!iesp->isProc()) 
	{
		SET_IREF(iesp--);
		icount = 0;
		goto top;
  }
  
  SET_IREF(iesp->getArray());
  icount = iesp->size() - 1;
  if (icount <= 0)
  {
  	iesp--;	
		if (icount < 0)
	    goto up;
  }
  goto top;
  
res:
sched:
	if (code < 0)
	{
		set_error(code);
		ierror.full.makeArray(PS_A_EXECUTABLE + PS_A_READONLY);
		SET_IREF(ierror.obj = &ierror.full);
		goto error_exit;
	}
		
	iosp = op_stack.getCurrentTop();
	iesp = exec_stack.getCurrentTop();
	goto up;
	
slice:
	op_stack.setCurrentTop(iosp);
	exec_stack.setCurrentTop(iesp);
	code = 0;
  ticks_left = 0x7fff;
  goto sched;
  
rweci:
	ierror.code = code;
rwei:
	ierror.obj = IREF;
rwe:
	if (!r_is_packed(iref_packed))
		store_state(iesp);
	else
	{
		packedGet((ushort *)ierror.obj, &ierror.full);
		store_state_short(iesp);
		if (IREF == ierror.obj)
	    SET_IREF(&ierror.full);
		ierror.obj = &ierror.full;
	}
		
error_exit:
	op_stack.setCurrentTop(iosp);
	exec_stack.setCurrentTop(iesp);
	perror_object->assign(ierror.obj);
	return ierror.code;
	
#undef IREF
#undef SET_IREF
#undef set_error
#undef return_with_error
#undef return_with_error_iref
#undef return_with_code_iref
#undef return_with_error_code_op
#undef return_with_stackoverflow
#undef return_with_stackoverflow_iref
#undef IREF_NEXT
#undef IREF_NEXT_EITHER
#undef store_state
#undef store_state_short
#undef store_state_either
#undef next
#undef next_short
#undef next_either
}

int XWPSContextState::opArrayCleanup()
{
	XWPSRef * ep = exec_stack.getCurrentTop();
  uint ocount_old = (uint) ep[2].getInt();
  uint dcount_old = (uint) ep[3].getInt();
  uint ocount = op_stack.count();
  uint dcount = dict_stack.count();

  if (ocount > ocount_old)
		op_stack.pop(ocount - ocount_old);
  if (dcount > dcount_old) 
  {
		dict_stack.pop(dcount - dcount_old);
		dict_stack.setTop();
  }
  return 0;
}

XWPSRef * XWPSContextState::oparrayFind()
{
	long i;
  XWPSRef *ep;

  for (i = 0; (ep = exec_stack.index(i)) != 0; ++i) 
  {
		if (ep->isEStackMark() &&	    (ep->value.opproc == &XWPSContextState::opArrayCleanup ||
	     ep->value.opproc == &XWPSContextState::opArrayNoCleanup))
	    return ep;
  }
  return 0;
}

int XWPSContextState::opArrayPop()
{
	exec_stack.incCurrentTop(-3);
	return PS_O_POP_ESTACK;
}

int XWPSContextState::zcurrentStackProtect()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef *ep = oparrayFind();

  if (ep == 0)
  {
		return (int)(XWPSError::RangeCheck);
	}
  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->makeBool(ep->value.opproc == &XWPSContextState::opArrayCleanup);
  return 0;
}

int XWPSContextState::zsetStackProtect()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef *ep = oparrayFind();

  int code = op->checkType(XWPSRef::Boolean);
  if (code < 0)
  	return code;
  	
  if (ep == 0)
  {
		return (int)(XWPSError::RangeCheck);
	}
  ep->value.opproc =(op->value.boolval ? &XWPSContextState::opArrayCleanup : &XWPSContextState::opArrayNoCleanup);
  pop(1);
  return 0;
}
