/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include <QPointF>
#include <QVector2D>
#include "LaTeXKeyWord.h"
#include "PGFKeyWord.h"
#include "XWPGFPool.h"
#include "XWTikzState.h"
#include "XWTikzArrow.h"

XWTikzArrow::XWTikzArrow(int s)
:shape(s),
 code(XW_TIKZ_ARROW_CODE_SETUP),
 x(0),
 y(0),
 tipEnd(0),
 backEnd(0),
 lineEnd(0),
 visualTipEnd(0),
 visualBackEnd(0)
{}

void XWTikzArrow::draw(XWTikzState * state)
{
  code = XW_TIKZ_ARROW_CODE_DRAW;
  state->arrowBendMode = PGForthogonal;
  doArrow(state);
}

void XWTikzArrow::setup(XWTikzState * state)
{
  code = XW_TIKZ_ARROW_CODE_SETUP;
  doArrow(state);
}

void XWTikzArrow::doArrow(XWTikzState * state)
{
  if (shape == PGFarrowdefault)
    shape = state->arrowDefault;
    
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

    case PGFlatexreversed:
      state->arrowReversed = true;
    case PGFlatex:
      latexArrow(state);
      break;

    case PGFbar:
      barArrow(state);
      break;

    case PGFsquarebracketr:
      state->arrowReversed = true;
    case PGFsquarebracket:
    case PGFsquarebracketl:
      squareBracketArrow(state);
      break;

    case PGFroundbracketr:
      state->arrowReversed = true;
    case PGFroundbracketl:	  
      roundBracketArrow(state);
      break;

    case PGFangle90reversed:
      state->arrowReversed = true;
    case PGFangle90:
      angle90Arrow(state);
      break;

    case PGFangle60reversed:
      state->arrowReversed = true;
    case PGFangle60:	  
      angle60Arrow(state);
      break;

    case PGFangle45reversed:
      state->arrowReversed = true;
    case PGFangle45:	  
      angle45Arrow(state);
      break;

    case PGFdot:
      dotArrow(state);
      break;

    case PGFopendot:
      openDotArrow(state);
      break;

    case PGFdiamond:
      diamondArrow(state);
      break;

    case PGFopendiamond:
      openDiamondArrow(state);
      break;

    case PGFsquare:
      squareArrow(state);
      break;

    case PGFopensquare:
      openSquareArrow(state);
      break;

    case PGFtriangle90reversed:
      state->arrowReversed = true;
    case PGFtriangle90:
      triangle90Arrow(state);
      break;

    case PGFtriangle60reversed:
      state->arrowReversed = true;
    case PGFtriangle60:
      triangle60Arrow(state);
      break;

    case PGFtriangle45reversed:
      state->arrowReversed = true;
    case PGFtriangle45:
      triangle45Arrow(state);
      break;

    case PGFopentriangle90:
      openTriangle90Arrow(state);
      break;

    case PGFopentriangle90reversed:
      openTriangle90ReversedArrow(state);
      break;
	  
    
	  case PGFopentriangle60:
      openTriangle60Arrow(state);
      break;

    case PGFopentriangle60reversed:
      openTriangle60ReversedArrow(state);
      break;

	  case PGFopentriangle45:
      openTriangle45Arrow(state);
      break;

    case PGFopentriangle45reversed:
      openTriangle45ReversedArrow(state);
      break;

    case PGFlatexfleshyreversed:
      state->arrowReversed = true;
    case PGFlatexfleshy:
      latexFleshyArrow(state);
      break;

    case PGFstealthroundedreversed:
      state->arrowReversed = true;
    case PGFstealthrounded:
      stealthRoundedArrow(state);
      break;

    case PGFleftto:
      leftToArrow(state);
      break;

    case PGFrightto:
      rightToArrow(state);
      break;

    case PGFlefttoreversed:
      leftToReversedArrow(state);
      break;

    case PGFrighttoreversed:
      rightToReversedArrow(state);
      break;

    case PGFlefthookreversed:
      state->arrowReversed = true;
    case PGFlefthook:
      leftHookArrow(state);
      break;

    case PGFrighthookreversed:
      state->arrowReversed = true;
    case PGFrighthook:
      rightHookArrow(state);
      break;

    case PGFhooksreversed:
      state->arrowReversed = true;
    case PGFhooks:
      hooksArrow(state);
      break;

    case PGFserifcm:
      serifcmArrow(state);
      break;

    case PGFroundcap:
      roundcapArrow(state);
      break;

    case PGFbuttcap:
      buttcapArrow(state);
      break;

    case PGFtriangle90cap:
      triangle90capArrow(state);
      break;

    case PGFtriangle90capreversed:
      triangle90capReversedArrow(state);
      break;

    case PGFfastcap:
      fastcapArrow(state);
      break;

    case PGFfastcapreversed:
      fastcapReversedArrow(state);
      break;

    case PGFimplies:
      impliesArrow(state);
      break;

    case PGFStraightBarb:
      StraightBarbArrow(state);
      break;

    case PGFHooks:
      HooksArrow(state);
      break;

    case PGFArcBarb:
    case PGFParenthesis:
      ArcBarbArrow(state);
      break;

    case PGFTeeBarb:
    case PGFBar:
    case PGFBracket:
      TeeBarbArrow(state);
      break;

    case PGFClassicalTikZRightarrow:
      ClassicalTikZRightarrow(state);
      break;

    case PGFComputerModernRightarrow:
    case PGFTo:
      ComputerModernRightarrow(state);
      break;

    case PGFImplies:
      ImpliesArrow(state);
      break;

    case PGFLatex:
    case PGFLaTeX:
      LatexArrow(state);
      break;

    case PGFStealth:
    case PGFTriangle:
      StealthArrow(state);
      break;

    case PGFKite:
    case PGFDiamond:
    case PGFTurnedSquare:
      KiteArrow(state);
      break;

    case PGFSquare:
    case PGFRectangle:
      SquareArrow(state);
      break;

    case PGFCircle:
    case PGFEllipse:
      CircleArrow(state);
      break;

    case PGFRoundCap:
      RoundCapArrow(state);
      break;

    case PGFButtCap:
      ButtCapArrow(state);
      break;

    case PGFTriangleCap:
      TriangleCapArrow(state);
      break;

    case PGFFastTriangle:
      FastTriangleArrow(state);
      break;

    case PGFFastRound:
      FastRoundArrow(state);
      break;

    case PGFRays:
      RaysArrow(state);
      break;

    case PGFspacedto:
      spacedToArrow(state);
      break;

    case PGFspacedtoreversed:
      spacedToReversedArrow(state);
      break;

    case PGFspacedimplies:
      spacedImpliesArrow(state);
      break;

    case PGFspacedlatex:
      spacedLatexArrow(state);
      break;

    case PGFspacedlatexreversed:
      spacedLatexReversedArrow(state);
      break;

    case PGFspacedlatexfleshy:
      spacedLatexFleshyArrow(state);
      break;

    case PGFspacedlatexreversedfleshy:
      spacedLatexFleshyReversedArrow(state);
      break;

    case PGFspacedstealth:
      spacedStealthArrow(state);
      break;

    case PGFspacedstealthreversed:
      spacedStealthReversedArrow(state);
      break;

    case PGFspacedstealthrounded:
      spacedStealthRoundedArrow(state);
      break;

    case PGFspacedstealthroundedreversed:
      spacedStealthRoundedReversedArrow(state);
      break;

    case PGFspacedtriangle90:
      spacedTriangle90Arrow(state);
      break;

    case PGFspacedtriangle90reversed:
      spacedTriangle90ReversedArrow(state);
      break;

    case PGFspacedtriangle60:
      spacedTriangle60Arrow(state);
      break;

    case PGFspacedtriangle60reversed:
      spacedTriangle60ReversedArrow(state);
      break;

    case PGFspacedtriangle45:
      spacedTriangle45Arrow(state);
      break;

    case PGFspacedtriangle45reversed:
      spacedTriangle45ReversedArrow(state);
      break;

    case PGFspacedopentriangle90:
      spacedTriangle90Arrow(state);
      break;

    case PGFspacedopentriangle90reversed:
      spacedTriangle90ReversedArrow(state);
      break;

    case PGFspacedopentriangle60:
      spacedTriangle60Arrow(state);
      break;

    case PGFspacedopentriangle60reversed:
      spacedTriangle60ReversedArrow(state);
      break;

    case PGFspacedopentriangle45:
      spacedTriangle45Arrow(state);
      break;

    case PGFspacedopentriangle45reversed:
      spacedTriangle45ReversedArrow(state);
      break;

    case PGFspacedangle90:
      spacedAngle90Arrow(state);
      break;

	  case PGFspacedangle90reversed:
      spacedAngle90ReversedArrow(state);
      break;

	  case PGFspacedangle60:
      spacedAngle60Arrow(state);
      break;

	  case PGFspacedangle60reversed:
      spacedAngle60ReversedArrow(state);
      break;

	  case PGFspacedangle45:
      spacedAngle45Arrow(state);
      break;

	  case PGFspacedangle45reversed:
      spacedAngle45ReversedArrow(state);
      break;

    case PGFspacedhooks:
      spacedHooksArrow(state);
      break;

	  case PGFspacedhooksreversed:
      spacedHooksReversedArrow(state);
      break;

    case PGFspacedopendot:
      spacedOpenDotArrow(state);
      break;

    case PGFspaceddot:
      spacedDotArrow(state);
      break;

    case PGFspaceddiamond:
      spacedDiamondArrow(state);
      break;

    case PGFspacedopendiamond:
      spacedOpenDiamondArrow(state);
      break;

    case PGFspacedsquare:
      spacedSquareArrow(state);
      break;

    case PGFspacedopensquare:
      spacedOpenSquareArrow(state);
      break;

    case PGFspacedserifcm:
      spacedSerifcmArrow(state);
      break;

    case PGFspacedleftto:
      spacedLeftToArrow(state);
      break;

    case PGFspacedlefttoreversed:
      spacedLeftToReversedArrow(state);
      break;

    case PGFspacedrightto:
      spacedRightToArrow(state);
      break;

    case PGFspacedrighttoreversed:
      spacedRightToReversedArrow(state);
      break;

    case PGFspacedlefthook:
      spacedLeftHookArrow(state);
      break;

    case PGFspacedlefthookreversed:
      spacedLeftHookReversedArrow(state);
      break;

    case PGFspacedrighthook:
      spacedRightHookArrow(state);
      break;

    case PGFspacedrighthookreversed:
      spacedRightHookReversedArrow(state);
      break;

    case PGFspacedroundcap:
      spacedRoundCapArrow(state);
      break;

    case PGFspacedbuttcap:
      spacedButtCapArrow(state);
      break;

    case PGFspacedtriangle90cap:
      spacedTriangle90CapArrow(state);
      break;

    case PGFspacedtriangle90capreversed:
      spacedTriangle90CapReversedArrow(state);
      break;

    case PGFspacedroundbracketl:
      spacedRoundBracketLArrow(state);
      break;

	  case PGFspacedroundbracketr:
      spacedRoundBracketRArrow(state);
      break;

    case PGFspacedsquarebracket:
      spacedSquareBracketArrow(state);
      break;

	  case PGFspacedsquarebracketreversed:
      spacedSquareBracketReversedArrow(state);
      break;

    case PGFdirectionee:
      directioneeArrow(state);
      break;
  }
}

void XWTikzArrow::ArcBarbArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        if (shape == PGFArcBarb)
        {
          state->setArrowLength(state->lineWidthDependent(1.5,2,0));
          state->setArrowWidth(state->lengthDependent(0,2,0));
          state->setArrowLineWidth(state->lineWidthDependent(0,1,1));
        }
        else
        {
          state->setArrowArc(120);
          state->setArrowLength(state->lineWidthDependent(1.725,2.3,0));
        }
        x = 0.5 * state->arrowArc;
        state->arrowArc = 0.5 * state->arrowArc;
        tipEnd = state->arrowLineWidth;
        visualBackEnd = tipEnd;
        x -= 0.5 * state->arrowLineWidth;
        lineEnd = state->arrowLength * x;
        double mathresultx = cos(state->arrowArc);
        if (state->arrowLineCap == PGFround)
        {
          y = state->arrowLength - 0.5 * state->arrowLineWidth;
          x = mathresultx * y;
        }
        else
        {
           if (state->arrowArc < 90)
           {
             y = state->arrowLength - state->arrowLineWidth;
             x = mathresultx * y;
           }
           else
             x = mathresultx * state->arrowLength;
        }
        if (state->arrowLineCap == PGFround)
          backEnd = x - 0.5 * state->arrowLineWidth;
        else
          backEnd = x;
        state->arrowLength -= 0.5 * state->arrowLineWidth;
        state->arrowWidth -= state->arrowLineWidth;
        visualTipEnd = tipEnd;
      }
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      state->setDash(-1);
      if (state->arrowLineJoin == PGFround)
        state->setLineJoin(PGFround);
      else
        state->setLineJoin(PGFmiter);
      if (state->arrowLineCap == PGFround)
        state->setLineCap(PGFround);
      else
        state->setLineCap(PGFbutt);
      if (state->lineWidth != state->arrowLineWidth)
        state->setLineWidth(state->arrowLineWidth);
      state->scale(state->arrowLength, 0.5*state->arrowWidth);
      state->moveTo(cos(state->arrowArc), sin(state->arrowArc));      
      state->setXRadius(1);
      state->setYRadius(1);
      if (state->arrowHarpoon)
      {
        state->setStartAngle(state->arrowArc);
        state->setEndAngle(0);
      }
      else
      {
        state->setStartAngle(state->arrowArc);
        state->setEndAngle(-state->arrowArc);
      }
      state->addArc();
      if (state->arrowHarpoon)
      {
        if (state->arrowReversed)
          state->lineTo(state->arrowLength - 0.5 * state->arrowLineWidth, 0);
        else
          state->lineTo(state->arrowLength + 0.5 * state->arrowLineWidth, 0);
      }
      state->setDraw(true);
      break;
  }
}

void XWTikzArrow::angle45Arrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        double tmpa = 0.3 + 0.25 * state->lineWidth;
        double tmpb = 8.705 * tmpa + 0.5 * state->lineWidth;
        backEnd = -tmpb;
        tmpb = 0.5 * tmpa + 1.28 * state->lineWidth;
        tipEnd = tmpb;
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }      
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        double a = 0.3 + 0.25 * state->lineWidth;
        state->setDash(-1);
        state->setLineCap(PGFround);
        state->setLineJoin(PGFmiter);
        state->moveTo(0.5 * a + 10 * a * cos((double)157), 10 * a * sin((double)157));
        state->lineTo(0.5*a, 0);
        state->lineTo(0.5 * a + 10 * a * cos((double)-157), 10 * a * sin((double)-157));
        state->setDraw(true);
      }
      break;
  }
}

void XWTikzArrow::angle60Arrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        double tmpa = 0.3 + 0.25 * state->lineWidth;
        double tmpb = 7.29 * tmpa + 0.5 * state->lineWidth;
        backEnd = -tmpb;
        tmpb = 0.5 * tmpa + state->lineWidth;
        tipEnd = tmpb;
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }      
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        double a = 0.3 + 0.25 * state->lineWidth;
        state->setDash(-1);
        state->setLineCap(PGFround);
        state->setLineJoin(PGFmiter);
        state->moveTo(0.5 * a + 9 * a * cos((double)150), 9 * a * sin((double)150));
        state->lineTo(0.5*a, 0);
        state->lineTo(0.5 * a + 9 * a * cos((double)-150), 9 * a * sin((double)-150));
        state->setDraw(true);
      }
      break;
  }
}

void XWTikzArrow::angle90Arrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        double tmpa = 0.3 + 0.25 * state->lineWidth;
        double tmpb = 5.5 * tmpa + 0.5 * state->lineWidth;
        backEnd = -tmpb;
        tmpb = 0.5 * tmpa + 0.707 * state->lineWidth;
        tipEnd = tmpb;
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }      
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        double a = 0.3 + 0.25 * state->lineWidth;
        state->setDash(-1);
        state->setLineCap(PGFround);
        state->setLineJoin(PGFmiter);
        state->moveTo(-5.5 * a, -6 * a);
        state->lineTo(0.5*a, 0);
        state->lineTo(-5.5*a, 6 * a);
        state->setDraw(true);
      }
      break;
  }
}

void XWTikzArrow::barArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      tipEnd = 0.75 * state->innerLineWidth;
      backEnd = -0.25 * state->innerLineWidth;
      visualTipEnd = tipEnd;
      visualBackEnd = backEnd;
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        double a = 2 + 1.5 * state->lineWidth;
        state->setDash(-1);
        state->setLineCap(PGFrect);
        state->moveTo(0.25*state->lineWidth, -a);
        state->lineTo(0.25*state->lineWidth, a);
        state->setDraw(true);
      }
      break;
  }
}

void XWTikzArrow::ButtCapArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      state->setArrowLength(state->lineWidthDependent(0,0.5,0));
      tipEnd = state->arrowLength;
      lineEnd = -0.5 * state->lineWidth;
      visualTipEnd = tipEnd;
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        double tempdima = 0.5 * state->lineWidth;
        state->moveTo(-0.75 * state->lineWidth,tempdima);
        state->lineTo(0,tempdima);
        state->lineTo(state->arrowLength,tempdima);
        state->lineTo(state->arrowLength,-tempdima);
        state->lineTo(-0.75 * state->lineWidth,-tempdima);
        state->closePath();
        state->setFill(true);
      }
      break;
  }
}

void XWTikzArrow::buttcapArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        backEnd = -1 * state->lineWidth;
        tipEnd = 0.5 * state->lineWidth;
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }      
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        state->setDash(-1);
        state->setLineCap(PGFbutt);
        state->moveTo(-1 * state->lineWidth,(double)0);
        state->lineTo(0.5 * state->lineWidth,0);
        state->setDraw(true);
      }
      break;
  }
}

void XWTikzArrow::CircleArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        if (shape == PGFCircle)
        {
          state->setArrowLength(state->lineWidthDependent(2.39365,3.191538,0));
          state->setArrowWidth(state->lengthDependent(0,1,1));
          state->setArrowLineWidth(state->lineWidthDependent(0,1,1));
        }
        else
        {
          state->setArrowLength(state->lineWidthDependent(3.3,4.95,0));
          state->setArrowWidth(state->lengthDependent(0,0.5,0));
        }
        x = 0.5 * state->arrowLength;
        y = 0.5 * state->arrowWidth;
        if (x < state->arrowLineWidth)
          state->arrowLineWidth = x;
        if (state->arrowReversed)
          lineEnd = state->arrowLength - 0.5 * state->arrowLineWidth;
        else
          lineEnd = 0.5 * state->arrowLineWidth;
        tipEnd = state->arrowLength;
        visualTipEnd = tipEnd;
      }
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      state->setDash(-1);
      if (state->arrowLineJoin == PGFround)
        state->setLineJoin(PGFround);
      else
        state->setLineJoin(PGFmiter);
      if (state->lineWidth != state->arrowLineWidth)
        state->setLineWidth(state->arrowLineWidth);
      if (state->arrowHarpoon)
      {
        state->moveTo(state->arrowLength - 0.5 * state->arrowLineWidth, 0);
        double xa = 0.5 * state->arrowLength - 0.5 * state->arrowLineWidth;
        double ya = 0.5 * state->arrowWidth - 0.5 * state->arrowLineWidth;
        state->scale(xa,ya);
        state->setStartAngle(0);
        state->setEndAngle(180);
        state->setXRadius(1);
        state->setXRadius(1);
        state->addArc();
        state->closePath();
      }
      else
      {        
        QPointF c(0.5 * state->arrowLength, 0);
        QPointF a(0.5 * state->arrowLength - 0.5 * state->arrowLineWidth, 0);
        QPointF b(0, 0.5 * state->arrowWidth - 0.5 * state->arrowLineWidth);
        state->addEllipse(c,a,b);
      }
      if (!state->isArrowFillSet)
        state->setDraw(true);
      else
      {
        if (state->arrowLineWidth > 0)
        {
          state->setDraw(true);
          state->setFill(true);
        }
        else
          state->setFill(true);
      }
      break;
  }
}

void XWTikzArrow::ClassicalTikZRightarrow(XWTikzState * state)
{
  double tempdima = 0;
  double tempdimb = 0;
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        state->setArrowLength(state->lineWidthDependent(1.05,1.925,0));
        state->setArrowWidth(state->lengthDependent(0,1.9237,0));
        state->setArrowLineWidth(state->lineWidthDependent(0,0.8,1));
        state->setArrowLineCap(PGFround);
        state->setArrowLineJoin(PGFround);
        tempdima = state->arrowLength - state->arrowLineWidth;
        tempdimb = state->arrowWidth - state->arrowLineWidth;
        double xc = 0;
        x = state->arrowLineWidth;        
        if (!state->arrowHarpoon)
        {
          double mathresult = tempdima / tempdimb;
          x = 40.96 * mathresult * mathresult + 1;
          mathresult = sqrt(x);
          xc = 0.5 * mathresult * state->arrowLineWidth;
        }
        tipEnd = tempdima;
        if (state->arrowHarpoon)
          tipEnd += 0.5 * state->arrowLineWidth;
        else
          tipEnd += xc;
        visualBackEnd = tempdima + 0.5 * state->arrowLineWidth;
        if (state->arrowReversed)
        {
          if (state->arrowHarpoon)
            lineEnd = tempdima + 0.5 * state->arrowLineWidth;
          else
            lineEnd = tempdima - 0.25 * state->arrowLineWidth;
        }
        else
          lineEnd = tempdima - 0.6 * state->arrowLineWidth;
        backEnd = -0.5 * state->arrowLineWidth;
        visualTipEnd = tipEnd;
      }
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        double oldw = state->lineWidth;
        state->setDash(-1);
        if (state->arrowLineJoin == PGFround)
          state->setLineJoin(PGFround);
        else
          state->setLineJoin(PGFmiter);
        if (state->arrowLineCap == PGFround)
          state->setLineCap(PGFround);
        else
          state->setLineCap(PGFbutt);
        if (state->lineWidth != state->arrowLineWidth)
          state->setLineWidth(state->arrowLineWidth);
        state->moveTo(0, 0.5 * tempdimb);
        if (state->arrowHarpoon)
        {
          if (state->arrowReversed)
          {
            state = state->save(false);
            state->curveTo(0.066666*tempdima, 0.3125*tempdimb, 0.8*tempdima,0.03125*tempdimb, tempdima, 0.125*state->lineWidth);
            state->moveTo((double)0, 0.5 * tempdimb);
            state->curveTo(0.066666*tempdima,0.3125*tempdimb,0.8*tempdima,0.03125*tempdimb,tempdima,-0.125*state->lineWidth);
            state->setDraw(true);
            state = state->restore();
            state->setLineWidth(oldw);
            state->setLineCap(PGFround);
            state->moveTo(tempdima + 0.6*state->lineWidth,(double)0);
            state->lineTo(tempdima,0);
            state->setDraw(true);
          }
          else
          {
            state->curveTo(0.066666*tempdima,0.3125*tempdimb,0.8*tempdima,0.03125*tempdimb,tempdima,0);
            state->curveTo(0.95*tempdima,-0.125*state->lineWidth,0.933333*tempdima,-0.125*state->lineWidth,0.933333*tempdima,-0.125*state->lineWidth);
            state->lineTo(0.8 * tempdima - 0.5 * state->lineWidth,-0.125*state->lineWidth);
            state->setDraw(true);
          }
        }
        else
        {
          state->curveTo(0.066666*tempdima,0.3125*tempdimb,0.8*tempdima,0.03125*tempdimb,tempdima,0);
          state->curveTo(0.8*tempdima,-0.03125*tempdimb,0.066666*tempdima,-0.3125*tempdimb,0,-.5*tempdimb);
          state->setLineWidth(state->arrowLineWidth);
          state->setDraw(true);
        }
      }      
      break;
  }
}

void XWTikzArrow::ComputerModernRightarrow(XWTikzState * state)
{
  double tempdima = 0;
  double tempdimb = 0;
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        state->setArrowLength(state->lineWidthDependent(1.6,2.2,0));
        state->setArrowWidth(state->lengthDependent(0,2.096774,0));
        state->setArrowLineWidth(state->lineWidthDependent(0,1,1));
        state->setLineJoin(PGFround);
        state->setLineCap(PGFround);
        tempdima = state->arrowLength - state->arrowLineWidth;
        tempdimb = state->arrowWidth - state->arrowLineWidth;
        double xc = 0;
        double xa = 0;
        if (state->arrowLineJoin != PGFround)
        {
          double mathresult = tempdima / tempdimb;
          double quot = mathresult;
          x = 49.44662 * mathresult * mathresult + 1;
          mathresult = sqrt(x);
          xc = 0.5 * mathresult * state->arrowLineWidth;
          xa = 3.51591 * quot * state->arrowLineWidth;
        }
        if (state->arrowLineJoin == PGFround)
          tipEnd = 0.5 * state->arrowLineWidth;
        else
        {
          
          tipEnd = xc;
          if (state->arrowHarpoon)
            tipEnd += xa;
        }
        visualBackEnd = 0.5 * state->arrowLineWidth;
        if (state->arrowReversed)
        {
          if (state->arrowHarpoon)
            lineEnd = 0.5 * state->arrowLineWidth;
          else
            lineEnd = -0.5 * state->arrowLineWidth;
        }
        else
          lineEnd = -0.5 * state->arrowLineWidth;
        backEnd = -tempdima - 0.5 * state->arrowLineWidth;
        visualTipEnd = tipEnd;
      }      
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      state->setDash(-1);
        if (state->arrowLineJoin == PGFround)
          state->setLineJoin(PGFround);
        else
          state->setLineJoin(PGFmiter);
        if (state->arrowLineCap == PGFround)
          state->setLineCap(PGFround);
        else
          state->setLineCap(PGFbutt);
        if (state->lineWidth != state->arrowLineWidth)
          state->setLineWidth(state->arrowLineWidth);
        state->moveTo(-tempdima, 0.5*tempdimb);
        state->curveTo(-0.81731*tempdima,0.2*tempdimb,-0.41019*tempdima,0.05833333*tempdimb,0,0);
        if (state->arrowHarpoon)
        {
          if (state->arrowReversed)
            state->lineTo(0.5 * state->arrowLineWidth,0);
          else
            state->lineTo(-state->arrowLineWidth,0);
        }
        else
          state->curveTo(-0.41019*tempdima,-0.05833333*tempdimb,-0.81731*tempdima,-0.2*tempdimb,-tempdima,-0.5*tempdimb);
        state->setDraw(true);
      break;
  }
}

void XWTikzArrow::diamondArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        double tmpa = 0.4 + 0.275 * state->lineWidth;
        double tmpb = 13 * tmpa + 0.5 * state->lineWidth;
        backEnd = -tmpb;
        tmpb = tmpa + 0.5 * state->lineWidth;
        tipEnd = tmpb;
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }      
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        double a = 0.4 + 0.275 * state->lineWidth;
        state->setDash(-1);
        state->setLineJoin(PGFround);
        state->moveTo(a, (double)0);
        state->lineTo(-6 * a, 4 * a);
        state->lineTo(-13 * a, 0);
        state->lineTo(-6 * a, -4 * a);
        state->closePath();
        state->setFill(true);
        state->setDraw(true);
      }
      break;
  }
}

void XWTikzArrow::directioneeArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        double tempdima = 4 * 0.5;
        tipEnd = tempdima;
        backEnd = -tempdima;
      }
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        double tempdima = 4;
        state->setDash(-1);
        state->setLineJoin(PGFround);
        state->moveTo(-0.5*tempdima, 0.38268343237*tempdima);
        state->lineTo(0.5*tempdima, 0);
        state->lineTo(-0.5*tempdima, -0.38268343237*tempdima);
        state->closePath();
        state->setFill(true);
        state->setDraw(true);
      }
      break;
  }
}

void XWTikzArrow::dotArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        double tmpa = 0.4 + 0.2 * state->lineWidth;
        double tmpb = 7.5 * tmpa + 0.5 * state->lineWidth;
        backEnd = -tmpb;
        tmpb = 1.5 * tmpa + 0.5 * state->lineWidth;
        tipEnd = tmpb;
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }      
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        double a = 0.4 + 0.2 * state->lineWidth;
        state->setDash(-1);
        QPointF c(-3*a,0);
        state->addCircle(c, 4.5 * a);
        state->setDraw(true);
        state->setFill(true);
      }
      break;
  }
}

void XWTikzArrow::fastcapArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        backEnd = -1 * state->lineWidth;
        tipEnd = 2 * state->lineWidth;
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }      
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        state->moveTo(-1 * state->lineWidth,0.5*state->lineWidth);
        state->lineTo(0.5 * state->lineWidth,0.5 * state->lineWidth);
        state->lineTo(state->lineWidth,0);
        state->lineTo(0.5 * state->lineWidth,-0.5 * state->lineWidth);
        state->lineTo(-1 * state->lineWidth,-0.5 * state->lineWidth);
        state->closePath();
        state->moveTo(state->lineWidth,0.5*state->lineWidth);
        state->lineTo(1.5 * state->lineWidth,0.5 * state->lineWidth);
        state->lineTo(2 * state->lineWidth,0);
        state->lineTo(1.5 * state->lineWidth,-0.5 * state->lineWidth);
        state->lineTo(state->lineWidth,-0.5 * state->lineWidth);
        state->lineTo(1.5 * state->lineWidth,0);
        state->closePath();
        state->setFill(true);
      }
      break;
  }
}

void XWTikzArrow::fastcapReversedArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        backEnd = -1 * state->lineWidth;
        tipEnd = 2 * state->lineWidth;
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }      
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        state->moveTo(-1 * state->lineWidth,0.5*state->lineWidth);
        state->lineTo(state->lineWidth,0.5 * state->lineWidth);
        state->lineTo(0.5*state->lineWidth,0);
        state->lineTo(state->lineWidth,-0.5 * state->lineWidth);
        state->lineTo(-1 * state->lineWidth,-0.5 * state->lineWidth);
        state->closePath();
        state->moveTo(1.5*state->lineWidth,0.5*state->lineWidth);
        state->lineTo(2 * state->lineWidth,0.5 * state->lineWidth);
        state->lineTo(1.5 * state->lineWidth,0);
        state->lineTo(2 * state->lineWidth,-0.5 * state->lineWidth);
        state->lineTo(1.5*state->lineWidth,-0.5 * state->lineWidth);
        state->lineTo(state->lineWidth,0);
        state->closePath();
        state->setFill(true);
      }
      break;
  }
}

void XWTikzArrow::FastRoundArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      state->setArrowLength(state->lineWidthDependent(0,0.5,0));
      state->setArrowInset(state->lineWidthDependent(0,0.5,0));
      tipEnd = state->arrowLength + state->arrowInset;
      if (state->arrowReversed)
        lineEnd = state->arrowLength + state->arrowInset;
      visualTipEnd = tipEnd;
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        double tempdima = 0.5*state->lineWidth;
        state->moveTo((double)0,tempdima);
        state->lineTo(state->arrowInset,tempdima);
        state->addShift(state->arrowInset,0);
        state->curveTo(0.55228475*state->arrowLength,tempdima,state->arrowLength,0.55228475*tempdima,state->arrowLength,0);
        state->curveTo(state->arrowLength,-0.55228475*tempdima,0.55228475*state->arrowLength,-tempdima,0,-tempdima);
        state->lineTo(0,-tempdima);
        state->curveTo(0.55228475*state->arrowLength,-tempdima,state->arrowLength,-0.55228475*tempdima,state->arrowLength,0);
        state->curveTo(state->arrowLength,0.55228475*tempdima,0.55228475*state->arrowLength,tempdima,0,tempdima);
        state->closePath();
        state->setFill(true);
      }
      break;
  }
}

void XWTikzArrow::FastTriangleArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      state->setArrowLength(state->lineWidthDependent(0,0.5,0));
      state->setArrowInset(state->lineWidthDependent(0,0.5,0));
      tipEnd = state->arrowLength + state->arrowInset;
      if (state->arrowReversed)
        lineEnd = state->arrowLength + state->arrowInset;
      visualTipEnd = tipEnd;
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        double tempdima = 0.5*state->lineWidth;
        state->moveTo((double)0,tempdima);
        state->lineTo(state->arrowInset,tempdima);
        state->lineTo(state->arrowInset + state->arrowLength,0);
        state->lineTo(state->arrowInset,-tempdima);
        state->lineTo(0,tempdima);
        state->lineTo(state->arrowLength,0);
        state->closePath();
        state->setFill(true);
      }
      break;
  }
}

void XWTikzArrow::HooksArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      state->setArrowLength(state->lineWidthDependent(0.75,1.25,0));
      state->setArrowWidth(state->lengthDependent(0,4,-1));
      state->setArrowLineWidth(state->lineWidthDependent(0,1,1));
      state->arrowLength -= 0.5 * state->arrowLineWidth;
      state->arrowWidth -= state->arrowLineWidth;
      if (state->arrowReversed)
      {
        if (state->arrowLineJoin == PGFround)
          backEnd = -0.5 * state->arrowLineWidth;
      }
      if (state->arrowArc < 90)
        tipEnd = state->arrowLength * sin(state->arrowArc) + 0.5 * state->arrowLineWidth;
      else if (state->arrowArc < 180)
      {
        tipEnd = state->arrowLength + 0.5 * state->arrowLineWidth;
        if (state->arrowLineCap == PGFround)
          backEnd = -0.5 * state->arrowLineWidth;
      }
      else if (state->arrowArc < 270)
      {
        tipEnd = state->arrowLength + 0.5 * state->arrowLineWidth;
        if (state->arrowArc > 180)
          backEnd = state->arrowLength * sin(state->arrowArc) - 0.5 * state->arrowLineWidth;
        else
        {
           if (state->arrowLineCap == PGFround)
             backEnd = -0.5 * state->arrowLineWidth;
        }          
      }
      else
      {
        tipEnd = state->arrowLength + 0.5 * state->arrowLineWidth;
        backEnd = -state->arrowLength - 0.5 * state->arrowLineWidth;
      }
      if (state->arrowReversed)
        lineEnd = 0.5 * state->arrowLineWidth;
      else
      {
        if (state->arrowHarpoon)
          lineEnd = 0;
        else
          lineEnd = 0.25 * state->arrowLineWidth;
      }
      state->arrowArc -= 90;
      visualTipEnd = tipEnd;
      visualBackEnd = backEnd;
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      state->setDash(-1);
      if (state->arrowLineJoin != PGFround)
        state->setLineJoin(PGFmiter);
      if (state->arrowLineCap != PGFround)
        state->setLineCap(PGFbutt);
      if (state->lineWidth != state->arrowLineWidth)
        state->setLineWidth(state->arrowLineWidth);
      state->scale(state->arrowLength, 0.25*state->arrowWidth);
      state->moveTo(cos(state->arrowArc), 1 + sin(state->arrowArc));
      state->setStartAngle(state->arrowArc);
      state->setEndAngle(-90);
      state->setXRadius(1);
      state->setYRadius(1);
      state->addArc();
      if (!state->arrowHarpoon)
      {
        state->setStartAngle(90);
        state->setEndAngle(-state->arrowArc);
        state->addArc();
      }
      if (state->arrowHarpoon)
      {
        if (state->arrowReversed)
          state->lineTo(state->lineWidth,0);
      }
      state->setDraw(true);
      break;
  }
}

void XWTikzArrow::hooksArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        backEnd = -0.5 * state->lineWidth;
        double tmpa = 0.4 + 0.2 * state->lineWidth;
        double tmpb = 3.75 * tmpa + 0.5 * state->lineWidth;
        tipEnd = tmpb;
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      double a = 0.4 + 0.2 * state->lineWidth;
      state->setDash(-1);
      state->setLineCap(PGFround);
      state->moveTo((double)0, (double)0);
      state->lineTo(0.75*a, 0);
      state->curveTo(2.415*a, 0, 3.75*a, 1.665*a, 3.75*a, 3*a);      
      state->curveTo(3.75*a, 4.665*a, 2.415*a, 6*a, 0.75*a, 6*a);
      state->moveTo(0.75*a, 0);
      state->curveTo(2.415*a, 0, 3.75*a, -1.665*a, 3.75*a, -3*a);      
      state->curveTo(3.75*a, -4.665*a, 2.415*a, -6*a, 0.75*a, -6*a);
      state->setDraw(true);
      break;
  }
}

void XWTikzArrow::ImpliesArrow(XWTikzState * state)
{
  double tempdima = 0;
  double tempdimb = 0;
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        double xa = state->innerLineWidth;
        tempdima = 0.25*state->lineWidth + 0.25 * xa;
        tempdimb = 0.5*state->lineWidth - 0.5 * xa;
        tipEnd = 2.06 * tempdima + state->arrowSep + 0.5 * tempdimb;
        backEnd = -1.36 * tempdima + 0.5 * tempdimb;
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        state->addShift(0.06*tempdima,0);
        state->setLineWidth(tempdimb);
        state->setLineCap(PGFround);
        state->setLineJoin(PGFround);
        state->moveTo(-1.4*tempdima, 2.65*tempdima);
        state->curveTo(-0.75*tempdima, 1.25*tempdima,tempdima,0.05*tempdima,2*tempdima,0);
        state->curveTo(tempdima,-0.05*tempdima,-0.75*tempdima,-1.25*tempdima,-1.4*tempdima,-2.65*tempdima);
        state->setDraw(true);
      }
      break;
  }
}

void XWTikzArrow::impliesArrow(XWTikzState * state)
{
  double tmpa = 0.25 * state->lineWidth + 0.25 * state->innerLineWidth;
  double tmpb = 0.5 * state->lineWidth - 0.5*state->innerLineWidth;
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {        
        backEnd = -1.36 * tmpa - 0.5 * tmpb;
        tipEnd = 2.06 * tmpa + 0.5 * tmpb;
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      state->addShift(0.06*tmpa,0);
      state->setLineWidth(tmpb);
      state->setDash(-1);
      state->setLineCap(PGFround);
      state->setLineJoin(PGFround);
      state->moveTo(-1.4 * tmpa, 2.65 * tmpa);
      state->curveTo(-0.75*tmpa, 1.25*tmpa, tmpa, 0.05*tmpa, 2*tmpa, 0);      
      state->curveTo(tmpa, -0.05*tmpa, -0.75*tmpa, -1.25*tmpa, -1.4*tmpa, -2.65*tmpa);
      state->setDraw(true);
      break;
  }
}

void XWTikzArrow::KiteArrow(XWTikzState * state)
{
  double tempdima = 0;
  double tempdimb = 0;
  double yc = 0;
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        if (shape == PGFKite)
        {
          state->setArrowLength(state->lineWidthDependent(3.6,5.4,0));
          state->setArrowWidth(state->lengthDependent(0,0.5,0));
          state->setArrowInset(state->lengthDependent(0,0.25,0));
          state->setArrowLineWidth(state->lineWidthDependent(0,1,1));
        }
        else if (shape == PGFDiamond)
          state->setArrowWidth(state->lengthDependent(0,0.5,0));
        else
        {
          state->setArrowLength(state->lineWidthDependent(3,4,0));
          state->setArrowWidth(state->lengthDependent(0,1,0));
          state->setArrowInset(state->lengthDependent(0,0.5,0));
        }
        x = 0.4 * state->arrowLength;
        if (x < state->arrowLineWidth)
          state->arrowLineWidth = x;
        x = 0.4 * state->arrowWidth;
        if (x < state->arrowLineWidth)
          state->arrowLineWidth = x;
        double xa = state->arrowLength - state->arrowInset;
        double mathresult = xa / state->arrowWidth;
        double quot = mathresult;
        x = 4 * mathresult * mathresult + 1;
        mathresult = sqrt(x);
        double xc = 0.5 * mathresult * state->arrowLineWidth;
        xa = quot * state->arrowLineWidth;
        mathresult = state->arrowInset / state->arrowWidth;
        quot = mathresult;
        x = 4 * mathresult * mathresult + 1;
        mathresult = sqrt(x);
        yc = 0.5 * mathresult * state->arrowLineWidth;
        double ya = quot * state->arrowLineWidth;
        tempdimb = 0.5 * state->arrowWidth;
        tempdima = state->arrowLength - state->arrowInset;
        mathresult = atan2(tempdima, tempdimb);
        double yb = mathresult;
        mathresult = atan2(state->arrowInset, tempdimb);
        ya = mathresult;
        yb += ya;
        yb *= 0.5;
        if (yb == 45)
          mathresult = 1.414213;
        else
          mathresult = 1 / sin(yb);
        yc = 0.5 * mathresult * state->arrowLineWidth;
        ya = -ya;
        ya -= 90;
        ya += yb;
        double xb = cos(ya) * yc;
        yb = sin(ya) * yc;
        tempdima = state->arrowLength - xc;
        tempdimb = 0.5 * state->arrowWidth + yb;
        if (state->arrowLineJoin == PGFround)
          backEnd = ya - 0.5 * state->arrowLineWidth;
        else
          backEnd = 0;
        if (state->arrowReversed)
          lineEnd = tempdima + state->arrowLineWidth - state->lineWidth;
        else
          lineEnd = tempdima - state->arrowLineWidth + state->lineWidth;
        if (state->arrowLineJoin == PGFround)
          tipEnd = tempdima + 0.5 * state->arrowLineWidth;
        else
        {
          if (state->arrowHarpoon)
            tipEnd = state->arrowLength + xa;
          else
            tipEnd = state->arrowLength;
        }
        state->arrowInset += xb;
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      state->setDash(-1);
      if (state->arrowLineJoin == PGFround)
        state->setLineJoin(PGFround);
      else
        state->setLineJoin(PGFmiter);
      if (state->lineWidth != state->arrowLineWidth)
        state->setLineWidth(state->arrowLineWidth);
      state->moveTo(tempdima, (double)0);
      state->lineTo(state->arrowInset, tempdimb);
      state->lineTo(yc, 0);
      if (!state->arrowHarpoon)
        state->lineTo(state->arrowInset, -tempdimb);
      state->closePath();
      if (!state->isArrowFillSet)
        state->setDraw(true);
      else
      {
        if (state->arrowLineWidth > 0)
        {
          state->setDraw(true);
          state->setFill(true);
        }
        else
          state->setFill(true);
      }
      break;
  }
}

void XWTikzArrow::LatexArrow(XWTikzState * state)
{
  double tempdima = 0;
  double tempdimb = 0;
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        state->setArrowLength(state->lineWidthDependent(3,4.5,0.8));
        state->setArrowWidth(state->lengthDependent(0,0.75,0));
        state->setArrowLineWidth(state->lineWidthDependent(0,1,1));
        x = 0.2 * state->arrowLength;
        if (x < state->arrowLineWidth)
          state->arrowLineWidth = x;
        double mathresult = state->arrowLength / state->arrowWidth;
        double quot = mathresult;
        x = 9 * mathresult * mathresult + 1;
        mathresult = sqrt(x);
        double xc = mathresult * state->arrowLineWidth;
        double xa = quot * state->arrowLineWidth;
        tempdima = state->arrowLength - 0.5 * xc - 0.5 * state->arrowLineWidth;
        backEnd = -0.5 * state->arrowLineWidth;
        if (state->arrowReversed)
        {
          if (state->innerLineWidth > 0)
            lineEnd = 0;
          else
            lineEnd = tempdima - 0.5 * state->lineWidth;
        }
        else
          lineEnd = 0;
        if (state->arrowLineJoin == PGFround)
          tipEnd = tempdima + 0.5 * state->arrowLineWidth;
        else
        {
          tipEnd = state->arrowLength - 0.5 * state->arrowLineWidth;
          if (state->arrowHarpoon)
            tipEnd += 1.5 * xa;
        }
        xa = 0.3 * state->arrowLength;
        double ya = 0.2333333 * state->arrowWidth;
        QVector2D v(xa,ya);
        v.normalize();
        xa = v.x();
        ya = v.y() + 1;
        mathresult = ya / xa;
        tempdimb = -0.5 * mathresult * state->arrowLineWidth + 0.5 * state->arrowWidth;
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      state->setDash(-1);
      if (state->arrowLineJoin == PGFround)
        state->setLineJoin(PGFround);
      else
        state->setLineJoin(PGFmiter);
      if (state->lineWidth != state->arrowLineWidth)
        state->setLineWidth(state->arrowLineWidth);
      state->moveTo(tempdima, (double)0);
      state->curveTo(0.877192*tempdima,0.077922*tempdimb,0.337381*tempdima,0.519480*tempdimb,0,tempdimb);
      if (state->arrowHarpoon)
        state->lineTo(0,0);
      else
      {
        state->lineTo(0, -tempdimb);
        state->curveTo(0.337381*tempdima,-0.519480*tempdimb,0.877192*tempdima,-0.077922*tempdimb,tempdima,0);
      }
      state->closePath();
      if (!state->isArrowFillSet)
        state->setDraw(true);
      else
      {
        if (state->arrowLineWidth > 0)
        {
          state->setDraw(true);
          state->setFill(true);
        }
        else
          state->setFill(true);
      }
      break;
  }
}

void XWTikzArrow::latexArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        double tmpa = 0.28;
        double tmpb = state->lineWidth;
        if (state->innerLineWidth > 0)
          tmpb = 0.6 * state->lineWidth - 0.4 * state->innerLineWidth;
        tmpa = tmpa + 0.3 * tmpb;
        tipEnd = 9 * tmpa;
        backEnd = -tmpa;
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      double a = 0.28;
      double b = state->lineWidth;
      if (state->innerLineWidth > 0)
        b = 0.6 * state->lineWidth - 0.4 * state->innerLineWidth;
      a = a + 0.3 * b;
      state->moveTo(9*a, (double)0);
      state->curveTo(6.3333*a, 0.5*a, 2*a, 2*a, -a, 3.75*a);
      state->lineTo(-a, -3.75*a);
      state->curveTo(2*a, -2*a, 6.3333*a, -0.5*a, 9*a, 0);
      state->closePath();
      state->setFill(true);
      break;
  }
}

void XWTikzArrow::latexFleshyArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        double tmpa = 0.28 + 0.3 * state->lineWidth;
        backEnd = -4 * tmpa;
        tipEnd = 6 * tmpa;        
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      double a = 0.28 + 0.3 * state->lineWidth;
      state->moveTo(6*a, (double)0);
      state->curveTo(-3.5*a, 0.5*a, -1*a, 1.5*a, -4 * a, 3.75*a);
      state->curveTo(-1.5*a, a, -1.5*a, -1*a, -4*a, -3.75*a);
      state->curveTo(-1*a, -1.5*a, 3.5*a, -0.5*a, 6*a, 0);
      state->closePath();
      state->setFill(true);
      break;
  }
}

void XWTikzArrow::leftHookArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        backEnd = -0.5 * state->lineWidth;
        double tmpa = 0.4 + 0.2 * state->lineWidth;
        double tmpb = 3.75 * tmpa + 0.5 * state->lineWidth;
        tipEnd = tmpb;
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      double a = 0.4 + 0.2 * state->lineWidth;
      state->setDash(-1);
      state->setLineCap(PGFround);
      state->moveTo((double)0, (double)0);
      state->lineTo(0.75*a, 0);
      state->curveTo(2.415*a, 0, 3.75*a, 1.665*a, 3.75*a, 3*a);      
      state->curveTo(3.75*a, 4.665*a, 2.415*a, 6*a, 0.75*a, 6*a);
      state->setDraw(true);
      break;
  }
}

void XWTikzArrow::leftToArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        backEnd = -0.84 - 1.3 * state->lineWidth;
        tipEnd = 0.21 + 0.625 * state->lineWidth;        
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      double a = 0.28 + 0.3 * state->lineWidth;
      state->setLineWidth(0.8*state->lineWidth);
      state->setDash(-1);
      state->setLineCap(PGFround);
      state->setLineJoin(PGFround);
      state->moveTo(-3*a, 4*a);
      state->curveTo(-2.75*a, 2.5*a, 0, 0.25*a, 0.75 * a, 0);
      state->curveTo(0.55*a, -0.125*state->lineWidth, 0.5*a, -0.125*state->lineWidth, 0.5*a, -0.125*state->lineWidth);
      state->lineTo(0, -0.125*state->lineWidth);
      state->setDraw(true);
      break;
  }
}

void XWTikzArrow::leftToReversedArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        backEnd = -1 * state->lineWidth;
        double tmpa = 0.28 + 0.3 * state->lineWidth;
        double tmpb = 3.75 * tmpa + 0.9 * state->lineWidth;
        tipEnd = tmpb;        
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      double a = 0.28 + 0.3 * state->lineWidth;
      state->setDash(-1);
      state->setLineJoin(PGFround);
      state = state->save(false);      
      state->setLineCap(PGFbutt);      
      state->moveTo(0.5*state->lineWidth, (double)0);
      state->lineTo(-0.1*state->lineWidth, 0);
      state->setDraw(true);      
      state = state->restore();
      state->setLineCap(PGFround);
      state->setLineWidth(0.8*state->lineWidth);
      state->addShift(0.625 * state->lineWidth,0);
      state->moveTo(3.75*a, 4*a);
      state->curveTo(3.5*a, 2.5*a, 0.75*a, 0.25*a, 0, 0.125*state->lineWidth);
      state->moveTo(3.75*a, 4*a);
      state->curveTo(3.5*a, 2.5*a, 0.75*a, 0.25*a, 0, -0.125*state->lineWidth);
      state->setDraw(true);
      break;
  }
}

void XWTikzArrow::openDiamondArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        backEnd = -0.5 * state->lineWidth;
        double tmpa = 0.4 + 0.275 * state->lineWidth;
        double tmpb = 14 * tmpa + 0.5 * state->lineWidth;
        tipEnd = tmpb;
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }      
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        double a = 0.4 + 0.275 * state->lineWidth;
        state->setDash(-1);
        state->setLineJoin(PGFround);
        state->moveTo(14 * a, (double)0);
        state->lineTo(7 * a, 4 * a);
        state->lineTo(0, 0);
        state->lineTo(7 * a, -4 * a);
        state->closePath();
        state->setDraw(true);
      }
      break;
  }
}

void XWTikzArrow::openDotArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        backEnd = -0.5 * state->lineWidth;
        double tmpa = 0.4 + 0.2 * state->lineWidth;        
        double tmpb = 9 * tmpa + 0.5 * state->lineWidth;
        tipEnd = tmpb;
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }      
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        double a = 0.4 + 0.2 * state->lineWidth;
        state->setDash(-1);
        QPointF c(4.5*a,0);
        state->addCircle(c, 4.5 * a);
        state->setDraw(true);
      }
      break;
  }
}

void XWTikzArrow::openSquareArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        double arrowsize = 0.4 + 0.275 * state->lineWidth;
        backEnd = -0.5 * state->lineWidth;
        arrowsize = arrowsize + 7 * arrowsize;
        arrowsize += 0.5 * state->lineWidth;
        tipEnd = arrowsize;
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }      
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        double arrowsize = 0.4 + 0.275 * state->lineWidth;
        state->setDash(-1);
        state->setLineJoin(PGFround);
        state->moveTo(8 * arrowsize, 4 * arrowsize);
        state->lineTo(0, 4 * arrowsize);
        state->lineTo(0, -4 * arrowsize);
        state->lineTo(8 * arrowsize, -4 * arrowsize);
        state->closePath();
        state->setDraw(true);
      }
      break;
  }
}

void XWTikzArrow::openTriangle45Arrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        backEnd = -0.5 * state->lineWidth;
        double tmpa = 0.5 + 0.25 * state->lineWidth;
        double tmpb = 9.205 * tmpa + 1.28 * state->lineWidth;
        tipEnd = tmpb;
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }      
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        double a = 0.5 + 0.25 * state->lineWidth;
        state->setDash(-1);
        state->setLineJoin(PGFmiter);
        state->moveTo(9.205 + 10 * a * cos((double)157), 10 * a * sin((double)157));
        state->lineTo(9.205*a, 0);
        state->lineTo(9.205 + 10 * a * cos((double)-157), 10 * a * sin((double)-157));
        state->closePath();
        state->setDraw(true);
      }
      break;
  }
}

void XWTikzArrow::openTriangle45ReversedArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        backEnd = -1.28 * state->lineWidth;
        double tmpa = 0.5 + 0.25 * state->lineWidth;
        double tmpb = 9.205 * tmpa + 0.5 * state->lineWidth;
        tipEnd = tmpb;
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }      
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        double a = 0.5 + 0.25 * state->lineWidth;
        state->setDash(-1);
        state->setLineJoin(PGFmiter);
        state->moveTo(10 * a * cos((double)23), 10 * a * sin((double)23));
        state->lineTo(0, 0);
        state->lineTo(10 * a * cos((double)-23), 10 * a * sin((double)-23));
        state->closePath();
        state->setDraw(true);
      }
      break;
  }
}

void XWTikzArrow::openTriangle60Arrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        backEnd = -0.5 * state->lineWidth;
        double tmpa = 0.5 + 0.25 * state->lineWidth;
        double tmpb = 7.794 * tmpa + state->lineWidth;
        tipEnd = tmpb;
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }      
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        double a = 0.5 + 0.25 * state->lineWidth;
        state->setDash(-1);
        state->setLineJoin(PGFmiter);
        state->moveTo(7.794 + 9 * a * cos((double)150), 9 * a * sin((double)150));
        state->lineTo(7.794*a, 0);
        state->lineTo(7.794 + 9 * a * cos((double)-150), 9 * a * sin((double)-150));
        state->closePath();
        state->setDraw(true);
      }
      break;
  }
}

void XWTikzArrow::openTriangle60ReversedArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        backEnd = -state->lineWidth;
        double tmpa = 0.5 + 0.25 * state->lineWidth;
        double tmpb = 7.794 * tmpa + 0.5 * state->lineWidth;
        tipEnd = tmpb;
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }      
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        double a = 0.5 + 0.25 * state->lineWidth;
        state->setDash(-1);
        state->setLineJoin(PGFmiter);
        state->moveTo(9 * a * cos((double)30), 9 * a * sin((double)30));
        state->lineTo(0, 0);
        state->lineTo(9 * a * cos((double)-30), 9 * a * sin((double)-30));
        state->closePath();
        state->setDraw(true);
      }
      break;
  }
}

void XWTikzArrow::openTriangle90Arrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        backEnd = -0.5 * state->lineWidth;
        double tmpa = 0.5 + 0.25 * state->lineWidth;
        double tmpb = 6 * tmpa + 0.707 * state->lineWidth;
        tipEnd = tmpb;
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }      
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        double a = 0.5 + 0.25 * state->lineWidth;
        state->setDash(-1);
        state->setLineJoin(PGFmiter);
        state->moveTo((double)0, -6 * a);
        state->lineTo(6*a, 0);
        state->lineTo(0, 6 * a);
        state->closePath();
        state->setDraw(true);
      }
      break;
  }
}

void XWTikzArrow::openTriangle90ReversedArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        backEnd = -0.707 * state->lineWidth;
        double tmpa = 0.5 + 0.25 * state->lineWidth;
        double tmpb = 6 * tmpa + 0.5 * state->lineWidth;
        tipEnd = tmpb;
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }      
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        double a = 0.5 + 0.25 * state->lineWidth;
        state->setDash(-1);
        state->setLineJoin(PGFmiter);
        state->moveTo(6 * a, -6 * a);
        state->lineTo(0, 0);
        state->lineTo(6 * a, 6 * a);
        state->closePath();
        state->setDraw(true);
      }
      break;
  }
}

void XWTikzArrow::RaysArrow(XWTikzState * state)
{
  int arrown = 4;
  double inc = 0;
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        state->setArrowLength(state->lineWidthDependent(3,4,0));
        state->setArrowWidth(state->lengthDependent(0,1,0));
        state->setArrowLineWidth(state->lineWidthDependent(0,1,1));
        double mathresult = 360 / arrown;
        inc = mathresult;
        double xa = inc;
        double ya = 0;
        if (arrown % 2 == 1)
          ya = 0.25 *xa;
        else
          ya = 0.5 * xa;
        double mathresultx = cos(ya);
        double mathresulty = sin(ya);
        x = 0.5 * state->arrowLength;
        xa = mathresultx * x;
        if (state->arrowLineCap == PGFround)
          xa += state->arrowLineWidth;
        else
        {
          QVector2D v(mathresultx * state->arrowLength,mathresulty * state->arrowWidth);
          v.normalize();
          x = v.x();
          y = v.y();
          x = 0.5 * state->arrowLineWidth;
          xa += y * x;
        }
        tipEnd = xa;
        backEnd = -xa;
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        QTransform ot = state->transform;
        state->setDash(-1);
        if (state->arrowLineCap == PGFround)
          state->setLineCap(PGFround);
        else
          state->setLineCap(PGFbutt);
        if (state->lineWidth != state->arrowLineWidth)
          state->setLineWidth(state->arrowLineWidth);
        state->scale(0.5*state->arrowLength,0.5*state->arrowWidth);
        double xa = inc;
        double ya = 0;
        if (arrown % 2 == 1)
          ya = 0.25 *xa;
        else
          ya = 0.5 * xa;
        double yb = 0;
        if (state->arrowHarpoon)
          yb = 180;
        else
          yb = 360;        
        while (ya < yb)
        {
          state->moveTo(cos(ya), sin(ya));
          state->lineTo(0,0);
          if (state->arrowHarpoon)
          {
            if (ya <= xa)
            {
              state->transform = ot;
              state->lineTo(-0.5 * state->lineWidth, 0);
            }            
          }
          ya += xa;
        }
        state->setDraw(true);
      }
      break;
  }
}

void XWTikzArrow::rightHookArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        backEnd = -0.5 * state->lineWidth;
        double tmpa = 0.4 + 0.2 * state->lineWidth;
        double tmpb = 3.75 * tmpa + 0.5 * state->lineWidth;
        tipEnd = tmpb;
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      double a = 0.4 + 0.2 * state->lineWidth;
      state->setDash(-1);
      state->setLineCap(PGFround);
      state->moveTo((double)0, (double)0);
      state->lineTo(0.75*a, 0);
      state->curveTo(2.415*a, 0, 3.75*a, -1.665*a, 3.75*a, -3*a);      
      state->curveTo(3.75*a, -4.665*a, 2.415*a, -6*a, 0.75*a, -6*a);
      state->setDraw(true);
      break;
  }
}

void XWTikzArrow::rightToArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        backEnd = -0.84 - 1.3 * state->lineWidth;
        tipEnd = 0.21 + 0.625 * state->lineWidth;        
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      double a = 0.28 + 0.3 * state->lineWidth;
      state->setLineWidth(0.8*state->lineWidth);
      state->setDash(-1);
      state->setLineCap(PGFround);
      state->setLineJoin(PGFround);
      state->moveTo(-3*a, -4*a);
      state->curveTo(-2.75*a, -2.5*a, 0, -0.25*a, 0.75 * a, 0);
      state->curveTo(0.55*a, 0.125*state->lineWidth, 0.5*a, 0.125*state->lineWidth, 0.5*a, 0.125*state->lineWidth);
      state->lineTo(0, 0.125*state->lineWidth);
      state->setDraw(true);
      break;
  }
}

void XWTikzArrow::rightToReversedArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        backEnd = -1 * state->lineWidth;
        double tmpa = 0.28 + 1.3 * state->lineWidth;
        double tmpb = 3.75 * tmpa + 0.9 * state->lineWidth;
        tipEnd = tmpb;        
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      double a = 0.28 + 0.3 * state->lineWidth;
      state->setDash(-1);
      state->setLineJoin(PGFround);
      state = state->save(false);      
      state->setLineCap(PGFbutt);      
      state->moveTo(0.5*state->lineWidth, (double)0);
      state->lineTo(-0.1*state->lineWidth, 0);
      state->setDraw(true);
      state = state->restore();
      state->setLineCap(PGFround);
      state->setLineWidth(0.8*state->lineWidth);
      state->addShift(0.625 * state->lineWidth,0);
      state->moveTo(3.75*a, -4*a);
      state->curveTo(3.5*a, -2.5*a, 0.75*a, -0.25*a, 0, -0.125*state->lineWidth);
      state->moveTo(3.75*a, -4*a);
      state->curveTo(3.5*a, 2.5*a, 0.75*a, -0.25*a, 0, 0.125*state->lineWidth);
      state->setDraw(true);
      break;
  }
}

void XWTikzArrow::roundBracketArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        double tmpa = 2 + 1.5 * state->lineWidth;
        double tmpb = 0.0625 * tmpa + 0.5 * state->lineWidth;
        tipEnd = tmpb;
        tmpb = 0.5 * tmpa + 0.5 * state->lineWidth;
        backEnd = -tmpb;
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;      
      }
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        double a = 2 + 1.5 * state->lineWidth;
        state->setDash(-1);
        state->setLineCap(PGFround);
        state->moveTo(-0.5*a, -a);
        state->curveTo(0.25*a, -0.5*a,0.25*a, 0.5*a,-0.5*a,a);
        state->setDraw(true);
      }
      break;
  }
}

void XWTikzArrow::RoundCapArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        state->setArrowLength(state->lineWidthDependent(0,0.5,0)); 
        if (state->arrowReversed)
        {
          tipEnd = state->arrowLength;
          lineEnd = state->arrowLength + 0.5 * state->lineWidth;
        }
        else
        {
          tipEnd = state->arrowLength;
          lineEnd = -0.5 * state->lineWidth;
        }
      }
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        if (state->arrowReversed)
        {
          state->scale(-1,1);
          state->addShift(-state->arrowLength, 0);
        }
        double tempdima = 0.5*state->lineWidth;
        state->moveTo(-0.75*state->lineWidth,tempdima);
        state->lineTo(0,tempdima);
        if (state->arrowReversed)
        {
          state->lineTo(state->arrowLength,tempdima);
          state->addShift(state->arrowLength, 0);
          state->scale(-1,1);
        }
        state->curveTo(0.55228475*state->arrowLength,tempdima,state->arrowLength,0.55228475*tempdima,state->arrowLength,0);
        state->curveTo(state->arrowLength,-0.55228475*tempdima,0.55228475*state->arrowLength,-tempdima,0,-tempdima);
        state->lineTo(-0.75*state->arrowLength,-tempdima);
        state->closePath();
        state->setFill(true);
      }
      break;
  }
}

void XWTikzArrow::roundcapArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        backEnd = 0;
        tipEnd = state->lineWidth;
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }      
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        state->setDash(-1);
        state->setLineCap(PGFround);
        state->moveTo((double)0,(double)0);
        state->lineTo(0.5 * state->lineWidth,0);
        state->setDraw(true);
      }
      break;
  }
}

void XWTikzArrow::serifcmArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        double tmpa = 0.4 + 0.45 * state->lineWidth;
        backEnd = -0.75 * tmpa;
        tipEnd = 0.04 * state->lineWidth;
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }      
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        double a = 0.4 + 0.45 * state->lineWidth;
        state->addShift(0.04 * state->lineWidth,0);
        state->moveTo(-0.75*a, 0.5 * a);
        state->curveTo(-0.375*a, 0.5*state->lineWidth,-0.375*a, 0.7*state->lineWidth,-0.375*a,1.95*a);
        state->lineTo(0, 1.95 * a);
        state->curveTo(-0.04 * state->lineWidth, 0.5*a,-0.04 * state->lineWidth, -0.5*a,0,-1.95*a);
        state->lineTo(-0.375*a, -1.95*a);
        state->curveTo(-0.375*a, -0.7*state->lineWidth,-0.375*a, -0.5*state->lineWidth,-0.75*a, -0.5*state->lineWidth);
        state->closePath();
        state->setFill(true);
      }
      break;
  }
}

void XWTikzArrow::spaceArrow(XWTikzState * state)
{
  tipEnd += 0.88 + 0.3 * state->lineWidth;
}

void XWTikzArrow::spacedAngle45Arrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      angle45Arrow(state);
      spaceArrow(state);   
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      angle45Arrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedAngle45ReversedArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      angle45Arrow(state);
      spaceArrow(state);  
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      state->scale(-1,1);   
      angle45Arrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedAngle60Arrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      angle60Arrow(state);
      spaceArrow(state);   
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      angle60Arrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedAngle60ReversedArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      angle60Arrow(state);
      spaceArrow(state);  
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      state->scale(-1,1);   
      angle60Arrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedAngle90Arrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      angle90Arrow(state);
      spaceArrow(state);   
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      angle90Arrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedAngle90ReversedArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      angle90Arrow(state);
      spaceArrow(state);  
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      state->scale(-1,1);   
      angle90Arrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedBarArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      barArrow(state);
      spaceArrow(state);
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      barArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedButtCapArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      buttcapArrow(state);
      spaceArrow(state);
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      buttcapArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedDiamondArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      diamondArrow(state);
      spaceArrow(state);
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      diamondArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedDotArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      dotArrow(state);
      spaceArrow(state);
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      dotArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedCastCapArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      fastcapArrow(state);
      spaceArrow(state);
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      fastcapArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedCastCapReversedArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      fastcapReversedArrow(state);
      spaceArrow(state);
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      fastcapReversedArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedHooksArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      hooksArrow(state);
      spaceArrow(state);
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      hooksArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedHooksReversedArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      hooksArrow(state);
      spaceArrow(state);  
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      state->scale(-1,1);   
      hooksArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedImpliesArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      impliesArrow(state);
      spaceArrow(state);
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      impliesArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedLatexArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      latexArrow(state);
      spaceArrow(state);   
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      latexArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedLatexFleshyArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      latexFleshyArrow(state);
      spaceArrow(state); 
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      latexFleshyArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedLatexFleshyReversedArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      latexFleshyArrow(state);
      spaceArrow(state);  
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      state->scale(-1,1);   
      latexFleshyArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedLatexReversedArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      latexArrow(state);
      spaceArrow(state);  
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      state->scale(-1,1);    
      latexArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedLeftHookArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      leftHookArrow(state);
      spaceArrow(state);
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      leftHookArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedLeftHookReversedArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      leftHookArrow(state);
      spaceArrow(state);  
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      state->scale(-1,1);    
      leftHookArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedLeftToArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      leftToArrow(state);
      spaceArrow(state);
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      leftToArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedLeftToReversedArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      leftToReversedArrow(state);
      spaceArrow(state);
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      leftToReversedArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedOpenDiamondArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      openDiamondArrow(state);
      spaceArrow(state);
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      openDiamondArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedOpenDotArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      openDotArrow(state);
      spaceArrow(state);
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      openDotArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedOpenSquareArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      openSquareArrow(state);
      spaceArrow(state);
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      openSquareArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedOpenTriangle45Arrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      openTriangle45Arrow(state);
      spaceArrow(state);
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      openTriangle45Arrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedOpenTriangle45ReversedArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      openTriangle45Arrow(state);
      spaceArrow(state);
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      state->scale(-1,1);  
      openTriangle45Arrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedOpenTriangle60Arrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      openTriangle60Arrow(state);
      spaceArrow(state);
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      openTriangle60Arrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedOpenTriangle60ReversedArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      openTriangle60Arrow(state);
      spaceArrow(state);
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      state->scale(-1,1);  
      openTriangle60Arrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedOpenTriangle90Arrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      openTriangle90Arrow(state);
      spaceArrow(state);
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      openTriangle90Arrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedOpenTriangle90ReversedArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      openTriangle90Arrow(state);
      spaceArrow(state);
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      state->scale(-1,1);  
      openTriangle90Arrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedRightHookArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      rightHookArrow(state);
      spaceArrow(state);   
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      rightHookArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedRightHookReversedArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      rightHookArrow(state);
      spaceArrow(state);
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      state->scale(-1,1);  
      rightHookArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedRightToArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      rightToArrow(state);
      spaceArrow(state);   
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      rightToArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedRightToReversedArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      rightToReversedArrow(state);
      spaceArrow(state);   
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      rightToReversedArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedRoundBracketLArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      roundBracketArrow(state);
      spaceArrow(state);   
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      roundBracketArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedRoundBracketRArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      roundBracketArrow(state);
      spaceArrow(state);
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      state->scale(-1,1);  
      roundBracketArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedRoundCapArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      roundcapArrow(state);
      spaceArrow(state);   
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      roundcapArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedSerifcmArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      serifcmArrow(state);
      spaceArrow(state);   
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      serifcmArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedSquareArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      squareArrow(state);
      spaceArrow(state);   
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      squareArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedSquareBracketArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      squareBracketArrow(state);
      spaceArrow(state);   
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      squareBracketArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedSquareBracketReversedArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      squareBracketArrow(state);
      spaceArrow(state); 
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      state->scale(-1,1);      
      squareBracketArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedStealthArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      stealthArrow(state);
      spaceArrow(state);   
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      stealthArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedStealthReversedArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      stealthArrow(state);
      spaceArrow(state); 
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      state->scale(-1,1);      
      stealthArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedStealthRoundedArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      stealthRoundedArrow(state);
      spaceArrow(state);  
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      stealthRoundedArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedStealthRoundedReversedArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      stealthRoundedArrow(state);
      spaceArrow(state);
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      state->scale(-1,1);  
      stealthRoundedArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedToArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      toArrow(state);
      spaceArrow(state);
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      toArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedToReversedArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      toReversedArrow(state);
      spaceArrow(state);
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      toReversedArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedTriangle45Arrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      triangle45Arrow(state);
      spaceArrow(state);
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      triangle45Arrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedTriangle45ReversedArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      triangle45Arrow(state);
      spaceArrow(state);
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      state->scale(-1,1);  
      triangle45Arrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedTriangle60Arrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      triangle60Arrow(state);
      spaceArrow(state);
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      triangle60Arrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedTriangle60ReversedArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      triangle60Arrow(state);
      spaceArrow(state);
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      state->scale(-1,1);  
      triangle60Arrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedTriangle90Arrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      triangle90Arrow(state);
      spaceArrow(state);
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      triangle90Arrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedTriangle90ReversedArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      triangle90Arrow(state);
      spaceArrow(state);
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      state->scale(-1,1);  
      triangle90Arrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedTriangle90CapArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      triangle90capArrow(state);
      spaceArrow(state);
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      triangle90capArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::spacedTriangle90CapReversedArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      triangle90capReversedArrow(state);
      spaceArrow(state);
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      triangle90capReversedArrow(state);
      state->setArrowSep(state->lineWidthDependent(0,1,0));
      break;
  }
}

void XWTikzArrow::SquareArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        if (shape == PGFSquare)
        {
          state->setArrowLength(state->lineWidthDependent(2.12132,2.828427,0));
          state->setArrowWidth(state->lengthDependent(0,1,1));
          state->setArrowLineWidth(state->lineWidthDependent(0,1,1));
        }
        else
        {
          state->setArrowLength(state->lineWidthDependent(3,4.5,0));
          state->setArrowWidth(state->lengthDependent(0,0.5,0));
        }        
        x = state->arrowLength;
        y = state->arrowWidth;
        if (x < state->arrowLineWidth)
          state->arrowLineWidth = x;
        if (state->arrowReversed)
          lineEnd = state->arrowLength - 0.5 * state->arrowLineWidth;
        else
          lineEnd = 0.5 * state->arrowLineWidth;
        tipEnd = state->arrowLength;
      }
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      state->setDash(-1);
      if (state->arrowLineJoin == PGFround)
        state->setLineJoin(PGFround);
      else
        state->setLineJoin(PGFmiter);
      if (state->lineWidth != state->arrowLineWidth)
        state->setLineWidth(state->arrowLineWidth);
      state->moveTo(state->arrowLength - 0.5 * state->arrowLineWidth, state->arrowWidth - 0.5 * state->arrowLineWidth);
      y -= 0.5 * state->arrowLineWidth;
      state->lineTo(0.5 * state->arrowLineWidth, 0.5 * state->arrowWidth - 0.5 * state->arrowLineWidth);
      if (state->arrowHarpoon)
        state->lineTo(0.5 * state->arrowLineWidth, 0);
      else
        state->lineTo(0.5 * state->arrowLineWidth, -0.5 * state->arrowWidth + 0.5 * state->arrowLineWidth);
      if (state->arrowHarpoon)
        state->lineTo(state->arrowLength - 0.5 * state->arrowLineWidth, 0);
      else
        state->lineTo(state->arrowLength - 0.5 * state->arrowLineWidth, -0.5 * state->arrowWidth + 0.5 * state->arrowLineWidth);
      state->closePath();
      if (!state->isArrowFillSet)
        state->setDraw(true);
      else
      {
        if (state->arrowLineWidth > 0)
        {
          state->setDraw(true);
          state->setFill(true);
        }
        else
          state->setFill(true);
      }
      break;
  }
}

void XWTikzArrow::squareArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        double arrowsize = 0.4 + 0.275 * state->lineWidth;
        backEnd = -7 * arrowsize - 0.5 * state->lineWidth;
        arrowsize += 0.5 * state->lineWidth;
        tipEnd = arrowsize;
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }      
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        double arrowsize = 0.4 + 0.275 * state->lineWidth;
        state->setDash(-1);
        state->setLineJoin(PGFround);
        state->moveTo(arrowsize, 4 * arrowsize);
        state->lineTo(-7 * arrowsize, 4 * arrowsize);
        state->lineTo(-7 * arrowsize, -4 * arrowsize);
        state->lineTo(arrowsize, -4 * arrowsize);
        state->closePath();
        state->setDraw(true);
        state->setFill(true);
      }
      break;
  }
}

void XWTikzArrow::squareBracketArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      backEnd = -(1 + 1.25 * state->lineWidth);
      tipEnd = 0.5 * state->lineWidth;
      visualTipEnd = tipEnd;
      visualBackEnd = backEnd;
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        double tempdima = 2 + 1.5 * state->lineWidth;
        double tempdimb = tempdima + state->lineWidth;
        state->setDash(-1);
        state->setLineJoin(PGFmiter);
        state->setLineCap(PGFbutt);
        state->moveTo(-0.5 * tempdimb, -tempdima);
        state->lineTo(0, -tempdima);
        state->lineTo(0, tempdima);
        state->lineTo(-0.5 * tempdimb, tempdima);
        state->setDraw(true);
      }
      break;
  }
}

void XWTikzArrow::StealthArrow(XWTikzState * state)
{
  double tempdima = 0;
  double tempdimb = 0;
  double xa = 0;
  double ya = 0;
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        if (shape == PGFStealth)
        {
          state->setArrowLength(state->lineWidthDependent(3,4.5,0.8));
          state->setArrowWidth(state->lengthDependent(0,0.75,0));
          state->setArrowInset(state->lengthDependent(0,0.325,0));
          state->setArrowLineWidth(state->lineWidthDependent(0,1,1));
        }
        else
        {
          state->setArrowInset(state->lengthDependent(0,0,0));
          state->angleSetup(60,2.7,3.6,0);
        }        
        x = state->arrowLength - state->arrowInset;
        x *= 0.25;
        if (x < state->arrowLineWidth)
          state->arrowLineWidth = x;
        double mathresult = state->arrowLength / state->arrowWidth;
        double quot = mathresult;
        x = 4 * mathresult * mathresult + 1;
        mathresult = sqrt(x);
        double xc = mathresult * state->arrowLineWidth * 0.5;
        xa = quot * state->arrowLineWidth;
        ya = 0.5 * state->arrowWidth;
        mathresult = atan2(state->arrowLength,ya);
        double yb = mathresult;
        mathresult = atan2(state->arrowInset,ya);
        ya = mathresult;
        yb -= ya;
        yb *= 0.5;
        mathresult = 1 / tan(yb);
        double yc = mathresult * state->arrowLineWidth * 0.5;
        ya += yb;
        double mathresulty = sin(ya);
        double mathresultx = cos(ya);
        ya = mathresulty * yc;
        yb = mathresultx * yc;
        if (state->arrowInset == 0)
          ya = 0.5 * state->arrowLineWidth;
        mathresult = state->arrowInset / state->arrowWidth;
        quot = mathresult;
        x = 4 * mathresult * mathresult + 1;
        mathresult = sqrt(x);
        yc = 0.5 * mathresult * state->arrowLineWidth;
        tempdima = state->arrowLength - xc - ya;
        tempdimb = 0.5 * state->arrowWidth - yb;
        if (state->arrowLineJoin == PGFround)
          backEnd = ya - 0.5 * state->arrowLineWidth;
        else
          backEnd = 0;
        if (state->arrowHarpoon)
          lineEnd = state->arrowInset + yc + 0.5 * state->arrowLineWidth;
        else
          lineEnd = state->arrowInset + yc - 0.25 * state->arrowLineWidth;
        if (state->arrowLineJoin == PGFround)
          tipEnd = tempdima + ya + 0.5 * state->arrowLineWidth;
        else
        {
          if (state->arrowHarpoon)
            tipEnd = state->arrowLength + xa;
          else
            tipEnd = state->arrowLength;          
        }
        visualBackEnd = state->arrowInset;
        state->arrowInset += yc;
        visualTipEnd = tipEnd;
      }
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      state->setDash(-1);
      if (state->arrowLineJoin == PGFround)
        state->setLineJoin(PGFround);
      else
        state->setLineJoin(PGFmiter);
      if (state->lineWidth != state->arrowLineWidth)
        state->setLineWidth(state->arrowLineWidth);
      state->moveTo(tempdima + ya, (double)0);
      state->lineTo(ya, tempdimb);
      state->lineTo(state->arrowInset, 0);
      if (!state->arrowHarpoon)
        state->lineTo(ya, -tempdimb);
      state->closePath();
      if (!state->isArrowFillSet)
        state->setDraw(true);
      else
      {
        if (state->arrowLineWidth > 0)
        {
          state->setDraw(true);
          state->setFill(true);
        }
        else
          state->setFill(true);
      }
      break;
  }
}

void XWTikzArrow::stealthArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        double tmpa = 0.28;
        double tmpb = state->lineWidth;
        if (state->innerLineWidth > 0)
          tmpb = 0.6 * state->lineWidth - 0.4 * state->innerLineWidth;
        tmpa = tmpa + 0.3 * tmpb;
        tipEnd = 5 * tmpa;
        backEnd = -3 * tmpa;
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        double a = 0.28;
        double b = state->lineWidth;
        if (state->innerLineWidth > 0)
          b = 0.6 * state->lineWidth - 0.4 * state->innerLineWidth;
        a = a + 0.3 * b;
        state->moveTo(5*a, (double)0);
        state->lineTo(-3 * a, 4 * a);
        state->lineTo(0, 0);
        state->lineTo(-3 * a, -4 * a);
        state->closePath();
        state->setFill(true);
      }
      break;
  }
}

void XWTikzArrow::stealthRoundedArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        double tmpa = 0.28 + 0.3 * state->lineWidth;
        double tmpb = 6 * tmpa + 0.5 * state->lineWidth;
        backEnd = -tmpb;
        tmpb = 2 * tmpa + 0.5 * state->lineWidth;
        tipEnd = tmpb;        
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        double a = 0.28 + 0.3 * state->lineWidth;
        state->setDash(-1);
        state->setLineJoin(PGFround);
        state->moveTo(2*a, (double)0);
        state->curveTo(-0.5*a, 0.5*a,-3*a, 1.5*a,-6*a,3.25*a);
        state->curveTo(-3*a, a,-3*a, -1*a,-6*a,-3.25*a);
        state->curveTo(-3*a, -1.5*a,-0.5*a, -0.5*a,2*a,0);
        state->closePath();
        state->setFill(true);
        state->setDraw(true);
      }
      break;
  }
}

void XWTikzArrow::StraightBarbArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      state->setArrowLength(state->lineWidthDependent(1.5,2,0));
      state->setArrowWidth(state->lengthDependent(0,2,0));
      state->setArrowLineWidth(state->lineWidthDependent(0,1,1));
      {
        double mathresult = state->arrowLength / state->arrowWidth;
        double quot = mathresult;
        x = 4 * mathresult * mathresult + 1;
        mathresult = sqrt(x);
        double xc = 0.5 * mathresult * state->arrowLineWidth;
        double xa = quot * state->arrowLineWidth;
        if (state->arrowLineJoin == PGFround)
          tipEnd = state->arrowLength + 0.5 * state->arrowLineWidth;
        else
        {
          tipEnd = xc;
          if (state->arrowHarpoon)
            tipEnd += xa;
        }
        visualBackEnd = state->arrowLength + 0.5 * state->arrowLineWidth;
        backEnd = -0.5 * state->arrowLineWidth;
        if (state->arrowReversed)
        {
          lineEnd = state->arrowLength;
          if (state->arrowHarpoon)
            lineEnd += 0.5 * state->arrowLineWidth;
        }
        else
          lineEnd = state->arrowLength + 0.5 * state->arrowLineWidth;
        visualTipEnd = tipEnd;
      }
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      state->setDash(-1);
      if (state->arrowLineJoin == PGFround)
        state->setLineJoin(PGFround);
      else
        state->setLineJoin(PGFmiter);
      if (state->arrowLineCap == PGFround)
        state->setLineCap(PGFround);
      else
        state->setLineCap(PGFbutt);
      if (state->lineWidth != state->arrowLineWidth)
        state->setLineWidth(state->arrowLineWidth);
      state->moveTo((double)0, 0.5 * state->arrowWidth);
      state->lineTo(state->arrowLength, 0);
      if (state->arrowHarpoon)
      {
        if (state->arrowReversed)
          state->lineTo(state->arrowLength + state->arrowLineWidth, 0);
        else
          state->lineTo(state->arrowLength - state->arrowLineWidth, 0);
      }
      else
        state->lineTo(0, -0.5 * state->arrowWidth);
      state->setDraw(true);
      break;
  }
}

void XWTikzArrow::TeeBarbArrow(XWTikzState * state)
{
  double tempdima = 0;
  double tempdimb = 0;
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      if (shape == PGFTeeBarb)
      {
        state->setArrowLength(state->lineWidthDependent(1.5,2,0));
        state->setArrowWidth(state->lineWidthDependent(3,4,0));
        state->setArrowInset(state->lineWidthDependent(0,0.5,0));
        state->setArrowLineWidth(state->lineWidthDependent(0,1,1));
      }
      else if (shape == PGFBar)
        state->setArrowLength(state->lineWidthDependent(0,0,0));
      else
      {
        state->setArrowInset(state->lineWidthDependent(0,1,0));
        state->setArrowLength(state->lineWidthDependent(0.75,1,0));
      }
      {
        x = state->arrowLineWidth;
        bool tempswa = false;
        tempdima = state->arrowLength - state->arrowInset;
        if (tempdima < 0.5 * state->arrowLineWidth)
        {
          tempdima = 0.5 * state->arrowLineWidth;
          tempswa = true;
        }          
        tempdimb = -state->arrowInset;
        if (tempdimb > 0.5 * state->arrowLineWidth)
        {
          tempdimb -= 0.5 * state->arrowLineWidth;
          tempswa = true;
        }
        tipEnd = tempdima;
        if (state->arrowLineCap == PGFround)
        {
          if (!tempswa)
            tempdima += 0.5 * state->arrowLineWidth;
        }
        visualBackEnd = 0.5 * state->arrowLineWidth;
        backEnd = tempdimb;
        if (state->arrowLineCap == PGFround)
        {
          if (!tempswa)
            backEnd -= 0.5 * state->arrowLineWidth;
        }
        if (state->arrowReversed)
          lineEnd = 0.25 * state->arrowLineWidth;
        else
          lineEnd = -0.25 * state->arrowLineWidth;
        visualTipEnd = tipEnd;
      }
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        state->setDash(-1);
        double yc = state->lineWidth;
        if (state->arrowLineJoin == PGFround)
          state->setLineJoin(PGFround);
        else
          state->setLineJoin(PGFmiter);
        if (state->arrowLineCap == PGFround)
          state->setLineCap(PGFround);
        else
          state->setLineCap(PGFbutt);
        if (state->lineWidth != state->arrowLineWidth)
          state->setLineWidth(state->arrowLineWidth);
        x = tempdima - 0.5 * state->arrowLineWidth;
        y = tempdimb + 0.5 * state->arrowLineWidth;
        if (x == y)
        {
          state->moveTo((double)0, 0.5 * state->arrowWidth);
          if (state->arrowHarpoon)
            state->lineTo(0, -0.5 * state->arrowLineWidth);
          else
            state->lineTo(0, -0.5 * state->arrowWidth);
        }
        else
        {
          if (tempdima == 0.5 * state->arrowLineWidth)
          {
            state->moveTo(tempdimb, 0.5 * state->arrowWidth - 0.5 * state->arrowLineWidth);
            state->lineTo(0, 0.5 * state->arrowWidth - 0.5 * state->arrowLineWidth);
            if (state->arrowHarpoon)
              state->lineTo(0, -0.5 * yc);
            else
            {
              state->lineTo(0, -0.5 * state->arrowWidth + 0.5 * state->arrowLineWidth);
              state->lineTo(tempdimb, -0.5 * state->arrowWidth + 0.5 * state->arrowLineWidth);
            }
          }
          else
          {
             state->moveTo(tempdimb, 0.5 * state->arrowWidth - 0.5 * state->arrowLineWidth);
             state->lineTo(tempdima, 0.5 * state->arrowWidth - 0.5 * state->arrowLineWidth);
             state->moveTo(0, 0.5 * state->arrowWidth - 0.5 * state->arrowLineWidth);
             if (state->arrowHarpoon)
              state->lineTo(0, -0.5 * yc);
            else
            {
              state->lineTo(0, -0.5 * state->arrowWidth + 0.5 * state->arrowLineWidth);
              state->moveTo(tempdimb, -0.5 * state->arrowWidth + 0.5 * state->arrowLineWidth);
              state->lineTo(tempdima, -0.5 * state->arrowWidth + 0.5 * state->arrowLineWidth);
            }
          }
        }
        state->setDraw(true);
      }
      break;
  }
}

void XWTikzArrow::toArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      backEnd = 0.21 + 0.625 * state->lineWidth;
      tipEnd = -0.84 - 1.3 * state->lineWidth;
      visualTipEnd = tipEnd;
      visualBackEnd = backEnd;
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        double a = 0.28 + 0.3 * state->lineWidth;
        state->lineWidth = 0.8 * state->lineWidth;
        state->moveTo(-3 * a, 4 * a);
        state->curveTo(-2.75*a, 2.5*a, 0, 0.25*a, 0.75*a, 0);
        state->curveTo(0, -0.25*a, -2.75*a, -2.5*a, -3*a, -4*a);
        state->setDraw(true);
      }
      break;
  }
}

void XWTikzArrow::toReversedArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      tipEnd = 0.98 + 1.45 * state->lineWidth;
      backEnd = -0.21 - 0.475 * state->lineWidth;
      visualTipEnd = tipEnd;
      visualBackEnd = backEnd;
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        double a = 0.28 + 0.3 * state->lineWidth;
        state->lineWidth = 0.8 * state->lineWidth;
        state->moveTo(3.5 * a, 4 * a);
        state->curveTo(3.25*a, 2.5*a, 0.5*a, 0.25*a, -0.25*a, 0);
        state->curveTo(0.5*a, -0.25*a, 3.25*a, -2.5*a, 3.5*a, -4*a);
        state->setDraw(true);
      }
      break;
  }
}

void XWTikzArrow::TriangleCapArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      state->setArrowLength(state->lineWidthDependent(0,0.5,0));
      if (state->arrowReversed)
      {
        tipEnd = state->arrowLength;
        lineEnd = state->arrowLength + 0.5*state->lineWidth;
      }
      else
      {
        tipEnd = state->arrowLength;
        lineEnd = -0.5 * state->lineWidth;
      }
      visualTipEnd = tipEnd;
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        if (state->arrowReversed)
        {
          state->scale(-1,1);
          state->addShift(-state->arrowLength, 0);
        }
        double tempdima = 0.5*state->lineWidth;
        state->moveTo(-0.75*state->lineWidth,tempdima);
        state->lineTo(0,tempdima);
        if (state->arrowReversed)
        {
          state->lineTo(state->arrowLength,tempdima);
          state->addShift(state->arrowLength, 0);
          state->scale(-1,1);
        }
        state->lineTo(state->arrowLength,0);
        if (state->arrowReversed)
          state->lineTo(state->arrowLength,-tempdima);
        state->lineTo(0,-tempdima);
        state->lineTo(-0.75*state->lineWidth,-tempdima);
        state->closePath();
        state->setFill(true);
      }
      break;
  }
}

void XWTikzArrow::triangle45Arrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        double tmpa = 0.5 + 0.25 * state->lineWidth;
        double tmpb = 8.705 * tmpa + 0.5 * state->lineWidth;
        backEnd = -tmpb;
        tmpb = 0.5 * tmpa + 1.28 * state->lineWidth;
        tipEnd = tmpb;
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }      
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        double a = 0.5 + 0.25 * state->lineWidth;
        state->setDash(-1);
        state->setLineJoin(PGFmiter);
        state->moveTo(0.5 * a + 10 * a * cos((double)157), 10 * a * sin((double)157));
        state->lineTo(0.5*a, 0);
        state->lineTo(0.5 * a + 10 * a * cos((double)-157), 10 * a * sin((double)-157));
        state->closePath();
        state->setDraw(true);
        state->setFill(true);
      }
      break;
  }
}

void XWTikzArrow::triangle60Arrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        double tmpa = 0.5 + 0.25 * state->lineWidth;
        double tmpb = 7.29 * tmpa + 0.5 * state->lineWidth;
        backEnd = -tmpb;
        tmpb = 0.5 * tmpa + state->lineWidth;
        tipEnd = tmpb;
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }      
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        double a = 0.5 + 0.25 * state->lineWidth;
        state->setDash(-1);
        state->setLineJoin(PGFmiter);
        state->moveTo(0.5 * a + 9 * a * cos((double)150), 9 * a * sin((double)150));
        state->lineTo(0.5*a, 0);
        state->lineTo(0.5 * a + 9 * a * cos((double)-150), 9 * a * sin((double)-150));
        state->closePath();
        state->setDraw(true);
        state->setFill(true);
      }
      break;
  }
}

void XWTikzArrow::triangle90Arrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        double tmpa = 0.5 + 0.25 * state->lineWidth;
        double tmpb = 5.5 * tmpa + 0.5 * state->lineWidth;
        backEnd = -tmpb;
        tmpb = 0.5 * tmpa + 0.707 * state->lineWidth;
        tipEnd = tmpb;
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }      
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        double a = 0.5 + 0.25 * state->lineWidth;
        state->setDash(-1);
        state->setLineJoin(PGFmiter);
        state->moveTo(-5.5 * a, -6 * a);
        state->lineTo(0.5*a, 0);
        state->lineTo(-5.5*a, 6 * a);
        state->closePath();
        state->setDraw(true);
        state->setFill(true);
      }
      break;
  }
}

void XWTikzArrow::triangle90capArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        backEnd = -1 * state->lineWidth;
        tipEnd = state->lineWidth;
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }      
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        state->moveTo(-1 * state->lineWidth,0.5*state->lineWidth);
        state->lineTo(0.5 * state->lineWidth,0.5 * state->lineWidth);
        state->lineTo(state->lineWidth,0);
        state->lineTo(0.5 * state->lineWidth,-0.5 * state->lineWidth);
        state->lineTo(-1 * state->lineWidth,-0.5 * state->lineWidth);
        state->closePath();
        state->setFill(true);
      }
      break;
  }
}

void XWTikzArrow::triangle90capReversedArrow(XWTikzState * state)
{
  switch (code)
  {
    default:
      break;

    case XW_TIKZ_ARROW_CODE_SETUP:
      {
        backEnd = -1 * state->lineWidth;
        tipEnd = state->lineWidth;
        visualTipEnd = tipEnd;
        visualBackEnd = backEnd;
      }      
      break;

    case XW_TIKZ_ARROW_CODE_DRAW:
      {
        state->moveTo(state->lineWidth,0.5*state->lineWidth);
        state->lineTo(-1 * state->lineWidth,0.5 * state->lineWidth);
        state->lineTo(-1 * state->lineWidth,-0.5 * state->lineWidth);
        state->lineTo(state->lineWidth,-0.5 * state->lineWidth);
        state->lineTo(0.5 * state->lineWidth,0);
        state->closePath();
        state->setFill(true);
      }
      break;
  }
}
