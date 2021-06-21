/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTEXOBJECT_H
#define XWTEXOBJECT_H

#include "XWTeXDocumentObject.h"

XW_TEXDOCUMENT_EXPORT XWTeXDocumentObject * createTexObject(int t, XWTeXDocument * docA, QObject * parent);

class XW_TEXDOCUMENT_EXPORT XWTeXObject : public XWTeXDocumentBlock
{
  Q_OBJECT

public:
  XWTeXObject(int tA, XWTeXDocument * docA, QObject * parent = 0);

  void breakPage(double & curx,double & cury,bool & firstcolumn);

  virtual void scan(const QString & str, int & len, int & pos);

  virtual void write(QTextStream & strm, int & linelen);

protected:
  void scanBox(const QString & str, int & len, int & pos);
  void scanDef(const QString & str, int & len, int & pos);
  void scanGlue(const QString & str, int & len, int & pos);
  void scanRule(const QString & str, int & len, int & pos);
};

#endif //XWTEXDOCUMENT_H
