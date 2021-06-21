/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWRASTERCLIP_H
#define XWRASTERCLIP_H

#include "XWGlobal.h"

class XWRasterPath;
class XWRasterXPath;
class XWRasterXPathScanner;
class XWBitmap;


class XW_RASTER_EXPORT XWRasterClip
{
public:
    XWRasterClip(double x0, 
                 double y0,
	             double x1, 
	             double y1, 
	             bool antialiasA);
    XWRasterClip(XWRasterClip * clip);
    ~XWRasterClip();
    
    void clipAALine(XWBitmap *aaBuf, int *x0, int *x1, int y);
    bool clipToPath(XWRasterPath *path, 
                    double * matrix, 
                    double flatness, 
                    bool eo);
    bool clipToRect(double x0, double y0, double x1, double y1);
    
    XWRasterClip *copy() { return new XWRasterClip(this); }
    
    int getNumPaths() { return length; }
    double getXMin() { return xMin; }
  	double getXMax() { return xMax; }
  	double getYMin() { return yMin; }
  	double getYMax() { return yMax; }
    int getXMaxI() { return xMaxI; }
    int getXMinI() { return xMinI; }
    int getYMaxI() { return yMaxI; }
    int getYMinI() { return yMinI; }
    
    void resetToRect(double x0, double y0, double x1, double y1);
    
    bool test(int x, int y);
    int  testRect(int rectXMin, int rectYMin, int rectXMax, int rectYMax);
    int  testSpan(int spanXMin, int spanXMax, int spanY);
    
public:
    bool antialias;
    double xMin, yMin, xMax, yMax;
    int xMinI, yMinI, xMaxI, yMaxI;
    XWRasterXPath **paths;
    uchar *flags;
    XWRasterXPathScanner **scanners;
    int length, size;
    
private:
    void grow(int nPaths);
};


#endif // XWRASTERCLIP_H

