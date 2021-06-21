/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWRASTERXPATH_H
#define XWRASTERXPATH_H

#include "XWGlobal.h"

#define MAX_CURVE_SPLITS (1 << 10)


#define XPATH_FIRST   0x01
#define XPATH_LAST    0x02
#define XPATH_END0    0x04
#define XPATH_END1    0x08

#define XPATH_HORIZ   0x10
#define XPATH_VERT    0x20
#define XPATH_FLIP	  0x40


class XWRasterPath;
struct XPathAdjust;

struct XPathSeg 
{
    double x0, y0;		// first endpoint
    double x1, y1;		// second endpoint
    double dxdy;		// slope: delta-x / delta-y
    double dydx;		// slope: delta-y / delta-x
    uint flags;
};

class XW_RASTER_EXPORT XWRasterXPath
{
public:    
    XWRasterXPath(XWRasterPath *path, 
                  double * matrix, 
                  double flatness, 
                  bool closeSubpaths);
    
    ~XWRasterXPath();
    
    void aaScale();
    
    XWRasterXPath *copy() { return new XWRasterXPath(this); }
    
    void sort();
        
private:
    XWRasterXPath(XWRasterXPath * xPath);
    
    void addCurve(double x0, 
                  double y0,
		          double x1, 
		          double y1,
		          double x2, 
		          double y2,
		          double x3, 
		          double y3,
		          double flatness,
		          bool first, 
		          bool last, 
		          bool end0, 
		          bool end1);
		
    void addSegment(double x0, 
                    double y0,
			        double x1, 
			        double y1);
		  
    bool grow(int nSegs);
    
    void strokeAdjust(XPathAdjust *adjust, double *xp, double *yp);
    
    void transform(double * matrix, double xi, double yi, double *xo, double *yo)
         {
            *xo = xi * matrix[0] + yi * matrix[2] + matrix[4];
            *yo = xi * matrix[1] + yi * matrix[3] + matrix[5];
         }
         
private:
    int length, size;    
    XPathSeg * segs;
#if defined(XW_DEBUG)
		XPathSeg * loSegs;
		XPathSeg * hiSegs;
#endif
    
    friend class XWRasterXPathScanner;
    friend class XWRasterClip;
    friend class XWRaster;
};


#endif // XWRASTERXPATH_H

