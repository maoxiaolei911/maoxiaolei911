/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include "XWRasterPath.h"

XWRasterPath::XWRasterPath()
{
  pts = NULL;
  flags = NULL;
  length = size = 0;
  curSubpath = 0;
  hints = NULL;
  hintsLength = hintsSize = 0;
}

XWRasterPath::XWRasterPath(XWRasterPath *path)
{
    length = path->length;
    size = path->size;
    pts = (PathPoint *)malloc(size * sizeof(PathPoint));
    flags = (uchar *)malloc(size * sizeof(uchar));
    memcpy(pts, path->pts, length * sizeof(PathPoint));
    memcpy(flags, path->flags, length * sizeof(uchar));
#if defined(XW_DEBUG)
		loPts = pts;
		hiPts = pts + size;
		loFlags = flags;
		hiFlags = flags + size;
#endif
    curSubpath = path->curSubpath;
    if (path->hints) 
    {
        hintsLength = hintsSize = path->hintsLength;
        hints = (PathHint *)malloc(hintsSize * sizeof(PathHint));
        memcpy(hints, path->hints, hintsLength * sizeof(PathHint));
#if defined(XW_DEBUG)
				loHints = hints;
				hiHints = hints + hintsSize;
#endif
    } 
    else 
    {    	
        hints = 0;
#if defined(XW_DEBUG)
				loHints = 0;
				hiHints = 0;
#endif
    }
}

XWRasterPath::~XWRasterPath()
{
    if (pts)
        free(pts);
        
    if (flags)
        free(flags);
        
    if (hints)
        free(hints);
}

void XWRasterPath::addStrokeAdjustHint(int ctrl0, int ctrl1, int firstPt, int lastPt)
{
    if (hintsLength == hintsSize) 
    {
        hintsSize = hintsLength ? 2 * hintsLength : 8;
        hints = (PathHint *)realloc(hints, hintsSize * sizeof(PathHint));
#if defined(XW_DEBUG)
				loHints = hints;
				hiHints = hints + hintsSize;
#endif
    }
    
    hints[hintsLength].ctrl0 = ctrl0;
    hints[hintsLength].ctrl1 = ctrl1;
    hints[hintsLength].firstPt = firstPt;
    hints[hintsLength].lastPt = lastPt;
    ++hintsLength;
}

void XWRasterPath::append(XWRasterPath *path)
{
    curSubpath = length + path->curSubpath;
    grow(path->length);
    for (int i = 0; i < path->length; ++i) 
    {
        pts[length] = path->pts[i];
        flags[length] = path->flags[i];
        ++length;
    }
}

bool XWRasterPath::close(bool force)
{
    if (noCurrentPoint()) 
        return false;
        
    if (force || curSubpath == length - 1 ||
        pts[length - 1].x != pts[curSubpath].x ||
        pts[length - 1].y != pts[curSubpath].y) 
    {
        lineTo(pts[curSubpath].x, pts[curSubpath].y);
    }
    
    flags[curSubpath] |= PATH_CLOSED;
    flags[length - 1] |= PATH_CLOSED;
    curSubpath = length;
    return true;
}

bool XWRasterPath::curveTo(double x1, double y1,
		                  double x2, double y2,
		                  double x3, double y3)
{
    if (noCurrentPoint()) 
        return false;
    
    flags[length-1] &= ~PATH_LAST;
    grow(3);
    pts[length].x = x1;
    pts[length].y = y1;
    flags[length] = PATH_CURVE;
    ++length;
    pts[length].x = x2;
    pts[length].y = y2;
    flags[length] = PATH_CURVE;
    ++length;
    pts[length].x = x3;
    pts[length].y = y3;
    flags[length] = PATH_LAST;
    ++length;
    return true;
}

bool XWRasterPath::getCurPt(double *x, double *y)
{
    if (noCurrentPoint()) 
        return false;
        
    *x = pts[length - 1].x;
    *y = pts[length - 1].y;
    return true;
}

bool XWRasterPath::lineTo(double x, double y)
{
    if (noCurrentPoint()) 
        return false;
    
    flags[length-1] &= ~PATH_LAST;
    grow(1);
    pts[length].x = x;
    pts[length].y = y;
    flags[length] = PATH_LAST;
    ++length;
    return true;
}

bool XWRasterPath::moveTo(double x, double y)
{
    if (onePointSubpath()) 
        return false;
    
    grow(1);
    pts[length].x = x;
    pts[length].y = y;
    flags[length] = PATH_FIRST | PATH_LAST;
    curSubpath = length++;
    return true;
}

void XWRasterPath::offset(double dx, double dy)
{
    for (int i = 0; i < length; ++i) 
    {
        pts[i].x += dx;
        pts[i].y += dy;
    }
}

void XWRasterPath::grow(int nPts)
{
    if (length + nPts > size) 
    {
        if (size == 0) 
            size = 32;
        
        while (size < length + nPts) 
            size *= 2;
            
        pts = (PathPoint *)realloc(pts, size * sizeof(PathPoint));
        flags = (uchar *)realloc(flags, size * sizeof(uchar));
#if defined(XW_DEBUG)
				loPts = pts;
				hiPts = pts + size;
				loFlags = flags;
				hiFlags = flags + size;
#endif
    }
}

