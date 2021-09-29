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

void XWTikzShape::circularSectorShape(bool dopath)
{
  double x = state->outerXSep;
  double y = state->outerYSep;
  if (x < y)
    x = y;
  double outersep = x;

  double mathresult = (int)(state->circularSectorAngle) % 360;
  if (mathresult < 0)
    mathresult += 360;
  double angle = mathresult;
  mathresult = mathresult / 2;
  double halfangle = mathresult;
  mathresult = sin(mathresult);
//  double sinehalfangle = mathresult;
  mathresult = 1 / mathresult;
  double cosechalfangle = mathresult;
  mathresult = qAbs(halfangle);
  mathresult = cos(mathresult);
  double coshalfangle = mathresult;
  x = cosechalfangle * mathresult;
//  double cothalfangle = x;
  x = outersep * cosechalfangle;
  double centermiter = x;

  double startangle = 180 - halfangle;
  double endangle = 180 + halfangle;

  x = 0.5 * boxWidth + state->innerXSep;
  y = 0.5 * (boxHeight + boxDepth) + state->innerYSep;
  double rotate = state->shapeBorderRotate;
  double xa = 0;
  double xb = 0;
//  double ya = 0;
  if (state->shapeBorderUsesIncircle)
  {
    if (x < y)
      x = y;

    x *= 1.41421;
    xa = cosechalfangle * x;
    xb = xa;
    xa += x;
  }
  else
  {
    mathresult = (int)(state->shapeBorderRotate) % 360;
    if (mathresult < 0)
      mathresult += 360;
    int counta = (int)mathresult;
    counta += 45;
    counta /= 90;
    counta *= 90;
    rotate = counta;
    if (counta == 0)
    {
      xa = x;
      x = y;
      y = xa;
    }
    else
    {
      if (counta == 270)
      {
        xa = x;
        x = y;
        y = xa;
      }
    }

    xa = cosechalfangle * y;
    xa = coshalfangle * xa;
    xa += x;
    xb = xa;
    xa += x;
    xa = state->veclen(xa,y);
  }

  x = 0.5 * state->minWidth;
  mathresult = 1 / cos(halfangle);
  double xc = mathresult * xb;
  if (xc < 0)
    xc = -xc;
  if (xc < x)
  {
    mathresult = 1 / xc;
    xc = mathresult * x;
    xa = xc * xa;
    xb = xc * xb;
  }
  y = state->minHeight;
  if (xa < y)
  {
    mathresult = 1 / xa;
    xc = mathresult * y;
    xa = y;
    xb = xc * xb;
  }
  double centeroffset = xb;
  radius = xa;
  xa += outersep;
  double borderradius = xa;
  x = outersep * cosechalfangle;
  x *= coshalfangle;
  xa += x;
  double cornerradius = xa;

  QPointF sectorcenter(centerPoint.x() + centeroffset, centerPoint.y());
  x = radius * cos(startangle) + sectorcenter.x();
  y = radius * sin(startangle) + sectorcenter.y();
  QPointF arcstart(x,y);
  QPointF sectorcenterborder(sectorcenter.x() + centermiter,sectorcenter.y());
  x = borderradius * cos(startangle) + sectorcenter.x();
  y = borderradius * sin(startangle) + sectorcenter.y();
  QPointF arcstartborder(x,y);
  x = borderradius * cos(endangle) + sectorcenter.x();
  y = borderradius * sin(endangle) + sectorcenter.y();
  QPointF arcendborder(x,y);
  x = cornerradius * cos(startangle) + sectorcenter.x();
  y = cornerradius * sin(startangle) + sectorcenter.y();
  QPointF arcstartcorner(x,y);
  x = cornerradius * cos(endangle) + sectorcenter.x();
  y = cornerradius * sin(endangle) + sectorcenter.y();
  QPointF arcendcorner(x,y);

//  double angletosectorcenterborder = state->angleBetweenPoints(centerPoint,sectorcenterborder);
  double angletoarcstartborder = state->angleBetweenPoints(centerPoint,arcstartborder);
  double angletoarcendborder = state->angleBetweenPoints(centerPoint,arcendborder);
  double angletoarcstartcorner = state->angleBetweenPoints(centerPoint,arcstartcorner);
  double angletoarcendcorner = state->angleBetweenPoints(centerPoint,arcendcorner);

  sectorcenter = state->rotatePointAround(sectorcenter,centerPoint,rotate);
  arcstart = state->rotatePointAround(arcstart,centerPoint,rotate);
  sectorcenterborder = state->rotatePointAround(sectorcenterborder,centerPoint,rotate);
  arcstartborder = state->rotatePointAround(arcstartborder,centerPoint,rotate);
  arcendborder = state->rotatePointAround(arcendborder,centerPoint,rotate);
  arcstartcorner = state->rotatePointAround(arcstartcorner,centerPoint,rotate);
  arcendcorner = state->rotatePointAround(arcendcorner,centerPoint,rotate);

  startangle += rotate;
  endangle += rotate;

#define XW_TIKZ_SHAPE_CIRCULARSECTOR_ANCHOR_BORDER(ex,ey) \
  QPointF ep(centerPoint.x() + ex,centerPoint.y() + ey); \
  mathresult = state->angleBetweenPoints(centerPoint,ep); \
  double externalangle = mathresult; \
  mathresult -= rotate; \
  if (mathresult < 0) \
    mathresult += 360; \
  double angle = mathresult; \
  QPointF firstpoint,secondpoint; \
  if (angle > angletoarcendcorner) \
  {\
    firstpoint = arcendcorner; \
    secondpoint = sectorcenterborder; \
  }\
  else \
  {\
    if (angle > angletoarcendborder) \
    {\
      firstpoint = arcendborder; \
      secondpoint = arcendcorner; \
    }\
    else \
    {\
      if (angle <= angletoarcstartborder) \
      {\
        if (angle > angletoarcstartcorner) \
        {\
          firstpoint = arcstartborder; \
          secondpoint = arcstartcorner ;\
        }\
        else \
        {\
          firstpoint = sectorcenterborder; \
          secondpoint = arcstartcorner; \
        } \
      }\
    }\
  }\
  if (firstpoint.isNull()) \
  {\
    mathresult = state->angleBetweenLines(centerPoint,ep,sectorcenter,centerPoint);\
    mathresult = sin(mathresult); \
    double sineangle = mathresult; \
    x = borderradius;\
    mathresult = 1 / x; \
    double reciprocalradius = mathresult;\
    x = centeroffset; \
    x *= sineangle; \
    x *= reciprocalradius; \
    mathresult = asin(x); \
    x = mathresult; \
    x += externalangle; \
    angle = x; \
    QPointF p(borderradius * cos(angle), borderradius * sin(angle)); \
    anchorPos = sectorcenter + p; \
  }\
  else \
    anchorPos = state->intersectionOfLines(centerPoint,ep,firstpoint,secondpoint);

  switch (state->anchor)
  {
    default:
      {
        double delta = state->angle;
        double externalx = cos(delta);
        double externaly = sin(delta);
        XW_TIKZ_SHAPE_CIRCULARSECTOR_ANCHOR_BORDER(externalx,externaly)
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

    case PGFarcstart:
      anchorPos = arcstartcorner;
      break;

    case PGFarcend:
      anchorPos = arcendcorner;
      break;

    case PGFsectorcenter:
      anchorPos = sectorcenterborder;
      break;

    case PGFarccenter:
      {
        angle = rotate + 180;
        x = cornerradius * cos(angle);
        y = cornerradius * sin(angle);
        XW_TIKZ_SHAPE_CIRCULARSECTOR_ANCHOR_BORDER(x,y)
      }
      break;

    case PGFnorth:
      {
        XW_TIKZ_SHAPE_CIRCULARSECTOR_ANCHOR_BORDER(0,cornerradius)
      }
      break;

    case PGFsouth:
      {
        XW_TIKZ_SHAPE_CIRCULARSECTOR_ANCHOR_BORDER(0,-cornerradius)
      }
      break;

    case PGFeast:
      {
        XW_TIKZ_SHAPE_CIRCULARSECTOR_ANCHOR_BORDER(cornerradius,0)
      }
      break;

    case PGFwest:
      {
        XW_TIKZ_SHAPE_CIRCULARSECTOR_ANCHOR_BORDER(-cornerradius,0)
      }
      break;

    case PGFnorthwest:
      {
        XW_TIKZ_SHAPE_CIRCULARSECTOR_ANCHOR_BORDER(-cornerradius,cornerradius)
      }
      break;

    case PGFsouthwest:
      {
        XW_TIKZ_SHAPE_CIRCULARSECTOR_ANCHOR_BORDER(-cornerradius,-cornerradius)
      }
      break;

    case PGFnortheast:
      {
        XW_TIKZ_SHAPE_CIRCULARSECTOR_ANCHOR_BORDER(cornerradius,cornerradius)
      }
      break;

    case PGFsoutheast:
      {
        XW_TIKZ_SHAPE_CIRCULARSECTOR_ANCHOR_BORDER(cornerradius,-cornerradius)
      }
      break;
  }

  if (dopath)
  {
    state = state->save(false);
    state->moveTo(sectorcenter);
    state->lineTo(arcstart);
    if (endangle > 360)
    {
      if (startangle > 360)
        state->addArc(startangle,endangle,radius,radius);
      else
      {
        state->addArc(startangle,360,radius,radius);
        state->addArc(0,endangle,radius,radius);
      }        
    }
    else
      state->addArc(startangle,endangle,radius,radius);

    state->closePath();

    state = state->restore();

    doContent();
  }
}

void XWTikzShape::cylinderShape(bool dopath)
{
  double xc = state->innerXSep;
  double yc = state->innerYSep;
  double x = xc + 0.5 * boxWidth;
  double y = yc + 0.5 * (boxHeight + boxDepth);
  double rotate = state->shapeBorderRotate;
  double xa = 0;
  double ya = 0;
  double mathresult = 0;
  if (state->shapeBorderUsesIncircle)
  {
    if (x < y)
     x = y;
    else
     y = x;
    x *= 1.414213;
    y *= 1.414213;
  }
  else
  {
    mathresult = (int)(state->shapeBorderRotate) % 360;
    if (mathresult < 0)
      mathresult += 360;
    int counta = (int)mathresult;
    counta += 45;
    counta /= 90;
    counta *= 90;
    rotate = counta;
    if (counta == 0)
    {
      xa = x;
      x = y;
      y = xa;
      yc = xc;
    }
    else
    {
      if (counta == 270)
      {
        xa = x;
        x = y;
        y = xa;
        yc = xc;
      }
    }
  }
  xa = x;
  ya = y;
  double tempdima = state->aspect * ya;
  double tempdimb = ya;
  xc = state->minWidth;
  if (tempdimb < 0.5 * xc)
  {
    tempdimb = 0.5 * xc;
    ya = tempdimb;
  }

  double yb = tempdimb - yc;
  mathresult = yb / tempdimb;
  mathresult = asin(mathresult);
  mathresult = cos(mathresult);
//  double angle = mathresult;
  double xb = mathresult * tempdima;
  x = 0.5 * state->lineWidth + 2 * xa + 3 * tempdima - xb;
  xc = state->minHeight;
  if (x < xc)
  {
    xc -= x;
    xa += 0.5 * xc;
  }

  x = tempdima;
  y = tempdimb;
  xc = state->outerXSep;
  yc = state->outerYSep;
  double outersep = 0;
  if (xc > yc)
  {
    x += xc;
    y += xc;
    outersep = xc;
  }
  else
  {
    x += yc;
    y += yc;
    outersep = yc;
  }

  double xradius = x;
  double yradius = y;

  y = 0;
  x = tempdima + 0.5 * state->lineWidth + xb;
  x *= 0.5;
  QPointF cylindercenter(x,y);

  y = ya;
  x = xa + tempdima + 0.5 * state->lineWidth;
  QPointF beforetop(x,y);

  x = xb - xa;
  y = ya;
  QPointF afterbottom(x,y);

  yc = state->outerYSep;
  QPointF beforetopanchor(beforetop.x(), beforetop.y() + yc);
  QPointF afterbottomanchor(afterbottom.x(),afterbottom.y() + yc);

  x = beforetopanchor.x() + xradius;
  y = beforetopanchor.y();
  double externalradius = y;
  if (x > y)
    externalradius = x;

#define XW_TIKZ_SHAPE_CYLINDER_ANCHOR_BORDER(ex,ey) \
  QPointF ep(referencePoint.x() + ex, referencePoint.y() + ey); \
  mathresult = state->angleBetweenPoints(centerPoint,ep); \
  mathresult -= rotate; \
  if (mathresult < 0) \
    mathresult += 360; \
  double externalangle = mathresult; \
  QPointF p1 = afterbottomanchor + centerPoint; \
  mathresult = state->angleBetweenPoints(referencePoint,p1); \
  if (externalangle < mathresult) \
  { \
    p1 = beforetopanchor + centerPoint; \
    mathresult = state->angleBetweenPoints(referencePoint,p1); \
    if (externalangle < mathresult) \
    {\
      QPointF p2 = state->rotatePointAround(ep,centerPoint, -rotate);\
      QPointF p3 = state->rotatePointAround(referencePoint,centerPoint, -rotate);\
      x = beforetop.x() + centerPoint.x(); \
      QPointF c(x,centerPoint.y()); \
      p1 = pointIntersectionOfLineAndArc(p2,p3,c,0,90,xradius,yradius); \
      anchorPos = state->rotatePointAround(p1,centerPoint,rotate); \
    }\
    else \
    {\
      QPointF p2 = afterbottomanchor + centerPoint; \
      p2 = state->rotatePointAround(p2,centerPoint,rotate); \
      QPointF p3 = beforetopanchor + centerPoint; \
      p3 = state->rotatePointAround(p3,centerPoint,rotate); \
      anchorPos = state->intersectionOfLines(p2,p3,referencePoint,ep); \
    }\
  }\
  else \
  {\
    x = afterbottomanchor.x() + centerPoint.x(); \
    y = -afterbottomanchor.y() + centerPoint.y(); \
    p1.setX(x); \
    p1.setY(y); \
    mathresult = state->angleBetweenPoints(referencePoint,p1); \
    if (externalangle > mathresult) \
    {\
      x = beforetopanchor.x() + centerPoint.x(); \
      y = -beforetopanchor.y() + centerPoint.y(); \
      p1.setX(x); \
      p1.setY(y); \
      mathresult = state->angleBetweenPoints(referencePoint,p1); \
      if (externalangle > mathresult) \
      {\
        QPointF p2 = state->rotatePointAround(ep,centerPoint,-rotate); \
        QPointF p3 = state->rotatePointAround(referencePoint,centerPoint,-rotate); \
        x = beforetop.x() + centerPoint.x(); \
        QPointF c(x,centerPoint.y()); \
        p1 = pointIntersectionOfLineAndArc(p2,p3,c,270,360,xradius,yradius); \
        anchorPos = state->rotatePointAround(p1,centerPoint,rotate); \
      }\
      else \
      {\
        x = afterbottomanchor.x() + centerPoint.x(); \
        y = -afterbottomanchor.y() + centerPoint.y(); \
        p1.setX(x); \
        p1.setY(y); \
        QPointF p2 = state->rotatePointAround(p1,centerPoint,rotate); \
        x = beforetopanchor.x() + centerPoint.x(); \
        y = -beforetopanchor.y() + centerPoint.y(); \
        p1.setX(x); \
        p1.setY(y); \
        QPointF p3 = state->rotatePointAround(p1,centerPoint,rotate); \
        anchorPos = state->intersectionOfLines(p2,p3,referencePoint,ep); \
      }\
    }\
    else \
    {\
      QPointF p2 = state->rotatePointAround(ep,centerPoint,-rotate); \
      QPointF p3 = state->rotatePointAround(referencePoint,centerPoint,-rotate); \
      x = afterbottom.x() + centerPoint.x(); \
      QPointF c(x,centerPoint.y()); \
      p1 = pointIntersectionOfLineAndArc(p2,p3,c,90,270,xradius,yradius); \
      anchorPos = state->rotatePointAround(p1,centerPoint,rotate); \
    }\
  }

  switch (state->anchor)
  {
    default:
      {
        double delta = state->angle;
        double externalx = cos(delta);
        double externaly = sin(delta);
        XW_TIKZ_SHAPE_CYLINDER_ANCHOR_BORDER(externalx,externaly)
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

    case PGFshapecenter:
      {
        QPointF p = cylindercenter + centerPoint;
        anchorPos = state->rotatePointAround(p,centerPoint,rotate);
      }
      break;

    case PGFmideast:
      {
        referencePoint = midPoint;
        XW_TIKZ_SHAPE_CYLINDER_ANCHOR_BORDER(externalradius,0)
      }
      break;

    case PGFmidwest:
      {
        referencePoint = midPoint;
        XW_TIKZ_SHAPE_CYLINDER_ANCHOR_BORDER(-externalradius,0)
      }
      break;

    case PGFbaseeast:
      {
        referencePoint = basePoint;
        XW_TIKZ_SHAPE_CYLINDER_ANCHOR_BORDER(externalradius,0)
      }
      break;

    case PGFbasewest:
      {
        referencePoint = basePoint;
        XW_TIKZ_SHAPE_CYLINDER_ANCHOR_BORDER(-externalradius,0)
      }
      break;

    case PGFnorth:
      {
        XW_TIKZ_SHAPE_CYLINDER_ANCHOR_BORDER(0,externalradius)
      }
      break;

    case PGFsouth:
      {
        XW_TIKZ_SHAPE_CYLINDER_ANCHOR_BORDER(0,-externalradius)
      }
      break;

    case PGFeast:
      {
        XW_TIKZ_SHAPE_CYLINDER_ANCHOR_BORDER(externalradius,0)
      }
      break;

    case PGFwest:
      {
        XW_TIKZ_SHAPE_CYLINDER_ANCHOR_BORDER(-externalradius,0)
      }
      break;

    case PGFnortheast:
      {
        XW_TIKZ_SHAPE_CYLINDER_ANCHOR_BORDER(externalradius,externalradius)
      }
      break;

    case PGFsouthwest:
      {
        XW_TIKZ_SHAPE_CYLINDER_ANCHOR_BORDER(-externalradius,-externalradius)
      }
      break;

    case PGFsoutheast:
      {
        XW_TIKZ_SHAPE_CYLINDER_ANCHOR_BORDER(externalradius,-externalradius)
      }
      break;

    case PGFnorthwest:
      {
        XW_TIKZ_SHAPE_CYLINDER_ANCHOR_BORDER(-externalradius,externalradius)
      }
      break;

    case PGFbeforetop:
      {
        QPointF p = beforetopanchor + centerPoint;
        anchorPos = state->rotatePointAround(p,centerPoint,rotate);
      }
      break;

    case PGFtop:
      {
        x = beforetop.x() + xradius + centerPoint.x();
        y = centerPoint.y();
        QPointF p(x,y);
        anchorPos = state->rotatePointAround(p,centerPoint,rotate);
      }
      break;

    case PGFaftertop:
      {
        x = beforetopanchor.x() + centerPoint.x();
        y = -beforetopanchor.y() + centerPoint.y();
        QPointF p(x,y);
        anchorPos = state->rotatePointAround(p,centerPoint,rotate);
      }
      break;

    case PGFbeforebottom:
      {
        x = afterbottomanchor.x() + centerPoint.x();
        y = -afterbottomanchor.y() + centerPoint.y();
        QPointF p(x,y);
        anchorPos = state->rotatePointAround(p,centerPoint,rotate);
      }
      break;

    case PGFbottom:
      {
        x = afterbottom.x() - xradius + centerPoint.x();
        y = centerPoint.y();
        QPointF p(x,y);
        anchorPos = state->rotatePointAround(p,centerPoint,rotate);
      }
      break;

    case PGFafterbottom:
      {
        QPointF p = afterbottomanchor + centerPoint;
        anchorPos = state->rotatePointAround(p,centerPoint,rotate);
      }
      break;
  }

  if (dopath)
  {
    
    if (state->cylinderUsesCustomFill)
    {
      x = xradius - outersep;
      xradius = x;
      y = yradius - outersep;
      yradius = y;
      state = state->save(false);
      state->addShift(centerPoint.x(), centerPoint.y());
      state->rotate(rotate);
      state->moveTo(afterbottom);
      state->addArc(90,270,xradius,yradius);
      state->lineTo(beforetop.x(), -beforetop.y());
      state->addArc(270,90,xradius,yradius);
      state->closePath();
      state->setFillColor(state->cylinderBodyFill);
      state = state->restore();
      state = state->save(false);
      state->moveTo(beforetop);
      state->addArc(90,-270,xradius,yradius);
      state->closePath();
      state->setFillColor(state->cylinderEndFill);
      state = state->restore();
    }
    
    state = state->save(false);
    x = xradius - outersep;
    xradius = x;
    y = yradius - outersep;
    yradius = y;
    state->addShift(centerPoint.x(), centerPoint.y());
    state->rotate(rotate);
    state->moveTo(afterbottom);
    state->addArc(90,270,xradius,yradius);
    state->lineTo(beforetop.x(), -beforetop.y());
    state->addArc(-90,90,xradius,yradius);
    state->closePath();
    state->moveTo(beforetop);
    state->addArc(90,270,xradius,yradius);

    state = state->restore();

    doContent();
  }
}

void XWTikzShape::dartShape(bool dopath)
{
   double halftipangle = state->dartTipAngle / 2;
   double halftailangle = state->dartTailAngle / 2;
   double cothalftipangle = 1 / tan(halftipangle);
   double x = state->outerXSep;
   double y = state->outerYSep;
   if (x < y)
     x = y;
  double outersep = x;

  x = 0.5 * boxWidth + state->innerXSep;
  y = 0.5 * (boxHeight + boxDepth) + state->innerYSep;
  double rotate = state->shapeBorderRotate;
  double xa = 0;
  double ya = 0;
  double mathresult = 0;
  if (state->shapeBorderUsesIncircle)
  {
    if (x < y)
      x = y;

    x *= 1.41421;
    xa = cothalftipangle * x + x;
  }
  else
  {
    mathresult = (int)(state->shapeBorderRotate) % 360;
    if (mathresult < 0)
      mathresult += 360;
    int counta = (int)mathresult;
    counta += 45;
    counta /= 90;
    counta *= 90;
    rotate = counta;
    if (counta == 0)
    {
      xa = x;
      x = y;
      y = xa;
    }
    else
    {
      if (counta == 270)
      {
        xa = x;
        x = y;
        y = xa;
      }
    }
    xa = cothalftipangle * y + 2 * x;
  }
  mathresult = halftailangle - halftipangle;
  mathresult = 1 / sin(mathresult);
  ya = mathresult * xa;
  mathresult = sin(halftipangle);
  ya = mathresult * ya; 
  mathresult = cos(halftipangle);
  ya = mathresult * ya;
  double xb = cothalftipangle * ya;
  double xc = -xa + xb;
  y = state->minHeight;
  if (xb < y)
  {
    mathresult = 1 / xb;
    y = mathresult * y;
    xa = xa * y;
    xc = xc * y;
    ya = ya * y;
    x = x * y;
  }

  y = state->minWidth;
  y = 0.5 * y;
  if (ya < y)
  {
    mathresult = 1 / ya;
    ya = y;
    y = mathresult * y;
    xa = xa * y;
    xc = xc * y;
    x = x * y;
  }
  double dartlength = xa;
  double deltax = x;
  double taillength = xc;
  double halftailseparation = ya;

  QPointF tippoint(centerPoint.x() + dartlength - deltax, centerPoint.y());
  QPointF tailcenterpoint(centerPoint.x() - deltax, centerPoint.y());
  QPointF lefttailpoint(centerPoint.x() - deltax - taillength, centerPoint.y() + halftailseparation);
  QPointF righttailpoint(centerPoint.x() - deltax - taillength, centerPoint.y() - halftailseparation);

  mathresult = 1 / sin(halftipangle);
  x = outersep * mathresult;
  y = 0;
  QPointF tipmiter(x,y);

  mathresult = 1 / sin(halftipangle);
  x = -outersep * mathresult;
  y = 0;
  QPointF tailcentermiter(x,y);

  mathresult = (halftailangle - halftipangle) / 2;
  double angle = mathresult;
  mathresult = 1 / sin(mathresult);
  x = outersep * mathresult;
  mathresult = angle + 90;
  mathresult = mathresult - halftailangle;
  ya = x * cos(mathresult);
  xa = x * sin(mathresult);
  x = -xa;
  y = ya;
  QPointF lefttailmiter(x,y);
  QPointF righttailmiter(-xa,-ya);

  QPointF tipborderpoint = tippoint + tipmiter;
  QPointF tailcenterborderpoint = tailcenterpoint + tailcentermiter;
  QPointF lefttailborderpoint = lefttailpoint + lefttailmiter;
  QPointF righttailborderpoint = righttailpoint + righttailmiter;

  double angletotip = state->angleBetweenPoints(centerPoint,tipborderpoint);
  double angletotailcenter = state->angleBetweenPoints(centerPoint,tailcenterborderpoint);
  double angletolefttail = state->angleBetweenPoints(centerPoint,lefttailborderpoint);
  double angletorighttail = state->angleBetweenPoints(centerPoint,righttailborderpoint);

  QPointF rotatedbasepoint = state->rotatePointAround(basePoint,centerPoint,-rotate);
  double baseangletotip = state->angleBetweenPoints(rotatedbasepoint,tipborderpoint);
  double baseangletotailcenter = state->angleBetweenPoints(rotatedbasepoint,tailcenterborderpoint);
  double baseangletolefttail = state->angleBetweenPoints(rotatedbasepoint,lefttailborderpoint);
  double baseangletorighttail = state->angleBetweenPoints(rotatedbasepoint,righttailborderpoint);

  QPointF rotatedmidpoint = state->rotatePointAround(midPoint,centerPoint,-rotate);
  double midangletotip = state->angleBetweenPoints(rotatedmidpoint,tipborderpoint);
  double midangletotailcenter = state->angleBetweenPoints(rotatedmidpoint,tailcenterborderpoint);
  double midangletolefttail = state->angleBetweenPoints(rotatedmidpoint,lefttailborderpoint);
  double midangletorighttail = state->angleBetweenPoints(rotatedmidpoint,righttailborderpoint);

  tippoint = state->rotatePointAround(tippoint,centerPoint,rotate);
  tailcenterpoint = state->rotatePointAround(tailcenterpoint,centerPoint,rotate);
  lefttailpoint = state->rotatePointAround(lefttailpoint,centerPoint,rotate);
  righttailpoint = state->rotatePointAround(righttailpoint,centerPoint,rotate);

  xa = lefttailborderpoint.x() - tipborderpoint.x();
  ya = lefttailborderpoint.y() - righttailborderpoint.y();
  if (xa < 0)
    xa = -xa;
  if (ya < 0)
    ya = -ya;
  double externalradius = ya;
  if (xa > ya)
    externalradius = xa;

  tipborderpoint = state->rotatePointAround(tipborderpoint,centerPoint,rotate);
  tailcenterborderpoint = state->rotatePointAround(tailcenterborderpoint,centerPoint,rotate);
  lefttailborderpoint = state->rotatePointAround(lefttailborderpoint,centerPoint,rotate);
  righttailborderpoint = state->rotatePointAround(righttailborderpoint,centerPoint,rotate);

#define XW_TIKZ_SHAPE_DART_ANCHOR_BORDER(ex,ey) \
  QPointF ep(referencePoint.x() + ex,referencePoint.y() + ey); \
  mathresult = state->angleBetweenPoints(referencePoint,ep); \
  mathresult -= rotate; \
  mathresult = (int)mathresult % 360; \
  double externalangle = mathresult; \
  if (referencePoint == basePoint) \
  {\
    angletotip = baseangletotip; \
    angletotailcenter = baseangletotailcenter; \
    angletolefttail = baseangletolefttail; \
    angletorighttail = baseangletorighttail; \
  }\
  else \
  {\
    if (referencePoint == midPoint) \
    {\
      angletotip = midangletotip;\
      angletotailcenter = midangletotailcenter; \
      angletolefttail = midangletolefttail; \
      angletorighttail = midangletorighttail;\
    }\
  }\
  QPointF firstpoint,secondpoint;\
  if (externalangle < angletotip) \
  {\
    firstpoint = tipborderpoint; \
    secondpoint = righttailborderpoint; \
    if (externalangle < angletolefttail) \
    {\
      firstpoint = lefttailborderpoint; \
      secondpoint = tipborderpoint; \
    }\
    else \
    {\
      if (externalangle < angletotailcenter) \
      {\
        firstpoint = lefttailborderpoint; \
        secondpoint =  tailcenterborderpoint; \
      }\
      else \
      {\
        if (externalangle < angletorighttail) \
        {\
          firstpoint = righttailborderpoint; \
          secondpoint = tailcenterborderpoint; \
        }\
        else \
        {\
          firstpoint = tipborderpoint; \
          secondpoint = righttailborderpoint; \
        } \
      }\
    }\
  }\
  anchorPos = state->intersectionOfLines(referencePoint,ep,firstpoint,secondpoint);

  switch (state->anchor)
  {
    default:
      {
        double delta = state->angle;
        double externalx = cos(delta);
        double externaly = sin(delta);
        XW_TIKZ_SHAPE_DART_ANCHOR_BORDER(externalx,externaly)
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

    case PGFbasewest:
      {
        referencePoint = basePoint;
        XW_TIKZ_SHAPE_DART_ANCHOR_BORDER(-externalradius,0)
      }
      break;

    case PGFbaseeast:
      {
        referencePoint = basePoint;
        XW_TIKZ_SHAPE_DART_ANCHOR_BORDER(externalradius,0)
      }
      break;

    case PGFmid:
      anchorPos = midPoint;
      break;

    case PGFmidwest:
      {
        referencePoint = midPoint;
        XW_TIKZ_SHAPE_DART_ANCHOR_BORDER(-externalradius,0)
      }
      break;

    case PGFmideast:
      {
        referencePoint = midPoint;
        XW_TIKZ_SHAPE_DART_ANCHOR_BORDER(externalradius,0)
      }
      break;

    case PGFnorth:
      {
        XW_TIKZ_SHAPE_DART_ANCHOR_BORDER(0,externalradius)
      }
      break;

    case PGFsouth:
      {
        XW_TIKZ_SHAPE_DART_ANCHOR_BORDER(0,-externalradius)
      }
      break;

    case PGFeast:
      {
        XW_TIKZ_SHAPE_DART_ANCHOR_BORDER(externalradius,0)
      }
      break;

    case PGFwest:
      {
        XW_TIKZ_SHAPE_DART_ANCHOR_BORDER(-externalradius,0)
      }
      break;

    case PGFnortheast:
      {
        XW_TIKZ_SHAPE_DART_ANCHOR_BORDER(externalradius,externalradius)
      }
      break;

    case PGFsouthwest:
      {
        XW_TIKZ_SHAPE_DART_ANCHOR_BORDER(-externalradius,-externalradius)
      }
      break;

    case PGFsoutheast:
      {
        XW_TIKZ_SHAPE_DART_ANCHOR_BORDER(externalradius,-externalradius)
      }
      break;

    case PGFnorthwest:
      {
        XW_TIKZ_SHAPE_DART_ANCHOR_BORDER(-externalradius,externalradius)
      }
      break;

    case PGFtip:
      anchorPos = tipborderpoint;
      break;

    case PGFlefttail:
      anchorPos = lefttailborderpoint;
      break;

    case PGFrighttail:
      anchorPos = righttailborderpoint;
      break;

    case PGFtailcenter:
      anchorPos = tailcenterborderpoint;
      break;

    case PGFleftside:
      anchorPos = state->lineAtTime(0.5,tipborderpoint,lefttailborderpoint);
      break;

    case PGFrightside:
      anchorPos = state->lineAtTime(0.5,tipborderpoint,righttailborderpoint);
      break;
  }

  if (dopath)
  {
    state = state->save(false);
    state->moveTo(tippoint);
    state->lineTo(lefttailpoint);
    state->lineTo(tailcenterpoint);
    state->lineTo(righttailpoint);
    state->closePath();
    state = state->restore();

    doContent();
  }
}

void XWTikzShape::diamondShape(bool dopath)
{
  centerPoint.setX(0);
  centerPoint.setY(0);
  basePoint.setX(0);
  basePoint.setY(-0.5 * boxHeight + 0.5 * boxDepth);
  midPoint.setX(0);
  midPoint.setY(-0.5 * boxHeight + 0.5 * boxDepth + 5);

  double xa = 0.5 * boxWidth + state->innerXSep;
  double ya = 0.5 * (boxHeight + boxDepth) + state->innerYSep;
  double x = xa + state->aspect * ya;
  double y = ya + state->aspectInverse * xa;
  double xb = 0.5 * state->minWidth;
  if (x < xb)
    x = xb;
  double yb = 0.5 * state->minHeight;
  if (y < yb)
    y = yb;
  x += state->outerXSep;
  y += state->outerYSep;
  QPointF outernortheast(x,y);

  switch (state->anchor)
  {
    default:
      {
        double delta = state->angle;
        double xa = cos(delta);
        double ya = sin(delta);
        x = outernortheast.x();
        y = outernortheast.y();
        if (xa < 0)
          x = -x;
        if (ya < 0)
          y = -y;

        QPointF o(0,0), a(xa,ya),b(x,0),c(0,y);
        anchorPos = state->intersectionOfLines(o,a,b,c);
      }
      break;

    case PGFtext:
      anchorPos.setX(-0.5 * boxWidth);
      anchorPos.setY(-0.5 * boxHeight + 0.5 * boxDepth);
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
      anchorPos = outernortheast;
      anchorPos.setX(0);
      break;

    case PGFsouth:
      anchorPos.setY(-outernortheast.y());
      anchorPos.setX(0);
      break;

    case PGFwest:
      anchorPos.setX(-outernortheast.x());
      anchorPos.setY(0);
      break;

    case PGFnorthwest:
      anchorPos.setX(-0.5 * outernortheast.x());
      anchorPos.setY(0.5 * outernortheast.y());
      break;
    
    case PGFsouthwest:
      anchorPos.setX(-0.5 * outernortheast.x());
      anchorPos.setY(-0.5 * outernortheast.y());
      break;

    case PGFeast:
      anchorPos.setX(outernortheast.x());
      anchorPos.setY(0);
      break;

    case PGFnortheast:
      anchorPos.setX(0.5 * outernortheast.x());
      anchorPos.setY(0.5 * outernortheast.y());
      break;

    case PGFsoutheast:
      anchorPos.setX(0.5 * outernortheast.x());
      anchorPos.setY(-0.5 * outernortheast.y());
      break;
  }

  if (dopath)
  {
    state = state->save(false);
    double xc = outernortheast.x() - 1.414213 * state->outerXSep;
    double yc = outernortheast.y() - 1.414213 * state->outerYSep;
    state->moveTo(xc,0);
    state->lineTo(0,yc);
    state->lineTo(-xc,0);
    state->lineTo(0,-yc);
    state->closePath();
    state = state->restore();

    doContent();
  }
}

void XWTikzShape::ellipseShape(bool dopath)
{
  double y = centerPoint.y() + state->innerYSep;
  double x = centerPoint.x() + state->innerXSep;
  x *= 1.4142136;
  y *= 1.4142136;
  double yc = state->minHeight;
  if (y < 0.5 * yc)
    y = 0.5 * yc;
  double xc = state->minWidth;
  if (x < 0.5 * xc)
    x = 0.5 * xc;

  x += state->outerXSep;
  y += state->outerYSep;

  switch (state->anchor)
  {
    default:
      {
        double delta = state->angle;
        double xa = cos(delta);
        double ya = sin(delta);
        QPointF pd(xa,ya);
        QPointF ur(state->xradius,state->yradius);

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

    case PGFmid:
      anchorPos = midPoint;
      break;

    case PGFbase:
      anchorPos = basePoint;
      break;

    case PGFnorth:
      anchorPos.setX(centerPoint.x());
      anchorPos.setY(centerPoint.y() + state->yradius);
      break;

    case PGFsouth:
      anchorPos.setX(centerPoint.x());
      anchorPos.setY(centerPoint.y() - state->yradius);
      break;

    case PGFwest:
      anchorPos.setX(centerPoint.x() - state->xradius);
      anchorPos.setY(centerPoint.y());
      break;

    case PGFmidwest:
      anchorPos.setX(centerPoint.x() - state->xradius);
      anchorPos.setY(5);
      break;

    case PGFbasewest:
      anchorPos.setX(centerPoint.x() - state->xradius);
      anchorPos.setY(0);
      break;

    case PGFnorthwest:
      anchorPos.setX(centerPoint.x() - 0.707107 * state->xradius);
      anchorPos.setY(centerPoint.y() + 0.707107 * state->yradius);
      break;

    case PGFsouthwest:
      anchorPos.setX(centerPoint.x() - 0.707107 * state->xradius);
      anchorPos.setY(centerPoint.y() - 0.707107 * state->yradius);
      break;

    case PGFeast:
      anchorPos.setX(centerPoint.x() + state->xradius);
      anchorPos.setY(centerPoint.y());
      break;

    case PGFmideast:
      anchorPos.setX(centerPoint.x() + state->xradius);
      anchorPos.setY(5);
      break;

    case PGFbaseeast:
      anchorPos.setX(centerPoint.x() + state->xradius);
      anchorPos.setY(0);
      break;

    case PGFnortheast:
      anchorPos.setX(centerPoint.x() + 0.707107 * state->xradius);
      anchorPos.setY(centerPoint.y() + 0.707107 * state->yradius);
      break;

    case PGFsoutheast:
      anchorPos.setX(centerPoint.x() + 0.707107 * state->xradius);
      anchorPos.setY(centerPoint.y() - 0.707107 * state->yradius);
      break;
  }

  if (dopath)
  {
    state = state->save(false);
    double xr = state->xradius - state->outerXSep;
    double yr = state->yradius - state->outerYSep;
    QPointF a(xr,0),b(0,yr);
    state->moveTo(centerPoint);
    state->addEllipse(centerPoint,a,b);
    state = state->restore();

    doContent();
  }
}

void XWTikzShape::isosceleTriangleShape(bool dopath)
{
  double x = state->isoscelesTriangleApexAngle;
  x /= 2;
  double halfapexangle = x;
  double tanhalfapexangle = tan(halfapexangle);
  double cothalfapexangle = 1 / tan(halfapexangle);
  double sinhalfapexangle = sin(halfapexangle);
  double cosechalfapexangle = 1 / sinhalfapexangle;
  x = state->innerXSep + 0.5 * boxWidth;
  double y = state->innerYSep + 0.5 * (boxHeight + boxDepth);
  double rotate = state->shapeBorderRotate;
  double xa = 0;
  double ya = 0;
  double mathresult = 0;
  if (state->shapeBorderUsesIncircle)
  {
    if (x < y)
      x = y;
    else
      y = x;
    x *= 1.414213;
    y *= 1.414213;
    xa = x + cosechalfapexangle * x;
    ya = tanhalfapexangle * xa;
  }
  else
  {
    mathresult = (int)(state->shapeBorderRotate) % 360;
    if (mathresult < 0)
      mathresult += 360;
    int counta = (int)mathresult;
    counta += 45;
    counta /= 90;
    counta *= 90;
    rotate = counta;
    if (counta == 0)
    {
      xa = x;
      x = y;
      y = xa;
    }
    else
    {
      if (counta == 270)
      {
        xa = x;
        x = y;
        y = xa;
      }
    }
    xa = 2 * x;
    ya = tanhalfapexangle * xa;
    xa += cothalfapexangle * y;
    ya += y;
  }

  if (xa == 0)
    xa = state->minHeight;
  double yb = state->minWidth;
  if (ya < 0.5 * yb)
  {
    if (state->isoscelesTriangleStretches)
    {
      ya = 0.5 * yb;
      mathresult = ya / xa;
      mathresult = atan(mathresult);
      halfapexangle = mathresult;
    }
    else
    {
      ya = 0.5 * yb;
      xa = cothalfapexangle * ya;
    }
  }

  double xb = state->minHeight;
  if (xa < xb)
  {
    if (state->isoscelesTriangleStretches)
    {
      xa = xb;
      mathresult = ya / xa;
      mathresult = atan(mathresult);
      halfapexangle = mathresult;
    }
    else
    {
      xa = xb;
      ya = tanhalfapexangle * xa;
    }
  }

  double tempdima = 0;
  double yc = 0;
  if (state->shapeBorderUsesIncircle)
  {
    double xc = xa;
    mathresult = sin(halfapexangle);
    xc = mathresult * xc;
    yc = 1 + mathresult;
    mathresult = xc / yc;
    tempdima = mathresult;
  }
  else
  {
    double xc = ya - y;
    mathresult = cos(halfapexangle);
    xc *= mathresult;
    mathresult = sin(halfapexangle);
    xc -= mathresult * x;
    xc -= mathresult * x;
    yc = mathresult + 1;
    mathresult = xc / yc;
    tempdima = mathresult + x;
  }

  QPointF apex(xa - tempdima,0);
  QPointF lowerleft(-tempdima,ya);
  xb = state->outerXSep;
  yb = state->outerYSep;
  if (xb < yb)
    xb = yb;
  QPointF apexanchor(apex.x() + cosechalfapexangle * xb,apex.y());
  yc = -halfapexangle + 90;
  yc /= 2;
  mathresult = 1 / tan(yc);
  QPointF lowerleftanchor(lowerleft.x() - xb,lowerleft.y() + mathresult * xb);
  QPointF lowerrightanchor(lowerleftanchor.x(),-lowerleftanchor.y());

  QPointF o(0,0);

#define XW_TIKZ_SHAPE_ISOSCELETRIANGLE_ANCHOR_BORDER(ex,ey) \
  QPointF ep(referencePoint.x() + ex,referencePoint.y() + ey); \
  mathresult = state->angleBetweenPoints(referencePoint,ep); \
  mathresult -= rotate; \
  mathresult = (int)mathresult % 360; \
  if (mathresult < 0) \
    mathresult += 360; \
  double externalangle = mathresult; \
  QPointF firstpoint,secondpoint; \
  QPointF pp = centerPoint + lowerrightanchor; \
  mathresult = state->angleBetweenPoints(referencePoint,pp); \
  if (externalangle < mathresult) \
  {\
    pp = centerPoint + lowerleftanchor; \
    mathresult = state->angleBetweenPoints(referencePoint,pp); \
    if (externalangle < mathresult) \
    { \
      pp = centerPoint + apexanchor; \
      mathresult = state->angleBetweenPoints(referencePoint,pp); \
      if (externalangle > mathresult) \
      {\
        firstpoint = apexanchor; \
        secondpoint = lowerleftanchor;\
      }\
      else \
      {\
        firstpoint = apexanchor; \
        secondpoint = lowerrightanchor; \
      }\
    }\
    else \
    {\
      firstpoint = lowerleftanchor; \
      secondpoint = lowerrightanchor; \
    }\
  }\
  else \
  {\
    firstpoint = lowerrightanchor; \
    secondpoint = apexanchor; \
  }\
  QPointF p1 = centerPoint + firstpoint; \
  QPointF p2 = centerPoint + secondpoint; \
  p1 = state->rotatePointAround(p1,centerPoint,rotate); \
  p2 = state->rotatePointAround(p2,centerPoint,rotate); \
  anchorPos = state->intersectionOfLines(referencePoint,ep,p1,p2);

  switch (state->anchor)
  {
    default:
      {
        double delta = state->angle;
        double externalx = cos(delta);
        double externaly = sin(delta);
        XW_TIKZ_SHAPE_ISOSCELETRIANGLE_ANCHOR_BORDER(externalx,externaly)
      }
      break;

    case PGFapex:
      {
        QPointF p = state->rotatePointAround(apexanchor,o,rotate);
        anchorPos = centerPoint + p;
      }
      break;

    case PGFleftcorner:
      {
        QPointF p = state->rotatePointAround(lowerleftanchor,o,rotate);
        anchorPos = centerPoint + p;
      }
      break;

    case PGFrightcorner:
      {
        QPointF p = state->rotatePointAround(lowerrightanchor,o,rotate);
        anchorPos = centerPoint + p;
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

    case PGFmidwest:
      {
        referencePoint = midPoint;
        XW_TIKZ_SHAPE_ISOSCELETRIANGLE_ANCHOR_BORDER(-1,0)
      }
      break;

    case PGFmideast:
      {
        referencePoint = midPoint;
        XW_TIKZ_SHAPE_ISOSCELETRIANGLE_ANCHOR_BORDER(1,0)
      }
      break;

    case PGFbase:
      anchorPos = basePoint;
      break;

    case PGFbasewest:
      {
        referencePoint = basePoint;
        XW_TIKZ_SHAPE_ISOSCELETRIANGLE_ANCHOR_BORDER(-1,0)
      }
      break;

    case PGFbaseeast:
      {
        referencePoint = basePoint;
        XW_TIKZ_SHAPE_ISOSCELETRIANGLE_ANCHOR_BORDER(1,0)
      }
      break;

    case PGFnorth:
      {
        XW_TIKZ_SHAPE_ISOSCELETRIANGLE_ANCHOR_BORDER(0,1)
      }
      break;

    case PGFsouth:
      {
        XW_TIKZ_SHAPE_ISOSCELETRIANGLE_ANCHOR_BORDER(0,-1)
      }
      break;

    case PGFeast:
      {
        XW_TIKZ_SHAPE_ISOSCELETRIANGLE_ANCHOR_BORDER(1,0)
      }
      break;

    case PGFwest:
      {
        XW_TIKZ_SHAPE_ISOSCELETRIANGLE_ANCHOR_BORDER(-1,0)
      }
      break;

    case PGFnortheast:
      {
        XW_TIKZ_SHAPE_ISOSCELETRIANGLE_ANCHOR_BORDER(1,1)
      }
      break;

    case PGFsouthwest:
      {
        XW_TIKZ_SHAPE_ISOSCELETRIANGLE_ANCHOR_BORDER(-1,-1)
      }
      break;

    case PGFsoutheast:
      {
        XW_TIKZ_SHAPE_ISOSCELETRIANGLE_ANCHOR_BORDER(1,-1)
      }
      break;

    case PGFnorthwest:
      {
        XW_TIKZ_SHAPE_ISOSCELETRIANGLE_ANCHOR_BORDER(-1,1)
      }
      break;

    case PGFleftside:
      {
        QPointF p = state->lineAtTime(0.5,lowerleftanchor,apexanchor);
        p = p + centerPoint;
        anchorPos = state->rotatePointAround(p,centerPoint,rotate);
      }
      break;

    case PGFrightside:
      {
        QPointF p = state->lineAtTime(0.5,lowerrightanchor,apexanchor);
        p = p + centerPoint;
        anchorPos = state->rotatePointAround(p,centerPoint,rotate);
      }
      break;

    case PGFlowerside:
      {
        QPointF p = state->lineAtTime(0.5,lowerleftanchor,lowerrightanchor);
        p = p + centerPoint;
        anchorPos = state->rotatePointAround(p,centerPoint,rotate);
      }
      break;
  }

  if (dopath)
  {
    state = state->save(false);
    state->addShift(centerPoint.x(),centerPoint.y());
    state->rotate(rotate);
    state->moveTo(apex);
    state->lineTo(lowerleft);
    state->lineTo(lowerleft.x(),-lowerleft.y());
    state = state->restore();

    doContent();
  }
}

void XWTikzShape::kiteShape(bool dopath)
{
  double x = state->outerXSep;
  double y = state->outerYSep;
  if (x < y)
    x = y;
  double outersep = x;

  double halfuppervertexangle = state->kiteUpperVertexAngle / 2;
  double halflowervertexangle = state->kiteLowerVertexAngle / 2;
  x = state->innerXSep + 0.5 * boxWidth;
  y = state->innerYSep + 0.5 * (boxHeight + boxDepth);
  double rotate = state->shapeBorderRotate;
  double xa = 0;
  double xb = 0;
  double ya = 0;
  double yb = 0;
  double yc = 0;
  double mathresult = 0;
  double deltay = 0;
  if (state->shapeBorderUsesIncircle)
  {
    if (x < y)
      x = y;
    x *= 1.414213;
    mathresult = sin(halfuppervertexangle);
    double sinehalfuppervertexangle = mathresult;
    mathresult = 1 / mathresult;
    double cosechalfuppervertexangle = mathresult;
    ya = mathresult * x;
    mathresult = sin(halflowervertexangle);
    double sinehalflowervertexangle = mathresult;
    mathresult = 1 / mathresult;
    yb = mathresult * x;
    yc = x;
    mathresult = halfuppervertexangle + halflowervertexangle;
    mathresult = 1 / sin(mathresult);
    yc = mathresult * yc;
    mathresult = sinehalfuppervertexangle + sinehalflowervertexangle;
    yc = mathresult * yc;
    mathresult = cos(halfuppervertexangle);
    yc = -cosechalfuppervertexangle * yc;
    yc += cosechalfuppervertexangle * x;
    deltay = yc;
    ya += -yc;
    yb += yc;
    mathresult = tan(halfuppervertexangle);
    xa = mathresult * ya;
  }
  else
  {
    x *= 2;
    y *= 2;

    mathresult = (int)(state->shapeBorderRotate) % 360;
    if (mathresult < 0)
      mathresult += 360;
    int counta = (int)mathresult;
    counta += 45;
    counta /= 90;
    counta *= 90;
    rotate = counta;

    if (counta == 0)
    {
      xa = x;
      x = y;
      y = xa;
    }
    else
    {
      if (counta == 270)
      {
        xa = x;
        x = y;
        y = xa;
      }
    }

    mathresult = halfuppervertexangle + halflowervertexangle;
    mathresult = sin(mathresult);
    mathresult = 1 / mathresult;
    ya = mathresult * y;
    mathresult = cos(halfuppervertexangle);
    ya = mathresult * ya;
    mathresult = sin(halflowervertexangle);
    ya = mathresult * ya;
    yb = y - ya;
    yc = 0.5 * y - ya;
    deltay = yc;
    mathresult = tan(halfuppervertexangle);
    xa = 0.5 * x + mathresult * ya;
    xb = 0.5 * x;
    mathresult = 1 / mathresult;
    ya += mathresult * xb;
    mathresult = tan(halflowervertexangle);
    mathresult = 1 / mathresult;
    yb += mathresult * xb;
  }
  yc = state->minHeight;
  y = ya + yb;
  if (y < yc)
  {
    mathresult = 1 / y;
    yc = mathresult * yc;
    xa = yc * xa;
    ya = yc * ya;
    yb = yc * yb;
  }
  x = 2 * xa;
  double xc = state->minWidth;
  if (x < xc)
  {
    mathresult = 1 / x;
    xc = mathresult * xc;
    xa = xc * xa;
    ya = xc * ya;
    yb = xc * yb;
  }
  double kitehalfwidth = xa;
  double kiteheight = ya;
  double kitedepth = yb;
  QPointF toppoint(centerPoint.x(),centerPoint.y() + deltay + kiteheight);
  QPointF bottompoint(centerPoint.x(),centerPoint.y() + deltay - kitedepth);
  QPointF leftpoint(centerPoint.x() - kitehalfwidth, centerPoint.y() + deltay);
  QPointF rightpoint(centerPoint.x() + kitehalfwidth, centerPoint.y() + deltay);

  mathresult = 1 / sin(halfuppervertexangle);
  y = mathresult * outersep;
  double topmiter = y;

  mathresult = 1 / sin(halflowervertexangle);
  y = mathresult * outersep;
  double bottommiter = y;

  mathresult = 180 - halflowervertexangle;
  mathresult -= halfuppervertexangle;
  mathresult /= 2;
  mathresult = sin(mathresult);
  mathresult = 1/ mathresult;
  xa = mathresult * outersep;
  mathresult = halfuppervertexangle - halflowervertexangle;
  mathresult /= 2;
  double angle = mathresult;
  QPointF rightmiter(xa * cos(angle),xa * sin(angle));

  angle = 180 - angle;
  QPointF leftmiter(xa * cos(angle),xa * sin(angle));

  QPointF topborderpoint(toppoint.x(),toppoint.y() + topmiter);
  QPointF bottomborderpoint(bottompoint.x(), bottompoint.y() - bottommiter);
  QPointF leftborderpoint = leftpoint + leftmiter;
  QPointF rightborderpoint = rightpoint + rightmiter;

  double angletotoppoint = state->angleBetweenPoints(centerPoint,topborderpoint);
  double angletoleftpoint = state->angleBetweenPoints(centerPoint,leftborderpoint);
  double angletobottompoint = state->angleBetweenPoints(centerPoint,bottomborderpoint);
  double angletorightpoint = state->angleBetweenPoints(centerPoint,rightborderpoint);
  QPointF rotatedbasepoint = state->rotatePointAround(basePoint,centerPoint,-rotate);
  double baseangletotoppoint = state->angleBetweenPoints(rotatedbasepoint,topborderpoint);
  double baseangletoleftpoint = state->angleBetweenPoints(rotatedbasepoint,leftborderpoint);
  double baseangletobottompoint = state->angleBetweenPoints(rotatedbasepoint,bottomborderpoint);
  double baseangletorightpoint = state->angleBetweenPoints(rotatedbasepoint,rightborderpoint);

  QPointF rotatedmidpoint = state->rotatePointAround(midPoint,centerPoint,-rotate);
  double midangletotoppoint = state->angleBetweenPoints(rotatedmidpoint,topborderpoint);
  double midangletoleftpoint = state->angleBetweenPoints(rotatedmidpoint,leftborderpoint);
  double midangletobottompoint = state->angleBetweenPoints(rotatedmidpoint,bottomborderpoint);
  double midangletorightpoint = state->angleBetweenPoints(rotatedmidpoint,rightborderpoint);

  xa = leftborderpoint.x() - rightborderpoint.x();
  if (xa < 0)
    xa = -xa;
  ya = topborderpoint.y() - bottomborderpoint.y();
  if (ya < 0)
    ya = -ya;
  double externalradius = xa;
  if (ya > xa)
    externalradius = ya;
  toppoint = state->rotatePointAround(toppoint,centerPoint,rotate);
  bottompoint = state->rotatePointAround(bottompoint,centerPoint,rotate);
  leftpoint = state->rotatePointAround(leftpoint,centerPoint,rotate);
  rightpoint = state->rotatePointAround(rightpoint,centerPoint,rotate);

  topborderpoint = state->rotatePointAround(topborderpoint,centerPoint,rotate);
  bottomborderpoint = state->rotatePointAround(bottomborderpoint,centerPoint,rotate);
  leftborderpoint = state->rotatePointAround(leftborderpoint,centerPoint,rotate);
  rightborderpoint = state->rotatePointAround(rightborderpoint,centerPoint,rotate);

#define XW_TIKZ_SHAPE_KITE_ANCHOR_BORDER(ex,ey) \
  QPointF ep(referencePoint.x() + ex,referencePoint.y() + ey); \
  mathresult = state->angleBetweenPoints(referencePoint,ep); \
  mathresult -= rotate; \
  mathresult = (int)mathresult % 360; \
  double externalangle = mathresult; \
  if (referencePoint == basePoint) \
  { \
    angletotoppoint = baseangletotoppoint;\
    angletobottompoint = baseangletobottompoint; \
    angletoleftpoint = baseangletoleftpoint; \
    angletorightpoint = baseangletorightpoint; \
  } \
  else \
  { \
    if (referencePoint == midPoint) \
    {\
      angletotoppoint = midangletotoppoint; \
      angletobottompoint = midangletobottompoint; \
      angletoleftpoint = midangletoleftpoint; \
      angletorightpoint = midangletorightpoint; \
    }\
  } \
  QPointF firstpoint,secondpoint; \
  if (angletorightpoint < angletotoppoint) \
  {\
    if (externalangle < angletorightpoint) \
    { \
      firstpoint = rightborderpoint; \
      secondpoint = bottomborderpoint; \
    } \
    else \
    {\
      if (externalangle < angletotoppoint) \
      {\
        firstpoint = rightborderpoint; \
        secondpoint = topborderpoint; \
      }\
      else \
      {\
        if (externalangle < angletoleftpoint) \
        {\
          firstpoint = topborderpoint; \
          secondpoint = leftborderpoint; \
        }\
        else \
        {\
          if (externalangle < angletobottompoint) \
          {\
            firstpoint = leftborderpoint; \
            secondpoint = bottomborderpoint;\
          }\
          else \
          {\
            firstpoint = rightborderpoint; \
            secondpoint = bottomborderpoint;\
          }\
        }\
      }\
    }\
  } \
  else \
  {\
    if (externalangle < angletotoppoint) \
    {\
      firstpoint = rightborderpoint;\
      secondpoint = topborderpoint; \
    }\
    else \
    {\
      if (externalangle < angletoleftpoint) \
      {\
        firstpoint = leftborderpoint;\
        secondpoint = topborderpoint;\
      }\
      else \
      {\
        if (externalangle < angletobottompoint) \
        {\
          firstpoint = bottomborderpoint; \
          secondpoint = leftborderpoint; \
        }\
        else \
        {\
          if (externalangle < angletorightpoint)\
          {\
            firstpoint = rightborderpoint; \
            secondpoint = bottomborderpoint; \
          }\
          else \
          {\
            firstpoint = rightborderpoint; \
            secondpoint = topborderpoint; \
          }\
        }\
      }\
    }\
  }\
  anchorPos = state->intersectionOfLines(referencePoint,ep,firstpoint,secondpoint);

  switch (state->anchor)
  {
    default:
      {
        double delta = state->angle;
        double externalx = cos(delta);
        double externaly = sin(delta);
        XW_TIKZ_SHAPE_KITE_ANCHOR_BORDER(externalx,externaly)
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

    case PGFbasewest:
      {
        referencePoint = basePoint;
        XW_TIKZ_SHAPE_KITE_ANCHOR_BORDER(-externalradius,0)
      }
      break;

    case PGFbaseeast:
      {
        referencePoint = basePoint;
        XW_TIKZ_SHAPE_KITE_ANCHOR_BORDER(externalradius,0)
      }
      break;

    case PGFmid:
      anchorPos = midPoint;
      break;

    case PGFmidwest:
      {
        referencePoint = midPoint;
        XW_TIKZ_SHAPE_KITE_ANCHOR_BORDER(-externalradius,0)
      }
      break;

    case PGFmideast:
      {
        referencePoint = midPoint;
        XW_TIKZ_SHAPE_KITE_ANCHOR_BORDER(externalradius,0)
      }
      break;

    case PGFnorth:
      {
        XW_TIKZ_SHAPE_KITE_ANCHOR_BORDER(0,externalradius)
      }
      break;

    case PGFsouth:
      {
        XW_TIKZ_SHAPE_KITE_ANCHOR_BORDER(0,-externalradius)
      }
      break;

    case PGFeast:
      {
        XW_TIKZ_SHAPE_KITE_ANCHOR_BORDER(externalradius,0)
      }
      break;

    case PGFwest:
      {
        XW_TIKZ_SHAPE_KITE_ANCHOR_BORDER(-externalradius,0)
      }
      break;

    case PGFnortheast:
      {
        XW_TIKZ_SHAPE_KITE_ANCHOR_BORDER(externalradius,externalradius)
      }
      break;

    case PGFsouthwest:
      {
        XW_TIKZ_SHAPE_KITE_ANCHOR_BORDER(-externalradius,-externalradius)
      }
      break;

    case PGFsoutheast:
      {
        XW_TIKZ_SHAPE_KITE_ANCHOR_BORDER(externalradius,-externalradius)
      }
      break;

    case PGFnorthwest:
      {
        XW_TIKZ_SHAPE_KITE_ANCHOR_BORDER(-externalradius,externalradius)
      }
      break;

    case PGFuppervertex:
      anchorPos = topborderpoint;
      break;

    case PGFlowervertex:
      anchorPos = bottomborderpoint;
      break;

    case PGFleftvertex:
      anchorPos = leftborderpoint;
      break;

    case PGFrightvertex:
      anchorPos = rightborderpoint;
      break;

    case PGFupperleftside:
      anchorPos = state->lineAtTime(0.5,topborderpoint,leftborderpoint);
      break;

    case PGFlowerleftside:
      anchorPos = state->lineAtTime(0.5,bottomborderpoint,leftborderpoint);
      break;

    case PGFupperrightside:
      anchorPos = state->lineAtTime(0.5,topborderpoint,rightborderpoint);
      break;

    case PGFlowerrightside:
      anchorPos = state->lineAtTime(0.5,bottomborderpoint,rightborderpoint);
      break;
  }

  if (dopath)
  {
    state = state->save(false);
    state->moveTo(toppoint);
    state->lineTo(leftpoint);
    state->lineTo(bottompoint);
    state->lineTo(rightpoint);
    state->closePath();
    state = state->restore();

    doContent();
  }
}

void XWTikzShape::regularPolygonShape(bool dopath)
{
  int sides = state->regularPolygonSides;
  double anglestep = 360 / state->regularPolygonSides;
  double x = 0.5 * boxWidth + state->innerXSep;
  double y = 0.5 * (boxHeight + boxDepth) + state->innerYSep;
  if (y > x)
    x = y;
  x *= 1.41421;
  x *= 1 / sin((double)180 / state->regularPolygonSides);
  double xa = state->minWidth;
  double ya = state->minHeight;
  if (ya > xa)
    xa = ya;
  if (x < 0.5 * xa)
    x = 0.5 * xa;
  radius = x;
  xa = state->outerXSep;
  ya = state->outerYSep;
  if (ya > xa)
    xa = ya;
  x = xa * 1 / sin((double)90 - ((double)180 / state->regularPolygonSides));
  double anchorradius = x;
  double mathresult = sides;
  if ((sides % 2) == 1)
    mathresult = 90;
  else
    mathresult = 90 - anglestep / 2;
  mathresult += state->shapeBorderRotate;
  double startangle = mathresult;

#define XW_TIKZ_SHAPE_REGULARPOLYGON_ANCHOR_BORDER(ex,ey) \
  QPointF ep(centerPoint.x() + ex, centerPoint.y() + ey); \
  mathresult = state->angleBetweenPoints(centerPoint, ep); \
  mathresult -= startangle; \
  if (mathresult < 0) \
    mathresult += 360; \
  mathresult /= anglestep; \
  mathresult = floor(mathresult); \
  mathresult *= anglestep; \
  mathresult += startangle; \
  double firstangle = mathresult; \
  mathresult += anglestep; \
  double secondangle = mathresult; \
  x = centerPoint.x() + anchorradius * cos(firstangle); \
  y = centerPoint.y() + anchorradius * sin(firstangle); \
  QPointF p1(x,y); \
  x = centerPoint.x() + anchorradius * cos(secondangle); \
  y = centerPoint.y() + anchorradius * sin(secondangle); \
  QPointF p2(x,y); \
  anchorPos = state->intersectionOfLines(centerPoint,ep,p1,p2);

  switch (state->anchor)
  {
    default:
      {
        double delta = state->angle;
        double externalx = cos(delta);
        double externaly = sin(delta);
        XW_TIKZ_SHAPE_REGULARPOLYGON_ANCHOR_BORDER(externalx,externaly)
      }
      break;

    case PGFtext:
      anchorPos.setX(0);
      anchorPos.setY(0);
      break;

    case PGFcenter:
      anchorPos =  centerPoint;
      break;

    case PGFmid:
      anchorPos =  midPoint;
      break;

    case PGFbase:
      anchorPos = basePoint;
      break;

    case PGFnorth:
      {
        XW_TIKZ_SHAPE_REGULARPOLYGON_ANCHOR_BORDER(0,anchorradius)
      }
      break;

    case PGFsouth:
      {
        XW_TIKZ_SHAPE_REGULARPOLYGON_ANCHOR_BORDER(0,-anchorradius)
      }
      break;

    case PGFeast:
      {
        XW_TIKZ_SHAPE_REGULARPOLYGON_ANCHOR_BORDER(anchorradius,0)
      }
      break;

    case PGFwest:
      {
        XW_TIKZ_SHAPE_REGULARPOLYGON_ANCHOR_BORDER(-anchorradius,0)
      }
      break;

    case PGFnortheast:
      {
        XW_TIKZ_SHAPE_REGULARPOLYGON_ANCHOR_BORDER(anchorradius,anchorradius)
      }
      break;

    case PGFnorthwest:
      {
        XW_TIKZ_SHAPE_REGULARPOLYGON_ANCHOR_BORDER(-anchorradius,anchorradius)
      }
      break;

    case PGFsoutheast:
      {
        XW_TIKZ_SHAPE_REGULARPOLYGON_ANCHOR_BORDER(anchorradius,-anchorradius)
      }
      break;

    case PGFsouthwest:
      {
        XW_TIKZ_SHAPE_REGULARPOLYGON_ANCHOR_BORDER(-anchorradius,-anchorradius)
      }
      break;
  }

  if (dopath)
  {
    x = centerPoint.x() + radius * cos(startangle);
    y = centerPoint.y() + radius * sin(startangle);
    state = state->save(false);
    state->moveTo(x,y);
    double angle = startangle;
    for (int i = 1; i <= sides; i++)
    {
      if (i == sides)
        state->closePath();
      else
      {
        angle += anglestep;
        x = centerPoint.x() + radius * cos(angle);
        y = centerPoint.y() + radius * sin(angle);
        state->lineTo(x,y);
      }
    }

    state = state->restore();

    doContent();
  }
}

void XWTikzShape::semicircleShape(bool dopath)
{
  double x = state->outerXSep;
  double y = state->outerYSep;
  if (x < y)
    x = y;
  double outersep = x;
  x = 0.5 * boxWidth + state->innerXSep;
  y = 0.5 * (boxHeight + boxDepth) + state->innerYSep;
  double rotate = state->shapeBorderRotate;
  double halfheight = 0;
  if (state->shapeBorderUsesIncircle)
  {
    if (x < y)
      x = y;
    x *= 1.41421;
    halfheight = x;
    x += x;
  }
  else
  {
    int counta = (int)(state->shapeBorderRotate) % 360;
    counta += 45;
    counta /= 90;
    counta *= 90;
    if (counta < 0)
      counta += 360;
    rotate = counta;

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

    y += y;
    x = state->veclen(x,y);
    y *= 0.5;
    halfheight = y;
  }

  double defaultradius = x;
  double xa = state->minWidth;
  if (x < 0.5 * xa)
    x = 0.5 * xa;
  double ya = state->minHeight;
  if (x < ya)
    x = ya;
  double semicircleradius = x;

  x = centerPoint.x();
  ya = semicircleradius - defaultradius;
  ya *= 0.4;
  y = centerPoint.y() - ya;
  ya = halfheight;
  y -= ya;
  QPointF semicirclecenterpoint(x,y);

  x = semicircleradius + outersep;
  semicircleradius = x;
  y = centerPoint.y() - semicirclecenterpoint.y();
  if (y < 0)
    y = -y;
  double centerpointdiff = y;
  QPointF arcstartborder(semicirclecenterpoint.x() + semicircleradius,semicirclecenterpoint.y());
  QPointF arcendborder(semicirclecenterpoint.x() - semicircleradius,semicirclecenterpoint.y());
  QPointF arcstartcorner(arcstartborder.x(),arcstartborder.y()-outersep);
  QPointF arcendcorner(arcendborder.x(),arcendborder.y()-outersep);

  double angletoarcstartborder = state->angleBetweenPoints(centerPoint,arcstartborder);
  double angletoarcendborder = state->angleBetweenPoints(centerPoint,arcendborder);
  double angletoarcstartcorner = state->angleBetweenPoints(centerPoint,arcstartcorner);
  double angletoarcendcorner = state->angleBetweenPoints(centerPoint,arcendcorner);

  QPointF rotatedbasepoint = state->rotatePointAround(basePoint,centerPoint,-rotate);
  double baseangletoarcstartborder = state->angleBetweenPoints(rotatedbasepoint,arcstartborder);
  double baseangletoarcendborder = state->angleBetweenPoints(rotatedbasepoint,arcendborder);
  double baseangletoarcstartcorner = state->angleBetweenPoints(rotatedbasepoint,arcstartcorner);
  double baseangletoarcendcorner = state->angleBetweenPoints(rotatedbasepoint,arcendcorner);

  QPointF rotatedmidpoint = state->rotatePointAround(midPoint,centerPoint,-rotate);
  double midangletoarcstartborder = state->angleBetweenPoints(rotatedmidpoint,arcstartborder);
  double midangletoarcendborder = state->angleBetweenPoints(rotatedmidpoint,arcendborder);
  double midangletoarcstartcorner = state->angleBetweenPoints(rotatedmidpoint,arcstartcorner);
  double midangletoarcendcorner = state->angleBetweenPoints(rotatedmidpoint,arcendcorner);

  semicirclecenterpoint = state->rotatePointAround(semicirclecenterpoint,centerPoint,rotate);
  arcstartborder = state->rotatePointAround(arcstartborder,centerPoint,rotate);
  arcendborder = state->rotatePointAround(arcendborder,centerPoint,rotate);
  arcstartcorner = state->rotatePointAround(arcstartcorner,centerPoint,rotate);
  arcendcorner = state->rotatePointAround(arcendcorner,centerPoint,rotate);

  x = basePoint.x() - semicirclecenterpoint.x();
  y = basePoint.y() - semicirclecenterpoint.y();
  double basesemicirclecenterdiff = state->veclen(x,y);

  x = midPoint.x() - semicirclecenterpoint.x();
  y = midPoint.y() - semicirclecenterpoint.y();
  double midsemicirclecenterdiff = state->veclen(x,y);

#define XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_BORDER(ex,ey) \
  QPointF ep(referencePoint.x() + ex,referencePoint.y() + ey); \
  double mathresult = state->angleBetweenPoints(referencePoint,ep); \
  double externalangle = mathresult; \
  mathresult -= rotate; \
  if (mathresult < 0) \
    mathresult += 360; \
  double angle = mathresult; \
  if (referencePoint == basePoint) \
  { \
    angletoarcstartborder = baseangletoarcstartborder; \
    angletoarcendborder = baseangletoarcendborder; \
    angletoarcstartcorner = baseangletoarcstartcorner; \
    angletoarcendcorner = baseangletoarcendcorner; \
    centerpointdiff = basesemicirclecenterdiff; \
  } \
  else \
  { \
    if (referencePoint == midPoint) \
    { \
      angletoarcstartborder = midangletoarcstartborder; \
      angletoarcendborder = midangletoarcendborder; \
      angletoarcstartcorner = midangletoarcstartcorner; \
      angletoarcendcorner = midangletoarcendcorner; \
      centerpointdiff = midsemicirclecenterdiff; \
    } \
  } \
  QPointF firstpoint,secondpoint; \
  if (angle <= angletoarcstartborder) \
  { \
    if (angle > angletoarcstartcorner) \
    { \
      firstpoint = arcstartcorner; \
      secondpoint = arcstartborder; \
    } \
    else \
    { \
      if (angle > angletoarcendcorner) \
      { \
        firstpoint = arcendcorner; \
        secondpoint = arcstartcorner; \
      } \
      else \
      { \
        if (angle > angletoarcendborder) \
        { \
          firstpoint = arcendborder; \
          secondpoint = arcendcorner; \
        } \
      }\
    } \
  } \
  if (firstpoint.isNull()) \
  { \
    mathresult = state->angleBetweenLines(referencePoint,ep,semicirclecenterpoint,referencePoint); \
    mathresult = sin(mathresult); \
    double sineangle = mathresult; \
    x = semicircleradius; \
    mathresult = 1 / x; \
    double reciprocalradius = mathresult; \
    x = centerpointdiff; \
    x *= sineangle; \
    x *= reciprocalradius; \
    mathresult = asin(x); \
    x = mathresult; \
    x += externalangle; \
    angle = x; \
    x = semicirclecenterpoint.x() + semicircleradius * cos(angle); \
    y = semicirclecenterpoint.y() + semicircleradius * sin(angle); \
    anchorPos.setX(x); \
    anchorPos.setY(y); \
  } \
  else \
  { \
    anchorPos = state->intersectionOfLines(referencePoint,ep,firstpoint,secondpoint); \
  }

  switch (state->anchor)
  {
    default:
      {
        double delta = state->angle;
        double externalx = cos(delta);
        double externaly = sin(delta);
        XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_BORDER(externalx,externaly)
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
      {
        referencePoint = basePoint;
        XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_BORDER(-semicircleradius,0)
      }
      break;

    case PGFbaseeast:
      {
        referencePoint = basePoint;
        XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_BORDER(semicircleradius,0)
      }
      break;

    case PGFmidwest:
      {
        referencePoint = midPoint;
        XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_BORDER(-semicircleradius,0)
      }
      break;

    case PGFmideast:
      {
        referencePoint = midPoint;
        XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_BORDER(semicircleradius,0)
      }
      break;

    case PGFapex:
      {
        double angle = rotate + 90;
        x = semicirclecenterpoint.x() + semicircleradius * cos(angle);
        y = semicirclecenterpoint.y() + semicircleradius * sin(angle);
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      break;

    case PGFarcstart:
      anchorPos = arcstartcorner;
      break;

    case PGFarcend:
      anchorPos = arcendcorner;
      break;

    case PGFchordcenter:
      anchorPos = state->lineAtTime(0.5,arcstartcorner,arcendcorner);
      break;

    case PGFnorth:
      {
        x = semicircleradius * cos((double)90);
        y = semicircleradius * sin((double)90);
        XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_BORDER(x,y)
      }
      break;

    case PGFsouth:
      {
        x = semicircleradius * cos((double)270);
        y = semicircleradius * sin((double)270);
        XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_BORDER(x,y)
      }
      break;

    case PGFeast:
      {
        x = semicircleradius * cos((double)0);
        y = semicircleradius * sin((double)0);
        XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_BORDER(x,y)
      }
      break;

    case PGFwest:
      {
        x = semicircleradius * cos((double)180);
        y = semicircleradius * sin((double)180);
        XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_BORDER(x,y)
      }
      break;

    case PGFnorthwest:
      {
        x = semicircleradius * cos((double)135);
        y = semicircleradius * sin((double)135);
        XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_BORDER(x,y)
      }
      break;

    case PGFsouthwest:
      {
        x = semicircleradius * cos((double)225);
        y = semicircleradius * sin((double)225);
        XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_BORDER(x,y)
      }
      break;

    case PGFnortheast:
      {
        x = semicircleradius * cos((double)45);
        y = semicircleradius * sin((double)45);
        XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_BORDER(x,y)
      }
      break;

    case PGFsoutheast:
      {
        x = semicircleradius * cos((double)315);
        y = semicircleradius * sin((double)315);
        XW_TIKZ_SHAPE_SEMICIRCLE_ANCHOR_BORDER(x,y)
      }
      break;
  }

  if (dopath)
  {
    x = semicircleradius - outersep;
    semicircleradius = x;
    x = semicirclecenterpoint.x() + semicircleradius * cos(rotate);
    y = semicirclecenterpoint.y() + semicircleradius * sin(rotate);
    state = state->save(false);
    state->moveTo(x,y);
    state->rotate(rotate);
    state->addArc(0,180,semicircleradius,semicircleradius);
    state->closePath();
    state = state->restore();

    doContent();
  }
}

void XWTikzShape::starShape(bool dopath)
{
  int totalstarpoints = state->starPoints * 2;
  double anglestep = 180 / state->starPoints;
  double x = 0.5 * boxWidth + state->innerXSep;
  double y = 0.5 * (boxHeight + boxDepth) + state->innerYSep;
  if (x < y)
    x = y;
  x *= 1.41421;
  double innerradius = x;
  if (state->useStarRatio)
    x *= state->starPointRatio;
  else
    x += state->starPointHeight;
  double xc = x;
  double xa = state->minWidth;
  double ya = state->minHeight;
  if (ya > xa)
    xa = ya;
  if (x < 0.5 * xa)
    x = 0.5 * xa;
  double outerradius = x;
  if (x > xc)
  {
    if (state->useStarRatio)
    {
      xa = x / state->starPointRatio;
      innerradius = xa;
    }
    else
    {
      xa = x - state->starPointHeight;
      innerradius = xa;
    }
  }
  outerradius = x;
  xa = state->outerXSep;
  ya = state->outerYSep;
  if (ya > xa)
    xa = ya;
  double angletofirstpoint = 180 / state->starPoints;
  double angletosecondpoint = angletofirstpoint * 2;
  QPointF p1(outerradius * cos(angletofirstpoint),outerradius * sin(angletofirstpoint));
  QPointF p2(innerradius * cos(angletosecondpoint),innerradius * sin(angletosecondpoint));
  QPointF p3(innerradius * cos((double)0),innerradius * sin((double)0));
  double mathresult = state->angleBetweenLines(p1,p2,p1,p3);
  mathresult /= 2;
  x = outerradius + xa / sin(mathresult);
  double anchorouterradius = x;
  QPointF p4(innerradius * cos(angletofirstpoint),innerradius * sin(angletofirstpoint));
  QPointF p5(outerradius * cos((double)0),outerradius * sin((double)0));
  QPointF p6(outerradius * cos(angletosecondpoint),outerradius * sin(angletosecondpoint));
  mathresult = state->angleBetweenLines(p4,p5,p4,p6);
  mathresult /= 2;
  x = innerradius + xa / sin(mathresult);
  double anchorinnerradius = x;
  double startangle = 90 + state->shapeBorderRotate;

#define XW_TIKZ_SHAPE_STAR_ANCHOR_BORDER(ex,ey) \
  QPointF ep(centerPoint.x() + ex, centerPoint.y() + ey); \
  mathresult = state->angleBetweenPoints(centerPoint, ep); \
  mathresult -= startangle; \
  if (mathresult < 0) \
    mathresult += 360; \
  mathresult /= anglestep; \
  int counta = floor(mathresult); \
  mathresult *= anglestep; \
  mathresult += startangle; \
  double firstangle = mathresult; \
  mathresult += anglestep; \
  double secondangle = mathresult; \
  if ((counta % 2) == 1) \
  { \
    double ax = centerPoint.x() + anchorinnerradius * cos(firstangle); \
    double ay = centerPoint.y() + anchorinnerradius * sin(firstangle); \
    QPointF p1(ax,ay); \
    ax = centerPoint.x() + anchorouterradius * cos(secondangle); \
    ay = centerPoint.y() + anchorouterradius * sin(secondangle); \
    QPointF p2(ax,ay); \
    anchorPos = state->intersectionOfLines(centerPoint,ep,p1,p2); \
  } \
  else \
  { \
    double ax = centerPoint.x() + anchorouterradius * cos(firstangle); \
    double ay = centerPoint.y() + anchorouterradius * sin(firstangle); \
    QPointF p1(ax,ay); \
    ax = centerPoint.x() + anchorinnerradius * cos(secondangle); \
    ay = centerPoint.y() + anchorinnerradius * sin(secondangle); \
    QPointF p2(ax,ay); \
    anchorPos = state->intersectionOfLines(centerPoint,ep,p1,p2); \
  }
  
  switch (state->anchor)
  {
    default:
      {
        double delta = state->angle;
        double externalx = cos(delta);
        double externaly = sin(delta);
        XW_TIKZ_SHAPE_STAR_ANCHOR_BORDER(externalx,externaly)
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
      {
        XW_TIKZ_SHAPE_STAR_ANCHOR_BORDER(0,anchorouterradius)
      }
      break;

    case PGFsouth:
      {
        XW_TIKZ_SHAPE_STAR_ANCHOR_BORDER(0,-anchorouterradius)
      }
      break;

    case PGFeast:
      {
        XW_TIKZ_SHAPE_STAR_ANCHOR_BORDER(anchorouterradius,0)
      }
      break;

    case PGFwest:
      {
        XW_TIKZ_SHAPE_STAR_ANCHOR_BORDER(-anchorouterradius,0)
      }
      break;

    case PGFnortheast:
      {
        XW_TIKZ_SHAPE_STAR_ANCHOR_BORDER(anchorouterradius,anchorouterradius)
      }
      break;

    case PGFnorthwest:
      {
        XW_TIKZ_SHAPE_STAR_ANCHOR_BORDER(-anchorouterradius,anchorouterradius)
      }
      break;

    case PGFsoutheast:
      {
        XW_TIKZ_SHAPE_STAR_ANCHOR_BORDER(anchorouterradius,-anchorouterradius)
      }
      break;

    case PGFsouthwest:
      {
        XW_TIKZ_SHAPE_STAR_ANCHOR_BORDER(-anchorouterradius,-anchorouterradius)
      }
      break;
  }

  if (dopath)
  {
    double angle = startangle;
    x = centerPoint.x() + outerradius * cos(startangle);
    y = centerPoint.y() + outerradius * sin(startangle);
    state = state->save(false);
    state->moveTo(x,y);
    for (int i = 1; i <= totalstarpoints; i++)
    {
      angle += anglestep;
      if (i == totalstarpoints)
        state->closePath();
      else
      {
        if ((i % 2) == 1)
          state->lineTo(centerPoint.x() + innerradius * cos(angle),centerPoint.y() + innerradius * sin(angle));
        else
          state->lineTo(centerPoint.x() + outerradius * cos(angle),centerPoint.y() + outerradius * sin(angle));
      }
    }

    state = state->restore();

    doContent();
  }
}

void XWTikzShape::trapeziumShape(bool dopath)
{
  double x = 0.5 * boxWidth + state->innerXSep;
  double y = 0.5 * (boxHeight + boxDepth) + state->innerYSep;
  double rotate = state->shapeBorderRotate;
  if (state->shapeBorderUsesIncircle)
  {
    if (x < y)
      x = y;
    x *= 1.41421;
    y = x;
  }
  else
  {
    int counta = (int)(state->shapeBorderRotate) % 360;
    counta += 45;
    counta /= 90;
    counta *= 90;
    if (counta < 0)
      counta += 360;
    rotate = counta;
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
  }

  double mathresult = (int)(state->trapeziumLeftAngle) % 360;
  if (mathresult < 0)
    mathresult += 360;
  double leftangle = mathresult;
  if (leftangle == 0)
    mathresult = 0;
  else
    mathresult = 1 / tan(leftangle);
  double xa = 2 * y * mathresult;

  mathresult = (int)(state->trapeziumRightAngle) % 360;
  if (mathresult < 0)
    mathresult += 360;
  double rightangle = mathresult;
  if (rightangle == 0)
    mathresult = 0;
  else
    mathresult = 1 / tan(rightangle);
  double xb = 2 * y * mathresult;
  double ya = state->minHeight;
  if (y < 0.5 * ya)
  {
    if (state->trapeziumStretches)
      y = 0.5 * ya;
    else
    {
      if (state->trapeziumStretchesBody)
        y = 0.5 * ya;
      else
      {
        mathresult = 1 / y;
        double yb = 0.5 * ya * mathresult;
        y = 0.5 * ya;
        x = yb * x;
        xa = yb * xa;
        xb = yb * xb;
      }
    }
  }
  ya = state->minWidth;
  double xc = 2 * x;
  if (xa < 0)
    xc -= xa;
  else
    xc += xa;
  if (xb < 0)
    xc -= xb;
  else
    xc += xb;
  if (xc < ya)
  {
    if (state->trapeziumStretchesBody)
    {
      xc = -xc;
      xc += ya;
      x += 0.5 * xc;
    }
    else
    {
      ya = ya / xc;
      x *= ya;
      xa *= ya;
      xb *= ya;
      if (!state->trapeziumStretches)
        y *= ya;
    }
  }

  double halfheight = y;
  double halfwidth = x;
  double leftextension = xa;
  double rightextension = xb;
  xc = 2 * x;
  x = state->outerXSep;
  y = state->outerYSep;
  if (y > x)
    x = y;
  double outersep = x;
  xc += 2.0 * x;
  double yc = halfheight * 2 + x * 2;
  double externalradius = yc;
  if (xc >= yc)
    externalradius = xc;

  x = centerPoint.x() - halfwidth;
  if (leftextension > 0)
    x -= leftextension;
  y = centerPoint.y() - halfheight;
  QPointF lowerleftpoint(x,y);

  x = centerPoint.x() - halfwidth;
  if (leftextension > 0)
    x += leftextension;
  y = centerPoint.y() + halfheight;
  QPointF upperleftpoint(x,y);

  x = centerPoint.x() + halfwidth;
  if (rightextension > 0)
    x -= rightextension;
  y = centerPoint.y() + halfheight;
  QPointF upperrightpoint(x,y);

  x = centerPoint.x() + halfwidth;
  if (rightextension > 0)
    x += rightextension;
  y = centerPoint.y() - halfheight;
  QPointF lowerrightpoint(x,y);

  mathresult = state->angleBetweenLines(lowerleftpoint,lowerrightpoint,lowerleftpoint,upperleftpoint);
  mathresult *= 0.5;
  mathresult = tan(mathresult);
  mathresult = 1 / mathresult;
  x = -outersep * mathresult;
  y = -outersep;
  QPointF lowerleftmiter(x,y);

  mathresult = state->angleBetweenLines(upperleftpoint,lowerleftpoint,upperleftpoint,upperrightpoint);
  mathresult *= 0.5;
  mathresult = tan(mathresult);
  mathresult = 1 / mathresult;
  x = -outersep * mathresult;
  y = outersep;
  QPointF upperleftmiter(x,y);

  mathresult = state->angleBetweenLines(upperrightpoint,upperleftpoint,upperrightpoint,lowerrightpoint);
  mathresult *= 0.5;
  mathresult = tan(mathresult);
  mathresult = 1 / mathresult;
  x = outersep * mathresult;
  y = outersep;
  QPointF upperrightmiter(x,y);

  mathresult = state->angleBetweenLines(lowerrightpoint,upperrightpoint,lowerrightpoint,lowerleftpoint);
  mathresult *= 0.5;
  mathresult = tan(mathresult);
  mathresult = 1 / mathresult;
  x = outersep * mathresult;
  y = -outersep;
  QPointF lowerrightmiter(x,y);

  QPointF lowerleftborderpoint = lowerleftpoint + lowerleftmiter;
  QPointF upperleftborderpoint = upperleftpoint + upperleftmiter;
  QPointF upperrightborderpoint = upperrightpoint + upperrightmiter;
  QPointF lowerrightborderpoint = lowerrightpoint + lowerrightmiter;

  double angletolowerleft = state->angleBetweenPoints(centerPoint,lowerleftborderpoint);
  double angletoupperleft = state->angleBetweenPoints(centerPoint,upperleftborderpoint);
  double angletoupperright = state->angleBetweenPoints(centerPoint,upperrightborderpoint);
  double angletolowerright = state->angleBetweenPoints(centerPoint,lowerrightborderpoint);

  QPointF rotatedbasepoint = state->rotatePointAround(basePoint,centerPoint,-rotate);
  double baseangletolowerleft = state->angleBetweenPoints(rotatedbasepoint,lowerleftborderpoint);
  double baseangletoupperleft = state->angleBetweenPoints(rotatedbasepoint,upperleftborderpoint);
  double baseangletoupperright = state->angleBetweenPoints(rotatedbasepoint,upperrightborderpoint);
  double baseangletolowerright = state->angleBetweenPoints(rotatedbasepoint,lowerrightborderpoint);

  QPointF rotatedmidpoint = state->rotatePointAround(midPoint,centerPoint,-rotate);
  double midangletolowerleft = state->angleBetweenPoints(rotatedmidpoint,lowerleftborderpoint);
  double midangletoupperleft = state->angleBetweenPoints(rotatedmidpoint,upperleftborderpoint);
  double midangletoupperright = state->angleBetweenPoints(rotatedmidpoint,upperrightborderpoint);
  double midangletolowerright = state->angleBetweenPoints(rotatedmidpoint,lowerrightborderpoint);

  lowerleftborderpoint = state->rotatePointAround(lowerleftborderpoint,centerPoint,rotate);
  upperleftborderpoint = state->rotatePointAround(upperleftborderpoint,centerPoint,rotate);
  upperrightborderpoint = state->rotatePointAround(upperrightborderpoint,centerPoint,rotate);
  lowerrightborderpoint = state->rotatePointAround(lowerrightborderpoint,centerPoint,rotate);

  lowerleftpoint = state->rotatePointAround(lowerleftpoint,centerPoint,rotate);
  upperleftpoint = state->rotatePointAround(upperleftpoint,centerPoint,rotate);
  upperrightpoint = state->rotatePointAround(upperrightpoint,centerPoint,rotate);
  lowerrightpoint = state->rotatePointAround(lowerrightpoint,centerPoint,rotate);

#define XW_TIKZ_SHAPE_TRAPEZIUM_ANCHOR_BORDER(ex,ey) \
  QPointF ep(referencePoint.x() + ex,referencePoint.y() + ey); \
  mathresult = state->angleBetweenPoints(referencePoint,ep); \
  mathresult -= rotate; \
  if (mathresult < 0) \
    mathresult += 360; \
  double externalangle = mathresult; \
  if (referencePoint == basePoint) \
  { \
    angletoupperright = baseangletoupperright; \
    angletoupperleft = baseangletoupperleft; \
    angletolowerright = baseangletolowerright; \
    angletolowerleft = baseangletolowerleft; \
  } \
  else \
  { \
    if (referencePoint == midPoint) \
    {\
      angletoupperright = midangletoupperright; \
      angletoupperleft = midangletoupperleft; \
      angletolowerright = midangletolowerright; \
      angletolowerleft = midangletolowerleft; \
    }\
  } \
  QPointF firstpoint,secondpoint;\
  if (externalangle < angletoupperright) \
  { \
    firstpoint = upperrightborderpoint; \
    secondpoint = lowerrightborderpoint; \
  } \
  else \
  { \
    if (externalangle < angletoupperleft) \
    { \
      firstpoint = upperleftborderpoint; \
      secondpoint = upperrightborderpoint; \
    }\
    else \
    { \
      if (externalangle < angletolowerleft) \
      { \
        firstpoint = upperleftborderpoint; \
        secondpoint = lowerleftborderpoint; \
      }\
      else \
      {\
        if (externalangle < angletolowerright) \
        {\
          firstpoint = lowerleftborderpoint; \
          secondpoint = lowerrightborderpoint; \
        } \
        else \
        { \
          firstpoint = upperrightborderpoint; \
          secondpoint = lowerrightborderpoint; \
        } \
      }\
    } \
  } \
  anchorPos = state->intersectionOfLines(referencePoint,ep,firstpoint,secondpoint);

  switch (state->anchor)
  {
    default:
      {
        double delta = state->angle;
        double externalx = cos(delta);
        double externaly = sin(delta);
        XW_TIKZ_SHAPE_TRAPEZIUM_ANCHOR_BORDER(externalx,externaly)
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

    case PGFbaseeast:
      {
        referencePoint = basePoint;
        XW_TIKZ_SHAPE_TRAPEZIUM_ANCHOR_BORDER(externalradius,0)
      }
      break;

    case PGFbasewest:
      {
        referencePoint = basePoint;
        XW_TIKZ_SHAPE_TRAPEZIUM_ANCHOR_BORDER(-externalradius,0)
      }
      break;

    case PGFmideast:
      {
        referencePoint = midPoint;
        XW_TIKZ_SHAPE_TRAPEZIUM_ANCHOR_BORDER(externalradius,0)
      }
      break;

    case PGFmidwest:
      {
        referencePoint = midPoint;
        XW_TIKZ_SHAPE_TRAPEZIUM_ANCHOR_BORDER(-externalradius,0)
      }
      break;

    case PGFnorth:
      {
        XW_TIKZ_SHAPE_TRAPEZIUM_ANCHOR_BORDER(0,externalradius)
      }
      break;

    case PGFsouth:
      {
        XW_TIKZ_SHAPE_TRAPEZIUM_ANCHOR_BORDER(0,-externalradius)
      }
      break;

    case PGFeast:
      {
        XW_TIKZ_SHAPE_TRAPEZIUM_ANCHOR_BORDER(externalradius,0)
      }
      break;

    case PGFwest:
      {
        XW_TIKZ_SHAPE_TRAPEZIUM_ANCHOR_BORDER(-externalradius,0)
      }
      break;

    case PGFnortheast:
      {
        XW_TIKZ_SHAPE_TRAPEZIUM_ANCHOR_BORDER(externalradius,externalradius)
      }
      break;

    case PGFsouthwest:
      {
        XW_TIKZ_SHAPE_TRAPEZIUM_ANCHOR_BORDER(-externalradius,-externalradius)
      }
      break;

    case PGFsoutheast:
      {
        XW_TIKZ_SHAPE_TRAPEZIUM_ANCHOR_BORDER(externalradius,-externalradius)
      }
      break;

    case PGFnorthwest:
      {
        XW_TIKZ_SHAPE_TRAPEZIUM_ANCHOR_BORDER(-externalradius,externalradius)
      }
      break;

    case PGFbottomleftcorner:
      anchorPos = lowerleftborderpoint;
      break;

    case PGFtopleftcorner:
      anchorPos = upperleftborderpoint;
      break;

    case PGFtoprightcorner:
      anchorPos = upperrightborderpoint;
      break;

    case PGFbottomrightcorner:
      anchorPos = lowerrightborderpoint;
      break;

    case PGFleftside:
      anchorPos = state->lineAtTime(0.5,lowerleftborderpoint,upperleftborderpoint);
      break;

    case PGFrightside:
      anchorPos = state->lineAtTime(0.5,lowerrightborderpoint,upperrightborderpoint);
      break;

    case PGFtopside:
      anchorPos = state->lineAtTime(0.5,upperleftborderpoint,upperrightborderpoint);
      break;

    case PGFbottomside:
      anchorPos = state->lineAtTime(0.5,lowerleftborderpoint,lowerrightborderpoint);
      break;
  }

  if (dopath)
  {
    state = state->save(false);
    state->moveTo(lowerleftpoint);
    state->lineTo(upperleftpoint);
    state->lineTo(upperrightpoint);
    state->lineTo(lowerrightpoint);
    state->closePath();
    state = state->restore();

    doContent();
  }
}
