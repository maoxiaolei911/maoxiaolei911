/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <math.h>
#include <QFile>
#include <QTextStream>
#include "XWDocSea.h"
#include "XWPictureSea.h"
#include "XWPGFPool.h"
#include "PGFKeyWord.h"
#include "XWTeXBox.h"
#include "XWTikzState.h"
#include "XWTikzExpress.h"
#include "XWTikzGraphic.h"
#include "XWTikzCommand.h"
#include "XWTikzCoord.h"
#include "XWTikzOptions.h"
#include "XWTikzNode.h"
#include "XWTikzOptionDialog.h"
#include "XWTikzUndoCommand.h"
#include "XWTikzOperationDialog.h"
#include "XWTikzOperation.h"

XWTikzOperation::XWTikzOperation(XWTikzGraphic * graphicA, int idA, QObject * parent)
 :QObject(parent),
  graphic(graphicA),
  keyWord(idA),
  subkeyWord(-1),
  curPoint(0)
{}

XWTikzOperation::XWTikzOperation(XWTikzGraphic * graphicA, int idA, int subk, QObject * parent)
:QObject(parent),
  graphic(graphicA),
  keyWord(idA),
  subkeyWord(subk),
  curPoint(0)
{}

XWTikzOperation::~XWTikzOperation()
{}

bool XWTikzOperation::addAction(QMenu &, XWTikzState * )
{
  return false;
}

bool XWTikzOperation::back(XWTikzState *)
{
  return false;
}

bool XWTikzOperation::cut(XWTikzState * )
{
  return false;
}

bool XWTikzOperation::del(XWTikzState * )
{
  return false;
}

void XWTikzOperation::doCompute(XWTikzState *)
{}

void XWTikzOperation::doPath(XWTikzState *, bool)
{}

void XWTikzOperation::doPre(XWTikzState * , bool )
{}

void XWTikzOperation::doPost(XWTikzState * , bool)
{}

void XWTikzOperation::dragTo(XWTikzState * )
{
}

bool XWTikzOperation::dropTo(XWTikzState * )
{
  return false;
}

QPointF XWTikzOperation::getAnchor(int , XWTikzState * )
{
  return QPointF();
}

QPointF XWTikzOperation::getAnchor(XWTikzState * )
{
  return QPointF();
}

int XWTikzOperation::getAnchorPosition()
{
  return -1;
}

QPointF XWTikzOperation::getAngle(double , XWTikzState * )
{
  return QPointF();
}

QPointF XWTikzOperation::getBorder(const QPointF & ,XWTikzState * )
{
  return QPointF();
}

XWTikzCoord * XWTikzOperation::getCurrentPoint()
{
  return curPoint;
}

int XWTikzOperation::getCursorPosition()
{
  return -1;
}

QString XWTikzOperation::getCurrentText()
{
  return QString();
}

QPointF XWTikzOperation::getPoint(XWTikzState * )
{
  return QPointF();
}

QVector3D XWTikzOperation::getPoint3D(XWTikzState * )
{
  return QVector3D();
}

double XWTikzOperation::getRadius(XWTikzState * )
{
  return 0.0;
}

QString XWTikzOperation::getSelectedText()
{
  return QString();
}

QString XWTikzOperation::getText()
{
  return QString();
}

QString XWTikzOperation::getTips(XWTikzState *state)
{
  if (hitTest(state))
    return getText();

  return QString();
}

void XWTikzOperation::goToEnd()
{}

bool XWTikzOperation::goToNext()
{
  return false;
}

bool XWTikzOperation::goToPrevious()
{
  return false;
}

void XWTikzOperation::goToStart()
{}

bool XWTikzOperation::hitTest(XWTikzState *)
{
  return false;
}

bool XWTikzOperation::keyInput(XWTikzState * )
{
  return false;
}

bool XWTikzOperation::insertText(XWTikzState *)
{
  return false;
}

bool XWTikzOperation::isMe(const QString &,XWTikzState *)
{
  return false;
}

bool XWTikzOperation::newPar(XWTikzState *)
{
  return false;
}

bool XWTikzOperation::paste(XWTikzState * )
{
  return false;
}

void XWTikzOperation::scan(const QString &, int &, int &)
{}

void XWTikzOperation::scanKey(const QString & str, int & len, 
                    int & pos, QString & key)
{
  key.clear();
  if (pos >= len || 
      str[pos] == QChar(']') || 
      str[pos] == QChar('}') || 
      str[pos] == QChar(')'))
    return ;

  int i = pos;
  while ((pos < len) && 
        str[pos] != QChar('=') && 
        str[pos] != QChar(',') && 
        str[pos] != QChar(']') && 
        str[pos] != QChar('}') && 
        str[pos] != QChar(')'))
    pos++;

  key = str.mid(i, pos - i);
  key = key.simplified();
  if (str[pos] == QChar('='))
    pos++;
}

void XWTikzOperation::scanKeyValue(const QString & str, int & len, 
                                   int & pos, QString & key, 
                                   QString & value)
{
  scanKey(str,len,pos,key);
  scanValue(str,len,pos,value);
}

void XWTikzOperation::scanValue(const QString & str, int & len, 
                    int & pos, QString & value)
{
  value.clear();
  if (pos >= len ||
      str[pos] == QChar(',') || 
      str[pos] == QChar(']') || 
      str[pos] == QChar('}') || 
      str[pos] == QChar(')'))
    return ;

  while (str[pos].isSpace())
    pos++;

  bool b = false;
  if (str[pos] == QChar('{'))
  {
    pos++;
    b = true;
  }

  int i = pos;
  while ((pos < len) && 
         str[pos] != QChar(',') && 
         str[pos] != QChar(']') && 
         str[pos] != QChar('}') && 
         str[pos] != QChar(')'))
    pos++;

  if (b && str[pos] == QChar('}'))
    pos++;
  value = str.mid(i, pos - i);
  value = value.simplified();
  if (str[pos] == QChar(','))
    pos++;
}

XWTikzMacro::XWTikzMacro(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, -1,parent)
{}

void XWTikzMacro::doCompute(XWTikzState * state)
{
  XWTikzCoord * p = state->getCoord(name);
  if (p)
    p->doCompute(state);
}

void XWTikzMacro::doPath(XWTikzState * state, bool showpoint)
{
  XWTikzCoord * p = state->getCoord(name);
  if (p)
    p->doPath(state,showpoint);
}

void XWTikzMacro::scan(const QString & str, int & len, int & pos)
{
  name = XWTeXBox::scanControlSequence(str,len,pos);
  name.insert(0,QChar('\\'));
}

XWTikzLocal::XWTikzLocal(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, XW_TIKZ_LOCAL,parent),
cur(-1)
{}

bool XWTikzLocal::back(XWTikzState * state)
{
  if (cur < 0 || cur >= ops.size())
    return false;

  return ops[cur]->back(state);
}

bool XWTikzLocal::del(XWTikzState * state)
{
  if (cur < 0 || cur >= ops.size())
    return false;

  return ops[cur]->del(state);
}

void XWTikzLocal::doCompute(XWTikzState * state)
{
  for (int i = 0; i < ops.size(); i++)
    ops[i]->doCompute(state);
}

void XWTikzLocal::doPath(XWTikzState * state, bool showpoint)
{
  for (int i = 0; i < ops.size(); i++)
    ops[i]->doPath(state,showpoint);
}

void XWTikzLocal::dragTo(XWTikzState * state)
{
  if (cur < 0 || cur >= ops.size())
    return ;

  ops[cur]->dragTo(state);
}

bool XWTikzLocal::dropTo(XWTikzState * state)
{
  if (cur < 0 || cur >= ops.size())
    return false;

  return ops[cur]->dropTo(state);
}

int XWTikzLocal::getCursorPosition()
{
  if (cur < 0 || cur >= ops.size())
    return 0;

  return ops[cur]->getCursorPosition();
}

QString XWTikzLocal::getCurrentText()
{
  if (cur < 0 || cur >= ops.size())
    return QString();

  return ops[cur]->getCurrentText();
}

QString XWTikzLocal::getSelectedText()
{
  if (cur < 0 || cur >= ops.size())
    return QString();

  return ops[cur]->getSelectedText();
}

QString XWTikzLocal::getText()
{
  QString ret = "{";
  for (int i = 0; i < ops.size(); i++)
  {
    QString tmp = ops[i]->getText();
    ret += tmp;
  }

  ret += "}";

  return ret;
}

QString XWTikzLocal::getTips(XWTikzState * state)
{
  if (cur < 0 || cur >= ops.size())
    return QString();

  return ops[cur]->getTips(state);
}

bool XWTikzLocal::hitTest(XWTikzState * state)
{
  bool ret = false;
  for (int i = 0; i < ops.size(); i++)
  {
    ret = ops[i]->hitTest(state);
    if (ret)
    {
      cur = i;
      break;
    }
  }

  return ret;
}

bool XWTikzLocal::insertText(XWTikzState * state)
{
  if (cur < 0 || cur >= ops.size())
    return false;

  return ops[cur]->insertText(state);
}

bool XWTikzLocal::keyInput(XWTikzState * state)
{
  if (cur < 0 || cur >= ops.size())
    return false;

  return ops[cur]->keyInput(state);
}

bool XWTikzLocal::newPar(XWTikzState * state)
{
  if (cur < 0 || cur >= ops.size())
    return false;

  return ops[cur]->newPar(state);
}

bool XWTikzLocal::paste(XWTikzState * state)
{
  if (cur < 0 || cur >= ops.size())
    return false;

  return ops[cur]->paste(state);
}

void XWTikzLocal::scan(const QString & str, int & len, int & pos)
{
  while (str[pos].isSpace())
    pos++;

  if (str[pos] != QChar('{'))
    return ;

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
          pos += 7;
          obj = new XWTikzEllipse(graphic,PGFellipse,this);
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

XWTikzLineTo::XWTikzLineTo(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, PGFlineto,parent)
{
  coord = new XWTikzCoord(graphicA,this);
}

bool XWTikzLineTo::addAction(QMenu & menu, XWTikzState *)
{
  QAction * a = menu.addAction(tr("line to"));
  connect(a, SIGNAL(triggered()), this, SLOT(setCoord()));
  return true;
}

void XWTikzLineTo::doCompute(XWTikzState * state)
{
  coord->doCompute(state);
}

void XWTikzLineTo::doPath(XWTikzState * state, bool showpoint)
{
  state->lineTo(coord);
  if (showpoint)
    coord->draw(state);
}

void XWTikzLineTo::dragTo(XWTikzState * state)
{
  QPointF mp = state->getLastMousePoint();
  state->lineTo(mp);
}

bool XWTikzLineTo::dropTo(XWTikzState * state)
{
  if (!curPoint)
    return false;

  return coord->dropTo(state);
}

QPointF XWTikzLineTo::getPoint(XWTikzState * state)
{
  return coord->getPoint(state);
}

QString XWTikzLineTo::getText()
{
  QString ret = " -- ";
  QString tmp = coord->getText();
  ret += tmp;
  return ret;
}

bool XWTikzLineTo::hitTest(XWTikzState * state)
{
  curPoint = 0;
  if (state->hitTestPoint(coord))
  {
    curPoint = coord;
    return true;
  }
  
  return state->hitTestLine();
}

void XWTikzLineTo::scan(const QString & str, int & len, int & pos)
{
  while (str[pos].isSpace())
    pos++;

  coord->scan(str,len,pos);
}

void XWTikzLineTo::setCoord(const QString & str)
{
  coord->setText(str);
}

void XWTikzLineTo::setCoord()
{
  coord->setCoord(tr("line to"));
}

XWTikzHVLinesTo::XWTikzHVLinesTo(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, PGFhorivertlines,parent)
{
  coord = new XWTikzCoord(graphicA,this);
}

bool XWTikzHVLinesTo::addAction(QMenu & menu, XWTikzState *)
{
  QAction * a = menu.addAction(tr("horizontal,vertical"));
  connect(a, SIGNAL(triggered()), this, SLOT(setCoord()));
  return true;
}

void XWTikzHVLinesTo::doCompute(XWTikzState * state)
{
  coord->doCompute(state);
}

void XWTikzHVLinesTo::doPath(XWTikzState * state, bool showpoint)
{
  state->lineToHV(coord);
  if (showpoint)
    coord->draw(state);
}

void XWTikzHVLinesTo::dragTo(XWTikzState * state)
{
  QPointF mp = state->getLastMousePoint();
  state->lineToHV(mp);
}

bool XWTikzHVLinesTo::dropTo(XWTikzState * state)
{
  if (!curPoint)
    return false;

  return coord->dropTo(state);
}

QPointF XWTikzHVLinesTo::getPoint(XWTikzState * state)
{
  return coord->getPoint(state);
}

QString XWTikzHVLinesTo::getText()
{
  QString ret = " -| ";
  QString tmp = coord->getText();
  ret += tmp;
  return ret;
}

bool XWTikzHVLinesTo::hitTest(XWTikzState * state)
{
  curPoint = 0;
  if (state->hitTestPoint(coord))
  {
    curPoint = coord;
    return true;
  }

  return state->hitTestHVLine();
}

void XWTikzHVLinesTo::scan(const QString & str, int & len, int & pos)
{
  while (str[pos].isSpace())
    pos++;

  coord->scan(str,len,pos);
}

void XWTikzHVLinesTo::setCoord(const QString & str)
{
  coord->setText(str);
}

void XWTikzHVLinesTo::setCoord()
{
  coord->setCoord(tr("first horizontal, then vertical"));
}

XWTikzVHLinesTo::XWTikzVHLinesTo(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, PGFverthorilines,parent)
{
  coord = new XWTikzCoord(graphicA,this);
}

bool XWTikzVHLinesTo::addAction(QMenu & menu, XWTikzState *)
{
  QAction * a = menu.addAction(tr("vertical,horizontal"));
  connect(a, SIGNAL(triggered()), this, SLOT(setCoord()));
  return true;
}

void XWTikzVHLinesTo::doCompute(XWTikzState * state)
{
  coord->doCompute(state);
}

void XWTikzVHLinesTo::doPath(XWTikzState * state, bool showpoint)
{
  state->lineToVH(coord);
  if (showpoint)
    coord->draw(state);
}

void XWTikzVHLinesTo::dragTo(XWTikzState * state)
{
  QPointF mp = state->getLastMousePoint();
  state->lineToVH(mp);
}

bool XWTikzVHLinesTo::dropTo(XWTikzState * state)
{
  if (!curPoint)
    return false;

  return coord->dropTo(state);
}

QPointF XWTikzVHLinesTo::getPoint(XWTikzState * state)
{
  return coord->getPoint(state);
}

QString XWTikzVHLinesTo::getText()
{
  QString ret = " |- ";
  QString tmp = coord->getText();
  ret += tmp;
  return ret;
}

bool XWTikzVHLinesTo::hitTest(XWTikzState * state)
{
  curPoint = 0;
  if (state->hitTestPoint(coord))
  {
    curPoint = coord;
    return true;
  }    

  return state->hitTestVHLine();
}

void XWTikzVHLinesTo::scan(const QString & str, int & len, int & pos)
{
  while (str[pos].isSpace())
    pos++;

  coord->scan(str,len,pos);
}

void XWTikzVHLinesTo::setCoord(const QString & str)
{
  coord->setText(str);
}

void XWTikzVHLinesTo::setCoord()
{
  coord->setCoord(tr("firrst vertical, then horizontal"));
}

XWTikzCurveTo::XWTikzCurveTo(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, PGFcurveto,parent),
c2(0)
{
  c1 = new XWTikzCoord(graphicA,this);
  endPoint = new XWTikzCoord(graphicA,this);
}

bool XWTikzCurveTo::addAction(QMenu & menu, XWTikzState *)
{
  QAction * a = menu.addAction(tr("first control point"));
  connect(a, SIGNAL(triggered()), this, SLOT(setC1()));
  a = menu.addAction(tr("second control point"));
  connect(a, SIGNAL(triggered()), this, SLOT(setC2()));
  a = menu.addAction(tr("end point"));
  connect(a, SIGNAL(triggered()), this, SLOT(setEndPoint()));
  return true;
}

void XWTikzCurveTo::doCompute(XWTikzState * state)
{
  c1->doCompute(state);
  if (c2)
    c2->doCompute(state);
  endPoint->doCompute(state);
}

void XWTikzCurveTo::doPath(XWTikzState * state, bool showpoint)
{
  if (c2)
  {
    state->curveTo(c1,c2,endPoint);
    if (showpoint)
    {
      c1->draw(state);
      c2->draw(state);
      endPoint->draw(state);
    }
  }
  else
  {
    state->curveTo(c1,endPoint);
    if (showpoint)
    {
      c1->draw(state);
      endPoint->draw(state);
    }
  }    
}

void XWTikzCurveTo::dragTo(XWTikzState * state)
{
  if (!curPoint)
    return ;

  QPointF mp = state->getLastMousePoint();
  if (c2)
  {
    if (c1 == curPoint)
    {
      QPointF p2 = c2->getPoint(state);
      QPointF ep = endPoint->getPoint(state);
      state->curveTo(mp,p2,ep);
    }
    else if (c2 == curPoint)
    {
      QPointF p1 = c1->getPoint(state);
      QPointF ep = endPoint->getPoint(state);
      state->curveTo(p1,mp,ep);
    }
    else
    {
      QPointF p1 = c1->getPoint(state);
      QPointF p2 = c2->getPoint(state);
      state->curveTo(p1,p2,mp);
    }
  }
  else
  {
    if (c1 == curPoint)
    {
      QPointF ep = endPoint->getPoint(state);
      state->curveTo(mp,mp,ep);
    }
    else
    {
      QPointF p1 = c1->getPoint(state);
      state->curveTo(p1,p1,mp);
    }
  }
}

bool XWTikzCurveTo::dropTo(XWTikzState * state)
{
  bool ret = false;
  if (curPoint)
    ret = curPoint->dropTo(state);

  return ret;
}

QPointF XWTikzCurveTo::getPoint(XWTikzState * state)
{
  return endPoint->getPoint(state);
}

QString XWTikzCurveTo::getText()
{
  QString ret = " .. controls ";
  QString tmp = c1->getText();
  ret += tmp;
  if (c2)
  {
    ret += " and ";
    tmp = c2->getText();
    ret += tmp;
  }

  ret += " .. ";

  tmp = endPoint->getText();
  ret += tmp;
  return ret;
}

void XWTikzCurveTo::goToEnd()
{
  curPoint = endPoint;
}

bool XWTikzCurveTo::goToNext()
{
  if (curPoint == endPoint)
    return false;

  if (curPoint == c1)
  {
    if (c2)
      curPoint = c2;
    else
      curPoint = endPoint;
  }
  else if (curPoint == c2)
    curPoint = endPoint;

  return true;
}

bool XWTikzCurveTo::goToPrevious()
{
  if (curPoint == c1)
    return false;

  if (curPoint == endPoint)
  {
    if (c2)
      curPoint = c2;
    else
      curPoint = c1;
  }
  else if (curPoint == c2)
    curPoint = c1;

  return true;
}

void XWTikzCurveTo::goToStart()
{
  curPoint = c1;
}

bool XWTikzCurveTo::hitTest(XWTikzState * state)
{
  curPoint = 0;
  if (c1->hitTest(state))
  {    
    curPoint = c1;
    return true;
  }

  state->removeLastCoord();

  if (endPoint->hitTest(state))
  {
    curPoint = endPoint;
    return true;
  }

  if (c2)
  {
    if (c2->hitTest(state))
    {
      curPoint = c2;
      return true;
    }
    state->removeLastCoord();
  }

  return state->hitTestCurve(c1,c2);
}

void XWTikzCurveTo::scan(const QString & str, int & len, int & pos)
{
  while (str[pos].isSpace())
    pos++;

  c1->scan(str,len,pos);

  while (str[pos].isSpace())
    pos++;

  if (str[pos] == QChar('a'))
  {
    c2 = new XWTikzCoord(graphic,this);

    pos += 3;
    while (str[pos].isSpace())
      pos++;

    c2->scan(str,len,pos);
  }

  endPoint->scan(str,len,pos);
}

void XWTikzCurveTo::setC1(const QString & str)
{
  c1->setText(str);
}

void XWTikzCurveTo::setC2(const QString & str)
{
  if (str.isEmpty())
  {
    if (c2)
    {
      delete c2;
      c2 = 0;
    }
  }
  else
  {
    if (!c2)
      c2 = new XWTikzCoord(graphic,str,this);
    else
      c2->setText(str);
  }
}

void XWTikzCurveTo::setEndPoint(const QString & str)
{
  endPoint->setText(str);
}

void XWTikzCurveTo::setC1()
{
  c1->setCoord(tr("first control point"));
}

void XWTikzCurveTo::setC2()
{
  XWTikzCoordDialog dlg(tr("second control point"));
  if (c2)
  {    
    QString str = c2->getText();
    dlg.setCoord(str);
    if (dlg.exec() == QDialog::Accepted)
    {
      str = dlg.getCoord();
      if (str.isEmpty())
      {
        delete c2;
        c2 = 0;
      }
      else
      {
        XWTikzSetCoordinate * cmd = new XWTikzSetCoordinate(c2,str);
        graphic->push(cmd);
      }      
    }
  }
  else
  {
    if (dlg.exec() == QDialog::Accepted)
    {
      QString str = dlg.getCoord();
      c2 = new XWTikzCoord(graphic,str);
    }
  }  
}

void XWTikzCurveTo::setEndPoint()
{
  endPoint->setCoord(tr("end point"));
}

XWTikzCycle::XWTikzCycle(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, PGFcycle,parent)
{}

void XWTikzCycle::doPath(XWTikzState * state, bool)
{
  state->cycle();
}

QString XWTikzCycle::getText()
{
  QString ret = " cycle";
  return ret;
}

bool XWTikzCycle::hitTest(XWTikzState * state)
{
  return state->hitTestCycle();
}

XWTikzRectangle::XWTikzRectangle(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, PGFrectangle,parent)
{
  coord = new XWTikzCoord(graphicA,this);
}

bool XWTikzRectangle::addAction(QMenu & menu, XWTikzState *)
{
  QAction * a = menu.addAction(tr("corner"));
  connect(a, SIGNAL(triggered()), this, SLOT(setCoord()));
  return true;
}

void XWTikzRectangle::doCompute(XWTikzState * state)
{
  coord->doCompute(state);
}

void XWTikzRectangle::doPath(XWTikzState * state, bool showpoint)
{
  state->addRectangle(coord);
  if (showpoint)
    coord->draw(state);
}

void XWTikzRectangle::dragTo(XWTikzState * state)
{
  QPointF mp = state->getLastMousePoint();
  state->addRectangle(mp);
}

bool XWTikzRectangle::dropTo(XWTikzState * state)
{
  if (!curPoint)
    return false;

  return coord->dropTo(state);
}

QPointF XWTikzRectangle::getPoint(XWTikzState * state)
{
  return coord->getPoint(state);
}

QString XWTikzRectangle::getText()
{
  QString ret = " rectangle ";
  QString tmp = coord->getText();
  ret += tmp;
  return ret;
}

bool XWTikzRectangle::hitTest(XWTikzState * state)
{
  curPoint = 0;
  if (coord->hitTest(state))
  {
    curPoint = coord;
    return true;
  }

  return state->hitTestRectangle();
}

void XWTikzRectangle::scan(const QString & str, int & len, int & pos)
{
  while (str[pos].isSpace())
    pos++;

  coord->scan(str,len,pos);
}

void XWTikzRectangle::setCoord(const QString & str)
{
  coord->setText(str);
}

void XWTikzRectangle::setCoord()
{
  coord->setCoord(tr("corner"));
}

XWTikzEllipse::XWTikzEllipse(XWTikzGraphic * graphicA, int idA,QObject * parent)
:XWTikzOperation(graphicA, idA,parent)
{
  options = new XWTIKZOptions(graphicA,this);
}

bool XWTikzEllipse::addAction(QMenu & menu, XWTikzState * state)
{
  options->doPath(state,false);
  if (keyWord == PGFcircle)
    options->addRadiusAction(menu);
  else
  {
    options->addXRadiusAction(menu);
    options->addYRadiusAction(menu);
  }
  
  options->addAtAction(menu);
  
  return true;
}

void XWTikzEllipse::doCompute(XWTikzState * state)
{
  options->doCompute(state);
  state->addEllipse();
}

void XWTikzEllipse::doPath(XWTikzState * state, bool showpoint)
{
  options->doPath(state,showpoint);
  state->addEllipse();
}

void XWTikzEllipse::dragTo(XWTikzState * state)
{
  options->dragTo(state);
  state->addCircle();
}

bool XWTikzEllipse::dropTo(XWTikzState * state)
{
  return options->dropTo(state);
}

QString XWTikzEllipse::getText()
{
  QString ret;
  if (keyWord == PGFcircle)
    ret = " circle";
  else
    ret = " ellipse";

  QString tmp = options->getText();
  ret += tmp;
  return ret;
}

bool XWTikzEllipse::hitTest(XWTikzState * state)
{
  if (options->hitTest(state))
    return true;
  return state->hitTestEllipse();
}

void XWTikzEllipse::scan(const QString & str, int & len, int & pos)
{
  options->scan(str,len,pos);
}

void XWTikzEllipse::setAt(const QString & str)
{
  options->addValueCoord(PGFat,str);
}

void XWTikzEllipse::setRadius(const QString & str)
{
  options->addValueExpress(PGFradius,str);
}

void XWTikzEllipse::setXRadius(const QString & str)
{
  options->addValueExpress(PGFxradius,str);
}

void XWTikzEllipse::setYRadius(const QString & str)
{
  options->addValueExpress(PGFyradius,str);
}

XWTikzArc::XWTikzArc(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, PGFarc,parent)
{
  options = new XWTIKZOptions(graphicA,this);
}

bool XWTikzArc::addAction(QMenu & menu, XWTikzState * state)
{
  options->doPath(state,false);
  options->addAngleAction(menu);
  return true;
}

void XWTikzArc::doCompute(XWTikzState * state)
{
  options->doCompute(state);
  state->addArc();
}

void XWTikzArc::doPath(XWTikzState * state, bool showpoint)
{
  options->doPath(state,showpoint);
  state->addArc();
}

void XWTikzArc::dragTo(XWTikzState * state)
{
  options->dragTo(state);
  state->addArc();
}

bool XWTikzArc::dropTo(XWTikzState * state)
{
  return options->dropTo(state);
}

QString XWTikzArc::getText()
{
  QString ret = " arc";
  QString tmp = options->getText();
  ret += tmp;
  return ret;
}

bool XWTikzArc::hitTest(XWTikzState * state)
{
  if (options->hitTest(state))
    return true;

  return state->hitTestArc();
}

void XWTikzArc::scan(const QString & str, int & len, int & pos)
{
  options->scan(str,len,pos);
}

void XWTikzArc::setEndAngle(const QString & str)
{
  options->addValueExpress(PGFendangle,str);
}

void XWTikzArc::setStartAngle(const QString & str)
{
  options->addValueExpress(PGFstartangle,str);
}

XWTikzGrid::XWTikzGrid(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, PGFgrid,parent)
{
  options = new XWTIKZOptions(graphicA,this);
  coord = new XWTikzCoord(graphicA,this);
}

bool XWTikzGrid::addAction(QMenu & menu, XWTikzState * state)
{
  options->doPath(state,false);
  QAction * a = menu.addAction(tr("corner"));
  connect(a, SIGNAL(triggered()), this, SLOT(setCoord()));
  options->addStepAction(menu);
  return true;
}

void XWTikzGrid::doCompute(XWTikzState * state)
{
  options->doCompute(state);
  coord->doCompute(state);
}

void XWTikzGrid::doPath(XWTikzState * state, bool showpoint)
{
  options->doPath(state,showpoint);
  state->addGrid(coord);
  if (showpoint)
    coord->draw(state);
}

void XWTikzGrid::dragTo(XWTikzState * state)
{
  if (!curPoint)
    return ;

  options->dragTo(state);
  QPointF mp = state->getLastMousePoint();
  state->addGrid(mp);
}

bool XWTikzGrid::dropTo(XWTikzState * state)
{
  if (!curPoint)
    return false;

  options->dropTo(state);    
  return coord->dropTo(state);
}

QPointF XWTikzGrid::getPoint(XWTikzState * state)
{
  return coord->getPoint(state);
}

QString XWTikzGrid::getText()
{
  QString ret = " grid";
  QString tmp = options->getText();
  ret += tmp;
  tmp = coord->getText();
  ret += tmp;
  return ret;
}

bool XWTikzGrid::hitTest(XWTikzState * state)
{
  curPoint = 0;
  options->hitTest(state);
  if (coord->hitTest(state))
  {
    curPoint = coord;
    return true;
  }

  return state->hitTestGrid();
}

void XWTikzGrid::scan(const QString & str, int & len, int & pos)
{
  options->scan(str,len,pos);
  while (str[pos].isSpace())
    pos++;
  coord->scan(str,len,pos);
}

void XWTikzGrid::setCorner(const QString & str)
{
  coord->setText(str);
}

void XWTikzGrid::setStep(const QString & str)
{
  options->addValueExpress(PGFstep,str);
}

void XWTikzGrid::setCoord()
{
  coord->setCoord(tr("corner"));
}

XWTikzParabola::XWTikzParabola(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, PGFparabola,parent),
bendCoord(0)
{
  options = new XWTIKZOptions(graphicA,this);
  coord = new XWTikzCoord(graphicA,this);
}

bool XWTikzParabola::addAction(QMenu & menu, XWTikzState * state)
{
  options->doPath(state,false);
  QAction * a = menu.addAction(tr("bend point"));
  connect(a, SIGNAL(triggered()), this, SLOT(setBend()));
  a = menu.addAction(tr("end point"));
  connect(a, SIGNAL(triggered()), this, SLOT(setEnd()));
  return true;
}

void XWTikzParabola::doCompute(XWTikzState * state)
{
  options->doCompute(state);
  state->addParabola(bendCoord,coord);
}

void XWTikzParabola::doPath(XWTikzState * state, bool showpoint)
{
  options->doPath(state,showpoint);
  state->addParabola(bendCoord,coord);
  if (showpoint)
  {
    coord->draw(state);
    if (bendCoord)
      bendCoord->draw(state);
  }
}

void XWTikzParabola::dragTo(XWTikzState * state)
{
  if (!curPoint)
    return ;

  QPointF mp = state->getLastMousePoint();
  if (curPoint == bendCoord)
  {
    QPointF p = coord->getPoint(state);
    state->addParabola(mp,p);
  }
  else if (bendCoord)
  {
    QPointF p = bendCoord->getPoint(state);
    state->addParabola(p,mp);
  }
  else
    state->addParabola(mp);
}

bool XWTikzParabola::dropTo(XWTikzState * state)
{
  if (curPoint)
    return curPoint->dropTo(state);

  return false;
}

QPointF XWTikzParabola::getPoint(XWTikzState * state)
{
  return coord->getPoint(state);
}

QString XWTikzParabola::getText()
{
  QString ret = " paralobla";
  QString tmp = options->getText();
  ret += tmp;
  if (bendCoord)
  {
    ret += " bend ";
    tmp = bendCoord->getText();
    ret += tmp;
  }

  tmp = coord->getText();
  ret += tmp;
  return ret;
}

void XWTikzParabola::goToEnd()
{
  curPoint = coord;
}

bool XWTikzParabola::goToNext()
{
  if (curPoint == coord)
    return false;

  curPoint = coord;
  return true;
}

bool XWTikzParabola::goToPrevious()
{
  if (curPoint == bendCoord || !bendCoord)
    return false;

  curPoint = bendCoord;
  return true;
}

void XWTikzParabola::goToStart()
{
  if (bendCoord)
    curPoint = bendCoord;
  else
    curPoint = coord;
}

bool XWTikzParabola::hitTest(XWTikzState * state)
{
  curPoint = 0;
  if (bendCoord)
  {
    if (state->hitTestPoint(bendCoord))
    {
      curPoint = bendCoord;
      return true;
    }

    state->removeLastCoord();
  } 

  if (state->hitTestPoint(coord))
  {
    curPoint = coord;
    return true;
  }

  state->removeLastCoord();

  return state->hitTestParabola(bendCoord,coord);
}

void XWTikzParabola::scan(const QString & str, int & len, int & pos)
{
  options->scan(str,len,pos);
  while (str[pos].isSpace())
    pos++;
  if (str[pos] == QChar('b'))
  {
    pos += 4;
    while (str[pos].isSpace())
      pos++;

    bendCoord = new XWTikzCoord(graphic,this);
    bendCoord->scan(str,len,pos);
  }
  while (str[pos].isSpace())
    pos++;
  coord->scan(str,len,pos);
}

void XWTikzParabola::setBend(const QString & str)
{
  if (str.isEmpty())
  {
    if (bendCoord)
    {
      delete bendCoord;
      bendCoord = 0;
    }
  }
  else
    bendCoord = new XWTikzCoord(graphic,str,this);
}

void XWTikzParabola::setEnd(const QString & str)
{
  coord->setText(str);
}

void XWTikzParabola::setBend()
{
  XWTikzCoordDialog dlg(tr("bend point"));
  if (bendCoord)
  {    
    QString str = bendCoord->getText();
    dlg.setCoord(str);
    if (dlg.exec() == QDialog::Accepted)
    {
      str = dlg.getCoord();
      if (str.isEmpty())
      {
        delete bendCoord;
        bendCoord = 0;
      }
      else
      {
        XWTikzSetCoordinate * cmd = new XWTikzSetCoordinate(bendCoord,str);
        graphic->push(cmd);
      }      
    }
  }
  else
  {
    if (dlg.exec() == QDialog::Accepted)
    {
      QString str = dlg.getCoord();
      bendCoord = new XWTikzCoord(graphic,str);
    }
  }  
}

void XWTikzParabola::setEnd()
{
  coord->setCoord(tr("end point"));
}

XWTikzSine::XWTikzSine(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, PGFsin,parent)
{
  coord = new XWTikzCoord(graphicA,this);
}

bool XWTikzSine::addAction(QMenu & menu, XWTikzState *)
{
  QAction * a = menu.addAction(tr("end point"));
  connect(a, SIGNAL(triggered()), this, SLOT(setCoord()));
  return true;
}

void XWTikzSine::doCompute(XWTikzState * state)
{
  state->addSine(coord);
}

void XWTikzSine::doPath(XWTikzState * state, bool showpoint)
{
  state->addSine(coord);
  if (showpoint)
    coord->draw(state);
}

void XWTikzSine::dragTo(XWTikzState * state)
{
  if (!curPoint)
    return ;

  QPointF mp = state->getLastMousePoint();
  state->addSine(mp);
}

bool XWTikzSine::dropTo(XWTikzState * state)
{
  if (!curPoint)
    return false;
    
  return coord->dropTo(state);
}

QPointF XWTikzSine::getPoint(XWTikzState * state)
{
  return coord->getPoint(state);
}

QString XWTikzSine::getText()
{
  QString ret = " sin ";
  QString tmp = coord->getText();
  ret += tmp;
  return ret;
}

bool XWTikzSine::hitTest(XWTikzState * state)
{
  curPoint = 0;
  if (state->hitTestPoint(coord))
  {
    curPoint = coord;
    return true;
  }

  state->removeLastCoord();

  return state->hitTestSine(coord);
}

void XWTikzSine::scan(const QString & str, int & len, int & pos)
{
  while (str[pos].isSpace())
    pos++;
  coord->scan(str,len,pos);
}

void XWTikzSine::setCoord(const QString & str)
{
  coord->setText(str);
}

void XWTikzSine::setCoord()
{
  coord->setCoord(tr("end point"));
}

XWTikzCosine::XWTikzCosine(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, PGFcos,parent)
{
  coord = new XWTikzCoord(graphicA,this);
}

bool XWTikzCosine::addAction(QMenu & menu, XWTikzState *)
{
  QAction * a = menu.addAction(tr("end point"));
  connect(a, SIGNAL(triggered()), this, SLOT(setCoord()));
  return true;
}

void XWTikzCosine::doCompute(XWTikzState * state)
{
  state->addCosine(coord);
}

void XWTikzCosine::doPath(XWTikzState * state, bool showpoint)
{
  state->addCosine(coord);
  if (showpoint)
    coord->draw(state);
}

void XWTikzCosine::dragTo(XWTikzState * state)
{
  if (!curPoint)
    return ;

  QPointF mp = state->getLastMousePoint();
  state->addCosine(mp);
}

bool XWTikzCosine::dropTo(XWTikzState * state)
{
  if (!curPoint)
    return false;

  return coord->dropTo(state);
}

QPointF XWTikzCosine::getPoint(XWTikzState * state)
{
  return coord->getPoint(state);
}

QString XWTikzCosine::getText()
{
  QString ret = " cos ";
  QString tmp = coord->getText();
  ret += tmp;
  return ret;
}

bool XWTikzCosine::hitTest(XWTikzState * state)
{
  curPoint = 0;
  if (state->hitTestPoint(coord))
  {
    curPoint = coord;
    return true;
  }    

  state->removeLastCoord();

  return state->hitTestCosine(coord);
}

void XWTikzCosine::scan(const QString & str, int & len, int & pos)
{
  while (str[pos].isSpace())
    pos++;
  coord->scan(str,len,pos);
}

void XWTikzCosine::setCoord(const QString & str)
{
  coord->setText(str);
}

void XWTikzCosine::setCoord()
{
  coord->setCoord(tr("end point"));
}

XWTikzPlotCoordinates::XWTikzPlotCoordinates(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, PGFplot,parent),
isContinue(false),
cur(-1)
{
  options = new XWTIKZOptions(graphicA, this);
}

XWTikzPlotCoordinates::XWTikzPlotCoordinates(XWTikzGraphic * graphicA, bool c,XWTIKZOptions * opt,QObject * parent)
:XWTikzOperation(graphicA, PGFplot,parent),
isContinue(c),
options(opt),
cur(-1)
{
  options->setParent(this);
}

bool XWTikzPlotCoordinates::addAction(QMenu & menu, XWTikzState * state)
{
  options->doPath(state,false);
  options->addPlotAction(menu);
  return true;
}

void XWTikzPlotCoordinates::doCompute(XWTikzState * state)
{
  if (coords.size() <= 0)
    return ;

  options->doCompute(state);
  for (int i = 1; i < coords.size(); i++)
    coords[i]->doCompute(state);
}

void XWTikzPlotCoordinates::doPath(XWTikzState * state, bool showpoint)
{
  if (coords.size() <= 0)
    return ;
  
  options->doPath(state,showpoint);
  for (int i = 1; i < coords.size(); i++)
    state->addPlot(coords[i]);

  state->plotStreamEnd(isContinue);
}

bool XWTikzPlotCoordinates::dropTo(XWTikzState * state)
{
  if (curPoint)
    return curPoint->dropTo(state);

  return false;
}

QString XWTikzPlotCoordinates::getText()
{
  QString ret;
  if (isContinue)
    ret = " --plot";
  else
    ret = " plot";

  QString tmp = options->getText();
  ret += tmp;
  ret += " coordinates {";
  for (int i = 1; i < coords.size(); i++)
  {
    tmp = coords[i]->getText();
    ret += tmp;
    if (i < coords.size() - 1)
      ret += " ";
  }

  ret += "}";
  return ret;
}

void XWTikzPlotCoordinates::goToEnd()
{
  if (coords.size() <= 0)
    return ;
  cur = coords.size() - 1;
  curPoint = coords[cur];
}

bool XWTikzPlotCoordinates::goToNext()
{
  if (cur >= (coords.size() - 1))
    return false;

  cur++;
  curPoint = coords[cur];
  return true;
}

bool XWTikzPlotCoordinates::goToPrevious()
{
  if (cur <= 0)
    return false;

  cur--;
  curPoint = coords[cur];
  return true;
}

void XWTikzPlotCoordinates::goToStart()
{
  if (coords.size() > 0)
  {
    cur = 0;
    curPoint = coords[cur];
  }
}

bool XWTikzPlotCoordinates::hitTest(XWTikzState * state)
{
  curPoint = 0;
  cur = -1;
  for (int i = 0; i < coords.size(); i++)
  {
    if (state->hitTestPoint(coords[i]))
    {
      cur = i;
      curPoint = coords[i];
      return true;
    }
  }

  return false;
}

void XWTikzPlotCoordinates::scan(const QString & str, int & len, int & pos)
{
  while (str[pos].isSpace())
    pos++;

  if (str[pos] == QChar('{'))
    pos++;
  while (pos < len)
  {
    if (str[pos].isSpace())
      pos++;
    else if (str[pos] == QChar('}'))
    {
      pos++;
      return ;
    }
    else
    {
      XWTikzCoord * obj = new XWTikzCoord(graphic,this);
      coords << obj;
      obj->scan(str,len,pos);
    }
  }
}

void XWTikzPlotCoordinates::setCoords(const QString & str)
{
  while (!coords.isEmpty())
    delete coords.takeFirst();

  int len = str.length();
  int pos = 0;
  scan(str,len,pos);
}

XWTikzPlotFile::XWTikzPlotFile(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, PGFfile,parent),
isContinue(false)
{
  options = new XWTIKZOptions(graphicA, this);
}

XWTikzPlotFile::XWTikzPlotFile(XWTikzGraphic * graphicA, bool c,XWTIKZOptions * opt,QObject * parent)
:XWTikzOperation(graphicA, PGFfile,parent),
isContinue(c),
options(opt)
{
  options->setParent(this);
}

bool XWTikzPlotFile::addAction(QMenu & menu, XWTikzState * state)
{
  options->doPath(state,false);
  options->addPlotAction(menu);
  return true;
}

void XWTikzPlotFile::doCompute(XWTikzState * state)
{
  options->doCompute(state);
   XWDocSea sea;
  QString path = sea.findFile(fileName);
  if (path.isEmpty())
    return ;

  QFile file(path);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    return ;

  QTextStream stream(&file);
  QString line;
  QRegExp reg("\\s+");
  QString c;
  while (!stream.atEnd())
  {
    line = stream.readLine();
    QStringList list = line.split(reg);
    if (list.size() == 2)
      c = QString("(%1,%2)").arg(list[0]).arg(list[1]);
    else
      c = "(1,1)";
    
    XWTikzCoord * obj = new XWTikzCoord(graphic,c,state);
    obj->doCompute(state);
  }   
}

void XWTikzPlotFile::doPath(XWTikzState * state, bool showpoint)
{
  options->doPath(state,showpoint);

  XWDocSea sea;
  QString path = sea.findFile(fileName);
  if (path.isEmpty())
    return ;

  QFile file(path);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    return ;

  QTextStream stream(&file);
  QString line;
  QRegExp reg("\\s+");
  QString c;
  while (!stream.atEnd())
  {
    line = stream.readLine();
    QStringList list = line.split(reg);
    if (list.size() == 2)
      c = QString("(%1,%2)").arg(list[0]).arg(list[1]);
    else
      c = "(1,1)";
    
    XWTikzCoord * obj = new XWTikzCoord(graphic,c,state);
    state->addPlot(obj);
  }   

  state->plotStreamEnd(isContinue);
}

QString XWTikzPlotFile::getText()
{
  QString ret;
  if (isContinue)
    ret = " --plot";
  else
    ret = " plot";

  QString tmp = options->getText();
  ret += tmp;
  ret += " file{";
  ret += fileName;
  ret += "}";
  return ret;
}

void XWTikzPlotFile::scan(const QString & str, int & len, int & pos)
{
  while (str[pos].isSpace())
    pos++;
  pos++;
  int i = pos;
  while (pos < len)
  {
    if (str[pos] == QChar('}'))
      break;

    pos++;
  }

  fileName = str.mid(i,pos - i);
  fileName = fileName.trimmed();
  pos++;
}

void XWTikzPlotFile::setFileName(const QString & str)
{
  fileName = str;
}

XWTikzPlotFunction::XWTikzPlotFunction(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, PGFfunction,parent),
isContinue(false)
{
  options = new XWTIKZOptions(graphicA, this);
}

XWTikzPlotFunction::XWTikzPlotFunction(XWTikzGraphic * graphicA, bool c,XWTIKZOptions * opt, QObject * parent)
:XWTikzOperation(graphicA, PGFfunction,parent),
isContinue(c),
options(opt)
{
  options->setParent(this);
  coordExp = new XWTikzCoord(graphicA,this);
}

bool XWTikzPlotFunction::addAction(QMenu & menu, XWTikzState * state)
{
  options->doPath(state,false);
  options->addDomainAction(menu);
  QAction * a = menu.addAction(tr("function"));
  connect(a, SIGNAL(triggered()), this, SLOT(setExpress()));
  return true;
}

void XWTikzPlotFunction::doCompute(XWTikzState * state)
{
  options->doCompute(state);
  state->computePlotFunction(coordExp);
}

void XWTikzPlotFunction::doPath(XWTikzState * state, bool showpoint)
{
  options->doPath(state,showpoint);
  state->addPlotFunction(coordExp);
  state->plotStreamEnd(isContinue);
}

QString XWTikzPlotFunction::getText()
{
  QString ret;
  if (isContinue)
    ret = " --plot";
  else
    ret = " plot";

  QString tmp = options->getText();
  ret += tmp;

  tmp = coordExp->getText();
  ret += tmp;

  return ret;
}

bool XWTikzPlotFunction::hitTest(XWTikzState * state)
{
  if (options->hitTest(state))
    return true;
  return state->hitTestFunction(coordExp);
}

void XWTikzPlotFunction::scan(const QString & str, int & len, int & pos)
{
  coordExp->scan(str,len,pos);
}

void XWTikzPlotFunction::setDomain(const QString & s,const QString & e)
{
  options->addDomain(s,e);
}

void XWTikzPlotFunction::setExpress(const QString & str)
{
  coordExp->setText(str);
}

void XWTikzPlotFunction::setExpress()
{
  XWTikzCoordDialog dlg(tr("plot function"));
  QString str = coordExp->getText();
  dlg.setCoord(str);
  if (dlg.exec() == QDialog::Accepted)
  {
    str = dlg.getCoord();
    XWTikzSetCoordinate * cmd = new XWTikzSetCoordinate(coordExp,str);
    graphic->push(cmd);
  }
}

XWTikzTo::XWTikzTo(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, PGFto,parent),
 cur(-1),
 options(0),
 coord(0)
{
  options = new XWTIKZOptions(graphicA, this);
}

bool XWTikzTo::addAction(QMenu & menu, XWTikzState * state)
{
  options->doCompute(state);
  options->addToPathAction(menu);
  return true;
}

bool XWTikzTo::back(XWTikzState * state)
{
  if (cur < 0 || cur >= nodes.size())
    return false;

  return nodes[cur]->back(state);
}

bool XWTikzTo::cut(XWTikzState * state)
{
  if (cur < 0 || cur >= nodes.size())
    return false;

  return nodes[cur]->cut(state);
}

bool XWTikzTo::del(XWTikzState * state)
{
  if (cur < 0 || cur >= nodes.size())
    return false;

  return nodes[cur]->del(state);
}

void XWTikzTo::doCompute(XWTikzState * state)
{
  options->doCompute(state);
  for (int i = 0; i < nodes.size(); i++)
    nodes[i]->doCompute(state);
  if (coord)
    coord->doCompute(state);
}

void XWTikzTo::doPath(XWTikzState * state, bool showpoint)
{
  state = state->save(true);
  options->doPath(state,showpoint);
  for (int i = 0; i < nodes.size(); i++)
    nodes[i]->doPath(state,showpoint);

  if (coord)
  {
    state->toPath(coord);
    if (showpoint)
      coord->draw(state);
  }

  state = state->restore();
}

void XWTikzTo::dragTo(XWTikzState * state)
{
  if (curPoint)
  {
    QPointF mp = state->getLastMousePoint();
    state->lineTo(mp);
  }
  else
  {
    if (cur < 0 || cur >= nodes.size())
      return ;

    nodes[cur]->dragTo(state);
  }
}

bool XWTikzTo::dropTo(XWTikzState * state)
{
  if (curPoint)
    return coord->dropTo(state);

  if (cur < 0 || cur >= nodes.size())
      return false;

  return nodes[cur]->dropTo(state);
}

int XWTikzTo::getAnchorPosition()
{
  if (cur < 0 || cur >= nodes.size())
    return 0;

  return nodes[cur]->getAnchorPosition();
}

int XWTikzTo::getCursorPosition()
{
  if (cur < 0 || cur >= nodes.size())
    return 0;

  return nodes[cur]->getCursorPosition();
}

QString XWTikzTo::getCurrentText()
{
  if (cur < 0 || cur >= nodes.size())
    return QString();

  return nodes[cur]->getCurrentText();
}

QString XWTikzTo::getSelectedText()
{
  if (cur < 0 || cur >= nodes.size())
    return QString();

  return nodes[cur]->getSelectedText();
}

QString XWTikzTo::getText()
{
  QString ret = " to";
  QString tmp = options->getText();
  ret += tmp;
  for (int i = 0; i < nodes.size(); i++)
  {
    tmp = nodes[i]->getText();
    ret += tmp;    
  }

  if (coord)
  {
    ret += " ";
    tmp = coord->getText();
    ret += tmp;
  }
  else
    ret += " cycle";

  return ret;
}

void XWTikzTo::goToEnd()
{
  if (nodes.size() == 0)
    return ;

  cur = nodes.size() - 1;
  nodes[cur]->goToEnd();
}

bool XWTikzTo::goToNext()
{
  if (cur < 0 || cur > nodes.size() -1)
    return false;

  if (nodes[cur]->goToNext())
    return true;

  if (cur == nodes.size() -1)
    return false;

  cur++;
  nodes[cur]->goToStart();
  return true;
}

bool XWTikzTo::goToPrevious()
{
  if (cur < 0)
    return false;

  if (nodes[cur]->goToPrevious())
    return true;

  if (cur == 0)
    return false;

  cur--;
  nodes[cur]->goToEnd();
  return true;
}

void XWTikzTo::goToStart()
{
  if (nodes.size() <= 0)
    return ;

  cur = 0;
  nodes[cur]->goToStart();
}

bool XWTikzTo::hitTest(XWTikzState * state)
{
  curPoint = 0;
  bool ret = options->hitTest(state);
  if (!ret && coord)
    ret = coord->hitTest(state);

  cur = -1;
  if (!ret)
  {
    for (int i = 0; i < nodes.size(); i++)
    {
      ret = nodes[i]->hitTest(state);
      if (ret)
      {
        cur = i;
        break;
      }
    }
  }

  return ret;
}

bool XWTikzTo::insertText(XWTikzState * state)
{
  if (cur < 0 || cur >= nodes.size())
    return false;

  bool ret = options->insertText(state);
  if (!ret)
    ret = nodes[cur]->insertText(state);
  return ret;
}

bool XWTikzTo::keyInput(XWTikzState * state)
{
  if (cur < 0 || cur >= nodes.size())
    return false;

  bool ret = options->keyInput(state);
  if (!ret)
    ret = nodes[cur]->keyInput(state);
  return ret;
}

bool XWTikzTo::newPar(XWTikzState * state)
{
  if (cur < 0 || cur >= nodes.size())
    return false;

  bool ret = options->newPar(state);
  if (!ret)
    ret = nodes[cur]->newPar(state);
  return ret;
}

bool XWTikzTo::paste(XWTikzState * state)
{
  if (cur < 0 || cur >= nodes.size())
    return false;

  bool ret = options->paste(state);
  if (!ret)
    ret = nodes[cur]->paste(state);
  return ret;
}

void XWTikzTo::scan(const QString & str, int & len, int & pos)
{
  options->scan(str,len,pos);
  
  while (str[pos].isSpace())
    pos++;

  while (str[pos] == QChar('n'))
  {
    pos += 4;
    XWTikzNode * obj = new XWTikzNode(graphic,this);
    obj->scan(str,len,pos);
    nodes << obj;

    while (str[pos].isSpace())
      pos++;
  }

  if (str[pos] == QChar('('))
  {
    coord = new XWTikzCoord(graphic,this);
    coord->scan(str,len,pos);
  }
}

XWTikzDecorate::XWTikzDecorate(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, PGFdecorate,parent),
 cur(-1),
 options(0)
{
  options = new XWTIKZOptions(graphicA, this);
}

bool XWTikzDecorate::addAction(QMenu & menu, XWTikzState * state)
{
  options->doPath(state,false);
  options->addDecorationAction(menu);
  return true;
}

bool XWTikzDecorate::back(XWTikzState * state)
{
  if (cur < 0 || cur >= ops.size())
    return false;

  return ops[cur]->back(state);
}

bool XWTikzDecorate::cut(XWTikzState * state)
{
  if (cur < 0 || cur >= ops.size())
    return false;

  return ops[cur]->cut(state);
}

bool XWTikzDecorate::del(XWTikzState * state)
{
  if (cur < 0 || cur >= ops.size())
    return false;

  return ops[cur]->del(state);
}

void XWTikzDecorate::doPath(XWTikzState * state, bool showpoint)
{
  state = state->save(false);
  state->setDecorate(true);
  options->doPath(state,showpoint);
  for (int i = 0; i < ops.size(); i++)
    ops[i]->doPath(state,showpoint);
  state = state->restore();
}

void XWTikzDecorate::dragTo(XWTikzState * state)
{
  if (cur < 0 || cur >= ops.size())
    return ;

  ops[cur]->dragTo(state);
}

bool XWTikzDecorate::dropTo(XWTikzState * state)
{
  bool ret = options->dropTo(state);
  if (!ret)
  {
    for (int i = 0; i < ops.size(); i++)
    {
      ret = ops[i]->dropTo(state);
      if (ret)
      {
        cur = i;
        break;
      }
    }
  }
  return ret;
}

int XWTikzDecorate::getAnchorPosition()
{
  if (cur < 0 || cur >= ops.size())
    return 0;

  return ops[cur]->getAnchorPosition();
}

int XWTikzDecorate::getCursorPosition()
{
  if (cur < 0 || cur >= ops.size())
    return 0;

  return ops[cur]->getCursorPosition();
}

QString XWTikzDecorate::getCurrentText()
{
  if (cur < 0 || cur >= ops.size())
    return QString();

  return ops[cur]->getCurrentText();
}

QString XWTikzDecorate::getSelectedText()
{
  if (cur < 0 || cur >= ops.size())
    return QString();

  return ops[cur]->getSelectedText();
}

QString XWTikzDecorate::getText()
{
  QString ret = " decorate";
  QString tmp = options->getText();
  ret += tmp;
  ret += "{";
  for (int i = 0; i < ops.size(); i++)
  {
    tmp = ops[i]->getText();
    ret += tmp;    
  }

  ret += "}";
  return ret;
}

void XWTikzDecorate::goToEnd()
{
  if (ops.size() == 0)
    return ;

  cur = ops.size() - 1;
  ops[cur]->goToEnd();
}

bool XWTikzDecorate::goToNext()
{
  if (cur < 0 || cur > ops.size() -1)
    return false;

  if (ops[cur]->goToNext())
    return true;

  if (cur == ops.size() -1)
    return false;

  cur++;
  ops[cur]->goToStart();
  return true;
}

bool XWTikzDecorate::goToPrevious()
{
  if (cur < 0)
    return false;

  if (ops[cur]->goToPrevious())
    return true;

  if (cur == 0)
    return false;

  cur--;
  ops[cur]->goToEnd();
  return true;
}

void XWTikzDecorate::goToStart()
{
  if (ops.size() <= 0)
    return ;

  cur = 0;
  ops[cur]->goToStart();
}

bool XWTikzDecorate::hitTest(XWTikzState * state)
{
  bool ret = options->hitTest(state);
  if (!ret)
  {
    for (int i = 0; i < ops.size(); i++)
    {
      ret = ops[i]->hitTest(state);
      if (ret)
      {
        cur = i;
        break;
      }
    }
  }

  return ret;
}

bool XWTikzDecorate::insertText(XWTikzState * state)
{
  if (cur < 0 || cur >= ops.size())
    return false;

  bool ret = options->insertText(state);
  if (!ret)
    ret = ops[cur]->insertText(state);
  return ret;
}

bool XWTikzDecorate::keyInput(XWTikzState * state)
{
  if (cur < 0 || cur >= ops.size())
    return false;

  bool ret = options->keyInput(state);
  if (!ret)
    ret = ops[cur]->keyInput(state);
  return ret;
}

bool XWTikzDecorate::newPar(XWTikzState * state)
{
  if (cur < 0 || cur >= ops.size())
    return false;

  bool ret = options->newPar(state);
  if (!ret)
    ret = ops[cur]->newPar(state);
  return ret;
}

bool XWTikzDecorate::paste(XWTikzState * state)
{
  if (cur < 0 || cur >= ops.size())
    return false;

  bool ret = options->paste(state);
  if (!ret)
    ret = ops[cur]->paste(state);
  return ret;
}

void XWTikzDecorate::scan(const QString & str, int & len, int & pos)
{
  options->scan(str,len,pos);

  while (str[pos].isSpace())
    pos++;

  if (str[pos] == QChar('{'))
    pos++;

  XWTikzOperation * obj = 0;
  while (pos < len)
  {
    if (str[pos] == QChar(';'))
      break;
    else if (str[pos] == QChar('}'))
    {
      pos++;
      break;
    }
    else if (str[pos].isSpace())
      pos++;
    else if (str[pos] == QChar('%'))
      XWTeXBox::skipComment(str,len,pos);
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
          else
          {
            pos += 4;
            obj = new XWTikzCycle(graphic,this);
          }            
          break;

        case 'e':
          pos++;
          if (str[pos] == QChar('d'))
          {
            pos += 3;
            while (str[pos].isSpace())
              pos++;
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

        case 'r':
          pos += 9;
          obj = new XWTikzRectangle(graphic,this);
          break;

        case 't':
          pos += 2;
          obj = new XWTikzTo(graphic,this);
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

XWTikzForeach::XWTikzForeach(XWTikzGraphic * graphicA,XWTikzScope *scopeA,QObject * parent)
:XWTikzOperation(graphicA,PGFforeach,parent),
scope(scopeA),
cur(-1),
options(0)
{
  options = new XWTIKZOptions(graphicA,this);
}

bool XWTikzForeach::back(XWTikzState * state)
{
  if (cur < 0 || cur >= cmds.size())
    return false;

  return cmds[cur]->back(state);
}

bool XWTikzForeach::cut(XWTikzState * state)
{
  if (cur < 0 || cur >= cmds.size())
    return false;

  return cmds[cur]->cut(state);
}

bool XWTikzForeach::del(XWTikzState * state)
{
  if (cur < 0 || cur >= cmds.size())
    return false;

  return cmds[cur]->del(state);
}

void XWTikzForeach::doCopy(XWTikzState * state)
{
  options->doPath(state,false);  
  for (int i = 0; i < list.size(); i++)
  {
    QString tmp = list[i];
    QStringList vars = tmp.split(QChar('/'));
    for (int j = 0; j < vars.size(); j++)
    {
      QString var = vars[j];
      state->setVariables(variables[j],var);
    }

    for (int k = 0; k < cmds.size(); k++)
      cmds[k]->doCopy(state);
  }
}

void XWTikzForeach::doOperation(XWTikzState * state, bool showpoint)
{
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

    cmds[i]->doOperation(state,showpoint);
  }
}

void XWTikzForeach::doPath(XWTikzState * state, bool showpoint)
{
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

    cmds[i]->doPath(state,showpoint);
  }
}

void XWTikzForeach::doPathNoOptions(XWTikzState * state)
{
  if (cur < 0 || cur >= cmds.size())
    return;

  for (int i = 0; i < list.size(); i++)
  {
    QString tmp = list[i];
    QStringList vars = tmp.split(QChar('/'));
    for (int j = 0; j < vars.size(); j++)
    {
      QString var = vars[j];
      state->setVariables(variables[j],var);
    }

    cmds[cur]->doPath(state,false);
  }
}

bool XWTikzForeach::dropTo(XWTikzState * state)
{
  options->doCompute(state);
  bool ret = false;
  switch (graphic->getCurrentScope())
  {
    case XW_TIKZ_S_OPERATION:
    case XW_TIKZ_S_PATH:
      if (cur >= 0 && cur < cmds.size())
        ret = cmds[cur]->dropTo(state);
      break;

    default:
      for (int i = 0; i < cmds.size(); i++)
      {
        ret = cmds[i]->dropTo(state);
        if (ret)
          break;
      }
      break;
  }
  return ret;
}

int XWTikzForeach::getAnchorPosition()
{
  if (cur < 0 || cur >= cmds.size())
    return 0;

  return cmds[cur]->getAnchorPosition();
}

XWTikzOperation * XWTikzForeach::getCurrentOperation()
{
  if (cur < 0 || cur >= cmds.size())
    return 0;

  return cmds[cur]->getCurrentOperation();
}

XWTikzCoord * XWTikzForeach::getCurrentPoint()
{
  if (cur < 0 || cur >= cmds.size())
    return 0;

  return cmds[cur]->getCurrentPoint();
}

int XWTikzForeach::getCursorPosition()
{
  if (cur < 0 || cur >= cmds.size())
    return 0;

  return cmds[cur]->getCursorPosition();
}

QString XWTikzForeach::getCurrentText()
{
  if (cur < 0 || cur >= cmds.size())
    return QString();

  return cmds[cur]->getCurrentText();
}

QString XWTikzForeach::getSelectedText()
{
  if (cur < 0 || cur >= cmds.size())
    return QString();

  return cmds[cur]->getSelectedText();
}

QString XWTikzForeach::getText()
{
  QString ret = "foreach ";
  QString tmp = variables.join("/");
  ret += tmp;

  tmp = options->getText();
  ret += tmp;

  ret += " in {";
  tmp = list.join(",");
  ret += tmp;
  ret += "}\n";

  if (cmds.size() == 1)
  {
    tmp = cmds[0]->getText();
    ret += tmp;
  }
  else
  {
    ret += "  {\n";

    for (int i = 0; i < cmds.size(); i++)
    {
      tmp = cmds[i]->getText();
      ret += tmp;
      ret += "\n";
    }

    ret += "  }\n";
  }

  return ret;
}

QString XWTikzForeach::getTips(XWTikzState * state)
{
  QString ret;
  if (cur >= 0 && cur < cmds.size())
  {
    options->doCompute(state);
    ret = cmds[cur]->getTips(state);
  }

  return ret;
}

void XWTikzForeach::goToEnd()
{
  if (cmds.size() <= 0)
    return ;

  cur = cmds.size() - 1;
  cmds[cur]->goToEnd();
}

bool XWTikzForeach::goToNext()
{
  if (cur >= (cmds.size() - 1))
    return false;

  cur++;
  cmds[cur]->goToStart();
  return true;
}

bool XWTikzForeach::goToNextOperation()
{
  if (cur < 0 || cur >= cmds.size())
    return false;

  if (!cmds[cur]->goToNext())
    return goToNext();

  return true;
}

bool XWTikzForeach::goToNextPath()
{
  if (cur >= (cmds.size() - 1))
    return false;

  cur++;
  cmds[cur]->goToStart();
  return true;
}

void XWTikzForeach::goToOperationEnd()
{
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->goToOperationEnd();
}

void XWTikzForeach::goToOperationStart()
{
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->goToOperationStart();
}

void XWTikzForeach::goToPathEnd()
{
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->goToPathEnd();
}

void XWTikzForeach::goToPathStart()
{
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->goToPathStart();
}

bool XWTikzForeach::goToPrevious()
{
  if (cur <= 0)
    return false;

  cur--;
  cmds[cur]->goToEnd();
  return true;
}

bool XWTikzForeach::goToPreviousOperation()
{
  if (cur < 0 || cur >= cmds.size())
    return false;

  if (!cmds[cur]->goToPrevious())
    return goToPrevious();

  return true;
}

bool XWTikzForeach::goToPreviousPath()
{
  if (cur <= 0)
    return false;

  cur--;
  cmds[cur]->goToEnd();
  return true;
}

void XWTikzForeach::goToStart()
{
  if (cmds.size() <= 0)
    return ;

  cur = 0;
  cmds[cur]->goToStart();
}

bool XWTikzForeach::hitTest(XWTikzState * state)
{
  options->doCompute(state);
  bool ret = false;
  switch (graphic->getCurrentScope())
  {
    case XW_TIKZ_S_OPERATION:
    case XW_TIKZ_S_PATH:
      if (cur >= 0 && cur < cmds.size())
        ret = cmds[cur]->hitTest(state);
      break;

    default:
      cur = -1;
      for (int i = 0; i < cmds.size(); i++)
      {
        ret = cmds[i]->hitTest(state);
        if (ret)
        {
          cur = i;
          break;
        }
      }
      break;
  }
  return ret;
}

bool XWTikzForeach::insertText(XWTikzState * state)
{
  if (cur < 0 || cur >= cmds.size())
    return false;

  options->doCompute(state);
  return cmds[cur]->insertText(state);
}

bool XWTikzForeach::keyInput(XWTikzState * state)
{
  if (cur < 0 || cur >= cmds.size())
    return false;

  options->doCompute(state);
  return cmds[cur]->keyInput(state);
}

bool XWTikzForeach::newPar(XWTikzState * state)
{
  if (cur < 0 || cur >= cmds.size())
    return false;

  options->doCompute(state);
  return cmds[cur]->newPar(state);
}

bool XWTikzForeach::paste(XWTikzState * state)
{
  if (cur < 0 || cur >= cmds.size())
    return false;

  options->doCompute(state);
  return cmds[cur]->paste(state);
}

void XWTikzForeach::scan(const QString & str, int & len, int & pos)
{
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
    else if (str[pos] == QChar('['))
      options->scan(str,len,pos);
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

  while (!str[pos].isSpace())
    pos++;

  if (str[pos] == QChar('{'))
  {
    pos++;
    while (pos < len)
    {
      if (str[pos] == QChar('}'))
      {
        pos++;
        break;
      }
      else if (str[pos].isSpace())
        pos++;
      else
      {
        QString key = XWTeXBox::scanControlSequence(str,len,pos);
        int id = lookupPGFID(key);
        if (id == PGFbegin)
        {
          key = XWTeXBox::scanEnviromentName(str,len,pos);
          id = lookupPGFID(key);
        }
        XWTikzCommand * cmd = createPGFObject(graphic,scope,id,this);
        cmds << cmd;
        cmd->scan(str,len,pos);
      }
    }
  }
  else
  {
    QString key = XWTeXBox::scanControlSequence(str,len,pos);
    int id = lookupPGFID(key);
    if (id == PGFbegin)
    {
      key = XWTeXBox::scanEnviromentName(str,len,pos);
      id = lookupPGFID(key);
    }
    XWTikzCommand * cmd = createPGFObject(graphic,scope,id,this);
    cmds << cmd;
    cmd->scan(str,len,pos);
  }  
}
