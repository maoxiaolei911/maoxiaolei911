/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include "PGFKeyWord.h"
#include "XWTikzTextBox.h"
#include "XWTikzState.h"
#include "XWTikzPlotMark.h"

XWTikzPlotMark::XWTikzPlotMark(int ptypeA)
:ptype(ptypeA)
{}

void XWTikzPlotMark::doMark(XWTikzState * stateA)
{
  switch (ptype)
  {
    default:
      break;

    case PGFopendot:
      doOpenDot(stateA);
      break;

    case PGFMercedesstar:
      doMercedesStar(stateA);
      break;

    case PGFMercedesstarflipped:
      doMercedesStarFlipped(stateA);
      break;

    case PGFasterisk:
      doAsterisk(stateA);
      break;

    case PGFstar:
      doStar(stateA);
      break;

    case PGF10pointedstar:
      doPointed10Star(stateA);
      break;

    case PGFoplus:
      doOPlus(stateA);
      break;

    case PGFoplusstar:
      doOPlusStar(stateA);
      break;

    case PGFotimes:
      doOTimes(stateA);
      break;

    case PGFotimesstar:
      doOTimesStar(stateA);
      break;

    case PGFverticalbar:
      doBarMarkVertical(stateA);
      break;

    case PGFhorizontalbar:
      doBarMarkHorizontal(stateA);
      break;

    case PGFsquare:
      doSquare(stateA);
      break;

    case PGFtriangle:
      doTriangle(stateA);
      break;

    case PGFtrianglestar:
      doTriangleStar(stateA);
      break;

    case PGFdiamond:
      doDiamond(stateA);
      break;

    case PGFdiamondstar:
      doDiamondStar(stateA);
      break;

    case PGFpentagon:
      doPentagon(stateA);
      break;

    case PGFpentagonstar:
      doPentagonStar(stateA);
      break;

    case PGFhalfcircle:
      doHalfCircle(stateA);
      break;

    case PGFhalfcirclestar:
      doHalfCircleStar(stateA);
      break;

    case PGFhalfdiamondstar:
      doHalfDiamondStar(stateA);
      break;

    case PGFhalfsquarestar:
      doHalfSquareStar(stateA);
      break;

    case PGFhalfsquarerightstar:
      doHalfSquareRightStar(stateA);
      break;

    case PGFhalfsquareleftstar:
      doHalfSquareLeftStar(stateA);
      break;

    case PGFheart:
      doHeart(stateA);
      break;

    case PGFball:
      doBall(stateA);
      break;
  }
}

void XWTikzPlotMark::doAsterisk(XWTikzState * stateA)
{
  stateA->moveTo(0, -stateA->markSize);
  stateA->lineTo(0, stateA->markSize);
  stateA->moveTo(stateA->markSize * cos((double)30), stateA->markSize * sin((double)30));
  stateA->lineTo(stateA->markSize * cos((double)210), stateA->markSize * sin((double)210));
  stateA->moveTo(stateA->markSize * cos((double)-30), stateA->markSize * sin((double)-30));
  stateA->lineTo(stateA->markSize * cos((double)-210), stateA->markSize * sin((double)-210));
  stateA->setDraw(true);
}

void XWTikzPlotMark::doBall(XWTikzState * stateA)
{
  double r = stateA->markSize;
  stateA->setRadius(r);
  stateA->moveTo(0,0);
  stateA->addEllipse();
  stateA->setShade(true);
}

void XWTikzPlotMark::doBarMarkHorizontal(XWTikzState * stateA)
{
  stateA->moveTo(stateA->markSize,0);
  stateA->lineTo(-stateA->markSize,0);
  stateA->setDraw(true);
}

void XWTikzPlotMark::doBarMarkVertical(XWTikzState * stateA)
{
  stateA->moveTo(0, stateA->markSize);
  stateA->lineTo(0, -stateA->markSize);
  stateA->setDraw(true);
}

void XWTikzPlotMark::doDiamond(XWTikzState * stateA)
{
  stateA->moveTo(0, stateA->markSize);
  stateA->lineTo(0.75 * stateA->markSize, 0);
  stateA->lineTo(0, -stateA->markSize);
  stateA->lineTo(-0.75 * stateA->markSize, 0);
  stateA->closePath();
  stateA->setDraw(true);
}

void XWTikzPlotMark::doDiamondStar(XWTikzState * stateA)
{
  stateA->moveTo(0, stateA->markSize);
  stateA->lineTo(0.75 * stateA->markSize, 0);
  stateA->lineTo(0, -stateA->markSize);
  stateA->lineTo(-0.75 * stateA->markSize, 0);
  stateA->closePath();
  stateA->setFill(true);
}

void XWTikzPlotMark::doHalfCircle(XWTikzState * stateA)
{
  if (stateA->isMarkColorSet)
  {
    stateA = stateA->save();
    stateA->moveTo(-stateA->markSize,0);
    stateA->setStartAngle(180);
    stateA->setEndAngle(360);
    stateA->setRadius(stateA->markSize);
    stateA->addArc();
    stateA->setFillColor(stateA->markColor);
    stateA = stateA->restore();
  }

  stateA->moveTo(-stateA->markSize,0);
  stateA->lineTo(stateA->markSize, 0);
  QPointF c(0,0);
  stateA->addCircle(c, stateA->markSize);
  stateA->setDraw(true);
}

void XWTikzPlotMark::doHalfCircleStar(XWTikzState * stateA)
{
  stateA = stateA->save();
  stateA->moveTo(stateA->markSize,0);
  stateA->setStartAngle(0);
  stateA->setEndAngle(180);
  stateA->setRadius(stateA->markSize);
  stateA->addArc();
  stateA->setFill(true);
  stateA = stateA->restore();
  if (stateA->isMarkColorSet)
  {
    stateA = stateA->save();
    stateA->moveTo(-stateA->markSize,0);
    stateA->setStartAngle(180);
    stateA->setEndAngle(360);
    stateA->setRadius(stateA->markSize);
    stateA->addArc();
    stateA->setFillColor(stateA->markColor);
    stateA = stateA->restore();
  }
  QPointF c(0,0);
  stateA->addCircle(c, stateA->markSize);
  stateA->setDraw(true);
}

void XWTikzPlotMark::doHalfDiamondStar(XWTikzState * stateA)
{
  stateA = stateA->save();
  stateA->moveTo(-0.75 * stateA->markSize,0);
  stateA->lineTo(0,-stateA->markSize);
  stateA->lineTo(0.75 * stateA->markSize,0);
  stateA->setFill(true);
  stateA = stateA->restore();

  if (stateA->isMarkColorSet)
  {
    stateA = stateA->save();
    stateA->moveTo(0.75 * stateA->markSize,0);
    stateA->lineTo(0,stateA->markSize);
    stateA->lineTo(-0.75 * stateA->markSize,0);
    stateA->setFillColor(stateA->markColor);
    stateA = stateA->restore();
  }

  stateA->moveTo(0.75 * stateA->markSize,0);
  stateA->lineTo(0,stateA->markSize);
  stateA->lineTo(-0.75 * stateA->markSize,0);
  stateA->lineTo(0,-stateA->markSize);
  stateA->closePath();
  stateA->setDraw(true);
}

void XWTikzPlotMark::doHalfSquareLeftStar(XWTikzState * stateA)
{
  stateA = stateA->save();
  stateA->moveTo(0,-stateA->markSize);
  stateA->lineTo(-stateA->markSize, 0);
  stateA->lineTo(0,stateA->markSize);
  stateA->setFill(true);
  stateA = stateA->restore();

  if (stateA->isMarkColorSet)
  {
    stateA = stateA->save();
    stateA->moveTo(0,-stateA->markSize);
    stateA->lineTo(stateA->markSize, 0);
    stateA->lineTo(0,stateA->markSize);
    stateA->setFillColor(stateA->markColor);
    stateA = stateA->restore();
  }

  stateA->moveTo(stateA->markSize,0);
  stateA->lineTo(0,stateA->markSize);
  stateA->lineTo(-stateA->markSize,0);
  stateA->lineTo(0,-stateA->markSize);
  stateA->closePath();
  stateA->setDraw(true);
}

void XWTikzPlotMark::doHalfSquareRightStar(XWTikzState * stateA)
{
  stateA = stateA->save();
  stateA->moveTo(0,-stateA->markSize);
  stateA->lineTo(stateA->markSize,0);
  stateA->lineTo(0,stateA->markSize);
  stateA->setFill(true);
  stateA = stateA->restore();

  if (stateA->isMarkColorSet)
  {
    stateA = stateA->save();
    stateA->moveTo(0,-stateA->markSize);
    stateA->lineTo(-stateA->markSize,0);
    stateA->lineTo(0,stateA->markSize);
    stateA->setFillColor(stateA->markColor);
    stateA = stateA->restore();
  }

  stateA->moveTo(stateA->markSize,0);
  stateA->lineTo(0,stateA->markSize);
  stateA->lineTo(-stateA->markSize,0);
  stateA->lineTo(0,-stateA->markSize);
  stateA->closePath();
  stateA->setDraw(true);
}

void XWTikzPlotMark::doHalfSquareStar(XWTikzState * stateA)
{
  stateA = stateA->save();
  stateA->moveTo(-stateA->markSize,0);
  stateA->lineTo(0,-stateA->markSize);
  stateA->lineTo(stateA->markSize,0);
  stateA->setFill(true);
  stateA = stateA->restore();

  if (stateA->isMarkColorSet)
  {
    stateA = stateA->save();
    stateA->moveTo(stateA->markSize,0);
    stateA->lineTo(0,stateA->markSize);
    stateA->lineTo(-stateA->markSize,0);
    stateA->setFillColor(stateA->markColor);
    stateA = stateA->restore();
  }

  stateA->moveTo(stateA->markSize,0);
  stateA->lineTo(0,stateA->markSize);
  stateA->lineTo(-stateA->markSize,0);
  stateA->lineTo(-stateA->markSize,0);
  stateA->closePath();
  stateA->setDraw(true);
}

void XWTikzPlotMark::doHeart(XWTikzState * stateA)
{
  stateA->moveTo(0, -1.75 * stateA->markSize);
  stateA->curveTo(0, -1.75 * stateA->markSize, 0, -1.66 * stateA->markSize, -0.5 * stateA->markSize, -1.165 * stateA->markSize);
  stateA->curveTo(-0.5 * stateA->markSize, -1.165 * stateA->markSize, -stateA->markSize, -0.75 * stateA->markSize, -stateA->markSize, 0);
  stateA->curveTo(-stateA->markSize, 0, -stateA->markSize, 0.5825 * stateA->markSize, -0.5825 * stateA->markSize,0.5825 * stateA->markSize);
  stateA->curveTo(-0.5825 * stateA->markSize,0.5825 * stateA->markSize, 0, 0.5825 * stateA->markSize, 0, 0);
  stateA->curveTo(0, 0, 0, 0.5825 * stateA->markSize, 0.5825 * stateA->markSize, 0.5825 * stateA->markSize);
  stateA->curveTo(0.5825 * stateA->markSize, 0.5825 * stateA->markSize, stateA->markSize, 0.5825 * stateA->markSize, stateA->markSize, 0);
  stateA->curveTo(stateA->markSize, 0, stateA->markSize, -0.75 * stateA->markSize, 0.5 * stateA->markSize, -1.165 * stateA->markSize);
  stateA->curveTo(0.5 * stateA->markSize, -1.165 * stateA->markSize, 0, -1.66 * stateA->markSize, 0, -1.75 * stateA->markSize);
  stateA->closePath();
  stateA->setDraw(true);
  stateA->setFill(true);
}

void XWTikzPlotMark::doMercedesStar(XWTikzState * stateA)
{
  stateA->moveTo(stateA->markSize * cos((double)90), stateA->markSize * sin((double)90));
  stateA->lineTo(0,0);
  stateA->moveTo(stateA->markSize * cos((double)-30), stateA->markSize * sin((double)-30));
  stateA->lineTo(0,0);
  stateA->moveTo(stateA->markSize * cos((double)-150), stateA->markSize * sin((double)-150));
  stateA->lineTo(0,0);
  stateA->setDraw(true);
}

void XWTikzPlotMark::doMercedesStarFlipped(XWTikzState * stateA)
{
  stateA->moveTo(stateA->markSize * cos((double)-90), stateA->markSize * sin((double)-90));
  stateA->lineTo(0,0);
  stateA->moveTo(stateA->markSize * cos((double)30), stateA->markSize * sin((double)30));
  stateA->lineTo(0,0);
  stateA->moveTo(stateA->markSize * cos((double)150), stateA->markSize * sin((double)150));
  stateA->lineTo(0,0);
  stateA->setDraw(true);
}

void XWTikzPlotMark::doOpenDot(XWTikzState * stateA)
{
  QPointF c(0,0);
  QPointF a(stateA->markSize,0);
  QPointF b(0,stateA->markSize);
  stateA->addEllipse(c,a,b);
  stateA->setDraw(true);
}

void XWTikzPlotMark::doOPlus(XWTikzState * stateA)
{
  QPointF c(0,0);
  stateA->addCircle(c, stateA->markSize);
  stateA->moveTo(-stateA->markSize, 0);
  stateA->lineTo(stateA->markSize, 0);
  stateA->moveTo(0, stateA->markSize);
  stateA->lineTo(0,-stateA->markSize);
  stateA->setDraw(true);
}

void XWTikzPlotMark::doOPlusStar(XWTikzState * stateA)
{
  QPointF c(0,0);
  stateA->addCircle(c, stateA->markSize);
  stateA->moveTo(-stateA->markSize, 0);
  stateA->lineTo(stateA->markSize, 0);
  stateA->moveTo(0, stateA->markSize);
  stateA->lineTo(0,-stateA->markSize);
  stateA->setDraw(true);
  stateA->setFill(true);
}

void XWTikzPlotMark::doOTimes(XWTikzState * stateA)
{
  QPointF c(0,0);
  stateA->addCircle(c, stateA->markSize);
  stateA->moveTo(-0.70710678 * stateA->markSize, -0.70710678 * stateA->markSize);
  stateA->lineTo(0.70710678 * stateA->markSize, 0.70710678 * stateA->markSize);
  stateA->moveTo(-0.70710678 * stateA->markSize, 0.70710678 * stateA->markSize);
  stateA->lineTo(0.70710678 * stateA->markSize,-0.70710678 * stateA->markSize);
  stateA->setDraw(true);
}

void XWTikzPlotMark::doOTimesStar(XWTikzState * stateA)
{
  QPointF c(0,0);
  stateA->addCircle(c, stateA->markSize);
  stateA->moveTo(-0.70710678 * stateA->markSize, -0.70710678 * stateA->markSize);
  stateA->lineTo(0.70710678 * stateA->markSize, 0.70710678 * stateA->markSize);
  stateA->moveTo(-0.70710678 * stateA->markSize, 0.70710678 * stateA->markSize);
  stateA->lineTo(0.70710678 * stateA->markSize,-0.70710678 * stateA->markSize);
  stateA->setDraw(true);
  stateA->setFill(true);
}

void XWTikzPlotMark::doPentagon(XWTikzState * stateA)
{
  stateA->moveTo(0, stateA->markSize);
  stateA->lineTo(stateA->markSize * cos((double)18), stateA->markSize * sin((double)18));
  stateA->lineTo(stateA->markSize * cos((double)-54), stateA->markSize * sin((double)-54));
  stateA->lineTo(stateA->markSize * cos((double)234), stateA->markSize * sin((double)234));
  stateA->lineTo(stateA->markSize * cos((double)162), stateA->markSize * sin((double)162));
  stateA->closePath();
  stateA->setDraw(true);
}

void XWTikzPlotMark::doPentagonStar(XWTikzState * stateA)
{
  stateA->moveTo(0, stateA->markSize);
  stateA->lineTo(stateA->markSize * cos((double)18), stateA->markSize * sin((double)18));
  stateA->lineTo(stateA->markSize * cos((double)-54), stateA->markSize * sin((double)-54));
  stateA->lineTo(stateA->markSize * cos((double)234), stateA->markSize * sin((double)234));
  stateA->lineTo(stateA->markSize * cos((double)162), stateA->markSize * sin((double)162));
  stateA->closePath();
  stateA->setFill(true);
}

void XWTikzPlotMark::doPointed10Star(XWTikzState * stateA)
{
  stateA->moveTo(0, -stateA->markSize);
  stateA->lineTo(0, stateA->markSize);
  stateA->moveTo(-stateA->markSize * cos((double)18), -stateA->markSize * sin((double)18));
  stateA->lineTo(stateA->markSize * cos((double)18), stateA->markSize * sin((double)18));
  stateA->moveTo(-stateA->markSize * cos((double)-54), -stateA->markSize * sin((double)-54));
  stateA->lineTo(stateA->markSize * cos((double)-54), stateA->markSize * sin((double)-54));
  stateA->moveTo(-stateA->markSize * cos((double)234), -stateA->markSize * sin((double)234));
  stateA->lineTo(stateA->markSize * cos((double)234), stateA->markSize * sin((double)234));
  stateA->moveTo(-stateA->markSize * cos((double)162), -stateA->markSize * sin((double)162));
  stateA->lineTo(stateA->markSize * cos((double)162), stateA->markSize * sin((double)162));
  stateA->setDraw(true);
}

void XWTikzPlotMark::doSquare(XWTikzState * stateA)
{
  QPointF ll(-stateA->markSize, -stateA->markSize);
  stateA->rectangle(ll, 2 * stateA->markSize, 2 * stateA->markSize);
  stateA->setDraw(true);
}

void XWTikzPlotMark::doSquareStar(XWTikzState * stateA)
{
  QPointF ll(-stateA->markSize, -stateA->markSize);
  stateA->rectangle(ll, 2 * stateA->markSize, 2 * stateA->markSize);
  stateA->setFill(true);
}

void XWTikzPlotMark::doStar(XWTikzState * stateA)
{
  stateA->moveTo(0, 0);
  stateA->lineTo(0, stateA->markSize);
  stateA->moveTo(0, 0);
  stateA->lineTo(stateA->markSize * cos((double)18), stateA->markSize * sin((double)18));
  stateA->moveTo(0, 0);
  stateA->lineTo(stateA->markSize * cos((double)-54), stateA->markSize * sin((double)-54));
  stateA->moveTo(0, 0);
  stateA->lineTo(stateA->markSize * cos((double)234), stateA->markSize * sin((double)234));
  stateA->moveTo(0, 0);
  stateA->lineTo(stateA->markSize * cos((double)162), stateA->markSize * sin((double)162));
  stateA->setDraw(true);
}

void XWTikzPlotMark::doText(XWTikzState * stateA)
{
  XWTikzTextBox * box = new XWTikzTextBox(stateA->graphic,stateA);
  int len = stateA->markText.length();
  int pos = 0;
  box->scan(stateA->markText,len, pos);
  stateA = stateA->saveNode(box,XW_TIKZ_NODE);
}

void XWTikzPlotMark::doTriangle(XWTikzState * stateA)
{
  stateA->moveTo(0, stateA->markSize);
  stateA->lineTo(stateA->markSize * cos((double)-30), stateA->markSize * sin((double)-30));
  stateA->lineTo(stateA->markSize * cos((double)-150), stateA->markSize * sin((double)-150));
  stateA->closePath();
  stateA->setDraw(true);
}

void XWTikzPlotMark::doTriangleStar(XWTikzState * stateA)
{
  stateA->moveTo(0, stateA->markSize);
  stateA->lineTo(stateA->markSize * cos((double)-30), stateA->markSize * sin((double)-30));
  stateA->lineTo(stateA->markSize * cos((double)-150), stateA->markSize * sin((double)-150));
  stateA->closePath();
  stateA->setFill(true);
}
