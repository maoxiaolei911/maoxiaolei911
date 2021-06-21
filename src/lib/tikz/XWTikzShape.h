/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTIKZSHAPE_H
#define XWTIKZSHAPE_H

#include <QPointF>
#include <QTransform>

class XWTeXBox;
class XWPDFDriver;
class XWTikzState;

class XWTikzShape
{
public:
  XWTikzShape(XWPDFDriver*driverA,
              XWTeXBox * boxA,
              XWTikzState * stateA,
              int nt);
  ~XWTikzShape();

  static double angleOnEllipse(const QPointF & p, const QPointF & ur);

  void doShape(bool dopath = true);

  static QPointF pointBorderEllipse(const QPointF & pd, const QPointF & ur);
  static QPointF pointBorderRectangle(const QPointF & pd, const QPointF & ur);


  void setFontSize(int s);

private:  
  void arrowBoxShape(bool dopath = true);
  void chamferedRectangleShape(bool dopath = true);
  void circleShape(bool dopath = true);
  void circleSolidusShape(bool dopath = true);
  void circleSplitShape(bool dopath = true);
  void circularSectorShape(bool dopath = true);
  void cloudCalloutShape(bool dopath = true);
  void cloudShape(bool dopath = true);
  void coordinateShape(bool dopath = true);
  void correctForbiddenSignShape(bool dopath = true);
  void crossoutShape(bool dopath = true);
  void cylinderShape(bool dopath = true);
  void dartShape(bool dopath = true);
  void diamondShape(bool dopath = true);
  void doubleArrowShape(bool dopath = true);
  void ellipseCalloutShape(bool dopath = true);
  void ellipseShape(bool dopath = true);
  void ellipseSplitShape(bool dopath = true);
  void forbiddenSignShape(bool dopath = true);
  void isosceleTriangleShape(bool dopath = true);
  void kiteShape(bool dopath = true);
  void magnifyingGlassShape(bool dopath = true);
  void magneticTapeShape(bool dopath = true);
  void rectangleCalloutShape(bool dopath = true);
  void rectangleShape(bool dopath = true);
  void rectangleSplitShape(bool dopath = true);
  void regularPolygonShape(bool dopath = true);
  void roundedRectangleShape(bool dopath = true);
  void semicircleShape(bool dopath = true);
  void signalShape(bool dopath = true);
  void singleArrowShape(bool dopath = true);
  void starShape(bool dopath = true);
  void starburstShape(bool dopath = true);
  void strikeoutShape(bool dopath = true);
  void tapeShape(bool dopath = true);
  void trapeziumShape(bool dopath = true);

  QPointF pointIntersectionOfLineAndArc(const QPointF & p1,
                                        const QPointF & p2,
                                        const QPointF & c,
                                        double sa,double ea,
                                        double xr,double yr);

  void circleAnchor();

  void rectangleAnchor();

public:
  XWPDFDriver * driver;
  XWTeXBox * box;
  XWTikzState * state;
  int nodeType;

  double boxWidth,boxHeight,boxDepth,radius;

  QPointF anchorPos,centerPos,westPos,eastPos,northPos,southPos;
  QPointF centerPoint,basePoint,midPoint,referencePoint,lowerPoint;
  QPointF northEast,southWest;
};

#endif //XWTIKZSHAPE_H
