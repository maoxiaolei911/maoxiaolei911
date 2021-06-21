/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWLaTeXEditorMainWindow.h"
#include "XWLaTeXEditorCore.h"

XWLaTeXEditorCore::XWLaTeXEditorCore(XWLaTeXEditorMainWindow * mainwinA, QWidget * parent)
	:XWGuiCore(false, parent)
{
	fullScreen = false;
	continuousMode = true;
	doubleMode = false;
	horiMode = false;
	rightToLeft = false;
	marking = false;
	mainWin = mainwinA;
}

void XWLaTeXEditorCore::showContextMenu(const QPoint &p)
{
	QMenu menu;
	QAction * a = 0;
	if (canNextPage())
	{
		a = menu.addAction(tr("next page"));
		connect(a, SIGNAL(triggered()), this, SLOT(gotoNextPage()));
	}

	if (canPrevPage())
	{
		a = menu.addAction(tr("previous page"));
		connect(a, SIGNAL(triggered()), this, SLOT(gotoPrevPage()));
	}

	if (canFirstPage())
	{
		a = menu.addAction(tr("first page"));
		connect(a, SIGNAL(triggered()), this, SLOT(gotoFirstPage()));
	}

	if (canLastPage())
	{
		a = menu.addAction(tr("last page"));
		connect(a, SIGNAL(triggered()), this, SLOT(gotoLastPage()));
	}

  if (a)
	  menu.addSeparator();

	a = menu.addAction(tr("zoom in"));
	connect(a, SIGNAL(triggered()), this, SLOT(zoomIn()));
	a = menu.addAction(tr("zoom out"));
	connect(a, SIGNAL(triggered()), this, SLOT(zoomOut()));

	menu.exec(p);
}
