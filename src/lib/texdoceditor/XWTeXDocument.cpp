/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <QApplication>
#include <QtGui>
#include "XWApplication.h"
#include "XWFileName.h"
#include "XWTeXPool.h"
#include "XWTikzDialog.h"
#include "XWPSTricksDialog.h"
#include "XWTeXDocumentObject.h"
#include "XWTeXDocumentUndoCommand.h"
#include "XWTeXDocumentPage.h"
#include "XWTeXDocument.h"

#define XWTEXDOCUMENTPAGES_PP 10

XWTeXDocument::XWTeXDocument(QObject * parent)
 : QObject(parent),
   head(0),
   last(0),
   isModified(false),
   pageWidth(596.95),
   savedPageWidth(596.95),
   pageHeight(844.84),
   savedPageHeight(844.84),
   twocolumn(false),
   direction(0),
   merginWidth(72.27),
   lineSkip(1.5),
   columSep(10.0),
   top(0),
   bottom(0),
   left(0),
   right(0),
   savedTop(0),
   savedBottom(0),
   normalsize(10),
   currentSize(10),
   italic(false),
   weight(QFont::Normal),
   showPreamble(false),
   isappendix(false),
   inPreamble(false),
   partCounter(0),
   chapterCounter(0),
   sectionCounter(0),
   subsectionCounter(0),
   subsubsectionCounter(0),
   paragraphCounter(0),
   subparagraphCounter(0),
   itemCounter(0),
   itemLevel(0),
   bibitemCounter(0),
   cursorWidth(1),
   allSelect(false),
   lineMax(80),
   maxPageNum(0),
   lastPage(0),
   curPage(0),
   isTitlePage(false),
   pages(0)
{
  resetSides();
  curCursor = 0;
  undoStack = new QUndoStack;

  inCodecName = "UTF-8";
  outCodecName = "UTF-8";
}

XWTeXDocument::~XWTeXDocument()
{
  clearCursors();
  clearPages();

  if (undoStack)
  {
    delete undoStack;
    undoStack = 0;
  }
}

void XWTeXDocument::addSelect(XWTeXDocumentBlock * block)
{
  if (!selected.contains(block))
    selected << block;
}

void XWTeXDocument::append(XWTeXDocumentObject * obj)
{
  obj->setParent(this);
  if (!head)
  {
    head = obj;
    last = obj;
  }
  else
  {
    obj->prev = last;
    last->next = obj;
    last = obj;
  }
  last->next = 0;
}

void XWTeXDocument::append(XWTeXDocumentObject * sobj,XWTeXDocumentObject*eobj)
{
  XWTeXDocumentObject * tmp = sobj;
  while (tmp)
  {
    tmp->setParent(this);
    if (tmp == eobj)
      break;
    tmp = tmp->next;
  }
  
  if (!head)
  {
    head = sobj;
    last = eobj;
  }
  else
  {
    last->next = sobj;
    sobj->prev = last;
    last = eobj;    
  }

  last->next = 0;
}

void XWTeXDocument::bfseries()
{
  weightStack.push(weight);
  weight = QFont::Bold;
}

void XWTeXDocument::drawPage(int pageno, QPainter * painter, const QColor & pagecolor, const QRectF & rect)
{
  if (pageno < 0)
    pageno = 0;

  if (pageno > lastPage)
    pageno = lastPage;

  XWTeXDocumentPage * pg = pages[pageno];
  pg->draw(painter,pagecolor,rect);
}

void XWTeXDocument::drawPic(const QString & outname, const QString & fmt,double xscale, double yscale, const QColor & pagecolor)
{
  QString suffix = fmt.toLower();
  suffix.insert(0, ".");
  QByteArray fmtba = fmt.toAscii();
  for (int i = 0; i <= lastPage; i++)
  {
    XWTeXDocumentPage * pg = pages[i];
    QImage * img = new QImage(pageWidth * xscale,pageHeight * yscale, QImage::Format_RGB32);
    QPainter * painter = new QPainter(img);
    painter->fillRect(0,0,pageWidth * xscale,pageHeight * yscale,pagecolor);
    painter->scale(xscale,yscale);
    pg->drawPic(painter);
    delete painter;
    QString filename = QString("%1%2%3").arg(outname).arg(i).arg(suffix);
    img->save(filename, fmtba.constData(), 0);
		delete img;
  }
}

void XWTeXDocument::find(XWTeXDocSearhList * listA)
{
  for (int i = 0; i <= lastPage; i++)
  {
    XWTeXDocumentPage * pg = pages[i];
    pg->find(i, listA);
  }
}

void XWTeXDocument::findNext()
{
  if (curPage > lastPage)
    curPage = 0;

  while (!pages[curPage]->findNext())
  {
    curPage++;
    if (curPage > lastPage)
      break;
  }
}

void XWTeXDocument::footnoteSize()
{
  sizeStack.push(currentSize);

  switch (normalsize)
  {
    case 10:
      currentSize = 8;
      break;

    case 11:
      currentSize = 9;
      break;

    default:
      currentSize = 10;
      break;
  }
}

XWTeXDocumentBlock * XWTeXDocument::getCurrentBlock()
{
  if (!curCursor)
    return 0;

  return curCursor->getBlock();
}

QColor  XWTeXDocument::getCurrentBlockColor()
{
  if (!curCursor)
    return QColor();

  return curCursor->getBlock()->color;
}

QString XWTeXDocument::getCurrentBlockText()
{
  if (!curCursor)
    return QString();

  return curCursor->getBlock()->getText();
}

QFont XWTeXDocument::getCurrentFont()
{
  if (!curCursor)
    return QFont();

  return curCursor->getBlock()->font;
}

XWTeXDocumentPage * XWTeXDocument::getCurrentPage()
{
  if (maxPageNum <= 0)
  {
    maxPageNum = 50;
    pages = (XWTeXDocumentPage**)realloc(pages, maxPageNum * sizeof(XWTeXDocumentPage*));
    for (int i = 0; i < maxPageNum; i++)
      pages[i] = new XWTeXDocumentPage;
  }

  if (curPage < 0)
    curPage = 0;

  lastPage = curPage;
  return pages[lastPage];
}

QString XWTeXDocument::getCurrentSelected()
{
  if (!curCursor)
    return QString();

  return curCursor->getBlock()->getSelected();
}

int XWTeXDocument::getCurrentStart()
{
  if (!curCursor)
    return 0;

  return curCursor->getStartPos();
}

XWTeXDocumentCursor * XWTeXDocument::getCursor(XWTeXDocumentBlock * block)
{
  XWTeXDocumentCursor * c = 0;
  if (cursors.contains(block))
    c = cursors[block];
  else
  {
    c = new XWTeXDocumentCursor(block);
    cursors[block] = c;
  }

  return c;
}

int XWTeXDocument::getHitPos()
{
  if (!curCursor)
    return 0;

  return curCursor->getHitPos();
}

QString XWTeXDocument::getInputCodecName()
{
  QString ret = inCodecName;
	return ret;
}

XWTeXDocumentPage * XWTeXDocument::getNewPage()
{
  XWTeXDocumentPage * page = 0;
  if (maxPageNum <= 0)
  {
    maxPageNum = 50;
    pages = (XWTeXDocumentPage**)realloc(pages, maxPageNum * sizeof(XWTeXDocumentPage*));
    for (int i = 0; i < maxPageNum; i++)
      pages[i] = new XWTeXDocumentPage;

    if (curPage < 0)
      curPage = 0;

    lastPage = curPage;
    page = pages[lastPage];
  }
  else
  {
    lastPage = ++curPage;
    if (curPage >= maxPageNum)
    {
      maxPageNum += 10;
      pages = (XWTeXDocumentPage**)realloc(pages, maxPageNum * sizeof(XWTeXDocumentPage*));
      for (int i = curPage; i < maxPageNum; i++)
        pages[i] = new XWTeXDocumentPage;
    }
    page = pages[lastPage];
    page->clear();
  }

  switch (direction)
  {
    case TEX_DOC_WD_TLT:
    case TEX_DOC_WD_TRT:
      top = savedTop;
      bottom = savedBottom;
      break;

    case TEX_DOC_WD_RTT:
      right = savedTop;
      left = savedBottom;
      break;

    default:
      left = savedTop;
      right = savedBottom;
      break;
  }

  return page;
}

QString XWTeXDocument::getOutputCodecName()
{
  QString ret = outCodecName;
	return ret;
}

QString XWTeXDocument::getPath()
{
  XWFileName file(fileName);
  return file.getPath();
}

bool XWTeXDocument::hasSelect()
{
  return !selected.isEmpty() || allSelect;
}

bool XWTeXDocument::hitTest(int pageno,const QPointF & p)
{
  if (pageno < 0)
    pageno = 0;

  if (pageno > lastPage)
    pageno = lastPage;

  curPage = pageno;
  XWTeXDocumentPage * pg = pages[curPage];
  curCursor = 0;
  bool r = pg->hitTest(p);
  emit sectionChanged();
  return r;
}

void XWTeXDocument::huge()
{
  sizeStack.push(currentSize);

  switch (normalsize)
  {
    case 10:
      currentSize = 21;
      break;

    case 11:
      currentSize = 21;
      break;

    default:
      currentSize = 25;
      break;
  }
}

void XWTeXDocument::Huge()
{
  sizeStack.push(currentSize);

  switch (normalsize)
  {
    case 10:
      currentSize = 25;
      break;

    case 11:
      currentSize = 25;
      break;

    default:
      currentSize = 25;
      break;
  }
}

void XWTeXDocument::insert(XWTeXDocumentObject * obj)
{
  if (!head)
  {
    head = obj;
    last = obj;
    last->next = 0;
    head->prev = 0;
  }
  else 
  {
    XWTeXDocumentBlock * block = curCursor->getBlock();
    XWTeXDocumentObject * pobj = block->getObject();
    if (pobj == head)
    {
      if (pobj->isAtStart())
      {
        obj->next = pobj;
        pobj->prev = obj;
        head = obj;
        head->prev = 0;
      }
      else
      {
        obj->prev = head;
        obj->next = head->next;
        if (head->next)
          head->next->prev = obj;
        else
        {
          last = obj;
          last->next = 0;
        }

        head->next = obj;      
      }    

      obj->setParent(this);
    }
    else if (pobj == last)
    {
      if (pobj->isAtStart())
      {
        obj->prev = last->prev;
        if (last->prev)
          last->prev->next = obj;
        last->prev = obj;
        obj->next = last;
      }
      else
      {
        obj->prev = last;
        last->next = obj;
        last = obj;
        last->next = 0;
      }    
      obj->setParent(this);
    }
    else
    {
      if (obj->prev)
        obj->prev->insertAtAfter(obj);
      else if (obj->next)
        obj->next->insertAtBefore(obj);
      else
      {      
        if (block->parent() == obj->parent())
          block->insert(obj);
        else
        {        
          while (pobj->parent() != obj->parent())
            pobj = (XWTeXDocumentObject*)(pobj->parent());
          
          pobj->insert(obj);
        }
      }    
    }  
  }  

  obj->setCursor();
}

void XWTeXDocument::insertChildren(XWTeXDocumentObject*obj, 
                                         XWTeXDocumentObject*sobj, 
                                         XWTeXDocumentObject*eobj)
{
  XWTeXDocumentObject * tmp = sobj;
  while (tmp)
  {
    tmp->setParent(this);
    if (tmp == eobj)
      break;
    tmp = tmp->next;
  }

  if (!obj)
  {
    if (!head)
    {
      head = sobj;
      head->prev = 0;
      last = eobj;
      last->next = 0;
    }
    else
    {
      eobj->next = head;
      head->prev = eobj;
      head = sobj;
      head->prev = 0;
    }
  } 
  else if (obj == last)
  {
    obj->next = sobj;
    sobj->prev = obj;
    last = eobj;
    last->next = 0;
  }
  else if (obj->isAtStart())
  {
    sobj->prev = obj->prev;
    if (obj == head)
      head = sobj;
    else
      obj->prev->next = sobj;    
    eobj->next = obj;
    obj->prev = eobj;
  }
  else
  {
    sobj->prev = obj;    
    eobj->next = obj->next;
    obj->next->prev = eobj;  
    obj->next = sobj;
  }  
}

void XWTeXDocument::insertText(const QString & str)
{
  if (!curCursor)
    return ;

  XWTeXDocumentBlock * block = curCursor->getBlock();
  if (block->isReadOnly())
    return ;

  QUndoCommand * cmd = 0;
  if (curCursor->hasSelect())
  {
    cmd = new QUndoCommand;
    new XWTeXDocumentDeleteString(block,cmd);
    new XWTeXDocumentInsertString(block,str,cmd);
  }
  else
    cmd = new XWTeXDocumentInsertString(block,str);

  undoStack->push(cmd);
  isModified = true;
  breakPage();    
  emit modificationChanged(isModified);
}

bool XWTeXDocument::isCurrentBlock(XWTeXDocumentBlock * block)
{
  return curCursor && (curCursor->block == block);
}

void XWTeXDocument::keyInput(const QString & str)
{
  if (!curCursor)
    return ;

  XWTeXDocumentBlock * block = curCursor->getBlock();
  if (block->isReadOnly())
    return ;

  QUndoCommand * cmd = 0;
  if (block->type() == TEX_DOC_B_TEXT)
  {
    QRegExp patt("[^\\$&\\^\\{\\}\\~\\\\]");
    if (!str.contains(patt))
      cmd = new XWTeXDocumentInsertString(block,str);
    else
    {
      XWTeXDocumentObject * obj = 0;
      if (str[0] == QChar('\\'))
      {
        if (block->isAtEnd())
        {
          XWTeXControlSequence * unkown = new XWTeXControlSequence(this,block->parent());
          unkown->append(str);
          obj = unkown;
          cmd = new XWTeXDocumentNewObject(obj);
        }
        else
        {
          QString tmp = block->text;    
          int pos = curCursor->getHitPos();
          int len = tmp.length();
          if (pos < len)
            tmp.insert(pos,str);
          else
            tmp.append(str);
          int p = pos;
          len = tmp.length();
          QString key = XWTeXDocumentObject::scanControlSequence(tmp, len,pos);    
          obj = createObject(key,tmp,pos,block->parent());
          cmd = new QUndoCommand;
          new XWTeXDocumentNewObject(obj,cmd);
          if (p == 0)
            new XWTeXDocumentRemoveObject(block,cmd);
          else
          {
            curCursor->setStartPos(p);
            curCursor->setEndPos(block->text.length());
            new XWTeXDocumentDeleteString(block,cmd);
          }
          
          if (pos < len)
          {
            XWTeXDocumentText * nblock = new XWTeXDocumentText(this,block->parent());
            nblock->text = tmp.mid(pos,-1);
            new XWTeXDocumentInsertObject(nblock,obj,cmd);
          }
        }
      }
      else
      {
        QString tmp = QString("\\%1").arg(str);
        obj = new XWTeXControlSequence(TEX_DOC_B_UNKNOWN,this, tmp, block->parent());
        cmd = new XWTeXDocumentNewObject(obj);
      }
    }  
  }
  else if (block->type() == TEX_DOC_B_UNKNOWN)
  {    
    QString tmp = block->text;    
    int pos = curCursor->getHitPos();
    int len = tmp.length();
    if (pos < len)
      tmp.insert(pos,str);
    else
      tmp.append(str);
    pos = 0;
    len = tmp.length();
    QString key = XWTeXDocumentObject::scanControlSequence(tmp, len,pos);    
    XWTeXDocumentObject * obj = createObject(key,tmp,pos,block->parent());
    if (obj->type() == TEX_DOC_B_UNKNOWN)
    {
      delete obj;
      cmd = new XWTeXDocumentInsertString(block,str);
    }
    else
    {
      cmd = new QUndoCommand;
      new XWTeXDocumentNewObject(obj,cmd);
      new XWTeXDocumentRemoveObject(block,cmd);
      if (pos < len)
      {
        XWTeXDocumentText * nblock = new XWTeXDocumentText(this,block->parent());
        nblock->text = tmp.mid(pos,-1);
        new XWTeXDocumentInsertObject(nblock,obj,cmd);
      }
    }
  }
  else
  {
    if (curCursor->hasSelect())
    {
      cmd = new QUndoCommand;
      new XWTeXDocumentDeleteString(block,cmd);
      new XWTeXDocumentInsertString(block,str,cmd);
    }
    else
      cmd = new XWTeXDocumentInsertString(block,str);
  }  

  undoStack->push(cmd);
  isModified = true;
  breakPage();    
  emit modificationChanged(isModified);
}

void XWTeXDocument::large()
{
  sizeStack.push(currentSize);

  switch (normalsize)
  {
    case 10:
      currentSize = 12;
      break;

    case 11:
      currentSize = 14;
      break;

    default:
      currentSize = 18;
      break;
  }
}

void XWTeXDocument::Large()
{
  sizeStack.push(currentSize);

  switch (normalsize)
  {
    case 10:
      currentSize = 14;
      break;

    case 11:
      currentSize = 15;
      break;

    default:
      currentSize = 17;
      break;
  }
}

void XWTeXDocument::LARGE()
{
  sizeStack.push(currentSize);

  switch (normalsize)
  {
    case 10:
      currentSize = 17;
      break;

    case 11:
      currentSize = 17;
      break;

    default:
      currentSize = 21;
      break;
  }
}

void XWTeXDocument::moveToFirstRow(int pageno)
{
  if (pageno < 0)
    pageno = 0;

  if (pageno > lastPage)
    pageno = lastPage;

  curPage = pageno;
  XWTeXDocumentPage * pg = pages[curPage];
  pg->moveToFirstRow();
}

void XWTeXDocument::moveToLastRow(int pageno)
{
  if (pageno < 0)
    pageno = 0;

  if (pageno > lastPage)
    pageno = lastPage;

  curPage = pageno;
  XWTeXDocumentPage * pg = pages[curPage];
  pg->moveToLastRow();
}

bool XWTeXDocument::moveToNextChar(int pageno,bool & m)
{
  if (pageno < 0)
    pageno = 0;

  if (pageno > lastPage)
    pageno = lastPage;

  curPage = pageno;
  XWTeXDocumentPage * pg = pages[curPage];
  return pg->moveToNextChar(m);
}

bool XWTeXDocument::moveToNextRow(int pageno)
{
  if (pageno < 0)
    pageno = 0;

  if (pageno > lastPage)
    pageno = lastPage;

  curPage = pageno;
  XWTeXDocumentPage * pg = pages[curPage];
  return pg->moveToNextRow();
}

bool XWTeXDocument::moveToNextWord(int pageno, bool & m)
{
  if (pageno < 0)
    pageno = 0;

  if (pageno > lastPage)
    pageno = lastPage;

  curPage = pageno;
  XWTeXDocumentPage * pg = pages[curPage];
  return pg->moveToNextWord(m);
}

void XWTeXDocument::moveToPageEnd(int pageno)
{
  if (pageno < 0)
    pageno = 0;

  if (pageno > lastPage)
    pageno = lastPage;

  curPage = pageno;
  XWTeXDocumentPage * pg = pages[curPage];
  pg->moveToEnd();
}

void XWTeXDocument::moveToPageStart(int pageno)
{
  if (pageno < 0)
    pageno = 0;

  if (pageno > lastPage)
    pageno = lastPage;

  curPage = pageno;
  XWTeXDocumentPage * pg = pages[curPage];
  pg->moveToStart();
}

bool XWTeXDocument::moveToPreviousChar(int pageno, bool & m)
{
  if (pageno < 0)
    pageno = 0;

  if (pageno > lastPage)
    pageno = lastPage;

  curPage = pageno;
  XWTeXDocumentPage * pg = pages[curPage];
  return pg->moveToPreviousChar(m);
}

bool XWTeXDocument::moveToPreviousRow(int pageno)
{
  if (pageno < 0)
    pageno = 0;

  if (pageno > lastPage)
    pageno = lastPage;

  curPage = pageno;
  XWTeXDocumentPage * pg = pages[curPage];
  return pg->moveToPreviousRow();
}

bool XWTeXDocument::moveToPreviousWord(int pageno, bool & m)
{
  if (pageno < 0)
    pageno = 0;

  if (pageno > lastPage)
    pageno = lastPage;

  curPage = pageno;
  XWTeXDocumentPage * pg = pages[curPage];
  return pg->moveToPreviousWord(m);
}

void XWTeXDocument::moveToRowEnd(int pageno)
{
  if (pageno < 0)
    pageno = 0;

  if (pageno > lastPage)
    pageno = lastPage;

  curPage = pageno;
  XWTeXDocumentPage * pg = pages[curPage];
  pg->moveToRowEnd();
}

void XWTeXDocument::moveToRowStart(int pageno)
{
  if (pageno < 0)
    pageno = 0;

  if (pageno > lastPage)
    pageno = lastPage;

  curPage = pageno;
  XWTeXDocumentPage * pg = pages[curPage];
  pg->moveToRowStart();
}

void XWTeXDocument::normal()
{
  weightStack.push(weight);
  weight = QFont::Normal;
}

void XWTeXDocument::normalSize()
{
  sizeStack.push(currentSize);
  currentSize = normalsize;
}

void XWTeXDocument::open(const QString & filename)
{
  fileName = filename;
	if (!fileName.isEmpty())
  {
    QFile file(fileName);
		if (file.open(QFile::ReadOnly | QFile::Text))
    {
      char buf[1025];
      qint64 len = file.readLine(buf, 1024);
      file.seek(0);
      if (len > 2)
      {
        if ((buf[0] == 0x5c || buf[0] == 0x25) && buf[1] == 0)
            inCodecName = "UTF-16LE";
        else if (buf[0] == 0 && (buf[1] == 0x5c || buf[1] == 0x25))
          inCodecName = "UTF-16BE";
        else if (buf[0] == 0x5c)
        {
          QByteArray ba = QByteArray::fromRawData(buf, len);
          int i = ba.indexOf("coding:");
          if (i > 1)
          {
            i += 7;
            for (; i< ba.size(); i++)
            {
              if (ba.at(i) != ' ' && ba.at(i) != '\t')
                break;
            }

            int k = i;

            for (; i< ba.size(); i++)
            {
              if (ba.at(i) == ' ' || ba.at(i) == '\t')
                break;
            }

            inCodecName = ba.mid(k,i - k);
          }
        }
      }

      QTextStream stream(&file);
      outCodecName = inCodecName;
      QTextCodec *codec = QTextCodec::codecForName(inCodecName);
      stream.setCodec(codec);
      QApplication::setOverrideCursor(Qt::WaitCursor);
      QString str = stream.readAll();
      file.close();
      int pos = 0;
      int ln = str.length();
      setup(str,ln, pos);
      breakPage();

      QApplication::restoreOverrideCursor();
    }
  }
}

void XWTeXDocument::popItalic()
{
  italic = italicStack.pop();
}

void XWTeXDocument::popSize()
{
  currentSize = sizeStack.pop();
}

void XWTeXDocument::popWeight()
{
  weight = weightStack.pop();
}

void XWTeXDocument::redo()
{
  undoStack->redo();
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWTeXDocument::remove(XWTeXDocumentObject * obj)
{
  if (obj)
  {
    obj->remove();
    if (obj == head)
    {
      head = obj->next;
      if (head)
        head->prev = 0;
    }
      
    if (obj == last)
    {
      last = obj->prev;
      if (last)
        last->next = 0;
    }
  }
}

void XWTeXDocument::removeChildren(XWTeXDocumentObject**sobj, 
                                           XWTeXDocumentObject**eobj,
                                           XWTeXDocumentObject**obj)
{
  if (*sobj == *eobj)
  {
    *obj = (*sobj)->prev;
    (*sobj)->remove();
    if ((*sobj) == head)
    {
      head = (*sobj)->next;
      if (head)
        head->prev = 0;
    }      

    if ((*sobj) == last)
    {
      last = (*sobj)->prev;
      if (last)
        last->next = 0;
    }      
  }
  else
  {
    XWTeXDocumentObject * prevobj = (*sobj)->prev;
    *obj = prevobj;
    XWTeXDocumentObject * nextobj = (*eobj)->next;

    if (prevobj)
      prevobj->next = nextobj;
    else
    {
      head = nextobj;
      if (nextobj)
        nextobj->prev = 0;
    }      

    if (nextobj)
      nextobj->prev = prevobj;
    else
    {
      last = prevobj;
      if (prevobj)
        prevobj->next = 0;
    }      
  }
}

void XWTeXDocument::replace(XWTeXDocSearhList * listA)
{
  QList<XWTeXDocumentBlock*> blocklist;
  for (int i = 0; i <= lastPage; i++)
  {
    XWTeXDocumentPage * pg = pages[i];
    pg->find(blocklist);
  }

  if (blocklist.isEmpty())
    return ;

  QUndoCommand * cmd = new QUndoCommand();
  QTextDocument::FindFlags op = getFindFlags();
  for (int i = 0; i < blocklist.size(); i++)
  {
    QString tmp = blocklist[i]->text;
    QTextDocument d(tmp);
    QTextCursor cursor(&d);
    while (!cursor.isNull())
    {
      cursor = d.find(findString, cursor, op);
      if (!cursor.isNull())
        cursor.insertText(replaceString);
    }
    blocklist[i]->selectBlock();
    new XWTeXDocumentDeleteString(blocklist[i], cmd);
    new XWTeXDocumentInsertString(blocklist[i],tmp, cmd);
  }

  undoStack->push(cmd);
  isModified = true;
  breakPage();    

  int k = 0;
  bool r = false;
  for (int i = 0; i < blocklist.size(); i++)
  {
    for (int j = k; j <= lastPage; j++)
    {
      XWTeXDocumentPage * pg = pages[i];
      if (pg->find(j, blocklist[i], listA))
      {
        k++;
        r = true;
      }
      else
      {
        if (r)
        {
          k--;
          r = false;
          break;
        }
      }
    }
  }

  emit modificationChanged(isModified);
}

void XWTeXDocument::replaceNext()
{
  if (curPage > lastPage)
    curPage = 0;

  while (!pages[curPage]->replaceNext())
  {
    curPage++;
    if (curPage > lastPage)
      break;
  }    
}

void XWTeXDocument::resetSides()
{
  top = merginWidth;
  bottom = pageHeight - merginWidth;
  left = merginWidth;
  right = pageWidth - merginWidth;

  switch (direction)
  {
    case TEX_DOC_WD_TLT:
    case TEX_DOC_WD_TRT:
      savedTop = top;
      savedBottom = bottom;
      break;

    case TEX_DOC_WD_RTT:
      savedTop = right;
      savedBottom = left;
      break;

    default:
      savedTop = left;
      savedBottom = right;
      break;
  }
}

void XWTeXDocument::resetSelect()
{
  for (int i = 0; i < selected.size(); i++)
  {
    XWTeXDocumentBlock * block = selected[i];
    block->resetSelect();
  }
  clearSelected();
  allSelect = false;
}

void XWTeXDocument::save()
{
  if (!isModified)
    return ;

  QFile file(fileName);
  if (file.open(QFile::WriteOnly | QFile::Truncate))
  {
    QApplication::setOverrideCursor(Qt::WaitCursor);

    QTextStream stream(&file);
    stream.setGenerateByteOrderMark(false);
    QTextCodec *codec = QTextCodec::codecForName(outCodecName);
    stream.setCodec(codec);

    int linelen = 0;
    XWTeXDocumentObject * obj = head;
    while (obj)
    {
      obj->write(stream, linelen);
      obj = obj->getNext();
    }

    if (linelen != 0)
    {
      stream << "\n";
      linelen = 0;
    }

    file.close();

    QApplication::restoreOverrideCursor();

    isModified = false;
    emit modificationChanged(isModified);
  }
}

void XWTeXDocument::saveAs(const QString & filename)
{
  fileName = filename;
  save();
}

void XWTeXDocument::scriptSize()
{
  sizeStack.push(currentSize);

  switch (normalsize)
  {
    case 10:
      currentSize = 7;
      break;

    case 11:
      currentSize = 8;
      break;

    default:
      currentSize = 9;
      break;
  }
}

void XWTeXDocument::selectAll()
{
  allSelect = true;
}

void XWTeXDocument::selectEndOfDocument(int pageno, double & minX, double & minY, double & maxX, double & maxY)
{
  if (pageno < 0)
    pageno = 0;

  if (pageno > lastPage)
    pageno = lastPage;

  XWTeXDocumentPage * pg = pages[pageno];
  pg->selectEnd(minX,minY,maxX,maxY);
  for (int i = pageno + 1; i <= lastPage; i++)
  {
    pg = pages[i];
    pg->selectAll(minX,minY,maxX,maxY);
  }
}

void XWTeXDocument::selectFirstRow(int pageno,bool all, double & minX, double & minY, double & maxX, double & maxY)
{
  if (pageno < 0)
    pageno = 0;

  if (pageno > lastPage)
    pageno = lastPage;

  curPage = pageno;
  XWTeXDocumentPage * pg = pages[curPage];
  pg->selectFirstRow(all, minX,minY,maxX,maxY);
}

void XWTeXDocument::selectLastRow(int pageno,bool all, double & minX, double & minY, double & maxX, double & maxY)
{
  if (pageno < 0)
    pageno = 0;

  if (pageno > lastPage)
    pageno = lastPage;

  curPage = pageno;
  XWTeXDocumentPage * pg = pages[curPage];
  pg->selectLastRow(all, minX,minY,maxX,maxY);
}

bool XWTeXDocument::selectNextChar(int pageno,double & minX, double & minY, double & maxX, double & maxY)
{
  if (pageno < 0)
    pageno = 0;

  if (pageno > lastPage)
    pageno = lastPage;

  curPage = pageno;
  XWTeXDocumentPage * pg = pages[curPage];
  return pg->selectNextChar(minX,minY,maxX,maxY);
}

bool XWTeXDocument::selectNextRow(int pageno,double & minX, double & minY, double & maxX, double & maxY)
{
  if (pageno < 0)
    pageno = 0;

  if (pageno > lastPage)
    pageno = lastPage;

  curPage = pageno;
  XWTeXDocumentPage * pg = pages[curPage];
  return pg->selectNextRow(minX,minY,maxX,maxY);
}

bool XWTeXDocument::selectNextWord(int pageno,double & minX, double & minY, double & maxX, double & maxY)
{
  if (pageno < 0)
    pageno = 0;

  if (pageno > lastPage)
    pageno = lastPage;

  curPage = pageno;
  XWTeXDocumentPage * pg = pages[curPage];
  return pg->selectNextWord(minX,minY,maxX,maxY);
}

void XWTeXDocument::selectPage(int pageno,double & minX, double & minY, double & maxX, double & maxY)
{
  if (pageno < 0)
    pageno = 0;

  if (pageno > lastPage)
    pageno = lastPage;

  curPage = pageno;
  XWTeXDocumentPage * pg = pages[curPage];
  pg->selectAll(minX,minY,maxX,maxY);
}

bool XWTeXDocument::selectPreviousChar(int pageno,double & minX, double & minY, double & maxX, double & maxY)
{
  if (pageno < 0)
    pageno = 0;

  if (pageno > lastPage)
    pageno = lastPage;

  curPage = pageno;
  XWTeXDocumentPage * pg = pages[curPage];
  return pg->selectPreviousChar(minX,minY,maxX,maxY);
}

bool XWTeXDocument::selectPreviousRow(int pageno,double & minX, double & minY, double & maxX, double & maxY)
{
  if (pageno < 0)
    pageno = 0;

  if (pageno > lastPage)
    pageno = lastPage;

  curPage = pageno;
  XWTeXDocumentPage * pg = pages[curPage];
  return pg->selectPreviousRow(minX,minY,maxX,maxY);
}

bool XWTeXDocument::selectPreviousWord(int pageno,double & minX, double & minY, double & maxX, double & maxY)
{
  if (pageno < 0)
    pageno = 0;

  if (pageno > lastPage)
    pageno = lastPage;

  curPage = pageno;
  XWTeXDocumentPage * pg = pages[curPage];
  return pg->selectPreviousWord(minX,minY,maxX,maxY);
}

void XWTeXDocument::selectRow(int pageno,double & minX, double & minY, double & maxX, double & maxY)
{
  if (pageno < 0)
    pageno = 0;

  if (pageno > lastPage)
    pageno = lastPage;

  curPage = pageno;
  XWTeXDocumentPage * pg = pages[curPage];
  pg->selectRow(minX,minY,maxX,maxY);
}

void XWTeXDocument::selectRowEnd(int pageno,double & minX, double & minY, double & maxX, double & maxY)
{
  if (pageno < 0)
    pageno = 0;

  if (pageno > lastPage)
    pageno = lastPage;

  curPage = pageno;
  XWTeXDocumentPage * pg = pages[curPage];
  pg->selectRowEnd(minX,minY,maxX,maxY);
}

void XWTeXDocument::selectRowStart(int pageno,double & minX, double & minY, double & maxX, double & maxY)
{
  if (pageno < 0)
    pageno = 0;

  if (pageno > lastPage)
    pageno = lastPage;

  curPage = pageno;
  XWTeXDocumentPage * pg = pages[curPage];
  pg->selectRowStart(minX,minY,maxX,maxY);
}

void XWTeXDocument::selectStartOfDocument(int pageno, double & minX, double & minY, double & maxX, double & maxY)
{
  if (pageno < 0)
    pageno = 0;

  if (pageno > lastPage)
    pageno = lastPage;

  XWTeXDocumentPage * pg = pages[pageno];
  pg->selectStart(minX,minY,maxX,maxY);
  for (int i = pageno - 1; i >= 0; i--)
  {
    pg = pages[i];
    pg->selectAll(minX,minY,maxX,maxY);
  }
}

bool XWTeXDocument::selectWord(int pageno,double & minX, double & minY, double & maxX, double & maxY)
{
  if (pageno < 0)
    pageno = 0;

  if (pageno > lastPage)
    pageno = lastPage;

  curPage = pageno;
  XWTeXDocumentPage * pg = pages[curPage];
  return pg->selectWord(minX,minY,maxX,maxY);
}

void XWTeXDocument::setCursor(double minx, double miny, double maxx,double maxy)
{
  emit cursorChanged(curPage + 1,minx,miny,maxx,maxy);
}

void XWTeXDocument::setDirection(int d)
{
  if (d != direction)
  {
    direction = d;
    breakPage();
  }
}

void XWTeXDocument::setFindCondition(const QString & str, bool casesensitivity, bool wholeword, bool regexpA)
{
  findString = str;
  findFlags = 0;
  if (casesensitivity)
	  findFlags |= QTextDocument::FindCaseSensitively;

	if (wholeword)
	  findFlags |= QTextDocument::FindWholeWords;

	isRegexpFind = regexpA;
}

void XWTeXDocument::setFileName(const QString & filename)
{
  fileName = filename;
}

void XWTeXDocument::setFontSize(int s)
{
  if (s != normalsize)
  {
    normalsize = s;
    breakPage();
  }
}

void XWTeXDocument::setInputCodec(const QString & name)
{
  inCodecName = name.toAscii();
  outCodecName = inCodecName;
  QFile file(fileName);
  if (file.open(QFile::ReadOnly | QFile::Text))
  {
    QTextStream stream(&file);
    QTextCodec *codec = QTextCodec::codecForName(inCodecName);
    stream.setCodec(codec);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QString str = stream.readAll();
    file.close();
    int pos = 0;
    int len = str.length();
    if (len > 0)
    {
      setup(str,len,pos);
      breakPage();
    }

    QApplication::restoreOverrideCursor();
  }
}

void XWTeXDocument::setItalic(bool e)
{
  italicStack.push(italic);
  italic = e;
}

void XWTeXDocument::setLineSkip(double s)
{
  if (s != lineSkip)
  {
    lineSkip = s;
    breakPage();
  }
}

void XWTeXDocument::setOutputCodec(const QString & name)
{
  outCodecName = name.toAscii();
}

void XWTeXDocument::setPaper(double w, double h)
{
  if (w != pageWidth || h != pageHeight)
  {
    pageWidth = w;
    pageHeight = h;
    savedPageWidth = pageWidth;
    savedPageHeight = pageHeight;
    breakPage();
  }
}

void XWTeXDocument::setReplaceCondition(const QString & str, const QString & bystr, 
                          bool casesensitivity, bool wholeword, 
                          bool regexpA)
{
  findString = str;
  replaceString = bystr;
  findFlags = 0;
  if (casesensitivity)
	  findFlags |= QTextDocument::FindCaseSensitively;

	if (wholeword)
	  findFlags |= QTextDocument::FindWholeWords;

	isRegexpFind = regexpA;
}

void XWTeXDocument::setSavedBottom(double b)
{
  switch (direction)
  {
    case TEX_DOC_WD_TLT:
    case TEX_DOC_WD_TRT:
      bottom = pageHeight - b;
      break;

    case TEX_DOC_WD_RTT:
      left = pageWidth - b;
      break;

    case TEX_DOC_WD_LTL:
      right = b;
      break;

    default:
      break;
  }
}

void XWTeXDocument::setSavedTop(double t)
{
  switch (direction)
  {
    case TEX_DOC_WD_TLT:
    case TEX_DOC_WD_TRT:
      top = t;
      break;

    case TEX_DOC_WD_RTT:
      right = t;
      break;

    case TEX_DOC_WD_LTL:
      left = t;
      break;

    default:
      break;
  }
}

void XWTeXDocument::setSelected(int pageno, const QRectF & rect)
{
  if (pageno < 0)
    pageno = 0;

  if (pageno > lastPage)
    pageno = lastPage;

  curPage = pageno;
  XWTeXDocumentPage * pg = pages[curPage];
  pg->setSelected(rect);
}

void XWTeXDocument::setShowPreamble(bool s)
{
  if (s != showPreamble)
  {
    showPreamble = s;
    breakPage();
  }
}

void XWTeXDocument::setTwoColumn(bool e)
{
  if (twocolumn != e)
  {
    twocolumn = e;
    breakPage();
  }
}

void XWTeXDocument::small()
{
  sizeStack.push(currentSize);

  switch (normalsize)
  {
    case 10:
      currentSize = 11;
      break;

    case 11:
      currentSize = 12;
      break;

    default:
      currentSize = 14;
      break;
  }
}

void XWTeXDocument::tiny()
{
  sizeStack.push(currentSize);
  switch (normalsize)
  {
    case 10:
      currentSize = 6;
      break;

    case 11:
      currentSize = 7;
      break;

    default:
      currentSize = 8;
      break;
  }
}

void XWTeXDocument::undo()
{
  undoStack->undo();
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWTeXDocument::formular()
{
  if (!curCursor)
    return ;

  XWTeXDocumentBlock * block = curCursor->getBlock();
  XWTeXDocumentObject * pobj = block->getObject();  
  QUndoCommand * cmd = 0;
  if (pobj->type() == TEX_DOC_B_TEXT)
  {
    XWTeXDocumentFormular * obj = new XWTeXDocumentFormular(this,pobj->parent());
    if (!block->hasSelected())
      cmd = new XWTeXDocumentNewObject(obj);
    else
    {
      if (block->isAllSelected())
      {
        obj->text = block->text;
        cmd = new QUndoCommand;
        new XWTeXDocumentNewObject(obj,cmd);
        new XWTeXDocumentRemoveObject(pobj,cmd);
      }
      else
      {
        obj->text = block->getSelected();
        cmd = new QUndoCommand;        
        new XWTeXDocumentDeleteString(block);
        new XWTeXDocumentNewObject(obj,cmd);
      }
    }
  }
  else if (block->type() == TEX_DOC_B_DISFOMULAR)
  {
    XWTeXDocumentFormular * obj = new XWTeXDocumentFormular(this,pobj->parent());
    obj->text = block->text;
    cmd = new QUndoCommand;
    new XWTeXDocumentNewObject(obj,cmd);
    new XWTeXDocumentRemoveObject(pobj,cmd);    
  }

  if (cmd)
  {
    undoStack->push(cmd);
    breakPage();
    isModified = true;
    emit modificationChanged(isModified);
  }
}

void XWTeXDocument::displayFormular()
{
  if (!curCursor)
    return ;

  XWTeXDocumentBlock * block = curCursor->getBlock();  
  XWTeXDocumentObject * pobj = block->getObject();
  QUndoCommand * cmd = 0;
  if (block->type() == TEX_DOC_B_TEXT)
  {
    XWTeXDocumentDisplayFormular * obj = new XWTeXDocumentDisplayFormular(this,pobj->parent());
    if (!block->hasSelected())
      cmd = new XWTeXDocumentNewObject(obj);
    else
    {
      if (block->isAllSelected())
      {
        obj->text = block->text;
        cmd = new QUndoCommand;
        new XWTeXDocumentNewObject(obj,cmd);
        new XWTeXDocumentRemoveObject(pobj,cmd);
      }
      else
      {
        obj->text = block->getSelected();
        cmd = new QUndoCommand;
        new XWTeXDocumentDeleteString(block);
        new XWTeXDocumentNewObject(obj,cmd);        
      }
    }
  }
  else if (block->type() == TEX_DOC_B_FOMULAR)
  {
    XWTeXDocumentDisplayFormular * obj = new XWTeXDocumentDisplayFormular(this,pobj->parent());
    obj->text = block->text;
    cmd = new QUndoCommand;
    new XWTeXDocumentNewObject(obj,cmd);
    new XWTeXDocumentRemoveObject(pobj,cmd);
  }

  if (cmd)
  {
    undoStack->push(cmd);
    breakPage();
    isModified = true;
    emit modificationChanged(isModified);
  }
}

void XWTeXDocument::text()
{
  if (!curCursor)
    return ;

  XWTeXDocumentBlock * block = curCursor->getBlock();
  if (block->type() == TEX_DOC_B_FOMULAR ||
      block->type() == TEX_DOC_B_DISFOMULAR)
  {
    XWTeXDocumentText * obj = new XWTeXDocumentText(this,block->parent());
    obj->text = block->text;
    QUndoCommand * cmd = new QUndoCommand;
    new XWTeXDocumentNewObject(obj,cmd);
    new XWTeXDocumentRemoveObject(block,cmd);
    undoStack->push(cmd);
    breakPage();
    isModified = true;
    emit modificationChanged(isModified);
  }
}

void XWTeXDocument::pstricksConnection()
{
  XWPSTricksConnectionDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString c = dlg.getConnection();
  if (c.isEmpty())
    return ;

  insertText(c);
}

void XWTeXDocument::pstricksLabels()
{
  XWPSTricksLabelDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString l = dlg.getLabel();
  if (l.isEmpty())
    return ;

  insertText(l);
}

void XWTeXDocument::pstricksNode()
{
  XWPSTricksNodeDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString n = dlg.getNode();
  if (n.isEmpty())
    return ;

  insertText(n);
}

void XWTeXDocument::pstricksObject()
{
  XWPSTricksObjectDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString o = dlg.getObject();
  if (o.isEmpty())
    return ;

  insertText(o);
}

void XWTeXDocument::pstricksOptionArc()
{
  XWPSTricksOpArcDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString o = dlg.getArc();
  if (o.isEmpty())
    return ;

  insertText(o);
}

void XWTeXDocument::pstricksOptionArrow()
{
  XWPSTricksOpArrowleDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString o = dlg.getArrow();
  if (o.isEmpty())
    return ;

  insertText(o);
}

void XWTeXDocument::pstricksOptionCoordSystem()
{
  XWPSTricksOpCoordSystemDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString o = dlg.getCoordSystem();
  if (o.isEmpty())
    return ;

  insertText(o);
}

void XWTeXDocument::pstricksOptionDot()
{
  XWPSTricksOpDotDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString o = dlg.getDot();
  if (o.isEmpty())
    return ;

  insertText(o);
}

void XWTeXDocument::pstrickOptionFill()
{
  XWPSTricksOpFillStyleDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString o = dlg.getStyle();
  if (o.isEmpty())
    return ;

  insertText(o);
}

void XWTeXDocument::pstricksOptionGrid()
{
  XWPSTricksOpGridDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString o = dlg.getGrid();
  if (o.isEmpty())
    return ;

  insertText(o);
}

void XWTeXDocument::pstricksOptionLine()
{
  XWPSTricksOpLineStyleDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString o = dlg.getStyle();
  if (o.isEmpty())
    return ;

  insertText(o);
}

void XWTeXDocument::pstricksOptionMatrix()
{
  XWPSTricksOpMatrixDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString o = dlg.getMatrixOption();
  if (o.isEmpty())
    return ;

  insertText(o);
}

void XWTeXDocument::pstricksOptionMisc()
{
  XWPSTricksOpMiscDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString o = dlg.getMisc();
  if (o.isEmpty())
    return ;

  insertText(o);
}

void XWTeXDocument::pstrickOptionNode()
{
  XWPSTricksOpNodeDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString o = dlg.getNodeOption();
  if (o.isEmpty())
    return ;

  insertText(o);
}

void XWTeXDocument::pstricksOptionPlot()
{
  XWPSTricksOpPlotDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString o = dlg.getPlot();
  if (o.isEmpty())
    return ;

  insertText(o);
}

void XWTeXDocument::pstricksOptionTree()
{
  XWPSTricksOpTreeDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString o = dlg.getTreeOption();
  if (o.isEmpty())
    return ;

  insertText(o);
}

void XWTeXDocument::pstrickPut()
{
  XWPSTricksPutDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString p = dlg.getPut();
  if (p.isEmpty())
    return ;

  insertText(p);
}

void XWTeXDocument::pstricksRepetion()
{
  XWPSTricksRepetionDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString r = dlg.getRepetion();
  if (r.isEmpty())
    return ;

  insertText(r);
}

void XWTeXDocument::pstricksTextBox()
{
  XWPSTricksTextBoxDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString t = dlg.getBox();
  if (t.isEmpty())
    return ;

  insertText(t);
}

void XWTeXDocument::pstricksTreeNode()
{
  XWPSTricksTreeNodeDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString t = dlg.getNode();
  if (t.isEmpty())
    return ;

  insertText(t);
}

void XWTeXDocument::tikzExecuteAt()
{
  XWTikzExecuteDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString ex = dlg.getExecute();
  if (ex.isEmpty())
    return ;

  insertText(ex);
}

void XWTeXDocument::tikzOptionAutomata()
{
  XWTikzAutomataDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString b = dlg.getAutomataOption();
  if (b.isEmpty())
    return ;

  insertText(b);
}

void XWTeXDocument::tikzOptionBackground()
{
  XWTikzBackgroundDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString b = dlg.getBackground();
  if (b.isEmpty())
    return ;

  insertText(b);
}

void XWTeXDocument::tikzOptionCalendar()
{
  XWTikzCalendarDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString c = dlg.getCalendarOption();
  if (c.isEmpty())
    return ;

  insertText(c);
}

void XWTeXDocument::tikzOptionChain()
{
  XWTikzChainDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString c = dlg.getChainOption();
  if (c.isEmpty())
    return ;

  insertText(c);
}

void XWTeXDocument::tikzOptionCircuit()
{
  XWTikzCircuitDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString c = dlg.getCircuitOption();
  if (c.isEmpty())
    return ;

  insertText(c);
}

void XWTeXDocument::tikzOptionCircuitElectrical()
{
  XWTikzCircuitElectricalDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString c = dlg.getCircuitElectricalOption();
  if (c.isEmpty())
    return ;

  insertText(c);
}

void XWTeXDocument::tikzOptionCircuitLogic()
{
  XWTikzCircuitLogicDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
    return ;

  QString c = dlg.getCircuitLogicOption();
  if (c.isEmpty())
    return ;

  insertText(c);
}

void XWTeXDocument::tikzOptionDecorate()
{
  XWTikzDecorateDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString d = dlg.getDecorate();
  if (d.isEmpty())
    return ;

  insertText(d);
}

void XWTeXDocument::tikzOptionFill()
{
  XWTikzFillDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString f = dlg.getFill();
  if (f.isEmpty())
    return ;

  insertText(f);
}

void XWTeXDocument::tikzOptionFit()
{
  XWTikzFitDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString f = dlg.getFit();
  if (f.isEmpty())
    return ;

  insertText(f);
}

void XWTeXDocument::tikzOptionLine()
{
  XWTikzLineDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString l = dlg.getLine();
  if (l.isEmpty())
    return ;

  insertText(l);
}

void XWTeXDocument::tikzOptionLSystem()
{
  XWTikzLSystemDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString l = dlg.getLSystemOption();
  if (l.isEmpty())
    return ;

  insertText(l);
}

void XWTeXDocument::tikzOptionMatrix()
{
  XWTikzMatrixDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString m = dlg.getMatrixOption();
  if (m.isEmpty())
    return ;

  insertText(m);
}

void XWTeXDocument::tikzOptionMindmap()
{
  XWTikzMindmapDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString m = dlg.getMindmapOption();
  if (m.isEmpty())
    return ;

  insertText(m);
}

void XWTeXDocument::tikzOptionNode()
{
  XWTikzNodeDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString n = dlg.getNodeOption();
  if (n.isEmpty())
    return ;

  insertText(n);
}

void XWTeXDocument::tikzOptionNodeShape()
{
  XWTikzNodeShapeDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString n = dlg.getShape();
  if (n.isEmpty())
    return ;

  insertText(n);
}

void XWTeXDocument::tikzOptionPath()
{
  XWTikzPathDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString p = dlg.getPathOption();
  if (p.isEmpty())
    return ;

  insertText(p);
}

void XWTeXDocument::tikzOptionPetriNet()
{
  XWTikzPetrinetDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString p = dlg.getPetrinetOption();
  if (p.isEmpty())
    return ;

  insertText(p);
}

void XWTeXDocument::tikzOptionPlot()
{
  XWTikzPlotDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString p = dlg.getPlotOption();
  if (p.isEmpty())
    return ;

  insertText(p);
}

void XWTeXDocument::tikzOptionShadow()
{
  XWTikzShadowDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString s = dlg.getShadow();
  if (s.isEmpty())
    return ;

  insertText(s);
}

void XWTeXDocument::tikzOptionShape()
{
  XWTikzShapeDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString s = dlg.getShapeOption();
  if (s.isEmpty())
    return ;

  insertText(s);
}

void XWTeXDocument::tikzOptionSpy()
{
  XWTikzSpyDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString s = dlg.getSpyOption();
  if (s.isEmpty())
    return ;

  insertText(s);
}

void XWTeXDocument::tikzOptionTo()
{
  XWTikzToDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString t = dlg.getTo();
  if (t.isEmpty())
    return ;

  insertText(t);
}

void XWTeXDocument::tikzOptionTransform()
{
  XWTikzTransformDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString t = dlg.getTransform();
  if (t.isEmpty())
    return ;

  insertText(t);
}

void XWTeXDocument::tikzOptionThrough()
{
  XWTikzThroughDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString t = dlg.getThrough();
  if (t.isEmpty())
    return ;

  insertText(t);
}

void XWTeXDocument::tikzOptionTree()
{
  XWTikzTreeDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString t = dlg.getTreeOption();
  if (t.isEmpty())
    return ;

  insertText(t);
}

void XWTeXDocument::tikzOptionTurtle()
{
  XWTikzTurtleDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString t = dlg.getTurtleOption();
  if (t.isEmpty())
    return ;

  insertText(t);
}

void XWTeXDocument::tikzPathAction()
{
  XWTikzPathActionDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString act = dlg.getAction();
  if (act.isEmpty())
    return ;

  insertText(act);
}

void XWTeXDocument::tikzPathOperate()
{
  XWTikzPathOperationDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString op = dlg.getOperation();
  if (op.isEmpty())
    return ;

  insertText(op);
}

void XWTeXDocument::tikzStyleGate()
{
  XWTikzGateStyleDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString style = dlg.getStyle();
  if (style.isEmpty())
    return ;

  insertText(style);
}

void XWTeXDocument::tikzStyleGeneral()
{
  XWTikzGeneralStyleDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString style = dlg.getStyle();
  if (style.isEmpty())
    return ;

  insertText(style);
}

void XWTeXDocument::tikzStyleShape()
{
  XWTikzShapeStyleDialog dlg;
  if (dlg.exec() != QDialog::Accepted)
		return ;

  QString style = dlg.getStyle();
  if (style.isEmpty())
    return ;

  insertText(style);
}

void XWTeXDocument::clearCursors()
{
  QHash<XWTeXDocumentBlock*, XWTeXDocumentCursor*>::iterator i = cursors.begin();
  while (i != cursors.end())
	{
		XWTeXDocumentCursor * c = i.value();
		if (c)
			delete c;
		i = cursors.erase(i);
	}
}

void XWTeXDocument::clearPages()
{
  if (pages)
  {
    for (int i = 0; i < maxPageNum; i++)
    {
      if (pages[i])
      {
        delete pages[i];
        pages[i] = 0;
      }
    }

    free(pages);
    pages = 0;
  }

  maxPageNum = 0;
  lastPage = -1;
}

void XWTeXDocument::clearSelected()
{
  selected.clear();
  selectedObjs.clear();
}

void XWTeXDocument::createPSTricksMenu(QMenu * menu)
{
  QAction * act = menu->addAction(tr("object"));
  connect(act, SIGNAL(triggered()), this, SLOT(pstricksObject()));

  act = menu->addAction(tr("text box"));
  connect(act, SIGNAL(triggered()), this, SLOT(pstricksTextBox()));

  menu->addSeparator();

  act = menu->addAction(tr("node"));
  connect(act, SIGNAL(triggered()), this, SLOT(pstricksNode()));

  act = menu->addAction(tr("connection"));
  connect(act, SIGNAL(triggered()), this, SLOT(pstricksConnection()));

  act = menu->addAction(tr("label"));
  connect(act, SIGNAL(triggered()), this, SLOT(pstricksLabels()));

  act = menu->addAction(tr("tree node"));
  connect(act, SIGNAL(triggered()), this, SLOT(pstricksTreeNode()));

  menu->addSeparator();

  act = menu->addAction(tr("repetion"));
  connect(act, SIGNAL(triggered()), this, SLOT(pstricksRepetion()));

  menu->addSeparator();

  act = menu->addAction(tr("put"));
  connect(act, SIGNAL(triggered()), this, SLOT(pstrickPut()));
}

void XWTeXDocument::createPSTricksOptionMenu(QMenu * menu)
{
  QAction * act = menu->addAction(tr("coordinate system"));
  connect(act, SIGNAL(triggered()), this, SLOT(pstricksOptionCoordSystem()));

  act = menu->addAction(tr("line"));
  connect(act, SIGNAL(triggered()), this, SLOT(pstricksOptionLine()));

  act = menu->addAction(tr("fill"));
  connect(act, SIGNAL(triggered()), this, SLOT(pstrickOptionFill()));

  act = menu->addAction(tr("arrow"));
  connect(act, SIGNAL(triggered()), this, SLOT(pstricksOptionArrow()));

  menu->addSeparator();

  act = menu->addAction(tr("arc"));
  connect(act, SIGNAL(triggered()), this, SLOT(pstricksOptionArc()));

  act = menu->addAction(tr("dot"));
  connect(act, SIGNAL(triggered()), this, SLOT(pstricksOptionDot()));

  act = menu->addAction(tr("grid"));
  connect(act, SIGNAL(triggered()), this, SLOT(pstricksOptionGrid()));

  menu->addSeparator();

  act = menu->addAction(tr("miscellany"));
  connect(act, SIGNAL(triggered()), this, SLOT(pstricksOptionMisc()));

  act = menu->addAction(tr("plot"));
  connect(act, SIGNAL(triggered()), this, SLOT(pstricksOptionPlot()));

  menu->addSeparator();

  act = menu->addAction(tr("node"));
  connect(act, SIGNAL(triggered()), this, SLOT(pstrickOptionNode()));

  act = menu->addAction(tr("tree"));
  connect(act, SIGNAL(triggered()), this, SLOT(pstricksOptionTree()));

  act = menu->addAction(tr("matrix"));
  connect(act, SIGNAL(triggered()), this, SLOT(pstricksOptionMatrix()));
}

void XWTeXDocument::createTikzPathMenu(QMenu * menu)
{
  QAction * act = menu->addAction(tr("path action"));
  connect(act, SIGNAL(triggered()), this, SLOT(tikzPathAction()));

  act = menu->addAction(tr("path operator"));
  connect(act, SIGNAL(triggered()), this, SLOT(tikzPathOperate()));
}

void XWTeXDocument::createTikzOptionMenu(QMenu * menu)
{
  QMenu * smenu = menu->addMenu(tr("style"));
  QAction * act = smenu->addAction(tr("every"));
  connect(act, SIGNAL(triggered()), this, SLOT(tikzStyleGeneral()));
  act = smenu->addAction(tr("shape"));
  connect(act, SIGNAL(triggered()), this, SLOT(tikzStyleShape()));
  act = smenu->addAction(tr("gate"));
  connect(act, SIGNAL(triggered()), this, SLOT(tikzStyleGate()));

  act = menu->addAction(tr("execute at"));
  connect(act, SIGNAL(triggered()), this, SLOT(tikzExecuteAt()));

  menu->addSeparator();

  smenu = menu->addMenu(tr("path option"));

  act = smenu->addAction(tr("transform"));
  connect(act, SIGNAL(triggered()), this, SLOT(tikzOptionTransform()));

  act = smenu->addAction(tr("line"));
  connect(act, SIGNAL(triggered()), this, SLOT(tikzOptionLine()));

  act = smenu->addAction(tr("fill"));
  connect(act, SIGNAL(triggered()), this, SLOT(tikzOptionFill()));

  act = smenu->addAction(tr("path"));
  connect(act, SIGNAL(triggered()), this, SLOT(tikzOptionPath()));

  smenu->addSeparator();

  act = smenu->addAction(tr("fit"));
  connect(act, SIGNAL(triggered()), this, SLOT(tikzOptionFit()));

  act = smenu->addAction(tr("to"));
  connect(act, SIGNAL(triggered()), this, SLOT(tikzOptionTo()));

  act = smenu->addAction(tr("through"));
  connect(act, SIGNAL(triggered()), this, SLOT(tikzOptionThrough()));

  smenu->addSeparator();

  act = smenu->addAction(tr("decorate"));
  connect(act, SIGNAL(triggered()), this, SLOT(tikzOptionDecorate()));

  act = smenu->addAction(tr("shadow"));
  connect(act, SIGNAL(triggered()), this, SLOT(tikzOptionShadow()));

  act = smenu->addAction(tr("background"));
  connect(act, SIGNAL(triggered()), this, SLOT(tikzOptionBackground()));

  smenu->addSeparator();

  act = smenu->addAction(tr("node"));
  connect(act, SIGNAL(triggered()), this, SLOT(tikzOptionNode()));

  act = smenu->addAction(tr("shape"));
  connect(act, SIGNAL(triggered()), this, SLOT(tikzOptionNodeShape()));

  act = smenu->addAction(tr("node shape"));
  connect(act, SIGNAL(triggered()), this, SLOT(tikzOptionNodeShape()));

  act = smenu->addAction(tr("shape"));
  connect(act, SIGNAL(triggered()), this, SLOT(tikzOptionShape()));

  act = smenu->addAction(tr("tree"));
  connect(act, SIGNAL(triggered()), this, SLOT(tikzOptionTree()));

  act = smenu->addAction(tr("matrix"));
  connect(act, SIGNAL(triggered()), this, SLOT(tikzOptionMatrix()));

  menu->addSeparator();

  smenu = menu->addMenu(tr("general option"));
  act = smenu->addAction(tr("calendar"));
  connect(act, SIGNAL(triggered()), this, SLOT(tikzOptionCalendar()));

  act = smenu->addAction(tr("mindmap"));
  connect(act, SIGNAL(triggered()), this, SLOT(tikzOptionMindmap()));

  act = smenu->addAction(tr("chain"));
  connect(act, SIGNAL(triggered()), this, SLOT(tikzOptionChain()));

  act = smenu->addAction(tr("spy"));
  connect(act, SIGNAL(triggered()), this, SLOT(tikzOptionSpy()));

  act = smenu->addAction(tr("turtle"));
  connect(act, SIGNAL(triggered()), this, SLOT(tikzOptionTurtle()));

  act = smenu->addAction(tr("plot"));
  connect(act, SIGNAL(triggered()), this, SLOT(tikzOptionPlot()));

  act = smenu->addAction(tr("lindenmayer system"));
  connect(act, SIGNAL(triggered()), this, SLOT(tikzOptionLSystem()));

  menu->addSeparator();

  smenu = menu->addMenu(tr("computer option"));
  act = smenu->addAction(tr("automata"));
  connect(act, SIGNAL(triggered()), this, SLOT(tikzOptionAutomata()));

  act = smenu->addAction(tr("petri-net"));
  connect(act, SIGNAL(triggered()), this, SLOT(tikzOptionPetriNet()));

  menu->addSeparator();
  smenu = menu->addMenu(tr("circuit option"));
  act = smenu->addAction(tr("common"));
  connect(act, SIGNAL(triggered()), this, SLOT(tikzOptionCircuit()));

  act = smenu->addAction(tr("logic"));
  connect(act, SIGNAL(triggered()), this, SLOT(tikzOptionCircuitLogic()));

  act = smenu->addAction(tr("electrical"));
  connect(act, SIGNAL(triggered()), this, SLOT(tikzOptionCircuitElectrical()));
}

void XWTeXDocument::pageClear()
{
  if (maxPageNum > 0)
  {
    for (int i = 0; i < maxPageNum; i++)
    {
      if (pages[i])
        pages[i]->clear();
    }
  }
}
