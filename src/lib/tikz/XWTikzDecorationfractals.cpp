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

void XWTikzDecoration::doKochCurveType1InitState(XWTikzState * stateA)
{
  stateA->decorateWidth = stateA->decoratedInputSegmentRemainingDistance;
  stateA->lineTo(0.33333 * stateA->decoratedInputSegmentRemainingDistance, 0);
  stateA->lineTo(0.33333 * stateA->decoratedInputSegmentRemainingDistance, 0.33333 * stateA->decoratedInputSegmentRemainingDistance);
  stateA->lineTo(0.66666 * stateA->decoratedInputSegmentRemainingDistance, 0.33333 * stateA->decoratedInputSegmentRemainingDistance);
  stateA->lineTo(0.66666 * stateA->decoratedInputSegmentRemainingDistance, 0);
  stateA->lineTo(stateA->decoratedInputSegmentRemainingDistance, 0);
}

void XWTikzDecoration::doKochCurveType2InitState(XWTikzState * stateA)
{
  stateA->decorateWidth = stateA->decoratedInputSegmentRemainingDistance;
  stateA->lineTo(0.25 * stateA->decoratedInputSegmentRemainingDistance, 0);
  stateA->lineTo(0.25 * stateA->decoratedInputSegmentRemainingDistance, 0.25*stateA->decoratedInputSegmentRemainingDistance);
  stateA->lineTo(0.5 * stateA->decoratedInputSegmentRemainingDistance, 0.25 * stateA->decoratedInputSegmentRemainingDistance);
  stateA->lineTo(0.5 * stateA->decoratedInputSegmentRemainingDistance, 0);
  stateA->lineTo(0.5 * stateA->decoratedInputSegmentRemainingDistance, -0.25 * stateA->decoratedInputSegmentRemainingDistance);
  stateA->lineTo(0.75 * stateA->decoratedInputSegmentRemainingDistance, -0.25 * stateA->decoratedInputSegmentRemainingDistance);
  stateA->lineTo(0.75 * stateA->decoratedInputSegmentRemainingDistance, 0);
  stateA->lineTo(stateA->decoratedInputSegmentRemainingDistance, 0);
}

void XWTikzDecoration::doKochSnowFlakeInitState(XWTikzState * stateA)
{
  stateA->decorateWidth = stateA->decoratedInputSegmentRemainingDistance;
  stateA->lineTo(0.3333 * stateA->decoratedInputSegmentRemainingDistance, 0);
  stateA->lineTo(0.5 * stateA->decoratedInputSegmentRemainingDistance, 0.2886751347 * stateA->decoratedInputSegmentRemainingDistance);
  stateA->lineTo(0.6666 * stateA->decoratedInputSegmentRemainingDistance, 0);
  stateA->lineTo(stateA->decoratedInputSegmentRemainingDistance, 0);
}

void XWTikzDecoration::doCantorSetInitState(XWTikzState * stateA)
{
  stateA->decorateWidth = stateA->decoratedInputSegmentRemainingDistance;
  stateA->lineTo(0.3333 * stateA->decoratedInputSegmentRemainingDistance, 0);
  stateA->moveTo(0.6666 * stateA->decoratedInputSegmentRemainingDistance, 0);
  stateA->lineTo(stateA->decoratedInputSegmentRemainingDistance, 0);
}
