/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTIKZMAINWINDOW_H
#define XWTIKZMAINWINDOW_H

#include <QMainWindow>
#include "XWTikzType.h"

class QAction;
class QMenu;

class XWTikzGraphic;
class XWTikzCanvas;
class XWTikzEdit;
class XWTikzToolBox;

class XW_TIKZ_EXPORT XWTikzMainWindow : public QMainWindow
{
  Q_OBJECT

public:
  XWTikzMainWindow();

  QString getText();

  void setText(const QString & str);

signals:
  void insert(const QString & str);

protected:
  void closeEvent(QCloseEvent *event);

private slots:
  void loadBg();
  
  void open();

  void save();
  void saveAs();
  void saveAsImage();
  void setGridStep();
  void setSize();
  void showBackground(bool e);
  void showGrid(bool e);

private:
  void createActions();
  void createDockWindows();
  void createMenus();

  XWTikzCanvas * getCanvas();

private:
  XWTikzGraphic * graphic;
  XWTikzEdit * graphicEdit;
  XWTikzEdit * operationEdit;
  XWTikzEdit * pathEdit;
  XWTikzEdit * scopeEdit;

  XWTikzToolBox * toolBox;  

  QMenu *fileMenu;
  QMenu *editMenu;
  QMenu *viewMenu;

  QAction * newtikzpictureAct;
  QAction * newtikzAct;

  QAction * loadBgAct;
  QAction * openAct;
  QAction * saveAct;
  QAction * saveAsAct;
  QAction * saveAsImageAct;

  QAction * sizeAct;
  QAction * gridStepAct;
  QAction * undoAct;
  QAction * redoAct;

  QAction * showBgAct;
  QAction * showGridAct;

  QString fileName;
};

#endif //XWTIKZMAINWINDOW_H
