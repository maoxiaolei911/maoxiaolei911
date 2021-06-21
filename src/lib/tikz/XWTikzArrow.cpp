/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "LaTeXKeyWord.h"
#include "PGFKeyWord.h"
#include "XWPGFPool.h"
#include "XWTikzState.h"
#include "XWTikzArrow.h"

XWTikzArrow::XWTikzArrow()
:shape(-1)
{}

void XWTikzArrow::doArrow(XWTikzState * state)
{
  state = state->save(false);
  switch (shape)
  {
    default:
      break;

    case PGFstealth:      
      stealthArrow(state);  
      break;

    case PGFto:
      toArrow(state);
      break;

    case PGFtoreversed:
      toReversedArrow(state);
      break;

    case PGFlatex:
      latexArrow(state);
      break;

    case PGFbar:
      barArrow(state);
      break;
  }

  state = state->restore();
}

void XWTikzArrow::initPath(XWTikzState * state, bool isend)
{
  double tmpa = 0;
  double tmpb = 0;
  switch (shape)
  {
    default:
      break;

    case PGFstealth:
      tmpa = 0.28;
      tmpb = state->lineWidth;
      if (state->innerLineWidth > 0)
        tmpb = 0.6 * state->lineWidth - 0.4 * state->innerLineWidth;
      tmpa = tmpa + 0.3 * tmpb;
      if (isend)
        state->rightExtend = 5 * tmpa;
      else
        state->leftExtend = -3 * tmpa;
      break;

    case PGFto:
      if (isend)
      {
        tmpb = 0.21 + 0.625 * state->lineWidth;
        state->rightExtend = 0.21 + 0.625 * state->lineWidth;
      }
      else
        state->leftExtend = -0.84 - 1.3 * state->lineWidth;
      break;

    case PGFtoreversed:
      if (isend)
        state->rightExtend = 0.98 + 1.45 * state->lineWidth;
      else
        state->leftExtend = -0.21 -0.475 * state->lineWidth;
      break;

    case PGFlatex:
      tmpa = 0.28;
      tmpb = state->lineWidth;
      if (state->innerLineWidth > 0)
        tmpb = 0.6 * state->lineWidth - 0.4 * state->innerLineWidth;
      tmpa = tmpa + 0.3 * tmpb;
      if (isend)
        state->rightExtend = 9 * tmpa;
      else
        state->leftExtend = -tmpa;
      break;

    case PGFbar:
      if (isend)
        state->rightExtend = 0.75 * state->innerLineWidth;
      else
        state->leftExtend = -0.25 * state->innerLineWidth;
      break;

    case PGFspace:
      state->rightExtend = 0.88;
      break;
  }
}

void XWTikzArrow::barArrow(XWTikzState * state)
{
  double a = 2 + 1.5 * state->lineWidth;
  state->moveTo(0.25*state->lineWidth, -a);
  state->lineTo(0.25*state->lineWidth, a);
  state->setDraw(true);
}

void XWTikzArrow::latexArrow(XWTikzState * state)
{
  double a = 0.28;
  double b = state->lineWidth;
  if (state->innerLineWidth > 0)
    b = 0.6 * state->lineWidth - 0.4 * state->innerLineWidth;
  a = a + 0.3 * b;
  state->moveTo(9*a, 0);
  state->curveTo(6.3333*a, 0.5*a, 2*a, 2*a, -a, 3.75*a);
  state->lineTo(-a, -3.75*a);
  state->curveTo(2*a, -2*a, 6.3333*a, -0.5*a, 9*a, 0);
  state->closePath();
  state->setFill(true);
}

void XWTikzArrow::stealthArrow(XWTikzState * state)
{
  double a = 0.28;
  double b = state->lineWidth;
  if (state->innerLineWidth > 0)
    b = 0.6 * state->lineWidth - 0.4 * state->innerLineWidth;
  a = a + 0.3 * b;
  state->moveTo(5*a, 0);
  state->lineTo(-3 * a, 4 * a);
  state->lineTo(0, 0);
  state->lineTo(-3 * a, -4 * a);
  state->closePath();
  state->setFill(true);
}

void XWTikzArrow::toArrow(XWTikzState * state)
{
  double a = 0.28 + 0.3 * state->lineWidth;
  state->lineWidth = 0.8 * state->lineWidth;
  state->moveTo(-3 * a, 4 * a);
  state->curveTo(-2.75*a, 2.5*a, 0, 0.25*a, 0.75*a, 0);
  state->curveTo(0, -0.25*a, -2.75*a, -2.5*a, -3*a, -4*a);
  state->setDraw(true);
}

void XWTikzArrow::toReversedArrow(XWTikzState * state)
{
  double a = 0.28 + 0.3 * state->lineWidth;
  state->lineWidth = 0.8 * state->lineWidth;
  state->moveTo(3.5 * a, 4 * a);
  state->curveTo(3.25*a, 2.5*a, 0.5*a, 0.25*a, -0.25*a, 0);
  state->curveTo(0.5*a, -0.25*a, 3.25*a, -2.5*a, 3.5*a, -4*a);
  state->setDraw(true);
}

