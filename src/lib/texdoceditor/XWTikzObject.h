/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTIKZOBJECT_H
#define XWTIKZOBJECT_H

#include "XWTeXDocumentObject.h"

#define XW_TIKZ_CODE   -10
#define XW_TIKZ_OPTION -11
#define XW_TIKZ_PARAM   -12
#define XW_TIKZ_NAME   -13

XW_TEXDOCUMENT_EXPORT XWTeXDocumentObject * createTikzObject(int t, XWTeXDocument * docA, QObject * parent);

class XW_TEXDOCUMENT_EXPORT XWTikzBlock : public XWTeXDocumentBlock
{
  Q_OBJECT

public:
  XWTikzBlock(int tA, XWTeXDocument * docA, QObject * parent = 0);
  XWTikzBlock(int tA, XWTeXDocument * docA,
                     bool newrow,
                     const QString & str,
                     QObject * parent = 0);

  virtual void scanEnvironment(int endid, const QString & str, int & len, int & pos);
  virtual void scanOption(const QString & str, int & len, int & pos);
  virtual void scanParam(const QString & str, int & len, int & pos);
  virtual void scanPath(const QString & str, int & len, int & pos);
};

class XW_TEXDOCUMENT_EXPORT XWTikzCode : public XWTeXDocumentObject
{
  Q_OBJECT

public:
  XWTikzCode(XWTeXDocument * docA, const QString & nameA,QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  QString getMimeData();

  bool hasSelected();

  bool isAtEnd();
  bool isAtStart();
  bool isEmpty();
  bool isFirst(XWTeXDocumentObject * obj) {return obj == last;}

  void scanEnvironment(int endid, const QString & str, int & len, int & pos);
  void scanParam(const QString & str, int & len, int & pos);
  void scanPath(const QString & str, int & len, int & pos);
  void setCursorAtStart();

public:
  XWTikzBlock * name;
};

class XW_TEXDOCUMENT_EXPORT XWTikzOption : public XWTeXDocumentObject
{
  Q_OBJECT

public:
  XWTikzOption(XWTeXDocument * docA, const QString & nameA,QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  QString getMimeData();

  bool hasSelected();

  bool isAtStart();
  bool isEmpty();
  bool isFirst(XWTeXDocumentObject * obj) {return obj == last;}

  void scan(const QString & str, int & len, int & pos);
  void setCursor();
  void setCursorAtEnd();
  void setCursorAtStart();

  void write(QTextStream & strm, int & linelen);

public:
  XWTikzBlock * name;
};

class XW_TEXDOCUMENT_EXPORT XWTikzParam : public XWTeXDocumentObject
{
  Q_OBJECT

public:
  XWTikzParam(XWTeXDocument * docA, const QString & nameA,QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  bool hasSelected();

  bool isAtStart();
  bool isEmpty();
  bool isFirst(XWTeXDocumentObject * obj) {return obj == last;}

  void scan(const QString & str, int & len, int & pos);
  void setCursorAtStart();

public:
  XWTikzBlock * name;
};

class XW_TEXDOCUMENT_EXPORT XWUsetikzlibrary : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWUsetikzlibrary(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWTikzdeclarecoordinatesystem : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWTikzdeclarecoordinatesystem(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

   QString getMimeData();

   void scan(const QString & str, int & len, int & pos);
   void setCursor();

   void write(QTextStream & strm, int & linelen);
};

class XW_TEXDOCUMENT_EXPORT XWTikzaliascoordinatesystem : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWTikzaliascoordinatesystem(XWTeXDocument * docA, QObject * parent = 0);

  void setCursor();
};

class XW_TEXDOCUMENT_EXPORT XWTikzfading : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWTikzfading(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWTikzfadingfrompicture : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWTikzfadingfrompicture(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  bool isAllSelected();
  bool isEmpty();

  void scan(const QString & str, int & len, int & pos);
  void setCursor();
};

class XW_TEXDOCUMENT_EXPORT XWTikzexternalize : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWTikzexternalize(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWTikzsetexternalprefix : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWTikzsetexternalprefix(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWTikzsetnextfilename : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWTikzsetnextfilename(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWTikzsetfigurename : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWTikzsetfigurename(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWTikzappendtofigurename : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWTikzappendtofigurename(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWTikzpicturedependsonfile : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWTikzpicturedependsonfile(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWTikzexternalfiledependsonfile : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWTikzexternalfiledependsonfile(XWTeXDocument * docA, QObject * parent = 0);

  void setCursor();
};

class XW_TEXDOCUMENT_EXPORT XWTikzset : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWTikzset(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);
};

class XW_TEXDOCUMENT_EXPORT XWTikzstyle : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWTikzstyle(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  QString getMimeData();

  bool isAllSelected();
  bool isEmpty();

  void scan(const QString & str, int & len, int & pos);
  void setCursor();

  void write(QTextStream & strm, int & linelen);
};

class XW_TEXDOCUMENT_EXPORT XWTikz : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWTikz(XWTeXDocument * docA, QObject * parent = 0);

  bool hasSelected();

  bool isEmpty();

  void scan(const QString & str, int & len, int & pos);

  void write(QTextStream & strm, int & linelen);
};

class XW_TEXDOCUMENT_EXPORT XWTikzpicture : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWTikzpicture(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  bool hasSelected();

  bool isAllSelected();
  bool isEmpty();

  void scan(const QString & str, int & len, int & pos);
  void setCursor();
};

#endif //XWTIKZOBJECT_H
