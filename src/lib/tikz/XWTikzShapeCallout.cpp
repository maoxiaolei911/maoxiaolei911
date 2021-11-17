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

#define XW_TIKZ_SHAPE_CALLOUT_POINTANCHOR \
  tempdimb = outersep; \
  mathresult = state->angleBetweenLines(calloutpointer,aftercalloutpointer,calloutpointer,beforecalloutpointer); \
  mathresult = mathresult / 2; \
  tempdima = mathresult; \
  mathresult = 1 / sin(mathresult); \
  tempdimb *= mathresult; \
  mathresult = state->angleBetweenPoints(calloutpointer,aftercalloutpointer); \
  tempdima += mathresult; \
  tempdima += 180; \
  tempdimb *= state->calloutPointerAnchorAspect; \
  x = tempdimb * cos(tempdima) + calloutpointer.x(); \
  y = tempdimb * sin(tempdima) + calloutpointer.y(); \
  QPointF calloutpointeranchor(x,y);

void XWTikzShape::cloudCalloutShape(bool dopath)
{
  cloudShape(dopath);
}

void XWTikzShape::ellipseCalloutShape(bool dopath)
{
  double x = state->outerXSep;
  double y = state->outerYSep;
  double outersep = y;
  if (x > y)
    outersep = x;

  x = state->innerXSep + boxWidth;
  x *= 1.4142136;
  double xa = state->minWidth;
  if (x < 0.5 * xa)
    x = 0.5 * xa;

  y = state->innerYSep + 0.5 * (boxHeight + boxDepth);
  y *= 1.4142136;
  double ya = state->minHeight;
  if (y < 0.5 * ya)
    y = 0.5 * ya;

  double xpathradius = x;
  double ypathradius = y;

  x += state->outerXSep;
  y += state->outerYSep;
  double xradius = x;
  double yradius = y;
  double mathresult = 0;

  double pointerarc = state->calloutPointerArc;
  QPointF calloutpointer;
  if (state->isCalloutAbsolutePointer)
    calloutpointer = state->calloutAbsolutePointer;
  else
  {
    QPointF p(xpathradius,ypathradius);
    QPointF borderpoint = pointBorderEllipse(state->calloutRelativePointer,p);

    QPointF o(0,0);
    double mathresult = state->angleBetweenPoints(o,borderpoint);
    double pointerangle = mathresult;
    mathresult = state->veclen(state->calloutRelativePointer.x(),state->calloutRelativePointer.y());
    double pointerradius = mathresult;
    double xa = pointerradius * cos(pointerangle) + borderpoint.x();
    double ya = pointerradius * sin(pointerangle) + borderpoint.y();

    x = centerPoint.x() + xa;
    y = centerPoint.y() + ya;
    calloutpointer.setX(x);
    calloutpointer.setY(y);
  }

  XW_TIKZ_SHAPE_CALLOUT_SHORTERNPOINT
  
  QPointF pd = calloutpointer - centerPoint;
  QPointF ur(xpathradius,ypathradius);
  QPointF borderpoint = pointBorderEllipse(pd,ur);
  mathresult = angleOnEllipse(borderpoint,ur);
  double tempdima = pointerarc;
  double tempdimb = mathresult - 0.5 * tempdima;
  if (tempdimb < 0)
    tempdimb += 360;
  double beforecalloutangle = tempdimb;
  tempdimb += tempdima;
  if (tempdimb >= 360)
    tempdimb -= 360;
  double aftercalloutangle = tempdimb;
  x = xpathradius * cos(beforecalloutangle) + centerPoint.x();
  y = ypathradius * sin(beforecalloutangle) + centerPoint.y();
  QPointF beforecalloutpointer(x,y);

  x = xpathradius * cos(aftercalloutangle) + centerPoint.x();
  y = ypathradius * sin(aftercalloutangle) + centerPoint.y();
  QPointF aftercalloutpointer(x,y);

  XW_TIKZ_SHAPE_CALLOUT_POINTANCHOR
  
  switch (state->anchor)
  {
    default:
      {
        double xa = state->anotherPoint.x();
        double ya = state->anotherPoint.y();
        QPointF pd(xa,ya);
        QPointF ur(xradius,yradius);

        QPointF p = pointBorderEllipse(pd,ur);
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

    case PGFbase:
      anchorPos = basePoint;
      break;

    case PGFmid:
      anchorPos = midPoint;
      break;

    case PGFmideast:
      {
        QPointF p(midPoint.x() + xradius,midPoint.y());
        anchorPos = pointIntersectionOfLineAndArc(p,midPoint,centerPoint,270,450,xradius,yradius);
      }
      break;

    case PGFmidwest:
      {
        QPointF p(midPoint.x() - xradius,midPoint.y());
        anchorPos = pointIntersectionOfLineAndArc(p,midPoint,centerPoint,90,270,xradius,yradius);
      }
      break;

    case PGFbaseeast:
      {
        QPointF p(basePoint.x() + xradius,basePoint.y());
        anchorPos = pointIntersectionOfLineAndArc(p,basePoint,centerPoint,270,450,xradius,yradius);
      }
      break;

    case PGFbasewest:
      {
        QPointF p(basePoint.x() - xradius,basePoint.y());
        anchorPos = pointIntersectionOfLineAndArc(p,basePoint,centerPoint,90,270,xradius,yradius);
      }
      break;

    case PGFnorth:
      anchorPos.setX(centerPoint.x());
      anchorPos.setX(centerPoint.y() + yradius);
      break;

    case PGFsouth:
      anchorPos.setX(centerPoint.x());
      anchorPos.setX(centerPoint.y() - yradius);
      break;

    case PGFeast:
      anchorPos.setX(centerPoint.x() + xradius);
      anchorPos.setX(centerPoint.y());
      break;

    case PGFwest:
      anchorPos.setX(centerPoint.x() - xradius);
      anchorPos.setX(centerPoint.y());
      break;

    case PGFnorthwest:
      anchorPos.setX(centerPoint.x() - 0.7071067 * xradius);
      anchorPos.setX(centerPoint.y() + 0.7071067 * yradius);
      break;

    case PGFnortheast:
      anchorPos.setX(centerPoint.x() + 0.7071067 * xradius);
      anchorPos.setX(centerPoint.y() + 0.7071067 * yradius);
      break;

    case PGFsouthwest:
      anchorPos.setX(centerPoint.x() - 0.7071067 * xradius);
      anchorPos.setX(centerPoint.y() - 0.7071067 * yradius);
      break;

    case PGFsoutheast:
      anchorPos.setX(centerPoint.x() + 0.7071067 * xradius);
      anchorPos.setX(centerPoint.y() - 0.7071067 * yradius);
      break;

    case PGFpointer:
      anchorPos = calloutpointeranchor;
      break;
  }

  if (dopath)
  {
    state = state->save(false);
    state->moveTo(calloutpointer);
    state->lineTo(aftercalloutpointer);
    if (aftercalloutangle < beforecalloutangle)
      state->addArc(aftercalloutangle,beforecalloutangle,xpathradius,ypathradius);
    else
    {
      state->addArc(aftercalloutangle,360,xpathradius,ypathradius);
      state->addArc(0,beforecalloutangle,xpathradius,ypathradius);
    }

    state->closePath();
    state = state->restore();

    doContent();
  }
}

void XWTikzShape::rectangleCalloutShape(bool dopath)
{
  double x = 0.5 * boxWidth;
  double xa = state->minWidth;
  if (x < 0.5 * xa)
    x = 0.5 * xa;
  double xtemp = x;
  x += state->outerXSep;

  double y = 0.5 * (boxHeight + boxDepth);
  double ya = state->minHeight;
  if (y < 0.5 * ya)
    y = 0.5 * ya;
  double ytemp = y;
  y += state->outerYSep;
  double xlength = x;
  double ylength = y;
  double pointerwidth = state->calloutPointerWidth;
  double mathresult = 0;

  QPointF calloutpointer;
  if (state->isCalloutAbsolutePointer)
    calloutpointer = state->calloutAbsolutePointer;
  else
  {
    QPointF p(xtemp,ytemp);
    QPointF borderpoint = pointBorderRectangle(state->calloutRelativePointer,p);
    QPointF o(0,0);
    mathresult = state->angleBetweenPoints(o,borderpoint);
    double pointerangle = mathresult;
    mathresult = state->veclen(state->calloutRelativePointer.x(),state->calloutRelativePointer.y());
    double pointerradius = mathresult;
    x = centerPoint.x() + pointerradius * cos(pointerangle);
    y = centerPoint.y() + pointerradius * sin(pointerangle);
    calloutpointer.setX(x);
    calloutpointer.setY(y);
  }

  {
    XW_TIKZ_SHAPE_CALLOUT_SHORTERNPOINT
  }

  QPointF pd = calloutpointer - centerPoint;
  QPointF ur(xtemp,ytemp),o(0,0);
  QPointF borderpoint = pointBorderRectangle(pd,ur);
  mathresult = state->angleBetweenPoints(o,borderpoint);
  double borderangle = mathresult;
  double tempdima = pointerwidth;
  xa = xtemp - tempdima;
  ya = ytemp - tempdima;

  double xb = borderpoint.x();
  double yb = borderpoint.y();
  double xc = 0;
  double yc = 0;
  mathresult = state->angleBetweenPoints(o,ur);

  x = state->outerXSep;
  y = state->outerYSep;
  double outersep = y;
  if (x > y)
    outersep = x;

  if (borderangle < mathresult)
  {
    yc = 0.5 * tempdima;
    if (yb > ya)
      yb = ya;
  }
  else
  {
    QPointF p(-xtemp,ytemp);
    mathresult = state->angleBetweenPoints(o,p);
    if (borderangle < mathresult)
    {
      xc = 0.5 * tempdima;
      if (xb > xa)
        xb = xa;
      else
      {
        if (xb < -xa)
          xb = -xa;
      }
    }
    else
    {
      QPointF pp(-xtemp,-ytemp);
      mathresult = state->angleBetweenPoints(o,pp);
      if (borderangle < mathresult)
      {
        yc = 0.5 * tempdima;
        if (yb > ya)
          yb = ya;
        else
        {
          if (yb < -ya)
            yb = -ya;
        }
      }
      else
      {
        QPointF ppp(xtemp,-ytemp);
        mathresult = state->angleBetweenPoints(o,ppp);
        if (borderangle < mathresult)
        {
          xc = 0.5 * tempdima;
          if (xb > xa)
            xb = xa;
          else
          {
            if (xb < -xa)
              xb = -xa;
          }
        }
        else
        {
          yc = 0.5 * tempdima;
          if (yb < -ya)
            yb = -ya;
        }
      }
    }
  }

  QPointF beforecalloutpointer(centerPoint.x() + xb - xc,centerPoint.y() + yb - yc);
  QPointF aftercalloutpointer(centerPoint.x() + xb + xc,centerPoint.y() + yb + yc);

  double tempdimb = 0;
  XW_TIKZ_SHAPE_CALLOUT_POINTANCHOR

  switch (state->anchor)
  {
    default:
      {
        xa = state->anotherPoint.x() + centerPoint.x();
        ya = state->anotherPoint.y() + centerPoint.y();
        QPointF pd(xa,ya);
        QPointF ur(xlength,ylength);

        QPointF p = pointBorderRectangle(pd,ur);
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

    case PGFmid:
      anchorPos = midPoint;
      break;

    case PGFmideast:
      anchorPos.setX(midPoint.x() + xlength);
      anchorPos.setY(midPoint.y());
      break;

    case PGFmidwest:
      anchorPos.setX(midPoint.x() - xlength);
      anchorPos.setY(midPoint.y());
      break;

    case PGFbase:
      anchorPos = basePoint;
      break;

    case PGFbaseeast:
      anchorPos.setX(basePoint.x() + xlength);
      anchorPos.setY(basePoint.y());
      break;

    case PGFbasewest:
      anchorPos.setX(basePoint.x() - xlength);
      anchorPos.setY(basePoint.y());
      break;

    case PGFnorth:
      anchorPos.setX(centerPoint.x() + ylength);
      anchorPos.setY(centerPoint.y());
      break;

    case PGFsouth:
      anchorPos.setX(centerPoint.x() - ylength);
      anchorPos.setY(centerPoint.y());
      break;

    case PGFeast:
      anchorPos.setX(centerPoint.x() + xlength);
      anchorPos.setY(centerPoint.y());
      break;

    case PGFwest:
      anchorPos.setX(centerPoint.x() - xlength);
      anchorPos.setY(centerPoint.y());
      break;

    case PGFnortheast:
      anchorPos.setX(centerPoint.x() + xlength);
      anchorPos.setY(centerPoint.y() + ylength);
      break;

    case PGFsouthwest:
      anchorPos.setX(centerPoint.x() - xlength);
      anchorPos.setY(centerPoint.y() - ylength);
      break;

    case PGFsoutheast:
      anchorPos.setX(centerPoint.x() + xlength);
      anchorPos.setY(centerPoint.y() - ylength);
      break;

    case PGFnorthwest:
      anchorPos.setX(centerPoint.x() - xlength);
      anchorPos.setY(centerPoint.y() + ylength);
      break;

    case PGFpointer:
      anchorPos = calloutpointeranchor;
      break;
  }

  if (dopath)
  {
    xtemp = xlength - state->outerXSep;
    ytemp = ylength - state->outerYSep;
    QPointF firstpoint,secondpoint,thirdpoint,fourthpoint;
    if (borderangle < mathresult)
    {
      firstpoint.setX(xtemp);
      firstpoint.setY(ytemp);
      secondpoint.setX(-xtemp);
      secondpoint.setY(ytemp);
      thirdpoint.setX(-xtemp);
      thirdpoint.setY(-ytemp);
      fourthpoint.setX(xtemp);
      fourthpoint.setY(-ytemp);
    }
    else
    {
      QPointF p(-xtemp,ytemp);
      mathresult = state->angleBetweenPoints(o,p);
      if (borderangle < mathresult)
      {
        firstpoint.setX(-xtemp);
        firstpoint.setY(ytemp);
        secondpoint.setX(-xtemp);
        secondpoint.setY(-ytemp);
        thirdpoint.setX(xtemp);
        thirdpoint.setY(-ytemp);
        fourthpoint.setX(xtemp);
        fourthpoint.setY(ytemp);
      }
      else
      {
        QPointF pp(-xtemp,-ytemp);
        mathresult = state->angleBetweenPoints(o,pp);
        if (borderangle < mathresult)
        {
          firstpoint.setX(-xtemp);
          firstpoint.setY(-ytemp);
          secondpoint.setX(xtemp);
          secondpoint.setY(-ytemp);
          thirdpoint.setX(xtemp);
          thirdpoint.setY(ytemp);
          fourthpoint.setX(-xtemp);
          fourthpoint.setY(ytemp);
        }
        else
        {
          QPointF ppp(xtemp,-ytemp);
          mathresult = state->angleBetweenPoints(o,ppp);
          if (borderangle < mathresult)
          {
            firstpoint.setX(xtemp);
            firstpoint.setY(-ytemp);
            secondpoint.setX(xtemp);
            secondpoint.setY(ytemp);
            thirdpoint.setX(-xtemp);
            thirdpoint.setY(ytemp);
            fourthpoint.setX(-xtemp);
            fourthpoint.setY(-ytemp);
          }
          else
          {
            firstpoint.setX(xtemp);
            firstpoint.setY(ytemp);
            secondpoint.setX(-xtemp);
            secondpoint.setY(ytemp);
            thirdpoint.setX(-xtemp);
            thirdpoint.setY(-ytemp);
            fourthpoint.setX(xtemp);
            fourthpoint.setY(-ytemp);
          }
        }
      }
    }

    state = state->save(false);
    state->roundedCorners = 0;
    state->moveTo(beforecalloutpointer);
    state->lineTo(calloutpointer);
    state->lineTo(aftercalloutpointer);
    state->shift(centerPoint.x(),centerPoint.y());
    state->lineTo(firstpoint);
    state->lineTo(secondpoint);
    state->lineTo(thirdpoint);
    state->lineTo(fourthpoint);
    state->closePath();
    state = state->restore();

    doContent();
  }
}
