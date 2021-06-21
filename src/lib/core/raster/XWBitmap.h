/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWBITMAP_H
#define XWBITMAP_H

#include <QImage>
#include <QPixmap>
#include <QColor>
#include <QIODevice>

#include "XWGlobal.h"

class XW_RASTER_EXPORT XWBitmap
{
public:
    XWBitmap(int widthA, 
             int heightA, 
             int rowPad,
	       		 int modeA, 
	       		 bool alphaA,
	       		 bool topDown = true);
    ~XWBitmap();
    
    bool blitTransparent(XWBitmap *src, 
                         int xSrc, 
                         int ySrc,
				         int xDest, 
				         int yDest, 
				         int w, 
				         int h);
				         
    void clear(uchar * color, uchar a);
    void compositeBackground(uchar *color);
    
    uchar   getAlpha(int x, int y);
    uchar * getAlphaPtr() { return alpha; }
    int      getAlphaRowSize() { return width; }
    uchar * getDataPtr() { return data; }
    int      getHeight() { return height; }
    int      getMode() { return mode; }
    void     getPixel(int x, int y, uchar* pixel);
    int      getRowSize() { return rowSize; }
    int      getWidth() { return width; }
    
    uchar * takeData();
    
    QImage toQImage();
    QImage toQImage(int xo, int yo, int w, int h);
                              
	void writeAlphaPGMFile(const QString & filename);
	void writeAlphaPGMFile(QIODevice * f);
	bool writePNMFile(const QString & filename);
	bool writePNMFile(QIODevice * f);
    
public:
    int width;
    int height;
    int mode;
    
    int rowSize;
    uchar * data;
    uchar * alpha;
    
#if defined(XW_DEBUG)
		uchar * loData;
		uchar * hiData;
		uchar * loAlpha;
		uchar * hiAlpha;
#endif
};


#endif // XWBITMAP_H

