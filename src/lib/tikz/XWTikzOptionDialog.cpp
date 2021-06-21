/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include "PGFKeyWord.h"
#include "XWPGFPool.h"
#include "XWTikzType.h"
#include "tikzcolor.h"
#include "XWTikzOptionDialog.h"


XWPictureSizeDialog::XWPictureSizeDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowTitle(tr("Graphic size"));
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  ptButton = new QRadioButton(tr("pt"),this);
  cmButton = new QRadioButton(tr("cm"),this);
  mmButton = new QRadioButton(tr("mm"),this);

  ptButton->setChecked(true);

  QGroupBox *groupBox = new QGroupBox(tr("Unit"));
  QHBoxLayout * grplayout = new QHBoxLayout;
  grplayout->addWidget(ptButton);
  grplayout->addWidget(cmButton);
  grplayout->addWidget(mmButton);
  grplayout->addStretch(1);
  groupBox->setLayout(grplayout);

  leftLabel = new QLabel(tr("minimum x:"), this);
  leftBox = new QDoubleSpinBox;
  leftBox->setMinimum(1);
  leftBox->setMaximum(1000);
  leftBox->setSingleStep(0.1);
  rightLabel = new QLabel(tr("maximum x:"), this);
  rightBox = new QDoubleSpinBox;
  rightBox->setMinimum(1);
  rightBox->setMaximum(1000);
  rightBox->setSingleStep(0.1);

  bottomLabel = new QLabel(tr("minimum y:"), this);
  bottomBox = new QDoubleSpinBox;
  bottomBox->setMinimum(1);
  bottomBox->setMaximum(1000);
  bottomBox->setSingleStep(0.1);

  topLabel = new QLabel(tr("maximum y:"), this);
  topBox = new QDoubleSpinBox;
  topBox->setMinimum(1);
  topBox->setMaximum(1000);
  topBox->setSingleStep(0.1);

  QGridLayout * whlayout = new QGridLayout;
  whlayout->addWidget(leftLabel,0,0);
  whlayout->addWidget(leftBox,0,1);
  whlayout->addWidget(rightLabel,1,0);
  whlayout->addWidget(rightBox,1,1);
  whlayout->addWidget(bottomLabel,2,0);
  whlayout->addWidget(bottomBox,2,1);
  whlayout->addWidget(topLabel,3,0);
  whlayout->addWidget(topBox,3,1);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout * layout = new QVBoxLayout;
  layout->addWidget(groupBox);
  layout->addLayout(whlayout);
  layout->addWidget(buttonBox);

  setLayout(layout);
}

double XWPictureSizeDialog::getBottom()
{
  return bottomBox->value();
}

double XWPictureSizeDialog::getLeft()
{
  return leftBox->value();
}

double XWPictureSizeDialog::getRight()
{
  return rightBox->value();
}

double XWPictureSizeDialog::getTop()
{
  return topBox->value();
}

int XWPictureSizeDialog::getUnit()
{
  if (ptButton->isChecked())
    return XW_TIKZ_UNIT_PT;

  if (cmButton->isChecked())
    return XW_TIKZ_UNIT_CM;

  return XW_TIKZ_UNIT_MM;
}

void XWPictureSizeDialog::setSize(int unit,double minxA, double minyA,double maxxA,double maxyA)
{
  leftBox->setValue(minxA);
  rightBox->setValue(maxxA);

  topBox->setValue(maxyA);
  bottomBox->setValue(minyA);

  switch (unit)
  {
    default:
      ptButton->setChecked(true);
      break;

    case XW_TIKZ_UNIT_CM:
      cmButton->setChecked(true);
      break;

    case XW_TIKZ_UNIT_MM:
      mmButton->setChecked(true);
      break;
  }
}

XWGridStepDialog::XWGridStepDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowTitle(tr("Grid step"));
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  ptButton = new QRadioButton(tr("pt"),this);
  cmButton = new QRadioButton(tr("cm"),this);
  mmButton = new QRadioButton(tr("mm"),this);

  ptButton->setChecked(true);

  QGroupBox *groupBox = new QGroupBox(tr("Unit"));
  QHBoxLayout * grplayout = new QHBoxLayout;
  grplayout->addWidget(ptButton);
  grplayout->addWidget(cmButton);
  grplayout->addWidget(mmButton);
  grplayout->addStretch(1);
  groupBox->setLayout(grplayout);

  stepLabel = new QLabel(tr("step:"), this);
  stepBox = new QDoubleSpinBox;
  stepBox->setMinimum(1);
  stepBox->setMaximum(1000);
  stepBox->setSingleStep(0.1);

  QHBoxLayout * whlayout = new QHBoxLayout;
  whlayout->addWidget(stepLabel);
  whlayout->addWidget(stepBox);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout * layout = new QVBoxLayout;
  layout->addWidget(groupBox);
  layout->addLayout(whlayout);
  layout->addWidget(buttonBox);

  setLayout(layout);
}

double XWGridStepDialog::getStep()
{
  return stepBox->value();
}

int XWGridStepDialog::getUnit()
{
  if (ptButton->isChecked())
    return XW_TIKZ_UNIT_PT;

  if (cmButton->isChecked())
    return XW_TIKZ_UNIT_CM;

  return XW_TIKZ_UNIT_MM;
}

void XWGridStepDialog::setStep(int unit, double s)
{
  stepBox->setValue(s);
  switch (unit)
  {
    default:
      ptButton->setChecked(true);
      break;

    case XW_TIKZ_UNIT_CM:
      cmButton->setChecked(true);
      break;

    case XW_TIKZ_UNIT_MM:
      mmButton->setChecked(true);
      break;
  }
}

XWTikzExpressDialog::XWTikzExpressDialog(const QString & title,
                                         const QString & ltxt,
                                         QWidget * parent)
:QDialog(parent)
{
  setWindowTitle(title);
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  label = new QLabel(ltxt,this);
  edit = new QLineEdit(this);

  QHBoxLayout * toplayout = new QHBoxLayout;
  toplayout->addWidget(label);
  toplayout->addWidget(edit);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout * layout = new QVBoxLayout;
  layout->addLayout(toplayout);
  layout->addWidget(buttonBox);

  setLayout(layout);
}

QString XWTikzExpressDialog::getExpress()
{
  return edit->text();
}

void XWTikzExpressDialog::setExpress(const QString & str)
{
  edit->setText(str);
}

XWTikzColorDialog::XWTikzColorDialog(const QString & title,QWidget * parent)
:QDialog(parent)
{
  setWindowTitle(title);
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  c1Box = new QComboBox(this);
  slider = new QSlider(this);
  slider->setRange(0,100);
  slider->setValue(100);
  c1label = new QLabel(this);

  QGroupBox * colorBox = new QGroupBox(tr("color"));
  QVBoxLayout * colorlayout = new QVBoxLayout;
  colorlayout->addWidget(c1Box);
  colorlayout->addWidget(slider);
  colorlayout->addStretch(1);
  colorlayout->addWidget(c1label);
  colorBox->setLayout(colorlayout);

  c2Box = new QComboBox(this);
  label = new QLabel(this);

  QGroupBox * mixBox = new QGroupBox(tr("mix"));
  QVBoxLayout * mixlayout = new QVBoxLayout;
  mixlayout->addWidget(c2Box);
  mixlayout->addStretch(1);
  mixlayout->addWidget(label);
  mixBox->setLayout(mixlayout);

  createColor();
  c1Box->setCurrentIndex(0);
  c2Box->setCurrentIndex(0);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout * layout = new QVBoxLayout;
  layout->addWidget(colorBox);
  layout->addWidget(mixBox);
  layout->addWidget(buttonBox);

  setLayout(layout);

  updateC1(0);

  connect(slider, SIGNAL(valueChanged(int)), this, SLOT(updateColor(int)));
  connect(c1Box, SIGNAL(currentIndexChanged(int)), this, SLOT(updateC1(int)));
  connect(c2Box, SIGNAL(currentIndexChanged(int)), this, SLOT(updateC(int)));
}

void XWTikzColorDialog::getColor(int & c1A,double & pA,int & c2A)
{
  int index = c1Box->currentIndex();
  c1A = c1Box->itemData(index).toInt();
  pA = slider->value() / 100.00;
  index = c2Box->currentIndex();
  c2A = c2Box->itemData(index).toInt();
}

void XWTikzColorDialog::setColor(int c1A,double pA,int c2A)
{
  int v = (int)(pA * 100);
  slider->setValue(v);
  int index = c1Box->findData(c1A);
  if (index >= 0)
    c1Box->setCurrentIndex(index);

  index = c2Box->findData(c2A);
  if (index >= 0)
    c2Box->setCurrentIndex(index);
}

void XWTikzColorDialog::updateC1(int index)
{
  int v = slider->value();
  double t = (double)v / 100.0;
  int c1 = c1Box->itemData(index).toInt();
  QColor color1 = tikzEnumToColor(c1);
  QColor color = calulateColor(color1,t);
  QPixmap pix(75,30);
  pix.fill(color);
  c1label->setPixmap(pix);

  updateColor(v);
}

void XWTikzColorDialog::updateC(int )
{
  int v = slider->value();
  updateColor(v);
}

void XWTikzColorDialog::updateColor(int v)
{
  double t = (double)v / 100.0;
  int index = c1Box->currentIndex();
  int c1 = c1Box->itemData(index).toInt();
  QColor color1 = tikzEnumToColor(c1);
  index = c2Box->currentIndex();
  int c2 = c2Box->itemData(index).toInt();
  QColor color;
  if (c2 > 0)
  {
    QColor color2 = tikzEnumToColor(c2);
    color = calulateColor(color1,t,color2);
  }
  else
    color = calulateColor(color1,t);

  QPixmap pix(75,30);
  pix.fill(color);
  label->setPixmap(pix);
}

void XWTikzColorDialog::createColor()
{
  createColorItem(c1Box,0,PGFwhite,tr("white"));
  createColorItem(c1Box,1,PGFblack,tr("black"));
  createColorItem(c1Box,2,PGFdarkgray,tr("darkgray"));
  createColorItem(c1Box,3,PGFgray,tr("gray"));
  createColorItem(c1Box,4,PGFlightgray,tr("lightgray"));
  createColorItem(c1Box,5,PGFred,tr("red"));
  createColorItem(c1Box,6,PGFgreen,tr("green"));
  createColorItem(c1Box,7,PGFblue,tr("blue"));
  createColorItem(c1Box,8,PGFcyan,tr("cyan"));
  createColorItem(c1Box,9,PGFmagenta,tr("magenta"));
  createColorItem(c1Box,10,PGFyellow,tr("yellow"));
  createColorItem(c1Box,11,PGFbrown,tr("brown"));
  createColorItem(c1Box,12,PGFlime,tr("lime"));
  createColorItem(c1Box,13,PGFolive,tr("olive"));
  createColorItem(c1Box,14,PGForange,tr("orange"));
  createColorItem(c1Box,15,PGFpink,tr("pink"));
  createColorItem(c1Box,16,PGFpurple,tr("purple"));
  createColorItem(c1Box,17,PGFteal,tr("teal"));
  createColorItem(c1Box,18,PGFviolet,tr("violet"));

  c2Box->setItemData(0,0);
  c2Box->setItemText(0,tr("none"));
  createColorItem(c2Box,1,PGFwhite,tr("white"));
  createColorItem(c2Box,2,PGFblack,tr("black"));
  createColorItem(c2Box,3,PGFdarkgray,tr("darkgray"));
  createColorItem(c2Box,4,PGFgray,tr("gray"));
  createColorItem(c2Box,5,PGFlightgray,tr("lightgray"));
  createColorItem(c2Box,6,PGFred,tr("red"));
  createColorItem(c2Box,7,PGFgreen,tr("green"));
  createColorItem(c2Box,8,PGFblue,tr("blue"));
  createColorItem(c2Box,9,PGFcyan,tr("cyan"));
  createColorItem(c2Box,10,PGFmagenta,tr("magenta"));
  createColorItem(c2Box,11,PGFyellow,tr("yellow"));
  createColorItem(c2Box,12,PGFbrown,tr("brown"));
  createColorItem(c2Box,13,PGFlime,tr("lime"));
  createColorItem(c2Box,14,PGFolive,tr("olive"));
  createColorItem(c2Box,15,PGForange,tr("orange"));
  createColorItem(c2Box,16,PGFpink,tr("pink"));
  createColorItem(c2Box,17,PGFpurple,tr("purple"));
  createColorItem(c2Box,18,PGFteal,tr("teal"));
  createColorItem(c2Box,19,PGFviolet,tr("violet"));
}

void XWTikzColorDialog::createColorItem(QComboBox * box,int index, int d, const QString & str)
{
  box->setItemData(index,d);
  box->setItemText(index,str);

  QColor color = tikzEnumToColor(d);

  QPixmap pix(75,25);
  pix.fill(color);

  QIcon icon(pix);
  box->setItemIcon(index,icon);
}

XWTikzAroundDialog::XWTikzAroundDialog(const QString & title, 
                                       const QString & ltxt,
                                       QWidget * parent)
:QDialog(parent)
{
  setWindowTitle(title);
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  expLabel = new QLabel(ltxt,this);
  expEdit = new QLineEdit(this);
  coordLabel = new QLabel(tr("around:"),this);
  coordEdit = new QLineEdit(this);
  coordEdit->setText("(1,1)");

  QGridLayout * toplayout = new QGridLayout;
  toplayout->addWidget(expLabel,0,0);
  toplayout->addWidget(expEdit,0,1);
  toplayout->addWidget(coordLabel,1,0);
  toplayout->addWidget(coordEdit,1,1);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout * layout = new QVBoxLayout;
  layout->addLayout(toplayout);
  layout->addWidget(buttonBox);

  setLayout(layout);
}

QString XWTikzAroundDialog::getCoord()
{
  QString ret = coordEdit->text();
  ret = ret.simplified();
  if (ret.isEmpty())
    ret = "(0,0)";

  if (ret[0] != QChar('(') && ret[0] != QChar('+'))
    ret.insert(0,"(");

  if (ret[ret.length() - 1] != QChar(')'))
    ret.append(")");

  return ret;
}

QString XWTikzAroundDialog::getExpress()
{
  QString ret = expEdit->text();
  if (ret.isEmpty())
    ret = "0";

  return ret;
}

void XWTikzAroundDialog::setCoord(const QString & str)
{
  coordEdit->setText(str);
}

void XWTikzAroundDialog::setExpress(const QString & str)
{
  expEdit->setText(str);
}

XWTikzTransformDialog::XWTikzTransformDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowTitle(tr("Transform matrix"));
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  aLabel = new QLabel(tr("m11:"),this);
  aEdit = new QLineEdit(this);
  bLabel = new QLabel(tr("m12:"),this);
  bEdit = new QLineEdit(this);
  cLabel = new QLabel(tr("m21:"),this);
  cEdit = new QLineEdit(this);
  dLabel = new QLabel(tr("m22:"),this);
  dEdit = new QLineEdit(this);
  txLabel = new QLabel(tr("tx:"),this);
  txEdit = new QLineEdit(this);
  tyLabel = new QLabel(tr("ty:"),this);
  tyEdit = new QLineEdit(this);

  QGridLayout * toplayout = new QGridLayout;
  toplayout->addWidget(aLabel,0,0);
  toplayout->addWidget(aEdit,0,1);
  toplayout->addWidget(bLabel,1,0);
  toplayout->addWidget(bEdit,1,1);
  toplayout->addWidget(cLabel,2,0);
  toplayout->addWidget(cEdit,2,1);
  toplayout->addWidget(dLabel,3,0);
  toplayout->addWidget(dEdit,3,1);
  toplayout->addWidget(txLabel,4,0);
  toplayout->addWidget(txEdit,4,1);
  toplayout->addWidget(tyLabel,5,0);
  toplayout->addWidget(tyEdit,5,1);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout * layout = new QVBoxLayout;
  layout->addLayout(toplayout);
  layout->addWidget(buttonBox);

  setLayout(layout);
}

QString XWTikzTransformDialog::getA()
{
  QString ret = aEdit->text() ;
  if (ret.isEmpty())
    ret = "0";

  return ret;
}

QString XWTikzTransformDialog::getB()
{
  QString ret = bEdit->text() ;
  if (ret.isEmpty())
    ret = "0";

  return ret;
}

QString XWTikzTransformDialog::getC()
{
  QString ret = cEdit->text() ;
  if (ret.isEmpty())
    ret = "0";

  return ret;
}

QString XWTikzTransformDialog::getCoord()
{
  QString ret = "(";
  QString tmp = txEdit->text();
  if (tmp.isEmpty())
    tmp = "0";

  ret += tmp;
  ret += ",";

  tmp = tyEdit->text();
  if (tmp.isEmpty())
    tmp = "0";

  ret += ")";
  return ret;
}

QString XWTikzTransformDialog::getD()
{
  QString ret = dEdit->text() ;
  if (ret.isEmpty())
    ret = "0";

  return ret;
}

void XWTikzTransformDialog::setA(const QString & str)
{
  aEdit->setText(str);
}

void XWTikzTransformDialog::setB(const QString & str)
{
  bEdit->setText(str);
}

void XWTikzTransformDialog::setC(const QString & str)
{
  cEdit->setText(str);
}

void XWTikzTransformDialog::setCoord(const QString & str)
{
  QString tmp = str.mid(1,str.length() - 1);
  QStringList list = str.split(QChar(','));
  if (list.size() == 2)
  {
    txEdit->setText(list[0]);
    tyEdit->setText(list[1]);
  }
}

void XWTikzTransformDialog::setD(const QString & str)
{
  dEdit->setText(str);
}

XWTikzDomainDialog::XWTikzDomainDialog(const QString & title,
                     const QString & stitle,
                     const QString & etitle,
                     QWidget * parent)
:QDialog(parent)
{
  setWindowTitle(title);
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  aLabel = new QLabel(stitle,this);
  aEdit = new QLineEdit(this);

  bLabel = new QLabel(etitle,this);
  bEdit = new QLineEdit(this);

  QGridLayout * toplayout = new QGridLayout;
  toplayout->addWidget(aLabel,0,0);
  toplayout->addWidget(aEdit,0,1);
  toplayout->addWidget(bLabel,1,0);
  toplayout->addWidget(bEdit,1,1);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout * layout = new QVBoxLayout;
  layout->addLayout(toplayout);
  layout->addWidget(buttonBox);

  setLayout(layout);
}

QString XWTikzDomainDialog::getEnd()
{
  return aEdit->text();
}

QString XWTikzDomainDialog::getStart()
{
  return bEdit->text();
}

void XWTikzDomainDialog::setEnd(const QString & str)
{
  aEdit->setText(str);
}

void XWTikzDomainDialog::setStart(const QString & str)
{
  bEdit->setText(str);
}
