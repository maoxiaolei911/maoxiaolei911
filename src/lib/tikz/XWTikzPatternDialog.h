/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTIKZPATTERNDIALOG_H
#define XWTIKZPATTERNDIALOG_H

#include <QDialog>
#include <QPixmap>

class QAbstractButton;
class QDialogButtonBox;
class QToolButton;
class QButtonGroup;


class XWTikzPatternWidget : public QWidget
{
  Q_OBJECT

public:
   XWTikzPatternWidget(QWidget * parent = 0);

signals:
  void patternSelected(int s);

private slots:
  void buttonGroupClicked(QAbstractButton * button);

private:
  QToolButton * createButton(const QString & txt);

private:
  QButtonGroup * buttonGroup;
};

class XWTikzPatternDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWTikzPatternDialog(QWidget * parent = 0);

  int getPattern();

public slots:
  void setPattern(int d);

private:
  int pattern;
  XWTikzPatternWidget * patternWidget;
  
  QDialogButtonBox *buttonBox;
};


#endif //XWTIKZPATTERNDIALOG_H
