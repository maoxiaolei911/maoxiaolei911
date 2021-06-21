/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSCIE_H
#define XWPSCIE_H

#include "XWPSType.h"

class XWPSColorSpace;
class XWPSContextState;
class XWPSState;
class XWPSParamList;
class XWPSDevice;
class XWPSCie;
class XWPSCieA;
class XWPSCieABC;
class XWPSCieDef;
class XWPSCieDefg;
class XWPSCieRender;

#define float_lshift(v, nb) ((v) * (1L << (nb)))
#define float_rshift(v, nb) ((v) * (1.0 / (1L << (nb))))

#ifdef CIE_CACHE_INTERPOLATE
#  define _cie_interpolate_bits\
     qMin(arch_sizeof_int * 8 - ps_cie_log2_cache_size - 2, 10)
#  define _cix(i) ((i) >> _cie_interpolate_bits)
#  define _cif(i) ((int)(i) & ((1 << _cie_interpolate_bits) - 1))
#  define cie_interpolate_between(v0, v1, i)\
     ((v0) + cie_cached_rshift(((v1) - (v0)) * _cif(i) +\
			        (1 << (_cie_interpolate_bits - 1)),\
			       _cie_interpolate_bits))
#  define cie_interpolate(p, i)\
     cie_interpolate_between((p)[_cix(i)], (p)[_cix(i) + 1], i)
#  define cie_interpolate_fracs(p, i)\
     ((p)[_cix(i)] +\
      (short)arith_rshift((long)((p)[_cix(i) + 1] - (p)[_cix(i)]) * _cif(i), _cie_interpolate_bits))
#else
#  define _cie_interpolate_bits 0
#  define cie_interpolate_between(v0, v1, i) (v0)
#  define cie_interpolate(p, i) ((p)[i])
#  define cie_interpolate_fracs(p, i) ((p)[i])
#endif

#ifdef CIE_CACHE_USE_FIXED
typedef long cie_cached_value;

#  define _cie_fixed_shift CIE_FIXED_FRACTION_BITS
#  define float2cie_cached(v)\
     ((cie_cached_value)float_lshift(v, _cie_fixed_shift))
#  define cie_cached2float(v)\
     float_rshift(v, _cie_fixed_shift)
#  define cie_cached2int(v, fbits)\
     arith_rshift(v, _cie_fixed_shift - (fbits))
     
#  define _cie_product_excess_bits\
     (_cie_fixed_shift * 2 + ps_cie_log2_cache_size - (arch_sizeof_long * 8 - 1))
#  define cie_cached_product2int(v, factor, fbits)\
     (_cie_product_excess_bits > 0 ?\
      arith_rshift( (v) * arith_rshift(factor, _cie_product_excess_bits) +\
		    arith_rshift(v, _cie_product_excess_bits) *\
		     ((factor) & ((1 << _cie_product_excess_bits) - 1)),\
		    _cie_fixed_shift * 2 - _cie_product_excess_bits - (fbits)) :\
      arith_rshift((v) * (factor), _cie_fixed_shift * 2 - (fbits)))
#  define cie_cached_rshift(v, n) arith_rshift(v, n)
#else
typedef float cie_cached_value;
#  define float2cie_cached(v) (v)
#  define cie_cached2float(v) (v)
#  define cie_cached2int(v, fbits)\
     ((int)float_lshift(v, fbits))
#  define cie_cached_product2int(v, factor, fbits)\
     ((int)float_lshift((v) * (factor), fbits))
#  define cie_cached_rshift(v, n) float_rshift(v, n)
#endif

#define CIE_CLAMP_INDEX(index)\
  index = (index < 0 ? 0 :\
	   index >= ps_cie_cache_size ? ps_cie_cache_size - 1 : index)
	   	
#define LOOKUP_INDEX(vin, pcache, fbits)\
  ((vin) <= (pcache)->vecs.params.base ? 0 :\
   (vin) >= (pcache)->vecs.params.limit ? (ps_cie_cache_size - 1) << (fbits) :\
   cie_cached_product2int( ((vin) - (pcache)->vecs.params.base),\
			   (pcache)->vecs.params.factor, fbits ))
			   
#define LOOKUP_VALUE(vin, pcache)\
  ((pcache)->vecs.values[LOOKUP_INDEX(vin, pcache, 0)])
  
extern const PSVector3 MatrixA_default;
extern const PSVector3 BlackPoint_default;

extern const PSMatrix3 Matrix3_default;


extern const PSRange RangeA_default;

extern const PSRange3 Range3_default;

extern const PSRange4 Range4_default;

typedef int (XWPSCieRender::*ps_cie_transform_proc)(int, float, const PSCieWBSD *,	float *);

struct PSCieTransformProc3
{
	ps_cie_transform_proc proc;
	const char *proc_name;
	const uchar *data;
	uint   size;
  const char *driver_name;
};

typedef float (XWPSCieRender::*ps_cie_render_proc)(float);
struct PSCieRenderProc3 
{
  ps_cie_render_proc procs[3];
};

typedef ushort(XWPSCieRender::*ps_cie_render_table_proc)(uchar);
struct PSCieRenderTableProcs 
{
  ps_cie_render_table_proc procs[4];
};

float ps_cie_cached_value(float in, const PSCieCacheFloats *pcache);

void ps_cie_cache_init(PSCieCacheParams * pcache, PSForLoopParams * pflp, const PSRange * domain);

void ps_cie_cache_to_fracs(const PSCieCacheFloats *pfloats, PSCieCacheFracs *pfracs);

void cie_lookup_mult3(PSCieCachedVector3 * pvec, const PSCieVectorCache * pc);

#define cie_lookup_map3(pvec, pc, cname) cie_lookup_mult3(pvec, pc)

#define PS_DEVICE_CRD1_TYPE 101

class XWPSColorLookupTable
{
public:
	XWPSColorLookupTable();
	~XWPSColorLookupTable();
	
	void interpolateAccum(const long * pi, ushort * pv, long factor);
	void interpolateLinear(const long * pi, ushort * pv);
	void interpolateNearest(const long * pi, ushort * pv);
	
public:
	int n;
  int dims[4];
  int m;
  XWPSString *table;
  int table_size;
  bool * own_datas;
};

typedef float (XWPSCie::*cie_common_proc)(float);
struct PSCieCommonProc3 
{
    cie_common_proc procs[3];
};

extern const PSCieCommonProc3 DecodeLMN_default;
extern const PSCieCommonProc3 DecodeLMN_from_cache;

class XWPSCie : public XWPSStruct
{
public:
	XWPSCie(void *client_dataA = 0);
	virtual ~XWPSCie() {}
	
	bool cache3IsIdentity();
	void commonComplete();
	float commonIdentity(float in) {return in;}
	
	virtual int getLength();
	virtual const char * getTypeName();
	
	int installCSpace(XWPSColorSpace * pcs, XWPSState *pgs);
	
	float lmnfromCache0(float in);
	float lmnfromCache1(float in);
	float lmnfromCache2(float in);
	
	bool scalarCacheIsExponential(const PSCieScalarCache * pc, float *pexpt);
	bool scalar3CacheIsExponential(PSVector3 * expts);
	
	bool vectorCacheIsExponential(const PSCieVectorCache * pc, float *pexpt);
	bool valuesAreExponential(float va, float vb, float k, float *pexpt);
	
public:
	struct
	{
		int (XWPSColorSpace::*install_cspace)(XWPSState *);
    void *client_data;
		PSRange3 RangeLMN;
		PSCieCommonProc3 DecodeLMN;
		PSMatrix3 MatrixLMN;
		PSCieWB points;
		struct 
		{
			PSCieScalarCache DecodeLMN[3];
  	} caches;
  } common;
};

typedef float (XWPSCieA::*cie_a_proc)(float);
	
extern const cie_a_proc DecodeA_default;
extern const cie_a_proc DecodeA_from_cache;

class XWPSCieA : public XWPSCie
{
public:
	XWPSCieA(void *client_dataA = 0);	
	
	void  acomplete();
	float afromCache(float in);
	float aidentity(float in) {return in;}
	
	bool cacheIsIdentity() {return caches.DecodeA.floats.params.is_identity;}
	
	virtual int getLength();
	virtual const char * getTypeName();
		
public:
	PSRange RangeA;
	cie_a_proc DecodeA;
	PSVector3 MatrixA;
	struct 
	{
		PSCieVectorCache DecodeA;
  } caches;
};

typedef float (XWPSCieABC::*cie_abc_proc) (float);
struct PSCieABCProc3 
{
    cie_abc_proc procs[3];
};

extern const PSCieABCProc3 DecodeABC_default;
extern const PSCieABCProc3 DecodeABC_from_cache;

class XWPSCieABC : public XWPSCie
{
public:
	XWPSCieABC(void *client_dataA = 0);
	
	void  abccomplete();
	float abcfromCache0(float in);
	float abcfromCache1(float in);
	float abcfromCache2(float in);
	float abcidentity(float in) {return in;}
	
	bool cache3IsIdentityABC();
	
	virtual int getLength();
	virtual const char * getTypeName();
	
	bool vector3CacheIsExponential(PSVector3 * expts);
	
public:
	PSRange3 RangeABC;
	PSCieABCProc3 DecodeABC;
	PSMatrix3 MatrixABC;
	struct 
	{
		bool skipABC;
		PSCieVectorCache DecodeABC[3];
	} caches;
};

typedef float (XWPSCieDef::*cie_def_proc)(float);
struct PSCieDEFProc3 
{
    cie_def_proc procs[3];
};

extern const PSCieDEFProc3 DecodeDEF_default;
extern const PSCieDEFProc3 DecodeDEF_from_cache;

class XWPSCieDef : public XWPSCieABC
{
public:
	XWPSCieDef(void *client_dataA = 0);
	
	void  defcomplete();
	float defidentity(float in)  {return in;}
	float deffromCache0(float in);
	float deffromCache1(float in);
	float deffromCache2(float in);
	
	virtual int getLength();
	virtual const char * getTypeName();
	
public:
	PSRange3 RangeDEF;
	PSCieDEFProc3 DecodeDEF;
	PSRange3 RangeHIJ;
	XWPSColorLookupTable Table;
	struct 
	{
		PSCieScalarCache DecodeDEF[3];
  } caches_def;
};

typedef float (XWPSCieDefg::*cie_defg_proc)(float);
struct PSCieDEFGProc4 
{
    cie_defg_proc procs[4];
};

extern const PSCieDEFGProc4 DecodeDEFG_default;
extern const PSCieDEFGProc4 DecodeDEFG_from_cache;

class XWPSCieDefg : public XWPSCieABC
{
public:
	XWPSCieDefg(void *client_dataA = 0);
	
	void  defgcomplete();
	float defgfromCache0(float in);
	float defgfromCache1(float in);
	float defgfromCache2(float in);
	float defgfromCache3(float in);
	float defgidentity(float in)  {return in;}
	
	virtual int getLength();
	virtual const char * getTypeName();
	
public:
  PSRange4 RangeDEFG;
  PSCieDEFGProc4 DecodeDEFG;
  PSRange4 RangeHIJK;
  XWPSColorLookupTable Table;
  struct 
  {
		PSCieScalarCache DecodeDEFG[4];
  } caches_defg;
};

class XWPSCieRenderTable
{
public:
	XWPSCieRenderTable() {}
	
public:
	XWPSColorLookupTable lookup;
  PSCieRenderTableProcs T;
};

extern const PSCieTransformProc3 TransformPQR_default;
extern const PSCieTransformProc3 TransformPQR_from_cache;
extern const ps_cie_transform_proc TransformPQR_lookup_proc_name;
extern const PSCieRenderProc3 Encode_default;
extern const PSCieRenderProc3 EncodeLMN_from_cache;
extern const PSCieRenderProc3 EncodeABC_from_cache;
extern const PSCieRenderTableProcs RenderTableT_default;
extern const PSCieRenderTableProcs RenderTableT_from_cache;

class XWPSCieRender : public XWPSStruct
{
public:
	XWPSCieRender();
	virtual ~XWPSCieRender();
	
	int complete();
	
	float EncodeABCCachedA(float in);
	float EncodeABCCachedB(float in);
	float EncodeABCCachedC(float in);
	float encodeAbc0FromData(float v);
	float encodeAbc1FromData(float v);
	float encodeAbc2FromData(float v);
	float EncodeLMNCachedL(float in);
	float EncodeLMNCachedM(float in);
	float EncodeLMNCachedN(float in);
	float encodeLmn0FromData(float v);
	float encodeLmn1FromData(float v);
	float encodeLmn2FromData(float v);
	
	virtual int getLength();
	virtual const char * getTypeName();
	
	int init();
	
	int paramGetCieRender1(XWPSContextState * ctx, 
	                       XWPSParamList * plist,
		                     XWPSDevice * dev);
	int paramInitialize(XWPSContextState * ctx, 
	                     XWPSParamList * plist,
				               const char * key, 
				               XWPSDevice * dev);
	int paramPutCieRender1(XWPSContextState * ctx, XWPSParamList * plist);
	int paramWriteCieRender1(XWPSContextState * ctx,
	                         XWPSParamList * plist, 
	                         const char * key);
	
	float renderIdentity(float in) {return in;}
	ushort renderTableIdentity(uchar in) {return byte2frac(in);}
	int render1InitFrom(void *client_dataA,
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
			 								const XWPSCieRenderTable * RenderTableA);
	int render1Initialize(void *client_dataA,
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
			                  const XWPSCieRenderTable * RenderTableA);
	ushort renderTableT0FromData(uchar v);
	ushort renderTableT1FromData(uchar v);
	ushort renderTableT2FromData(uchar v);
	ushort renderTableT3FromData(uchar v);
	ushort RTTCached(uchar in, int i);
	ushort RTTCached0(uchar in);
	ushort RTTCached1(uchar in);
	ushort RTTCached2(uchar in);
	ushort RTTCached3(uchar in);
	
	int sample();
	
	int tpqrDoLookup(XWPSDevice *dev_proto);
	int tpqrFromCache(int , 
	                  float in, 
	                  const PSCieWBSD * ,
								    float *out) {*out = in; return 0;}
	int tpqrIdentity(int , 
	                 float in, 
	                 const PSCieWBSD * ,
	                float *out) {*out = in; return 0;}
	int tpqrLookup(int index, 
	                float in, 
	                const PSCieWBSD * pwbsd,
	               float *out);
	
public:
	PSCieRenderStatus status;
	ulong id;
	void *client_data;
	PSCieWB points;
  PSMatrix3 MatrixPQR;
  PSRange3 RangePQR;
  PSCieTransformProc3 TransformPQR;
  PSMatrix3 MatrixLMN;
  PSCieRenderProc3 EncodeLMN;
  PSRange3 RangeLMN;
  PSMatrix3 MatrixABC;
  PSCieRenderProc3 EncodeABC;
  PSRange3 RangeABC;
  XWPSCieRenderTable RenderTable;
  PSRange3 DomainLMN;
  PSRange3 DomainABC;
  PSMatrix3 MatrixABCEncode;
  float EncodeABC_base[3];
  PSMatrix3 MatrixPQR_inverse_LMN;
  PSVector3 wdpqr, bdpqr;
  struct 
  {
		PSCieVectorCache EncodeLMN[3];
		PSCieFloatFixedCache EncodeABC[3];
		PSCieScalarCache RenderTableT[4];
		bool RenderTableT_is_identity;
  } caches;
  
  XWPSDevice * device;
  bool own_proc_name;
  bool own_data;
  
private:
	int writeProc3(XWPSContextState * ctx, 
	               XWPSParamList * plist, const char * key,
	               const PSCieRenderProc3 * procs,
	               const PSRange3 * domain);
};

class XWPSCieJointCaches : public XWPSStruct
{
public:
	XWPSCieJointCaches();
	
	void complete(const XWPSCie * pcie,
			  				const XWPSCieABC * pabc,
			          const XWPSCieRender * pcrd);
			  
	int computePointsSD(const XWPSCie * pcie, const XWPSCieRender * pcrd);
	
	virtual int getLength();
	virtual const char * getTypeName();
	
	int init(const XWPSCie * pcie, XWPSCieRender * pcrd);
	
public:
	long cspace_id;
  long render_id;
  PSCieJointCachesStatus id_status;
  PSCieJointCachesStatus status; 
  bool skipDecodeABC;
  bool skipDecodeLMN;
  PSCieVectorCache DecodeLMN[3];
  PSCieWBSD points_sd;
  bool skipPQR;
  PSCieVectorCache TransformPQR[3];
  bool skipEncodeLMN;
};

#endif //XWPSCIE_H
