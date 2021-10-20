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
#include "XWTikzShadowDialog.h"


XWTikzShadowWidget::XWTikzShadowWidget(QWidget * parent)
:QWidget(parent)
{
  buttonGroup = new QButtonGroup(this);
  connect(buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(buttonGroupClicked(QAbstractButton*)));

  QGridLayout *layout = new QGridLayout;
  layout->addWidget(createButton("general shadow"),0,0);
  layout->addWidget(createButton("drop shadow"),0,1);
  layout->addWidget(createButton("circular drop shadow"),0,2);
  layout->addWidget(createButton("circular glow"),0,3);

  layout->addWidget(createButton("copy shadow"),1,0);
  layout->addWidget(createButton("double copy shadow"),1,1);

  setLayout(layout);
}

QToolButton * XWTikzShadowWidget::createButton(const QString & txt)
{
  XWPDFDriver driver(100,100);
  XWTikzGraphic graphicA;
  QString src = QString("\\tikz \\node[%1,draw=blue,fill=blue!20,thick] at (1.76,1.7) {shadow};").arg(txt);
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

void XWTikzShadowWidget::buttonGroupClicked(QAbstractButton * button)
{
  QList<QAbstractButton *> buttons = buttonGroup->buttons();
  foreach (QAbstractButton *myButton, buttons)
  {
    if (myButton != button)
      myButton->setChecked(false);
  }

  QString text = button->text();
  int d = lookupPGFID(text);
  emit shadowSelected(d);
}

XWTikzShadowDialog::XWTikzShadowDialog(QWidget * parent)
:QDialog(parent),
shadow(-1)
{
  setWindowTitle(tr("Shadow"));
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  shadowWidget = new XWTikzShadowWidget(this);
  connect(shadowWidget, SIGNAL(shadowSelected(int)), this, SLOT(setShadow(int)));

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout * layout = new QVBoxLayout;
  layout->addWidget(shadowWidget);
  layout->addWidget(buttonBox);

  setLayout(layout);
}

int XWTikzShadowDialog::getShadow()
{
  return shadow;
}

void XWTikzShadowDialog::setShadow(int d)
{
  shadow = d;
}
