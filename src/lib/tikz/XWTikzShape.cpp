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

#define XW_TIKZ_SHAPE_CALLOUT_SHORTERNPOINT \
  mathresult = state->angleBetweenPoints(calloutpointer,centerPoint); \
  double nangle = mathresult; \
  double sinpointerangle = sin(nangle); \
  double cospointerangle = cos(nangle); \
  x = calloutpointer.x() + sinpointerangle * state->calloutPointerShorten;\
  y = calloutpointer.y() + cospointerangle * state->calloutPointerShorten; \
  calloutpointer.setX(x); \
  calloutpointer.setY(y);

XWTikzShape::XWTikzShape(XWPDFDriver*driverA,
                         XWTeXBox * boxA,
                         XWTikzState * stateA,
                         int nt)
:driver(driverA),
 box(boxA),
 state(stateA),
 nodeType(nt)
{}

XWTikzShape::~XWTikzShape()
{}

double XWTikzShape::angleOnEllipse(const QPointF & p, const QPointF & ur)
{
  double xa = ur.x();
  double ya = ur.y();
  double x = p.x();
  double y = p.y();
  double tempdima = 1;
  double mathresult = 0;
  if (x != 0)
  {
    tempdima = x;
    mathresult = xa / tempdima;
    tempdima = mathresult;
  }

  if (y == 0)
    tempdima = 1;
  else
  {
    mathresult = y / ya;
    tempdima = mathresult * tempdima;
    mathresult = atan(tempdima);
  }

  tempdima = mathresult;
  if (tempdima < 0)
    tempdima += 360;

  if (x < 0)
  {
    if (y == 0)
      tempdima = 180;
    else
    {
      if (y < 0)
        tempdima += 180;
      else
        tempdima -= 180;
    }
  }
  else
  {
    if (x == 0)
    {
      if (y < 0)
        tempdima = 270;
      else
        tempdima = 90;
    }
    else
    {
      if (y == 0)
        tempdima = 0;
    }
  }

  return tempdima;
}

void XWTikzShape::doShape(bool dopath)
{
  if (box)
  {
    boxWidth = box->getWidth(state->textWidth);
    boxHeight = box->getHeight(state->textWidth);
    boxDepth = box->getDepth(state->textWidth);
  }
  centerPoint.setX(0.5 * boxWidth);
  centerPoint.setY(0.5 * boxHeight - 0.5 * boxDepth);
  basePoint.setX(centerPoint.x());
  basePoint.setY(0);
  midPoint.setX(centerPoint.x());
  midPoint.setY(5);
  referencePoint = centerPoint;

  switch (state->shape)
  {
    default:
      rectangleShape(dopath);
      break;

    case PGFcoordinate:
      coordinateShape(dopath);
      break;

    case PGFrectangle:
      rectangleShape(dopath);
      break;

    case PGFcircle:
      circleShape(dopath);
      break;

    case PGFellipse:
      ellipseShape(dopath);
      break;

    case PGFdiamond:
      diamondShape(dopath);
      break;

    case PGFstar:
      starShape(dopath);
      break;

    case PGFregularpolygon:
      regularPolygonShape(dopath);
      break;

    case PGFtrapezium:
      trapeziumShape(dopath);
      break;

    case PGFsemicircle:
      semicircleShape(dopath);
      break;

    case PGFisoscelestriangle:
      isosceleTriangleShape(dopath);
      break;

    case PGFkite:
      kiteShape(dopath);
      break;

    case PGFdart:
      dartShape(dopath);
      break;

    case PGFcircularsector:
      circularSectorShape(dopath);
      break;

    case PGFcylinder:
      cylinderShape(dopath);
      break;

    case PGFforbiddensign:
      forbiddenSignShape(dopath);
      break;

    case PGFcorrectforbiddensign:
      correctForbiddenSignShape(dopath);
      break;

    case PGFstarburst:
      starburstShape(dopath);
      break;

    case PGFcloud:
      cloudShape(dopath);
      break;

    case PGFsignal:
      signalShape(dopath);
      break;

    case PGFtape:
      tapeShape(dopath);
      break;

    case PGFmagnifyingglass:
      magnifyingGlassShape(dopath);
      break;

    case PGFmagnetictape:
      magneticTapeShape(dopath);
      break;

    case PGFellipsecallout:
      ellipseCalloutShape(dopath);
      break;

    case PGFrectanglecallout:
      ellipseCalloutShape(dopath);
      break;

    case PGFcloudcallout:
      cloudCalloutShape(dopath);
      break;

    case PGFsinglearrow:
      singleArrowShape(dopath);
      break;

    case PGFdoublearrow:
      doubleArrowShape(dopath);
      break;

    case PGFarrowbox:
      arrowBoxShape(dopath);
      break;

    case PGFcrossout:
      crossoutShape(dopath);
      break;

    case PGFstrikeout:
      strikeoutShape(dopath);
      break;

    case PGFroundedrectangle:
      roundedRectangleShape(dopath);
      break;

    case PGFchamferedrectangle:
      chamferedRectangleShape(dopath);
      break;

    case PGFcirclesplit:
      circleSplitShape(dopath);
      break;

    case PGFcirclesolidus:
      circleSolidusShape(dopath);
      break;

    case PGFrectanglesplit:
      rectangleSplitShape(dopath);
      break;

    case PGFellipsesplit:
      ellipseSplitShape(dopath);
      break;
  }

  centerPos = state->map(centerPos);
  anchorPos = state->map(anchorPos);
  westPos = state->map(westPos);
  eastPos = state->map(eastPos);
  northPos = state->map(northPos);
  southPos = state->map(southPos);

  if (dopath && box )
  {
    box->setXY(state->align, centerPoint.x() - 0.5 * boxWidth, centerPoint.y() + 0.5 * boxHeight);
    QTransform transA = state->transform;
    if (box->isTextBox())
      driver->setTextMatrix(transA.m11(),transA.m12(),transA.m21(),transA.m22(),transA.dx(),transA.dy());
    else
      driver->setMatrix(transA.m11(),transA.m12(),transA.m21(),transA.m22(),transA.dx(),transA.dy());
    box->doContent(driver);    

    state->flush();

    switch (state->shape)
    {
      default:
        box->setXY(state->align,centerPoint.x() - 0.5 * boxWidth,centerPoint.y() + 0.5 * boxHeight);
        break;

      case PGFcirclesplit:
      case PGFellipsesplit:      
        {
          double x = centerPoint.x() - 0.5 * boxWidth;
          double y = centerPoint.y() + boxHeight + state->lineWidth + state->innerYSep;
          box->setXY(state->align, x, y, 0);
          y = centerPoint.y() + state->lineWidth + state->innerYSep;
          box->setXY(state->align, x, y, 1);
        }
        break;

      case PGFcirclesolidus:
        {
          box->setXY(state->align, 0, 0, 0);
          box->setXY(state->align, lowerPoint.x(), lowerPoint.y(), 1);
        }
        break;

      case PGFrectanglesplit:
        break;
    }

    state->flush();
  }
}

QPointF XWTikzShape::pointBorderEllipse(const QPointF & pd, const QPointF & ur)
{
  double xa = ur.x();
  double ya = ur.y();
  double x = xa;
  double y = ya;
  if (xa == ya)
  {
    QVector2D v(pd.x(),pd.y());
    v.normalize();
    x = v.x() * xa;
    y = v.y() * ya;
  }
  else
  {
    if (xa < ya)
    {
      int countb = qRound(ya);//qRound(ya / 65536);
      x = x / countb;
      y = y / countb;
      double xc = x;
      double yc = 1;//8192;
      //y = 0.125 * y;
      countb = qRound(y);
      yc = yc / countb;
      x = pd.x();
      y = pd.y();
      y = yc * y;
      y = xc * y;
      QVector2D v(x,y);
      v.normalize();
      x = v.x() * xa;
      y = v.y() * ya;
    }
    else
    {
      int countb = qRound(xa);//qRound(xa / 65536);
      x = x / countb;
      y = y / countb;
      double yc = y;
      double xc = 1;//8192;
      //x = 0.125 * x;
      countb = qRound(x);
      xc = xc / countb;
      x = pd.x();
      y = pd.y();
      y = yc * y;
      y = xc * y;
      QVector2D v(x,y);
      v.normalize();
      x = v.x() * xa;
      y = v.y() * ya;
    }
  }

  QPointF ret(x,y);
  return ret;
}

QPointF XWTikzShape::pointBorderRectangle(const QPointF & pd, const QPointF & ur)
{
  double xb = ur.x();
  double yb = ur.y();
  double xa = pd.x();
  double ya = pd.y();

  double x = xa;
  double y = ya;
  if (x < 0)
    x = -x;

  if (y < 0)
    y = -y;

  double xc = x;//0.125 * x;
  double yc = y;//0.125 * y;
  int counta = qRound(xc);
  int countb = qRound(yc);
  if (countb < counta)
  {
    if (counta < 255)
    {
      y = yb;
      x = 0;
    }
    else
    {
      xc = 1 / counta;//8192 / counta;
      y = xc * y;
      y = xb * y;
      if (y < yb)
        x = xb;
      else
      {
        if (countb < 255)
        {
          x = xb;
          y = 0;
        }
        else
        {
          yc = 1 / countb;//8192 / countb;
          x = yc * x;
          x = yb * x;
          y = yb;
        }
      }
    }
  }
  else
  {
    if (countb < 255)
    {
      x = xb;
      y = 0;
    }
    else
    {
      yc = 1 / countb;//8192 / countb;
      x = yc * x;
      x = yb * x;
      if (x < xb)
        y = yb;
      else
      {
        if (counta < 255)
        {
          y = yb;
          x = 0;
        }
        else
        {
          xc = 1 / counta;//8192 / counta;
          y = xc * y;
          y = xb * y;
          x = xb;
        }
      }
    }
  }

  if (xa < 0)
    x = -x;

  if (ya < 0)
    y = -y;

  QPointF ret(x,y);
  return ret;
}

void XWTikzShape::setFontSize(int s)
{
  if (box)
    box->setFontSize(s);
}

void XWTikzShape::arrowBoxShape(bool dopath)
{
  double shaftwidth = state->arrowBoxShaftWidth;
  double x = state->innerXSep + 0.5 * boxWidth;
  double xa = state->minWidth;
  if (x < 0.5 * xa)
    x = 0.5 * xa;
  x += state->outerXSep;

  double y = state->innerYSep + 0.5 * (boxHeight + boxDepth);
  double ya = state->minHeight;
  if (y < 0.5 * ya)
    y = 0.5 * ya;
  y += state->outerYSep;
  double halfboxwidth = x;
  double halfboxheight = y;
  QPointF arrowboxcorner(x,y);

  x = state->arrowBoxWestArrow.y();
  int stype = (int)(state->arrowBoxWestArrow.x());
  if (stype == XW_TIKZ_FROM_BORDER && x > 0)
    x += halfboxwidth;
  double westextend = x;
  
  x = state->arrowBoxEastArrow.y();
  stype = (int)(state->arrowBoxEastArrow.x());
  if (stype == XW_TIKZ_FROM_BORDER && x > 0)
    x += halfboxwidth;
  double eastextend = x;

  x = state->arrowBoxNorthArrow.y();
  stype = (int)(state->arrowBoxNorthArrow.x());
  if (stype == XW_TIKZ_FROM_BORDER && x > 0)
    x += halfboxheight;
  double northextend = x;

  x = state->arrowBoxSouthArrow.y();
  stype = (int)(state->arrowBoxSouthArrow.x());
  if (stype == XW_TIKZ_FROM_BORDER && x > 0)
    x += halfboxheight;
  double southextend = x;

//  double arrowheadextend = state->arrowBoxHeadExtend;
  double arrowheadindent = state->arrowBoxHeadIndent;
  double mathresult = state->arrowBoxTipAngle / 2;
  double arrowtipmiterangle = mathresult;
  mathresult = 1 / sin(arrowtipmiterangle);
//  double cosecarrowtipmiterangle = mathresult;
  x = state->arrowBoxHeadExtend;
  y = state->arrowBoxHeadIndent;
  if (y == 0)
    mathresult = 90;
  else
  {
    if (y < 0)
    {
      mathresult = x / y;
      mathresult = atan(mathresult) + 180;
    }
    else
    {
      mathresult = x / y;
      mathresult = atan(mathresult);
    }
  }

  mathresult /= 2;
  double beforearrowheadmiterangle = mathresult;
  mathresult = 1 / sin(beforearrowheadmiterangle);
  double cosecbeforearrowheadmiterangle = mathresult;
  double tempdima = 90 - arrowtipmiterangle;
  double tempdimb = 2 * beforearrowheadmiterangle - 90;
  tempdima += tempdimb;
  tempdima /= 2;
  double beforearrowtipmiterangle = tempdima;
  mathresult = 1 / sin(beforearrowtipmiterangle);
  double cosecbeforearrowtipmiterangle = mathresult;
  double outerxsep = state->outerXSep;
  double outerysep = state->outerYSep;
  mathresult = state->arrowBoxTipAngle / 2;
  mathresult = 1 / tan(mathresult);
  double cothalfangle = mathresult;
  y = 0.5 * state->arrowBoxShaftWidth;
  y += state->arrowBoxHeadExtend;
  x = cothalfangle * y;
  QPointF beforearrowtip(x,y);

#define XW_TIKZ_SHAPE_ARROWBOX_ANCHOR_BORDER(ex,ey) \
  QPointF ep(referencePoint.x() + ex,referencePoint.y() + ey); \
  mathresult = state->angleBetweenPoints(centerPoint,ep); \
  double externalangle = mathresult; \
  QPointF p; \
  if (westextend > 0)\
  {\
    p.setX(centerPoint.x() - westextend - outerxsep / sin(arrowtipmiterangle));\
    p.setY(centerPoint.y());\
  }\
  else\
  {\
    p.setX(centerPoint.x() - arrowboxcorner.x());\
    p.setY(centerPoint.y());\
  }\
  mathresult = state->angleBetweenPoints(referencePoint, p);\
  QPointF firstpoint,secondpoint;\
  if (externalangle < mathresult)\
  {\
    if (northextend > 0)\
    {\
      p.setX(centerPoint.x());\
      p.setY(centerPoint.y() + northextend + outerysep / sin(arrowtipmiterangle));\
    }\
    else\
    {\
      p.setX(centerPoint.x());\
      p.setY(centerPoint.y() + arrowboxcorner.y());\
    }\
    mathresult = state->angleBetweenPoints(referencePoint, p);\
    if (externalangle < mathresult)\
    {\
      if (northextend > 0)\
      {\
        p.setX(centerPoint.x());\
        p.setY(centerPoint.y() + northextend + outerysep / sin(arrowtipmiterangle));\
      }\
      else\
      {\
        p.setX(centerPoint.x());\
        p.setY(centerPoint.y() + arrowboxcorner.y());\
      }\
      mathresult = state->angleBetweenPoints(referencePoint, p);\
      if (externalangle < mathresult)\
      {\
        p = centerPoint + arrowboxcorner;\
        mathresult = state->angleBetweenPoints(referencePoint, p);\
        if (externalangle < mathresult)\
        {\
          if (eastextend > 0)\
          {\
            tempdima = 180 - arrowtipmiterangle - beforearrowtipmiterangle;\
            tempdimb = cosecbeforearrowtipmiterangle * outerysep;\
            x = centerPoint.x() + tempdimb * cos(tempdima) - beforearrowtip.x() + eastextend;\
            y = centerPoint.y() + tempdimb * sin(tempdima) + beforearrowtip.y();\
            p.setX(x);\
            p.setY(y);\
          }\
          else\
          {\
            p.setX(centerPoint.x() + arrowboxcorner.x());\
            p.setY(centerPoint.y());\
          }\
          mathresult = state->angleBetweenPoints(referencePoint, p);\
          if (externalangle < mathresult)\
          {\
            if (eastextend > 0)\
            {\
              firstpoint.setX(centerPoint.x() + eastextend + outerxsep / sin(arrowtipmiterangle));\
              firstpoint.setY(centerPoint.y());\
            }\
            else\
            {\
              firstpoint.setX(centerPoint.x() + arrowboxcorner.x());\
              firstpoint.setY(centerPoint.y());\
            }\
            if (eastextend > 0)\
            {\
              tempdima = 180 - arrowtipmiterangle - beforearrowtipmiterangle;\
              tempdimb = cosecbeforearrowtipmiterangle * outerysep;\
              x = centerPoint.x() + tempdimb * cos(tempdima) - beforearrowtip.x() + eastextend;\
              y = centerPoint.y() + tempdimb * sin(tempdima) + beforearrowtip.y();\
              secondpoint.setX(x);\
              secondpoint.setY(y);\
            }\
            else\
            {\
              secondpoint.setX(centerPoint.x() + arrowboxcorner.x());\
              secondpoint.setY(centerPoint.y());\
            }\
          }\
          else\
          {\
            if (eastextend > 0)\
            {\
              p.setX(centerPoint.x() + arrowboxcorner.x());\
              p.setY(centerPoint.y() + 0.5 * shaftwidth + outerysep);\
            }\
            else\
            {\
              p.setX(centerPoint.x() + arrowboxcorner.x());\
              p.setY(centerPoint.y());\
            }\
            mathresult = state->angleBetweenPoints(referencePoint, p);\
            if (externalangle < mathresult)\
            {\
              if (eastextend > 0)\
              {\
                tempdima = 180 - beforearrowheadmiterangle;\
                tempdimb = cosecbeforearrowheadmiterangle * outerxsep;\
                x = centerPoint.x() + tempdimb * cos(tempdima) + eastextend + arrowheadindent - beforearrowtip.x();\
                y = centerPoint.y() + tempdimb * sin(tempdima) + 0.5 * shaftwidth;\
                firstpoint.setX(x);\
                firstpoint.setY(y);\
              }\
              else\
              {\
                firstpoint.setX(centerPoint.x() + arrowboxcorner.x());\
                firstpoint.setY(centerPoint.y());\
              }\
              if (eastextend > 0)\
              {\
                secondpoint.setX(centerPoint.x() + arrowboxcorner.x());\
                secondpoint.setY(centerPoint.y() + 0.5 * shaftwidth + outerysep);\
              }\
              else\
              {\
                secondpoint.setX(centerPoint.x() + arrowboxcorner.x());\
                secondpoint.setY(centerPoint.y());\
              }\
            }\
            else\
            {\
              if (eastextend > 0)\
              {\
                firstpoint.setX(centerPoint.x() + arrowboxcorner.x());\
                firstpoint.setY(centerPoint.y() + 0.5 * shaftwidth + outerysep);\
              }\
              else\
              {\
                firstpoint.setX(centerPoint.x() + arrowboxcorner.x());\
                firstpoint.setY(centerPoint.y());\
              }\
              secondpoint = centerPoint + arrowboxcorner;\
            }\
          }\
        }\
        else\
        {\
          if (northextend > 0)\
          {\
            tempdima = arrowtipmiterangle + beforearrowtipmiterangle - 90;\
            tempdimb = outerxsep * cosecbeforearrowtipmiterangle;\
            x = centerPoint.x() + tempdimb * cos(tempdima) + beforearrowtip.y();\
            y = centerPoint.y() + tempdimb * sin(tempdima) - beforearrowtip.x() + northextend;\
            p.setX(x);\
            p.setY(y);\
          }\
          else\
          {\
            p.setX(centerPoint.x());\
            p.setY(centerPoint.y() + arrowboxcorner.y());\
          }\
          mathresult = state->angleBetweenPoints(referencePoint, p);\
          if (externalangle < mathresult)\
          {\
            if (northextend > 0)\
            {\
              p.setX(centerPoint.x() + 0.5 * shaftwidth + outerxsep);\
              p.setY(centerPoint.y() + arrowboxcorner.y());\
            }\
            else\
            {\
              p.setX(centerPoint.x());\
              p.setY(centerPoint.y() + arrowboxcorner.y());\
            }\
            mathresult = state->angleBetweenPoints(referencePoint, p);\
            if (externalangle < mathresult)\
            {\
              firstpoint = centerPoint + arrowboxcorner;\
              if (northextend > 0)\
              {\
                secondpoint.setX(centerPoint.x() + 0.5 * shaftwidth + outerxsep);\
                secondpoint.setY(centerPoint.y() + arrowboxcorner.y());\
              }\
              else\
              {\
                secondpoint.setX(centerPoint.x());\
                secondpoint.setY(centerPoint.y() + arrowboxcorner.y());\
              }\
            }\
            else\
            {\
              if (northextend > 0)\
              {\
                firstpoint.setX(centerPoint.x() + 0.5 * shaftwidth + outerxsep);\
                firstpoint.setY(centerPoint.y() + arrowboxcorner.y());\
              }\
              else\
              {\
                firstpoint.setX(centerPoint.x());\
                firstpoint.setY(centerPoint.y() + arrowboxcorner.y());\
              }\
              if (northextend > 0)\
              {\
                tempdima = 270 - beforearrowheadmiterangle;\
                tempdimb = outerxsep * cosecbeforearrowheadmiterangle;\
                x = centerPoint.x() - tempdimb * cos(tempdima) + 0.5 * shaftwidth;\
                y = centerPoint.y() + tempdimb * sin(tempdima) - beforearrowtip.x() + northextend + arrowheadindent;\
                secondpoint.setX(x);\
                secondpoint.setY(y);\
              }\
              else\
              {\
                secondpoint.setX(centerPoint.x());\
                secondpoint.setY(centerPoint.y() + arrowboxcorner.y());\
              }\
            }\
          }\
          else\
          {\
            p.setX(centerPoint.x() - arrowboxcorner.x());\
            p.setY(centerPoint.y() + arrowboxcorner.y());\
            mathresult = state->angleBetweenPoints(referencePoint, p);\
            if (externalangle < mathresult)\
            {\
              if (northextend > 0)\
              {\
                tempdima = arrowtipmiterangle + beforearrowtipmiterangle - 90;\
                tempdimb = outerxsep * cosecbeforearrowtipmiterangle;\
                x = centerPoint.x() - tempdimb * cos(tempdima) - beforearrowtip.y();\
                y = centerPoint.y() + tempdimb * sin(tempdima) - beforearrowtip.x() + northextend;\
                p.setX(x);\
                p.setY(y);\
              }\
              else\
              {\
                p.setX(centerPoint.x());\
                p.setY(centerPoint.y() + arrowboxcorner.y());\
              }\
              mathresult = state->angleBetweenPoints(referencePoint, p);\
              if (externalangle < mathresult)\
              {\
                if (northextend > 0)\
                {\
                  firstpoint.setX(centerPoint.x());\
                  firstpoint.setY(centerPoint.y() + northextend + outerysep / sin(arrowtipmiterangle));\
                }\
                else\
                {\
                  firstpoint.setX(centerPoint.x());\
                  firstpoint.setY(centerPoint.y() + arrowboxcorner.y());\
                }\
                if (northextend > 0)\
                {\
                  tempdima = arrowtipmiterangle + beforearrowtipmiterangle - 90;\
                  tempdimb = outerxsep * cosecbeforearrowtipmiterangle;\
                  x = centerPoint.x() - tempdimb * cos(tempdima) - beforearrowtip.y();\
                  y = centerPoint.y() + tempdimb * sin(tempdima) - beforearrowtip.x() + northextend;\
                  secondpoint.setX(x);\
                  secondpoint.setY(y);\
                }\
                else\
                {\
                  secondpoint.setX(centerPoint.x());\
                  secondpoint.setY(centerPoint.y() + arrowboxcorner.y());\
                }\
              }\
              else\
              {\
                if (northextend > 0)\
                {\
                  p.setX(centerPoint.x() - 0.5 * shaftwidth);\
                  p.setY(centerPoint.y() + arrowboxcorner.y());\
                }\
                else\
                {\
                  p.setX(centerPoint.x());\
                  p.setY(centerPoint.y() + arrowboxcorner.y());\
                }\
                mathresult = state->angleBetweenPoints(referencePoint, p);\
                if (externalangle < mathresult)\
                {\
                  if (northextend > 0)\
                  {\
                    tempdima = 270 - beforearrowheadmiterangle;\
                    tempdimb = outerysep * cosecbeforearrowheadmiterangle;\
                    x = centerPoint.x() + tempdimb * cos(tempdima) - 0.5 * shaftwidth;\
                    y = centerPoint.y() + tempdimb * sin(tempdima) - beforearrowtip.x() + northextend + arrowheadindent;\
                    firstpoint.setX(x);\
                    firstpoint.setY(y);\
                  }\
                  else\
                  {\
                    firstpoint.setX(centerPoint.x());\
                    firstpoint.setY(centerPoint.y() + arrowboxcorner.y());\
                  }\
                  if (northextend > 0)\
                  {\
                    secondpoint.setX(centerPoint.x() - 0.5 * shaftwidth);\
                    secondpoint.setY(centerPoint.y() + arrowboxcorner.y());\
                  }\
                  else\
                  {\
                    secondpoint.setX(centerPoint.x());\
                    secondpoint.setY(centerPoint.y() + arrowboxcorner.y());\
                  }\
                }\
                else\
                {\
                  if (northextend > 0)\
                  {\
                    firstpoint.setX(centerPoint.x() - 0.5 * shaftwidth);\
                    firstpoint.setY(centerPoint.y() + arrowboxcorner.y());\
                  }\
                  else\
                  {\
                    firstpoint.setX(centerPoint.x());\
                    firstpoint.setY(centerPoint.y() + arrowboxcorner.y());\
                  }\
                  secondpoint.setX(centerPoint.x() - arrowboxcorner.x());\
                  secondpoint.setY(centerPoint.y() + arrowboxcorner.y());\
                }\
              }\
            }\
            else\
            {\
              if (westextend > 0)\
              {\
                tempdima = 180 - arrowtipmiterangle - beforearrowtipmiterangle;\
                tempdimb = outerysep * cosecbeforearrowtipmiterangle;\
                x = centerPoint.x() - tempdimb * cos(tempdima) + beforearrowtip.x() - westextend;\
                y = centerPoint.y() + tempdimb * sin(tempdima) + beforearrowtip.y();\
                p.setX(x);\
                p.setY(y);\
              }\
              else\
              {\
                p.setX(centerPoint.x() - arrowboxcorner.x());\
                p.setY(centerPoint.y());\
              }\
              mathresult = state->angleBetweenPoints(referencePoint, p);\
              if (externalangle < mathresult)\
              {\
                if (westextend > 0)\
                {\
                  p.setX(centerPoint.x() - arrowboxcorner.x());\
                  p.setY(centerPoint.y() + 0.5 * shaftwidth + outerysep);\
                }\
                else\
                {\
                  p.setX(centerPoint.x() - arrowboxcorner.x());\
                  p.setY(centerPoint.y());\
                }\
                mathresult = state->angleBetweenPoints(referencePoint, p);\
                if (externalangle < mathresult)\
                {\
                  firstpoint.setX(centerPoint.x() - arrowboxcorner.x());\
                  firstpoint.setY(centerPoint.y() + arrowboxcorner.y());\
                  if (westextend > 0)\
                  {\
                    secondpoint.setX(centerPoint.x() - arrowboxcorner.x());\
                    secondpoint.setY(centerPoint.y() + 0.5 * shaftwidth + outerysep);\
                  }\
                  else\
                  {\
                    secondpoint.setX(centerPoint.x() - arrowboxcorner.x());\
                    secondpoint.setY(centerPoint.y());\
                  }\
                }\
                else\
                {\
                  if (westextend > 0)\
                  {\
                    firstpoint.setX(centerPoint.x() - arrowboxcorner.x());\
                    firstpoint.setY(centerPoint.y() + 0.5 * shaftwidth + outerysep);\
                  }\
                  else\
                  {\
                    firstpoint.setX(centerPoint.x() - arrowboxcorner.x());\
                    firstpoint.setY(centerPoint.y());\
                  }\
                  if (westextend > 0)\
                  {\
                    tempdima = 180 - beforearrowheadmiterangle;\
                    tempdimb = outerxsep * cosecbeforearrowheadmiterangle;\
                    x = centerPoint.x() - tempdimb * cos(tempdima) + beforearrowtip.x() - westextend - arrowheadindent;\
                    y = centerPoint.y() + tempdimb * sin(tempdima) + 0.5 * shaftwidth;\
                    secondpoint.setX(x);\
                    secondpoint.setY(y);\
                  }\
                  else\
                  {\
                    secondpoint.setX(centerPoint.x() - arrowboxcorner.x());\
                    secondpoint.setY(centerPoint.y());\
                  }\
                }\
              }\
              else\
              {\
                if (westextend > 0)\
                {\
                  tempdima = 180 - arrowtipmiterangle - beforearrowtipmiterangle;\
                  tempdimb = outerysep * cosecbeforearrowtipmiterangle;\
                  x = centerPoint.x() - tempdimb * cos(tempdima) + beforearrowtip.x() - westextend;\
                  y = centerPoint.y() + tempdimb * sin(tempdima) + beforearrowtip.y();\
                  firstpoint.setX(x);\
                  firstpoint.setY(y);\
                }\
                else\
                {\
                  firstpoint.setX(centerPoint.x() - arrowboxcorner.x());\
                  firstpoint.setY(centerPoint.y());\
                }\
                if (westextend > 0)\
                {\
                  secondpoint.setX(centerPoint.x() - westextend - outerxsep / sin(arrowtipmiterangle));\
                  secondpoint.setY(centerPoint.y());\
                }\
                else\
                {\
                  secondpoint.setX(centerPoint.x() - arrowboxcorner.x());\
                  secondpoint.setY(centerPoint.y());\
                }\
              }\
            }\
          }\
        }\
      }\
      else\
      {\
        if (southextend > 0)\
        {\
          p.setX(centerPoint.x());\
          p.setY(centerPoint.y() - southextend - outerysep / sin(arrowtipmiterangle));\
        }\
        else\
        {\
          p.setX(centerPoint.x() + arrowboxcorner.x());\
          p.setY(centerPoint.y());\
        }\
        mathresult = state->angleBetweenPoints(referencePoint, p);\
        if (externalangle < mathresult)\
        {\
          p.setX(centerPoint.x() - arrowboxcorner.x());\
          p.setY(centerPoint.y() - arrowboxcorner.y());\
          mathresult = state->angleBetweenPoints(referencePoint, p);\
          if (externalangle < mathresult)\
          {\
            if (westextend > 0)\
            {\
              tempdima = 180 - arrowtipmiterangle - beforearrowtipmiterangle;\
              tempdimb = outerysep * cosecbeforearrowtipmiterangle;\
              x = centerPoint.x() - tempdimb * cos(tempdima) + beforearrowtip.x() - westextend;\
              y = centerPoint.y() - tempdimb * sin(tempdima) - beforearrowtip.y();\
              p.setX(x);\
              p.setY(y);\
            }\
            else\
            {\
              p.setX(centerPoint.x() - arrowboxcorner.x());\
              p.setY(centerPoint.y());\
            }\
            mathresult = state->angleBetweenPoints(referencePoint, p);\
            if (externalangle < mathresult)\
            {\
              if (westextend > 0)\
              {\
                firstpoint.setX(centerPoint.x() - westextend - outerxsep / sin(arrowtipmiterangle));\
                firstpoint.setY(centerPoint.y());\
              }\
              else\
              {\
                firstpoint.setX(centerPoint.x() - arrowboxcorner.x());\
                firstpoint.setY(centerPoint.y());\
              }\
              if (westextend > 0)\
              {\
                tempdima = 180 - arrowtipmiterangle - beforearrowtipmiterangle;\
                tempdimb = outerysep * cosecbeforearrowtipmiterangle;\
                x = centerPoint.x() - tempdimb * cos(tempdima) + beforearrowtip.x() - westextend;\
                y = centerPoint.y() - tempdimb * sin(tempdima) - beforearrowtip.y();\
                secondpoint.setX(x);\
                secondpoint.setY(y);\
              }\
              else\
              {\
                secondpoint.setX(centerPoint.x() - arrowboxcorner.x());\
                secondpoint.setY(centerPoint.y());\
              }\
            }\
            else \
            {\
              if (westextend > 0)\
              {\
                p.setX(centerPoint.x() - arrowboxcorner.x());\
                p.setY(centerPoint.y() - 0.5 * shaftwidth - outerysep);\
              }\
              else\
              {\
                p.setX(centerPoint.x() - arrowboxcorner.x());\
                p.setY(centerPoint.y());\
              }\
              mathresult = state->angleBetweenPoints(referencePoint, p);\
              if (externalangle < mathresult)\
              {\
                if (westextend > 0)\
                {\
                  tempdima = 180 - beforearrowheadmiterangle;\
                  tempdimb = cosecbeforearrowheadmiterangle * outerxsep;\
                  x = centerPoint.x() - tempdimb * cos(tempdima) + beforearrowtip.x() - westextend - arrowheadindent;\
                  y = centerPoint.y() - tempdimb * sin(tempdima) - 0.5 * shaftwidth;\
                  firstpoint.setX(x);\
                  firstpoint.setY(y);\
                }\
                else\
                {\
                  firstpoint.setX(centerPoint.x() - arrowboxcorner.x());\
                  firstpoint.setY(centerPoint.y());\
                }\
                if (westextend > 0)\
                {\
                  secondpoint.setX(centerPoint.x() - arrowboxcorner.x());\
                  secondpoint.setY(centerPoint.y() - 0.5 * shaftwidth - outerysep);\
                }\
                else\
                {\
                  secondpoint.setX(centerPoint.x() - arrowboxcorner.x());\
                  secondpoint.setY(centerPoint.y());\
                }\
              }\
              else\
              {\
                if (westextend > 0)\
                {\
                  firstpoint.setX(centerPoint.x() - arrowboxcorner.x());\
                  firstpoint.setY(centerPoint.y() - 0.5 * shaftwidth - outerysep);\
                }\
                else\
                {\
                  firstpoint.setX(centerPoint.x() - arrowboxcorner.x());\
                  firstpoint.setY(centerPoint.y());\
                }\
                secondpoint.setX(centerPoint.x() - arrowboxcorner.x());\
                secondpoint.setY(centerPoint.y() - arrowboxcorner.y());\
              }\
            }\
          }\
          else\
          {\
            if (northextend > 0)\
            {\
              tempdima = arrowtipmiterangle + beforearrowtipmiterangle - 90;\
              tempdimb = outerxsep * cosecbeforearrowtipmiterangle;\
              x = centerPoint.x() - tempdimb * cos(tempdima) - beforearrowtip.y();\
              y = centerPoint.y() - tempdimb * sin(tempdima) + beforearrowtip.x() - northextend;\
              p.setX(x);\
              p.setY(y);\
            }\
            else\
            {\
              p.setX(centerPoint.x());\
              p.setY(centerPoint.y() - arrowboxcorner.y());\
            }\
            mathresult = state->angleBetweenPoints(referencePoint, p);\
            if (externalangle < mathresult)\
            {\
              if (southextend > 0)\
              {\
                p.setX(centerPoint.x() - 0.5 * shaftwidth - outerxsep);\
                p.setY(centerPoint.y() - arrowboxcorner.y());\
              }\
              else\
              {\
                p.setX(centerPoint.x());\
                p.setY(centerPoint.y() - arrowboxcorner.y());\
              }\
              mathresult = state->angleBetweenPoints(referencePoint, p);\
              if (externalangle < mathresult)\
              {\
                firstpoint.setX(centerPoint.x() - arrowboxcorner.x());\
                firstpoint.setY(centerPoint.y() - arrowboxcorner.y());\
                if (southextend > 0)\
                {\
                  secondpoint.setX(centerPoint.x() - 0.5 * shaftwidth - outerxsep);\
                  secondpoint.setY(centerPoint.y() - arrowboxcorner.y());\
                }\
                else\
                {\
                  secondpoint.setX(centerPoint.x());\
                  secondpoint.setY(centerPoint.y() - arrowboxcorner.y());\
                }\
              }\
              else\
              {\
                if (southextend > 0)\
                {\
                  firstpoint.setX(centerPoint.x() - 0.5 * shaftwidth - outerxsep);\
                  firstpoint.setY(centerPoint.y() - arrowboxcorner.y());\
                }\
                else\
                {\
                  firstpoint.setX(centerPoint.x());\
                  firstpoint.setY(centerPoint.y() - arrowboxcorner.y());\
                }\
                if (northextend > 0)\
                {\
                  tempdima = 270 - beforearrowheadmiterangle;\
                  tempdimb = outerysep * cosecbeforearrowheadmiterangle;\
                  x = centerPoint.x() + tempdimb * cos(tempdima) - 0.5 * shaftwidth;\
                  y = centerPoint.y() - tempdimb * sin(tempdima) + beforearrowtip.x() - northextend - arrowheadindent;\
                  secondpoint.setX(x);\
                  secondpoint.setY(y);\
                }\
                else\
                {\
                  secondpoint.setX(centerPoint.x());\
                  secondpoint.setY(centerPoint.y() - arrowboxcorner.y());\
                }\
              }\
            }\
            else\
            {\
              if (northextend > 0)\
              {\
                tempdima = arrowtipmiterangle + beforearrowtipmiterangle - 90;\
                tempdimb = outerxsep * cosecbeforearrowtipmiterangle;\
                x = centerPoint.x() - tempdimb * cos(tempdima) - beforearrowtip.y();\
                y = centerPoint.y() - tempdimb * sin(tempdima) + beforearrowtip.x() - northextend;\
                firstpoint.setX(x);\
                firstpoint.setY(y);\
              }\
              else\
              {\
                firstpoint.setX(centerPoint.x());\
                firstpoint.setY(centerPoint.y() - arrowboxcorner.y());\
              }\
              if (southextend > 0)\
              {\
                secondpoint.setX(centerPoint.x());\
                secondpoint.setY(centerPoint.y() - southextend - outerysep / sin(arrowtipmiterangle));\
              }\
              else\
              {\
                secondpoint.setX(centerPoint.x() + arrowboxcorner.x());\
                secondpoint.setY(centerPoint.y());\
              }\
            }\
          }\
        }\
        else\
        {\
          p.setX(centerPoint.x() + arrowboxcorner.x());\
          p.setY(centerPoint.y() - arrowboxcorner.y());\
          mathresult = state->angleBetweenPoints(referencePoint, p);\
          if (externalangle < mathresult)\
          {\
            if (northextend > 0)\
            {\
              tempdima = arrowtipmiterangle + beforearrowtipmiterangle - 90;\
              tempdimb = outerxsep * cosecbeforearrowtipmiterangle;\
              x = centerPoint.x() + tempdimb * cos(tempdima) + beforearrowtip.y();\
              y = centerPoint.y() - tempdimb * sin(tempdima) + beforearrowtip.x() - northextend;\
              p.setX(x);\
              p.setY(y);\
            }\
            else\
            {\
              p.setX(centerPoint.x());\
              p.setY(centerPoint.y() - arrowboxcorner.y());\
            }\
            mathresult = state->angleBetweenPoints(referencePoint, p);\
            if (externalangle < mathresult)\
            {\
              if (southextend > 0)\
              {\
                firstpoint.setX(centerPoint.x());\
                firstpoint.setY(centerPoint.y() - southextend - outerysep / sin(arrowtipmiterangle));\
              }\
              else\
              {\
                firstpoint.setX(centerPoint.x() + arrowboxcorner.x());\
                firstpoint.setY(centerPoint.y());\
              }\
              if (northextend > 0)\
              {\
                tempdima = arrowtipmiterangle + beforearrowtipmiterangle - 90;\
                tempdimb = outerxsep * cosecbeforearrowtipmiterangle;\
                x = centerPoint.x() + tempdimb * cos(tempdima) + beforearrowtip.y();\
                y = centerPoint.y() - tempdimb * sin(tempdima) + beforearrowtip.x() - northextend;\
                p.setX(x);\
                p.setY(y);\
              }\
              else\
              {\
                p.setX(centerPoint.x());\
                p.setY(centerPoint.y() - arrowboxcorner.y());\
              }\
            }\
            else \
            {\
              if (southextend > 0)\
              {\
                p.setX(centerPoint.x() + 0.5 * shaftwidth + outerxsep);\
                p.setY(centerPoint.y() - arrowboxcorner.y());\
              }\
              else\
              {\
                p.setX(centerPoint.x());\
                p.setY(centerPoint.y() - arrowboxcorner.y());\
              }\
              mathresult = state->angleBetweenPoints(referencePoint, p);\
              if (externalangle < mathresult)\
              {\
                if (northextend > 0)\
                {\
                  tempdima = 270 - beforearrowheadmiterangle;\
                  tempdimb = outerysep * cosecbeforearrowheadmiterangle;\
                  x = centerPoint.x() - tempdimb * cos(tempdima) + 0.5 * shaftwidth;\
                  y = centerPoint.y() - tempdimb * sin(tempdima) + beforearrowtip.x() - northextend - arrowheadindent;\
                  firstpoint.setX(x);\
                  firstpoint.setY(y);\
                }\
                else\
                {\
                  firstpoint.setX(centerPoint.x());\
                  firstpoint.setY(centerPoint.y() - arrowboxcorner.y());\
                }\
                if (southextend > 0)\
                {\
                  secondpoint.setX(centerPoint.x() + 0.5 * shaftwidth + outerxsep);\
                  secondpoint.setY(centerPoint.y() - arrowboxcorner.y());\
                }\
                else\
                {\
                  secondpoint.setX(centerPoint.x());\
                  secondpoint.setY(centerPoint.y() - arrowboxcorner.y());\
                }\
              }\
              else\
              {\
                if (southextend > 0)\
                {\
                  firstpoint.setX(centerPoint.x() + 0.5 * shaftwidth + outerxsep);\
                  firstpoint.setY(centerPoint.y() - arrowboxcorner.y());\
                }\
                else\
                {\
                  firstpoint.setX(centerPoint.x());\
                  firstpoint.setY(centerPoint.y() - arrowboxcorner.y());\
                }\
                secondpoint.setX(centerPoint.x() + arrowboxcorner.x());\
                secondpoint.setY(centerPoint.y() - arrowboxcorner.y());\
              }\
            }\
          }\
          else\
          {\
            if (eastextend > 0)\
            {\
              tempdima = 180 - arrowtipmiterangle - beforearrowtipmiterangle;\
              tempdimb = outerysep * cosecbeforearrowtipmiterangle;\
              x = centerPoint.x() + tempdimb * cos(tempdima) - beforearrowtip.x() + eastextend;\
              y = centerPoint.y() - tempdimb * sin(tempdima) - beforearrowtip.y();\
              p.setX(x);\
              p.setY(y);\
            }\
            else\
            {\
              p.setX(centerPoint.x() + arrowboxcorner.x());\
              p.setY(centerPoint.y());\
            }\
            mathresult = state->angleBetweenPoints(referencePoint, p);\
            if (externalangle < mathresult)\
            {\
              if (eastextend > 0)\
              {\
                p.setX(centerPoint.x() + arrowboxcorner.x());\
                p.setY(centerPoint.y() - 0.5 * shaftwidth - outerysep);\
              }\
              else\
              {\
                p.setX(centerPoint.x() + arrowboxcorner.x());\
                p.setY(centerPoint.y());\
              }\
              mathresult = state->angleBetweenPoints(referencePoint, p);\
              if (externalangle < mathresult)\
              {\
                firstpoint.setX(centerPoint.x() + arrowboxcorner.x());\
                firstpoint.setY(centerPoint.y() - arrowboxcorner.y());\
                if (eastextend > 0)\
                {\
                  secondpoint.setX(centerPoint.x() + arrowboxcorner.x());\
                  secondpoint.setY(centerPoint.y() - 0.5 * shaftwidth - outerysep);\
                }\
                else\
                {\
                  secondpoint.setX(centerPoint.x() + arrowboxcorner.x());\
                  secondpoint.setY(centerPoint.y());\
                }\
              }\
              else\
              {\
                if (eastextend > 0)\
                {\
                  firstpoint.setX(centerPoint.x() + arrowboxcorner.x());\
                  firstpoint.setY(centerPoint.y() - 0.5 * shaftwidth - outerysep);\
                  if (eastextend > 0)\
                  {\
                    secondpoint.setX(centerPoint.x() + arrowboxcorner.x());\
                    secondpoint.setY(centerPoint.y() - 0.5 * shaftwidth - outerysep);\
                  }\
                  else\
                  {\
                    secondpoint.setX(centerPoint.x() + arrowboxcorner.x());\
                    secondpoint.setY(centerPoint.y());\
                  }\
                }\
                else\
                {\
                  if (eastextend > 0)\
                  {\
                    firstpoint.setX(centerPoint.x() + arrowboxcorner.x());\
                    firstpoint.setY(centerPoint.y() - 0.5 * shaftwidth - outerysep);\
                  }\
                  else\
                  {\
                    firstpoint.setX(centerPoint.x() + arrowboxcorner.x());\
                    firstpoint.setY(centerPoint.y());\
                  }\
                  if (eastextend > 0)\
                  {\
                    tempdima = 180 - beforearrowheadmiterangle;\
                    tempdimb = cosecbeforearrowheadmiterangle * outerxsep;\
                    x = centerPoint.x() + tempdimb * cos(tempdima) - beforearrowtip.x() + eastextend + arrowheadindent;\
                    y = centerPoint.y() - tempdimb * sin(tempdima) - 0.5 * shaftwidth;\
                    secondpoint.setX(x);\
                    secondpoint.setY(y);\
                  }\
                  else\
                  {\
                    secondpoint.setX(centerPoint.x() + arrowboxcorner.x());\
                    secondpoint.setY(centerPoint.y());\
                  }\
                }\
              }\
            }\
            else\
            {\
              if (eastextend > 0)\
              {\
                tempdima = 180 - arrowtipmiterangle - beforearrowtipmiterangle;\
                tempdimb = outerysep * cosecbeforearrowtipmiterangle;\
                x = centerPoint.x() + tempdimb * cos(tempdima) - beforearrowtip.x() + eastextend;\
                y = centerPoint.y() - tempdimb * sin(tempdima) - beforearrowtip.y();\
                firstpoint.setX(x);\
                firstpoint.setY(y);\
              }\
              else\
              {\
                firstpoint.setX(centerPoint.x() + arrowboxcorner.x());\
                firstpoint.setY(centerPoint.y());\
              }\
              if (eastextend > 0)\
              {\
                secondpoint.setX(centerPoint.x() + eastextend + outerxsep / sin(arrowtipmiterangle));\
                secondpoint.setY(centerPoint.y());\
              }\
              else\
              {\
                secondpoint.setX(centerPoint.x() + arrowboxcorner.x());\
                secondpoint.setY(centerPoint.y());\
              }\
            }\
          }\
        }\
      }\
    }\
  }\
  anchorPos = state->intersectionOfLines(referencePoint,ep, firstpoint,secondpoint);

  switch (state->anchor)
  {
    default:
      {
        double delta = state->angle;
        double externalx = cos(delta);
        double externaly = sin(delta);
        XW_TIKZ_SHAPE_ARROWBOX_ANCHOR_BORDER(externalx,externaly)
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
      if (eastextend > 0)
      {
        referencePoint = midPoint;
        XW_TIKZ_SHAPE_ARROWBOX_ANCHOR_BORDER(eastextend,0)
      }
      else
      {
        anchorPos.setX(midPoint.x() + arrowboxcorner.x());
        anchorPos.setY(midPoint.y());
      }
      break;

    case PGFmidwest:
      if (westextend > 0)
      {
        referencePoint = midPoint;
        XW_TIKZ_SHAPE_ARROWBOX_ANCHOR_BORDER(-westextend,0)
      }
      else
      {
        anchorPos.setX(midPoint.x() - arrowboxcorner.x());
        anchorPos.setY(midPoint.y());
      }
      break;

    case PGFbase:
      anchorPos = basePoint;
      break;

    case PGFbaseeast:
      if (eastextend > 0)
      {
        referencePoint = basePoint;
        XW_TIKZ_SHAPE_ARROWBOX_ANCHOR_BORDER(eastextend,0)
      }
      else
      {
        anchorPos.setX(basePoint.x() + arrowboxcorner.x());
        anchorPos.setY(basePoint.y());
      }
      break;

    case PGFbasewest:
      if (westextend > 0)
      {
        referencePoint = basePoint;
        XW_TIKZ_SHAPE_ARROWBOX_ANCHOR_BORDER(-westextend,0)
      }
      else
      {
        anchorPos.setX(basePoint.x() - arrowboxcorner.x());
        anchorPos.setY(basePoint.y());
      }
      break;

    case PGFnorth:
      if (northextend > 0)
        goto northarrowtipanchor;
      else
      {
        anchorPos.setX(centerPoint.x());
        anchorPos.setY(centerPoint.y() + arrowboxcorner.y());
      }
      break;

    case PGFsouth:
      if (southextend > 0)
        goto southarrowtipanchor;
      else
      {
        anchorPos.setX(centerPoint.x());
        anchorPos.setY(centerPoint.y() - arrowboxcorner.y());
      }
      break;

    case PGFeast:
      if (eastextend > 0)
        goto eastarrowtipanchor;
      else
      {
        anchorPos.setX(centerPoint.x() + arrowboxcorner.x());
        anchorPos.setY(centerPoint.y());
      }
      break;

    case PGFwest:
      if (westextend > 0)
        goto westarrowtipanchor;
      else
      {
        anchorPos.setX(centerPoint.x() - arrowboxcorner.x());
        anchorPos.setY(centerPoint.y());
      }
      break;

    case PGFnortheast:
      anchorPos = centerPoint + arrowboxcorner;
      break;

    case PGFnorthwest:
      anchorPos.setX(centerPoint.x() - arrowboxcorner.x());
      anchorPos.setY(centerPoint.y() + arrowboxcorner.y());
      break;

    case PGFsouthwest:
      anchorPos.setX(centerPoint.x() - arrowboxcorner.x());
      anchorPos.setY(centerPoint.y() - arrowboxcorner.y());
      break;

    case PGFsoutheast:
      anchorPos.setX(centerPoint.x() + arrowboxcorner.x());
      anchorPos.setY(centerPoint.y() - arrowboxcorner.y());
      break;

    case PGFbeforeeastarrow:
      if (eastextend > 0)
      {
        anchorPos.setX(centerPoint.x() + arrowboxcorner.x());
        anchorPos.setY(centerPoint.y() + 0.5 * shaftwidth + outerysep);
      }
      else
      {
        anchorPos.setX(centerPoint.x() + arrowboxcorner.x());
        anchorPos.setY(centerPoint.y());
      }
      break;

    case PGFbeforeeastarrowhead:
      if (eastextend > 0)
      {
        tempdima = 180 - beforearrowheadmiterangle;
        tempdimb = cosecbeforearrowheadmiterangle * outerxsep;
        x = centerPoint.x() + tempdimb * cos(tempdima) + eastextend + arrowheadindent - beforearrowtip.x();
        y = centerPoint.y() + tempdimb * sin(tempdima) + 0.5 * shaftwidth;
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      else
      {
        anchorPos.setX(centerPoint.x() + arrowboxcorner.x());
        anchorPos.setY(centerPoint.y());
      }
      break;

    case PGFbeforeeastarrowtip:
      if (eastextend > 0)
      {
        tempdima = 180 - arrowtipmiterangle - beforearrowtipmiterangle;
        tempdimb = cosecbeforearrowtipmiterangle * outerysep;
        x = centerPoint.x() + tempdimb * cos(tempdima) - beforearrowtip.x() + eastextend;
        y = centerPoint.y() + tempdimb * sin(tempdima) + beforearrowtip.y();
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      else
      {
        anchorPos.setX(centerPoint.x() + arrowboxcorner.x());
        anchorPos.setY(centerPoint.y());
      }
      break;

    case PGFeastarrowtip:
eastarrowtipanchor:
      if (eastextend > 0)
      {
        anchorPos.setX(centerPoint.x() + eastextend + outerxsep / sin(arrowtipmiterangle));
        anchorPos.setY(centerPoint.y());
      }
      else
      {
        anchorPos.setX(centerPoint.x() + arrowboxcorner.x());
        anchorPos.setY(centerPoint.y());
      }
      break;

    case PGFaftereastarrowtip:
      if (eastextend > 0)
      {
        tempdima = 180 - arrowtipmiterangle - beforearrowtipmiterangle;
        tempdimb = outerysep * cosecbeforearrowtipmiterangle;
        x = centerPoint.x() + tempdimb * cos(tempdima) - beforearrowtip.x() + eastextend;
        y = centerPoint.y() - tempdimb * sin(tempdima) - beforearrowtip.y();
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      else
      {
        anchorPos.setX(centerPoint.x() + arrowboxcorner.x());
        anchorPos.setY(centerPoint.y());
      }
      break;

    case PGFaftereastarrowhead:
      if (eastextend > 0)
      {
        tempdima = 180 - beforearrowheadmiterangle;
        tempdimb = cosecbeforearrowheadmiterangle * outerxsep;
        x = centerPoint.x() + tempdimb * cos(tempdima) - beforearrowtip.x() + eastextend + arrowheadindent;
        y = centerPoint.y() - tempdimb * sin(tempdima) - 0.5 * shaftwidth;
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      else
      {
        anchorPos.setX(centerPoint.x() + arrowboxcorner.x());
        anchorPos.setY(centerPoint.y());
      }
      break;

    case PGFaftereastarrow:
      if (eastextend > 0)
      {
        anchorPos.setX(centerPoint.x() + arrowboxcorner.x());
        anchorPos.setY(centerPoint.y() - 0.5 * shaftwidth - outerysep);
      }
      else
      {
        anchorPos.setX(centerPoint.x() + arrowboxcorner.x());
        anchorPos.setY(centerPoint.y());
      }
      break;

    case PGFbeforewestarrow:
      if (westextend > 0)
      {
        anchorPos.setX(centerPoint.x() - arrowboxcorner.x());
        anchorPos.setY(centerPoint.y() - 0.5 * shaftwidth - outerysep);
      }
      else
      {
        anchorPos.setX(centerPoint.x() - arrowboxcorner.x());
        anchorPos.setY(centerPoint.y());
      }
      break;

    case PGFbeforewestarrowhead:
      if (westextend > 0)
      {
        tempdima = 180 - beforearrowheadmiterangle;
        tempdimb = cosecbeforearrowheadmiterangle * outerxsep;
        x = centerPoint.x() - tempdimb * cos(tempdima) + beforearrowtip.x() - westextend - arrowheadindent;
        y = centerPoint.y() - tempdimb * sin(tempdima) - 0.5 * shaftwidth;
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      else
      {
        anchorPos.setX(centerPoint.x() - arrowboxcorner.x());
        anchorPos.setY(centerPoint.y());
      }
      break;

    case PGFbeforewestarrowtip:
      if (westextend > 0)
      {
        tempdima = 180 - arrowtipmiterangle - beforearrowtipmiterangle;
        tempdimb = outerysep * cosecbeforearrowtipmiterangle;
        x = centerPoint.x() - tempdimb * cos(tempdima) + beforearrowtip.x() - westextend;
        y = centerPoint.y() - tempdimb * sin(tempdima) - beforearrowtip.y();
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      else
      {
        anchorPos.setX(centerPoint.x() - arrowboxcorner.x());
        anchorPos.setY(centerPoint.y());
      }
      break;

    case PGFwestarrowtip:
westarrowtipanchor:
      if (westextend > 0)
      {
        anchorPos.setX(centerPoint.x() - westextend - outerxsep / sin(arrowtipmiterangle));
        anchorPos.setY(centerPoint.y());
      }
      else
      {
        anchorPos.setX(centerPoint.x() - arrowboxcorner.x());
        anchorPos.setY(centerPoint.y());
      }
      break;

    case PGFafterwestarrowtip:
      if (westextend > 0)
      {
        tempdima = 180 - arrowtipmiterangle - beforearrowtipmiterangle;
        tempdimb = outerysep * cosecbeforearrowtipmiterangle;
        x = centerPoint.x() - tempdimb * cos(tempdima) + beforearrowtip.x() - westextend;
        y = centerPoint.y() + tempdimb * sin(tempdima) + beforearrowtip.y();
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      else
      {
        anchorPos.setX(centerPoint.x() - arrowboxcorner.x());
        anchorPos.setY(centerPoint.y());
      }
      break;

    case PGFafterwestarrowhead:
      if (westextend > 0)
      {
        tempdima = 180 - beforearrowheadmiterangle;
        tempdimb = outerxsep * cosecbeforearrowheadmiterangle;
        x = centerPoint.x() - tempdimb * cos(tempdima) + beforearrowtip.x() - westextend - arrowheadindent;
        y = centerPoint.y() + tempdimb * sin(tempdima) + 0.5 * shaftwidth;
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      else
      {
        anchorPos.setX(centerPoint.x() - arrowboxcorner.x());
        anchorPos.setY(centerPoint.y());
      }
      break;

    case PGFafterwestarrow:
      if (westextend > 0)
      {
        anchorPos.setX(centerPoint.x() - arrowboxcorner.x());
        anchorPos.setY(centerPoint.y() + 0.5 * shaftwidth + outerysep);
      }
      else
      {
        anchorPos.setX(centerPoint.x() - arrowboxcorner.x());
        anchorPos.setY(centerPoint.y());
      }
      break;

    case PGFbeforenortharrow:
      if (northextend > 0)
      {
        anchorPos.setX(centerPoint.x() - 0.5 * shaftwidth);
        anchorPos.setY(centerPoint.y() + arrowboxcorner.y());
      }
      else
      {
        anchorPos.setX(centerPoint.x());
        anchorPos.setY(centerPoint.y() + arrowboxcorner.y());
      }
      break;

    case PGFbeforenortharrowhead:
      if (northextend > 0)
      {
        tempdima = 270 - beforearrowheadmiterangle;
        tempdimb = outerysep * cosecbeforearrowheadmiterangle;
        x = centerPoint.x() + tempdimb * cos(tempdima) - 0.5 * shaftwidth;
        y = centerPoint.y() + tempdimb * sin(tempdima) - beforearrowtip.x() + northextend + arrowheadindent;
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      else
      {
        anchorPos.setX(centerPoint.x());
        anchorPos.setY(centerPoint.y() + arrowboxcorner.y());
      }
      break;

    case PGFbeforenortharrowtip:
      if (northextend > 0)
      {
        tempdima = arrowtipmiterangle + beforearrowtipmiterangle - 90;
        tempdimb = outerxsep * cosecbeforearrowtipmiterangle;
        x = centerPoint.x() - tempdimb * cos(tempdima) - beforearrowtip.y();
        y = centerPoint.y() + tempdimb * sin(tempdima) - beforearrowtip.x() + northextend;
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      else
      {
        anchorPos.setX(centerPoint.x());
        anchorPos.setY(centerPoint.y() + arrowboxcorner.y());
      }
      break;

    case PGFnortharrowtip:
  northarrowtipanchor:
      if (northextend > 0)
      {
        anchorPos.setX(centerPoint.x());
        anchorPos.setY(centerPoint.y() + northextend + outerysep / sin(arrowtipmiterangle));
      }
      else
      {
        anchorPos.setX(centerPoint.x());
        anchorPos.setY(centerPoint.y() + arrowboxcorner.y());
      }
      break;

    case PGFafternortharrowtip:
      if (northextend > 0)
      {
        tempdima = arrowtipmiterangle + beforearrowtipmiterangle - 90;
        tempdimb = outerxsep * cosecbeforearrowtipmiterangle;
        x = centerPoint.x() + tempdimb * cos(tempdima) + beforearrowtip.y();
        y = centerPoint.y() + tempdimb * sin(tempdima) - beforearrowtip.x() + northextend;
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      else
      {
        anchorPos.setX(centerPoint.x());
        anchorPos.setY(centerPoint.y() + arrowboxcorner.y());
      }
      break;

    case PGFafternortharrowhead:
      if (northextend > 0)
      {
        tempdima = 270 - beforearrowheadmiterangle;
        tempdimb = outerxsep * cosecbeforearrowheadmiterangle;
        x = centerPoint.x() - tempdimb * cos(tempdima) + 0.5 * shaftwidth;
        y = centerPoint.y() + tempdimb * sin(tempdima) - beforearrowtip.x() + northextend + arrowheadindent;
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      else
      {
        anchorPos.setX(centerPoint.x());
        anchorPos.setY(centerPoint.y() + arrowboxcorner.y());
      }
      break;

    case PGFafternortharrow:
      if (northextend > 0)
      {
        anchorPos.setX(centerPoint.x() + 0.5 * shaftwidth + outerxsep);
        anchorPos.setY(centerPoint.y() + arrowboxcorner.y());
      }
      else
      {
        anchorPos.setX(centerPoint.x());
        anchorPos.setY(centerPoint.y() + arrowboxcorner.y());
      }
      break;

    case PGFbeforesoutharrow:
      if (southextend > 0)
      {
        anchorPos.setX(centerPoint.x() + 0.5 * shaftwidth + outerxsep);
        anchorPos.setY(centerPoint.y() - arrowboxcorner.y());
      }
      else
      {
        anchorPos.setX(centerPoint.x());
        anchorPos.setY(centerPoint.y() - arrowboxcorner.y());
      }
      break;

    case PGFbeforesoutharrowhead:
      if (northextend > 0)
      {
        tempdima = 270 - beforearrowheadmiterangle;
        tempdimb = outerysep * cosecbeforearrowheadmiterangle;
        x = centerPoint.x() - tempdimb * cos(tempdima) + 0.5 * shaftwidth;
        y = centerPoint.y() - tempdimb * sin(tempdima) + beforearrowtip.x() - northextend - arrowheadindent;
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      else
      {
        anchorPos.setX(centerPoint.x());
        anchorPos.setY(centerPoint.y() - arrowboxcorner.y());
      }
      break;

    case PGFbeforesoutharrowtip:
      if (northextend > 0)
      {
        tempdima = arrowtipmiterangle + beforearrowtipmiterangle - 90;
        tempdimb = outerxsep * cosecbeforearrowtipmiterangle;
        x = centerPoint.x() + tempdimb * cos(tempdima) + beforearrowtip.y();
        y = centerPoint.y() - tempdimb * sin(tempdima) + beforearrowtip.x() - northextend;
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      else
      {
        anchorPos.setX(centerPoint.x());
        anchorPos.setY(centerPoint.y() - arrowboxcorner.y());
      }
      break;

    case PGFsoutharrowtip:
southarrowtipanchor:
      if (southextend > 0)
      {
        anchorPos.setX(centerPoint.x());
        anchorPos.setY(centerPoint.y() - southextend - outerysep / sin(arrowtipmiterangle));
      }
      else
      {
        anchorPos.setX(centerPoint.x() + arrowboxcorner.x());
        anchorPos.setY(centerPoint.y());
      }
      break;

    case PGFaftersoutharrowtip:
      if (northextend > 0)
      {
        tempdima = arrowtipmiterangle + beforearrowtipmiterangle - 90;
        tempdimb = outerxsep * cosecbeforearrowtipmiterangle;
        x = centerPoint.x() - tempdimb * cos(tempdima) - beforearrowtip.y();
        y = centerPoint.y() - tempdimb * sin(tempdima) + beforearrowtip.x() - northextend;
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      else
      {
        anchorPos.setX(centerPoint.x());
        anchorPos.setY(centerPoint.y() - arrowboxcorner.y());
      }
      break;

    case PGFaftersoutharrowhead:
      if (northextend > 0)
      {
        tempdima = 270 - beforearrowheadmiterangle;
        tempdimb = outerysep * cosecbeforearrowheadmiterangle;
        x = centerPoint.x() + tempdimb * cos(tempdima) - 0.5 * shaftwidth;
        y = centerPoint.y() - tempdimb * sin(tempdima) + beforearrowtip.x() - northextend - arrowheadindent;
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      else
      {
        anchorPos.setX(centerPoint.x());
        anchorPos.setY(centerPoint.y() - arrowboxcorner.y());
      }
      break;

    case PGFaftersoutharrow:
      if (southextend > 0)
      {
        anchorPos.setX(centerPoint.x() - 0.5 * shaftwidth - outerxsep);
        anchorPos.setY(centerPoint.y() - arrowboxcorner.y());
      }
      else
      {
        anchorPos.setX(centerPoint.x());
        anchorPos.setY(centerPoint.y() - arrowboxcorner.y());
      }
      break;
  }

  if (dopath)
  {
    x = arrowboxcorner.x() - state->outerXSep;
    y = arrowboxcorner.y() - state->outerYSep;
    arrowboxcorner.setX(x);
    arrowboxcorner.setY(y);
    state->addShift(centerPoint.x(), centerPoint.y());
    state->moveTo(arrowboxcorner);
    if (eastextend > 0)
    {
      state->lineTo(arrowboxcorner.x(), 0.5 * shaftwidth);
      state->lineTo(-beforearrowtip.x() + eastextend + arrowheadindent, 0.5 * shaftwidth);
      state->lineTo(-beforearrowtip.x() + eastextend, beforearrowtip.y());
      state->lineTo(eastextend, 0);
      state->lineTo(-beforearrowtip.x() + eastextend, -beforearrowtip.y());
      state->lineTo(-beforearrowtip.x() + eastextend + arrowheadindent, -0.5 * shaftwidth);
      state->lineTo(arrowboxcorner.x(), -0.5 * shaftwidth);
    }

    state->lineTo(arrowboxcorner.x(), -arrowboxcorner.y());

    if (southextend > 0)
    {
      state->lineTo(0.5 * shaftwidth, -arrowboxcorner.y());
      state->lineTo(0.5 * shaftwidth, beforearrowtip.x() - southextend - arrowheadindent);
      state->lineTo(beforearrowtip.y(), beforearrowtip.x() - southextend);
      state->lineTo(0, -southextend);
      state->lineTo(-beforearrowtip.y(), beforearrowtip.x() - southextend);
      state->lineTo(-0.5 * shaftwidth, beforearrowtip.x() - southextend - arrowheadindent);
      state->lineTo(-0.5 * shaftwidth, -arrowboxcorner.y());
    }

    state->lineTo(-arrowboxcorner.x(), -arrowboxcorner.y());

    if (westextend > 0)
    {
      state->lineTo(-arrowboxcorner.x(), -0.5 * shaftwidth);
      state->lineTo(beforearrowtip.x() - westextend - arrowheadindent, -0.5 * shaftwidth);
      state->lineTo(beforearrowtip.x() - westextend, -beforearrowtip.y());
      state->lineTo(-westextend, 0);
      state->lineTo(beforearrowtip.x() - westextend, beforearrowtip.y());
      state->lineTo(beforearrowtip.x() - westextend - arrowheadindent, 0.5 * shaftwidth);
      state->lineTo(-arrowboxcorner.x(), 0.5 * shaftwidth);
    }

    state->lineTo(-arrowboxcorner.x(), arrowboxcorner.y());

    if (northextend > 0)
    {
      state->lineTo(-0.5 * shaftwidth, arrowboxcorner.y());
      state->lineTo(-0.5 * shaftwidth, -beforearrowtip.x() + northextend + arrowheadindent);
      state->lineTo(-beforearrowtip.y(), -beforearrowtip.x() + northextend);
      state->lineTo(0, northextend);
      state->lineTo(beforearrowtip.y(), -beforearrowtip.x() + northextend);
      state->lineTo(0.5 * shaftwidth, -beforearrowtip.x() + northextend + arrowheadindent);
      state->lineTo(0.5 * shaftwidth, arrowboxcorner.y());
    }

    state->closePath();
  }
}

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
        double delta = state->angle;
        double externalx = cos(delta) + centerPoint.x();
        double externaly = sin(delta) + centerPoint.y();
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
  }
}

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
  circleShape(dopath);
  if (dopath)
  {
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
    state->moveTo(x,y);
    x = centerPoint.x() + 0.437 * tempdima;
    y = centerPoint.y() + 0.437 * tempdima;
    state->lineTo(x,y);
    state->setDraw(true);
  }
}

void XWTikzShape::circleShape(bool dopath)
{
  circleAnchor();
  if (dopath)
  {
    if (state->outerXSep < state->outerYSep)
      radius = radius - state->outerYSep;
    else
      radius = radius - state->outerXSep;
    state->moveTo(centerPoint);
    state->addCircle(centerPoint,radius);    
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
  circleShape(dopath);
  if (dopath)
  {
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
    state->moveTo(x,y);
    x = centerPoint.x() + tempdima;
    y = centerPoint.y();
    state->lineTo(x,y);
    state->setDraw(true);
  }
}

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
  }
}

void XWTikzShape::cloudCalloutShape(bool dopath)
{
  cloudShape(dopath);
}

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
        double delta = state->angle;
        double externalx = cos(delta) + centerPoint.x();
        double externaly = sin(delta) + centerPoint.y();
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
  }
}

void XWTikzShape::coordinateShape(bool dopath)
{
  anchorPos = centerPoint;
  westPos = centerPoint;
  eastPos = centerPoint;
  northPos = centerPoint;
  southPos = centerPoint;
  if (dopath)
    state->moveTo(anchorPos);
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
    state->moveTo(centerPoint);
    state->addCircle(centerPoint,radius);
    QPointF p1(0.707107 * radius,-0.707107 * radius);
    QPointF p2(-0.707107 * radius,0.707107 * radius);
    p1 = centerPoint + p1;
    p2 = centerPoint + p2;
    state->moveTo(p1);
    state->lineTo(p2);
  }
}

void XWTikzShape::crossoutShape(bool dopath)
{
  rectangleAnchor();
  if (dopath)
  {
    double xa = southWest.x();
    double ya = southWest.y();
    double xb = northEast.x();
    double yb = northEast.y();
    state->moveTo(xa,ya);
    state->lineTo(xb,yb);
    state->moveTo(xa,yb);
    state->lineTo(xb,ya);
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
    if (state->pathType == XW_TIKZ_PATH_BEFOREBACKGROUND)
    {
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
    }
    else
    {
      if (state->cylinderUsesCustomFill)
      {
        x = xradius - outersep;
        xradius = x;
        y = yradius - outersep;
        yradius = y;
        state->addShift(centerPoint.x(), centerPoint.y());
        state->rotate(rotate);
        state->moveTo(afterbottom);
        state->addArc(90,270,xradius,yradius);
        state->lineTo(beforetop.x(), -beforetop.y());
        state->addArc(270,90,xradius,yradius);
        state->closePath();
        state->setFillColor(state->cylinderBodyFill);
        state->flush();
        state->closePath();
        state->moveTo(beforetop);
        state->addArc(90,-270,xradius,yradius);
        state->closePath();
        state->setFillColor(state->cylinderEndFill);
      }
    }    
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
    state->moveTo(tippoint);
    state->lineTo(lefttailpoint);
    state->lineTo(tailcenterpoint);
    state->lineTo(righttailpoint);
    state->closePath();
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
    double xc = outernortheast.x() - 1.414213 * state->outerXSep;
    double yc = outernortheast.y() - 1.414213 * state->outerYSep;
    state->moveTo(xc,0);
    state->lineTo(0,yc);
    state->lineTo(-xc,0);
    state->lineTo(0,-yc);
    state->closePath();
  }
}

void XWTikzShape::doubleArrowShape(bool dopath)
{
  double x = state->outerXSep;
  double xoutersep = x;
  double y = state->outerYSep;
  double youtersep = y;

  double xa = state->innerXSep + 0.5 * (boxWidth + state->lineWidth);
  double ya = state->innerYSep + 0.5 * (boxHeight + boxDepth + state->lineWidth);

  double rotate = state->shapeBorderRotate;
  if (state->shapeBorderUsesIncircle)
  {
    if (xa < ya)
      xa = ya;
    xa *= 1.41421;
    ya = xa;
    if (xoutersep > youtersep)
      youtersep = xoutersep;
    else
      xoutersep = youtersep;
  }
  else
  {
    int counta = (int)rotate % 360;
    counta += 45;
    counta /= 90;
    counta *= 90;
    if (counta < 0)
      counta += 360;
    rotate = counta;
    if (counta == 90)
    {
      x = xa;
      xa = ya;
      ya = x;
      x = xoutersep;
      xoutersep = youtersep;
      youtersep = x;
    }
    else
    {
      if (counta == 270)
      {
        x = ya;
        xa = ya;
        ya = x;
        x = xoutersep;
        xoutersep = youtersep;
        youtersep = x;
      }
    }
  }

  double mathresult = state->doubleArrowTipAngle / 2;
  double halftipangle = mathresult;
  mathresult = 1 / sin(halftipangle);
  double cosechalftipangle = mathresult;
  mathresult = cos(halftipangle);
  double tempdima = mathresult;
  tempdima *= cosechalftipangle;
  double cothalftipangle = tempdima;
  mathresult = 1 / mathresult;
  double sechalftipangle = mathresult;
  mathresult = sin(halftipangle);
  tempdima = mathresult;
  tempdima *= sechalftipangle;
//  double tanhalftipangle = tempdima;
  double xb = ya;
  xb *= cothalftipangle;
  double xc = state->doubleArrowHeadExtend;
  double yc = xc;
  xc += ya;
  double tempdimb = state->minWidth;
  if (xc < 0.5 * tempdimb)
  {
    tempdimb = 0.5 * tempdimb;
    mathresult = tempdimb / xc;
    ya *= mathresult;
    xc *= mathresult;
    yc *= mathresult;
    xb *= mathresult;
  }
  xc -= ya;
  xc *= cothalftipangle;
  xa += xb;
  xa *= 2;
  tempdimb = state->minHeight;
  if (xa < tempdimb)
    xa = tempdimb;
  xa *= 0.5;
  xa -= xb;
  tempdima = state->doubleArrowHeadIndent;

  QPointF arrowtip(xa + xb, 0);
  QPointF beforearrowtip(xa - xc, ya + yc);
  QPointF beforearrowhead(xa - xc + tempdima, ya);

  x = centerPoint.x() + arrowtip.x() + cosechalftipangle * xoutersep;
  y = centerPoint.y() + arrowtip.y();
  QPointF arrowtipanchor(x,y);

  x += 0.5 * boxWidth;
  double externalradius = x;
  mathresult = state->angleBetweenLines(beforearrowtip,beforearrowhead,beforearrowtip,arrowtip);
  xa = xoutersep;
  tempdima = mathresult;
  tempdima *= 0.5;
  mathresult = 1 / sin(tempdima);
  xa *= mathresult;
  tempdima = -tempdima;
  tempdima += 180;
  tempdima -= halftipangle;
  x = xa * cos(tempdima) + centerPoint.x() + beforearrowtip.x();
  y = xa * sin(tempdima) + centerPoint.y() + beforearrowtip.y();
  QPointF beforearrowtipanchor(x,y);

  mathresult = state->angleBetweenPoints(beforearrowhead,beforearrowtip);
  tempdima = -mathresult;
  tempdima += 180;
  tempdima *= 0.5;
  mathresult = 1 / sin(tempdima);
  xa = xoutersep;
  xa *= mathresult;
  tempdima = -tempdima;
  tempdima += 180;
  x = xa * cos(tempdima) + centerPoint.x() + beforearrowhead.x();
  y = xa * sin(tempdima) + centerPoint.y() + beforearrowhead.y();
  QPointF beforearrowheadanchor(x,y);

  mathresult = state->angleBetweenPoints(centerPoint, beforearrowtipanchor);
  double centeranglebeforearrowtip = mathresult;

  mathresult = state->angleBetweenPoints(midPoint, beforearrowtipanchor);
  double midanglebeforearrowtip = mathresult;

  mathresult = state->angleBetweenPoints(basePoint, beforearrowtipanchor);
  double baseanglebeforearrowtip = mathresult;

#define XW_TIKZ_SHAPE_DOUBLEARROW_ANCHOR_BORDER(ex,ey)\
  QPointF ep(referencePoint.x() + ex, referencePoint.y() + ey);\
  mathresult = state->angleBetweenPoints(centerPoint,ep); \
  mathresult -= rotate; \
  if (mathresult < 0) \
    mathresult += 360; \
  double externalangle = mathresult; \
  x = externalangle;\
  if (referencePoint == midPoint)\
    xa = midanglebeforearrowtip;\
  else\
  {\
    if (referencePoint == basePoint)\
      xa = baseanglebeforearrowtip;\
    else\
      xa = centeranglebeforearrowtip;\
  }\
  QPointF firstpoint,secondpoint;\
  if (x < 180)\
  {\
    if (x < xa)\
    {\
      firstpoint = arrowtipanchor;\
      secondpoint = beforearrowtipanchor;\
    }\
    else \
    {\
      xa = 180 - xa;\
      if (x < xa)\
      {\
        firstpoint = beforearrowheadanchor;\
        x = beforearrowheadanchor.x() - centerPoint.x() + centerPoint.x();\
        y = centerPoint.y() - beforearrowheadanchor.y() + centerPoint.y();\
        secondpoint.setX(x);\
        secondpoint.setY(y);\
      }\
      else\
      {\
        x = beforearrowtipanchor.x() - centerPoint.x() + centerPoint.x();\
        y = centerPoint.y() - beforearrowtipanchor.y() + centerPoint.y();\
        firstpoint.setX(x);\
        firstpoint.setY(y);\
        x = arrowtipanchor.x() - centerPoint.x() + centerPoint.x();\
        y = arrowtipanchor.y() - centerPoint.y() + centerPoint.y();\
        secondpoint.setX(x);\
        secondpoint.setY(y);\
      }\
    }\
  }\
  else\
  {\
    xa = 360 - xa;\
    if (x < xa)\
    {\
      xa = 540 - xa;\
      if (x < xa)\
      {\
        x = arrowtipanchor.x() - centerPoint.x() + centerPoint.x();\
        y = centerPoint.y() - arrowtipanchor.y() + centerPoint.y();\
        firstpoint.setX(x);\
        firstpoint.setY(y);\
        x = beforearrowtipanchor.x() - centerPoint.x() + centerPoint.x();\
        y = beforearrowtipanchor.y() - centerPoint.y() + centerPoint.y();\
        secondpoint.setX(x);\
        secondpoint.setY(y);\
      }\
      else\
      {\
        x = beforearrowheadanchor.x() - centerPoint.x() + centerPoint.x();\
        y = beforearrowheadanchor.y() - centerPoint.y() + centerPoint.y();\
        firstpoint.setX(x);\
        firstpoint.setY(y);\
        x = centerPoint.x() - beforearrowheadanchor.x() + centerPoint.x();\
        y = beforearrowheadanchor.y() - centerPoint.y() + centerPoint.y();\
        secondpoint.setX(x);\
        secondpoint.setY(y);\
      }\
    }\
    else\
    {\
      x = beforearrowheadanchor.x() - centerPoint.x() + centerPoint.x();\
      y = centerPoint.y() - beforearrowheadanchor.y() + centerPoint.y();\
      firstpoint.setX(x);\
      firstpoint.setY(y);\
      secondpoint = arrowtipanchor;\
    }\
  }\
  firstpoint = state->rotatePointAround(firstpoint,centerPoint,rotate);\
  secondpoint = state->rotatePointAround(secondpoint,centerPoint,rotate);\
  anchorPos = state->intersectionOfLines(referencePoint,ep,firstpoint,secondpoint);

  switch (state->anchor)
  {
    default:
      {
        double delta = state->angle;
        double externalx = cos(delta);
        double externaly = sin(delta);
        XW_TIKZ_SHAPE_DOUBLEARROW_ANCHOR_BORDER(externalx,externaly)
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
      {
        referencePoint = midPoint;
        XW_TIKZ_SHAPE_DOUBLEARROW_ANCHOR_BORDER(externalradius,0)
      }
      break;

    case PGFmidwest:
      {
        referencePoint = midPoint;
        XW_TIKZ_SHAPE_DOUBLEARROW_ANCHOR_BORDER(-externalradius,0)
      }
      break;

    case PGFbase:
      anchorPos = basePoint;
      break;

    case PGFbaseeast:
      {
        referencePoint = basePoint;
        XW_TIKZ_SHAPE_DOUBLEARROW_ANCHOR_BORDER(externalradius,0)
      }
      break;

    case PGFbasewest:
      {
        referencePoint = basePoint;
        XW_TIKZ_SHAPE_DOUBLEARROW_ANCHOR_BORDER(-externalradius,0)
      }
      break;

    case PGFnorth:
      {
        XW_TIKZ_SHAPE_DOUBLEARROW_ANCHOR_BORDER(0,externalradius)
      }
      break;

    case PGFsouth:
      {
        XW_TIKZ_SHAPE_DOUBLEARROW_ANCHOR_BORDER(0,-externalradius)
      }
      break;

    case PGFeast:
      {
        XW_TIKZ_SHAPE_DOUBLEARROW_ANCHOR_BORDER(externalradius,0)
      }
      break;

    case PGFwest:
      {
        XW_TIKZ_SHAPE_DOUBLEARROW_ANCHOR_BORDER(-externalradius,0)
      }
      break;

    case PGFnortheast:
      {
        XW_TIKZ_SHAPE_DOUBLEARROW_ANCHOR_BORDER(externalradius,externalradius)
      }
      break;

    case PGFsoutheast:
      {
        XW_TIKZ_SHAPE_DOUBLEARROW_ANCHOR_BORDER(externalradius,-externalradius)
      }
      break;

    case PGFsouthwest:
      {
        XW_TIKZ_SHAPE_DOUBLEARROW_ANCHOR_BORDER(-externalradius,-externalradius)
      }
      break;

    case PGFnorthwest:
      {
        XW_TIKZ_SHAPE_DOUBLEARROW_ANCHOR_BORDER(-externalradius,externalradius)
      }
      break;

    case PGFbeforehead1:
      anchorPos = state->rotatePointAround(beforearrowheadanchor,centerPoint,rotate);
      break;

    case PGFbeforetip1:
      anchorPos = state->rotatePointAround(beforearrowtipanchor,centerPoint,rotate);
      break;

    case PGFtip1:
      anchorPos = state->rotatePointAround(arrowtipanchor,centerPoint,rotate);
      break;

    case PGFaftertip1:
      {
        x = centerPoint.x() - beforearrowtipanchor.x() + centerPoint.x();
        y = beforearrowtipanchor.y();
        QPointF p(x,y);
        anchorPos = state->rotatePointAround(p,centerPoint,rotate);
      }
      break;

    case PGFafterhead1:
      {
        x = centerPoint.x() - beforearrowheadanchor.x() + centerPoint.x();
        y = beforearrowheadanchor.y();
        QPointF p(x,y);
        anchorPos = state->rotatePointAround(p,centerPoint,rotate);
      }
      break;

    case PGFbeforehead2:
      {
        x = beforearrowheadanchor.x();
        y = beforearrowheadanchor.y();
        QPointF p(x,y);
        anchorPos = state->rotatePointAround(p,centerPoint,rotate);
      }
      break;

    case PGFbeforetip2:
      {
        x = beforearrowtipanchor.x();
        y = beforearrowtipanchor.y();
        QPointF p(x,y);
        anchorPos = state->rotatePointAround(p,centerPoint,rotate);
      }
      break;

    case PGFtip2:
      {
        x = arrowtipanchor.x();
        y = centerPoint.y() - arrowtipanchor.y() + centerPoint.y();
        QPointF p(x,y);
        anchorPos = state->rotatePointAround(p,centerPoint,rotate);
      }
      break;

    case PGFaftertip2:
      {
        x = beforearrowtipanchor.x();
        y = centerPoint.y() - beforearrowtipanchor.y() + centerPoint.y();
        QPointF p(x,y);
        anchorPos = state->rotatePointAround(p,centerPoint,rotate);
      }
      break;

    case PGFafterhead2:
      {
        x = beforearrowheadanchor.x();
        y = centerPoint.y() - beforearrowheadanchor.y() + centerPoint.y();
        QPointF p(x,y);
        anchorPos = state->rotatePointAround(p,centerPoint,rotate);
      }
      break;
  }

  if (dopath)
  {
    state->addShift(centerPoint.x(), centerPoint.y());
    state->rotate(rotate);
    state->moveTo(arrowtip);
    state->lineTo(beforearrowtip);
    state->lineTo(beforearrowhead);
    state->lineTo(-beforearrowhead.x(), beforearrowhead.y());
    state->lineTo(-beforearrowtip.x(), beforearrowtip.y());
    state->lineTo(-arrowtip.x(), arrowtip.y());
    state->lineTo(-beforearrowtip.x(), -beforearrowtip.y());
    state->lineTo(-beforearrowhead.x(), -beforearrowhead.y());
    state->lineTo(beforearrowhead.x(), -beforearrowhead.y());
    state->lineTo(beforearrowtip.x(), -beforearrowtip.y());
    state->closePath();
  }
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
        double delta = state->angle;
        double xa = cos(delta);
        double ya = sin(delta);
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
    double xr = state->xradius - state->outerXSep;
    double yr = state->yradius - state->outerYSep;
    QPointF a(xr,0),b(0,yr);
    state->moveTo(centerPoint);
    state->addEllipse(centerPoint,a,b);
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
        double delta = state->angle;
        double xa = cos(delta);
        double ya = sin(delta);
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
    state->addEllipse(centerPoint,a,b);
    state->moveTo(centerPoint.x() - tempdima, centerPoint.y());
    state->lineTo(centerPoint.x() + tempdima, centerPoint.y());
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
    state->moveTo(centerPoint);
    state->addCircle(centerPoint,radius);
    QPointF p1(-0.707107 * radius,-0.707107 * radius);
    QPointF p2(0.707107 * radius,0.707107 * radius);
    p1 = centerPoint + p1;
    p2 = centerPoint + p2;
    state->moveTo(p1);
    state->lineTo(p2);
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
    state->addShift(centerPoint.x(),centerPoint.y());
    state->rotate(rotate);
    state->moveTo(apex);
    state->lineTo(lowerleft);
    state->lineTo(lowerleft.x(),-lowerleft.y());
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
    state->moveTo(toppoint);
    state->lineTo(leftpoint);
    state->lineTo(bottompoint);
    state->lineTo(rightpoint);
    state->closePath();
  }
}

void XWTikzShape::magnifyingGlassShape(bool dopath)
{
  circleShape(dopath);
  if (dopath)
  {
    if (state->outerXSep < state->outerYSep)
      radius = radius - state->outerYSep;
    else
      radius = radius - state->outerXSep;
    state->moveTo(centerPoint);
    state->addCircle(centerPoint,radius); 
    
    QPointF a(radius * cos(state->MGHA),radius * sin(state->MGHA));
    QPointF b((radius + radius * state->MGHAaspect) * cos(state->MGHA),(radius + radius * state->MGHAaspect) * sin(state->MGHA));

    QPointF p1 = centerPoint + a;
    QPointF p2 = centerPoint + b;
    state->moveTo(p1);
    state->lineTo(p2);
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
        double delta = state->angle;
        double xb = cos(delta);
        double yb = sin(delta);
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
    state->addShift(centerPoint.x(),centerPoint.y());
    x = radius * cos(tailangle);
    y = radius * sin(tailangle);
    state->moveTo(x,y);
    state->addArc(tailangle,360,radius,radius);
    state->addArc(0,270,radius,radius);
    state->lineTo(radius + tailextend, -radius);
    state->lineTo(radius + tailextend, -radius + tailheight);
    state->closePath();
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
        double delta = state->angle;
        xa = cos(delta) + centerPoint.x();
        ya = sin(delta) + centerPoint.y();
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

    state->roundedCorners = 0;
    state->moveTo(beforecalloutpointer);
    state->lineTo(calloutpointer);
    state->lineTo(aftercalloutpointer);
    state->addShift(centerPoint.x(),centerPoint.y());
    state->lineTo(firstpoint);
    state->lineTo(secondpoint);
    state->lineTo(thirdpoint);
    state->lineTo(fourthpoint);
    state->closePath();
  }
}

void XWTikzShape::rectangleShape(bool dopath)
{
  rectangleAnchor();
  if (dopath)
  {
    double xa = southWest.x() + state->outerXSep;
    double ya = southWest.y() + state->outerYSep;
    double xb = northEast.x() - state->outerXSep;
    double yb = northEast.y() - state->outerYSep;

    QPointF ll(xa,ya);
    QPointF ur(xb,yb);
    state->moveTo(ll);
    state->addRectangle(ll,ur);
    centerPoint.setX(0.5 * southWest.x() + 0.5 * northEast.x());
    centerPoint.setY(0.5 * southWest.y() + 0.5 * northEast.y());
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
        double delta = state->angle;
        double tempdima = cos(delta);
        double tempdimb = sin(delta);
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
          state->flush();
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
          state->flush();
        }
      }
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

    for (int i = 0; i < parts; i++)
    {
      box->setXY(state->rectangleSplitPartAlign[i],centers[i].x() - 0.5 * wds[i],centers[i].y() +  0.5 * (hts[i] + deps[i]), i);
    }
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
        double delta = state->angle;
        xc = cos(delta) + centerPoint.x();
        yc = sin(delta) + centerPoint.y();

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
    state->moveTo(x,y);
    state->rotate(rotate);
    state->addArc(0,180,semicircleradius,semicircleradius);
    state->closePath();
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
        double delta = state->angle;
        double externalx = cos(delta) + centerPoint.x();
        double externaly = sin(delta) + centerPoint.y();
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
    state->moveTo(north);
    state->lineTo(northeast);
    state->lineTo(east);
    state->lineTo(southeast);
    state->lineTo(south);
    state->lineTo(southwest);
    state->lineTo(west);
    state->lineTo(northwest);
    state->closePath();
  }
}

void XWTikzShape::singleArrowShape(bool dopath)
{
  double x = state->outerXSep;
  double xoutersep = x;
  double y = state->outerYSep;
  double youtersep = y;

  double xa = state->innerXSep + 0.5 * (boxWidth + state->lineWidth);
  double ya = state->innerYSep + 0.5 * (boxHeight + boxDepth + state->lineWidth);

  double rotate = state->shapeBorderRotate;
  if (state->shapeBorderUsesIncircle)
  {
    if (xa < ya)
      xa = ya;
    xa *= 1.41421;
    ya = xa;
    if (xoutersep > youtersep)
      youtersep = xoutersep;
    else
      xoutersep = youtersep;
  }
  else
  {
    int counta = (int)rotate % 360;
    counta += 45;
    counta /= 90;
    counta *= 90;
    if (counta < 0)
      counta += 360;
    rotate = counta;
    if (counta == 90)
    {
      x = xa;
      xa = ya;
      ya = x;
      x = xoutersep;
      xoutersep = youtersep;
      youtersep = x;
    }
    else
    {
      if (counta == 270)
      {
        x = ya;
        xa = ya;
        ya = x;
        x = xoutersep;
        xoutersep = youtersep;
        youtersep = x;
      }
    }
  }

  double mathresult = state->singleArrowTipAngle / 2;
  double halftipangle = mathresult;
  mathresult = 1 / sin(halftipangle);
  double cosechalftipangle = mathresult;
  mathresult = cos(halftipangle);
  double tempdima = mathresult;
  tempdima *= cosechalftipangle;
  double cothalftipangle = tempdima;
  mathresult = 1 / mathresult;
  double sechalftipangle = mathresult;
  mathresult = sin(halftipangle);
  tempdima = mathresult;
  tempdima *= sechalftipangle;
//  double tanhalftipangle = tempdima;

  double xb = ya;
  xb *= cothalftipangle;
  double xc = state->singleArrowHeadExtend;
  double yc = xc;
  xc += ya;
  double tempdimb = state->minWidth;
  if (xc < 0.5 * tempdimb)
  {
    tempdimb *= 0.5;
    mathresult = tempdimb / xc;
    ya *= mathresult;
    xc *= mathresult;
    yc *= mathresult;
    xb *= mathresult;
  }

  xc -= ya;
  xc *= cothalftipangle;
  xa *= 2;
  xa += xb;
  tempdimb = state->minHeight;
  if (xa < tempdimb)
    xa = tempdimb;
  xa -= xb;
  xa *= 0.5;
  tempdima = state->singleArrowHeadIndent;

  QPointF arrowtip(xa + xb, 0);
  QPointF beforearrowtip(xa - xc, ya + yc);
  QPointF beforearrowhead(xa - xc + tempdima, ya);
  QPointF afterarrowtail(-xa, ya);
  x = centerPoint.x() + arrowtip.x() + cosechalftipangle * xoutersep;
  y = centerPoint.y() + arrowtip.y();
  QPointF arrowtipanchor(x,y);

  double externalradius = 0.5 * boxWidth;

  mathresult = state->angleBetweenLines(beforearrowtip,beforearrowhead,beforearrowtip,arrowtip);
  xa = xoutersep;
  tempdima = mathresult;
  tempdima *= 0.5;
  mathresult = 1 / sin(tempdima);
  xa *= mathresult;
  tempdima = -tempdima + 180 - halftipangle;
  x = xa * cos(tempdima) + centerPoint.x() + beforearrowtip.x();
  x = xa * sin(tempdima) + centerPoint.y() + beforearrowtip.y();
  QPointF beforearrowtipanchor(x,y);

  mathresult = state->angleBetweenPoints(beforearrowhead,beforearrowtip);
  tempdima = -mathresult + 180;
  tempdima *= 0.5;
  mathresult = 1 / sin(tempdima);
  xa = xoutersep;
  xa *= mathresult;
  tempdima = -tempdima + 180;
  x = xa * cos(tempdima) + centerPoint.x() + beforearrowhead.x();
  y = ya * sin(tempdima) + centerPoint.y() + beforearrowhead.y();
  QPointF beforearrowheadanchor(x,y);

  x = centerPoint.x() + afterarrowtail.x() - xoutersep;
  y = centerPoint.y() + afterarrowtail.y() + youtersep;
  QPointF afterarrowtailanchor(x,y);

  double centeranglebeforearrowtip = state->angleBetweenPoints(centerPoint,beforearrowtipanchor);
  double centeranglebeforearrowhead = state->angleBetweenPoints(centerPoint,beforearrowheadanchor);
  double centerangleafterarrowtail = state->angleBetweenPoints(centerPoint,afterarrowtailanchor);

  double midanglebeforearrowtip = state->angleBetweenPoints(midPoint,beforearrowtipanchor);
  double midanglebeforearrowhead = state->angleBetweenPoints(midPoint,beforearrowheadanchor);
  double midangleafterarrowtail = state->angleBetweenPoints(midPoint,afterarrowtailanchor);

  double baseanglebeforearrowtip = state->angleBetweenPoints(basePoint,beforearrowtipanchor);
  double baseanglebeforearrowhead = state->angleBetweenPoints(basePoint,beforearrowheadanchor);
  double baseangleafterarrowtail = state->angleBetweenPoints(basePoint,afterarrowtailanchor);

#define XW_TIKZ_SHAPE_SINGLEARROW_ANCHOR_BORDER(ex,ey) \
  QPointF ep(referencePoint.x() + ex, referencePoint.y() + ey);\
  mathresult = state->angleBetweenPoints(centerPoint,ep); \
  mathresult -= rotate; \
  if (mathresult < 0) \
    mathresult += 360; \
  double externalangle = mathresult; \
  x = externalangle;\
  if (referencePoint == midPoint) \
  {\
    xa = midanglebeforearrowtip;\
    xb = midanglebeforearrowhead;\
    xc = midangleafterarrowtail; \
  }\
  else \
  {\
    if (referencePoint == basePoint)\
    {\
      xa = baseanglebeforearrowtip;\
      xb = baseanglebeforearrowhead;\
      xc = baseangleafterarrowtail; \
    }\
    else\
    {\
      xa = centeranglebeforearrowtip;\
      xb = centeranglebeforearrowhead;\
      xc = centerangleafterarrowtail; \
    }\
  } \
  QPointF firstpoint,secondpoint;\
  if (x < xa)\
  {\
    firstpoint = arrowtipanchor;\
    secondpoint = beforearrowtipanchor;\
  }\
  else \
  {\
    if (x < xb)\
    {\
      if (xb < xa)\
      {\
        firstpoint = arrowtipanchor;\
        secondpoint = beforearrowtipanchor;\
      }\
      else \
      {\
        firstpoint = beforearrowheadanchor;\
        secondpoint = beforearrowtipanchor;\
      }\
    }\
    else \
    {\
      if (x < xc)\
      {\
        firstpoint = beforearrowheadanchor;\
        secondpoint = afterarrowtailanchor;\
      }\
      else \
      {\
        xc = -xc + 360;\
        if (x < xc)\
        {\
          firstpoint = afterarrowtailanchor;\
          x = centerPoint.x() - afterarrowtailanchor.x() + centerPoint.x();\
          y = afterarrowtailanchor.y() - centerPoint.y() + centerPoint.y();\
          secondpoint.setX(x);\
          secondpoint.setY(y);\
        }\
        else \
        {\
          xa = -xa + 360;\
          xb = -xb + 360;\
          if (x < xa)\
          {\
            if (x < xb)\
            {\
              x = centerPoint.x() - afterarrowtailanchor.x() + centerPoint.x();\
              y = afterarrowtailanchor.y() - centerPoint.y() + centerPoint.y();\
              firstpoint.setX(x);\
              firstpoint.setY(y);\
              x = centerPoint.x() - beforearrowheadanchor.x() + centerPoint.x();\
              y = beforearrowheadanchor.y() - centerPoint.y() + centerPoint.y();\
              secondpoint.setX(x);\
              secondpoint.setY(y);\
            }\
            else \
            {\
              if (xb < xa)\
              {\
                x = centerPoint.x() - beforearrowheadanchor.x() + centerPoint.x();\
                y = beforearrowheadanchor.y() - centerPoint.y() + centerPoint.y();\
                firstpoint.setX(x);\
                firstpoint.setY(y);\
              }\
              else\
              {\
                x = centerPoint.x() - arrowtipanchor.x() + centerPoint.x();\
                y = arrowtipanchor.y() - centerPoint.y() + centerPoint.y();\
                firstpoint.setX(x);\
                firstpoint.setY(y);\
              }\
              x = centerPoint.x() - beforearrowtipanchor.x() + centerPoint.x();\
              y = beforearrowtipanchor.y() - centerPoint.y() + centerPoint.y();\
              secondpoint.setX(x);\
              secondpoint.setY(y);\
            }\
          }\
          else \
          {\
            x = centerPoint.x() - beforearrowheadanchor.x() + centerPoint.x();\
            y = beforearrowheadanchor.y() - centerPoint.y() + centerPoint.y();\
            firstpoint.setX(x);\
            firstpoint.setY(y);\
            secondpoint = arrowtipanchor;\
          }\
        }\
      }\
    }\
  }\
  firstpoint = state->rotatePointAround(firstpoint,centerPoint,rotate);\
  secondpoint = state->rotatePointAround(secondpoint,centerPoint,rotate);\
  anchorPos = state->intersectionOfLines(referencePoint,ep,firstpoint,secondpoint);

  switch (state->anchor)
  {
    default:
      {
        double delta = state->angle;
        double externalx = cos(delta);
        double externaly = sin(delta);
        XW_TIKZ_SHAPE_SINGLEARROW_ANCHOR_BORDER(externalx,externaly)
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
      {
        referencePoint = midPoint;
        XW_TIKZ_SHAPE_SINGLEARROW_ANCHOR_BORDER(externalradius,0)
      }
      break;

    case PGFmidwest:
      {
        referencePoint = midPoint;
        XW_TIKZ_SHAPE_SINGLEARROW_ANCHOR_BORDER(-externalradius,0)
      }
      break;

    case PGFbase:
      anchorPos = basePoint;
      break;

    case PGFbaseeast:
      {
        referencePoint = basePoint;
        XW_TIKZ_SHAPE_SINGLEARROW_ANCHOR_BORDER(externalradius,0)
      }
      break;

    case PGFbasewest:
      {
        referencePoint = basePoint;
        XW_TIKZ_SHAPE_SINGLEARROW_ANCHOR_BORDER(-externalradius,0)
      }
      break;

    case PGFnorth:
      {
        XW_TIKZ_SHAPE_SINGLEARROW_ANCHOR_BORDER(0,externalradius)
      }
      break;

    case PGFsouth:
      {
        XW_TIKZ_SHAPE_SINGLEARROW_ANCHOR_BORDER(0,-externalradius)
      }
      break;

    case PGFeast:
      {
        XW_TIKZ_SHAPE_SINGLEARROW_ANCHOR_BORDER(externalradius,0)
      }
      break;

    case PGFwest:
      {
        XW_TIKZ_SHAPE_SINGLEARROW_ANCHOR_BORDER(-externalradius,0)
      }
      break;

    case PGFnortheast:
      {
        XW_TIKZ_SHAPE_SINGLEARROW_ANCHOR_BORDER(externalradius,externalradius)
      }
      break;

    case PGFsoutheast:
      {
        XW_TIKZ_SHAPE_SINGLEARROW_ANCHOR_BORDER(externalradius,-externalradius)
      }
      break;

    case PGFsouthwest:
      {
        XW_TIKZ_SHAPE_SINGLEARROW_ANCHOR_BORDER(-externalradius,-externalradius)
      }
      break;

    case PGFnorthwest:
      {
        XW_TIKZ_SHAPE_SINGLEARROW_ANCHOR_BORDER(-externalradius,externalradius)
      }
      break;

    case PGFbeforehead:
      anchorPos = state->rotatePointAround(beforearrowheadanchor,centerPoint,rotate);
      break;

    case PGFbeforetip:
      anchorPos = state->rotatePointAround(beforearrowtipanchor,centerPoint,rotate);
      break;

    case PGFtip:
      anchorPos = state->rotatePointAround(arrowtipanchor,centerPoint,rotate);
      break;

    case PGFaftertip:
      {
        x = centerPoint.x() - beforearrowtipanchor.x() + centerPoint.x();
        y = -centerPoint.y() + beforearrowtipanchor.y() + centerPoint.y();
        QPointF p(x,y);
        anchorPos = state->rotatePointAround(p,centerPoint,rotate);
      }
      break;

    case PGFafterhead:
      {
        x = centerPoint.x() - beforearrowheadanchor.x() + centerPoint.x();
        y = -centerPoint.y() + beforearrowheadanchor.y() + centerPoint.y();
        QPointF p(x,y);
        anchorPos = state->rotatePointAround(p,centerPoint,rotate);
      }
      break;

    case PGFbeforetail:
      {
        x = centerPoint.x() - afterarrowtailanchor.x() + centerPoint.x();
        y = -centerPoint.y() + afterarrowtailanchor.y() + centerPoint.y();
        QPointF p(x,y);
        anchorPos = state->rotatePointAround(p,centerPoint,rotate);
      }
      break;

    case PGFaftertail:
      anchorPos = state->rotatePointAround(afterarrowtailanchor,centerPoint,rotate);
      break;

    case PGFtail:
      {
        QPointF p1 = state->rotatePointAround(afterarrowtailanchor,centerPoint,rotate);
        x = centerPoint.x() - afterarrowtailanchor.x() + centerPoint.x();
        y = -centerPoint.y() + afterarrowtailanchor.y() + centerPoint.y();
        QPointF p2(x,y);
        p2 = state->rotatePointAround(p2,centerPoint,rotate);
        anchorPos = state->lineAtTime(0.5,p1,p2);
      }
      break;
  }

  if (dopath)
  {
    state->addShift(centerPoint.x(),centerPoint.y());
    state->rotate(rotate);
    state->moveTo(arrowtip);
    state->lineTo(beforearrowtip);
    state->lineTo(beforearrowhead);
    state->lineTo(afterarrowtail);
    state->lineTo(afterarrowtail.x(),-afterarrowtail.y());
    state->lineTo(beforearrowhead.x(),-beforearrowhead.y());
    state->lineTo(beforearrowtip.x(),-beforearrowtip.y());
    state->closePath();
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
        double delta = state->angle;
        double externalx = cos(delta) + centerPoint.x();
        double externaly = sin(delta) + centerPoint.y();
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
      state->moveTo(points[0]);
      for (int i = 1; i < points.size(); i++)
        state->lineTo(points[i]);
    }
  }
}

void XWTikzShape::strikeoutShape(bool dopath)
{
  rectangleAnchor();
  if (dopath)
  {
    state->moveTo(southWest);
    state->lineTo(northEast);
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
        double delta = state->angle;
        double externalx = cos(delta) + centerPoint.x();
        double externaly = sin(delta) + centerPoint.y();
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
    state->moveTo(lowerleftpoint);
    state->lineTo(upperleftpoint);
    state->lineTo(upperrightpoint);
    state->lineTo(lowerrightpoint);
    state->closePath();
  }
}

QPointF XWTikzShape::pointIntersectionOfLineAndArc(const QPointF & p1,
                                        const QPointF & p2,
                                        const QPointF & c,
                                        double sa,double ea,
                                        double xr,double yr)
{
  double x = state->angleBetweenPoints(p1,p2);
  double s = sa;
  double ss = (int)s % 360;
  if (ss < 0)
    ss += 360;

  double e = ea;
  double ee = (int)e % 360;
  if (ee < 0)
    ee += 360;

  if (ee < ss)
    x = ((int)x + 180) % 360;
  int m = 360;
  double n = (s + e) / 2;
  double xarcradius = xr;
  double yarcradius = yr;
  while (true)
  {
    double p = (s + e) / 2;
    if (p == s)
      break;

    QPointF oo(xarcradius * cos(p),yarcradius * sin(p));
    QPointF oc = c + oo;
    double mathresult = state->angleBetweenPoints(p2,oc);
    if (ee < ss)
    {
      mathresult += 180;
      mathresult = (int)mathresult % 360;
    }
    double q = mathresult;
    if (x > 335)
    {
      if (q < 45)
        q += 360;
    }

    if (x == q)
      break;

    if (x < q)
      e = p;
    else
      s = p;

    mathresult = qAbs(x - q);
    if (mathresult < m)
    {
      m = mathresult;
      n = p;
    }
  }

  QPointF ret(xarcradius * cos((double)n),yarcradius * sin((double)n));
  ret = ret + c;
  return ret;
}

void XWTikzShape::circleAnchor()
{
  double x,y;
  if (state->shape == PGFcirclesplit)
  {
    double ht = 0;
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
    
    double wd = 0;
    if (box)
      wd = box->getWidth(state->textWidth,1);
    double xa = 0.5 * boxWidth;
    if (xa < 0.5 * wd)
      xa = 0.5 * wd;
    xa += state->innerXSep;
    QVector2D v(xa,ya);
    v.normalize();
    double x = v.x();
    double y = v.y();
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
  }
  else if (state->shape == PGFcirclesolidus)
  {
    double ht = 0;
    double dp = 0;
    double wd = 0;
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
  }
  else
  {
    double ya = 0.5 * boxHeight + 0.5 * boxDepth + state->innerYSep;
    double xa = 0.5 * boxWidth + state->innerXSep;
    radius = sqrt(xa * xa + ya * ya);
    x = xa / radius;
    y = ya / radius;
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
  }

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
  }

  anchorPos.setX(x);
  anchorPos.setY(y);
}

void XWTikzShape::rectangleAnchor()
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
  }

  anchorPos.setX(x);
  anchorPos.setY(y);
}
