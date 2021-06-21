/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTIKZDECORATION_H
#define XWTIKZDECORATION_H

class XWTikzState;

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

  void   decorateAutoCorner(XWTikzState * stateA);
  void   decorateAutoEnd(XWTikzState * stateA,void (XWTikzDecoration::*final)(XWTikzState *));

  void nullState(XWTikzState * stateA);

private:
  int dtype,cur;

  int sequenceNumber;
  double distanceFromStart,computedWidth;
  double position,markStart,markEnd,step;

  void (XWTikzDecoration::*current_state)(XWTikzState * stateA);
  void (XWTikzDecoration::*final_state)(XWTikzState * stateA);
};

#endif //XWTIKZDECORATION_H
