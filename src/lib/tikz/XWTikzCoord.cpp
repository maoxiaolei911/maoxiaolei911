/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include "PGFKeyWord.h"
#include "XWTikzState.h"
#include "XWTikzExpress.h"
#include "XWTikzGraphic.h"
#include "XWTikzCommand.h"
#include "XWTikzOptions.h"
#include "XWTikzUndoCommand.h"
#include "XWTikzOperationDialog.h"
#include "XWTikzCoord.h"


XWTikzCoord::XWTikzCoord(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, XW_TIKZ_COORD,parent),
 cs(XW_TIKZ_CS_NULL),
 rel(0)
{
  options = new XWTIKZOptions(graphicA, this);
}

XWTikzCoord::XWTikzCoord(XWTikzGraphic * graphicA, const QString & str, QObject * parent)
:XWTikzOperation(graphicA, XW_TIKZ_COORD,parent),
 cs(XW_TIKZ_CS_NULL)
{
  options = new XWTIKZOptions(graphicA, this);
  int pos = 0;
  int len = str.length();
  scan(str,len,pos);
}

XWTikzCoord::~XWTikzCoord()
{
  if (cs == XW_TIKZ_CS_BARYCENTRIC)
  {
    if (csc.barycentric.vas)
      free(csc.barycentric.vas);
    csc.barycentric.vas = 0;
  }
}

bool XWTikzCoord::addAction(QMenu & menu, XWTikzState *)
{
  QAction * a = menu.addAction(tr("move to"));
  connect(a, SIGNAL(triggered()), this, SLOT(setCoord()));
  return true;
}

void XWTikzCoord::doCompute(XWTikzState * state)
{
  state->saveTransform();
  options->doCompute(state);
  state->moveTo(this);
  state->restoreTransform();
}

void XWTikzCoord::doPath(XWTikzState * state, bool showpoint)
{
  state->saveTransform();
  options->doPath(state, false);
  state->moveTo(this);
  if (showpoint)
    draw(state);
  state->restoreTransform();
}

void XWTikzCoord::dragTo(XWTikzState * state)
{
  if (!curPoint)
    return ;
  state->saveTransform();
  options->doCompute(state);
  QPointF mp = state->getLastMousePoint();
  state->moveTo(mp);
  state->restoreTransform();
}

void XWTikzCoord::draw(XWTikzState * state)
{
  state = state->save(false);
  QPointF c = getPoint(state);
  QPointF a(1.5,0);
  QPointF b(0,1.5);
  state->addEllipse(c,a,b);

  if (graphic->getCurrentPoint() == this)
    state->setFillColor(Qt::darkGray);
  else
    state->setFillColor(Qt::lightGray);

  state = state->restore();
}

bool XWTikzCoord::dropTo(XWTikzState * state)
{
  if (!curPoint)
    return false;

  state->saveTransform();
  options->doCompute(state);
  bool ret = false;
  QVector3D newp;
  QVector3D oldp = getRelPoint3D(state);

  QPointF mp = state->getLastMousePoint();
  QPointF pm = getPoint(state);
  
  switch (cs)
  {
    default:
      break;

    case XW_TIKZ_CS_CANVAS:  
      if (csc.compEx.c1 && !csc.compEx.c1->isVar())
      {
        newp.setX(oldp.x() + mp.x() - pm.x());
        ret = true;
      }        
      if (csc.compEx.c2 && !csc.compEx.c2->isVar())
      {
        newp.setY(oldp.y() + mp.y() - pm.y());
        ret = true;
      }
      break;

    case XW_TIKZ_CS_XY:
      if (csc.compEx.c1 && !csc.compEx.c1->isVar())
      {
        newp.setX(oldp.x() + (mp.x() - pm.x()) / state->getXVec());
        ret = true;
      }
      if (csc.compEx.c2 && !csc.compEx.c2->isVar())
      {
        newp.setY(oldp.y() + (mp.y() - pm.y()) / state->getYVec());
        ret = true;
      }     
      break;

    case XW_TIKZ_CS_XYZ:
      {
        double d = sqrt((mp.x()-pm.x())*(mp.x()-pm.x()) + (mp.y()-pm.y())*(mp.y()-pm.y()));
        if ((qAbs(mp.y()-pm.y()) / d) < 0.5)
        {
          if (csc.compEx.c1 && !csc.compEx.c1->isVar())
          {
            newp.setX(oldp.x() + (mp.x() - pm.x()) / state->getXVec());
            ret = true;
          }
        }
        else if ((qAbs(mp.x()-pm.x()) / d) < 0.5)
        {
          if (csc.compEx.c2 && !csc.compEx.c2->isVar())
          {
            newp.setY(oldp.y() + (mp.y() - pm.y()) / state->getYVec());
            ret = true;
          }
        }
        else
        {
          if (csc.compEx.c3 && !csc.compEx.c3->isVar())
          {
            if (mp.y() > pm.y())
            {
              newp.setZ(oldp.z() - d / state->getZVec());
              ret = true;
            }              
            else
            {
              newp.setZ(oldp.z() + d / state->getZVec());
              ret = true;
            }       
          }
        }
      }
      break;
  }

  if (ret)
  {
    XWTikzMoveTo * cmd = new XWTikzMoveTo(this,newp,oldp);
    graphic->push(cmd);
  }

  state->restoreTransform();

  return ret;
}

QPointF XWTikzCoord::getPoint(XWTikzState * state)
{
  state->saveTransform();
  options->doCompute(state);
  QVector3D p3 = getPoint3D(state);
  QPointF p;
  
  switch (cs)
  {
    default:
      p.setX(p3.x());
      p.setY(p3.y());
      break;

    case XW_TIKZ_CS_XYZ:
    case XW_TIKZ_CS_XYZ_CYLINDRICAL:
    case XW_TIKZ_CS_XYZ_SPHERICAL:
      p.setX(p3.x() + p3.z());
      p.setX(p3.y() + p3.z());
      break;
  }
  state->restoreTransform();
  return p;
}

QVector3D XWTikzCoord::getPoint3D(XWTikzState * state)
{
  QVector3D p = getRelPoint3D(state);
  switch (rel)
  {
    default:
      break;

    case PGFSPlus:
      {
        QVector3D ip;
        XWTikzCoord * iip = state->getInitialCoord();
        if (iip)
          ip = iip->getPoint3D(state);
        p += ip;
      }      
      break;

    case PGFDPlus:
      {
        QVector3D pp;
        XWTikzCoord * ppp = state->getPreviousCoord(this); 
        if (ppp)
          pp = ppp->getPoint3D(state);
        p += pp;
      }      
      break;
  }

  return p;
}

QVector3D XWTikzCoord::getRelPoint3D(XWTikzState * state)
{
  QVector3D p;
  switch (cs)
  {
    default:
      if (csc.compEx.c1)
        p.setX(csc.compEx.c1->getResult(state));
      if (csc.compEx.c2)
        p.setY(csc.compEx.c2->getResult(state));
      break;

    case XW_TIKZ_CS_XY:
      if (csc.compEx.c1)
        p.setX(state->getXVec() * csc.compEx.c1->getResult(state));
      if (csc.compEx.c2)
        p.setY(state->getYVec() * csc.compEx.c2->getResult(state));
      break;

    case XW_TIKZ_CS_XYZ:    
      if (csc.compEx.c1)
        p.setX(state->getXVec() * csc.compEx.c1->getResult(state));
      if (csc.compEx.c2)
        p.setY(state->getYVec() * csc.compEx.c2->getResult(state));
      if (csc.compEx.c3)
        p.setZ(state->getZVec() * csc.compEx.c3->getResult(state));
      break;

    case XW_TIKZ_CS_POLAR_CANVAS:
      {
        double a = 0;
        if (csc.compEx.c1)
          a = csc.compEx.c1->getResult(state);
        double r = 0;
        if (csc.compEx.c2)
          r = csc.compEx.c2->getResult(state);
        p.setX(r * cos(a));
        p.setY(r * sin(a));
      }
      break;

    case XW_TIKZ_CS_POLAR_XYZ:
    case XW_TIKZ_CS_POLAR_XYZ_XY:
      {
        double a = 0;
        if (csc.compEx.c1)
          a = csc.compEx.c1->getResult(state);
        double r = 0;
        if (csc.compEx.c2)
          r = csc.compEx.c2->getResult(state);
        p.setX(state->getXVec() * r * cos(a));
        p.setY(state->getYVec() * r * sin(a));
      }
      break;

    case XW_TIKZ_CS_BARYCENTRIC:
      if (csc.barycentric.num > 0)
      {
        double cx = 0;
        double cy = 0;
        double cz = 0;
        double a = 0;
        for (int i = 0; i < csc.barycentric.num; i++)
        {
          QVector3D tmp = csc.barycentric.vas[i].v->getResult(state);
          double ai = csc.barycentric.vas[i].a->getResult(state);
          a += ai;
          double tmpx = tmp.x() * ai;
          double tmpy = tmp.y() * ai;
          double tmpz = tmp.z() * ai;
          
          cx += tmpx;
          cy += tmpy;
          cz += tmpz;
        }
        p.setX(cx / a);
        p.setY(cy / a);
        p.setZ(cz / a);
      }
      break;

    case XW_TIKZ_CS_NODE_ANCHOR:
      {
        QString n = csc.nodeEx.node->getText();
        int a = csc.nodeEx.a->getResult(state);        
        QPointF np = graphic->getNodeAnchor(n,a);
        p.setX(np.x());
        p.setY(np.y());
      }
      break;

    case XW_TIKZ_CS_NODE_ANGLE:
      {
        QString n = csc.nodeEx.node->getText();
        double a = csc.nodeEx.a->getResult(state);        
        QPointF np = graphic->getNodeAngle(n,a);
        p.setX(np.x());
        p.setY(np.y());
      }
      break;

    case XW_TIKZ_CS_TANGENT:
      {
        QString n = csc.tangent.node->getText();
        QPointF nc = graphic->getNodeAnchor(n,PGFcenter);
        QPointF ne = graphic->getNodeAnchor(n,PGFeast);
        p = csc.tangent.point->getResult(state);
        QPointF pp(p.x(),p.y());
        int s = (int)(csc.tangent.solution->getResult(state));
        QPointF np = state->tangent(nc,ne,pp,s);
        p.setX(np.x());
        p.setY(np.y());
      }
      break;

    case XW_TIKZ_CS_INTERSECTION:
      {
        QString n = csc.coordEx.coord->getText();
        int i = n.length() - 1;
        while (n[i].isDigit())
           i--;

        n = n.mid(i + 1,-1);
        i = n.toInt();
        QPointF t = graphic->getIntersection(i);
        p.setX(t.x());
        p.setY(t.y());
      }
      break;

    case XW_TIKZ_CS_PERPENDICUPAR_HV:
      {
        QVector3D p1 = csc.perpendicular.coord1->getResult(state);
        QVector3D p2 = csc.perpendicular.coord2->getResult(state);
        p.setX(p2.x());
        p.setY(p1.y());
      }
      break;

    case XW_TIKZ_CS_PERPENDICUPAR_VH:
      {
        QVector3D p1 = csc.perpendicular.coord1->getResult(state);
        QVector3D p2 = csc.perpendicular.coord2->getResult(state);
        p.setX(p1.x());
        p.setY(p2.y());
      }
      break;
      
    case XW_TIKZ_CS_NAME:
      if (csc.coordEx.coord)
      {
        QString n = csc.coordEx.coord->getText();
        p = graphic->getPoint3D(n);
      }
      break;

    case XW_TIKZ_CS_CALCU:
      if (csc.coordEx.coord)
        p = csc.coordEx.coord->getResult(state);
      break;

    case XW_TIKZ_CS_XYZ_CYLINDRICAL:
      {
        double a = 0;
        if (csc.cylindrical.angle)
          a = csc.cylindrical.angle->getResult(state);
        double r = 0;
        if (csc.cylindrical.radius)
          r = csc.cylindrical.radius->getResult(state);
        double z = 0;
        if (csc.cylindrical.z)
          z = csc.cylindrical.z->getResult(state);
        p.setX(state->getXVec() * r * cos(a));
        p.setY(state->getYVec() *r * sin(a));
        p.setZ(state->getZVec() * z);
      }
      break;

    case XW_TIKZ_CS_XYZ_SPHERICAL:
      {
        double lo = 0;
        if (csc.spherical.longitude)
          lo = csc.spherical.longitude->getResult(state);
        double la = 0;
        if (csc.spherical.latitude)
          la = csc.spherical.latitude->getResult(state);
        double r = 0;
        if (csc.spherical.radius)
          r = csc.spherical.radius->getResult(state);
        
        p.setX(state->getXVec() * r * cos(lo) * cos(la));
        p.setY(state->getYVec() * r * sin(lo) * cos(la));
        p.setZ(state->getZVec() * r * sin(la));
      }
      break;
  }

  return p;
}

QString XWTikzCoord::getText()
{
  QString ret;
  switch (rel)
  {
    default:
      ret = " (";
      break;

    case PGFSPlus:
      ret = " +(";
      break;

    case PGFDPlus:
      ret = " ++(";
      break;
  }

  switch (cs)
  {
    default:
      break;

    case XW_TIKZ_CS_CANVAS:
      if (csc.compEx.c1)
      {
        QString tmp = csc.compEx.c1->getText();
        ret += tmp;  
      }      
      else
        ret += "1cm,";
      if (csc.compEx.c2)
      {
        QString tmp = csc.compEx.c2->getText();
        ret += tmp;   
      }      
      else
        ret += "1cm";
      break;

    case XW_TIKZ_CS_XY:
      if (csc.compEx.c1)
      {
        QString tmp = csc.compEx.c1->getText();
        ret += tmp;
        ret += ",";
      }
      else
        ret += "1,";
      if (csc.compEx.c2)
      {
        QString tmp = csc.compEx.c2->getText();
        ret += tmp;
      }
      else
        ret += "1";
      break;

    case XW_TIKZ_CS_XYZ:
      if (csc.compEx.c1)
      {
        QString tmp = csc.compEx.c1->getText();
        ret += tmp;
        ret += ",";
      }
      else
        ret += "1,";
      if (csc.compEx.c2)
      {
        QString tmp = csc.compEx.c2->getText();
        ret += tmp;
        ret += ",";
      }
      else
        ret += "1,";
      if (csc.compEx.c3)
      {
        QString tmp = csc.compEx.c3->getText();
        ret += tmp;
      }
      else
        ret += "1";
      break;

    case XW_TIKZ_CS_POLAR_CANVAS:
      if (csc.compEx.c1)
      {
        QString tmp = csc.compEx.c1->getText();
        ret += tmp;
        ret += ":";
      }
      else
        ret += "0:";
      if (csc.compEx.c2)
      {
        QString tmp = csc.compEx.c2->getText();
        ret += tmp;        
      }      
      else
        ret += "1cm";
      break;

    case XW_TIKZ_CS_POLAR_CANVAS_XY:
      ret += "canvas polar cs:";
      if (csc.compEx.c1)
      {
        ret += "x radius=";
        QString tmp = csc.compEx.c1->getText();
        ret += tmp;       
        ret += ",";
      }      
      else
        ret += "1cm,";
      if (csc.compEx.c2)
      {
        ret += "y radius=";
        QString tmp = csc.compEx.c2->getText();
        ret += tmp;       
        ret += ",";   
      }      
      else
        ret += "1cm";
      break;

    case XW_TIKZ_CS_POLAR_XYZ:
    case XW_TIKZ_CS_POLAR_XYZ_XY:
      if (csc.compEx.c1)
      {
        QString tmp = csc.compEx.c1->getText();
        ret += tmp;
        ret += ":";
      }
      else
        ret += "0:";
      if (csc.compEx.c2)
      {
        QString tmp = csc.compEx.c2->getText();
        ret += tmp;
      }
      else
        ret += "1";
      break;

    case XW_TIKZ_CS_BARYCENTRIC:
      ret += "barycentric cs:";
      {
        QString tmp;
        for (int i = 0; i < csc.barycentric.num; i++)
        {
          QString v = csc.barycentric.vas[i].v->getText();
          QString a = csc.barycentric.vas[i].a->getText();
          if (i < (csc.barycentric.num - 1))
            tmp = QString("%1=%2,").arg(v).arg(a);
          else
            tmp = QString("%1=%2").arg(v).arg(a);

          ret += tmp;
        }
      }
      break;

    case XW_TIKZ_CS_NODE_ANCHOR:
      ret += "node cs:";
      if (csc.nodeEx.node)
      {
        ret += "name=";
        QString tmp = csc.nodeEx.node->getText();
        ret += tmp;
        ret += ",";
      }
      if (csc.nodeEx.a)
      {
        ret += "anchor=";
        QString tmp = csc.nodeEx.a->getText();
        ret += tmp;
      }
      break;

    case XW_TIKZ_CS_NODE_ANGLE:
      ret += "node cs:";
      if (csc.nodeEx.node)
      {
        ret += "name=";
        QString tmp = csc.nodeEx.node->getText();
        ret += tmp;
        ret += ",";
      }
      if (csc.nodeEx.a)
      {
        ret += "angle=";
        QString tmp = csc.nodeEx.a->getText();
        ret += tmp;
      }
      break;

    case XW_TIKZ_CS_TANGENT:
      ret += "tangent cs:";
      if (csc.tangent.node)
      {
        ret += "node=";
        QString tmp = csc.tangent.node->getText();
        ret += tmp;
        ret += ",";
      }
      if (csc.tangent.point)
      {
        ret += "point={(";
        QString tmp = csc.tangent.point->getText();
        ret += tmp;
        ret += ")},";
      }
      if (csc.tangent.solution)
      {
        ret += "solution=";
        QString tmp = csc.tangent.solution->getText();
        ret += tmp;
      }
      break;

    case XW_TIKZ_CS_INTERSECTION:
    case XW_TIKZ_CS_NAME:
      {
        QString tmp = csc.coordEx.coord->getText();
        ret += tmp;
      }
      break;

    case XW_TIKZ_CS_PERPENDICUPAR_HV:
      {
        QString c1 = csc.perpendicular.coord1->getText();
        QString c2 = csc.perpendicular.coord2->getText();
        ret = QString("%1 -| %2").arg(c1).arg(c2);
      }
      break;

    case XW_TIKZ_CS_PERPENDICUPAR_VH:
      {
        QString c1 = csc.perpendicular.coord1->getText();
        QString c2 = csc.perpendicular.coord2->getText();
        ret = QString("%1 |- %2").arg(c1).arg(c2);
      }
      break;

    case XW_TIKZ_CS_CALCU:
      ret += "$";
      {
        QString tmp = csc.coordEx.coord->getText();
        ret += tmp;
      }
      ret += "$";
      break;

    case XW_TIKZ_CS_XYZ_CYLINDRICAL:
      ret += "xyz cylindrical cs:";
      if (csc.cylindrical.angle)
      {
        ret += "angle=";
        QString tmp = csc.cylindrical.angle->getText();
        ret += tmp;
        ret += ",";
      }
      if (csc.cylindrical.radius)
      {
        ret += "radius=";
        QString tmp = csc.cylindrical.radius->getText();
        ret += tmp;
        ret += ",";
      }
      if (csc.cylindrical.z)
      {
        ret += "z=";
        QString tmp = csc.cylindrical.z->getText();
        ret += tmp;
      }
      break;

    case XW_TIKZ_CS_XYZ_SPHERICAL:
      ret += "xyz spherical cs:";
      if (csc.spherical.longitude)
      {
        ret += "longitude=";
        QString tmp = csc.spherical.longitude->getText();
        ret += tmp;
        ret += ",";
      }
      if (csc.spherical.latitude)
      {
        ret += "latitude=";
        QString tmp = csc.spherical.latitude->getText();
        ret += tmp;
        ret += ",";
      }
      if (csc.spherical.radius)
      {
        ret += "radius=";
        QString tmp = csc.spherical.radius->getText();
        ret += tmp;
      }
      break;
  }

  ret += ")";
  return ret;
}

QStringList XWTikzCoord::getVarNames()
{
  QStringList ret;
  switch (cs)
  {
    default:
      if (csc.compEx.c1)
        ret = csc.compEx.c1->getVarNames();
      break;

    case XW_TIKZ_CS_XYZ:
      if (csc.compEx.c1)
        ret = csc.compEx.c1->getVarNames();
      if (csc.compEx.c2)
      {
        QStringList tmp = csc.compEx.c2->getVarNames();
        for (int i = 0; i < tmp.size(); i++)
        {
          if (!ret.contains(tmp[i]))
            ret << tmp[i];
        }
      }
      break;
  }

  return ret;
}

bool XWTikzCoord::hitTest(XWTikzState * state)
{
  state->saveTransform();
  options->doCompute(state);
  curPoint = 0;
  bool ret = state->hitTestPoint(this);
  if (ret)
    curPoint = this;

  state->restoreTransform();

  return ret;
}

bool XWTikzCoord::isNull()
{
  return cs == XW_TIKZ_CS_NULL;
}

void XWTikzCoord::moveTo(const QVector3D & p)
{
  switch (cs)
  {
    default:
      break;

    case XW_TIKZ_CS_CANVAS:
    case XW_TIKZ_CS_XY:
      if (csc.compEx.c1 && !csc.compEx.c1->isVar())
        csc.compEx.c1->setValue(p.x());
      if (csc.compEx.c2 && !csc.compEx.c2->isVar())
        csc.compEx.c2->setValue(p.y());
      break;

    case XW_TIKZ_CS_XYZ:
      if (csc.compEx.c1 && !csc.compEx.c1->isVar())
        csc.compEx.c1->setValue(p.x());
      if (csc.compEx.c2 && !csc.compEx.c2->isVar())
        csc.compEx.c2->setValue(p.y());
      if (csc.compEx.c3 && !csc.compEx.c3->isVar())
        csc.compEx.c3->setValue(p.z());
      break;
  }
}

void XWTikzCoord::scan(const QString & str, int & len, int & pos)
{
  if (str[pos] == QChar('+'))
  {
    pos++;
    rel = PGFSPlus;
    if (str[pos] == QChar('+'))
    {
      pos++;
      rel = PGFDPlus;
    }      

    while (str[pos].isSpace())
      pos++;
  }

  if (pos >= len || str[pos] != QChar('('))
    return ;

  pos++;
  options->scan(str,len,pos);
  if (str[pos] == QChar(')'))
  {
    pos++;
    return ;
  }

  int i = pos;
  while (str[pos].isLetter() || str[pos].isSpace())
    pos++;

  if (str[pos] == QChar(':'))
    pos++;

  QString key = str.mid(i, pos - i);
  key = key.simplified();
  if (str[pos] == QChar('$'))
  {
    cs = XW_TIKZ_CS_CALCU;
    pos++;
    while (str[pos] != QChar('$'))
      pos++;

    key = str.mid(i, pos - i);
    key = key.simplified();
    while (str[pos] != QChar(')'))
      pos++;
    pos++;
    csc.coordEx.coord = new XWTikzCoordExpress(graphic,key,this);
  }
  else if (key.isEmpty())
  {
    QString x,y,z;
    bool polar = false;
    bool dim = false;
    scanComponent(str,len,pos,x,polar,dim);
    if (str[pos] == QChar('-') && str[pos + 1] == QChar('|'))
    {
      cs = XW_TIKZ_CS_PERPENDICUPAR_HV;
      pos += 2;
      scanRest(str,len,pos,y);
      csc.perpendicular.coord1 = new XWTikzCoordExpress(graphic,x,this);
      csc.perpendicular.coord2 = new XWTikzCoordExpress(graphic,y,this);
    }
    else if (str[pos] == QChar('|') && str[pos + 1] == QChar('-'))
    {
      cs = XW_TIKZ_CS_PERPENDICUPAR_VH;
      pos += 2;
      scanRest(str,len,pos,y);
      csc.perpendicular.coord1 = new XWTikzCoordExpress(graphic,x,this);
      csc.perpendicular.coord2 = new XWTikzCoordExpress(graphic,y,this);
    }
    else
    {
      csc.compEx.c1 = new XWTikzExpress(graphic,x,this);
      scanComponent(str,len,pos,y,polar,dim);      
      csc.compEx.c2 = new XWTikzExpress(graphic,y,this);
      csc.compEx.c3 = 0;
      if (str[pos] == QChar(')'))
      {
        if (dim)
        {
          if (polar)
            cs = XW_TIKZ_CS_POLAR_CANVAS;
          else
            cs = XW_TIKZ_CS_CANVAS;
        }
        else
        {
          if (polar)
            cs = XW_TIKZ_CS_POLAR_XYZ;
          else
            cs = XW_TIKZ_CS_XY;
        }      
      }
      else
      {
        scanComponent(str,len,pos,z,polar,dim);
        cs = XW_TIKZ_CS_XYZ;
        csc.compEx.c3 = new XWTikzExpress(graphic,z,this);
      }  
    }
    
    pos++;
  }
  else if (key == "canvas cs:")
  {
    cs = XW_TIKZ_CS_CANVAS;
    csc.compEx.c3 = 0;
    QString kx,ky,x,y;
    scanKeyValue(str,len,pos,kx,x);
    scanKeyValue(str,len,pos,ky,y);
    pos++;
    if (!kx.isEmpty() && kx[0] == QChar('x'))
    {
      csc.compEx.c1 = new XWTikzExpress(graphic,x,this);
      csc.compEx.c2 = new XWTikzExpress(graphic,y,this);
    }
    else
    {
      csc.compEx.c1 = new XWTikzExpress(graphic,y,this);
      csc.compEx.c2 = new XWTikzExpress(graphic,x,this);
    }    
  }
  else if (key == "xyz cs:")
  {
    cs = XW_TIKZ_CS_XY;
    csc.compEx.c3 = 0;
    QString kx,ky,kz,x,y,z;
    scanKeyValue(str,len,pos,kx,x);
    scanKeyValue(str,len,pos,ky,y);
    scanKeyValue(str,len,pos,kz,z);
    pos++;

    if (kz.isEmpty())
    {
      if (!kx.isEmpty() && kx[0] == QChar('x'))
      {
        csc.compEx.c1 = new XWTikzExpress(graphic,x,this);
        csc.compEx.c2 = new XWTikzExpress(graphic,y,this);
      }
      else
      {
        csc.compEx.c1 = new XWTikzExpress(graphic,y,this);
        csc.compEx.c2 = new XWTikzExpress(graphic,x,this);
      }   
    }
    else
    {
      cs = XW_TIKZ_CS_XYZ;
      if (!kx.isEmpty() && kx[0] == QChar('x'))
      {
        csc.compEx.c1 = new XWTikzExpress(graphic,x,this);
        if (!ky.isEmpty() && ky[0] == QChar('y'))
        {
          csc.compEx.c2 = new XWTikzExpress(graphic,y,this);
          csc.compEx.c3 = new XWTikzExpress(graphic,z,this);
        }
        else
        {
          csc.compEx.c2 = new XWTikzExpress(graphic,z,this);
          csc.compEx.c3 = new XWTikzExpress(graphic,y,this);
        }        
      }
      else if (!kx.isEmpty() && kx[0] == QChar('y'))
      {
        csc.compEx.c2 = new XWTikzExpress(graphic,x,this);
        if (!ky.isEmpty() && ky[0] == QChar('x'))
        {
          csc.compEx.c1 = new XWTikzExpress(graphic,y,this);
          csc.compEx.c3 = new XWTikzExpress(graphic,z,this);
        }
        else
        {
          csc.compEx.c1 = new XWTikzExpress(graphic,z,this);
          csc.compEx.c3 = new XWTikzExpress(graphic,y,this);
        }    
      }
      else
      {
        csc.compEx.c3 = new XWTikzExpress(graphic,x,this);
        if (!ky.isEmpty() && ky[0] == QChar('x'))
        {
          csc.compEx.c1 = new XWTikzExpress(graphic,y,this);
          csc.compEx.c2 = new XWTikzExpress(graphic,z,this);
        }
        else
        {
          csc.compEx.c1 = new XWTikzExpress(graphic,z,this);
          csc.compEx.c2 = new XWTikzExpress(graphic,y,this);
        }    
      }      
    }
  }
  else if (key == "canvas polar cs:" || key == "xyz polar cs:" || key == "xy polar cs:")
  {
    if (key == "canvas polar cs:")
      cs = XW_TIKZ_CS_POLAR_CANVAS;
    else
      cs = XW_TIKZ_CS_POLAR_XYZ;
    
    csc.compEx.c3 = 0;

    QString ka,kr,kx,ky,a,r,x,y;
    scanKeyValue(str,len,pos,ka,a);
    scanKeyValue(str,len,pos,kr,r);
    pos++;

    if (!ka.isEmpty() && (ka[0] == QChar('a') || ka[0] == QChar('x')))
    {
      if (ka[0] == QChar('x'))
      {
        if (cs == XW_TIKZ_CS_POLAR_CANVAS)
          cs = XW_TIKZ_CS_POLAR_CANVAS_XY;
      }
        
      csc.compEx.c1 = new XWTikzExpress(graphic,a,this);
      csc.compEx.c2 = new XWTikzExpress(graphic,r,this);
    }
    else
    {
      if (!ka.isEmpty() && ka[0] == QChar('y'))
      {
        if (cs == XW_TIKZ_CS_POLAR_CANVAS)
          cs = XW_TIKZ_CS_POLAR_XYZ_XY;
      }
      csc.compEx.c1 = new XWTikzExpress(graphic,r,this);
      csc.compEx.c2 = new XWTikzExpress(graphic,a,this);
    }
  }
  else if (key == "barycentric cs:")
  {
    cs = XW_TIKZ_CS_BARYCENTRIC;
    QString v,a;
    int i = 0;
    while (str[pos] != QChar(')'))
    {
      scanKeyValue(str,len,pos,v,a);
      csc.barycentric.vas = (_CS_VA*)realloc(csc.barycentric.vas,(i + 1) * sizeof(_CS_VA));
      csc.barycentric.vas[i].v = new XWTikzCoordExpress(graphic,v,this);
      csc.barycentric.vas[i].a = new XWTikzExpress(graphic,a,this);
    }
    csc.barycentric.num = i;
    pos++;
  }
  else if (key == "node cs:")
  {
    cs = XW_TIKZ_CS_NODE_ANCHOR;
    QString kn,ka,n,a;
    scanKeyValue(str,len,pos,kn,n);
    scanKeyValue(str,len,pos,ka,a);
    pos++;
    if (!kn.isEmpty() && kn[0] == QChar('n'))
    {
      csc.nodeEx.node = new XWTikzCoordExpress(graphic,n,this);
      if (ka == "anchor")
      {
        csc.nodeEx.a = new XWTikzExpress(graphic,a,this); 
        cs = XW_TIKZ_CS_NODE_ANCHOR;
      }
      else if (ka == "angle")
      {
        csc.nodeEx.a = new XWTikzExpress(graphic,a,this); 
        cs = XW_TIKZ_CS_NODE_ANGLE;
      }
    }
    else
    {
      csc.nodeEx.node = new XWTikzCoordExpress(graphic,a,this);
      
      if (kn == "anchor")
      {
        csc.nodeEx.a = new XWTikzExpress(graphic,n,this);
        cs = XW_TIKZ_CS_NODE_ANCHOR;
      }
      else if (ka == "angle")
      {
        csc.nodeEx.a = new XWTikzExpress(graphic,n,this);
        cs = XW_TIKZ_CS_NODE_ANGLE;
      }
    }    
  }
  else if (key == "tangent cs:")
  {
    csc.tangent.node = 0;
    csc.tangent.point = 0;
    csc.tangent.solution = 0;
    cs = XW_TIKZ_CS_TANGENT;

    QString kn,kp,ks,n,p,s;
    scanKeyValue(str,len,pos,kn,n);
    scanKeyValue(str,len,pos,kp,p);
    scanKeyValue(str,len,pos,ks,s);
    pos++;
    if (!kn.isEmpty())
    {
      if (kn[0] == QChar('n'))
      {
        csc.tangent.node = new XWTikzCoordExpress(graphic,n,this);
        if (!kp.isEmpty())
        {
          if (kp[0] == QChar('p'))
          {
            csc.tangent.point = new XWTikzCoordExpress(graphic,p,this);
            csc.tangent.solution = new XWTikzExpress(graphic,s,this);
          }
          else
          {
            csc.tangent.solution = new XWTikzExpress(graphic,p,this);
            csc.tangent.point = new XWTikzCoordExpress(graphic,s,this);
          }
        }
      }
      else if (kn[0] == QChar('p'))
      {
        csc.tangent.point = new XWTikzCoordExpress(graphic,n,this);
        if (!kp.isEmpty())
        {
          if (kp[0] == QChar('s'))
          {
            csc.tangent.node = new XWTikzCoordExpress(graphic,s,this);
            csc.tangent.solution = new XWTikzExpress(graphic,p,this);            
          }
          else
          {            
            csc.tangent.node = new XWTikzCoordExpress(graphic,p,this);
            csc.tangent.solution = new XWTikzExpress(graphic,s,this);
          }
        }
      }
      else
      {
        csc.tangent.solution = new XWTikzExpress(graphic,n,this);
        if (!kp.isEmpty())
        {
          if (kp[0] == QChar('p'))
          {
            csc.tangent.point = new XWTikzCoordExpress(graphic,p,this);
            csc.tangent.node = new XWTikzCoordExpress(graphic,s,this);
          }
          else
          {
            csc.tangent.point = new XWTikzCoordExpress(graphic,s,this);
            csc.tangent.node = new XWTikzCoordExpress(graphic,p,this);
          }          
        }
      }      
    }
  }
  else if (key == "intersection")
  {
    cs = XW_TIKZ_CS_INTERSECTION;
    while (str[pos] != QChar(')'))
      pos++;

    key = str.mid(i, pos - i);
    key = key.simplified();
    csc.coordEx.coord = new XWTikzCoordExpress(graphic,key,this);
    pos++;
  }
  else if (key == "xyz cylindrical cs:")
  {
    cs = XW_TIKZ_CS_XYZ_CYLINDRICAL;
    csc.cylindrical.angle = 0;
    csc.cylindrical.radius = 0;
    csc.cylindrical.z = 0;
    QString a,av,r,rv,z,zv;
    scanKeyValue(str,len,pos,a,av);
    scanKeyValue(str,len,pos,r,rv);
    scanKeyValue(str,len,pos,z,zv);
    if (a == "angle")
    {
      csc.cylindrical.angle = new XWTikzExpress(graphic,av,this);
      if (r == "radius")
      {
        csc.cylindrical.radius = new XWTikzExpress(graphic,rv,this);
        if (!z.isEmpty())
          csc.cylindrical.z = new XWTikzExpress(graphic,zv,this);
      }
      else if (r == "z")
      {
        csc.cylindrical.z = new XWTikzExpress(graphic,rv,this);
        if (!z.isEmpty())
          csc.cylindrical.radius = new XWTikzExpress(graphic,zv,this);
      }
    }
    else if (a == "radius")
    {
      csc.cylindrical.radius = new XWTikzExpress(graphic,av,this);
      if (r == "angle")
      {
        csc.cylindrical.angle = new XWTikzExpress(graphic,rv,this);
        if (!z.isEmpty())
          csc.cylindrical.z = new XWTikzExpress(graphic,zv,this);
      }
      else if (r == "z")
      {
        csc.cylindrical.z = new XWTikzExpress(graphic,rv,this);
        if (!z.isEmpty())
          csc.cylindrical.angle = new XWTikzExpress(graphic,zv,this);
      }
    }
    else if (a == "z")
    {
      csc.cylindrical.z = new XWTikzExpress(graphic,av,this);
      if (r == "radius")
      {
        csc.cylindrical.radius = new XWTikzExpress(graphic,rv,this);
        if (!z.isEmpty())
          csc.cylindrical.angle = new XWTikzExpress(graphic,zv,this);
      }
      else if (r == "angle")
      {
        csc.cylindrical.angle = new XWTikzExpress(graphic,rv,this);
        if (!z.isEmpty())
          csc.cylindrical.radius = new XWTikzExpress(graphic,zv,this);
      }
    }
  }
  else if (key == "xyz spherical cs:")
  {
    cs = XW_TIKZ_CS_XYZ_SPHERICAL;
    csc.spherical.longitude = 0;
    csc.spherical.latitude = 0;
    csc.spherical.radius = 0;
    QString lo,lov,la,lav,ra,rav;
    scanKeyValue(str,len,pos,lo,lov);
    scanKeyValue(str,len,pos,la,lav);
    scanKeyValue(str,len,pos,ra,rav);
    if (lo == "longitude" || lo == "angle")
    {
      csc.spherical.longitude = new XWTikzExpress(graphic,lov,this);
      if (la == "latitude")
      {
        csc.spherical.latitude = new XWTikzExpress(graphic,lav,this);
        if (!ra.isEmpty())
          csc.spherical.radius = new XWTikzExpress(graphic,rav,this);
      }
      else if (la == "radius")
      {
        csc.spherical.radius = new XWTikzExpress(graphic,lav,this);
        if(!ra.isEmpty())
          csc.spherical.latitude = new XWTikzExpress(graphic,rav,this);
      }
    }
    else if (lo == "latitude")
    {
      csc.spherical.latitude = new XWTikzExpress(graphic,lov,this);
      if (la == "longitude" || la == "angle")
      {
        csc.spherical.longitude = new XWTikzExpress(graphic,lav,this);
        if (!ra.isEmpty())
          csc.spherical.radius = new XWTikzExpress(graphic,rav,this);
      }
      else if (la == "radius")
      {
        csc.spherical.radius = new XWTikzExpress(graphic,lav,this);
        if (!ra.isEmpty())
          csc.spherical.longitude = new XWTikzExpress(graphic,rav,this);
      }
    }
    else if (lo == "radius")
    {
      csc.spherical.radius = new XWTikzExpress(graphic,lov,this);
      if (la == "latitude")
      {
        csc.spherical.latitude = new XWTikzExpress(graphic,lav,this);
        if (!ra.isEmpty())
          csc.spherical.longitude = new XWTikzExpress(graphic,rav,this);
      }
      else if (la == "longitude" || la == "angle")
      {
        csc.spherical.longitude = new XWTikzExpress(graphic,lav,this);
        if (!ra.isEmpty())
          csc.spherical.latitude = new XWTikzExpress(graphic,rav,this);
      }
    }
  }
  else
  {
    QString x,y;
    bool polar = false;
    bool dim = false;
    pos = i;
    scanComponent(str,len,pos,x,polar,dim);
    if (str[pos] == QChar('-') && str[pos + 1] == QChar('|'))
    {
      cs = XW_TIKZ_CS_PERPENDICUPAR_HV;
      scanRest(str,len,pos,y);
      csc.perpendicular.coord1 = new XWTikzCoordExpress(graphic,x,this);
      csc.perpendicular.coord2 = new XWTikzCoordExpress(graphic,y,this);
    }
    else if (str[pos] == QChar('|') && str[pos] == QChar('-'))
    {
      cs = XW_TIKZ_CS_PERPENDICUPAR_VH;
      scanRest(str,len,pos,y);
      csc.perpendicular.coord1 = new XWTikzCoordExpress(graphic,x,this);
      csc.perpendicular.coord2 = new XWTikzCoordExpress(graphic,y,this);
    }
    else
    {
      cs = XW_TIKZ_CS_NAME;
      csc.coordEx.coord = new XWTikzCoordExpress(graphic,x,this);
    }
    
    pos++;
  }
}

void XWTikzCoord::setCoord(const QString & title)
{
  XWTikzCoordDialog dlg(title);
  QString str = getText();
  dlg.setCoord(str);
  if (dlg.exec() == QDialog::Accepted)
  {
    str = dlg.getCoord();
    XWTikzSetCoordinate * cmd = new XWTikzSetCoordinate(this,str);
    graphic->push(cmd);
  }
}

void XWTikzCoord::setText(const QString & str)
{
  clear();
  options = new XWTIKZOptions(graphic,this);
  int len = str.length();
  int pos = 0;
  scan(str,len,pos);
}

void XWTikzCoord::setCoord()
{
  setCoord(tr("move to"));
}

void XWTikzCoord::clear()
{
  if (cs == XW_TIKZ_CS_NULL)
    return ;
    
  if (options)
  {
    delete options;
    options = 0;
  }

  switch (cs)
  {
    default:
      if (csc.compEx.c1)
      {
        delete csc.compEx.c1;
        csc.compEx.c1 = 0;
      }        
      if (csc.compEx.c2)
      {
        delete csc.compEx.c2;
        csc.compEx.c2 = 0;
      }
      break;

    case XW_TIKZ_CS_XYZ:    
      if (csc.compEx.c1)
      {
        delete csc.compEx.c1;
        csc.compEx.c1 = 0;
      }        
      if (csc.compEx.c2)
      {
        delete csc.compEx.c2;
        csc.compEx.c2 = 0;
      }
      if (csc.compEx.c3)
      {
        delete csc.compEx.c3;
        csc.compEx.c3 = 0;
      }
      break;

    case XW_TIKZ_CS_BARYCENTRIC:
      if (csc.barycentric.num > 0)
      {
        for (int i = 0; i < csc.barycentric.num; i++)
        {
          if (csc.barycentric.vas[i].v)
          {
            delete csc.barycentric.vas[i].v;
            csc.barycentric.vas[i].v = 0;
          }
          if (csc.barycentric.vas[i].a)
          {
            delete csc.barycentric.vas[i].a;
            csc.barycentric.vas[i].a = 0;
          }
        }

        free(csc.barycentric.vas);
        csc.barycentric.vas = 0;
        csc.barycentric.num = 0;
      }
      break;

    case XW_TIKZ_CS_NODE_ANCHOR:
    case XW_TIKZ_CS_NODE_ANGLE:
      if (csc.nodeEx.node)
      {
        delete csc.nodeEx.node;
        csc.nodeEx.node = 0;
      }
      if (csc.nodeEx.a)
      {
        delete csc.nodeEx.a;
        csc.nodeEx.a = 0;
      }
      break;

    case XW_TIKZ_CS_TANGENT:
      if (csc.tangent.node)
      {
        delete csc.tangent.node;
        csc.tangent.node = 0;
      }
      if (csc.tangent.point)
      {
        delete csc.tangent.point;
        csc.tangent.point = 0;
      }
      if (csc.tangent.solution)
      {
        delete csc.tangent.solution;
        csc.tangent.solution = 0;
      }
      break;

    case XW_TIKZ_CS_PERPENDICUPAR_HV:
    case XW_TIKZ_CS_PERPENDICUPAR_VH:
      if (csc.perpendicular.coord1)
      {
        delete csc.perpendicular.coord1;
        csc.perpendicular.coord1 = 0;
      }
      if (csc.perpendicular.coord2)
      {
        delete csc.perpendicular.coord2;
        csc.perpendicular.coord2 = 0;
      }
      break;

    case XW_TIKZ_CS_INTERSECTION:
    case XW_TIKZ_CS_NAME:
    case XW_TIKZ_CS_CALCU:
      if (csc.coordEx.coord)
      {
        delete csc.coordEx.coord;
        csc.coordEx.coord = 0;
      }        
      break;

    case XW_TIKZ_CS_XYZ_CYLINDRICAL:
      if (csc.cylindrical.radius)
      {
        delete csc.cylindrical.radius;
        csc.cylindrical.radius = 0;
      }
      if (csc.cylindrical.z)
      {
        delete csc.cylindrical.z;
        csc.cylindrical.z = 0;
      }
      break;

    case XW_TIKZ_CS_XYZ_SPHERICAL:
      if (csc.spherical.longitude)
      {
        delete csc.spherical.longitude;
        csc.spherical.longitude = 0;
      }
      if (csc.spherical.latitude)
      {
        delete csc.spherical.latitude;
        csc.spherical.latitude = 0;
      }
      if (csc.spherical.radius)
      {
        delete csc.spherical.radius;
        csc.spherical.radius = 0;
      }
      break;
  }
}

void XWTikzCoord::scanComponent(const QString & str, 
                                int & len, 
                                int & pos, 
                                QString & compA,
                                bool & polar,
                                bool & dim)
{
  if (pos >= len || str[pos] == QChar(')'))
    return ;

  int i = pos;
  while ((pos < len) && 
          str[pos] != QChar(',') && 
          str[pos] != QChar(':') && 
          str[pos] != QChar(')'))
  {
    if ((str[pos] == QChar('-') && str[pos+1] == QChar('|')) ||
        (str[pos] == QChar('|') && str[pos+1] == QChar('-')))
    {
      break;
    }
    pos++;
  }

  compA = str.mid(i, pos - i);
  compA = compA.simplified();
  if (compA.endsWith("cm") || compA.endsWith("mm") || compA.endsWith("pt"))
    dim = true;

  if (str[pos] == QChar(',') || str[pos] == QChar(':'))
  {
    if (str[pos] == QChar(':'))
      polar = true;
    pos++;
    i = pos;
    while (str[i] != QChar(')'))
    {
      if ((str[i] == QChar('-') && str[i+1] == QChar('|')) ||
        (str[i] == QChar('|') && str[i+1] == QChar('-')))
      {
        compA = str.mid(pos, i);
        compA = compA.simplified();
        pos = i;
        break;
      }

      i++;
    }
  }
}

void XWTikzCoord::scanRest(const QString & str, 
                     int & len, int & pos, 
                     QString & compA)
{
  if (pos >= len || str[pos] == QChar(')'))
    return ;

  int i = pos;
  while (str[pos] != QChar(')'))
    pos++;

  compA = str.mid(i, pos - i);
  compA = compA.simplified();
}
