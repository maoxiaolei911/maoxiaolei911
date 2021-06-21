/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QApplication>
#include "XWTeXDocument.h"
#include "XWTeXDocumentObject.h"
#include "XWTeXDocumentUndoCommand.h"


XWTeXDocumentDeleteChar::XWTeXDocumentDeleteChar(XWTeXDocumentBlock * blockA,
                        int dirA,
                        QUndoCommand * parent)
  : QUndoCommand(parent),
    block(blockA),
    pos(0),
    dir(dirA)
{
  setText("Delete character");
}

void XWTeXDocumentDeleteChar::redo()
{
  pos = block->getCurrentPos();
  if (dir < 0)
    c = block->deletePreviousChar();
  else
    c = block->deleteChar();
}

void XWTeXDocumentDeleteChar::undo()
{
  if (dir < 0)
    block->insert(pos - 1, c);
  else
    block->insert(pos, c);
}

XWTeXDocumentDeleteString::XWTeXDocumentDeleteString(XWTeXDocumentBlock * blockA,
                                                     QUndoCommand * parent)
 : QUndoCommand(parent),
   block(blockA),
   startPos(0),
   endPos(0)
{
  setText("Delete string");
  startPos = block->getStartPos();
  endPos = block->getEndPos();
}

void XWTeXDocumentDeleteString::redo()
{
  text = block->del();
}

void XWTeXDocumentDeleteString::undo()
{
  block->insert(startPos,text);
  block->setSelected(startPos,endPos);
}

XWTeXDocumentInsertString::XWTeXDocumentInsertString(XWTeXDocumentBlock * blockA,
                          const QString & str,
                          QUndoCommand * parent)
: QUndoCommand(parent),
  block(blockA),
  text(str)
{
  setText("Insert String");
  pos = block->getCurrentPos();
}

void XWTeXDocumentInsertString::redo()
{
  block->insert(pos, text);
}

void XWTeXDocumentInsertString::undo()
{
  block->text.remove(pos, text.length());
}

XWTeXDocumentRemoveObject::XWTeXDocumentRemoveObject(XWTeXDocumentObject * objA,
                                                    QUndoCommand * parent)
:QUndoCommand(parent),
  obj(objA)
{
  setText("Delete Object");
}

void XWTeXDocumentRemoveObject::redo()
{
  obj->doc->remove(obj);
}

void XWTeXDocumentRemoveObject::undo()
{
  obj->doc->insert(obj);
}

XWTeXDocumentNewObject::XWTeXDocumentNewObject(XWTeXDocumentObject * newobjA,
                          QUndoCommand * parent)
:QUndoCommand(parent),
 newObj(newobjA)
{
  setText("Insert Object");
}

void XWTeXDocumentNewObject::redo()
{
  newObj->doc->insert(newObj);
}

void XWTeXDocumentNewObject::undo()
{
  newObj->doc->remove(newObj);
}

XWTeXDocumentInsertObject::XWTeXDocumentInsertObject(XWTeXDocumentObject * newobjA,
                        XWTeXDocumentObject * objA,
                      QUndoCommand * parent)
:QUndoCommand(parent),
 newObj(newobjA),
 obj(objA)
{
  setText("New Object");
}

void XWTeXDocumentInsertObject::redo()
{
  if (obj)
    obj->insertAtAfter(newObj);
  else if (newObj->isTopLevel())
    newObj->doc->insert(newObj);
  else
  {
    XWTeXDocumentObject * pobj = (XWTeXDocumentObject*)(newObj->parent());
    newObj->prev = 0;
    newObj->next = pobj->head;
    if (pobj->head)
      pobj->head->prev = newObj;
    pobj->head = newObj;
  }  
}

void XWTeXDocumentInsertObject::undo()
{
  newObj->remove();
}

XWTeXDocumentNewParent::XWTeXDocumentNewParent(XWTeXDocumentObject * newobjA,
                         XWTeXDocumentObject * objA,
                            QUndoCommand * parent)
:QUndoCommand(parent),
 newObj(newobjA),
 obj(objA)
{
  setText("New Parent");
}

void XWTeXDocumentNewParent::redo()
{
  obj->insertAtAfter(newObj);
  obj->remove();
  XWTeXDocumentObject* nsobj = 0;
  XWTeXDocumentObject* neobj = 0;
  newObj->removeChildren(&nsobj,&neobj);
  XWTeXDocumentObject* sobj = 0;
  XWTeXDocumentObject* eobj = 0;
  obj->removeChildren(&sobj,&eobj);
  obj->insertChildren(nsobj,neobj);
  newObj->insertChildren(sobj,eobj);  
}

void XWTeXDocumentNewParent::undo()
{
  newObj->insertAtAfter(obj);
  newObj->remove();
  XWTeXDocumentObject* nsobj = 0;
  XWTeXDocumentObject* neobj = 0;
  newObj->removeChildren(&nsobj,&neobj);
  XWTeXDocumentObject* sobj = 0;
  XWTeXDocumentObject* eobj = 0;
  obj->removeChildren(&sobj,&eobj);
  newObj->insertChildren(sobj,eobj);
  obj->insertChildren(nsobj,neobj);
}

XWTeXDocumentAppendChildren::XWTeXDocumentAppendChildren(XWTeXDocumentObject * pobjA,
                              XWTeXDocumentObject * sobjA,
                              XWTeXDocumentObject * eobjA,
                            QUndoCommand * parent)
:QUndoCommand(parent),
pobj(pobjA),
sobj(sobjA),
eobj(eobjA),
oldPobj(0),
obj(0)
{
  setText("Append Children");
  if (!sobj->isTopLevel())
    oldPobj = (XWTeXDocumentObject*)(sobj->parent());
}

void XWTeXDocumentAppendChildren::redo()
{
  if (oldPobj)
    oldPobj->removeChildren(&sobj,&eobj,&obj);
  else
    pobj->doc->removeChildren(&sobj,&eobj,&obj);
  pobj->append(sobj,eobj);
}

void XWTeXDocumentAppendChildren::undo()
{
  XWTeXDocumentObject * tmp = 0;
  pobj->removeChildren(&sobj,&eobj,&tmp);
  if (oldPobj)
    oldPobj->insertChildren(obj,sobj,eobj);
  else
    pobj->doc->insertChildren(obj,sobj,eobj);
}

XWTeXDocumentAppendChild::XWTeXDocumentAppendChild(XWTeXDocumentObject * newobjA,
                           XWTeXDocumentObject * objA,
                            QUndoCommand * parent)
:QUndoCommand(parent),
newObj(newobjA),
obj(objA),
prev(0),
next(0),
pobj(0)
{
  setText("Append Child");
  prev = obj->prev;
  next = obj->next;
  if (!obj->isTopLevel())
    pobj = (XWTeXDocumentObject*)(obj->parent());
}

void XWTeXDocumentAppendChild::redo()
{
  obj->remove();
  newObj->append(obj);
}

void XWTeXDocumentAppendChild::undo()
{
  obj->remove();
  if (prev)
    prev->insertAtAfter(obj);
  else if (next)
    next->insertAtBefore(obj);
  else if (pobj)
    pobj->append(obj);
  else
    obj->doc->append(obj);
}

XWTeXDocumentRemoveChildren::XWTeXDocumentRemoveChildren(XWTeXDocumentObject * sobjA,
                              XWTeXDocumentObject * eobjA,
                            QUndoCommand * parent)
:QUndoCommand(parent),
sobj(sobjA),
eobj(eobjA),
pobj(0),
obj(0)
{
  setText("Remove Children");
  if (!sobj->isTopLevel())
    pobj = (XWTeXDocumentObject*)(sobj->parent());
}

void XWTeXDocumentRemoveChildren::redo()
{
  if (pobj)
    pobj->removeChildren(&sobj,&eobj,&obj);
  else
    sobj->doc->removeChildren(&sobj,&eobj,&obj);
}

void XWTeXDocumentRemoveChildren::undo()
{
  if (pobj)
    pobj->insertChildren(obj,sobj,eobj);
  else
    sobj->doc->insertChildren(obj,sobj,eobj);
}

XWTeXDocumentBack::XWTeXDocumentBack(XWTeXDocumentObject * objA,
                            QUndoCommand * parent)
:QUndoCommand(parent),
obj(objA),
prev(0),
sobj(0),
eobj(0)
{
  setText("Back");
  prev = obj->prev;
}

void XWTeXDocumentBack::redo()
{
  obj->removeChildren(&sobj,&eobj);  
  prev->append(sobj,eobj);
  obj->remove();
}

void XWTeXDocumentBack::undo()
{
  prev->insertAtAfter(obj);
  XWTeXDocumentObject * tmp = 0;
  prev->removeChildrenFrom(&sobj,&eobj,&tmp);  
  obj->insertChildren(sobj,eobj);
}

XWTeXDocumentSplitBlock::XWTeXDocumentSplitBlock(XWTeXDocumentBlock * objA,
                        XWTeXDocumentBlock * newobjA,
                            QUndoCommand * parent)
:QUndoCommand(parent),
obj(objA),
newObj(newobjA)
{
  setText("Split Block");
}

void XWTeXDocumentSplitBlock::redo()
{
  int pos = obj->getCurrentPos();
  newObj->text = obj->text.mid(pos,-1);
  obj->text.remove(pos,newObj->text.length());
  obj->insertAtAfter(newObj);
  newObj->setCursor();
}

void XWTeXDocumentSplitBlock::undo()
{
  obj->text.append(newObj->text);
  newObj->remove();
  obj->setCursor();
}

XWTeXDocumentMergeBlock::XWTeXDocumentMergeBlock(XWTeXDocumentBlock * objA,
                            QUndoCommand * parent)
:QUndoCommand(parent),
obj(objA),
prev(0)
{
  setText("Merge Block");
  prev = (XWTeXDocumentBlock*)(obj->prev);
}

void XWTeXDocumentMergeBlock::redo()
{
  prev->text.append(obj->text);
  obj->remove();
  prev->setCursor();
}

void XWTeXDocumentMergeBlock::undo()
{
  prev->insertAtAfter(obj);
  int pos = prev->text.length() - obj->text.length();
  prev->text.remove(pos,obj->text.length());
  obj->setCursor();
}

XWTeXDocumentNewPar::XWTeXDocumentNewPar(XWTeXDocumentObject * parA,
                      XWTeXDocumentObject * prevA,
                      XWTeXDocumentObject * sobjA,
                            QUndoCommand * parent)
:QUndoCommand(parent),
par(parA),
prev(prevA),
sobj(sobjA),
eobj(0),
obj(0)
{
  setText("New par");
  par->clear();
}

void XWTeXDocumentNewPar::redo()
{
  prev->insertAtAfter(par);
  prev->removeChildrenFrom(&sobj,&eobj,&obj);  
  par->append(sobj,eobj);
}

void XWTeXDocumentNewPar::undo()
{
  par->remove();
  par->removeChildren(&sobj,&eobj);  
  prev->insertChildren(obj,sobj,eobj);
}
