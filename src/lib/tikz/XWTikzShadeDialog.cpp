/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include "PGFKeyWord.h"
#include "XWPGFPool.h"
#include "XWPDFDriver.h"
#include "XWTikzGraphic.h"
#include "XWTikzState.h"
#include "XWTikzShadeDialog.h"

XWTikzShadeWidget::XWTikzShadeWidget(QWidget * parent)
:QWidget(parent)
{
  buttonGroup = new QButtonGroup(this);
  connect(buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(buttonGroupClicked(QAbstractButton*)));

  QGridLayout *layout = new QGridLayout;
  layout->addWidget(createButton("axis"),0,0);
  layout->addWidget(createButton("ball"),0,1);
  layout->addWidget(createButton("radial"),0,2);
  layout->addWidget(createButton("color wheel white center"),0,3);

  layout->addWidget(createButton("color wheel black center"),1,0);
  layout->addWidget(createButton("color wheel"),1,1);
  layout->addWidget(createButton("bilinear interpolation"),1,2);
  layout->addWidget(createButton("Mandelbrot set"),1,3);

  setLayout(layout);
}

void XWTikzShadeWidget::buttonGroupClicked(QAbstractButton * button)
{
  QList<QAbstractButton *> buttons = buttonGroup->buttons();
  foreach (QAbstractButton *myButton, buttons)
  {
    if (myButton != button)
      myButton->setChecked(false);
  }

  QString text = button->text();
  int d = lookupPGFID(text);
  emit shadeSelected(d);
}


QToolButton * XWTikzShadeWidget::createButton(const QString & txt)
{
  XWPDFDriver driver(100,100);
  XWTikzGraphic graphicA;
  QString src = QString("\\tikz \\shade[shading=%1] (1.76,1.76) circle [radius=1.5cm];").arg(txt);
  graphicA.scan(src);
  graphicA.doGraphic(&driver);

  QPixmap pix(100,100);
  QPainter painter(&pix);
  driver.display(&painter);

  QSize iconSize(100,100);
  QToolButton *button = new QToolButton;
  button->setIconSize(iconSize);
  button->setText(txt);
  button->setToolTip(txt);

  button->setIcon(QIcon(pix));
  button->setAutoRaise(true);
  return button;
}

XWTikzShadeDialog::XWTikzShadeDialog(QWidget * parent)
:QDialog(parent),
shade(-1)
{
  setWindowTitle(tr("Shade"));
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  shadeWidget = new XWTikzShadeWidget(this);
  connect(shadeWidget, SIGNAL(shadeSelected(int)), this, SLOT(setShade(int)));

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout * layout = new QVBoxLayout;
  layout->addWidget(shadeWidget);
  layout->addWidget(buttonBox);

  setLayout(layout);
}

int XWTikzShadeDialog::getShade()
{
  return shade;
}

void XWTikzShadeDialog::setShade(int d)
{
  shade = d;
}
