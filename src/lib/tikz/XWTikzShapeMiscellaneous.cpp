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

void XWTikzShape::chamferedRectangleShape(bool dopath)
{
  bool northeastcorner = false;
  bool southeastcorner = false;
  bool northwestcorner = false;
  bool southwestcorner = false;
  for (int i = 0; i < state->chamferedRectangleCorners.size(); i++)
  {
    switch (state->chamferedRectangleCorners[i])
    {
      default:
        break;

      case PGFchamferall:
        northeastcorner = true;
        southeastcorner = true;
        northwestcorner = true;
        southwestcorner = true;
        break;

      case PGFnortheast:
        northeastcorner = true;
        break;

      case PGFsoutheast:
        southeastcorner = true;
        break;

      case PGFnorthwest:
        northwestcorner = true;
        break;

      case PGFsouthwest:
        southwestcorner = true;
        break;
    }
  }
  double xa = 0.5 * boxWidth;
  double ya = 0.5 * (boxHeight + boxDepth);
  double tempdima = state->chamferedRectangleAngle;
  if (tempdima < 89)
  {
    if (tempdima < 1)
      tempdima = 1;
  }
  else
    tempdima = 89;
  tempdima = -tempdima;
  tempdima += 90;
  double mathresult = tan(tempdima);
  double tanangle = mathresult;
  mathresult = 1 / tan(tempdima);
  double cotangle = mathresult;
  double xb = state->chamferedRectangleXsep;
  double yc = tanangle * xb;
  if (yc > ya)
  {
    yc = ya;
    xb = cotangle * yc;
  }
  mathresult = 1 / tan(tempdima);
  double yb = state->chamferedRectangleYsep;
  double xc = cotangle * yb;
  if (xc > xa)
  {
    xc = xa;
    yb = tanangle * xc;
  }
  tempdima = xa + xb;
  double tempdimb = state->minWidth;
  if (tempdima < 0.5 * tempdimb)
  {
    xa = 0.5 * tempdimb;
    xa -= xb;
  }
  tempdima = ya;
  tempdima += yb;
  tempdimb = state->minHeight;
  if (tempdima < 0.5 * tempdimb)
  {
    ya = 0.5 * tempdimb;
    ya -= yb;
  }

  double x = centerPoint.x() + xa + xb;
  double y = centerPoint.y() + ya - yc;
  QPointF beforenortheast(x,y);

  x = centerPoint.x() + xa;
  y = centerPoint.y() + ya;
  if (!northeastcorner)
  {
    x += xb;
    y += yb;
  }
  QPointF northeast(x,y);

  x = centerPoint.x() + xa - xc;
  y = centerPoint.y() + ya + yb;
  QPointF afternortheast(x,y);

  x = centerPoint.x() - xa;
  y = centerPoint.y() + ya;
  if (!northwestcorner)
  {
    x -= xb;
    y += yb;
  }
  QPointF northwest(x,y);

  x = centerPoint.x() - xa - xb;
  y = centerPoint.y() - ya + yc;
  QPointF beforesouthwest(x,y);

  x = centerPoint.x() - xa;
  y = centerPoint.y() - ya;
  if (!southwestcorner)
  {
    x -= xb;
    y -= yb;
  }
  QPointF southwest(x,y);

  x = centerPoint.x() - xa + xc;
  y = centerPoint.y() - ya - yb;
  QPointF aftersouthwest(x,y);

  x = centerPoint.x() + xa;
  y = centerPoint.y() - ya;
  if (!southeastcorner)
  {
    x += xb;
    y -= yb;
  }
  QPointF southeast(x,y);

  QPointF p(beforenortheast.x(), beforesouthwest.y());
  mathresult = state->angleBetweenLines(beforenortheast,afternortheast,beforenortheast,p);
  tempdima = mathresult;
  if (tempdima > 180)
    tempdima -= 180;
  tempdimb = state->outerXSep;
  if (tempdima < 90)
  {
    mathresult = 1 / sin(tempdima);
    tempdimb *= mathresult;
    tempdima = 0;
  }
  else
  {
    tempdima *= 0.5;
    mathresult = 1 / sin(tempdima);
    tempdimb *= mathresult;
    tempdima = 90 - tempdima;
  }

  x = tempdimb * cos(tempdima) + beforenortheast.x();
  y = tempdimb * sin(tempdima) + beforenortheast.y();
  QPointF beforeneanchor(x,y);

  tempdima = 180 - tempdima;
  x = -tempdimb * cos(tempdima) + beforesouthwest.x();
  y = -tempdimb * sin(tempdima) + beforesouthwest.y();
  QPointF beforeswanchor(x,y);

  p.setX(aftersouthwest.x());
  p.setY(afternortheast.y());
  mathresult = state->angleBetweenLines(afternortheast,p,afternortheast,beforenortheast);
  tempdima = mathresult;
  if (tempdima > 270)
    tempdima -= 270;
  tempdimb = state->outerYSep;
  if (tempdima < 90)
  {
    mathresult = 1 / sin(tempdima);
    tempdimb *= mathresult;
    tempdima = 90;
  }
  else
  {
    tempdima *= 0.5;
    mathresult = 1 / sin(tempdima);
    tempdimb *= mathresult;
  }

  x = tempdimb * cos(tempdima) + afternortheast.x();
  y = tempdimb * sin(tempdima) + afternortheast.y();
  QPointF afterneanchor(x,y);

  tempdima = 180 - tempdima;
  x = -tempdimb * cos(tempdima) + aftersouthwest.x();
  y = -tempdimb * sin(tempdima) + aftersouthwest.y();
  QPointF afterswanchor(x,y);

  if (!northeastcorner)
  {
    x = northeast.x() + state->outerXSep;
    y = northeast.y() + state->outerYSep;
  }
  else
  {
    p = state->lineAtTime(0.5,beforeneanchor,afterneanchor);
    x = p.x();
    y = p.y();
  }
  QPointF neanchor(x,y);

  if (!northwestcorner)
  {
    x = northwest.x() - state->outerXSep;
    y = northwest.y() - state->outerYSep;
  }
  else
  {
    QPointF p1(beforeswanchor.x(), beforeneanchor.y());
    QPointF p2(afterswanchor.x(), afterneanchor.y());
    p = state->lineAtTime(0.5,p1,p2);
    x = p.x();
    y = p.y();
  }
  QPointF nwanchor(x,y);

  if (!southwestcorner)
  {
    x = southwest.x() - state->outerXSep;
    y = southwest.y() - state->outerYSep;
  }
  else
  {
    p = state->lineAtTime(0.5,beforeswanchor,afterswanchor);
    x = p.x();
    y = p.y();
  }
  QPointF swanchor(x,y);

  if (!southeastcorner)
  {
    x = southeast.x() - state->outerXSep;
    y = southeast.y() - state->outerYSep;
  }
  else
  {
    QPointF p1(beforeneanchor.x(), beforeswanchor.y());
    QPointF p2(afterneanchor.x(), afterswanchor.y());
    p = state->lineAtTime(0.5,p1,p2);
    x = p.x();
    y = p.y();
  }
  QPointF seanchor(x,y);

  switch (state->anchor)
  {
    default:
      {
        double externalx = state->anotherPoint.x() + centerPoint.x();
        double externaly = state->anotherPoint.y() + centerPoint.y();
        QPointF externalpoint(externalx, externaly);
        mathresult = state->angleBetweenPoints(centerPoint, externalpoint);
        double externalangle = mathresult;
        QPointF p2(beforeswanchor.x(), centerPoint.y());
        mathresult = state->angleBetweenPoints(centerPoint, p2);
        if (externalangle < mathresult)
        {
          QPointF p3(centerPoint.x(), afterneanchor.y());
          mathresult = state->angleBetweenPoints(centerPoint, p3);
          if (externalangle < mathresult)
          {
            mathresult = state->angleBetweenPoints(centerPoint, neanchor);
            if (externalangle < mathresult)
            {
              mathresult = state->angleBetweenPoints(centerPoint, beforeneanchor);
              if (externalangle < mathresult)
              {
                QPointF p4(beforeneanchor.x(), centerPoint.y());
                anchorPos = state->intersectionOfLines(centerPoint,externalpoint,p4,beforeneanchor);
              }
              else
                anchorPos = state->intersectionOfLines(centerPoint,externalpoint,beforeneanchor,neanchor);
            }
            else
            {
              mathresult = state->angleBetweenPoints(centerPoint, afterneanchor);
              if (externalangle < mathresult)
                anchorPos = state->intersectionOfLines(centerPoint,externalpoint,neanchor,afterneanchor);
              else
              {
                QPointF p4(centerPoint.x(), afterneanchor.y());
                anchorPos = state->intersectionOfLines(centerPoint,externalpoint,afterneanchor,p4);
              }                
            }
          }
          else
          {
            mathresult = state->angleBetweenPoints(centerPoint, nwanchor);
            if (externalangle < mathresult)
            {
              QPointF p4(afterswanchor.x(), afterneanchor.y());
              mathresult = state->angleBetweenPoints(centerPoint, p4);
              if (externalangle < mathresult)
              {
                QPointF p5(centerPoint.x(), afterneanchor.y());
                anchorPos = state->intersectionOfLines(centerPoint,externalpoint,p5,p4);
              }
              else
              {
                QPointF p5(afterswanchor.x(), afterneanchor.y());
                anchorPos = state->intersectionOfLines(centerPoint,externalpoint,p5,nwanchor);
              }
            }
            else
            {
              QPointF p4(beforeswanchor.x(), beforeneanchor.y());
              mathresult = state->angleBetweenPoints(centerPoint, p4);
              if (externalangle < mathresult)
              {
                QPointF p5(beforeswanchor.x(), beforeneanchor.y());
                anchorPos = state->intersectionOfLines(centerPoint,externalpoint,nwanchor,p5);
              }
              else
              {
                QPointF p5(beforeswanchor.x(), centerPoint.y());
                anchorPos = state->intersectionOfLines(centerPoint,externalpoint,p4,p5);
              }
            }
          }
        }
        else
        {
          QPointF p3(centerPoint.x(), afterswanchor.y());
          mathresult = state->angleBetweenPoints(centerPoint, p3);
          if (externalangle < mathresult)
          {
            mathresult = state->angleBetweenPoints(centerPoint, swanchor);
            if (externalangle < mathresult)
            {
              mathresult = state->angleBetweenPoints(centerPoint, beforeswanchor);
              if (externalangle < mathresult)
              {
                QPointF p4(beforeswanchor.x(), centerPoint.y());
                anchorPos = state->intersectionOfLines(centerPoint,externalpoint,p4,beforeswanchor);
              }
              else
                anchorPos = state->intersectionOfLines(centerPoint,externalpoint,beforeswanchor,swanchor);
            }
            else
            {
              mathresult = state->angleBetweenPoints(centerPoint, afterswanchor);
              if (externalangle < mathresult)
                anchorPos = state->intersectionOfLines(centerPoint,externalpoint,swanchor,afterswanchor);
              else
              {
                QPointF p4(centerPoint.x(), afterswanchor.y());
                anchorPos = state->intersectionOfLines(centerPoint,externalpoint,afterswanchor,p4);
              }
            }
          }
          else
          {
            mathresult = state->angleBetweenPoints(centerPoint, seanchor);
            if (externalangle < mathresult)
            {
              QPointF p4(afterneanchor.x(), afterswanchor.y());
              mathresult = state->angleBetweenPoints(centerPoint, p4);
              if (externalangle < mathresult)
              {
                QPointF p5(centerPoint.x(), afterswanchor.y());
                anchorPos = state->intersectionOfLines(centerPoint,externalpoint,p4,p5);
              }
              else
                anchorPos = state->intersectionOfLines(centerPoint,externalpoint,p4,seanchor);
            }
            else
            {
              QPointF p4(beforeneanchor.x(), beforeswanchor.y());
              mathresult = state->angleBetweenPoints(centerPoint, p4);
              if (externalangle < mathresult)
                anchorPos = state->intersectionOfLines(centerPoint,externalpoint,seanchor,p4);
              else
              {
                QPointF p5(beforeneanchor.x(), centerPoint.y());
                anchorPos = state->intersectionOfLines(centerPoint,externalpoint,p4,p5);
              }
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

    case PGFmid:
      anchorPos = midPoint;
      break;

    case PGFmideast:
      ya = midPoint.y();
      y = beforeswanchor.y();
      if (ya < y)
      {
        QPointF p2(beforeneanchor.x(), midPoint.y());
        QPointF p3(beforeneanchor.x(), beforeswanchor.y());
        QPointF p4(afterneanchor.x(), afterswanchor.y());
        anchorPos = state->intersectionOfLines(midPoint,p2,p3,p4);
      }
      else
      {
        y = beforeneanchor.y();
        if (ya < y)
        {
          QPointF p2(beforeneanchor.x(), midPoint.y());
          QPointF p3(beforeneanchor.x(), beforeswanchor.y());
          anchorPos = state->intersectionOfLines(midPoint,p2,p3,beforeneanchor);
        }
        else
        {
          QPointF p2(beforeneanchor.x(), midPoint.y());
          anchorPos = state->intersectionOfLines(midPoint,p2,beforeneanchor,afterneanchor);
        }
      }
      break;

    case PGFmidwest:
      ya = midPoint.y();
      y = beforeswanchor.y();
      if (ya < y)
      {
        QPointF p2(beforeswanchor.x(), midPoint.y());
        anchorPos = state->intersectionOfLines(midPoint,p2,beforeswanchor,afterswanchor);
      }
      else
      {
        y = beforeneanchor.y();
        if (ya < y)
        {
          QPointF p2(beforeswanchor.x(), midPoint.y());
          QPointF p3(beforeswanchor.x(), beforeneanchor.y());
          anchorPos = state->intersectionOfLines(midPoint,p2,p3,beforeswanchor);
        }
        else
        {
          QPointF p2(beforeswanchor.x(), midPoint.y());
          QPointF p3(beforeswanchor.x(), beforeneanchor.y());
          QPointF p4(afterswanchor.x(), afterneanchor.y());
          anchorPos = state->intersectionOfLines(midPoint,p2,p3,p4);
        }
      }
      break;

    case PGFbase:
      anchorPos = basePoint;
      break;

    case PGFbaseeast:
      ya = basePoint.y();
      y = beforeswanchor.y();
      if (ya < y)
      {
        QPointF p2(beforeneanchor.x(), basePoint.y());
        QPointF p3(beforeneanchor.x(), beforeswanchor.y());
        QPointF p4(afterneanchor.x(), afterswanchor.y());
        anchorPos = state->intersectionOfLines(basePoint,p2,p3,p4);
      }
      else
      {
        y = beforeneanchor.y();
        if (ya < y)
        {
          QPointF p2(beforeneanchor.x(), basePoint.y());
          QPointF p3(beforeneanchor.x(), beforeswanchor.y());
          anchorPos = state->intersectionOfLines(basePoint,p2,p3,beforeneanchor);
        }
        else
        {
          QPointF p2(beforeneanchor.x(), basePoint.y());
          anchorPos = state->intersectionOfLines(basePoint,p2,beforeneanchor,afterneanchor);
        }
      }
      break;

    case PGFbasewest:
      ya = basePoint.y();
      y = beforeswanchor.y();
      if (ya < y)
      {
        QPointF p2(beforeswanchor.x(), basePoint.y());
        anchorPos = state->intersectionOfLines(basePoint,p2,beforeswanchor,afterswanchor);
      }
      else
      {
        y = beforeneanchor.y();
        if (ya < y)
        {
          QPointF p2(beforeswanchor.x(), basePoint.y());
          QPointF p3(beforeswanchor.x(), beforeneanchor.y());
          anchorPos = state->intersectionOfLines(basePoint,p2,p3,beforeswanchor);
        }
        else
        {
          QPointF p2(beforeswanchor.x(), basePoint.y());
          QPointF p3(beforeswanchor.x(), beforeneanchor.y());
          QPointF p4(afterswanchor.x(), afterneanchor.y());
          anchorPos = state->intersectionOfLines(basePoint,p2,p3,p4);
        }
      }
      break;

    case PGFbeforenortheast:
      anchorPos = beforeneanchor;
      break;

    case PGFnortheast:
      anchorPos = neanchor;
      break;

    case PGFafternortheast:
      anchorPos = afterneanchor;
      break;

    case PGFnorth:
      anchorPos.setX(centerPoint.x());
      anchorPos.setY(afterneanchor.y());
      break;

    case PGFbeforenorthwest:
      anchorPos.setX(afterswanchor.x());
      anchorPos.setY(afterneanchor.y());
      break;

    case PGFnorthwest:
      anchorPos = nwanchor;
      break;

    case PGFafternorthwest:
      anchorPos.setX(beforeswanchor.x());
      anchorPos.setY(beforeneanchor.y());
      break;

    case PGFwest:
      anchorPos.setX(beforeswanchor.x());
      anchorPos.setY(centerPoint.y());
      break;

    case PGFbeforesouthwest:
      anchorPos = beforeswanchor;
      break;

    case PGFsouthwest:
      anchorPos = swanchor;
      break;

    case PGFaftersouthwest:
      anchorPos = afterswanchor;
      break;

    case PGFsouth:
      anchorPos.setX(centerPoint.x());
      anchorPos.setY(afterswanchor.y());
      break;

    case PGFbeforesoutheast:
      anchorPos.setX(afterneanchor.x());
      anchorPos.setY(afterswanchor.y());
      break;

    case PGFsoutheast:
      anchorPos = seanchor;
      break;

    case PGFaftersoutheast:
      anchorPos.setX(beforeneanchor.x());
      anchorPos.setY(beforeswanchor.y());
      break;

    case PGFeast:
      anchorPos.setX(beforeneanchor.x());
      anchorPos.setY(centerPoint.y());
      break;
  }

  if (dopath)
  {
    state = state->save(false);
    state->moveTo(beforenortheast);
    state->lineTo(northeast);
    state->lineTo(afternortheast);
    state->lineTo(aftersouthwest.x(), afternortheast.y());
    state->lineTo(northwest);
    state->lineTo(beforesouthwest.x(), beforenortheast.y());
    state->lineTo(beforesouthwest);
    state->lineTo(southwest);
    state->lineTo(aftersouthwest);
    state->lineTo(afternortheast.x(), aftersouthwest.y());
    state->lineTo(southeast);
    state->lineTo(beforenortheast.x(), beforesouthwest.y());
    state->closePath();
    state = state->restore();

    doContent();
  }
}

void XWTikzShape::crossoutShape(bool dopath)
{
  rectangleAnchor();
  if (dopath)
  {
    doContent();

    double xa = southWest.x();
    double ya = southWest.y();
    double xb = northEast.x();
    double yb = northEast.y();
    state = state->save(false);
    state->moveTo(xa,ya);
    state->lineTo(xb,yb);
    state->moveTo(xa,yb);
    state->lineTo(xb,ya);
    state = state->restore();
  }
}

void XWTikzShape::roundedRectangleShape(bool dopath)
{
  int westarc = state->roundedRectangleWestArc;
  int eastarc = state->roundedRectangleEastArc;
  double x = 0.5 * state->roundedRectangleArcLength;
  double halfarcangle = x;
  x = state->innerXSep;
  double innerxsep = x;
  double xa = 0.5 * boxWidth;
  double halftextwidth = xa;
  x += xa;
  double y = state->innerYSep;
//  double innerysep = y;
  double ya = 0.5 * (boxHeight + boxDepth);
  double halftextheight = ya;
  y += ya;
  double yb = state->minHeight;
  if (y < 0.5 * yb)
    y = 0.5 * yb;
  double halfheight = y;
  double mathresult = 1 / sin(halfarcangle);
  ya = mathresult * y;
  radius = ya;

  mathresult = cos(halfarcangle);
  xa = ya - mathresult * ya;

  mathresult = halftextheight / radius;
  mathresult = asin(mathresult);
  mathresult = cos(mathresult);
  double xb = ya - mathresult * ya;

  double tempdima = 2 * x;
  if (westarc == PGFconcave)
    tempdima += xa;
  else
  {
    if (westarc == PGFconvex)
      tempdima += xb;
  }

  if (eastarc == PGFconcave)
    tempdima += xa;
  else
  {
    if (eastarc == PGFconvex)
      tempdima += xb;
  }

  double tempdimb = state->minWidth;
  double xoffset = innerxsep;
  if (tempdima < tempdimb)
  {
    tempdimb -= tempdima;
    tempdimb /= 2;
    xoffset = tempdimb;
  }

  x = halftextwidth;
  x += xoffset;
  double xc = ya - xb;
  if (xc > x)
  {
    xc -= x;
    xc += xoffset;
    xoffset = xc;
  }

  x = halftextwidth;
  x += xoffset;
  double halfwidth = x;
  double arcwidth = xa;
  double chordwidth = xb;

  double outerxsep = state->outerXSep;
  double outerysep = state->outerYSep;
  double concavexshift = 0;
  if (halfarcangle != 90)
  {
    mathresult = 90 - halfarcangle;
    mathresult /= 2;
    double angletemp = mathresult;
    x = outerxsep;
    mathresult = 1 / sin(angletemp);
    x *= mathresult;
    mathresult = cos(angletemp);
    x *= mathresult;
    concavexshift = x;
  }

  double convexxshift = 0;
  if (halfarcangle != 90)
  {
    mathresult = 90 + halfarcangle;
    mathresult /= 2;
    double angletemp = mathresult;
    x = outerxsep;
    mathresult = 1 / sin(angletemp);
    x *= mathresult;
    mathresult = cos(angletemp);
    x *= mathresult;
    convexxshift = x;
  }

//  double halflinewidth = state->lineWidth;
  double yc = 0;

#define XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_EASET \
  if (eastarc == PGFconcave)\
  {\
    x = centerPoint.x() + halfwidth + arcwidth + concavexshift;\
    y = centerPoint.y();\
  }\
  else\
  {\
    if (eastarc == PGFconvex)\
    {\
      x = centerPoint.x() + halfwidth + chordwidth + outerxsep;\
      y = centerPoint.y();\
    }\
    else\
    {\
      x = centerPoint.x() + halfwidth + outerxsep;\
      y = centerPoint.y();\
    }\
  }

#define XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_WEST \
  if (westarc == PGFconcave)\
  {\
    x = centerPoint.x() - halfwidth - arcwidth - concavexshift;\
    y = centerPoint.y();\
  }\
  else\
  {\
    if (westarc == PGFconvex)\
    {\
      x = centerPoint.x() - halfwidth - chordwidth - outerxsep;\
      y = centerPoint.y();\
    }\
    else\
    {\
      x = centerPoint.x() - halfwidth - outerxsep;\
      y = centerPoint.y();\
    }\
  }

#define XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_NORTH \
  x = centerPoint.x();\
  y = centerPoint.y() + halfheight + outerysep;

#define XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_SOUTH \
  x = centerPoint.x();\
  y = centerPoint.y() - halfheight - outerysep;

#define XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_NORTHWEST \
  if (westarc == PGFconvex)\
  {\
    x = centerPoint.x() - halfwidth - chordwidth + arcwidth - convexxshift;\
    y = centerPoint.y();\
  }\
  else\
  {\
    XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_WEST\
  }\
  xc = x;\
  XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_NORTH\
  x = xc;

#define XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_NORTHEAST \
  if (eastarc == PGFconvex)\
  {\
    x = centerPoint.x() + halfwidth + chordwidth - arcwidth + convexxshift;\
    y = centerPoint.y();\
  }\
  else\
  {\
    XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_EASET\
  }\
  xc = x;\
  XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_NORTH\
  x = xc;

#define XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_SOUTHWEST \
  if (westarc == PGFconvex)\
  {\
    x = centerPoint.x() - halfwidth - chordwidth + arcwidth - convexxshift;\
    y = centerPoint.y();\
  }\
  else\
  {\
    XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_WEST\
  }\
  xc = x;\
  XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_SOUTH\
  x = xc;

#define XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_SOUTHEAST \
  if (eastarc == PGFconvex)\
  {\
    x = centerPoint.x() + halfwidth + chordwidth - arcwidth + convexxshift;\
    y = centerPoint.y();\
  }\
  else\
  {\
    XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_EASET\
  }\
  xc = x;\
  XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_SOUTH\
  x = xc;

  switch (state->anchor)
  {
    default:
      {
        xc = state->anotherPoint.x() + centerPoint.x();
        yc = state->anotherPoint.y() + centerPoint.y();

        QPointF externalpoint(xc,yc);
        mathresult = state->angleBetweenPoints(centerPoint, externalpoint);
        double externalangle = mathresult;
        XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_NORTHWEST
        QPointF p1(x,y);
        mathresult = state->angleBetweenPoints(centerPoint, p1);
        if (externalangle < mathresult)
        {
          XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_NORTHEAST
          QPointF p2(x,y);
          mathresult = state->angleBetweenPoints(centerPoint, p2);
          if (externalangle < mathresult)
          {
            if (eastarc == PGFconvex)
            {
              x = centerPoint.x() + halfwidth + chordwidth - radius;
              y = centerPoint.y();
              QPointF arccenter(x,y);
              anchorPos = pointIntersectionOfLineAndArc(externalpoint,centerPoint,arccenter,0,halfarcangle,radius + outerxsep, radius + outerysep);
            }
            else
            {
              XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_NORTHEAST
              QPointF p3(x,y);
              XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_SOUTHEAST
              QPointF p4(x,y);
              anchorPos = state->intersectionOfLines(p3,p4,externalpoint,centerPoint);
            }
          }
          else
          {
            XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_NORTHEAST
            QPointF p3(x,y);
            XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_NORTHWEST
            QPointF p4(x,y);
            anchorPos = state->intersectionOfLines(p3,p4,externalpoint,centerPoint);
          }
        }
        else
        {
          XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_SOUTHWEST
          QPointF p2(x,y);
          mathresult = state->angleBetweenPoints(centerPoint, p2);
          if (externalangle < mathresult)
          {
            if (westarc == PGFconvex)
            {
              x = centerPoint.x() - halfwidth - chordwidth + radius;
              y = centerPoint.y();
              QPointF arccenter(x,y);
              anchorPos = pointIntersectionOfLineAndArc(externalpoint,centerPoint,arccenter,180-halfarcangle,180+halfarcangle,radius + outerxsep, radius + outerysep);
            }
            else
            {
              XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_NORTHWEST
              QPointF p3(x,y);
              XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_SOUTHWEST
              QPointF p4(x,y);
              anchorPos = state->intersectionOfLines(p3,p4,externalpoint,centerPoint);
            }
          }
          else
          {
            XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_SOUTHEAST
            QPointF p3(x,y);
            mathresult = state->angleBetweenPoints(centerPoint, p3);
            if (externalangle < mathresult)
            {
              XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_SOUTHWEST
              QPointF p4(x,y);
              anchorPos = state->intersectionOfLines(p3,p4,externalpoint,centerPoint);
            }
            else
            {
              if (eastarc == PGFconvex)
              {
                x = centerPoint.x() + halfwidth + chordwidth - radius;
                y = centerPoint.y();
                QPointF arccenter(x,y);
                anchorPos = pointIntersectionOfLineAndArc(externalpoint,centerPoint,arccenter,360-halfarcangle,360,radius + outerxsep, radius + outerysep);
              }
              else
              {
                XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_NORTHEAST
                QPointF p4(x,y);
                XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_SOUTHEAST
                QPointF p5(x,y);
                anchorPos = state->intersectionOfLines(p4,p5,externalpoint,centerPoint);
              }
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
      x = centerPoint.x();
      y = centerPoint.y();
      break;

    case PGFmid:
      x = midPoint.x();
      y = midPoint.y();
      break;

    case PGFmidwest:
      XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_WEST
      y = midPoint.y();
      break;

    case PGFmideast:
      XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_EASET
      y = midPoint.y();
      break;

    case PGFbase:
      x = basePoint.x();
      y = basePoint.y();
      break;

    case PGFbasewest:
      XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_WEST
       y = basePoint.y();
      break;

    case PGFbaseeast:
      XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_EASET
      y = basePoint.y();
      break;

    case PGFnorth:
      XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_NORTH
      break;

    case PGFsouth:
      XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_SOUTH
      break;

    case PGFwest:
      XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_WEST
      break;

    case PGFnorthwest:
      XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_NORTHWEST
      break;

    case PGFsouthwest:
      XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_SOUTHWEST
      break;

    case PGFeast:
      XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_EASET
      break;

    case PGFnortheast:
      XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_NORTHEAST
      break;

    case PGFsoutheast:
      XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_SOUTHEAST
      break;
  }

  anchorPos.setX(x);
  anchorPos.setY(y);

  if (dopath)
  {
    state = state->save(false);
    state->addShift(centerPoint.x(),centerPoint.y());
    state->moveTo(0, halfheight);
    if (eastarc == PGFconcave)
    {
      state->lineTo(halfwidth + arcwidth, halfheight);
      state->addArc(180 - halfarcangle, 180 + halfarcangle, radius, radius);
    }
    else
    {
      if (eastarc == PGFconvex)
      {
        state->lineTo(halfwidth + chordwidth - arcwidth, halfheight);
        state->addArc(halfarcangle, -halfarcangle, radius, radius);
      }
      else
      {
        state->lineTo(halfwidth, halfheight);
        state->lineTo(halfwidth, -halfheight);
      }
    }

    if (westarc == PGFconcave)
    {
      state->lineTo(-halfwidth - arcwidth, -halfheight);
      state->addArc(-halfarcangle, halfarcangle, radius, radius);
    }
    else
    {
      if (westarc == PGFconvex)
      {
        state->lineTo(-halfwidth - chordwidth + arcwidth, -halfheight);
        state->addArc(180 + halfarcangle, 180 - halfarcangle, radius, radius);
      }
      else
      {
        state->lineTo(-halfwidth, -halfheight);
        state->lineTo(-halfwidth, halfheight);
      }
    }

    state->closePath();
    state = state->restore();

    doContent();
  }
}

void XWTikzShape::strikeoutShape(bool dopath)
{
  rectangleAnchor();
  if (dopath)
  {
    doContent();

    state = state->save(false);
    state->moveTo(southWest);
    state->lineTo(northEast);
    state = state->restore();
  }
}
