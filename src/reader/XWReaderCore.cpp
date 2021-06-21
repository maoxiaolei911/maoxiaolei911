/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWApplication.h"
#include "XWDoc.h"
#include "XWReaderMainWindow.h"
#include "XWReaderCore.h"

XWReaderCore::XWReaderCore(XWReaderMainWindow * mainwinA, QWidget * parent)
	:XWGuiCore(false, parent)
{
	mainWin = mainwinA;
}

void XWReaderCore::showContextMenu(const QPoint &p)
{
	if (getNumPages() == 0)
		return ;
		
	QMenu menu;
	mainWin->addToContextMenu(&menu);
	menu.exec(p);
}


