/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTIKZOPTIONS_H
#define XWTIKZOPTIONS_H

#include "XWTikzOperation.h"

class XWTikzState;
class XWTikzKey;
class XWTikzcm;
class XWTikzAround;
class XWTikzArrows;
class XWTikzColor;
class XWTikzValue;
class XWTikzDomain;
class XWTikzUnit;
class XWTikzLabel;
class XWTikzCircuitSymbol;

class XWTIKZOptions : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTIKZOptions(XWTikzGraphic * graphicA, QObject * parent = 0);
  XWTIKZOptions(XWTikzGraphic * graphicA, int idA,QObject * parent = 0);
  XWTIKZOptions(XWTikzGraphic * graphicA, 
                int idA, 
                int subk,
                QObject * parent = 0);

  bool addAction(QMenu & menu, XWTikzState * state);

  void addAnchorAction(QMenu & menu);
  void addAngleAction(QMenu & menu);
  void addArrowsAction(QMenu & menu);
  void addAtAction(QMenu & menu);
  void addCircuitAction(QMenu & menu);
  void addCircuiteeSymbolAction(QMenu & menu);
  void addCircuiteeUnitAction(QMenu & menu);
  void addCircuitLogicSymbolAction(QMenu & menu);
  void addCircuitSymbolAction(QMenu & menu);
  void addCMAction(QMenu & menu);
  void addColorAction(QMenu & menu);
  void addConceptAction(QMenu & menu);
  void addConceptColorAction(QMenu & menu);
  void addConnectSpiesAction(QMenu & menu);
  void addDecorationAction(QMenu & menu);
  void addDomainAction(QMenu & menu);
  void addDoubleAction(QMenu & menu);
  void addDoubleDistanceAction(QMenu & menu);
  void addInfoAction(QMenu & menu);
  void addInputAction(QMenu & menu);
  void addIntersectionsAction(QMenu & menu);
  void addLabelAction(QMenu & menu);
  void addLineAction(QMenu & menu);
  void addLineWidthAction(QMenu & menu);
  void addMinmapAction(QMenu & menu);
  void addOpacityAction(QMenu & menu);
  void addPathFading(QMenu & menu);
  void addPatternAction(QMenu & menu);
  void addPlotAction(QMenu & menu);
  void addPointAction(QMenu & menu);
  void addRadiusAction(QMenu & menu);
  void addRoundedCornersAction(QMenu & menu);
  void addRotateAction(QMenu & menu);
  void addScaleAction(QMenu & menu);
  void addScopeFading(QMenu & menu);
  void addShadeAction(QMenu & menu);
  void addShapeAction(QMenu & menu);
  void addShiftAction(QMenu & menu);
  void addSizeAction(QMenu & menu);
  void addSlantAction(QMenu & menu);
  void addSpyAction(QMenu & menu);
  void addStepAction(QMenu & menu);
  void addTextColorAction(QMenu & menu);
  void addTransformShapeAction(QMenu & menu);
  void addXRadiusAction(QMenu & menu);
  void addXYZAction(QMenu & menu);
  void addYRadiusAction(QMenu & menu);

  void addDomain(const QString & s,const QString & e);
  void addValueCoord(int keywordA, const QString & str);
  void addValueExpress(int keywordA, const QString & str);

  virtual bool back(XWTikzState * state);

  virtual bool del(XWTikzState * state);
  virtual void doChildAnchor(XWTikzState * state);
  virtual void doDecoration(XWTikzState * state);
  virtual void doEdgeFromParent(XWTikzState * state);
  virtual void doEdgeFromParentPath(XWTikzState * state);
  virtual void doEveryChild(XWTikzState * state);
  virtual void doEveryChildNode(XWTikzState * state);
  virtual void doEveryCircuitAnnotation(XWTikzState * state);
  virtual void doEveryCircuitSymbol(XWTikzState * state);
  virtual void doEveryConcept(XWTikzState * state);
  virtual void doEveryEdge(XWTikzState * state);
  virtual void doEveryInfo(XWTikzState * state);
  virtual void doEveryLabel(XWTikzState * state);
  virtual void doEveryMark(XWTikzState * state);
  virtual void doEveryMatrix(XWTikzState * state);
  virtual void doEveryMindmap(XWTikzState * state);
  virtual void doEveryNode(XWTikzState * state);
  virtual void doEveryPin(XWTikzState * state);
  virtual void doEveryPinEdge(XWTikzState * state);
  virtual void doEveryShape(XWTikzState * state);
  virtual void doLevel(XWTikzState * state);
  virtual void doLevelConcept(XWTikzState * state);
  virtual void doLevelNumber(XWTikzState * state);
  virtual void doParentAnchor(XWTikzState * state);
  virtual void doPath(XWTikzState * state, bool showpoint = false);
  virtual void doPre(XWTikzState * state);
  virtual void doPost(XWTikzState * state);
  virtual void doRootConcept(XWTikzState * state);
  virtual void doSpyConnection(XWTikzState * state);
  virtual void doSpyNode(XWTikzState * state);
  virtual void doToPath(XWTikzState * state);
  virtual void dragTo(XWTikzState * state);
  virtual bool dropTo(XWTikzState * state);

  XWTikzcm     * getcm();
  XWTikzAround * getAround(int keywordA);
  XWTikzArrows * getArrows();
  XWTikzColor  * getColor(int keywordA);
  virtual QString getContent();
  XWTikzDomain * getDomain();
  XWTikzKey    * getKey(int keywordA);
  virtual QPointF getPoint(XWTikzState * stateA);
  virtual QString getText();
  XWTikzValue  * getValue(int keywordA);

  virtual bool hasPost();
  virtual bool hasPre();
  virtual bool hitTest(XWTikzState * state);

  virtual void insert(int i, XWTikzOperation * opA);
          bool isMatrix();
  virtual bool isMe(const QString & nameA,XWTikzState * state);

  virtual bool keyInput(XWTikzState * state);

  virtual bool newPar(XWTikzState * state);

  virtual bool paste(XWTikzState * state);

  virtual void scan(const QString & str, int & len, int & pos);

  XWTikzOperation * takeAt(int i);

public slots:
  void addCircuit();
  void addCircuiteeIEC();
  void addCircuitLogicCDH();
  void addCircuitLogicIEC();
  void addCircuitLogicUS();
  void addConcept();
  void addConnectSpies();
  void addExtraConcept();
  void addHugeCircuitSymbols();
  void addHugeMindmap();
  void addInputInverted();
  void addInputNormal();
  void addLargeCircuitSymbols();
  void addLargeMindmap();
  void addMediumCircuitSymbols();
  void addMindmap();
  void addPointDown();
  void addPointLeft();
  void addPointRight();
  void addPointUp();
  void addSmallCircuitSymbols();
  void addSmallMindmap();
  void addSpyUsingOutlines();
  void addspyUsingOverlays();
  void addTinyCircuitSymbols();
  void addTransformShape();

  void removeInput();
  void removeTransformShape();

  void setAmpere();
  void setAmpereOppsite();
  void setAmpereSloped();
  void setAmpereSlopedOppsite();
  void setAnchor();
  void setAngle();
  void setArrowBoxTipAngle();
  void setArrowBoxHeadExtend();
  void setArrowBoxHeadIndent();
  void setArrowBoxShaftWidth();
  void setArrowBoxNorthArrow();
  void setArrowBoxSouthArrow();
  void setArrowBoxEastArrow();
  void setArrowBoxWestArrow();
  void setArrows();
  void setAt();
  void setBallColor();
  void setBottomColor();
  void setCalloutAbsolutePointer();
  void setCalloutRelativePointer();
  void setCalloutPointerArc();
  void setCalloutPointerSegments();
  void setCalloutPointerShorten();
  void setCalloutPointerWidth();
  void setChamferedRectangleAngle();
  void setChamferedRectangleSep();  
  void setCircuiteeSymbol();
  void setCircuitLogicSymbol();
  void setCircuitSymbolSize();
  void setCircuitSymbolUnit();
  void setCircularSectorAngle();
  void setCloudPuffArc();
  void setCloudPuffs();
  void setCM();
  void setColor();
  void setConceptColor();
  void setConnectSpies();
  void setCoulomb();
  void setCoulombOppsite();
  void setCoulombSloped();
  void setCoulombSlopedOppsite();
  void setCylinderEndFill();
  void setCylinderBodyFill();
  void setDartAngles();
  void setDash();
  void setDecoration();
  void setDomain();
  void setDouble();
  void setDoubleArrowTipAngle();
  void setDoubleArrowHeadExtend();
  void setDoubleArrowHeadIndent();
  void setDoubleDistance();
  void setDoubleDistanceBetweenLineCenters();
  void setDrawColor();
  void setDrawOpacity();
  void setEndAngle();
  void setFadingAngle();
  void setFarad();
  void setFaradOppsite();
  void setFaradSloped();
  void setFaradSlopedOppsite();
  void setFillColor();
  void setFillOpacity();
  void setHenry();
  void setHenryOppsite();
  void setHenrySloped();
  void setHenrySlopedOppsite();
  void setHertz();
  void setHertzOppsite();
  void setHertzSloped();
  void setHertzSlopedOppsite();
  void setInfo();
  void setInfoMissingAngle();
  void setInfoSloped();
  void setInfoSlopedMissingAngle();
  void setInnerColor();
  void setInnerSep();
  void setIsoscelesTriangleApexAngle();
  void setKiteVertexAngles();
  void setLabel();
  void setLeftColor();
  void setLineCap();
  void setLineJoin();
  void setLineWidth();
  void setLineWidthStyle();
  void setLowerLeftColor();
  void setLowerRightColor();
  void setMagneticTapeTail();
  void setMagneticTapeTailExtend();
  void setMagnification();
  void setMagnifyingGlassHandleAngleFill();
  void setMagnifyingGlassHandleAngleAspect();
  void setMiddleColor();
  void setMinimumSize();
  void setMiterLimit();
  void setName();
  void setNamePath();
  void setOf();
  void setohm();
  void setohmOppsite();
  void setohmSloped();
  void setohmSlopedOppsite();
  void setOuterColor();
  void setOuterSep();
  void setPathFading();
  void setPattern();
  void setPatternColor();
  void setPin();
  void setPlotHandler();
  void setPlotMark();
  void setPos();
  void setPost();
  void setPostlength();
  void setPre();
  void setPrelength();
  void setRadius();
  void setRaise();
  void setRandomStarburst();
  void setRegularPolygonSides();
  void setRightColor();
  void setRoundedCorners();
  void setRoundedRectangleArcLength();
  void setRotate();
  void setRotateAround();
  void setScale();
  void setScaleAround();
  void setScopeFading();
  void setSiemens();
  void setSiemensOppsite();
  void setSiemensSloped();
  void setSiemensSlopedOppsite();
  void setShade();
  void setShadeAngle();
  void setShape();
  void setShapeAspect();
  void setShapeBorderRotate();
  void setShift();
  void setSignalPointerAngle();
  void setSingleArrowTipAngle();
  void setSingleArrowHeadExtend();
  void setSingleArrowHeadIndent();
  void setSize();
  void setSortBy();
  void setSpyNodeShape();
  void setSpySize();
  void setStarburstPointHeight();
  void setStarburstPoints();
  void setStarPointHeight();
  void setStarPointRatio();
  void setStarPoints();
  void setStartAngle();
  void setStep();
  void setTapeBendHeight();
  void setTextColor();
  void setTopColor();
  void setTrapeziumAngle();
  void setUpperLeftColor();
  void setUpperRightColor();
  void setVolt();
  void setVoltOppsite();
  void setVoltSloped();
  void setVoltSlopedOppsite();
  void setVoltAmpere();
  void setVoltAmpereOppsite();
  void setVoltAmpereSloped();
  void setVoltAmpereSlopedOppsite();
  void setWatt();
  void setWattOppsite();
  void setWattSloped();
  void setWattSlopedOppsite();
  void setXRadius();
  void setXScale();
  void setXShift();
  void setXSlant();
  void setXVector();
  void setYRadius();
  void setYScale();
  void setYShift();
  void setYSlant();
  void setYVector();
  void setZVector();

protected:
  virtual void setAround(int keywordA, 
                       const QString & title,
                       const QString & ltxt);
  virtual void setColor(int keywordA, const QString & title);
  virtual void setCoord(int keywordA, const QString & title);
  
          void setDecoration(int keywordA);
          void setDecorationExpress(int keywordA,
                                    const QString & title,
                                    const QString & ltxt);
  virtual void setExpress(int keywordA, 
                       const QString & title,
                       const QString & ltxt);
  virtual void setInfo(const QString & title, int kw);
  virtual void setLabel(const QString & title, int kw);
  virtual void setTwoValue(const QString & title,
                           int k1, const QString & title1,
                           int k2,const QString & title2);
  virtual void setUnit(const QString & title, const QString & u);

protected:
  void doPathDefault(XWTikzState * state, bool showpoint = false);

  XWTikzOperation * find(int keywordA);
  XWTikzKey * findCircuitSymbols();
  XWTikzCircuitSymbol * findCircuiteeSymbol();
  XWTikzCircuitSymbol * findCircuitLogicSymbol();
  XWTikzKey * findConcept();
  XWTikzKey * findDash();
  XWTikzKey * findDecoration();
  XWTikzLabel * findInfo();
  XWTikzLabel * findLabel();
  XWTikzKey * findLineWidth();
  XWTikzKey * findMindmap();
  XWTikzKey * findPictureType();
  XWTikzKey * findPlotHandler();
  XWTikzKey * findPoint();
  XWTikzKey * findShape();
  XWTikzUnit * findUnit();

  QString getOptions();

protected:
  int cur;
  QList<XWTikzOperation*> ops;
};

class XWTikzFadingTransform : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzFadingTransform(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();

  void scan(const QString & str, int & len, int & pos);
};

class XWTikzNameIntersections : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzNameIntersections(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();

  void scan(const QString & str, int & len, int & pos);
};

class XWTikzPreactions : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzPreactions(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzPostaction : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzPostaction(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzEveryNode : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzEveryNode(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzEveryShape : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzEveryShape(XWTikzGraphic * graphicA, int k, int subk, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzEveryLabel : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzEveryLabel(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzEveryPin : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzEveryPin(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzEveryPinEdge : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzEveryPinEdge(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzPinEdge : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzPinEdge(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzEveryEdge : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzEveryEdge(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzLateOptions : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzLateOptions(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzEveryMatrix : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzEveryMatrix(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzEveryCell : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzEveryCell(XWTikzGraphic * graphicA, QObject * parent = 0);

  QString getText();

  void scan(const QString & str, int & len, int & pos);

private:
  XWTIKZOptions * row;
  XWTIKZOptions * cell;
};

class XWTikzCells : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzCells(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzNodes : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzNodes(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzEveryOddColumn : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzEveryOddColumn(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzEveryEvenColumn : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzEveryEvenColumn(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzEveryOddRow : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzEveryOddRow(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzEveryEvenRow : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzEveryEvenRow(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzEveryChild : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzEveryChild(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzEveryChildNode : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzEveryChildNode(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzLevel : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzLevel(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzLevelNumber : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzLevelNumber(XWTikzGraphic * graphicA, int k, int subk, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzEdgeFromParentStyle : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzEdgeFromParentStyle(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzEveryMark : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzEveryMark(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzMarkOptions : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzMarkOptions(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzDecorations : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzDecorations(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();

  void scan(const QString & str, int & len, int & pos);
};

class XWTikzTransform : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzTransform(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzEveryTo : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzEveryTo(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzEveryMindmap : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzEveryMindmap(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzEveryConcept : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzEveryConcept(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzRootConcept : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzRootConcept(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();

private:
  void doHuge(XWTikzState * state);
  void doLarge(XWTikzState * state);
  void doNormal(XWTikzState * state);
  void doSmall(XWTikzState * state);
};

class XWTikzLevelConcept : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzLevelConcept(XWTikzGraphic * graphicA,int id, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();

private:
  void doHuge(XWTikzState * state);
  void doLarge(XWTikzState * state);
  void doNormal(XWTikzState * state);
  void doSmall(XWTikzState * state);
};

class XWTikzEveryAnnotation : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzEveryAnnotation(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzSpyScope : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzSpyScope(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzLens : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzLens(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzEverySpyOnNode : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzEverySpyOnNode(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzEverySpyInNode : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzEverySpyInNode(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzSpyUsing : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzSpyUsing(XWTikzGraphic * graphicA, int id, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzCircuitHandleSymbol : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzCircuitHandleSymbol(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzEveryInfo : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzEveryInfo(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzEveryCircuitSymbol : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzEveryCircuitSymbol(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzEveryCircuitAnnotation : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzEveryCircuitAnnotation(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzCircuitSymbolOpen : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzCircuitSymbolOpen(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzCircuitSymbolFilled : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzCircuitSymbolFilled(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzCircuitSymbolLines : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzCircuitSymbolLines(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzCircuitSymbolWires : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzCircuitSymbolWires(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();
};

class XWTikzCircuitSymbol : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzCircuitSymbol(XWTikzGraphic * graphicA, int id, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);
  
  QString getText();
};

class XWTikzAnnotation : public XWTIKZOptions
{
  Q_OBJECT

public:
  XWTikzAnnotation(XWTikzGraphic * graphicA, int id, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);
  
  QString getText();

private:
  bool b;
};

#endif //XWTIKZOPTIONS_H
