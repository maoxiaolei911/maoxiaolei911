/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWPSError.h"
#include "XWPSContextState.h"

int XWPSContextState::tokenContinue(XWPSStream * s, XWPSScannerState * pstate, bool save)
{
	XWPSRef * op = op_stack.getCurrentTop();
	XWPSRef fref;
	fref.assign(op);
	int code = 0;
	XWPSRef token;
	
again:
	pop(1);
	code = scanToken(s, &token, pstate);
  op = op_stack.getCurrentTop();
  switch (code)
  {
  	default:
	    if (code > 0)
				code = (int)(XWPSError::SyntaxError);
	    push(&op, 1);
	    op->assign(&fref);
	    break;
	    
	  case scan_BOS:
	    code = 0;
		case 0:
	    code = push(&op, 2);
	    if (code < 0)
	    	return code;
	    op[-1].assign(&token);
	    op->makeTrue();
	    break;
	    
		case scan_EOF:
	    code = push(&op, 1);
	    if (code < 0)
	    	return code;
	    op->makeFalse();
	    code = 0;
	    break;
	    
	  case scan_Refill:
	  	code = push(&op, 1);
	    if (code < 0)
	    	return code;
	    op->assign(&fref);
	    code = scanHandleRefill(op, pstate, save, false, &XWPSContextState::ztokenContinue);
	    switch (code)
	    {
	    	case 0:
		    	goto again;
				case PS_O_PUSH_ESTACK:
		    	return code;
	    }
  }
  
  return code;
}

int XWPSContextState::tokenExecContinue(XWPSStream * s, XWPSScannerState * pstate, bool save)
{
	XWPSRef * op = op_stack.getCurrentTop();
	int code;
	XWPSRef fref;
	XWPSRef * p;
	
	fref.assign(op);
	pop(1);
	
again:
	code = checkEStack(1);
	if (code < 0)
		return code;
		
  code = scanToken(s, (exec_stack.getCurrentTop() + 1), pstate);
  op = op_stack.getCurrentTop();
  switch (code)
  {
  	case 0:
  		p = exec_stack.getCurrentTop() + 1;
	    if (p->isProc()) 
	    {
				code = push(&op, 1);
				if (code < 0)
					return code;
					
				op->assign(exec_stack.getCurrentTop() + 1);
				code = 0;
				break;
	    }
	    
		case scan_BOS:
	    exec_stack.incCurrentTop(1);
	    code = PS_O_PUSH_ESTACK;
	    break;
	    
		case scan_EOF:
	    code = 0;
	    break;
	    
	  case scan_Refill:
	    code = scanHandleRefill(&fref, pstate, save, true, &XWPSContextState::ztokenExecContinue);
	    switch (code) 
	    {
				case 0:
		    	goto again;
				case PS_O_PUSH_ESTACK:
		    	return code;
	    }
	    break;
	    
	  case scan_Comment:
		case scan_DSC_Comment:
	    return ztokenHandleComment(&fref, pstate, exec_stack.getCurrentTop() + 1, code, save, true, &XWPSContextState::ztokenExecContinue);
	    
		default:
	    break;
  }
  
  if (code < 0) 
  {
		push(&op, 1);
		op->assign(&fref);
  }
    
  return code;
}

int XWPSContextState::ztoken()
{
	XWPSRef * op = op_stack.getCurrentTop();
	int ccode;
  switch (op->type())
  {
  	default:
  		return (int)(XWPSError::TypeCheck);
  			
  	case XWPSRef::File:
  		{
  			XWPSStream *s;
  			ccode = op->checkReadType(XWPSRef::File);
  			if (ccode < 0)
  				return ccode;
  			s = op->getStream();
  			if (s->read_id != op->size())
  			{
  				if (s->read_id == 0 && s->write_id == op->size())
  				{
  					ccode = fileSwitchToRead(op);
  					if (ccode < 0)
  						return ccode;
  				}
  				else
  					s = &invalid_file_stream;
  			}
  
  			ccode = checkOStack(1);
  			if (ccode < 0)
  				return ccode;
	    	XWPSScannerState state;
	    	state.init(false);
	    	int code = tokenContinue(s, &state, true);
	    	state.s_da.base = 0;
	    	return code;
  		}
  		
  	case XWPSRef::String:
  		{
  			XWPSRef token;
	    	int code = scanStringToken(op, &token);
	    	switch (code) 
	    	{
	    		case scan_EOF:
						op->makeFalse();
						return 0;
						
	    		default:
						if (code < 0)
		    			return code;
	    	}
	    	ccode = push(&op,2);
	    	if (ccode < 0)
					return ccode;
	    	op[-1].assign(&token);
	    	op->makeTrue();
	    	return 0;
  		}
  }
}

int XWPSContextState::ztokenContinue()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSStream *s;
  int code = op[-1].checkReadType(XWPSRef::File);
  if (code < 0)
  	return code;
  	
  code = op->checkType("scannerstate");
	if (code < 0)
		return code;
  	
  s = op[-1].getStream();
  if (s->read_id != op[-1].size())
  {
  	if (s->read_id == 0 && s->write_id == op[-1].size())
  	{
  		code = fileSwitchToRead(op - 1);
  		if (code < 0)
  			return code;
  	}
  	else
  		s = &invalid_file_stream;
  }
  XWPSScannerState *pstate = (XWPSScannerState*)(op->getStruct());
  pop(1);
  return tokenContinue(s, pstate, false);
}

int XWPSContextState::ztokenExec()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSStream *s;
  int code = op->checkReadType(XWPSRef::File);
  if (code < 0)
  	return code;
  	
  s = op->getStream();
  if (s->read_id != op->size())
  {
  	if (s->read_id == 0 && s->write_id == op->size())
  	{
  		code = fileSwitchToRead(op);
  		if (code < 0)
  			return code;
  	}
  	else
  		s = &invalid_file_stream;
  }
  	
  code = checkEStack(1);
  if (code < 0)
  	return code;
  	
  XWPSScannerState state;  
  state.init(false);
  code = tokenExecContinue(s, &state, true);
  state.s_da.base = 0;
  return code;
}

int XWPSContextState::ztokenExecContinue()
{
	XWPSRef * op = op_stack.getCurrentTop();
	XWPSStream *s;
	int code = op[-1].checkReadType(XWPSRef::File);
  if (code < 0)
  	return code;
  	
  code = op->checkType("scannerstate");
	if (code < 0)
		return code;
		
  s = op[-1].getStream();
  if (s->read_id != op[-1].size())
  {
  	if (s->read_id == 0 && s->write_id == op[-1].size())
  	{
  		code = fileSwitchToRead(op - 1);
  		if (code < 0)
  			return code;
  	}
  	else
  		s = &invalid_file_stream;
  }
  
  XWPSScannerState *pstate = (XWPSScannerState*)(op->getStruct());
  pop(1);
  return tokenExecContinue(s, pstate, false);
}

int XWPSContextState::ztokenHandleComment(XWPSRef *fop, 
	                                       XWPSScannerState *sstate,
		                                     XWPSRef *ptoken, 
		                                     int scan_code,
		                                     bool save, 
		                                     bool , 
		                                     op_proc_t cont)
{
	const char *proc_name;
  XWPSScannerState *pstate;
  XWPSRef * op;
  XWPSRef * p;
  int code;
  
  switch (scan_code)
  {
  	case scan_Comment:
			proc_name = "%ProcessComment";
			break;
			
    case scan_DSC_Comment:
			proc_name = "%ProcessDSCComment";
			break;
			
    default:
			return (int)(XWPSError::Fatal);
  }
  
  if (op_stack.getTop() - op_stack.getCurrentTop() < 2) 
  {
		code = op_stack.extend(2);
		if (code < 0)
	    return code;
  }
  
  code = checkEStack(4);
  if (code < 0)
  	return code;
  	
  code = nameEnterString(proc_name, exec_stack.getCurrentTop() + 4);
  if (code < 0)
		return code;
		
	if (save)
	{
		pstate = new XWPSScannerState;
		*pstate = *sstate;
	}
	else
	{
		pstate = sstate;
		pstate->incRef();
	}
	
	if (!pstate->s_pstack)
	{
		p = op_stack.getCurrentTop() + 2;
		p->assign(ptoken);
	}
	
	p = exec_stack.getCurrentTop() + 1;
	p->makeOper(0, cont);
	p = exec_stack.getCurrentTop() + 2;
	p->makeStruct(currentSpace() | 0, pstate);
	p = exec_stack.getCurrentTop() + 3;
	p->assign(fop);
	p->clearAttrs(PS_A_EXECUTABLE);
	p = exec_stack.getCurrentTop() + 4;
	XWPSRef * ppcproc = dictFindName(p);
	if (ppcproc == 0)
	{
		if (pstate->s_pstack)
	    op_stack.incCurrentTop(-1);
		exec_stack.incCurrentTop(3);
	}
	else
	{
		if (pstate->s_pstack) 
		{
	    op = op_stack.incCurrentTop(1);
	    p = op - 1;
	    op->assign(p);
		} 
		else 
	  	op = op_stack.incCurrentTop(2);
	  	
	  p = op - 1;
	 	p->assign(fop);
	  p = exec_stack.getCurrentTop() + 4;
		p->assign(ppcproc);
		exec_stack.incCurrentTop(4);
	}
	
	return PS_O_PUSH_ESTACK;
}

int XWPSContextState::ztokenScannerOptions(XWPSRef *upref, int old_options)
{
	typedef struct named_scanner_option_s 
	{
		const char *pname;
		int option;
  } named_scanner_option_t;
  
  static const named_scanner_option_t named_options[2] = 
  {
		{"ProcessComment", SCAN_PROCESS_COMMENTS},
		{"ProcessDSCComment", SCAN_PROCESS_DSC_COMMENTS}
  };
    
  int options = old_options;
  for (int i = 0; i < sizeof(named_options) / sizeof(named_options[0]); ++i) 
  {
		const named_scanner_option_t *pnso = &named_options[i];
		XWPSRef *ppcproc;
	  int code = upref->dictFindString(this, pnso->pname, &ppcproc);
		if (code >= 0) 
		{
	    if (ppcproc->hasType(XWPSRef::Null))
				options &= ~pnso->option;
	    else
				options |= pnso->option;
		}
  }
  return options;
}
