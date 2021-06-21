/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWLOGMAINWINDOW_H
#define XWLOGMAINWINDOW_H

#include <QMainWindow>

class QAction;
class QMenu;
class QPlainTextEdit;

class XWLogMainWindow : public QMainWindow
{
	Q_OBJECT

public:
  XWLogMainWindow();
  XWLogMainWindow(const QString &fileName);
  
private slots:
	void open();
  
private:
	void init();
	
	void loadFile(const QString &fileName);
  
private:
	QPlainTextEdit * textEdit;
	QMenu *fileMenu;
	QAction *openAct;
	QAction *exitAct;	
};

#endif //XWLOGMAINWINDOW_H
