/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSTRICKSPOOL_H
#define XWPSTRICKSPOOL_H

#include "XWTeXGuiType.h"

QString XW_TEXGUI_EXPORT getPSTrickString(qint32 s);
QString XW_TEXGUI_EXPORT getPSTrickTranslate(int type);

qint32 XW_TEXGUI_EXPORT lookupPSTrickID(qint32 * buf, qint32 l);
qint32 XW_TEXGUI_EXPORT lookupPSTrickID(const QString & str);

#endif //XWPSTRICKSPOOL_H
