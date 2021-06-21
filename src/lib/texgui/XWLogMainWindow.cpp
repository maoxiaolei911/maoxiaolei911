/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWLogMainWindow.h"

XWLogMainWindow::XWLogMainWindow()
{
	init();
	setWindowTitle(tr("Log"));
}

XWLogMainWindow::XWLogMainWindow(const QString &fileName)
{
	init();
  loadFile(fileName);
}

void XWLogMainWindow::open()
{
	QString path = QCoreApplication::applicationDirPath();
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open Log File"),
                                                 path,
                                                 tr("Log (*.log)"));
	if (!fileName.isEmpty())
		loadFile(fileName);
}

void XWLogMainWindow::init()
{
	setWindowIcon(QIcon(":/images/xiuwen24.png"));
	textEdit = new QPlainTextEdit;
  setCentralWidget(textEdit);
  
  openAct = new QAction(tr("&Open..."), this);
  openAct->setShortcuts(QKeySequence::Open);
  openAct->setStatusTip(tr("Open an existing file"));
  connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
  
  exitAct = new QAction(tr("E&xit"), this);
  exitAct->setShortcuts(QKeySequence::Quit);
  exitAct->setStatusTip(tr("Exit the application"));
  connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));
  
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(openAct);
  fileMenu->addAction(exitAct);
}

void XWLogMainWindow::loadFile(const QString &fileName)
{
	QFile file(fileName);
	if (!file.open(QFile::ReadOnly | QFile::Text)) 
	{
     QMessageBox::warning(this, tr("Log File"),
                          tr("Cannot read file %1:\n%2.")
                            .arg(fileName)
                            .arg(file.errorString()));
     return;
  }
  QTextStream in(&file);
  QApplication::setOverrideCursor(Qt::WaitCursor);
  textEdit->setPlainText(in.readAll());
  QApplication::restoreOverrideCursor();
  setWindowTitle(fileName);
}
