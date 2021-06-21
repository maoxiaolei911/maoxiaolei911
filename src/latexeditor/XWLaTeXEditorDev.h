/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWLATEXEDITORDEV_H
#define XWLATEXEDITORDEV_H

#include "XWTeXIODev.h"

class XWLaTeXEditorDev : public XWTeXIODev
{
	Q_OBJECT

public:
	XWLaTeXEditorDev(QObject * parent = 0);
	~XWLaTeXEditorDev();

	void compile();
};

#endif //XWLATEXEDITORDEV_H
