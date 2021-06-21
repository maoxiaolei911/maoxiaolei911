/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "XWApplication.h"
#include "XWObject.h"
#include "XWDict.h"
#include "XWStream.h"
#include "XWFunction.h"

#define recursionLimit 8

XWFunction::XWFunction()
{
}

XWFunction::~XWFunction()
{
}

bool XWFunction::init(XWDict *dict)
{
  XWObject obj1, obj2;
  int i;

  //----- Domain
  if (!dict->lookup("Domain", &obj1)->isArray()) {
    xwApp->error("Function is missing domain");
    goto err2;
  }
  m = obj1.arrayGetLength() / 2;
  if (m > FUNC_MAX_INPUTS) {
    xwApp->error("Functions with more than {0:d} inputs are unsupported");
    goto err2;
  }
  for (i = 0; i < m; ++i) {
    obj1.arrayGet(2*i, &obj2);
    if (!obj2.isNum()) {
      xwApp->error("Illegal value in function domain array");
      goto err1;
    }
    domain[i][0] = obj2.getNum();
    obj2.free();
    obj1.arrayGet(2*i+1, &obj2);
    if (!obj2.isNum()) {
      xwApp->error("Illegal value in function domain array");
      goto err1;
    }
    domain[i][1] = obj2.getNum();
    obj2.free();
  }
  obj1.free();

  //----- Range
  hasRange = false;
  n = 0;
  if (dict->lookup("Range", &obj1)->isArray()) {
    hasRange = true;
    n = obj1.arrayGetLength() / 2;
    if (n > FUNC_MAX_OUTPUTS) {
      xwApp->error("Functions with more than {0:d} outputs are unsupported");
      goto err2;
    }
    for (i = 0; i < n; ++i) {
      obj1.arrayGet(2*i, &obj2);
      if (!obj2.isNum()) {
	xwApp->error("Illegal value in function range array");
	goto err1;
      }
      range[i][0] = obj2.getNum();
      obj2.free();
      obj1.arrayGet(2*i+1, &obj2);
      if (!obj2.isNum()) {
	xwApp->error("Illegal value in function range array");
	goto err1;
      }
      range[i][1] = obj2.getNum();
      obj2.free();
    }
  }
  obj1.free();

  return true;

 err1:
  obj2.free();
 err2:
  obj1.free();
  return false;
}

XWFunction * XWFunction::parse(XWObject *funcObj, int recursion)
{
  XWFunction *func;
  XWDict *dict;
  int funcType;
  XWObject obj1;

  if (recursion > recursionLimit) 
  {
    xwApp->error("Loop detected in function objects");
    return NULL;
  }

  if (funcObj->isStream()) 
  {
    dict = funcObj->streamGetDict();
  } 
  else if (funcObj->isDict()) 
  {
    dict = funcObj->getDict();
  } 
  else if (funcObj->isName("Identity")) 
  {
    return new XWIdentityFunc();
  } 
  else 
  {
    xwApp->error("Expected function dictionary or stream");
    return NULL;
  }

  if (!dict->lookup("FunctionType", &obj1)->isInt()) 
  {
    xwApp->error("Function type is missing or wrong type");
    obj1.free();
    return NULL;
  }
  funcType = obj1.getInt();
  obj1.free();

  if (funcType == 0) {
  
    func = new XWSampledFunc(funcObj, dict);
  } 
  else if (funcType == 2) 
  {
    func = new XWExponentialFunc(funcObj, dict);
  } 
  else if (funcType == 3) 
  {
    func = new XWStitchingFunc(funcObj, dict, recursion);
  } 
  else if (funcType == 4) 
  {
    func = new XWPostScriptFunc(funcObj, dict);
  } 
  else 
  {
    xwApp->error("Unimplemented function type");
    return NULL;
  }
  
  if (!func->isOk()) 
  {
    delete func;
    return NULL;
  }

  return func;
}

XWIdentityFunc::XWIdentityFunc()
{
    m = FUNC_MAX_INPUTS;
    n = FUNC_MAX_OUTPUTS;
    for (int i = 0; i < FUNC_MAX_INPUTS; ++i) 
    {
        domain[i][0] = 0;
        domain[i][1] = 1;
    }
    hasRange = false;
}

XWIdentityFunc::~XWIdentityFunc()
{
}

void XWIdentityFunc::transform(double *in, double *out)
{
    for (int i = 0; i < FUNC_MAX_OUTPUTS; ++i) 
        out[i] = in[i];
}

XWSampledFunc::XWSampledFunc(XWObject *funcObj, XWDict *dict)
{
  XWStream *str;
  int sampleBits;
  double sampleMul;
  XWObject obj1, obj2;
  uint buf, bitMask;
  int bits;
  uint s;
  double in[FUNC_MAX_INPUTS];
  int i, j, t, bit, idx;

  idxOffset = NULL;
  samples = NULL;
  sBuf = NULL;
  ok = false;

  //----- initialize the generic stuff
  if (!init(dict)) 
  {
    goto err1;
  }
  if (!hasRange) 
  {
    xwApp->error("Type 0 function is missing range");
    goto err1;
  }
  if (m > SAMPLED_FUNC_MAX_INPUTS) 
    goto err1;

  //----- buffer
  sBuf = (double *)malloc((1 << m) * sizeof(double));

  //----- get the stream
  if (!funcObj->isStream()) 
  {
    xwApp->error("Type 0 function isn't a stream");
    goto err1;
  }
  str = funcObj->getStream();

  //----- Size
  if (!dict->lookup("Size", &obj1)->isArray() || obj1.arrayGetLength() != m) 
  {
    xwApp->error("Function has missing or invalid size array");
    goto err2;
  }
  for (i = 0; i < m; ++i) 
  {
    obj1.arrayGet(i, &obj2);
    if (!obj2.isInt()) 
    {
      xwApp->error("Illegal value in function size array");
      goto err3;
    }
    sampleSize[i] = obj2.getInt();
    if (sampleSize[i] <= 0) 
    {
      xwApp->error("Illegal non-positive value in function size array");
      goto err3;
    }
    obj2.free();
  }
  obj1.free();
  idxOffset = (int *)malloc((1 << m) * sizeof(int));
  for (i = 0; i < (1<<m); ++i) 
  {
    idx = 0;
    for (j = m - 1, t = i; j >= 1; --j, t <<= 1) 
    {
      if (sampleSize[j] == 1) 
      {
				bit = 0;
      } 
      else 
      {
				bit = (t >> (m - 1)) & 1;
      }
      idx = (idx + bit) * sampleSize[j-1];
    }
    if (sampleSize[0] == 1) 
    {
      bit = 0;
    } 
    else 
    {
      bit = (t >> (m - 1)) & 1;
    }
    idxOffset[i] = (idx + bit) * n;
  }

  //----- BitsPerSample
  if (!dict->lookup("BitsPerSample", &obj1)->isInt()) 
  {
    xwApp->error("Function has missing or invalid BitsPerSample");
    goto err2;
  }
  sampleBits = obj1.getInt();
  sampleMul = 1.0 / (pow(2.0, (double)sampleBits) - 1);
  obj1.free();

  //----- Encode
  if (dict->lookup("Encode", &obj1)->isArray() && obj1.arrayGetLength() == 2*m) 
  {
    for (i = 0; i < m; ++i) 
    {
      obj1.arrayGet(2*i, &obj2);
      if (!obj2.isNum()) 
      {
				xwApp->error("Illegal value in function encode array");
				goto err3;
      }
      encode[i][0] = obj2.getNum();
      obj2.free();
      obj1.arrayGet(2*i+1, &obj2);
      if (!obj2.isNum()) 
      {
				xwApp->error("Illegal value in function encode array");
				goto err3;
      }
      encode[i][1] = obj2.getNum();
      obj2.free();
    }
  } 
  else 
  {
    for (i = 0; i < m; ++i) {
      encode[i][0] = 0;
      encode[i][1] = sampleSize[i] - 1;
    }
  }
  obj1.free();
  for (i = 0; i < m; ++i) 
 	{
    inputMul[i] = (encode[i][1] - encode[i][0]) /  (domain[i][1] - domain[i][0]);
  }

  //----- Decode
  if (dict->lookup("Decode", &obj1)->isArray() && obj1.arrayGetLength() == 2*n) 
  {
    for (i = 0; i < n; ++i) 
    {
      obj1.arrayGet(2*i, &obj2);
      if (!obj2.isNum()) 
      {
				xwApp->error("Illegal value in function decode array");
				goto err3;
      }
      decode[i][0] = obj2.getNum();
      obj2.free();
      obj1.arrayGet(2*i+1, &obj2);
      if (!obj2.isNum()) 
      {
				xwApp->error("Illegal value in function decode array");
				goto err3;
      }
      decode[i][1] = obj2.getNum();
      obj2.free();
    }
  } 
  else 
  {
    for (i = 0; i < n; ++i) 
   	{
      decode[i][0] = range[i][0];
      decode[i][1] = range[i][1];
    }
  }
  obj1.free();

  //----- samples
  nSamples = n;
  for (i = 0; i < m; ++i)
    nSamples *= sampleSize[i];
  samples = (double *)malloc(nSamples * sizeof(double));
  buf = 0;
  bits = 0;
  bitMask = (1 << sampleBits) - 1;
  str->reset();
  for (i = 0; i < nSamples; ++i) 
  {
    if (sampleBits == 8) 
    {
      s = str->getChar();
    } 
    else if (sampleBits == 16) 
    {
      s = str->getChar();
      s = (s << 8) + str->getChar();
    } 
    else if (sampleBits == 32) 
    {
      s = str->getChar();
      s = (s << 8) + str->getChar();
      s = (s << 8) + str->getChar();
      s = (s << 8) + str->getChar();
    } 
    else 
    {
      while (bits < sampleBits) 
      {
				buf = (buf << 8) | (str->getChar() & 0xff);
				bits += 8;
      }
      s = (buf >> (bits - sampleBits)) & bitMask;
      bits -= sampleBits;
    }
    samples[i] = (double)s * sampleMul;
  }
  str->close();

  // set up the cache
  for (i = 0; i < m; ++i) 
  {
    in[i] = domain[i][0];
    cacheIn[i] = in[i] - 1;
  }
  transform(in, cacheOut);

  ok = true;
  return;

 err3:
  obj2.free();
 err2:
  obj1.free();
 err1:
  return;
}

XWSampledFunc::XWSampledFunc(XWSampledFunc *func)
{
  memcpy(this, func, sizeof(XWSampledFunc));
  idxOffset = (int *)malloc((1 << m) * sizeof(int));
  memcpy(idxOffset, func->idxOffset, (1 << m) * (int)sizeof(int));
  samples = (double *)malloc(nSamples * sizeof(double));
  memcpy(samples, func->samples, nSamples * sizeof(double));
  sBuf = (double *)malloc((1 << m) * sizeof(double));
}

XWSampledFunc::~XWSampledFunc()
{
    if (idxOffset) 
    {
    	free(idxOffset);
  	}
  	if (samples) 
  	{
    	free(samples);
  	}
  	if (sBuf) 
  	{
    	free(sBuf);
  	}
}

void XWSampledFunc::transform(double *in, double *out)
{
  double x;
  int e[FUNC_MAX_INPUTS];
  double efrac0[FUNC_MAX_INPUTS];
  double efrac1[FUNC_MAX_INPUTS];
  int i, j, k, idx0, t;

  // check the cache
  for (i = 0; i < m; ++i) 
  {
    if (in[i] != cacheIn[i]) 
    {
      break;
    }
  }
  if (i == m) 
  {
    for (i = 0; i < n; ++i) 
    {
      out[i] = cacheOut[i];
    }
    return;
  }

  // map input values into sample array
  for (i = 0; i < m; ++i) 
  {
    x = (in[i] - domain[i][0]) * inputMul[i] + encode[i][0];
    if (x < 0 || x != x) 
    {  // x!=x is a more portable version of isnan(x)
      x = 0;
    } 
    else if (x > sampleSize[i] - 1) 
    {
      x = sampleSize[i] - 1;
    }
    e[i] = (int)x;
    if (e[i] == sampleSize[i] - 1 && sampleSize[i] > 1) 
    {
      // this happens if in[i] = domain[i][1]
      e[i] = sampleSize[i] - 2;
    }
    efrac1[i] = x - e[i];
    efrac0[i] = 1 - efrac1[i];
  }

  // compute index for the first sample to be used
  idx0 = 0;
  for (k = m - 1; k >= 1; --k) 
  {
    idx0 = (idx0 + e[k]) * sampleSize[k-1];
  }
  idx0 = (idx0 + e[0]) * n;

  // for each output, do m-linear interpolation
  for (i = 0; i < n; ++i) 
  {

    // pull 2^m values out of the sample array
    for (j = 0; j < (1<<m); ++j) 
    {
      sBuf[j] = samples[idx0 + idxOffset[j] + i];
    }

    // do m sets of interpolations
    for (j = 0, t = (1<<m); j < m; ++j, t >>= 1) 
    {
      for (k = 0; k < t; k += 2) 
      {
				sBuf[k >> 1] = efrac0[j] * sBuf[k] + efrac1[j] * sBuf[k+1];
      }
    }

    // map output value to range
    out[i] = sBuf[0] * (decode[i][1] - decode[i][0]) + decode[i][0];
    if (out[i] < range[i][0]) 
    {
      out[i] = range[i][0];
    } 
    else if (out[i] > range[i][1]) 
    {
      out[i] = range[i][1];
    }
  }

  // save current result in the cache
  for (i = 0; i < m; ++i) 
  {
    cacheIn[i] = in[i];
  }
  for (i = 0; i < n; ++i) 
  {
    cacheOut[i] = out[i];
  }
}

XWExponentialFunc::XWExponentialFunc(XWObject *, XWDict *dict)
{
  XWObject obj1, obj2;
  int i;

  ok = false;

  //----- initialize the generic stuff
  if (!init(dict)) {
    goto err1;
  }
  if (m != 1) {
    xwApp->error("Exponential function with more than one input");
    goto err1;
  }

  //----- C0
  if (dict->lookup("C0", &obj1)->isArray()) {
    if (hasRange && obj1.arrayGetLength() != n) {
      xwApp->error("Function's C0 array is wrong length");
      goto err2;
    }
    n = obj1.arrayGetLength();
    for (i = 0; i < n; ++i) {
      obj1.arrayGet(i, &obj2);
      if (!obj2.isNum()) {
	xwApp->error("Illegal value in function C0 array");
	goto err3;
      }
      c0[i] = obj2.getNum();
      obj2.free();
    }
  } else {
    if (hasRange && n != 1) {
      xwApp->error("Function's C0 array is wrong length");
      goto err2;
    }
    n = 1;
    c0[0] = 0;
  }
  obj1.free();

  //----- C1
  if (dict->lookup("C1", &obj1)->isArray()) {
    if (obj1.arrayGetLength() != n) {
      xwApp->error("Function's C1 array is wrong length");
      goto err2;
    }
    for (i = 0; i < n; ++i) {
      obj1.arrayGet(i, &obj2);
      if (!obj2.isNum()) {
	xwApp->error("Illegal value in function C1 array");
	goto err3;
      }
      c1[i] = obj2.getNum();
      obj2.free();
    }
  } else {
    if (n != 1) {
      xwApp->error("Function's C1 array is wrong length");
      goto err2;
    }
    c1[0] = 1;
  }
  obj1.free();

  //----- N (exponent)
  if (!dict->lookup("N", &obj1)->isNum()) {
    xwApp->error("Function has missing or invalid N");
    goto err2;
  }
  e = obj1.getNum();
  obj1.free();

  ok = true;
  return;

 err3:
  obj2.free();
 err2:
  obj1.free();
 err1:
  return;
}

XWExponentialFunc::XWExponentialFunc(XWExponentialFunc *func)
{
    memcpy(this, func, sizeof(XWExponentialFunc));
}

XWExponentialFunc::~XWExponentialFunc()
{
}

void XWExponentialFunc::transform(double *in, double *out)
{
    double x = 0.0;
    if (in[0] < domain[0][0]) 
        x = domain[0][0];
    else if (in[0] > domain[0][1]) 
        x = domain[0][1];
    else 
        x = in[0];
    
    for (int i = 0; i < n; ++i) 
    {
        out[i] = c0[i] + pow(x, e) * (c1[i] - c0[i]);
        if (hasRange) 
        {
            if (out[i] < range[i][0]) 
	            out[i] = range[i][0];
            else if (out[i] > range[i][1]) 
	            out[i] = range[i][1];
        }
    }
}

XWStitchingFunc::XWStitchingFunc(XWObject *, XWDict *dict, int recursion)
{
  XWObject obj1, obj2;
  int i;

  ok = false;
  funcs = NULL;
  bounds = NULL;
  encode = NULL;
  scale = NULL;

  //----- initialize the generic stuff
  if (!init(dict)) {
    goto err1;
  }
  if (m != 1) {
    xwApp->error("Stitching function with more than one input");
    goto err1;
  }

  //----- Functions
  if (!dict->lookup("Functions", &obj1)->isArray()) {
    xwApp->error("Missing 'Functions' entry in stitching function");
    goto err1;
  }
  k = obj1.arrayGetLength();
  funcs = (XWFunction **)malloc(k * sizeof(XWFunction *));
  bounds = (double *)malloc((k + 1) * sizeof(double));
  encode = (double *)malloc(2 * k * sizeof(double));
  scale = (double *)malloc(k * sizeof(double));
  for (i = 0; i < k; ++i) {
    funcs[i] = NULL;
  }
  for (i = 0; i < k; ++i) {
    if (!(funcs[i] = XWFunction::parse(obj1.arrayGet(i, &obj2),
				     recursion + 1))) {
      goto err2;
    }
    if (funcs[i]->getInputSize() != 1 ||
	(i > 0 && funcs[i]->getOutputSize() != funcs[0]->getOutputSize())) {
      xwApp->error("Incompatible subfunctions in stitching function");
      goto err2;
    }
    obj2.free();
  }
  obj1.free();

  //----- Bounds
  if (!dict->lookup("Bounds", &obj1)->isArray() ||
      obj1.arrayGetLength() != k - 1) {
    xwApp->error("Missing or invalid 'Bounds' entry in stitching function");
    goto err1;
  }
  bounds[0] = domain[0][0];
  for (i = 1; i < k; ++i) {
    if (!obj1.arrayGet(i - 1, &obj2)->isNum()) {
      xwApp->error("Invalid type in 'Bounds' array in stitching function");
      goto err2;
    }
    bounds[i] = obj2.getNum();
    obj2.free();
  }
  bounds[k] = domain[0][1];
  obj1.free();

  //----- Encode
  if (!dict->lookup("Encode", &obj1)->isArray() ||
      obj1.arrayGetLength() != 2 * k) {
    xwApp->error("Missing or invalid 'Encode' entry in stitching function");
    goto err1;
  }
  for (i = 0; i < 2 * k; ++i) {
    if (!obj1.arrayGet(i, &obj2)->isNum()) {
      xwApp->error("Invalid type in 'Encode' array in stitching function");
      goto err2;
    }
    encode[i] = obj2.getNum();
    obj2.free();
  }
  obj1.free();

  //----- pre-compute the scale factors
  for (i = 0; i < k; ++i) {
    if (bounds[i] == bounds[i+1]) {
      // avoid a divide-by-zero -- in this situation, function i will
      // never be used anyway
      scale[i] = 0;
    } else {
      scale[i] = (encode[2*i+1] - encode[2*i]) / (bounds[i+1] - bounds[i]);
    }
  }

  ok = true;
  return;

 err2:
  obj2.free();
 err1:
  obj1.free();
}

XWStitchingFunc::XWStitchingFunc(XWStitchingFunc *func)
{
   memcpy(this, func, sizeof(XWStitchingFunc));
  funcs = (XWFunction **)malloc(k * sizeof(XWFunction *));
  for (int i = 0; i < k; ++i) 
  {
    funcs[i] = func->funcs[i]->copy();
  }
  bounds = (double *)malloc((k + 1) * sizeof(double));
  memcpy(bounds, func->bounds, (k + 1) * sizeof(double));
  encode = (double *)malloc(2 * k * sizeof(double));
  memcpy(encode, func->encode, 2 * k * sizeof(double));
  scale = (double *)malloc(k * sizeof(double));
  memcpy(scale, func->scale, k * sizeof(double));
  ok = true;
}

XWStitchingFunc::~XWStitchingFunc()
{
    if (funcs)
    {
        for (int i = 0; i < k; ++i)
        {
            if (funcs[i])
                delete funcs[i];
        }
        
        free(funcs);
    }
    	
    if (bounds)
  		free(bounds);
  		
  	if (encode)
  		free(encode);
  		
  	if (scale)
  		free(scale);
}

void XWStitchingFunc::transform(double *in, double *out)
{
    double x = 0.0;
    if (in[0] < domain[0][0]) 
        x = domain[0][0];
    else if (in[0] > domain[0][1]) 
        x = domain[0][1];
    else 
        x = in[0];
    
    int i = 0;
    for (; i < k - 1; ++i) 
    {
        if (x < bounds[i+1]) 
            break;
    }
    x = encode[2*i] + (x - bounds[i]) * scale[i];
    funcs[i]->transform(&x, out);
}


enum PSOp {
  psOpAbs,
  psOpAdd,
  psOpAnd,
  psOpAtan,
  psOpBitshift,
  psOpCeiling,
  psOpCopy,
  psOpCos,
  psOpCvi,
  psOpCvr,
  psOpDiv,
  psOpDup,
  psOpEq,
  psOpExch,
  psOpExp,
  psOpFalse,
  psOpFloor,
  psOpGe,
  psOpGt,
  psOpIdiv,
  psOpIndex,
  psOpLe,
  psOpLn,
  psOpLog,
  psOpLt,
  psOpMod,
  psOpMul,
  psOpNe,
  psOpNeg,
  psOpNot,
  psOpOr,
  psOpPop,
  psOpRoll,
  psOpRound,
  psOpSin,
  psOpSqrt,
  psOpSub,
  psOpTrue,
  psOpTruncate,
  psOpXor,
  psOpIf,
  psOpIfelse,
  psOpReturn
};

char *psOpNames[] = {
  "abs",
  "add",
  "and",
  "atan",
  "bitshift",
  "ceiling",
  "copy",
  "cos",
  "cvi",
  "cvr",
  "div",
  "dup",
  "eq",
  "exch",
  "exp",
  "false",
  "floor",
  "ge",
  "gt",
  "idiv",
  "index",
  "le",
  "ln",
  "log",
  "lt",
  "mod",
  "mul",
  "ne",
  "neg",
  "not",
  "or",
  "pop",
  "roll",
  "round",
  "sin",
  "sqrt",
  "sub",
  "true",
  "truncate",
  "xor"
};

#define nPSOps (sizeof(psOpNames) / sizeof(char *))

enum PSObjectType 
{
    psBool,
    psInt,
    psReal,
    psOperator,
    psBlock
};

struct PSObject 
{
    PSObjectType type;
    union 
    {
        bool booln;		// boolean (stack only)
        int intg;			// integer (stack and code)
        double real;		// real (stack and code)
        PSOp op;			// operator (code only)
        int blk;			// if/ifelse block pointer (code only)
    };
};

#define psStackSize 100


class PSStack 
{
public:
    PSStack() { sp = psStackSize; }
    
    void copy(int n);
    
    bool empty() { return sp == psStackSize; }
    
    void index(int i);
    
    void roll(int n, int j);
    
    void pop();
    bool popBool();
    int popInt();
    double popNum();
    
    void pushBool(bool booln);
    void pushInt(int intg);
    void pushReal(double real);
    
    bool topIsInt() { return sp < psStackSize && stack[sp].type == psInt; }
    bool topIsReal() { return sp < psStackSize && stack[sp].type == psReal; }
    bool topTwoAreInts() { return sp < psStackSize - 1 && stack[sp].type == psInt && stack[sp+1].type == psInt; }
    bool topTwoAreNums() { return sp < psStackSize - 1 && (stack[sp].type == psInt || stack[sp].type == psReal) && (stack[sp+1].type == psInt || stack[sp+1].type == psReal); }

private:
    bool checkOverflow(int n = 1);
    bool checkType(PSObjectType t1, PSObjectType t2);
    bool checkUnderflow();

private:
    PSObject stack[psStackSize];
    int sp;
};


void PSStack::copy(int n) 
{
    if (sp + n > psStackSize) 
        return;
    
    if (!checkOverflow(n)) 
        return;
        
    for (int i = sp + n - 1; i >= sp; --i) 
        stack[i - n] = stack[i];
        
    sp -= n;
}

void PSStack::index(int i) 
{
    if (!checkOverflow()) 
        return;
        
    --sp;
    stack[sp] = stack[sp + 1 + i];
}

void PSStack::pop() 
{
    if (!checkUnderflow()) 
        return;
        
    ++sp;
}

bool PSStack::popBool() 
{
    if (checkUnderflow() && checkType(psBool, psBool)) 
        return stack[sp++].booln;
        
    return false;
}

int PSStack::popInt() 
{
    if (checkUnderflow() && checkType(psInt, psInt)) 
        return stack[sp++].intg;
        
    return 0;
}

double PSStack::popNum() 
{
    if (checkUnderflow() && checkType(psInt, psReal)) 
    {
        double ret = (stack[sp].type == psInt) ? (double)stack[sp].intg : stack[sp].real;
        ++sp;
        return ret;
    }
    return 0;
}

void PSStack::pushBool(bool booln) 
{
    if (checkOverflow()) 
    {
        stack[--sp].type = psBool;
        stack[sp].booln = booln;
    }
}

void PSStack::pushInt(int intg) 
{
    if (checkOverflow()) 
    {
        stack[--sp].type = psInt;
        stack[sp].intg = intg;
    }
}

void PSStack::pushReal(double real) 
{
    if (checkOverflow()) 
    {
        stack[--sp].type = psReal;
        stack[sp].real = real;
    }
}

void PSStack::roll(int n, int j) 
{
    if (j >= 0) 
        j %= n;
    else 
    {
        j = -j % n;
        if (j != 0) 
        {
            j = n - j;
        }
    }
    
    if (n <= 0 || j == 0) 
        return;
        
    for (int i = 0; i < j; ++i) 
    {
        PSObject obj = stack[sp];
        for (int k = sp; k < sp + n - 1; ++k) 
            stack[k] = stack[k+1];
            
        stack[sp + n - 1] = obj;
    }
}

bool PSStack::checkOverflow(int n) 
{
    if (sp - n < 0) 
        return false;
    return true;
}

bool PSStack::checkType(PSObjectType t1, PSObjectType t2) 
{
    if (stack[sp].type != t1 && stack[sp].type != t2) 
        return false;
    return true;
}

bool PSStack::checkUnderflow() 
{
    if (sp == psStackSize) 
        return false;
    return true;
}

XWPostScriptFunc::XWPostScriptFunc(XWObject *funcObj, XWDict *dict)
{
  XWStream *str;
  int codePtr;
  XWString *tok;
  double in[FUNC_MAX_INPUTS];
  int i;

  codeString = NULL;
  code = NULL;
  codeSize = 0;
  ok = false;

  //----- initialize the generic stuff
  if (!init(dict)) {
    goto err1;
  }
  if (!hasRange) {
    xwApp->error("Type 4 function is missing range");
    goto err1;
  }

  //----- get the stream
  if (!funcObj->isStream()) {
    xwApp->error("Type 4 function isn't a stream");
    goto err1;
  }
  str = funcObj->getStream();

  //----- parse the function
  codeString = new XWString();
  str->reset();
  if (!(tok = getToken(str)) || tok->cmp("{")) {
    xwApp->error("Expected '{' at start of PostScript function");
    if (tok) {
      delete tok;
    }
    goto err1;
  }
  delete tok;
  codePtr = 0;
  if (!parseCode(str, &codePtr)) {
    goto err2;
  }
  str->close();

  //----- set up the cache
  for (i = 0; i < m; ++i) {
    in[i] = domain[i][0];
    cacheIn[i] = in[i] - 1;
  }
  transform(in, cacheOut);

  ok = true;

 err2:
  str->close();
 err1:
  return;
}

XWPostScriptFunc::XWPostScriptFunc(XWPostScriptFunc *func)
{
    memcpy(this, func, sizeof(XWPostScriptFunc));
    code = (PSObject *)malloc(codeSize * sizeof(PSObject));
    memcpy(code, func->code, codeSize * sizeof(PSObject));
    codeString = func->codeString->copy();
}

XWPostScriptFunc::~XWPostScriptFunc()
{
    if (code)
        free(code);
        
    if (codeString)
        delete codeString;
}

void XWPostScriptFunc::transform(double *in, double *out)
{
    int i;

  // check the cache
  for (i = 0; i < m; ++i) 
  {
    if (in[i] != cacheIn[i]) 
    {
      break;
    }
  }
  if (i == m) 
  {
    for (i = 0; i < n; ++i) 
    {
      out[i] = cacheOut[i];
    }
    return;
  }

  PSStack * stack = new PSStack();
  for (i = 0; i < m; ++i) 
  {
    //~ may need to check for integers here
    stack->pushReal(in[i]);
  }
  exec(stack, 0);
  for (i = n - 1; i >= 0; --i) 
  {
    out[i] = stack->popNum();
    if (out[i] < range[i][0]) 
    {
      out[i] = range[i][0];
    } 
    else if (out[i] > range[i][1]) 
    {
      out[i] = range[i][1];
    }
  }
  // if (!stack->empty()) {
  //   error(errSyntaxWarning, -1,
  //         "Extra values on stack at end of PostScript function");
  // }
  delete stack;

  // save current result in the cache
  for (i = 0; i < m; ++i) 
  {
    cacheIn[i] = in[i];
  }
  for (i = 0; i < n; ++i) 
  {
    cacheOut[i] = out[i];
  }
}

void XWPostScriptFunc::exec(PSStack *stack, int codePtr)
{
    int i1 = 0;
    int i2 = 0;
    double r1 = 0.0;
    double r2 = 0.0;
    bool b1 = false;
    bool b2 = false;
    
    while (1)
    {
        switch (code[codePtr].type)
        {
            case psInt:
                stack->pushInt(code[codePtr++].intg);
                break;
                
            case psReal:
                stack->pushReal(code[codePtr++].real);
                break;
                
            case psOperator:
                switch (code[codePtr++].op) 
                {
                    case psOpAbs:
	                    if (stack->topIsInt()) 
	                        stack->pushInt(abs(stack->popInt()));
	                    else 
	                        stack->pushReal(fabs(stack->popNum()));
	                    break;
	                    
                    case psOpAdd:
	                    if (stack->topTwoAreInts()) 
	                    {
	                        i2 = stack->popInt();
	                        i1 = stack->popInt();
	                        stack->pushInt(i1 + i2);
	                    } 
	                    else 
	                    {
	                        r2 = stack->popNum();
	                        r1 = stack->popNum();
	                        stack->pushReal(r1 + r2);
	                    }
	                    break;
	                    
                    case psOpAnd:
	                    if (stack->topTwoAreInts()) 
	                    {
	                        i2 = stack->popInt();
	                        i1 = stack->popInt();
	                        stack->pushInt(i1 & i2);
	                    } 
	                    else 
	                    {
	                        b2 = stack->popBool();
	                        b1 = stack->popBool();
	                        stack->pushBool(b1 && b2);
	                    }
	                    break;
	                    
                    case psOpAtan:
	                    r2 = stack->popNum();
	                    r1 = stack->popNum();
	                    stack->pushReal(atan2(r1, r2));
	                    break;
	                    
                    case psOpBitshift:
	                    i2 = stack->popInt();
	                    i1 = stack->popInt();
	                    if (i2 > 0) 
	                        stack->pushInt(i1 << i2);
	                    else if (i2 < 0) 
	                        stack->pushInt((int)((uint)i1 >> i2));
	                    else 
	                        stack->pushInt(i1);
	                    break;
	                    
                    case psOpCeiling:
	                    if (!stack->topIsInt()) 
	                        stack->pushReal(ceil(stack->popNum()));
	                    break;
	                    
                    case psOpCopy:
	                    stack->copy(stack->popInt());
	                    break;
	                    
                    case psOpCos:
	                    stack->pushReal(cos(stack->popNum()));
	                    break;
	                    
                    case psOpCvi:
	                    if (!stack->topIsInt()) 
	                        stack->pushInt((int)stack->popNum());
	                    break;
	                    
                    case psOpCvr:
	                    if (!stack->topIsReal()) 
	                        stack->pushReal(stack->popNum());
	                    break;
	                    
                    case psOpDiv:
	                    r2 = stack->popNum();
	                    r1 = stack->popNum();
	                    stack->pushReal(r1 / r2);
	                    break;
	                    
                    case psOpDup:
	                    stack->copy(1);
	                    break;
	                    
                    case psOpEq:
	                    if (stack->topTwoAreInts()) 
	                    {
	                        i2 = stack->popInt();
	                        i1 = stack->popInt();
	                        stack->pushBool(i1 == i2);
	                    } 
	                    else if (stack->topTwoAreNums()) 
	                    {
	                        r2 = stack->popNum();
	                        r1 = stack->popNum();
	                        stack->pushBool(r1 == r2);
	                    } 
	                    else 
	                    {
	                        b2 = stack->popBool();
	                        b1 = stack->popBool();
	                        stack->pushBool(b1 == b2);
	                    }
	                    break;
	                    
                    case psOpExch:
	                    stack->roll(2, 1);
	                    break;
	                    
                    case psOpExp:
	                    r2 = stack->popNum();
	                    r1 = stack->popNum();
	                    stack->pushReal(pow(r1, r2));
	                    break;
	                    
                    case psOpFalse:   
	                    stack->pushBool(false);
	                    break;
	                    
                    case psOpFloor:
	                    if (!stack->topIsInt()) 
	                        stack->pushReal(floor(stack->popNum()));
	                    break;
	                    
                    case psOpGe:
	                    if (stack->topTwoAreInts()) 
	                    {
	                        i2 = stack->popInt();
	                        i1 = stack->popInt();
	                        stack->pushBool(i1 >= i2);
	                    } 
	                    else 
	                    {
	                        r2 = stack->popNum();
	                        r1 = stack->popNum();
	                        stack->pushBool(r1 >= r2);
	                    }
	                    break;
	                    
                    case psOpGt:
	                    if (stack->topTwoAreInts()) 
	                    {
	                        i2 = stack->popInt();
	                        i1 = stack->popInt();
	                        stack->pushBool(i1 > i2);
	                    } 
	                    else 
	                    {
	                        r2 = stack->popNum();
	                        r1 = stack->popNum();
	                        stack->pushBool(r1 > r2);
	                    }
	                    break;
	                    
                    case psOpIdiv:
	                    i2 = stack->popInt();
	                    i1 = stack->popInt();
	                    stack->pushInt(i1 / i2);
	                    break;
	                    
                    case psOpIndex:
	                    stack->index(stack->popInt());
	                    break;
	                    
                    case psOpLe:
	                    if (stack->topTwoAreInts()) 
	                    {
	                        i2 = stack->popInt();
	                        i1 = stack->popInt();
	                        stack->pushBool(i1 <= i2);
	                    } 
	                    else 
	                    {
	                        r2 = stack->popNum();
	                        r1 = stack->popNum();
	                        stack->pushBool(r1 <= r2);
	                    }
	                    break;
	                    
                    case psOpLn:
	                    stack->pushReal(log(stack->popNum()));
	                    break;
	                    
                    case psOpLog:
	                    stack->pushReal(log10(stack->popNum()));
	                    break;
	                    
                    case psOpLt:
	                    if (stack->topTwoAreInts()) 
	                    {
	                        i2 = stack->popInt();
	                        i1 = stack->popInt();
	                        stack->pushBool(i1 < i2);
	                    } 
	                    else 
	                    {
	                        r2 = stack->popNum();
	                        r1 = stack->popNum();
	                        stack->pushBool(r1 < r2);
	                    }
	                    break;
	                    
                    case psOpMod:
	                    i2 = stack->popInt();
	                    i1 = stack->popInt();
	                    stack->pushInt(i1 % i2);
	                    break;
	                    
                    case psOpMul: 
	                    if (stack->topTwoAreInts()) 
	                    {
	                        i2 = stack->popInt();
	                        i1 = stack->popInt();
	                        stack->pushInt(i1 * i2);
	                    } 
	                    else 
	                    {
	                        r2 = stack->popNum();
	                        r1 = stack->popNum();
	                        stack->pushReal(r1 * r2);
	                    }
	                    break;
	                    
                    case psOpNe:
	                    if (stack->topTwoAreInts()) 
	                    {
	                        i2 = stack->popInt();
	                        i1 = stack->popInt();
	                        stack->pushBool(i1 != i2);
	                    } 
	                    else if (stack->topTwoAreNums()) 
	                    {
	                        r2 = stack->popNum();
	                        r1 = stack->popNum();
	                        stack->pushBool(r1 != r2);
	                    } 
	                    else 
	                    {
	                        b2 = stack->popBool();
	                        b1 = stack->popBool();
	                        stack->pushBool(b1 != b2);
	                    }
	                    break;
	                    
                    case psOpNeg:
	                    if (stack->topIsInt()) 
	                        stack->pushInt(-stack->popInt());
	                    else 
	                        stack->pushReal(-stack->popNum());
	                    break;
	                    
                    case psOpNot:
	                    if (stack->topIsInt()) 
	                        stack->pushInt(~stack->popInt());
	                    else 
	                        stack->pushBool(!stack->popBool());
	                    break;
	                    
                    case psOpOr:
	                    if (stack->topTwoAreInts()) 
	                    {
	                        i2 = stack->popInt();
	                        i1 = stack->popInt();
	                        stack->pushInt(i1 | i2);
	                    } 
	                    else 
	                    {
	                        b2 = stack->popBool();
	                        b1 = stack->popBool();
	                        stack->pushBool(b1 || b2);
	                    }
	                    break;
	                    
                    case psOpPop:
	                    stack->pop();
	                    break;
	                    
                    case psOpRoll:
	                    i2 = stack->popInt();
	                    i1 = stack->popInt();
	                    stack->roll(i1, i2);
	                    break;
	                    
                    case psOpRound:
	                    if (!stack->topIsInt()) 
	                    {
	                        r1 = stack->popNum();
	                        stack->pushReal((r1 >= 0) ? floor(r1 + 0.5) : ceil(r1 - 0.5));
	                    }
	                    break;
	                    
                    case psOpSin:
	                    stack->pushReal(sin(stack->popNum()));
	                    break;
	                    
                    case psOpSqrt:
	                    stack->pushReal(sqrt(stack->popNum()));
	                    break;
	                    
                    case psOpSub:
	                    if (stack->topTwoAreInts()) 
	                    {
	                        i2 = stack->popInt();
	                        i1 = stack->popInt();
	                        stack->pushInt(i1 - i2);
	                    } 
	                    else 
	                    {
	                        r2 = stack->popNum();
	                        r1 = stack->popNum();
	                        stack->pushReal(r1 - r2);
	                    }
	                    break;
	                    
                    case psOpTrue:
	                    stack->pushBool(true);
	                    break;
	                    
                    case psOpTruncate:
	                    if (!stack->topIsInt()) 
	                    {
	                        r1 = stack->popNum();
	                        stack->pushReal((r1 >= 0) ? floor(r1) : ceil(r1));
	                    }
	                    break;
	                    
                    case psOpXor:
	                    if (stack->topTwoAreInts()) 
	                    {
	                        i2 = stack->popInt();
	                        i1 = stack->popInt();
	                        stack->pushInt(i1 ^ i2);
	                    } 
	                    else 
	                    {
	                        b2 = stack->popBool();
	                        b1 = stack->popBool();
	                        stack->pushBool(b1 ^ b2);
	                    }
	                    break;
	                    
                    case psOpIf:
	                    b1 = stack->popBool();
	                    if (b1) 
	                        exec(stack, codePtr + 2);
	                    codePtr = code[codePtr + 1].blk;
	                    break;
	                    
                    case psOpIfelse:
	                    b1 = stack->popBool();
	                    if (b1) 
	                        exec(stack, codePtr + 2);
	                    else 
	                        exec(stack, code[codePtr].blk);
	                    codePtr = code[codePtr + 1].blk;
	                    break;
	                
                    case psOpReturn:
	                    return;
                }
                break;
                
            default:
                break;
        }
    }
}

XWString * XWPostScriptFunc::getToken(XWStream *str)
{
    XWString * s = new XWString;
    bool comment = false;
    int c = 0;
    while (1)
    {
        if ((c = str->getChar()) == EOF)
            break;
            
        codeString->append(c);
        if (comment)
        {
            if (c == '\x0a' || c == '\x0d')
                comment = false;
        }
        else if (c == '%')
            comment = true;
        else if (!isspace(c))
            break;
    }
    
    if (c == '{' || c == '}')
        s->append((char)c);
    else if (isdigit(c) || c == '.' || c == '-')
    {
        while (1)
        {
            s->append((char)c);
            c = str->lookChar();
            if (c == EOF || !(isdigit(c) || c == '.' || c == '-'))
                break;
                
            str->getChar();
            codeString->append(c);
        }
    }
    else
    {
        while (1)
        {
            s->append((char)c);
            c = str->lookChar();
            if (c == EOF || !isalnum(c)) 
	            break;
	            
            str->getChar();
            codeString->append(c);
        }
    }
    
    return s;
}

bool XWPostScriptFunc::parseCode(XWStream *str, int *codePtr)
{
  XWString *tok;
  char *p;
  bool isReal;
  int opPtr, elsePtr;
  int a, b, mid, cmp;

  while (1) 
  {
    if (!(tok = getToken(str))) 
    {
      xwApp->error("Unexpected end of PostScript function stream");
      return false;
    }
    p = tok->getCString();
    if (isdigit(*p) || *p == '.' || *p == '-') 
    {
      isReal = false;
      for (; *p; ++p) 
      {
				if (*p == '.') 
				{
	  			isReal = true;
	  			break;
				}
      }
      resizeCode(*codePtr);
      if (isReal) 
      {
				code[*codePtr].type = psReal;
				code[*codePtr].real = atof(tok->getCString());
      } 
      else 
      {
				code[*codePtr].type = psInt;
				code[*codePtr].intg = atoi(tok->getCString());
      }
      ++*codePtr;
      delete tok;
    } 
    else if (!tok->cmp("{")) 
    {
      delete tok;
      opPtr = *codePtr;
      *codePtr += 3;
      resizeCode(opPtr + 2);
      if (!parseCode(str, codePtr)) 
      {
				return false;
      }
      if (!(tok = getToken(str))) 
      {
				xwApp->error("Unexpected end of PostScript function stream");
				return false;
      }
      if (!tok->cmp("{")) 
      {
				elsePtr = *codePtr;
				if (!parseCode(str, codePtr)) 
				{
	  			return false;
				}
				delete tok;
				if (!(tok = getToken(str))) 
				{
	  			xwApp->error("Unexpected end of PostScript function stream");
	  			return false;
				}
      } 
      else 
      {
				elsePtr = -1;
      }
      if (!tok->cmp("if")) 
      {
				if (elsePtr >= 0) 
				{
	  			xwApp->error("Got 'if' operator with two blocks in PostScript function");
	  			return false;
				}
				code[opPtr].type = psOperator;
				code[opPtr].op = psOpIf;
				code[opPtr+2].type = psBlock;
				code[opPtr+2].blk = *codePtr;
      } 
      else if (!tok->cmp("ifelse")) 
      {
				if (elsePtr < 0) 
				{
	  			xwApp->error("Got 'ifelse' operator with one block in PostScript function");
	  			return false;
				}
				code[opPtr].type = psOperator;
				code[opPtr].op = psOpIfelse;
				code[opPtr+1].type = psBlock;
				code[opPtr+1].blk = elsePtr;
				code[opPtr+2].type = psBlock;
				code[opPtr+2].blk = *codePtr;
      } 
      else 
      {
				xwApp->error("Expected if/ifelse operator in PostScript function");
				delete tok;
				return false;
      }
      delete tok;
    } 
    else if (!tok->cmp("}")) 
    {
      delete tok;
      resizeCode(*codePtr);
      code[*codePtr].type = psOperator;
      code[*codePtr].op = psOpReturn;
      ++*codePtr;
      break;
    } 
    else 
    {
      a = -1;
      b = nPSOps;
      cmp = 0; // make gcc happy
      // invariant: psOpNames[a] < tok < psOpNames[b]
      while (b - a > 1) 
      {
				mid = (a + b) / 2;
				cmp = tok->cmp(psOpNames[mid]);
				if (cmp > 0) 
				{
	  			a = mid;
				} 
				else if (cmp < 0) 
				{
	  			b = mid;
				} 
				else 
				{
	  			a = b = mid;
				}
      }
      if (cmp != 0) 
      {
				delete tok;
				return false;
      }
      delete tok;
      resizeCode(*codePtr);
      code[*codePtr].type = psOperator;
      code[*codePtr].op = (PSOp)a;
      ++*codePtr;
    }
  }
  return true;
}

void XWPostScriptFunc::resizeCode(int newSize)
{
    if (newSize >= codeSize) 
    {
        codeSize += 64;
        code = (PSObject *)realloc(code, codeSize * sizeof(PSObject));
    }
}

