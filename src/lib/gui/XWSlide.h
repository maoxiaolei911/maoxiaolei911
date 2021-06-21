/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWSLIDE_H
#define XWSLIDE_H

#include <QTimer>
#include <QImage>
#include <QList>
#include <QPoint>

#include "XWDocCanvas.h"

#define XW_SLIDE_TABLE_SIZE 2048

class XWSlide : public XWDocCanvas
{
    Q_OBJECT

public:
    XWSlide(XWGuiCore * coreA, QWidget * parent = 0);
    virtual ~XWSlide();
    
public slots:
    virtual void start();
    virtual void stop();
    
protected slots:
    virtual void animate();
    
protected:
	bool animating;
    int timerInterval;
    QTimer animateTimer;
};

class ExtendedSlide : public XWSlide
{
    Q_OBJECT

public:
	enum ExtendedBy
	{
		Left,
		Right,
		Top,
		Bottom,
		Middle
	};
	
    ExtendedSlide(XWGuiCore * coreA, QWidget * parent = 0);
    ~ExtendedSlide() {}
    
public slots:
    void start();
        
protected slots:
    void animate();
    
protected:
    void paintEvent(QPaintEvent *);

private:
    ExtendedBy side;
    int curw, curh, x, y, delta;
};

class XWFadeSlide: public XWSlide
{
    Q_OBJECT

public:
    XWFadeSlide(XWGuiCore * coreA, QWidget * parent = 0);
    ~XWFadeSlide();
    
public slots:
    void start();
    
protected slots:
    void animate();
    
protected:
    void paintEvent(QPaintEvent *);
    
private:
	bool nextPage;
    int alpha;
};

class XWRotateSlide: public XWSlide
{
    Q_OBJECT

public:
    XWRotateSlide(XWGuiCore * coreA, QWidget * parent = 0);
    ~XWRotateSlide();
    
public slots:
    void start();
        
protected slots:
    void animate();
    
protected:
    void paintEvent(QPaintEvent *);
    
private:
	bool nextPage;
  int rotation;
};

class XWInkSlide: public XWSlide
{
Q_OBJECT

public:
    XWInkSlide(XWGuiCore * coreA, QWidget * parent = 0);
    ~XWInkSlide();
    
public slots:
    void start();
        
protected slots:
    void animate();
    
    void paintEvent(QPaintEvent *);
    
private:
	bool nextPage;
	QImage pageImage;
	QImage resultImage;
	QList<QPoint> centers;
	QList<int> radius;
};

#endif // XWSLIDE_H
