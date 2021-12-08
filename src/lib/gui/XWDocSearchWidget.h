/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWDOCSEARCHLIST_H
#define XWDOCSEARCHLIST_H

#include <QTreeWidget>
#include <QListWidgetItem>
#include <QListWidget>
#include "XWGlobal.h"

class QLabel;
class QLineEdit;
class QToolButton;
class QAction;
class QTreeWidgetItem;
class XWPDFSearcher;
class XWQTextDocumentSearcher;

class XW_GUI_EXPORT XWPDFSearchListItem : public QListWidgetItem
{
public:
  XWPDFSearchListItem(QListWidget * parent = 0, int type = Type);

  int  getPage() {return pg;}
  void getRect(double & minxA, double & minyA, double & maxxA, double & maxyA);

  void setPage(int pgA) {pg=pgA;}
  void setRect(double minxA, double minyA, double maxxA, double maxyA);

private:
  int pg;
  double minx, miny, maxx, maxy;
};

class XW_GUI_EXPORT XWPDFSearhList : public QListWidget
{
  Q_OBJECT

public:
  XWPDFSearhList(QWidget * parent = 0);

  void createItem(int pg, double minx, double miny, 
                  double maxx, double maxy,const QString & content);

signals:
  void positionActivated(int pg, double minxA, double minyA, double maxxA, double maxyA);

private slots:
  void activePosition(QListWidgetItem * item);
};

class XW_GUI_EXPORT XWTeXPDFSearchWidget : public QWidget
{
  Q_OBJECT

public:
  XWTeXPDFSearchWidget(XWPDFSearcher * searcherA,QWidget * parent = 0);

signals:
  void positionActivated(int pg, double minxA, double minyA, double maxxA, double maxyA);

private slots:
  void search();
  void setCaseSensitivity(bool e);
  void setWholeWord(bool e);

private:
  XWPDFSearcher * searcher;
  QLabel     * textLabel;
  QLineEdit  * textEdit;
  QToolButton * startButton;
  XWPDFSearhList * list;
};

class XW_GUI_EXPORT XWQTextDocumentSearchList : public QTreeWidget
{
  Q_OBJECT

public:
  XWQTextDocumentSearchList(XWQTextDocumentSearcher * searcherA,QWidget * parent = 0);

  void createFileItem(const QString & fileA);
  void createTextItem(int pos, const QString & content);

  void reset();

signals:
  void fileActivated(const QString & fileA);
  void positionActivated(int pos, int len);

private slots:	
  void activePosition(QTreeWidgetItem * item, int col);

protected:
  XWQTextDocumentSearcher * searcher;
  QTreeWidgetItem * curFileItem;
};

class XW_GUI_EXPORT XWQTextDocumentSearchWidget : public QWidget
{
   Q_OBJECT

public:
  XWQTextDocumentSearchWidget(XWQTextDocumentSearcher * searcherA, QWidget * parent = 0);

signals:
  void fileActivated(const QString & fileA);
  void positionActivated(int pos, int len);

private slots:
  void search();
  void setCaseSensitivity(bool e);
  void setSearchType(QAction * a);
  void setWholeWord(bool e);

private:
  XWQTextDocumentSearcher * searcher;
  QLabel     * textLabel;
  QLineEdit  * textEdit;
  QToolButton * startButton;
  XWQTextDocumentSearchList * list;
};

class XW_GUI_EXPORT XWQTextDocumentReplaceWidget : public QWidget
{
   Q_OBJECT

public:
  XWQTextDocumentReplaceWidget(XWQTextDocumentSearcher * searcherA, QWidget * parent = 0);

signals:
  void fileActivated(const QString & fileA);
  void positionActivated(int pos, int len);

private slots:
  void replace();
  void setCaseSensitivity(bool e);
  void setSearchType(QAction * a);
  void setWholeWord(bool e);

private:
  XWQTextDocumentSearcher * searcher;
  QLabel     * textLabel;
  QLineEdit  * textEdit;
  QLabel     * replacingLabel;
  QLineEdit  * replacingEdit;
  QToolButton * startButton;
  XWQTextDocumentSearchList * list;
};

#endif //XWDOCSEARCHLIST_H
