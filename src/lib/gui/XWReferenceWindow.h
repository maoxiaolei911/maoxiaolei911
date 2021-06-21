/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWREFERENCEWINDOW_H
#define XWREFERENCEWINDOW_H

#include <QMainWindow>
#include <QString>
#include "XWGuiCore.h"

class QAction;
class QDockWidget;
class QLabel;
class QStatusBar;
class XWOutlineWindow;
class XWReferenceWindow;

class XWRefCore : public XWGuiCore
{
	Q_OBJECT
	
public:       
    XWRefCore(XWReferenceWindow * refwinA, QWidget * parent = 0);
    
protected:
	void showContextMenu(const QPoint &p);
	
private:
	XWReferenceWindow * refWin;
	
	QAction * copyAct;
	QAction * markingAct;
	QAction * clrAct;
	
	QAction * fitPageAct;
    QAction * fitWidthAct;
    QAction * zoomInAct;
    QAction * zoomOutAct;
    
    QAction * nextAct;
    QAction * prevAct;
    
    QAction * outlineAct;
    QAction * statusAct;
};

class XWReferenceWindow : public QMainWindow
{
	Q_OBJECT
	
public:  
	XWReferenceWindow(XWDoc * docA, QWidget * parent = 0);
	virtual ~XWReferenceWindow();
	
	void displayDest(XWString *namedDest);
	void displayDest(XWLinkDest *dest);
	void displayPage(int pg);
	
public slots:
	void setPageStatus(int cur, int num);
	void showOutline(bool e);
	void showStatus(bool e);
	
private slots:
	void setAuthor(const QString & a);
	void setCreationDate(const QString & d);
	void setFileName(const QString & n);
	void setKeywords(const QString & kws);
	void setLastModifiedDate(const QString & d);
	void setSubject(const QString & s);
	void setTitle(const QString & t);
		
private:
	XWRefCore * core;
	QDockWidget * dock;
	XWOutlineWindow * outlineWin;
	
	QLabel * fileLabel;
	QLabel * authorLabel;
	QLabel * creationDateLabel;
	QLabel * lastModifiedDateLabel;
	QLabel * pagesLabel;
};

#endif // XWREFERENCEWINDOW_H

