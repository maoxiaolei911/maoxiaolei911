/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#include <QtDebug>
#include <QFile>
#include <QTextStream>
#include "XWApplication.h"
#include "XWFontSetting.h"
#include "XWDocSetting.h"
#include "XWDocEncoding.h"
#include "XWList.h"
#include "XWObject.h"
#include "XWDoc.h"
#include "XWArray.h"
#include "XWDict.h"
#include "XWStream.h"
#include "XWPDFLexer.h"
#include "XWPDFParser.h"
#include "XWCoreFont.h"
#include "XWGraphixState.h"
#include "XWOutputDev.h"
#include "XWPage.h"
#include "XWAnnot.h"
#include "XWOptionalContent.h"
#include "XWGraphix.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//------------------------------------------------------------------------
// constants
//------------------------------------------------------------------------

// Max recursive depth for a function shading fill.
#define functionMaxDepth 6

// Max delta allowed in any color component for a function shading fill.
#define functionColorDelta (dblToCol(1 / 256.0))

// Max number of splits along the t axis for an axial shading fill.
#define axialMaxSplits 256

// Max delta allowed in any color component for an axial shading fill.
#define axialColorDelta (dblToCol(1 / 256.0))

// Max number of splits along the t axis for a radial shading fill.
#define radialMaxSplits 256

// Max delta allowed in any color component for a radial shading fill.
#define radialColorDelta (dblToCol(1 / 256.0))

// Max recursive depth for a Gouraud triangle shading fill.
#define gouraudMaxDepth 6

// Max delta allowed in any color component for a Gouraud triangle
// shading fill.
#define gouraudColorDelta (dblToCol(1 / 256.0))

// Max recursive depth for a patch mesh shading fill.
#define patchMaxDepth 6

// Max delta allowed in any color component for a patch mesh shading
// fill.
#define patchColorDelta (dblToCol(1 / 256.0))


#ifdef WIN32 // this works around a bug in the VC7 compiler
#  pragma optimize("",off)
#endif


Operator XWGraphix::opTab[] = {
  {"\"",  3, {tchkNum,    tchkNum,    tchkString},
          &XWGraphix::opMoveSetShowText},
  {"'",   1, {tchkString},
          &XWGraphix::opMoveShowText},
  {"B",   0, {tchkNone},
          &XWGraphix::opFillStroke},
  {"B*",  0, {tchkNone},
          &XWGraphix::opEOFillStroke},
  {"BDC", 2, {tchkName,   tchkProps},
          &XWGraphix::opBeginMarkedContent},
  {"BI",  0, {tchkNone},
          &XWGraphix::opBeginImage},
  {"BMC", 1, {tchkName},
          &XWGraphix::opBeginMarkedContent},
  {"BT",  0, {tchkNone},
          &XWGraphix::opBeginText},
  {"BX",  0, {tchkNone},
          &XWGraphix::opBeginIgnoreUndef},
  {"CS",  1, {tchkName},
          &XWGraphix::opSetStrokeColorSpace},
  {"DP",  2, {tchkName,   tchkProps},
          &XWGraphix::opMarkPoint},
  {"Do",  1, {tchkName},
          &XWGraphix::opXObject},
  {"EI",  0, {tchkNone},
          &XWGraphix::opEndImage},
  {"EMC", 0, {tchkNone},
          &XWGraphix::opEndMarkedContent},
  {"ET",  0, {tchkNone},
          &XWGraphix::opEndText},
  {"EX",  0, {tchkNone},
          &XWGraphix::opEndIgnoreUndef},
  {"F",   0, {tchkNone},
          &XWGraphix::opFill},
  {"G",   1, {tchkNum},
          &XWGraphix::opSetStrokeGray},
  {"ID",  0, {tchkNone},
          &XWGraphix::opImageData},
  {"J",   1, {tchkInt},
          &XWGraphix::opSetLineCap},
  {"K",   4, {tchkNum,    tchkNum,    tchkNum,    tchkNum},
          &XWGraphix::opSetStrokeCMYKColor},
  {"M",   1, {tchkNum},
          &XWGraphix::opSetMiterLimit},
  {"MP",  1, {tchkName},
          &XWGraphix::opMarkPoint},
  {"Q",   0, {tchkNone},
          &XWGraphix::opRestore},
  {"RG",  3, {tchkNum,    tchkNum,    tchkNum},
          &XWGraphix::opSetStrokeRGBColor},
  {"S",   0, {tchkNone},
          &XWGraphix::opStroke},
  {"SC",  -4, {tchkNum,   tchkNum,    tchkNum,    tchkNum},
          &XWGraphix::opSetStrokeColor},
  {"SCN", -33, {tchkSCN,   tchkSCN,    tchkSCN,    tchkSCN,
	        tchkSCN,   tchkSCN,    tchkSCN,    tchkSCN,
	        tchkSCN,   tchkSCN,    tchkSCN,    tchkSCN,
	        tchkSCN,   tchkSCN,    tchkSCN,    tchkSCN,
	        tchkSCN,   tchkSCN,    tchkSCN,    tchkSCN,
	        tchkSCN,   tchkSCN,    tchkSCN,    tchkSCN,
	        tchkSCN,   tchkSCN,    tchkSCN,    tchkSCN,
	        tchkSCN,   tchkSCN,    tchkSCN,    tchkSCN,
	        tchkSCN},
          &XWGraphix::opSetStrokeColorN},
  {"T*",  0, {tchkNone},
          &XWGraphix::opTextNextLine},
  {"TD",  2, {tchkNum,    tchkNum},
          &XWGraphix::opTextMoveSet},
  {"TJ",  1, {tchkArray},
          &XWGraphix::opShowSpaceText},
  {"TL",  1, {tchkNum},
          &XWGraphix::opSetTextLeading},
  {"Tc",  1, {tchkNum},
          &XWGraphix::opSetCharSpacing},
  {"Td",  2, {tchkNum,    tchkNum},
          &XWGraphix::opTextMove},
  {"Tf",  2, {tchkName,   tchkNum},
          &XWGraphix::opSetFont},
  {"Tj",  1, {tchkString},
          &XWGraphix::opShowText},
  {"Tm",  6, {tchkNum,    tchkNum,    tchkNum,    tchkNum,
	      tchkNum,    tchkNum},
          &XWGraphix::opSetTextMatrix},
  {"Tr",  1, {tchkInt},
          &XWGraphix::opSetTextRender},
  {"Ts",  1, {tchkNum},
          &XWGraphix::opSetTextRise},
  {"Tw",  1, {tchkNum},
          &XWGraphix::opSetWordSpacing},
  {"Tz",  1, {tchkNum},
          &XWGraphix::opSetHorizScaling},
  {"W",   0, {tchkNone},
          &XWGraphix::opClip},
  {"W*",  0, {tchkNone},
          &XWGraphix::opEOClip},
  {"b",   0, {tchkNone},
          &XWGraphix::opCloseFillStroke},
  {"b*",  0, {tchkNone},
          &XWGraphix::opCloseEOFillStroke},
  {"c",   6, {tchkNum,    tchkNum,    tchkNum,    tchkNum,
	      tchkNum,    tchkNum},
          &XWGraphix::opCurveTo},
  {"cm",  6, {tchkNum,    tchkNum,    tchkNum,    tchkNum,
	      tchkNum,    tchkNum},
          &XWGraphix::opConcat},
  {"cs",  1, {tchkName},
          &XWGraphix::opSetFillColorSpace},
  {"d",   2, {tchkArray,  tchkNum},
          &XWGraphix::opSetDash},
  {"d0",  2, {tchkNum,    tchkNum},
          &XWGraphix::opSetCharWidth},
  {"d1",  6, {tchkNum,    tchkNum,    tchkNum,    tchkNum,
	      tchkNum,    tchkNum},
          &XWGraphix::opSetCacheDevice},
  {"f",   0, {tchkNone},
          &XWGraphix::opFill},
  {"f*",  0, {tchkNone},
          &XWGraphix::opEOFill},
  {"g",   1, {tchkNum},
          &XWGraphix::opSetFillGray},
  {"gs",  1, {tchkName},
          &XWGraphix::opSetExtGState},
  {"h",   0, {tchkNone},
          &XWGraphix::opClosePath},
  {"i",   1, {tchkNum},
          &XWGraphix::opSetFlat},
  {"j",   1, {tchkInt},
          &XWGraphix::opSetLineJoin},
  {"k",   4, {tchkNum,    tchkNum,    tchkNum,    tchkNum},
          &XWGraphix::opSetFillCMYKColor},
  {"l",   2, {tchkNum,    tchkNum},
          &XWGraphix::opLineTo},
  {"m",   2, {tchkNum,    tchkNum},
          &XWGraphix::opMoveTo},
  {"n",   0, {tchkNone},
          &XWGraphix::opEndPath},
  {"q",   0, {tchkNone},
          &XWGraphix::opSave},
  {"re",  4, {tchkNum,    tchkNum,    tchkNum,    tchkNum},
          &XWGraphix::opRectangle},
  {"rg",  3, {tchkNum,    tchkNum,    tchkNum},
          &XWGraphix::opSetFillRGBColor},
  {"ri",  1, {tchkName},
          &XWGraphix::opSetRenderingIntent},
  {"s",   0, {tchkNone},
          &XWGraphix::opCloseStroke},
  {"sc",  -4, {tchkNum,   tchkNum,    tchkNum,    tchkNum},
          &XWGraphix::opSetFillColor},
  {"scn", -33, {tchkSCN,   tchkSCN,    tchkSCN,    tchkSCN,
	        tchkSCN,   tchkSCN,    tchkSCN,    tchkSCN,
	        tchkSCN,   tchkSCN,    tchkSCN,    tchkSCN,
	        tchkSCN,   tchkSCN,    tchkSCN,    tchkSCN,
	        tchkSCN,   tchkSCN,    tchkSCN,    tchkSCN,
	        tchkSCN,   tchkSCN,    tchkSCN,    tchkSCN,
	        tchkSCN,   tchkSCN,    tchkSCN,    tchkSCN,
	        tchkSCN,   tchkSCN,    tchkSCN,    tchkSCN,
	        tchkSCN},
          &XWGraphix::opSetFillColorN},
  {"sh",  1, {tchkName},
          &XWGraphix::opShFill},
  {"v",   4, {tchkNum,    tchkNum,    tchkNum,    tchkNum},
          &XWGraphix::opCurveTo1},
  {"w",   1, {tchkNum},
          &XWGraphix::opSetLineWidth},
  {"y",   4, {tchkNum,    tchkNum,    tchkNum,    tchkNum},
          &XWGraphix::opCurveTo2},
};


#ifdef WIN32 // this works around a bug in the VC7 compiler
#  pragma optimize("",on)
#endif

#define numOps (sizeof(opTab) / sizeof(Operator))

XWGraphixResources::XWGraphixResources(XWRef *xref, 
                                       XWDict *resDict, 
                                       XWGraphixResources *nextA)
{
	XWObject obj1, obj2;
  ObjRef r;

  if (resDict) {

    // build font dictionary
    fonts = NULL;
    resDict->lookupNF("Font", &obj1);
    if (obj1.isRef()) {
      obj1.fetch(xref, &obj2);
      if (obj2.isDict()) {
	r = obj1.getRef();
	fonts = new XWCoreFontDict(xref, &r, obj2.getDict());
      }
      obj2.free();
    } else if (obj1.isDict()) {
      fonts = new XWCoreFontDict(xref, NULL, obj1.getDict());
    }
    obj1.free();

    // get XObject dictionary
    resDict->lookup("XObject", &xObjDict);

    // get color space dictionary
    resDict->lookup("ColorSpace", &colorSpaceDict);

    // get pattern dictionary
    resDict->lookup("Pattern", &patternDict);

    // get shading dictionary
    resDict->lookup("Shading", &shadingDict);

    // get graphics state parameter dictionary
    resDict->lookup("ExtGState", &gStateDict);

    // get properties dictionary
    resDict->lookup("Properties", &propsDict);

  } else {
    fonts = NULL;
    xObjDict.initNull();
    colorSpaceDict.initNull();
    patternDict.initNull();
    shadingDict.initNull();
    gStateDict.initNull();
    propsDict.initNull();
  }

  next = nextA;
}

XWGraphixResources::~XWGraphixResources()
{
	if (fonts) {
    delete fonts;
  }
  xObjDict.free();
  colorSpaceDict.free();
  patternDict.free();
  shadingDict.free();
  gStateDict.free();
  propsDict.free();
}

void XWGraphixResources::lookupColorSpace(char *name, XWObject *obj)
{
  XWGraphixResources *resPtr;

  for (resPtr = this; resPtr; resPtr = resPtr->next) {
    if (resPtr->colorSpaceDict.isDict()) {
      if (!resPtr->colorSpaceDict.dictLookup(name, obj)->isNull()) {
	return;
      }
      obj->free();
    }
  }
  obj->initNull();
}

XWCoreFont * XWGraphixResources::lookupFont(char *name)
{
  XWCoreFont *font;
  XWGraphixResources *resPtr;

  for (resPtr = this; resPtr; resPtr = resPtr->next) {
    if (resPtr->fonts) {
      if ((font = resPtr->fonts->lookup(name)))
	return font;
    }
  }
  xwApp->error("Unknown font tag '{0:s}'");
  return NULL;
}

bool XWGraphixResources::lookupGState(char *name, XWObject *obj)
{
  XWGraphixResources *resPtr;

  for (resPtr = this; resPtr; resPtr = resPtr->next) {
    if (resPtr->gStateDict.isDict()) {
      if (!resPtr->gStateDict.dictLookup(name, obj)->isNull()) {
	return true;
      }
      obj->free();
    }
  }
  xwApp->error("ExtGState '{0:s}' is unknown");
  return false;
}

XWPattern  * XWGraphixResources::lookupPattern(char *name)
{
  XWGraphixResources *resPtr;
  XWPattern *pattern;
  XWObject obj;

  for (resPtr = this; resPtr; resPtr = resPtr->next) {
    if (resPtr->patternDict.isDict()) {
      if (!resPtr->patternDict.dictLookup(name, &obj)->isNull()) {
	pattern = XWPattern::parse(&obj);
	obj.free();
	return pattern;
      }
      obj.free();
    }
  }
  xwApp->error("Unknown pattern '{0:s}'");
  return NULL;
}

bool  XWGraphixResources::lookupPropertiesNF(char *name, XWObject *obj)
{
	XWGraphixResources *resPtr;

  for (resPtr = this; resPtr; resPtr = resPtr->next) {
    if (resPtr->propsDict.isDict()) {
      if (!resPtr->propsDict.dictLookupNF(name, obj)->isNull()) {
	return true;
      }
      obj->free();
    }
  }
  xwApp->error("Properties '{0:s}' is unknown");
  return false;
}

XWShading  * XWGraphixResources::lookupShading(char *name)
{
  XWGraphixResources *resPtr;
  XWShading *shading;
  XWObject obj;

  for (resPtr = this; resPtr; resPtr = resPtr->next) {
    if (resPtr->shadingDict.isDict()) {
      if (!resPtr->shadingDict.dictLookup(name, &obj)->isNull()) {
	shading = XWShading::parse(&obj);
	obj.free();
	return shading;
      }
      obj.free();
    }
  }
  xwApp->error("Unknown shading '{0:s}'");
  return NULL;
}

bool XWGraphixResources::lookupXObject(char *name, XWObject *obj)
{
  XWGraphixResources *resPtr;

  for (resPtr = this; resPtr; resPtr = resPtr->next) {
    if (resPtr->xObjDict.isDict()) {
      if (!resPtr->xObjDict.dictLookup(name, obj)->isNull())
	return true;
      obj->free();
    }
  }
  xwApp->error("XObject '{0:s}' is unknown");
  return false;
}

bool XWGraphixResources::lookupXObjectNF(char *name, XWObject *obj)
{
  XWGraphixResources *resPtr;

  for (resPtr = this; resPtr; resPtr = resPtr->next) {
    if (resPtr->xObjDict.isDict()) {
      if (!resPtr->xObjDict.dictLookupNF(name, obj)->isNull())
	return true;
      obj->free();
    }
  }
  xwApp->error("XObject '{0:s}' is unknown");
  return false;
}

XWGraphix::XWGraphix(XWDoc *docA, 
                     XWOutputDev *outA, 
                     int pageNum, 
                     XWDict *resDict,
                     double hDPI, 
                     double vDPI, 
                     XWPDFRectangle *box,
                     XWPDFRectangle *cropBox, 
                     int rotate,
                     bool (*abortCheckCbkA)(void *data),
                     void *abortCheckCbkDataA)
{
  int i;

  doc = docA;
  xref = doc->getXRef();
  subPage = false;
  printCommands = false;

  // start the resource stack
  res = new XWGraphixResources(xref, resDict, NULL);

  // initialize
  out = outA;
  state = new XWGraphixState(hDPI, vDPI, box, rotate, out->upsideDown());
  fontChanged = false;
  clip = clipNone;
  ignoreUndef = 0;
  out->startPage(pageNum, state);
  out->setDefaultCTM(state->getCTM());
  out->updateAll(state);
  for (i = 0; i < 6; ++i) {
    baseMatrix[i] = state->getCTM()[i];
  }
  formDepth = 0;
  textClipBBoxEmpty = true;
  markedContentStack = new XWList();
  ocState = true;
  parser = NULL;
  abortCheckCbk = abortCheckCbkA;
  abortCheckCbkData = abortCheckCbkDataA;

  // set crop box
  if (cropBox) {
    state->moveTo(cropBox->x1, cropBox->y1);
    state->lineTo(cropBox->x2, cropBox->y1);
    state->lineTo(cropBox->x2, cropBox->y2);
    state->lineTo(cropBox->x1, cropBox->y2);
    state->closePath();
    state->clip();
    out->clip(state);
    state->clearPath();
  }
}

XWGraphix::XWGraphix(XWDoc *docA, 
                       XWOutputDev *outA, 
                       XWDict *resDict,
                       XWPDFRectangle *box, 
                       XWPDFRectangle *cropBox,
                       bool (*abortCheckCbkA)(void *data),
                       void *abortCheckCbkDataA)
{
  int i;

  doc = docA;
  xref = doc->getXRef();
  subPage = true;
  printCommands = false;

  // start the resource stack
  res = new XWGraphixResources(xref, resDict, NULL);

  // initialize
  out = outA;
  state = new XWGraphixState(72, 72, box, 0, false);
  fontChanged = false;
  clip = clipNone;
  ignoreUndef = 0;
  for (i = 0; i < 6; ++i) {
    baseMatrix[i] = state->getCTM()[i];
  }
  formDepth = 0;
  textClipBBoxEmpty = true;
  markedContentStack = new XWList();
  ocState = true;
  parser = NULL;
  abortCheckCbk = abortCheckCbkA;
  abortCheckCbkData = abortCheckCbkDataA;

  // set crop box
  if (cropBox) {
    state->moveTo(cropBox->x1, cropBox->y1);
    state->lineTo(cropBox->x2, cropBox->y1);
    state->lineTo(cropBox->x2, cropBox->y2);
    state->lineTo(cropBox->x1, cropBox->y2);
    state->closePath();
    state->clip();
    out->clip(state);
    state->clearPath();
  }
}

XWGraphix::~XWGraphix()
{
  if (!subPage) {
    out->endPage();
  }
  while (state->hasSaves()) {
    restoreState();
  }
  delete state;
  while (res) {
    popResources();
  }
  deleteXWList(markedContentStack, XWMarkedContent);
}

void XWGraphix::display(XWObject *obj, bool topLevel)
{
  XWObject obj2;
  int i;

  if (obj->isArray()) {
    for (i = 0; i < obj->arrayGetLength(); ++i) {
      obj->arrayGet(i, &obj2);
      if (!obj2.isStream()) {
	xwApp->error("Weird page contents");
	obj2.free();
	return;
      }
      obj2.free();
    }
  } else if (!obj->isStream()) {
    xwApp->error("Weird page contents");
    return;
  }
  parser = new XWPDFParser(xref, new XWPDFLexer(xref, obj), false);
  go(topLevel);
  delete parser;
  parser = NULL;
}

void XWGraphix::drawAnnot(XWObject *str, 
                          XWAnnotBorderStyle *borderStyle,
		                  double xMin, 
		                  double yMin, 
		                  double xMax, 
		                  double yMax)
{
  XWDict *dict, *resDict;
  XWObject matrixObj, bboxObj, resObj, obj1;
  double formXMin, formYMin, formXMax, formYMax;
  double x, y, sx, sy, tx, ty;
  double m[6], bbox[4];
  double r, g, b;
  CoreColor color;
  double *dash, *dash2;
  int dashLength;
  int i;

  // this function assumes that we are in the default user space,
  // i.e., baseMatrix = ctm

  // if the bounding box has zero width or height, don't draw anything
  // at all
  if (xMin == xMax || yMin == yMax) 
  {
    return;
  }

  // draw the appearance stream (if there is one)
  if (str->isStream()) 
  {

    // get stream dict
    dict = str->streamGetDict();

    // get the form bounding box
    dict->lookup("BBox", &bboxObj);
    if (!bboxObj.isArray()) 
    {
      bboxObj.free();
      xwApp->error("Bad form bounding box");
      return;
    }
    for (i = 0; i < 4; ++i) 
   	{
      bboxObj.arrayGet(i, &obj1);
      bbox[i] = obj1.getNum();
      obj1.free();
    }
    bboxObj.free();

    // get the form matrix
    dict->lookup("Matrix", &matrixObj);
    if (matrixObj.isArray()) 
    {
      for (i = 0; i < 6; ++i) 
      {
				matrixObj.arrayGet(i, &obj1);
				m[i] = obj1.getNum();
				obj1.free();
      }
    } 
    else 
    {
      m[0] = 1; m[1] = 0;
      m[2] = 0; m[3] = 1;
      m[4] = 0; m[5] = 0;
    }
    matrixObj.free();

    // transform the four corners of the form bbox to default user
    // space, and construct the transformed bbox
    x = bbox[0] * m[0] + bbox[1] * m[2] + m[4];
    y = bbox[0] * m[1] + bbox[1] * m[3] + m[5];
    formXMin = formXMax = x;
    formYMin = formYMax = y;
    x = bbox[0] * m[0] + bbox[3] * m[2] + m[4];
    y = bbox[0] * m[1] + bbox[3] * m[3] + m[5];
    if (x < formXMin) 
    {
      formXMin = x;
    } 
    else if (x > formXMax) 
    {
      formXMax = x;
    }
    if (y < formYMin) 
    {
      formYMin = y;
    } 
    else if (y > formYMax) 
    {
      formYMax = y;
    }
    x = bbox[2] * m[0] + bbox[1] * m[2] + m[4];
    y = bbox[2] * m[1] + bbox[1] * m[3] + m[5];
    if (x < formXMin) 
    {
      formXMin = x;
    } 
    else if (x > formXMax) 
    {
      formXMax = x;
    }
    if (y < formYMin) 
    {
      formYMin = y;
    } 
    else if (y > formYMax) 
    {
      formYMax = y;
    }
    x = bbox[2] * m[0] + bbox[3] * m[2] + m[4];
    y = bbox[2] * m[1] + bbox[3] * m[3] + m[5];
    if (x < formXMin) 
    {
      formXMin = x;
    } 
    else if (x > formXMax) 
    {
      formXMax = x;
    }
    if (y < formYMin) 
    {
      formYMin = y;
    } 
    else if (y > formYMax) 
    {
      formYMax = y;
    }

    // construct a mapping matrix, [sx 0  0], which maps the transformed
    //                             [0  sy 0]
    //                             [tx ty 1]
    // bbox to the annotation rectangle
    if (formXMin == formXMax) 
    {
      // this shouldn't happen
      sx = 1;
    } 
    else 
    {
      sx = (xMax - xMin) / (formXMax - formXMin);
    }
    if (formYMin == formYMax) 
    {
      // this shouldn't happen
      sy = 1;
    } 
    else 
    {
      sy = (yMax - yMin) / (formYMax - formYMin);
    }
    tx = -formXMin * sx + xMin;
    ty = -formYMin * sy + yMin;

    // the final transform matrix is (form matrix) * (mapping matrix)
    m[0] *= sx;
    m[1] *= sy;
    m[2] *= sx;
    m[3] *= sy;
    m[4] = m[4] * sx + tx;
    m[5] = m[5] * sy + ty;

    // get the resources
    dict->lookup("Resources", &resObj);
    resDict = resObj.isDict() ? resObj.getDict() : (XWDict *)NULL;

    // draw it
    drawForm(str, resDict, m, bbox);

    resObj.free();
  }

  // draw the border
  if (borderStyle && borderStyle->getWidth() > 0) 
  {
    if (state->getStrokeColorSpace()->getMode() != COLOR_SPACE_DEVICERGB) 
    {
      state->setStrokePattern(NULL);
      state->setStrokeColorSpace(new XWDeviceRGBColorSpace());
      out->updateStrokeColorSpace(state);
    }
    borderStyle->getColor(&r, &g, &b);
    color.c[0] = dblToCol(r);
    color.c[1] = dblToCol(g);
    color.c[2] = dblToCol(b);
    state->setStrokeColor(&color);
    out->updateStrokeColor(state);
    state->setLineWidth(borderStyle->getWidth());
    out->updateLineWidth(state);
    borderStyle->getDash(&dash, &dashLength);
    if (borderStyle->getType() == annotBorderDashed && dashLength > 0) 
    {
      dash2 = (double *)malloc(dashLength * sizeof(double));
      memcpy(dash2, dash, dashLength * sizeof(double));
      state->setLineDash(dash2, dashLength, 0);
      out->updateLineDash(state);
    }
    //~ this doesn't currently handle the beveled and engraved styles
    state->clearPath();
    state->moveTo(xMin, yMin);
    state->lineTo(xMax, yMin);
    if (borderStyle->getType() != annotBorderUnderlined) 
    {
      state->lineTo(xMax, yMax);
      state->lineTo(xMin, yMax);
      state->closePath();
    }
    out->stroke(state);
  }
}

void XWGraphix::drawForm(XWObject *str, 
		                     XWDict *resDict, 
		                     double *matrix, 
		                     double *bbox,
									       bool transpGroup, 
									       bool softMask,
									       XWColorSpace *blendingColorSpace,
									       bool isolated, 
									       bool knockout,
									       bool alpha, 
									       XWFunction *transferFunc,
									       CoreColor *backdropColor)
{
	XWPDFParser *oldParser;
  XWGraphixState *savedState;
  double oldBaseMatrix[6];
  int i;

  // push new resources on stack
  pushResources(resDict);

  // save current graphics state
  savedState = saveStateStack();

  // kill any pre-existing path
  state->clearPath();

  // save current parser
  oldParser = parser;

  // set form transformation matrix
  state->concatCTM(matrix[0], matrix[1], matrix[2], matrix[3], matrix[4], matrix[5]);
  out->updateCTM(state, matrix[0], matrix[1], matrix[2], matrix[3], matrix[4], matrix[5]);

  // set form bounding box
  state->moveTo(bbox[0], bbox[1]);
  state->lineTo(bbox[2], bbox[1]);
  state->lineTo(bbox[2], bbox[3]);
  state->lineTo(bbox[0], bbox[3]);
  state->closePath();
  state->clip();
  out->clip(state);
  state->clearPath();

  if (softMask || transpGroup) 
  {
    if (state->getBlendMode() != BLEND_NORMAL) 
    {
      state->setBlendMode(BLEND_NORMAL);
      out->updateBlendMode(state);
    }
    if (state->getFillOpacity() != 1) 
    {
      state->setFillOpacity(1);
      out->updateFillOpacity(state);
    }
    if (state->getStrokeOpacity() != 1) 
    {
      state->setStrokeOpacity(1);
      out->updateStrokeOpacity(state);
    }
    out->clearSoftMask(state);
    out->beginTransparencyGroup(state, bbox, blendingColorSpace, isolated, knockout, softMask);
  }

  // set new base matrix
  for (i = 0; i < 6; ++i) 
  {
    oldBaseMatrix[i] = baseMatrix[i];
    baseMatrix[i] = state->getCTM()[i];
  }

  // draw the form
  display(str, false);

  if (softMask || transpGroup) 
  {
    out->endTransparencyGroup(state);
  }

  // restore base matrix
  for (i = 0; i < 6; ++i) 
 	{
    baseMatrix[i] = oldBaseMatrix[i];
  }

  // restore parser
  parser = oldParser;

  // restore graphics state
  restoreStateStack(savedState);

  // pop resource stack
  popResources();

  if (softMask) 
  {
    out->setSoftMask(state, bbox, alpha, transferFunc, backdropColor);
  } 
  else if (transpGroup) 
  {
    out->paintTransparencyGroup(state, bbox);
  }

  return;
}

void XWGraphix::restoreState()
{
    state = state->restore();
    out->restoreState(state);
}

void XWGraphix::saveState()
{
    out->saveState(state);
    state = state->save();
}

XWStream * XWGraphix::buildImageStream()
{
  XWObject dict;
  XWObject obj;
  char *key;
  XWStream *str;

  // build dictionary
  dict.initDict(xref);
  parser->getObj(&obj);
  while (!obj.isCmd("ID") && !obj.isEOF()) {
    if (!obj.isName()) {
      xwApp->error("Inline image dictionary key must be a name object");
      obj.free();
    } else {
      key = qstrdup(obj.getName());
      obj.free();
      parser->getObj(&obj);
      if (obj.isEOF() || obj.isError()) {
	delete [] (key);
	break;
      }
      dict.dictAdd(key, &obj);
    }
    parser->getObj(&obj);
  }
  if (obj.isEOF()) {
    xwApp->error("End of file in inline image");
    obj.free();
    dict.free();
    return NULL;
  }
  obj.free();

  // make stream
  if (!(str = parser->getStream())) {
    xwApp->error("Invalid inline image data");
    dict.free();
    return NULL;
  }
  str = new XWEmbedStream(str, &dict, false, 0);
  str = str->addFilters(&dict);

  return str;
}

bool XWGraphix::checkArg(XWObject *arg, TchkType type)
{
  switch (type) {
  case tchkBool:   return arg->isBool();
  case tchkInt:    return arg->isInt();
  case tchkNum:    return arg->isNum();
  case tchkString: return arg->isString();
  case tchkName:   return arg->isName();
  case tchkArray:  return arg->isArray();
  case tchkProps:  return arg->isDict() || arg->isName();
  case tchkSCN:    return arg->isNum() || arg->isName();
  case tchkNone:   return false;
  }
  return false;
}

void XWGraphix::doAxialShFill(XWAxialShading *shading)
{    
  double xMin, yMin, xMax, yMax;
  double x0, y0, x1, y1;
  double dx, dy, mul;
  bool dxdyZero, horiz;
  double tMin, tMax, t, tx, ty;
  double sMin, sMax, tmp;
  double ux0, uy0, ux1, uy1, vx0, vy0, vx1, vy1;
  double t0, t1, tt;
  double ta[axialMaxSplits + 1];
  int next[axialMaxSplits + 1];
  CoreColor color0, color1;
  int nComps;
  int i, j, k;

  if (out->useShadedFills() && 
  	  out->axialShadedFill(state, shading)) 
  {
    return;
  }

  // get the clip region bbox
  state->getUserClipBBox(&xMin, &yMin, &xMax, &yMax);

  // compute min and max t values, based on the four corners of the
  // clip region bbox
  shading->getCoords(&x0, &y0, &x1, &y1);
  dx = x1 - x0;
  dy = y1 - y0;
  dxdyZero = fabs(dx) < 0.01 && fabs(dy) < 0.01;
  horiz = fabs(dy) < fabs(dx);
  if (dxdyZero) 
  {
    tMin = tMax = 0;
  } 
  else 
  {
    mul = 1 / (dx * dx + dy * dy);
    tMin = tMax = ((xMin - x0) * dx + (yMin - y0) * dy) * mul;
    t = ((xMin - x0) * dx + (yMax - y0) * dy) * mul;
    if (t < tMin) 
    {
      tMin = t;
    } 
    else if (t > tMax) 
    {
      tMax = t;
    }
    t = ((xMax - x0) * dx + (yMin - y0) * dy) * mul;
    if (t < tMin) 
    {
      tMin = t;
    } 
    else if (t > tMax) 
    {
      tMax = t;
    }
    t = ((xMax - x0) * dx + (yMax - y0) * dy) * mul;
    if (t < tMin) 
    {
      tMin = t;
    } 
    else if (t > tMax) 
    {
      tMax = t;
    }
    if (tMin < 0 && !shading->getExtend0()) 
    {
      tMin = 0;
    }
    if (tMax > 1 && !shading->getExtend1()) 
    {
      tMax = 1;
    }
  }

  // get the function domain
  t0 = shading->getDomain0();
  t1 = shading->getDomain1();

  // Traverse the t axis and do the shading.
  //
  // For each point (tx, ty) on the t axis, consider a line through
  // that point perpendicular to the t axis:
  //
  //     x(s) = tx + s * -dy   -->   s = (x - tx) / -dy
  //     y(s) = ty + s * dx    -->   s = (y - ty) / dx
  //
  // Then look at the intersection of this line with the bounding box
  // (xMin, yMin, xMax, yMax).  For -1 < |dy/dx| < 1, look at the
  // intersection with yMin, yMax:
  //
  //     s0 = (yMin - ty) / dx
  //     s1 = (yMax - ty) / dx
  //
  // else look at the intersection with xMin, xMax:
  //
  //     s0 = (xMin - tx) / -dy
  //     s1 = (xMax - tx) / -dy
  //
  // Each filled polygon is bounded by two of these line segments
  // perpdendicular to the t axis.
  //
  // The t axis is bisected into smaller regions until the color
  // difference across a region is small enough, and then the region
  // is painted with a single color.

  // set up
  nComps = shading->getColorSpace()->getNComps();
  ta[0] = tMin;
  next[0] = axialMaxSplits;
  ta[axialMaxSplits] = tMax;

  // compute the color at t = tMin
  if (tMin < 0) 
  {
    tt = t0;
  } 
  else if (tMin > 1) 
  {
    tt = t1;
  } 
  else 
  {
    tt = t0 + (t1 - t0) * tMin;
  }
  shading->getColor(tt, &color0);

  // compute the coordinates of the point on the t axis at t = tMin;
  // then compute the intersection of the perpendicular line with the
  // bounding box
  tx = x0 + tMin * dx;
  ty = y0 + tMin * dy;
  if (dxdyZero) 
  {
    sMin = sMax = 0;
  } 
  else 
  {
    if (horiz) 
    {
      sMin = (yMin - ty) / dx;
      sMax = (yMax - ty) / dx;
    } 
    else 
    {
      sMin = (xMin - tx) / -dy;
      sMax = (xMax - tx) / -dy;
    }
    if (sMin > sMax) 
    {
      tmp = sMin; sMin = sMax; sMax = tmp;
    }
  }
  ux0 = tx - sMin * dy;
  uy0 = ty + sMin * dx;
  vx0 = tx - sMax * dy;
  vy0 = ty + sMax * dx;

  i = 0;
  while (i < axialMaxSplits) 
  {

    // bisect until color difference is small enough or we hit the
    // bisection limit
    j = next[i];
    while (j > i + 1) 
    {
      if (ta[j] < 0) 
      {
				tt = t0;
      } 
      else if (ta[j] > 1) 
      {
				tt = t1;
      } 
      else 
      {
				tt = t0 + (t1 - t0) * ta[j];
      }
      // require at least two splits (to avoid problems where the
      // color doesn't change smoothly along the t axis)
      if (j - i <= axialMaxSplits / 4) 
      {
				shading->getColor(tt, &color1);
				for (k = 0; k < nComps; ++k) 
				{
	  			if (abs(color1.c[k] - color0.c[k]) > axialColorDelta) 
	  			{
	    			break;
	  			}
				}
				if (k == nComps) 
				{
	  			break;
				}
      }
      k = (i + j) / 2;
      ta[k] = 0.5 * (ta[i] + ta[j]);
      next[i] = k;
      next[k] = j;
      j = k;
    }

    // use the average of the colors of the two sides of the region
    for (k = 0; k < nComps; ++k) 
    {
      color0.c[k] = (color0.c[k] + color1.c[k]) / 2;
    }

    // compute the coordinates of the point on the t axis; then
    // compute the intersection of the perpendicular line with the
    // bounding box
    tx = x0 + ta[j] * dx;
    ty = y0 + ta[j] * dy;
    if (dxdyZero) 
    {
      sMin = sMax = 0;
    } 
    else 
    {
      if (horiz) 
      {
				sMin = (yMin - ty) / dx;
				sMax = (yMax - ty) / dx;
      } 
      else 
      {
				sMin = (xMin - tx) / -dy;
				sMax = (xMax - tx) / -dy;
      }
      if (sMin > sMax) 
      {
				tmp = sMin; sMin = sMax; sMax = tmp;
      }
    }
    ux1 = tx - sMin * dy;
    uy1 = ty + sMin * dx;
    vx1 = tx - sMax * dy;
    vy1 = ty + sMax * dx;

    // set the color
    state->setFillColor(&color0);
    out->updateFillColor(state);

    // fill the region
    state->moveTo(ux0, uy0);
    state->lineTo(vx0, vy0);
    state->lineTo(vx1, vy1);
    state->lineTo(ux1, uy1);
    state->closePath();
    out->fill(state);
    state->clearPath();

    // set up for next region
    ux0 = ux1;
    uy0 = uy1;
    vx0 = vx1;
    vy0 = vy1;
    color0 = color1;
    i = next[i];
  }
}

void XWGraphix::doEndPath()
{
  if (state->isCurPt() && clip != clipNone) {
    state->clip();
    if (clip == clipNormal) {
      out->clip(state);
    } else {
      out->eoClip(state);
    }
  }
  clip = clipNone;
  state->clearPath();
}

void XWGraphix::doForm(XWObject *str)
{
  XWDict *dict;
  bool transpGroup, isolated, knockout;
  XWColorSpace *blendingColorSpace;
  XWObject matrixObj, bboxObj;
  double m[6], bbox[4];
  XWObject resObj;
  XWDict *resDict;
  bool oc, ocSaved;
  XWObject obj1, obj2, obj3;
  int i;

  // check for excessive recursion
  if (formDepth > 100) 
  {
    return;
  }

  // get stream dict
  dict = str->streamGetDict();

  // check form type
  dict->lookup("FormType", &obj1);
  if (!(obj1.isNull() || (obj1.isInt() && obj1.getInt() == 1))) 
  {
    xwApp->error("Unknown form type");
  }
  obj1.free();

  // check for optional content key
  ocSaved = ocState;
  dict->lookupNF("OC", &obj1);
  if (doc->getOptionalContent()->evalOCObject(&obj1, &oc) && !oc) 
  {
    obj1.free();
    if (out->needCharCount()) 
    {
      ocState = false;
    } 
    else 
    {
      return;
    }
  }
  obj1.free();

  // get bounding box
  dict->lookup("BBox", &bboxObj);
  if (!bboxObj.isArray()) 
  {
    bboxObj.free();
    ocState = ocSaved;
    return;
  }
  for (i = 0; i < 4; ++i) 
  {
    bboxObj.arrayGet(i, &obj1);
    bbox[i] = obj1.getNum();
    obj1.free();
  }
  bboxObj.free();

  // get matrix
  dict->lookup("Matrix", &matrixObj);
  if (matrixObj.isArray()) 
  {
    for (i = 0; i < 6; ++i) 
    {
      matrixObj.arrayGet(i, &obj1);
      m[i] = obj1.getNum();
      obj1.free();
    }
  } 
  else 
  {
    m[0] = 1; m[1] = 0;
    m[2] = 0; m[3] = 1;
    m[4] = 0; m[5] = 0;
  }
  matrixObj.free();

  // get resources
  dict->lookup("Resources", &resObj);
  resDict = resObj.isDict() ? resObj.getDict() : (XWDict *)NULL;

  // check for a transparency group
  transpGroup = isolated = knockout = false;
  blendingColorSpace = NULL;
  if (dict->lookup("Group", &obj1)->isDict()) 
  {
    if (obj1.dictLookup("S", &obj2)->isName("Transparency")) 
    {
      transpGroup = true;
      if (!obj1.dictLookup("CS", &obj3)->isNull()) 
      {
				blendingColorSpace = XWColorSpace::parse(&obj3);
      }
      obj3.free();
      if (obj1.dictLookup("I", &obj3)->isBool()) 
      {
				isolated = obj3.getBool();
      }
      obj3.free();
      if (obj1.dictLookup("K", &obj3)->isBool()) 
      {
				knockout = obj3.getBool();
      }
      obj3.free();
    }
    obj2.free();
  }
  obj1.free();

  // draw it
  ++formDepth;
  drawForm(str, resDict, m, bbox, transpGroup, false, blendingColorSpace, isolated, knockout);
  --formDepth;

  if (blendingColorSpace) 
  {
    delete blendingColorSpace;
  }
  resObj.free();

  ocState = ocSaved;
}

void XWGraphix::doForm1(XWObject *str, 
                        XWDict *resDict, 
                        double *matrix, 
                        double *bbox,
	                    bool transpGroup, 
	                    bool softMask,
	                    XWColorSpace *blendingColorSpace,
	                    bool isolated, 
	                    bool knockout,
	                    bool alpha, 
	                    XWFunction *transferFunc,
	                    CoreColor *backdropColor)
{
    pushResources(resDict);
    saveState();
    state->clearPath();
    XWPDFParser * oldParser = parser;
    state->concatCTM(matrix[0], matrix[1], matrix[2],
		             matrix[3], matrix[4], matrix[5]);
    out->updateCTM(state, matrix[0], matrix[1], matrix[2],
		            matrix[3], matrix[4], matrix[5]);
		            
    state->moveTo(bbox[0], bbox[1]);
    state->lineTo(bbox[2], bbox[1]);
    state->lineTo(bbox[2], bbox[3]);
    state->lineTo(bbox[0], bbox[3]);
    state->closePath();
    state->clip();
    out->clip(state);
    state->clearPath();
    
    if (softMask || transpGroup) 
    {
        if (state->getBlendMode() != BLEND_NORMAL) 
        {
            state->setBlendMode(BLEND_NORMAL);
            out->updateBlendMode(state);
        }
        
        if (state->getFillOpacity() != 1) 
        {
            state->setFillOpacity(1);
            out->updateFillOpacity(state);
        }
        
        if (state->getStrokeOpacity() != 1) 
        {
            state->setStrokeOpacity(1);
            out->updateStrokeOpacity(state);
        }
        out->clearSoftMask(state);
        out->beginTransparencyGroup(state, bbox, blendingColorSpace, isolated, knockout, softMask);
    }
    
    double oldBaseMatrix[6];
    for (int i = 0; i < 6; ++i) 
    {
        oldBaseMatrix[i] = baseMatrix[i];
        baseMatrix[i] = state->getCTM()[i];
    }
    
    display(str, false);
    if (softMask || transpGroup) 
        out->endTransparencyGroup(state);
        
    for (int i = 0; i < 6; ++i) 
        baseMatrix[i] = oldBaseMatrix[i];
        
    parser = oldParser;
    restoreState();
    popResources();
    if (softMask) 
        out->setSoftMask(state, bbox, alpha, transferFunc, backdropColor);
    else if (transpGroup) 
        out->paintTransparencyGroup(state, bbox);
}

void XWGraphix::doFunctionShFill(XWFunctionShading *shading)
{
  double x0, y0, x1, y1;
  CoreColor colors[4];

  if (out->useShadedFills() &&
      out->functionShadedFill(state, shading)) {
    return;
  }

  shading->getDomain(&x0, &y0, &x1, &y1);
  shading->getColor(x0, y0, &colors[0]);
  shading->getColor(x0, y1, &colors[1]);
  shading->getColor(x1, y0, &colors[2]);
  shading->getColor(x1, y1, &colors[3]);
  doFunctionShFill1(shading, x0, y0, x1, y1, colors, 0);
}

void XWGraphix::doFunctionShFill1(XWFunctionShading *shading,
			                       double x0, double y0,
			                       double x1, double y1,
			                       CoreColor *colors, int depth)
{
  CoreColor fillColor;
  CoreColor color0M, color1M, colorM0, colorM1, colorMM;
  CoreColor colors2[4];
  double *matrix;
  double xM, yM;
  int nComps, i, j;

  nComps = shading->getColorSpace()->getNComps();
  matrix = shading->getMatrix();

  // compare the four corner colors
  for (i = 0; i < 4; ++i) {
    for (j = 0; j < nComps; ++j) {
      if (abs(colors[i].c[j] - colors[(i+1)&3].c[j]) > functionColorDelta) {
	break;
      }
    }
    if (j < nComps) {
      break;
    }
  }

  // center of the rectangle
  xM = 0.5 * (x0 + x1);
  yM = 0.5 * (y0 + y1);

  // the four corner colors are close (or we hit the recursive limit)
  // -- fill the rectangle; but require at least one subdivision
  // (depth==0) to avoid problems when the four outer corners of the
  // shaded region are the same color
  if ((i == 4 && depth > 0) || depth == functionMaxDepth) {

    // use the center color
    shading->getColor(xM, yM, &fillColor);
    state->setFillColor(&fillColor);
    out->updateFillColor(state);

    // fill the rectangle
    state->moveTo(x0 * matrix[0] + y0 * matrix[2] + matrix[4],
		  x0 * matrix[1] + y0 * matrix[3] + matrix[5]);
    state->lineTo(x1 * matrix[0] + y0 * matrix[2] + matrix[4],
		  x1 * matrix[1] + y0 * matrix[3] + matrix[5]);
    state->lineTo(x1 * matrix[0] + y1 * matrix[2] + matrix[4],
		  x1 * matrix[1] + y1 * matrix[3] + matrix[5]);
    state->lineTo(x0 * matrix[0] + y1 * matrix[2] + matrix[4],
		  x0 * matrix[1] + y1 * matrix[3] + matrix[5]);
    state->closePath();
    out->fill(state);
    state->clearPath();

  // the four corner colors are not close enough -- subdivide the
  // rectangle
  } else {

    // colors[0]       colorM0       colors[2]
    //   (x0,y0)       (xM,y0)       (x1,y0)
    //         +----------+----------+
    //         |          |          |
    //         |    UL    |    UR    |
    // color0M |       colorMM       | color1M
    // (x0,yM) +----------+----------+ (x1,yM)
    //         |       (xM,yM)       |
    //         |    LL    |    LR    |
    //         |          |          |
    //         +----------+----------+
    // colors[1]       colorM1       colors[3]
    //   (x0,y1)       (xM,y1)       (x1,y1)

    shading->getColor(x0, yM, &color0M);
    shading->getColor(x1, yM, &color1M);
    shading->getColor(xM, y0, &colorM0);
    shading->getColor(xM, y1, &colorM1);
    shading->getColor(xM, yM, &colorMM);

    // upper-left sub-rectangle
    colors2[0] = colors[0];
    colors2[1] = color0M;
    colors2[2] = colorM0;
    colors2[3] = colorMM;
    doFunctionShFill1(shading, x0, y0, xM, yM, colors2, depth + 1);
    
    // lower-left sub-rectangle
    colors2[0] = color0M;
    colors2[1] = colors[1];
    colors2[2] = colorMM;
    colors2[3] = colorM1;
    doFunctionShFill1(shading, x0, yM, xM, y1, colors2, depth + 1);
    
    // upper-right sub-rectangle
    colors2[0] = colorM0;
    colors2[1] = colorMM;
    colors2[2] = colors[2];
    colors2[3] = color1M;
    doFunctionShFill1(shading, xM, y0, x1, yM, colors2, depth + 1);

    // lower-right sub-rectangle
    colors2[0] = colorMM;
    colors2[1] = colorM1;
    colors2[2] = color1M;
    colors2[3] = colors[3];
    doFunctionShFill1(shading, xM, yM, x1, y1, colors2, depth + 1);
  }
}

void XWGraphix::doGouraudTriangleShFill(XWGouraudTriangleShading *shading)
{
    double x0, y0, x1, y1, x2, y2;
    CoreColor color0, color1, color2;
    
    for (int i = 0; i < shading->getNTriangles(); ++i) 
    {
        shading->getTriangle(i, &x0, &y0, &color0,
			                 &x1, &y1, &color1,
			                 &x2, &y2, &color2);
        gouraudFillTriangle(x0, y0, &color0, x1, y1, &color1, x2, y2, &color2,
			                shading->getColorSpace()->getNComps(), 0);
    }
}

void XWGraphix::doImage(XWObject *ref, XWStream *str, bool inlineImg)
{
  XWDict *dict, *maskDict;
  int width, height;
  int bits, maskBits;
  int csMode;
  bool mask;
  bool invert;
  XWColorSpace *colorSpace, *maskColorSpace;
  XWImageColorMap *colorMap, *maskColorMap;
  XWObject maskObj, smaskObj;
  bool haveColorKeyMask, haveExplicitMask, haveSoftMask;
  int maskColors[2*COLOR_MAX_COMPS];
  int maskWidth, maskHeight;
  bool maskInvert;
  XWStream *maskStr;
  XWObject obj1, obj2;
  int i, n;

  // get info from the stream
  bits = 0;
  csMode = STREAM_CS_NONE;
  str->getImageParams(&bits, &csMode);

  // get stream dict
  dict = str->getDict();

  // get size
  dict->lookup("Width", &obj1);
  if (obj1.isNull()) 
  {
    obj1.free();
    dict->lookup("W", &obj1);
  }
  if (!obj1.isInt()) 
  {
    goto err2;
  }
  width = obj1.getInt();
  obj1.free();
  if (width <= 0) 
  {
    goto err1;
  }
  dict->lookup("Height", &obj1);
  if (obj1.isNull()) 
  {
    obj1.free();
    dict->lookup("H", &obj1);
  }
  if (!obj1.isInt()) 
  {
    goto err2;
  }
  height = obj1.getInt();
  obj1.free();
  if (height <= 0) 
  {
    goto err1;
  }

  // image or mask?
  dict->lookup("ImageMask", &obj1);
  if (obj1.isNull()) 
  {
    obj1.free();
    dict->lookup("IM", &obj1);
  }
  mask = false;
  if (obj1.isBool())
    mask = obj1.getBool();
  else if (!obj1.isNull())
    goto err2;
  obj1.free();

  // bit depth
  if (bits == 0) 
  {
    dict->lookup("BitsPerComponent", &obj1);
    if (obj1.isNull()) 
    {
      obj1.free();
      dict->lookup("BPC", &obj1);
    }
    if (obj1.isInt()) 
    {
      bits = obj1.getInt();
    } 
    else if (mask) 
    {
      bits = 1;
    } 
    else 
    {
      goto err2;
    }
    obj1.free();
  }

  // display a mask
  if (mask) 
  {

    // check for inverted mask
    if (bits != 1)
      goto err1;
    invert = false;
    dict->lookup("Decode", &obj1);
    if (obj1.isNull()) 
    {
      obj1.free();
      dict->lookup("D", &obj1);
    }
    if (obj1.isArray()) 
    {
      obj1.arrayGet(0, &obj2);
      invert = obj2.isNum() && obj2.getNum() == 1;
      obj2.free();
    } 
    else if (!obj1.isNull()) 
    {
      goto err2;
    }
    obj1.free();

    // if drawing is disabled, skip over inline image data
    if (!ocState) 
    {
      str->reset();
      n = height * ((width + 7) / 8);
      for (i = 0; i < n; ++i) 
     	{
				str->getChar();
      }
      str->close();

    // draw it
    } 
    else 
    {
      if (state->getFillColorSpace()->getMode() == COLOR_SPACE_PATTERN) 
      {
				doPatternImageMask(ref, str, width, height, invert, inlineImg);
      } 
      else 
      {
				out->drawImageMask(state, ref, str, width, height, invert, inlineImg);
      }
    }

  } 
  else 
  {

    // get color space and color map
    dict->lookup("ColorSpace", &obj1);
    if (obj1.isNull()) 
    {
      obj1.free();
      dict->lookup("CS", &obj1);
    }
    if (obj1.isName()) 
    {
      res->lookupColorSpace(obj1.getName(), &obj2);
      if (!obj2.isNull()) 
      {
				obj1.free();
				obj1 = obj2;
      } 
      else 
      {
				obj2.free();
      }
    }
    if (!obj1.isNull()) 
    {
      colorSpace = XWColorSpace::parse(&obj1);
    } 
    else if (csMode == STREAM_CS_DEVICEGRAY) 
    {
      colorSpace = new XWDeviceGrayColorSpace();
    } 
    else if (csMode == STREAM_CS_DEVICERGB) 
    {
      colorSpace = new XWDeviceRGBColorSpace();
    } 
    else if (csMode == STREAM_CS_DEVICECMYK) 
    {
      colorSpace = new XWDeviceCMYKColorSpace();
    } 
    else 
    {
      colorSpace = NULL;
    }
    obj1.free();
    if (!colorSpace) 
    {
      goto err1;
    }
    dict->lookup("Decode", &obj1);
    if (obj1.isNull()) 
    {
      obj1.free();
      dict->lookup("D", &obj1);
    }
    colorMap = new XWImageColorMap(bits, &obj1, colorSpace);
    obj1.free();
    if (!colorMap->isOk()) 
    {
      delete colorMap;
      goto err1;
    }

    // get the mask
    haveColorKeyMask = haveExplicitMask = haveSoftMask = false;
    maskStr = NULL; // make gcc happy
    maskWidth = maskHeight = 0; // make gcc happy
    maskInvert = false; // make gcc happy
    maskColorMap = NULL; // make gcc happy
    dict->lookup("Mask", &maskObj);
    dict->lookup("SMask", &smaskObj);
    if (smaskObj.isStream()) 
    {
      // soft mask
      if (inlineImg) 
      {
				goto err1;
      }
      maskStr = smaskObj.getStream();
      maskDict = smaskObj.streamGetDict();
      maskDict->lookup("Width", &obj1);
      if (obj1.isNull()) 
      {
				obj1.free();
				maskDict->lookup("W", &obj1);
      }
      if (!obj1.isInt()) 
     	{
				goto err2;
      }
      maskWidth = obj1.getInt();
      obj1.free();
      maskDict->lookup("Height", &obj1);
      if (obj1.isNull()) 
      {
				obj1.free();
				maskDict->lookup("H", &obj1);
      }
      if (!obj1.isInt()) 
      {
				goto err2;
      }
      maskHeight = obj1.getInt();
      obj1.free();
      maskDict->lookup("BitsPerComponent", &obj1);
      if (obj1.isNull()) 
      {
				obj1.free();
				maskDict->lookup("BPC", &obj1);
      }
      if (!obj1.isInt()) 
      {
				goto err2;
      }
      maskBits = obj1.getInt();
      obj1.free();
      maskDict->lookup("ColorSpace", &obj1);
      if (obj1.isNull()) 
      {
				obj1.free();
				maskDict->lookup("CS", &obj1);
      }
      if (obj1.isName()) 
      {
				res->lookupColorSpace(obj1.getName(), &obj2);
				if (!obj2.isNull()) 
				{
	  			obj1.free();
	  			obj1 = obj2;
				} 
				else 
				{
	  			obj2.free();
				}
      }
      maskColorSpace = XWColorSpace::parse(&obj1);
      obj1.free();
      if (!maskColorSpace || maskColorSpace->getMode() != COLOR_SPACE_DEVICEGRAY) 
      {
				goto err1;
      }
      maskDict->lookup("Decode", &obj1);
      if (obj1.isNull()) 
      {
				obj1.free();
				maskDict->lookup("D", &obj1);
      }
      maskColorMap = new XWImageColorMap(maskBits, &obj1, maskColorSpace);
      obj1.free();
      if (!maskColorMap->isOk()) 
      {
				delete maskColorMap;
				goto err1;
      }
      //~ handle the Matte entry
      haveSoftMask = true;
    } 
    else if (maskObj.isArray()) 
    {
      // color key mask
      haveColorKeyMask = true;
      for (i = 0; i+1 < maskObj.arrayGetLength() && i+1 < 2*COLOR_MAX_COMPS; i += 2) 
      {
				maskObj.arrayGet(i, &obj1);
				if (!obj1.isInt()) 
				{
	  			obj1.free();
	  			haveColorKeyMask = false;
	  			break;
				}
				maskColors[i] = obj1.getInt();
				obj1.free();
				if (maskColors[i] < 0 || maskColors[i] >= (1 << bits)) 
				{
	  			haveColorKeyMask = false;
	  			break;
				}
				maskObj.arrayGet(i+1, &obj1);
				if (!obj1.isInt()) 
				{
	  			obj1.free();
	  			haveColorKeyMask = false;
	  			break;
				}
				maskColors[i+1] = obj1.getInt();
				obj1.free();
				if (maskColors[i+1] < 0 || maskColors[i+1] >= (1 << bits) || maskColors[i] > maskColors[i+1]) 
				{
	  			haveColorKeyMask = false;
	  			break;
				}
      }
    } else if (maskObj.isStream()) 
    {
      // explicit mask
      if (inlineImg) 
      {
				goto err1;
      }
      maskStr = maskObj.getStream();
      maskDict = maskObj.streamGetDict();
      maskDict->lookup("Width", &obj1);
      if (obj1.isNull()) 
      {
				obj1.free();
				maskDict->lookup("W", &obj1);
      }
      if (!obj1.isInt()) 
      {
				goto err2;
      }
      maskWidth = obj1.getInt();
      obj1.free();
      maskDict->lookup("Height", &obj1);
      if (obj1.isNull()) 
      {
				obj1.free();
				maskDict->lookup("H", &obj1);
      }
      if (!obj1.isInt()) 
      {
				goto err2;
      }
      maskHeight = obj1.getInt();
      obj1.free();
      maskDict->lookup("ImageMask", &obj1);
      if (obj1.isNull()) 
      {
				obj1.free();
				maskDict->lookup("IM", &obj1);
      }
      if (!obj1.isBool() || !obj1.getBool()) 
      {
				goto err2;
      }
      obj1.free();
      maskInvert = false;
      maskDict->lookup("Decode", &obj1);
      if (obj1.isNull()) 
      {
				obj1.free();
				maskDict->lookup("D", &obj1);
      }
      if (obj1.isArray()) 
      {
				obj1.arrayGet(0, &obj2);
				maskInvert = obj2.isNum() && obj2.getNum() == 1;
				obj2.free();
      } 
      else if (!obj1.isNull()) 
      {
				goto err2;
      }
      obj1.free();
      haveExplicitMask = true;
    }

    // if drawing is disabled, skip over inline image data
    if (!ocState) 
    {
      str->reset();
      n = height * ((width * colorMap->getNumPixelComps() * colorMap->getBits() + 7) / 8);
      for (i = 0; i < n; ++i) 
      {
				str->getChar();
      }
      str->close();

    // draw it
    } 
    else 
    {
      if (haveSoftMask) 
      {
				out->drawSoftMaskedImage(state, ref, str, width, height, colorMap, maskStr, maskWidth, maskHeight, maskColorMap);
				delete maskColorMap;
      } 
      else if (haveExplicitMask) 
      {
				out->drawMaskedImage(state, ref, str, width, height, colorMap, maskStr, maskWidth, maskHeight, maskInvert);
      } 
      else 
      {
				out->drawImage(state, ref, str, width, height, colorMap, haveColorKeyMask ? maskColors : (int *)NULL, inlineImg);
      }
    }

    delete colorMap;
    maskObj.free();
    smaskObj.free();
  }

  if ((i = width * height) > 1000) 
  {
    i = 1000;
  }
  updateLevel += i;

  return;

 err2:
  obj1.free();
 err1:
  return;
}

void XWGraphix::doIncCharCount(XWString *s)
{
	if (out->needCharCount()) 
	{
    out->incCharCount(s->getLength());
  }
}

void XWGraphix::doPatchMeshShFill(XWPatchMeshShading *shading)
{
  int start, i;

  if (shading->getNPatches() > 128) {
    start = 3;
  } else if (shading->getNPatches() > 64) {
    start = 2;
  } else if (shading->getNPatches() > 16) {
    start = 1;
  } else {
    start = 0;
  }
  for (i = 0; i < shading->getNPatches(); ++i) {
    fillPatch(shading->getPatch(i), shading->getColorSpace()->getNComps(),
	      start);
  }
}

void XWGraphix::doPatternFill(bool eoFill)
{
  XWPattern *pattern;

  // this is a bit of a kludge -- patterns can be really slow, so we
  // skip them if we're only doing text extraction, since they almost
  // certainly don't contain any text
  if (!out->needNonText()) {
    return;
  }

  if (!(pattern = state->getFillPattern())) {
    return;
  }
  switch (pattern->getType()) {
  case 1:
    doTilingPatternFill((XWTilingPattern *)pattern, false, eoFill, false);
    break;
  case 2:
    doShadingPatternFill((XWShadingPattern *)pattern, false, eoFill, false);
    break;
  default:
    break;
  }
}

void XWGraphix::doPatternImageMask(XWObject *ref, 
                                   XWStream *str, 
                                   int width, 
                                   int height,
			                             bool invert, 
			                             bool inlineImg)
{
	saveState();

  out->setSoftMaskFromImageMask(state, ref, str,
				width, height, invert, inlineImg);

  state->clearPath();
  state->moveTo(0, 0);
  state->lineTo(1, 0);
  state->lineTo(1, 1);
  state->lineTo(0, 1);
  state->closePath();
  doPatternFill(true);

  restoreState();
}

void XWGraphix::doPatternStroke()
{
  XWPattern *pattern;

  // this is a bit of a kludge -- patterns can be really slow, so we
  // skip them if we're only doing text extraction, since they almost
  // certainly don't contain any text
  if (!out->needNonText()) {
    return;
  }

  if (!(pattern = state->getStrokePattern())) {
    return;
  }
  switch (pattern->getType()) {
  case 1:
    doTilingPatternFill((XWTilingPattern *)pattern, true, false, false);
    break;
  case 2:
    doShadingPatternFill((XWShadingPattern *)pattern, true, false, false);
    break;
  default:
    break;
  }
}

void XWGraphix::doPatternText()
{
	XWPattern *pattern;

  // this is a bit of a kludge -- patterns can be really slow, so we
  // skip them if we're only doing text extraction, since they almost
  // certainly don't contain any text
  if (!out->needNonText()) {
    return;
  }

  if (!(pattern = state->getFillPattern())) {
    return;
  }
  switch (pattern->getType()) {
  case 1:
    doTilingPatternFill((XWTilingPattern *)pattern, false, false, true);
    break;
  case 2:
    doShadingPatternFill((XWShadingPattern *)pattern, false, false, true);
    break;
  default:
    break;
  }
}

void XWGraphix::doRadialShFill(XWRadialShading *shading)
{
  double xMin, yMin, xMax, yMax;
  double x0, y0, r0, x1, y1, r1, t0, t1;
  int nComps;
  CoreColor colorA, colorB;
  double xa, ya, xb, yb, ra, rb;
  double ta, tb, sa, sb;
  double sz, sMin, sMax, h;
  double sLeft, sRight, sTop, sBottom, sZero, sDiag;
  bool haveSLeft, haveSRight, haveSTop, haveSBottom, haveSZero;
  bool haveSMin, haveSMax;
  bool enclosed;
  int ia, ib, k, n;
  double *ctm;
  double theta, alpha, angle, t;

  if (out->useShadedFills() &&
      out->radialShadedFill(state, shading)) 
  {
    return;
  }

  // get the shading info
  shading->getCoords(&x0, &y0, &r0, &x1, &y1, &r1);
  t0 = shading->getDomain0();
  t1 = shading->getDomain1();
  nComps = shading->getColorSpace()->getNComps();

  // Compute the point at which r(s) = 0; check for the enclosed
  // circles case; and compute the angles for the tangent lines.
  h = sqrt((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0));
  if (h == 0) 
  {
    enclosed = true;
    theta = 0; // make gcc happy
    sz = 0; // make gcc happy
  } 
  else if (r1 - r0 == 0) 
  {
    enclosed = false;
    theta = 0;
    sz = 0; // make gcc happy
  } 
  else if (fabs(r1 - r0) >= h) 
  {
    enclosed = true;
    theta = 0; // make gcc happy
    sz = 0; // make gcc happy
  } 
  else 
  {
    enclosed = false;
    sz = -r0 / (r1 - r0);
    theta = asin((r1 - r0) / h);
  }
  if (enclosed) 
  {
    alpha = 0;
  } 
  else 
  {
    alpha = atan2(y1 - y0, x1 - x0);
  }

  // compute the (possibly extended) s range
  state->getUserClipBBox(&xMin, &yMin, &xMax, &yMax);
  if (enclosed) 
  {
    sMin = 0;
    sMax = 1;
  } 
  else 
  {
    // solve x(sLeft) + r(sLeft) = xMin
    if ((haveSLeft = fabs((x1 + r1) - (x0 + r0)) > 0.000001)) 
    {
      sLeft = (xMin - (x0 + r0)) / ((x1 + r1) - (x0 + r0));
    } 
    else 
    {
      sLeft = 0; // make gcc happy
    }
    // solve x(sRight) - r(sRight) = xMax
    if ((haveSRight = fabs((x1 - r1) - (x0 - r0)) > 0.000001)) 
    {
      sRight = (xMax - (x0 - r0)) / ((x1 - r1) - (x0 - r0));
    } 
    else 
    {
      sRight = 0; // make gcc happy
    }
    // solve y(sBottom) + r(sBottom) = yMin
    if ((haveSBottom = fabs((y1 + r1) - (y0 + r0)) > 0.000001)) 
    {
      sBottom = (yMin - (y0 + r0)) / ((y1 + r1) - (y0 + r0));
    } 
    else 
    {
      sBottom = 0; // make gcc happy
    }
    // solve y(sTop) - r(sTop) = yMax
    if ((haveSTop = fabs((y1 - r1) - (y0 - r0)) > 0.000001)) 
    {
      sTop = (yMax - (y0 - r0)) / ((y1 - r1) - (y0 - r0));
    } 
    else 
    {
      sTop = 0; // make gcc happy
    }
    // solve r(sZero) = 0
    if ((haveSZero = fabs(r1 - r0) > 0.000001)) 
    {
      sZero = -r0 / (r1 - r0);
    } 
    else 
    {
      sZero = 0; // make gcc happy
    }
    // solve r(sDiag) = sqrt((xMax-xMin)^2 + (yMax-yMin)^2)
    if (haveSZero) 
    {
      sDiag = (sqrt((xMax - xMin) * (xMax - xMin) + (yMax - yMin) * (yMax - yMin)) - r0) / (r1 - r0);
    } 
    else 
    {
      sDiag = 0; // make gcc happy
    }
    // compute sMin
    if (shading->getExtend0()) 
    {
      sMin = 0;
      haveSMin = false;
      if (x0 < x1 && haveSLeft && sLeft < 0) 
      {
				sMin = sLeft;
				haveSMin = true;
      } 
      else if (x0 > x1 && haveSRight && sRight < 0) 
      {
				sMin = sRight;
				haveSMin = true;
      }
      if (y0 < y1 && haveSBottom && sBottom < 0) 
      {
				if (!haveSMin || sBottom > sMin) 
				{
	  			sMin = sBottom;
	  			haveSMin = true;
				}
      } 
      else if (y0 > y1 && haveSTop && sTop < 0) 
      {
				if (!haveSMin || sTop > sMin) 
				{
	  			sMin = sTop;
	  			haveSMin = true;
				}
      }
      if (haveSZero && sZero < 0) 
      {
				if (!haveSMin || sZero > sMin) 
				{
	  			sMin = sZero;
				}
      }
    } 
    else 
    {
      sMin = 0;
    }
    // compute sMax
    if (shading->getExtend1()) 
    {
      sMax = 1;
      haveSMax = false;
      if (x1 < x0 && haveSLeft && sLeft > 1) 
      {
				sMax = sLeft;
				haveSMax = true;
      } 
      else if (x1 > x0 && haveSRight && sRight > 1) 
      {
				sMax = sRight;
				haveSMax = true;
      }
      if (y1 < y0 && haveSBottom && sBottom > 1) 
      {
				if (!haveSMax || sBottom < sMax) 
				{
	  			sMax = sBottom;
	  			haveSMax = true;
				}
      } 
      else if (y1 > y0 && haveSTop && sTop > 1) 
      {
				if (!haveSMax || sTop < sMax) 
				{
	  			sMax = sTop;
	  			haveSMax = true;
				}
      }
      if (haveSZero && sDiag > 1) 
      {
				if (!haveSMax || sDiag < sMax) 
				{
	  			sMax = sDiag;
				}
      }
    } 
    else 
    {
      sMax = 1;
    }
  }

  // compute the number of steps into which circles must be divided to
  // achieve a curve flatness of 0.1 pixel in device space for the
  // largest circle (note that "device space" is 72 dpi when generating
  // PostScript, hence the relatively small 0.1 pixel accuracy)
  ctm = state->getCTM();
  t = fabs(ctm[0]);
  if (fabs(ctm[1]) > t) 
  {
    t = fabs(ctm[1]);
  }
  if (fabs(ctm[2]) > t) 
  {
    t = fabs(ctm[2]);
  }
  if (fabs(ctm[3]) > t) 
  {
    t = fabs(ctm[3]);
  }
  if (r0 > r1) 
  {
    t *= r0;
  } 
  else 
  {
    t *= r1;
  }
  if (t < 1) 
  {
    n = 3;
  } 
  else 
  {
    n = (int)(M_PI / acos(1 - 0.1 / t));
    if (n < 3) 
    {
      n = 3;
    } 
    else if (n > 200) 
    {
      n = 200;
    }
  }

  // setup for the start circle
  ia = 0;
  sa = sMin;
  ta = t0 + sa * (t1 - t0);
  xa = x0 + sa * (x1 - x0);
  ya = y0 + sa * (y1 - y0);
  ra = r0 + sa * (r1 - r0);
  if (ta < t0) 
  {
    shading->getColor(t0, &colorA);
  } 
  else if (ta > t1) 
  {
    shading->getColor(t1, &colorA);
  } 
  else 
  {
    shading->getColor(ta, &colorA);
  }

  // fill the circles
  while (ia < radialMaxSplits) 
  {

    // go as far along the t axis (toward t1) as we can, such that the
    // color difference is within the tolerance (radialColorDelta) --
    // this uses bisection (between the current value, t, and t1),
    // limited to radialMaxSplits points along the t axis; require at
    // least one split to avoid problems when the innermost and
    // outermost colors are the same
    ib = radialMaxSplits;
    sb = sMax;
    tb = t0 + sb * (t1 - t0);
    if (tb < t0) 
    {
      shading->getColor(t0, &colorB);
    } 
    else if (tb > t1) 
    {
      shading->getColor(t1, &colorB);
    } 
    else 
    {
      shading->getColor(tb, &colorB);
    }
    while (ib - ia > 1) 
    {
      for (k = 0; k < nComps; ++k) 
      {
				if (abs(colorB.c[k] - colorA.c[k]) > radialColorDelta) 
				{
	  			break;
				}
      }
      if (k == nComps && ib < radialMaxSplits) 
      {
				break;
      }
      ib = (ia + ib) / 2;
      sb = sMin + ((double)ib / (double)radialMaxSplits) * (sMax - sMin);
      tb = t0 + sb * (t1 - t0);
      if (tb < t0) 
      {
				shading->getColor(t0, &colorB);
      } 
      else if (tb > t1) 
      {
				shading->getColor(t1, &colorB);
      } 
      else 
      {
				shading->getColor(tb, &colorB);
      }
    }

    // compute center and radius of the circle
    xb = x0 + sb * (x1 - x0);
    yb = y0 + sb * (y1 - y0);
    rb = r0 + sb * (r1 - r0);

    // use the average of the colors at the two circles
    for (k = 0; k < nComps; ++k) 
   	{
      colorA.c[k] = (colorA.c[k] + colorB.c[k]) / 2;
    }
    state->setFillColor(&colorA);
    out->updateFillColor(state);

    if (enclosed) 
    {

      // construct path for first circle (counterclockwise)
      state->moveTo(xa + ra, ya);
      for (k = 1; k < n; ++k) 
     	{
				angle = ((double)k / (double)n) * 2 * M_PI;
				state->lineTo(xa + ra * cos(angle), ya + ra * sin(angle));
      }
      state->closePath();

      // construct and append path for second circle (clockwise)
      state->moveTo(xb + rb, yb);
      for (k = 1; k < n; ++k) 
      {
				angle = -((double)k / (double)n) * 2 * M_PI;
				state->lineTo(xb + rb * cos(angle), yb + rb * sin(angle));
      }
      state->closePath();

    } 
    else 
    {

      // construct the first subpath (clockwise)
      state->moveTo(xa + ra * cos(alpha + theta + 0.5 * M_PI), ya + ra * sin(alpha + theta + 0.5 * M_PI));
      for (k = 0; k < n; ++k) 
      {
				angle = alpha + theta + 0.5 * M_PI - ((double)k / (double)n) * (2 * theta + M_PI);
				state->lineTo(xb + rb * cos(angle), yb + rb * sin(angle));
      }
      for (k = 0; k < n; ++k) 
      {
				angle = alpha - theta - 0.5 * M_PI + ((double)k / (double)n) * (2 * theta - M_PI);
				state->lineTo(xa + ra * cos(angle), ya + ra * sin(angle));
      }
      state->closePath();

      // construct the second subpath (counterclockwise)
      state->moveTo(xa + ra * cos(alpha + theta + 0.5 * M_PI),
		    ya + ra * sin(alpha + theta + 0.5 * M_PI));
      for (k = 0; k < n; ++k) 
      {
				angle = alpha + theta + 0.5 * M_PI + ((double)k / (double)n) * (-2 * theta + M_PI);
				state->lineTo(xb + rb * cos(angle), yb + rb * sin(angle));
      }
      for (k = 0; k < n; ++k) 
      {
				angle = alpha - theta - 0.5 * M_PI + ((double)k / (double)n) * (2 * theta + M_PI);
				state->lineTo(xa + ra * cos(angle), ya + ra * sin(angle));
      }
      state->closePath();
    }

    // fill the path
    out->fill(state);
    state->clearPath();

    // step to the next value of t
    ia = ib;
    sa = sb;
    ta = tb;
    xa = xb;
    ya = yb;
    ra = rb;
    colorA = colorB;
  }

  if (enclosed) 
  {
    // extend the smaller circle
    if ((shading->getExtend0() && r0 <= r1) ||
				(shading->getExtend1() && r1 < r0)) 
		{
      if (r0 <= r1) 
      {
				ta = t0;
				ra = r0;
				xa = x0;
				ya = y0;
      } 
      else 
      {
				ta = t1;
				ra = r1;
				xa = x1;
				ya = y1;
      }
      shading->getColor(ta, &colorA);
      state->setFillColor(&colorA);
      out->updateFillColor(state);
      state->moveTo(xa + ra, ya);
      for (k = 1; k < n; ++k) 
      {
				angle = ((double)k / (double)n) * 2 * M_PI;
				state->lineTo(xa + ra * cos(angle), ya + ra * sin(angle));
      }
      state->closePath();
      out->fill(state);
      state->clearPath();
    }

    // extend the larger circle
    if ((shading->getExtend0() && r0 > r1) ||
				(shading->getExtend1() && r1 >= r0)) 
		{
      if (r0 > r1) 
      {
				ta = t0;
				ra = r0;
				xa = x0;
				ya = y0;
      } 
      else 
      {
				ta = t1;
				ra = r1;
				xa = x1;
				ya = y1;
      }
      shading->getColor(ta, &colorA);
      state->setFillColor(&colorA);
      out->updateFillColor(state);
      state->moveTo(xMin, yMin);
      state->lineTo(xMin, yMax);
      state->lineTo(xMax, yMax);
      state->lineTo(xMax, yMin);
      state->closePath();
      state->moveTo(xa + ra, ya);
      for (k = 1; k < n; ++k) 
      {
				angle = ((double)k / (double)n) * 2 * M_PI;
				state->lineTo(xa + ra * cos(angle), ya + ra * sin(angle));
      }
      state->closePath();
      out->fill(state);
      state->clearPath();
    }
  }
}

void XWGraphix::doShadingPatternFill(XWShadingPattern *sPat, bool stroke, bool eoFill, bool text)
{
  XWShading *shading;
  XWGraphixState *savedState;
  double *ctm, *btm, *ptm;
  double m[6], ictm[6], m1[6];
  double xMin, yMin, xMax, yMax;
  double det;

  shading = sPat->getShading();

  // save current graphics state
  savedState = saveStateStack();

  // clip to current path
  if (stroke) 
  {
    state->clipToStrokePath();
    out->clipToStrokePath(state);
  } 
  else if (!text) 
  {
    state->clip();
    if (eoFill) 
    {
      out->eoClip(state);
    } 
    else 
    {
      out->clip(state);
    }
  }
  state->clearPath();

  // construct a (pattern space) -> (current space) transform matrix
  ctm = state->getCTM();
  btm = baseMatrix;
  ptm = sPat->getMatrix();
  // iCTM = invert CTM
  det = 1 / (ctm[0] * ctm[3] - ctm[1] * ctm[2]);
  ictm[0] = ctm[3] * det;
  ictm[1] = -ctm[1] * det;
  ictm[2] = -ctm[2] * det;
  ictm[3] = ctm[0] * det;
  ictm[4] = (ctm[2] * ctm[5] - ctm[3] * ctm[4]) * det;
  ictm[5] = (ctm[1] * ctm[4] - ctm[0] * ctm[5]) * det;
  // m1 = PTM * BTM = PTM * base transform matrix
  m1[0] = ptm[0] * btm[0] + ptm[1] * btm[2];
  m1[1] = ptm[0] * btm[1] + ptm[1] * btm[3];
  m1[2] = ptm[2] * btm[0] + ptm[3] * btm[2];
  m1[3] = ptm[2] * btm[1] + ptm[3] * btm[3];
  m1[4] = ptm[4] * btm[0] + ptm[5] * btm[2] + btm[4];
  m1[5] = ptm[4] * btm[1] + ptm[5] * btm[3] + btm[5];
  // m = m1 * iCTM = (PTM * BTM) * (iCTM)
  m[0] = m1[0] * ictm[0] + m1[1] * ictm[2];
  m[1] = m1[0] * ictm[1] + m1[1] * ictm[3];
  m[2] = m1[2] * ictm[0] + m1[3] * ictm[2];
  m[3] = m1[2] * ictm[1] + m1[3] * ictm[3];
  m[4] = m1[4] * ictm[0] + m1[5] * ictm[2] + ictm[4];
  m[5] = m1[4] * ictm[1] + m1[5] * ictm[3] + ictm[5];

  // set the new matrix
  state->concatCTM(m[0], m[1], m[2], m[3], m[4], m[5]);
  out->updateCTM(state, m[0], m[1], m[2], m[3], m[4], m[5]);

  // clip to bbox
  if (shading->getHasBBox()) 
  {
    shading->getBBox(&xMin, &yMin, &xMax, &yMax);
    state->moveTo(xMin, yMin);
    state->lineTo(xMax, yMin);
    state->lineTo(xMax, yMax);
    state->lineTo(xMin, yMax);
    state->closePath();
    state->clip();
    out->clip(state);
    state->clearPath();
  }

  // set the color space
  state->setFillColorSpace(shading->getColorSpace()->copy());
  out->updateFillColorSpace(state);

  // background color fill
  if (shading->getHasBackground()) 
  {
    state->setFillColor(shading->getBackground());
    out->updateFillColor(state);
    state->getUserClipBBox(&xMin, &yMin, &xMax, &yMax);
    state->moveTo(xMin, yMin);
    state->lineTo(xMax, yMin);
    state->lineTo(xMax, yMax);
    state->lineTo(xMin, yMax);
    state->closePath();
    out->fill(state);
    state->clearPath();
  }
  
  out->setInShading(true);
  switch (shading->getType()) 
  {
  	case 1:
    	doFunctionShFill((XWFunctionShading *)shading);
    	break;
    	
  	case 2:
    	doAxialShFill((XWAxialShading *)shading);
    	break;
    	
  	case 3:
    	doRadialShFill((XWRadialShading *)shading);
    	break;
    	
  	case 4:
  	case 5:
    	doGouraudTriangleShFill((XWGouraudTriangleShading *)shading);
    	break;
    	
  	case 6:
  	case 7:
    	doPatchMeshShFill((XWPatchMeshShading *)shading);
    	break;
  }
  out->setInShading(false);
  restoreStateStack(savedState);
}

void XWGraphix::doShowText(XWString *s)
{
  XWCoreFont *font;
  int wMode;
  double riseX, riseY;
  uint code;
  uint u[8];
  double x, y, dx, dy, dx2, dy2, curX, curY, tdx, tdy, ddx, ddy;
  double originX, originY, tOriginX, tOriginY;
  double x0, y0, x1, y1;
  double oldCTM[6], newCTM[6];
  double *mat;
  XWObject charProc;
  XWDict *resDict;
  XWPDFParser *oldParser;
  XWGraphixState *savedState;
  char *p;
  int render;
  bool patternFill;
  int len, n, uLen, nChars, nSpaces, i;

  font = state->getFont();
  wMode = font->getWMode();

  if (out->useDrawChar()) 
  {
    out->beginString(state, s);
  }

  // if we're doing a pattern fill, set up clipping
  render = state->getRender();
  if (!(render & 1) &&
      state->getFillColorSpace()->getMode() == COLOR_SPACE_PATTERN) 
  {
    patternFill = true;
    saveState();
    // disable fill, enable clipping, leave stroke unchanged
    if ((render ^ (render >> 1)) & 1) 
    {
      render = 5;
    } 
    else 
    {
      render = 7;
    }
    state->setRender(render);
    out->updateRender(state);
  } 
  else 
  {
    patternFill = false;
  }

  state->textTransformDelta(0, state->getRise(), &riseX, &riseY);
  x0 = state->getCurX() + riseX;
  y0 = state->getCurY() + riseY;

  // handle a Type 3 char
  if (font->getType() == fontType3 && out->interpretType3Chars()) 
  {
    mat = state->getCTM();
    for (i = 0; i < 6; ++i) 
    {
      oldCTM[i] = mat[i];
    }
    mat = state->getTextMat();
    newCTM[0] = mat[0] * oldCTM[0] + mat[1] * oldCTM[2];
    newCTM[1] = mat[0] * oldCTM[1] + mat[1] * oldCTM[3];
    newCTM[2] = mat[2] * oldCTM[0] + mat[3] * oldCTM[2];
    newCTM[3] = mat[2] * oldCTM[1] + mat[3] * oldCTM[3];
    mat = font->getFontMatrix();
    newCTM[0] = mat[0] * newCTM[0] + mat[1] * newCTM[2];
    newCTM[1] = mat[0] * newCTM[1] + mat[1] * newCTM[3];
    newCTM[2] = mat[2] * newCTM[0] + mat[3] * newCTM[2];
    newCTM[3] = mat[2] * newCTM[1] + mat[3] * newCTM[3];
    newCTM[0] *= state->getFontSize();
    newCTM[1] *= state->getFontSize();
    newCTM[2] *= state->getFontSize();
    newCTM[3] *= state->getFontSize();
    newCTM[0] *= state->getHorizScaling();
    newCTM[2] *= state->getHorizScaling();
    curX = state->getCurX();
    curY = state->getCurY();
    oldParser = parser;
    p = s->getCString();
    len = s->getLength();
    while (len > 0) 
    {
      n = font->getNextChar(p, len, &code, u, (int)(sizeof(u) / sizeof(uint)), &uLen, &dx, &dy, &originX, &originY);
      dx = dx * state->getFontSize() + state->getCharSpace();
      if (n == 1 && *p == ' ') 
      {
				dx += state->getWordSpace();
      }
      dx *= state->getHorizScaling();
      dy *= state->getFontSize();
      state->textTransformDelta(dx, dy, &tdx, &tdy);
      state->transform(curX + riseX, curY + riseY, &x, &y);
      savedState = saveStateStack();
      state->setCTM(newCTM[0], newCTM[1], newCTM[2], newCTM[3], x, y);
      //~ the CTM concat values here are wrong (but never used)
      out->updateCTM(state, 1, 0, 0, 1, 0, 0);
      state->transformDelta(dx, dy, &ddx, &ddy);
      if (!out->beginType3Char(state, curX + riseX, curY + riseY, ddx, ddy, code, u, uLen)) 
      {
				((XWCore8BitFont *)font)->getCharProc(code, &charProc);
				if ((resDict = ((XWCore8BitFont *)font)->getResources())) 
				{
	  			pushResources(resDict);
				}
				if (charProc.isStream()) 
				{
	  			display(&charProc, false);
				} 
				out->endType3Char(state);
				if (resDict) 
				{
	  			popResources();
				}
				charProc.free();
      }
      restoreStateStack(savedState);
      curX += tdx;
      curY += tdy;
      state->moveTo(curX, curY);
      p += n;
      len -= n;
    }
    parser = oldParser;

  } 
  else if (out->useDrawChar()) 
  {
    p = s->getCString();
    len = s->getLength();
    while (len > 0) 
    {
      n = font->getNextChar(p, len, &code, u, (int)(sizeof(u) / sizeof(uint)), &uLen, &dx, &dy, &originX, &originY);
      if (wMode) 
      {
				dx *= state->getFontSize();
				dy = dy * state->getFontSize() + state->getCharSpace();
				if (n == 1 && *p == ' ') 
				{
	  			dy += state->getWordSpace();
				}
      } 
      else 
      {
				dx = dx * state->getFontSize() + state->getCharSpace();
				if (n == 1 && *p == ' ') 
				{
	  			dx += state->getWordSpace();
				}
				dx *= state->getHorizScaling();
				dy *= state->getFontSize();
      }
      state->textTransformDelta(dx, dy, &tdx, &tdy);
      originX *= state->getFontSize();
      originY *= state->getFontSize();
      state->textTransformDelta(originX, originY, &tOriginX, &tOriginY);
      out->drawChar(state, state->getCurX() + riseX, state->getCurY() + riseY, tdx, tdy, tOriginX, tOriginY, code, n, u, uLen);
      state->shift(tdx, tdy);
      p += n;
      len -= n;
    }

  } 
  else 
  {
    dx = dy = 0;
    p = s->getCString();
    len = s->getLength();
    nChars = nSpaces = 0;
    while (len > 0) 
   	{
      n = font->getNextChar(p, len, &code, u, (int)(sizeof(u) / sizeof(uint)), &uLen, &dx2, &dy2, &originX, &originY);
      dx += dx2;
      dy += dy2;
      if (n == 1 && *p == ' ') 
      {
				++nSpaces;
      }
      ++nChars;
      p += n;
      len -= n;
    }
    if (wMode) 
    {
      dx *= state->getFontSize();
      dy = dy * state->getFontSize()  + nChars * state->getCharSpace() + nSpaces * state->getWordSpace();
    } 
    else 
    {
      dx = dx * state->getFontSize() + nChars * state->getCharSpace() + nSpaces * state->getWordSpace();
      dx *= state->getHorizScaling();
      dy *= state->getFontSize();
    }
    state->textTransformDelta(dx, dy, &tdx, &tdy);
    out->drawString(state, s);
    state->shift(tdx, tdy);
  }

  if (out->useDrawChar()) 
  {
    out->endString(state);
  }

  if (patternFill) 
  {
    out->saveTextPos(state);
    // tell the OutputDev to do the clipping
    out->endTextObject(state);
    // set up a clipping bbox so doPatternText will work -- assume
    // that the text bounding box does not extend past the baseline in
    // any direction by more than twice the font size
    x1 = state->getCurX() + riseX;
    y1 = state->getCurY() + riseY;
    if (x0 > x1) 
    {
      x = x0; x0 = x1; x1 = x;
    }
    if (y0 > y1) 
    {
      y = y0; y0 = y1; y1 = y;
    }
    state->textTransformDelta(0, state->getFontSize(), &dx, &dy);
    state->textTransformDelta(state->getFontSize(), 0, &dx2, &dy2);
    dx = fabs(dx);
    dx2 = fabs(dx2);
    if (dx2 > dx) 
    {
      dx = dx2;
    }
    dy = fabs(dy);
    dy2 = fabs(dy2);
    if (dy2 > dy) 
    {
      dy = dy2;
    }
    state->clipToRect(x0 - 2 * dx, y0 - 2 * dy, x1 + 2 * dx, y1 + 2 * dy);
    // set render mode to fill-only
    state->setRender(0);
    out->updateRender(state);
    doPatternText();
    restoreState();
    out->restoreTextPos(state);
  }

  updateLevel += 10 * s->getLength();
}

void XWGraphix::doSoftMask(XWObject *str, 
                            bool alpha,
		                    XWColorSpace *blendingColorSpace,
		                    bool isolated, 
		                    bool knockout,
		                    XWFunction *transferFunc, 
		                    CoreColor *backdropColor)
{
  XWDict *dict, *resDict;
  double m[6], bbox[4];
  XWObject obj1, obj2;
  int i;

  // check for excessive recursion
  if (formDepth > 20) {
    return;
  }

  // get stream dict
  dict = str->streamGetDict();

  // check form type
  dict->lookup("FormType", &obj1);
  if (!(obj1.isNull() || (obj1.isInt() && obj1.getInt() == 1))) {
    xwApp->error("Unknown form type");
  }
  obj1.free();

  // get bounding box
  dict->lookup("BBox", &obj1);
  if (!obj1.isArray()) {
    obj1.free();
    xwApp->error("Bad form bounding box");
    return;
  }
  for (i = 0; i < 4; ++i) {
    obj1.arrayGet(i, &obj2);
    bbox[i] = obj2.getNum();
    obj2.free();
  }
  obj1.free();

  // get matrix
  dict->lookup("Matrix", &obj1);
  if (obj1.isArray()) {
    for (i = 0; i < 6; ++i) {
      obj1.arrayGet(i, &obj2);
      m[i] = obj2.getNum();
      obj2.free();
    }
  } else {
    m[0] = 1; m[1] = 0;
    m[2] = 0; m[3] = 1;
    m[4] = 0; m[5] = 0;
  }
  obj1.free();

  // get resources
  dict->lookup("Resources", &obj1);
  resDict = obj1.isDict() ? obj1.getDict() : (XWDict *)NULL;

  // draw it
  ++formDepth;
  drawForm(str, resDict, m, bbox, true, true,
	   blendingColorSpace, isolated, knockout,
	   alpha, transferFunc, backdropColor);
  --formDepth;

  if (blendingColorSpace) {
    delete blendingColorSpace;
  }
  obj1.free();
}

void XWGraphix::doTilingPatternFill(XWTilingPattern *tPat, bool stroke, bool eoFill, bool text)
{
  XWPatternColorSpace *patCS;
  XWColorSpace *cs;
  XWGraphixState *savedState;
  double xMin, yMin, xMax, yMax, x, y, x1, y1;
  double cxMin, cyMin, cxMax, cyMax;
  int xi0, yi0, xi1, yi1, xi, yi;
  double *ctm, *btm, *ptm;
  double m[6], ictm[6], m1[6], imb[6];
  double det;
  double xstep, ystep;
  int i;

  // get color space
  patCS = (XWPatternColorSpace *)(stroke ? state->getStrokeColorSpace() : state->getFillColorSpace());

  // construct a (pattern space) -> (current space) transform matrix
  ctm = state->getCTM();
  btm = baseMatrix;
  ptm = tPat->getMatrix();
  // iCTM = invert CTM
  det = 1 / (ctm[0] * ctm[3] - ctm[1] * ctm[2]);
  ictm[0] = ctm[3] * det;
  ictm[1] = -ctm[1] * det;
  ictm[2] = -ctm[2] * det;
  ictm[3] = ctm[0] * det;
  ictm[4] = (ctm[2] * ctm[5] - ctm[3] * ctm[4]) * det;
  ictm[5] = (ctm[1] * ctm[4] - ctm[0] * ctm[5]) * det;
  // m1 = PTM * BTM = PTM * base transform matrix
  m1[0] = ptm[0] * btm[0] + ptm[1] * btm[2];
  m1[1] = ptm[0] * btm[1] + ptm[1] * btm[3];
  m1[2] = ptm[2] * btm[0] + ptm[3] * btm[2];
  m1[3] = ptm[2] * btm[1] + ptm[3] * btm[3];
  m1[4] = ptm[4] * btm[0] + ptm[5] * btm[2] + btm[4];
  m1[5] = ptm[4] * btm[1] + ptm[5] * btm[3] + btm[5];
  // m = m1 * iCTM = (PTM * BTM) * (iCTM)
  m[0] = m1[0] * ictm[0] + m1[1] * ictm[2];
  m[1] = m1[0] * ictm[1] + m1[1] * ictm[3];
  m[2] = m1[2] * ictm[0] + m1[3] * ictm[2];
  m[3] = m1[2] * ictm[1] + m1[3] * ictm[3];
  m[4] = m1[4] * ictm[0] + m1[5] * ictm[2] + ictm[4];
  m[5] = m1[4] * ictm[1] + m1[5] * ictm[3] + ictm[5];

  // construct a (device space) -> (pattern space) transform matrix
  det = 1 / (m1[0] * m1[3] - m1[1] * m1[2]);
  imb[0] = m1[3] * det;
  imb[1] = -m1[1] * det;
  imb[2] = -m1[2] * det;
  imb[3] = m1[0] * det;
  imb[4] = (m1[2] * m1[5] - m1[3] * m1[4]) * det;
  imb[5] = (m1[1] * m1[4] - m1[0] * m1[5]) * det;

  // save current graphics state
  savedState = saveStateStack();

  // set underlying color space (for uncolored tiling patterns); set
  // various other parameters (stroke color, line width) to match
  // Adobe's behavior
  state->setFillPattern(NULL);
  state->setStrokePattern(NULL);
  if (tPat->getPaintType() == 2 && (cs = patCS->getUnder())) 
  {
    state->setFillColorSpace(cs->copy());
    out->updateFillColorSpace(state);
    state->setStrokeColorSpace(cs->copy());
    out->updateStrokeColorSpace(state);
    state->setStrokeColor(state->getFillColor());
    out->updateFillColor(state);
    out->updateStrokeColor(state);
  } 
  else 
  {
    state->setFillColorSpace(new XWDeviceGrayColorSpace());
    out->updateFillColorSpace(state);
    state->setStrokeColorSpace(new XWDeviceGrayColorSpace());
    out->updateStrokeColorSpace(state);
  }
  if (!stroke) 
  {
    state->setLineWidth(0);
    out->updateLineWidth(state);
  }

  // clip to current path
  if (stroke) 
  {
    state->clipToStrokePath();
    out->clipToStrokePath(state);
  } 
  else if (!text) 
  {
    state->clip();
    if (eoFill) 
    {
      out->eoClip(state);
    } 
    else 
    {
      out->clip(state);
    }
  }
  state->clearPath();

  // get the clip region, check for empty
  state->getClipBBox(&cxMin, &cyMin, &cxMax, &cyMax);
  if (cxMin > cxMax || cyMin > cyMax) 
  {
    goto err;
  }

  // transform clip region bbox to pattern space
  xMin = xMax = cxMin * imb[0] + cyMin * imb[2] + imb[4];
  yMin = yMax = cxMin * imb[1] + cyMin * imb[3] + imb[5];
  x1 = cxMin * imb[0] + cyMax * imb[2] + imb[4];
  y1 = cxMin * imb[1] + cyMax * imb[3] + imb[5];
  if (x1 < xMin) 
  {
    xMin = x1;
  } 
  else if (x1 > xMax) 
  {
    xMax = x1;
  }
  if (y1 < yMin) 
  {
    yMin = y1;
  } 
  else if (y1 > yMax) 
  {
    yMax = y1;
  }
  x1 = cxMax * imb[0] + cyMin * imb[2] + imb[4];
  y1 = cxMax * imb[1] + cyMin * imb[3] + imb[5];
  if (x1 < xMin) 
  {
    xMin = x1;
  } 
  else if (x1 > xMax) 
  {
    xMax = x1;
  }
  if (y1 < yMin) 
  {
    yMin = y1;
  } 
  else if (y1 > yMax) 
  {
    yMax = y1;
  }
  x1 = cxMax * imb[0] + cyMax * imb[2] + imb[4];
  y1 = cxMax * imb[1] + cyMax * imb[3] + imb[5];
  if (x1 < xMin) 
  {
    xMin = x1;
  } 
  else if (x1 > xMax) 
  {
    xMax = x1;
  }
  if (y1 < yMin) 
  {
    yMin = y1;
  } 
  else if (y1 > yMax) 
  {
    yMax = y1;
  }

  // draw the pattern
  //~ this should treat negative steps differently -- start at right/top
  //~ edge instead of left/bottom (?)
  xstep = fabs(tPat->getXStep());
  ystep = fabs(tPat->getYStep());
  xi0 = (int)ceil((xMin - tPat->getBBox()[2]) / xstep);
  xi1 = (int)floor((xMax - tPat->getBBox()[0]) / xstep) + 1;
  yi0 = (int)ceil((yMin - tPat->getBBox()[3]) / ystep);
  yi1 = (int)floor((yMax - tPat->getBBox()[1]) / ystep) + 1;
  for (i = 0; i < 4; ++i) 
  {
    m1[i] = m[i];
  }
  if (out->useTilingPatternFill()) 
  {
    m1[4] = m[4];
    m1[5] = m[5];
    out->tilingPatternFill(state, this, tPat->getContentStream(),
			   tPat->getPaintType(), tPat->getResDict(),
			   m1, tPat->getBBox(),
			   xi0, yi0, xi1, yi1, xstep, ystep);
  } 
  else 
  {
    for (yi = yi0; yi < yi1; ++yi) 
    {
      for (xi = xi0; xi < xi1; ++xi) 
      {
				x = xi * xstep;
				y = yi * ystep;
				m1[4] = x * m[0] + y * m[2] + m[4];
				m1[5] = x * m[1] + y * m[3] + m[5];
				drawForm(tPat->getContentStream(), tPat->getResDict(), m1, tPat->getBBox());
      }
    }
  }

  // restore graphics state
 err:
  restoreStateStack(savedState);
}

void XWGraphix::execOp(XWObject *cmd, XWObject args[], int numArgs)
{
  Operator *op;
  char *name;
  XWObject *argPtr;
  int i;

  // find operator
  name = cmd->getCmd();  
  if (!(op = findOp(name))) {
    if (ignoreUndef == 0)
      xwApp->error("Unknown operator '{0:s}'");
    return;
  }

  // type check args
  argPtr = args;
  if (op->numArgs >= 0) {
    if (numArgs < op->numArgs) {
      xwApp->error("Too few ({0:d}) args to '{1:s}' operator");
      return;
    }
    if (numArgs > op->numArgs) {
#if 0
      xwApp->error("Too many ({0:d}) args to '{1:s}' operator");
#endif
      argPtr += numArgs - op->numArgs;
      numArgs = op->numArgs;
    }
  } else {
    if (numArgs > -op->numArgs) {
      xwApp->error("Too many ({0:d}) args to '{1:s}' operator");
      return;
    }
  }
  for (i = 0; i < numArgs; ++i) {
    if (!checkArg(&argPtr[i], op->tchk[i])) {
      xwApp->error("Arg #{0:d} to '{1:s}' operator is wrong type ({2:s})");
      return;
    }
  }
  
  // do it
  (this->*op->func)(argPtr, numArgs);
}

void XWGraphix::fillPatch(XWPatch *patch, int nComps, int depth)
{
  XWPatch patch00, patch01, patch10, patch11;
  double xx[4][8], yy[4][8];
  double xxm, yym;
  int i;

  for (i = 0; i < nComps; ++i) {
    if (abs(patch->color[0][0].c[i] - patch->color[0][1].c[i])
	  > patchColorDelta ||
	abs(patch->color[0][1].c[i] - patch->color[1][1].c[i])
	  > patchColorDelta ||
	abs(patch->color[1][1].c[i] - patch->color[1][0].c[i])
	  > patchColorDelta ||
	abs(patch->color[1][0].c[i] - patch->color[0][0].c[i])
	  > patchColorDelta) {
      break;
    }
  }
  if (i == nComps || depth == patchMaxDepth) {
    state->setFillColor(&patch->color[0][0]);
    out->updateFillColor(state);
    state->moveTo(patch->x[0][0], patch->y[0][0]);
    state->curveTo(patch->x[0][1], patch->y[0][1],
		   patch->x[0][2], patch->y[0][2],
		   patch->x[0][3], patch->y[0][3]);
    state->curveTo(patch->x[1][3], patch->y[1][3],
		   patch->x[2][3], patch->y[2][3],
		   patch->x[3][3], patch->y[3][3]);
    state->curveTo(patch->x[3][2], patch->y[3][2],
		   patch->x[3][1], patch->y[3][1],
		   patch->x[3][0], patch->y[3][0]);
    state->curveTo(patch->x[2][0], patch->y[2][0],
		   patch->x[1][0], patch->y[1][0],
		   patch->x[0][0], patch->y[0][0]);
    state->closePath();
    out->fill(state);
    state->clearPath();
  } else {
    for (i = 0; i < 4; ++i) {
      xx[i][0] = patch->x[i][0];
      yy[i][0] = patch->y[i][0];
      xx[i][1] = 0.5 * (patch->x[i][0] + patch->x[i][1]);
      yy[i][1] = 0.5 * (patch->y[i][0] + patch->y[i][1]);
      xxm = 0.5 * (patch->x[i][1] + patch->x[i][2]);
      yym = 0.5 * (patch->y[i][1] + patch->y[i][2]);
      xx[i][6] = 0.5 * (patch->x[i][2] + patch->x[i][3]);
      yy[i][6] = 0.5 * (patch->y[i][2] + patch->y[i][3]);
      xx[i][2] = 0.5 * (xx[i][1] + xxm);
      yy[i][2] = 0.5 * (yy[i][1] + yym);
      xx[i][5] = 0.5 * (xxm + xx[i][6]);
      yy[i][5] = 0.5 * (yym + yy[i][6]);
      xx[i][3] = xx[i][4] = 0.5 * (xx[i][2] + xx[i][5]);
      yy[i][3] = yy[i][4] = 0.5 * (yy[i][2] + yy[i][5]);
      xx[i][7] = patch->x[i][3];
      yy[i][7] = patch->y[i][3];
    }
    for (i = 0; i < 4; ++i) {
      patch00.x[0][i] = xx[0][i];
      patch00.y[0][i] = yy[0][i];
      patch00.x[1][i] = 0.5 * (xx[0][i] + xx[1][i]);
      patch00.y[1][i] = 0.5 * (yy[0][i] + yy[1][i]);
      xxm = 0.5 * (xx[1][i] + xx[2][i]);
      yym = 0.5 * (yy[1][i] + yy[2][i]);
      patch10.x[2][i] = 0.5 * (xx[2][i] + xx[3][i]);
      patch10.y[2][i] = 0.5 * (yy[2][i] + yy[3][i]);
      patch00.x[2][i] = 0.5 * (patch00.x[1][i] + xxm);
      patch00.y[2][i] = 0.5 * (patch00.y[1][i] + yym);
      patch10.x[1][i] = 0.5 * (xxm + patch10.x[2][i]);
      patch10.y[1][i] = 0.5 * (yym + patch10.y[2][i]);
      patch00.x[3][i] = 0.5 * (patch00.x[2][i] + patch10.x[1][i]);
      patch00.y[3][i] = 0.5 * (patch00.y[2][i] + patch10.y[1][i]);
      patch10.x[0][i] = patch00.x[3][i];
      patch10.y[0][i] = patch00.y[3][i];
      patch10.x[3][i] = xx[3][i];
      patch10.y[3][i] = yy[3][i];
    }
    for (i = 4; i < 8; ++i) {
      patch01.x[0][i-4] = xx[0][i];
      patch01.y[0][i-4] = yy[0][i];
      patch01.x[1][i-4] = 0.5 * (xx[0][i] + xx[1][i]);
      patch01.y[1][i-4] = 0.5 * (yy[0][i] + yy[1][i]);
      xxm = 0.5 * (xx[1][i] + xx[2][i]);
      yym = 0.5 * (yy[1][i] + yy[2][i]);
      patch11.x[2][i-4] = 0.5 * (xx[2][i] + xx[3][i]);
      patch11.y[2][i-4] = 0.5 * (yy[2][i] + yy[3][i]);
      patch01.x[2][i-4] = 0.5 * (patch01.x[1][i-4] + xxm);
      patch01.y[2][i-4] = 0.5 * (patch01.y[1][i-4] + yym);
      patch11.x[1][i-4] = 0.5 * (xxm + patch11.x[2][i-4]);
      patch11.y[1][i-4] = 0.5 * (yym + patch11.y[2][i-4]);
      patch01.x[3][i-4] = 0.5 * (patch01.x[2][i-4] + patch11.x[1][i-4]);
      patch01.y[3][i-4] = 0.5 * (patch01.y[2][i-4] + patch11.y[1][i-4]);
      patch11.x[0][i-4] = patch01.x[3][i-4];
      patch11.y[0][i-4] = patch01.y[3][i-4];
      patch11.x[3][i-4] = xx[3][i];
      patch11.y[3][i-4] = yy[3][i];
    }
    //~ if the shading has a Function, this should interpolate on the
    //~ function parameter, not on the color components
    for (i = 0; i < nComps; ++i) {
      patch00.color[0][0].c[i] = patch->color[0][0].c[i];
      patch00.color[0][1].c[i] = (patch->color[0][0].c[i] +
				  patch->color[0][1].c[i]) / 2;
      patch01.color[0][0].c[i] = patch00.color[0][1].c[i];
      patch01.color[0][1].c[i] = patch->color[0][1].c[i];
      patch01.color[1][1].c[i] = (patch->color[0][1].c[i] +
				  patch->color[1][1].c[i]) / 2;
      patch11.color[0][1].c[i] = patch01.color[1][1].c[i];
      patch11.color[1][1].c[i] = patch->color[1][1].c[i];
      patch11.color[1][0].c[i] = (patch->color[1][1].c[i] +
				  patch->color[1][0].c[i]) / 2;
      patch10.color[1][1].c[i] = patch11.color[1][0].c[i];
      patch10.color[1][0].c[i] = patch->color[1][0].c[i];
      patch10.color[0][0].c[i] = (patch->color[1][0].c[i] +
				  patch->color[0][0].c[i]) / 2;
      patch00.color[1][0].c[i] = patch10.color[0][0].c[i];
      patch00.color[1][1].c[i] = (patch00.color[1][0].c[i] +
				  patch01.color[1][1].c[i]) / 2;
      patch01.color[1][0].c[i] = patch00.color[1][1].c[i];
      patch11.color[0][0].c[i] = patch00.color[1][1].c[i];
      patch10.color[0][1].c[i] = patch00.color[1][1].c[i];
    }
    fillPatch(&patch00, nComps, depth + 1);
    fillPatch(&patch10, nComps, depth + 1);
    fillPatch(&patch01, nComps, depth + 1);
    fillPatch(&patch11, nComps, depth + 1);
  }
}

Operator * XWGraphix::findOp(const char *name)
{
  int a, b, m, cmp;

  a = -1;
  b = numOps;
  cmp = 0; // make gcc happy
  // invariant: opTab[a] < name < opTab[b]
  while (b - a > 1) {
    m = (a + b) / 2;
    cmp = strcmp(opTab[m].name, name);
    if (cmp < 0)
      a = m;
    else if (cmp > 0)
      b = m;
    else
      a = b = m;
  }
  if (cmp != 0)
    return NULL;
  return &opTab[a];
}

int XWGraphix::getPos()
{
    return parser ? parser->getPos() : -1;
}

void XWGraphix::go(bool topLevel)
{
  XWObject obj;
  XWObject args[maxArgs];
  int numArgs, i;
  int lastAbortCheck;

  // scan a sequence of objects
  updateLevel = 1; // make sure even empty pages trigger a call to dump()
  lastAbortCheck = 0;
  numArgs = 0;
  parser->getObj(&obj);
  while (!obj.isEOF()) {

    // got a command - execute it
    if (obj.isCmd()) {
      
      execOp(&obj, args, numArgs);
      obj.free();
      for (i = 0; i < numArgs; ++i)
	args[i].free();
      numArgs = 0;

      // periodically update display
      if (++updateLevel >= 20000) {
	out->dump();
	updateLevel = 0;
      }

      // check for an abort
      if (abortCheckCbk) {
	if (updateLevel - lastAbortCheck > 10) {
	  if ((*abortCheckCbk)(abortCheckCbkData)) {
	    break;
	  }
	  lastAbortCheck = updateLevel;
	}
      }

    // got an argument - save it
    } else if (numArgs < maxArgs) {
      args[numArgs++] = obj;

    // too many arguments - something is wrong
    } else {
      xwApp->error("Too many args in content stream");
      
      obj.free();
    }

    // grab the next object
    parser->getObj(&obj);
  }
  obj.free();

  // args at end with no command
  if (numArgs > 0) {
    xwApp->error("Leftover args in content stream");
    
    for (i = 0; i < numArgs; ++i)
      args[i].free();
  }

  // update display
  if (topLevel && updateLevel > 0) {
    out->dump();
  }
}

void XWGraphix::gouraudFillTriangle(double x0, double y0, CoreColor *color0,
			                         double x1, double y1, CoreColor *color1,
			                         double x2, double y2, CoreColor *color2,
			                         int nComps, int depth)
{
  double x01, y01, x12, y12, x20, y20;
  CoreColor color01, color12, color20;
  int i;

  for (i = 0; i < nComps; ++i) {
    if (abs(color0->c[i] - color1->c[i]) > gouraudColorDelta ||
	abs(color1->c[i] - color2->c[i]) > gouraudColorDelta) {
      break;
    }
  }
  if (i == nComps || depth == gouraudMaxDepth) {
    state->setFillColor(color0);
    out->updateFillColor(state);
    state->moveTo(x0, y0);
    state->lineTo(x1, y1);
    state->lineTo(x2, y2);
    state->closePath();
    out->fill(state);
    state->clearPath();
  } else {
    x01 = 0.5 * (x0 + x1);
    y01 = 0.5 * (y0 + y1);
    x12 = 0.5 * (x1 + x2);
    y12 = 0.5 * (y1 + y2);
    x20 = 0.5 * (x2 + x0);
    y20 = 0.5 * (y2 + y0);
    //~ if the shading has a Function, this should interpolate on the
    //~ function parameter, not on the color components
    for (i = 0; i < nComps; ++i) {
      color01.c[i] = (color0->c[i] + color1->c[i]) / 2;
      color12.c[i] = (color1->c[i] + color2->c[i]) / 2;
      color20.c[i] = (color2->c[i] + color0->c[i]) / 2;
    }
    gouraudFillTriangle(x0, y0, color0, x01, y01, &color01,
			x20, y20, &color20, nComps, depth + 1);
    gouraudFillTriangle(x01, y01, &color01, x1, y1, color1,
			x12, y12, &color12, nComps, depth + 1);
    gouraudFillTriangle(x01, y01, &color01, x12, y12, &color12,
			x20, y20, &color20, nComps, depth + 1);
    gouraudFillTriangle(x20, y20, &color20, x12, y12, &color12,
			x2, y2, color2, nComps, depth + 1);
  }
}

void XWGraphix::opBeginIgnoreUndef(XWObject [], int)
{
    ++ignoreUndef;
}

void XWGraphix::opBeginImage(XWObject [], int)
{
  XWStream *str;
  int c1, c2;

  // NB: this function is run even if ocState is false -- doImage() is
  // responsible for skipping over the inline image data

  // build dict/stream
  str = buildImageStream();

  // display the image
  if (str) {
    doImage(NULL, str, true);
  
    // skip 'EI' tag
    c1 = str->getUndecodedStream()->getChar();
    c2 = str->getUndecodedStream()->getChar();
    while (!(c1 == 'E' && c2 == 'I') && c2 != EOF) {
      c1 = c2;
      c2 = str->getUndecodedStream()->getChar();
    }
    delete str;
  }
}

void XWGraphix::opBeginMarkedContent(XWObject args[], int numArgs)
{
	XWMarkedContent *mc;
  XWObject obj;
  bool ocStateNew;
  XWString *s;
  uint *u;
  int uLen, i;
  MarkedContentKind mcKind;


  mcKind = MCOther;
  if (args[0].isName("OC") && numArgs == 2 && args[1].isName() &&
      res->lookupPropertiesNF(args[1].getName(), &obj)) 
  {
    if (doc->getOptionalContent()->evalOCObject(&obj, &ocStateNew)) 
    {
      ocState = ocStateNew;
    }
    obj.free();
    mcKind = MCOptionalContent;
  } 
  else if (args[0].isName("Span") && numArgs == 2 && args[1].isDict()) 
  {
    if (args[1].dictLookup("ActualText", &obj)->isString()) 
    {
      s = obj.getString();
      if ((s->getChar(0) & 0xff) == 0xfe && (s->getChar(1) & 0xff) == 0xff) 
      {
				uLen = (s->getLength() - 2) / 2;
				u = (uint *)malloc(uLen * sizeof(uint));
				for (i = 0; i < uLen; ++i) 
				{
	  			u[i] = ((s->getChar(2 + 2*i) & 0xff) << 8) | (s->getChar(3 + 2*i) & 0xff);
				}
      } 
      else 
      {
				uLen = s->getLength();
				u = (uint *)malloc(uLen * sizeof(uint));
				for (i = 0; i < uLen; ++i) 
				{
	  			u[i] = pdfDocEncoding[s->getChar(i) & 0xff];
				}
      }
      out->beginActualText(state, u, uLen);
      free(u);
      mcKind = MCActualText;
    }
    obj.free();
  }
  mc = new XWMarkedContent(mcKind, ocState);
  markedContentStack->append(mc);
}

void XWGraphix::opBeginText(XWObject [], int)
{
  state->setTextMat(1, 0, 0, 1, 0, 0);
  state->textMoveTo(0, 0);
  out->updateTextMat(state);
  out->updateTextPos(state);
  fontChanged = true;
  textClipBBoxEmpty = true;
}

void XWGraphix::opClip(XWObject [], int)
{
    clip = clipNormal;
}

void XWGraphix::opCloseEOFillStroke(XWObject [], int)
{
  if (!state->isCurPt()) {
    //error(errSyntaxError, getPos(), "No path in closepath/eofill/stroke");
    return;
  }
  if (state->isPath()) {
    state->closePath();
    if (ocState) {
      if (state->getFillColorSpace()->getMode() == COLOR_SPACE_PATTERN) {
	doPatternFill(true);
      } else {
	out->eoFill(state);
      }
      if (state->getStrokeColorSpace()->getMode() == COLOR_SPACE_PATTERN) {
	doPatternStroke();
      } else {
	out->stroke(state);
      }
    }
  }
  doEndPath();
}

void XWGraphix::opCloseFillStroke(XWObject [], int)
{
  if (!state->isCurPt()) {
    return;
  }
  if (state->isPath()) {
    state->closePath();
    if (ocState) {
      if (state->getFillColorSpace()->getMode() == COLOR_SPACE_PATTERN) {
	doPatternFill(false);
      } else {
	out->fill(state);
      }
      if (state->getStrokeColorSpace()->getMode() == COLOR_SPACE_PATTERN) {
	doPatternStroke();
      } else {
	out->stroke(state);
      }
    }
  }
  doEndPath();
}

void XWGraphix::opClosePath(XWObject [], int)
{
    if (!state->isCurPt()) 
        return;
        
    state->closePath();
}

void XWGraphix::opCloseStroke(XWObject [], int)
{
  if (!state->isCurPt()) {
    //error(errSyntaxError, getPos(), "No path in closepath/stroke");
    return;
  }
  if (state->isPath()) {
    state->closePath();
    if (ocState) {
      if (state->getStrokeColorSpace()->getMode() == COLOR_SPACE_PATTERN) {
	doPatternStroke();
      } else {
	out->stroke(state);
      }
    }
  }
  doEndPath();
}

void XWGraphix::opConcat(XWObject args[], int)
{
  state->concatCTM(args[0].getNum(), args[1].getNum(),
		   args[2].getNum(), args[3].getNum(),
		   args[4].getNum(), args[5].getNum());
  out->updateCTM(state, args[0].getNum(), args[1].getNum(),
		 args[2].getNum(), args[3].getNum(),
		 args[4].getNum(), args[5].getNum());
  fontChanged = true;
}

void XWGraphix::opCurveTo(XWObject args[], int)
{
  double x1, y1, x2, y2, x3, y3;

  if (!state->isCurPt()) {
    return;
  }
  x1 = args[0].getNum();
  y1 = args[1].getNum();
  x2 = args[2].getNum();
  y2 = args[3].getNum();
  x3 = args[4].getNum();
  y3 = args[5].getNum();
  state->curveTo(x1, y1, x2, y2, x3, y3);
}

void XWGraphix::opCurveTo1(XWObject args[], int)
{
  double x1, y1, x2, y2, x3, y3;

  if (!state->isCurPt()) {
    return;
  }
  x1 = state->getCurX();
  y1 = state->getCurY();
  x2 = args[0].getNum();
  y2 = args[1].getNum();
  x3 = args[2].getNum();
  y3 = args[3].getNum();
  state->curveTo(x1, y1, x2, y2, x3, y3);
}

void XWGraphix::opCurveTo2(XWObject args[], int)
{
  double x1, y1, x2, y2, x3, y3;

  if (!state->isCurPt()) {
    return;
  }
  x1 = args[0].getNum();
  y1 = args[1].getNum();
  x2 = args[2].getNum();
  y2 = args[3].getNum();
  x3 = x2;
  y3 = y2;
  state->curveTo(x1, y1, x2, y2, x3, y3);
}

void XWGraphix::opEndIgnoreUndef(XWObject [], int)
{
    if (ignoreUndef > 0)
        --ignoreUndef;
}

void XWGraphix::opEndImage(XWObject [], int)
{
}

void XWGraphix::opEndMarkedContent(XWObject [], int)
{
	XWMarkedContent *mc;
  MarkedContentKind mcKind;

  if (markedContentStack->getLength() > 0) 
  {
    mc = (XWMarkedContent *)markedContentStack->del(markedContentStack->getLength() - 1);
    mcKind = mc->kind;
    delete mc;
    if (mcKind == MCOptionalContent) 
    {
      if (markedContentStack->getLength() > 0) 
      {
				mc = (XWMarkedContent *)markedContentStack->get(markedContentStack->getLength() - 1);
				ocState = mc->ocState;
      } 
      else 
      {
				ocState = true;
      }
    } 
    else if (mcKind == MCActualText) 
    {
      out->endActualText(state);
    }
  }
}

void XWGraphix::opEndPath(XWObject [], int)
{
    doEndPath();
}

void XWGraphix::opEndText(XWObject [], int)
{
    out->endTextObject(state);
}

void XWGraphix::opEOClip(XWObject [], int)
{
    clip = clipEO;
}

void XWGraphix::opEOFill(XWObject [], int)
{
  if (!state->isCurPt()) {
    //error(errSyntaxError, getPos(), "No path in eofill");
    return;
  }
  if (state->isPath()) {
    if (ocState) {
      if (state->getFillColorSpace()->getMode() == COLOR_SPACE_PATTERN) {
	doPatternFill(true);
      } else {
	out->eoFill(state);
      }
    }
  }
  doEndPath();
}

void XWGraphix::opEOFillStroke(XWObject [], int)
{
  if (!state->isCurPt()) {
    //error(errSyntaxError, getPos(), "No path in eofill/stroke");
    return;
  }
  if (state->isPath()) {
    if (ocState) {
      if (state->getFillColorSpace()->getMode() == COLOR_SPACE_PATTERN) {
	doPatternFill(true);
      } else {
	out->eoFill(state);
      }
      if (state->getStrokeColorSpace()->getMode() == COLOR_SPACE_PATTERN) {
	doPatternStroke();
      } else {
	out->stroke(state);
      }
    }
  }
  doEndPath();
}

void XWGraphix::opFill(XWObject [], int)
{
  if (!state->isCurPt()) {
    //error(errSyntaxError, getPos(), "No path in fill");
    return;
  }
  if (state->isPath()) {
    if (ocState) {
      if (state->getFillColorSpace()->getMode() == COLOR_SPACE_PATTERN) {
	doPatternFill(false);
      } else {
	out->fill(state);
      }
    }
  }
  doEndPath();
}

void XWGraphix::opFillStroke(XWObject [], int)
{
  if (!state->isCurPt()) {
    //error(errSyntaxError, getPos(), "No path in fill/stroke");
    return;
  }
  if (state->isPath()) {
    if (ocState) {
      if (state->getFillColorSpace()->getMode() == COLOR_SPACE_PATTERN) {
	doPatternFill(false);
      } else {
	out->fill(state);
      }
      if (state->getStrokeColorSpace()->getMode() == COLOR_SPACE_PATTERN) {
	doPatternStroke();
      } else {
	out->stroke(state);
      }
    }
  }
  doEndPath();
}

void XWGraphix::opImageData(XWObject [], int)
{
}

void XWGraphix::opLineTo(XWObject args[], int)
{
    if (!state->isCurPt()) 
        return;
        
    state->lineTo(args[0].getNum(), args[1].getNum());
}

void XWGraphix::opMarkPoint(XWObject [], int)
{
}

void XWGraphix::opMoveSetShowText(XWObject args[], int)
{
  double tx, ty;

  if (!state->getFont()) {
    return;
  }
  if (fontChanged) {
    out->updateFont(state);
    fontChanged = false;
  }
  state->setWordSpace(args[0].getNum());
  state->setCharSpace(args[1].getNum());
  tx = state->getLineX();
  ty = state->getLineY() - state->getLeading();
  state->textMoveTo(tx, ty);
  out->updateWordSpace(state);
  out->updateCharSpace(state);
  out->updateTextPos(state);
  if (ocState) {
    out->beginStringOp(state);
    doShowText(args[2].getString());
    out->endStringOp(state);
  } else {
    doIncCharCount(args[2].getString());
  }
}

void XWGraphix::opMoveShowText(XWObject args[], int)
{
  double tx, ty;

  if (!state->getFont()) {
    return;
  }
  if (fontChanged) {
    out->updateFont(state);
    fontChanged = false;
  }
  tx = state->getLineX();
  ty = state->getLineY() - state->getLeading();
  state->textMoveTo(tx, ty);
  out->updateTextPos(state);
  if (ocState) {
    out->beginStringOp(state);
    doShowText(args[0].getString());
    out->endStringOp(state);
  } else {
    doIncCharCount(args[0].getString());
  }
}

void XWGraphix::opMoveTo(XWObject args[], int)
{
    state->moveTo(args[0].getNum(), args[1].getNum());
}

void XWGraphix::opRectangle(XWObject args[], int)
{
  double x, y, w, h;

  x = args[0].getNum();
  y = args[1].getNum();
  w = args[2].getNum();
  h = args[3].getNum();
  state->moveTo(x, y);
  state->lineTo(x + w, y);
  state->lineTo(x + w, y + h);
  state->lineTo(x, y + h);
  state->closePath();
}

void XWGraphix::opRestore(XWObject [], int)
{
    restoreState();
}

void XWGraphix::opSave(XWObject [], int)
{
    saveState();
}

void XWGraphix::opSetCacheDevice(XWObject args[], int)
{
  out->type3D1(state, args[0].getNum(), args[1].getNum(),
	       args[2].getNum(), args[3].getNum(),
	       args[4].getNum(), args[5].getNum());
}

void XWGraphix::opSetCharSpacing(XWObject args[], int)
{
    state->setCharSpace(args[0].getNum());
    out->updateCharSpace(state);
}

void XWGraphix::opSetCharWidth(XWObject args[], int)
{
    out->type3D0(state, args[0].getNum(), args[1].getNum());
}

void XWGraphix::opSetDash(XWObject args[], int)
{
  XWArray *a;
  int length;
  XWObject obj;
  double *dash;
  int i;

  a = args[0].getArray();
  length = a->getLength();
  if (length == 0) {
    dash = NULL;
  } else {
    dash = (double *)malloc(length * sizeof(double));
    for (i = 0; i < length; ++i) {
      dash[i] = a->get(i, &obj)->getNum();
      obj.free();
    }
  }
  state->setLineDash(dash, length, args[1].getNum());
  out->updateLineDash(state);
}

void XWGraphix::opSetExtGState(XWObject args[], int)
{
    XWObject obj1, obj2, obj3, obj4, obj5;    
    XWObject args2[2];
  	int mode;
  	bool haveFillOP;
  	XWFunction *funcs[4];
  	CoreColor backdropColor;
  	bool haveBackdropColor;
  	XWColorSpace *blendingColorSpace;
  	bool alpha, isolated, knockout;
  	double opac;
  	int i;
  
    if (!res->lookupGState(args[0].getName(), &obj1)) 
        return;
        
    if (!obj1.isDict()) 
    {
        obj1.free();
        return;
    }
    
    if (obj1.dictLookup("LW", &obj2)->isNum()) 
    {
    	opSetLineWidth(&obj2, 1);
  	}
  	obj2.free();
  	if (obj1.dictLookup("LC", &obj2)->isInt()) 
  	{
    	opSetLineCap(&obj2, 1);
  	}
  	obj2.free();
  	if (obj1.dictLookup("LJ", &obj2)->isInt()) 
  	{
    	opSetLineJoin(&obj2, 1);
  	}
  	obj2.free();
  	if (obj1.dictLookup("ML", &obj2)->isNum()) 
  	{
    	opSetMiterLimit(&obj2, 1);
  	}
  	obj2.free();
  	if (obj1.dictLookup("D", &obj2)->isArray() && obj2.arrayGetLength() == 2) 
  	{
    	obj2.arrayGet(0, &args2[0]);
    	obj2.arrayGet(1, &args2[1]);
    	if (args2[0].isArray() && args2[1].isNum()) 
    	{
      	opSetDash(args2, 2);
    	}
    	args2[0].free();
    	args2[1].free();
  	}
  	obj2.free();
  	
  	if (obj1.dictLookup("FL", &obj2)->isNum()) 
  	{
    	opSetFlat(&obj2, 1);
  	}
  	obj2.free();

  	// transparency support: blend mode, fill/stroke opacity
  	if (!obj1.dictLookup("BM", &obj2)->isNull()) 
  	{
    	if (state->parseBlendMode(&obj2, &mode)) 
    	{
      	state->setBlendMode(mode);
      	out->updateBlendMode(state);
    	} 
  	}
  	obj2.free();
  	if (obj1.dictLookup("ca", &obj2)->isNum()) 
  	{
    	opac = obj2.getNum();
    	state->setFillOpacity(opac < 0 ? 0 : opac > 1 ? 1 : opac);
    	out->updateFillOpacity(state);
  	}
  	obj2.free();
  	if (obj1.dictLookup("CA", &obj2)->isNum()) 
  	{
    	opac = obj2.getNum();
    	state->setStrokeOpacity(opac < 0 ? 0 : opac > 1 ? 1 : opac);
    	out->updateStrokeOpacity(state);
  	}
  	obj2.free();

  	// fill/stroke overprint, overprint mode
  	if ((haveFillOP = (obj1.dictLookup("op", &obj2)->isBool()))) 
  	{
    	state->setFillOverprint(obj2.getBool());
    	out->updateFillOverprint(state);
  	}
  	obj2.free();
  	if (obj1.dictLookup("OP", &obj2)->isBool()) 
  	{
    	state->setStrokeOverprint(obj2.getBool());
    	out->updateStrokeOverprint(state);
    	if (!haveFillOP) 
    	{
      	state->setFillOverprint(obj2.getBool());
      	out->updateFillOverprint(state);
    	}
  	}
  	obj2.free();
  	if (obj1.dictLookup("OPM", &obj2)->isInt()) 
  	{
    	state->setOverprintMode(obj2.getInt());
    	out->updateOverprintMode(state);
  	}
  	obj2.free();

  	// stroke adjust
  	if (obj1.dictLookup("SA", &obj2)->isBool()) 
  	{
    	state->setStrokeAdjust(obj2.getBool());
    	out->updateStrokeAdjust(state);
  	}
  	obj2.free();

  	// transfer function
  	if (obj1.dictLookup("TR2", &obj2)->isNull()) 
  	{
    	obj2.free();
    	obj1.dictLookup("TR", &obj2);
  	}
  	if (obj2.isName("Default") ||  obj2.isName("Identity")) 
  	{
    	funcs[0] = funcs[1] = funcs[2] = funcs[3] = NULL;
    	state->setTransfer(funcs);
    	out->updateTransfer(state);
  	} 
  	else if (obj2.isArray() && obj2.arrayGetLength() == 4) 
  	{
    	for (i = 0; i < 4; ++i) 
    	{
      	obj2.arrayGet(i, &obj3);
      	funcs[i] = XWFunction::parse(&obj3);
      	obj3.free();
      	if (!funcs[i]) 
      	{
					break;
      	}
    	}
    	if (i == 4) 
    	{
      	state->setTransfer(funcs);
      	out->updateTransfer(state);
    	}
  	} 
  	else if (obj2.isName() || obj2.isDict() || obj2.isStream()) 
  	{
    	if ((funcs[0] = XWFunction::parse(&obj2))) 
    	{
      	funcs[1] = funcs[2] = funcs[3] = NULL;
      	state->setTransfer(funcs);
      	out->updateTransfer(state);
    	}
  	} 
  	obj2.free();

  // soft mask
  	if (!obj1.dictLookup("SMask", &obj2)->isNull()) 
  	{
    	if (obj2.isName("None")) 
    	{
      	out->clearSoftMask(state);
    	} 
    	else if (obj2.isDict()) 
    	{
      	if (obj2.dictLookup("S", &obj3)->isName("Alpha")) 
      	{
					alpha = true;
      	} 
      	else 
      	{ // "Luminosity"
					alpha = false;
      	}
      	obj3.free();
      	funcs[0] = NULL;
      	if (!obj2.dictLookup("TR", &obj3)->isNull()) 
      	{
					if (obj3.isName("Default") || obj3.isName("Identity")) 
					{
	  				funcs[0] = NULL;
					} 
					else 
					{
	  				funcs[0] = XWFunction::parse(&obj3);
	  				if (funcs[0]->getInputSize() != 1 || funcs[0]->getOutputSize() != 1) 
	  				{
	    				delete funcs[0];
	    				funcs[0] = NULL;
	  				}
					}
      	}
      	obj3.free();
      	if ((haveBackdropColor = obj2.dictLookup("BC", &obj3)->isArray())) 
      	{
					for (i = 0; i < COLOR_MAX_COMPS; ++i) 
					{
	  				backdropColor.c[i] = 0;
					}
					for (i = 0; i < obj3.arrayGetLength() && i < COLOR_MAX_COMPS; ++i) 
					{
	  				obj3.arrayGet(i, &obj4);
	  				if (obj4.isNum()) 
	  				{
	    				backdropColor.c[i] = dblToCol(obj4.getNum());
	  				}
	  				obj4.free();
					}
      	}
      	obj3.free();
      if (obj2.dictLookup("G", &obj3)->isStream()) 
      {
				if (obj3.streamGetDict()->lookup("Group", &obj4)->isDict()) 
				{
	  			blendingColorSpace = NULL;
	  			isolated = knockout = false;
	  			if (!obj4.dictLookup("CS", &obj5)->isNull()) 
	  			{
	    			blendingColorSpace = XWColorSpace::parse(&obj5);
	  			}
	  			obj5.free();
	  			if (obj4.dictLookup("I", &obj5)->isBool()) 
	  			{
	    			isolated = obj5.getBool();
	  			}
	  			obj5.free();
	  			if (obj4.dictLookup("K", &obj5)->isBool()) 
	  			{
	    			knockout = obj5.getBool();
	  			}
	  			obj5.free();
	  			if (!haveBackdropColor) 
	  			{
	    			if (blendingColorSpace) 
	    			{
	      			blendingColorSpace->getDefaultColor(&backdropColor);
	    			} 
	    			else 
	    			{
	      			//~ need to get the parent or default color space (?)
	      			for (i = 0; i < COLOR_MAX_COMPS; ++i) 
	      			{
								backdropColor.c[i] = 0;
	      			}
	    			}
	  			}
	  			doSoftMask(&obj3, alpha, blendingColorSpace,
		     	isolated, knockout, funcs[0], &backdropColor);
	  			if (funcs[0]) 
	  			{
	    			delete funcs[0];
	  			}
				} 
				obj4.free();
      } 
      obj3.free();
    } 
  }
  obj2.free();

  obj1.free();
}

void XWGraphix::opSetFillCMYKColor(XWObject args[], int)
{
    state->setFillPattern(NULL);
    state->setFillColorSpace(new XWDeviceCMYKColorSpace());
    out->updateFillColorSpace(state);
    
    CoreColor color;
    for (int i = 0; i < 4; ++i) 
        color.c[i] = dblToCol(args[i].getNum());
    state->setFillColor(&color);
    out->updateFillColor(state);
}

void XWGraphix::opSetFillColor(XWObject args[], int numArgs)
{
    if (numArgs != state->getFillColorSpace()->getNComps()) 
        return;
    
    state->setFillPattern(0);
    CoreColor color;
    for (int i = 0; i < numArgs; ++i) 
        color.c[i] = dblToCol(args[i].getNum());
    state->setFillColor(&color);
    out->updateFillColor(state);
}

void XWGraphix::opSetFillColorN(XWObject args[], int numArgs)
{
  CoreColor color;
  XWPattern *pattern;
  int i;

  if (state->getFillColorSpace()->getMode() == COLOR_SPACE_PATTERN) {
    if (numArgs > 1) {
      if (!((XWPatternColorSpace *)state->getFillColorSpace())->getUnder() ||
	  numArgs - 1 != ((XWPatternColorSpace *)state->getFillColorSpace())
	                     ->getUnder()->getNComps()) {
	xwApp->error("Incorrect number of arguments in 'scn' command");
	return;
      }
      for (i = 0; i < numArgs - 1 && i < COLOR_MAX_COMPS; ++i) {
	if (args[i].isNum()) {
	  color.c[i] = dblToCol(args[i].getNum());
	}
      }
      state->setFillColor(&color);
      out->updateFillColor(state);
    }
    if (args[numArgs-1].isName() &&
	(pattern = res->lookupPattern(args[numArgs-1].getName()))) {
      state->setFillPattern(pattern);
    }

  } else {
    if (numArgs != state->getFillColorSpace()->getNComps()) {
      xwApp->error("Incorrect number of arguments in 'scn' command");
      return;
    }
    state->setFillPattern(NULL);
    for (i = 0; i < numArgs && i < COLOR_MAX_COMPS; ++i) {
      if (args[i].isNum()) {
	color.c[i] = dblToCol(args[i].getNum());
      }
    }
    state->setFillColor(&color);
    out->updateFillColor(state);
  }
}

void XWGraphix::opSetFillColorSpace(XWObject args[], int)
{
    state->setFillPattern(0);
    
    XWObject obj;
    res->lookupColorSpace(args[0].getName(), &obj);
    XWColorSpace * colorSpace = 0;
    if (obj.isNull()) 
        colorSpace = XWColorSpace::parse(&args[0]);
    else 
        colorSpace = XWColorSpace::parse(&obj);
    
    obj.free();
    if (colorSpace) 
    {
        state->setFillColorSpace(colorSpace);
        out->updateFillColorSpace(state);
        
        CoreColor color;
        colorSpace->getDefaultColor(&color);
        state->setFillColor(&color);
        out->updateFillColor(state);
    } 
}

void XWGraphix::opSetFillGray(XWObject args[], int)
{    
    state->setFillPattern(NULL);
    state->setFillColorSpace(new XWDeviceGrayColorSpace());
    out->updateFillColorSpace(state);
    
    CoreColor color;
    color.c[0] = dblToCol(args[0].getNum());
    state->setFillColor(&color);
    out->updateFillColor(state);
}

void XWGraphix::opSetFillRGBColor(XWObject args[], int)
{
    state->setFillPattern(NULL);
    state->setFillColorSpace(new XWDeviceRGBColorSpace());
    out->updateFillColorSpace(state);
    
    CoreColor color;
    for (int i = 0; i < 3; ++i) 
        color.c[i] = dblToCol(args[i].getNum());
    state->setFillColor(&color);
    out->updateFillColor(state);
}

void XWGraphix::opSetFlat(XWObject args[], int)
{
    state->setFlatness((int)args[0].getNum());
    out->updateFlatness(state);
}

void XWGraphix::opSetFont(XWObject args[], int)
{
    XWCoreFont *font = res->lookupFont(args[0].getName());
    if (!font) 
        return;
    
    state->setFont(font, args[1].getNum());
    fontChanged = true;
}

void XWGraphix::opSetHorizScaling(XWObject args[], int)
{
    state->setHorizScaling(args[0].getNum());
    out->updateHorizScaling(state);
    fontChanged = true;
}

void XWGraphix::opSetLineCap(XWObject args[], int)
{
    state->setLineCap(args[0].getInt());
    out->updateLineCap(state);
}

void XWGraphix::opSetLineJoin(XWObject args[], int)
{
    state->setLineJoin(args[0].getInt());
    out->updateLineJoin(state);
}

void XWGraphix::opSetLineWidth(XWObject args[], int)
{
    state->setLineWidth(args[0].getNum());
    out->updateLineWidth(state);
}

void XWGraphix::opSetMiterLimit(XWObject args[], int)
{
    state->setMiterLimit(args[0].getNum());
    out->updateMiterLimit(state);
}

void XWGraphix::opSetRenderingIntent(XWObject [], int)
{
}

void XWGraphix::opSetStrokeCMYKColor(XWObject args[], int)
{
    state->setStrokePattern(NULL);
    state->setStrokeColorSpace(new XWDeviceCMYKColorSpace());
    out->updateStrokeColorSpace(state);
    
    CoreColor color;
    for (int i = 0; i < 4; ++i) 
        color.c[i] = dblToCol(args[i].getNum());
    state->setStrokeColor(&color);
    out->updateStrokeColor(state);
}

void XWGraphix::opSetStrokeColor(XWObject args[], int numArgs)
{
    if (numArgs != state->getStrokeColorSpace()->getNComps()) 
        return;
    
    state->setStrokePattern(0);
    CoreColor color;
    for (int i = 0; i < numArgs; ++i) 
        color.c[i] = dblToCol(args[i].getNum());
    state->setStrokeColor(&color);
    out->updateStrokeColor(state);
}

void XWGraphix::opSetStrokeColorN(XWObject args[], int numArgs)
{
  CoreColor color;
  XWPattern *pattern;
  int i;

  if (state->getStrokeColorSpace()->getMode() == COLOR_SPACE_PATTERN) {
    if (numArgs > 1) {
      if (!((XWPatternColorSpace *)state->getStrokeColorSpace())
	       ->getUnder() ||
	  numArgs - 1 != ((XWPatternColorSpace *)state->getStrokeColorSpace())
	                     ->getUnder()->getNComps()) {
	xwApp->error("Incorrect number of arguments in 'SCN' command");
	return;
      }
      for (i = 0; i < numArgs - 1 && i < COLOR_MAX_COMPS; ++i) {
	if (args[i].isNum()) {
	  color.c[i] = dblToCol(args[i].getNum());
	}
      }
      state->setStrokeColor(&color);
      out->updateStrokeColor(state);
    }
    if (args[numArgs-1].isName() &&
	(pattern = res->lookupPattern(args[numArgs-1].getName()))) {
      state->setStrokePattern(pattern);
    }

  } else {
    if (numArgs != state->getStrokeColorSpace()->getNComps()) {
      xwApp->error("Incorrect number of arguments in 'SCN' command");
      return;
    }
    state->setStrokePattern(NULL);
    for (i = 0; i < numArgs && i < COLOR_MAX_COMPS; ++i) {
      if (args[i].isNum()) {
	color.c[i] = dblToCol(args[i].getNum());
      }
    }
    state->setStrokeColor(&color);
    out->updateStrokeColor(state);
  }
}

void XWGraphix::opSetStrokeColorSpace(XWObject args[], int)
{
    state->setStrokePattern(0);
    XWObject obj;
    res->lookupColorSpace(args[0].getName(), &obj);
    XWColorSpace * colorSpace = 0;
    if (obj.isNull()) 
        colorSpace = XWColorSpace::parse(&args[0]);
    else 
        colorSpace = XWColorSpace::parse(&obj);
    
    obj.free();
    if (colorSpace) 
    {
        state->setStrokeColorSpace(colorSpace);
        out->updateStrokeColorSpace(state);
        CoreColor color;
        colorSpace->getDefaultColor(&color);
        state->setStrokeColor(&color);
        out->updateStrokeColor(state);
    } 
}

void XWGraphix::opSetStrokeGray(XWObject args[], int)
{
    state->setStrokePattern(NULL);
    state->setStrokeColorSpace(new XWDeviceGrayColorSpace());
    out->updateStrokeColorSpace(state);
    
    CoreColor color;
    color.c[0] = dblToCol(args[0].getNum());
    state->setStrokeColor(&color);
    out->updateStrokeColor(state);
}

void XWGraphix::opSetStrokeRGBColor(XWObject args[], int)
{
    state->setStrokePattern(NULL);
    state->setStrokeColorSpace(new XWDeviceRGBColorSpace());
    out->updateStrokeColorSpace(state);
    
    CoreColor color;
    for (int i = 0; i < 3; ++i) 
        color.c[i] = dblToCol(args[i].getNum());
    state->setStrokeColor(&color);
    out->updateStrokeColor(state);
}

void XWGraphix::opSetTextLeading(XWObject args[], int)
{
    state->setLeading(args[0].getNum());
}

void XWGraphix::opSetTextMatrix(XWObject args[], int)
{
  state->setTextMat(args[0].getNum(), args[1].getNum(),
		    args[2].getNum(), args[3].getNum(),
		    args[4].getNum(), args[5].getNum());
  state->textMoveTo(0, 0);
  out->updateTextMat(state);
  out->updateTextPos(state);
  fontChanged = true;
}

void XWGraphix::opSetTextRender(XWObject args[], int)
{
    state->setRender(args[0].getInt());
    out->updateRender(state);
}

void XWGraphix::opSetTextRise(XWObject args[], int )
{
    state->setRise(args[0].getNum());
    out->updateRise(state);
}

void XWGraphix::opSetWordSpacing(XWObject args[], int)
{
    state->setWordSpace(args[0].getNum());
    out->updateWordSpace(state);
}

void XWGraphix::opShFill(XWObject args[], int)
{
	XWShading *shading;
  XWGraphixState *savedState;
  double xMin, yMin, xMax, yMax;

  if (!ocState) 
    return;

  if (!(shading = res->lookupShading(args[0].getName()))) 
    return;

  // save current graphics state
  savedState = saveStateStack();

  // clip to bbox
  if (shading->getHasBBox()) 
  {
    shading->getBBox(&xMin, &yMin, &xMax, &yMax);
    state->moveTo(xMin, yMin);
    state->lineTo(xMax, yMin);
    state->lineTo(xMax, yMax);
    state->lineTo(xMin, yMax);
    state->closePath();
    state->clip();
    out->clip(state);
    state->clearPath();
  }

  // set the color space
  state->setFillColorSpace(shading->getColorSpace()->copy());
  out->updateFillColorSpace(state);

  out->setInShading(true);

  // do shading type-specific operations
  switch (shading->getType()) 
  {
  	case 1:
    	doFunctionShFill((XWFunctionShading *)shading);
    	break;
    	
  	case 2:
    	doAxialShFill((XWAxialShading *)shading);
    	break;
    	
  	case 3:
    	doRadialShFill((XWRadialShading *)shading);
    	break;
    	
  	case 4:
  	case 5:
    	doGouraudTriangleShFill((XWGouraudTriangleShading *)shading);
    	break;
    	
  	case 6:
  	case 7:
    	doPatchMeshShFill((XWPatchMeshShading *)shading);
    	break;
  }

  out->setInShading(false);

  // restore graphics state
  restoreStateStack(savedState);

  delete shading;
}

void XWGraphix::opShowSpaceText(XWObject args[], int)
{
	XWArray *a;
  XWObject obj;
  int wMode;
  int i;

  if (!state->getFont()) {
    xwApp->error("No font in show/space");
    return;
  }
  if (fontChanged) {
    out->updateFont(state);
    fontChanged = false;
  }
  if (ocState) {
    out->beginStringOp(state);
    wMode = state->getFont()->getWMode();
    a = args[0].getArray();
    for (i = 0; i < a->getLength(); ++i) {
      a->get(i, &obj);
      if (obj.isNum()) {
	if (wMode) {
	  state->textShift(0, -obj.getNum() * 0.001 *
			   state->getFontSize());
	} else {
	  state->textShift(-obj.getNum() * 0.001 *
			   state->getFontSize() *
			   state->getHorizScaling(), 0);
	}
	out->updateTextShift(state, obj.getNum());
      } else if (obj.isString()) {
	doShowText(obj.getString());
      } else {
	xwApp->error("Element of show/space array must be number or string");
      }
      obj.free();
    }
    out->endStringOp(state);
  } else {
    a = args[0].getArray();
    for (i = 0; i < a->getLength(); ++i) {
      a->get(i, &obj);
      if (obj.isString()) {
	doIncCharCount(obj.getString());
      }
      obj.free();
    }
  }
}

void XWGraphix::opShowText(XWObject args[], int)
{
  if (!state->getFont()) {
    return;
  }
  if (fontChanged) {
    out->updateFont(state);
    fontChanged = false;
  }
  if (ocState) {
    out->beginStringOp(state);
    doShowText(args[0].getString());
    out->endStringOp(state);
  } else {
    doIncCharCount(args[0].getString());
  }
}

void XWGraphix::opStroke(XWObject [], int)
{
	if (!state->isCurPt()) {
    //error(errSyntaxError, getPos(), "No path in stroke");
    return;
  }
  if (state->isPath()) {
    if (ocState) {
      if (state->getStrokeColorSpace()->getMode() == COLOR_SPACE_PATTERN) {
	doPatternStroke();
      } else {
	out->stroke(state);
      }
    }
  }
  doEndPath();
}

void XWGraphix::opTextMove(XWObject args[], int)
{
    double tx = state->getLineX() + args[0].getNum();
    double ty = state->getLineY() + args[1].getNum();
    state->textMoveTo(tx, ty);
    out->updateTextPos(state);
}

void XWGraphix::opTextMoveSet(XWObject args[], int)
{
  double tx, ty;

  tx = state->getLineX() + args[0].getNum();
  ty = args[1].getNum();
  state->setLeading(-ty);
  ty += state->getLineY();
  state->textMoveTo(tx, ty);
  out->updateTextPos(state);
}

void XWGraphix::opTextNextLine(XWObject [], int)
{
  double tx, ty;

  tx = state->getLineX();
  ty = state->getLineY() - state->getLeading();
  state->textMoveTo(tx, ty);
  out->updateTextPos(state);
}

void XWGraphix::opXObject(XWObject args[], int)
{
  char *name;
  XWObject obj1, obj2, obj3, refObj;
  XWObject opiDict;

  if (!ocState && !out->needCharCount()) {
    return;
  }
  name = args[0].getName();
  if (!res->lookupXObject(name, &obj1)) {
    return;
  }
  if (!obj1.isStream()) {
    xwApp->error("XObject '{0:s}' is wrong type");
    obj1.free();
    return;
  }
  obj1.streamGetDict()->lookup("OPI", &opiDict);
  if (opiDict.isDict()) {
    out->opiBegin(state, opiDict.getDict());
  }
  obj1.streamGetDict()->lookup("Subtype", &obj2);
  if (obj2.isName("Image")) {
    if (out->needNonText()) {
      res->lookupXObjectNF(name, &refObj);
      doImage(&refObj, obj1.getStream(), false);
      refObj.free();
    }
  } else if (obj2.isName("Form")) {
    res->lookupXObjectNF(name, &refObj);
    if (out->useDrawForm() && refObj.isRef()) {
      out->drawForm(refObj.getRef());
    } else {
      doForm(&obj1);
    }
    refObj.free();
  } else if (obj2.isName("PS")) {
    obj1.streamGetDict()->lookup("Level1", &obj3);
    out->psXObject(obj1.getStream(),
		   obj3.isStream() ? obj3.getStream() : (XWStream *)NULL);
  } else if (obj2.isName()) {
    xwApp->error("Unknown XObject subtype '{0:s}'");
  } else {
    xwApp->error("XObject subtype is missing or wrong type");
  }
  obj2.free();
  if (opiDict.isDict()) {
    out->opiEnd(state, opiDict.getDict());
  }
  opiDict.free();
  obj1.free();
}

void XWGraphix::popResources()
{
    XWGraphixResources * resPtr = res->getNext();
    delete res;
    res = resPtr;
}

void XWGraphix::pushResources(XWDict *resDict)
{
    res = new XWGraphixResources(xref, resDict, res);
}

void XWGraphix::restoreStateStack(XWGraphixState *oldState)
{
	while (state->hasSaves()) {
    restoreState();
  }
  delete state;
  state = oldState;
  out->restoreState(state);
}

XWGraphixState * XWGraphix::saveStateStack()
{
	out->saveState(state);
  XWGraphixState * oldState = state;
  state = state->copy(true);
  return oldState;
}
