/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include <QTextDocument>
#include "XWSearcher.h"
#include "XWDocSearchWidget.h"

#define XW_S_L_FILE      -1
#define XW_S_L_TEXT_POS  -2

XWPDFSearchListItem::XWPDFSearchListItem(QListWidget * parent, int type)
:QListWidgetItem(parent,type)
{}

void XWPDFSearchListItem::getRect(double & minxA, double & minyA, double & maxxA, double & maxyA)
{
  minxA = minx;
  minyA = miny;
  maxxA = maxx;
  maxyA = maxy;
}

void XWPDFSearchListItem::setRect(double minxA, double minyA, double maxxA, double maxyA)
{
  minx = minxA;
  miny = minyA;
  maxx = maxxA;
  maxy = maxyA;
}

XWPDFSearhList::XWPDFSearhList(QWidget * parent)
:QListWidget(parent)
{
  connect(this, SIGNAL(itemActivated(QListWidgetItem *)), this, SLOT(activePosition(QListWidgetItem*)));
}

void XWPDFSearhList::createItem(int pg, double minx, double miny, double maxx, 
                                double maxy,const QString & content)
{
  XWPDFSearchListItem * item = new XWPDFSearchListItem(this);
  item->setPage(pg);
  item->setRect(minx, miny, maxx, maxy);
  item->setText(content);
}

void XWPDFSearhList::activePosition(QListWidgetItem * item)
{
  XWPDFSearchListItem * myitem = (XWPDFSearchListItem*)(item);
  int pg = myitem->getPage();
  double minx = 0.0;
  double miny = 0.0;
  double maxx = 0.0;
  double maxy = 0.0;
  myitem->getRect(minx, miny, maxx, maxy);
  emit positionActivated(pg, minx, miny, maxx, maxy);
}

XWTeXPDFSearchWidget::XWTeXPDFSearchWidget(XWPDFSearcher * searcherA,QWidget * parent)
:QWidget(parent),
 searcher(searcherA)
{
  textLabel = new QLabel(tr("text:"), this);
  textEdit = new QLineEdit(this);

  startButton = new QToolButton(this);
	startButton->setPopupMode(QToolButton::MenuButtonPopup);

  list = new XWPDFSearhList(this);

  QGridLayout *toplayout = new QGridLayout;
  toplayout->addWidget(textLabel,0,0);
  toplayout->addWidget(textEdit,0,1,1,2);
  toplayout->addWidget(startButton,0,3);

  QVBoxLayout *layout = new QVBoxLayout;
  layout->addLayout(toplayout);
  layout->addWidget(list);

  setLayout(layout);

  QAction * a = new QAction(tr("search"), startButton);
  startButton->setDefaultAction(a);
  connect(a, SIGNAL(triggered()), this, SLOT(search()));

  QMenu * submenu = new QMenu(startButton);
  startButton->setMenu(submenu);
  a = submenu->addAction(tr("case sensitivity"));
  a->setCheckable(true);
  a->setChecked(true);
  connect(a, SIGNAL(triggered(bool)), this, SLOT(setCaseSensitivity(bool)));

  a = submenu->addAction(tr("whole word"));
  a->setCheckable(true);
  a->setChecked(true);
  connect(a, SIGNAL(triggered(bool)), this, SLOT(setWholeWord(bool)));

  connect(list, SIGNAL(positionActivated(int,double,double,double,double)), 
         this, SLOT(SIGNAL(int,double,double,double,double)));
}

void XWTeXPDFSearchWidget::search()
{
  list->clear();
  QString txt = textEdit->text();
  if (txt.isEmpty())
    return ;

  searcher->setList(list);
  searcher->setReplace(false);
  searcher->setText(txt);
  searcher->start();
}

void XWTeXPDFSearchWidget::setCaseSensitivity(bool e)
{
  searcher->setCaseSensitivity(e);
}

void XWTeXPDFSearchWidget::setWholeWord(bool e)
{
  searcher->setWholeWord(e);
}

XWQTextDocumentSearchList::XWQTextDocumentSearchList(XWQTextDocumentSearcher * searcherA, QWidget * parent)
:QTreeWidget(parent),
searcher(searcherA),
 curFileItem(0)
{
  connect(this, SIGNAL(itemActivated(QTreeWidgetItem *, int)), this, SLOT(activePosition(QTreeWidgetItem *, int)));
}

void XWQTextDocumentSearchList::reset()
{
  curFileItem = 0;
  clear();
}

void XWQTextDocumentSearchList::createFileItem(const QString & fileA)
{
  curFileItem = new QTreeWidgetItem(this, XW_S_L_FILE);
  curFileItem->setText(0, fileA);
}

void XWQTextDocumentSearchList::createTextItem(int pos, const QString & content)
{
  QTreeWidgetItem * item = 0;
  if (curFileItem)
    item = new QTreeWidgetItem(curFileItem, XW_S_L_TEXT_POS);
  else
    item = new QTreeWidgetItem(this, XW_S_L_TEXT_POS);
  item->setText(0, content);
  item->setData(0, Qt::UserRole, pos);
}

void XWQTextDocumentSearchList::activePosition(QTreeWidgetItem * item, int)
{
  if (!item)
    return ;

  switch (item->type())
  {
    default:
      break;

    case XW_S_L_FILE:
      {
        QString tmp = item->text(0);
        emit fileActivated(tmp);
      }
      break;

    case XW_S_L_TEXT_POS:
      {
        int pos = item->data(0,Qt::UserRole).toInt();
        emit positionActivated(pos, searcher->getTextLength());
      }
      break;
  }
}

XWQTextDocumentSearchWidget::XWQTextDocumentSearchWidget(XWQTextDocumentSearcher * searcherA, QWidget * parent)
:QWidget(parent),
 searcher(searcherA)
{
  textLabel = new QLabel(tr("text:"), this);
  textEdit = new QLineEdit(this);

  startButton = new QToolButton(this);
  startButton->setPopupMode(QToolButton::MenuButtonPopup);

  list = new XWQTextDocumentSearchList(searcherA,this);

  QGridLayout *toplayout = new QGridLayout;
  toplayout->addWidget(textLabel,0,0);
  toplayout->addWidget(textEdit,0,1,1,2);
  toplayout->addWidget(startButton,0,3);

  QVBoxLayout *layout = new QVBoxLayout;
  layout->addLayout(toplayout);
  layout->addWidget(list);

  setLayout(layout);

  QAction * a = new QAction(tr("search"), startButton);
  startButton->setDefaultAction(a);
  connect(a, SIGNAL(triggered()), this, SLOT(search()));
  
  QMenu * submenu = new QMenu(startButton);
  startButton->setMenu(submenu);
  a = submenu->addAction(tr("case sensitivity"));
  a->setCheckable(true);
  a->setChecked(true);
  connect(a, SIGNAL(triggered(bool)), this, SLOT(setCaseSensitivity(bool)));

  a = submenu->addAction(tr("whole word"));
  a->setCheckable(true);
  a->setChecked(true);
  connect(a, SIGNAL(triggered(bool)), this, SLOT(setWholeWord(bool)));

  submenu->addSeparator();
  QActionGroup * grp = new QActionGroup(submenu);
  grp->setExclusive(true);
  a = new QAction(tr("in current file"), this);
  a->setChecked(true);
  a->setData(XW_SEACHER_FILE);
  grp->addAction(a);
	submenu->addAction(a);

  a = new QAction(tr("in current directory"), this);
  a->setData(XW_SEACHER_CUR_DIR);
  grp->addAction(a);
	submenu->addAction(a);

  a = new QAction(tr("in all directories"), this);
  a->setData(XW_SEACHER_DIRS);
  grp->addAction(a);
	submenu->addAction(a);

  connect(grp, SIGNAL(triggered(QAction*)), this, SLOT(setSearchType(QAction*)));
  connect(list, SIGNAL(fileActivated(const QString & )), this, SIGNAL(fileActivated(const QString & )));
  connect(list, SIGNAL(positionActivated(int, int)), this, SIGNAL(positionActivated(int, int)));
}

void XWQTextDocumentSearchWidget::search()
{
  list->reset();
  QString txt = textEdit->text();
  if (txt.isEmpty())
    return ;

  searcher->setList(list);
  searcher->setReplace(false);
  searcher->setText(txt);
  searcher->start();
}

void XWQTextDocumentSearchWidget::setCaseSensitivity(bool e)
{
  searcher->setCaseSensitivity(e);
}

void XWQTextDocumentSearchWidget::setSearchType(QAction * a)
{
  if (!a)
    return ;

  searcher->setSearchType(a->data().toInt());
}

void XWQTextDocumentSearchWidget::setWholeWord(bool e)
{
  searcher->setWholeWord(e);
}

XWQTextDocumentReplaceWidget::XWQTextDocumentReplaceWidget(XWQTextDocumentSearcher * searcherA, QWidget * parent)
:QWidget(parent),
 searcher(searcherA)
{
  textLabel = new QLabel(tr("text:"), this);
  textEdit = new QLineEdit(this);

  replacingLabel = new QLabel(tr("replaced by:"), this);
  replacingEdit = new QLineEdit(this);

  startButton = new QToolButton(this);
  startButton->setPopupMode(QToolButton::MenuButtonPopup);

  list = new XWQTextDocumentSearchList(searcherA,this);

  QGridLayout *toplayout = new QGridLayout;
  toplayout->addWidget(textLabel,0,0);
  toplayout->addWidget(textEdit,0,1,1,2);
  toplayout->addWidget(replacingLabel,1,0);
  toplayout->addWidget(replacingEdit,1,1,1,2);
  toplayout->addWidget(startButton,1,3);

  QVBoxLayout *layout = new QVBoxLayout;
  layout->addLayout(toplayout);
  layout->addWidget(list);

  setLayout(layout);

  QAction * a = new QAction(tr("replace"), startButton);
  startButton->setDefaultAction(a);
  connect(a, SIGNAL(triggered()), this, SLOT(replace()));

  QMenu * submenu = new QMenu(startButton);
  startButton->setMenu(submenu);
  a = submenu->addAction(tr("case sensitivity"));
  a->setCheckable(true);
  a->setChecked(true);
  connect(a, SIGNAL(triggered(bool)), this, SLOT(setCaseSensitivity(bool)));

  a = submenu->addAction(tr("whole word"));
  a->setCheckable(true);
  a->setChecked(true);
  connect(a, SIGNAL(triggered(bool)), this, SLOT(setWholeWord(bool)));

  submenu->addSeparator();
  QActionGroup * grp = new QActionGroup(submenu);
  grp->setExclusive(true);
  a = new QAction(tr("in current file"), this);
  a->setChecked(true);
  a->setData(XW_SEACHER_FILE);
  grp->addAction(a);
	submenu->addAction(a);

  a = new QAction(tr("in current directory"), this);
  a->setData(XW_SEACHER_CUR_DIR);
  grp->addAction(a);
	submenu->addAction(a);

  a = new QAction(tr("in all directories"), this);
  a->setData(XW_SEACHER_DIRS);
  grp->addAction(a);
	submenu->addAction(a);

  connect(grp, SIGNAL(triggered(QAction*)), this, SLOT(setSearchType(QAction*)));
  connect(list, SIGNAL(fileActivated(const QString & )), this, SIGNAL(fileActivated(const QString & )));
  connect(list, SIGNAL(positionActivated(int, int)), this, SIGNAL(positionActivated(int, int)));
}

void XWQTextDocumentReplaceWidget::replace()
{
  list->reset();
  QString txt = textEdit->text();
  if (txt.isEmpty())
    return ;

  searcher->setList(list);
  searcher->setReplace(true);
  searcher->setText(txt);
  txt = replacingEdit->text();
  searcher->setReplacing(txt);
  searcher->start();
}

void XWQTextDocumentReplaceWidget::setCaseSensitivity(bool e)
{
  searcher->setCaseSensitivity(e);
}

void XWQTextDocumentReplaceWidget::setSearchType(QAction * a)
{
  if (!a)
    return ;

  searcher->setSearchType(a->data().toInt());
}

void XWQTextDocumentReplaceWidget::setWholeWord(bool e)
{
  searcher->setWholeWord(e);
}
