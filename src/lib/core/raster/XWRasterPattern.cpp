/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
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
