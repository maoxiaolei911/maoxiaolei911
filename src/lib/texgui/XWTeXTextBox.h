/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTEXTEXTBOX_H
#define XWTEXTEXTBOX_H

#include <QList>
#include <QRectF>

#include "XWTeXBox.h"

class QPainter;
class XWFTFontEngine;
class XWFTFont;
class XWTeXText;
class XWPDFDriver;
class XWTeXTextBox;

class XWTeXTextRow
{
public:
  XWTeXTextRow();

  void add(XWTeXText * obj);

  void clear();

  bool contains(double xA, double yA);

  void doContent(XWPDFDriver * driver);
  void draw(QPainter * painter);
  void draw(QPainter * painter,const QRectF & r);

  XWTeXText * getFirst();
  XWTeXText * getLast();
  XWTeXText * getObject(double xA, double yA);

  bool hitTest(double xA, double yA);

  void setEndPos(int p) {endPos=p;}
  void setStartPos(int p) {startPos=p;}
  void setXY(double & xA, double yA);

public:
  int startPos,endPos;
  double x,y,width,height,depth;
  QList<XWTeXText*> objs;
};


class XW_TEXGUI_EXPORT XWTeXTextBoxPart : public QObject
{
  Q_OBJECT

public:
  XWTeXTextBoxPart(XWTeXTextBox * boxA,QObject * parent = 0);
  virtual ~XWTeXTextBoxPart();

  void append(XWTeXText*obj);
  
  virtual bool back();

  bool contains(double xA, double yA);
  virtual bool cut();

  virtual bool del();
  QChar deleteChar(int pos);
  virtual void doContent(XWPDFDriver * driver);
  virtual void draw(QPainter * painter,const QRectF & r);
  virtual void draw(QPainter * painter);
  virtual void drawChar(XWPDFDriver * driver, int i);
  void dragTo(XWPDFDriver * driver, double xA, double yA);
  void dragTo(QPainter * painter, double xA, double yA);
  virtual bool dropTo(double xA,double yA);

  int getAnchorPosition();
  int getCharacterNumber();
  double getCharHeight(int i);
  double getCharWidth(int i);
  int getCursorPosition() {return textPos;}
  XWTeXTextRow * getCurrentRow();
  virtual XWTeXText * getCurrent();
  QString getCurrentText();
  double getDepth(double maxw);
  double getHeight(double maxw);
  XWTeXTextRow * getNewRow();
  XWTeXText * getObject(double xA, double yA);
  virtual QString getSelected();
  QString getSelectedText();
  int     getSpaceNumber();
  virtual QString getText();
  virtual double getWidth(double maxw);
  void goToEnd();
  bool goToNext();
  bool goToPrevious();
  void goToStart();

  virtual bool hitTest(double xA, double yA);

  void insert(XWTeXText * newobj);
  virtual bool insert(const QString & str, QUndoCommand * cmdA = 0);
  bool insertText(const QString & str);
  bool isSpace(int i);
  bool isTextBox() {return true;}

  virtual bool keyInput(const QString & str);

  XWFTFont * loadFTFont(int fam, int s,bool bold,bool slant,
                         bool italic,bool cal);

  void moveToEndOfLine();
  void moveToNextLine();
  void moveToPreLine();
  void moveToStartOfLine();

  virtual bool newPar();

  virtual bool paste();

  void remove(XWTeXText*obj);
  virtual bool removeSelected(QUndoCommand * cmdA);

  virtual void setContents(const QString & str);
  void setCurrentObject(XWTeXText * obj) {curObj=obj;}
  virtual void setFontSize(int s);
  void setTextPos(int p) {textPos=p;}
  virtual void setXY(int align, double xA,double yA);

  void typeset(double & pagewidth,double & pageheight);

public slots:
  virtual void addSubscript();
  virtual void addSupscript();
  virtual void clear();

  void delColumn();
  void delRow();

  void insertColumn();
  void insertRow();

protected:
  void breakLine(double maxw);

protected:
  XWTeXTextBox * box;
  int  textPos;

  int maxRow;
  int lastRow;
  int curRow;
  XWTeXTextRow ** rows;

  XWTeXText * curObj;
  XWTeXText* head;
  XWTeXText* last;
};

class XW_TEXGUI_EXPORT XWTeXTextBox : public XWTeXBox
{
  Q_OBJECT

public:
  XWTeXTextBox(QObject * parent = 0);
  virtual ~XWTeXTextBox();

  virtual bool back();

  bool contains(double xA, double yA);
  virtual bool cut();

  virtual bool del();
  QChar deleteChar(int pos);
  virtual void doContent(XWPDFDriver * driver);
  virtual void draw(QPainter * painter,const QRectF & r);
  virtual void draw(QPainter * painter);
  virtual void drawChar(XWPDFDriver * driver, int i);
  void dragTo(XWPDFDriver * driver, double xA, double yA);
  void dragTo(QPainter * painter, double xA, double yA);
  virtual bool dropTo(double xA,double yA);

  int getAnchorPosition();
  int getCharacterNumber();
  double getCharHeight(int i);
  double getCharWidth(int i);
  int getCursorPosition();
  virtual XWTeXTextRow * getCurrentRow();
  virtual XWTeXText * getCurrent();
  QString getCurrentText();
  double getDepth(double maxw, int nop = 0);
  double getHeight(double maxw, int nop = 0);
  virtual XWTeXTextRow * getNewRow();
  XWTeXText * getObject(double xA, double yA);
  virtual QString getSelected();
  QString getSelectedText();
  int     getSpaceNumber();
  virtual QString getText();
  virtual double getWidth(double maxw, int nop = 0);
  void goToEnd();
  bool goToNext();
  bool goToPrevious();
  void goToStart();

  virtual bool hitTest(double xA, double yA);

  void insert(XWTeXText * newobj);
  virtual bool insert(const QString & str, QUndoCommand * cmdA = 0);
  bool insertText(const QString & str);
  bool isSpace(int i);
  bool isTextBox() {return true;}

  virtual bool keyInput(const QString & str);

  XWFTFont * loadFTFont(int fam, int s,bool bold,bool slant,
                         bool italic,bool cal);

  virtual void moveToEndOfLine();
  virtual void moveToNextLine();
  virtual void moveToPreLine();
  virtual void moveToStartOfLine();

  virtual bool newPar();

  virtual bool paste();

  void remove(XWTeXText*obj);
  virtual bool removeSelected(QUndoCommand * cmdA);

  virtual void setContents(const QString & str);
  void setCurrentObject(XWTeXText * obj);
  virtual void setFontSize(int s);
  virtual void setXY(int align, double xA,double yA, int nop = 0);

  void typeset(double & pagewidth,double & pageheight);

  virtual void updateCursor(double minx, double miny, double maxx,
                    double maxy, int textposA);

public slots:
  virtual void clear();

  void delColumn();
  void delRow();

  void insertColumn();
  void insertRow();

signals:
  void cursorChanged(double minxA,double minyA,double maxxA,double maxyA);

protected:
  int cur;
  XWFTFontEngine * fontEngine;
  QList<XWTeXTextBoxPart*> parts;
};

#endif //XWTEXTEXTBOX_H
