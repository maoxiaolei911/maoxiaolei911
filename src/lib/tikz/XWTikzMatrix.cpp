/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include "PGFKeyWord.h"
#include "XWPGFPool.h"
#include "XWTikzGraphic.h"
#include "XWTikzState.h"
#include "XWTikzCommand.h"
#include "XWTikzMatrix.h"

XWTikzCell::XWTikzCell(XWTikzGraphic * graphicA, QObject * parent)
:QObject(parent),
 graphic(graphicA),
 cur(-1),
 width(0),
 state(0)
{
  state = new XWTikzState(false,this);
}

bool XWTikzCell::back()
{
  if (cur < 0 || cur >= cmds.size())
    return false;

  return cmds[cur]->back(state);
}

bool XWTikzCell::cut()
{
  if (cur < 0 || cur >= cmds.size())
    return false;

  return cmds[cur]->cut(state);
}

bool XWTikzCell::del()
{
  if (cur < 0 || cur >= cmds.size())
    return false;

  return cmds[cur]->del(state);
}

void XWTikzCell::doPath(XWTikzState * stateA, int align, double maxw)
{
  double x = 0;
  double x1 = 0;
  switch (align)
  {
    default:
      x1 = maxw;
      break;

    case 1:
      x = 0.5 * (maxw - width);
      x1 = width + x;
      break;

    case 2:
      x = maxw - width;
      break;
  }

  stateA->shift(x,0);
  stateA->copy(state);

  for (int i = 0; i < cmds.size(); i++)
    cmds[i]->doPath(state,false);

  stateA->shift(x1,0);
}

void XWTikzCell::dragTo(XWTikzState * stateA)
{
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->dragTo(stateA);
}

bool XWTikzCell::dropTo(XWTikzState * stateA)
{
  if (cur < 0 || cur >= cmds.size())
    return false;

  return cmds[cur]->dropTo(stateA);
}

int XWTikzCell::getAnchorPosition()
{
  if (cur < 0 || cur >= cmds.size())
    return 0;

  return cmds[cur]->getAnchorPosition();
}

int XWTikzCell::getCursorPosition()
{
  if (cur < 0 || cur >= cmds.size())
    return 0;

  return cmds[cur]->getCursorPosition();
}

QString XWTikzCell::getCurrentText()
{
  if (cur < 0 || cur >= cmds.size())
    return QString();

  return cmds[cur]->getCurrentText();
}

QString XWTikzCell::getSelectedText()
{
  if (cur < 0 || cur >= cmds.size())
    return QString();

  return cmds[cur]->getSelectedText();
}

QString XWTikzCell::getText()
{
  QString ret;
  for (int i = 0; i < cmds.size(); i++)
  {
    QString tmp = cmds[i]->getText();
    ret += tmp;
  }

  return ret;
}

void XWTikzCell::getWidthAndHeight(double & w, double & h)
{
  w = 0;
  h = 0;
  double tmpw = 0;
  double tmph = 0;
  for (int i = 0; i < cmds.size(); i++)
  {
    cmds[i]->getWidthAndHeight(tmpw,tmph);
    if (tmpw > w)
      w = tmpw;

    if (tmph > h)
      h = tmph;
  }

  width = w;
}

void XWTikzCell::goToEnd()
{
  if (cmds.size() == 0)
    return ;

  cur = cmds.size() - 1;
  cmds[cur]->goToEnd();
}

bool XWTikzCell::goToNext()
{
  if (cur < 0 || cur >= cmds.size())
    return false;

  if (cmds[cur]->goToNext())
    return true;

  if (cur == cmds.size() -1)
    return false;

  cur++;
  cmds[cur]->goToStart();
  return true;
}

bool XWTikzCell::goToPrevious()
{
  if (cur < 0 || cur >= cmds.size())
    return false;

  if (cmds[cur]->goToPrevious())
    return true;

  if (cur == 0)
    return false;

  cur--;
  cmds[cur]->goToEnd();
  return true;
}

void XWTikzCell::goToStart()
{
  if (cmds.size() == 0)
    return ;

  cur = 0;
  cmds[cur]->goToStart();
}

bool XWTikzCell::hitTest(double x, double y)
{
  QPointF mp(x,y);
  state->setMousePoint(mp);
  cur = -1;
  for (int i = 0; i < cmds.size(); i++)
  {
    if (cmds[i]->hitTest(state))
    {
      cur = i;
      return true;
    }
  }

  return false;
}

bool XWTikzCell::insertText(const QString & str)
{
  if (cur < 0 || cur >= cmds.size())
    return false;
  state->setText(str);
  return cmds[cur]->insertText(state);
}

bool XWTikzCell::keyInput(const QString & str)
{
  if (cur < 0 || cur >= cmds.size())
    return false;
  state->setText(str);
  return cmds[cur]->keyInput(state);
}

bool XWTikzCell::newPar()
{
  if (cur < 0 || cur >= cmds.size())
    return false;

  return cmds[cur]->newPar(state);
}

bool XWTikzCell::paste()
{
  if (cur < 0 || cur >= cmds.size())
    return false;

  return cmds[cur]->paste(state);
}

void XWTikzCell::scan(const QString & str, int & len, int & pos)
{
  while (pos < len)
  {
    if (str[pos] == QChar('&'))
    {
      pos++;
      break;
    }
    else if (str[pos] == QChar('%'))
      XWTeXBox::skipComment(str,len,pos);
    else if (str[pos].isSpace())
      pos++;
    else
    {
      if (str[pos] == QChar('\\'))
      {
        if (str[pos+1] == QChar('\\'))
        {
          pos += 2;
          break;
        }

        QString key = XWTeXBox::scanControlSequence(str,len,pos);
        int id = lookupPGFID(key);
        if (id == PGFbegin)
        {
          key = XWTeXBox::scanEnviromentName(str,len,pos);
          id = lookupPGFID(key);
        }

        XWTikzCommand * cmd = createPGFObject(graphic,0,id,this);
        cmds << cmd;
        cmd->scan(str,len,pos);
      }
    }
  }
}

XWTikzRow::XWTikzRow(XWTikzGraphic * graphicA, QObject * parent)
:QObject(parent),
 graphic(graphicA),
 cur(-1)
{}

bool XWTikzRow::back()
{
  if (cur < 0 || cur >= columns.size())
    return false;

  return columns[cur]->back();
}

bool XWTikzRow::cut()
{
  if (cur < 0 || cur >= columns.size())
    return false;

  return columns[cur]->cut();
}

bool XWTikzRow::del()
{
  if (cur < 0 || cur >= columns.size())
    return false;

  return columns[cur]->del();
}

void XWTikzRow::doPath(XWTikzState * state, int align, const QList<double> & wlist)
{
  XWTikzState mystate;
  state->copy(&mystate);
  for (int i = 0; i < columns.size(); i++)
  {
    columns[i]->doPath(&mystate,align,wlist[i]);
    mystate.transformColumn();
  }
}

void XWTikzRow::dragTo(XWTikzState * state)
{
  if (cur < 0 || cur >= columns.size())
    return ;

  columns[cur]->dragTo(state);
}

bool XWTikzRow::dropTo(XWTikzState * state)
{
  if (cur < 0 || cur >= columns.size())
    return false;

  return columns[cur]->dropTo(state);
}

int XWTikzRow::getAnchorPosition()
{
  if (cur < 0 || cur >= columns.size())
    return 0;

  return columns[cur]->getAnchorPosition();
}

int XWTikzRow::getCursorPosition()
{
  if (cur < 0 || cur >= columns.size())
    return 0;

  return columns[cur]->getCursorPosition();
}

QString XWTikzRow::getCurrentText()
{
  if (cur < 0 || cur >= columns.size())
    return QString();

  return columns[cur]->getCurrentText();
}

void XWTikzRow::getMaxWidthAndHeight(QList<double> & wlist,double & rh)
{
  double w = 0;
  double h = 0;
  rh = 0;
  for (int i = 0; i < columns.size(); i++)
  {
    columns[i]->getWidthAndHeight(w,h);
    if (w > wlist[i])
      wlist[i] = w;

    if (h > rh)
      rh = h;
  }
}

double XWTikzRow::getNumberOfColumns()
{
  return columns.size();
}

QString XWTikzRow::getSelectedText()
{
  if (cur < 0 || cur >= columns.size())
    return QString();

  return columns[cur]->getSelectedText();
}

QString XWTikzRow::getText()
{
  QString ret;
  for (int i = 0; i < columns.size(); i++)
  {
    QString tmp = columns[i]->getText();
    ret += tmp;
    if (i < columns.size() - 1)
      ret += " & ";
  }

  ret += "\\\\\n";
  return ret;
}

void XWTikzRow::goToEnd()
{
  if (columns.size() == 0)
    return ;

  cur = columns.size() - 1;
  columns[cur]->goToEnd();
}

bool XWTikzRow::goToNext()
{
  if (cur < 0 || cur >= columns.size())
    return false;

  if (columns[cur]->goToNext())
    return true;

  if (cur == columns.size() -1)
    return false;

  cur++;
  columns[cur]->goToStart();
  return true;
}

bool XWTikzRow::goToPrevious()
{
  if (cur < 0 || cur >= columns.size())
    return false;

  if (columns[cur]->goToPrevious())
    return true;

  if (cur == 0)
    return false;

  cur--;
  columns[cur]->goToEnd();
  return true;
}

void XWTikzRow::goToStart()
{
  if (columns.size() == 0)
    return ;

  cur = 0;
  columns[cur]->goToStart();
}

bool XWTikzRow::hitTest(double x, double y)
{
  cur = -1;
  for (int i = 0; i < columns.size(); i++)
  {
    if (columns[i]->hitTest(x,y))
    {
      cur = -1;
      return true;
    }
  }

  return false;
}

bool XWTikzRow::insertText(const QString & str)
{
  if (cur < 0 || cur >= columns.size())
    return false;

  return columns[cur]->insertText(str);
}

bool XWTikzRow::keyInput(const QString & str)
{
  if (cur < 0 || cur >= columns.size())
    return false;

  return columns[cur]->keyInput(str);
}

bool XWTikzRow::newPar()
{
  if (cur < 0 || cur >= columns.size())
    return false;

  return columns[cur]->newPar();
}

bool XWTikzRow::paste()
{
  if (cur < 0 || cur >= columns.size())
    return false;

  return columns[cur]->paste();
}

void XWTikzRow::scan(const QString & str, int & len, int & pos)
{
  while (pos < len)
  {
    if (str[pos] == QChar('}'))
      break;
    else if (str[pos] == QChar('%'))
      XWTeXBox::skipComment(str,len,pos);
    else if (str[pos].isSpace())
      pos++;
    else
    {
      XWTikzCell * col = new XWTikzCell(graphic,this);
      columns << col;
      col->scan(str,len,pos);
    }
  }
}

XWTikzMatrix::XWTikzMatrix(XWTikzGraphic * graphicA, QObject * parent)
:XWTeXBox(parent),
graphic(graphicA),
state(0),
cur(-1),
align(0),
width(0),
height(0),
depth(0)
{}

bool XWTikzMatrix::back()
{
  if (cur < 0 || cur >= rows.size())
    return false;

  return rows[cur]->back();
}

bool XWTikzMatrix::cut()
{
  if (cur < 0 || cur >= rows.size())
    return false;

  return rows[cur]->cut();
}

bool XWTikzMatrix::del()
{
  if (cur < 0 || cur >= rows.size())
    return false;

  return rows[cur]->del();
}

void XWTikzMatrix::doPath(XWTikzState * stateA, bool showpoint)
{
  if (!state)
    new XWTikzState(false,this);

  stateA->copy(state);
  doRows(stateA,showpoint);
}

void XWTikzMatrix::doContent(XWPDFDriver * )
{
  XWTikzState mystate;
  doRows(&mystate,false);
}

void XWTikzMatrix::dragTo(XWPDFDriver * , double xA, double yA)
{
  if (cur < 0 || cur >= rows.size())
    return ;

  QPointF mp(xA,yA);
  state->setLastMousePoint(mp);
  rows[cur]->dragTo(state);
}

void XWTikzMatrix::dragTo(XWTikzState * state)
{
  if (cur < 0 || cur >= rows.size())
    return ;

  rows[cur]->dragTo(state);
}

bool XWTikzMatrix::dropTo(XWTikzState * state)
{
  if (cur < 0 || cur >= rows.size())
    return false;

  return rows[cur]->dropTo(state);
}

int XWTikzMatrix::getAnchorPosition()
{
  if (cur < 0 || cur >= rows.size())
    return 0;

  return rows[cur]->getAnchorPosition();
}

int XWTikzMatrix::getCursorPosition()
{
  if (cur < 0 || cur >= rows.size())
    return 0;

  return rows[cur]->getCursorPosition();
}

QString XWTikzMatrix::getCurrentText()
{
  if (cur < 0 || cur >= rows.size())
    return QString();

  return rows[cur]->getCurrentText();
}

double XWTikzMatrix::getDepth(double )
{
  return depth;
}

double XWTikzMatrix::getHeight(double )
{
  if (height <= 0)
    getDimension();

  return height;
}

QString XWTikzMatrix::getSelectedText()
{
  if (cur < 0 || cur >= rows.size())
    return QString();

  return rows[cur]->getSelectedText();
}

QString XWTikzMatrix::getText()
{
  QString m;
  for (int i = 0; i < rows.size(); i++)
  {
    QString tmp = rows[i]->getText();
    m += tmp;
  }

  QString ret = QString("{\n%1}\n").arg(m);
  return ret;
}

double  XWTikzMatrix::getWidth(double )
{
  if (width <= 0)
    getDimension();

  return width;
}

void XWTikzMatrix::goToEnd()
{
  if (rows.size() == 0)
    return ;

  cur = rows.size() - 1;
  rows[cur]->goToEnd();
}

bool XWTikzMatrix::goToNext()
{
  if (cur < 0 || cur >= rows.size())
    return false;

  if (rows[cur]->goToNext())
    return true;

  if (cur == rows.size() -1)
    return false;

  cur++;
  rows[cur]->goToStart();
  return true;
}

bool XWTikzMatrix::goToPrevious()
{
  if (cur < 0 || cur >= rows.size())
    return false;

  if (rows[cur]->goToPrevious())
    return true;

  if (cur == 0)
    return false;

  cur--;
  rows[cur]->goToEnd();
  return true;
}

void XWTikzMatrix::goToStart()
{
  if (rows.size() == 0)
    return ;

  cur = 0;
  rows[cur]->goToStart();
}

bool XWTikzMatrix::hitTest(double x, double y)
{
  cur = -1;
  for (int i = 0; i < rows.size(); i++)
  {
    if (rows[i]->hitTest(x,y))
    {
      cur = i;
      return true;
    }
  }

  return false;
}

bool XWTikzMatrix::insertText(const QString & str)
{
  if (cur < 0 || cur >= rows.size())
    return false;

  return rows[cur]->insertText(str);
}

bool XWTikzMatrix::keyInput(const QString & str)
{
  if (cur < 0 || cur >= rows.size())
    return false;

  return rows[cur]->keyInput(str);
}

bool XWTikzMatrix::newPar()
{
  if (cur < 0 || cur >= rows.size())
    return false;

  return rows[cur]->newPar();
}

bool XWTikzMatrix::paste()
{
  if (cur < 0 || cur >= rows.size())
    return false;

  return rows[cur]->paste();
}

void XWTikzMatrix::scan(const QString & str, int & len, int & pos)
{
  while (pos < len)
  {
    if (str[pos] == QChar('}'))
    {
      pos++;
      break;
    }
    else if (str[pos] == QChar('%'))
      XWTeXBox::skipComment(str,len,pos);
    else if (str[pos].isSpace())
      pos++;
    else
    {
      XWTikzRow * r = new XWTikzRow(graphic,this);
      rows << r;
      r->scan(str,len,pos);
    }
  }
}

void XWTikzMatrix::setState(XWTikzState * stateA)
{
  if (!state)
    new XWTikzState(false,this);

  stateA->copy(state);
}

void XWTikzMatrix::setXY(int alignA, double ,double )
{
  align = alignA;
}

void XWTikzMatrix::updateCursor(double minx, double miny, double maxx,
                    double maxy, int )
{
  QPointF p1(minx,miny);
  QPointF p2(maxx,maxy);
  if (state)
  {
    p1 = state->map(p1);
    p2 = state->map(p2);
  }
  graphic->updateCursor(p1.x(),p1.y(),p2.x(),p2.y());
}

void XWTikzMatrix::doRows(XWTikzState * stateA, bool )
{
  width = 0;
  height = 0;
  if (rows.size() == 0)
    return ;

  int numberofcolumns = rows[0]->getNumberOfColumns();
  QList<double> maxcolumnwidths;
  for (int i = 0; i < numberofcolumns; i++)
    maxcolumnwidths << 0;

  QList<double> maxrowheights;
  double h = 0;
  for (int i = 0; i < rows.size(); i++)
  {
    rows[i]->getMaxWidthAndHeight(maxcolumnwidths,h);
    maxrowheights << h;
  }

  for (int i = 0; i < rows.size(); i++)
  {
    rows[i]->doPath(stateA,align,maxcolumnwidths);
    stateA->transformRow(maxrowheights[i]);
  }
}

void XWTikzMatrix::getDimension()
{
  if (rows.size() == 0)
    return ;

  int numberofcolumns = rows[0]->getNumberOfColumns();
  QList<double> maxcolumnwidths;
  for (int i = 0; i < numberofcolumns; i++)
    maxcolumnwidths << 0;

  QList<double> maxrowheights;
  double h = 0;
  for (int i = 0; i < rows.size(); i++)
  {
    rows[i]->getMaxWidthAndHeight(maxcolumnwidths,h);
    maxrowheights << h;
  }

  for (int i = 0; i < numberofcolumns; i++)
    width += maxcolumnwidths[i];

  for (int i = 0; i < rows.size(); i++)
    height += maxrowheights[i];
}
