/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QPointF>
#include <QVector2D>
#include <QList>
#include <math.h>
#include "PGFKeyWord.h"
#include "XWPDFDriver.h"
#include "XWTikzState.h"
#include "XWTikzTextBox.h"
#include "XWTikzShape.h"

void XWTikzShape::circleSolidusShape(bool dopath)
{
  double x = 0.5 * (boxWidth + boxHeight + boxDepth) + 0.3536 * state->lineWidth + state->innerXSep;
  double y = -x + boxHeight - 0.3536 * state->lineWidth - state->outerYSep;
  centerPoint.setX(x);
  centerPoint.setY(y);
  double wd = 0;
  double ht = 0;
  double dp = 0;
  if (box)
  {
    wd = box->getWidth(state->textWidth, 1);
    ht = box->getHeight(state->textWidth,1);
    dp = box->getDepth(state->textWidth,1);
  }
  
  x = 0.5 * (boxWidth + boxHeight + boxDepth);
  y = -x;
  x += 0.5 * (ht + dp - wd) + 0.7071 * state->lineWidth + 2 * state->innerXSep;
  y += 0.5 * (dp - ht - wd) - 0.7071 * state->lineWidth - 2 * state->outerYSep;
  lowerPoint.setX(x);
  lowerPoint.setY(y);
  
  if (box)
  {
    ht = box->getHeight(state->textWidth,1);
    dp = box->getDepth(state->textWidth,1);
    wd = box->getWidth(state->textWidth,1);
  }
  x = boxWidth + boxHeight + boxDepth;
  y = ht + dp + wd;
  if (x < y)
    x = y;
  x *= 0.7071;
  x += 0.5 * state->lineWidth;
  double xa = state->innerXSep;
  double ya = state->innerYSep;
  x += state->veclen(2 * xa, y * ya);
  xa = state->minWidth;
  ya = state->minHeight;
  if (x < xa)
    x = xa;
  if (x < ya)
    x = ya;
  xa = state->outerXSep;
  ya = state->outerYSep;
  if (xa < ya)
    x += ya;
  else
    x += xa;
  radius = x;
  centerPos = centerPoint;
  westPos.setX(x - radius - state->left);
  westPos.setY(y);
  eastPos.setX(x + radius + state->right);
  eastPos.setY(y);
  northPos.setX(x);
  northPos.setY(y + radius + state->above);
  southPos.setX(x);
  southPos.setY(y - radius - state->below);
  switch (state->anchor)
  {
    default:
      {
        double xa = state->anotherPoint.x();
        double ya = state->anotherPoint.y();
        QPointF pd(xa,ya);
        QPointF ur(radius,radius);

        QPointF p = pointBorderEllipse(pd,ur);
        x = p.x() + centerPoint.x();
        y = p.y() + centerPoint.y();
      }
      break;

    case PGFtext:
      x = 0;
      y = 0;
      break;

    case PGFlower:
      x = lowerPoint.x();
      y = lowerPoint.y();
      break;

    case PGFmid:
      y = 0.5 * 4.3;
      break;

    case PGFbase:
      y = 0;
      break;

    case PGFnorth:
    case PGFabove:
      y = northPos.y();
      break;

    case PGFsouth:
    case PGFbelow:
      y = southPos.y();
      break;

    case PGFwest:
    case PGFleft:
      x = westPos.x();
      break;

    case PGFeast:
    case PGFright:
      x = eastPos.x();
      break;

    case PGFmidwest:
      x = x - radius;
      y = 0.5 * 4.3;
      break;

    case PGFmideast:
      x = x + radius;
      y = 0.5 * 4.3;
      break;

    case PGFbasewest:
      x = x - radius;
      y = 0;
      break;

    case PGFbaseeast:
      x = x + radius;
      y = 0;
      break;

    case PGFnorthwest:
    case PGFbelowright:
      x = x - 0.707107 * radius;
      y = y + 0.707107 * radius;
      break;

    case PGFsouthwest:
    case PGFaboveright:
      x = x - 0.707107 * radius;
      y = y - 0.707107 * radius;
      break;

    case PGFnortheast:
    case PGFbelowleft:
      x = x + 0.707107 * radius;
      y = y + 0.707107 * radius;
      break;

    case PGFsoutheast:
    case PGFaboveleft:
      x = x + 0.707107 * radius;
      y = y - 0.707107 * radius;
      break;
  }

  anchorPos.setX(x);
  anchorPos.setY(y);

  if (dopath)
  {
    state = state->save(false);
    if (state->outerXSep < state->outerYSep)
      radius = radius - state->outerYSep;
    else
      radius = radius - state->outerXSep;
    state->moveTo(centerPoint);
    state->addCircle(centerPoint,radius);  
    state = state->restore();
    
    double tempdima = radius;
    double xb = state->outerXSep;
    double yb = state->outerYSep;
    if (xb < yb)
      tempdima -= yb;
    else
      tempdima -= xb;
    tempdima -= 0.5 * state->lineWidth;
    x = centerPoint.x() - 0.437 * tempdima;
    y = centerPoint.y() - 0.437 * tempdima;
    state = state->save(false);
    state->moveTo(x,y);
    x = centerPoint.x() + 0.437 * tempdima;
    y = centerPoint.y() + 0.437 * tempdima;
    state->lineTo(x,y);
    state->setDraw(true);
    state = state->restore();

    if (state->isFillText)
      driver->fillText();

    QTransform transA = state->transform;
    if (box->isTextBox())
      driver->setTextMatrix(transA.m11(),transA.m12(),transA.m21(),transA.m22(),transA.dx(),transA.dy());
    else
      driver->setMatrix(transA.m11(),transA.m12(),transA.m21(),transA.m22(),transA.dx(),transA.dy());

    box->setXY(state->align, 0, 0, 0);
    box->setXY(state->align, lowerPoint.x(), lowerPoint.y(), 1);

    box->doContent(driver); 
  }
}

void XWTikzShape::circleSplitShape(bool dopath)
{
  double x = 0.5 * boxWidth;
  double y = -state->innerYSep - boxDepth - 0.5 * state->lineWidth;
  centerPoint.setX(x);
  centerPoint.setY(y);
  double wd = 0;
  double ht = 0;
  if (box)
  {
    wd = box->getWidth(state->textWidth, 1);
    ht = box->getHeight(state->textWidth,1);
  }
  
  x = -0.5 * wd + 0.5 * boxWidth;
  y = -2 * state->innerYSep - 0.5 * ht - state->lineWidth - boxDepth;
  lowerPoint.setX(x);
  lowerPoint.setY(y);
  
  double dp = 0;
  if (box)
  {
    ht = box->getHeight(state->textWidth,1);
    dp = box->getDepth(state->textWidth,1);
  }
  double ya = 0.5 * (boxHeight + boxDepth);
  double yb = 0.5 * (ht + dp);
  if (ya > yb)
    ya *= 2;
  else
    ya = 2 * yb;
  ya += 0.5 * state->lineWidth;
  yb = state->innerYSep;
  ya += 2 * yb;
  
  if (box)
    wd = box->getWidth(state->textWidth,1);
  double xa = 0.5 * boxWidth;
  if (xa < 0.5 * wd)
    xa = 0.5 * wd;
  xa += state->innerXSep;
  QVector2D v(xa,ya);
  v.normalize();
  x = v.x();
  y = v.y();
  if (x > y)
  {
    int counta = (int)x;
    if (counta != 0)
    {
      counta /= 255;
      xa *= 16;
      xa /= counta;
      xa *= 16;
    }
    else
    {
      counta = (int)y;
      if (counta != 0)
      {
        counta /= 255;
        ya *= 16;
        ya /= counta;
        xa = 16 * ya;
      }
    }
  }
  x = xa;
  double xb = state->minWidth;
  yb = state->minHeight;
  if (x < 0.5 * xb)
    x = 0.5 * xb;
  if (x < 0.5 * yb)
    x = 0.5 * yb;
  xb = state->outerXSep;
  yb = state->outerYSep;
  if (xb < yb)
    x += yb;
  else
    x += xb;
  radius = x;

  switch (state->anchor)
  {
    default:
      {
        double xa = state->anotherPoint.x();
        double ya = state->anotherPoint.y();
        QPointF pd(xa,ya);
        QPointF ur(radius,radius);

        QPointF p = pointBorderEllipse(pd,ur);
        x = p.x() + centerPoint.x();
        y = p.y() + centerPoint.y();
      }
      break;

    case PGFtext:
      x = 0;
      y = 0;
      break;

    case PGFlower:
      x = lowerPoint.x();
      y = lowerPoint.y();
      break;

    case PGFmid:
      y = 0.5 * 4.3;
      break;

    case PGFbase:
      y = 0;
      break;

    case PGFnorth:
    case PGFabove:
      y = northPos.y();
      break;

    case PGFsouth:
    case PGFbelow:
      y = southPos.y();
      break;

    case PGFwest:
    case PGFleft:
      x = westPos.x();
      break;

    case PGFeast:
    case PGFright:
      x = eastPos.x();
      break;

    case PGFmidwest:
      x = x - radius;
      y = 0.5 * 4.3;
      break;

    case PGFmideast:
      x = x + radius;
      y = 0.5 * 4.3;
      break;

    case PGFbasewest:
      x = x - radius;
      y = 0;
      break;

    case PGFbaseeast:
      x = x + radius;
      y = 0;
      break;

    case PGFnorthwest:
    case PGFbelowright:
      x = x - 0.707107 * radius;
      y = y + 0.707107 * radius;
      break;

    case PGFsouthwest:
    case PGFaboveright:
      x = x - 0.707107 * radius;
      y = y - 0.707107 * radius;
      break;

    case PGFnortheast:
    case PGFbelowleft:
      x = x + 0.707107 * radius;
      y = y + 0.707107 * radius;
      break;

    case PGFsoutheast:
    case PGFaboveleft:
      x = x + 0.707107 * radius;
      y = y - 0.707107 * radius;
      break;
  }

  anchorPos.setX(x);
  anchorPos.setY(y);

  if (dopath)
  {
    state = state->save(false);
    if (state->outerXSep < state->outerYSep)
      radius = radius - state->outerYSep;
    else
      radius = radius - state->outerXSep;
    state->moveTo(centerPoint);
    state->addCircle(centerPoint,radius);  
    state = state->restore();

    double tempdima = radius;
    double xb = state->outerXSep;
    double yb = state->outerYSep;
    if (xb < yb)
      tempdima -= yb;
    else
      tempdima -= xb;

    tempdima -= 0.5 * state->lineWidth;
    x = centerPoint.x() - tempdima;
    y = centerPoint.y();
    state = state->save(false);
    state->moveTo(x,y);
    x = centerPoint.x() + tempdima;
    y = centerPoint.y();
    state->lineTo(x,y);
    state->setDraw(true);
    state = state->restore();

    if (!driver || !box)
    return ;
  
    if (state->isFillText)
      driver->fillText();

    QTransform transA = state->transform;
    if (box->isTextBox())
      driver->setTextMatrix(transA.m11(),transA.m12(),transA.m21(),transA.m22(),transA.dx(),transA.dy());
    else
      driver->setMatrix(transA.m11(),transA.m12(),transA.m21(),transA.m22(),transA.dx(),transA.dy());

    x = centerPoint.x() - 0.5 * boxWidth;
    y = centerPoint.y() + boxHeight + state->lineWidth + state->innerYSep;
    box->setXY(state->align, x, y, 0);
    y = centerPoint.y() + state->lineWidth + state->innerYSep;
    box->setXY(state->align, x, y, 1);
    box->doContent(driver); 
  }
}

void XWTikzShape::ellipseSplitShape(bool dopath)
{
  double wd = 0;
  double ht = 0;
  double dp = 0;
  if (box)
  {
    wd = box->getWidth(state->textWidth,1);
    ht = box->getHeight(state->textWidth,1);
    dp = box->getDepth(state->textWidth,1);
  }
  double x = state->innerXSep;
  double y = state->innerYSep;
  y *= 2;
  y += 0.5 * state->lineWidth;
  double xa = 0.5 * boxWidth + x;
  double xb = 0.5 * wd + x;
  double ya = boxHeight + boxDepth + y;
  double yb = ht + dp + y;
  if (xa > xb)
    x = 1.414213 * xa;
  else
    x = 1.414213 * xb;
  if (ya > yb)
    y = 1.414213 * ya;
  else
    y = 1.414213 * yb;
  xa = state->minWidth;
  ya = state->minHeight;
  if (x < 0.5 * xa)
    x = 0.5 * xa;
  if (y < 0.5 * ya)
    y = 0.5 * ya;
  QPointF radii(x,y);

  x = 0.5 * (boxWidth - wd);
  y = state->innerYSep;
  y *= -2;
  y -= state->lineWidth;
  y -= boxDepth;
  y -= ht;
  lowerPoint.setX(x);
  lowerPoint.setY(y);

  x = 0.5 * boxWidth;
  y = -state->innerYSep - boxDepth - 0.5 * state->lineWidth;
  centerPoint.setX(x);
  centerPoint.setY(y);

  switch (state->anchor)
  {
    default:
      {
        double xa = state->anotherPoint.x();
        double ya = state->anotherPoint.y();
        QPointF pd(xa,ya);

        QPointF p = pointBorderEllipse(pd,radii);
        x = p.x() + centerPoint.x();
        y = p.y() + centerPoint.y();
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      break;

    case PGFtext:
      anchorPos.setX(0);
      anchorPos.setY(0);
      break;

    case PGFcenter:
      anchorPos = centerPoint;
      break;

    case PGFlower:
      anchorPos = lowerPoint;
      break;

    case PGFmid:
      anchorPos = midPoint;
      break;

    case PGFmideast:
      anchorPos.setX(midPoint.x() + radii.x());
      anchorPos.setY(midPoint.y());
      break;

    case PGFmidwest:
      anchorPos.setX(midPoint.x() - radii.x());
      anchorPos.setY(midPoint.y());
      break;

    case PGFbase:
      anchorPos = basePoint;
      break;

    case PGFbaseeast:
      anchorPos.setX(basePoint.x() + radii.x());
      anchorPos.setY(basePoint.y());
      break;

    case PGFbasewest:
      anchorPos.setX(basePoint.x() - radii.x());
      anchorPos.setY(basePoint.y());
      break;

    case PGFnorth:
      anchorPos.setX(centerPoint.x());
      anchorPos.setY(centerPoint.y() + radii.y());
      break;

    case PGFsouth:
      anchorPos.setX(centerPoint.x());
      anchorPos.setY(centerPoint.y() - radii.y());
      break;

    case PGFeast:
      anchorPos.setX(centerPoint.x() + radii.x());
      anchorPos.setY(centerPoint.y());
      break;

    case PGFwest:
      anchorPos.setX(centerPoint.x() - radii.x());
      anchorPos.setY(centerPoint.y());
      break;

    case PGFnorthwest:
      anchorPos.setX(centerPoint.x() - 0.707106 * radii.x());
      anchorPos.setY(centerPoint.y() + 0.707106 * radii.y());
      break;

    case PGFsouthwest:
      anchorPos.setX(centerPoint.x() - 0.707106 * radii.x());
      anchorPos.setY(centerPoint.y() - 0.707106 * radii.y());
      break;

    case PGFnortheast:
      anchorPos.setX(centerPoint.x() + 0.707106 * radii.x());
      anchorPos.setY(centerPoint.y() + 0.707106 * radii.y());
      break;

    case PGFsoutheast:
      anchorPos.setX(centerPoint.x() + 0.707106 * radii.x());
      anchorPos.setY(centerPoint.y() - 0.707106 * radii.y());
      break;
  }

  if (dopath)
  {
    x = radii.x() - state->outerXSep;
    y = radii.y() - state->outerYSep;
    double tempdima = x;
    double tempdimb = y;
    QPointF a(tempdima,0);
    QPointF b(0,tempdimb);
    state = state->save(false);
    state->addEllipse(centerPoint,a,b);
    state->moveTo(centerPoint.x() - tempdima, centerPoint.y());
    state->lineTo(centerPoint.x() + tempdima, centerPoint.y());
    state = state->restore();

    if (!driver || !box)
    return ;
  
    if (state->isFillText)
      driver->fillText();

    QTransform transA = state->transform;
    if (box->isTextBox())
      driver->setTextMatrix(transA.m11(),transA.m12(),transA.m21(),transA.m22(),transA.dx(),transA.dy());
    else
      driver->setMatrix(transA.m11(),transA.m12(),transA.m21(),transA.m22(),transA.dx(),transA.dy());

    x = centerPoint.x() - 0.5 * boxWidth;
    y = centerPoint.y() + boxHeight + state->lineWidth + state->innerYSep;
    box->setXY(state->align, x, y, 0);
    y = centerPoint.y() + state->lineWidth + state->innerYSep;
    box->setXY(state->align, x, y, 1);
    box->doContent(driver); 
  }
}

void XWTikzShape::rectangleSplitShape(bool dopath)
{
  int counta = state->rectangleSplitParts;
  int parts = counta;
  double x = state->innerXSep;
  double innerxsep = x;
  double y = state->innerYSep;
  double innerysep = y;
  x = state->outerXSep;
  double outerxsep = x;
  y = state->outerYSep;
  double outerysep = y;
  x = 0;
  y = 0;
  double tempdima = 0;
  double tempdimb = 0;
  QList<double> wds,hts,deps;
  QList<bool> emptys;
  for (int i = 0; i < parts; i++)
  {
    double xa = 0;
    double ya = 0;
    double yb = 0;
    if (box)
    {
      xa = box->getWidth(state->textWidth, i);
      ya = box->getHeight(state->textWidth, i);
      yb = box->getDepth(state->textWidth, i);
    }
    if (xa == 0 && ya == 0 && yb == 0)
      emptys << true;
    else
      emptys << false;
    wds << xa;
    hts << ya;
    deps << yb;
    if (xa > x)
      x = xa;
    if (ya > tempdima)
      tempdima = ya;
    if (yb > tempdimb)
      tempdimb = yb;
    double yc = tempdima + tempdimb;
    if (yc > y)
      y = yc;
  }

  if (!(state->rectangleSplitIgnoreEmptyParts))
  {
    for (int i = 0; i < parts; i++)
    {
      if (emptys[i])
      {
        wds[i] = state->rectangleSplitEmptyPartWidth;
        hts[i] = state->rectangleSplitEmptyPartHeight;
        deps[i] = state->rectangleSplitEmptyPartDepth;
      }
    }
  }

  double maxwidth = x;
  double maxtotalheight = y;
  x = state->minWidth - state->innerXSep - state->innerXSep;
  double xa = x - maxwidth;
  if (xa > 0)
    maxwidth = x;
  y = state->minHeight - state->innerYSep - state->innerYSep;
  double ya = y - maxtotalheight;
  if (ya > 0)
    maxtotalheight = y;
  QList<QPointF> anchors;
  double yoffset = 0;
  double xoffset = 0;
  QPointF northeast,southwest, lastanchor;
  QList<QPointF> souths,norths,wests,easts,centers;
  if (state->rectangleSplitHorizontal)
  {
    x = 0;
    for (int i = 0; i < parts; i++)
    {
      if (state->rectangleSplitPartAlign[i] == PGFbottom)
        y = deps[i];
      else if (state->rectangleSplitPartAlign[i] == PGFtop)
        y = maxtotalheight - hts[i];
      else if (state->rectangleSplitPartAlign[i] == PGFbase)
        y = (maxtotalheight - tempdima + tempdimb) / 2;
      else
        y = (maxtotalheight - hts[i] - deps[i]) / 2;

      if (i == 0)
      {
        yoffset = y;
        y = 0;
      }
      else
      {
        y -= yoffset;
        x += wds[i];
        x += innerxsep;
        x += state->lineWidth;
        x += innerxsep;
      }
      lastanchor.setX(x);
      lastanchor.setY(y);
      anchors << lastanchor;
    }
    x = lastanchor.x() + wds[parts - 1] + innerxsep + outerxsep;
    y = maxtotalheight - yoffset + innerysep + outerysep;
    northeast.setX(x);
    northeast.setY(y);

    x = anchors[0].x() - innerxsep - outerxsep;
    y = -yoffset - innerysep - outerysep;
    southwest.setX(x);
    southwest.setY(y);

    centerPoint.setX((southwest.x() + northeast.x()) / 2);
    centerPoint.setY((southwest.y() + northeast.y()) / 2);

    for (int i = 0; i < parts; i++)
    {
      xa = anchors[i].x();
      double xb = wds[i];
      xa += 0.5 * xb;
      //x = xa;
      //y = southwest.y();
      //QPointF ps(x,y);
      //y = northeast.y();
      //QPointF pn(x,y);
      xa += 0.5 * xb;
      xa += innerxsep;
      xa += 0.5 * state->lineWidth;
      x = xa;
      y = southwest.y();
      QPointF ps(x,y);
      souths << ps;
      y = northeast.y();
      QPointF pn(x,y);
      norths << pn;
      y = centerPoint.y();
      QPointF pc(x,y);
      centers << pc;
    }
  }
  else
  {
    y = 0;
    for (int i = 0; i < parts; i++)
    {
      if (state->rectangleSplitPartAlign[i] == PGFleft)
        x = 0;
      else if (state->rectangleSplitPartAlign[i] == PGFright)
        x = maxwidth - wds[i];
      else 
        x = (maxwidth - wds[i]) / 2;
      if (i == 0)
      {
        xoffset = x;
        x = 0;
      }
      else
      {
        x -= xoffset;
        y -= deps[i];
        y -= innerysep;
        y -= 0.5 * state->lineWidth;
        y -= innerysep;
        y -= hts[i];
      }
      lastanchor.setX(x);
      lastanchor.setY(y);
      anchors << lastanchor;
    }

    x = maxwidth - xoffset + innerxsep + outerxsep;
    y = hts[0] + innerysep + outerysep + 0.5 * state->lineWidth;
    northeast.setX(x);
    northeast.setY(y);

    x = -xoffset - innerxsep - outerxsep;
    y = lastanchor.y() - deps[parts - 1] - innerysep - outerysep - 0.5 * state->lineWidth;
    southwest.setX(x);
    southwest.setY(y);

    centerPoint.setX((southwest.x() + northeast.x()) / 2);
    centerPoint.setY((southwest.y() + northeast.y()) / 2);

    for (int i = 0; i < parts; i++)
    {
      ya = hts[i];
      y += 0.5 * ya;
      ya = deps[i];
      y += 0.5 * ya;
      ya = anchors[i].y() - deps[i] - innerysep - 0.5 * state->lineWidth;
      x = southwest.x();
      y = ya;
      QPointF pw(x,y);
      wests << pw;
      x = northeast.x();
      QPointF pe(x,y);
      easts << pe;
      x = centerPoint.x();
      QPointF pc(x,y);
      centers << pc;
    }
  }

  switch (state->anchor)
  {
    default:
      {
        double tempdima = state->anotherPoint.x();
        double tempdimb = state->anotherPoint.y();
        x = 0.5 * (southwest.x() - northeast.x());
        y = 0.5 * (southwest.y() - northeast.y());
        QPointF p1(tempdima,tempdimb);
        QPointF p2(x,y);
        QPointF p = pointBorderRectangle(p1,p2);
        anchorPos = centerPoint + p;
      }
      break;

    case PGFcenter:
      anchorPos = centerPoint;
      break;

    case PGFbase:
      anchorPos = basePoint;
      break;

    case PGFbaseeast:
      anchorPos.setX(northeast.x());
      anchorPos.setY(0);
      break;

    case PGFbasewest:
      anchorPos.setX(southwest.x());
      anchorPos.setY(0);
      break;

    case PGFmid:
      anchorPos = midPoint;
      break;

    case PGFmideast:
      anchorPos.setX(northeast.x());
      anchorPos.setY(midPoint.y());
      break;

    case PGFmidwest:
      anchorPos.setX(southwest.x());
      anchorPos.setY(midPoint.y());
      break;

    case PGFnorth:
      anchorPos.setX(centerPoint.x());
      anchorPos.setY(northeast.y());
      break;

    case PGFsouth:
      anchorPos.setX(centerPoint.x());
      anchorPos.setY(southwest.y());
      break;

    case PGFeast:
      anchorPos.setX(northeast.x());
      anchorPos.setY(centerPoint.y());
      break;

    case PGFwest:
      anchorPos.setX(southwest.x());
      anchorPos.setY(centerPoint.y());
      break;

    case PGFnortheast:
      anchorPos = northeast;
      break;

    case PGFnorthwest:
      anchorPos.setX(southwest.x());
      anchorPos.setY(northeast.y());
      break;

    case PGFsouthwest:
      anchorPos = southwest;
      break;

    case PGFsoutheast:
      anchorPos.setX(northeast.x());
      anchorPos.setY(southwest.y());
      break;
  }

  if (dopath && box)
  {
    if (state->rectangleSplitUseCustomFill)
    {
      state = state->save(false);
      if (state->rectangleSplitHorizontal)
      {
        QPointF bottomleft = southwest;
        QPointF topright;
        for (int i = 0; i < parts; i++)
        {
          if (i == parts - 1)
            topright = northeast;
          else
            topright = norths[i];
          x = bottomleft.x();
          y = bottomleft.y() + outerysep;
          if (i == 0)
            x += outerxsep;
          bottomleft.setX(x);
          bottomleft.setY(y);

          x = topright.x();
          y = topright.y() - outerysep;
          if (i == parts - 1)
            x -= outerxsep;
          topright.setX(x);
          topright.setY(y);

          x = topright.x();
          y = bottomleft.y();
          state->moveTo(x,y);
          state->lineTo(bottomleft);
          x = bottomleft.x();
          y = topright.y();
          state->lineTo(x,y);
          state->lineTo(topright);
          state->closePath();
          state->setFillColor(state->rectangleSplitPartFill[i]);
        }
      }
      else
      {
        QPointF topright = northeast;
        QPointF bottomleft;
        for (int i = 0; i < parts; i++)
        {
          if (i == parts - 1)
            bottomleft = southwest;
          else
            bottomleft = wests[i];
          x = bottomleft.x() + outerxsep;
          y = bottomleft.y();
          if (parts == 1)
            y += outerysep;
          else
          {
            if (i == parts - 1)
              y += outerysep;
          }
          bottomleft.setX(x);
          bottomleft.setY(y);

          x = topright.x() - outerxsep;
          y = topright.y();
          if (parts == 1)
            y -= outerysep;
          else
          {
            if (i == 0)
              y -= outerysep;
          }
          topright.setX(x);
          topright.setY(y);
          state->moveTo(bottomleft);
          x = bottomleft.x();
          y = topright.y();
          state->lineTo(x,y);
          state->lineTo(topright);
          x = topright.x();
          y = bottomleft.y();
          state->lineTo(x,y);
          state->closePath();
          state->setFillColor(state->rectangleSplitPartFill[i]);
        }
      }
      state = state->restore();
    }

    x = southwest.x() + outerxsep;
    y = southwest.y() + outerysep;
    southwest.setX(x);
    southwest.setY(y);

    x = northeast.x() - outerxsep;
    y = northeast.y() - outerysep;
    northeast.setX(x);
    northeast.setY(y);
    QPointF p = southwest - northeast;
    state = state->save(false);
    state->addRectangle(southwest, p);
    if (state->rectangleSplitDrawSplits)
    {
      for (int i = 0; i < parts; i++)
      {
        if (state->rectangleSplitHorizontal)
        {
          x = norths[i].x();
          y = norths[i].y() - outerysep;
        }
        else
        {
          x = easts[i].x() - outerxsep;
          y = easts[i].y();
        }
        state->moveTo(x,y);
        if (state->rectangleSplitHorizontal)
        {
          x = souths[i].x();
          y = souths[i].y() + outerysep;
        }
        else
        {
          x = wests[i].x() + outerxsep;
          y = wests[i].y();
        }
        state->lineTo(x,y);
      }
    }
    state = state->restore();

    for (int i = 0; i < parts; i++)
    {
      box->setXY(state->rectangleSplitPartAlign[i],centers[i].x() - 0.5 * wds[i],centers[i].y() +  0.5 * (hts[i] + deps[i]), i);
    }

    doContent();
  }
}
