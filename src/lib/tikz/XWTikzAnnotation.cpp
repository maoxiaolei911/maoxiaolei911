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
#include "XWTikzState.h"
#include "XWTikzOption.h"
#include "XWTikzOptions.h"
#include "XWTikzShape.h"

void XWTikzShape::doAnnotation()
{
  mapPos();
  int kw = state->annotation->getKeyWord();
  state = state->save();
  state->annotation->doPath(state,false);
  state->computeAngle();
  state->isPath = true;
  XWTikzAnnotationArrow arrow(state->graphic);
  arrow.doPath(state,false);
  double x = 0;
  double y = 0;
  switch (kw)
  {
    default:
      break;

    case PGFdirectioninfo:
    case PGFdirectioninfosouth:
      state->labelDistance = 0.5 * state->circuitsSizeUnit;
      if (kw == PGFdirectioninfo)
      {
        x = northPos.x() + state->labelDistance * cos(state->labelAngle);
        y = northPos.y() + state->labelDistance * sin(state->labelAngle);
        state->shift(x,y);
      }
      else
      {
        x = southPos.x() + state->labelDistance * cos(state->labelAngle);
        y = southPos.y() + state->labelDistance * sin(state->labelAngle);
        state->shift(x,y);
        state->scale(1, -1);
      }      
      x = -1.25 * state->circuitsSizeUnit;
      y = 0.3333 * state->circuitsSizeUnit;
      state->moveTo(x,y);
      x = 1.25 * state->circuitsSizeUnit;
      y = 0.3333 * state->circuitsSizeUnit;
      state->lineTo(x,y);
      break;

    case PGFlightemitting:
	  case PGFlightemittingsouth:
      state->labelDistance = 1.75 * state->circuitsSizeUnit;
      if (kw == PGFlightemitting)
      {
        x = northPos.x() + state->labelDistance * cos(state->labelAngle);
        y = northPos.y() + state->labelDistance * sin(state->labelAngle);
        state->shift(x,y);
      }
      else
      {
        x = southPos.x() + state->labelDistance * cos(state->labelAngle);
        y = southPos.y() + state->labelDistance * sin(state->labelAngle);
        state->shift(x,y);
        state->scale(1, -1);
      }  
      x = -0.2 * state->circuitsSizeUnit;
      y = 0.65 * state->circuitsSizeUnit;
      state->moveTo(x,y);
      x += 1.25 * cos((double)45);
      y += 1.25 * sin((double)45);
      state->lineTo(x,y);
      x = 0.2 * state->circuitsSizeUnit;
      y = 0.25 * state->circuitsSizeUnit;
      state->moveTo(x,y);
      x += 1.25 * cos((double)45);
      y += 1.25 * sin((double)45);
      state->lineTo(x,y);
      break;

    case PGFlightDependent:
	  case PGFlightDependentsouth:
      state->labelDistance = 1.75 * state->circuitsSizeUnit;
      if (kw == PGFlightemitting)
      {
        x = northPos.x() + state->labelDistance * cos(state->labelAngle) + 1.25 * cos((double)135);
        y = northPos.y() + state->labelDistance * sin(state->labelAngle) + 1.25 * sin((double)135);
        state->shift(x,y);
      }
      else
      {
        x = southPos.x() + state->labelDistance * cos(state->labelAngle) + 1.25 * cos((double)135);
        y = southPos.y() + state->labelDistance * sin(state->labelAngle) + 1.25 * sin((double)135);
        state->shift(x,y);
        state->scale(1, -1);
      }  
      x = 0.2 * state->circuitsSizeUnit;
      y = 0.65 * state->circuitsSizeUnit;
      state->moveTo(x,y);
      x += 1.25 * cos((double)-45);
      y += 1.25 * sin((double)-45);
      state->lineTo(x,y);
      x = -0.2 * state->circuitsSizeUnit;
      y = 0.25 * state->circuitsSizeUnit;
      state->moveTo(x,y);
      x += 1.25 * cos((double)-45);
      y += 1.25 * sin((double)-45);
      state->lineTo(x,y);
      break;

    case PGFadjustable:
      state->labelDistance = 1.5 * state->circuitsSizeUnit;
      x = centerPos.x() + state->labelDistance * cos(state->labelAngle);
      y = centerPos.y() + state->labelDistance * sin(state->labelAngle);
      state->shift(x,y);
      x = -1.5 * state->circuitsSizeUnit;
      y = -1.5 * state->circuitsSizeUnit;
      state->moveTo(x,y);
      x = 1.5 * state->circuitsSizeUnit;
      y = 1.5 * state->circuitsSizeUnit;
      state->lineTo(x,y);
      break;

	  case PGFadjustablesouth:
      state->labelDistance = 1.5 * state->circuitsSizeUnit;
      x = centerPos.x() + state->labelDistance * cos(state->labelAngle);
      y = centerPos.y() + state->labelDistance * sin(state->labelAngle);
      state->shift(x,y);
      x = -1.5 * state->circuitsSizeUnit;
      y = 1.5 * state->circuitsSizeUnit;
      state->moveTo(x,y);
      x = 1.5 * state->circuitsSizeUnit;
      y = -1.5 * state->circuitsSizeUnit;
      state->lineTo(x,y);
      break;
  }

  state = state->restore();
}
