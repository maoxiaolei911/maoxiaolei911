/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTIKZSHADEDIALOG_H
#define XWTIKZSHADEDIALOG_H

#include <QDialog>
#include <QPixmap>

class QAbstractButton;
class QDialogButtonBox;
class QToolButton;
class QButtonGroup;

class XWTikzShadeWidget : public QWidget
{
  Q_OBJECT

public:
   XWTikzShadeWidget(QWidget * parent = 0);

signals:
  void shadeSelected(int s);

private slots:
  void buttonGroupClicked(QAbstractButton * button);

private:
  QToolButton * createButton(const QString & txt);

private:
  QButtonGroup * buttonGroup;
};

class XWTikzShadeDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWTikzShadeDialog(QWidget * parent = 0);

  int getShade();

public slots:
  void setShade(int d);

private:
  int shade;
  XWTikzShadeWidget * shadeWidget;
  
  QDialogButtonBox *buttonBox;
};

#endif //XWTIKZSHADEDIALOG_H
