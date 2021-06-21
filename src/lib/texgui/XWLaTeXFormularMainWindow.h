/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWLATEXFORMULARMAINWINDOW_H
#define XWLATEXFORMULARMAINWINDOW_H

#include <QMainWindow>
#include "XWTeXGuiType.h"

class QAction;
class QActionGroup;
class QMenu;

class XWVisualLaTeXFormularEdit;
class XWLaTeXSymbolToolBox;
class XWLaTeXFormularToolBox;

class XW_TEXGUI_EXPORT XWLaTeXFormularMainWindow : public QMainWindow
{
  Q_OBJECT

public:
  XWLaTeXFormularMainWindow();

public slots:
  void open();

  void save();
  void saveAs();
  void setFormular(const QString & formular);

  void zoom(QAction * a);

signals:
  void insert(const QString & str);

protected:
  void closeEvent(QCloseEvent *event);

private:
  void createActions();
  void createDockWindows();
  void createMenus();

  QString getLocalPath();

private:
  XWVisualLaTeXFormularEdit * formularEdit;
  XWLaTeXSymbolToolBox * symbolToolBox;
  XWLaTeXFormularToolBox * formularToolBox;

  QMenu *fileMenu;
  QMenu *editMenu;
  QMenu *viewMenu;

  QAction * displayAct;
  QAction * inlineAct;
  QAction * insertAct;
  QAction * replaceAct;

  QAction * openAct;
  QAction * saveAct;
  QAction * saveAsAct;

  QAction * undoAct;
  QAction * redoAct;
  QAction * copyAct;
  QAction * pasteAct;
  QAction * clearAct;

  QActionGroup * zoomActs;

  QString fileName;
};

#endif //XWLATEXFORMULARMAINWINDOW_H
