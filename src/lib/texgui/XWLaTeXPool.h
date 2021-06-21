/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWLATEXPOOL_H
#define XWLATEXPOOL_H

#include "XWTeXGuiType.h"

QString XW_TEXGUI_EXPORT getLaTeXString(qint32 s);
QString XW_TEXGUI_EXPORT getLaTeXTranslate(int type);

qint32 XW_TEXGUI_EXPORT lookupLaTeXID(qint32 * buf, qint32 l);
qint32 XW_TEXGUI_EXPORT lookupLaTeXID(const QString & str);

#endif //XWLATEXPOOL_H
