/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include <QPointF>
#include "PGFKeyWord.h"
#include "XWTikzGraphic.h"
#include "XWTikzShape.h"
#include "XWTikzExpress.h"
#include "XWTikzState.h"
#include "XWTikzOption.h"
#include "XWTikzDecoration.h"

void XWTikzDecoration::doTrianglesInitState(XWTikzState * stateA)
{
  stateA->decorateWidth = 0;
  ssw = stateA->shapeStartWidth;
  ssh = stateA->shapeStartHeight / 2;
  current_state = &XWTikzDecoration::doTrianglesTriangleState;
}

void XWTikzDecoration::doTrianglesTriangleState(XWTikzState * stateA)
{
  stateA->decorateWidth = stateA->decorationSegmentLength;
  if (stateA->decoratedRemainingDistance < stateA->decorationSegmentLength)
    current_state = &XWTikzDecoration::doTrianglesLastState;

  stateA->moveTo(0, ssh);
  stateA->lineTo(ssw, 0);
  stateA->lineTo(0, -ssh);
  stateA->closePath();
}

void XWTikzDecoration::doTrianglesLastState(XWTikzState * stateA)
{
  stateA->decorateWidth = stateA->decoratedRemainingDistance;
  if (stateA->decoratedRemainingDistance < ssw)
    current_state = &XWTikzDecoration::doTrianglesSkipState;
  else
    current_state = &XWTikzDecoration::doTrianglesFinalState;

  stateA->moveTo(0, ssh);
  stateA->lineTo(ssw, 0);
  stateA->lineTo(0, -ssh);
  stateA->closePath();
}

void XWTikzDecoration::doTrianglesSkipState(XWTikzState * stateA)
{
  stateA->decorateWidth = stateA->decoratedRemainingDistance;
}

void XWTikzDecoration::doTrianglesFinalState(XWTikzState * stateA)
{
  stateA->moveTo(stateA->decoratedPathLast);
  current_state = &XWTikzDecoration::nullState;
}

void XWTikzDecoration::doCrossesInitState(XWTikzState * stateA)
{
  stateA->decorateWidth = 0;
  ssw = stateA->shapeStartWidth / 2;
  ssh = stateA->shapeStartHeight / 2;
  current_state = &XWTikzDecoration::doCrossesCrossesState;
}

void XWTikzDecoration::doCrossesCrossesState(XWTikzState * stateA)
{
  stateA->decorateWidth = stateA->decorationSegmentLength;
  if (stateA->decoratedRemainingDistance < stateA->decorationSegmentLength)
    current_state = &XWTikzDecoration::doCrossesLastState;

  stateA->moveTo(-ssw, ssh);
  stateA->lineTo(ssw, -ssh);
  stateA->moveTo(-ssw, -ssh);
  stateA->lineTo(ssw, ssh);
}

void XWTikzDecoration::doCrossesLastState(XWTikzState * stateA)
{
  stateA->decorateWidth = stateA->decoratedRemainingDistance;
  stateA->moveTo(-ssw, ssh);
  stateA->lineTo(ssw, -ssh);
  stateA->moveTo(-ssw, -ssh);
  stateA->lineTo(ssw, ssh);
}

void XWTikzDecoration::doCrossesFinalState(XWTikzState * stateA)
{
  stateA->moveTo(stateA->decoratedPathLast);
  current_state = &XWTikzDecoration::nullState;
}

void XWTikzDecoration::doShapeBackgroundsInitState(XWTikzState * stateA)
{
  stateA->decorateWidth = initialise;
  current_state = &XWTikzDecoration::doShapeBackgroundsShapeState;
  node = new XWTikzShape(stateA->driver,0,stateA, XW_TIKZ_NODE);
  stateA->setInnerXSep(50);
  stateA->setInnerXSep(50);
  stateA->seiMinimumWidth(1);
  stateA->seiMinimumHeight(1);
  node->doShape(false);
  if (stateA->pathMinX > node->westPos.x())
    stateA->pathMinX = node->westPos.x();

  if (stateA->pathMinY > node->southPos.y())
    stateA->pathMinY = node->southPos.y();

  if (stateA->pathMaxX < node->eastPos.x())
    stateA->pathMaxX = node->eastPos.x();

  if (stateA->pathMaxY < node->northPos.y())
    stateA->pathMaxY = node->northPos.y();

  stateA->width = stateA->pathMaxX - stateA->pathMinX;
  stateA->height = stateA->pathMaxY - stateA->pathMinY;
  beforeShape = 0;
  afterShape = 0;
  width = stateA->shapeStartWidth;
  widthChange = stateA->shapeStartWidth - stateA->shapeEndWidth;
  heightChange = stateA->shapeStartHeight - stateA->shapeEndHeight;
  if (stateA->betweenOrBy != PGFbycenters && stateA->betweenOrBy != PGFbetweencenters)
    betweenBorders = true;

  if (stateA->shapeEvenlySpread == 0)
    shapeSep = stateA->shapeSep;
  else
  {
    int counta = stateA->shapeEvenlySpread;
    double x = 0;
    if (betweenBorders)
    {
      if (counta > 1)
      {
        counta--;
        x = stateA->shapeStartWidth;
        if (stateA->shapeScaled)
          x += stateA->shapeEndWidth;
        else
          x *= 2;

        x *= 0.5;
        x *= -counta;
        x += stateA->decoratedRemainingDistance;
        x /= counta;
        x *= 0.9999;

        if (stateA->shapeScaled)
          specialWidth = widthChange / counta;
      }
      else
      {
        betweenBorders = false;
        x = stateA->decoratedRemainingDistance;
        if (counta == 1)
          initialise = 0.5 * x;
        else
        {
          x += 5;
          initialise = x;
        }
      }
    }
    else
    {
      x = stateA->decoratedRemainingDistance;
      if (counta > 1)
      {
        counta--;
        x /= counta;
      }
      else
      {
        if (counta == 1)
          initialise = 0.5 * x;
        else
        {
          x += 5;
          initialise = x;
        }
      }
    }

    shapeSep = x;
  }
}

void XWTikzDecoration::doShapeBackgroundsBeforeShapeState(XWTikzState * stateA)
{
  stateA->decorateWidth = beforeShape - 0.9963;
  current_state = &XWTikzDecoration::doShapeBackgroundsShapeState;
  if (stateA->shapeScaled)
  {
    double mathresult = 0;
    if (betweenBorders)
    {
      if (stateA->shapeEvenlySpread == 0)
      {
        double x = stateA->decoratedCompletedDistance + stateA->decoratedRemainingDistance;
        double xa = 0.5 * stateA->shapeStartWidth + stateA->decoratedCompletedDistance;
        double xb = -0.5 * widthChange + x;
        mathresult = xa / xb;
      }
      else
      {
        double y = stateA->decoratedCompletedDistance + stateA->decoratedRemainingDistance;
        mathresult = stateA->decoratedCompletedDistance / y;
      }
    }

    double x = 0;
    if (stateA->shapeEvenlySpread == 0)
      x = mathresult * widthChange + stateA->shapeStartWidth;
    else
    {
      if (betweenBorders)
      {
        x = width + specialWidth;
        double xa = x - stateA->shapeStartWidth;
        double xb = widthChange;
        mathresult = xa / xb;
      }
      else
        x = mathresult * widthChange + stateA->shapeStartWidth;
    }

    width = x;
    double y = mathresult * heightChange + stateA->shapeStartHeight;
    height = y;
  }

  if (betweenBorders)
    beforeShape = 0.5 * width;
  else
    beforeShape = 0;
}

void XWTikzDecoration::doShapeBackgroundsShapeState(XWTikzState * stateA)
{
  stateA->decorateWidth = 0;
  current_state = &XWTikzDecoration::doShapeBackgroundsAfterShapeState;
  if (!stateA->shapeSloped)
    stateA->rotate(-stateA->decoratedAngle);

  double tempdima = stateA->width;
  double tempdimb = stateA->height;
  stateA->scale(width / tempdima, height / tempdimb);
  node->doShape(false);
  stateA->shift(-node->anchorPos.x(), -node->anchorPos.y());
  node->doShape(true);
}

void XWTikzDecoration::doShapeBackgroundsAfterShapeState(XWTikzState * stateA)
{
  stateA->decorateWidth = afterShape - 0.9963;
  current_state = &XWTikzDecoration::doShapeBackgroundsSepState;
  if (betweenBorders)
    afterShape = width * 0.5;
  else
    afterShape = 0;
}

void XWTikzDecoration::doShapeBackgroundsSepState(XWTikzState * stateA)
{
  stateA->decorateWidth = shapeSep;
  beforeShape = 0;
  current_state = &XWTikzDecoration::doShapeBackgroundsBeforeShapeState;
}

void XWTikzDecoration::doShapeBackgroundsFinalState(XWTikzState * stateA)
{
  if (node)
    delete node;
  stateA->moveTo(stateA->decoratedPathLast);
  current_state = &XWTikzDecoration::nullState;
}
