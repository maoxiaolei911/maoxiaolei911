/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWRASTERPATH_H
#define XWRASTERPATH_H

#include "XWGlobal.h"

#define PATH_FIRST         0x01
#define PATH_LAST          0x02
#define PATH_CLOSED        0x04
#define PATH_CURVE         0x08

struct XW_RASTER_EXPORT PathPoint 
{
    double x, y;
};


struct XW_RASTER_EXPORT PathHint 
{
    int ctrl0, ctrl1;
    int firstPt, lastPt;
};

class XW_RASTER_EXPORT XWRasterPath
{
public:
    XWRasterPath();
    ~XWRasterPath();
    
    void addStrokeAdjustHint(int ctrl0, int ctrl1, int firstPt, int lastPt);
    void append(XWRasterPath *path);
    
    bool          close(bool force = false);
    XWRasterPath *copy() { return new XWRasterPath(this); }
    bool          curveTo(double x1, double y1,
		                  double x2, double y2,
		                  double x3, double y3);
    
    bool getCurPt(double *x, double *y);
    int  getLength() { return length; }
    void getPoint(int i, double *x, double *y, uchar *f)
            { *x = pts[i].x; *y = pts[i].y; *f = flags[i]; }
    
    bool lineTo(double x, double y);
    
    bool moveTo(double x, double y);
    
    void offset(double dx, double dy);
    
private:
    XWRasterPath(XWRasterPath *path);
    
    void grow(int nPts);
    
    bool noCurrentPoint() { return curSubpath == length; }
    
    bool onePointSubpath() { return curSubpath == length - 1; }
    bool openSubpath() { return curSubpath < length - 1; }
    
private:
    PathPoint *pts;
    uchar *flags;
    int length, size;
    int curSubpath;
    PathHint *hints;
    int hintsLength, hintsSize;
    
#if defined(XW_DEBUG)
		PathPoint * loPts;
		PathPoint * hiPts;
		uchar     * loFlags;
		uchar     * hiFlags;
		PathHint  * loHints;
		PathHint  * hiHints;
#endif
    
    friend class XWRasterXPath;
    friend class XWRaster;
};

#endif // XWRASTERPATH_H

