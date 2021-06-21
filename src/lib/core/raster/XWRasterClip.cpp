/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "XWRasterType.h"
#include "XWRasterPath.h"
#include "XWRasterXPath.h"
#include "XWRasterXPathScanner.h"
#include "XWBitmap.h"
#include "XWRasterClip.h"

#define RASTER_CLIPEO       0x01

XWRasterClip::XWRasterClip(double x0, 
                           double y0,
	                       double x1, 
	                       double y1, 
	                       bool antialiasA)
{
  antialias = antialiasA;
  if (x0 < x1) {
    xMin = x0;
    xMax = x1;
  } else {
    xMin = x1;
    xMax = x0;
  }
  if (y0 < y1) {
    yMin = y0;
    yMax = y1;
  } else {
    yMin = y1;
    yMax = y0;
  }
  xMinI = rasterFloor(xMin);
  yMinI = rasterFloor(yMin);
  xMaxI = rasterCeil(xMax) - 1;
  yMaxI = rasterCeil(yMax) - 1;
  paths = NULL;
  flags = NULL;
  scanners = NULL;
  length = size = 0;
}

XWRasterClip::XWRasterClip(XWRasterClip * clip)
{
  int yMinAA, yMaxAA;
  int i;

  antialias = clip->antialias;
  xMin = clip->xMin;
  yMin = clip->yMin;
  xMax = clip->xMax;
  yMax = clip->yMax;
  xMinI = clip->xMinI;
  yMinI = clip->yMinI;
  xMaxI = clip->xMaxI;
  yMaxI = clip->yMaxI;
  length = clip->length;
  size = clip->size;
  paths = (XWRasterXPath **)malloc(size * sizeof(XWRasterXPath *));
  flags = (uchar *)malloc(size * sizeof(uchar));
  scanners = (XWRasterXPathScanner **)malloc(size * sizeof(XWRasterXPathScanner *));
  for (i = 0; i < length; ++i) {
    paths[i] = clip->paths[i]->copy();
    flags[i] = clip->flags[i];
    if (antialias) {
      yMinAA = yMinI * AA_SIZE;
      yMaxAA = (yMaxI + 1) * AA_SIZE - 1;
    } else {
      yMinAA = yMinI;
      yMaxAA = yMaxI;
    }
    scanners[i] = new XWRasterXPathScanner(paths[i], flags[i] & RASTER_CLIPEO, yMinAA, yMaxAA);
  }
}

XWRasterClip::~XWRasterClip()
{
  int i;

  for (i = 0; i < length; ++i) {
    delete paths[i];
    delete scanners[i];
  }
  if (paths)
  	free(paths);
  	
  if (flags)
  	free(flags);
  	
  if (scanners)
  	free(scanners);
}

void XWRasterClip::clipAALine(XWBitmap *aaBuf, int *x0, int *x1, int y)
{
  int xx0, xx1, xx, yy, i;
  uchar* p;

  // zero out pixels with x < xMin
  xx0 = *x0 * AA_SIZE;
  xx1 = rasterFloor(xMin * AA_SIZE);
  if (xx1 > aaBuf->getWidth()) {
    xx1 = aaBuf->getWidth();
  }
  if (xx0 < xx1) {
    xx0 &= ~7;
    for (yy = 0; yy < AA_SIZE; ++yy) {
      p = aaBuf->getDataPtr() + yy * aaBuf->getRowSize() + (xx0 >> 3);
      for (xx = xx0; xx + 7 < xx1; xx += 8) {
	*p++ = 0;
      }
      if (xx < xx1) {
	*p &= 0xff >> (xx1 & 7);
      }
    }
    *x0 = rasterFloor(xMin);
  }

  // zero out pixels with x > xMax
  xx0 = rasterFloor(xMax * AA_SIZE) + 1;
  if (xx0 < 0) {
    xx0 = 0;
  }
  xx1 = (*x1 + 1) * AA_SIZE;
  if (xx0 < xx1) {
    for (yy = 0; yy < AA_SIZE; ++yy) {
      p = aaBuf->getDataPtr() + yy * aaBuf->getRowSize() + (xx0 >> 3);
      xx = xx0;
      if (xx & 7) {
	*p &= 0xff00 >> (xx & 7);
	xx = (xx & ~7) + 8;
	++p;
      }
      for (; xx < xx1; xx += 8) {
	*p++ = 0;
      }
    }
    *x1 = rasterFloor(xMax);
  }

  // check the paths
  for (i = 0; i < length; ++i) {
    scanners[i]->clipAALine(aaBuf, x0, x1, y);
  }
}

bool XWRasterClip::clipToPath(XWRasterPath *path, 
                              double * matrix, 
                              double flatness, 
                              bool eo)
{
  XWRasterXPath *xPath;
  int yMinAA, yMaxAA;

  xPath = new XWRasterXPath(path, matrix, flatness, true);

  // check for an empty path
  if (xPath->length == 0) {
    xMax = xMin - 1;
    yMax = yMin - 1;
    xMaxI = rasterCeil(xMax) - 1;
    yMaxI = rasterCeil(yMax) - 1;
    delete xPath;

  // check for a rectangle
  } else if (xPath->length == 4 &&
	     ((xPath->segs[0].x0 == xPath->segs[0].x1 &&
	       xPath->segs[0].x0 == xPath->segs[1].x0 &&
	       xPath->segs[0].x0 == xPath->segs[3].x1 &&
	       xPath->segs[2].x0 == xPath->segs[2].x1 &&
	       xPath->segs[2].x0 == xPath->segs[1].x1 &&
	       xPath->segs[2].x0 == xPath->segs[3].x0 &&
	       xPath->segs[1].y0 == xPath->segs[1].y1 &&
	       xPath->segs[1].y0 == xPath->segs[0].y1 &&
	       xPath->segs[1].y0 == xPath->segs[2].y0 &&
	       xPath->segs[3].y0 == xPath->segs[3].y1 &&
	       xPath->segs[3].y0 == xPath->segs[0].y0 &&
	       xPath->segs[3].y0 == xPath->segs[2].y1) ||
	      (xPath->segs[0].y0 == xPath->segs[0].y1 &&
	       xPath->segs[0].y0 == xPath->segs[1].y0 &&
	       xPath->segs[0].y0 == xPath->segs[3].y1 &&
	       xPath->segs[2].y0 == xPath->segs[2].y1 &&
	       xPath->segs[2].y0 == xPath->segs[1].y1 &&
	       xPath->segs[2].y0 == xPath->segs[3].y0 &&
	       xPath->segs[1].x0 == xPath->segs[1].x1 &&
	       xPath->segs[1].x0 == xPath->segs[0].x1 &&
	       xPath->segs[1].x0 == xPath->segs[2].x0 &&
	       xPath->segs[3].x0 == xPath->segs[3].x1 &&
	       xPath->segs[3].x0 == xPath->segs[0].x0 &&
	       xPath->segs[3].x0 == xPath->segs[2].x1))) {
    clipToRect(xPath->segs[0].x0, xPath->segs[0].y0,
	       xPath->segs[2].x0, xPath->segs[2].y0);
    delete xPath;

  } else {
    grow(1);
    if (antialias) {
      xPath->aaScale();
    }
    xPath->sort();
    paths[length] = xPath;
    flags[length] = eo ? RASTER_CLIPEO : 0;
    if (antialias) {
      yMinAA = yMinI * AA_SIZE;
      yMaxAA = (yMaxI + 1) * AA_SIZE - 1;
    } else {
      yMinAA = yMinI;
      yMaxAA = yMaxI;
    }
    scanners[length] = new XWRasterXPathScanner(xPath, eo, yMinAA, yMaxAA);
    ++length;
  }

  return true;
}

bool XWRasterClip::clipToRect(double x0, double y0, double x1, double y1)
{
  if (x0 < x1) {
    if (x0 > xMin) {
      xMin = x0;
      xMinI = rasterFloor(xMin);
    }
    if (x1 < xMax) {
      xMax = x1;
      xMaxI = rasterCeil(xMax) - 1;
    }
  } else {
    if (x1 > xMin) {
      xMin = x1;
      xMinI = rasterFloor(xMin);
    }
    if (x0 < xMax) {
      xMax = x0;
      xMaxI = rasterCeil(xMax) - 1;
    }
  }
  if (y0 < y1) {
    if (y0 > yMin) {
      yMin = y0;
      yMinI = rasterFloor(yMin);
    }
    if (y1 < yMax) {
      yMax = y1;
      yMaxI = rasterCeil(yMax) - 1;
    }
  } else {
    if (y1 > yMin) {
      yMin = y1;
      yMinI = rasterFloor(yMin);
    }
    if (y0 < yMax) {
      yMax = y0;
      yMaxI = rasterCeil(yMax) - 1;
    }
  }
  return true;
}

void XWRasterClip::resetToRect(double x0, double y0, double x1, double y1)
{
  int i;

  for (i = 0; i < length; ++i) {
    delete paths[i];
    delete scanners[i];
  }
  
  if (paths)
  	free(paths);
  	
  if (flags)
  	free(flags);
  	
  if (scanners)
  	free(scanners);
  	
  paths = NULL;
  flags = NULL;
  scanners = NULL;
  length = size = 0;

  if (x0 < x1) {
    xMin = x0;
    xMax = x1;
  } else {
    xMin = x1;
    xMax = x0;
  }
  if (y0 < y1) {
    yMin = y0;
    yMax = y1;
  } else {
    yMin = y1;
    yMax = y0;
  }
  xMinI = rasterFloor(xMin);
  yMinI = rasterFloor(yMin);
  xMaxI = rasterCeil(xMax) - 1;
  yMaxI = rasterCeil(yMax) - 1;
}

bool XWRasterClip::test(int x, int y)
{
  int i;

  // check the rectangle
  if (x < xMinI || x > xMaxI || y < yMinI || y > yMaxI) {
    return false;
  }

  // check the paths
  if (antialias) {
    for (i = 0; i < length; ++i) {
      if (!scanners[i]->test(x * AA_SIZE, y * AA_SIZE)) {
	return false;
      }
    }
  } else {
    for (i = 0; i < length; ++i) {
      if (!scanners[i]->test(x, y)) {
	return false;
      }
    }
  }

  return true;
}

int  XWRasterClip::testRect(int rectXMin, int rectYMin, int rectXMax, int rectYMax)
{
  // This tests the rectangle:
  //     x = [rectXMin, rectXMax + 1)    (note: rect coords are ints)
  //     y = [rectYMin, rectYMax + 1)
  // against the clipping region:
  //     x = [xMin, xMax)                (note: clipping coords are fp)
  //     y = [yMin, yMax)
  if ((double)(rectXMax + 1) <= xMin || (double)rectXMin >= xMax ||
      (double)(rectYMax + 1) <= yMin || (double)rectYMin >= yMax) {
    return CLIP_ALLOUTSIDE;
  }
  if ((double)rectXMin >= xMin && (double)(rectXMax + 1) <= xMax &&
      (double)rectYMin >= yMin && (double)(rectYMax + 1) <= yMax &&
      length == 0) {
    return CLIP_ALLINSIDE;
  }
  return CLIP_PARTIAL;
}

int  XWRasterClip::testSpan(int spanXMin, int spanXMax, int spanY)
{
  int i;

  // This tests the rectangle:
  //     x = [spanXMin, spanXMax + 1)    (note: span coords are ints)
  //     y = [spanY, spanY + 1)
  // against the clipping region:
  //     x = [xMin, xMax)                (note: clipping coords are fp)
  //     y = [yMin, yMax)
  if ((double)(spanXMax + 1) <= xMin || (double)spanXMin >= xMax ||
      (double)(spanY + 1) <= yMin || (double)spanY >= yMax) {
    return CLIP_ALLOUTSIDE;
  }
  if (!((double)spanXMin >= xMin && (double)(spanXMax + 1) <= xMax &&
	(double)spanY >= yMin && (double)(spanY + 1) <= yMax)) {
    return CLIP_PARTIAL;
  }
  if (antialias) {
    for (i = 0; i < length; ++i) {
      if (!scanners[i]->testSpan(spanXMin * AA_SIZE,
				 spanXMax * AA_SIZE + (AA_SIZE - 1),
				 spanY * AA_SIZE)) {
	return CLIP_PARTIAL;
      }
    }
  } else {
    for (i = 0; i < length; ++i) {
      if (!scanners[i]->testSpan(spanXMin, spanXMax, spanY)) {
	return CLIP_PARTIAL;
      }
    }
  }
  return CLIP_ALLINSIDE;
}

void XWRasterClip::grow(int nPaths)
{
  if (length + nPaths > size) {
    if (size == 0) {
      size = 32;
    }
    while (size < length + nPaths) {
      size *= 2;
    }
    paths = (XWRasterXPath **)realloc(paths, size * sizeof(XWRasterXPath *));
    flags = (uchar *)realloc(flags, size * sizeof(uchar));
    scanners = (XWRasterXPathScanner **)realloc(scanners, size * sizeof(XWRasterXPathScanner *));
  }
}

