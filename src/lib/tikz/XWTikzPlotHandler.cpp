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
  switch (ptype)
  {
    default:
      doDefaultHandler(stateA,pointsA,iscontinue);
      break;
  }
}

void XWTikzPlotHandler::doDefaultHandler(XWTikzState * stateA,
                                         const QList<QPointF> & pointsA, 
                                         bool iscontinue)
{
  if (pointsA.size() == 0)
    return ;

  if (iscontinue)
    stateA->lineTo(pointsA[0]);
  else
    stateA->moveTo(pointsA[0]);

  for (int i = 1; i < pointsA.size(); i++)
    stateA->lineTo(pointsA[i]);
}
