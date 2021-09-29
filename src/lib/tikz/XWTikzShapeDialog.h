/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTIKZSHAPEDIALOG_H
#define XWTIKZSHAPEDIALOG_H

#include <QDialog>
#include <QPixmap>
#include "XWTeXBox.h"

class QAbstractButton;
class QLabel;
class QComboBox;
class QDialogButtonBox;
class QToolButton;
class QButtonGroup;
class QUndoCommand;

class XWTikzAnchorDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWTikzAnchorDialog(QWidget * parent = 0);

  int getAnchor();

  void setAnchor(int d);

private:
  void createAnchors();
  void createAnchor(int index, int a, const QString & str);

private:
  QLabel * label;
  QComboBox * anchors;

  QDialogButtonBox *buttonBox;
};

class XWTikzShapeWidget : public QWidget
{
  Q_OBJECT

public:
   XWTikzShapeWidget(QWidget * parent = 0);

signals:
  void shapeSelected(int s);

private slots:
  void buttonGroupClicked(QAbstractButton * button);

private:
  QToolButton * createButton(const QString & txt);

private:
  QButtonGroup * buttonGroup;
};

class XWTikzShapeDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWTikzShapeDialog(QWidget * parent = 0);

  int getShape();

public slots:
  void setShape(int d);

private:
  int shape;
  XWTikzShapeWidget * shapeWidget;
  
  QDialogButtonBox *buttonBox;
};

class XWTikzCircuiteeWidget : public QWidget
{
  Q_OBJECT

public:
   XWTikzCircuiteeWidget(const QString & styleA,QWidget * parent = 0);

signals:
  void shapeSelected(int s);

private slots:
  void buttonGroupClicked(QAbstractButton * button);

private:
  QToolButton * createButton(const QString & txt);

private:
  QString style;
  QButtonGroup * buttonGroup;
};

class XWTikzCircuiteeDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWTikzCircuiteeDialog(int styleA,QWidget * parent = 0);

  int getShape();

public slots:
  void setShape(int d);

private:
  int shape;
  XWTikzCircuiteeWidget * shapeWidget;
  
  QDialogButtonBox *buttonBox;
};

class XWTikzCircuitLogicWidget : public QWidget
{
  Q_OBJECT

public:
   XWTikzCircuitLogicWidget(const QString & styleA,QWidget * parent = 0);

signals:
  void shapeSelected(int s);

private slots:
  void buttonGroupClicked(QAbstractButton * button);

private:
  QToolButton * createButton(const QString & txt);

private:
  QString style;
  QButtonGroup * buttonGroup;
};

class XWTikzCircuitLogicDialog : public QDialog
{
  Q_OBJECT
	
public:       
  XWTikzCircuitLogicDialog(int styleA,QWidget * parent = 0);

  int getShape();

public slots:
  void setShape(int d);

private:
  int shape;
  XWTikzCircuitLogicWidget * shapeWidget;
  
  QDialogButtonBox *buttonBox;
};

#endif //XWTIKZSHAPEDIALOG_H
