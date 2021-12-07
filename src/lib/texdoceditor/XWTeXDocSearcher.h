/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTEXDOCSEARCHER_H
#define XWTEXDOCSEARCHER_H

#include <QListWidgetItem>
#include <QListWidget>
#include <QRectF>
#include "XWSearcher.h"
#include "XWTeXDocumentType.h"

class QLabel;
class QLineEdit;
class QMenu;
class QAction;
class XWTeXDocumentBlock;
class XWTeXDocument;
class XWTeXDocSearcher;

class XWTeXDocSearchListItem : public QListWidgetItem
{
public:
  XWTeXDocSearchListItem(QListWidget * parent = 0, int type = Type);

  XWTeXDocumentBlock * getBlock() {return block;}
  int    getEnd() {return end;}
  int    getPage() {return pg;}
  void   getRect(double & minxA, double & minyA, double & maxxA, double & maxyA);
  int    getStart() {return start;}

  void setBlock(XWTeXDocumentBlock * b) {block=b;}
  void setEnd(int e) {end=e;}
  void setPage(int pgA) {pg=pgA;}
  void setRect(double minxA, double minyA, double maxxA, double maxyA);
  void setStart(int s) {start=s;}

private:
  int start,end,pg;
  double minx, miny, maxx, maxy;
  XWTeXDocumentBlock * block;
};

class XW_TEXDOCUMENT_EXPORT XWTeXDocSearhList : public QListWidget
{
  Q_OBJECT

public:
  XWTeXDocSearhList(QWidget * parent = 0);

  void createItem(XWTeXDocumentBlock * block,
                  int start, int end, int pg, 
                  double minx, double miny, 
                  double maxx, double maxy,
                  const QString & content);

  void setDoc(XWTeXDocument * docA) {doc=docA;}

signals:
  void positionActivated(int pg, double minx, double miny, 
                  double maxx, double maxy);

private slots:
  void activePosition(QListWidgetItem * item);

private:
  XWTeXDocument * doc;
};

class XW_TEXDOCUMENT_EXPORT XWTeXDocSearchWidget : public QWidget
{
  Q_OBJECT

public:
  XWTeXDocSearchWidget(XWTeXDocSearcher * searcherA,QWidget * parent = 0);

signals:
  void positionActivated(int pg, double minx, double miny, 
                  double maxx, double maxy);

private slots:
  void search();
  void setCaseSensitivity(bool e);
  void setWholeWord(bool e);

private:
  XWTeXDocSearcher * searcher;
  QLabel     * textLabel;
  QLineEdit  * textEdit;
  QMenu * startMenu;
  XWTeXDocSearhList * list;
};

class XW_TEXDOCUMENT_EXPORT XWTeXDocReplaceWidget : public QWidget
{
  Q_OBJECT

public:
  XWTeXDocReplaceWidget(XWTeXDocSearcher * searcherA, QWidget * parent = 0);

signals:
  void positionActivated(int pg, double minx, double miny, 
                  double maxx, double maxy);

private slots:
  void replace();
  void setCaseSensitivity(bool e);
  void setWholeWord(bool e);

private:
  XWTeXDocSearcher * searcher;
  QLabel     * textLabel;
  QLineEdit  * textEdit;
  QLabel     * replacingLabel;
  QLineEdit  * replacingEdit;
  QMenu * startMenu;
  XWTeXDocSearhList * list;
};

class XW_TEXDOCUMENT_EXPORT XWTeXDocSearcher : public XWSearcher
{
  Q_OBJECT

public:
  XWTeXDocSearcher(QObject * parent = 0);

  XWTeXDocument * getDoc() {return doc;}

  void setDoc(XWTeXDocument * docA) {doc=docA;}
  void setList(XWTeXDocSearhList * l) {list = l;}

protected:
  void run();

private:
  XWTeXDocument * doc;
  XWTeXDocSearhList * list;
};

#endif //XWTEXDOCSEARCHER_H
