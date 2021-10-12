/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTIKZOPTIONDIALOG_H
#define XWTIKZOPTIONDIALOG_H

#include <QDialog>
#include <QPixmap>

class QDialogButtonBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QComboBox;
class QDoubleSpinBox;
class QSlider;
class QButtonGroup;
class QAbstractButton;
class QToolButton;

class XWPictureSizeDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWPictureSizeDialog(QWidget * parent = 0);

  double getBottom();
  double getLeft();
  double getRight();
  double getTop();

  int getUnit();

  void setSize(int unit,double minxA, double minyA,double maxxA,double maxyA);

private:
  QRadioButton * ptButton;
  QRadioButton * cmButton;
  QRadioButton * mmButton;

  QLabel * leftLabel;
  QDoubleSpinBox * leftBox;
  QLabel * rightLabel;
  QDoubleSpinBox * rightBox;
  QLabel * topLabel;
  QDoubleSpinBox * topBox;
  QLabel * bottomLabel;
  QDoubleSpinBox * bottomBox;

  QDialogButtonBox *buttonBox;
};

class XWGridStepDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWGridStepDialog(QWidget * parent = 0);

  double getStep();
  int getUnit();

  void setStep(int unit, double s);

private:
  QRadioButton * ptButton;
  QRadioButton * cmButton;
  QRadioButton * mmButton;

  QLabel * stepLabel;
  QDoubleSpinBox * stepBox;

  QDialogButtonBox *buttonBox;
};

class XWTikzExpressDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWTikzExpressDialog(const QString & title,
                      const QString & ltxt,
                      QWidget * parent = 0);

  QString getExpress();

  void setExpress(const QString & str);

private:
  QLabel * label;
  QLineEdit * edit;
  QDialogButtonBox *buttonBox;
};

class XWTikzColorDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWTikzColorDialog(const QString & title,QWidget * parent = 0);

  void getColor(int & c1A,double & pA,int & c2A);

  void setColor(int c1A,double pA,int c2A);

private slots:
  void updateC1(int index);
  void updateC(int index);
  void updateColor(int v);

private:
  void createColor();
  void createColorItem(QComboBox * box,int index, int d, const QString & str);

private:
  QComboBox * c1Box;
  QSlider * slider;
  QLabel * c1label;
  QComboBox * c2Box;
  QLabel * label;
  QDialogButtonBox *buttonBox;
};

class XWTikzAroundDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWTikzAroundDialog(const QString & title, 
                     const QString & ltxt,
                     QWidget * parent = 0);

  QString getCoord();
  QString getExpress();

  void setCoord(const QString & str);
  void setExpress(const QString & str);

private:
  QLabel * expLabel;
  QLineEdit * expEdit;
  QLabel * coordLabel;
  QLineEdit * coordEdit;

  QDialogButtonBox *buttonBox;
};

class XWTikzTransformDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWTikzTransformDialog(QWidget * parent = 0);

  QString getA();
  QString getB();
  QString getC();
  QString getCoord();
  QString getD();

  void setA(const QString & str);
  void setB(const QString & str);
  void setC(const QString & str);
  void setCoord(const QString & str);
  void setD(const QString & str);

private:
  QLabel * aLabel;
  QLineEdit * aEdit;
  QLabel * bLabel;
  QLineEdit * bEdit;
  QLabel * cLabel;
  QLineEdit * cEdit;
  QLabel * dLabel;
  QLineEdit * dEdit;
  QLabel * txLabel;
  QLineEdit * txEdit;
  QLabel * tyLabel;
  QLineEdit * tyEdit;

  QDialogButtonBox *buttonBox;
};

class XWTikzDomainDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWTikzDomainDialog(const QString & title,
                     const QString & stitle,
                     const QString & etitle,
                     QWidget * parent = 0);

  QString getEnd();
  QString getStart();

  void setEnd(const QString & str);
  void setStart(const QString & str);

private:
  QLabel * aLabel;
  QLineEdit * aEdit;
  QLabel * bLabel;
  QLineEdit * bEdit;

  QDialogButtonBox *buttonBox;
};

class XWTikzLabelDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWTikzLabelDialog(const QString & title,QWidget * parent = 0);

  QString getLabel();

  void setAngle(const QString & a);
  void setOptions(const QString & o);
  void setText(const QString & txt);

private:
  QLabel * aLabel;
  QLineEdit * aEdit;
  QLabel * bLabel;
  QLineEdit * bEdit;
  QLabel * cLabel;
  QLineEdit * cEdit;

  QDialogButtonBox *buttonBox;
};

#endif XWTIKZOPTIONDIALOG_H
