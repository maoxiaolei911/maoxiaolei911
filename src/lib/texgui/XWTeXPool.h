/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTEXPOOL_H
#define XWTEXPOOL_H

#include "XWTeXGuiType.h"

QString XW_TEXGUI_EXPORT getTexString(qint32 s);
QString XW_TEXGUI_EXPORT getTexTranslate(int type);

qint32 XW_TEXGUI_EXPORT lookupTexID(qint32 * buf, qint32 l);
qint32 XW_TEXGUI_EXPORT lookupTexID(const QString & str);

#endif //XWTEXPOOL_H
