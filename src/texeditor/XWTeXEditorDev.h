/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
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

