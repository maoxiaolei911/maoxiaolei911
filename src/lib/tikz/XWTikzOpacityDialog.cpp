/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWPDFDriver.h"
#include "XWTikzGraphic.h"
#include "XWTikzState.h"
#include "XWTikzOpacityDialog.h"

XWTikzDrawOpacityWidget::XWTikzDrawOpacityWidget(QWidget * parent)
:QWidget(parent),
 opacity(0.5)
{}

QSize XWTikzDrawOpacityWidget::minimumSizeHint() const
{
  return QSize(200, 200);
}

QSize XWTikzDrawOpacityWidget::sizeHint() const
{
  return QSize(200, 200);
}

void XWTikzDrawOpacityWidget::setOpacity(double o)
{
  opacity = o;
  update();
}

void XWTikzDrawOpacityWidget::paintEvent(QPaintEvent *)
{
#define XW_TIKZ_SRC_DRAWOPACITY "\
\\begin{tikzpicture}[line width=10pt]\
\\draw (1,1) -- (6,6);\
\\filldraw [fill=red,draw opacity=%1] (2,1) rectangle (5,6);\
\\end{tikzpicture}"

  QString src = QString(XW_TIKZ_SRC_DRAWOPACITY).arg(opacity);
  XWPDFDriver driver(200,200);
  XWTikzGraphic graphicA;
  graphicA.scan(src);
  graphicA.doGraphic(&driver);

  QPainter painter(this);
  driver.display(&painter);
}

XWTikzDrawOpacityDialog::XWTikzDrawOpacityDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowTitle(tr("Draw opacity"));
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  QLabel * label = new QLabel(tr("opacity"),this);
  opacitySpinBox = new QDoubleSpinBox(this);
  opacitySpinBox->setRange(0,1);
  opacitySpinBox->setSingleStep(0.02);
  opacitySpinBox->setValue(0.5);

  opacityWidget = new XWTikzDrawOpacityWidget(this);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);

  connect(opacitySpinBox, SIGNAL(valueChanged(double)), opacityWidget, SLOT(setOpacity(double)));

	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QHBoxLayout * toplayout = new QHBoxLayout;
  toplayout->addWidget(label);
  toplayout->addWidget(opacitySpinBox);

  QVBoxLayout * layout = new QVBoxLayout;
  layout->addLayout(toplayout);
  layout->addWidget(opacityWidget);
  layout->addWidget(buttonBox);

  setLayout(layout);
}

double XWTikzDrawOpacityDialog::getOpacity()
{
  return opacitySpinBox->value();
}

void XWTikzDrawOpacityDialog::setOpacity(double o)
{
  opacitySpinBox->setValue(o);
}

XWTikzFillOpacityWidget::XWTikzFillOpacityWidget(QWidget * parent)
:QWidget(parent),
 opacity(0.5)
{}

QSize XWTikzFillOpacityWidget::minimumSizeHint() const
{
  return QSize(200, 200);
}

QSize XWTikzFillOpacityWidget::sizeHint() const
{
  return QSize(200, 200);
}

void XWTikzFillOpacityWidget::setOpacity(double o)
{
  opacity = o;
  update();
}

void XWTikzFillOpacityWidget::paintEvent(QPaintEvent *)
{
#define XW_TIKZ_SRC_FILLOPACITY "\
\\begin{tikzpicture}[fill opacity=%1]\
\\filldraw[fill=red] (4.5,3.5) circle [radius=12mm];\
\\filldraw[fill=green] (3,4.37) circle [radius=12mm];\
\\filldraw[fill=blue] (3,2.63) circle [radius=12mm];\
\\end{tikzpicture}"

  QString src = QString(XW_TIKZ_SRC_FILLOPACITY).arg(opacity);
  XWPDFDriver driver(200,200);
  XWTikzGraphic graphicA;
  graphicA.scan(src);
  graphicA.doGraphic(&driver);

  QPainter painter(this);
  driver.display(&painter);
}

XWTikzFillOpacityDialog::XWTikzFillOpacityDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowTitle(tr("Fill opacity"));
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  QLabel * label = new QLabel(tr("opacity"),this);
  opacitySpinBox = new QDoubleSpinBox(this);
  opacitySpinBox->setRange(0,1);
  opacitySpinBox->setSingleStep(0.02);
  opacitySpinBox->setValue(0.5);

  opacityWidget = new XWTikzFillOpacityWidget(this);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);

  connect(opacitySpinBox, SIGNAL(valueChanged(double)), opacityWidget, SLOT(setOpacity(double)));

	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QHBoxLayout * toplayout = new QHBoxLayout;
  toplayout->addWidget(label);
  toplayout->addWidget(opacitySpinBox);

  QVBoxLayout * layout = new QVBoxLayout;
  layout->addLayout(toplayout);
  layout->addWidget(opacityWidget);
  layout->addWidget(buttonBox);

  setLayout(layout);
}

double XWTikzFillOpacityDialog::getOpacity()
{
  return opacitySpinBox->value();
}

void XWTikzFillOpacityDialog::setOpacity(double o)
{
  opacitySpinBox->setValue(o);
}
