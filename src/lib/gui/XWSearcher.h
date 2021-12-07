/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWSEARCHER_H
#define XWSEARCHER_H

#include <QString>
#include <QThread>
#include "XWGlobal.h"

#define XW_SEACHER_FILE    0
#define XW_SEACHER_CUR_DIR 1
#define XW_SEACHER_DIRS    2

class QTextDocument;
class XWDoc;
class XWPDFSearhList;
class XWQTextDocumentSearchList;

class XW_GUI_EXPORT XWSearcher : public QThread
{
  Q_OBJECT

public:
  XWSearcher(QObject * parent = 0);

  int getTextLength();

  bool isCaseSensitivity() {return caseSensitivity;}
  bool isRegexpFind() {return regexpFind;}
  bool isWholeWord() {return wholeWord;}

  void setCaseSensitivity(bool e) {caseSensitivity=e;}
  void setRegexpFind(bool e) {regexpFind=e;}
  void setReplace(bool e) {isReplace=e;}
  void setReplacing(const QString & txt) {replacing=txt;}
  void setSearchType(int t) {stype=t;}
  void setText(const QString & txt) {text=txt;}
  void setWholeWord(bool e) {wholeWord=e;}

signals:
  void textFinded(int pos, const QString & content);
  void textFinded(int pg, double minx, double miny, 
                  double maxx, double maxy, const QString & content);
  void fileFinded(const QString & fileA);

protected:
  int  stype;
  bool caseSensitivity;
  bool wholeWord;
  bool regexpFind;
  bool isReplace;
  QString text;
  QString replacing;
};

class XW_GUI_EXPORT XWPDFSearcher : public XWSearcher
{
  Q_OBJECT

public:
  XWPDFSearcher(QObject * parent = 0);

  void setDoc(XWDoc * docA) {doc=docA;}
  void setDPI(double dpiA) {dpi=dpiA;}
  void setList(XWPDFSearhList * listA) {list=listA;}
  void setRotate(int r) {rotate=r;}

protected:
  void run();

private:
  XWDoc * doc;
  double dpi;
  int    rotate;
  XWPDFSearhList * list;
};

class XW_GUI_EXPORT XWQTextDocumentSearcher : public XWSearcher
{
  Q_OBJECT

public:
  XWQTextDocumentSearcher(QObject * parent = 0);

  void setDoc(QTextDocument * docA) {doc=docA;}
  void setList(XWQTextDocumentSearchList * listA) {list=listA;}
  void setPath(const QString & pathA) {path=pathA;}
  void setRoot(const QString & r) {root=r;}

protected:
  void run();

private:
  void searchDocument();
  void searchPath();
  void searchRoot();

private:
  XWQTextDocumentSearchList * list;
  QTextDocument * doc;
  QString path;
  QString root;
};

#endif //XWSEARCHER_H
