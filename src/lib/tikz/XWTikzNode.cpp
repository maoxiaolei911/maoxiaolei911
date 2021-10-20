/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include "XWApplication.h"
#include "PGFKeyWord.h"
#include "XWPGFPool.h"
#include "XWTikzGraphic.h"
#include "XWTikzCommand.h"
#include "XWTikzState.h"
#include "XWTikzCoord.h"
#include "XWTikzMatrix.h"
#include "XWTikzOptions.h"
#include "XWTikzOperationDialog.h"
#include "XWTikzTextBox.h"
#include "XWTikzUndoCommand.h"
#include "XWTikzNode.h"

XWTikzCoordinate::XWTikzCoordinate(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA,PGFcoordinate,parent),
options(0),
box(0),
cur(-1)
{
  options = new XWTIKZOptions(graphicA, this);
}

XWTikzCoordinate::XWTikzCoordinate(XWTikzGraphic * graphicA, int idA,QObject * parent)
:XWTikzOperation(graphicA,idA,parent),
options(0),
box(0),
cur(-1)
{
  options = new XWTIKZOptions(graphicA, this);
}

bool XWTikzCoordinate::back(XWTikzState * state)
{
  bool ret = options->back(state);
  switch (graphic->getCurrentScope())
  {
    default:
      break;

    case XW_TIKZ_S_PATH:
      if (cur > 0)
      {
        ret = true;
        XWTikzRemovechild * cmd = new XWTikzRemovechild(this,cur - 1);          
        graphic->push(cmd);
      }
      break;

    case XW_TIKZ_S_OPERATION:
      if (cur >= 0 && cur < children.size())
        ret = children[cur]->back(state);
      if (!ret && box)
        ret = box->back();
      break;
  }
  
  return ret;
}

bool XWTikzCoordinate::cut(XWTikzState * state)
{
  bool ret = options->cut(state);
  if (!ret && box)
    ret = box->cut();
  if (!ret)
  {
    if (cur >= 0 && cur < children.size())
      ret = children[cur]->cut(state);
  }

  return ret;
}

bool XWTikzCoordinate::del(XWTikzState * state)
{
  bool ret = options->del(state);
  switch (graphic->getCurrentScope())
  {
    default:
      break;

    case XW_TIKZ_S_PATH:
      if (cur > 0)
      {
        ret = true;
        XWTikzRemovechild * cmd = new XWTikzRemovechild(this,cur);
        graphic->push(cmd);
      }
      break;

    case XW_TIKZ_S_OPERATION:
      if (cur >= 0 && cur < children.size())
        ret = children[cur]->del(state);
      if (!ret && box)
        ret = box->del();
      break;
  }

  return ret;
}

void XWTikzCoordinate::doCompute(XWTikzState * state)
{
  options->doCompute(state);
  state->doNodeCompute(box);
}

void XWTikzCoordinate::doPath(XWTikzState * state, bool showpoint)
{
  if (!name.isEmpty())
    graphic->registNamed(name);
  state = state->saveNode(box,XW_TIKZ_NODE);
  state->setShape(PGFcoordinate);
  options->doPath(state,showpoint);

  if (graphic->getCurrentScope() == XW_TIKZ_S_OPERATION)
  {
    if (cur >= 0 && cur < children.size())
    {
      for (int i = 0; i <= cur; i++)
        children[i]->doPath(state,showpoint);
    }
  }
  else
  {
    for (int i = 0; i < children.size(); i++)
      children[i]->doPath(state,showpoint);
  }
}

void XWTikzCoordinate::dragTo(XWTikzState * state)
{
  doPath(state,false);
}

bool XWTikzCoordinate::dropTo(XWTikzState * state)
{
  XWTikzState mystate;
  state->copy(&mystate);
  bool ret = options->dropTo(&mystate);
  if (!ret)
    ret = mystate.moveTest(box);
  if (!ret)
  {
    mystate.setChildrenNumber(children.size());
    for (int i = 0; i < children.size(); i++)
    {
      mystate.setCurrentChild(i);
      ret = children[i]->dropTo(&mystate);
      if (ret)
      {
        cur = i;
        break;
      }
    }
  }
  
  return ret;
}

QPointF XWTikzCoordinate::getAnchor(int a, XWTikzState * state)
{
  state->setAnchor(a);
  state->setAngle(0);
  return state->doAnchor(box);
}

QPointF XWTikzCoordinate::getAnchor(XWTikzState * state)
{
  return state->doAnchor(box);
}

int XWTikzCoordinate::getAnchorPosition()
{
  if (cur < 0 || cur >= children.size())
    return 0;

  return children[cur]->getAnchorPosition();
}

QPointF XWTikzCoordinate::getAngle(double a, XWTikzState * state)
{
  state->setAnchor(-1);
  state->setAngle(a);
  return state->doAnchor(box);
}

int XWTikzCoordinate::getCursorPosition()
{
  if (cur < 0 || cur >= children.size())
    return 0;

  return children[cur]->getCursorPosition();
}

QString XWTikzCoordinate::getCurrentText()
{
  if (cur < 0 || cur >= children.size())
    return QString();

  return children[cur]->getCurrentText();
}

QPointF XWTikzCoordinate::getPoint(XWTikzState * state)
{
  return state->getCurrentPoint();
}

QVector3D XWTikzCoordinate::getPoint3D(XWTikzState * state)
{
  XWTikzCoord * c = state->getCurrentCoord();
  return c->getPoint3D(state);
}

QString XWTikzCoordinate::getSelectedText()
{
  if (cur < 0 || cur >= children.size())
    return QString();

  return children[cur]->getSelectedText();
}

QString XWTikzCoordinate::getText()
{
  QString ret = " coordinate";
  QString tmp = options->getText();
  ret += tmp;
  if (!name.isEmpty())
  {
    ret += " (";
    ret += name;
    ret += ")";
  }

  for (int i = 0; i < children.size(); i++)
  {
    tmp = children[i]->getText();
    ret += tmp;
  }

  return ret;
}

QString XWTikzCoordinate::getTextForPath()
{
  QString ret = "coordinate";
  QString tmp = options->getText();
  ret += tmp;
  if (!name.isEmpty())
  {
    ret += " (";
    ret += name;
    ret += ")";
  }

  for (int i = 0; i < children.size(); i++)
  {
    tmp = children[i]->getText();
    ret += tmp;
  }

  return ret;
}

bool XWTikzCoordinate::goToNext()
{
  if (cur == children.size() -1)
    return false;

  cur++;
  children[cur]->goToStart();
  return true;
}

void XWTikzCoordinate::goToOperationEnd()
{
  if (cur < 0 || cur > children.size() -1)
    return ;

  children[cur]->goToEnd();
}

void XWTikzCoordinate::goToOperationStart()
{
  if (cur < 0 || cur > children.size() -1)
    return ;

  children[cur]->goToStart();
}

void XWTikzCoordinate::goToPathEnd()
{
  if (children.size() == 0)
    return ;

  cur = children.size() - 1;
  children[cur]->goToEnd();
}

void XWTikzCoordinate::goToPathStart()
{
  if (children.size() == 0)
    return ;

  cur = 0;
  children[cur]->goToStart();
}

bool XWTikzCoordinate::goToPrevious()
{
  if (cur == 0)
    return false;

  cur--;
  children[cur]->goToEnd();
  return true;
}

void XWTikzCoordinate::goToEnd()
{
  if (box)
    box->goToEnd();
}

void XWTikzCoordinate::goToStart()
{
  if (box)
    box->goToStart();
}

bool XWTikzCoordinate::hitTest(XWTikzState * state)
{
  XWTikzState mystate;
  state->copy(&mystate);
  cur = -1;
  bool ret = options->hitTest(&mystate);
  if (!ret)
    ret = mystate.hitTestCoordinate(box);
  if (!ret)
  {
    mystate.setChildrenNumber(children.size());
    for (int i = 0; i < children.size(); i++)
    {
      mystate.setCurrentChild(i);
      ret = children[i]->hitTest(&mystate);
      if (ret)
      {
        cur = i;
        break;
      }
    }
  }

  return ret;
}

void XWTikzCoordinate::insert(int i, XWTikzCoordinate * opA)
{
  cur = i;
  if (i >= children.size())
    children << opA;
  else
    children.insert(i,opA);
}

bool XWTikzCoordinate::insertText(XWTikzState * state)
{
  bool ret = options->insertText(state);
  if (!ret && box)
  {
    QString str = state->getText();
    ret = box->insertText(str);
  }
    
  if (!ret)
  {
    if (cur >= 0 && cur < children.size())
      ret = children[cur]->insertText(state);
  }

  return ret;
}

bool XWTikzCoordinate::isMe(const QString & nameA,XWTikzState * state)
{
  QString n = nameA;
  if (n.contains("-"))
  {
    QString prefx = state->getNamePrefix();
    prefx += "-";
    QString suffix = state->getNameSuffix();
    suffix.insert(0,"-");
    n.remove(prefx);
    n.remove(suffix);
  }
  else
  {
    int index = nameA.indexOf(".");
    if (index > 0)
    {
      n = nameA.left(index);
      if (n == name)
      {
        index++;
        QString a = nameA.mid(index, -1);
        int i = a.length() - 1;
        while (a[i].isDigit())
          i--;
        if (i == a.length() - 1)
        {
          i = lookupPGFID(a);
          state->setAnchor(i);
        }
        else
        {
          i++;
          QString d = a.mid(i, -1);
          double angle = d.toDouble();
          i--;
          while (a[i].isSpace())
            i--;
          i++;
          a = a.left(i);
          i = lookupPGFID(a);
          state->setAnchor(i);
          state->setAngle(angle);
        }
        return true;
      }
    }
  }

  return (name == n);
}

bool XWTikzCoordinate::keyInput(XWTikzState * state)
{
  bool ret = options->keyInput(state);
  if (!ret && box)
  {
    QString str = state->getText();
    ret = box->keyInput(str);
  }
    
  if (!ret)
  {
    if (cur >= 0 && cur < children.size())
      ret = children[cur]->keyInput(state);
  }

  return ret;
}

bool XWTikzCoordinate::newPar(XWTikzState * state)
{
  bool ret = options->newPar(state);
  if (!ret && box)
    ret = box->newPar();
  if (!ret)
  {
    if (cur >= 0 && cur < children.size())
      ret = children[cur]->newPar(state);
  }

  return ret;
}

bool XWTikzCoordinate::paste(XWTikzState * state)
{
  bool ret = options->paste(state);
  if (!ret && box)
    ret = box->paste();
  if (!ret)
  {
    if (cur >= 0 && cur < children.size())
      ret = children[cur]->paste(state);
  }

  return ret;
}

void XWTikzCoordinate::scan(const QString & str, int & len, int & pos)
{
  options->scan(str,len,pos);

  while (str[pos].isSpace())
    pos++;

  if (str[pos] == QChar('('))
  {
    pos++;
    int i = pos;
    while (pos < len && str[pos] != QChar(')'))
      pos++;

    name = str.mid(i,pos - i);
    name = name.simplified();

    while (str[pos].isSpace())
      pos++;
  }

  if (str[pos] == QChar('c'))
  {
    pos += 5;
    scanChildren(str,len,pos);
  }
}

void XWTikzCoordinate::scanOption(const QString & str, int & len, int & pos)
{
  options->scan(str,len,pos);
}

void XWTikzCoordinate::scanText(const QString & , int & , int & )
{}

void XWTikzCoordinate::setText(const QString & )
{}

void XWTikzCoordinate::setName(const QString & str)
{
  name = str;
}

XWTikzCoordinate * XWTikzCoordinate::takeAt(int i)
{
  cur = i - 1;
  return children.takeAt(i);
}

void XWTikzCoordinate::addChild()
{
  XWTikzChild * child = new XWTikzChild(graphic,this);
  QUndoCommand * cmd = new XWTikzAddchild(this,cur+1,child);
  graphic->push(cmd);
}

void XWTikzCoordinate::addNode()
{
  XWTikzNodeDialog dlg;
  if (dlg.exec() == QDialog::Accepted)
  {
    XWTikzNode * n = new XWTikzNode(graphic,this);
    QString tmp = dlg.getName();    
    n->setName(tmp);
    tmp = dlg.getText();    
    n->setText(tmp);
    if (keyWord == PGFchild)
      box = n->box;
    XWTikzAddchild * cmd = new XWTikzAddchild(this,cur + 1,n);
    graphic->push(cmd);
  }
}

void XWTikzCoordinate::scanChildren(const QString & str, int & len, int & pos)
{
  while (str[pos].isSpace())
    pos++;
  if (str[pos] == QChar('{'))
  {
    pos++;
    while (pos < len)
    {
      if (str[pos].isSpace())
        pos++;
      else if (str[pos] == QChar('%'))
        XWTeXBox::skipComment(str,len,pos);
      else if (str[pos] == QChar('}'))
      {
        pos++;
        break;
      } 
      else
      {
        XWTikzCoordinate * obj = 0;
        int c = str[pos].unicode();
        switch (c)
        {
          default:
            break;

          case 'c':
            pos++;
            if (str[pos] == QChar('o'))
            {
              pos += 9;
              obj = new XWTikzCoordinate(graphic,this);
            }
            else
            {
              pos += 4;
              obj = new XWTikzChild(graphic,this); 
            }
            break;

          case 'e':
            pos += 4;
            while (str[pos].isSpace())
                pos++;
            pos += 4;
            while (str[pos].isSpace())
              pos++;
            pos += 6;
            obj = new XWTikzEdgeFromParent(graphic,this);
            break;

          case 'n':
            pos += 4;
            obj = new XWTikzNode(graphic,this);
            break;
        }

        if (obj)
        {
          children << obj;
          obj->scan(str,len,pos);
          if (keyWord == PGFchild && c == 'n')
            box = obj->box;
        }          
      }
    }
  }
}
XWTikzNode::XWTikzNode(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzCoordinate(graphicA,PGFnode,parent)
{}

bool XWTikzNode::addAction(QMenu & menu, XWTikzState * state)
{
  options->doPath(state,false);
  QMenu * submenu;
  switch (state->getPictureType())
  {
    default:
      options->addAnchorAction(menu);
      menu.addSeparator();     
      options->addShapeAction(menu);
      menu.addSeparator();
      submenu = menu.addMenu(tr("automata"));
      options->addStateAction(*submenu);
      menu.addSeparator();
      submenu = menu.addMenu(tr("entity relationship"));
      options->addEntityRelationshipAction(*submenu);
      menu.addSeparator();
      options->addColorAction(menu);
      menu.addSeparator();      
      options->addLabelAction(menu);
      menu.addSeparator();
      options->addTransformShapeAction(menu);
      break;

    case PGFmindmap:
      options->addConceptColorAction(menu);
      menu.addSeparator();
      options->addOpacityAction(menu);
      menu.addSeparator();
      options->addShadeAction(menu);
      menu.addSeparator();
      options->addConceptAction(menu);
      break;

    case PGFcircuiteeIEC:
      options->addCircuiteeSymbolAction(menu);
      menu.addSeparator();
      options->addCircuiteeUnitAction(menu);
      menu.addSeparator();
      options->addInfoAction(menu);
      menu.addSeparator();
      options->addPointAction(menu);
      menu.addSeparator();
      options->addColorAction(menu);
      menu.addSeparator();
      options->addLineWidthAction(menu);      
      break;

    case PGFcircuitlogicIEC:
    case PGFcircuitlogicUS:
    case PGFcircuitlogicCDH:
      options->addCircuitLogicSymbolAction(menu);
      menu.addSeparator();
      options->addInputAction(menu);
      menu.addSeparator();
      options->addInfoAction(menu);
      menu.addSeparator();
      options->addPointAction(menu);
      menu.addSeparator();
      options->addColorAction(menu);
      menu.addSeparator();
      options->addLineWidthAction(menu);
      break;
  }
  
  return true;
}

void XWTikzNode::doPath(XWTikzState * state, bool showpoint)
{
  if (!name.isEmpty())
    graphic->registNamed(name);
  state = state->saveNode(box,XW_TIKZ_NODE);   
  options->doPath(state,showpoint);

  if (graphic->getCurrentScope() == XW_TIKZ_S_OPERATION)
  {
    if (cur >= 0 && cur < children.size())
    {
      for (int i = 0; i <= cur; i++)
        children[i]->doPath(state,showpoint);
    }
  }
  else
  {
    for (int i = 0; i < children.size(); i++)
      children[i]->doPath(state,showpoint);
  }
}

void XWTikzNode::dragTo(XWTikzState * state)
{
  state->dragTo(box);
}

bool XWTikzNode::dropTo(XWTikzState * state)
{
  if (!box)
    return false;

  XWTikzState mystate;
  state->copy(&mystate);
  if (!mystate.moveTest(box))
    return false;

  XWTikzOperation * op = graphic->getCurrentOperation();
  if (!op || op->getKeyWord() != PGFnode)
    return false;

  bool ret = options->dropTo(&mystate);
  QPointF mp = mystate.getLastMousePoint();
  if (op == this)
    ret = box->dropTo(mp.x(),mp.y());
  else
  {//从结点op移动到本节点
    QString str = box->getSelected();
    if (!str.isEmpty())
    {
      QUndoCommand * cmd = new QUndoCommand;
      if (!box->removeSelected(cmd))
        delete cmd;
      else
      {
        if (!box->insert(str,cmd))
          delete cmd;
        else
          box->push(cmd);
      }
    }
  }

  return ret;
}

int XWTikzNode::getAnchorPosition()
{
  int ret = options->getAnchorPosition();
  if (ret <= 0 && box)
    ret = box->getAnchorPosition();
  if (!ret)
    ret = XWTikzCoordinate::getAnchorPosition();

  return ret;
}

QString XWTikzNode::getContent()
{
  QString ret;
  if (box)
  {
    ret = "{";
    QString tmp = box->getText();
    ret += tmp;
    ret += "}";
  }

  for (int i = 0; i < children.size(); i++)
  {
    QString tmp = children[i]->getText();
    ret += tmp;
    ret += "\n";
  }

  return ret;
}

int XWTikzNode::getCursorPosition()
{
  int ret = options->getCursorPosition();
  if (ret <= 0 && box)
    ret = box->getCursorPosition();
  if (ret <= 0)
    ret = XWTikzCoordinate::getCursorPosition();

  return ret;
}

QString XWTikzNode::getCurrentText()
{
  QString ret = options->getCurrentText();
  if (ret.isEmpty() && box)
    ret = box->getCurrentText();
  if (ret.isEmpty())
    ret = XWTikzCoordinate::getCurrentText();

  return ret;
}

QString XWTikzNode::getSelectedText()
{
  QString ret = options->getSelectedText();
  if (ret.isEmpty() && box)
    ret = box->getSelectedText();
  if (ret.isEmpty())
    ret = XWTikzCoordinate::getSelectedText();

  return ret;
}

QString XWTikzNode::getText()
{
  QString ret = " node";
  QString tmp = options->getText();
  ret += tmp;
  if (!name.isEmpty())
  {
    ret += " (";
    ret += name;
    ret += ") ";
  }

  if (box)
  {
    ret += "{";
    tmp = box->getText();
    ret += tmp;
    ret += "}";
  }

  for (int i = 0; i < children.size(); i++)
  {
    tmp = children[i]->getText();
    ret += tmp;
    ret += "\n";
  }

  return ret;
}

QString XWTikzNode::getTextForPath()
{
  QString ret = "node";
  QString tmp = options->getText();
  ret += tmp;
  if (!name.isEmpty())
  {
    ret += " (";
    ret += name;
    ret += ") ";
  }

  return ret;
}

bool XWTikzNode::goToNext()
{
  bool ret = options->goToNext();
  if (!ret && box)
    ret = box->goToNext();
  if (!ret)
    ret = XWTikzCoordinate::goToNext();

  return ret;
}

bool XWTikzNode::goToPrevious()
{
  bool ret = options->goToPrevious();
  if (!ret && box)
    ret = box->goToPrevious();
  if (!ret)
    ret = XWTikzCoordinate::goToPrevious();

  return ret;
}

bool XWTikzNode::hitTest(XWTikzState * state)
{
  XWTikzState mystate;
  state->copy(&mystate);
  bool ret = options->hitTest(&mystate);
  if (!ret && box)
  {
    if (options->isMatrix())
      ret = mystate.hitTestMatrix((XWTikzMatrix*)box);
    else
      ret = mystate.hitTestText((XWTikzTextBox*)box);
  }

  if (!ret)
  {
    cur = -1;
    mystate.setChildrenNumber(children.size());
    for (int i = 0; i < children.size(); i++)
    {
      mystate.setCurrentChild(i);
      ret = children[i]->hitTest(&mystate);
      if (ret)
      {
        cur = i;
        break;
      }
    }
  }

  return ret;
}

void XWTikzNode::scan(const QString & str, int & len, int & pos)
{
  options->scan(str,len,pos);

  while (str[pos].isSpace())
    pos++;

  if (str[pos] == QChar('('))
  {
    pos++;
    int i = pos;
    while (pos < len && str[pos] != QChar(')'))
      pos++;

    name = str.mid(i,pos - i);
    name = name.simplified();

    while (str[pos].isSpace())
      pos++;
  }

  if (str[pos] != QChar('a'))
    scanText(str,len,pos);
}

void XWTikzNode::scanText(const QString & str, int & len, int & pos)
{
  while (str[pos].isSpace())
    pos++;

  if (str[pos] != QChar('{'))
  {
    if (str[pos] == QChar('c'))
    {
      pos += 5;
      scanChildren(str,len,pos);
    }
    return ;
  }
  
  pos++;
  if (options->isMatrix())
    box = new XWTikzMatrix(graphic,this);
  else
    box = new XWTikzTextBox(graphic,this);
  box->scan(str,len,pos);
  pos++;

  while (str[pos].isSpace())
    pos++;

  if (str[pos] == QChar('c'))
  {
    pos += 5;
    scanChildren(str,len,pos);
  }
}

void XWTikzNode::setText(const QString & str)
{
  if (box)
    delete box;

  box = new XWTikzTextBox(graphic,this);
  int len = str.length();
  if (len == 0)
    return ;
    
  int pos = 0;
  box->scan(str,len,pos);
}

XWTikzEdge::XWTikzEdge(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA,PGFedge,parent),
 options(0),
 coord(0),
 cur(-1)
{
  options = new XWTIKZOptions(graphicA, this);
  coord = new XWTikzCoord(graphicA,this);
}

bool XWTikzEdge::addAction(QMenu & menu, XWTikzState * state)
{
  options->doPath(state,false);
  QAction * a = menu.addAction(tr("add node"));
  connect(a, SIGNAL(triggered()), this, SLOT(addNode()));
  if (cur >= 0)
  {
    a = menu.addAction(tr("remove node"));
    connect(a, SIGNAL(triggered()), this, SLOT(removeNode()));
  }
  
  return true;
}

bool XWTikzEdge::back(XWTikzState * state)
{
  if (cur < 0 || cur >= nodes.size())
    return false;

  return nodes[cur]->back(state);
}

bool XWTikzEdge::cut(XWTikzState * state)
{
  if (cur < 0 || cur >= nodes.size())
    return false;

  return nodes[cur]->cut(state);
}

bool XWTikzEdge::del(XWTikzState * state)
{
  if (cur < 0 || cur >= nodes.size())
    return false;

  return nodes[cur]->del(state);
}

void XWTikzEdge::doPath(XWTikzState * state, bool showpoint)
{
  options->doPath(state,showpoint);
  state->addEdge(coord);
  if (showpoint)
    coord->draw(state);
  for (int i = 0; i < nodes.size(); i++)
    nodes[i]->doPath(state,false);
}

bool XWTikzEdge::dropTo(XWTikzState * state)
{
  if (cur < 0 || cur >= nodes.size())
    return coord->dropTo(state);

  return nodes[cur]->dropTo(state);
}

int XWTikzEdge::getCursorPosition()
{
  if (cur < 0 || cur >= nodes.size())
    return 0;

  return nodes[cur]->getCursorPosition();
}

QString XWTikzEdge::getCurrentText()
{
  QString ret;
  if (cur >=0 && cur < nodes.size())
    ret = nodes[cur]->getCurrentText();

  return ret;
}

QString XWTikzEdge::getSelectedText()
{
  QString ret;
  if (cur >=0 || cur < nodes.size())
    ret = nodes[cur]->getSelectedText();

  return ret;
}

void XWTikzEdge::goToEnd()
{
  if (cur < 0 || cur >= nodes.size())
    return ;

  nodes[cur]->goToEnd();
}

bool XWTikzEdge::goToNext()
{
  if (cur < 0 || cur >= nodes.size())
    return false;

  bool ret = nodes[cur]->goToNext();
  if (!ret)
  {
    if (cur < nodes.size() - 1)
    {
      cur++;
      nodes[cur]->goToStart();
      ret = true;
    }
  }

  return ret;
}

bool XWTikzEdge::goToPrevious()
{
  if (cur < 0 || cur >= nodes.size())
    return false;

  bool ret = nodes[cur]->goToPrevious();
  if (!ret)
  {
    if (cur > 0)
    {
      cur--;
      nodes[cur]->goToEnd();
      ret = true;
    }
  }

  return ret;
}

void XWTikzEdge::goToStart()
{
  if (cur >= 0 && nodes.size() > 0)
    nodes[cur]->goToStart();
}

QString XWTikzEdge::getText()
{
  QString ret = " edge";
  QString tmp = options->getText();
  ret += tmp;

  for (int i = 0; i < nodes.size(); i++)
  {
    tmp = nodes[i]->getText();
    ret += tmp;
    ret += "\n";
  }

  int len = ret.length();
  if (ret[len - 1] == QChar('\n'))
    ret.remove(len - 1, 1);

  ret += " ";
  tmp = coord->getText();
  return ret;
}

bool XWTikzEdge::hitTest(XWTikzState * state)
{
  cur = -1;
  bool ret = options->hitTest(state);
  if (!ret)
    ret = coord->hitTest(state);
  if (!ret)
  {
    for (int i = 0; i < nodes.size(); i++)
    {
      if (nodes[i]->hitTest(state))
      {
        cur = i;
        break;
      }
    }
  }

  return ret;
}

void XWTikzEdge::insert(int i, XWTikzCoordinate * node)
{
  cur = i;
  if (i >= nodes.size())
    nodes << node;
  else
    nodes.insert(i,node);
}

bool XWTikzEdge::insertText(XWTikzState * state)
{
  if (cur < 0 || cur >= nodes.size())
    return false;

  return nodes[cur]->insertText(state);
}

bool XWTikzEdge::keyInput(XWTikzState * state)
{
  if (cur < 0 || cur >= nodes.size())
    return false;

  return nodes[cur]->keyInput(state);
}

bool XWTikzEdge::newPar(XWTikzState * state)
{
  if (cur < 0 || cur >= nodes.size())
    return false;

  return nodes[cur]->newPar(state);
}

bool XWTikzEdge::paste(XWTikzState * state)
{
  if (cur < 0 || cur >= nodes.size())
    return false;

  return nodes[cur]->paste(state);
}

void XWTikzEdge::scan(const QString & str, int & len, int & pos)
{
  options->scan(str,len,pos);

  while (str[pos].isSpace())
    pos++;

  while (str[pos] != QChar('('))
  {
    if (str[pos].isSpace())
      pos++;
    else if (str[pos] == QChar('%'))
      XWTeXBox::skipComment(str,len,pos);
    else if (str[pos] == QChar('n'))
    {
      pos += 4;
      XWTikzNode * node = new XWTikzNode(graphic,this);
      node->scanOption(str,len,pos);
      node->scanText(str,len,pos);
      nodes << node;
    }
    else
    {
      pos += 10;
      XWTikzCoordinate * node = new XWTikzCoordinate(graphic,this);
      node->scanOption(str,len,pos);
      nodes << node;
    }
  }

  coord->scan(str,len,pos);
}

void XWTikzEdge::setCoord(const QString & str)
{
  coord->setText(str);
}

XWTikzCoordinate * XWTikzEdge::takeAt(int i)
{
  cur = i - 1;
  return nodes.takeAt(i);
}

void XWTikzEdge::addNode()
{
  XWTikzNodeDialog dlg;
  if (dlg.exec() == QDialog::Accepted)
  {
    XWTikzNode * n = new XWTikzNode(graphic,this);
    QString tmp = dlg.getText();    
    n->setText(tmp);
    XWTikzAddNode * cmd = new XWTikzAddNode(this,cur + 1,n);
    graphic->push(cmd);
  }
}

void XWTikzEdge::removeNode()
{
  if (cur < 0)
    return ;

  XWTikzRemoveNode * cmd = new XWTikzRemoveNode(this,cur + 1);
  graphic->push(cmd);
}

XWTikzLabel::XWTikzLabel(XWTikzGraphic * graphicA, int idA, QObject * parent)
:XWTikzOperation(graphicA,idA,parent),
options(0),
angle(XW_TIKZ_LABEL_NOANGLE),
box(0)
{
}

XWTikzLabel::XWTikzLabel(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA,PGFlabel,parent),
options(0),
angle(XW_TIKZ_LABEL_NOANGLE),
box(0)
{
}

bool XWTikzLabel::back(XWTikzState * )
{
  return box->back();
}

bool XWTikzLabel::cut(XWTikzState * )
{
  return box->cut();
}

bool XWTikzLabel::del(XWTikzState * )
{
  return box->del();
}

void XWTikzLabel::doPath(XWTikzState * state, bool showpoint)
{
  switch (keyWord)
  {
    default:
      state = state->saveNode(box,XW_TIKZ_LABEL);
      if (angle != XW_TIKZ_LABEL_NOANGLE)
        state->setLabelAngle(angle);
      break;

    case PGFpin:
      state = state->saveNode(box,XW_TIKZ_PIN);   
      if (angle != XW_TIKZ_LABEL_NOANGLE)
        state->setLabelAngle(angle);   
      break;

    case PGFinfo:
      state = state->saveNode(box,XW_TIKZ_INFO);
      if (angle != XW_TIKZ_LABEL_NOANGLE)
        state->setLabelAngle(angle);  
      break;

    case PGFinfomissingangle:
      state = state->saveNode(box,XW_TIKZ_LABEL);
      if (angle == XW_TIKZ_LABEL_NOANGLE)
        state->setLabelAngle(PGFbelow);
      else
         state->setLabelAngle(angle);
      break;

    case PGFinfosloped:
      state = state->saveNode(box,XW_TIKZ_LABEL);
      if (angle != XW_TIKZ_LABEL_NOANGLE)
        state->setLabelAngle(angle);
      state->setTransformShape();
      break;

    case PGFinfomissinganglesloped:
      state = state->saveNode(box,XW_TIKZ_LABEL);
      if (angle == XW_TIKZ_LABEL_NOANGLE)
        state->setLabelAngle(PGFbelow);
      else
        state->setLabelAngle(angle);
      state->setTransformShape();
      break;
  }
  options->doPath(state,showpoint);
}

bool XWTikzLabel::dropTo(XWTikzState * )
{
  return false;
}

int XWTikzLabel::getAnchorPosition()
{
  return box->getAnchorPosition();
}

QString XWTikzLabel::getAngle()
{
  QString ret;
  if (angle == XW_TIKZ_LABEL_NOANGLE)
    return ret;

  if (angle >= PGF_MIN_KEYWORD)
  {
    int key = (int)(angle);
    ret = getPGFString(key);
  }
  else
    ret = QString("%1").arg(angle);
  return ret;
}

QString XWTikzLabel::getContent()
{
  QString ret;
  if (box)
    ret = box->getText();
  return ret;
}

int XWTikzLabel::getCursorPosition()
{
  return box->getCursorPosition();
}

QString XWTikzLabel::getCurrentText()
{
  return box->getCurrentText();
}

QString XWTikzLabel::getOptions()
{
  return options->getContent();
}

QString XWTikzLabel::getSelectedText()
{
  return box->getSelectedText();
}

QString XWTikzLabel::getText()
{
  QString ret = getPGFString(keyWord);
  ret += "=";
  QString tmp = options->getText();
  bool b = false;
  if (!tmp.isEmpty())
  {
    b =true;
    ret += "{";
    ret += tmp;
  }

  if (angle >= PGF_MIN_KEYWORD)
  {
    int key = (int)(angle);
    tmp = getPGFString(key);
    ret += tmp;
  }
  else
  {
    if (angle != XW_TIKZ_LABEL_NOANGLE)
    {
      tmp = QString("%1").arg(angle);
      ret += tmp;
    }
  }

  ret += ":";
  tmp = box->getText();
  ret += tmp;
  if (b)
    ret += "}";
  
  return ret;
}

void XWTikzLabel::goToEnd()
{
  box->goToEnd();
}

bool XWTikzLabel::goToNext()
{
  return box->goToNext();
}

bool XWTikzLabel::goToPrevious()
{
  return box->goToPrevious();
}

void XWTikzLabel::goToStart()
{
  box->goToStart();
}

bool XWTikzLabel::hitTest(XWTikzState * )
{
  return false;
}

bool XWTikzLabel::insertText(XWTikzState * state)
{
  QString str = state->getText();
  return box->insertText(str);
}

bool XWTikzLabel::keyInput(XWTikzState * state)
{
  QString str = state->getText();
  return box->keyInput(str);
}

bool XWTikzLabel::newPar(XWTikzState * )
{
  return box->newPar();
}

bool XWTikzLabel::paste(XWTikzState * )
{
  return box->paste();
}

void XWTikzLabel::scan(const QString & str, int & len, int & pos)
{
  if (box)
  {
    delete box;
    box = new XWTikzTextBox(graphic, this);
  }

  if (options)
  {
    delete options;
    options = new XWTIKZOptions(graphic, this);
  }
  
  while (str[pos].isSpace())
    pos++;

  if (str[pos] == QChar('='))
  {
    pos++;
    while (str[pos].isSpace())
      pos++;
  }

  if (str[pos] == QChar('{'))
    pos++;

  options->scan(str,len,pos);
  while (str[pos].isSpace())
    pos++;
  int i = pos;
  while (str[pos] != QChar(':'))
    pos++;

  QString tmp = str.mid(i,pos);
  tmp = tmp.simplified();
  if (!tmp.isEmpty())
  {
    if (tmp[0].isDigit())
      angle = tmp.toDouble();
    else
      angle = lookupPGFID(tmp);

    pos++;
  }

  i = pos;
  while (str[pos] != QChar(']') && 
         str[pos] != QChar('}') &&
         str[pos] != QChar(','))
  {
    pos++;
  }

  tmp = str.mid(i,pos);
  int l = tmp.length();
  int p = 0;
  box->scan(tmp,l,p);
  if (str[pos] == QChar('}') || str[pos] == QChar(','))
    pos++;
}

void XWTikzLabel::setAngle(const QString & str)
{
  if (!str.isEmpty())
  {
    if (str[0].isDigit())
      angle = str.toDouble();
    else
      angle = lookupPGFID(str);
  }
}

void XWTikzLabel::setText(const QString & str)
{
  if (box)
  {
    delete box;
    box = new XWTikzTextBox(graphic, this);
  }

  int len = str.length();
  int pos = 0;
  box->scan(str,len,pos);
}

XWTikzChild::XWTikzChild(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzCoordinate(graphicA,PGFchild,parent)
{
}

bool XWTikzChild::addAction(QMenu & menu, XWTikzState * state)
{
  options->doPath(state,false);
  QAction * a = menu.addAction(tr("add child"));
  connect(a, SIGNAL(triggered()), this, SLOT(addChild()));
  a = menu.addAction(tr("add node"));
  connect(a, SIGNAL(triggered()), this, SLOT(addNode()));
  return true;
}

void XWTikzChild::doPath(XWTikzState * state, bool showpoint)
{
  if (!box)
  {
    for (int i = 0; i < children.size(); i++)
    {
      if (children[i]->getKeyWord() == PGFnode)
      {
        XWTikzCoordinate * n = (XWTikzCoordinate*)(children[i]);
        box = n->getBox();
        break;
      }
    }
  }
  state = state->saveNode(box,XW_TIKZ_CHILD);
  options->doPath(state,showpoint);
  for (int i = 0; i < list.size(); i++)
  {
    QString tmp = list[i];
    QStringList vars = tmp.split(QChar('/'));
    for (int j = 0; j < vars.size(); j++)
    {
      QString var = vars[j];
      state->setVariables(variables[j],var);
    }
  }

  if (graphic->getCurrentScope() == XW_TIKZ_S_OPERATION)
  {
    if (cur >= 0 && cur < children.size())
    {
      for (int i = 0; i <= cur; i++)
        children[i]->doPath(state,showpoint);
    }
  }
  else
  {
    for (int i = 0; i < children.size(); i++)
      children[i]->doPath(state,showpoint);
  }
}

bool XWTikzChild::dropTo(XWTikzState * state)
{
  XWTikzState mystate;
  state->copy(&mystate);
  bool ret = options->dropTo(&mystate);
  if (!ret)
    ret = mystate.hitTestChild(box);
  if (!ret)
  {
    mystate.setChildrenNumber(children.size());
    for (int i = 0; i < children.size(); i++)
    {
      mystate.setCurrentChild(i);
      ret = children[i]->dropTo(&mystate);
      if (ret)
        break;
    }
  }
  
  return ret;
}

QString XWTikzChild::getText()
{
  QString ret = " child";
  QString tmp = options->getText();
  ret += tmp;

  if (!variables.isEmpty())
  {
    ret += " foreach ";
    tmp = variables.join("/");
    ret += tmp;
    ret += " in {";
    tmp = list.join(",");
    ret += tmp;
    ret += "} ";
  }

  if (children.size() > 0)
  {
    ret += "{";
    for (int i = 0; i < children.size(); i++)
    {
      tmp = children[i]->getText();
      ret += tmp;
      ret += "\n";
    }
    ret += "} ";
  }

  return ret;
}

bool XWTikzChild::hitTest(XWTikzState * state)
{
  XWTikzState mystate;
  state->copy(&mystate);
  bool ret = options->hitTest(&mystate);
  if (!ret)
    ret = mystate.hitTestChild(box);
  if (!ret)
  {
    cur = -1;
    for (int i = 0; i < children.size(); i++)
    {
      if (children[i]->hitTest(&mystate))
      {
        cur = i;
        break;
      }
    }
  }

  return ret;
}

void XWTikzChild::scan(const QString & str, int & len, int & pos)
{
  options->scan(str,len,pos);
  while (str[pos].isSpace())
    pos++;

  if (str[pos] == QChar('f'))
  {
    pos += 7;
    while (pos < len)
    {
      if (str[pos] == QChar('{'))
        break;
      else if (str[pos].isSpace() || str[pos] == QChar('/'))
        pos++;
      else if (str[pos] == QChar('\\'))
      {
        int i = pos;
        while (str[pos].isLetter())
          pos++;

        QString v = str.mid(i,pos);
        v = v.simplified();
        variables << v;
      }
      else if (str[pos] == QChar('i'))
        pos += 2;
    }
    pos++;
    int b = 0;
    bool incoord = false;
    int i = pos;
    while (pos < len)
    {
      if (str[pos].isSpace())
        pos++;
      else 
      {
        if (str[pos] == QChar('}'))
        {
          b--;
          if (b < 0)
          {
            QString v = str.mid(i,pos-i);
            v = v.simplified();
            list << v;
            pos++;
            break;
          }
        }
        else if (str[pos] == QChar('('))
          incoord = true;
        else if (str[pos] == QChar(')'))
          incoord = false;
        else if (str[pos] == QChar('{'))
          b++;
        else if (str[pos] == QChar(','))
        {
          if (!incoord)
          {
            QString v = str.mid(i,pos-i);
            v = v.simplified();
            list << v;
            i = pos + 1;          
          }
        }

        pos++;
      }
    }
    while (str[pos].isSpace())
      pos++;
  }

  scanChildren(str,len,pos);
}

XWTikzEdgeFromParent::XWTikzEdgeFromParent(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzCoordinate(graphicA,PGFedgefromparent,parent)
{
}

void XWTikzEdgeFromParent::doPath(XWTikzState * state, bool showpoint)
{
  state = state->saveNode(box,XW_TIKZ_EDGE);
  options->doPath(state,showpoint);
}

QString XWTikzEdgeFromParent::getText()
{
  QString ret = " edge from parent";
  QString tmp = options->getText();
  ret += tmp;
  return ret;
}

void XWTikzEdgeFromParent::scan(const QString & str, int & len, int & pos)
{
  options->scan(str,len,pos);
}

XWTikzOperationText::XWTikzOperationText(XWTikzGraphic * graphicA, int id, QObject * parent)
:XWTikzOperation(graphicA,id,parent)
{}

QString XWTikzOperationText::getText()
{
  QString key = getPGFString(keyWord);
  QString ret = QString("%1={\n%2}\n").arg(key).arg(text);
  return ret;
}

void XWTikzOperationText::doChildAnchor(XWTikzState * state)
{
  if (text.isEmpty())
    return ;

  int i = text.indexOf("\\\\tikzchildnode");
  if (i < 0)
    return ;

  i += 14;
  if (text[i] == QChar('.'))
  {
    i++;
    int pos = i;
    while (text[pos] != QChar(')'))
      pos++;

    QString str = text.mid(i,pos - i);
    if (str[0].isDigit())
    {
      double a = str.toDouble();
      state->setAngle(a);
    }
    else
    {
      int a = lookupPGFID(str);
      state->setAnchor(a);
    }
  }
}

void XWTikzOperationText::doParentAnchor(XWTikzState * state)
{
  if (text.isEmpty())
    return ;

  int i = text.indexOf("\\\\tikzparentnode");
  if (i < 0)
    return ;

  i += 15;
  if (text[i] == QChar('.'))
  {
    i++;
    int pos = i;
    while (text[pos] != QChar(')'))
      pos++;

    QString str = text.mid(i,pos - i);
    if (str[0].isDigit())
    {
      double a = str.toDouble();
      state->setAngle(a);
    }
    else
    {
      int a = lookupPGFID(str);
      state->setAnchor(a);
    }
  }
}

void XWTikzOperationText::doPath(XWTikzState * state, bool showpoint)
{
  if (text.isEmpty())
    state->doToPath();
  else
  {
    QPointF s = state->getToStart();
    QPointF t = state->getToTarget();
    QString sstr = QString("%1,%2").arg(s.x()).arg(s.y());
    QString tstr = QString("%1,%2").arg(t.x()).arg(t.y());
    QString str = text;

    switch (keyWord)
    {
      default:
        break;

      case PGFtopath:
        str.replace(QRegExp("\\\\tikztostart"),sstr);
        str.replace(QRegExp("\\\\tikztotarget\\s*\\\\tikztonodes"),tstr);
        break;

      case PGFedgefromparentpath:
        str.replace(QRegExp("\\\\tikzparentnode[\\.\\\\]?\\w*"),sstr);
        str.replace(QRegExp("\\\\tikzchildnode[\\.\\\\]?\\w*"),tstr);
        break;
    }

    scan(str);
    state = state->save(false);
    for (int i = 0; i < ops.size(); i++)
      ops[i]->doPath(state,showpoint);
    state = state->restore();

    while (!ops.isEmpty())
      delete ops.takeFirst();
  }
}

void XWTikzOperationText::scan(const QString & str, int & len, int & pos)
{
  while (str[pos].isSpace())
    pos++;
  pos++;
  int i = pos;
  int b = 0;
  while (pos < len)
  {
    if (str[pos] == QChar('%'))
      XWTeXBox::skipComment(str,len,pos);
    else if (str[pos] == QChar('}'))
    {
      b--;
      if (b < 0)
      {
        text = str.mid(i,pos);
        pos++;
        break;
      }
      pos++;
    }
    else if (str[pos] == QChar('{'))
    {
      b++;
      pos++;
    }
    else    
      pos++;
  }

  while (str[pos].isSpace())
    pos++;

  if (str[pos] == QChar(','))
    pos++;
}

void XWTikzOperationText::scan(const QString & str)
{
  int len = str.length();
  int pos = 0;
  XWTikzOperation * obj = 0;
  while (pos < len)
  {
    if (str[pos].isSpace())
      pos++;
    else if (str[pos] == QChar('%'))
      XWTeXBox::skipComment(str,len,pos);
    else if (str[pos] == QChar('}'))
    {
      pos++;
      break;
    }      
    else
    {
      int c = str[pos].unicode();
      switch (c)
      {
        case '(':
          obj = new XWTikzCoord(graphic,this);
          break;

        case '|':
          pos += 2;
          obj = new XWTikzVHLinesTo(graphic,this);
          break;

        case '\\':
          obj = new XWTikzMacro(graphic,this);
          break;

        case '-':
          pos++;
          if (str[pos] == QChar('|'))
          {
            pos++;
            obj = new XWTikzHVLinesTo(graphic,this);
          }            
          else
          {
            pos++;
            while (str[pos].isSpace())
              pos++;
            if (str[pos] == QChar('(') || str[pos] == QChar('+'))
              obj = new XWTikzLineTo(graphic,this);
            else
            {
              pos += 4;
              XWTIKZOptions * opts = new XWTIKZOptions(graphic);
              opts->scan(str,len,pos);
              while (str[pos].isSpace())
                pos++;
              if (str[pos] == QChar('c'))
              {
                pos += 11;
                obj = new XWTikzPlotCoordinates(graphic,true,opts,this);
              }                
              else if (str[pos] == QChar('('))
                obj = new XWTikzPlotFunction(graphic,true,opts,this);
              else
              {
                pos += 4;
                obj = new XWTikzPlotFile(graphic,true,opts,this);
              }
            }
          }
          break;

        case 'a':
          pos += 3;
          obj = new XWTikzArc(graphic,this);
          break;

        case 'c':
          pos++;
          if (str[pos] == QChar('i'))
          {
            pos += 5;
            obj = new XWTikzEllipse(graphic,PGFcircle,this);
          }
          else if (str[pos] == QChar('o'))
          {
            pos++;
            if (str[pos] == QChar('n'))
            {
              pos += 6;
              obj = new XWTikzCurveTo(graphic,this);
            }
            else if (str[pos] == QChar('s'))
            {
              pos++;
              obj = new XWTikzCosine(graphic,this);
            }      
            else
            {
              pos += 8;
              obj = new XWTikzCoordinate(graphic,this);
            }      
          }
          else if (str[pos] == QChar('h'))
          {
            pos += 4;
            obj = new XWTikzChild(graphic,this);
          }
          else
          {
            pos += 4;
            obj = new XWTikzCycle(graphic,this);
          }            
          break;

        case 'd':
          pos += 8;
          obj = new XWTikzDecorate(graphic,this);
          break;

        case 'e':
          pos++;
          if (str[pos] == QChar('d'))
          {
            pos += 3;
            while (str[pos].isSpace())
              pos++;
            if (str[pos] == 'f')
            {
              pos += 4;
              while (str[pos].isSpace())
                pos++;
              pos += 6;
              obj = new XWTikzEdgeFromParent(graphic,this);
            }
            else
              obj = new XWTikzEdge(graphic,this);
          }
          else
          {
            pos += 6;
            obj = new XWTikzEllipse(graphic,PGFellipse,this);
          }
          break;

        case 'g':
          pos += 4;
          obj = new XWTikzGrid(graphic,this);
          break;

        case 'n':
          pos += 4;
          obj = new XWTikzNode(graphic,this);
          break;

        case 'p':
          pos++;
          if (str[pos] == QChar('l'))
          {
            pos += 3;
            XWTIKZOptions * opts = new XWTIKZOptions(graphic);
            opts->scan(str,len,pos);
            while (str[pos].isSpace())
              pos++;
            if (str[pos] == QChar('c'))
            {
              pos += 11;
              obj = new XWTikzPlotCoordinates(graphic,false,opts,this);
            }                
            else if (str[pos] == QChar('('))
              obj = new XWTikzPlotFunction(graphic,false,opts,this);
            else
            {
              pos += 4;
              obj = new XWTikzPlotFile(graphic,false,opts,this);
            }
          }
          else
          {
            pos += 7;
            obj = new XWTikzParabola(graphic,this);
          }          
          break;

        case 's':
          pos += 3;
          obj = new XWTikzSine(graphic,this);
          break;

        case 't':
          pos += 2;
          obj = new XWTikzTo(graphic,this);
          break;

        case 'r':
          pos += 9;
          obj = new XWTikzRectangle(graphic,this);
          break;

        case '[':
          obj = new XWTIKZOptions(graphic,this);
          break;

        case '{':
          obj = new XWTikzLocal(graphic,this);
          break;

        default:
          break;
      }

      if (obj)
      {
        ops << obj;
        obj->scan(str,len,pos);
      }
    }
  }
}

XWTikzPathText::XWTikzPathText(XWTikzGraphic * graphicA, int id, QObject * parent)
:XWTikzOperation(graphicA,id,parent)
{}

void XWTikzPathText::doPath(XWTikzState * state, bool)
{
  QPointF s = state->getToStart();
  QPointF t = state->getToTarget();
  QString sstr = QString("%1,%2").arg(s.x()).arg(s.y());
  QString tstr = QString("%1,%2").arg(t.x()).arg(t.y());
  QString str = text;
  switch (keyWord)
  {
    default:
      break;

    case PGFspyconnectionpath:
      if (str.isEmpty())
        str = QString("\\draw[thin] (%1) -- (%2);").arg(sstr).arg(tstr);
      else
      {
        str.replace("tikzspyonnode",sstr);
        str.replace("tikzspyinnode",tstr);
      }
      break;
  }

  scan(str);
  for (int i = 0; i < cmds.size(); i++)
    cmds[i]->doPath(state,false);

  while (!cmds.isEmpty())
    delete cmds.takeFirst();
}

QString XWTikzPathText::getText()
{
  QString key = getPGFString(keyWord);
  QString ret = QString("%1={\n%2}\n").arg(key).arg(text);
  return ret;
}

void XWTikzPathText::scan(const QString & str, int & len, int & pos)
{
  while (str[pos].isSpace())
    pos++;
  pos++;
  int i = pos;
  int b = 0;
  while (pos < len)
  {
    if (str[pos] == QChar('%'))
      XWTeXBox::skipComment(str,len,pos);
    else if (str[pos] == QChar('}'))
    {
      b--;
      if (b < 0)
      {
        text = str.mid(i,pos);
        pos++;
        break;
      }
      pos++;
    }
    else if (str[pos] == QChar('{'))
    {
      b++;
      pos++;
    }
    else    
      pos++;
  }

  while (str[pos].isSpace())
    pos++;

  if (str[pos] == QChar(','))
    pos++;
}

void XWTikzPathText::scan(const QString & str)
{
  int len = str.length();
  int pos = 0;
  while (pos < len)
  {
    if (str[pos].isSpace())
      pos++;
    else if (str[pos] == QChar('%'))
      XWTeXBox::skipComment(str,len,pos);
    else
    {
      if (str[pos] == QChar('\\'))
      {
        QString key = XWTeXBox::scanControlSequence(str,len,pos);
        int id = lookupPGFID(key);
        XWTikzCommand * cmd = createPGFObject(graphic,0,id,this);
        cmds << cmd;
        cmd->scan(str,len,pos);
      }
      else
        pos++;
    }
  }
}

XWTikzNodePart::XWTikzNodePart(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA,PGFnodepart,parent)
{}

QString XWTikzNodePart::getText()
{
  QString ret = QString("\n\\nodepart{%1}\n").arg(text);
  return ret;
}

void XWTikzNodePart::scan(const QString & str, int & len, int & pos)
{
  while (str[pos].isSpace())
    pos++;
  pos++;
  int i = pos;
  int b = 0;
  while (pos < len)
  {
    if (str[pos] == QChar('%'))
      XWTeXBox::skipComment(str,len,pos);
    else if (str[pos] == QChar('}'))
    {
      b--;
      if (b < 0)
      {
        text = str.mid(i,pos);
        pos++;
        break;
      }
      pos++;
    }
    else if (str[pos] == QChar('{'))
    {
      b++;
      pos++;
    }
    else    
      pos++;
  }
}

XWTikzUnit::XWTikzUnit(XWTikzGraphic * graphicA, 
             const QString & nameA,
             QObject * parent)
:XWTikzLabel(graphicA, PGFohm,parent),
name(nameA)
{}

void XWTikzUnit::doPath(XWTikzState * state, bool )
{
  state = state->saveNode(box,XW_TIKZ_LABEL);
  if (name.endsWith("'"))
    state->setLabelAngle(PGFbelow);  
  else if (name.endsWith("' sloped"))
  {
    state->setLabelAngle(PGFbelow);
    state->setTransformShape();
  }
  else if (name.endsWith("sloped"))
    state->setTransformShape();
}

QString XWTikzUnit::getText()
{
  QString ret = QString("%1=%2").arg(name).arg(value);
  return ret;
}

void XWTikzUnit::getUnit(QString & nameA, QString & valueA)
{
  nameA = name;
  valueA = value;
}

void XWTikzUnit::scan(const QString & str, int & len, int & pos)
{
  scanValue(str,len,pos,value);
  QString u = graphic->getUnit(name);
  QString tmp = QString("%1%2").arg(value).arg(u);
  int slen = tmp.length();
  int spos = 0;
  box->scan(tmp, slen, spos);
}

void XWTikzUnit::setUnit(const QString & nameA,const QString & valueA)
{
  name = nameA;
  value = valueA;
  if (box)
    delete box;
  box = new XWTikzTextBox(graphic, this);
  QString u = graphic->getUnit(name);
  QString tmp = QString("%1%2").arg(value).arg(u);
  int slen = tmp.length();
  int spos = 0;
  box->scan(tmp, slen, spos);
}

void XWTikzUnit::setValue(const QString & valueA)
{
  value = valueA;
  if (box)
    delete box;
  box = new XWTikzTextBox(graphic, this);
  QString u = graphic->getUnit(name);
  QString tmp = QString("%1%2").arg(value).arg(u);
  int slen = tmp.length();
  int spos = 0;
  box->scan(tmp, slen, spos);
}
