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
#include "XWTikzPatternDialog.h"

XWTikzPatternWidget::XWTikzPatternWidget(QWidget * parent)
:QWidget(parent)
{
  buttonGroup = new QButtonGroup(this);
  connect(buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(buttonGroupClicked(QAbstractButton*)));

  QGridLayout *layout = new QGridLayout;
  layout->addWidget(createButton("horizontal lines"),0,0);
  layout->addWidget(createButton("vertical lines"),0,1);
  layout->addWidget(createButton("north east lines"),0,2);
  layout->addWidget(createButton("north west lines"),0,3);

  layout->addWidget(createButton("grid"),1,0);
  layout->addWidget(createButton("crosshatch"),1,1);
  layout->addWidget(createButton("dots"),1,2);
  layout->addWidget(createButton("crosshatch dots"),1,3);

  layout->addWidget(createButton("fivepointed stars"),2,0);
  layout->addWidget(createButton("sixpointed stars"),2,1);
  layout->addWidget(createButton("bricks"),2,2);
  layout->addWidget(createButton("checkerboard"),2,3);

  layout->addWidget(createButton("checkerboard light gray"),3,0);
  layout->addWidget(createButton("horizontal lines light gray"),3,1);
  layout->addWidget(createButton("horizontal lines gray"),3,2);
  layout->addWidget(createButton("horizontal lines dark gray"),3,3);

  layout->addWidget(createButton("horizontal lines light blue"),4,0);
  layout->addWidget(createButton("horizontal lines dark blue"),4,1);
  layout->addWidget(createButton("crosshatch dots gray"),4,2);
  layout->addWidget(createButton("crosshatch dots light steel blue"),4,3);

  setLayout(layout);
}

void XWTikzPatternWidget::buttonGroupClicked(QAbstractButton * button)
{
  QList<QAbstractButton *> buttons = buttonGroup->buttons();
  foreach (QAbstractButton *myButton, buttons)
  {
    if (myButton != button)
      myButton->setChecked(false);
  }

  QString text = button->text();
  int d = lookupPGFID(text);
  emit patternSelected(d);
}

QToolButton * XWTikzPatternWidget::createButton(const QString & txt)
{
  XWPDFDriver driver(100,100);
  XWTikzGraphic graphicA;
  QString src = QString("\\tikz \\pattern[pattern=%1] (1.76,1.76) circle [radius=1.5cm];").arg(txt);
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

XWTikzPatternDialog::XWTikzPatternDialog(QWidget * parent)
:QDialog(parent),
pattern(-1)
{
  setWindowTitle(tr("Pattern"));
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  patternWidget = new XWTikzPatternWidget(this);
  connect(patternWidget, SIGNAL(patternSelected(int)), this, SLOT(setPattern(int)));

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout * layout = new QVBoxLayout;
  layout->addWidget(patternWidget);
  layout->addWidget(buttonBox);

  setLayout(layout);
}

int XWTikzPatternDialog::getPattern()
{
  return pattern;
}

void XWTikzPatternDialog::setPattern(int d)
{
  pattern = d;
}
