/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWTeXEditorMainWindow.h"
#include "XWTeXEditorCore.h"

XWTeXEditorCore::XWTeXEditorCore(XWTeXEditorMainWindow * mainwinA, QWidget * parent)
	:XWGuiCore(false, parent)
{
	mainWin = mainwinA;
}

void XWTeXEditorCore::showContextMenu(const QPoint &p)
{
	if (getNumPages() == 0)
		return ;
		
	QMenu menu;
	mainWin->addToContextMenu(&menu);
	menu.exec(p);
}


