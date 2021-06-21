/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWTikzGraphic.h"
#include "XWTikzCanvas.h"
#include "XWTikzArea.h"

XWTikzArea::XWTikzArea(XWTikzGraphic * graphicA, int areaA,QWidget * parent)
:QScrollArea(parent)
{
  setBackgroundRole(QPalette::NoRole);
	setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	setWidgetResizable(false);

  canvas = new XWTikzCanvas(graphicA,areaA);
  setWidget(canvas);
}

void XWTikzArea::setZoomFactor(int f)
{
  canvas->setZoomFactor(0.01 * f);
}

XWTikzEdit::XWTikzEdit(XWTikzGraphic * graphicA,int areaA,QWidget * parent)
:QWidget(parent)
{
  slider = new QSlider(Qt::Horizontal);
  slider->setRange(100,800);
  slider->setTickInterval(20);

  area = new XWTikzArea(graphicA,areaA);

  QVBoxLayout * layout = new QVBoxLayout;
  layout->addWidget(slider);
  layout->addWidget(area);
  setLayout(layout);

  connect(slider, SIGNAL(valueChanged(int)), area, SLOT(setZoomFactor(int)));
}
