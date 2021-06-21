/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTEXTEXT_H
#define XWTEXTEXT_H

#include <QObject>
#include <QString>
#include <QRectF>
#include <QRect>
#include <QPainterPath>
#include <QUndoCommand>

#include "XWTeXGuiType.h"

#define XW_TEX_FONT_CMR  1
#define XW_TEX_FONT_CMM  2
#define XW_TEX_FONT_CMTT 3
#define XW_TEX_FONT_CMSY 4
#define XW_TEX_FONT_CMEX 5
#define XW_TEX_FONT_LASY 6
#define XW_TEX_FONT_MSAM 7
#define XW_TEX_FONT_MSBM 8

#define XW_TEX_FONT_TINY   5
#define XW_TEX_FONT_SCRIPT 7
#define XW_TEX_FONT_NOTE   8
#define XW_TEX_FONT_SMALL  9
#define XW_TEX_FONT_NORMAL 10
#define XW_TEX_FONT_large  12
#define XW_TEX_FONT_Large  14
#define XW_TEX_FONT_LARGE  17
#define XW_TEX_FONT_huge   20
#define XW_TEX_FONT_Huge   25

#define XW_TEX_FONT_TEXT  0

#define XW_TEX_UNKNOWN -1

#define XW_TEX_LETTER -2
#define XW_TEX_DIGIT  -3
#define XW_TEX_ASE    -4
#define XW_TEX_ROW    -5
#define XW_TEX_SUB    -6
#define XW_TEX_SUP    -7
#define XW_TEX_COL    -8
#define XW_TEX_PARAM  -9
#define XW_TEX_OPT    -10
#define XW_TEX_TEXT   -11

#define XW_TEX_POS_BEFORE  0
#define XW_TEX_POS_AFTER   1

#define XW_TEX_LEFT    0
#define XW_TEX_CENTER  1
#define XW_TEX_RIGHT   2

#define XW_TEX_FOMULAR     69002
#define XW_TEX_DISFOMULAR  69003

class QPainter;
class XWFTFontEngine;
class XWTeXTextBox;
class XWPDFDriver;

class XW_TEXGUI_EXPORT XWTeXText : public QObject
{
  Q_OBJECT

public:
  XWTeXText(int keywordA,XWTeXTextBox * boxA,QObject * parent = 0);

  void append(const QString & t);
  void append(XWTeXText * obj);

  virtual void breakLine(double maxW, double & curw);

  bool contains(double xA, double yA);
  bool cut();

  QChar deleteChar(int pos);
  virtual void doText(XWPDFDriver * driver, double xA, double yA,int s, int e);
  virtual void draw(QPainter * painter,const QRectF & r);
  virtual void draw(QPainter * painter);
  void dragTo(XWPDFDriver * driver, double xA, double yA);
  void dragTo(QPainter * painter, double xA, double yA);
  void dropTo(double xA,double yA);

  int getAnchorPosition() {return anchorPos;}
  QString getCurrentText();
  virtual void getDimension();
  static QString getFontName(int fam, int s,
                             bool bold,bool slant,
                             bool italic,bool cal);
  QString getFontName();
  QString getSelectedText();
  QString getSubstring(int pos, int len);
  virtual QString getText();
  int  getTextPosition() {return textPos;}
  int  getTextPosition(double xA,double yA);

  bool hasSelect() {return textPos == anchorPos;}
  virtual bool hitTest(double xA, double yA, double xr, double yr,int s, int e);

  int  insert(QChar c);
  void insert(int pos, const QString & t);
  void insert(XWTeXText * obj);
  void insertAtAfter(XWTeXText * obj);

  bool moveToNextChar();
  bool moveToPrevChar();

  void remove();
  void remove(int pos, int len);

  virtual void scan(const QString & str, int & len, int & pos);
          void setFontSize(int s);
  virtual void setXY(double & xA, double yA);

public:
  int keyWord;
  XWTeXTextBox * box;
  XWTeXText * head;
  XWTeXText * last;
  XWTeXText * next;
  XWTeXText * prev;
  double x,y,width,height,depth;
  QString text;

protected:
  void aseDimension();
  void amsaDimension(int c);
  void amsaDimensionLR(int c1, int c2, int c3);
  void amsbDimension(int c);
  void cdotsDimension();
  void charDimension(int fam, int c);
  void charsFourLR(int fam, int c1, int c2, int c3, int c4);
  void charsThreeLR(int fam, int c);
  void charsThreeLR(int fam, int c1, int c2, int c3);
  void charTwo(int fam, int c1, int c2);
  void charTwo(int fam, int c1, int fam2, int c2);
  void charsTwoLR(int fam, int c1, int c2);
  void charsTwoLR(int fam, int c1, int fam2, int c2);
  void charsTwoTB(int fam, int c1, int fam2, int c2);
  void ddotsDimension();
  void delimiterDimension();
  void doChildren(XWPDFDriver * driver, double xA, double yA,int s, int e);
  void dotsDimension();
  void drawAccent(QPainter * painter);
  void drawChar(QPainter * painter);
  void drawChildren(QPainter * painter);
  void drawDDots(QPainter * painter);
  void drawDotsLR(QPainter * painter);
  void drawHookArrow(QPainter * painter);
  void drawLeft(QPainter * painter);
  void drawNormalText(QPainter * painter);
  void drawNoteq(QPainter * painter);
  void drawNotin(QPainter * painter);
  void drawOverBrace(QPainter * painter);
  void drawOverTwoChars(QPainter * painter);
  void drawRight(QPainter * painter);
  void drawText(QPainter * painter);
  void drawText(QPainter * painter,  int n);
  void drawThreeCharsLR(QPainter * painter);
  void drawTwoCharsLR(QPainter * painter);
  void drawTwoCharsTB(QPainter * painter);
  void drawTwoTwoCharsTB(QPainter * painter);
  void drawUnderBrace(QPainter * painter);
  void drawUnderTwoChars(QPainter * painter);
  void drawVDots(QPainter * painter);
  void getMaxColumnWidth(QList<double> & list);
  void hookArrowDimension(int c1, int c2);
  void largeSymbolDimension(int c);
  void largeSymbolDimension(int och, int lsch);
  void largeSymbolDimensionLR(int c1, int c2, int c3, int c4);
  void lasyDimension(int c);
  void leftDimension();
  void lettersDimension();
  void letterDimension(int c);
  void letterDimensionLR(int c1, int c2);
  void LongArrowDimension(int c1, int c2);
  void modelsDimension();
  void neqDimension();
  void notinDimension();
  void operatorsDimension();
  void operatorsDimension(int n);
  void operatorDimension(int c);
  void operatorDimension(int och, int lsch);
  void operatorDimensionTB(int c1, int c2);
  void rightDimension();
  void scanArray(const QString & str, int & len, int & pos);
  void scanASE(const QString & str, int & len, int & pos);
  void scanColumn(const QString & str, int & len, int & pos);
  void scanDigit(const QString & str, int & len, int & pos);
  void scanEnvironment(const QString & str, int & len, int & pos);
  void scanFormular(const QString & str, int & len, int & pos);
  void scanOption(const QString & str, int & len, int & pos);
  void scanParam(const QString & str, int & len, int & pos);
  void scanRow(const QString & str, int & len, int & pos);
  void scanScript(const QString & str, int & len, int & pos);
  void scanText(const QString & str, int & len, int & pos);
  void scriptDimension();
  void setMaxColumnWidthAndAlign(QList<double> & w, QList<int> & a);
  void symbolDimension(int c);
  void symbolDimension(int sch, int lsch);
  void symbolDimensionLR(int c1, int c2);
  void symbolDimensionLR(int c1, int c2, int c3);
  void symbolDimensionTB(int c1, int c2);
  void textDimension();
  void textDimension(int fam);
  void textDimension(int fam, int n);
  void unknownDimension();
  void updateCursor(double xA, double yA);
  void vdotsDimension();
  void vertDimension(int c);

protected:  
  int family,family2,fontSize,textPos,anchorPos,align;
  bool bold,slant,italic,cal,sc,sf;
  double width2,height2,depth2;
};

class XW_TEXGUI_EXPORT XWTeXDeleteChar : public QUndoCommand
{
public:
  XWTeXDeleteChar(XWTeXText * textA, int posA,QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
  XWTeXText * text;
  int pos;
  QChar c;
};

class XW_TEXGUI_EXPORT XWTeXInsert : public QUndoCommand
{
public:
  XWTeXInsert(XWTeXText * newobjA,
                   XWTeXText * prevA,
                   QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
  XWTeXText * newObj;
  XWTeXText * prev;
};

class XW_TEXGUI_EXPORT XWTeXRemove : public QUndoCommand
{
public:
  XWTeXRemove(XWTeXText * objA, QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
  XWTeXText * obj;
};


class XW_TEXGUI_EXPORT XWTeXNew : public QUndoCommand
{
public:
  XWTeXNew(XWTeXText * newobjA, QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
  XWTeXText * newObj;
};

class XW_TEXGUI_EXPORT XWTeXAppend : public QUndoCommand
{
public:
  XWTeXAppend(XWTeXText * newobjA,
                          XWTeXText * pobjA,
                            QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
  XWTeXText * newObj;
  XWTeXText * pobj;
};


class XW_TEXGUI_EXPORT XWTeXRemoveString : public QUndoCommand
{
public:
  XWTeXRemoveString(XWTeXText * objA,
                          int sposA,
                          int eposA,
                            QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
  XWTeXText * obj;
  int spos;
  int epos;
  QString text;
};

class XW_TEXGUI_EXPORT XWTeXInsertString : public QUndoCommand
{
public:
  XWTeXInsertString(XWTeXText * objA,
                          int posA,
                          const QString & str,
                            QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
  XWTeXText * obj;
  int pos;
  QString text;
};


#endif //XWTEXTEXT_H
