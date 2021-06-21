/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QApplication>
#include <QClipboard>
#include "XWFontSea.h"
#include "XWFontFileID.h"
#include "XWFTFontFile.h"
#include "XWFTFontEngine.h"
#include "XWFTFont.h"
#include "LaTeXKeyWord.h"
#include "XWLaTeXPool.h"
#include "XWPDFDriver.h"
#include "XWTeXText.h"
#include "XWTeXTextBox.h"

XWTeXTextRow::XWTeXTextRow()
:startPos(0),
 endPos(0),
 x(0.0),
 y(0.0),
 width(0.0),
 height(0.0),
 depth(0.0)
{}

void XWTeXTextRow::add(XWTeXText * obj)
{
  objs << obj;
}

void XWTeXTextRow::clear()
{
  x = 0.0;
  y = 0.0;
  width = 0.0;
  height = 0.0;
  depth = 0.0;
  objs.clear();
}

bool XWTeXTextRow::contains(double xA, double yA)
{
  return (xA >= x && xA < (x + width) && yA <= (y + height) && yA >= (y - depth));
}

void XWTeXTextRow::doContent(XWPDFDriver * driver)
{
  if (objs.size() == 1)
    objs[0]->doText(driver,x,y,startPos,endPos);
  else if (objs.size() == 2)
  {
    objs[0]->doText(driver,x,y,startPos,objs[0]->text.length());
    objs[1]->doText(driver,x,y,0,endPos);
  }
  else
  {
    objs[0]->doText(driver,x,y,startPos,objs[0]->text.length());
    for (int i = 1;  i < (objs.size() - 1); i++)
      objs[i]->doText(driver,x,y,0,objs[i]->text.length());
    objs[objs.size() - 1]->doText(driver,x,y,0,endPos);
  }
}

void XWTeXTextRow::draw(QPainter * painter)
{
  for (int i = 0; i < objs.size(); i++)
    objs[i]->draw(painter);
}

void XWTeXTextRow::draw(QPainter * painter,const QRectF & r)
{
  QRectF rr(x,y - height,width,height);
  if (rr.intersects(r))
  {
    for (int i = 0; i < objs.size(); i++)
      objs[i]->draw(painter,r);
  }
}

XWTeXText * XWTeXTextRow::getFirst()
{
  if (objs.size() > 0)
    return objs[0];

  return 0;
}

XWTeXText * XWTeXTextRow::getLast()
{
  if (objs.size() > 0)
    return objs[objs.size() - 1];

  return 0;
}

XWTeXText * XWTeXTextRow::getObject(double xA, double yA)
{
  if (xA >= x && xA < (x + width) && yA <= (y + height) && yA >= (y - depth))
  {
    for (int i = 0; i < objs.size(); i++)
    {
      if (objs[i]->contains(xA,yA))
        return objs[i];
    }
  }

  return 0;
}

bool XWTeXTextRow::hitTest(double xA, double yA)
{
  if (xA >= x && xA < (x + width) && yA <= (y + height) && yA >= (y - depth))
  {
    if (objs.size() == 1)
      return objs[0]->hitTest(xA,yA,x,y,startPos,endPos);
    
    if (objs.size() == 2)
    {
      if (objs[0]->hitTest(xA,yA,x,y,startPos,objs[0]->text.length()))
        return true;
      return objs[1]->hitTest(xA,yA,x,y,0,endPos);
    }
    
    if (objs[0]->hitTest(xA,yA,x,y,startPos,objs[0]->text.length()))
      return true;

    for (int i = 1;  i < (objs.size() - 1); i++)
    {
      if (objs[i]->hitTest(xA,yA,x,y,0,objs[i]->text.length()))
        return true;
    }
      
    return objs[objs.size() - 1]->hitTest(xA,yA,x,y,0,endPos);
  }

  return false;
}

void XWTeXTextRow::setXY(double & xA, double yA)
{
  x = xA;
  y = yA;
  for (int i = 0; i < objs.size(); i++)
    objs[i]->setXY(xA,yA);
}

XWTeXTextBoxPart::XWTeXTextBoxPart(XWTeXTextBox * boxA,QObject * parent)
:QObject(parent),
box(boxA),
maxRow(0),
lastRow(-1),
curRow(-1),
rows(0),
curObj(0),
head(0),
last(0)
{}

XWTeXTextBoxPart::~XWTeXTextBoxPart()
{
  curObj = head;
  while (curObj)
  {
    XWTeXText * tmp = curObj->next;
    delete curObj;
    curObj = tmp;
  }

  if (rows)
  {
    for (int i = 0; i < maxRow; i++)
    {
      delete rows[i];
      rows[i] = 0;
    }

    free(rows);
    rows = 0;
  }
}

void XWTeXTextBoxPart::append(XWTeXText*obj)
{
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

  curObj = obj;
  if (curObj)
  {
    if (curObj->keyWord == XW_TEX_LETTER || curObj->keyWord == XW_TEX_DIGIT)
      textPos = curObj->text.length();
    else
      textPos = XW_TEX_POS_AFTER;
  }
}

bool XWTeXTextBoxPart::back()
{
  bool ret = false;
  if (!curObj)
    return ret;

  switch (curObj->keyWord)
  {
    case XW_TEX_LETTER:
    case XW_TEX_DIGIT:
    case XW_TEX_UNKNOWN:
    case XW_TEX_TEXT:
      if (textPos > 0)
      {
        XWTeXDeleteChar * cmd = new XWTeXDeleteChar(curObj,textPos - 1);
        box->push(cmd);
        ret = true;
      }
      else
      {
        if (curObj->text.length() <= 0)
        {
          if (curObj->prev || curObj->next)
          {
            XWTeXRemove * cmd = new XWTeXRemove(curObj);
            box->push(cmd);     
            ret = true;
          }
          else
          {
            if (!curObj->parent())
            {
              XWTeXRemove * cmd = new XWTeXRemove(curObj);
              box->push(cmd);
              ret = true;
            }
            else
            {
              XWTeXText * pobj = (XWTeXText*)(curObj->parent());
              if (pobj->keyWord == XW_TEX_SUP || pobj->keyWord == XW_TEX_SUB )
              {
                XWTeXRemove * cmd = new XWTeXRemove(pobj);
                box->push(cmd);
                ret = true;
              }
              else if (pobj->keyWord == XW_TEX_PARAM)
              {
                XWTeXText * ppobj = (XWTeXText*)(pobj->parent());
                if (ppobj->keyWord == XW_TEX_SUP || ppobj->keyWord == XW_TEX_SUB )
                {
                  XWTeXRemove * cmd = new XWTeXRemove(ppobj);
                  box->push(cmd);
                  ret = true;
                }
              }
              else
              {
                XWTeXRemove * cmd = new XWTeXRemove(curObj);
                box->push(cmd);
                ret = true;
              }
            }            
          }             
        }
        else
        {
          if (curObj->prev)
          {
            XWTeXRemove * cmd = new XWTeXRemove(curObj->prev);
            box->push(cmd);
            ret = true;
          }
        }        
      }      
      break;


    default:
      if (textPos == XW_TEX_POS_AFTER)
      {
        if (curObj->prev || curObj->next)
        {
          if (curObj->keyWord > XW_TEX_COL)
          {
            XWTeXRemove * cmd = new XWTeXRemove(curObj);
            box->push(cmd);
            ret = true;
          }
        }
        else
        {
          if (!curObj->parent())
          {
            XWTeXRemove * cmd = new XWTeXRemove(curObj);
            box->push(cmd);
            ret = true;
          }
          else
          {
            XWTeXText * pobj = (XWTeXText*)(curObj->parent());
            if (pobj->keyWord == XW_TEX_SUP || pobj->keyWord == XW_TEX_SUB )
            {
              XWTeXRemove * cmd = new XWTeXRemove(pobj);
              box->push(cmd);
              ret = true;
            }
            else if (pobj->keyWord == XW_TEX_PARAM)
            {
              XWTeXText * ppobj = (XWTeXText*)(pobj->parent());
              if (ppobj->keyWord == XW_TEX_SUP || ppobj->keyWord == XW_TEX_SUB )
              {
                XWTeXRemove * cmd = new XWTeXRemove(ppobj);
                box->push(cmd);
                ret = true;
              }
            }
            else
            {
              if (curObj->keyWord > XW_TEX_COL)
              {
                XWTeXRemove * cmd = new XWTeXRemove(curObj);
                box->push(cmd);
                ret = true;
              }
            }
          }            
        }    
      }
      else if (curObj->prev) 
      {
        if (curObj->prev->keyWord > XW_TEX_COL)
        {
          XWTeXRemove * cmd = new XWTeXRemove(curObj->prev);
          box->push(cmd);
          ret = true;
        }        
      }
      break;
  }

  return ret;
}

bool XWTeXTextBoxPart::contains(double xA, double yA)
{
  for (int i = 0; i <= lastRow; i++)
  {
    if (rows[i]->contains(xA,yA))
      return true;
  }

  return false;
}

bool XWTeXTextBoxPart::cut()
{
  if (!curObj)
    return false;

  return curObj->cut();
}

bool XWTeXTextBoxPart::del()
{
  if (!curObj)
    return false;

  bool ret = false;
  switch (curObj->keyWord)
  {
    case XW_TEX_LETTER:
    case XW_TEX_DIGIT:
    case XW_TEX_UNKNOWN:
    case XW_TEX_TEXT:
      if (textPos < curObj->text.length())
      {
        XWTeXDeleteChar * cmd = new XWTeXDeleteChar(curObj,textPos);
        box->push(cmd);
        ret = true;
      }
      else if (curObj->next)
      {
        XWTeXRemove * cmd = new XWTeXRemove(curObj->next);
        box->push(cmd);
        ret = true;
      }      
      break;

    default:
      if (textPos == XW_TEX_POS_BEFORE)
      {
        if (curObj->keyWord > XW_TEX_COL)
        {
          XWTeXRemove * cmd = new XWTeXRemove(curObj);
          box->push(cmd);
          ret = true;
        }
      }
      else if (curObj->next)
      {
        if (curObj->next->keyWord > XW_TEX_COL)
        {
          XWTeXRemove * cmd = new XWTeXRemove(curObj->next);
          box->push(cmd);
          ret = true;
        }        
      } 
      break;
  }

  return ret;
}

QChar XWTeXTextBoxPart::deleteChar(int pos)
{
  QChar ch;
  if (curObj)
  {
    ch = curObj->deleteChar(pos);
    textPos = pos;
  }    

  return ch;
}

void XWTeXTextBoxPart::doContent(XWPDFDriver * driver)
{
  for (int i = 0; i <= lastRow; i++)
    rows[i]->doContent(driver);
}

void XWTeXTextBoxPart::draw(QPainter * painter,const QRectF & r)
{
  if (lastRow < 0)
    return ;

  for (int i = 0; i <= lastRow; i++)
    rows[i]->draw(painter,r);
}

void XWTeXTextBoxPart::draw(QPainter * painter)
{
  for (int i = 0; i <= lastRow; i++)
    rows[i]->draw(painter);
}

void XWTeXTextBoxPart::dragTo(XWPDFDriver * driver, double xA, double yA)
{
  if (!curObj)
    return ;

  curObj->dragTo(driver, xA,yA);
}

void XWTeXTextBoxPart::dragTo(QPainter * painter, double xA, double yA)
{
  if (!curObj)
    return ;

  curObj->dragTo(painter, xA,yA);
}

bool XWTeXTextBoxPart::dropTo(double xA,double yA)
{
  if (!curObj)
    return false;

  if (curObj->keyWord != XW_TEX_LETTER && 
       curObj->keyWord != XW_TEX_TEXT &&
       curObj->keyWord != XW_TEX_DIGIT)
    return false;

  if (curObj->contains(xA,yA))
    curObj->dropTo(xA,yA);
  else
  {
    if (!curObj->hasSelect())
      return false;

    XWTeXText * obj = getObject(xA,yA);
    if (!obj)
      return false;

    if (obj->keyWord != XW_TEX_LETTER && 
       obj->keyWord != XW_TEX_TEXT &&
       obj->keyWord != XW_TEX_DIGIT)
      return false;

    int i = obj->getTextPosition(xA,yA);
    if (i < 0)
      return false;

    QString str = curObj->getSelectedText();
    int sp = curObj->getTextPosition();
    int ep = sp + str.length();
    QUndoCommand * cmd = new QUndoCommand;
    new XWTeXRemoveString(curObj,sp,ep,cmd);
    new XWTeXInsertString(obj,i,str,cmd); 
    box->push(cmd);
  }
  
  return true;
}

int XWTeXTextBoxPart::getAnchorPosition()
{
  if (!curObj)
    return 0;

  return curObj->getAnchorPosition();
}

XWTeXText * XWTeXTextBoxPart::getCurrent()
{
  return curObj;
}

XWTeXTextRow * XWTeXTextBoxPart::getCurrentRow()
{
  if (maxRow <= 0)
  {
    maxRow = 5;
    rows = (XWTeXTextRow**)realloc(rows, maxRow * sizeof(XWTeXTextRow*));
    for (int i = 0; i < maxRow; i++)
      rows[i] = new XWTeXTextRow;
  }

  if (curRow < 0)
    curRow = 0;

  lastRow = curRow;
  return rows[lastRow];
}

QString XWTeXTextBoxPart::getCurrentText()
{
  if (curObj)
    return curObj->getCurrentText();

  return QString();
}

double XWTeXTextBoxPart::getDepth(double maxw)
{
  if (lastRow < 0)
  {
    XWTeXText * obj = head;
    while (obj)
    {
      obj->getDimension();
      obj = obj->next;
    }
    breakLine(maxw);
  }

  if (lastRow < 0)
    return 0.0;

  return rows[lastRow]->depth;
}

double XWTeXTextBoxPart::getHeight(double maxw)
{
  if (lastRow < 0)
  {
    XWTeXText * obj = head;
    while (obj)
    {
      obj->getDimension();
      obj = obj->next;
    }
    breakLine(maxw);
  }

  double h = 0.0;
  for (int i = 0; i <= lastRow;i++)
    h += rows[i]->height;

  return h;
}

XWTeXTextRow * XWTeXTextBoxPart::getNewRow()
{
  XWTeXTextRow * row = 0;
  if (maxRow <= 0)
  {
    maxRow = 5;
    rows = (XWTeXTextRow**)realloc(rows, maxRow * sizeof(XWTeXTextRow*));
    for (int i = 0; i < maxRow; i++)
      rows[i] = new XWTeXTextRow;

    if (curRow < 0)
      curRow = 0;

    lastRow = curRow;
    row = rows[lastRow];
  }
  else
  {
    lastRow = ++curRow;
    if (curRow >= maxRow)
    {
      maxRow += 5;
      rows = (XWTeXTextRow**)realloc(rows, maxRow * sizeof(XWTeXTextRow*));
      for (int i = curRow; i < maxRow; i++)
        rows[i] = new XWTeXTextRow;
    }
    row = rows[lastRow];
    row->clear();
  }

  return row;
}

XWTeXText * XWTeXTextBoxPart::getObject(double xA, double yA)
{
  XWTeXText * obj = head;
  while (obj)
  {
    if (obj->contains(xA,yA))
      return obj;

    obj = obj->next;
  }

  return 0;
}

QString XWTeXTextBoxPart::getSelected()
{
  return getSelectedText();
}

QString XWTeXTextBoxPart::getSelectedText()
{
  if (curObj)
    return curObj->getSelectedText();

  return QString();
}

QString XWTeXTextBoxPart::getText()
{
  XWTeXText * obj = head;
  QString str;
  while (obj)
  {
    QString tmp = obj->getText();
    str += tmp;
    obj = obj->next;
  }

  return str;
}

double XWTeXTextBoxPart::getWidth(double maxw)
{
  if (lastRow < 0)
  {
    XWTeXText * obj = head;
    while (obj)
    {
      obj->getDimension();
      obj = obj->next;
    }
    breakLine(maxw);
  }

  double w = 0.0;
  for (int i = 0; i <= lastRow;i++)
    w = qMax(w,rows[i]->width);

  return w;
}

void XWTeXTextBoxPart::goToEnd()
{
  if (!last)
    return ;

  curObj = last;
  if (curObj->keyWord != XW_TEX_LETTER && 
      curObj->keyWord != XW_TEX_TEXT &&
      curObj->keyWord != XW_TEX_DIGIT)
  {
    box->updateCursor(curObj->x + curObj->width - 0.5, curObj->y - curObj->height, 
                curObj->x + curObj->width + 0.5, curObj->y + curObj->depth, 
                XW_TEX_POS_AFTER);
  }
  else
  {
    box->updateCursor(curObj->x + curObj->width - 0.5, curObj->y - curObj->height, 
                curObj->x + curObj->width + 0.5, curObj->y + curObj->depth, 
                curObj->text.length());
  }
}

bool XWTeXTextBoxPart::goToNext()
{
  if (!curObj)
    return false;

  if (!curObj->moveToNextChar())
  {
    XWTeXText * obj = curObj;
    if (textPos == XW_TEX_POS_BEFORE && obj->next)
    {
      obj = obj->next;
      box->updateCursor(obj->x - 0.5, obj->y - obj->height, 
                 obj->x + 0.5, obj->y + obj->depth, 
                 XW_TEX_POS_BEFORE);
      curObj = obj;
    }      
    else if (obj->head)
    {
      while(obj->head)
        obj = obj->head;

      box->updateCursor(obj->x - 0.5, obj->y - obj->height, 
                 obj->x + 0.5, obj->y + obj->depth, 
                 XW_TEX_POS_BEFORE);
      curObj = obj;
    }
    else
    {  
      while (obj->parent())
      {
        obj = (XWTeXText*)(obj->parent());
        if (obj->next)         
          break;
      }

      if (obj->next)
      {
        obj = obj->next;
        switch (obj->keyWord)
        {
          case XW_TEX_ROW:
          case XW_TEX_SUB:
          case XW_TEX_SUP:
          case XW_TEX_COL:
          case XW_TEX_PARAM:
          case XW_TEX_OPT:
          case LAarray:
          case LAeqnarray:
          case LAeqnarrayStar:
          case LAequation:
            {
              while(obj->head)
                obj = obj->head;
            }
            break;

          default:
            break;
        }

        box->updateCursor(obj->x - 0.5, obj->y - obj->height, 
                     obj->x + 0.5, obj->y + obj->depth, 
                     XW_TEX_POS_BEFORE);
        curObj = obj;
      }
      else
      {
        box->updateCursor(obj->x + obj->width - 0.5, obj->y - obj->height, 
                obj->x + obj->width + 0.5, obj->y + obj->depth, 
                XW_TEX_POS_AFTER);
        curObj = obj;
      }
    }    
  }

  return true;
}

bool XWTeXTextBoxPart::goToPrevious()
{
  if (!curObj)
    return false;

  if (!curObj->moveToPrevChar())
  {
    XWTeXText * obj = curObj;
    if (textPos == XW_TEX_POS_AFTER && obj->last)
    {
      while(obj->last)
        obj = obj->last;

      box->updateCursor(obj->x + obj->width - 0.5, obj->y - obj->height, 
                obj->x + obj->width + 0.5, obj->y + obj->depth, 
                XW_TEX_POS_AFTER);
      curObj = obj;
      if (obj->keyWord == XW_TEX_LETTER || obj->keyWord == XW_TEX_DIGIT || obj->keyWord == XW_TEX_TEXT)
          textPos = obj->text.length();
    }
    else if (obj->prev)
    {
      obj = obj->prev;
      box->updateCursor(obj->x + obj->width - 0.5, obj->y - obj->height, 
                obj->x + obj->width + 0.5, obj->y + obj->depth, 
                XW_TEX_POS_AFTER);
      curObj = obj;
      if (obj->keyWord == XW_TEX_LETTER || obj->keyWord == XW_TEX_DIGIT || obj->keyWord == XW_TEX_TEXT)
          textPos = obj->text.length();
    }      
    else
    {  
      while (obj->parent())
      {
        obj = (XWTeXText*)(obj->parent());
        if (obj->prev)         
          break;
      }

      if (obj->prev)
      {
        obj = obj->prev;
        switch (obj->keyWord)
        {
          case XW_TEX_ROW:
          case XW_TEX_SUB:
          case XW_TEX_SUP:
          case XW_TEX_COL:
          case XW_TEX_PARAM:
          case XW_TEX_OPT:
          case LAarray:
          case LAeqnarray:
          case LAeqnarrayStar:
          case LAequation:
            {
              while(obj->last)
                obj = obj->last;
            }
            break;

          default:
            break;
        }

        box->updateCursor(obj->x + obj->width - 0.5, obj->y - obj->height, 
                obj->x + obj->width + 0.5, obj->y + obj->depth, 
                XW_TEX_POS_AFTER);
        curObj = obj;
        if (obj->keyWord == XW_TEX_LETTER || obj->keyWord == XW_TEX_DIGIT || obj->keyWord == XW_TEX_TEXT)
            textPos = obj->text.length();
      }
      else
      {
        box->updateCursor(obj->x - 0.5, obj->y - obj->height, 
                     obj->x + 0.5, obj->y + obj->depth, 
                     XW_TEX_POS_BEFORE);
        curObj = obj;
      }
    }        
  }

  return true;
}

void XWTeXTextBoxPart::goToStart()
{
  if (!head)
    return ;

  curObj = head;
  if (curObj->keyWord != XW_TEX_LETTER && 
      curObj->keyWord != XW_TEX_TEXT &&
      curObj->keyWord != XW_TEX_DIGIT)
  {
    box->updateCursor(curObj->x - 0.5, curObj->y - curObj->height, 
                curObj->x + 0.5, curObj->y + curObj->depth, 
                XW_TEX_POS_BEFORE);
  }
  else
  {
    box->updateCursor(curObj->x - 0.5, curObj->y - curObj->height, 
                curObj->x + 0.5, curObj->y + curObj->depth, 
                0);
  }
}

bool XWTeXTextBoxPart::hitTest(double xA, double yA)
{
  for (int i = 0; i <= lastRow; i++)
  {
    if (rows[i]->hitTest(xA,yA))
    {
      curRow = i;
      return true;
    }
  }

  return false;
}

void XWTeXTextBoxPart::insert(XWTeXText * newobj)
{
  if (!head)
  {
    head = newobj;
    head->prev = 0;
    last = newobj;
    last->next = 0;
  }
  else if (curObj == head)
  {
    if (textPos == XW_TEX_POS_BEFORE)
    {
      newobj->next = curObj;
      curObj->prev = newobj;
      head = newobj;
      head->prev = 0;
    }
    else
    {
      newobj->prev = curObj;
      newobj->next = curObj->next;      
      if (curObj->next)
        curObj->next->prev = newobj;
      else
      {
        last = newobj;
        last->next = 0;
      }
      curObj->next = newobj;
    }    
  }
  else if (curObj == last)
  {
    if (textPos == XW_TEX_POS_BEFORE)
    {
      newobj->prev = last->prev;
      if (last->prev)
        last->prev->next = newobj;
      last->prev = newobj;
      newobj->next = last;
    }
    else
    {
      last->next = newobj;
      newobj->prev = last;
      last = newobj;
      last->next = 0;
    }    
  }
  else
    curObj->insert(newobj);
    
  curObj = newobj;
  if (curObj->keyWord == XW_TEX_LETTER || 
      curObj->keyWord == XW_TEX_DIGIT || 
      curObj->keyWord == XW_TEX_TEXT)
      textPos = curObj->text.length();
  else
    textPos = XW_TEX_POS_AFTER;
}

bool XWTeXTextBoxPart::insert(const QString & str, QUndoCommand * cmdA)
{
  int pos = 0;
  int len = str.length();
  bool o = true;
  if (!cmdA)
  {
    cmdA = new QUndoCommand;
    o = false;
  }
    
  XWTeXText * obj = 0;
  QObject * parent = 0;
  if (curObj)
  {
    switch (curObj->keyWord)
    {
      case XW_TEX_COL:
      case XW_TEX_SUB:
      case XW_TEX_SUP:
      case XW_TEX_PARAM:
      case XW_TEX_OPT:
        parent = curObj;
        break;

      default:
        parent = curObj->parent();
        break;
    }
  }

  while (pos < len)
  {
    if (str[pos] == QChar('$') || str[pos].isSpace())
      pos++;
    else if (str[pos] == QChar('%'))
      XWTeXTextBox::skipComment(str,len,pos);
    else
    {
      if (str[pos] == QChar('\\'))
      {
        QString key = XWTeXTextBox::scanControlSequence(str,len,pos);
        int id = lookupLaTeXID(key);
        if (id == LAbegin)
        {
          key = XWTeXTextBox::scanEnviromentName(str,len,pos);
          id = lookupLaTeXID(key);
        }
        
        obj = new XWTeXText(id,box,parent);
      }
      else if (str[pos] == QChar('+') || 
               str[pos] == QChar('-') || 
               str[pos] == QChar('=') ||
               str[pos] == QChar('*') ||
               str[pos] == QChar(';') ||
               str[pos] == QChar('!') ||
               str[pos] == QChar(':') ||
               str[pos] == QChar('?') )
      {
        obj = new XWTeXText(XW_TEX_ASE,box,parent);
      }        
      else if (str[pos] == QChar('.') || str[pos].isDigit())
        obj = new XWTeXText(XW_TEX_DIGIT,box,parent);
      else
        obj = new XWTeXText(XW_TEX_LETTER,box,parent);

      if (!parent || curObj != parent)
        new XWTeXNew(obj,cmdA);
      else 
        new XWTeXAppend(obj,curObj,cmdA);
      
      obj->scan(str,len,pos);
    }    
  }

  if (!o)
    box->push(cmdA);

  return true;
}

bool XWTeXTextBoxPart::insertText(const QString & str)
{
  if (!curObj)
    return false;

  QUndoCommand * cmd = 0;
  if (curObj->keyWord == XW_TEX_TEXT)
    cmd = new XWTeXInsertString(curObj,textPos,str);
  else
  {
    XWTeXText * obj = new XWTeXText(XW_TEX_TEXT,box,curObj->parent());
    obj->text = str;
    if (textPos == XW_TEX_POS_AFTER)
      cmd = new XWTeXInsert(obj,curObj);
    else
      cmd = new XWTeXInsert(obj,curObj->prev);
  }

  box->push(cmd);

  return true;
}

bool XWTeXTextBoxPart::keyInput(const QString & str)
{
  bool ret = false;
  QUndoCommand * cmd = 0;
  if (str[0] == QChar('\\'))
  {
    if (curObj && 
        curObj->keyWord == XW_TEX_LETTER && 
        textPos < curObj->text.length())
    {
      QString tmp = curObj->text;
      tmp.insert(textPos,str);
      int pos = textPos;
      int len = tmp.length();
      QString key = XWTeXTextBox::scanControlSequence(tmp, len,pos);
      int id = lookupLaTeXID(key);
      if (id == LAbegin)
      {
        key = XWTeXTextBox::scanEnviromentName(tmp,len,pos);
        id = lookupLaTeXID(key);
      }

      XWTeXText * obj = new XWTeXText(id,box,curObj->parent());
      cmd = new QUndoCommand;
      new XWTeXNew(obj,cmd);
      if (textPos == 0)
        new XWTeXRemove(curObj,cmd);
      else
        new XWTeXRemoveString(curObj,textPos,pos - textPos,cmd);
      if (pos < len)
      {
        XWTeXText * txt = new XWTeXText(XW_TEX_LETTER,box,curObj->parent());
        txt->text = tmp.mid(pos,-1);
        new XWTeXInsert(txt,obj,cmd);
      }
      box->push(cmd);
      ret = true;
    }
    else
    {
      XWTeXText * obj = 0;
      if (curObj)
      {
        switch (curObj->keyWord)
        {
          case XW_TEX_COL:
          case XW_TEX_SUB:
          case XW_TEX_SUP:
          case XW_TEX_PARAM:
          case XW_TEX_OPT:
            obj = new XWTeXText(XW_TEX_UNKNOWN,box,curObj);
            cmd = new XWTeXAppend(obj,curObj);
            curObj = obj;
            break;

          default:
            obj = new XWTeXText(XW_TEX_UNKNOWN,box,curObj->parent());
            cmd = new XWTeXNew(obj);
            break;
        }   
      }
      else
      {
        obj = new XWTeXText(XW_TEX_UNKNOWN,box,0);
        cmd = new XWTeXNew(obj);
      }
        
      obj->text = str;      
      box->push(cmd);
      ret = true;
    }
  }
  else if (str[0] == QChar('^'))
  {
    if (curObj)
    {
      XWTeXText * obj = new XWTeXText(XW_TEX_SUP,box,curObj);
      XWTeXText * param = new XWTeXText(XW_TEX_PARAM,box,obj);
      obj->append(param);
      XWTeXAppend * cmd = new XWTeXAppend(obj,curObj);
      curObj = param;
      box->push(cmd);
      ret = true;
    }
  }
  else if (str[0] == QChar('_'))
  {
    if (curObj)
    {
      XWTeXText * obj = new XWTeXText(XW_TEX_SUB,box,curObj);
      XWTeXText * param = new XWTeXText(XW_TEX_PARAM,box,obj);
      obj->append(param);
      XWTeXAppend * cmd = new XWTeXAppend(obj,curObj);
      curObj = param;
      box->push(cmd);
      ret = true;
    }
  }
  else if (str[0] == QChar('+') || 
               str[0] == QChar('-') || 
               str[0] == QChar('=') ||
               str[0] == QChar('*') ||
               str[0] == QChar(';') ||
               str[0] == QChar('!') ||
               str[0] == QChar(':') ||
               str[0] == QChar('?') )
  {
    XWTeXText * obj = 0;
    if (curObj)
    {
      switch (curObj->keyWord)
      {
        case XW_TEX_COL:
        case XW_TEX_SUB:
        case XW_TEX_SUP:
        case XW_TEX_PARAM:
        case XW_TEX_OPT:
          obj = new XWTeXText(XW_TEX_ASE,box,curObj);
          cmd = new XWTeXAppend(obj,curObj);          
          curObj = obj;
          break;

        default:
          obj = new XWTeXText(XW_TEX_ASE,box,curObj->parent());
          cmd = new XWTeXAppend(obj,curObj);
          break;
      }
    }      
    else
    {
      obj = new XWTeXText(XW_TEX_ASE,box,0);
      cmd = new XWTeXNew(obj);
    }
      
    int len = str.length();
    int pos = 0;
    obj->scan(str,len,pos);    
    box->push(cmd);
    ret = true;
  }
  else if (str[0] == QChar('.') || str[0].isDigit())
  {
    if (curObj)
    {
      if (curObj->keyWord == XW_TEX_DIGIT)
      {
        cmd = new XWTeXInsertString(curObj,textPos,str);
        box->push(cmd);
        ret = true;
      }        
      else
      {
        XWTeXText * obj = 0;
        switch (curObj->keyWord)
        {
          case XW_TEX_COL:
          case XW_TEX_SUB:
          case XW_TEX_SUP:
          case XW_TEX_PARAM:
          case XW_TEX_OPT:
            obj = new XWTeXText(XW_TEX_DIGIT,box,curObj);
            cmd = new XWTeXAppend(obj,curObj);
            curObj = obj;
            break;

          default:
            obj = new XWTeXText(XW_TEX_DIGIT,box,curObj->parent());
            cmd = new XWTeXNew(obj);
            break;
        }   

        obj->text = str;
        textPos = obj->text.length();        
        box->push(cmd);
        ret = true;
      }
    }
    else
    {
      XWTeXText * obj = new XWTeXText(XW_TEX_DIGIT,box,0);
      obj->text = str;
      textPos = obj->text.length();
      cmd = new XWTeXNew(obj);
      box->push(cmd);
      ret = true;
    }    
  }
  else
  {
    if (curObj)
    {
      if (curObj->keyWord == XW_TEX_LETTER)
      {
        cmd = new XWTeXInsertString(curObj,textPos,str);
        box->push(cmd);
        ret = true;
      }        
      else if (curObj->keyWord == XW_TEX_UNKNOWN)
      {
        QString tmp = curObj->text;
        tmp.insert(textPos,str);
        int len = tmp.length();
        int pos = 0;
        QString key = XWTeXTextBox::scanControlSequence(tmp, len,pos);
        int id = lookupLaTeXID(key);
        if (id == LAbegin)
        {
          key = XWTeXTextBox::scanEnviromentName(tmp,len,pos);
          id = lookupLaTeXID(key);
        }
        if (id == XW_TEX_UNKNOWN)
        {
          cmd = new XWTeXInsertString(curObj,textPos,str);
          box->push(cmd);
          ret = true;
        }          
        else
        {
          XWTeXText * obj = new XWTeXText(id,box,curObj->parent());
          cmd = new QUndoCommand;
          new XWTeXNew(obj,cmd);
          new XWTeXRemove(curObj,cmd);
          box->push(cmd);
          ret = true;
        }
      }
      else
      {
        XWTeXText * obj = 0;
        switch (curObj->keyWord)
        {
          case XW_TEX_COL:
          case XW_TEX_SUB:
          case XW_TEX_SUP:
          case XW_TEX_PARAM:
          case XW_TEX_OPT:
            obj = new XWTeXText(XW_TEX_LETTER,box,curObj);
            cmd = new XWTeXAppend(obj,curObj);
            curObj = obj;
            break;

          default:
            obj = new XWTeXText(XW_TEX_LETTER,box,curObj->parent());
            cmd = new XWTeXNew(obj);
            break;
        }        
        obj->text = str;
        textPos = obj->text.length();        
        box->push(cmd);
        ret = true;
      }      
    }
    else
    {
      XWTeXText * obj = new XWTeXText(XW_TEX_LETTER,box,0);
      obj->text = str;
      textPos = obj->text.length();
      cmd = new XWTeXNew(obj);
      box->push(cmd);
      ret = true;
    }    
  } 

  return ret;
}

void XWTeXTextBoxPart::moveToEndOfLine()
{
  if (!head)
    return ;

  XWTeXTextRow * row = rows[curRow];
  XWTeXText * obj = row->getLast();
  if (obj)
  {
    curObj = obj;
    box->updateCursor(obj->x + obj->width - 0.5, obj->y - obj->height, 
                 obj->x + obj->width + 0.5, obj->y + obj->depth, 
                 XW_TEX_POS_AFTER);
    if (obj->keyWord == XW_TEX_LETTER || obj->keyWord == XW_TEX_DIGIT || obj->keyWord == XW_TEX_TEXT)
      textPos = obj->text.length();
  }
}

void XWTeXTextBoxPart::moveToNextLine()
{
  if (curRow == lastRow)
    return ;

  curRow++;
  XWTeXTextRow * row = rows[curRow];
  XWTeXText * obj = row->getFirst();
  if (obj)
  {
    curObj = obj;
    box->updateCursor(obj->x - 0.5, obj->y - obj->height, 
                 obj->x + 0.5, obj->y + obj->depth, 
                 XW_TEX_POS_BEFORE);
  }
}

void XWTeXTextBoxPart::moveToPreLine()
{
  if (curRow <= 0)
    return ;

  curRow--;
  XWTeXTextRow * row = rows[curRow];
  XWTeXText * obj = row->getLast();
  if (obj)
  {
    curObj = obj;
    box->updateCursor(obj->x + obj->width - 0.5, obj->y - obj->height, 
                 obj->x + obj->width + 0.5, obj->y + obj->depth, 
                 XW_TEX_POS_AFTER);
    if (obj->keyWord == XW_TEX_LETTER || obj->keyWord == XW_TEX_DIGIT || obj->keyWord == XW_TEX_TEXT)
      textPos = obj->text.length();
  }
}

void XWTeXTextBoxPart::moveToStartOfLine()
{
  if (!head)
    return ;

  XWTeXTextRow * row = rows[curRow];
  XWTeXText * obj = row->getFirst();
  if (obj)
  {
    curObj = obj;
    box->updateCursor(obj->x - 0.5, obj->y - obj->height, 
                 obj->x + 0.5, obj->y + obj->depth, 
                 XW_TEX_POS_BEFORE);
  }
}

bool XWTeXTextBoxPart::newPar()
{
  if (!curObj)
    return false;

  XWTeXText * lb = new XWTeXText(LAlinebreak,box,curObj->parent());
  XWTeXText * obj = 0;
  if (curObj->keyWord == XW_TEX_TEXT)
    obj = new XWTeXText(XW_TEX_TEXT,box,curObj->parent());
  else 
    obj = new XWTeXText(XW_TEX_LETTER,box,curObj->parent());

  QUndoCommand * cmd = new QUndoCommand;
  if (textPos > 0)
  {
    int epos = curObj->text.length();
    obj->text = curObj->text.mid(textPos,-1);
    new XWTeXRemoveString(curObj,textPos,epos,cmd);
  }

  if (textPos == XW_TEX_POS_BEFORE)
  {
    new XWTeXNew(obj,cmd);
    new XWTeXNew(lb,cmd);
  }
  else
  {
    new XWTeXNew(lb,cmd);
    new XWTeXNew(obj,cmd);
  }

  box->push(cmd);

  return true;
}

bool XWTeXTextBoxPart::paste()
{
  QString str = QApplication::clipboard()->text();
  if (str.isEmpty())
    return false;

  int pos = 0;
  int len = str.length();
  QUndoCommand * cmd = new QUndoCommand;
  XWTeXText * obj = 0;
  QObject * parent = 0;
  if (curObj)
  {
    switch (curObj->keyWord)
    {
      case XW_TEX_COL:
      case XW_TEX_SUB:
      case XW_TEX_SUP:
      case XW_TEX_PARAM:
      case XW_TEX_OPT:
        parent = curObj;
        break;

      default:
        parent = curObj->parent();
        break;
    }
  }

  while (pos < len)
  {
    if (str[pos] == QChar('$') || str[pos].isSpace())
      pos++;
    else if (str[pos] == QChar('%'))
      XWTeXTextBox::skipComment(str,len,pos);
    else
    {
      if (str[pos] == QChar('\\'))
      {
        QString key = XWTeXTextBox::scanControlSequence(str,len,pos);
        int id = lookupLaTeXID(key);
        if (id == LAbegin)
        {
          key = XWTeXTextBox::scanEnviromentName(str,len,pos);
          id = lookupLaTeXID(key);
        }
        
        obj = new XWTeXText(id,box,parent);
      }
      else if (str[pos] == QChar('+') || 
               str[pos] == QChar('-') || 
               str[pos] == QChar('=') ||
               str[pos] == QChar('*') ||
               str[pos] == QChar(';') ||
               str[pos] == QChar('!') ||
               str[pos] == QChar(':') ||
               str[pos] == QChar('?') )
      {
        obj = new XWTeXText(XW_TEX_ASE,box,parent);
      }        
      else if (str[pos] == QChar('.') || str[pos].isDigit())
        obj = new XWTeXText(XW_TEX_DIGIT,box,parent);
      else if (str[pos].isLetter())
        obj = new XWTeXText(XW_TEX_LETTER,box,parent);
      else
        obj = new XWTeXText(XW_TEX_TEXT,box,parent);

      if (!parent || curObj != parent)
        new XWTeXNew(obj,cmd);
      else 
        new XWTeXAppend(obj,curObj,cmd);
      
      obj->scan(str,len,pos);
    }    
  }
  box->push(cmd);
  return true;
}

void XWTeXTextBoxPart::remove(XWTeXText*obj)
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

    if (obj == curObj)
    {
      if (!obj->next && !obj->prev) 
      {
        if (obj->parent())
        {
          XWTeXText * pobj = (XWTeXText*)(obj->parent());
          curObj = pobj;
          textPos = XW_TEX_POS_AFTER;
        }
        else
          curObj = 0;
      }
      else
      {
        if (textPos == XW_TEX_POS_BEFORE)
        {
          if (obj->next)
            curObj = obj->next;
          else
          {
            curObj = obj->prev;
            textPos = XW_TEX_POS_AFTER;
          }
        }
        else
        {
          if (obj->prev)
            curObj = obj->prev;
          else
          {
            curObj = obj->next;
            textPos = XW_TEX_POS_BEFORE;
          }
        }
      }
    }
  }
}

bool XWTeXTextBoxPart::removeSelected(QUndoCommand * cmdA)
{
  if (!cmdA || !curObj || !curObj->hasSelect())
    return false;

  QString str = curObj->getSelectedText();
  int sp = curObj->getTextPosition();
  int ep = sp + str.length();
  new XWTeXRemoveString(curObj,sp,ep,cmdA);
  return true;
}

void XWTeXTextBoxPart::setContents(const QString & str)
{
  curObj = head;
  while (curObj)
  {
    XWTeXText * tmp = curObj->next;
    delete curObj;
    curObj = tmp;
  }

  clear();
  int len = str.length();
  int pos = 0;
  box->scan(str,len,pos);
}

void XWTeXTextBoxPart::setFontSize(int s)
{
  XWTeXText * obj = head;
  while (obj)
  {
    obj->setFontSize(s);
    obj = obj->next;
  }
}

void XWTeXTextBoxPart::setXY(int align, double xA,double yA)
{
  double maxw = 0.0;
  for (int i = 0; i <= lastRow; i++)
    maxw = qMax(maxw,rows[i]->width);
    
  for (int i = 0; i <= lastRow; i++)
  {
    switch (align)
    {
      default:
        break;

      case XW_TEX_CENTER:
        xA = xA + (maxw - rows[i]->width) / 2;
        break;

      case XW_TEX_RIGHT:
        xA = xA + (maxw - rows[i]->width);
        break;
    }

    yA -= rows[i]->height;
    rows[i]->setXY(xA,yA);
    yA -= rows[i]->depth;
  }
}

void XWTeXTextBoxPart::typeset(double & pagewidth,double & pageheight)
{
  XWTeXText * obj = head;
  while (obj)
  {
    obj->getDimension();
    obj = obj->next;
  }

  double maxW = pagewidth - 2 * 72.27;
  breakLine(maxW);

  double totalh = 0;
  maxW = 0;
  for (int i = 0; i <= lastRow; i++)
  {
    totalh += rows[i]->height;
    totalh += rows[i]->depth;
    totalh += 15;
    maxW = qMax(maxW,rows[i]->width);
  }

  totalh += lastRow * 15;
  if (totalh > (419.53 - 2 * 72.27))
    pageheight = totalh + 2 * 72.27;
  else
    pageheight = 419.53;

  double y = (pageheight + totalh) / 2; 
  double x = (pagewidth - maxW) / 2;
  for (int i = 0; i <= lastRow; i++)
  {
    y -= rows[i]->height; 
    rows[i]->setXY(x,y);
    y -= rows[i]->depth;
    y -= 15;
  }    
}

void XWTeXTextBoxPart::addSubscript()
{
  if (!curObj)
    return ;

  XWTeXText * obj = new XWTeXText(XW_TEX_SUB,box,curObj);
  XWTeXText * param = new XWTeXText(XW_TEX_PARAM,box,obj);
  obj->append(param);
  XWTeXAppend * cmd = new XWTeXAppend(obj,curObj);
  curObj = param;
  box->push(cmd);
}

void XWTeXTextBoxPart::addSupscript()
{
  if (!curObj)
    return ;

  XWTeXText * obj = new XWTeXText(XW_TEX_SUP,box,curObj);
  XWTeXText * param = new XWTeXText(XW_TEX_PARAM,box,obj);
  obj->append(param);
  XWTeXAppend * cmd = new XWTeXAppend(obj,curObj);
  curObj = param;
  box->push(cmd);
}

void XWTeXTextBoxPart::clear()
{
  if (rows)
  {
    for (int i = 0; i < maxRow; i++)
    {
      delete rows[i];
      rows[i] = 0;
    }

    free(rows);
    rows = 0;
  }

  maxRow = 0;
  lastRow = -1;
  curRow = -1;
}

void XWTeXTextBoxPart::delColumn()
{
  if (!curObj)
    return ;

  XWTeXText * row = curObj;
  XWTeXText * col = 0;
  while (row->parent())
  {
    if (row->keyWord == XW_TEX_ROW)
      break;

    col = row;
    row = (XWTeXText*)(row->parent());
  }

  if (!col || row->keyWord != XW_TEX_ROW)
    return ;

  int k = 0;
  XWTeXText * tmp = row->head;
  while (tmp)
  {
    if (tmp == col)
      break;
    tmp = tmp->next;
    k++;
  }

  XWTeXText * arry = (XWTeXText*)(row->parent());
  row = arry->head->next;
  QList<XWTeXText*> columns;
  while (row)
  {
    col = row->head;
    for (int i = 0; i < k; i++)
      col = col->next;

    columns << col;
      
    row = row->next;
  }

  QUndoCommand * cmd = new QUndoCommand;
  for (int i = 0; i < columns.size(); i++)
    new XWTeXRemove(columns[i],cmd);
  new XWTeXRemoveString(arry->head,k,k+1,cmd);

  box->push(cmd);
}

void XWTeXTextBoxPart::delRow()
{
  if (!curObj)
    return ;

  XWTeXText * row = curObj;
  while (row->parent())
  {
    if (row->keyWord == XW_TEX_ROW)
      break;
    row = (XWTeXText*)(row->parent());
  }

  if (row->keyWord != XW_TEX_ROW)
    return ;

  XWTeXRemove * cmd = new XWTeXRemove(row);
  box->push(cmd);
}

void XWTeXTextBoxPart::insertColumn()
{
  if (!curObj)
    return ;

  XWTeXText * row = curObj;
  XWTeXText * col = 0;
  while (row->parent())
  {
    if (row->keyWord == XW_TEX_ROW)
      break;

    col = row;
    row = (XWTeXText*)(row->parent());
  }

  if (!col || row->keyWord != XW_TEX_ROW)
    return ;

  XWTeXText * arry = (XWTeXText*)(row->parent());
  if (arry->keyWord != LAarray)
    return ;

  int k = 0;
  XWTeXText * tmp = row->head;
  while (tmp)
  {
    if (tmp == col)
      break;
    tmp = tmp->next;
    k++;
  }

  row = arry->head->next;
  QList<XWTeXText*> columns;
  while (row)
  {
    col = row->head;
    for (int i = 0; i < k; i++)
      col = col->next;

    columns << col;
      
    row = row->next;
  }

  QUndoCommand * cmd = new QUndoCommand;
  for (int i = 0; i < columns.size(); i++)
  {
    col = columns[i];
    XWTeXText * ncol = new XWTeXText(XW_TEX_COL,box,col->parent());
    new XWTeXInsert(ncol,col->prev,cmd);
  }

  new XWTeXInsertString(arry->head,k,"c",cmd);
  curObj = columns[0]->prev;
  box->push(cmd);
}

void XWTeXTextBoxPart::insertRow()
{
  if (!curObj)
    return ;

  XWTeXText * row = curObj;
  while (row->parent())
  {
    if (row->keyWord == XW_TEX_ROW)
      break;
    row = (XWTeXText*)(row->parent());
  }

  if (row->keyWord != XW_TEX_ROW)
    return ;

  XWTeXText * nrow = new XWTeXText(XW_TEX_ROW,box,row->parent());
  XWTeXText * col = row->head;
  while (col)
  {
    XWTeXText * ncol = new XWTeXText(XW_TEX_COL,box,nrow);
    nrow->append(ncol);
    col = col->next;
  }

  XWTeXInsert * cmd = new XWTeXInsert(nrow,row->prev);
  curObj = nrow->head;
  box->push(cmd);
}

void XWTeXTextBoxPart::breakLine(double maxw)
{
  if (!head)
    return ;

  double curw = 0;
  lastRow = -1;
  curRow = -1;
  for (int i = 0; i < maxRow; i++)
    rows[i]->clear();
  XWTeXText  * obj = head;
  while (obj)
  {
    obj->breakLine(maxw,curw);
    obj = obj->next;
  }
}

XWTeXTextBox::XWTeXTextBox(QObject * parent)
:XWTeXBox(parent),
 cur(-1)
{}

XWTeXTextBox::~XWTeXTextBox()
{}

bool XWTeXTextBox::back()
{
  if (cur < 0)
    return false;

  return parts[cur]->back();
}

bool XWTeXTextBox::contains(double xA, double yA)
{
  for (int i = 0; i < parts.size(); i++)
  {
    if (parts[i]->contains(xA,yA))
      return true;
  }

  return false;
}

bool XWTeXTextBox::cut()
{
  if (cur < 0)
    return false;

  return parts[cur]->cut();
}

bool XWTeXTextBox::del()
{
  if (cur < 0)
    return false;

  return parts[cur]->del();
}

QChar XWTeXTextBox::deleteChar(int pos)
{
  return parts[cur]->deleteChar(pos);
}

void XWTeXTextBox::doContent(XWPDFDriver * driver)
{
  for (int i = 0; i < parts.size(); i++)
    parts[i]->doContent(driver);
}

void XWTeXTextBox::draw(QPainter * painter,const QRectF & r)
{
  for (int i = 0; i < parts.size(); i++)
    parts[i]->draw(painter,r);
}

void XWTeXTextBox::draw(QPainter * painter)
{
  for (int i = 0; i < parts.size(); i++)
    parts[i]->draw(painter);
}

void XWTeXTextBox::dragTo(XWPDFDriver * driver, double xA, double yA)
{
  if (cur < 0)
    return ;

  parts[cur]->dragTo(driver,xA,yA);
}

void XWTeXTextBox::dragTo(QPainter * painter, double xA, double yA)
{
  if (cur < 0)
    return ;

  parts[cur]->dragTo(painter,xA,yA);
}

bool XWTeXTextBox::dropTo(double xA,double yA)
{
  if (cur < 0)
    return false;

  return parts[cur]->dropTo(xA,yA);
}

int XWTeXTextBox::getAnchorPosition()
{
  if (cur < 0)
    return 0;

  return parts[cur]->getAnchorPosition();
}

int XWTeXTextBox::getCursorPosition()
{
  if (cur < 0)
    return 0;

  return parts[cur]->getCursorPosition();
}

XWTeXTextRow * XWTeXTextBox::getCurrentRow()
{
  return parts[cur]->getCurrentRow();
}

XWTeXText * XWTeXTextBox::getCurrent()
{
  if (cur < 0)
    return 0;

  return parts[cur]->getCurrent();
}

QString XWTeXTextBox::getCurrentText()
{
  if (cur < 0)
    return QString();

  return parts[cur]->getCurrentText();
}

double XWTeXTextBox::getDepth(double maxw, int nop)
{
  if (nop < 0 || nop >= parts.size())
    return 0;

  cur = nop;
  return parts[nop]->getDepth(maxw);
}

double XWTeXTextBox::getHeight(double maxw, int nop)
{
  if (nop < 0 || nop >= parts.size())
    return 0;

  cur = nop;
  return parts[nop]->getHeight(maxw);
}

XWTeXTextRow * XWTeXTextBox::getNewRow()
{
  return parts[cur]->getNewRow();
}

XWTeXText * XWTeXTextBox::getObject(double xA, double yA)
{
  for (int i = 0; i < parts.size(); i++)
  {
    if (parts[i]->contains(xA,yA))
      return parts[i]->getObject(xA,yA);
  }

  return 0;
}

QString XWTeXTextBox::getSelected()
{
  if (cur < 0)
    return QString();

  return parts[cur]->getSelected();
}

QString XWTeXTextBox::getSelectedText()
{
  if (cur < 0)
    return QString();

  return parts[cur]->getSelectedText();
}

QString XWTeXTextBox::getText()
{
  QString ret;
  for (int i = 0; i < parts.size(); i++)
  {
    QString tmp = parts[i]->getText();
    ret += tmp;
  }

  return ret;
}

double XWTeXTextBox::getWidth(double maxw, int nop)
{
  if (nop < 0 || nop >= parts.size())
    return 0;

  cur = nop;
  return parts[nop]->getWidth(maxw);
}

void XWTeXTextBox::goToEnd()
{
  int i = parts.size();
  if (i <= 0)
    return ;

  parts[i-1]->goToEnd();
}

bool XWTeXTextBox::goToNext()
{
  if (cur < 0)
    return false;

  if (parts[cur]->goToNext())
    return true;

  if (cur == parts.size() - 1)
    return false;

  cur--;
  parts[cur]->goToStart();
  return true;
}

bool XWTeXTextBox::goToPrevious()
{
  if (cur < 0)
    return false;

  if (parts[cur]->goToPrevious())
    return true;

  if (cur == 0)
    return false;

  cur--;
  parts[cur]->goToEnd();
  return true;
}

void XWTeXTextBox::goToStart()
{
  int i = parts.size();
  if (i <= 0)
    return ;

  parts[0]->goToStart();
}

bool XWTeXTextBox::hitTest(double xA, double yA)
{
  cur = -1;
  for (int i = 0; i < parts.size(); i++)
  {
    if (parts[i]->hitTest(xA,yA))
    {
      cur = i;
      return true;
    }
  }

  return true;
}

void XWTeXTextBox::insert(XWTeXText * newobj)
{
  parts[cur]->insert(newobj);
}

bool XWTeXTextBox::insert(const QString & str, QUndoCommand * cmdA)
{
  if (cur < 0)
  {
    if (parts.size() > 0)
      return false;
    cur = 0;
    XWTeXTextBoxPart * part = new XWTeXTextBoxPart(this,this);
    parts << part;
  }

  return parts[cur]->insert(str,cmdA);
}

bool XWTeXTextBox::insertText(const QString & str)
{
  if (cur < 0)
  {
    if (parts.size() > 0)
      return false;
    cur = 0;
    XWTeXTextBoxPart * part = new XWTeXTextBoxPart(this,this);
    parts << part;
  }

  return parts[cur]->insertText(str);
}

bool XWTeXTextBox::keyInput(const QString & str)
{
  if (cur < 0)
  {
    if (parts.size() > 0)
      return false;
      
    cur = 0;
    XWTeXTextBoxPart * part = new XWTeXTextBoxPart(this,this);
    parts << part;
  }

  return parts[cur]->keyInput(str);
}

XWFTFont * XWTeXTextBox::loadFTFont(int fam, int s,bool bold,bool slant,
                         bool italic,bool cal)
{
  QString fontname = XWTeXText::getFontName(fam,s,bold,slant,italic,cal);
  fontname += ".pfb";
  int map = 1;
  int index = 0;
  QString idname = QString("%1%2").arg(fontname).arg(index);
  QByteArray ba = idname.toAscii();
  XWFontFileID * id = new XWFontFileID(ba.constData());
  if (!fontEngine)
    fontEngine = new XWFTFontEngine(true);
  XWFTFontFile * ftfontFile = fontEngine->getFontFile(id);
  if (ftfontFile)
  {
  	delete id;
  	id = 0;
  }
  else
  {
    XWFontSea sea;
    QString fn = sea.findFile(fontname);
    if (fn.isEmpty())
    {
  	  delete id;
  	  return 0;
    }

    ba = QFile::encodeName(fn);
  	ftfontFile = fontEngine->load(id, ba.data(), index, 0);
  	if (!ftfontFile)
  	{
  		delete id;
  		return 0;
  	}
  }

  XWFTFont  * font = fontEngine->getFont(ftfontFile, s, 1.0, 0.0, 0.0, false,false);
  if (font)
    font->setCharMap(map);
  return font;
}

void XWTeXTextBox::moveToEndOfLine()
{
  if (cur < 0)
    return ;

  parts[cur]->moveToEndOfLine();
}

void XWTeXTextBox::moveToNextLine()
{
  if (cur < 0)
    return ;

  parts[cur]->moveToNextLine();
}

void XWTeXTextBox::moveToPreLine()
{
  if (cur < 0)
    return ;

  parts[cur]->moveToPreLine();
}

void XWTeXTextBox::moveToStartOfLine()
{
  if (cur < 0)
    return ;

  parts[cur]->moveToStartOfLine();
}

bool XWTeXTextBox::newPar()
{
  if (cur < 0)
    return false;

  return parts[cur]->newPar();
}

bool XWTeXTextBox::paste()
{
  if (cur < 0)
    return false;

  return parts[cur]->paste();
}

void XWTeXTextBox::remove(XWTeXText*obj)
{
  parts[cur]->remove(obj);
}

bool XWTeXTextBox::removeSelected(QUndoCommand * cmdA)
{
  if (cur < 0)
    return false;

  return parts[cur]->removeSelected(cmdA);
}

void XWTeXTextBox::setContents(const QString & str)
{
  for (int i = 0; i < parts.size(); i++)
    parts[i]->setContents(str);
}

void XWTeXTextBox::setCurrentObject(XWTeXText * obj)
{
  if (cur < 0)
    return ;

  parts[cur]->setCurrentObject(obj);
}

void XWTeXTextBox::setFontSize(int s)
{
  for (int i = 0; i < parts.size(); i++)
    parts[i]->setFontSize(s);
}

void XWTeXTextBox::setXY(int align, double xA,double yA, int nop)
{
  if (nop < 0 || nop >= parts.size())
    return ;

  cur = nop;
  parts[nop]->setXY(align,xA,yA);
}

void XWTeXTextBox::typeset(double & pagewidth,double & pageheight)
{
  int ocur = cur;
  for (int i = 0; i < parts.size(); i++)
  {
    cur = i;
    parts[i]->typeset(pagewidth,pageheight);
  }
  cur = ocur;
}

void XWTeXTextBox::updateCursor(double , double , double ,
                    double , int )
{
}

void XWTeXTextBox::clear()
{
  for (int i = 0; i < parts.size(); i++)
    parts[i]->clear();
}

void XWTeXTextBox::delColumn()
{
  if (cur < 0)
    return ;

  parts[cur]->delColumn();
}

void XWTeXTextBox::delRow()
{
  if (cur < 0)
    return ;

  parts[cur]->delRow();
}

void XWTeXTextBox::insertColumn()
{
  if (cur < 0)
    return ;

  parts[cur]->insertColumn();
}

void XWTeXTextBox::insertRow()
{
  if (cur < 0)
    return ;

  parts[cur]->insertRow();
}
