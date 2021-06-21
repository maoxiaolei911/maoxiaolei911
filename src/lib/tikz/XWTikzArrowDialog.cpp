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
#include "XWTikzArrowDialog.h"


XWTikzArrowDialog::XWTikzArrowDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowTitle(tr("Arrow"));
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  endlabel = new QLabel(tr("end arrow:"),this);
  endarrows = new QComboBox(this);
  startlabel = new QLabel(tr("start arrow:"),this);
  startarrows = new QComboBox(this);

  createArrows();

  QGridLayout * toplayout = new QGridLayout;
  toplayout->addWidget(endlabel,0,0);
  toplayout->addWidget(endarrows,0,1);
  toplayout->addWidget(startlabel,1,0);
  toplayout->addWidget(startarrows,1,1);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout * layout = new QVBoxLayout;
  layout->addLayout(toplayout);
  layout->addWidget(buttonBox);

  setLayout(layout);
}

int XWTikzArrowDialog::getEndArrow()
{
  int index = endarrows->currentIndex();
  return endarrows->itemData(index).toInt();
}

int XWTikzArrowDialog::getStartArrow()
{
  int index = startarrows->currentIndex();
  return startarrows->itemData(index).toInt();
}

void XWTikzArrowDialog::setEndArrow(int a)
{
  int index = endarrows->findData(a);
  if (index >= 0)
    endarrows->setCurrentIndex(index);
}

void XWTikzArrowDialog::setStartArrow(int a)
{
  int index = startarrows->findData(a);
  if (index >= 0)
    startarrows->setCurrentIndex(index);
}

void XWTikzArrowDialog::createArrows()
{
  endarrows->setItemData(0,0);
  endarrows->setItemText(0,tr("none"));
  startarrows->setItemData(0,0);
  startarrows->setItemText(0,tr("none"));

  createEndArrow(1,PGFstealth,"stealth");
  createStartArrow(1,PGFstealth,"stealth");
  createEndArrow(2,PGFto,"to");
  createStartArrow(2,PGFto,"to");
  createEndArrow(3,PGFtoreversed,"to reversed");
  createStartArrow(3,PGFtoreversed,"to reversed");
  createEndArrow(4,PGFlatex,"latex");
  createStartArrow(4,PGFlatex,"latex");
  createEndArrow(5,PGFbar,"bar");
  createStartArrow(5,PGFbar,"bar");
}

void XWTikzArrowDialog::createEndArrow(int index,int d,const QString & str)
{
  endarrows->setItemData(index,d);
  endarrows->setItemText(index,str);
  QIcon icon = createArrow(str);
  endarrows->setItemIcon(index,icon);
}

void XWTikzArrowDialog::createStartArrow(int index,int d,const QString & str)
{
  startarrows->setItemData(index,d);
  startarrows->setItemText(index,str);
  QIcon icon = createArrow(str);
  startarrows->setItemIcon(index,icon);
}

QIcon XWTikzArrowDialog::createArrow(const QString & arrow)
{
  XWPDFDriver driver(75,25);
  XWTikzGraphic graphicA;
  QString src = QString("\\tikz[%1-%2] \\draw (0.5,0.44) -- (2,0.44);").arg(arrow).arg(arrow);
  graphicA.scan(src);
  graphicA.doGraphic(&driver);

  QPixmap pix(75,25);
  QPainter painter(&pix);
  driver.display(&painter);

  QIcon icon(pix);
  return icon;
}
