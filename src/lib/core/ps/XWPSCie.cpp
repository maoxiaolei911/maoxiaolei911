/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include "XWPSError.h"
#include "XWPSParam.h"
#include "XWPSDevice.h"
#include "XWPSColorSpace.h"
#include "XWPSContextState.h"
#include "XWPSCie.h"

float ps_cie_cached_value(float in, const PSCieCacheFloats *pcache)
{
	int index =	(int)((in - pcache->params.base) * pcache->params.factor + 0.0001);

  CIE_CLAMP_INDEX(index);
  return pcache->values[index];
}

void ps_cie_cache_init(PSCieCacheParams * pcache, PSForLoopParams * pflp, const PSRange * domain)
{
	double a = domain->rmin, b = domain->rmax;
  double R = b - a;
  double delta;
#define N (ps_cie_cache_size - 1)

  if (a < 0 && b >= 0) 
  {
		double x = -N * a / R;	/* must be > 0 */
		double Kb = floor(x);	/* must be >= 0 */
		double Ka = ceil(x) - N;	/* must be <= 0 */

		if (Kb == 0 || (Ka != 0 && -b / Ka < -a / Kb))	/* use R'a */
	    R = -N * b / Ka, a = b - R;
		else			/* use R'b */
	    R = -N * a / Kb, b = a + R;
  }
  delta = R / N;
#ifdef CIE_CACHE_INTERPOLATE
  pcache->base = a;		/* no rounding */
#else
  pcache->base = a - delta / 2;	/* so lookup will round */
#endif
  pcache->factor = (delta == 0 ? 0 : N / R);
  pflp->init = a;
  pflp->step = delta;
  pflp->limit = b + delta / 2;
#undef N
}

void ps_cie_cache_to_fracs(const PSCieCacheFloats *pfloats, PSCieCacheFracs *pfracs)
{
	int i;
  for (i = 0; i < ps_cie_cache_size; ++i)
		pfracs->values[i] = float2frac(pfloats->values[i]);
  pfracs->params = pfloats->params;
}

void cie_lookup_mult3(PSCieCachedVector3 * pvec, const PSCieVectorCache * pc)
{
#ifdef CIE_INTERPOLATE_INTERMEDIATE
#ifdef CIE_CACHE_USE_FIXED
#  define lookup_interpolate_between(v0, v1, i, ftemp)\
     cie_interpolate_between(v0, v1, i)
#else
    float ftu, ftv, ftw;

#  define lookup_interpolate_between(v0, v1, i, ftemp)\
     ((v0) + ((v1) - (v0)) *\
      ((ftemp = float_rshift(i, _cie_interpolate_bits)), ftemp - (int)ftemp))
#endif

  cie_cached_value iu =	 LOOKUP_INDEX(pvec->u, pc, _cie_interpolate_bits);
  const PSCieCachedVector3 *pu =	&pc[0].vecs.values[(int)cie_cached_rshift(iu, _cie_interpolate_bits)];
  const PSCieCachedVector3 *pu1 =	(iu >= (ps_cie_cache_size - 1) << _cie_interpolate_bits ? pu : pu + 1);

  cie_cached_value iv =	LOOKUP_INDEX(pvec->v, pc + 1, _cie_interpolate_bits);
  const PSCieCachedVector3 *pv =	&pc[1].vecs.values[(int)cie_cached_rshift(iv, _cie_interpolate_bits)];
  const PSCieCachedVector3 *pv1 =	(iv >= (ps_cie_cache_size - 1) << _cie_interpolate_bits ? pv : pv + 1);

  cie_cached_value iw =	LOOKUP_INDEX(pvec->w, pc + 2, _cie_interpolate_bits);
  const PSCieCachedVector3 *pw =	&pc[2].vecs.values[(int)cie_cached_rshift(iw,  _cie_interpolate_bits)];
  const PSCieCachedVector3 *pw1 =	(iw >= (ps_cie_cache_size - 1) << _cie_interpolate_bits ?	 pw : pw + 1);

  pvec->u = lookup_interpolate_between(pu->u, pu1->u, iu, ftu) + 
            lookup_interpolate_between(pv->u, pv1->u, iv, ftv) +
						lookup_interpolate_between(pw->u, pw1->u, iw, ftw);
  pvec->v = lookup_interpolate_between(pu->v, pu1->v, iu, ftu) +
						lookup_interpolate_between(pv->v, pv1->v, iv, ftv) +
						lookup_interpolate_between(pw->v, pw1->v, iw, ftw);
  pvec->w = lookup_interpolate_between(pu->w, pu1->w, iu, ftu) +
						lookup_interpolate_between(pv->w, pv1->w, iv, ftv) +
						lookup_interpolate_between(pw->w, pw1->w, iw, ftw);
#else
  const PSCieCachedVector3 *pu = &LOOKUP_VALUE(pvec->u, pc);
  const PSCieCachedVector3 *pv = &LOOKUP_VALUE(pvec->v, pc + 1);
  const PSCieCachedVector3 *pw = &LOOKUP_VALUE(pvec->w, pc + 2);

  pvec->u = pu->u + pv->u + pw->u;
  pvec->v = pu->v + pv->v + pw->v;
  pvec->w = pu->w + pv->w + pw->w;
#endif
}

static void 
cie_cache_mult(PSCieVectorCache * pcache, const PSVector3 * pvec,
	       const PSCieCacheFloats * pcf)
{
	int i;

  pcache->vecs.params.base = float2cie_cached(pcf->params.base);
  pcache->vecs.params.factor = float2cie_cached(pcf->params.factor);
  pcache->vecs.params.limit =	float2cie_cached((ps_cie_cache_size - 1) / pcf->params.factor + pcf->params.base);
  for (i = 0; i < ps_cie_cache_size; ++i) 
  {
		float f = pcf->values[i];

		pcache->vecs.values[i].u = float2cie_cached(f * pvec->u);
		pcache->vecs.values[i].v = float2cie_cached(f * pvec->v);
		pcache->vecs.values[i].w = float2cie_cached(f * pvec->w);
  }
}

static bool
cie_cache_mult3(PSCieVectorCache * pc  , const PSMatrix3 * pmat)
{
  cie_cache_mult(pc, &pmat->cu, &pc->floats);
  cie_cache_mult(pc + 1, &pmat->cv, &pc[1].floats);
  cie_cache_mult(pc + 2, &pmat->cw, &pc[2].floats);
  return pmat->is_identity & pc[0].floats.params.is_identity &
		pc[1].floats.params.is_identity & pc[2].floats.params.is_identity;
}

static bool
cache_is_linear(PSCieLinearParams *params, const PSCieCacheFloats *pcf)
{
	double origin = pcf->values[0];
  double diff = pcf->values[_COUNT_OF_(pcf->values) - 1] - origin;
  double scale = diff / (_COUNT_OF_(pcf->values) - 1);
  int i;
  double test = origin + scale;

  for (i = 1; i < _COUNT_OF_(pcf->values) - 1; ++i, test += scale)
		if (fabs(pcf->values[i] - test) >= 0.5 / ps_cie_cache_size)
	    return (params->is_linear = false);
  params->origin = origin - pcf->params.base;
  params->scale = diff * pcf->params.factor / (_COUNT_OF_(pcf->values) - 1);
  return (params->is_linear = true);
}

static bool
cache_set_linear(PSCieCacheFloats *pcf)
{
	if (pcf->params.is_identity) 
	{
	   pcf->params.linear.is_linear = true;
	   pcf->params.linear.origin = 0;
	   pcf->params.linear.scale = 1;
	} 
	else if (cache_is_linear(&pcf->params.linear, pcf)) 
	{
	    if (pcf->params.linear.origin == 0 && fabs(pcf->params.linear.scale - 1) < 0.00001)
				pcf->params.is_identity = true;
	}
	return false;
}

static void
cache3_set_linear(PSCieVectorCache *caches)
{
	cache_set_linear(&caches[0].floats);
  cache_set_linear(&caches[1].floats);
  cache_set_linear(&caches[2].floats);
}

static void
ps_cie_defx_scale(float *values, const PSRange *range, int dim)
{
    double scale = (dim - 1.0) / (range->rmax - range->rmin);
    int i;

    for (i = 0; i < ps_cie_cache_size; ++i) 
    {
			float value = values[i];

			values[i] =  (value <= range->rmin ? 0 :  value >= range->rmax ? dim - 1 : (value - range->rmin) * scale);
    }
}

static void
cie_transform_range(const PSRange3 * in, float mu, float mv, float mw,  PSRange * out)
{
    float umin = mu * in->ranges[0].rmin, umax = mu * in->ranges[0].rmax;
    float vmin = mv * in->ranges[1].rmin, vmax = mv * in->ranges[1].rmax;
    float wmin = mw * in->ranges[2].rmin, wmax = mw * in->ranges[2].rmax;
    float temp;

    if (umin > umax)
			temp = umin, umin = umax, umax = temp;
    if (vmin > vmax)
			temp = vmin, vmin = vmax, vmax = temp;
    if (wmin > wmax)
			temp = wmin, wmin = wmax, wmax = temp;
    out->rmin = umin + vmin + wmin;
    out->rmax = umax + vmax + wmax;
}

static void
cie_transform_range3(const PSRange3 * in, const PSMatrix3 * mat,  PSRange3 * out)
{
    cie_transform_range(in, mat->cu.u, mat->cv.u, mat->cw.u,	&out->ranges[0]);
    cie_transform_range(in, mat->cu.v, mat->cv.v, mat->cw.v,	&out->ranges[1]);
    cie_transform_range(in, mat->cu.w, mat->cv.w, mat->cw.w,	&out->ranges[2]);
}

static void
cie_cache_restrict(PSCieCacheFloats * pcache, const PSRange * prange)
{
    int i;

    for (i = 0; i < ps_cie_cache_size; i++) 
    {
			float v = pcache->values[i];

			if (v < prange->rmin)
	    	pcache->values[i] = prange->rmin;
			else if (v > prange->rmax)
	    	pcache->values[i] = prange->rmax;
    }
}

static void
cie_mult3(const PSVector3 * in, register const PSMatrix3 * mat,  PSVector3 * out)
{
	float u = in->u, v = in->v, w = in->w;

	out->u = (u * mat->cu.u) + (v * mat->cv.u) + (w * mat->cw.u);
	out->v = (u * mat->cu.v) + (v * mat->cv.v) + (w * mat->cw.v);
	out->w = (u * mat->cu.w) + (v * mat->cv.w) + (w * mat->cw.w);
}

static void
cie_matrix_mult3(const PSMatrix3 *ma, const PSMatrix3 *mb, PSMatrix3 *mc)
{
    PSMatrix3 mprod;
    PSMatrix3 *mp = (mc == ma || mc == mb ? &mprod : mc);

    cie_mult3(&mb->cu, ma, &mp->cu);
    cie_mult3(&mb->cv, ma, &mp->cv);
    cie_mult3(&mb->cw, ma, &mp->cw);
    cie_matrix_init(mp);
    if (mp != mc)
			*mc = *mp;
}

static void
cie_invert3(const PSMatrix3 *in, PSMatrix3 *out)
{	
#define A cu.u
#define B cv.u
#define C cw.u
#define D cu.v
#define E cv.v
#define F cw.v
#define G cu.w
#define H cv.w
#define I cw.w
    double coA = in->E * in->I - in->F * in->H;
    double coB = in->F * in->G - in->D * in->I;
    double coC = in->D * in->H - in->E * in->G;
    double det = in->A * coA + in->B * coB + in->C * coC;

    out->A = coA / det;
    out->D = coB / det;
    out->G = coC / det;
    out->B = (in->C * in->H - in->B * in->I) / det;
    out->E = (in->A * in->I - in->C * in->G) / det;
    out->H = (in->B * in->G - in->A * in->H) / det;
    out->C = (in->B * in->F - in->C * in->E) / det;
    out->F = (in->C * in->D - in->A * in->F) / det;
    out->I = (in->A * in->E - in->B * in->D) / det;
#undef A
#undef B
#undef C
#undef D
#undef E
#undef F
#undef G
#undef H
#undef I
    out->is_identity = in->is_identity;
}

static void
store_vector3(float *p, const PSVector3 * pvec)
{
    p[0] = pvec->u, p[1] = pvec->v, p[2] = pvec->w;
}

static int
write_floats(XWPSContextState * ctx, XWPSParamList * plist, const char * key,
	     const float *values, int size)
{
    float *p = 0;
    if (size > 0)
    {
    	p = new float[size];
    	memcpy(p, values, size * sizeof(float));
    }
    PSParamFloatArray fa(p,size);
    fa.persistent = false;
    return plist->writeFloatArray(ctx, key, &fa);
}

static int
write_vector3(XWPSContextState * ctx, XWPSParamList * plist, const char * key,
	      const PSVector3 * pvec)
{
    float values[3];

    store_vector3(values, pvec);
    return write_floats(ctx, plist, key, values, 3);
}

static int
write_matrix3(XWPSContextState * ctx, XWPSParamList * plist, const char * key,
	      const PSMatrix3 * pmat)
{
    float values[9];

    if (!memcmp(pmat, &Matrix3_default, sizeof(*pmat)))
			return 0;
    store_vector3(values, &pmat->cu);
    store_vector3(values + 3, &pmat->cv);
    store_vector3(values + 6, &pmat->cw);
    return write_floats(ctx, plist, key, values, 9);
}

static int
write_range3(XWPSContextState * ctx, XWPSParamList * plist, const char * key,
	     const PSRange3 * prange)
{
    float values[6];

    if (!memcmp(prange, &Range3_default, sizeof(*prange)))
			return 0;
    values[0] = prange->ranges[0].rmin, values[1] = prange->ranges[0].rmax;
    values[2] = prange->ranges[1].rmin, values[3] = prange->ranges[1].rmax;
    values[4] = prange->ranges[2].rmin, values[5] = prange->ranges[2].rmax;
    return write_floats(ctx, plist, key, values, 6);
}

static void
load_vector3(PSVector3 * pvec, const float *p)
{
    pvec->u = p[0], pvec->v = p[1], pvec->w = p[2];
}

static int
read_floats(XWPSContextState * ctx, XWPSParamList * plist, const char * key, float *values, int count)
{
    PSParamFloatArray fa;
    int code = plist->readFloatArray(ctx, key, &fa);

    if (code)
			return code;
    if (fa.size != count)
			return (int)(XWPSError::RangeCheck);
    memcpy(values, fa.data, sizeof(float) * count);

    return 0;
}

static int
read_vector3(XWPSContextState * ctx, XWPSParamList * plist, const char * key,
	     PSVector3 * pvec, const PSVector3 * dflt)
{
    float values[3];
    int code = read_floats(ctx, plist, key, values, 3);

    switch (code) 
    {
			case 1:
	    	if (dflt)
					*pvec = *dflt;
	    	break;
	    	
			case 0:
	    	load_vector3(pvec, values);
			default:	
	   	 break;
    }
    return code;
}

static int
read_matrix3(XWPSContextState * ctx, XWPSParamList * plist, const char * key, PSMatrix3 * pmat)
{
    float values[9];
    int code = read_floats(ctx, plist, key, values, 9);

    switch (code) 
    {
			case 1:	
	    	*pmat = Matrix3_default;
	    	break;
	    	
			case 0:
	    	load_vector3(&pmat->cu, values);
	    	load_vector3(&pmat->cv, values + 3);
	    	load_vector3(&pmat->cw, values + 6);
			default:	
	    	break;
    }
    return code;
}

static int
read_range3(XWPSContextState * ctx, XWPSParamList * plist, const char * key, PSRange3 * prange)
{
    float values[6];
    int code = read_floats(ctx, plist, key, values, 6);

    switch (code) 
    {
			case 1:	
	    	*prange = Range3_default;
	    	break;
	    	
			case 0:
	    	prange->ranges[0].rmin = values[0];
	    	prange->ranges[0].rmax = values[1];
	    	prange->ranges[1].rmin = values[2];
	    	prange->ranges[1].rmax = values[3];
	    	prange->ranges[2].rmin = values[4];
	    	prange->ranges[2].rmax = values[5];
			default:	
	    	break;
    }
    return code;
}

static int
read_proc3(XWPSContextState * ctx, XWPSParamList * plist, const char * key,
	   float values[ps_cie_cache_size * 3])
{
    return read_floats(ctx, plist, key, values, ps_cie_cache_size * 3);
}

static float
encode_from_data(float v, const float values[ps_cie_cache_size],
		 const PSRange * range)
{
    return (v <= range->rmin ? values[0] :
	    v >= range->rmax ? values[ps_cie_cache_size - 1] :
	    values[(int)((v - range->rmin) / (range->rmax - range->rmin) *
			 (ps_cie_cache_size - 1) + 0.5)]);
}

const PSRange3 Range3_default = {
    { {0, 1}, {0, 1}, {0, 1} }
};

const PSRange4 Range4_default = {
    { {0, 1}, {0, 1}, {0, 1}, {0, 1} }
};

const PSCieDEFGProc4 DecodeDEFG_default = 
{
    {&XWPSCieDefg::defgidentity, 
     &XWPSCieDefg::defgidentity, 
     &XWPSCieDefg::defgidentity, 
     &XWPSCieDefg::defgidentity}
};

const PSCieDEFGProc4 DecodeDEFG_from_cache = 
{
    {&XWPSCieDefg::defgfromCache0, 
    	&XWPSCieDefg::defgfromCache1, 
    	&XWPSCieDefg::defgfromCache2, 
    	&XWPSCieDefg::defgfromCache3}
};

const PSCieDEFProc3 DecodeDEF_default = 
{
    {&XWPSCieDef::defidentity, 
    	&XWPSCieDef::defidentity, 
    	&XWPSCieDef::defidentity}
};

const PSCieDEFProc3 DecodeDEF_from_cache = 
{
    {&XWPSCieDef::deffromCache0, 
    	&XWPSCieDef::deffromCache1, 
    	&XWPSCieDef::deffromCache2}
};

const PSCieABCProc3 DecodeABC_default = 
{
    {&XWPSCieABC::abcidentity, 
    	&XWPSCieABC::abcidentity, 
    	&XWPSCieABC::abcidentity}
};

const PSCieABCProc3 DecodeABC_from_cache = 
{
    {&XWPSCieABC::abcfromCache0, 
    	&XWPSCieABC::abcfromCache1, 
    	&XWPSCieABC::abcfromCache2}
};

const PSCieCommonProc3 DecodeLMN_default = 
{
    {&XWPSCie::commonIdentity, 
    	&XWPSCie::commonIdentity, 
    	&XWPSCie::commonIdentity}
};

const PSCieCommonProc3 DecodeLMN_from_cache = 
{
    {&XWPSCie::lmnfromCache0, 
    	&XWPSCie::lmnfromCache1, 
    	&XWPSCie::lmnfromCache2}
};

const PSMatrix3 Matrix3_default = 
{
    {1, 0, 0},
    {0, 1, 0},
    {0, 0, 1},
    1 /*true */
};

const PSRange RangeA_default = {0, 1};
const cie_a_proc DecodeA_default = &XWPSCieA::aidentity;
const cie_a_proc DecodeA_from_cache = &XWPSCieA::afromCache;
const PSVector3 MatrixA_default = {1, 1, 1};
const PSVector3 BlackPoint_default = {0, 0, 0};


XWPSColorLookupTable::XWPSColorLookupTable()
{
	n = 0;
	m = 3;
	memset(dims, 0, 4 * sizeof(int));
	table = 0;
	table_size = 0;
	own_datas = 0;
}

XWPSColorLookupTable::~XWPSColorLookupTable()
{
	if (table)
	{
		for (int i = 0; i < table_size; i++)
		{
			if (own_datas)
			{
				if (own_datas[i])
				{
					if (table[i].data)
					{
					  delete [] table[i].data;
					  table[i].data = 0;
					} 
					own_datas[i] = false;
				}
			}
		}
		delete [] table;
		table = 0;
	}
}

void XWPSColorLookupTable::interpolateAccum(const long * pi, ushort * pv, long factor)
{
	int *pdim = dims;

  if (n > 3) 
  {
		XWPSColorLookupTable clt3;
		int ix = fixed2int_var(pi[0]);
		long fx = fixed_fraction(pi[0]);

		clt3.n = 3;
		clt3.dims[0] = pdim[1];
		clt3.dims[1] = pdim[2];
		clt3.dims[2] = pdim[3];
		clt3.m = m;
		clt3.table = table + ix * pdim[1];
		clt3.interpolateAccum(pi + 1, pv, fixed_1);
		if (ix == pdim[0] - 1)
		{
			clt3.table = 0;
	    return;
	  }
	  
		clt3.table += pdim[1];
		clt3.interpolateAccum(pi + 1, pv, fx);
		clt3.table = 0;
  } 
  else 
  {
		int ic = fixed2int_var(pi[2]);
		long fc = fixed_fraction(pi[2]);
		uint dc1 = (ic == pdim[2] - 1 ? 0 : m);
		int ib = fixed2int_var(pi[1]);
		long fb = fixed_fraction(pi[1]);
		uint db1 = (ib == pdim[1] - 1 ? 0 : pdim[2] * m);
		uint dbc = (ib * pdim[2] + ic) * m;
		uint dbc1 = db1 + dc1;
		int ia = fixed2int_var(pi[0]);
		long fa = fixed_fraction(pi[0]);
		uchar *pa0 = table[ia].data + dbc;
		uchar *pa1 = (ia == pdim[0] - 1 ? pa0 : table[ia + 1].data + dbc);
		int j;

		for (j = 0; j < m; ++j, ++pa0, ++pa1) 
		{
	    short v000 = byte2frac(pa0[0]);
	    short v001 = byte2frac(pa0[dc1]);
	    short v010 = byte2frac(pa0[db1]);
	    short v011 = byte2frac(pa0[dbc1]);
	    short v100 = byte2frac(pa1[0]);
	    short v101 = byte2frac(pa1[dc1]);
	    short v110 = byte2frac(pa1[db1]);
	    short v111 = byte2frac(pa1[dbc1]);
	    short rv;

	    short v00 = v000 + (short) arith_rshift((long)fc * (v001 - v000), _fixed_shift);
	    short v01 = v010 + (short) arith_rshift((long)fc * (v011 - v010), _fixed_shift);
	    short v10 = v100 + (short) arith_rshift((long)fc * (v101 - v100), _fixed_shift);
	    short v11 = v110 + (short) arith_rshift((long)fc * (v111 - v110), _fixed_shift);

	    short v0 = v00 + (short) arith_rshift((long)fb * (v01 - v00), _fixed_shift);
	    short v1 = v10 + (short) arith_rshift((long)fb * (v11 - v10), _fixed_shift);

	    rv = v0 +	(short) arith_rshift((long)fa * (v1 - v0), _fixed_shift);
	    if (factor == fixed_1)
				pv[j] = rv;
	    else
				pv[j] += (short) arith_rshift((long)factor * (rv - pv[j]), _fixed_shift);
		}
  }
}

void XWPSColorLookupTable::interpolateLinear(const long * pi, ushort * pv)
{
	interpolateAccum(pi, pv, fixed_1);
}

void XWPSColorLookupTable::interpolateNearest(const long * pi, ushort * pv)
{
	XWPSString *t = table;
	int *pdim = dims;
	if (n > 3) 
	{
		t += fixed2int_var_rounded(pi[0]) * pdim[1];
		++pi, ++pdim;
  } 
  
  {
		int ic = fixed2int_var_rounded(pi[2]);
		int ib = fixed2int_var_rounded(pi[1]);
		int ia = fixed2int_var_rounded(pi[0]);
		uchar *p = table[ia].data + (ib * pdim[2] + ic) * m;
		int j;

		for (j = 0; j < m; ++j, ++p)
	    pv[j] = byte2frac(*p);
  }
}

XWPSCie::XWPSCie(void *client_dataA)
	:XWPSStruct()
{
	common.RangeLMN = Range3_default;
  common.DecodeLMN = DecodeLMN_default;
  common.MatrixLMN = Matrix3_default;
  common.points.WhitePoint = BlackPoint_default;
  common.points.BlackPoint = BlackPoint_default;
    
	common.install_cspace = 0;
	common.client_data = client_dataA;
}

bool XWPSCie::cache3IsIdentity()
{
	return (common.caches.DecodeLMN[0].floats.params.is_identity &&
	        common.caches.DecodeLMN[1].floats.params.is_identity &&
	        common.caches.DecodeLMN[2].floats.params.is_identity);
}

void XWPSCie::commonComplete()
{
	int i;

  for (i = 0; i < 3; ++i)
		cache_set_linear(&common.caches.DecodeLMN[i].floats);
}

int XWPSCie::getLength()
{
	return sizeof(XWPSCie);
}

const char * XWPSCie::getTypeName()
{
	return "cie";
}

int XWPSCie::installCSpace(XWPSColorSpace * pcs, XWPSState *pgs)
{
	return (pcs->*(common.install_cspace))(pgs);
}

float XWPSCie::lmnfromCache0(float in)
{
	return ps_cie_cached_value(in, &common.caches.DecodeLMN[0].floats);
}

float XWPSCie::lmnfromCache1(float in)
{
	return ps_cie_cached_value(in, &common.caches.DecodeLMN[1].floats);
}

float XWPSCie::lmnfromCache2(float in)
{
	return ps_cie_cached_value(in, &common.caches.DecodeLMN[2].floats);
}

bool XWPSCie::scalarCacheIsExponential(const PSCieScalarCache * pc, float *pexpt)
{
	double k, va, vb;

  if (fabs(pc->floats.values[0]) >= 0.001)
		return false;
    
  k = pc->floats.values[ps_cie_cache_size - 1];
  va = pc->floats.values[ps_cie_cache_size / 3];
  vb = pc->floats.values[ps_cie_cache_size * 2 / 3];
  return valuesAreExponential(va, vb, k, pexpt);
}

bool XWPSCie::scalar3CacheIsExponential(PSVector3 * expts)
{
	return (scalarCacheIsExponential(&common.caches.DecodeLMN[0], &(expts->u)) && 
	        scalarCacheIsExponential(&common.caches.DecodeLMN[1], &(expts->v)) &&
	        scalarCacheIsExponential(&common.caches.DecodeLMN[1], &(expts->w)));
}

bool XWPSCie::vectorCacheIsExponential(const PSCieVectorCache * pc, float *pexpt)
{
	double k, va, vb;

  if (fabs(pc->vecs.values[0].u) >= 0.001)
		return false;
  k = pc->vecs.values[ps_cie_cache_size - 1].u;
  va = pc->vecs.values[ps_cie_cache_size / 3].u;
  vb = pc->vecs.values[ps_cie_cache_size * 2 / 3].u;
  return valuesAreExponential(va, vb, k, pexpt);
}

bool XWPSCie::valuesAreExponential(float va, float vb, float k, float *pexpt)
{
	double p;

  if (fabs(k) < 0.001)
		return false;
  if (va == 0 || (va > 0) != (k > 0))
		return false;
  p = log(va / k) / log((ps_cie_cache_size / 3) / (double)(ps_cie_cache_size - 1));
  if (fabs(vb - k * pow(((ps_cie_cache_size * 2 / 3) / (double)(ps_cie_cache_size - 1)), p)) >= 0.001)
		return false;
  *pexpt = p;
  return true;
}

XWPSCieA::XWPSCieA(void *client_dataA)
	:XWPSCie(client_dataA)
{
	common.install_cspace = &XWPSColorSpace::installCIEA;
	RangeA = RangeA_default;
  DecodeA = DecodeA_default;
  MatrixA = MatrixA_default;
}

void  XWPSCieA::acomplete()
{
	cie_cache_mult(&caches.DecodeA, &MatrixA,   &caches.DecodeA.floats);
  cache_set_linear(&caches.DecodeA.floats);
  commonComplete();
}

float XWPSCieA::afromCache(float in)
{
	return ps_cie_cached_value(in, &caches.DecodeA.floats);
}

int XWPSCieA::getLength()
{
	return sizeof(XWPSCieA);
}

const char * XWPSCieA::getTypeName()
{
	return "ciea";
}

XWPSCieABC::XWPSCieABC(void *client_dataA)
	:XWPSCie(client_dataA)
{
	common.install_cspace = &XWPSColorSpace::installCIEABC;
	RangeABC = Range3_default;
  DecodeABC = DecodeABC_default;
  MatrixABC = Matrix3_default;
}

void  XWPSCieABC::abccomplete()
{
	cache3_set_linear(caches.DecodeABC);
  caches.skipABC = cie_cache_mult3(caches.DecodeABC, &MatrixABC);
  commonComplete();
}

float XWPSCieABC::abcfromCache0(float in)
{
	return ps_cie_cached_value(in, &caches.DecodeABC[0].floats);
}

float XWPSCieABC::abcfromCache1(float in)
{
	return ps_cie_cached_value(in, &caches.DecodeABC[1].floats);
}

float XWPSCieABC::abcfromCache2(float in)
{
	return ps_cie_cached_value(in, &caches.DecodeABC[2].floats);
}

bool XWPSCieABC::cache3IsIdentityABC()
{
	return (caches.DecodeABC[0].floats.params.is_identity &&
	        caches.DecodeABC[1].floats.params.is_identity &&
	        caches.DecodeABC[2].floats.params.is_identity);
}

int XWPSCieABC::getLength()
{
	return sizeof(XWPSCieABC);
}

const char * XWPSCieABC::getTypeName()
{
	return "cieabc";
}

bool XWPSCieABC::vector3CacheIsExponential(PSVector3 * expts)
{
	return (vectorCacheIsExponential(&caches.DecodeABC[0], &(expts->u)) && 
				  vectorCacheIsExponential(&caches.DecodeABC[1], &(expts->v)) &&
				  vectorCacheIsExponential(&caches.DecodeABC[2], &(expts->w)));
}

XWPSCieDef::XWPSCieDef(void *client_dataA)
	:XWPSCieABC(client_dataA)
{
	common.install_cspace = &XWPSColorSpace::installCIEDEF;
	 RangeDEF = Range3_default;
   DecodeDEF = DecodeDEF_default;
   RangeHIJ = Range3_default;
   Table.n = 3;
}

void  XWPSCieDef::defcomplete()
{
	int j;

  for (j = 0; j < 3; ++j)
		ps_cie_defx_scale(caches_def.DecodeDEF[j].floats.values, &RangeHIJ.ranges[j], Table.dims[j]);
  abccomplete();
}

float XWPSCieDef::deffromCache0(float in)
{
	return ps_cie_cached_value(in, &caches_def.DecodeDEF[0].floats);
}

float XWPSCieDef::deffromCache1(float in)
{
	return ps_cie_cached_value(in, &caches_def.DecodeDEF[1].floats);
}

float XWPSCieDef::deffromCache2(float in)
{
	return ps_cie_cached_value(in, &caches_def.DecodeDEF[2].floats);
}

int XWPSCieDef::getLength()
{
	return sizeof(XWPSCieDef);
}

const char * XWPSCieDef::getTypeName()
{
	return "ciedef";
}


XWPSCieDefg::XWPSCieDefg(void *client_dataA)
	:XWPSCieABC(client_dataA)
{
	common.install_cspace = &XWPSColorSpace::installCIEDEFG;
	RangeDEFG = Range4_default;
  DecodeDEFG = DecodeDEFG_default;
  RangeHIJK = Range4_default;
  Table.n = 4;
}

void  XWPSCieDefg::defgcomplete()
{
	int j;

  for (j = 0; j < 4; ++j)
		ps_cie_defx_scale(caches_defg.DecodeDEFG[j].floats.values,  &RangeHIJK.ranges[j], Table.dims[j]);
  abccomplete();
}

float XWPSCieDefg::defgfromCache0(float in)
{
	return ps_cie_cached_value(in, &caches_defg.DecodeDEFG[0].floats);
}

float XWPSCieDefg::defgfromCache1(float in)
{
	return ps_cie_cached_value(in, &caches_defg.DecodeDEFG[1].floats);
}


float XWPSCieDefg::defgfromCache2(float in)
{
	return ps_cie_cached_value(in, &caches_defg.DecodeDEFG[2].floats);
}

float XWPSCieDefg::defgfromCache3(float in)
{
	return ps_cie_cached_value(in, &caches_defg.DecodeDEFG[3].floats);
}

int XWPSCieDefg::getLength()
{
	return sizeof(XWPSCieDefg);
}

const char * XWPSCieDefg::getTypeName()
{
	return "ciedefg";
}

const PSCieTransformProc3 TransformPQR_default = 
{
    &XWPSCieRender::tpqrIdentity,
    0,				/* proc_name */
    0, 0,			/* proc_data */
    0				/* driver_name */
};

const PSCieTransformProc3 TransformPQR_from_cache = 
{
    &XWPSCieRender::tpqrFromCache,
    0,				/* proc_name */
    0, 0,			/* proc_data */
    0				/* driver_name */
};

const ps_cie_transform_proc TransformPQR_lookup_proc_name = &XWPSCieRender::tpqrLookup;
const PSCieRenderProc3 Encode_default = 
{
    {&XWPSCieRender::renderIdentity, 
    	&XWPSCieRender::renderIdentity, 
    	&XWPSCieRender::renderIdentity}
};

const PSCieRenderProc3 EncodeLMN_from_cache = 
{
    {&XWPSCieRender::EncodeLMNCachedL, 
    	&XWPSCieRender::EncodeLMNCachedM, 
    	&XWPSCieRender::EncodeLMNCachedN}
};

const PSCieRenderProc3 EncodeABC_from_cache = 
{
    {&XWPSCieRender::EncodeABCCachedA, 
    	&XWPSCieRender::EncodeABCCachedB, 
    	&XWPSCieRender::EncodeABCCachedC}
};

const PSCieRenderTableProcs RenderTableT_default = 
{
    {&XWPSCieRender::renderTableIdentity, 
    	&XWPSCieRender::renderTableIdentity, 
    	&XWPSCieRender::renderTableIdentity,
     &XWPSCieRender::renderTableIdentity
    }
};

const PSCieRenderTableProcs RenderTableT_from_cache = 
{
    {&XWPSCieRender::RTTCached0, 
    	&XWPSCieRender::RTTCached1, 
    	&XWPSCieRender::RTTCached2, 
    	&XWPSCieRender::RTTCached3}
};

static const PSCieRenderProc3 EncodeLMN_from_data = 
{
    {&XWPSCieRender::encodeLmn0FromData, 
    	&XWPSCieRender::encodeLmn1FromData, 
    	&XWPSCieRender::encodeLmn2FromData}
};

static const PSCieRenderProc3 EncodeABC_from_data = 
{
    {&XWPSCieRender::encodeAbc0FromData, 
    	&XWPSCieRender::encodeAbc1FromData, 
    	&XWPSCieRender::encodeAbc2FromData}
};

static const PSCieRenderTableProcs RenderTableT_from_data = 
{
    {&XWPSCieRender::renderTableT0FromData, 
    	&XWPSCieRender::renderTableT1FromData,
     &XWPSCieRender::renderTableT2FromData, 
     &XWPSCieRender::renderTableT3FromData
    }
};

XWPSCieRender::XWPSCieRender()
	:XWPSStruct()
{
	client_data = 0;
	RenderTable.lookup.table = 0;
	status = CIE_RENDER_STATUS_BUILT;
	own_proc_name = false;
	own_data = false;
}

XWPSCieRender::~XWPSCieRender()
{
	if (own_proc_name)
	{
		if (TransformPQR.proc_name)
		{
			uchar * p = (uchar*)(TransformPQR.proc_name);
			TransformPQR.proc_name = 0;
			delete [] p;
		}
	}
	
	if (own_data)
	{
		if (TransformPQR.data)
		{
			uchar * p = (uchar*)(TransformPQR.data);
			TransformPQR.data = 0;
			delete [] p;
		}
	}
}

#define RESTRICTED_INDEX(v, n, itemp)\
  ((uint)(itemp = (int)(v)) >= (n) ?\
   (itemp < 0 ? 0 : (n) - 1) : itemp)
   	
int XWPSCieRender::complete()
{
	int code;

  if (status >= CIE_RENDER_STATUS_COMPLETED)
		return 0;
    
  code = sample();
  if (code < 0)
		return code;
	
  MatrixABCEncode = MatrixABC;
  {
		int c;
		double f;

		for (c = 0; c < 3; c++) 
		{
	    PSCieFloatFixedCache *pcache = &caches.EncodeABC[c];

	    cie_cache_restrict(&caches.EncodeLMN[c].floats,  &RangeLMN.ranges[c]);
	    cie_cache_restrict(&caches.EncodeABC[c].floats,  &RangeABC.ranges[c]);
	    if (RenderTable.lookup.table == 0) 
	    {
				cie_cache_restrict(&pcache->floats,  &Range3_default.ranges[0]);
				ps_cie_cache_to_fracs(&pcache->floats, &pcache->fixeds.fracs);
				pcache->fixeds.fracs.params.is_identity = false;
	    } 
	    else 
	    {
				int i;
				int n = RenderTable.lookup.dims[c];

#ifdef CIE_RENDER_TABLE_INTERPOLATE
#  define SCALED_INDEX(f, n, itemp)\
     RESTRICTED_INDEX(f * (1 << _cie_interpolate_bits),\
		      (n) << _cie_interpolate_bits, itemp)
#else
		int m = RenderTable.lookup.m;
		int k = (c == 0 ? 1 : c == 1 ? m * RenderTable.lookup.dims[2] : m);
#  define SCALED_INDEX(f, n, itemp)\
     (RESTRICTED_INDEX(f, n, itemp) * k)
#endif
		const PSRange *prange = RangeABC.ranges + c;
		double scale = (n - 1) / (prange->rmax - prange->rmin);

		for (i = 0; i < ps_cie_cache_size; ++i) 
		{
		    float v =	(pcache->floats.values[i] - prange->rmin) * scale
#ifndef CIE_RENDER_TABLE_INTERPOLATE
			+ 0.5
#endif
			;
		    int itemp;
		    pcache->fixeds.ints.values[i] =	SCALED_INDEX(v, n, itemp);
		}
		pcache->fixeds.ints.params = pcache->floats.params;
		pcache->fixeds.ints.params.is_identity = false;
#undef SCALED_INDEX
	    }
	}
	
#define MABC(i, t)\
  f = caches.EncodeABC[i].floats.params.factor;\
  MatrixABCEncode.cu.t *= f;\
  MatrixABCEncode.cv.t *= f;\
  MatrixABCEncode.cw.t *= f;\
  EncodeABC_base[i] =\
    float2cie_cached(caches.EncodeABC[i].floats.params.base * f)
	MABC(0, u);
	MABC(1, v);
	MABC(2, w);
#undef MABC
		MatrixABCEncode.is_identity = 0;
  }
  cie_cache_mult3(caches.EncodeLMN, &MatrixABCEncode);
  status = CIE_RENDER_STATUS_COMPLETED;
  return 0;
}


float XWPSCieRender::EncodeABCCachedA(float in)
{
	return ps_cie_cached_value(in, &caches.EncodeABC[0].floats);
}

float XWPSCieRender::EncodeABCCachedB(float in)
{
	return ps_cie_cached_value(in, &caches.EncodeABC[1].floats);
}

float XWPSCieRender::EncodeABCCachedC(float in)
{
	return ps_cie_cached_value(in, &caches.EncodeABC[2].floats);
}

float XWPSCieRender::encodeAbc0FromData(float v)
{
	const PSEncodeData *data = (PSEncodeData*)client_data;

    return encode_from_data(v, &data->abc[0],  &DomainABC.ranges[0]);
}

float XWPSCieRender::encodeAbc1FromData(float v)
{
	const PSEncodeData *data = (PSEncodeData*)client_data;

    return encode_from_data(v, &data->abc[ps_cie_cache_size],  &DomainABC.ranges[1]);
}

float XWPSCieRender::encodeAbc2FromData(float v)
{
	const PSEncodeData *data = (PSEncodeData*)client_data;

    return encode_from_data(v, &data->abc[ps_cie_cache_size * 2],  &DomainABC.ranges[2]);
}

float XWPSCieRender::EncodeLMNCachedL(float in)
{
	return ps_cie_cached_value(in, &caches.EncodeLMN[0].floats);
}

float XWPSCieRender::EncodeLMNCachedM(float in)
{
	return ps_cie_cached_value(in, &caches.EncodeLMN[1].floats);
}

float XWPSCieRender::EncodeLMNCachedN(float in)
{
	return ps_cie_cached_value(in, &caches.EncodeLMN[2].floats);
}

float XWPSCieRender::encodeLmn0FromData(float v)
{
	const PSEncodeData *data = (PSEncodeData*)client_data;

  return encode_from_data(v, &data->lmn[0],  &DomainLMN.ranges[0]);
}

float XWPSCieRender::encodeLmn1FromData(float v)
{
	const PSEncodeData *data = (PSEncodeData*)client_data;

  return encode_from_data(v, &data->lmn[ps_cie_cache_size],  &DomainLMN.ranges[1]);
}

float XWPSCieRender::encodeLmn2FromData(float v)
{
	const PSEncodeData *data = (PSEncodeData*)client_data;

  return encode_from_data(v, &data->lmn[ps_cie_cache_size * 2],  &DomainLMN.ranges[2]);
}

int XWPSCieRender::getLength()
{
	return sizeof(XWPSCieRender);
}

const char * XWPSCieRender::getTypeName()
{
	return "cierender";
}

int XWPSCieRender::init()
{
	PSMatrix3 PQR_inverse;

  if (status >= CIE_RENDER_STATUS_INITED)
		return 0;	
    
  cie_matrix_init(&MatrixLMN);  
  cie_matrix_init(&MatrixABC);
  cie_matrix_init(&MatrixPQR);
  cie_invert3(&MatrixPQR, &PQR_inverse);
  cie_matrix_mult3(&MatrixLMN, &PQR_inverse, &MatrixPQR_inverse_LMN);
  cie_transform_range3(&RangePQR, &MatrixPQR_inverse_LMN, &DomainLMN);
  cie_transform_range3(&RangeLMN, &MatrixABC, &DomainABC);
  cie_mult3(&points.WhitePoint, &MatrixPQR, &wdpqr);
  cie_mult3(&points.BlackPoint, &MatrixPQR, &bdpqr);
  status = CIE_RENDER_STATUS_INITED;
  return 0;
}

int XWPSCieRender::paramGetCieRender1(XWPSContextState * ctx, 
	                       XWPSParamList * plist,
		                     XWPSDevice * dev)
{
	PSEncodeData data;
  PSParamIntArray rt_size;
  int crd_type;
  int code, code_lmn, code_abc, code_rt, code_t;
  PSParamString pname, pdata;

  status = CIE_RENDER_STATUS_BUILT;
  if ((code = plist->readInt(ctx, "ColorRenderingType", &crd_type)) < 0 ||
			crd_type != PS_DEVICE_CRD1_TYPE ||
			(code = read_vector3(ctx, plist, "WhitePoint", &points.WhitePoint, NULL)) < 0 ||
			(code = read_vector3(ctx, plist, "BlackPoint", &points.BlackPoint,  &BlackPoint_default)) < 0 ||
			(code = read_matrix3(ctx, plist, "MatrixPQR", &MatrixPQR)) < 0 ||
			(code = read_range3(ctx, plist, "RangePQR", &RangePQR)) < 0 ||
			(code = read_matrix3(ctx, plist, "MatrixLMN", &MatrixLMN)) < 0 ||
			(code_lmn = code = read_proc3(ctx, plist, "EncodeLMNValues", data.lmn)) < 0 ||
			(code = read_range3(ctx, plist, "RangeLMN", &RangeLMN)) < 0 ||
			(code = read_matrix3(ctx, plist, "MatrixABC", &MatrixABC)) < 0 ||
			(code_abc = code = read_proc3(ctx, plist, "EncodeABCValues", data.abc)) < 0 ||
			(code = read_range3(ctx, plist, "RangeABC", &RangeABC)) < 0)
		return code;
    
    
  switch (code = plist->readString(ctx, "TransformPQRName", &pname)) 
  {
		default:	
	    return code;
	    
		case 1:	
	    TransformPQR = TransformPQR_default;
	    break;
	    
		case 0:	
	    if (pname.size < 1 || pname.data[pname.size - 1] != 0)
	    {
				return (int)(XWPSError::RangeCheck);
			}
	    TransformPQR.proc = TransformPQR_lookup_proc_name;
	    TransformPQR.proc_name = (char *)pname.data;
	    if (!pname.persistent)
	    {
	    	pname.persistent = true;
	    	pname.data = 0;
	    	own_proc_name = true;
	    }
	    switch (code = plist->readString(ctx, "TransformPQRData", &pdata)) 
	    {
				default:	
		    	return code;
		    	
				case 1:	
		    	TransformPQR.data = 0;
		    	TransformPQR.size = 0;
		    	break;
		    	
				case 0:
		    	TransformPQR.data = pdata.data;
		    	TransformPQR.size = pdata.size;
		    	if (!pdata.persistent)
		    	{
		    		pdata.persistent = true;
		    		pdata.data = 0;
		    		own_data = true;
		    	}
	    }
	    TransformPQR.driver_name = dev->dname;
	    break;
  }
  client_data = &data;
  if (code_lmn > 0)
		EncodeLMN = Encode_default;
  else
		EncodeLMN = EncodeLMN_from_data;
  if (code_abc > 0)
		EncodeABC = Encode_default;
  else
		EncodeABC = EncodeABC_from_data;
  code_rt = code = plist->readIntArray(ctx, "RenderTableSize", &rt_size);
  if (code == 1) 
  {
		if (RenderTable.lookup.table) 
		{
			for (int i = 0; i < RenderTable.lookup.table_size; i++)
			{
				if (RenderTable.lookup.own_datas)
				{
					if (RenderTable.lookup.own_datas[i])
						if (RenderTable.lookup.table[i].data)
							delete [] RenderTable.lookup.table[i].data;
				}
			}
			delete [] RenderTable.lookup.table;
			RenderTable.lookup.table = 0;
			if (RenderTable.lookup.own_datas)
			{
				delete [] RenderTable.lookup.own_datas;
				RenderTable.lookup.own_datas = 0;
			}
		}
		RenderTable.T = RenderTableT_default;
		code_t = 1;
  } 
  else if (code < 0)
		return code;
  else if (rt_size.size != 4)
  {
		return (int)(XWPSError::RangeCheck);
	}
  else 
  {
		PSParamStringArray rt_values;
		XWPSString *table;
		int n, m, j;

		for (j = 0; j < rt_size.size; ++j)
	    if (rt_size.data[j] < 1)
	    {
				return (int)(XWPSError::RangeCheck);
			}
	
		code = plist->readStringArray(ctx, "RenderTableTable", &rt_values);
		if (code < 0)
	    return code;
		if (code > 0 || rt_values.size != rt_size.data[0])
		{
			return (int)(XWPSError::RangeCheck);
	  }
	  
		for (j = 0; j < rt_values.size; ++j)
	    if (rt_values.data[j].size != rt_size.data[1] * rt_size.data[2] * rt_size.data[3])
	    {
				return (int)(XWPSError::RangeCheck);
			}
		RenderTable.lookup.n = n = rt_size.size - 1;
		RenderTable.lookup.m = m = rt_size.data[n];
		if (n > 4 || m > 4)
		{
			return (int)(XWPSError::RangeCheck);
	  }
		memcpy(RenderTable.lookup.dims, rt_size.data, n * sizeof(int));
		table = new XWPSString[RenderTable.lookup.dims[0]];
		RenderTable.lookup.table_size = RenderTable.lookup.dims[0];
		RenderTable.lookup.own_datas = new bool[RenderTable.lookup.table_size];
		for (j = 0; j < RenderTable.lookup.dims[0]; ++j) 
		{
	    table[j].data = rt_values.data[j].data;
	    table[j].size = rt_values.data[j].size;
	    RenderTable.lookup.own_datas[j] = false;
	    if (!(rt_values.data[j].persistent))
	    {
	      rt_values.data[j].persistent = true;
	      rt_values.data[j].data = 0;
	      RenderTable.lookup.own_datas[j] = true;
	    }
		}
		RenderTable.lookup.table = table;
		RenderTable.T = RenderTableT_from_data;
		code_t = code = read_floats(ctx, plist, "RenderTableTValues", data.t,  ps_cie_cache_size * m);
		if (code > 0)
	    RenderTable.T = RenderTableT_default;
		else if (code == 0)
	    RenderTable.T = RenderTableT_from_data;
  }
  if ((code = init()) >= 0 && (code = sample()) >= 0)
		code = complete();
		
  client_data = 0;
  if (code_lmn == 0)
		EncodeLMN = EncodeLMN_from_cache;
  if (code_abc == 0)
		EncodeABC = EncodeABC_from_cache;
  if (code_t == 0)
		RenderTable.T = RenderTableT_from_cache;
  return code;
}

int XWPSCieRender::paramInitialize(XWPSContextState * ctx, 
	                     XWPSParamList * plist,
				               const char * key, 
				               XWPSDevice * dev)
{
	PSParamCollection dict;
  int code = plist->beginTransmit(ctx, key, &dict, ps_param_collection_dict_any);
  int dcode;

  if (code < 0)
		return code;
    
  code = paramGetCieRender1(ctx, dict.list, dev);
  dcode = plist->endTransmit(ctx, key, &dict);
  if (code < 0)
		return code;
    
  if (dcode < 0)
		return dcode;
  
  init();
  sample();
  return complete();
}

int XWPSCieRender::paramPutCieRender1(XWPSContextState * ctx, XWPSParamList * plist)
{
	int crd_type = PS_DEVICE_CRD1_TYPE;
  int code = sample();

  if (code < 0)
		return code;
    
  if (TransformPQR.proc_name) 
  {
		PSParamString pn, pd;

		pn.fromString(TransformPQR.proc_name);
		pn.size++;
		pd.data = (uchar*)(TransformPQR.data);
		pd.size = TransformPQR.size;
		if ((code = plist->writeName(ctx, "TransformPQRName", &pn)) < 0 ||
	    (code = plist->writeString(ctx, "TransformPQRData", &pd)) < 0)
	    return code;
  }
  else if (TransformPQR.proc != TransformPQR_default.proc) 
  {
		return (int)(XWPSError::RangeCheck);
  }
  
  if ((code = plist->writeInt(ctx, "ColorRenderingType", &crd_type)) < 0 ||
			(code = write_vector3(ctx, plist, "WhitePoint", &points.WhitePoint)) < 0)
		return code;
    
  if (memcmp(&points.BlackPoint, &BlackPoint_default, sizeof(points.BlackPoint))) 
  {
		if ((code = write_vector3(ctx, plist, "BlackPoint", &points.BlackPoint)) < 0)
	    return code;
  }
    
  if ((code = write_matrix3(ctx, plist, "MatrixPQR", &MatrixPQR)) < 0 ||
			(code = write_range3(ctx, plist, "RangePQR", &RangePQR)) < 0 ||
    	(code = write_matrix3(ctx, plist, "MatrixLMN", &MatrixLMN)) < 0 ||
			(code = writeProc3(ctx, plist, "EncodeLMNValues", &EncodeLMN, &DomainLMN)) < 0 ||
			(code = write_range3(ctx, plist, "RangeLMN", &RangeLMN)) < 0 ||
    	(code = write_matrix3(ctx, plist, "MatrixABC", &MatrixABC)) < 0 ||
			(code = writeProc3(ctx, plist, "EncodeABCValues", &EncodeABC, &DomainABC)) < 0 ||
			(code = write_range3(ctx, plist, "RangeABC", &RangeABC)) < 0)
		return code;
  
  if (RenderTable.lookup.table) 
  {
		int n = RenderTable.lookup.n;
		int m = RenderTable.lookup.m;
		int na = RenderTable.lookup.dims[0];
		int *size = new int[n+1];	    
		PSParamString *table = new PSParamString[na];

	  memcpy(size, RenderTable.lookup.dims, sizeof(int) * n);

	  size[n] = m;
	  PSParamIntArray ia(size, n + 1);	
	  ia.persistent = false;	  
	  code = plist->writeIntArray(ctx, "RenderTableSize", &ia);	  
		if (code >= 0) 
		{
	    int a;

	    for (a = 0; a < na; ++a)
				table[a].data = RenderTable.lookup.table[a].data,
		    table[a].size = RenderTable.lookup.table[a].size;
		    
		  PSParamStringArray sa(table, na);
		  sa.persistent = false;
	    code = plist->writeStringArray(ctx, "RenderTableTable", &sa);
	    if (code >= 0 && !caches.RenderTableT_is_identity) 
	    {
				uint size = ps_cie_cache_size;
				float *values = new float[size * m];
				for (int i = 0; i < m; ++i) 
				{
		    	double scale = 255.0 / (size - 1);
		    	int j;

		    	for (j = 0; j < size; ++j)
						values[i * size + j] =  frac2float((this->*(RenderTable.T.procs[i]))(j * scale));
				}
				
				PSParamFloatArray fa(values, size * m);
				fa.persistent = false;
				code = plist->writeFloatArray(ctx, "RenderTableTValues",  &fa);
	    }
		}
		else 
		{
			delete [] table;
	    return code;
		}
  }
  return code;
}

int XWPSCieRender::paramWriteCieRender1(XWPSContextState * ctx,
	                         XWPSParamList * plist, 
	                         const char * key)
{
	PSParamCollection dict;
  int code, dcode;
  dict.size = 20;
  if ((code = plist->beginTransmit(ctx, key, &dict, ps_param_collection_dict_any)) < 0)
		return code;
		
	code = paramPutCieRender1(ctx, dict.list);
  dcode = plist->endTransmit(ctx, key, &dict);
  return (code < 0 ? code : dcode);
}

int XWPSCieRender::render1InitFrom(void *client_dataA,
			 								const XWPSCieRender * pfrom_crd,
			 								const PSVector3 * WhitePointA,
			 								const PSVector3 * BlackPointA,
			 								const PSMatrix3 * MatrixPQRA,
			 								const PSRange3 * RangePQRA,
			 								const PSCieTransformProc3 * TransformPQRA,
			 								const PSMatrix3 * MatrixLMNA,
			 								const PSCieRenderProc3 * EncodeLMNA,
			 								const PSRange3 * RangeLMNA,
			 								const PSMatrix3 * MatrixABCA,
			 								const PSCieRenderProc3 * EncodeABCA,
			 								const PSRange3 * RangeABCA,
			 								const XWPSCieRenderTable * RenderTableA)
{
  client_data = client_dataA;
  points.WhitePoint = *WhitePointA;
  points.BlackPoint =	*(BlackPointA ? BlackPointA : &BlackPoint_default);
  MatrixPQR = *(MatrixPQRA ? MatrixPQRA : &Matrix3_default);
  RangePQR = *(RangePQRA ? RangePQRA : &Range3_default);
  TransformPQR =	*(TransformPQRA ? TransformPQRA : &TransformPQR_default);
  MatrixLMN = *(MatrixLMNA ? MatrixLMNA : &Matrix3_default);
  EncodeLMN = *(EncodeLMNA ? EncodeLMNA : &Encode_default);
  if (pfrom_crd &&	!memcmp(&EncodeLMN, &EncodeLMN_from_cache,	sizeof(EncodeLMN_from_cache)))
		memcpy(caches.EncodeLMN, pfrom_crd->caches.EncodeLMN,   sizeof(caches.EncodeLMN));
  RangeLMN = *(RangeLMNA ? RangeLMNA : &Range3_default);
  MatrixABC = *(MatrixABCA ? MatrixABCA : &Matrix3_default);
  EncodeABC = *(EncodeABCA ? EncodeABCA : &Encode_default);
  if (pfrom_crd &&	!memcmp(&EncodeABC, &EncodeABC_from_cache,	sizeof(EncodeABC_from_cache)))
		memcpy(caches.EncodeABC, pfrom_crd->caches.EncodeABC, sizeof(caches.EncodeABC));
  RangeABC = *(RangeABCA ? RangeABCA : &Range3_default);
  if (RenderTableA) 
  {
		RenderTable = *RenderTableA;
		if (pfrom_crd && !memcmp(&RenderTable.T, &RenderTableT_from_cache, sizeof(RenderTableT_from_cache))) 
		{
	    memcpy(caches.RenderTableT, pfrom_crd->caches.RenderTableT, sizeof(caches.RenderTableT));
	    caches.RenderTableT_is_identity =	pfrom_crd->caches.RenderTableT_is_identity;
		}
  } 
  else 
 	{
		RenderTable.lookup.table = 0;
		RenderTable.T = RenderTableT_default;
  }
  status = CIE_RENDER_STATUS_BUILT;
  return 0;
}

int XWPSCieRender::render1Initialize(void *client_dataA,
			                  const PSVector3 * WhitePointA,
			                  const PSVector3 * BlackPointA,
			                  const PSMatrix3 * MatrixPQRA,
			                  const PSRange3 * RangePQRA,
			                  const PSCieTransformProc3 * TransformPQRA,
			                  const PSMatrix3 * MatrixLMNA,
			                  const PSCieRenderProc3 * EncodeLMNA,
			                  const PSRange3 * RangeLMNA,
			                  const PSMatrix3 * MatrixABCA,
			                  const PSCieRenderProc3 * EncodeABCA,
			                  const PSRange3 * RangeABCA,
			                  const XWPSCieRenderTable * RenderTableA)
{
	return render1InitFrom(client_dataA, NULL,
				    WhitePointA, BlackPointA,
				    MatrixPQRA, RangePQRA, TransformPQRA,
				    MatrixLMNA, EncodeLMNA, RangeLMNA,
				    MatrixABCA, EncodeABCA, RangeABCA,
				    RenderTableA);
}

ushort XWPSCieRender::renderTableT0FromData(uchar v)
{
	const PSEncodeData *data = (PSEncodeData*)client_data;

  return float2frac(encode_from_data(v / 255.0,  &data->t[0], &Range3_default.ranges[0]));
}

ushort XWPSCieRender::renderTableT1FromData(uchar v)
{
	const PSEncodeData *data = (PSEncodeData*)client_data;

  return float2frac(encode_from_data(v / 255.0,  &data->t[ps_cie_cache_size], &Range3_default.ranges[0]));
}

ushort XWPSCieRender::renderTableT2FromData(uchar v)
{
	const PSEncodeData *data = (PSEncodeData*)client_data;

  return float2frac(encode_from_data(v / 255.0,  &data->t[ps_cie_cache_size*2], &Range3_default.ranges[0]));
}

ushort XWPSCieRender::renderTableT3FromData(uchar v)
{
	const PSEncodeData *data = (PSEncodeData*)client_data;

  return float2frac(encode_from_data(v / 255.0,  &data->t[ps_cie_cache_size*3], &Range3_default.ranges[0]));
}

ushort XWPSCieRender::RTTCached(uchar in, int i)
{
	return caches.RenderTableT[i].fracs.values[in * (ps_cie_cache_size - 1) / 255];
}

ushort XWPSCieRender::RTTCached0(uchar in)
{
	return RTTCached(in, 0);
}

ushort XWPSCieRender::RTTCached1(uchar in)
{
	return RTTCached(in, 1);
}

ushort XWPSCieRender::RTTCached2(uchar in)
{
	return RTTCached(in, 2);
}

ushort XWPSCieRender::RTTCached3(uchar in)
{
	return RTTCached(in, 3);
}

int XWPSCieRender::sample()
{
	int code;

  if (status >= CIE_RENDER_STATUS_SAMPLED)
		return 0;
		
  code = init();
  if (code < 0)
		return code;

  int j;
	for (j = 0; j < (sizeof(caches.EncodeLMN) / sizeof(caches.EncodeLMN[0])); j++) 
	{
	    PSCieCacheFloats *pcf = &caches.EncodeLMN[j].floats;
	    int i;
	    PSForLoopParams lp;

	    ps_cie_cache_init(&pcf->params, &lp, &DomainLMN.ranges[j]);
	    for (i = 0; i < ps_cie_cache_size; lp.init += lp.step, i++) 
	    {
				pcf->values[i] = (this->*(EncodeLMN.procs[j]))(lp.init);
	    }
	    pcf->params.is_identity =	(EncodeLMN.procs[j] == Encode_default.procs[j]);
	}
	
  cache3_set_linear(caches.EncodeLMN);
    
  for (j = 0; j < (sizeof(caches.EncodeABC) / sizeof(caches.EncodeABC[0])); j++) 
	{
	    PSCieCacheFloats *pcf = &caches.EncodeABC[j].floats;
	    int i;
	    PSForLoopParams lp;

	    ps_cie_cache_init(&pcf->params, &lp, &DomainABC.ranges[j]);
	    for (i = 0; i < ps_cie_cache_size; lp.init += lp.step, i++) 
	    {
				pcf->values[i] = (this->*(EncodeABC.procs[j]))(lp.init);
	    }
	    pcf->params.is_identity =	(EncodeABC.procs[j] == Encode_default.procs[j]);
	}
	
  if (RenderTable.lookup.table != 0) 
  {
		int i, j, m = RenderTable.lookup.m;
		PSForLoopParams flp;
		bool is_identity = true;

		for (j = 0; j < m; j++) 
		{
	    ps_cie_cache_init(&caches.RenderTableT[j].fracs.params, &flp, &Range3_default.ranges[0]);
	    is_identity &= RenderTable.T.procs[j] ==	RenderTableT_default.procs[j];
		}
		caches.RenderTableT_is_identity = is_identity;
		
		for (i = 0; i < ps_cie_cache_size; i++) 
		{
#if ps_cie_log2_cache_size >= 8
	    uchar value = i >> (ps_cie_log2_cache_size - 8);
#else
	    uchar value = (i << (8 - ps_cie_log2_cache_size)) + (i >> (ps_cie_log2_cache_size * 2 - 8));
#endif
	    
	    for (j = 0; j < m; j++) 
	    {
				caches.RenderTableT[j].fracs.values[i] = (this->*(RenderTable.T.procs[j]))(value);
	    }
		}
  }
  status = CIE_RENDER_STATUS_SAMPLED;
  return 0;
}

int XWPSCieRender::tpqrDoLookup(XWPSDevice *dev_proto)
{
	XWPSDevice *dev;
  XWPSCParamList list;
  PSParamString proc_addr;
  int code;

  code = dev_proto->copyDevice(&dev);
  if (code < 0)
		return code;
    
  list.write();
  code = list.request(dev->context_state, TransformPQR.proc_name);
  if (code >= 0) 
  {
		code = dev->getDeviceOrHWParams(&list, false);
		if (code >= 0) 
		{
	    list.read();
	    code = list.readString(dev->context_state, TransformPQR.proc_name, &proc_addr);
	    if (code == 0 && proc_addr.size == sizeof(ps_cie_transform_proc)) 
	    {
				memcpy(&TransformPQR.proc, proc_addr.data,  sizeof(ps_cie_transform_proc));
	    } 
	    else
				code = (int)(XWPSError::RangeCheck);
		}
  }
  if (!dev->decRef())
  	delete dev;
  
  return code;
}

int XWPSCieRender::tpqrLookup(int index, 
	                float in, 
	                const PSCieWBSD * pwbsd,
	               float *out)
{
	int code = tpqrDoLookup(device);
	if (code < 0)
		return code;
  return (this->*(TransformPQR.proc))(index, in, pwbsd, out);
}

int XWPSCieRender::writeProc3(XWPSContextState * ctx, 
	               XWPSParamList * plist, const char * key,
	               const PSCieRenderProc3 * procs,
	               const PSRange3 * domain)
{
	float *values;
  uint size = ps_cie_cache_size;
  int i;

  if (!memcmp(procs, &Encode_default, sizeof(*procs)))
		return 0;
    
  values = new float[size * 3];
  for (i = 0; i < 3; ++i) 
  {
		double base = domain->ranges[i].rmin;
		double scale = (domain->ranges[i].rmax - base) / (size - 1);
		int j;

		for (j = 0; j < size; ++j)
	    values[i * size + j] = (this->*(procs->procs[i])) (j * scale + base);
  }
  
  PSParamFloatArray fa(values, size * 3);
  fa.persistent = false;
  int code = plist->writeFloatArray(ctx, key, &fa);
  return code;
}

XWPSCieJointCaches::XWPSCieJointCaches()
	:XWPSStruct()
{
	cspace_id = render_id = ps_no_id;
	id_status = status = CIE_JC_STATUS_BUILT;
	skipDecodeABC = false;
	skipDecodeLMN = false;
	skipPQR = false;
	skipEncodeLMN = false;
}

void XWPSCieJointCaches::complete(const XWPSCie * pcie,
			  				const XWPSCieABC * pabc,
			          const XWPSCieRender * pcrd)
{
	PSMatrix3 mat3, mat2, mat1;
  PSMatrix3 MatrixLMN_PQR;
  int j;
  
  if (pcrd->caches.EncodeLMN[0].floats.params.is_identity &&
			pcrd->caches.EncodeLMN[1].floats.params.is_identity &&
			pcrd->caches.EncodeLMN[2].floats.params.is_identity) 
	{
		cie_matrix_mult3(&pcrd->MatrixABCEncode, &pcrd->MatrixPQR_inverse_LMN, &mat3);
		skipEncodeLMN = true;
  } 
  else 
  {
		mat3 = pcrd->MatrixPQR_inverse_LMN;
		skipEncodeLMN = false;
  }
  
  cache3_set_linear(TransformPQR);
  cie_matrix_mult3(&pcrd->MatrixPQR, &pcie->common.MatrixLMN, &MatrixLMN_PQR);
  if (TransformPQR[0].floats.params.is_identity &
			TransformPQR[1].floats.params.is_identity &
			TransformPQR[2].floats.params.is_identity) 
	{
		cie_matrix_mult3(&mat3, &MatrixLMN_PQR, &mat2);
		skipPQR = true;
  } 
  else 
  {
		mat2 = MatrixLMN_PQR;
		for (j = 0; j < 3; j++) 
		{
	    cie_cache_restrict(&TransformPQR[j].floats,  &pcrd->RangePQR.ranges[j]);
		}
		cie_cache_mult3(TransformPQR, &mat3);
		skipPQR = false;
  }
  
  if (pcie->common.caches.DecodeLMN[0].floats.params.is_identity &
			pcie->common.caches.DecodeLMN[1].floats.params.is_identity &
			pcie->common.caches.DecodeLMN[2].floats.params.is_identity) 
	{
		if (!pabc) 
		{
	    skipDecodeLMN = mat2.is_identity;
	    skipDecodeABC = false;
	    if (!skipDecodeLMN)
				for (j = 0; j < 3; j++) 
				{
		    	cie_cache_mult(&DecodeLMN[j], &mat2.cu + j,  &pcie->common.caches.DecodeLMN[j].floats);
				}
		} 
		else 
		{
	    cie_matrix_mult3(&mat2, &pabc->MatrixABC, &mat1);
	    for (j = 0; j < 3; j++) 
	    {
				cie_cache_mult(&DecodeLMN[j], &mat1.cu + j,  &pabc->caches.DecodeABC[j].floats);
	    }
	    skipDecodeLMN = false;
	    skipDecodeABC = true;
		}
  } 
  else 
  {
		for (j = 0; j < 3; j++) 
		{
	    cie_cache_mult(&DecodeLMN[j], &mat2.cu + j,  &pcie->common.caches.DecodeLMN[j].floats);
		}
		skipDecodeLMN = false;
		skipDecodeABC = pabc != 0 && pabc->caches.skipABC;
  }
}

int XWPSCieJointCaches::computePointsSD(const XWPSCie * pcie, const XWPSCieRender * pcrd)
{
	PSCieWBSD *pwbsd = &points_sd;

  pwbsd->ws.xyz = pcie->common.points.WhitePoint;
  cie_mult3(&pwbsd->ws.xyz, &pcrd->MatrixPQR, &pwbsd->ws.pqr);
  pwbsd->bs.xyz = pcie->common.points.BlackPoint;
  cie_mult3(&pwbsd->bs.xyz, &pcrd->MatrixPQR, &pwbsd->bs.pqr);
  pwbsd->wd.xyz = pcrd->points.WhitePoint;
  pwbsd->wd.pqr = pcrd->wdpqr;
  pwbsd->bd.xyz = pcrd->points.BlackPoint;
  pwbsd->bd.pqr = pcrd->bdpqr;
  return 0;
}

int XWPSCieJointCaches::getLength()
{
	return sizeof(XWPSCieJointCaches);
}

const char * XWPSCieJointCaches::getTypeName()
{
	return "ciejoincaches";
}

int XWPSCieJointCaches::init(const XWPSCie * pcie, XWPSCieRender * pcrd)
{
	bool is_identity;
  int j;

  computePointsSD(pcie, pcrd);
  if (pcrd->TransformPQR.proc == TransformPQR_from_cache.proc)
		return 0;
  is_identity = pcrd->TransformPQR.proc == TransformPQR_default.proc;
  for (j = 0; j < 3; j++) 
  {
		int i;
		PSForLoopParams lp;

		ps_cie_cache_init(&TransformPQR[j].floats.params, &lp,  &pcrd->RangePQR.ranges[j]);
		for (i = 0; i < ps_cie_cache_size; lp.init += lp.step, i++) 
		{
	    float out;
	    int code = (pcrd->*pcrd->TransformPQR.proc)(j, lp.init, &points_sd, &out);

	    if (code < 0)
				return code;
	    TransformPQR[j].floats.values[i] = out;
		}
		TransformPQR[j].floats.params.is_identity = is_identity;
  }
  return 0;
}
