/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWRasterPattern.h"

XWRasterPattern::XWRasterPattern()
{
}

XWRasterPattern::~XWRasterPattern()
{
}

XWSolidColor::XWSolidColor(uchar * c)
{
    colorCopy(color, c);
}

XWSolidColor::~XWSolidColor()
{
}

void XWSolidColor::getColor(int, int, uchar * c)
{
    colorCopy(c, color);
}
