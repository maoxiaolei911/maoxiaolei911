/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTEXDOCFONTSETTING_H
#define XWTEXDOCFONTSETTING_H

#include <QObject>
#include <QDialog>
#include "XWTeXDocumentType.h"

class QComboBox;
class QPushButton;
class XWSystemFontWidget;

class XWTexDocFontWidget : public QWidget
{
  Q_OBJECT

public:
  XWTexDocFontWidget(QWidget * parent = 0);

  virtual void get();

  virtual void set();
  void setBgColor(const QColor & c);

protected:
  void getSetting(const QString & grp);

  void setSetting(const QString & grp);

private:
  XWSystemFontWidget * wdiget;
};

class XWTexDocNormalFontWidget : public XWTexDocFontWidget
{
  Q_OBJECT

public:
  XWTexDocNormalFontWidget(QWidget * parent = 0);

  void get();

  void set();
};

class XWTexDocBoldFontWidget : public XWTexDocFontWidget
{
  Q_OBJECT

public:
  XWTexDocBoldFontWidget(QWidget * parent = 0);

  void get();

  void set();
};

class XWTexDocItalicFontWidget : public XWTexDocFontWidget
{
  Q_OBJECT

public:
  XWTexDocItalicFontWidget(QWidget * parent = 0);

  void get();

  void set();
};

class XWTexDocMathFontWidget : public XWTexDocFontWidget
{
  Q_OBJECT

public:
  XWTexDocMathFontWidget(QWidget * parent = 0);

  void get();

  void set();

private:
  XWSystemFontWidget * wdiget;
};

class XWTexDocTypewriterFontWidget : public XWTexDocFontWidget
{
  Q_OBJECT

public:
  XWTexDocTypewriterFontWidget(QWidget * parent = 0);

  void get();

  void set();

private:
  XWSystemFontWidget * wdiget;
};

class XWTexDocCommentFontWidget : public XWTexDocFontWidget
{
  Q_OBJECT

public:
  XWTexDocCommentFontWidget(QWidget * parent = 0);

  void get();

  void set();

private:
  XWSystemFontWidget * wdiget;
};

class XWTexDocControlSequenceFontWidget : public XWTexDocFontWidget
{
  Q_OBJECT

public:
  XWTexDocControlSequenceFontWidget(QWidget * parent = 0);

  void get();

  void set();

private:
  XWSystemFontWidget * wdiget;
};

class XWTexDocParameterFontWidget : public XWTexDocFontWidget
{
  Q_OBJECT

public:
  XWTexDocParameterFontWidget(QWidget * parent = 0);

  void get();

  void set();

private:
  XWSystemFontWidget * wdiget;
};

class XWTexDocOptionFontWidget : public XWTexDocFontWidget
{
  Q_OBJECT

public:
  XWTexDocOptionFontWidget(QWidget * parent = 0);

  void get();

  void set();

private:
  XWSystemFontWidget * wdiget;
};

class XWTexDocPageColorWidget : public QWidget
{
  Q_OBJECT

public:
  XWTexDocPageColorWidget(QWidget * parent = 0);

  void get();
  QColor getColor() {return color;}

  void set();

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

class XW_TEXDOCUMENT_EXPORT XWTexDocFontSetting : public QDialog
{
  Q_OBJECT

public:
  XWTexDocFontSetting(QWidget * parent = 0);

public slots:
  void accept();

  void changeCurrent(const QString & s);
  void changePage(QListWidgetItem *current, QListWidgetItem *previous);

  void setBgColor(const QColor & c);
  void setCurrent(const QString & s);

private:
  QComboBox * stylesCombo;
  QListWidget *contentsWidget;
  QStackedWidget *pagesWidget;

  XWTexDocFontWidget * normalFont;
  XWTexDocFontWidget * boldFont;
  XWTexDocFontWidget * italicFont;
  XWTexDocFontWidget * mathFont;
  XWTexDocFontWidget * typewriterFont;
  XWTexDocFontWidget * commentFont;
  XWTexDocFontWidget * csFont;
  XWTexDocFontWidget * paramFont;
  XWTexDocFontWidget * optionFont;
  XWTexDocPageColorWidget * pageColor;

  QDialogButtonBox * buttonBox;
};

#endif //XWTEXDOCFONTSETTING_H
