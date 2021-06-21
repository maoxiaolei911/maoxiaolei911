/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTIKZFADINGDIALOG_H
#define XWTIKZFADINGDIALOG_H

#include <QDialog>
#include <QPixmap>

class QAbstractButton;
class QDialogButtonBox;
class QToolButton;
class QButtonGroup;


class XWTikzFadingWidget : public QWidget
{
  Q_OBJECT

public:
   XWTikzFadingWidget(QWidget * parent = 0);

signals:
  void fadingSelected(int s);

private slots:
  void buttonGroupClicked(QAbstractButton * button);

private:
  QToolButton * createButton(const QString & txt);

private:
  QButtonGroup * buttonGroup;
};

class XWTikzFadingDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWTikzFadingDialog(QWidget * parent = 0);

  int getFading();

public slots:
  void setFading(int d);

private:
  int fading;
  XWTikzFadingWidget * fadingWidget;
  
  QDialogButtonBox *buttonBox;
};


#endif //XWTIKZFADINGDIALOG_H
