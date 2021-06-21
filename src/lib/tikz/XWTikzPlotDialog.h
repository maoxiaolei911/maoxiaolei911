/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTIKZPLOTDIALOG_H
#define XWTIKZPLOTDIALOG_H

#include <QDialog>
#include <QPixmap>

#include <QDialog>
#include <QPixmap>

class QAbstractButton;
class QDialogButtonBox;
class QToolButton;
class QButtonGroup;


class XWTikzPlotHandlerWidget : public QWidget
{
  Q_OBJECT

public:
   XWTikzPlotHandlerWidget(QWidget * parent = 0);

signals:
  void handlerSelected(int s);

private slots:
  void buttonGroupClicked(QAbstractButton * button);

private:
  QToolButton * createButton(const QString & txt);

private:
  QButtonGroup * buttonGroup;
};

class XWTikzPlotHandlerDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWTikzPlotHandlerDialog(QWidget * parent = 0);

  int getHandler();

public slots:
  void setHandler(int d);

private:
  int handler;
  XWTikzPlotHandlerWidget * handlerWidget;
  
  QDialogButtonBox *buttonBox;
};


class XWTikzPlotMarkWidget : public QWidget
{
  Q_OBJECT

public:
   XWTikzPlotMarkWidget(QWidget * parent = 0);

signals:
  void markSelected(int s);

private slots:
  void buttonGroupClicked(QAbstractButton * button);

private:
  QToolButton * createButton(const QString & txt);

private:
  QButtonGroup * buttonGroup;
};


class XWTikzPlotMarkDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWTikzPlotMarkDialog(QWidget * parent = 0);

  int getMark();

public slots:
  void setMark(int d);

private:
  int mark;
  XWTikzPlotMarkWidget * markWidget;
  
  QDialogButtonBox *buttonBox;
};

#endif //XWTIKZPLOTDIALOG_H
