/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QCoreApplication>
#include <QAction>
#include <QDir>
#include "PGFKeyWord.h"
#include "XWPGFPool.h"
#include "XWPDFDriver.h"
#include "XWTeXBox.h"
#include "XWTikzCommand.h"
#include "XWTikzCoord.h"
#include "XWTikzOptions.h"
#include "XWTikzState.h"
#include "XWTikzCommandDialog.h"
#include "XWTikzUndoCommand.h"
#include "XWTikzGraphic.h"


XWTikzGraphic::XWTikzGraphic(QObject * parent)
:QObject(parent),
 keyWord(-1),
 cur(-1),
 undoStack(0),
 options(0)
{
  minX = 0;
  minY = 0;
  maxX = 720;
  maxY = 720;
  reset();
}

XWTikzGraphic::~XWTikzGraphic()
{}

bool XWTikzGraphic::addMenuAction(QMenu & menu)
{
  if (cur < 0 || cur >= cmds.size())
    return false;

  bool ret = true;
  switch (curScope)
  {
    case XW_TIKZ_S_OPERATION:
      ret = cmds[cur]->addOperationAction(menu);
      break;

    case XW_TIKZ_S_PATH:
      cmds[cur]->addPathAction(menu);
      break;

    case XW_TIKZ_S_SCOPE:
      if (cmds[cur]->getKeyWord() == PGFscope || 
          cmds[cur]->getKeyWord() == XW_TIKZ_GROUP)
      {
        XWTikzScope * s = (XWTikzScope*)(cmds[cur]);
        s->addScopeAction(menu);
      }
      break;

    default:
      {
         QMenu * submenu = menu.addMenu(tr("draw"));
         options->addLineAction(*submenu);
         options->addArrowsAction(*submenu);
         options->addRoundedCornersAction(*submenu);
         options->addDoubleDistanceAction(*submenu);
         menu.addSeparator();
         submenu = menu.addMenu(tr("decoration"));
         menu.addSeparator();
         submenu = menu.addMenu(tr("color"));
         options->addColorAction(*submenu);
         options->addOpacityAction(*submenu);
         options->addDoubleAction(*submenu);
         menu.addSeparator();
         submenu = menu.addMenu(tr("pattern"));
         options->addPatternAction(*submenu);
         menu.addSeparator();
         submenu = menu.addMenu(tr("shade"));
         options->addShadeAction(*submenu);
         menu.addSeparator();
         submenu = menu.addMenu(tr("fading"));
         options->addPathFading(*submenu);
         menu.addSeparator();
         options->addDecorationAction(*submenu);
         submenu = menu.addMenu(tr("node"));
         options->addShapeAction(*submenu);
         options->addAnchorAction(*submenu);    
         menu.addSeparator();     
         submenu = menu.addMenu(tr("spy"));
         options->addSpyAction(*submenu);
         menu.addSeparator();
         submenu = menu.addMenu(tr("mindmap"));
         options->addMinmapAction(*submenu);
         menu.addSeparator();
         options->addDomainAction(menu);
         menu.addSeparator();
         submenu = menu.addMenu(tr("transform"));
         options->addXYZAction(*submenu);
         options->addScaleAction(*submenu);
         options->addShiftAction(*submenu);
         options->addRotateAction(*submenu);
         options->addSlantAction(*submenu);         
      }      
      break;
  }

  return ret;
}

void XWTikzGraphic::back()
{
  if (cur < 0 || cur >= cmds.size())
    return ;

  XWTikzState * state = new XWTikzState(this,0,false);
  switch (curScope)
  {
    case XW_TIKZ_S_OPERATION:
    case XW_TIKZ_S_SCOPE:
    case XW_TIKZ_S_PATH:
      cmds[cur]->back(state);
      break;

    default:
      if (cur > 0)
      {
        XWTikzRemovePath * cmd = new XWTikzRemovePath(this,cur - 1);          
        push(cmd);
      }
      break;
  }

  delete state;
}

void XWTikzGraphic::cut()
{
  if (cur < 0 || cur >= cmds.size())
    return ;

  XWTikzState * state = new XWTikzState(this,0,false);
  cmds[cur]->cut(state);
  delete state;
}

void XWTikzGraphic::del()
{
  if (cur < 0 || cur >= cmds.size())
    return ;

  XWTikzState * state = new XWTikzState(this,0,false);
  switch (curScope)
  {
    case XW_TIKZ_S_OPERATION:
    case XW_TIKZ_S_SCOPE:
    case XW_TIKZ_S_PATH:
      cmds[cur]->del(state);
      break;

    default: 
      {
        XWTikzRemovePath * cmd = new XWTikzRemovePath(this,cur);
        push(cmd);
      }
      break;
  }

  delete state;
}

void XWTikzGraphic::doChildAnchor(XWTikzState * state)
{
  if (cur < 0 || cur >= cmds.size())
    return ;
  options->doChildAnchor(state);
  if (cmds[cur]->getKeyWord() == PGFscope || 
      cmds[cur]->getKeyWord() == XW_TIKZ_GROUP)
  {
    cmds[cur]->doChildAnchor(state);
  }
}

void XWTikzGraphic::doCopy(XWTikzState * state)
{
  if (cur < 0 || cur >= cmds.size())
    return ;

  options->doPath(state);
  if (cmds[cur]->getKeyWord() == PGFscope || 
      cmds[cur]->getKeyWord() == XW_TIKZ_GROUP)
  {
    cmds[cur]->doCopy(state);
  }
  else
  {
    int tcur = cur;
    QList<int> spies;
    for (int i = 0; i < cmds.size(); i++)
    {
      cur = i;
      if (cmds[i]->getKeyWord() == PGFscope || 
        cmds[i]->getKeyWord() == XW_TIKZ_GROUP)    
      {
        XWTikzScope * s = (XWTikzScope*)(cmds[i]);
        s->doScope(state);
      }
      else if (cmds[i]->getKeyWord() == PGFforeach)
      {
        XWTikzForeach * s = (XWTikzForeach*)(cmds[i]);
        s->doScope(state);
      }
      else if (cmds[i]->getKeyWord() == PGFspy)
        spies << i;
      else
        cmds[i]->doPath(state,false);
    }
    cur = tcur;
  }
}

void XWTikzGraphic::doDecoration(XWTikzState * state)
{
  options->doDecoration(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doDecoration(state);
}

void XWTikzGraphic::doEdgeFromParent(XWTikzState * state)
{
  options->doEdgeFromParent(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEdgeFromParent(state);
}

void XWTikzGraphic::doEdgeFromParentPath(XWTikzState * state)
{
  options->doEdgeFromParentPath(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEdgeFromParentPath(state);
}

void XWTikzGraphic::doEveryChild(XWTikzState * state)
{
  options->doEveryChild(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryChild(state);
}

void XWTikzGraphic::doEveryChildNode(XWTikzState * state)
{
  options->doEveryChildNode(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryChildNode(state);
}

void XWTikzGraphic::doEveryConcept(XWTikzState * state)
{
  options->doEveryConcept(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryConcept(state);
}

void XWTikzGraphic::doEveryEdge(XWTikzState * state)
{
  options->doEveryEdge(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryEdge(state);
}

void XWTikzGraphic::doEveryLabel(XWTikzState * state)
{
  options->doEveryLabel(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryLabel(state);
}

void XWTikzGraphic::doEveryMark(XWTikzState * state)
{
  options->doEveryMark(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryMark(state);
}

void XWTikzGraphic::doEveryMatrix(XWTikzState * state)
{
  options->doEveryMatrix(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryMatrix(state);
}

void XWTikzGraphic::doEveryMindmap(XWTikzState * state)
{
  options->doEveryMindmap(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryMindmap(state);
}

void XWTikzGraphic::doEveryNode(XWTikzState * state)
{
  options->doEveryNode(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryNode(state);
}

void XWTikzGraphic::doEveryPin(XWTikzState * state)
{
  options->doEveryPin(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryPin(state);
}

void XWTikzGraphic::doEveryPinEdge(XWTikzState * state)
{
  options->doEveryPinEdge(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryPinEdge(state);
}

void XWTikzGraphic::doEveryShape(XWTikzState * state)
{
  options->doEveryShape(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryShape(state);
}

void XWTikzGraphic::doGraphic(XWPDFDriver * driver)
{
  if (cmds.size() <= 0)
    return ;

  XWTikzState * state = new XWTikzState(this,driver);
  options->doPath(state);
  int tcur = cur;
  QList<int> spies;
  for (int i = 0; i < cmds.size(); i++)
  {
    cur = i;
    if (cmds[i]->getKeyWord() == PGFscope || 
        cmds[i]->getKeyWord() == XW_TIKZ_GROUP)    
    {
      XWTikzScope * s = (XWTikzScope*)(cmds[i]);
      s->doScope(state);
    }
    else if (cmds[i]->getKeyWord() == PGFforeach)
    {
      XWTikzForeach * s = (XWTikzForeach*)(cmds[i]);
      s->doScope(state);
    }
    else if (cmds[i]->getKeyWord() == PGFforeach)
      spies << i;
    else
      cmds[i]->doPath(state,false);
  }
  cur = tcur;

  for (int i = 0; i < spies.size(); i++)
  {
    state = state->save();
    state->setOnNode(true);
    cmds[i]->doPath(state,false);
    state->setOnNode(false);
    cmds[i]->doPath(state,false);
    state = state->restore();
  }

  delete state;
}

void XWTikzGraphic::doLevel(XWTikzState * state)
{
  options->doLevel(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doLevel(state);
}

void XWTikzGraphic::doLevelConcept(XWTikzState * state)
{
  options->doLevelConcept(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doLevelConcept(state);
}

void XWTikzGraphic::doLevelNumber(XWTikzState * state)
{
  options->doLevelNumber(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doLevelNumber(state);
}

void XWTikzGraphic::doOperation(XWPDFDriver * driver)
{
  if (cur < 0 || cur >= cmds.size())
    return ;

  XWTikzState * state = new XWTikzState(this,driver);
  options->doPath(state);
  cmds[cur]->doOperation(state,true);
  delete state;
}

void XWTikzGraphic::doParentAnchor(XWTikzState * state)
{
  if (cur < 0 || cur >= cmds.size())
    return ;
  options->doParentAnchor(state);
  if (cmds[cur]->getKeyWord() == PGFscope || 
      cmds[cur]->getKeyWord() == XW_TIKZ_GROUP)
  {
    cmds[cur]->doParentAnchor(state);
  }
}

void XWTikzGraphic::doPath(XWPDFDriver * driver)
{
  if (cur < 0 || cur >= cmds.size())
    return ;

  XWTikzState * state = new XWTikzState(this,driver);
  
  options->doPath(state,true);
  if (cmds[cur]->getKeyWord() == PGFspy)
  {
    state = state->save();
    state->setOnNode(true);
    cmds[cur]->doPath(state,false);
    state->setOnNode(false);
    cmds[cur]->doPath(state,true);
    state = state->restore();
  }
  else
    cmds[cur]->doPath(state,true);
  delete state;
}

void XWTikzGraphic::doRootConcept(XWTikzState * state)
{
  options->doRootConcept(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doRootConcept(state);
}

void XWTikzGraphic::doScope(XWPDFDriver * driver)
{
  if (cur < 0 || cur >= cmds.size())
    return ;

  if (cmds[cur]->getKeyWord() != PGFscope && 
     cmds[cur]->getKeyWord() != XW_TIKZ_GROUP &&
     cmds[cur]->getKeyWord() != PGFforeach)
     return ;

  
  XWTikzState * state = new XWTikzState(this,driver);
  options->doPath(state,false);
  if (cmds[cur]->getKeyWord() != PGFforeach)
  {
    XWTikzScope * s = (XWTikzScope*)(cmds[cur]);
    s->doScope(state);
  }
  else
  {
    XWTikzForeach * s = (XWTikzForeach*)(cmds[cur]);
    s->doScope(state);
  }
  
  delete state;
}

void XWTikzGraphic::doSpyConnection(XWTikzState * state)
{
  options->doSpyConnection(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doSpyConnection(state);
}

void XWTikzGraphic::doSpyNode(XWTikzState * state)
{
  options->doSpyNode(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doSpyNode(state);
}

void XWTikzGraphic::doToPath(XWTikzState * state)
{
  options->doToPath(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doToPath(state);
}

void XWTikzGraphic::dragTo(XWPDFDriver * driver,const QPointF & p)
{
  if (cur < 0 || cur >= cmds.size())
    return ;

  XWTikzState * state = new XWTikzState(this,driver);
  state->setMousePoint(lastPoint);
  state->setLastMousePoint(p);
  options->dragTo(state);
  cmds[cur]->dragTo(state);
  delete state;
}

void XWTikzGraphic::dropTo(const QPointF & p)
{
  if (cur < 0 || cur >= cmds.size())
    return ;

  XWTikzState * state = new XWTikzState(this,0,false);
  state->setMousePoint(lastPoint);
  state->setLastMousePoint(p);
  if (!options->dropTo(state))
  {
    switch (curScope)
    {
      case XW_TIKZ_S_GRAPHIC:
        for (int i = 0; i < cmds.size(); i++)
        {
          if (cmds[i]->dropTo(state))
            break;
        }
        break;

      default:
        if (cur >= 0 && cur < cmds.size())
          cmds[cur]->dropTo(state);
        break;
    }
  }

  delete state;
}

int XWTikzGraphic::getAnchorPosition()
{
  if (cur < 0 || cur >= cmds.size())
    return 0;

  return cmds[cur]->getAnchorPosition();
}

XWTikzOperation * XWTikzGraphic::getCurrentOperation()
{
  if (cur < 0 || cur >= cmds.size())
    return 0;

  return cmds[cur]->getCurrentOperation();
}

XWTikzCoord * XWTikzGraphic::getCurrentPoint()
{
  if (cur < 0 || cur >= cmds.size())
    return 0;

  return cmds[cur]->getCurrentPoint();
}

int XWTikzGraphic::getCursorPosition()
{
  if (cur < 0 || cur >= cmds.size())
    return 0;

  return cmds[cur]->getCursorPosition();
}

QString XWTikzGraphic::getCurrentText()
{
  if (cur < 0 || cur >= cmds.size())
    return QString();

  return cmds[cur]->getCurrentText();
}

double XWTikzGraphic::getHeight()
{
  return maxY - minY;
}

QPointF XWTikzGraphic::getIntersection(int s)
{
  QPointF ret;
  if (intersections.isEmpty())
  {
    if (namedPath1 >= 0 && namedPath2 >= 0)
    {
      XWTikzState * state = new XWTikzState(this,0,false);
      if (sortBy != namedPath1 && sortBy != namedPath2)
        sortBy = namedPath1;

      QList<int> operationsA;
      QList<QPointF> pointsA;
      if (sortBy == namedPath1)
      {
        cmds[namedPath2]->getPath(operationsA,pointsA,state);
        intersections = cmds[namedPath1]->getIntersections(operationsA,pointsA,state);
      }
      else
      {
        cmds[namedPath1]->getPath(operationsA,pointsA,state);
        intersections = cmds[namedPath2]->getIntersections(operationsA,pointsA,state);
      }

      delete state;
    }
  }

  if (s < intersections.size())
    ret = intersections[s - 1];

  return ret;
}

void XWTikzGraphic::getIntersections(const QString & nameA,const QString & nameB)
{
  if (names.contains(nameA))
  {
    namedPath1 = names[nameA];
    if (cmds[namedPath1]->getKeyWord() == PGFscope || 
      cmds[namedPath1]->getKeyWord() == XW_TIKZ_GROUP)
    {
      XWTikzScope * s = (XWTikzScope*)(cmds[namedPath1]);
      s->getIntersections(nameA,nameB);
    }
  }
  else
    namedPath1 = -1;

  if (names.contains(nameB))
  {
    namedPath2 = names[nameB];
    if (cmds[namedPath2]->getKeyWord() == PGFscope || 
      cmds[namedPath2]->getKeyWord() == XW_TIKZ_GROUP)
    {
      XWTikzScope * s = (XWTikzScope*)(cmds[namedPath2]);
      s->getIntersections(nameA,nameB);
    }
  }
  else
    namedPath2 = -1;

  intersections.clear();
}

QString XWTikzGraphic::getLocalPath()
{
  QString path = QCoreApplication::applicationDirPath();
	int idx = path.lastIndexOf(QChar('/'));
	if (idx > 0)
		path = path.left(idx + 1);

	if (path[path.length() - 1] != QChar('/'))
		path += QChar('/');

	path += "picture";
	QDir d(path);
	if (!d.exists())
		d.mkdir(path);

  return path;
}

QPointF XWTikzGraphic::getNodeAnchor(const QString & nameA,int a,XWTikzState * stateA)
{
  QPointF ret;
  QString n = nameA;
  XWTikzState * state = new XWTikzState(this,0,false);
  options->doPath(state,false);
  if (nameA.contains("."))
  {
    int index = nameA.indexOf(".");
    n = nameA.left(index);
  }

  if (names.contains(n))
  {
    int i = names[n];
    ret = cmds[i]->getAnchor(nameA,a,stateA,state);
  }

  delete state;
  return ret;
}

QPointF XWTikzGraphic::getNodeAngle(const QString & nameA,double a,XWTikzState * stateA)
{
  XWTikzState * state = new XWTikzState(this,0,false);
  options->doPath(state,false);
  QString n = nameA;
  if (nameA.contains("."))
  {
    int index = nameA.indexOf(".");
    n = nameA.left(index);
  }

  QPointF ret;
  if (names.contains(n))
  {
    int i = names[n];
    ret = cmds[i]->getAngle(nameA,a,stateA,state);
  }

  delete state;

  return ret;
}

QPointF XWTikzGraphic::getPoint(const QString & nameA,XWTikzState * stateA)
{
  if (options->isMe(nameA,stateA))
    return options->getPoint(stateA);
    
  QString n = nameA;
  if (nameA.contains("."))
  {
    int index = nameA.indexOf(".");
    n = nameA.left(index);
  }

  QPointF ret;
  if (names.contains(n))
  {
    int i = names[n];
    XWTikzState * state = new XWTikzState(this,0,false);
    options->doPath(state,false);
    ret = cmds[i]->getPoint(nameA,stateA,state);
    delete state;
  }
  else if (by.contains(n))
  {
    int i = by.indexOf(n);
    ret = getIntersection(i);
  }
  else
  {
    int i = n.length() - 1;
    if (n[i].isDigit())
    {
      while (n[i].isDigit())
        i--;

      if (n[i] == QChar('-'))
      {
        i++;
        n = n.mid(i,-1);
        i = n.toInt();
        ret = getIntersection(i);
      }
    }
  }
  
  return ret;
}

QVector3D XWTikzGraphic::getPoint3D(const QString & nameA,XWTikzState * stateA)
{
  QString n = nameA;
  if (nameA.contains("."))
  {
    int index = nameA.indexOf(".");
    n = nameA.left(index);
  }

  QVector3D ret;
  if (names.contains(n))
  {
    int i = names[n];
    XWTikzState * state = new XWTikzState(this,0,false);
    options->doPath(state,false);
    ret = cmds[i]->getPoint3D(nameA,stateA,state);
    delete state;
  }
  else if (by.contains(n))
  {
    int i = by.indexOf(n);
    QPointF p = getIntersection(i);
    ret.setX(p.x());
    ret.setY(p.y());
  }
  else
  {
    int i = n.length() - 1;
    if (n[i].isDigit())
    {
      while (n[i].isDigit())
        i--;

      if (n[i] == QChar('-'))
      {
        i++;
        n = n.mid(i,-1);
        i = n.toInt();
        QPointF p = getIntersection(i);
        ret.setX(p.x());
        ret.setY(p.y());
      }
    }
  }

  return ret;
}

QString XWTikzGraphic::getSelectedText()
{
  if (cur < 0 || cur >= cmds.size())
    return QString();

  return cmds[cur]->getSelectedText();
}

QString XWTikzGraphic::getText()
{
  QString ret;
  if (cmds.size() > 0)
  {
    QString ops = options->getText();
    switch (keyWord)
    {
      case PGFtikz:
        if (cmds.size() == 1)
        {
          QString body = cmds[0]->getText();
          if (ops.isEmpty())
            ret = QString("\\tikz %1").arg(body);
          else
            ret = QString("\\tikz%1 %2").arg(ops).arg(body);
        }
        else
        {
          QString body;
          for (int i = 0; i < cmds.size(); i++)
          {
            QString tmp = cmds[i]->getText();
            body += tmp;
          }

          if (ops.isEmpty())
            ret = QString("\\tikz{%1}").arg(body);
          else
            ret = QString("\\tikz%1{%2}").arg(ops).arg(body);
        }
        break;

      case PGFtikzpicture:
        {
          QString body;
          for (int i = 0; i < cmds.size(); i++)
          {
            QString tmp = cmds[i]->getText();
            body += tmp;
            body += "\n";
          }

          if (ops.isEmpty())
            ret = QString("\\begin{tikzpicture}\n%1\\end{tikzpicture}\n").arg(body);
          else
            ret = QString("\\begin{tikzpicture}%1\n%2\\end{tikzpicture}\n").arg(ops).arg(body);
        }
        break;

      case PGFtikzfadingfrompicture:
        {
          QString body;
          for (int i = 0; i < cmds.size(); i++)
          {
            QString tmp = cmds[i]->getText();
            body += tmp;
            body += "\n";
          }

          if (ops.isEmpty())
            ret = QString("\\begin{tikzfadingfrompicture}\n%1\\end{tikzfadingfrompicture}\n").arg(body);
          else
            ret = QString("\\begin{tikzfadingfrompicture}%1\n%2\\end{tikzfadingfrompicture}\n").arg(ops).arg(body);
        }
        break;

      default:
        if (cmds.size() == 1)
        {
          ret = ops;
          QString tmp = cmds[0]->getText();
          ret += tmp;
        }
        else
        {
          ret = ops;
          for (int i = 0; i < cmds.size(); i++)
          {
            QString tmp = cmds[i]->getText();
            ret += tmp;
            ret += "\n";
          }
        }
        break;
    }
  }

  return ret;
}

QString XWTikzGraphic::getTips(const QPointF & p)
{
  QString ret;
  XWTikzState * state = new XWTikzState(this,0,false);
  state->setMousePoint(p);
  ret = options->getTips(state);
  switch (curScope)
  {
    case XW_TIKZ_S_GRAPHIC:      
      break;

    default:
      if (cur >= 0 && cur < cmds.size())
        ret = cmds[cur]->getTips(state);
      break;
  }

  delete state;

  return ret;
}

double XWTikzGraphic::getWidth()
{
  return maxX - minX;
}

void XWTikzGraphic::goToEnd()
{
  if (cmds.size() <= 0 || cur == cmds.size() - 1)
    return ;

  cur = cmds.size() - 1;
  cmds[cur]->goToEnd();
  emit viewChanged();
}

void XWTikzGraphic::goToNext()
{
  switch (curScope)
  {
    case XW_TIKZ_S_GRAPHIC:
      if (cur >= cmds.size() - 1)
        return ;
      cur++;
      cmds[cur]->goToStart();
      break;

    case XW_TIKZ_S_OPERATION:
      if (cur < 0 || cur >= cmds.size())
        return ;
      cmds[cur]->goToNextPoint();
      break;

    default:
      if (cur < 0 || cur >= cmds.size())
        return ;
      cmds[cur]->goToNext();
      break;
  }

  emit viewChanged();
}

void XWTikzGraphic::goToNextOperation()
{
  if (cur < 0 || cur >= cmds.size())
    return ;

  switch (cmds[cur]->getKeyWord())
  {
    case PGFscope:
    case XW_TIKZ_GROUP:
      {
        XWTikzScope * sc = (XWTikzScope*)(cmds[cur]);
        if (!sc->goToNextOperation())
        {
          if (cur >= cmds.size() - 1)
            return ;
          cur++;
          cmds[cur]->goToStart();
        }

        emit viewChanged();
      }
      break;

    case PGFpath:
    case PGFdraw:
    case PGFfill:
    case PGFfilldraw:
    case PGFpattern:
    case PGFshade:
    case PGFshadedraw:
    case PGFclip:
      {
        XWTikzPath * p = (XWTikzPath*)(cmds[cur]);
        if (!p->goToNext())
        {
          if (cur >= cmds.size() - 1)
            return ;
          cur++;
          cmds[cur]->goToStart();
        }

        emit viewChanged();
      }
      break;

    case PGFforeach:
      {
        XWTikzForeach * sc = (XWTikzForeach*)(cmds[cur]);
        if (!sc->goToNextOperation())
        {
          if (cur >= cmds.size() - 1)
            return ;
          cur++;
          cmds[cur]->goToStart();
        }

        emit viewChanged();
      }
      break;

    default:
      break;
  }
}

void XWTikzGraphic::goToNextPath()
{
  if (cur < 0 || cur >= cmds.size())
    return ;

  switch (cmds[cur]->getKeyWord())
  {
    case PGFscope:
    case XW_TIKZ_GROUP:
      {
        XWTikzScope * sc = (XWTikzScope*)(cmds[cur]);
        if (!sc->goToNextPath())
        {
          if (cur >= cmds.size() - 1)
            return ;
          cur++;
          cmds[cur]->goToStart();
        }
        emit viewChanged();
      }
      break;

    case PGFforeach:
      {
        XWTikzForeach * sc = (XWTikzForeach*)(cmds[cur]);
        if (!sc->goToNextPath())
        {
          if (cur >= cmds.size() - 1)
            return ;
          cur++;
          cmds[cur]->goToStart();
        }
        emit viewChanged();
      }
      break;

    default:
      if (cur >= cmds.size() - 1)
        return ;
      cur++;
      cmds[cur]->goToStart();
      emit viewChanged();
      break;
  }
}

void XWTikzGraphic::goToNextScope()
{
  int i = cur;
  while (i < (cmds.size() - 1))
  {
    i++;
    if (cmds[i]->getKeyWord() == PGFscope || cmds[i]->getKeyWord() == XW_TIKZ_GROUP)
    {
      XWTikzScope * sc = (XWTikzScope*)(cmds[i]);
      sc->goToStart();
      cur = i;
      emit viewChanged();
      return ;
    }
  }
}

void XWTikzGraphic::goToOperationEnd()
{
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->goToOperationEnd();
  emit viewChanged();
}

void XWTikzGraphic::goToOperationStart()
{
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->goToOperationStart();
  emit viewChanged();
}

void XWTikzGraphic::goToPathEnd()
{
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->goToPathEnd();
  emit viewChanged();
}

void XWTikzGraphic::goToPathStart()
{
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->goToPathStart();
  emit viewChanged();
}

void XWTikzGraphic::goToPrevious()
{
  if (cur < 0)
    return ;

  switch (curScope)
  {
    case XW_TIKZ_S_GRAPHIC:
      if (cur == 0)
        return ;
      cur--;
      cmds[cur]->goToEnd();
      break;

    case XW_TIKZ_S_OPERATION:
      if (cur >= cmds.size())
        return ;
      cmds[cur]->goToPreviousPoint();
      break;

    default:
      if (cur >= cmds.size())
        return ;
      cmds[cur]->goToPrevious();
      break;
  }

  emit viewChanged();
}

void XWTikzGraphic::goToPreviousOperation()
{
  if (cur < 0 || cur >= cmds.size())
    return ;

  switch (cmds[cur]->getKeyWord())
  {
    case PGFscope:
    case XW_TIKZ_GROUP:
      {
        XWTikzScope * sc = (XWTikzScope*)(cmds[cur]);
        if (!sc->goToPreviousOperation())
        {
          if (cur <= 0)
            return ;
          cur--;
          cmds[cur]->goToEnd();
        }
        emit viewChanged();
      }
      break;

    case PGFforeach:
      {
        XWTikzForeach * sc = (XWTikzForeach*)(cmds[cur]);
        if (!sc->goToPreviousOperation())
        {
          if (cur <= 0)
            return ;
          cur--;
          cmds[cur]->goToEnd();
        }
        emit viewChanged();
      }
      break;

    case PGFpath:
    case PGFdraw:
    case PGFfill:
    case PGFfilldraw:
    case PGFpattern:
    case PGFshade:
    case PGFshadedraw:
    case PGFclip:
      {
        XWTikzPath * p = (XWTikzPath*)(cmds[cur]);
        if (!p->goToPrevious())
        {
          if (cur <= 0)
            return ;
          cur--;
          cmds[cur]->goToEnd();
        }
        emit viewChanged();
      }
      break;

    default:
      break;
  }
}

void XWTikzGraphic::goToPreviousPath()
{
  if (cur < 0)
    return ;

  switch (cmds[cur]->getKeyWord())
  {
    case PGFscope:
    case XW_TIKZ_GROUP:
      {
        XWTikzScope * sc = (XWTikzScope*)(cmds[cur]);
        if (!sc->goToPreviousPath())
        {
          if (cur <= 0)
            return ;
          cur--;
          cmds[cur]->goToEnd();
        }
        emit viewChanged();
      }
      break;

    case PGFforeach:
      {
        XWTikzForeach * sc = (XWTikzForeach*)(cmds[cur]);
        if (!sc->goToPreviousPath())
        {
          if (cur <= 0)
            return ;
          cur--;
          cmds[cur]->goToEnd();
        }
        emit viewChanged();
      }
      break;

    default:
      if (cur == 0)
        return ;
      cur--;
      cmds[cur]->goToEnd();
      emit viewChanged();
      break;
  }
}

void XWTikzGraphic::goToPreviousScope()
{
  int i = cur;
  while (i > 0)
  {
    i--;
    if (cmds[i]->getKeyWord() == PGFscope || cmds[i]->getKeyWord() == XW_TIKZ_GROUP)
    {
      XWTikzScope * sc = (XWTikzScope*)(cmds[i]);
      sc->goToEnd();
      cur = i;
      emit viewChanged();
      return ;
    }
  }
}

void XWTikzGraphic::goToStart()
{
  if (cmds.size() == 0 || cur == 0)
    return ;

  cur = 0;
  cmds[cur]->goToStart();
  emit viewChanged();
}

void XWTikzGraphic::hitTest(const QPointF & p)
{
  lastPoint = p;
  XWTikzState * state = new XWTikzState(this,0,false);
  state->setMousePoint(p);
  state->setLastMousePoint(p);
  bool b = false;
  b = options->hitTest(state);
  if (!b)
  {
    switch (curScope)
    {
      case XW_TIKZ_S_GRAPHIC:
        cur = -1;
        for (int i = 0; i < cmds.size(); i++)
        {
          b = cmds[i]->hitTest(state);
          if (b)
          {
            cur = i;
            break;
          }
        }
        break;

      default:
        if (cur >= 0 && cur < cmds.size())
          b = cmds[cur]->hitTest(state);
        break;
    }
  } 

  delete state;

  if (b)
    emit viewChanged();
}

void XWTikzGraphic::insert(int i, XWTikzCommand * cmd)
{
  cur = i;
  if (i >= cmds.size())
    cmds << cmd;
  else
    cmds.insert(i,cmd);
}

void XWTikzGraphic::insertText(const QString & str)
{
  if (cur < 0 || cur >= cmds.size())
    return ;

  XWTikzState * state = new XWTikzState(this,0,false);
  state->setText(str);
  if (!options->insertText(state))
    cmds[cur]->insertText(state);

  delete state;
}

void XWTikzGraphic::keyInput(const QString & str)
{
  if (cur < 0 || cur >= cmds.size())
    return ;

  XWTikzState * state = new XWTikzState(this,0,false);
  state->setText(str);
  if (!options->keyInput(state))
    cmds[cur]->keyInput(state);

  delete state;
}

void XWTikzGraphic::moveDown()
{
  QPointF p(lastPoint.x(),lastPoint.y() - gridStep);
  dropTo(p);
}

void XWTikzGraphic::moveDownBig()
{
  QPointF p(lastPoint.x(),lastPoint.y() - 2 * gridStep);
  dropTo(p);
}

void XWTikzGraphic::moveDownSmall()
{
  QPointF p(lastPoint.x(),lastPoint.y() - 0.5 * gridStep);
  dropTo(p);
}

void XWTikzGraphic::moveLeft()
{
  QPointF p(lastPoint.x() - gridStep,lastPoint.y());
  dropTo(p);
}

void XWTikzGraphic::moveLeftBig()
{
  QPointF p(lastPoint.x() - 2 * gridStep,lastPoint.y());
  dropTo(p);
}

void XWTikzGraphic::moveLeftHuge()
{
  QPointF p(lastPoint.x() - 3 * gridStep,lastPoint.y());
  dropTo(p);
}

void XWTikzGraphic::moveLeftSmall()
{
  QPointF p(lastPoint.x() - 0.5 * gridStep,lastPoint.y());
  dropTo(p);
}

void XWTikzGraphic::moveRight()
{
  QPointF p(lastPoint.x() + gridStep,lastPoint.y());
  dropTo(p);
}

void XWTikzGraphic::moveRightBig()
{
  QPointF p(lastPoint.x() + 2 * gridStep,lastPoint.y());
  dropTo(p);
}

void XWTikzGraphic::moveRightHuge()
{
  QPointF p(lastPoint.x() + 3 * gridStep,lastPoint.y());
  dropTo(p);
}

void XWTikzGraphic::moveRightSmall()
{
  QPointF p(lastPoint.x() + 0.5 * gridStep,lastPoint.y());
  dropTo(p);
}

void XWTikzGraphic::moveUp()
{
  QPointF p(lastPoint.x(),lastPoint.y() + gridStep);
  dropTo(p);
}

void XWTikzGraphic::moveUpBig()
{
  QPointF p(lastPoint.x(),lastPoint.y() + 2 * gridStep);
  dropTo(p);
}

void XWTikzGraphic::moveUpSmall()
{
  QPointF p(lastPoint.x(),lastPoint.y() + 0.5 * gridStep);
  dropTo(p);
}

void XWTikzGraphic::newPar()
{
  if (cur < 0 || cur >= cmds.size())
    return ;

  XWTikzState * state = new XWTikzState(this,0,false);
  if (!options->newPar(state))
    cmds[cur]->newPar(state);
  delete state;
}

void XWTikzGraphic::paste(const QString & str)
{
  if (cur < 0 || cur >= cmds.size())
    return ;

  XWTikzState * state = new XWTikzState(this,0,false);
  state->setText(str);
  if (!options->paste(state))
    cmds[cur]->paste(state);

  delete state;
}

void XWTikzGraphic::push(QUndoCommand * c)
{
  undoStack->push(c);
  emit changed();
}

void XWTikzGraphic::registNamed(const QString & n)
{
  QString nameA = n;
  if (n.contains("."))
  {
    int index = n.indexOf(".");
    nameA = n.left(index);
  }

  XWTikzState state(this,0,false);
  options->doPath(&state,false);
  QString prefix = state.getNamePrefix();
  if (!prefix.isEmpty())
  {
    prefix += "-";
    nameA.insert(0,prefix);
  }

  QString suffix = state.getNameSuffix();
  if (!suffix.isEmpty())
  {
    suffix.insert(0,"-");
    nameA += suffix;
  }
  
  names[nameA] = cur;
  if (cmds[cur]->getKeyWord() == PGFscope || 
      cmds[cur]->getKeyWord() == XW_TIKZ_GROUP)
  {
    XWTikzScope * s = (XWTikzScope*)(cmds[cur]);
    s->registNamed(n);
  }
}

void XWTikzGraphic::scan(const QString & str)
{
  int len = str.length();
  int pos = 0;

  cur = -1;
  XWTikzCommand * obj = 0;
  while (pos < len)
  {
    if (str[pos] == QChar('\\'))
    {
      QString key = XWTeXBox::scanControlSequence(str,len,pos);
      int id = lookupPGFID(key);
      if (id == PGFbegin)
      {
        key = XWTeXBox::scanEnviromentName(str,len,pos);
        id = lookupPGFID(key);
        if (id == PGFtikzpicture || id == PGFtikzfadingfrompicture)
        {
          keyWord = id;
          options->scan(str,len,pos);
        }
        else
        {
          obj = createPGFObject(this,id,this);
          cmds << obj;
          cur++;
          obj->scan(str,len,pos);
        }        
      }
      else if (id == PGFend)
      {
        key = XWTeXBox::scanEnviromentName(str,len,pos);
        id = lookupPGFID(key);
        if (id == PGFtikzpicture || id == PGFtikzfadingfrompicture)
          break;
      }
      else if (id == PGFtikz)
      {
        keyWord = id;
        options->scan(str,len,pos);
        while (str[pos].isSpace())
          pos++;

        if (str[pos] == QChar('{'))
          pos++;
      }
      else
      {
        obj = createPGFObject(this,id,this);
        cmds << obj;
        cur++;
        obj->scan(str,len,pos);        
      }
    }
    else  if (str[pos] == QChar('{'))
    {
      obj = new XWTikzScope(this,XW_TIKZ_GROUP,this);
      cmds << obj;
      cur++;
      obj->scan(str,len,pos);
    }
    else if (str[pos] == QChar('}'))
    {
      pos++;
      break;
    }
    else if (str[pos] == QChar('%'))
      XWTeXBox::skipComment(str,len,pos);
    else
      pos++;
  }

  emit changed();
}

void XWTikzGraphic::setBy(const QStringList & b)
{
  by = b;
}

void XWTikzGraphic::setSize(double minxA, double minyA,double maxxA,double maxyA)
{
  minX = minxA;
  maxX = maxxA;
  minY = minyA;
  maxY = maxyA;
  emit changed();
}

void XWTikzGraphic::setSortBy(const QString & nameA)
{
  if (names.contains(nameA))
  {
    sortBy = names[nameA];
    if (cmds[sortBy]->getKeyWord() == PGFscope || 
      cmds[sortBy]->getKeyWord() == XW_TIKZ_GROUP)
    {
      XWTikzScope * s = (XWTikzScope*)(cmds[sortBy]);
      s->setSortBy(nameA);
    }
  }
  else
    sortBy = -1;

  intersections.clear();
}

XWTikzCommand * XWTikzGraphic::takeAt(int i)
{
  cur = i - 1;
  return cmds.takeAt(i);
}

void XWTikzGraphic::updateCursor(double minxA,double minyA,double maxxA,double maxyA)
{
  emit cursorChanged(minxA,minyA,maxxA,maxyA);
}

void XWTikzGraphic::addArc()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addArc();
}

void XWTikzGraphic::addCircle()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addCircle();
}

void XWTikzGraphic::addCircleSolidus()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addSplit("circle solidus");
}

void XWTikzGraphic::addCircleSplit()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addSplit("circle split");
}

void XWTikzGraphic::addCoordinateCommand()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)) && 
      ((cmds[cur]->getKeyWord() == PGFscope) ||
       (cmds[cur]->getKeyWord() == XW_TIKZ_GROUP)))
  {
    XWTikzScope * s = (XWTikzScope*)(cmds[cur]);
    s->addCoordinateCommand();
    return ;
  }

  XWTikzCoordinateCommandDialog dlg;
  if (dlg.exec() == QDialog::Accepted)
  {
    XWTikzCoordinatePath * node = new XWTikzCoordinatePath(this,this);
    QString tmp = dlg.getName();
    node->setName(tmp);
    tmp = dlg.getCoord();
    node->setAt(tmp);
    XWTikzAddPath * cmd = new XWTikzAddPath(this,cur + 1,node);
    push(cmd);
  }
}

void XWTikzGraphic::addCoordinate()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addCoordinate();
}

void XWTikzGraphic::addCosine()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addCosine();
}

void XWTikzGraphic::addCurveTo()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addCurveTo();
}

void XWTikzGraphic::addCycle()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addCycle();
}

void XWTikzGraphic::addEdge()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addEdge();
}

void XWTikzGraphic::addEllipse()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addEllipse();
}

void XWTikzGraphic::addEllipseSplit()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addSplit("ellipse split");
}

void XWTikzGraphic::addGrid()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addGrid();
}

void XWTikzGraphic::addHVLines()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addHVLines();
}

void XWTikzGraphic::addLineTo()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addLineTo();
}

void XWTikzGraphic::addMoveTo()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addMoveTo();
}

void XWTikzGraphic::addNode()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addNode();
}

void XWTikzGraphic::addNodeCommand()
{
  if ((cur >= 0) && (cur < (cmds.size() - 1)) && 
      ((cmds[cur]->getKeyWord() == PGFscope) ||
       (cmds[cur]->getKeyWord() == XW_TIKZ_GROUP)))
  {
    XWTikzScope * s = (XWTikzScope*)(cmds[cur]);
    s->addNodeCommand();
    return ;
  }

  XWTikzNodeCommandDialog dlg;
  if (dlg.exec() == QDialog::Accepted)
  {
    XWTikzNodePath * node = new XWTikzNodePath(this,this);
    QString tmp = dlg.getName();
    node->setName(tmp);
    tmp = dlg.getCoord();
    node->setAt(tmp);
    tmp = dlg.getText();
    node->setText(tmp);
    XWTikzAddPath * cmd = new XWTikzAddPath(this,cur + 1,node);
    push(cmd);
  }
}

void XWTikzGraphic::addParabola()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addParabola();
}

void XWTikzGraphic::addPath(int keywordA)
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)) && 
      ((cmds[cur]->getKeyWord() == PGFscope) ||
       (cmds[cur]->getKeyWord() == XW_TIKZ_GROUP)))
  {
    XWTikzScope * s = (XWTikzScope*)(cmds[cur]);
    s->addPath(keywordA);
    return ;
  }

  XWTikzPath * path = new XWTikzPath(this,keywordA,this);
  XWTikzAddPath * cmd = new XWTikzAddPath(this,cur + 1,path);
  push(cmd);
}

void XWTikzGraphic::addPlotCoordinates()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addPlotCoordinates();
}

void XWTikzGraphic::addPlotFile()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addPlotFile();
}

void XWTikzGraphic::addPlotFunction()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addPlotFunction();
}

void XWTikzGraphic::addRectangle()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addRectangle();
}

void XWTikzGraphic::addRectangleSplit()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addSplit("rectangle split");
}

void XWTikzGraphic::addScope()
{
  if (curScope == XW_TIKZ_S_SCOPE)
  {
    if ((cur >= 0) && (cur <= (cmds.size() - 1)) && 
      ((cmds[cur]->getKeyWord() == PGFscope) ||
       (cmds[cur]->getKeyWord() == XW_TIKZ_GROUP)))
    {
      XWTikzScope * s = (XWTikzScope*)(cmds[cur]);
      s->addScope();
    }
    else
    {
      XWTikzScope * s = new XWTikzScope(this,PGFscope,this);
      XWTikzAddPath * cmd = new XWTikzAddPath(this,cur + 1,s);
      push(cmd);
    }
  }
  else
  {
    XWTikzScope * s = new XWTikzScope(this,PGFscope,this);
    XWTikzAddPath * cmd = new XWTikzAddPath(this,cur + 1,s);
    push(cmd);
  }  
}

void XWTikzGraphic::addSine()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addSine();
}

void XWTikzGraphic::addSpy()
{
  if (curScope == XW_TIKZ_S_SCOPE)
  {
    if ((cur >= 0) && (cur <= (cmds.size() - 1)) && 
      ((cmds[cur]->getKeyWord() == PGFscope) ||
       (cmds[cur]->getKeyWord() == XW_TIKZ_GROUP)))
    {
      XWTikzScope * s = (XWTikzScope*)(cmds[cur]);
      s->addSpy();
    }
  }
  else
  {
    XWTikzTwoCoordDialog dlg(tr("spy"),tr("on"), tr("at"));
    if (dlg.exec() == QDialog::Accepted)
    {
      XWTikzSpy * spy = new XWTikzSpy(this,this);
      QString on = dlg.getCoord1();
      QString at = dlg.getCoord2();
      spy->setOn(on);
      spy->setAt(at);
      XWTikzAddPath * cmd = new XWTikzAddPath(this,cur + 1,spy);
      push(cmd);
    }
  }
}

void XWTikzGraphic::addVHLines()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addVHLines();
}

void XWTikzGraphic::newTikz()
{
  reset();
  keyWord = PGFtikz;
  emit changed();
}

void XWTikzGraphic::newTikzpicture()
{
  reset();
  keyWord = PGFtikzpicture;
  emit changed();
}

void XWTikzGraphic::redo()
{
  undoStack->redo();
  emit changed();
}

void XWTikzGraphic::undo()
{
  undoStack->undo();
  emit changed();
}

void XWTikzGraphic::reset()
{
  if (undoStack)
  {
    delete undoStack;
    undoStack = 0;
  }

  if (options)
  {
    delete options;
    options = 0;
  }

  while (!cmds.isEmpty())
    delete cmds.takeFirst();

  names.clear();

  curScope = XW_TIKZ_S_GRAPHIC;
  keyWord = -1;
  gridStep = 28.4528;

  cur = -1;

  namedPath1 = -1;
  namedPath2 = -1;
  sortBy = -1;
  undoStack = new QUndoStack(this);
  options = new XWTIKZOptions(this);
}
