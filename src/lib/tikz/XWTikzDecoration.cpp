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

XWTikzDecoration::XWTikzDecoration(int dtypeA)
:dtype(dtypeA),
 cur(-1),
sequenceNumber(0),
distanceFromStart(0),
computedWidth(0)
{
  switch (dtype)
  {
    default:
      current_state = &XWTikzDecoration::nullState;
      final_state = &XWTikzDecoration::nullState;
      break;

    case PGFlineto:
      current_state = &XWTikzDecoration::doLineToInitState;
      final_state = &XWTikzDecoration::doLineToFinalState;
      break;

    case PGFmoveto:
      current_state = &XWTikzDecoration::doMoveToInitState;
      final_state = &XWTikzDecoration::doMoveToFinalState;
      break;

    case PGFcurveto:
      current_state = &XWTikzDecoration::doCurveToInitState;
      final_state = &XWTikzDecoration::doCurveToFinalState;
      break;

    case PGFcircleconnectionbar:
      current_state = &XWTikzDecoration::circleConnectionBarInitState;
      final_state = &XWTikzDecoration::nullState;
      break;

    case PGFmarkings:
      current_state = &XWTikzDecoration::doMarkingsInitState;
      final_state = &XWTikzDecoration::doMarkingsFinalState;
      break;

    case PGFzigzag:
      current_state = &XWTikzDecoration::doZigzagUpFromCenterState;
      final_state = &XWTikzDecoration::doZigzagFinalState;
      break;

    case PGFsaw:
      current_state = &XWTikzDecoration::doSawInitState;
      final_state = &XWTikzDecoration::doSawFinalState;
      break;

    case PGFrandomsteps:
      current_state = &XWTikzDecoration::doRandomStepsStartState;
      final_state = &XWTikzDecoration::doRandomStepsFinalState;
      break;

    case PGFstraightzigzag:
      current_state = &XWTikzDecoration::doStraightZigzagLineToState;
      final_state = &XWTikzDecoration::doStraightZigzagFinalState;
      break;

    case PGFbent:
      current_state = &XWTikzDecoration::doBentBentState;
      final_state = &XWTikzDecoration::doBentFinalState;
      break;

    case PGFsnake:
      current_state = &XWTikzDecoration::doSnakeInitState;
      final_state = &XWTikzDecoration::doSnakeFinalState;
      break;

    case PGFcoil:
      current_state = &XWTikzDecoration::doCoilCoilState;
      final_state = &XWTikzDecoration::doCoilFinalState;
      break;

    case PGFbumps:
      current_state = &XWTikzDecoration::doBumpsInitState;
      final_state = &XWTikzDecoration::doBumpsFinalState;
      break;
  }
}

void XWTikzDecoration::doCurrentState(XWTikzState * stateA)
{
  (this->*(current_state))(stateA);
}

void XWTikzDecoration::doFinalState(XWTikzState * stateA)
{
  (this->*(final_state))(stateA);
}

void XWTikzDecoration::doInitState(XWTikzState * stateA)
{
  switch (dtype)
  {
    default:
      break;
      
    case PGFlineto:
      doLineToInitState(stateA);
      break;

    case PGFmoveto:
      doMoveToInitState(stateA);
      break;

    case PGFcurveto:
      doCurveToInitState(stateA);
      break;

    case PGFcircleconnectionbar:
      circleConnectionBarInitState(stateA);
      break;

    case PGFmarkings:
      doMarkingsInitState(stateA);
      break;

    case PGFzigzag:
      doZigzagUpFromCenterState(stateA);
      break;

    case PGFsaw:
      doSawInitState(stateA);
      break;

    case PGFrandomsteps:
      doRandomStepsStartState(stateA);
      break;

    case PGFstraightzigzag:
      doStraightZigzagLineToState(stateA);
      break;

    case PGFbent:
      doBentBentState(stateA);
      break;

    case PGFsnake:
      doSnakeInitState(stateA);
      break;

    case PGFcoil:
      doCoilCoilState(stateA);
      break;

    case PGFbumps:
      doBumpsInitState(stateA);
      break;
  }
}

bool XWTikzDecoration::isNullState()
{
  return current_state == &XWTikzDecoration::nullState;
}

void XWTikzDecoration::circleConnectionBarInitState(XWTikzState * stateA)
{
  stateA->shift(stateA->startRadius, 0);
  double x = stateA->startRadius * cos(stateA->decorationSegmentAngle);
  double y = stateA->startRadius * sin(stateA->decorationSegmentAngle);
  stateA->moveTo(x, y);
  stateA->addArc(stateA->decorationSegmentAngle,-stateA->decorationSegmentAngle,stateA->startRadius,stateA->startRadius);
  double tempcnta = 90 - stateA->decorationSegmentAngle;
  double tempdima = stateA->startRadius;
  double tempdimb = stateA->decorationSegmentAmplitude;
  x = stateA->startRadius * cos(stateA->decorationSegmentAngle) + 0.25 * tempdima * cos(tempcnta);
  y = stateA->startRadius * sin(stateA->decorationSegmentAngle) + 0.25 * tempdima * sin(tempcnta);
  stateA->curveTo(x, y, 1.25 * tempdima, -0.5 * tempdimb, 1.5 * tempdima, -0.5 * tempdimb);
  stateA->lineTo(1.5 * tempdima, 0.5 * tempdimb);
  tempcnta = stateA->decorationSegmentAngle - 90;
  x = stateA->startRadius * cos(stateA->decorationSegmentAngle) + 0.25 * tempdima * cos(tempcnta);
  y = stateA->startRadius * sin(stateA->decorationSegmentAngle) + 0.25 * tempdima * sin(tempcnta);
  double xa = stateA->startRadius * cos(stateA->decorationSegmentAngle);
  double ya = stateA->startRadius * sin(stateA->decorationSegmentAngle);
  stateA->curveTo(1.25 * tempdima, 0.5 * tempdimb, x, y, xa, ya);
  stateA->closePath();

  current_state = &XWTikzDecoration::circleConnectionBarBarState;
}

void XWTikzDecoration::circleConnectionBarBarState(XWTikzState * stateA)
{
  double tempdima = stateA->startRadius;
  double tempdimb = stateA->endRadius;
  double xc = stateA->decorationSegmentAmplitude;
  QPointF ll(0.5 * tempdima, -0.5 * xc);
  QPointF ur(stateA->decoratedRemainingDistance - 0.5 * tempdimb - 0.5 * tempdima, xc);
  stateA->addRectangle(ll, ur);
  current_state = &XWTikzDecoration::circleConnectionBarEndState;
}

void XWTikzDecoration::circleConnectionBarEndState(XWTikzState * stateA)
{
  stateA->shift(stateA->decoratedRemainingDistance,0);
  stateA->scale(-1,1);
  stateA->shift(stateA->decoratedRemainingDistance,0);
  double x = stateA->endRadius * cos(stateA->decorationSegmentAngle);
  double y = stateA->endRadius * sin(stateA->decorationSegmentAngle);
  stateA->moveTo(x, y);
  stateA->addArc(stateA->decorationSegmentAngle, -stateA->decorationSegmentAngle,stateA->endRadius,stateA->endRadius);
  double tempcnta = 90 + stateA->decorationSegmentAngle;
  double tempdima = stateA->endRadius;
  double tempdimb = stateA->decorationSegmentAmplitude;
  x = stateA->endRadius * cos(stateA->decorationSegmentAngle) + 0.25 * tempdima * cos(tempdimb);
  x = stateA->endRadius * sin(stateA->decorationSegmentAngle) + 0.25 * tempdima * sin(tempdimb);
  stateA->curveTo(x,y,1.25 * tempdima,-0.5 * tempdimb, 1.5 * tempdima, -0.5 * tempdimb);
  stateA->lineTo(1.5 * tempdima, 0.5 * tempdimb);
  tempcnta = stateA->decorationSegmentAngle - 90;
  x = stateA->endRadius * cos(stateA->decorationSegmentAngle) + 0.25 * tempdima * cos(tempdimb);
  x = stateA->endRadius * sin(stateA->decorationSegmentAngle) + 0.25 * tempdima * sin(tempdimb);
  double xa = stateA->endRadius * cos(stateA->decorationSegmentAngle);
  double ya = stateA->endRadius * sin(stateA->decorationSegmentAngle);
  stateA->curveTo(1.25 * tempdima, 0.5 * tempdimb, x, y, xa, ya);
  stateA->closePath();

  current_state = &XWTikzDecoration::nullState;
}

void XWTikzDecoration::doBentBentState(XWTikzState * stateA)
{
  stateA->decorateWidth = stateA->decoratedInputSegmentRemainingDistance;
  double x = stateA->decorationSegmentAspect * stateA->decoratedInputSegmentRemainingDistance;
  double y = stateA->decorationSegmentAmplitude;
  QPointF c1(x,y);

  x = stateA->decoratedInputSegmentRemainingDistance * (1 - stateA->decorationSegmentAspect);
  QPointF c2(x,y);

  QPointF e(stateA->decoratedInputSegmentRemainingDistance, 0);
  stateA->curveTo(c1,c2,e);
}

void XWTikzDecoration::doBentFinalState(XWTikzState * )
{
  current_state = &XWTikzDecoration::nullState;
}

void XWTikzDecoration::doBumpsInitState(XWTikzState * stateA)
{
  stateA->autoEndOnLength = 0.51 * stateA->decorationSegmentLength;
  decorateAutoEnd(stateA,&XWTikzDecoration::doBumpsFinalState);
  stateA->autoCornerOnLength = 0.51 * stateA->decorationSegmentLength;
  decorateAutoCorner(stateA);
  stateA->decorateWidth = 0.5 * stateA->decorationSegmentLength;
  double x = 0;
  double y = .555 * stateA->decorationSegmentAmplitude;
  QPointF c1(x,y);
  x = 0.11125 * stateA->decorationSegmentLength;
  y = stateA->decorationSegmentAmplitude;
  QPointF c2(x,y);
  x = 0.25 * stateA->decorationSegmentLength;
  QPointF e1(x,y);
  stateA->curveTo(c1,c2,e1);

  x = 0.38875 * stateA->decorationSegmentLength;
  QPointF c3(x,y);
  x = 0.5 * stateA->decorationSegmentLength;
  y = 0.5 * stateA->decorationSegmentAmplitude;
  QPointF c4(x,y);
  y = 0;
  QPointF e2(x,y);
  stateA->curveTo(c1,c2,e2);
}

void XWTikzDecoration::doBumpsFinalState(XWTikzState * stateA)
{
  QPointF p = stateA->getDecoratedPathLast();
  stateA->lineTo(p);
  current_state = &XWTikzDecoration::nullState;
}

#define XW_TIKZ_POINTONCOIL(a,b,c) \
  x = a * stateA->decorationSegmentAspect * stateA->decorationSegmentLength;\
  x += 0.083333333333 * c * stateA->decorationSegmentLength;\
  y = b * stateA->decorationSegmentAmplitude;

void XWTikzDecoration::doCoilCoilState(XWTikzState * stateA)
{
  double x = 1.5 * stateA->decorationSegmentLength + 2 * stateA->decorationSegmentAspect * stateA->decorationSegmentAmplitude;
  if (stateA->decoratedRemainingDistance < x)
    current_state = &XWTikzDecoration::doCoilLastState;
  stateA->decorateWidth = stateA->decorationSegmentLength;
  double y;
  XW_TIKZ_POINTONCOIL(0,0.555,1)
  QPointF c1(x,y);
  XW_TIKZ_POINTONCOIL(0.445,1,2)
  QPointF c2(x,y);
  XW_TIKZ_POINTONCOIL(1,1,3)
  QPointF e1(x,y);
  stateA->curveTo(c1,c2,e1);

  XW_TIKZ_POINTONCOIL(1.555,1,4)
  QPointF c3(x,y);
  XW_TIKZ_POINTONCOIL(2,0.555,5)
  QPointF c4(x,y);
  XW_TIKZ_POINTONCOIL(2,0,6)
  QPointF e2(x,y);
  stateA->curveTo(c3,c4,e2);

  XW_TIKZ_POINTONCOIL(2,-0.555,7)
  QPointF c5(x,y);
  XW_TIKZ_POINTONCOIL(1.555,-1,8)
  QPointF c6(x,y);
  XW_TIKZ_POINTONCOIL(1,-1,9)
  QPointF e3(x,y);
  stateA->curveTo(c5,c6,e3);

  XW_TIKZ_POINTONCOIL(0.445,-1,10)
  QPointF c7(x,y);
  XW_TIKZ_POINTONCOIL(0,-0.555,11)
  QPointF c8(x,y);
  XW_TIKZ_POINTONCOIL(0,0,12)
  QPointF e4(x,y);
  stateA->curveTo(c7,c8,e4);
}

void XWTikzDecoration::doCoilLastState(XWTikzState * stateA)
{
  double x = 0.5 * stateA->decorationSegmentLength + 2 * stateA->decorationSegmentAspect * stateA->decorationSegmentAmplitude;
  stateA->decorateWidth = x;
  double y;
  XW_TIKZ_POINTONCOIL(0,0.555,1)
  QPointF c1(x,y);
  XW_TIKZ_POINTONCOIL(0.445,1,2)
  QPointF c2(x,y);
  XW_TIKZ_POINTONCOIL(1,1,3)
  QPointF e1(x,y);
  stateA->curveTo(c1,c2,e1);

  XW_TIKZ_POINTONCOIL(1.555,1,4)
  QPointF c3(x,y);
  XW_TIKZ_POINTONCOIL(2,0.555,5)
  QPointF c4(x,y);
  XW_TIKZ_POINTONCOIL(2,0,6)
  QPointF e2(x,y);
  stateA->curveTo(c3,c4,e2);
  current_state = &XWTikzDecoration::doCoilFinalState;
}

void XWTikzDecoration::doCoilFinalState(XWTikzState * stateA)
{
  QPointF p = stateA->getDecoratedPathLast();
  stateA->lineTo(p);
  current_state = &XWTikzDecoration::nullState;
}

void XWTikzDecoration::doLineToInitState(XWTikzState * stateA)
{
  stateA->decorateWidth = stateA->decoratedRemainingDistance;
  if (stateA->decoratedInputSegmentRemainingDistance < stateA->decoratedRemainingDistance)
    current_state = &XWTikzDecoration::doLineToInputSegmentState;
}

void XWTikzDecoration::doLineToInputSegmentState(XWTikzState * stateA)
{
  stateA->decorateWidth = stateA->decoratedInputSegmentRemainingDistance;
  QPointF p = stateA->getDecorateInputSegmentLast();
  stateA->lineTo(p);
  current_state = &XWTikzDecoration::doLineToInitState;
}

void XWTikzDecoration::doLineToFinalState(XWTikzState * stateA)
{
  QPointF p = stateA->getDecoratedPathLast();
  stateA->lineTo(p);
  current_state = &XWTikzDecoration::nullState;
}

void XWTikzDecoration::doMoveToInitState(XWTikzState * stateA)
{
  stateA->decorateWidth = stateA->decoratedRemainingDistance;
}

void XWTikzDecoration::doMoveToFinalState(XWTikzState * stateA)
{
  QPointF p = stateA->getDecoratedPathLast();
  stateA->moveTo(p);
  current_state = &XWTikzDecoration::nullState;
}

void XWTikzDecoration::doCurveToInitState(XWTikzState * stateA)
{
  stateA->decorateWidth = stateA->decoratedInputSegmentLength / 100;
  QPointF p(0,0);
  stateA->lineTo(p);
}

void XWTikzDecoration::doCurveToFinalState(XWTikzState * stateA)
{
  QPointF p = stateA->getDecoratedPathLast();
  stateA->lineTo(p);
  current_state = &XWTikzDecoration::nullState;
}

void XWTikzDecoration::doMarkingsInitState(XWTikzState * stateA)
{
  stateA->decorateWidth = 0;
  sequenceNumber = 0;
  distanceFromStart = 0;
  computedWidth = 0;
  cur = -1;
  stateA->position = 0;
  stateA->startPosition = 0;
  stateA->endPosition = 0;
  stateA->step = 0;
}

void XWTikzDecoration::doMarkingsPreState(XWTikzState * stateA)
{
  computedWidth = 0;
  cur++;
  if (cur >= 0 && cur < stateA->codes.size())
  {
    if (!stateA->position && !stateA->startPosition)
    {
      stateA->codes[cur]->doPath(stateA,false);
      if (stateA->position)
        position = stateA->position->getResult(stateA);
      else
      {
        markStart = stateA->startPosition->getResult(stateA);
        markEnd = stateA->endPosition->getResult(stateA);
        step = stateA->step->getResult(stateA);
      }
    }

    if (stateA->position)
    {
      if (stateA->position->isDim())
      {
        if (position < 0)
          stateA->decoratedPathLength = position;
        computedWidth = position;
      }
      else
      {
        if (position < 0)
        {
          computedWidth = position * stateA->decoratedPathLength;
          stateA->decoratedPathLength = computedWidth;
        }
        else
          computedWidth = position * stateA->decoratedPathLength;
      }
    }
    else
    {
      if (stateA->startPosition->isDim())
      {
        if (markStart < 0)
          stateA->decoratedPathLength = markStart;
        computedWidth = markStart;
      }
      else
      {
        if (markStart < 0)
        {
          computedWidth = markStart * stateA->decoratedPathLength;
          stateA->decoratedPathLength = computedWidth;
        }
        else
          computedWidth = markStart * stateA->decoratedPathLength;
      }

      markStart = computedWidth;

      if (stateA->endPosition->isDim())
      {
        if (markEnd < 0)
          stateA->decoratedPathLength = markEnd;
        computedWidth = markEnd;
      }
      else
      {
        if (markEnd < 0)
        {
          computedWidth = markEnd * stateA->decoratedPathLength;
          stateA->decoratedPathLength = computedWidth;
        }
        else
          computedWidth = markEnd * stateA->decoratedPathLength;
      }

      markEnd = computedWidth;

      if (markEnd >= markStart)
      {
        if (stateA->step->isDim())
        {
          if (step < 0)
            stateA->decoratedPathLength = step;
          computedWidth = step;
        }
        else
        {
          if (step < 0)
          {
            computedWidth = step * stateA->decoratedPathLength;
            stateA->decoratedPathLength = computedWidth;
          }
          else
            computedWidth = step * stateA->decoratedPathLength;
        }

        markStart += step;
        computedWidth = markStart;
      }
    }
  }

  current_state = &XWTikzDecoration::doMarkingsSkipperState;
}

void XWTikzDecoration::doMarkingsSkipperState(XWTikzState * stateA)
{
  stateA->decorateWidth = computedWidth - stateA->decoratedCompletedDistance - 1;
  if (cur >= stateA->codes.size())
    current_state = &XWTikzDecoration::doMarkingsFinalState;
  else
  {
    if (stateA->position)
    {
      stateA->position = 0;
      current_state = &XWTikzDecoration::doMarkingsMainState;
    }
    else
    {
      if (markEnd < markStart)
      {
        stateA->startPosition = 0;
        current_state = &XWTikzDecoration::doMarkingsFinalState;
      }
      else
      {
        cur--;
        current_state = &XWTikzDecoration::doMarkingsMainState;
      }        
    }
  }
}

void XWTikzDecoration::doMarkingsMainState(XWTikzState * stateA)
{
  stateA->decorateWidth = 1;
  sequenceNumber++;
  distanceFromStart = computedWidth;
  stateA->runCodes();
  if (!stateA->markNode.isEmpty())
  {
    QPointF p = stateA->graphic->getPoint(stateA->markNode);
    QPointF ur1(-1,0), ur2(1,0);
    QPointF a = XWTikzShape::pointBorderRectangle(p,ur1);
    stateA->lineTo(a);
    QPointF b = XWTikzShape::pointBorderRectangle(p,ur2);
    stateA->moveTo(b);
  }
  current_state = &XWTikzDecoration::doMarkingsPreState;
}

void XWTikzDecoration::doMarkingsFinalState(XWTikzState * stateA)
{
  stateA->decorateWidth = stateA->decoratedRemainingDistance;
  QPointF p = stateA->getDecoratedPathLast();
  if (stateA->markNode.isEmpty())
    stateA->moveTo(p);
  else
    stateA->lineTo(p);
  current_state = &XWTikzDecoration::nullState;
}

void XWTikzDecoration::doRandomStepsStartState(XWTikzState * stateA)
{
  stateA->decorateWidth += 0;
  current_state = &XWTikzDecoration::doRandomStepsStepState;  
}

void XWTikzDecoration::doRandomStepsStepState(XWTikzState * stateA)
{
  stateA->autoEndOnLength = 1.5 * stateA->decorationSegmentLength;
  decorateAutoEnd(stateA, &XWTikzDecoration::doRandomStepsFinalState);
  stateA->autoCornerOnLength = 1.5 * stateA->decorationSegmentLength;
  decorateAutoCorner(stateA);
  stateA->decorateWidth = stateA->decorationSegmentLength;
  int z = stateA->mathDefaultZ();
  double rand = stateA->mathrand(z);
  double x = stateA->decorationSegmentLength + rand * stateA->decorationSegmentAmplitude;
  double y = rand * stateA->decorationSegmentAmplitude;
  stateA->lineTo(x,y);
}

void XWTikzDecoration::doRandomStepsFinalState(XWTikzState * )
{
  current_state = &XWTikzDecoration::nullState;
}

void XWTikzDecoration::doSawInitState(XWTikzState * stateA)
{
  stateA->autoEndOnLength += stateA->decorationSegmentLength;
  decorateAutoEnd(stateA, &XWTikzDecoration::doSawFinalState);
  stateA->autoCornerOnLength += stateA->decorationSegmentLength;
  decorateAutoCorner(stateA);
  stateA->decorateWidth = stateA->decorationSegmentLength;
  stateA->lineTo(stateA->decorationSegmentLength,stateA->decorationSegmentAmplitude);
  stateA->lineTo(stateA->decorationSegmentLength,0);
  current_state = &XWTikzDecoration::doZigzagFinalState;
}

void XWTikzDecoration::doSawFinalState(XWTikzState * )
{
  current_state = &XWTikzDecoration::nullState;
}

void XWTikzDecoration::doSnakeInitState(XWTikzState * stateA)
{
  double x = 0.625 * stateA->decorationSegmentLength;
  if (stateA->decoratedRemainingDistance < x)
    current_state = &XWTikzDecoration::doSnakeFinalState;
  else
    current_state = &XWTikzDecoration::doSnakeDown;
  stateA->decorateWidth = 0.3125 * stateA->decorationSegmentLength;
  x = 0.125 * stateA->decorationSegmentLength;
  double y = 0;
  QPointF c1(x,y);

  x = 0.1875 * stateA->decorationSegmentLength;
  y = stateA->decorationSegmentAmplitude;
  QPointF c2(x,y);

  x = 0.3125 * stateA->decorationSegmentLength;
  QPointF e(x,y);

  stateA->curveTo(c1,c2,e);
}

void XWTikzDecoration::doSnakeDown(XWTikzState * stateA)
{
  double x = 0.8125 * stateA->decorationSegmentLength;
  if (stateA->decoratedRemainingDistance < x)
    current_state = &XWTikzDecoration::doSnakeEndDown;
  else
    current_state = &XWTikzDecoration::doSnakeUp;
  x = 0.5 * stateA->decorationSegmentLength;
  stateA->decorateWidth = x;
  x = 0.25 * stateA->decorationSegmentLength;
  double y = -stateA->decorationSegmentAmplitude;
  QPointF p(x,y);
  stateA->addCosine(p);
  stateA->addSine(p);
}

void XWTikzDecoration::doSnakeUp(XWTikzState * stateA)
{
  double x = 0.8125 * stateA->decorationSegmentLength;
  if (stateA->decoratedRemainingDistance < x)
    current_state = &XWTikzDecoration::doSnakeEndUp;
  else
    current_state = &XWTikzDecoration::doSnakeDown;
  x = 0.5 * stateA->decorationSegmentLength;
  stateA->decorateWidth = x;
  x = 0.25 * stateA->decorationSegmentLength;
  double y = stateA->decorationSegmentAmplitude;
  QPointF p(x,y);
  stateA->addCosine(p);
  stateA->addSine(p);
}

void XWTikzDecoration::doSnakeEndDown(XWTikzState * stateA)
{
  double x = 0.3125 * stateA->decorationSegmentLength;
  stateA->decorateWidth = x;
  x = 0.125 * stateA->decorationSegmentLength;
  double y = stateA->decorationSegmentAmplitude;
  QPointF c1(x,y);
  x = 0.1875 * stateA->decorationSegmentLength;
  y = 0;
  QPointF c2(x,y);
  x = 0.3125 * stateA->decorationSegmentLength;
  QPointF e(x,y);
  stateA->curveTo(c1,c2,e);
  current_state = &XWTikzDecoration::doSnakeFinalState;
}

void XWTikzDecoration::doSnakeEndUp(XWTikzState * stateA)
{
  double x = 0.3125 * stateA->decorationSegmentLength;
  stateA->decorateWidth = x;
  x = 0.125 * stateA->decorationSegmentLength;
  double y = -stateA->decorationSegmentAmplitude;
  QPointF c1(x,y);
  x = 0.1875 * stateA->decorationSegmentLength;
  y = 0;
  QPointF c2(x,y);
  x = 0.3125 * stateA->decorationSegmentLength;
  QPointF e(x,y);
  stateA->curveTo(c1,c2,e);
  current_state = &XWTikzDecoration::doSnakeFinalState;
}

void XWTikzDecoration::doSnakeFinalState(XWTikzState * stateA)
{
  QPointF p = stateA->getDecoratedPathLast();
  stateA->lineTo(p);
  current_state = &XWTikzDecoration::nullState;
}

void XWTikzDecoration::doStraightZigzagLineToState(XWTikzState * stateA)
{
  stateA->decorateWidth = stateA->decorationSegmentLength;
  doCurveToInitState(stateA);
  current_state = &XWTikzDecoration::doStraightZigzagZigzagState;
}

void XWTikzDecoration::doStraightZigzagZigzagState(XWTikzState * stateA)
{
  stateA->decorateWidth = stateA->decorationSegmentLength;
  doZigzagUpFromCenterState(stateA);
  current_state = &XWTikzDecoration::doStraightZigzagLineToState;
}

void XWTikzDecoration::doStraightZigzagFinalState(XWTikzState * stateA)
{
  doCurveToInitState(stateA);
  current_state = &XWTikzDecoration::nullState;
}

void XWTikzDecoration::doZigzagUpFromCenterState(XWTikzState * stateA)
{
  stateA->decorateWidth += 0.5 * stateA->decorationSegmentLength;
  double x = 0.25 * stateA->decorationSegmentLength;
  double y = stateA->decorationSegmentAmplitude;
  stateA->lineTo(x,y);
  current_state = &XWTikzDecoration::doZigzagBigDownState;
}

void XWTikzDecoration::doZigzagBigDownState(XWTikzState * stateA)
{
  double x = 0.5 * stateA->decorationSegmentLength;
  if (stateA->decoratedRemainingDistance < x)
    current_state = &XWTikzDecoration::doZigzagCenterFinishState;
  else
    current_state = &XWTikzDecoration::doZigzagBigUpState;
  stateA->decorateWidth += x;
  x = 0.25 * stateA->decorationSegmentLength;
  double y = stateA->decorationSegmentAmplitude;
  stateA->lineTo(x,y);
}

void XWTikzDecoration::doZigzagBigUpState(XWTikzState * stateA)
{
  double x = 0.5 * stateA->decorationSegmentLength;
  if (stateA->decoratedRemainingDistance < x)
    current_state = &XWTikzDecoration::doZigzagCenterFinishState;
  else
    current_state = &XWTikzDecoration::doZigzagBigDownState;
  stateA->decorateWidth += x;
  x = 0.25 * stateA->decorationSegmentLength;
  double y = stateA->decorationSegmentAmplitude;
  stateA->lineTo(x,y);
}

void XWTikzDecoration::doZigzagCenterFinishState(XWTikzState * stateA)
{
  stateA->lineTo(0,0);
  current_state = &XWTikzDecoration::doZigzagFinalState;
}

void XWTikzDecoration::doZigzagFinalState(XWTikzState * stateA)
{
  QPointF p = stateA->getDecoratedPathLast();
  stateA->lineTo(p);
  current_state = &XWTikzDecoration::nullState;
}


void XWTikzDecoration::decorateAutoCorner(XWTikzState * stateA)
{
  if (stateA->pathHasCorners)
  {
    if (stateA->decoratedInputSegmentRemainingDistance <= stateA->autoCornerOnLength)
    {
      stateA->lineTo(stateA->decoratedInputSegmentRemainingDistance,0);
      stateA->decoratedCompletedDistance += stateA->decoratedInputSegmentRemainingDistance;
      stateA->decoratedRemainingDistance -= stateA->decoratedInputSegmentRemainingDistance;
    }
  }
}

void XWTikzDecoration::decorateAutoEnd(XWTikzState * stateA,void (XWTikzDecoration::*final)(XWTikzState *))
{
  if (stateA->curOperation == XW_TIKZ_CLOSE)
  {
    if (stateA->decoratedRemainingDistance > stateA->autoEndOnLength)
    {
      if (stateA->decoratedInputSegmentRemainingDistance <= stateA->autoEndOnLength)
      {
        stateA->closePath();
        stateA->decoratedCompletedDistance += stateA->decoratedInputSegmentRemainingDistance;
        stateA->decoratedRemainingDistance -= stateA->decoratedInputSegmentRemainingDistance;
      }
    }
    else
    {
      stateA->closePath();
      final_state = final;
    }
  }
  else
  {
    if (stateA->decoratedRemainingDistance <= stateA->autoEndOnLength)
      stateA->lineTo(stateA->decoratedRemainingDistance,0);
    final_state = final;

    stateA->decorateWidth = stateA->decoratedRemainingDistance;
  }
}

void XWTikzDecoration::nullState(XWTikzState * )
{}
