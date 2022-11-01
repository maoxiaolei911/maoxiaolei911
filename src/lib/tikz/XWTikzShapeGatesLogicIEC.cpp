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

#define XW_TIKZ_SHAPE_LOGIC_GATE_IEC_DIMENSION \
  int numinputs = state->inputs.size(); \
  double invertedradius = state->logicGateInvertedRadius; \
  double outerinvertedradius = state->logicGateInvertedRadius; \
  double x = 0.5 * boxWidth + state->innerXSep; \
  double y = 0.5 * boxHeight + boxDepth + state->innerYSep; \
  double xa = 0.5 * state->minWidth; \
  double ya = 0.5 * state->minHeight; \
  if (x < xa) \
    x = 0.5 * state->minWidth; \
  if (y < ya) \
    y = 0.5 * state->minHeight; \
  double yb = 2 * state->logicGateInvertedRadius + state->lineWidth; \
  double yc = state->logicGateInputSep;\
  if (yb < yc) \
    yb = yc; \
  int counta = numinputs + 1; \
  yb *= counta; \
  if (y < 0.5 * yb) \
    y = 0.5 * yb; \
  xa = x - state->innerXSep; \
  ya = y - state->innerYSep;\
  x += state->outerXSep; \
  y += state->outerYSep; \
  double halfwidth = x; \
  double halfheight = y; \
  double tempdima;

#define XW_TIKZ_SHAPE_LOGIC_GATE_IEC_INPUT_ANCHOR(n) \
  x = centerPoint.x() - halfwidth;\
  y = centerPoint.y(); \
  if (state->inputs[i] == QChar('i')) \
  {\
    x -= invertedradius; \
    x -= outerinvertedradius; \
  }\
  tempdima = 2 * halfheight;\
  counta = numinputs + 1; \
  tempdima /= counta;\
  tempdima *= n; \
  y += halfheight; \
  y -= tempdima;

void XWTikzShape::andGateIECShape(bool dopath)
{
  XW_TIKZ_SHAPE_LOGIC_GATE_IEC_DIMENSION

  switch (state->anchor)
  {
    default:
      {
        x = state->anotherPoint.x();
        y = state->anotherPoint.y();
        QPointF pd(x,y);
        QPointF ur(halfwidth,halfheight);
        QPointF p = pointBorderRectangle(pd,ur);
        anchorPos = centerPoint + p;
      }
      break;

    case PGFcenter:
      anchorPos = centerPoint;
      break;

    case PGFmid:
      anchorPos = midPoint;
      break;

    case PGFmidwest:
      anchorPos = midPoint;
      anchorPos.setX(midPoint.x() - halfwidth);
      break;

    case PGFmideast:
      anchorPos = midPoint;
      anchorPos.setX(midPoint.x() + halfwidth);
      break;

    case PGFbase:
      anchorPos = basePoint;
      break;

    case PGFbasewest:
      anchorPos = basePoint;
      anchorPos.setX(basePoint.x() - halfwidth);
      break;

    case PGFbaseeast:
      anchorPos = basePoint;
      anchorPos.setX(basePoint.x() + halfwidth);
      break;

    case PGFnorth:
      anchorPos = centerPoint;
      anchorPos.setY(centerPoint.y() + halfheight);
      break;

    case PGFsouth:
      anchorPos = centerPoint;
      anchorPos.setY(centerPoint.y() - halfheight);
      break;

    case PGFeast:
      anchorPos = centerPoint;
      anchorPos.setX(centerPoint.x() + halfwidth);
      break;

    case PGFwest:
      anchorPos = centerPoint;
      anchorPos.setX(centerPoint.x() - halfwidth);
      break;

    case PGFnortheast:
      anchorPos.setX(centerPoint.x() + halfwidth);
      anchorPos.setY(centerPoint.y() + halfheight);
      break;

    case PGFnorthwest:
      anchorPos.setX(centerPoint.x() - halfwidth);
      anchorPos.setY(centerPoint.y() + halfheight);
      break;

    case PGFsoutheast:
      anchorPos.setX(centerPoint.x() + halfwidth);
      anchorPos.setY(centerPoint.y() - halfheight);
      break;

    case PGFsouthwest:
      anchorPos.setX(centerPoint.x() - halfwidth);
      anchorPos.setY(centerPoint.y() - halfheight);
      break;

    case PGFinput:
      {
        int i = (int)(state->angle);
        XW_TIKZ_SHAPE_LOGIC_GATE_IEC_INPUT_ANCHOR(i);
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      break;

    case PGFoutput:
      anchorPos = centerPoint;
      anchorPos.setX(centerPoint.x() + halfwidth);
      break;
  }
  
  if (dopath)
  {
    double xc = halfwidth - state->outerXSep;
    yc = halfheight - state->outerXSep;    
    state = state->save();    
    state->shift(centerPoint.x(), centerPoint.y());
    QPointF ll(-xc, -yc);
    state->rectangle(ll,2 * xc, 2 * yc);
    state = state->restore();

    xc = -halfwidth - invertedradius;
    yc = halfheight;
    tempdima = 2 * yc;
    counta = numinputs + 1;
    tempdima /= counta;
    for (int i = 0; i < state->inputs.size(); i++)
    {
      yc -= tempdima;
      if (state->inputs[i] == QChar('i'))
      {
        state = state->save();
        QPointF c(xc, yc);
        state->addCircle(c,invertedradius);
        state = state->restore();
      }
    }

    doContent();
  }
}

void XWTikzShape::bufferGateIECShape(bool dopath)
{
  andGateIECShape(dopath);
}

void XWTikzShape::nandGateIECShape(bool dopath)
{
  XW_TIKZ_SHAPE_LOGIC_GATE_IEC_DIMENSION

  switch (state->anchor)
  {
    default:
      {
        x = state->anotherPoint.x();
        y = state->anotherPoint.y();
        QPointF pd(x,y);
        QPointF ur(halfwidth,halfheight);
        QPointF p = pointBorderRectangle(pd,ur);
        anchorPos = centerPoint + p;
      }
      break;

    case PGFcenter:
      anchorPos = centerPoint;
      break;

    case PGFmid:
      anchorPos = midPoint;
      break;

    case PGFmidwest:
      anchorPos = midPoint;
      anchorPos.setX(midPoint.x() - halfwidth);
      break;

    case PGFmideast:
      anchorPos = midPoint;
      anchorPos.setX(midPoint.x() + halfwidth);
      break;

    case PGFbase:
      anchorPos = basePoint;
      break;

    case PGFbasewest:
      anchorPos = basePoint;
      anchorPos.setX(basePoint.x() - halfwidth);
      break;

    case PGFbaseeast:
      anchorPos = basePoint;
      anchorPos.setX(basePoint.x() + halfwidth);
      break;

    case PGFnorth:
      anchorPos = centerPoint;
      anchorPos.setY(centerPoint.y() + halfheight);
      break;

    case PGFsouth:
      anchorPos = centerPoint;
      anchorPos.setY(centerPoint.y() - halfheight);
      break;

    case PGFeast:
      anchorPos = centerPoint;
      anchorPos.setX(centerPoint.x() + halfwidth);
      break;

    case PGFwest:
      anchorPos = centerPoint;
      anchorPos.setX(centerPoint.x() - halfwidth);
      break;

    case PGFnortheast:
      anchorPos.setX(centerPoint.x() + halfwidth);
      anchorPos.setY(centerPoint.y() + halfheight);
      break;

    case PGFnorthwest:
      anchorPos.setX(centerPoint.x() - halfwidth);
      anchorPos.setY(centerPoint.y() + halfheight);
      break;

    case PGFsoutheast:
      anchorPos.setX(centerPoint.x() + halfwidth);
      anchorPos.setY(centerPoint.y() - halfheight);
      break;

    case PGFsouthwest:
      anchorPos.setX(centerPoint.x() - halfwidth);
      anchorPos.setY(centerPoint.y() - halfheight);
      break;

    case PGFinput:
      {
        int i = (int)(state->angle);
        XW_TIKZ_SHAPE_LOGIC_GATE_IEC_INPUT_ANCHOR(i);
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      break;

    case PGFoutput:
      x = centerPoint.x() + halfwidth + invertedradius;
      y = centerPoint.y();
      anchorPos.setX(x);
      anchorPos.setY(y);
      break;
  }
  
  if (dopath)
  {
    double xc = halfwidth - state->outerXSep;
    yc = halfheight - state->outerXSep;
    state = state->save();
    state->shift(centerPoint.x(), centerPoint.y());
    QPointF ll(-xc, -yc);
    state->rectangle(ll,2 * xc, 2 * yc);
    state = state->restore();

    xc = -halfwidth - invertedradius;
    yc = halfheight;
    tempdima = 2 * yc;
    counta = numinputs + 1;
    tempdima /= counta;
    for (int i = 0; i < state->inputs.size(); i++)
    {
      yc -= tempdima;
      if (state->inputs[i] == QChar('i'))
      {
        state = state->save();
        QPointF c(xc, yc);
        state->addCircle(c,invertedradius);
        state = state->restore();
      }
    }

    state = state->save();
    x = centerPoint.x() + halfwidth + invertedradius;
    y = centerPoint.y();
    QPointF c(x,y);
    state->addCircle(c,invertedradius);
    state = state->restore();

    doContent();
  }
}

void XWTikzShape::norGateIECShape(bool dopath)
{
  XW_TIKZ_SHAPE_LOGIC_GATE_IEC_DIMENSION

  switch (state->anchor)
  {
    default:
      {
        x = state->anotherPoint.x();
        y = state->anotherPoint.y();
        QPointF pd(x,y);
        QPointF ur(halfwidth,halfheight);
        QPointF p = pointBorderRectangle(pd,ur);
        anchorPos = centerPoint + p;
      }
      break;

    case PGFcenter:
      anchorPos = centerPoint;
      break;

    case PGFmid:
      anchorPos = midPoint;
      break;

    case PGFmidwest:
      anchorPos = midPoint;
      anchorPos.setX(midPoint.x() - halfwidth);
      break;

    case PGFmideast:
      anchorPos = midPoint;
      anchorPos.setX(midPoint.x() + halfwidth);
      break;

    case PGFbase:
      anchorPos = basePoint;
      break;

    case PGFbasewest:
      anchorPos = basePoint;
      anchorPos.setX(basePoint.x() - halfwidth);
      break;

    case PGFbaseeast:
      anchorPos = basePoint;
      anchorPos.setX(basePoint.x() + halfwidth);
      break;

    case PGFnorth:
      anchorPos = centerPoint;
      anchorPos.setY(centerPoint.y() + halfheight);
      break;

    case PGFsouth:
      anchorPos = centerPoint;
      anchorPos.setY(centerPoint.y() - halfheight);
      break;

    case PGFeast:
      anchorPos = centerPoint;
      anchorPos.setX(centerPoint.x() + halfwidth);
      break;

    case PGFwest:
      anchorPos = centerPoint;
      anchorPos.setX(centerPoint.x() - halfwidth);
      break;

    case PGFnortheast:
      anchorPos.setX(centerPoint.x() + halfwidth);
      anchorPos.setY(centerPoint.y() + halfheight);
      break;

    case PGFnorthwest:
      anchorPos.setX(centerPoint.x() - halfwidth);
      anchorPos.setY(centerPoint.y() + halfheight);
      break;

    case PGFsoutheast:
      anchorPos.setX(centerPoint.x() + halfwidth);
      anchorPos.setY(centerPoint.y() - halfheight);
      break;

    case PGFsouthwest:
      anchorPos.setX(centerPoint.x() - halfwidth);
      anchorPos.setY(centerPoint.y() - halfheight);
      break;

    case PGFinput:
      {
        int i = (int)(state->angle);
        XW_TIKZ_SHAPE_LOGIC_GATE_IEC_INPUT_ANCHOR(i);
        anchorPos.setX(x);
        anchorPos.setY(y);
      }
      break;

    case PGFoutput:
      anchorPos = centerPoint;
      anchorPos.setX(centerPoint.x() + halfwidth);
      break;
  }

  if (dopath)
  {
    double xc = halfwidth - state->outerXSep;
    yc = halfheight - state->outerXSep;
    state = state->save();
    state->shift(centerPoint.x(), centerPoint.y());
    QPointF ll(-xc, -yc);
    state->rectangle(ll,2 * xc, 2 * yc);
    state = state->restore();

    xc = -halfwidth - invertedradius;
    yc = halfheight;
    tempdima = 2 * yc;
    counta = numinputs + 1;
    tempdima /= counta;
    for (int i = 0; i < state->inputs.size(); i++)
    {
      yc -= tempdima;
      if (state->inputs[i] == QChar('i'))
      {
        state = state->save();
        QPointF c(xc, yc);
        state->addCircle(c,invertedradius);
        state = state->restore();
      }
    }

    state = state->save();
    x = centerPoint.x() + halfwidth + invertedradius;
    y = centerPoint.y();
    QPointF c(x,y);
    state->addCircle(c,invertedradius);
    state = state->restore();

    doContent();
  }
}

void XWTikzShape::notGateIECShape(bool dopath)
{
  norGateIECShape(dopath);
}

void XWTikzShape::orGateIECShape(bool dopath)
{
  andGateIECShape(dopath);
}

void XWTikzShape::xnorGateIECShape(bool dopath)
{
  norGateIECShape(dopath);
}

void XWTikzShape::xorGateIECShape(bool dopath)
{
  andGateIECShape(dopath);
}