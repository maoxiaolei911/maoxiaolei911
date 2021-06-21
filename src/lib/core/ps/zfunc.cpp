/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include "XWPSError.h"
#include "XWPSFunction.h"
#include "XWPSContextState.h"

#define MAX_SUB_FUNCTION_DEPTH 3

int XWPSContextState::buildFunction0(XWPSRef *op, 
	                   XWPSFunctionParams * mnDR,
		                 int , 
		                 XWPSFunction ** ppfn)
{
	XWPSFunctionSdParams params;
  XWPSRef *pDataSource;
  int code;

  params.m = mnDR->m;
  params.n = mnDR->n;
  params.Domain = mnDR->Domain;
  params.Range = mnDR->Range;
  mnDR->Domain = 0;
  mnDR->Range = 0;
  if ((code = op->dictFindString(this, "DataSource", &pDataSource)) <= 0)
		return (code < 0 ? code : (int)(XWPSError::RangeCheck));
  switch (pDataSource->type()) 
  {
		case XWPSRef::String:
				{
					params.DataSource.initString2(pDataSource->getBytes(), pDataSource->size());
					params.ds_bytes = pDataSource->value.bytes->arr;
					if (params.ds_bytes)
						params.ds_bytes->incRef();
				}
	    	break;
	    	
		case XWPSRef::File: 
			{
	    	XWPSStream *s;

	    	s = pDataSource->getStream();
	    	if (s->read_id != pDataSource->size())
	    	{
	    		if (s->read_id == 0 && s->write_id == pDataSource->size())
	    		{
	    			int fcode = fileSwitchToWrite(op);
	    			if (fcode < 0)
	    				return fcode;
	    		}
	    		else
	    			return (int)(XWPSError::InvalidFileAccess);
	    	}
	    	if (!(s->modes & PS_STREAM_MODE_SEEK))
					return (int)(XWPSError::IOError);
	    	s->incRef();
	    	params.DataSource.initStream(s);
	    	break;
			}
			
		default:
	    return (int)(XWPSError::RangeCheck);
  }
    
  if ((code = op->dictIntParam(this, "Order", 1, 3, 1, &params.Order)) < 0 ||
			(code = op->dictIntParam(this, "BitsPerSample", 1, 32, 0, &params.BitsPerSample)) < 0 ||
			((code = fnbuildFloatArray(op, "Encode", false, true, &params.Encode)) != 2 * params.m && 
			 (code != 0 || params.Encode != 0)) ||
	    ((code = fnbuildFloatArray(op, "Decode", false, true, &params.Decode)) != 2 * params.n && 
	    (code != 0 || params.Decode != 0))) 
	{
		goto fail;
  } 
  
  {
		int *ptr = 0;
		if (params.m > 0)
		  ptr = new int[params.m]; 
		params.Size = ptr;
		code = op->dictIntsParam(this, "Size", params.m, ptr);
		if (code != params.m)
	    goto fail;
  }
  
  code = params.check();
  if (code < 0)
  	return code;
  
  *ppfn = new XWPSFunctionSd(&params);
	return 0;
		
fail:
  return (code < 0 ? code : (int)(XWPSError::RangeCheck));
}

int XWPSContextState::buildFunction2(XWPSRef *op, 
	                   XWPSFunctionParams * mnDR,
		                 int , 
		                 XWPSFunction ** ppfn)
{
	XWPSFunctionElInParams params;
  int code, n0, n1;

  params.m = mnDR->m;
  params.n = mnDR->n;
  params.Domain = mnDR->Domain;
  params.Range = mnDR->Range;
  mnDR->Domain = 0;
  mnDR->Range = 0;
  if ((code = op->dictFloatParam(this, "N", 0.0, &params.N)) != 0 ||
			(code = n0 = fnbuildFloatArray(op, "C0", false, false, &params.C0)) < 0 ||
			(code = n1 = fnbuildFloatArray(op, "C1", false, false, &params.C1)) < 0)
		goto fail;
		
  if (params.C0 == 0)
		n0 = 1;
  if (params.C1 == 0)
		n1 = 1;	
  if (params.Range == 0)
		params.n = n0;		/* either one will do */
  if (n0 != n1 || n0 != params.n)
		goto fail;
    
  code = params.check();
  if (code < 0)
  	return code;
  *ppfn = new XWPSFunctionElIn(&params);
	return 0;
	
fail:
  return (code < 0 ? code : (int)(XWPSError::RangeCheck));
}

int XWPSContextState::buildFunction3(XWPSRef *op, 
	                   XWPSFunctionParams * mnDR,
		                 int depth, 
		                 XWPSFunction ** ppfn)
{
	XWPSFunction1ItSgParams params;
  int code;

  params.m = mnDR->m;
  params.n = mnDR->n;
  params.Domain = mnDR->Domain;
  params.Range = mnDR->Range;
  mnDR->Domain = 0;
  mnDR->Range = 0;
    
  {
		XWPSRef *pFunctions;
		XWPSFunction **ptr = 0;
		int i;

		if ((code = op->dictFindString(this, "Functions", &pFunctions)) <= 0)
	    return (code < 0 ? code : (int)(XWPSError::RangeCheck));
		code = pFunctions->checkArrayOnly();
		if (code < 0)
  		return code;
		params.k = pFunctions->size();
		ptr = (XWPSFunction**)malloc((params.k+1) * sizeof(XWPSFunction*));
		params.Functions = ptr;
		for (i = 0; i < params.k; ++i) 
		{
	    XWPSRef subfn;

			ptr[i] = 0;
	    pFunctions->arrayGet(this, (long)i, &subfn);
	    code = fnbuildSubFunction(&subfn, &ptr[i], depth);
		}
  }
    
  if ((code = fnbuildFloatArray(op, "Bounds", true, false, &params.Bounds)) != params.k - 1 ||
			(code = fnbuildFloatArray(op, "Encode", true, true, &params.Encode)) != 2 * params.k)
		goto fail;
    
  if (params.Range == 0)
		params.n = params.Functions[0]->params->n;
    
  code = params.check();
  if (code < 0)
  	return code;
  *ppfn = new XWPSFunction1ItSg(&params);
	return 0;
	
fail:
  return (code < 0 ? code : (int)(XWPSError::RangeCheck));
}

struct PSCalcOp 
{
    op_proc_t proc;
    PSPtCrOpCode opcode;
} calc_op_t;

static const PSCalcOp calc_ops[] = {


    {&XWPSContextState::zabs, PtCr_abs},
    {&XWPSContextState::zadd, PtCr_add},
    {&XWPSContextState::zand, PtCr_and},
    {&XWPSContextState::zatan, PtCr_atan},
    {&XWPSContextState::zbitShift, PtCr_bitshift},
    {&XWPSContextState::zceiling, PtCr_ceiling},
    {&XWPSContextState::zcos, PtCr_cos},
    {&XWPSContextState::zcvi, PtCr_cvi},
    {&XWPSContextState::zcvr, PtCr_cvr},
    {&XWPSContextState::zdiv, PtCr_div},
    {&XWPSContextState::zexp, PtCr_exp},
    {&XWPSContextState::zfloor, PtCr_floor},
    {&XWPSContextState::zidiv, PtCr_idiv},
    {&XWPSContextState::zln, PtCr_ln},
    {&XWPSContextState::zlog, PtCr_log},
    {&XWPSContextState::zmod, PtCr_mod},
    {&XWPSContextState::zmul, PtCr_mul},
    {&XWPSContextState::zneg, PtCr_neg},
    {&XWPSContextState::znot, PtCr_not},
    {&XWPSContextState::zor, PtCr_or},
    {&XWPSContextState::zround, PtCr_round},
    {&XWPSContextState::zsin, PtCr_sin},
    {&XWPSContextState::zsqrt, PtCr_sqrt},
    {&XWPSContextState::zsub, PtCr_sub},
    {&XWPSContextState::ztruncate, PtCr_truncate},
    {&XWPSContextState::zxor, PtCr_xor},

    {&XWPSContextState::zeq, PtCr_eq},
    {&XWPSContextState::zge, PtCr_ge},
    {&XWPSContextState::zgt, PtCr_gt},
    {&XWPSContextState::zle, PtCr_le},
    {&XWPSContextState::zlt, PtCr_lt},
    {&XWPSContextState::zne, PtCr_ne},

    {&XWPSContextState::zcopy, PtCr_copy},
    {&XWPSContextState::z2copy, PtCr_copy},
    {&XWPSContextState::zdup, PtCr_dup},
    {&XWPSContextState::zexch, PtCr_exch},
    {&XWPSContextState::zindex, PtCr_index},
    {&XWPSContextState::zpop, PtCr_pop},
    {&XWPSContextState::zroll, PtCr_roll}
};

static void
psc_fixup(uchar *p, uchar *to)
{
    int skip = to - (p + 3);

    p[1] = (uchar)(skip >> 8);
    p[2] = (uchar)skip;
}

#define MAX_PSC_FUNCTION_NESTING 10

int XWPSContextState::checkPSCFunction(XWPSRef *pref, int depth, uchar *ops, int *psize)
{
  long i;
  uint size = pref->size();

  for (i = 0; i < size; ++i) 
  {
		uchar no_ops[1 + 64];
		uchar *p = (ops ? ops + *psize : no_ops);
		XWPSRef elt, elt2, elt3;
		int code;

		pref->arrayGet(this, i, &elt);
		switch (elt.btype()) 
		{
			case XWPSRef::Integer: 
				{
	    		int i = elt.value.intval;

#if ARCH_SIZEOF_INT < ARCH_SIZEOF_LONG
	    		if (i != elt.value.intval) 
						return (int)(XWPSError::RangeCheck);
#endif
	    		if (i == (uchar)i) 
	    		{
						*p = PtCr_byte;
						p[1] = (uchar)i;
						*psize += 2;
	    		} 
	    		else 
	    		{
						*p = PtCr_int;
						memcpy(p + 1, &i, sizeof(i));
						*psize += 1 + sizeof(int);
	    		}
	    		break;
	    		
				}
				
			case XWPSRef::Real: 
				{
	    		float f = elt.value.realval;

	    		*p = PtCr_float;
	    		memcpy(p + 1, &f, sizeof(f));
	    		*psize += 1 + sizeof(float);
	    		break;
				}
				
			case XWPSRef::Boolean:
	    	*p = (elt.value.boolval ? PtCr_true : PtCr_false);
	    	++*psize;
	    	break;
	    	
			case XWPSRef::Name:
	    	if (!elt.hasAttr(PS_A_EXECUTABLE))
					return (int)(XWPSError::RangeCheck);
	    	nameStringRef(&elt, &elt);
	    	if (!bytes_compare(elt.getBytes(), elt.size(), (const uchar *)"true", 4))
					*p = PtCr_true;
	    	else if (!bytes_compare(elt.getBytes(), elt.size(), (const uchar *)"false", 5))
					*p = PtCr_false;
	    	else
					return (int)(XWPSError::RangeCheck);
	    	++*psize;
	    	break;
	    	
			case XWPSRef::Operator: 
				{
	    		int j;

	    		for (j = 0; j < (sizeof(calc_ops) / sizeof(calc_ops[0])); ++j)
						if (elt.getOper() == calc_ops[j].proc) 
						{
		    			*p = calc_ops[j].opcode;
		    			++*psize;
		    			goto next;
						}
	    		return (int)(XWPSError::RangeCheck);
				}
				
			default: 
				{
	    		if (!elt.isProc())
						return elt.checkTypeFailed();
	    		if (depth == MAX_PSC_FUNCTION_NESTING)
						return (int)(XWPSError::LimitCheck);
	    		if ((code = pref->arrayGet(this, ++i, &elt2)) < 0)
						return code;
	    		*psize += 3;
	    		code = checkPSCFunction(&elt, depth + 1, ops, psize);
	    		if (code < 0)
						return code;
						
#define R_IS_OPER(pref, proc)\
  (pref.btype() == XWPSRef::Operator && pref.hasAttr(PS_A_EXECUTABLE) &&\
   pref.getOper() == proc)
	    
	    		if (R_IS_OPER(elt2, &XWPSContextState::zif)) 
	    		{
						if (ops) 
						{
		    			*p = PtCr_if;
		    			psc_fixup(p, ops + *psize);
						}
	    		} 
	    		else if (!elt2.isProc())
						return (int)(XWPSError::RangeCheck);
	    		else if ((code = pref->arrayGet(this, ++i, &elt3)) < 0)
						return code;
	    		else if (R_IS_OPER(elt3, &XWPSContextState::zifelse)) 
	    		{
						if (ops) 
						{
		    			*p = PtCr_if;
		    			psc_fixup(p, ops + *psize + 3);
		    			p = ops + *psize;
		    			*p = PtCr_else;
						}
						*psize += 3;
						code = checkPSCFunction(&elt2, depth + 1, ops, psize);
						if (code < 0)
		    			return code;
						if (ops)
		    			psc_fixup(p, ops + *psize);
	    		} 
	    		else
						return (int)(XWPSError::RangeCheck);
#undef R_IS_OPER
				}
		}
    
next:
			;
  }
  return 0;
}
#undef MAX_PSC_FUNCTION_NESTING

int XWPSContextState::buildFunction4(XWPSRef *op, 
	                   XWPSFunctionParams * mnDR,
		                 int , 
		                 XWPSFunction ** ppfn)
{
	XWPSfunctionPtCrParams params;
  XWPSRef *proc;
  int code;
  uchar *ops;
  int size;

  params.m = mnDR->m;
  params.n = mnDR->n;
  params.Domain = mnDR->Domain;
  params.Range = mnDR->Range;
  mnDR->Domain = 0;
  mnDR->Range = 0;
  
  if (op->dictFindString(this, "Function", &proc) <= 0) 
  {
		code = (int)(XWPSError::RangeCheck);
		goto fail;
  }
  
  if (!proc->isProc()) 
  {
		code = (int)(XWPSError::RangeCheck);
		goto fail;
  }
  
  size = 0;
  code = checkPSCFunction(proc, 0, NULL, &size);
  if (code < 0)
		goto fail;
    
  ops = new uchar[size + 1];
  size = 0;
  checkPSCFunction(proc, 0, ops, &size);
  ops[size] = PtCr_return;
  params.ops.data = ops;
  params.ops.size = size + 1;
  code = params.check();
  if (code < 0)
  	return code;
  	
  *ppfn = new XWPSFunctionPtCr(&params);
	return 0;
	
fail:
  return (code < 0 ? code : (int)(XWPSError::RangeCheck));
}

int XWPSContextState::fnbuildFloatArray(XWPSRef * op, 
	                      const char *kstr, 
	                      bool required,
		                    bool even, 
		                    float **pparray)
{
	XWPSRef *par;
  int code;

  *pparray = 0;
  if (op->dictFindString(this, kstr, &par) <= 0)
		return (required ? (int)(XWPSError::RangeCheck) : 0);
			
  if (!par->isArray())
		return par->checkTypeFailed();
  
  {
		uint size = par->size();
		float *ptr = 0;
		if (size > 0)
		{
			ptr = new float[size];
			code = op->dictFloatArrayCheckParam(this, kstr, size, ptr, NULL, 0, XWPSError::RangeCheck);
			if (code < 0 || (even && (code & 1) != 0)) 
			{
	    	delete [] ptr;
	    	return(code < 0 ? code : (int)(XWPSError::RangeCheck));
			}
		}
		*pparray = ptr;
  }
  return code;
}

int XWPSContextState::fnbuildFunction(XWPSRef * op, XWPSFunction ** ppfn)
{
	return fnbuildSubFunction(op, ppfn, 0);
}

int XWPSContextState::fnbuildSubFunction(XWPSRef * op, 
	                       XWPSFunction ** ppfn, 
	                       int depth)
{
	int code, type, i;
  XWPSFunctionParams params;

  if (depth > MAX_SUB_FUNCTION_DEPTH)
		return (int)(XWPSError::LimitCheck);
    
  code = op->checkType(XWPSRef::Dictionary);
  if (code < 0)
  	return code;
  	
  code = op->dictIntParam(this, "FunctionType", 0, max_int, -1, &type);
  if (code < 0)
		return code;
    
  for (i = 0; i < 4; ++i)
		if (build_function_type_table[i].type == type)
	    break;
  if (i == 4)
		return (int)(XWPSError::RangeCheck);
	
  code = fnbuildFloatArray(op, "Domain", true, true, &params.Domain);
  if (code < 0)
		goto fail;
		
  params.m = code >> 1;
  code = fnbuildFloatArray(op, "Range", false, true, &params.Range);
  if (code < 0)
		goto fail;
		
  params.n = code >> 1;
  
  return (this->*build_function_type_table[i].build_function_proc_t)(op, &params, depth + 1, ppfn);
  
fail:
  return code;
}

XWPSFunction * XWPSContextState::refFunction(XWPSRef *op)
{
	if (op->hasType(XWPSRef::Array) && 
		  op->hasMaskedAttrs(PS_A_EXECUTABLE | PS_A_EXECUTE, PS_A_EXECUTABLE | PS_A_ALL) &&
			op->size() == 2)
	{
		XWPSRef * p = op->getArray();
		XWPSRef * pp = op->getArray() + 1;
		if (pp->hasTypeAttrs(XWPSRef::Operator, PS_A_EXECUTABLE) && 
			 	pp->getOper() == &XWPSContextState::zexecFunction &&
			 	p->isStruct() && 
			 	p->hasMaskedAttrs(PS_A_EXECUTABLE | PS_A_EXECUTE, PS_A_EXECUTABLE | PS_A_ALL))
		{
			return (XWPSFunction *)(p->getStruct());
		}
	}
	
  return 0;
}

int XWPSContextState::zbuildFunction()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSFunction *pfn;
  XWPSRef cref;	
  int code;
  
  cref.makeArray(currentSpace() | PS_A_EXECUTABLE | PS_A_EXECUTE, 2);
  code = fnbuildFunction(op, &pfn);
  if (code < 0) 
		return code;
		
	XWPSRef * p = cref.getArray();
	p->makeStruct(PS_A_EXECUTABLE | PS_A_EXECUTE | currentSpace(), pfn);
	p++;
	p->makeOper(PS_A_EXECUTABLE, 0, &XWPSContextState::zexecFunction);
  op->assign(&cref);
  return 0;
}

int XWPSContextState::zexecFunction()
{
	XWPSRef * op = op_stack.getCurrentTop();
	if (!op->isStruct() ||
			!op->hasMaskedAttrs(PS_A_EXECUTABLE | PS_A_EXECUTE, PS_A_EXECUTABLE | PS_A_ALL))
		return op->checkTypeFailed();
  
  {
		XWPSFunction *pfn = (XWPSFunction *)(op->getStruct());
		int m = pfn->params->m, n = pfn->params->n;
		int diff = n - (m + 1);

		if (diff > 0)
	    checkOStack(diff);
		{
	    float params[20];	
	    float *in;
	    float *out;
	    int code = 0;

	    if (m + n <= 20) 
				in = params;
	    else 
				in = new float[m + n];
	    out = in + m;
	    if (code < 0 ||	(code = floatParams(op - 1, m, in)) < 0 ||	(code = pfn->evaluate(in, out)) < 0	)
				;
	    else 
	    {
				if (diff > 0)
				{
		    	code = push(&op, diff);
		    	if (code < 0)
						return code;
		    }
				else if (diff < 0) 
				{
		    	pop(-diff);
		    	op = op_stack.getCurrentTop();
				}
				
				float * pval = out;
				int count = n;
				XWPSRef * p = op + 1 - n;
				for (; count--; p++, pval++)
					p->makeReal(*pval);
	    }
	    if (in != params)
				delete [] in;
	    return code;
		}
  }
}
