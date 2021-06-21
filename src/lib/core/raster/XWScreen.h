/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWSCREEN_H
#define XWSCREEN_H

#include "XWGlobal.h"

struct ScreenParams;

class XW_RASTER_EXPORT XWScreen
{
public:
    XWScreen(ScreenParams *params);
    XWScreen(XWScreen * screen);
    ~XWScreen();
    
    XWScreen *copy() { return new XWScreen(this); }
    
    bool isStatic(uchar value);
    
    int test(int x, int y, uchar value);
    
private:    
    void buildClusteredMatrix();
    void buildDispersedMatrix(int i, int j, int val, int delta, int offset);
    void buildSCDMatrix(int r);
    int  distance(int x0, int y0, int x1, int y1);
  
private:
    uchar * mat;
    int size;	
    int sizeM1;
    int log2Size;
    uchar minVal;
    uchar maxVal;
};


#endif // XWSCREEN_H

