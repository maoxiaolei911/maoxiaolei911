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

#define XW_TIKZ_SHAPE_LOGIC_GATE_US_OR \
  int numinputs = state->inputs.size(); \
  double halflinewidth = 0.5 * state->lineWidth; \
  double invertedradius = state->logicGateInvertedRadius; \
  double outerinvertedradius = state->logicGateInvertedRadius; \
  double x = 0.5 * boxWidth + state->innerXSep; \
  double y = 0.5 * boxHeight + boxDepth + state->innerYSep; \
  int counta = numinputs + 1; \
  double ya = state->logicGateInvertedRadius + state->lineWidth; \
  double yb = 0.5 * state->logicGateInputSep; \
  if (ya < yb) \
    ya = yb; \
  ya *= counta; \
  if (y < ya) \
    y = ya; \
  if (x > y) \
    y = x; \
  else \
    x = y; \
  double xa = state->minWidth; \
  ya = state->minHeight; \
  if (y < 0.5 * ya) \
    y = 0.5 * ya; \
  double xb = x + 1.732051 * y;\
  if (xb < xa) \
  {\
    xb = xa; \
    x = 0.366025 * xb; \
  }\
  if (x > y) \
    y = x; \
  else \
    x = y; \
  double halfside = x; \
  xa = state->outerXSep;\
  ya = state->outerYSep;\
  double outerxsep = xa;\
  double outerysep = ya;\
  x += xa; \
  y += ya; \
  double halfwidth = x; \
  double halfheight = y; \
  x -= xa; \
  y -= ya; \
  x *= -0.16666; \
  yb = 2 * y; \
  x += 0.866025 * yb; \
  x += 1.154701 * xa; \
  y = 0; \
  QPointF tipanchor(x,y);


#define XW_TIKZ_SHAPE_LOGIC_GATE_US_AND \
  int numinputs = state->inputs.size(); \
  double invertedradius = state->logicGateInvertedRadius; \
  double outerinvertedradius = state->logicGateInvertedRadius; \
  double x = 0.5 * boxWidth + state->innerXSep; \
  double y = 0.5 * boxHeight + boxDepth + state->innerYSep; \
  int counta = numinputs + 1; \
  double ya = state->logicGateInvertedRadius + state->lineWidth; \
  double yb = 0.5 * state->logicGateInputSep; \
  if (ya < yb) \
    ya = yb; \
  ya *= counta; \
  if (y < ya) \
    y = ya; \
  if (x > y) \
    y = x; \
  else \
    x = y; \
  double xa = state->minWidth; \
  ya = state->minHeight; \
  if (y < 0.5 * ya) \
    y = 0.5 * ya; \
  double xb = 2.5 * x; \
  if (xb < xa )\
  {\
    xb = xa; \
    x = 0.4 * xb; \
  } \
  if (x > y) \
    y = x; \
  else \
    x = y; \
  double halfside = x; \
  xa = state->outerXSep;\
  ya = state->outerYSep;\
  double outerxsep = xa;\
  double outerysep = ya;\
  x += xa; \
  y += ya; \
  double halfwidth = x; \
  double halfheight = y;

#define XW_TIKZ_SHAPE_LOGIC_GATE_IEC_AND_INPUT_ANCHOR(n) \
  xa = halfside;\
  x = centerPoint.x() - 0.166666 * xa - halfwidth;\
  if (state->inputs[n - 1] == QChar('i')) \
  {\
    x -= invertedradius; \
    x -= outerinvertedradius; \
  }\
  double tempdima = 2 * halfside; \
  tempdima /= counta; \
  tempdima *= n; \
  y += halfside;\
  y -= tempdima;

#define XW_TIKZ_SHAPE_LOGIC_GATE_IEC_OR_INPUT_ANCHOR(n) \
  ya = halfside; \
  yb = 2 * ya; \
  double tempdima = halfside * 2; \
  tempdima /= counta; \
  tempdima *= n; \
  double yc = ya - tempdima; \
  xb = yb - halflinewidth; \
  double mathresult = yc / yb; \
  mathresult = asin(mathresult); \
  mathresult = cos(mathresult); \
  double xc = -1.166666 * ya - 0.866025 * xb + mathresult * yb + halflinewidth - outerxsep; \
  if (state->inputs[n - 1] == QChar('i')) \
  {\
    xc -= invertedradius;\
    xc -= outerinvertedradius;\
  }\
  x = centerPoint.x() + xc; \
  y = centerPoint.y() + yc; 

#define XW_TIKZ_SHAPE_LOGIC_GATE_IEC_XOR_INPUT_ANCHOR(n) \
  ya = halfside; \
  yb = 2 * ya; \
  double tempdima = 2 * halfside; \
  tempdima /= counta; \
  tempdima *= n; \
  double yc = ya - tempdima; \
  xb = yb; \
  yb -= halflinewidth; \
  double mathresult = yc / yb; \
  mathresult = asin(mathresult); \
  mathresult = cos(mathresult); \
  double xc = -1.166666 * ya - 0.866025 * xb + mathresult * yb + halflinewidth - outerxsep; \
  if (state->inputs[n - 1] == QChar('i')) \
  {\
    xc -= invertedradius;\
    xc -= outerinvertedradius;\
  }\
   x = centerPoint.x() + xc; \
  y = centerPoint.y() + yc; \
  xa = halfside; \
  x -= 0.333333 * xa;

void XWTikzShape::andGateUSShape(bool dopath)
{
  XW_TIKZ_SHAPE_LOGIC_GATE_US_AND
  switch (state->anchor)
  {
    default:
      {
        x = state->anotherPoint.x() + centerPoint.x();
        y = state->anotherPoint.y() + centerPoint.y();
        QPointF externalpoint(x,y);             
        double externalangle = state->angleBetweenPoints(centerPoint, externalpoint);
        xb = halfside;
        double xc = 1.66666 * xb + outerxsep;
        double yc = halfheight;
        xa = halfside;
        x = centerPoint.x() - 1.16666 * xa - outerxsep;
        y = centerPoint.y() + halfheight;
        QPointF northwest(x,y);   
        double mathresult = state->angleBetweenPoints(centerPoint, northwest);
        if (externalangle < externalangle)
        {
          x = centerPoint.x() + 0.333333 * xb;
          y = centerPoint.y() + yc;
          QPointF p1(x,y);
          mathresult = state->angleBetweenPoints(centerPoint, p1);
          if (externalangle < mathresult)
          {
            x = centerPoint.x() + 0.333333 * xb;
            y = centerPoint.y();
            QPointF c(x,y);
            anchorPos = pointIntersectionOfLineAndArc(externalpoint,centerPoint,c,0,90,halfwidth,halfheight);
          }
          else
          {
            x = centerPoint.x() + 0.333333 * xb;
            y = centerPoint.y() + yc;
            QPointF p3(x,y);
            x = centerPoint.x() - xc;
            y = centerPoint.y() + yc;
            QPointF p4(x,y);
            anchorPos = state->intersectionOfLines(externalpoint,centerPoint,p3,p4);
          }
        }
        else
        {
          x = centerPoint.x() + 0.333333 * xb;
          y = centerPoint.y() - yc;
          QPointF p3(x,y);
          mathresult = state->angleBetweenPoints(centerPoint, p3);
          if (externalangle > mathresult)
          {
            x = centerPoint.x() + 0.333333 * xb;
            y = centerPoint.y();
            QPointF c(x,y);
            anchorPos = pointIntersectionOfLineAndArc(externalpoint,centerPoint,c,270,360,halfwidth,halfheight);
          }
          else
          {
            xa = halfside;
            x = centerPoint.x() - 1.16666 * xa - outerxsep;
            y = centerPoint.y() - halfheight;
            QPointF southwest(x,y);
            mathresult = state->angleBetweenPoints(centerPoint, southwest);
            if (externalangle > mathresult)
            {
              x = centerPoint.x() + 0.333333 * xb;
              y = centerPoint.y() - yc;
              QPointF p3(x,y);
              anchorPos = state->intersectionOfLines(externalpoint,centerPoint,p3,southwest);
            }
            else
              anchorPos = state->intersectionOfLines(externalpoint,centerPoint,northwest,southwest);
          }
        }
      }
      break;

    case PGFcenter:
      anchorPos = centerPoint;
      break;

    case PGFmid:
      anchorPos = midPoint;
      break;

    case PGFmideast:
      xa = halfwidth;
      xb = xa - outerxsep;
      x = centerPoint.x() + 0.333333 * xb + xa;
      y = midPoint.y();
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFmidwest:
      xa = halfside;
      x = centerPoint.x() + -1.16666 * xa - outerxsep;
      y = midPoint.y();
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFbase:
      anchorPos = basePoint;
      break;

    case PGFbaseeast:
      xa = halfwidth;
      xb = xa - outerxsep;
      x = centerPoint.x() + 0.333333 * xb + xa;
      y = basePoint.y();
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFbasewest:
      xa = halfside;
      x = centerPoint.x() + -1.16666 * xa - outerxsep;
      y = basePoint.y();
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFinput:
      {
        int i = (int)(state->angle);
        XW_TIKZ_SHAPE_LOGIC_GATE_IEC_AND_INPUT_ANCHOR(i);
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      break;

    case PGFoutput:
    case PGFeast:
      xa = halfwidth;
      xb = xa - outerxsep;
      x = centerPoint.x() + 0.333333 * xb + xa;
      y = centerPoint.y();
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFnortheast:
      if (state->logicGateAnchorsUseBoundingBox)
      {
        xa = halfwidth;
        xb = xa - outerxsep;
        x = centerPoint.x() + 0.333333 * xb + xa;
        y = centerPoint.y() + halfheight;          
      }
      else
      {
        xa = halfwidth;
        ya = halfheight;
        xb = xa - outerxsep;
        x = centerPoint.x() + 0.333333 * xb + 0.707106 * xa;
        y = centerPoint.y() + 0.707106 * ya;
      }
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFsoutheast:
      if (state->logicGateAnchorsUseBoundingBox)
      {
        xa = halfwidth;
        xb = xa - outerxsep;
        x = centerPoint.x() + 0.333333 * xb + xa;
        y = centerPoint.y() - halfheight;          
      }
      else
      {
        xa = halfwidth;
        ya = halfheight;
        xb = xa - outerxsep;
        x = centerPoint.x() + 0.333333 * xb + 0.707106 * xa;
        y = centerPoint.y() - 0.707106 * ya;
      }
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFnorth:
      anchorPos.setX(centerPoint.x());
      anchorPos.setY(centerPoint.y() + halfheight);
      break;

    case PGFsouth:
      anchorPos.setX(centerPoint.x());
      anchorPos.setY(centerPoint.y() - halfheight);
      break;

    case PGFsouthwest:
      xa = halfside;
      x = centerPoint.x() - 1.16666 * xa - outerxsep;
      y = centerPoint.y() - halfheight;
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFnorthwest:
      xa = halfside;
      x = centerPoint.x() - 1.16666 * xa - outerxsep;
      y = centerPoint.y() + halfheight;
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFwest:
      xa = halfside;
      x = centerPoint.x() + -1.16666 * xa - outerxsep;
      y = centerPoint.y();
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;
  }

  if (dopath)
  {
    double xc = halfwidth - outerxsep;
    double yc = halfheight - outerysep;
    xb = 0.333333 * xc;
    state = state->save();
    state->shift(centerPoint.x(), centerPoint.y());
    state->moveTo(xb, yc);
    state->setStartAngle(0);
    state->setEndAngle(-90);
    state->setXRadius(yc);
    state->setYRadius(yc);
    state->addArc();
    state->lineTo(-1.16666 * xc, -yc);
    state->lineTo(-1.16666 * xc, yc);
    state->closePath();
    state = state->restore();

    double tempdima = 2 * yc;
    tempdima /= counta;
    for (int i = 0; i < numinputs; i++)
    {
      yc -= tempdima;
      if (state->inputs[i] == QChar('i'))
      {
        state = state->save();
        x = -1.16666 * halfside - 0.5 * state->lineWidth - invertedradius;
        y = yc;
        QPointF c(x, y);
        state->addCircle(c,invertedradius);
        state = state->restore();
      }
    }
  }
}

void XWTikzShape::bufferGateUSShape(bool dopath)
{
  XW_TIKZ_SHAPE_LOGIC_GATE_US_OR
  switch (state->anchor)
  {
    default:
      {
        x = state->anotherPoint.x() + centerPoint.x();
        y = state->anotherPoint.y() + centerPoint.y();
        QPointF externalpoint(x,y);
        double mathresult = state->angleBetweenPoints(centerPoint, externalpoint);
        if (mathresult == 0.0)
        {
          x = centerPoint.x() + tipanchor.x() + 2.350943 * outerxsep;
          y = centerPoint.y() + tipanchor.y();
          anchorPos.setX(x);
          anchorPos.setY(y);
        }
        else
        {
          if (mathresult == 180)
          {
            XW_TIKZ_SHAPE_LOGIC_GATE_IEC_AND_INPUT_ANCHOR(1);
            double xc = halfside;
            x += 0.333333 * xc;
            anchorPos.setX(x);
            anchorPos.setY(y);
          }
          else
          {
            double externalangle = mathresult;
            xa = halfside;
            x = centerPoint.x() - 0.833333 * xa - outerxsep;
            y = centerPoint.y() + 1.166666 * xa;
            ya = outerysep;
            y += 1.565 * ya;
            QPointF northwest(x,y);
            mathresult = state->angleBetweenPoints(centerPoint, northwest);
            if (externalangle < mathresult)
            {
              x = centerPoint.x() + tipanchor.x() + 2.350943 * outerxsep;
              y = centerPoint.y() + tipanchor.y();
              QPointF p(x,y);
              anchorPos = state->intersectionOfLines(centerPoint, externalpoint,northwest,p);
            }
            else
            {
              xa = halfside;
              x = centerPoint.x() - 0.833333 * xa - outerxsep;
              y = centerPoint.y() - 1.166666 * xa;
              ya = outerysep;
              y -= 1.565 * ya;
              QPointF southwest(x,y);
              mathresult = state->angleBetweenPoints(centerPoint, southwest);
              if (externalangle < mathresult)
                anchorPos = state->intersectionOfLines(externalpoint,centerPoint, northwest,southwest);
              else
              {
                x = centerPoint.x() + tipanchor.x() + 2.350943 * outerxsep;
                y = centerPoint.y() + tipanchor.y();
                QPointF p(x,y);
                anchorPos = state->intersectionOfLines(centerPoint, externalpoint,southwest,p);
              }
            }
          }
        }
      }
      break;

    case PGFcenter:
      anchorPos = centerPoint;
      break;

    case PGFmid:
      anchorPos = midPoint;
      break;

    case PGFmideast:
      x = centerPoint.x() + tipanchor.x() + 2.350943 * outerxsep;
      y = midPoint.y();
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFmidwest:
      {
        double xc = -0.833333 * halfside - outerxsep;
        x = midPoint.x() + xc;
        y = midPoint.y();
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      break;

    case PGFbase:
      anchorPos = basePoint;
      break;

    case PGFbaseeast:
      x = centerPoint.x() + tipanchor.x() + 2.350943 * outerxsep;
      y = basePoint.y();
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFbasewest:
      {
        double xc = -0.833333 * halfside - outerxsep;
        x = midPoint.x() + xc;
        y = basePoint.y();
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      break;

    case PGFoutput:
    case PGFeast:
      x = centerPoint.x() + tipanchor.x() + 2.350943 * outerxsep;
      y = centerPoint.y() + tipanchor.y();
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFnorth:
      if (state->logicGateAnchorsUseBoundingBox)
      {
        xa = halfside;
        x = centerPoint.x();
        y = centerPoint.y() + 1.166666 * xa;
        ya = outerysep;
        y += 1.565 * ya;
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      else
      {
        x = centerPoint.x();
        y = centerPoint.y() + 1;
        QPointF p1(x,y);
        xa = halfside;
        x = centerPoint.x() - 0.833333 * xa - outerxsep;
        y = centerPoint.y() + 1.166666 * xa;
        ya = outerysep;
        y += 1.565 * ya;
        QPointF northwest(x,y);
        x = centerPoint.x() + tipanchor.x() + 2.350943 * outerxsep;
        y = centerPoint.y() + tipanchor.y();
        QPointF p2(x,y);
        anchorPos = state->intersectionOfLines(centerPoint, p1, northwest, p2);
      }
      break;

    case PGFsouth:
      if (state->logicGateAnchorsUseBoundingBox)
      {
        xa = halfside;
        x = centerPoint.x();
        y = centerPoint.y() - 1.166666 * xa;
        ya = outerysep;
        y -= 1.565 * ya;
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      else
      {
        x = centerPoint.x();
        y = centerPoint.y() - 1;
        QPointF p1(x,y);
        xa = halfside;
        x = centerPoint.x() - 0.833333 * xa - outerxsep;
        y = centerPoint.y() - 1.166666 * xa;
        ya = outerysep;
        y -= 1.565 * ya;
        QPointF southwest(x,y);
        x = centerPoint.x() + tipanchor.x() + 2.350943 * outerxsep;
        y = centerPoint.y() + tipanchor.y();
        QPointF p2(x,y);
        anchorPos = state->intersectionOfLines(centerPoint, p1, southwest, p2);
      }
      break;

    case PGFsoutheast:
      if (state->logicGateAnchorsUseBoundingBox)
      {
        xa = halfside;
        x = centerPoint.x() + tipanchor.x() + 2.350943 * outerxsep;
        y = centerPoint.y() - 1.166666 * xa;
        ya = outerysep;
        y -= 1.565 * ya;
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      else
      {
        x = centerPoint.x() + 1;
        y = centerPoint.y() - 1;
        QPointF p1(x,y);
        xa = halfside;
        x = centerPoint.x() - 0.833333 * xa - outerxsep;
        y = centerPoint.y() - 1.166666 * xa;
        ya = outerysep;
        y -= 1.565 * ya;
        QPointF southwest(x,y);
        x = centerPoint.x() + tipanchor.x() + 2.350943 * outerxsep;
        y = centerPoint.y() + tipanchor.y();
        QPointF p2(x,y);
        anchorPos = state->intersectionOfLines(centerPoint, p1, southwest, p2);
      }
      break;

    case PGFnortheast:
      if (state->logicGateAnchorsUseBoundingBox)
      {
        xa = halfside;
        x = centerPoint.x() + tipanchor.x() + 2.350943 * outerxsep;
        y = centerPoint.y() + 1.166666 * xa;
        ya = outerysep;
        y += 1.565 * ya;
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      else
      {
        x = centerPoint.x() + 1;
        y = centerPoint.y() + 1;
        QPointF p1(x,y);
        xa = halfside;
        x = centerPoint.x() - 0.833333 * xa - outerxsep;
        y = centerPoint.y() + 1.166666 * xa;
        ya = outerysep;
        y += 1.565 * ya;
        QPointF northwest(x,y);
        x = centerPoint.x() + tipanchor.x() + 2.350943 * outerxsep;
        y = centerPoint.y() + tipanchor.y();
        QPointF p2(x,y);
        anchorPos = state->intersectionOfLines(centerPoint, p1, northwest, p2);
      }
      break;

    case PGFsouthwest:
      xa = halfside;
      x = centerPoint.x() - 0.833333 * xa - outerxsep;
      y = centerPoint.y() - 1.166666 * xa;
      ya = outerysep;
      y -= 1.565 * ya;
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFnorthwest:
      xa = halfside;
      x = centerPoint.x() - 0.833333 * xa - outerxsep;
      y = centerPoint.y() + 1.166666 * xa;
      ya = outerysep;
      y += 1.565 * ya;
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFinput:
      {
        XW_TIKZ_SHAPE_LOGIC_GATE_IEC_AND_INPUT_ANCHOR(1);
        double xc = halfside;
        x += 0.333333 * xc;
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      break;

    case PGFwest:
      {
        double xc = -0.833333 * halfside;
        x = centerPoint.x() + xc;
        y = centerPoint.y();
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      break;
  }

  if (dopath)
  {
    double xc = halfwidth - outerxsep;
    double yc = halfheight - outerysep;
    state = state->save();
    state->shift(centerPoint.x(), centerPoint.y());
    state->moveTo(tipanchor);
    state->lineTo(-0.833333 * xc, 1.166666 * yc);
    state->lineTo(-0.833333 * xc, -1.166666 * yc);
    state->closePath();
    state = state->restore();

    if (state->inputs[0] == QChar('i'))
    {
      xa = halfside;
      x = -0.833333 * xa - 0.5 * state->lineWidth - invertedradius;
      y = 0;
      QPointF c(x,y);
      state = state->save();
      state->addCircle(c,invertedradius);
      state = state->restore();
    }
  }
}

void XWTikzShape::nandGateUSShape(bool dopath)
{
  XW_TIKZ_SHAPE_LOGIC_GATE_US_AND
  switch (state->anchor)
  {
    default:
      {
        x = state->anotherPoint.x();
        y = state->anotherPoint.y();
        QPointF externalpoint(x,y);
        double externalangle = state->angleBetweenPoints(centerPoint, externalpoint);
        xb = halfside;
        double xc = 1.66666 * xb + outerxsep;
        double yc = halfheight;
        xa = halfside;
        x = centerPoint.x() - 1.16666 * xa - outerxsep;
        y = centerPoint.y() + halfheight;
        QPointF northwest(x,y);      
        double mathresult = state->angleBetweenPoints(centerPoint, northwest);
        if (externalangle < externalangle)
        {
          x = centerPoint.x() + 0.333333 * xb;
          y = centerPoint.y() + yc;
          QPointF p1(x,y);
          mathresult = state->angleBetweenPoints(centerPoint, p1);
          if (externalangle < mathresult)
          {
            x = centerPoint.x() + 0.333333 * xb;
            y = centerPoint.y();
            QPointF c(x,y);
            anchorPos = pointIntersectionOfLineAndArc(externalpoint,centerPoint,c,0,90,halfwidth,halfheight);
          }
          else
          {
            x = centerPoint.x() + 0.333333 * xb;
            y = centerPoint.y() + yc;
            QPointF p3(x,y);
            x = centerPoint.x() - xc;
            y = centerPoint.y() + yc;
            QPointF p4(x,y);
            anchorPos = state->intersectionOfLines(externalpoint,centerPoint,p3,p4);
          }
        }
        else
        {
          x = centerPoint.x() + 0.333333 * xb;
          y = centerPoint.y() - yc;
          QPointF p3(x,y);
          mathresult = state->angleBetweenPoints(centerPoint, p3);
          if (externalangle > mathresult)
          {
            x = centerPoint.x() + 0.333333 * xb;
            y = centerPoint.y();
            QPointF c(x,y);
            anchorPos = pointIntersectionOfLineAndArc(externalpoint,centerPoint,c,270,360,halfwidth,halfheight);
          }
          else
          {
            xa = halfside;
            x = centerPoint.x() - 1.16666 * xa - outerxsep;
            y = centerPoint.y() - halfheight;
            QPointF southwest(x,y);
            mathresult = state->angleBetweenPoints(centerPoint, southwest);
            if (externalangle > mathresult)
            {
              x = centerPoint.x() + 0.333333 * xb;
              y = centerPoint.y() - yc;
              QPointF p3(x,y);
              anchorPos = state->intersectionOfLines(externalpoint,centerPoint,p3,southwest);
            }
            else
              anchorPos = state->intersectionOfLines(externalpoint,centerPoint,northwest,southwest);
          }
        }
      }
      break;

    case PGFcenter:
      anchorPos = centerPoint;
      break;

    case PGFmid:
      anchorPos = midPoint;
      break;

    case PGFmideast:
      xa = halfwidth;
      xb = xa - outerxsep;
      x = centerPoint.x() + 0.333333 * xb + xa;
      y = midPoint.y();
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFmidwest:
      xa = halfside;
      x = centerPoint.x() + -1.16666 * xa - outerxsep;
      y = midPoint.y();
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFbase:
      anchorPos = basePoint;
      break;

    case PGFbaseeast:
      xa = halfwidth;
      xb = xa - outerxsep;
      x = centerPoint.x() + 0.333333 * xb + xa;
      y = basePoint.y();
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFbasewest:
      xa = halfside;
      x = centerPoint.x() + -1.16666 * xa - outerxsep;
      y = basePoint.y();
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFinput:
      {
        int i = (int)(state->angle);
        XW_TIKZ_SHAPE_LOGIC_GATE_IEC_AND_INPUT_ANCHOR(i);
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      break;

    case PGFoutput:
      xa = halfside;
      x = 1.33333 * xa + invertedradius + outerinvertedradius + outerxsep;
      y = 0;
      {
        QPointF p(x,y);
        anchorPos = centerPoint + p;
      }
      break;

    case PGFeast:
      xa = halfwidth;
      xb = xa - outerxsep;
      x = centerPoint.x() + 0.333333 * xb + xa;
      y = centerPoint.y();
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFnortheast:
      if (state->logicGateAnchorsUseBoundingBox)
      {
        xa = halfwidth;
        xb = xa - outerxsep;
        x = centerPoint.x() + 0.333333 * xb + xa;
        y = centerPoint.y() + halfheight;          
      }
      else
      {
        xa = halfwidth;
        ya = halfheight;
        xb = xa - outerxsep;
        x = centerPoint.x() + 0.333333 * xb + 0.707106 * xa;
        y = centerPoint.y() + 0.707106 * ya;
      }
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFsoutheast:
      if (state->logicGateAnchorsUseBoundingBox)
      {
        xa = halfwidth;
        xb = xa - outerxsep;
        x = centerPoint.x() + 0.333333 * xb + xa;
        y = centerPoint.y() - halfheight;          
      }
      else
      {
        xa = halfwidth;
        ya = halfheight;
        xb = xa - outerxsep;
        x = centerPoint.x() + 0.333333 * xb + 0.707106 * xa;
        y = centerPoint.y() - 0.707106 * ya;
      }
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFnorth:
      anchorPos.setX(centerPoint.x());
      anchorPos.setY(centerPoint.y() + halfheight);
      break;

    case PGFsouth:
      anchorPos.setX(centerPoint.x());
      anchorPos.setY(centerPoint.y() - halfheight);
      break;

    case PGFsouthwest:
      xa = halfside;
      x = centerPoint.x() - 1.16666 * xa - outerxsep;
      y = centerPoint.y() - halfheight;
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFnorthwest:
      xa = halfside;
      x = centerPoint.x() - 1.16666 * xa - outerxsep;
      y = centerPoint.y() + halfheight;
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFwest:
      xa = halfside;
      x = centerPoint.x() + -1.16666 * xa - outerxsep;
      y = centerPoint.y();
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;
  }

  if (dopath)
  {
    double xc = halfwidth - outerxsep;
    double yc = halfheight - outerysep;
    xb = 0.333333 * xc;
    state = state->save();
    state->shift(centerPoint.x(), centerPoint.y());
    state->moveTo(xb, yc);
    state->setStartAngle(0);
    state->setEndAngle(-90);
    state->setXRadius(yc);
    state->setYRadius(yc);
    state->addArc();
    state->lineTo(-1.16666 * xc, -yc);
    state->lineTo(-1.16666 * xc, yc);
    state->closePath();
    state = state->restore();

    double tempdima = 2 * yc;
    tempdima /= counta;
    for (int i = 0; i < numinputs; i++)
    {
      yc -= tempdima;
      if (state->inputs[i] == QChar('i'))
      {
        state = state->save();
        x = -1.16666 * halfside - 0.5 * state->lineWidth - invertedradius;
        y = yc;
        QPointF c(x, y);
        state->addCircle(c,invertedradius);
        state = state->restore();
      }
    }

    state = state->save();
    x = 1.33333 * xc + outerinvertedradius;
    y = 0;
    QPointF c(x,y);
    state->addCircle(c,invertedradius);
    state = state->restore();
  }
}

void XWTikzShape::norGateUSShape(bool dopath)
{
  XW_TIKZ_SHAPE_LOGIC_GATE_US_OR

  switch (state->anchor)
  {
    default:
      {
        x = state->anotherPoint.x() + centerPoint.x();
        y = state->anotherPoint.y() + centerPoint.y();
        QPointF externalpoint(x,y);
        xa = x;
        ya = y;
        xb = centerPoint.x();
        yb = centerPoint.y();
        double mathresult = state->angleBetweenPoints(centerPoint, externalpoint);
        double externalangle = mathresult;
        double xc = -0.166666 * halfside;
        if (xa < xc)
        {
          x = centerPoint.x() - 1.16666 * halfside - 1.732050 * outerxsep;
          y = centerPoint.y() + halfheight;
          QPointF northwest(x,y);
          mathresult = state->angleBetweenPoints(centerPoint, northwest);
          if (externalangle < mathresult)
          {
            x = centerPoint.x();
            y = centerPoint.y() + halfheight;
            QPointF north(x,y);
            anchorPos = state->intersectionOfLines(externalpoint,centerPoint,north,northwest);
          }
          else
          {
            mathresult = 360 - mathresult;
            if (externalangle > mathresult)
            {
              x = centerPoint.x();
              y = centerPoint.y() - halfheight;
              QPointF sourth(x,y);
              x = centerPoint.x() - 1.16666 * halfside - 1.732050 * outerxsep;
              y = centerPoint.y() - halfheight;
              QPointF sourthwest(x,y);
              anchorPos = state->intersectionOfLines(externalpoint,centerPoint,sourth,sourthwest);
            }
            else
            {
              if (ya > yb)
              {
                double yc = halfheight + halfside - outerxsep;
                x = centerPoint.x();
                xa = halfside;
                x -= 1.166666 * xa;
                xa *= 2;
                x -= 0.866025 * xa;
                x -= outerxsep;
                y = centerPoint.y();
                QPointF c(x,y);
                anchorPos = pointIntersectionOfLineAndArc(centerPoint,externalpoint,c,0,90,yc,yc);
              }
              else
              {
                double yc = halfheight + halfside - outerxsep;
                x = centerPoint.x();
                xa = halfside;
                x -= 1.166666 * xa;
                xa *= 2;
                x -= 0.866025 * xa;
                x -= outerxsep;
                y = centerPoint.y();
                QPointF c(x,y);
                anchorPos = pointIntersectionOfLineAndArc(centerPoint,externalpoint,c,270,360,yc,yc);
              }
            }
          }
        }
        else
        {
          if (y == 0)
            anchorPos = centerPoint + tipanchor;
          else
          {
            double xc = halfwidth + halfside;
            double yc = halfheight + halfside;
            xb = -0.166666 * halfside;
            if (ya < 0)
            {
              y = centerPoint.y() + halfside;
              x = centerPoint.x() + xb;
              QPointF c(x,y);
              anchorPos = pointIntersectionOfLineAndArc(externalpoint,centerPoint,c,270,330,yc,yc);
            }
            else
            {
              y = centerPoint.y() - halfside;
              x = centerPoint.x() + xb;
              QPointF c(x,y);
              anchorPos = pointIntersectionOfLineAndArc(externalpoint,centerPoint,c,30,90,xc,yc);
            }
          }
        }
      }
      break;

    case PGFcenter:
      anchorPos = centerPoint;
      break;

    case PGFmid:
      anchorPos = midPoint;
      break;

    case PGFmideast:
      x = centerPoint.x() + tipanchor.x();
      y = midPoint.y();
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFmidwest:
      x = centerPoint.x() - 1.16666 * halfside - 1.732050 * outerxsep;
      y = midPoint.y();
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFbase:
      anchorPos = basePoint;
      break;

    case PGFbaseeast:
      x = centerPoint.x() + tipanchor.x();
      y = 0;
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFbasewest:
      x = centerPoint.x() - 1.16666 * halfside - 1.732050 * outerxsep;
      y = 0;
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFeast:
    case PGFoutput:
      anchorPos = centerPoint + tipanchor;
      break;

    case PGFinput:
      {
        int i = (int)(state->angle);
        XW_TIKZ_SHAPE_LOGIC_GATE_IEC_OR_INPUT_ANCHOR(i);
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      break;

    case PGFnortheast:
      if (state->logicGateAnchorsUseBoundingBox)
      {
        x = centerPoint.x() + tipanchor.x();
        y = centerPoint.y() + halfheight;
      }
      else
      {
        xa = halfwidth;
        ya = halfheight;
        xb = halfside;
        x = centerPoint.x() - 0.166666 * xb;
        xa += xb;
        x += 0.5 * xa;
        y = centerPoint.y() - xb;
        ya += xb;
        y += 0.866025 * ya;
      }
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFsoutheast:
      if (state->logicGateAnchorsUseBoundingBox)
      {
        x = centerPoint.x() + tipanchor.x();
        y = centerPoint.y() - halfheight;
      }
      else
      {
        xa = halfwidth;
        ya = halfheight;
        xb = halfside;
        x = centerPoint.x() - 0.166666 * xb;
        xa += xb;
        x += 0.5 * xa;
        y = centerPoint.y() + xb;
        ya += xb;
        y -= 0.866025 * ya;
      }
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFnorth:
      if (state->logicGateAnchorsUseBoundingBox)
      {
        x = centerPoint.x();
        y = centerPoint.y() + halfheight;
      }
      else
      {
        x = centerPoint.x();
        y = centerPoint.y() + 0.993043 * halfheight;
      }
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFsouth:
      if (state->logicGateAnchorsUseBoundingBox)
      {
        x = centerPoint.x();
        y = centerPoint.y() - halfheight;
      }
      else
      {
        x = centerPoint.x();
        y = centerPoint.y() - 0.993043 * halfheight;
      }
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFsouthwest:
      x = centerPoint.x() - 1.16666 * halfside - 1.732050 * outerxsep;
      y = centerPoint.y() - halfheight;
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFnorthwest:
      x = centerPoint.x() - 1.16666 * halfside - 1.732050 * outerxsep;
      y = centerPoint.y() + halfheight;
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFwest:
      xa = halfside;
      y = centerPoint.y();
      if (state->logicGateAnchorsUseBoundingBox)
        x = centerPoint.x() - 1.16666 * xa - outerxsep;
      else
        x = centerPoint.x() - 1.16666 * xa - outerxsep + 0.267949 * xa;
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;
  }

  if (dopath)
  {
    double xc = halfwidth - outerxsep;
    double yc = halfheight - outerysep;
    state = state->save();
    state->shift(centerPoint.x(), centerPoint.y());
    state->moveTo(-0.16666 * xc, yc);
    yc *= 2;
    state->setStartAngle(90);
    state->setEndAngle(30);
    state->setXRadius(yc);
    state->setYRadius(yc);
    state->addArc();
    state->lineTo(-1.16666 * xc, -yc);
    yc *= 2;
    state->setStartAngle(-30);
    state->setEndAngle(-90);
    state->setXRadius(yc);
    state->setYRadius(yc);
    state->addArc();
    state->setStartAngle(-30);
    state->setEndAngle(0);
    state->setXRadius(yc);
    state->setYRadius(yc);
    state->addArc();
    state->setStartAngle(0);
    state->setEndAngle(30);
    state->setXRadius(yc);
    state->setYRadius(yc);
    state->addArc();
    state->closePath();
    state = state->restore();

    state = state->save();
    x = -0.166666 * xc;
    yc *= 2;
    x += 0.866025 * yc;
    x += outerinvertedradius;
    y = 0;
    QPointF b(x,y);
    state->addCircle(b,invertedradius);
    state = state->restore();

    double tempdima = 2 * yc;
    tempdima /= counta;
    for (int i = 0; i < numinputs; i++)
    {
      yc -= tempdima;
      if (state->inputs[i] == QChar('i'))
      {
        ya = halfside;
        yb = 2 * ya;
        xa = yb;
        yb -= halflinewidth;
        double mathresult = yc / yb;
        mathresult = asin(mathresult);
        mathresult = cos(mathresult);
        x = -1.166666 * ya - 0.866025 * xa + mathresult * yb - invertedradius;
        y = yc;
        QPointF c(x,y);
        state = state->save();
        state->addCircle(c,invertedradius);
        state = state->restore();
      }
    }
  }
}

void XWTikzShape::notGateUSShape(bool dopath)
{
  XW_TIKZ_SHAPE_LOGIC_GATE_US_OR

  switch (state->anchor)
  {
    default:
      {
        x = state->anotherPoint.x() + centerPoint.x();
        y = state->anotherPoint.y() + centerPoint.y();
        QPointF externalpoint(x,y);
        double mathresult = state->angleBetweenPoints(centerPoint, externalpoint);
        if (mathresult == 0.0)
        {
          double xc = halfside;
          x = centerPoint.x() - 0.166666 * xc;
          xc *= 2;
          x += 0.866025 * xc;
          x += invertedradius;
          x += outerinvertedradius;
          x += outerxsep;
          y = centerPoint.y();
          anchorPos.setX(x);
          anchorPos.setY(y);
        }
        else
        {
          if (mathresult == 180)
          {
            XW_TIKZ_SHAPE_LOGIC_GATE_IEC_AND_INPUT_ANCHOR(1);
            double xc = halfside;
            x += 0.333333 * xc;
            anchorPos.setX(x);
            anchorPos.setY(y);
          }
          else
          {
            double externalangle = mathresult;
            xa = halfside;
            x = centerPoint.x() - 0.833333 * xa - outerxsep;
            y = centerPoint.y() + 1.166666 * xa;
            ya = outerysep;
            y += 1.565 * ya;
            QPointF northwest(x,y);
            mathresult = state->angleBetweenPoints(centerPoint, northwest);
            if (externalangle < mathresult)
            {
              x = centerPoint.x() + tipanchor.x() + 2.350943 * outerxsep;
              y = centerPoint.y() + tipanchor.y();
              QPointF p(x,y);
              anchorPos = state->intersectionOfLines(centerPoint, externalpoint,northwest,p);
            }
            else
            {
              xa = halfside;
              x = centerPoint.x() - 0.833333 * xa - outerxsep;
              y = centerPoint.y() - 1.166666 * xa;
              ya = outerysep;
              y -= 1.565 * ya;
              QPointF southwest(x,y);
              mathresult = state->angleBetweenPoints(centerPoint, southwest);
              if (externalangle < mathresult)
                anchorPos = state->intersectionOfLines(externalpoint,centerPoint, northwest,southwest);
              else
              {
                x = centerPoint.x() + tipanchor.x() + 2.350943 * outerxsep;
                y = centerPoint.y() + tipanchor.y();
                QPointF p(x,y);
                anchorPos = state->intersectionOfLines(centerPoint, externalpoint,southwest,p);
              }
            }
          }
        }
      }
      break;

    case PGFcenter:
      anchorPos = centerPoint;
      break;

    case PGFmid:
      anchorPos = midPoint;
      break;

    case PGFmideast:
      x = tipanchor.x() + centerPoint.x();
      y = midPoint.y();
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFmidwest:
      {
        double xc = -0.833333 * halfside - outerxsep;
        x = midPoint.x() + xc;
        y = midPoint.y();
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      break;

    case PGFbase:
      anchorPos = basePoint;
      break;

    case PGFbaseeast:
      x = tipanchor.x() + centerPoint.x();
      y = basePoint.y();
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFbasewest:
      {
        double xc = -0.833333 * halfside - outerxsep;
        x = midPoint.x() + xc;
        y = basePoint.y();
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      break;

    case PGFoutput:
      {
        double xc = halfside;
        x = centerPoint.x() - 0.166666 * xc;
        xc *= 2;
        x += 0.866025 * xc;
        x += invertedradius;
        x += outerinvertedradius;
        x += outerxsep;
        y = centerPoint.y();
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      break;

    case PGFeast:
      anchorPos.setX(tipanchor.x() + centerPoint.x());
      anchorPos.setY(centerPoint.y());
      break;

    case PGFnorth:
      if (state->logicGateAnchorsUseBoundingBox)
      {
        x = centerPoint.x();
        xa = halfside;
        y = centerPoint.y() + 1.166666 * xa;
        ya = outerysep;
        y += 1.565 * ya;
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      else
      {
        x = centerPoint.x();
        y = centerPoint.y() + 1;
        QPointF p1(x,y);
        xa = halfside;
        x = centerPoint.x() - 0.833333 * xa - outerxsep;
        y = centerPoint.y() + 1.166666 * xa;
        ya = outerysep;
        y += 1.565 * ya;
        QPointF northwest(x,y);
        x = centerPoint.x() + tipanchor.x() + 2.350943 * outerxsep;
        y = centerPoint.y() + tipanchor.y();
        QPointF p2(x,y);
        anchorPos = state->intersectionOfLines(centerPoint,p1,northwest,p2);
      }
      break;

    case PGFsouth:
      if (state->logicGateAnchorsUseBoundingBox)
      {
        x = centerPoint.x();
        xa = halfside;
        y = centerPoint.y() - 1.166666 * xa;
        ya = outerysep;
        y -= 1.565 * ya;
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      else
      {
        x = centerPoint.x();
        y = centerPoint.y() - 1;
        QPointF p1(x,y);
        xa = halfside;
        y = centerPoint.y() - 1.166666 * xa;
        ya = outerysep;
        y -= 1.565 * ya;
        QPointF southwest(x,y);
        x = centerPoint.x() + tipanchor.x() + 2.350943 * outerxsep;
        y = centerPoint.y() + tipanchor.y();
        QPointF p2(x,y);
        anchorPos = state->intersectionOfLines(centerPoint,p1,southwest,p2);
      }
      break;

    case PGFsoutheast:
      if (state->logicGateAnchorsUseBoundingBox)
      {
        xa = halfside;
        y = centerPoint.y() - 1.166666 * xa;
        ya = outerysep;
        y -= 1.565 * ya;
        x = tipanchor.x() + centerPoint.x();
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      else
      {
        x = centerPoint.x() + 1;
        y = centerPoint.y() + 1;
        QPointF p1(x,y);
        xa = halfside;
        x = centerPoint.x() - 0.833333 * xa - outerxsep;
        y = centerPoint.y() - 1.166666 * xa;
        ya = outerysep;
        y -= 1.565 * ya;
        QPointF southwest(x,y);
        x = centerPoint.x() + tipanchor.x() + 2.350943 * outerxsep;
        y = centerPoint.y() + tipanchor.y();
        QPointF p2(x,y);
        anchorPos = state->intersectionOfLines(centerPoint,p1,southwest,p2);
      }
      break;

    case PGFnortheast:
      if (state->logicGateAnchorsUseBoundingBox)
      {
        xa = halfside;
        y = centerPoint.y() + 1.166666 * xa;
        ya = outerysep;
        y += 1.565 * ya;
        x = tipanchor.x() + centerPoint.x();
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      else
      {
        x = centerPoint.x() + 1;
        y = centerPoint.y() + 1;
        QPointF p1(x,y);
        xa = halfside;
        x = centerPoint.x() - 0.833333 * xa - outerxsep;
        y = centerPoint.y() + 1.166666 * xa;
        ya = outerysep;
        y += 1.565 * ya;
        QPointF northwest(x,y);
        x = centerPoint.x() + tipanchor.x() + 2.350943 * outerxsep;
        y = centerPoint.y() + tipanchor.y();
        QPointF p2(x,y);
        anchorPos = state->intersectionOfLines(centerPoint,p1,northwest,p2);
      }
      break;

    case PGFsouthwest:
      xa = halfside;
      x = centerPoint.x() - 0.833333 * xa - outerxsep;
      y = centerPoint.y() - 1.166666 * xa;
      ya = outerysep;
      y -= 1.565 * ya;
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFnorthwest:
      xa = halfside;
      x = centerPoint.x() - 0.833333 * xa - outerxsep;
      y = centerPoint.y() + 1.166666 * xa;
      ya = outerysep;
      y += 1.565 * ya;
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFinput:
      {
        XW_TIKZ_SHAPE_LOGIC_GATE_IEC_AND_INPUT_ANCHOR(1);
        double xc = halfside;
        x += 0.333333 * xc;
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      break;

    case PGFwest:
      {
        double xc = -0.833333 * halfside;
        x = centerPoint.x() + xc;
        y = centerPoint.y();
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      break;
  }

  if (dopath)
  {
    double xc = halfwidth - outerxsep;
    double yc = halfheight - outerysep;
    state = state->save();
    state->shift(centerPoint.x(), centerPoint.y());
    state->moveTo(tipanchor);
    state->lineTo(-0.833333 * xc, 1.166666 * yc);
    state->lineTo(-0.833333 * xc, -1.166666 * yc);
    state->lineTo(tipanchor);
    state = state->restore();

    x = -0.166666 * xc;
    yc *= 2;
    x += 0.866025 * yc;
    x += outerinvertedradius;
    y = 0;
    QPointF b(x,y);
    state = state->save();
    state->addCircle(b,invertedradius);
    state = state->restore();

    if (state->inputs[0] == QChar('i'))
    {
      xa = halfside;
      x = -0.833333 * xa - 0.5 * state->lineWidth - invertedradius;
      y = 0;
      QPointF c(x,y);
      state = state->save();
      state->addCircle(c,invertedradius);
      state = state->restore();
    }
  }
}

void XWTikzShape::orGateUSShape(bool dopath)
{
  XW_TIKZ_SHAPE_LOGIC_GATE_US_OR

  switch (state->anchor)
  {
    default:
      {
        x = state->anotherPoint.x() + centerPoint.x();
        y = state->anotherPoint.y() + centerPoint.y();
        QPointF externalpoint(x,y);
        xa = x;
        ya = y;
        xb = centerPoint.x();
        yb = centerPoint.y();
        double mathresult = state->angleBetweenPoints(centerPoint, externalpoint);
        double externalangle = mathresult;
        double xc = -0.166666 * halfside;
        if (xa < xc)
        {
          x = centerPoint.x() - 1.16666 * halfside - 1.732050 * outerxsep;
          y = centerPoint.y() + halfheight;
          QPointF northwest(x,y);
          mathresult = state->angleBetweenPoints(centerPoint, northwest);
          if (externalangle < mathresult)
          {
            x = centerPoint.x();
            y = centerPoint.y() + halfheight;
            QPointF north(x,y);
            anchorPos = state->intersectionOfLines(externalpoint,centerPoint,north,northwest);
          }
          else
          {
            mathresult = 360 - mathresult;
            if (externalangle > mathresult)
            {
              x = centerPoint.x();
              y = centerPoint.y() - halfheight;
              QPointF sourth(x,y);
              x = centerPoint.x() - 1.16666 * halfside - 1.732050 * outerxsep;
              y = centerPoint.y() - halfheight;
              QPointF sourthwest(x,y);
              anchorPos = state->intersectionOfLines(externalpoint,centerPoint,sourth,sourthwest);
            }
            else
            {
              if (ya > yb)
              {
                double yc = halfheight + halfside - outerxsep;
                x = centerPoint.x();
                xa = halfside;
                x -= 1.166666 * xa;
                xa *= 2;
                x -= 0.866025 * xa;
                x -= outerxsep;
                y = centerPoint.y();
                QPointF c(x,y);
                anchorPos = pointIntersectionOfLineAndArc(centerPoint,externalpoint,c,0,90,yc,yc);
              }
              else
              {
                double yc = halfheight + halfside - outerxsep;
                x = centerPoint.x();
                xa = halfside;
                x -= 1.166666 * xa;
                xa *= 2;
                x -= 0.866025 * xa;
                x -= outerxsep;
                y = centerPoint.y();
                QPointF c(x,y);
                anchorPos = pointIntersectionOfLineAndArc(centerPoint,externalpoint,c,270,360,yc,yc);
              }
            }
          }
        }
        else
        {
          if (y == 0)
            anchorPos = centerPoint + tipanchor;
          else
          {
            double xc = halfwidth + halfside;
            double yc = halfheight + halfside;
            xb = -0.166666 * halfside;
            if (ya < 0)
            {
              y = centerPoint.y() + halfside;
              x = centerPoint.x() + xb;
              QPointF c(x,y);
              anchorPos = pointIntersectionOfLineAndArc(externalpoint,centerPoint,c,270,330,yc,yc);
            }
            else
            {
              y = centerPoint.y() - halfside;
              x = centerPoint.x() + xb;
              QPointF c(x,y);
              anchorPos = pointIntersectionOfLineAndArc(externalpoint,centerPoint,c,30,90,xc,yc);
            }
          }
        }
      }
      break;

    case PGFcenter:
      anchorPos = centerPoint;
      break;

    case PGFmid:
      anchorPos = midPoint;
      break;

    case PGFmideast:
      x = centerPoint.x() + tipanchor.x();
      y = midPoint.y();
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFmidwest:
      x = centerPoint.x() - 1.16666 * halfside - 1.732050 * outerxsep;
      y = midPoint.y();
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFbase:
      anchorPos = basePoint;
      break;

    case PGFbaseeast:
      x = centerPoint.x() + tipanchor.x();
      y = 0;
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFbasewest:
      x = centerPoint.x() - 1.16666 * halfside - 1.732050 * outerxsep;
      y = 0;
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFeast:
    case PGFoutput:
      anchorPos = centerPoint + tipanchor;
      break;

    case PGFinput:
      {
        int i = (int)(state->angle);
        XW_TIKZ_SHAPE_LOGIC_GATE_IEC_OR_INPUT_ANCHOR(i);
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      break;

    case PGFnortheast:
      if (state->logicGateAnchorsUseBoundingBox)
      {
        x = centerPoint.x() + tipanchor.x();
        y = centerPoint.y() + halfheight;
      }
      else
      {
        xa = halfwidth;
        ya = halfheight;
        xb = halfside;
        x = centerPoint.x() - 0.166666 * xb;
        xa += xb;
        x += 0.5 * xa;
        y = centerPoint.y() - xb;
        ya += xb;
        y += 0.866025 * ya;
      }
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFsoutheast:
      if (state->logicGateAnchorsUseBoundingBox)
      {
        x = centerPoint.x() + tipanchor.x();
        y = centerPoint.y() - halfheight;
      }
      else
      {
        xa = halfwidth;
        ya = halfheight;
        xb = halfside;
        x = centerPoint.x() - 0.166666 * xb;
        xa += xb;
        x += 0.5 * xa;
        y = centerPoint.y() + xb;
        ya += xb;
        y -= 0.866025 * ya;
      }
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFnorth:
      if (state->logicGateAnchorsUseBoundingBox)
      {
        x = centerPoint.x();
        y = centerPoint.y() + halfheight;
      }
      else
      {
        x = centerPoint.x();
        y = centerPoint.y() + 0.993043 * halfheight;
      }
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFsouth:
      if (state->logicGateAnchorsUseBoundingBox)
      {
        x = centerPoint.x();
        y = centerPoint.y() - halfheight;
      }
      else
      {
        x = centerPoint.x();
        y = centerPoint.y() - 0.993043 * halfheight;
      }
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFsouthwest:
      x = centerPoint.x() - 1.16666 * halfside - 1.732050 * outerxsep;
      y = centerPoint.y() - halfheight;
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFnorthwest:
      x = centerPoint.x() - 1.16666 * halfside - 1.732050 * outerxsep;
      y = centerPoint.y() + halfheight;
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFwest:
      xa = halfside;
      y = centerPoint.y();
      if (state->logicGateAnchorsUseBoundingBox)
        x = centerPoint.x() - 1.16666 * xa - outerxsep;
      else
        x = centerPoint.x() - 1.16666 * xa - outerxsep + 0.267949 * xa;
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;
  }

  if (dopath)
  {
    double xc = halfwidth - outerxsep;
    double yc = halfheight - outerysep;
    state = state->save();
    state->shift(centerPoint.x(), centerPoint.y());
    state->moveTo(-0.16666 * xc, yc);
    yc *= 2;
    state->setStartAngle(90);
    state->setEndAngle(30);
    state->setXRadius(yc);
    state->setYRadius(yc);
    state->addArc();
    state->lineTo(-1.16666 * xc, -yc);
    yc *= 2;
    state->setStartAngle(-30);
    state->setEndAngle(-90);
    state->setXRadius(yc);
    state->setYRadius(yc);
    state->addArc();
    state->setStartAngle(-30);
    state->setEndAngle(0);
    state->setXRadius(yc);
    state->setYRadius(yc);
    state->addArc();
    state->setStartAngle(0);
    state->setEndAngle(30);
    state->setXRadius(yc);
    state->setYRadius(yc);
    state->addArc();
    state->closePath();
    state = state->restore();

    double tempdima = 2 * yc;
    tempdima /= counta;
    for (int i = 0; i < numinputs; i++)
    {
      yc -= tempdima;
      if (state->inputs[i] == QChar('i'))
      {
        ya = halfside;
        yb = 2 * ya;
        xa = yb;
        yb -= halflinewidth;
        double mathresult = yc / yb;
        mathresult = asin(mathresult);
        mathresult = cos(mathresult);
        x = -1.166666 * ya - 0.866025 * xa + mathresult * yb - invertedradius;
        y = yc;
        QPointF c(x,y);
        state = state->save();
        state->addCircle(c,invertedradius);
        state = state->restore();
      }
    }
  }
}

void XWTikzShape::xorGateUSShape(bool dopath)
{
  XW_TIKZ_SHAPE_LOGIC_GATE_US_OR

  switch (state->anchor)
  {
    default:
      {
        x = state->anotherPoint.x() + centerPoint.x();
        y = state->anotherPoint.y() + centerPoint.y();
        QPointF externalpoint(x,y);
        xa = x;
        ya = y;
        xb = centerPoint.x();
        yb = centerPoint.y();
        double mathresult = state->angleBetweenPoints(centerPoint, externalpoint);
        double externalangle = mathresult;
        double xc = -0.166666 * halfside;
        if (xa < xc)
        {
          xa = -3.232051 * halfside;
          xb = 2 * halfside - halflinewidth;
          xa += 0.866025 * xb;
          ya = 0.5 * xb;
          x = centerPoint.x() + xa;
          y = centerPoint.y() + halfheight;
          QPointF northwest(x,y);
          mathresult = state->angleBetweenPoints(centerPoint, northwest);
          if (externalangle < mathresult)
          {
            x = centerPoint.x();
            y = centerPoint.y() + halfheight;
            QPointF north(x,y);
            anchorPos = state->intersectionOfLines(externalpoint,centerPoint,north,northwest);
          }
          else
          {
            mathresult = 360 - mathresult;
            if (externalangle > mathresult)
            {
              x = centerPoint.x();
              y = centerPoint.y() - halfheight;
              QPointF south(x,y);
              xa = -3.232051 * halfside;
              xb = 2 * halfside - halflinewidth;
              xa += 0.866025 * xb;
              ya = 0.5 * xb;
              x = centerPoint.x() + xa;
              y = centerPoint.y() - halfheight;
              QPointF southwest(x,y);
              anchorPos = state->intersectionOfLines(externalpoint,centerPoint,south,southwest);
            }
            else
            {
              if (ya > yb)
              {
                double yc = halfheight + halfside - outerxsep;
                xa = halfside;
                x = centerPoint.x() - 1.166666 * xa;
                xa *= 2;
                x -= 0.866025 * xa - outerxsep - 0.166666 * xa;
                y = centerPoint.y();
                QPointF c(x,y);
                anchorPos = pointIntersectionOfLineAndArc(centerPoint,externalpoint,c,0,90,yc,yc);
              }
              else
              {
                double yc = halfheight + halfside - outerxsep;
                xa = halfside;
                x = centerPoint.x() - 1.166666 * xa;
                xa *= 2;
                x -= 0.866025 * xa - outerxsep - 0.166666 * xa;
                y = centerPoint.y();
                QPointF c(x,y);
                anchorPos = pointIntersectionOfLineAndArc(centerPoint,externalpoint,c,270,360,yc,yc);
              }
            }
          }
        }
        else
        {
          if (y == 0)
            anchorPos = centerPoint + tipanchor;
          else
          {
            double xc = halfwidth + halfside;
            double yc = halfheight + halfside;
            xb = -0.166666 * halfside;
            if (ya < 0)
            {
              x = centerPoint.x() + xb;
              y = centerPoint.y() + halfside;
              QPointF c(x,y);
              anchorPos = pointIntersectionOfLineAndArc(externalpoint,centerPoint,c,270,330,yc,yc);
            }
            else
            {
              x = centerPoint.x() + xb;
              y = centerPoint.y() - halfside;
              QPointF c(x,y);
              anchorPos = pointIntersectionOfLineAndArc(externalpoint,centerPoint,c,30,90,xc,yc);
            }
          }
        }
      }
      break;

    case PGFcenter:
      anchorPos = centerPoint;
      break;

    case PGFmid:
      anchorPos = midPoint;
      break;

    case PGFmideast:
      x = centerPoint.x() + tipanchor.x();
      y = midPoint.y();
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFmidwest:
      xa = -3.232051 * halfside;
      xb = 2 * halfside - halflinewidth;
      xa += 0.866025 * xb;
      ya = 0.5 * xb;
      x = centerPoint.x() + xa;
      y = midPoint.y();
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFbase:
      anchorPos = basePoint;
      break;

    case PGFeast:
    case PGFoutput:
      anchorPos = centerPoint + tipanchor;
      break;

    case PGFinput:
      {
        int i = (int)(state->angle);
        XW_TIKZ_SHAPE_LOGIC_GATE_IEC_XOR_INPUT_ANCHOR(i);
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      break;

    case PGFnortheast:
      if (state->logicGateAnchorsUseBoundingBox)
      {
        x = centerPoint.x() + tipanchor.x();
        y = centerPoint.y() + halfheight;
      }
      else
      {
        xa = halfwidth;
        ya = halfheight;
        xb = halfside;
        x = centerPoint.x() - 0.166666 * xb;
        xa += xb;
        x += 0.5 * xa;
        y = centerPoint.y() - xb;
        ya += xb;
        y += 0.866025 * ya;
      }
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFsoutheast:
      if (state->logicGateAnchorsUseBoundingBox)
      {
        x = centerPoint.x() + tipanchor.x();
        y = centerPoint.y() - halfheight;
      }
      else
      {
        xa = halfwidth;
        ya = halfheight;
        xb = halfside;
        x = centerPoint.x() - 0.166666 * xb;
        xa += xb;
        x += 0.5 * xa;
        y = centerPoint.y() + xb;
        ya += xb;
        y -= 0.866025 * ya;
      }
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFnorth:
      if (state->logicGateAnchorsUseBoundingBox)
      {
        x = centerPoint.x();
        y = centerPoint.y() + halfheight;
      }
      else
      {
        x = centerPoint.x();
        y = centerPoint.y() + 0.993043 * halfheight;
      }
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFsouth:
      if (state->logicGateAnchorsUseBoundingBox)
      {
        x = centerPoint.x();
        y = centerPoint.y() - halfheight;
      }
      else
      {
        x = centerPoint.x();
        y = centerPoint.y() - 0.993043 * halfheight;
      }
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFsouthwest:
      xa = -3.232051 * halfside;
      xb = 2 * halfside - halflinewidth;
      xa += 0.866025 * xb;
      ya = 0.5 * xb;
      x = centerPoint.x() + xa;
      if (state->logicGateAnchorsUseBoundingBox)
        y = centerPoint.y() - halfheight;
      else
        y = centerPoint.y() - ya;
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFnorthwest:
      xa = -3.232051 * halfside;
      xb = 2 * halfside - halflinewidth;
      xa += 0.866025 * xb;
      ya = 0.5 * xb;
      x = centerPoint.x() + xa;
      if (state->logicGateAnchorsUseBoundingBox)
        y = centerPoint.y() + halfheight;
      else
        y = centerPoint.y() + ya;
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFwest:
      ya = halfside;
      yb = 2 * ya;
      xb = yb;
      yb -= halflinewidth;
      {
        double mathresult = 0 / yb;
        mathresult = asin(mathresult);
        mathresult = cos(mathresult);
        double xc = -1.166666 * ya - 0.866025 * xb + mathresult * yb + halflinewidth - outerxsep;
        x = centerPoint.x() + xc;
        xa = halfside;
        x -= 0.333333 * xa;
        if (state->logicGateAnchorsUseBoundingBox)
        {
          xa = 2 * xa;
          x -= 0.133974 * xa;
        }
      } 
      y = centerPoint.y();
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;
  }

  if (dopath)
  {
    double xc = halfwidth - outerxsep;
    double yc = halfheight - outerysep;
    state = state->save();
    state->shift(centerPoint.x(), centerPoint.y());
    state->moveTo(-0.16666 * xc, yc);
    yc = 2 * yc;
    state->setStartAngle(90);
    state->setEndAngle(30);
    state->setXRadius(yc);
    state->setYRadius(yc);
    state->addArc();
    yc = 2 * yc;
    state->setStartAngle(-30);
    state->setEndAngle(-90);
    state->setXRadius(yc);
    state->setYRadius(yc);
    state->addArc();
    state->lineTo(-1.16666 * xc, -yc);
    yc = 2.66666 * yc;
    state->setStartAngle(-22);
    state->setEndAngle(0);
    state->setXRadius(1.166666 * yc);
    state->setYRadius(yc);
    state->addArc();
    yc = 2.66666 * yc;
    state->setStartAngle(0);
    state->setEndAngle(22);
    state->setXRadius(1.166666 * yc);
    state->setYRadius(yc);
    state->addArc();
    state->closePath();
    state = state->restore();

    double tempdima = 2 * yc;
    tempdima /= counta;
    for (int i = 0; i < numinputs; i++)
    {
      yc -= tempdima;
      if (state->inputs[i] == QChar('i'))
      {
        ya = halfside;
        yb = 2 * ya;
        xa = yb;
        yb -= halflinewidth;
        double mathresult = yc / yb;
        mathresult = asin(mathresult);
        mathresult = cos(mathresult);
        x = -1.5 * ya - 0.866025 * xa + mathresult * yb - invertedradius;
        y = yc;
        QPointF c(x,y);
        state = state->save();
        state->addCircle(c,invertedradius);
        state = state->restore();
      }
    }

    xc = halfside;
    yc = halfside;
    state = state->save();
    state->moveTo(-1.5 * xc, -yc);
    yc *= 2;
    for (int i = 0; i < 60; i++)
    {
      x = -3.232051 * halfside + yc * cos(double(i - 29));
      y = yc * sin(double(i - 29));
      state->lineTo(x,y);
      state->moveTo(x,y);
    }
    state = state->restore();
  }
}

void XWTikzShape::xnorGateUSShape(bool dopath)
{
  XW_TIKZ_SHAPE_LOGIC_GATE_US_OR

  switch (state->anchor)
  {
    default:
      {
        x = state->anotherPoint.x() + centerPoint.x();
        y = state->anotherPoint.y() + centerPoint.y();
        QPointF externalpoint(x,y);
        xa = x;
        ya = y;
        xb = centerPoint.x();
        yb = centerPoint.y();
        double mathresult = state->angleBetweenPoints(centerPoint, externalpoint);
        double externalangle = mathresult;
        double xc = -0.166666 * halfside;
        if (xa < xc)
        {
          xa = -3.232051 * halfside;
          xb = 2 * halfside - halflinewidth;
          xa += 0.866025 * xb;
          ya = 0.5 * xb;
          x = centerPoint.x() + xa;
          y = centerPoint.y() + halfheight;
          QPointF northwest(x,y);
          mathresult = state->angleBetweenPoints(centerPoint, northwest);
          if (externalangle < mathresult)
          {
            x = centerPoint.x();
            y = centerPoint.y() + halfheight;
            QPointF north(x,y);
            anchorPos = state->intersectionOfLines(externalpoint,centerPoint,north,northwest);
          }
          else
          {
            mathresult = 360 - mathresult;
            if (externalangle > mathresult)
            {
              x = centerPoint.x();
              y = centerPoint.y() - halfheight;
              QPointF south(x,y);
              xa = -3.232051 * halfside;
              xb = 2 * halfside - halflinewidth;
              xa += 0.866025 * xb;
              ya = 0.5 * xb;
              x = centerPoint.x() + xa;
              y = centerPoint.y() - halfheight;
              QPointF southwest(x,y);
              anchorPos = state->intersectionOfLines(externalpoint,centerPoint,south,southwest);
            }
            else
            {
              if (ya > yb)
              {
                double yc = halfheight + halfside - outerxsep;
                xa = halfside;
                x = centerPoint.x() - 1.166666 * xa;
                xa *= 2;
                x -= 0.866025 * xa - outerxsep - 0.166666 * xa;
                y = centerPoint.y();
                QPointF c(x,y);
                anchorPos = pointIntersectionOfLineAndArc(centerPoint,externalpoint,c,0,90,yc,yc);
              }
              else
              {
                double yc = halfheight + halfside - outerxsep;
                xa = halfside;
                x = centerPoint.x() - 1.166666 * xa;
                xa *= 2;
                x -= 0.866025 * xa - outerxsep - 0.166666 * xa;
                y = centerPoint.y();
                QPointF c(x,y);
                anchorPos = pointIntersectionOfLineAndArc(centerPoint,externalpoint,c,270,360,yc,yc);
              }
            }
          }
        }
        else
        {
          if (y == 0)
            anchorPos = centerPoint + tipanchor;
          else
          {
            double xc = halfwidth + halfside;
            double yc = halfheight + halfside;
            xb = -0.166666 * halfside;
            if (ya < 0)
            {
              x = centerPoint.x() + xb;
              y = centerPoint.y() + halfside;
              QPointF c(x,y);
              anchorPos = pointIntersectionOfLineAndArc(externalpoint,centerPoint,c,270,330,yc,yc);
            }
            else
            {
              x = centerPoint.x() + xb;
              y = centerPoint.y() - halfside;
              QPointF c(x,y);
              anchorPos = pointIntersectionOfLineAndArc(externalpoint,centerPoint,c,30,90,xc,yc);
            }
          }
        }
      }
      break;

    case PGFcenter:
      anchorPos = centerPoint;
      break;

    case PGFmid:
      anchorPos = midPoint;
      break;

    case PGFmideast:
      x = centerPoint.x() + tipanchor.x();
      y = midPoint.y();
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFmidwest:
      xa = -3.232051 * halfside;
      xb = 2 * halfside - halflinewidth;
      xa += 0.866025 * xb;
      ya = 0.5 * xb;
      x = centerPoint.x() + xa;
      y = midPoint.y();
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFbase:
      anchorPos = basePoint;
      break;

    case PGFeast:
    case PGFoutput:
      anchorPos = centerPoint + tipanchor;
      break;

    case PGFinput:
      {
        int i = (int)(state->angle);
        XW_TIKZ_SHAPE_LOGIC_GATE_IEC_XOR_INPUT_ANCHOR(i);
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      break;

    case PGFnortheast:
      if (state->logicGateAnchorsUseBoundingBox)
      {
        x = centerPoint.x() + tipanchor.x();
        y = centerPoint.y() + halfheight;
      }
      else
      {
        xa = halfwidth;
        ya = halfheight;
        xb = halfside;
        x = centerPoint.x() - 0.166666 * xb;
        xa += xb;
        x += 0.5 * xa;
        y = centerPoint.y() - xb;
        ya += xb;
        y += 0.866025 * ya;
      }
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFsoutheast:
      if (state->logicGateAnchorsUseBoundingBox)
      {
        x = centerPoint.x() + tipanchor.x();
        y = centerPoint.y() - halfheight;
      }
      else
      {
        xa = halfwidth;
        ya = halfheight;
        xb = halfside;
        x = centerPoint.x() - 0.166666 * xb;
        xa += xb;
        x += 0.5 * xa;
        y = centerPoint.y() + xb;
        ya += xb;
        y -= 0.866025 * ya;
      }
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFnorth:
      if (state->logicGateAnchorsUseBoundingBox)
      {
        x = centerPoint.x();
        y = centerPoint.y() + halfheight;
      }
      else
      {
        x = centerPoint.x();
        y = centerPoint.y() + 0.993043 * halfheight;
      }
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFsouth:
      if (state->logicGateAnchorsUseBoundingBox)
      {
        x = centerPoint.x();
        y = centerPoint.y() - halfheight;
      }
      else
      {
        x = centerPoint.x();
        y = centerPoint.y() - 0.993043 * halfheight;
      }
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFsouthwest:
      xa = -3.232051 * halfside;
      xb = 2 * halfside - halflinewidth;
      xa += 0.866025 * xb;
      ya = 0.5 * xb;
      x = centerPoint.x() + xa;
      if (state->logicGateAnchorsUseBoundingBox)
        y = centerPoint.y() - halfheight;
      else
        y = centerPoint.y() - ya;
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFnorthwest:
      xa = -3.232051 * halfside;
      xb = 2 * halfside - halflinewidth;
      xa += 0.866025 * xb;
      ya = 0.5 * xb;
      x = centerPoint.x() + xa;
      if (state->logicGateAnchorsUseBoundingBox)
        y = centerPoint.y() + halfheight;
      else
        y = centerPoint.y() + ya;
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFwest:
      ya = halfside;
      yb = 2 * ya;
      xb = yb;
      yb -= halflinewidth;
      {
        double mathresult = 0 / yb;
        mathresult = asin(mathresult);
        mathresult = cos(mathresult);
        double xc = -1.166666 * ya - 0.866025 * xb + mathresult * yb + halflinewidth - outerxsep;
        x = centerPoint.x() + xc;
        xa = halfside;
        x -= 0.333333 * xa;
        if (state->logicGateAnchorsUseBoundingBox)
        {
          xa = 2 * xa;
          x -= 0.133974 * xa;
        }
      } 
      y = centerPoint.y();
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;
  }

  if (dopath)
  {
    double xc = halfwidth - outerxsep;
    double yc = halfheight - outerysep;
    state = state->save();
    state->shift(centerPoint.x(), centerPoint.y());
    state->moveTo(-0.16666 * xc, yc);
    yc = 2 * yc;
    state->setStartAngle(90);
    state->setEndAngle(30);
    state->setXRadius(yc);
    state->setYRadius(yc);
    state->addArc();
    yc = 2 * yc;
    state->setStartAngle(-30);
    state->setEndAngle(-90);
    state->setXRadius(yc);
    state->setYRadius(yc);
    state->addArc();
    state->lineTo(-1.16666 * xc, -yc);
    yc = 2.66666 * yc;
    state->setStartAngle(-22);
    state->setEndAngle(0);
    state->setXRadius(1.166666 * yc);
    state->setYRadius(yc);
    state->addArc();
    yc = 2.66666 * yc;
    state->setStartAngle(0);
    state->setEndAngle(22);
    state->setXRadius(1.166666 * yc);
    state->setYRadius(yc);
    state->addArc();
    state->closePath();
    state = state->restore();

    double tempdima = 2 * yc;
    tempdima /= counta;
    for (int i = 0; i < numinputs; i++)
    {
      yc -= tempdima;
      if (state->inputs[i] == QChar('i'))
      {
        ya = halfside;
        yb = 2 * ya;
        xa = yb;
        yb -= halflinewidth;
        double mathresult = yc / yb;
        mathresult = asin(mathresult);
        mathresult = cos(mathresult);
        x = -1.5 * ya - 0.866025 * xa + mathresult * yb - invertedradius;
        y = yc;
        QPointF c(x,y);
        state = state->save();
        state->addCircle(c,invertedradius);
        state = state->restore();
      }
    }

    xc = halfside;
    yc = halfside;
    state = state->save();
    state->moveTo(-1.5 * xc, -yc);
    yc *= 2;
    for (int i = 0; i < 60; i++)
    {
      x = -3.232051 * halfside + yc * cos(double(i - 29));
      y = yc * sin(double(i - 29));
      state->lineTo(x,y);
      state->moveTo(x,y);
    }
    state = state->restore();
  }
}

void XWTikzShape::andGateCDHShape(bool dopath)
{
  XW_TIKZ_SHAPE_LOGIC_GATE_US_OR

  switch (state->anchor)
  {
    default:
      {
        xa = state->anotherPoint.x();
        ya = state->anotherPoint.y();
        QPointF externalpoint(xa,ya);
        xb = -0.166666 * halfside;
        if (xa < xb)
        {
          double xc = 0.166666 * halfside + halfwidth;
          QPointF p(xc,halfheight);
          p = pointBorderRectangle(externalpoint, p);
          anchorPos = centerPoint + p;
        }
        else
        {
          if (y == 0)
            anchorPos = centerPoint + tipanchor;
          else
          {
            externalpoint = centerPoint + externalpoint;
            double xc = halfwidth + halfside;
            double yc = halfheight + halfside;
            if (ya < 0)
            {
              x = centerPoint.x() + xb;
              y = centerPoint.y() + halfside;
              QPointF c(x,y);
              anchorPos = pointIntersectionOfLineAndArc(externalpoint, centerPoint, c, 270, 330,yc, yc);
            }
            else
            {
              x = centerPoint.x() + xb;
              y = centerPoint.y() - halfside;
              QPointF c(x,y);
              anchorPos = pointIntersectionOfLineAndArc(externalpoint, centerPoint, c, 30, 90,xc, yc);
            }
          }
        }
      }
      break;

    case PGFcenter:
      anchorPos = centerPoint;
      break;

    case PGFmid:
      anchorPos = midPoint;
      break;

    case PGFmideast:
      x = centerPoint.x() + tipanchor.x();
      y = midPoint.y();
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFmidwest:
      x = centerPoint.x() - 1.16666 * xa - outerxsep;
      y = midPoint.y();
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFbase:
      anchorPos = basePoint;
      break;

    case PGFbaseeast:
      x = centerPoint.x() + tipanchor.x();
      y = basePoint.y();
      break;

    case PGFbasewest:
      x = centerPoint.x() - 1.16666 * xa - outerxsep;
      y = basePoint.y();
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFeast:
    case PGFoutput:
      anchorPos = centerPoint + tipanchor;
      break;

    case PGFnortheast:
      if (state->logicGateAnchorsUseBoundingBox)
      {
        x = centerPoint.x() + tipanchor.x();
        y = centerPoint.y() + halfheight;
      }
      else
      {
        xa = halfwidth;
        ya = halfheight;
        xb = halfside;
        x = centerPoint.x() - 0.166666 * xb;
        xa += xb;
        x += 0.5 * xa;
        y = centerPoint.y() - xb;
        ya += xb;
        y += 0.866025 * ya;
      }
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFsoutheast:
      if (state->logicGateAnchorsUseBoundingBox)
      {
        x = centerPoint.x() + tipanchor.x();
        y = centerPoint.y() - halfheight;
      }
      else
      {
        xa = halfwidth;
        ya = halfheight;
        xb = halfside;
        x = centerPoint.x() - 0.166666 * xb;
        xa += xb;
        x += 0.5 * xa;
        y = centerPoint.y() + xb;
        ya += xb;
        y -= 0.866025 * ya;
      }
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFnorth:
      x = centerPoint.x();
      y = centerPoint.y();
      if (state->logicGateAnchorsUseBoundingBox)
        y += halfheight;
      else
        y += 0.993043 * halfheight;
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFsouth:
      x = centerPoint.x();
      y = centerPoint.y();
      if (state->logicGateAnchorsUseBoundingBox)
        y -= halfheight;
      else
        y -= 0.993043 * halfheight;
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFsouthwest:
      xa = halfside;
      x = centerPoint.x() - 1.16666 * xa - outerxsep;
      y = centerPoint.y() - halfheight;
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFnorthwest:
      xa = halfside;
      x = centerPoint.x() - 1.16666 * xa - outerxsep;
      y = centerPoint.y() + halfheight;
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFwest:
      xa = halfside;
      x = centerPoint.x() - 1.16666 * xa - outerxsep;
      y = centerPoint.y();
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;
  }

  if (dopath)
  {
    double xc = halfwidth - outerxsep;
    double yc = halfheight - outerysep;
    state = state->save();
    state->shift(centerPoint.x(), centerPoint.y());
    state->moveTo(-0.16666 * xc, yc);
    yc = 2 * yc;
    state->setStartAngle(90);
    state->setEndAngle(30);
    state->setXRadius(yc);
    state->setYRadius(yc);
    state->addArc();
    yc = 2 * yc;
    state->setStartAngle(90);
    state->setEndAngle(30);
    state->setXRadius(yc);
    state->setYRadius(yc);
    state->addArc();
    state->lineTo(-1.16666 * xc, -yc);
    state->lineTo(-1.16666 * xc, yc);
    state->closePath();
    state = state->restore();

    double tempdima = 2 * yc;
    tempdima /= counta;
    for (int i = 0; i < numinputs; i++)
    {
      yc -= tempdima;
      if (state->inputs[i] == QChar('i'))
      {
        xa = halfside;
        x = -1.16666 * xa - 0.5 * state->lineWidth - invertedradius;
        y = yc;
        QPointF c(x,y);
        state = state->save();
        state->addCircle(c,invertedradius);
        state = state->restore();
      }
    }
  }
}

void XWTikzShape::nandGateCDHShape(bool dopath)
{
  XW_TIKZ_SHAPE_LOGIC_GATE_US_OR

  switch (state->anchor)
  {
    default:
      {
        xa = state->anotherPoint.x();
        ya = state->anotherPoint.y();
        QPointF externalpoint(xa,ya);
        xb = -0.166666 * halfside;
        if (xa < xb)
        {
          double xc = 0.166666 * halfside + halfwidth;
          QPointF p(xc,halfheight);
          p = pointBorderRectangle(externalpoint, p);
          anchorPos = centerPoint + p;
        }
        else
        {
          if (y == 0)
            anchorPos = centerPoint + tipanchor;
          else
          {
            externalpoint = centerPoint + externalpoint;
            double xc = halfwidth + halfside;
            double yc = halfheight + halfside;
            if (ya < 0)
            {
              x = centerPoint.x() + xb;
              y = centerPoint.y() + halfside;
              QPointF c(x,y);
              anchorPos = pointIntersectionOfLineAndArc(externalpoint, centerPoint, c, 270, 330,yc, yc);
            }
            else
            {
              x = centerPoint.x() + xb;
              y = centerPoint.y() - halfside;
              QPointF c(x,y);
              anchorPos = pointIntersectionOfLineAndArc(externalpoint, centerPoint, c, 30, 90,xc, yc);
            }
          }
        }
      }
      break;

    case PGFcenter:
      anchorPos = centerPoint;
      break;

    case PGFmid:
      anchorPos = midPoint;
      break;

    case PGFmideast:
      x = centerPoint.x() + tipanchor.x();
      y = midPoint.y();
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFmidwest:
      x = centerPoint.x() - 1.16666 * xa - outerxsep;
      y = midPoint.y();
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFbase:
      anchorPos = basePoint;
      break;

    case PGFbaseeast:
      x = centerPoint.x() + tipanchor.x();
      y = basePoint.y();
      break;

    case PGFbasewest:
      x = centerPoint.x() - 1.16666 * xa - outerxsep;
      y = basePoint.y();
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFoutput:
      xa = halfside;
      x = -0.166666 * xa;
      xb = 2 * xa;
      x += 0.866025 * xb;
      x += invertedradius;
      x += outerinvertedradius;
      x += outerxsep;
      x += centerPoint.x();
      y = centerPoint.y();
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFeast:    
      anchorPos = centerPoint + tipanchor;
      break;

    case PGFnortheast:
      if (state->logicGateAnchorsUseBoundingBox)
      {
        x = centerPoint.x() + tipanchor.x();
        y = centerPoint.y() + halfheight;
      }
      else
      {
        xa = halfwidth;
        ya = halfheight;
        xb = halfside;
        x = centerPoint.x() - 0.166666 * xb;
        xa += xb;
        x += 0.5 * xa;
        y = centerPoint.y() - xb;
        ya += xb;
        y += 0.866025 * ya;
      }
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFsoutheast:
      if (state->logicGateAnchorsUseBoundingBox)
      {
        x = centerPoint.x() + tipanchor.x();
        y = centerPoint.y() - halfheight;
      }
      else
      {
        xa = halfwidth;
        ya = halfheight;
        xb = halfside;
        x = centerPoint.x() - 0.166666 * xb;
        xa += xb;
        x += 0.5 * xa;
        y = centerPoint.y() + xb;
        ya += xb;
        y -= 0.866025 * ya;
      }
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFnorth:
      x = centerPoint.x();
      y = centerPoint.y();
      if (state->logicGateAnchorsUseBoundingBox)
        y += halfheight;
      else
        y += 0.993043 * halfheight;
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFsouth:
      x = centerPoint.x();
      y = centerPoint.y();
      if (state->logicGateAnchorsUseBoundingBox)
        y -= halfheight;
      else
        y -= 0.993043 * halfheight;
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFsouthwest:
      xa = halfside;
      x = centerPoint.x() - 1.16666 * xa - outerxsep;
      y = centerPoint.y() - halfheight;
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFnorthwest:
      xa = halfside;
      x = centerPoint.x() - 1.16666 * xa - outerxsep;
      y = centerPoint.y() + halfheight;
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;

    case PGFwest:
      xa = halfside;
      x = centerPoint.x() - 1.16666 * xa - outerxsep;
      y = centerPoint.y();
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;
  }

  if (dopath)
  {
    double xc = halfwidth - outerxsep;
    double yc = halfheight - outerysep;
    state = state->save();
    state->shift(centerPoint.x(), centerPoint.y());
    state->moveTo(-0.16666 * xc, yc);
    yc = 2 * yc;
    state->setStartAngle(90);
    state->setEndAngle(30);
    state->setXRadius(yc);
    state->setYRadius(yc);
    state->addArc();
    yc = 2 * yc;
    state->setStartAngle(90);
    state->setEndAngle(30);
    state->setXRadius(yc);
    state->setYRadius(yc);
    state->addArc();
    state->lineTo(-1.16666 * xc, -yc);
    state->lineTo(-1.16666 * xc, yc);
    state->closePath();
    state = state->restore();

    x = -0.166666 * xc;
    yc *= 2;
    x += 0.866025 * yc;
    x += outerinvertedradius;
    y = 0;
    QPointF b(x,y);
    state = state->save();
    state->addCircle(b,invertedradius);
    state = state->restore();

    double tempdima = 2 * yc;
    tempdima /= counta;
    for (int i = 0; i < numinputs; i++)
    {
      yc -= tempdima;
      if (state->inputs[i] == QChar('i'))
      {
        xa = halfside;
        x = -1.16666 * xa - 0.5 * state->lineWidth - invertedradius;
        y = yc;
        QPointF c(x,y);
        state = state->save();
        state->addCircle(c,invertedradius);
        state = state->restore();
      }
    }
  }
}
