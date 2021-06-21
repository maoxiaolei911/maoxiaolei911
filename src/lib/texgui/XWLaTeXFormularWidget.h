/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWLATEXFORMULARWIDGET_H
#define XWLATEXFORMULARWIDGET_H

#include <QWidget>
#include <QToolBox>
#include "XWTeXGuiType.h"

class QButtonGroup;
class QAbstractButton;


class XW_TEXGUI_EXPORT XWLaTeXFormularToolBox : public QToolBox
{
   Q_OBJECT

public:
   XWLaTeXFormularToolBox(QWidget * parent = 0);

signals:
	void formularClicked(const QString & symbol);
};


class XW_TEXGUI_EXPORT XWLaTeXFormularWidget : public QWidget
{
  Q_OBJECT

public:
   XWLaTeXFormularWidget(QWidget * parent = 0);

signals:
	void formularClicked(const QString & symbol);

protected slots:
   void buttonGroupClicked(QAbstractButton * button);

protected:
   QWidget * createButton(int iconwidth,int iconheight,
                          const QString & imgfile, 
                          const QString & formular);

protected:
   QButtonGroup * buttonGroup;
};

class XW_TEXGUI_EXPORT XWSetFormularWidget : public XWLaTeXFormularWidget
{
  Q_OBJECT

public:
   XWSetFormularWidget(QWidget * parent = 0);
};

class XW_TEXGUI_EXPORT XWFunctionFormularWidget : public XWLaTeXFormularWidget
{
  Q_OBJECT

public:
   XWFunctionFormularWidget(QWidget * parent = 0);
};

class XW_TEXGUI_EXPORT XWExponentFormularWidget : public XWLaTeXFormularWidget
{
  Q_OBJECT

public:
   XWExponentFormularWidget(QWidget * parent = 0);
};

class XW_TEXGUI_EXPORT XWLogarithmFormularWidget : public XWLaTeXFormularWidget
{
  Q_OBJECT

public:
   XWLogarithmFormularWidget(QWidget * parent = 0);
};

class XW_TEXGUI_EXPORT XWGeometryFormularWidget : public XWLaTeXFormularWidget
{
  Q_OBJECT

public:
   XWGeometryFormularWidget(QWidget * parent = 0);
};

class XW_TEXGUI_EXPORT XWStatisticsFormularWidget : public XWLaTeXFormularWidget
{
  Q_OBJECT

public:
   XWStatisticsFormularWidget(QWidget * parent = 0);
};

class XW_TEXGUI_EXPORT XWTrigonometricFormularWidget : public XWLaTeXFormularWidget
{
  Q_OBJECT

public:
   XWTrigonometricFormularWidget(QWidget * parent = 0);
};

class XW_TEXGUI_EXPORT XWCalculusFormularWidget : public XWLaTeXFormularWidget
{
  Q_OBJECT

public:
   XWCalculusFormularWidget(QWidget * parent = 0);
};

class XW_TEXGUI_EXPORT XWMatricFormularWidget : public XWLaTeXFormularWidget
{
  Q_OBJECT

public:
   XWMatricFormularWidget(QWidget * parent = 0);
};

class XW_TEXGUI_EXPORT XWTransformationFormularWidget : public XWLaTeXFormularWidget
{
  Q_OBJECT

public:
   XWTransformationFormularWidget(QWidget * parent = 0);
};

#endif //XWLATEXFORMULARWIDGET_H
