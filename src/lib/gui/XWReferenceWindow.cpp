/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWApplication.h"
#include "XWDoc.h"
#include "XWOutlineWindow.h"
#include "XWReferenceWindow.h"

XWRefCore::XWRefCore(XWReferenceWindow * refwinA,
	                           QWidget * parent)
	:XWGuiCore(true, parent)
{
	continuousMode = false;
	horiMode = false;
	
	refWin = refwinA;
	copyAct = new QAction(tr("Copy"), this);
	connect(copyAct, SIGNAL(triggered()), this, SLOT(copy()));
	connect(this, SIGNAL(copyAvailable(bool)), copyAct, SLOT(setVisible(bool)));
	
	markingAct = new QAction(tr("Marking"), this);
	markingAct->setCheckable(true);
	markingAct->setChecked(false);
	connect(markingAct, SIGNAL(toggled(bool)), this, SLOT(setMarking(bool)));
	
	clrAct = new QAction(tr("Clear Marks"), this);
	connect(clrAct, SIGNAL(triggered()), this, SLOT(clearMark()));
	
	fitPageAct = new QAction(tr("Fit Page"), this);
	connect(fitPageAct, SIGNAL(triggered()), this, SLOT(fitToPage()));
		
	fitWidthAct = new QAction(tr("Fit Width"), this);
	connect(fitWidthAct, SIGNAL(triggered()), this, SLOT(fitToWidth()));
		
	zoomInAct = new QAction(tr("Zoom in"), this);
	connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));
		
	zoomOutAct = new QAction(tr("Zoom out"), this);
	connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));
		
	nextAct = new QAction(tr("Next Page"), this);
	connect(nextAct, SIGNAL(triggered()), this, SLOT(gotoNextPage()));
	connect(this, SIGNAL(nextPageAvailable(bool)), nextAct, SLOT(setVisible(bool)));
		
	prevAct = new QAction(tr("Previous Page"), this);
	connect(prevAct, SIGNAL(triggered()), this, SLOT(gotoPrevPage()));
	connect(this, SIGNAL(prevPageAvailable(bool)), prevAct, SLOT(setVisible(bool)));
	
	outlineAct = new QAction(tr("Show Outline"), this);
	outlineAct->setCheckable(true);
	outlineAct->setChecked(false);
	connect(outlineAct, SIGNAL(toggled(bool)), refWin, SLOT(showOutline(bool)));
	
	statusAct = new QAction(tr("Show Status"), this);
	statusAct->setCheckable(true);
	statusAct->setChecked(true);
	connect(statusAct, SIGNAL(toggled(bool)), refWin, SLOT(showStatus(bool)));
}

void XWRefCore::showContextMenu(const QPoint &p)
{
	QMenu menu;
	if (copyAct->isVisible())
	{
		menu.addAction(copyAct);	
		menu.addSeparator();
	}
	
	QMenu * colorMenu = menu.addMenu(tr("Mark"));
	colorMenu->addAction(markingAct);
	colorMenu->addSeparator();
	addMarkingActions(colorMenu);
	colorMenu->addSeparator();
	colorMenu->addAction(clrAct);
	menu.addSeparator();
	
	menu.addAction(fitPageAct);
	menu.addAction(fitWidthAct);
	menu.addAction(zoomInAct);
	menu.addAction(zoomOutAct);	
	menu.addSeparator();
	
	bool sep = true;
	if (nextAct->isVisible())
	{
		sep = false;
		menu.addSeparator();
		menu.addAction(nextAct);
	}
		
	if (prevAct->isVisible())
	{
		if (sep)
			menu.addSeparator();
		menu.addAction(prevAct);
		sep = true;
	}
	
	menu.addSeparator();		
	menu.addAction(outlineAct);
	menu.addAction(statusAct);
	
	menu.exec(p);
}

XWReferenceWindow::XWReferenceWindow(XWDoc * docA, QWidget * parent)
	:QMainWindow(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	core = new XWRefCore(this, this);
	setCentralWidget(core);
	core->setDoc(docA);  	
	setWindowIcon(QIcon(":/images/xiuwen24.png"));
	
	outlineWin = new XWOutlineWindow(core, this); 
	outlineWin->setup();
	
	dock = new QDockWidget(tr("Outline"), this);
	dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	dock->setWidget(outlineWin);
	addDockWidget(Qt::LeftDockWidgetArea, dock);
	dock->setVisible(false);
	
	fileLabel = new QLabel(tr("File:"), this);
	QPalette newPalette = fileLabel->palette();
	newPalette.setBrush(QPalette::WindowText, Qt::black);
	fileLabel->setPalette(newPalette);
	statusBar()->addWidget(fileLabel, 1);
	
	authorLabel = new QLabel(tr("Author:"), this);	
	authorLabel->setPalette(newPalette);
	statusBar()->addWidget(authorLabel, 1);
	
	creationDateLabel = new QLabel(tr("Creation Date:"), this);
	creationDateLabel->setPalette(newPalette);
	statusBar()->addWidget(creationDateLabel, 1);
	
	lastModifiedDateLabel = new QLabel(tr("Last Modified Date:"), this);
	lastModifiedDateLabel->setPalette(newPalette);
	statusBar()->addWidget(lastModifiedDateLabel, 1);
	
	pagesLabel = new QLabel(this);
	pagesLabel->setPalette(newPalette);
	statusBar()->addWidget(pagesLabel, 1);
		
	statusBar()->setVisible(true);
	
/*	if (xwApp->hasSetting("RefWinBackgroundColor"))
	{
		QVariant v = xwApp->getSetting("RefWinBackgroundColor");
		QColor color = v.value<QColor>();
		QPalette newPalette = palette();
		newPalette.setBrush(QPalette::Window, color);
		setPalette(newPalette);
	}*/
			
	if (xwApp->hasSetting("RefWinOpacity"))
	{
		double op = xwApp->getSetting("RefWinOpacity").toDouble();
		if (op >= 0.0 && op <= 1.0)
			setWindowOpacity(op);
	}
	
	if (xwApp->hasSetting("RefWinPos"))
	{
		QPoint pos = xwApp->getSetting("RefWinPos").toPoint();
		move(pos);
	}
	
	if (xwApp->hasSetting("RefWinSize"))
	{
		QSize size = xwApp->getSetting("RefWinSize").toSize();
		resize(size);
	}
	else
	{
		int w = core->getMaxUnscaledPageW();
		int h = core->getMaxUnscaledPageH();
		resize(QSize(w + 2, h + 2));
	}
    
  connect(core, SIGNAL(currentChanged(int, int)), this, SLOT(setPageStatus(int, int)));
  connect(core, SIGNAL(authorChanged(const QString &)), this, SLOT(setAuthor(const QString &)));
  connect(core, SIGNAL(creationDateChanged(const QString &)), this, SLOT(setCreationDate(const QString &)));
  connect(core, SIGNAL(fileNameChanged(const QString &)), this, SLOT(setFileName(const QString &)));
  connect(core, SIGNAL(keywordsChanged(const QString &)), this, SLOT(setKeywords(const QString &)));
  connect(core, SIGNAL(lastModifiedDateChanged(const QString &)), this, SLOT(setLastModifiedDate(const QString &)));
  connect(core, SIGNAL(subjectChanged(const QString &)), this, SLOT(setSubject(const QString &)));
  connect(core, SIGNAL(titleChanged(const QString &)), this, SLOT(setTitle(const QString &)));
}

XWReferenceWindow::~XWReferenceWindow()
{
	xwApp->saveSetting("RefWinPos", pos());
	xwApp->saveSetting("RefWinSize", size());
}

void XWReferenceWindow::displayDest(XWString *namedDest)
{
	core->displayDest(namedDest);
}

void XWReferenceWindow::displayDest(XWLinkDest *dest)
{
	core->displayDest(dest);
}

void XWReferenceWindow::displayPage(int pg)
{
	core->displayPage(pg);
}

void XWReferenceWindow::setPageStatus(int cur, int num)
{
	QString p = QString(tr("Current Page: %1/%2")).arg(cur).arg(num);
	pagesLabel->setText(p);
}

void XWReferenceWindow::showOutline(bool e)
{
	dock->setVisible(e);
}

void XWReferenceWindow::showStatus(bool e)
{
	statusBar()->setVisible(e);
}

void XWReferenceWindow::setAuthor(const QString & a)
{
	QString author = QString(tr("Author: %1")).arg(a);
	authorLabel->setText(author);
}

void XWReferenceWindow::setCreationDate(const QString & d)
{
	QString date = QString(tr("Creation Date: %1")).arg(d);
	creationDateLabel->setText(date);
}

void XWReferenceWindow::setFileName(const QString & n)
{
	QString f = QString(tr("File: %1")).arg(n);
	fileLabel->setText(f);
}

void XWReferenceWindow::setKeywords(const QString & kws)
{
	QString tip = fileLabel->toolTip();
	tip += QString(tr("Keywords: %1\n")).arg(kws);
	fileLabel->setToolTip(tip);
}

void XWReferenceWindow::setLastModifiedDate(const QString & d)
{
	QString date = QString(tr("Last Modified Date: %1")).arg(d);
	lastModifiedDateLabel->setText(date);
}

void XWReferenceWindow::setSubject(const QString & s)
{
	QString tip = QString(tr("Subject: %1\n")).arg(s);
	fileLabel->setToolTip(tip);
}

void XWReferenceWindow::setTitle(const QString & t)
{
	setWindowTitle(t);
}

