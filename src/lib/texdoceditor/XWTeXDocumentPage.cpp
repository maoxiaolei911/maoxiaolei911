/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <stdlib.h>
#include "XWTeXDocSearcher.h"
#include "XWTeXDocument.h"
#include "XWTeXDocumentObject.h"
#include "XWTeXDocumentPage.h"

XWTeXDocumentRow::XWTeXDocumentRow()
 : startPos(0),
   endPos(0),
   dir(0),
   index(0),
   limit(0.0),
   minx(0.0),
   miny(0.0),
   maxx(0.0),
   maxy(0.0),
   maxlength(0),
   curBlock(0)
{}

void XWTeXDocumentRow::addBlock(XWTeXDocumentBlock * block)
{
  curBlock = block;
  blocks << block;
}

void XWTeXDocumentRow::clear()
{
  dir = 0;
  index = 0;
  startPos = 0;
  endPos = 0;
  limit = 0.0;
  minx = 0.0;
  miny = 0.0;
  maxx = 0.0;
  maxy = 0.0;
  maxlength = 0;
  curBlock = 0;
  blocks.clear();
}

void XWTeXDocumentRow::draw(QPainter * painter,const QColor & pagecolor,const QRectF & rect)
{
  QRectF r(minx,miny, maxx-minx,maxy-miny);
  r = r.intersected(rect);
  if (r.isEmpty())
    return ;

  double curx = 0.0;
  double cury = 0.0;
  switch (dir)
  {
    case TEX_DOC_WD_LTL:
      curx = maxx;
      cury = miny;
      break;

    case TEX_DOC_WD_TLT:
      curx = minx;
      cury = maxy;
      break;

    case TEX_DOC_WD_RTT:
      curx = minx;
      cury = miny;
      break;

    default:
      curx = maxx;
      cury = maxy;
      break;
  }

  if (blocks.size() == 1)
    blocks[0]->draw(painter,pagecolor,r,curx,cury,startPos,endPos);
  else if (blocks.size() == 2)
  {
    blocks[0]->draw(painter,pagecolor,r,curx,cury,startPos);
    blocks[1]->draw(painter,pagecolor,r,curx,cury,0,endPos);
  }
  else
  {
    blocks[0]->draw(painter,pagecolor,r,curx,cury,startPos);

    for (int i = 1; i < (blocks.size() - 1); i++)
      blocks[i]->draw(painter,pagecolor,r,curx,cury);

    blocks[blocks.size() - 1]->draw(painter,pagecolor,r,curx,cury,0,endPos);
  }
}

void XWTeXDocumentRow::drawPic(QPainter * painter)
{
  double curx = 0.0;
  double cury = 0.0;
  switch (dir)
  {
    case TEX_DOC_WD_LTL:
      curx = maxx;
      cury = miny;
      break;

    case TEX_DOC_WD_TLT:
      curx = minx;
      cury = maxy;
      break;

    case TEX_DOC_WD_RTT:
      curx = minx;
      cury = miny;
      break;

    default:
      curx = maxx;
      cury = maxy;
      break;
  }

  if (blocks.size() == 1)
    blocks[0]->drawPic(painter,curx,cury,startPos,endPos);
  else if (blocks.size() == 2)
  {
    blocks[0]->drawPic(painter,curx,cury,startPos);
    blocks[1]->drawPic(painter,curx,cury,0,endPos);
  }
  else
  {
    blocks[0]->drawPic(painter,curx,cury,startPos);

    for (int i = 1; i < (blocks.size() - 1); i++)
      blocks[i]->drawPic(painter,curx,cury);

    blocks[blocks.size() - 1]->drawPic(painter,curx,cury,0,endPos);
  }
}

void XWTeXDocumentRow::find(int pg, XWTeXDocSearhList * list)
{
  double cur = 0.0;
  double mi = 0.0;
  double ma = 0.0;
  QString content;
  switch (dir)
  {
    default:
      {
        cur = miny;
        if (blocks.size() == 1)
        {
          int s = startPos;
          int e = endPos;
          while (blocks[0]->find(s, e, cur, mi, ma, content))
          {
            list->createItem(blocks[0], s, e, pg, minx,mi,maxx,ma,content);
            s = e + 1;
            e = endPos;
            if (s > endPos)
              break;
          }
        }
        else if (blocks.size() == 2)
        {
          int s = startPos;
          int tmp = blocks[0]->getLength() - 1;
          int e = tmp;
          while (blocks[0]->find(s, e, cur, mi, ma, content))
          {
            list->createItem(blocks[0], s, e, pg, minx,mi,maxx,ma,content);
            s = e + 1;
            e = tmp;
            if (s > tmp)
              break;
          }

          s = 0;
          e = endPos;
          while (blocks[1]->find(s, e, cur, mi, ma, content))
          {
            list->createItem(blocks[1], s, e, pg, minx,mi,maxx,ma,content);
            s = e + 1;
            e = endPos;
            if (s > endPos)
              break;
          }
        }
        else
        {
          int s = startPos;
          int tmp = blocks[0]->getLength() - 1;
          int e = tmp;
          while (blocks[0]->find(s, e, cur, mi, ma, content))
          {
            list->createItem(blocks[0], s, e, pg, minx,mi,maxx,ma,content);
            s = e + 1;
            e = tmp;
            if (s > tmp)
              break;
          }

          for (int i = 1; i < blocks.size() - 1; i++)
          {
            s = 0;
            tmp = blocks[i]->getLength() - 1;
            e = tmp;
            while (blocks[i]->find(s, e, cur, mi, ma, content))
            {
              list->createItem(blocks[i], s, e, pg, minx,mi,maxx,ma,content);
              s = e + 1;
              e = tmp;
              if (s > tmp)
                break;
            }
          }

          s = 0;
          e = endPos;
          while (blocks[1]->find(s, e, cur, mi, ma, content))
          {
            list->createItem(blocks[1], s, e, pg, minx,mi,maxx,ma,content);
            s = e + 1;
            e = endPos;
            if (s > endPos)
              break;
          }
        }
      }      
      break;

    case TEX_DOC_WD_TLT:
      {
        cur = minx;
        if (blocks.size() == 1)
        {
          int s = startPos;
          int e = endPos;
          while (blocks[0]->find(s, e, cur, mi, ma, content))
          {
            list->createItem(blocks[0], s, e, pg, mi, miny,ma, maxy,content);
            s = e + 1;
            e = endPos;
            if (s > endPos)
              break;
          }
        }
        else if (blocks.size() == 2)
        {
          int s = startPos;
          int tmp = blocks[0]->getLength() - 1;
          int e = tmp;
          while (blocks[0]->find(s, e, cur, mi, ma, content))
          {
            list->createItem(blocks[0], s, e, pg, mi, miny,ma, maxy,content);
            s = e + 1;
            e = tmp;
            if (s > tmp)
              break;
          }

          s = 0;
          e = endPos;
          while (blocks[1]->find(s, e, cur, mi, ma, content))
          {
            list->createItem(blocks[1], s, e, pg, mi, miny,ma, maxy,content);
            s = e + 1;
            e = endPos;
            if (s > endPos)
              break;
          }
        }
        else
        {
          int s = startPos;
          int tmp = blocks[0]->getLength() - 1;
          int e = tmp;
          while (blocks[0]->find(s, e, cur, mi, ma, content))
          {
            list->createItem(blocks[0], s, e, pg, mi, miny,ma, maxy,content);
            s = e + 1;
            e = tmp;
            if (s > tmp)
              break;
          }

          for (int i = 1; i < blocks.size() - 1; i++)
          {
            s = 0;
            tmp = blocks[i]->getLength() - 1;
            e = tmp;
            while (blocks[i]->find(s, e, cur, mi, ma, content))
            {
              list->createItem(blocks[i], s, e, pg, mi, miny,ma, maxy,content);
              s = e + 1;
              e = tmp;
              if (s > tmp)
                break;
            }
          }

          s = 0;
          e = endPos;
          while (blocks[1]->find(s, e, cur, mi, ma, content))
          {
            list->createItem(blocks[1], s, e, pg, mi, miny,ma, maxy,content);
            s = e + 1;
            e = endPos;
            if (s > endPos)
              break;
          }
        }
      }
      break;

    case TEX_DOC_WD_TRT:
      {
        cur = maxx;
        if (blocks.size() == 1)
        {
          int s = startPos;
          int e = endPos;
          while (blocks[0]->find(s, e, cur, mi, ma, content))
          {
            list->createItem(blocks[0], s, e, pg, mi, miny,ma, maxy,content);
            s = e + 1;
            e = endPos;
            if (s > endPos)
              break;
          }
        }
        else if (blocks.size() == 2)
        {
          int s = startPos;
          int tmp = blocks[0]->getLength() - 1;
          int e = tmp;
          while (blocks[0]->find(s, e, cur, mi, ma, content))
          {
            list->createItem(blocks[0], s, e, pg, mi, miny,ma, maxy,content);
            s = e + 1;
            e = tmp;
            if (s > tmp)
              break;
          }

          s = 0;
          e = endPos;
          while (blocks[1]->find(s, e, cur, mi, ma, content))
          {
            list->createItem(blocks[1], s, e, pg, mi, miny,ma, maxy,content);
            s = e + 1;
            e = endPos;
            if (s > endPos)
              break;
          }
        }
        else
        {
          int s = startPos;
          int tmp = blocks[0]->getLength() - 1;
          int e = tmp;
          while (blocks[0]->find(s, e, cur, mi, ma, content))
          {
            list->createItem(blocks[0], s, e, pg, mi, miny,ma, maxy,content);
            s = e + 1;
            e = tmp;
            if (s > tmp)
              break;
          }

          for (int i = 1; i < blocks.size() - 1; i++)
          {
            s = 0;
            tmp = blocks[i]->getLength() - 1;
            e = tmp;
            while (blocks[i]->find(s, e, cur, mi, ma, content))
            {
              list->createItem(blocks[i], s, e, pg, mi, miny,ma, maxy,content);
              s = e + 1;
              e = tmp;
              if (s > tmp)
                break;
            }
          }

          s = 0;
          e = endPos;
          while (blocks[1]->find(s, e, cur, mi, ma, content))
          {
            list->createItem(blocks[1], s, e, pg, mi, miny,ma, maxy,content);
            s = e + 1;
            e = endPos;
            if (s > endPos)
              break;
          }
        }
      }
      break;
  }
}

void XWTeXDocumentRow::find(QList<XWTeXDocumentBlock*> & blocksA)
{
  for (int i = 0; i < blocks.size(); i++)
  {
    if (blocks[i]->find())
    {
      if (!blocksA.contains(blocks[i]))
        blocksA << blocks[i];
    }
  }
}

bool XWTeXDocumentRow::find(int pg, XWTeXDocumentBlock * blockA, 
                            XWTeXDocSearhList * list)
{
  int k = blocks.indexOf(blockA);
  if (k < 0)
    return false;

  double cur = 0.0;
  double mi = 0.0;
  double ma = 0.0;
  QString content;
  switch (dir)
  {
    default:
      {
        cur = miny;
        if (blocks.size() == 1)
        {
          int s = startPos;
          int e = endPos;
          while (blocks[0]->findReplaced(s, e, cur, mi, ma, content))
          {
            list->createItem(blocks[0], s, e, pg, minx,mi,maxx,ma,content);
            s = e + 1;
            e = endPos;
            if (s > endPos)
              break;
          }
        }
        else if (blocks.size() == 2)
        {
          int s = startPos;
          int tmp = blocks[0]->getLength() - 1;
          int e = tmp;
          while (blocks[0]->findReplaced(s, e, cur, mi, ma, content))
          {
            list->createItem(blocks[0], s, e, pg, minx,mi,maxx,ma,content);
            s = e + 1;
            e = tmp;
            if (s > tmp)
              break;
          }

          s = 0;
          e = endPos;
          while (blocks[1]->findReplaced(s, e, cur, mi, ma, content))
          {
            list->createItem(blocks[1], s, e, pg, minx,mi,maxx,ma,content);
            s = e + 1;
            e = endPos;
            if (s > endPos)
              break;
          }
        }
        else
        {
          int s = startPos;
          int tmp = blocks[0]->getLength() - 1;
          int e = tmp;
          while (blocks[0]->findReplaced(s, e, cur, mi, ma, content))
          {
            list->createItem(blocks[0], s, e, pg, minx,mi,maxx,ma,content);
            s = e + 1;
            e = tmp;
            if (s > tmp)
              break;
          }

          for (int i = 1; i < blocks.size() - 1; i++)
          {
            s = 0;
            tmp = blocks[i]->getLength() - 1;
            e = tmp;
            while (blocks[i]->findReplaced(s, e, cur, mi, ma, content))
            {
              list->createItem(blocks[i], s, e, pg, minx,mi,maxx,ma,content);
              s = e + 1;
              e = tmp;
              if (s > tmp)
                break;
            }
          }

          s = 0;
          e = endPos;
          while (blocks[1]->findReplaced(s, e, cur, mi, ma, content))
          {
            list->createItem(blocks[1], s, e, pg, minx,mi,maxx,ma,content);
            s = e + 1;
            e = endPos;
            if (s > endPos)
              break;
          }
        }
      }
      break;

    case TEX_DOC_WD_TLT:
      {
        cur = minx;
        if (blocks.size() == 1)
        {
          int s = startPos;
          int e = endPos;
          while (blocks[0]->findReplaced(s, e, cur, mi, ma, content))
          {
            list->createItem(blocks[0], s, e, pg, mi,miny,ma,maxy,content);
            s = e + 1;
            e = endPos;
            if (s > endPos)
              break;
          }
        }
        else if (blocks.size() == 2)
        {
          int s = startPos;
          int tmp = blocks[0]->getLength() - 1;
          int e = tmp;
          while (blocks[0]->findReplaced(s, e, cur, mi, ma, content))
          {
            list->createItem(blocks[0], s, e, pg, mi,miny,ma,maxy,content);
            s = e + 1;
            e = tmp;
            if (s > tmp)
              break;
          }

          s = 0;
          e = endPos;
          while (blocks[1]->findReplaced(s, e, cur, mi, ma, content))
          {
            list->createItem(blocks[1], s, e, pg, mi,miny,ma,maxy,content);
            s = e + 1;
            e = endPos;
            if (s > endPos)
              break;
          }
        }
        else
        {
          int s = startPos;
          int tmp = blocks[0]->getLength() - 1;
          int e = tmp;
          while (blocks[0]->findReplaced(s, e, cur, mi, ma, content))
          {
            list->createItem(blocks[0], s, e, pg, mi,miny,ma,maxy,content);
            s = e + 1;
            e = tmp;
            if (s > tmp)
              break;
          }

          for (int i = 1; i < blocks.size() - 1; i++)
          {
            s = 0;
            tmp = blocks[i]->getLength() - 1;
            e = tmp;
            while (blocks[i]->findReplaced(s, e, cur, mi, ma, content))
            {
              list->createItem(blocks[i], s, e, pg, mi,miny,ma,maxy,content);
              s = e + 1;
              e = tmp;
              if (s > tmp)
                break;
            }
          }

          s = 0;
          e = endPos;
          while (blocks[1]->findReplaced(s, e, cur, mi, ma, content))
          {
            list->createItem(blocks[1], s, e, pg, mi,miny,ma,maxy,content);
            s = e + 1;
            e = endPos;
            if (s > endPos)
              break;
          }
        }
      }
      break;

    case TEX_DOC_WD_TRT:
      {
        cur = maxx;
        if (blocks.size() == 1)
        {
          int s = startPos;
          int e = endPos;
          while (blocks[0]->findReplaced(s, e, cur, mi, ma, content))
          {
            list->createItem(blocks[0], s, e, pg, mi,miny,ma,maxy,content);
            s = e + 1;
            e = endPos;
            if (s > endPos)
              break;
          }
        }
        else if (blocks.size() == 2)
        {
          int s = startPos;
          int tmp = blocks[0]->getLength() - 1;
          int e = tmp;
          while (blocks[0]->findReplaced(s, e, cur, mi, ma, content))
          {
            list->createItem(blocks[0], s, e, pg, mi,miny,ma,maxy,content);
            s = e + 1;
            e = tmp;
            if (s > tmp)
              break;
          }

          s = 0;
          e = endPos;
          while (blocks[1]->findReplaced(s, e, cur, mi, ma, content))
          {
            list->createItem(blocks[1], s, e, pg, mi,miny,ma,maxy,content);
            s = e + 1;
            e = endPos;
            if (s > endPos)
              break;
          }
        }
        else
        {
          int s = startPos;
          int tmp = blocks[0]->getLength() - 1;
          int e = tmp;
          while (blocks[0]->findReplaced(s, e, cur, mi, ma, content))
          {
            list->createItem(blocks[0], s, e, pg, mi,miny,ma,maxy,content);
            s = e + 1;
            e = tmp;
            if (s > tmp)
              break;
          }

          for (int i = 1; i < blocks.size() - 1; i++)
          {
            s = 0;
            tmp = blocks[i]->getLength() - 1;
            e = tmp;
            while (blocks[i]->findReplaced(s, e, cur, mi, ma, content))
            {
              list->createItem(blocks[i], s, e, pg, mi,miny,ma,maxy,content);
              s = e + 1;
              e = tmp;
              if (s > tmp)
                break;
            }
          }

          s = 0;
          e = endPos;
          while (blocks[1]->findReplaced(s, e, cur, mi, ma, content))
          {
            list->createItem(blocks[1], s, e, pg, mi,miny,ma,maxy,content);
            s = e + 1;
            e = endPos;
            if (s > endPos)
              break;
          }
        }
      }
      break;
  }

  return true;
}

bool XWTeXDocumentRow::findNext()
{
  if (index == 0)
  {
    if (blocks.size() == 1)
    {
      if (blocks[index]->findNext(startPos, endPos))
      {
        moveToHitPos();
        return true;
      }

      return false;
    }
    else
    {
      if (blocks[index]->findNext(startPos, blocks[index]->getLength() - 1))
      {
        moveToHitPos();
        return true;
      }
    }

    index++;
  }

  if (index == blocks.size() - 1)
  {
    if (blocks.size() == 2)
    {
      if (blocks[index]->findNext(0, endPos))
      {
        moveToHitPos();
        return true;
      }
    }

    index++;
  }

  if (index >= blocks.size())
  {
    index = 0;
    return false;
  }

  if (blocks[index]->findNext(0, blocks[index]->getLength() - 1))
  {
    moveToHitPos();
    return true;
  }

  return false;
}

bool XWTeXDocumentRow::hitTest(const QPointF & p)
{
  double curx = 0.0;
  double cury = 0.0;
  switch (dir)
  {
    case TEX_DOC_WD_TLT:
      if (p.y() < miny || p.y() > maxy)
        return false;
      if (p.x() < minx)
      {
        moveToStart();
        return true;
      }

      if (p.x() > maxx)
      {
        moveToEnd();
        return true;
      }
      curx = minx;
      cury = maxy;
      break;

    case TEX_DOC_WD_RTT:
      if (p.x() < minx || p.x() > maxx)
        return false;
      if (p.y() < miny)
      {
        moveToStart();
        return true;
      }
      if (p.y() > maxy)
      {
        moveToEnd();
        return true;
      }
      curx = minx;
      cury = miny;
      break;

    case TEX_DOC_WD_LTL:
      if (p.x() < minx || p.x() > maxx)
        return false;
      if (p.y() < miny)
      {
        moveToStart();
        return true;
      }
      if (p.y() > maxy)
      {
        moveToEnd();
        return true;
      }
      curx = maxx;
      cury = miny;
      break;

    case TEX_DOC_WD_TRT:
      if (p.y() < miny || p.y() > maxy)
        return false;
      if (p.x() < minx)
      {
        moveToEnd();
        return true;
      }

      if (p.x() > maxx)
      {
        moveToStart();
        return true;
      }
      curx = maxx;
      cury = maxy;
      break;

    default:
      break;
  }

  curBlock = blocks[0];
  if (blocks.size() == 1)
    return curBlock->hitTest(p,curx,cury,startPos,endPos);

  if (blocks.size() == 2)
  {
    if (curBlock->hitTest(p,curx,cury,startPos))
      return true;

    curBlock = blocks[1];
    return curBlock->hitTest(p,curx,cury,0,endPos);
  }

  if (curBlock->hitTest(p,curx,cury,startPos))
    return true;

  for (int i = 1; i < (blocks.size() - 1); i++)
  {
    curBlock = blocks[i];
    if (curBlock->hitTest(p,curx,cury))
      return true;
  }

  curBlock = blocks[blocks.size() - 1];
  return curBlock->hitTest(p,curx,cury,0,endPos);
}

void XWTeXDocumentRow::moveToEnd()
{
  double curx = 0.0;
  double cury = 0.0;
  switch (dir)
  {
    case TEX_DOC_WD_TLT:
      curx = maxx;
      cury = maxy;
      break;

    case TEX_DOC_WD_RTT:
      curx = minx;
      cury = maxy;
      break;

    case TEX_DOC_WD_LTL:
      curx = maxx;
      cury = maxy;
      break;

    case TEX_DOC_WD_TRT:
      curx = minx;
      cury = maxy;
      break;

    default:
      break;
  }

  curBlock = blocks[blocks.size()-1];
  curBlock->moveToRowEnd(curx,cury,endPos);
}

void XWTeXDocumentRow::moveToHitPos()
{
  double curx = 0.0;
  double cury = 0.0;
  switch (dir)
  {
    case TEX_DOC_WD_LTL:
      curx = maxx;
      cury = miny;
      break;

    case TEX_DOC_WD_TLT:
      curx = minx;
      cury = maxy;
      break;

    case TEX_DOC_WD_RTT:
      curx = minx;
      cury = miny;
      break;

    default:
      curx = maxx;
      cury = maxy;
      break;
  }

  if (blocks.size() == 1)
    curBlock->moveToHitPos(curx,cury,startPos,endPos);
  else
  {
    int k = blocks.indexOf(curBlock);
    if (k == 0)
      curBlock->moveToHitPos(curx,cury,startPos);
    else
    {
      if (k == blocks.size() - 1)
        curBlock->moveToHitPos(curx,cury,0,endPos);
      else
        curBlock->moveToHitPos(curx,cury);
    }
  }
}

bool XWTeXDocumentRow::moveToNextChar(bool & m)
{
  m = true;
  int k = blocks.indexOf(curBlock);
  if (curBlock->isAtEnd())
  {
    if (k == blocks.size() - 1)
    {
      m = false;
      return false;
    }

    curBlock = blocks[++k];
  }

  curBlock->moveToNextChar();
  int pos = curBlock->getCurrentPos();
  if (k == (blocks.size() - 1))
  {
    if (pos > endPos)
      return false;
  }

  moveToHitPos();
  return true;
}

bool XWTeXDocumentRow::moveToNextWord(bool & m)
{
  m = true;
  int k = blocks.indexOf(curBlock);
  if (curBlock->isAtEnd())
  {
    if (k == blocks.size() - 1)
    {
      m = false;
      return false;
    }

    curBlock = blocks[++k];
  }

  curBlock->moveToNextWord();
  int pos = curBlock->getCurrentPos();
  if (k == (blocks.size() - 1))
  {
    if (pos > endPos)
      return false;
  }

  moveToHitPos();
  return true;
}

bool XWTeXDocumentRow::moveToPreviousChar(bool & m)
{
  m = true;
  int k = blocks.indexOf(curBlock);
  if (curBlock->isAtStart())
  {
    if (k == 0)
    {
      m = false;
      return false;
    }

    curBlock = blocks[--k];
  }

  curBlock->moveToPreviousChar();
  int pos = curBlock->getCurrentPos();
  if (k == 0)
  {
    if (pos < startPos)
      return false;
  }

  moveToHitPos();
  return true;
}

bool XWTeXDocumentRow::moveToPreviousWord(bool & m)
{
  m = true;
  int k = blocks.indexOf(curBlock);
  if (curBlock->isAtStart())
  {
    if (k == 0)
    {
      m = false;
      return false;
    }

    curBlock = blocks[--k];
  }

  curBlock->moveToPreviousWord();
  int pos = curBlock->getCurrentPos();
  if (k == 0)
  {
    if (pos < startPos)
      return false;
  }

  moveToHitPos();
  return true;
}

void XWTeXDocumentRow::moveToStart()
{
  double curx = 0.0;
  double cury = 0.0;
  switch (dir)
  {
    case TEX_DOC_WD_LTL:
      curx = maxx;
      cury = miny;
      break;

    case TEX_DOC_WD_TLT:
      curx = minx;
      cury = maxy;
      break;

    case TEX_DOC_WD_RTT:
      curx = minx;
      cury = miny;
      break;

    default:
      curx = maxx;
      cury = maxy;
      break;
  }

  curBlock = blocks[0];
  curBlock->moveToRowStart(curx,cury,startPos);
}

bool XWTeXDocumentRow::replaceNext()
{
  if (index == 0)
  {
    if (blocks.size() == 1)
      return blocks[index]->replaceNext(startPos, endPos);
    else
    {
      if (blocks[index]->replaceNext(startPos, blocks[index]->getLength()))
        return true;
    }

    index++;
  }

  if (index == blocks.size() - 1)
  {
    if (blocks.size() == 2)
    {
      if (blocks[index]->replaceNext(0, endPos))
        return true;
    }

    index++;
  }

  if (index >= blocks.size())
  {
    index = 0;
    return false;
  }

  return blocks[index]->replaceNext(0, blocks[index]->getLength());
}

void XWTeXDocumentRow::reset()
{
  int i = 0;
  if (startPos > 0)
    i++;

  for (; i < blocks.size(); i++)
    blocks[i]->resetSelect();

  index = 0;
  curBlock = blocks[index];
}

void XWTeXDocumentRow::selectEnd(double & minX, double & minY, double & maxX, double & maxY)
{
  getBoundRect(minX,minY,maxX,maxY);
  curBlock = blocks[blocks.size() - 1];
  curBlock->selectForward(endPos);
  moveToHitPos();
}

int XWTeXDocumentRow::selectNextChar(double & minX, double & minY, double & maxX, double & maxY)
{
  int k = blocks.indexOf(curBlock);
  if (curBlock->isAtEnd())
  {
    if (k == (blocks.size() - 1))
      return -1;

    k++;
    curBlock = blocks[k];
  }

  getBoundRect(minX,minY,maxX,maxY);
  curBlock->selectNextChar();
  if (k == (blocks.size() - 1))
  {
    if (curBlock->getCurrentPos() > endPos)
      return 1;
  }
  moveToHitPos();
  return 0;
}

int XWTeXDocumentRow::selectNextWord(double & minX, double & minY, double & maxX, double & maxY)
{
  int k = blocks.indexOf(curBlock);
  if (curBlock->isAtEnd())
  {
    if (k == (blocks.size() - 1))
      return -1;

    k++;
    curBlock = blocks[k];
  }

  getBoundRect(minX,minY,maxX,maxY);
  curBlock->selectNextWord();
  if (k == (blocks.size() - 1))
  {
    if (curBlock->getCurrentPos() > endPos)
      return 1;
  }

  moveToHitPos();
  return 0;
}

int XWTeXDocumentRow::selectPreviousChar(double & minX, double & minY, double & maxX, double & maxY)
{
  int k = blocks.indexOf(curBlock);
  if (curBlock->isAtStart())
  {
    if (k == 0)
      return -1;

    k--;
    curBlock = blocks[k];
  }

  getBoundRect(minX,minY,maxX,maxY);
  curBlock->selectPreviousChar();
  if (k == 0)
  {
    if (curBlock->getCurrentPos() < startPos)
      return 1;
  }
  moveToHitPos();
  return 0;
}

int XWTeXDocumentRow::selectPreviousWord(double & minX, double & minY, double & maxX, double & maxY)
{
  int k = blocks.indexOf(curBlock);
  if (curBlock->isAtStart())
  {
    if (k == 0)
      return -1;

    k--;
    curBlock = blocks[k];
  }

  getBoundRect(minX,minY,maxX,maxY);
  curBlock->selectPreviousWord();
  if (k == 0)
  {
    if (curBlock->getCurrentPos() < startPos)
      return 1;
  }

  moveToHitPos();
  return 0;
}

void XWTeXDocumentRow::selectRow(double & minX, double & minY, double & maxX, double & maxY)
{
  getBoundRect(minX,minY,maxX,maxY);
  double curx = 0.0;
  double cury = 0.0;
  switch (dir)
  {
    case TEX_DOC_WD_LTL:
      curx = maxx;
      cury = miny;
      break;

    case TEX_DOC_WD_TLT:
      curx = minx;
      cury = maxy;
      break;

    case TEX_DOC_WD_RTT:
      curx = minx;
      cury = miny;
      break;

    default:
      curx = maxx;
      cury = maxy;
      break;
  }
  if (blocks.size() == 1)
  {
    blocks[0]->selectBack(startPos);
    blocks[0]->selectForward(endPos);
    curBlock = blocks[0];
  }
  else if (blocks.size() == 2)
  {
    blocks[0]->selectBack(startPos);
    blocks[0]->selectEndOfBlock();
    blocks[1]->selectStartOfBlock();
    blocks[1]->selectForward(endPos);
    curBlock = blocks[1];
  }
  else
  {
    blocks[0]->selectBack(startPos);
    blocks[0]->selectEndOfBlock();
    for (int i = 1; i < (blocks.size() - 1); i++)
      blocks[i]->selectBlock();
    blocks[blocks.size() - 1]->selectStartOfBlock();
    blocks[blocks.size() - 1]->selectForward(endPos);
    curBlock = blocks[blocks.size() - 1];
  }
  moveToHitPos();
}

bool XWTeXDocumentRow::selectWord(double & minX, double & minY, double & maxX, double & maxY)
{
  getBoundRect(minX,minY,maxX,maxY);
  curBlock->selectWord();
  moveToHitPos();
  return true;
}

void XWTeXDocumentRow::selectStart(double & minX, double & minY, double & maxX, double & maxY)
{
  getBoundRect(minX,minY,maxX,maxY);
  curBlock = blocks[0];
  curBlock->selectBack(startPos);
  moveToHitPos();
}

void XWTeXDocumentRow::setSelected(const QRectF & rect)
{
  QRectF r(minx,miny,maxx-minx,maxy-miny);
  if (!r.intersects(rect))
    return ;

  r = r.intersected(rect);
  double curx = 0.0;
  double cury = 0.0;
  switch (dir)
  {
    case TEX_DOC_WD_LTL:
      curx = maxx;
      cury = miny;
      break;

    case TEX_DOC_WD_TLT:
      curx = minx;
      cury = maxy;
      break;

    case TEX_DOC_WD_RTT:
      curx = minx;
      cury = miny;
      break;

    default:
      curx = maxx;
      cury = maxy;
      break;
  }

  if (blocks.size() == 1)
  {
    blocks[0]->setSelected(r,curx,cury,startPos,endPos);
    return ;
  }

  if (blocks.size() == 2)
  {
    blocks[0]->setSelected(r,curx,cury,startPos);
    blocks[1]->setSelected(r,curx,cury,0,endPos);
    return ;
  }

  blocks[0]->setSelected(r,curx,cury,startPos);
  for (int i = 1; i < (blocks.size() - 1); i++)
    blocks[i]->setSelected(r,curx,cury);

  blocks[blocks.size() - 1]->setSelected(r,curx,cury,0,endPos);
}

XWTeXDocumentPage::XWTeXDocumentPage()
 : maxRow(0),
   lastRow(0),
   curRow(0),
   rows(0)
{}

XWTeXDocumentPage::~XWTeXDocumentPage()
{
  if (rows)
  {
    for (int i = 0; i < maxRow; i++)
    {
      if (rows[i])
      {
        delete rows[i];
        rows[i] = 0;
      }
    }

    free(rows);
    rows = 0;
  }
}

void XWTeXDocumentPage::clear()
{
  for (int i = 0; i < maxRow; i++)
    rows[i]->clear();
  lastRow = -1;
  curRow = -1;
}

void XWTeXDocumentPage::draw(QPainter * painter,const QColor & pagecolor,const QRectF & rect)
{
  for (int i = 0; i <= lastRow; i++)
    rows[i]->draw(painter,pagecolor,rect);
}

void XWTeXDocumentPage::drawPic(QPainter * painter)
{
  for (int i = 0; i <= lastRow; i++)
    rows[i]->drawPic(painter);
}

void XWTeXDocumentPage::find(int pg, XWTeXDocSearhList * list)
{
  for (int i = 0; i <= lastRow; i++)
    rows[i]->find(pg, list);
}

void XWTeXDocumentPage::find(QList<XWTeXDocumentBlock*> & blocksA)
{
  for (int i = 0; i <= lastRow; i++)
    rows[i]->find(blocksA);
}

bool XWTeXDocumentPage::find(int pg, XWTeXDocumentBlock * blockA, 
                            XWTeXDocSearhList * list)
{
  bool ret = false;
  for (int i = 0; i <= lastRow; i++)
  {
    if (rows[i]->find(pg, blockA, list))
      ret = true;
    else
    {
      if (ret)
        break;
    }
  }

  return ret;
}

bool XWTeXDocumentPage::findNext()
{
  if (curRow > lastRow)
    curRow = 0;

  bool r = false;
  while (!r)
  {
    r = rows[curRow]->findNext();
    if (r)
      break;

    curRow++;
    if (curRow > lastRow)
      break;
  }

  return r;
}

XWTeXDocumentRow * XWTeXDocumentPage::getCurrentRow()
{
  if (maxRow <= 0)
  {
    maxRow = 25;
    rows = (XWTeXDocumentRow**)realloc(rows, maxRow * sizeof(XWTeXDocumentRow*));
    for (int i = 0; i < maxRow; i++)
      rows[i] = new XWTeXDocumentRow;
  }

  if (curRow < 0)
    curRow = 0;

  lastRow = curRow;
  return rows[lastRow];
}

XWTeXDocumentRow * XWTeXDocumentPage::getNewRow()
{
  XWTeXDocumentRow * row = 0;
  if (maxRow <= 0)
  {
    maxRow = 25;
    rows = (XWTeXDocumentRow**)realloc(rows, maxRow * sizeof(XWTeXDocumentRow*));
    for (int i = 0; i < maxRow; i++)
      rows[i] = new XWTeXDocumentRow;

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
      rows = (XWTeXDocumentRow**)realloc(rows, maxRow * sizeof(XWTeXDocumentRow*));
      for (int i = curRow; i < maxRow; i++)
        rows[i] = new XWTeXDocumentRow;
    }
    row = rows[lastRow];
    row->clear();
  }

  return row;
}

bool XWTeXDocumentPage::hitTest(const QPointF & p)
{
  for (int i = 0; i <= lastRow; i++)
  {
    if (rows[i]->hitTest(p))
    {
      curRow = i;
      return true;
    }
  }

  return false;
}

bool XWTeXDocumentPage::isEmpty()
{
  return lastRow == 0;
}

void XWTeXDocumentPage::moveToEnd()
{
  curRow = lastRow;
  rows[curRow]->moveToEnd();
}

void XWTeXDocumentPage::moveToFirstRow()
{
  curRow = 0;
  rows[curRow]->moveToStart();
}

void XWTeXDocumentPage::moveToLastRow()
{
  curRow = lastRow;
  rows[curRow]->moveToEnd();
}

bool XWTeXDocumentPage::moveToNextChar(bool & m)
{
  if (rows[curRow]->moveToNextChar(m))
    return true;

  if (m)
  {
    if (curRow >= lastRow)
      return false;

    curRow++;
    rows[curRow]->moveToHitPos();
    return true;
  }

  return false;
}

bool XWTeXDocumentPage::moveToNextRow()
{
  if (curRow >= lastRow)
    return false;

  rows[++curRow]->moveToStart();
  return true;
}

bool XWTeXDocumentPage::moveToNextWord(bool & m)
{
  if (rows[curRow]->moveToNextWord(m))
    return true;

  if (m)
  {
    if (curRow >= lastRow)
      return false;

    curRow++;
    rows[curRow]->moveToHitPos();
    return true;
  }

  return false;
}

bool XWTeXDocumentPage::moveToPreviousChar(bool & m)
{
  if (rows[curRow]->moveToPreviousChar(m))
    return true;

  if (m)
  {
    if (curRow == 0)
      return false;

    curRow--;
    rows[curRow]->moveToHitPos();
    return true;
  }

  return false;
}

bool XWTeXDocumentPage::moveToPreviousRow()
{
  if (curRow == 0)
    return false;

  rows[--curRow]->moveToEnd();
  return true;
}

bool XWTeXDocumentPage::moveToPreviousWord(bool & m)
{
  if (rows[curRow]->moveToPreviousWord(m))
    return true;

  if (m)
  {
    if (curRow == 0)
      return false;

    curRow--;
    rows[curRow]->moveToHitPos();
    return true;
  }

  return false;
}

void XWTeXDocumentPage::moveToRowEnd()
{
  rows[curRow]->moveToEnd();
}

void XWTeXDocumentPage::moveToRowStart()
{
  rows[curRow]->moveToStart();
}

void XWTeXDocumentPage::moveToStart()
{
  curRow = 0;
  rows[curRow]->moveToStart();
}

bool XWTeXDocumentPage::replaceNext()
{
  if (curRow > lastRow)
    curRow = 0;

  bool r = false;
  while (!r)
  {
    r = rows[curRow]->replaceNext();
    if (r)
      break;
      
    curRow++;
    if (curRow > lastRow)
      break;
  }

  return r;
}

void XWTeXDocumentPage::reset()
{
  curRow = 0;
  for (int i = 0; i <= lastRow; i++)
    rows[i]->reset();
}

void XWTeXDocumentPage::selectAll(double & minX, double & minY, double & maxX, double & maxY)
{
  for (int i = 0; i <= lastRow; i++)
    rows[i]->selectRow(minX,minY,maxX,maxY);
}

void XWTeXDocumentPage::selectEnd(double & minX, double & minY, double & maxX, double & maxY)
{
  rows[curRow]->selectEnd(minX,minY,maxX,maxY);
  for (int i = curRow + 1; i <= lastRow; i++)
    rows[i]->selectRow(minX,minY,maxX,maxY);
  curRow = lastRow;
}

void XWTeXDocumentPage::selectFirstRow(bool all, double & minX, double & minY, double & maxX, double & maxY)
{
  curRow = 0;
  if (all)
    rows[curRow]->selectRow(minX,minY,maxX,maxY);
  else
    rows[curRow]->moveToHitPos();
}

void XWTeXDocumentPage::selectLastRow(bool all, double & minX, double & minY, double & maxX, double & maxY)
{
  curRow = lastRow;
  if (all)
    rows[curRow]->selectRow(minX,minY,maxX,maxY);
  else
    rows[curRow]->moveToHitPos();
}

bool XWTeXDocumentPage::selectNextChar(double & minX, double & minY, double & maxX, double & maxY)
{
  int i = rows[curRow]->selectNextChar(minX,minY,maxX,maxY);
  if (i == 0)
    return true;

  if (curRow >= lastRow)
    return false;

  curRow++;
  if (i == 1)
  {
    rows[curRow]->moveToHitPos();
    return true;
  }

  rows[curRow]->selectNextChar(minX,minY,maxX,maxY);
  return true;
}

bool XWTeXDocumentPage::selectNextRow(double & minX, double & minY, double & maxX, double & maxY)
{
  if (curRow >= lastRow)
    return false;

  rows[++curRow]->selectRow(minX,minY,maxX,maxY);
  return true;
}

bool XWTeXDocumentPage::selectNextWord(double & minX, double & minY, double & maxX, double & maxY)
{
  int i = rows[curRow]->selectNextWord(minX,minY,maxX,maxY);
  if (i == 0)
    return true;

  if (curRow >= lastRow)
    return false;

  curRow++;
  if (i == 1)
  {
    rows[curRow]->moveToHitPos();
    return true;
  }

  rows[curRow]->selectNextWord(minX,minY,maxX,maxY);
  return true;
}

bool XWTeXDocumentPage::selectPreviousChar(double & minX, double & minY, double & maxX, double & maxY)
{
  int i = rows[curRow]->selectPreviousChar(minX,minY,maxX,maxY);
  if (i == 0)
    return true;

  if (curRow == 0)
      return false;
  curRow--;
  if (i == 1)
  {
    rows[curRow]->moveToHitPos();
    return true;
  }

  rows[curRow]->selectPreviousChar(minX,minY,maxX,maxY);
  return true;
}

bool XWTeXDocumentPage::selectPreviousRow(double & minX, double & minY, double & maxX, double & maxY)
{
  if (curRow == 0)
    return false;

  rows[--curRow]->selectRow(minX,minY,maxX,maxY);
  return true;
}

bool XWTeXDocumentPage::selectPreviousWord(double & minX, double & minY, double & maxX, double & maxY)
{
  int i = rows[curRow]->selectPreviousWord(minX,minY,maxX,maxY);
  if (i == 0)
    return true;

  if (curRow == 0)
    return false;
  curRow--;
  if (i == 1)
  {
    rows[curRow]->moveToHitPos();
    return true;
  }
  rows[curRow]->selectPreviousWord(minX,minY,maxX,maxY);
  return true;
}

void XWTeXDocumentPage::selectRow(double & minX, double & minY, double & maxX, double & maxY)
{
  rows[curRow]->selectRow(minX,minY,maxX,maxY);
}

void XWTeXDocumentPage::selectRowEnd(double & minX, double & minY, double & maxX, double & maxY)
{
  rows[curRow]->selectEnd(minX,minY,maxX,maxY);
}

void XWTeXDocumentPage::selectRowStart(double & minX, double & minY, double & maxX, double & maxY)
{
  rows[curRow]->selectStart(minX,minY,maxX,maxY);
}

bool XWTeXDocumentPage::selectWord(double & minX, double & minY, double & maxX, double & maxY)
{
  if (rows[curRow]->selectWord(minX,minY,maxX,maxY))
    return true;

  if (curRow < lastRow)
  {
    curRow++;
    rows[curRow]->moveToHitPos();
    return true;
  }

  return false;
}

void XWTeXDocumentPage::setSelected(const QRectF & rect)
{
  for (int i = 0; i <= lastRow; i++)
    rows[i]->setSelected(rect);
}

void XWTeXDocumentPage::selectStart(double & minX, double & minY, double & maxX, double & maxY)
{
  rows[curRow]->selectStart(minX,minY,maxX,maxY);
  for (int i = curRow - 1; i >= 0; i--)
    rows[i]->selectRow(minX,minY,maxX,maxY);
  curRow = 0;
}

XWTeXDocumentCursor::XWTeXDocumentCursor()
 : block(0),
   hitPos(0),
   startPos(0),
   endPos(0),
   selected(false)
{}

XWTeXDocumentCursor::XWTeXDocumentCursor(XWTeXDocumentBlock * blockA)
 : block(blockA),
   hitPos(0),
   startPos(0),
   endPos(0),
   selected(false)
{
}
