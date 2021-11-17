/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTIKZDECORATION_H
#define XWTIKZDECORATION_H

class XWTikzState;
class XWTikzTextBox;
class XWTikzShape;

class XWTikzDecoration
{
public:
  XWTikzDecoration(int dtypeA);

  void doCurrentState(XWTikzState * stateA);
  void doFinalState(XWTikzState * stateA);
  void doInitState(XWTikzState * stateA);
  
  bool isNullState();

private:
  void circleConnectionBarInitState(XWTikzState * stateA);
  void circleConnectionBarBarState(XWTikzState * stateA);
  void circleConnectionBarEndState(XWTikzState * stateA);

  void doBentBentState(XWTikzState * stateA);
  void doBentFinalState(XWTikzState * stateA);

  void doBumpsInitState(XWTikzState * stateA);
  void doBumpsFinalState(XWTikzState * stateA);

  void doCoilCoilState(XWTikzState * stateA);
  void doCoilLastState(XWTikzState * stateA);
  void doCoilFinalState(XWTikzState * stateA);

  void doLineToInitState(XWTikzState * stateA);
  void doLineToInputSegmentState(XWTikzState * stateA);
  void doLineToFinalState(XWTikzState * stateA);

  void doMoveToInitState(XWTikzState * stateA);
  void doMoveToFinalState(XWTikzState * stateA);

  void doCurveToInitState(XWTikzState * stateA);
  void doCurveToFinalState(XWTikzState * stateA);

  void doMarkingsInitState(XWTikzState * stateA);
  void doMarkingsPreState(XWTikzState * stateA);
  void doMarkingsSkipperState(XWTikzState * stateA);
  void doMarkingsMainState(XWTikzState * stateA);
  void doMarkingsFinalState(XWTikzState * stateA);

  void doRandomStepsStartState(XWTikzState * stateA);
  void doRandomStepsStepState(XWTikzState * stateA);
  void doRandomStepsFinalState(XWTikzState * stateA);

  void doSawInitState(XWTikzState * stateA);
  void doSawFinalState(XWTikzState * stateA);

  void doSnakeInitState(XWTikzState * stateA);
  void doSnakeDown(XWTikzState * stateA);
  void doSnakeUp(XWTikzState * stateA);
  void doSnakeEndDown(XWTikzState * stateA);
  void doSnakeEndUp(XWTikzState * stateA);
  void doSnakeFinalState(XWTikzState * stateA);

  void doStraightZigzagLineToState(XWTikzState * stateA);
  void doStraightZigzagZigzagState(XWTikzState * stateA);
  void doStraightZigzagFinalState(XWTikzState * stateA);

  void doZigzagUpFromCenterState(XWTikzState * stateA);
  void doZigzagBigDownState(XWTikzState * stateA);
  void doZigzagBigUpState(XWTikzState * stateA);
  void doZigzagCenterFinishState(XWTikzState * stateA);
  void doZigzagFinalState(XWTikzState * stateA);

  void doFootPrintsLeftState(XWTikzState * stateA);
  void doFootPrintsRightState(XWTikzState * stateA);
  void doFootPrintsBird(XWTikzState * stateA);
  void doFootPrintsFelisSilvestris(XWTikzState * stateA);
  void doFootPrintsGnome(XWTikzState * stateA);
  void doFootPrintsHuman(XWTikzState * stateA);

  void doKochCurveType1InitState(XWTikzState * stateA);
  void doKochCurveType2InitState(XWTikzState * stateA);
  void doKochSnowFlakeInitState(XWTikzState * stateA);
  void doCantorSetInitState(XWTikzState * stateA);

  void doTicksTicksState(XWTikzState * stateA);
  void doTicksFinalState(XWTikzState * stateA);
  void doExpandingWavesInitState(XWTikzState * stateA);
  void doExpandingWavesWaveState(XWTikzState * stateA);
  void doExpandingWavesLastState(XWTikzState * stateA);
  void doExpandingWavesFinalState(XWTikzState * stateA);
  void doWavesInitState(XWTikzState * stateA);
  void doWavesFinalState(XWTikzState * stateA);
  void doBorderTickState(XWTikzState * stateA);
  void doBorderLastState(XWTikzState * stateA);
  void doBorderFinalState(XWTikzState * stateA);
  void doBraceBraceState(XWTikzState * stateA);
  void doBraceFinalState(XWTikzState * stateA);

  void doTextAlongPathInitState(XWTikzState * stateA);
  void doTextAlongPathLeftIndentState(XWTikzState * stateA);
  void doTextAlongPathScanState(XWTikzState * stateA);
  void doTextAlongPathBeforeTypesetState(XWTikzState * stateA);
  void doTextAlongPathTypesetState(XWTikzState * stateA);
  void doTextAlongPathAfterTypesetState(XWTikzState * stateA);
  void doTextAlongPathShitState(XWTikzState * stateA);
  void doTextAlongPathFinalState(XWTikzState * stateA);

  void doTrianglesInitState(XWTikzState * stateA);
  void doTrianglesTriangleState(XWTikzState * stateA);
  void doTrianglesLastState(XWTikzState * stateA);
  void doTrianglesSkipState(XWTikzState * stateA);
  void doTrianglesFinalState(XWTikzState * stateA);

  void doCrossesInitState(XWTikzState * stateA);
  void doCrossesCrossesState(XWTikzState * stateA);
  void doCrossesLastState(XWTikzState * stateA);
  void doCrossesFinalState(XWTikzState * stateA);

  void doShapeBackgroundsInitState(XWTikzState * stateA);
  void doShapeBackgroundsBeforeShapeState(XWTikzState * stateA);
  void doShapeBackgroundsShapeState(XWTikzState * stateA);
  void doShapeBackgroundsAfterShapeState(XWTikzState * stateA);
  void doShapeBackgroundsSepState(XWTikzState * stateA);
  void doShapeBackgroundsFinalState(XWTikzState * stateA);
  
  void   decorateAutoCorner(XWTikzState * stateA);
  void   decorateAutoEnd(XWTikzState * stateA,void (XWTikzDecoration::*final)(XWTikzState *));

  void nullState(XWTikzState * stateA);

private:
  int dtype,cur;

  int sequenceNumber;
  double distanceFromStart,computedWidth;
  double position,markStart,markEnd,step;

  double ssw, ssh, beforeShape, afterShape, width, height,initialise, specialWidth;
  double shapeSep, widthChange, heightChange;
  bool betweenBorders;

  double spaceShift, characterShift, indentLeft, textShift;

  XWTikzTextBox * box;
  XWTikzShape * node;

  void (XWTikzDecoration::*current_state)(XWTikzState * stateA);
  void (XWTikzDecoration::*final_state)(XWTikzState * stateA);
};

#endif //XWTIKZDECORATION_H
