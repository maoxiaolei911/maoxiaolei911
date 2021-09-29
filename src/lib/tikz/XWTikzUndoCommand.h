/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTIKZUNDOCOMMAND_H
#define XWTIKZUNDOCOMMAND_H

#include <QUndoCommand>
#include <QString>
#include <QPointF>
#include <QVector3D>

class XWTikzGraphic;
class XWTikzCommand;
class XWTikzScope;
class XWTikzPath;
class XWTikzOperation;
class XWTikzCoord;
class XWTIKZOptions;
class XWTikzKey;
class XWTikzTextBox;
class XWTikzDomain;
class XWTikzArrows;
class XWTikzColor;
class XWTikzAround;
class XWTikzcm;
class XWTikzText;
class XWTikzValue;
class XWTikzOf;
class XWTikzEdge;
class XWTikzCoordinate;
class XWTikzSize;
class XWTikzUnit;

class XWTikzSetCoordinate : public QUndoCommand
{
public:
  XWTikzSetCoordinate(XWTikzCoord * coordA, 
                      const QString & newvA,
                      QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
   XWTikzCoord * coord;
   QString newV;
   QString oldV;
};

class XWTikzMoveTo : public QUndoCommand
{
public:
  XWTikzMoveTo(XWTikzCoord * pA,
               const QVector3D & newpA,
               const QVector3D & oldpA,
               QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
  XWTikzCoord * p;  
  QVector3D newP;
  QVector3D oldP;
};

class XWTikzSetKey : public QUndoCommand
{
public:
  XWTikzSetKey(XWTikzOperation * keyA, int newkA,QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
   XWTikzOperation * key;
   int newK;
};

class XWTikzSetDomain : public QUndoCommand
{
public:
  XWTikzSetDomain(XWTikzDomain * domainA, 
                  const QString & sA,
                  const QString & eA,
                  QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
   XWTikzDomain * domain;
   QString s;
   QString e;
};

class XWTikzSetArrow : public QUndoCommand
{
public:
  XWTikzSetArrow(XWTikzArrows * arrowsA, int sA,int eA,QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
   XWTikzArrows * arrows;
   int s;
   int e;
};

class XWTikzSetValue : public QUndoCommand
{
public:
  XWTikzSetValue(XWTikzValue * valueA, double newvA,QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
   XWTikzValue * value;
   double newV;
   double oldV;
};

class XWTikzSetExpress : public QUndoCommand
{
public:
  XWTikzSetExpress(XWTikzValue * valueA, const QString & newvA,QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
   XWTikzValue * value;
   QString newV;
   QString oldV;
};

class XWTikzSetCoord : public QUndoCommand
{
public:
  XWTikzSetCoord(XWTikzValue * valueA, const QString & newvA,QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
   XWTikzValue * value;
   QString newV;
   QString oldV;
};

class XWTikzSetColor : public QUndoCommand
{
public:
  XWTikzSetColor(XWTikzColor * colorA, int newc1A,double newpA,int newc2A,QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
   XWTikzColor * color;
   int newC1,newC2,oldC1,oldC2;
   double newP,oldP;
};

class XWTikzSetAround : public QUndoCommand
{
public:
  XWTikzSetAround(XWTikzAround * aroundA, const QString & newexpA,
                 const QString & newcoordA,QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
   XWTikzAround * around;
   QString newExp,newCoord,oldExp,oldCoord;
};

class XWTikzSetcm : public QUndoCommand
{
public:
  XWTikzSetcm(XWTikzcm * cmA, const QString & newa,
                 const QString & newb,const QString & newc,
                 const QString & newd,const QString & newcoord,
                 QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
   XWTikzcm * cm;
   QString newA,newB,newC,newD,newCoord,oldA,oldB,oldC,oldD,oldCoord;
};

class XWTikzAddOption: public QUndoCommand
{
public:
  XWTikzAddOption(XWTIKZOptions * optionsA, int indexA,
                  XWTikzOperation *optionA, QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
   XWTIKZOptions * options;
   int index;
   XWTikzOperation* option;
};

class XWTikzRemoveOption: public QUndoCommand
{
public:
  XWTikzRemoveOption(XWTIKZOptions * optionsA, int indexA,QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
   XWTIKZOptions * options;
   int index;
   XWTikzOperation* option;
};

class XWTikzAddPath : public QUndoCommand
{
public:
  XWTikzAddPath(XWTikzGraphic * graphicA, int indexA,
                XWTikzCommand * pathA,QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
   XWTikzGraphic * graphic;
   int index;
   XWTikzCommand* path;
};

class XWTikzRemovePath : public QUndoCommand
{
public:
  XWTikzRemovePath(XWTikzGraphic * graphicA, int indexA,QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
   XWTikzGraphic * graphic;
   int index;
   XWTikzCommand* path;
};

class XWTikzAddScopePath : public QUndoCommand
{
public:
  XWTikzAddScopePath(XWTikzScope * scopeA, int indexA,
                     XWTikzCommand * pathA,QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
   XWTikzScope * scope;
   int index;
   XWTikzCommand* path;
};

class XWTikzRemoveScopePath : public QUndoCommand
{
public:
  XWTikzRemoveScopePath(XWTikzScope * scopeA, int indexA,QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
   XWTikzScope * scope;
   int index;
   XWTikzCommand* path;
};

class XWTikzAddOperation : public QUndoCommand
{
public:
  XWTikzAddOperation(XWTikzPath * pathA, int indexA,
                    XWTikzOperation * opA,QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
   XWTikzPath * path;
   int index;
   XWTikzOperation * op;
};

class XWTikzRemoveOperation : public QUndoCommand
{
public:
  XWTikzRemoveOperation(XWTikzPath * pathA, int indexA,QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
   XWTikzPath * path;
   int index;
   XWTikzOperation * op;
};

class XWTikzSetOf : public QUndoCommand
{
public:
  XWTikzSetOf(XWTikzOf * ofA, 
              const QString & p1,
              const QString & p2,
              QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
  XWTikzOf * of;
  QString path1;
  QString path2;
};

class XWTikzAddNode : public QUndoCommand
{
public:
  XWTikzAddNode(XWTikzEdge * edgeA, int indexA,
                XWTikzCoordinate * nodeA,QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
   XWTikzEdge * edge;
   int index;
   XWTikzCoordinate * node;
};

class XWTikzRemoveNode : public QUndoCommand
{
public:
  XWTikzRemoveNode(XWTikzEdge * edgeA, int indexA,
                   QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
   XWTikzEdge * edge;
   int index;
   XWTikzCoordinate * node;
};

class XWTikzAddchild : public QUndoCommand
{
public:
  XWTikzAddchild(XWTikzCoordinate * pnodeA, 
                  int indexA,
                  XWTikzCoordinate * childA,
                   QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
   XWTikzCoordinate * pnode;
   int index;
   XWTikzCoordinate * child;
};

class XWTikzRemovechild : public QUndoCommand
{
public:
  XWTikzRemovechild(XWTikzCoordinate * pnodeA, 
                  int indexA,
                   QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
   XWTikzCoordinate * pnode;
   int index;
   XWTikzCoordinate * child;
};

class XWTikzRenameOperation : public QUndoCommand
{
public:
  XWTikzRenameOperation(XWTikzOperation * opA, 
                        int k,
                        QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
   XWTikzOperation * op;
   int keyWord;
};

class XWTikzSetSize : public QUndoCommand
{
public:
  XWTikzSetSize(XWTikzSize * sizeA, 
                  const QString & wA,
                  const QString & hA,
                  QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
   XWTikzSize * size;
   QString width;
   QString height;
};

class XWTikzSetUnit : public QUndoCommand
{
public:
  XWTikzSetUnit(XWTikzUnit * unitA, 
                  const QString & nA,
                  const QString & vA,
                  QUndoCommand * parent=0);

  virtual void redo();
  virtual void undo();

private:
   XWTikzUnit * unit;
   QString name;
   QString value;
};

#endif //XWTIKZUNDOCOMMAND_H
