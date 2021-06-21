/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWRuler.h"

XWRuler::XWRuler(XWRuler:: RulerType rulertypeA,QWidget * parent)
:QWidget(parent),
 rulerType(rulertypeA),
 origin(0.0),
 unit(1.0),
 zoom(1.0),
 mouseTracking(false),
 drawText(false)
{
  setMouseTracking(true);
}

QSize XWRuler::minimumSizeHint() const
{
  return QSize(RULER_BREADTH,RULER_BREADTH);
}

void XWRuler::setCursorPos(const QPoint pos)
{
  cursorPos = mapFromGlobal(pos);
  cursorPos += QPoint(RULER_BREADTH,RULER_BREADTH);
  update();
}

void XWRuler::setMouseTrack(const bool track)
{
  if (mouseTracking != track)
  {
    mouseTracking = track;
    update();
  }
}

void XWRuler::setOrigin(const double originA)
{
  if (origin != originA)
  {
    origin = originA;
    update();
  }
}

void XWRuler::setUnit(const double unitA)
{
  if (unit != unitA)
  {
    unit = unitA;
    update();
  }
}

void XWRuler::setZoom(const double zoomA)
{
  if (zoom != zoomA)
  {
    zoom = zoomA;
    update();
  }
}

void XWRuler::mouseMoveEvent(QMouseEvent* event)
{
  cursorPos = event->pos();
  update();  
  QWidget::mouseMoveEvent(event);  
}

void XWRuler::paintEvent(QPaintEvent* )
{
  QPainter painter(this);
  QPen pen(Qt::black,0);
  painter.setPen(pen);

  QRectF rulerrect = rect();
  painter.fillRect(rulerrect,QColor(236,233,216));

  double ticklen = rulerrect.height() / 4;
  drawText = false;
  drawMarks(&painter,25,ticklen);
  ticklen = 2 * ticklen;
  drawMarks(&painter,50,ticklen);
  ticklen = 2 * ticklen;
  drawText = true;
  drawMarks(&painter,100,ticklen);
  drawText = false;

  painter.setOpacity(0.4);
  drawMousePosTick(&painter);
}

void XWRuler::drawMarks(QPainter* painter,double step,double ticklen)
{
  step = step * unit * zoom;
  QRectF rulerrect = rect();
  double start,end;
  switch (rulerType)
  {
    case Left:
    case Right:
      start = rulerrect.top();
      end = rulerrect.bottom();
      break;

    default:
      start = rulerrect.left();
      end = rulerrect.right();
      break;
  } 

  if (origin >= start && origin <= end)
  {
    drawMark(painter,origin,end,0,step,ticklen);
    drawMark(painter,origin,start,0,-step,ticklen);
  }
  else if (origin < start)
  {
    int tickno = int((start - origin) / step);
    drawMark(painter,origin + step * tickno,end,tickno,step,ticklen);
  }    
  else
  {
    int tickno = int((origin - end) / step);
    drawMark(painter,origin - step * tickno,start,tickno,-step,ticklen);
  }   
}

void XWRuler::drawMark(QPainter* painter,double start,double end, int tickno,double step,double ticklen)
{
  double x1,y1,x2,y2,tx,ty;
  QRectF rulerrect = rect();
  for (double cur = start; (step < 0 ? cur >= start : cur <= end); cur += step)
  {
    switch (rulerType)
    {
      case Left:
        y1 = y2 = cur;
        x1 = rulerrect.left();
        x2 = ticklen + x1;
        tx = x2 + 7;
        ty = y1 + 1;
        break;

      case Right:
        y1 = y2 = cur;
        x1 = rulerrect.right();
        x2 = x1 - ticklen;
        tx = x2 - 7;
        ty = y1 + 1;
        break;

      case Bottom:
        x1 = x2 = cur;
        y1 = rulerrect.bottom();
        y2 = y1 - ticklen;
        tx = x1 + 1;
        ty = y2 - 7;
        break;

      default:
        x1 = x2 = cur;
        y1 = rulerrect.top();
        y2 = ticklen + y1;
        tx = x1 + 1;
        ty = y2 + 7;
        break;
    }

    painter->drawLine(QLineF(x1,y1,x2,y2));
    if (drawText)
    {
      QPainterPath txtPath;
      txtPath.addText(tx,ty,this->font(),QString::number(qAbs(int(step) * tickno++)));
      painter->drawPath(txtPath);
    }
  }
}

void XWRuler::drawMousePosTick(QPainter* painter)
{
  if (mouseTracking)
  {
    QRectF rulerrect = rect();
    QPoint start = cursorPos;
    QPoint end;
    switch (rulerType)
    {
      case Left:
      case Right:
        start.setX(rulerrect.left());
        end.setX(rulerrect.right());
        end.setY(start.y());
        break;

      default:
        start.setY(rulerrect.top());
        end.setX(start.x());
        end.setY(rulerrect.bottom());
        break;
    }

    painter->drawLine(start,end);
  }
}