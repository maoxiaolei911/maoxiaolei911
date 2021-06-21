/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWTikzCommandDialog.h"

XWTikzNodeCommandDialog::XWTikzNodeCommandDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowTitle(tr("Node"));
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  nlabel = new QLabel(tr("name:"),this);
  nedit = new QLineEdit(this);

  atlabel = new QLabel(tr("at:"),this);
  atedit = new QLineEdit(this);
  atedit->setText("(1,1)");

  tlabel = new QLabel(tr("text:"),this);
  tedit = new QLineEdit(this);

  QGridLayout * toplayout = new QGridLayout;
  toplayout->addWidget(nlabel,0,0);
  toplayout->addWidget(nedit,0,1);
  toplayout->addWidget(atlabel,1,0);
  toplayout->addWidget(atedit,1,1);
  toplayout->addWidget(tlabel,2,0);
  toplayout->addWidget(tedit,2,1);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout * layout = new QVBoxLayout;
  layout->addLayout(toplayout);
  layout->addWidget(buttonBox);

  setLayout(layout);
}

QString XWTikzNodeCommandDialog::getCoord()
{
  QString ret = atedit->text();
  ret = ret.simplified();
  if (ret.isEmpty())
    ret = "(1,1)";

  if (ret[0] != QChar('(') && ret[0] != QChar('+'))
    ret.insert(0,"(");

  if (ret[ret.length() - 1] != QChar(')'))
    ret.append(")");

  return ret;
}

QString XWTikzNodeCommandDialog::getName()
{
  return nedit->text();
}

QString XWTikzNodeCommandDialog::getText()
{
  return tedit->text();
}

void XWTikzNodeCommandDialog::setCoord(const QString & coordA)
{
  atedit->setText(coordA);
}

void XWTikzNodeCommandDialog::setName(const QString & nameA)
{
  nedit->setText(nameA);
}

void XWTikzNodeCommandDialog::setText(const QString & txt)
{
  tedit->setText(txt);
}

XWTikzCoordinateCommandDialog::XWTikzCoordinateCommandDialog(QWidget * parent)
:QDialog(parent)
{
  setWindowTitle(tr("Coordinate"));
  setWindowIcon(QIcon(":/images/xiuwen24.png"));

  nlabel = new QLabel(tr("name:"),this);
  nedit = new QLineEdit(this);

  atlabel = new QLabel(tr("at:"),this);
  atedit = new QLineEdit(this);
  atedit->setText("(1,1)");

  QGridLayout * toplayout = new QGridLayout;
  toplayout->addWidget(nlabel,0,0);
  toplayout->addWidget(nedit,0,1);
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

QString XWTikzCoordinateCommandDialog::getCoord()
{
  QString ret = atedit->text();
  ret = ret.simplified();
  if (ret.isEmpty())
    ret = "(1,1)";

  if (ret[0] != QChar('(') && ret[0] != QChar('+'))
    ret.insert(0,"(");

  if (ret[ret.length() - 1] != QChar(')'))
    ret.append(")");

  return ret;
}

QString XWTikzCoordinateCommandDialog::getName()
{
  return nedit->text();
}

void XWTikzCoordinateCommandDialog::setCoord(const QString & coordA)
{
  atedit->setText(coordA);
}

void XWTikzCoordinateCommandDialog::setName(const QString & nameA)
{
  nedit->setText(nameA);
}

XWTikzTwoCoordDialog::XWTikzTwoCoordDialog(const QString & title,
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

QString XWTikzTwoCoordDialog::getCoord1()
{
  QString ret = aEdit->text();
  ret = ret.simplified();
  if (ret.isEmpty())
    ret = "(1,1)";

  if (ret[0] != QChar('(') && ret[0] != QChar('+'))
    ret.insert(0,"(");

  if (ret[ret.length() - 1] != QChar(')'))
    ret.append(")");

  return ret;
}

QString XWTikzTwoCoordDialog::getCoord2()
{
  QString ret = bEdit->text();
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

void XWTikzTwoCoordDialog::setCoord1(const QString & coordA)
{
  aEdit->setText(coordA);
}

void XWTikzTwoCoordDialog::setCoord2(const QString & coordA)
{
  bEdit->setText(coordA);
}
