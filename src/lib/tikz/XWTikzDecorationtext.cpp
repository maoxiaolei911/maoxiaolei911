/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include <QPointF>
#include "PGFKeyWord.h"
#include "XWPDFDriver.h"
#include "XWTikzGraphic.h"
#include "XWTikzTextBox.h"
#include "XWTikzShape.h"
#include "XWTikzExpress.h"
#include "XWTikzState.h"
#include "XWTikzOption.h"
#include "XWTikzDecoration.h"

void XWTikzDecoration::doTextAlongPathInitState(XWTikzState * stateA)
{
  stateA->decorateWidth = 0;
  stateA->decoratedRemainingDistance = stateA->decoratedPathLength - stateA->rightIndent;
  stateA->decoratedPathLength = stateA->decoratedRemainingDistance;
  box = new XWTikzTextBox(stateA->graphic,stateA);
  int len = stateA->text.length();
  int pos = 0;
  box->scan(stateA->text, len, pos);
  double x = box->getWidth(stateA->textWidth);
  double y = stateA->decoratedRemainingDistance;
  if (stateA->fitToPath)
  {
    y -= stateA->leftIndent;
    y -= x;
    if (stateA->fitToPathStretchingSpaces)
    {
      int n = box->getSpaceNumber();
      y /= n;
      spaceShift = y;
    }
    else
    {
      int n = box->getCharacterNumber() - box->getSpaceNumber();
      n--;
      y /= n;
      characterShift = y;
    }

    if (y < 0)
    {
      spaceShift = 0;
      characterShift = 0;
    }
  }
  else
  {
    if (stateA->align != PGFleft)
    {
      if (stateA->align == PGFright)
        y -= x;
      else
      {
        y -= x;
        y -= stateA->leftIndent;
        y *= 0.5;
        y += stateA->leftIndent;
        indentLeft = y;
      }
    }
  }
  current_state = &XWTikzDecoration::doTextAlongPathLeftIndentState;
}

void XWTikzDecoration::doTextAlongPathLeftIndentState(XWTikzState * stateA)
{
  stateA->decorateWidth = indentLeft;
  current_state = &XWTikzDecoration::doTextAlongPathScanState;
}

void XWTikzDecoration::doTextAlongPathScanState(XWTikzState * stateA)
{
  stateA->decorateWidth = 0;
  cur++;
  current_state = &XWTikzDecoration::doTextAlongPathBeforeTypesetState;
}

void XWTikzDecoration::doTextAlongPathBeforeTypesetState(XWTikzState * stateA)
{
  stateA->decorateWidth = 0.5 * box->getCharWidth(cur);
  current_state = &XWTikzDecoration::doTextAlongPathTypesetState;
}

void XWTikzDecoration::doTextAlongPathTypesetState(XWTikzState * stateA)
{
  stateA->decorateWidth = 0;
  stateA->shift(-0.5 * box->getCharWidth(cur), 0);
  QTransform transA = stateA->transform;
  stateA->driver->setTextMatrix(transA.m11(),transA.m12(),transA.m21(),transA.m22(),transA.dx(),transA.dy());
  box->drawChar(stateA->driver, cur);
  current_state = &XWTikzDecoration::doTextAlongPathAfterTypesetState;
}

void XWTikzDecoration::doTextAlongPathAfterTypesetState(XWTikzState * stateA)
{
  if (stateA->fitToPath)
  {
    if (stateA->fitToPathStretchingSpaces)
    {
      if (box->isSpace(cur + 1))
        textShift = spaceShift;
      else
        textShift = 0;
    }
    else
      textShift = characterShift;
  }
  else
    textShift = 0;
  current_state = &XWTikzDecoration::doTextAlongPathShitState;
}

void XWTikzDecoration::doTextAlongPathShitState(XWTikzState * stateA)
{
  stateA->decorateWidth = textShift;
  current_state = &XWTikzDecoration::doTextAlongPathScanState;
}

void XWTikzDecoration::doTextAlongPathFinalState(XWTikzState * )
{
  current_state = &XWTikzDecoration::nullState;
}
