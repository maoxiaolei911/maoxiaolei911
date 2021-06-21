/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTIKZAREA_H
#define XWTIKZAREA_H

#include <QScrollArea>

class QSlider;

class XWTikzGraphic;
class XWTikzCanvas;

class XWTikzArea : public QScrollArea
{
  Q_OBJECT

public:
  XWTikzArea(XWTikzGraphic * graphicA, int areaA,QWidget * parent = 0);

  XWTikzCanvas * getCanvas() {return canvas;}

public slots:
  void setZoomFactor(int f);

protected:
  XWTikzCanvas * canvas;
};

class XWTikzEdit : public QWidget
{
  Q_OBJECT

public:
  XWTikzEdit(XWTikzGraphic * graphicA,int areaA,QWidget * parent = 0);

  XWTikzCanvas * getCanvas() {return area->getCanvas();}

private:
  QSlider * slider;
  XWTikzArea * area;
};

#endif //XWTIKZAREA_H
