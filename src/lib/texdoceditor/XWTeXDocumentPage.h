/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTEXDOCUMENTPAGE_H
#define XWTEXDOCUMENTPAGE_H

#include <QList>
#include <QColor>
#include <QFont>
#include <QFontMetricsF>
#include <QString>
#include <QPointF>
#include <QRectF>
#include <QPainter>

#include "XWTeXDocumentType.h"

class XWTeXDocument;
class XWTeXDocumentBlock;
class XWTeXDocSearhList;

class XW_TEXDOCUMENT_EXPORT XWTeXDocumentRow
{
public:
  XWTeXDocumentRow();

  void addBlock(XWTeXDocumentBlock * block);

  void clear();

  void draw(QPainter * painter,const QColor & pagecolor,const QRectF & rect);
  void drawPic(QPainter * painter);

  void find(int pg, XWTeXDocSearhList * list);
  void find(QList<XWTeXDocumentBlock*> & blocksA);
  bool find(int pg, XWTeXDocumentBlock * blockA, XWTeXDocSearhList * list);
  bool findNext();

  void   getBoundRect(double & minX, double & minY, double & maxX, double & maxY)
    {minX = minx; minY = miny; maxX = maxx; maxY = maxy;}

  double getLimit() {return limit;}
  double getMaxLength() {return maxlength;}
  double getMaxX() {return maxx;}
  double getMaxY() {return maxy;}
  double getMinX() {return minx;}
  double getMinY() {return miny;}

  bool hitTest(const QPointF & p);

  void moveToEnd();
  void moveToHitPos();
  bool moveToNextChar(bool & m);
  bool moveToNextWord(bool & m);
  bool moveToPreviousChar(bool & m);
  bool moveToPreviousWord(bool & m);
  void moveToStart();

  bool replaceNext();
  void reset();

  void selectEnd(double & minX, double & minY, double & maxX, double & maxY);
  int  selectNextChar(double & minX, double & minY, double & maxX, double & maxY);
  int  selectNextWord(double & minX, double & minY, double & maxX, double & maxY);
  int  selectPreviousChar(double & minX, double & minY, double & maxX, double & maxY);
  int selectPreviousWord(double & minX, double & minY, double & maxX, double & maxY);
  void selectRow(double & minX, double & minY, double & maxX, double & maxY);
  void selectStart(double & minX, double & minY, double & maxX, double & maxY);
  bool selectWord(double & minX, double & minY, double & maxX, double & maxY);
  void setDirection(int d) {dir=d;}
  void setEndPos(int pos) {endPos=pos;}
  void setLimit(double l) {limit = l;}
  void setMaxLength(double l) {maxlength=l;}
  void setMaxX(double x) {maxx = x;}
  void setMaxY(double y) {maxy = y;}
  void setMinX(double x) {minx = x;}
  void setMinY(double y) {miny = y;}
  void setSelected(const QRectF & rect);
  void setStartPos(int pos) {startPos=pos;}

public:
  int startPos;
  int endPos;
  int dir,index;
  double limit,minx,miny,maxx,maxy,maxlength;
  XWTeXDocumentBlock * curBlock;
  QList<XWTeXDocumentBlock*> blocks;
};

class XW_TEXDOCUMENT_EXPORT XWTeXDocumentPage
{
public:
  XWTeXDocumentPage();
  ~XWTeXDocumentPage();

  void clear();

  void draw(QPainter * painter,const QColor & pagecolor,const QRectF & rect);
  void drawPic(QPainter * painter);

  void find(int pg, XWTeXDocSearhList * list);
  void find(QList<XWTeXDocumentBlock*> & blocksA);
  bool find(int pg, XWTeXDocumentBlock * blockA, XWTeXDocSearhList * list);
  bool findNext();

  XWTeXDocumentRow * getCurrentRow();
  XWTeXDocumentRow * getNewRow();

  bool hitTest(const QPointF & p);

  bool isEmpty();

  void moveToEnd();
  void moveToFirstRow();
  void moveToLastRow();
  bool moveToNextChar(bool & m);
  bool moveToNextRow();
  bool moveToNextWord(bool & m);
  bool moveToPreviousChar(bool & m);
  bool moveToPreviousRow();
  bool moveToPreviousWord(bool & m);
  void moveToRowEnd();
  void moveToRowStart();
  void moveToStart();

  bool replaceNext();
  void reset();

  void selectAll(double & minX, double & minY, double & maxX, double & maxY);
  void selectEnd(double & minX, double & minY, double & maxX, double & maxY);
  void selectFirstRow(bool all, double & minX, double & minY, double & maxX, double & maxY);
  void selectLastRow(bool all, double & minX, double & minY, double & maxX, double & maxY);
  bool selectNextChar(double & minX, double & minY, double & maxX, double & maxY);
  bool selectNextRow(double & minX, double & minY, double & maxX, double & maxY);
  bool selectNextWord(double & minX, double & minY, double & maxX, double & maxY);
  bool selectPreviousChar(double & minX, double & minY, double & maxX, double & maxY);
  bool selectPreviousRow(double & minX, double & minY, double & maxX, double & maxY);
  bool selectPreviousWord(double & minX, double & minY, double & maxX, double & maxY);
  void selectRow(double & minX, double & minY, double & maxX, double & maxY);
  void selectRowEnd(double & minX, double & minY, double & maxX, double & maxY);
  void selectRowStart(double & minX, double & minY, double & maxX, double & maxY);
  bool selectWord(double & minX, double & minY, double & maxX, double & maxY);
  void setSelected(const QRectF & rect);
  void selectStart(double & minX, double & minY, double & maxX, double & maxY);

public:
  int maxRow;
  int lastRow;
  int curRow;
  XWTeXDocumentRow** rows;
};

class XW_TEXDOCUMENT_EXPORT XWTeXDocumentCursor
{
public:
  XWTeXDocumentCursor();
  XWTeXDocumentCursor(XWTeXDocumentBlock * blockA);

  XWTeXDocumentBlock * getBlock() {return block;}
  int getEndPos() {return endPos;}
  int getHitPos() {return hitPos;}
  int getStartPos() {return startPos;}

  bool hasSelect() {return (startPos < endPos) || selected;}

  bool isSelected() {return selected;}
  bool isSelected(int i) {return (i >= startPos && i < endPos);}

  void setBlock(XWTeXDocumentBlock * blockA) {block = blockA;}
  void setEndPos(int pos) {endPos = pos;}
  void setHitPos(int pos) {hitPos = pos;}
  void setSelected(bool s) {selected = s;}
  void setStartPos(int pos) {startPos = pos;}

public:
  XWTeXDocumentBlock * block;
  int hitPos;
  int startPos;
  int endPos;
  bool selected;
};

#endif //XWTEXDOCUMENTPAGE_H
