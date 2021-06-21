/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTEXFMTEDITORDEV_H
#define XWTEXFMTEDITORDEV_H

#include <QBuffer>
#include "XWTeXIODev.h"

class XWTeXFmtEditorDev : public XWTeXIODev
{
	Q_OBJECT
	
public:
	XWTeXFmtEditorDev(QObject * parent = 0);
	~XWTeXFmtEditorDev();
};

#endif //XWTEXFMTEDITORDEV_H

