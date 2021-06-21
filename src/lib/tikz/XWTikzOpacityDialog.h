/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTIKZOPACIITYDIALOG_H
#define XWTIKZOPACIITYDIALOG_H

#include <QDialog>

class QDoubleSpinBox;
class QDialogButtonBox;

class XWTikzDrawOpacityWidget : public QWidget
{
  Q_OBJECT

public:
  XWTikzDrawOpacityWidget(QWidget * parent = 0);

  QSize minimumSizeHint() const;
  QSize sizeHint() const;

public slots:
  void setOpacity(double o);

protected:
  void paintEvent(QPaintEvent *event);

private:
  double opacity;
};

class XWTikzDrawOpacityDialog : public QDialog
{
  Q_OBJECT

public:
  XWTikzDrawOpacityDialog(QWidget * parent = 0);

  double getOpacity();

  void setOpacity(double o);

private:
  QDoubleSpinBox * opacitySpinBox;

  XWTikzDrawOpacityWidget * opacityWidget;
  
  QDialogButtonBox *buttonBox;
};

class XWTikzFillOpacityWidget : public QWidget
{
  Q_OBJECT

public:
  XWTikzFillOpacityWidget(QWidget * parent = 0);

  QSize minimumSizeHint() const;
  QSize sizeHint() const;

public slots:
  void setOpacity(double o);

protected:
  void paintEvent(QPaintEvent *event);

private:
  double opacity;
};


class XWTikzFillOpacityDialog : public QDialog
{
  Q_OBJECT

public:
  XWTikzFillOpacityDialog(QWidget * parent = 0);

  double getOpacity();

  void setOpacity(double o);

private:
  QDoubleSpinBox * opacitySpinBox;

  XWTikzFillOpacityWidget * opacityWidget;
  
  QDialogButtonBox *buttonBox;
};


#endif //
