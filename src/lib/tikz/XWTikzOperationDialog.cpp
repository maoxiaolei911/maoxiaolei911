/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include "PGFKeyWord.h"
#include "XWTikzOperationDialog.h"

XWTikzCoordDialog::XWTikzCoordDialog(const QString & title,QWidget * parent)
:QDialog(parent)
{
  setWindowTitle(title);
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  label = new QLabel(tr("coordinate:"),this);
  edit = new QLineEdit(this);
  edit->setText("(1,1)");

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

QString XWTikzCoordDialog::getCoord()
{
  QString ret = edit->text();
  ret = ret.simplified();
  if (ret.isEmpty())
    ret = "(1,1)";

  if (ret[0] != QChar('(') && ret[0] != QChar('+'))
    ret.insert(0,"(");

  if (ret[ret.length() - 1] != QChar(')'))
    ret.append(")");

  return ret;
}

void XWTikzCoordDialog::setCoord(const QString & coordA)
{
  edit->setText(coordA);
}

XWTikzCurveToDialog::XWTikzCurveToDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowTitle(tr("Curve to"));
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  c1label = new QLabel(tr("first control point:"),this);
  c1edit = new QLineEdit(this);
  c1edit->setText("(1,1)");
  c2label = new QLabel(tr("second control point:"),this);
  c2edit = new QLineEdit(this);
  endlabel = new QLabel(tr("end point:"),this);
  endedit = new QLineEdit(this);
  endedit->setText("(3,3)");

  QGridLayout * toplayout = new QGridLayout;
  toplayout->addWidget(c1label,0,0);
  toplayout->addWidget(c1edit,0,1);
  toplayout->addWidget(c2label,1,0);
  toplayout->addWidget(c2edit,1,1);
  toplayout->addWidget(endlabel,2,0);
  toplayout->addWidget(endedit,2,1);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout * layout = new QVBoxLayout;
  layout->addLayout(toplayout);
  layout->addWidget(buttonBox);

  setLayout(layout);
}

QString XWTikzCurveToDialog::getC1()
{
  QString ret = c1edit->text();
  ret = ret.simplified();
  if (ret.isEmpty())
    ret = "(1,1)";

  if (ret[0] != QChar('(') && ret[0] != QChar('+'))
    ret.insert(0,"(");

  if (ret[ret.length() - 1] != QChar(')'))
    ret.append(")");

  return ret;
}

QString XWTikzCurveToDialog::getC2()
{
  QString ret = c2edit->text();
  ret = ret.simplified();
  if (!ret.isEmpty())
  {
    if (ret[0] != QChar('(') && ret[0] != QChar('+'))
      ret.insert(0,"(");

    if (ret[ret.length() - 1] != QChar(')'))
      ret.append(")");
  }

  return ret;
}

QString XWTikzCurveToDialog::getEnd()
{
  QString ret = endedit->text();
  ret = ret.simplified();
  if (ret.isEmpty())
    ret = "(3,3)";

  if (ret[0] != QChar('(') && ret[0] != QChar('+'))
    ret.insert(0,"(");

  if (ret[ret.length() - 1] != QChar(')'))
    ret.append(")");

  return ret;
}

void XWTikzCurveToDialog::setC1(const QString & coordA)
{
  c1edit->setText(coordA);
}

void XWTikzCurveToDialog::setC2(const QString & coordA)
{
  c2edit->setText(coordA);
}

void XWTikzCurveToDialog::setEnd(const QString & coordA)
{
  endedit->setText(coordA);
}

XWTikzCircleDialog::XWTikzCircleDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowTitle(tr("Circle"));
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  rlabel = new QLabel(tr("radius:"),this);
  redit = new QLineEdit(this);

  atlabel = new QLabel(tr("center at:"),this);
  atedit = new QLineEdit(this);

  QGridLayout * toplayout = new QGridLayout;
  toplayout->addWidget(rlabel,0,0);
  toplayout->addWidget(redit,0,1);
  toplayout->addWidget(atlabel,1,0);
  toplayout->addWidget(atedit,1,1);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout * layout = new QVBoxLayout;
  layout->addLayout(toplayout);
  layout->addWidget(buttonBox);

  setLayout(layout);
}

QString XWTikzCircleDialog::getAt()
{
  QString ret = atedit->text();
  ret = ret.simplified();
  if (!ret.isEmpty())
  {
    if (ret[0] != QChar('(') && ret[0] != QChar('+'))
      ret.insert(0,"(");

    if (ret[ret.length() - 1] != QChar(')'))
      ret.append(")");
  }

  return ret;
}

QString XWTikzCircleDialog::getRadius()
{
  return redit->text();
}

void XWTikzCircleDialog::setAt(const QString & coordA)
{
  atedit->setText(coordA);
}

void XWTikzCircleDialog::setRadius(const QString & rA)
{
  redit->setText(rA);
}

XWTikzEllipseDialog::XWTikzEllipseDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowTitle(tr("Ellipse"));
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  rxlabel = new QLabel(tr("x radius:"),this);
  rxedit = new QLineEdit(this);
  rylabel = new QLabel(tr("y radius:"),this);
  ryedit = new QLineEdit(this);

  atlabel = new QLabel(tr("center at:"),this);
  atedit = new QLineEdit(this);

  QGridLayout * toplayout = new QGridLayout;
  toplayout->addWidget(rxlabel,0,0);
  toplayout->addWidget(rxedit,0,1);
  toplayout->addWidget(rylabel,1,0);
  toplayout->addWidget(ryedit,1,1);
  toplayout->addWidget(atlabel,2,0);
  toplayout->addWidget(atedit,2,1);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout * layout = new QVBoxLayout;
  layout->addLayout(toplayout);
  layout->addWidget(buttonBox);

  setLayout(layout);
}

QString XWTikzEllipseDialog::getAt()
{
  QString ret = atedit->text();
  ret = ret.simplified();
  if (!ret.isEmpty())
  {
    if (ret[0] != QChar('(') && ret[0] != QChar('+'))
      ret.insert(0,"(");

    if (ret[ret.length() - 1] != QChar(')'))
      ret.append(")");
  }

  return ret;
}

QString XWTikzEllipseDialog::getXRadius()
{
  return rxedit->text();
}

QString XWTikzEllipseDialog::getYRadius()
{
  return ryedit->text();
}

void XWTikzEllipseDialog::setAt(const QString & coordA)
{
  atedit->setText(coordA);
}

void XWTikzEllipseDialog::setXRadius(const QString & rA)
{
  rxedit->setText(rA);
}

void XWTikzEllipseDialog::setYRadius(const QString & rA)
{
  ryedit->setText(rA);
}

XWTikzArcDialog::XWTikzArcDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowTitle(tr("Arc"));
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  slabel = new QLabel(tr("start angle:"),this);
  sedit = new QLineEdit(this);
  sedit->setText("0");

  elabel = new QLabel(tr("end angle:"),this);
  eedit = new QLineEdit(this);
  eedit->setText("90");

  QGridLayout * toplayout = new QGridLayout;
  toplayout->addWidget(slabel,0,0);
  toplayout->addWidget(sedit,0,1);
  toplayout->addWidget(elabel,1,0);
  toplayout->addWidget(eedit,1,1);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout * layout = new QVBoxLayout;
  layout->addLayout(toplayout);
  layout->addWidget(buttonBox);

  setLayout(layout);
}

QString XWTikzArcDialog::getEndAngle()
{
  return eedit->text();
}

QString XWTikzArcDialog::getStartAngle()
{
  return sedit->text();
}

void XWTikzArcDialog::setEndAngle(const QString & a)
{
  eedit->setText(a);
}

void XWTikzArcDialog::setStartAngle(const QString & a)
{
  sedit->setText(a);
}

XWTikzGridDialog::XWTikzGridDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowTitle(tr("Grid"));
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  clabel = new QLabel(tr("corner coordinate:"),this);
  cedit = new QLineEdit(this);
  cedit->setText("(1,1)");

  slabel = new QLabel(tr("step:"),this);
  sedit = new QLineEdit(this);

  QGridLayout * toplayout = new QGridLayout;
  toplayout->addWidget(clabel,0,0);
  toplayout->addWidget(cedit,0,1);
  toplayout->addWidget(slabel,1,0);
  toplayout->addWidget(sedit,1,1);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout * layout = new QVBoxLayout;
  layout->addLayout(toplayout);
  layout->addWidget(buttonBox);

  setLayout(layout);
}

QString XWTikzGridDialog::getCorner()
{
  QString ret = cedit->text();
  ret = ret.simplified();
  if (ret.isEmpty())
    ret = "(1,1)";

  if (ret[0] != QChar('(') && ret[0] != QChar('+'))
    ret.insert(0,"(");

  if (ret[ret.length() - 1] != QChar(')'))
    ret.append(")");

  return ret;
}

QString XWTikzGridDialog::getStep()
{
  return sedit->text();
}

void XWTikzGridDialog::setCorner(const QString & coordA)
{
  cedit->setText(coordA);
}

void XWTikzGridDialog::setStep(const QString & s)
{
  sedit->setText(s);
}

XWTikzParabolaDialog::XWTikzParabolaDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowTitle(tr("Parabola"));
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  blabel = new QLabel(tr("bend at:"),this);
  bedit = new QLineEdit(this);

  elabel = new QLabel(tr("end at:"),this);
  eedit = new QLineEdit(this);
  eedit->setText("(1,1)");

  QGridLayout * toplayout = new QGridLayout;
  toplayout->addWidget(blabel,0,0);
  toplayout->addWidget(bedit,0,1);
  toplayout->addWidget(elabel,1,0);
  toplayout->addWidget(eedit,1,1);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout * layout = new QVBoxLayout;
  layout->addLayout(toplayout);
  layout->addWidget(buttonBox);

  setLayout(layout);
}

QString XWTikzParabolaDialog::getBend()
{
  QString ret = bedit->text();
  ret = ret.simplified();
  if (!ret.isEmpty())
  {
    if (ret[0] != QChar('(') && ret[0] != QChar('+'))
      ret.insert(0,"(");

    if (ret[ret.length() - 1] != QChar(')'))
      ret.append(")");
  }

  return ret;
}

QString XWTikzParabolaDialog::getEnd()
{
  QString ret = eedit->text();
  ret = ret.simplified();
  if (ret.isEmpty())
    ret = "(1,1)";

  if (ret[0] != QChar('(') && ret[0] != QChar('+'))
    ret.insert(0,"(");

  if (ret[ret.length() - 1] != QChar(')'))
    ret.append(")");

  return ret;
}

void XWTikzParabolaDialog::setBend(const QString & coordA)
{
  bedit->setText(coordA);
}

void XWTikzParabolaDialog::setEnd(const QString & coordA)
{
  eedit->setText(coordA);
}

XWTikzNodeDialog::XWTikzNodeDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowTitle(tr("Node"));
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  nlabel = new QLabel(tr("name:"),this);
  nedit = new QLineEdit(this);

  tlabel = new QLabel(tr("text:"),this);
  tedit = new QLineEdit(this);

  QGridLayout * toplayout = new QGridLayout;
  toplayout->addWidget(nlabel,0,0);
  toplayout->addWidget(nedit,0,1);
  toplayout->addWidget(tlabel,1,0);
  toplayout->addWidget(tedit,1,1);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout * layout = new QVBoxLayout;
  layout->addLayout(toplayout);
  layout->addWidget(buttonBox);

  setLayout(layout);
}

QString XWTikzNodeDialog::getName()
{
  return nedit->text();
}

QString XWTikzNodeDialog::getText()
{
  return tedit->text();
}

void XWTikzNodeDialog::setName(const QString & nameA)
{
  nedit->setText(nameA);
}

void XWTikzNodeDialog::setText(const QString & txt)
{
  tedit->setText(txt);
}

XWTikzNameDialog::XWTikzNameDialog(const QString & title,QWidget * parent)
:QDialog(parent)
{
  setWindowTitle(title);
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  label = new QLabel(tr("name:"),this);
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

QString XWTikzNameDialog::getName()
{
  return edit->text();
}

void XWTikzNameDialog::setName(const QString & nameA)
{
  edit->setText(nameA);
}

XWTikzPlotCoordsDialog::XWTikzPlotCoordsDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowTitle(tr("Plot coordinates"));
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  label = new QLabel(tr("coordinates:"),this);
  edit = new QLineEdit(this);
  edit->setText("(1,1)(2,2)(3,3)");

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

QString XWTikzPlotCoordsDialog::getCoords()
{
  return edit->text();
}

void XWTikzPlotCoordsDialog::setCoords(const QString & nameA)
{
  edit->setText(nameA);
}

XWTikzPlotFunctionDialog::XWTikzPlotFunctionDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowTitle(tr("Plot function"));
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  flabel = new QLabel(tr("coordinate:"),this);
  fedit = new QLineEdit(this);
  fedit->setText("(\\x,sin(\\x r))");

  slabel = new QLabel(tr("domain start:"),this);
  sedit = new QLineEdit(this);
  sedit->setText("0");

  elabel = new QLabel(tr("domain end:"),this);
  eedit = new QLineEdit(this);
  eedit->setText("6.283");

  QGridLayout * toplayout = new QGridLayout;
  toplayout->addWidget(flabel,0,0);
  toplayout->addWidget(fedit,0,1);
  toplayout->addWidget(slabel,1,0);
  toplayout->addWidget(sedit,1,1);
  toplayout->addWidget(elabel,2,0);
  toplayout->addWidget(eedit,2,1);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout * layout = new QVBoxLayout;
  layout->addLayout(toplayout);
  layout->addWidget(buttonBox);

  setLayout(layout);
}

QString XWTikzPlotFunctionDialog::getEnd()
{
  return eedit->text();
}

QString XWTikzPlotFunctionDialog::getFunction()
{
  return fedit->text();
}

QString XWTikzPlotFunctionDialog::getStart()
{
  return sedit->text();
}

void XWTikzPlotFunctionDialog::setFunction(const QString & str)
{
  fedit->setText(str);
}
