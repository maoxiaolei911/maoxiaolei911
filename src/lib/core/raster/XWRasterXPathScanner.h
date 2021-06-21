/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWRASTERXPATHSCANNER_H
#define XWRASTERXPATHSCANNER_H

#include "XWGlobal.h"

struct XW_RASTER_EXPORT RasterIntersect
{
		int y;
    int x0, x1;
    int count;
};

class XWRasterXPath;
class XWBitmap;


class XW_RASTER_EXPORT XWRasterXPathScanner
{
public:
    XWRasterXPathScanner(XWRasterXPath * xPathA, bool eoA, int clipYMin, int clipYMax);
    ~XWRasterXPathScanner();
    
    void clipAALine(XWBitmap *aaBuf, int *x0, int *x1, int y);
    
    void getBBox(int *xMinA, int *yMinA, int *xMaxA, int *yMaxA)
            { *xMinA = xMin; *yMinA = yMin; *xMaxA = xMax; *yMaxA = yMax; }
    void getBBoxAA(int *xMinA, int *yMinA, int *xMaxA, int *yMaxA);
    bool getNextSpan(int y, int *x0, int *x1);
    void getSpanBounds(int y, int *spanXMin, int *spanXMax);
    
    bool hasPartialClip() { return partialClip; }
    
    void renderAALine(XWBitmap *aaBuf, int *x0, int *x1, int y);
    
    bool test(int x, int y);
    bool testSpan(int x0, int x1, int y);
    
public:
    XWRasterXPath *xPath;
    bool eo;
    int xMin, yMin, xMax, yMax;
    bool partialClip;

    RasterIntersect *allInter;
    int allInterLen;	
  	int allInterSize;
  	int *inter;
    int interY;
    int interIdx;
    int interCount;
    
private:
	void addIntersection(double segYMin, double segYMax,
		       						uint segFlags,
		       						int y, int x0, int x1);
    void computeIntersections();
};

#endif // XWRASTERXPATHSCANNER_H

