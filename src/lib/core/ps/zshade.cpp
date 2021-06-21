/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include "XWPSError.h"
#include "XWPSFunction.h"
#include "XWPSShading.h"
#include "XWPSColor.h"
#include "XWPSPattern.h"
#include "XWPSState.h"
#include "XWPSDataSource.h"
#include "XWPSContextState.h"

int XWPSContextState::buildDirectionalShading(XWPSRef * op, 
	                            float *Coords, 
	                            int num_Coords,
			                        float Domain[2], 
			                        XWPSFunction ** pFunction,
			                        bool Extend[2])
{
	int code = op->dictFloatsParam(this, "Coords", num_Coords, Coords, NULL);
  static const float default_Domain[2] = {0, 1};
  XWPSRef *pExtend;

  *pFunction = 0;
  if (code < 0 ||
		(code = op->dictFloatsParam(this, "Domain", 2, Domain, default_Domain)) < 0 ||
			(code = buildShadingFunction(op, pFunction, 1)) < 0)
		return code;
  if (op->dictFindString(this, "Extend", &pExtend) <= 0)
		Extend[0] = Extend[1] = false;
  else 
  {
		XWPSRef E0, E1;

		if (!pExtend->isArray())
	    return pExtend->checkTypeFailed();
		else if (pExtend->size() != 2)
	    return (int)(XWPSError::RangeCheck);
		else if ((pExtend->arrayGet(this, 0L, &E0), !E0.hasType(XWPSRef::Boolean)) ||
		 					(pExtend->arrayGet(this, 1L, &E1), !E1.hasType(XWPSRef::Boolean)))
	    return pExtend->checkTypeFailed();
		Extend[0] = E0.value.boolval, Extend[1] = E1.value.boolval;
  }
  return 0;
}

int XWPSContextState::buildMeshShading(XWPSRef * op, 
	                     XWPSShadingMeshParams * params,
		                   float **pDecode, 
		                   XWPSFunction ** pFunction)
{
	int code;
  XWPSRef *pDataSource;

  *pDecode = 0;
  *pFunction = 0;
  if (op->dictFindString(this, "DataSource", &pDataSource) <= 0)
	  return (int)(XWPSError::RangeCheck);
    
  if (pDataSource->isArray()) 
  {
		uint size = pDataSource->size();
		float *data = 0;
		if (size > 0)
		{
			data = new float[size];
			XWPSRef * p = pDataSource->getArray() + size - 1;
			code = floatParams(p, size, data);
			if (code < 0) 
			{
	    	delete [] data;
	    	return code;
			}
		}
		params->DataSource.initFloats(data, size);
  } 
  else
		switch (pDataSource->type()) 
		{
	    case XWPSRef::File: 
	    	{
					XWPSStream *s;
				
					code = pDataSource->checkType(XWPSRef::File);
					if (code < 0)
						return code;
					
					s = pDataSource->getStream();
					if (s->read_id != pDataSource->size())
					{
						if (s->read_id == 0 && s->write_id == pDataSource->size())
						{
							int fcode = fileSwitchToRead(pDataSource);
							if (fcode < 0)
								return fcode;
						}
						else
							s = &invalid_file_stream;
					}
					s->incRef();
					params->DataSource.initStream(s);
					break;
	    	}
	    
	    case XWPSRef::String:
				code = pDataSource->checkRead();
				if (code < 0)
					return code;
				{
					params->DataSource.initString2(pDataSource->getBytes(), pDataSource->size());
					params->ds_bytes = pDataSource->value.bytes->arr;
					if (pDataSource->value.bytes->arr)
						pDataSource->value.bytes->arr->incRef();
				}
				break;
				
	    default:
				return pDataSource->checkTypeFailed();
		}
    if (params->DataSource.isArray()) 
    {
			params->BitsPerCoordinate = 0;
			params->BitsPerComponent = 0;
    } 
    else 
    {
			int num_decode =  4 + params->ColorSpace->numComponents() * 2;
			if ((code = op->dictIntParam(this, "BitsPerCoordinate", 1, 32, 0,  &params->BitsPerCoordinate)) < 0 ||
	    		(code = op->dictIntParam(this, "BitsPerComponent", 1, 16, 0, &params->BitsPerComponent)) < 0)
	    	return code;
	    if (num_decode > 0)
	    {
				*pDecode = new float[num_decode];
				code = op->dictFloatsParam(this, "Decode", num_decode, *pDecode, NULL);
				if (code < 0) 
				{
	    		delete [] *pDecode;
	    		*pDecode = 0;
	    		return code;
				}
			}
    }
    code = buildShadingFunction(op, pFunction, 1);
    if (code < 0) 
    {
			delete [] *pDecode;
			*pDecode = 0;
    }
  return code;
}

int XWPSContextState::buildShading(build_shading_proc_t proc)
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code;
  float box[4];
  XWPSShadingParams params;
  XWPSShading *psh;
  XWPSRef *pvalue;

  code = op->checkType(XWPSRef::Dictionary);
  if (code < 0)
  	return code;
  
  {
		XWPSColorSpace *pcs_orig = pgs->currentColorSpace();
//		int num_comp = pcs_orig->numComponents();
		XWPSColorSpace *pcs = new XWPSColorSpace(pcs_orig);
		pcs->i_ctx_p = this;
		params.ColorSpace = pcs;
		if (op->dictFindString(this, "Background", &pvalue) > 0) 
		{
	    XWPSClientColor *pcc = new XWPSClientColor;
	    params.Background = pcc;
	    code = op->dictFloatsParam(this, "Background",  pcs->numComponents(), pcc->paint.values, NULL);
	    if (code < 0)
				goto fail;
		}
  }
  if (op->dictFindString(this, "BBox", &pvalue) <= 0)
		params.have_BBox = false;
  else if ((code = op->dictFloatsParam(this, "BBox", 4, box, NULL)) == 4) 
  {
		params.BBox.p.x = box[0];
		params.BBox.p.y = box[1];
		params.BBox.q.x = box[2];
		params.BBox.q.y = box[3];
		params.have_BBox = true;
  } 
  else
		goto fail;
    
  code = op->dictBoolParam(this, "AntiAlias", false, &params.AntiAlias);
  if (code < 0)
		goto fail;
		
  code = (this->*proc)(op, &params, &psh);
  params.ColorSpace = 0;
  params.Background = 0;
  if (code < 0)
		goto fail;
    
  op->makeStruct(currentSpace(), psh);
  return code;
  
fail:
  return (code < 0 ? code : (int)(XWPSError::RangeCheck));
}

int XWPSContextState::buildShadingFunction(XWPSRef * op,XWPSFunction ** ppfn, int )
{
	XWPSRef *pFunction;

  *ppfn = 0;
  if (op->dictFindString(this, "Function", &pFunction) <= 0)
		return 0;
    
  if (pFunction->isArray()) 
  {
		uint size = pFunction->size();
		XWPSFunction **Functions;
		uint i;
		XWPSFunctionAdOtParams params;
		int code;
	
		code = pFunction->checkRead();
		if (code < 0)
  		return code;
		if (size == 0)
	    return (int)(XWPSError::RangeCheck);
		
		Functions = (XWPSFunction**)malloc((size+1) * sizeof(XWPSFunction*));
		for (i = 0; i < size; ++i) 
		{
	    XWPSRef rsubfn;

	    pFunction->arrayGet(this, (long)i, &rsubfn);
	    Functions[i] = 0;
	    code = fnbuildFunction(&rsubfn, &Functions[i]);
	    if (code < 0)
				break;
		}
		params.m = 1;
		params.Domain = 0;
		params.n = size;
		params.Range = 0;
		params.Functions = Functions;
		code = params.check();
		if (code < 0)
			return code;
			
		*ppfn = new XWPSFunctionAdOt(&params);
		return 0;
	}
	return fnbuildFunction(pFunction, ppfn);
}

int XWPSContextState::buildShading1(XWPSRef * op, 
	                  const XWPSShadingParams * pcommon,
		                XWPSShading ** ppsh)
{
	XWPSShadingFbParams params;
  int code;
  static const float default_Domain[4] = {0, 1, 0, 1};

  params.ColorSpace = pcommon->ColorSpace;
  params.Background = pcommon->Background;
  params.have_BBox = pcommon->have_BBox;
  params.BBox = pcommon->BBox;
  params.AntiAlias = pcommon->AntiAlias;
  
  if ((code = op->dictFloatsParam(this, "Domain", 4, params.Domain,  default_Domain)) < 0 ||
			(code = op->dictMatrixParam(this, "Matrix", &params.Matrix)) < 0 ||
			(code = buildShadingFunction(op, &params.Function, 2)) < 0 ||
			(code = params.check()) < 0) 
	{
		return code;
  }
  
  *ppsh = new XWPSShading(&params);
  return 0;
}

int XWPSContextState::buildShading2(XWPSRef * op, 
	                  const XWPSShadingParams * pcommon,
		                XWPSShading ** ppsh)
{
	XWPSShadingAParams params;
  int code;

  params.ColorSpace = pcommon->ColorSpace;
  params.Background = pcommon->Background;
  params.have_BBox = pcommon->have_BBox;
  params.BBox = pcommon->BBox;
  params.AntiAlias = pcommon->AntiAlias;
  if ((code = buildDirectionalShading(op, params.Coords, 4, params.Domain, &params.Function, params.Extend)) < 0 ||
			(code = params.check()) < 0) 
	{
		return code;
  }
  *ppsh = new XWPSShading(&params);
  return 0;
}

int XWPSContextState::buildShading3(XWPSRef * op, 
	                  const XWPSShadingParams * pcommon,
		                XWPSShading ** ppsh)
{
	XWPSShadingRParams params;
  int code;

  params.ColorSpace = pcommon->ColorSpace;
  params.Background = pcommon->Background;
  params.have_BBox = pcommon->have_BBox;
  params.BBox = pcommon->BBox;
  params.AntiAlias = pcommon->AntiAlias;
  if ((code = buildDirectionalShading(op, params.Coords, 6,params.Domain, &params.Function, params.Extend)) < 0 ||
			(code = params.check()) < 0) 
	{
		return code;
  }
  *ppsh = new XWPSShading(&params);
  return 0;
}

int XWPSContextState::buildShading4(XWPSRef * op, 
	                  const XWPSShadingParams * pcommon,
		                XWPSShading ** ppsh)
{
	XWPSShadingFfGtParams params;
  int code;

  params.ColorSpace = pcommon->ColorSpace;
  params.Background = pcommon->Background;
  params.have_BBox = pcommon->have_BBox;
  params.BBox = pcommon->BBox;
  params.AntiAlias = pcommon->AntiAlias;
  if ((code = buildMeshShading(op, &params, &params.Decode, &params.Function)) < 0 ||
			(code = flagBitsParam(op, &params, &params.BitsPerFlag)) < 0 ||
			(code = params.check()) < 0) 
	{
		return code;
  }
  
  *ppsh = new XWPSShading(&params);
  return 0;
}

int XWPSContextState::buildShading5(XWPSRef * op, 
	                  const XWPSShadingParams * pcommon,
		                XWPSShading ** ppsh)
{
	XWPSShadingLfGtParams params;
  int code;

  params.ColorSpace = pcommon->ColorSpace;
  params.Background = pcommon->Background;
  params.have_BBox = pcommon->have_BBox;
  params.BBox = pcommon->BBox;
  params.AntiAlias = pcommon->AntiAlias;
  if ((code = buildMeshShading(op, &params,  &params.Decode, &params.Function)) < 0 ||
			(code = op->dictIntParam(this, "VerticesPerRow", 2, max_int, 0, &params.VerticesPerRow)) < 0 ||
			(code = params.check()) < 0) 
	{
		return code;
  }
  *ppsh = new XWPSShading(&params);
  return 0;
}

int XWPSContextState::buildShading6(XWPSRef * op, 
	                  const XWPSShadingParams * pcommon,
		                XWPSShading ** ppsh)
{
	XWPSShadingCpParams params;
  int code;

  params.ColorSpace = pcommon->ColorSpace;
  params.Background = pcommon->Background;
  params.have_BBox = pcommon->have_BBox;
  params.BBox = pcommon->BBox;
  params.AntiAlias = pcommon->AntiAlias;
  if ((code = buildMeshShading(op, &params, &params.Decode, &params.Function)) < 0 ||
			(code = flagBitsParam(op, &params, &params.BitsPerFlag)) < 0 ||
			(code = params.check()) < 0) 
	{
		return code;
  }
  *ppsh = new XWPSShading(&params);
  return 0;
}

int XWPSContextState::buildShading7(XWPSRef * op, 
	                  const XWPSShadingParams * pcommon,
		                XWPSShading ** ppsh)
{
	XWPSShadingTppParams params;
  int code;

  params.ColorSpace = pcommon->ColorSpace;
  params.Background = pcommon->Background;
  params.have_BBox = pcommon->have_BBox;
  params.BBox = pcommon->BBox;
  params.AntiAlias = pcommon->AntiAlias;
  if ((code = buildMeshShading(op, &params, &params.Decode, &params.Function)) < 0 ||
			(code = flagBitsParam(op, &params,&params.BitsPerFlag)) < 0 ||
			(code = params.check()) < 0) 
	{
		return code;
  }
  *ppsh = new XWPSShading(&params);
  return 0;
}

int XWPSContextState::flagBitsParam(XWPSRef * op, 
	                  XWPSShadingMeshParams * params,
		                int *pBitsPerFlag)
{
	if (params->DataSource.isArray()) 
	{
		*pBitsPerFlag = 0;
		return 0;
  } 
  else 
		return op->dictIntParam(this, "BitsPerFlag", 2, 8, 0, pBitsPerFlag);
}

int XWPSContextState::shadingParam(XWPSRef * op, XWPSShading ** ppsh)
{
	if (!op->isStruct() || op->hasMaskedAttrs(PS_A_EXECUTABLE | PS_A_EXECUTE, PS_A_ALL))
		return (int)(XWPSError::TypeCheck);
			
  XWPSShading *psh = (XWPSShading *)(op->getStruct());
  if (psh)
  	psh->incRef();
  *ppsh = psh;
  return 0;
}

int XWPSContextState::zbuildShadingPattern()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * op2 = op - 2;
  XWPSMatrix mat;
  XWPSPattern2 templat;
  XWPSIntPattern *pdata;
  XWPSClientColor cc_instance;
  int code;

  code = op2->checkType(XWPSRef::Dictionary);
  if (code < 0)
  	return code;
  	
  code = op2->checkDictRead();
  if ((code = op[-1].readMatrix(this, &mat)) < 0 ||
			(code = op2->dictUidParam(&templat.uid, 1, this)) != 1 ||
			(code = shadingParam(op, &templat.Shading)) < 0)
	{
		return ((code < 0 ? code : (int)(XWPSError::RangeCheck)));
	}
  
  pdata = new XWPSIntPattern(op2);
  templat.client_data = pdata;
  code = cc_instance.makePattern(&templat, &mat, pgs);
  if (code < 0) 
		return code;
    
  cc_instance.pattern->incRef();
  op[-1].makeStruct(currentSpace() | PS_A_READONLY, cc_instance.pattern);
  pop(1);
  return code;
}

int XWPSContextState::zbuildShading1()
{
	return buildShading(&XWPSContextState::buildShading1);
}

int XWPSContextState::zbuildShading2()
{
	return buildShading(&XWPSContextState::buildShading2);
}

int XWPSContextState::zbuildShading3()
{
	return buildShading(&XWPSContextState::buildShading3);
}

int XWPSContextState::zbuildShading4()
{
	return buildShading(&XWPSContextState::buildShading4);
}

int XWPSContextState::zbuildShading5()
{
	return buildShading(&XWPSContextState::buildShading5);
}

int XWPSContextState::zbuildShading6()
{
	return buildShading(&XWPSContextState::buildShading6);
}

int XWPSContextState::zbuildShading7()
{
	return buildShading(&XWPSContextState::buildShading7);
}

int XWPSContextState::zcurrentSmoothness()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->makeReal(pgs->currentSmoothness());
  return 0;
}

int XWPSContextState::zsetSmoothness()
{
	XWPSRef * op = op_stack.getCurrentTop();
  double smoothness;
  int code;

  if (op->realParam(&smoothness) < 0)
		return op->checkTypeFailed();
  if ((code = pgs->setSmoothness(smoothness)) < 0)
		return code;
  pop(1);
  return 0;
}

int XWPSContextState::zshfill()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSShading *psh;
  int code = shadingParam(op, &psh);

  if (code < 0 || (code = pgs->shfill(psh)) < 0)
		return code;
  pop(1);
  return 0;
}
