/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stddef.h>
#include <math.h>
#include <string.h>
#include "XWApplication.h"
#include "XWObject.h"
#include "XWArray.h"
#include "XWPattern.h"

XWPattern::XWPattern(int typeA)
    :type(typeA)
{
}

XWPattern::~XWPattern()
{
}

XWPattern * XWPattern::parse(XWObject *obj)
{
  XWPattern *pattern;
  XWObject obj1;

  if (obj->isDict()) {
    obj->dictLookup("PatternType", &obj1);
  } else if (obj->isStream()) {
    obj->streamGetDict()->lookup("PatternType", &obj1);
  } else {
    return NULL;
  }
  pattern = NULL;
  if (obj1.isInt() && obj1.getInt() == 1) {
    pattern = XWTilingPattern::parse(obj);
  } else if (obj1.isInt() && obj1.getInt() == 2) {
    pattern = XWShadingPattern::parse(obj);
  }
  obj1.free();
  return pattern;
}

XWTilingPattern::XWTilingPattern(int paintTypeA, 
                                 int tilingTypeA,
		                         double *bboxA, 
		                         double xStepA, 
		                         double yStepA,
		                         XWObject *resDictA, 
		                         double *matrixA,
		                         XWObject *contentStreamA)
	:XWPattern(1)
{
  int i;

  paintType = paintTypeA;
  tilingType = tilingTypeA;
  for (i = 0; i < 4; ++i) {
    bbox[i] = bboxA[i];
  }
  xStep = xStepA;
  yStep = yStepA;
  resDictA->copy(&resDict);
  for (i = 0; i < 6; ++i) {
    matrix[i] = matrixA[i];
  }
  contentStreamA->copy(&contentStream);
}

XWTilingPattern::~XWTilingPattern()
{
    resDict.free();
    contentStream.free();
}

XWPattern * XWTilingPattern::copy()
{
    return new XWTilingPattern(paintType, tilingType, bbox, xStep, yStep, &resDict, matrix, &contentStream);
}

XWTilingPattern * XWTilingPattern::parse(XWObject *patObj)
{
  XWTilingPattern *pat;
  XWDict *dict;
  int paintTypeA, tilingTypeA;
  double bboxA[4], matrixA[6];
  double xStepA, yStepA;
  XWObject resDictA;
  XWObject obj1, obj2;
  int i;

  if (!patObj->isStream()) {
    return NULL;
  }
  dict = patObj->streamGetDict();

  if (dict->lookup("PaintType", &obj1)->isInt()) {
    paintTypeA = obj1.getInt();
  } else {
    paintTypeA = 1;
    xwApp->error("Invalid or missing PaintType in pattern");
  }
  obj1.free();
  if (dict->lookup("TilingType", &obj1)->isInt()) {
    tilingTypeA = obj1.getInt();
  } else {
    tilingTypeA = 1;
    xwApp->error("Invalid or missing TilingType in pattern");
  }
  obj1.free();
  bboxA[0] = bboxA[1] = 0;
  bboxA[2] = bboxA[3] = 1;
  if (dict->lookup("BBox", &obj1)->isArray() &&
      obj1.arrayGetLength() == 4) {
    for (i = 0; i < 4; ++i) {
      if (obj1.arrayGet(i, &obj2)->isNum()) {
	bboxA[i] = obj2.getNum();
      }
      obj2.free();
    }
  } else {
    xwApp->error("Invalid or missing BBox in pattern");
  }
  obj1.free();
  if (dict->lookup("XStep", &obj1)->isNum()) {
    xStepA = obj1.getNum();
  } else {
    xStepA = 1;
    xwApp->error("Invalid or missing XStep in pattern");
  }
  obj1.free();
  if (dict->lookup("YStep", &obj1)->isNum()) {
    yStepA = obj1.getNum();
  } else {
    yStepA = 1;
    xwApp->error("Invalid or missing YStep in pattern");
  }
  obj1.free();
  if (!dict->lookup("Resources", &resDictA)->isDict()) {
    resDictA.free();
    resDictA.initNull();
    xwApp->error("Invalid or missing Resources in pattern");
  }
  matrixA[0] = 1; matrixA[1] = 0;
  matrixA[2] = 0; matrixA[3] = 1;
  matrixA[4] = 0; matrixA[5] = 0;
  if (dict->lookup("Matrix", &obj1)->isArray() &&
      obj1.arrayGetLength() == 6) {
    for (i = 0; i < 6; ++i) {
      if (obj1.arrayGet(i, &obj2)->isNum()) {
	matrixA[i] = obj2.getNum();
      }
      obj2.free();
    }
  }
  obj1.free();

  pat = new XWTilingPattern(paintTypeA, tilingTypeA, bboxA, xStepA, yStepA,
			     &resDictA, matrixA, patObj);
  resDictA.free();
  return pat;
}

XWShadingPattern::XWShadingPattern(XWShading *shadingA, double *matrixA)
	:XWPattern(2)
{
    shading = shadingA;
    for (int i = 0; i < 6; ++i) 
        matrix[i] = matrixA[i];
}

XWShadingPattern::~XWShadingPattern()
{
    if (shading)
        delete shading;
}

XWPattern *XWShadingPattern::copy()
{
    return new XWShadingPattern(shading->copy(), matrix);
}

XWShadingPattern * XWShadingPattern::parse(XWObject *patObj)
{
  XWDict *dict;
  XWShading *shadingA;
  double matrixA[6];
  XWObject obj1, obj2;
  int i;

  if (!patObj->isDict()) {
    return NULL;
  }
  dict = patObj->getDict();

  dict->lookup("Shading", &obj1);
  shadingA = XWShading::parse(&obj1);
  obj1.free();
  if (!shadingA) {
    return NULL;
  }

  matrixA[0] = 1; matrixA[1] = 0;
  matrixA[2] = 0; matrixA[3] = 1;
  matrixA[4] = 0; matrixA[5] = 0;
  if (dict->lookup("Matrix", &obj1)->isArray() &&
      obj1.arrayGetLength() == 6) {
    for (i = 0; i < 6; ++i) {
      if (obj1.arrayGet(i, &obj2)->isNum()) {
	matrixA[i] = obj2.getNum();
      }
      obj2.free();
    }
  }
  obj1.free();

  return new XWShadingPattern(shadingA, matrixA);
}

XWShading::XWShading(int typeA)
{
    type = typeA;
    colorSpace = 0;
}

XWShading::XWShading(XWShading *shading)
{
    type = shading->type;
    colorSpace = shading->colorSpace->copy();
    for (int i = 0; i < COLOR_MAX_COMPS; ++i) 
        background.c[i] = shading->background.c[i];
        
    hasBackground = shading->hasBackground;
    xMin = shading->xMin;
    yMin = shading->yMin;
    xMax = shading->xMax;
    yMax = shading->yMax;
    hasBBox = shading->hasBBox;
}

XWShading::~XWShading()
{
    if (colorSpace) 
        delete colorSpace;
}

XWShading * XWShading::parse(XWObject *obj)
{
  XWShading *shading;
  XWDict *dict;
  int typeA;
  XWObject obj1;

  if (obj->isDict()) {
    dict = obj->getDict();
  } else if (obj->isStream()) {
    dict = obj->streamGetDict();
  } else {
    return NULL;
  }

  if (!dict->lookup("ShadingType", &obj1)->isInt()) {
    xwApp->error("Invalid ShadingType in shading dictionary");
    obj1.free();
    return NULL;
  }
  typeA = obj1.getInt();
  obj1.free();

  switch (typeA) {
  case 1:
    shading = XWFunctionShading::parse(dict);
    break;
  case 2:
    shading = XWAxialShading::parse(dict);
    break;
  case 3:
    shading = XWRadialShading::parse(dict);
    break;
  case 4:
    if (obj->isStream()) {
      shading = XWGouraudTriangleShading::parse(4, dict, obj->getStream());
    } else {
      xwApp->error("Invalid Type 4 shading object");
      goto err1;
    }
    break;
  case 5:
    if (obj->isStream()) {
      shading = XWGouraudTriangleShading::parse(5, dict, obj->getStream());
    } else {
      xwApp->error("Invalid Type 5 shading object");
      goto err1;
    }
    break;
  case 6:
    if (obj->isStream()) {
      shading = XWPatchMeshShading::parse(6, dict, obj->getStream());
    } else {
      xwApp->error("Invalid Type 6 shading object");
      goto err1;
    }
    break;
  case 7:
    if (obj->isStream()) {
      shading = XWPatchMeshShading::parse(7, dict, obj->getStream());
    } else {
      xwApp->error("Invalid Type 7 shading object");
      goto err1;
    }
    break;
  default:
    goto err1;
  }

  return shading;

 err1:
  return NULL;
}

bool XWShading::init(XWDict *dict)
{
  XWObject obj1, obj2;
  int i;

  dict->lookup("ColorSpace", &obj1);
  if (!(colorSpace = XWColorSpace::parse(&obj1))) {
    xwApp->error("Bad color space in shading dictionary");
    obj1.free();
    return false;
  }
  obj1.free();

  for (i = 0; i < COLOR_MAX_COMPS; ++i) {
    background.c[i] = 0;
  }
  hasBackground = false;
  if (dict->lookup("Background", &obj1)->isArray()) {
    if (obj1.arrayGetLength() == colorSpace->getNComps()) {
      hasBackground = true;
      for (i = 0; i < colorSpace->getNComps(); ++i) {
	background.c[i] = dblToCol(obj1.arrayGet(i, &obj2)->getNum());
	obj2.free();
      }
    } else {
      xwApp->error("Bad Background in shading dictionary");
    }
  }
  obj1.free();

  xMin = yMin = xMax = yMax = 0;
  hasBBox = false;
  if (dict->lookup("BBox", &obj1)->isArray()) {
    if (obj1.arrayGetLength() == 4) {
      hasBBox = true;
      xMin = obj1.arrayGet(0, &obj2)->getNum();
      obj2.free();
      yMin = obj1.arrayGet(1, &obj2)->getNum();
      obj2.free();
      xMax = obj1.arrayGet(2, &obj2)->getNum();
      obj2.free();
      yMax = obj1.arrayGet(3, &obj2)->getNum();
      obj2.free();
    } else {
      xwApp->error("Bad BBox in shading dictionary");
    }
  }
  obj1.free();

  return true;
}


XWFunctionShading::XWFunctionShading(double x0A, 
                                     double y0A,
		                             double x1A, 
		                             double y1A,
		                             double *matrixA,
		                             XWFunction **funcsA, 
		                             int nFuncsA)
    :XWShading(1)
{
    x0 = x0A;
    y0 = y0A;
    x1 = x1A;
    y1 = y1A;
    for (int i = 0; i < 6; ++i) 
        matrix[i] = matrixA[i];
        
    nFuncs = nFuncsA;
    for (int i = 0; i < nFuncs; ++i) 
        funcs[i] = funcsA[i];
}

XWFunctionShading::XWFunctionShading(XWFunctionShading *shading)
    :XWShading(shading)
{
    x0 = shading->x0;
    y0 = shading->y0;
    x1 = shading->x1;
    y1 = shading->y1;
    for (int i = 0; i < 6; ++i) 
        matrix[i] = shading->matrix[i];
        
    nFuncs = shading->nFuncs;
    for (int i = 0; i < nFuncs; ++i) 
        funcs[i] = shading->funcs[i]->copy();
}

XWFunctionShading::~XWFunctionShading()
{
    for (int i = 0; i < nFuncs; ++i) 
    {
        if (funcs[i])
            delete funcs[i];
    }
}

XWShading * XWFunctionShading::copy()
{
    return new XWFunctionShading(this);
}

void XWFunctionShading::getColor(double x, double y, CoreColor *color)
{
    double in[2], out[COLOR_MAX_COMPS];
    for (int i = 0; i < COLOR_MAX_COMPS; ++i)
        out[i] = 0;
        
    in[0] = x;
    in[1] = y;
    for (int i = 0; i < nFuncs; ++i)
    {
        if (funcs[i])
            funcs[i]->transform(in, &out[i]);
    }
        
    for (int i = 0; i < COLOR_MAX_COMPS; ++i)
        color->c[i] = dblToCol(out[i]);
}

XWFunctionShading *XWFunctionShading::parse(XWDict *dict)
{
  XWFunctionShading *shading;
  double x0A, y0A, x1A, y1A;
  double matrixA[6];
  XWFunction *funcsA[COLOR_MAX_COMPS];
  int nFuncsA;
  XWObject obj1, obj2;
  int i;

  x0A = y0A = 0;
  x1A = y1A = 1;
  if (dict->lookup("Domain", &obj1)->isArray() &&
      obj1.arrayGetLength() == 4) {
    x0A = obj1.arrayGet(0, &obj2)->getNum();
    obj2.free();
    y0A = obj1.arrayGet(1, &obj2)->getNum();
    obj2.free();
    x1A = obj1.arrayGet(2, &obj2)->getNum();
    obj2.free();
    y1A = obj1.arrayGet(3, &obj2)->getNum();
    obj2.free();
  }
  obj1.free();

  matrixA[0] = 1; matrixA[1] = 0;
  matrixA[2] = 0; matrixA[3] = 1;
  matrixA[4] = 0; matrixA[5] = 0;
  if (dict->lookup("Matrix", &obj1)->isArray() &&
      obj1.arrayGetLength() == 6) {
    matrixA[0] = obj1.arrayGet(0, &obj2)->getNum();
    obj2.free();
    matrixA[1] = obj1.arrayGet(1, &obj2)->getNum();
    obj2.free();
    matrixA[2] = obj1.arrayGet(2, &obj2)->getNum();
    obj2.free();
    matrixA[3] = obj1.arrayGet(3, &obj2)->getNum();
    obj2.free();
    matrixA[4] = obj1.arrayGet(4, &obj2)->getNum();
    obj2.free();
    matrixA[5] = obj1.arrayGet(5, &obj2)->getNum();
    obj2.free();
  }
  obj1.free();

  dict->lookup("Function", &obj1);
  if (obj1.isArray()) {
    nFuncsA = obj1.arrayGetLength();
    if (nFuncsA > COLOR_MAX_COMPS) {
      xwApp->error("Invalid Function array in shading dictionary");
      goto err1;
    }
    for (i = 0; i < nFuncsA; ++i) {
      obj1.arrayGet(i, &obj2);
      if (!(funcsA[i] = XWFunction::parse(&obj2))) {
	goto err2;
      }
      obj2.free();
    }
  } else {
    nFuncsA = 1;
    if (!(funcsA[0] = XWFunction::parse(&obj1))) {
      goto err1;
    }
  }
  obj1.free();

  shading = new XWFunctionShading(x0A, y0A, x1A, y1A, matrixA,
				   funcsA, nFuncsA);
  if (!shading->init(dict)) {
    delete shading;
    return NULL;
  }
  return shading;

 err2:
  obj2.free();
 err1:
  obj1.free();
  return NULL;
}

XWAxialShading::XWAxialShading(double x0A, 
                               double y0A,
		                       double x1A, 
		                       double y1A,
		                       double t0A, 
		                       double t1A,
		                       XWFunction **funcsA, 
		                       int nFuncsA,
		                       bool extend0A, 
		                       bool extend1A)
    :XWShading(2)
{
    x0 = x0A;
    y0 = y0A;
    x1 = x1A;
    y1 = y1A;
    t0 = t0A;
    t1 = t1A;
    nFuncs = nFuncsA;
    for (int i = 0; i < nFuncs; ++i) 
        funcs[i] = funcsA[i];
        
    extend0 = extend0A;
    extend1 = extend1A;
}

XWAxialShading::XWAxialShading(XWAxialShading *shading)
    :XWShading(shading)
{
    x0 = shading->x0;
    y0 = shading->y0;
    x1 = shading->x1;
    y1 = shading->y1;
    t0 = shading->t0;
    y1 = shading->t1;
    nFuncs = shading->nFuncs;
    for (int i = 0; i < nFuncs; ++i) 
    	funcs[i] = shading->funcs[i]->copy();
        
    extend0 = shading->extend0;
    extend1 = shading->extend1;
}

XWAxialShading::~XWAxialShading()
{
    for (int i = 0; i < nFuncs; ++i)
    {
        if (funcs[i])
            delete funcs[i];
    }
}

XWShading *XWAxialShading::copy()
{
    return new XWAxialShading(this);
}

void  XWAxialShading::getColor(double t, CoreColor *color)
{
    double out[COLOR_MAX_COMPS];
    for (int i = 0; i < COLOR_MAX_COMPS; ++i)
        out[i] = 0;
        
    for (int i = 0; i < nFuncs; ++i)
    {
        if (funcs[i])
            funcs[i]->transform(&t, &out[i]);
    }
        
    for (int i = 0; i < COLOR_MAX_COMPS; ++i)
        color->c[i] = dblToCol(out[i]);
}

XWAxialShading *XWAxialShading::parse(XWDict *dict)    
{        
  XWAxialShading *shading;
  double x0A, y0A, x1A, y1A;
  double t0A, t1A;
  XWFunction *funcsA[COLOR_MAX_COMPS];
  int nFuncsA;
  bool extend0A, extend1A;
  XWObject obj1, obj2;
  int i;

  x0A = y0A = x1A = y1A = 0;
  if (dict->lookup("Coords", &obj1)->isArray() &&
      obj1.arrayGetLength() == 4) {
    x0A = obj1.arrayGet(0, &obj2)->getNum();
    obj2.free();
    y0A = obj1.arrayGet(1, &obj2)->getNum();
    obj2.free();
    x1A = obj1.arrayGet(2, &obj2)->getNum();
    obj2.free();
    y1A = obj1.arrayGet(3, &obj2)->getNum();
    obj2.free();
  } else {
    xwApp->error("Missing or invalid Coords in shading dictionary");
    goto err1;
  }
  obj1.free();

  t0A = 0;
  t1A = 1;
  if (dict->lookup("Domain", &obj1)->isArray() &&
      obj1.arrayGetLength() == 2) {
    t0A = obj1.arrayGet(0, &obj2)->getNum();
    obj2.free();
    t1A = obj1.arrayGet(1, &obj2)->getNum();
    obj2.free();
  }
  obj1.free();

  dict->lookup("Function", &obj1);
  if (obj1.isArray()) {
    nFuncsA = obj1.arrayGetLength();
    if (nFuncsA > COLOR_MAX_COMPS) {
      xwApp->error("Invalid Function array in shading dictionary");
      goto err1;
    }
    for (i = 0; i < nFuncsA; ++i) {
      obj1.arrayGet(i, &obj2);
      if (!(funcsA[i] = XWFunction::parse(&obj2))) {
	obj1.free();
	obj2.free();
	goto err1;
      }
      obj2.free();
    }
  } else {
    nFuncsA = 1;
    if (!(funcsA[0] = XWFunction::parse(&obj1))) {
      obj1.free();
      goto err1;
    }
  }
  obj1.free();

  extend0A = extend1A = false;
  if (dict->lookup("Extend", &obj1)->isArray() &&
      obj1.arrayGetLength() == 2) {
    extend0A = obj1.arrayGet(0, &obj2)->getBool();
    obj2.free();
    extend1A = obj1.arrayGet(1, &obj2)->getBool();
    obj2.free();
  }
  obj1.free();

  shading = new XWAxialShading(x0A, y0A, x1A, y1A, t0A, t1A,
				funcsA, nFuncsA, extend0A, extend1A);
  if (!shading->init(dict)) {
    delete shading;
    return NULL;
  }
  return shading;

 err1:
  return NULL;
}

XWRadialShading::XWRadialShading(double x0A, 
                                       double y0A, 
                                       double r0A,
		                               double x1A, 
		                               double y1A, 
		                               double r1A,
		                               double t0A, 
		                               double t1A,
		                               XWFunction **funcsA, 
		                               int nFuncsA,
		                               bool extend0A, 
		                               bool extend1A)
    :XWShading(3)
{
    x0 = x0A;
    y0 = y0A;
    r0 = r0A;
    x1 = x1A;
    y1 = y1A;
    r1 = r1A;
    t0 = t0A;
    t1 = t1A;
    nFuncs = nFuncsA;
    for (int i = 0; i < nFuncs; ++i) 
        funcs[i] = funcsA[i];
        
    extend0 = extend0A;
    extend1 = extend1A;
}

XWRadialShading::XWRadialShading(XWRadialShading *shading)
    :XWShading(shading)
{
    x0 = shading->x0;
    y0 = shading->y0;
    r0 = shading->r0;
    x1 = shading->x1;
    y1 = shading->y1;
    r1 = shading->r1;
    t0 = shading->t0;
    y1 = shading->t1;
    nFuncs = shading->nFuncs;
    for (int i = 0; i < nFuncs; ++i) 
    	funcs[i] = shading->funcs[i]->copy();
    extend0 = shading->extend0;
    extend1 = shading->extend1;
}

XWRadialShading::~XWRadialShading()
{
    for (int i = 0; i < nFuncs; ++i) 
    {
        if (funcs[i])
            delete funcs[i];
    }
}

XWShading * XWRadialShading::copy()
{
    return new XWRadialShading(this);
}

void XWRadialShading::getColor(double t, CoreColor *color)
{
    double out[COLOR_MAX_COMPS];
    
    for (int i = 0; i < COLOR_MAX_COMPS; ++i) 
        out[i] = 0;
        
    for (int i = 0; i < nFuncs; ++i) 
    {
        if (funcs[i])
            funcs[i]->transform(&t, &out[i]);
    }
    
    for (int i = 0; i < COLOR_MAX_COMPS; ++i) 
        color->c[i] = dblToCol(out[i]);
}

XWRadialShading * XWRadialShading::parse(XWDict *dict)
{
  XWRadialShading *shading;
  double x0A, y0A, r0A, x1A, y1A, r1A;
  double t0A, t1A;
  XWFunction *funcsA[COLOR_MAX_COMPS];
  int nFuncsA;
  bool extend0A, extend1A;
  XWObject obj1, obj2;
  int i;

  x0A = y0A = r0A = x1A = y1A = r1A = 0;
  if (dict->lookup("Coords", &obj1)->isArray() &&
      obj1.arrayGetLength() == 6) {
    x0A = obj1.arrayGet(0, &obj2)->getNum();
    obj2.free();
    y0A = obj1.arrayGet(1, &obj2)->getNum();
    obj2.free();
    r0A = obj1.arrayGet(2, &obj2)->getNum();
    obj2.free();
    x1A = obj1.arrayGet(3, &obj2)->getNum();
    obj2.free();
    y1A = obj1.arrayGet(4, &obj2)->getNum();
    obj2.free();
    r1A = obj1.arrayGet(5, &obj2)->getNum();
    obj2.free();
  } else {
    xwApp->error("Missing or invalid Coords in shading dictionary");
    goto err1;
  }
  obj1.free();

  t0A = 0;
  t1A = 1;
  if (dict->lookup("Domain", &obj1)->isArray() &&
      obj1.arrayGetLength() == 2) {
    t0A = obj1.arrayGet(0, &obj2)->getNum();
    obj2.free();
    t1A = obj1.arrayGet(1, &obj2)->getNum();
    obj2.free();
  }
  obj1.free();

  dict->lookup("Function", &obj1);
  if (obj1.isArray()) {
    nFuncsA = obj1.arrayGetLength();
    if (nFuncsA > COLOR_MAX_COMPS) {
      xwApp->error("Invalid Function array in shading dictionary");
      goto err1;
    }
    for (i = 0; i < nFuncsA; ++i) {
      obj1.arrayGet(i, &obj2);
      if (!(funcsA[i] = XWFunction::parse(&obj2))) {
	obj1.free();
	obj2.free();
	goto err1;
      }
      obj2.free();
    }
  } else {
    nFuncsA = 1;
    if (!(funcsA[0] = XWFunction::parse(&obj1))) {
      obj1.free();
      goto err1;
    }
  }
  obj1.free();

  extend0A = extend1A = false;
  if (dict->lookup("Extend", &obj1)->isArray() &&
      obj1.arrayGetLength() == 2) {
    extend0A = obj1.arrayGet(0, &obj2)->getBool();
    obj2.free();
    extend1A = obj1.arrayGet(1, &obj2)->getBool();
    obj2.free();
  }
  obj1.free();

  shading = new XWRadialShading(x0A, y0A, r0A, x1A, y1A, r1A, t0A, t1A,
				 funcsA, nFuncsA, extend0A, extend1A);
  if (!shading->init(dict)) {
    delete shading;
    return NULL;
  }
  return shading;

 err1:
  return NULL;
}


class XWShadingBitBuf 
{
public:
    XWShadingBitBuf(XWStream *strA);
    ~XWShadingBitBuf();
        
    void flushBits();
    
    bool getBits(int n, uint *val);

private:
    XWStream *str;
    int bitBuf;
    int nBits;
};

XWShadingBitBuf::XWShadingBitBuf(XWStream *strA) 
{
    str = strA;
    str->reset();
    bitBuf = 0;
    nBits = 0;
}

XWShadingBitBuf::~XWShadingBitBuf()
{
    str->close();
}

bool XWShadingBitBuf::getBits(int n, uint *val)
{
  int x;

  if (nBits >= n) {
    x = (bitBuf >> (nBits - n)) & ((1 << n) - 1);
    nBits -= n;
  } else {
    x = 0;
    if (nBits > 0) {
      x = bitBuf & ((1 << nBits) - 1);
      n -= nBits;
      nBits = 0;
    }
    while (n > 0) {
      if ((bitBuf = str->getChar()) == EOF) {
	nBits = 0;
	return false;
      }
      if (n >= 8) {
	x = (x << 8) | bitBuf;
	n -= 8;
      } else {
	x = (x << n) | (bitBuf >> (8 - n));
	nBits = 8 - n;
	n = 0;
      }
    }
  }
  *val = x;
  return true;
}

void XWShadingBitBuf::flushBits()
{
    bitBuf = 0;
    nBits = 0;
}

XWGouraudTriangleShading::XWGouraudTriangleShading(int typeA,
			                                       GouraudVertex *verticesA, 
			                                       int nVerticesA,
			                                       int (*trianglesA)[3], 
			                                       int nTrianglesA,
			                                       XWFunction **funcsA, 
			                                       int nFuncsA)
	:XWShading(typeA)
{
    vertices = verticesA;
    nVertices = nVerticesA;
    triangles = trianglesA;
    nTriangles = nTrianglesA;
    nFuncs = nFuncsA;
    for (int i = 0; i < nFuncs; ++i) 
        funcs[i] = funcsA[i];
}

XWGouraudTriangleShading::XWGouraudTriangleShading(XWGouraudTriangleShading *shading)
    :XWShading(shading)
{
    if (shading)
    {
        nVertices = shading->nVertices;
        vertices = (GouraudVertex *)malloc(nVertices * sizeof(GouraudVertex));
        memcpy(vertices, shading->vertices, nVertices * sizeof(GouraudVertex));
        nTriangles = shading->nTriangles;
        triangles = (int (*)[3])malloc(nTriangles * 3 * sizeof(int));
        memcpy(triangles, shading->triangles, nTriangles * 3 * sizeof(int));
        nFuncs = shading->nFuncs;
        for (int i = 0; i < nFuncs; ++i) 
        {
            if (shading->funcs[i])
                funcs[i] = shading->funcs[i]->copy();
            else
                funcs[i] = 0;
        }
    }
    else
    {
        nVertices = 0;
        vertices = 0;
        nTriangles = 0;
        triangles = 0;
        for (int i = 0; i < nFuncs; ++i) 
            funcs[i] = 0;
    }
}

XWGouraudTriangleShading::~XWGouraudTriangleShading()
{
    if (vertices)
        free(vertices);
        
    if (triangles)
        free(triangles);
        
    for (int i = 0; i < nFuncs; ++i)
    {
        if (funcs[i])
            delete funcs[i];
    }
}

XWShading * XWGouraudTriangleShading::copy()
{
    return new XWGouraudTriangleShading(this);
}

void XWGouraudTriangleShading::getTriangle(int i, double *x0, double *y0, CoreColor *color0,
		                                      double *x1, double *y1, CoreColor *color1,
		                                      double *x2, double *y2, CoreColor *color2)
{
  double in;
  double out[COLOR_MAX_COMPS];
  int v, j;

  v = triangles[i][0];
  *x0 = vertices[v].x;
  *y0 = vertices[v].y;
  if (nFuncs > 0) {
    in = colToDbl(vertices[v].color.c[0]);
    for (j = 0; j < nFuncs; ++j) {
      funcs[j]->transform(&in, &out[j]);
    }
    for (j = 0; j < COLOR_MAX_COMPS; ++j) {
      color0->c[j] = dblToCol(out[j]);
    }
  } else {
    *color0 = vertices[v].color;
  }
  v = triangles[i][1];
  *x1 = vertices[v].x;
  *y1 = vertices[v].y;
  if (nFuncs > 0) {
    in = colToDbl(vertices[v].color.c[0]);
    for (j = 0; j < nFuncs; ++j) {
      funcs[j]->transform(&in, &out[j]);
    }
    for (j = 0; j < COLOR_MAX_COMPS; ++j) {
      color1->c[j] = dblToCol(out[j]);
    }
  } else {
    *color1 = vertices[v].color;
  }
  v = triangles[i][2];
  *x2 = vertices[v].x;
  *y2 = vertices[v].y;
  if (nFuncs > 0) {
    in = colToDbl(vertices[v].color.c[0]);
    for (j = 0; j < nFuncs; ++j) {
      funcs[j]->transform(&in, &out[j]);
    }
    for (j = 0; j < COLOR_MAX_COMPS; ++j) {
      color2->c[j] = dblToCol(out[j]);
    }
  } else {
    *color2 = vertices[v].color;
  }
}

XWGouraudTriangleShading * XWGouraudTriangleShading::parse(int typeA, XWDict *dict, XWStream *str)
{
  XWGouraudTriangleShading *shading;
  XWFunction *funcsA[COLOR_MAX_COMPS];
  int nFuncsA;
  int coordBits, compBits, flagBits, vertsPerRow, nRows;
  double xMin, xMax, yMin, yMax;
  double cMin[COLOR_MAX_COMPS], cMax[COLOR_MAX_COMPS];
  double xMul, yMul;
  double cMul[COLOR_MAX_COMPS];
  GouraudVertex *verticesA;
  int (*trianglesA)[3];
  int nComps, nVerticesA, nTrianglesA, vertSize, triSize;
  uint x, y, flag;
  uint c[COLOR_MAX_COMPS];
  XWShadingBitBuf *bitBuf;
  XWObject obj1, obj2;
  int i, j, k, state;

  if (dict->lookup("BitsPerCoordinate", &obj1)->isInt()) {
    coordBits = obj1.getInt();
  } else {
    xwApp->error("Missing or invalid BitsPerCoordinate in shading dictionary");
    goto err2;
  }
  obj1.free();
  if (dict->lookup("BitsPerComponent", &obj1)->isInt()) {
    compBits = obj1.getInt();
  } else {
    xwApp->error("Missing or invalid BitsPerComponent in shading dictionary");
    goto err2;
  }
  obj1.free();
  flagBits = vertsPerRow = 0; // make gcc happy
  if (typeA == 4) {
    if (dict->lookup("BitsPerFlag", &obj1)->isInt()) {
      flagBits = obj1.getInt();
    } else {
      xwApp->error("Missing or invalid BitsPerFlag in shading dictionary");
      goto err2;
    }
    obj1.free();
  } else {
    if (dict->lookup("VerticesPerRow", &obj1)->isInt()) {
      vertsPerRow = obj1.getInt();
    } else {
      xwApp->error("Missing or invalid VerticesPerRow in shading dictionary");
      goto err2;
    }
    obj1.free();
  }
  if (dict->lookup("Decode", &obj1)->isArray() &&
      obj1.arrayGetLength() >= 6) {
    xMin = obj1.arrayGet(0, &obj2)->getNum();
    obj2.free();
    xMax = obj1.arrayGet(1, &obj2)->getNum();
    obj2.free();
    xMul = (xMax - xMin) / (pow(2.0, coordBits) - 1);
    yMin = obj1.arrayGet(2, &obj2)->getNum();
    obj2.free();
    yMax = obj1.arrayGet(3, &obj2)->getNum();
    obj2.free();
    yMul = (yMax - yMin) / (pow(2.0, coordBits) - 1);
    for (i = 0; 5 + 2*i < obj1.arrayGetLength() && i < COLOR_MAX_COMPS; ++i) {
      cMin[i] = obj1.arrayGet(4 + 2*i, &obj2)->getNum();
      obj2.free();
      cMax[i] = obj1.arrayGet(5 + 2*i, &obj2)->getNum();
      obj2.free();
      cMul[i] = (cMax[i] - cMin[i]) / (double)((1 << compBits) - 1);
    }
    nComps = i;
  } else {
    xwApp->error("Missing or invalid Decode array in shading dictionary");
    goto err2;
  }
  obj1.free();

  if (!dict->lookup("Function", &obj1)->isNull()) {
    if (obj1.isArray()) {
      nFuncsA = obj1.arrayGetLength();
      if (nFuncsA > COLOR_MAX_COMPS) {
	xwApp->error("Invalid Function array in shading dictionary");
	goto err1;
      }
      for (i = 0; i < nFuncsA; ++i) {
	obj1.arrayGet(i, &obj2);
	if (!(funcsA[i] = XWFunction::parse(&obj2))) {
	  obj1.free();
	  obj2.free();
	  goto err1;
	}
	obj2.free();
      }
    } else {
      nFuncsA = 1;
      if (!(funcsA[0] = XWFunction::parse(&obj1))) {
	obj1.free();
	goto err1;
      }
    }
  } else {
    nFuncsA = 0;
  }
  obj1.free();

  nVerticesA = nTrianglesA = 0;
  verticesA = NULL;
  trianglesA = NULL;
  vertSize = triSize = 0;
  state = 0;
  flag = 0; // make gcc happy
  bitBuf = new XWShadingBitBuf(str);
  while (1) {
    if (typeA == 4) {
      if (!bitBuf->getBits(flagBits, &flag)) {
	break;
      }
    }
    if (!bitBuf->getBits(coordBits, &x) ||
	!bitBuf->getBits(coordBits, &y)) {
      break;
    }
    for (i = 0; i < nComps; ++i) {
      if (!bitBuf->getBits(compBits, &c[i])) {
	break;
      }
    }
    if (i < nComps) {
      break;
    }
    if (nVerticesA == vertSize) {
      vertSize = (vertSize == 0) ? 16 : 2 * vertSize;
      verticesA = (GouraudVertex *)realloc(verticesA, vertSize * sizeof(GouraudVertex));
    }
    verticesA[nVerticesA].x = xMin + xMul * (double)x;
    verticesA[nVerticesA].y = yMin + yMul * (double)y;
    for (i = 0; i < nComps; ++i) {
      verticesA[nVerticesA].color.c[i] =
	  dblToCol(cMin[i] + cMul[i] * (double)c[i]);
    }
    ++nVerticesA;
    bitBuf->flushBits();
    if (typeA == 4) {
      if (state == 0 || state == 1) {
	++state;
      } else if (state == 2 || flag > 0) {
	if (nTrianglesA == triSize) {
	  triSize = (triSize == 0) ? 16 : 2 * triSize;
	  trianglesA = (int (*)[3])realloc(trianglesA, triSize * 3 * sizeof(int));
	}
	if (state == 2) {
	  trianglesA[nTrianglesA][0] = nVerticesA - 3;
	  trianglesA[nTrianglesA][1] = nVerticesA - 2;
	  trianglesA[nTrianglesA][2] = nVerticesA - 1;
	  ++state;
	} else if (flag == 1) {
	  trianglesA[nTrianglesA][0] = trianglesA[nTrianglesA - 1][1];
	  trianglesA[nTrianglesA][1] = trianglesA[nTrianglesA - 1][2];
	  trianglesA[nTrianglesA][2] = nVerticesA - 1;
	} else { // flag == 2
	  trianglesA[nTrianglesA][0] = trianglesA[nTrianglesA - 1][0];
	  trianglesA[nTrianglesA][1] = trianglesA[nTrianglesA - 1][2];
	  trianglesA[nTrianglesA][2] = nVerticesA - 1;
	}
	++nTrianglesA;
      } else { // state == 3 && flag == 0
	state = 1;
      }
    }
  }
  delete bitBuf;
  if (typeA == 5) {
    nRows = nVerticesA / vertsPerRow;
    nTrianglesA = (nRows - 1) * 2 * (vertsPerRow - 1);
    trianglesA = (int (*)[3])malloc(nTrianglesA * 3 * sizeof(int));
    k = 0;
    for (i = 0; i < nRows - 1; ++i) {
      for (j = 0; j < vertsPerRow - 1; ++j) {
	trianglesA[k][0] = i * vertsPerRow + j;
	trianglesA[k][1] = i * vertsPerRow + j+1;
	trianglesA[k][2] = (i+1) * vertsPerRow + j;
	++k;
	trianglesA[k][0] = i * vertsPerRow + j+1;
	trianglesA[k][1] = (i+1) * vertsPerRow + j;
	trianglesA[k][2] = (i+1) * vertsPerRow + j+1;
	++k;
      }
    }
  }

  shading = new XWGouraudTriangleShading(typeA, verticesA, nVerticesA,
					  trianglesA, nTrianglesA,
					  funcsA, nFuncsA);
  if (!shading->init(dict)) {
    delete shading;
    return NULL;
  }
  return shading;

 err2:
  obj1.free();
 err1:
  return NULL;
}

XWPatchMeshShading::XWPatchMeshShading(int typeA, 
                                       XWPatch *patchesA, 
                                       int nPatchesA,
		                               XWFunction **funcsA, 
		                               int nFuncsA)
    :XWShading(typeA)
{
    patches = patchesA;
    nPatches = nPatchesA;
    nFuncs = nFuncsA;
    for (int i = 0; i < nFuncs; ++i) 
        funcs[i] = funcsA[i];
}

XWPatchMeshShading::XWPatchMeshShading(XWPatchMeshShading *shading)
    :XWShading(shading)
{
    if (shading)
    {
        nPatches = shading->nPatches;
        patches = (XWPatch *)malloc(nPatches * sizeof(XWPatch));
        memcpy(patches, shading->patches, nPatches * sizeof(XWPatch));
        nFuncs = shading->nFuncs;
        for (int i = 0; i < nFuncs; ++i) 
        	funcs[i] = shading->funcs[i]->copy();
    }
    else
    {
        nPatches = 0;
        patches = 0;
        nFuncs = 0;
    }
}

XWPatchMeshShading::~XWPatchMeshShading()
{
    if (patches)
        free(patches);
        
    for (int i = 0; i < nFuncs; ++i)
    {
        if (funcs[i])
            delete funcs[i];
    }
}

XWShading * XWPatchMeshShading::copy()
{
    return new XWPatchMeshShading(this);
}

XWPatchMeshShading * XWPatchMeshShading::parse(int typeA, XWDict *dict, XWStream *str)
{
  XWPatchMeshShading *shading;
  XWFunction *funcsA[COLOR_MAX_COMPS];
  int nFuncsA;
  int coordBits, compBits, flagBits;
  double xMin, xMax, yMin, yMax;
  double cMin[COLOR_MAX_COMPS], cMax[COLOR_MAX_COMPS];
  double xMul, yMul;
  double cMul[COLOR_MAX_COMPS];
  XWPatch *patchesA, *p;
  int nComps, nPatchesA, patchesSize, nPts, nColors;
  uint flag;
  double x[16], y[16];
  uint xi, yi;
  int c[4][COLOR_MAX_COMPS];
  uint ci;
  XWShadingBitBuf *bitBuf;
  XWObject obj1, obj2;
  int i, j;

  if (dict->lookup("BitsPerCoordinate", &obj1)->isInt()) {
    coordBits = obj1.getInt();
  } else {
    xwApp->error("Missing or invalid BitsPerCoordinate in shading dictionary");
    goto err2;
  }
  obj1.free();
  if (dict->lookup("BitsPerComponent", &obj1)->isInt()) {
    compBits = obj1.getInt();
  } else {
    xwApp->error("Missing or invalid BitsPerComponent in shading dictionary");
    goto err2;
  }
  obj1.free();
  if (dict->lookup("BitsPerFlag", &obj1)->isInt()) {
    flagBits = obj1.getInt();
  } else {
    xwApp->error("Missing or invalid BitsPerFlag in shading dictionary");
    goto err2;
  }
  obj1.free();
  if (dict->lookup("Decode", &obj1)->isArray() &&
      obj1.arrayGetLength() >= 6) {
    xMin = obj1.arrayGet(0, &obj2)->getNum();
    obj2.free();
    xMax = obj1.arrayGet(1, &obj2)->getNum();
    obj2.free();
    xMul = (xMax - xMin) / (pow(2.0, coordBits) - 1);
    yMin = obj1.arrayGet(2, &obj2)->getNum();
    obj2.free();
    yMax = obj1.arrayGet(3, &obj2)->getNum();
    obj2.free();
    yMul = (yMax - yMin) / (pow(2.0, coordBits) - 1);
    for (i = 0; 5 + 2*i < obj1.arrayGetLength() && i < COLOR_MAX_COMPS; ++i) {
      cMin[i] = obj1.arrayGet(4 + 2*i, &obj2)->getNum();
      obj2.free();
      cMax[i] = obj1.arrayGet(5 + 2*i, &obj2)->getNum();
      obj2.free();
      cMul[i] = (cMax[i] - cMin[i]) / (double)((1 << compBits) - 1);
    }
    nComps = i;
  } else {
    xwApp->error("Missing or invalid Decode array in shading dictionary");
    goto err2;
  }
  obj1.free();

  if (!dict->lookup("Function", &obj1)->isNull()) {
    if (obj1.isArray()) {
      nFuncsA = obj1.arrayGetLength();
      if (nFuncsA > COLOR_MAX_COMPS) {
	xwApp->error("Invalid Function array in shading dictionary");
	goto err1;
      }
      for (i = 0; i < nFuncsA; ++i) {
	obj1.arrayGet(i, &obj2);
	if (!(funcsA[i] = XWFunction::parse(&obj2))) {
	  obj1.free();
	  obj2.free();
	  goto err1;
	}
	obj2.free();
      }
    } else {
      nFuncsA = 1;
      if (!(funcsA[0] = XWFunction::parse(&obj1))) {
	obj1.free();
	goto err1;
      }
    }
  } else {
    nFuncsA = 0;
  }
  obj1.free();

  nPatchesA = 0;
  patchesA = NULL;
  patchesSize = 0;
  bitBuf = new XWShadingBitBuf(str);
  while (1) {
    if (!bitBuf->getBits(flagBits, &flag)) {
      break;
    }
    if (typeA == 6) {
      switch (flag) {
      case 0: nPts = 12; nColors = 4; break;
      case 1:
      case 2:
      case 3:
      default: nPts =  8; nColors = 2; break;
      }
    } else {
      switch (flag) {
      case 0: nPts = 16; nColors = 4; break;
      case 1:
      case 2:
      case 3:
      default: nPts = 12; nColors = 2; break;
      }
    }
    for (i = 0; i < nPts; ++i) {
      if (!bitBuf->getBits(coordBits, &xi) ||
	  !bitBuf->getBits(coordBits, &yi)) {
	break;
      }
      x[i] = xMin + xMul * (double)xi;
      y[i] = yMin + yMul * (double)yi;
    }
    if (i < nPts) {
      break;
    }
    for (i = 0; i < nColors; ++i) {
      for (j = 0; j < nComps; ++j) {
	if (!bitBuf->getBits(compBits, &ci)) {
	  break;
	}
	c[i][j] = dblToCol(cMin[j] + cMul[j] * (double)ci);
      }
      if (j < nComps) {
	break;
      }
    }
    if (i < nColors) {
      break;
    }
    if (nPatchesA == patchesSize) {
      patchesSize = (patchesSize == 0) ? 16 : 2 * patchesSize;
      patchesA = (XWPatch *)realloc(patchesA, patchesSize * sizeof(XWPatch));
    }
    p = &patchesA[nPatchesA];
    if (typeA == 6) {
      switch (flag) {
      case 0:
	p->x[0][0] = x[0];
	p->y[0][0] = y[0];
	p->x[0][1] = x[1];
	p->y[0][1] = y[1];
	p->x[0][2] = x[2];
	p->y[0][2] = y[2];
	p->x[0][3] = x[3];
	p->y[0][3] = y[3];
	p->x[1][3] = x[4];
	p->y[1][3] = y[4];
	p->x[2][3] = x[5];
	p->y[2][3] = y[5];
	p->x[3][3] = x[6];
	p->y[3][3] = y[6];
	p->x[3][2] = x[7];
	p->y[3][2] = y[7];
	p->x[3][1] = x[8];
	p->y[3][1] = y[8];
	p->x[3][0] = x[9];
	p->y[3][0] = y[9];
	p->x[2][0] = x[10];
	p->y[2][0] = y[10];
	p->x[1][0] = x[11];
	p->y[1][0] = y[11];
	for (j = 0; j < nComps; ++j) {
	  p->color[0][0].c[j] = c[0][j];
	  p->color[0][1].c[j] = c[1][j];
	  p->color[1][1].c[j] = c[2][j];
	  p->color[1][0].c[j] = c[3][j];
	}
	break;
      case 1:
	p->x[0][0] = patchesA[nPatchesA-1].x[0][3];
	p->y[0][0] = patchesA[nPatchesA-1].y[0][3];
	p->x[0][1] = patchesA[nPatchesA-1].x[1][3];
	p->y[0][1] = patchesA[nPatchesA-1].y[1][3];
	p->x[0][2] = patchesA[nPatchesA-1].x[2][3];
	p->y[0][2] = patchesA[nPatchesA-1].y[2][3];
	p->x[0][3] = patchesA[nPatchesA-1].x[3][3];
	p->y[0][3] = patchesA[nPatchesA-1].y[3][3];
	p->x[1][3] = x[0];
	p->y[1][3] = y[0];
	p->x[2][3] = x[1];
	p->y[2][3] = y[1];
	p->x[3][3] = x[2];
	p->y[3][3] = y[2];
	p->x[3][2] = x[3];
	p->y[3][2] = y[3];
	p->x[3][1] = x[4];
	p->y[3][1] = y[4];
	p->x[3][0] = x[5];
	p->y[3][0] = y[5];
	p->x[2][0] = x[6];
	p->y[2][0] = y[6];
	p->x[1][0] = x[7];
	p->y[1][0] = y[7];
	for (j = 0; j < nComps; ++j) {
	  p->color[0][0].c[j] = patchesA[nPatchesA-1].color[0][1].c[j];
	  p->color[0][1].c[j] = patchesA[nPatchesA-1].color[1][1].c[j];
	  p->color[1][1].c[j] = c[0][j];
	  p->color[1][0].c[j] = c[1][j];
	}
	break;
      case 2:
	p->x[0][0] = patchesA[nPatchesA-1].x[3][3];
	p->y[0][0] = patchesA[nPatchesA-1].y[3][3];
	p->x[0][1] = patchesA[nPatchesA-1].x[3][2];
	p->y[0][1] = patchesA[nPatchesA-1].y[3][2];
	p->x[0][2] = patchesA[nPatchesA-1].x[3][1];
	p->y[0][2] = patchesA[nPatchesA-1].y[3][1];
	p->x[0][3] = patchesA[nPatchesA-1].x[3][0];
	p->y[0][3] = patchesA[nPatchesA-1].y[3][0];
	p->x[1][3] = x[0];
	p->y[1][3] = y[0];
	p->x[2][3] = x[1];
	p->y[2][3] = y[1];
	p->x[3][3] = x[2];
	p->y[3][3] = y[2];
	p->x[3][2] = x[3];
	p->y[3][2] = y[3];
	p->x[3][1] = x[4];
	p->y[3][1] = y[4];
	p->x[3][0] = x[5];
	p->y[3][0] = y[5];
	p->x[2][0] = x[6];
	p->y[2][0] = y[6];
	p->x[1][0] = x[7];
	p->y[1][0] = y[7];
	for (j = 0; j < nComps; ++j) {
	  p->color[0][0].c[j] = patchesA[nPatchesA-1].color[1][1].c[j];
	  p->color[0][1].c[j] = patchesA[nPatchesA-1].color[1][0].c[j];
	  p->color[1][1].c[j] = c[0][j];
	  p->color[1][0].c[j] = c[1][j];
	}
	break;
      case 3:
	p->x[0][0] = patchesA[nPatchesA-1].x[3][0];
	p->y[0][0] = patchesA[nPatchesA-1].y[3][0];
	p->x[0][1] = patchesA[nPatchesA-1].x[2][0];
	p->y[0][1] = patchesA[nPatchesA-1].y[2][0];
	p->x[0][2] = patchesA[nPatchesA-1].x[1][0];
	p->y[0][2] = patchesA[nPatchesA-1].y[1][0];
	p->x[0][3] = patchesA[nPatchesA-1].x[0][0];
	p->y[0][3] = patchesA[nPatchesA-1].y[0][0];
	p->x[1][3] = x[0];
	p->y[1][3] = y[0];
	p->x[2][3] = x[1];
	p->y[2][3] = y[1];
	p->x[3][3] = x[2];
	p->y[3][3] = y[2];
	p->x[3][2] = x[3];
	p->y[3][2] = y[3];
	p->x[3][1] = x[4];
	p->y[3][1] = y[4];
	p->x[3][0] = x[5];
	p->y[3][0] = y[5];
	p->x[2][0] = x[6];
	p->y[2][0] = y[6];
	p->x[1][0] = x[7];
	p->y[1][0] = y[7];
	for (j = 0; j < nComps; ++j) {
	  p->color[0][1].c[j] = patchesA[nPatchesA-1].color[1][0].c[j];
	  p->color[0][1].c[j] = patchesA[nPatchesA-1].color[0][0].c[j];
	  p->color[1][1].c[j] = c[0][j];
	  p->color[1][0].c[j] = c[1][j];
	}
	break;
      }
    } else {
      switch (flag) {
      case 0:
	p->x[0][0] = x[0];
	p->y[0][0] = y[0];
	p->x[0][1] = x[1];
	p->y[0][1] = y[1];
	p->x[0][2] = x[2];
	p->y[0][2] = y[2];
	p->x[0][3] = x[3];
	p->y[0][3] = y[3];
	p->x[1][3] = x[4];
	p->y[1][3] = y[4];
	p->x[2][3] = x[5];
	p->y[2][3] = y[5];
	p->x[3][3] = x[6];
	p->y[3][3] = y[6];
	p->x[3][2] = x[7];
	p->y[3][2] = y[7];
	p->x[3][1] = x[8];
	p->y[3][1] = y[8];
	p->x[3][0] = x[9];
	p->y[3][0] = y[9];
	p->x[2][0] = x[10];
	p->y[2][0] = y[10];
	p->x[1][0] = x[11];
	p->y[1][0] = y[11];
	p->x[1][1] = x[12];
	p->y[1][1] = y[12];
	p->x[1][2] = x[13];
	p->y[1][2] = y[13];
	p->x[2][2] = x[14];
	p->y[2][2] = y[14];
	p->x[2][1] = x[15];
	p->y[2][1] = y[15];
	for (j = 0; j < nComps; ++j) {
	  p->color[0][0].c[j] = c[0][j];
	  p->color[0][1].c[j] = c[1][j];
	  p->color[1][1].c[j] = c[2][j];
	  p->color[1][0].c[j] = c[3][j];
	}
	break;
      case 1:
	p->x[0][0] = patchesA[nPatchesA-1].x[0][3];
	p->y[0][0] = patchesA[nPatchesA-1].y[0][3];
	p->x[0][1] = patchesA[nPatchesA-1].x[1][3];
	p->y[0][1] = patchesA[nPatchesA-1].y[1][3];
	p->x[0][2] = patchesA[nPatchesA-1].x[2][3];
	p->y[0][2] = patchesA[nPatchesA-1].y[2][3];
	p->x[0][3] = patchesA[nPatchesA-1].x[3][3];
	p->y[0][3] = patchesA[nPatchesA-1].y[3][3];
	p->x[1][3] = x[0];
	p->y[1][3] = y[0];
	p->x[2][3] = x[1];
	p->y[2][3] = y[1];
	p->x[3][3] = x[2];
	p->y[3][3] = y[2];
	p->x[3][2] = x[3];
	p->y[3][2] = y[3];
	p->x[3][1] = x[4];
	p->y[3][1] = y[4];
	p->x[3][0] = x[5];
	p->y[3][0] = y[5];
	p->x[2][0] = x[6];
	p->y[2][0] = y[6];
	p->x[1][0] = x[7];
	p->y[1][0] = y[7];
	p->x[1][1] = x[8];
	p->y[1][1] = y[8];
	p->x[1][2] = x[9];
	p->y[1][2] = y[9];
	p->x[2][2] = x[10];
	p->y[2][2] = y[10];
	p->x[2][1] = x[11];
	p->y[2][1] = y[11];
	for (j = 0; j < nComps; ++j) {
	  p->color[0][0].c[j] = patchesA[nPatchesA-1].color[0][1].c[j];
	  p->color[0][1].c[j] = patchesA[nPatchesA-1].color[1][1].c[j];
	  p->color[1][1].c[j] = c[0][j];
	  p->color[1][0].c[j] = c[1][j];
	}
	break;
      case 2:
	p->x[0][0] = patchesA[nPatchesA-1].x[3][3];
	p->y[0][0] = patchesA[nPatchesA-1].y[3][3];
	p->x[0][1] = patchesA[nPatchesA-1].x[3][2];
	p->y[0][1] = patchesA[nPatchesA-1].y[3][2];
	p->x[0][2] = patchesA[nPatchesA-1].x[3][1];
	p->y[0][2] = patchesA[nPatchesA-1].y[3][1];
	p->x[0][3] = patchesA[nPatchesA-1].x[3][0];
	p->y[0][3] = patchesA[nPatchesA-1].y[3][0];
	p->x[1][3] = x[0];
	p->y[1][3] = y[0];
	p->x[2][3] = x[1];
	p->y[2][3] = y[1];
	p->x[3][3] = x[2];
	p->y[3][3] = y[2];
	p->x[3][2] = x[3];
	p->y[3][2] = y[3];
	p->x[3][1] = x[4];
	p->y[3][1] = y[4];
	p->x[3][0] = x[5];
	p->y[3][0] = y[5];
	p->x[2][0] = x[6];
	p->y[2][0] = y[6];
	p->x[1][0] = x[7];
	p->y[1][0] = y[7];
	p->x[1][1] = x[8];
	p->y[1][1] = y[8];
	p->x[1][2] = x[9];
	p->y[1][2] = y[9];
	p->x[2][2] = x[10];
	p->y[2][2] = y[10];
	p->x[2][1] = x[11];
	p->y[2][1] = y[11];
	for (j = 0; j < nComps; ++j) {
	  p->color[0][0].c[j] = patchesA[nPatchesA-1].color[1][1].c[j];
	  p->color[0][1].c[j] = patchesA[nPatchesA-1].color[1][0].c[j];
	  p->color[1][1].c[j] = c[0][j];
	  p->color[1][0].c[j] = c[1][j];
	}
	break;
      case 3:
	p->x[0][0] = patchesA[nPatchesA-1].x[3][0];
	p->y[0][0] = patchesA[nPatchesA-1].y[3][0];
	p->x[0][1] = patchesA[nPatchesA-1].x[2][0];
	p->y[0][1] = patchesA[nPatchesA-1].y[2][0];
	p->x[0][2] = patchesA[nPatchesA-1].x[1][0];
	p->y[0][2] = patchesA[nPatchesA-1].y[1][0];
	p->x[0][3] = patchesA[nPatchesA-1].x[0][0];
	p->y[0][3] = patchesA[nPatchesA-1].y[0][0];
	p->x[1][3] = x[0];
	p->y[1][3] = y[0];
	p->x[2][3] = x[1];
	p->y[2][3] = y[1];
	p->x[3][3] = x[2];
	p->y[3][3] = y[2];
	p->x[3][2] = x[3];
	p->y[3][2] = y[3];
	p->x[3][1] = x[4];
	p->y[3][1] = y[4];
	p->x[3][0] = x[5];
	p->y[3][0] = y[5];
	p->x[2][0] = x[6];
	p->y[2][0] = y[6];
	p->x[1][0] = x[7];
	p->y[1][0] = y[7];
	p->x[1][1] = x[8];
	p->y[1][1] = y[8];
	p->x[1][2] = x[9];
	p->y[1][2] = y[9];
	p->x[2][2] = x[10];
	p->y[2][2] = y[10];
	p->x[2][1] = x[11];
	p->y[2][1] = y[11];
	for (j = 0; j < nComps; ++j) {
	  p->color[0][0].c[j] = patchesA[nPatchesA-1].color[1][0].c[j];
	  p->color[0][1].c[j] = patchesA[nPatchesA-1].color[0][0].c[j];
	  p->color[1][1].c[j] = c[0][j];
	  p->color[1][0].c[j] = c[1][j];
	}
	break;
      }
    }
    ++nPatchesA;
    bitBuf->flushBits();
  }
  delete bitBuf;

  if (typeA == 6) {
    for (i = 0; i < nPatchesA; ++i) {
      p = &patchesA[i];
      p->x[1][1] = (-4 * p->x[0][0]
		    +6 * (p->x[0][1] + p->x[1][0])
		    -2 * (p->x[0][3] + p->x[3][0])
		    +3 * (p->x[3][1] + p->x[1][3])
		    - p->x[3][3]) / 9;
      p->y[1][1] = (-4 * p->y[0][0]
		    +6 * (p->y[0][1] + p->y[1][0])
		    -2 * (p->y[0][3] + p->y[3][0])
		    +3 * (p->y[3][1] + p->y[1][3])
		    - p->y[3][3]) / 9;
      p->x[1][2] = (-4 * p->x[0][3]
		    +6 * (p->x[0][2] + p->x[1][3])
		    -2 * (p->x[0][0] + p->x[3][3])
		    +3 * (p->x[3][2] + p->x[1][0])
		    - p->x[3][0]) / 9;
      p->y[1][2] = (-4 * p->y[0][3]
		    +6 * (p->y[0][2] + p->y[1][3])
		    -2 * (p->y[0][0] + p->y[3][3])
		    +3 * (p->y[3][2] + p->y[1][0])
		    - p->y[3][0]) / 9;
      p->x[2][1] = (-4 * p->x[3][0]
		    +6 * (p->x[3][1] + p->x[2][0])
		    -2 * (p->x[3][3] + p->x[0][0])
		    +3 * (p->x[0][1] + p->x[2][3])
		    - p->x[0][3]) / 9;
      p->y[2][1] = (-4 * p->y[3][0]
		    +6 * (p->y[3][1] + p->y[2][0])
		    -2 * (p->y[3][3] + p->y[0][0])
		    +3 * (p->y[0][1] + p->y[2][3])
		    - p->y[0][3]) / 9;
      p->x[2][2] = (-4 * p->x[3][3]
		    +6 * (p->x[3][2] + p->x[2][3])
		    -2 * (p->x[3][0] + p->x[0][3])
		    +3 * (p->x[0][2] + p->x[2][0])
		    - p->x[0][0]) / 9;
      p->y[2][2] = (-4 * p->y[3][3]
		    +6 * (p->y[3][2] + p->y[2][3])
		    -2 * (p->y[3][0] + p->y[0][3])
		    +3 * (p->y[0][2] + p->y[2][0])
		    - p->y[0][0]) / 9;
    }
  }

  shading = new XWPatchMeshShading(typeA, patchesA, nPatchesA,
				    funcsA, nFuncsA);
  if (!shading->init(dict)) {
    delete shading;
    return NULL;
  }
  return shading;

 err2:
  obj1.free();
 err1:
  return NULL;
}


XWImageColorMap::XWImageColorMap(int bitsA, XWObject *decode, XWColorSpace *colorSpaceA)
{
	XWIndexedColorSpace *indexedCS;
  XWSeparationColorSpace *sepCS;
  int maxPixel, indexHigh;
  quint8 *indexedLookup;
  XWFunction *sepFunc;
  XWObject obj;
  double x[COLOR_MAX_COMPS];
  double y[COLOR_MAX_COMPS];
  int i, j, k;

  ok = true;

  // bits per component and color space
  bits = bitsA;
  maxPixel = (1 << bits) - 1;
  colorSpace = colorSpaceA;

  // initialize
  for (k = 0; k < COLOR_MAX_COMPS; ++k) 
  {
    lookup[k] = NULL;
    lookup2[k] = NULL;
  }

  // get decode map
  if (decode->isNull()) 
  {
    nComps = colorSpace->getNComps();
    colorSpace->getDefaultRanges(decodeLow, decodeRange, maxPixel);
  } 
  else if (decode->isArray()) 
  {
    nComps = decode->arrayGetLength() / 2;
    if (nComps < colorSpace->getNComps()) 
    {
      goto err1;
    }
    if (nComps > colorSpace->getNComps()) 
    {
      xwApp->error("Too many elements in Decode array");
      nComps = colorSpace->getNComps();
    }
    for (i = 0; i < nComps; ++i) 
    {
      decode->arrayGet(2*i, &obj);
      if (!obj.isNum()) 
      {
				goto err2;
      }
      decodeLow[i] = obj.getNum();
      obj.free();
      decode->arrayGet(2*i+1, &obj);
      if (!obj.isNum()) 
      {
				goto err2;
      }
      decodeRange[i] = obj.getNum() - decodeLow[i];
      obj.free();
    }
  } 
  else 
  {
    goto err1;
  }

  // Construct a lookup table -- this stores pre-computed decoded
  // values for each component, i.e., the result of applying the
  // decode mapping to each possible image pixel component value.
  for (k = 0; k < nComps; ++k) 
  {
    lookup[k] = (int *)malloc((maxPixel + 1) * sizeof(int));
    for (i = 0; i <= maxPixel; ++i) 
    {
      lookup[k][i] = dblToCol(decodeLow[k] + (i * decodeRange[k]) / maxPixel);
    }
  }

  // Optimization: for Indexed and Separation color spaces (which have
  // only one component), we pre-compute a second lookup table with
  // color values
  colorSpace2 = NULL;
  nComps2 = 0;
  if (colorSpace->getMode() == COLOR_SPACE_INDEXED) 
  {
    // Note that indexHigh may not be the same as maxPixel --
    // Distiller will remove unused palette entries, resulting in
    // indexHigh < maxPixel.
    indexedCS = (XWIndexedColorSpace *)colorSpace;
    colorSpace2 = indexedCS->getBase();
    indexHigh = indexedCS->getIndexHigh();
    nComps2 = colorSpace2->getNComps();
    indexedLookup = indexedCS->getLookup();
    colorSpace2->getDefaultRanges(x, y, indexHigh);
    for (k = 0; k < nComps2; ++k) 
    {
      lookup2[k] = (int *)malloc((maxPixel + 1) * sizeof(int));
    }
    for (i = 0; i <= maxPixel; ++i) 
    {
      j = (int)(decodeLow[0] + (i * decodeRange[0]) / maxPixel + 0.5);
      if (j < 0) 
      {
				j = 0;
      } 
      else if (j > indexHigh) 
      {
				j = indexHigh;
      }
      for (k = 0; k < nComps2; ++k) 
      {
				lookup2[k][i] = dblToCol(x[k] + (indexedLookup[j*nComps2 + k] / 255.0) * y[k]);
      }
    }
  } 
  else if (colorSpace->getMode() == COLOR_SPACE_SEPARATION) 
  {
    sepCS = (XWSeparationColorSpace *)colorSpace;
    colorSpace2 = sepCS->getAlt();
    nComps2 = colorSpace2->getNComps();
    sepFunc = sepCS->getFunc();
    for (k = 0; k < nComps2; ++k) 
    {
      lookup2[k] = (int *)malloc((maxPixel + 1) * sizeof(int));
    }
    for (i = 0; i <= maxPixel; ++i) 
    {
      x[0] = decodeLow[0] + (i * decodeRange[0]) / maxPixel;
      sepFunc->transform(x, y);
      for (k = 0; k < nComps2; ++k) 
      {
				lookup2[k][i] = dblToCol(y[k]);
      }
    }
  }

  return;

 err2:
  obj.free();
 err1:
  ok = false;
}

XWImageColorMap::XWImageColorMap(XWImageColorMap *colorMap)
{
	int n, i, k;
  colorSpace = colorMap->colorSpace->copy();
  bits = colorMap->bits;
  nComps = colorMap->nComps;
  nComps2 = colorMap->nComps2;
  colorSpace2 = NULL;
  for (k = 0; k < COLOR_MAX_COMPS; ++k) 
  {
    lookup[k] = NULL;
    lookup2[k] = NULL;
  }
  n = 1 << bits;
  for (k = 0; k < nComps; ++k) 
  {
    lookup[k] = (int *)malloc(n * sizeof(int));
    memcpy(lookup[k], colorMap->lookup[k], n * sizeof(int));
  }
  if (colorSpace->getMode() == COLOR_SPACE_INDEXED) 
  {
    colorSpace2 = ((XWIndexedColorSpace *)colorSpace)->getBase();
    for (k = 0; k < nComps2; ++k) 
    {
      lookup2[k] = (int *)malloc(n * sizeof(int));
      memcpy(lookup2[k], colorMap->lookup2[k], n * sizeof(int));
    }
  } 
  else if (colorSpace->getMode() == COLOR_SPACE_SEPARATION) 
  {
    colorSpace2 = ((XWSeparationColorSpace *)colorSpace)->getAlt();
    for (k = 0; k < nComps2; ++k) 
    {
      lookup2[k] = (int *)malloc(n * sizeof(int));
      memcpy(lookup2[k], colorMap->lookup2[k], n * sizeof(int));
    }
  }
  for (i = 0; i < nComps; ++i) 
  {
    decodeLow[i] = colorMap->decodeLow[i];
    decodeRange[i] = colorMap->decodeRange[i];
  }
  ok = true;
}

XWImageColorMap::~XWImageColorMap()
{
    if (colorSpace)
        delete colorSpace;
        
    for (int i = 0; i < COLOR_MAX_COMPS; ++i)
    {
        if (lookup[i])
            free(lookup[i]);
            
        if (lookup2[i])
        	free(lookup2[i]);
    }
}

void XWImageColorMap::getCMYK(quint8 *x, CoreCMYK *cmyk)
{
  CoreColor color;
    
  if (colorSpace2) 
  {
    for (int i = 0; i < nComps2; ++i) 
    {
      color.c[i] = lookup2[i][x[0]];
    }
    colorSpace2->getCMYK(&color, cmyk);
  } 
  else 
  {
    for (int i = 0; i < nComps; ++i) 
    {
      color.c[i] = lookup[i][x[i]];
    }
    colorSpace->getCMYK(&color, cmyk);
  }
}

void  XWImageColorMap::getColor(quint8 *x, CoreColor *color)
{
    int maxPixel = (1 << bits) - 1;
    for (int i = 0; i < nComps; ++i) 
        color->c[i] = dblToCol(decodeLow[i] + (x[i] * decodeRange[i]) / maxPixel);
}

void XWImageColorMap::getGray(quint8 *x, CoreGray *gray)
{
  CoreColor color;
    
  if (colorSpace2) 
  {
    for (int i = 0; i < nComps2; ++i) 
    {
      color.c[i] = lookup2[i][x[0]];
    }
    colorSpace2->getGray(&color, gray);
  } 
  else 
  {
    for (int i = 0; i < nComps; ++i) 
    {
      color.c[i] = lookup[i][x[i]];
    }
    colorSpace->getGray(&color, gray);
  }
}

void XWImageColorMap::getRGB(quint8 *x, CoreRGB *rgb)
{
    CoreColor color;
    
  if (colorSpace2) 
  {
    for (int i = 0; i < nComps2; ++i) 
   	{
      color.c[i] = lookup2[i][x[0]];
    }
    colorSpace2->getRGB(&color, rgb);
  } 
  else 
  {
    for (int i = 0; i < nComps; ++i) 
    {
      color.c[i] = lookup[i][x[i]];
    }
    colorSpace->getRGB(&color, rgb);
  }
}

void XWImageColorMap::getGrayByteLine(quint8 *in, quint8 *out, int n)
{
	CoreColor color;
  int gray;

  if (colorSpace2) 
  {
    for (int j = 0; j < n; ++j) 
    {
      for (int i = 0; i < nComps2; ++i) 
      {
				color.c[i] = lookup2[i][in[j]];
      }
      colorSpace2->getGray(&color, &gray);
      out[j] = colToByte(gray);
    }
  } 
  else 
  {
    for (int j = 0; j < n; ++j) 
    {
      for (int i = 0; i < nComps; ++i) 
      {
				color.c[i] = lookup[i][in[j * nComps + i]];
      }
      colorSpace->getGray(&color, &gray);
      out[j] = colToByte(gray);
    }
  }
}

void XWImageColorMap::getRGBByteLine(quint8 *in, quint8 *out, int n)
{
	CoreColor color;
  CoreRGB rgb;
  int i, j;

  if (colorSpace2) 
  {
    for (j = 0; j < n; ++j) 
   {
      for (i = 0; i < nComps2; ++i) 
      {
				color.c[i] = lookup2[i][in[j]];
      }
      colorSpace2->getRGB(&color, &rgb);
      out[j*3] = colToByte(rgb.r);
      out[j*3 + 1] = colToByte(rgb.g);
      out[j*3 + 2] = colToByte(rgb.b);
    }
  } 
  else 
  {
    for (j = 0; j < n; ++j) 
    {
      for (i = 0; i < nComps; ++i) 
      {
				color.c[i] = lookup[i][in[j * nComps + i]];
      }
      colorSpace->getRGB(&color, &rgb);
      out[j*3] = colToByte(rgb.r);
      out[j*3 + 1] = colToByte(rgb.g);
      out[j*3 + 2] = colToByte(rgb.b);
    }
  }
}

void XWImageColorMap::getCMYKByteLine(quint8 *in, quint8 *out, int n)
{
	CoreColor color;
  CoreCMYK cmyk;
  int i, j;

  if (colorSpace2) 
  {
    for (j = 0; j < n; ++j) 
    {
      for (i = 0; i < nComps2; ++i) 
      {
				color.c[i] = lookup2[i][in[j]];
      }
      colorSpace2->getCMYK(&color, &cmyk);
      out[j*4] = colToByte(cmyk.c);
      out[j*4 + 1] = colToByte(cmyk.m);
      out[j*4 + 2] = colToByte(cmyk.y);
      out[j*4 + 3] = colToByte(cmyk.k);
    }
  } 
  else 
  {
    for (j = 0; j < n; ++j) 
    {
      for (i = 0; i < nComps; ++i) 
      {
				color.c[i] = lookup[i][in[j * nComps + i]];
      }
      colorSpace->getCMYK(&color, &cmyk);
      out[j*4] = colToByte(cmyk.c);
      out[j*4 + 1] = colToByte(cmyk.m);
      out[j*4 + 2] = colToByte(cmyk.y);
      out[j*4 + 3] = colToByte(cmyk.k);
    }
  }
}
