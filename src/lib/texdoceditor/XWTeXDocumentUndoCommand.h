/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTEXDOCUMENTUNDOCOMMAND_H
#define XWTEXDOCUMENTUNDOCOMMAND_H

#include <QUndoCommand>
#include <QString>
#include <QList>
#include "XWTeXDocumentType.h"

class XWTeXDocumentBlock;
class XWTeXDocumentSection;


class XW_TEXDOCUMENT_EXPORT XWTeXDocumentDeleteChar : public QUndoCommand
{
public:
  XWTeXDocumentDeleteChar(XWTeXDocumentBlock * blockA,
                          int dirA,
                          QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
  XWTeXDocumentBlock * block;
  int pos;
  int dir;
  QChar c;
};

class XW_TEXDOCUMENT_EXPORT XWTeXDocumentDeleteString : public QUndoCommand
{
public:
  XWTeXDocumentDeleteString(XWTeXDocumentBlock * blockA,
                            QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
  XWTeXDocumentBlock * block;
  int startPos;
  int endPos;
  QString text;
};

class XW_TEXDOCUMENT_EXPORT XWTeXDocumentInsertString : public QUndoCommand
{
public:
  XWTeXDocumentInsertString(XWTeXDocumentBlock * blockA,
                            const QString & str,
                            QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
  XWTeXDocumentBlock * block;
  int pos;
  QString text;
};

class XW_TEXDOCUMENT_EXPORT XWTeXDocumentRemoveObject : public QUndoCommand
{
public:
  XWTeXDocumentRemoveObject(XWTeXDocumentObject * objA,
                            QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
  XWTeXDocumentObject * obj;
};

class XW_TEXDOCUMENT_EXPORT XWTeXDocumentNewObject : public QUndoCommand
{
public:
  XWTeXDocumentNewObject(XWTeXDocumentObject * newobjA,
                            QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
  XWTeXDocumentObject * newObj;
};

class XW_TEXDOCUMENT_EXPORT XWTeXDocumentInsertObject : public QUndoCommand
{
public:
  XWTeXDocumentInsertObject(XWTeXDocumentObject * newobjA,
                        XWTeXDocumentObject * objA,
                      QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
  XWTeXDocumentObject * newObj;
  XWTeXDocumentObject * obj;
};

class XW_TEXDOCUMENT_EXPORT XWTeXDocumentNewParent : public QUndoCommand
{
public:
  XWTeXDocumentNewParent(XWTeXDocumentObject * newobjA,
                         XWTeXDocumentObject * objA,
                            QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
  XWTeXDocumentObject * newObj;
  XWTeXDocumentObject * obj;
};

class XW_TEXDOCUMENT_EXPORT XWTeXDocumentRemoveChildren : public QUndoCommand
{
public:
  XWTeXDocumentRemoveChildren(XWTeXDocumentObject * sobjA,
                              XWTeXDocumentObject * eobjA,
                            QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:  
  XWTeXDocumentObject * sobj;
  XWTeXDocumentObject * eobj;  
  XWTeXDocumentObject * pobj;
  XWTeXDocumentObject * obj;
};

class XW_TEXDOCUMENT_EXPORT XWTeXDocumentAppendChild : public QUndoCommand
{
public:
  XWTeXDocumentAppendChild(XWTeXDocumentObject * newobjA,
                           XWTeXDocumentObject * objA,
                            QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
  XWTeXDocumentObject * newObj;
  XWTeXDocumentObject * obj;
  XWTeXDocumentObject * prev; 
  XWTeXDocumentObject * next; 
  XWTeXDocumentObject * pobj;
};

class XW_TEXDOCUMENT_EXPORT XWTeXDocumentAppendChildren : public QUndoCommand
{
public:
  XWTeXDocumentAppendChildren(XWTeXDocumentObject * pobjA,
                              XWTeXDocumentObject * sobjA,
                              XWTeXDocumentObject * eobjA,
                            QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
  XWTeXDocumentObject * pobj;
  XWTeXDocumentObject * sobj;
  XWTeXDocumentObject * eobj; 
  XWTeXDocumentObject * oldPobj; 
  XWTeXDocumentObject * obj;
};

class XW_TEXDOCUMENT_EXPORT XWTeXDocumentBack : public QUndoCommand
{
public:
  XWTeXDocumentBack(XWTeXDocumentObject * objA,
                            QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
  XWTeXDocumentObject * obj;
  XWTeXDocumentObject * prev;
  XWTeXDocumentObject * sobj;
  XWTeXDocumentObject * eobj; 
};

class XW_TEXDOCUMENT_EXPORT XWTeXDocumentSplitBlock : public QUndoCommand
{
public:
  XWTeXDocumentSplitBlock(XWTeXDocumentBlock * objA,
                        XWTeXDocumentBlock * newobjA,
                            QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
  XWTeXDocumentBlock * obj;
  XWTeXDocumentBlock * newObj;
};

class XW_TEXDOCUMENT_EXPORT XWTeXDocumentMergeBlock : public QUndoCommand
{
public:
  XWTeXDocumentMergeBlock(XWTeXDocumentBlock * objA,
                            QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
  XWTeXDocumentBlock * obj;
  XWTeXDocumentBlock * prev;
};

class XW_TEXDOCUMENT_EXPORT XWTeXDocumentNewPar : public QUndoCommand
{
public:
  XWTeXDocumentNewPar(XWTeXDocumentObject * parA,
                      XWTeXDocumentObject * prevA,
                      XWTeXDocumentObject * sobjA,
                            QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
  XWTeXDocumentObject * par;
  XWTeXDocumentObject * prev;
  XWTeXDocumentObject * sobj;
  XWTeXDocumentObject * eobj;
  XWTeXDocumentObject * obj;
};

#endif //XWTEXDOCUMENTUNDOCOMMAND_H
