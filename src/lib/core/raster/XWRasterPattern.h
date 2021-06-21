/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWRASTERPATTERN_H
#define XWRASTERPATTERN_H

#include "XWGlobal.h"
#include "XWRasterType.h"

class XW_RASTER_EXPORT XWRasterPattern
{
public:
    XWRasterPattern();
    virtual ~XWRasterPattern();
    
    virtual XWRasterPattern *copy() = 0;
    
    virtual void getColor(int x, int y, uchar * c) = 0;
    
    virtual bool isStatic() = 0;
};

class XW_RASTER_EXPORT XWSolidColor: public XWRasterPattern
{
public:
    XWSolidColor(uchar * c);
    virtual ~XWSolidColor();
    
    virtual XWRasterPattern *copy() { return new XWSolidColor(color); }
    
    virtual void getColor(int, int, uchar * c);
    
    virtual bool isStatic() { return true; }
    
private:
    uchar color[MAX_COLOR_COMPS];
}; 

#endif // XWRASTERPATTERN_H

