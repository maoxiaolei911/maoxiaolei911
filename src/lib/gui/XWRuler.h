/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWRULER_H
#define XWRULER_H

#include <QWidget>
#include <QPoint>
#include "XWGlobal.h"

class QPainter;

#define RULER_BREADTH 20

class XW_GUI_EXPORT XWRuler : public QWidget
{
  Q_OBJECT

public:
  enum RulerType {Top,Left,Bottom,Right};

  XWRuler(XWRuler:: RulerType rulertypeA,QWidget * parent = 0);

  QSize minimumSizeHint() const;

public slots:
  void setCursorPos(const QPoint pos);
  void setMouseTrack(const bool track);
  void setOrigin(const double originA);
  void setUnit(const double unitA);
  void setZoom(const double zoomA);

protected:
  void mouseMoveEvent(QMouseEvent* event);

  void paintEvent(QPaintEvent* event);

private:
  void drawMarks(QPainter* painter,double step,double ticklen);
  void drawMark(QPainter* painter,double start,double end, int tickno,double step,double ticklen);
  void drawMousePosTick(QPainter* painter);

private:
  RulerType rulerType;
  double origin;
  double unit;
  double zoom;
  QPoint cursorPos;
  bool mouseTracking;
  bool drawText;
};

#endif //XWRULER_H
