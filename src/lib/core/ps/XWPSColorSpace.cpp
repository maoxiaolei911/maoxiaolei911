/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <string.h>
#include <math.h>
#include "XWPSError.h"
#include "psbittable.h"
#include "XWPSBitmap.h"
#include "XWPSPath.h"
#include "XWPSColor.h"
#include "XWPSTransferMap.h"
#include "XWPSPattern.h"
#include "XWPSFunction.h"
#include "XWPSHalftone.h"
#include "XWPSDeviceClip.h"
#include "XWPSDeviceMem.h"
#include "XWPSDeviceBbox.h"
#include "XWPSDeviceRopTexture.h"
#include "XWPSDeviceMaskClip.h"
#include "XWPSDevicePDF.h"
#include "XWPSDeviceDVI.h"
#include "XWPSDevicePatternAccum.h"
#include "XWPSState.h"
#include "XWPSCosObject.h"
#include "XWPSRef.h"
#include "XWPSContextState.h"
#include "XWPSColorSpace.h"

#define unit_frac(v, ftemp)\
  (ftemp = (v),\
   (is_fneg(ftemp) ? frac_0 : is_fge1(ftemp) ? frac_1 : float2frac(ftemp)))
   	
#define max_cached_patterns_LARGE 50
#define max_pattern_bits_LARGE 100000
#define max_cached_patterns_SMALL 5
#define max_pattern_bits_SMALL 1000

PSColorSpaceType ps_color_space_type_DeviceGray = {
    ps_color_space_index_DeviceGray, true, true,
    &XWPSColorSpace::numComponents1,
    &XWPSColorSpace::noBaseSpace, 
    &XWPSColorSpace::isEqual,
    &XWPSColorSpace::initPaint1, 
    &XWPSColorSpace::restrict01Paint1,
    &XWPSColorSpace::sameConcreteSpace,
    &XWPSColorSpace::concretizeDeviceGray, 
    &XWPSImagerState::remapConcreteDGray,
    &XWPSImagerState::remapDeviceGray, 
    &XWPSColorSpace::noInstallCSpace
};

PSColorSpaceType ps_color_space_type_DeviceRGB = {
    ps_color_space_index_DeviceRGB, true, true,
    &XWPSColorSpace::numComponents3,
    &XWPSColorSpace::noBaseSpace, 
    &XWPSColorSpace::isEqual,
    &XWPSColorSpace::initPaint3, 
    &XWPSColorSpace::restrict01Paint3,
    &XWPSColorSpace::sameConcreteSpace,
    &XWPSColorSpace::concretizeDeviceRGB, 
    &XWPSImagerState::remapConcreteDRGB,
    &XWPSImagerState::remapDeviceRGB, 
    &XWPSColorSpace::noInstallCSpace
};

PSColorSpaceType ps_color_space_type_DeviceCMYK = {
    ps_color_space_index_DeviceCMYK, true, true,
    &XWPSColorSpace::numComponents4,
    &XWPSColorSpace::noBaseSpace, 
    &XWPSColorSpace::isEqual,
    &XWPSColorSpace::initPaint4, 
    &XWPSColorSpace::restrict01Paint4,
    &XWPSColorSpace::sameConcreteSpace,
    &XWPSColorSpace::concretizeDeviceCMYK, 
    &XWPSImagerState::remapConcreteDCMYK,
    &XWPSImagerState::remapDeviceCMYK, 
    &XWPSColorSpace::noInstallCSpace
};

PSColorSpaceType ps_color_space_type_DevicePixel = {
    ps_color_space_index_DevicePixel, true, false,
    &XWPSColorSpace::numComponents1,
    &XWPSColorSpace::noBaseSpace, 
    &XWPSColorSpace::equalDevicePixel,
    &XWPSColorSpace::initPaint1, 
    &XWPSColorSpace::restrictDevicePixel,
    &XWPSColorSpace::sameConcreteSpace,
    &XWPSColorSpace::concretizeDevicePixel, 
    &XWPSImagerState::remapConcreteDPixel,
    &XWPSImagerState::remapColorDefault, 
    &XWPSColorSpace::noInstallCSpace
};

PSColorSpaceType ps_color_space_type_CIEDEFG = {
    ps_color_space_index_CIEDEFG, true, true,
    &XWPSColorSpace::numComponents4,
    &XWPSColorSpace::noBaseSpace, 
    &XWPSColorSpace::notEqual,
    &XWPSColorSpace::initCIE, 
    &XWPSColorSpace::restrictCIEDEFG,
    &XWPSColorSpace::concreteSpaceCIE,
    &XWPSColorSpace::concretizeCIEDEFG, 
    NULL,
    &XWPSImagerState::remapColorDefault, 
    &XWPSColorSpace::installCIE
};

PSColorSpaceType ps_color_space_type_CIEDEF = {
    ps_color_space_index_CIEDEF, true, true,
    &XWPSColorSpace::numComponents3,
    &XWPSColorSpace::noBaseSpace, 
    &XWPSColorSpace::notEqual,
    &XWPSColorSpace::initCIE, 
    &XWPSColorSpace::restrictCIEDEF,
    &XWPSColorSpace::concreteSpaceCIE,
    &XWPSColorSpace::concretizeCIEDEF, 
    NULL,
    &XWPSImagerState::remapColorDefault, 
    &XWPSColorSpace::installCIE
};

PSColorSpaceType ps_color_space_type_CIEABC = {
    ps_color_space_index_CIEABC, true, true,
    &XWPSColorSpace::numComponents3,
    &XWPSColorSpace::noBaseSpace,
    &XWPSColorSpace::notEqual,
    &XWPSColorSpace::initCIE, 
    &XWPSColorSpace::restrictCIEABC,
    &XWPSColorSpace::concreteSpaceCIE,
    &XWPSColorSpace::concretizeCIEABC, 
    NULL,
    &XWPSImagerState::remapCIEABC, 
    &XWPSColorSpace::installCIE
};

PSColorSpaceType ps_color_space_type_CIEA = {
    ps_color_space_index_CIEA, true, true,
    &XWPSColorSpace::numComponents1,
    &XWPSColorSpace::noBaseSpace,
    &XWPSColorSpace::notEqual,
    &XWPSColorSpace::initCIE, 
    &XWPSColorSpace::restrictCIEA,
    &XWPSColorSpace::concreteSpaceCIE,
    &XWPSColorSpace::concretizeCIEA, 
    NULL,
    &XWPSImagerState::remapColorDefault, 
    &XWPSColorSpace::installCIE
};

PSColorSpaceType ps_color_space_type_DeviceN = {
    ps_color_space_index_DeviceN, true, false,
    &XWPSColorSpace::numComponentsDeviceN,
    &XWPSColorSpace::altSpaceDeviceN, 
    &XWPSColorSpace::equalDeviceN,
    &XWPSColorSpace::initDeviceN, 
    &XWPSColorSpace::restrictDeviceN,
    &XWPSColorSpace::concreteSpaceDeviceN,
    &XWPSColorSpace::concretizeDeviceN, 
    &XWPSImagerState::remapConcreteDeviceN,
     &XWPSImagerState::remapColorDefault, 
    &XWPSColorSpace::installDeviceN
};

PSColorSpaceType ps_color_space_type_Indexed = {
    ps_color_space_index_Indexed, false, false,
    &XWPSColorSpace::numComponents1,
    &XWPSColorSpace::baseSpaceIndexed, 
    &XWPSColorSpace::equalIndexed,
    &XWPSColorSpace::initPaint1, 
    &XWPSColorSpace::restrictIndexed,
    &XWPSColorSpace::concreteSpaceIndexed,
    &XWPSColorSpace::concretizeIndexed, 
    NULL,
    &XWPSImagerState::remapColorDefault, 
    &XWPSColorSpace::installIndexed
};

PSColorSpaceType ps_color_space_type_Separation = {
    ps_color_space_index_Separation, true, false,
    &XWPSColorSpace::numComponents1,
    &XWPSColorSpace::altSpaceSeparation, 
    &XWPSColorSpace::equalSeparation,
    &XWPSColorSpace::initSeparation, 
    &XWPSColorSpace::restrict01Paint1,
    &XWPSColorSpace::concreteSpaceSeparation,
    &XWPSColorSpace::concretizeSeparation, 
    &XWPSImagerState::remapConcreteSeparation,
    &XWPSImagerState::remapColorDefault, 
    &XWPSColorSpace::installSeparation
};

PSColorSpaceType ps_color_space_type_Pattern = {
    ps_color_space_index_Pattern, false, false,
    &XWPSColorSpace::numComponentsPattern,
    &XWPSColorSpace::baseSpacePattern, 
    &XWPSColorSpace::notEqual,
    &XWPSColorSpace::initPattern, 
    &XWPSColorSpace::restrictPattern,
    &XWPSColorSpace::concreteSpaceNo,
    &XWPSColorSpace::concretizeNo, 
    NULL,
    &XWPSImagerState::remapPattern, 
    &XWPSColorSpace::installPattern
};

XWPSColorSpace::XWPSColorSpace()
	:XWPSStruct()
{
	i_ctx_p = 0;
	id = 0;
	params.pixel = 0;
	type = &ps_color_space_type_DeviceGray;
}

XWPSColorSpace::XWPSColorSpace(PSColorSpaceType * t, void *client_dataA)
	:XWPSStruct()
{
	i_ctx_p = 0;
	id = 0;
	params.pixel = 0;
	init(t, client_dataA);
}

XWPSColorSpace::XWPSColorSpace(XWPSColorSpace * src)
	:XWPSStruct()
{
	id = 0;
	params.pixel = 0;
	copy(src);
}

XWPSColorSpace::~XWPSColorSpace()
{
	freeParams();
}

XWPSColorSpace * XWPSColorSpace::altSpaceDeviceN()
{
	return params.device_n->alt_space;
}

XWPSColorSpace * XWPSColorSpace::altSpaceSeparation()
{
	return params.separation->alt_space;
}

void XWPSColorSpace::assign(XWPSColorSpace * psrc)
{
	if (!psrc)
		return ;
		
	if (this == psrc)
		return;
		
  copy(psrc);
}

XWPSColorSpace * XWPSColorSpace::baseSpace()
{
	return (this->*(type->base_space))();
}

XWPSColorSpace * XWPSColorSpace::baseSpaceIndexed()
{
	return params.indexed->base_space;
}

XWPSColorSpace * XWPSColorSpace::baseSpacePattern()
{
	return (params.pattern->has_base_space ? params.pattern->base_space : NULL);
}

XWPSCie * XWPSColorSpace::cieCSCommonABC(XWPSCieABC **ppabc)
{
	XWPSColorSpace * pcs = this;
  *ppabc = 0;
  do 
  {
    switch (pcs->type->index) 
    {
			case ps_color_space_index_CIEDEF:
	   		*ppabc = pcs->params.def;
	   		return pcs->params.def;
	    		
			case ps_color_space_index_CIEDEFG:
	   		*ppabc = pcs->params.defg;
	   		return pcs->params.defg;
	    		
			case ps_color_space_index_CIEABC:
	   		*ppabc = pcs->params.abc;
	   		return pcs->params.abc;
	    		
			case ps_color_space_index_CIEA:
	   		return pcs->params.a;
	    		
			default:
        pcs = pcs->baseSpace();
        break;
    }
  } while (pcs != 0);

  return 0;
}

int XWPSColorSpace::cieRemapFinish(PSCieCachedVector3 vec3, 
	                   ushort * pconc,
		                 const XWPSImagerState * pis)
{
	XWPSCieRender *pcrd = (XWPSCieRender*)(pis->cie_render);
  XWPSCieJointCaches *pjc = (XWPSCieJointCaches*)(pis->cie_joint_caches);
  XWPSString *table = pcrd->RenderTable.lookup.table;
  int tabc[3];
  
  if (!pjc->skipDecodeLMN)
		cie_lookup_map3(&vec3, &pjc->DecodeLMN[0], "Decode/MatrixLMN+MatrixPQR");

  
  if (!pjc->skipPQR)
		cie_lookup_map3(&vec3, &pjc->TransformPQR[0],	"Transform/Matrix'PQR+MatrixLMN");

  if (!pjc->skipEncodeLMN)
		cie_lookup_map3(&vec3, &pcrd->caches.EncodeLMN[0],"EncodeLMN+MatrixABC");

#define SET_TABC(i, t)\
  do{\
    tabc[i] = cie_cached2int(vec3.t - pcrd->EncodeABC_base[i],\
			     _cie_interpolate_bits);\
    if ((uint)tabc[i] > (ps_cie_cache_size - 1) << _cie_interpolate_bits)\
	tabc[i] = (tabc[i] < 0 ? 0 :\
		   (ps_cie_cache_size - 1) << _cie_interpolate_bits);\
  }while(0)
    
  SET_TABC(0, u);
  SET_TABC(1, v);
  SET_TABC(2, w);
  
#undef SET_TABC
    
  if (table == 0) 
  {
#define EABC(i)\
  cie_interpolate_fracs(pcrd->caches.EncodeABC[i].fixeds.fracs.values, tabc[i])
	
		pconc[0] = EABC(0);
		pconc[1] = EABC(1);
		pconc[2] = EABC(2);
#undef EABC
		return 3;
  } 
  else 
  {
		int m = pcrd->RenderTable.lookup.m;

#define RT_LOOKUP(j, i) pcrd->caches.RenderTableT[j].fracs.values[i]
#ifdef CIE_RENDER_TABLE_INTERPOLATE

		long rfix[3];

#define EABC(i)\
  cie_interpolate_fracs(pcrd->caches.EncodeABC[i].fixeds.ints.values, tabc[i])
#define FABC(i)\
  (EABC(i) << (_fixed_shift - _cie_interpolate_bits))
  
		rfix[0] = FABC(0);
		rfix[1] = FABC(1);
		rfix[2] = FABC(2);
#undef FABC
#undef EABC

		pcrd->RenderTable.lookup.interpolateLinear(rfix, pconc);
		
		if (!pcrd->caches.RenderTableT_is_identity) 
		{
#define frac2cache_index(v) frac2bits(v, ps_cie_log2_cache_size)
	    pconc[0] = RT_LOOKUP(0, frac2cache_index(pconc[0]));
	    pconc[1] = RT_LOOKUP(1, frac2cache_index(pconc[1]));
	    pconc[2] = RT_LOOKUP(2, frac2cache_index(pconc[2]));
	    if (m > 3)
				pconc[3] = RT_LOOKUP(3, frac2cache_index(pconc[3]));
#undef frac2cache_index
		}

#else /* !CIE_RENDER_TABLE_INTERPOLATE */

#define RI(i)\
  pcrd->caches.EncodeABC[i].ints.values[tabc[i] >> _cie_interpolate_bits]
		
		int ia = RI(0);
		int ib = RI(1);		
		int ic = RI(2);	
		uchar *prtc = table[ia].data + ib + ic;

		if (pcrd->caches.RenderTableT_is_identity) 
		{
	    pconc[0] = byte2frac(prtc[0]);
	    pconc[1] = byte2frac(prtc[1]);
	    pconc[2] = byte2frac(prtc[2]);
	    if (m > 3)
				pconc[3] = byte2frac(prtc[3]);
		} 
		else 
		{
#if ps_cie_log2_cache_size == 8
#  define byte2cache_index(b) (b)
#else
# if ps_cie_log2_cache_size > 8
#  define byte2cache_index(b)\
    ( ((b) << (ps_cie_log2_cache_size - 8)) +\
      ((b) >> (16 - ps_cie_log2_cache_size)) )
# else				/* < 8 */
#  define byte2cache_index(b) ((b) >> (8 - ps_cie_log2_cache_size))
# endif
#endif
	    pconc[0] = RT_LOOKUP(0, byte2cache_index(prtc[0]));
	    pconc[1] = RT_LOOKUP(1, byte2cache_index(prtc[1]));
	    pconc[2] = RT_LOOKUP(2, byte2cache_index(prtc[2]));
	    if (m > 3)
				pconc[3] = RT_LOOKUP(3, byte2cache_index(prtc[3]));
#undef byte2cache_index
		}

#endif /* !CIE_RENDER_TABLE_INTERPOLATE */
#undef RI
#undef RT_LOOKUP
		return m;
  }
}

int XWPSColorSpace::concretizeCIEA(const XWPSClientColor * pc, 
		                 ushort * pconc, 
		                 const XWPSImagerState * pis)
{
	XWPSCieA *pcie = params.a;
  cie_cached_value a = float2cie_cached(pc->paint.values[0]);
  PSCieCachedVector3 vlmn;

  if (pis->cie_render == 0) 
  {
		pconc[0] = pconc[1] = pconc[2] = frac_0;
		return 0;
  }
  
  if (pis->cie_joint_caches->status != CIE_JC_STATUS_COMPLETED) 
  {
		int code = ((XWPSImagerState*)pis)->cieJCComplete(this);

		if (code < 0)
	    return code;
  }

  if (!pis->cie_joint_caches->skipDecodeABC)
		vlmn = LOOKUP_VALUE(a, &pcie->caches.DecodeA);
  else
		vlmn.u = vlmn.v = vlmn.w = a;
  cieRemapFinish(vlmn, pconc, pis);
  return 0;
}

int XWPSColorSpace::concretizeCIEABC(const XWPSClientColor * pc, 
		     							  ushort * pconc, 
		     							  const XWPSImagerState * pis)
{
	XWPSCieABC *pcie = params.abc;
  PSCieCachedVector3 vec3;

  if (pis->cie_render == 0) 
  {
		pconc[0] = pconc[1] = pconc[2] = frac_0;
		return 0;
  }
  
  if (pis->cie_joint_caches->status != CIE_JC_STATUS_COMPLETED) 
  {
		int code = ((XWPSImagerState*)pis)->cieJCComplete(this);

		if (code < 0)
	    return code;
  }

  vec3.u = float2cie_cached(pc->paint.values[0]);
  vec3.v = float2cie_cached(pc->paint.values[1]);
  vec3.w = float2cie_cached(pc->paint.values[2]);
  if (!pis->cie_joint_caches->skipDecodeABC)
		cie_lookup_map3(&vec3 , &pcie->caches.DecodeABC[0],"Decode/MatrixABC");
  cieRemapFinish(vec3, pconc, pis);
  return 0;
}

int XWPSColorSpace::concretizeCIEDEF(const XWPSClientColor * pc, 
		     								ushort * pconc, 
		     								const XWPSImagerState * pis)
{
	XWPSCieDef *pcie = params.def;
  int i;
  long hij[3];
  ushort abc[3];
  PSCieCachedVector3 vec3;

  if (pis->cie_render == 0) 
  {
		pconc[0] = pconc[1] = pconc[2] = frac_0;
		return 0;
  }
  
  if (pis->cie_joint_caches->status != CIE_JC_STATUS_COMPLETED) 
  {
		int code = ((XWPSImagerState*)pis)->cieJCComplete(this);

		if (code < 0)
	    return code;
  }

  for (i = 0; i < 3; ++i) 
  {
		int tdim = pcie->Table.dims[i] - 1;
		double factor = pcie->caches_def.DecodeDEF[i].floats.params.factor;
		double v0 = pc->paint.values[i];
		PSRange * rangeDEF = &pcie->RangeDEF.ranges[i];
		double value = (v0 < rangeDEF->rmin ? 0.0 : v0 > rangeDEF->rmax ? factor : (v0 - rangeDEF->rmin) * factor /
	       	(rangeDEF->rmax - rangeDEF->rmin));
		int vi = (int)value;
		double vf = value - vi;
		double v = pcie->caches_def.DecodeDEF[i].floats.values[vi];

		if (vf != 0 && vi < factor)
	  	v += vf *	(pcie->caches_def.DecodeDEF[i].floats.values[vi + 1] - v);
		v = (v < 0 ? 0 : v > tdim ? tdim : v);
		hij[i] = float2fixed(v);
  }
   
  pcie->Table.interpolateLinear(hij, abc);
  vec3.u = float2cie_cached(frac2float(abc[0]));
  vec3.v = float2cie_cached(frac2float(abc[1]));
  vec3.w = float2cie_cached(frac2float(abc[2]));

  if (!pis->cie_joint_caches->skipDecodeABC)
		cie_lookup_map3(&vec3, &pcie->caches.DecodeABC[0],"Decode/MatrixABC");
  cieRemapFinish(vec3, pconc, pis);
  return 0;
}

int XWPSColorSpace::concretizeCIEDEFG(const XWPSClientColor * pc, 
		      							ushort * pconc, 
		      							const XWPSImagerState * pis)
{
	XWPSCieDefg *pcie = params.defg;
  int i;
  long hijk[4];
  ushort abc[3];
  PSCieCachedVector3 vec3;

  if (pis->cie_render == 0) 
  {
		pconc[0] = pconc[1] = pconc[2] = frac_0;
		return 0;
  }
  
  if (pis->cie_joint_caches->status != CIE_JC_STATUS_COMPLETED) 
  {
		int code = ((XWPSImagerState*)pis)->cieJCComplete(this);

		if (code < 0)
	    return code;
  }

  for (i = 0; i < 4; ++i) 
  {
		int tdim = pcie->Table.dims[i] - 1;
		double factor = pcie->caches_defg.DecodeDEFG[i].floats.params.factor;
		double v0 = pc->paint.values[i];
		PSRange * rangeDEFG = &pcie->RangeDEFG.ranges[i];
		double value = (v0 < rangeDEFG->rmin ? 0.0 : v0 > rangeDEFG->rmax ? factor :
	     			(v0 - rangeDEFG->rmin) * factor / (rangeDEFG->rmax - rangeDEFG->rmin));
		int vi = (int)value;
		double vf = value - vi;
		double v = pcie->caches_defg.DecodeDEFG[i].floats.values[vi];

		if (vf != 0 && vi < factor)
	    v += vf *	(pcie->caches_defg.DecodeDEFG[i].floats.values[vi + 1] - v);
		v = (v < 0 ? 0 : v > tdim ? tdim : v);
		hijk[i] = float2fixed(v);
  }
  
  pcie->Table.interpolateLinear(hijk, abc);
  vec3.u = float2cie_cached(frac2float(abc[0]));
  vec3.v = float2cie_cached(frac2float(abc[1]));
  vec3.w = float2cie_cached(frac2float(abc[2]));
  
  if (!pis->cie_joint_caches->skipDecodeABC)
		cie_lookup_map3(&vec3, &pcie->caches.DecodeABC[0],"Decode/MatrixABC");
  cieRemapFinish(vec3, pconc, pis);
  return 0;
}

int XWPSColorSpace::concretizeColor(const XWPSClientColor * pc,
			                     ushort * pconc, 
			                     const XWPSImagerState *pis)
{
	return (this->*(type->concretize_color))(pc, pconc, pis);
}

int XWPSColorSpace::concretizeDeviceCMYK(const XWPSClientColor * pc,
			                                   ushort * pconc, 
			                                   const XWPSImagerState *)
{
	float ftemp;

  pconc[0] = unit_frac(pc->paint.values[0], ftemp);
  pconc[1] = unit_frac(pc->paint.values[1], ftemp);
  pconc[2] = unit_frac(pc->paint.values[2], ftemp);
  pconc[3] = unit_frac(pc->paint.values[3], ftemp);
  return 0;
}

int XWPSColorSpace::concretizeDeviceGray(const XWPSClientColor * pc,
			                                   ushort * pconc, 
			                                   const XWPSImagerState *)
{
	float ftemp;

  pconc[0] = unit_frac(pc->paint.values[0], ftemp);
  return 0;
}

int XWPSColorSpace::concretizeDeviceN(const XWPSClientColor * pc, 
		                      ushort * pconc, 
		                      const XWPSImagerState * pis)
{
	int code, tcode;
  XWPSClientColor cc;
  XWPSColorSpace *pacs =	params.device_n->alt_space;
  XWPSDeviceNMap *map = params.device_n->map;

  if (map->cache_valid) 
  {
		int i;

		for (i = params.device_n->num_components; --i >= 0;) 
		{
	    if (map->tint[i] != pc->paint.values[i])
				break;
		}
		if (i < 0) 
		{
	    int num_out = pacs->numComponents();

	    for (i = 0; i < num_out; ++i)
				pconc[i] = map->conc[i];
	    return 0;
		}
  }
  
  tcode = params.device_n->map->tintTransform(params.device_n, 
                      pc->paint.values, &cc.paint.values[0], pis, params.device_n->map->tint_transform_data);
  if (tcode < 0)
		return tcode;
  code = pacs->concretizeColor(&cc , pconc, pis);
  return (code < 0 || tcode == 0 ? code : tcode);
}

int XWPSColorSpace::concretizeDevicePixel(const XWPSClientColor * pc,
			                     ushort * pconc, 
			                     const XWPSImagerState *)
{
	 pconc[0] = (uchar)(pc->paint.values[0]);
   return 0;
}

int XWPSColorSpace::concretizeDeviceRGB(const XWPSClientColor * pc,
			                                  ushort * pconc, 
			                                  const XWPSImagerState *)
{
	float ftemp;

  pconc[0] = unit_frac(pc->paint.values[0], ftemp);
  pconc[1] = unit_frac(pc->paint.values[1], ftemp);
  pconc[2] = unit_frac(pc->paint.values[2], ftemp);
  return 0;
}

int XWPSColorSpace::concretizeIndexed(const XWPSClientColor * pc, 
			                    ushort * pconc, 
			                    const XWPSImagerState * pis)
{
	float value = pc->paint.values[0];
  int index =	(is_fneg(value) ? 0 : value >= params.indexed->hival ? params.indexed->hival : (int)value);
  XWPSColorSpace *pbcs = params.indexed->base_space;
  XWPSClientColor cc;
  int code = params.indexed->indexedLookup(index, &cc);

  if (code < 0)
		return code;
  return pbcs->concretizeColor(&cc, pconc, pis);
}

int XWPSColorSpace::concretizeNo(const XWPSClientColor * , 
			                    ushort * , 
			                    const XWPSImagerState * )
{
	return (int)(XWPSError::RangeCheck);
}

int XWPSColorSpace::concretizeSeparation(const XWPSClientColor *pc, 
			                     ushort *pconc, 
			                     const XWPSImagerState *pis)
{
	float tint = pc->paint.values[0];
  int code;
  XWPSClientColor cc;
  XWPSColorSpace *pacs = params.separation->alt_space;

  if (tint < 0)
		tint = 0;
  else if (tint > 1)
		tint = 1;
		
  code = params.separation->map->tintTransform(params.separation, tint, &cc.paint.values[0]);
  if (code < 0)
		return code;
  return pacs->concretizeColor(&cc, pconc, pis);
}

XWPSColorSpace * XWPSColorSpace::concreteSpace(const XWPSImagerState*pis)
{
	return (this->*(type->concrete_space))(pis);
}

XWPSColorSpace * XWPSColorSpace::concreteSpaceDeviceN(const XWPSImagerState * pis)
{
	XWPSColorSpace *pacs = params.device_n->alt_space;

  return pacs->concreteSpace(pis);
}

XWPSColorSpace * XWPSColorSpace::concreteSpaceCIE(const XWPSImagerState * pis)
{
	const XWPSCieRender *pcie = pis->cie_render;

  if (pcie == 0 || pcie->RenderTable.lookup.table == 0 ||
			pcie->RenderTable.lookup.m == 3)
		return ((XWPSImagerState*)pis)->deviceRGB();
  else	
		return ((XWPSImagerState*)pis)->deviceCMYK();
}

XWPSColorSpace * XWPSColorSpace::concreteSpaceIndexed(const XWPSImagerState * pis)
{
  return params.indexed->base_space->concreteSpace(pis);
}

XWPSColorSpace * XWPSColorSpace::concreteSpaceSeparation(const XWPSImagerState * pis)
{
	return params.separation->alt_space->concreteSpace(pis);
}

void XWPSColorSpace::copy(XWPSColorSpace * src)
{
	type = src->type;
	id = src->id;
	i_ctx_p = src->i_ctx_p;
	switch (getIndex())
	{
		case ps_color_space_index_DevicePixel:
			if (src->params.pixel != params.pixel)
			{
				if (params.pixel)
					if (!params.pixel->decRef())
						delete params.pixel;
				params.pixel = src->params.pixel;
				if (params.pixel)
					params.defg->incRef();
			}
			break;
			
		case ps_color_space_index_CIEDEFG:
			if (src->params.defg != params.defg)
			{
				if (params.defg)
					if (!params.defg->decRef())
						delete params.defg;
				params.defg = src->params.defg;
				if (params.defg)
					params.defg->incRef();
			}
			break;
			
		case ps_color_space_index_CIEDEF:
			if (src->params.def != params.def)
			{
				if (params.def)
					if (!params.def->decRef())
						delete params.def;
				params.def = src->params.def;
				if (params.def)
					params.def->incRef();
			}
			break;
			
		case ps_color_space_index_CIEABC:
			if (src->params.abc != params.abc)
			{
				if (params.abc)
					if (!params.abc->decRef())
						delete params.abc;
				if (params.abc)
					params.abc->incRef();
			}
			break;
			
		case ps_color_space_index_CIEA:
			if (src->params.a != params.a)
			{
				if (params.a)
					if (!params.a->decRef())
						delete params.a;
				params.a = src->params.a;
				if (params.a)
					params.a->incRef();
			}
			break;
			
		case ps_color_space_index_Separation:
			if (src->params.separation != params.separation)
			{
				if (params.separation)
					if (!params.separation->decRef())
						delete params.separation;
				params.separation = src->params.separation;
				if (params.separation)
					params.separation->incRef();
			}
			break;
			
		case ps_color_space_index_DeviceN:
			if (src->params.device_n != params.device_n)
			{
				if (params.device_n)
					if (!params.device_n->decRef())
						delete params.device_n;
				params.device_n = src->params.device_n;
				if (params.device_n)
					params.device_n->incRef();
			}
			break;
			
		case ps_color_space_index_Indexed:
			if (src->params.indexed != params.indexed)
			{
				if (params.indexed)
					if (!params.indexed->decRef())
						delete params.indexed;
				params.indexed = src->params.indexed;
				if (params.indexed)
					params.indexed->incRef();
			}
			break;
			
		case ps_color_space_index_Pattern:
			if (src->params.pattern != params.pattern)
			{
				if (params.pattern)
					if (!params.pattern->decRef())
						delete params.pattern;
				params.pattern = src->params.pattern;
				if (params.pattern)
					params.pattern->incRef();
			}
			break;
			
		default:
			break;
	}
}

bool XWPSColorSpace::equal(XWPSColorSpace *pcs2)
{
	return ((id == pcs2->id && id != ps_no_id) ||  (type == type && (this->*type->equal)(pcs2)));
}

bool XWPSColorSpace::equalDeviceN(XWPSColorSpace *pcs2)
{
	return (altSpaceDeviceN()->equal(pcs2->altSpaceDeviceN()) &&
	    		 params.device_n->num_components ==  pcs2->params.device_n->num_components &&
	    		!memcmp(params.device_n->names, pcs2->params.device_n->names, 
	    		         params.device_n->num_components *  sizeof(params.device_n->names[0])) &&
	    		params.device_n->map->tint_transform == pcs2->params.device_n->map->tint_transform &&
	    		params.device_n->map->tint_transform_data == pcs2->params.device_n->map->tint_transform_data);
}

bool XWPSColorSpace::equalDevicePixel(XWPSColorSpace *pcs2)
{
	return params.pixel->depth == pcs2->params.pixel->depth;
}

bool XWPSColorSpace::equalIndexed(XWPSColorSpace *pcs2)
{
	XWPSColorSpace *base = baseSpace();
  uint hival = params.indexed->hival;

  if (!base->equal(pcs2->baseSpace()))
		return false;
  if (hival == pcs2->params.indexed->hival ||
			params.indexed->use_proc != pcs2->params.indexed->use_proc)
		return false;
    
  if (params.indexed->use_proc) 
  {
		return !memcmp(params.indexed->lookup.map->values,	pcs2->params.indexed->lookup.map->values,
		       					params.indexed->lookup.map->num_values * sizeof(params.indexed->lookup.map->values[0]));
  } 
  else 
  {
		return !memcmp(params.indexed->lookup.table->data,  &pcs2->params.indexed->lookup.table->data,
		       					base->numComponents() * (hival + 1));
  }
}

bool XWPSColorSpace::equalSeparation(XWPSColorSpace *pcs2)
{
	return (altSpaceSeparation()->equal(pcs2->altSpaceSeparation()) &&
	    			params.separation->sname == pcs2->params.separation->sname &&
	    		((params.separation->map->proc.tint_transform == pcs2->params.separation->map->proc.tint_transform &&
	      		params.separation->map->proc_data ==  pcs2->params.separation->map->proc_data) ||
	     			!memcmp(params.separation->map->values, pcs2->params.separation->map->values, 
	     			      params.separation->map->num_values * sizeof(params.separation->map->values[0]))));
}

XWPSFunction * XWPSColorSpace::getDevNFunction()
{
	if (getIndex() == ps_color_space_index_DeviceN && 
		  params.device_n->map->tint_transform ==  &XWPSDeviceNParams::usingFunction)
		return (XWPSFunction*)(params.device_n->map->tint_transform_data);
  return 0;
}

PSColorSpaceIndex XWPSColorSpace::getIndex()
{
	return type->index;
}

XWPSFunction * XWPSColorSpace::getSeprFunction()
{
	if (getIndex() == ps_color_space_index_Separation &&
			params.separation->map->proc.tint_transform == &XWPSSeparationParams::usingFunction)
		return (XWPSFunction*)(params.separation->map->proc_data);
  return 0;
}

uint XWPSColorSpace::indexedTableSize()
{
	return (params.indexed->hival + 1) *	params.indexed->base_space->numComponents();
}

void XWPSColorSpace::init(PSColorSpaceType * pcstype, void *client_dataA)
{
	type = pcstype;
	switch (getIndex())
	{
		case ps_color_space_index_DevicePixel:
			params.pixel = new XWPSDevicePixelParams;
			break;
			
		case ps_color_space_index_CIEDEFG:
			params.defg = new XWPSCieDefg(client_dataA);
			break;
			
		case ps_color_space_index_CIEDEF:
			params.def = new XWPSCieDef(client_dataA);
			break;
			
		case ps_color_space_index_CIEABC:
			params.abc = new XWPSCieABC(client_dataA);
			break;
			
		case ps_color_space_index_CIEA:
			params.a = new XWPSCieA(client_dataA);
			break;
			
		case ps_color_space_index_Separation:
			params.separation = new XWPSSeparationParams;
			break;
			
		case ps_color_space_index_DeviceN:
			params.device_n = new XWPSDeviceNParams;
			break;
			
		case ps_color_space_index_Indexed:
			params.indexed = new XWPSIndexedParams;
			break;
			
		case ps_color_space_index_Pattern:
			params.pattern = new XWPSPatternParams;
			break;
			
		default:
			break;
	}
	
	if (i_ctx_p)
  	id = i_ctx_p->nextIDS(1);
}

void XWPSColorSpace::initCIE(XWPSClientColor * pcc)
{
	initPaint4(pcc);
  restrictColor(pcc);
}

void XWPSColorSpace::initColor(XWPSClientColor * pcc)
{
	return (this->*(type->init_color))(pcc);
}

int  XWPSColorSpace::initDevice(int num_components)
{
	switch (num_components) 
	{
    case 1: 
    	initDeviceGray(); 
    	break;
    	
    case 3: 
    	initDeviceRGB(); 
    	break;
    	
    case 4: 
    	initDeviceCMYK(); 
    	break;
    	
    default: 
    	return (int)(XWPSError::RangeCheck);
  }
  return 0;
}

void XWPSColorSpace::initDeviceCMYK()
{
	init(&ps_color_space_type_DeviceCMYK);
}

void XWPSColorSpace::initDeviceGray()
{
	init(&ps_color_space_type_DeviceGray);
}

void XWPSColorSpace::initDeviceN(XWPSClientColor * pcc)
{
	int i;

  for (i = 0; i < params.device_n->num_components; ++i)
		pcc->paint.values[i] = 1.0;
}

void XWPSColorSpace::initDevicePixel(int depth)
{
	switch (depth) 
	{
		case 1:
		case 2:
		case 4:
		case 8:
		case 16:
		case 24:
		case 32:
	    break;
	    
		default:
	    return ;
  }
  init(&ps_color_space_type_DevicePixel);
  params.pixel->depth = depth;
  return ;
}

void XWPSColorSpace::initDeviceRGB()
{
	init(&ps_color_space_type_DeviceRGB);
}

void XWPSColorSpace::initPattern(XWPSClientColor * pcc)
{
	if (params.pattern->has_base_space) 
	{
		XWPSColorSpace *pbcs = params.pattern->base_space;

		pbcs->initColor(pcc);
  }
}

void XWPSColorSpace::initSeparation(XWPSClientColor * pcc)
{
	pcc->paint.values[0] = 1.0;
}

void XWPSColorSpace::initFrom(XWPSColorSpace * pcsfrom)
{
	copy(pcsfrom);
}

void XWPSColorSpace::initPaint1(XWPSClientColor * pcc)
{
	pcc->paint.values[0] = 0.0;
}

void XWPSColorSpace::initPaint3(XWPSClientColor * pcc)
{
	pcc->paint.values[2] = 0.0;
  pcc->paint.values[1] = 0.0;
  pcc->paint.values[0] = 0.0;
}

void XWPSColorSpace::initPaint4(XWPSClientColor * pcc)
{
	pcc->paint.values[3] = 1.0;
  initPaint3(pcc);
}

int XWPSColorSpace::installCIEA(XWPSState * pgs)
{
	XWPSCieA *pcie = params.a;
  int i;
  PSForLoopParams lp;
  float in;

  ps_cie_cache_init(&pcie->caches.DecodeA.floats.params, &lp, &pcie->RangeA);
  for (i = 0, in = lp.init; i < ps_cie_cache_size; in += lp.step, i++) 
		pcie->caches.DecodeA.floats.values[i] = (pcie->*(pcie->DecodeA))(in);
  pgs->cieLoadCommonCache(pcie);
  pcie->acomplete();
  return pgs->cieCSComplete(true);
}

int XWPSColorSpace::installCIEABC(XWPSState * pgs)
{
	return pgs->installCieABC(params.abc);
}

int XWPSColorSpace::installCIEDEF(XWPSState * pgs)
{
	XWPSCieDef *pcie = params.def;
  
  int j;

	for (j = 0; j < (sizeof(pcie->caches_def.DecodeDEF) / sizeof(pcie->caches_def.DecodeDEF[0])); j++) 
	{
	    PSCieCacheFloats *pcf = &(pcie->caches_def.DecodeDEF)[j].floats;
	    int i;
	    PSForLoopParams lp;

	    ps_cie_cache_init(&pcf->params, &lp, &pcie->RangeDEF.ranges[j]);
	    for (i = 0; i < ps_cie_cache_size; lp.init += lp.step, i++) 
				pcf->values[i] = (pcie->*(pcie->DecodeDEF.procs[j]))(lp.init);
	    pcf->params.is_identity =	(pcie->DecodeDEF.procs[j] == DecodeDEF_default.procs[j]);
	}
	
  return pgs->installCieABC(pcie);
}

int XWPSColorSpace::installCIEDEFG(XWPSState * pgs)
{
	XWPSCieDefg *pcie = params.defg;
  
	int j;

	for (j = 0; j < (sizeof(pcie->caches_defg.DecodeDEFG) / sizeof(pcie->caches_defg.DecodeDEFG[0])); j++) 
	{
	    PSCieCacheFloats *pcf = &pcie->caches_defg.DecodeDEFG[j].floats;
	    int i;
	    PSForLoopParams lp;

	    ps_cie_cache_init(&pcf->params, &lp, &pcie->RangeDEFG.ranges[j]);
	    for (i = 0; i < ps_cie_cache_size; lp.init += lp.step, i++) 
				pcf->values[i] = (pcie->*(pcie->DecodeDEFG.procs[j]))(lp.init);
	    pcf->params.is_identity =	(pcie->DecodeDEFG.procs[j] == DecodeDEFG_default.procs[j]);
	}
	
  return pgs->installCieABC(pcie);
}

int  XWPSColorSpace::installCIE(XWPSState * pgs)
{
	return params.a->installCSpace(this, pgs);
}

int  XWPSColorSpace::installCSpace(XWPSState*pgs)
{
	return (this->*(type->install_cspace))(pgs);
}

int XWPSColorSpace::installDeviceN(XWPSState * pgs)
{
	const ulong *names = params.device_n->names;
  uint i, j;

  for (i = 1; i < params.device_n->num_components; ++i)
		for (j = 0; j < i; ++j)
	    if (names[i] == names[j])
				return (int)(XWPSError::RangeCheck);
  return params.device_n->alt_space->installCSpace(pgs);
}

int XWPSColorSpace::installIndexed(XWPSState * pgs)
{
	return baseSpace()->installCSpace(pgs);
}

int XWPSColorSpace::installPattern(XWPSState * pgs)
{
	if (!params.pattern->has_base_space)
		return 0;
  return params.pattern->base_space->installCSpace(pgs);
}

int XWPSColorSpace::installSeparation(XWPSState * pgs)
{
	return params.separation->alt_space->installCSpace(pgs);
}

int XWPSColorSpace::numComponents()
{
	return (this->*(type->num_components))();
}

int XWPSColorSpace::numComponentsDeviceN()
{
	return params.device_n->num_components;
}

int XWPSColorSpace::numComponentsPattern()
{
	return	(params.pattern->has_base_space ? -1 - params.pattern->base_space->numComponents() :	 -1);
}

int XWPSColorSpace::remapColor(XWPSImagerState * pis,
	                XWPSClientColor * pcc, 
	                XWPSColorSpace * pcs,
	                XWPSDeviceColor * pdc, 
	                XWPSDevice * dev,
		              PSColorSelect select)
{
	return (pis->*(type->remap_color))(pcc, pcs, pdc, dev, select);
}

int  XWPSColorSpace::remapConcreteColor(XWPSImagerState * pis, 
	                        const ushort * pconc,
	                       XWPSDeviceColor * pdc, 
	                       XWPSDevice * dev,
			                   PSColorSelect select)
{
	return (pis->*(type->remap_concrete_color))(pconc, pdc, dev, select);
}

void XWPSColorSpace::restrictCIEA(XWPSClientColor * pcc)
{
	const XWPSCieA *pcie = params.a;

  cie_restrict(&pcc->paint.values[0], &pcie->RangeA);
}

void XWPSColorSpace::restrictCIEABC(XWPSClientColor * pcc)
{
	const XWPSCieABC *pcie = params.abc;

  cie_restrict(&pcc->paint.values[0], &pcie->RangeABC.ranges[0]);
  cie_restrict(&pcc->paint.values[1], &pcie->RangeABC.ranges[1]);
  cie_restrict(&pcc->paint.values[2], &pcie->RangeABC.ranges[2]);
}

void XWPSColorSpace::restrictCIEDEF(XWPSClientColor * pcc)
{
	const XWPSCieDef *pcie = params.def;

  cie_restrict(&pcc->paint.values[0], &pcie->RangeDEF.ranges[0]);
  cie_restrict(&pcc->paint.values[1], &pcie->RangeDEF.ranges[1]);
  cie_restrict(&pcc->paint.values[2], &pcie->RangeDEF.ranges[2]);
}

void XWPSColorSpace::restrictCIEDEFG(XWPSClientColor * pcc)
{
	const XWPSCieDefg *pcie = params.defg;

  cie_restrict(&pcc->paint.values[0], &pcie->RangeDEFG.ranges[0]);
  cie_restrict(&pcc->paint.values[1], &pcie->RangeDEFG.ranges[1]);
  cie_restrict(&pcc->paint.values[2], &pcie->RangeDEFG.ranges[2]);
  cie_restrict(&pcc->paint.values[3], &pcie->RangeDEFG.ranges[3]);
}

void XWPSColorSpace::restrictColor(XWPSClientColor * pcc)
{
	(this->*(type->restrict_color))(pcc);
}

void XWPSColorSpace::restrictDeviceN(XWPSClientColor * pcc)
{
	int i;

  for (i = 0; i < params.device_n->num_components; ++i) 
  {
		float value = pcc->paint.values[i];

		pcc->paint.values[i] = (value <= 0 ? 0 : value >= 1 ? 1 : value);
  }
}

void XWPSColorSpace::restrictDevicePixel(XWPSClientColor * pcc)
{
	float pixel = pcc->paint.values[0];
  ulong max_value = (1L << params.pixel->depth) - 1;
  pcc->paint.values[0] = (pixel < 0 ? 0 : qMin(pixel, (float)max_value));
}

void XWPSColorSpace::restrictIndexed(XWPSClientColor * pcc)
{
	float value = pcc->paint.values[0];

  pcc->paint.values[0] =(is_fneg(value) ? 0 : value >= params.indexed->hival ? params.indexed->hival : value);
}

void XWPSColorSpace::restrict01Paint1(XWPSClientColor * pcc)
{
	pcc->paint.values[0] = FORCE_UNIT(pcc->paint.values[0]);
}

void XWPSColorSpace::restrict01Paint3(XWPSClientColor * pcc)
{
	pcc->paint.values[2] = FORCE_UNIT(pcc->paint.values[2]);
  pcc->paint.values[1] = FORCE_UNIT(pcc->paint.values[1]);
  pcc->paint.values[0] = FORCE_UNIT(pcc->paint.values[0]);
}

void XWPSColorSpace::restrict01Paint4(XWPSClientColor * pcc)
{
	pcc->paint.values[3] = FORCE_UNIT(pcc->paint.values[3]);
  restrict01Paint3(pcc);
}

void XWPSColorSpace::restrictPattern(XWPSClientColor * pcc)
{
	if (pcc->pattern &&	pcc->getPattern()->usesBaseSpace() &&	params.pattern->has_base_space) 
	{
		XWPSColorSpace *pbcs = params.pattern->base_space;

		pbcs->restrictColor(pcc);
  }
}

int XWPSColorSpace::setDevNFunction(XWPSFunction *pfn)
{
	if (getIndex() != ps_color_space_index_DeviceN ||
			pfn->params->m != params.device_n->num_components ||
			pfn->params->n !=  params.device_n->alt_space->numComponents())
	{
	 	return (int)(XWPSError::RangeCheck);
	}
    
  XWPSDeviceNMap * pimap = params.device_n->map;
  pimap->tint_transform = &XWPSDeviceNParams::mapDevNUsingFunction;
  pimap->tint_transform_data = pfn;
  pimap->cache_valid = false;
  pimap->tint_transform_data_pfn = true;
  return 0;
}

int XWPSColorSpace::setSeprFunction(XWPSFunction *pfn)
{
	XWPSIndexedMap *pimap;

  if (getIndex() != ps_color_space_index_Separation ||
				pfn->params->m != 1 ||
				pfn->params->n != params.separation->alt_space->numComponents())
	{
	 	return (int)(XWPSError::RangeCheck);
	}
  pimap = params.separation->map;
  pimap->proc.tint_transform = &XWPSSeparationParams::mapSeprUsingFunction;
  pimap->proc_data = pfn;
  return 0;
}

void XWPSColorSpace::freeParams()
{
	switch (getIndex())
	{
		case ps_color_space_index_DevicePixel:
			if (params.pixel)
			{
				if (!params.pixel->decRef())
					delete params.pixel;
				params.pixel = 0;
			}
			break;
			
		case ps_color_space_index_CIEDEFG:
			if (params.defg)
			{
				if (!params.defg->decRef())
					delete params.defg;
				params.defg = 0;
			}
			break;
			
		case ps_color_space_index_CIEDEF:
			if (params.def)
			{
				if (!params.def->decRef())
					delete params.def;
				params.def = 0;
			}
			break;
			
		case ps_color_space_index_CIEABC:
			if (params.abc)
			{
				if (!params.abc->decRef())
					delete params.abc;
				params.abc = 0;
			}
			break;
			
		case ps_color_space_index_CIEA:
			if (params.a)
			{
				if (!params.a->decRef())
					delete params.a;
				params.a = 0;
			}
			break;
			
		case ps_color_space_index_Separation:
			if (params.separation)
			{
				if (!params.separation->decRef())
					delete params.separation;
				params.separation = 0;
			}
			break;
			
		case ps_color_space_index_DeviceN:
			if (params.device_n)
			{
				if (!params.device_n->decRef())
					delete params.device_n;
				params.device_n = 0;
			}
			break;
			
		case ps_color_space_index_Indexed:
			if (params.indexed)
			{
				if (!params.indexed->decRef())
					delete params.indexed;
				params.indexed = 0;
			}
			break;
			
		case ps_color_space_index_Pattern:
			if (params.pattern)
			{
				if (!params.pattern->decRef())
					delete params.pattern;
				params.pattern = 0;
			}
			break;
			
		default:
			break;
	}
}

XWPSIndexedMap::XWPSIndexedMap()
	:XWPSStruct(),
	 proc_data(0),
	 num_values(0),
	 values(0)
{
}

XWPSIndexedMap::XWPSIndexedMap(int nvals)
	:XWPSStruct(),
	 proc_data(0),
	 num_values(0),
	 values(0)
{
	num_values = nvals;
	if (num_values > 0)
		values = new float[nvals];
}

XWPSIndexedMap::XWPSIndexedMap(XWPSColorSpace * pbase_cspace, int nvals)
	:XWPSStruct(),
	 proc_data(0),
	 num_values(0),
	 values(0)
{
	num_values = nvals;
	if (num_values > 0)
		values = new float[nvals];
	int num_comps = pbase_cspace->numComponents();
	if (num_comps == 1)
		proc.lookup_index = &XWPSIndexedParams::mapPaletteEntry1;
	else if (num_comps == 3)
		proc.lookup_index = &XWPSIndexedParams::mapPaletteEntry3;
  else if (num_comps == 4)
		proc.lookup_index = &XWPSIndexedParams::mapPaletteEntry4;
  else
		proc.lookup_index = &XWPSIndexedParams::mapPaletteEntryN;
}

XWPSIndexedMap::~XWPSIndexedMap()
{
	if (values)
	{
		delete [] values;
		values = 0;
	}
	
	if (proc_data)
	{
		XWPSFunction * pfn = (XWPSFunction*)(proc_data);
		if (pfn->decRef() == 0)
			delete pfn;
		proc_data = 0;
	}
}

int XWPSIndexedMap::getLength()
{
	return sizeof(XWPSIndexedMap);
}

const char * XWPSIndexedMap::getTypeName()
{
	return "indexedmap";
}

int XWPSIndexedMap::lookupIndex(XWPSIndexedParams *params, int index, float * valuesA)
{
	return (params->*(proc.lookup_index))(index, valuesA);
}

int XWPSIndexedMap::tintTransform(XWPSSeparationParams * params, float tint, float *valuesA)
{
	return (params->*(proc.tint_transform))(tint, valuesA);
}

XWPSDevicePixelParams::XWPSDevicePixelParams()
	:XWPSStruct()
{
	depth = 0;
}

XWPSSeparationParams::XWPSSeparationParams()
	:XWPSStruct(),
	 sname(0),
	 alt_space(0),
	 map(0)
{
}

XWPSSeparationParams::~XWPSSeparationParams()
{
	if (alt_space)
	{
		if (!alt_space->decRef())
			delete alt_space;
		alt_space = 0;
	}
	
	if (map)
	{
		if (!map->decRef())
			delete map;
		map = 0;
	}
}

int XWPSSeparationParams::lookupTint(float tint, float *values)
{
	int m = alt_space->numComponents();
  int value_index =	(tint < 0 ? 0 : tint > 1 ? map->num_values - m : (int)(tint * SEPARATION_CACHE_SIZE + 0.5) * m);
  const float *pv = &map->values[value_index];

  memcpy(values, pv, sizeof(*values) * m);
  return 0;
}

int XWPSSeparationParams::mapSeprUsingFunction(float in_val, float *out_vals)
{
	float in = in_val;
  XWPSFunction * pfn = (XWPSFunction*)(map->proc_data);

  return pfn->evaluate(&in, out_vals);
}

int XWPSSeparationParams::mapTintValue(float in_val, float *out_vals)
{
	int ncomps =   alt_space->numComponents();
  int nentries = map->num_values / ncomps;
  int indx;
  const float *pv = map->values;
  int i;

  if (nentries == 0) 
  {
		for (i = 0; i < ncomps; i++)
	    out_vals[i] = 0.0;
		return 0;
  }
  if (in_val > 1)
		indx = nentries - 1;
  else if (in_val <= 0)
		indx = 0;
  else
		indx = (int)(in_val * nentries + 0.5);
    
  pv += indx * ncomps;

  for (i = 0; i < ncomps; i++)
		out_vals[i] = pv[i];
  return 0;
}

int XWPSSeparationParams::usingFunction(float in_val, float *out_vals)
{
	float in = in_val;
  XWPSFunction *const pfn = (XWPSFunction*)(map->proc_data);

  return pfn->evaluate(&in, out_vals);
}

XWPSDeviceNMap::XWPSDeviceNMap()
	:XWPSStruct(),
	 tint_transform(0),
	 tint_transform_data(0),
	 cache_valid(false),
	 tint_transform_data_pfn(false)
{
}

XWPSDeviceNMap::~XWPSDeviceNMap()
{
	if (tint_transform_data_pfn && tint_transform_data)
	{
		XWPSFunction * pfn = (XWPSFunction*)(tint_transform_data);
		if (pfn->decRef() == 0)
			delete pfn;
		tint_transform_data = 0;
		tint_transform_data_pfn = false;
	}
}

int XWPSDeviceNMap::getLength()
{
	return sizeof(XWPSDeviceNMap);
}

const char * XWPSDeviceNMap::getTypeName()
{
	return "devicemap";
}

int XWPSDeviceNMap::tintTransform(XWPSDeviceNParams * params,
	                  const float *in, 
	                  float *out,
	                  const XWPSImagerState *pis, 
	                  void *data)
{
	return (params->*tint_transform)(in, out, pis, data);
}

XWPSDeviceNParams::XWPSDeviceNParams()
	:XWPSStruct(),
		names(0),
	 num_components(0),
	 alt_space(0)
{
	map = new XWPSDeviceNMap;
}

XWPSDeviceNParams::~XWPSDeviceNParams()
{
	if (names)
	{
		delete [] names;
		names = 0;
	}
	if (alt_space)
	{
		if (!alt_space->decRef())
			delete alt_space;
		alt_space = 0;
	}
	
	if (map)
	{
		if (!map->decRef())
			delete map;
		map = 0;
	}
}

int XWPSDeviceNParams::deviceNRemapTransform(const float *in,
			                       float *out, 
			                       const XWPSImagerState *, 
			                       void *data)
{
	XWPSContextState *i_ctx_p = (XWPSContextState*)data;
  int num_in = num_components;
  int num_out =	alt_space->numComponents();
  int i;

  int code = i_ctx_p->checkEStack(num_in + 4);
  if (code < 0)
  	return code;
  	
  code = i_ctx_p->checkOStack(num_in);
  if (code < 0)
  	return code;
  	
  XWPSOpStack * o_stack = i_ctx_p->getOpStack();
  XWPSExecStack * e_stack = i_ctx_p->getExecStack();
  XWPSRef * osp = o_stack->getCurrentTop();
  XWPSRef * esp = e_stack->getCurrentTop();
  for (i = 0; i < num_in; ++i) 
  {
		osp = o_stack->incCurrentTop(1);
		osp->makeReal(in[i]);
		esp = e_stack->incCurrentTop(1);
		esp->assign(osp);
  }
  esp = e_stack->incCurrentTop(1);
  esp->makeInt(num_in);
  esp = e_stack->incCurrentTop(1);
  esp->makeMarkEStack(PS_ES_OTHER, &XWPSContextState::deviceNRemapCleanup);  
  esp = e_stack->incCurrentTop(1);
  esp->makeOper(0, &XWPSContextState::deviceNRemapFinish);
  
  esp = e_stack->incCurrentTop(1);
  XWPSIntGState * iigs = (XWPSIntGState*)(i_ctx_p->getState()->client_data);
  esp->assign(&iigs->colorspace.procs.device_n.tint_transform);
  for (i = 0; i < num_out; ++i)
		out[i] = 0;

  map->tint_transform = &XWPSDeviceNParams::ztransformDeviceN;
  map->tint_transform_data = 0;
  return PS_O_PUSH_ESTACK;
}

int XWPSDeviceNParams::mapDevNUsingFunction(const float *in, 
	                         float *out,
			                     const XWPSImagerState *, 
			                     void *data)
{
	XWPSFunction *const pfn = (XWPSFunction*)data;

  return pfn->evaluate(in, out);
}

int XWPSDeviceNParams::usingFunction(const float *in, 
	                  float *out,
			              const XWPSImagerState *, 
			              void *data)
{
	XWPSFunction *const pfn = (XWPSFunction*)data;

  return pfn->evaluate(in, out);
}

int XWPSDeviceNParams::ztransformDeviceN(const float *in,
		                    float *, 
		                    const XWPSImagerState *pis, 
		                    void *)
{
	XWPSIntGState * iigs = (XWPSIntGState*)pis->client_data;
	XWPSIntRemapColorInfo * prci = (XWPSIntRemapColorInfo*)(iigs->remap_color_info.getStruct());
	prci->proc = &XWPSContextState::deviceNRemapPrepare;
  memcpy(prci->tint, in, num_components * sizeof(float));
  return (int)(XWPSError::RemapColor);
}

XWPSIndexedParams::XWPSIndexedParams()
	:XWPSStruct()
{
	base_space = 0;
	hival = 0;
	lookup.table = 0;
	use_proc = false;
}

XWPSIndexedParams::~XWPSIndexedParams()
{
	if (use_proc)
	{
		if (lookup.map)
		{
			if (!lookup.map->decRef())
				delete lookup.map;
			lookup.map = 0;
		}
	}
	else
	{
		if (lookup.table)
		{
			delete lookup.table;
			lookup.table = 0;
		}
	}
	if (base_space)
	{
		if (!base_space->decRef())
			delete base_space;
		base_space = 0;
	}
}

int XWPSIndexedParams::indexedLookup(int index,	 XWPSClientColor *pcc)
{
	if (use_proc) 
		return lookup.map->lookupIndex(this, index, &pcc->paint.values[0]);
  else 
  {
		int m = base_space->numComponents();
		uchar *pcomp = lookup.table->data + m * index;
		switch (m) 
		{
	    default: 
	    	{
					for (int i = 0; i < m; ++i)
		    		pcc->paint.values[i] = pcomp[i] * (1.0 / 255.0);
	    	}
				break;
				
	    case 4:
				pcc->paint.values[3] = pcomp[3] * (1.0 / 255.0);
	    case 3:
				pcc->paint.values[2] = pcomp[2] * (1.0 / 255.0);
	    case 2:
				pcc->paint.values[1] = pcomp[1] * (1.0 / 255.0);
	    case 1:
				pcc->paint.values[0] = pcomp[0] * (1.0 / 255.0);
		}
		return 0;
  }
}

int XWPSIndexedParams::lookupIndexedMap(int index, float *values)
{
	int m = base_space->numComponents();
  float *pv = &lookup.map->values[index * m];

  memcpy(values, pv, sizeof(*values) * m);
  return 0;
}

int XWPSIndexedParams::mapPaletteEntry1(int indx, float *values)
{
	values[0] = lookup.map->values[indx];
  return 0;
}

int XWPSIndexedParams::mapPaletteEntry3(int indx, float *values)
{
	const float *pv = &(lookup.map->values[3 * indx]);

  values[0] = pv[0];
  values[1] = pv[1];
  values[2] = pv[2];
  return 0;
}

int XWPSIndexedParams::mapPaletteEntry4(int indx, float *values)
{
	const float *pv = &(lookup.map->values[4 * indx]);

  values[0] = pv[0];
  values[1] = pv[1];
  values[2] = pv[2];
  values[3] = pv[3];
  return 0;
}

int XWPSIndexedParams::mapPaletteEntryN(int indx, float *values)
{
	int m = base_space->numComponents();

  memcpy((void *)values,  (const void *)(lookup.map->values + indx * m),   m * sizeof(float));

  return 0;
}

XWPSPatternParams::XWPSPatternParams()
	:XWPSStruct(),
	 has_base_space(false),
	 base_space(0)
{
}

XWPSPatternParams::~XWPSPatternParams()
{
	if (base_space)
	{
		if (!base_space->decRef())
			delete base_space;
		base_space = 0;
	}
}

XWPSDeviceColorSpace::XWPSDeviceColorSpace()
{
	named.Gray = 0;
	named.RGB = 0;
	named.CMYK = 0;
	indexed[0] = 0;
	indexed[1] = 0;
	indexed[2] = 0;
}

XWPSDeviceColorSpace::~XWPSDeviceColorSpace()
{
	free();
	if (named.Gray)
	{
		if (named.Gray->decRef() == 0)
			delete named.Gray;
		named.Gray = 0;
	}
	
	if (named.RGB)
	{
		if (named.RGB->decRef() == 0)
			delete named.RGB;
		named.RGB = 0;
	}
	
	if (named.CMYK)
	{
		if (named.CMYK->decRef() == 0)
			delete named.CMYK;
		named.CMYK = 0;
	}
}

void XWPSDeviceColorSpace::free()
{
	for (int i = 0; i < 3; i++)
	{
		if (indexed[i])
		{
			if (indexed[i]->decRef() == 0)
			  delete indexed[i];
			indexed[i] = 0;
		}
	}
}

XWPSImagerStateShared::XWPSImagerStateShared()
	:XWPSStruct()
{
}

XWPSImagerStateShared::~XWPSImagerStateShared()
{
}

int XWPSImagerStateShared::getLength()
{
	return sizeof(XWPSImagerStateShared);
}

const char * XWPSImagerStateShared::getTypeName()
{
	return "imagerstateshared";
}

XWPSColorSpace *XWPSImagerStateShared::deviceCMYK()
{
	return device_color_spaces.deviceCMYK();
}

XWPSColorSpace *XWPSImagerStateShared::deviceGray()
{
	return device_color_spaces.deviceGray();
}

XWPSColorSpace *XWPSImagerStateShared::deviceRGB()
{
	return device_color_spaces.deviceRGB();
}

XWPSTransparencySource::XWPSTransparencySource()
	:XWPSStruct(),
	 alpha(1.0)
{
}

int XWPSTransparencySource::getLength()
{
	return sizeof(XWPSTransparencySource);
}

const char * XWPSTransparencySource::getTypeName()
{
	return "transparencysource";
}

XWPSTransparencyMaskParams::XWPSTransparencyMaskParams()
{
	subtype = TRANSPARENCY_MASK_Alpha;
	has_Background = false;
	memset(Background, 0, PS_CLIENT_COLOR_MAX_COMPONENTS*sizeof(float));
	TransferFunction = &XWPSTransparencyMaskParams::maskTransferIdentity;
	TransferFunction_data = 0;
}

XWPSTransparencyMaskParams::~XWPSTransparencyMaskParams()
{
	if (TransferFunction_data)
	{
		XWPSFunction * pfn = (XWPSFunction*)(TransferFunction_data);
		if (pfn->decRef() == 0)
			delete pfn;
		TransferFunction_data = 0;
	}
}

int XWPSTransparencyMaskParams::maskTransferIdentity(float in, float *out, void *)
{
	*out = in;
  return 0;
}

int XWPSTransparencyMaskParams::tfusingFunction(float in_val, float *out, void *proc_data)
{
	float in = in_val;
  XWPSFunction *const pfn = (XWPSFunction*)proc_data;
  return pfn->evaluate(&in, out);
}

XWPSTransfer::XWPSTransfer()
{
	u.indexed[0] = 0;
	u.indexed[1] = 0;
	u.indexed[2] = 0;
	u.indexed[3] = 0;
	u.colored.red = 0;
	u.colored.green = 0;
	u.colored.blue = 0;
	u.colored.gray = 0;
}

XWPSTransfer::~XWPSTransfer()
{
	if (u.colored.red)
	{
			if (!(u.colored.red->decRef()))
				delete u.colored.red;	
		u.colored.red = 0;
	}
	
	if (u.colored.green)
	{
			if (!(u.colored.green->decRef()))
				delete u.colored.green;	
		u.colored.green = 0;
	}
	
	if (u.colored.blue)
	{
			if (!(u.colored.blue->decRef()))
				delete u.colored.blue;	
		u.colored.blue = 0;
	}
	
	if (u.colored.gray)
	{
			if (!(u.colored.gray->decRef()))
				delete u.colored.gray;	
		u.colored.gray = 0;
	}
}

void XWPSTransfer::copy(XWPSTransfer * other)
{		
	if (u.colored.red != other->u.colored.red)
	{
		if (u.colored.red)
			if (!(u.colored.red->decRef()))
				delete u.colored.red;	
		u.colored.red = other->u.colored.red;
		if (u.colored.red)
			u.colored.red->incRef();
	}
	
	if (u.colored.green != other->u.colored.green)
	{
		if (u.colored.green)
			if (!(u.colored.green->decRef()))
				delete u.colored.green;	
		u.colored.green = other->u.colored.green;
		if (u.colored.green)
			u.colored.green->incRef();
	}
	
	if (u.colored.blue != other->u.colored.blue)
	{
		if (u.colored.blue)
			if (!(u.colored.blue->decRef()))
				delete u.colored.blue;	
		u.colored.blue = other->u.colored.blue;
		if (u.colored.blue)
			u.colored.blue->incRef();
	}
		
	if (u.colored.gray != other->u.colored.gray)
	{
		if (u.colored.gray)
			if (!(u.colored.gray->decRef()))
				delete u.colored.gray;	
		u.colored.gray = other->u.colored.gray;
		if (u.colored.gray)
			u.colored.gray->incRef();
	}
}

void XWPSTransfer::loadTransferMap(float min_value)
{
	u.colored.red->loadTransferMap(min_value);
	u.colored.green->loadTransferMap(min_value);
	u.colored.blue->loadTransferMap(min_value);
	u.colored.gray->loadTransferMap(min_value);
}

void XWPSTransfer::remap(int t)
{
	u.colored.red->setType((PSTransferMapType)t);
	u.colored.green->setType((PSTransferMapType)t);
	u.colored.blue->setType((PSTransferMapType)t);
	u.colored.gray->setType((PSTransferMapType)t);
}

void XWPSTransfer::setIndexed(XWPSTransferMap * pmap)
{
	for (int i = 0; i < 4; i++)
	{
		if (u.indexed[i] != pmap)
		{
			if (u.indexed[i])
				if (!(u.indexed[i]->decRef()))
					delete u.indexed[i];	
			u.indexed[i] = pmap;
			if (pmap)
				pmap->incRef();
		}
	}
}

XWPSPatternCache::XWPSPatternCache()
{
	tiles = 0;
	num_tiles = 0;
	tiles_used = 0;
	next = 0;
	bits_used = 0;
	max_bits = 0;
	own_tiles = true;
}

XWPSPatternCache::XWPSPatternCache(uint num_tilesA, ulong max_bitsA)
{
	num_tiles = num_tilesA;
	max_bits = max_bitsA;
	tiles = new XWPSColorTile[num_tilesA];
	tiles_used = 0;
	next = 0;
	bits_used = 0;
	for (uint i = 0; i < num_tiles; i++)
		tiles[i].index = i;
	own_tiles = true;
}

XWPSPatternCache::~XWPSPatternCache()
{
	if (tiles)
	{
		if (own_tiles)
		{
			for (int i = 0; i < num_tiles; i++)
			{
				if (tiles[i].tbits.data)
				{
					delete [] tiles[i].tbits.data;
					tiles[i].tbits.data = 0;
				}
				
				if (tiles[i].tmask.data)
				{
					delete [] tiles[i].tmask.data;
					tiles[i].tmask.data = 0;
				}
			}
			
			delete [] tiles;
		}
		tiles = 0;
	}
}

void XWPSPatternCache::copy(const XWPSPatternCache * from)
{
	own_tiles = false;
	tiles = from->tiles;
	num_tiles = from->num_tiles;
	tiles_used = from->tiles_used;
	next = from->next;
	bits_used = from->bits_used;
	max_bits = from->max_bits;
}

void XWPSPatternCache::freeEntry(XWPSColorTile * ctile)
{
	if (ctile->id != ps_no_bitmap_id)
	{
		XWPSDeviceMem mdev;
		mdev.incRef();
		mdev.makeMemMonoDevice(NULL);
		if (ctile->tmask.data != 0) 
		{
	    mdev.width = ctile->tmask.size.x;
	    mdev.height = ctile->tmask.size.y;
	    
	    bits_used -= mdev.bitmapSize();
	    if (ctile->tmask.data)
	    	delete [] (ctile->tmask.data);
	    ctile->tmask.data = 0;
		}
		if (ctile->tbits.data != 0) 
		{
	    mdev.width = ctile->tbits.size.x;
	    mdev.height = ctile->tbits.size.y;
	    mdev.color_info.depth = ctile->depth;
	    bits_used -= mdev.bitmapSize();
	    if (ctile->tbits.data)
	    	delete [] (ctile->tbits.data);
	    ctile->tbits.data = 0;
		}
		ctile->id = ps_no_bitmap_id;
		tiles_used--;
  }
}

static const PSColorMapProcs cmap_gray_many = {
	&XWPSImagerState::cmapGrayDirect, 
	&XWPSImagerState::cmapRGBToGrayDirect, 
	&XWPSImagerState::cmapCMYKToGray,
  &XWPSImagerState::cmapRGBAlpha2GrayDirect
};

static const PSColorMapProcs cmap_rgb_many = {
	&XWPSImagerState::cmapGrayToRGBDirect, 
	&XWPSImagerState::cmapRGBDirect, 
	&XWPSImagerState::cmapCMYKToRGB,
  &XWPSImagerState::cmapRGBAlphaDirect
};

static const PSColorMapProcs cmap_cmyk_many = {
	&XWPSImagerState::cmapGrayToCMYKDirect, 
	&XWPSImagerState::cmapRGBToCMYK, 
	&XWPSImagerState::cmapCMYKDirect,
  &XWPSImagerState::cmapRGBAlphaToCMYK
};

static const PSColorMapProcs *const cmap_many[] =
{
    0, &cmap_gray_many, 0, &cmap_rgb_many, &cmap_cmyk_many
};

XWPSImagerState::XWPSImagerState()
	:XWPSStruct()
{
	client_data = 0;
	shared = 0;
	log_op = lop_default;
	alpha = ps_max_color_value;
	ctm.yy = -1.0;
	blend_mode = BLEND_MODE_Compatible;
	text_knockout = false;
	transparency_stack = 0;
	overprint = false;
	overprint_mode = 0;
	flatness = 1.0;
	fill_adjust.x = fixed_half;
	fill_adjust.y = fixed_half;
	stroke_adjust = false;
	accurate_curves = false;
	smoothness = 1.0;
	get_cmap_procs = &XWPSImagerState::getCmapProcsDefault;
	halftone = 0;
	dev_ht = 0;
	ht_cache = 0;
	cie_render = 0;
	black_generation = 0;
	undercolor_removal = 0;
	
	cie_joint_caches = 0;
	
	cmap_procs = &cmap_gray_many;
	
	pattern_cache = 0;
	i_ctx_p = 0;
	own_pattern_cache = false;
}

XWPSImagerState::~XWPSImagerState()
{
	release();
}

double XWPSImagerState::charFlatness(float default_scale)
{
	double cxx = fabs(ctm.xx), cyy = fabs(ctm.yy);

  if (is_fzero(cxx) || (cyy < cxx && !is_fzero(cyy)))
		cxx = cyy;
  if (!ctm.isXXYY()) 
  {
		double cxy = fabs(ctm.xy), cyx = fabs(ctm.yx);

		if (is_fzero(cxx) || (cxy < cxx && !is_fzero(cxy)))
	    cxx = cxy;
		if (is_fzero(cxx) || (cyx < cxx && !is_fzero(cyx)))
	    cxx = cyx;
  }
  cxx *= 0.001 / default_scale;
  if (cxx > flatness)
		cxx = flatness;
  if (cxx < 0.2)
		cxx = 0;
  return cxx;
}

bool XWPSImagerState::checkTileCache()
{
	XWPSHTOrder *porder = &dev_ht->order;
  XWPSHTCache *pcache = ht_cache;

  if (pcache == 0 || dev_ht == 0)
		return false;	
  
  if (pcache->order.bit_data->arr != porder->bit_data->arr)
		pcache->init(porder);
  if (pcache->tiles_fit >= 0)
		return (bool)pcache->tiles_fit;
    
  {
		bool fit = false;
		int bits_per_level;

		if (pcache->num_cached < porder->num_levels)
	    ;
		else if (pcache->levels_per_tile == 1)
	    fit = true;
		else if (porder->num_bits % porder->num_levels == 0 &&
		 	(bits_per_level = porder->num_bits / porder->num_levels) % pcache->levels_per_tile == 0) 
		{
	    const uint *level = porder->levels->ptr;
	    int i = porder->num_levels, j = 0;

	    for (; i > 0; --i, j += bits_per_level, ++level)
				if (*level != j)
		    	break;
	    fit = i == 0;
		}
		pcache->tiles_fit = (int)fit;
		return fit;
  }
}

int XWPSImagerState::checkTileSize(int w, int y, int h,  PSColorSelect select, int *ppx)
{
	int tsy;
  XWPSStripBitmap *ptile0;

  if (ht_cache == 0)
		return -1;
    
  ptile0 = &ht_cache->ht_tiles[0].tiles;
  if (h > ptile0->rep_height || w > ptile0->rep_width ||	ptile0->shift != 0)
		return -1;
    
  tsy = (y + imod(-screen_phase[select].y, ptile0->rep_height)) %	ptile0->rep_height;
  if (tsy + h > ptile0->size.y)
		return -1;
  *ppx = imod(-screen_phase[select].x, ptile0->rep_width);
  return tsy * ptile0->raster;
}

int  XWPSImagerState::cieJCComplete(XWPSColorSpace *pcs)
{
	XWPSCieABC *pabc;
  XWPSCie *common = pcs->cieCSCommonABC(&pabc);
  XWPSCieRender *pcrd = cie_render;
  XWPSCieJointCaches *pjc = cie_joint_caches;

  if (pjc->cspace_id == pcs->id &&	pjc->render_id == pcrd->id)
		pjc->status = pjc->id_status;
  switch (pjc->status) 
  {
    case CIE_JC_STATUS_BUILT: 
    	{
				int code = pjc->init(common, pcrd);

				if (code < 0)
	    		return code;
    	}
    	
    case CIE_JC_STATUS_INITED:
			pjc->complete(common, pabc, pcrd);
			pjc->cspace_id = pcs->id;
			pjc->render_id = pcrd->id;
			pjc->id_status = pjc->status = CIE_JC_STATUS_COMPLETED;
			
    case CIE_JC_STATUS_COMPLETED:
			break;
  }
  return 0;
}

void XWPSImagerState::cmapCMYKDirect(short c, 
	                    short m, 
	                    short y, 
	                    short k, 
	                    XWPSDeviceColor * pdc,
                      XWPSDevice * dev, 
                      PSColorSelect select)
{
	short mcyan = frac_1 - effective_transfer.u.colored.red->mapColorFrac(frac_1 - c);
  short mmagenta = frac_1 - effective_transfer.u.colored.green->mapColorFrac(frac_1 - m);
  short myellow = frac_1 - effective_transfer.u.colored.blue->mapColorFrac(frac_1 - y);
  short mblack = frac_1 - effective_transfer.u.colored.gray->mapColorFrac(frac_1 - k);

  ulong color =	dev->mapCMYKColor(frac2cv(mcyan), frac2cv(mmagenta), frac2cv(myellow), frac2cv(mblack));

  if (color != ps_no_color_index) 
  {
	  pdc->setPure(color);
	  return;
	}
  
  
  if (pdc->renderCMYK(mcyan, mmagenta, myellow, mblack, this, dev, select) == 1)
		pdc->loadSelect(this, dev, select);
}

void XWPSImagerState::cmapCMYKToGray(short c, 
	                    short m, 
	                    short y, 
	                    short k, 
	                    XWPSDeviceColor * pdc,
                      XWPSDevice * dev, 
                      PSColorSelect select)
{
	(this->*(cmap_procs->map_gray))(colorCMYKToGray(c, m, y, k), pdc, dev, select);
}

void XWPSImagerState::cmapCMYKToRGB(short c, 
	                   short m, 
	                   short y, 
	                   short k, 
	                   XWPSDeviceColor * pdc,
                     XWPSDevice * dev, 
                     PSColorSelect select)
{
	short rgb[3];

  colorCMYKToRgb(c, m, y, k, rgb);
  mapRGB(rgb[0], rgb[1], rgb[2], pdc, dev, select);
}

void XWPSImagerState::cmapGrayDirect(short gray, 
	                                   XWPSDeviceColor * pdc, 
		                                 XWPSDevice * dev, 
		                                 PSColorSelect select)
{
	short mgray = effective_transfer.u.colored.gray->mapColorFrac(gray);
  ulong cv_gray = frac2cv(mgray);
  ulong color = (alpha == ps_max_color_value ?
     						dev->mapRGBColor(cv_gray, cv_gray, cv_gray) :
     						dev->mapRGBAlphaColor(cv_gray, cv_gray, cv_gray, alpha));

  if (color == ps_no_color_index) 
  {
		if (pdc->renderGray(mgray, this, dev, select) == 1)
	    pdc->loadSelect(this, dev, select);
		return;
  }
  pdc->setPure(color);
}

void XWPSImagerState::cmapGrayToCMYKDirect(short gray, 
	                          XWPSDeviceColor * pdc,
                            XWPSDevice * dev, 
                            PSColorSelect select)
{
	short mgray = effective_transfer.u.colored.gray->mapColorFrac(gray);
  short mblack = frac_1 - mgray;
  ulong color = dev->mapCMYKColor(frac2cv(frac_0), frac2cv(frac_0), frac2cv(frac_0), frac2cv(mblack));

  if (color != ps_no_color_index) 
  {
		pdc->setPure(color);
		return;
  }
  
  if (pdc->renderGray(mgray, this, dev, select) == 1)
		pdc->loadSelect(this, dev, select);
}

void XWPSImagerState::cmapGrayToRGBDirect(short gray, 
	                         XWPSDeviceColor * pdc,
                           XWPSDevice * dev, 
                           PSColorSelect select)
{
	cmapRGBDirect(gray, gray, gray, pdc, dev, select);
}

#ifdef PREMULTIPLY_TOWARDS_WHITE
#  define alpha_bias_value frac_1 - alpha
#  define alpha_bias(v) ((v) + alpha_bias_value
#else
#  define alpha_bias(v) (v)
#endif

void XWPSImagerState::cmapRGBAlpha2GrayDirect(short r, 
	                             short g, 
	                             short b, 
	                             short alpha,
	                             XWPSDeviceColor * pdc, 
	                             XWPSDevice * dev,
			                         PSColorSelect select)
{
	short gray = colorRGBToGray(r, g, b);

  if (alpha != frac_1)
		gray = alpha_bias((short) ((long)gray * alpha / frac_1));
  {
		short mgray = effective_transfer.u.colored.gray->mapColorFrac(gray);
		ushort cv_gray = frac2cv(mgray);
		ulong color =	(alpha == frac_1 ? dev->mapRGBColor(cv_gray, cv_gray, cv_gray) :
	 			dev->mapRGBAlphaColor(cv_gray, cv_gray, cv_gray, frac2cv(alpha)));

		if (color == ps_no_color_index) 
		{
	    if (pdc->renderGrayAlpha(mgray, frac2cv(alpha), this, dev, select) == 1)
				pdc->loadSelect(this, dev, select);
	    return;
		}
		pdc->setPure(color);
  }
}

void XWPSImagerState::cmapRGBAlphaDirect(short r, 
	                        short g, 
	                        short b, 
	                        short alphaA, 
	                        XWPSDeviceColor * pdc,
                          XWPSDevice * dev, 
                          PSColorSelect select)
{
	short red = r, green = g, blue = b;

  if (alphaA != frac_1) 
  {	/* premultiply */
		red = alpha_bias((short) ((long)red * alphaA / frac_1));
		green = alpha_bias((short) ((long)green * alphaA / frac_1));
		blue = alpha_bias((short) ((long)blue * alphaA / frac_1));
  } 
  
  {
		short mred =	effective_transfer.u.colored.red->mapColorFrac(red);
		short mgreen =	effective_transfer.u.colored.green->mapColorFrac(green);
		short mblue =	effective_transfer.u.colored.blue->mapColorFrac(blue);
		ushort cv_alpha = frac2cv(alphaA);
		ulong color =	(cv_alpha == ps_max_color_value ? dev->mapRGBColor(frac2cv(mred), frac2cv(mgreen), frac2cv(mblue)) :
	 									dev->mapRGBAlphaColor(frac2cv(mred), frac2cv(mgreen),	frac2cv(mblue), cv_alpha));

		if (color != ps_no_color_index) 
		{
	    pdc->setPure(color);
	    return;
		}
		if ((mred == mgreen && mred == mblue ?	/* gray shade */
	     pdc->renderGrayAlpha(mred, cv_alpha, this, dev, select) :
	     pdc->renderRGBAlpha(mred, mgreen, mblue, cv_alpha, this, dev, select)) == 1)
	    pdc->loadSelect(this, dev, select);
  }
}

void XWPSImagerState::cmapRGBDirect(ushort r, 
	                   ushort g, 
	                   ushort b, 
	                   XWPSDeviceColor * pdc,
                     XWPSDevice * dev, 
                     PSColorSelect select)
{
	short mred = effective_transfer.u.colored.red->mapColorFrac(r);
  short mgreen = effective_transfer.u.colored.green->mapColorFrac(g);
  short mblue = effective_transfer.u.colored.blue->mapColorFrac(b);
  ulong color = (alpha == ps_max_color_value ? dev->mapRGBColor(frac2cv(mred), frac2cv(mgreen), frac2cv(mblue)) :
     							dev->mapRGBAlphaColor(frac2cv(mred), frac2cv(mgreen), frac2cv(mblue), alpha));
     							
  if (color != ps_no_color_index) 
  {
		pdc->setPure(color);
		return;
  }
  
  if ((mred == mgreen && mred == mblue ?	/* gray shade */
	    pdc->renderGray(mred, this, dev, select) :
	 		pdc->renderRGB(mred, mgreen, mblue, this, dev, select)) == 1)
	pdc->loadSelect(this, dev, select);
}

void XWPSImagerState::cmapRGBToCMYK(ushort r, 
	                   ushort g, 
	                   ushort b, 
	                   XWPSDeviceColor * pdc,
                     XWPSDevice * dev, 
                     PSColorSelect select)
{
	short cmyk[4];

  colorRGBToCMYK(r, g, b, cmyk);
  mapCMYK(cmyk[0], cmyk[1], cmyk[2], cmyk[3], pdc, dev, select);
}

void XWPSImagerState::cmapRGBToGrayDirect(ushort r, 
	                         ushort g, ushort b, 
	                         XWPSDeviceColor * pdc,
                           XWPSDevice * dev, 
                           PSColorSelect select)
{
	cmapGrayDirect(colorRGBToGray(r, g, b), pdc, dev, select);
}

void XWPSImagerState::cmapRGBAlphaToCMYK(short r, 
	                        short g, 
	                        short b, 
	                        short alphaA,
	                        XWPSDeviceColor * pdc, 
	                        XWPSDevice * dev,
		                      PSColorSelect select)
{
#ifdef PREMULTIPLY_TOWARDS_WHITE
#  undef alpha_bias_value
    short alpha_bias_value = frac_1 - alpha;

#endif

    cmapRGBToCMYK(alpha_bias((short) ((long)r * alpha / frac_1)),
		     alpha_bias((short) ((long)g * alpha / frac_1)),
		     alpha_bias((short) ((long)b * alpha / frac_1)),
		     pdc, dev, select);
}

#undef alpha_bias
#undef alpha_bias_value

short XWPSImagerState::colorCMYKToGray(short c, short m, short y, short k)
{
	short not_gray = colorRGBToGray(c, m, y);
  return (not_gray > frac_1 - k ?	 frac_0 : frac_1 - (not_gray + k));
}

void  XWPSImagerState::colorCMYKToRgb(short c, short m, short y, short k, short * rgb)
{
	switch (k) 
	{
		case frac_0:
	    rgb[0] = frac_1 - c;
	    rgb[1] = frac_1 - m;
	    rgb[2] = frac_1 - y;
	    break;
	    
		case frac_1:
	    rgb[0] = rgb[1] = rgb[2] = frac_0;
	    break;
	    
		default:
	    {
#ifdef USE_ADOBE_CMYK_RGB
				short not_k = frac_1 - k;

				rgb[0] = (c > not_k ? frac_0 : not_k - c);
				rgb[1] = (m > not_k ? frac_0 : not_k - m);
				rgb[2] = (y > not_k ? frac_0 : not_k - y);
#else
				ulong not_k = frac_1 - k;
				ulong prod;

#define deduct_black(v)\
  (prod = (frac_1 - (v)) * not_k, frac_1_quo(prod))
  
				rgb[0] = deduct_black(c);
				rgb[1] = deduct_black(m);
				rgb[2] = deduct_black(y);
#undef deduct_black
#endif
	    }
  }
}

void XWPSImagerState::colorRGBToCMYK(short r, short g, short b, short * cmyk)
{
	short c = frac_1 - r, m = frac_1 - g, y = frac_1 - b;
  short k = (c < m ? qMin(c, y) : qMin(m, y));
  short bg = (black_generation == NULL ? frac_0 :	 black_generation->mapColorFrac(k));
  short ucr =	(undercolor_removal == NULL ? frac_0 : undercolor_removal->mapColorFrac(k));

  if (ucr == frac_1)
		cmyk[0] = cmyk[1] = cmyk[2] = 0;
  else if (ucr == frac_0)
		cmyk[0] = c, cmyk[1] = m, cmyk[2] = y;
  else 
  {
#ifdef USE_ADOBE_CMYK_RGB

		short not_ucr = (ucr < 0 ? frac_1 + ucr : frac_1);

		cmyk[0] = (c < ucr ? frac_0 : c > not_ucr ? frac_1 : c - ucr);
		cmyk[1] = (m < ucr ? frac_0 : m > not_ucr ? frac_1 : m - ucr);
		cmyk[2] = (y < ucr ? frac_0 : y > not_ucr ? frac_1 : y - ucr);
#else
		float denom = frac2float(frac_1 - ucr);	
		float v = (float)frac_1 - r / denom;
		cmyk[0] =   (is_fneg(v) ? frac_0 : v >= (float)frac_1 ? frac_1 : (frac) v);
		v = (float)frac_1 - g / denom;
		cmyk[1] = (is_fneg(v) ? frac_0 : v >= (float)frac_1 ? frac_1 : (frac) v);
		v = (float)frac_1 - b / denom;
		cmyk[2] = (is_fneg(v) ? frac_0 : v >= (float)frac_1 ? frac_1 : (frac) v);
#endif
  }
  cmyk[3] = bg;
}

short XWPSImagerState::colorRGBToGray(short r, short g, short b)
{
	return (r * (unsigned long)lum_red_weight +
	    		g * (unsigned long)lum_green_weight +
	    		b * (unsigned long)lum_blue_weight +
	    		(lum_all_weights / 2))	/ lum_all_weights;
}

void XWPSImagerState::ctmOnly(XWPSMatrix * pmat)
{
	pmat->xx = ctm.xx;
	pmat->xy = ctm.xy;
	pmat->yx = ctm.yx;
	pmat->yy = ctm.yy;
	pmat->tx = ctm.tx;
	pmat->ty = ctm.ty;
}

int  XWPSImagerState::currentCurveJoin()
{
	return line_params.curve_join;
}

bool XWPSImagerState::currentDashAdapt()
{
	return line_params.dash.adapt;
}

uint XWPSImagerState::currentDashLength()
{
	return line_params.dash.pattern_size;
}

float XWPSImagerState::currentDashOffset()
{
	return line_params.dash.offset;
}

const float * XWPSImagerState::currentDashPattern()
{
	return line_params.dash.pattern;
}

float XWPSImagerState::currentDotLength()
{
	return line_params.dot_length;
}

bool  XWPSImagerState::currentDotLengthAbsolute()
{
	return line_params.dot_length_absolute;
}

int XWPSImagerState::currentHalftonePhase(XWPSIntPoint * pphase)
{
	return currentScreenPhase(pphase, (PSColorSelect)0);
}

PSLineCap XWPSImagerState::currentLineCap()
{
	return line_params.cap;
}

PSLineJoin XWPSImagerState::currentLineJoin()
{
	return line_params.join;
}

float XWPSImagerState::currentLineWidth()
{
	return line_params.currentLineWidth();
}

int XWPSImagerState::currentMatrix(XWPSMatrix * pmat)
{
	pmat->xx = ctm.xx, 
	pmat->xy = ctm.xy, 
	pmat->yx = ctm.yx, 
	pmat->yy = ctm.yy, 
	pmat->tx = ctm.tx, 
	pmat->ty = ctm.ty;
	return 0;
}

float XWPSImagerState::currentMiterLimit()
{
	return line_params.miter_limit;
}

int XWPSImagerState::currentScreenPhase(XWPSIntPoint * pphase, PSColorSelect select)
{
	if (select < 0 || select >= ps_color_select_count)
		return (int)(XWPSError::RangeCheck);
  *pphase = screen_phase[select];
  return 0;
}

int XWPSImagerState::devhtInstall(XWPSDeviceHalftone * pdht, PSHalftoneType type, 
			             const XWPSDevice * dev)
{
	if ((ulong) pdht->order.raster *	(pdht->order.num_bits / pdht->order.width) > ht_cache->bits_size)
		return (int)(XWPSError::LimitCheck);
	
	if (pdht != dev_ht)
	{
		if (dev_ht)
			if (dev_ht->decRef() == 0)
				delete dev_ht;
				
		dev_ht = pdht;
		if (dev_ht)
			dev_ht->incRef();
	}
  
  ht_cache->clear();
  if (i_ctx_p)
  	dev_ht->id = i_ctx_p->nextIDS(1);
  if (dev_ht->components != 0)
  {
  	static const PSHTSeparationName dcnames[5][4] =
		{
	    {ps_ht_separation_Default},
	    {ps_ht_separation_Default, ps_ht_separation_Default,
	     ps_ht_separation_Default, ps_ht_separation_Gray
	    },
	    {ps_ht_separation_Default},
	    {ps_ht_separation_Red, ps_ht_separation_Green,
	     ps_ht_separation_Blue, ps_ht_separation_Default
	    },
	    {ps_ht_separation_Cyan, ps_ht_separation_Magenta,
	     ps_ht_separation_Yellow, ps_ht_separation_Black
	    }
		};
		static const PSHTSeparationName cscnames[4] =
		{
			ps_ht_separation_Red, ps_ht_separation_Green,
	 		ps_ht_separation_Blue, ps_ht_separation_Default
		};
		
		int num_comps = dev->color_info.num_components;
		const PSHTSeparationName *cnames = dcnames[num_comps];
		int lcm_width = 1, lcm_height = 1;
		uint i;
		if (num_comps == 4 && (type == ht_type_colorscreen ||  type == ht_type_multiple_colorscreen))
	    cnames = cscnames;
	   
		for (i = 0; i < dev_ht->num_comp; i++)
		{
			const XWPSHTOrderComponent *pcomp = &dev_ht->components[i];
	    int j;

	    for (j = 0; j < 4; j++) 
	    {
				if (pcomp->cname == cnames[j]) 
		    	dev_ht->color_indices[j] = i;
	    }
		}
		for (i = 0; i < 4; ++i) 
		{
	    const XWPSHTOrderComponent *pcomp =  &dev_ht->components[dev_ht->color_indices[i]];
	    uint cw = pcomp->corder.width;
	    uint ch = pcomp->corder.full_height;
	    int dw = lcm_width / igcd(lcm_width, cw);
	    int dh = lcm_height / igcd(lcm_height, ch);

	    lcm_width = (cw > max_int / dw ? max_int : cw * dw);
	    lcm_height = (ch > max_int / dh ? max_int : ch * dh);
		}
		dev_ht->lcm_width = lcm_width;
		dev_ht->lcm_height = lcm_height;
  }
  else
  {
  	dev_ht->lcm_width = dev_ht->order.width;
		dev_ht->lcm_height = dev_ht->order.full_height;
  }
  setEffectiveXFer();
  return 0;
}

XWPSColorSpace *XWPSImagerState::deviceCMYK()
{
	return shared->deviceCMYK();
}

XWPSColorSpace *XWPSImagerState::deviceGray()
{
	return shared->deviceGray();
}

XWPSColorSpace *XWPSImagerState::deviceRGB()
{
	return shared->deviceRGB();
}

int  XWPSImagerState::dotOrientation()
{
	return setMatrix(&line_params.dot_orientation);
}

int XWPSImagerState::ensurePatternCache()
{
	if (!pattern_cache)
	{
		pattern_cache = new XWPSPatternCache(max_cached_patterns_SMALL, max_pattern_bits_SMALL);
		own_pattern_cache = true;
	}
	
	return 0;
}

const PSColorMapProcs * XWPSImagerState::getCmapProcs(XWPSDevice * dev)
{
	if (get_cmap_procs)
		return (this->*get_cmap_procs)(dev);
		
	return 0;
}

const PSColorMapProcs * XWPSImagerState::getCmapProcsDefault(XWPSDevice * dev)
{
	return cmap_many[dev->color_info.num_components];
}

int XWPSImagerState::getLength()
{
	return sizeof(XWPSImagerState);
}

const char * XWPSImagerState::getTypeName()
{
	return "imagerstate";
}

int XWPSImagerState::idtransform(double dx, double dy, XWPSPoint * pt)
{
	XWPSMatrix c;
	c = ctm;
	return pt->distanceTransformInverse(dx, dy, &c);
}

float XWPSImagerState::joinExpansionFactor(PSLineJoin join)
{
	switch (join) 
	{
    case ps_join_miter: 
    	return line_params.miter_limit;
    	
    case ps_join_triangle: 
    	return 2.0;
    	
    default: 
    	return 1.0;
  }
}

void XWPSImagerState::mapCMYK(short c, 
		           short m, 
		           short y, 
		           short k, 
		           XWPSDeviceColor * pdc, 
		           XWPSDevice * dev, 
		           PSColorSelect select)
{
	(this->*(cmap_procs->map_cmyk))(c,m,y,k, pdc, dev, select);
}

void XWPSImagerState::mapGray(short gray, 
		           XWPSDeviceColor * pdc, 
		           XWPSDevice * dev, 
		           PSColorSelect select)
{
	(this->*(cmap_procs->map_gray))(gray, pdc, dev, select);
}

void XWPSImagerState::mapGrayNo(short pixel, 
		           XWPSDeviceColor * pdc, 
		           XWPSDevice * , 
		           PSColorSelect )
{
	pdc->setPure(frac2byte(pixel));
}

void XWPSImagerState::mapRGB(ushort r, 
		          ushort g, 
		          ushort b , 
		          XWPSDeviceColor * pdc, 
		          XWPSDevice * dev, 
		          PSColorSelect select)
{
	(this->*(cmap_procs->map_rgb))(r, g, b, pdc, dev, select);
}

void XWPSImagerState::mapRGBAlpha(short r, 
		          short g, 
		          short b , 
		          short a,
		          XWPSDeviceColor * pdc, 
		          XWPSDevice * dev, 
		          PSColorSelect select)
{
	(this->*(cmap_procs->map_rgb_alpha))(r, g, a, b, pdc, dev, select);
}

int XWPSImagerState::patternCacheAddEntry(XWPSDevicePatternAccum * padev, 
	                         XWPSColorTile ** pctile)
{
	XWPSDeviceMem *mbits = padev->bits;
  XWPSDeviceMem *mmask = padev->mask;
  XWPSPattern1Instance *pinst = padev->instance;
  XWPSPatternCache *pcache;
  ulong used = 0;
  ulong id = pinst->id;
  XWPSColorTile *ctile;
  int code = ensurePatternCache();

  if (code < 0)
		return code;
  pcache = pattern_cache;
  
  if (mmask != 0) 
  {
		int y;

		for (y = 0; y < mmask->height; y++) 
		{
	    uchar *row = mmask->scanLineBase(y);
	    int w;

	    for (w = mmask->width; w > 8; w -= 8)
				if (*row++ != 0xff)
		    	goto keep;
	    if ((*row | (0xff >> w)) != 0xff)
				goto keep;
		}
		
		mmask = 0;

keep:
		;
  }
  
  if (mbits != 0)
		used += mbits->bitmapSize();
  if (mmask != 0)
		used += mmask->bitmapSize();
  ctile = &pcache->tiles[id % pcache->num_tiles];
  pcache->freeEntry(ctile);
  while (pcache->bits_used + used > pcache->max_bits && pcache->bits_used != 0)
  {
  	pcache->next = (pcache->next + 1) % pcache->num_tiles;
		pcache->freeEntry(&pcache->tiles[pcache->next]);
  }
  
  ctile->id = id;
  ctile->depth = padev->color_info.depth;
  ctile->uid = pinst->templat.uid;
  ctile->tiling_type = pinst->templat.TilingType;
  ctile->step_matrix = pinst->step_matrix;
  ctile->bbox = pinst->bbox;
  ctile->is_simple = pinst->is_simple;
  if (mbits != 0) 
		makeBitmap(&ctile->tbits, mbits, i_ctx_p->nextIDS(1));
  else
		ctile->tbits.data = 0;
  if (mmask != 0) 
		makeBitmap(&ctile->tmask, mmask, id);
  else
		ctile->tmask.data = 0;
  pcache->bits_used += used;
  pcache->tiles_used++;
  *pctile = ctile;
  return 0;
}

int XWPSImagerState::putDeviceParams(XWPSContextState * ctx, 
	                            XWPSDevice *dev,  
	                            XWPSParamList *plist)
{
	int code = dev->putDeviceParams(ctx, plist);

  if (code >= 0)
		setCmapProcs(dev);
  return code;
}

void XWPSImagerState::release()
{
	if (pattern_cache)
	{
		if (own_pattern_cache)
			delete pattern_cache;
		pattern_cache = 0;
	}
	
	if (client_data)
	{
		XWPSIntGState * iigs = (XWPSIntGState*)client_data;
		delete iigs;
	}
	
	if (cie_joint_caches)
	{
		if (!cie_joint_caches->decRef())
			delete cie_joint_caches;		
		cie_joint_caches = 0;
	}
	
	if (undercolor_removal)
	{
		if (!undercolor_removal->decRef())
			delete undercolor_removal;		
		undercolor_removal = 0;
	}
	
	if (black_generation)
	{
		if (!black_generation->decRef())
			delete black_generation;		
		black_generation = 0;
	}
	
	if (cie_render)
	{
		if (cie_render->decRef() == 0)
			delete cie_render;
		cie_render = 0;
	}
	
	if (dev_ht)
	{
		if (dev_ht->decRef() == 0)
			delete dev_ht;
		dev_ht = 0;
	}
	
	if (halftone)
	{
		if (halftone->decRef() == 0)
			delete halftone;
		halftone = 0;
	}
	
	if (ht_cache)
	{
		if (!ht_cache->decRef())
		{
			ht_cache->order.cache = 0;
			delete ht_cache;
		}
		ht_cache = 0;
	}
	
	if (shared)
	{
		if (!shared->decRef())
			delete shared;		
		shared = 0;
	}
}

int XWPSImagerState::remapCIEABC(XWPSClientColor * pc, 
	                 XWPSColorSpace * pcs,
	                 XWPSDeviceColor * pdc, 
	                 XWPSDevice * dev,
		               PSColorSelect select)
{
	ushort conc[4];
  PSCieCachedVector3 vec3;
  
  if (cie_render == 0) 
  {
		conc[0] = conc[1] = conc[2] = frac_0;
		goto map3;
  }
  
  if (cie_joint_caches->status != CIE_JC_STATUS_COMPLETED) 
  {
		int code = cieJCComplete(pcs);
		if (code < 0)
	    return code;
  }
  
  vec3.u = float2cie_cached(pc->paint.values[0]);
  vec3.v = float2cie_cached(pc->paint.values[1]);
  vec3.w = float2cie_cached(pc->paint.values[2]);

  if (!cie_joint_caches->skipDecodeABC) 
  {
		const XWPSCieABC *pcie = pcs->params.abc;

		cie_lookup_map3(&vec3, &pcie->caches.DecodeABC[0],"Decode/MatrixABC");
  }
  switch (pcs->cieRemapFinish(vec3, conc, this)) 
  {
		case 4:
	    mapCMYK(conc[0], conc[1], conc[2], conc[3],  pdc, dev, select);
	    return 0;
	    
		default:	
	    return (int)(XWPSError::Unknown);
			
		case 3:
	    ;
  }
  
map3:
  mapRGB(conc[0], conc[1], conc[2], pdc, dev, select);
  return 0;
}

int XWPSImagerState::remapColorDefault(XWPSClientColor * pcc, 
	                      XWPSColorSpace * pcs,
	                      XWPSDeviceColor * pdc, 
	                      XWPSDevice * dev,
		                    PSColorSelect select)
{
	ushort conc[PS_CLIENT_COLOR_MAX_COMPONENTS];
  XWPSColorSpace *pconcs;
  int code = pcs->concretizeColor(pcc, conc, this);

  if (code < 0)
		return code;
  pconcs = pcs->concreteSpace(this);
  return pconcs->remapConcreteColor(this, conc, pdc, dev, select);
}

int XWPSImagerState::remapColorPattern1(XWPSClientColor * pc, 
	                      XWPSColorSpace * pcs,
	                      XWPSDeviceColor * pdc, 
	                      XWPSDevice * dev,
		                    PSColorSelect select)
{
	XWPSPattern1Instance *pinst = (XWPSPattern1Instance *)pc->pattern;
  int code;

  pdc->ccolor = *pc;
  if (pinst == 0) 
  {
		pdc->setNullPattern();
		return 0;
  }
  
  if (pinst->templat.PaintType == 2) 
  {
		code = pcs->params.pattern->base_space->remapColor(this, pc, 
		                pcs->params.pattern->base_space,  pdc, dev, select);
		if (code < 0)
	    return code;
		if (pdc->type == &ps_dc_type_data_pure)
	    pdc->type = &ps_dc_pure_masked;
		else if (pdc->type == &ps_dc_type_ht_binary)
	    pdc->type = &ps_dc_binary_masked;
		else if (pdc->type == &ps_dc_type_ht_colored)
	    pdc->type = &ps_dc_colored_masked;
		else
	    return (int)(XWPSError::Unregistered);
  } 
  else
		pdc->setNullPattern();
  pdc->mask.id = pinst->id;
  pdc->mask.m_tile = 0;
  return pdc->loadPattern(this, dev, select);
}

int XWPSImagerState::remapColorPattern2(XWPSClientColor * pcc, 
	                      XWPSColorSpace * ,
	                      XWPSDeviceColor * pdc, 
	                      XWPSDevice * ,
		                    PSColorSelect )
{
	pdc->type = &ps_dc_pattern2;
  pdc->ccolor = *pcc;
  return 0;
}

int XWPSImagerState::remapConcreteDCMYK(const ushort * pconc,
	                                      XWPSDeviceColor * pdc, 
	                                      XWPSDevice * dev,
			                                  PSColorSelect select)
{
	(this->*(cmap_procs->map_cmyk))(pconc[0], pconc[1], pconc[2], pconc[3], pdc, dev, select);
  return 0;
}

int XWPSImagerState::remapConcreteDGray(const ushort * pconc,
	                                      XWPSDeviceColor * pdc, 
	                                      XWPSDevice * dev,
			                                  PSColorSelect select)
{
	if (alpha == ps_max_color_value)
		(this->*(cmap_procs->map_gray))(pconc[0], pdc, dev, select);
  else
		(this->*(cmap_procs->map_rgb_alpha))(pconc[0], pconc[0], pconc[0], cv2frac(alpha), pdc, dev, select);
  return 0;
}

int XWPSImagerState::remapConcreteDeviceN(const ushort * ,
	                          XWPSDeviceColor * , 
	                          XWPSDevice * ,
			                      PSColorSelect )
{
	return (int)(XWPSError::RangeCheck);
}

int XWPSImagerState::remapConcreteDPixel(const ushort * pconc,
	                        XWPSDeviceColor * pdc, 
	                        XWPSDevice * dev,
			                    PSColorSelect)
{
	pdc->setPure(pconc[0] & ((1 << dev->color_info.depth) - 1));
  return 0;
}

int XWPSImagerState::remapConcreteDRGB(const ushort * pconc,
	                                     XWPSDeviceColor * pdc, 
	                                     XWPSDevice * dev,
			                                 PSColorSelect select)
{
	if (alpha == ps_max_color_value)
		(this->*(cmap_procs->map_rgb))(pconc[0], pconc[1], pconc[2], pdc, dev, select);
  else
		(this->*(cmap_procs->map_rgb_alpha))(pconc[0], pconc[1], pconc[2], cv2frac(alpha), pdc, dev, select);
  return 0;
}

int XWPSImagerState::remapConcreteSeparation(const ushort * ,
	                             XWPSDeviceColor * , 
	                             XWPSDevice * ,
			                         PSColorSelect )
{
	return (int)(XWPSError::RangeCheck);
}

int XWPSImagerState::remapDeviceCMYK(XWPSClientColor * pc, 
	                                   XWPSColorSpace * ,
	                                   XWPSDeviceColor * pdc, 
	                                   XWPSDevice * dev,
		                                 PSColorSelect select)
{
	float ft0, ft1, ft2, ft3;

  (this->*(cmap_procs->map_cmyk))(unit_frac(pc->paint.values[0], ft0),
			                          unit_frac(pc->paint.values[1], ft1),
			                          unit_frac(pc->paint.values[2], ft2),
			                          unit_frac(pc->paint.values[3], ft3),
			                          pdc, dev, select);
  return 0;
}

int XWPSImagerState::remapDeviceGray(XWPSClientColor * pc, 
	                                   XWPSColorSpace * ,
	                                   XWPSDeviceColor * pdc, 
	                                   XWPSDevice * dev,
		                                 PSColorSelect select)
{
	float ftemp;
  ushort fgray = unit_frac(pc->paint.values[0], ftemp);
  if (alpha == ps_max_color_value)
		(this->*(cmap_procs->map_gray))(fgray, pdc, dev, select);
  else
		(this->*(cmap_procs->map_rgb_alpha))(fgray, fgray, fgray, cv2frac(alpha), pdc, dev, select);
  return 0;
}

int XWPSImagerState::remapDeviceRGB(XWPSClientColor * pc, 
	                                  XWPSColorSpace * ,
	                                  XWPSDeviceColor * pdc, 
	                                  XWPSDevice * dev,
		                                PSColorSelect select)
{
	float ftemp;
  ushort fred = unit_frac(pc->paint.values[0], ftemp), fgreen = unit_frac(pc->paint.values[1], ftemp);
  ushort fblue = unit_frac(pc->paint.values[2], ftemp);

  if (alpha == ps_max_color_value)
		(this->*(cmap_procs->map_rgb))(fred, fgreen, fblue, pdc, dev, select);
  else
		(this->*(cmap_procs->map_rgb_alpha))(fred, fgreen, fblue, cv2frac(alpha), pdc, dev, select);
  return 0;
}

int XWPSImagerState::remapPattern(XWPSClientColor * pc, 
	                    XWPSColorSpace * pcs,
	                    XWPSDeviceColor * pdc, 
	                    XWPSDevice * dev,
		                  PSColorSelect select)
{
	if (pc->pattern == 0) 
	{
		pdc->setNullPattern();
		return 0;	
  }
  return	pc->pattern->remapColor(this, pc, pcs, pdc, dev, select);
}

void XWPSImagerState::resetLineParams()
{
	line_params.reset();
	flatness = -1;
}

void XWPSImagerState::setCmapProcs(XWPSDevice * dev)
{
	cmap_procs = getCmapProcs(dev);
}

void XWPSImagerState::setCurveJoin(int join)
{
	line_params.setCurveJoin(join);
}

int XWPSImagerState::setDash(const float *patternA, uint lengthA, float offsetA)
{
	return line_params.setDash(patternA, lengthA, offsetA);
}

void XWPSImagerState::setDashAdapt(bool a)
{
	line_params.setDashAdapt(a);
}

int XWPSImagerState::setDotLength(float length, bool absolute)
{
	return line_params.setDotLength(length, absolute);
}

int  XWPSImagerState::setDotOrientation()
{
	if (ctm.isXXYY() || ctm.isXYYX())
		return currentMatrix(&line_params.dot_orientation);
    
  return (int)(XWPSError::RangeCheck);
}

void XWPSImagerState::setEffectiveTransfer()
{
	effective_transfer.copy(&set_transfer);
}

void  XWPSImagerState::setEffectiveXFer()
{
	effective_transfer.copy(&set_transfer);
  if (dev_ht == 0)
		return;	
  
  if (dev_ht->components == 0) 
  {
		XWPSTransferMap *pmap = dev_ht->order.transfer;
		if (pmap != 0)
	    effective_transfer.setIndexed(pmap);
  } 
  else 
  {
		int i;

		for (i = 0; i < 4; ++i) 
		{
	    XWPSTransferMap *pmap = dev_ht->components[dev_ht->color_indices[i]].corder.transfer;

	    if (pmap != 0)
	    {
	    	if (effective_transfer.u.indexed[i] != pmap)
				{
					if (effective_transfer.u.indexed[i])
						if (!(effective_transfer.u.indexed[i]->decRef()))
							delete effective_transfer.u.indexed[i];	
					effective_transfer.u.indexed[i] = pmap;
					pmap->incRef();
				}
			}
		}
  }
}

void XWPSImagerState::setFillAdjust(float adjust_x, float adjust_y)
{
#define CLAMP_TO_HALF(v)\
    ((v) <= 0 ? fixed_0 : (v) >= 0.5 ? fixed_half : float2fixed(v));

  fill_adjust.x = CLAMP_TO_HALF(adjust_x);
  fill_adjust.y = CLAMP_TO_HALF(adjust_y);
   
#undef CLAMP_TO_HALF
}

void XWPSImagerState::setFlat(float flat)
{
	if (flat <= 0.2)
		flat = 0.2;
  else if (flat > 100)
		flat = 100;
  flatness = flat;
}

void XWPSImagerState::setLineCap(PSLineCap cap)
{
	line_params.setLineCap(cap);
}

void XWPSImagerState::setLineJoin(PSLineJoin join)
{
	line_params.setLineJoin(join);
}

void XWPSImagerState::setLineWidth(double width)
{
	line_params.setLineWidth(width);
}

int  XWPSImagerState::setMatrix(XWPSMatrix * pmat)
{
	int code = ctm.updateMatrixFixed(pmat->tx, pmat->ty);
	if (code < 0)
		return code;
		
  ctm = *pmat;
  return 0;
}

void  XWPSImagerState::setMiterLimit(float limit)
{
	line_params.setMiterLimit(limit);
}

int  XWPSImagerState::setScreenPhase(int x, int y, PSColorSelect select)
{
	if (select == ps_color_select_all) 
	{
		int i;

		for (i = 0; i < ps_color_select_count; ++i)
	    setScreenPhase(x, y, (PSColorSelect) i);
		return 0;
  } 
  else if (select < 0 || select >= ps_color_select_count)
		return (int)(XWPSError::RangeCheck);
  screen_phase[select].x = x;
  screen_phase[select].y = y;
  return 0;
}

int  XWPSImagerState::strokeAddImager(XWPSPath *ppath, 
	                     XWPSPath *to_path,
		                   XWPSDevice *dev)
{
	XWPSStrokeParams params;
	params.flatness = flatness;
	return dev->strokePathOnly(ppath, to_path, this, &params, NULL, NULL);
}

int  XWPSImagerState::strokePathExpansion(XWPSPath * ppath, XWPSFixedPoint * ppt)
{
	XWPSSubpath *psub = ppath->firstSubpath();
  XWPSSegment *pseg;
  double cx = fabs(ctm.xx) + fabs(ctm.yx);
  double cy = fabs(ctm.xy) + fabs(ctm.yy);
  double expand = line_params.half_width;
  int result = 1;
  
  if (is_fzero2(ctm.xy, ctm.yx) || is_fzero2(ctm.xx, ctm.yy)) 
  {
		bool must_be_closed = !(line_params.cap == ps_cap_square || line_params.cap == ps_cap_round);
		XWPSFixedPoint prev;

		for (pseg = psub; pseg;  prev = pseg->pt, pseg = pseg->next)
	    switch (pseg->type) 
	    {
	    	case s_start:
					if (((XWPSSubpath *)pseg)->curve_count || (must_be_closed && !((XWPSSubpath *)pseg)->is_closed))
		    		goto not_exact;
					break;
					
	    	case s_line:
	    	case s_line_close:
					if (!(pseg->pt.x == prev.x || pseg->pt.y == prev.y))
		    		goto not_exact;
					break;
					
	    	default:
					goto not_exact;
	  }
		result = 0;
  }
  
not_exact:
  if (result) 
  {
		if (!ppath->hasCurves() && ppath->subpathCount() <= 1 &&
	    (psub == 0 || (pseg = psub->next) == 0 ||
	     (pseg = pseg->next) == 0 || pseg->type == s_line_close))
	    ;
		else 
		{
	    float factor = joinExpansionFactor(line_params.join);
	    if (line_params.curve_join >= 0)
				factor = qMax(factor, joinExpansionFactor((PSLineJoin)line_params.curve_join));
	    	expand *= factor;
		}
  }
  
  {
		float exx = expand * cx;
		float exy = expand * cy;
		int code = set_float2fixed_vars(ppt->x, exx);

		if (code < 0)
	    return code;
		code = set_float2fixed_vars(ppt->y, exy);
		if (code < 0)
	    return code;
  }

  return result;
}

void XWPSImagerState::makeBitmap(XWPSStripBitmap * pbm, 
	                XWPSDeviceMem * mdev,
	                ulong id)
{
	pbm->data = mdev->base;
	mdev->foreign_bits = true;
  pbm->raster = mdev->raster;
  pbm->rep_width = pbm->size.x = mdev->width;
  pbm->rep_height = pbm->size.y = mdev->height;
  pbm->id = id;
  pbm->rep_shift = pbm->shift = 0;
}

#define map4tox(z,a,b,c,d)\
    z, z^a, z^b, z^(a+b),\
    z^c, z^(a+c), z^(b+c), z^(a+b+c),\
    z^d, z^(a+d), z^(b+d), z^(a+b+d),\
    z^(c+d), z^(a+c+d), z^(b+c+d), z^(a+b+c+d)

#ifdef __STDC__
#  define n0L 0xffffffffU
#  define ffL8 0x0000ff00U
#  define ffL16 0x00ff0000U
#  define ffL24 0xff000000U
#else
#if arch_sizeof_long == 4
/*
 * The compiler evaluates long expressions mod 2^32.  Even very picky
 * compilers allow assigning signed longs to unsigned longs, so we use
 * signed constants.
 */
#  define n0L (-1)
#  define ffL8 0x0000ff00
#  define ffL16 0x00ff0000
#  define ffL24 (-0x01000000)
#else
/*
 * The compiler evaluates long expressions mod 2^64.
 */
#  define n0L 0xffffffffL
#  define ffL8 0x0000ff00L
#  define ffL16 0x00ff0000L
#  define ffL24 0xff000000L
#endif
#endif
#if arch_is_big_endian
const ulong lookup4x1to32_identity[16] = {
    map4tox(0, 0xff, ffL8, ffL16, ffL24)
};
const ulong lookup4x1to32_inverted[16] = {
    map4tox(n0L, 0xff, ffL8, ffL16, ffL24)
};
#else /* !arch_is_big_endian */
const ulong lookup4x1to32_identity[16] = {
    map4tox(0, ffL24, ffL16, ffL8, 0xff)
};
const ulong lookup4x1to32_inverted[16] = {
    map4tox(n0L, ffL24, ffL16, ffL8, 0xff)
};
#endif
#undef n0L
#undef ffL8
#undef ffL16
#undef ffL24

#define PI_ImageMatrix 0x001
#define PI_BPC_SHIFT 1
#define PI_BPC_MASK 0xf
#define PI_FORMAT_SHIFT 5
#define PI_FORMAT_MASK 0x3
#define PI_Decode 0x080
#define PI_Interpolate 0x100
#define PI_CombineWithColor 0x200
#define PI_BITS 10

#define DECODE_DEFAULT(i, dd1)\
  ((i) == 1 ? dd1 : (i) & 1)

ps_image_class_t ps_image_class_table [] = {
	&XWPSImageEnum::imageClass0Interpolate,
	&XWPSImageEnum::imageClass1Simple,
	&XWPSImageEnum::imageClass2Fracs,
	&XWPSImageEnum::imageClass3Mono,
	&XWPSImageEnum::imageClass4Color
};

const unsigned ps_image_class_table_count = (sizeof(ps_image_class_table) / sizeof(ps_image_class_table[0])) - 1;

static const uchar * sample_unpack_copy(uchar * , 
                                       int *pdata_x, 
                                       const uchar * data, 
                                       int data_x,
		                                    uint , 
		                                    const PSSampleLookup * , 
		                                    int )
{	
  *pdata_x = data_x;
  return data;
}

static const uchar * sample_unpack_1(uchar * bptr, 
                                     int *pdata_x, 
                                     const uchar * data, 
                                     int data_x,
		                                 uint dsize, 
		                                 const PSSampleLookup * ptab, 
		                                 int spread)
{
	const uchar *psrc = data + (data_x >> 3);
  int left = dsize - (data_x >> 3);

  if (spread == 1) 
  {
		ulong *bufp = (ulong *) bptr;
		const ulong *map = &ptab->lookup4x1to32[0];
		uint b;

		if (left & 1) 
		{
	    b = psrc[0];
	    bufp[0] = map[b >> 4];
	    bufp[1] = map[b & 0xf];
	    psrc++, bufp += 2;
		}
		left >>= 1;
		while (left--) 
		{
	    b = psrc[0];
	    bufp[0] = map[b >> 4];
	    bufp[1] = map[b & 0xf];
	    b = psrc[1];
	    bufp[2] = map[b >> 4];
	    bufp[3] = map[b & 0xf];
	    psrc += 2, bufp += 4;
		}
  } 
  else 
  {
		uchar *bufp = bptr;
		const uchar *map = &ptab->lookup8[0];
		while (left--) 
		{
	    uint b = *psrc++;
	    *bufp = map[b >> 7];
	    bufp += spread;
	    *bufp = map[(b >> 6) & 1];
	    bufp += spread;
	    *bufp = map[(b >> 5) & 1];
	    bufp += spread;
	    *bufp = map[(b >> 4) & 1];
	    bufp += spread;
	    *bufp = map[(b >> 3) & 1];
	    bufp += spread;
	    *bufp = map[(b >> 2) & 1];
	    bufp += spread;
	    *bufp = map[(b >> 1) & 1];
	    bufp += spread;
	    *bufp = map[b & 1];
	    bufp += spread;
		}
  }
  *pdata_x = data_x & 7;
  return bptr;
}

static const uchar * sample_unpack_2(uchar * bptr, 
                                     int *pdata_x, 
                                     const uchar * data, 
                                     int data_x,
		                                 uint dsize, 
		                                 const PSSampleLookup * ptab, 
		                                 int spread)
{
  const uchar *psrc = data + (data_x >> 2);
  int left = dsize - (data_x >> 2);

  if (spread == 1) 
  {
		ushort *bufp = (ushort *) bptr;
		const ushort *map = &ptab->lookup2x2to16[0];

		while (left--) 
		{
	    uint b = *psrc++;

	    *bufp++ = map[b >> 4];
	    *bufp++ = map[b & 0xf];
		}
  } 
  else 
  {
		uchar *bufp = bptr;
		const uchar *map = &ptab->lookup8[0];
		while (left--) 
		{
	    unsigned b = *psrc++;
	    *bufp = map[b >> 6];
	    bufp += spread;
	    *bufp = map[(b >> 4) & 3];
	    bufp += spread;
	    *bufp = map[(b >> 2) & 3];
	    bufp += spread;
	    *bufp = map[b & 3];
	    bufp += spread;
		}
  }
  *pdata_x = data_x & 3;
  return bptr;
}

static const uchar * sample_unpack_4(uchar * bptr, 
                                     int *pdata_x, 
                                     const uchar * data, 
                                     int data_x,
		                                 uint dsize, 
		                                 const PSSampleLookup * ptab, 
		                                 int spread)
{
  uchar *bufp = bptr;
  const uchar *psrc = data + (data_x >> 1);
  int left = dsize - (data_x >> 1);
  const uchar *map = &ptab->lookup8[0];

  while (left--) 
  {
		uint b = *psrc++;

		*bufp = map[b >> 4];
		bufp += spread;
		*bufp = map[b & 0xf];
		bufp += spread;
  }
  *pdata_x = data_x & 1;
  return bptr;
}

const uchar *
sample_unpack_8(uchar * bptr, int *pdata_x, const uchar * data, int data_x,
		uint dsize, const PSSampleLookup * ptab, int spread)
{
    uchar *bufp = bptr;
    const uchar *psrc = data + data_x;

    *pdata_x = 0;
    if (spread == 1) 
    {
			if (ptab->lookup8[0] != 0 || 	ptab->lookup8[255] != 255) 
			{
	    	uint left = dsize - data_x;
	    	const uchar *map = &ptab->lookup8[0];

	    	while (left--)
					*bufp++ = map[*psrc++];
			} 
			else 
			{	
	    	return psrc;
			}
    } 
    else 
    {
			int left = dsize - data_x;
			const uchar *map = &ptab->lookup8[0];

			for (; left--; psrc++, bufp += spread)
	    	*bufp = map[*psrc];
    }
    return bptr;
}

static const uchar *
sample_unpack_12(uchar * bptr, int *pdata_x, const uchar * data,
		 int data_x, uint dsize, const PSSampleLookup * ,
		 int spread)
{
    short *bufp = (short *) bptr;
    uint dskip = (data_x >> 1) * 3;
    const uchar *psrc = data + dskip;
#define inc_bufp(bp, n) bp = (short *)((uchar *)(bp) + (n))
    uint sample;
    int left = dsize - dskip;

    if ((data_x & 1) && left > 0)
			switch (left) 
			{
	    	default:
					sample = ((uint) (psrc[1] & 0xf) << 8) + psrc[2];
					*bufp = bits2frac(sample, 12);
					inc_bufp(bufp, spread);
					psrc += 3;
					left -= 3;
					break;
					
	    	case 2:		/* xxxxxxxx xxxxdddd */
					*bufp = (psrc[1] & 0xf) * (frac_1 / 15);
	    	case 1:		/* xxxxxxxx */
					left = 0;
			}
    while (left >= 3) 
    {
			sample = ((uint) * psrc << 4) + (psrc[1] >> 4);
			*bufp = bits2frac(sample, 12);
			inc_bufp(bufp, spread);
			sample = ((uint) (psrc[1] & 0xf) << 8) + psrc[2];
			*bufp = bits2frac(sample, 12);
			inc_bufp(bufp, spread);
			psrc += 3;
			left -= 3;
    }
    /* Handle trailing bytes. */
    switch (left) 
    {
			case 2:		/* dddddddd ddddxxxx */
	    	sample = ((uint) * psrc << 4) + (psrc[1] >> 4);
	    	*bufp = bits2frac(sample, 12);
	    	inc_bufp(bufp, spread);
	    	*bufp = (psrc[1] & 0xf) * (frac_1 / 15);
	    	break;
	    	
			case 1:		/* dddddddd */
	    	sample = (uint) * psrc << 4;
	    	*bufp = bits2frac(sample, 12);
	    	break;
	    	
			case 0:		/* Nothing more to do. */
	    	;
    }
    *pdata_x = 0;
    return bptr;
}

const sample_unpack_proc_t sample_unpack_12_proc = sample_unpack_12;

XWPSImageCommon::XWPSImageCommon()
{
	type.begin_typed_image_.begin_typed_image = 0;
	type.source_size_.source_size = 0;
	type.sput_.sput = &XWPSImageCommon::sputNo;
	type.sget_.sget = &XWPSImageCommon::sgetNo;
	type.index = 0;
}

int  XWPSImageCommon::beginTyped(XWPSState * pgs,  
	                               bool uses_color, 
	                               XWPSImageEnumCommon ** ppie)
{
	XWPSDevice *dev = pgs->currentDevice();
  XWPSClipPath *pcpath;
  int code = pgs->effectiveClipPath(&pcpath);

  if (code < 0)
		return code;
  if (uses_color)
		pgs->setDevColor();
  return dev->beginTypedImage(pgs,NULL, this, NULL, pgs->dev_color, pcpath, ppie);
}

int  XWPSImageCommon::beginTypedImage(XWPSDevice * dev,
		                   XWPSImagerState * pis, 
		                   XWPSMatrix * pmat,
		                   XWPSIntRect * prect,
	                     XWPSDeviceColor * pdcolor, 
	                     XWPSClipPath * pcpath,
		                   XWPSImageEnumCommon **pinfo)
{
	return (this->*(type.begin_typed_image_.begin_typed_image))(dev, pis, pmat, prect, pdcolor, pcpath, pinfo);
}

void XWPSImageCommon::commonInit()
{
	ImageMatrix.reset();
}

int XWPSImageCommon::sget(XWPSStream *s,	 XWPSColorSpace *cs)
{
	return (this->*(type.sget_.sget))(s, cs);
}

int XWPSImageCommon::sgetNo(XWPSStream *,	 XWPSColorSpace *)
{
	return (int)(XWPSError::RangeCheck);
}

int XWPSImageCommon::sourceSize(XWPSImagerState * pis, XWPSIntPoint * psize)
{
	return (this->*(type.source_size_.source_size))(pis, psize);
}

int XWPSImageCommon::sput(XWPSStream *s, XWPSColorSpace **cs)
{
	return (this->*(type.sput_.sput))(s, cs);
}

int XWPSImageCommon::sputNo(XWPSStream *, XWPSColorSpace **)
{
	return (int)(XWPSError::RangeCheck);
}

XWPSDataImage::XWPSDataImage()
	:XWPSImageCommon()
{
	type.source_size_.source_sized = &XWPSDataImage::dataSourceSize;
	Width = Height = 0;
  BitsPerComponent = 0;
  Interpolate = false;
}

void XWPSDataImage::dataInit(int num_components)
{
	commonInit();
  Width = Height = 0;
  BitsPerComponent = 1;
  if (num_components >= 0) 
  {
		for (int i = 0; i < num_components * 2; i += 2)
	    Decode[i] = 0, Decode[i + 1] = 1;
  } 
  else 
  {
		for (int i = 0; i < num_components * -2; i += 2)
	    Decode[i] = 1, Decode[i + 1] = 0;
  }
  Interpolate = false;
}

int XWPSDataImage::dataSourceSize(XWPSImagerState * , XWPSIntPoint * psize)
{
	psize->x = Width;
  psize->y = Height;
  return 0;
}

bool XWPSDataImage::matrixIsDefault()
{
	 return (ImageMatrix.isXXYY() &&
	    		 ImageMatrix.xx == Width &&
	    		 ImageMatrix.yy == -Height &&
	    		 is_fzero(ImageMatrix.tx) &&
	    		 ImageMatrix.ty == Height);
}

void XWPSDataImage::matrixSetDefault()
{
	ImageMatrix.xx = Width;
  ImageMatrix.xy = 0;
  ImageMatrix.yx = 0;
  ImageMatrix.yy = -Height;
  ImageMatrix.tx = 0;
  ImageMatrix.ty = Height;
}

XWPSPixelImage::XWPSPixelImage()
	:XWPSDataImage()
{
	type.sput_.sputp = 0;
	type.sget_.sgetp = &XWPSPixelImage::pixelImageSGet;
	format = ps_image_format_chunky;
	ColorSpace = 0;
	CombineWithColor = false;
}

void XWPSPixelImage::pixelInit(XWPSColorSpace * color_space)
{
	int num_components;
   if (color_space == 0 || (num_components = color_space->numComponents()) < 0)
		num_components = 0;
  dataInit(num_components);
  format = ps_image_format_chunky;
  ColorSpace = color_space;
  CombineWithColor = false;
}

int  XWPSPixelImage::pixelImageSGet(XWPSStream *s, XWPSColorSpace *pcs)
{
	uint control;
  float decode_default_1 = 1;
  int num_components, num_decode;
  int code;
  uint ignore;

  if ((code = s->getVariableUint(&control)) < 0 ||
				(code = s->getVariableUint((uint *)&Width)) < 0 ||
				(code = s->getVariableUint((uint *)&Height)) < 0)
		return code;
  
  if (control & PI_ImageMatrix) 
  {
		if ((code = s->getMatrix(&ImageMatrix)) < 0)
	    return code;
  } 
  else
		matrixSetDefault();
  BitsPerComponent = ((control >> PI_BPC_SHIFT) & PI_BPC_MASK) + 1;
  format = (PSImageFormat)((control >> PI_FORMAT_SHIFT) & PI_FORMAT_MASK);
  ColorSpace = pcs;
  num_components = pcs->numComponents();
  num_decode = num_components * 2;
  if (pcs->getIndex() == ps_color_space_index_Indexed)
		decode_default_1 = pcs->params.indexed->hival;
  if (control & PI_Decode) 
  {
		uint dflags = 0x10000;
		float *dp = Decode;

		for (int i = 0; i < num_decode; i += 2, dp += 2, dflags <<= 2) 
		{
	    if (dflags >= 0x10000) 
	    {
				dflags = s->getc() + 0x100;
				if (dflags < 0x100)
		    	return (int)(XWPSError::IOError);
	    }
	    switch (dflags & 0xc0) 
	    {
	    	case 0x00:
					dp[0] = 0, dp[1] = DECODE_DEFAULT(i + 1, decode_default_1);
					break;
					
	    	case 0x40:
					dp[0] = DECODE_DEFAULT(i + 1, decode_default_1), dp[1] = 0;
					break;
					
	    	case 0x80:
					dp[0] = 0;
					if (s->gets((uchar *)(dp + 1), sizeof(float), &ignore) < 0)
		    		return (int)(XWPSError::IOError);
					break;
					
	    	case 0xc0:
					if (s->gets((uchar *)dp, sizeof(float) * 2, &ignore) < 0)
		    		return (int)(XWPSError::IOError);
					break;
	    }
		}
  } 
  else 
  {
    for (int i = 0; i < num_decode; ++i)
	   	Decode[i] = DECODE_DEFAULT(i, decode_default_1);
  }
  Interpolate = (control & PI_Interpolate) != 0;
  CombineWithColor = (control & PI_CombineWithColor) != 0;
  return control >> PI_BITS;
}

int  XWPSPixelImage::pixelImageSPut(XWPSStream *s,  XWPSColorSpace **ppcs, int extra)
{
	XWPSColorSpace *pcs = ColorSpace;
  int bpc = BitsPerComponent;
  int num_components = pcs->numComponents();
  int num_decode;
  uint control = extra << PI_BITS;
  float decode_default_1 = 1;
  uint ignore;

    if (!matrixIsDefault())
		control |= PI_ImageMatrix;
  switch (format) 
  {
    case ps_image_format_chunky:
    case ps_image_format_component_planar:
			switch (bpc) 
			{
				case 1: 
				case 2: 
				case 4: 
				case 8: 
				case 12: 
					break;
					
				default: 
					return (int)(XWPSError::RangeCheck);
			}
			break;
			
    case ps_image_format_bit_planar:
			if (bpc < 1 || bpc > 8)
	    	return (int)(XWPSError::RangeCheck);
  }
  control |= (bpc - 1) << PI_BPC_SHIFT;
  control |= format << PI_FORMAT_SHIFT;
  num_decode = num_components * 2;
  if (pcs->getIndex() == ps_color_space_index_Indexed)
	decode_default_1 = pcs->params.indexed->hival;
  for (int i = 0; i < num_decode; ++i)
		if (Decode[i] != DECODE_DEFAULT(i, decode_default_1)) 
		{
	    control |= PI_Decode;
	    break;
		}
  if (Interpolate)
		control |= PI_Interpolate;
  if (CombineWithColor)
		control |= PI_CombineWithColor;

  s->putVariableUint(control);
  s->putVariableUint((uint)Width);
  s->putVariableUint((uint)Height);
  if (control & PI_ImageMatrix) 
  {
		s->putMatrix(&ImageMatrix);
  }
  if (control & PI_Decode) 
  {
		uint dflags = 1;
		float decode[8];
		int di = 0;

		for (int i = 0; i < num_decode; i += 2) 
		{
	    float u = Decode[i], v = Decode[i + 1];
	    float dv = DECODE_DEFAULT(i + 1, decode_default_1);

	    if (dflags >= 0x100) 
	    {
				s->putc(dflags & 0xff);
				s->puts((const uchar *)decode, di * sizeof(float), &ignore);
				dflags = 1;
				di = 0;
	    }
	    dflags <<= 2;
	    if (u == 0 && v == dv)
				;
	    else if (u == dv && v == 0)
				dflags += 1;
	    else 
	    {
				if (u != 0) 
				{
		    	dflags++;
		    	decode[di++] = u;
				}
				dflags += 2;
				decode[di++] = v;
	    }
		}
		s->putc((dflags << (8 - num_decode)) & 0xff);
		s->puts((const uchar *)decode, di * sizeof(float), &ignore);
  }
  *ppcs = pcs;
  return 0;
}

XWPSImage1::XWPSImage1()
	:XWPSPixelImage()
{
	type.begin_typed_image_.begin_typed_image1 = &XWPSImage1::beginImage1;
	type.sput_.sput1 = &XWPSImage1::image1SPut;
	type.sget_.sget1 = &XWPSImage1::image1SGet;
	type.index = 1;
	ImageMask = false;
	adjust = false;
	Alpha = ps_image_alpha_none;
}

int  XWPSImage1::beginImage1(XWPSDevice * dev,
									 XWPSImagerState * pis, 
									 XWPSMatrix * pmat,
									 XWPSIntRect * prect,
									 XWPSDeviceColor * pdcolor, 
									 XWPSClipPath * pcpath,
									 XWPSImageEnumCommon ** pinfo)
{
	if (Width < 0 || Height < 0)
		return (int)(XWPSError::RangeCheck);
	XWPSImageEnum *penum = new XWPSImageEnum(this, prect);
  penum->alpha = Alpha;
  penum->use_mask_color = false;
  penum->masked = ImageMask;
  penum->adjust = (ImageMask && adjust ? float2fixed(0.25) : fixed_0);
  penum->image1beginImage(dev, pis, pmat, this, pdcolor, pcpath);
  *pinfo = penum;
  return 0;
}

void XWPSImage1::initAdjust(XWPSColorSpace * color_space, bool adjustA)
{
	pixelInit(color_space);
  ImageMask = (color_space == NULL);
  adjust = adjustA;
  if (ImageMask)
  {
		type.sput_.sput1 = &XWPSImage1::image1MaskSPut;
		type.sget_.sget1 = &XWPSImage1::image1MaskSGet;
  }
  else
  {
  	type.sput_.sput1 = &XWPSImage1::image1SPut;
		type.sget_.sget1 = &XWPSImage1::image1SGet;		
  }
  Alpha = ps_image_alpha_none;
}

void XWPSImage1::initMask(bool write_1s)
{
	initMaskAdjust(write_1s, true);
}

void XWPSImage1::initMaskAdjust(bool write_1s, bool adjustA)
{
	initAdjust(NULL, true);
	if (write_1s)
		Decode[0] = 1, Decode[1] = 0;
  else
		Decode[0] = 0, Decode[1] = 1;
  adjust = adjustA;
}

int  XWPSImage1::image1MaskSGet(XWPSStream *s, XWPSColorSpace *)
{
	int code;
  uint control;

  if ((code = s->getVariableUint(&control)) < 0)
		return code;
  initMask((control & MI_Decode) != 0);
  if ((code = s->getVariableUint((uint *)&Width)) < 0 ||
			(code = s->getVariableUint((uint *)&Height)) < 0)
		return code;
  if (control & MI_ImageMatrix) 
  {
		if ((code = s->getMatrix(&ImageMatrix)) < 0)
	    return code;
  } 
  else
		matrixSetDefault();
  Interpolate = (control & MI_Interpolate) != 0;
  adjust = (control & MI_adjust) != 0;
  Alpha = (PSImageAlpha)((control >> MI_Alpha_SHIFT) & MI_Alpha_MASK);
  BitsPerComponent = ((control >> MI_BPC_SHIFT) & MI_BPC_MASK) + 1;
  return 0;
}

int  XWPSImage1::image1MaskSPut(XWPSStream *s,  XWPSColorSpace **)
{
	uint control =(matrixIsDefault() ? 0 : 
		            MI_ImageMatrix) |	(Decode[0] != 0 ? MI_Decode : 0) |
	             (Interpolate ? MI_Interpolate : 0) |	(adjust ? MI_adjust : 0) |
	             (Alpha << MI_Alpha_SHIFT) | ((BitsPerComponent - 1) << MI_BPC_SHIFT);

  s->putVariableUint(control);
  s->putVariableUint((uint)Width);
  s->putVariableUint((uint)Height);
  if (control & MI_ImageMatrix)
		s->putMatrix(&ImageMatrix);
  return 0;
}

int  XWPSImage1::image1SGet(XWPSStream *s,  XWPSColorSpace *pcs)
{
	int code = pixelImageSGet(s, pcs);
  if (code < 0)
		return code;
  type.sput_.sput1 = &XWPSImage1::image1SPut;
	type.sget_.sget1 = &XWPSImage1::image1SGet;		
  ImageMask = false;
  Alpha = (PSImageAlpha)code;
  return 0;
}

int XWPSImage1::image1SPut(XWPSStream *s, XWPSColorSpace **ppcs)
{
	return pixelImageSPut(s, ppcs, (int)Alpha);
}

void XWPSImage1::makeBitmapImage(int x, int y, int w, int h)
{
	Width = w;
  Height = h;
  ImageMatrix.makeBitmapMatrix(x, y, w, h, h);
}

XWPSImage1 & XWPSImage1::operator=(XWPSImage1 & other)
{
	ImageMatrix = other.ImageMatrix;
	Width = other.Width;
	Height = other.Height;
	BitsPerComponent = other.BitsPerComponent;
	memcpy(Decode, other.Decode, (PS_IMAGE_MAX_COMPONENTS * 2)*sizeof(float));
	Interpolate = other.Interpolate;
	format = other.format;
	ColorSpace = other.ColorSpace;
	CombineWithColor = other.CombineWithColor;
	ImageMask = other.ImageMask;
	adjust = other.adjust;
	Alpha = other.Alpha;
	return *this;
}

XWPSImage2::XWPSImage2()
	:XWPSImageCommon()
{
	type.begin_typed_image_.begin_typed_image2 = &XWPSImage2::beginImage2;
	type.source_size_.source_size2 = &XWPSImage2::image2SourceSize;
	type.sput_.sput = &XWPSImageCommon::sputNo;
	type.sget_.sget = &XWPSImageCommon::sgetNo;
	type.index = 2;
  UnpaintedPath = 0;
  PixelCopy = false;
}

XWPSImage2::~XWPSImage2()
{
	if (UnpaintedPath)
	{
		delete UnpaintedPath;
		UnpaintedPath = 0;
	}
}

int XWPSImage2::beginImage2(XWPSDevice * dev,
		              XWPSImagerState * pis, 
		              XWPSMatrix * pmat,
		              XWPSIntRect * ,
	                XWPSDeviceColor * pdcolor, 
	                XWPSClipPath * pcpath,
		              XWPSImageEnumCommon ** )
{
	XWPSState *pgs = DataSource;
  XWPSDevice *sdev = pgs->currentDevice();
  int depth = sdev->color_info.depth;

  bool pixel_copy = PixelCopy && depth <= 8 && !memcmp(&dev->color_info, &sdev->color_info, sizeof(dev->color_info));
  bool has_alpha;
  bool direct_copy;
  XWPSimage2Data idata;
  uchar *row;
  uint row_size, source_size;
  XWPSImageEnumCommon *info;
  XWPSMatrix smat, dmat;
  XWPSColorSpace cs;
  cs.i_ctx_p = dev->context_state;
  XWPSColorSpace *pcs;
  int code;

  idata.image.initAdjust(pis->deviceRGB(), true);
  idata.image.Decode[6] = idata.image.Decode[8] = 0.0;
  idata.image.Decode[7] = idata.image.Decode[9] = 1.0;
  if (pmat == 0) 
  {
		pgs->currentMatrix(&dmat);
		pmat = &dmat;
  } 
  else
		dmat = *pmat;
    
  pgs->currentMatrix(&smat);
  code = image2SetData(&idata);
  if (code < 0)
		return code;
		
  if (idata.bbox.p.x != floor(idata.origin.x))
	  return (int)(XWPSError::RangeCheck);
    
 	if (!(idata.bbox.p.y == floor(idata.origin.y) ||  idata.bbox.q.y == ceil(idata.origin.y)))
	  return (int)(XWPSError::RangeCheck);
    
  source_size = (idata.image.Width * depth + 7) >> 3;
  row_size = qMax((uint)(3 * idata.image.Width), source_size);
  row = new uchar[row_size];
  if (pixel_copy &&	(pcpath == NULL ||
	 		pcpath->includesRectangle(int2fixed(idata.bbox.p.x), int2fixed(idata.bbox.p.y),
				     int2fixed(idata.bbox.q.x), int2fixed(idata.bbox.q.y)))) 
	{
		XWPSMatrix mat;

		idata.image.BitsPerComponent = depth;
		cs.initDevicePixel(depth);
		pcs = &cs;
		mat.matrixMultiply(&idata.image.ImageMatrix, &smat);
		direct_copy = (dmat.isXXYY() || dmat.isXYYX()) &&
#define eqe(e) mat.e == dmat.e
	    eqe(xx) && eqe(xy) && eqe(yx) && eqe(yy);
#undef eqe
			has_alpha = false;
  } 
  else 
  {
		pixel_copy = false;
		idata.image.BitsPerComponent = 8;
		pcs = pis->deviceRGB();
		direct_copy = false;
		{
	    ulong trans_black = sdev->mapRGBAlphaColor(0, 0, 0, 0);

	    has_alpha =	trans_black != sdev->mapRGBAlphaColor(0, 0, 0, ps_max_color_value) &&
									trans_black != sdev->mapRGBAlphaColor(ps_max_color_value, ps_max_color_value,
		 													ps_max_color_value, ps_max_color_value);
		}
  }
  idata.image.ColorSpace = pcs;
  idata.image.Alpha =	(has_alpha ? ps_image_alpha_last : ps_image_alpha_none);
  if (smat.yy < 0) 
  {
		idata.image.ImageMatrix.ty += idata.image.Height *  idata.image.ImageMatrix.yy;
		idata.image.ImageMatrix.xy = -idata.image.ImageMatrix.xy;
		idata.image.ImageMatrix.yy = -idata.image.ImageMatrix.yy;
  }
  if (!direct_copy)
		code = dev->beginTypedImage(pis, pmat, &idata.image, NULL, pdcolor, pcpath, &info);
  if (code >= 0) 
  {
		int y;
		XWPSIntRect rect;
		XWPSGetBitsParams params;
		uchar *data;
		uint offset = row_size - source_size;

		rect = idata.bbox;
		for (y = 0; code >= 0 && y < idata.image.Height; ++y) 
		{
	    XWPSIntRect *unread = 0;
	    int num_unread;

	    rect.q.y = rect.p.y + 1;
	    params.options = GB_ALIGN_ANY | (GB_RETURN_COPY | GB_RETURN_POINTER) |
												GB_OFFSET_0 | (GB_RASTER_STANDARD | GB_RASTER_ANY) |	GB_PACKING_CHUNKY;
	   	if (pixel_copy) 
	   	{
				params.options |= GB_COLORS_NATIVE;
				params.data[0] = row + offset;
				code = sdev->getBitsRectangle(&rect, &params, &unread);
				if (code < 0)
		    	break;
				num_unread = code;
				data = params.data[0];
				if (direct_copy) 
				{
		    	code = dev->copyColor(data, 0, row_size, ps_no_bitmap_id,
			 						(int)(dmat.tx - idata.image.ImageMatrix.tx),
			 						(int)(dmat.ty - idata.image.ImageMatrix.ty + (dmat.yy < 0 ? ~y : y)), idata.image.Width, 1);
		    	continue;
				}
	    } 
	    else 
	    {
				params.options |= GB_COLORS_RGB | GB_DEPTH_8 | (has_alpha ? GB_ALPHA_LAST : GB_ALPHA_NONE);
				params.data[0] = row;
				code = sdev->getBitsRectangle(&rect, &params, &unread);
				if (code < 0)
		    	break;
				num_unread = code;
				data = params.data[0];
	    }
	    if (num_unread > 0 && UnpaintedPath) 
	    {
				int i;

				for (i = 0; code >= 0 && i < num_unread; ++i)
		    	code = UnpaintedPath->addRectangle(int2fixed(unread[i].p.x),
						 										int2fixed(unread[i].p.y),	int2fixed(unread[i].q.x), int2fixed(unread[i].q.y));
				if (unread)
				{
					delete unread;
					unread = 0;
				}
	    }
	    code = info->imageData((const uchar**)&data, 0, row_size, 1);
	    rect.p.y = rect.q.y;
		}
		if (!direct_copy) 
		{
	    if (code >= 0)
				code = info->end(true);
	    else
				info->end(false);
		}
  }
  delete [] row;
  return (code < 0 ? code : 1);
}

int XWPSImage2::image2SetData(XWPSimage2Data * pid)
{
	XWPSState *pgs = DataSource;
  XWPSMatrix smat;
  XWPSRect sbox, dbox;

  pgs->transform(XOrigin, YOrigin, &pid->origin);
  sbox.q.x = (sbox.p.x = XOrigin) + Width;
  sbox.q.y = (sbox.p.y = YOrigin) + Height;
  pgs->currentMatrix(&smat);
  sbox.bboxTransform(&smat, &dbox);
  pid->bbox.p.x = (int)floor(dbox.p.x);
  pid->bbox.p.y = (int)floor(dbox.p.y);
  pid->bbox.q.x = (int)ceil(dbox.q.x);
  pid->bbox.q.y = (int)ceil(dbox.q.y);
  pid->image.Width = pid->bbox.q.x - pid->bbox.p.x;
  pid->image.Height = pid->bbox.q.y - pid->bbox.p.y;
  pid->image.ImageMatrix = ImageMatrix;
  return 0;
}

int XWPSImage2::image2SourceSize(XWPSImagerState * , XWPSIntPoint * psize)
{
	XWPSimage2Data idata;

  image2SetData(&idata);
  psize->x = idata.image.Width;
  psize->y = idata.image.Height;
  return 0;
}

static int make_mid_default(XWPSDevice **pmidev, XWPSDevice *dev, int width, int height)
{
	XWPSDeviceMem *midev = new XWPSDeviceMem;
	midev->context_state = dev->context_state;
	midev->makeMemMonoDevice(NULL);
  midev->width = width;
  midev->height = height;
  int code = midev->open();
  if (code < 0)
  {
  	delete midev;
  	return code;
  }
  
  midev->is_open = true;
  midev->fillRectangle(0, 0, width, height, 0);
  *pmidev = midev;
  return 0;
}

static int make_mcde_default(XWPSDevice *dev, 
                             XWPSImagerState *pis,
		                         XWPSMatrix *pmat, 
		                         XWPSImageCommon *pic,
		                         XWPSIntRect *prect, 
		                         XWPSDeviceColor *pdcolor,
		                         XWPSClipPath *pcpath, 
		                         XWPSImageEnumCommon **pinfo,
		                         XWPSDevice **pmcdev, 
		                         XWPSDevice *midev,
		                         XWPSImageEnumCommon *,
		                         XWPSIntPoint *origin)
{
	XWPSDeviceMem * mdev = (XWPSDeviceMem *)midev;
  XWPSDeviceMaskClip *mcdev = new XWPSDeviceMaskClip;
  mcdev->context_state = dev->context_state;
  XWPSStripBitmap bits;
  int code;
  bits.data = mdev->base;
  bits.raster = mdev->raster;
  bits.size.x = mdev->width;
  bits.size.y = mdev->height;
  bits.id = ps_no_bitmap_id;
  code = mcdev->maskClipInitialize(0, &bits, dev, origin->x, origin->y);
  if (code < 0) 
  {
		delete mcdev;
		return code;
  }
  
  mcdev->tiles = bits;
  code = mcdev->beginTypedImage(pis, pmat, pic, prect, pdcolor, pcpath, pinfo);
  if (code < 0) 
  {
		delete mcdev;
		return code;
  }
  *pmcdev = mcdev;
  return 0;
}

static bool
check_image3_extent(float mask_coeff, float data_coeff)
{
    if (mask_coeff == 0)
			return data_coeff == 0;
    if (data_coeff == 0 || (mask_coeff > 0) != (data_coeff > 0))
			return false;
    return true;
}

XWPSImage3::XWPSImage3()
	:XWPSPixelImage()
{
	type.begin_typed_image_.begin_typed_image3 = &XWPSImage3::beginImage3;
	type.sput_.sput = &XWPSImageCommon::sputNo;
	type.sget_.sget = &XWPSImageCommon::sgetNo;
	type.index = 3;
	InterleaveType = interleave_chunky;
}

int XWPSImage3::beginImage3(XWPSDevice * dev,
									 XWPSImagerState * pis, 
									 XWPSMatrix * pmat,
									 XWPSIntRect * prect,
									 XWPSDeviceColor * pdcolor, 
									 XWPSClipPath * pcpath,
									 XWPSImageEnumCommon ** pinfo)
{
	return beginImage3Generic(dev, pis, pmat, prect, pdcolor,
				   								pcpath, make_mid_default, make_mcde_default, pinfo);
}

int  XWPSImage3::beginImage3Generic(XWPSDevice * dev,
			                    XWPSImagerState *pis, 
			                    XWPSMatrix *pmat,
			                    XWPSIntRect *prect,
			                    XWPSDeviceColor *pdcolor,
			                    XWPSClipPath *pcpath, 
			                    image3_make_mid_proc_t make_mid,
			                    image3_make_mcde_proc_t make_mcde,
			                    XWPSImageEnumCommon **pinfo)
{
	XWPSImage3Enum *penum;
  XWPSIntRect mask_rect, data_rect;
  XWPSDevice *mdev = 0;
  XWPSDevice *pcdev = 0;
  XWPSImage1 i_pixel, i_mask;
  XWPSMatrix mi_pixel, mi_mask, mat;
  XWPSRect mrect;
  XWPSIntPoint origin;
  int code;
  
  if (Height <= 0 || MaskDict.Height <= 0)
		return (int)(XWPSError::RangeCheck);
  switch (InterleaveType) 
  {
		default:
	    return (int)(XWPSError::RangeCheck);
	    	
		case interleave_chunky:
	    if (MaskDict.Width != Width ||
					MaskDict.Height != Height ||
					MaskDict.BitsPerComponent != BitsPerComponent ||
					format != ps_image_format_chunky)
				return (int)(XWPSError::RangeCheck);
	    break;
	    
		case interleave_scan_lines:
	    if (MaskDict.Height % Height != 0 && Height % MaskDict.Height != 0)
				return (int)(XWPSError::RangeCheck);
					
		case interleave_separate_source:
	    if (MaskDict.BitsPerComponent != 1)
				return (int)(XWPSError::RangeCheck);
  }
  
  if (!check_image3_extent(ImageMatrix.xx, MaskDict.ImageMatrix.xx) ||
			!check_image3_extent(ImageMatrix.xy, MaskDict.ImageMatrix.xy) ||
			!check_image3_extent(ImageMatrix.yx, MaskDict.ImageMatrix.yx) ||
			!check_image3_extent(ImageMatrix.yy, MaskDict.ImageMatrix.yy))
		return (int)(XWPSError::RangeCheck);
			
	if ((code = mi_pixel.matrixInvert(&ImageMatrix)) < 0 ||
			(code = mi_mask.matrixInvert(&MaskDict.ImageMatrix)) < 0)
		return code;
  if (fabs(mi_pixel.tx - mi_mask.tx) >= 0.5 ||	fabs(mi_pixel.ty - mi_mask.ty) >= 0.5)
		return (int)(XWPSError::RangeCheck);
			
	{
		XWPSPoint ep, em;

		if ((code = ep.transform(Width, Height, &mi_pixel)) < 0 ||
	    	(code = em.transform(MaskDict.Width, MaskDict.Height, &mi_mask)) < 0)
	    return code;
		if (fabs(ep.x - em.x) >= 0.5 || fabs(ep.y - em.y) >= 0.5)
	    return (int)(XWPSError::RangeCheck);
  }
  
  penum = new XWPSImage3Enum;
  penum->num_components = ColorSpace->numComponents();
  penum->commonInit(this,  dev,  1 + penum->num_components, format);
  if (prect) 
  {
		long lmw = MaskDict.Width, lmh = MaskDict.Height;

		data_rect = *prect;
		mask_rect.p.x = (int)(data_rect.p.x * lmw / Width);
		mask_rect.p.y = (int)(data_rect.p.y * lmh / Height);
		mask_rect.q.x = (int)((data_rect.q.x + Width - 1) * lmw / Width);
		mask_rect.q.y = (int)((data_rect.q.y + Height - 1) * lmh / Height);
  } 
  else 
  {
		mask_rect.q.x = MaskDict.Width;
		mask_rect.q.y = MaskDict.Height;
		data_rect.q.x = Width;
		data_rect.q.y = Height;
  }
  penum->mask_width = mask_rect.q.x - mask_rect.p.x;
  penum->mask_height = mask_rect.q.y - mask_rect.p.y;
  penum->mask_full_height = MaskDict.Height;
  penum->pixel_width = data_rect.q.x - data_rect.p.x;
  penum->pixel_height = data_rect.q.y - data_rect.p.y;
  penum->pixel_full_height = Height;
  
  if (InterleaveType == interleave_chunky) 
  {
		penum->pixel_data = new uchar[(penum->pixel_width * BitsPerComponent * penum->num_components + 7) >> 3];
		penum->mask_data = new uchar[(penum->mask_width + 7) >> 3];
  }
  
  penum->InterleaveType = (PSImage3InterleaveType)InterleaveType;
  penum->bpc = BitsPerComponent;
  mrect.p.x = mrect.p.y = 0;
  mrect.q.x = MaskDict.Width;
  mrect.q.y = MaskDict.Height;
  XWPSMatrix ctm;
  pis->ctmOnly(&ctm);
  if (pmat == 0)
		pmat = &ctm;
  if ((code = mat.matrixMultiply(&mi_mask, pmat)) < 0 ||
			(code = mrect.bboxTransform(&mat, &mrect)) < 0)
			return code;
  origin.x = floor(mrect.p.x);
  origin.y = floor(mrect.p.y);
  code = make_mid(&mdev, dev, (int)ceil(mrect.q.x) - origin.x, (int)ceil(mrect.q.y) - origin.y);
  if (code < 0)
		goto out1;
  penum->mdev = mdev;
  i_mask.initMask(false);
  i_mask.adjust = false;
  {
		i_mask.ImageMatrix = MaskDict.ImageMatrix;
		i_mask.Width = MaskDict.Width;
		i_mask.Height = MaskDict.Height;
		i_mask.BitsPerComponent = MaskDict.BitsPerComponent;
		memcpy(i_mask.Decode, MaskDict.Decode, (PS_IMAGE_MAX_COMPONENTS * 2) * sizeof(float));
		i_mask.Interpolate = MaskDict.Interpolate;
		i_mask.BitsPerComponent = 1;
  }
  
  {
		XWPSDeviceColor dcolor;
		XWPSMatrix m_mat;

		dcolor.setPure(1);
		m_mat = *pmat;
		m_mat.tx -= origin.x;
		m_mat.ty -= origin.y;
		code = i_mask.beginTypedImage(mdev, NULL, &m_mat,  &mask_rect, &dcolor, NULL, &penum->mask_info);
		if (code < 0)
	    goto out2;
  }
  
  i_pixel.initAdjust(ColorSpace, true);
  {
  	i_pixel.ImageMatrix = ImageMatrix;
		i_pixel.Width = Width;
		i_pixel.Height = Height;
		i_pixel.BitsPerComponent = BitsPerComponent;
		memcpy(i_pixel.Decode, Decode, (PS_IMAGE_MAX_COMPONENTS * 2) * sizeof(float));
		i_pixel.Interpolate = Interpolate;
		i_pixel.format = format;
		i_pixel.ColorSpace = ColorSpace;
		i_pixel.CombineWithColor = CombineWithColor;
  }
  
  code = make_mcde(dev, pis, pmat, &i_pixel, prect, pdcolor, pcpath, &penum->pixel_info,  &pcdev, mdev, penum->mask_info, &origin);
  if (code < 0)
		goto out3;
  penum->pcdev = pcdev;
  
  switch (InterleaveType)
  {
  	case interleave_chunky:
  		penum->num_planes = 1;
			penum->plane_widths[0] = Width;
			penum->plane_depths[0] =  penum->pixel_info->plane_depths[0] *   (penum->num_components + 1) / penum->num_components;
			break;
			
    case interleave_scan_lines:
    	penum->num_planes = 1;
			penum->plane_depths[0] = 1;
			penum->plane_widths[0] = MaskDict.Width;
			break;
			
    case interleave_separate_source:
    	penum->num_planes = penum->pixel_info->num_planes + 1;
			penum->plane_widths[0] = MaskDict.Width;
			penum->plane_depths[0] = 1;
			memcpy(&penum->plane_widths[1], &penum->pixel_info->plane_widths[0], (penum->num_planes - 1) * sizeof(penum->plane_widths[0]));
			memcpy(&penum->plane_depths[1], &penum->pixel_info->plane_depths[0], (penum->num_planes - 1) * sizeof(penum->plane_depths[0]));
			break;
  }
  
  *pinfo = penum;
  return 0;
  
out3:
	if (penum->mask_info)
		penum->mask_info->end(false);
		
out2:
  mdev->close();
  
out1:
	if (penum)
	{
		delete penum;
		penum = 0;
	}
  return code;
}

void XWPSImage3::image3Init(XWPSColorSpace * color_space,
		 							PSImage3InterleaveType interleave_typeA)
{
	pixelInit(color_space);
	InterleaveType = interleave_typeA;
	dataInit(-1);
}

static bool
is_multiple(int x, int y)
{
    return (x % y == 0 || y % x == 0);
}
static bool
check_image3x_extent(float mask_coeff, float data_coeff)
{
  if (mask_coeff == 0)
		return data_coeff == 0;
  if (data_coeff == 0 || (mask_coeff > 0) != (data_coeff > 0))
		return false;
  return true;
}

static int
make_midx_default(XWPSDevice **pmidev, XWPSDevice *dev, int width, int height, int depth)
{
 	XWPSDeviceMem *midev = new XWPSDeviceMem;
 	midev->context_state = dev->context_state;
  midev->makeMemDevice(depth, 0, NULL);
  midev->width = width;
  midev->height = height;
  int code = midev->open();
  if (code < 0) 
  {
  	delete midev;
		return code;
  }
  midev->is_open = true;
  midev->fillRectangle(0, 0, width, height, 0);
  *pmidev = midev;
  return 0;
}

static int
make_mcdex_default(XWPSDevice *dev, 
                   XWPSImagerState *pis,
		               XWPSMatrix *pmat, 
		               XWPSImageCommon *pic,
		   						 XWPSIntRect *prect, 
		   						 XWPSDeviceColor *pdcolor,
		   						 XWPSClipPath *pcpath, 
		   						 XWPSImageEnumCommon **pinfo,
		   						 XWPSDevice **pmcdev, 
		   						 XWPSDevice **,
		               XWPSImageEnumCommon **,
		   						 XWPSIntPoint * ,
		   						 XWPSImage3X *)
{
  XWPSDeviceBbox *bbdev = new XWPSDeviceBbox;
  bbdev->context_state = dev->context_state;
  int code;

  bbdev->initBbox(dev);
  bbdev->fwdOpenCloseBbox(false);
  code = bbdev->beginTypedImage(pis, pmat, pic, prect, pdcolor, pcpath, pinfo);
  if (code < 0) 
  {
		delete bbdev;
		return code;
  }
  *pmcdev = bbdev;
  return 0;
}

XWPSImage3xMask::XWPSImage3xMask()
{
	InterleaveType = 0;
	memset(Matte, 0, PS_CLIENT_COLOR_MAX_COMPONENTS * sizeof(float));
	has_Matte = false;
	MaskDict.dataInit(1);
}

XWPSImage3XChannelState::XWPSImage3XChannelState()
{
	info = 0;
	mdev = 0;
	InterleaveType = (PSImage3InterleaveType)0;
	width = height = full_height = depth = 0;
	data = 0;
	y = 0;
	skip = 0;
	own_data = false;
}

XWPSImage3XChannelState::~XWPSImage3XChannelState()
{
	if (mdev)
	{
		if (!mdev->decRef())
		  delete mdev;
		mdev = 0;
	}
	
	if (own_data)
	{
		if (data)
			delete [] data;
		data = 0;
		own_data = false;
	}
}

int XWPSImage3XChannelState::channelNext(XWPSImage3XChannelState *pics2)
{
	int h1 = full_height;
  int h2 = pics2->full_height;
  long current = y * (long)h2 - pics2->y * (long)h1;
  return ((current -= h1) >= 0 ? -1 :  current + h2 >= 0 ? 0 : 1);
}

XWPSImage3XChannelValues::~XWPSImage3XChannelValues()
{
}

XWPSImage3X::XWPSImage3X()
	:XWPSPixelImage()
{
	type.begin_typed_image_.begin_typed_image3X = &XWPSImage3X::beginImage3x;
	type.sput_.sput = &XWPSImageCommon::sputNo;
	type.sget_.sget = &XWPSImageCommon::sgetNo;
	type.index = IMAGE3X_IMAGETYPE;
}

int XWPSImage3X::beginImage3x(XWPSDevice * dev,
		               XWPSImagerState * pis, 
		               XWPSMatrix * pmat,
		               XWPSIntRect * prect,
		               XWPSDeviceColor * pdcolor, 
		               XWPSClipPath * pcpath,
		               XWPSImageEnumCommon ** pinfo)
{
	return beginImage3xGeneric(dev, pis, pmat, prect, pdcolor,
				    pcpath, make_midx_default, make_mcdex_default, pinfo);
}

int XWPSImage3X::beginImage3xGeneric(XWPSDevice * dev,
													XWPSImagerState *pis, 
													XWPSMatrix *pmat,
													XWPSIntRect *prect,
													XWPSDeviceColor *pdcolor,
													XWPSClipPath *pcpath, 
													image3x_make_mid_proc_t make_mid,
													image3x_make_mcde_proc_t make_mcde,
													XWPSImageEnumCommon **pinfo)
{
	XWPSImage3XEnum *penum;
  XWPSDevice *pcdev = 0;
  XWPSImage3XChannelValues mask[2], pixel;
  XWPSMatrix mat;
  XWPSDevice *midev[2];
  XWPSImageEnumCommon *minfo[2];
  XWPSIntPoint origin[2];
  int code;
  int i;
  XWPSMatrix ctm;
  
  if (Height <= 0)
		return (int)(XWPSError::RangeCheck);
  penum = new XWPSImage3XEnum;
  if (prect)
		pixel.rect = *prect;
  else 
  {
		pixel.rect.q.x = Width;
		pixel.rect.q.y = Height;
  }
  
  if ((code = pixel.matrix.matrixInvert(&ImageMatrix)) < 0 ||
			(code = pixel.corner.transform(Width, Height, &pixel.matrix)) < 0 ||
			(code = checkImage3xMask(&Opacity, &pixel, &mask[0], &penum->mask[0])) < 0 ||
			(code = checkImage3xMask(&Shape, &pixel, &mask[1],  &penum->mask[1])) < 0) 
	{
		goto out0;
  }
  penum->num_components =	ColorSpace->numComponents();
  penum->commonInit(this, dev, 1 + penum->num_components, format);
  penum->pixel.width = pixel.rect.q.x - pixel.rect.p.x;
  penum->pixel.height = pixel.rect.q.y - pixel.rect.p.y;
  penum->pixel.full_height = Height;
  penum->pixel.y = 0;
  if (penum->mask[0].data || penum->mask[1].data) 
  {
		penum->pixel.data = new uchar[(penum->pixel.width * BitsPerComponent * penum->num_components + 7) >> 3];
		penum->pixel.own_data = true;
  }
  penum->bpc = BitsPerComponent;
  
  pis->ctmOnly(&ctm);
  if (pmat == 0)
		pmat = &ctm;
  for (i = 0; i < NUM_MASKS; ++i)
  {
  	XWPSRect mrect;
		XWPSDevice *mdev;
		XWPSColorSpace *pmcs;
		if (penum->mask[i].depth == 0) 
		{
	    midev[0] = 0;
	    minfo[0] = 0;
	    continue;
		}
		
		pmcs = new XWPSColorSpace;
		pmcs->i_ctx_p = dev->context_state;
		pmcs->initDevicePixel(penum->mask[i].depth);
		mrect.p.x = mrect.p.y = 0;
		mrect.q.x = penum->mask[i].width;
		mrect.q.y = penum->mask[i].height;
		if ((code = mat.matrixMultiply(&mask[i].matrix, pmat)) < 0 ||
	    	(code = mrect.bboxTransform(&mat, &mrect)) < 0)
	  {
	  	delete pmcs;
	  	pmcs = 0;
	    return code;
	  }
		origin[i].x = floor(mrect.p.x);
		origin[i].y = floor(mrect.p.y);
		code = make_mid(&mdev, dev,	(int)ceil(mrect.q.x) - origin[i].x, (int)ceil(mrect.q.y) - origin[i].y,	penum->mask[i].depth);
		if (code < 0)
		{
			delete pmcs;
	  	pmcs = 0;
	    goto out1;
	  }
	  
	  penum->mask[i].mdev = mdev;
		mask[i].image.initAdjust(pis->deviceGray(), true); 
		mask[i].image.ColorSpace = pmcs;
		mask[i].image.adjust = false;
		
		{
	    XWPSImage3xMask *pixm =	(i == 0 ? &Opacity : &Shape);

	    mask[i].image.ImageMatrix = pixm->MaskDict.ImageMatrix;
	    mask[i].image.Width = pixm->MaskDict.Width;
	    mask[i].image.Height = pixm->MaskDict.Height;
	    memcpy(mask[i].image.Decode, pixm->MaskDict.Decode, (PS_IMAGE_MAX_COMPONENTS * 2) * sizeof(float));
	    mask[i].image.Interpolate = pixm->MaskDict.Interpolate;
	    mask[i].image.BitsPerComponent = pixm->MaskDict.BitsPerComponent;
		}
		
		{
	    XWPSMatrix m_mat;
	    m_mat = *pmat;
	    m_mat.tx -= origin[i].x;
	    m_mat.ty -= origin[i].y;
	    code = mdev->beginTypedImage(NULL, &m_mat, &mask[i].image, &mask[i].rect, NULL, NULL, &penum->mask[i].info);
	    if (code < 0)
				goto out2;
		}
		midev[i] = mdev;
		minfo[i] = penum->mask[i].info;
  }
  pixel.image.initAdjust(ColorSpace, true);
  {
  	pixel.image.ImageMatrix = ImageMatrix;
  	pixel.image.Width = Width;
  	pixel.image.Height = Height;
  	pixel.image.BitsPerComponent = BitsPerComponent;
  	memcpy(pixel.image.Decode, Decode, (PS_IMAGE_MAX_COMPONENTS * 2) * sizeof(float));
  	pixel.image.Interpolate = Interpolate;
  	pixel.image.format = format;
  	pixel.image.ColorSpace = ColorSpace;
  	pixel.image.CombineWithColor = CombineWithColor;
  }
  code = make_mcde(dev, pis, pmat, &pixel.image,  prect, pdcolor, pcpath, &penum->pixel.info,  &pcdev, midev, minfo, origin, this);
  if (code < 0)
		goto out3;
    
  penum->pcdev = pcdev;
  
  {
		int added_depth = 0;
		int pi = 0;

		for (i = 0; i < NUM_MASKS; ++i) 
		{
	    if (penum->mask[i].depth == 0)
				continue;
	    switch (penum->mask[i].InterleaveType) 
	    {
	    	case interleave_chunky:
					added_depth += BitsPerComponent;
					break;
					
	    	case interleave_separate_source:
					penum->plane_widths[pi] = penum->mask[i].width;
					penum->plane_depths[pi] = penum->mask[i].depth;
					++pi;
					break;
					
	    	default:
					code = XWPSError::Fatal;
					goto out3;
	  	}
		}
		memcpy(&penum->plane_widths[pi], &penum->pixel.info->plane_widths[0],
		                         penum->pixel.info->num_planes * sizeof(penum->plane_widths[0]));
		memcpy(&penum->plane_depths[pi], &penum->pixel.info->plane_depths[0],
		                         penum->pixel.info->num_planes * sizeof(penum->plane_depths[0]));
		penum->plane_depths[pi] += added_depth;
		penum->num_planes = pi + penum->pixel.info->num_planes;
  }
  
  *pinfo = penum;
   return 0;
   
out3:
  if (penum->mask[1].info)
		penum->mask[1].info->end(false);
    
  if (penum->mask[0].info)
		penum->mask[0].info->end(false);
		
out2:
  if (penum->mask[1].mdev) 
		penum->mask[1].mdev->close();
    
  if (penum->mask[0].mdev) 
		penum->mask[0].mdev->close();
  
out1:
out0:
  delete penum;
  return code;
}

int XWPSImage3X::checkImage3xMask(XWPSImage3xMask *pimm,
		   									XWPSImage3XChannelValues *ppcv,
		                    XWPSImage3XChannelValues *pmcv,
		                    XWPSImage3XChannelState *pmcs)
{
	int mask_width = pimm->MaskDict.Width, mask_height = pimm->MaskDict.Height;
  int code;

  if (pimm->MaskDict.BitsPerComponent == 0)
		return 0;
  if (mask_height <= 0)
		return (int)(XWPSError::RangeCheck);
  switch (pimm->InterleaveType)
  {
  	default:
	    return (int)(XWPSError::RangeCheck);
	    
		case interleave_chunky:
	    if (mask_width != Width ||
					mask_height != Height ||
					pimm->MaskDict.BitsPerComponent != BitsPerComponent ||
					format != ps_image_format_chunky)
				return (int)(XWPSError::RangeCheck);
	    break;
	    if (!is_multiple(mask_height, Height))
				return (int)(XWPSError::RangeCheck);
				
			case interleave_separate_source:
				switch (pimm->MaskDict.BitsPerComponent) 
				{
	    		case 1: case 2: case 4: case 8:
						break;
						
	    		default:
						return (int)(XWPSError::RangeCheck);
	    	}
  }
  
  if (!check_image3x_extent(ImageMatrix.xx, pimm->MaskDict.ImageMatrix.xx) ||
			!check_image3x_extent(ImageMatrix.xy, pimm->MaskDict.ImageMatrix.xy) ||
			!check_image3x_extent(ImageMatrix.yx, pimm->MaskDict.ImageMatrix.yx) ||
			!check_image3x_extent(ImageMatrix.yy, pimm->MaskDict.ImageMatrix.yy))
		return (int)(XWPSError::RangeCheck);
  
  if ((code = pmcv->matrix.matrixInvert(&pimm->MaskDict.ImageMatrix)) < 0 ||
			(code = pmcv->corner.transform(mask_width, mask_height, &pmcv->matrix)) < 0)
		return code;
  
  if (fabs(ppcv->matrix.tx - pmcv->matrix.tx) >= 0.5 ||
			fabs(ppcv->matrix.ty - pmcv->matrix.ty) >= 0.5 ||
			fabs(ppcv->corner.x - pmcv->corner.x) >= 0.5 ||
			fabs(ppcv->corner.y - pmcv->corner.y) >= 0.5)
		return (int)(XWPSError::RangeCheck);
    
  pmcv->rect.p.x = ppcv->rect.p.x * mask_width / Width;
  pmcv->rect.p.y = ppcv->rect.p.y * mask_height / Height;
  pmcv->rect.q.x = (ppcv->rect.q.x * mask_width + Width - 1) / Width;
  pmcv->rect.q.y = (ppcv->rect.q.y * mask_height + Height - 1) / Height;
  pmcs->InterleaveType = (PSImage3InterleaveType)(pimm->InterleaveType);
  pmcs->width = pmcv->rect.q.x - pmcv->rect.p.x;
  pmcs->height = pmcv->rect.q.y - pmcv->rect.p.y;
  pmcs->full_height = pimm->MaskDict.Height;
  pmcs->depth = pimm->MaskDict.BitsPerComponent;
  if (pmcs->InterleaveType == interleave_chunky) 
  {
		pmcs->data = new uchar[(pmcs->width * pimm->MaskDict.BitsPerComponent + 7) >> 3];
		pmcs->own_data = true;
	}
  pmcs->y = pmcs->skip = 0;
  return 0;
}

void XWPSImage3X::image3xInit(XWPSColorSpace * color_space)
{
	pixelInit(color_space);
}

XWPSImage4::XWPSImage4()
	:XWPSPixelImage()
{
	type.begin_typed_image_.begin_typed_image4 = &XWPSImage4::beginImage4;
	type.sput_.sput4 = &XWPSImage4::image4SPut;
	type.sget_.sget4 = &XWPSImage4::image4SGet;
	type.index = 4;
	MaskColor_is_range = false;
}

int XWPSImage4::beginImage4(XWPSDevice * dev,
									 XWPSImagerState * pis, 
									XWPSMatrix * pmat,
		              XWPSIntRect * ,
		              XWPSDeviceColor * pdcolor, 
		              XWPSClipPath * pcpath,
		              XWPSImageEnumCommon ** pinfo)
{
	XWPSImageEnum *penum = new XWPSImageEnum;
	penum->alpha = ps_image_alpha_none;
  penum->masked = false;
  penum->adjust = fixed_0;
  
  {
		bool opaque = false;
		uint max_value = (1 << BitsPerComponent) - 1;
		int spp = ColorSpace->numComponents();
		int i;

		for (i = 0; i < spp * 2; i += 2) 
		{
	    uint c0, c1;

	    if (MaskColor_is_range)
				c0 = MaskColor[i], c1 = MaskColor[i + 1];
	    else
				c0 = c1 = MaskColor[i >> 1];

	    if ((c0 | c1) > max_value)
				return (int)(XWPSError::RangeCheck);
	    if (c0 > c1) 
	    {
				opaque = true;
				break;
	    }
	    penum->mask_color.values[i] = c0;
	    penum->mask_color.values[i + 1] = c1;
		}
		penum->use_mask_color = !opaque;
  }
  int code = penum->image1beginImage(dev, pis, pmat, this, pdcolor, pcpath);
  if (code >= 0)
		*pinfo = penum;
	else
		delete penum;
  return code;
}

void XWPSImage4::image4Init(XWPSColorSpace * color_space)
{
	pixelInit(color_space);
}

int  XWPSImage4::image4SGet(XWPSStream *s, XWPSColorSpace *pcs)
{
	int num_values;
  int i;
  int code = pixelImageSGet(s, pcs);

  if (code < 0)
		return code;
  MaskColor_is_range = code;
  num_values = pcs->numComponents() *	(MaskColor_is_range ? 2 : 1);
  for (i = 0; i < num_values; ++i)
		s->getVariableUint(&MaskColor[i]);
  return 0;
}

int XWPSImage4::image4SPut(XWPSStream *s,  XWPSColorSpace **ppcs)
{
	bool is_range = MaskColor_is_range;
  int code = pixelImageSPut(s, ppcs, is_range);
  int num_values =	ColorSpace->numComponents() * (is_range ? 2 : 1);
  int i;

  if (code < 0)
		return code;
  for (i = 0; i < num_values; ++i)
		s->putVariableUint(MaskColor[i]);
  *ppcs = ColorSpace;
  return 0;
}

XWPSImageEnumCommon::XWPSImageEnumCommon()
{
	image_type = 0;
	procs.plane_data_.plane_data = 0;
	procs.end_image_.end_image = 0;
	procs.flush_.flush = 0;
	procs.planes_wanted_.planes_wanted = 0;
	dev = 0;
	id = 0;
	num_planes = 0;
	memset(plane_depths, 0, ps_image_max_planes * sizeof(int));
	memset(plane_widths, 0, ps_image_max_planes * sizeof(int));
}

XWPSImageEnumCommon::~XWPSImageEnumCommon()
{
}

void XWPSImageEnumCommon::commonInit(XWPSDataImage * pic,
			  				                     XWPSDevice * devA, 
			  				                     int num_componentsA,
			                               PSImageFormat formatA)
{
	int bpc = pic->BitsPerComponent;
  int i;

  image_type = &pic->type;
  dev = devA;
  id = 0;
  if (devA->context_state)
  	id = devA->context_state->nextIDS(1);
  switch (formatA) 
  {
		case ps_image_format_chunky:
	    num_planes = 1;
	    plane_depths[0] = bpc * num_componentsA;
	    break;
	    
		case ps_image_format_component_planar:
	    num_planes = num_componentsA;
	    for (i = 0; i < num_componentsA; ++i)
				plane_depths[i] = bpc;
	    break;
	    
		case ps_image_format_bit_planar:
	    num_planes = bpc * num_componentsA;
	    for (i = 0; i < num_planes; ++i)
				plane_depths[i] = 1;
	    break;
	    
		default:
	    return ;
  }
  for (i = 0; i < num_planes; ++i)
		plane_widths[i] = pic->Width;
}

int XWPSImageEnumCommon::end(bool draw_last)
{
	return (this->*(procs.end_image_.end_image))(draw_last);
}

int XWPSImageEnumCommon::flush()
{
	if (procs.flush_.flush)
			(this->*(procs.flush_.flush))();
	
	return 0;
}

int XWPSImageEnumCommon::imageData(const uchar ** plane_data, int data_x, uint raster, int height)
{
  PSImagePlane planes[ps_image_max_planes];
  for (int i = 0; i < num_planes; ++i) 
 	{
		planes[i].data = plane_data[i];
		planes[i].data_x = data_x;
		planes[i].raster = raster;
  }
  return planeData(planes, height);
}

int XWPSImageEnumCommon::planeData(const PSImagePlane * planes, int height)
{
	int ignore_rows_used;
  return planeDataRows(planes, height, &ignore_rows_used);
}

int  XWPSImageEnumCommon::planeDataNo(const PSImagePlane * , int , int *)
{
	return (int)(XWPSError::Fatal);
}

int  XWPSImageEnumCommon::planeDataRows(const PSImagePlane * planes, int height,int *rows_used)
{
	return (this->*(procs.plane_data_.plane_data))(planes, height, rows_used);
}

bool XWPSImageEnumCommon::planesWanted(uchar *wanted)
{
  if (procs.planes_wanted_.planes_wanted)
		return (this->*(procs.planes_wanted_.planes_wanted))(wanted);
  else 
  {
		memset(wanted, 0xff, num_planes);
		return true;
  }
}

XWPSImageClue::XWPSImageClue()
{
	key = 0;
}

#define decode_base decode_lookup[0]
#define decode_max decode_lookup[15]

#define decode_sample(sample_value, cc, i)\
  switch ( map[i].decoding )\
  {\
  case sd_none:\
    cc.paint.values[i] = (sample_value) * (1.0 / 255.0);  /* faster than / */\
    break;\
  case sd_lookup:	/* <= 4 significant bits */\
    cc.paint.values[i] =\
      map[i].decode_lookup[(sample_value) >> 4];\
    break;\
  case sd_compute:\
    cc.paint.values[i] =\
      map[i].decode_base + (sample_value) * map[i].decode_factor;\
  }
  
#define decode_frac(frac_value, cc, i)\
  cc.paint.values[i] =\
    map[i].decode_base + (frac_value) * map[i].decode_factor

XWPSImageEnum::XWPSImageEnum()
	:XWPSImageEnumCommon()
{
	procs.plane_data_.plane_data1 = &XWPSImageEnum::image1PlaneData;
	procs.end_image_.end_image1 = &XWPSImageEnum::image1EndImage;
	procs.flush_.flush1 = &XWPSImageEnum::image1Flush;
	bps = 0;
	unpack_bps = 0;
	log2_xbytes = 0;
	spp = 0;
	use_mask_color = false;
	spread = 0;
	interpolate = 0;
	rect.x = 0;
	rect.y = 0;
	rect.w = 0;
	rect.h = 0;
	unpack = 0;
	pis = 0;
	render = 0;
	pcs = 0;
	buffer = 0;
	buffer_size = 0;
	line = 0;
	line_size = 0;
	line_width = 0;
	use_rop = 0;
	clip_image = 0;
	slow_loop = 0;
	device_color = 0;
	log_op = 0;
	dxx = dxy = 0;
	clip_dev = 0;
	rop_dev = 0;
	scaler = 0;
	y = 0;
	line_xy = 0;
	xi_next = 0;
	yci = hci = 0;
	xci = wci = 0;	
	unpack = 0;
  alpha = (PSImageAlpha)0;
  masked = 0;
  adjust = false;
}

XWPSImageEnum::XWPSImageEnum(const XWPSPixelImage * pim, const XWPSIntRect * prect)
	:XWPSImageEnumCommon()
{
	procs.plane_data_.plane_data1 = &XWPSImageEnum::image1PlaneData;
	procs.end_image_.end_image1 = &XWPSImageEnum::image1EndImage;
	procs.flush_.flush1 = &XWPSImageEnum::image1Flush;
	bps = 0;
	unpack_bps = 0;
	log2_xbytes = 0;
	spp = 0;
	use_mask_color = false;
	spread = 0;
	interpolate = 0;
	rect.x = 0;
	rect.y = 0;
	rect.w = 0;
	rect.h = 0;
	unpack = 0;
	pis = 0;
	render = 0;
	pcs = 0;
	buffer = 0;
	buffer_size = 0;
	line = 0;
	line_size = 0;
	line_width = 0;
	use_rop = 0;
	clip_image = 0;
	slow_loop = 0;
	device_color = 0;
	log_op = 0;
	dxx = dxy = 0;
	clip_dev = 0;
	rop_dev = 0;
	scaler = 0;
	y = 0;
	line_xy = 0;
	xi_next = 0;
	yci = hci = 0;
	xci = wci = 0;	
	unpack = 0;
	int width = pim->Width, height = pim->Height;
  int bpc = pim->BitsPerComponent;
  switch (pim->format)
  {
  	case ps_image_format_chunky:
    case ps_image_format_component_planar:
    	switch (bpc)
    	{
    		case 1: case 2: case 4: case 8: case 12: break;
				default:
					break;
    	}
    	
    case ps_image_format_bit_planar:
    	break;
  }
  
  if (prect) 
  {
		rect.x = prect->p.x;
		rect.y = prect->p.y;
		rect.w = prect->q.x - prect->p.x;
		rect.h = prect->q.y - prect->p.y;
  } 
  else 
  {
		rect.w = width, rect.h = height;
  }
}

XWPSImageEnum::~XWPSImageEnum()
{
	clear();
}

irender_proc_t XWPSImageEnum::imageClass0Interpolate()
{
	PSStreamImageScaleParams iss;
  PSStreamImageScaleState *pss;
  const PSStreamTemplate *templat;
  
  XWPSColorSpace *pccs;
  XWPSPoint dst_xy;
  uint in_size;

  if (!interpolate || use_mask_color)
		return 0;
  
  if (posture != image_portrait || masked || alpha) 
  {
		interpolate = false;
		return 0;
  }
  
#ifdef USE_MITCHELL_FILTERX
  if (bps < 4 || bps * penum->spp < 8 || (fabs(matrix.xx) <= 5 && fabs(matrix.yy <= 5))) 
  {
		interpolate = false;
		return 0;
  }
#endif
  
  dst_xy.distanceTransform((float)rect.w, (float)rect.h, &matrix);
  iss.BitsPerComponentOut = sizeof(short) * 8;
  iss.MaxValueOut = frac_1;
  iss.WidthOut = (int)ceil(fabs(dst_xy.x));
  iss.HeightOut = (int)ceil(fabs(dst_xy.y));
  iss.WidthIn = rect.w;
  iss.HeightIn = rect.h;
  pccs = pcs->concreteSpace(pis);
  iss.Colors = pccs->numComponents();
  if (bps <= 8 && device_color) 
  {
		iss.BitsPerComponentIn = 8;
		iss.MaxValueIn = 0xff;
		in_size = (matrix.xx < 0 ?   iss.WidthIn * iss.Colors : 0);
  } 
  else 
  {
		iss.BitsPerComponentIn = sizeof(short) * 8;
		iss.MaxValueIn = frac_1;
		in_size = ROUND_UP(iss.WidthIn * iss.Colors * sizeof(short),  align_bitmap_mod);
  }
  
  {
		uint out_size =  iss.WidthOut * qMax((uint)(iss.Colors * (iss.BitsPerComponentOut / 8)), (uint)(sizeof(ulong)));

		line = new uchar[in_size + out_size];
  }
#ifdef USE_MITCHELL_FILTER
  templat = &s_IScale_template;
#else
  templat = &s_IIEncode_template;
#endif
  
  pss = (PSStreamImageScaleState*)((*(templat->create_state))());
  if (line == 0 || pss == 0 ||(pss->params = iss, pss->templat = templat, 
  	   (*(pss->templat->init))((PSStreamState *) pss) < 0)) 
 	{
 		if (pss)
 		{
 			if (pss->templat)
  		{
  			if (pss->templat->release)
  				(*(pss->templat->release))((PSStreamState *) pss);
  		}
 		}
 		
 		if (line)
 		{
 			delete [] line;
 			line = 0;
 		}
 		
		interpolate = false;
		return 0;
  }
  scaler = pss;
  line_xy = 0;
  
  {
		PSDDAFixed x0;

		x0 = dda.pixel0.x;
		if (matrix.xx < 0)
	    dda_advance(x0, rect.w);
		xyi.x = fixed2int_pixround(dda_current(x0));
  }
  xyi.y = fixed2int_pixround(dda_current(dda.pixel0.y));
  return &XWPSImageEnum::imageRenderInterpolate;
}

irender_proc_t XWPSImageEnum::imageClass1Simple()
{
	irender_proc_t rproc;
  long ox = dda_current(dda.pixel0.x);
  long oy = dda_current(dda.pixel0.y);

  if (use_rop || spp != 1 || bps != 1)
		return 0;
  
  switch (posture) 
  {
		case image_portrait:
	    {	
				long dev_width =   fixed2long_pixround(ox + x_extent.x) - fixed2long_pixround(ox);

				if (dev_width != rect.w) 
				{
		    	long line_sizeA =	bitmap_raster(qAbs(dev_width)) + align_bitmap_mod;

		    	if (adjust != 0 || line_sizeA > max_uint)
						return 0;
		    	line_width = qAbs(dev_width);
		    	line_size = (uint) line_sizeA;
		    	line = new uchar[line_size];
		    	if (line == 0) 
		    	{
						dev->endImageDefault(this, false);
						return 0;
		    	}
				}
				rproc = &XWPSImageEnum::imageRenderSimple;
				break;
	    }
	    
		case image_landscape:
	    {	
				long dev_width =  fixed2long_pixround(oy + x_extent.y) - fixed2long_pixround(oy);
				long line_sizeA = (dev_width = qAbs(dev_width), bitmap_raster(dev_width) * 8 +   ROUND_UP(dev_width, 8) * align_bitmap_mod);

				if ((dev_width != rect.w && adjust != 0) ||  line_size > max_uint )
		    	return 0;
		    	
				line_width = dev_width;
				line_size = (uint) line_sizeA;
				line = new uchar[line_size];
				if (line == 0) 
				{
		    	dev->endImageDefault(this, false);
					return 0;
				}
				xi_next = line_xy = fixed2int_var_rounded(ox);
				rproc = &XWPSImageEnum::imageRenderLandscape;
				dxy =  float2fixed(matrix.xy +	fixed2float(fixed_epsilon) / 2);
				break;
	    }
	    
		default:
	    return 0;
  }
  
  dxx =	float2fixed(matrix.xx + fixed2float(fixed_epsilon) / 2);
  unpack = sample_unpack_copy;
  unpack_bps = 8;
  if (use_mask_color) 
  {
		masked = true;
		if (mask_color.values[0] == 1) 
		{
	    memcpy(&map[0].table.lookup4x1to32[0],  lookup4x1to32_inverted, 16 * 4);
	    clues[255].dev_color = clues[0].dev_color;
		} 
		else if (mask_color.values[1] == 0) 
	    memcpy(&map[0].table.lookup4x1to32[0],  lookup4x1to32_identity, 16 * 4);
		else 
	    rproc = &XWPSImageEnum::imageRenderSkip;
		clues[0].dev_color.setPure(ps_no_color_index);
		map[0].decoding = sd_none;
  }
  return rproc;
}

irender_proc_t XWPSImageEnum::imageClass2Fracs()
{
	if (bps > 8) 
	{
		if (use_mask_color) 
		{
	    int i;

	    for (i = 0; i < spp * 2; ++i)
				mask_color.values[i] =  bits2frac(mask_color.values[i], 12);
		}
		return &XWPSImageEnum::imageRenderFrac;
  }
  return 0;
}

irender_proc_t XWPSImageEnum::imageClass3Mono()
{
	if (spp == 1) 
	{
		slow_loop = (masked && !clues[255].dev_color.isPure()) || use_rop;
		if (!(slow_loop || posture != image_portrait))
	    clip_image &= ~(image_clip_xmin | image_clip_xmax);
		dxx = float2fixed(matrix.xx + fixed2float(fixed_epsilon) / 2);
		if (use_mask_color) 
		{
	    scaleMaskColors(0);
	    if (mask_color.values[0] <= 0)
				clues[0].dev_color.setNull();
	    if (mask_color.values[1] >= 255)
				clues[255].dev_color.setNull();
		}
		return &XWPSImageEnum::imageRenderMono;
  }
  return 0;
}

typedef union {
    uchar v[PS_IMAGE_MAX_COLOR_COMPONENTS];
#define BYTES_PER_BITS32 4
#define BITS32_PER_COLOR_SAMPLES\
  ((PS_IMAGE_MAX_COLOR_COMPONENTS + BYTES_PER_BITS32 - 1) / BYTES_PER_BITS32)
    quint32 all[BITS32_PER_COLOR_SAMPLES];	/* for fast comparison */
} PSColorSamples;

irender_proc_t XWPSImageEnum::imageClass4Color()
{
	if (use_mask_color) 
	{
		int i;
		PSColorSamples mask, test;
		bool exact = spp <= BYTES_PER_BITS32;

		memset(&mask, 0, sizeof(mask));
		memset(&test, 0, sizeof(test));
		for (i = 0; i < spp; ++i) 
		{
	    uchar v0, v1;
	    uchar match = 0xff;

	    scaleMaskColors(i);
	    v0 = (uchar)mask_color.values[2 * i];
	    v1 = (uchar)mask_color.values[2 * i + 1];
	    while ((v0 & match) != (v1 & match))
				match <<= 1;
	    mask.v[i] = match;
	    test.v[i] = v0 & match;
	    exact &= (v0 == match && (v1 | match) == 0xff);
		}
		mask_color.mask = mask.all[0];
		mask_color.test = test.all[0];
		mask_color.exact = exact;
  } 
  else 
  {
		mask_color.mask = 0;
		mask_color.test = ~0;
  }
  return &XWPSImageEnum::imageRenderColor;
}

int XWPSImageEnum::image1beginImage(XWPSDevice * devA, 
	                       XWPSImagerState * pisA,
		                     XWPSMatrix *pmat, 
		                     XWPSImageCommon * pic,
		                     XWPSDeviceColor * pdcolor,
		                     XWPSClipPath * pcpath)
{
	XWPSPixelImage *pim = (XWPSPixelImage *)pic;
  PSImageFormat formatA = pim->format;
  const int widthA = pim->Width;
  const int heightA = pim->Height;
  const int bpsA = pim->BitsPerComponent;
  bool maskedA = masked;
  const float *decodeA = pim->Decode;
  XWPSMatrix mat, ctm;
  int index_bps;
  XWPSColorSpace *pcsA = pim->ColorSpace;
  ulong lop = (pisA ? pisA->log_op : lop_default);
  int code;
  int log2_xbytesA = (bpsA <= 8 ? 0 : arch_log2_sizeof_frac);
  int sppA, nplanes, spreadA;
  uint bsize;
  uchar *bufferA;
  long mtx, mty;
  XWPSFixedPoint row_extent, col_extent, x_extentA, y_extentA;
  bool device_colorA;
  XWPSFixedRect obox, cbox;
  ctm = pisA->ctm;
  if (pmat == 0)
  	pmat = &ctm;
  	
  if ((code = mat.matrixInvert(&pim->ImageMatrix)) < 0 ||
			(code = mat.matrixMultiply(&mat, pmat)) < 0) 
	{		
		return code;
  }
  matrix = mat;
  index_bps = (bpsA < 8 ? bpsA >> 1 : (bpsA >> 2) + 1);
  mtx = float2fixed(mat.tx);
  mty = float2fixed(mat.ty);
  row_extent.x = float2fixed(widthA * mat.xx + mat.tx) - mtx;
  row_extent.y =(is_fzero(mat.xy) ? fixed_0 : float2fixed(widthA * mat.xy + mat.ty) - mty);
  col_extent.x =(is_fzero(mat.yx) ? fixed_0 :	 float2fixed(heightA * mat.yx + mat.tx) - mtx);
  col_extent.y = float2fixed(heightA * mat.yy + mat.ty) - mty;
  commonInit((XWPSDataImage *)pim, devA, (maskedA ? 1 : pcs->numComponents()), formatA);
  if (rect.w == widthA && rect.h == heightA) 
  {
		x_extentA = row_extent;
		y_extentA = col_extent;
  } 
  else 
  {
		int rw = rect.w, rh = rect.h;

		x_extentA.x = float2fixed(rw * mat.xx + mat.tx) - mtx;
		x_extentA.y = (is_fzero(mat.xy) ? fixed_0 : float2fixed(rw * mat.xy + mat.ty) - mty);
		y_extentA.x = (is_fzero(mat.yx) ? fixed_0 :  float2fixed(rh * mat.yx + mat.tx) - mtx);
		y_extentA.y = float2fixed(rh * mat.yy + mat.ty) - mty;
  }
  if (maskedA) 
  {	
		if (bpsA != 1 || pcsA != NULL || alpha ||
	    !((decodeA[0] == 0.0 && decodeA[1] == 1.0) ||
	      (decodeA[0] == 1.0 && decodeA[1] == 0.0))) 
	  {
	    return (int)(XWPSError::RangeCheck);
		}
		
		clues[0].dev_color.setPure(ps_no_color_index);
		clues[255].dev_color = *pdcolor;
		memcpy(&map[0].table.lookup4x1to32[0], (decodeA[0] == 0 ? lookup4x1to32_inverted : lookup4x1to32_identity), 16 * 4);
		map[0].decoding = sd_none;
		sppA = 1;
		lop = rop3_know_S_0(lop);
  }
  else
  {
  	PSColorSpaceType *pcst = pcsA->type;
		int b_w_color;

		sppA = pcsA->numComponents();
		if (sppA < 0) 
	    return (int)(XWPSError::RangeCheck);
	    	
		if (alpha)
	    ++sppA;
	    
		switch (formatA) 
		{
			case ps_image_format_bit_planar:
	    	if (bpsA > 1)
					break;
	    	formatA = ps_image_format_component_planar;
			case ps_image_format_component_planar:
	    	if (sppA == 1)
					formatA = ps_image_format_chunky;
			default:
	    	break;
		}
		device_colorA = (pcsA->*pcst->concrete_space)(pisA) == pcsA;
		initColors(bpsA, sppA, formatA, decodeA, pisA, devA,  pcsA, &device_colorA);
		if (!pim->CombineWithColor)
	    lop = rop3_know_T_0(lop) & ~lop_T_transparent;
		else 
		{
	    if (rop3_uses_T(lop))
				switch (color_draws_b_w(devA, pdcolor)) 
				{
			    case 0:
						lop = rop3_know_T_0(lop);
						break;
						
		    	case 1:
						lop = rop3_know_T_1(lop);
						break;
						
		    	default:
						;
				}
		}
		if (lop != rop3_S &&
	    	!rop3_uses_T(lop) && bpsA == 1 && sppA == 1 &&
	    	(b_w_color = color_draws_b_w(dev, &clues[0].dev_color)) >= 0 &&
	    	color_draws_b_w(devA, &clues[255].dev_color) == (b_w_color ^ 1)) 
	  {
	    if (b_w_color) 
	    {
				XWPSDeviceColor dcolor;

				dcolor = clues[0].dev_color;
				clues[0].dev_color = clues[255].dev_color;
				clues[255].dev_color = dcolor;
				lop = rop3_invert_S(lop);
	    }
	    
	    switch (lop) 
	    {
				case rop3_D & rop3_S:
		    	clues[255].dev_color = clues[0].dev_color;
				case rop3_D | rop3_not(rop3_S):
		    	memcpy(&map[0].table.lookup4x1to32[0],  lookup4x1to32_inverted, 16 * 4);
		  
rmask:
		    	masked = maskedA = true;
		    	clues[0].dev_color.setPure(ps_no_color_index);
		    	map[0].decoding = sd_none;
		    	lop = rop3_T;
		    	break;
		    
				case rop3_D & rop3_not(rop3_S):
		    	clues[255].dev_color = clues[0].dev_color;
				case rop3_D | rop3_S:
		    	memcpy(&map[0].table.lookup4x1to32[0], lookup4x1to32_identity, 16 * 4);
		    	goto rmask;
		    	
				default:
		    	;
	    }
		}
  }
  
  device_color = device_colorA;
  bsize = ((bpsA > 8 ? widthA * 2 : widthA) + 15) * sppA;
  bufferA = new uchar[bsize];
  bps = bpsA;
  unpack_bps = bpsA;
  log2_xbytes = log2_xbytesA;
  spp = sppA;
  switch (formatA)
  {
  	case ps_image_format_chunky:
			nplanes = 1;
			spreadA = 1 << log2_xbytesA;
			break;
			
    case ps_image_format_component_planar:
			nplanes = sppA;
			spreadA = sppA << log2_xbytesA;
			break;
			
    case ps_image_format_bit_planar:
			nplanes = sppA * bpsA;
			spreadA = sppA << log2_xbytesA;
			break;
			
    default:
			return (int)(XWPSError::Fatal);
  }
  
  num_planes = nplanes;
  spread = spreadA;
  interpolate = pim->Interpolate;
  x_extent = x_extentA;
  y_extent = y_extentA;
  posture =	((x_extentA.y | y_extentA.x) == 0 ? image_portrait : (x_extentA.x | y_extentA.y) == 0 ? image_landscape : image_skewed);
  pis = pisA;
  pcs = pcsA;
  buffer = bufferA;
  buffer_size = bsize;
  line = 0;
  line_size = 0;
  use_rop = lop != (maskedA ? rop3_T : rop3_S);
  slow_loop = 0;
  if (pcpath == 0) 
  {
		devA->getClipingBox(&obox);
		cbox = obox;
		clip_image = 0;
  } 
  else
		clip_image = (pcpath->outerBox(&obox) | pcpath->innerBox(&cbox) ?  0 : image_clip_region);
			
	clip_outer = obox;
  clip_inner = cbox;
  log_op = rop3_T;
  clip_dev = 0;
  rop_dev = 0;
  scaler = 0;
  
  {
  	long  epx = qMin(row_extent.x, (long)0) + qMin(col_extent.x, (long)0),
	    		eqx = qMax(row_extent.x, (long)0) + qMax(col_extent.x, (long)0),
	    		epy = qMin(row_extent.y, (long)0) + qMin(col_extent.y, (long)0),
	    		eqy = qMax(row_extent.y, (long)0) + qMax(col_extent.y, (long)0);
	    		
		{
	    int hwx, hwy;

	    switch (posture) 
	    {
				case image_portrait:
		    	hwx = widthA, hwy = heightA;
		    	break;
		    		
				case image_landscape:
		    	hwx = heightA, hwy = widthA;
		    	break;
		    	
				default:
		    	hwx = hwy = 0;
	    }
	    
	    if (hwx == 1 && eqx - epx < fixed_1) 
	    {
				long diff =	arith_rshift_1(row_extent.x + col_extent.x);

				mtx = (((mtx + diff) | fixed_half) & -fixed_half) - diff;
	    }
	    
	    if (hwy == 1 && eqy - epy < fixed_1) 
	    {
				long diff =	arith_rshift_1(row_extent.y + col_extent.y);

				mty = (((mty + diff) | fixed_half) & -fixed_half) - diff;
	    }
		}
		
		dda_init(dda.row.x, mtx, col_extent.x, heightA);
		dda_init(dda.row.y, mty, col_extent.y, heightA);
		if (rect.y) 
		{
	    dda_advance(dda.row.x, rect.y);
	    dda_advance(dda.row.y, rect.y);
		}
		cur.x = prev.x = dda_current(dda.row.x);
		cur.y = prev.y = dda_current(dda.row.y);
		dda_init(dda.strip.x, cur.x, row_extent.x, widthA);
		dda_init(dda.strip.y, cur.y, row_extent.y, widthA);
		
		if (rect.x) 
		{
	    dda_advance(dda.strip.x, rect.x);
	    dda_advance(dda.strip.y, rect.x);
		}
		
		{
			long ox = dda_current(dda.strip.x);
	    long oy = dda_current(dda.strip.y);

	    if (!clip_image)
				clip_image = (fixed_pixround(ox + epx) < fixed_pixround(cbox.p.x) ?
		     							image_clip_xmin : 0) + (fixed_pixround(ox + eqx) >= fixed_pixround(cbox.q.x) ?
		     							image_clip_xmax : 0) +  (fixed_pixround(oy + epy) < fixed_pixround(cbox.p.y) ?
		     							image_clip_ymin : 0) +  (fixed_pixround(oy + eqy) >= fixed_pixround(cbox.q.y) ?
		     							image_clip_ymax : 0);
		}
  }
  
  y = 0;
  used.x = 0;
  used.y = 0;
  {
		static const sample_unpack_proc_t procs[4] = {
	    sample_unpack_1, sample_unpack_2,
	    sample_unpack_4, sample_unpack_8};
	    
		int i;

		if (index_bps == 4)
		{
			if ((unpack = sample_unpack_12_proc) == 0) 
			{
				end(false);
				return (int)(XWPSError::RangeCheck);
	    }	    
		}
		else
			unpack = procs[index_bps];
			
		dda.pixel0 = dda.strip;
		for (i = 0; i < ps_image_class_table_count; ++i)
	    if ((render = (this->*(ps_image_class_table[i]))()) != 0)
				break;
				
		if (i == ps_image_class_table_count)
		{
			end( false);
	    return (int)(XWPSError::RangeCheck);
		}
	}
	
	if (clip_image && pcpath) 
	{
		XWPSDeviceClip *cdev = new XWPSDeviceClip;
		cdev->context_state = devA->context_state;
		cdev->makeClipTranslateDevice(pcpath->getList(), 0, 0);
		cdev->setTarget(devA);
		cdev->open();
		clip_dev = cdev;
  }
  
  if (use_rop)
  {
  	XWPSDeviceRopTexture *rtdev = new XWPSDeviceRopTexture;
  	rtdev->context_state = devA->context_state;
  	rtdev->makeRopTextureDevice((clip_dev != 0 ? clip_dev : devA), lop, pdcolor);
  	rop_dev = rtdev;
  }
  return 0;
}

int XWPSImageEnum::image1EndImage(bool draw_last)
{
	if (draw_last) 
	{
		int code = flush();
		if (code < 0)
	    return code;
  }
  
  clear();
  return 0;
}

int XWPSImageEnum::image1Flush()
{
	int width_spp = rect.w * spp;
  cur.x = dda_current(dda.row.x);
  cur.y = dda_current(dda.row.y);
  switch (posture) 
  {
		case image_portrait:
	    {
				long yc = cur.y;
				yci = fixed2int_rounded(yc - adjust);
				hci = fixed2int_rounded(yc + adjust) - yci;
	    }
	    break;
	    
		case image_landscape:
	    {
				long xc = cur.x;

				xci = fixed2int_rounded(xc - adjust);
				wci = fixed2int_rounded(xc + adjust) - xci;
	    }
	    break;
	    
		case image_skewed:
	    ;
  }
  updateStrip();
  prev = cur;
  return (this->*render)(NULL, 0, width_spp, 0, setupImageDevice());
}

int XWPSImageEnum::image1PlaneData(const PSImagePlane * planes, 
	                                 int height, 
	                                 int *rows_used)
{
	int yA = y;
	int y_end = qMin((int)(yA + height), (int)(rect.h));
  int width_spp = rect.w * spp;
  
#define BCOUNT(plane)		/* bytes per data row */\
  (((rect.w + (plane).data_x) * spp * bps / num_planes\
    + 7) >> 3)
    
  ulong offsets[ps_image_max_planes];
  int ignore_data_x;
  bool bit_planar = num_planes > spp;
  int code;
  
  if (height == 0) 
  {
		*rows_used = 0;
		return 0;
  }
  XWPSDevice * devA = setupImageDevice();
  if (used.y) 
  {
		for (int px = 0; px < num_planes; ++px)
	    offsets[px] = planes[px].raster * used.y;
		used.y = 0;
  } 
  else
		memset(offsets, 0, num_planes * sizeof(offsets[0]));
		
	for (; y < y_end; y++)
	{
		int sourcex;
		int x_used = used.x;
		if (bit_planar)
		{
			sourcex = 0;
	    for (int px = 0; px < num_planes; px += bps)
				repackBitPlanes(planes, offsets, bps, buffer, rect.w, &map[0].table, spread);
	    for (int px = 0; px < num_planes; ++px)
				offsets[px] += planes[px].raster;
		}
		else
		{
			const uchar * bufferA = (unpack)(buffer, &sourcex, planes[0].data + offsets[0], planes[0].data_x, BCOUNT(planes[0]), &map[0].table, spread);
	    offsets[0] += planes[0].raster;
	    for (int px = 1; px < num_planes; ++px) 
	    {
				(unpack)((uchar*)bufferA + (px << log2_xbytes), &ignore_data_x, planes[px].data + offsets[px],
				 		planes[px].data_x, BCOUNT(planes[px]),	 &map[px].table, spread);	
				 offsets[px] += planes[px].raster;
	    }
		}
		
		cur.x = dda_current(dda.row.x);
		dda_next(dda.row.x);
		cur.y = dda_current(dda.row.y);
		dda_next(dda.row.y);
		if (!interpolate)
			switch (posture)
			{
				case image_portrait:
					{
						long yc = cur.y,	yn = dda_current(dda.row.y);
						if (yn < yc) 
						{
			    		long temp = yn;
			    		yn = yc;
			    		yc = temp;
						}
						yc -= adjust;
						if (yc >= clip_outer.q.y)
			    		goto mt;
						yn += adjust;
						if (yn <= clip_outer.p.y)
			    		goto mt;
						yci = fixed2int_pixround(yc);
						hci = fixed2int_pixround(yn) - yci;
						if (hci == 0)
			    		goto mt;
		    	}
		    	break;
		    	
		    case image_landscape:
		    	{
						long xc = cur.x,
			    	xn = dda_current(dda.row.x);
						if (xn < xc) 
						{
			    		long temp = xn;
			    		xn = xc;
			    		xc = temp;
						}
						xc -= adjust;
						if (xc >= clip_outer.q.x)
			    		goto mt;
						xn += adjust;
						if (xn <= clip_outer.p.x)
			    		goto mt;
						xci = fixed2int_pixround(xc);
						wci = fixed2int_pixround(xn) - xci;
						if (wci == 0)
			    		goto mt;
		    	}
		    	break;
		    	
		    case image_skewed:
		    	;
			}
			
		updateStrip();
		if (x_used) 
		{
	    dda_advance(dda.pixel0.x, x_used);
	    dda_advance(dda.pixel0.y, x_used);
	    used.x = 0;
		}
		
		code = (this->*render)(buffer, sourcex + x_used,	width_spp - x_used * spp, 1, devA);
		if (code < 0) 
		{
	  	used.x += x_used;
	    if (!used.y) 
	    {
				dda_previous(dda.row.x);
				dda_previous(dda.row.y);
				dda_translate(dda.strip.x, prev.x - cur.x);
				dda_translate(dda.strip.y, prev.y - cur.y);
	    }
	    goto out;
		}
		prev = cur;
		
mt:
		;
	}
	
	if (y < rect.h) 
	{
		code = 0;
  } 
  else 
  {
		code = image1Flush();
		if (code >= 0)
	    code = 1;
  }
  
out:
  *rows_used = y - yA;
  return code;
}

int XWPSImageEnum::imageRenderColor(const uchar *bufferA, 
	                    int data_x,
		                  uint w, 
		                  int h, 
		                  XWPSDevice * devA)
{
	ulong lop = log_op;
  PSDDAFixedPoint pnext;
  long xprev, yprev;
  long pdyx, pdyy;
  int vci, vdi;
  XWPSClientColor cc;
  quint32 mask = mask_color.mask;
  quint32 test = mask_color.test;
  XWPSImageClue *pic = &clues[0];
  XWPSImageClue *pic_next = &clues[1];
  XWPSImageClue empty_clue;
  XWPSImageClue clue_temp;
  
  const uchar *psrc_initial = bufferA + data_x * spp;
  const uchar *psrc = psrc_initial;
  const uchar *rsrc = psrc + spp; 
  long xrun;			/* x ditto */
  long yrun;			/* y ditto */
  int irun;			/* int x/rrun */
  PSColorSamples run;		/* run value */
  PSColorSamples next;		/* next sample value */
  const uchar *bufend = psrc + w;
  bool use_cache = spp * bps <= 12;
  int code = 0, mcode = 0;

  if (h == 0)
		return 0;
		
	pnext = dda.pixel0;
  xrun = xprev = dda_current(pnext.x);
  yrun = yprev = dda_current(pnext.y);
  pdyx = dda_current(dda.row.x) - cur.x;
  pdyy = dda_current(dda.row.y) - cur.y;
  switch (posture) 
  {
		case image_portrait:
	    vci = yci, vdi = hci;
	    irun = fixed2int_var_rounded(xrun);
	    break;
	    
		case image_landscape:
	    vci = xci, vdi = wci;
	    irun = fixed2int_var_rounded(yrun);
	    break;
	    
		default:
	    break;
  }
  
  memset(&run, 0, sizeof(run));
  memset(&next, 0, sizeof(next));
  if (use_cache) 
  {
		empty_clue.dev_color.setPure(ps_no_color_index);
		pic = &empty_clue;
  }
  pcs->initColor(&cc);
  run.v[0] = ~psrc[0];	/* force remap */
  while (psrc < bufend) 
  {
		dda_next(pnext.x);
		dda_next(pnext.y);
#define CLUE_HASH3(next)\
  &clues[(next.v[0] + (next.v[1] << 2) + (next.v[2] << 4)) & 255];
#define CLUE_HASH4(next)\
  &clues[(next.v[0] + (next.v[1] << 2) + (next.v[2] << 4) +\
		 (next.v[3] << 6)) & 255]

		if (spp == 4) 
		{	
	    next.v[0] = psrc[0];
	    next.v[1] = psrc[1];
	    next.v[2] = psrc[2];
	    next.v[3] = psrc[3];
	    psrc += 4;
map4:	    
			if (next.all[0] == run.all[0])
				goto inc;
	    if (use_cache) 
	    {
				pic_next = CLUE_HASH4(next);
				if (pic_next->key == next.all[0])
		    	goto f;
				if (pic_next == pic) 
				{
		    	clue_temp = *pic;
		    	pic = &clue_temp;
				}
				pic_next->key = next.all[0];
	    }
	    if ((next.all[0] & mask) == test &&	(mask_color.exact || maskColorMatches(next.v, 4))) 
	    {
				pic_next->dev_color.setNull();
				goto mapped;
	    }
	    if (device_color) 
	    {
	    	if (alpha)
	    		pis->mapRGBAlpha(byte2frac(next.v[0]), byte2frac(next.v[1]),
			 				byte2frac(next.v[2]), byte2frac(next.v[3]),	 &(pic_next->dev_color), devA, ps_color_select_source);
	    	else
	    		pis->mapCMYK(next.v[0], byte2frac(next.v[1]),
			 				byte2frac(next.v[2]), byte2frac(next.v[3]),	 &(pic_next->dev_color), devA, ps_color_select_source);
				goto mapped;
	    }
	    decode_sample(next.v[3], cc, 3);
	    
do3:	
			decode_sample(next.v[0], cc, 0);
	    decode_sample(next.v[1], cc, 1);
	    decode_sample(next.v[2], cc, 2);
		} 
		else if (spp == 3) 
		{
	    next.v[0] = psrc[0];
	    next.v[1] = psrc[1];
	    next.v[2] = psrc[2];
	    psrc += 3;
	    if (next.all[0] == run.all[0])
				goto inc;
	    if (use_cache) 
	    {
				pic_next = CLUE_HASH3(next);
				if (pic_next->key == next.all[0])
		    	goto f;
		    	
				if (pic_next == pic) 
				{
		    	clue_temp = *pic;
		    	pic = &clue_temp;
				}
				pic_next->key = next.all[0];
	    }
	    
	    if ((next.all[0] & mask) == test && (mask_color.exact || maskColorMatches(next.v, 3))) 
	    {
				pic_next->dev_color.setNull();
				goto mapped;
	    }
	    if (device_color) 
	    {
				pis->mapRGB(byte2frac(next.v[0]), byte2frac(next.v[1]),byte2frac(next.v[2]),
			 								&(pic_next->dev_color), devA, ps_color_select_source);
				goto mapped;
	    }
	    goto do3;
		} 
		else if (alpha) 
		{
	    if (spp == 2) 
	    {	
				next.v[2] = next.v[1] = next.v[0] = psrc[0];
				next.v[3] = psrc[1];
				psrc += 2;
				goto map4;
	    } 
	    else if (spp == 5) 
	    {	
				short rgb[3];

				pis->colorCMYKToRgb(byte2frac(psrc[0]), byte2frac(psrc[1]),  byte2frac(psrc[2]), byte2frac(psrc[3]), &rgb[0]);
		
				next.v[0] = frac2byte(rgb[0]);
				next.v[1] = frac2byte(rgb[1]);
				next.v[2] = frac2byte(rgb[2]);
				next.v[3] = psrc[4];
				psrc += 5;
				goto map4;
	    }
		} 
		else 
		{	
	    int i;

	    use_cache = false;
	    if (!memcmp(psrc, run.v, spp)) 
	    {
				psrc += spp;
				goto inc;
	    }
	    memcpy(next.v, psrc, spp);
	    psrc += spp;
	    if ((next.all[0] & mask) == test &&	(mask_color.exact || maskColorMatches(next.v, spp))) 
	    {
				pic_next->dev_color.setNull();
				goto mapped;
	    }
	    for (i = 0; i < spp; ++i)
				decode_sample(next.v[i], cc, i);
		}
		mcode = pcs->remapColor(pis, &cc, pcs, &(pic_next->dev_color), devA,  ps_color_select_source);
		if (mcode < 0)
	    goto fill;
	    
mapped:	
		if (pic == pic_next)
	    goto fill;
	    
f:	
		if (pic->dev_color.equal(&(pic_next->dev_color)))
	    goto set;
	    
fill:	
		switch (posture) 
		{
			case image_portrait:
	    	{	
					int xi = irun;
					int wi =   (irun = fixed2int_var_rounded(xprev)) - xi;

					if (wi < 0)
		    		xi += wi, wi = -wi;
					if (wi > 0)
		    		code = pic->dev_color.fillRectangle(xi, vci, wi, vdi, devA, lop, NULL);
					xrun = xprev;	
	    	}
	    	break;
	    	
			case image_landscape:
	    	{
					int yi = irun;
					int hi = (irun = fixed2int_var_rounded(yprev)) - yi;

					if (hi < 0)
		    		yi += hi, hi = -hi;
					if (hi > 0)
		    		code = pic->dev_color.fillRectangle(vci, yi, vdi, hi, devA, lop, NULL);
					yrun = yprev;	
	    	}
	    	break;
	    	
			default:
	    	{	
					code = devA->fillParallelogram(xrun, yrun, xprev - xrun, yprev - yrun, pdyx, pdyy,  &(pic->dev_color), lop);
					xrun = xprev;
					yrun = yprev;
	    	}
		}
		if (code < 0)
	    goto err;
		rsrc = psrc;
		if ((code = mcode) < 0) 
		{
	    if (use_cache)
				pic_next->key = ~next.all[0];
	    goto err;
		}
		if (use_cache)
	    pic = pic_next;
		else 
		{
	    XWPSImageClue *ptemp = pic;

	    pic = pic_next;
	    pic_next = ptemp;
		}
		
set:	
		run = next;
inc:	
		xprev = dda_current(pnext.x);
		yprev = dda_current(pnext.y);	
  }
  
  code = devA->fillParallelogram(xrun, yrun, xprev - xrun, yprev - yrun, pdyx, pdyy, &(pic->dev_color), lop);
  return (code < 0 ? code : 1);
  	
err:
  used.x = (rsrc - spp - psrc_initial) / spp;
  used.y = 0;
  return code;
}

#define FRACS_PER_LONG (arch_sizeof_long / arch_sizeof_frac)
typedef union {
    short v[PS_IMAGE_MAX_COLOR_COMPONENTS];
#define LONGS_PER_COLOR_FRACS\
  ((PS_IMAGE_MAX_COLOR_COMPONENTS + FRACS_PER_LONG - 1) / FRACS_PER_LONG)
    long all[LONGS_PER_COLOR_FRACS];	/* for fast comparison */
} color_fracs;

#define LONGS_PER_4_FRACS ((FRACS_PER_LONG + 3) / 4)
#if LONGS_PER_4_FRACS == 1
#  define COLOR_FRACS_4_EQ(f1, f2)\
     ((f1).all[0] == (f2).all[0])
#else
#if LONGS_PER_4_FRACS == 2
#  define COLOR_FRACS_4_EQ(f1, f2)\
     ((f1).all[0] == (f2).all[0] && (f1).all[1] == (f2).all[1])
#endif
#endif

int XWPSImageEnum::imageRenderFrac(const uchar * bufferA, 
	                    int data_x,
		                  uint w, 
		                  int h, 
		                  XWPSDevice * devA)
{
	ulong lop = log_op;
  PSDDAFixedPoint pnext;
  long xl, ytf;
  long pdyx, pdyy;
  int yt = yci, iht = hci;
  XWPSClientColor cc;
  XWPSDeviceColor devc1, devc2;
  XWPSDeviceColor *pdevc = &devc1;
  XWPSDeviceColor *pdevc_next = &devc2;
  const short *psrc_initial = (const short *)bufferA + data_x * spp;
  const short *psrc = psrc_initial;
  const short *rsrc = psrc + spp;
  long xrun;
  int irun;	
  long yrun;	
  color_fracs run;
  color_fracs next;	
  const short *bufend = psrc + w;
  int code = 0, mcode = 0;

  if (h == 0)
		return 0;
    
  pnext = dda.pixel0;
  xrun = xl = dda_current(pnext.x);
  irun = fixed2int_var_rounded(xrun);
  yrun = ytf = dda_current(pnext.y);
  pdyx = dda_current(dda.row.x) - cur.x;
  pdyy = dda_current(dda.row.y) - cur.y;
  memset(&run, 0, sizeof(run));
  memset(&next, 0, sizeof(next));
  devc1.setPure(ps_no_color_index);
  pcs->initColor(&cc);
  run.v[0] = ~psrc[0];

  while (psrc < bufend) 
  {
		next.v[0] = psrc[0];
		switch (spp) 
		{
	    case 4:		/* may be CMYK */
				next.v[1] = psrc[1];
				next.v[2] = psrc[2];
				next.v[3] = psrc[3];
				psrc += 4;
				if (COLOR_FRACS_4_EQ(next, run))
		    	goto inc;
				if (use_mask_color && maskColor12Matches(next.v, 4)) 
				{
		    	pdevc_next->setNull();
		    	goto f;
				}
				if (device_color) 
				{
		    	pis->mapCMYK(next.v[0], next.v[1], next.v[2], next.v[3], pdevc_next, devA,ps_color_select_source);
		    	goto f;
				}
				decode_frac(next.v[0], cc, 0);
				decode_frac(next.v[1], cc, 1);
				decode_frac(next.v[2], cc, 2);
				decode_frac(next.v[3], cc, 3);
				break;
				
	    case 3:		/* may be RGB */
				next.v[1] = psrc[1];
				next.v[2] = psrc[2];
				psrc += 3;
				if (COLOR_FRACS_4_EQ(next, run))
		    	goto inc;
				if (use_mask_color && maskColor12Matches(next.v, 3)) 
				{
		    	pdevc_next->setNull();
		    	goto f;
				}
				if (device_color) 
				{
		    	pis->mapRGB(next.v[0], next.v[1],	next.v[2], pdevc_next, devA,ps_color_select_source);
		    	goto f;
				}
				decode_frac(next.v[0], cc, 0);
				decode_frac(next.v[1], cc, 1);
				decode_frac(next.v[2], cc, 2);
				break;
				
	    case 1:		/* may be Gray */
				psrc++;
				if (next.v[0] == run.v[0])
		    	goto inc;
				if (use_mask_color && maskColor12Matches(next.v, 1)) 
				{
		    	pdevc_next->setNull();
		    	goto f;
				}
				if (device_color) 
				{
		    	pis->mapRGB(next.v[0], next.v[0],	next.v[0], pdevc_next, devA,ps_color_select_source);
		    	goto f;
				}
				decode_frac(next.v[0], cc, 0);
				break;
				
	    default:		/* DeviceN */
				{
		    	int i;

		    	for (i = 1; i < spp; ++i)
						next.v[i] = psrc[i];
		    	psrc += spp;
		    	if (!memcmp(next.v, run.v, spp * sizeof(next.v[0])))
						goto inc;
		    	if (use_mask_color && maskColor12Matches(next.v, spp)) 
		    	{
						pdevc_next->setNull();
						goto f;
		    	}
		    	for (i = 0; i < spp; ++i)
						decode_frac(next.v[i], cc, i);
				}
				break;
		}
		mcode = pcs->remapColor(pis, &cc, pcs, pdevc_next, devA, ps_color_select_source);
		if (mcode < 0)
	    goto fill;
f:
		if (!devc1.equal(&devc2)) 
		{
	  	XWPSDeviceColor *ptemp;

fill:
	  	if (posture != image_portrait) 
				code = devA->fillParallelogram(xrun, yrun, xl - xrun, ytf - yrun, pdyx, pdyy, pdevc, lop);
	  	else 
	  	{
				int xi = irun;
				int wi = (irun = fixed2int_var_rounded(xl)) - xi;

				if (wi < 0)
		    	xi += wi, wi = -wi;
				code = pdevc->fillRectangle(xi, yt,  wi, iht, devA, lop, NULL);
	  	}
	  	if (code < 0)
				goto err;
	  	rsrc = psrc;
	  	if ((code = mcode) < 0)
				goto err;
	    
	  	ptemp = pdevc;
	  	pdevc = pdevc_next;
	  	pdevc_next = ptemp;
	  	xrun = xl;
	  	yrun = ytf;
		}
			run = next;
	
inc:
			xl = dda_next(pnext.x);
			ytf = dda_next(pnext.y);
 	}
  code = devA->fillParallelogram(xrun, yrun, xl - xrun, ytf - yrun, pdyx, pdyy, pdevc, lop);
  return (code < 0 ? code : 1);

err:
  used.x = (rsrc - spp - psrc_initial) / spp;
  used.y = 0;
  return code;
}

#define LINE_ACCUM_COPY_DEV(devA, lineA, bpp, xo, xe, raster, yA)\
	if ( (xe) > l_xprev ) {\
	    int code;\
	    LINE_ACCUM_STORE(bpp);\
	    code = devA->copyColor(lineA, l_xprev - (xo), raster, ps_no_bitmap_id, l_xprev, yA, (xe) - l_xprev, 1);\
	    if ( code < 0 )\
	      return code;\
	}

int XWPSImageEnum::imageRenderInterpolate(const uchar * bufferA,
			 									     int data_x, 
			 									     uint , 
			 									     int h, 
			 									     XWPSDevice * devA)
{
	PSStreamImageScaleState *pss = scaler;
	ulong lop = log_op;
  int c = pss->params.Colors;
  PSStreamCursorRead r;
  PSStreamCursorWrite w;
  uchar *out = line;
  if (h != 0) 
  {
		int sizeofPixelIn = pss->params.BitsPerComponentIn / 8;
		uint row_size = pss->params.WidthIn * c * sizeofPixelIn;
		uchar *bdata = (uchar*)bufferA + data_x * c * sizeofPixelIn;

		if (sizeofPixelIn == 1) 
		{
	    if (matrix.xx >= 0) 
				r.ptr = bdata - 1;
	    else 
	    {
				uchar *p = bdata + row_size - c;
				uchar *q = out;
				int i;

				for (i = 0; i < pss->params.WidthIn; p -= c, q += c, ++i)
		    	memcpy(q, p, c);
				r.ptr = out - 1;
				out = q;
	    }
		} 
		else 
		{
	    int dc = spp;
	    uchar *pdata = bdata;
	    ushort *psrc = (ushort *)line;
	    XWPSClientColor cc;
	    int i;

	    r.ptr = (uchar *) psrc - 1;
	    for (i = 0; i < pss->params.WidthIn; i++, psrc += c) 
	    {
				int j;

				if (bps <= 8)
		    	for (j = 0; j < dc; ++pdata, ++j) 
		    	{
						decode_sample(*pdata, cc, j);
					} 
				else		/* bps == 12 */
		    	for (j = 0; j < dc; pdata += sizeof(short), ++j) 
		    	{
						decode_frac(*(short *)pdata, cc, j);
		    	}
				(pcs->concretizeColor)(&cc, psrc, pis);
	    }
	    out += ROUND_UP(pss->params.WidthIn * c * sizeof(short),  align_bitmap_mod);
		}
		r.limit = r.ptr + row_size;
  } 
  else	
		r.ptr = 0, r.limit = 0;

  {
		int xo = xyi.x;
		int yo = xyi.y;
		int width = pss->params.WidthOut;
		int sizeofPixelOut = pss->params.BitsPerComponentOut / 8;
		int dy;
		XWPSColorSpace *pconcs = pcs->concreteSpace(pis);
		int bpp = devA->color_info.depth;
		uint raster = bitmap_raster(width * bpp);

		if (matrix.yy > 0)
	    dy = 1;
		else
	    dy = -1, yo--;
		for (;;) 
		{
	    int ry = yo + line_xy * dy;
	    int x;
	    ushort *psrc;
	    XWPSDeviceColor devc;
	    int status, code;

	    DECLARE_LINE_ACCUM_COPY(out, bpp, xo);

	    w.limit = out + width *	qMax((int)(c * sizeofPixelOut), (int)(sizeof(ulong))) - 1;
	    w.ptr = w.limit - width * c * sizeofPixelOut;
	    psrc = (ushort *)(w.ptr + 1);
	    status = (*(pss->templat->process))	((PSStreamState *) pss, &r, &w, h == 0);
	    if (status < 0 && status != EOFC)
				return (int)(XWPSError::IOError);
	    if (w.ptr == w.limit) 
	    {
				int xe = xo + width;
				for (x = xo; x < xe;) 
				{
		    	code = pconcs->remapConcreteColor(pis, psrc, &devc, devA, ps_color_select_source);
		    	if (code < 0)
						return code;
		    	if (devc.isPure()) 
		    	{
						ulong color = devc.pureColor();
						if (c == 3) 
						{
			    		do 
			    		{
								LINE_ACCUM(color, bpp);
								x++, psrc += 3;
			    		} while (x < xe && psrc[-3] == psrc[0] &&  psrc[-2] == psrc[1] && psrc[-1] == psrc[2]);
						} 
						else 
						{
			    		LINE_ACCUM(color, bpp);
			    		x++, psrc += c;
						}
		    	} 
		    	else 
		    	{
						int rcode;

						LINE_ACCUM_COPY_DEV(devA, out, bpp, xo, x, raster, ry);
						rcode = devc.fillRectangle(x, ry, 1, 1, devA, lop, NULL);
						if (rcode < 0)
			    		return rcode;
						LINE_ACCUM_SKIP(bpp);
						l_xprev = x + 1;
						x++, psrc += c;
		    	}
				}
				LINE_ACCUM_COPY_DEV(devA, out, bpp, xo, x, raster, ry);
				line_xy++;
	    }
	    if ((status == 0 && r.ptr == r.limit) || status == EOFC)
				break;
		}
  }

  return (h == 0 ? 0 : 1);
}

int XWPSImageEnum::imageRenderLandscape(const uchar * bufferA,
			 									     int data_x, 
			 									     uint w, 
			 									     int h, 
			 									     XWPSDevice * devA)
{
	uint raster = bitmap_raster(line_width);
  int ix = xci, iw = wci;
  int xinc, xmod;
  uchar *row;
  const uchar *orig_row = 0;
  bool y_neg = dxy < 0;

  if (is_fneg(matrix.yx))
		ix += iw, iw = -iw, xinc = -1;
  else
		xinc = 1;
		
  if (ix != xi_next || h == 0) 
  {
		int xi = xi_next;
		int code = (xinc > 0 ? copyLandscape(line_xy, xi, y_neg, devA) :
	     					copyLandscape(xi, line_xy, y_neg, devA));

		if (code < 0)
	    return code;
		line_xy = xi_next = ix;
		if (h == 0)
	    return code;
  }
  for (; iw != 0; iw -= xinc) 
 	{
		if (xinc < 0)
	    --ix;
		xmod = ix & 7;
		row = line + xmod * raster;
		if (orig_row == 0) 
		{
	    image_simple_expand(row, 0, raster,	bufferA, data_x, w,dda_current(dda.pixel0.y),x_extent.y, 0);
	    orig_row = row;
		} 
		else
	    memcpy(row, orig_row, raster);
		if (xinc > 0) 
		{
	    ++ix;
	    if (xmod == 7) 
	    {
				int code = copyLandscape(line_xy, ix, y_neg, devA);

				if (code < 0)
		    	return code;
				orig_row = 0;
				line_xy = ix;
	    }
		} 
		else 
		{
	    if (xmod == 0) 
	    {
				int code =  copyLandscape(ix, line_xy, y_neg, devA);

				if (code < 0)
		    	return code;
				orig_row = 0;
				line_xy = ix;
	    }
		}
  }
  xi_next = ix;
  return 0;
}

int XWPSImageEnum::imageRenderMono(const uchar * bufferA, 
	                    int data_x,
		                  uint w, 
		                  int h, 
		                  XWPSDevice * devA)
{
	ulong lop = log_op;
	XWPSColorSpace *pcsA;
	XWPSClientColor cc;
  XWPSDeviceColor *pdevc = &clues[255].dev_color;
  void (XWPSImagerState::*map__gray)(short,XWPSDeviceColor *, XWPSDevice *,PSColorSelect);
  int (XWPSImagerState::*remap__color)(XWPSClientColor *,XWPSColorSpace * ,XWPSDeviceColor * ,XWPSDevice * ,PSColorSelect );
  
  bool tiles_fit = (pis && device_color ? pis->checkTileCache() : false);
  uint mask_base = mask_color.values[0];
  uint mask_limit =	(use_mask_color ?	mask_color.values[1] - mask_base + 1 : 0);
  
#define IMAGE_SET_GRAY(sample_value)\
    do{pdevc = &clues[sample_value].dev_color;\
    if (!pdevc->isSet()) {\
	if ((uint)(sample_value - mask_base) < mask_limit)\
	    pdevc->setNull();\
	else if (device_color)\
	    (pis->*map__gray)(byte2frac(sample_value), pdevc, devA, ps_color_select_source);\
	else {\
	    decode_sample(sample_value, cc, 0);\
	    code = (pis->*remap__color)(&cc, pcs, pdevc, devA, ps_color_select_source);\
	    if (code < 0)\
		goto err;\
	}\
    } else if (!pdevc->isPure()) {\
	if (!tiles_fit) {\
	    code = pdevc->loadSelect(pis, devA, ps_color_select_source);\
	    if (code < 0)\
		goto err;\
	}\
  }}while(0)
    
  PSDDAFixedPoint next;	
  PSDDAStepFixed dxx2, dxx3, dxx4;
  const uchar *psrc_initial = bufferA + data_x;
  const uchar *psrc = psrc_initial;
  const uchar *rsrc = psrc;
  const uchar *endp = psrc + w;
  const uchar *stop = endp;
  long xrun;	
  uchar run;	
  int htrun = (masked ? 255 : -2);	
  int code = 0;

  if (h == 0)
		return 0;
    
  next = dda.pixel0;
  xrun = dda_current(next.x);
  if (!masked) 
  {
		pcsA = pcs;
		if (device_color)
	    map__gray = (pcsA->getIndex() == ps_color_space_index_DeviceGray ?
		 			pis->cmap_procs->map_gray :	 &XWPSImagerState::mapGrayNo);
		else
	    remap__color = pcsA->type->remap_color;
  }
  run = *psrc;
 
  {
		uchar last = stop[-1];

		while (stop > psrc && stop[-1] == last)
	    --stop;
  }
  if (slow_loop || posture != image_portrait) 
  {
		long yrun;
		const long pdyx = dda_current(dda.row.x) - cur.x;
		const long pdyy = dda_current(dda.row.y) - cur.y;

#define xl dda_current(next.x)
#define ytf dda_current(next.y)
		yrun = ytf;
		if (masked) 
		{
	    pdevc = &clues[255].dev_color;
	    code = pdevc->load(pis, devA);
	    if (code < 0)
				return code;
	    if (stop <= psrc)
				goto last;
	    if (posture == image_portrait) 
	    {
				long ax = (matrix.xx < 0 ? -adjust : adjust);
				long ay = (pdyy < 0 ? -adjust : adjust);
				long dyy = pdyy + (ay << 1);

				yrun -= ay;
				dda_translate(next.x, -ax);
				ax <<= 1;
				dxx2 = next.x.step;
				dda_step_add(dxx2, next.x.step);
				dxx3 = dxx2;
				dda_step_add(dxx3, next.x.step);
				dxx4 = dxx3;
				dda_step_add(dxx4, next.x.step);
				for (;;) 
				{	
		    	while (!psrc[0])
						if (!psrc[1]) 
						{
			    		if (!psrc[2]) 
			    		{
								if (!psrc[3]) 
								{
				    			psrc += 4;
				    			dda_state_next(next.x.state, dxx4);
				    			continue;
								}
								psrc += 3;
								dda_state_next(next.x.state, dxx3);
								break;
			    		}
			    		psrc += 2;
			    		dda_state_next(next.x.state, dxx2);
			    		break;
						} 
						else 
						{
			    		++psrc;
			    		dda_next(next.x);
			    		break;
						}
		    		xrun = xl;
		    		if (psrc >= stop)
							break;
		    		for (; *psrc; ++psrc)
							dda_next(next.x);
		    		code = devA->fillParallelogram(xrun, yrun,	 xl - xrun + ax, fixed_0, fixed_0, dyy, pdevc, lop);
		    		if (code < 0)
							goto err;
		    		rsrc = psrc;
		    		if (psrc >= stop)
							break;
					}
	    	} 
	    	else if (posture == image_landscape) 
	    	{
					long ax = (pdyx < 0 ? -adjust : adjust);
					long dyx = pdyx + (ax << 1);
					long ay = (matrix.xy < 0 ? -adjust : adjust);

					xrun -= ax;
					dda_translate(next.y, -ay);
					ay <<= 1;
					for (;;) 
					{
		    		for (; !*psrc; ++psrc)
							dda_next(next.y);
		    		yrun = ytf;
		    		if (psrc >= stop)
							break;
		    		for (; *psrc; ++psrc)
							dda_next(next.y);
		    		code = devA->fillParallelogram(xrun, yrun, fixed_0, ytf - yrun + ay, dyx, fixed_0, pdevc, lop);
		    		if (code < 0)
							goto err;
		    		rsrc = psrc;
		    		if (psrc >= stop)
							break;
					}
	    	} 
	    	else 
	    	{
					for (;;) 
					{
		    		for (; !*psrc; ++psrc) 
		    		{
							dda_next(next.x);
							dda_next(next.y);
		    		}
		    		yrun = ytf;
		    		xrun = xl;
		    		if (psrc >= stop)
							break;
		    		for (; *psrc; ++psrc) 
		    		{
							dda_next(next.x);
							dda_next(next.y);
		    		}
		    		code = devA->fillParallelogram(xrun, yrun, xl - xrun,ytf - yrun, pdyx, pdyy, pdevc, lop);
		    		if (code < 0)
							goto err;
		    		rsrc = psrc;
		    		if (psrc >= stop)
							break;
					}
	    	}
			} 
			else if (posture == image_portrait || posture == image_landscape) 
			{
	    	if (stop <= psrc)
					goto last;
	    	for (;;) 
	    	{
					if (*psrc != run) 
					{
		    		if (run != htrun) 
		    		{
							htrun = run;
							IMAGE_SET_GRAY(run);
		    		}
		    		code = devA->fillParallelogram(xrun, yrun, xl - xrun, ytf - yrun, pdyx, pdyy, pdevc, lop);
		    		if (code < 0)
							goto err;
		    		yrun = ytf;
		    		xrun = xl;
		    		rsrc = psrc;
		    		if (psrc >= stop)
							break;
		    		run = *psrc;
					}
					psrc++;
					dda_next(next.x);
					dda_next(next.y);
	    	}
			} 
			else 
			{
	    	stop = endp;
	    	for (;;) 
	    	{
					if (run != htrun) 
					{
		    		htrun = run;
		    		IMAGE_SET_GRAY(run);
					}
					code = devA->fillParallelogram(xrun, yrun, xl - xrun, ytf - yrun, pdyx, pdyy, pdevc, lop);
					if (code < 0)
		    		goto err;
					yrun = ytf;
					xrun = xl;
					rsrc = psrc;
					if (psrc >= stop)
		    		break;
					run = *psrc++;
					dda_next(next.x);
					dda_next(next.y);	
	    	}
			}
			
last:
			if (stop < endp && (*stop || !masked)) 
			{
	    	if (!masked) 
	    	{
					IMAGE_SET_GRAY(*stop);
	    	}
	    	dda_advance(next.x, endp - stop);
	    	dda_advance(next.y, endp - stop);
	    	code = devA->fillParallelogram(xrun, yrun, xl - xrun,  ytf - yrun, pdyx, pdyy, pdevc, lop);
			}
#undef xl
#undef ytf
    } 
    else 
    {
			long xa = (dxx >= 0 ? adjust : -adjust);
			const int yt = yci, iht = hci;
			int bstart;
			int phase_x;
			int tile_offset = (pis && device_color &&  devA->getBand(yt, &bstart) == 0 ?
	     		pis->checkTileSize(fixed2int_ceiling(qAbs(dxx) + (xa << 1)),
	     		    yt, iht, ps_color_select_source, &phase_x) :  -1);
			int xmin = fixed2int_pixround(clip_outer.p.x);
			int xmax = fixed2int_pixround(clip_outer.q.x);

#define xl dda_current(next.x)

			xrun = xrun - xa + (fixed_half - fixed_epsilon);
			dda_translate(next.x, xa + (fixed_half - fixed_epsilon));
			xa <<= 1;
			dxx2 = next.x.step;
			dda_step_add(dxx2, next.x.step);
			dxx3 = dxx2;
			dda_step_add(dxx3, next.x.step);
			dxx4 = dxx3;
			dda_step_add(dxx4, next.x.step);
			if (stop > psrc)
	    	for (;;) 
	    	{	
	      
skf:
					if (psrc[0] == run) 
					{
		    		if (psrc[1] == run) 
		    		{
							if (psrc[2] == run) 
							{
			    			if (psrc[3] == run) 
			    			{
									psrc += 4;
									dda_state_next(next.x.state, dxx4);
									goto skf;
			    			} 
			    			else 
			    			{
									psrc += 4;
									dda_state_next(next.x.state, dxx3);
			    			}
							} 
							else 
							{
			    			psrc += 3;
			    			dda_state_next(next.x.state, dxx2);
							}
		    		} 
		    		else 
		    		{
							psrc += 2;
							dda_next(next.x);
		    		}
					} 
					else
		    		psrc++;
					{	
		    		int xi = fixed2int_var(xrun);
		    		int wi = fixed2int_var(xl) - xi;
		    		int xei, tsx;
		    		XWPSStripBitmap *tile;

		    		if (wi <= 0) 
		    		{
							if (wi == 0)
			    			goto mt;
							xi += wi, wi = -wi;
		    		}
		    		if ((xei = xi + wi) > xmax || xi < xmin) 
		    		{
							if (xi < xmin)
			    			wi -= xmin - xi, xi = xmin;
							if (xei > xmax)
			    			wi -= xei - xmax;
							if (wi <= 0)
			    			goto mt;
		    		}
		    		switch (run) 
		    		{
							case 0:
			    			if (masked)
									goto mt;
			    			if (!clues[0].dev_color.isPure())
									goto ht;
			    			code = devA->fillRectangle(xi, yt, wi, iht, clues[0].dev_color.pureColor());
			    			break;
			    			
							case 255:
			    			if (!clues[255].dev_color.isPure())
									goto ht;
			    			code = devA->fillRectangle(xi, yt, wi, iht, clues[255].dev_color.pureColor());
			    			break;
			    			
							default:
ht:	
			    			if (run != htrun) 
			    			{
									IMAGE_SET_GRAY(run);
									htrun = run;
			    			}
			    			if (pdevc->isPure()) 
									code = devA->fillRectangle(xi, yt, wi, iht, pdevc->pureColor());
			    			else if (!pdevc->isBinaryHalftone()) 
									code =  pdevc->fillRectangle(xi, yt, wi, iht, devA, lop, NULL);
			    			else if (tile_offset >= 0 &&	(tile = &pdevc->colors.binary.b_tile->tiles,
				 						(tsx = (xi + phase_x) % tile->rep_width) + wi <= tile->size.x)) 
				 				{	
									uchar *row = tile->data + tile_offset;

									code = devA->copyMono(row, tsx, tile->raster, ps_no_bitmap_id,
				     						xi, yt, wi, iht,  pdevc->colors.binary.color[0], pdevc->colors.binary.color[1]);
			    			} 
			    			else 
			    			{
									code = devA->stripTileRectangle(	&pdevc->colors.binary.b_tile->tiles,
						     						xi, yt, wi, iht, pdevc->colors.binary.color[0],   pdevc->colors.binary.color[1],
					    							pdevc->phase.x, pdevc->phase.y);
			    			}
		    			}
		    			if (code < 0)
								goto err;
mt:
							xrun = xl - xa;	
		    			rsrc = psrc - 1;
		    			if (psrc > stop) 
		    			{
								--psrc;
								break;
		    			}
		    			run = psrc[-1];
						}
						dda_next(next.x);
	    		}
	    		
					if (*stop != 0 || !masked) 
					{
	    			int xi = fixed2int_var(xrun);
	    			int wi, xei;

	    			dda_advance(next.x, endp - stop);
	    			wi = fixed2int_var(xl) - xi;
	    			if (wi <= 0) 
	    			{
							if (wi == 0)
		    				goto lmt;
							xi += wi, wi = -wi;
	    			}
	    			if ((xei = xi + wi) > xmax || xi < xmin) 
	    			{
							if (xi < xmin)
		    				wi -= xmin - xi, xi = xmin;
							if (xei > xmax)
		    				wi -= xei - xmax;
							if (wi <= 0)
		    				goto lmt;
	    			}
	    			IMAGE_SET_GRAY(*stop);
	    			code = pdevc->fillRectangle(xi, yt, wi, iht,devA, lop, NULL);
lmt:
						;
					}
    }
#undef xl
    if (code >= 0)
			return 1;
err:
   used.x = rsrc - psrc_initial;
   used.y = 0;
   return code;
}

int XWPSImageEnum::imageRenderSimple(const uchar * bufferA, 
	                     int data_x,
		                   uint w, 
		                   int h, 
		                   XWPSDevice * devA)
{
  const uchar *lineA;
  uint line_widthA, line_sizeA;
  int line_x;
  long xcur = dda_current(dda.pixel0.x);
  int ix = fixed2int_pixround(xcur);
  int ixr;
  const int iy = yci, ih = hci;
  XWPSDeviceColor * pdc0 = &clues[0].dev_color;
  XWPSDeviceColor * pdc1 = &clues[255].dev_color;
  int dy;
  int code;

  if (h == 0)
		return 0;
    
  if ((!pdc0->isNull() && (code = pdc0->load(pis, devA)) < 0) ||
			(!pdc1->isNull() && (code = pdc1->load(pis, devA)) < 0)	)
		return code;
  
  if (line == 0) 
  {	
		lineA = bufferA;
		line_size = (w + 7) >> 3;
		line_width = w;
		line_x = 0;
  } 
  else if (devA->procs.copy_mono_.copy_monomem == &XWPSDeviceMem::copyMonoMono &&
	       		dxx > 0 && pdc1->isPure() && pdc0->isPure() &&
	       	(pdc0->pureColor() ^ pdc1->pureColor()) == 1 &&
	       	!clip_image &&  ix >= 0 &&
	       	(ixr = fixed2int_pixround(xcur + x_extent.x) - 1) < devA->width &&
	       iy >= 0 && iy + ih <= devA->height) 
	{
		int line_ix;
		int ib_left = ix >> 3, ib_right = ixr >> 3;
		uchar *scan_line = ((XWPSDeviceMem*)devA)->scanLineBase(iy);
		uchar save_left, save_right, mask;

		line_x = ix & (align_bitmap_mod * 8 - 1);
		line_ix = ix - line_x;
		line_sizeA = (ixr >> 3) + 1 - (line_ix >> 3);
		line_widthA = ixr + 1 - ix;
		save_left = scan_line[ib_left];
		save_right = scan_line[ib_right];
		image_simple_expand(scan_line + (line_ix >> 3), line_x, line_size, bufferA, data_x, w, xcur,
			    x_extent.x,  ((pdc0->colors.pure == 0) != (map[0].table.lookup4x1to32[0] == 0) ?  0xff : 0));
		if (ix & 7)
	    mask = (uchar) (0xff00 >> (ix & 7)),
		scan_line[ib_left] = (save_left & mask) + (scan_line[ib_left] & ~mask);
		if ((ixr + 1) & 7)
	    mask = (uchar) (0xff00 >> ((ixr + 1) & 7)),		scan_line[ib_right] =	(scan_line[ib_right] & mask) + (save_right & ~mask);
		if (ih <= 1)
	    return 1;
		lineA = scan_line + (line_ix >> 3);
		if (dxx < 0)
	    ix -= line_widthA;
		for (dy = 1; dy < ih; dy++) 
		{
	    int code = devA->copyMono(line, line_x, line_sizeA, ps_no_bitmap_id, ix, iy + dy, line_widthA, 1, 0, 1);

	    if (code < 0)
				return code;
		}
		return 0;
    
  } 
  else 
  {
		lineA = line;
		line_sizeA = line_size;
		line_widthA = line_width;
		line_x = ix & (align_bitmap_mod * 8 - 1);
		image_simple_expand(line, line_x, line_sizeA,  bufferA, data_x, w, xcur, x_extent.x, 0);
  }

  if (dxx < 0)
		ix -= line_widthA;
  for (dy = 0; dy < ih; dy++) 
  {
		int code = copyPortrait(lineA, line_x, line_sizeA,ix, iy + dy, line_widthA, 1, devA);

		if (code < 0)
	    return code;
  }

  return 1;
}

int XWPSImageEnum::imageRenderSkip(const uchar * , 
	                    int ,
		                  uint , 
		                  int h, 
		                  XWPSDevice * )
{
	return h;
}

void XWPSImageEnum::scaleMaskColors(int component_index)
{
	uint scale = 255 / ((1 << bps) - 1);
  uint *values = &mask_color.values[component_index * 2];
  uint v0 = values[0] *= scale;
  uint v1 = values[1] *= scale;

  if (map[component_index].decoding == sd_none &&	map[component_index].inverted) 
  {
		values[0] = 255 - v1;
		values[1] = 255 - v0;
  }
}

void XWPSImageEnum::clear()
{
	if (rop_dev)
  {
  	if (!rop_dev->decRef())
  		delete rop_dev;
  		
  	rop_dev = 0;
  }
  
  if (clip_dev)
  {
  	if (!clip_dev->decRef())
  		delete clip_dev;
  		
  	clip_dev = 0;
  }
  
  if (scaler)
  {
  	if (scaler->templat)
  	{
  		if (scaler->templat->release)
  			(*(scaler->templat->release))((PSStreamState *) scaler);
  	}
  	
  	delete scaler;
  	scaler = 0;
  }
  
  if (line)
  {
  	delete [] line;
  	line = 0;
  }
  
  if (buffer)
  {
  	delete [] buffer;
  	buffer = 0;
  }
}

int XWPSImageEnum::color_draws_b_w(XWPSDevice * devA, XWPSDeviceColor * pdcolor)
{
	if (pdcolor->isPure()) 
	{
		ushort rgb[3];
		devA->mapColorRGB(pdcolor->pureColor(), rgb);
		if (!(rgb[0] | rgb[1] | rgb[2]))
	    return 0;
		if ((rgb[0] & rgb[1] & rgb[2]) == ps_max_color_value)
	    return 1;
  }
  return -1;
}

int XWPSImageEnum::copyLandscape(int x0, int x1, bool y_neg, XWPSDevice * devA)
{
	uint line_widthA = line_width;
  uint raster = bitmap_raster(line_widthA);
  uchar *flipped = line + raster * 8;
  int w = x1 - x0;
  int yA = fixed2int_pixround(dda_current(dda.pixel0.y));

  if (w == 0 || line_widthA == 0)
		return 0;
		
  if (line_widthA > 0) 
  {
		int i;

		for (i = (line_widthA - 1) >> 3; i >= 0; --i)
	    memflip8x8(line + i, raster, flipped + (i << (log2_align_bitmap_mod + 3)), align_bitmap_mod);
  }
  if (w < 0)
		x0 = x1, w = -w;
  if (y_neg)
		yA -= line_widthA;
  return copyPortrait(flipped, x0 & 7, align_bitmap_mod,x0, yA, w, line_widthA, devA);
}

int XWPSImageEnum::copyPortrait(const uchar * data, 
	                 int dx, int raster,
	                 int x, 
	                 int yA, 
	                 int w, 
	                 int h,
	                  XWPSDevice * devA)
{
	XWPSDeviceColor *pdc0;
  XWPSDeviceColor *pdc1;
  uint align = ALIGNMENT_MOD(data, align_bitmap_mod);
  if (map[0].table.lookup4x1to32[0])
		pdc0 = &clues[255].dev_color, pdc1 = &clues[0].dev_color;
  else
		pdc0 = &clues[0].dev_color, pdc1 = &clues[255].dev_color;
  data -= align;
  dx += align << 3;
  if (pdc0->isPure() && pdc1->isPure()) 
  {
  	if (h == 1 || (raster & (align_bitmap_mod - 1)) == 0)
  		return devA->copyMono(data, dx, raster, ps_no_bitmap_id,  x, yA, w, h, pdc0->colors.pure, pdc1->pureColor());
  	else
  		return devA->copyMonoUnaligned(data, dx, raster, ps_no_bitmap_id,  x, yA, w, h, pdc0->colors.pure, pdc1->pureColor());	
  }
  
  {
		XWPSDeviceColor *pdc;
		bool invert;

		if (pdc1->isNull()) 
		{
	    pdc = pdc0;
	    invert = true;
		} 
		else 
		{
	    if (!pdc0->isNull()) 
	    {
				int code = pdc0->fillRectangle(x, yA, w, h, devA, lop_default, NULL);

			if (code < 0)
		    return code;
	  	}
	  	pdc = pdc1;
	  	invert = false;
		}
		return pdc->fillMasked(data, dx, raster, ps_no_bitmap_id, x, yA, w, h, devA, lop_default, invert);

  }
}

void XWPSImageEnum::fill_row(uchar *lineA, int line_x, uint raster, int value)
{
	memset(lineA + (line_x >> 3), value, raster - (line_x >> 3));
}

void XWPSImageEnum::image_simple_expand(uchar * lineA, 
	                                int line_x, 
	                                uint raster,
		                              const uchar * bufferA, 
		                              int data_x, 
		                              uint w,
		    													long xcur, 
		    													long x_extent, 
		    													uchar zero)
{
	int dbitx = data_x & 7;
  uchar sbit = 0x80 >> dbitx;
  uchar sbitmask = 0xff >> dbitx;
  uint wx = dbitx + w;
  PSDDAFixed xl;
  PSDDAStepFixed dxx4, dxx8, dxx16, dxx24, dxx32;
  const uchar *psrc = bufferA + (data_x >> 3);
  
  const uchar *endp = psrc + (wx >> 3);
  int endx = ~wx & 7;
  uchar endbit = 1 << endx;
  
  const uchar *stop = endp;
  int stopx;
  uchar stopbit = endbit;
  uchar data;
  uchar one = ~zero;
  long xl0;

  if (w == 0)
		return;
		
	if (stopbit == 0x80)
		--stop, stopbit = 1;
  else
		stopbit <<= 1;

  {
		uchar stopmask = -stopbit << 1;
		uchar last = *stop;

		if (stop == psrc)	
	    stopmask &= sbitmask;
		if (last & stopbit) 
		{
	    if (~last & stopmask) 
				last |= stopbit - 1;
	    else 
	    {	
				while (stop > psrc && stop[-1] == 0xff)
		    	--stop;
				if (stop == psrc || (stop == psrc + 1 && !(~*psrc & sbitmask))) 
				{
		    	fill_row(lineA, line_x, raster, one);
		    	return;
				}
				last = *--stop;
	    }
	    stopx = byte_bit_run_length_0[byte_reverse_bits[last]] - 1;
		} 
		else 
		{
	    if (last & stopmask) 
				last &= -stopbit;
	    else 
	    {
				while (stop > psrc && stop[-1] == 0)
		    	--stop;
				if (stop == psrc || (stop == psrc + 1 && !(*psrc & sbitmask))) 
				{
		    	fill_row(lineA, line_x, raster, zero);
		    	return;
				}
				last = *--stop;
	    }
	    stopx = byte_bit_run_length_0[byte_reverse_bits[last ^ 0xff]] - 1;
		}
		if (stopx < 0)
	    stopx = 7, ++stop;
		stopbit = 1 << stopx;
  }

  fill_row(lineA, line_x, raster, zero);

  xl0 =	(x_extent >= 0 ?	 fixed_fraction(fixed_pre_pixround(xcur)) :
	 	fixed_fraction(fixed_pre_pixround(xcur + x_extent)) - x_extent);
  xl0 += int2fixed(line_x);
  dda_init(xl, xl0, x_extent, w);
  dxx4 = xl.step;
  dda_step_add(dxx4, xl.step);
  dda_step_add(dxx4, dxx4);
  dxx8 = dxx4;
  dda_step_add(dxx8, dxx4);
  dxx16 = dxx8;
  dda_step_add(dxx16, dxx8);
  dxx24 = dxx16;
  dda_step_add(dxx24, dxx8);
  dxx32 = dxx24;
  dda_step_add(dxx32, dxx8);

  
  for (data = *psrc;;) 
  {
		int x0, n, bit;
		uchar *bp;
		static const uchar lmasks[9] = {
	    0xff, 0x7f, 0x3f, 0x1f, 0xf, 7, 3, 1, 0	};
		static const uchar rmasks[9] = {
	    0, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff	};

		data ^= 0xff;	
		while (data & sbit) 
		{
	    dda_next(xl);
	    sbit >>= 1;
		}
		if (!sbit) 
		{	
sw:	    
			if ((data = psrc[1]) != 0) 
				psrc++;
	    else if ((data = psrc[2]) != 0) 
	    {
				dda_state_next(xl.state, dxx8);
				psrc += 2;
	    } 
	    else if ((data = psrc[3]) != 0) 
	    {
				dda_state_next(xl.state, dxx16);
				psrc += 3;
	    } 
	    else if ((data = psrc[4]) != 0) 
	    {
				dda_state_next(xl.state, dxx24);
				psrc += 4;
	    } 
	    else 
	    {
				dda_state_next(xl.state, dxx32);
				psrc += 4;
				goto sw;
	    }
	    if (data > 0xf)
				sbit = 0x80;
	    else 
	    {
				sbit = 0x08;
				dda_state_next(xl.state, dxx4);
	    }
	    data ^= 0xff;	
	    while (data & sbit) 
	    {
				dda_next(xl);
				sbit >>= 1;
	    }
		}
		x0 = dda_current_fixed2int(xl);
		if (psrc >= stop && sbit == stopbit) 
		{
	    n = fixed2int(xl0 + x_extent) - x0;
		} 
		else 
		{	
	    data ^= 0xff;	
	    do 
	    {
				dda_next(xl);
				sbit >>= 1;
	    }
	    while (data & sbit);
	    	if (!sbit) 
	    	{	
					while ((data = *++psrc) == 0xff) 
					{
		    		dda_state_next(xl.state, dxx8);
					}
					if (data < 0xf0)
		    		sbit = 0x80;
					else 
					{
		    		sbit = 0x08;
		    		dda_state_next(xl.state, dxx4);
					}
					while (data & sbit) 
					{
		    		dda_next(xl);
		    		sbit >>= 1;
					}
	    	}
	    n = dda_current_fixed2int(xl) - x0;
		}

		if (n < 0)
	    x0 += n, n = -n;
		bp = lineA + (x0 >> 3);
		bit = x0 & 7;
		if ((n += bit) <= 8) 
		{
	    *bp ^= lmasks[bit] - lmasks[n];
		} 
		else if ((n -= 8) <= 8) 
		{
	    *bp ^= lmasks[bit];
	    bp[1] ^= rmasks[n];
		} 
		else 
		{
	    *bp++ ^= lmasks[bit];
	    if (n >= 56) 
	    {
				int nb = n >> 3;

				memset(bp, one, nb);
				bp += nb;
	    } 
	    else 
	    {
				while ((n -= 8) >= 0)
		    	*bp++ = one;
	    }
	    *bp ^= rmasks[n & 7];
		}
		if (psrc >= stop && sbit == stopbit)
	    break;
  }
}

void XWPSImageEnum::initColors(int bpsA, 
								               int sppA,
		  					               PSImageFormat formatA, 
		  					               const float *decode ,
		  					               XWPSImagerState * pisA, 
		  					               XWPSDevice * devA,
		  					               XWPSColorSpace * pcsA, 
		  					               bool * pdcb)
{
	int ci;
  static const float default_decode[] = {
		0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0 };

    /* Initialize the color table */

#define ictype(i)\
   clues[i].dev_color.type
   
  switch ((sppA == 1 ? bpsA : 8))
  {
  	case 8:
  		{
				XWPSImageClue *pcht = &clues[0];
				int n = 64;

				do 
				{
		    	pcht[0].dev_color.type = pcht[1].dev_color.type =	
		    	pcht[2].dev_color.type =	pcht[3].dev_color.type = &ps_dc_type_data_none;
		    	pcht[0].key = pcht[1].key =	pcht[2].key = pcht[3].key = 0;
		    	pcht += 4;
				}	while (--n > 0);
				clues[0].key = 1;
				break;
	    }
	    
	  case 4:
	    ictype(17) = ictype(2 * 17) = ictype(3 * 17) =
			ictype(4 * 17) = ictype(6 * 17) = ictype(7 * 17) =
			ictype(8 * 17) = ictype(9 * 17) = ictype(11 * 17) =
			ictype(12 * 17) = ictype(13 * 17) = ictype(14 * 17) = &ps_dc_type_data_none;
		case 2:
	    ictype(5 * 17) = ictype(10 * 17) = &ps_dc_type_data_none;
#undef ictype
  }
  
  for (ci = 0; ci < sppA; ci++)
  {
  	PSSampleMap *pmap = &map[ci];
  	const float *this_decode = &decode[ci * 2];
		const float *map_decode;
		const float *real_decode;
		bool no_decode;

		map_decode = real_decode = this_decode;
		if (map_decode[0] == 0.0 && map_decode[1] == 1.0)
	    no_decode = true;
		else if (map_decode[0] == 1.0 && map_decode[1] == 0.0 && bpsA <= 8) 
		{
	    no_decode = true;
	    real_decode = default_decode;
		} 
		else 
		{
	    no_decode = false;
	    *pdcb = false;
	    map_decode = default_decode;
		}
		
		if (bpsA > 2 || formatA != ps_image_format_chunky) 
		{
	    if (bpsA <= 8)
				initMap(&pmap->table.lookup8[0], 1 << bpsA, map_decode);
		}
		else
		{
			uchar mapA[4];
	    initMap(&mapA[0], 1 << bpsA, map_decode);
	    switch (bpsA) 
	    {
				case 1:
		    	{
						ulong *p = &pmap->table.lookup4x1to32[0];
						if (mapA[0] == 0 && mapA[1] == 0xff)
			    		memcpy((uchar *) p, lookup4x1to32_identity, 16 * 4);
						else if (mapA[0] == 0xff && mapA[1] == 0)
			    		memcpy((uchar *) p, lookup4x1to32_inverted, 16 * 4);
						else
			    		for (int i = 0; i < 16; i++, p++)
								((uchar *) p)[0] = mapA[i >> 3],  ((uchar *) p)[1] = mapA[(i >> 2) & 1],
				    		((uchar *) p)[2] = mapA[(i >> 1) & 1],  ((uchar *) p)[3] = mapA[i & 1];
		    	}
		    	break;
		    	
				case 2:
		    	{
						ushort *p = &pmap->table.lookup2x2to16[0];

						for (int i = 0; i < 16; i++, p++)
			    		((uchar *) p)[0] = mapA[i >> 2], ((uchar *) p)[1] = mapA[i & 3];
		    }
		    break;
	    }
		}
		
		pmap->decode_base = real_decode[0];
		pmap->decode_factor = (real_decode[1] - real_decode[0]) / (bpsA <= 8 ? 255.0 : (float)frac_1);
		pmap->decode_max = real_decode[1];
		if (no_decode) 
		{
	    pmap->decoding = sd_none;
	    pmap->inverted = map_decode[0] != 0;
		} 
		else if (bpsA <= 4) 
		{
	    int stepA = 15 / ((1 << bpsA) - 1);
	    pmap->decoding = sd_lookup;
	    for (int i = 15 - stepA; i > 0; i -= stepA)
				pmap->decode_lookup[i] = pmap->decode_base +  i * (255.0 / 15) * pmap->decode_factor;
		} 
		else
	    pmap->decoding = sd_compute;
		if (sppA == 1) 
		{
	    XWPSClientColor ccA;
	    ccA.paint.values[0] = real_decode[0];
	    pcsA->remapColor(pisA, &ccA, pcsA, &clues[0].dev_color, devA, ps_color_select_source);
	    ccA.paint.values[0] = real_decode[1];
	    pcsA->remapColor(pisA, &ccA, pcsA, &clues[255].dev_color, devA, ps_color_select_source);
		}
  }
}

void XWPSImageEnum::initMap(uchar * mapA, int map_size, const float *decode)
{
	float min_v = decode[0];
  float diff_v = decode[1] - min_v;
  if (diff_v == 1 || diff_v == -1) 
  {
		uchar *limit = mapA + map_size;
		uint value = min_v * 0xffffL;
		int diff = diff_v * (0xffffL / (map_size - 1));

		for (; mapA != limit; mapA++, value += diff) *mapA = value >> 8;   
	} 
	else 
	{
		for (int i = 0; i < map_size; ++i) 
		{
	    int value = (int)((min_v + diff_v * i / (map_size - 1)) * 255);
	    mapA[i] = (value < 0 ? 0 : value > 255 ? 255 : value);
		}
  }
}

bool XWPSImageEnum::maskColor12Matches(const short *v, int num_components)
{
	int i;

  for (i = num_components * 2, v += num_components - 1; (i -= 2) >= 0; --v)
		if (*v < mask_color.values[i] || *v > mask_color.values[i + 1] )
	    return false;
  return true;
}

bool XWPSImageEnum::maskColorMatches(const uchar *v, int num_components)
{
	int i;

  for (i = num_components * 2, v += num_components - 1; (i -= 2) >= 0; --v)
		if (*v < mask_color.values[i] || *v > mask_color.values[i + 1])
	    return false;
  return true;
}

void XWPSImageEnum::repackBitPlanes(const PSImagePlane *src_planes, 
	                                  const ulong *offsets,
		                                int num_planesA, 
		                                uchar *bufferA, 
		                                int width,
		                                const PSSampleLookup * ptab, 
		                                int spreadA)
{
	PSImagePlane planes[8];
  uchar *zeros = 0;
  uchar *dest = bufferA;
  int any_data_x = 0;
  bool direct = (spreadA == 1 && ptab->lookup8[0] == 0 &&  ptab->lookup8[255] == 255);
  int pi, x;
  PSImagePlane *pp;
  
  for (pi = 0, pp = planes; pi < num_planesA; ++pi, ++pp)
		if (src_planes[pi].data == 0) 
		{
	    if (!zeros) 
				zeros = bufferA + width - ((width + 7) >> 3);
	    pp->data = zeros;
	    pp->data_x = 0;
		} 
		else 
		{
	    int dx = src_planes[pi].data_x;

	    pp->data = src_planes[pi].data + (dx >> 3) + offsets[pi];
	    any_data_x |= (pp->data_x = dx & 7);
		}
  if (zeros)
		memset(zeros, 0, bufferA + width - zeros);
		
	for (x = 0; x < width; x += 8)
	{
		ulong w0 = 0, w1 = 0;
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
		static const quint32 expand[16] = {
	    0x00000000, 0x00000001, 0x00000100, 0x00000101,
	    0x00010000, 0x00010001, 0x00010100, 0x00010101,
	    0x01000000, 0x01000001, 0x01000100, 0x01000101,
	    0x01010000, 0x01010001, 0x01010100, 0x01010101
		};
#else
		static const quint32 expand[16] = {
	    0x00000000, 0x01000000, 0x00010000, 0x01010000,
	    0x00000100, 0x01000100, 0x00010100, 0x01010100,
	    0x00000001, 0x01000001, 0x00010001, 0x01010001,
	    0x00000101, 0x01000101, 0x00010101, 0x01010101
		};
#endif

		if (any_data_x) 
		{
	    for (pi = 0, pp = planes; pi < num_planesA; ++pi, ++pp) 
	    {
				uint b = *(pp->data++);
				int dx = pp->data_x;

				if (dx) 
				{
		    	b <<= dx;
		    	if (x + 8 - dx < width)
						b += *pp->data >> (8 - dx);
				}
				w0 = (w0 << 1) | expand[b >> 4];
				w1 = (w1 << 1) | expand[b & 0xf];
	    }
		} 
		else 
		{
	    for (pi = 0, pp = planes; pi < num_planesA; ++pi, ++pp) 
	    {
				uint b = *(pp->data++);

				w0 = (w0 << 1) | expand[b >> 4];
				w1 = (w1 << 1) | expand[b & 0xf];
	    }
		}
		
		if (direct) 
		{
	    ((ulong *)dest)[0] = w0;
	    ((ulong *)dest)[1] = w1;
	    dest += 8;
		} 
		else 
		{
#define MAP_BYTE(v) (ptab->lookup8[(uchar)(v)])
	    dest[0] = MAP_BYTE(w0 >> 24); dest += spreadA;
	    dest[1] = MAP_BYTE(w0 >> 16); dest += spreadA;
	    dest[2] = MAP_BYTE(w0 >> 8); dest += spreadA;
	    dest[3] = MAP_BYTE(w0); dest += spreadA;
	    dest[4] = MAP_BYTE(w1 >> 24); dest += spreadA;
	    dest[5] = MAP_BYTE(w1 >> 16); dest += spreadA;
	    dest[6] = MAP_BYTE(w1 >> 8); dest += spreadA;
	    dest[7] = MAP_BYTE(w1); dest += spreadA;
#undef MAP_BYTE
		}
	}
}

XWPSDevice * XWPSImageEnum::setupImageDevice()
{
	XWPSDevice *retdev = dev;

  if (clip_dev) 
  {
		clip_dev->setTarget(retdev);
		retdev = clip_dev;
  }
  if (rop_dev) 
  {
		rop_dev->setTarget(retdev);
		retdev = rop_dev;
  }
  return retdev;
}

void XWPSImageEnum::updateStrip()
{
	dda_translate(dda.strip.x, cur.x - prev.x);
  dda_translate(dda.strip.y, cur.y - prev.y);
  dda.pixel0 = dda.strip;
}

XWPSImageEnumPlane::XWPSImageEnumPlane()
{
	pos = 0;
}

XWPSImageEnumPlane::~XWPSImageEnumPlane()
{
}

XWPSImageEnums::XWPSImageEnums()
	:XWPSStruct()
{
	init();
	height = 0;
	wanted_varies = false;
	y = 0;
	error = false;
}

XWPSImageEnums::~XWPSImageEnums()
{
	freeRowBuffers(num_planes);
  if (dev != 0)
  {
  	if (info)
  	{
			info->end(!error);
			delete info;
			info = 0;
		}
	}
}

int XWPSImageEnums::getLength()
{
	return sizeof(XWPSImageEnums);
}

const char * XWPSImageEnums::getTypeName()
{
	return "imageenums";
}

int XWPSImageEnums::imageCommonInit(XWPSImageEnumCommon * pie,
	                     const XWPSDataImage * pim, 
	                     XWPSDevice * devA)
{
	if (pim->Width == 0 || pim->Height == 0) 
	{
		pie->end(false);
		return 1;
  }
  
  dev = devA;
  info = pie;
  num_planes = pie->num_planes;
  
  height = pim->Height;
  for (int i = 0; i < pie->num_planes; ++i)
  {
  	planes[i].pos = 0;
  	planes[i].source.size = 0;
  	planes[i].row.data = 0;
  	planes[i].row.size = 0;
		image_planes[i].data_x = 0;
  }
  y = 0;
  error = false;
  wanted_varies = true;
  beginPlanes();
  return 0;
}

int XWPSImageEnums::imageEnumInit(XWPSImageEnumCommon * pie,
		                const XWPSDataImage * pim, 
		                XWPSState *pgs)
{
	return imageCommonInit(pie, pim, (pgs->in_charpath ? NULL : pgs->currentDevice()));
}

int XWPSImageEnums::imageInit(XWPSImage1 * pim, bool , XWPSState * pgs)
{
	XWPSImage1 image;
  XWPSImageEnumCommon *pie;
  int code;

  image = *pim;
  if (image.ImageMask) 
  {
		image.ColorSpace = NULL;
		if (pgs->in_cachedevice <= 1)
	    image.adjust = false;
  } 
  else 
  {
		if (pgs->in_cachedevice)
	    return (int)(XWPSError::Undefined);
		if (image.ColorSpace == NULL)
	    image.ColorSpace =pgs->deviceGray();
  }
  code = image.beginTyped(pgs, image.ImageMask | image.CombineWithColor,&pie);
  if (code < 0)
		return code;
  return imageEnumInit(pie, &image, pgs);
}

int  XWPSImageEnums::imageNext(const uchar * dbytes, uint dsize, uint * pused)
{
	int px = plane_index;
  uint used[ps_image_max_planes];
  XWPSString plane_data[ps_image_max_planes];

  if (planes[px].source.size != 0)
		return (int)(XWPSError::RangeCheck);
			
  plane_data[px].data = (uchar*)dbytes;
  plane_data[px].size = dsize;
  error = false;
  int  code = imageNextPlanes(plane_data, used);
  *pused = used[px];
  if (code >= 0)
		nextPlane();
  return code;
}

int  XWPSImageEnums::imageNextPlanes(XWPSString *plane_data, uint *used)
{
	int code = 0;
	for (int i = 0; i < num_planes; ++i)
	{
		used[i] = 0;
		if (wanted[i] && plane_data[i].size != 0) 
		{
	    planes[i].source.size = plane_data[i].size;
	    planes[i].source.data = plane_data[i].data;
		}
	}
	
	for (;;)
	{
		int h = (wanted_varies ? 1 : max_int);
		for (int i = 0; i < num_planes; ++i)
		{
	    if (!wanted[i])
				continue;
				
			int pos = planes[i].pos;
	    int size = planes[i].source.size;
	    uint raster = image_planes[i].raster;
	    if (size > 0)
	    {
	    	if (pos < raster && (pos != 0 || size < raster))
	    	{
	    		int copy = qMin(size, (int)(raster - pos));
		    	uint old_size = planes[i].row.size;
		    	if (raster > old_size)
		    	{
		    		uchar *old_data = planes[i].row.data;
		    		uchar *row = (uchar*)realloc(old_data, (raster+1));
		    		planes[i].row.data = row;
						planes[i].row.size = raster;
		    	}
		    	memcpy(planes[i].row.data + pos,planes[i].source.data, copy);
		    	planes[i].source.data += copy;
		    	planes[i].source.size = size -= copy;
		    	planes[i].pos = pos += copy;
		    	used[i] += copy;
	    	}
	    }
	    
	    if (h == 0)
				continue;
				
			if (pos == raster)
			{
				h = qMin(h, (int)1);
				image_planes[i].data = planes[i].row.data;
			}
			else if (pos == 0 && size >= raster)
			{
				h = qMin(h, (int)(size / raster));
				image_planes[i].data = planes[i].source.data;
			}
			else
				h = 0;
		}
		if (h == 0 || code != 0)
	    break;
	    
	  if (dev == 0) 
	  {
	  	 if (y + h < height)
				code = 0;
	    else
				h = height - y, code = 1;
	  }
	  else
	  {
	  	code = info->planeDataRows(image_planes,  h, &h);
	    error = code < 0;
	  }
	  
	  if (h == 0)
	    break;
		for (int i = 0; i < num_planes; ++i)
		{
			if (!wanted[i])
				continue;
	    int count = image_planes[i].raster * h;
	    if (planes[i].pos)
	    {
	    	planes[i].pos = 0;
	    }
	    else
	    {
	    	planes[i].source.data += count;
				planes[i].source.size -= count;
				used[i] += count;
	    }
		}
		
		cachePlanes();
		if (code > 0)
	    break;
	}
	
	for (int i = 0; i < num_planes; ++i)
		plane_data[i] = planes[i].source;
  return code;
}

const uchar * XWPSImageEnums::imagePlanesWanted()
{
	for (int i = 0; i < num_planes; ++i)
		client_wanted[i] = (wanted[i] && planes[i].pos + planes[i].source.size < image_planes[i].raster);
  return client_wanted;
}

void XWPSImageEnums::init()
{
	info = 0;
  dev = 0;
  plane_index = 0;
  num_planes = 0;
}

void XWPSImageEnums::beginPlanes()
{
	cachePlanes();
  plane_index = -1;
  nextPlane();
}

void XWPSImageEnums::cachePlanes()
{
	if (wanted_varies) 
	{
		wanted_varies =  !info->planesWanted(wanted);
		for (int i = 0; i < num_planes; ++i)
	    if (wanted[i])
				image_planes[i].raster =  imageBytesPerPlaneRow(i);
	    else
				image_planes[i].data = 0;
  }
}

void XWPSImageEnums::freeRowBuffers(int num_planesA)
{
	for (int i = num_planesA - 1; i >= 0; --i) 
	{
		if (planes[i].row.data)
		{
			free(planes[i].row.data);
			planes[i].row.data = 0;
			planes[i].row.size = 0;
		}
  }
}

uint XWPSImageEnums::imageBytesPerPlaneRow(int plane)
{
	return (info->plane_widths[plane] * info->plane_depths[plane] + 7) >> 3;
}

void XWPSImageEnums::nextPlane()
{
	int px = plane_index;

  do 
 	{
		if (++px == num_planes)
	    px = 0;
  } while (!wanted[px]);
  plane_index = px;
}

XWPSImage3Enum::XWPSImage3Enum()
	:XWPSImageEnumCommon()
{
	procs.plane_data_.plane_data3 = &XWPSImage3Enum::image3PlaneData;
	procs.end_image_.end_image3 = &XWPSImage3Enum::image3EndImage;
	procs.flush_.flush3 = &XWPSImage3Enum::image3Flush;
	procs.planes_wanted_.planes_wanted3 = &XWPSImage3Enum::image3PlanesWanted;
	mdev = 0;
	pcdev = 0;
	mask_info = 0;
	pixel_info = 0;
	InterleaveType = interleave_chunky;
	num_components = 0;
	bpc = 0;
	mask_width = mask_height = mask_full_height = 0;
	pixel_width = pixel_height = pixel_full_height = 0;
	mask_data = 0;
	pixel_data = 0;
	mask_y = 0;
	pixel_y = 0;
	mask_skip = 0;
}

XWPSImage3Enum::~XWPSImage3Enum()
{
	if (mdev)
	{
		if (!mdev->decRef())
			delete mdev;
		mdev = 0;
	}
	
	if (pcdev)
	{
		if (!pcdev->decRef())
			delete pcdev;
		pcdev = 0;
	}
	
	if (mask_data)
	{
		delete [] mask_data;
		mask_data = 0;
	}
	
	if (pixel_data)
	{
		delete [] pixel_data;
		pixel_data = 0;
	}
	
	if (mask_info)
	{
		delete mask_info;
		mask_info = 0;
	}
	
	if (pixel_info)
	{
		delete pixel_info;
		pixel_info = 0;
	}
}

int XWPSImage3Enum::image3EndImage(bool draw_last)
{
	mask_info->end(draw_last);
	pixel_info->end(draw_last);
	pcdev->close();
	mdev->close();
	return 0;
}

int XWPSImage3Enum::image3Flush()
{
	int code = mask_info->flush();

  if (code >= 0)
		code = pixel_info->flush();
  return code;
}

int XWPSImage3Enum::image3PlaneData(const PSImagePlane * planes, int height, int *rows_used)
{
  int pixel_used = 0;
  int mask_used = 0;
  int h1 = qMax(pixel_height - pixel_y, mask_height - mask_y);
  int h = qMin(height, h1);
  const PSImagePlane *pixel_planes;
  PSImagePlane pixel_plane, mask_plane;
  int code = 0;
  
  *rows_used = 0;
  switch (InterleaveType)
  {
  	case interleave_chunky:
	    if (h <= 0)
				return 0;
	    if (h > 1)
	    {
	    	int h_orig = h;
				mask_plane = planes[0];
				do 
				{
		    	code = image3PlaneData(&mask_plane, 1,rows_used);
		    	h -= *rows_used;
		    	if (code)
						break;
		    	mask_plane.data += mask_plane.raster;
				} while (h);
				*rows_used = h_orig - h;
				return code;
	    }
			{
				int width = pixel_width;

				uint bit_x = bpc * (num_components + 1) * planes[0].data_x;

				sample_load_declare_setup(sptr, sbit,  planes[0].data + (bit_x >> 3), bit_x & 7, bpc);
				sample_store_declare_setup(mptr, mbit, mbbyte, mask_data, 0, 1);
				sample_store_declare_setup(pptr, pbit, pbbyte, pixel_data, 0, bpc);
				int x;

				mask_plane.data = mptr;
				mask_plane.data_x = 0;
				pixel_plane.data = pptr;
				pixel_plane.data_x = 0;
				
				pixel_planes = &pixel_plane;
				for (x = 0; x < width; ++x) 
				{
		    	uint value;
		    	int i;

		    	sample_load_next12(value, sptr, sbit, bpc);
		    	sample_store_next12(value != 0, mptr, mbit, 1, mbbyte);
		    	for (i = 0; i < num_components; ++i) 
		    	{
						sample_load_next12(value, sptr, sbit, bpc);
						sample_store_next12(value, pptr, pbit, bpc, pbbyte);
		    	}
				}
				sample_store_flush(mptr, mbit, 1, mbbyte);
				sample_store_flush(pptr, pbit, bpc, pbbyte);
	    }
	    break;
	    
	  case interleave_scan_lines:
	    if (planesNext() >= 0) 
	    {
				mask_plane = planes[0];
				pixel_planes = &pixel_plane;
				pixel_plane.data = 0;
	    } 
	    else 
	    {
				mask_plane.data = 0;
				pixel_planes = planes;
	    }
	    break;
	    
		case interleave_separate_source:
			if (h > 1)
				h = 1;
	    mask_plane = planes[0];
	    pixel_planes = planes + 1;
	    break;
	    
		default:
	    return (int)(XWPSError::RangeCheck);
  }
  
  if (mask_plane.data)
  {
  	int skip = mask_skip;

		if (skip >= h) 
	    mask_skip = skip - (mask_used = h);
		else 
		{
	    int mask_h = h - skip;

	    mask_plane.data += skip * mask_plane.raster;
	    mask_skip = 0;
	    code = mask_info->planeDataRows(&mask_plane,  mask_h, &mask_used);
	    mask_used += skip;
		}
		*rows_used = mask_used;
		mask_y += mask_used;
		if (code < 0)
	    return code;
  }
  
  if (pixel_planes[0].data)
  {
  	mask_info->flush();
		code = pixel_info->planeDataRows(pixel_planes, h,	&pixel_used);
		*rows_used = pixel_used;
		pixel_y += pixel_used;
		if (code < 0)
		{
			if (mask_used > pixel_used) 
			{
				int skip = mask_used - pixel_used;

				mask_skip = skip;
				mask_y -= skip;
				mask_used = pixel_used;
	    }
		}
  }
  
  if (mask_y >= mask_height && pixel_y >= pixel_height)
		return 1;
  if (InterleaveType == interleave_scan_lines)
  {
  	if (planesNext() >= 0) 
  	{
	    plane_widths[0] = mask_width;
	    plane_depths[0] = 1;
		} 
		else 
		{
	    plane_widths[0] = pixel_width;
	    plane_depths[0] = pixel_info->plane_depths[0];
		}
  }
  
  return (code < 0 ? code : 0);
}

bool XWPSImage3Enum::image3PlanesWanted(uchar *wanted)
{
	switch (InterleaveType)
	{
		case interleave_chunky:
			wanted[0] = 0xff;
			return true;
			
    case interleave_scan_lines:
			wanted[0] = 0xff;
			return false;
			
    case interleave_separate_source:
    	{
    		int next = planesNext();

				wanted[0] = (next >= 0 ? 0xff : 0);
				memset(wanted + 1, (next <= 0 ? 0xff : 0), num_planes - 1);
				return false;
    	}
    	
    default:
			memset(wanted, 0, num_planes);
			return false;
	}
}

int XWPSImage3Enum::planesNext()
{
	int mask_h = mask_full_height;
  int pixel_h = pixel_full_height;
  long current = pixel_y * (long)mask_h -	mask_y * (long)pixel_h;
  return ((current += mask_h) <= 0 ? -1 : current - pixel_h <= 0 ? 0 : 1);
}

XWPSImage3XEnum::XWPSImage3XEnum()
	:XWPSImageEnumCommon()
{
	procs.plane_data_.plane_data3X = &XWPSImage3XEnum::image3xPlaneData;
	procs.end_image_.end_image3X = &XWPSImage3XEnum::image3xEndImage;
	procs.flush_.flush3X = &XWPSImage3XEnum::image3xFlush;
	procs.planes_wanted_.planes_wanted3X = &XWPSImage3XEnum::image3xPlanesWanted;
	pcdev = 0;
	num_components = 0;
	bpc = 0;
}

XWPSImage3XEnum::~XWPSImage3XEnum()
{
	if (pcdev)
	{
		if (!pcdev->decRef())
		  delete pcdev;
		pcdev = 0;
	}
}

int  XWPSImage3XEnum::image3xEndImage(bool draw_last)
{
	XWPSDevice *mdev0 = mask[0].mdev;
  int ocode =	(mask[0].info ? mask[0].info->end(draw_last) : 0);
  XWPSDevice *mdev1 = mask[1].mdev;
  int scode =	(mask[1].info ? mask[1].info->end(draw_last) : 0);
  int pcode = pixel.info->end(draw_last);

  pcdev->close();
  if (mdev0)
		mdev0->close();
  if (mdev1)
		mdev1->close();
  return (pcode < 0 ? pcode : scode < 0 ? scode : ocode);
}

int  XWPSImage3XEnum::image3xFlush()
{
	int code = mask[0].info->flush();

  if (code >= 0)
		code = mask[1].info->flush();
  if (code >= 0)
		code = pixel.info->flush();
  return code;
}

int  XWPSImage3XEnum::image3xPlaneData(const PSImagePlane * planes, int height, int *rows_used)
{
	int pixel_height = pixel.height;
  int pixel_used = 0;
  int mask_height[2];
  int mask_used[2];
  int h1 = pixel_height - pixel.y;
  int h;
  const PSImagePlane *pixel_planes;
  PSImagePlane pixel_plane, mask_plane[2];
  int code = 0;
  int i, pi = 0;
  int num_chunky = 0;

  for (i = 0; i < NUM_MASKS; ++i)
  {
  	int mh = mask_height[i] = mask[i].height;

		mask_plane[i].data = 0;
		mask_used[i] = 0;
		if (!mask[i].depth)
	    continue;
		h1 = qMin(h1, mh - mask[i].y);
		if (mask[i].InterleaveType == interleave_chunky)
	    ++num_chunky;
  }
  
  h = qMin(height, h1);
  *rows_used = 0;
  if (h <= 0)
		return 0;
		
	for (i = 0; i < NUM_MASKS; ++i)
		if (mask[i].InterleaveType == interleave_separate_source)
		{
			if (h > 1)
				h = 1;
	    mask_plane[i] = planes[pi++];
		}
		
	pixel_planes = &planes[pi];
	if (num_chunky)
	{
		int width = pixel.width;
		uint bit_x = bpc * (num_components + num_chunky) * planes[pi].data_x;
		sample_load_declare_setup(sptr, sbit, planes[0].data + (bit_x >> 3), bit_x & 7, bpc);
		sample_store_declare_setup(pptr, pbit, pbbyte, pixel.data, 0, bpc);
		sample_store_declare(dptr[NUM_MASKS], dbit[NUM_MASKS], dbbyte[NUM_MASKS]);
		int depth[NUM_MASKS];
		int x;

		if (h > 1)
			h = 1;
		
		for (i = 0; i < NUM_MASKS; ++i)
	    if (mask[i].data)
	    {
	    	depth[i] = mask[i].depth;
				mask_plane[i].data = dptr[i] = mask[i].data;
				mask_plane[i].data_x = 0;
				sample_store_setup(dbit[i], 0, depth[i]);
				sample_store_preload(dbbyte[i], dptr[i], 0, depth[i]);
	    }
	    else
	    	depth[i] = 0;
	    	
	  pixel_plane.data = pptr;
		pixel_plane.data_x = 0;
		pixel_planes = &pixel_plane;
		for (x = 0; x < width; ++x)
		{
			uint value;

	    for (i = 0; i < NUM_MASKS; ++i)
				if (depth[i]) 
				{
		    	sample_load_next12(value, sptr, sbit, bpc);
		    	sample_store_next12(value, dptr[i], dbit[i], depth[i], dbbyte[i]);
				}
	    for (i = 0; i < num_components; ++i) 
	    {
				sample_load_next12(value, sptr, sbit, bpc);
				sample_store_next12(value, pptr, pbit, bpc, pbbyte);
	    }
		}
		
		for (i = 0; i < NUM_MASKS; ++i)
	    if (mask[i].data)
				sample_store_flush(dptr[i], dbit[i], depth[i], dbbyte[i]);
		sample_store_flush(pptr, pbit, bpc, pbbyte);
	}
	
	for (i = 0; i < NUM_MASKS; ++i)
		if (mask_plane[i].data)
		{
			int skip = mask[i].skip;

	    if (skip >= h) 
	    {
				mask[i].skip = skip - (mask_used[i] = h);
	    } 
	    else 
	    {
				int mask_h = h - skip;

				mask_plane[i].data += skip * mask_plane[i].raster;
				mask[i].skip = 0;
				code = mask[i].info->planeDataRows(&mask_plane[i],	mask_h, &mask_used[i]);
				mask_used[i] += skip;
	    }
	    *rows_used = mask_used[i];
	    mask[i].y += mask_used[i];
	    if (code < 0)
		   return code;
		}
		
	if (pixel_planes[0].data)
	{
		for (i = 0; i < NUM_MASKS; ++i)
	    if (mask[i].info)
				mask[i].info->flush();
		code = pixel.info->planeDataRows(pixel_planes, h,	&pixel_used);
		*rows_used = pixel_used;
		pixel.y += pixel_used;
		if (code < 0)
		{
			for (i = 0; i < NUM_MASKS; ++i)
				if (mask_used[i] > pixel_used) 
				{
		    	int skip = mask_used[i] - pixel_used;

		    	mask[i].skip = skip;
		    	mask[i].y -= skip;
		    	mask_used[i] = pixel_used;
				}
		}
	}
	
	if (mask[0].y >= mask[0].height &&
			mask[1].y >= mask[1].height &&
			pixel.y >= pixel.height)
		return 1;
		
	return (code < 0 ? code : 0);
}

bool XWPSImage3XEnum::image3xPlanesWanted(uchar *wanted)
{
	bool	sso = mask[0].InterleaveType == interleave_separate_source,
				sss = mask[1].InterleaveType == interleave_separate_source;
				
	if (sso & sss)
	{
		int mask_next = mask[1].channelNext(&pixel);

		memset(wanted + 2, (mask_next <= 0 ? 0xff : 0), num_planes - 2);
		wanted[1] = (mask_next >= 0 ? 0xff : 0);
		if (wanted[1]) 
		{
	    mask_next = mask[0].channelNext(&mask[1]);
	    wanted[0] = mask_next >= 0;
		} 
		else
	    wanted[0] = 0;
		return false;
	}
	else if (sso | sss)
	{
		XWPSImage3XChannelState *pics =  (sso ? &mask[0] : &mask[1]);
		int mask_next = pics->channelNext(&pixel);

		wanted[0] = (mask_next >= 0 ? 0xff : 0);
		memset(wanted + 1, (mask_next <= 0 ? 0xff : 0), num_planes - 1);
		return false;
	}
	else
	{
		wanted[0] = 0xff;
		return true;
	}
}

XWPSBboxImageEnum::XWPSBboxImageEnum()
	:XWPSImageEnumCommon()
{
	procs.plane_data_.plane_databbox = &XWPSBboxImageEnum::bboxImagePlaneData;
	procs.end_image_.end_imagebbox = &XWPSBboxImageEnum::bboxImageEndImage;
	procs.flush_.flushbbox = &XWPSBboxImageEnum::bboxImageFlush;
	procs.planes_wanted_.planes_wantedbbox = &XWPSBboxImageEnum::bboxImagePlanesWanted;
	pcpath = 0;
	target_info = 0;
	params_are_const = false;
	x0 = x1 = 0;
	y = height = 0;
}

XWPSBboxImageEnum::~XWPSBboxImageEnum()
{
}

int XWPSBboxImageEnum::alloc(XWPSImagerState * pis, 
	                  XWPSMatrix * pmat,
		 								XWPSImageCommon * pic, 
		 								XWPSIntRect * prect,
		 								XWPSClipPath * pcpathA,
		 								XWPSBboxImageEnum ** ppbe)
{
	XWPSMatrix mat;
	XWPSMatrix ctm;
	pis->ctmOnly(&ctm);
	if (pmat == 0)
		pmat = &ctm;
		
	int code = mat.matrixInvert(&pic->ImageMatrix);
	if (code < 0)
		return code;
		
	code = mat.matrixMultiply(&mat, pmat);
	if (code < 0)
		return code;
		
	XWPSBboxImageEnum * pbe = new XWPSBboxImageEnum;
	
	pbe->matrix = mat;
  pbe->pcpath = pcpathA;
  if (prect) 
  {
		pbe->x0 = prect->p.x, pbe->x1 = prect->q.x;
		pbe->y = prect->p.y, pbe->height = prect->q.y - prect->p.y;
  } 
  else 
  {
		XWPSIntPoint size;
		code = pic->sourceSize(pis, &size);
		if (code < 0)
		{
			delete pbe;
			return code;
		}
		pbe->x0 = 0, pbe->x1 = size.x;
		pbe->y = 0, pbe->height = size.y;
  }
  
  *ppbe = pbe;
  return 0;
}

int XWPSBboxImageEnum::bboxImageEndImage(bool draw_last)
{
	if (!target_info)
		return 0;
		
	int code = target_info->end(draw_last);
  return code;
}

int XWPSBboxImageEnum::bboxImageFlush()
{
	return (target_info ? target_info->flush() : 0);
}

int XWPSBboxImageEnum::bboxImagePlaneData(const PSImagePlane * planes, 
	                       int heightA,
		                     int *rows_used)
{
	XWPSDeviceBbox * bdev = (XWPSDeviceBbox *)dev;
  XWPSDevice *tdev = bdev->target;
	XWPSRect sbox, dbox;
  XWPSPoint corners[4];
  XWPSFixedRect ibox;
  int code;
  code = target_info->planeDataRows(planes, heightA, rows_used);
  if (code != 1 && !params_are_const)
		copyTargetInfo();
  sbox.p.x = x0;
  sbox.p.y = y;
  sbox.q.x = x1;
  sbox.q.y = y = qMin(y + heightA, height);
  sbox.bboxTransformOnly(&matrix, corners);
  dbox.pointsBbox(corners);
  ibox.p.x = float2fixed(dbox.p.x);
  ibox.p.y = float2fixed(dbox.p.y);
  ibox.q.x = float2fixed(dbox.q.x);
  ibox.q.y = float2fixed(dbox.q.y);
  if (pcpath != NULL &&
			!pcpath->includesRectangle(ibox.p.x, ibox.p.y, ibox.q.x, ibox.q.y))
	{
		XWPSDeviceClip cdev;
		cdev.incRef();
		XWPSDeviceColor devc;
		long x0A = float2fixed(corners[0].x), y0A = float2fixed(corners[0].y);
		long bx2 = float2fixed(corners[2].x) - x0A, by2 = float2fixed(corners[2].y) - y0A;

		cdev.makeClipPathDevice(pcpath);
		cdev.target = dev;
		dev->incRef();
		cdev.open();
		devc.setPure(bdev->black);
		bdev->target = 0;
		cdev.fillTriangleDefault(x0A, y0A, float2fixed(corners[1].x) - x0A, float2fixed(corners[1].y) - y0A, bx2, by2, &devc, lop_default);
		cdev.fillTriangleDefault(x0A, y0A, float2fixed(corners[3].x) - x0A, float2fixed(corners[3].y) - y0A, bx2, by2, &devc, lop_default);
		bdev->target = tdev;
	}
	else 
		bdev->addRect(ibox.p.x, ibox.p.y, ibox.q.x, ibox.q.y);
  return code;
}

bool XWPSBboxImageEnum::bboxImagePlanesWanted(uchar *wanted)
{
	if (!target_info)
		return 0;
		
	return target_info->planesWanted(wanted);
}

void XWPSBboxImageEnum::copyTargetInfo()
{
	if (!target_info)
		return ;
		
	num_planes = target_info->num_planes;
  memcpy(plane_depths, target_info->plane_depths,
	num_planes * sizeof(plane_depths[0]));
  memcpy(plane_widths, target_info->plane_widths, num_planes * sizeof(plane_widths[0]));
}

XWPSPDFImageEnum::XWPSPDFImageEnum()
	:XWPSImageEnumCommon()
{
	procs.plane_data_.plane_datapdf = &XWPSPDFImageEnum::pdfImagePlaneData;
	procs.end_image_.end_imagepdf = &XWPSPDFImageEnum::pdfImageEndImage;
		
	width = 0;
	bits_per_pixel = 0;
	rows_left = 0;
	writer = new XWPSPDFImageWriter;
}

XWPSPDFImageEnum::~XWPSPDFImageEnum()
{
	if (writer)
	{
		delete writer;
		writer = 0;
	}
}

int XWPSPDFImageEnum::pdfImageEndImage(bool draw_last)
{
	return pdfImageEndImageData(draw_last, true);
}

int XWPSPDFImageEnum::pdfImageEndImageData(bool , bool do_image)
{
	XWPSDevicePDF *pdev = (XWPSDevicePDF *)dev;
  int height = writer->height;
  int data_height = height - rows_left;
    int code;

  if (writer->pres)
		((XWPSPDFXObjects *)(writer->pres))->data_height = data_height;
  else
		pdev->putImageMatrix(&mat, (height == 0 || data_height == 0 ? 1.0 : (double)data_height / height));
  code = pdev->endImageBinary(writer, data_height);
  if (code < 0)
		return code;
    
  code = pdev->endWriteImage(writer);
  switch (code) 
  {
    default:
			return code;
			
    case 1:
			code = 0;
			break;
			
    case 0:
			if (do_image)
	    code = pdev->doImage(writer->pres, &mat, true);
  }
  
  return code;
}

int XWPSPDFImageEnum::pdfImageEndImageObject(bool draw_last)
{
	return pdfImageEndImageData(draw_last, false);
}

int XWPSPDFImageEnum::pdfImagePlaneData(const PSImagePlane * planes, int height, int *rows_used)
{
	XWPSDevicePDF *pdev = (XWPSDevicePDF*)dev;
  int h = height;
  int y;
  
  uint width_bits = width * plane_depths[0];
  uint bcount = (width_bits + 7) >> 3;
  uint ignore;
  int nplanes = num_planes;
  XWPSStream *s = pdev->streams->strm;
  long pos = s->tell();
  int code;
  int status = 0;

  if (h > rows_left)
		h = rows_left;
  rows_left -= h;
  for (y = 0; y < h; ++y)
  {
  	if (nplanes > 1)
  	{
  		int pi;
	    uint count = bcount;
	    uint offset = 0;
	    const uchar *bit_planes[PS_IMAGE_MAX_COMPONENTS];
	    int block_bytes = 200 / (3 * nplanes) * 3;
	    uchar row[200];

	    for (pi = 0; pi < nplanes; ++pi)
				bit_planes[pi] = planes[pi].data + planes[pi].raster * y;
	    while (count) 
	    {
				uint flip_count;
				uint flipped_count;

				if (count >= block_bytes) 
				{
		    	flip_count = block_bytes;
		    	flipped_count = block_bytes * nplanes;
				} 
				else 
				{
		    	flip_count = count;
		    	flipped_count =	(width_bits % (block_bytes * 8) * nplanes + 7) >> 3;
				}
				image_flip_planes(row, bit_planes, offset, flip_count, nplanes, plane_depths[0]);
				status = writer->binary.strm->puts(row, flipped_count, &ignore);
				if (status < 0)
		    	break;
				offset += flip_count;
				count -= flip_count;
	    }
  	}
  	else
  		status = writer->binary.strm->puts(planes[0].data + planes[0].raster * y, bcount,  &ignore);
  	
  	if (status < 0)
	    break;
  }
  
  *rows_used = h;
  if (status < 0)
		return (int)(XWPSError::IOError);
  code = writer->data->streamAddSince(pos);
  return (code < 0 ? code : !rows_left);
}

XWPSDVIImageEnum::XWPSDVIImageEnum()
	:XWPSImageEnumCommon()
{
	procs.plane_data_.plane_datadvi = &XWPSDVIImageEnum::dviImagePlaneData;
	procs.end_image_.end_imagedvi = &XWPSDVIImageEnum::dviImageEndImage;
		
	width = 0;
	bits_per_pixel = 0;
	rows_left = 0;
	writer = new XWPSDVIImageWriter;
}

XWPSDVIImageEnum::~XWPSDVIImageEnum()
{
	if (writer)
	{
		delete writer;
		writer = 0;
	}
}

int XWPSDVIImageEnum::dviImageEndImage(bool draw_last)
{
	return dviImageEndImageData(draw_last, true);
}

int XWPSDVIImageEnum::dviImageEndImageData(bool , bool do_image)
{
	XWPSDeviceDVI *pdev = (XWPSDeviceDVI *)dev;
  int height = writer->height;
  int data_height = height - rows_left;
    int code;

  if (writer->pres)
		((XWPSDVIXObjects *)(writer->pres))->data_height = data_height;
  else
		pdev->putImageMatrix(&mat, (height == 0 || data_height == 0 ? 1.0 : (double)data_height / height));
  code = pdev->endImageBinary(writer, data_height);
  if (code < 0)
		return code;
    
  code = pdev->endWriteImage(writer);
  switch (code) 
  {
    default:
			return code;
			
    case 1:
			code = 0;
			break;
			
    case 0:
			if (do_image)
	    code = pdev->doImage(writer->pres, &mat);
  }
  
  return code;
}

int XWPSDVIImageEnum::dviImageEndImageObject(bool draw_last)
{
	return dviImageEndImageData(draw_last, false);
}

int XWPSDVIImageEnum::dviImagePlaneData(const PSImagePlane * planes, int height, int *rows_used)
{
  int h = height;
  int y;
  
  uint width_bits = width * plane_depths[0];
  uint bcount = (width_bits + 7) >> 3;
  uint ignore;
  int nplanes = num_planes;
  int code = 0;
  int status = 0;

  if (h > rows_left)
		h = rows_left;
  rows_left -= h;
  for (y = 0; y < h; ++y)
  {
  	if (nplanes > 1)
  	{
  		int pi;
	    uint count = bcount;
	    uint offset = 0;
	    const uchar *bit_planes[PS_IMAGE_MAX_COMPONENTS];
	    int block_bytes = 200 / (3 * nplanes) * 3;
	    uchar row[200];

	    for (pi = 0; pi < nplanes; ++pi)
				bit_planes[pi] = planes[pi].data + planes[pi].raster * y;
	    while (count) 
	    {
				uint flip_count;
				uint flipped_count;

				if (count >= block_bytes) 
				{
		    	flip_count = block_bytes;
		    	flipped_count = block_bytes * nplanes;
				} 
				else 
				{
		    	flip_count = count;
		    	flipped_count =	(width_bits % (block_bytes * 8) * nplanes + 7) >> 3;
				}
				image_flip_planes(row, bit_planes, offset, flip_count, nplanes, plane_depths[0]);
				status = writer->binary.strm->puts(row, flipped_count, &ignore);
				if (status < 0)
		    	break;
				offset += flip_count;
				count -= flip_count;
	    }
  	}
  	else
  		status = writer->binary.strm->puts(planes[0].data + planes[0].raster * y, bcount,  &ignore);
  	
  	if (status < 0)
	    break;
  }
  
  *rows_used = h;
  if (status < 0)
		return (int)(XWPSError::IOError);
  return (code < 0 ? code : !rows_left);
}
