/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "XWRasterType.h"
#include "XWBitmap.h"
#include "XWRasterXPath.h"
#include "XWRasterXPathScanner.h"

static int cmpIntersect(const void *p0, const void *p1) 
{
  RasterIntersect *i0 = (RasterIntersect *)p0;
  RasterIntersect *i1 = (RasterIntersect *)p1;
  int cmp;

  if ((cmp = i0->y - i1->y) == 0) {
    cmp = i0->x0 - i1->x0;
  }
  return cmp;
}

XWRasterXPathScanner::XWRasterXPathScanner(XWRasterXPath * xPathA, bool eoA, int clipYMin, int clipYMax)
{
  XPathSeg *seg;
  double xMinFP, yMinFP, xMaxFP, yMaxFP;
  int i;

  xPath = xPathA;
  eo = eoA;
  partialClip = false;

  // compute the bbox
  if (xPath->length == 0) {
    xMin = yMin = 1;
    xMax = yMax = 0;
  } else {
    seg = &xPath->segs[0];
    if (seg->x0 <= seg->x1) {
      xMinFP = seg->x0;
      xMaxFP = seg->x1;
    } else {
      xMinFP = seg->x1;
      xMaxFP = seg->x0;
    }
    if (seg->flags & XPATH_FLIP) {
      yMinFP = seg->y1;
      yMaxFP = seg->y0;
    } else {
      yMinFP = seg->y0;
      yMaxFP = seg->y1;
    }
    for (i = 1; i < xPath->length; ++i) {
      seg = &xPath->segs[i];
      if (seg->x0 < xMinFP) {
	xMinFP = seg->x0;
      } else if (seg->x0 > xMaxFP) {
	xMaxFP = seg->x0;
      }
      if (seg->x1 < xMinFP) {
	xMinFP = seg->x1;
      } else if (seg->x1 > xMaxFP) {
	xMaxFP = seg->x1;
      }
      if (seg->flags & XPATH_FLIP) {
	if (seg->y0 > yMaxFP) {
	  yMaxFP = seg->y0;
	}
      } else {
	if (seg->y1 > yMaxFP) {
	  yMaxFP = seg->y1;
	}
      }
    }
    xMin = rasterFloor(xMinFP);
    xMax = rasterFloor(xMaxFP);
    yMin = rasterFloor(yMinFP);
    yMax = rasterFloor(yMaxFP);
    if (clipYMin > yMin) {
      yMin = clipYMin;
      partialClip = true;
    }
    if (clipYMax < yMax) {
      yMax = clipYMax;
      partialClip = true;
    }
  }

  allInter = NULL;
  inter = NULL;
  computeIntersections();
  interY = yMin - 1;
}

XWRasterXPathScanner::~XWRasterXPathScanner()
{
  if (inter)
    free(inter);
        
  if (allInter)
   	free(allInter);
}

void XWRasterXPathScanner::clipAALine(XWBitmap *aaBuf, int *x0, int *x1, int y)
{
  int xx0, xx1, xx, yy, interEnd;
  uchar mask;
  uchar * p;

  for (yy = 0; yy < AA_SIZE; ++yy) {
    xx = *x0 * AA_SIZE;
    if (yMin <= yMax) {
      if (AA_SIZE * y + yy < yMin) {
	interIdx = interEnd = inter[0];
      } else if (AA_SIZE * y + yy > yMax) {
	interIdx = interEnd = inter[yMax - yMin + 1];
      } else {
	interIdx = inter[AA_SIZE * y + yy - yMin];
	if (AA_SIZE * y + yy > yMax) {
	  interEnd = inter[yMax - yMin + 1];
	} else {
	  interEnd = inter[AA_SIZE * y + yy - yMin + 1];
	}
      }
      interCount = 0;
      while (interIdx < interEnd && xx < (*x1 + 1) * AA_SIZE) {
	xx0 = allInter[interIdx].x0;
	xx1 = allInter[interIdx].x1;
	interCount += allInter[interIdx].count;
	++interIdx;
	while (interIdx < interEnd &&
	       (allInter[interIdx].x0 <= xx1 ||
		(eo ? (interCount & 1) : (interCount != 0)))) {
	  if (allInter[interIdx].x1 > xx1) {
	    xx1 = allInter[interIdx].x1;
	  }
	  interCount += allInter[interIdx].count;
	  ++interIdx;
	}
	if (xx0 > aaBuf->getWidth()) {
	  xx0 = aaBuf->getWidth();
	}
	// set [xx, xx0) to 0
	if (xx < xx0) {
	  p = aaBuf->getDataPtr() + yy * aaBuf->getRowSize() + (xx >> 3);
	  if (xx & 7) {
	    mask = (uchar)(0xff00 >> (xx & 7));
	    if ((xx & ~7) == (xx0 & ~7)) {
	      mask |= 0xff >> (xx0 & 7);
	    }
	    *p++ &= mask;
	    xx = (xx & ~7) + 8;
	  }
	  for (; xx + 7 < xx0; xx += 8) {
	    *p++ = 0x00;
	  }
	  if (xx < xx0) {
	    *p &= 0xff >> (xx0 & 7);
	  }
	}
	if (xx1 >= xx) {
	  xx = xx1 + 1;
	}
      }
    }
    xx0 = (*x1 + 1) * AA_SIZE;
    // set [xx, xx0) to 0
    if (xx < xx0) {
      p = aaBuf->getDataPtr() + yy * aaBuf->getRowSize() + (xx >> 3);
      if (xx & 7) {
	mask = (uchar)(0xff00 >> (xx & 7));
	if ((xx & ~7) == (xx0 & ~7)) {
	  mask &= 0xff >> (xx0 & 7);
	}
	*p++ &= mask;
	xx = (xx & ~7) + 8;
      }
      for (; xx + 7 < xx0; xx += 8) {
	*p++ = 0x00;
      }
      if (xx < xx0) {
	*p &= 0xff >> (xx0 & 7);
      }
    }
  }
}

void XWRasterXPathScanner::getBBoxAA(int *xMinA, int *yMinA, int *xMaxA, int *yMaxA)
{
  *xMinA = xMin / AA_SIZE;
  *yMinA = yMin / AA_SIZE;
  *xMaxA = xMax / AA_SIZE;
  *yMaxA = yMax / AA_SIZE;
}

bool XWRasterXPathScanner::getNextSpan(int y, int *x0, int *x1)
{
    int interEnd, xx0, xx1;

  if (y < yMin || y > yMax) 
  {
    return false;
  }
  if (interY != y) 
  {
    interY = y;
    interIdx = inter[y - yMin];
    interCount = 0;
  }
  interEnd = inter[y - yMin + 1];
  if (interIdx >= interEnd) 
  {
    return false;
  }
  xx0 = allInter[interIdx].x0;
  xx1 = allInter[interIdx].x1;
  interCount += allInter[interIdx].count;
  ++interIdx;
  while (interIdx < interEnd &&
	 			(allInter[interIdx].x0 <= xx1 ||
	  		(eo ? (interCount & 1) : (interCount != 0)))) 
	{
    if (allInter[interIdx].x1 > xx1) 
    {
      xx1 = allInter[interIdx].x1;
    }
    interCount += allInter[interIdx].count;
    ++interIdx;
  }
  *x0 = xx0;
  *x1 = xx1;
  return true;
}

void XWRasterXPathScanner::getSpanBounds(int y, int *spanXMin, int *spanXMax)
{
	int interBegin, interEnd, xx, i;
  if (y < yMin || y > yMax) 
  {
    interBegin = interEnd = 0;
  } 
  else 
  {
    interBegin = inter[y - yMin];
    interEnd = inter[y - yMin + 1];
  }
  if (interBegin < interEnd) 
  {
    *spanXMin = allInter[interBegin].x0;
    xx = allInter[interBegin].x1;
    for (i = interBegin + 1; i < interEnd; ++i) 
    {
      if (allInter[i].x1 > xx) 
      {
				xx = allInter[i].x1;
      }
    }
    *spanXMax = xx;
  } 
  else 
  {
    *spanXMin = xMax + 1;
    *spanXMax = xMax;
  }
}

void XWRasterXPathScanner::renderAALine(XWBitmap *aaBuf, int *x0, int *x1, int y)
{
  int xx0, xx1, xx, xxMin, xxMax, yy, interEnd;
  uchar mask;
  uchar* p;

  memset(aaBuf->getDataPtr(), 0, aaBuf->getRowSize() * aaBuf->getHeight());
  xxMin = aaBuf->getWidth();
  xxMax = -1;
  if (yMin <= yMax) 
  {
    if (AA_SIZE * y < yMin) 
    {
      interIdx = inter[0];
    } 
    else if (AA_SIZE * y > yMax) 
    {
      interIdx = inter[yMax - yMin + 1];
    } 
    else 
    {
      interIdx = inter[AA_SIZE * y - yMin];
    }
    for (yy = 0; yy < AA_SIZE; ++yy) 
   	{
      if (AA_SIZE * y + yy < yMin) 
      {
				interEnd = inter[0];
      } 
      else if (AA_SIZE * y + yy > yMax) 
      {
				interEnd = inter[yMax - yMin + 1];
      } 
      else 
      {
				interEnd = inter[AA_SIZE * y + yy - yMin + 1];
      }
      interCount = 0;
      while (interIdx < interEnd) 
      {
				xx0 = allInter[interIdx].x0;
				xx1 = allInter[interIdx].x1;
				interCount += allInter[interIdx].count;
				++interIdx;
				while (interIdx < interEnd &&
	       				(allInter[interIdx].x0 <= xx1 ||
							(eo ? (interCount & 1) : (interCount != 0)))) 
				{
	  			if (allInter[interIdx].x1 > xx1) 
	  			{
	    			xx1 = allInter[interIdx].x1;
	  			}
	  			interCount += allInter[interIdx].count;
	  			++interIdx;
				}
				if (xx0 < 0) 
				{
	  			xx0 = 0;
				}
				++xx1;
				if (xx1 > aaBuf->getWidth()) 
				{
	  			xx1 = aaBuf->getWidth();
				}
				// set [xx0, xx1) to 1
				if (xx0 < xx1) 
				{
	  			xx = xx0;
	  			p = aaBuf->getDataPtr() + yy * aaBuf->getRowSize() + (xx >> 3);
	  			if (xx & 7) 
	  			{
	    			mask = 0xff >> (xx & 7);
	    			if ((xx & ~7) == (xx1 & ~7)) 
	    			{
	      			mask &= (uchar)(0xff00 >> (xx1 & 7));
	    			}
	   			 	*p++ |= mask;
	   			 	xx = (xx & ~7) + 8;
	  			}
	  			for (; xx + 7 < xx1; xx += 8) 
	  			{
	    			*p++ |= 0xff;
	  			}
	  			if (xx < xx1) 
	  			{
	    			*p |= (uchar)(0xff00 >> (xx1 & 7));
	  			}
				}
				if (xx0 < xxMin) 
				{
	  			xxMin = xx0;
				}
				if (xx1 > xxMax) 
				{
	  			xxMax = xx1;
				}
      }
    }
  }
  *x0 = xxMin / AA_SIZE;
  *x1 = (xxMax - 1) / AA_SIZE;
}

bool XWRasterXPathScanner::test(int x, int y)
{
  int interBegin, interEnd, count, i;

  if (y < yMin || y > yMax) 
  {
    return false;
  }
  interBegin = inter[y - yMin];
  interEnd = inter[y - yMin + 1];
  count = 0;
  for (i = interBegin; i < interEnd && allInter[i].x0 <= x; ++i) 
 	{
    if (x <= allInter[i].x1) 
    {
      return true;
    }
    count += allInter[i].count;
  }
  return eo ? (count & 1) : (count != 0);
}

bool XWRasterXPathScanner::testSpan(int x0, int x1, int y)
{
    int interBegin, interEnd, count, xx1, i;

  if (y < yMin || y > yMax) 
  {
    return false;
  }
  interBegin = inter[y - yMin];
  interEnd = inter[y - yMin + 1];
  count = 0;
  for (i = interBegin; i < interEnd && allInter[i].x1 < x0; ++i) 
 	{
    count += allInter[i].count;
  }

  xx1 = x0 - 1;
  while (xx1 < x1) 
  {
    if (i >= interEnd) 
    {
      return false;
    }
    if (allInter[i].x0 > xx1 + 1 && !(eo ? (count & 1) : (count != 0))) 
    {
      return false;
    }
    if (allInter[i].x1 > xx1) 
    {
      xx1 = allInter[i].x1;
    }
    count += allInter[i].count;
    ++i;
  }

  return true;
}

void XWRasterXPathScanner::addIntersection(double segYMin, double segYMax,
		       																	uint segFlags,
		       																	int y, int x0, int x1)
{
	if (allInterLen == allInterSize) 
	{
    allInterSize *= 2;
    allInter = (RasterIntersect *)realloc(allInter, allInterSize * sizeof(RasterIntersect));
  }
  allInter[allInterLen].y = y;
  if (x0 < x1) 
  {
    allInter[allInterLen].x0 = x0;
    allInter[allInterLen].x1 = x1;
  } 
  else 
  {
    allInter[allInterLen].x0 = x1;
    allInter[allInterLen].x1 = x0;
  }
  if (segYMin <= y &&
      (double)y < segYMax &&
      !(segFlags & XPATH_HORIZ)) 
 {
    allInter[allInterLen].count = eo ? 1 : (segFlags & XPATH_FLIP) ? 1 : -1;
  } 
  else 
  {
    allInter[allInterLen].count = 0;
  }
  ++allInterLen;
}

void XWRasterXPathScanner::computeIntersections()
{
  XPathSeg *seg;
  double segXMin, segXMax, segYMin, segYMax, xx0, xx1;
  int x, y, y0, y1, i;

  if (yMin > yMax) 
  {
    return;
  }
  
  allInterLen = 0;
  allInterSize = 16;
  allInter = (RasterIntersect *)malloc(allInterSize * sizeof(RasterIntersect));
  for (i = 0; i < xPath->length; ++i) 
  {
    seg = &xPath->segs[i];
    if (seg->flags & XPATH_FLIP) 
    {
      segYMin = seg->y1;
      segYMax = seg->y0;
    } 
    else 
    {
      segYMin = seg->y0;
      segYMax = seg->y1;
    }
    if (seg->flags & XPATH_HORIZ) 
    {
      y = rasterFloor(seg->y0);
      if (y >= yMin && y <= yMax) 
      {
				addIntersection(segYMin, segYMax, seg->flags, y, rasterFloor(seg->x0), rasterFloor(seg->x1));
      }
    } 
    else if (seg->flags & XPATH_VERT) 
    {
      y0 = rasterFloor(segYMin);
      if (y0 < yMin) 
      {
				y0 = yMin;
      }
      y1 = rasterFloor(segYMax);
      if (y1 > yMax) 
      {
				y1 = yMax;
      }
      x = rasterFloor(seg->x0);
      for (y = y0; y <= y1; ++y) 
      {
				addIntersection(segYMin, segYMax, seg->flags, y, x, x);
      }
    } 
    else 
    {
      if (seg->x0 < seg->x1) 
      {
				segXMin = seg->x0;
				segXMax = seg->x1;
      } 
      else 
      {
				segXMin = seg->x1;
				segXMax = seg->x0;
      }
      y0 = rasterFloor(segYMin);
      if (y0 < yMin) 
      {
				y0 = yMin;
      }
      y1 = rasterFloor(segYMax);
      if (y1 > yMax) 
      {
				y1 = yMax;
      }
      xx1 = seg->x0 + ((double)y0 - seg->y0) * seg->dxdy;
      for (y = y0; y <= y1; ++y) 
      {
				xx0 = xx1;
				xx1 = seg->x0 + ((double)(y + 1) - seg->y0) * seg->dxdy;
				// the segment may not actually extend to the top and/or bottom edges
				if (xx0 < segXMin) 
				{
	  			xx0 = segXMin;
				} 
				else if (xx0 > segXMax) 
				{
	  			xx0 = segXMax;
				}
				if (xx1 < segXMin) 
				{
	  			xx1 = segXMin;
				} 
				else if (xx1 > segXMax) 
				{
	  			xx1 = segXMax;
				}
				addIntersection(segYMin, segYMax, seg->flags, y, rasterFloor(xx0), rasterFloor(xx1));
      }
    }
  }
  
  qsort(allInter, allInterLen, sizeof(RasterIntersect), cmpIntersect);
  inter = (int *)malloc((yMax - yMin + 2) * sizeof(int));
  i = 0;
  for (y = yMin; y <= yMax; ++y) 
  {
    inter[y - yMin] = i;
    while (i < allInterLen && allInter[i].y <= y) 
    {
      ++i;
    }
  }
  inter[yMax - yMin + 1] = i;
}

