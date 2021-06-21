/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include <QPrinter>
#include <QImage>
#include <QPointF>
#include <QString>
#include <QTextCodec>
#include <QRegExp>
#include <QFont>
#include <QVector>
#include <QRectF>

#include "XWApplication.h"
#include "XWBitmap.h"
#include "XWRasterPattern.h"
#include "XWRasterState.h"
#include "XWRasterPath.h"
#include "XWRaster.h"
#include "XWPattern.h"
#include "XWPath.h"
#include "XWPaintOutputDev.h"

XWPaintOutputDev::XWPaintOutputDev(QPainter * painterA)
	:hOff(0),
	 vOff(0),
	 painter(painterA),	 
	 drawX(0),
	 drawY(0),
	 drawW(0),
	 drawH(0)
{
}

XWPaintOutputDev::XWPaintOutputDev(QPainter * painterA, 
                     			   int colorModeA, 
                                   int bitmapRowPadA,
		                           bool reverseVideoA, 
		                           quint8 * paperColorA,
		                           bool bitmapTopDownA,
		                           bool allowAntialiasA)
	:XWRasterOutputDev(colorModeA, bitmapRowPadA, reverseVideoA, paperColorA, bitmapTopDownA, allowAntialiasA),
	 hOff(0),
	 vOff(0),
	 painter(painterA),	 
	 drawX(0),
	 drawY(0),
	 drawW(0),
	 drawH(0)
{
}

XWPaintOutputDev::~XWPaintOutputDev()
{
}

void XWPaintOutputDev::setArea(int xo, int yo, int x, int y, int w, int h)
{
	hOff = xo;
	vOff = yo;
	drawX = x;
	drawY = y;
	drawW = w;
	drawH = h;
}

void XWPaintOutputDev::endPage()
{
	XWRasterOutputDev::endPage();
	if (drawW == 0)
		drawW = getBitmapWidth();
		
	if (drawH == 0)
		drawH = getBitmapHeight();
		
	if (drawX < 0 || drawX > getBitmapWidth())
		drawX = 0;
		
	if (drawY < 0 || drawY > getBitmapHeight())
		drawY = 0;
		
	QImage img = bitmap->toQImage(drawX, drawY, drawW, drawH);
	painter->drawImage(QPoint(hOff, vOff), img);
}
