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

void XWTikzDecoration::doTicksTicksState(XWTikzState * stateA)
{
  stateA->decorateWidth = stateA->decorationSegmentLength;
  stateA->moveTo(0, stateA->decorationSegmentAmplitude);
  stateA->lineTo(0, -stateA->decorationSegmentAmplitude);
  current_state = &XWTikzDecoration::doTicksFinalState;
}

void XWTikzDecoration::doTicksFinalState(XWTikzState * stateA)
{
  stateA->moveTo(0, stateA->decorationSegmentAmplitude);
  stateA->lineTo(0, -stateA->decorationSegmentAmplitude);
  stateA->moveTo(stateA->decoratedPathLast);
  current_state = &XWTikzDecoration::doTicksTicksState;
}

void XWTikzDecoration::doExpandingWavesInitState(XWTikzState * stateA)
{
  stateA->decorateWidth = stateA->decorationSegmentLength;
  current_state = &XWTikzDecoration::doExpandingWavesWaveState;
}

void XWTikzDecoration::doExpandingWavesWaveState(XWTikzState * stateA)
{
  stateA->decorateWidth = stateA->decorationSegmentLength;
  if (stateA->decoratedRemainingDistance < stateA->decorationSegmentLength)
    current_state = &XWTikzDecoration::doExpandingWavesLastState;

  double x = -stateA->decoratedCompletedDistance + stateA->decoratedCompletedDistance * cos(stateA->decorationSegmentAngle);
  double y = stateA->decoratedCompletedDistance * sin(stateA->decorationSegmentAngle);
  stateA->moveTo(x, y);
  stateA->setStartAngle(stateA->decorationSegmentAngle);
  stateA->setEndAngle(-stateA->decorationSegmentAngle);
  stateA->setRadius(stateA->decoratedCompletedDistance);
  stateA->addArc();
}

void XWTikzDecoration::doExpandingWavesLastState(XWTikzState * stateA)
{
  double x = -stateA->decoratedCompletedDistance + stateA->decoratedCompletedDistance * cos(stateA->decorationSegmentAngle);
  double y = stateA->decoratedCompletedDistance * sin(stateA->decorationSegmentAngle);
  stateA->moveTo(x, y);
  stateA->setStartAngle(stateA->decorationSegmentAngle);
  stateA->setEndAngle(-stateA->decorationSegmentAngle);
  stateA->setRadius(stateA->decoratedCompletedDistance);
  stateA->addArc();
  current_state = &XWTikzDecoration::doExpandingWavesFinalState;
}

void XWTikzDecoration::doExpandingWavesFinalState(XWTikzState * stateA)
{
  stateA->moveTo(stateA->decoratedPathLast);
  current_state = &XWTikzDecoration::nullState;
}

void XWTikzDecoration::doWavesInitState(XWTikzState * stateA)
{
  stateA->decorateWidth = stateA->decorationSegmentLength;
  stateA->shift(0, stateA->decorationSegmentLength);
  double x = -stateA->startRadius + stateA->startRadius * cos(stateA->decorationSegmentAngle);
  double y = stateA->startRadius * sin(stateA->decorationSegmentAngle);
  stateA->moveTo(x, y);
  stateA->setStartAngle(stateA->decorationSegmentAngle);
  stateA->setEndAngle(-stateA->decorationSegmentAngle);
  stateA->setRadius(stateA->startRadius);
  stateA->addArc();
  current_state = &XWTikzDecoration::doWavesFinalState;
}

void XWTikzDecoration::doWavesFinalState(XWTikzState * stateA)
{
  stateA->moveTo(stateA->decoratedPathLast);
  current_state = &XWTikzDecoration::nullState;
}

void XWTikzDecoration::doBorderTickState(XWTikzState * stateA)
{
  stateA->decorateWidth = stateA->decorationSegmentLength;
  if (stateA->decoratedRemainingDistance < stateA->decorationSegmentLength)
    current_state = &XWTikzDecoration::doBorderLastState;

  stateA->moveTo(0, 0);
  double x = stateA->decorationSegmentAmplitude * cos(stateA->decorationSegmentAngle);
  double y = stateA->decorationSegmentAmplitude * sin(stateA->decorationSegmentAngle);
  stateA->lineTo(x,y);
}

void XWTikzDecoration::doBorderLastState(XWTikzState * stateA)
{
  stateA->decorateWidth = stateA->decorationSegmentAmplitude;
  stateA->moveTo(0, 0);
  double x = stateA->decorationSegmentAmplitude * cos(stateA->decorationSegmentAngle);
  double y = stateA->decorationSegmentAmplitude * sin(stateA->decorationSegmentAngle);
  stateA->lineTo(x,y);
  current_state = &XWTikzDecoration::doBorderFinalState;
}

void XWTikzDecoration::doBorderFinalState(XWTikzState * stateA)
{
  stateA->moveTo(stateA->decoratedPathLast);
  current_state = &XWTikzDecoration::nullState;
}

void XWTikzDecoration::doBraceBraceState(XWTikzState * stateA)
{
  stateA->decorateWidth = stateA->decoratedRemainingDistance;
  double yc = stateA->decorationSegmentAspect * stateA->decoratedRemainingDistance;
  if (2 * stateA->decorationSegmentAmplitude > yc)
    yc = 0.5 * yc;
  else
    yc = stateA->decorationSegmentAmplitude;
  double xc = stateA->decorationSegmentAspect * stateA->decoratedRemainingDistance - stateA->decoratedRemainingDistance;
  if (-2 * stateA->decorationSegmentAmplitude < xc)
    xc = -0.5 * xc;
  else
    xc = stateA->decorationSegmentAmplitude;
  stateA->moveTo(0, 0);
  stateA->curveTo(0.15*yc,0.3*stateA->decorationSegmentAmplitude, 0.5*yc,0.5*stateA->decorationSegmentAmplitude,yc,0.5*stateA->decorationSegmentAmplitude);
  stateA->shift(stateA->decorationSegmentAspect * stateA->decoratedRemainingDistance,0);
  stateA->lineTo(-yc, 0.5 * stateA->decorationSegmentAmplitude);
  stateA->curveTo(-0.5*yc,0.5*stateA->decorationSegmentAmplitude,-0.15*yc,0.7*stateA->decorationSegmentAmplitude,0,stateA->decorationSegmentAmplitude);
  stateA->curveTo(0.15*xc,0.7*stateA->decorationSegmentAmplitude,0.5*xc,0.5*stateA->decorationSegmentAmplitude,xc,0.5*stateA->decorationSegmentAmplitude);
  stateA->shift(stateA->decoratedRemainingDistance,0);
  stateA->lineTo(-xc,0.5*stateA->decorationSegmentAmplitude);
  stateA->curveTo(-0.5*xc,0.5*stateA->decorationSegmentAmplitude,-0.15*xc,0.3*stateA->decorationSegmentAmplitude,0,0);
  current_state = &XWTikzDecoration::doBraceFinalState;
}

void XWTikzDecoration::doBraceFinalState(XWTikzState * stateA)
{
  current_state = &XWTikzDecoration::nullState;
}
