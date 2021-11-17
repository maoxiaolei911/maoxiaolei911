/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "PGFKeyWord.h"
#include "XWTikzState.h"
#include "XWTikzPlotHandler.h"

XWTikzPlotHandler::XWTikzPlotHandler(int ptypeA)
:ptype(ptypeA)
{}


void XWTikzPlotHandler::doHandler(XWTikzState * stateA,
                                  const QList<QPointF> & pointsA, 
                                  bool iscontinue)
{
  if (pointsA.size() < 2)
    return ;

  if (iscontinue)
    stateA->lineTo(pointsA[0]);
  else
    stateA->moveTo(pointsA[0]);

  switch (ptype)
  {
    default:
      doHandlerLineTo(stateA,pointsA);
      break;

    case PGFsharpcycle:
      doHandlerPolygon(stateA,pointsA);
      break;

    case PGFonlymarks:
      doHandlerDiscard(stateA,pointsA);
      break;

    case PGFsmooth:
      doHandlerCurveTo(stateA,pointsA);
      break;

    case PGFsmoothcycle:
      doHandlerClosedCurve(stateA,pointsA);
      break;

    case PGFxcomb:
      doHandlerXComb(stateA,pointsA);
      break;

    case PGFycomb:
      doHandlerYComb(stateA,pointsA);
      break;

    case PGFybar:
      doHandlerYBar(stateA,pointsA);
      break;

    case PGFxbar:
      doHandlerXBar(stateA,pointsA);
      break;

    case PGFybarinterval:
      doHandlerYBarInterval(stateA,pointsA);
      break;

    case PGFxbarinterval:
      doHandlerXBarInterval(stateA,pointsA);
      break;

    case PGFconstplot:
    case PGFconstplotmarkleft:
      doHandlerConstantLineTo(stateA,pointsA);
      break;

    case PGFconstplotmarkright:
      doHandlerConstantLineToMarkRight(stateA,pointsA);
      break;

    case PGFconstplotmarkmid:
      doHandlerConstantLineToMarkMid(stateA,pointsA);
      break;

    case PGFjumpmarkright:
      doHandlerJumpMarkRight(stateA,pointsA);
      break;

    case PGFjumpmarkleft:
      doHandlerJumpMarkLeft(stateA,pointsA);
      break;

    case PGFjumpmarkmid:
      doHandlerJumpMarkMid(stateA,pointsA);
      break;

    case PGFpolarcomb:
      doHandlerPolarComb(stateA,pointsA);
      break;
  }
}

void XWTikzPlotHandler::doHandlerClosedCurve(XWTikzState * stateA,
                                             const QList<QPointF> & pointsA)
{
  if (pointsA.size() < 4)
    return ;

  QPointF initial,afterinitial,first,second,support,presupport;
  for (int i = 0; i < pointsA.size() - 4; i += 4)
  {
    initial = pointsA[i];
    afterinitial = pointsA[i+1];
    QPointF a = stateA->tension * (pointsA[i+2] - initial);
    QPointF b = afterinitial - a;
    QPointF c = afterinitial + a;
    first = afterinitial;
    second = pointsA[i+2];
    presupport = b;
    support = c;
    a = stateA->tension * (pointsA[i+3] - first);
    b = second - a;
    c = second + a;
    stateA->curveTo(support, b, second);
    first = second;
    second = pointsA[i+3];
    support = c;
  }

  QPointF a = stateA->tension * (initial - first);
  QPointF b = second - a;
  QPointF c = second + a;
  stateA->curveTo(support, b, second);
  support = c;
  a = stateA->tension * (afterinitial - second);
  b = initial - a;
  c = initial + a;
  stateA->curveTo(support, b, initial);
  stateA->curveTo(c, presupport, afterinitial);
  stateA->closePath();
}

void XWTikzPlotHandler::doHandlerConstantLineTo(XWTikzState * stateA,
                                                const QList<QPointF> & pointsA)
{
  for (int i = 0; i < pointsA.size() - 1; i++)
  {
    stateA->lineTo(pointsA[i+1].x(),pointsA[i].y());
    stateA->lineTo(pointsA[i+1]);
  }
}

void XWTikzPlotHandler::doHandlerConstantLineToMarkMid(XWTikzState * stateA,
                                                       const QList<QPointF> & pointsA)
{
  for (int i = 0; i < pointsA.size() - 1; i++)
  {
    stateA->lineTo(0.5 * pointsA[i+1].x(),pointsA[i].y());
    stateA->lineTo(0.5 * pointsA[i+1].x(),pointsA[i+1].y());
    stateA->lineTo(pointsA[i+1]);
  }
}

void XWTikzPlotHandler::doHandlerConstantLineToMarkRight(XWTikzState * stateA,
                                                         const QList<QPointF> & pointsA)
{
  for (int i = 0; i < pointsA.size() - 1; i++)
  {
    stateA->lineTo(pointsA[i].x(),pointsA[i+1].y());
    stateA->lineTo(pointsA[i+1]);
  }
}

void XWTikzPlotHandler::doHandlerCurveTo(XWTikzState * stateA,
                                         const QList<QPointF> & pointsA)
{
  QPointF first,support,second;
  for (int i = 0; i < pointsA.size() - 2; i+=2)
  {
    first = pointsA[i];
    support = pointsA[i];
    second = pointsA[i+1];
    QPointF a = stateA->tension * (pointsA[i+1] - first);
    QPointF b = second - a;
    QPointF c = second + a;
    stateA->curveTo(support, b, second);
    first = second;
    second = pointsA[i];
    support = c;
  }

  stateA->curveTo(support, second, second);
}

void XWTikzPlotHandler::doHandlerDiscard(XWTikzState * ,
                                         const QList<QPointF> &)
{}

void XWTikzPlotHandler::doHandlerJumpMarkLeft(XWTikzState * stateA,
                                              const QList<QPointF> & pointsA)
{
  for (int i = 0; i < pointsA.size() - 1; i++)
  {
    stateA->moveTo(pointsA[i+1].x(), pointsA[i].y());
    stateA->lineTo(pointsA[i+1]);
  }
}

void XWTikzPlotHandler::doHandlerJumpMarkMid(XWTikzState * stateA,
                                             const QList<QPointF> & pointsA)
{
  for (int i = 0; i < pointsA.size() - 1; i++)
  {
    stateA->moveTo(pointsA[i].x(), pointsA[i+1].y());
    stateA->lineTo(pointsA[i+1]);
  }
}

void XWTikzPlotHandler::doHandlerJumpMarkRight(XWTikzState * stateA,
                                               const QList<QPointF> & pointsA)
{
  for (int i = 0; i < pointsA.size() - 1; i++)
  {
    double xc = 0.5 * (pointsA[i].x(), pointsA[i+1].x());
    stateA->lineTo(xc,pointsA[i].y());
    stateA->moveTo(xc, pointsA[i+1].y());
    stateA->lineTo(pointsA[i+1]);
  }
}

void XWTikzPlotHandler::doHandlerLineTo(XWTikzState * stateA,
                                         const QList<QPointF> & pointsA)
{
  for (int i = 1; i < pointsA.size(); i++)
    stateA->lineTo(pointsA[i]);
}

void XWTikzPlotHandler::doHandlerPolarComb(XWTikzState * stateA,
                                           const QList<QPointF> & pointsA)
{
  for (int i = 0; i < pointsA.size(); i++)
  {
    stateA->moveTo(0,0);
    stateA->lineTo(pointsA[i]);
  }
}

void XWTikzPlotHandler::doHandlerPolygon(XWTikzState * stateA,
                                         const QList<QPointF> & pointsA)
{
  for (int i = 1; i < pointsA.size(); i++)
    stateA->lineTo(pointsA[i]);

  stateA->closePath();
}

void XWTikzPlotHandler::doHandlerXBar(XWTikzState * stateA,
                                      const QList<QPointF> & pointsA)
{
  for (int i = 0; i < pointsA.size() - 1; i++)
  {
    double x = pointsA[i+1].x();
    double y = pointsA[i].y() - 0.5 * stateA->barWidth + stateA->barShift;
    QPointF ll(x,y);
    stateA->rectangle(ll, pointsA[i].x() - x,stateA->barWidth);
  }
}

void XWTikzPlotHandler::doHandlerXBarInterval(XWTikzState * stateA,
                                              const QList<QPointF> & pointsA)
{
  for (int i = 0; i < pointsA.size() - 1; i++)
  {
    double h = stateA->barIntervalWidth * (pointsA[i+1].y() - pointsA[i].y());
    double x = pointsA[i+1].x();
    double y = pointsA[i].y() + stateA->barIntervalShift * (pointsA[i+1].y() - pointsA[i].y()) - 0.5 * h;
    QPointF ll(x,y);
    stateA->rectangle(ll,pointsA[i].x() - pointsA[i+1].x(), h);
  }
}

void XWTikzPlotHandler::doHandlerXComb(XWTikzState * stateA,
                                       const QList<QPointF> & pointsA)
{
  
  for (int i = 0; i < pointsA.size() - 1; i++)
  {
    stateA->moveTo(pointsA[i+1].x(), pointsA[i].y());
    stateA->lineTo(pointsA[i]);
  }
}

void XWTikzPlotHandler::doHandlerYBar(XWTikzState * stateA,
                                      const QList<QPointF> & pointsA)
{
  for (int i = 0; i < pointsA.size() - 1; i++)
  {
    double x = pointsA[i].x() - 0.5 * stateA->barWidth + stateA->barShift;
    double y = pointsA[i+1].y();
    QPointF ll(x,y);
    stateA->rectangle(ll,stateA->barWidth,pointsA[i].y() - y);
  }
}

void XWTikzPlotHandler::doHandlerYBarInterval(XWTikzState * stateA,
                                              const QList<QPointF> & pointsA)
{
  for (int i = 0; i < pointsA.size() - 1; i++)
  {
    double w = stateA->barIntervalWidth * (pointsA[i+1].x() - pointsA[i].x());
    double x = pointsA[i].x() + stateA->barIntervalShift * (pointsA[i+1].x() - pointsA[i].x()) - 0.5 * w;
    double y = pointsA[i+1].y();
    QPointF ll(x,y);
    stateA->rectangle(ll,w,pointsA[i].y() - pointsA[i+1].y());
  }
}

void XWTikzPlotHandler::doHandlerYComb(XWTikzState * stateA,
                      const QList<QPointF> & pointsA)
{
  for (int i = 0; i < pointsA.size() - 1; i++)
  {
    stateA->moveTo(pointsA[i].x(), pointsA[i+1].y());
    stateA->lineTo(pointsA[i]);
  }
}
