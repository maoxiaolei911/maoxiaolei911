/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWLaTeXSymbolWidget.h"
#include "XWLaTeXFormularWidget.h"
#include "XWVisualLaTeXFormularEdit.h"
#include "XWLaTeXFormularMainWindow.h"

XWLaTeXFormularMainWindow::XWLaTeXFormularMainWindow()
{
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowTitle(tr("LaTeX Formular"));

  formularEdit = new XWVisualLaTeXFormularEdit;
  setCentralWidget(formularEdit);

  createMenus();
  createActions();
  createDockWindows();
}

void XWLaTeXFormularMainWindow::open()
{
  QString path = getLocalPath();
  QString filename = QFileDialog::getOpenFileName(this,
		                                            tr("Open file"),
		                                            path,
		                                            tr("Files (*.tex)"));
	if (filename.isEmpty())
		return ;

  QFile file(filename);
  if (!file.open(QFile::ReadOnly | QFile::Text)) 
  {
    QMessageBox::warning(this, tr("LaTeX Formular"),
                        tr("Cannot read file %1:\n%2.")
                              .arg(filename)
                              .arg(file.errorString()));
         return;
  }

  QTextStream in(&file);
  QApplication::setOverrideCursor(Qt::WaitCursor);
  QString str = in.readAll();
  QApplication::restoreOverrideCursor();
  formularEdit->setText(str);
}

void XWLaTeXFormularMainWindow::save()
{
  if (fileName.isEmpty())
  {
    QString path = getLocalPath();
    QString files = tr("Files (*.tex)");
	  fileName = QFileDialog::getSaveFileName(this, tr("Save"), path, files);
	  if (fileName.isEmpty())
		  return ;
  }

  QFile file(fileName);
  if (!file.open(QFile::WriteOnly | QFile::Text)) 
  {
    QMessageBox::warning(this, tr("LaTeX Formular"),
                        tr("Cannot write file %1:\n%2.")
                              .arg(fileName)
                              .arg(file.errorString()));
         return;
  }

  QTextStream out(&file);
  QApplication::setOverrideCursor(Qt::WaitCursor);
  QString str = formularEdit->getFormular();
  out << str;
  QApplication::restoreOverrideCursor();
  statusBar()->showMessage(tr("Saved '%1'").arg(fileName), 2000);
}

void XWLaTeXFormularMainWindow::saveAs()
{
  QString path = getLocalPath();
  QString files = tr("Files (*.tex)");
	QString filename = QFileDialog::getSaveFileName(this, tr("Save as"), path, files);
	if (filename.isEmpty())
	  return ;

  QFile file(filename);
  if (!file.open(QFile::WriteOnly | QFile::Text)) 
  {
    QMessageBox::warning(this, tr("LaTeX Formular"),
                        tr("Cannot write file %1:\n%2.")
                              .arg(filename)
                              .arg(file.errorString()));
         return;
  }

  QTextStream out(&file);
  QApplication::setOverrideCursor(Qt::WaitCursor);
  QString str = formularEdit->getFormular();
  out << str;
  QApplication::restoreOverrideCursor();
}

void XWLaTeXFormularMainWindow::setFormular(const QString & formular)
{
  formularEdit->setText(formular);
}

void XWLaTeXFormularMainWindow::zoom(QAction * a)
{
  if (!a)
    return ;

  int i = a->data().toInt();
  formularEdit->zoom(i);
}

void XWLaTeXFormularMainWindow::closeEvent(QCloseEvent *event)
{
  QMessageBox::StandardButton ret = QMessageBox::warning(this, tr("Formular"),
                      tr("Do you want to insert formular into the document?"),
                      QMessageBox::Yes | QMessageBox::No);
  if (ret == QMessageBox::Yes)
  {
    QString str = formularEdit->getFormular();
    emit insert(str);
  }
  
  QMainWindow::closeEvent(event);
}

void XWLaTeXFormularMainWindow::createActions()
{
  displayAct = new  QAction(tr("Insert as display formular"), this);
  connect(displayAct, SIGNAL(triggered()), this, SLOT(display()));

  inlineAct = new  QAction(tr("Insert as inline formular"), this);
  connect(inlineAct, SIGNAL(triggered()), this, SLOT(inlineFormular()));

  insertAct = new  QAction(tr("Insert"), this);
  connect(insertAct, SIGNAL(triggered()), this, SLOT(insert()));

  replaceAct = new  QAction(tr("Replace"), this);
  connect(replaceAct, SIGNAL(triggered()), this, SLOT(replace()));

  openAct = new  QAction(tr("&Open..."), this);
  openAct->setShortcuts(QKeySequence::Open);
  connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

  saveAct = new  QAction(tr("&Save..."), this);
  saveAct->setShortcuts(QKeySequence::Save);
  connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

  saveAsAct = new  QAction(tr("Save &as..."), this);
  saveAsAct->setShortcuts(QKeySequence::SaveAs);
  connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

  fileMenu->addAction(displayAct);
  fileMenu->addAction(inlineAct);
  fileMenu->addSeparator();
  fileMenu->addAction(insertAct);
  fileMenu->addAction(replaceAct);

  fileMenu->addSeparator();
  fileMenu->addAction(openAct);
  fileMenu->addAction(saveAct);
  fileMenu->addAction(saveAsAct);

  undoAct = new  QAction(tr("&Undo..."), this);
  undoAct->setShortcuts(QKeySequence::Undo);
  connect(undoAct, SIGNAL(triggered()), formularEdit, SLOT(undo()));

  redoAct = new  QAction(tr("&Redo..."), this);
  redoAct->setShortcuts(QKeySequence::Redo);
  connect(redoAct, SIGNAL(triggered()), formularEdit, SLOT(redo()));

  pasteAct = new  QAction(tr("&Paste..."), this);
  pasteAct->setShortcuts(QKeySequence::Paste);
  connect(pasteAct, SIGNAL(triggered()), formularEdit, SLOT(paste()));

  copyAct = new  QAction(tr("&Copy..."), this);
  copyAct->setShortcuts(QKeySequence::Copy);
  connect(copyAct, SIGNAL(triggered()), formularEdit, SLOT(copy()));

  clearAct = new  QAction(tr("&Clear..."), this);
  connect(clearAct, SIGNAL(triggered()), formularEdit, SLOT(clear()));

  editMenu->addAction(undoAct);
  editMenu->addAction(redoAct);
  editMenu->addSeparator();
  editMenu->addAction(pasteAct);
  editMenu->addAction(copyAct);
  editMenu->addSeparator();
  editMenu->addAction(clearAct);

  QStringList zoomlist = XWVisualLaTeXFormularEdit::getZooms();
  zoomActs = new QActionGroup(this);
  connect(zoomActs, SIGNAL(triggered(QAction*)), this, SLOT(zoom(QAction*)));
  for (int i = 0; i < zoomlist.size(); i++)
  {
    QAction *action = new QAction(zoomlist[i], this);
    action->setData(i);
  	action->setCheckable(true);
    if (i == 9)
      action->setChecked(true);
    else
  	  action->setChecked(false);
  	zoomActs->addAction(action);
    viewMenu->addAction(action);
  }
}

void XWLaTeXFormularMainWindow::createDockWindows()
{
  viewMenu->addSeparator();

  QDockWidget *dock = new QDockWidget(tr("Symbol"), this);
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  symbolToolBox = new XWLaTeXSymbolToolBox;
  dock->setWidget(symbolToolBox);
  addDockWidget(Qt::LeftDockWidgetArea, dock);
  viewMenu->addAction(dock->toggleViewAction());
  connect(symbolToolBox, SIGNAL(symbolClicked(const QString & )), 
             formularEdit, SLOT(insertFormular(const QString & )));

  dock = new QDockWidget(tr("Formular"), this);
  formularToolBox = new XWLaTeXFormularToolBox;
  dock->setWidget(formularToolBox);
  addDockWidget(Qt::RightDockWidgetArea, dock);
  viewMenu->addAction(dock->toggleViewAction());
  connect(formularToolBox, SIGNAL(formularClicked(const QString & )), 
             formularEdit, SLOT(insertFormular(const QString & )));
}

void XWLaTeXFormularMainWindow::createMenus()
{
  fileMenu = menuBar()->addMenu(tr("&File"));
  editMenu = menuBar()->addMenu(tr("&Edit"));
  viewMenu = menuBar()->addMenu(tr("&View"));
}

QString XWLaTeXFormularMainWindow::getLocalPath()
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