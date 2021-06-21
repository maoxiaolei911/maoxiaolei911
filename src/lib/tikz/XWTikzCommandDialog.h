/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTIKZCOMMANDDIALOG_H
#define XWTIKZCOMMANDDIALOG_H

#include <QDialog>

class QDialogButtonBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;

class XWTikzNodeCommandDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWTikzNodeCommandDialog(QWidget * parent = 0);

  QString getCoord();
  QString getName();
  QString getText();

  void setCoord(const QString & coordA);
  void setName(const QString & nameA);
  void setText(const QString & txt);

private:
  QLabel * nlabel;
  QLineEdit * nedit;
  QLabel * atlabel;
  QLineEdit * atedit;
  QLabel * tlabel;
  QLineEdit * tedit;
  QDialogButtonBox *buttonBox;
};

class XWTikzCoordinateCommandDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWTikzCoordinateCommandDialog(QWidget * parent = 0);

  QString getCoord();
  QString getName();

  void setCoord(const QString & coordA);
  void setName(const QString & nameA);

private:
  QLabel * nlabel;
  QLineEdit * nedit;
  QLabel * atlabel;
  QLineEdit * atedit;
  QDialogButtonBox *buttonBox;
};


class XWTikzTwoCoordDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWTikzTwoCoordDialog(const QString & title,
                     const QString & stitle,
                     const QString & etitle,
                     QWidget * parent = 0);

  QString getCoord1();
  QString getCoord2();

  void setCoord1(const QString & coordA);
  void setCoord2(const QString & coordA);

private:
  QLabel * aLabel;
  QLineEdit * aEdit;
  QLabel * bLabel;
  QLineEdit * bEdit;

  QDialogButtonBox *buttonBox;
};

#endif //XWTIKZCOMMANDDIALOG_H

