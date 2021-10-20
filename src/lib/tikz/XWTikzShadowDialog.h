/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTIKZSHADOWDIALOG_H
#define XWTIKZSHADOWDIALOG_H

#include <QDialog>
#include <QPixmap>

class QAbstractButton;
class QLabel;
class QComboBox;
class QDialogButtonBox;
class QToolButton;
class QButtonGroup;
class QUndoCommand;

class XWTikzShadowWidget : public QWidget
{
  Q_OBJECT

public:
   XWTikzShadowWidget(QWidget * parent = 0);

signals:
  void shadowSelected(int s);

private slots:
  void buttonGroupClicked(QAbstractButton * button);

private:
  QToolButton * createButton(const QString & txt);

private:
  QButtonGroup * buttonGroup;
};

class XWTikzShadowDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWTikzShadowDialog(QWidget * parent = 0);

  int getShadow();

public slots:
  void setShadow(int d);

private:
  int shadow;
  XWTikzShadowWidget * shadowWidget;
  
  QDialogButtonBox *buttonBox;
};

#endif //XWTIKZSHADOWDIALOG_H
