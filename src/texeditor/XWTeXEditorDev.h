/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTEXEDITORDEV_H
#define XWTEXEDITORDEV_H

#include "XWTeXIODev.h"

class XWTeXEditorDev : public XWTeXIODev
{
	Q_OBJECT
	
public:
	XWTeXEditorDev(QObject * parent = 0);
	~XWTeXEditorDev();
	
	void compile();
};

#endif //XWTEXEDITORDEV_H

