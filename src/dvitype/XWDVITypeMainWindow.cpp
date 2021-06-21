/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWApplication.h"
#include "XWDVITypeMainWindow.h"
#include "XWDVITypeWindow.h"

XWDVITypeMainWindow::XWDVITypeMainWindow()
{
	QString title = xwApp->getProductName();
	setWindowTitle(title);
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
  
  mdiArea = new QMdiArea;
  mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setCentralWidget(mdiArea);
  connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(updateMenus()));
  windowMapper = new QSignalMapper(this);
  connect(windowMapper, SIGNAL(mapped(QWidget*)), this, SLOT(setActiveSubWindow(QWidget*)));
  
  createActions();
  createMenus();
}

void XWDVITypeMainWindow::open()
{
	QString path = getLocalPath();
	QString filename = QFileDialog::getOpenFileName(this, 
		                                            tr("Open file"), 
		                                            path, 
		                                            tr("Files (*.dvi)"));
	if (filename.isEmpty())
		return ;
		
	QMdiSubWindow *existing = findMdiChild(filename);
	if (existing) 
	{
     mdiArea->setActiveSubWindow(existing);
     return;
  }
  
  XWDVITypeWindow *child = createMdiChild();
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  child->loadFile(filename);
  QApplication::restoreOverrideCursor();
  child->show();
}

void XWDVITypeMainWindow::setActiveSubWindow(QWidget *window)
{
	if (!window)
     return;
  mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

void XWDVITypeMainWindow::showAbout()
{
	QString product = xwApp->getProductName();
	QString version = xwApp->getVersion();
	QString title = QString(tr("%1 %2")).arg(product).arg(version);
	QString infomation = QString(tr("<p>%1 is a DVI tools for dvi file detail.  "
	                                "Please visit <a href=\"http://www.xwtex.com\">xiuwen</a>"
	                                "for all products and services.</p>"))
	                              	.arg(product);
	QMessageBox msgBox(this);
	msgBox.setWindowTitle(product);
	msgBox.setText(title);
	msgBox.setInformativeText(infomation);
	QPixmap pm(QLatin1String(":/images/tex64.png"));
  if (!pm.isNull())
     msgBox.setIconPixmap(pm);
  msgBox.exec();
}

void XWDVITypeMainWindow::updateMenus()
{
	bool hasMdiChild = (activeMdiChild() != 0);
	closeAct->setEnabled(hasMdiChild);
  closeAllAct->setEnabled(hasMdiChild);
  tileAct->setEnabled(hasMdiChild);
  cascadeAct->setEnabled(hasMdiChild);
  nextAct->setEnabled(hasMdiChild);
  previousAct->setEnabled(hasMdiChild);
  separatorAct->setVisible(hasMdiChild);
}

void XWDVITypeMainWindow::updateWindowMenu()
{
	windowMenu->clear();
  windowMenu->addAction(closeAct);
  windowMenu->addAction(closeAllAct);
  windowMenu->addSeparator();
  windowMenu->addAction(tileAct);
  windowMenu->addAction(cascadeAct);
  windowMenu->addSeparator();
  windowMenu->addAction(nextAct);
  windowMenu->addAction(previousAct);
  windowMenu->addAction(separatorAct);
  QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
  separatorAct->setVisible(!windows.isEmpty());
  for (int i = 0; i < windows.size(); ++i)
  {
  	XWDVITypeWindow *child = qobject_cast<XWDVITypeWindow *>(windows.at(i)->widget());
  	QString t = child->getFileName();
  	QString text;
  	if (i < 9) 
       text = tr("&%1 %2").arg(i + 1).arg(t);
    else 
       text = tr("%1 %2").arg(i + 1).arg(t);
    QAction *action  = windowMenu->addAction(text);
    action->setCheckable(true);
    action ->setChecked(child == activeMdiChild());
    connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
    windowMapper->setMapping(action, windows.at(i));
  }
}

void XWDVITypeMainWindow::closeEvent(QCloseEvent *event)
{
	xwApp->cleanup();
	QMainWindow::closeEvent(event);
}

XWDVITypeWindow * XWDVITypeMainWindow::activeMdiChild()
{
	if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
     return qobject_cast<XWDVITypeWindow *>(activeSubWindow->widget());
  return 0;
}

void XWDVITypeMainWindow::createActions()
{
	openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
  openAct->setShortcuts(QKeySequence::Open);
  openAct->setStatusTip(tr("Open an existing file"));
  connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
  
  exitAct = new QAction(tr("&Quit"), this);
	exitAct->setShortcut(Qt::CTRL + Qt::Key_Q);
	connect(exitAct, SIGNAL(triggered()), xwApp, SLOT(quit()));
	
	aboutAct = new QAction(tr("About software"), this);
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(showAbout()));
	
	closeAct = new QAction(tr("Cl&ose"), this);
  closeAct->setStatusTip(tr("Close the active window"));
  connect(closeAct, SIGNAL(triggered()), mdiArea, SLOT(closeActiveSubWindow()));

  closeAllAct = new QAction(tr("Close &all"), this);
  closeAllAct->setStatusTip(tr("Close all the windows"));
  connect(closeAllAct, SIGNAL(triggered()), mdiArea, SLOT(closeAllSubWindows()));

  tileAct = new QAction(tr("&Tile"), this);
  tileAct->setStatusTip(tr("Tile the windows"));
  connect(tileAct, SIGNAL(triggered()), mdiArea, SLOT(tileSubWindows()));

  cascadeAct = new QAction(tr("&Cascade"), this);
  cascadeAct->setStatusTip(tr("Cascade the windows"));
  connect(cascadeAct, SIGNAL(triggered()), mdiArea, SLOT(cascadeSubWindows()));

  nextAct = new QAction(tr("Ne&xt"), this);
  nextAct->setShortcuts(QKeySequence::NextChild);
  nextAct->setStatusTip(tr("Move the focus to the next window"));
  connect(nextAct, SIGNAL(triggered()), mdiArea, SLOT(activateNextSubWindow()));

  previousAct = new QAction(tr("Pre&vious"), this);
  previousAct->setShortcuts(QKeySequence::PreviousChild);
  previousAct->setStatusTip(tr("Move the focus to the previous window"));
  connect(previousAct, SIGNAL(triggered()), mdiArea, SLOT(activatePreviousSubWindow()));

  separatorAct = new QAction(this);
  separatorAct->setSeparator(true);
}

XWDVITypeWindow * XWDVITypeMainWindow::createMdiChild()
{
	XWDVITypeWindow *child = new XWDVITypeWindow;	
  mdiArea->addSubWindow(child);

  return child;
}

void XWDVITypeMainWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(openAct);
	fileMenu->addAction(exitAct);
	windowMenu = menuBar()->addMenu(tr("&Window"));
  updateWindowMenu();
  connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));
  
  helpMenu = menuBar()->addMenu(tr("&About"));
  helpMenu->addAction(aboutAct);
}

QMdiSubWindow * XWDVITypeMainWindow::findMdiChild(const QString &fileName)
{
	foreach (QMdiSubWindow *window, mdiArea->subWindowList()) 
	{
      XWDVITypeWindow *mdiChild = qobject_cast<XWDVITypeWindow *>(window->widget());
      QString fn = mdiChild->getFileName();
      if (fn == fileName)
          return window;
  }
  return 0;
}

QString XWDVITypeMainWindow::getLocalPath()
{
	QString path = QCoreApplication::applicationDirPath();
	int idx = path.lastIndexOf(QChar('/'));
	if (idx > 0)
		path = path.left(idx + 1);
					
	if (path[path.length() - 1] != QChar('/'))
		path += QChar('/');
			
	path += "doc";					
	QDir d(path);
	if (!d.exists())
		d.mkdir(path);
		
	return path;
}
