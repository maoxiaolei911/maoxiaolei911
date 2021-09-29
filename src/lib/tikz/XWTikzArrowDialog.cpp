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

  createArrow(1,PGFstealth,"stealth");
  createArrow(2,PGFto,"to");
  createArrow(3,PGFtoreversed,"to reversed");
  createArrow(4,PGFlatex,"latex");
  createArrow(5,PGFbar,"bar");
  createArrow(6,PGFbar,"[");
  createArrow(7,PGFbar,"]");
  createArrow(8,PGFbar,"(");
  createArrow(9,PGFbar,")");
  createArrow(10,PGFbar,"angle 90");
  createArrow(11,PGFbar,"angle 90 reversed");
  createArrow(12,PGFbar,"angle 60");
  createArrow(13,PGFbar,"angle 60 reversed");
  createArrow(14,PGFbar,"angle 45");
  createArrow(15,PGFbar,"angle 45 reversed");
  createArrow(16,PGFbar,"*");
  createArrow(17,PGFbar,"o");
  createArrow(18,PGFbar,"diamond");
  createArrow(19,PGFbar,"open diamond");
  createArrow(20,PGFbar,"square");
  createArrow(21,PGFbar,"open square");
  createArrow(22,PGFbar,"triangle 90");
  createArrow(23,PGFbar,"triangle 90 reversed");
  createArrow(24,PGFbar,"triangle 60");
  createArrow(25,PGFbar,"triangle 60 reversed");
  createArrow(26,PGFbar,"triangle 45");
  createArrow(27,PGFbar,"triangle 45 reversed");
  createArrow(28,PGFbar,"open triangle 90");
  createArrow(29,PGFbar,"open triangle 90 reversed");
  createArrow(30,PGFbar,"open triangle 60");
  createArrow(31,PGFbar,"open triangle 60 reversed");
  createArrow(32,PGFbar,"open triangle 45");
  createArrow(33,PGFbar,"open triangle 45 reversed");
  createArrow(34,PGFbar,"latex'");
  createArrow(35,PGFbar,"stealth'");
  createArrow(36,PGFbar,"stealth' reversed");
  createArrow(37,PGFbar,"left to");
  createArrow(38,PGFbar,"right to");
  createArrow(39,PGFbar,"left to reversed");
  createArrow(40,PGFbar,"right to reversed");
  createArrow(41,PGFbar,"left hook");
  createArrow(42,PGFbar,"left hook reversed");
  createArrow(43,PGFbar,"right hook");
  createArrow(44,PGFbar,"right hook reversed");
  createArrow(45,PGFbar,"hooks");
  createArrow(46,PGFbar,"hooks reversed");
  createArrow(47,PGFbar,"serif cm");
  createArrow(48,PGFbar,"round cap");
  createArrow(49,PGFbar,"butt cap");
  createArrow(50,PGFbar,"triangle 90 cap");
  createArrow(51,PGFbar,"triangle 90 cap reversed");
  createArrow(52,PGFbar,"fast cap");
  createArrow(53,PGFbar,"fast cap reversed");
  createArrow(54,PGFbar,"implies");
  createArrow(55,PGFbar,"Straight Barb");
  createArrow(56,PGFbar,"Hooks");
  createArrow(57,PGFbar,"Arc Barb");
  createArrow(58,PGFbar,"Tee Barb");
  createArrow(59,PGFbar,"Classical TikZ Rightarrow");
  createArrow(60,PGFbar,"Computer Modern Rightarrow");
  createArrow(61,PGFbar,"Implies");
  createArrow(62,PGFbar,"Latex");
  createArrow(63,PGFbar,"Stealth");
  createArrow(64,PGFbar,"Kite");
  createArrow(65,PGFbar,"Square");
  createArrow(66,PGFbar,"Circle");
  createArrow(67,PGFbar,"Round Cap");
  createArrow(68,PGFbar,"Butt Cap");
  createArrow(69,PGFbar,"Triangle Cap");
  createArrow(70,PGFbar,"Fast Triangle");
  createArrow(71,PGFbar,"Fast Round");
  createArrow(72,PGFbar,"Rays");
}

void XWTikzArrowDialog::createArrow(int index,int d,const QString & str)
{
  startarrows->setItemData(index,d);
  startarrows->setItemText(index,str);
  endarrows->setItemData(index,d);
  endarrows->setItemText(index,str);
  QIcon icon = createArrow(str);
  startarrows->setItemIcon(index,icon);
  endarrows->setItemIcon(index,icon);
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
