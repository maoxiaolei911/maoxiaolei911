/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTIKZSHAPE_H
#define XWTIKZSHAPE_H

#include <QPointF>
#include <QTransform>

#define XW_TIKZ_SHAPE_CALLOUT_SHORTERNPOINT \
  mathresult = state->angleBetweenPoints(calloutpointer,centerPoint); \
  double nangle = mathresult; \
  double sinpointerangle = sin(nangle); \
  double cospointerangle = cos(nangle); \
  x = calloutpointer.x() + sinpointerangle * state->calloutPointerShorten;\
  y = calloutpointer.y() + cospointerangle * state->calloutPointerShorten; \
  calloutpointer.setX(x); \
  calloutpointer.setY(y);


class XWTeXBox;
class XWPDFDriver;
class XWTikzState;
class XWTikzOperation;

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

  void init();

  void mapPos();

  static QPointF pointBorderEllipse(const QPointF & pd, const QPointF & ur);
  static QPointF pointBorderRectangle(const QPointF & pd, const QPointF & ur);

  QPointF pointShapeBorder(double x, double y);


  void setFontSize(int s);

private:  
  void circleShape(bool dopath = true);
  void coordinateShape(bool dopath = true);
  void rectangleShape(bool dopath = true);

  void circularSectorShape(bool dopath = true);
  void cylinderShape(bool dopath = true);
  void dartShape(bool dopath = true);
  void diamondShape(bool dopath = true);
  void ellipseShape(bool dopath = true);
  void isosceleTriangleShape(bool dopath = true);
  void kiteShape(bool dopath = true);
  void regularPolygonShape(bool dopath = true);
  void semicircleShape(bool dopath = true);
  void starShape(bool dopath = true);
  void trapeziumShape(bool dopath = true);

  void cloudShape(bool dopath = true);
  void correctForbiddenSignShape(bool dopath = true);
  void forbiddenSignShape(bool dopath = true);
  void magneticTapeShape(bool dopath = true);
  void magnifyingGlassShape(bool dopath = true);
  void signalShape(bool dopath = true);
  void starburstShape(bool dopath = true);
  void tapeShape(bool dopath = true);

  void arrowBoxShape(bool dopath = true);
  void doubleArrowShape(bool dopath = true);
  void singleArrowShape(bool dopath = true);

  void circleSolidusShape(bool dopath = true);
  void circleSplitShape(bool dopath = true);
  void ellipseSplitShape(bool dopath = true);
  void rectangleSplitShape(bool dopath = true);

  void cloudCalloutShape(bool dopath = true);
  void ellipseCalloutShape(bool dopath = true);
  void rectangleCalloutShape(bool dopath = true);

  void chamferedRectangleShape(bool dopath = true);
  void crossoutShape(bool dopath = true);
  void roundedRectangleShape(bool dopath = true);
  void strikeoutShape(bool dopath = true);
  
  void circleeeShape(bool dopath = true);
  void directioneeShape(bool dopath = true);
  void rectangleeeShape(bool dopath = true);

  void batteryIECShape(bool dopath = true);
  void breakcontactIECShape(bool dopath = true);
  void breakdowndiodeIECShape(bool dopath = true);
  void capacitorIECShape(bool dopath = true);
  void genericcircleIECShape(bool dopath = true);
  void genericdiodeIECShape(bool dopath = true);
  void groundIECShape(bool dopath = true);
  void inductorIECShape(bool dopath = true);
  void makecontactIECShape(bool dopath = true);
  void varmakecontactIECShape(bool dopath = true);
  void varresistorIECShape(bool dopath = true);

  void andGateIECShape(bool dopath = true);
  void bufferGateIECShape(bool dopath = true);
  void nandGateIECShape(bool dopath = true);
  void norGateIECShape(bool dopath = true);
  void notGateIECShape(bool dopath = true);
  void orGateIECShape(bool dopath = true);  
  void xnorGateIECShape(bool dopath = true);
  void xorGateIECShape(bool dopath = true);

  void andGateUSShape(bool dopath = true);
  void bufferGateUSShape(bool dopath = true);
  void nandGateUSShape(bool dopath = true);
  void norGateUSShape(bool dopath = true);
  void notGateUSShape(bool dopath = true);
  void orGateUSShape(bool dopath = true);
  void xnorGateUSShape(bool dopath = true);
  void xorGateUSShape(bool dopath = true);

  void andGateCDHShape(bool dopath = true);
  void nandGateCDHShape(bool dopath = true);

  QPointF pointIntersectionOfLineAndArc(const QPointF & p1,
                                        const QPointF & p2,
                                        const QPointF & c,
                                        double sa,double ea,
                                        double xr,double yr);

  void circleAnchor();

  void doContent();

  void rectangleAnchor();

  void beforeBgACSource();
  void beforeBgBackwardDiodeIEC();
  void beforeBgBulbIEC();
  void beforeBgCurrentSourceIEC();
  void beforeBgDCSource();
  void beforeBgDiodeIEC();
  void beforeBgSchottkyDiodeIEC();
  void beforeBgTunnelDiodeIEC();
  void beforeBgVoltageSourceIEC();
  void beforeBgZenerDiodeIEC();

  void setCircuitShape();

  void doAnnotation();

  void doBeforeBackground();
  void doForeground();

public:
  XWPDFDriver * driver;
  XWTeXBox * box;
  XWTikzState * state;
  int nodeType;
  int codeType;

  double boxWidth,boxHeight,boxDepth,radius;

  QPointF anchorPos,centerPos,westPos,eastPos,northPos,southPos;
  QPointF centerPoint,basePoint,midPoint,referencePoint,lowerPoint;
  QPointF northEast,southWest;

  void (XWTikzShape::*sh_beforebackground)();
  void (XWTikzShape::*sh_foreground)();

  XWTikzOperation * option;
};

#endif //XWTIKZSHAPE_H
