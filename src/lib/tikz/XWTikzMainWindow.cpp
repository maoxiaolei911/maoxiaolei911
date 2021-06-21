/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QApplication>
#include <QtGui>
#include "XWTikzGraphic.h"
#include "XWTikzToolWidget.h"
#include "XWTikzArea.h"
#include "XWTikzCanvas.h"
#include "XWTikzOptionDialog.h"
#include "XWTikzMainWindow.h"

XWTikzMainWindow::XWTikzMainWindow()
{
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowTitle(tr("Tikz picture"));

  graphic = new XWTikzGraphic(this);

  graphicEdit = new XWTikzEdit(graphic,XW_TIKZ_S_GRAPHIC,this);
  setCentralWidget(graphicEdit);

  createMenus();
  createActions();
  createDockWindows();
}

QString XWTikzMainWindow::getText()
{
  return graphic->getText();
}

void XWTikzMainWindow::setText(const QString & str)
{
  graphic->scan(str);
}

void XWTikzMainWindow::closeEvent(QCloseEvent *event)
{
  QMessageBox::StandardButton ret = QMessageBox::warning(this, tr("Tikz"),
                      tr("Do you want to insert graphic into the document?"),
                      QMessageBox::Yes | QMessageBox::No);
  if (ret == QMessageBox::Yes)
  {
    QString str = graphic->getText();
    emit insert(str);
  }
  
  QMainWindow::closeEvent(event);
}

void XWTikzMainWindow::loadBg()
{
  QString path = graphic->getLocalPath();
  QString filename = QFileDialog::getOpenFileName(this,
		                                            tr("Load background image"),
		                                            path,
		                                            tr("Files (*.png *.xpm *.jpg *.bmp *.jpeg *.gif *.xbm *.pbm *.pgm *.tiff)"));

  if (filename.isEmpty())
		return ;

  getCanvas()->loadBgImage(filename);
}

void XWTikzMainWindow::open()
{
  QString path = graphic->getLocalPath();
  QString filename = QFileDialog::getOpenFileName(this,
		                                            tr("Open file"),
		                                            path,
		                                            tr("Files (*.tex)"));
	if (filename.isEmpty())
		return ;

  QFile file(filename);
  if (!file.open(QFile::ReadOnly | QFile::Text)) 
  {
    QMessageBox::warning(this, tr("LaTeX file"),
                        tr("Cannot read file %1:\n%2.")
                              .arg(filename)
                              .arg(file.errorString()));
         return;
  }

  QTextStream in(&file);
  QApplication::setOverrideCursor(Qt::WaitCursor);
  QString str = in.readAll();
  graphic->scan(str);
  QApplication::restoreOverrideCursor();
}

void XWTikzMainWindow::save()
{
  if (fileName.isEmpty())
  {
    QString path = graphic->getLocalPath();
    QString files = tr("Files (*.tex)");
	  fileName = QFileDialog::getSaveFileName(this, tr("Save"), path, files);
	  if (fileName.isEmpty())
		  return ;
  }

  QFile file(fileName);
  if (!file.open(QFile::WriteOnly | QFile::Text)) 
  {
    QMessageBox::warning(this, tr("Tikz picture"),
                        tr("Cannot write file %1:\n%2.")
                              .arg(fileName)
                              .arg(file.errorString()));
         return;
  }

  QTextStream out(&file);
  QApplication::setOverrideCursor(Qt::WaitCursor);
  QString str = graphic->getText();
  out << str;
  QApplication::restoreOverrideCursor();
  statusBar()->showMessage(tr("Saved '%1'").arg(fileName), 2000);
}

void XWTikzMainWindow::saveAs()
{
  QString path = graphic->getLocalPath();
  QString files = tr("Files (*.tex)");
	QString filename = QFileDialog::getSaveFileName(this, tr("Save as"), path, files);
	if (filename.isEmpty())
	  return ;

  QFile file(filename);
  if (!file.open(QFile::WriteOnly | QFile::Text)) 
  {
    QMessageBox::warning(this, tr("Tikz picture"),
                        tr("Cannot write file %1:\n%2.")
                              .arg(filename)
                              .arg(file.errorString()));
         return;
  }

  QTextStream out(&file);
  QApplication::setOverrideCursor(Qt::WaitCursor);
  QString str = graphic->getText();
  out << str;
  QApplication::restoreOverrideCursor();
}

void XWTikzMainWindow::saveAsImage()
{
  getCanvas()->saveAsImage();
}

void XWTikzMainWindow::setGridStep()
{
  double s = graphic->getGridStep();
  XWGridStepDialog dlg;
  dlg.setStep(XW_TIKZ_UNIT_PT,s);
  if (dlg.exec() != QDialog::Accepted)
		return ;

  int u = dlg.getUnit();
  s = dlg.getStep();
  switch (u)
  {
    case XW_TIKZ_UNIT_CM:
      s = 72.27 * s / 2.54;
      break;

    case XW_TIKZ_UNIT_MM:
      s = 72.27 * s / 25.4;
      break;

    default:
      break;
  }  

  graphic->setGridStep(s);
}

void XWTikzMainWindow::setSize()
{
  double x1 = graphic->getMinX();
  double x2 = graphic->getMaxX();
  double y1 = graphic->getMinY();
  double y2 = graphic->getMaxY();
  XWPictureSizeDialog dlg;
  dlg.setSize(XW_TIKZ_UNIT_PT,x1,y1,x2,y2);

  if (dlg.exec() != QDialog::Accepted)
		return ;

  int u = dlg.getUnit();
  x1 = dlg.getLeft();
  y1 = dlg.getBottom();
  x2 = dlg.getRight();
  y2 = dlg.getTop();
  switch (u)
  {
    case XW_TIKZ_UNIT_CM:
      x1 = 72.27 * x1 / 2.54;
      y1 = 72.27 * y1 / 2.54;
      x2 = 72.27 * x2 / 2.54;
      y2 = 72.27 * y2 / 2.54;
      break;

    case XW_TIKZ_UNIT_MM:
      x1 = 72.27 * x1 / 25.4;
      y1 = 72.27 * y1 / 25.4;
      x2 = 72.27 * x2 / 25.4;
      y2 = 72.27 * y2 / 25.4;
      break;

    default:
      break;
  }  

  graphic->setSize(x1,y1,x2,y2);
}

void XWTikzMainWindow::showBackground(bool e)
{
  getCanvas()->setShowBg(e);
}

void XWTikzMainWindow::showGrid(bool e)
{
  getCanvas()->setShowGrid(e);
}

void XWTikzMainWindow::createActions()
{
  newtikzpictureAct = new QAction(tr("New tikzpicture"), this);
  connect(newtikzpictureAct, SIGNAL(triggered()), graphic, SLOT(newTikzpicture()));

  newtikzAct = new QAction(tr("New tikz"), this);
  connect(newtikzAct, SIGNAL(triggered()), graphic, SLOT(newTikz()));

  loadBgAct = new QAction(tr("Load background image"), this);
  connect(loadBgAct, SIGNAL(triggered()), this, SLOT(loadBg()));

  openAct  = new QAction(tr("Load from file"), this);
  connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

  saveAct  = new QAction(tr("Save"), this);
  connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

  saveAsAct  = new QAction(tr("Save as"), this);
  connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

  saveAsImageAct  = new QAction(tr("Save as image"), this);
  connect(saveAsImageAct, SIGNAL(triggered()), this, SLOT(saveAsImage()));

  fileMenu->addAction(newtikzpictureAct);
  fileMenu->addAction(newtikzAct);
  fileMenu->addSeparator();

  fileMenu->addAction(loadBgAct);
  fileMenu->addSeparator();

  fileMenu->addAction(openAct);
  fileMenu->addSeparator();

  fileMenu->addAction(saveAct);
  fileMenu->addAction(saveAsAct);
  fileMenu->addAction(saveAsImageAct);

  sizeAct  = new QAction(tr("Set picture size"), this);
  connect(sizeAct, SIGNAL(triggered()), this, SLOT(setSize()));

  gridStepAct  = new QAction(tr("Set grid step"), this);
  connect(gridStepAct, SIGNAL(triggered()), this, SLOT(setGridStep()));

  undoAct  = new QAction(tr("Undo"), this);
  connect(undoAct, SIGNAL(triggered()), graphic, SLOT(undo()));

  redoAct  = new QAction(tr("Redo"), this);
  connect(redoAct, SIGNAL(triggered()), graphic, SLOT(redo()));

  editMenu->addAction(sizeAct);
  editMenu->addAction(gridStepAct);
  editMenu->addSeparator();

  editMenu->addAction(undoAct);
  editMenu->addAction(redoAct);

  showBgAct  = new QAction(tr("Show background image"), this);
  showBgAct->setCheckable(true);
  connect(showBgAct, SIGNAL(triggered(bool)), this, SLOT(showBackground(bool)));

  showGridAct  = new QAction(tr("Show grid"), this);
  showGridAct->setCheckable(true);
  connect(showGridAct, SIGNAL(triggered(bool)), this, SLOT(showGrid(bool)));

  viewMenu->addAction(showBgAct);
  viewMenu->addSeparator();
  viewMenu->addAction(showGridAct);
}

void XWTikzMainWindow::createDockWindows()
{
  viewMenu->addSeparator();

  QDockWidget *dock = new QDockWidget(tr("Tools"), this);
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  toolBox = new XWTikzToolBox(graphic,this);
  dock->setWidget(toolBox);
  addDockWidget(Qt::LeftDockWidgetArea, dock);
  viewMenu->addAction(dock->toggleViewAction());

  dock = new QDockWidget(tr("Operation"), this);
  operationEdit = new XWTikzEdit(graphic,XW_TIKZ_S_OPERATION,this);
  dock->setWidget(operationEdit);
  addDockWidget(Qt::RightDockWidgetArea, dock);
  viewMenu->addAction(dock->toggleViewAction());

  dock = new QDockWidget(tr("Path"), this);
  pathEdit = new XWTikzEdit(graphic,XW_TIKZ_S_PATH,this);
  dock->setWidget(pathEdit);
  addDockWidget(Qt::RightDockWidgetArea, dock);
  viewMenu->addAction(dock->toggleViewAction());

  dock = new QDockWidget(tr("Scope"), this);
  scopeEdit = new XWTikzEdit(graphic,XW_TIKZ_S_SCOPE,this);
  dock->setWidget(scopeEdit);
  addDockWidget(Qt::RightDockWidgetArea, dock);
  viewMenu->addAction(dock->toggleViewAction());
}

void XWTikzMainWindow::createMenus()
{
  fileMenu = menuBar()->addMenu(tr("&File"));
  editMenu = menuBar()->addMenu(tr("&Edit"));
  viewMenu = menuBar()->addMenu(tr("&View"));
}

XWTikzCanvas * XWTikzMainWindow::getCanvas()
{
  XWTikzCanvas * canvas = 0;
  switch (graphic->getCurrentScope())
  {
    case XW_TIKZ_S_GRAPHIC:
      canvas = graphicEdit->getCanvas();
      break;

    case XW_TIKZ_S_OPERATION:
      canvas = operationEdit->getCanvas();
      break;

    case XW_TIKZ_S_PATH:
      canvas = pathEdit->getCanvas();
      break;

    default:
      canvas = scopeEdit->getCanvas();
      break;
  }

  return canvas;
}
