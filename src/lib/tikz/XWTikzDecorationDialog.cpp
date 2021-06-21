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
#include "XWTikzDecorationDialog.h"


XWTikzDecorationWidget::XWTikzDecorationWidget(QWidget * parent)
:QWidget(parent)
{
  buttonGroup = new QButtonGroup(this);
  connect(buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(buttonGroupClicked(QAbstractButton*)));

  QGridLayout *layout = new QGridLayout;
  layout->addWidget(createButton("line to"),0,0);
  layout->addWidget(createButton("move to"),0,1);
  layout->addWidget(createButton("curve to"),0,2);

  layout->addWidget(createButton("zigzag"),1,0);
  layout->addWidget(createButton("saw"),1,1);
  layout->addWidget(createButton("random steps"),1,2);
  layout->addWidget(createButton("straight zigzag"),1,3);

  layout->addWidget(createButton("bent"),2,0);
  layout->addWidget(createButton("snake"),2,1);
  layout->addWidget(createButton("coil"),2,2);
  layout->addWidget(createButton("bumps"),2,3);

  setLayout(layout);
}

void XWTikzDecorationWidget::buttonGroupClicked(QAbstractButton * button)
{
  QList<QAbstractButton *> buttons = buttonGroup->buttons();
  foreach (QAbstractButton *myButton, buttons)
  {
    if (myButton != button)
      myButton->setChecked(false);
  }

  QString text = button->text();
  int d = lookupPGFID(text);
  emit decorationSelected(d);
}

QToolButton * XWTikzDecorationWidget::createButton(const QString & txt)
{
  XWPDFDriver driver(100,100);
  XWTikzGraphic graphicA;
  QString src = QString("\\tikz \\draw[decorate,decoration=%1] (0.18,0.18) -- (3.35,3.35);").arg(txt);
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

XWTikzDecorationDialog::XWTikzDecorationDialog(QWidget * parent)
:QDialog(parent),
decoration(-1)
{
  setWindowTitle(tr("Decoration"));
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  decorationWidget = new XWTikzDecorationWidget(this);
  connect(decorationWidget, SIGNAL(decorationSelected(int)), this, SLOT(setDecoration(int)));

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout * layout = new QVBoxLayout;
  layout->addWidget(decorationWidget);
  layout->addWidget(buttonBox);

  setLayout(layout);
}

int XWTikzDecorationDialog::getDecoration()
{
  return decoration;
}

void XWTikzDecorationDialog::setDecoration(int d)
{
  decoration = d;
}
