/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTIKZPLOTMARK_H
#define XWTIKZPLOTMARK_H

class XWTikzState;

class XWTikzPlotMark
{
public:
  XWTikzPlotMark(int ptypeA);

  void doMark(XWTikzState * stateA);

private:
  void doAsterisk(XWTikzState * stateA);
  void doBall(XWTikzState * stateA);
  void doBarMarkHorizontal(XWTikzState * stateA);
  void doBarMarkVertical(XWTikzState * stateA);
  void doDiamond(XWTikzState * stateA);
  void doDiamondStar(XWTikzState * stateA);
  void doHalfCircle(XWTikzState * stateA);
  void doHalfCircleStar(XWTikzState * stateA);
  void doHalfDiamondStar(XWTikzState * stateA);
  void doHalfSquareLeftStar(XWTikzState * stateA);
  void doHalfSquareRightStar(XWTikzState * stateA);
  void doHalfSquareStar(XWTikzState * stateA);
  void doHeart(XWTikzState * stateA);
  void doMercedesStar(XWTikzState * stateA);
  void doMercedesStarFlipped(XWTikzState * stateA);
  void doOpenDot(XWTikzState * stateA);
  void doOPlus(XWTikzState * stateA);
  void doOPlusStar(XWTikzState * stateA);
  void doOTimes(XWTikzState * stateA);
  void doOTimesStar(XWTikzState * stateA);
  void doPentagon(XWTikzState * stateA);
  void doPentagonStar(XWTikzState * stateA);
  void doPointed10Star(XWTikzState * stateA);
  void doSquare(XWTikzState * stateA);
  void doSquareStar(XWTikzState * stateA);
  void doStar(XWTikzState * stateA);
  void doText(XWTikzState * stateA);
  void doTriangle(XWTikzState * stateA);
  void doTriangleStar(XWTikzState * stateA);

private:
  int ptype;
};

#endif //XWTIKZPLOTMARK_H
