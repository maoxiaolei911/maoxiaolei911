/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QApplication>
#include <QtGui>
#include "XWTeXPool.h"
#include "TeXKeyWord.h"
#include "XWPGFPool.h"
#include "PGFKeyWord.h"
#include "XWPSTricksPool.h"
#include "PSTricksKeyWord.h"
#include "XWLaTeXPool.h"
#include "LaTeXKeyWord.h"
#include "XWBeamerPool.h"
#include "BeamerKeyWord.h"
#include "XWPSTricksObject.h"
#include "XWTikzObject.h"
#include "XWTeXDocumentPage.h"
#include "XWLaTeXDocumentObject.h"
#include "XWBeamerObject.h"
#include "XWTeXDocumentUndoCommand.h"
#include "XWLaTeXDocument.h"

XWLaTeXDocument::XWLaTeXDocument(QObject * parent)
:XWTeXDocument(parent),
 classIndex(XW_LTX_DOC_CLS_ARTICLE),
 showSectioning(0),
 firstComment(0),
 documentClass(0),
 titleM(0),
 authorM(0),
 dateM(0),
 thanksM(0),
 nameM(0),
 signatureM(0),
 telephoneM(0),
 locationM(0),
 addressM(0),
 subtitleM(0),
 instituteM(0),
 documentEnv(0),
 curPart(0),
 curChapter(0),
 curSection(0),
 curSubsection(0),
 curSubsubsection(0),
 curParagraph(0),
 curSubparagraph(0)
{
  selectType = XW_LTX_INS_TEXT;
}

XWLaTeXDocument::~XWLaTeXDocument()
{}

void XWLaTeXDocument::alignEnv(int id)
{
  if (!curCursor)
    return ;

  if (getCurrenrObjectType() == id)
    return ;

  QUndoCommand * pcmd = 0;
  XWTeXDocumentBlock * block = curCursor->getBlock();
  if (selected.isEmpty() && block->type() == XW_LTX_ROW)
  {    
    XWTeXDocumentObject * pobj = block->getObject();
    XWTeXDocumentObject * obj = createObject(id, pobj->parent());
    QUndoCommand * pcmd = 0;
    pcmd = new XWTeXDocumentNewParent(obj,pobj); 
  }
  else
  {
    pcmd = new QUndoCommand;
    getSelectedPars();
    XWTeXDocumentObject * obj = 0;
    if (selectedObjs.isEmpty())
    {
      XWTeXDocumentObject * pobj = block->getObject();
      selectedObjs << (XWTeXDocumentObject*)(pobj->parent());
    }

    int size = selectedObjs.size();
    XWTeXDocumentObject * next = selectedObjs[size - 1]->next;
    XWTeXDocumentObject * prev = selectedObjs[0]->prev;
    if (!prev || prev->type() != LArealparagraph)
    {
      prev = new XWLTXDocumentPar(this,selectedObjs[0]->parent());
      prev->clear();      
      obj = createObject(id, prev);
      prev->append(obj);
      new XWTeXDocumentInsertObject(prev,selectedObjs[0]->prev,pcmd);
    }
    else
    {
      obj = createObject(id, prev);
      new XWTeXDocumentInsertObject(obj,prev->last,pcmd);
    }      

    for (int i = 0; i < size; i++)
    {
      QString str = selectedObjs[i]->getMimeData();
      if (str.length() > 0)
      {
        if (str[str.length() - 1] == QChar('\n'))
          str.remove(str.length() - 1, 1);

        if (str.length() > 0)
        {
          if (str[str.length() - 1] == QChar('\n'))
            str.remove(str.length() - 1, 1);
        }
      }
      XWTeXDocumentBlock * nblock = new XWLTXRow(this,obj);
      nblock->append(str);
      obj->append(nblock);
    }
    
    if (next && next->type() == LArealparagraph)
    {
      new XWTeXDocumentAppendChildren(prev,next->head,next->last,pcmd);
      new XWTeXDocumentRemoveChildren(selectedObjs[0],next,pcmd);
    }
    else
      new XWTeXDocumentRemoveChildren(selectedObjs[0],selectedObjs[size - 1],pcmd);
  }
  
  undoStack->push(pcmd);
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
  emit sectionChanged();
}

void XWLaTeXDocument::back()
{
  if (!curCursor)
    return ;

  if (selected.isEmpty())
  {
    XWTeXDocumentBlock * block = curCursor->getBlock();
    if (!block->isAtStart() && !block->isReadOnly())
    {
      QUndoCommand * cmd = new XWTeXDocumentDeleteChar(block, -1);
      undoStack->push(cmd);
      breakPage();
      isModified = true;
      emit modificationChanged(isModified);

      return ;
    }

    switch (block->type())
    {
      case XW_LTX_ROW:
        if (block->isEmpty())
        {
          if ((block->prev && block->prev->type() == XW_LTX_ROW) ||
              block->next && block->next->type() == XW_LTX_ROW)
          {
            QUndoCommand * cmd = new XWTeXDocumentRemoveObject(block);
            if (block->prev)
              block->prev->setCursorAtEnd();
            else
              block->next->setCursorAtStart();            
            undoStack->push(cmd);
            breakPage();
            isModified = true;
            emit modificationChanged(isModified);
          }
          else
          {
            XWTeXDocumentObject * obj = block->getObject();
            if (!obj->prev && !obj->next)
            {
              QUndoCommand * pcmd = new QUndoCommand;
              XWTeXDocumentText * txt = new XWTeXDocumentText(this,obj->parent());
              new XWTeXDocumentInsertObject(txt,obj,pcmd);
              new XWTeXDocumentRemoveObject(obj,pcmd);
              txt->setCursor();
              undoStack->push(pcmd);
              breakPage();
              isModified = true;
              emit modificationChanged(isModified);
            }
            else
            {
              QUndoCommand * cmd = new XWTeXDocumentRemoveObject(obj);
              if (obj->prev)
                obj->prev->setCursorAtEnd();
              else
                obj->next->setCursorAtStart();  
              undoStack->push(cmd);
              breakPage();
              isModified = true;
              emit modificationChanged(isModified);
            }
          }
        }
        else
        {
          if (block->prev && block->prev->type() == XW_LTX_ROW)
          {
            QUndoCommand * pcmd = new XWTeXDocumentMergeBlock(block);
            block->prev->setCursorAtEnd();
            undoStack->push(pcmd);
            breakPage();
            isModified = true;
            emit modificationChanged(isModified);
          }
        }
        break;

      case TEX_DOC_B_COMMENT:
        if (block->isEmpty())
        {
          QUndoCommand * cmd = new XWTeXDocumentRemoveObject(block);
          if (block->prev)
            block->prev->setCursorAtEnd();
          else
            block->next->setCursorAtStart();  
          undoStack->push(cmd);
          breakPage();
          isModified = true;
          emit modificationChanged(isModified);
        }
        break;

      case TEX_DOC_B_DISFOMULAR:
      case TEX_DOC_B_FOMULAR:
        if (block->isEmpty())
        {
          if (!block->prev && !block->next)
          {              
            QUndoCommand * pcmd = new QUndoCommand;
            XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(block->getObject()->parent());
            XWTeXDocumentText * txt = new XWTeXDocumentText(this,pobj);
            new XWTeXDocumentInsertObject(txt,block,pcmd);
            new XWTeXDocumentRemoveObject(block,pcmd);  
            txt->setCursor();
            undoStack->push(pcmd);
            breakPage();
            isModified = true;
            emit modificationChanged(isModified);
          }
          else
          {
            QUndoCommand * cmd = new XWTeXDocumentRemoveObject(block);
            if (block->prev)
              block->prev->setCursorAtEnd();
            else
              block->next->setCursorAtStart();  
            undoStack->push(cmd);
            breakPage();
            isModified = true;
            emit modificationChanged(isModified);
          }
        }
        else
        {
          XWTeXDocumentObject * obj = (XWTeXDocumentObject*)(block->getObject()->parent());
          if (obj->type() == LArealparagraph && obj->isAtStart())
          {
            if (obj->prev && obj->prev->type() == LArealparagraph)
            {
              QUndoCommand * pcmd = new XWTeXDocumentBack(obj);
              undoStack->push(pcmd);
              breakPage();
              isModified = true;
              emit modificationChanged(isModified);
            }
            else if (obj->parent() != this)
            {
              XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(obj->parent());
              if ((pobj->type() == LAitem || pobj->type() == BMcolumn) && pobj->isAtStart())
              {
                if (pobj->prev && (pobj->prev->type() == LAitem || pobj->prev->type() == BMcolumn))
                {
                  QUndoCommand * pcmd = new XWTeXDocumentBack(pobj);
                  undoStack->push(pcmd);
                  breakPage();
                  isModified = true;
                  emit modificationChanged(isModified);
                }
              }
            }
          }
        }
        break;

      case TEX_DOC_B_TEXT:
        if (block->isEmpty())
        {
          if (block->prev || block->next)
          {
            QUndoCommand * cmd = new XWTeXDocumentRemoveObject(block);
            if (block->prev)
              block->prev->setCursorAtEnd();
            else
              block->next->setCursorAtStart();  
            undoStack->push(cmd);
            breakPage();
            isModified = true;
            emit modificationChanged(isModified);
          }
          else
          {
            XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(block->parent());
            if (pobj->parent() == this || pobj->parent() == documentEnv)
            {
              QUndoCommand * cmd = new XWTeXDocumentRemoveObject(pobj);
              if (pobj->prev)
                pobj->prev->setCursorAtEnd();
              else
                pobj->next->setCursorAtStart();  
              undoStack->push(cmd);
              breakPage();
              isModified = true;
              emit modificationChanged(isModified);
            }
            else
            {
              XWTeXDocumentObject * ppobj = (XWTeXDocumentObject*)(pobj->parent());
              if (ppobj->isEmpty())
              {
                QUndoCommand * cmd = new XWTeXDocumentRemoveObject(ppobj);
                if (ppobj->prev)
                  ppobj->prev->setCursorAtEnd();
                else
                  ppobj->next->setCursorAtStart();  
                undoStack->push(cmd);
                breakPage();
                isModified = true;
                emit modificationChanged(isModified);
              }
              else
              {
                QUndoCommand * cmd = new XWTeXDocumentRemoveObject(pobj);
                undoStack->push(cmd);
                breakPage();
                isModified = true;
                emit modificationChanged(isModified);
              }                  
            }
          }
        }
        else if (block->prev)
        {
          QUndoCommand * cmd = new XWTeXDocumentRemoveObject(block->prev);
          undoStack->push(cmd);
          breakPage();
          isModified = true;
          emit modificationChanged(isModified);
        }
        else
        {
          XWTeXDocumentObject * obj = (XWTeXDocumentObject*)(block->getObject());
          if (obj->type() == LArealparagraph)
          {
            if (obj->isAtStart())
            {
              if (obj->prev && obj->prev->type() == LArealparagraph)
              {
                QUndoCommand * pcmd = new XWTeXDocumentBack(obj);
                if (obj->prev)
                  obj->prev->setCursorAtEnd();
                else
                  obj->next->setCursorAtStart();  
                undoStack->push(pcmd);
                breakPage();
                isModified = true;
                emit modificationChanged(isModified);
              }
              else
              {
                if (obj->parent() != this)
                {
                  XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(obj->parent());
                  if ((pobj->type() == LAitem || pobj->type() == BMcolumn) && pobj->isAtStart())
                  {
                    if (pobj->prev && (pobj->prev->type() == LAitem || pobj->prev->type() == BMcolumn))
                    {
                      QUndoCommand * pcmd = new XWTeXDocumentBack(pobj);
                      undoStack->push(pcmd);
                      breakPage();
                      isModified = true;
                      emit modificationChanged(isModified);
                    }
                  }
                }
              }
            }
          }
        }
        break;

      default:
        if (!block->isAtStart())
        {
          XWTeXDocumentObject * obj = block->getObject();
          if (obj->parent() == this || obj->parent() == documentEnv)
          {
            QUndoCommand * cmd = new XWTeXDocumentRemoveObject(obj);
            if (obj->prev)
              obj->prev->setCursorAtEnd();
            else
              obj->next->setCursorAtStart();  
            undoStack->push(cmd);
            breakPage();
            isModified = true;
            emit modificationChanged(isModified);
          }
          else
          {
            if (!obj->prev && !obj->next)
            {
              QUndoCommand * pcmd = new QUndoCommand;
              XWTeXDocumentText * txt = new XWTeXDocumentText(this,obj->parent());
              new XWTeXDocumentInsertObject(txt,obj,pcmd);
              new XWTeXDocumentRemoveObject(obj,pcmd);
              txt->setCursor();
              undoStack->push(pcmd);
              breakPage();
              isModified = true;
              emit modificationChanged(isModified);
            }
            else
            {
              QUndoCommand * cmd = new XWTeXDocumentRemoveObject(obj);
              if (obj->prev)
                obj->prev->setCursorAtEnd();
              else
                obj->next->setCursorAtStart(); 
              undoStack->push(cmd);
              breakPage();
              isModified = true;
              emit modificationChanged(isModified);
            }
          }
        }
        else
        {
          XWTeXDocumentObject * obj = block->getObject();
          if (obj->parent() != this && obj->parent() != documentEnv)
          {
            XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(obj->parent());
            if (pobj->type() == LArealparagraph)
            {
              if (pobj->isAtStart())
              {
                if (pobj->prev && pobj->prev->type() == LArealparagraph)
                {
                  QUndoCommand * pcmd = new XWTeXDocumentBack(pobj);
                  undoStack->push(pcmd);
                  breakPage();
                  isModified = true;
                  emit modificationChanged(isModified);
                }
                else if (pobj->parent() != this && pobj->parent() != documentEnv)
                {
                  XWTeXDocumentObject * ppobj = (XWTeXDocumentObject*)(pobj->parent());
                  if ((ppobj->type() == LAitem || ppobj->type() == BMcolumn) && ppobj->isAtStart())
                  {
                    if (ppobj->prev && (ppobj->prev->type() == LAitem || ppobj->prev->type() == BMcolumn))
                    {
                      QUndoCommand * pcmd = new XWTeXDocumentBack(ppobj);
                      undoStack->push(pcmd);
                      breakPage();
                      isModified = true;
                      emit modificationChanged(isModified);
                    }
                  }
                }
              }
            }
          }
        }
        break;
    }
  }
  else
    deleteObjs();
}

void XWLaTeXDocument::beamerBlock(int id)
{
  if (getCurrenrObjectType() == id)
    return ;

  getSelectedPars();
  if (selectedObjs.isEmpty())
  {
    XWTeXDocumentBlock * block = curCursor->getBlock();
    XWTeXDocumentObject * tmp = block->getObject();
    selectedObjs << (XWTeXDocumentObject*)(tmp->parent());
  }

  QUndoCommand * pcmd = 0;
  XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(selectedObjs[0]->parent());
  XWTeXDocumentObject * obj = 0;
  if (pobj->type() == BMframe)
  {
    pcmd = new QUndoCommand;
    int size = selectedObjs.size();
    XWTeXDocumentObject * next = selectedObjs[size - 1]->next;
    XWTeXDocumentObject * prev = selectedObjs[0]->prev;
    if (prev && prev->type() == LArealparagraph)
    {
      obj = createObject(id, prev);
      new XWTeXDocumentInsertObject(obj,prev->last,pcmd);
    }
    else
    {
      prev = new XWLTXDocumentPar(this,pobj);
      prev->clear();
      obj = createObject(id, prev);
      prev->append(obj);
      new XWTeXDocumentInsertObject(prev,selectedObjs[0]->prev,pcmd);
    }

    obj->clear();
    new XWTeXDocumentAppendChildren(obj,selectedObjs[0],selectedObjs[size - 1],pcmd);
    if (next && next->type() == LArealparagraph)
    {
      new XWTeXDocumentAppendChildren(prev,next->head,next->last,pcmd);
      new XWTeXDocumentRemoveObject(next,pcmd);
    }
  }
  else
  {
    obj = createObject(id, pobj->parent());
    pcmd = new XWTeXDocumentNewParent(obj,pobj);
  }

  undoStack->push(pcmd);
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
  emit sectionChanged();
}

void XWLaTeXDocument::beamerBoxed(int id)
{
  beamerBlock(id);
}

void XWLaTeXDocument::beamerDynChange(int id)
{
  beamerBlock(id);
}

void XWLaTeXDocument::beamerColumns()
{
  getSelectedPars();
  if (selectedObjs.isEmpty())
  {
    XWTeXDocumentBlock * block = curCursor->getBlock();
    XWTeXDocumentObject * tmp = block->getObject();
    selectedObjs << (XWTeXDocumentObject*)(tmp->parent());
  }

  if (selectedObjs[0]->parent() == this ||
      selectedObjs[0]->parent() == documentEnv)
      return ;

  QUndoCommand * pcmd = new QUndoCommand;
  XWTeXDocumentObject * obj = 0;
  XWTeXDocumentObject * prev = selectedObjs[0]->prev;
  int size = selectedObjs.size();
  XWTeXDocumentObject * next = selectedObjs[size - 1]->next;
  if (prev && prev->type() == LArealparagraph)
  {
    obj = createObject(BMcolumns, prev);
    new XWTeXDocumentInsertObject(obj,prev->last,pcmd);
  }
  else
  {
    prev = new XWLTXDocumentPar(this,selectedObjs[0]->parent());
    prev->clear();
    obj = createObject(BMcolumns, prev);
    obj->clear();
    prev->append(obj);
    new XWTeXDocumentInsertObject(prev,selectedObjs[0]->prev,pcmd);
  }

  obj->clear();
  for (int i = 0; i < size; i++)
  {
    XWTeXDocumentObject * col = createObject(BMcolumn, obj);
    col->clear();
    new XWTeXDocumentAppendChild(col,selectedObjs[i],pcmd);
  }

  if (next && next->type() == LArealparagraph)
  {
    new XWTeXDocumentAppendChildren(prev,next->head,next->last,pcmd);
    new XWTeXDocumentRemoveObject(next,pcmd);
  }

  undoStack->push(pcmd);
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
  emit sectionChanged();
}

void XWLaTeXDocument::breakPage()
{
  lastPage = -1;
  curPage = -1;
  pageClear();
  isappendix = false;
  partCounter = 0;
  chapterCounter = 0;
  sectionCounter = 0;
  subsectionCounter = 0;
  subsubsectionCounter = 0;
  paragraphCounter = 0;
  subparagraphCounter = 0;
  itemCounter = 0;
  itemLevel = 0;
  bibitemCounter = 0;
  curPart = 0;
  resetSides();
  normalSize();
  normal();
  setItalic(false);
  bool firstcolumn = true;
  double curx,cury;
  switch (direction)
  {
    case TEX_DOC_WD_TLT:
    case TEX_DOC_WD_LTL:
      curx = getLeft();
      cury = getTop();
      break;

    default:
      curx = getRight();
      cury = getTop();
      break;
  }

  inPreamble = (documentEnv != 0) && showPreamble;

  XWTeXDocumentObject * obj = head;
  if (!showPreamble)
  {
    if (head == firstComment)
      obj = obj->next;

    if (documentEnv)
      obj = documentEnv;

    if (classIndex == XW_LTX_DOC_CLS_LETTER)
      makeLetterHead(curx,cury,firstcolumn);
    else if (classIndex != XW_LTX_DOC_CLS_BEAMER)
      makeTitle(curx,cury,firstcolumn);
  }

  while (obj)
  {
    obj->newRow = true;
    if (classIndex == XW_LTX_DOC_CLS_BEAMER && !showPreamble && !documentEnv)
    {
      if (obj->type() == BMframe)
      {
        obj->newPage = true;
        obj->breakPage(curx,cury,firstcolumn);
      }
    }
    else
    {
      obj->breakPage(curx,cury,firstcolumn);
      if (obj->type() == LAappendix)
        isappendix = true;
    }
    obj = obj->next;
  }

  popItalic();
  popSize();
  popWeight();

  emit pagesChanged();
}

bool XWLaTeXDocument::canFigure()
{
  if (!curCursor)
    return false;

  XWTeXDocumentBlock * block = curCursor->getBlock();
  XWTeXDocumentObject * obj = block->getObject();
  if (obj->parent() != this && obj->parent() != documentEnv)
  {
    XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(obj->parent());
    if (pobj->type() != LAfigure)
      return true;
  }

  return false;
}

bool XWLaTeXDocument::canFigureStar()
{
  if (!curCursor)
    return false;

  XWTeXDocumentBlock * block = curCursor->getBlock();
  XWTeXDocumentObject * obj = block->getObject();
  if (obj->parent() != this && obj->parent() != documentEnv)
  {
    XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(obj->parent());
    if (pobj->type() != LAfigureStar)
      return true;
  }

  return false;
}

bool XWLaTeXDocument::canFrameSec()
{
  if (classIndex != XW_LTX_DOC_CLS_BEAMER)
    return false;

  if (!curCursor)
    return false;

  XWTeXDocumentBlock * block = curCursor->getBlock();
  XWTeXDocumentObject * obj = block->getObject();
  while (obj->parent() != this && obj->parent() != documentEnv)
  {
    obj = (XWTeXDocumentObject*)(obj->parent());
  }

  if ((obj->isAtStart() || obj->isAtEnd()))
    return true;

  return false;
}

bool XWLaTeXDocument::canInsertFromMimeData(const QMimeData * /*source*/,
                                      int pageno,
                                      const QPointF & p)
{
  if (!hitTest(pageno, p))
    return false;

  XWTeXDocumentBlock * block = curCursor->getBlock();
//  XWTeXDocumentObject * obj = block->getObject();
  if (selectType == XW_LTX_INS_TEXT || selectType == XW_LTX_INS_OBJECTS)
  {
    if (!block->isReadOnly())
      return true;
  }
  else if (selectType == XW_LTX_INS_SEC)
  {
    XWTeXDocumentObject * obj = block->getObject();
    while (obj->parent() != this && obj->parent() != documentEnv)
    {
      obj = (XWTeXDocumentObject*)(obj->parent());
    }

    if (obj->isAtStart() || obj->isAtEnd())
      return true;
  }
  else if (selectType == XW_LTX_INS_PAR)
  {
    XWTeXDocumentObject * obj = block->getObject();
    while (obj->parent() != this && obj->parent() != documentEnv)
    {
      if (obj->type() == LArealparagraph)
      {
        if (obj->isAtStart() || obj->isAtEnd())
          return true;

        return false;
      }
      obj = (XWTeXDocumentObject*)(obj->parent());
    }

    if (obj->isAtStart() || obj->isAtEnd())
      return true;
  }
  else if (selectType == XW_LTX_INS_ITEM)
  {
    XWTeXDocumentObject * obj = block->getObject();
    while (obj->parent() != this && obj->parent() != documentEnv)
    {
      obj = (XWTeXDocumentObject*)(obj->parent());
      if (obj->type() == LAitem)
      {
        if (obj->isAtStart() || obj->isAtEnd())
          return true;

        return false;
      }
    }
  }
  else if (selectType == XW_LTX_INS_BIBITEM)
  {
    XWTeXDocumentObject * obj = block->getObject();
    while (obj->parent() != this && obj->parent() != documentEnv)
    {
      obj = (XWTeXDocumentObject*)(obj->parent());
      if (obj->type() == LAbibitem)
      {
        if (obj->isAtStart() || obj->isAtEnd())
          return true;

        return false;
      }
    }
  }

  return false;
}

bool XWLaTeXDocument::canInsertObject()
{
  if (!curCursor)
    return false;

  XWTeXDocumentBlock * block = curCursor->getBlock();
  if (!block->isReadOnly())
    return true;

  XWTeXDocumentObject * obj = block->getObject();
  if (obj->isAtStart() || obj->isAtEnd())
    return true;

  return false;
}

bool XWLaTeXDocument::canMatter()
{
  if (!curCursor)
    return false;

  XWTeXDocumentBlock * block = curCursor->getBlock();
  XWTeXDocumentObject * obj = block->getObject();
  while (obj->parent() != this && obj->parent() != documentEnv)
  {
    obj = (XWTeXDocumentObject*)(obj->parent());
  }

  if ((obj->isAtStart() || obj->isAtEnd()))
    return true;

  return false;
}

bool XWLaTeXDocument::canPaste()
{
  if (!curCursor)
    return false;

  XWTeXDocumentBlock * block = curCursor->getBlock();
  if ((selectType == XW_LTX_INS_TEXT || 
      selectType == XW_LTX_INS_OBJECTS) && 
      !block->isReadOnly())
    return true;

  if (selectType == XW_LTX_INS_PAR)
  {
    XWTeXDocumentObject * obj = block->getObject();
    while (obj->parent() != this && obj->parent() != documentEnv)
    {
      if (obj->type() == LArealparagraph)
      {
        if ((obj->isAtStart() || obj->isAtEnd()))
          return true;

        return false;
      }

      obj = (XWTeXDocumentObject*)(obj->parent());      
    }

    if ((obj->isAtStart() || obj->isAtEnd()))
      return true;
  }
  else if (selectType == XW_LTX_INS_SEC || selectType == XW_LTX_INS_FRAME)
  {
    XWTeXDocumentObject * obj = block->getObject();
    while (obj->parent() != this && obj->parent() != documentEnv)
    {
      if (obj->type() == BMframe)
      {
        if ((obj->isAtStart() || obj->isAtEnd()))
          return true;

        return false;
      }
      obj = (XWTeXDocumentObject*)(obj->parent());
    }

    if (obj->isAtStart() || obj->isAtEnd())
      return true;
  }
  else if (selectType == XW_LTX_INS_ITEM)
  {
    XWTeXDocumentObject * obj = block->getObject();
    while (obj->parent() != this && obj->parent() != documentEnv)
    {
      if (obj->type() == LAitem)
      {
        if (obj->isAtStart() || obj->isAtEnd())
          return true;

        return false;
      }
      obj = (XWTeXDocumentObject*)(obj->parent());      
    }
  }
  else if (selectType == XW_LTX_INS_BIBITEM)
  {
    XWTeXDocumentObject * obj = block->getObject();
    while (obj->parent() != this && obj->parent() != documentEnv)
    {
      if (obj->type() == LAbibitem)
      {
        if (obj->isAtStart() || obj->isAtEnd())
          return true;

        return false;
      }
      obj = (XWTeXDocumentObject*)(obj->parent());
    }
  }

  return false;
}

bool XWLaTeXDocument::canRemoveFloat()
{
  if (!curCursor)
    return false;

  XWTeXDocumentBlock * block = curCursor->getBlock();
  XWTeXDocumentObject * obj = block->getObject();
  if (obj->type() == LAfigure ||
      obj->type() == LAfigureStar ||
      obj->type() == LAtable)
  {
    return true;
  }

  if (obj->type() == LApicture ||
      obj->type() == LAincludegraphics ||
      obj->type() == PGFtikzpicture ||
      obj->type() == PSTpspicture ||
      obj->type() == LAtabular ||
      obj->type() == LAtabularStar)
  {
    obj = (XWTeXDocumentObject*)(obj->parent());
    if (obj->type() == LAfigure ||
        obj->type() == LAfigureStar ||
        obj->type() == LAtable)
    {
      return true;
    }
  }

  return false;
}

bool XWLaTeXDocument::canTable()
{
  if (!curCursor)
    return false;

  XWTeXDocumentBlock * block = curCursor->getBlock();
  XWTeXDocumentObject * obj = block->getObject();
  if (obj->type() == LAtabular ||
     obj->type() == LAtabularStar)
  {
    XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(obj->parent());
    if (pobj->type() != LAtable)
      return true;
  }

  return false;
}

QMenu * XWLaTeXDocument::createContextMenu()
{
  if (!curCursor)
    return 0;

  QMenu * menu = new QMenu;
  QAction * act = 0;

  QString str = QApplication::clipboard()->text();
  if (!str.isEmpty())
  {
    if (canPaste())
    {
      act = menu->addAction(tr("paste"));
      connect(act, SIGNAL(triggered()), this, SLOT(paste()));
    }
  }

  if (hasSelect())
  {
    if (act)
      menu->addSeparator();

    act = menu->addAction(tr("copy"));
    connect(act, SIGNAL(triggered()), this, SLOT(copy()));

    act = menu->addAction(tr("cut"));
    connect(act, SIGNAL(triggered()), this, SLOT(cut()));
  }

  XWTeXDocumentBlock * block = curCursor->getBlock();
  XWTeXDocumentObject * obj = block->getObject();
  if (obj->parent() != this && obj->parent() != documentEnv)
  {
    obj = (XWTeXDocumentObject*)(obj->parent());
    if (obj->type() == XW_LTX_PARAM)
    {
      obj = (XWTeXDocumentObject*)(obj->parent());
      if (obj->type() == LApart ||
          obj->type() == LAchapter ||
          obj->type() == LAsection ||
          obj->type() == LAsubsection ||
          obj->type() == LAsubsubsection ||
          obj->type() == LAparagraph ||
          obj->type() == LAsubparagraph)
      {
        if (act)
          menu->addSeparator();

        XWLTXStar * s = (XWLTXStar*)(obj->head->next);
        if (s->isEmpty())
        {
          act = menu->addAction(tr("not in table of contents"));
          connect(act, SIGNAL(triggered()), this, SLOT(star()));
        }
        else
        {
          act = menu->addAction(tr("in table of contents"));
          connect(act, SIGNAL(triggered()), this, SLOT(removeStar()));
        }

        menu->addSeparator();

        if (showSectioning == obj)
        {
          act = menu->addAction(tr("close short title"));
          connect(act, SIGNAL(triggered()), this, SLOT(closeShort()));
        }
        else
        {
          act = menu->addAction(tr("edit short title"));
          connect(act, SIGNAL(triggered()), this, SLOT(editShort()));
        }
      }
    }
    else if (block->type() == TEX_DOC_B_VERB)
    {
      obj = (XWTeXDocumentObject*)(block->parent());
      if (obj->type() == XW_TIKZ_CODE)
      {
        if (act)
          menu->addSeparator();
        createTikzPathMenu(menu);
      }
      else if (obj->type() == XW_TIKZ_OPTION ||
               obj->type() == XW_TIKZ_PARAM)
      {
        if (act)
          menu->addSeparator();

        createTikzOptionMenu(menu);
        menu->addSeparator();
        createTikzPathMenu(menu);
      }
      else if (obj->type() == XW_PSTRICKS_CODE)
      {
        if (act)
          menu->addSeparator();

        createPSTricksMenu(menu);
        createPSTricksOptionMenu(menu);
      }
    }
    else if (block->type() == XW_LTX_ROW)
    {
      obj = (XWTeXDocumentObject*)(block->parent());
      if (obj->type() == LAtabular)
      {
        if (act)
          menu->addSeparator();

        act = menu->addAction(tr("star"));
        connect(act, SIGNAL(triggered()), this, SLOT(tabularToStar()));
      }
      else if (obj->type() == LAtabularStar)
      {
        if (act)
          menu->addSeparator();

        act = menu->addAction(tr("remove star"));
        connect(act, SIGNAL(triggered()), this, SLOT(starToTabular()));
      }
      else if (obj->type() == LAeqnarray)
      {
        if (act)
          menu->addSeparator();

        act = menu->addAction(tr("star"));
        connect(act, SIGNAL(triggered()), this, SLOT(eqnarrayToStar()));
      }
      else if (obj->type() == LAeqnarrayStar)
      {
        act = menu->addAction(tr("remove star"));
        connect(act, SIGNAL(triggered()), this, SLOT(starToEqnarray()));
      }
    }
  }

  if (canFrameSec())
  {
    if (act)
      menu->addSeparator();
    QActionGroup * ag = new QActionGroup(menu);
    connect(ag, SIGNAL(triggered(QAction*)), this, SLOT(insertFrameSec(QAction*)));
    QMenu * cmenu = menu->addMenu(tr("Multimedia"));
    act = cmenu->addAction(tr("movie"));
    act->setData(BMmovie);
    ag->addAction(act);
    act = cmenu->addAction(tr("hyperlinkmovie"));
    act->setData(BMhyperlinkmovie);
    ag->addAction(act);
    cmenu->addSeparator();
    act = cmenu->addAction(tr("animate"));
    act->setData(BManimate);
    ag->addAction(act);
    act = cmenu->addAction(tr("animatevalue"));
    act->setData(BManimatevalue);
    ag->addAction(act);
    act = cmenu->addAction(tr("multiinclude"));
    act->setData(BMmultiinclude);
    ag->addAction(act);
    cmenu->addSeparator();
    act = cmenu->addAction(tr("sound"));
    act->setData(BMsound);
    ag->addAction(act);
    act = cmenu->addAction(tr("hyperlinksound"));
    act->setData(BMhyperlinksound);
    ag->addAction(act);
    act = cmenu->addAction(tr("hyperlinkmute"));
    act->setData(BMhyperlinkmute);
    ag->addAction(act);
    menu->addSeparator();
    cmenu = menu->addMenu(tr("Transition"));
    act = cmenu->addAction(tr("transblindshorizontal"));
    act->setData(BMtransblindshorizontal);
    ag->addAction(act);
    act = cmenu->addAction(tr("transblindsvertical"));
    act->setData(BMtransblindsvertical);
    ag->addAction(act);
    act = cmenu->addAction(tr("transboxin"));
    act->setData(BMtransboxin);
    ag->addAction(act);
    act = cmenu->addAction(tr("transboxout"));
    act->setData(BMtransboxout);
    ag->addAction(act);
    act = cmenu->addAction(tr("transdissolve"));
    act->setData(BMtransdissolve);
    ag->addAction(act);
    act = cmenu->addAction(tr("transglitter"));
    act->setData(BMtransglitter);
    ag->addAction(act);
    act = cmenu->addAction(tr("transreplace"));
    act->setData(BMtransreplace);
    ag->addAction(act);
    act = cmenu->addAction(tr("transsplitverticalin"));
    act->setData(BMtranssplitverticalin);
    ag->addAction(act);
    act = cmenu->addAction(tr("transsplitverticalout"));
    act->setData(BMtranssplitverticalout);
    ag->addAction(act);
    act = cmenu->addAction(tr("transsplithorizontalin"));
    act->setData(BMtranssplithorizontalin);
    ag->addAction(act);
    act = cmenu->addAction(tr("transsplithorizontalout"));
    act->setData(BMtranssplithorizontalout);
    ag->addAction(act);
    act = cmenu->addAction(tr("transwipe"));
    act->setData(BMtranswipe);
    ag->addAction(act);
    act = cmenu->addAction(tr("transduration"));
    act->setData(BMtransduration);
    ag->addAction(act);
    menu->addSeparator();
    act = menu->addAction(tr("frametitle"));
    act->setData(BMframetitle);
    ag->addAction(act);
    act = menu->addAction(tr("framesubtitle"));
    act->setData(BMframesubtitle);
    ag->addAction(act);
    menu->addSeparator();
    act = menu->addAction(tr("framezoom"));
    act->setData(BMframezoom);
    ag->addAction(act);
    menu->addSeparator();
    act = menu->addAction(tr("tableofcontents"));
    act->setData(BMtableofcontents);
    ag->addAction(act);
    act = menu->addAction(tr("appendix"));
    act->setData(BMappendix);
    ag->addAction(act);
    act = menu->addAction(tr("partpage"));
    act->setData(BMappendix);
    ag->addAction(act);
    act = menu->addAction(tr("titlepage"));
    act->setData(BMappendix);
    ag->addAction(act);
  }

  if (classIndex == XW_LTX_DOC_CLS_BEAMER)
  {
    if (act)
      menu->addSeparator();

    if (canInsertObject())
    {
      QActionGroup * ag = new QActionGroup(menu);
      connect(ag, SIGNAL(triggered(QAction*)), this, SLOT(insertObject(QAction*)));
      QMenu * cmenu = menu->addMenu(tr("Overlay"));
      act = cmenu->addAction(tr("pause"));
      act->setData(BMpause);
      ag->addAction(act);
      act = cmenu->addAction(tr("onslide"));
      act->setData(BMonslide);
      ag->addAction(act);
      act = cmenu->addAction(tr("only"));
      act->setData(BMonly);
      ag->addAction(act);
      act = cmenu->addAction(tr("uncover"));
      act->setData(BMuncover);
      ag->addAction(act);
      act = cmenu->addAction(tr("visible"));
      act->setData(BMvisible);
      ag->addAction(act);
      act = cmenu->addAction(tr("invisible"));
      act->setData(BMinvisible);
      ag->addAction(act);
      act = cmenu->addAction(tr("alt"));
      act->setData(BMalt);
      ag->addAction(act);
      act = menu->addAction(tr("action"));
      act->setData(BMaction);
      ag->addAction(act);
      menu->addSeparator();
      cmenu = menu->addMenu(tr("Button"));
      act = cmenu->addAction(tr("hypertarget"));
      act->setData(BMhypertarget);
      ag->addAction(act);
      act = cmenu->addAction(tr("beamerbutton"));
      act->setData(BMbeamerbutton);
      ag->addAction(act);
      act = cmenu->addAction(tr("beamergotobutton"));
      act->setData(BMbeamergotobutton);
      ag->addAction(act);
      act = cmenu->addAction(tr("beamerskipbutton"));
      act->setData(BMbeamerskipbutton);
      ag->addAction(act);
      act = cmenu->addAction(tr("beamerreturnbutton"));
      act->setData(BMbeamerreturnbutton);
      ag->addAction(act);
      menu->addSeparator();
      cmenu = menu->addMenu(tr("Hyperlink"));
      act = cmenu->addAction(tr("hyperlink"));
      act->setData(BMhyperlink);
      ag->addAction(act);
      act = cmenu->addAction(tr("hyperlinkslideprev"));
      act->setData(BMhyperlinkslideprev);
      ag->addAction(act);
      act = cmenu->addAction(tr("hyperlinkslidenext"));
      act->setData(BMhyperlinkslidenext);
      ag->addAction(act);
      act = cmenu->addAction(tr("hyperlinkframestart"));
      act->setData(BMhyperlinkframestart);
      ag->addAction(act);
      act = cmenu->addAction(tr("hyperlinkframeend"));
      act->setData(BMhyperlinkframeend);
      ag->addAction(act);
      act = cmenu->addAction(tr("hyperlinkframestartnext"));
      act->setData(BMhyperlinkframestartnext);
      ag->addAction(act);
      act = cmenu->addAction(tr("hyperlinkframeendprev"));
      act->setData(BMhyperlinkframeendprev);
      ag->addAction(act);
      act = cmenu->addAction(tr("hyperlinkpresentationstart"));
      act->setData(BMhyperlinkpresentationstart);
      ag->addAction(act);
      act = cmenu->addAction(tr("hyperlinkpresentationend"));
      act->setData(BMhyperlinkpresentationend);
      ag->addAction(act);
      act = cmenu->addAction(tr("hyperlinkappendixstart"));
      act->setData(BMhyperlinkappendixstart);
      ag->addAction(act);
      act = cmenu->addAction(tr("hyperlinkappendixend"));
      act->setData(BMhyperlinkappendixend);
      ag->addAction(act);
      act = cmenu->addAction(tr("hyperlinkdocumentstart"));
      act->setData(BMhyperlinkdocumentstart);
      ag->addAction(act);
      act = cmenu->addAction(tr("hyperlinkdocumentend"));
      act->setData(BMhyperlinkdocumentend);
      ag->addAction(act);
      menu->addSeparator();
      cmenu = menu->addMenu(tr("Highlighting"));
      act = cmenu->addAction(tr("structure"));
      act->setData(BMstructure);
      ag->addAction(act);
      act = cmenu->addAction(tr("alert"));
      act->setData(BMalert);
      ag->addAction(act);
      menu->addSeparator();
      act = menu->addAction(tr("includeslide"));
      act->setData(BMincludeslide);
      ag->addAction(act);
    }

    if (act)
      menu->addSeparator();

    act = menu->addAction(tr("insert frame at after"));
    connect(act, SIGNAL(triggered()), this, SLOT(insertFrameAtAfter()));
    act = menu->addAction(tr("insert frame at before"));
    connect(act, SIGNAL(triggered()), this, SLOT(insertFrameAtBefore()));

    menu->addSeparator();
    QActionGroup * sg = new QActionGroup(menu);
    connect(sg, SIGNAL(triggered(QAction*)), this, SLOT(insertSection(QAction*)));
    {
      QMenu * cmenu = menu->addMenu(tr("insert section"));
      act = cmenu->addAction(tr("part"));
      act->setData(LApart);
      sg->addAction(act);
      act = cmenu->addAction(tr("section"));
      act->setData(LAsection);
      sg->addAction(act);
      act = cmenu->addAction(tr("subsection"));
      act->setData(LAsubsection);
      sg->addAction(act);
      act = cmenu->addAction(tr("subsubsection"));
      act->setData(LAsubsubsection);
      sg->addAction(act);
    }
    menu->addSeparator();
    act = menu->addAction(tr("delete frame"));
    connect(act, SIGNAL(triggered()), this, SLOT(delFrame()));
  }

  if (canTable())
  {
    if (act)
      menu->addSeparator();

    act = menu->addAction(tr("float"));
    connect(act, SIGNAL(triggered()), this, SLOT(table()));
  }
  else if (canFigure())
  {
    if (act)
      menu->addSeparator();

    act = menu->addAction(tr("float"));
    connect(act, SIGNAL(triggered()), this, SLOT(figure()));
  }
  else if (canFigureStar())
  {
    if (act)
      menu->addSeparator();

    act = menu->addAction(tr("float for full-width"));
    connect(act, SIGNAL(triggered()), this, SLOT(figureStar()));
  }
  else if (canRemoveFloat())
  {
    if (act)
      menu->addSeparator();

    act = menu->addAction(tr("no float"));
    connect(act, SIGNAL(triggered()), this, SLOT(removeFloat()));
  }

  return menu;
}

QMimeData * XWLaTeXDocument::createMimeDataFromSelection()
{
  selectType = XW_LTX_INS_TEXT;

  if (selectedObjs.isEmpty())
    getSelectedObjs();

  QString str;
  for (int i = 0; i < selectedObjs.size(); i++)
  {
    QString tmp = selectedObjs[i]->getSelected();
    str += tmp;
  }

  for (int i = 0; i < selectedObjs.size(); i++)
  {
    if (selectedObjs[i]->parent() == this || selectedObjs[i]->parent() == documentEnv)
      selectType = XW_LTX_INS_SEC;
    else
    {
      switch (selectedObjs[i]->type())
      {
        case LAabstract:
        case LAtableofcontents:
        case LAappendix:
        case LApart:
        case LAchapter:
        case LAsection:
        case LAsubsection:
        case LAsubsubsection:
        case LAparagraph:
        case LAsubparagraph:
        case LAthebibliography:
        case BMframe:
        case BMlecture:
        case BMnote:
        case BMmode:
        case BMagainframe:
          selectType = XW_LTX_INS_SEC;
          break;

        case LAitem:
          selectType = XW_LTX_INS_ITEM;
          break;

        case LAbibitem:
          selectType = XW_LTX_INS_BIBITEM;
          break;

        case LArealparagraph:
          selectType = XW_LTX_INS_PAR;
          break;

        default:
          if (selectedObjs[i]->type() != TEX_DOC_B_TEXT)
            selectType = XW_LTX_INS_OBJECTS;
          break;
      }
    }

    if (selectedObjs[i]->type() != TEX_DOC_B_TEXT)
      break;
  }

  QMimeData *mimeData = new QMimeData;
  mimeData->setText(str);

  return mimeData;
}

XWTeXDocumentObject * XWLaTeXDocument::createObject(const QString & key,
                                                    const QString & str,
                                                    int & pos,
                                                    QObject * parent)
{
  int id = lookupLaTeXID(key);
  XWTeXDocumentObject * obj = 0;
  if (id == LAbegin)
  {
    int len = str.length();
    QString cs = XWTeXDocumentObject::scanEnviromentName(str,len,pos);
    id = lookupLaTeXID(cs);
    obj = createLaTeXObject(id, this, parent);
    if (!obj)
    {
      id = lookupBeamerID(cs);
      obj = createBeamerObject(id, this, parent);
    }

    if (!obj)
    {
      id = lookupPGFID(cs);
      obj = createTikzObject(id, this, parent);
    }

    if (!obj)
    {
      id = lookupPSTrickID(cs);
      obj = createPSTricksObject(id, this, parent);
    }

    if (!obj)
      obj = new XWTeXDocumentUnkownEnviroment(this, cs,parent);
  }
  else
  {
    obj = createLaTeXObject(id, this, parent);
    if (!obj)
    {
      id = lookupBeamerID(key);
      obj = createBeamerObject(id, this, parent);
    }

    if (!obj)
    {
      id = lookupPGFID(key);
      obj = createTikzObject(id, this, parent);
    }

    if (!obj)
    {
      id = lookupPSTrickID(key);
      obj = createPSTricksObject(id, this, parent);
    }

    if (!obj)
    {
      XWTeXControlSequence * block = new XWTeXControlSequence(this,parent);
      block->text = QString("\\%1").arg(key);
      obj = block;
    }
  }

  return obj;
}

XWTeXDocumentObject * XWLaTeXDocument::createObject(int id, QObject * parent)
{
  XWTeXDocumentObject * obj = createLaTeXObject(id, this,parent);
  if (!obj)
    obj = createBeamerObject(id, this, parent);

  if (!obj)
    obj = createTikzObject(id, this, parent);

  if (!obj)
    obj = createPSTricksObject(id, this, parent);

  return obj;
}

void XWLaTeXDocument::del()
{
  if (!curCursor)
    return ;

  if (!hasSelect())
  {
    XWTeXDocumentBlock * block = curCursor->getBlock();
    if (!block->isAtEnd() && !block->isReadOnly())
    {
      QUndoCommand * cmd = new XWTeXDocumentDeleteChar(block,1);
      undoStack->push(cmd);
      breakPage();
      isModified = true;
      emit modificationChanged(isModified);
      return ;
    }

    switch (block->type())
    {
      case XW_LTX_ROW:
        if (block->next && block->next->type() == XW_LTX_ROW)
        {
          QUndoCommand * pcmd = new XWTeXDocumentMergeBlock((XWTeXDocumentBlock*)(block->next));
          undoStack->push(pcmd);
          breakPage();
          isModified = true;
          emit modificationChanged(isModified);
        }
        break;

      case TEX_DOC_B_COMMENT:
        break;

      default:
        {
          XWTeXDocumentObject * obj = block->getObject();
          if (obj->parent() == this)
          {
            if (obj->isAtStart())
            {
              QUndoCommand * cmd = new XWTeXDocumentRemoveObject(obj);
              if (obj->prev)
                obj->prev->setCursorAtEnd();
              else
                obj->next->setCursorAtStart();
              undoStack->push(cmd);
              breakPage();
              isModified = true;
              emit modificationChanged(isModified);
            }
          }
          else
          {
            obj = (XWTeXDocumentObject*)(obj->parent());
            if (obj->isAtEnd())
            {
              if (obj->next && obj->next->type() == LArealparagraph)
              {
                QUndoCommand * cmd = new XWTeXDocumentBack(obj->next);
                undoStack->push(cmd);
                breakPage();
                isModified = true;
                emit modificationChanged(isModified);
              }
              else if (obj->parent() != this)
              {
                XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(obj->parent());
                if (pobj->isAtEnd())
                {
                  if (pobj->type() == LAitem || pobj->type() == BMcolumn)
                  {
                    if (pobj->next && (pobj->next->type() == LAitem || pobj->next->type() == BMcolumn))
                    {
                      QUndoCommand * cmd = new XWTeXDocumentBack(pobj->next);
                      undoStack->push(cmd);
                      breakPage();
                      isModified = true;
                      emit modificationChanged(isModified);
                    }
                  }
                }
              }            
            }
          }
        }
        break;
    }
  }
  else
    deleteObjs();
}

int XWLaTeXDocument::geCurrentBlockType()
{
  if (!curCursor)
    return -1;

  XWTeXDocumentBlock * block = curCursor->getBlock();
  return block->type();
}

int  XWLaTeXDocument::getCurrenrObjectType()
{
  if (!curCursor)
    return -1;

  XWTeXDocumentBlock * block = curCursor->getBlock();
  XWTeXDocumentObject * obj = block->getObject();
  if (obj->parent() == this || obj->parent() == documentEnv)
    return obj->type();

  XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(obj->parent());
  if (pobj->parent() == this || pobj->parent() == documentEnv)
    return pobj->type();

  if (pobj->type() == LArealparagraph)
  {
    XWTeXDocumentObject * ppobj = (XWTeXDocumentObject*)(pobj->parent());
    if (ppobj->type() == LAitem || ppobj->type() == LAbibitem)
    {
      XWTeXDocumentObject * pppobj = (XWTeXDocumentObject*)(ppobj->parent());
      return pppobj->type();
    }

    return ppobj->type();
  }

  return pobj->type();
}

QString XWLaTeXDocument::getFormular()
{
  if (!curCursor)
    return QString();

  XWTeXDocumentBlock * block = curCursor->getBlock();
  XWTeXDocumentObject * obj = block->getObject();
  QString str;
  if (obj->type() == TEX_DOC_B_FOMULAR || 
      obj->type() == TEX_DOC_B_DISFOMULAR)
  {
    if (!block->hasSelected())
      str = block->getText();
    else
      str = block->getSelected();
  }
  else if (obj->type() == LAarray || 
          obj->type() == LAeqnarray ||
          obj->type() == LAeqnarrayStar ||
          obj->type() == LAequation)
    str = obj->getMimeData();

  return str;
}

QString XWLaTeXDocument::getTikzPicture()
{
  if (!curCursor)
    return QString();

  XWTeXDocumentBlock * block = curCursor->getBlock();
  XWTeXDocumentObject * obj = block->getObject();
  QString str;
  if (obj->type() == PGFtikz || obj->type() == PGFtikzpicture)
  {
    if (block->hasSelected() && !block->isAllSelected())
      str = block->getSelected();
    else
      str = obj->getMimeData();
  }

  return str;
}

bool XWLaTeXDocument::hitTest(int pageno,const QPointF & p)
{
  if (pageno < 0)
    pageno = 0;

  if (pageno > lastPage)
    pageno = lastPage;

  curPage = pageno;
  XWTeXDocumentPage * pg = pages[curPage];
  XWLTXDocumentSectioning * oldsec = showSectioning;
  showSectioning = 0;
  curCursor = 0;
  bool r = pg->hitTest(p);
  if (r)
  {
    XWTeXDocumentBlock * block = curCursor->getBlock();
    XWTeXDocumentObject * obj = block->getObject();
    if (obj->parent() != this)
    {
      obj = (XWTeXDocumentObject*)(obj->parent());
      if (obj->type() == XW_LTX_PARAM)
      {
        obj = (XWTeXDocumentObject*)(obj->parent());
        if (obj == oldsec)
          showSectioning = oldsec;
      }
    }
  }
  emit sectionChanged();
  return r;
}

void XWLaTeXDocument::insertFromMimeData(const QMimeData * source,
                                      int /*pageno*/,
                                      const QPointF & /*p*/)
{
  QString str = source->text();
  insert(str);
}

bool XWLaTeXDocument::isFramePar()
{
  if (classIndex != XW_LTX_DOC_CLS_BEAMER || !curCursor)
    return false;

  XWTeXDocumentBlock * block = curCursor->getBlock();
  XWTeXDocumentObject * obj = block->getObject();
  if (obj->parent() == this || obj->parent() == documentEnv)
    return false;

  obj = (XWTeXDocumentObject*)(obj->parent());
  if (obj->parent() == this || obj->parent() == documentEnv || obj->type() != LArealparagraph)
    return false;

  XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(obj->parent());
  if (pobj->type() != BMframe)
    return false;

  return true;
}

bool XWLaTeXDocument::isMainFile()
{
  return (documentClass != 0);
}

bool XWLaTeXDocument::isTopObject(XWTeXDocumentObject * obj)
{
  if (obj == 0)
    return false;

  if (obj->parent() == this || obj->parent() == documentEnv)
    return true;

  return false;
}

void XWLaTeXDocument::itemEnv(int id)
{
  if (getCurrenrObjectType() == id)
    return ;

  getSelectedPars();
  if (selectedObjs.isEmpty())
  {
    XWTeXDocumentBlock * block = curCursor->getBlock();
    XWTeXDocumentObject * tmp = block->getObject();
    selectedObjs << (XWTeXDocumentObject*)(tmp->parent());
  }

  bool newparent = false;
  if (selectedObjs[0]->parent() != this && selectedObjs[0]->parent() != documentEnv)
  {
    XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(selectedObjs[0]->parent());
    if (pobj->type() == LAitem)
      newparent = true;
  }

  QUndoCommand * pcmd = 0;
  if (newparent)
  {
    XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(selectedObjs[0]->parent());
    pobj = (XWTeXDocumentObject*)(pobj->parent());
    XWTeXDocumentObject * obj = createObject(id, pobj->parent());
    pcmd = new XWTeXDocumentNewParent(obj,pobj);
  }
  else
  {
    pcmd = new QUndoCommand;
    XWTeXDocumentObject * obj = 0;
    int size = selectedObjs.size();
    XWTeXDocumentObject * prev = selectedObjs[0]->prev;
    XWTeXDocumentObject * next = selectedObjs[size - 1]->next;
    if (prev && prev->type() == LArealparagraph)
    {
      obj = createObject(id, prev);
      obj->clear();
      new XWTeXDocumentInsertObject(obj,prev->last);
    }
    else
    {
      prev = new XWLTXDocumentPar(this,selectedObjs[0]->parent());
      new XWTeXDocumentInsertObject(prev,selectedObjs[0]->prev);
      prev->clear();
      obj = createObject(id, prev);
      obj->clear();
      prev->append(obj);
    }

    for (int i = 0; i < size; i++)
    {
      XWTeXDocumentObject * item = new XWLTXDocumentItem(this,obj);
      item->clear();
      obj->append(item);      
      new XWTeXDocumentAppendChild(item,selectedObjs[i],pcmd);
    }

    if (next && next->type() == LArealparagraph)
    {
      new XWTeXDocumentAppendChildren(prev,next->head,next->last,pcmd);
      new XWTeXDocumentRemoveObject(next,pcmd);
    }      
  }

  undoStack->push(pcmd);
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
  emit sectionChanged();
}

void XWLaTeXDocument::keyInput(const QString & str)
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
            curCursor->setEndPos(pos);
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
      else if (str[0] == QChar('{') || 
               str[0] == QChar('}') || 
               str[0] == QChar('#') ||
               str[0] == QChar('&') ||
               str[0] == QChar('$') )
      {
        int id = lookupLaTeXID(str);
        QString tmp = QString("\\%1").arg(str);
        obj = new XWTeXControlSequence(id,this, tmp, block->parent());
        cmd = new XWTeXDocumentNewObject(obj);
      }
      else
      {
        XWLTXDocumentVerb * verb = new XWLTXDocumentVerb(this,block->parent());
        obj = verb;
        QString tmp = QString("+%1+").arg(str);
        verb->text.append(tmp);
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

void XWLaTeXDocument::matter(int id)
{
  QObject * p = this;
  switch (id)
  {
    case LAmaketitle:
    case LAtableofcontents:
    case LAappendix:
    case LAprintindex:
    case LAprintglossary:
    case LAthebibliography:
    case BMlecture:
    case BMagainframe:
    case BMmode:
    case BMAtBeginPart:
    case BMAtBeginLecture:
    case BMAtBeginNote:
    case BMAtEndNote:
    case BMnote:
    case BMframe:
      if (documentEnv)
        p = documentEnv;
      break;

    default:
      break;
  }

  XWTeXDocumentObject * obj = createObject(id, p);
  QUndoCommand * cmd = new XWTeXDocumentNewObject(obj);
  undoStack->push(cmd);
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
  emit sectionChanged();
}

void XWLaTeXDocument::newParagraph()
{
  if (!curCursor)
    return ;

  XWTeXDocumentBlock * block = curCursor->getBlock();
  QUndoCommand * cmd = 0;
  switch (block->type())
  {
    case XW_LTX_ROW:
      {
        if (block->isEmpty())
        {
          XWTeXDocumentObject * obj = block->getObject();
          if (obj->type() == LAverse ||
              obj->type() == LAarray ||
              obj->type() == LAeqnarray ||
              obj->type() == LAeqnarrayStar ||
              obj->type() == LAcenter ||
              obj->type() == LAflushleft ||
              obj->type() == LAflushright ||
              obj->type() == LAtabular ||
              obj->type() == LAtabularStar)
          {
            if (obj->isAtEnd() && (block->prev->type() == XW_LTX_ROW))
            {
              if (obj->next)
              {
                cmd = new XWTeXDocumentRemoveObject(block);
                obj->next->setCursor();
              }                
              else
              {
                cmd = new QUndoCommand;
                XWTeXDocumentObject * nblock = new XWTeXDocumentText(this,obj->parent());
                new XWTeXDocumentInsertObject(nblock,obj,cmd);
                new XWTeXDocumentRemoveObject(block,cmd);
                nblock->setCursor();
              }
            }
            else
            {
              XWTeXDocumentBlock * nblock = new XWLTXRow(this,block->parent());
              cmd = new XWTeXDocumentNewObject(nblock);
            }
          }
          else
          {
            XWTeXDocumentBlock * nblock = new XWLTXRow(this,block->parent());
            cmd = new XWTeXDocumentNewObject(nblock);
          }
        }
        else if (block->isAtStart() || block->isAtEnd())
        {
          XWTeXDocumentBlock * nblock = new XWLTXRow(this,block->parent());
          cmd = new XWTeXDocumentNewObject(nblock);
        }
        else
        {
          XWTeXDocumentBlock * nblock = new XWLTXRow(this,block->parent());
          cmd = new XWTeXDocumentSplitBlock(nblock,block);
          nblock->setCursor();
        }

        undoStack->push(cmd);
        breakPage();
        isModified = true;
        emit modificationChanged(isModified);
      }
      break;

    case TEX_DOC_B_VERB:
      {
        XWTeXDocumentObject * obj =block->getObject();
        if (obj->type() == XW_PSTRICKS_CODE ||
            obj->type() == XW_TIKZ_CODE ||
            obj->type() == XW_TIKZ_OPTION ||
            obj->type() == XW_TIKZ_PARAM ||
            obj->type() == XW_LTX_CODE ||
            obj->type() == XW_BEAMER_CODE)
        {
          if (block->isAtEnd())
          {
            int len = block->text.length();
            if (block->text[len - 1] != QChar('\n'))
              cmd = new XWTeXDocumentInsertString(block,"\n");
            else
            {
              if (obj->next)
              {
                cmd = new XWTeXDocumentDeleteChar(block,-1);
                obj->next->setCursor();
              }                
              else
              {
                cmd = new QUndoCommand;
                new XWTeXDocumentDeleteChar(block,-1,cmd);
                XWTeXDocumentBlock * nblock = new XWTeXDocumentText(this,obj->parent());
                new XWTeXDocumentInsertObject(nblock,obj,cmd);
                nblock->setCursor();
              }
            }
          }
          else
            cmd = new XWTeXDocumentInsertString(block,"\n");

          undoStack->push(cmd);
          breakPage();
          isModified = true;
          emit modificationChanged(isModified);
        }
      }
      break;

    case TEX_DOC_B_DISFOMULAR:
      {
        if (block->isAtEnd())
        {
          int len = block->text.length();
          if (block->text[len - 1] != QChar('\n'))
            cmd = new XWTeXDocumentInsertString(block,"\n");
          else
          {
            if (block->next)
            {
              cmd = new XWTeXDocumentDeleteChar(block,-1);
              block->next->setCursor();
            }              
            else
            {
              cmd = new QUndoCommand;
              new XWTeXDocumentDeleteChar(block,-1,cmd);
              XWTeXDocumentBlock * nblock = new XWTeXDocumentText(this,block->parent());
              new XWTeXDocumentInsertObject(nblock,block,cmd);
              nblock->setCursor();
            }
          }
        }
        else
          cmd = new XWTeXDocumentInsertString(block,"\n");

        undoStack->push(cmd);
        breakPage();
        isModified = true;
        emit modificationChanged(isModified);
      }
      break;

    case TEX_DOC_B_COMMENT:
      if (block->next)
        block->next->setCursor();
      break;

    default:
      {
        XWTeXDocumentObject * obj = block->getObject();
        if (obj->parent() == this)
        {
          if (!documentEnv)
          {
            XWTeXDocumentObject * newobj = 0;
            if (classIndex != XW_LTX_DOC_CLS_BEAMER)
              newobj = new XWLTXDocumentPar(this,this);
            else
              newobj = new XWBeamerFrameEnv(this,this);            
            QUndoCommand * cmd = new XWTeXDocumentNewObject(newobj);
            undoStack->push(cmd);
            breakPage();
            isModified = true;
            emit modificationChanged(isModified);
          }          
        }
        else if (obj->parent() == documentEnv)
        {
          XWTeXDocumentObject * newobj = 0;
          if (classIndex != XW_LTX_DOC_CLS_BEAMER)
            newobj = new XWLTXDocumentPar(this,documentEnv);
          else
            newobj = new XWBeamerFrameEnv(this,documentEnv); 
          QUndoCommand * cmd = new XWTeXDocumentNewObject(newobj);
          undoStack->push(cmd);
          breakPage();
          isModified = true;
          emit modificationChanged(isModified);
        }
        else
        {
          XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(obj->parent());
          if (pobj->parent() == this || pobj->parent() == documentEnv)
          {
            QUndoCommand * cmd = 0;
            XWTeXDocumentObject * newobj = 0;
            if (classIndex != XW_LTX_DOC_CLS_BEAMER)
            {
              newobj = new XWLTXDocumentPar(this,pobj->parent());
              if (pobj->isAtStart() || pobj->isAtEnd())
                cmd = new XWTeXDocumentNewObject(newobj);
              else
                cmd = new XWTeXDocumentNewPar(newobj,pobj,obj);
            }
            else
            {
              newobj = new XWBeamerFrameEnv(this,pobj->parent()); 
              cmd = new XWTeXDocumentNewObject(newobj);
            }
            
            undoStack->push(cmd);
            breakPage();
            isModified = true;
            emit modificationChanged(isModified);
          }
          else
          {
            XWTeXDocumentObject * ppobj = (XWTeXDocumentObject*)(pobj->parent());
            if (ppobj->type() == LAitem)
            {
              XWTeXDocumentObject * pppobj = (XWTeXDocumentObject*)(ppobj->parent());
              if (pppobj->isAtEnd() && ppobj->isEmpty())
              {
                QUndoCommand * cmd = 0;
                if (pppobj->next)
                {
                  cmd = new XWTeXDocumentRemoveObject(ppobj);
                  pppobj->next->setCursor();
                }                  
                else
                {
                  cmd = new QUndoCommand;
                  XWTeXDocumentBlock * nblock = new XWTeXDocumentText(this,pppobj->parent());
                  new XWTeXDocumentInsertObject(nblock,pppobj,cmd);
                  new XWTeXDocumentRemoveObject(ppobj,cmd);
                  nblock->setCursor();
                }
                undoStack->push(cmd);
                breakPage();
                isModified = true;
                emit modificationChanged(isModified);
              }
              else
              {
                QUndoCommand * cmd = 0;                
                if (ppobj->isAtEnd() || ppobj->isAtStart())
                {
                  XWTeXDocumentObject * item = new XWLTXDocumentItem(this,pppobj);
                  cmd = new XWTeXDocumentNewObject(item);
                }                  
                else if (pobj->isAtStart())
                {
                  XWTeXDocumentObject * item = new XWLTXDocumentItem(this,pppobj);
                  new XWTeXDocumentNewPar(item,ppobj,pobj);
                }                  
                else
                {
                  XWTeXDocumentObject * newobj = new XWLTXDocumentPar(this,ppobj);
                  QUndoCommand * cmd = new XWTeXDocumentNewPar(newobj,pobj,obj);
                  undoStack->push(cmd);
                  breakPage();
                  isModified = true;
                  emit modificationChanged(isModified);
                }
                undoStack->push(cmd);
                breakPage();
                isModified = true;
                emit modificationChanged(isModified);
              }              
            }
            else if (ppobj->type() == LAbibitem)
            {
              XWTeXDocumentObject * pppobj = (XWTeXDocumentObject*)(ppobj->parent());
              if (pppobj->isAtEnd() && ppobj->isEmpty())
              {
                QUndoCommand * cmd = new XWTeXDocumentRemoveObject(ppobj);
                if (pppobj->next)
                  pppobj->next->setCursor();
                undoStack->push(cmd);
                breakPage();
                isModified = true;
                emit modificationChanged(isModified);
              }
              else
              {
                if (ppobj->isAtEnd() || ppobj->isAtStart())
                {
                  XWTeXDocumentObject * item = new XWLTXDocumentBibitem(this,pppobj);
                  QUndoCommand * cmd = new XWTeXDocumentNewObject(item);
                  undoStack->push(cmd);
                  breakPage();
                  isModified = true;
                  emit modificationChanged(isModified);
                }
              }
            }
            else if (ppobj->type() == BMcolumn)
            {
              XWTeXDocumentObject * pppobj = (XWTeXDocumentObject*)(ppobj->parent());
              if (pppobj->isAtEnd() && ppobj->isEmpty())
              {
                QUndoCommand * cmd = new XWTeXDocumentRemoveObject(ppobj);
                if (pppobj->next)
                  pppobj->next->setCursor();
                undoStack->push(cmd);
                breakPage();
                isModified = true;
                emit modificationChanged(isModified);
              }
              else
              {
                QUndoCommand * cmd = 0;
                if (ppobj->isAtEnd() || ppobj->isAtStart())
                {
                  XWTeXDocumentObject * item = new XWBeamerColumn(this,pppobj);
                  cmd = new XWTeXDocumentNewObject(item);
                }                  
                else
                {
                  XWTeXDocumentObject * item = new XWBeamerColumn(this,pppobj);
                  new XWTeXDocumentNewPar(item,ppobj,pobj);
                }      
                undoStack->push(cmd);
                breakPage();
                isModified = true;
                emit modificationChanged(isModified);
              }
            }
            else
            {
              QUndoCommand * cmd = 0;
              if (ppobj->isAtEnd() && pobj->isEmpty())
              {
                if (ppobj->next)
                {
                  cmd = new XWTeXDocumentRemoveObject(pobj);
                  ppobj->next->setCursor();
                }                  
                else
                {
                  cmd = new QUndoCommand;
                  XWTeXDocumentBlock * nblock = new XWTeXDocumentText(this,ppobj->parent());
                  new XWTeXDocumentInsertObject(nblock,ppobj,cmd);
                  new XWTeXDocumentRemoveObject(pobj,cmd);
                  nblock->setCursor();
                }                
              }
              else
              {
                XWTeXDocumentObject * newobj = new XWLTXDocumentPar(this,ppobj);
                if (pobj->isAtEnd() || pobj->isAtStart())
                  cmd = new XWTeXDocumentNewObject(newobj);
                else
                  cmd = new XWTeXDocumentNewPar(newobj,pobj,obj);
              }
              undoStack->push(cmd);
              breakPage();
              isModified = true;
              emit modificationChanged(isModified);
            }            
          }          
        }        
      }
  }
}

void XWLaTeXDocument::parEnv(int id)
{
  if (getCurrenrObjectType() == id)
    return ;

  getSelectedPars();
  if (selectedObjs.isEmpty())
  {
    XWTeXDocumentBlock * block = curCursor->getBlock();
    XWTeXDocumentObject * tmp = block->getObject();
    selectedObjs << (XWTeXDocumentObject*)(tmp->parent());
  }

  bool parenv = false;
  if (selectedObjs[0]->parent() != this)
  {
    XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(selectedObjs[0]->parent());
    if (pobj != documentEnv && pobj->type() != BMframe)
      parenv = true;
  }

  QUndoCommand * pcmd = 0;
  XWTeXDocumentObject * obj = 0;
  if (parenv)
  {
    XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(selectedObjs[0]->parent());
    obj = createObject(id, pobj->parent());
    pcmd = new XWTeXDocumentNewParent(obj,pobj);
  }
  else
  {
    pcmd = new QUndoCommand;
    int size = selectedObjs.size();
    XWTeXDocumentObject * next = selectedObjs[size - 1]->next;
    XWTeXDocumentObject * prev = selectedObjs[0]->prev;
    if (prev && prev->type() == LArealparagraph)
    {
      obj = createObject(id, prev);
      obj->clear();
      new XWTeXDocumentInsertObject(obj,prev->last,pcmd);
    }
    else
    {
      prev = new XWLTXDocumentPar(this,selectedObjs[0]->parent());
      new XWTeXDocumentInsertObject(prev,selectedObjs[0]->prev,pcmd);
      obj = createObject(id, prev);
      prev->clear();
      prev->append(obj);
    }

    obj->clear();
    new XWTeXDocumentAppendChildren(obj,selectedObjs[0],selectedObjs[size - 1],pcmd);
    if (next && next->type() == LArealparagraph)
    {
      new XWTeXDocumentAppendChildren(prev,next->head,next->last,pcmd);
      new XWTeXDocumentRemoveObject(next,pcmd);
    }      
  }

  undoStack->push(pcmd);
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
  emit sectionChanged();
}

void XWLaTeXDocument::resetSides()
{
  if (classIndex == XW_LTX_DOC_CLS_BEAMER && !showPreamble)
  {
    pageWidth = savedPageWidth;
    pageHeight = savedPageHeight;
    switch (direction)
    {
      case TEX_DOC_WD_TLT:
      case TEX_DOC_WD_TRT:
        top = 10;
        bottom = pageHeight - 10;
        left = 72;
        right = pageWidth - 72;
        savedTop = top;
        savedBottom = bottom;
        break;

      case TEX_DOC_WD_RTT:
        right = pageWidth - 10;
        left = 10;
        top = 72;
        bottom = pageHeight - 72;
        savedTop = right;
        savedBottom = left;
        break;

      default:
        right = pageWidth - 10;
        left = 10;
        top = 72;
        bottom = pageHeight - 72;
        savedTop = left;
        savedBottom = right;
        break;
    }
  }
  else
  {
    if (classIndex == XW_LTX_DOC_CLS_BEAMER)
    {
      savedPageWidth = pageWidth;
      savedPageHeight = pageHeight;
      pageWidth = 595.28;
      pageHeight = 841.89;
    }

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
}

void XWLaTeXDocument::setBeamerClass()
{
  classIndex = XW_LTX_DOC_CLS_BEAMER;
  pageWidth = 362.84;
  pageHeight = 272.13;
  savedPageWidth = pageWidth;
  savedPageHeight = pageHeight;
  top = 10;
  bottom = pageHeight - 10;
  left = 72;
  right = pageWidth - 72;
  emit setSlide();
}

void XWLaTeXDocument::setClassIndex(int idx)
{
  if (idx != classIndex)
  {
    classIndex = idx;
    breakPage();
  }
}

void XWLaTeXDocument::section(int id)
{
  if (!curCursor)
    return ;

  if (getCurrenrObjectType() == id)
    return ;

  XWTeXDocumentBlock * block = curCursor->getBlock();
  XWTeXDocumentObject * pobj = (XWTeXDocumentBlock*)(block->getObject()->parent());
  if (pobj->parent() != this && pobj->parent() != documentEnv)
  {
    pobj = (XWTeXDocumentObject*)(pobj->parent());
    if (pobj->type() != LApart && pobj->type() != LAchapter && pobj->type() != LAsection &&
        pobj->type() != LAsubsection && pobj->type() != LAsubsubsection &&
        pobj->type() != LAparagraph && pobj->type() != LAsubparagraph)
      return ;
  }

  QObject * parent = 0;
  if (documentEnv)
    parent = documentEnv;
  else
    parent = this;
  XWTeXDocumentObject * obj = createObject(id,parent);
  QUndoCommand * pcmd = new XWTeXDocumentNewParent(obj,pobj);
  undoStack->push(pcmd);
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
  emit sectionChanged();
}

void XWLaTeXDocument::copy()
{
  if (!hasSelect())
    return ;

  QMimeData * mimeData = createMimeDataFromSelection();
  QString str = mimeData->text();
  delete mimeData;
  QApplication::clipboard()->setText(str);
}

void XWLaTeXDocument::cut()
{
  if (!hasSelect())
    return ;

  QMimeData * mimeData = createMimeDataFromSelection();
  QString str = mimeData->text();
  delete mimeData;
  QApplication::clipboard()->setText(str);
  deleteObjs();
}

void XWLaTeXDocument::paste()
{
  if (!curCursor)
    return ;

  QString str = QApplication::clipboard()->text();
  if (str.isEmpty())
    return ;

  insert(str);
}

void XWLaTeXDocument::address()
{
  addressM = new XWLTXDocumentAddress(this,this);
  QUndoCommand * cmd = new XWTeXDocumentInsertObject(addressM,documentEnv->prev);
  undoStack->push(cmd);
  showPreamble = true;
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::author()
{
  authorM = new XWLTXDocumentAuthor(this,this);
  QUndoCommand * cmd = new XWTeXDocumentInsertObject(authorM,documentEnv->prev);
  undoStack->push(cmd);
  showPreamble = true;
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::date()
{
  dateM = new XWLTXDocumentDate(this,this);
  QUndoCommand * cmd = new XWTeXDocumentInsertObject(dateM,documentEnv->prev);
  undoStack->push(cmd);
  showPreamble = true;
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::includeOnly()
{
  XWTeXDocumentObject * obj = new XWLTXDocumentIncludeOnly(this,this);
  QUndoCommand * cmd = new XWTeXDocumentInsertObject(obj,documentEnv->prev);
  undoStack->push(cmd);
  breakPage();
  showPreamble = true;
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::insertFormular(const QString & str)
{
  if (!curCursor)
    return ;

	if (str.isEmpty())
	  return ;

  XWTeXDocumentBlock * block = curCursor->getBlock();
  XWTeXDocumentObject * obj = block->getObject();
  QUndoCommand * cmd = 0;
  if (str.startsWith("\\begin{"))
  {
    if (obj->type() == LAarray || 
            obj->type() == LAeqnarray ||
            obj->type() == LAeqnarrayStar ||
            obj->type() == LAequation)
    {
      int len = 0;
      int pos = 0;
      QString key = XWTeXDocumentObject::scanControlSequence(str,len,pos);
      XWTeXDocumentObject * nobj = createObject(key,str,pos,obj->parent());
      nobj->scan(str,len,pos);
      cmd = new QUndoCommand;
      new XWTeXDocumentInsertObject(nobj,obj,cmd);
      new XWTeXDocumentRemoveObject(obj,cmd);
    }
    else if (block->type() != TEX_DOC_B_TEXT)
    {
      if (block->hasSelected())
      {
        cmd = new QUndoCommand;
        new XWTeXDocumentDeleteString(block,cmd);
        new XWTeXDocumentInsertString(block,str,cmd);
      }
      else
        cmd = new XWTeXDocumentInsertString(block,str);
    }
    else
    {
      int len = 0;
      int pos = 0;
      QString key = XWTeXDocumentObject::scanControlSequence(str,len,pos);
      XWTeXDocumentObject * nobj = createObject(key,str,pos,obj->parent());
      nobj->scan(str,len,pos);
      cmd = new QUndoCommand;
      XWTeXDocumentBlock * nblock = new XWTeXDocumentText(this,obj->parent());
      if (block->hasSelected())
        new XWTeXDocumentDeleteString(block,cmd);
      
      new XWTeXDocumentSplitBlock(block,nblock,cmd);
      new XWTeXDocumentInsertObject(nobj,block,cmd);
    }
  }
  else if (block->hasSelected())
  {
    switch (block->type())
    {
      case TEX_DOC_B_FOMULAR:
      case TEX_DOC_B_DISFOMULAR:
        cmd = new QUndoCommand;
        new XWTeXDocumentDeleteString(block,cmd);
        new XWTeXDocumentInsertString(block,str,cmd);
        break;

      case TEX_DOC_B_TEXT:
        {
          XWTeXDocumentFormular * nobj = new XWTeXDocumentFormular(this,obj->parent());
          nobj->setText(str);
          XWTeXDocumentBlock * nblock = new XWTeXDocumentText(this,obj->parent());
          cmd = new QUndoCommand;
          new XWTeXDocumentDeleteString(block,cmd);
          new XWTeXDocumentSplitBlock(block,nblock,cmd);
          new XWTeXDocumentInsertObject(nobj,block,cmd);
        }
        break;

      default:
        {
          QString tmp = QString("$%1$").arg(str);
          new XWTeXDocumentDeleteString(block,cmd);
          new XWTeXDocumentInsertString(block,tmp,cmd);
        }
        break;
    }
  }
  else
  {
    switch (block->type())
    {
      case TEX_DOC_B_FOMULAR:
      case TEX_DOC_B_DISFOMULAR:
        cmd = new XWTeXDocumentInsertString(block,str);
        break;

      case TEX_DOC_B_TEXT:
        {
          cmd = new QUndoCommand;
          XWTeXDocumentFormular * nobj = new XWTeXDocumentFormular(this,block->parent());
          nobj->setText(str);
          XWTeXDocumentBlock * nblock = new XWTeXDocumentText(this,obj->parent());
          new XWTeXDocumentSplitBlock(block,nblock);
          new XWTeXDocumentInsertObject(nobj,block,cmd);
        }
        break;

      default:
        {
          QString tmp = QString("$%1$").arg(str);
          cmd = new XWTeXDocumentInsertString(block,tmp);
        }
        break;
    }
  }

  undoStack->push(cmd);
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::insertTikz(const QString & s)
{
  if (!curCursor)
    return ;

	if (s.isEmpty())
	  return ;

  QString str = s;
  XWTeXDocumentBlock * block = curCursor->getBlock();
  XWTeXDocumentObject * obj = block->getObject();
  if (block->type() != TEX_DOC_B_TEXT && 
      obj->type() != PGFtikz && 
      obj->type() != PGFtikzpicture &&
      obj->type() != LAverbatim)
    return ;

  QUndoCommand * cmd = 0;
  int len = str.length();
  int pos = 0;
  if (block->type() == TEX_DOC_B_TEXT)
  {
    XWTeXDocumentObject * pic = 0;
    if (str.startsWith("\\begin{"))
    {
      pos = 19;
      pic = new XWTikzpicture(this,block->parent());
    }
    else if (str.startsWith("\\tikz"))
    {
      pos = 5;
      pic = new XWTikz(this,block->parent());
    }
    else
    {
      len += 17;
      str.insert(0,"\\begin{tikzpicture}");
      str.append("\\end{tikzpicture}");
      pic = new XWTikzpicture(this,block->parent());
    }

    pic->scan(str,len,pos);
    cmd = new QUndoCommand;
    XWTeXDocumentBlock * nblock = new XWTeXDocumentText(this,obj->parent());
    if (block->hasSelected())
    {
      new XWTeXDocumentDeleteString(block,cmd);
      new XWTeXDocumentSplitBlock(block,nblock,cmd);
    }
    else
      new XWTeXDocumentSplitBlock(block,nblock,cmd);
    new XWTeXDocumentInsertObject(pic,obj,cmd);
  }
  else if (obj->type() == LAverbatim)
  {
    if (block->hasSelected())
    {
      cmd = new QUndoCommand;
      new XWTeXDocumentDeleteString(block,cmd);
      new XWTeXDocumentInsertString(block,str,cmd);
    }
    else
      cmd = new XWTeXDocumentInsertString(block,str);
  }
  else
  {
    if (block->hasSelected() && !obj->isAllSelected())
    {
      cmd = new QUndoCommand;
      new XWTeXDocumentDeleteString(block,cmd);
      new XWTeXDocumentInsertString(block,str,cmd);
    }
    else
    {
      XWTeXDocumentObject * pic = 0;
      if (str.startsWith("\\begin{"))
      {
        pos = 19;
        pic = new XWTikzpicture(this,block->parent());        
      }
      else if (str.startsWith("\\tikz"))
      {
        pos = 5;
        pic = new XWTikz(this,block->parent());
      }
      else
      {
        len += 17;
        str.insert(0,"\\begin{tikzpicture}");
        str.append("\\end{tikzpicture}");
        pic = new XWTikzpicture(this,block->parent());
      }

      pic->scan(str,len,pos);

      cmd = new QUndoCommand;
      new XWTeXDocumentInsertObject(pic,obj);
      new XWTeXDocumentRemoveObject(obj);
    }
  }
  
  undoStack->push(cmd);
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::location()
{
  locationM = new XWLTXDocumentLocation(this,this);
  QUndoCommand * cmd = new XWTeXDocumentInsertObject(locationM,documentEnv->prev);
  undoStack->push(cmd);
  showPreamble = true;
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::name()
{
  nameM = new XWLTXDocumentName(this,this);
  QUndoCommand * cmd = new XWTeXDocumentInsertObject(nameM,documentEnv->prev);
  undoStack->push(cmd);
  showPreamble = true;
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::newCommand()
{
  XWTeXDocumentObject * obj = new XWLTXDocumentNewCommand(this,this);
  QUndoCommand * cmd = new XWTeXDocumentInsertObject(obj,documentEnv->prev);
  undoStack->push(cmd);
  showPreamble = true;
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::newCounter()
{
  XWTeXDocumentObject * obj = new XWLTXDocumentNewCounter(this,this);
  QUndoCommand * cmd = new XWTeXDocumentInsertObject(obj,documentEnv->prev);
  undoStack->push(cmd);
  showPreamble = true;
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::newEnvironment()
{
  XWTeXDocumentObject * obj = new XWLTXDocumentNewEnvironment(this,this);
  QUndoCommand * cmd = new XWTeXDocumentInsertObject(obj,documentEnv->prev);
  undoStack->push(cmd);
  showPreamble = true;
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::newFont()
{
  XWTeXDocumentObject * obj = new XWLTXDocumentNewFont(this,this);
  QUndoCommand * cmd = new XWTeXDocumentInsertObject(obj,documentEnv->prev);
  undoStack->push(cmd);
  showPreamble = true;
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::newLength()
{
  XWTeXDocumentObject * obj = new XWLTXDocumentNewLength(this,this);
  QUndoCommand * cmd = new XWTeXDocumentInsertObject(obj,documentEnv->prev);
  undoStack->push(cmd);
  showPreamble = true;
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::newTheorem()
{
  XWTeXDocumentObject * obj = new XWLTXDocumentNewTheorem(this,this);
  QUndoCommand * cmd = new XWTeXDocumentInsertObject(obj,documentEnv->prev);
  undoStack->push(cmd);
  showPreamble = true;
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::renewCommand()
{
  XWTeXDocumentObject * obj = new XWLTXDocumentRenewCommand(this,this);
  QUndoCommand * cmd = new XWTeXDocumentInsertObject(obj,documentEnv->prev);
  undoStack->push(cmd);
  showPreamble = true;
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::renewEnvironment()
{
  XWTeXDocumentObject * obj = new XWLTXDocumentRenewEnvironment(this,this);
  QUndoCommand * cmd = new XWTeXDocumentInsertObject(obj,documentEnv->prev);
  undoStack->push(cmd);
  showPreamble = true;
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::signature()
{
  signatureM = new XWLTXDocumentSignature(this,this);
  QUndoCommand * cmd = new XWTeXDocumentInsertObject(signatureM,documentEnv->prev);
  undoStack->push(cmd);
  showPreamble = true;
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::telephone()
{
  telephoneM = new XWLTXDocumentTelephone(this,this);
  QUndoCommand * cmd = new XWTeXDocumentInsertObject(telephoneM,documentEnv->prev);
  undoStack->push(cmd);
  showPreamble = true;
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::thanks()
{
  thanksM = new XWLTXDocumentThanks(this,this);
  QUndoCommand * cmd = new XWTeXDocumentInsertObject(thanksM,documentEnv->prev);
  undoStack->push(cmd);
  showPreamble = true;
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::title()
{
  titleM = new XWLTXDocumentTitle(this,this);
  QUndoCommand * cmd = new XWTeXDocumentInsertObject(titleM,documentEnv->prev);
  undoStack->push(cmd);
  showPreamble = true;
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::subtitle()
{
  subtitleM = new XWBeamerSubtitle(this,this);
  QUndoCommand * cmd = 0;
  if (titleM)
    cmd = new XWTeXDocumentInsertObject(subtitleM,titleM);
  else
    cmd = new XWTeXDocumentInsertObject(subtitleM,documentEnv->prev);
  undoStack->push(cmd);
  showPreamble = true;
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::institute()
{
  instituteM = new XWBeamerInstitute(this,this);
  QUndoCommand * cmd = 0;
  if (authorM)
    cmd = new XWTeXDocumentInsertObject(instituteM,authorM);
  else
    cmd = new XWTeXDocumentInsertObject(instituteM,documentEnv->prev);
  undoStack->push(cmd);
  showPreamble = true;
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::usePackage()
{
  XWTeXDocumentObject * obj = new XWLTXDocumentUsePackage(this,this);
  QUndoCommand * cmd = new XWTeXDocumentInsertObject(obj,documentClass);
  undoStack->push(cmd);
  showPreamble = true;
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::tikzUseTikzLibrary()
{
  XWTeXDocumentObject * obj = new XWUsetikzlibrary(this,this);
  QUndoCommand * cmd = new XWTeXDocumentInsertObject(obj,documentClass);
  undoStack->push(cmd);
  showPreamble = true;
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::useTheme()
{
  XWTeXDocumentObject * obj = createBeamerObject(BMusetheme,this,this);
  QUndoCommand * cmd = new XWTeXDocumentInsertObject(obj,documentEnv->prev);
  undoStack->push(cmd);
  showPreamble = true;
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::includeOnlyLecture()
{
  XWTeXDocumentObject * obj = createBeamerObject(BMincludeonlylecture,this,this);
  QUndoCommand * cmd = new XWTeXDocumentInsertObject(obj,documentEnv->prev);
  undoStack->push(cmd);
  showPreamble = true;
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::insertObject(QAction * a)
{
  if (!a)
    return ;

  int id = a->data().toInt();
  insertObject(id);
}

void XWLaTeXDocument::delFrame()
{
  if (curCursor)
  {
    XWTeXDocumentBlock * block = curCursor->getBlock();
    XWTeXDocumentObject * obj = block->getObject();
    while (obj->parent() != this)
    {
      if (obj->parent() == documentEnv)
        break;

      obj = (XWTeXDocumentObject*)(obj->parent());
    }

    if (obj->type() == BMframe)
    {
      QUndoCommand * cmd = new XWTeXDocumentRemoveObject(obj);
      undoStack->push(cmd);
      breakPage();
      isModified = true;
      emit modificationChanged(isModified);
    }
  }
}

void XWLaTeXDocument::insertFrameSec(QAction * a)
{
  if (!a)
    return ;

  int id = a->data().toInt();
  XWTeXDocumentBlock * block = curCursor->getBlock();
  XWTeXDocumentObject * obj = block->getObject();
  XWTeXDocumentObject * sec = 0;
  if (obj->type() == BMframe)
    sec = createObject(id,obj);
  else
  {
    XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(obj->parent());
    sec = createObject(id,pobj);
  }
  QUndoCommand * cmd = new XWTeXDocumentNewObject(sec);
  undoStack->push(cmd);
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::insertFrameAtAfter()
{
  if (curCursor)
  {
    XWTeXDocumentBlock * block = curCursor->getBlock();
    XWTeXDocumentObject * obj = block->getObject();
    while (obj->parent() != this)
    {
      if (obj->parent() == documentEnv)
        break;

      obj = (XWTeXDocumentObject*)(obj->parent());
    }

    XWTeXDocumentObject * f = new XWBeamerFrameEnv(this,obj->parent());
    QUndoCommand * cmd = new XWTeXDocumentInsertObject(f,obj);
    undoStack->push(cmd);
    breakPage();
    isModified = true;
    emit modificationChanged(isModified);
  }
}

void XWLaTeXDocument::insertFrameAtBefore()
{
  if (curCursor)
  {
    XWTeXDocumentBlock * block = curCursor->getBlock();
    XWTeXDocumentObject * obj = block->getObject();
    while (obj->parent() != this)
    {
      if (obj->parent() == documentEnv)
        break;

      obj = (XWTeXDocumentObject*)(obj->parent());
    }

    XWTeXDocumentObject * f = new XWBeamerFrameEnv(this,obj->parent());
    QUndoCommand * cmd = new XWTeXDocumentInsertObject(f,obj->prev);
    undoStack->push(cmd);
    breakPage();
    isModified = true;
    emit modificationChanged(isModified);
  }
}

void XWLaTeXDocument::insertSection(QAction * a)
{
  if (!a || !curCursor)
    return ;

  XWTeXDocumentBlock * block = curCursor->getBlock();
  XWTeXDocumentObject * obj = block->getObject();
  while (obj->parent() != this)
  {
    if (obj->parent() == documentEnv)
      break;

    obj = (XWTeXDocumentObject*)(obj->parent());
  }

  QObject * parent = 0;
  if (documentEnv)
    parent = documentEnv;
  else
    parent = this;
  int id = a->data().toInt();
  XWTeXDocumentObject * s = createObject(id, parent);
  QUndoCommand * cmd = new XWTeXDocumentNewObject(s);
  undoStack->push(cmd);
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
  emit sectionChanged();
}

void XWLaTeXDocument::maketitle()
{
  matter(LAmaketitle);
}

void XWLaTeXDocument::titlePage()
{
  QObject * parent = 0;
  if (documentEnv)
    parent = documentEnv;
  else
    parent = this;
  XWTeXDocumentObject * obj = new XWLTXDocumentTitlePage(this,parent);
  QUndoCommand * cmd = new XWTeXDocumentNewObject(obj);
  undoStack->push(cmd);
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::tableOfContents()
{
  matter(LAtableofcontents);
}

void XWLaTeXDocument::abstract()
{
  getSelectedPars();
  if (selectedObjs.isEmpty())
  {
    XWTeXDocumentBlock * block = curCursor->getBlock();
    XWTeXDocumentObject * tmp = block->getObject();
    selectedObjs << (XWTeXDocumentObject*)(tmp->parent());
  }

  QObject * parent = 0;
  if (documentEnv)
    parent = documentEnv;
  else
    parent = this;

  XWTeXDocumentObject * obj = new XWLTXDocumentAbstract(this,parent);
  obj->clear();
  int size = selectedObjs.size();
  QUndoCommand * pcmd = new XWTeXDocumentAppendChildren(obj,selectedObjs[0],selectedObjs[size - 1]);
  undoStack->push(pcmd);
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::appendix()
{
  matter(LAappendix);
}

void XWLaTeXDocument::printIndex()
{
  matter(LAprintindex);
}

void XWLaTeXDocument::printGlossary()
{
  matter(LAprintglossary);
}

void XWLaTeXDocument::thebibliography()
{
  matter(LAthebibliography);
}

void XWLaTeXDocument::insertQuote()
{
  insertObject(LAquote);
}

void XWLaTeXDocument::insertQuotation()
{
  insertObject(LAquotation);
}

void XWLaTeXDocument::insertTheorem()
{
  insertObject(LAtheorem);
}

void XWLaTeXDocument::insertProof()
{
  insertObject(LAproof);
}

void XWLaTeXDocument::insertAssumption()
{
  insertObject(LAassumption);
}

void XWLaTeXDocument::insertDefinition()
{
  insertObject(LAdefinition);
}

void XWLaTeXDocument::insertProposition()
{
  insertObject(LAproposition);
}

void XWLaTeXDocument::insertLemma()
{
  insertObject(LAlemma);
}

void XWLaTeXDocument::insertAxiom()
{
  insertObject(LAaxiom);
}

void XWLaTeXDocument::insertCorollary()
{
  insertObject(LAcorollary);
}

void XWLaTeXDocument::insertConjecture()
{
  insertObject(LAconjecture);
}

void XWLaTeXDocument::insertExercise()
{
  insertObject(LAexercise);
}

void XWLaTeXDocument::insertExample()
{
  insertObject(LAexample);
}

void XWLaTeXDocument::insertRemark()
{
  insertObject(LAremark);
}

void XWLaTeXDocument::insertProblem()
{
  insertObject(LAproblem);
}

void XWLaTeXDocument::insertCenter()
{
  insertObject(LAcenter);
}

void XWLaTeXDocument::insertFlushLeft()
{
  insertObject(LAflushleft);
}

void XWLaTeXDocument::insertFlushRight()
{
  insertObject(LAflushright);
}

void XWLaTeXDocument::insertVerse()
{
  insertObject(LAverse);
}

void XWLaTeXDocument::verse()
{
  alignEnv(LAverse);
}

void XWLaTeXDocument::insertDescription()
{
  insertObject(LAdescription);
}

void XWLaTeXDocument::insertEnumerate()
{
  insertObject(LAenumerate);
}

void XWLaTeXDocument::insertItemize()
{
  insertObject(LAitemize);
}

void XWLaTeXDocument::insertList()
{
  insertObject(LAlist);
}

void XWLaTeXDocument::array()
{
  insertObject(LAarray);
}

void XWLaTeXDocument::cite()
{
  insertObject(LAcite);
}

void XWLaTeXDocument::eqnarray()
{
  insertObject(LAeqnarray);
}

void XWLaTeXDocument::eqnarrayStar()
{
  insertObject(LAeqnarrayStar);
}

void XWLaTeXDocument::equation()
{
  insertObject(LAequation);
}

void XWLaTeXDocument::footnote()
{
  insertObject(LAfootnote);
}

void XWLaTeXDocument::glossary()
{
  insertObject(LAglossary);
}

void XWLaTeXDocument::include()
{
  insertObject(LAinclude);
}

void XWLaTeXDocument::includeGraphics()
{
  insertObject(LAincludegraphics);
}

void XWLaTeXDocument::index()
{
  insertObject(LAindex);
}

void XWLaTeXDocument::input()
{
  insertObject(LAinput);
}

void XWLaTeXDocument::label()
{
  insertObject(LAlabel);
}

void XWLaTeXDocument::marginpar()
{
  insertObject(LAmarginpar);
}

void XWLaTeXDocument::miniPage()
{
  insertObject(LAminipage);
}

void XWLaTeXDocument::nocite()
{
  insertObject(LAnocite);
}

void XWLaTeXDocument::pageref()
{
  insertObject(LApageref);
}

void XWLaTeXDocument::ref()
{
  insertObject(LAref);
}

void XWLaTeXDocument::picture()
{
  insertObject(LApicture);
}

void XWLaTeXDocument::pstricksPicture()
{
  insertObject(PSTpspicture);
}

void XWLaTeXDocument::tabular()
{
  insertObject(LAtabular);
}

void XWLaTeXDocument::tabularStar()
{
  insertObject(LAtabularStar);
}

void XWLaTeXDocument::tikz()
{
  insertObject(PGFtikz);
}

void XWLaTeXDocument::tikzPicture()
{
  insertObject(PGFtikzpicture);
}

void XWLaTeXDocument::verbatim()
{
  insertObject(LAverbatim);
}

void XWLaTeXDocument::figure()
{
  XWTeXDocumentBlock * block = curCursor->getBlock();
  XWTeXDocumentObject * obj = block->getObject();
  XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(obj->parent());
  QUndoCommand * pcmd = new QUndoCommand;
  if (pobj->type() == LAfigureStar)
  {
    XWTeXDocumentObject *  f = new XWLTXDocumentFigure(this,pobj->parent());
    pcmd = new XWTeXDocumentNewParent(f,pobj,pcmd);
  }
  else
  {
    XWTeXDocumentObject * prev = obj->prev;
    pcmd = new QUndoCommand;
    new XWTeXDocumentRemoveObject(obj,pcmd);
    XWTeXDocumentObject *  f = new XWLTXDocumentFigure(this,pobj);
    new XWTeXDocumentInsertObject(f,prev,pcmd);
    new XWTeXDocumentInsertObject(obj,f->head->next,pcmd);
    XWTeXDocumentObject * l = new XWLTXDocumentLabel(this,f);
    new XWTeXDocumentInsertObject(l, obj,pcmd);
    XWTeXDocumentObject * c = new XWLTXDocumentCaption(this,f);
    new XWTeXDocumentInsertObject(c, l,pcmd);
  }

  undoStack->push(pcmd);
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::figureStar()
{
  XWTeXDocumentBlock * block = curCursor->getBlock();
  XWTeXDocumentObject * obj = block->getObject();
  XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(obj->parent());
  QUndoCommand * pcmd = 0;
  if (pobj->type() == LAfigure)
  {
    XWTeXDocumentObject *  f = new XWLTXDocumentFigureStar(this,pobj->parent());
    pcmd = new XWTeXDocumentNewParent(f,pobj,pcmd);
  }
  else
  {
    XWTeXDocumentObject * prev = obj->prev;
    pcmd = new QUndoCommand;
    new XWTeXDocumentRemoveObject(obj,pcmd);
    XWTeXDocumentObject *  f = new XWLTXDocumentFigureStar(this,pobj);
    new XWTeXDocumentInsertObject(f,prev,pcmd);
    new XWTeXDocumentInsertObject(obj,f->head->next,pcmd);
    XWTeXDocumentObject * l = new XWLTXDocumentLabel(this,f);
    new XWTeXDocumentInsertObject(l, obj,pcmd);
    XWTeXDocumentObject * c = new XWLTXDocumentCaption(this,f);
    new XWTeXDocumentInsertObject(c, l,pcmd);
  }

  undoStack->push(pcmd);
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::table()
{
  XWTeXDocumentBlock * block = curCursor->getBlock();
  XWTeXDocumentObject * obj = block->getObject();
  XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(obj->parent());
  XWTeXDocumentObject * prev = obj->prev;
  
  QUndoCommand * pcmd = new QUndoCommand;
  new XWTeXDocumentRemoveObject(obj,pcmd);
  XWTeXDocumentObject *  f = new XWLTXDocumentTable(this,pobj);
  new XWTeXDocumentInsertObject(f,prev,pcmd);
  new XWTeXDocumentInsertObject(obj,f->head->next,pcmd);
  XWTeXDocumentObject * l = new XWLTXDocumentLabel(this,f);
  new XWTeXDocumentInsertObject(l, obj,pcmd);
  XWTeXDocumentObject * c = new XWLTXDocumentCaption(this,f);
  new XWTeXDocumentInsertObject(c, l,pcmd);

  undoStack->push(pcmd);
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::removeFloat()
{
  XWTeXDocumentBlock * block = curCursor->getBlock();
  XWTeXDocumentObject * obj = block->getObject();
  if (obj->type() != LAfigure && obj->type() != LAfigureStar &&
      obj->type() != LAtable)
  {
    obj = (XWTeXDocumentObject*)(obj->parent());
    if (obj->type() != LAfigure && obj->type() != LAfigureStar &&
        obj->type() != LAtable)
    {
      obj = (XWTeXDocumentObject*)(obj->parent());
    }
  }

  QUndoCommand * pcmd = new QUndoCommand;
  XWTeXDocumentObject * cl = obj->last->prev;
  XWTeXDocumentObject * nobj = obj;
  while (cl != obj->head->next)
  {
    if (cl->type() == LAcaption || cl->type() == LAlabel)
      new XWTeXDocumentRemoveObject(cl,pcmd);
    else
    {
      new XWTeXDocumentInsertObject(cl,nobj,pcmd);
      nobj = cl;
    }

    cl = cl->prev;
  }

  new XWTeXDocumentRemoveObject(obj,pcmd);
  undoStack->push(pcmd);
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::star()
{
  XWTeXDocumentBlock * block = curCursor->getBlock();
  XWTeXDocumentObject * obj = (XWTeXDocumentObject*)(block->getObject()->parent());
  obj = (XWTeXDocumentObject*)(obj->parent());

  XWLTXStar * s = (XWLTXStar*)(obj->head->next);
  s->setCursorAtStart();
  QUndoCommand * cmd = new XWTeXDocumentInsertString(s,"*");
  undoStack->push(cmd);
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::removeStar()
{
  XWTeXDocumentBlock * block = curCursor->getBlock();
  XWTeXDocumentObject * obj = (XWTeXDocumentObject*)(block->getObject()->parent());
  obj = (XWTeXDocumentObject*)(obj->parent());

  XWLTXStar * s = (XWLTXStar*)(obj->head->next);
  QUndoCommand * cmd = new XWTeXDocumentDeleteChar(s,1);
  undoStack->push(cmd);
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::editShort()
{
  XWTeXDocumentBlock * block = curCursor->getBlock();
  XWTeXDocumentObject * obj = (XWTeXDocumentObject*)(block->getObject()->parent());
  showSectioning = (XWLTXDocumentSectioning*)(obj->parent());
  breakPage();
}

void XWLaTeXDocument::closeShort()
{
  showSectioning = 0;
}

void XWLaTeXDocument::tabularToStar()
{
  XWTeXDocumentBlock * block = curCursor->getBlock();
  XWTeXDocumentObject * obj = block->getObject();
  XWTeXDocumentObject * tobj = createObject(LAtabularStar,obj->parent());
  QUndoCommand * pcmd = new XWTeXDocumentNewParent(tobj,obj);
  undoStack->push(pcmd);
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::starToTabular()
{
  XWTeXDocumentBlock * block = curCursor->getBlock();
  XWTeXDocumentObject * obj = block->getObject();
  XWTeXDocumentObject * tobj = createObject(LAtabular,obj->parent());
  QUndoCommand * pcmd = new XWTeXDocumentNewParent(tobj,obj);
  undoStack->push(pcmd);
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::eqnarrayToStar()
{
  XWTeXDocumentBlock * block = curCursor->getBlock();
  XWTeXDocumentObject * obj = block->getObject();
  XWTeXDocumentObject * tobj = createObject(LAeqnarrayStar,obj->parent());
  QUndoCommand * pcmd = new XWTeXDocumentNewParent(tobj,obj);
  undoStack->push(pcmd);
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::starToEqnarray()
{
  XWTeXDocumentBlock * block = curCursor->getBlock();
  XWTeXDocumentObject * obj = block->getObject();
  XWTeXDocumentObject * tobj = createObject(LAeqnarray,obj->parent());
  QUndoCommand * pcmd = new XWTeXDocumentNewParent(tobj,obj);
  undoStack->push(pcmd);
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::deleteObjs()
{
  getSelectedObjs();
  QUndoCommand * pcmd = 0;
  int size = selectedObjs.size();
  if (size == 1 && !selected[0]->isAllSelected() && !selected[0]->isReadOnly())
    pcmd = new XWTeXDocumentDeleteString(selected[0]);
  else
    pcmd = new XWTeXDocumentRemoveChildren(selectedObjs[0],selectedObjs[size - 1]);

  undoStack->push(pcmd);
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::getSelectedObjs()
{
  selectedObjs.clear();
  for (int i = 0; i < selected.size(); i++)
  {
    XWTeXDocumentObject*obj = selected[i]->getObject();
    if (!selectedObjs.contains(obj) && obj != documentEnv && obj != documentClass)
      selectedObjs << obj;
  }
}

void XWLaTeXDocument::getSelectedPars()
{
  int size = selected.size();
  if (size <= 0)
    return ;

  selectType = XW_LTX_INS_PAR;
  selectedObjs.clear();
  XWTeXDocumentObject * sobj = (XWTeXDocumentObject*)(selected[0]->getObject()->parent());
  XWTeXDocumentObject * eobj = (XWTeXDocumentObject*)(selected[size - 1]->getObject()->parent());

  if ((sobj->parent() == eobj->parent()) && 
      (sobj->type() == LArealparagraph) && 
      (eobj->type() == LArealparagraph))
  {
    XWTeXDocumentObject * obj = sobj;
    while (obj)
    {
      selectedObjs << obj;
      if (obj == eobj)
        break;
      obj = obj->next;
    }
  }
}

void XWLaTeXDocument::insert(const QString & str)
{
  QUndoCommand * pcmd = new QUndoCommand;

  XWTeXDocumentBlock * block = curCursor->getBlock();
  if (selectType == XW_LTX_INS_TEXT)
    new XWTeXDocumentInsertString(block,str,pcmd);
  else if (selectType == XW_LTX_INS_OBJECTS)
  {
    if (block->type() != TEX_DOC_B_TEXT)
      new XWTeXDocumentInsertString(block,str,pcmd);
    else
    {
      int len = str.length();
      int pos = 0;
      XWTeXDocumentObject * obj = 0;
      XWTeXDocumentObject * pobj = block->getObject();
      if (str[pos] == QChar('\\'))
      {
        QString key = XWTeXDocumentObject::scanControlSequence(str,len,pos);
        obj = createObject(key,str,pos,pobj->parent());
      }
      else if (str[pos] == QChar('$'))
      {
        if (str[pos + 1] == QChar('$'))
          obj = new XWTeXDocumentDisplayFormular(this,pobj->parent());
        else
          obj = new XWTeXDocumentFormular(this,pobj->parent());
      }
      else if (str[pos] == QChar('%'))
        obj = new XWTeXDocumentComment(this,pobj->parent());
      else
        obj = new XWTeXDocumentText(this,pobj->parent());

      obj->scan(str,len,pos);
      new XWTeXDocumentNewObject(obj,pcmd);

      XWTeXDocumentObject * nobj = obj;
      while (pos < len)
      {
        if (str[pos] == QChar('\\'))
        {
          QString key = XWTeXDocumentObject::scanControlSequence(str,len,pos);
          obj = createObject(key,str,pos,pobj->parent());
        }
        else if (str[pos] == QChar('$'))
        {
          if (str[pos + 1] == QChar('$'))
            obj = new XWTeXDocumentDisplayFormular(this,pobj->parent());
          else
            obj = new XWTeXDocumentFormular(this,pobj->parent());
        }
        else if (str[pos] == QChar('%'))
          obj = new XWTeXDocumentComment(this,pobj->parent());
        else
          obj = new XWTeXDocumentText(this,pobj->parent());

        obj->scan(str,len,pos);
        new XWTeXDocumentInsertObject(obj,nobj,pcmd);

        nobj = obj;
      }
    }
  }
  else if (selectType == XW_LTX_INS_PAR)
  {
    int len = str.length();
    int pos = 0;
    XWTeXDocumentObject * obj = block->getObject();
    if (obj->parent() != this)
    {
      XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(obj->parent());
      QObject * parent = this;      
      if (pobj->type() == LArealparagraph)
        parent = pobj->parent();
      else if (pobj->type() == XW_LTX_PARAM)
      {
        pobj = (XWTeXDocumentObject*)(pobj->parent());
        parent = pobj->parent();
      }

      obj = new XWLTXDocumentPar(this, parent);
      new XWTeXDocumentNewObject(obj,pcmd);
    }
    else
    {
      obj = new XWLTXDocumentPar(this, this);
      new XWTeXDocumentNewObject(obj,pcmd);
    }

    obj->scan(str,len,pos);
    while (pos < len)
    {
      if (str[pos].isSpace())
        pos++;
      else
      {
        XWTeXDocumentObject * nobj = new XWLTXDocumentPar(this, obj->parent());
        nobj->scan(str,len,pos);
        new XWTeXDocumentInsertObject(nobj,obj,pcmd);
        obj = nobj;
      }
    }
  }
  else if (selectType == XW_LTX_INS_ITEM)
  {
    int len = str.length();
    int pos = 0;
    XWTeXDocumentObject * obj = 0;
    XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(block->getObject()->parent());
    XWTeXDocumentObject * ppobj = (XWTeXDocumentObject*)(pobj->parent());
    QObject * parent = ppobj->parent();
    if (str.startsWith("\\item"))
    {
      pos += 5;
      obj = new XWLTXDocumentItem(this, parent);
      new XWTeXDocumentNewObject(obj,pcmd);
    }
    else
    {
      if (pobj->isAtEnd())
      {
        obj = new XWLTXDocumentPar(this, ppobj);
        new XWTeXDocumentNewObject(obj,pcmd);
      }
      else
      {
        obj = new XWLTXDocumentItem(this, parent);
        new XWTeXDocumentNewObject(obj,pcmd);          
      }
    }

    obj->scan(str,len,pos);
    while (pos < len)
    {
      if (str[pos].isSpace())
        pos++;
      else
      {
        XWTeXDocumentObject * nobj = new XWLTXDocumentItem(this, parent);
        nobj->scan(str,len,pos);
        new XWTeXDocumentInsertObject(nobj,obj,pcmd);
        obj = nobj;
      }
    }
  }
  else if (selectType == XW_LTX_INS_BIBITEM)
  {
    int len = str.length();
    int pos = 0;
    XWTeXDocumentObject * obj = 0;
    XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(block->getObject()->parent());
    XWTeXDocumentObject * ppobj = (XWTeXDocumentObject*)(pobj->parent());
    QObject * parent = ppobj->parent();
    if (str.startsWith("\\bibitem"))
      pos += 8;
    obj = new XWLTXDocumentBibitem(this, parent);
    new XWTeXDocumentNewObject(obj,pcmd);
    obj->scan(str,len,pos);
    while (pos < len)
    {
      if (str[pos].isSpace())
        pos++;
      else
      {
        XWTeXDocumentObject * nobj = new XWLTXDocumentBibitem(this, parent);
        nobj->scan(str,len,pos);
        new XWTeXDocumentInsertObject(nobj,obj,pcmd);
        obj = nobj;
      }
    }
  }
  else
  {
    int len = str.length();
    int pos = 0;
    XWTeXDocumentObject * obj = 0;
    XWTeXDocumentObject * pobj = block->getObject();
    QObject * parent = 0;
    if (documentEnv)
      parent = documentEnv;
    else
      parent = this;

    while (pobj->parent() != parent)
      pobj = (XWTeXDocumentObject*)(pobj->parent());

    if (str[pos] == QChar('\\'))
    {
      QString key = XWTeXDocumentObject::scanControlSequence(str,len,pos);
      obj = createObject(key,str,pos,parent);
      switch (obj->type())
      {
        case LAinput:
        case LAinclude:
        case LApart:
        case LAchapter:
        case LAsection:
        case LAsubsection:
        case LAsubsubsection:
        case LAparagraph:
        case LAsubparagraph:
        case LAbibliographystyle:
        case LAbibliography:
        case LAtableofcontents:
        case LAmaketitle:
        case LAprintindex:
        case LAprintglossary:
        case LAappendix:
        case BMlecture:
        case BMagainframe:
        case BMmode:
        case BMAtBeginPart:
        case BMAtBeginLecture:
        case BMAtBeginNote:
        case BMAtEndNote:
        case BMnote:
        case BMframe:
          new XWTeXDocumentNewObject(obj,pcmd);
          break;

        default:
          {
            delete obj;
            obj = new XWLTXDocumentPar(this,parent);
            new XWTeXDocumentNewObject(obj,pcmd);
          }
          break;
      }
    }
    else if (str[pos] == QChar('%'))
    {
      obj = new XWTeXDocumentComment(this,parent);
      new XWTeXDocumentNewObject(obj,pcmd);
    }
    else
    {
      obj = new XWLTXDocumentPar(this,parent);
      new XWTeXDocumentNewObject(obj,pcmd);      
    }

    if (pobj->isEmpty())
      new XWTeXDocumentRemoveObject(pobj,pcmd);

    obj->scan(str,len,pos);
    XWTeXDocumentObject*nobj = 0;
    while (pos < len)
    {
      if (str[pos].isSpace())
        pos++;
      else
      {
        if (str[pos] == QChar('\\'))
        {
          QString key = XWTeXDocumentObject::scanControlSequence(str,len,pos);
          nobj = createObject(key,str,pos, parent);
          switch (nobj->type())
          {
            case LAinput:
            case LAinclude:
            case LApart:
            case LAchapter:
            case LAsection:
            case LAsubsection:
            case LAsubsubsection:
            case LAparagraph:
            case LAsubparagraph:
            case LAbibliographystyle:
            case LAbibliography:
            case LAtableofcontents:
            case LAmaketitle:
            case LAprintindex:
            case LAprintglossary:
            case LAappendix:
            case BMlecture:
            case BMagainframe:
            case BMmode:
            case BMAtBeginPart:
            case BMAtBeginLecture:
            case BMAtBeginNote:
            case BMAtEndNote:
            case BMnote:
            case BMframe:
              break;

            default:
              delete obj;
              nobj = new XWLTXDocumentPar(this,parent);
              break;
          }
        }
        else if (str[pos] == QChar('%'))
          nobj = new XWTeXDocumentComment(this,parent);
        else
          nobj = new XWLTXDocumentPar(this,parent);

        nobj->scan(str,len,pos);
        new XWTeXDocumentInsertObject(nobj,obj,pcmd);
        obj = nobj;
      }
    }
  }

  undoStack->push(pcmd);

  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
  if (selectType != XW_LTX_INS_TEXT)
    emit sectionChanged();
}

void XWLaTeXDocument::insertObject(int id)
{
  XWTeXDocumentBlock * block = curCursor->getBlock();
  XWTeXDocumentObject * obj = block->getObject();
  XWTeXDocumentObject * newobj = createObject(id, obj->parent());
  QUndoCommand * cmd = new XWTeXDocumentNewObject(newobj);
  undoStack->push(cmd);
  breakPage();
  isModified = true;
  emit modificationChanged(isModified);
}

void XWLaTeXDocument::makeLetterHead(double & curx,double & cury,bool & firstcolumn)
{
  if (direction == TEX_DOC_WD_RTT || direction == TEX_DOC_WD_LTL)
    return ;

  if (nameM)
  {
    nameM->setNewRow(true);
    nameM->breakPage(curx,cury,firstcolumn);
  }

  if (addressM)
  {
    addressM->setNewRow(true);
    addressM->breakPage(curx,cury,firstcolumn);
  }

  if (locationM)
  {
    locationM->setNewRow(true);
    locationM->breakPage(curx,cury,firstcolumn);
  }


  if (telephoneM)
  {
    telephoneM->setNewRow(true);
    telephoneM->breakPage(curx,cury,firstcolumn);
  }

  if (dateM)
  {
    dateM->setNewRow(true);
    dateM->breakPage(curx,cury,firstcolumn);
  }
}

void XWLaTeXDocument::makeTitle(double & curx,double & cury,bool & firstcolumn)
{
  if (titleM)
    titleM->breakPage(curx,cury,firstcolumn);

  if (authorM)
    authorM->breakPage(curx,cury,firstcolumn);

  if (dateM)
    dateM->breakPage(curx,cury,firstcolumn);

  if (thanksM)
    thanksM->breakPage(curx,cury,firstcolumn);
}

void XWLaTeXDocument::setup(const QString & str, int & len, int & pos)
{
  if (head)
  {
    XWTeXDocumentObject * tobj = head;
    while (tobj)
    {
      XWTeXDocumentObject * tmp = tobj;
      tobj = tobj->next;
      delete tmp;
    }

    head = 0;
    last = 0;
    firstComment = 0;
    documentClass = 0;
    titleM = 0;
    authorM = 0;
    dateM = 0;
    thanksM = 0;
    nameM = 0;
    signatureM = 0;
    telephoneM = 0;
    locationM = 0;
    addressM = 0;
    subtitleM = 0;
    instituteM = 0;
    documentEnv = 0;
    undoStack->clear();
  }

  if (len <= 0)
  {
    XWTeXDocumentObject * obj = new XWLTXDocumentPar(this,this);
    head = obj;
    last = obj;

    obj->setCursor();

    return ;
  }

  if (str[pos] == QChar('%'))
  {
    firstComment = new XWTeXDocumentComment(this,this);
    firstComment->scan(str,len,pos);
    head = firstComment;
    last = head;
    while (pos < len && str[pos].isSpace())
      pos++;
  }

  XWTeXDocumentObject * obj = 0;
  if (pos >= len)
  {
    obj = new XWLTXDocumentPar(this,this);
    last->next = obj;
    obj->prev = last;
    last = obj;

    return ;
  }

  while (pos < len)
  {
    if (str[pos].isSpace())
      pos++;
    else
    {
      if (str[pos] == QChar('%'))
        obj = new XWTeXDocumentComment(this,this);
      else if (str[pos] == QChar('\\'))
      {
        int p = pos;
        QString key = XWTeXDocumentObject::scanControlSequence(str,len,pos);
        obj = createObject(key,str,pos,this);
        switch (obj->type())
        {
          case LAdocumentclass:
            documentClass = (XWLTXDocumentClass*)obj;
            break;

          case LAdocument:
            documentEnv = (XWLTXDocumentEnviroment*)obj;
            break;

          case LAtitle:
            titleM = (XWLTXDocumentTitle*)obj;
            break;

          case LAauthor:
            authorM = (XWLTXDocumentAuthor*)obj;
            break;

          case LAdate:
            dateM = (XWLTXDocumentDate*)obj;
            break;

          case LAthanks:
            thanksM = (XWLTXDocumentThanks*)obj;
            break;

          case LAname:
            nameM = (XWLTXDocumentName*)obj;
            break;

          case LAsignature:
            signatureM = (XWLTXDocumentSignature*)obj;
            break;

          case LAtelephone:
            telephoneM = (XWLTXDocumentTelephone*)obj;
            break;

          case LAlocation:
            locationM = (XWLTXDocumentLocation*)obj;
            break;

          case LAaddress:
            addressM = (XWLTXDocumentAddress*)obj;
            break;

          case BMsubtitle:
            subtitleM = (XWBeamerSubtitle*)obj;
            break;

          case BMinstitute:
            instituteM = (XWBeamerInstitute*)obj;
            break;

          case BMframe:
            setBeamerClass();
            break;

          case LAchapter:
            setBookClass();
            break;

          case LAletter:
            setLetterClass();
            break;

          case LAinput:
          case LAinclude:
          case LApart:
          case LAsection:
          case LAsubsection:
          case LAsubsubsection:
          case LAparagraph:
          case LAsubparagraph:
          case LAbibliographystyle:
          case LAbibliography:
          case LAabstract:
          case LAthebibliography:
          case LACJK:
          case LACJKStar:
          case LAmaketitle:
          case LAprintindex:
          case LAprintglossary:
          case LAtableofcontents:
          case LAappendix:
          case BMlecture:
          case BMagainframe:
          case BMmode:
          case BMAtBeginPart:
          case BMAtBeginLecture:
          case BMAtBeginNote:
          case BMAtEndNote:
          case BMnote:
            break;

          default:
            pos = p;
            delete obj;
            obj = new XWLTXDocumentPar(this,this);
            break;
        }
      }
      else
        obj = new XWLTXDocumentPar(this, this);

      if (!head)
      {
        head = obj;
        last = head;
      }
      else
      {
        last->next = obj;
        obj->prev = last;
        last = obj;
      }

      obj->scan(str,len, pos);
    }
  }
}
