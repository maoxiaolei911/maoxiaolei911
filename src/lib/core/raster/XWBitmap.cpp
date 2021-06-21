/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <string.h>
#include <stdio.h>
#include <QtDebug>
#include <QByteArray>
#include <QFile>
#include "XWApplication.h"
#include "XWRasterType.h"
#include "XWBitmap.h"

XWBitmap::XWBitmap(int widthA, 
             int heightA, 
             int rowPad,
	       		 int modeA, 
	       		 bool alphaA,
	       		 bool topDown)
{
  width = widthA;
  height = heightA;
  mode = modeA;
  switch (mode) {
  case COLOR_MODE_MONO1:
    if (width > 0) {
      rowSize = (width + 7) >> 3;
    } else {
      rowSize = -1;
    }
    break;
  case COLOR_MODE_MONO8:
    if (width > 0) {
      rowSize = width;
    } else {
      rowSize = -1;
    }
    break;
  case COLOR_MODE_RGB8:
  case COLOR_MODE_BGR8:
    if (width > 0 && width <= INT_MAX / 3) {
      rowSize = width * 3;
    } else {
      rowSize = -1;
    }
    break;
  case COLOR_MODE_CMYK8:
    if (width > 0 && width <= INT_MAX / 4) {
      rowSize = width * 4;
    } else {
      rowSize = -1;
    }
    break;
  }
  if (rowSize > 0) {
    rowSize += rowPad - 1;
    rowSize -= rowSize % rowPad;
  }
  
  data = (uchar*)malloc(height * rowSize * sizeof(uchar)); 
#if defined(XW_DEBUG)
	loData = data;
	hiData = data + height * rowSize;
#endif

  if (!topDown) {
    data += (height - 1) * rowSize;
    rowSize = -rowSize;
  }
  if (alphaA) {
    alpha = (uchar *)malloc(width * height * sizeof(uchar));
#if defined(XW_DEBUG)
		loAlpha = alpha;
		hiAlpha = alpha + width * height;
#endif
  } else {
    alpha = NULL;
#if defined(XW_DEBUG)
		loAlpha = 0;
		hiAlpha = 0;
#endif
  }
}

XWBitmap::~XWBitmap()
{
   if (data) {
    if (rowSize < 0) {
      free(data + (height - 1) * rowSize);
    } else {
      free(data);
    }
  }
  if (alpha)
  {
  	free(alpha);
  }
}

bool XWBitmap::blitTransparent(XWBitmap *src,
                               int xSrc,
                               int ySrc,
                               int xDest,
                               int yDest,
                               int w,
                               int h)
{
    if (src->mode != mode)
        return false;

    uchar * p = 0;
    uchar * q = 0;
    int mask = 0;
    int srcMask = 0;
    switch (mode)
    {
        case COLOR_MODE_MONO1:
            for (int y = 0; y < h; ++y)
            {
                p = &data[(yDest + y) * rowSize + (xDest >> 3)];
                mask = 0x80 >> (xDest & 7);
                q = &src->data[(ySrc + y) * src->rowSize + (xSrc >> 3)];
      					srcMask = 0x80 >> (xSrc & 7);
                for (int x = 0; x < w; ++x)
                {
                    if (*q & srcMask)
                        *p |= mask;
                    else
                        *p &= ~mask;

                    if (!(mask >>= 1))
                    {
                        mask = 0x80;
                        ++p;
                    }
                    
                    if (!(srcMask >>= 1)) 
                    {
	  									srcMask = 0x80;
	  									++q;
										}
                }
            }
            break;

        case COLOR_MODE_MONO8:
            for (int y = 0; y < h; ++y)
            {
                p = &data[(yDest + y) * rowSize + xDest];
                q = &src->data[(ySrc + y) * src->rowSize + xSrc];
                for (int x = 0; x < w; ++x)
                    *p++ = *q++;
            }
            break;

        case COLOR_MODE_RGB8:
        case COLOR_MODE_BGR8:
            for (int y = 0; y < h; ++y)
            {
                p = &data[(yDest + y) * rowSize + 3 * xDest];
                q = &src->data[(ySrc + y) * src->rowSize + 3 * xSrc];
                for (int x = 0; x < w; ++x)
                {
                    *p++ = *q++;
										*p++ = *q++;
										*p++ = *q++;
                }
            }
            break;

        case COLOR_MODE_CMYK8:
            for (int y = 0; y < h; ++y)
            {
                p = &data[(yDest + y) * rowSize + 4 * xDest];
                q = &src->data[(ySrc + y) * src->rowSize + 4 * xSrc];
                for (int x = 0; x < w; ++x)
                {
                    *p++ = *q++;
										*p++ = *q++;
										*p++ = *q++;
										*p++ = *q++;
                }
            }
            break;
    }

    if (alpha)
    {
        for (int y = 0; y < h; ++y)
        {
            q = &alpha[(yDest + y) * width + xDest];
            for (int x = 0; x < w; ++x)
                *q++ = 0x00;
        }
    }

    return true;
}

void XWBitmap::clear(uchar * color, uchar a)
{
    uchar * row = 0;
    uchar * p = 0;
    uchar mono = 0;
    switch (mode)
    {
        case COLOR_MODE_MONO1:
            mono = (color[0] & 0x80) ? 0xff : 0x00;
            if (rowSize < 0)
                memset(data + rowSize * (height - 1), mono, -rowSize * height);
            else
                memset(data, mono, rowSize * height);
            break;

        case COLOR_MODE_MONO8:
            if (rowSize < 0)
                memset(data + rowSize * (height - 1), color[0], -rowSize * height);
            else
                memset(data, color[0], rowSize * height);
            break;

        case COLOR_MODE_RGB8:
            if (color[0] == color[1] && color[1] == color[2])
            {
                if (rowSize < 0)
                    memset(data + rowSize * (height - 1), color[0], -rowSize * height);
                else
                    memset(data, color[0], rowSize * height);
            }
            else
            {
                row = data;
                for (int y = 0; y < height; ++y)
                {
                    p = row;
                    for (int x = 0; x < width; ++x)
                    {
                        *p++ = color[2];
                        *p++ = color[1];
                        *p++ = color[0];
                    }
                    row += rowSize;
                }
            }
            break;

        case COLOR_MODE_BGR8:
            if (color[0] == color[1] && color[1] == color[2])
            {
                if (rowSize < 0)
                    memset(data + rowSize * (height - 1), color[0], -rowSize * height);
                else
                    memset(data, color[0], rowSize * height);
            }
            else
            {
                row = data;
                for (int y = 0; y < height; ++y)
                {
                    p = row;
                    for (int x = 0; x < width; ++x)
                    {
                        *p++ = color[0];
                        *p++ = color[1];
                        *p++ = color[2];
                    }
                    row += rowSize;
                }
            }
            break;

        case COLOR_MODE_CMYK8:
            if (color[0] == color[1] &&
                color[1] == color[2] &&
                color[2] == color[3])
            {
                if (rowSize < 0)
                    memset(data + rowSize * (height - 1), color[0], -rowSize * height);
                else
                    memset(data, color[0], rowSize * height);
            }
            else
            {
                row = data;
                for (int y = 0; y < height; ++y)
                {
                    p = row;
                    for (int x = 0; x < width; ++x)
                    {
                        *p++ = color[0];
                        *p++ = color[1];
                        *p++ = color[2];
                        *p++ = color[3];
                    }
                    row += rowSize;
                }
            }
            break;
    }

    if (alpha)
        memset(alpha, a, width * height);
}

void XWBitmap::compositeBackground(uchar *color)
{
    uchar * p = 0;
    uchar * q = 0;
    uchar a, a1, c, c0, c1, c2, c3;
    int mask = 0;
    switch (mode)
    {
        case COLOR_MODE_MONO1:
            c0 = color[0];
            for (int y = 0; y < height; ++y)
            {
                p = &data[y * rowSize];
                q = &alpha[y * width];
                mask = 0x80;
                for (int x = 0; x < width; ++x)
                {
                    a = *q++;
                    a1 = 255 - a;
                    c = (*p & mask) ? 0xff : 0x00;
                    c = div255(a1 * c0 + a * c);
                    if (c & 0x80)
                        *p |= mask;
                    else
                        *p &= ~mask;

                    if (!(mask >>= 1))
                    {
                        mask = 0x80;
                        ++p;
                    }
                }
            }
            break;

        case COLOR_MODE_MONO8:
            c0 = color[0];
            for (int y = 0; y < height; ++y)
            {
                p = &data[y * rowSize];
                q = &alpha[y * width];
                for (int x = 0; x < width; ++x)
                {
                    a = *q++;
                    a1 = 255 - a;
                    p[0] = div255(a1 * c0 + a * p[0]);
                    ++p;
                }
            }
            break;

        case COLOR_MODE_RGB8:
        case COLOR_MODE_BGR8:
            c0 = color[0];
            c1 = color[1];
            c2 = color[2];
            for (int y = 0; y < height; ++y)
            {
                p = &data[y * rowSize];
                q = &alpha[y * width];
                for (int x = 0; x < width; ++x)
                {
                    a = *q++;
                    a1 = 255 - a;
                    p[0] = div255(a1 * c0 + a * p[0]);
                    p[1] = div255(a1 * c1 + a * p[1]);
                    p[2] = div255(a1 * c2 + a * p[2]);
                    p += 3;
                }
            }
            break;

        case COLOR_MODE_CMYK8:
            c0 = color[0];
            c1 = color[1];
            c2 = color[2];
            c3 = color[3];
            for (int y = 0; y < height; ++y)
            {
                p = &data[y * rowSize];
                q = &alpha[y * width];
                for (int x = 0; x < width; ++x)
                {
                    a = *q++;
                    a1 = 255 - a;
                    p[0] = div255(a1 * c0 + a * p[0]);
                    p[1] = div255(a1 * c1 + a * p[1]);
                    p[2] = div255(a1 * c2 + a * p[2]);
                    p[3] = div255(a1 * c3 + a * p[3]);
                    p += 4;
                }
            }
            break;
    }

    memset(alpha, 255, width * height);
}

uchar XWBitmap::getAlpha(int x, int y)
{
    return alpha[y * width + x];
}

void XWBitmap::getPixel(int x, int y, uchar* pixel)
{
    if (y < 0 || y >= height || x < 0 || x >= width)
        return ;

    uchar * p = 0;
    switch (mode)
    {
        case COLOR_MODE_MONO1:
            p = &data[y * rowSize + (x >> 3)];
            pixel[0] = (p[0] & (0x80 >> (x & 7))) ? 0xff : 0x00;
            break;

        case COLOR_MODE_MONO8:
            p = &data[y * rowSize + x];
            pixel[0] = p[0];
            break;

        case COLOR_MODE_RGB8:
            p = &data[y * rowSize + 3 * x];
            pixel[0] = p[0];
            pixel[1] = p[1];
            pixel[2] = p[2];
            break;

        case COLOR_MODE_BGR8:
            p = &data[y * rowSize + 3 * x];
            pixel[0] = p[2];
            pixel[1] = p[1];
            pixel[2] = p[0];
            break;

        case COLOR_MODE_CMYK8:
            p = &data[y * rowSize + 4 * x];
            pixel[0] = p[0];
            pixel[1] = p[1];
            pixel[2] = p[2];
            pixel[3] = p[3];
            break;
    }
}

uchar * XWBitmap::takeData()
{
	uchar * data2 = data;
  data = 0;
  return data2;
}

QImage XWBitmap::toQImage()
{
	QImage img(width, height, QImage::Format_RGB32);
	uchar pixel[4];
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			getPixel(x, y, pixel);
			img.setPixel(x, y, qRgb(pixel[0], pixel[1], pixel[2]));
		}
	}
	
	return img;
}

QImage XWBitmap::toQImage(int xo, int yo, int w, int h)
{
	QImage img(w, h, QImage::Format_RGB32);
	uchar pixel[4];
	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; y++)
		{
			getPixel(x + xo, y + yo, pixel);
			img.setPixel(x, y, qRgb(pixel[0], pixel[1], pixel[2]));
		}
	}
	
	return img;
}

void XWBitmap::writeAlphaPGMFile(const QString & filename)
{
	QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
        xwApp->openError(filename, true);

    writeAlphaPGMFile(&file);
}

void XWBitmap::writeAlphaPGMFile(QIODevice * f)
{
	char buf[100];
	int l = sprintf(buf, "P5\n%d %d\n255\n", width, height);
	f->write(buf, l);
	f->write((char*)alpha, width * height);
	f->close();
}

bool XWBitmap::writePNMFile(const QString & filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
        return xwApp->openError(filename, true);

    return writePNMFile(&file);
}

bool XWBitmap::writePNMFile(QIODevice * f)
{
	uchar * row = 0;
    uchar * p = 0;
    QByteArray ba("P");
    switch (mode)
    {
        case COLOR_MODE_MONO1:
            ba += "4\n";
            ba += QByteArray::number(width);
            ba += " ";
            ba += QByteArray::number(height);
            ba += "\n";
            f->write(ba);
            row = data;
            for (int y = 0; y < height; ++y)
            {
                p = row;
                for (int x = 0; x < width; x += 8)
                {
                    f->putChar((char)(*p ^ 0xff));
                    ++p;
                }
                row += rowSize;
            }
            break;

        case COLOR_MODE_MONO8:
            ba += "5\n";
            ba += QByteArray::number(width);
            ba += " ";
            ba += QByteArray::number(height);
            ba += "\n255\n";
            f->write(ba);
            row = data;
            for (int y = 0; y < height; ++y)
            {
                p = row;
                for (int x = 0; x < width; ++x)
                {
                    f->putChar((char)(*p));
                    ++p;
                }
                row += rowSize;
            }
            break;

        case COLOR_MODE_RGB8:
            ba += "6\n";
            ba += QByteArray::number(width);
            ba += " ";
            ba += QByteArray::number(height);
            ba += "\n255\n";
            f->write(ba);
            row = data;
            for (int y = 0; y < height; ++y)
            {
                p = row;
                for (int x = 0; x < width; ++x)
                {
                    f->putChar((char)(rgb8R(p)));
                    f->putChar((char)(rgb8G(p)));
                    f->putChar((char)(rgb8B(p)));
                    p += 3;
                }
                row += rowSize;
            }
            break;

        case COLOR_MODE_BGR8:
            ba += "6\n";
            ba += QByteArray::number(width);
            ba += " ";
            ba += QByteArray::number(height);
            ba += "\n255\n";
            f->write(ba);
            row = data;
            for (int y = 0; y < height; ++y)
            {
                p = row;
                for (int x = 0; x < width; ++x)
                {
                    f->putChar((char)(bgr8R(p)));
                    f->putChar((char)(bgr8G(p)));
                    f->putChar((char)(bgr8B(p)));
                    p += 3;
                }
                row += rowSize;
            }
            break;

        default:
            break;
    }

    f->close();
    return true;
}

