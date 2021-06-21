/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTIKZOPERATIONDIALOG_H
#define XWTIKZOPERATIONDIALOG_H

#include <QDialog>

class QDialogButtonBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;

class XWTikzCoordDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWTikzCoordDialog(const QString & title,QWidget * parent = 0);

  QString getCoord();

  void setCoord(const QString & coordA);

private:
  QLabel * label;
  QLineEdit * edit;
  QDialogButtonBox *buttonBox;
};

class XWTikzCurveToDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWTikzCurveToDialog(QWidget * parent = 0);

  QString getC1();
  QString getC2();
  QString getEnd();

  void setC1(const QString & coordA);
  void setC2(const QString & coordA);
  void setEnd(const QString & coordA);

private:
  QLabel * c1label;
  QLineEdit * c1edit;
  QLabel * c2label;
  QLineEdit * c2edit;
  QLabel * endlabel;
  QLineEdit * endedit;

  QDialogButtonBox *buttonBox;
};

class XWTikzCircleDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWTikzCircleDialog(QWidget * parent = 0);

  QString getAt();
  QString getRadius();

  void setAt(const QString & coordA);
  void setRadius(const QString & rA);

private:
  QLabel * rlabel;
  QLineEdit * redit;
  QLabel * atlabel;
  QLineEdit * atedit;
  QDialogButtonBox *buttonBox;
};

class XWTikzEllipseDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWTikzEllipseDialog(QWidget * parent = 0);

  QString getAt();
  QString getXRadius();
  QString getYRadius();

  void setAt(const QString & coordA);
  void setXRadius(const QString & rA);
  void setYRadius(const QString & rA);

private:
  QLabel * rxlabel;
  QLineEdit * rxedit;
  QLabel * rylabel;
  QLineEdit * ryedit;
  QLabel * atlabel;
  QLineEdit * atedit;
  QDialogButtonBox *buttonBox;
};

class XWTikzArcDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWTikzArcDialog(QWidget * parent = 0);

  QString getEndAngle();
  QString getStartAngle();

  void setEndAngle(const QString & a);
  void setStartAngle(const QString & a);

private:
  QLabel * slabel;
  QLineEdit * sedit;
  QLabel * elabel;
  QLineEdit * eedit;
  QDialogButtonBox *buttonBox;
};

class XWTikzGridDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWTikzGridDialog(QWidget * parent = 0);

  QString getCorner();
  QString getStep();

  void setCorner(const QString & coordA);
  void setStep(const QString & s);

private:
  QLabel * clabel;
  QLineEdit * cedit;
  QLabel * slabel;
  QLineEdit * sedit;
  QDialogButtonBox *buttonBox;
};

class XWTikzParabolaDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWTikzParabolaDialog(QWidget * parent = 0);

  QString getBend();
  QString getEnd();

  void setBend(const QString & coordA);
  void setEnd(const QString & coordA);

private:
  QLabel * blabel;
  QLineEdit * bedit;
  QLabel * elabel;
  QLineEdit * eedit;
  QDialogButtonBox *buttonBox;
};

class XWTikzNodeDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWTikzNodeDialog(QWidget * parent = 0);

  QString getName();
  QString getText();

  void setName(const QString & nameA);
  void setText(const QString & txt);

private:
  QLabel * nlabel;
  QLineEdit * nedit;
  QLabel * tlabel;
  QLineEdit * tedit;
  QDialogButtonBox *buttonBox;
};

class XWTikzNameDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWTikzNameDialog(const QString & title,QWidget * parent = 0);

  QString getName();

  void setName(const QString & nameA);

private:
  QLabel * label;
  QLineEdit * edit;
  QDialogButtonBox *buttonBox;
};

class XWTikzPlotCoordsDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWTikzPlotCoordsDialog(QWidget * parent = 0);

  QString getCoords();

  void setCoords(const QString & nameA);

private:
  QLabel * label;
  QLineEdit * edit;
  QDialogButtonBox *buttonBox;
};

class XWTikzPlotFunctionDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWTikzPlotFunctionDialog(QWidget * parent = 0);

  QString getEnd();
  QString getFunction();
  QString getStart();

  void setFunction(const QString & str);

private:
  QLabel * flabel;
  QLineEdit * fedit;
  QLabel * slabel;
  QLineEdit * sedit;
  QLabel * elabel;
  QLineEdit * eedit;
  QDialogButtonBox *buttonBox;
};

#endif //XWTIKZOPERATIONDIALOG_H

