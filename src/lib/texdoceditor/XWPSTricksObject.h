/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWPSTRICKSOBJECT_H
#define XWPSTRICKSOBJECT_H

#include "XWTeXDocumentObject.h"

#define XW_PSTRICKS_CODE   -20

XW_TEXDOCUMENT_EXPORT XWTeXDocumentObject * createPSTricksObject(int t, XWTeXDocument * docA, QObject * parent);

class XW_TEXDOCUMENT_EXPORT XWPSTricksBlock : public XWTeXDocumentBlock
{
  Q_OBJECT

public:
  XWPSTricksBlock(int tA, XWTeXDocument * docA, QObject * parent = 0);
  XWPSTricksBlock(int tA, XWTeXDocument * docA,
                     bool newrow,
                     const QString & str,
                     QObject * parent = 0);

  virtual void scanEnvironment(int endid, const QString & str, int & len, int & pos);
};


class XW_TEXDOCUMENT_EXPORT XWPSTricksCode : public XWTeXDocumentObject
{
  Q_OBJECT

public:
  XWPSTricksCode(XWTeXDocument * docA, const QString & nameA,QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  QString getMimeData();

  bool hasSelected();

  bool isAllSelected();
  bool isAtEnd();
  bool isAtStart();
  bool isEmpty();
  bool isFirst(XWTeXDocumentObject * obj) {return obj==last;}

  void scanEnvironment(int endid, const QString & str, int & len, int & pos);
  void scanParam(const QString & str, int & len, int & pos);
  void setCursorAtStart();

public:
  XWPSTricksBlock * name;
};


class XW_TEXDOCUMENT_EXPORT XWPSTricksOff : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWPSTricksOff(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWPSTricksAltcolormode : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWPSTricksAltcolormode(XWTeXDocument * docA, QObject * parent = 0);
};

class XW_TEXDOCUMENT_EXPORT XWPSTricksPspicture : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWPSTricksPspicture(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  bool hasSelected();

  bool isAllSelected();
  bool isEmpty();

  void scan(const QString & str, int & len, int & pos);
  void setCursor();
};

class XW_TEXDOCUMENT_EXPORT XWPSTricksPsmatrix : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWPSTricksPsmatrix(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  bool hasSelected();

  bool isAllSelected();
  bool isEmpty();
  void setCursor();

  void scan(const QString & str, int & len, int & pos);
};

class XW_TEXDOCUMENT_EXPORT XWpsTree : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWpsTree(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  QString getMimeData();

  bool hasSelected();

  bool isAllSelected();
  bool isEmpty();

  void scan(const QString & str, int & len, int & pos);
  void setCursor();

  void write(QTextStream & strm, int & linelen);
};


class XW_TEXDOCUMENT_EXPORT XWpstree : public XWTeXDocumentCommand
{
  Q_OBJECT

public:
  XWpstree(XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  QString getMimeData();

  bool hasSelected();

  bool isAllSelected();
  bool isEmpty();

  void scan(const QString & str, int & len, int & pos);
  void setCursor();

  void write(QTextStream & strm, int & linelen);
};


#endif //XWPSTRICKSOBJECT_H
