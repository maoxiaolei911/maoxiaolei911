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
#include "XWTikzShapeDialog.h"


XWTikzAnchorDialog::XWTikzAnchorDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowTitle(tr("Anchor"));
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  label = new QLabel(tr("anchor:"),this);
  anchors = new QComboBox(this);
  createAnchors();
  anchors->setCurrentIndex(0);

  QHBoxLayout * toplayout = new QHBoxLayout;
  toplayout->addWidget(label);
  toplayout->addWidget(anchors);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout * layout = new QVBoxLayout;
  layout->addLayout(toplayout);
  layout->addWidget(buttonBox);

  setLayout(layout);
}

int XWTikzAnchorDialog::getAnchor()
{
  int index = anchors->currentIndex();
  return anchors->itemData(index).toInt();
}

void XWTikzAnchorDialog::setAnchor(int d)
{
  int index = anchors->findData(d);
  if (index >= 0)
    anchors->setCurrentIndex(index);
}

void XWTikzAnchorDialog::createAnchors()
{
  createAnchor(0,PGFmid,tr("mid"));
  createAnchor(1,PGFbase,tr("base"));
  createAnchor(2,PGFnorth,tr("north"));
  createAnchor(3,PGFsouth,tr("south"));
  createAnchor(4,PGFwest,tr("west"));
  createAnchor(5,PGFeast,tr("east"));
  createAnchor(6,PGFmidwest,tr("mid west"));
  createAnchor(7,PGFmideast,tr("mid east"));
  createAnchor(8,PGFbasewest,tr("base west"));
  createAnchor(9,PGFbaseeast,tr("base east"));
  createAnchor(10,PGFnorthwest,tr("north west"));
  createAnchor(11,PGFsouthwest,tr("south west"));
  createAnchor(12,PGFnortheast,tr("north east"));
  createAnchor(13,PGFsoutheast,tr("south east"));
}

void XWTikzAnchorDialog::createAnchor(int index, int a, const QString & str)
{
  anchors->setItemData(index,a);
  anchors->setItemText(index,str);
}

XWTikzShapeWidget::XWTikzShapeWidget(QWidget * parent)
:QWidget(parent)
{
  buttonGroup = new QButtonGroup(this);
  connect(buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(buttonGroupClicked(QAbstractButton*)));

  QGridLayout *layout = new QGridLayout;
  layout->addWidget(createButton("rectangle"),0,0);
  layout->addWidget(createButton("circle"),0,1);
  layout->addWidget(createButton("ellipse"),0,2);
  layout->addWidget(createButton("diamond"),0,3);

  layout->addWidget(createButton("star"),1,0);
  layout->addWidget(createButton("regular polygon"),1,1);
  layout->addWidget(createButton("trapezium"),1,2);
  layout->addWidget(createButton("semicircle"),1,3);

  layout->addWidget(createButton("isosceles triangle"),2,0);
  layout->addWidget(createButton("kite"),2,1);
  layout->addWidget(createButton("dart"),2,2);
  layout->addWidget(createButton("circular sector"),2,3);

  layout->addWidget(createButton("cylinder"),3,0);

  layout->addWidget(createButton("forbidden sign"),4,0);
  layout->addWidget(createButton("correct forbidden sign"),4,1);
  layout->addWidget(createButton("starburst"),4,2);
  layout->addWidget(createButton("cloud"),4,3);

  layout->addWidget(createButton("signal"),5,0);
  layout->addWidget(createButton("tape"),5,1);
  layout->addWidget(createButton("magnifying glass"),5,2);
  layout->addWidget(createButton("magnetic tape"),5,3);

  layout->addWidget(createButton("ellipse callout"),6,0);
  layout->addWidget(createButton("rectangle callout"),6,1);
  layout->addWidget(createButton("cloud callout"),6,2);

  layout->addWidget(createButton("single arrow"),7,0);
  layout->addWidget(createButton("double arrow"),7,1);
  layout->addWidget(createButton("arrow box"),7,2);

  layout->addWidget(createButton("cross out"),8,0);
  layout->addWidget(createButton("strike out"),8,1);
  layout->addWidget(createButton("rounded rectangle"),8,2);
  layout->addWidget(createButton("chamfered rectangle"),8,3);

  setLayout(layout);
}

QToolButton * XWTikzShapeWidget::createButton(const QString & txt)
{
  XWPDFDriver driver(100,100);
  XWTikzGraphic graphicA;
  QString src = QString("\\tikz \\filldraw (1.76,1.7) node[shape=%1,draw=black,fill=red!20] {\\Large{%1}};").arg(txt).arg(txt);
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

void XWTikzShapeWidget::buttonGroupClicked(QAbstractButton * button)
{
  QList<QAbstractButton *> buttons = buttonGroup->buttons();
  foreach (QAbstractButton *myButton, buttons)
  {
    if (myButton != button)
      myButton->setChecked(false);
  }

  QString text = button->text();
  int d = lookupPGFID(text);
  emit shapeSelected(d);
}

XWTikzShapeDialog::XWTikzShapeDialog(QWidget * parent)
:QDialog(parent),
shape(-1)
{
  setWindowTitle(tr("Shape"));
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  shapeWidget = new XWTikzShapeWidget(this);
  connect(shapeWidget, SIGNAL(shapeSelected(int)), this, SLOT(setShape(int)));

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout * layout = new QVBoxLayout;
  layout->addWidget(shapeWidget);
  layout->addWidget(buttonBox);

  setLayout(layout);
}

int XWTikzShapeDialog::getShape()
{
  return shape;
}

void XWTikzShapeDialog::setShape(int d)
{
  shape = d;
}

XWTikzCircuiteeWidget::XWTikzCircuiteeWidget(const QString & styleA,QWidget * parent)
:QWidget(parent),
style(styleA)
{
  buttonGroup = new QButtonGroup(this);
  connect(buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(buttonGroupClicked(QAbstractButton*)));

  QGridLayout *layout = new QGridLayout;
  layout->addWidget(createButton("resistor"),0,0);
  layout->addWidget(createButton("inductor"),0,1);
  layout->addWidget(createButton("capacitor"),0,2);
  layout->addWidget(createButton("contact"),0,3);

  layout->addWidget(createButton("ground"),2,0);
  layout->addWidget(createButton("battery"),2,1);
  layout->addWidget(createButton("diode"),2,2);
  layout->addWidget(createButton("Zener diode"),2,3);

  layout->addWidget(createButton("Schottky diode"),3,0);
  layout->addWidget(createButton("tunnel diode"),3,1);
  layout->addWidget(createButton("backward diode"),3,2);
  layout->addWidget(createButton("breakdown diode"),3,3);

  layout->addWidget(createButton("bulb"),4,0);
  layout->addWidget(createButton("current source"),4,1);
  layout->addWidget(createButton("voltage source"),4,2);
  layout->addWidget(createButton("current direction"),4,3);

  layout->addWidget(createButton("current direction'"),5,0);
  layout->addWidget(createButton("make contact"),5,1);
  layout->addWidget(createButton("break contact"),5,2);
  layout->addWidget(createButton("amperemeter"),5,3);

  layout->addWidget(createButton("voltmeter"),6,0);
  layout->addWidget(createButton("ohmmeter"),6,1);
  layout->addWidget(createButton("ac source"),6,2);
  layout->addWidget(createButton("dc source"),6,3);

  setLayout(layout);
}

QToolButton * XWTikzCircuiteeWidget::createButton(const QString & txt)
{
  XWPDFDriver driver(100,100);
  XWTikzGraphic graphicA;
  QString src = QString("\\tikz[%1] \\node[%2] at (1.76,1.7);").arg(style).arg(txt);
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

void XWTikzCircuiteeWidget::buttonGroupClicked(QAbstractButton * button)
{
  QList<QAbstractButton *> buttons = buttonGroup->buttons();
  foreach (QAbstractButton *myButton, buttons)
  {
    if (myButton != button)
      myButton->setChecked(false);
  }

  QString text = button->text();
  int d = lookupPGFID(text);
  emit shapeSelected(d);
}

XWTikzCircuiteeDialog::XWTikzCircuiteeDialog(int styleA, QWidget * parent)
:QDialog(parent),
shape(-1)
{
  setWindowTitle(tr("electrical engineering symbol"));
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  QString style = getPGFString(styleA);

  shapeWidget = new XWTikzCircuiteeWidget(style,this);
  connect(shapeWidget, SIGNAL(shapeSelected(int)), this, SLOT(setShape(int)));

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout * layout = new QVBoxLayout;
  layout->addWidget(shapeWidget);
  layout->addWidget(buttonBox);

  setLayout(layout);
}

int XWTikzCircuiteeDialog::getShape()
{
  return shape;
}

void XWTikzCircuiteeDialog::setShape(int d)
{
  shape = d;
}

XWTikzCircuitLogicWidget::XWTikzCircuitLogicWidget(const QString & styleA,QWidget * parent)
:QWidget(parent),
style(styleA)
{
  buttonGroup = new QButtonGroup(this);
  connect(buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(buttonGroupClicked(QAbstractButton*)));

  QGridLayout *layout = new QGridLayout;
  layout->addWidget(createButton("and gate"),0,0);
  layout->addWidget(createButton("nand gate"),0,1);
  layout->addWidget(createButton("or gate"),0,2);

  layout->addWidget(createButton("nor gate"),1,0);
  layout->addWidget(createButton("xor gate"),1,1);
  layout->addWidget(createButton("xnor gate"),1,2);

  layout->addWidget(createButton("not gate"),2,0);
  layout->addWidget(createButton("buffer gate"),2,1);

  setLayout(layout);
}

QToolButton * XWTikzCircuitLogicWidget::createButton(const QString & txt)
{
  XWPDFDriver driver(100,100);
  XWTikzGraphic graphicA;
  QString src = QString("\\tikz[%1] \\node[%2] at (1.76,1.7);").arg(style).arg(txt);
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

void XWTikzCircuitLogicWidget::buttonGroupClicked(QAbstractButton * button)
{
  QList<QAbstractButton *> buttons = buttonGroup->buttons();
  foreach (QAbstractButton *myButton, buttons)
  {
    if (myButton != button)
      myButton->setChecked(false);
  }

  QString text = button->text();
  int d = lookupPGFID(text);
  emit shapeSelected(d);
}

XWTikzCircuitLogicDialog::XWTikzCircuitLogicDialog(int styleA, QWidget * parent)
:QDialog(parent),
shape(-1)
{
  setWindowTitle(tr("logical circuit symbol"));
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  QString style = getPGFString(styleA);

  shapeWidget = new XWTikzCircuitLogicWidget(style,this);
  connect(shapeWidget, SIGNAL(shapeSelected(int)), this, SLOT(setShape(int)));

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout * layout = new QVBoxLayout;
  layout->addWidget(shapeWidget);
  layout->addWidget(buttonBox);

  setLayout(layout);
}

int XWTikzCircuitLogicDialog::getShape()
{
  return shape;
}

void XWTikzCircuitLogicDialog::setShape(int d)
{
  shape = d;
}
