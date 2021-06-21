/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <string.h>
#include <math.h>
#include "XWPSError.h"
#include "XWPSStream.h"
#include "XWPSParam.h"
#include "XWPSState.h"
#include "XWPSFunction.h"

#define max_Sd_m 16
#define max_Sd_n 16
#define MAX_VSTACK 100

XWPSFunctionParams::~XWPSFunctionParams()
{
	if (Domain)
	{
		delete [] Domain;
		Domain = 0;
	}
	
	if (Range)
	{
		delete [] Range;
		Range = 0;
	}
}

int XWPSFunctionParams::checkmnDR(int mA, int nA)
{
	int i;

  if (mA <= 0 || nA <= 0)
		return (int)(XWPSError::RangeCheck);
  for (i = 0; i < mA; ++i)
		if (Domain[2 * i] > Domain[2 * i + 1])
	    return (int)(XWPSError::RangeCheck);
  if (Range != 0)
		for (i = 0; i < nA; ++i)
	  	if (Range[2 * i] > Range[2 * i + 1])
			return (int)(XWPSError::RangeCheck);
  return 0;
}

XWPSFunctionSdParams::XWPSFunctionSdParams()
	:XWPSFunctionParams()
{
	Order = 0;
	BitsPerSample = 0;
	Encode = 0;
	Decode = 0;
	Size = 0;
	ds_bytes = 0;
}

XWPSFunctionSdParams::~XWPSFunctionSdParams()
{
	if (Encode)
	{
		delete [] Encode;
		Encode = 0;
	}
	
	if (Decode)
	{
		delete [] Decode;
		Decode = 0;
	}
	
	if (Size)
	{
		delete [] Size;
		Size = 0;
	}
	
	if (ds_bytes)
	{
		if (ds_bytes->decRef() == 0)
			delete ds_bytes;
		ds_bytes = 0;
	}
}

int XWPSFunctionSdParams::check()
{
	int code;
  int i;
  code = checkmnDR(m, n);
  if (code < 0)
		return code;
		
	if (m > max_Sd_m)
		return (int)(XWPSError::LimitCheck);
  
  switch (Order) 
  {
		case 0:	
		case 1:
		case 3:
	    break;
	    
		default:
	    return (int)(XWPSError::RangeCheck);
  }
  
  switch (BitsPerSample) 
  {
		case 1:
		case 2:
		case 4:
		case 8:
		case 12:
		case 16:
		case 24:
		case 32:
	    break;
	    
		default:
	    return (int)(XWPSError::RangeCheck);
  }
  
  for (i = 0; i < m; ++i)
		if (Size[i] <= 0)
	    return (int)(XWPSError::RangeCheck);
	  
	if (Order == 0)
	    Order = 1;
	  
	return 0;
}

XWPSFunctionElInParams::XWPSFunctionElInParams()
	:XWPSFunctionParams()
{
	C0 = 0;
	C1 = 0;
	N = 0;
}

XWPSFunctionElInParams::~XWPSFunctionElInParams()
{
	if (C0)
	{
		delete [] C0;
		C0 = 0;
	}
	
	if (C1)
	{
		delete [] C1;
		C1 = 0;
	}
}

int XWPSFunctionElInParams::check()
{
	int code = checkmnDR(1, n);
  if (code < 0)
		return code;
    
  if ((C0 == 0 || C1 == 0) && n != 1)
	  return (int)(XWPSError::RangeCheck);
    
  if (N != floor(N)) 
  {
		if (Domain[0] < 0)
	  	return (int)(XWPSError::RangeCheck);
  }
  
  if (N < 0) 
  {
		if (Domain[0] <= 0 && Domain[1] >= 0)
	  	return (int)(XWPSError::RangeCheck);
  }
  
  return 0;
}

XWPSFunction1ItSgParams::XWPSFunction1ItSgParams()
	:XWPSFunctionParams()
{
	k = 0;
	Functions = 0;
	Bounds = 0;
	Encode = 0;
}

XWPSFunction1ItSgParams::~XWPSFunction1ItSgParams()
{
	if (Functions)
	{
		for (int i = 0; i < k; i++)
		{
			if (Functions[i])
			{
				if (!Functions[i]->decRef())
					delete Functions[i];
				Functions[i] = 0;
			}
		}
		
		free(Functions);
		Functions = 0;
	}
	
	if (Bounds)
	{
		delete [] Bounds;
		Bounds = 0;
	}
	
	if (Encode)
	{
		delete [] Encode;
		Encode = 0;
	}
}

int XWPSFunction1ItSgParams::check()
{
	int nA = (Range == 0 ? 0 : n);
  float prev = Domain[0];
  int i;

  for (i = 0; i < k; ++i) 
  {
		XWPSFunction *psubfn = Functions[i];

		if (psubfn->params->m != 1)
	    return (int)(XWPSError::RangeCheck);
	  
		if (nA == 0)
	    nA = psubfn->params->n;
		else if (psubfn->params->n != nA)
	    return (int)(XWPSError::RangeCheck);
	  
		if (i < k - 1) 
		{
	    if (Bounds[i] <= prev)
	    	return (int)(XWPSError::RangeCheck);
			
	    prev = Bounds[i];
		}
  }
  
  if (Domain[1] < prev)
	  return (int)(XWPSError::RangeCheck);
    
  return checkmnDR(1, nA);
}

XWPSFunctionAdOtParams::XWPSFunctionAdOtParams()
	:XWPSFunctionParams()
{
	Functions = 0;
}

XWPSFunctionAdOtParams::~XWPSFunctionAdOtParams()
{
	if (Functions)
	{
		for (int i = 0; i < n; i++)
		{
			if (Functions[i])
			{
				if (!Functions[i]->decRef())
					delete Functions[i];
				Functions[i] = 0;
			}
		}
		
		free(Functions);
		Functions = 0;
	}
}

int XWPSFunctionAdOtParams::check()
{
	int mA = m, nA = n;
  int i;
  
  if (mA <= 0 || nA <= 0)
	  return (int)(XWPSError::RangeCheck);
    
  for (i = 0; i < nA; ++i) 
  {
		XWPSFunction *psubfn = Functions[i];
		if (psubfn->params->m != m || psubfn->params->n != 1)
	  	return (int)(XWPSError::RangeCheck);
  }
  
  return 0;
}

static void
store_float(PSCalcValue *vsp, float f)
{
    vsp->value.f = f;
    vsp->type = CVT_FLOAT;
}

static int
calc_put_ops(XWPSStream *s, const uchar *ops, uint size)
{
    const uchar *p;

    s->pputc('{');
    for (p = ops; p < ops + size; )
			switch (*p++) 
			{
				case PtCr_byte:
	    		s->print("%d ", *p++);
	    		break;
	    		
				case PtCr_int: 
					{
	    			int i;

	    			memcpy(&i, p, sizeof(int));
	    			s->print("%d ", i);
	    			p += sizeof(int);
	    			break;
					}
					
				case PtCr_float: 
					{
	    			float f;

	    			memcpy(&f, p, sizeof(float));
	    			s->print("%g ", f);
	    			p += sizeof(float);
	    			break;
	    			
					}
					
				case PtCr_true:
	    		s->pputs("true ");
	    		break;
	    		
				case PtCr_false:
	    		s->pputs("false ");
	    		break;
	    		
				case PtCr_if: 
					{
	    			int skip = (p[0] << 8) + p[1];
	    			int code;

	    			code = calc_put_ops(s, p += 2, skip);
	    			p += skip;
	    			if (code < 0)
							return code;
	    			if (code > 0) 
	    			{	
							skip = (p[-2] << 8) + p[-1];
							code = calc_put_ops(s, p, skip);
							p += skip;
							if (code < 0)
		    				return code;
							s->pputs(" ifelse ");
	    			} 
	    			else
							s->pputs(" if ");
					}
					
				case PtCr_else:
	    		if (p != ops + size - 2)
	    		{
						return (int)(XWPSError::RangeCheck);
					}
	    		return 1;
	    		
				default: 
					{
						static const char *const op_names[] = {
		    			"abs", "add", "and", "atan", "bitshift",
		    			"ceiling", "cos", "cvi", "cvr", "div", "exp",
		    			"floor", "idiv", "ln", "log", "mod", "mul",
		    			"neg", "not", "or", "round", "sin", "sqrt", "sub",
		    			"truncate", "xor", "eq", "ge", "gt", "le", "lt", "ne",
		    			"copy", "dup", "exch", "index", "pop", "roll"};

						s->print("%s ", op_names[p[-1]]);
	    		}
			}
   s->pputc('}');
   return 0;
}

XWPSfunctionPtCrParams::XWPSfunctionPtCrParams()
	:XWPSFunctionParams()
{
}

XWPSfunctionPtCrParams::~XWPSfunctionPtCrParams()
{
	if (ops.data)
	{
		delete [] ops.data;
		ops.data = 0;
	}
}

int XWPSfunctionPtCrParams::check()
{
	int code;
	code = checkmnDR(m, n);
  if (code < 0)
		return code;
    
  if (m > MAX_VSTACK || n > MAX_VSTACK)
		return (int)(XWPSError::LimitCheck);
			
	{
		uchar *p = ops.data;

		for (; *p != PtCr_return; ++p)
	    switch ((PSPtCrOpCode)*p) 
	    {
	    	case PtCr_byte:
					++p; 
					break;
					
	    	case PtCr_int:
					p += sizeof(int); 
					break;
					
	    	case PtCr_float:
					p += sizeof(float); 
					break;
					
	    	case PtCr_if:
	    	case PtCr_else:
					p += 2;
	    	case PtCr_true:
	    	case PtCr_false:
					break;
					
	    	default:
					if (*p >= PtCr_NUM_OPS)
						return (int)(XWPSError::RangeCheck);
	    }
			if (p != ops.data + ops.size - 1)
				return (int)(XWPSError::RangeCheck);
  }
  
  return 0;
}

XWPSFunctionVaParams::XWPSFunctionVaParams()
	:XWPSFunctionParams()
{
	is_monotonic = 0;
}

XWPSFunctionVaParams::~XWPSFunctionVaParams()
{
}

int XWPSFunctionVaParams::check()
{
	return checkmnDR(1, n);
}

XWPSFunctionInfo::XWPSFunctionInfo()
{
	DataSource = 0;
	data_size = 0;
	Functions = 0;
	num_Functions = 0;
}

XWPSFunctionInfo::~XWPSFunctionInfo()
{
}

XWPSFunction::XWPSFunction()
	:XWPSStruct()
{
	params = 0;
}

XWPSFunction::~XWPSFunction()
{
}

int XWPSFunction::domainIsMonotonic(PSFunctionEffort effort)
{
#define MAX_M 16		/* arbitrary */
  float lower[MAX_M], upper[MAX_M];
  int i;

  if (params->m > MAX_M)
		return (int)(XWPSError::Undefined);
  for (i = 0; i < params->m; ++i) 
  {
		lower[i] = params->Domain[2 * i];
		upper[i] = params->Domain[2 * i + 1];
  }
  return isMonotonic(lower, upper, effort);
}

int XWPSFunction::evaluate(const float *in, float *out)
{
	return (this->*(head.procs.evaluate_.evaluate))(in, out);
}

int XWPSFunction::getLength()
{
	return sizeof(XWPSFunction);
}

const char * XWPSFunction::getTypeName()
{
	return "function";
}

void XWPSFunction::getInfo(XWPSFunctionInfo *pfi)
{
	(this->*(head.procs.get_info_.get_info))(pfi);
}

void XWPSFunction::getInfoDefault(XWPSFunctionInfo *pfi)
{
	pfi->DataSource = 0;
  pfi->Functions = 0;
}

int  XWPSFunction::getParams(XWPSContextState * ctx, XWPSParamList *plist)
{
	return (this->*(head.procs.get_params_.get_params))(ctx, plist);
}

int  XWPSFunction::getParamsCommon(XWPSContextState * ctx, XWPSParamList *plist)
{
	int ecode = plist->writeInt(ctx, "FunctionType", &(head.type));
  int code;

  if (params->Domain) 
  {
		code = plist->writeFloatValues(ctx, "Domain", params->Domain,	2 * params->m, true);
		if (code < 0)
	    ecode = code;
  }
  if (params->Range) 
  {
		code = plist->writeFloatValues(ctx, "Range", params->Range,2 * params->n, true);
		if (code < 0)
	    ecode = code;
  }
  return ecode;
}

int XWPSFunction::isMonotonic(const float *lower, const float *upper, PSFunctionEffort effort)
{
	return (this->*(head.procs.is_monotonic_.is_monotonic))(lower, upper, effort);
}

int (XWPSFunctionSd::*fn_get_samples[])(ulong offset, uint * samples) =
{
  0, &XWPSFunctionSd::gets1, &XWPSFunctionSd::gets2, 0, &XWPSFunctionSd::gets4, 0, 0, 0,
	&XWPSFunctionSd::gets8, 0, 0, 0, &XWPSFunctionSd::gets12, 0, 0, 0,
	&XWPSFunctionSd::gets16, 0, 0, 0, 0, 0, 0, 0,
	&XWPSFunctionSd::gets24, 0, 0, 0, 0, 0, 0, 0,
	&XWPSFunctionSd::gets32
};

XWPSFunctionSd::XWPSFunctionSd(XWPSFunctionSdParams * paramsA)
	:XWPSFunction()
{
	head.type = function_type_Sampled;
	head.procs.evaluate_.evaluatesd = &XWPSFunctionSd::evaluateSd;
	head.procs.is_monotonic_.is_monotonicsd = &XWPSFunctionSd::isMonotonicSd;
	head.procs.get_info_.get_infosd = &XWPSFunctionSd::getInfoSd;
	head.procs.get_params_.get_paramssd = &XWPSFunctionSd::getParamsSd;
	
	XWPSFunctionSdParams * paramsB = new XWPSFunctionSdParams;
	*paramsB = *paramsA;
	paramsA->Domain = 0;
	paramsA->Range = 0;
	paramsA->Encode = 0;
	paramsA->Decode = 0;
	paramsA->Size = 0;
	paramsA->ds_bytes = 0;
	params = paramsB;
	head.is_monotonic = domainIsMonotonic(EFFORT_MODERATE);
}

XWPSFunctionSd::~XWPSFunctionSd()
{
	if (params)
	{
		XWPSFunctionSdParams * paramssd = (XWPSFunctionSdParams*)params;
		if (paramssd->DataSource.isStream())
		{
			if (paramssd->DataSource.data.strm)
				if (!(paramssd->DataSource.data.strm->decRef()))
					delete paramssd->DataSource.data.strm;
								
			paramssd->DataSource.data.strm = 0;
		}
		else if (paramssd->DataSource.isArray())
		{
			float * d = (float*)(paramssd->DataSource.data.str.data);
			delete [] d;
		}
				
		delete params;
		params = 0;
	}
}

int XWPSFunctionSd::evaluateSd(const float *in, float *out)
{
	XWPSFunctionSdParams * paramssd = (XWPSFunctionSdParams*)params;
	int bps = paramssd->BitsPerSample;
  ulong offset = 0;
  int i;
  float encoded[max_Sd_m];
  ulong factors[max_Sd_m];
  float samples[max_Sd_n];
  
  for (i = 0; i < paramssd->m; ++i) 
  {
		float d0 = paramssd->Domain[2 * i], d1 = paramssd->Domain[2 * i + 1];
		float arg = in[i], enc;

		if (arg < d0)
	    arg = d0;
		else if (arg > d1)
	    arg = d1;
		if (paramssd->Encode) 
		{
	    float e0 = paramssd->Encode[2 * i];
	    float e1 = paramssd->Encode[2 * i + 1];

	    enc = (arg - d0) * (e1 - e0) / (d1 - d0) + e0;
	    if (enc < 0)
				encoded[i] = 0;
	    else if (enc >= paramssd->Size[i] - 1)
				encoded[i] = paramssd->Size[i] - 1;
	    else
				encoded[i] = enc;
		} 
		else 
	    encoded[i] = (arg - d0) * (paramssd->Size[i] - 1) / (d1 - d0);
  }
  
  {
		ulong factor = bps * paramssd->n;

		for (i = 0; i < paramssd->m; factor *= paramssd->Size[i++]) 
		{
	    int ipart = (int)encoded[i];

	    offset += (factors[i] = factor) * ipart;
	    encoded[i] -= ipart;
		}
  }
  
  interpolateLinear(encoded, factors, samples, offset, paramssd->m);
  for (i = 0; i < paramssd->n; ++i) 
  {
		float d0, d1, r0, r1, value;

		if (paramssd->Range)
	    r0 = paramssd->Range[2 * i], r1 = paramssd->Range[2 * i + 1];
		else
	    r0 = 0, r1 = (1 << bps) - 1;
		if (paramssd->Decode)
	    d0 = paramssd->Decode[2 * i], d1 = paramssd->Decode[2 * i + 1];
		else
	    d0 = r0, d1 = r1;

		value = samples[i] * (d1 - d0) / ((1 << bps) - 1) + d0;
		if (value < r0)
	    out[i] = r0;
		else if (value > r1)
	    out[i] = r1;
		else
	    out[i] = value;
  }

  return 0;
}

int XWPSFunctionSd::getLength()
{
	return sizeof(XWPSFunctionSd);
}

const char * XWPSFunctionSd::getTypeName()
{
	return "functionsd";
}

void XWPSFunctionSd::getInfoSd(XWPSFunctionInfo *pfi)
{
	XWPSFunctionSdParams * paramssd = (XWPSFunctionSdParams*)params;
	int i; long size;

  getInfoDefault(pfi);
  pfi->DataSource = &paramssd->DataSource;
  for (i = 0, size = 1; i < paramssd->m; ++i)
		size *= paramssd->Size[i];
  pfi->data_size = (size * paramssd->n * paramssd->BitsPerSample + 7) >> 3;
}

int  XWPSFunctionSd::getParamsSd(XWPSContextState * ctx, XWPSParamList *plist)
{
	XWPSFunctionSdParams * paramssd = (XWPSFunctionSdParams*)params;
	int ecode = getParamsCommon(ctx, plist);
  int code;

  if (paramssd->Order != 1) 
  {
		if ((code = plist->writeInt(ctx, "Order", &paramssd->Order)) < 0)
	    ecode = code;
  }
  if ((code = plist->writeInt(ctx, "BitsPerSample",	&paramssd->BitsPerSample)) < 0)
		ecode = code;
  if (paramssd->Encode) 
  {
		if ((code = plist->writeFloatValues(ctx, "Encode", paramssd->Encode, 2 * paramssd->m, true)) < 0)
	    ecode = code;
  }
  if (paramssd->Decode) 
  {
		if ((code = plist->writeFloatValues(ctx, "Decode", paramssd->Decode, 2 * paramssd->n, true)) < 0)
	    ecode = code;
  }
  if (paramssd->Size) 
  {
		if ((code = plist->writeIntValues(ctx, "Size", paramssd->Size, paramssd->m, true)) < 0)
	    ecode = code;
  }
  return ecode;
}

int XWPSFunctionSd::gets1(ulong offset, uint * samples)
{
	XWPSFunctionSdParams * paramssd = (XWPSFunctionSdParams*)params;
	int n = paramssd->n;
	uchar buf[max_Sd_n * ((1 + 7) >> 3)];
	uchar *p;
	int i;
	int code = paramssd->DataSource.accessOnly(offset >> 3, ((offset & 7) + n + 7) >> 3, buf, &p);
	if (code < 0)
		return code;
		
	for (i = 0; i < n; ++i) 
	{
		samples[i] = (*p >> (~offset & 7)) & 1;
		if (!(++offset & 7))
	    p++;
  }
  return 0;
}

int XWPSFunctionSd::gets2(ulong offset, uint * samples)
{
	XWPSFunctionSdParams * paramssd = (XWPSFunctionSdParams*)params;
	int n = paramssd->n;
	uchar buf[max_Sd_n * ((2 + 7) >> 3)];
	uchar *p;
	int i;
	int code = paramssd->DataSource.accessOnly(offset >> 3, (((offset & 7) >> 1) + n + 3) >> 2, buf, &p);
	if (code < 0)
		return code;
		
	for (i = 0; i < n; ++i) 
	{
		samples[i] = (*p >> (6 - (offset & 7))) & 3;
		if (!((offset += 2) & 7))
	    p++;
  }
  return 0;
}

int XWPSFunctionSd::gets4(ulong offset, uint * samples)
{
	XWPSFunctionSdParams * paramssd = (XWPSFunctionSdParams*)params;
	int n = paramssd->n;
	uchar buf[max_Sd_n * ((4 + 7) >> 3)];
	uchar *p;
	int i;
	int code = paramssd->DataSource.accessOnly(offset >> 3, (((offset & 7) >> 2) + n + 1) >> 1, buf, &p);
	if (code < 0)
		return code;
		
	for (i = 0; i < n; ++i) 
		samples[i] = ((offset ^= 4) & 4 ? *p >> 4 : *p++ & 0xf);
  return 0;
}

int XWPSFunctionSd::gets8(ulong offset, uint * samples)
{
	XWPSFunctionSdParams * paramssd = (XWPSFunctionSdParams*)params;
	int n = paramssd->n;
	uchar buf[max_Sd_n * ((8 + 7) >> 3)];
	uchar *p;
	int i;
	int code = paramssd->DataSource.accessOnly(offset >> 3, n, buf, &p);
	if (code < 0)
		return code;
		
	for (i = 0; i < n; ++i) 
		samples[i] = *p++;
  return 0;
}

int XWPSFunctionSd::gets12(ulong offset, uint * samples)
{
	XWPSFunctionSdParams * paramssd = (XWPSFunctionSdParams*)params;
	int n = paramssd->n;
	uchar buf[max_Sd_n * ((12 + 7) >> 3)];
	uchar *p;
	int i;
	int code = paramssd->DataSource.accessOnly(offset >> 3, (((offset & 7) >> 2) + 3 * n + 1) >> 1, buf, &p);
	if (code < 0)
		return code;
		
	for (i = 0; i < n; ++i) 
	{
		if (offset & 4)
	    samples[i] = ((*p & 0xf) << 8) + p[1], p += 2;
		else
	    samples[i] = (*p << 4) + (p[1] >> 4), p++;
		offset ^= 4;
  }
  return 0;
}

int XWPSFunctionSd::gets16(ulong offset, uint * samples)
{
	XWPSFunctionSdParams * paramssd = (XWPSFunctionSdParams*)params;
	int n = paramssd->n;
	uchar buf[max_Sd_n * ((16 + 7) >> 3)];
	uchar *p;
	int i;
	int code = paramssd->DataSource.accessOnly(offset >> 3, n * 2, buf, &p);
	if (code < 0)
		return code;
		
	for (i = 0; i < n; ++i) 
	{
		samples[i] = (*p << 8) + p[1];
		p += 2;
  }
  return 0;
}

int XWPSFunctionSd::gets24(ulong offset, uint * samples)
{
	XWPSFunctionSdParams * paramssd = (XWPSFunctionSdParams*)params;
	int n = paramssd->n;
	uchar buf[max_Sd_n * ((24 + 7) >> 3)];
	uchar *p;
	int i;
	int code = paramssd->DataSource.accessOnly(offset >> 3, n * 3, buf, &p);
	if (code < 0)
		return code;
		
	for (i = 0; i < n; ++i) 
	{
		samples[i] = (*p << 16) + (p[1] << 8) + p[2];
		p += 3;
  }
  return 0;
}

int XWPSFunctionSd::gets32(ulong offset, uint * samples)
{
	XWPSFunctionSdParams * paramssd = (XWPSFunctionSdParams*)params;
	int n = paramssd->n;
	uchar buf[max_Sd_n * ((32 + 7) >> 3)];
	uchar *p;
	int i;
	int code = paramssd->DataSource.accessOnly(offset >> 3, n * 4, buf, &p);
	if (code < 0)
		return code;
		
	for (i = 0; i < n; ++i) 
	{
		samples[i] = (*p << 24) + (p[1] << 16) + (p[2] << 8) + p[3];
		p += 4;
  }
  return 0;
}

void XWPSFunctionSd::interpolateLinear(const float *fparts,
		 										 const ulong *factors, 
		 										 float *samples, 
		 										 ulong offset, 
		 										 int m)
{
	int j;
	XWPSFunctionSdParams * paramssd = (XWPSFunctionSdParams*)params;

top:
  if (m == 0) 
  {
		uint sdata[max_Sd_n];

		(this->*(fn_get_samples[paramssd->BitsPerSample]))(offset, sdata);
		for (j = paramssd->n - 1; j >= 0; --j)
	    samples[j] = sdata[j];
  } 
  else 
  {
		float fpart = *fparts++;
		float samples1[max_Sd_n];

		if (is_fzero(fpart)) 
		{
	    ++factors;
	    --m;
	    goto top;
		}
		interpolateLinear(fparts, factors + 1, samples, offset, m - 1);
		interpolateLinear(fparts, factors + 1, samples1, offset + *factors, m - 1);
		for (j = paramssd->n - 1; j >= 0; --j)
	    samples[j] += (samples1[j] - samples[j]) * fpart;
  }
}

int XWPSFunctionSd::isMonotonicSd(const float *lower, const float *upper, PSFunctionEffort )
{
	XWPSFunctionSdParams * paramssd = (XWPSFunctionSdParams*)params;
	float d0 = paramssd->Domain[0], d1 = paramssd->Domain[1];
  float v0 = lower[0], v1 = upper[0];
  float e0, e1, w0, w1;
  float r0[max_Sd_n], r1[max_Sd_n];
  int code, i, result;
  
  if (paramssd->m > 1)
		return (int)(XWPSError::Undefined);
  if (lower[0] > paramssd->Domain[1] || upper[0] < paramssd->Domain[0])
		return (int)(XWPSError::RangeCheck);
  if (paramssd->n > sizeof(int) * 4 - 1)
		return 0;
  if (paramssd->Encode)
		e0 = paramssd->Encode[0], e1 = paramssd->Encode[1];
  else
		e0 = 0, e1 = paramssd->Size[0];
  w0 = (v0 - d0) * (e1 - e0) / (d1 - d0) + e0;
  if (w0 < 0)
		w0 = 0;
  else if (w0 >= paramssd->Size[0] - 1)
		w0 = paramssd->Size[0] - 1;
  w1 = (v1 - d0) * (e1 - e0) / (d1 - d0) + e0;
  if (w1 < 0)
		w1 = 0;
  else if (w1 >= paramssd->Size[0] - 1)
		w1 = paramssd->Size[0] - 1;
  if ((int)w0 != (int)w1)
		return (int)(XWPSError::Undefined);
    
  code = evaluate(lower, r0);
  if (code < 0)
		return code;
  evaluate(upper, r1);
  if (code < 0)
		return code;
  for (i = 0, result = 0; i < paramssd->n; ++i) 
  {
		double diff = r1[i] - r0[i];

		result |= (diff < 0 ? FN_MONOTONIC_DECREASING :
	     					diff > 0 ? FN_MONOTONIC_INCREASING :
	     					FN_MONOTONIC_DECREASING | FN_MONOTONIC_INCREASING) << (2 * i);
  }
  return result;
}

#define MASK1 ((uint)(~0) / 3)

XWPSFunctionElIn::XWPSFunctionElIn(XWPSFunctionElInParams * paramsA)
	:XWPSFunction()
{
	head.type = function_type_ExponentialInterpolation;
	head.procs.evaluate_.evaluateelin = &XWPSFunctionElIn::evaluateElIn;
	head.procs.is_monotonic_.is_monotonicelin = &XWPSFunctionElIn::isMonotonicElIn;
	head.procs.get_info_.get_infoelin = &XWPSFunction::getInfoDefault;
	head.procs.get_params_.get_paramselin = &XWPSFunctionElIn::getParamsElIn;
	
	XWPSFunctionElInParams * paramsB = new XWPSFunctionElInParams;
	*paramsB = *paramsA;
	paramsA->Domain = 0;
	paramsA->Range = 0;
	paramsA->C0 = 0;
	paramsA->C1 = 0;
	params = paramsB;
	head.is_monotonic = domainIsMonotonic(EFFORT_MODERATE);
}

XWPSFunctionElIn::~XWPSFunctionElIn()
{
	if (params)
	{
		delete params;
		params = 0;
	}
}

int XWPSFunctionElIn::evaluateElIn(const float *in, float *out)
{
	XWPSFunctionElInParams * paramselin = (XWPSFunctionElInParams*)params;
	double arg = in[0], raised;
  int i;

  if (arg < paramselin->Domain[0])
		arg = paramselin->Domain[0];
  else if (arg > paramselin->Domain[1])
		arg = paramselin->Domain[1];
  raised = pow(arg, (double)(paramselin->N));
  for (i = 0; i < paramselin->n; ++i) 
  {
		float v0 = (paramselin->C0 == 0 ? 0.0 : paramselin->C0[i]);
		float v1 = (paramselin->C1 == 0 ? 1.0 : paramselin->C1[i]);
		double value = v0 + raised * (v1 - v0);

		if (paramselin->Range) 
		{
	    float r0 = paramselin->Range[2 * i], r1 = paramselin->Range[2 * i + 1];

	    if (value < r0)
				value = r0;
	    else if (value > r1)
				value = r1;
		}
		out[i] = value;
  }
  return 0;
}

int XWPSFunctionElIn::getLength()
{
	return sizeof(XWPSFunctionElIn);
}

const char * XWPSFunctionElIn::getTypeName()
{
	return "functionelin";
}

int XWPSFunctionElIn::getParamsElIn(XWPSContextState * ctx, XWPSParamList *plist)
{
	XWPSFunctionElInParams * paramselin = (XWPSFunctionElInParams*)params;
	
	int ecode = getParamsCommon(ctx, plist);
  int code;

  if (paramselin->C0) 
  {
		if ((code = plist->writeFloatValues(ctx, "C0", paramselin->C0,  paramselin->n, true)) < 0)
	    ecode = code;
  }
  if (paramselin->C1) 
  {
		if ((code = plist->writeFloatValues(ctx, "C1", paramselin->C1, paramselin->n, true)) < 0)
	    ecode = code;
  }
  if ((code = plist->writeFloat(ctx, "N", &paramselin->N)) < 0)
		ecode = code;
  return ecode;
}

int XWPSFunctionElIn::isMonotonicElIn(const float *lower, const float *upper, PSFunctionEffort)
{
	int i, result;
	XWPSFunctionElInParams * paramselin = (XWPSFunctionElInParams*)params;
	if (lower[0] > paramselin->Domain[1] ||	upper[0] < paramselin->Domain[0])
	{
		return (int)(XWPSError::RangeCheck);
	}
  
  for (i = 0, result = 0; i < paramselin->n; ++i) 
  {
		double diff = (paramselin->C1 == 0 ? 1.0 : paramselin->C1[i]) -
			   (paramselin->C0 == 0 ? 0.0 : paramselin->C0[i]);

		if (paramselin->N < 0)
	    diff = -diff;
		else if (paramselin->N == 0)
	    diff = 0;
		result |=   (diff < 0 ? FN_MONOTONIC_DECREASING :  diff > 0 ? FN_MONOTONIC_INCREASING :
	     		FN_MONOTONIC_DECREASING | FN_MONOTONIC_INCREASING) <<  (2 * i);
  }
  return result;
}

XWPSFunction1ItSg::XWPSFunction1ItSg(XWPSFunction1ItSgParams * paramsA)
	:XWPSFunction()
{
	head.type = function_type_1InputStitching;
	head.procs.evaluate_.evaluate1itsg = &XWPSFunction1ItSg::evaluate1ItSg;
	head.procs.is_monotonic_.is_monotonic1itsg = &XWPSFunction1ItSg::isMonotonic1ItSg;
	head.procs.get_info_.get_info1itsg = &XWPSFunction1ItSg::getInfo1ItSg;
	head.procs.get_params_.get_params1itsg = &XWPSFunction1ItSg::getParams1ItSg;
		
	XWPSFunction1ItSgParams * paramsB = new XWPSFunction1ItSgParams;
	*paramsB = *paramsA;
	paramsA->Domain = 0;
	paramsA->Range = 0;
	paramsA->Functions = 0;
	paramsA->Bounds = 0;
	paramsA->Encode = 0;
	params = paramsB;
	head.is_monotonic = domainIsMonotonic(EFFORT_MODERATE);
}

XWPSFunction1ItSg::~XWPSFunction1ItSg()
{
	if (params)
	{
		delete params;
		params = 0;
	}
}

int XWPSFunction1ItSg::evaluate1ItSg(const float *in, float *out)
{
	XWPSFunction1ItSgParams * params1 = (XWPSFunction1ItSgParams*)params;
	float arg = in[0], b0, b1, e0, encoded;
  int k = params1->k;
  int i;

  if (arg < params1->Domain[0]) 
  {
		arg = params1->Domain[0];
		i = 0;
  } 
  else if (arg > params1->Domain[1]) 
  {
		arg = params1->Domain[1];
		i = k - 1;
  } 
  else 
  {
		for (i = 0; i < k - 1; ++i)
	    if (arg <= params1->Bounds[i])
				break;
  }
  b0 = (i == 0 ? params1->Domain[0] : params1->Bounds[i - 1]);
  b1 = (i == k - 1 ? params1->Domain[1] : params1->Bounds[i]);
  e0 = params1->Encode[2 * i];
  encoded =	(arg - b0) * (params1->Encode[2 * i + 1] - e0) / (b1 - b0) + e0;
  return params1->Functions[i]->evaluate(&encoded, out);
}

int XWPSFunction1ItSg::getLength()
{
	return sizeof(XWPSFunction1ItSg);
}

const char * XWPSFunction1ItSg::getTypeName()
{
	return "functionitsg";
}

void XWPSFunction1ItSg::getInfo1ItSg(XWPSFunctionInfo *pfi)
{
	XWPSFunction1ItSgParams * params1 = (XWPSFunction1ItSgParams*)params;
	getInfoDefault(pfi);
  pfi->Functions = params1->Functions;
  pfi->num_Functions = params1->k;
}

int XWPSFunction1ItSg::getParams1ItSg(XWPSContextState * ctx, XWPSParamList *plist)
{
	XWPSFunction1ItSgParams * params1 = (XWPSFunction1ItSgParams*)params;
	int ecode = getParamsCommon(ctx, plist);
  int code;

  if ((code = plist->writeFloatValues(ctx, "Bounds", params1->Bounds, params1->k - 1, true)) < 0)
		ecode = code;
  if ((code = plist->writeFloatValues(ctx, "Encode", params1->Encode, 2 * params1->k, true)) < 0)
		ecode = code;
  return ecode;
}

int XWPSFunction1ItSg::isMonotonic1ItSg(const float *lower, const float *upper, PSFunctionEffort effort)
{
	XWPSFunction1ItSgParams * params1 = (XWPSFunction1ItSgParams*)params;
	float v0 = lower[0], v1 = upper[0];
  float d0 = params1->Domain[0], d1 = params1->Domain[1];
  int k = params1->k;
  int i;
  int result = 0;

  if (v0 < d0)
		v0 = d0;
  if (v1 > d1)
		v1 = d1;
    
  for (i = 0; i < params1->k; ++i) 
  {
		float b0 = (i == 0 ? d0 : params1->Bounds[i - 1]);
		float b1 = (i == k - 1 ? d1 : params1->Bounds[i]);
		float e0, e1;
		float w0, w1;
		int code;

		if (v0 >= b1 || v1 <= b0)
	    continue;
		e0 = params1->Encode[2 * i];
		e1 = params1->Encode[2 * i + 1];
		w0 = (qMax(v0, b0) - b0) * (e1 - e0) / (b1 - b0) + e0;
		w1 = (qMin(v1, b1) - b0) * (e1 - e0) / (b1 - b0) + e0;
		if (w0 > w1) 
		{
	    code = params1->Functions[i]->isMonotonic(&w1, &w0, effort);
	    if (code > 0)
	    	code = ((code & MASK1) << 1) | ((code & (MASK1 << 1)) >> 1);
		} 
		else 
	    code = params1->Functions[i]->isMonotonic(&w0, &w1, effort);
	    
		if (result == 0)
	    result = code;
		else 
		{
	    result &= code;
	    code = result | ((result & MASK1) << 1) |	((result & (MASK1 << 1)) >> 1);
		}
  }
  return result;
}

XWPSFunctionAdOt::XWPSFunctionAdOt(XWPSFunctionAdOtParams * paramsA)
	:XWPSFunction()
{
	head.type = function_type_ArrayedOutput;
	head.procs.evaluate_.evaluateadot = &XWPSFunctionAdOt::evaluateAdOt;
	head.procs.is_monotonic_.is_monotonicadot = &XWPSFunctionAdOt::isMonotonicAdOt;
	head.procs.get_info_.get_info = &XWPSFunction::getInfoDefault;
	head.procs.get_params_.get_params = &XWPSFunction::getParamsCommon;
		
	XWPSFunctionAdOtParams * paramsB = new XWPSFunctionAdOtParams;
	*paramsB = *paramsA;
	paramsA->Domain = 0;
	paramsA->Range = 0;
	paramsA->Functions = 0;
	params = paramsB;
	int is_monotonic = 0;
	for (int i = 0; i < paramsA->n; ++i) 
	{
		XWPSFunction *psubfn = paramsA->Functions[i];
		int sub_mono;

		sub_mono = psubfn->domainIsMonotonic(EFFORT_MODERATE);
		if (i == 0 || sub_mono < 0)
	    is_monotonic = sub_mono;
		else if (is_monotonic >= 0)
	    is_monotonic &= sub_mono;
  }
  head.is_monotonic = is_monotonic;
}

XWPSFunctionAdOt::~XWPSFunctionAdOt()
{
	if (params)
	{
		delete params;
		params = 0;
	}
}

int XWPSFunctionAdOt::evaluateAdOt(const float *in, float *out)
{
	XWPSFunctionAdOtParams * paramsa = (XWPSFunctionAdOtParams*)params;
	for (int i = 0; i < paramsa->n; ++i) 
	{
		int code =  paramsa->Functions[i]->evaluate(in, out + i);

		if (code < 0)
	    return code;
  }
  return 0;
}

int XWPSFunctionAdOt::getLength()
{
	return sizeof(XWPSFunctionAdOt);
}

const char * XWPSFunctionAdOt::getTypeName()
{
	return "functionadot";
}

int XWPSFunctionAdOt::isMonotonicAdOt(const float *lower, const float *upper, PSFunctionEffort effort)
{
	int i, result;
	XWPSFunctionAdOtParams * paramsa = (XWPSFunctionAdOtParams*)params;
  for (i = 0, result = 0; i < paramsa->n; ++i) 
  {
		int code = paramsa->Functions[i]->isMonotonic(lower, upper, effort);

		if (code <= 0)
	    return code;
		result |= code << (2 * i);
  }
  return result;
}

XWPSFunctionPtCr::XWPSFunctionPtCr(XWPSfunctionPtCrParams * paramsA)
	:XWPSFunction()
{
	head.type = function_type_PostScript_Calculator;
	head.procs.evaluate_.evaluateptcr = &XWPSFunctionPtCr::evaluatePtCr;
	head.procs.is_monotonic_.is_monotonicptcr = &XWPSFunctionPtCr::isMonotonicPtCr;
	head.procs.get_info_.get_infoptcr = &XWPSFunctionPtCr::getInfoPtCr;
	head.procs.get_params_.get_params = &XWPSFunction::getParamsCommon;
		
	XWPSfunctionPtCrParams * paramsB = new XWPSfunctionPtCrParams;
	*paramsB = *paramsA;
	paramsA->Domain = 0;
	paramsA->Range = 0;
	paramsA->ops.data = 0;
	params = paramsB;
	head.is_monotonic = domainIsMonotonic(EFFORT_MODERATE);
	data_source.fn = this;
	data_source.access = &XWPSDataSource::accessCalc;
}

XWPSFunctionPtCr::~XWPSFunctionPtCr()
{
	if (params)
	{
		XWPSfunctionPtCrParams * paramsp = (XWPSfunctionPtCrParams*)params;
		if (paramsp->ops.data)
		{
			delete [] paramsp->ops.data;
			paramsp->ops.data = 0;
		}
		delete params;
		params = 0;
	}
}

int XWPSFunctionPtCr::calcPut(XWPSStream *s)
{
	XWPSfunctionPtCrParams * paramsp = (XWPSfunctionPtCrParams*)params;
	calc_put_ops(s, paramsp->ops.data, paramsp->ops.size - 1);
  return 0;
}

int XWPSFunctionPtCr::evaluatePtCr(const float *in, float *out)
{
	XWPSfunctionPtCrParams * paramsp = (XWPSfunctionPtCrParams*)params;
	PSCalcValue vstack[1 + MAX_VSTACK + 1];
  PSCalcValue *vsp = vstack + paramsp->m;
  uchar *p = paramsp->ops.data;
  int i;

  static const struct op_defn_s 
  {
		uchar opcode[16];	
  } op_defn_table[] = {

#define O4(op) op,op,op,op
#define E PtCr_typecheck
#define E4 O4(E)
#define N PtCr_no_op
	/* 0-operand operators */
#define OP_NONE(op)\
  {{O4(op), O4(op), O4(op), O4(op)}}
	/* 1-operand operators */
#define OP1(b, i, f)\
  {{E,b,i,f, E,b,i,f, E,b,i,f, E,b,i,f}}
#define OP_NUM1(i, f)\
  OP1(E, i, f)
#define OP_MATH1(f)\
  OP1(E, PtCr_int_to_float, f)
#define OP_ANY1(op)\
  OP1(op, op, op)
	/* 2-operand operators */
#define OP_NUM2(i, f)\
  {{E4, E4, E,E,i,PtCr_2nd_int_to_float, E,E,PtCr_int_to_float,f}}
#define OP_INT_BOOL2(i)\
  {{E4, E,i,i,E, E,i,i,E, E4}}
#define OP_MATH2(f)\
  {{E4, E4, E,E,PtCr_int2_to_float,PtCr_2nd_int_to_float,\
    E,E,PtCr_int_to_float,f}}
#define OP_INT2(i)\
  {{E4, E4, E,E,i,E, E4}}
#define OP_REL2(i, f)\
  {{E4, E,i,E,E, E,E,i,PtCr_2nd_int_to_float, E,E,PtCr_int_to_float,f}}
#define OP_ANY2(op)\
  {{E4, E,op,op,op, E,op,op,op, E,op,op,op}}

	OP_NUM1(PtCr_abs_int, PtCr_abs),	/* abs */
	OP_NUM2(PtCr_add_int, PtCr_add),	/* add */
	OP_INT_BOOL2(PtCr_and),  /* and */
	OP_MATH2(PtCr_atan),	/* atan */
	OP_INT2(PtCr_bitshift),	/* bitshift */
	OP_NUM1(N, PtCr_ceiling),	/* ceiling */
	OP_MATH1(PtCr_cos),	/* cos */
	OP_NUM1(N, PtCr_cvi),	/* cvi */
	OP_NUM1(PtCr_int_to_float, N),	/* cvr */
	OP_MATH2(PtCr_div),	/* div */
	OP_MATH2(PtCr_exp),	/* exp */
	OP_NUM1(N, PtCr_floor),	/* floor */
	OP_INT2(PtCr_idiv),	/* idiv */
	OP_MATH1(PtCr_ln),	/* ln */
	OP_MATH1(PtCr_log),	/* log */
	OP_INT2(PtCr_mod),	/* mod */
	OP_NUM2(PtCr_mul_int, PtCr_mul),	/* mul */
	OP_NUM1(PtCr_neg_int, PtCr_neg),	/* neg */
	OP1(PtCr_not, PtCr_not, E),	/* not */
	OP_INT_BOOL2(PtCr_or),  /* or */
	OP_NUM1(N, PtCr_round),	/* round */
	OP_MATH1(PtCr_sin),	/* sin */
	OP_MATH1(PtCr_sqrt),	/* sqrt */
	OP_NUM2(PtCr_sub_int, PtCr_sub),	/* sub */
	OP_NUM1(N, PtCr_truncate),	/* truncate */
	OP_INT_BOOL2(PtCr_xor),  /* xor */

	OP_REL2(PtCr_eq_int, PtCr_eq),	/* eq */
	OP_NUM2(PtCr_ge_int, PtCr_ge),	/* ge */
	OP_NUM2(PtCr_gt_int, PtCr_gt),	/* gt */
	OP_NUM2(PtCr_le_int, PtCr_le),	/* le */
	OP_NUM2(PtCr_lt_int, PtCr_lt),	/* lt */
	OP_REL2(PtCr_ne_int, PtCr_ne),	/* ne */

	OP1(E, PtCr_copy, E),	/* copy */
	OP_ANY1(PtCr_dup),	/* dup */
	OP_ANY2(PtCr_exch),	/* exch */
	OP1(E, PtCr_index, E),	/* index */
	OP_ANY1(PtCr_pop),	/* pop */
	OP_INT2(PtCr_roll),	/* roll */

	OP_NONE(PtCr_byte),		/* byte */
	OP_NONE(PtCr_int),		/* int */
	OP_NONE(PtCr_float),		/* float */
	OP_NONE(PtCr_true),		/* true */
	OP_NONE(PtCr_false),		/* false */

	OP1(PtCr_if, E, E),		/* if */
	OP_NONE(PtCr_else),		/* else */
	OP_NONE(PtCr_return)		/* return */
  };

  vstack[0].type = CVT_NONE;	/* catch underflow */
  for (i = 0; i < paramsp->m; ++i)
		store_float(&vstack[i + 1], in[i]);

  for (; ; ) 
  {
		int code, n;

sw:
		switch (op_defn_table[*p++].opcode[(vsp[-1].type << 2) + vsp->type]) 
		{

			case PtCr_no_op:
	    	continue;
	    	
			case PtCr_typecheck:
	    	return (int)(XWPSError::TypeCheck);

			case PtCr_int_to_float:
	    	store_float(vsp, (float)vsp->value.i);
	    	--p; 
	    	goto sw;
	    	
			case PtCr_int2_to_float:
	    	store_float(vsp, (float)vsp->value.i);
			case PtCr_2nd_int_to_float:
	    	store_float(vsp - 1, (float)vsp[-1].value.i);
	    	--p; 
	    	goto sw;

			case PtCr_abs_int:
	    	if (vsp->value.i < 0)
					goto neg_int;
					
	    	continue;
	    	
			case PtCr_abs:
	    	vsp->value.f = fabs(vsp->value.f);
	    	continue;
	    	
			case PtCr_add_int: 
				{
	    		int int1 = vsp[-1].value.i, int2 = vsp->value.i;

	    		if ((int1 ^ int2) >= 0 && ((int1 + int2) ^ int1) < 0)
						store_float(vsp - 1, (double)int1 + int2);
	    		else
						vsp[-1].value.i = int1 + int2;
	    		--vsp; 
	    		continue;
				}
				
			case PtCr_add:
	    	vsp[-1].value.f += vsp->value.f;
	    	--vsp; 
	    	continue;
	    	
			case PtCr_and:
	    	vsp[-1].value.i &= vsp->value.i;
	    	--vsp; 
	    	continue;
	    	
			case PtCr_atan: 
				{
	    		double result;

	    		code = ps_atan2_degrees(vsp[-1].value.f, vsp->value.f,  &result);
	    		if (code < 0)
						return code;
	    		vsp[-1].value.f = result;
	    		--vsp; 
	    		continue;
				}
				
			case PtCr_bitshift:
#define MAX_SHIFT (ARCH_SIZEOF_INT * 8 - 1)
	    	if (vsp->value.i < -MAX_SHIFT || vsp->value.i > MAX_SHIFT)
					vsp[-1].value.i = 0;
#undef MAX_SHIFT
	    	else if ((n = vsp->value.i) < 0)
					vsp[-1].value.i = ((uint)(vsp[-1].value.i)) >> -n;
	    	else
					vsp[-1].value.i <<= n;
	    	--vsp; 
	    	continue;
	    	
			case PtCr_ceiling:
	    	vsp->value.f = ceil(vsp->value.f);
	    	continue;
	    	
			case PtCr_cos:
	    	vsp->value.f = cos(vsp->value.f);
	    	continue;
	    	
			case PtCr_cvi:
	    	vsp->value.i = (int)(vsp->value.f);
	    	vsp->type = CVT_INT;
	    	continue;
	    	
			case PtCr_cvr:
	    	continue;	
	    	
			case PtCr_div:
	    	if (vsp->value.f == 0)
					return (XWPSError::UndefinedResult);
	    	vsp[-1].value.f /= vsp->value.f;
	    	--vsp; 
	    	continue;
	    	
			case PtCr_exp:
	    	vsp[-1].value.f = pow(vsp[-1].value.f, vsp->value.f);
	    	--vsp; 
	    	continue;
	    	
			case PtCr_floor:
	    	vsp->value.f = floor(vsp->value.f);
	    	continue;
	    	
			case PtCr_idiv:
	    	if (vsp->value.i == 0)
					return (XWPSError::UndefinedResult);
	    	if ((vsp[-1].value.i /= vsp->value.i) == min_int &&	vsp->value.i == -1)
					return (int)(XWPSError::RangeCheck);
	    	--vsp; 
	    	continue;
	    	
			case PtCr_ln:
	    	vsp->value.f = log(vsp->value.f);
	    	continue;
	    	
			case PtCr_log:
	    	vsp->value.f = log10(vsp->value.f);
	    	continue;
	    	
			case PtCr_mod:
	    	if (vsp->value.i == 0)
					return (XWPSError::UndefinedResult);
	    	vsp[-1].value.i %= vsp->value.i;
	    	--vsp; 
	    	continue;
	    	
			case PtCr_mul_int: 
				{
	    		double prod = (double)vsp[-1].value.i * vsp->value.i;

	    		if (prod < min_int || prod > max_int)
						store_float(vsp - 1, prod);
	    		else
						vsp[-1].value.i = (int)prod;
	    		--vsp; 
	    		continue;
				}
				
			case PtCr_mul:
	    	vsp[-1].value.f *= vsp->value.f;
	    	--vsp; 
	    	continue;
	    	
			case PtCr_neg_int:
neg_int:
	    	if (vsp->value.i == min_int)
					store_float(vsp, (float)vsp->value.i); 
	    	else
					vsp->value.i = -vsp->value.i;
	    	continue;
	    	
			case PtCr_neg:
	    	vsp->value.f = -vsp->value.f;
	    	continue;
	    	
			case PtCr_not_bool:
	    	vsp->value.i = !vsp->value.i;
	    	continue;
	    	
			case PtCr_not:
	    	vsp->value.i = ~vsp->value.i;
	    	continue;
	    	
			case PtCr_or:
	    	vsp[-1].value.i |= vsp->value.i;
	    	--vsp; 
	    	continue;
	    	
			case PtCr_round:
	    	vsp->value.f = floor(vsp->value.f + 0.5);
	    	continue;
	    	
			case PtCr_sin:
	    	vsp->value.f = sin(vsp->value.f);
	    	continue;
	    	
			case PtCr_sqrt:
	    	vsp->value.f = sqrt(vsp->value.f);
	    	continue;
	    	
			case PtCr_sub_int: 
				{
	    		int int1 = vsp[-1].value.i, int2 = vsp->value.i;

	    		if ((int1 ^ int2) < 0 && ((int1 - int2) ^ int1) >= 0)
						store_float(vsp - 1, (double)int1 - int2);
	    		else
						vsp[-1].value.i = int1 - int2;
	    		--vsp; 
	    		continue;
				}
				
			case PtCr_sub:
	    	vsp[-1].value.f -= vsp->value.f;
	    	--vsp; 
	    	continue;
	    	
			case PtCr_truncate:
	    	vsp->value.f = (vsp->value.f < 0 ? ceil(vsp->value.f) :  floor(vsp->value.f));
	    	continue;
	    	
			case PtCr_xor:
	    	vsp[-1].value.i ^= vsp->value.i;
	    	--vsp; 
	    	continue;

#define DO_REL(rel, m)\
  vsp[-1].value.i = vsp[-1].value.m rel vsp->value.m

			case PtCr_eq_int:
	    	DO_REL(==, i);
	    	goto rel;
	    	
			case PtCr_eq:
	    	DO_REL(==, f);
	    	
rel:
	    	vsp[-1].type = CVT_BOOL;
	    	--vsp; 
	    	continue;
	    	
			case PtCr_ge_int:
	    	DO_REL(>=, i);
	    	goto rel;
	    	
			case PtCr_ge:
	    	DO_REL(>=, f);
	    	goto rel;
	    	
			case PtCr_gt_int:
	    	DO_REL(>, i);
	    	goto rel;
	    	
			case PtCr_gt:
	    	DO_REL(>, f);
	    	goto rel;
	    	
			case PtCr_le_int:
	    	DO_REL(<=, i);
	    	goto rel;
	    	
			case PtCr_le:
	    	DO_REL(<=, f);
	    	goto rel;
	    	
			case PtCr_lt_int:
	    	DO_REL(<, i);
	    	goto rel;
	    	
			case PtCr_lt:
	    	DO_REL(<, f);
	    	goto rel;
	    	
			case PtCr_ne_int:
	    	DO_REL(!=, i);
	    	goto rel;
	    	
			case PtCr_ne:
	    	DO_REL(!=, f);
	    	goto rel;

#undef DO_REL

			case PtCr_copy:
	    	i = vsp->value.i;
	    	n = vsp - vstack;
	    	if (i < 0 || i >= n)
					return (int)(XWPSError::RangeCheck);
	    	if (i > MAX_VSTACK - (n - 1))
					return (int)(XWPSError::LimitCheck);
	    	memcpy(vsp, vsp - i, i * sizeof(*vsp));
	    	vsp += i - 1;
	    	continue;
	    	
			case PtCr_dup:
	    	vsp[1] = *vsp;
	    	goto push;
	    	
			case PtCr_exch:
	    	vstack[MAX_VSTACK] = *vsp;
	    	*vsp = vsp[-1];
	    	vsp[-1] = vstack[MAX_VSTACK];
	    	continue;
	    	
			case PtCr_index:
	    	i = vsp->value.i;
	    	if (i < 0 || i >= vsp - vstack - 1)
					return (int)(XWPSError::RangeCheck);
	    	*vsp = vsp[-i - 1];
	    	continue;
	    	
			case PtCr_pop:
	    	--vsp;
	    	continue;
	    	
			case PtCr_roll:
	    	n = vsp[-1].value.i;
	    	i = vsp->value.i;
	    	if (n < 0 || n > vsp - vstack - 2)
					return (int)(XWPSError::RangeCheck);
	    	for (; i > 0; i--) 
	    	{
					memmove(vsp - n, vsp - (n + 1), n * sizeof(*vsp));
					vsp[-(n + 1)] = vsp[-1];
	    	}
	    	for (; i < 0; i++) 
	    	{
					vsp[-1] = vsp[-(n + 1)];
					memmove(vsp - (n + 1), vsp - n, n * sizeof(*vsp));
	    	}
	    	vsp -= 2;
	    	continue;

			case PtCr_byte:
	    	vsp[1].value.i = *p++, vsp[1].type = CVT_INT;
	    	goto push;
	    	
			case PtCr_int /* native */:
	    	memcpy(&vsp[1].value.i, p, sizeof(int));
	    	vsp[1].type = CVT_INT;
	    	p += sizeof(int);
	    	goto push;
	    	
			case PtCr_float /* native */:
	    	memcpy(&vsp[1].value.f, p, sizeof(float));
	    	vsp[1].type = CVT_FLOAT;
	    	p += sizeof(float);
	    	goto push;
	    	
			case PtCr_true:
	    	vsp[1].value.i = true, vsp[1].type = CVT_BOOL;
	    	goto push;
	    	
			case PtCr_false:
	    	vsp[1].value.i = false, vsp[1].type = CVT_BOOL;
push:
	    	if (vsp == &vstack[MAX_VSTACK])
					return (int)(XWPSError::LimitCheck);
	    	++vsp;
	    	continue;

			case PtCr_if:
	    	if ((vsp--)->value.i) 
	    	{
					p += 2;
					continue;
	    	}
	    	
			case PtCr_else:
	    	p += 2 + (p[0] << 8) + p[1];
	    	continue;
	    	
			case PtCr_return:
	    	goto fin;
		}
  }
  
fin:

  if (vsp != vstack + paramsp->n)
		return (int)(XWPSError::RangeCheck);
			
  for (i = 0; i < paramsp->n; ++i) 
  {
		switch (vstack[i + 1].type) 
		{
			case CVT_INT:
	    	out[i] = vstack[i + 1].value.i;
	    	break;
	    	
			case CVT_FLOAT:
	    	out[i] = vstack[i + 1].value.f;
	    	break;
	    	
			default:
	    	return (int)(XWPSError::TypeCheck);
		}
  }
  return 0;
}

int XWPSFunctionPtCr::getLength()
{
	return sizeof(XWPSFunctionPtCr);
}

const char * XWPSFunctionPtCr::getTypeName()
{
	return "functionptcr";
}

void XWPSFunctionPtCr::getInfoPtCr(XWPSFunctionInfo *pfi)
{
	getInfoDefault(pfi);
  pfi->DataSource = &data_source;
  {
		XWPSStream s;

		s.writePositionOnly();
		calcPut(&s);
		pfi->data_size = s.tell();
  }
}

XWPSFunctionVa::XWPSFunctionVa(XWPSFunctionVaParams * paramsA)
	:XWPSFunction()
{
	head.procs.evaluate_.evaluateva = NULL;
	head.procs.is_monotonic_.is_monotonicva = &XWPSFunctionVa::isMonotonicVa;
	head.procs.get_info_.get_info = &XWPSFunction::getInfoDefault;
	head.procs.get_params_.get_params = &XWPSFunction::getParamsCommon;
		
	XWPSFunctionVaParams * paramsB = new XWPSFunctionVaParams;
	*paramsB = *paramsA;
	paramsA->Domain = 0;
	paramsA->Range = 0;
	paramsA->eval_data = 0;
	params = paramsB;
	head.is_monotonic = paramsA->is_monotonic;
}

XWPSFunctionVa::~XWPSFunctionVa()
{
	if (params)
	{
		delete params;
		params = 0;
	}
}

int XWPSFunctionVa::getLength()
{
	return sizeof(XWPSFunctionVa);
}

const char * XWPSFunctionVa::getTypeName()
{
	return "functionva";
}

int XWPSFunctionVa::isMonotonicVa(const float *, const float *, PSFunctionEffort )
{
	XWPSFunctionVaParams * paramsv = (XWPSFunctionVaParams*)params;
	return paramsv->is_monotonic;
}
