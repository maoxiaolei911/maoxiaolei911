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
#include "XWTikzArrow.h"
#include "XWTikzState.h"
#include "XWTikzTextBox.h"
#include "XWTikzShape.h"

void XWTikzShape::circleeeShape(bool dopath)
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
        double xa = state->anotherPoint.x();
        double ya = state->anotherPoint.x();
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
    state = state->save();
    if (state->outerXSep < state->outerYSep)
      radius = radius - state->outerYSep;
    else
      radius = radius - state->outerXSep;
    state->moveTo(centerPoint);
    state->addCircle(centerPoint,radius);  
    state = state->restore();
    doContent();
  }
}

void XWTikzShape::directioneeShape(bool dopath)
{
  double x,y;
  northEast.setX(0);
  northEast.setY(0.5 * state->minHeight);

  southWest.setX(-state->getArrowTotalLength(PGFdirectionee));
  southWest.setY(-0.5 * state->minHeight);

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
        double xb = state->anotherPoint.x();
        double yb = state->anotherPoint.y();
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
    centerPoint.setX(0.5 * southWest.x() + 0.5 * northEast.x());
    centerPoint.setY(0.5 * southWest.y() + 0.5 * northEast.y());
    QPointF ll(xa,ya);
    QPointF ur(xb,yb);

    state = state->save();
    state->moveTo(southWest.x(), 0);
    state->lineTo(northEast.x(), 0);
    state->setDraw(true);
    state = state->restore();

    state = state->save();
    state->moveTo(ll);
    state->addRectangle(ll,ur);
    state = state->restore();
      
    state = state->save();
    XWTikzArrow arrow(PGFdirectionee);
    arrow.setup(state);
    arrow.draw(state);
    state = state->restore();

    doContent();
  }
}

void XWTikzShape::rectangleeeShape(bool dopath)
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
        double xb = state->anotherPoint.x();
        double yb = state->anotherPoint.y();
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
    centerPoint.setX(0.5 * southWest.x() + 0.5 * northEast.x());
    centerPoint.setY(0.5 * southWest.y() + 0.5 * northEast.y());
    QPointF ll(xa,ya);
    QPointF ur(xb,yb);

    state = state->save();
    state->moveTo(ll);
    state->addRectangle(ll,ur);
    state = state->restore();
    doContent();
  }
}
