/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include <QtGui>
#include "XWGuiCore.h"
#include "XWSlide.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif
 
XWSlide::XWSlide(XWGuiCore * coreA, QWidget * parent)
    :XWDocCanvas(coreA, parent)
{
	setBackgroundRole(QPalette::WindowText);
	playing = true;
	animating = false;
  timerInterval = 25;
    
  animateTimer.setSingleShot(false);    
  connect(&animateTimer, SIGNAL(timeout()), this, SLOT(animate()));
}

XWSlide::~XWSlide()
{
	core->setTile(-1, 16);
}

void XWSlide::start()
{	
  core->gotoNextPage(1, true);
  animating = true;
}

void XWSlide::stop()
{
	if (animateTimer.isActive())
		animateTimer.stop();
		
	animating = false;
	core->gotoNextPage(1, true);
}

#define PI (3.1415926535897932385)

void XWSlide::animate()
{
}

ExtendedSlide::ExtendedSlide(XWGuiCore * coreA, QWidget * parent)
    :XWSlide(coreA, parent)
{
	side = Left;
	curw = 0;
	curh = 0;
	x = 0;
	y = 0;
	delta = 2;
}

void ExtendedSlide::start()
{
	int i = qrand();
	i = (i % 5);
	side = (ExtendedBy)i;
  switch (side)
    {
       	case Left:
       		curh = height();
       		curw = delta;
       		x = 0;
       		y = 0;
       		break;
        		
       	case Right:
       		curh = height();
       		curw = delta;
       		x = width();
       		y = 0;
       		break;
        		
       	case Top:
       		curw = width();
       		curh = delta;
       		x = 0;
       		y = 0;
       		break;
        		
       	case Bottom:
       		curw = width();
       		curh = delta;
       		x = 0;
       		y = height();
       		break;
        		
       	case Middle:
       		curw = delta;
       		curh = height();
       		x = width() / 2;
       		y = 0;
       		break;
    }
	core->setCurrentPage(core->getPageNum() + 1);
	animateTimer.start(timerInterval);
}

void ExtendedSlide::animate()
{
	animating = true;
	switch (side)
    {
       	case Left:
       		curw += delta;
       		if (curw > width())
       			animating = false;
       		break;
        		
       	case Right:
       		x -= delta;
       		curw += delta;
       		if (curw > width() || x <= 0)
       			animating = false;
       		break;
        		
       	case Top:
       		curh += delta;
       		if (curh > height())
       			animating = false;
       		break;
        		
       	case Bottom:
       		y -= delta;
       		curh += delta;
       		if (curh > height() || y <= 0)
       			animating = false;
       		break;
        		
       	case Middle:
       		curw += delta;
       		curw += delta;
       		x -= delta;
       		if (curw > width() || x <= 0)
       			animating = false;
       		break;
    }
    
    update();
}

void ExtendedSlide::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	if (animating)
	{
		QRect r(x, y, curw, curh);
		core->redraw(&painter, r);
	}
	else
	{
		animateTimer.stop();
		QRect r(0, 0, width(), height());
		core->redraw(&painter, r);
	}
}

XWFadeSlide::XWFadeSlide(XWGuiCore * coreA, QWidget * parent)
   :XWSlide(coreA, parent)
{
	nextPage = false;
	alpha = 255;
}

XWFadeSlide::~XWFadeSlide()
{
}

void XWFadeSlide::start()
{
	alpha = 0;
	nextPage = false;
	animating = true;
	timerInterval = 50;
	animateTimer.start(timerInterval);
}

void XWFadeSlide::animate()
{
	animating = true;
    if (alpha == 255)
    {
    	nextPage = true;
    	core->setCurrentPage(core->getPageNum() + 1);
    }
    
    if (nextPage)
    {
    	if (alpha == 0)
    		animating = false;
    	else
    		alpha--;
    }
    else
    	alpha++;
    
    update();
}

void XWFadeSlide::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	QRect r(0, 0, width(), height());
	if (animating)
	{
		painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
		core->redraw(&painter, r);
		painter.fillRect(0, 0, width(), height(), QColor(0, 0, 0, alpha));		
	}
	else
	{
		animateTimer.stop();		
		core->redraw(&painter, r);
	}
}

XWRotateSlide::XWRotateSlide(XWGuiCore * coreA, QWidget * parent)
    :XWSlide(coreA, parent)
{
	nextPage = false;
	rotation = 0;
	timerInterval = 5;
}

XWRotateSlide::~XWRotateSlide()
{
}

void XWRotateSlide::start()
{
	nextPage = false;
	rotation = 0;
  animating = true;
  animateTimer.start(timerInterval);
}

void XWRotateSlide::animate()
{
	animating = true;
	if (!nextPage)
	{
		if (rotation >= 5760)
		{
			nextPage = true;	
			rotation = 5760;
			core->setCurrentPage(core->getPageNum() + 1);
		}
		else
			rotation += 32;
  }
  else
  {
  	if (rotation <= 0)
  	{
  		rotation = 0;
			animating = false;
		}
		else
			rotation -= 32;
  }
  
  update();
}

void XWRotateSlide::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	QRect r(0, 0, width(), height());
	if (animating)
	{
		core->redraw(&painter, r);
		painter.setBrush(QColor(0, 0, 0));
		painter.drawPie(r, 0, rotation);
		int w = 150;
		int h = 150;
		painter.drawPie(0, 0, w, h, 0, rotation);
		painter.drawPie(width() - w - 1, 0, w, h, 0, rotation);
		painter.drawPie(0, height() - h - 1, w, h, 0, rotation);
		painter.drawPie(width() - w - 1, height() - h - 1, w, h, 0, rotation);
	}
	else
	{
		animateTimer.stop();		
		core->redraw(&painter, r);
	}
}

XWInkSlide::XWInkSlide(XWGuiCore * coreA, QWidget * parent)
	:XWSlide(coreA, parent)
{
	nextPage = false;
}

XWInkSlide::~XWInkSlide()
{
}

void XWInkSlide::start()
{
	nextPage = false;
	pageImage = core->getImage(core->getPageNum());
	int w = pageImage.width();
	int h = pageImage.height();
	resultImage = pageImage;
	centers.clear();
	radius.clear();
	for (int i = 0; i < 20; i++)
	{
		int x = qrand() % w;
		int y = qrand() % h;
		int r = qrand() % 4;
		
		centers << QPoint(x, y);
		radius << r;
	}
	
	animateTimer.start(timerInterval);
}

void XWInkSlide::animate()
{
	animating = true;
	int w = width();
	int h = height();
	if (!nextPage)
	{
		QPainter painter(&resultImage);
		painter.setBrush(QColor(0, 0, 0));
		bool d = false;
		for (int i = 0; i < centers.size(); i++)
		{
			int r = radius[i];
			int x = centers.at(i).x() - r;
			int y = centers.at(i).y() - r;
			if (x < 0)
				x = 0;
				
			if (y < 0)
				y = 0;
				
			if (x + (2 * r) < w && y + (2 * r) < h)
			{
				painter.drawEllipse(x, y, 2 * r, 2 * r);
				r++;
				radius[i] = r;
				d = true;
			}
		}
		
		nextPage = !d;
		if (nextPage)
			core->setCurrentPage(core->getPageNum() + 1);
	}
	
	update();
}

void XWInkSlide::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	QRect r(0, 0, width(), height());
	if (animating)
	{
		if (!nextPage)
			painter.drawImage(0, 0, resultImage);		
		else
		{
			core->redraw(&painter, r);
			painter.setBrush(QColor(0, 0, 0));
			bool d = false;
			for (int i = 0; i < centers.size(); i++)
			{
				int r = radius[i];
				if (r > 0)
				{
					int x = centers.at(i).x() - r;
					int y = centers.at(i).y() - r;
					if (x < 0)
						x = 0;
				
					if (y < 0)
						y = 0;
				
					painter.drawEllipse(x, y, 2 * r, 2 * r);
					r--;
					if (r <= 0)
						r = 0;
					else
						d = true;
				
					radius[i] = r;
				}
			}
		
			animating = !d;
		}
	}
	else
	{
		animateTimer.stop();		
		core->redraw(&painter, r);
	}
}

