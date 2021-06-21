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
#include "XWTikzFadingDialog.h"


XWTikzFadingWidget::XWTikzFadingWidget(QWidget * parent)
:QWidget(parent)
{
  buttonGroup = new QButtonGroup(this);
  connect(buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(buttonGroupClicked(QAbstractButton*)));

  QGridLayout *layout = new QGridLayout;
  layout->addWidget(createButton("east"),0,0);
  layout->addWidget(createButton("west"),0,1);
  layout->addWidget(createButton("north"),0,2);
  layout->addWidget(createButton("south"),0,3);

  layout->addWidget(createButton("circle with fuzzy edge 10 percent"),1,0);
  layout->addWidget(createButton("circle with fuzzy edge 15 percent"),1,1);
  layout->addWidget(createButton("circle with fuzzy edge 20 percent"),1,2);
  layout->addWidget(createButton("fuzzy ring 15 percent"),1,3);

  setLayout(layout);
}

void XWTikzFadingWidget::buttonGroupClicked(QAbstractButton * button)
{
  QList<QAbstractButton *> buttons = buttonGroup->buttons();
  foreach (QAbstractButton *myButton, buttons)
  {
    if (myButton != button)
      myButton->setChecked(false);
  }

  QString text = button->text();
  int d = lookupPGFID(text);
  emit fadingSelected(d);
}

QToolButton * XWTikzFadingWidget::createButton(const QString & txt)
{
  XWPDFDriver driver(100,100);
  XWTikzGraphic graphicA;
  QString src = QString("\\tikz \\fill[color=red,path fading=%1] (1.76,1.76) ellispe[x radius=15mm,y radius=10mm];").arg(txt);
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

  buttonGroup->addButton(button);
  
  return button;
}

XWTikzFadingDialog::XWTikzFadingDialog(QWidget * parent)
:QDialog(parent),
fading(-1)
{
  setWindowTitle(tr("Fading"));
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  fadingWidget = new XWTikzFadingWidget(this);
  connect(fadingWidget, SIGNAL(fadingSelected(int)), this, SLOT(setFading(int)));

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout * layout = new QVBoxLayout;
  layout->addWidget(fadingWidget);
  layout->addWidget(buttonBox);

  setLayout(layout);
}

int XWTikzFadingDialog::getFading()
{
  return fading;
}

void XWTikzFadingDialog::setFading(int d)
{
  fading = d;
}
