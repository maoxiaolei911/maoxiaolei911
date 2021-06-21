/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWLATEXEDITORCORE_H
#define XWLATEXEDITORCORE_H

#include "XWGuiCore.h"

class XWLaTeXEditorMainWindow;
class QAction;

class XWLaTeXEditorCore : public XWGuiCore
{
	Q_OBJECT

public:
    XWLaTeXEditorCore(XWLaTeXEditorMainWindow * mainwinA, QWidget * parent = 0);

protected:
  	void showContextMenu(const QPoint &p);

private:
	XWLaTeXEditorMainWindow * mainWin;
};

#endif // XWLATEXEDITORCORE_H
