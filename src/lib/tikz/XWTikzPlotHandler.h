/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTIKZPLOTHANDLER_H
#define XWTIKZPLOTHANDLER_H

#include <QList>
#include <QPointF>

class XWTikzState;

class XWTikzPlotHandler
{
public:
  XWTikzPlotHandler(int ptypeA);

  void doHandler(XWTikzState * stateA,
                 const QList<QPointF> & pointsA, 
                 bool iscontinue);

private:
  void doHandlerClosedCurve(XWTikzState * stateA,
                      const QList<QPointF> & pointsA);
  void doHandlerConstantLineTo(XWTikzState * stateA,
                      const QList<QPointF> & pointsA);
  void doHandlerConstantLineToMarkMid(XWTikzState * stateA,
                      const QList<QPointF> & pointsA);
  void doHandlerConstantLineToMarkRight(XWTikzState * stateA,
                      const QList<QPointF> & pointsA);
  void doHandlerCurveTo(XWTikzState * stateA,
                      const QList<QPointF> & pointsA);
  void doHandlerDiscard(XWTikzState * stateA,
                      const QList<QPointF> & pointsA);
  void doHandlerJumpMarkLeft(XWTikzState * stateA,
                      const QList<QPointF> & pointsA);
  void doHandlerJumpMarkMid(XWTikzState * stateA,
                      const QList<QPointF> & pointsA);
  void doHandlerJumpMarkRight(XWTikzState * stateA,
                      const QList<QPointF> & pointsA);
  void doHandlerLineTo(XWTikzState * stateA,
                      const QList<QPointF> & pointsA);
  void doHandlerPolarComb(XWTikzState * stateA,
                      const QList<QPointF> & pointsA);
  void doHandlerPolygon(XWTikzState * stateA,
                      const QList<QPointF> & pointsA);
  void doHandlerXBar(XWTikzState * stateA,
                      const QList<QPointF> & pointsA);
  void doHandlerXBarInterval(XWTikzState * stateA,
                      const QList<QPointF> & pointsA);
  void doHandlerXComb(XWTikzState * stateA,
                      const QList<QPointF> & pointsA);
  void doHandlerYBar(XWTikzState * stateA,
                      const QList<QPointF> & pointsA);
  void doHandlerYBarInterval(XWTikzState * stateA,
                      const QList<QPointF> & pointsA);
  void doHandlerYComb(XWTikzState * stateA,
                      const QList<QPointF> & pointsA);

private:
  int ptype;
};

#endif //XWTIKZPLOTHANDLER_H
