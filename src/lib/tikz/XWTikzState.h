/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTIKZSTATE_H
#define XWTIKZSTATE_H

#include <QObject>
#include <QHash>
#include <QColor>
#include <QPointF>
#include <QRectF>
#include <QVector>
#include <QVector3D>
#include <QBrush>
#include <QPen>

#include "XWObject.h"
#include "XWTikzShape.h"

#define XW_TIKZ_HIT_DIST 3

#define XW_TIKZ_OUTSIDE 1
#define XW_TIKZ_INSIDE  2
#define XW_TIKZ_MOVETO  3
#define XW_TIKZ_LINETO  4
#define XW_TIKZ_CURVETO 5
#define XW_TIKZ_CLOSE   6

#define XW_TIKZ_NODE  0
#define XW_TIKZ_LABEL 1
#define XW_TIKZ_PIN   2
#define XW_TIKZ_CHILD 3
#define XW_TIKZ_EDGE  4
#define XW_TIKZ_CIRCUIT_HANDLE_SYMBOL 5
#define XW_TIKZ_INFO  6
#define XW_TIKZ_CIRCUIT_SYMBOL 7

#define XW_TIKZ_PATH_BEFOREBACKGROUND 0
#define XW_TIKZ_PATH_BEHINDFOREGROUND 1
#define XW_TIKZ_PATH_FOREGROUND 2
#define XW_TIKZ_PATH_BEFOREFOREGROUNDD 3

#define XW_TIKZ_CPS_CALLOUT -1
#define XW_TIKZ_CPS_SINGLE  -2
#define XW_TIKZ_CPS_AND     -3

#define XW_TIKZ_FROM_BORDER -1
#define XW_TIKZ_FROM_CENTER -2

class XWPDFDriver;
class XWTeXBox;
class XWTikzTextBox;
class XWTikzGraphic;
class XWTikzExpress;
class XWTikzCommand;
class XWTikzMatrix;
class XWTikzCoord;
class XWTIKZOptions;
class XWTikzDecoration;
class XWTikzCodes;
class XWTikzArrowSpecification;
class XWTikzArrow;
class XWTikzAnnotation;

class XWTikzState : public QObject
{
  Q_OBJECT

public:
  friend class XWTikzDecoration;
  friend class XWTikzArrow;
  friend class XWTikzShape;
  friend class XWTikzShade;
  friend class XWTikzFading;
  friend class XWTikzPattern;
  friend class XWTikzPlotHandler;
  friend class XWTikzPlotMark;
  friend class XWTikzArrowDependent;

  XWTikzState(XWTikzGraphic * graphicA,XWPDFDriver * driverA = 0,bool ispathA = true, QObject * parent = 0);
  XWTikzState(bool ispathA = false, QObject * parent = 0);
  ~XWTikzState();

  void acceptingByArrow();
  void acceptingByDouble();
  void addArc();
  void addCircle(const QPointF & c,double r);
  void addCircle();
  void addCode(XWTikzCodes * c);
  void addCosine(XWTikzCoord * p);
  void addCosine(const QPointF & p);
  void addEdge(XWTikzCoord * p);
  void addEdge(const QPointF & p);
  void addEdge();
  void addEllipse();
  void addEllipse(const QPointF & c,const QPointF & a,const QPointF & b);
  void addGrid(XWTikzCoord * p);
  void addGrid(const QPointF & p);
  void addParabola(XWTikzCoord * b, XWTikzCoord * e);
  void addParabola(const QPointF & b,const QPointF & e);
  void addParabola(const QPointF & e);
  void addPlot(XWTikzCoord * p);
  void addPlotFunction(XWTikzCoord * exp);
  void addRectangle(XWTikzCoord * p);
  void addRectangle(const QPointF & p);
  void addRectangle(const QPointF & ll,const QPointF & ur);
  void addShift(double xA,double yA);
  void addSine(XWTikzCoord * p);  
  void addSine(const QPointF & p);
  bool allowUpsideDown() {return isAllowUpsideDown;}  
  void angleSetup(double a,
                  double dimension, 
                  double linewidthfactor,
                  double outerfactor);
  void angleSetupPrime(double a);
  
  void clearPath();
  void clockwiseFrom(double a);
  void closePath();
  void concat(XWTikzState * newstate);
  void copy(XWTikzState * newstate,bool n = false);
  void copyTransform(XWTikzState * newstate);
  void counterClockwiseFrom(double a);
  
  void curveTo(double xA,double yA, double xB,double yB, double xC,double yC);
  void curveTo(XWTikzCoord * c1,XWTikzCoord * c2,XWTikzCoord * endpoint);
  void curveTo(XWTikzCoord * c1,XWTikzCoord * endpoint);
  void curveTo(const QPointF & c1,const QPointF & c2,const QPointF & endpoint);
  void cycle();

  QPointF doAnchor(XWTeXBox * box);
  void doEdgeFromParentForkDown();
  void doEdgeFromParentForkLeft();
  void doEdgeFromParentForkRight();
  void doEdgeFromParentForkUp();
  void doEdgeFromParentPath();
  void doToPath();
  void dragTo(XWTeXBox * box);
  void drawArrow(int a);

  void flush();

  double getAbove() {return above;}
  int getAlign() {return align;}
  double getAngle() {return angle;}
  int getAnchor() {return anchor;}
  double getArrowLength() {return arrowLength;}
  double getArrowTotalLength(int a);
  double getArrowWidth() {return arrowWidth;}
  int getAuto() {return autoAnchor;}
  QColor getBallColor() {return ballColor;}
  double getBelow() {return below;}
  QColor getBottomColor() {return bottomColor;}
  QPointF getCenterPoint();
  QColor getColor(const QString & nameA);
  XWTikzCoord * getCoord(const QString & nameA);
  XWTikzCoord * getCurrentCoord();
  int    getCurrentColumn() {return curColumn;}
  QPointF       getCurrentPoint();
  int    getCurrentRow() {return curRow;}
  QPointF getDecoratedPathFirst();
  QPointF getDecoratedPathLast();
  QPointF getDecorateInputSegmentFirst();
  QPointF getDecorateInputSegmentLast();
  QPointF getFirstPoint();
  XWTikzCoord * getInitialCoord();
  QColor  getInnerColor() {return innerColor;}
  double  getInnerLineWidth() {return innerLineWidth;}
  double  getInnerXSep() {return innerXSep;}
  double  getInnerYSep() {return innerYSep;}
  QList<QPointF> getIntersections(const QList<int> & operationsA,
                                  const QList<QPointF> & pointsA);
  QPointF getLastPoint(); 
  QPointF getLastMousePoint(); 
  double  getLeft() {return left;}
  QColor  getLeftColor() {return leftColor;}
  int     getLevel() {return level;}
  double  getLineWidth() {return lineWidth;}
  QColor  getLowerLeftColor() {return lowerLeftColor;}
  QColor  getLowerRightColor() {return lowerRightColor;}
  int     getMarkPhase() {return markPhase;}
  int     getMarkRepeat() {return markRepeat;}
  double  getMarkSize() {return markSize;}
  int     getMindmap() {return mindmap;}
  QColor  getMiddleColor() {return middleColor;}
  double  getMinHeight() {return minHeight;}
  double  getMinWidth() {return minWidth;}
  QPointF getMousePoint();  
  QString getNamePrefix() {return namePrefix;}
  QString getNameSuffix() {return nameSuffix;}
  QColor  getOuterColor() {return outerColor;}
  double  getOuterXSep() {return outerXSep;}
  double  getOuterYSep() {return outerYSep;}
  void    getPath(QList<int> & operationsA,QList<QPointF> & pointsA);
  double  getPathMaxX() {return pathMaxX;}
  double  getPathMaxY() {return pathMaxY;}
  double  getPathMinX() {return pathMinX;}
  double  getPathMinY() {return pathMinY;}
  QColor  getPatternColor() {return patternColor;}
  int     getPictureType() {return pictureType;}
  double  getPos() {return nodeTime;}
  int     getPost() {return post;}
  double  getPostlength() {return postlength;}
  int     getPre() {return pre;}
  double  getPrelength() {return prelength;}
  XWTikzCoord * getPreviousCoord(XWTikzCoord * p);
  double getRight() {return right;}
  QColor getRightColor() {return rightColor;}
  int getShape() {return shape;}
  int getSpyUsing() {return spyUsing;}
  QString getString(const QString & nameA);  
  double  getTension() {return tension;}
  QString getText() {return text;}
  double getTextWidth() {return textWidth;}
  QColor getTopColor() {return topColor;}
  QPointF getToStart() {return toStart;}
  QPointF getToTarget() {return toTarget;}
  QTransform getTransform() {return transform;}
  QColor getUpperLeftColor() {return upperLeftColor;}
  QColor getUpperRightColor() {return upperRightColor;}
  double getValue(const QString & nameA);
  void   getWidthAndHeight(double & w, double & h);
  double getXVec() {return xVec;}
  double getYVec() {return yVec;}
  double getZVec() {return zVec;}
  void   growCyclic();
  void   growViaThreePoints(const QPointF & p1, 
                            const QPointF & p2, 
                            const QPointF & p3);

  bool hitTestArc();
  bool hitTestChild(XWTeXBox * box);
  bool hitTestCoordinate(XWTeXBox * box);
  bool hitTestCosine(XWTikzCoord * p);
  bool hitTestCurve(XWTikzCoord * c1,XWTikzCoord * c2);
  bool hitTestCycle();
  bool hitTestEllipse();
  bool hitTestFunction(XWTikzCoord * exp);
  bool hitTestGrid();
  bool hitTestHVLine();
  bool hitTestLine();
  bool hitTestMatrix(XWTikzMatrix * box);
  bool hitTestParabola(XWTikzCoord * b, XWTikzCoord * e);
  bool hitTestPoint(XWTikzCoord * p);
  bool hitTestRectangle();
  bool hitTestSine(XWTikzCoord * p);
  bool hitTestText(XWTikzTextBox * box);
  bool hitTestVHLine();

  void incLevel() {level++;}
  void initialByArrow();
  void inverted();
  bool isAuto() {return isAutoSet;}
  bool isMatrix() {return matrix;}
  bool isOnNode() {return onNode;}
  bool isPosSet() {return isTimeSet;}
  bool isSloped() {return isSlopedSet;}
  bool isSwap() {return isSwapSet;}

  double  lengthDependent(double dimension, 
                          double lengthfactor,
                          double linewidthfactor);
  void    lineTo(XWTikzCoord * p);
  void    lineTo(const QPointF & p);
  void    lineTo(double xA, double yA);
  void    lineToHV(XWTikzCoord * p);
  void    lineToHV(const QPointF & p);
  void    lineToVH(XWTikzCoord * p);
  void    lineToVH(const QPointF & p);
  double  lineWidthDependent(double dimension, 
                          double linewidthfactor,
                          double outerfactor);

  QRectF  map(const QRectF & rectA);
  QPointF map(const QPointF & pA);
  QPointF map(double x,double y);
  QPointF mapInverted(const QPointF & pA);
  void    map(double x1,double y1, double * x2, double * y2);
  bool    moveTest(XWTeXBox * box);
  void    moveTo(double xA, double yA);
  void    moveTo(XWTikzCoord * p);
  void    moveTo(const QPointF & p);

  bool   needEdgeFromParent() {return edgeFromParent;}

  void plotStreamEnd(bool c);

  void removeLastCoord();
  void resetArrowBoxArrows();
  void resetTransform() {transform.reset();}
  void rotate(double d) {transform.rotate(d);}

  XWTikzState * restore();
    
  XWTikzState *save(bool ispathA = true);
  XWTikzState *saveNode(XWTeXBox * boxA,int nt);
  void scale(double sx,double sy) {transform.scale(sx,sy);}
  void setAbsolute(bool e) {absolute=e;}
  void setAcceptingAnchor(int a) {acceptingAnchor=a;}
  void setAcceptingAngle(double a) {acceptingAngle=a;}
  void setAcceptingDistance(double d) {acceptingDistance=d;}
  void setAcceptingText(const QString & t) {acceptingText=t;}
  void setAlign(int a);
  void setAllowUpsideDown() {isAllowUpsideDown=true;}
  void setAnchor(int a) {anchor=a;}
  void setAndGateIECSymbol(const QString & t) {andGateIECSymbol=t;}
  void setAngle(double a) {angle=a;}
  void setAnnotationArrow(XWTikzArrowSpecification * e);
  void setAnnotation(XWTikzAnnotation * a) {annotation=a;}
  void setArrowAngle(double a) {arrowAngle=a;}
  void setArrowArc(double a) {arrowArc=a;}
  void setArrowBoxEastArrow(const QPointF & e) {arrowBoxEastArrow=e;}
  void setArrowBoxTipAngle(double a) {arrowBoxTipAngle=a;}
  void setArrowBoxHeadExtend(double e) {arrowBoxHeadExtend=e;}
  void setArrowBoxHeadIndent(double i) {arrowBoxHeadIndent=i;}
  void setArrowBoxNorthArrow(const QPointF & n) {arrowBoxNorthArrow=n;}
  void setArrowBoxShaftWidth(double w) {arrowBoxShaftWidth=w;}
  void setArrowBoxSouthArrow(const QPointF & s) {arrowBoxSouthArrow=s;}
  void setArrowBoxWestArrow(const QPointF & w) {arrowBoxWestArrow=w;}
  void setArrowDefault(int a) {arrowDefault=a;}
  void setArrowDrawColor(const QColor & c) {isArrowDrawSet=true;arrowDrawColor=c;}
  void setArrowFill(bool e) {isArrowFillSet=e;}
  void setArrowFillColor(const QColor & c) {isArrowFillSet=true;arrowFillColor=c;}
  void setArrowFlex(double f) {arrowFlex=f;}
  void setArrowFlexMode(int m) {arrowFlexMode=m;}
  void setArrowHarpoon(bool e) {arrowHarpoon=e;}
  void setArrowInset(double i) {arrowInset=i;}
  void setArrowLength(double l) {arrowLength=l;}
  void setArrowLineCap(int c) {arrowLineCap=c;}
  void setArrowLineJoin(int j) {arrowLineJoin=j;}
  void setArrowLineWidth(double w) {arrowLineWidth=w;}
  void setArrowReversed(bool e) {arrowReversed=e;}
  void setArrowScaleLength(double l) {arrowScaleLength=l;}
  void setArrowScaleWidth(double w) {arrowScaleWidth=w;}
  void setArrowSep(double s) {arrowSep=s;}
  void setArrowSlant(double s) {arrowSlant=s;}
  void setArrowSwap(bool e) {arrowSwap=e;}
  void setArrowWidth(double w) {arrowWidth=w;}
  void setAspect(double a);
  void setAt(XWTikzCoord * p) {at = p;}
  void setAtPosition(XWTikzExpress * p) {position=p;}
  void setAuto(int d);
  void setBallColor(const QColor & c) {ballColor=c;}
  void setBend(XWTikzCoord * p) {bend=p;}
  void setBendAtEnd() {bendAtEnd=true;}
  void setBendAtStart() {bendAtStart=true;}
  void setBendHeight(double h);
  void setBendPos(double v) {bendPosIsSet=true;bendPos=v;}
  void setBottomColor(const QColor & c) {bottomColor=c;}
  void setBufferGateIECSymbol(const QString & t) {bufferGateIECSymbol=t;}
  void setCalloutPointerArc(double a) {calloutPointerArc=a;}
  void setCalloutPointerEndSize(const QPointF & s) {calloutPointerEndSize=s;}
  void setCalloutPointerSegments(int s) {calloutPointerSegments=s;}
  void setCalloutPointerShorten(double s) {calloutPointerShorten=s;}
  void setCalloutPointerStartSize(const QPointF & s) {calloutPointerStartSize=s;}
  void setCalloutPointerWidth(double w) {calloutPointerWidth=w;}
  void setCalloutRelativePointer(const QPointF & p) {calloutRelativePointer=p;}
  void setChamferedRectangleAngle(double a) {chamferedRectangleAngle=a;}
  void setChamferedRectangleXsep(double s) {chamferedRectangleXsep=s;}
  void setChamferedRectangleCorners(const QList<int> & cs);
  void setChamferedRectangleYsep(double s) {chamferedRectangleYsep=s;}
  void setChildAnchor(int a) {childAnchor=a;isChildAnchorSet=true;}
  void setChildrenNumber(int i) {childrenNumber=i;}
  void setCircuitSizeUnit(double s);
  void setCircuitSymbolSize(double w, double h);
  void setCircularSectorAngle(double a) {circularSectorAngle=a;}  
  void setClip(bool e) {isClipSet=e;}
  void setCloudIgnoresAspect(bool e) {cloudIgnoresAspect=e;}
  void setCloudPuffArc(double a) {cloudPuffArc=a;}
  void setCloudPuffs(double p) {cloudPuffs=p;}
  void setCodes(const QList<XWTikzCommand*> & cmdsA);
  void setColor(const QColor & c);
  void setColumnSep(double s) {columnSep=s;}
  void setCoreColor(const QColor & c) {coreColor=c;}
  void setCurrentDirectionArrow(XWTikzArrowSpecification * e);
  void setCurrentChild(int i) {currentChild=i;}
  void setCurrentColumn(int i) {curColumn=i;}
  void setCurrentRow(int i) {curRow=i;}
  void setCylinderBodyFill(const QColor & c) {cylinderBodyFill=c;}
  void setCylinderEndFill(const QColor & c) {cylinderEndFill=c;}
  void setCylinderUsesCustomFill(bool e) {cylinderUsesCustomFill=e;}
  void setDartTailAngle(double a) {dartTailAngle=a;}
  void setDartTipAngle(double a) {dartTipAngle=a;}
  void setDash(int d);
  void setDashPattern(const QVector<qreal> & dpA);
  void setDashPhase(double v) {dashPhase=v;}
  void setDecorate(bool e) {isDecorateSet=e;}
  void setDecoration(int d);
  void setDecorationSegmentAngle(double a) {decorationSegmentAngle=a;}
  void setDeltaAngle(double a) {deltaIsSet=true;deltaAngle=a;}
  void setDomain(double s, double e) {domainStart=s;domainEnd=e;}
  void setDoubleArrowTipAngle(double a) {doubleArrowTipAngle=a;}
  void setDoubleArrowHeadExtend(double e) {doubleArrowHeadExtend=e;}
  void setDoubleArrowHeadIndent(double i) {doubleArrowHeadIndent=i;}
  void setDraw(bool e) {isDrawSet = e;}
  void setDrawColor(const QColor & c);
  void setDrawOpacity(double v) {drawOpacity=v;}
  void setEdgeFromParent(bool e=true) {edgeFromParent=e;}
  void setEndAngle(double a) {endIsSet=true;endAngle=a;}
  void setEndArrow(XWTikzArrowSpecification * e) {endArrow=e;}
  void setEndPosition(XWTikzExpress * p) {endPosition=p;}
  void setFadingRoate(double d);
  void setFadingScale(double sx,double sy);
  void setFadingShift(double dx,double dy);
  void setFadingSlant(double sx,double sy);
  void setFill(bool e) {isFillSet = e;}
  void setFillColor(const QColor & c);
  void setFillOpacity(double v) {fillOpacity=v;}
  void setFillRule(int r) {interiorRule=r;}
  void setFitFading(bool e) {fitFading=e;}
  void setFontSize(int s) {fontSize=s;}
  void setGrow(int g);
  void setGrowOpposite(int g);
  void setGrowthParentAnchor(int a) {growthParentAnchor=a;}
  void setHelpLines(bool h) {helpLines=h;}
  void setInitialAnchor(int a) {initialAnchor=a;}
  void setInitialAngle(double a) {initialAngle=a;}
  void setInitialDistance(double d) {initialDistance=d;}
  void setInitialText(const QString & t) {initialText=t;}
  void setInnerColor(const QColor & c) {innerColor=c;}
  void setInnerLineWidth(double w) {innerLineWidth=w;}
  void setInnerXSep(double s) {innerXSep=s;}
  void setInnerYSep(double s) {innerYSep=s;}
  void setInputs(const QList<QChar> & ins);
  void setIsoscelesTriangleApexAngle(double a) {isoscelesTriangleApexAngle=a;}
  void setIsoscelesTriangleStretches(double s) {isoscelesTriangleStretches=s;}
  void setKiteLowerVertexAngle(double a) {kiteLowerVertexAngle=a;}
  void setKiteUpperVertexAngle(double a) {kiteUpperVertexAngle=a;}
  void setLabelAngle(double a) {labelAngle=a;}
  void setLabelDistance(double d) {labelDistance=d;}
  void setLabelPosition(double p) {labelAngle=p;}
  void setLastMousePoint(const QPointF & p) {lastMousePoint = p;}
  void setLeftColor(const QColor & c) {leftColor=c;}
  void setLevel(int l) {level=l;}
  void setLevelDistance(double d) {levelDistance=d;}
  void setLineCap(int c);
  void setLineJoin(int j);
  void setLineWidth(double w);
  void setLocation(int l) {location=l;}
  void setLogicGateAnchorsUseBoundingBox(bool e) {logicGateAnchorsUseBoundingBox=e;}
  void setLogicGateIECSymbolAlign(int a) {logicGateIECSymbolAlign=a;}
  void setLogicGateIECSymbolColor(const QColor & c) {logicGateIECSymbolColor=c;}
  void setLogicGateInputSep(double s) {logicGateInputSep=s;}
  void setLogicGateInvertedRadius(double r) {logicGateInvertedRadius=r;}
  void setLowerLeftColor(const QColor & c) {lowerLeftColor=c;}
  void setLowerRightColor(const QColor & c) {lowerRightColor=c;}
  void setMagneticTapeTail(double p) {magneticTapeTail=p;}
  void setMagneticTapeTailExtend(double e) {magneticTapeTailExtend=e;}
  void setMGHAaspect(double a) {MGHAaspect=a;}
  void setMGHAfill(double f) {MGHAfill=f;}
  void setMark(int m) {mark=m;}
  void setMarkNode(const QString & n) {markNode=n;}
  void setMarkPhase(int p) {markPhase=p;}
  void setMarkRepeat(int r) {markRepeat=r;}
  void setMarkSize(double m) {markSize=m;}
  void setMatrix() {matrix=true;}
  void setMatrix(double a,double b,
                 double c,double d,
                 double dx, double dy);
  void setMatrixAnchor(int a) {matrixAnchor=a;isMatrixAnchorSet=true;}
  void setMiddleColor(const QColor & c) {middleColor=c;}
  void setMindmap(int m);
  void seiMinimumHeight(double h) {minHeight=h;}
  void seiMinimumWidth(double w) {minWidth=w;}
  void setMiterLimit(double v);
  void setMirror(bool m);
  void setMissing(bool e) {missing=e;}
  void setMousePoint(const QPointF & p) {mousePoint = p;}
  void setNamePrefix(const QString & n) {namePrefix=n;}
  void setNameSuffix(const QString & n) {nameSuffix=n;}
  void setNAndGateIECSymbol(const QString & t) {nandGateIECSymbol=t;}
  void setNodeContents(const QString & str);
  void setNodeType(int shapeA,int nt);
  void setNorGateIECSymbol(const QString & t) {norGateIECSymbol=t;}
  void setNotGateIECSymbol(const QString & t) {notGateIECSymbol=t;}
  void setOnNode(bool e) {onNode=e;}
  void setOpacity(double v);
  void setOrGateIECSymbol(const QString & t) {orGateIECSymbol=t;}
  void setOuterColor(const QColor & c) {outerColor=c;}
  void setOuterXSep(double s) {outerXSep=s;}
  void setOuterYSep(double s) {outerYSep=s;}
  void setParentAnchor(int a) {parentAnchor=a;isParentAnchorSet=true;}
  void setPathFading(int f) {pathFading = f;}
  void setPattern(bool e) {isPatternSet=e;}
  void setPatternColor(const QColor & c);
  void setPatternName(int n);
  void setPictureType(int t);
  void setPlane();
  void setPlane(double xa, double ya,
                double xb, double yb,
                double x, double y);
  void setPlaneX(const QPointF & p);
  void setPlaneY(const QPointF & p);
  void setPlaneOrigin(const QPointF & p);
  void setPlotHandler(int h) {handler=h;}
  void setPos(double p);
  void setPost(int p) {post=p;}
  void setPostlength(double l) {postlength=l;}
  void setPre(int p) {pre=p;}
  void setPrelength(double l) {prelength = l;}
  void setRadius(double r) {xradius=r;yradius=r;}
  void setRaise(double r);
  void setRandomStarburst(int r) {randomStarburst=r;}
  void setRegularPolygonSides(double s) {regularPolygonSides=s;}
  void setRectangleSplitDrawSplits(bool e) {rectangleSplitDrawSplits=e;}
  void setRectangleSplitEmptyPartDepth(double d) {rectangleSplitEmptyPartDepth=d;}
  void setRectangleSplitEmptyPartHeight(double h) {rectangleSplitEmptyPartHeight=h;}
  void setRectangleSplitEmptyPartWidth(double w) {rectangleSplitEmptyPartWidth=w;}
  void setRectangleSplitHorizontal(bool e) {rectangleSplitHorizontal=e;}
  void setRectangleSplitIgnoreEmptyParts(bool e) {rectangleSplitIgnoreEmptyParts=e;}
  void setRectangleSplitPartAlign(const QList<int> & l);
  void setRectangleSplitPartFill(const QList<QColor> & l);
  void setRectangleSplitParts(int p) {rectangleSplitParts=p;}
  void setRectangleSplitUseCustomFill(bool e) {rectangleSplitUseCustomFill=e;}
  void setReversePath(bool e) {reversePath=e;}
  void setRightColor(const QColor & c) {rightColor=c;}
  void setRoundedCorners(double v) {roundedCorners=v;}
  void setRoundedRectangleArcLength(double l) {roundedRectangleArcLength=l;}
  void setRoundedRectangleEastArc(int t) {roundedRectangleEastArc=t;}
  void setRoundedRectangleLeftArc(int t) {roundedRectangleLeftArc=t;}
  void setRoundedRectangleRightArc(int t) {roundedRectangleRightArc=t;}
  void setRoundedRectangleWestArc(int t) {roundedRectangleWestArc=t;}
  void setRowSep(double s) {rowSep=s;}
  void setSamples(int i) {samples=i;}
  void setSamplesAt(const QList<double> & sa);
  void setShade(bool e) {isShadeSet=e;}
  void setShape(int s) {shape=s;}
  void setShadingAngle(double a) {shadingAngle=a;}
  void setShadingName(int n);
  void setShapeAspect(double a) {shapeAspect=a;}
  void setShapeBorderRotate(double a) {shapeBorderRotate=a;}
  void setShapeBorderUsesIncircle(bool e) {shapeBorderUsesIncircle=e;}
  void setShortenStart(double l) {shortenStart = l;}
  void setShortenEnd(double l) {shortenEnd = l;}
  void setSiblingAngle(double a) {siblingAngle=a;}
  void setSiblingDistance(double d) {siblingDistance=d;}
  void setSignalFrom(int f) {signalFrom=f;}
  void setSignalFromOpposite(int o) {signalFromOpposite=o;}
  void setSignalPointerAngle(double a) {signalPointerAngle=a;}  
  void setSignalTo(int t) {signalTo=t;}
  void setSignalToOpposite(int o) {signalToOpposite=o;}
  void setSingleArrowTipAngle(double a) {singleArrowTipAngle=a;}
  void setSingleArrowHeadExtend(double e) {singleArrowHeadExtend=e;}
  void setSingleArrowHeadIndent(double i) {singleArrowHeadIndent=i;}
  void setSloped() {isSlopedSet=true;}
  void setSpyUsing(int s) {spyUsing=s;}
  void setStarburstPointHeight(double h) {starburstPointHeight=h;}
  void setStarburstPoints(int p) {starburstPoints=p;}
  void setStarPointHeight(double h) {starPointHeight=h;useStarRatio=false;}
  void setStarPoints(int p) {starPoints=p;}
  void setStarPointRatio(double r) {starPointRatio=r;useStarRatio=true;}
  void setStartAngle(double a) {startIsSet=true;startAngle=a;}
  void setStartArrow(XWTikzArrowSpecification * a) {startArrow = a;}
  void setStartPosition(XWTikzExpress * p) {startPosition=p;}
  void setStep(double sx,double sy) {xstep = sx;ystep=sy;}
  void setStep(XWTikzExpress * s) {step=s;}
  void setSwap() {isSwapSet=true;}
  void setTapeBendBottom(int s) {tapeBendBottom=s;}
  void setTapeBendHeight(double h) {tapeBendHeight=h;}
  void setTapeBendTop(int s) {tapeBendTop=s;}
  void setTension(double t) {tension=t;}
  void setText(const QString & s) {text=s;}  
  void setTextColor(const QColor & c);
  void setTextOpacity(double o);
  void setTextWidth(double w) {textWidth = w;}
  void setTopColor(const QColor & c) {topColor=c;}
  void setTransform(const QTransform & transA);
  void setTransformShape() {transformShape=true;}
  void setTrapeziumLeftAngle(double a) {trapeziumLeftAngle=a;}
  void setTrapeziumRightAngle(double a) {trapeziumRightAngle=a;}
  void setTrapeziumStretches(bool e) {trapeziumStretches=e;}
  void setTrapeziumStretchesBody(bool e) {trapeziumStretchesBody=e;}
  void setupArrow(int a);
  void setUpperLeftColor(const QColor & c) {upperLeftColor=c;}
  void setUpperRightColor(const QColor & c) {upperRightColor=c;}
  void setUseAsBoundingBox(bool e) {isUseAsBoundingBoxSet=e;}
  void setVariables(const QString & nameA,const QString & var);
  void setXNorGateIECSymbol(const QString & t) {xnorGateIECSymbol=t;}
  void setXorGateIECSymbol(const QString & t) {xorGateIECSymbol=t;}
  void setXRadius(double r) {xradius=r;}
  void setXStep(double sx) {xstep = sx;}
  void setXVec(double v) {xVec=v;}
  void setYRadius(double r) {yradius=r;}
  void setYStep(double sy) {ystep=sy;}
  void setYVec(double v) {yVec=v;}
  void setZVec(double v) {zVec=v;}
  void shift(double dx,double dy);
  void shiftOnly();
  void slant(double sx,double sy);
  void spyAt(XWTikzCoord * p);
  void spyOn(XWTikzCoord * p);

  QPointF tangent(const QPointF & nc, 
                  const QPointF & ne,
                  const QPointF & p, 
                  int s);
  void toPath(XWTikzCoord * p = 0);
  
  void transformColumn();
  
  
  void transformRow(double h);

private:
  void addArc(double sa,double ea, double xr,double yr);
  void addGrid(const QPointF & ll,const QPointF & ur);
  void addPlotStreamPoint(const QPointF & p);
  void addRoundedCorner(const QPointF & a,const QPointF & b,const QPointF & c);
  void anchorChild();
  void anchorNode();
  void anchorParent();  
  double angleBetweenLines(const QPointF & p1,const QPointF & p2,
                           const QPointF & p3,const QPointF & p4);
  double angleBetweenPoints(const QPointF & p1,const QPointF & p2);
  void arcOfEllipse(const QPointF & c,const QPointF & endpoint,
                   double sa,double sb);
  void arcTo(double la,double lb, double xr,double yr); 

  void    circuitHandleSymbol();
  void    computeAngle();
  void    computeDirection();
  void    computeShortening(XWTikzArrow * a);
  void    constructPath();  
  QPointF curveAtTime(double t,
                      const QPointF & startpoint,
                      const QPointF & c1, 
                      const QPointF & c2,
                      const QPointF & endpoint);
  void curveBetweenTime(double s, double t,QPointF & startpoint,
                        QPointF & c1,QPointF & c2,
                        QPointF & endpoint,bool ignoremoveto);
  void curveBetweenTime(double t,const double ts,double tt,
                      const QPointF & startpoint,const QPointF & c1, 
                      const QPointF & c2,const QPointF & endpoint,bool ignoremoveto);
  double curviLinearDistanceToTime(double d);
  void   curviLinearBezierOrthogonal(double & d,double & f);
  void   curviLinearBezierPolar(double & d,double & f);

  double decorateCurveLength(const QPointF & c1,
                             const QPointF & c2,
                             const QPointF & c3,
                             const QPointF & c4);
  double decorateCurveLeft(const QPointF & c1,
                             const QPointF & c2,
                             const QPointF & c3,
                             const QPointF & c4);
  double decorateCurveRight(const QPointF & c1,
                             const QPointF & c2,
                             const QPointF & c3,
                             const QPointF & c4);
  double decorateLineLength(const QPointF & c1,const QPointF & c2);
  void   decorateMoveAlongInputSegmentCurve(double distancetomove,
                                            double currentpathlength,
                                            double & decorateangle,
                                            double & inputsegmenttime,
                                            const QPointF & startpoint,
                                            const QPointF & c1, 
                                            const QPointF & c2,
                                            const QPointF & endpoint);
  void   decorateTransformCurve(double t,
                                const QPointF & startpoint,
                                const QPointF & c1, 
                                const QPointF & c2,
                                const QPointF & endpoint);
  void   decorateTransformLine(const QPointF & c,double decorateangle,double decoratedistance);
  void   doNodes();

  void   edgeFromParentForkDown();
  void   edgeFromParentForkLeft();
  void   edgeFromParentForkRight();
  void   edgeFromParentForkUp();

  bool hitTestLine(const QPointF & p1, const QPointF & p2);
  bool hitTestLines();
  bool hitTestPoint(const QPointF & p);

  void init();
  void initPath();
  void intersectionsOfCurversLeft(const QPointF & p1,const QPointF & p2,
                                        const QPointF & p3,const QPointF & p4,
                                        const QPointF & p5,const QPointF & p6,
                                        const QPointF & p7,const QPointF & p8);
  void intersectionsOfCurversRight(const QPointF & p1,const QPointF & p2,
                                        const QPointF & p3,const QPointF & p4,
                                        const QPointF & p5,const QPointF & p6,
                                        const QPointF & p7,const QPointF & p8);
  QPointF intersectionOfCircles(const QPointF & c1,const QPointF & c2,
                                double r1,double r2,int s);
  void intersectionsOfCurvers(const QPointF & p1,const QPointF & p2,
                                        const QPointF & p3,const QPointF & p4,
                                        const QPointF & p5,const QPointF & p6,
                                        const QPointF & p7,const QPointF & p8);
  void intersectionOfCurveAndLine(const QPointF & p1,const QPointF & p2,
                                  const QPointF & p3,const QPointF & p4,
                                  const QPointF & p5,const QPointF & p8);
  void intersectionOfLineAndCurve(const QPointF & p1,const QPointF & p4,
                                  const QPointF & p5,const QPointF & p6,
                                  const QPointF & p7,const QPointF & p8);
  QPointF intersectionOfLines(const QPointF & p1,const QPointF & p2,
                              const QPointF & p3,const QPointF & p4);
  void invokeDecorate();
  bool isLinesIntersect(const QPointF & p1,
                        const QPointF & p2,
                        const QPointF & p3,
                        const QPointF & p4);

  QPointF lineAtDistance(double t, const QPointF & p1,const QPointF & p2);
  QPointF lineAtTime(double t, const QPointF & p1,const QPointF & p2);

  int    mathDefaultZ();
  int    mathGeneratePseudoRandomNumber(int z);
  double mathrand(int z);
  int    mathRandomInteger(int l, int u, int z);
  double mathrnd(int z);

  void quadraticCurveTo(const QPointF & c1,const QPointF & endpoint);

  void    rectangle(const QPointF & ll,double w, double h);
  QPointF rotatePointAround(const QPointF & p,const QPointF & r,double d);
  void    runCodes();

  void setAutoAnchor(double x, double y);
  void setCurviLinearBezierCurve(const QPointF & startpoint,
                                 const QPointF & c1, 
                                 const QPointF & c2,
                                 const QPointF & endpoint);

  void timerCurver();
  void timerHVLine();
  void timerLine();
  void timerVHLine();
  void toPathAcceptingByArrow();
  void toPathDefault();
  void toPathInitialByArrow();
  void transformArrow(const QPointF & p1,const QPointF & p2);
  void transformArrowBend();
  void transformArrowCurved(XWTikzArrow * a,
                         const QPointF & startpoint,
                         const QPointF & c1, 
                         const QPointF & c2,
                         const QPointF & endpoint);
  void transformArrowRigit(XWTikzArrow * a,double s, double e);
  void transformArrowStraight(XWTikzArrow * a,
                           const QPointF & c1, 
                           const QPointF & c2);
  void transformChild();
  void transformCurveAtTime(double t,const QPointF & startpoint,const QPointF & c1, 
                      const QPointF & c2,const QPointF & endpoint);
  void transformLineAtTime(double t, const QPointF & p1,const QPointF & p2);
  void transformNode();
  void transformTriangle(double xa,double ya,
                         double xb,double yb,
                         double x,double y);

  double veclen(double xA,double yA);

private:  
  XWTikzGraphic * graphic;
  XWPDFDriver * driver;
  bool isPath;
  int curLabel;

  int    pictureType;
  double circuitsSizeUnit;
  double circuitSymbolWidth;
  double circuitSymbolHeight;
  XWTikzAnnotation *  annotation;
  double logicGateInvertedRadius;
  double logicGateInputSep;
  bool   logicGateAnchorsUseBoundingBox;
  QString andGateIECSymbol;
  QString nandGateIECSymbol;
  QString orGateIECSymbol;
  QString norGateIECSymbol;
  QString xorGateIECSymbol;
  QString xnorGateIECSymbol;
  QString notGateIECSymbol;
  QString bufferGateIECSymbol;
  int     logicGateIECSymbolAlign;
  QColor  logicGateIECSymbolColor;

  double lineWidth;
  double innerLineWidth;

  double roundedCorners;

  double xradius,yradius;

  XWTikzCoord * at;

  bool endIsSet,startIsSet,deltaIsSet;
  double startAngle,endAngle,deltaAngle;

  double xstep;
  double ystep;

  bool helpLines;

  XWTikzCoord * bend;
  bool bendPosIsSet;
  double bendPos;
  bool bendHeightIsSet;
  double bendHeight;
  bool bendAtStart,bendAtEnd;

  bool isDrawSet;

  bool isFillSet;
  bool isFillText;
  int  interiorRule;

  bool isClipSet;
  
  bool isPatternSet;
  int  patternName;
  QColor patternColor;

  bool isShadeSet;
  int shadingName;
  double shadingAngle;
  QColor topColor;
  QColor bottomColor;
  QColor middleColor;
  QColor leftColor;
  QColor rightColor;
  QColor ballColor;
  QColor lowerLeftColor;
  QColor upperLeftColor;
  QColor lowerRightColor;
  QColor upperRightColor;
  QColor innerColor;
  QColor outerColor;

  int pathFading;
  bool fitFading;
  QTransform fadingTransform;

  QColor coreColor;

  bool isDecorateSet;
  int  decoration;
  int  pre;
  double prelength;
  int  post;
  double postlength;
  bool reversePath,pathHasCorners;
  double decorateWidth;
  double decoratedAngle;
  double decoratedCompletedDistance;
  double decoratedRemainingDistance;
  double decoratedPathLength;
  double decoratedInputSegmentLength;
  double decoratedInputSegmentCompletedDistance;
  double decoratedInputSegmentRemainingDistance;
  double decorationSegmentAmplitude;
  double decorationSegmentLength;
  double decorationSegmentAngle;
  double decorationSegmentAspect;
  double metaDecorationSegmentAmplitude;
  double metaDecorationSegmentLength;
  double startRadius,endRadius,decorationAngle;
  double autoEndOnLength,autoCornerOnLength;
  int    curOperation;

  bool isUseAsBoundingBoxSet;

  bool matrix;
  bool isMatrixAnchorSet;
  int matrixAnchor;
  double columnSep;
  double rowSep;
  int  curColumn;
  int  curRow;

  int    level;
  double levelDistance;
  double siblingDistance;
  double  growAngle;
  double  growLeft;
  double  growRight;
  bool missing;
  int  growthParentAnchor;
  bool isChildAnchorSet;
  int  childAnchor;
  bool isParentAnchorSet;
  int  parentAnchor;
  bool edgeFromParent;
  int  currentChild;
  int  childrenNumber;
  bool isTransformChildSet;

  bool   isContinue;
  int    handler;
  double tension;
  int    mark;
  int    markRepeat;
  int    markPhase;
  double markSize;

  int arrowDefault;
  XWTikzArrowSpecification * startArrow;
  XWTikzArrowSpecification * endArrow;
  double shortenStart,shortenEnd;
  bool   isEnd;
  double arrowLength,arrowWidth,arrowInset,arrowAngle,arrowLineWidth,arrowSep;
  double shorteningDistance, arrowTotalLength;
  double arrowScaleLength,arrowScaleWidth;
  double arrowArc;
  double arrowSlant;
  bool   arrowReversed,arrowHarpoon,arrowSwap;
  QColor arrowDrawColor,arrowFillColor;
  bool   isArrowFillSet, isArrowDrawSet;
  int    arrowLineCap,arrowLineJoin;
  double arrowFlex;
  int    arrowFlexMode;
  int    arrowBendMode;
  bool   preciseShortening;
  double nextTip;

  QTransform transform;
  QTransform decorateTransform;

  double xVec,yVec,zVec;

  double drawOpacity;
  double fillOpacity;

  int anchor;
  double angle;
  double textWidth;
  int align;
  int shape;
  int fontSize;
  double innerXSep;
  double innerYSep;
  double outerXSep;
  double outerYSep;
  double minHeight;
  double minWidth;
  double shapeAspect;
  bool shapeBorderUsesIncircle;
  double shapeBorderRotate;
  double above,below,left,right;
  bool   transformShape;
  double nodeTime;
  bool   isTimeSet;
  int    autoAnchor;
  bool   isAutoSet;
  bool   isSwapSet;
  bool   isSlopedSet;
  bool   isAllowUpsideDown;
  int    location;

  double labelAngle;
  double labelDistance;
  bool   absolute;

  double aspect,aspectInverse;

  double trapeziumLeftAngle,trapeziumRightAngle;
  bool   trapeziumStretches,trapeziumStretchesBody;

  int regularPolygonSides;

  int starPoints;
  double starPointHeight,starPointRatio;
  bool useStarRatio;

  double isoscelesTriangleApexAngle;
  bool   isoscelesTriangleStretches;

  double kiteUpperVertexAngle,kiteLowerVertexAngle;

  double dartTipAngle,dartTailAngle;

  double circularSectorAngle;

  bool cylinderUsesCustomFill;
  QColor cylinderEndFill,cylinderBodyFill;

  double cloudPuffs,cloudPuffArc;
  bool   cloudIgnoresAspect,cloudAnchorsUseEllipse;

  double MGHAfill,MGHAaspect,MGHA;

  int    starburstPoints,randomStarburst;
  double starburstPointHeight;

  double signalPointerAngle;
  int    signalFrom,signalFromOpposite,signalTo,signalToOpposite;

  int tapeBendTop,tapeBendBottom;
  double tapeBendHeight;

  double magneticTapeTailExtend,magneticTapeTail;

  double singleArrowTipAngle,singleArrowHeadExtend,singleArrowHeadIndent;

  double doubleArrowTipAngle,doubleArrowHeadExtend,doubleArrowHeadIndent;

  double arrowBoxTipAngle,arrowBoxHeadExtend,arrowBoxHeadIndent,arrowBoxShaftWidth;
  QPointF arrowBoxNorthArrow,arrowBoxSouthArrow,arrowBoxEastArrow,arrowBoxWestArrow;

  int  rectangleSplitParts;
  bool rectangleSplitHorizontal,rectangleSplitIgnoreEmptyParts;
  double rectangleSplitEmptyPartWidth,rectangleSplitEmptyPartHeight,rectangleSplitEmptyPartDepth;
  bool rectangleSplitDrawSplits,rectangleSplitUseCustomFill;
  QList<int> rectangleSplitPartAlign;
  QList<QColor> rectangleSplitPartFill;

  double calloutPointerAnchorAspect;

  QPointF calloutRelativePointer,calloutAbsolutePointer;
  double calloutPointerShorten;
  bool isCalloutAbsolutePointer;

  double calloutPointerWidth;

  double calloutPointerArc;

  QPointF calloutPointerStartSize,calloutPointerEndSize;
  int calloutPointerSegments;

  double roundedRectangleArcLength;
  int roundedRectangleWestArc,roundedRectangleLeftArc,roundedRectangleEastArc,roundedRectangleRightArc;

  double chamferedRectangleAngle,chamferedRectangleXsep,chamferedRectangleYsep;
  QList<int> chamferedRectangleCorners;

  double siblingAngle;

  int mindmap;

  int spyUsing;
  bool onNode;

  double initialDistance, acceptingDistance;
  double initialAngle, acceptingAngle;
  int    initialAnchor, acceptingAnchor;

  double curveXA, curveYA;
  double curveXB, curveYB;
  double curveXC, curveYC;
  double curviLinearTimeA;
  double curviLinearLengthA;
  double curviLinearLengthB;
  double curviLinearLengthC;
  double curviLinearLengthD;
  QPointF curviLinearLineA;
  QPointF curviLinearLineB;
  QPointF curviLinearLineC;
  QPointF curviLinearLineD;
  QPointF firstOnPath,secondOnPath,thirdOnPath,fourthOnPath;
  QPointF lastOnPath,secondLastOnPath,thirdLastOnPath,fourthLastOnPath;

  QVector3D planeX,planeY,planeOrigin;

  void (XWTikzState::*tikzTimer)();
  void (XWTikzState::*before_code)();
  void (XWTikzState::*after_code)();
  void (XWTikzState::*to_path)();
  void (XWTikzState::*nolinear_map)(double & x, double & y);
  void (XWTikzState::*after_node)();

  int samples;
  double domainStart;
  double domainEnd;
  QList<double> samplesAt;

  QString text,namePrefix,nameSuffix,acceptingText,initialText;

  double dashPhase;
  QVector<qreal> dashPattern;

  double pathMinX,pathMinY,pathMaxX,pathMaxY;

  QHash<QString,double> values;
  QHash<QString,QColor> colors;
  QHash<QString,XWTikzCoord*> coordsNamed;
  QHash<QString,QString> strings;

  QPointF mousePoint,lastMousePoint,pathLast;
  QPointF decorateInputSegmentFirst,decorateInputSegmentLast;
  QPointF decoratedPathFirst,decoratedPathLast;

  QPointF toStart, toTarget;
  bool isTarget;

  XWTeXBox *   myBox;
  XWTikzShape* myNode;

  XWTikzExpress  * position;
  XWTikzExpress  * startPosition;
  XWTikzExpress  * endPosition;
  XWTikzExpress  * step;

  XWTikzState * saved;

  QString markNode;

  QList<QChar> inputs;

  QList<XWTikzCoord*> coords;
  QList<int> operations;
  QList<QPointF> points;
  QList<QPointF> intersections;

  QList<QPointF> plotStream;

  QList<XWTikzShape*> nodes;
  QList<XWTikzShape*> toNodes;
  QList<XWTikzCommand*> cmds;
  QList<XWTikzCodes*> codes;
};

#endif //XWTIKZSTATE_H
