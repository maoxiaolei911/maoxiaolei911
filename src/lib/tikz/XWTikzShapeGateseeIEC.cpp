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

void XWTikzShape::beforeBgACSource()
{
  state->moveTo(0.7,0);
  QPointF c1(0.35,-0.5);
  QPointF e1(0,0);
  state->quadraticCurveTo(c1,e1);
  QPointF c2(-0.35,0.5);
  QPointF e2(-0.7,0);
  state->quadraticCurveTo(c2,e2);
  state->setDraw(true);
}

void XWTikzShape::beforeBgBackwardDiodeIEC()
{
  state->moveTo(-0.5,-1);
  state->lineTo(0.5,-1);
  state->moveTo(0,-1);
  state->lineTo(0,1);
  state->moveTo(-0.5,1);
  state->lineTo(0.5,1);
  state->setDraw(true);
}

void XWTikzShape::beforeBgBulbIEC()
{
  state->moveTo(cos((double)-135),sin((double)-135));
  state->lineTo(cos((double)45),sin((double)45));
  state->moveTo(cos((double)-45),sin((double)-45));
  state->lineTo(cos((double)135),sin((double)135));
  state->setDraw(true);
}

void XWTikzShape::beforeBgCurrentSourceIEC()
{
  state->moveTo(0,-1);
  state->lineTo(0,1);
  state->setDraw(true);
}

void XWTikzShape::beforeBgDCSource()
{
  state->moveTo(0.7,0.2);
  state->lineTo(-0.7,0.2);
  state->moveTo(0.7,-0.2);
  state->lineTo(-0.7,-0.2);
  state->setDraw(true);
}

void XWTikzShape::beforeBgDiodeIEC()
{
  state->moveTo(0,-1);
  state->lineTo(0,1);
  state->setDraw(true);
}

void XWTikzShape::beforeBgSchottkyDiodeIEC()
{
  state->moveTo(-0.4,-0.6);
  state->lineTo(-0.4,-1);
  state->lineTo(0,-1);
  state->lineTo(0,1);
  state->lineTo(0.4,1);
  state->lineTo(0.4,0.6);
  state->setDraw(true);
}

void XWTikzShape::beforeBgTunnelDiodeIEC()
{
  state->moveTo(-0.5,-1);
  state->lineTo(0,-1);
  state->lineTo(0,1);
  state->lineTo(-0.5,1);
  state->setDraw(true);
}

void XWTikzShape::beforeBgVoltageSourceIEC()
{
  state->moveTo(-1,0);
  state->lineTo(1,0);
  state->setDraw(true);
}

void XWTikzShape::beforeBgZenerDiodeIEC()
{
  state->moveTo(-0.5,-1);
  state->lineTo(0,-1);
  state->lineTo(0,1);
  state->setDraw(true);
}

void XWTikzShape::batteryIECShape(bool dopath)
{
  double x = boxWidth + 2 * state->innerXSep;
  if (x < state->minWidth)
    x = state->minWidth;
  x = 0.5 * x + 0.5 * boxWidth + state->outerXSep;
  double y = boxHeight + boxDepth + 2 * state->innerYSep;
  if (y < state->minHeight)
    y = state->minHeight;
  y = 0.5 * y - 0.5 * boxDepth + 0.5 * boxHeight + state->outerYSep;
  northEast.setX(x);
  northEast.setY(y);

  x = boxWidth + 2 * state->innerXSep;
  if (x < state->minWidth)
    x = state->minWidth;
  x = -0.5 * x + 0.5 * boxWidth - state->outerXSep;
  y = boxHeight + boxDepth + 2 * state->innerYSep;
  if (y < state->minHeight)
    y = state->minHeight;
  y = -0.5 * y - 0.5 * boxDepth + 0.5 * boxHeight - state->outerYSep;
  southWest.setX(x);
  southWest.setY(y);

  centerPos.setX(0.5 * southWest.x() + 0.5 * northEast.x());
  centerPos.setY(0.5 * southWest.y() + 0.5 * northEast.y());

  westPos.setX(southWest.x() - state->left);
  westPos.setY(0.5 * southWest.y() + 0.5 * northEast.y());
  eastPos.setX(northEast.x());
  eastPos.setY(0.5 * southWest.y() + 0.5 * northEast.y());
  northPos.setX(0.5 * northEast.x() + 0.5 * southWest.x());
  northPos.setY(northEast.y() + state->above);
  southPos.setX(0.5 * southWest.x() + 0.5 * northEast.x());
  southPos.setY(southWest.y() - state->below);

  switch (state->anchor)
  {
    default:
      {
        double delta = state->angle;
        double xb = cos(delta);
        double yb = sin(delta);
        double xa = southWest.x();
        double ya = southWest.y();
        double xc = 0.5 * (northEast.x() - xa);
        double yc = 0.5 * (northEast.y() - ya);
        xa += xc;
        ya += yc;
        QPointF pd(xb,yb);
        QPointF ur(xc,yc);
        QPointF p = pointBorderRectangle(pd,ur);
        x = p.x() + xa;
        y = p.y() + ya;
      }
      break;

    case PGFtext:
      anchorPos.setX(0);
      anchorPos.setY(0);
      break;

    case PGFcenter:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = 0.5 * southWest.y() + 0.5 * northEast.y();
      break;

    case PGFmid:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = 0.5 * 4.3;
      break;

    case PGFbase:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = 0;
      break;

    case PGFnorth:
    case PGFabove:
      x = 0.5 * northEast.x() + 0.5 * southWest.x();
      y = northEast.y() + state->above;
      break;

    case PGFsouth:
    case PGFbelow:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = southWest.y() - state->below;
      break;

    case PGFwest:
    case PGFleft:
      x = southWest.x() - state->left;
      y = 0.5 * southWest.y() + 0.5 * northEast.y();
      break;

    case PGFmidwest:
      x = southWest.x();
      y = 0.5 * 4.3;
      break;

    case PGFbasewest:
      x = southWest.x();
      y = 0;
      break;

    case PGFnorthwest:
      x = southWest.x();
      y = northEast.y();
      break;

    case PGFsouthwest:
      x = southWest.x();
      y = southWest.y();
      break;

    case PGFeast:
    case PGFright:
      x = northEast.x();
      y = 0.5 * southWest.y() + 0.5 * northEast.y();
      break;

    case PGFmideast:
      x = northEast.x();
      y = 0.5 * 4.3;
      break;

    case PGFbaseeast:
      x = northEast.x();
      y = 0;
      break;

    case PGFnortheast:
      x = northEast.x();
      y = northEast.y();
      break;

    case PGFsoutheast:
      x = northEast.x();
      y = southWest.y();
      break;

    case PGFinput:
      x = northEast.x();
      y = 0.5 * (northEast.y() + southWest.y());
      break;

    case PGFoutput:
      x = southWest.x();
      y = 0.5 * (northEast.y() + southWest.y());
      break;
  }
  anchorPos.setX(x);
  anchorPos.setY(y);

  if (dopath)
  {
    double xa = southWest.x() + state->outerXSep;
    double ya = southWest.y() + state->outerYSep;
    double xb = northEast.x() - state->outerXSep;
    double yb = northEast.y() - state->outerYSep;

    state = state->save(false);
    state->moveTo(xa, ya);
    state->lineTo(xa, yb);
    double yc = 0.25*yb - 0.25*ya;
    ya += yc;
    yb -= yc;
    state->moveTo(xb, ya);
    state->lineTo(xb, yb);
    state = state->restore();
    doContent();
  }
}

void XWTikzShape::breakcontactIECShape(bool dopath)
{
  double x = state->outerXSep + 0.5 * state->minWidth;
  double y = state->outerYSep + state->minHeight;
  northEast.setX(x);
  northEast.setY(y);

  x = -0.5 * state->outerXSep - 0.5 * state->minWidth;
  y = -state->outerYSep;
  southWest.setX(x);
  southWest.setY(y);

  centerPos.setX(0);
  centerPos.setY(0);

  westPos.setX(southWest.x() - state->left);
  westPos.setY(0.5 * southWest.y() + 0.5 * northEast.y());
  eastPos.setX(northEast.x());
  eastPos.setY(0.5 * southWest.y() + 0.5 * northEast.y());
  northPos.setX(0.5 * northEast.x() + 0.5 * southWest.x());
  northPos.setY(northEast.y() + state->above);
  southPos.setX(0.5 * southWest.x() + 0.5 * northEast.x());
  southPos.setY(southWest.y() - state->below);

  switch (state->anchor)
  {
    default:
      {
        double delta = state->angle;
        double xc = cos(delta);
        double yc = sin(delta);
        QPointF pd(xc,yc);
        if (yc < 0)
        {
          double xa = southWest.x();
          double ya = southWest.y() - 0.5;          
          QPointF ur(-xa, -ya);
          QPointF p = pointBorderRectangle(pd, ur);
          x = p.x();
          y = p.y();
        }
        else
        {
          QPointF p = pointBorderRectangle(pd, northEast);
          x = p.x();
          y = p.y();
        }
      }
      break;

    case PGFtext:
      anchorPos.setX(0);
      anchorPos.setY(0);
      break;

    case PGFcenter:
      x = 0;
      y = 0;
      break;

    case PGFmid:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = 0.5 * 4.3;
      break;

    case PGFbase:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = 0;
      break;

    case PGFnorth:
    case PGFabove:
      x = 0.5 * northEast.x() + 0.5 * southWest.x();
      y = northEast.y() + state->above;
      break;

    case PGFsouth:
    case PGFbelow:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = southWest.y() - state->below;
      break;

    case PGFwest:
    case PGFleft:
      x = southWest.x() - state->left;
      y = 0.5 * southWest.y() + 0.5 * northEast.y();
      break;

    case PGFmidwest:
      x = southWest.x();
      y = 0.5 * 4.3;
      break;

    case PGFbasewest:
      x = southWest.x();
      y = 0;
      break;

    case PGFnorthwest:
      x = southWest.x();
      y = northEast.y();
      break;

    case PGFsouthwest:
      x = southWest.x();
      y = southWest.y();
      break;

    case PGFeast:
    case PGFright:
      x = northEast.x();
      y = 0.5 * southWest.y() + 0.5 * northEast.y();
      break;

    case PGFmideast:
      x = northEast.x();
      y = 0.5 * 4.3;
      break;

    case PGFbaseeast:
      x = northEast.x();
      y = 0;
      break;

    case PGFnortheast:
      x = northEast.x();
      y = northEast.y();
      break;

    case PGFsoutheast:
      x = northEast.x();
      y = southWest.y();
      break;

    case PGFinput:
      x = northEast.x();
      y = 0.5 * (northEast.y() + southWest.y());
      break;

    case PGFoutput:
      x = southWest.x();
      y = 0.5 * (northEast.y() + southWest.y());
      break;
  }
  anchorPos.setX(x);
  anchorPos.setY(y);

  if (dopath)
  {
    x = northEast.x() - state->outerXSep;
    y = northEast.y() - state->outerYSep;
    double xa = -x;
    double ya = 0;
    double xb = x;
    double yb = y;
    state = state->save(false);
    state->moveTo(xa, ya);
    state->lineTo(xb, yb);
    double xc = 0.2 * xb - 0.2 * xa;
    state->moveTo(xb, 0);
    xb -= xc;
    state->lineTo(xb, 0);
    state->lineTo(xb, yb);
    state = state->restore();
    doContent();
  }
}

void XWTikzShape::breakdowndiodeIECShape(bool dopath)
{
  double x = boxWidth + 2 * state->innerXSep;
  if (x < state->minWidth)
    x = state->minWidth;
  x = 0.5 * x + 0.5 * boxWidth + state->outerXSep;
  double y = boxHeight + boxDepth + 2 * state->innerYSep;
  if (y < state->minHeight)
    y = state->minHeight;
  y = 0.5 * y - 0.5 * boxDepth + 0.5 * boxHeight + state->outerYSep;
  northEast.setX(x);
  northEast.setY(y);

  x = boxWidth + 2 * state->innerXSep;
  if (x < state->minWidth)
    x = state->minWidth;
  x = -0.5 * x + 0.5 * boxWidth - state->outerXSep;
  y = boxHeight + boxDepth + 2 * state->innerYSep;
  if (y < state->minHeight)
    y = state->minHeight;
  y = -0.5 * y - 0.5 * boxDepth + 0.5 * boxHeight - state->outerYSep;
  southWest.setX(x);
  southWest.setY(y);

  centerPos.setX(0.5 * southWest.x() + 0.5 * northEast.x());
  centerPos.setY(0.5 * southWest.y() + 0.5 * northEast.y());

  westPos.setX(southWest.x() - state->left);
  westPos.setY(0.5 * southWest.y() + 0.5 * northEast.y());
  eastPos.setX(northEast.x());
  eastPos.setY(0.5 * southWest.y() + 0.5 * northEast.y());
  northPos.setX(0.5 * northEast.x() + 0.5 * southWest.x());
  northPos.setY(northEast.y() + state->above);
  southPos.setX(0.5 * southWest.x() + 0.5 * northEast.x());
  southPos.setY(southWest.y() - state->below);

  switch (state->anchor)
  {
    default:
      {
        double delta = state->angle;
        double xb = cos(delta);
        double yb = sin(delta);
        double xa = southWest.x();
        double ya = southWest.y();
        double xc = 0.5 * (northEast.x() - xa);
        double yc = 0.5 * (northEast.y() - ya);
        xa += xc;
        ya += yc;
        QPointF pd(xb,yb);
        QPointF ur(xc,yc);
        QPointF p = pointBorderRectangle(pd,ur);
        x = p.x() + xa;
        y = p.y() + ya;
      }
      break;

    case PGFtext:
      anchorPos.setX(0);
      anchorPos.setY(0);
      break;

    case PGFcenter:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = 0.5 * southWest.y() + 0.5 * northEast.y();
      break;

    case PGFmid:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = 0.5 * 4.3;
      break;

    case PGFbase:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = 0;
      break;

    case PGFnorth:
    case PGFabove:
      x = 0.5 * northEast.x() + 0.5 * southWest.x();
      y = northEast.y() + state->above;
      break;

    case PGFsouth:
    case PGFbelow:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = southWest.y() - state->below;
      break;

    case PGFwest:
    case PGFleft:
      x = southWest.x() - state->left;
      y = 0.5 * southWest.y() + 0.5 * northEast.y();
      break;

    case PGFmidwest:
      x = southWest.x();
      y = 0.5 * 4.3;
      break;

    case PGFbasewest:
      x = southWest.x();
      y = 0;
      break;

    case PGFnorthwest:
      x = southWest.x();
      y = northEast.y();
      break;

    case PGFsouthwest:
      x = southWest.x();
      y = southWest.y();
      break;

    case PGFeast:
    case PGFright:
      x = northEast.x();
      y = 0.5 * southWest.y() + 0.5 * northEast.y();
      break;

    case PGFmideast:
      x = northEast.x();
      y = 0.5 * 4.3;
      break;

    case PGFbaseeast:
      x = northEast.x();
      y = 0;
      break;

    case PGFnortheast:
      x = northEast.x();
      y = northEast.y();
      break;

    case PGFsoutheast:
      x = northEast.x();
      y = southWest.y();
      break;

    case PGFinput:
      x = northEast.x();
      y = 0.5 * (northEast.y() + southWest.y());
      break;

    case PGFoutput:
      x = southWest.x();
      y = 0.5 * (northEast.y() + southWest.y());
      break;
  }
  anchorPos.setX(x);
  anchorPos.setY(y);

  if (dopath)
  {
    double xa = southWest.x() + state->outerXSep;
    double ya = southWest.y() + state->outerYSep;
    double xb = northEast.x() - state->outerXSep;
    double yb = northEast.y() - state->outerYSep;
    double xc = 0.5 * xa + 0.5 * xb;
    double yc = 0.5 * ya + 0.5 * yb;

    state = state->save(false);
    state->moveTo(xa, ya);
    state->lineTo(xc, yc);
    state->lineTo(xa, yb);
    state->closePath();
    state->moveTo(xb, ya);
    state->lineTo(xc, yc);
    state->lineTo(xb, yb);
    state->closePath();
    state = state->restore();

    state = state->save(false);
    state->moveTo(xa, yc);
    state->lineTo(xb, yc);
    state->setDraw(true);
    state = state->restore();
    state = state->save(false);
    state->addShift(xc, yc);
    yc = 0.5 * yb - 0.5 * ya;
    state->scale(yc, yc);
    state->moveTo(-0.5, -1);
    state->lineTo(0, -1);
    state->lineTo(0, 1);
    state->lineTo(0.5, 1);
    state->setDraw(true);
    state = state->restore();

    doContent();
  }
}

void XWTikzShape::capacitorIECShape(bool dopath)
{
  double x = boxWidth + 2 * state->innerXSep;
  if (x < state->minWidth)
    x = state->minWidth;
  x = 0.5 * x + 0.5 * boxWidth + state->outerXSep;
  double y = boxHeight + boxDepth + 2 * state->innerYSep;
  if (y < state->minHeight)
    y = state->minHeight;
  y = 0.5 * y - 0.5 * boxDepth + 0.5 * boxHeight + state->outerYSep;
  northEast.setX(x);
  northEast.setY(y);

  x = boxWidth + 2 * state->innerXSep;
  if (x < state->minWidth)
    x = state->minWidth;
  x = -0.5 * x + 0.5 * boxWidth - state->outerXSep;
  y = boxHeight + boxDepth + 2 * state->innerYSep;
  if (y < state->minHeight)
    y = state->minHeight;
  y = -0.5 * y - 0.5 * boxDepth + 0.5 * boxHeight - state->outerYSep;
  southWest.setX(x);
  southWest.setY(y);

  centerPos.setX(0.5 * southWest.x() + 0.5 * northEast.x());
  centerPos.setY(0.5 * southWest.y() + 0.5 * northEast.y());

  westPos.setX(southWest.x() - state->left);
  westPos.setY(0.5 * southWest.y() + 0.5 * northEast.y());
  eastPos.setX(northEast.x());
  eastPos.setY(0.5 * southWest.y() + 0.5 * northEast.y());
  northPos.setX(0.5 * northEast.x() + 0.5 * southWest.x());
  northPos.setY(northEast.y() + state->above);
  southPos.setX(0.5 * southWest.x() + 0.5 * northEast.x());
  southPos.setY(southWest.y() - state->below);

  switch (state->anchor)
  {
    default:
      {
        double delta = state->angle;
        double xb = cos(delta);
        double yb = sin(delta);
        double xa = southWest.x();
        double ya = southWest.y();
        double xc = 0.5 * (northEast.x() - xa);
        double yc = 0.5 * (northEast.y() - ya);
        xa += xc;
        ya += yc;
        QPointF pd(xb,yb);
        QPointF ur(xc,yc);
        QPointF p = pointBorderRectangle(pd,ur);
        x = p.x() + xa;
        y = p.y() + ya;
      }
      break;

    case PGFtext:
      anchorPos.setX(0);
      anchorPos.setY(0);
      break;

    case PGFcenter:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = 0.5 * southWest.y() + 0.5 * northEast.y();
      break;

    case PGFmid:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = 0.5 * 4.3;
      break;

    case PGFbase:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = 0;
      break;

    case PGFnorth:
    case PGFabove:
      x = 0.5 * northEast.x() + 0.5 * southWest.x();
      y = northEast.y() + state->above;
      break;

    case PGFsouth:
    case PGFbelow:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = southWest.y() - state->below;
      break;

    case PGFwest:
    case PGFleft:
      x = southWest.x() - state->left;
      y = 0.5 * southWest.y() + 0.5 * northEast.y();
      break;

    case PGFmidwest:
      x = southWest.x();
      y = 0.5 * 4.3;
      break;

    case PGFbasewest:
      x = southWest.x();
      y = 0;
      break;

    case PGFnorthwest:
      x = southWest.x();
      y = northEast.y();
      break;

    case PGFsouthwest:
      x = southWest.x();
      y = southWest.y();
      break;

    case PGFeast:
    case PGFright:
      x = northEast.x();
      y = 0.5 * southWest.y() + 0.5 * northEast.y();
      break;

    case PGFmideast:
      x = northEast.x();
      y = 0.5 * 4.3;
      break;

    case PGFbaseeast:
      x = northEast.x();
      y = 0;
      break;

    case PGFnortheast:
      x = northEast.x();
      y = northEast.y();
      break;

    case PGFsoutheast:
      x = northEast.x();
      y = southWest.y();
      break;

    case PGFinput:
      x = northEast.x();
      y = 0.5 * (northEast.y() + southWest.y());
      break;

    case PGFoutput:
      x = southWest.x();
      y = 0.5 * (northEast.y() + southWest.y());
      break;
  }
  anchorPos.setX(x);
  anchorPos.setY(y);

  if (dopath)
  {
    double xa = southWest.x() + state->outerXSep;
    double ya = southWest.y() + state->innerYSep;
    double xb = northEast.x() - state->outerXSep;
    double yb = northEast.y() - state->innerYSep;
    state = state->save(false);
    state->moveTo(xa, ya);
    state->lineTo(xa, yb);
    state->moveTo(xb, ya);
    state->lineTo(xb, yb);
    state = state->restore();
    doContent();
  }
}

void XWTikzShape::genericcircleIECShape(bool dopath)
{
  double ya = 0.5 * boxHeight + 0.5 * boxDepth + state->innerYSep;
  double xa = 0.5 * boxWidth + state->innerXSep;
  radius = sqrt(xa * xa + ya * ya);
  double x = xa / radius;
  double y = ya / radius;
  if (x > y)
    radius = x * xa;
  else
    radius = y * ya;

  if (radius < 0.5 * state->minWidth)
    radius = 0.5 * state->minWidth;

  if (radius < 0.5 * state->minHeight)
    radius = 0.5 * state->minHeight;

  if (state->outerXSep < state->outerYSep)
    radius = radius + state->outerYSep;
  else
    radius = radius + state->outerXSep;

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
        double delta = state->angle;
        double xa = cos(delta);
        double ya = sin(delta);
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

    case PGFinput:
      x = centerPoint.x() - radius;
      y = centerPoint.y();
      break;

    case PGFoutput:
      x = centerPoint.x() + radius;
      y = centerPoint.y();
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
    x = centerPoint.x();
    y = centerPoint.y();
    double tempdima = radius;
    double xb = state->outerXSep;
    double yb = state->outerYSep;
    if (xb < yb)
      tempdima -= yb;
    else
      tempdima -= xb;
    state->scale(tempdima, tempdima);
    state = state->save(false);
    doBeforeBackground();
    state = state->restore();
    doContent();
  }
}

void XWTikzShape::genericdiodeIECShape(bool dopath)
{
  double x = boxWidth + 2 * state->innerXSep;
  if (x < state->minWidth)
    x = state->minWidth;
  x = 0.5 * x + 0.5 * boxWidth + state->outerXSep;
  double y = boxHeight + boxDepth + 2 * state->innerYSep;
  if (y < state->minHeight)
    y = state->minHeight;
  y = 0.5 * y - 0.5 * boxDepth + 0.5 * boxHeight + state->outerYSep;
  northEast.setX(x);
  northEast.setY(y);

  x = boxWidth + 2 * state->innerXSep;
  if (x < state->minWidth)
    x = state->minWidth;
  x = -0.5 * x + 0.5 * boxWidth - state->outerXSep;
  y = boxHeight + boxDepth + 2 * state->innerYSep;
  if (y < state->minHeight)
    y = state->minHeight;
  y = -0.5 * y - 0.5 * boxDepth + 0.5 * boxHeight - state->outerYSep;
  southWest.setX(x);
  southWest.setY(y);

  centerPos.setX(0.5 * southWest.x() + 0.5 * northEast.x());
  centerPos.setY(0.5 * southWest.y() + 0.5 * northEast.y());

  westPos.setX(southWest.x() - state->left);
  westPos.setY(0.5 * southWest.y() + 0.5 * northEast.y());
  eastPos.setX(northEast.x());
  eastPos.setY(0.5 * southWest.y() + 0.5 * northEast.y());
  northPos.setX(0.5 * northEast.x() + 0.5 * southWest.x());
  northPos.setY(northEast.y() + state->above);
  southPos.setX(0.5 * southWest.x() + 0.5 * northEast.x());
  southPos.setY(southWest.y() - state->below);

  switch (state->anchor)
  {
    default:
      {
        double delta = state->angle;
        double xb = cos(delta);
        double yb = sin(delta);
        double xa = southWest.x();
        double ya = southWest.y();
        double xc = 0.5 * (northEast.x() - xa);
        double yc = 0.5 * (northEast.y() - ya);
        xa += xc;
        ya += yc;
        QPointF pd(xb,yb);
        QPointF ur(xc,yc);
        QPointF p = pointBorderRectangle(pd,ur);
        x = p.x() + xa;
        y = p.y() + ya;
      }
      break;

    case PGFtext:
      anchorPos.setX(0);
      anchorPos.setY(0);
      break;

    case PGFcenter:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = 0.5 * southWest.y() + 0.5 * northEast.y();
      break;

    case PGFmid:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = 0.5 * 4.3;
      break;

    case PGFbase:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = 0;
      break;

    case PGFnorth:
    case PGFabove:
      x = 0.5 * northEast.x() + 0.5 * southWest.x();
      y = northEast.y() + state->above;
      break;

    case PGFsouth:
    case PGFbelow:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = southWest.y() - state->below;
      break;

    case PGFwest:
    case PGFleft:
      x = southWest.x() - state->left;
      y = 0.5 * southWest.y() + 0.5 * northEast.y();
      break;

    case PGFmidwest:
      x = southWest.x();
      y = 0.5 * 4.3;
      break;

    case PGFbasewest:
      x = southWest.x();
      y = 0;
      break;

    case PGFnorthwest:
      x = southWest.x();
      y = northEast.y();
      break;

    case PGFsouthwest:
      x = southWest.x();
      y = southWest.y();
      break;

    case PGFeast:
    case PGFright:
      x = northEast.x();
      y = 0.5 * southWest.y() + 0.5 * northEast.y();
      break;

    case PGFmideast:
      x = northEast.x();
      y = 0.5 * 4.3;
      break;

    case PGFbaseeast:
      x = northEast.x();
      y = 0;
      break;

    case PGFnortheast:
      x = northEast.x();
      y = northEast.y();
      break;

    case PGFsoutheast:
      x = northEast.x();
      y = southWest.y();
      break;

    case PGFinput:
      x = northEast.x();
      y = 0.5 * (northEast.y() + southWest.y());
      break;

    case PGFoutput:
      x = southWest.x();
      y = 0.5 * (northEast.y() + southWest.y());
      break;
  }
  anchorPos.setX(x);
  anchorPos.setY(y);

  if (dopath)
  {
    double xa = southWest.x() + state->outerXSep;
    double ya = southWest.y() + state->outerYSep;
    double xb = northEast.x() - state->outerXSep;
    double yb = northEast.y() - state->outerYSep;

    xb -= 0.5 * state->lineWidth;
    state = state->save(false);
    state->moveTo(xa, ya);
    state->lineTo(xa, yb);
    double yc = 0.5 * ya + 0.5 * yb;
    state->lineTo(xb, yc);
    state->closePath();
    state = state->restore();

    xa = southWest.x() + state->outerXSep;
    ya = southWest.y() + state->outerYSep;
    xb = northEast.x() - state->outerXSep;
    yb = northEast.y() - state->outerYSep;
    state = state->save(false);
    state->moveTo(xa, yc);
    state->lineTo(xb, yc);
    state->setDraw(true);
    state = state->restore();
    state = state->save(false);
    state->addShift(xb, yc);
    yc = 0.5 * yb - 0.5 * ya;
    state->scale(yc, yc);
    doBeforeBackground();
    state = state->restore();

    doContent();
  }
}

void XWTikzShape::groundIECShape(bool dopath)
{
  double x = boxWidth + 2 * state->innerXSep;
  if (x < state->minWidth)
    x = state->minWidth;
  x = 0.5 * x + 0.5 * boxWidth + state->outerXSep;
  double y = boxHeight + boxDepth + 2 * state->innerYSep;
  if (y < state->minHeight)
    y = state->minHeight;
  y = 0.5 * y - 0.5 * boxDepth + 0.5 * boxHeight + state->outerYSep;
  northEast.setX(x);
  northEast.setY(y);

  x = boxWidth + 2 * state->innerXSep;
  if (x < state->minWidth)
    x = state->minWidth;
  x = -0.5 * x + 0.5 * boxWidth - state->outerXSep;
  y = boxHeight + boxDepth + 2 * state->innerYSep;
  if (y < state->minHeight)
    y = state->minHeight;
  y = -0.5 * y - 0.5 * boxDepth + 0.5 * boxHeight - state->outerYSep;
  southWest.setX(x);
  southWest.setY(y);

  centerPos.setX(0.5 * southWest.x() + 0.5 * northEast.x());
  centerPos.setY(0.5 * southWest.y() + 0.5 * northEast.y());

  westPos.setX(southWest.x() - state->left);
  westPos.setY(0.5 * southWest.y() + 0.5 * northEast.y());
  eastPos.setX(northEast.x());
  eastPos.setY(0.5 * southWest.y() + 0.5 * northEast.y());
  northPos.setX(0.5 * northEast.x() + 0.5 * southWest.x());
  northPos.setY(northEast.y() + state->above);
  southPos.setX(0.5 * southWest.x() + 0.5 * northEast.x());
  southPos.setY(southWest.y() - state->below);

  switch (state->anchor)
  {
    default:
      {
        double delta = state->angle;
        double xb = cos(delta);
        double yb = sin(delta);
        double xa = southWest.x();
        double ya = southWest.y();
        double xc = 0.5 * (northEast.x() - xa);
        double yc = 0.5 * (northEast.y() - ya);
        xa += xc;
        ya += yc;
        QPointF pd(xb,yb);
        QPointF ur(xc,yc);
        QPointF p = pointBorderRectangle(pd,ur);
        x = p.x() + xa;
        y = p.y() + ya;
      }
      break;

    case PGFtext:
      anchorPos.setX(0);
      anchorPos.setY(0);
      break;

    case PGFcenter:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = 0.5 * southWest.y() + 0.5 * northEast.y();
      break;

    case PGFmid:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = 0.5 * 4.3;
      break;

    case PGFbase:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = 0;
      break;

    case PGFnorth:
    case PGFabove:
      x = 0.5 * northEast.x() + 0.5 * southWest.x();
      y = northEast.y() + state->above;
      break;

    case PGFsouth:
    case PGFbelow:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = southWest.y() - state->below;
      break;

    case PGFwest:
    case PGFleft:
      x = southWest.x() - state->left;
      y = 0.5 * southWest.y() + 0.5 * northEast.y();
      break;

    case PGFmidwest:
      x = southWest.x();
      y = 0.5 * 4.3;
      break;

    case PGFbasewest:
      x = southWest.x();
      y = 0;
      break;

    case PGFnorthwest:
      x = southWest.x();
      y = northEast.y();
      break;

    case PGFsouthwest:
      x = southWest.x();
      y = southWest.y();
      break;

    case PGFeast:
    case PGFright:
      x = northEast.x();
      y = 0.5 * southWest.y() + 0.5 * northEast.y();
      break;

    case PGFmideast:
      x = northEast.x();
      y = 0.5 * 4.3;
      break;

    case PGFbaseeast:
      x = northEast.x();
      y = 0;
      break;

    case PGFnortheast:
      x = northEast.x();
      y = northEast.y();
      break;

    case PGFsoutheast:
      x = northEast.x();
      y = southWest.y();
      break;

    case PGFinput:
      x = northEast.x();
      y = 0.5 * (northEast.y() + southWest.y());
      break;

    case PGFoutput:
      x = southWest.x();
      y = 0.5 * (northEast.y() + southWest.y());
      break;
  }
  anchorPos.setX(x);
  anchorPos.setY(y);

  if (dopath)
  {
    double xa = southWest.x() + state->outerXSep;
    double ya = southWest.y() + state->innerYSep;
    double xb = northEast.x() - state->outerXSep;
    double yb = northEast.y() - state->innerYSep;
    state = state->save(false);
    state->moveTo(xa, ya);
    state->lineTo(xa, yb);
    xa = 0.5 * xa + 0.5 * xb;
    double yc = 0.16666 * yb - 0.16666*ya;
    ya += yc;
    yb -= yc;
    state->moveTo(xa, ya);
    state->lineTo(xa, yb);
    ya += yc;
    yb -= yc;
    state->moveTo(xb, ya);
    state->lineTo(xb, yb);
    state = state->restore();
    doContent();
  }
}

void XWTikzShape::inductorIECShape(bool dopath)
{
  double x = state->outerXSep + 0.5 * state->minWidth;
  double y = state->innerYSep + state->minHeight;
  northEast.setX(x);
  northEast.setY(y);

  x = -state->outerXSep - 0.5 * state->minWidth;
  y = -state->outerYSep;
  southWest.setX(x);
  southWest.setY(y);

  centerPos.setX(0.5 * southWest.x() + 0.5 * northEast.x());
  centerPos.setY(0.5 * southWest.y() + 0.5 * northEast.y());

  westPos.setX(southWest.x() - state->left);
  westPos.setY(0.5 * southWest.y() + 0.5 * northEast.y());
  eastPos.setX(northEast.x());
  eastPos.setY(0.5 * southWest.y() + 0.5 * northEast.y());
  northPos.setX(0.5 * northEast.x() + 0.5 * southWest.x());
  northPos.setY(northEast.y() + state->above);
  southPos.setX(0.5 * southWest.x() + 0.5 * northEast.x());
  southPos.setY(southWest.y() - state->below);

  switch (state->anchor)
  {
    default:
      {
        double delta = state->angle;
        double xc = cos(delta);
        double yc = sin(delta);
        QPointF pd(xc,yc);
        if (yc < 0)
        {
          double xa = southWest.x();
          double ya = southWest.y() - 0.5;          
          QPointF ur(-xa, -ya);
          QPointF p = pointBorderRectangle(pd, ur);
          x = p.x();
          y = p.y();
        }
        else
        {
          QPointF p = pointBorderRectangle(pd, northEast);
          x = p.x();
          y = p.y();
        }
      }
      break;

    case PGFtext:
      anchorPos.setX(0);
      anchorPos.setY(0);
      break;

    case PGFcenter:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = 0.5 * southWest.y() + 0.5 * northEast.y();
      break;

    case PGFmid:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = 0.5 * 4.3;
      break;

    case PGFbase:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = 0;
      break;

    case PGFnorth:
    case PGFabove:
      x = 0.5 * northEast.x() + 0.5 * southWest.x();
      y = northEast.y() + state->above;
      break;

    case PGFsouth:
    case PGFbelow:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = southWest.y() - state->below;
      break;

    case PGFwest:
    case PGFleft:
      x = southWest.x() - state->left;
      y = 0.5 * southWest.y() + 0.5 * northEast.y();
      break;

    case PGFmidwest:
      x = southWest.x();
      y = 0.5 * 4.3;
      break;

    case PGFbasewest:
      x = southWest.x();
      y = 0;
      break;

    case PGFnorthwest:
      x = southWest.x();
      y = northEast.y();
      break;

    case PGFsouthwest:
      x = southWest.x();
      y = southWest.y();
      break;

    case PGFeast:
    case PGFright:
      x = northEast.x();
      y = 0.5 * southWest.y() + 0.5 * northEast.y();
      break;

    case PGFmideast:
      x = northEast.x();
      y = 0.5 * 4.3;
      break;

    case PGFbaseeast:
      x = northEast.x();
      y = 0;
      break;

    case PGFnortheast:
      x = northEast.x();
      y = northEast.y();
      break;

    case PGFsoutheast:
      x = northEast.x();
      y = southWest.y();
      break;

    case PGFinput:
      x = northEast.x();
      y = 0.5 * (northEast.y() + southWest.y());
      break;

    case PGFoutput:
      x = southWest.x();
      y = 0.5 * (northEast.y() + southWest.y());
      break;
  }
  anchorPos.setX(x);
  anchorPos.setY(y);

  if (dopath)
  {
    double xa = -northEast.x() + state->outerXSep;
    double ya = 0;
    double xb = northEast.x() - state->outerXSep;
    double yb = northEast.y() - state->innerYSep;
    double xc = yb;
    state = state->save(false);
    state->moveTo(xa, ya);
    xb += 1;
    xa += 2 * xc;
    state->setStartAngle(180);
    state->setEndAngle(0);
    while (xa < xb)
    {
      state->setXRadius(xc);
      state->setXRadius(xc);
      state->addArc();
      xa += 2 * xc;
    }
    xb -= 1;
    state->lineTo(xb, 0);
    state = state->restore();
    doContent();
  }
}

void XWTikzShape::makecontactIECShape(bool dopath)
{
  double x = state->outerXSep + 0.5 * state->minWidth;
  double y = state->outerYSep + state->minHeight;
  northEast.setX(x);
  northEast.setY(y);

  x = -0.5 * state->outerXSep - 0.5 * state->minWidth;
  y = -state->outerYSep;
  southWest.setX(x);
  southWest.setY(y);

  centerPos.setX(0);
  centerPos.setY(0);

  westPos.setX(southWest.x() - state->left);
  westPos.setY(0.5 * southWest.y() + 0.5 * northEast.y());
  eastPos.setX(northEast.x());
  eastPos.setY(0.5 * southWest.y() + 0.5 * northEast.y());
  northPos.setX(0.5 * northEast.x() + 0.5 * southWest.x());
  northPos.setY(northEast.y() + state->above);
  southPos.setX(0.5 * southWest.x() + 0.5 * northEast.x());
  southPos.setY(southWest.y() - state->below);

  switch (state->anchor)
  {
    default:
      {
        double delta = state->angle;
        double xc = cos(delta);
        double yc = sin(delta);
        QPointF pd(xc,yc);
        if (yc < 0)
        {
          double xa = southWest.x();
          double ya = southWest.y() - 0.5;          
          QPointF ur(-xa, -ya);
          QPointF p = pointBorderRectangle(pd, ur);
          x = p.x();
          y = p.y();
        }
        else
        {
          QPointF p = pointBorderRectangle(pd, northEast);
          x = p.x();
          y = p.y();
        }
      }
      break;

    case PGFtext:
      anchorPos.setX(0);
      anchorPos.setY(0);
      break;

    case PGFcenter:
      x = 0;
      y = 0;
      break;

    case PGFmid:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = 0.5 * 4.3;
      break;

    case PGFbase:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = 0;
      break;

    case PGFnorth:
    case PGFabove:
      x = 0.5 * northEast.x() + 0.5 * southWest.x();
      y = northEast.y() + state->above;
      break;

    case PGFsouth:
    case PGFbelow:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = southWest.y() - state->below;
      break;

    case PGFwest:
    case PGFleft:
      x = southWest.x() - state->left;
      y = 0.5 * southWest.y() + 0.5 * northEast.y();
      break;

    case PGFmidwest:
      x = southWest.x();
      y = 0.5 * 4.3;
      break;

    case PGFbasewest:
      x = southWest.x();
      y = 0;
      break;

    case PGFnorthwest:
      x = southWest.x();
      y = northEast.y();
      break;

    case PGFsouthwest:
      x = southWest.x();
      y = southWest.y();
      break;

    case PGFeast:
    case PGFright:
      x = northEast.x();
      y = 0.5 * southWest.y() + 0.5 * northEast.y();
      break;

    case PGFmideast:
      x = northEast.x();
      y = 0.5 * 4.3;
      break;

    case PGFbaseeast:
      x = northEast.x();
      y = 0;
      break;

    case PGFnortheast:
      x = northEast.x();
      y = northEast.y();
      break;

    case PGFsoutheast:
      x = northEast.x();
      y = southWest.y();
      break;

    case PGFinput:
      x = northEast.x();
      y = 0.5 * (northEast.y() + southWest.y());
      break;

    case PGFoutput:
      x = southWest.x();
      y = 0.5 * (northEast.y() + southWest.y());
      break;
  }
  anchorPos.setX(x);
  anchorPos.setY(y);

  if (dopath)
  {
    x = northEast.x() - state->outerXSep;
    y = northEast.y() - state->outerYSep;
    double xa = -x;
    double ya = 0;
    double xb = x;
    double yb = y;
    state = state->save(false);
    state->moveTo(xa, ya);
    state->lineTo(xb, yb);
    state = state->restore();
    doContent();
  }
}

void XWTikzShape::varmakecontactIECShape(bool dopath)
{
  double x = state->outerXSep + 0.5 * state->minWidth;
  double y = state->outerYSep + state->minHeight;
  northEast.setX(x);
  northEast.setY(y);

  x = -state->outerXSep - 0.5 * state->minWidth;
  y = -state->outerYSep + 0.083333 * x;
  southWest.setX(x);
  southWest.setY(y);

  centerPos.setX(0);
  centerPos.setY(0);

  westPos.setX(southWest.x() - state->left);
  westPos.setY(0.5 * southWest.y() + 0.5 * northEast.y());
  eastPos.setX(northEast.x());
  eastPos.setY(0.5 * southWest.y() + 0.5 * northEast.y());
  northPos.setX(0.5 * northEast.x() + 0.5 * southWest.x());
  northPos.setY(northEast.y() + state->above);
  southPos.setX(0.5 * southWest.x() + 0.5 * northEast.x());
  southPos.setY(southWest.y() - state->below);

  switch (state->anchor)
  {
    default:
      {
        double delta = state->angle;
        double xc = cos(delta);
        double yc = sin(delta);
        QPointF pd(xc,yc);
        if (yc < 0)
        {
          double xa = southWest.x();
          double ya = southWest.y();          
          QPointF ur(-xa, -ya);
          QPointF p = pointBorderRectangle(pd, ur);
          x = p.x();
          y = p.y();
        }
        else
        {
          QPointF p = pointBorderRectangle(pd, northEast);
          x = p.x();
          y = p.y();
        }
      }
      break;

    case PGFtext:
      anchorPos.setX(0);
      anchorPos.setY(0);
      break;

    case PGFcenter:
      x = 0;
      y = 0;
      break;

    case PGFmid:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = 0.5 * 4.3;
      break;

    case PGFbase:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = 0;
      break;

    case PGFnorth:
    case PGFabove:
      x = 0.5 * northEast.x() + 0.5 * southWest.x();
      y = northEast.y() + state->above;
      break;

    case PGFsouth:
    case PGFbelow:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = southWest.y() - state->below;
      break;

    case PGFwest:
    case PGFleft:
      x = southWest.x() - state->left;
      y = 0.5 * southWest.y() + 0.5 * northEast.y();
      break;

    case PGFmidwest:
      x = southWest.x();
      y = 0.5 * 4.3;
      break;

    case PGFbasewest:
      x = southWest.x();
      y = 0;
      break;

    case PGFnorthwest:
      x = southWest.x();
      y = northEast.y();
      break;

    case PGFsouthwest:
      x = southWest.x();
      y = southWest.y();
      break;

    case PGFeast:
    case PGFright:
      x = northEast.x();
      y = 0.5 * southWest.y() + 0.5 * northEast.y();
      break;

    case PGFmideast:
      x = northEast.x();
      y = 0.5 * 4.3;
      break;

    case PGFbaseeast:
      x = northEast.x();
      y = 0;
      break;

    case PGFnortheast:
      x = northEast.x();
      y = northEast.y();
      break;

    case PGFsoutheast:
      x = northEast.x();
      y = southWest.y();
      break;

    case PGFinput:
      x = northEast.x();
      y = 0.5 * (northEast.y() + southWest.y());
      break;

    case PGFoutput:
      x = southWest.x();
      y = 0.5 * (northEast.y() + southWest.y());
      break;
  }
  anchorPos.setX(x);
  anchorPos.setY(y);

  if (dopath)
  {
    x = northEast.x() - state->outerXSep;
    y = northEast.y() - state->outerYSep;
    double xa = -x;
    double ya = 0;
    double xb = x;
    double yb = y;
    double xc = xa;
    double tempdima = 0.083333 * 2 * xb;
    tempdima += xa;
    state = state->save(false);
    QPointF c(xa, ya);
    state->addCircle(c, tempdima);
    QVector2D v(xa - xb, -yb);
    v.normalize();
    xc = v.x();
    double yc = v.y();
    state->moveTo(xa + tempdima * xc, tempdima * yc);
    state->lineTo(xb, yb);
    state = state->restore();
    doContent();
  }
}

void XWTikzShape::varresistorIECShape(bool dopath)
{
  double x = boxWidth + 2 * state->innerXSep;
  if (x < state->minWidth)
    x = state->minWidth;
  x = 0.5 * x + 0.5 * boxWidth + state->outerXSep;
  double y = boxHeight + boxDepth + 2 * state->innerYSep;
  if (y < state->minHeight)
    y = state->minHeight;
  y = 0.5 * y - 0.5 * boxDepth + 0.5 * boxHeight + state->outerYSep;
  northEast.setX(x);
  northEast.setY(y);

  x = boxWidth + 2 * state->innerXSep;
  if (x < state->minWidth)
    x = state->minWidth;
  x = -0.5 * x + 0.5 * boxWidth - state->outerXSep;
  y = boxHeight + boxDepth + 2 * state->innerYSep;
  if (y < state->minHeight)
    y = state->minHeight;
  y = -0.5 * y - 0.5 * boxDepth + 0.5 * boxHeight - state->outerYSep;
  southWest.setX(x);
  southWest.setY(y);

  centerPos.setX(0.5 * southWest.x() + 0.5 * northEast.x());
  centerPos.setY(0.5 * southWest.y() + 0.5 * northEast.y());

  westPos.setX(southWest.x() - state->left);
  westPos.setY(0.5 * southWest.y() + 0.5 * northEast.y());
  eastPos.setX(northEast.x());
  eastPos.setY(0.5 * southWest.y() + 0.5 * northEast.y());
  northPos.setX(0.5 * northEast.x() + 0.5 * southWest.x());
  northPos.setY(northEast.y() + state->above);
  southPos.setX(0.5 * southWest.x() + 0.5 * northEast.x());
  southPos.setY(southWest.y() - state->below);

  switch (state->anchor)
  {
    default:
      {
        double delta = state->angle;
        double xb = cos(delta);
        double yb = sin(delta);
        double xa = southWest.x();
        double ya = southWest.y();
        double xc = 0.5 * (northEast.x() - xa);
        double yc = 0.5 * (northEast.y() - ya);
        xa += xc;
        ya += yc;
        QPointF pd(xb,yb);
        QPointF ur(xc,yc);
        QPointF p = pointBorderRectangle(pd,ur);
        x = p.x() + xa;
        y = p.y() + ya;
      }
      break;

    case PGFtext:
      anchorPos.setX(0);
      anchorPos.setY(0);
      break;

    case PGFcenter:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = 0.5 * southWest.y() + 0.5 * northEast.y();
      break;

    case PGFmid:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = 0.5 * 4.3;
      break;

    case PGFbase:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = 0;
      break;

    case PGFnorth:
    case PGFabove:
      x = 0.5 * northEast.x() + 0.5 * southWest.x();
      y = northEast.y() + state->above;
      break;

    case PGFsouth:
    case PGFbelow:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = southWest.y() - state->below;
      break;

    case PGFwest:
    case PGFleft:
      x = southWest.x() - state->left;
      y = 0.5 * southWest.y() + 0.5 * northEast.y();
      break;

    case PGFmidwest:
      x = southWest.x();
      y = 0.5 * 4.3;
      break;

    case PGFbasewest:
      x = southWest.x();
      y = 0;
      break;

    case PGFnorthwest:
      x = southWest.x();
      y = northEast.y();
      break;

    case PGFsouthwest:
      x = southWest.x();
      y = southWest.y();
      break;

    case PGFeast:
    case PGFright:
      x = northEast.x();
      y = 0.5 * southWest.y() + 0.5 * northEast.y();
      break;

    case PGFmideast:
      x = northEast.x();
      y = 0.5 * 4.3;
      break;

    case PGFbaseeast:
      x = northEast.x();
      y = 0;
      break;

    case PGFnortheast:
      x = northEast.x();
      y = northEast.y();
      break;

    case PGFsoutheast:
      x = northEast.x();
      y = southWest.y();
      break;

    case PGFinput:
      x = northEast.x();
      y = 0.5 * (northEast.y() + southWest.y());
      break;

    case PGFoutput:
      x = southWest.x();
      y = 0.5 * (northEast.y() + southWest.y());
      break;
  }
  anchorPos.setX(x);
  anchorPos.setY(y);

  if (dopath)
  {
    double xa = southWest.x() + state->outerXSep;
    double ya = southWest.y() + state->outerYSep;
    double xb = northEast.x() - state->outerXSep;
    double yb = northEast.y() - state->outerYSep;
    double yc = 0.5 * ya + 0.5 * yb;
    double xc = -0.5 * ya + 0.5 * yb;

    state = state->save(false);
    state->moveTo(xa, yc);
    xb += 0.1;
    xa += 4 * xc;
    while (xa < xb)
    {
      xa -= 3 * xc;
      state->lineTo(xa, yb);
      xa += 2 * xc;
      state->lineTo(xa, ya);
      xa += xc;
      state->lineTo(xa, yc);
      xa += 4 * xc;
    }
    xb -= 0.1;
    state->lineTo(xb, yc);
    state = state->restore();
    doContent();
  }
}
