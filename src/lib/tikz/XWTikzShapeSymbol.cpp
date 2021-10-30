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

void XWTikzShape::cloudShape(bool dopath)
{
  double anglestep = 360 / state->cloudPuffs;
  double x = 1.4142135 * 0.5 * boxWidth;
  double y = 1.4142135 * 0.5 * (boxHeight + boxDepth);
  double xc = x;
  double yc = y;
  if (!state->cloudIgnoresAspect)
  {
    xc = state->aspect * y;
    if (xc < x)
      xc = x;
    yc = xc * state->aspectInverse;
    if (yc < y)
    {
      yc = y;
      xc = state->aspect * y;
    }
  }

  double xinnerradius = xc;
  double yinnerradius = yc;
  double outersep = state->outerXSep;
  if (state->outerXSep < state->outerYSep)
    outersep = state->outerYSep;

  double tempangle = (180 - state->cloudPuffArc) / 2;
  double arcradiusquotient = 1 / (2 * cos(tempangle));
  //double archeightquotient = arcradiusquotient * (1 - sin(tempangle));
  double halfarcangle = 0.5 * state->cloudPuffArc;
  double temp = (1 - cos(halfarcangle)) / sin(halfarcangle);
  double k = sin(anglestep / 2) * temp;
  double coshalfanglestep = cos(anglestep / 2);
  x = xinnerradius;
  y = yinnerradius;
  double xa = coshalfanglestep * x + k * y;
  double ya = coshalfanglestep * y + k * x;
  if (ya < 0.5 * state->minHeight)
    ya = 0.5 * state->minHeight;
  if (xa < 0.5 * state->minWidth)
    xa = 0.5 * state->minWidth;
  double xouterradius = xa;
  double youterradius = ya;
  xc = k * k;
  yc = coshalfanglestep * coshalfanglestep;
  xc += yc;
  temp = 1 / xc;
  x = (coshalfanglestep * xa - k * ya) * temp;
  y = (coshalfanglestep * ya - k * xa) * temp;
  xinnerradius = x;
  yinnerradius = y;
  double quarterarc = state->cloudPuffArc / 4;
  double halfcomplementarc = (180 - state->cloudPuffArc) / 2;
  //double sechalfcomplementarc = 1 / cos(halfcomplementarc);
  double sinhalfcomplementarc = sin(halfcomplementarc);
  double sinquarterarc = sin(quarterarc);
  double cosquarterarc = cos(quarterarc);
  double tanquarterarc = sinquarterarc / cosquarterarc;
  switch (state->anchor)
  {
    default:
      {
        double externalx = state->anotherPoint.x() + centerPoint.x();
        double externaly = state->anotherPoint.y() + centerPoint.y();
        if (state->cloudAnchorsUseEllipse)
        {
          QPointF s(externalx,externaly),e(xouterradius,youterradius);
          anchorPos = pointBorderEllipse(s,e);
          anchorPos = centerPoint + anchorPos;
        }
        else
        {
          x = centerPoint.x() + externalx;
          y = centerPoint.y() + externaly;
          QPointF externalpoint(x,y);
          double externalangle = state->angleBetweenPoints(centerPoint,externalpoint);
          double halfanglestep = anglestep / 2;
          double endangle = 90 - halfanglestep;
          while (endangle >= -anglestep)
            endangle -= anglestep;
          double angle = 0;
          double lastangle = angle;
          QPointF o(0,0);
          while (externalangle <= angle)
          {
            endangle += anglestep;
            x = xinnerradius * cos(endangle);
            y = yinnerradius * sin(endangle);
            QPointF secondpoint(x,y);

            double angletemp = endangle + anglestep;
            x = xinnerradius * cos(angletemp);
            y = yinnerradius * sin(angletemp);
            QPointF thirdpoint(x,y);

            angletemp = endangle - anglestep;
            x = xinnerradius * cos(angletemp);
            y = yinnerradius * sin(angletemp);
            QPointF firstpoint(x,y);

            double anglealpha = state->angleBetweenPoints(firstpoint,secondpoint);
            double anglebeta = state->angleBetweenPoints(secondpoint,thirdpoint);
            double miterradius = outersep / sin(halfcomplementarc + (anglebeta - anglealpha) / 2);
            double miterangle = (anglealpha + anglebeta - 180) / 2;
            x = miterradius * cos(miterangle);
            y = miterradius * sin(miterangle);
            QPointF miterpoint(secondpoint.x() + x, secondpoint.y() + y);

            angle = state->angleBetweenPoints(o,miterpoint);

            if (angle < lastangle)
              angle += 360;

            lastangle = angle;            
          }

          endangle = (int)endangle % 360;
          double startangle = endangle - anglestep;
          if (startangle < 0)
            startangle += 360;
          x = xinnerradius * cos(startangle) + centerPoint.x();
          y = yinnerradius * sin(startangle) + centerPoint.y();
          QPointF arcstartpoint(x,y);

          x = xinnerradius * cos(endangle) + centerPoint.x();
          y = yinnerradius * sin(endangle) + centerPoint.y();
          QPointF arcendpoint(x,y);

          //pgf@sh@getcloudpuffparameters
          double arcslope = state->angleBetweenPoints(arcendpoint,arcstartpoint);
          x = state->veclen(arcendpoint.x()-arcstartpoint.x(), arcendpoint.y()-arcstartpoint.y());
          double halfchordlength = 0.5 * x;
          double arcradius = arcradiusquotient * x;
          double outerarcradius = outersep + x;
          double segmentheight = -sinhalfcomplementarc * x + x;
          x = -halfchordlength;
          y = segmentheight - arcradius;
          QPointF circlecenterpoint(x,y);
          QPointF oo = state->rotatePointAround(circlecenterpoint,o,arcslope);
          circlecenterpoint = arcstartpoint + oo;
          //pgf@sh@getcloudpuffparameters

          if (endangle < startangle)
          {
            x = externalangle + 180;
            x = (int)x % 360;
          }
          else
            x = externalangle;

          double s = halfcomplementarc;
          double e = s + state->cloudPuffArc;
          double n = (s + e) / 2;
          double m = 360;
          while (true)
          {
            double p = (e + s) / 2;
            if (p == s)
              break;

            double a = p + arcslope;
            QPointF arcpoint(outerarcradius * cos(a) + circlecenterpoint.x(),outerarcradius * sin(a) + circlecenterpoint.y());
            double q = state->angleBetweenPoints(centerPoint,arcpoint);
            if (endangle < startangle)
            {
              q += 180;
              q = (int)q % 360;
            }

            if (x == q)
              break;
            else
            {
              if (x < q)
                e = p;
              else
                s = p;
            }

            double t = qAbs(x - q);
            if (t < m)
            {
              m = t;
              n = p;
            }
          }

          double anchorangle = n + arcslope;
          double x = outerarcradius * cos(anchorangle) + circlecenterpoint.x();
          double y = outerarcradius * sin(anchorangle) + circlecenterpoint.y();
          anchorPos.setX(x);
          anchorPos.setY(y);
        }
      }
      break;

    case PGFpointer:
      {
        if (state->isCalloutAbsolutePointer)
          anchorPos = state->calloutAbsolutePointer;
        else
        {
          QPointF p(xouterradius,youterradius);
          QPointF borderpoint = pointBorderEllipse(state->calloutRelativePointer, p);
          QPointF o(0,0);
          double pointerangle = state->angleBetweenPoints(o,borderpoint);
          double pointerradius = state->veclen(state->calloutRelativePointer.x(),state->calloutRelativePointer.y());
          x = borderpoint.x() + pointerradius * cos(pointerangle) + centerPoint.x();
          y = borderpoint.x() + pointerradius * sin(pointerangle) + centerPoint.y();
          QPointF calloutpointer(x,y);
          double mathresult = 0;
          XW_TIKZ_SHAPE_CALLOUT_SHORTERNPOINT

          anchorPos = calloutpointer;
        }
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

    case PGFbase:
      anchorPos = basePoint;
      break;  

    case PGFnorth:
      anchorPos.setX(0);
      anchorPos.setY(youterradius);
      break;

    case PGFsouth:
      anchorPos.setX(0);
      anchorPos.setY(youterradius);
      break;

    case PGFeast:
      anchorPos.setX(xouterradius);
      anchorPos.setY(0);
      break;

    case PGFwest:
      anchorPos.setX(xouterradius);
      anchorPos.setY(0);
      break;

    case PGFnorthwest:
      anchorPos.setX((1-0.707106) * xouterradius);
      anchorPos.setY(youterradius * 0.707106);
      break;

    case PGFnortheast:
      anchorPos.setX(xouterradius * 0.707106);
      anchorPos.setY(0.707106 * youterradius);
      break;

    case PGFsouthwest:
      anchorPos.setX((1-0.707106) * xouterradius);
      anchorPos.setY(youterradius * (1-0.707106));
      break;

    case PGFsoutheast:
      anchorPos.setX(xouterradius * 0.707106);
      anchorPos.setY(youterradius * (1-0.707106));
      break;
  }

  if (dopath)
  {
    double angle = 90 - anglestep / 2;
    x = centerPoint.x() + xinnerradius * cos(angle);
    y = centerPoint.y() + yinnerradius * sin(angle);
    state = state->save(false);
    state->moveTo(x,y);
    QPointF arcfirstpoint(x,y);
    QPointF arcendpoint = arcfirstpoint;
    QPointF circlecenterpoint,arcstartpoint;
    QPointF o(0,0),oo;
    int puffs = (int)state->cloudPuffs;
    for (int i = 1; i <= puffs; i++)
    {
      arcstartpoint = arcendpoint;
      if (i == puffs)
        arcendpoint = arcstartpoint;
      else
      {
        angle += anglestep;
        x = centerPoint.x() + xinnerradius * cos(angle);
        y = centerPoint.y() + yinnerradius * sin(angle);
        arcendpoint.setX(x);
        arcendpoint.setY(y);
      }

      //pgf@sh@getcloudpuffparameters
      double arcslope = state->angleBetweenPoints(arcendpoint,arcstartpoint);
      x = state->veclen(arcendpoint.x()-arcstartpoint.x(), arcendpoint.y()-arcstartpoint.y());
      double halfchordlength = 0.5 * x;
      double arcradius = arcradiusquotient * x;
      //double outerarcradius = outersep + x;
      double segmentheight = -sinhalfcomplementarc * x + x;
      x = -halfchordlength;
      y = segmentheight - arcradius;
      circlecenterpoint.setX(x);
      circlecenterpoint.setY(y);
      oo = state->rotatePointAround(circlecenterpoint,o,arcslope);
      circlecenterpoint = arcstartpoint + oo;
      //pgf@sh@getcloudpuffparameters

      double arcrotate = 90 - quarterarc + arcslope;
      double sinarcrotate = sin(arcrotate);
      double cosarcrotate = cos(arcrotate);
      double controlscale = tanquarterarc * arcradius;
      x = 0.55228475 * sinquarterarc;
      y = 0.55228475 * cosquarterarc;
      x *= controlscale;
      y *= controlscale;
      double xa = cosarcrotate * x - sinarcrotate * y;
      double ya = cosarcrotate * y + sinarcrotate * x;
      x = arcstartpoint.x() + xa;
      y = arcstartpoint.y() + ya;
      QPointF controlone(x,y);
      QPointF arcmidpoint(-halfchordlength,segmentheight);
      oo = state->rotatePointAround(arcmidpoint,o,arcslope);
      arcmidpoint = arcstartpoint + oo;
      x = 0.55228475 * sinquarterarc;
      y = -0.55228475 * cosquarterarc;
      x *= controlscale;
      y *= controlscale;
      xa = cosarcrotate * x - sinarcrotate * y;
      ya = cosarcrotate * y + sinarcrotate * x;
      x = arcmidpoint.x() + xa;
      y = arcmidpoint.y() + ya;
      QPointF controltwo(x,y);
      state->curveTo(controlone,controltwo,arcmidpoint);

      arcrotate = quarterarc + 90 + arcslope;
      sinarcrotate = sin(arcrotate);
      cosarcrotate = cos(arcrotate);
      x = 0.55228475 * sinquarterarc;
      y = 0.55228475 * cosquarterarc;
      x *= controlscale;
      y *= controlscale;
      xa = cosarcrotate * x - sinarcrotate * y;
      ya = cosarcrotate * y + sinarcrotate * x;
      x = arcmidpoint.x() + xa;
      y = arcmidpoint.y() + ya;
      controlone.setX(x);
      controlone.setY(y);

      x = 0.55228475 * sinquarterarc;
      y = -0.55228475 * cosquarterarc;
      x *= controlscale;
      y *= controlscale;
      xa = cosarcrotate * x - sinarcrotate * y;
      ya = cosarcrotate * y + sinarcrotate * x;
      x = arcendpoint.x() + xa;
      y = arcendpoint.y() + ya;
      controltwo.setX(x);
      controltwo.setY(y);
      state->curveTo(controlone,controltwo,arcendpoint);
    }
    state->closePath();

    if (state->shape == PGFcloudcallout)
    {
      double mathresult = 0;
      QPointF ur(xouterradius,youterradius);
      QPointF calloutpointer;
      if (state->isCalloutAbsolutePointer)
        calloutpointer = state->calloutAbsolutePointer;
      else
      {
        QPointF borderpoint = pointBorderEllipse(state->calloutRelativePointer,ur);
        QPointF o(0,0);
        double pointerangle = state->angleBetweenPoints(o,borderpoint);
        double pointerradius = state->veclen(state->calloutRelativePointer.x(),state->calloutRelativePointer.y());
        x = borderpoint.x() + pointerradius * cos(pointerangle) + centerPoint.x();
        y = borderpoint.x() + pointerradius * sin(pointerangle) + centerPoint.y();
        calloutpointer.setX(x);
        calloutpointer.setY(y);
        XW_TIKZ_SHAPE_CALLOUT_SHORTERNPOINT
      }

      QPointF pd = centerPoint - calloutpointer;
      QPointF p = pointBorderEllipse(pd,ur);
      QPointF borderpoint = p + centerPoint;
      mathresult = state->angleBetweenPoints(calloutpointer,centerPoint);
      double sinpointerangle = sin(mathresult);
      double cospointerangle = cos(mathresult);
      x = 2 * xouterradius;
      y = 2 * yinnerradius;
      int stype = (int)(state->calloutPointerStartSize.x());
      double xb = 0;
      double yb = 0;
      switch (stype)
      {
        default:
          xb = state->calloutPointerStartSize.x();
          yb = state->calloutPointerStartSize.y();
          break;

        case XW_TIKZ_CPS_CALLOUT:
          xb = x * state->calloutPointerStartSize.y();
          yb = y * state->calloutPointerStartSize.y();
          break;

        case XW_TIKZ_CPS_SINGLE:
          xb = state->calloutPointerStartSize.y();
          yb = xb;
          break;
      }
      stype = (int)(state->calloutPointerEndSize.x());
      switch (stype)
      {
        default:
          xa = state->calloutPointerEndSize.x();
          ya = state->calloutPointerEndSize.y();
          break;

        case XW_TIKZ_CPS_CALLOUT:
          xa = x * state->calloutPointerEndSize.y();
          ya = y * state->calloutPointerEndSize.y();
          break;

        case XW_TIKZ_CPS_SINGLE:
          xa = state->calloutPointerEndSize.y();
          ya = xa;
          break;
      }
      xb -= xa;
      yb -= ya;
      int counta = state->calloutPointerSegments;
      xb /= counta;
      yb /= counta;

      p = borderpoint - calloutpointer;
      mathresult = state->veclen(p.x(), p.y());
      double tempdima = mathresult;
      tempdima /= counta;
      double tempdimb = 0;
      for (int i = 1; i <= counta; i++)
      {
        xa = 0.5 * xa;
        ya = 0.5 * ya;
        x = calloutpointer.x() + cospointerangle * tempdimb;
        y = calloutpointer.y() + sinpointerangle * tempdimb;

        QPointF c(x,y),a(xa,0),b(0,ya);
        state->addEllipse(c,a,b);
        xa += xb;
        ya += yb;
        tempdimb += tempdima;
      }
    }

    state = state->restore();

    doContent();
  }
}

void XWTikzShape::correctForbiddenSignShape(bool dopath)
{
  circleAnchor();
  if (dopath)
  {
    if (state->outerXSep < state->outerYSep)
      radius = radius - state->outerYSep;
    else
      radius = radius - state->outerXSep;
    state = state->save(false);
    state->moveTo(centerPoint);
    state->addCircle(centerPoint,radius);
    state = state->restore();

    doContent();

    QPointF p1(0.707107 * radius,-0.707107 * radius);
    QPointF p2(-0.707107 * radius,0.707107 * radius);
    p1 = centerPoint + p1;
    p2 = centerPoint + p2;
    state = state->save(false);
    state->moveTo(p1);
    state->lineTo(p2);
    state = state->restore();
  }
}

void XWTikzShape::forbiddenSignShape(bool dopath)
{
  circleAnchor();
  if (dopath)
  {
    if (state->outerXSep < state->outerYSep)
      radius = radius - state->outerYSep;
    else
      radius = radius - state->outerXSep;
    state = state->save(false);
    state->moveTo(centerPoint);
    state->addCircle(centerPoint,radius);
    state = state->restore();

    doContent();

    QPointF p1(-0.707107 * radius,-0.707107 * radius);
    QPointF p2(0.707107 * radius,0.707107 * radius);
    p1 = centerPoint + p1;
    p2 = centerPoint + p2;
    state = state->save(false);
    state->moveTo(p1);
    state->lineTo(p2);
    state = state->restore();
  }
}

void XWTikzShape::magneticTapeShape(bool dopath)
{
  double x = 0.5 * boxWidth + 2 * state->innerXSep;
  double y = 0.5 * (boxHeight + boxDepth) + 2 * state->innerYSep;
  if (y > x)
    x = y;
  x *= 1.414213;
  if (x < state->minWidth)
    x = state->minWidth;
  if (x < state->minHeight)
    x = state->minHeight;
  x *= 0.5;
  radius = x;
  double tailextend = qMax((double)(state->magneticTapeTailExtend),(double)0);
  double tailheight = x * qMin(qMax((double)(state->magneticTapeTail),(double)0), (double)1);
  double outersep = state->outerXSep;
  if (state->outerXSep < state->outerYSep)
    outersep = state->outerYSep;
  double outerradius = outersep + radius;
  double tailangle = 360 - asin((radius - tailheight) / radius);
  double tailbottomangle = 360 - atan(outerradius / (outerradius + tailextend));
  double tailtopangle = 360 - atan((outerradius - outersep - tailheight - outersep) / (outerradius + tailextend));

  switch (state->anchor)
  {
    default:
      {
        double xb = state->anotherPoint.x();
        double yb = state->anotherPoint.y();
        QPointF o(0,0), externalpoint(xb + centerPoint.x(),yb + centerPoint.y());
        double mathresult = state->angleBetweenPoints(o,externalpoint);
        if (mathresult < 270)
        {
          QPointF p(outerradius,outerradius);
          p = pointBorderEllipse(externalpoint,p);
          anchorPos = centerPoint + p;
        }
        else
        {
          if (mathresult < tailbottomangle)
          {
            QPointF p1(0,-outerradius), p2(outerradius,-outerradius);
            QPointF p = state->intersectionOfLines(externalpoint,o,p1,p2);
            anchorPos = centerPoint + p;
          }
          else
          {
            if (mathresult < tailtopangle)
            {
              QPointF p1(outerradius,outerradius), p2(outerradius,-outerradius);
              QPointF p = state->intersectionOfLines(externalpoint,o,p1,p2);
              anchorPos = centerPoint + p;
            }
            else
            {
              QPointF p(outerradius,outerradius);
              p = pointBorderEllipse(externalpoint,p);
              anchorPos = centerPoint + p;
            }
          }
        }
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

    case PGFbasewest:
      anchorPos.setX(centerPoint.x() - outerradius);
      anchorPos.setY(0);
      break;

    case PGFbaseeast:
      anchorPos.setX(centerPoint.x() + outerradius);
      anchorPos.setY(0);
      break;

    case PGFmidwest:
      anchorPos.setX(centerPoint.x() - outerradius);
      anchorPos.setY(5);
      break;

    case PGFmideast:
      anchorPos.setX(centerPoint.x() + outerradius);
      anchorPos.setY(5);
      break;

    case PGFnorth:
      anchorPos = centerPoint;
      anchorPos.setY(centerPoint.y() + outerradius);
      break;

    case PGFnortheast:
      anchorPos.setX(centerPoint.x() + outerradius * cos((double)45));
      anchorPos.setY(centerPoint.y() + outerradius * sin((double)45));
      break;

    case PGFsouth:
      anchorPos = centerPoint;
      anchorPos.setY(centerPoint.y() - outerradius);
      break;

    case PGFsoutheast:
      anchorPos.setX(centerPoint.x() + outerradius);
      anchorPos.setY(centerPoint.y() - outerradius);
      break;

    case PGFeast:
      anchorPos = centerPoint;
      anchorPos.setX(centerPoint.x() + outerradius);
      break;

    case PGFsouthwest:
      anchorPos.setX(centerPoint.x() + outerradius * cos((double)225));
      anchorPos.setY(centerPoint.y() + outerradius * sin((double)225));
      break;

    case PGFwest:
      anchorPos = centerPoint;
      anchorPos.setX(centerPoint.x() - outerradius);
      break;

    case PGFnorthwest:
      anchorPos.setX(centerPoint.x() + outerradius * cos((double)135));
      anchorPos.setY(centerPoint.y() + outerradius * sin((double)135));
      break;

    case PGFtaileast:
      anchorPos.setX(centerPoint.x() + outerradius + tailextend + outersep);
      anchorPos.setY(centerPoint.y() - outerradius + outersep + tailheight / 2);
      break;

    case PGFtailsoutheast:
      anchorPos.setX(centerPoint.x() + outerradius + tailextend + outersep);
      anchorPos.setY(centerPoint.y() - outerradius);
      break;

    case PGFtailnortheast:
      anchorPos.setX(centerPoint.x() + outerradius + tailextend + outersep);
      anchorPos.setY(centerPoint.y() - outerradius + outersep + tailheight + outersep);
      break;
  }

  if (dopath)
  {
    state = state->save(false);
    state->addShift(centerPoint.x(),centerPoint.y());
    x = radius * cos(tailangle);
    y = radius * sin(tailangle);
    state->moveTo(x,y);
    state->addArc(tailangle,360,radius,radius);
    state->addArc(0,270,radius,radius);
    state->lineTo(radius + tailextend, -radius);
    state->lineTo(radius + tailextend, -radius + tailheight);
    state->closePath();
    state = state->restore();

    doContent();
  }
}

void XWTikzShape::magnifyingGlassShape(bool dopath)
{
  circleShape(dopath);
  if (dopath)
  {
    doContent();

    if (state->outerXSep < state->outerYSep)
      radius = radius - state->outerYSep;
    else
      radius = radius - state->outerXSep;
    state = state->save(false);
    state->moveTo(centerPoint);
    state->addCircle(centerPoint,radius); 
    
    QPointF a(radius * cos(state->MGHA),radius * sin(state->MGHA));
    QPointF b((radius + radius * state->MGHAaspect) * cos(state->MGHA),(radius + radius * state->MGHAaspect) * sin(state->MGHA));

    QPointF p1 = centerPoint + a;
    QPointF p2 = centerPoint + b;
    state->moveTo(p1);
    state->lineTo(p2);
    state = state->restore();
  }
}

void XWTikzShape::signalShape(bool dopath)
{
  double halfpointerangle = state->signalPointerAngle / 2;
  double cosechalfpointerangle = 1 / sin(halfpointerangle);
  double quarterpointerangle = halfpointerangle / 2;
  double cosecquarterpointerangle = 1 / sin(quarterpointerangle);
  double secquarterpointerangle = 1 / cos(quarterpointerangle);
  double complementquarterpointerangle = 90 - quarterpointerangle;
  double x = state->outerXSep;
  double y = state->outerYSep;
  if (x > y)
    y = x;
  double yc = y;
  double pointerapexmiter = cosechalfpointerangle * y;
  double tocornermiter = secquarterpointerangle * y;
  double fromcornermiter = cosecquarterpointerangle * y;
  double xa = state->innerXSep + 0.5 * boxWidth;
  double ya = state->innerYSep + 0.5 * (boxHeight + boxDepth);
  double xb = ya * sin(90 - halfpointerangle) / cos(90 - halfpointerangle);
  double yb = xa * sin(90 - halfpointerangle) / cos(90 - halfpointerangle);
  double tempdima = 2 * ya;
  if (state->signalFrom == PGFnorth || 
      state->signalFrom == PGFabove || 
      state->signalTo == PGFnorth || 
      state->signalTo == PGFabove) 
  {
    tempdima += yb;
  }

  if (state->signalFrom == PGFsouth || 
      state->signalFrom == PGFbelow || 
      state->signalTo == PGFsouth || 
      state->signalTo == PGFbelow) 
  {
    tempdima += yb;
  }

  y = state->minHeight;
  if (tempdima < y)
  {
    tempdima = y;
    if (state->signalFrom == PGFnorth || 
      state->signalFrom == PGFabove || 
      state->signalTo == PGFnorth || 
      state->signalTo == PGFabove) 
    {
      tempdima -= yb;
    }

    if (state->signalFrom == PGFsouth || 
        state->signalFrom == PGFbelow || 
        state->signalTo == PGFsouth || 
        state->signalTo == PGFbelow) 
    {
      tempdima -= yb;
    }

    ya = 0.5 * tempdima;
    xb = ya * sin(90 - halfpointerangle) / cos(90 - halfpointerangle);
  }

  tempdima = 2 * xa;
  if (state->signalFrom == PGFeast || 
      state->signalFrom == PGFright || 
      state->signalTo == PGFeast || 
      state->signalTo == PGFright) 
  {
    tempdima += xb;
  }

  if (state->signalFrom == PGFwest || 
      state->signalFrom == PGFleft || 
      state->signalTo == PGFwest || 
      state->signalTo == PGFleft) 
  {
    tempdima += xb;
  }

  x = state->minWidth;
  if (tempdima < x)
  {
    tempdima = x;
    if (state->signalFrom == PGFeast || 
        state->signalFrom == PGFright || 
        state->signalTo == PGFeast || 
        state->signalTo == PGFright) 
    {
      tempdima -= xb;
    }

    if (state->signalFrom == PGFwest || 
        state->signalFrom == PGFleft || 
        state->signalTo == PGFwest || 
        state->signalTo == PGFleft) 
    {
      tempdima -= xb;
    }

    xa = 0.5 * tempdima;
    yb = xa * sin(90 - halfpointerangle) / cos(90 - halfpointerangle);
  }

  x = centerPoint.x();
  y = centerPoint.y() + ya;
  if (state->signalTo == PGFnorth || state->signalTo == PGFabove)
    y += yb;
  QPointF north(x,y);

  x = centerPoint.x();
  y = centerPoint.y() - ya;
  if (state->signalTo == PGFsouth || state->signalTo == PGFbelow)
    y -= yb;
  QPointF south(x,y);

  y = centerPoint.y();
  x = centerPoint.x() + xa;
  if (state->signalTo == PGFeast || state->signalTo == PGFright)
    x += xb;
  QPointF east(x,y);

  y = centerPoint.y();
  x = centerPoint.x() - xa;
  if (state->signalTo == PGFeast || state->signalTo == PGFright)
    x -= xb;
  QPointF west(x,y);

  x = centerPoint.x() + xa;
  y = centerPoint.y() + ya;
  if (state->signalFrom == PGFnorth || state->signalFrom == PGFabove)
    y += yb;
  if (state->signalFrom == PGFeast || state->signalFrom == PGFright)
    x += xb;
  QPointF northeast(x,y);

  x = centerPoint.x() + xa;
  y = centerPoint.y() - ya;
  if (state->signalFrom == PGFsouth || state->signalFrom == PGFbelow)
    y -= yb;
  if (state->signalFrom == PGFeast || state->signalFrom == PGFright)
    x += xb;
  QPointF southeast(x,y);

  x = centerPoint.x() - xa;
  y = centerPoint.y() - ya;
  if (state->signalFrom == PGFsouth || state->signalFrom == PGFbelow)
    y -= yb;
  if (state->signalFrom == PGFwest || state->signalFrom == PGFleft)
    x -= xb;
  QPointF southwest(x,y);

  x = centerPoint.x() - xa;
  y = centerPoint.y() + ya;
  if (state->signalFrom == PGFnorth || state->signalFrom == PGFabove)
    y += yb;
  if (state->signalFrom == PGFwest || state->signalFrom == PGFleft)
    x -= xb;
  QPointF northwest(x,y);

  x = 0;
  if (state->signalFrom == PGFnorth || 
      state->signalFrom == PGFabove || 
      state->signalTo == PGFnorth || 
      state->signalTo == PGFabove) 
  {
    y = pointerapexmiter;
  }
  else
    y = yc;
  QPointF northmiter(x,y);

  x = 0;
  if (state->signalFrom == PGFsouth || 
      state->signalFrom == PGFbelow || 
      state->signalTo == PGFsouth || 
      state->signalTo == PGFbelow) 
  {
    y = -pointerapexmiter;
  }
  else
    y = -yc;
  QPointF southmiter(x,y);

  y = 0;
  if (state->signalFrom == PGFeast || 
      state->signalFrom == PGFright || 
      state->signalTo == PGFeast || 
      state->signalTo == PGFright) 
  {
    x = pointerapexmiter;
  }
  else
    x = yc;
  QPointF eastmiter(x,y);

  y = 0;
  if (state->signalFrom == PGFwest || 
      state->signalFrom == PGFleft || 
      state->signalTo == PGFwest || 
      state->signalTo == PGFleft) 
  {
    x = -pointerapexmiter;
  }
  else
    x = -yc;
  QPointF westmiter(x,y);

  if (state->signalFrom == PGFeast || 
      state->signalFrom == PGFright || 
      state->signalTo == PGFeast || 
      state->signalTo == PGFright) 
  {
    if (state->signalFrom == PGFeast || state->signalFrom == PGFright)
    {
      x = fromcornermiter * cos(quarterpointerangle);
      y = fromcornermiter * sin(quarterpointerangle);
    }
    else
    {
      if (state->signalTo == PGFeast || state->signalTo == PGFright)
      {
        x = tocornermiter * cos(complementquarterpointerangle);
        y = tocornermiter * sin(complementquarterpointerangle);
      }
    }

    if (state->signalFrom == PGFnorth || state->signalFrom == PGFabove)
    {
      x = fromcornermiter * cos(complementquarterpointerangle);
      y = fromcornermiter * sin(complementquarterpointerangle);
    }
    else
    {
      if (state->signalTo == PGFnorth || state->signalTo == PGFabove)
      {
        x = tocornermiter * cos(quarterpointerangle);
        y = tocornermiter * sin(quarterpointerangle);
      }
    }
  }
  else
  {
    x = yc;
    y = yc;
  }
  QPointF northeastmiter(x,y);

  if (state->signalFrom == PGFeast || 
      state->signalFrom == PGFright || 
      state->signalTo == PGFeast || 
      state->signalTo == PGFright) 
  {
    if (state->signalFrom == PGFeast || state->signalFrom == PGFright)
    {
      x = fromcornermiter * cos(-quarterpointerangle);
      y = fromcornermiter * sin(-quarterpointerangle);
    }
    else
    {
      if (state->signalTo == PGFeast || state->signalTo == PGFright)
      {
        x = tocornermiter * cos(-complementquarterpointerangle);
        y = tocornermiter * sin(-complementquarterpointerangle);
      }

      if (state->signalFrom == PGFsouth || state->signalFrom == PGFbelow)
      {
        x = fromcornermiter * cos(-complementquarterpointerangle);
        y = fromcornermiter * sin(-complementquarterpointerangle);
      }
      else
      {
        if (state->signalTo == PGFsouth || state->signalTo == PGFbelow)
        {
          x = tocornermiter * cos(-quarterpointerangle);
          y = tocornermiter * sin(-quarterpointerangle);
        }
      }
    }
  }
  else
  {
    x = yc;
    y = yc;
  }
  QPointF southeastmiter(x,y);

  if (state->signalFrom == PGFwest || 
      state->signalFrom == PGFleft || 
      state->signalTo == PGFwest || 
      state->signalTo == PGFleft) 
  {
    if (state->signalFrom == PGFwest || state->signalFrom == PGFleft)
    {
      x = fromcornermiter * cos(180 + quarterpointerangle);
      y = fromcornermiter * sin(180 + quarterpointerangle);
    }
    else
    {
      if (state->signalTo == PGFwest || state->signalTo == PGFleft)
      {
        x = tocornermiter * cos(180 + complementquarterpointerangle);
        y = tocornermiter * sin(180 + complementquarterpointerangle);
      }
    }

    if (state->signalFrom == PGFsouth || state->signalFrom == PGFbelow)
    {
      x = fromcornermiter * cos(180 + complementquarterpointerangle);
      y = fromcornermiter * sin(180 + complementquarterpointerangle);
    }
    else
    {
      if (state->signalTo == PGFsouth || state->signalTo == PGFbelow)
      {
        x = tocornermiter * cos(180 + quarterpointerangle);
        y = tocornermiter * sin(180 + quarterpointerangle);
      }
    }
  }
  else
  {
    x = -yc;
    y = -yc;
  }
  QPointF southwestmiter(x,y);

  if (state->signalFrom == PGFwest || 
      state->signalFrom == PGFleft || 
      state->signalTo == PGFwest || 
      state->signalTo == PGFleft) 
  {
    if (state->signalFrom == PGFwest || state->signalFrom == PGFleft)
    {
      x = fromcornermiter * cos(180 - quarterpointerangle);
      y = fromcornermiter * sin(180 - quarterpointerangle);
    }
    else
    {
      if (state->signalTo == PGFwest || state->signalTo == PGFleft)
      {
        x = tocornermiter * cos(180 - complementquarterpointerangle);
        y = tocornermiter * sin(180 - complementquarterpointerangle);
      }
    }

    if (state->signalFrom == PGFnorth || state->signalFrom == PGFabove)
    {
      x = fromcornermiter * cos(180 - complementquarterpointerangle);
      y = fromcornermiter * sin(180 - complementquarterpointerangle);
    }
    else
    {
      if (state->signalTo == PGFnorth || state->signalTo == PGFabove)
      {
        x = tocornermiter * cos(180 - quarterpointerangle);
        y = tocornermiter * sin(180 - quarterpointerangle);
      }
    }
  }
  else
  {
    x = -yc;
    y = yc;
  }
  QPointF northwestmiter(x,y);

  QPointF anchornorth = north + northmiter;
  QPointF anchorsouth = south + southmiter;
  QPointF anchoreast = east + eastmiter;
  QPointF anchorwest = west + westmiter;
  QPointF anchornortheast = northeast + northeastmiter;
  QPointF anchorsoutheast = southeast + southeastmiter;
  QPointF anchorsouthwest = southwest + southwestmiter;
  QPointF anchornorthwest = northwest + northwestmiter;

  switch (state->anchor)
  {
    default:
      {
        double externalx = state->anotherPoint.x() + centerPoint.x();
        double externaly = state->anotherPoint.y() + centerPoint.y();
        QPointF e(externalx,externaly);
        double externalangle = state->angleBetweenPoints(centerPoint,e);
        double mathresult = state->angleBetweenPoints(centerPoint,anchorwest);
        QPointF firstpoint,secondpoint;
        if (externalangle < mathresult)
        {
          mathresult = state->angleBetweenPoints(centerPoint,anchornorth);
          if (externalangle < mathresult)
          {
            mathresult = state->angleBetweenPoints(centerPoint,anchornortheast);
            if (externalangle < mathresult)
            {
              firstpoint = anchoreast;
              secondpoint = anchornortheast;
            }
            else
            {
              secondpoint = anchornortheast;
              firstpoint = anchornorth;
            }
          }
        }
        else
        {
          mathresult = state->angleBetweenPoints(centerPoint,anchorsouth);
          if (externalangle < mathresult)
          {
            mathresult = state->angleBetweenPoints(centerPoint,anchorsouthwest);
            if (externalangle < mathresult)
            {
              firstpoint = anchorwest;
              secondpoint = anchorsouthwest;
            }
            else
            {
              secondpoint = anchorsouthwest;
              firstpoint = anchorsouth;
            }
          }
          else
          {
            mathresult = state->angleBetweenPoints(centerPoint,anchorsoutheast);
            if (externalangle < mathresult)
            {
              firstpoint = anchorsouth;
              secondpoint = anchorsoutheast;
            }
            else
            {
              secondpoint = anchorsoutheast;
              firstpoint = anchoreast;
            }
          }
        }
        anchorPos = state->intersectionOfLines(centerPoint,e,firstpoint,secondpoint);
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

    case PGFbaseeast:
      xa = anchoreast.x();
      ya = anchoreast.y();
      x = anchorsoutheast.x();
      y = anchorsoutheast.y();
      if (xa > x)
        tempdima = xa;
      else
        tempdima = x;
      {
        QPointF externalpoint(tempdima,basePoint.y());
        x = basePoint.x();
        y = basePoint.y();
        QPointF firstpoint = anchoreast;
        QPointF secondpoint;
        if (y < ya)
          secondpoint = anchorsoutheast;
        else
          secondpoint = anchornortheast;
        anchorPos = state->intersectionOfLines(midPoint,externalpoint,firstpoint,secondpoint);
      }
      break;

    case PGFbasewest:
      xa = anchorwest.x();
      ya = anchorwest.y();
      x = anchorsouthwest.x();
      y = anchorsouthwest.y();
      if (xa < x)
        tempdima = xa;
      else
        tempdima = x;
      {
        QPointF externalpoint(tempdima,basePoint.y());
        x = basePoint.x();
        y = basePoint.y();
        QPointF firstpoint = anchorwest;
        QPointF secondpoint;
        if (y < ya)
          secondpoint = anchorsouthwest;
        else
          secondpoint = anchornorthwest;
        anchorPos = state->intersectionOfLines(midPoint,externalpoint,firstpoint,secondpoint);
      }
      break;

    case PGFmid:
      anchorPos = midPoint;
      break;

    case PGFmideast:
      xa = anchoreast.x();
      ya = anchoreast.y();
      x = anchorsoutheast.x();
      y = anchorsoutheast.y();
      if (tempdima > xa)
        tempdima = xa;
      else
        tempdima = x;
      {
        QPointF externalpoint(tempdima,midPoint.y());
        x = midPoint.x();
        y = midPoint.y();
        QPointF firstpoint = anchoreast;
        QPointF secondpoint;
        if (y < ya)
          secondpoint = anchorsoutheast;
        else
          secondpoint = anchornortheast;
        anchorPos = state->intersectionOfLines(midPoint,externalpoint,firstpoint,secondpoint);
      }
      break;

    case PGFmidwest:
      {
        xa = anchorwest.x();
        ya = anchorwest.y();
        x = anchorsouthwest.x();
        y = anchorsouthwest.y();
        if (xa < x)
          tempdima = xa;
        else
          tempdima = x;

        QPointF e = midPoint;
        x = tempdima;
        e.setX(x);
        x = midPoint.x();
        y = midPoint.y();
        QPointF firstpoint = anchorwest;
        QPointF secondpoint;
        if (y < ya)
          secondpoint = anchorsouthwest;
        else
          secondpoint = anchornorthwest;

        anchorPos = state->intersectionOfLines(midPoint,e,firstpoint,secondpoint);
      }
      break;

    case PGFnorth:
      anchorPos = anchornorth;
      break;

    case PGFsouth:
      anchorPos = anchorsouth;
      break;

    case PGFeast:
      anchorPos = anchoreast;
      break;

    case PGFwest:
      anchorPos = anchorwest;
      break;

    case PGFnortheast:
      anchorPos = anchornortheast;
      break;

    case PGFsoutheast:
      anchorPos = anchorsoutheast;
      break;

    case PGFsouthwest:
      anchorPos = anchorsouthwest;
      break;

    case PGFnorthwest:
      anchorPos = anchornorthwest;
      break;
  }

  if (dopath)
  {
    state = state->save(false);
    state->moveTo(north);
    state->lineTo(northeast);
    state->lineTo(east);
    state->lineTo(southeast);
    state->lineTo(south);
    state->lineTo(southwest);
    state->lineTo(west);
    state->lineTo(northwest);
    state->closePath();
    state = state->restore();

    doContent();
  }
}

void XWTikzShape::starburstShape(bool dopath)
{
  int anglestep = 180 / state->starburstPoints;
  int totalpoints = 2 * state->starburstPoints;
  double outersep = state->outerXSep;
  if (state->outerXSep < state->outerYSep)
    outersep = state->outerYSep;

  double x = state->innerXSep + 0.5 * boxWidth;
  double y = state->outerYSep + 0.5 * (boxHeight + boxDepth);
  int rotate = 0;
  if (state->shapeBorderUsesIncircle)
  {
    rotate = (int)state->shapeBorderRotate;
    if (y > x)
      x = y;

    x = 1.41421 * x;
    y = x;
  }
  else
  {
    rotate = (int)state->shapeBorderRotate;
    int counta = rotate % 360;
    counta += 45;
    counta /= 90;
    counta *= 90;
    if (counta < 0)
      counta += 360;

    if (counta == 90)
    {
      double xc = x;
      x = y;
      y = xc;
    }
    else
    {
      if (counta == 270)
      {
        double xc = x;
        x = y;
        y = xc;
      }
    }

    rotate = counta;
    x = 1.41421 * x;
    y = 1.41421 * y;
  }

  double xa = x + state->starburstPointHeight;
  if (xa < 0.5 * state->minWidth)
    x = 0.5 * state->minWidth - state->starburstPointHeight;

  double ya = y + state->starburstPointHeight;
  if (ya < 0.5 * state->minHeight)
    y = 0.5 * state->minHeight - state->starburstPointHeight;

  double xinnerradius = x;
  double yinnerradius = y;
  if (y > x)
    x = y;

  x += state->starburstPointHeight;
  double externalradius = x;

  int angle = 90;
  int counta = 1;
  int countb = 0;
  int countc = totalpoints + 2;
  int looppoints = countc;
  QList<QPointF> points,borderpoints;
  QList<double> angletoborderpoints;
  QPointF firstpoint,secondpoint,thirdpoint,angletoborderpoint;
  for (int i = 1; i <= looppoints; i++)
  {
    firstpoint = secondpoint;
    secondpoint = thirdpoint;
    if (i <= totalpoints)
    {
      if ((i % 2) == 1)
      {
        if (state->randomStarburst == 0)
          xa = state->starburstPointHeight;
        else
        {
          x = state->starburstPointHeight;
          xa = 0.75 * x;
          double xb = 0.25 * x;
          xa = state->mathrnd(state->randomStarburst) * xa + xb;
        }

        x = xinnerradius + xa;
        y = yinnerradius + xa;
        x = centerPoint.x() + x * cos((double)angle);
        y = centerPoint.y() + y * sin((double)angle);
      }
      else
      {
        x = centerPoint.x() + xinnerradius * cos((double)angle);
        y = centerPoint.y() + yinnerradius * sin((double)angle);
      }
      thirdpoint.setX(x);
      thirdpoint.setY(y);
      points << thirdpoint;
    }

    if (!firstpoint.isNull())
    {
      double defaultmiterangle = state->angleBetweenLines(secondpoint,thirdpoint,secondpoint,firstpoint) / 2;
      double miterlength = outersep / sin(defaultmiterangle);

      double angletemp = 180 - state->angleBetweenLines(firstpoint,secondpoint,firstpoint,thirdpoint) - defaultmiterangle;
      double miterangle = angletemp - 180 + state->angleBetweenPoints(firstpoint,thirdpoint);

      x = miterlength * cos(miterangle) + secondpoint.x();
      y = miterlength * sin(miterangle) + secondpoint.y();

      QPointF borderpoint(x,y);    
      angletoborderpoints << state->angleBetweenPoints(centerPoint,borderpoint); 
      borderpoint = state->rotatePointAround(borderpoint,centerPoint,rotate);
      borderpoints << borderpoint;
      countc = countb;
      countc++;
      countc /= 2;
    }

    angle += anglestep;
    angle = angle % 360;
    counta++;
    if (counta > totalpoints)
      counta = 1;
    countb++;
    if (countb > totalpoints)
      countb = 1;
  }

  switch (state->anchor)
  {
    default:
      {
        double externalx = state->anotherPoint.x() + centerPoint.x();
        double externaly = state->anotherPoint.y() + centerPoint.y();
        QPointF e(externalx,externaly);
        double externalangle = state->angleBetweenPoints(centerPoint,e) - rotate;
        if (externalangle < 0)
          externalangle += 360;
        int first = 0;
        int second = 0;
        if (externalangle < 90)
        {
          counta = 0;
          countb = totalpoints;
          for (int i = 0; i < angletoborderpoints.size(); i++)
          {
            if (counta <= 0)
            {
              if (angletoborderpoints[i] > 90)
                counta = countb;
              else
              {
                if (externalangle > angletoborderpoints[i])
                  counta = countb;
              }
            }
            countb--;
          }

          first = counta;
          counta++;
          if (counta > totalpoints)
            counta++;
          second = counta;
        }
        else
        {
          counta = 0;
          for (int i = 0; i < angletoborderpoints.size(); i++)
          {
            if (counta <= 0)
            {
              if (angletoborderpoints[i] < 90)
                counta = i;
              else
              {
                if (externalangle < angletoborderpoints[i])
                  counta = i;
              }
            }
          }

          first = counta;
          counta--;
          if (counta == 0)
            counta = totalpoints;
          second = counta;
        }
        anchorPos = state->intersectionOfLines(centerPoint,e,borderpoints[first],borderpoints[second]);
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

    case PGFbase:
      anchorPos = basePoint;
      break;  

    case PGFnorth:
      anchorPos.setX(0);
      anchorPos.setY(externalradius);
      break;

    case PGFsouth:
      anchorPos.setX(0);
      anchorPos.setY(-externalradius);
      break;

    case PGFeast:
      anchorPos.setX(externalradius);
      anchorPos.setY(0);
      break;

    case PGFwest:
      anchorPos.setX(-externalradius);
      anchorPos.setY(0);
      break;

    case PGFnorthwest:
      anchorPos.setX(-externalradius);
      anchorPos.setY(externalradius);
      break;

    case PGFsouthwest:
      anchorPos.setX(-externalradius);
      anchorPos.setY(-externalradius);
      break;

    case PGFnortheast:
      anchorPos.setX(externalradius);
      anchorPos.setY(externalradius);
      break;

    case PGFsoutheast:
      anchorPos.setX(externalradius);
      anchorPos.setY(-externalradius);
      break;
  }

  centerPos = centerPoint;

  if (dopath)
  {
    if (points.size() > 0)
    {
      state = state->save(false);
      state->moveTo(points[0]);
      for (int i = 1; i < points.size(); i++)
        state->lineTo(points[i]);

      state = state->restore();
    }

    doContent();
  }
}

void XWTikzShape::tapeShape(bool dopath)
{
  double x = 0.5 * boxWidth + state->innerXSep;
  double y = 0.5 * (boxHeight + boxDepth) + state->innerYSep;
  double tempdima = 0.5 * state->tapeBendHeight;
  double halfbendheight = tempdima;
  if (state->tapeBendTop > 0)
    y += tempdima;
  if (state->tapeBendBottom > 0)
    y += tempdima;

  double xa = state->minWidth;
  double ya = state->minHeight;
  if (x < 0.5 * xa)
    x = 0.5 * xa;
  if (y < 0.5 * ya)
    y = 0.5 * ya;
  if (state->tapeBendTop > 0)
    y -= tempdima;
  if (state->tapeBendBottom > 0)
    y -= tempdima;
  tempdima = 3.414213 * tempdima;
  double bendyradius = tempdima;
  tempdima += state->outerYSep;
  double outerbendyradius = tempdima;
  tempdima -= state->outerYSep;
  tempdima -= state->outerYSep;
  double innerbendyradius = tempdima;

  tempdima = 0.707106 * x;
  double bendxradius = tempdima;
  tempdima += state->outerXSep;
  double outerbendxradius = tempdima;
  tempdima -= state->outerXSep;
  tempdima -= state->outerXSep;
  double innerbendxradius = tempdima;

  double halfwidth = x;
  double halfheight = y;
  x += state->outerXSep;
  double outerhalfwidth = x;

  double xc = bendxradius;
  double yc = bendyradius;
  xc = 0.5 * atan(yc / xc);
  double halfangle = xc;
  double cothalfanglein = 1 / tan(45 - halfangle);
  double cothalfangleout = 1 / tan(90 - halfangle);

  x = centerPoint.x() - outerhalfwidth;
  y = centerPoint.y() + halfheight;
  yc = state->outerYSep;
  if (state->tapeBendTop == PGFinandout)
  {
    y += halfbendheight;
    y += cothalfanglein * yc;
  }
  else
  {
    if (state->tapeBendTop == PGFoutandin)
    {
      y += halfbendheight;
      y += cothalfangleout * yc;
    }
    else
      y += yc;
  }
  QPointF northwest(x,y);

  x = centerPoint.x() + outerhalfwidth;
  y = centerPoint.y() + halfheight;
  yc = state->outerYSep;
  if (state->tapeBendTop == PGFinandout)
  {
    y += halfbendheight;
    y += cothalfangleout * yc;
  }
  else
  {
    if (state->tapeBendTop == PGFoutandin)
    {
      y += halfbendheight;
      y += cothalfanglein * yc;
    }
    else
      y += yc;
  }
  QPointF northeast(x,y);

  x = centerPoint.x() + outerhalfwidth;
  y = centerPoint.y() - halfheight;
  yc = state->outerYSep;
  if (state->tapeBendTop == PGFoutandin)
  {
    y -= halfbendheight;
    y -= cothalfangleout * yc;
  }
  else
  {
    if (state->tapeBendTop == PGFinandout)
    {
      y -= halfbendheight;
      y -= cothalfanglein * yc;
    }
    else
      y -= yc;
  }
  QPointF southeast(x,y);

  x = centerPoint.x() - outerhalfwidth;
  y = centerPoint.y() - halfheight;
  yc = state->outerYSep;
  if (state->tapeBendTop == PGFoutandin)
  {
    y -= halfbendheight;
    y -= cothalfanglein * yc;
  }
  else
  {
    if (state->tapeBendTop == PGFinandout)
    {
      y -= halfbendheight;
      y -= cothalfangleout * yc;
    }
    else
      y -= yc;
  }
  QPointF southwest(x,y);

  switch (state->anchor)
  {
    default:
      {
        double externalx = state->anotherPoint.x() + centerPoint.x();
        double externaly = state->anotherPoint.y() + centerPoint.y();
        QPointF e(externalx,externaly);
        double externalangle = state->angleBetweenPoints(centerPoint,e);
        double mathresult = state->angleBetweenPoints(centerPoint,northwest);
        if (externalangle < mathresult)
        {
          if (externalangle < 90)
          {
            mathresult = state->angleBetweenPoints(centerPoint,northeast);
            if (externalangle < mathresult)
              anchorPos = state->intersectionOfLines(e,centerPoint,northeast,southeast);
            else
            {
              if (state->tapeBendTop == PGFinandout)
              {
                x = centerPoint.x();
                xc = halfwidth;
                x += 0.5 * xc;
                y += halfheight;
                y += halfbendheight;
                yc = bendyradius;
                y += -0.707106 * yc;

                QPointF c(x,y);
                anchorPos = pointIntersectionOfLineAndArc(e,centerPoint,c,5,175,outerbendxradius,outerbendyradius);
              }
              else
              {
                if (state->tapeBendTop == PGFoutandin)
                {
                  x = centerPoint.x() + 0.5 * halfwidth;
                  y = centerPoint.y() + halfheight + halfbendheight + 0.707106 * bendyradius;
                  QPointF c(x,y);
                  anchorPos = pointIntersectionOfLineAndArc(e,centerPoint,c,185,355,innerbendxradius,innerbendyradius);
                }
                else
                  anchorPos = state->intersectionOfLines(e,centerPoint,northeast,northwest);
              }
            }
          }
          else
          {
            if (state->tapeBendTop == PGFinandout)
            {
              x = centerPoint.x() - 0.5 * halfwidth;
              y = centerPoint.y() + halfheight + halfbendheight + 0.707106 * bendyradius;
              QPointF c(x,y);
              anchorPos = pointIntersectionOfLineAndArc(centerPoint,e,c,185,355,innerbendxradius,innerbendyradius);
            }
            else
            {
              if (state->tapeBendTop == PGFoutandin)
              {
                x = centerPoint.x() - 0.5 * halfwidth;
                y = centerPoint.y() + halfheight + halfbendheight - 0.707106 * bendyradius;
                QPointF c(x,y);
                anchorPos = pointIntersectionOfLineAndArc(centerPoint,e,c,5,175,outerbendxradius,outerbendyradius);
              }
              else
                anchorPos = state->intersectionOfLines(e,centerPoint,northeast,northwest);
            }
          }
        }
        else
        {
          mathresult = state->angleBetweenPoints(centerPoint,southwest);
          if (externalangle < mathresult)
          {
            if (externalangle > 270)
            {
              mathresult = state->angleBetweenPoints(centerPoint,southeast);
              if (externalangle > mathresult)
                anchorPos = state->intersectionOfLines(e,centerPoint,northeast,southeast);
              else
              {
                if (state->tapeBendBottom == PGFinandout)
                {
                  x = centerPoint.x() + 0.5 * halfwidth;
                  y = centerPoint.y() - halfheight - halfbendheight - 0.707106 * bendyradius;
                  QPointF c(x,y);
                  anchorPos = pointIntersectionOfLineAndArc(centerPoint,e,c,5,175,innerbendxradius,innerbendyradius);
                }
                else
                {
                  if (state->tapeBendBottom == PGFoutandin)
                  {
                    x = centerPoint.x() + 0.5 * halfwidth;
                    y = centerPoint.y() - halfheight - halfbendheight + 0.707106 * bendyradius;
                    QPointF c(x,y);
                    anchorPos = pointIntersectionOfLineAndArc(centerPoint,e,c,185,355,outerbendxradius,outerbendyradius);
                  }
                  else
                    anchorPos = state->intersectionOfLines(e,centerPoint,southeast,southwest);
                }
              }
            }
            else
            {
              if (state->tapeBendBottom == PGFinandout)
              {
                x = centerPoint.x() - 0.5 * halfwidth;
                y = centerPoint.y() - halfheight - halfbendheight + 0.707106 * bendyradius;
                QPointF c(x,y);
                anchorPos = pointIntersectionOfLineAndArc(centerPoint,e,c,185,355,outerbendxradius,outerbendyradius);
              }
              else
              {
                if (state->tapeBendBottom == PGFoutandin)
                {
                  x = centerPoint.x() - 0.5 * halfwidth;
                  y = centerPoint.y() - halfheight - halfbendheight - 0.707106 * bendyradius;
                  QPointF c(x,y);
                  anchorPos = pointIntersectionOfLineAndArc(centerPoint,e,c,5,175,innerbendxradius,innerbendyradius);
                }
                else
                  anchorPos = state->intersectionOfLines(e,centerPoint,southeast,southwest);
              }
            }
          }
          else
            anchorPos = state->intersectionOfLines(e,centerPoint,northwest,southwest);
        }
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

    case PGFbase:
      anchorPos = basePoint;
      break;

    case PGFmideast:
      anchorPos.setY(5);
      x = centerPoint.x() + outerhalfwidth;
      anchorPos.setX(x);
      break;

    case PGFmidwest:
      anchorPos.setY(5);
      x = centerPoint.x() - outerhalfwidth;
      anchorPos.setX(x);
      break;

    case PGFbaseeast:
      anchorPos.setY(0);
      x = centerPoint.x() + outerhalfwidth;
      anchorPos.setX(x);
      break;

    case PGFbasewest:
      anchorPos.setY(0);
      x = centerPoint.x() - outerhalfwidth;
      anchorPos.setX(x);
      break;

    case PGFnorth:
      anchorPos.setX(centerPoint.x());
      anchorPos.setY(0.5 * (northeast.y() + northwest.y()));
      break;

    case PGFsouth:
      anchorPos.setX(centerPoint.x());
      anchorPos.setY(0.5 * (southeast.y() + southwest.y()));
      break;

    case PGFeast:
      anchorPos.setX(centerPoint.x() + outerhalfwidth);
      anchorPos.setY(centerPoint.y());
      break;

    case PGFwest:
      anchorPos.setX(centerPoint.x() - outerhalfwidth);
      anchorPos.setY(centerPoint.y());
      break;

    case PGFnortheast:
      anchorPos = northeast;
      break;

    case PGFnorthwest:
      anchorPos = northwest;
      break;

    case PGFsoutheast:
      anchorPos = southeast;
      break;

    case PGFsouthwest:
      anchorPos = southwest;
      break;
  }

  if (dopath)
  {
    xc = bendxradius;
    yc = bendyradius;
    state = state->save(false);
    state->addShift(centerPoint.x(), centerPoint.y());
    state->moveTo(-halfwidth,0);
    state->lineTo(-halfwidth,halfheight);
    if (state->tapeBendTop == PGFinandout)
    {
      state->lineTo(-halfwidth,halfheight + halfbendheight);
      state->addArc(225,315,bendxradius,bendyradius);
      state->addArc(135,45,bendxradius,bendyradius);
    }
    else
    {
      if (state->tapeBendTop == PGFoutandin)
      {
        state->lineTo(-halfwidth,halfheight + halfbendheight);
        state->addArc(135,45,bendxradius,bendyradius);
        state->addArc(225,315,bendxradius,bendyradius);
      }
      else
        state->lineTo(halfwidth,halfheight);
    }
    state->lineTo(halfwidth,-halfheight);
    if (state->tapeBendBottom == PGFinandout)
    {
      state->lineTo(halfwidth,-halfheight - halfbendheight);
      state->addArc(45,135,bendxradius,bendyradius);
      state->addArc(315,225,bendxradius,bendyradius);
    }
    else
    {
      if (state->tapeBendBottom == PGFoutandin)
      {
        state->lineTo(halfwidth,-halfheight - halfbendheight);
        state->addArc(315,225,bendxradius,bendyradius);
        state->addArc(45,135,bendxradius,bendyradius);
      }
      else
        state->lineTo(-halfwidth, -halfheight);
    }

    state->closePath();
    state = state->restore();

    doContent();
  }
}
