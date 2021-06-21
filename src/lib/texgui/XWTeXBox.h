/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTEXBOX_H
#define XWTEXBOX_H

#include <QObject>
#include <QString>

#include "XWTeXGuiType.h"

class QUndoCommand;
class QPainter;
class XWPDFDriver;

class XW_TEXGUI_EXPORT XWTeXBox : public QObject
{
  Q_OBJECT

public:
  XWTeXBox(QObject * parent = 0);

  virtual bool back();

  virtual bool contains(double xA, double yA);
  virtual bool cut();

  virtual bool del();
  virtual void doContent(XWPDFDriver * driver);
  virtual void dragTo(XWPDFDriver * driver,double xA,double yA);
  virtual void dragTo(QPainter * painter,double xA,double yA);
  virtual void draw(QPainter * painter);
  virtual bool dropTo(double xA,double yA);

  virtual int getAnchorPosition();
  virtual int getCursorPosition();
  virtual QString getCurrentText();
  virtual double getDepth(double maxw,int nop=0);
  virtual double getHeight(double maxw,int nop=0);
  virtual int getNumberOfParts() {return numberOfParts;}
  virtual QString getSelected();
  virtual QString getSelectedText();
  virtual QString getText();
  virtual double getWidth(double maxw,int nop=0);
  virtual void goToEnd();
  virtual bool goToNext();
  virtual bool goToPrevious();
  virtual void goToStart();

  virtual bool hitTest(double xA,double xB);

  virtual bool insert(const QString & str, QUndoCommand * cmdA = 0);
  virtual bool insertText(const QString & str);
  virtual bool isTextBox() {return false;}

  virtual bool keyInput(const QString & str);

  virtual bool newPar();

  virtual bool paste();
  virtual void push(QUndoCommand * );

  virtual bool removeSelected(QUndoCommand * cmdA);

  virtual void scan(const QString & str, int & len, int & pos);
  static  QString scanControlSequence(const QString & str, int & len, int & pos);
  static  QString scanEnviromentName(const QString & str, int & len, int & pos);
  virtual void setContents(const QString & str);
  virtual void setFontSize(int s);
  virtual void setNumberOfParts(int n) {numberOfParts=n;}
  virtual void setXY(int align, double xA,double yA, int nop = 0);
  static  void skipComment(const QString & str, int & len, int & pos);

protected:
  int  keyWord;
  int  numberOfParts;
};

#endif //XWTEXBOX_H
