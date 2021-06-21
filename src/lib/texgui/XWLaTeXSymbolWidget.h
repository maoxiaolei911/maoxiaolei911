/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWLATEXSYMBOLWIDGET_H
#define XWLATEXSYMBOLWIDGET_H

#include <QWidget>
#include <QToolBox>
#include <QPainterPath>
#include <QTransform>

#include "XWTeXGuiType.h"

class QButtonGroup;
class QAbstractButton;
class XWFTFont;

class XW_TEXGUI_EXPORT XWLaTeXSymbolWidget : public QWidget
{
  Q_OBJECT

public:
   XWLaTeXSymbolWidget(QWidget * parent = 0);

signals:
	void symbolClicked(const QString & symbol);

protected slots:
   void buttonGroupClicked(QAbstractButton * button);

protected:
   QWidget * createButton(const QPainterPath & path, 
                           const QString & cmd);
   QWidget * createButton(XWFTFont * font, int c, 
                           const QString & cmd);
   QWidget * createButton(XWFTFont * font, int c, 
                           const QTransform & m, 
                           const QString & cmd);
   QWidget * createButton(XWFTFont * font1, int ch1, 
                          XWFTFont * font2, int ch2, 
                          const QString & cmd);
   QWidget * createButton(XWFTFont * font1, int ch1, 
                          XWFTFont * font2, int ch2,                            
                          const QTransform & m1,
                          const QTransform & m2,
                          const QString & cmd);
   QWidget * createButton(XWFTFont * font1, int ch1, 
                          XWFTFont * font2, int ch2,  
                          XWFTFont * font3, int ch3,                            
                          const QTransform & m1,
                          const QTransform & m2,
                          const QTransform & m3,
                          const QString & cmd);

   XWFTFont * loadFont(const QString & filename, int index);

protected:
   QButtonGroup * buttonGroup;
};

class XW_TEXGUI_EXPORT XWGreeceLetterWidget : public XWLaTeXSymbolWidget
{
   Q_OBJECT

public:
   XWGreeceLetterWidget(QWidget * parent = 0);
};

class XW_TEXGUI_EXPORT XWBinRelOperatorWidget : public XWLaTeXSymbolWidget
{
   Q_OBJECT

public:
   XWBinRelOperatorWidget(QWidget * parent = 0);
};

class XW_TEXGUI_EXPORT XWBinOperatorWidget : public XWLaTeXSymbolWidget
{
   Q_OBJECT

public:
   XWBinOperatorWidget(QWidget * parent = 0);
};

class XW_TEXGUI_EXPORT XWBigOperatorWidget : public XWLaTeXSymbolWidget
{
   Q_OBJECT

public:
   XWBigOperatorWidget(QWidget * parent = 0);
};

class XW_TEXGUI_EXPORT XWArrowWidget : public XWLaTeXSymbolWidget
{
   Q_OBJECT

public:
   XWArrowWidget(QWidget * parent = 0);
};

class XW_TEXGUI_EXPORT XWDelimiterWidget : public XWLaTeXSymbolWidget
{
   Q_OBJECT

public:
   XWDelimiterWidget(QWidget * parent = 0);
};

class XW_TEXGUI_EXPORT XWOtherWidget : public XWLaTeXSymbolWidget
{
   Q_OBJECT

public:
   XWOtherWidget(QWidget * parent = 0);
};

class XW_TEXGUI_EXPORT XWAMSDelimiterLetterWidget : public XWLaTeXSymbolWidget
{
   Q_OBJECT

public:
   XWAMSDelimiterLetterWidget(QWidget * parent = 0);
};

class XW_TEXGUI_EXPORT XWAMSBinRelOperatorWidget : public XWLaTeXSymbolWidget
{
   Q_OBJECT

public:
   XWAMSBinRelOperatorWidget(QWidget * parent = 0);
};

class XW_TEXGUI_EXPORT XWAMSArrowWidget : public XWLaTeXSymbolWidget
{
   Q_OBJECT

public:
   XWAMSArrowWidget(QWidget * parent = 0);
};

class XW_TEXGUI_EXPORT XWAMSNotRelWidget : public XWLaTeXSymbolWidget
{
   Q_OBJECT

public:
   XWAMSNotRelWidget(QWidget * parent = 0);
};

class XW_TEXGUI_EXPORT XWAMSBinOperaterWidget : public XWLaTeXSymbolWidget
{
   Q_OBJECT

public:
   XWAMSBinOperaterWidget(QWidget * parent = 0);
};

class XW_TEXGUI_EXPORT XWAMSOtherWidget : public XWLaTeXSymbolWidget
{
   Q_OBJECT

public:
   XWAMSOtherWidget(QWidget * parent = 0);
};

class XW_TEXGUI_EXPORT XWLaTeXSymbolToolBox : public QToolBox
{
   Q_OBJECT

public:
   XWLaTeXSymbolToolBox(QWidget * parent = 0);

signals:
	void symbolClicked(const QString & symbol);
};

#endif // XWLATEXSYMBOLWIDGET_H
