/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTEXDOCUMENTOBJECT_H
#define XWTEXDOCUMENTOBJECT_H

#include <QObject>
#include <QList>
#include <QHash>
#include <QColor>
#include <QFont>
#include <QFontMetricsF>
#include <QString>
#include <QPointF>
#include <QRectF>
#include <QPainter>
#include <QTextStream>

#include "XWTeXDocumentType.h"

#define TEX_DOC_P_OPTION  0x00000001
#define TEX_DOC_P_COMMENT 0x00000010

#define TEX_TIP_DARKER 250

class QMimeData;
class XWTeXDocument;
class XWTeXDocumentBlock;
class XWTeXDocumentRow;
class XWTeXDocumentPage;

class XW_TEXDOCUMENT_EXPORT XWTeXDocumentObject : public QObject
{
  Q_OBJECT

public:
  XWTeXDocumentObject(int tA, XWTeXDocument * docA, QObject * parent = 0);
  XWTeXDocumentObject(int tA, XWTeXDocument * docA, bool newrow, QObject * parent = 0);
  XWTeXDocumentObject(int tA, XWTeXDocument * docA, bool newpage, bool newrow,
                      QObject * parent = 0);
  virtual ~XWTeXDocumentObject();
  
  void add(XWTeXDocumentObject*obj);
  void addIndent(double i) {indent += i;}
  virtual void append(XWTeXDocumentObject * obj);
  virtual void append(XWTeXDocumentObject * sobj,XWTeXDocumentObject * eobj);

  virtual void breakPage(const QFont & fontA,const QColor & c,
                        double & curx,double & cury,
                        bool & firstcolumn);
  virtual void breakPage(double & curx,double & cury,bool & firstcolumn);

  virtual void clear();
  virtual void centered(const QFont & fontA,  const QColor & c,double & curx, double & cury, bool & firstcolumn);
  virtual void centered(double & curx,double & cury,bool & firstcolumn);


  void del(XWTeXDocumentObject*obj);  
  XWTeXDocument * document() {return doc;}

  virtual void flushBottom(const QFont & fontA,const QColor & c,
                            double & curx,double & cury,bool & firstcolumn);
  virtual void flushBottom(double & curx,double & cury,bool & firstcolumn);

  QFont getFont();
  virtual QString getMimeData();
  XWTeXDocumentObject * getNext() {return next;}
  XWTeXDocumentObject * getPrevious() {return prev;}
  QString getRomanNumber(int n);
  QString getSelected();

  virtual bool hasSelected();
          bool hasSpecialFont() {return specialFont;}
  virtual double height();

  virtual void insert(XWTeXDocumentObject * obj);  
          void insertAtAfter(XWTeXDocumentObject * obj);
          void insertAtBefore(XWTeXDocumentObject * obj);
  virtual void insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj);
  virtual void insertChildren(XWTeXDocumentObject*obj, XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj);

  virtual bool isAllSelected();
  virtual bool isAtEnd();
  virtual bool isAtStart();
          bool isAlone() { return next == 0 && prev == 0;}
  virtual bool isBlock() {return false;}
  virtual bool isEmpty();
          bool isReadOnly() {return isOnlyRead;}
          bool isTopLevel();

  virtual void remove();
  virtual void removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj);
  virtual void removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj,XWTeXDocumentObject**obj);
  virtual void removeChildrenFrom(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj,XWTeXDocumentObject**obj);
  virtual void removeChildrenTo(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj);

  virtual void scan(const QString & str, int & len, int & pos);
  static  QString scanControlSequence(const QString & str, int & len, int & pos);
  static  QString scanEnviromentName(const QString & str, int & len, int & pos);
          void setBoldFont();
          void setCommentFont();
          void setControlSequenceFont();
  virtual void setCursor();
  virtual void setCursorAtEnd();
  virtual void setCursorAtStart();
  virtual void setFirstIndent(double indentA) {indent=indentA;}
  virtual void setFont();
          void setFont(const QString & group);
  virtual void setIndent(double indentA) {indent=indentA;}
          void setItalicFont();
          void setMathFont();
  virtual void setNewPage(bool n) {newPage=n;}
  virtual void setNewRow(bool n) {newRow=n;}
          void setNormalFont();
          void setOptionFont();
          void setParamFont();
          void setVerbFont();
  static  void skipCoord(const QString & str, int & len, int & pos);
  static  void skipGroup(const QString & str, int & len, int & pos);
  static  void skipOption(const QString & str, int & len, int & pos);

          int type() {return objectType;}

  virtual double width();
  virtual void write(QTextStream & strm, int & linelen);

public:
  int objectType;
  XWTeXDocument * doc;
  double indent;
  double firstIndent;
  bool newPage;
  bool newRow;
  bool afterNewRow;
  bool isOnlyRead;
  int dir;
  bool specialFont;
  XWTeXDocumentObject * head;
  XWTeXDocumentObject * last;
  XWTeXDocumentObject * next;
  XWTeXDocumentObject * prev;

  QFont font;
  QColor color;

protected:
  XWTeXDocumentRow * getRowForTRT(const QFontMetricsF & metrics, bool reset,
                                  double & curx, double & cury, bool & firstcolumn);
  XWTeXDocumentRow * getRowForTLT(const QFontMetricsF & metrics, bool reset,
                                  double & curx, double & cury,  bool & firstcolumn);
  XWTeXDocumentRow * getRowForRTT(const QFontMetricsF & metrics, bool reset,
                                  double & curx, double & cury, bool & firstcolumn);
  XWTeXDocumentRow * getRowForLTL(const QFontMetricsF & metrics, bool reset,
                                  double & curx, double & cury, bool & firstcolumn);
  void writeStr(const QString & str, QTextStream & strm, int & linelen);
};

#define TEX_DOC_B_TEXT        69000
#define TEX_DOC_B_COMMENT     69001
#define TEX_DOC_B_FOMULAR     69002
#define TEX_DOC_B_DISFOMULAR  69003


#define TEX_DOC_B_UNKNOWN   -1
#define TEX_DOC_B_CS        -2
#define TEX_DOC_B_VERB      -3
#define TEX_DOC_B_NAME      -4
#define TEX_DOC_OPTION      -5
#define TEX_DOC_PARAM       -6
#define TEX_DOC_ENV         -7
#define TEX_DOC_BEAMER_SPEC -8

class XW_TEXDOCUMENT_EXPORT XWTeXDocumentBlock : public XWTeXDocumentObject
{
  Q_OBJECT

public:
  XWTeXDocumentBlock(int tA, XWTeXDocument * docA, QObject * parent = 0);
  XWTeXDocumentBlock(int tA, XWTeXDocument * docA,
                     bool newrow,
                     QObject * parent = 0);
  XWTeXDocumentBlock(int tA, XWTeXDocument * docA, bool newrow, const QString & str, QObject * parent = 0);
  virtual ~XWTeXDocumentBlock();

  void append(const QString & str);
  void append(QChar c);

  virtual void breakPage(const QFont & fontA,  const QColor & c,
                         double & curx, double & cury,bool & firstcolumn);
  virtual void breakPage(double & curx, double & cury, bool & firstcolumn);

  virtual bool canDeleteChar();
  virtual bool canDeletePreviousChar();

  double charWidth(QChar c);
  virtual void clear();

  virtual QString del();
  virtual QChar deleteChar();
  virtual QChar deletePreviousChar();
  virtual void draw(QPainter * painter, const QColor & pagecolor, const QRectF & rect,
                    double & curx, double & cury, int from, int to);
  virtual void drawPic(QPainter * painter, double & curx, double & cury, int from, int to);
  virtual void draw(QPainter * painter, const QColor & pagecolor, const QRectF & rect,
                    double & curx, double & cury, int from);
  virtual void drawPic(QPainter * painter, double & curx, double & cury, int from);
  virtual void draw(QPainter * painter, const QColor & pagecolor, const QRectF & rect,
                    double & curx, double & cury);
  virtual void drawPic(QPainter * painter, double & curx, double & cury);

  virtual bool find() {return false;}
  virtual bool find(int & s,int & e, double & cur,
                    double & mi, double & ma,
                    QString & content);
  virtual bool findReplaced(int & s,int & e, double & cur,
                            double & mi, double & ma,
                            QString & content);
  virtual bool findNext(int,int) {return false;}
  virtual void flushBottom(const QFont & fontA,const QColor & c,double & curx,double & cury, bool & firstcolumn);
  virtual void flushBottom(double & curx,double & cury, bool & firstcolumn);

  QChar getChar(int i);
  int   getCurrentPos();
  int   getEndPos();
  int   getLength() {return text.length();}
  virtual QString getMimeData();
  XWTeXDocumentObject * getObject();  
  QString getSelected();
  int     getStartPos();
  QString getText() {return text;}

  virtual bool hasSelected();
        double height();
  virtual bool hitTest(const QPointF & p, double & curx, double & cury, int from, int to);
  virtual bool hitTest(const QPointF & p, double & curx, double & cury,int from);
  virtual bool hitTest(const QPointF & p, double & curx, double & cury);

  virtual void insert(XWTeXDocumentObject * obj);
  virtual void insert(int pos, const QString & str);
  virtual void insert(int pos,QChar c);

  bool isAllSelected();
  bool isAtEnd();
  bool isAtStart();
  bool isBlock() {return true;}
  virtual bool isEmpty() {return text.isEmpty();}

  virtual void moveTo(double & curx, double & cury,int from, int to, int pos);
  virtual void moveTo(double & curx, double & cury,int from, int pos);
  virtual void moveTo(double & curx, double & cury,int pos);
          void moveToEnd();
          void moveToFirst();
  virtual void moveToHitPos(double & curx, double & cury,int from, int to);
  virtual void moveToHitPos(double & curx, double & cury,int from);
  virtual void moveToHitPos(double & curx, double & cury);
          void moveToLast();
          void moveToNextChar();
          void moveToNextWord();
          void moveToPreviousChar();
          void moveToPreviousWord();
  virtual void moveToRowEnd(double & curx, double & cury,int pos);
  virtual void moveToRowStart(double & curx, double & cury,int pos);
          void moveToStart();

  virtual bool replaceNext(int, int) {return false;}
  virtual void resetSelect();

          void scanBeamerSpec(const QString & str, int & len, int & pos);
          void scanCoord(const QString & str, int & len, int & pos);
  virtual void scanGroup(const QString & str, int & len, int & pos);
  virtual void scanOption(const QString & str, int & len, int & pos);
  virtual void scanParam(const QString & str, int & len, int & pos);
          void selectBack(int pos);
          void selectBlock();
          void selectEndOfBlock();
          void selectForward(int pos);
          void selectNextChar();
          void selectNextWord();
          void selectPreviousChar();
          void selectPreviousWord();
          void selectStartOfBlock();
          void selectWord();
  virtual void setCursor();
  virtual void setCursorAtEnd();
  virtual void setCursorAtStart();
  virtual void setSelected(const QRectF & rect, double & curx, double & cury,
                                 int from, int to);
  virtual void setSelected(const QRectF & rect, double & curx, double & cury,
                                int from);
  virtual void setSelected(const QRectF & rect, double & curx, double & cury);
          void setSelected(int s, int e);
          void setText(const QString & str) {text = str;}
          void skip(int s, int e, double & cur);

          double width();
  virtual void write(QTextStream & strm, int & linelen);

protected:
  virtual void breakPageForTRT(double & curx,double & cury,bool & firstcolumn);
  virtual void breakPageForTLT(double & curx,double & cury,bool & firstcolumn);
  virtual void breakPageForRTT(double & curx,double & cury,bool & firstcolumn);
  virtual void breakPageForLTL(double & curx,double & cury,bool & firstcolumn);
  virtual void drawForTRT(QPainter * painter,const QColor & pagecolor, const QRectF & rect,
                          double & curx, double & cury,int from, int to);
  virtual void drawPicForTRT(QPainter * painter, double & curx, double & cury,int from, int to);
  virtual void drawForTLT(QPainter * painter,const QColor & pagecolor, const QRectF & rect,
                          double & curx, double & cury,int from, int to);
  virtual void drawPicForTLT(QPainter * painter,  double & curx, double & cury,int from, int to);
  virtual void drawForRTT(QPainter * painter,const QColor & pagecolor, const QRectF & rect,
                          double & curx, double & cury,int from, int to);
  virtual void drawPicForRTT(QPainter * painter,double & curx, double & cury,int from, int to);
  virtual void drawForLTL(QPainter * painter,const QColor & pagecolor, const QRectF & rect,
                          double & curx, double & cury,int from, int to);
  virtual void drawPicForLTL(QPainter * painter,double & curx, double & cury,int from, int to);
  virtual void flushBottomForRTT(double & curx,double & cury,bool & firstcolumn);
  virtual void flushBottomForLTL(double & curx,double & cury,bool & firstcolumn);
  virtual void flushLeftForTRT(double & curx,double & cury,bool & firstcolumn);
  virtual void flushRightForTLT(double & curx,double & cury,bool & firstcolumn);
  virtual bool hitTestForTRT(const QPointF & p,double & curx, double & cury,int from, int to);
  virtual bool hitTestForTLT(const QPointF & p,double & curx, double & cury,int from, int to);
  virtual bool hitTestForRTT(const QPointF & p,double & curx, double & cury,int from, int to);
  virtual bool hitTestForLTL(const QPointF & p,double & curx, double & cury,int from, int to);
  virtual bool isSelected(int i);
  virtual void moveToForTRT(double & curx, double & cury,int from, int to, int pos);
  virtual void moveToForTLT(double & curx, double & cury,int from, int to, int pos);
  virtual void moveToForRTT(double & curx, double & cury,int from, int to, int pos);
  virtual void moveToForLTL(double & curx, double & cury,int from, int to, int pos);
          void setIndent(double i) {indent=i;}
  virtual void setSelectedForTRT(const QRectF & rect, double & curx, double & cury,
                                 int from, int to);
  virtual void setSelectedForTLT(const QRectF & rect, double & curx, double & cury,
                                 int from, int to);
  virtual void setSelectedForRTT(const QRectF & rect, double & curx, double & cury,
                                 int from, int to);
  virtual void setSelectedForLTL(const QRectF & rect, double & curx, double & cury,
                                 int from, int to);

public:
  QString text;
};

class XW_TEXDOCUMENT_EXPORT XWTeXDocumentText : public XWTeXDocumentBlock
{
  Q_OBJECT

public:
  XWTeXDocumentText(XWTeXDocument * docA, QObject * parent = 0);

  bool find();
  bool find(int & s,int & e, double & cur,
            double & mi, double & ma,
            QString & content);
  bool findNext(int s, int e);
  bool findReplaced(int & s,int & e, double & cur,
                            double & mi, double & ma,
                            QString & content);

  void insert(XWTeXDocumentObject * obj);

  bool replaceNext(int s, int e);

  void scan(const QString & str, int & len, int & pos);

  void write(QTextStream & strm, int & linelen);

protected:
  bool isSelected(int i);
};

class XW_TEXDOCUMENT_EXPORT XWTeXDocumentComment : public XWTeXDocumentBlock
{
  Q_OBJECT

public:
  XWTeXDocumentComment(XWTeXDocument * docA, QObject * parent = 0);

  QString getMimeData();

  void scan(const QString & str, int & len, int & pos);
  void setFont();

  void write(QTextStream & strm, int & linelen);
};

class XW_TEXDOCUMENT_EXPORT XWTeXDocumentFormular : public XWTeXDocumentBlock
{
  Q_OBJECT

public:
  XWTeXDocumentFormular(XWTeXDocument * docA, QObject * parent = 0);

  QString getMimeData();

  void scan(const QString & str, int & len, int & pos);
  void setFont();

  void write(QTextStream & strm, int & linelen);
};

class XW_TEXDOCUMENT_EXPORT XWTeXDocumentDisplayFormular : public XWTeXDocumentBlock
{
  Q_OBJECT

public:
  XWTeXDocumentDisplayFormular(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  QString getMimeData();

  void scan(const QString & str, int & len, int & pos);
  void setFont();

  void write(QTextStream & strm, int & linelen);
};

class XW_TEXDOCUMENT_EXPORT XWTeXControlSequence : public XWTeXDocumentBlock
{
  Q_OBJECT

public:
  XWTeXControlSequence(XWTeXDocument * docA, QObject * parent = 0);
  XWTeXControlSequence(int tA,XWTeXDocument * docA, const QString & str, QObject * parent = 0);
  XWTeXControlSequence(XWTeXDocument * docA, bool newrow, const QString & str, QObject * parent = 0);

  bool isEmpty() {return true;}

  void scan(const QString & /*str*/, int & /*len*/,int & /*pos*/) {}
  void setFont();
};


class XW_TEXDOCUMENT_EXPORT XWTeXCode : public XWTeXDocumentObject
{
  Q_OBJECT

public:
  XWTeXCode(int tA,XWTeXDocument * docA, const QString & nameA,QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  QString getMimeData();

  bool hasSelected();

  bool isAllSelected();
  virtual bool isAtEnd();
  virtual bool isAtStart();
  bool isEmpty();

  void scanParam(const QString & str, int & len, int & pos);
  void setFont();

public:
  XWTeXDocumentBlock * name;
};


class XW_TEXDOCUMENT_EXPORT XWTeXAbstractParam : public XWTeXDocumentObject
{
  Q_OBJECT

public:
  XWTeXAbstractParam(int tA,
                           XWTeXDocument * docA,
                           const QString & nameA,
                           const QString & hA,
                           const QString & eA,
                           QObject * parent = 0);

  virtual void breakPage(double & curx,double & cury,bool & firstcolumn);
  virtual void breakPage(const QFont & fontA,  const QColor & c,
                         double & curx, double & cury,bool & firstcolumn);

  virtual void flushBottom(double & curx,double & cury,bool & firstcolumn);

  virtual QString getMimeData();

  bool hasSelected();

  bool isAllSelected();
  virtual bool isAtEnd();
  bool isEmpty();

  void setCursor();
  void setCursorAtEnd();
  void setCursorAtStart();
  virtual void setFont();

  virtual void write(QTextStream & strm, int & linelen);

public:
  XWTeXDocumentBlock * name;
};

class XW_TEXDOCUMENT_EXPORT XWTeXCoord : public XWTeXAbstractParam
{
  Q_OBJECT

public:
  XWTeXCoord(int tA, XWTeXDocument * docA, const QString & nameA, QObject * parent = 0);

  void scan(const QString & str, int & len, int & pos);
  void setFont();
};

class XW_TEXDOCUMENT_EXPORT XWTeXDocumentOption : public XWTeXAbstractParam
{
  Q_OBJECT

public:
  XWTeXDocumentOption(int tA, XWTeXDocument * docA, const QString & nameA, QObject * parent = 0);
  void scan(const QString & str, int & len, int & pos);
};

class XW_TEXDOCUMENT_EXPORT XWTeXDocumentParam : public XWTeXAbstractParam
{
  Q_OBJECT

public:
  XWTeXDocumentParam(int tA, XWTeXDocument * docA, const QString & nameA, QObject * parent = 0);

  void scan(const QString & str, int & len, int & pos);
  void setFont();

  void write(QTextStream & strm, int & linelen);
};

class XW_TEXDOCUMENT_EXPORT XWTeXOptionParam : public XWTeXAbstractParam
{
  Q_OBJECT

public:
  XWTeXOptionParam(XWTeXDocument * docA, const QString & nameA, QObject * parent = 0);

  void scan(const QString & str, int & len, int & pos);
};

class XW_TEXDOCUMENT_EXPORT XWTeXDocumentBeamerSpec : public XWTeXAbstractParam
{
  Q_OBJECT

public:
  XWTeXDocumentBeamerSpec(XWTeXDocument * docA,const QString & nameA, QObject * parent = 0);

  void scan(const QString & str, int & len, int & pos);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerDefaultSpec : public XWTeXAbstractParam
{
  Q_OBJECT

public:
  XWBeamerDefaultSpec(XWTeXDocument * docA,const QString & nameA, QObject * parent = 0);

  void scan(const QString & str, int & len, int & pos);
};

class XW_TEXDOCUMENT_EXPORT XWTeXDocumentCommand : public XWTeXDocumentObject
{
  Q_OBJECT

public:
  XWTeXDocumentCommand(int tA, XWTeXDocument * docA,
                       bool newrow,
                       const QString & headA,
                       QObject * parent = 0);
  virtual ~XWTeXDocumentCommand();

  virtual bool hasSelected();

  virtual bool isAllSelected();
  virtual bool isAtEnd();
  virtual bool isAtStart();
  virtual bool isEmpty();

  virtual void setCursor();
  virtual void setCursorAtEnd();
  virtual void setCursorAtStart();
};

class XW_TEXDOCUMENT_EXPORT XWTeXOptionCommand : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWTeXOptionCommand(int tA, XWTeXDocument * docA,
                       bool newrow,
                       const QString & hA,
                       const QString & oA,
                       QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWTeXParamCommand : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWTeXParamCommand(int tA, XWTeXDocument * docA,
                       bool newrow,
                       const QString & hA,
                       const QString & pA,
                       QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerSpecCommand : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWBeamerSpecCommand(int tA, XWTeXDocument * docA,
                       bool newrow,
                       const QString & hA,
                       QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWTeXOptionParamCommand : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWTeXOptionParamCommand(int tA, XWTeXDocument * docA,
                       const QString & hA,
                       const QString & oA,
                       const QString & pA,
                       QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerSpecParamCommand : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWBeamerSpecParamCommand(int tA, XWTeXDocument * docA,
                       const QString & hA,
                       const QString & pA,
                       QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWBeamerSpecOptionCommand : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWBeamerSpecOptionCommand(int tA, XWTeXDocument * docA,
                       const QString & hA,
                       QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWTeXDocumentUnkownEnviroment : public XWTeXDocumentObject
{
  Q_OBJECT

public:
  XWTeXDocumentUnkownEnviroment(XWTeXDocument * docA,
                       const QString & nameA,
                       QObject * parent = 0);
  virtual ~XWTeXDocumentUnkownEnviroment();

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  void scan(const QString & str, int & len, int & pos);

public:
  QString name;
};

#endif //XWTEXDOCUMENTOBJECT_H
