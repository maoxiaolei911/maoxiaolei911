/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWLATEXDOCUMENTOBJECT_H
#define XWLATEXDOCUMENTOBJECT_H

#include "XWTeXDocumentObject.h"

#define XW_LTX_CODE   -30
#define XW_LTX_OPTION -31
#define XW_LTX_PARAM  -32
#define XW_LTX_NAME   -33
#define XW_LTX_COORD  -34
#define XW_LTX_STAR   -35
#define XW_LTX_ROW    -36

#define XW_LTX_ROWS   69004

XW_TEXDOCUMENT_EXPORT XWTeXDocumentObject * createLaTeXObject(int t, XWTeXDocument * docA, QObject * parent);

class XW_TEXDOCUMENT_EXPORT XWLTXRow : public XWTeXDocumentBlock
{
  Q_OBJECT

public:
  XWLTXRow(XWTeXDocument * docA, QObject * parent = 0);

  QString getMimeData();

  void scan(const QString & str, int & len, int & pos);
};

class XW_TEXDOCUMENT_EXPORT XWLTXCode : public XWTeXCode
{
  Q_OBJECT

public:
  XWLTXCode(XWTeXDocument * docA, const QString & nameA,QObject * parent = 0);

  void scan(const QString & str, int & len, int & pos);
};

class XW_TEXDOCUMENT_EXPORT XWLTXStar : public XWTeXDocumentBlock
{
  Q_OBJECT

public:
  XWLTXStar(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & /*curx*/,double & /*cury*/,bool & /*firstcolumn*/) {}

  void clear();

  bool isEmpty();

  void scan(const QString & str, int & len, int & pos);

  void star();
};

class XW_TEXDOCUMENT_EXPORT XWLTXOption : public XWTeXDocumentObject
{
  Q_OBJECT

public:
  XWLTXOption(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(const QFont & fontA,const QColor & c,
                double & curx,double & cury,
                bool & firstcolumn);

  void flushBottom(const QFont & fontA,const QColor & c,
                    double & curx,double & cury,bool & firstcolumn);

  QString getMimeData();

  bool hasSelected();
  double height();

  bool isAllSelected();
  bool isEmpty();

  void scan(const QString & str, int & len, int & pos);

  double width();
  void write(QTextStream & strm, int & linelen);

public:
  XWTeXDocumentBlock * name;
};

class XW_TEXDOCUMENT_EXPORT XWLTXParam : public XWTeXDocumentObject
{
  Q_OBJECT

public:
  XWLTXParam(XWTeXDocument * docA, QObject * parent = 0);

  void append(XWTeXDocumentObject * obj);
  void append(XWTeXDocumentObject * sobj,XWTeXDocumentObject * eobj);

  void breakPage(const QFont & fontA,const QColor & c,
                double & curx,double & cury,
                bool & firstcolumn);
  void centered(const QFont & fontA,const QColor & c,
                double & curx,double & cury,
                bool & firstcolumn);

  void flushBottom(const QFont & fontA,const QColor & c,
                    double & curx,double & cury,bool & firstcolumn);

  bool hasSelected();
  double height();

  void insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj);
  bool isAllSelected();
  bool isEmpty();

  void removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj);
  void removeChildren(XWTeXDocumentObject**sobj, 
                                           XWTeXDocumentObject**eobj,
                                           XWTeXDocumentObject**obj);
  void removeChildrenFrom(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj,XWTeXDocumentObject**obj);
  void removeChildrenTo(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj);

  void scan(const QString & str, int & len, int & pos);

  double width();

public:
  XWTeXDocumentBlock * name;
};

class XW_TEXDOCUMENT_EXPORT XWLTXEnviroment : public XWTeXDocumentObject
{
  Q_OBJECT

public:
  XWLTXEnviroment(int tA, XWTeXDocument * docA,
                       const QString & headA,
                       QObject * parent = 0);

  virtual void append(XWTeXDocumentObject * obj);
  virtual void append(XWTeXDocumentObject * sobj,XWTeXDocumentObject * eobj);

  virtual void breakPage(double & curx,double & cury,bool & firstcolumn);

  virtual void clear();

  virtual bool hasSelected();

  virtual void insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj);
  virtual bool isAllSelected();
  virtual bool isAtEnd();
  virtual bool isAtStart();
  virtual bool isEmpty();

  virtual void removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj);  
  virtual void removeChildrenFrom(XWTeXDocumentObject**sobj, 
                                   XWTeXDocumentObject**eobj,
                                   XWTeXDocumentObject**obj);
  virtual void removeChildrenTo(XWTeXDocumentObject**sobj, 
                                       XWTeXDocumentObject**eobj,
                                       XWTeXDocumentObject**obj);

  virtual void setCursor();
  virtual void setCursorAtEnd();
  virtual void setCursorAtStart();
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentClass : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentClass(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentUsePackage : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentUsePackage(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentDefine: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentDefine(int tA, XWTeXDocument * docA, const QString & nameA, QObject * parent = 0);

  QString getMimeData();

  bool hasSelected();

  void scan(const QString & str, int & len, int & pos);
  void setCursor();

  void write(QTextStream & strm, int & linelen);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentNewCommand: public XWLTXDocumentDefine
{
  Q_OBJECT

public:
  XWLTXDocumentNewCommand(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentRenewCommand: public XWLTXDocumentDefine
{
  Q_OBJECT

public:
  XWLTXDocumentRenewCommand(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentDefineEnv: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentDefineEnv(int tA, XWTeXDocument * docA, const QString & nameA, QObject * parent = 0);

  QString getMimeData();

  bool hasSelected();

  bool isAllSelected();
  bool isEmpty();

  void setCursor();

  void write(QTextStream & strm, int & linelen);
};


class XW_TEXDOCUMENT_EXPORT XWLTXDocumentNewEnvironment: public XWLTXDocumentDefineEnv
{
  Q_OBJECT

public:
  XWLTXDocumentNewEnvironment(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentRenewEnvironment: public XWLTXDocumentDefineEnv
{
  Q_OBJECT

public:
  XWLTXDocumentRenewEnvironment(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentNewTheorem: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentNewTheorem(XWTeXDocument * docA, QObject * parent = 0);

  QString getMimeData();

  bool hasSelected();

  bool isAllSelected();
  bool isEmpty();

  void setCursor();
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentNewFont: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentNewFont(XWTeXDocument * docA, QObject * parent = 0);

  void setCursor();
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentNewCounter: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentNewCounter(XWTeXDocument * docA, QObject * parent = 0);

  void setCursor();
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentNewLength: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentNewLength(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentIncludeOnly: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentIncludeOnly(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentEnviroment: public XWLTXEnviroment
{
  Q_OBJECT

public:
  XWLTXDocumentEnviroment(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  void scan(const QString & str, int & len, int & pos);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentCJK: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentCJK(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  void clear();

  void insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj);
  
  void removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj);
  void removeChildrenTo(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj,XWTeXDocumentObject**obj);
  void scan(const QString & str, int & len, int & pos);

public:
  bool topCJK;
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentCJKStar: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentCJKStar(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  void clear();

  void insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj);
  
  void removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj);
  void removeChildrenTo(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj,XWTeXDocumentObject**obj);

  void scan(const QString & str, int & len, int & pos);

public:
  bool topCJK;
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentInclude: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentInclude(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentInput: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentInput(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentTitle: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentTitle(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentAuthor: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentAuthor(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  QString getMimeData();

  void scan(const QString & str, int & len, int & pos);

  void write(QTextStream & strm, int & linelen);

public:
  XWTeXControlSequence * andObj;
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentDate: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentDate(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentThanks: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentThanks(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentTitlePage: public XWLTXEnviroment
{
  Q_OBJECT

public:
  XWLTXDocumentTitlePage(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  bool isEmpty();

  void scan(const QString & str, int & len, int & pos);
  void setCursor();
};

class XW_TEXDOCUMENT_EXPORT XWLTXTableOfContents: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXTableOfContents(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);
};

class XW_TEXDOCUMENT_EXPORT XWLTXAppendix: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXAppendix(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);
};

class XW_TEXDOCUMENT_EXPORT XWLTXParEnv: public XWLTXEnviroment
{
  Q_OBJECT

public:
  XWLTXParEnv(int tA, XWTeXDocument * docA, const QString & nameA, QObject * parent = 0);

  virtual void clear();

  void insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj);
  virtual bool isAtStart();
  
  void removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj);
  void removeChildrenTo(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj,XWTeXDocumentObject**obj);

  virtual void scan(const QString & str, int & len, int & pos);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentAbstract: public XWLTXParEnv
{
  Q_OBJECT

public:
  XWLTXDocumentAbstract(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  bool isAtStart();

public:
  XWTeXDocumentText * name;
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentSectioning: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentSectioning(int tA, XWTeXDocument * docA, const QString & nameA,QObject * parent = 0);

  void addStar();
  void append(XWTeXDocumentObject * obj);
  void append(XWTeXDocumentObject * sobj,XWTeXDocumentObject * eobj);

  void clear();

  bool hasStar();

  void insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj);
  void insertChildren(XWTeXDocumentObject*obj, XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj);
  bool isAllSelected();
  bool isAtEnd();
  bool isAtStart();
  bool isEmpty();

  void removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj);
  void removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj,XWTeXDocumentObject**obj);
  void removeChildrenFrom(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj,XWTeXDocumentObject**obj);
  void removeChildrenTo(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj);
  void removeStar();

  void write(QTextStream & strm, int & linelen);

public:
  XWTeXDocumentText * countText;
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentPart: public XWLTXDocumentSectioning
{
  Q_OBJECT

public:
  XWLTXDocumentPart(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentChapter: public XWLTXDocumentSectioning
{
  Q_OBJECT

public:
  XWLTXDocumentChapter(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentSection: public XWLTXDocumentSectioning
{
  Q_OBJECT

public:
  XWLTXDocumentSection(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentSubsection: public XWLTXDocumentSectioning
{
  Q_OBJECT

public:
  XWLTXDocumentSubsection(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentSubsubsection: public XWLTXDocumentSectioning
{
  Q_OBJECT

public:
  XWLTXDocumentSubsubsection(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentParagraph: public XWLTXDocumentSectioning
{
  Q_OBJECT

public:
  XWLTXDocumentParagraph(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentSubparagraph: public XWLTXDocumentSectioning
{
  Q_OBJECT

public:
  XWLTXDocumentSubparagraph(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentPar: public XWTeXDocumentObject
{
  Q_OBJECT

public:
  XWLTXDocumentPar(XWTeXDocument * docA, QObject * parent = 0);

  QString getMimeData();

  bool isEmpty();

  void scan(const QString & str, int & len, int & pos);

  void write(QTextStream & strm, int & linelen);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentQuote: public XWLTXParEnv
{
  Q_OBJECT

public:
  XWLTXDocumentQuote(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentQuotation: public XWLTXParEnv
{
  Q_OBJECT

public:
  XWLTXDocumentQuotation(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentVerse: public XWLTXEnviroment
{
  Q_OBJECT

public:
  XWLTXDocumentVerse(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  void scan(const QString & str, int & len, int & pos);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentItem: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentItem(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  void clear();

  void insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj);
  bool isAllSelected();
  bool isAtEnd();
  bool isAtStart();
  bool isEmpty();

  void removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj);
  void removeChildrenTo(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj);

  void scan(const QString & str, int & len, int & pos);
  void setCursorAtStart();

public:
  XWTeXDocumentBlock * mark;
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentItemEnv: public XWLTXEnviroment
{
  Q_OBJECT

public:
  XWLTXDocumentItemEnv(int tA, XWTeXDocument * docA, const QString & nameA,QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  void clear();

  void insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj);
  bool isAllSelected();
  bool isAtEnd();
  bool isAtStart();
  bool isEmpty();
  
  void removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj);
  void removeChildrenTo(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj,XWTeXDocumentObject**obj);

  void scan(const QString & str, int & len, int & pos);
  void setCursorAtStart();
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentDescription: public XWLTXDocumentItemEnv
{
  Q_OBJECT

public:
  XWLTXDocumentDescription(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentEnumerate: public XWLTXDocumentItemEnv
{
  Q_OBJECT

public:
  XWLTXDocumentEnumerate(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentItemize: public XWLTXDocumentItemEnv
{
  Q_OBJECT

public:
  XWLTXDocumentItemize(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentArray: public XWLTXEnviroment
{
  Q_OBJECT

public:
  XWLTXDocumentArray(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  void clear();

  void scan(const QString & str, int & len, int & pos);

  void setFont();
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentDisplayMath: public XWLTXEnviroment
{
  Q_OBJECT

public:
  XWLTXDocumentDisplayMath(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  void setFont();
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentEqnarray: public XWLTXEnviroment
{
  Q_OBJECT

public:
  XWLTXDocumentEqnarray(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  void scan(const QString & str, int & len, int & pos);

  void setFont();
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentEqnarrayStar: public XWLTXEnviroment
{
  Q_OBJECT

public:
  XWLTXDocumentEqnarrayStar(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  void scan(const QString & str, int & len, int & pos);
  void setFont();
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentEquation: public XWLTXEnviroment
{
  Q_OBJECT

public:
  XWLTXDocumentEquation(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  void setFont();
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentFloat: public XWLTXEnviroment
{
  Q_OBJECT

public:
  XWLTXDocumentFloat(int tA, XWTeXDocument * docA, const QString & nameA,QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  void clear();

  virtual void scan(const QString & str, int & len, int & pos);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentFigure: public XWLTXDocumentFloat
{
  Q_OBJECT

public:
  XWLTXDocumentFigure(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentFigureStar: public XWLTXDocumentFloat
{
  Q_OBJECT

public:
  XWLTXDocumentFigureStar(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentAlign: public XWLTXEnviroment
{
  Q_OBJECT

public:
  XWLTXDocumentAlign(int tA, XWTeXDocument * docA, const QString & nameA,QObject * parent = 0);

  void scan(const QString & str, int & len, int & pos);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentCenter: public XWLTXDocumentAlign
{
  Q_OBJECT

public:
  XWLTXDocumentCenter(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentFlushLeft: public XWLTXDocumentAlign
{
  Q_OBJECT

public:
  XWLTXDocumentFlushLeft(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentFlushRight: public XWLTXDocumentAlign
{
  Q_OBJECT

public:
  XWLTXDocumentFlushRight(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentList: public XWLTXEnviroment
{
  Q_OBJECT

public:
  XWLTXDocumentList(XWTeXDocument * docA, QObject * parent = 0);

  void scan(const QString & str, int & len, int & pos);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentMath: public XWLTXEnviroment
{
  Q_OBJECT

public:
  XWLTXDocumentMath(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentMiniPage: public XWLTXEnviroment
{
  Q_OBJECT

public:
  XWLTXDocumentMiniPage(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentPicture: public XWLTXEnviroment
{
  Q_OBJECT

public:
  XWLTXDocumentPicture(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentTabbing: public XWLTXDocumentAlign
{
  Q_OBJECT

public:
  XWLTXDocumentTabbing(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentTable: public XWLTXDocumentFloat
{
  Q_OBJECT

public:
  XWLTXDocumentTable(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentTabular: public XWLTXEnviroment
{
  Q_OBJECT

public:
  XWLTXDocumentTabular(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  void clear();

  void insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj);

  void removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj);
  void removeChildrenTo(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj);

  void scan(const QString & str, int & len, int & pos);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentTabularStar: public XWLTXEnviroment
{
  Q_OBJECT

public:
  XWLTXDocumentTabularStar(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  void clear();

  void insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj);

  void removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj);
  void removeChildrenTo(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj);

  void scan(const QString & str, int & len, int & pos);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentBibitem: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentBibitem(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  void clear();

  void insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj);
  void insertChildren(XWTeXDocumentObject*obj, XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj);

  bool isAtEnd();
  bool isAtStart();

  void removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj);
  void removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj,XWTeXDocumentObject**obj);
  void removeChildrenFrom(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj,XWTeXDocumentObject**obj);
  void removeChildrenTo(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj);

  void scan(const QString & str, int & len, int & pos);

public:
  XWTeXDocumentBlock * mark;
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentThebibliography: public XWLTXEnviroment
{
  Q_OBJECT

public:
  XWLTXDocumentThebibliography(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  void clear();

  void scan(const QString & str, int & len, int & pos);

public:
  XWTeXDocumentBlock * title;
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentTheoremEnv: public XWLTXEnviroment
{
  Q_OBJECT

public:
  XWLTXDocumentTheoremEnv(int tA, XWTeXDocument * docA,const QString & nameA, QObject * parent = 0);

  virtual void breakPage(double & curx,double & cury,bool & firstcolumn);

  virtual void clear();

  void insertChildren(XWTeXDocumentObject*sobj, XWTeXDocumentObject*eobj);
  virtual bool isAtStart();

  void removeChildren(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj);
  void removeChildrenTo(XWTeXDocumentObject**sobj, XWTeXDocumentObject**eobj);

  virtual void scan(const QString & str, int & len, int & pos);

public:
  XWTeXDocumentBlock * title;
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentTheorem: public XWLTXDocumentTheoremEnv
{
  Q_OBJECT

public:
  XWLTXDocumentTheorem(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentProof: public XWLTXDocumentTheoremEnv
{
  Q_OBJECT

public:
  XWLTXDocumentProof(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentAssumption: public XWLTXDocumentTheoremEnv
{
  Q_OBJECT

public:
  XWLTXDocumentAssumption(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentDefinition: public XWLTXDocumentTheoremEnv
{
  Q_OBJECT

public:
  XWLTXDocumentDefinition(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentProposition: public XWLTXDocumentTheoremEnv
{
  Q_OBJECT

public:
  XWLTXDocumentProposition(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentLemma: public XWLTXDocumentTheoremEnv
{
  Q_OBJECT

public:
  XWLTXDocumentLemma(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentAxiom: public XWLTXDocumentTheoremEnv
{
  Q_OBJECT

public:
  XWLTXDocumentAxiom(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentCorollary: public XWLTXDocumentTheoremEnv
{
  Q_OBJECT

public:
  XWLTXDocumentCorollary(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentConjecture: public XWLTXDocumentTheoremEnv
{
  Q_OBJECT

public:
  XWLTXDocumentConjecture(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentExercise: public XWLTXDocumentTheoremEnv
{
  Q_OBJECT

public:
  XWLTXDocumentExercise(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentExample: public XWLTXDocumentTheoremEnv
{
  Q_OBJECT

public:
  XWLTXDocumentExample(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentRemark: public XWLTXDocumentTheoremEnv
{
  Q_OBJECT

public:
  XWLTXDocumentRemark(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentProblem: public XWLTXDocumentTheoremEnv
{
  Q_OBJECT

public:
  XWLTXDocumentProblem(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentVerbatim: public XWLTXEnviroment
{
  Q_OBJECT

public:
  XWLTXDocumentVerbatim(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentVerb: public XWTeXDocumentBlock
{
  Q_OBJECT

public:
  XWLTXDocumentVerb(XWTeXDocument * docA, QObject * parent = 0);

  void scan(const QString & str, int & len, int & pos);
  void setFont();
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentFootnote: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentFootnote(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentMarginpar: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentMarginpar(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentCaption: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentCaption(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentLabel: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentLabel(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentPageref: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentPageref(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentRef: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentRef(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentIndex: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentIndex(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentGlossary: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentGlossary(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentCite: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentCite(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentNocite: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentNocite(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentBibliographystyle: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentBibliographystyle(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentBibliography: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentBibliography(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentRows: public XWTeXDocumentObject
{
  Q_OBJECT

public:
  XWLTXDocumentRows(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  QString getMimeData();

  bool isEmpty();

  void scan(const QString & str, int & len, int & pos);

  void write(QTextStream & strm, int & linelen);

public:
   XWTeXControlSequence * lineEndObj;
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentAddress: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentAddress(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentLocation: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentLocation(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentTelephone: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentTelephone(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentSignature: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentSignature(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentOpening: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentOpening(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  void scan(const QString & str, int & len, int & pos);

  void write(QTextStream & strm, int & linelen);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentName: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentName(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentMakelabels: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentMakelabels(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentEncl: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentEncl(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

public:
  XWTeXDocumentBlock * title;
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentClosing: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentClosing(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  void scan(const QString & str, int & len, int & pos);

  void write(QTextStream & strm, int & linelen);
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentCc: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentCc(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

public:
  XWTeXDocumentBlock * title;
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentLetter: public XWLTXEnviroment
{
  Q_OBJECT

public:
  XWLTXDocumentLetter(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  void scan(const QString & str, int & len, int & pos);
  void setCursor();

public:
  XWTeXDocumentObject * closing;
  XWTeXDocumentObject * ps;
  XWTeXDocumentObject * cc;
  XWTeXDocumentObject * encl;
};

class XW_TEXDOCUMENT_EXPORT XWLTXDocumentIncludegraphics: public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWLTXDocumentIncludegraphics(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);
};

#endif //XWLATEXDOCUMENTOBJECT_H
