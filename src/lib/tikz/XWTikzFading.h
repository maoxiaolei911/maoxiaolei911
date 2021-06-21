/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTIKZFADING_H
#define XWTIKZFADING_H

#include <QTransform>
#include <QPointF>
#include <QString>

class XWPDFDriver;
class XWTikzState;
class XWObject;

class XWTikzFading
{
public:
  XWTikzFading(XWPDFDriver * driverA,int ftypeA,const QTransform & transformA);

  void doFading(XWTikzState * state);

private:
  void circleWithFuzzyEdge10Percent(XWTikzState * state);
  void circleWithFuzzyEdge15Percent(XWTikzState * state);
  void circleWithFuzzyEdge20Percent(XWTikzState * state);
  void doEast(XWTikzState * state);
  void doFuzzyRing15Percent(XWTikzState * state);
  void doNorth(XWTikzState * state);
  void doSouth(XWTikzState * state);
  void doWest(XWTikzState * state);

  void useFading(const QString & fname,XWTikzState * state,XWObject * stm);

private:
  XWPDFDriver * driver;
  int ftype;
  QTransform transform;
};

#endif //XWTIKZFADING_H
