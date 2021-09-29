/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTIKZARROW_H
#define XWTIKZARROW_H

#define XW_TIKZ_ARROW_CODE_SETUP 0
#define XW_TIKZ_ARROW_CODE_DRAW  1

class XWTikzState;

class XWTikzArrow
{
public:
  XWTikzArrow(int s);

  void draw(XWTikzState * state);

  void setup(XWTikzState * state);

private:
  void ArcBarbArrow(XWTikzState * state);
  void angle45Arrow(XWTikzState * state);
  void angle60Arrow(XWTikzState * state);
  void angle90Arrow(XWTikzState * state);
  
  void barArrow(XWTikzState * state);
  void ButtCapArrow(XWTikzState * state);
  void buttcapArrow(XWTikzState * state);

  void CircleArrow(XWTikzState * state);
  void ClassicalTikZRightarrow(XWTikzState * state);
  void ComputerModernRightarrow(XWTikzState * state);

  void diamondArrow(XWTikzState * state);
  void directioneeArrow(XWTikzState * state);
  void dotArrow(XWTikzState * state);

  void fastcapArrow(XWTikzState * state);
  void fastcapReversedArrow(XWTikzState * state);
  void FastRoundArrow(XWTikzState * state);
  void FastTriangleArrow(XWTikzState * state);

  void HooksArrow(XWTikzState * state);
  void hooksArrow(XWTikzState * state);

  void ImpliesArrow(XWTikzState * state);
  void impliesArrow(XWTikzState * state);

  void KiteArrow(XWTikzState * state);

  void LatexArrow(XWTikzState * state);
  void latexArrow(XWTikzState * state);
  void latexFleshyArrow(XWTikzState * state);
  void leftHookArrow(XWTikzState * state);
  void leftToArrow(XWTikzState * state);
  void leftToReversedArrow(XWTikzState * state);

  void openDiamondArrow(XWTikzState * state);
  void openDotArrow(XWTikzState * state);
  void openSquareArrow(XWTikzState * state);
  void openTriangle45Arrow(XWTikzState * state);
  void openTriangle45ReversedArrow(XWTikzState * state);
  void openTriangle60Arrow(XWTikzState * state);
  void openTriangle60ReversedArrow(XWTikzState * state);
  void openTriangle90Arrow(XWTikzState * state);
  void openTriangle90ReversedArrow(XWTikzState * state);

  void RaysArrow(XWTikzState * state);
  void rightHookArrow(XWTikzState * state);
  void rightToArrow(XWTikzState * state);
  void rightToReversedArrow(XWTikzState * state);
  void roundBracketArrow(XWTikzState * state);
  void RoundCapArrow(XWTikzState * state);
  void roundcapArrow(XWTikzState * state);

  void serifcmArrow(XWTikzState * state);
  void spaceArrow(XWTikzState * state);
  void spacedAngle45Arrow(XWTikzState * state);
  void spacedAngle45ReversedArrow(XWTikzState * state);
  void spacedAngle60Arrow(XWTikzState * state);
  void spacedAngle60ReversedArrow(XWTikzState * state);
  void spacedAngle90Arrow(XWTikzState * state);
  void spacedAngle90ReversedArrow(XWTikzState * state);
  void spacedBarArrow(XWTikzState * state);
  void spacedButtCapArrow(XWTikzState * state);
  void spacedDiamondArrow(XWTikzState * state);
  void spacedDotArrow(XWTikzState * state);
  void spacedCastCapArrow(XWTikzState * state);
  void spacedCastCapReversedArrow(XWTikzState * state);
  void spacedHooksArrow(XWTikzState * state);
  void spacedHooksReversedArrow(XWTikzState * state);
  void spacedImpliesArrow(XWTikzState * state);
  void spacedLatexArrow(XWTikzState * state);
  void spacedLatexFleshyArrow(XWTikzState * state);
  void spacedLatexFleshyReversedArrow(XWTikzState * state);
  void spacedLatexReversedArrow(XWTikzState * state);
  void spacedLeftHookArrow(XWTikzState * state);
  void spacedLeftHookReversedArrow(XWTikzState * state);
  void spacedLeftToArrow(XWTikzState * state);
  void spacedLeftToReversedArrow(XWTikzState * state);
  void spacedOpenDiamondArrow(XWTikzState * state);
  void spacedOpenDotArrow(XWTikzState * state);
  void spacedOpenSquareArrow(XWTikzState * state);
  void spacedOpenTriangle45Arrow(XWTikzState * state);
  void spacedOpenTriangle45ReversedArrow(XWTikzState * state);
  void spacedOpenTriangle60Arrow(XWTikzState * state);
  void spacedOpenTriangle60ReversedArrow(XWTikzState * state);
  void spacedOpenTriangle90Arrow(XWTikzState * state);
  void spacedOpenTriangle90ReversedArrow(XWTikzState * state);
  void spacedRightHookArrow(XWTikzState * state);
  void spacedRightHookReversedArrow(XWTikzState * state);
  void spacedRightToArrow(XWTikzState * state);
  void spacedRightToReversedArrow(XWTikzState * state);
  void spacedRoundBracketLArrow(XWTikzState * state);
  void spacedRoundBracketRArrow(XWTikzState * state);
  void spacedRoundCapArrow(XWTikzState * state);
  void spacedSerifcmArrow(XWTikzState * state);
  void spacedSquareArrow(XWTikzState * state);
  void spacedSquareBracketArrow(XWTikzState * state);
  void spacedSquareBracketReversedArrow(XWTikzState * state);
  void spacedStealthArrow(XWTikzState * state);
  void spacedStealthReversedArrow(XWTikzState * state);
  void spacedStealthRoundedArrow(XWTikzState * state);
  void spacedStealthRoundedReversedArrow(XWTikzState * state);
  void spacedToArrow(XWTikzState * state);
  void spacedToReversedArrow(XWTikzState * state);
  void spacedTriangle45Arrow(XWTikzState * state);
  void spacedTriangle45ReversedArrow(XWTikzState * state);
  void spacedTriangle60Arrow(XWTikzState * state);
  void spacedTriangle60ReversedArrow(XWTikzState * state);
  void spacedTriangle90Arrow(XWTikzState * state);
  void spacedTriangle90ReversedArrow(XWTikzState * state);
  void spacedTriangle90CapArrow(XWTikzState * state);
  void spacedTriangle90CapReversedArrow(XWTikzState * state);
  void SquareArrow(XWTikzState * state);
  void squareArrow(XWTikzState * state);
  void squareBracketArrow(XWTikzState * state);
  void StealthArrow(XWTikzState * state);
  void stealthArrow(XWTikzState * state);
  void stealthRoundedArrow(XWTikzState * state);
  void StraightBarbArrow(XWTikzState * state);

  void TeeBarbArrow(XWTikzState * state);
  void toArrow(XWTikzState * state);
  void toReversedArrow(XWTikzState * state);
  void TriangleCapArrow(XWTikzState * state);
  void triangle45Arrow(XWTikzState * state);
  void triangle60Arrow(XWTikzState * state);
  void triangle90Arrow(XWTikzState * state);
  void triangle90capArrow(XWTikzState * state);
  void triangle90capReversedArrow(XWTikzState * state);

  void doArrow(XWTikzState * state);

public:
  int    shape;
  int    code;
  double x;
  double y;
  double tipEnd;
  double backEnd;
  double lineEnd;
  double visualTipEnd;
  double visualBackEnd;
};

#endif //XWTIKZARROW_H
