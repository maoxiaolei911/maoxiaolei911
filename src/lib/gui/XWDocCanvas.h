/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWDOCCANVAS_H
#define XWDOCCANVAS_H

#include <QWidget>
#include <QTimer>
#include <QImage>
#include <QRect>

#include "XWGlobal.h"

class XWLinkAction;
class XWGuiCore;

class XW_GUI_EXPORT XWDocCanvas : public QWidget
{
	Q_OBJECT

public:
	friend class XWDocWindow;
	
	XWDocCanvas(XWGuiCore * coreA, QWidget * parent = 0);
  virtual ~XWDocCanvas();
  
  bool isPlaying() {return playing;}
  
public:
	QSize sizeHint() const;
  
public slots:	
	virtual void start() {}
	virtual void stop() {}
	
	
signals:
	void copyAvailable(bool yes);
	
	void fullScreenChanged(bool);	
	
protected:
	virtual void contextMenuEvent(QContextMenuEvent * e);
	
	virtual bool doLink(int wx, int wy, bool onlyIfNoSelection);
	
	virtual void endSelection(int wx, int wy);
	
	virtual void keyPressEvent(QKeyEvent *e);
	
	virtual void mouseMoveEvent(QMouseEvent *e);
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *e);
	
	virtual void paintEvent(QPaintEvent *e);
	
	virtual void startSelection(int wx, int wy);
  
protected:
	XWGuiCore *core;
	bool dragging;
	bool playing;
	bool panning;
		
	XWLinkAction * linkAction;
};

#endif //XWDOCCANVAS_H
