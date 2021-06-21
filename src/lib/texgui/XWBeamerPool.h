/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWBEAMERPOOL_H
#define XWBEAMERPOOL_H

#include "XWTeXGuiType.h"

QString XW_TEXGUI_EXPORT getBeamerString(qint32 s);
QString XW_TEXGUI_EXPORT getBeamerTranslate(int type);

qint32 XW_TEXGUI_EXPORT lookupBeamerID(qint32 * buf, qint32 l);
qint32 XW_TEXGUI_EXPORT lookupBeamerID(const QString & str);

#endif //XWBEAMERPOOL_H
