/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <QtDebug>
#include <QByteArray>
#include "XWBitmap.h"
#include "XWRasterState.h"
#include "XWRasterPattern.h"
#include "XWScreen.h"
#include "XWRasterPath.h"
#include "XWRasterXPathScanner.h"
#include "XWRasterXPath.h"
#include "XWRasterFont.h"
#include "XWRaster.h"
#include "XWRasterType.h"
#include "XWRasterClip.h"

#define rasterAAGamma 1.5

#define bezierCircle ((double)0.55228475)
#define bezierCircle2 ((double)(0.5 * 0.55228475))

struct ImageSection 
{
  int y0, y1;	
  int ia0, ia1;	
  int ib0, ib1;
  double xa0, ya0, xa1, ya1;
  double dxdya;
  double xb0, yb0, xb1, yb1;
  double dxdyb;
};

struct RasterPipe
{
    int x, y;
    XWRasterPattern *pattern;
    double   aInput;
    bool     usesShape;
    uchar * cSrc;
    uchar   cSrcVal[4];
    uchar * alpha0Ptr;
    uchar * softMaskPtr;
    uchar * destColorPtr;
    int      destColorMask;
    uchar * destAlphaPtr;
    uchar    shape;
    bool     noTransparency;
    int      resultColorCtrl;
    bool     nonIsolatedGroup;
    void (XWRaster::*run)(RasterPipe *pipe);
};

#define PIPE_RESULT_COLOR_NOALPHABLENDMONO  0
#define PIPE_RESULT_COLOR_NOALPHABLENDRGB   1
#define PIPE_RESULT_COLOR_NOALPHABLENDCMYK  2
#define PIPE_RESULT_COLOR_ALPHANOBLENDMONO  3
#define PIPE_RESULT_COLOR_ALPHANOBLENDRGB   4
#define PIPE_RESULT_COLOR_ALPHANOBLENDCMYK  5
#define PIPE_RESULT_COLOR_ALPHABLENDMONO    6
#define PIPE_RESULT_COLOR_ALPHABLENDRGB     7
#define PIPE_RESULT_COLOR_ALPHABLENDCMYK    8

int XWRaster::pipeResultColorNoAlphaBlend[] = {
    PIPE_RESULT_COLOR_NOALPHABLENDMONO,
    PIPE_RESULT_COLOR_NOALPHABLENDMONO,
    PIPE_RESULT_COLOR_NOALPHABLENDRGB,
    PIPE_RESULT_COLOR_NOALPHABLENDRGB,
    PIPE_RESULT_COLOR_NOALPHABLENDCMYK
};

int XWRaster::pipeResultColorAlphaNoBlend[] = {
    PIPE_RESULT_COLOR_ALPHANOBLENDMONO,
    PIPE_RESULT_COLOR_ALPHANOBLENDMONO,
    PIPE_RESULT_COLOR_ALPHANOBLENDRGB,
    PIPE_RESULT_COLOR_ALPHANOBLENDRGB,
    PIPE_RESULT_COLOR_ALPHANOBLENDCMYK
};

int XWRaster::pipeResultColorAlphaBlend[] = {
    PIPE_RESULT_COLOR_ALPHABLENDMONO,
    PIPE_RESULT_COLOR_ALPHABLENDMONO,
    PIPE_RESULT_COLOR_ALPHABLENDRGB,
    PIPE_RESULT_COLOR_ALPHABLENDRGB,
    PIPE_RESULT_COLOR_ALPHABLENDCMYK
};


int colorModeNComps[] = {
  1, 1, 3, 3, 4
};

static void blendXor(uchar * src, uchar * dest, uchar * blend, int cm) 
{
    for (int i = 0; i < colorModeNComps[cm]; ++i) 
        blend[i] = src[i] ^ dest[i];
}


XWRaster::XWRaster(XWBitmap *bitmapA, 
                   bool vectorAntialiasA, 
                   ScreenParams *screenParams)
{
    bitmap = bitmapA;
    vectorAntialias = vectorAntialiasA;
    inShading = false;
    state = new XWRasterState(bitmap->width, bitmap->height, vectorAntialias, screenParams);
    if (vectorAntialias) 
    {
        aaBuf = new XWBitmap(AA_SIZE * bitmap->width, AA_SIZE, 1, COLOR_MODE_MONO1, false);
        for (int i = 0; i <= AA_SIZE * AA_SIZE; ++i) 
            aaGamma[i] = (uchar)rasterRound(rasterPow((double)i / (double)(AA_SIZE * AA_SIZE), rasterAAGamma) * 255);
    } 
    else
        aaBuf = 0;
        
    minLineWidth = 0;
    clearModRegion();
    debugMode = false;
}

XWRaster::XWRaster(XWBitmap *bitmapA, bool vectorAntialiasA, XWScreen *screenA)
{
    bitmap = bitmapA;
    vectorAntialias = vectorAntialiasA;
    inShading = false;
    state = new XWRasterState(bitmap->width, bitmap->height, vectorAntialias, screenA);
    if (vectorAntialias) 
    {
        aaBuf = new XWBitmap(AA_SIZE * bitmap->width, AA_SIZE, 1, COLOR_MODE_MONO1, false);
        for (int i = 0; i <= AA_SIZE * AA_SIZE; ++i) 
            aaGamma[i] = (uchar)rasterRound(rasterPow((double)i / (double)(AA_SIZE * AA_SIZE), rasterAAGamma) * 255);
    } 
    else
        aaBuf = 0;
        
    minLineWidth = 0;
    clearModRegion();
    debugMode = false;
}

XWRaster::~XWRaster()
{
    while (state->next) 
        restoreState();
        
    if (state)
        delete state;
        
    if (vectorAntialias) 
        delete aaBuf;
}

bool XWRaster::blitTransparent(XWBitmap *src, 
	                           int xSrc, 
	                           int ySrc, 
	                           int xDest, 
	                           int yDest, 
	                           int w, 
	                           int h)
{
    return bitmap->blitTransparent(src, xSrc, ySrc, xDest, yDest, w, h);
}

void XWRaster::clear(uchar * color, uchar alpha)
{
	bitmap->clear(color, alpha);
    updateModX(0);
    updateModY(0);
    updateModX(bitmap->width - 1);
    updateModY(bitmap->height - 1);
}

void XWRaster::clearModRegion()
{
    modXMin = bitmap->getWidth();
    modYMin = bitmap->getHeight();
    modXMax = -1;
    modYMax = -1;
}

void XWRaster::clipResetToRect(double x0, 
                               double y0,
		                       double x1, 
		                       double y1)
{
    state->clip->resetToRect(x0, y0, x1, y1);
}

bool XWRaster::clipToPath(XWRasterPath *path, bool eo)
{
    return state->clip->clipToPath(path, state->matrix, state->flatness, eo);
}

bool XWRaster::clipToRect(double x0, 
                          double y0,
		                  double x1, 
		                  double y1)
{
    return state->clip->clipToRect(x0, y0, x1, y1);
}

bool XWRaster::composite(XWBitmap *src, 
                         int xSrc, 
                         int ySrc,
			             int xDest, 
			             int yDest, 
			             int w, 
			             int h,
			             bool noClip, 
			             bool nonIsolated)
{
  RasterPipe pipe;
  uchar pixel[4];
  uchar alpha;
  uchar *ap;
  int x, y;

  if (src->mode != bitmap->mode) {
    return false;
  }

  if (src->alpha) {
    pipeInit(&pipe, xDest, yDest, NULL, pixel,
	     (uchar)rasterRound(state->fillAlpha * 255), true, nonIsolated);
    if (noClip) {
      for (y = 0; y < h; ++y) {
	pipeSetXY(&pipe, xDest, yDest + y);
	ap = src->getAlphaPtr() + (ySrc + y) * src->getWidth() + xSrc;
	for (x = 0; x < w; ++x) {
	  src->getPixel(xSrc + x, ySrc + y, pixel);
	  alpha = *ap++;
	  // this uses shape instead of alpha, which isn't technically
	  // correct, but works out the same
	  pipe.shape = alpha;
	  (this->*pipe.run)(&pipe);
	}
      }
      updateModX(xDest);
      updateModX(xDest + w - 1);
      updateModY(yDest);
      updateModY(yDest + h - 1);
    } else {
      for (y = 0; y < h; ++y) {
	pipeSetXY(&pipe, xDest, yDest + y);
	ap = src->getAlphaPtr() + (ySrc + y) * src->getWidth() + xSrc;
	for (x = 0; x < w; ++x) {
	  src->getPixel(xSrc + x, ySrc + y, pixel);
	  alpha = *ap++;
	  if (state->clip->test(xDest + x, yDest + y)) {
	    // this uses shape instead of alpha, which isn't technically
	    // correct, but works out the same
	    pipe.shape = alpha;
	    (this->*pipe.run)(&pipe);
	    updateModX(xDest + x);
	    updateModY(yDest + y);
	  } else {
	    pipeIncX(&pipe);
	  }
	}
      }
    }
  } else {
    pipeInit(&pipe, xDest, yDest, NULL, pixel,
	     (uchar)rasterRound(state->fillAlpha * 255), false, nonIsolated);
    if (noClip) {
      for (y = 0; y < h; ++y) {
	pipeSetXY(&pipe, xDest, yDest + y);
	for (x = 0; x < w; ++x) {
	  src->getPixel(xSrc + x, ySrc + y, pixel);
	  (this->*pipe.run)(&pipe);
	}
      }
      updateModX(xDest);
      updateModX(xDest + w - 1);
      updateModY(yDest);
      updateModY(yDest + h - 1);
    } else {
      for (y = 0; y < h; ++y) {
	pipeSetXY(&pipe, xDest, yDest + y);
	for (x = 0; x < w; ++x) {
	  src->getPixel(xSrc + x, ySrc + y, pixel);
	  if (state->clip->test(xDest + x, yDest + y)) {
	    (this->*pipe.run)(&pipe);
	    updateModX(xDest + x);
	    updateModY(yDest + y);
	  } else {
	    pipeIncX(&pipe);
	  }
	}
      }
    }
  }

  return true;
}

void XWRaster::compositeBackground(uchar * color)
{
	bitmap->compositeBackground(color);
}

bool XWRaster::drawImage(XWRasterImageSource src, 
                         void *srcData,
			             int srcMode, 
			             bool srcAlpha,
			             int w, 
			             int h, 
			             double *mat)
{
  bool ok;
  XWBitmap *scaledImg;
  int clipRes;
  bool minorAxisZero;
  int x0, y0, x1, y1, scaledWidth, scaledHeight;
  int nComps;
    switch (bitmap->mode)
    {
        case COLOR_MODE_MONO1:
        case COLOR_MODE_MONO8:
            ok = srcMode == COLOR_MODE_MONO8;
            nComps = 1;
            break;
            
        case COLOR_MODE_RGB8:
            ok = srcMode == COLOR_MODE_RGB8;
            nComps = 3;
            break;
            
        case COLOR_MODE_BGR8:
            ok = srcMode == COLOR_MODE_BGR8;
            nComps = 3;
            break;
            
        case COLOR_MODE_CMYK8:
            ok = srcMode == COLOR_MODE_CMYK8;
            nComps = 4;
            break;
    }
    if (!ok)
        return false;
        
    if (!rasterCheckDet(mat[0], mat[1], mat[2], mat[3], 0.000001)) {
    return false;
  }
        
  minorAxisZero = mat[1] == 0 && mat[2] == 0;

  // scaling only
  if (mat[0] > 0 && minorAxisZero && mat[3] > 0) {
    x0 = imgCoordMungeLower(mat[4]);
    y0 = imgCoordMungeLower(mat[5]);
    x1 = imgCoordMungeUpper(mat[0] + mat[4]);
    y1 = imgCoordMungeUpper(mat[3] + mat[5]);
    // make sure narrow images cover at least one pixel
    if (x0 == x1) {
      ++x1;
    }
    if (y0 == y1) {
      ++y1;
    }
    clipRes = state->clip->testRect(x0, y0, x1 - 1, y1 - 1);
    opClipRes = clipRes;
    if (clipRes != CLIP_ALLOUTSIDE) {
      scaledWidth = x1 - x0;
      scaledHeight = y1 - y0;
      scaledImg = scaleImage(src, srcData, srcMode, nComps, srcAlpha, w, h,
			     scaledWidth, scaledHeight);
      blitImage(scaledImg, srcAlpha, x0, y0, clipRes);
      delete scaledImg;
    }
    
  // scaling plus vertical flip
  } else if (mat[0] > 0 && minorAxisZero && mat[3] < 0) {
    x0 = imgCoordMungeLower(mat[4]);
    y0 = imgCoordMungeLower(mat[3] + mat[5]);
    x1 = imgCoordMungeUpper(mat[0] + mat[4]);
    y1 = imgCoordMungeUpper(mat[5]);
    if (x0 == x1) {
      if (mat[4] + mat[0] * 0.5 < x0) {
	--x0;
      } else {
	++x1;
      }
    }
    if (y0 == y1) {
      if (mat[5] + mat[1] * 0.5 < y0) {
	--y0;
      } else {
	++y1;
      }
    }
    clipRes = state->clip->testRect(x0, y0, x1 - 1, y1 - 1);
    opClipRes = clipRes;
    if (clipRes != CLIP_ALLOUTSIDE) {
      scaledWidth = x1 - x0;
      scaledHeight = y1 - y0;
      scaledImg = scaleImage(src, srcData, srcMode, nComps, srcAlpha, w, h,
			     scaledWidth, scaledHeight);
      vertFlipImage(scaledImg, scaledWidth, scaledHeight, nComps);
      blitImage(scaledImg, srcAlpha, x0, y0, clipRes);
      delete scaledImg;
    }

  // all other cases
  } else {
    arbitraryTransformImage(src, srcData, srcMode, nComps, srcAlpha,
			    w, h, mat);
  }

  return true;
}

bool XWRaster::fill(XWRasterPath *path, bool eo)
{
    return fillWithPattern(path, eo, state->fillPattern, state->fillAlpha);
}

bool XWRaster::fillChar(double x, 
	                    double y,
			            int c, 
			            XWRasterFont *font)
{
    double xt, yt;
    transform(state->matrix, x, y, &xt, &yt);
    int x0 = rasterFloor(xt);
    int xFrac = rasterFloor((xt - x0) * RasterFontFraction);
    int y0 = rasterFloor(yt);
    int yFrac = rasterFloor((yt - y0) * RasterFontFraction);
    GlyphBitmap glyph;
    if (!font->getGlyph(c, xFrac, yFrac, &glyph)) 
        return false;
    
    bool err = fillGlyph2(x0, y0, &glyph);
    if (glyph.freeData) 
        free(glyph.data);
    return err;
}

bool XWRaster::fillGlyph(double x, double y, GlyphBitmap *glyph)
{
    double xt, yt;
    transform(state->matrix, x, y, &xt, &yt);
    
    int x0 = rasterFloor(xt);
    int y0 = rasterFloor(yt);
    return fillGlyph2(x0, y0, glyph);
}

bool XWRaster::fillImageMask(XWRasterImageMaskSource src, 
	                         void *srcData,
			                 int w, int h, 
			                 double *mat,
			                 bool glyphMode)
{
  XWBitmap *scaledMask;
  int clipRes;
  bool minorAxisZero;
  int x0, y0, x1, y1, scaledWidth, scaledHeight;

  // check for singular matrix
  if (!rasterCheckDet(mat[0], mat[1], mat[2], mat[3], 0.000001)) {
    return false;
  }

  minorAxisZero = mat[1] == 0 && mat[2] == 0;

  // scaling only
  if (mat[0] > 0 && minorAxisZero && mat[3] > 0) {
    x0 = imgCoordMungeLowerC(mat[4], glyphMode);
    y0 = imgCoordMungeLowerC(mat[5], glyphMode);
    x1 = imgCoordMungeUpperC(mat[0] + mat[4], glyphMode);
    y1 = imgCoordMungeUpperC(mat[3] + mat[5], glyphMode);
    // make sure narrow images cover at least one pixel
    if (x0 == x1) {
      ++x1;
    }
    if (y0 == y1) {
      ++y1;
    }
    clipRes = state->clip->testRect(x0, y0, x1 - 1, y1 - 1);
    opClipRes = clipRes;
    if (clipRes != CLIP_ALLOUTSIDE) {
      scaledWidth = x1 - x0;
      scaledHeight = y1 - y0;
      scaledMask = scaleMask(src, srcData, w, h, scaledWidth, scaledHeight);
      blitMask(scaledMask, x0, y0, clipRes);
      delete scaledMask;
    }
    
  // scaling plus vertical flip
  } else if (mat[0] > 0 && minorAxisZero && mat[3] < 0) {
    x0 = imgCoordMungeLowerC(mat[4], glyphMode);
    y0 = imgCoordMungeLowerC(mat[3] + mat[5], glyphMode);
    x1 = imgCoordMungeUpperC(mat[0] + mat[4], glyphMode);
    y1 = imgCoordMungeUpperC(mat[5], glyphMode);
    // make sure narrow images cover at least one pixel
    if (x0 == x1) {
      ++x1;
    }
    if (y0 == y1) {
      ++y1;
    }
    clipRes = state->clip->testRect(x0, y0, x1 - 1, y1 - 1);
    opClipRes = clipRes;
    if (clipRes != CLIP_ALLOUTSIDE) {
      scaledWidth = x1 - x0;
      scaledHeight = y1 - y0;
      scaledMask = scaleMask(src, srcData, w, h, scaledWidth, scaledHeight);
      vertFlipImage(scaledMask, scaledWidth, scaledHeight, 1);
      blitMask(scaledMask, x0, y0, clipRes);
      delete scaledMask;
    }

  // all other cases
  } else {
    arbitraryTransformMask(src, srcData, w, h, mat, glyphMode);
  }

  return true;
}

XWRasterBlendFunc XWRaster::getBlendFunc()
{
    return state->blendFunc;
}

XWRasterClip * XWRaster::getClip()
{
    return state->clip;
}

double XWRaster::getFillAlpha()
{
    return state->fillAlpha;
}

XWRasterPattern * XWRaster::getFillPattern()
{
    return state->fillPattern;
}

double XWRaster::getFlatness()
{
    return state->flatness;
}

bool XWRaster::getInNonIsolatedGroup()
{
    return state->inNonIsolatedGroup;
}

int XWRaster::getLineCap()
{
    return state->lineCap;
}

double * XWRaster::getLineDash()
{
    return state->lineDash;
}

int XWRaster::getLineDashLength()
{
    return state->lineDashLength;
}

double XWRaster::getLineDashPhase()
{
    return state->lineDashPhase;
}

int XWRaster::getLineJoin()
{
    return state->lineJoin;
}

double XWRaster::getLineWidth()
{
    return state->lineWidth;
}

double * XWRaster::getMatrix()
{
    return state->matrix;
}

double XWRaster::getMiterLimit()
{
    return state->miterLimit;
}

XWScreen * XWRaster::getScreen()
{
    return state->screen;
}

XWBitmap * XWRaster::getSoftMask()
{
    return state->softMask;
}

bool XWRaster::getStrokeAdjust() 
{
  return state->strokeAdjust;
}

double XWRaster::getStrokeAlpha()
{
    return state->strokeAlpha;
}

XWRasterPattern * XWRaster::getStrokePattern()
{
    return state->strokePattern;
}

XWRasterPath * XWRaster::makeStrokePath(XWRasterPath *path, 
                                        double w,
				                                bool flatten)
{
  XWRasterPath *pathIn, *dashPath, *pathOut;
  double d, dx, dy, wdx, wdy, dxNext, dyNext, wdxNext, wdyNext;
  double crossprod, dotprod, miter, m;
  bool first, last, closed;
  int subpathStart0, subpathStart1, seg, i0, i1, j0, j1, k0, k1;
  int left0, left1, left2, right0, right1, right2, join0, join1, join2;
  int leftFirst, rightFirst, firstPt;

  pathOut = new XWRasterPath();

  if (path->length == 0) {
    return pathOut;
  }

  if (flatten) {
    pathIn = flattenPath(path, state->matrix, state->flatness);
    if (state->lineDashLength > 0) {
      dashPath = makeDashedPath(pathIn);
      delete pathIn;
      pathIn = dashPath;
      if (pathIn->length == 0) {
	delete pathIn;
	return pathOut;
      }
    }
  } else {
    pathIn = path;
  }

  subpathStart0 = subpathStart1 = 0; // make gcc happy
  seg = 0; // make gcc happy
  closed = false; // make gcc happy
  left0 = left1 = right0 = right1 = join0 = join1 = 0; // make gcc happy
  leftFirst = rightFirst = firstPt = 0; // make gcc happy

  i0 = 0;
  for (i1 = i0;
       !(pathIn->flags[i1] & PATH_LAST) &&
	 i1 + 1 < pathIn->length &&
	 pathIn->pts[i1+1].x == pathIn->pts[i1].x &&
	 pathIn->pts[i1+1].y == pathIn->pts[i1].y;
       ++i1) ;

  while (i1 < pathIn->length) {
    if ((first = pathIn->flags[i0] & PATH_FIRST)) {
      subpathStart0 = i0;
      subpathStart1 = i1;
      seg = 0;
      closed = pathIn->flags[i0] & PATH_CLOSED;
    }
    j0 = i1 + 1;
    if (j0 < pathIn->length) {
      for (j1 = j0;
	   !(pathIn->flags[j1] & PATH_LAST) &&
	     j1 + 1 < pathIn->length &&
	     pathIn->pts[j1+1].x == pathIn->pts[j1].x &&
	     pathIn->pts[j1+1].y == pathIn->pts[j1].y;
	   ++j1) ;
    } else {
      j1 = j0;
    }
    if (pathIn->flags[i1] & PATH_LAST) {
      if (first && state->lineCap == LINE_CAP_ROUND) {
	// special case: zero-length subpath with round line caps -->
	// draw a circle
	pathOut->moveTo(pathIn->pts[i0].x + (double)0.5 * w,
			pathIn->pts[i0].y);
	pathOut->curveTo(pathIn->pts[i0].x + (double)0.5 * w,
			 pathIn->pts[i0].y + bezierCircle2 * w,
			 pathIn->pts[i0].x + bezierCircle2 * w,
			 pathIn->pts[i0].y + (double)0.5 * w,
			 pathIn->pts[i0].x,
			 pathIn->pts[i0].y + (double)0.5 * w);
	pathOut->curveTo(pathIn->pts[i0].x - bezierCircle2 * w,
			 pathIn->pts[i0].y + (double)0.5 * w,
			 pathIn->pts[i0].x - (double)0.5 * w,
			 pathIn->pts[i0].y + bezierCircle2 * w,
			 pathIn->pts[i0].x - (double)0.5 * w,
			 pathIn->pts[i0].y);
	pathOut->curveTo(pathIn->pts[i0].x - (double)0.5 * w,
			 pathIn->pts[i0].y - bezierCircle2 * w,
			 pathIn->pts[i0].x - bezierCircle2 * w,
			 pathIn->pts[i0].y - (double)0.5 * w,
			 pathIn->pts[i0].x,
			 pathIn->pts[i0].y - (double)0.5 * w);
	pathOut->curveTo(pathIn->pts[i0].x + bezierCircle2 * w,
			 pathIn->pts[i0].y - (double)0.5 * w,
			 pathIn->pts[i0].x + (double)0.5 * w,
			 pathIn->pts[i0].y - bezierCircle2 * w,
			 pathIn->pts[i0].x + (double)0.5 * w,
			 pathIn->pts[i0].y);
	pathOut->close();
      }
      i0 = j0;
      i1 = j1;
      continue;
    }
    last = pathIn->flags[j1] & PATH_LAST;
    if (last) {
      k0 = subpathStart1 + 1;
    } else {
      k0 = j1 + 1;
    }
    for (k1 = k0;
	 !(pathIn->flags[k1] & PATH_LAST) &&
	   k1 + 1 < pathIn->length &&
	   pathIn->pts[k1+1].x == pathIn->pts[k1].x &&
	   pathIn->pts[k1+1].y == pathIn->pts[k1].y;
	 ++k1) ;

    // compute the deltas for segment (i1, j0)
    d = (double)1 / rasterDist(pathIn->pts[i1].x, pathIn->pts[i1].y,
				    pathIn->pts[j0].x, pathIn->pts[j0].y);
    dx = d * (pathIn->pts[j0].x - pathIn->pts[i1].x);
    dy = d * (pathIn->pts[j0].y - pathIn->pts[i1].y);
    wdx = (double)0.5 * w * dx;
    wdy = (double)0.5 * w * dy;

    // draw the start cap
    pathOut->moveTo(pathIn->pts[i0].x - wdy, pathIn->pts[i0].y + wdx);
    if (i0 == subpathStart0) {
      firstPt = pathOut->length - 1;
    }
    if (first && !closed) {
      switch (state->lineCap) {
      case LINE_CAP_BUTT:
	pathOut->lineTo(pathIn->pts[i0].x + wdy, pathIn->pts[i0].y - wdx);
	break;
      case LINE_CAP_ROUND:
	pathOut->curveTo(pathIn->pts[i0].x - wdy - bezierCircle * wdx,
			 pathIn->pts[i0].y + wdx - bezierCircle * wdy,
			 pathIn->pts[i0].x - wdx - bezierCircle * wdy,
			 pathIn->pts[i0].y - wdy + bezierCircle * wdx,
			 pathIn->pts[i0].x - wdx,
			 pathIn->pts[i0].y - wdy);
	pathOut->curveTo(pathIn->pts[i0].x - wdx + bezierCircle * wdy,
			 pathIn->pts[i0].y - wdy - bezierCircle * wdx,
			 pathIn->pts[i0].x + wdy - bezierCircle * wdx,
			 pathIn->pts[i0].y - wdx - bezierCircle * wdy,
			 pathIn->pts[i0].x + wdy,
			 pathIn->pts[i0].y - wdx);
	break;
      case LINE_CAP_PROJECTING:
	pathOut->lineTo(pathIn->pts[i0].x - wdx - wdy,
			pathIn->pts[i0].y + wdx - wdy);
	pathOut->lineTo(pathIn->pts[i0].x - wdx + wdy,
			pathIn->pts[i0].y - wdx - wdy);
	pathOut->lineTo(pathIn->pts[i0].x + wdy,
			pathIn->pts[i0].y - wdx);
	break;
      }
    } else {
      pathOut->lineTo(pathIn->pts[i0].x + wdy, pathIn->pts[i0].y - wdx);
    }

    // draw the left side of the segment rectangle
    left2 = pathOut->length - 1;
    pathOut->lineTo(pathIn->pts[j0].x + wdy, pathIn->pts[j0].y - wdx);

    // draw the end cap
    if (last && !closed) {
      switch (state->lineCap) {
      case LINE_CAP_BUTT:
	pathOut->lineTo(pathIn->pts[j0].x - wdy, pathIn->pts[j0].y + wdx);
	break;
      case LINE_CAP_ROUND:
	pathOut->curveTo(pathIn->pts[j0].x + wdy + bezierCircle * wdx,
			 pathIn->pts[j0].y - wdx + bezierCircle * wdy,
			 pathIn->pts[j0].x + wdx + bezierCircle * wdy,
			 pathIn->pts[j0].y + wdy - bezierCircle * wdx,
			 pathIn->pts[j0].x + wdx,
			 pathIn->pts[j0].y + wdy);
	pathOut->curveTo(pathIn->pts[j0].x + wdx - bezierCircle * wdy,
			 pathIn->pts[j0].y + wdy + bezierCircle * wdx,
			 pathIn->pts[j0].x - wdy + bezierCircle * wdx,
			 pathIn->pts[j0].y + wdx + bezierCircle * wdy,
			 pathIn->pts[j0].x - wdy,
			 pathIn->pts[j0].y + wdx);
	break;
      case LINE_CAP_PROJECTING:
	pathOut->lineTo(pathIn->pts[j0].x + wdy + wdx,
			pathIn->pts[j0].y - wdx + wdy);
	pathOut->lineTo(pathIn->pts[j0].x - wdy + wdx,
			pathIn->pts[j0].y + wdx + wdy);
	pathOut->lineTo(pathIn->pts[j0].x - wdy,
			pathIn->pts[j0].y + wdx);
	break;
      }
    } else {
      pathOut->lineTo(pathIn->pts[j0].x - wdy, pathIn->pts[j0].y + wdx);
    }

    // draw the right side of the segment rectangle
    // (NB: if stroke adjustment is enabled, the closepath operation MUST
    // add a segment because this segment is used for a hint)
    right2 = pathOut->length - 1;
    pathOut->close(state->strokeAdjust);

    // draw the join
    join2 = pathOut->length;
    if (!last || closed) {

      // compute the deltas for segment (j1, k0)
      d = (double)1 / rasterDist(pathIn->pts[j1].x, pathIn->pts[j1].y,
				      pathIn->pts[k0].x, pathIn->pts[k0].y);
      dxNext = d * (pathIn->pts[k0].x - pathIn->pts[j1].x);
      dyNext = d * (pathIn->pts[k0].y - pathIn->pts[j1].y);
      wdxNext = (double)0.5 * w * dxNext;
      wdyNext = (double)0.5 * w * dyNext;

      // compute the join parameters
      crossprod = dx * dyNext - dy * dxNext;
      dotprod = -(dx * dxNext + dy * dyNext);
      if (dotprod > 0.9999) {
	// avoid a divide-by-zero -- set miter to something arbitrary
	// such that sqrt(miter) will exceed miterLimit (and m is never
	// used in that situation)
	// (note: the comparison value (0.9999) has to be less than
	// 1-epsilon, where epsilon is the smallest value
	// representable in the fixed point format)
	miter = (state->miterLimit + 1) * (state->miterLimit + 1);
	m = 0;
      } else {
	miter = (double)2 / ((double)1 - dotprod);
	if (miter < 1) {
	  // this can happen because of floating point inaccuracies
	  miter = 1;
	}
	m = rasterSqrt(miter - 1);
      }

      // round join
      if (state->lineJoin == LINE_JOIN_ROUND) {
	pathOut->moveTo(pathIn->pts[j0].x + (double)0.5 * w,
			pathIn->pts[j0].y);
	pathOut->curveTo(pathIn->pts[j0].x + (double)0.5 * w,
			 pathIn->pts[j0].y + bezierCircle2 * w,
			 pathIn->pts[j0].x + bezierCircle2 * w,
			 pathIn->pts[j0].y + (double)0.5 * w,
			 pathIn->pts[j0].x,
			 pathIn->pts[j0].y + (double)0.5 * w);
	pathOut->curveTo(pathIn->pts[j0].x - bezierCircle2 * w,
			 pathIn->pts[j0].y + (double)0.5 * w,
			 pathIn->pts[j0].x - (double)0.5 * w,
			 pathIn->pts[j0].y + bezierCircle2 * w,
			 pathIn->pts[j0].x - (double)0.5 * w,
			 pathIn->pts[j0].y);
	pathOut->curveTo(pathIn->pts[j0].x - (double)0.5 * w,
			 pathIn->pts[j0].y - bezierCircle2 * w,
			 pathIn->pts[j0].x - bezierCircle2 * w,
			 pathIn->pts[j0].y - (double)0.5 * w,
			 pathIn->pts[j0].x,
			 pathIn->pts[j0].y - (double)0.5 * w);
	pathOut->curveTo(pathIn->pts[j0].x + bezierCircle2 * w,
			 pathIn->pts[j0].y - (double)0.5 * w,
			 pathIn->pts[j0].x + (double)0.5 * w,
			 pathIn->pts[j0].y - bezierCircle2 * w,
			 pathIn->pts[j0].x + (double)0.5 * w,
			 pathIn->pts[j0].y);

      } else {
	pathOut->moveTo(pathIn->pts[j0].x, pathIn->pts[j0].y);

	// angle < 180
	if (crossprod < 0) {
	  pathOut->lineTo(pathIn->pts[j0].x - wdyNext,
			  pathIn->pts[j0].y + wdxNext);
	  // miter join inside limit
	  if (state->lineJoin == LINE_JOIN_MITER &&
	      rasterSqrt(miter) <= state->miterLimit) {
	    pathOut->lineTo(pathIn->pts[j0].x - wdy + wdx * m,
			    pathIn->pts[j0].y + wdx + wdy * m);
	    pathOut->lineTo(pathIn->pts[j0].x - wdy,
			    pathIn->pts[j0].y + wdx);
	  // bevel join or miter join outside limit
	  } else {
	    pathOut->lineTo(pathIn->pts[j0].x - wdy,
			    pathIn->pts[j0].y + wdx);
	  }

	// angle >= 180
	} else {
	  pathOut->lineTo(pathIn->pts[j0].x + wdy,
			  pathIn->pts[j0].y - wdx);
	  // miter join inside limit
	  if (state->lineJoin == LINE_JOIN_MITER &&
	      rasterSqrt(miter) <= state->miterLimit) {
	    pathOut->lineTo(pathIn->pts[j0].x + wdy + wdx * m,
			    pathIn->pts[j0].y - wdx + wdy * m);
	    pathOut->lineTo(pathIn->pts[j0].x + wdyNext,
			    pathIn->pts[j0].y - wdxNext);
	  // bevel join or miter join outside limit
	  } else {
	    pathOut->lineTo(pathIn->pts[j0].x + wdyNext,
			    pathIn->pts[j0].y - wdxNext);
	  }
	}
      }

      pathOut->close();
    }

    // add stroke adjustment hints
    if (state->strokeAdjust) {
      if (seg == 0 && !closed) {
	if (state->lineCap == LINE_CAP_BUTT) {
	  pathOut->addStrokeAdjustHint(firstPt, left2 + 1,
				       firstPt, firstPt + 1);
	  if (last) {
	    pathOut->addStrokeAdjustHint(firstPt, left2 + 1,
					 left2 + 1, left2 + 2);
	  }
	} else if (state->lineCap == LINE_CAP_PROJECTING) {
	  if (last) {
	    pathOut->addStrokeAdjustHint(firstPt + 1, left2 + 2,
					 firstPt + 1, firstPt + 2);
	    pathOut->addStrokeAdjustHint(firstPt + 1, left2 + 2,
					 left2 + 2, left2 + 3);
	  } else {
	    pathOut->addStrokeAdjustHint(firstPt + 1, left2 + 1,
					 firstPt + 1, firstPt + 2);
	  }
	}
      }
      if (seg >= 1) {
	if (seg >= 2) {
	  pathOut->addStrokeAdjustHint(left1, right1, left0 + 1, right0);
	  pathOut->addStrokeAdjustHint(left1, right1, join0, left2);
	} else {
	  pathOut->addStrokeAdjustHint(left1, right1, firstPt, left2);
	}
	pathOut->addStrokeAdjustHint(left1, right1, right2 + 1, right2 + 1);
      }
      left0 = left1;
      left1 = left2;
      right0 = right1;
      right1 = right2;
      join0 = join1;
      join1 = join2;
      if (seg == 0) {
	leftFirst = left2;
	rightFirst = right2;
      }
      if (last) {
	if (seg >= 2) {
	  pathOut->addStrokeAdjustHint(left1, right1, left0 + 1, right0);
	  pathOut->addStrokeAdjustHint(left1, right1,
				       join0, pathOut->length - 1);
	} else {
	  pathOut->addStrokeAdjustHint(left1, right1,
				       firstPt, pathOut->length - 1);
	}
	if (closed) {
	  pathOut->addStrokeAdjustHint(left1, right1, firstPt, leftFirst);
	  pathOut->addStrokeAdjustHint(left1, right1,
				       rightFirst + 1, rightFirst + 1);
	  pathOut->addStrokeAdjustHint(leftFirst, rightFirst,
				       left1 + 1, right1);
	  pathOut->addStrokeAdjustHint(leftFirst, rightFirst,
				       join1, pathOut->length - 1);
	}
	if (!closed && seg > 0) {
	  if (state->lineCap == LINE_CAP_BUTT) {
	    pathOut->addStrokeAdjustHint(left1 - 1, left1 + 1,
					 left1 + 1, left1 + 2);
	  } else if (state->lineCap == LINE_CAP_PROJECTING) {
	    pathOut->addStrokeAdjustHint(left1 - 1, left1 + 2,
					 left1 + 2, left1 + 3);
	  }
	}
      }
    }

    i0 = j0;
    i1 = j1;
    ++seg;
  }

  if (pathIn != path) {
    delete pathIn;
  }

  return pathOut;
}

void XWRaster::restoreState()
{
    if (!state->next) 
        return ;
  
    XWRasterState * oldState = state;
    state = state->next;
    delete oldState;
}

void XWRaster::saveState()
{
    XWRasterState * newState = state->copy();
    newState->next = state;
    state = newState;
}

void XWRaster::setBlendFunc(XWRasterBlendFunc func)
{
    state->blendFunc = func;
}

void XWRaster::setFillAlpha(double alpha)
{
    state->fillAlpha = alpha;
}

void XWRaster::setFillPattern(XWRasterPattern *fillPattern)
{
    state->setFillPattern(fillPattern);
}

void XWRaster::setFlatness(double flatness)
{
    if (flatness < 1) 
        state->flatness = 1;
    else
        state->flatness = flatness;
}

void XWRaster::setInNonIsolatedGroup(XWBitmap *alpha0BitmapA, int alpha0XA, int alpha0YA)
{
    alpha0Bitmap = alpha0BitmapA;
    alpha0X = alpha0XA;
    alpha0Y = alpha0YA;
    state->inNonIsolatedGroup = true;
}

void XWRaster::setLineCap(int lineCap)
{
    state->lineCap = lineCap;
}

void XWRaster::setLineDash(double *lineDash, int lineDashLength, double lineDashPhase)
{
    state->setLineDash(lineDash, lineDashLength, lineDashPhase);
}

void XWRaster::setLineJoin(int lineJoin)
{
    state->lineJoin = lineJoin;
}

void XWRaster::setLineWidth(double lineWidth)
{
    state->lineWidth = lineWidth;
}

void XWRaster::setMatrix(double * m)
{
    memcpy(state->matrix, m, 6 * sizeof(double));
}

void XWRaster::setMiterLimit(double miterLimit)
{
    state->miterLimit = miterLimit;
}

void XWRaster::setOverprintMask(uint overprintMask)
{
	state->overprintMask = overprintMask;
}

void XWRaster::setScreen(XWScreen *screen)
{
    state->setScreen(screen);
}

void XWRaster::setSoftMask(XWBitmap *softMask)
{
    state->setSoftMask(softMask);
}

void XWRaster::setStrokeAdjust(bool strokeAdjust)
{
    state->strokeAdjust = strokeAdjust;
}

void XWRaster::setStrokeAlpha(double alpha)
{
    state->strokeAlpha = alpha;
}

void XWRaster::setStrokePattern(XWRasterPattern *strokePattern)
{
    state->setStrokePattern(strokePattern);
}

void XWRaster::setTransfer(uchar *red, uchar *green, uchar *blue, uchar *gray)
{
	state->setTransfer(red, green, blue, gray);
}

bool XWRaster::stroke(XWRasterPath *path)
{
  XWRasterPath *path2, *dPath;
  double d1, d2, t1, t2, w;

  opClipRes = CLIP_ALLOUTSIDE;
  if (path->length == 0) {
    return false;
  }

  path2 = flattenPath(path, state->matrix, state->flatness);
  if (state->lineDashLength > 0) {
    dPath = makeDashedPath(path2);
    delete path2;
    path2 = dPath;
    if (path2->length == 0) {
      delete path2;
      return false;
    }
  }
  
  // transform a unit square, and take the half the max of the two
  // diagonals; the product of this number and the line width is the
  // (approximate) transformed line width
  t1 = state->matrix[0] + state->matrix[2];
  t2 = state->matrix[1] + state->matrix[3];
  d1 = t1 * t1 + t2 * t2;
  t1 = state->matrix[0] - state->matrix[2];
  t2 = state->matrix[1] - state->matrix[3];
  d2 = t1 * t1 + t2 * t2;
  if (d2 > d1) {
    d1 = d2;
  }
  d1 *= 0.5;
  if (d1 > 0 &&
      d1 * state->lineWidth * state->lineWidth < minLineWidth * minLineWidth) {
    w = minLineWidth / rasterSqrt(d1);
    strokeWide(path2, w);
  } else if (bitmap->mode == COLOR_MODE_MONO1) {
    // this gets close to Adobe's behavior in mono mode
    if (d1 <= 2) {
      strokeNarrow(path2);
    } else {
      strokeWide(path2, state->lineWidth);
    }
  } else {
    if (state->lineWidth == 0) {
      strokeNarrow(path2);
    } else {
      strokeWide(path2, state->lineWidth);
    }
  }

  delete path2;
  return true;
}

bool XWRaster::xorFill(XWRasterPath *path, bool eo)
{
  RasterPipe pipe;
  XWRasterXPath *xPath;
  XWRasterXPathScanner *scanner;
  int xMinI, yMinI, xMaxI, yMaxI, x0, x1, y;
  int clipRes, clipRes2;
  XWRasterBlendFunc origBlendFunc;

  if (path->length == 0) {
    return false;
  }
  xPath = new XWRasterXPath(path, state->matrix, state->flatness, true);
  xPath->sort();
  scanner = new XWRasterXPathScanner(xPath, eo, state->clip->getYMinI(), state->clip->getYMaxI());

  // get the min and max x and y values
  scanner->getBBox(&xMinI, &yMinI, &xMaxI, &yMaxI);

  // check clipping
  if ((clipRes = state->clip->testRect(xMinI, yMinI, xMaxI, yMaxI)) != CLIP_ALLOUTSIDE) {
    if (scanner->hasPartialClip()) {
      clipRes = CLIP_PARTIAL;
    }

    origBlendFunc = state->blendFunc;
    state->blendFunc = &blendXor;
    pipeInit(&pipe, 0, yMinI, state->fillPattern, NULL, 255, false, false);

    // draw the spans
    for (y = yMinI; y <= yMaxI; ++y) {
      while (scanner->getNextSpan(y, &x0, &x1)) {
	if (clipRes == CLIP_ALLINSIDE) {
	  drawSpan(&pipe, x0, x1, y, true);
	} else {
	  // limit the x range
	  if (x0 < state->clip->getXMinI()) {
	    x0 = state->clip->getXMinI();
	  }
	  if (x1 > state->clip->getXMaxI()) {
	    x1 = state->clip->getXMaxI();
	  }
	  clipRes2 = state->clip->testSpan(x0, x1, y);
	  drawSpan(&pipe, x0, x1, y, clipRes2 == CLIP_ALLINSIDE);
	}
      }
    }
    state->blendFunc = origBlendFunc;
  }
  opClipRes = clipRes;

  delete scanner;
  delete xPath;
  return true;
}

void XWRaster::arbitraryTransformMask(XWRasterImageMaskSource src, void *srcData,
			      									        int srcWidth, int srcHeight,
			      									        double *mat, bool glyphMode)
{
	XWBitmap *scaledMask;
  int clipRes, clipRes2;
  RasterPipe pipe;
  int scaledWidth, scaledHeight, t0, t1;
  double r00, r01, r10, r11, det, ir00, ir01, ir10, ir11;
  double vx[4], vy[4];
  int xMin, yMin, xMax, yMax;
  ImageSection section[3];
  int nSections;
  int y, xa, xb, x, i, xx, yy;
  
  vx[0] = mat[4];                    vy[0] = mat[5];
  vx[1] = mat[2] + mat[4];           vy[1] = mat[3] + mat[5];
  vx[2] = mat[0] + mat[2] + mat[4];  vy[2] = mat[1] + mat[3] + mat[5];
  vx[3] = mat[0] + mat[4];           vy[3] = mat[1] + mat[5];
  
  xMin = imgCoordMungeLowerC(vx[0], glyphMode);
  xMax = imgCoordMungeUpperC(vx[0], glyphMode);
  yMin = imgCoordMungeLowerC(vy[0], glyphMode);
  yMax = imgCoordMungeUpperC(vy[0], glyphMode);
  
  for (i = 1; i < 4; ++i) 
 	{
    t0 = imgCoordMungeLowerC(vx[i], glyphMode);
    if (t0 < xMin) 
      xMin = t0;
    t0 = imgCoordMungeUpperC(vx[i], glyphMode);
    if (t0 > xMax) 
      xMax = t0;
    t1 = imgCoordMungeLowerC(vy[i], glyphMode);
    if (t1 < yMin) 
      yMin = t1;
    t1 = imgCoordMungeUpperC(vy[i], glyphMode);
    if (t1 > yMax) 
      yMax = t1;
  }
  clipRes = state->clip->testRect(xMin, yMin, xMax - 1, yMax - 1);
  opClipRes = clipRes;
  if (clipRes == CLIP_ALLOUTSIDE) 
    return;
    
  if (mat[0] >= 0) 
    t0 = imgCoordMungeUpperC(mat[0] + mat[4], glyphMode) - imgCoordMungeLowerC(mat[4], glyphMode);
  else 
    t0 = imgCoordMungeUpperC(mat[4], glyphMode) - imgCoordMungeLowerC(mat[0] + mat[4], glyphMode);
    
  if (mat[1] >= 0) 
    t1 = imgCoordMungeUpperC(mat[1] + mat[5], glyphMode) - imgCoordMungeLowerC(mat[5], glyphMode);
  else 
    t1 = imgCoordMungeUpperC(mat[5], glyphMode) - imgCoordMungeLowerC(mat[1] + mat[5], glyphMode);
    
  scaledWidth = t0 > t1 ? t0 : t1;
  if (mat[2] >= 0) 
    t0 = imgCoordMungeUpperC(mat[2] + mat[4], glyphMode) - imgCoordMungeLowerC(mat[4], glyphMode);
  else 
    t0 = imgCoordMungeUpperC(mat[4], glyphMode) - imgCoordMungeLowerC(mat[2] + mat[4], glyphMode);
  if (mat[3] >= 0) 
    t1 = imgCoordMungeUpperC(mat[3] + mat[5], glyphMode) - imgCoordMungeLowerC(mat[5], glyphMode);
  else 
    t1 = imgCoordMungeUpperC(mat[5], glyphMode) - imgCoordMungeLowerC(mat[3] + mat[5], glyphMode);
  scaledHeight = t0 > t1 ? t0 : t1;
  if (scaledWidth == 0) 
    scaledWidth = 1;
  if (scaledHeight == 0) 
    scaledHeight = 1;
    
  r00 = mat[0] / scaledWidth;
  r01 = mat[1] / scaledWidth;
  r10 = mat[2] / scaledHeight;
  r11 = mat[3] / scaledHeight;
  det = r00 * r11 - r01 * r10;
  if (rasterAbs(det) < 1e-6)
    return;
    
  ir00 = r11 / det;
  ir01 = -r01 / det;
  ir10 = -r10 / det;
  ir11 = r00 / det;
  
  scaledMask = scaleMask(src, srcData, srcWidth, srcHeight, scaledWidth, scaledHeight);
  
  i = (vy[2] <= vy[3]) ? 2 : 3;
  if (vy[1] <= vy[i]) 
    i = 1;
  if (vy[0] < vy[i] || (i != 3 && vy[0] == vy[i])) 
    i = 0;
  if (vy[i] == vy[(i+1) & 3]) 
  {
    section[0].y0 = imgCoordMungeLowerC(vy[i], glyphMode);
    section[0].y1 = imgCoordMungeUpperC(vy[(i+2) & 3], glyphMode) - 1;
    if (vx[i] < vx[(i+1) & 3]) 
    {
      section[0].ia0 = i;
      section[0].ia1 = (i+3) & 3;
      section[0].ib0 = (i+1) & 3;
      section[0].ib1 = (i+2) & 3;
    } 
    else 
    {
      section[0].ia0 = (i+1) & 3;
      section[0].ia1 = (i+2) & 3;
      section[0].ib0 = i;
      section[0].ib1 = (i+3) & 3;
    }
    nSections = 1;
  } 
  else 
  {
    section[0].y0 = imgCoordMungeLowerC(vy[i], glyphMode);
    section[2].y1 = imgCoordMungeUpperC(vy[(i+2) & 3], glyphMode) - 1;
    section[0].ia0 = section[0].ib0 = i;
    section[2].ia1 = section[2].ib1 = (i+2) & 3;
    if (vx[(i+1) & 3] < vx[(i+3) & 3]) 
    {
      section[0].ia1 = section[2].ia0 = (i+1) & 3;
      section[0].ib1 = section[2].ib0 = (i+3) & 3;
    } 
    else 
    {
      section[0].ia1 = section[2].ia0 = (i+3) & 3;
      section[0].ib1 = section[2].ib0 = (i+1) & 3;
    }
    if (vy[(i+1) & 3] < vy[(i+3) & 3]) 
    {
      section[1].y0 = imgCoordMungeLowerC(vy[(i+1) & 3], glyphMode);
      section[2].y0 = imgCoordMungeUpperC(vy[(i+3) & 3], glyphMode);
      if (vx[(i+1) & 3] < vx[(i+3) & 3]) 
      {
				section[1].ia0 = (i+1) & 3;
				section[1].ia1 = (i+2) & 3;
				section[1].ib0 = i;
				section[1].ib1 = (i+3) & 3;
      } 
      else 
      {
				section[1].ia0 = i;
				section[1].ia1 = (i+3) & 3;
				section[1].ib0 = (i+1) & 3;
				section[1].ib1 = (i+2) & 3;
      }
    } 
    else 
    {
      section[1].y0 = imgCoordMungeLowerC(vy[(i+3) & 3], glyphMode);
      section[2].y0 = imgCoordMungeUpperC(vy[(i+1) & 3], glyphMode);
      if (vx[(i+1) & 3] < vx[(i+3) & 3]) 
      {
				section[1].ia0 = i;
				section[1].ia1 = (i+1) & 3;
				section[1].ib0 = (i+3) & 3;
				section[1].ib1 = (i+2) & 3;
      } 
      else 
      {
				section[1].ia0 = (i+3) & 3;
				section[1].ia1 = (i+2) & 3;
				section[1].ib0 = i;
				section[1].ib1 = (i+1) & 3;
      }
    }
    section[0].y1 = section[1].y0 - 1;
    section[1].y1 = section[2].y0 - 1;
    nSections = 3;
  }
  for (i = 0; i < nSections; ++i) 
  {
    section[i].xa0 = vx[section[i].ia0];
    section[i].ya0 = vy[section[i].ia0];
    section[i].xa1 = vx[section[i].ia1];
    section[i].ya1 = vy[section[i].ia1];
    section[i].xb0 = vx[section[i].ib0];
    section[i].yb0 = vy[section[i].ib0];
    section[i].xb1 = vx[section[i].ib1];
    section[i].yb1 = vy[section[i].ib1];
    section[i].dxdya = (section[i].xa1 - section[i].xa0) /
                       (section[i].ya1 - section[i].ya0);
    section[i].dxdyb = (section[i].xb1 - section[i].xb0) /
                       (section[i].yb1 - section[i].yb0);
  }
  
   pipeInit(&pipe, 0, 0, state->fillPattern, NULL, (uchar)rasterRound(state->fillAlpha * 255), true, false);
  if (vectorAntialias) 
    drawAAPixelInit();
    
   if (nSections == 1) 
   {
    	if (section[0].y0 == section[0].y1) 
    	{
      	++section[0].y1;
      	clipRes = opClipRes = CLIP_PARTIAL;
    	}
  	} 
  	else 
  	{
    	if (section[0].y0 == section[2].y1) 
    	{
      	++section[1].y1;
      	clipRes = opClipRes = CLIP_PARTIAL;
    	}
  	}
  	
  for (i = 0; i < nSections; ++i) 
  {
    for (y = section[i].y0; y <= section[i].y1; ++y) 
    {
      xa = imgCoordMungeLowerC(section[i].xa0 + ((double)y + 0.5 - section[i].ya0) * section[i].dxdya, glyphMode);
      xb = imgCoordMungeUpperC(section[i].xb0 + ((double)y + 0.5 - section[i].yb0) * section[i].dxdyb, glyphMode);
      // make sure narrow images cover at least one pixel
      if (xa == xb) 
				++xb;
      
      if (clipRes != CLIP_ALLINSIDE) 
				clipRes2 = state->clip->testSpan(xa, xb - 1, y);
      else 
				clipRes2 = clipRes;
      for (x = xa; x < xb; ++x) 
      {
				// map (x+0.5, y+0.5) back to the scaled image
				xx = rasterFloor(((double)x + 0.5 - mat[4]) * ir00 + ((double)y + 0.5 - mat[5]) * ir10);
				yy = rasterFloor(((double)x + 0.5 - mat[4]) * ir01 + ((double)y + 0.5 - mat[5]) * ir11);
				// xx should always be within bounds, but floating point
				// inaccuracy can cause problems
				if (xx < 0) 
	  			xx = 0;
				else if (xx >= scaledWidth) 
	  			xx = scaledWidth - 1;
				if (yy < 0) 
	  			yy = 0;
				else if (yy >= scaledHeight) 
	  			yy = scaledHeight - 1;
				pipe.shape = scaledMask->data[yy * scaledWidth + xx];
				if (vectorAntialias && clipRes2 != CLIP_ALLINSIDE) 
	  			drawAAPixel(&pipe, x, y);
				else 
	  			drawPixel(&pipe, x, y, clipRes2 == CLIP_ALLINSIDE);
      }
    }
  }

  delete scaledMask;
}

void XWRaster::arbitraryTransformImage(XWRasterImageSource src, void *srcData,
			       										       int srcMode, int nComps,
			       										       bool srcAlpha,
			       										       int srcWidth, int srcHeight,
			       										       double *mat)
{
	XWBitmap *scaledImg;
  int clipRes, clipRes2;
  RasterPipe pipe;
  uchar pixel[4];
  int scaledWidth, scaledHeight, t0, t1;
  double r00, r01, r10, r11, det, ir00, ir01, ir10, ir11;
  double vx[4], vy[4];
  int xMin, yMin, xMax, yMax;
  ImageSection section[3];
  int nSections;
  int y, xa, xb, x, i, xx, yy;
  
  vx[0] = mat[4];                    vy[0] = mat[5];
  vx[1] = mat[2] + mat[4];           vy[1] = mat[3] + mat[5];
  vx[2] = mat[0] + mat[2] + mat[4];  vy[2] = mat[1] + mat[3] + mat[5];
  vx[3] = mat[0] + mat[4];           vy[3] = mat[1] + mat[5];
  xMin = imgCoordMungeLower(vx[0]);
  xMax = imgCoordMungeUpper(vx[0]);
  yMin = imgCoordMungeLower(vy[0]);
  yMax = imgCoordMungeUpper(vy[0]);
  for (i = 1; i < 4; ++i)
  {
  	t0 = imgCoordMungeLower(vx[i]);
    if (t0 < xMin) 
      xMin = t0;
    t0 = imgCoordMungeUpper(vx[i]);
    if (t0 > xMax) 
      xMax = t0;
    t1 = imgCoordMungeLower(vy[i]);
    if (t1 < yMin) 
      yMin = t1;
    t1 = imgCoordMungeUpper(vy[i]);
    if (t1 > yMax) 
      yMax = t1;
  }
  
  clipRes = state->clip->testRect(xMin, yMin, xMax - 1, yMax - 1);
  opClipRes = clipRes;
  if (clipRes == CLIP_ALLOUTSIDE) 
    return;
    
  if (mat[0] >= 0) 
    t0 = imgCoordMungeUpper(mat[0] + mat[4]) - imgCoordMungeLower(mat[4]);
 	else 
    t0 = imgCoordMungeUpper(mat[4]) - imgCoordMungeLower(mat[0] + mat[4]);
  if (mat[1] >= 0) 
    t1 = imgCoordMungeUpper(mat[1] + mat[5]) - imgCoordMungeLower(mat[5]);
  else 
    t1 = imgCoordMungeUpper(mat[5]) - imgCoordMungeLower(mat[1] + mat[5]);
  scaledWidth = t0 > t1 ? t0 : t1;
  if (mat[2] >= 0) 
    t0 = imgCoordMungeUpper(mat[2] + mat[4]) - imgCoordMungeLower(mat[4]);
  else 
    t0 = imgCoordMungeUpper(mat[4]) - imgCoordMungeLower(mat[2] + mat[4]);
  if (mat[3] >= 0) 
    t1 = imgCoordMungeUpper(mat[3] + mat[5]) - imgCoordMungeLower(mat[5]);
  else 
    t1 = imgCoordMungeUpper(mat[5]) - imgCoordMungeLower(mat[3] + mat[5]);
  scaledHeight = t0 > t1 ? t0 : t1;
  if (scaledWidth == 0) 
    scaledWidth = 1;
  if (scaledHeight == 0) 
    scaledHeight = 1;
    
  r00 = mat[0] / scaledWidth;
  r01 = mat[1] / scaledWidth;
  r10 = mat[2] / scaledHeight;
  r11 = mat[3] / scaledHeight;
  det = r00 * r11 - r01 * r10;
  if (rasterAbs(det) < 1e-6) 
    return;
    
  ir00 = r11 / det;
  ir01 = -r01 / det;
  ir10 = -r10 / det;
  ir11 = r00 / det;
  
  scaledImg = scaleImage(src, srcData, srcMode, nComps, srcAlpha, srcWidth, srcHeight, scaledWidth, scaledHeight);
  i = 0;
  if (vy[1] < vy[i]) 
    i = 1;
  if (vy[2] < vy[i]) 
    i = 2;
  if (vy[3] < vy[i]) 
    i = 3;
    
  if (rasterAbs(vy[i] - vy[(i-1) & 3]) <= 0.000001 && vy[(i-1) & 3] < vy[(i+1) & 3]) 
    i = (i-1) & 3;
    
  if (rasterAbs(vy[i] - vy[(i+1) & 3]) <= 0.000001) 
  {
    section[0].y0 = imgCoordMungeLower(vy[i]);
    section[0].y1 = imgCoordMungeUpper(vy[(i+2) & 3]) - 1;
    if (vx[i] < vx[(i+1) & 3]) 
    {
      section[0].ia0 = i;
      section[0].ia1 = (i+3) & 3;
      section[0].ib0 = (i+1) & 3;
      section[0].ib1 = (i+2) & 3;
    } 
    else 
    {
      section[0].ia0 = (i+1) & 3;
      section[0].ia1 = (i+2) & 3;
      section[0].ib0 = i;
      section[0].ib1 = (i+3) & 3;
    }
    nSections = 1;
  } 
  else 
  {
    section[0].y0 = imgCoordMungeLower(vy[i]);
    section[2].y1 = imgCoordMungeUpper(vy[(i+2) & 3]) - 1;
    section[0].ia0 = section[0].ib0 = i;
    section[2].ia1 = section[2].ib1 = (i+2) & 3;
    if (vx[(i+1) & 3] < vx[(i+3) & 3]) 
    {
      section[0].ia1 = section[2].ia0 = (i+1) & 3;
      section[0].ib1 = section[2].ib0 = (i+3) & 3;
    } 
    else 
    {
      section[0].ia1 = section[2].ia0 = (i+3) & 3;
      section[0].ib1 = section[2].ib0 = (i+1) & 3;
    }
    if (vy[(i+1) & 3] < vy[(i+3) & 3]) 
    {
      section[1].y0 = imgCoordMungeLower(vy[(i+1) & 3]);
      section[2].y0 = imgCoordMungeUpper(vy[(i+3) & 3]);
      if (vx[(i+1) & 3] < vx[(i+3) & 3]) 
      {
				section[1].ia0 = (i+1) & 3;
				section[1].ia1 = (i+2) & 3;
				section[1].ib0 = i;
				section[1].ib1 = (i+3) & 3;
      } 
      else 
      {
				section[1].ia0 = i;
				section[1].ia1 = (i+3) & 3;
				section[1].ib0 = (i+1) & 3;
				section[1].ib1 = (i+2) & 3;
      }
    } 
    else 
    {
      section[1].y0 = imgCoordMungeLower(vy[(i+3) & 3]);
      section[2].y0 = imgCoordMungeUpper(vy[(i+1) & 3]);
      if (vx[(i+1) & 3] < vx[(i+3) & 3]) 
      {
				section[1].ia0 = i;
				section[1].ia1 = (i+1) & 3;
				section[1].ib0 = (i+3) & 3;
				section[1].ib1 = (i+2) & 3;
      } 
      else 
      {
				section[1].ia0 = (i+3) & 3;
				section[1].ia1 = (i+2) & 3;
				section[1].ib0 = i;
				section[1].ib1 = (i+1) & 3;
      }
    }
    section[0].y1 = section[1].y0 - 1;
    section[1].y1 = section[2].y0 - 1;
    nSections = 3;
  }
  for (i = 0; i < nSections; ++i) 
  {
    section[i].xa0 = vx[section[i].ia0];
    section[i].ya0 = vy[section[i].ia0];
    section[i].xa1 = vx[section[i].ia1];
    section[i].ya1 = vy[section[i].ia1];
    section[i].xb0 = vx[section[i].ib0];
    section[i].yb0 = vy[section[i].ib0];
    section[i].xb1 = vx[section[i].ib1];
    section[i].yb1 = vy[section[i].ib1];
    section[i].dxdya = (section[i].xa1 - section[i].xa0) /
                       (section[i].ya1 - section[i].ya0);
    section[i].dxdyb = (section[i].xb1 - section[i].xb0) /
                       (section[i].yb1 - section[i].yb0);
  }
  
  pipeInit(&pipe, 0, 0, NULL, pixel, (uchar)rasterRound(state->fillAlpha * 255),
	  			 srcAlpha || (vectorAntialias && clipRes != CLIP_ALLINSIDE), false);
  if (vectorAntialias) 
    drawAAPixelInit();
    
  if (nSections == 1) 
  {
    if (section[0].y0 == section[0].y1) 
    {
      ++section[0].y1;
      clipRes = opClipRes = CLIP_PARTIAL;
    }
  } 
  else 
  {
    if (section[0].y0 == section[2].y1) 
    {
      ++section[1].y1;
      clipRes = opClipRes = CLIP_PARTIAL;
    }
  }
  
  for (i = 0; i < nSections; ++i) 
  {
    for (y = section[i].y0; y <= section[i].y1; ++y) 
    {
      xa = imgCoordMungeLower(section[i].xa0 + ((double)y + 0.5 - section[i].ya0) * section[i].dxdya);
      xb = imgCoordMungeUpper(section[i].xb0 + ((double)y + 0.5 - section[i].yb0) * section[i].dxdyb);
      // make sure narrow images cover at least one pixel
      if (xa == xb) 
				++xb;
				
      if (clipRes != CLIP_ALLINSIDE) 
				clipRes2 = state->clip->testSpan(xa, xb - 1, y);
      else 
				clipRes2 = clipRes;
      
      for (x = xa; x < xb; ++x) 
      {
				// map (x+0.5, y+0.5) back to the scaled image
				xx = rasterFloor(((double)x + 0.5 - mat[4]) * ir00 + ((double)y + 0.5 - mat[5]) * ir10);
				yy = rasterFloor(((double)x + 0.5 - mat[4]) * ir01 + ((double)y + 0.5 - mat[5]) * ir11);
				// xx should always be within bounds, but floating point
				// inaccuracy can cause problems
				if (xx < 0) 
	  			xx = 0;
				else if (xx >= scaledWidth) 
	  			xx = scaledWidth - 1;
				if (yy < 0) 
	  			yy = 0;
				else if (yy >= scaledHeight) 
	  			yy = scaledHeight - 1;
				scaledImg->getPixel(xx, yy, pixel);
				if (srcAlpha) 
	  			pipe.shape = scaledImg->alpha[yy * scaledWidth + xx];
				else 
	  			pipe.shape = 255;
	  			
				if (vectorAntialias && clipRes2 != CLIP_ALLINSIDE) 
	  			drawAAPixel(&pipe, x, y);
				else 
	  			drawPixel(&pipe, x, y, clipRes2 == CLIP_ALLINSIDE);
      }
    }
  }

  delete scaledImg;
}

void XWRaster::blitImage(XWBitmap *src, bool srcAlpha, int xDest, int yDest, int clipRes)
{
	RasterPipe pipe;
  uchar pixel[4];
  uchar *ap;
  int w, h, x0, y0, x1, y1, x, y;
  
  w = src->getWidth();
  h = src->getHeight();
  if (clipRes == CLIP_ALLINSIDE) 
  {
    x0 = 0;
    y0 = 0;
    x1 = w;
    y1 = h;
  } 
  else 
  {
    if (state->clip->getNumPaths()) 
    {
      x0 = x1 = w;
      y0 = y1 = h;
    } 
    else 
    {
      if ((x0 = rasterCeil(state->clip->getXMin()) - xDest) < 0) 
				x0 = 0;
      if ((y0 = rasterCeil(state->clip->getYMin()) - yDest) < 0) 
				y0 = 0;
      if ((x1 = rasterFloor(state->clip->getXMax()) - xDest) > w) 
				x1 = w;
      if (x1 < x0) 
				x1 = x0;
      if ((y1 = rasterFloor(state->clip->getYMax()) - yDest) > h) 
				y1 = h;
      if (y1 < y0) 
				y1 = y0;
    }
  }
  
  if (x0 < w && y0 < h && x0 < x1 && y0 < y1) 
  {
    pipeInit(&pipe, xDest + x0, yDest + y0, NULL, pixel,
	     				(uchar)rasterRound(state->fillAlpha * 255), srcAlpha, false);
    if (srcAlpha) 
    {
      for (y = y0; y < y1; ++y) 
     {
				pipeSetXY(&pipe, xDest + x0, yDest + y);
				ap = src->getAlphaPtr() + y * w + x0;
				for (x = x0; x < x1; ++x) 
				{
	  			src->getPixel(x, y, pixel);
	  			pipe.shape = *ap++;
	  			(this->*pipe.run)(&pipe);
				}
      }
    } 
    else 
    {
      for (y = y0; y < y1; ++y) 
      {
				pipeSetXY(&pipe, xDest + x0, yDest + y);
				for (x = x0; x < x1; ++x) 
				{
	  			src->getPixel(x, y, pixel);
	  			(this->*pipe.run)(&pipe);
				}
      }
    }
    updateModX(xDest + x0);
    updateModX(xDest + x1 - 1);
    updateModY(yDest + y0);
    updateModY(yDest + y1 - 1);
  }
  
  if (y0 > 0) 
    blitImageClipped(src, srcAlpha, 0, 0, xDest, yDest, w, y0);
  if (y1 < h) 
    blitImageClipped(src, srcAlpha, 0, y1, xDest, yDest + y1, w, h - y1);
  if (x0 > 0 && y0 < y1) 
    blitImageClipped(src, srcAlpha, 0, y0, xDest, yDest + y0, x0, y1 - y0);
  if (x1 < w && y0 < y1) 
    blitImageClipped(src, srcAlpha, x1, y0, xDest + x1, yDest + y0, w - x1, y1 - y0);
}

void XWRaster::blitImageClipped(XWBitmap *src, bool srcAlpha,
																int xSrc, int ySrc, int xDest, int yDest,
																int w, int h)
{
	RasterPipe pipe;
  uchar pixel[4];
  uchar *ap;
  int x, y;
  
  if (vectorAntialias) 
  {
    pipeInit(&pipe, xDest, yDest, NULL, pixel,
	     			(uchar)rasterRound(state->fillAlpha * 255), true, false);
    drawAAPixelInit();
    if (srcAlpha) 
    {
      for (y = 0; y < h; ++y) 
     	{
				ap = src->getAlphaPtr() + (ySrc + y) * src->getWidth() + xSrc;
				for (x = 0; x < w; ++x) 
				{
	  			src->getPixel(xSrc + x, ySrc + y, pixel);
	  			pipe.shape = *ap++;
	  			drawAAPixel(&pipe, xDest + x, yDest + y);
				}
      }
    } 
    else 
    {
      for (y = 0; y < h; ++y) 
      {
				for (x = 0; x < w; ++x) 
				{
	  			src->getPixel(xSrc + x, ySrc + y, pixel);
	  			pipe.shape = 255;
	  			drawAAPixel(&pipe, xDest + x, yDest + y);
				}
      }
    }
  } 
  else 
  {
    pipeInit(&pipe, xDest, yDest, NULL, pixel,
	     			(uchar)rasterRound(state->fillAlpha * 255), srcAlpha, false);
    if (srcAlpha) 
    {
      for (y = 0; y < h; ++y) 
      {
				ap = src->getAlphaPtr() + (ySrc + y) * src->getWidth() + xSrc;
				pipeSetXY(&pipe, xDest, yDest + y);
				for (x = 0; x < w; ++x) 
				{
	  			if (state->clip->test(xDest + x, yDest + y)) 
	  			{
	    			src->getPixel(xSrc + x, ySrc + y, pixel);
	    			pipe.shape = *ap++;
	    			(this->*pipe.run)(&pipe);
	    			updateModX(xDest + x);
	    			updateModY(yDest + y);
	  			} 
	  			else 
	  			{
	    			pipeIncX(&pipe);
	    			++ap;
	  			}
				}
      }
    } 
    else 
    {
      for (y = 0; y < h; ++y) 
     {
				pipeSetXY(&pipe, xDest, yDest + y);
				for (x = 0; x < w; ++x) 
				{
	  			if (state->clip->test(xDest + x, yDest + y)) 
	  			{
	    			src->getPixel(xSrc + x, ySrc + y, pixel);
	    			(this->*pipe.run)(&pipe);
	    			updateModX(xDest + x);
	    			updateModY(yDest + y);
	  			} 
	  			else 
	  			{
	    			pipeIncX(&pipe);
	  			}
				}
      }
    }
  }
}

void XWRaster::blitMask(XWBitmap *src, int xDest, int yDest, int clipRes)
{
	RasterPipe pipe;
  uchar *p;
  int w, h, x, y;
  
  w = src->getWidth();
  h = src->getHeight();
  if (vectorAntialias && clipRes != CLIP_ALLINSIDE)
  {
  	pipeInit(&pipe, xDest, yDest, state->fillPattern, NULL,
	     			(uchar)rasterRound(state->fillAlpha * 255), true, false);
    drawAAPixelInit();
    p = src->getDataPtr();
    for (y = 0; y < h; ++y) 
    {
      for (x = 0; x < w; ++x) 
      {
				pipe.shape = *p++;
				drawAAPixel(&pipe, xDest + x, yDest + y);
      }
    }
  }
  else
  {
  	pipeInit(&pipe, xDest, yDest, state->fillPattern, NULL,
	     			(uchar)rasterRound(state->fillAlpha * 255), true, false);
    p = src->getDataPtr();
    if (clipRes == CLIP_ALLINSIDE) 
    {
      for (y = 0; y < h; ++y) 
      {
				pipeSetXY(&pipe, xDest, yDest + y);
				for (x = 0; x < w; ++x) 
				{
	  			if (*p) 
	  			{
	    			pipe.shape = *p;
	    			(this->*pipe.run)(&pipe);
	  			} 
	  			else 
	    			pipeIncX(&pipe);
	 			 	++p;
				}
      }
      updateModX(xDest);
      updateModX(xDest + w - 1);
      updateModY(yDest);
      updateModY(yDest + h - 1);
    } 
    else 
    {
      for (y = 0; y < h; ++y) 
      {
				pipeSetXY(&pipe, xDest, yDest + y);
				for (x = 0; x < w; ++x) 
				{
	  			if (*p && state->clip->test(xDest + x, yDest + y)) 
	  			{
	    			pipe.shape = *p;
	    			(this->*pipe.run)(&pipe);
	    			updateModX(xDest + x);
	    			updateModY(yDest + y);
	  			} 
	  			else 
	    			pipeIncX(&pipe);
	  			++p;
				}
      }
    }
  }
}

inline void XWRaster::drawAALine(RasterPipe *pipe, int x0, int x1, int y)
{
    static int bitCount4[16] = { 0, 1, 1, 2, 1, 2, 2, 3,
			                     1, 2, 2, 3, 2, 3, 3, 4 };
			                     
	uchar * p0 = aaBuf->getDataPtr() + (x0 >> 1);
    uchar * p1 = p0 + aaBuf->getRowSize();
    uchar * p2 = p1 + aaBuf->getRowSize();
    uchar * p3 = p2 + aaBuf->getRowSize();
    pipeSetXY(pipe, x0, y);
    int t = 0;
    for (int x = x0; x <= x1; ++x)
    {
        if (x & 1) 
        {
            t = bitCount4[*p0 & 0x0f] + bitCount4[*p1 & 0x0f] + bitCount4[*p2 & 0x0f] + bitCount4[*p3 & 0x0f];
            ++p0; ++p1; ++p2; ++p3;
        } 
        else 
            t = bitCount4[*p0 >> 4] + bitCount4[*p1 >> 4] + bitCount4[*p2 >> 4] + bitCount4[*p3 >> 4];
            
        if (t != 0) 
        {
            pipe->shape = aaGamma[t];
            (this->*pipe->run)(pipe);
            updateModX(x);
            updateModY(y);
        } 
        else 
            pipeIncX(pipe);
    }
}

inline void XWRaster::drawAAPixel(RasterPipe *pipe, int x, int y)
{
    static int bitCount4[16] = { 0, 1, 1, 2, 1, 2, 2, 3,
			                     1, 2, 2, 3, 2, 3, 3, 4 };
			                     
    if (x < 0 || x >= bitmap->width || y < state->clip->getYMinI() || y > state->clip->getYMaxI()) 
        return;
        
    if (y != aaBufY) 
    {
        memset(aaBuf->data, 0xff, aaBuf->getRowSize() * aaBuf->getHeight());
        int x0 = 0;
        int x1 = bitmap->width - 1;
        state->clip->clipAALine(aaBuf, &x0, &x1, y);
        aaBufY = y;
    }
    
    uchar * p = aaBuf->getDataPtr() + (x >> 1);
    int w = aaBuf->getRowSize();
    int t = 0;
    if (x & 1) 
        t = bitCount4[*p & 0x0f] + bitCount4[p[w] & 0x0f] + bitCount4[p[2*w] & 0x0f] + bitCount4[p[3*w] & 0x0f];
    else 
        t = bitCount4[*p >> 4] + bitCount4[p[w] >> 4] + bitCount4[p[2*w] >> 4] + bitCount4[p[3*w] >> 4];
        
    if (t != 0) 
    {
        pipeSetXY(pipe, x, y);
        pipe->shape *= aaGamma[t];
        (this->*pipe->run)(pipe);
        updateModX(x);
        updateModY(y);
    }
}

inline void XWRaster::drawAAPixelInit()
{
    aaBufY = -1;
}

inline void XWRaster::drawPixel(RasterPipe *pipe, int x, int y, bool noClip)
{
  if (noClip || state->clip->test(x, y)) {
    pipeSetXY(pipe, x, y);
    (this->*pipe->run)(pipe);
    updateModX(x);
    updateModY(y);
  }
}

inline void XWRaster::drawSpan(RasterPipe *pipe, int x0, int x1, int y, bool noClip)
{
  int x;

  if (noClip) {
    pipeSetXY(pipe, x0, y);
    for (x = x0; x <= x1; ++x) {
      (this->*pipe->run)(pipe);
    }
    updateModX(x0);
    updateModX(x1);
    updateModY(y);
  } else {
    if (x0 < state->clip->getXMinI()) {
      x0 = state->clip->getXMinI();
    }
    if (x1 > state->clip->getXMaxI()) {
      x1 = state->clip->getXMaxI();
    }
    pipeSetXY(pipe, x0, y);
    for (x = x0; x <= x1; ++x) {
      if (state->clip->test(x, y)) {
	(this->*pipe->run)(pipe);
	updateModX(x);
	updateModY(y);
      } else {
	pipeIncX(pipe);
      }
    }
  }
}

bool XWRaster::fillGlyph2(int x0, int y0, GlyphBitmap *glyph)
{
  RasterPipe pipe;
  int clipRes;
  bool noClip;
  int alpha0;
  uchar alpha;
  uchar *p;
  int x1, y1, xx, xx1, yy;

  if ((clipRes = state->clip->testRect(x0 - glyph->x,
				       y0 - glyph->y,
				       x0 - glyph->x + glyph->w - 1,
				       y0 - glyph->y + glyph->h - 1))
      != CLIP_ALLOUTSIDE) {
    noClip = clipRes == CLIP_ALLINSIDE;

    if (noClip) {
      if (glyph->aa) {
	pipeInit(&pipe, x0 - glyph->x, y0 - glyph->y, state->fillPattern, NULL,
		 (uchar)rasterRound(state->fillAlpha * 255), true, false);
	p = glyph->data;
	for (yy = 0, y1 = y0 - glyph->y; yy < glyph->h; ++yy, ++y1) {
	  pipeSetXY(&pipe, x0 - glyph->x, y1);
	  for (xx = 0, x1 = x0 - glyph->x; xx < glyph->w; ++xx, ++x1) {
	    alpha = *p++;
	    if (alpha != 0) {
	      pipe.shape = alpha;
	      (this->*pipe.run)(&pipe);
	      updateModX(x1);
	      updateModY(y1);
	    } else {
	      pipeIncX(&pipe);
	    }
	  }
	}
      } else {
	pipeInit(&pipe, x0 - glyph->x, y0 - glyph->y, state->fillPattern, NULL,
		 (uchar)rasterRound(state->fillAlpha * 255), false, false);
	p = glyph->data;
	for (yy = 0, y1 = y0 - glyph->y; yy < glyph->h; ++yy, ++y1) {
	  pipeSetXY(&pipe, x0 - glyph->x, y1);
	  for (xx = 0, x1 = x0 - glyph->x; xx < glyph->w; xx += 8) {
	    alpha0 = *p++;
	    for (xx1 = 0; xx1 < 8 && xx + xx1 < glyph->w; ++xx1, ++x1) {
	      if (alpha0 & 0x80) {
		(this->*pipe.run)(&pipe);
		updateModX(x1);
		updateModY(y1);
	      } else {
		pipeIncX(&pipe);
	      }
	      alpha0 <<= 1;
	    }
	  }
	}
      }
    } else {
      if (glyph->aa) {
	pipeInit(&pipe, x0 - glyph->x, y0 - glyph->y, state->fillPattern, NULL,
		 (uchar)rasterRound(state->fillAlpha * 255), true, false);
	p = glyph->data;
	for (yy = 0, y1 = y0 - glyph->y; yy < glyph->h; ++yy, ++y1) {
	  pipeSetXY(&pipe, x0 - glyph->x, y1);
	  for (xx = 0, x1 = x0 - glyph->x; xx < glyph->w; ++xx, ++x1) {
	    if (state->clip->test(x1, y1)) {
	      alpha = *p++;
	      if (alpha != 0) {
		pipe.shape = alpha;
		(this->*pipe.run)(&pipe);
		updateModX(x1);
		updateModY(y1);
	      } else {
		pipeIncX(&pipe);
	      }
	    } else {
	      pipeIncX(&pipe);
	      ++p;
	    }
	  }
	}
      } else {
	pipeInit(&pipe, x0 - glyph->x, y0 - glyph->y, state->fillPattern, NULL,
		 (uchar)rasterRound(state->fillAlpha * 255), false, false);
	p = glyph->data;
	for (yy = 0, y1 = y0 - glyph->y; yy < glyph->h; ++yy, ++y1) {
	  pipeSetXY(&pipe, x0 - glyph->x, y1);
	  for (xx = 0, x1 = x0 - glyph->x; xx < glyph->w; xx += 8) {
	    alpha0 = *p++;
	    for (xx1 = 0; xx1 < 8 && xx + xx1 < glyph->w; ++xx1, ++x1) {
	      if (state->clip->test(x1, y1)) {
		if (alpha0 & 0x80) {
		  (this->*pipe.run)(&pipe);
		  updateModX(x1);
		  updateModY(y1);
		} else {
		  pipeIncX(&pipe);
		}
	      } else {
		pipeIncX(&pipe);
	      }
	      alpha0 <<= 1;
	    }
	  }
	}
      }
    }
  }
  opClipRes = clipRes;

  return true;
}

bool XWRaster::fillWithPattern(XWRasterPath *path, bool eo, XWRasterPattern *pattern, double alpha)
{
  RasterPipe pipe;
  XWRasterXPath *xPath;
  XWRasterXPathScanner *scanner;
  int xMinI, yMinI, xMaxI, yMaxI, x0, x1, y;
  int clipRes, clipRes2;

  if (path->length == 0) {
    return false;
  }
  if (pathAllOutside(path)) {
    opClipRes = CLIP_ALLOUTSIDE;
    return true;
  }

  // add stroke adjustment hints for filled rectangles -- this only
  // applies to paths that consist of a single subpath
  // (this appears to match Acrobat's behavior)
  if (state->strokeAdjust && !path->hints) {
    int n;
    n = path->getLength();
    if (n == 4 &&
	!(path->flags[0] & PATH_CLOSED) &&
	!(path->flags[1] & PATH_LAST) &&
	!(path->flags[2] & PATH_LAST)) {
      path->close(true);
      path->addStrokeAdjustHint(0, 2, 0, 4);
      path->addStrokeAdjustHint(1, 3, 0, 4);
    } else if (n == 5 &&
	       (path->flags[0] & PATH_CLOSED) &&
	       !(path->flags[1] & PATH_LAST) &&
	       !(path->flags[2] & PATH_LAST) &&
	       !(path->flags[3] & PATH_LAST)) {
      path->addStrokeAdjustHint(0, 2, 0, 4);
      path->addStrokeAdjustHint(1, 3, 0, 4);
    }
  }

  xPath = new XWRasterXPath(path, state->matrix, state->flatness, true);
  if (vectorAntialias && !inShading) {
    xPath->aaScale();
  }
  xPath->sort();
  yMinI = state->clip->getYMinI();
  yMaxI = state->clip->getYMaxI();
  if (vectorAntialias && !inShading) {
    yMinI = yMinI * AA_SIZE;
    yMaxI = (yMaxI + 1) * AA_SIZE - 1;
  }
  scanner = new XWRasterXPathScanner(xPath, eo, yMinI, yMaxI);

  // get the min and max x and y values
  if (vectorAntialias && !inShading) {
    scanner->getBBoxAA(&xMinI, &yMinI, &xMaxI, &yMaxI);
  } else {
    scanner->getBBox(&xMinI, &yMinI, &xMaxI, &yMaxI);
  }
  
  // check clipping
  if ((clipRes = state->clip->testRect(xMinI, yMinI, xMaxI, yMaxI))
      != CLIP_ALLOUTSIDE) {
    if (scanner->hasPartialClip()) {
      clipRes = CLIP_PARTIAL;
    }

    pipeInit(&pipe, 0, yMinI, pattern, NULL, (uchar)rasterRound(alpha * 255),
	     vectorAntialias && !inShading, false);

    // draw the spans
    if (vectorAntialias && !inShading) {
      for (y = yMinI; y <= yMaxI; ++y) {
	scanner->renderAALine(aaBuf, &x0, &x1, y);
	if (clipRes != CLIP_ALLINSIDE) {
	  state->clip->clipAALine(aaBuf, &x0, &x1, y);
	}
	drawAALine(&pipe, x0, x1, y);
      }
    } else {
      for (y = yMinI; y <= yMaxI; ++y) {
	while (scanner->getNextSpan(y, &x0, &x1)) {
	  if (clipRes == CLIP_ALLINSIDE) {
	    drawSpan(&pipe, x0, x1, y, true);
	  } else {
	    // limit the x range
	    if (x0 < state->clip->getXMinI()) {
	      x0 = state->clip->getXMinI();
	    }
	    if (x1 > state->clip->getXMaxI()) {
	      x1 = state->clip->getXMaxI();
	    }
	    clipRes2 = state->clip->testSpan(x0, x1, y);
	    drawSpan(&pipe, x0, x1, y, clipRes2 == CLIP_ALLINSIDE);
	  }
	}
      }
    }
  }
  opClipRes = clipRes;

  delete scanner;
  delete xPath;
  return true;
}

void XWRaster::flattenCurve(double x0, 
                            double y0,
			                double x1, 
			                double y1,
			                double x2, 
			                double y2,
			                double x3, 
			                double y3,
			                double *matrix, 
		                    double flatness2,
		                    XWRasterPath *fPath)
{
  double cx[MAX_CURVE_SPLITS + 1][3];
  double cy[MAX_CURVE_SPLITS + 1][3];
  int cNext[MAX_CURVE_SPLITS + 1];
  double xl0, xl1, xl2, xr0, xr1, xr2, xr3, xx1, xx2, xh;
  double yl0, yl1, yl2, yr0, yr1, yr2, yr3, yy1, yy2, yh;
  double dx, dy, mx, my, tx, ty, d1, d2;
  int p1, p2, p3;

  // initial segment
  p1 = 0;
  p2 = MAX_CURVE_SPLITS;
  cx[p1][0] = x0;  cy[p1][0] = y0;
  cx[p1][1] = x1;  cy[p1][1] = y1;
  cx[p1][2] = x2;  cy[p1][2] = y2;
  cx[p2][0] = x3;  cy[p2][0] = y3;
  cNext[p1] = p2;

  while (p1 < MAX_CURVE_SPLITS) {

    // get the next segment
    xl0 = cx[p1][0];  yl0 = cy[p1][0];
    xx1 = cx[p1][1];  yy1 = cy[p1][1];
    xx2 = cx[p1][2];  yy2 = cy[p1][2];
    p2 = cNext[p1];
    xr3 = cx[p2][0];  yr3 = cy[p2][0];

    // compute the distances (in device space) from the control points
    // to the midpoint of the straight line (this is a bit of a hack,
    // but it's much faster than computing the actual distances to the
    // line)
    transform(matrix, (xl0 + xr3) * 0.5, (yl0 + yr3) * 0.5, &mx, &my);
    transform(matrix, xx1, yy1, &tx, &ty);
    dx = tx - mx;
    dy = ty - my;
    d1 = dx*dx + dy*dy;
    transform(matrix, xx2, yy2, &tx, &ty);
    dx = tx - mx;
    dy = ty - my;
    d2 = dx*dx + dy*dy;

    // if the curve is flat enough, or no more subdivisions are
    // allowed, add the straight line segment
    if (p2 - p1 == 1 || (d1 <= flatness2 && d2 <= flatness2)) {
      fPath->lineTo(xr3, yr3);
      p1 = p2;

    // otherwise, subdivide the curve
    } else {
      xl1 = rasterAvg(xl0, xx1);
      yl1 = rasterAvg(yl0, yy1);
      xh = rasterAvg(xx1, xx2);
      yh = rasterAvg(yy1, yy2);
      xl2 = rasterAvg(xl1, xh);
      yl2 = rasterAvg(yl1, yh);
      xr2 = rasterAvg(xx2, xr3);
      yr2 = rasterAvg(yy2, yr3);
      xr1 = rasterAvg(xh, xr2);
      yr1 = rasterAvg(yh, yr2);
      xr0 = rasterAvg(xl2, xr1);
      yr0 = rasterAvg(yl2, yr1);
      // add the new subdivision points
      p3 = (p1 + p2) / 2;
      cx[p1][1] = xl1;  cy[p1][1] = yl1;
      cx[p1][2] = xl2;  cy[p1][2] = yl2;
      cNext[p1] = p3;
      cx[p3][0] = xr0;  cy[p3][0] = yr0;
      cx[p3][1] = xr1;  cy[p3][1] = yr1;
      cx[p3][2] = xr2;  cy[p3][2] = yr2;
      cNext[p3] = p2;
    }
  }
}

XWRasterPath * XWRaster::flattenPath(XWRasterPath *path, double *matrix, double flatness)
{
  XWRasterPath *fPath;
  double flatness2;
  uchar flag;
  int i;

  fPath = new XWRasterPath();
  flatness2 = flatness * flatness;
  
  i = 0;
  while (i < path->length) {
    flag = path->flags[i];
    if (flag & PATH_FIRST) {
      fPath->moveTo(path->pts[i].x, path->pts[i].y);
      ++i;
    } else {
      if (flag & PATH_CURVE) {
	flattenCurve(path->pts[i-1].x, path->pts[i-1].y,
		     path->pts[i  ].x, path->pts[i  ].y,
		     path->pts[i+1].x, path->pts[i+1].y,
		     path->pts[i+2].x, path->pts[i+2].y,
		     matrix, flatness2, fPath);
	i += 3;
      } else {
	fPath->lineTo(path->pts[i].x, path->pts[i].y);
	++i;
      }
      if (path->flags[i-1] & PATH_CLOSED) {
	fPath->close();
      }
    }
  }
  return fPath;
}

XWRasterPath * XWRaster::makeDashedPath(XWRasterPath *path)
{
  XWRasterPath *dPath;
  double lineDashTotal;
  double lineDashStartPhase, lineDashDist, segLen;
  double x0, y0, x1, y1, xa, ya;
  bool lineDashStartOn, lineDashOn, newPath;
  int lineDashStartIdx, lineDashIdx;
  int i, j, k;

  lineDashTotal = 0;
  for (i = 0; i < state->lineDashLength; ++i) {
    lineDashTotal += state->lineDash[i];
  }
  // Acrobat simply draws nothing if the dash array is [0]
  if (lineDashTotal == 0) {
    return new XWRasterPath();
  }
  lineDashStartPhase = state->lineDashPhase;
  i = rasterFloor(lineDashStartPhase / lineDashTotal);
  lineDashStartPhase -= (double)i * lineDashTotal;
  lineDashStartOn = true;
  lineDashStartIdx = 0;
  if (lineDashStartPhase > 0) {
    while (lineDashStartPhase >= state->lineDash[lineDashStartIdx]) {
      lineDashStartOn = !lineDashStartOn;
      lineDashStartPhase -= state->lineDash[lineDashStartIdx];
      ++lineDashStartIdx;
    }
  }

  dPath = new XWRasterPath();

  // process each subpath
  i = 0;
  while (i < path->length) {

    // find the end of the subpath
    for (j = i;
	 j < path->length - 1 && !(path->flags[j] & PATH_LAST);
	 ++j) ;

    // initialize the dash parameters
    lineDashOn = lineDashStartOn;
    lineDashIdx = lineDashStartIdx;
    lineDashDist = state->lineDash[lineDashIdx] - lineDashStartPhase;

    // process each segment of the subpath
    newPath = true;
    for (k = i; k < j; ++k) {

      // grab the segment
      x0 = path->pts[k].x;
      y0 = path->pts[k].y;
      x1 = path->pts[k+1].x;
      y1 = path->pts[k+1].y;
      segLen = rasterDist(x0, y0, x1, y1);

      // process the segment
      while (segLen > 0) {

	if (lineDashDist >= segLen) {
	  if (lineDashOn) {
	    if (newPath) {
	      dPath->moveTo(x0, y0);
	      newPath = false;
	    }
	    dPath->lineTo(x1, y1);
	  }
	  lineDashDist -= segLen;
	  segLen = 0;

	} else {
	  xa = x0 + (lineDashDist / segLen) * (x1 - x0);
	  ya = y0 + (lineDashDist / segLen) * (y1 - y0);
	  if (lineDashOn) {
	    if (newPath) {
	      dPath->moveTo(x0, y0);
	      newPath = false;
	    }
	    dPath->lineTo(xa, ya);
	  }
	  x0 = xa;
	  y0 = ya;
	  segLen -= lineDashDist;
	  lineDashDist = 0;
	}

	// get the next entry in the dash array
	if (lineDashDist <= 0) {
	  lineDashOn = !lineDashOn;
	  if (++lineDashIdx == state->lineDashLength) {
	    lineDashIdx = 0;
	  }
	  lineDashDist = state->lineDash[lineDashIdx];
	  newPath = true;
	}
      }
    }
    i = j + 1;
  }

  return dPath;
}

bool XWRaster::pathAllOutside(XWRasterPath *path)
{
	double xMin1, yMin1, xMax1, yMax1;
  double xMin2, yMin2, xMax2, yMax2;
  double x, y;
  int xMinI, yMinI, xMaxI, yMaxI;
  
  xMin1 = xMax1 = path->pts[0].x;
  yMin1 = yMax1 = path->pts[0].y;
  for (int i = 1; i < path->length; ++i) 
 	{
    if (path->pts[i].x < xMin1) 
      xMin1 = path->pts[i].x;
    else if (path->pts[i].x > xMax1) 
      xMax1 = path->pts[i].x;
    
    if (path->pts[i].y < yMin1) 
      yMin1 = path->pts[i].y;
    else if (path->pts[i].y > yMax1) 
      yMax1 = path->pts[i].y;
  }

  transform(state->matrix, xMin1, yMin1, &x, &y);
  xMin2 = xMax2 = x;
  yMin2 = yMax2 = y;
  transform(state->matrix, xMin1, yMax1, &x, &y);
  if (x < xMin2) 
    xMin2 = x;
  else if (x > xMax2) 
    xMax2 = x;
  if (y < yMin2) 
    yMin2 = y;
  else if (y > yMax2) 
    yMax2 = y;
  transform(state->matrix, xMax1, yMin1, &x, &y);
  if (x < xMin2) 
    xMin2 = x;
  else if (x > xMax2) 
    xMax2 = x;
  if (y < yMin2) 
    yMin2 = y;
  else if (y > yMax2) 
    yMax2 = y;
  transform(state->matrix, xMax1, yMax1, &x, &y);
  if (x < xMin2) 
    xMin2 = x;
  else if (x > xMax2) 
    xMax2 = x;
  if (y < yMin2) 
    yMin2 = y;
  else if (y > yMax2) 
    yMax2 = y;
  xMinI = rasterFloor(xMin2);
  yMinI = rasterFloor(yMin2);
  xMaxI = rasterFloor(xMax2);
  yMaxI = rasterFloor(yMax2);

  return state->clip->testRect(xMinI, yMinI, xMaxI, yMaxI) == CLIP_ALLOUTSIDE;
}

inline void XWRaster::pipeIncX(RasterPipe *pipe)
{
  ++pipe->x;
  if (state->softMask) {
    ++pipe->softMaskPtr;
  }
  switch (bitmap->mode) {
  case COLOR_MODE_MONO1:
    if (!(pipe->destColorMask >>= 1)) {
      pipe->destColorMask = 0x80;
      ++pipe->destColorPtr;
    }
    break;
  case COLOR_MODE_MONO8:
    ++pipe->destColorPtr;
    break;
  case COLOR_MODE_RGB8:
  case COLOR_MODE_BGR8:
    pipe->destColorPtr += 3;
    break;
  case COLOR_MODE_CMYK8:
    pipe->destColorPtr += 4;
    break;
  }
  if (pipe->destAlphaPtr) {
    ++pipe->destAlphaPtr;
  }
  if (pipe->alpha0Ptr) {
    ++pipe->alpha0Ptr;
  }
}

inline void XWRaster::pipeInit(RasterPipe *pipe, int x, int y,
		                       XWRasterPattern *pattern, uchar * cSrc,
		                       double aInput, bool usesShape,
		                       bool nonIsolatedGroup)
{
  pipeSetXY(pipe, x, y);
  pipe->pattern = NULL;

  // source color
  if (pattern) {
    if (pattern->isStatic()) {
      pattern->getColor(x, y, pipe->cSrcVal);
    } else {
      pipe->pattern = pattern;
    }
    pipe->cSrc = pipe->cSrcVal;
  } else {
    pipe->cSrc = cSrc;
  }

  // source alpha
  pipe->aInput = aInput;
  pipe->usesShape = usesShape;

  // result alpha
  if (aInput == 255 && !state->softMask && !usesShape &&
      !state->inNonIsolatedGroup && !nonIsolatedGroup) {
    pipe->noTransparency = true;
  } else {
    pipe->noTransparency = false;
  }

  // result color
  if (pipe->noTransparency) {
    // the !state->blendFunc case is handled separately in pipeRun
    pipe->resultColorCtrl = pipeResultColorNoAlphaBlend[bitmap->mode];
  } else if (!state->blendFunc) {
    pipe->resultColorCtrl = pipeResultColorAlphaNoBlend[bitmap->mode];
  } else {
    pipe->resultColorCtrl = pipeResultColorAlphaBlend[bitmap->mode];
  }

  // non-isolated group correction
  pipe->nonIsolatedGroup = nonIsolatedGroup;

  // select the 'run' function
  pipe->run = &XWRaster::pipeRun;
  if (!pipe->pattern && pipe->noTransparency && !state->blendFunc) {
    if (bitmap->mode == COLOR_MODE_MONO1 && !pipe->destAlphaPtr) {
      pipe->run = &XWRaster::pipeRunSimpleMono1;
    } else if (bitmap->mode == COLOR_MODE_MONO8 && pipe->destAlphaPtr) {
      pipe->run = &XWRaster::pipeRunSimpleMono8;
    } else if (bitmap->mode == COLOR_MODE_RGB8 && pipe->destAlphaPtr) {
      pipe->run = &XWRaster::pipeRunSimpleRGB8;
    } else if (bitmap->mode == COLOR_MODE_BGR8 && pipe->destAlphaPtr) {
      pipe->run = &XWRaster::pipeRunSimpleBGR8;
    } else if (bitmap->mode == COLOR_MODE_CMYK8 && pipe->destAlphaPtr) {
      pipe->run = &XWRaster::pipeRunSimpleCMYK8;
    }
  } else if (!pipe->pattern && !pipe->noTransparency && !state->softMask &&
	     pipe->usesShape &&
	     !(state->inNonIsolatedGroup && alpha0Bitmap->alpha) &&
	     !state->blendFunc && !pipe->nonIsolatedGroup) {
    if (bitmap->mode == COLOR_MODE_MONO1 && !pipe->destAlphaPtr) {
      pipe->run = &XWRaster::pipeRunAAMono1;
    } else if (bitmap->mode == COLOR_MODE_MONO8 && pipe->destAlphaPtr) {
      pipe->run = &XWRaster::pipeRunAAMono8;
    } else if (bitmap->mode == COLOR_MODE_RGB8 && pipe->destAlphaPtr) {
      pipe->run = &XWRaster::pipeRunAARGB8;
    } else if (bitmap->mode == COLOR_MODE_BGR8 && pipe->destAlphaPtr) {
      pipe->run = &XWRaster::pipeRunAABGR8;
    } else if (bitmap->mode == COLOR_MODE_CMYK8 && pipe->destAlphaPtr) {
      pipe->run = &XWRaster::pipeRunAACMYK8;
    }
  }
}

inline void XWRaster::pipeRun(RasterPipe *pipe)
{
  uchar aSrc, aDest, alphaI, alphaIm1, alpha0, aResult;
  uchar cSrcNonIso[4], cDest[4], cBlend[4];
  uchar* cSrc;
  uchar cResult0, cResult1, cResult2, cResult3;
  int t;
  uchar cSrc2[4], cDest2[4];

  //----- source color

  // static pattern: handled in pipeInit
  // fixed color: handled in pipeInit

  // dynamic pattern
  if (pipe->pattern) {
    pipe->pattern->getColor(pipe->x, pipe->y, pipe->cSrcVal);
  }

  if (pipe->noTransparency && !state->blendFunc) {

    //----- write destination pixel

    switch (bitmap->mode) {
    case COLOR_MODE_MONO1:
      cResult0 = state->grayTransfer[pipe->cSrc[0]];
      if (state->screen->test(pipe->x, pipe->y, cResult0)) {
	*pipe->destColorPtr |= pipe->destColorMask;
      } else {
	*pipe->destColorPtr &= ~pipe->destColorMask;
      }
      if (!(pipe->destColorMask >>= 1)) {
	pipe->destColorMask = 0x80;
	++pipe->destColorPtr;
      }
      break;
    case COLOR_MODE_MONO8:
      *pipe->destColorPtr++ = state->grayTransfer[pipe->cSrc[0]];
      break;
    case COLOR_MODE_RGB8:
      *pipe->destColorPtr++ = state->rgbTransferR[pipe->cSrc[0]];
      *pipe->destColorPtr++ = state->rgbTransferG[pipe->cSrc[1]];
      *pipe->destColorPtr++ = state->rgbTransferB[pipe->cSrc[2]];
      break;
    case COLOR_MODE_BGR8:
      *pipe->destColorPtr++ = state->rgbTransferB[pipe->cSrc[2]];
      *pipe->destColorPtr++ = state->rgbTransferG[pipe->cSrc[1]];
      *pipe->destColorPtr++ = state->rgbTransferR[pipe->cSrc[0]];
      break;
    case COLOR_MODE_CMYK8:
      if (state->overprintMask & 1) {
	pipe->destColorPtr[0] = state->cmykTransferC[pipe->cSrc[0]];
      }
      if (state->overprintMask & 2) {
	pipe->destColorPtr[1] = state->cmykTransferM[pipe->cSrc[1]];
      }
      if (state->overprintMask & 4) {
	pipe->destColorPtr[2] = state->cmykTransferY[pipe->cSrc[2]];
      }
      if (state->overprintMask & 8) {
	pipe->destColorPtr[3] = state->cmykTransferK[pipe->cSrc[3]];
      }
      pipe->destColorPtr += 4;
      break;
    }
    if (pipe->destAlphaPtr) {
      *pipe->destAlphaPtr++ = 255;
    }

  } else {

    //----- read destination pixel

    switch (bitmap->mode) {
    case COLOR_MODE_MONO1:
      cDest[0] = (*pipe->destColorPtr & pipe->destColorMask) ? 0xff : 0x00;
      break;
    case COLOR_MODE_MONO8:
      cDest[0] = *pipe->destColorPtr;
      break;
    case COLOR_MODE_RGB8:
      cDest[0] = pipe->destColorPtr[0];
      cDest[1] = pipe->destColorPtr[1];
      cDest[2] = pipe->destColorPtr[2];
      break;
    case COLOR_MODE_BGR8:
      cDest[0] = pipe->destColorPtr[2];
      cDest[1] = pipe->destColorPtr[1];
      cDest[2] = pipe->destColorPtr[0];
      break;
    case COLOR_MODE_CMYK8:
      cDest[0] = pipe->destColorPtr[0];
      cDest[1] = pipe->destColorPtr[1];
      cDest[2] = pipe->destColorPtr[2];
      cDest[3] = pipe->destColorPtr[3];
      break;
    }
    if (pipe->destAlphaPtr) {
      aDest = *pipe->destAlphaPtr;
    } else {
      aDest = 0xff;
    }

    //----- source alpha

    if (state->softMask) {
      if (pipe->usesShape) {
	aSrc = div255(div255(pipe->aInput * *pipe->softMaskPtr++) *
		      pipe->shape);
      } else {
	aSrc = div255(pipe->aInput * *pipe->softMaskPtr++);
      }
    } else if (pipe->usesShape) {
      aSrc = div255(pipe->aInput * pipe->shape);
    } else {
      aSrc = pipe->aInput;
    }

    //----- non-isolated group correction

    if (pipe->nonIsolatedGroup) {
      // This path is only used when Splash::composite() is called to
      // composite a non-isolated group onto the backdrop.  In this
      // case, pipe->shape is the source (group) alpha.
      if (pipe->shape == 0) {
	// this value will be multiplied by zero later, so it doesn't
	// matter what we use
	cSrc = pipe->cSrc;
      } else {
	t = (aDest * 255) / pipe->shape - aDest;
	switch (bitmap->mode) {
	case COLOR_MODE_CMYK8:
	  cSrcNonIso[3] = clip255(pipe->cSrc[3] + 
				  ((pipe->cSrc[3] - cDest[3]) * t) / 255);
	case COLOR_MODE_RGB8:
	case COLOR_MODE_BGR8:
	  cSrcNonIso[2] = clip255(pipe->cSrc[2] +
				  ((pipe->cSrc[2] - cDest[2]) * t) / 255);
	  cSrcNonIso[1] = clip255(pipe->cSrc[1] +
				  ((pipe->cSrc[1] - cDest[1]) * t) / 255);
	case COLOR_MODE_MONO1:
	case COLOR_MODE_MONO8:
	  cSrcNonIso[0] = clip255(pipe->cSrc[0] +
				  ((pipe->cSrc[0] - cDest[0]) * t) / 255);
	  break;
	}
	cSrc = cSrcNonIso;
      }
    } else {
      cSrc = pipe->cSrc;
    }

    //----- blend function

    if (state->blendFunc) {
      if (bitmap->mode == COLOR_MODE_CMYK8) {
	// convert colors to additive
	cSrc2[0] = 0xff - cSrc[0];
	cSrc2[1] = 0xff - cSrc[1];
	cSrc2[2] = 0xff - cSrc[2];
	cSrc2[3] = 0xff - cSrc[3];
	cDest2[0] = 0xff - cDest[0];
	cDest2[1] = 0xff - cDest[1];
	cDest2[2] = 0xff - cDest[2];
	cDest2[3] = 0xff - cDest[3];
	(*state->blendFunc)(cSrc2, cDest2, cBlend, bitmap->mode);
	// convert result back to subtractive
	cBlend[0] = 0xff - cBlend[0];
	cBlend[1] = 0xff - cBlend[1];
	cBlend[2] = 0xff - cBlend[2];
	cBlend[3] = 0xff - cBlend[3];
      } 
      else
      	(*state->blendFunc)(cSrc, cDest, cBlend, bitmap->mode);
    }

    //----- result alpha and non-isolated group element correction

    if (pipe->noTransparency) {
      alphaI = alphaIm1 = aResult = 255;
    } else {
      aResult = aSrc + aDest - div255(aSrc * aDest);

      // alphaI = alpha_i
      // alphaIm1 = alpha_(i-1)
      if (pipe->alpha0Ptr) {
	alpha0 = *pipe->alpha0Ptr++;
	alphaI = aResult + alpha0 - div255(aResult * alpha0);
	alphaIm1 = alpha0 + aDest - div255(alpha0 * aDest);
      } else {
	alphaI = aResult;
	alphaIm1 = aDest;
      }
    }

    //----- result color

    cResult0 = cResult1 = cResult2 = cResult3 = 0; // make gcc happy

    switch (pipe->resultColorCtrl) {

    case PIPE_RESULT_COLOR_NOALPHABLENDMONO:
      cResult0 = state->grayTransfer[div255((255 - aDest) * cSrc[0] +
					    aDest * cBlend[0])];
      break;
    case PIPE_RESULT_COLOR_NOALPHABLENDRGB:
      cResult0 = state->rgbTransferR[div255((255 - aDest) * cSrc[0] +
					    aDest * cBlend[0])];
      cResult1 = state->rgbTransferG[div255((255 - aDest) * cSrc[1] +
					    aDest * cBlend[1])];
      cResult2 = state->rgbTransferB[div255((255 - aDest) * cSrc[2] +
					    aDest * cBlend[2])];
      break;
    case PIPE_RESULT_COLOR_NOALPHABLENDCMYK:
      cResult0 = state->cmykTransferC[div255((255 - aDest) * cSrc[0] +
					     aDest * cBlend[0])];
      cResult1 = state->cmykTransferM[div255((255 - aDest) * cSrc[1] +
					     aDest * cBlend[1])];
      cResult2 = state->cmykTransferY[div255((255 - aDest) * cSrc[2] +
					     aDest * cBlend[2])];
      cResult3 = state->cmykTransferK[div255((255 - aDest) * cSrc[3] +
					     aDest * cBlend[3])];
      break;

    case PIPE_RESULT_COLOR_ALPHANOBLENDMONO:
      if (alphaI == 0) {
	cResult0 = 0;
      } else {
	cResult0 = state->grayTransfer[((alphaI - aSrc) * cDest[0] +
					aSrc * cSrc[0]) / alphaI];
      }
      break;
    case PIPE_RESULT_COLOR_ALPHANOBLENDRGB:
      if (alphaI == 0) {
	cResult0 = 0;
	cResult1 = 0;
	cResult2 = 0;
      } else {
	cResult0 = state->rgbTransferR[((alphaI - aSrc) * cDest[0] +
					aSrc * cSrc[0]) / alphaI];
	cResult1 = state->rgbTransferG[((alphaI - aSrc) * cDest[1] +
					aSrc * cSrc[1]) / alphaI];
	cResult2 = state->rgbTransferB[((alphaI - aSrc) * cDest[2] +
					aSrc * cSrc[2]) / alphaI];
      }
      break;
    case PIPE_RESULT_COLOR_ALPHANOBLENDCMYK:
      if (alphaI == 0) {
	cResult0 = 0;
	cResult1 = 0;
	cResult2 = 0;
	cResult3 = 0;
      } else {
	cResult0 = state->cmykTransferC[((alphaI - aSrc) * cDest[0] +
					 aSrc * cSrc[0]) / alphaI];
	cResult1 = state->cmykTransferM[((alphaI - aSrc) * cDest[1] +
					 aSrc * cSrc[1]) / alphaI];
	cResult2 = state->cmykTransferY[((alphaI - aSrc) * cDest[2] +
					 aSrc * cSrc[2]) / alphaI];
	cResult3 = state->cmykTransferK[((alphaI - aSrc) * cDest[3] +
					 aSrc * cSrc[3]) / alphaI];
      }
      break;

    case PIPE_RESULT_COLOR_ALPHABLENDMONO:
      if (alphaI == 0) {
	cResult0 = 0;
      } else {
	cResult0 = state->grayTransfer[((alphaI - aSrc) * cDest[0] +
					aSrc * ((255 - alphaIm1) * cSrc[0] +
						alphaIm1 * cBlend[0]) / 255) /
				       alphaI];
      }
      break;
    case PIPE_RESULT_COLOR_ALPHABLENDRGB:
      if (alphaI == 0) {
	cResult0 = 0;
	cResult1 = 0;
	cResult2 = 0;
      } else {
	cResult0 = state->rgbTransferR[((alphaI - aSrc) * cDest[0] +
					aSrc * ((255 - alphaIm1) * cSrc[0] +
						alphaIm1 * cBlend[0]) / 255) /
				       alphaI];
	cResult1 = state->rgbTransferG[((alphaI - aSrc) * cDest[1] +
					aSrc * ((255 - alphaIm1) * cSrc[1] +
						alphaIm1 * cBlend[1]) / 255) /
				       alphaI];
	cResult2 = state->rgbTransferB[((alphaI - aSrc) * cDest[2] +
					aSrc * ((255 - alphaIm1) * cSrc[2] +
						alphaIm1 * cBlend[2]) / 255) /
				       alphaI];
      }
      break;
      
    case PIPE_RESULT_COLOR_ALPHABLENDCMYK:
      if (alphaI == 0) {
	cResult0 = 0;
	cResult1 = 0;
	cResult2 = 0;
	cResult3 = 0;
      } else {
	cResult0 = state->cmykTransferC[((alphaI - aSrc) * cDest[0] +
					 aSrc * ((255 - alphaIm1) * cSrc[0] +
						 alphaIm1 * cBlend[0]) / 255) /
					alphaI];
	cResult1 = state->cmykTransferM[((alphaI - aSrc) * cDest[1] +
					 aSrc * ((255 - alphaIm1) * cSrc[1] +
						 alphaIm1 * cBlend[1]) / 255) /
					alphaI];
	cResult2 = state->cmykTransferY[((alphaI - aSrc) * cDest[2] +
					 aSrc * ((255 - alphaIm1) * cSrc[2] +
						 alphaIm1 * cBlend[2]) / 255) /
					alphaI];
	cResult3 = state->cmykTransferK[((alphaI - aSrc) * cDest[3] +
					 aSrc * ((255 - alphaIm1) * cSrc[3] +
						 alphaIm1 * cBlend[3]) / 255) /
					alphaI];
      }
      break;
    }

    //----- write destination pixel

    switch (bitmap->mode) {
    case COLOR_MODE_MONO1:
      if (state->screen->test(pipe->x, pipe->y, cResult0)) {
	*pipe->destColorPtr |= pipe->destColorMask;
      } else {
	*pipe->destColorPtr &= ~pipe->destColorMask;
      }
      if (!(pipe->destColorMask >>= 1)) {
	pipe->destColorMask = 0x80;
	++pipe->destColorPtr;
      }
      break;
    case COLOR_MODE_MONO8:
      *pipe->destColorPtr++ = cResult0;
      break;
    case COLOR_MODE_RGB8:
      *pipe->destColorPtr++ = cResult0;
      *pipe->destColorPtr++ = cResult1;
      *pipe->destColorPtr++ = cResult2;
      break;
    case COLOR_MODE_BGR8:
      *pipe->destColorPtr++ = cResult2;
      *pipe->destColorPtr++ = cResult1;
      *pipe->destColorPtr++ = cResult0;
      break;
    case COLOR_MODE_CMYK8:
      if (state->overprintMask & 1) {
	pipe->destColorPtr[0] = cResult0;
      }
      if (state->overprintMask & 2) {
	pipe->destColorPtr[1] = cResult1;
      }
      if (state->overprintMask & 4) {
	pipe->destColorPtr[2] = cResult2;
      }
      if (state->overprintMask & 8) {
	pipe->destColorPtr[3] = cResult3;
      }
      pipe->destColorPtr += 4;
      break;
    }
  
    if (pipe->destAlphaPtr) {
      *pipe->destAlphaPtr++ = aResult;  
    }

  }

  ++pipe->x;
}

void XWRaster::pipeRunSimpleMono1(RasterPipe *pipe)
{
	uchar cResult0 = state->grayTransfer[pipe->cSrc[0]];
  if (state->screen->test(pipe->x, pipe->y, cResult0)) 
    *pipe->destColorPtr |= pipe->destColorMask;
  else 
    *pipe->destColorPtr &= ~pipe->destColorMask;
    
  if (!(pipe->destColorMask >>= 1)) 
  {
    pipe->destColorMask = 0x80;
    ++pipe->destColorPtr;
  }

  ++pipe->x;
}

void XWRaster::pipeRunSimpleMono8(RasterPipe *pipe)
{
	*pipe->destColorPtr++ = state->grayTransfer[pipe->cSrc[0]];
  *pipe->destAlphaPtr++ = 255;

  ++pipe->x;
}

void XWRaster::pipeRunSimpleRGB8(RasterPipe *pipe)
{
	*pipe->destColorPtr++ = state->rgbTransferR[pipe->cSrc[0]];
  *pipe->destColorPtr++ = state->rgbTransferG[pipe->cSrc[1]];
  *pipe->destColorPtr++ = state->rgbTransferB[pipe->cSrc[2]];
  *pipe->destAlphaPtr++ = 255;

  ++pipe->x;
}

void XWRaster::pipeRunSimpleBGR8(RasterPipe *pipe)
{
	*pipe->destColorPtr++ = state->rgbTransferB[pipe->cSrc[2]];
  *pipe->destColorPtr++ = state->rgbTransferG[pipe->cSrc[1]];
  *pipe->destColorPtr++ = state->rgbTransferR[pipe->cSrc[0]];
  *pipe->destAlphaPtr++ = 255;

  ++(pipe->x);
}

void XWRaster::pipeRunSimpleCMYK8(RasterPipe *pipe)
{
	if (state->overprintMask & 1) 
    pipe->destColorPtr[0] = state->cmykTransferC[pipe->cSrc[0]];
  if (state->overprintMask & 2) 
  	pipe->destColorPtr[1] = state->cmykTransferM[pipe->cSrc[1]];
  if (state->overprintMask & 4) 
    pipe->destColorPtr[2] = state->cmykTransferY[pipe->cSrc[2]];
  if (state->overprintMask & 8) 
    pipe->destColorPtr[3] = state->cmykTransferK[pipe->cSrc[3]];
  pipe->destColorPtr += 4;
  *pipe->destAlphaPtr++ = 255;

  ++pipe->x;
}

void XWRaster::pipeRunAAMono1(RasterPipe *pipe)
{
	uchar aSrc;
  uchar cDest[4];
  uchar cResult0;

  //----- read destination pixel
  cDest[0] = (*pipe->destColorPtr & pipe->destColorMask) ? 0xff : 0x00;

  //----- source alpha
  aSrc = div255(pipe->aInput * pipe->shape);

  //----- result color
  // note: aDest = alpha2 = aResult = 0xff
  cResult0 = state->grayTransfer[(uchar)div255((0xff - aSrc) * cDest[0] +
						aSrc * pipe->cSrc[0])];

  //----- write destination pixel
  if (state->screen->test(pipe->x, pipe->y, cResult0)) {
    *pipe->destColorPtr |= pipe->destColorMask;
  } else {
    *pipe->destColorPtr &= ~pipe->destColorMask;
  }
  if (!(pipe->destColorMask >>= 1)) {
    pipe->destColorMask = 0x80;
    ++pipe->destColorPtr;
  }

  ++pipe->x;
}

void XWRaster::pipeRunAAMono8(RasterPipe *pipe)
{
	uchar cDest[4];
	cDest[0] = *pipe->destColorPtr;
  uchar aDest = *pipe->destAlphaPtr;
  uchar aSrc = div255(pipe->aInput * pipe->shape);
  uchar aResult = aSrc + aDest - div255(aSrc * aDest);
  uchar alpha2 = aResult;
  
  uchar cResult0 = 0;
  if (alpha2 != 0) 
    cResult0 = state->grayTransfer[(uchar)(((alpha2 - aSrc) * cDest[0] + aSrc * pipe->cSrc[0]) / alpha2)];

  *pipe->destColorPtr++ = cResult0;
  *pipe->destAlphaPtr++ = aResult;

  ++pipe->x;
}

void XWRaster::pipeRunAARGB8(RasterPipe *pipe)
{
	uchar cDest[4];
	cDest[0] = pipe->destColorPtr[0];
  cDest[1] = pipe->destColorPtr[1];
  cDest[2] = pipe->destColorPtr[2];
  uchar aDest = *pipe->destAlphaPtr;
  uchar aSrc = div255(pipe->aInput * pipe->shape);
  
  uchar aResult = aSrc + aDest - div255(aSrc * aDest);
  uchar alpha2 = aResult;

  uchar cResult0 = 0;
  uchar cResult1 = 0;
  uchar cResult2 = 0;
  if (alpha2 != 0) 
  {
    cResult0 = state->rgbTransferR[(uchar)(((alpha2 - aSrc) * cDest[0] + aSrc * pipe->cSrc[0]) / alpha2)];
    cResult1 = state->rgbTransferG[(uchar)(((alpha2 - aSrc) * cDest[1] + aSrc * pipe->cSrc[1]) / alpha2)];
    cResult2 = state->rgbTransferB[(uchar)(((alpha2 - aSrc) * cDest[2] + aSrc * pipe->cSrc[2]) / alpha2)];
  }

  *pipe->destColorPtr++ = cResult0;
  *pipe->destColorPtr++ = cResult1;
  *pipe->destColorPtr++ = cResult2;
  *pipe->destAlphaPtr++ = aResult;

  ++pipe->x;
}

void XWRaster::pipeRunAABGR8(RasterPipe *pipe)
{
	uchar cDest[4];
	cDest[0] = pipe->destColorPtr[2];
  cDest[1] = pipe->destColorPtr[1];
  cDest[2] = pipe->destColorPtr[0];
  uchar aDest = *pipe->destAlphaPtr;

  uchar aSrc = div255(pipe->aInput * pipe->shape);
  uchar aResult = aSrc + aDest - div255(aSrc * aDest);
  uchar alpha2 = aResult;

  uchar cResult0 = 0;
  uchar cResult1 = 0;
  uchar cResult2 = 0;
  if (alpha2 != 0)  
  {
    cResult0 = state->rgbTransferR[(uchar)(((alpha2 - aSrc) * cDest[0] + aSrc * pipe->cSrc[0]) / alpha2)];
    cResult1 = state->rgbTransferG[(uchar)(((alpha2 - aSrc) * cDest[1] + aSrc * pipe->cSrc[1]) / alpha2)];
    cResult2 = state->rgbTransferB[(uchar)(((alpha2 - aSrc) * cDest[2] + aSrc * pipe->cSrc[2]) / alpha2)];
  }

  *pipe->destColorPtr++ = cResult2;
  *pipe->destColorPtr++ = cResult1;
  *pipe->destColorPtr++ = cResult0;
  *pipe->destAlphaPtr++ = aResult;

  ++pipe->x;
}

void XWRaster::pipeRunAACMYK8(RasterPipe *pipe)
{
	uchar cDest[4];
	cDest[0] = pipe->destColorPtr[0];
  cDest[1] = pipe->destColorPtr[1];
  cDest[2] = pipe->destColorPtr[2];
  cDest[3] = pipe->destColorPtr[3];
  uchar aDest = *pipe->destAlphaPtr;
  uchar aSrc = div255(pipe->aInput * pipe->shape);
  uchar aResult = aSrc + aDest - div255(aSrc * aDest);
  uchar alpha2 = aResult;
  uchar cResult0 = 0;
  uchar cResult1 = 0;
  uchar cResult2 = 0;
  uchar cResult3 = 0;
  if (alpha2 != 0)
  {
  	cResult0 = state->cmykTransferC[(uchar)(((alpha2 - aSrc) * cDest[0] + aSrc * pipe->cSrc[0]) / alpha2)];
    cResult1 = state->cmykTransferM[(uchar)(((alpha2 - aSrc) * cDest[1] + aSrc * pipe->cSrc[1]) / alpha2)];
    cResult2 = state->cmykTransferY[(uchar)(((alpha2 - aSrc) * cDest[2] + aSrc * pipe->cSrc[2]) / alpha2)];
    cResult3 = state->cmykTransferK[(uchar)(((alpha2 - aSrc) * cDest[3] + aSrc * pipe->cSrc[3]) / alpha2)];
  }
  
  if (state->overprintMask & 1) 
    pipe->destColorPtr[0] = cResult0;
  if (state->overprintMask & 2) 
    pipe->destColorPtr[1] = cResult1;
  if (state->overprintMask & 4) 
    pipe->destColorPtr[2] = cResult2;
  if (state->overprintMask & 8) 
    pipe->destColorPtr[3] = cResult3;
  pipe->destColorPtr += 4;
  *pipe->destAlphaPtr++ = aResult;

  ++pipe->x;
}

inline void XWRaster::pipeSetXY(RasterPipe *pipe, int x, int y)
{
  pipe->x = x;
  pipe->y = y;
  if (state->softMask) {
    pipe->softMaskPtr =
        &state->softMask->data[y * state->softMask->rowSize + x];
  }
  switch (bitmap->mode) {
  case COLOR_MODE_MONO1:
    pipe->destColorPtr = &bitmap->data[y * bitmap->rowSize + (x >> 3)];
    pipe->destColorMask = 0x80 >> (x & 7);
    break;
  case COLOR_MODE_MONO8:
    pipe->destColorPtr = &bitmap->data[y * bitmap->rowSize + x];
    break;
  case COLOR_MODE_RGB8:
  case COLOR_MODE_BGR8:
    pipe->destColorPtr = &bitmap->data[y * bitmap->rowSize + 3 * x];
    break;
  case COLOR_MODE_CMYK8:
    pipe->destColorPtr = &bitmap->data[y * bitmap->rowSize + 4 * x];
    break;
  }
  if (bitmap->alpha) {
    pipe->destAlphaPtr = &bitmap->alpha[y * bitmap->width + x];
  } else {
    pipe->destAlphaPtr = NULL;
  }
  if (state->inNonIsolatedGroup && alpha0Bitmap->alpha) {
    pipe->alpha0Ptr = &alpha0Bitmap->alpha[(alpha0Y + y) * alpha0Bitmap->width + (alpha0X + x)];
  } else {
    pipe->alpha0Ptr = NULL;
  }
}

XWBitmap * XWRaster::scaleImage(XWRasterImageSource src, void *srcData,
			   												int srcMode, int nComps,
			   												bool srcAlpha, int srcWidth, int srcHeight,
			   												int scaledWidth, int scaledHeight)
{
	XWBitmap * dest = new XWBitmap(scaledWidth, scaledHeight, 1, srcMode, srcAlpha);
  if (scaledHeight < srcHeight) 
  {
    if (scaledWidth < srcWidth) 
      scaleImageYdXd(src, srcData, srcMode, nComps, srcAlpha, srcWidth, srcHeight, scaledWidth, scaledHeight, dest);
   	else 
      scaleImageYdXu(src, srcData, srcMode, nComps, srcAlpha, srcWidth, srcHeight, scaledWidth, scaledHeight, dest);
  } 
  else 
  {
    if (scaledWidth < srcWidth) 
      scaleImageYuXd(src, srcData, srcMode, nComps, srcAlpha, srcWidth, srcHeight, scaledWidth, scaledHeight, dest);
    else 
      scaleImageYuXu(src, srcData, srcMode, nComps, srcAlpha, srcWidth, srcHeight, scaledWidth, scaledHeight, dest);
  }
  return dest;
}

void XWRaster::scaleImageYdXd(XWRasterImageSource src, void *srcData,
		      										int srcMode, int nComps,
		      										bool srcAlpha, int srcWidth, int srcHeight,
		      										int scaledWidth, int scaledHeight,
		      										XWBitmap *dest)
{
	uchar *lineBuf, *alphaLineBuf;
  uint *pixBuf, *alphaPixBuf;
  uint pix0, pix1, pix2;
  uint pix3;
  uint alpha;
  uchar *destPtr, *destAlphaPtr;
  int yp, yq, xp, xq, yt, y, yStep, xt, x, xStep, xx, xxa, d, d0, d1;
  int i, j;
  
  yp = srcHeight / scaledHeight;
  yq = srcHeight % scaledHeight;
  xp = srcWidth / scaledWidth;
  xq = srcWidth % scaledWidth;
  lineBuf = (uchar *)malloc(srcWidth * nComps * sizeof(uchar));
  pixBuf = (uint *)malloc(srcWidth * nComps * sizeof(uint));
  if (srcAlpha) 
  {
    alphaLineBuf = (uchar *)malloc(srcWidth * sizeof(uchar));
    alphaPixBuf = (uint *)malloc(srcWidth * sizeof(uint));
  } 
  else 
  {
    alphaLineBuf = 0;
    alphaPixBuf = 0;
  }
  
  yt = 0;
  destPtr = dest->data;
  destAlphaPtr = dest->alpha;
  for (y = 0; y < scaledHeight; ++y)
  {
  	if ((yt += yq) >= scaledHeight) 
  	{
      yt -= scaledHeight;
      yStep = yp + 1;
    } 
    else 
      yStep = yp;
      
    memset(pixBuf, 0, srcWidth * nComps * sizeof(uint));
    if (srcAlpha) 
      memset(alphaPixBuf, 0, srcWidth * sizeof(uint));
      
    for (i = 0; i < yStep; ++i) 
   	{
      (*src)(srcData, lineBuf, alphaLineBuf);
      for (j = 0; j < srcWidth * nComps; ++j) 
				pixBuf[j] += lineBuf[j];
				
      if (srcAlpha) 
      {
				for (j = 0; j < srcWidth; ++j) 
	  			alphaPixBuf[j] += alphaLineBuf[j];
      }
    }
    
    xt = 0;
    d0 = (1 << 23) / (yStep * xp);
    d1 = (1 << 23) / (yStep * (xp + 1));
    xx = xxa = 0;
    for (x = 0; x < scaledWidth; ++x)
    {
    	if ((xt += xq) >= scaledWidth) 
    	{
				xt -= scaledWidth;
				xStep = xp + 1;
				d = d1;
      } 
      else 
      {
				xStep = xp;
				d = d0;
      }
      
      switch (srcMode)
      {
      	case COLOR_MODE_MONO8:
					pix0 = 0;
					for (i = 0; i < xStep; ++i) 
	  				pix0 += pixBuf[xx++];
					pix0 = (pix0 * d) >> 23;
					*destPtr++ = (uchar)pix0;
					break;
					
				case COLOR_MODE_RGB8:
					pix0 = pix1 = pix2 = 0;
					for (i = 0; i < xStep; ++i) 
					{
	  				pix0 += pixBuf[xx];
	  				pix1 += pixBuf[xx+1];
	  				pix2 += pixBuf[xx+2];
	  				xx += 3;
					}
					pix0 = (pix0 * d) >> 23;
					pix1 = (pix1 * d) >> 23;
					pix2 = (pix2 * d) >> 23;
					*destPtr++ = (uchar)pix0;
					*destPtr++ = (uchar)pix1;
					*destPtr++ = (uchar)pix2;
					break;
					
				case COLOR_MODE_BGR8:
					pix0 = pix1 = pix2 = 0;
					for (i = 0; i < xStep; ++i) 
					{
	  				pix0 += pixBuf[xx];
	  				pix1 += pixBuf[xx+1];
	  				pix2 += pixBuf[xx+2];
	  				xx += 3;
					}
					pix0 = (pix0 * d) >> 23;
					pix1 = (pix1 * d) >> 23;
					pix2 = (pix2 * d) >> 23;
					*destPtr++ = (uchar)pix2;
					*destPtr++ = (uchar)pix1;
					*destPtr++ = (uchar)pix0;
					break;
					
				case COLOR_MODE_CMYK8:
					pix0 = pix1 = pix2 = pix3 = 0;
					for (i = 0; i < xStep; ++i) 
					{
	  				pix0 += pixBuf[xx];
	  				pix1 += pixBuf[xx+1];
	  				pix2 += pixBuf[xx+2];
	  				pix3 += pixBuf[xx+3];
	  				xx += 4;
					}
					pix0 = (pix0 * d) >> 23;
					pix1 = (pix1 * d) >> 23;
					pix2 = (pix2 * d) >> 23;
					pix3 = (pix3 * d) >> 23;
					*destPtr++ = (uchar)pix0;
					*destPtr++ = (uchar)pix1;
					*destPtr++ = (uchar)pix2;
					*destPtr++ = (uchar)pix3;
					break;
					
				case COLOR_MODE_MONO1:
      	default:
					break;
      }
      
      if (srcAlpha) 
      {
				alpha = 0;
				for (i = 0; i < xStep; ++i, ++xxa) 
	  			alpha += alphaPixBuf[xxa];
				alpha = (alpha * d) >> 23;
				*destAlphaPtr++ = (uchar)alpha;
      }
    }
  }
  
  free(alphaPixBuf);
  free(alphaLineBuf);
  free(pixBuf);
  free(lineBuf);
}

void XWRaster::scaleImageYdXu(XWRasterImageSource src, void *srcData,
		      										int srcMode, int nComps,
		      										bool srcAlpha, int srcWidth, int srcHeight,
		      										int scaledWidth, int scaledHeight,
		      										XWBitmap *dest)
{
	uchar *lineBuf, *alphaLineBuf;
  uint *pixBuf, *alphaPixBuf;
  uint pix[MAX_COLOR_COMPS];
  uint alpha;
  uchar *destPtr, *destAlphaPtr;
  int yp, yq, xp, xq, yt, y, yStep, xt, x, xStep, d;
  int i, j;
  
  yp = srcHeight / scaledHeight;
  yq = srcHeight % scaledHeight;
  xp = scaledWidth / srcWidth;
  xq = scaledWidth % srcWidth;
  lineBuf = (uchar *)malloc(srcWidth * nComps * sizeof(uchar));
  pixBuf = (uint *)malloc(srcWidth * nComps * sizeof(uint));
  if (srcAlpha) {
    alphaLineBuf = (uchar *)malloc(srcWidth * sizeof(uchar));
    alphaPixBuf = (uint *)malloc(srcWidth * sizeof(uint));
  } 
  else 
  {
    alphaLineBuf = 0;
    alphaPixBuf = 0;
  }
  
  yt = 0;
  destPtr = dest->data;
  destAlphaPtr = dest->alpha;
  for (y = 0; y < scaledHeight; ++y)
  {
  	if ((yt += yq) >= scaledHeight) 
  	{
      yt -= scaledHeight;
      yStep = yp + 1;
    } 
    else 
      yStep = yp;
      
    memset(pixBuf, 0, srcWidth * nComps * sizeof(uint));
    if (srcAlpha) 
      memset(alphaPixBuf, 0, srcWidth * sizeof(uint));
    for (i = 0; i < yStep; ++i) 
    {
      (*src)(srcData, lineBuf, alphaLineBuf);
      for (j = 0; j < srcWidth * nComps; ++j) 
				pixBuf[j] += lineBuf[j];
      if (srcAlpha) 
      {
				for (j = 0; j < srcWidth; ++j) 
	  			alphaPixBuf[j] += alphaLineBuf[j];
      }
    }
    
    xt = 0;
    d = (1 << 23) / yStep;
    for (x = 0; x < srcWidth; ++x)
    {
    	if ((xt += xq) >= srcWidth) 
    	{
				xt -= srcWidth;
				xStep = xp + 1;
      } 
      else 
				xStep = xp;
    }
    
    for (i = 0; i < nComps; ++i) 
			pix[i] = (pixBuf[x * nComps + i] * d) >> 23;
			
		switch (srcMode)
		{
			case COLOR_MODE_MONO1:
				break;
				
			case COLOR_MODE_MONO8:
				for (i = 0; i < xStep; ++i) 
	  			*destPtr++ = (uchar)pix[0];
				break;
				
      case COLOR_MODE_RGB8:
				for (i = 0; i < xStep; ++i) 
				{
	  			*destPtr++ = (uchar)pix[0];
	  			*destPtr++ = (uchar)pix[1];
	  			*destPtr++ = (uchar)pix[2];
				}
				break;
				
      case COLOR_MODE_BGR8:
				for (i = 0; i < xStep; ++i) 
				{
	  			*destPtr++ = (uchar)pix[2];
	  			*destPtr++ = (uchar)pix[1];
	  			*destPtr++ = (uchar)pix[0];
				}
				break;
				
			case COLOR_MODE_CMYK8:
				for (i = 0; i < xStep; ++i) 
				{
	  			*destPtr++ = (uchar)pix[0];
	  			*destPtr++ = (uchar)pix[1];
	  			*destPtr++ = (uchar)pix[2];
	  			*destPtr++ = (uchar)pix[3];
				}
				break;
		}
		
		if (srcAlpha) 
		{
			// alphaPixBuf[] / yStep
			alpha = (alphaPixBuf[x] * d) >> 23;
			for (i = 0; i < xStep; ++i) 
	  		*destAlphaPtr++ = (uchar)alpha;
    }
  }
  
  free(alphaPixBuf);
  free(alphaLineBuf);
  free(pixBuf);
  free(lineBuf);
}

void XWRaster::scaleImageYuXd(XWRasterImageSource src, void *srcData,
		      										int srcMode, int nComps,
		      										bool srcAlpha, int srcWidth, int srcHeight,
		      										int scaledWidth, int scaledHeight,
		      										XWBitmap *dest)
{
	uchar *lineBuf, *alphaLineBuf;
  uint pix[MAX_COLOR_COMPS];
  uint alpha;
  uchar *destPtr0, *destPtr, *destAlphaPtr0, *destAlphaPtr;
  int yp, yq, xp, xq, yt, y, yStep, xt, x, xStep, xx, xxa, d, d0, d1;
  int i, j;
  
  yp = scaledHeight / srcHeight;
  yq = scaledHeight % srcHeight;
  xp = srcWidth / scaledWidth;
  xq = srcWidth % scaledWidth;
  lineBuf = (uchar *)malloc(srcWidth * nComps * sizeof(uchar));
  if (srcAlpha) 
    alphaLineBuf = (uchar *)malloc(srcWidth * sizeof(uchar));
  else 
    alphaLineBuf = 0;
    
  yt = 0;
  destPtr0 = dest->data;
  destAlphaPtr0 = dest->alpha;
  for (y = 0; y < srcHeight; ++y)
  {
  	if ((yt += yq) >= srcHeight) 
  	{
      yt -= srcHeight;
      yStep = yp + 1;
    } 
    else 
      yStep = yp;
      
    (*src)(srcData, lineBuf, alphaLineBuf);
    xt = 0;
    d0 = (1 << 23) / xp;
    d1 = (1 << 23) / (xp + 1);

    xx = xxa = 0;
    for (x = 0; x < scaledWidth; ++x)
    {
    	if ((xt += xq) >= scaledWidth) 
    	{
				xt -= scaledWidth;
				xStep = xp + 1;
				d = d1;
      } 
      else 
      {
				xStep = xp;
				d = d0;
      }
      
      for (i = 0; i < nComps; ++i)
      	pix[i] = 0;
      	
      for (i = 0; i < xStep; ++i) 
      {
				for (j = 0; j < nComps; ++j, ++xx) 
	  			pix[j] += lineBuf[xx];
      }
      
      for (i = 0; i < nComps; ++i) 
				pix[i] = (pix[i] * d) >> 23;
				
			switch (srcMode)
			{
				case COLOR_MODE_MONO1:
					break;
					
				case COLOR_MODE_MONO8:
					for (i = 0; i < yStep; ++i) 
					{
	  				destPtr = destPtr0 + (i * scaledWidth + x) * nComps;
	  				*destPtr++ = (uchar)pix[0];
					}
					break;
					
				case COLOR_MODE_RGB8:
					for (i = 0; i < yStep; ++i) 
					{
	  				destPtr = destPtr0 + (i * scaledWidth + x) * nComps;
	  				*destPtr++ = (uchar)pix[0];
	  				*destPtr++ = (uchar)pix[1];
	  				*destPtr++ = (uchar)pix[2];
					}
					break;
					
				case COLOR_MODE_BGR8:
					for (i = 0; i < yStep; ++i) 
					{
	  				destPtr = destPtr0 + (i * scaledWidth + x) * nComps;
	  				*destPtr++ = (uchar)pix[2];
	  				*destPtr++ = (uchar)pix[1];
	  				*destPtr++ = (uchar)pix[0];
					}
					break;
					
				case COLOR_MODE_CMYK8:
					for (i = 0; i < yStep; ++i) 
					{
	  				destPtr = destPtr0 + (i * scaledWidth + x) * nComps;
	  				*destPtr++ = (uchar)pix[0];
	  				*destPtr++ = (uchar)pix[1];
	  				*destPtr++ = (uchar)pix[2];
	  				*destPtr++ = (uchar)pix[3];
					}
					break;
			}
			
			if (srcAlpha) 
			{
				alpha = 0;
				for (i = 0; i < xStep; ++i, ++xxa) 
	  			alpha += alphaLineBuf[xxa];
	  			
				alpha = (alpha * d) >> 23;
				for (i = 0; i < yStep; ++i) 
				{
	  			destAlphaPtr = destAlphaPtr0 + i * scaledWidth + x;
	  			*destAlphaPtr = (uchar)alpha;
				}
      }
    }
    
    destPtr0 += yStep * scaledWidth * nComps;
    if (srcAlpha) 
      destAlphaPtr0 += yStep * scaledWidth;
  }
  
  free(alphaLineBuf);
  free(lineBuf);
}

void XWRaster::scaleImageYuXu(XWRasterImageSource src, void *srcData,
		      										int srcMode, int nComps,
		      										bool srcAlpha, int srcWidth, int srcHeight,
		      										int scaledWidth, int scaledHeight,
		      										XWBitmap *dest)
{
	uchar *lineBuf, *alphaLineBuf;
  uint pix[MAX_COLOR_COMPS];
  uint alpha;
  uchar *destPtr0, *destPtr, *destAlphaPtr0, *destAlphaPtr;
  int yp, yq, xp, xq, yt, y, yStep, xt, x, xStep, xx;
  int i, j;
  
  yp = scaledHeight / srcHeight;
  yq = scaledHeight % srcHeight;
  xp = scaledWidth / srcWidth;
  xq = scaledWidth % srcWidth;
  lineBuf = (uchar *)malloc(srcWidth * nComps * sizeof(uchar));
  if (srcAlpha) 
    alphaLineBuf = (uchar *)malloc(srcWidth * sizeof(uchar));
  else 
    alphaLineBuf = 0;
    
  yt = 0;

  destPtr0 = dest->data;
  destAlphaPtr0 = dest->alpha;
  for (y = 0; y < srcHeight; ++y)
  {
  	if ((yt += yq) >= srcHeight) 
  	{
      yt -= srcHeight;
      yStep = yp + 1;
    } 
    else 
      yStep = yp;
      
    (*src)(srcData, lineBuf, alphaLineBuf);
    xt = 0;
    xx = 0;
    for (x = 0; x < srcWidth; ++x)
    {
    	if ((xt += xq) >= srcWidth) 
    	{
				xt -= srcWidth;
				xStep = xp + 1;
      } 
      else 
				xStep = xp;
				
			for (i = 0; i < nComps; ++i) 
				pix[i] = lineBuf[x * nComps + i];
				
			switch (srcMode)
			{
				case COLOR_MODE_MONO1:
					break;
					
				case COLOR_MODE_MONO8:
					for (i = 0; i < yStep; ++i) 
					{
	  				for (j = 0; j < xStep; ++j) 
	  				{
	    				destPtr = destPtr0 + (i * scaledWidth + xx + j) * nComps;
	    				*destPtr++ = (uchar)pix[0];
	  				}
					}
					break;
					
				case COLOR_MODE_RGB8:
					for (i = 0; i < yStep; ++i) 
					{
	  				for (j = 0; j < xStep; ++j) 
	  				{
	    				destPtr = destPtr0 + (i * scaledWidth + xx + j) * nComps;
	    				*destPtr++ = (uchar)pix[0];
	    				*destPtr++ = (uchar)pix[1];
	    				*destPtr++ = (uchar)pix[2];
	  				}
					}
					break;
					
				case COLOR_MODE_BGR8:
					for (i = 0; i < yStep; ++i) 
					{
	  				for (j = 0; j < xStep; ++j) 
	  				{
	    				destPtr = destPtr0 + (i * scaledWidth + xx + j) * nComps;
	    				*destPtr++ = (uchar)pix[2];
	    				*destPtr++ = (uchar)pix[1];
	    				*destPtr++ = (uchar)pix[0];
	  				}
					}
					break;
					
				case COLOR_MODE_CMYK8:
					for (i = 0; i < yStep; ++i) 
					{
	  				for (j = 0; j < xStep; ++j) 
	  				{
	    				destPtr = destPtr0 + (i * scaledWidth + xx + j) * nComps;
	    				*destPtr++ = (uchar)pix[0];
	    				*destPtr++ = (uchar)pix[1];
	    				*destPtr++ = (uchar)pix[2];
	    				*destPtr++ = (uchar)pix[3];
	  				}
					}
					break;
			}
			
			if (srcAlpha) 
			{
				alpha = alphaLineBuf[x];
				for (i = 0; i < yStep; ++i) 
				{
	  			for (j = 0; j < xStep; ++j) 
	  			{
	    			destAlphaPtr = destAlphaPtr0 + i * scaledWidth + xx + j;
	    			*destAlphaPtr = (uchar)alpha;
	  			}
				}
      }

      xx += xStep;
    }
    
    destPtr0 += yStep * scaledWidth * nComps;
    if (srcAlpha) 
      destAlphaPtr0 += yStep * scaledWidth;
  }
  
  free(alphaLineBuf);
  free(lineBuf);
}

XWBitmap * XWRaster::scaleMask(XWRasterImageMaskSource src, void *srcData,
			  								       int srcWidth, int srcHeight,
			  								       int scaledWidth, int scaledHeight)
{
	XWBitmap * dest = new XWBitmap(scaledWidth, scaledHeight, 1, COLOR_MODE_MONO8, false);
  if (scaledHeight < srcHeight) 
  {
    if (scaledWidth < srcWidth) 
      scaleMaskYdXd(src, srcData, srcWidth, srcHeight, scaledWidth, scaledHeight, dest);
    else 
      scaleMaskYdXu(src, srcData, srcWidth, srcHeight, scaledWidth, scaledHeight, dest);
  } 
  else 
  {
    if (scaledWidth < srcWidth) 
      scaleMaskYuXd(src, srcData, srcWidth, srcHeight, scaledWidth, scaledHeight, dest);
    else 
      scaleMaskYuXu(src, srcData, srcWidth, srcHeight, scaledWidth, scaledHeight, dest);
  }
  return dest;
}

void XWRaster::scaleMaskYdXd(XWRasterImageMaskSource src, void *srcData,
		     								     int srcWidth, int srcHeight,
		     								     int scaledWidth, int scaledHeight,
		     								     XWBitmap *dest)
{
	uchar *lineBuf;
  uint *pixBuf;
  uint pix;
  uchar *destPtr;
  int yp, yq, xp, xq, yt, y, yStep, xt, x, xStep, xx, d, d0, d1;
  int i, j;
  
  yp = srcHeight / scaledHeight;
  yq = srcHeight % scaledHeight;
  
  xp = srcWidth / scaledWidth;
  xq = srcWidth % scaledWidth;
  
  lineBuf = (uchar *)malloc(srcWidth * sizeof(uchar));
  pixBuf = (uint *)malloc(srcWidth * sizeof(uint));
  yt = 0;
  destPtr = dest->data;
  for (y = 0; y < scaledHeight; ++y)
  {
  	if ((yt += yq) >= scaledHeight) 
  	{
      yt -= scaledHeight;
      yStep = yp + 1;
    } 
    else 
      yStep = yp;
      
    memset(pixBuf, 0, srcWidth * sizeof(uint));
    for (i = 0; i < yStep; ++i) 
    {
      (*src)(srcData, lineBuf);
      for (j = 0; j < srcWidth; ++j) 
				pixBuf[j] += lineBuf[j];
    }
    
    xt = 0;
    d0 = (255 << 23) / (yStep * xp);
    d1 = (255 << 23) / (yStep * (xp + 1));

    xx = 0;
    for (x = 0; x < scaledWidth; ++x)
    {
    	if ((xt += xq) >= scaledWidth) 
    	{
				xt -= scaledWidth;
				xStep = xp + 1;
				d = d1;
      } 
      else 
      {
				xStep = xp;
				d = d0;
      }
      
      pix = 0;
    	for (i = 0; i < xStep; ++i)
    		pix += pixBuf[xx++];
    		
    	pix = (pix * d) >> 23;
      *destPtr++ = (uchar)pix;
    }
  }
  
  free(pixBuf);
  free(lineBuf);
}

void XWRaster::scaleMaskYdXu(XWRasterImageMaskSource src, void *srcData,
		     							       int srcWidth, int srcHeight,
		     							       int scaledWidth, int scaledHeight,
		     							       XWBitmap *dest)
{
	uchar *lineBuf;
  uint *pixBuf;
  uint pix;
  uchar *destPtr;
  int yp, yq, xp, xq, yt, y, yStep, xt, x, xStep, d;
  int i, j;
  
  yp = srcHeight / scaledHeight;
  yq = srcHeight % scaledHeight;
  xp = scaledWidth / srcWidth;
  xq = scaledWidth % srcWidth;
  lineBuf = (uchar *)malloc(srcWidth * sizeof(uchar));
  pixBuf = (uint *)malloc(srcWidth * sizeof(uint));
  yt = 0;
  destPtr = dest->data;
  for (y = 0; y < scaledHeight; ++y)
  {
  	if ((yt += yq) >= scaledHeight) 
  	{
      yt -= scaledHeight;
      yStep = yp + 1;
    } 
    else 
      yStep = yp;
      
    memset(pixBuf, 0, srcWidth * sizeof(uint));
    for (i = 0; i < yStep; ++i) 
    {
      (*src)(srcData, lineBuf);
      for (j = 0; j < srcWidth; ++j) 
				pixBuf[j] += lineBuf[j];
    }
    
    xt = 0;
    d = (255 << 23) / yStep;
    for (x = 0; x < srcWidth; ++x)
    {
    	if ((xt += xq) >= srcWidth) 
    	{
				xt -= srcWidth;
				xStep = xp + 1;
      } 
      else 
				xStep = xp;

      pix = pixBuf[x];
      pix = (pix * d) >> 23;
      for (i = 0; i < xStep; ++i) 
				*destPtr++ = (uchar)pix;
    }
  }
  
  free(pixBuf);
  free(lineBuf);
}

void XWRaster::scaleMaskYuXd(XWRasterImageMaskSource src, void *srcData,
		     											int srcWidth, int srcHeight,
		     											int scaledWidth, int scaledHeight,
		     											XWBitmap *dest)
{
	uchar *lineBuf;
  uint pix;
  uchar *destPtr0, *destPtr;
  int yp, yq, xp, xq, yt, y, yStep, xt, x, xStep, xx, d, d0, d1;
  int i;
  
  yp = scaledHeight / srcHeight;
  yq = scaledHeight % srcHeight;
  xp = srcWidth / scaledWidth;
  xq = srcWidth % scaledWidth;
  lineBuf = (uchar *)malloc(srcWidth * sizeof(uchar));
  yt = 0;
  destPtr0 = dest->data;
  for (y = 0; y < srcHeight; ++y)
  {
  	if ((yt += yq) >= srcHeight) 
  	{
      yt -= srcHeight;
      yStep = yp + 1;
    } 
    else 
      yStep = yp;
      
    (*src)(srcData, lineBuf);
    xt = 0;
    d0 = (255 << 23) / xp;
    d1 = (255 << 23) / (xp + 1);
    xx = 0;
    for (x = 0; x < scaledWidth; ++x)
    {
    	if ((xt += xq) >= scaledWidth) 
    	{
				xt -= scaledWidth;
				xStep = xp + 1;
				d = d1;
      } 
      else 
      {
				xStep = xp;
				d = d0;
      }
      
      pix = 0;
      for (i = 0; i < xStep; ++i) 
				pix += lineBuf[xx++];
				
			pix = (pix * d) >> 23;
			
			for (i = 0; i < yStep; ++i) 
			{
				destPtr = destPtr0 + i * scaledWidth + x;
				*destPtr = (uchar)pix;
      }
    }
    
    destPtr0 += yStep * scaledWidth;
  }
  
  free(lineBuf);
}

void XWRaster::scaleMaskYuXu(XWRasterImageMaskSource src, void *srcData,
		     										int srcWidth, int srcHeight,
		     										int scaledWidth, int scaledHeight,
		     										XWBitmap *dest)
{
	uchar *lineBuf;
  uint pix;
  uchar *destPtr0, *destPtr;
  int yp, yq, xp, xq, yt, y, yStep, xt, x, xStep, xx;
  int i, j;
  yp = scaledHeight / srcHeight;
  yq = scaledHeight % srcHeight;
  xp = scaledWidth / srcWidth;
  xq = scaledWidth % srcWidth;
  lineBuf = (uchar *)malloc(srcWidth * sizeof(uchar));
  yt = 0;
  destPtr0 = dest->data;
  for (y = 0; y < srcHeight; ++y)
  {
  	if ((yt += yq) >= srcHeight) 
  	{
      yt -= srcHeight;
      yStep = yp + 1;
    } 
    else 
      yStep = yp;
      
    (*src)(srcData, lineBuf);
    xt = 0;
    xx = 0;
    for (x = 0; x < srcWidth; ++x) 
    {
    	if ((xt += xq) >= srcWidth) 
    	{
				xt -= srcWidth;
				xStep = xp + 1;
      } 
      else 
				xStep = xp;

      pix = lineBuf[x] ? 255 : 0;
      for (i = 0; i < yStep; ++i) 
      {
				for (j = 0; j < xStep; ++j) 
				{
	  			destPtr = destPtr0 + i * scaledWidth + xx + j;
	  			*destPtr++ = (uchar)pix;
				}
      }

      xx += xStep;
    }
    
    destPtr0 += yStep * scaledWidth;
  }
  
  free(lineBuf);
}

void XWRaster::strokeNarrow(XWRasterPath *path)
{
  RasterPipe pipe;
  XWRasterXPath *xPath;
  XPathSeg *seg;
  int x0, x1, y0, y1, xa, xb, y;
  double dxdy;
  int clipRes;
  int nClipRes[3];
  int i;

  nClipRes[0] = nClipRes[1] = nClipRes[2] = 0;

  xPath = new XWRasterXPath(path, state->matrix, state->flatness, false);

  pipeInit(&pipe, 0, 0, state->strokePattern, NULL, (uchar)rasterRound(state->strokeAlpha * 255), false, false);

  clipRes = 0;
  for (i = 0, seg = xPath->segs; i < xPath->length; ++i, ++seg) {
    if (seg->y0 <= seg->y1) {
      y0 = rasterFloor(seg->y0);
      y1 = rasterFloor(seg->y1);
      x0 = rasterFloor(seg->x0);
      x1 = rasterFloor(seg->x1);
    } else {
      y0 = rasterFloor(seg->y1);
      y1 = rasterFloor(seg->y0);
      x0 = rasterFloor(seg->x1);
      x1 = rasterFloor(seg->x0);
    }
        
    if ((clipRes = state->clip->testRect(x0 <= x1 ? x0 : x1, y0,
					 x0 <= x1 ? x1 : x0, y1)) != CLIP_ALLOUTSIDE) {
      if (y0 == y1) {
	if (x0 <= x1) {
	  drawSpan(&pipe, x0, x1, y0, clipRes == CLIP_ALLINSIDE);
	} else {
	  drawSpan(&pipe, x1, x0, y0, clipRes == CLIP_ALLINSIDE);
	}
      } else {
	dxdy = seg->dxdy;
	if (y0 < state->clip->getYMinI()) {
	  y0 = state->clip->getYMinI();
	  x0 = rasterFloor(seg->x0 + ((double)y0 - seg->y0) * dxdy);
	}
	if (y1 > state->clip->getYMaxI()) {
	  y1 = state->clip->getYMaxI();
	  x1 = rasterFloor(seg->x0 + ((double)y1 - seg->y0) * dxdy);
	}
	if (x0 <= x1) {
	  xa = x0;
	  for (y = y0; y <= y1; ++y) {
	    if (y < y1) {
	      xb = rasterFloor(seg->x0 +
			       ((double)y + 1 - seg->y0) * dxdy);
	    } else {
	      xb = x1 + 1;
	    }
	    if (xa == xb) {
	      drawPixel(&pipe, xa, y, clipRes == CLIP_ALLINSIDE);
	    } else {
	      drawSpan(&pipe, xa, xb - 1, y, clipRes == CLIP_ALLINSIDE);
	    }
	    xa = xb;
	  }
	} else {
	  xa = x0;
	  for (y = y0; y <= y1; ++y) {
	    if (y < y1) {
	      xb = rasterFloor(seg->x0 +
			       ((double)y + 1 - seg->y0) * dxdy);
	    } else {
	      xb = x1 - 1;
	    }
	    if (xa == xb) {
	      drawPixel(&pipe, xa, y, clipRes == CLIP_ALLINSIDE);
	    } else {
	      drawSpan(&pipe, xb + 1, xa, y, clipRes == CLIP_ALLINSIDE);
	    }
	    xa = xb;
	  }
	}
      }
    }
    ++nClipRes[clipRes];
  }
    
  if (nClipRes[CLIP_PARTIAL] ||
      (nClipRes[CLIP_ALLINSIDE] && nClipRes[CLIP_ALLOUTSIDE])) {
    opClipRes = CLIP_PARTIAL;
  } else if (nClipRes[CLIP_ALLINSIDE]) {
    opClipRes = CLIP_ALLINSIDE;
  } else {
    opClipRes = CLIP_ALLOUTSIDE;
  }

  delete xPath;
}

void XWRaster::strokeWide(XWRasterPath *path, double w)
{
    XWRasterPath * path2 = makeStrokePath(path, w, false);
    fillWithPattern(path2, false, state->strokePattern, state->strokeAlpha);
    delete path2;
}

inline void XWRaster::updateModX(int x)
{
    if (x < modXMin) 
        modXMin = x;
        
    if (x > modXMax) 
        modXMax = x;
}

inline void XWRaster::updateModY(int y)
{
    if (y < modYMin) 
        modYMin = y;
        
    if (y > modYMax) 
        modYMax = y;
}

void XWRaster::vertFlipImage(XWBitmap *img, int width, int height, int nComps)
{
	uchar *lineBuf;
  uchar *p0, *p1;
  int w = width * nComps;
  lineBuf = (uchar *)malloc(w * sizeof(uchar));
  for (p0 = img->data, p1 = img->data + (height - 1) * w; p0 < p1; p0 += w, p1 -= w) 
  {
    memcpy(lineBuf, p0, w);
    memcpy(p0, p1, w);
    memcpy(p1, lineBuf, w);
  }
  
  if (img->alpha) 
  {
    for (p0 = img->alpha, p1 = img->alpha + (height - 1) * width; p0 < p1; p0 += width, p1 -= width) 
    {
      memcpy(lineBuf, p0, width);
      memcpy(p0, p1, width);
      memcpy(p1, lineBuf, width);
    }
  }
  free(lineBuf);
}

void XWRaster::dumpPath(XWRasterPath *path)
{
	int i;

  for (i = 0; i < path->length; ++i) {
    printf("  %3d: x=%8.2f y=%8.2f%s%s%s%s\n",
	   i, (double)path->pts[i].x, (double)path->pts[i].y,
	   (path->flags[i] & PATH_FIRST) ? " first" : "",
	   (path->flags[i] & PATH_LAST) ? " last" : "",
	   (path->flags[i] & PATH_CLOSED) ? " closed" : "",
	   (path->flags[i] & PATH_CURVE) ? " curve" : "");
  }
}
