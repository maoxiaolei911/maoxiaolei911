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
#include "XWTikzPlotDialog.h"


XWTikzPlotHandlerWidget::XWTikzPlotHandlerWidget(QWidget * parent)
:QWidget(parent)
{
  buttonGroup = new QButtonGroup(this);
  connect(buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(buttonGroupClicked(QAbstractButton*)));

  QGridLayout *layout = new QGridLayout;
  layout->addWidget(createButton("smooth"),0,0);

  setLayout(layout);
}

void XWTikzPlotHandlerWidget::buttonGroupClicked(QAbstractButton * button)
{
  QList<QAbstractButton *> buttons = buttonGroup->buttons();
  foreach (QAbstractButton *myButton, buttons)
  {
    if (myButton != button)
      myButton->setChecked(false);
  }

  QString text = button->text();
  int d = lookupPGFID(text);
  emit handlerSelected(d);
}

QToolButton * XWTikzPlotHandlerWidget::createButton(const QString & txt)
{
  XWPDFDriver driver(100,100);
  XWTikzGraphic graphicA;
  QString src = QString("\\tikz \\draw plot[%1] coordinates {(0.2,0.2) (1,1) (1.5,0.5) (2.5,1) (3,3)};").arg(txt);
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

XWTikzPlotHandlerDialog::XWTikzPlotHandlerDialog(QWidget * parent)
:QDialog(parent),
handler(-1)
{
  setWindowTitle(tr("Plot handler"));
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  handlerWidget = new XWTikzPlotHandlerWidget(this);
  connect(handlerWidget, SIGNAL(handlerSelected(int)), this, SLOT(setHandler(int)));

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout * layout = new QVBoxLayout;
  layout->addWidget(handlerWidget);
  layout->addWidget(buttonBox);

  setLayout(layout);
}

int XWTikzPlotHandlerDialog::getHandler()
{
  return handler;
}

void XWTikzPlotHandlerDialog::setHandler(int d)
{
  handler = d;
}

XWTikzPlotMarkWidget::XWTikzPlotMarkWidget(QWidget * parent)
:QWidget(parent)
{
  buttonGroup = new QButtonGroup(this);
  connect(buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(buttonGroupClicked(QAbstractButton*)));

  QGridLayout *layout = new QGridLayout;
  layout->addWidget(createButton("ball"),0,0);

  setLayout(layout);
}

void XWTikzPlotMarkWidget::buttonGroupClicked(QAbstractButton * button)
{
  QList<QAbstractButton *> buttons = buttonGroup->buttons();
  foreach (QAbstractButton *myButton, buttons)
  {
    if (myButton != button)
      myButton->setChecked(false);
  }

  QString text = button->text();
  int d = lookupPGFID(text);
  emit markSelected(d);
}

QToolButton * XWTikzPlotMarkWidget::createButton(const QString & txt)
{
  XWPDFDriver driver(100,100);
  XWTikzGraphic graphicA;
  QString src = QString("\\tikz \\draw plot[mark=%1] coordinates {(0.2,0.2) (1,1) (1.5,0.5) (2.5,1) (3,3)};").arg(txt);
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

XWTikzPlotMarkDialog::XWTikzPlotMarkDialog(QWidget * parent)
:QDialog(parent),
mark(-1)
{
  setWindowTitle(tr("Plot mark"));
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  markWidget = new XWTikzPlotMarkWidget(this);
  connect(markWidget, SIGNAL(markSelected(int)), this, SLOT(setMark(int)));

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout * layout = new QVBoxLayout;
  layout->addWidget(markWidget);
  layout->addWidget(buttonBox);

  setLayout(layout);
}

int XWTikzPlotMarkDialog::getMark()
{
  return mark;
}

void XWTikzPlotMarkDialog::setMark(int d)
{
  mark = d;
}
