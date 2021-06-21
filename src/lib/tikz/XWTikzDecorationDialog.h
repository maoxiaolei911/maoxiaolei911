/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTIKZDECORATIONDIALOG_H
#define XWTIKZDECORATIONDIALOG_H

#include <QDialog>
#include <QPixmap>

class QAbstractButton;
class QDialogButtonBox;
class QToolButton;
class QButtonGroup;

class XWTikzDecorationWidget : public QWidget
{
  Q_OBJECT

public:
   XWTikzDecorationWidget(QWidget * parent = 0);

signals:
  void decorationSelected(int s);

private slots:
  void buttonGroupClicked(QAbstractButton * button);

private:
  QToolButton * createButton(const QString & txt);

private:
  QButtonGroup * buttonGroup;
};

class XWTikzDecorationDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWTikzDecorationDialog(QWidget * parent = 0);

  int getDecoration();

public slots:
  void setDecoration(int d);

private:
  int decoration;
  XWTikzDecorationWidget * decorationWidget;
  
  QDialogButtonBox *buttonBox;
};

#endif //XWTIKZDECORATIONDIALOG_H
