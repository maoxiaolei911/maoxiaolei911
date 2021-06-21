/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stddef.h>
#include <math.h>
#include <string.h>
#include "XWObject.h"
#include "XWArray.h"
#include "XWPage.h"
#include "XWGraphixState.h"


struct BlendModeInfo {
  char *name;
  int mode;
};

static BlendModeInfo blendModeNames[] = {
  { "Normal",     BLEND_NORMAL },
  { "Compatible", BLEND_NORMAL },
  { "Multiply",   BLEND_MULTIPLY },
  { "Screen",     BLEND_SCREEN },
  { "Overlay",    BLEND_OVERLAY },
  { "Darken",     BLEND_DARKEN },
  { "Lighten",    BLEND_LIGHTEN },
  { "ColorDodge", BLEND_COLORDODGE },
  { "ColorBurn",  BLEND_COLORBURN },
  { "HardLight",  BLEND_HARDLIGHT },
  { "SoftLight",  BLEND_SOFTLIGHT },
  { "Difference", BLEND_DIFFERENCE },
  { "Exclusion",  BLEND_EXCLUSION },
  { "Hue",        BLEND_HUE },
  { "Saturation", BLEND_SATURATION },
  { "Color",      BLEND_COLOR },
  { "Luminosity", BLEND_LUMINOSITY }
};


#define nBlendModeNames ((int)((sizeof(blendModeNames) / sizeof(BlendModeInfo))))
          
XWGraphixState::XWGraphixState(double hDPIA, 
                               double vDPIA, 
                               XWPDFRectangle *pageBox,
	                           int rotateA, 
	                           bool upsideDown)
{
  double kx, ky;

  hDPI = hDPIA;
  vDPI = vDPIA;
  rotate = rotateA;
  px1 = pageBox->x1;
  py1 = pageBox->y1;
  px2 = pageBox->x2;
  py2 = pageBox->y2;
  kx = hDPI / 72.0;
  ky = vDPI / 72.0;
  if (rotate == 90) {
    ctm[0] = 0;
    ctm[1] = upsideDown ? ky : -ky;
    ctm[2] = kx;
    ctm[3] = 0;
    ctm[4] = -kx * py1;
    ctm[5] = ky * (upsideDown ? -px1 : px2);
    pageWidth = kx * (py2 - py1);
    pageHeight = ky * (px2 - px1);
  } else if (rotate == 180) {
    ctm[0] = -kx;
    ctm[1] = 0;
    ctm[2] = 0;
    ctm[3] = upsideDown ? ky : -ky;
    ctm[4] = kx * px2;
    ctm[5] = ky * (upsideDown ? -py1 : py2);
    pageWidth = kx * (px2 - px1);
    pageHeight = ky * (py2 - py1);
  } else if (rotate == 270) {
    ctm[0] = 0;
    ctm[1] = upsideDown ? -ky : ky;
    ctm[2] = -kx;
    ctm[3] = 0;
    ctm[4] = kx * py2;
    ctm[5] = ky * (upsideDown ? px2 : -px1);
    pageWidth = kx * (py2 - py1);
    pageHeight = ky * (px2 - px1);
  } else {
    ctm[0] = kx;
    ctm[1] = 0;
    ctm[2] = 0;
    ctm[3] = upsideDown ? -ky : ky;
    ctm[4] = -kx * px1;
    ctm[5] = ky * (upsideDown ? py2 : -py1);
    pageWidth = kx * (px2 - px1);
    pageHeight = ky * (py2 - py1);
  }

  fillColorSpace = new XWDeviceGrayColorSpace();
  strokeColorSpace = new XWDeviceGrayColorSpace();
  fillColor.c[0] = 0;
  strokeColor.c[0] = 0;
  fillPattern = NULL;
  strokePattern = NULL;
  blendMode = BLEND_NORMAL;
  fillOpacity = 1;
  strokeOpacity = 1;
  fillOverprint = false;
  strokeOverprint = false;
  overprintMode = 0;
  transfer[0] = transfer[1] = transfer[2] = transfer[3] = NULL;

  lineWidth = 1;
  lineDash = NULL;
  lineDashLength = 0;
  lineDashStart = 0;
  flatness = 1;
  lineJoin = 0;
  lineCap = 0;
  miterLimit = 10;
  strokeAdjust = false;

  font = NULL;
  fontSize = 0;
  textMat[0] = 1; textMat[1] = 0;
  textMat[2] = 0; textMat[3] = 1;
  textMat[4] = 0; textMat[5] = 0;
  charSpace = 0;
  wordSpace = 0;
  horizScaling = 1;
  leading = 0;
  rise = 0;
  render = 0;

  path = new XWPath();
  curX = curY = 0;
  lineX = lineY = 0;

  clipXMin = 0;
  clipYMin = 0;
  clipXMax = pageWidth;
  clipYMax = pageHeight;

  saved = NULL;
}

XWGraphixState::XWGraphixState(XWGraphixState *state, bool copypath)
{
  int i;

  memcpy(this, state, sizeof(XWGraphixState));
  if (fillColorSpace) {
    fillColorSpace = state->fillColorSpace->copy();
  }
  if (strokeColorSpace) {
    strokeColorSpace = state->strokeColorSpace->copy();
  }
  if (fillPattern) {
    fillPattern = state->fillPattern->copy();
  }
  if (strokePattern) {
    strokePattern = state->strokePattern->copy();
  }
  for (i = 0; i < 4; ++i) {
    if (transfer[i]) {
      transfer[i] = state->transfer[i]->copy();
    }
  }
  if (lineDashLength > 0) {
    lineDash = (double *)malloc(lineDashLength * sizeof(double));
    memcpy(lineDash, state->lineDash, lineDashLength * sizeof(double));
  }
  if (copypath) {
    path = state->path->copy();
  }
  saved = NULL;
}

XWGraphixState::~XWGraphixState()
{
  int i;

  if (fillColorSpace) {
    delete fillColorSpace;
  }
  if (strokeColorSpace) {
    delete strokeColorSpace;
  }
  if (fillPattern) {
    delete fillPattern;
  }
  if (strokePattern) {
    delete strokePattern;
  }
  for (i = 0; i < 4; ++i) {
    if (transfer[i]) {
      delete transfer[i];
    }
  }
  if (lineDash)
  free(lineDash);
  if (path) {
    // this gets set to NULL by restore()
    delete path;
  }
}

void  XWGraphixState::clearPath()
{
    if (path)
        delete path;
    path = new XWPath;
}

void  XWGraphixState::clip()
{
  double xMin, yMin, xMax, yMax, x, y;
  XWSubPath *subpath;
  int i, j;

  xMin = xMax = yMin = yMax = 0; // make gcc happy
  for (i = 0; i < path->getNumSubpaths(); ++i) {
    subpath = path->getSubpath(i);
    for (j = 0; j < subpath->getNumPoints(); ++j) {
      transform(subpath->getX(j), subpath->getY(j), &x, &y);
      if (i == 0 && j == 0) {
	xMin = xMax = x;
	yMin = yMax = y;
      } else {
	if (x < xMin) {
	  xMin = x;
	} else if (x > xMax) {
	  xMax = x;
	}
	if (y < yMin) {
	  yMin = y;
	} else if (y > yMax) {
	  yMax = y;
	}
      }
    }
  }
  if (xMin > clipXMin) {
    clipXMin = xMin;
  }
  if (yMin > clipYMin) {
    clipYMin = yMin;
  }
  if (xMax < clipXMax) {
    clipXMax = xMax;
  }
  if (yMax < clipYMax) {
    clipYMax = yMax;
  }
}

void XWGraphixState::clipToRect(double xMin, double yMin, double xMax, double yMax)
{
	double x, y, xMin1, yMin1, xMax1, yMax1;

  transform(xMin, yMin, &x, &y);
  xMin1 = xMax1 = x;
  yMin1 = yMax1 = y;
  transform(xMax, yMin, &x, &y);
  if (x < xMin1) {
    xMin1 = x;
  } else if (x > xMax1) {
    xMax1 = x;
  }
  if (y < yMin1) {
    yMin1 = y;
  } else if (y > yMax1) {
    yMax1 = y;
  }
  transform(xMax, yMax, &x, &y);
  if (x < xMin1) {
    xMin1 = x;
  } else if (x > xMax1) {
    xMax1 = x;
  }
  if (y < yMin1) {
    yMin1 = y;
  } else if (y > yMax1) {
    yMax1 = y;
  }
  transform(xMin, yMax, &x, &y);
  if (x < xMin1) {
    xMin1 = x;
  } else if (x > xMax1) {
    xMax1 = x;
  }
  if (y < yMin1) {
    yMin1 = y;
  } else if (y > yMax1) {
    yMax1 = y;
  }

  if (xMin1 > clipXMin) {
    clipXMin = xMin1;
  }
  if (yMin1 > clipYMin) {
    clipYMin = yMin1;
  }
  if (xMax1 < clipXMax) {
    clipXMax = xMax1;
  }
  if (yMax1 < clipYMax) {
    clipYMax = yMax1;
  }
}

void  XWGraphixState::clipToStrokePath()
{
  double xMin, yMin, xMax, yMax, x, y, t0, t1;
  XWSubPath *subpath;
  int i, j;

  xMin = xMax = yMin = yMax = 0; // make gcc happy
  for (i = 0; i < path->getNumSubpaths(); ++i) {
    subpath = path->getSubpath(i);
    for (j = 0; j < subpath->getNumPoints(); ++j) {
      transform(subpath->getX(j), subpath->getY(j), &x, &y);
      if (i == 0 && j == 0) {
	xMin = xMax = x;
	yMin = yMax = y;
      } else {
	if (x < xMin) {
	  xMin = x;
	} else if (x > xMax) {
	  xMax = x;
	}
	if (y < yMin) {
	  yMin = y;
	} else if (y > yMax) {
	  yMax = y;
	}
      }
    }
  }

  // allow for the line width
  //~ miter joins can extend farther than this
  t0 = fabs(ctm[0]);
  t1 = fabs(ctm[2]);
  if (t0 > t1) {
    xMin -= 0.5 * lineWidth * t0;
    xMax += 0.5 * lineWidth * t0;
  } else {
    xMin -= 0.5 * lineWidth * t1;
    xMax += 0.5 * lineWidth * t1;
  }
  t0 = fabs(ctm[0]);
  t1 = fabs(ctm[3]);
  if (t0 > t1) {
    yMin -= 0.5 * lineWidth * t0;
    yMax += 0.5 * lineWidth * t0;
  } else {
    yMin -= 0.5 * lineWidth * t1;
    yMax += 0.5 * lineWidth * t1;
  }

  if (xMin > clipXMin) {
    clipXMin = xMin;
  }
  if (yMin > clipYMin) {
    clipYMin = yMin;
  }
  if (xMax < clipXMax) {
    clipXMax = xMax;
  }
  if (yMax < clipYMax) {
    clipYMax = yMax;
  }
}

void  XWGraphixState::concatCTM(double a, double b, double c, double d, double e, double f)
{
  double a1 = ctm[0];
  double b1 = ctm[1];
  double c1 = ctm[2];
  double d1 = ctm[3];
  int i;

  ctm[0] = a * a1 + b * c1;
  ctm[1] = a * b1 + b * d1;
  ctm[2] = c * a1 + d * c1;
  ctm[3] = c * b1 + d * d1;
  ctm[4] = e * a1 + f * c1 + ctm[4];
  ctm[5] = e * b1 + f * d1 + ctm[5];

  // avoid FP exceptions on badly messed up PDF files
  for (i = 0; i < 6; ++i) {
    if (ctm[i] > 1e10) {
      ctm[i] = 1e10;
    } else if (ctm[i] < -1e10) {
      ctm[i] = -1e10;
    }
  }
}

void XWGraphixState::getFontTransMat(double *m11, double *m12, double *m21, double *m22)
{
  *m11 = (textMat[0] * ctm[0] + textMat[1] * ctm[2]) * fontSize;
  *m12 = (textMat[0] * ctm[1] + textMat[1] * ctm[3]) * fontSize;
  *m21 = (textMat[2] * ctm[0] + textMat[3] * ctm[2]) * fontSize;
  *m22 = (textMat[2] * ctm[1] + textMat[3] * ctm[3]) * fontSize;
}

double XWGraphixState::getTransformedFontSize()
{
  double x1, y1, x2, y2;

  x1 = textMat[2] * fontSize;
  y1 = textMat[3] * fontSize;
  x2 = ctm[0] * x1 + ctm[2] * y1;
  y2 = ctm[1] * x1 + ctm[3] * y1;
  return sqrt(x2 * x2 + y2 * y2);
}

void XWGraphixState::getUserClipBBox(double *xMin, double *yMin, double *xMax, double *yMax)
{
  double ictm[6];
  double xMin1, yMin1, xMax1, yMax1, det, tx, ty;

  // invert the CTM
  det = 1 / (ctm[0] * ctm[3] - ctm[1] * ctm[2]);
  ictm[0] = ctm[3] * det;
  ictm[1] = -ctm[1] * det;
  ictm[2] = -ctm[2] * det;
  ictm[3] = ctm[0] * det;
  ictm[4] = (ctm[2] * ctm[5] - ctm[3] * ctm[4]) * det;
  ictm[5] = (ctm[1] * ctm[4] - ctm[0] * ctm[5]) * det;

  // transform all four corners of the clip bbox; find the min and max
  // x and y values
  xMin1 = xMax1 = clipXMin * ictm[0] + clipYMin * ictm[2] + ictm[4];
  yMin1 = yMax1 = clipXMin * ictm[1] + clipYMin * ictm[3] + ictm[5];
  tx = clipXMin * ictm[0] + clipYMax * ictm[2] + ictm[4];
  ty = clipXMin * ictm[1] + clipYMax * ictm[3] + ictm[5];
  if (tx < xMin1) {
    xMin1 = tx;
  } else if (tx > xMax1) {
    xMax1 = tx;
  }
  if (ty < yMin1) {
    yMin1 = ty;
  } else if (ty > yMax1) {
    yMax1 = ty;
  }
  tx = clipXMax * ictm[0] + clipYMin * ictm[2] + ictm[4];
  ty = clipXMax * ictm[1] + clipYMin * ictm[3] + ictm[5];
  if (tx < xMin1) {
    xMin1 = tx;
  } else if (tx > xMax1) {
    xMax1 = tx;
  }
  if (ty < yMin1) {
    yMin1 = ty;
  } else if (ty > yMax1) {
    yMax1 = ty;
  }
  tx = clipXMax * ictm[0] + clipYMax * ictm[2] + ictm[4];
  ty = clipXMax * ictm[1] + clipYMax * ictm[3] + ictm[5];
  if (tx < xMin1) {
    xMin1 = tx;
  } else if (tx > xMax1) {
    xMax1 = tx;
  }
  if (ty < yMin1) {
    yMin1 = ty;
  } else if (ty > yMax1) {
    yMax1 = ty;
  }

  *xMin = xMin1;
  *yMin = yMin1;
  *xMax = xMax1;
  *yMax = yMax1;
}

bool XWGraphixState::parseBlendMode(XWObject *obj, int *mode)
{
  XWObject obj2;
  int i, j;

  if (obj->isName()) {
    for (i = 0; i < nBlendModeNames; ++i) {
      if (!strcmp(obj->getName(), blendModeNames[i].name)) {
	*mode = blendModeNames[i].mode;
	return true;
      }
    }
    return false;
  } else if (obj->isArray()) {
    for (i = 0; i < obj->arrayGetLength(); ++i) {
      obj->arrayGet(i, &obj2);
      if (!obj2.isName()) {
	obj2.free();
	return false;
      }
      for (j = 0; j < nBlendModeNames; ++j) {
	if (!strcmp(obj2.getName(), blendModeNames[j].name)) {
	  obj2.free();
	  *mode = blendModeNames[j].mode;
	  return true;
	}
      }
      obj2.free();
    }
    *mode = BLEND_NORMAL;
    return true;
  } else {
    return false;
  }
}

XWGraphixState * XWGraphixState::restore()
{
  XWGraphixState *oldState;

  if (saved) {
    oldState = saved;

    // these attributes aren't saved/restored by the q/Q operators
    oldState->path = path;
    oldState->curX = curX;
    oldState->curY = curY;
    oldState->lineX = lineX;
    oldState->lineY = lineY;

    path = NULL;
    saved = NULL;
    delete this;

  } else {
    oldState = this;
  }

  return oldState;
}

XWGraphixState *XWGraphixState::save()
{
    XWGraphixState * newState = copy();
    newState->saved = this;
  return newState;
}

void  XWGraphixState::setCTM(double a, double b, double c, double d, double e, double f)
{
  int i;

  ctm[0] = a;
  ctm[1] = b;
  ctm[2] = c;
  ctm[3] = d;
  ctm[4] = e;
  ctm[5] = f;

  // avoid FP exceptions on badly messed up PDF files
  for (i = 0; i < 6; ++i) {
    if (ctm[i] > 1e10) {
      ctm[i] = 1e10;
    } else if (ctm[i] < -1e10) {
      ctm[i] = -1e10;
    }
  }
}

void XWGraphixState::setFillColorSpace(XWColorSpace *colorSpace)
{
    if (fillColorSpace) 
        delete fillColorSpace;
    fillColorSpace = colorSpace;
}

void  XWGraphixState::setFillPattern(XWPattern *pattern)
{
    if (fillPattern) 
        delete fillPattern;
    fillPattern = pattern;
}

void XWGraphixState::setLineDash(double *dash, int length, double start)
{
   if (lineDash)
    free(lineDash);
  lineDash = dash;
  lineDashLength = length;
  lineDashStart = start;
}

void  XWGraphixState::setPath(XWPath *pathA)
{
    if (path)
        delete path;
    path = pathA;
}

void  XWGraphixState::setStrokeColorSpace(XWColorSpace *colorSpace)
{
    if (strokeColorSpace) 
        delete strokeColorSpace;
    strokeColorSpace = colorSpace;
}

void XWGraphixState::setStrokePattern(XWPattern *pattern)
{
    if (strokePattern) 
        delete strokePattern;
        
    strokePattern = pattern;
}

void XWGraphixState::setTransfer(XWFunction **funcs)
{
    for (int i = 0; i < 4; ++i) 
    {
        if (transfer[i]) 
            delete transfer[i];
        transfer[i] = funcs[i];
    }
}

void  XWGraphixState::shift(double dx, double dy)
{
    curX += dx;
    curY += dy;
}

void XWGraphixState::shiftCTM(double tx, double ty)
{
  ctm[4] += tx;
  ctm[5] += ty;
  clipXMin += tx;
  clipYMin += ty;
  clipXMax += tx;
  clipYMax += ty;
}

void XWGraphixState::textShift(double tx, double ty)
{
  double dx, dy;

  textTransformDelta(tx, ty, &dx, &dy);
  curX += dx;
  curY += dy;
}

double XWGraphixState::transformWidth(double w)
{
  double x, y;

  x = ctm[0] + ctm[2];
  y = ctm[1] + ctm[3];
  return w * sqrt(0.5 * (x * x + y * y));
}

