/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTEXEDITSETTING_H
#define XWTEXEDITSETTING_H

#include <QObject>
#include <QDialog>
#include "XWTeXGuiType.h"

class QPushButton;
class XWSystemFontWidget;

class XWTexEditSPWidget : public QWidget
{
  Q_OBJECT

public:
  XWTexEditSPWidget(QWidget * parent = 0);

  void save();
  void setBgColor(const QColor & c);

private:
  XWSystemFontWidget * wdiget;
};

class XWTexEditCommentWidget : public QWidget
{
  Q_OBJECT

public:
  XWTexEditCommentWidget(QWidget * parent = 0);

  void save();
  void setBgColor(const QColor & c);

private:
  XWSystemFontWidget * wdiget;
};

class XWTexEditCSWidget : public QWidget
{
  Q_OBJECT

public:
  XWTexEditCSWidget(QWidget * parent = 0);

  void save();
  void setBgColor(const QColor & c);

private:
  XWSystemFontWidget * wdiget;
};

class XWTexEditOptionWidget : public QWidget
{
  Q_OBJECT

public:
  XWTexEditOptionWidget(QWidget * parent = 0);

  void save();
  void setBgColor(const QColor & c);

private:
  XWSystemFontWidget * wdiget;
};

class XWTexEditParamWidget : public QWidget
{
  Q_OBJECT

public:
  XWTexEditParamWidget(QWidget * parent = 0);

  void save();
  void setBgColor(const QColor & c);

private:
  XWSystemFontWidget * wdiget;
};

class XWTeXEditTexTWidget : public QWidget
{
  Q_OBJECT

public:
  XWTeXEditTexTWidget(QWidget * parent = 0);

  void save();
  void setBgColor(const QColor & c);

private:
  XWSystemFontWidget * wdiget;
};

class XWTexEditBackgroundWidget : public QWidget
{
  Q_OBJECT

public:
  XWTexEditBackgroundWidget(QWidget * parent = 0);

  QColor getColor() {return color;}

  void save();

signals:
  void colorChanged(const QColor & c);

private slots:
  void setColor();

private:
  QPushButton * colorButton;
  QColor color;
};

class QListWidget;
class QListWidgetItem;
class QStackedWidget;
class QDialogButtonBox;

class XW_TEXGUI_EXPORT XWTexEditSetting : public QDialog
{
  Q_OBJECT

public:
  XWTexEditSetting(QWidget * parent = 0);

public slots:
  void accept();

  void changePage(QListWidgetItem *current, QListWidgetItem *previous);

  void setBgColor(const QColor & c);

private:
  QListWidget *contentsWidget;
  QStackedWidget *pagesWidget;

  XWTeXEditTexTWidget * textSetting;
  XWTexEditCSWidget * csSetting;
  XWTexEditCommentWidget * commentSetting;
  XWTexEditOptionWidget * optionSetting;
  XWTexEditParamWidget * paramSetting;
  XWTexEditSPWidget * spSetting;
  XWTexEditBackgroundWidget * bgSetting;

  QDialogButtonBox * buttonBox;
};

#endif //XWTEXEDITSETTING_H
