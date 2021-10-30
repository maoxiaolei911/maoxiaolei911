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
        double externalx = state->anotherPoint.x();
        double externaly = state->anotherPoint.y();
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
    state = state->save(false);
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

    state = state->restore();

    doContent();
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
        double externalx = state->anotherPoint.x();
        double externaly = state->anotherPoint.y();
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
    state = state->save(false);
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
    state = state->restore();

    doContent();
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
        double externalx = state->anotherPoint.x();
        double externaly = state->anotherPoint.y();
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
    state = state->save(false);
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
    state = state->restore();

    doContent();
  }
}
