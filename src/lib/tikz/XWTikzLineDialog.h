/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTIKZLINEDIALOG_H
#define XWTIKZLINEDIALOG_H

#include <QDialog>
#include <QPixmap>


class QLabel;
class QComboBox;
class QDialogButtonBox;

class XWTikzLineWidthDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWTikzLineWidthDialog(QWidget * parent = 0);

  int getLineWidth();

  void setLineWidth(int i);

private:
  void createItem(int index, int d, const QString & txt);

private:
  QLabel * label;
  QComboBox * widths;
  QDialogButtonBox *buttonBox;
};

class XWTikzLineCapDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWTikzLineCapDialog(QWidget * parent = 0);

  int  getCap();

  void setCap(int c);

private:
  void createItem(int index, int d, const QString & txt);

private:
  QLabel * label;
  QComboBox * caps;
  QDialogButtonBox *buttonBox;
};

class XWTikzLineJoinDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWTikzLineJoinDialog(QWidget * parent = 0);

  int  getJion();

  void setJion(int j);

private:
  void createItem(int index, int d, const QString & txt);

private:
  QLabel * label;
  QComboBox * joins;
  QDialogButtonBox *buttonBox;
};

class XWTikzDashDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWTikzDashDialog(QWidget * parent = 0);

  int getDash();

  void setDash(int d);

private:
  void createDash();
  void createDash(const QVector<qreal> & pattern);
  void createDash(int index,int d,const QString & str);

private:
  QLabel * label;
  QComboBox * dashs;

  QDialogButtonBox *buttonBox;
};


#endif //XWTIKZLINEDIALOG_H
