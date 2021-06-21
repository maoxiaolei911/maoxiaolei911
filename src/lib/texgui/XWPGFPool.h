/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPGFPOOL_H
#define XWPGFPOOL_H

#include "XWTeXGuiType.h"

QString XW_TEXGUI_EXPORT getPGFString(qint32 s);
QString XW_TEXGUI_EXPORT getPGFTranslate(int type);

qint32 XW_TEXGUI_EXPORT lookupPGFID(qint32 * buf, qint32 l);
qint32 XW_TEXGUI_EXPORT lookupPGFID(const QString & str);

#endif //XWPGFPOOL_H
