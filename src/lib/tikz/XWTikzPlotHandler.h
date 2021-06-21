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
  void doDefaultHandler(XWTikzState * stateA,
                      const QList<QPointF> & pointsA, 
                      bool iscontinue);

private:
  int ptype;
};

#endif //XWTIKZPLOTHANDLER_H
