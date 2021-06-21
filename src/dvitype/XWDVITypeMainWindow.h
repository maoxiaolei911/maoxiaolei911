/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWDVITYPEMAINWINDOW_H
#define XWDVITYPEMAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QUrl>

class QAction;
class QActionGroup;
class QMdiArea;
class QMdiSubWindow;
class QSignalMapper;
class XWDVITypeWindow;

class XWDVITypeMainWindow : public QMainWindow
{
	Q_OBJECT
	
public:  
	XWDVITypeMainWindow();
	~XWDVITypeMainWindow() {}
	
public slots:
	void open();
	
	void setActiveSubWindow(QWidget *window);
	void showAbout();
	
	void updateMenus();
  void updateWindowMenu();
	
protected:
	void closeEvent(QCloseEvent *event);
	
private:
	XWDVITypeWindow *activeMdiChild();
	void createActions();
	XWDVITypeWindow *createMdiChild();	
	void createMenus();
	
	QMdiSubWindow *findMdiChild(const QString &fileName);
	
	QString getLocalPath();
	
private:
	QMdiArea *mdiArea;
  QSignalMapper *windowMapper;
  
  QMenu *fileMenu;
  QMenu *windowMenu;
  QMenu *helpMenu;
  
  QAction *openAct;
  QAction * exitAct;
  QAction * aboutAct;
	
	QAction *closeAct;
  QAction *closeAllAct;
  QAction *tileAct;
  QAction *cascadeAct;
  QAction *nextAct;
  QAction *previousAct;
  QAction *separatorAct;
};

#endif //XWDVITYPEMAINWINDOW_H
