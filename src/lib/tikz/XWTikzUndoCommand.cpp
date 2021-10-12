/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include "XWTikzGraphic.h"
#include "XWTikzOperation.h"
#include "XWTikzCoord.h"
#include "XWTikzOption.h"
#include "XWTikzOptions.h"
#include "XWTikzNode.h"
#include "XWTikzTextBox.h"
#include "XWTikzCommand.h"
#include "XWTikzUndoCommand.h"


XWTikzSetCoordinate::XWTikzSetCoordinate(XWTikzCoord * coordA, 
                      const QString & newvA,
                      QUndoCommand * parent)
:QUndoCommand(parent),
coord(coordA),
newV(newvA)
{
  oldV = coord->getText();
}

void XWTikzSetCoordinate::redo()
{
  coord->setText(newV);
}

void XWTikzSetCoordinate::undo()
{
  coord->setText(oldV);
}

XWTikzMoveTo::XWTikzMoveTo(XWTikzCoord * pA,
               const QVector3D & newpA,
               const QVector3D & oldpA,
               QUndoCommand * parent)
:QUndoCommand(parent),
p(pA),
newP(newpA),
oldP(oldpA)
{}

void XWTikzMoveTo::redo()
{
  p->moveTo(newP);
}

void XWTikzMoveTo::undo()
{
  p->moveTo(oldP);
}

XWTikzSetKey::XWTikzSetKey(XWTikzOperation * keyA, 
                          int newkA,
                          QUndoCommand * parent)
:QUndoCommand(parent),
key(keyA),
newK(newkA)
{}

void XWTikzSetKey::redo()
{
  int k = key->getKeyWord();
  key->setKeyword(newK);
  newK = k;
}

void XWTikzSetKey::undo()
{
  int k = key->getKeyWord();
  key->setKeyword(newK);
  newK = k;
}

XWTikzSetDomain::XWTikzSetDomain(XWTikzDomain * domainA, 
                  const QString & sA,
                  const QString & eA,
                  QUndoCommand * parent)
:QUndoCommand(parent),
domain(domainA),
s(sA),
e(eA)
{}

void XWTikzSetDomain::redo()
{
  QString sA = domain->getStart();
  QString eA = domain->getEnd();
  domain->setEnd(e);
  domain->setStart(s);
  s = sA;
  e = eA;
}

void XWTikzSetDomain::undo()
{
  QString sA = domain->getStart();
  QString eA = domain->getEnd();
  domain->setEnd(e);
  domain->setStart(s);
  s = sA;
  e = eA;
}

XWTikzSetArrow::XWTikzSetArrow(XWTikzArrows * arrowsA, 
                              int sA,
                              int eA,
                              QUndoCommand * parent)
:QUndoCommand(parent),
arrows(arrowsA),
s(sA),
e(eA)
{}

void XWTikzSetArrow::redo()
{
  int sA = arrows->getStartArrow();
  int eA = arrows->getEndArrow();
  arrows->setEndArrow(e);
  arrows->setStartArrow(s);
  s = sA;
  e = eA;
}

void XWTikzSetArrow::undo()
{
  int sA = arrows->getStartArrow();
  int eA = arrows->getEndArrow();
  arrows->setEndArrow(e);
  arrows->setStartArrow(s);
  s = sA;
  e = eA;
}

XWTikzSetValue::XWTikzSetValue(XWTikzValue * valueA, 
                              double newvA,
                              QUndoCommand * parent)
:QUndoCommand(parent),
value(valueA),
newV(newvA)
{
  oldV = value->getValue();
}

void XWTikzSetValue::redo()
{
  value->setValue(newV);
}

void XWTikzSetValue::undo()
{
  value->setValue(oldV);
}

XWTikzSetExpress::XWTikzSetExpress(XWTikzValue * valueA, 
                                   const QString & newvA,
                                   QUndoCommand * parent)
:QUndoCommand(parent),
value(valueA),
newV(newvA)
{
  oldV = value->getExpress();
}

void XWTikzSetExpress::redo()
{
  value->setExpress(newV);
}

void XWTikzSetExpress::undo()
{
  value->setExpress(oldV);
}

XWTikzSetCoord::XWTikzSetCoord(XWTikzValue * valueA, 
                               const QString & newvA,
                               QUndoCommand * parent)
:QUndoCommand(parent),
value(valueA),
newV(newvA)
{
  oldV = value->getCoord();
}

void XWTikzSetCoord::redo()
{
  value->setCoord(newV);
}

void XWTikzSetCoord::undo()
{
  value->setCoord(oldV);
}

XWTikzSetColor::XWTikzSetColor(XWTikzColor * colorA, 
                              int newc1A,
                              double newpA,
                              int newc2A,
                              QUndoCommand * parent)
:QUndoCommand(parent),
color(colorA),
newC1(newc1A),
newP(newpA),
newC2(newc2A)
{
  color->getColor(oldC1,oldP,oldC2);
}

void XWTikzSetColor::redo()
{
  color->setColor(newC1,newP,newC2);
}

void XWTikzSetColor::undo()
{
  color->setColor(oldC1,oldP,oldC2);
}

XWTikzSetAround::XWTikzSetAround(XWTikzAround * aroundA, 
                                const QString & newexpA,
                                const QString & newcoordA,
                                QUndoCommand * parent)
:QUndoCommand(parent),
around(aroundA),
newExp(newexpA),
newCoord(newcoordA)
{
  oldExp = around->getExpress();
  oldCoord = around->getCoord();
}

void XWTikzSetAround::redo()
{
  around->setExpress(newExp);
  around->setCoord(newCoord);
}

void XWTikzSetAround::undo()
{
  around->setExpress(oldExp);
  around->setCoord(oldCoord);
}

XWTikzSetcm::XWTikzSetcm(XWTikzcm * cmA, 
                        const QString & newa,
                        const QString & newb,
                        const QString & newc,
                        const QString & newd,
                        const QString & newcoord,
                        QUndoCommand * parent)
:QUndoCommand(parent),
cm(cmA),
newA(newa),
newB(newb),
newC(newc),
newD(newd),
newCoord(newcoord)
{
  oldA = cm->getA();
  oldB = cm->getB();
  oldC = cm->getC();
  oldD = cm->getD();
  oldCoord = cm->getCoord();
}

void XWTikzSetcm::redo()
{
  cm->setA(newA);
  cm->setB(newB);
  cm->setC(newC);
  cm->setD(newD);
  cm->setCoord(newCoord);
}

void XWTikzSetcm::undo()
{
  cm->setA(oldA);
  cm->setB(oldB);
  cm->setC(oldC);
  cm->setD(oldD);
  cm->setCoord(oldCoord);
}

XWTikzAddOption::XWTikzAddOption(XWTIKZOptions * optionsA, 
                                 int indexA,
                                 XWTikzOperation *optionA, 
                                 QUndoCommand * parent)
:QUndoCommand(parent),
 options(optionsA),
 index(indexA),
 option(optionA)
{}

void XWTikzAddOption::redo()
{
  options->insert(index,option);
}

void XWTikzAddOption::undo()
{
  option = options->takeAt(index);
}

XWTikzRemoveOption::XWTikzRemoveOption(XWTIKZOptions * optionsA, 
                                      int indexA,
                                      QUndoCommand * parent)
:QUndoCommand(parent),
 options(optionsA),
 index(indexA)
{}

void XWTikzRemoveOption::redo()
{
  option = options->takeAt(index);
}

void XWTikzRemoveOption::undo()
{
  options->insert(index,option);
}

XWTikzAddPath::XWTikzAddPath(XWTikzGraphic * graphicA, 
                             int indexA,
                             XWTikzCommand * pathA,
                             QUndoCommand * parent)
:QUndoCommand(parent),
 graphic(graphicA),
 index(indexA),
 path(pathA)
{}

void XWTikzAddPath::redo()
{
  graphic->insert(index,path);
}

void XWTikzAddPath::undo()
{
  path = graphic->takeAt(index);
}

XWTikzRemovePath::XWTikzRemovePath(XWTikzGraphic * graphicA, 
                                   int indexA,
                                   QUndoCommand * parent)
:QUndoCommand(parent),
 graphic(graphicA),
 index(indexA)
{
}

void XWTikzRemovePath::redo()
{
  path = graphic->takeAt(index);
}

void XWTikzRemovePath::undo()
{
  graphic->insert(index,path);
}

XWTikzAddScopePath::XWTikzAddScopePath(XWTikzScope * scopeA, 
                                      int indexA,
                                      XWTikzCommand * pathA,
                                      QUndoCommand * parent)
:QUndoCommand(parent),
 scope(scopeA),
 index(indexA),
 path(pathA)
{}

void XWTikzAddScopePath::redo()
{
  scope->insert(index,path);
}

void XWTikzAddScopePath::undo()
{
  path = scope->takeAt(index);
}

XWTikzRemoveScopePath::XWTikzRemoveScopePath(XWTikzScope * scopeA, 
                                             int indexA,
                                             QUndoCommand * parent)
:QUndoCommand(parent),
 scope(scopeA),
 index(indexA)
{}

void XWTikzRemoveScopePath::redo()
{
  path = scope->takeAt(index);
}

void XWTikzRemoveScopePath::undo()
{
  scope->insert(index,path);
}

XWTikzAddOperation::XWTikzAddOperation(XWTikzPath * pathA, 
                                      int indexA,
                                      XWTikzOperation * opA,
                                      QUndoCommand * parent)
:QUndoCommand(parent),
 path(pathA),
 index(indexA),
 op(opA)
{}

void XWTikzAddOperation::redo()
{
  path->insert(index,op);
}

void XWTikzAddOperation::undo()
{
  path->takeAt(index);
}

XWTikzRemoveOperation::XWTikzRemoveOperation(XWTikzPath * pathA, 
                                             int indexA,
                                             QUndoCommand * parent)
:QUndoCommand(parent),
 path(pathA),
 index(indexA)
{}

void XWTikzRemoveOperation::redo()
{
  path->takeAt(index);
}

void XWTikzRemoveOperation::undo()
{
  path->insert(index,op);
}

XWTikzSetOf::XWTikzSetOf(XWTikzOf * ofA, 
              const QString & p1,
              const QString & p2,
              QUndoCommand * parent)
:QUndoCommand(parent),
 of(ofA),
 path1(p1),
 path2(p2)
{
}

void XWTikzSetOf::redo()
{
  QString p1 = of->getFirst();
  QString p2 = of->getSecond();
  of->setPath1(path1);
  of->setPath2(path2);
  path1 = p1;
  path2 = p2;
}

void XWTikzSetOf::undo()
{
  QString p1 = of->getFirst();
  QString p2 = of->getSecond();
  of->setPath1(path1);
  of->setPath2(path2);
  path1 = p1;
  path2 = p2;
}

XWTikzAddNode::XWTikzAddNode(XWTikzEdge * edgeA, int indexA,
                XWTikzCoordinate * nodeA,QUndoCommand * parent)
:QUndoCommand(parent),
 edge(edgeA),
 index(indexA),
 node(nodeA)
{
}

void XWTikzAddNode::redo()
{
  edge->insert(index,node);
}

void XWTikzAddNode::undo()
{
  node = edge->takeAt(index);
}

XWTikzRemoveNode::XWTikzRemoveNode(XWTikzEdge * edgeA, int indexA, QUndoCommand * parent)
:QUndoCommand(parent),
 edge(edgeA),
 index(indexA),
 node(0)
{
}

void XWTikzRemoveNode::redo()
{
  node = edge->takeAt(index);
}

void XWTikzRemoveNode::undo()
{
  edge->insert(index,node);
}

XWTikzAddchild::XWTikzAddchild(XWTikzCoordinate * pnodeA, 
                               int indexA,
                               XWTikzCoordinate * childA,
                               QUndoCommand * parent)
:QUndoCommand(parent),
pnode(pnodeA),
index(indexA),
child(childA)
{
}

void XWTikzAddchild::redo()
{
  pnode->insert(index,child);
}

void XWTikzAddchild::undo()
{
  child = pnode->takeAt(index);
}

XWTikzRemovechild::XWTikzRemovechild(XWTikzCoordinate * pnodeA, 
                               int indexA,
                               QUndoCommand * parent)
:QUndoCommand(parent),
pnode(pnodeA),
index(indexA)
{
}

void XWTikzRemovechild::redo()
{
  child = pnode->takeAt(index);
}

void XWTikzRemovechild::undo()
{
  pnode->insert(index,child);
}

XWTikzRenameOperation::XWTikzRenameOperation(XWTikzOperation * opA, 
                        int k,
                        QUndoCommand * parent)
:QUndoCommand(parent),
 op(opA),
 keyWord(k)
{
}

void XWTikzRenameOperation::redo()
{
  int k = op->getKeyWord();
  op->setKeyword(keyWord);
  keyWord = k;
}

void XWTikzRenameOperation::undo()
{
  int k = op->getKeyWord();
  op->setKeyword(keyWord);
  keyWord = k;
}

XWTikzSetSize::XWTikzSetSize(XWTikzSize * sizeA, 
                  const QString & wA,
                  const QString & hA,
                  QUndoCommand * parent)
:QUndoCommand(parent),
  size(sizeA),
  width(wA),
  height(hA)
{}                  

void XWTikzSetSize::redo()
{
  QString w = width;
  QString h = height;
  size->getSize(width, height);
  size->setSize(w, h);
}

void XWTikzSetSize::undo()
{
  QString w = width;
  QString h = height;
  size->getSize(width, height);
  size->setSize(w, h);
}

XWTikzSetUnit::XWTikzSetUnit(XWTikzUnit * unitA, 
                  const QString & nA,
                  const QString & vA,
                  QUndoCommand * parent)
:QUndoCommand(parent),
  unit(unitA),
  name(nA),
  value(vA)
{}                  

void XWTikzSetUnit::redo()
{
  QString n = name;
  QString v = value;
  unit->getUnit(name, value);
  unit->setUnit(n, v);
}

void XWTikzSetUnit::undo()
{
  QString n = name;
  QString v = value;
  unit->getUnit(name, value);
  unit->setUnit(n, v);
}

XWTikzAddInput::XWTikzAddInput(XWTikzInpus * inputsA, 
                               const QChar & cA,
                               QUndoCommand * parent)
:QUndoCommand(parent),
 inputs(inputsA),
 c(cA)
{
}

void XWTikzAddInput::redo()
{
  inputs->append(c);
}

void XWTikzAddInput::undo()
{
  c = inputs->remove();
}

XWTikzRemoveInput::XWTikzRemoveInput(XWTikzInpus * inputsA, 
                               QUndoCommand * parent)
:QUndoCommand(parent),
 inputs(inputsA)
{
}

void XWTikzRemoveInput::redo()
{
  c = inputs->remove();  
}

void XWTikzRemoveInput::undo()
{
  inputs->append(c);
}

XWTikzSetLabel::XWTikzSetLabel(XWTikzLabel * labelA,
                       int kw,
                       const QString & newstr,
                       QUndoCommand * parent)
:QUndoCommand(parent),
 label(labelA),
 keyword(kw),
 str(newstr)
{
}

void XWTikzSetLabel::redo()
{
  QString tmp = label->getText();
  int kw = label->getKeyWord();
  int len = str.length();
  int pos = 0;
  label->setKeyword(keyword);
  label->scan(str,len,pos);
  str = tmp;
  keyword = kw;
}

void XWTikzSetLabel::undo()
{
  QString tmp = label->getText();
  int kw = label->getKeyWord();
  int len = str.length();
  int pos = 0;
  label->setKeyword(keyword);
  label->scan(str,len,pos);
  str = tmp;
  keyword = kw;
}
