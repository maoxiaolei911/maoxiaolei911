/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include "PGFKeyWord.h"
#include "XWPDFDriver.h"
#include "XWTikzGraphic.h"
#include "XWTikzState.h"
#include "XWTikzLineDialog.h"


XWTikzLineWidthDialog::XWTikzLineWidthDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowTitle(tr("Line width"));
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  label = new QLabel(tr("line width:"), this);
  widths = new QComboBox(this);

  createItem(0,PGFultrathin,"ultra thin");
  createItem(1,PGFverythin, "very thin");
  createItem(2,PGFthin, "thin");
  createItem(3,PGFsemithick,"semithick");
  createItem(4,PGFthick, "thick");
  createItem(5,PGFverythick,"very thick");
  createItem(6,PGFultrathick,"ultra thick");

  QHBoxLayout * toplayout = new QHBoxLayout;
  toplayout->addWidget(label);
  toplayout->addWidget(widths);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout * layout = new QVBoxLayout;
  layout->addLayout(toplayout);
  layout->addWidget(buttonBox);

  setLayout(layout);
}

int XWTikzLineWidthDialog::getLineWidth()
{
  int index = widths->currentIndex();
  return widths->itemData(index).toInt();
}

void XWTikzLineWidthDialog::setLineWidth(int i)
{
  int index = widths->findData(i);
  if (index >= 0)
    widths->setCurrentIndex(index);
}

void XWTikzLineWidthDialog::createItem(int index, int d, const QString & txt)
{
  widths->setItemData(index,d);
  widths->setItemText(index,txt);

  XWPDFDriver driver(75,25);
  XWTikzGraphic graphicA;
  QString src = QString("\\tikz \\draw[%1] (0.5,0.44) -- (2,0.44);").arg(txt);
  graphicA.scan(src);
  graphicA.doGraphic(&driver);

  QPixmap pix(75,25);
  QPainter painter(&pix);
  driver.display(&painter);

  QIcon icon(pix);
  widths->setItemIcon(index,icon);
}

XWTikzLineCapDialog::XWTikzLineCapDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowTitle(tr("Line cap"));
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  label = new QLabel(tr("line cap:"), this);
  caps = new QComboBox(this);
  createItem(0,PGFround,"round");
  createItem(1,PGFrect,"rect");
  createItem(2,PGFbutt,"butt");

  QHBoxLayout * toplayout = new QHBoxLayout;
  toplayout->addWidget(label);
  toplayout->addWidget(caps);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout * layout = new QVBoxLayout;
  layout->addLayout(toplayout);
  layout->addWidget(buttonBox);

  setLayout(layout);
}

int XWTikzLineCapDialog::getCap()
{
  int index = caps->currentIndex();
  return caps->itemData(index).toInt();
}

void XWTikzLineCapDialog::setCap(int c)
{
  int index = caps->findData(c);
  if (index >= 0)
    caps->setCurrentIndex(index);
}

void XWTikzLineCapDialog::createItem(int index, int d, const QString & txt)
{
  caps->setItemData(index,d);
  caps->setItemText(index,txt);

  XWPDFDriver driver(75,25);
  XWTikzGraphic graphicA;
  QString src = QString("\\tikz \\draw[line width=10pt,line cap=%1] (0.5,0.44) -- (2,0.44);").arg(txt);
  graphicA.scan(src);
  graphicA.doGraphic(&driver);

  QPixmap pix(75,25);
  QPainter painter(&pix);
  driver.display(&painter);

  QIcon icon(pix);
  caps->setItemIcon(index,icon);
}

XWTikzLineJoinDialog::XWTikzLineJoinDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowTitle(tr("Line join"));
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  label = new QLabel(tr("line join:"), this);
  joins = new QComboBox(this);

  createItem(0,PGFround,"round");
  createItem(1,PGFbevel,"bevel");
  createItem(2,PGFmiter,"miter");

  QHBoxLayout * toplayout = new QHBoxLayout;
  toplayout->addWidget(label);
  toplayout->addWidget(joins);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout * layout = new QVBoxLayout;
  layout->addLayout(toplayout);
  layout->addWidget(buttonBox);

  setLayout(layout);
}

int XWTikzLineJoinDialog::getJion()
{
  int index = joins->currentIndex();
  return joins->itemData(index).toInt();
}

void XWTikzLineJoinDialog::setJion(int c)
{
  int index = joins->findData(c);
  if (index >= 0)
    joins->setCurrentIndex(index);
}

void XWTikzLineJoinDialog::createItem(int index, int d, const QString & txt)
{
  joins->setItemData(index,d);
  joins->setItemText(index,txt);

  XWPDFDriver driver(75,25);
  XWTikzGraphic graphicA;
  QString src = QString("\\tikz \\draw[line width=10pt,line jion=%1] (0.5,0) -- ++(2,0.4) -- ++(-2,0.4);").arg(txt);
  graphicA.scan(src);
  graphicA.doGraphic(&driver);

  QPixmap pix(75,25);
  QPainter painter(&pix);
  driver.display(&painter);

  QIcon icon(pix);
  joins->setItemIcon(index,icon);
}

XWTikzDashDialog::XWTikzDashDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowTitle(tr("Dash"));
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  label = new QLabel(tr("dash:"),this);
  dashs = new QComboBox(this);
  createDash();
  dashs->setCurrentIndex(0);

  QHBoxLayout * toplayout = new QHBoxLayout;
  toplayout->addWidget(label);
  toplayout->addWidget(dashs);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout * layout = new QVBoxLayout;
  layout->addLayout(toplayout);
  layout->addWidget(buttonBox);

  setLayout(layout);
}

int XWTikzDashDialog::getDash()
{
  int index = dashs->currentIndex();
  return dashs->itemData(index).toInt();
}

void XWTikzDashDialog::setDash(int d)
{
  int index = dashs->findData(d);
  if (index >= 0)
    dashs->setCurrentIndex(index);
}

void XWTikzDashDialog::createDash()
{
  createDash(0,PGFsolid,"solid");
  createDash(1,PGFdotted,"dotted");
  createDash(2,PGFdenselydotted,"densely dotted");
  createDash(3,PGFlooselydotted,"loosely dotted");
  createDash(4,PGFdashed,"dashed");
  createDash(5,PGFdenselydashed,"densely dashed");
  createDash(6,PGFlooselydashed,"loosely dashed");
  createDash(7,PGFdashdotted,"dashdotted");
  createDash(8,PGFdenselydashdotted,"densely dashdotted");
  createDash(9,PGFlooselydashdotted,"loosely dashdotted");
  createDash(10,PGFdashdotdotted,"dashdotdotted");
  createDash(11,PGFdenselydashdotdotted,"densely dashdotdotted");
  createDash(12,PGFlooselydashdotdotted,"loosely dashdotdotted");
}

void XWTikzDashDialog::createDash(int index,int d,const QString & str)
{
  dashs->setItemData(index,d);
  dashs->setItemText(index,str);

  XWPDFDriver driver(75,25);
  XWTikzGraphic graphicA;
  QString src = QString("\\tikz \\draw[%1] (0.5,0.44) -- (2,0.44);").arg(str);
  graphicA.scan(src);
  graphicA.doGraphic(&driver);

  QPixmap pix(75,25);
  QPainter painter(&pix);
  driver.display(&painter);

  QIcon icon(pix);
  dashs->setItemIcon(index,icon);
}
