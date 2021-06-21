/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSFUNCTION_H
#define XWPSFUNCTION_H

#include "XWPSDataSource.h"

#define FN_MONOTONIC_INCREASING 1
#define FN_MONOTONIC_DECREASING 2

class XWPSFunction;
class XWPSParamList;
class XWPSContextState;
class XWPSFunctionSd;
class XWPSFunctionElIn;
class XWPSFunction1ItSg;
class XWPSFunctionAdOt;
class XWPSFunctionPtCr;
class XWPSFunctionVa;

class XWPSFunctionParams
{
public:
	XWPSFunctionParams():m(0),Domain(0),n(0),Range(0){}
	virtual ~XWPSFunctionParams(); 
	
	virtual int check() {return 0;}
	
	int checkmnDR(int mA, int nA);
	
public:
	int m;
	float *Domain;
	int n;
	float *Range;
};

class XWPSFunctionSdParams : public XWPSFunctionParams
{
public:
	XWPSFunctionSdParams();
	~XWPSFunctionSdParams();
	
	int check();
	
public:
	int Order;
	XWPSDataSource DataSource;
	int BitsPerSample;
	float *Encode;
	float *Decode;
	int *Size;
	XWPSBytes * ds_bytes;
};

class XWPSFunctionElInParams: public XWPSFunctionParams
{
public:
	XWPSFunctionElInParams();
	~XWPSFunctionElInParams();
	
	int check();
	
public:
	float *C0;
	float *C1;
	float N;
};

class XWPSFunction1ItSgParams: public XWPSFunctionParams
{
public:
	XWPSFunction1ItSgParams();
	~XWPSFunction1ItSgParams();
	
	int check();
	
public:
	int k;
	XWPSFunction **Functions;
	float *Bounds;
	float *Encode;
};

class XWPSFunctionAdOtParams: public XWPSFunctionParams
{
public:
	XWPSFunctionAdOtParams();
	~XWPSFunctionAdOtParams();
	
	int check();
	
public:
	XWPSFunction **Functions;
};

class XWPSfunctionPtCrParams: public XWPSFunctionParams
{
public:
	XWPSfunctionPtCrParams();
	~XWPSfunctionPtCrParams();
	
	int check();
	
public:
	XWPSString ops;
};

typedef int (XWPSFunction::*fn_evaluate_proc_t)(const float *, float *);

class XWPSFunctionVaParams: public XWPSFunctionParams
{
public:
	XWPSFunctionVaParams();
	~XWPSFunctionVaParams();
	
	int check();
	
public:
	fn_evaluate_proc_t eval_proc;
	void *eval_data;
	int is_monotonic;
};

class XWPSFunctionInfo
{
public:
	XWPSFunctionInfo();
	~XWPSFunctionInfo();
	
public:
	XWPSDataSource *DataSource;
	ulong data_size;
	XWPSFunction **Functions;
	int num_Functions;
};

#define function_type_Sampled 0

enum {
    function_type_ExponentialInterpolation = 2,
    function_type_1InputStitching = 3,
    function_type_ArrayedOutput = -1
};

#define function_type_PostScript_Calculator 4

struct PSFunctionProcs
{
	union
	{
		int (XWPSFunction::*evaluate)(const float *, float *);
		int (XWPSFunctionSd::*evaluatesd)(const float *, float *);
		int (XWPSFunctionElIn::*evaluateelin)(const float *, float *);
		int (XWPSFunction1ItSg::*evaluate1itsg)(const float *, float *);
		int (XWPSFunctionAdOt::*evaluateadot)(const float *, float *);
		int (XWPSFunctionPtCr::*evaluateptcr)(const float *, float *);
		int (XWPSFunctionVa::*evaluateva)(const float *, float *);
	}evaluate_;
	
	union
	{
		int (XWPSFunction::*is_monotonic)(const float *,const float *, PSFunctionEffort);
		int (XWPSFunctionSd::*is_monotonicsd)(const float *,const float *, PSFunctionEffort);
		int (XWPSFunctionElIn::*is_monotonicelin)(const float *,const float *, PSFunctionEffort);
		int (XWPSFunction1ItSg::*is_monotonic1itsg)(const float *,const float *, PSFunctionEffort);
		int (XWPSFunctionAdOt::*is_monotonicadot)(const float *,const float *, PSFunctionEffort);
		int (XWPSFunctionPtCr::*is_monotonicptcr)(const float *,const float *, PSFunctionEffort);
		int (XWPSFunctionVa::*is_monotonicva)(const float *,const float *, PSFunctionEffort);
	}is_monotonic_;
	
	union
	{
		void (XWPSFunction::*get_info)(XWPSFunctionInfo *);
		void (XWPSFunctionSd::*get_infosd)(XWPSFunctionInfo *);
		void (XWPSFunctionElIn::*get_infoelin)(XWPSFunctionInfo *);
		void (XWPSFunction1ItSg::*get_info1itsg)(XWPSFunctionInfo *);
		void (XWPSFunctionAdOt::*get_infoadot)(XWPSFunctionInfo *);
		void (XWPSFunctionPtCr::*get_infoptcr)(XWPSFunctionInfo *);
		void (XWPSFunctionVa::*get_infova)(XWPSFunctionInfo *);
	}get_info_;
	
	union
	{
		int  (XWPSFunction::*get_params)(XWPSContextState *, XWPSParamList *plist);
		int (XWPSFunctionSd::*get_paramssd)(XWPSContextState *, XWPSParamList *plist);
		int (XWPSFunctionElIn::*get_paramselin)(XWPSContextState *, XWPSParamList *plist);
		int (XWPSFunction1ItSg::*get_params1itsg)(XWPSContextState *, XWPSParamList *plist);
		int (XWPSFunctionAdOt::*get_paramsadot)(XWPSContextState *, XWPSParamList *plist);
		int (XWPSFunctionPtCr::*get_paramsptcr)(XWPSContextState *, XWPSParamList *plist);
		int (XWPSFunctionVa::*get_paramsva)(XWPSContextState *, XWPSParamList *plist);
	}get_params_;
};

struct PSFunctionHead
{
	int type;
  PSFunctionProcs procs;
  int is_monotonic;
};

class XWPSFunction : public XWPSStruct
{
public:
	XWPSFunction();
	virtual ~XWPSFunction();
	
	int domainIsMonotonic(PSFunctionEffort effort);
	
	int evaluate(const float *in, float *out);
	
	int  functionType() {return head.type;}
	
	virtual int getLength();
	virtual const char * getTypeName();
	
	void getInfo(XWPSFunctionInfo *pfi);
	void getInfoDefault(XWPSFunctionInfo *pfi);
	int  getParams(XWPSContextState * ctx, XWPSParamList *plist);
	int  getParamsCommon(XWPSContextState * ctx, XWPSParamList *plist);
	
	int isMonotonic(const float *lower, const float *upper, PSFunctionEffort effort);
	
public:
	 PSFunctionHead head;
	 XWPSFunctionParams * params;
};

class XWPSFunctionSd : public XWPSFunction
{
public:
	XWPSFunctionSd(XWPSFunctionSdParams * paramsA);
	~XWPSFunctionSd();
	
	int evaluateSd(const float *in, float *out);
	
	virtual int getLength();
	virtual const char * getTypeName();
	void getInfoSd(XWPSFunctionInfo *pfi);
	int  getParamsSd(XWPSContextState * ctx, XWPSParamList *plist);
	int gets1(ulong offset, uint * samples);
	int gets2(ulong offset, uint * samples);
	int gets4(ulong offset, uint * samples);
	int gets8(ulong offset, uint * samples);
	int gets12(ulong offset, uint * samples);
	int gets16(ulong offset, uint * samples);
	int gets24(ulong offset, uint * samples);
	int gets32(ulong offset, uint * samples);
	
	void interpolateLinear(const float *fparts,
		 										 const ulong *factors, 
		 										 float *samples, 
		 										 ulong offset, 
		 										 int m);
		 										 
	int isMonotonicSd(const float *lower, const float *upper, PSFunctionEffort effort);
};

class XWPSFunctionElIn : public XWPSFunction
{
public:
	XWPSFunctionElIn(XWPSFunctionElInParams * paramsA);
	~XWPSFunctionElIn();
	
	int evaluateElIn(const float *in, float *out);
	
	virtual int getLength();
	virtual const char * getTypeName();
	int getParamsElIn(XWPSContextState * ctx, XWPSParamList *plist);
	
	int isMonotonicElIn(const float *lower, const float *upper, PSFunctionEffort effort);	
};

class XWPSFunction1ItSg : public XWPSFunction
{
public:
	XWPSFunction1ItSg(XWPSFunction1ItSgParams * paramsA);
	~XWPSFunction1ItSg();
	
	int evaluate1ItSg(const float *in, float *out);
	
	virtual int getLength();
	virtual const char * getTypeName();
	void getInfo1ItSg(XWPSFunctionInfo *pfi);
	int  getParams1ItSg(XWPSContextState * ctx, XWPSParamList *plist);
	
	int isMonotonic1ItSg(const float *lower, const float *upper, PSFunctionEffort effort);
};

class XWPSFunctionAdOt : public XWPSFunction
{
public:
	XWPSFunctionAdOt(XWPSFunctionAdOtParams * paramsA);
	~XWPSFunctionAdOt();
	
	int evaluateAdOt(const float *in, float *out);
	
	virtual int getLength();
	virtual const char * getTypeName();
	
	int isMonotonicAdOt(const float *lower, const float *upper, PSFunctionEffort effort);
};

class XWPSFunctionPtCr : public XWPSFunction
{
public:
	XWPSFunctionPtCr(XWPSfunctionPtCrParams * paramsA);
	~XWPSFunctionPtCr();
	
	int calcPut(XWPSStream *s);
	
	int evaluatePtCr(const float *in, float *out);
	
	virtual int getLength();
	virtual const char * getTypeName();
	void getInfoPtCr(XWPSFunctionInfo *pfi);
	
	int isMonotonicPtCr(const float *, const float *, PSFunctionEffort) {return 0;}
	
public:
	XWPSDataSource data_source;
};

class XWPSFunctionVa : public XWPSFunction
{
public:
	XWPSFunctionVa(XWPSFunctionVaParams * paramsA);
	~XWPSFunctionVa();
	
	virtual int getLength();
	virtual const char * getTypeName();
	int isMonotonicVa(const float *, const float *, PSFunctionEffort );
};

#endif //XWPSFUNCTION_H
