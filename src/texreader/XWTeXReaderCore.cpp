/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWTeXReaderMainWindow.h"
#include "XWTeXReaderCore.h"

XWTeXReaderCore::XWTeXReaderCore(XWTeXReaderMainWindow * mainwinA, QWidget * parent)
	:XWGuiCore(false, parent)
{
	mainWin = mainwinA;
}

void XWTeXReaderCore::showContextMenu(const QPoint &p)
{
	if (getNumPages() == 0)
		return ;
		
	QMenu menu;
	mainWin->addToContextMenu(&menu);
	menu.exec(p);
}


