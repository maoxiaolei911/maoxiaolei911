/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include "XWPSError.h"
#include "XWPSContextState.h"

int XWPSContextState::condContinue()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * ep = exec_stack.getCurrentTop();
  
  int code = op->checkType(XWPSRef::Boolean);
  if (code < 0)
  	return code;
  	
  if (op->value.boolval) 
  {
		ep->arrayGet(this, 1L, ep);
		exec_stack.checkCache();
		code = PS_O_POP_ESTACK;
  } 
  else if (ep->size() > 2) 
  {
		ushort *elts = ep->getPacked();

		code = checkEStack(2);
		if (code < 0)
  		return code;
		ep->decSize(2);
		elts = packed_next(elts);
		elts = packed_next(elts);
		ep->setPackedPtr(elts);
		ep->arrayGet(this, 0L, ep + 2);
		ep[1].makeOper(0, &XWPSContextState::condContinue);
		exec_stack.setCurrentTop(ep + 2);
		exec_stack.checkCache();
		code = PS_O_PUSH_ESTACK;
  } 
  else 
  {
		exec_stack.setCurrentTop(ep-1);
		code = PS_O_POP_ESTACK;
  }
  pop(1);
  return code;
}

uint XWPSContextState::countExecStack(bool include_marks)
{
	uint count = exec_stack.count();

  if (!include_marks) 
  {
		uint i;

		for (i = count; i--;)
	    if (exec_stack.index((long)i)->hasTypeAttrs(XWPSRef::Null, PS_A_EXECUTABLE))
				--count;
  }
  return count;
}

uint XWPSContextState::countToStopped(long mask)
{
	XWPSRefStackEnum rsenum;
  uint scanned = 0;

  rsenum.begin(exec_stack.getStack());
  do 
  {
		uint used = rsenum.size;
		XWPSRef * ep = rsenum.ptr + used - 1;
		uint count = used;

		for (; count; count--, ep--)
	    if (ep->isEStackMark() &&	ep->size() == PS_ES_STOPPED && (ep[2].value.intval & mask) != 0)
				return scanned + (used - count + 1);
		scanned += used;
  } while (rsenum.next());
  return 0;
}

int XWPSContextState::doExecStack(bool include_marks, XWPSRef * op1)
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef *arefs = op1->getArray();
  uint asize = op1->size();
  uint i;
  XWPSRef *rq;
    
  for (i = 0, rq = arefs + asize; rq != arefs; ++i) 
  {
		XWPSRef *rp = exec_stack.index((long)i);

		if (rp->hasTypeAttrs(XWPSRef::Null, PS_A_EXECUTABLE) && !include_marks)
	    continue;
		--rq;
		rq->assign(rp);
		switch (rq->type()) 
		{
	    case XWPSRef::Operator: 
	    	{
					uint opidx = opIndex(rq);

					if (opidx == 0 || op_def_is_internal(opIndexDef(opidx)))
		    		rq->clearAttrs(PS_A_EXECUTABLE);
					break;
	    	}
	    	
	    case XWPSRef::Struct:
	    case XWPSRef::AStruct: 
	    	{
	    		const char *tname = rq->getTypeName();
	    		rq->makeString(PS_A_READONLY | PS_AVM_FOREIGN, strlen(tname), (uchar *)tname);
	    	}	    	
				break;
	    	
	    default:
				;
		}
  }
  pop(op - op1);
  return 0;
}

int XWPSContextState::execStackContinue()
{
	XWPSRef * op = op_stack.getCurrentTop();

  return doExecStack(false, op);
}

int XWPSContextState::execStack2Continue()
{
	XWPSRef * op = op_stack.getCurrentTop();

  return doExecStack(op->value.boolval, op - 1);
}

int XWPSContextState::forFractionContinue()
{
	XWPSRef * ep = exec_stack.getCurrentTop();
  int code = forPosIntContinue();

  if (code != PS_O_PUSH_ESTACK)
		return code;
		
	XWPSRef * op = op_stack.getCurrentTop();
  op->makeReal((float)(op_stack.getCurrentTop()->value.intval / ep[-1].value.intval));
  return code;
}

int XWPSContextState::forNegIntContinue()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * ep = exec_stack.getCurrentTop();
  long var = ep[-3].value.intval;

  if (var < ep[-1].value.intval) 
  {
		exec_stack.incCurrentTop(-5);
		return PS_O_POP_ESTACK;
  }
    
  int code = push(&op, 1);
  if (code < 0)
  	return code;
  	
  op->makeInt(var);
  ep[-3].value.intval = var + ep[-2].value.intval;
  ep[2].assign(ep);
  exec_stack.setCurrentTop(ep + 2);
  return PS_O_PUSH_ESTACK;
}

int XWPSContextState::forPosIntContinue()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * ep = exec_stack.getCurrentTop();
  long var = ep[-3].value.intval;

  if (var > ep[-1].value.intval) 
  {
		exec_stack.incCurrentTop(-5);
		return PS_O_POP_ESTACK;
  }
  
  int code = push(&op, 1);
  if (code < 0)
  	return code;
  	
  op->makeInt(var);
  ep[-3].value.intval = var + ep[-2].value.intval;
  ep[2].assign(ep);
  exec_stack.setCurrentTop(ep + 2);
  return PS_O_PUSH_ESTACK;
}

int XWPSContextState::forRealContinue()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * ep = exec_stack.getCurrentTop();
  float var = ep[-3].value.realval;
  float incr = ep[-2].value.realval;

  if (incr >= 0 ? (var > ep[-1].value.realval) : (var < ep[-1].value.realval)) 
  {
		exec_stack.incCurrentTop(-5);
		return PS_O_POP_ESTACK;
  }
  int code = push(&op, 1);
  if (code < 0)
  	return code;
  	
  op->assign(ep - 3);
  ep[-3].value.realval = var + incr;
  exec_stack.setCurrentTop(ep + 2);
  ep[2].assign(ep);
  return PS_O_PUSH_ESTACK;
}

int XWPSContextState::loopContinue()
{
	XWPSRef * ep = exec_stack.getCurrentTop();

  ep[2].assign(ep);
  exec_stack.setCurrentTop(ep + 2);
  return PS_O_PUSH_ESTACK;
}

int XWPSContextState::pushExecStack(XWPSRef * op1, bool include_marks, op_proc_t cont)
{
	uint size;
  uint depth;

  int code = op1->checkWriteType(XWPSRef::Array);
  if (code < 0)
  	return code;
  size = op1->size();
  depth = countExecStack(include_marks);
  if (depth > size)
		return (int)(XWPSError::RangeCheck);
			
  {
		int code = exec_stack.storeCheck(op1, size, 0);
		if (code < 0)
	    return code;
  }
  checkEStack(1);
  op1->setSize(depth);
  XWPSRef * ep = exec_stack.incCurrentTop(1);
  ep->makeOper(0, cont);
  return PS_O_PUSH_ESTACK;
}

int XWPSContextState::repeatContinue()
{
	XWPSRef * ep = exec_stack.getCurrentTop();

  if (--(ep[-1].value.intval) >= 0) 
  {
  	exec_stack.incCurrentTop(2);		
		exec_stack.getCurrentTop()->assign(ep);
		return PS_O_PUSH_ESTACK;
  } 
  else 
  {
		exec_stack.incCurrentTop(-3);		
		return PS_O_POP_ESTACK;
  }
}

int XWPSContextState::stoppedPush()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op, 1);
  if (code < 0)
  	return code;
  	
  op->assign(exec_stack.getCurrentTop() - 1);
  exec_stack.incCurrentTop(-3);
  return PS_O_POP_ESTACK;
}

int XWPSContextState::unmatchedExit(XWPSRef * op, op_proc_t opproc)
{
	op[-1].makeOper(0, opproc);
  op->makeInt(XWPSError::InvalidExit);
  return (int)(XWPSError::Quit);
}

int XWPSContextState::zcond()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * ep = exec_stack.getCurrentTop();
  
  if (!op->isArray())
		return (int)(XWPSError::TypeCheck);
  
  int code = op->checkExecute();
  if (code < 0)
  	return code;
  	
  if ((op->size() & 1) != 0)
		return (int)(XWPSError::RangeCheck);
	
  if (op->size() == 0)
		return zpop();
		
  code = checkEStack(3);
  if (code < 0)
  	return code;
  	
  ep += 3;
  exec_stack.setCurrentTop(ep);
  ep[-2].assign(op);
  ep[-1].makeOper(0, &XWPSContextState::condContinue);
  op->arrayGet(this, 0L, ep);
  exec_stack.checkCache();
  pop(1);
  return PS_O_PUSH_ESTACK;
}

int XWPSContextState::zcountExecStack()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->makeInt(countExecStack(false));
  return 0;
}

int XWPSContextState::zcountExecStack1()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = op->checkType(XWPSRef::Boolean);
  if (code < 0)
  	return code;
  op->makeInt(countExecStack(op->value.boolval));
  return 0;
}

int XWPSContextState::zcurrentFile()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef *fp;

  int code = push(&op, 1);
  if (code < 0)
		return code;
  if (exec_stack.currentFile() != 0) 
	  op->assign(exec_stack.currentFile());
  else if ((fp = zgetCurrentFile()) == 0) 
  {
  	invalid_file_stream.incRef();
		op->makeFile(0, 0, &invalid_file_stream);
  } 
  else 
  {
		op->assign(fp);
		exec_stack.setCache(fp);
  }
  
  op->clearAttrs(PS_A_EXECUTABLE);
  return 0;
}

int XWPSContextState::zexec()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = checkOp(op, 1);
  if (code < 0)
  	return code;
  	
  if (!op->hasAttr(PS_A_EXECUTABLE))
		return 0;
  
  code = checkEStack(1);
  if (code < 0)
  	return code;
 
 	XWPSRef * ep = exec_stack.incCurrentTop(1);
 	ep->assign(op);
 	exec_stack.checkCache();
  pop(1);
  return PS_O_PUSH_ESTACK;
}

int XWPSContextState::zexecn()
{
	XWPSRef * op = op_stack.getCurrentTop();
  uint n, i;
  XWPSRef * esp_orig;

  int code = op->checkIntLEU(max_uint - 1);
  if (code < 0)
  	return code;
  n = (uint) op->value.intval;
  code = checkOp(op, n + 1);
  if (code < 0)
  	return code;
  
  code = checkEStack(n);
  if (code < 0)
  	return code;
  	
  esp_orig = exec_stack.getCurrentTop();
  for (i = 0; i < n; ++i) 
  {
		XWPSRef *rp = op_stack.index((long)(i + 1));
		if (ref_type_uses_access(rp->type())) 
		{
	    if (!rp->hasAttr(PS_A_EXECUTE) &&
					rp->hasAttr(PS_A_EXECUTABLE)) 
			{
				exec_stack.setCurrentTop(esp_orig);
				return (int)(XWPSError::InvalidAccess);
	    }
		}
		if (!rp->hasTypeAttrs(XWPSRef::Null, PS_A_EXECUTABLE)) 
		{
	    XWPSRef * ep = exec_stack.incCurrentTop(1);
	    ep->assign(rp);
		}
  }
  exec_stack.checkCache();
  pop(n + 1);
  return PS_O_PUSH_ESTACK;
}

int XWPSContextState::zexecStack()
{
	XWPSRef * op = op_stack.getCurrentTop();

  return pushExecStack(op, false, &XWPSContextState::execStackContinue);
}

int XWPSContextState::zexecStack2()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = op->checkType(XWPSRef::Boolean);
  if (code < 0)
  	return code;
  	
  return pushExecStack(op - 1, op->value.boolval, &XWPSContextState::execStack2Continue);
}

int XWPSContextState::zexit()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRefStackEnum rsenum;
  uint scanned = 0;

  rsenum.begin(exec_stack.getStack());
  do 
  {
		uint used = rsenum.size;
		XWPSRef * ep = rsenum.ptr + used - 1;
		uint count = used;

		for (; count; count--, ep--)
	    if (ep->isEStackMark())
				switch (ep->size()) 
				{
		    	case PS_ES_FOR:
						popEStack(scanned + (used - count + 1));
						return PS_O_POP_ESTACK;
						
		    	case PS_ES_STOPPED:
						return (int)(XWPSError::InvalidExit);
				}
				scanned += used;
  } while (rsenum.next());
  
  int code = push(&op, 2);
  if (code < 0)
		return code;
  return unmatchedExit(op, &XWPSContextState::zexit);
}

int XWPSContextState::zfor()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * ep;

  int code = checkEStack(7);
  if (code < 0)
  	return code;
  	
  ep = exec_stack.getCurrentTop() + 6;
  code = op->checkProc();
  if (code < 0)
  	return code;
  	
  if (op[-3].hasType(XWPSRef::Integer) &&
			op[-2].hasType(XWPSRef::Integer)) 
	{
		ep[-4].makeInt(op[-3].value.intval);
		ep[-3].makeInt(op[-2].value.intval);
		switch (op[-1].type()) 
		{
	    case XWPSRef::Integer:
				ep[-2].makeInt(op[-1].value.intval);
				break;
				
	    case XWPSRef::Real:
				ep[-2].makeInt((long)op[-1].value.realval);
				break;
				
	    default:
				return op[-1].checkTypeFailed();
		}
		if (ep[-3].value.intval >= 0)
	    ep->makeOper(0, &XWPSContextState::forPosIntContinue);
		else
	    ep->makeOper(0, &XWPSContextState::forNegIntContinue);
  } 
  else 
  {
		float params[3];
		int code;

		if ((code = floatParams(op - 1, 3, params)) < 0)
	    return code;
		ep[-4].makeReal(params[0]);
		ep[-3].makeReal(params[1]);
		ep[-2].makeReal(params[2]);
		ep->makeOper(0, &XWPSContextState::forRealContinue);
  }
  ep[-5].makeMarkEStack(PS_ES_FOR, &XWPSContextState::noCleanup);
  ep[-1].assign(op);
  exec_stack.setCurrentTop(ep);
  pop(4);
  return PS_O_PUSH_ESTACK;
}

int XWPSContextState::zforFraction()
{
	int code = zfor();

  if (code < 0)
		return code;
		
	XWPSRef * ep = exec_stack.getCurrentTop();
  ep->makeOper(0, &XWPSContextState::forFractionContinue);
  return code;
}

XWPSRef * XWPSContextState::zgetCurrentFile()
{
	XWPSRefStackEnum rsenum;

  rsenum.begin(exec_stack.getStack());
  do 
  {
		uint count = rsenum.size;
		XWPSRef * ep = rsenum.ptr + count - 1;

		for (; count; count--, ep--)
	    if (ep->hasTypeAttrs(XWPSRef::File, PS_A_EXECUTABLE))
		return ep;
  } while (rsenum.next());
  return 0;
}

int XWPSContextState::zif()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = op[-1].checkType(XWPSRef::Boolean);
  if (code < 0)
  	return code;
  code = op->checkProc();
  if (code < 0)
  	return code;
  	
  if (op[-1].value.boolval) 
  {
		code = checkEStack(1);
		if (code < 0)
  		return code;
  		
		XWPSRef * ep = exec_stack.incCurrentTop(1);
		ep->assign(op);
		exec_stack.checkCache();
  }
  pop(2);
  return PS_O_PUSH_ESTACK;
}

int XWPSContextState::zifelse()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = op[-2].checkType(XWPSRef::Boolean);
  if (code < 0)
  	return code;
  	
  code = op[-1].checkProc();
  if (code < 0)
  	return code;
  	
  code = op->checkProc();
  if (code < 0)
  	return code;
  	
  code = checkEStack(1);
  if (code < 0)
  	return code;
  	
  XWPSRef * ep =  exec_stack.incCurrentTop(1);
  if (op[-2].value.boolval) 
		ep->assign(op - 1);
  else 
		ep->assign(op);
  exec_stack.checkCache();
  pop(3);
  return PS_O_PUSH_ESTACK;
}

int XWPSContextState::zinstopped()
{
	XWPSRef * op = op_stack.getCurrentTop();
  uint count;

  int code = op->checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
  	
  count = countToStopped(op->value.intval);
  if (count) 
  {
		code = push(&op, 1);
		if (code < 0)
			return code;
		op[-1].assign(exec_stack.index(count - 2));
		op->makeTrue();
  } 
  else
		op->makeFalse();
  return 0;
}

int XWPSContextState::zloop()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = op->checkProc();
  if (code < 0)
  	return code;
  	
  code = checkEStack(4);
  if (code < 0)
  	return code;
  	
  XWPSRef * ep = exec_stack.incCurrentTop(1);
  ep->makeMarkEStack(PS_ES_FOR, &XWPSContextState::noCleanup);
  ep = exec_stack.incCurrentTop(1);
  ep->assign(op);
  ep = exec_stack.getCurrentTop() + 1;
  ep->makeOper(0, &XWPSContextState::loopContinue);
  pop(1);
  return loopContinue();
}

int XWPSContextState::zneedInput()
{
	return (int)(XWPSError::NeedInput);
}

int XWPSContextState::zquit()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = checkOp(op, 2);
  if (code < 0)
  	return code;
  	
  code = op->checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
  	
	return (int)(XWPSError::Quit);
}

int XWPSContextState::zrepeat()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code = op[-1].checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
  	
  code = op->checkProc();
  if (code < 0)
  	return code;
  	
  if (op[-1].value.intval < 0)
		return (int)(XWPSError::RangeCheck);
	
  code = checkEStack(5);
  if (code < 0)
  	return code;
  	
  XWPSRef * ep = exec_stack.incCurrentTop(1);
  ep->makeMarkEStack(PS_ES_FOR, &XWPSContextState::noCleanup);
  ep = exec_stack.incCurrentTop(1);
  ep->assign(op - 1);
  ep = exec_stack.incCurrentTop(1);
  ep->assign(op);
  
  ep[1].makeOper(0, &XWPSContextState::repeatContinue);
  pop(2);
  return repeatContinue();
}

int XWPSContextState::zstop()
{
	XWPSRef * op = op_stack.getCurrentTop();
  uint count = countToStopped(1L);

  if (count) 
  {
		int code = checkOStack(2);
		if (code < 0)
  		return code;
  		
		popEStack(count);
		op = op_stack.getCurrentTop();
		code = push(&op, 1);
		if (code < 0)
			return code;
		op->makeTrue();
		return PS_O_POP_ESTACK;
  }
  
  int ccode = push(&op, 2);
  if (ccode < 0)
		return ccode;
  return unmatchedExit(op, &XWPSContextState::zstop);
}

int XWPSContextState::zstopped()
{
	XWPSRef * op = op_stack.getCurrentTop();
  
  int code = checkOp(op, 1);
  if (code < 0)
  	return code;
  
  code = checkEStack(5);
  if (code < 0)
  	return code;
  	
  XWPSRef * ep = exec_stack.incCurrentTop(1);
  ep->makeMarkEStack(PS_ES_STOPPED, &XWPSContextState::noCleanup);
  	
  ep = exec_stack.incCurrentTop(1);
  ep->makeFalse();
  ep = exec_stack.incCurrentTop(1);
  ep->makeInt(1);
  ep = exec_stack.incCurrentTop(1);
  ep->makeOper(0, &XWPSContextState::stoppedPush);
  ep = exec_stack.incCurrentTop(1);
  ep->assign(op);
  exec_stack.checkCache();
  pop(1);
  return PS_O_PUSH_ESTACK;
}

int XWPSContextState::zsuperExec()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * ep;

  int code = checkOp(op, 1);
  if (code < 0)
  	return code;
  	
  if (!op->hasAttr(PS_A_EXECUTABLE))
		return 0;
		
  code = checkEStack(2);
  if (code < 0)
  	return code;
  	
  ep = exec_stack.incCurrentTop(3);
  ep[-2].makeMarkEStack(PS_ES_OTHER, &XWPSContextState::endSuperExec);
  ep[-1].makeOper(0,  &XWPSContextState::endSuperExec);
  ep->assign(op);
  exec_stack.checkCache();
  pop(1);
  in_superexec++;
  return PS_O_PUSH_ESTACK;
}

int XWPSContextState::zzstop()
{
	XWPSRef * op = op_stack.getCurrentTop();
  uint count;

  int code = op->checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
  	
  count = countToStopped(op->value.intval);
  if (count) 
  {
		XWPSRef save_result;

		code = checkOp(op, 2);
		if (code < 0)
  		return code;
		save_result.assign(op - 1);
		pop(2);
		popEStack(count);
		op = op_stack.getCurrentTop();
		code = push(&op, 1);
		if (code < 0)
			return code;
		op->assign(&save_result);
		return PS_O_POP_ESTACK;
  }
  
  return unmatchedExit(op, &XWPSContextState::zzstop);
}

int XWPSContextState::zzstopped()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code = op->checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
  code = checkOp(op, 3);
  if (code < 0)
  	return code;
  
  code = checkEStack(5);
  if (code < 0)
  	return code;
  	
  XWPSRef * ep = exec_stack.incCurrentTop(1);
  ep->makeMarkEStack(PS_ES_STOPPED, &XWPSContextState::noCleanup);
  ep = exec_stack.incCurrentTop(1);
  ep->assign(op - 1);
  ep = exec_stack.incCurrentTop(1);
  ep->assign(op);
  ep = exec_stack.incCurrentTop(1);
  ep->makeOper(0, &XWPSContextState::stoppedPush);
  ep = exec_stack.incCurrentTop(1);
  ep->assign(op - 2);
  exec_stack.checkCache();
  pop(3);
  return PS_O_PUSH_ESTACK;
}
