/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWTeXDocument.h"
#include "XWTeXDocumentObject.h"
#include "XWTeXDocumentPage.h"
#include "XWTeXDocSearcher.h"

XWTeXDocSearchListItem::XWTeXDocSearchListItem( QListWidget * parent, int type)
:QListWidgetItem(parent,type),
 start(0),
 end(0),
 pg(0),
 minx(0.0),
 miny(0.0),
 maxx(0.0),
 maxy(0.0),
 block(0)
{}

void XWTeXDocSearchListItem::getRect(double & minxA, double & minyA, double & maxxA, double & maxyA)
{
  minx = minxA;
  miny = minyA;
  maxx = maxxA;
  maxy = maxyA;
}

void XWTeXDocSearchListItem::setRect(double minxA, double minyA, double maxxA, double maxyA)
{
  minx = minxA;
  miny = minyA;
  maxx = maxxA;
  maxy = maxyA;
}

XWTeXDocSearhList::XWTeXDocSearhList(QWidget * parent)
:QListWidget(parent),
 doc(0)
{
  connect(this, SIGNAL(itemActivated(QListWidgetItem *)), this, SLOT(activePosition(QListWidgetItem*)));
}

void XWTeXDocSearhList::createItem(XWTeXDocumentBlock * block,
                                   int start, int end, int pg, 
                                   double minx, double miny, 
                                   double maxx, double maxy,
                                   const QString & content)
{
  XWTeXDocSearchListItem * item = new XWTeXDocSearchListItem(this);
  item->setBlock(block);
  item->setStart(start);
  item->setEnd(end);
  item->setPage(pg);
  item->setRect(minx, miny, maxx, maxy);
  item->setText(content);
}

void XWTeXDocSearhList::activePosition(QListWidgetItem * item)
{
  XWTeXDocSearchListItem * xitem = (XWTeXDocSearchListItem*)(item);
  XWTeXDocumentBlock * block = xitem->getBlock();
  XWTeXDocumentCursor * cursor = doc->getCursor(block);
  doc->setCurrentCursor(cursor);
  cursor->setStartPos(xitem->getStart());
  cursor->setEndPos(xitem->getEnd());
  cursor->setHitPos(xitem->getEnd());
  int pg = xitem->getPage();
  double minx = 0.0;
  double miny = 0.0;
  double maxx = 0.0;
  double maxy = 0.0;
  xitem->getRect(minx, miny, maxx, maxy);
  emit positionActivated(pg, minx, miny, maxx, maxy);
}

XWTeXDocSearchWidget::XWTeXDocSearchWidget(XWTeXDocSearcher * searcherA, QWidget * parent)
:QWidget(parent),
 searcher(searcherA)
{
  textLabel = new QLabel(tr("text:"), this);
  textEdit = new QLineEdit(this);

  startButton = new QToolButton(this);
  startButton->setPopupMode(QToolButton::MenuButtonPopup);

  list = new XWTeXDocSearhList(this);

  QGridLayout *toplayout = new QGridLayout;
  toplayout->addWidget(textLabel,0,0);
  toplayout->addWidget(textEdit,0,1,1,2);
  toplayout->addWidget(startButton,0,3);

  QVBoxLayout *layout = new QVBoxLayout;
  layout->addLayout(toplayout);
  layout->addWidget(list);

  setLayout(layout);

  QAction * a = new QAction(tr("search"),startButton);
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
         this, SIGNAL(positionActivated(int,double,double,double,double)));
}

void XWTeXDocSearchWidget::search()
{
  list->clear();
  QString txt = textEdit->text();
  if (txt.isEmpty())
    return ;

  list->setDoc(searcher->getDoc());
  searcher->setList(list);
  searcher->setReplace(false);
  searcher->setText(txt);
  searcher->start();
}

void XWTeXDocSearchWidget::setCaseSensitivity(bool e)
{
  searcher->setCaseSensitivity(e);
}

void XWTeXDocSearchWidget::setWholeWord(bool e)
{
  searcher->setWholeWord(e);
}

XWTeXDocReplaceWidget::XWTeXDocReplaceWidget(XWTeXDocSearcher * searcherA,QWidget * parent)
:QWidget(parent),
 searcher(searcherA)
{
  textLabel = new QLabel(tr("text:"), this);
  textEdit = new QLineEdit(this);

  replacingLabel = new QLabel(tr("replaced by:"), this);
  replacingEdit = new QLineEdit(this);

  startButton = new QToolButton(this);
  startButton->setPopupMode(QToolButton::MenuButtonPopup);

  list = new XWTeXDocSearhList(this);

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

  connect(list, SIGNAL(positionActivated(int,double,double,double,double)), 
         this, SIGNAL(positionActivated(int,double,double,double,double)));
}

void XWTeXDocReplaceWidget::replace()
{
  list->clear();
  QString txt = textEdit->text();
  if (txt.isEmpty())
    return ;

  list->setDoc(searcher->getDoc());
  searcher->setList(list);
  searcher->setReplace(true);
  searcher->setText(txt);
  txt = replacingEdit->text();
  searcher->setReplacing(txt);
  searcher->start();
}

void XWTeXDocReplaceWidget::setCaseSensitivity(bool e)
{
  searcher->setCaseSensitivity(e);
}

void XWTeXDocReplaceWidget::setWholeWord(bool e)
{
  searcher->setWholeWord(e);
}

XWTeXDocSearcher::XWTeXDocSearcher(QObject * parent)
:XWSearcher(parent),
 doc(0)
{}

void XWTeXDocSearcher::run()
{
  if (!doc)
    return ;
    
  if (isReplace)
  {
    doc->setReplaceCondition(text, replacing,caseSensitivity,wholeWord, false);
    doc->replace(list);
  }
  else
  {
    doc->setFindCondition(text,caseSensitivity,wholeWord, false);
    doc->find(list);
  }
}
