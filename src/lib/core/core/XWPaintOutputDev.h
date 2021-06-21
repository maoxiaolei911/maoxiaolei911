/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPAINTOUTPUTDEV_H
#define XWPAINTOUTPUTDEV_H

#include <QPainter>

#include "XWRasterOutputDev.h"

class XW_CORE_EXPORT XWPaintOutputDev : public XWRasterOutputDev
{
public:
		XWPaintOutputDev(QPainter * painterA);
    XWPaintOutputDev(QPainter * painterA, 
                     int colorModeA, 
                     int bitmapRowPadA,
		             bool reverseVideoA, 
		             quint8 * paperColorA,
		             bool bitmapTopDownA = true,
		             bool allowAntialiasA = true);
    virtual ~XWPaintOutputDev();
			              		
    virtual void endPage();
    
    void setArea(int xo, int yo, int x, int y, int w, int h);
    void setPainter(QPainter * painterA) {painter = painterA;}
	
private:
	QPainter * painter;
	int hOff;
	int vOff;
	int drawX;
	int drawY;
	int drawW;
	int drawH;
};

#endif // XWPAINTOUTPUTDEV_H

