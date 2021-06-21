/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTIKZCOLOR_H
#define XWTIKZCOLOR_H

#include <QColor>

QColor tikzEnumToColor(int C);
QColor calulateColor(const QColor & c, double p);
QColor calulateColor(const QColor & c1, double p, const QColor & c2);

#endif //XWTIKZCOLOR_H
