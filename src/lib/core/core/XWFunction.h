/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWFUNCTION_H
#define XWFUNCTION_H

#include <QByteArray>
#include "XWObject.h"

#define FUNC_SAMPLED                   0
#define FUNC_EXPONENTIAL_INTERPOLATION 2
#define FUNC_STITCHING                 3
#define FUNC_POSTSCRIPT_CALCULATOR     4


#define FUNC_MAX_INPUTS        32
#define FUNC_MAX_OUTPUTS       32
#define SAMPLED_FUNC_MAX_INPUTS 16


class XWDict;
class XWStream;
class PSStack;
struct PSObject;


class XW_CORE_EXPORT XWFunction
{
public:
    XWFunction();
    virtual ~XWFunction();
                
    virtual XWFunction *copy() = 0;

    double getDomainMin(int i) { return domain[i][0]; }
    double getDomainMax(int i) { return domain[i][1]; }
    bool   getHasRange() { return hasRange; }
    int    getInputSize() { return m; }
    int    getOutputSize() { return n; }
    double getRangeMin(int i) { return range[i][0]; }
    double getRangeMax(int i) { return range[i][1]; }
    virtual int getType() = 0;
    
    bool init(XWDict *dict);
    virtual bool isOk() = 0;
    
    static XWFunction *parse(XWObject *funcObj, int recursion = 0);
    
    virtual void transform(double *in, double *out) = 0;
    
protected:    
    int    m, n;
    double domain[FUNC_MAX_INPUTS][2];
    double range[FUNC_MAX_OUTPUTS][2];
    bool hasRange;
};


class XW_CORE_EXPORT XWIdentityFunc : public XWFunction
{
public:
    XWIdentityFunc();
    virtual ~XWIdentityFunc();
        
    virtual XWFunction *copy() { return new XWIdentityFunc(); }
    
    virtual int getType() { return -1; }
    
    virtual bool isOk() { return true; }
    
    virtual void transform(double *in, double *out);
};


class XW_CORE_EXPORT XWSampledFunc : public XWFunction
{
public:
    XWSampledFunc(XWObject *funcObj, XWDict *dict);
    virtual ~XWSampledFunc();
    
    virtual XWFunction *copy() { return new XWSampledFunc(this); }
    
    virtual bool isOk() { return ok; }
    
    double   getEncodeMin(int i) { return encode[i][0]; }
    double   getEncodeMax(int i) { return encode[i][1]; }
    double   getDecodeMin(int i) { return decode[i][0]; }
    double   getDecodeMax(int i) { return decode[i][1]; }
    double * getSamples() { return samples; }
    int      getSampleSize(int i) { return sampleSize[i]; }
    virtual int getType() { return 0; }
    
    virtual void transform(double *in, double *out);
    
private:
    XWSampledFunc(XWSampledFunc *func);

private:
	int *idxOffset;
	double * samples;
    int nSamples;	
    double *sBuf;
    bool ok;
    
    int	     sampleSize[FUNC_MAX_INPUTS];
    double	 encode[FUNC_MAX_INPUTS][2];
    double	 decode[FUNC_MAX_OUTPUTS][2];
    double	 inputMul[FUNC_MAX_INPUTS];
    int      idxMul[FUNC_MAX_INPUTS];
    
    double cacheIn[FUNC_MAX_INPUTS];
  	double cacheOut[FUNC_MAX_OUTPUTS];
};

class XW_CORE_EXPORT XWExponentialFunc : public XWFunction
{
public:
    XWExponentialFunc(XWObject *, XWDict *dict);
    virtual ~XWExponentialFunc();
        
    virtual XWFunction *copy() { return new XWExponentialFunc(this); }
    
    double * getC0() { return c0; }
    double * getC1() { return c1; }
    double   getE() { return e; }
    virtual int getType() { return 2; }
    virtual bool isOk() { return ok; }
    
    virtual void transform(double *in, double *out);
    
private:
    XWExponentialFunc(XWExponentialFunc *func);
    
private:
	double e;
    bool ok;
    double c0[FUNC_MAX_OUTPUTS];
    double c1[FUNC_MAX_OUTPUTS];
};

class XW_CORE_EXPORT XWStitchingFunc : public XWFunction
{
public:
    XWStitchingFunc(XWObject * funcObj, XWDict *dict, int recursion);
    virtual ~XWStitchingFunc();
        
    virtual XWFunction *copy() { return new XWStitchingFunc(this); }
    
    double *getBounds() { return bounds; }
    double *getEncode() { return encode; }
    XWFunction *getFunc(int i) { return funcs[i]; }
    int getNumFuncs() { return k; }
    double *getScale() { return scale; }
    virtual int getType() { return 3; }
    
    virtual bool isOk() { return ok; }
    
    virtual void transform(double *in, double *out);
        
private:
    XWStitchingFunc(XWStitchingFunc *func);

    int k;
    XWFunction **funcs;
    double *bounds;
    double *encode;
    double *scale;
    bool ok;
};

class XW_CORE_EXPORT XWPostScriptFunc : public XWFunction
{
public:
    XWPostScriptFunc(XWObject *funcObj, XWDict *dict);
    virtual ~XWPostScriptFunc();
        
    virtual XWFunction *copy() { return new XWPostScriptFunc(this); }
    
    XWString *getCodeString() { return codeString; }
    virtual int getType() { return 4; }
    
    virtual bool isOk() { return ok; }
    
    virtual void transform(double *in, double *out);
    
private:
    XWPostScriptFunc(XWPostScriptFunc *func);
    
    void exec(PSStack *stack, int codePtr);
    
    XWString *getToken(XWStream *str);
    
    bool parseCode(XWStream *str, int *codePtr);
    
    void resizeCode(int newSize);
        
private:
    XWString * codeString;
    
    int codeSize;
    PSObject *code;
    
    bool ok;
    
    double cacheIn[FUNC_MAX_INPUTS];
  double cacheOut[FUNC_MAX_OUTPUTS];
};


#endif // XWFUNCTION_H

