/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include <QVector2D>
#include <math.h>
#include "XWObject.h"
#include "LaTeXKeyWord.h"
#include "PGFKeyWord.h"
#include "XWPGFPool.h"
#include "XWTeXText.h"
#include "XWPDFDriver.h"
#include "tikzcolor.h"
#include "XWTikzExpress.h"
#include "XWTikzCoord.h"
#include "XWTikzGraphic.h"
#include "XWTikzCommand.h"
#include "XWTikzExpress.h"
#include "XWTikzNode.h"
#include "XWTikzOption.h"
#include "XWTikzOptions.h"
#include "XWTikzTextBox.h"
#include "XWTikzMatrix.h"
#include "XWTikzArrow.h"
#include "XWTikzPattern.h"
#include "XWTikzShade.h"
#include "XWTikzFading.h"
#include "XWTikzDecoration.h"
#include "XWTikzPlotHandler.h"
#include "XWTikzPlotMark.h"
#include "XWTikzState.h"

#define XW_TIKZ_INTERSECTION_TOLERANCE 0.1

XWTikzState::XWTikzState(XWTikzGraphic * graphicA,XWPDFDriver * driverA,bool ispathA,QObject * parent)
 :QObject(parent),
  graphic(graphicA),
  driver(driverA),
  isPath(ispathA),
  curLabel(0)
{
  init();
  saved = 0;
}

XWTikzState::XWTikzState(bool ispathA, QObject * parent)
:QObject(parent),
  graphic(0),
  driver(0),
  isPath(ispathA),
  curLabel(0)
{
  init();
  saved = 0;
}

XWTikzState::~XWTikzState()
{
  while (!nodes.isEmpty())
    delete nodes.takeFirst();
}

void XWTikzState::acceptingByArrow()
{
  XWTikzTextBox * box = new XWTikzTextBox(graphic,this);
  int len = acceptingText.length();
  int pos = 0;
  box->scan(acceptingText,len, pos);

  XWTikzState * state = saveNode(box,XW_TIKZ_NODE);
  state->shape = PGFrectangle;
  state->anchor = acceptingAnchor;
  state->parentNode = myNode;
  state->edge_from_parent = &XWTikzState::edgeFromParentAcceptingByArrow;

  XWTikzState tmpstate;
  copy(&tmpstate,true);
  tmpstate.transformNode();
  tmpstate.anchor = -1;
  tmpstate.anotherPoint.setX(cos(acceptingAngle));
  tmpstate.anotherPoint.setX(sin(acceptingAngle));
  myNode->state = &tmpstate;
  myNode->doShape(false);
  myNode->mapPos();
  double x = acceptingDistance * cos(acceptingAngle) + myNode->anchorPos.x();
  double y = acceptingDistance * sin(acceptingAngle) + myNode->anchorPos.y();
  state->pathLast.setX(x);
  state->pathLast.setY(y);
  myNode->state = this;
}

void XWTikzState::acceptingByDouble()
{
  lineWidth = 2 * lineWidth + 0.6;
  innerLineWidth = 0.6;
  outerXSep = 0.5 * lineWidth + 0.3;
  outerYSep = 0.5 * lineWidth + 0.3;
}

void XWTikzState::addArc()
{
  double sa = startAngle;
  double ea = endAngle;
  if (deltaIsSet) 
  {
    if (startIsSet)
      ea = startAngle + deltaAngle;
    else
      sa = endAngle - deltaAngle;
  }

  addArc(sa,ea,xradius,yradius);
}

void XWTikzState::addCircle(const QPointF & c,double r)
{
  QPointF a(r,0);
  QPointF b(0,r);
  addEllipse(c,a,b);
}

void XWTikzState::addCircle()
{
  QPointF c = getLastPoint();
  QPointF a(xradius,0);
  QPointF b(0,yradius);
  addEllipse(c,a,b);
}

void XWTikzState::addCode(XWTikzCodes * c)
{
  codes << c;
}

void XWTikzState::addCosine(XWTikzCoord * p)
{
  coords << p;
  QPointF p1 = p->getPoint(this);
  addCosine(p1);
}

void XWTikzState::addCosine(const QPointF & p)
{
  double xc = p.x();
  double yc = p.y();

  QPointF c1;
  c1.setX(0.36497 * xc);
  c1.setY(0);

  QPointF c2;
  c2.setX(0.68169 * xc);
  c2.setY(0.5 * yc);
  curveTo(c1,c2,p);
}

void XWTikzState::addEdge(XWTikzCoord * p)
{
  toStart = coords.last();
  coords << p;
  toTarget = p;
  isPath = true;
  graphic->doToPath(this);
}

void XWTikzState::addEdge()
{
  toStart = coords.last();
  toTarget = toStart;
  isPath = true;
  graphic->doToPath(this);
}

void XWTikzState::addEllipse()
{
  QPointF c = coords.last()->getPoint(this);
  if (at)
    c = at->getPoint(this);

  QPointF a(xradius,0);
  QPointF b(0,yradius);
  addEllipse(c,a,b);
}

void XWTikzState::addGrid(XWTikzCoord * p)
{
  QPointF ll = coords.last()->getPoint(this);
  coords << p;
  QPointF ur = p->getPoint(this);
  addGrid(ll,ur);
}

void XWTikzState::addGrid(const QPointF & p)
{
  QPointF ll = getLastPoint();
  addGrid(ll,p);
}

void XWTikzState::addParabola(XWTikzCoord * b, XWTikzCoord * e)
{
  QPointF lp = coords.last()->getPoint(this);
  QPointF bendp;
  if (b)
  {
    coords << b;
    bendp = b->getPoint(this);
  }

  QPointF endp;
  if (e)
  {
    coords << e;
    endp = e->getPoint(this);
  }

  if (bend)
    bendp = bend->getPoint(this);

  if (bendPosIsSet)
    bendp = lineAtTime(bendPos,lp,endp) + bendp;

  if (bendHeightIsSet)
  {
    QPointF h(0,bendHeight);
    bendp = lineAtTime(0.5,lp,endp) + h;
  }

  if (bendAtStart)
    bendp = lp;

  if (bendAtEnd)
    bendp = endp;

  addParabola(bendp, endp);
}

void XWTikzState::addParabola(const QPointF & b,const QPointF & e)
{
  double xb = e.x();
  double yb = e.y();

  double xc = b.x();
  double yc = b.y();
  bool tempswa = true;
  if (xb == 0)
  {
    if (yb == 0)
      tempswa = false;
  }

  bool tempswb = true;
  if (xc == 0)
  {
    if (yc == 0)
      tempswb = false;
  }

  if (tempswa)
  {
    QPointF c1;
    c1.setX(0.1125 * xc);
    c1.setY(0.225 * yc);

    QPointF c2;
    c2.setX(0.5 * xc);
    c2.setY(yc);

    curveTo(c1,c2,b);
  }

  if (tempswb)
  {
    QPointF c1;
    c1.setX(0.5 * xb);
    c1.setY(0 * yb);

    QPointF c2;
    c2.setX(0.8875 * xb);
    c2.setY(0.775 * yb);

    curveTo(c1,c2,e);
  }
}

void XWTikzState::addParabola(const QPointF & e)
{
  QPointF lp = getLastPoint();
  QPointF bendp;
  if (bend)
    bendp = bend->getPoint(this);

  if (bendPosIsSet)
    bendp = lineAtTime(bendPos,lp,e) + e;

  if (bendHeightIsSet)
  {
    QPointF h(0,bendHeight);
    bendp = lineAtTime(0.5,lp,e) + h;
  }

  if (bendAtStart)
    bendp = lp;

  if (bendAtEnd)
    bendp = e;

  addParabola(bendp, e);
}

void XWTikzState::addPlot(XWTikzCoord * p)
{
  coords << p;
  QPointF p1 = p->getPoint(this);
  addPlotStreamPoint(p1);
}

void XWTikzState::addPlotFunction(XWTikzCoord * exp)
{
  QStringList vars = exp->getVarNames();
  if (!vars.isEmpty())
  {
    QString var = vars[0];
    if (!samplesAt.isEmpty())
    {
      for (int i = 0; i < samplesAt.size(); i++)
      {
        values[var] = samplesAt[i];
        QPointF p = exp->getPoint(this);
        addPlotStreamPoint(p);
      }
    }
    else
    {
      if (samples == 0)
        samples = 25;
      double step = (domainEnd - domainStart) / samples;
      double x = domainStart;
      for (; x < domainEnd; x += step)
      {
        values[var] = x;
        QPointF p = exp->getPoint(this);
        addPlotStreamPoint(p);
      }
    }
  }
}

void XWTikzState::addRectangle(XWTikzCoord * p)
{
  coords << p;
  QPointF p1 = p->getPoint(this);
  QPointF p0 = coords[coords.size() - 2]->getPoint(this);
  addRectangle(p0,p1);
}

void XWTikzState::addRectangle(const QPointF & p)
{
  QPointF p0 = getLastPoint();
  addRectangle(p0,p);
}

void XWTikzState::addSine(XWTikzCoord * p)
{
  coords << p;
  QPointF p1 = p->getPoint(this);
  addSine(p1);
}

void XWTikzState::addSine(const QPointF & p)
{
  double xc = p.x();
  double yc = p.y();

  QPointF c1;
  c1.setX(0.31831 * xc);
  c1.setY(0.5 * yc);

  QPointF c2;
  c2.setX(0.63503 * xc);
  c2.setY(yc);
  curveTo(c1,c2,p);
}

void XWTikzState::angleSetup(double a,
                             double dimension, 
                             double linewidthfactor,
                             double outerfactor)
{
  double xc = lineWidthDependent(dimension,linewidthfactor,outerfactor);
  double yc = 0.5 * a;
  arrowLength = xc * cos(yc);
  arrowWidth = 2 * xc * sin(yc);
}

void XWTikzState::angleSetupPrime(double a)
{
  double yc = 0.5 * a;
  double mathresult = tan(yc);
  arrowWidth = 2 * mathresult * arrowLength;
}

void XWTikzState::clearPath()
{
  operations.clear();
  points.clear();
}

void XWTikzState::clockwiseFrom(double a)
{
  double ab = a - siblingAngle * (currentChild - 1);
  double x = labelDistance * cos(ab);
  double y = labelDistance * sin(ab);
  shift(x,y);
}

void XWTikzState::closePath()
{
  operations << XW_TIKZ_CLOSE;
}

void XWTikzState::computePath()
{
  int k = 0;
  QPointF cp;
  for (int i = 0; i < operations.size(); i++)
  {
    switch (operations[i])
    {
      default:
        break;

      case XW_TIKZ_MOVETO:
        cp = points[k++];
        break;

      case XW_TIKZ_LINETO:
        cp = points[k++];
        break;

      case XW_TIKZ_CURVETO:
        {
          QPointF c1 = points[k++];
          if (c1.x() < pathMinX)
            pathMinX = c1.x();
          if (c1.x() > pathMaxX)
            pathMaxX = c1.x();
          if (c1.y() < pathMinY)
            pathMinY = c1.y();
          if (c1.y() > pathMaxY)
            pathMaxY = c1.y();

          QPointF c2 = points[k++];
          if (c2.x() < pathMinX)
            pathMinX = c2.x();
          if (c2.x() > pathMaxX)
            pathMaxX = c2.x();
          if (c2.y() < pathMinY)
            pathMinY = c2.y();
          if (c2.y() > pathMaxY)
            pathMaxY = c2.y();

          cp = points[k++];      
        }
        break;
    }

    if (cp.x() < pathMinX)
      pathMinX = cp.x();
    if (cp.x() > pathMaxX)
      pathMaxX = cp.x();
    if (cp.y() < pathMinY)
      pathMinY = cp.y();
    if (cp.y() > pathMaxY)
      pathMaxY = cp.y();
  }

  width = pathMaxX - pathMinX;
  height = pathMaxY - pathMinY;
  center.setX((pathMaxX + pathMinX) / 2);
  center.setY((pathMaxY + pathMinY) / 2);
}

void XWTikzState::computePlotFunction(XWTikzCoord * exp)
{
  QStringList vars = exp->getVarNames();
  if (!vars.isEmpty())
  {
    QString var = vars[0];
    if (!samplesAt.isEmpty())
    {
      for (int i = 0; i < samplesAt.size(); i++)
      {
        values[var] = samplesAt[i];
        QPointF p = exp->getPoint(this);
        moveTo(p);
      }
    }
    else
    {
      if (samples == 0)
        samples = 25;
      double step = (domainEnd - domainStart) / samples;
      double x = domainStart;
      for (; x < domainEnd; x += step)
      {
        values[var] = x;
        QPointF p = exp->getPoint(this);
        moveTo(p);
      }
    }
  }
}

void XWTikzState::concat(double a,double b,
                 double c,double d,
                 double dx, double dy)
{
  QTransform newtrans(a,b,c,d,dx,dy);
  transform = newtrans * transform;
}

void XWTikzState::copy(XWTikzState * newstate,bool n)
{
  newstate->graphic = graphic;
  newstate->driver = driver;
  newstate->curLabel = curLabel++;
  newstate->circuitsSizeUnit = circuitsSizeUnit;
  newstate->pictureType = pictureType;
  newstate->circuitSymbolWidth = circuitSymbolWidth;
  newstate->circuitSymbolHeight = circuitSymbolHeight;
  newstate->annotation = annotation;
  newstate->logicGateInvertedRadius = logicGateInvertedRadius;
  newstate->logicGateInputSep = logicGateInputSep;
  newstate->logicGateAnchorsUseBoundingBox = logicGateAnchorsUseBoundingBox;
  newstate->andGateIECSymbol = andGateIECSymbol;
  newstate->nandGateIECSymbol = nandGateIECSymbol;
  newstate->orGateIECSymbol = orGateIECSymbol;
  newstate->norGateIECSymbol = norGateIECSymbol;
  newstate->xorGateIECSymbol = xorGateIECSymbol;
  newstate->xnorGateIECSymbol = xnorGateIECSymbol;
  newstate->notGateIECSymbol = notGateIECSymbol;
  newstate->bufferGateIECSymbol = bufferGateIECSymbol;
  newstate->logicGateIECSymbolAlign = logicGateIECSymbolAlign;
  newstate->logicGateIECSymbolColor = logicGateIECSymbolColor;
  if (driver)
  {
    newstate->roundedCorners = roundedCorners;
    newstate->xstep = xstep;
    newstate->ystep = ystep;
    newstate->helpLines = helpLines;
    if (!n)
    {
      newstate->isDrawSet = isDrawSet;
      newstate->isFillSet = isFillSet;
      newstate->isFillText = isFillText;
      newstate->isPatternSet = isPatternSet;
      newstate->isClipSet = isClipSet;
      newstate->isShadeSet = isShadeSet;
      newstate->isDecorateSet = isDecorateSet;
      newstate->pathFading = pathFading;
    }
    
    newstate->drawColor = drawColor;
    newstate->fillColor = fillColor;
    newstate->interiorRule = interiorRule;
    newstate->patternName = patternName;
    newstate->patternColor = patternColor;
    newstate->shadingName = shadingName;
    newstate->shadingAngle = shadingAngle;
    newstate->topColor = topColor;
    newstate->bottomColor = bottomColor;
    newstate->middleColor = middleColor;
    newstate->leftColor = leftColor;
    newstate->rightColor = rightColor;
    newstate->ballColor = ballColor;
    newstate->lowerLeftColor = lowerLeftColor;
    newstate->upperLeftColor = upperLeftColor;
    newstate->lowerRightColor = lowerRightColor;
    newstate->upperRightColor = upperRightColor;
    newstate->innerColor = innerColor;
    newstate->outerColor = outerColor;
    newstate->fitFading = fitFading;
    newstate->fadingTransform = fadingTransform;
    newstate->coreColor = coreColor;
    newstate->decoration = decoration;
    newstate->pre = pre;
    newstate->prelength = prelength;
    newstate->post = post;
    newstate->postlength = postlength;
    newstate->reversePath = reversePath;
    newstate->pathHasCorners = pathHasCorners;
    newstate->decorateWidth = decorateWidth;
    newstate->decoratedAngle = decoratedAngle;
    newstate->decoratedCompletedDistance = decoratedCompletedDistance;
    newstate->decoratedRemainingDistance = decoratedRemainingDistance;
    newstate->decoratedPathLength = decoratedPathLength;
    newstate->decoratedInputSegmentLength = decoratedInputSegmentLength;
    newstate->decoratedInputSegmentCompletedDistance = decoratedInputSegmentCompletedDistance;
    newstate->decoratedInputSegmentRemainingDistance = decoratedInputSegmentRemainingDistance;
    newstate->decorationSegmentAmplitude = decorationSegmentAmplitude;
    newstate->decorationSegmentLength = decorationSegmentLength;
    newstate->decorationSegmentAngle = decorationSegmentAngle;
    newstate->decorationSegmentAspect = decorationSegmentAspect;
    newstate->metaDecorationSegmentAmplitude = metaDecorationSegmentAmplitude;
    newstate->metaDecorationSegmentLength = metaDecorationSegmentLength;
    newstate->decorateTransform = decorateTransform;
    newstate->startRadius = startRadius;
    newstate->endRadius = endRadius;
    newstate->decorationAngle = decorationAngle;
    newstate->footLength = footLength;
    newstate->strideLength = strideLength;
    newstate->footSep = footSep;
    newstate->footAngle = footAngle;
    newstate->footOf = footOf;
    newstate->textFormatDelimiters = textFormatDelimiters;
    newstate->leftIndent = leftIndent;
    newstate->rightIndent = rightIndent;
    newstate->fitToPath = fitToPath;
    newstate->fitToPathStretchingSpaces = fitToPathStretchingSpaces;
    newstate->pathFromText = pathFromText;
    newstate->pathFromTextAngle = pathFromTextAngle;
    newstate->fitTextToPath = fitTextToPath;
    newstate->scaleTextToPath = scaleTextToPath;
    newstate->reverseText = reverseText;
    newstate->groupLetters = groupLetters;
    newstate->shapeStartWidth = shapeStartWidth;
    newstate->shapeStartHeight = shapeStartHeight;
    newstate->shapeEndWidth = shapeEndWidth;
    newstate->shapeEndHeight = shapeEndHeight;
    newstate->shapeSep = shapeSep;
    newstate->betweenOrBy = betweenOrBy;
    newstate->shapeSloped = shapeSloped;
    newstate->shapeScaled = shapeScaled;
    newstate->shapeEvenlySpread = shapeEvenlySpread;
    newstate->isUseAsBoundingBoxSet = isUseAsBoundingBoxSet;
    newstate->arrowDefault = arrowDefault;
    newstate->startArrow = startArrow;
    newstate->endArrow = endArrow;
    newstate->isEnd = isEnd;
    newstate->arrowLength = arrowLength;
    newstate->arrowWidth = arrowWidth;
    newstate->arrowInset = arrowInset;
    newstate->arrowAngle = arrowAngle;
    newstate->arrowLineWidth = arrowLineWidth;
    newstate->arrowSep = arrowSep;
    newstate->shorteningDistance = shorteningDistance;
    newstate->arrowTotalLength = arrowTotalLength;
    newstate->arrowScaleLength = arrowScaleLength;
    newstate->arrowScaleWidth = arrowScaleLength;
    newstate->arrowArc = arrowArc;
    newstate->arrowSlant = arrowSlant;
    newstate->arrowReversed = arrowReversed;
    newstate->arrowHarpoon = arrowHarpoon;
    newstate->arrowSwap = arrowSwap;
    newstate->arrowDrawColor = arrowDrawColor;
    newstate->arrowFillColor = arrowFillColor;
    newstate->isArrowFillSet = isArrowFillSet;
    newstate->isArrowDrawSet = isArrowDrawSet;
    newstate->arrowLineCap = arrowLineCap;
    newstate->arrowLineJoin = arrowLineJoin;
    newstate->arrowFlex = arrowFlex;
    newstate->arrowFlexMode = arrowFlexMode;
    newstate->arrowBendMode = arrowBendMode;
    newstate->preciseShortening = preciseShortening;
    newstate->nextTip = nextTip;

    newstate->drawOpacity = drawOpacity;
    newstate->fillOpacity = fillOpacity;

    newstate->matrix = matrix;
    newstate->isMatrixAnchorSet = isMatrixAnchorSet;
    newstate->matrixAnchor = matrixAnchor;

    newstate->isContinue = isContinue;
    newstate->handler = handler;
    newstate->tension = tension;
    newstate->barWidth = barWidth;
    newstate->barShift = barShift;
    newstate->barIntervalWidth = barIntervalWidth;
    newstate->barIntervalShift = barIntervalShift;
    newstate->mark = mark;
    newstate->markRepeat = markRepeat;
    newstate->markPhase = markPhase;
    newstate->markSize = markSize;
    newstate->isMarkColorSet = isMarkColorSet; 
    newstate->markColor = markColor;
    newstate->markText = markText;
    newstate->asNode = asNode;

    newstate->samples = samples;
    newstate->domainStart = domainStart;
    newstate->domainEnd = domainEnd;
    newstate->samplesAt = samplesAt;

    newstate->dashPhase = dashPhase;
    newstate->dashPattern = dashPattern;

    newstate->aspect = aspect;
    newstate->trapeziumLeftAngle = trapeziumLeftAngle;
    newstate->trapeziumRightAngle = trapeziumRightAngle;
    newstate->trapeziumStretches = trapeziumStretches;
    newstate->trapeziumStretchesBody = trapeziumStretchesBody;

    newstate->regularPolygonSides = regularPolygonSides;

    newstate->starPoints = starPoints;
    newstate->starPointHeight = starPointHeight;
    newstate->starPointRatio = starPointRatio;
    newstate->useStarRatio = useStarRatio;

    newstate->isoscelesTriangleApexAngle = isoscelesTriangleApexAngle;
    newstate->isoscelesTriangleStretches = isoscelesTriangleStretches;

    newstate->kiteUpperVertexAngle = kiteUpperVertexAngle;
    newstate->kiteLowerVertexAngle = kiteLowerVertexAngle;

    newstate->dartTipAngle = dartTipAngle;
    newstate->dartTailAngle = dartTailAngle;

    newstate->circularSectorAngle = circularSectorAngle;

    newstate->cylinderUsesCustomFill = cylinderUsesCustomFill;
    newstate->cylinderEndFill = cylinderEndFill;
    newstate->cylinderBodyFill = cylinderBodyFill;

    newstate->cloudPuffs = cloudPuffs;
    newstate->cloudPuffArc = cloudPuffArc;
    newstate->cloudIgnoresAspect = cloudIgnoresAspect;
    newstate->cloudAnchorsUseEllipse = cloudAnchorsUseEllipse;

    newstate->MGHAfill = MGHAfill;
    newstate->MGHAaspect = MGHAaspect;
    newstate->MGHA = MGHA;

    newstate->starburstPoints = starburstPoints;
    newstate->starburstPointHeight = starburstPointHeight;
    newstate->randomStarburst = randomStarburst;

    newstate->signalPointerAngle = signalPointerAngle;
    newstate->signalFrom = signalFrom;
    newstate->signalFromOpposite = signalFromOpposite;
    newstate->signalTo = signalTo;
    newstate->signalToOpposite = signalToOpposite;

    newstate->tapeBendTop = tapeBendTop;
    newstate->tapeBendBottom = tapeBendBottom;
    newstate->tapeBendHeight = tapeBendHeight;

    newstate->magneticTapeTailExtend = magneticTapeTailExtend;
    newstate->magneticTapeTail = magneticTapeTail;

    newstate->singleArrowTipAngle = singleArrowTipAngle;
    newstate->singleArrowHeadExtend = singleArrowHeadExtend;
    newstate->singleArrowHeadIndent = singleArrowHeadIndent;

    newstate->doubleArrowTipAngle = doubleArrowTipAngle;
    newstate->doubleArrowHeadExtend = doubleArrowHeadExtend;
    newstate->doubleArrowHeadIndent = doubleArrowHeadIndent;

    newstate->arrowBoxTipAngle = arrowBoxTipAngle;
    newstate->arrowBoxHeadExtend = arrowBoxHeadExtend;
    newstate->arrowBoxHeadIndent = arrowBoxHeadIndent;
    newstate->arrowBoxShaftWidth = arrowBoxShaftWidth;
    newstate->arrowBoxNorthArrow = arrowBoxNorthArrow;
    newstate->arrowBoxSouthArrow = arrowBoxSouthArrow;
    newstate->arrowBoxEastArrow = arrowBoxEastArrow;
    newstate->arrowBoxWestArrow = arrowBoxWestArrow;

    newstate->rectangleSplitParts = rectangleSplitParts;
    newstate->rectangleSplitHorizontal = rectangleSplitHorizontal;
    newstate->rectangleSplitIgnoreEmptyParts = rectangleSplitIgnoreEmptyParts;
    newstate->rectangleSplitEmptyPartWidth = rectangleSplitEmptyPartWidth;
    newstate->rectangleSplitEmptyPartHeight = rectangleSplitEmptyPartHeight;
    newstate->rectangleSplitEmptyPartDepth = rectangleSplitEmptyPartDepth;
    newstate->rectangleSplitDrawSplits = rectangleSplitDrawSplits;
    newstate->rectangleSplitUseCustomFill = rectangleSplitUseCustomFill;

    newstate->calloutPointerAnchorAspect = calloutPointerAnchorAspect;

    newstate->calloutRelativePointer = calloutRelativePointer;

    newstate->calloutPointerShorten = calloutPointerShorten;
    newstate->calloutPointerWidth = calloutPointerWidth;
    newstate->calloutPointerArc = calloutPointerArc;
    newstate->calloutPointerStartSize = calloutPointerStartSize;
    newstate->calloutPointerEndSize = calloutPointerEndSize;
    newstate->calloutPointerSegments = calloutPointerSegments;
    newstate->isCalloutAbsolutePointer = isCalloutAbsolutePointer;

    newstate->roundedRectangleArcLength = roundedRectangleArcLength;
    newstate->roundedRectangleWestArc = roundedRectangleWestArc;
    newstate->roundedRectangleLeftArc = roundedRectangleLeftArc;
    newstate->roundedRectangleEastArc = roundedRectangleEastArc;
    newstate->roundedRectangleRightArc = roundedRectangleRightArc;

    newstate->chamferedRectangleAngle = chamferedRectangleAngle;
    newstate->chamferedRectangleXsep = chamferedRectangleXsep;
    newstate->chamferedRectangleYsep = chamferedRectangleYsep;
    newstate->chamferedRectangleCorners = chamferedRectangleCorners;

    newstate->siblingAngle = siblingAngle;

    newstate->fromColor = fromColor;
    newstate->toColor = toColor;

    newstate->acceptingText = acceptingText;
    newstate->initialText = initialText;
    newstate->initialDistance = initialDistance;
    newstate->acceptingDistance = acceptingDistance;
    newstate->initialAngle = initialAngle;
    newstate->acceptingAngle = acceptingAngle;
    newstate->initialAnchor = initialAnchor;
    newstate->acceptingAnchor = acceptingAnchor;
    newstate->shadowScale = shadowScale;
    newstate->shadowXShift = shadowXShift;
    newstate->shadowYShift = shadowYShift;
    newstate->toIn = toIn;
    newstate->toOut = toOut;
    newstate->inLooseness = inLooseness;
    newstate->outLooseness = outLooseness;
    newstate->inMin = inMin;
    newstate->inMax = inMax;
    newstate->outMin = outMin;
    newstate->outMax = outMax;
    newstate->toBend = toBend;
    newstate->relative = relative;
    newstate->inControl = inControl;
    newstate->outControl = outControl;
    newstate->isInControlSet = isInControlSet;
    newstate->isOutControlSet = isOutControlSet;
    newstate->tokenDistance = tokenDistance;
  }

  newstate->lineWidth = lineWidth;
  newstate->innerLineWidth = innerLineWidth;

  newstate->anchor = anchor;
  newstate->angle = angle;
  newstate->textWidth = textWidth;
  newstate->align = align;
  newstate->shape = shape;
  newstate->fontSize = fontSize;
  newstate->innerXSep = innerXSep;
  newstate->innerYSep = innerYSep;
  newstate->outerXSep = outerXSep;
  newstate->outerYSep = outerYSep;
  newstate->minHeight = minHeight;
  newstate->minWidth = minWidth;
  newstate->shapeAspect = shapeAspect;
  newstate->shapeBorderUsesIncircle = shapeBorderUsesIncircle;
  newstate->shapeBorderRotate = shapeBorderRotate;
  newstate->above = above;
  newstate->below = below;
  newstate->left = left;
  newstate->right = right;
  newstate->nodeTime = nodeTime;
  newstate->isTimeSet = isTimeSet;
  newstate->autoAnchor = autoAnchor;
  newstate->isAutoSet = isAutoSet;
  newstate->isSwapSet = isSwapSet;
  newstate->isSlopedSet = isSlopedSet;
  newstate->isAllowUpsideDown = isAllowUpsideDown;
  newstate->location = location;

  newstate->labelAngle = labelAngle;
  newstate->labelDistance = labelDistance;
  newstate->absolute = absolute;

  newstate->columnSep = columnSep;
  newstate->rowSep = rowSep;
  newstate->curColumn = curColumn;
  newstate->curRow = curRow;

  newstate->level = level;
  newstate->levelDistance = levelDistance;
  newstate->siblingDistance = siblingDistance;
  newstate->growAngle = growAngle;
  newstate->missing = missing;
  newstate->growthParentAnchor = growthParentAnchor;
  newstate->isChildAnchorSet = isChildAnchorSet;
  newstate->isParentAnchorSet = isParentAnchorSet;
  newstate->childAnchor = childAnchor;
  newstate->parentAnchor = parentAnchor;
  newstate->isTransformChildSet = isTransformChildSet;

  newstate->xradius = xradius;
  newstate->yradius = yradius;
  newstate->at = at;

  newstate->endIsSet = endIsSet;
  newstate->startIsSet = startIsSet;
  newstate->deltaIsSet = deltaIsSet;
  newstate->startAngle = startAngle;
  newstate->endAngle = endAngle;
  newstate->deltaAngle = deltaAngle;

  newstate->bend = bend;
  newstate->bendPosIsSet = bendPosIsSet;
  newstate->bendPos = bendPos;
  newstate->bendHeightIsSet = bendHeightIsSet;
  newstate->bendHeight = bendHeight;
  newstate->bendAtStart = bendAtStart;
  newstate->bendAtEnd = bendAtEnd;
  newstate->xVec = xVec;
  newstate->yVec = yVec;
  newstate->zVec = zVec;

  newstate->text = text;
  newstate->namePrefix = namePrefix;
  newstate->nameSuffix = nameSuffix;

  newstate->values = values;
  newstate->colors = colors;
  newstate->coordsNamed = coordsNamed;
  newstate->strings = strings;

  newstate->mousePoint = mousePoint;
  newstate->lastMousePoint = lastMousePoint;
  newstate->pathLast = pathLast;

  newstate->pathMinX = pathMinX;
  newstate->pathMinY = pathMinY;
  newstate->pathMaxX = pathMaxX;
  newstate->pathMaxY = pathMaxY;

  newstate->mindmap = mindmap;

  newstate->tikzTimer = tikzTimer;

  newstate->transform = transform;
  newstate->transformShape = transformShape;

  newstate->firstOnPath = firstOnPath;
  newstate->secondOnPath = secondOnPath;
  newstate->thirdOnPath = thirdOnPath;
  newstate->fourthOnPath = fourthOnPath;
  newstate->lastOnPath = lastOnPath;
  newstate->secondLastOnPath = secondLastOnPath;
  newstate->thirdLastOnPath = thirdLastOnPath;
  newstate->fourthLastOnPath = fourthLastOnPath;

  newstate->toStart = toStart;
  newstate->toTarget = toTarget;

  newstate->position = position;
  newstate->startPosition = startPosition;
  newstate->endPosition = endPosition;
  newstate->step = step;

  newstate->planeX = planeX;
  newstate->planeY = planeY;
  newstate->planeOrigin = planeOrigin;

  newstate->markNode = markNode;

  newstate->to_path = to_path;
  newstate->edge_from_parent = edge_from_parent;

  newstate->coords = coords;
}

void XWTikzState::counterClockwiseFrom(double a)
{
  double ab = a + siblingAngle * (currentChild - 1);
  double x = labelDistance * cos(ab);
  double y = labelDistance * sin(ab);
  transform.translate(x,y);
}

void XWTikzState::curveTo(double xA,double yA, double xB,double yB, double xC,double yC)
{
  QPointF p1(xA,yA);
  QPointF p2(xB,yB);
  QPointF p3(xC,yC);

  curveTo(p1,p2,p3);
}

void XWTikzState::curveTo(XWTikzCoord * c1,XWTikzCoord * c2,XWTikzCoord * endpoint)
{
  if (coords.last()->isAtShapeBorder(this))
  {
    QPointF ap = c1->getAnchor(PGFcenter,this);
    QPointF mp = coords.last()->getBorder(ap,this);
    mp = map(mp);
    points << mp;
  }
  
  coords << c1;
  QPointF p1, p2, p3;
  if (c1->isAtShapeBorder(this))
  {
    QPointF ap = points.last();
    p1 = c1->getBorder(ap,this);
  }
  else
    p1 = c1->getPoint(this);

  coords << endpoint;
  if (c2->isAtShapeBorder(this))
    p2 = c2->getBorder(p1,this);
  else
    p2 = c2->getPoint(this);

  if (endpoint->isAtShapeBorder(this))
    p3 = endpoint->getBorder(p2,this);
  else
  {
    coords.removeLast();
    p3 = endpoint->getPoint(this);
    coords << endpoint;
  }

  curveTo(p1,p2,p3);
  tikzTimer = &XWTikzState::timerCurver;
}

void XWTikzState::curveTo(XWTikzCoord * c1,XWTikzCoord * endpoint)
{
  if (coords.last()->isAtShapeBorder(this))
  {
    QPointF ap = c1->getAnchor(PGFcenter,this);
    QPointF mp = coords.last()->getBorder(ap,this);
    mp = map(mp);
    points << mp;
  }
  
  coords << c1;
  QPointF p1;
  if (c1->isAtShapeBorder(this))
  {
    QPointF ap = points.last();
    p1 = c1->getBorder(ap,this);
  }
  else
    p1 = c1->getPoint(this);

  QPointF p3;
  if (endpoint->isAtShapeBorder(this))
    p3 = endpoint->getBorder(p1,this);
  else
    p3 = endpoint->getPoint(this);
  coords << endpoint;
    
  curveTo(p1,p1,p3);
}

void XWTikzState::curveTo(const QPointF & c1,const QPointF & c2,const QPointF & endpoint)
{
  fourthLastOnPath = pathLast;
  QPointF p1 = map(c1);
  QPointF p2 = map(c2);
  QPointF p3 = map(endpoint);

  operations << XW_TIKZ_CURVETO;
  if (points.size() < 4)
  {
    secondOnPath = p1;
    thirdOnPath = p2;
    fourthOnPath = p3;
  }

  points << p1;
  points << p2;
  points << p3;
  pathLast = p3;

  thirdLastOnPath = p1;
  secondLastOnPath = p2;
  lastOnPath = pathLast;
}

void XWTikzState::cycle()
{
  closePath();
}

QPointF XWTikzState::doAnchor(XWTeXBox * box)
{
  transformNode();
  if (anchor <= 0)
  {
    anotherPoint.setX(cos(angle));
    anotherPoint.setY(sin(angle));
  }
  XWTikzShape node(driver,box,this,XW_TIKZ_NODE);
  node.doShape(false);
  node.mapPos();
  return node.anchorPos;
}

QPointF XWTikzState::doBorder(XWTeXBox * box, const QPointF & p)
{
  transformNode();
  QTransform itrans = transform.inverted();
  anotherPoint = itrans.map(p);
  XWTikzShape node(driver,box,this,XW_TIKZ_NODE);
  node.doShape(false);
  node.mapPos();
  return node.anchorPos;
}

void XWTikzState::doEdgeFromParentForkDown()
{
  edge_from_parent = &XWTikzState::edgeFromParentForkDown;
}

void XWTikzState::doEdgeFromParentForkLeft()
{
  edge_from_parent = &XWTikzState::edgeFromParentForkLeft;
}

void XWTikzState::doEdgeFromParentForkRight()
{
  edge_from_parent = &XWTikzState::edgeFromParentForkRight;
}

void XWTikzState::doEdgeFromParentForkUp()
{
  edge_from_parent = &XWTikzState::edgeFromParentForkUp;
}

void XWTikzState::doEdgeFromParent()
{
  if (edge_from_parent != NULL)
    (this->*(edge_from_parent))();
}

void XWTikzState::doNodeCompute(XWTeXBox * box)
{
  transformNode();
  XWTikzShape node(driver,box,this,XW_TIKZ_NODE);
  node.doShape(false);  
  node.mapPos();
  if (pathMinX > node.westPos.x())
    pathMinX = node.westPos.x();

  if (pathMinY > node.southPos.y())
    pathMinY = node.southPos.y();

  if (pathMaxX < node.eastPos.x())
    pathMaxX = node.eastPos.x();

  if (pathMaxY < node.northPos.y())
    pathMaxY = node.northPos.y();

  width = pathMaxX - pathMinX;
  height = pathMaxY - pathMinY;
  center.setX((pathMaxX + pathMinX) / 2);
  center.setY((pathMaxY + pathMinY) / 2);
}

double XWTikzState::doRadius(XWTeXBox * box)
{
  transformNode();
  XWTikzShape node(driver,box,this,XW_TIKZ_NODE);
  node.doShape(false);
  return node.centerPos.x() - node.westPos.x();
}

void XWTikzState::doToPath()
{
  if (to_path != NULL)
    (this->*(to_path))();
}

void XWTikzState::dragTo(XWTeXBox * box)
{
  if (box)
  {
    QPointF mp = getLastMousePoint();
    box->dragTo(driver,mp.x(),mp.y());
  }
}

void XWTikzState::drawArrow(int a)
{
  int n = operations.size();
  if (n < 2)
    return ;

  int oldbm = arrowBendMode;
  bool oldr = arrowReversed;

  XWTikzArrow arrow(a);
  arrow.setup(this);
  computeShortening(&arrow);

  XWTikzState * state = save(false);
  if (isArrowDrawSet)
    state->setDrawColor(arrowDrawColor);

  if (isArrowFillSet)
    state->setFillColor(arrowFillColor);

  state->transform.reset();

  if (isEnd)
  {
    if (operations[n-1] == XW_TIKZ_CURVETO && preciseShortening)
      state->transformArrowCurved(&arrow,lastOnPath,secondLastOnPath,thirdLastOnPath,fourthLastOnPath);
    else
      state->transformArrowStraight(&arrow,secondLastOnPath,lastOnPath);
  }
  else
  {
    if (operations[1] == XW_TIKZ_CURVETO && preciseShortening)
      state->transformArrowCurved(&arrow,firstOnPath,secondOnPath,thirdOnPath,fourthOnPath);
    else
      state->transformArrowStraight(&arrow,secondOnPath,firstOnPath);
  }

  state->shift(nextTip,0);
  state->slant(arrowSlant,arrowSlant);
  if(arrowReversed)
    state->scale(-1,1);  
  if (arrowSwap)
    state->scale(1,-1);

  arrow.draw(state);

  restore();

  nextTip = arrow.tipEnd + arrowSep - arrow.backEnd;
  arrowBendMode = oldbm;
  arrowReversed = oldr;
}

void XWTikzState::flush()
{
  computePath();
  
  if (driver && !operations.isEmpty() && !points.isEmpty())
  {
    if (isUseAsBoundingBoxSet)
    {
      computePath();

      operations.clear();
      points.clear();
      transform.reset();
      moveTo(pathMinX,pathMinY);
      lineTo(pathMinX,pathMaxY);
      lineTo(pathMaxX,pathMaxY);
      lineTo(pathMaxX,pathMinY);
      closePath();

      constructPath();

      switch (interiorRule)
      {
        case PGFnonzerorule:
          driver->nonzeroClip();
          break;

        default:
          driver->evenoddClip();
          break;
      }

      return ;
    }

    QList<XWTikzShape*> behindnodes;
    for (int i = 0; i < nodes.size(); i++)
    {
      if (nodes[i]->state->location == PGFbehindpath)
      {
        XWTikzShape * n = nodes.takeAt(i);
        behindnodes << n;
      }
    }

    if (behindnodes.size() > 0)
    {
      QList<XWTikzShape*> oldnodes = nodes;
      nodes = behindnodes;
      doNodes();
      nodes = oldnodes;
    }

    initPath();
    invokeDecorate();
    constructPath();

    if (pathFading > -1)
    {
      if (fitFading)
      {
        double xb = 0.5 * (pathMinX + pathMaxX);
        double yb = 0.5 * (pathMinY + pathMaxY);
        double xc = 0.2 * (pathMaxX - pathMinX);
        double yc = 0.2 * (pathMaxY - pathMinY);
        QTransform trans(xc,0,0,yc,xb,yb);
        XWTikzFading fading(driver,pathFading,trans);
        fading.doFading(this);
      }
      else
      {
        XWTikzFading fading(driver,pathFading,fadingTransform);
        fading.doFading(this);
      }
    }

    if (isShadeSet)
    {
      XWTikzShade shad(driver,shadingName,shadingAngle);
      shad.doShading(this);
    }

    XWObject extgs;
    driver->initExtGState(drawOpacity,fillOpacity,&extgs);
    QString extgsname = QString("extgs%1"),arg(curLabel);
    driver->setExtGState(extgsname,&extgs);
    driver->useExtGState(extgsname);

    if (isPatternSet)
    {
      XWTikzPattern pat(driver,patternName);
      pat.doPattern(this);
    }

    switch (interiorRule)
    {
      case PGFnonzerorule:
        if (isClipSet)
          driver->nonzeroClip();
        if (isFillSet)
        {
          driver->setFillColor(fillColor);
          driver->nonzeroFill();
        }          
        break;

      default:
        if (isClipSet)
          driver->evenoddClip();
        if (isFillSet)
        {
          driver->setFillColor(fillColor);
          driver->evenoddFill();
        }          
        break;
    }

    if (!dashPattern.isEmpty())
      driver->setLineDash(dashPattern,dashPhase);

    if (isDrawSet)
    {
      driver->setStrokeColor(drawColor);
      driver->stroke();
      if (innerLineWidth > 0)
      {
        driver->gsave();
        driver->setStrokeColor(coreColor);
        driver->setLineWidth(innerLineWidth);
        driver->stroke();
        driver->grestore();
      }
    }

    if (mark > 0 && plotStream.size() > 0)
    {
      XWTikzPlotMark plotmark(mark);
      XWTikzState state(graphic,driver,false);
      state.copy(this);
      state.transform.reset();
      state.isDrawSet = false;
      state.isFillSet = false;
      state.isFillText = false;
      state.isClipSet = false;
      state.isClipSet = false;
      state.isPatternSet = false;
      state.isShadeSet = false;
      state.mark = 0;
      state.handler = 0;
      for (int i = 0; i < plotStream.size(); i++)
      {
        QPointF p = transform.map(plotStream[i]);
        state.shift(p.x(),p.y());
        plotmark.doMark(&state);
        state.flush();
      }
    }

    if (isPath)
    {
      isDrawSet = false;
      isFillSet = false;
      isFillText = false;
      isClipSet = false;
      isPatternSet = false;
      isShadeSet = false;
      isDecorateSet = false;
      shorteningDistance = shortenStart;
      arrowTotalLength = 0;
      nextTip = 0;
      if (startArrow)
      {
        isEnd = false;
        startArrow->doPath(this);
      }
      
      isEnd = true;
      shorteningDistance = shortenEnd;
      arrowTotalLength = 0;
      nextTip = 0;
      if (endArrow)
        endArrow->doPath(this);
    }

    operations.clear();
    points.clear();
    plotStream.clear();

    doNodes();
  }
  else
  {
    operations.clear();
    points.clear();
    plotStream.clear();
  }

  dashPattern.clear();
}

void XWTikzState::generalShadow()
{
  transformShape = true;
  QPointF c = graphic->getPathBoundboxCenter();
  transform.translate(c.x(), c.y());
  transform.scale(shadowScale, shadowScale);
  transform.translate(-c.x(), -c.y());
  transform.translate(shadowXShift, shadowYShift);
}

double XWTikzState::getArrowTotalLength(int a)
{
  XWTikzArrow arrow(a);
  arrow.setup(this);
  computeShortening(&arrow);
  return arrowTotalLength;
}

QPointF XWTikzState::getChildAnchor(int a)
{
  QPointF ret;
  if (myNode)
  {
    anchor = a;
    anchorChild();
    if (anchor <= 0)
    {
      if (parentNode)
      {
        parentNode->state->anchor = PGFcenter;
        parentNode->doShape(false);
        parentNode->mapPos();
        QTransform trans = transform.inverted();
        anotherPoint = trans.map(parentNode->anchorPos);
        anchor = -1;
      }
    }
    myNode->doShape(false);
    myNode->mapPos();
    ret = myNode->anchorPos;
  }
  return ret;
}

QColor XWTikzState::getColor(const QString & nameA)
{
  if (colors.contains(nameA))
    return colors[nameA];

  return QColor();
}

XWTikzCoord * XWTikzState::getCoord(const QString & nameA)
{
  if (coordsNamed.contains(nameA))
    return coordsNamed[nameA];

  return 0;
}

XWTikzCoord * XWTikzState::getCurrentCoord()
{
  return coords.last();
}

QPointF XWTikzState::getCurrentPoint()
{
  return coords.last()->getPoint(this);
}

QPointF XWTikzState::getDecoratedPathFirst()
{
  QTransform itrans = transform.inverted();
  return itrans.map(decoratedPathFirst);
}

QPointF XWTikzState::getDecoratedPathLast()
{
  QTransform itrans = transform.inverted();
  return itrans.map(decoratedPathLast);
}

QPointF XWTikzState::getDecorateInputSegmentFirst()
{
  QTransform itrans = transform.inverted();
  return itrans.map(decorateInputSegmentFirst);
}

QPointF XWTikzState::getDecorateInputSegmentLast()
{
  QTransform itrans = transform.inverted();
  return itrans.map(decorateInputSegmentLast);
}

QPointF XWTikzState::getFirstPoint()
{
  if (points.size() == 0)
    return QPointF();

  QTransform itrans = transform.inverted();
  return itrans.map(points[0]);
}

XWTikzCoord * XWTikzState::getInitialCoord()
{
  if (coords.size() > 0)
    return coords[0];

  return 0;
}

QList<QPointF> XWTikzState::getIntersections(const QList<int> & operationsA,
                                  const QList<QPointF> & pointsA)
{
  QPointF p1;
  int k = 0;
  for (int i = 0; i < operations.size(); i++)
  {
    switch (operations[i])
    {
      default:
        break;

      case XW_TIKZ_MOVETO:
        p1 = points[k++];
        break;

      case XW_TIKZ_LINETO:
        {
          QPointF p2 = points[k++];
          int m = 0;
          QPointF p3;
          for (int j = 0; j < operationsA.size(); j++)
          {
            switch (operationsA[j])
            {
              default:
                break;

              case XW_TIKZ_MOVETO:
                p3 = pointsA[m++];
                break;

              case XW_TIKZ_LINETO:
                {
                  QPointF p4 = pointsA[m++];
                  if (isLinesIntersect(p1,p2,p3,p4)) 
                  {
                    QPointF p = intersectionOfLines(p1,p2,p3,p4);
                    intersections << p;
                  }
                  p3 = p4;
                }
                break;

              case XW_TIKZ_CURVETO:
                {
                  QPointF p4 = pointsA[m++];
                  QPointF p5 = pointsA[m++];
                  QPointF p6 = pointsA[m++];
                  intersectionOfLineAndCurve(p1,p2,p3,p4,p5,p6);
                  p3 = p6;
                }
                break;
            }
          }
          p1 = p2;
        }
        break;

      case XW_TIKZ_CURVETO:
        {
          QPointF p2 = points[k++];
          QPointF p3 = points[k++];
          QPointF p4 = points[k++];
          int m = 0;
          QPointF p5;
          for (int j = 0; j < operationsA.size(); j++)
          {
            switch (operationsA[j])
            {
              default:
                break;

              case XW_TIKZ_MOVETO:
                p5 = pointsA[m++];
                break;

              case XW_TIKZ_LINETO:
                {
                  QPointF p6 = pointsA[m++];
                  intersectionOfCurveAndLine(p1,p2,p3,p4,p5,p6);
                  p5 = p6;
                }
                break;

              case XW_TIKZ_CURVETO:
                {
                  QPointF p6 = pointsA[m++];
                  QPointF p7 = pointsA[m++];
                  QPointF p8 = pointsA[m++];
                  intersectionsOfCurvers(p1,p2,p3,p4,p5,p6,p7,p8);
                  p5 = p8;
                }
                break;
            }
          }
          p1 = p4;
        }
        break;
    }
  }
  return intersections;
}

QPointF XWTikzState::getLastPoint()
{
  QTransform itrans = transform.inverted();
  return itrans.map(pathLast);
}

QPointF XWTikzState::getLastMousePoint()
{
  QTransform itrans = transform.inverted();
  return itrans.map(lastMousePoint);
}

QPointF XWTikzState::getMousePoint()
{
  QTransform itrans = transform.inverted();
  return itrans.map(mousePoint);
}

QPointF XWTikzState::getParentAnchor(int a)
{
  QPointF ret;
  if (parentNode)
  {
    parentNode->state->anchor = a;
    anchorParent();
    if (parentNode->state->anchor <= 0)
    {
      if (myNode)
      {
        myNode->doShape(false);
        myNode->mapPos();
        QTransform trans = parentNode->state->transform.inverted();
        parentNode->state->anotherPoint = trans.map(myNode->anchorPos);
        parentNode->state->anchor = -1;
      }
    }
    parentNode->doShape(false);
    parentNode->mapPos();
    ret = parentNode->anchorPos;
  }
  return ret;
}

void XWTikzState::getPath(QList<int> & operationsA,QList<QPointF> & pointsA)
{
  operationsA = operations;
  pointsA = points;
}

XWTikzCoord * XWTikzState::getPreviousCoord(XWTikzCoord * p)
{
  int i = coords.indexOf(p);
  if (i > 0)
    return coords[i-1];
  return 0;
}

QString XWTikzState::getString(const QString & nameA)
{
  if (strings.contains(nameA))
    return strings[nameA];

  return QString();
}

QPointF XWTikzState::getToStart()
{
  QPointF ret;
  if (toStart)
  {
    if (toStart->isAtShapeBorder(this))
    {
      QPointF ap;
      if (toTarget)
        ap = toTarget->getAnchor(PGFcenter, this);
      ret = toStart->getBorder(ap,this);
    }
    else
      ret = toStart->getPoint(this);
  }
  return ret;
}

QPointF XWTikzState::getToTarget()
{
  QPointF ret;
  if (toTarget)
  {
    if (toTarget->isAtShapeBorder(this))
    {
      QPointF ap;
      if (toStart)
        ap = toStart->getAnchor(PGFcenter, this);
      ret = toTarget->getBorder(ap,this);
    }
    else
      ret = toTarget->getPoint(this);
  }
  return ret;
}

double XWTikzState::getValue(const QString & nameA)
{
  if (values.contains(nameA))
    return values[nameA];

  return 1.0;
}

void XWTikzState::growCyclic()
{
  double a = siblingAngle * (-0.5 - 0.5 * childrenNumber + currentChild);
  transform.rotate(a);
  double x = levelDistance;
  double y = transform.dy();
  transform.translate(x,y);
  isTransformChildSet = true;
}

void XWTikzState::growViaThreePoints(const QPointF & p1, 
                                     const QPointF & p2, 
                                     const QPointF & p3)
{
  double xa = p1.x();
  double ya = p1.y();
  double xb = p2.x() - xa;
  double yb = p2.x() - ya;
  double xc = p3.x() - xa - xb;
  double yc = p3.y() - ya - yb;
  childrenNumber--;
  xb = childrenNumber * xb;
  yb = childrenNumber * yb;
  currentChild--;
  xc = currentChild * xc;
  yc = currentChild * yc;
  xa += xb;
  xa += xc;
  ya += yb;
  ya += yc;
  shift(xa, ya);
  isTransformChildSet = true;
}

bool XWTikzState::hitTestArc()
{
  points.clear();
  QPointF p = coords.last()->getPoint(this);
  pathLast = map(p);
  points << pathLast;
  double sa = startAngle;
  double ea = endAngle;
  if (deltaIsSet) 
  {
    if (startIsSet)
      ea = startAngle + deltaAngle;
    else
      sa = endAngle - deltaAngle;
  }

  addArc(sa,ea,xradius,yradius);

  return hitTestLines();
}

bool XWTikzState::hitTestChild()
{
  anchorChild();
  transformChild();
  shape = PGFcoordinate;
  XWTikzShape node(0,0,this,XW_TIKZ_CHILD);
  node.doShape(false);
  node.mapPos();
  pathLast = node.anchorPos;
  return hitTestPoint(node.anchorPos);
}

bool XWTikzState::hitTestCoordinate(XWTeXBox * box)
{
  anchorNode();
  transformNode();
  shape = PGFcoordinate;
  XWTikzShape node(0,box,this,XW_TIKZ_NODE);
  node.doShape(false);
  node.mapPos();
  pathLast = node.anchorPos;
  if (mousePoint.x() >= node.westPos.x() && 
      mousePoint.x() <= node.eastPos.x() && 
      mousePoint.y() >= node.northPos.y() &&
      mousePoint.y() <= node.southPos.y())
  {
    return true;
  }

  return false;
}

bool XWTikzState::hitTestCosine(XWTikzCoord * p)
{
  points.clear();
  coords << p;
  QPointF pp = coords.last()->getPoint(this);
  pathLast = map(pp);
  points << pathLast;
  addCosine(pp);
  return hitTestLines();
}

bool XWTikzState::hitTestCurve(XWTikzCoord * c1,XWTikzCoord * c2)
{
  points.clear();
  XWTikzCoord * endpoint = coords.last();
  removeLastCoord();
  curveTo(c1,c2,endpoint);
  return hitTestLines();
}

bool XWTikzState::hitTestCycle()
{
  tikzTimer = &XWTikzState::timerLine;
  QPointF p1 = coords[0]->getPoint(this);
  QPointF p2 = coords.last()->getPoint(this);
  moveTo(p1);
  lineTo(p2);
  return hitTestLine(p1,p2);
}

bool XWTikzState::hitTestEllipse()
{
  QPointF c = coords.last()->getPoint(this);
  QTransform itrans = transform.inverted();
  QPointF p = itrans.map(mousePoint);
  double y = yradius * sqrt(1 - (p.x()-c.x()) * (p.x()-c.x()) / (xradius*xradius));
  double y0 = p.y();

  p.setY(y + y0);
  if (hitTestPoint(p))
    return true;

  p.setY(-y+y0);
  return hitTestPoint(p);
}

bool XWTikzState::hitTestFunction(XWTikzCoord * exp)
{
  QStringList vars = exp->getVarNames();
  if (!vars.isEmpty())
  {
    QString var = vars[0];
    QTransform itrans = transform.inverted();
    QPointF p = itrans.map(mousePoint);
    values[var] = p.x();
    QPointF r =  exp->getPoint(this);
    return hitTestPoint(r);
  }

  return false;
}

bool XWTikzState::hitTestGrid()
{
  int n = coords.size();
  QPointF p1 = coords[n-2]->getPoint(this);
  QPointF p3 = coords[n-1]->getPoint(this);

  QRectF rect;
  rect.setBottomLeft(p1);
  rect.setTopRight(p3);

  rect = transform.mapRect(rect);
  return rect.contains(mousePoint);
}

bool XWTikzState::hitTestHVLine()
{
  tikzTimer = &XWTikzState::timerHVLine;
  int n = coords.size();
  QPointF p1 = coords[n-2]->getPoint(this);
  QPointF p3 = coords[n-1]->getPoint(this);
  QPointF p2;
  p2.setX(p3.x());
  p2.setY(p1.y());

  moveTo(p1);
  moveTo(p3);

  if (hitTestLine(p1,p2))
    return true;

  return hitTestLine(p2,p3);
}

bool XWTikzState::hitTestLine()
{
  tikzTimer = &XWTikzState::timerLine;
  int n = coords.size();
  QPointF p1 = coords[n-2]->getPoint(this);
  QPointF p2 = coords[n-1]->getPoint(this);
  moveTo(p1);
  lineTo(p2);
  return hitTestLine(p1,p2);
}

bool XWTikzState::hitTestMatrix(XWTikzMatrix * box)
{
  transformNode();
  if (!box)
    return false;
  box->setState(this);
  QPointF p = getMousePoint();
  if (box->hitTest(p.x(),p.y()))
    return true;

  XWTikzShape node(driver,box,this,XW_TIKZ_NODE);
  node.doShape(false);
  node.mapPos();
  if (mousePoint.x() >= node.westPos.x() && 
      mousePoint.x() <= node.eastPos.x() && 
      mousePoint.y() >= node.northPos.y() &&
      mousePoint.y() <= node.southPos.y())
  {
    return true;
  }

  return false;
}

bool XWTikzState::hitTestParabola(XWTikzCoord * b, XWTikzCoord * e)
{
  points.clear();
  QPointF p = coords.last()->getPoint(this);
  pathLast = map(p);
  points << pathLast;
  addParabola(b,e);
  return hitTestLines();
}

bool XWTikzState::hitTestPoint(XWTikzCoord * p)
{
  coords << p;
  QPointF p1 = p->getPoint(this);
  return hitTestPoint(p1);
}

bool XWTikzState::hitTestRectangle()
{
  int n = coords.size();
  QPointF p1 = coords[n-2]->getPoint(this);
  QPointF p3 = coords[n-1]->getPoint(this);

  QPointF p2,p4;
  p2.setX(p3.x());
  p2.setY(p1.y());
  p4.setX(p1.x());
  p4.setY(p3.y());

  if (hitTestLine(p1,p2))
    return true;

  if (hitTestLine(p2,p3))
    return true;

  if (hitTestLine(p3,p4))
    return true;

  return hitTestLine(p1,p4);
}

bool XWTikzState::hitTestSine(XWTikzCoord * p)
{
  points.clear();
  coords << p;
  QPointF pp = coords.last()->getPoint(this);
  pathLast = map(pp);
  points << pathLast;
  addSine(pp);
  return hitTestLines();
}

bool XWTikzState::hitTestText(XWTikzTextBox * box)
{
  transformNode();
  if (!box)
    return false;
  box->setState(this);
  QPointF p = getMousePoint();
  if (box->hitTest(p.x(),p.y()))
    return true;

  XWTikzShape node(driver,box,this,XW_TIKZ_NODE);
  node.doShape(false);
  node.mapPos();
  if (mousePoint.x() >= node.westPos.x() && 
      mousePoint.x() <= node.eastPos.x() && 
      mousePoint.y() >= node.northPos.y() &&
      mousePoint.y() <= node.southPos.y())
  {
    return true;
  }

  return false;
}

bool XWTikzState::hitTestVHLine()
{
  tikzTimer = &XWTikzState::timerVHLine;
  int n = coords.size();
  QPointF p1 = coords[n-2]->getPoint(this);
  QPointF p3 = coords[n-1]->getPoint(this);
  QPointF p2;
  p2.setX(p1.x());
  p2.setY(p3.y());

  moveTo(p1);
  moveTo(p3);

  if (hitTestLine(p1,p2))
    return true;

  return hitTestLine(p2,p3);
}

void XWTikzState::initialByArrow()
{
  XWTikzTextBox * box = new XWTikzTextBox(graphic,this);
  int len = initialText.length();
  int pos = 0;
  box->scan(initialText,len, pos);
  XWTikzState * state = saveNode(box,XW_TIKZ_NODE);
  state->shape = PGFrectangle;
  state->anchor = initialAnchor;
  state->parentNode = myNode;
  state->edge_from_parent = &XWTikzState::edgeFromParentInitialByArrow;

  XWTikzState tmpstate;
  copy(&tmpstate,true);
  tmpstate.transformNode();
  tmpstate.anchor = -1;
  tmpstate.anotherPoint.setX(cos(initialAngle));
  tmpstate.anotherPoint.setX(sin(initialAngle));
  myNode->state = &tmpstate;
  myNode->doShape(false);
  myNode->mapPos();
  double x = initialDistance * cos(initialAngle) + myNode->anchorPos.x();
  double y = initialDistance * sin(initialAngle) + myNode->anchorPos.y();
  state->pathLast.setX(x);
  state->pathLast.setY(y);
  myNode->state = this;
}

void XWTikzState::inverted()
{
  transform = transform.inverted();
}

double XWTikzState::lengthDependent(double dimension, 
                                    double lengthfactor,
                                    double linewidthfactor)
{
  double x = dimension + arrowLength * lengthfactor + lineWidth * linewidthfactor;
  return x;
}

void XWTikzState::lineTo(XWTikzCoord * p)
{
  if (coords.last()->isAtShapeBorder(this))
  {
    QPointF ap = p->getAnchor(PGFcenter,this);
    QPointF mp = coords.last()->getBorder(ap,this);
    mp = map(mp);
    points << mp;
  }

  coords << p;
  if (p->isAtShapeBorder(this))
  {
    QPointF ap = points.last();
    QPointF p1 = p->getBorder(ap,this);
    lineTo(p1);
    operations << XW_TIKZ_MOVETO;
  }
  else
  {
    QPointF p1 = p->getPoint(this);
    lineTo(p1);
  }
  
  tikzTimer = &XWTikzState::timerLine;
}

void XWTikzState::lineTo(const QPointF & p)
{
  secondLastOnPath = pathLast;
  QPointF p1 = map(p);
  pathLast = p1;
  if (points.size() == 1)
    secondOnPath = pathLast;
  operations << XW_TIKZ_LINETO;
  points << p1;

  lastOnPath = p1;
}

void XWTikzState::lineTo(double xA, double yA)
{
  QPointF p(xA,yA);
  lineTo(p);
}

void XWTikzState::lineToHV(XWTikzCoord * p)
{
  if (coords.last()->isAtShapeBorder(this))
  {
    QPointF ap = p->getAnchor(PGFcenter,this);
    QPointF mp = coords.last()->getBorder(ap,this);
    mp = map(mp);
    points << mp;
  }

  coords << p;
  if (p->isAtShapeBorder(this))
  {
    QPointF ap = points.last();
    QPointF p2 = p->getBorder(ap,this);
    lineToHV(p2);
    operations << XW_TIKZ_MOVETO;
  }
  else
  {
    QPointF p2 = p->getPoint(this);
    lineToHV(p2);
  }
}

void XWTikzState::lineToHV(const QPointF & p)
{
  QPointF p2 = map(p);
  QPointF p0 = pathLast;
  QPointF p1;
  p1.setX(p2.x());
  p1.setY(p0.y());

  operations << XW_TIKZ_LINETO;
   if (points.size() < 2)
    secondOnPath = p1;
  points << p1;

  pathLast = p2;
  operations << XW_TIKZ_LINETO;
  points << p2;
    
  secondLastOnPath = p1;
  lastOnPath = p2;

  tikzTimer = &XWTikzState::timerHVLine;
}

void XWTikzState::lineToVH(XWTikzCoord * p)
{
  if (coords.last()->isAtShapeBorder(this))
  {
    QPointF ap = p->getAnchor(PGFcenter,this);
    QPointF mp = coords.last()->getBorder(ap,this);
    mp = map(mp);
    points << mp;
  }

  coords << p;
  if (p->isAtShapeBorder(this))
  {
    QPointF ap = points.last();
    QPointF p2 = p->getBorder(ap,this);
    lineToVH(p2);
    operations << XW_TIKZ_MOVETO;
  }
  else
  {
    QPointF p2 = p->getPoint(this);
    lineToVH(p2);
  }
}

void XWTikzState::lineToVH(const QPointF & p)
{
  QPointF p2 = map(p);
  QPointF p0 = pathLast;
  QPointF p1;
  p1.setX(p0.x());
  p1.setY(p2.y());

  operations << XW_TIKZ_LINETO;
  if (points.size() < 2)
    secondOnPath = p1;
  points << p1;

  pathLast = p2;
  operations << XW_TIKZ_LINETO;
  points << p2;
    
  secondLastOnPath = p1;
  lastOnPath = p2;
}

double  XWTikzState::lineWidthDependent(double dimension, 
                          double linewidthfactor,
                          double outerfactor)
{
  double x = dimension;
  if (innerLineWidth > 0)
  {
    double xa = lineWidth;
    double xb = -0.5 * lineWidth * outerfactor;
    xa += xb;
    xb = -0.5 * innerLineWidth * outerfactor;
    xa += xb;
    xa = xa * linewidthfactor;
    x = x + xa;
  }
  else
  {
    x = x + lineWidth * linewidthfactor;
  }

  return x;
}

QRectF XWTikzState::map(const QRectF & rectA)
{
  return transform.mapRect(rectA);
}

QPointF XWTikzState::map(const QPointF & pA)
{
  double x = pA.x();
  double y = pA.y();
  if (nolinear_map != NULL)
    (this->*(nolinear_map))(x,y);
  QPointF p(x,y);
  return transform.map(p);
}

QPointF XWTikzState::map(double x,double y)
{
  double xA = x;
  double yA = y;
  (this->*(nolinear_map))(xA,yA);
  QPointF p(xA,yA);
  return transform.map(p);
}

void XWTikzState::map(double x1,double y1, double * x2, double * y2)
{
  QPointF p1(x1,y1);
  QPointF p2 = map(p1);
  *x2 = p2.x();
  *y2 = p2.y();
}

QPointF XWTikzState::mapInverted(const QPointF & pA)
{
  QTransform itrans = transform.inverted();
  return itrans.map(pA);
}

bool XWTikzState::moveTest(XWTeXBox * box)
{
  transformNode();
  if (!box)
    return false;
  QPointF p = getLastMousePoint();
  return box->contains(p.x(),p.y());
}

void XWTikzState::moveTo(double xA, double yA)
{
  QPointF p(xA,yA);
  moveTo(p);
}

void XWTikzState::moveTo(XWTikzCoord * p)
{
  coords << p;
  if (p->isAtShapeBorder(this))
  {
    operations << XW_TIKZ_MOVETO;
    return ;
  }
    
  QPointF p0 = p->getPoint(this);
  moveTo(p0);
}

void XWTikzState::moveTo(const QPointF & p)
{
  QPointF p1 = map(p);
  pathLast = p1;
  operations << XW_TIKZ_MOVETO;
  if (points.size() == 0)
    firstOnPath = p1;
  points << p1;
  
  lastOnPath = p1;
}

void XWTikzState::plotStreamEnd(bool c)
{
  isContinue = c;
  XWTikzPlotHandler plothandler(handler);
  plothandler.doHandler(this,plotStream,isContinue);
}

void XWTikzState::removeLastCoord()
{
  coords.removeLast();
}

void XWTikzState::resetArrowBoxArrows()
{
  arrowBoxNorthArrow.setX(0.0);
  arrowBoxNorthArrow.setY(0.0);
  arrowBoxSouthArrow.setX(0.0);
  arrowBoxSouthArrow.setY(0.0);
  arrowBoxEastArrow.setX(0.0);
  arrowBoxEastArrow.setY(0.0);
  arrowBoxWestArrow.setX(0.0);
  arrowBoxWestArrow.setY(0.0);
}

XWTikzState * XWTikzState::restore()
{
  if (driver)
  {
    flush();
    if (!isPath)
      driver->grestore();
  }
  
  XWTikzState * oldstate = this;
  if (saved)
  {
    oldstate = saved;
    saved = 0;
    delete this;
  }

  return oldstate;
}

void XWTikzState::rotate(double d)
{
  QTransform newtrans(cos(d),sin(d),-sin(d),cos(d),0,0);
  transform = newtrans * transform;
}

XWTikzState * XWTikzState::save(bool ispathA)
{
  if (driver && !ispathA)
    driver->gsave();
    
  XWTikzState * newstate = new XWTikzState(ispathA);
  copy(newstate,false);
  newstate->myNode = myNode;
  newstate->saved = this;
  
  return newstate;
}

XWTikzState * XWTikzState::saveNode(XWTeXBox * boxA,int nt)
{
  XWTikzState * newstate = new XWTikzState(false,this);
  copy(newstate,true);
  newstate->isPath = false;
  XWTikzShape * node = new XWTikzShape(driver,boxA,newstate,nt);
  newstate->myNode = node;
  newstate->myBox = boxA;
  switch (nt)
  {
    default:
      graphic->doEveryNode(newstate);
      graphic->doEveryShape(newstate);
      if (myNode && myNode->nodeType == XW_TIKZ_CHILD)
      {
        newstate->parentNode = parentNode;
        edgeFromParentFinished = true;
      }
      break;

    case XW_TIKZ_LABEL:      
      graphic->doEveryLabel(newstate);
      break;

    case XW_TIKZ_PIN:
      newstate->parentNode = myNode;
      graphic->doEveryPin(newstate);
      break;

    case XW_TIKZ_CHILD:
      newstate->parentNode = myNode;
      newstate->level = level + 1; 
      newstate->currentChild = childrenNumber++;
      graphic->doEveryChild(newstate);
      graphic->doEveryChildNode(newstate);
      break;

    case XW_TIKZ_INFO:
      graphic->doEveryInfo(newstate);
      break;

    case XW_TIKZ_CIRCUIT_SYMBOL:
      graphic->doEveryCircuitSymbol(newstate);
      break;
  }

  nodes << node;
  return newstate;
}

void XWTikzState::scale(double sx,double sy)
{
  QTransform newtrans(sx,0,0,sy,0,0);
  transform = newtrans * transform;
}

void XWTikzState::setAlign(int a)
{
  switch (a)
  {
    default:
      align = 0;
      break;

    case PGFcenter:
      align = 1;
      break;

    case PGFright:
      align = 2;
      break;
  }
}

void XWTikzState::setAnnotationArrow(XWTikzArrowSpecification * e)
{
  endArrow = e;
  arrowLength = 0.4 * circuitsSizeUnit + 0.3 * lineWidth;
}

void XWTikzState::setAspect(double a)
{
  aspect = a;
  aspectInverse = 1 / aspect;
}

void XWTikzState::setAuto(int d)
{
  autoAnchor = d;
  isAutoSet = true;
}

void XWTikzState::setBendHeight(double h)
{
  bendHeightIsSet = true;
  bendHeight = h;
}

void XWTikzState::setBendLeft(double a)
{
  toBend = a;
  toOut = toBend;
  toIn = 180 - toOut;
  relative = true;
}

void XWTikzState::setBendRight(double a)
{
  toBend = a;
  toOut = -toBend;
  toIn = 180 - toOut;
  relative = true;
}

void XWTikzState::setChamferedRectangleCorners(const QList<int> & cs)
{
  chamferedRectangleCorners = cs;
}

void XWTikzState::setCircuitSizeUnit(double s)
{
  circuitsSizeUnit = s;
  minWidth = circuitsSizeUnit * circuitSymbolWidth;
  minHeight = circuitsSizeUnit * circuitSymbolHeight;
}

void XWTikzState::setCircuitSymbolSize(double w, double h)
{
  circuitSymbolWidth = w;
  circuitSymbolHeight = h;
  minWidth = circuitsSizeUnit * circuitSymbolWidth;
  minHeight = circuitsSizeUnit * circuitSymbolHeight;
}

void XWTikzState::setCodes(const QList<XWTikzCommand*> & cmdsA)
{
  cmds = cmdsA;
}

void XWTikzState::setColor(const QColor & c)
{
  if (driver)
  {
    drawColor = c;
    fillColor = c;
    isFillText = true;
  }
}

void XWTikzState::setConceptColor(const QColor & c)
{
  fromColor = toColor;
  toColor = c;
}

void XWTikzState::setCurrentDirectionArrow(XWTikzArrowSpecification * e)
{
  endArrow = e;
  arrowLength = 1.3065 * 0.5 * circuitsSizeUnit + 1.3065 * 0.3 * lineWidth;
}

void XWTikzState::setCurveTo()
{
  to_path = &XWTikzState::toPathCurveTo;
}

void XWTikzState::setDash(int d)
{
  isDrawSet = true;
  switch (d)
  {
    default:
      break;

    case PGFdotted:
      dashPattern << 1 << 2 / lineWidth;
      break;

    case PGFdenselydotted:
      dashPattern << 1 << 1 / lineWidth;
      break;

    case PGFlooselydotted:
      dashPattern << 1 << 4 / lineWidth;
      break;

    case PGFdashed:
      dashPattern << 3 / lineWidth << 3 / lineWidth;
      break;

    case PGFdenselydashed:
      dashPattern << 3 / lineWidth << 2 / lineWidth;
      break;

    case PGFlooselydashed:
      dashPattern << 3 / lineWidth << 6 / lineWidth;
      break;

    case PGFdashdotted:
      dashPattern << 3 / lineWidth << 2 / lineWidth << 1 / lineWidth << 2 / lineWidth;
      break;

    case PGFdenselydashdotted:
      dashPattern << 3 / lineWidth << 1 / lineWidth << 1 / lineWidth << 1 / lineWidth;
      break;

    case PGFlooselydashdotted:
      dashPattern << 3 / lineWidth << 4 / lineWidth << 1 / lineWidth << 4 / lineWidth;
      break;

    case PGFdashdotdotted:
      dashPattern << 3 / lineWidth << 2 / lineWidth << 1 / lineWidth << 2 / lineWidth;
      dashPattern << 1 / lineWidth << 2 / lineWidth;
      break;

    case PGFdenselydashdotdotted:
      dashPattern << 3 / lineWidth << 1 / lineWidth << 1 / lineWidth << 1 / lineWidth;
      dashPattern << 1 / lineWidth << 1 / lineWidth;
      break;

    case PGFlooselydashdotdotted:
      dashPattern << 3 / lineWidth << 4 / lineWidth << 1 / lineWidth << 4 / lineWidth;
      dashPattern << 1 / lineWidth << 4 / lineWidth;
      break;
  }
}

void XWTikzState::setDashPattern(const QVector<qreal> & dpA)
{
  dashPattern = dpA;
}

void XWTikzState::setDecoration(int d)
{
  decoration = d;
  isDecorateSet = true;
}

void XWTikzState::setDrawColor(const QColor & c)
{
  drawColor = c;
  isDrawSet = true;
}

void XWTikzState::setFadingRoate(double d)
{
  QTransform newtrans(cos(d),sin(d),-sin(d),cos(d),0,0);
  fadingTransform = newtrans * fadingTransform;
}

void XWTikzState::setFadingScale(double sx,double sy)
{
  QTransform newtrans(sx,0,0,sy,0,0);
  fadingTransform = newtrans * fadingTransform;
}

void XWTikzState::setFadingShift(double dx,double dy)
{
  QTransform newtrans(1,0,0,1,dx,dy);
  fadingTransform = newtrans * fadingTransform;
}

void XWTikzState::setFadingSlant(double sx,double sy)
{
  QTransform newtrans(1,sx,sy,1,0,0);
  fadingTransform = newtrans * fadingTransform;
}

void XWTikzState::setFillColor(const QColor & c)
{
  fillColor = c;
  isFillSet = true;
}

void XWTikzState::setGrow(int g)
{
  switch (g)
  {
    default:
      break;

    case PGFdown:
    case PGFsouth:
      growAngle = -90;
      break;

    case PGFup:
    case PGFnorth:
      growAngle = 90;
      break;

    case PGFleft:
    case PGFwest:
      growAngle = 180;
      break;

    case PGFright:
    case PGFeast:
      growAngle = 0;
      break;

    case PGFnortheast:
      growAngle = 45;
      break;

    case PGFnorthwest:
      growAngle = 135;
      break;

    case PGFsoutheast:
      growAngle = -45;
      break;

    case PGFsouthwest:
      growAngle = -135;
      break;
  }

  growLeft = growAngle - 90;
  growRight = growLeft + 180;
}

void XWTikzState::setGrowOpposite(int g)
{
  setGrow(g);
  double tmp = growLeft;
  growLeft = growRight;
  growRight = tmp;
}

void XWTikzState::setInputs(const QList<QChar> & ins)
{
  inputs = ins;
}

void XWTikzState::setLineCap(int c)
{
  if (driver)
  {
    switch (c)
    {
      default:
        driver->setLineCap(0);
        break;

      case PGFround:
        driver->setLineCap(1);
        break;

      case PGFrect:
        driver->setLineCap(2);
        break;
    }
  }
}

void XWTikzState::setLineJoin(int j)
{
  if (driver)
  {
    switch (j)
    {
      default:
        driver->setLineJoin(0);
        break;

      case PGFround:
        driver->setLineJoin(1);
        break;

      case PGFbevel:
        driver->setLineJoin(2);
        break;
    }
  }
}

void XWTikzState::setLineTo()
{
  to_path = &XWTikzState::toPathDefault;
}

void XWTikzState::setLineWidth(double w)
{
  if (driver)
    driver->setLineWidth(w);
}

void XWTikzState::setLoop()
{
  toTarget = toStart;
  to_path = &XWTikzState::toPathCurveTo;
}

void XWTikzState::setMatrix(double a,double b,
                 double c,double d,
                 double dx, double dy)
{
  QTransform newtrans(a,b,c,d,dx,dy);
  transform = newtrans * transform;
}

void XWTikzState::setMindmap(int m)
{
  mindmap = m;
}

void XWTikzState::setMirror(bool e)
{
  if (e)
  {
    QTransform newtrans(1,0,0,-1,0,0);
    decorateTransform = newtrans * decorateTransform;
  }
}

void XWTikzState::setMiterLimit(double v)
{
  if (driver)
    driver->setMiterLimit(v);
}

void XWTikzState::setMoveTo()
{
  to_path = &XWTikzState::toPathMoveTo;
}

void XWTikzState::setNodeContents(const QString & str)
{
  if (myBox)
    myBox->setContents(str);
}

void XWTikzState::setNodeType(int shapeA,int nt)
{
  if (myNode)
  {
    myNode->nodeType = nt;
    switch (nt) 
    {
      default:
        break;

      case XW_TIKZ_CIRCUIT_SYMBOL:
        graphic->doEveryCircuitSymbol(this);
        shape = shapeA;
        break;
    }
  }
  else
  {
    XWTikzState * state = saveNode(0,nt);
    state->shape = shapeA;
  }
}

void XWTikzState::setOpacity(double v)
{
  drawOpacity = v;
  fillOpacity = v;
}

void XWTikzState::setPatternColor(const QColor & c)
{
  isPatternSet = true;
  patternColor = c;
}

void XWTikzState::setPatternName(int n)
{
  isPatternSet = true;
  patternName = n;
}

void XWTikzState::setPictureType(int t)
{
  pictureType = t;
  switch (pictureType)
  {
    default:
      break;

    case PGFcircuit:
    case PGFcircuiteeIEC:
    case PGFcircuitlogic:
    case PGFcircuitlogicIEC:
    case PGFcircuitlogicUS:
    case PGFcircuitlogicCDH:
      minWidth = circuitsSizeUnit * circuitSymbolWidth;
      minHeight = circuitsSizeUnit * circuitSymbolHeight;
      break;
  }
}

void XWTikzState::setPlane()
{
  setPlane(planeX.x(), planeX.y(),planeY.x(),planeY.y(),planeOrigin.x(),planeOrigin.y());
}

void XWTikzState::setPlane(double xa, double ya,
                           double xb, double yb,
                           double x, double y)
{
  transformTriangle(x,y,xa,ya,xb,yb);
  scale(0.035146,0.035146);
  xVec = 28.4527559;
  yVec = 28.4527559;
  zVec = 0;
}

void XWTikzState::setPlaneX(const QPointF & p)
{
  planeX.setX(p.x());
  planeX.setY(p.y());
}

void XWTikzState::setPlaneY(const QPointF & p)
{
  planeY.setX(p.x());
  planeY.setY(p.y());
}

void XWTikzState::setPlaneOrigin(const QPointF & p)
{
  planeOrigin.setX(p.x());
  planeOrigin.setY(p.y());
}

void XWTikzState::setPos(double p)
{
  nodeTime = p;
  isTimeSet = true;
}

void XWTikzState::setRaise(double r)
{
  QTransform newtrans(1,0,0,1,r,r);
  decorateTransform = newtrans * decorateTransform;
}

void XWTikzState::setRectangleSplitPartAlign(const QList<int> & l)
{
  rectangleSplitPartAlign = l;
}

void XWTikzState::setRectangleSplitPartFill(const QList<QColor> & l)
{
  rectangleSplitPartFill = l;
}

void XWTikzState::setSamplesAt(const QList<double> & sa)
{
  samplesAt = sa;
}

void XWTikzState::setShadingName(int n)
{
  shadingName = n;
  isShadeSet = true;
}

void XWTikzState::setTextColor(const QColor & c)
{
  fillColor = c;
  isFillText = true;
}

void XWTikzState::setTextOpacity(double o)
{
  fillOpacity = o;
  isFillText = true;
}

void XWTikzState::setToken(int num)
{
  setDraw(false);
  setInnerXSep(0.5);
  setInnerYSep(0.5);
  seiMinimumWidth(4.3);
  seiMinimumHeight(4.3);
  setTextColor(Qt::white);
  setFontSize(XW_TEX_FONT_TINY);
  graphic->doEveryToken(this);
  switch (childrenNumber)
  {
    default:
      break;

    case 2:
      {
        switch (num)
        {
          default:
            break;

          case 1:
            pathLast.setX(pathLast.x() - 0.5);
            break;

          case 2:
            pathLast.setX(pathLast.x() + 0.5);
            break;
        }
      }
      break;

    case 3:
      {
        switch (num)
        {
          default:
            break;

          case 1:
            pathLast.setY(pathLast.y() + 0.57);
            break;

          case 2:
            pathLast.setX(pathLast.x() - 0.5);
            pathLast.setY(pathLast.y() - 0.306025);
            break;

          case 3:
            pathLast.setX(pathLast.x() + 0.5);
            pathLast.setY(pathLast.y() - 0.306025);
            break;
        }
      }
      break;

    case 4:
      {
        switch (num)
        {
          default:
            break;

          case 1:
            pathLast.setX(pathLast.x() - 0.5);
            pathLast.setY(pathLast.y() + 0.5);
            break;

          case 2:
            pathLast.setX(pathLast.x() + 0.5);
            pathLast.setY(pathLast.y() + 0.5);
            break;

          case 3:
            pathLast.setX(pathLast.x() - 0.5);
            pathLast.setY(pathLast.y() - 0.5);
            break;

          case 4:
            pathLast.setX(pathLast.x() + 0.5);
            pathLast.setY(pathLast.y() - 0.5);
            break;
        }
      }
      break;

    case 5:
      {
        switch (num)
        {
          default:
            break;

          case 1:
            pathLast.setY(pathLast.y() + 0.85);
            break;

          case 2:
            pathLast.setX(pathLast.x() - 0.808398);
            pathLast.setY(pathLast.y() + 0.26266);
            break;

          case 3:
            pathLast.setX(pathLast.x() + 0.808398);
            pathLast.setY(pathLast.y() + 0.26266);
            break;

          case 4:
            pathLast.setX(pathLast.x() - 0.499617);
            pathLast.setY(pathLast.y() - 0.687664);
            break;

          case 5:
            pathLast.setX(pathLast.x() + 0.499617);
            pathLast.setY(pathLast.y() - 0.687664);
            break;
        }
      }
      break;

    case 6:
      {
        switch (num)
        {
          default:
            break;

          case 1:
            pathLast.setX(pathLast.x() - 1);
            pathLast.setY(pathLast.y() + 0.5);
            break;

          case 2:
            pathLast.setY(pathLast.y() + 0.5);
            break;

          case 3:
            pathLast.setX(pathLast.x() + 1);
            pathLast.setY(pathLast.y() + 0.5);
            break;

          case 4:
            pathLast.setX(pathLast.x() - 1);
            pathLast.setY(pathLast.y() - 0.5);
            break;

          case 5:
            pathLast.setY(pathLast.y() - 0.5);
            break;

          case 6:
            pathLast.setX(pathLast.x() + 1);
            pathLast.setY(pathLast.y() - 0.5);
            break;
        }
      }
      break;

    case 7:
      {
        switch (num)
        {
          default:
            break;

          case 1:
            pathLast.setY(pathLast.y() + 1);
            break;

          case 2:
            pathLast.setX(pathLast.x() - 1);
            pathLast.setY(pathLast.y() + 0.5);
            break;

          case 4:
            pathLast.setX(pathLast.x() + 1);
            pathLast.setY(pathLast.y() + 0.5);
            break;

          case 5:
            pathLast.setX(pathLast.x() - 1);
            pathLast.setY(pathLast.y() - 0.5);
            break;

          case 6:
            pathLast.setY(pathLast.y() - 1);
            break;

          case 7:
            pathLast.setX(pathLast.x() + 1);
            pathLast.setY(pathLast.y() - 0.5);
            break;
        }
      }
      break;

    case 8:
      {
        switch (num)
        {
          default:
            break;

          case 1:
            pathLast.setX(pathLast.x() - 0.5);
            pathLast.setY(pathLast.y() + 1);
            break;

          case 2:
            pathLast.setX(pathLast.x() + 0.5);
            pathLast.setY(pathLast.y() + 1);
            break;

          case 3:
            pathLast.setX(pathLast.x() - 1);
            break;

          case 5:
            pathLast.setX(pathLast.x() + 1);
            break;

          case 6:
            pathLast.setX(pathLast.x() - 1);
            pathLast.setY(pathLast.y() - 1);
            break;

          case 7:
            pathLast.setY(pathLast.y() - 1);
            break;

          case 8:
            pathLast.setX(pathLast.x() + 1);
            pathLast.setY(pathLast.y() - 1);
            break;
        }
      }
      break;

    case 9:
      {
        switch (num)
        {
          default:
            break;

          case 1:
            pathLast.setX(pathLast.x() - 1);
            pathLast.setY(pathLast.y() + 1);
            break;

          case 2:
            pathLast.setY(pathLast.y() + 1);
            break;

          case 3:
            pathLast.setX(pathLast.x() + 1);
            pathLast.setY(pathLast.y() + 1);
            break;

          case 4:
            pathLast.setX(pathLast.x() - 1);
            break;

          case 6:
            pathLast.setX(pathLast.x() + 1);
            break;

          case 7:
            pathLast.setX(pathLast.x() - 1);
            pathLast.setY(pathLast.y() - 1);
            break;

          case 8:
            pathLast.setY(pathLast.y() - 1);
            break;

          case 9:
            pathLast.setX(pathLast.x() + 1);
            pathLast.setY(pathLast.y() - 1);
            break;
        }
      }
      break;
  }
}

void XWTikzState::setupArrow(int a)
{
  int oldbm = arrowBendMode;
  bool oldr = arrowReversed;
  XWTikzArrow arrow(a);
  arrow.setup(this);
  computeShortening(&arrow);
  arrowBendMode = oldbm;
  arrowReversed = oldr;
}

void XWTikzState::setVariables(const QString & nameA,const QString & var)
{
  if (var.length() <= 0)
    strings[nameA] = var;
  else
  {
    if (var[0] == QChar('('))
    {
      XWTikzCoord * p = new XWTikzCoord(graphic,var,this);
      coordsNamed[nameA] = p;
    }
    else if (var[0] == QChar('+'))
    {
      if (var[1].isDigit())
      {
        XWTikzExpress exp(graphic,var);
        double v = exp.getResult(this);
        values[nameA] = v;
      }
      else
      {
        XWTikzCoord * p = new XWTikzCoord(graphic,var,this);
        coordsNamed[nameA] = p;
      }
    }
    else if (var[0] == QChar('.') ||var[0] == QChar('-') || var[0].isDigit())
    {
      XWTikzExpress exp(graphic,var);
      double v = exp.getResult(this);
      values[nameA] = v;
    }
    else
    {
      int id = lookupPGFID(var);
      colors[nameA] = tikzEnumToColor(id);
    }
  }
}

void XWTikzState::shift(double dx,double dy)
{
  QTransform newtrans(1,0,0,1,dx,dy);
  transform = newtrans * transform;
}

void XWTikzState::shiftOnly()
{
  double dx = transform.m31();
  double dy = transform.m32();
  transform.reset();
  transform.translate(dx,dy);
}

void XWTikzState::slant(double sx,double sy)
{
  QTransform newtrans(1,sx,sy,1,0,0);
  transform = newtrans * transform;
}

void XWTikzState::switchColor(const QColor & fromc,const QColor & toc)
{
  fromColor = fromc;
  toColor = toc;
}

QPointF XWTikzState::tangent(const QPointF & nc, 
                  const QPointF & ne,
                  const QPointF & p, 
                  int s)
{
  QPointF nnc = map(nc);
  QPointF pp = map(p);

  double xa = pp.x() - nnc.x();
  double ya = pp.y() - nnc.y();

  QTransform transform1;
  transform1.translate(nc.x(),nc.y());

  QPointF nne = transform1.map(ne);
  double r = veclen(xa,ya);
  double delta = acos(nne.x() / r);
  if (s > 1)
    delta = -delta;

  QVector2D v(xa,ya);
  v.normalize();

  ya = -v.y();

  QTransform transform2(v.x(),v.y(),ya,v.x(),0,0);
  double x = nne.x() * cos(delta);
  double y = nne.x() * sin(delta);

  QPointF np(x,y);
  np = transform2.map(np);
  transform2 = transform2.inverted();
  np = transform2.map(np);
  QTransform transform3;
  transform3.translate(-nc.x(),-nc.y());
  np = transform3.map(np);
  return np;
}

void XWTikzState::toPath(XWTikzCoord * p)
{
  if (p)
  {
    toStart = coords.last();
    coords << p;
    toTarget = p;
    isPath = true;
    graphic->doToPath(this);
  }
}

void XWTikzState::transformColumn()
{
  shift(columnSep,0);
}

void XWTikzState::transformNode()
{
  if (!isTimeSet)
  {
    QPointF p = pathLast;
    if (!transformShape)
    {      
      QTransform trans(1,0,0,1,p.x(),p.y());
      transform = trans;
    }
    else
      shift(p.x(),p.y());
  }
  else
    (this->*(tikzTimer))();
}

void XWTikzState::transformRow(double h)
{
  shift(0,h);
}

void XWTikzState::addArc(double sa,double ea, double xr,double yr)
{
  double la = sa;
  double lb = ea;
  double arctmp = 0;
  while (true)
  {
    double tmpa = la - lb;
    if (tmpa < 0)
      tmpa = -tmpa;

    if (tmpa <= 90)
      break;

    if (tmpa > 115)
      arctmp = 90;
    else
      arctmp = 60;

    if (lb > la)
    {
      lb = la + arctmp;
      arcTo(la,lb,xr,yr);
      la += arctmp;
    }
    else
    {
      lb = la - arctmp;
      arcTo(la,lb,xr,yr);
      la -= arctmp;
    }    

    operations << XW_TIKZ_INSIDE;
  }
  arcTo(la,lb,xr,yr);
  operations << XW_TIKZ_OUTSIDE;
}

void XWTikzState::addEllipse(const QPointF & c,const QPointF & a,const QPointF & b)
{
  QPointF p = map(c);
  double xc = p.x();
  double yc = p.y();

  p = map(a);
  double xa = p.x() + xc;
  double ya = p.y() + yc;
  operations << XW_TIKZ_MOVETO;
  QPointF p1(xa,ya);
  if (points.size() == 0)
    firstOnPath = p1;
  points << p1;

  operations << XW_TIKZ_INSIDE;

  p = map(b);
  double xb = p.x();
  double yb = p.y();

  double x = 0.55228475 * xb + xa + xc;
  double y = 0.55228475 * yb + ya + yc;
  double tmpx = x;
  double tmpy = y;
  x = 0.55228475 * xa + xb + xc;
  y = 0.55228475 * ya + yb + yc;
  xb += xc;
  yb += yc;

  operations << XW_TIKZ_CURVETO;
  QPointF p2(tmpx,tmpy);
  QPointF p3(x,y);
  QPointF p4(xb,yb);
  if (points.size() < 2)
    secondOnPath = p2;
  points << p2;
  points << p3;
  points << p4;

  xa = -xa;
  ya = -ya;
  x = 0.55228475 * xa + xb + xc;
  y = 0.55228475 * ya + yb + yc;
  tmpx = x;
  tmpy = y;
  x = 0.55228475 * xb + xa + xc;
  y = 0.55228475 * yb + ya + yc;
  xa += xc;
  ya += yc;

  operations << XW_TIKZ_CURVETO;
  QPointF p5(tmpx,tmpy);
  QPointF p6(x,y);
  QPointF p7(xa,ya);
  points << p5;
  points << p6;
  points << p7;

  xb = -xb;
  yb = -yb;
  x = 0.55228475 * xb + xa + xc;
  y = 0.55228475 * yb + ya + yc;
  tmpx = x;
  tmpy = y;
  x = 0.55228475 * xa + xb + xc;
  y = 0.55228475 * ya + yb + yc;
  xb += xc;
  yb += yc;

  operations << XW_TIKZ_CURVETO;
  QPointF p8(tmpx,tmpy);
  QPointF p9(x,y);
  QPointF p10(xb,yb);
  points << p8;
  points << p9;
  points << p10;

  xa = -xa;
  ya = -ya;
  x = 0.55228475 * xa + xb + xc;
  y = 0.55228475 * ya + yb + yc;
  tmpx = x;
  tmpy = y;
  x = 0.55228475 * xb + xa + xc;
  y = 0.55228475 * yb + ya + yc;
  xa += xc;
  ya += yc;
  operations << XW_TIKZ_CURVETO;
  QPointF p11(tmpx,tmpy);
  QPointF p12(x,y);
  QPointF p13(xa,ya);
  points << p11;
  points << p12;
  points << p13;

  secondLastOnPath = p12;
  lastOnPath = p13;

  operations << XW_TIKZ_CLOSE;

  operations << XW_TIKZ_MOVETO;
  QPointF p14(xc,yc);
  points << p14;
  operations << XW_TIKZ_OUTSIDE;
}

void XWTikzState::addGrid(const QPointF & ll,const QPointF & ur)
{
  int counta = (int)(ll.y() / ystep);
  double y = counta * ystep;
  if (y < ll.y())
    y += ystep;

  operations << XW_TIKZ_INSIDE;

  QPointF p;
  while (y < ur.y())
  {
    p.setX(ll.x());
    p.setY(y);
    p = map(p);
    operations << XW_TIKZ_MOVETO;
    points << p;
    p.setX(ur.x());
    p.setY(y);
    p = map(p);
    operations << XW_TIKZ_LINETO;
    points << p;
    y += ystep;
  }

  y -= 0.01;
  if (y < ur.y())
  {
    p.setX(ll.x());
    p.setY(y);
    p = map(p);
    operations << XW_TIKZ_MOVETO;
    points << p;
    p.setX(ur.x());
    p.setY(y);
    p = map(p);
    operations << XW_TIKZ_LINETO;
    points << p;
  }

  counta = (int)(ll.x() / xstep);
  double x = counta * xstep;
  if (x < ll.x())
    x += xstep;

  while (x < ur.x())
  {
    p.setX(x);
    p.setY(ll.y());
    p = map(p);
    operations << XW_TIKZ_MOVETO;
    points << p;
    p.setX(x);
    p.setY(ur.y());
    p = map(p);
    operations << XW_TIKZ_LINETO;
    points << p;
    x += xstep;
  }

  x -= 0.01;
  if (x < ur.x())
  {
    p.setX(x);
    p.setY(ll.y());
    p = map(p);
    operations << XW_TIKZ_MOVETO;
    points << p;
    p.setX(x);
    p.setY(ur.y());
    p = map(p);
    operations << XW_TIKZ_LINETO;
    points << p;
    x += xstep;
  }

  pathLast = ur;

  operations << XW_TIKZ_OUTSIDE;
}

void XWTikzState::addPlotStreamPoint(const QPointF & p)
{
  plotStream << p;
}

void XWTikzState::addRectangle(const QPointF & ll,const QPointF & ur)
{
  rectangle(ll,ur.x() - ll.x(),ur.y() - ll.y());
}

void XWTikzState::addRoundedCorner(const QPointF & a,const QPointF & b,const QPointF & c)
{
  double xa = a.x() + 0.5522847 * (b.x() - a.x());
  double ya = a.y() + 0.5522847 * (b.y() - a.y());

  double xc = c.x() + 0.5522847 * (b.x() - c.x());
  double yc = c.x() + 0.5522847 * (b.y() - c.y());

  QPointF p1(xa,ya);
  QPointF p2(xc,yc);
  driver->curveTo(p1,p2,c);
}

void XWTikzState::anchorChild()
{
  if (isChildAnchorSet)
    anchor = childAnchor;
}

void XWTikzState::anchorNode()
{
  if (matrix)
  {
    if (isMatrixAnchorSet)
      anchor = matrixAnchor;

    return ;
  }
  
  if (!isTimeSet)
    return ;
  
  if (isSwapSet)
  {
    if (autoAnchor == PGFleft)
      autoAnchor = PGFright;
    else
      autoAnchor = PGFleft;
  }

  if (isAutoSet)
  {
    transformShape = true;
    isSlopedSet = true;
    isAllowUpsideDown = true;
    anchor = autoAnchor;

    (this->*(tikzTimer))();

    setAutoAnchor(transform.m11(),transform.m12());
  }
}

void XWTikzState::anchorParent()
{
  if (isParentAnchorSet)
    anchor = parentAnchor;
}

double XWTikzState::angleBetweenLines(const QPointF & p1,const QPointF & p2,
                           const QPointF & p3,const QPointF & p4)
{
  double firstangle = angleBetweenPoints(p1,p2);
  double secondangle = angleBetweenPoints(p3,p4);
  if (firstangle > firstangle)
    secondangle += 360;

  secondangle -= firstangle;
  return secondangle;
}

double XWTikzState::angleBetweenPoints(const QPointF & p1,const QPointF & p2)
{
  double xa = p1.x() - p2.x();
  double ya = p1.y() - p2.y();
  double xb = xa;
  double yb = ya;
  if (xa < 0)
    xa = -xa;
  if (ya < 0)
    ya = -ya;

  double x = ya;
  double y = xa;
  if (ya > xa)
  {
    x = xa;
    y = ya;
  }

  if (y == 0)
    x = 0;
  else
    x = 1 / y;

  x = x * 1000;
  x = atan(x);
  if (ya > xa)
    x = 90 - x;

  if (xb < 0)
  {
    if (yb > 0)
      x = -x;

    x = 180 + x;
  }
  else
  {
    if (yb < 0)
      x = 360 - x;
  }
  
  return x;
}

void XWTikzState::arcOfEllipse(const QPointF & c,const QPointF & endpoint,
                               double sa,double sb)
{
  QPointF a = pathLast - c;
  double x = pathLast.x() - sa * a.y();
  double y = pathLast.y() + sb * a.x();
  QPointF c1(x,y);

  a = endpoint - c;
  x = endpoint.x() + sa * a.y();
  y = endpoint.y() - sb * a.x();
  QPointF c2(x,y);

  curveTo(c1,c2,endpoint);
}

void XWTikzState::arcTo(double la,double lb, double xr,double yr)
{
  fourthLastOnPath = pathLast;
  double xb = lb - la;
  if (lb < la)
    xb = la - lb;

  double m = 0;
  if (xb == 90)
    m = 0.55228475;
  else
    m = 1.333333333*tan(0.25*xb);

  double tmpa = m * xr;
  double tmpb = m * yr;

  double xa  = la - 90;
  if (lb > la)
    xa = la + 90;

  QPointF p;
  p.setX(tmpa * cos(xa));
  p.setY(tmpb * sin(xa));

  p = map(p);

  xa = pathLast.x() + p.x();
  double ya = pathLast.y() + p.y();

  p.setX(xr * cos(la));
  p.setY(yr * sin(la));
  p = map(p);

  xb = pathLast.x() - p.x();
  double yb = pathLast.y() - p.y();

  p.setX(xr * cos(lb));
  p.setY(yr * sin(lb));
  p = map(p);
  xb = xb + p.x();
  yb = yb + p.y();

  if (lb > la)
    lb -= 90;
  else
    lb += 90;

  p.setX(tmpa * cos(lb));
  p.setY(tmpb * sin(lb));
  p = map(p);

  double xc = xb + p.x();
  double yc = yb + p.y();

  pathLast.setX(xb);
  pathLast.setY(yb);
  
  QPointF p1(xa,ya);
  QPointF p2(xc,yc);

  if (points.size() < 4)
  {
    secondOnPath = p1;
    thirdOnPath = p2;
    fourthOnPath = pathLast;
  }

  operations << XW_TIKZ_CURVETO;
  points << p1;
  points << p2;
  points << pathLast;

  thirdLastOnPath = p1;
  secondLastOnPath = p2;
  lastOnPath = pathLast;
}

void XWTikzState::computeAngle()
{
  int counta = (int)(labelAngle);
  switch (counta)
  {
    default:
      break;

    case PGFright:
      labelAngle = 0;
      break;

    case PGFaboveright:
      labelAngle = 45;
      break;

    case PGFabove:
      labelAngle = 90;
      break;

    case PGFaboveleft:
      labelAngle = 135;
      break;

    case PGFleft:
      labelAngle = 180;
      break;

    case PGFbelowleft:
      labelAngle = 225;
      break;

    case PGFbelow:
      labelAngle = 270;
      break;

    case PGFbelowright:
      labelAngle = 315;
      break;
  }
}

void XWTikzState::computeDirection()
{
  int counta = (int)(labelAngle);
  if (counta < 0)
    counta += 360;
  if (counta > 359)
    counta -= 360;

  if (counta < 4)
    anchor = PGFwest;
  else if (counta < 87)
    anchor = PGFsouthwest;
  else if (counta < 94)
    anchor = PGFsouth;
  else if (counta < 177)
    anchor = PGFsoutheast;
  else if (counta < 184)
    anchor = PGFeast;
  else if (counta < 267)
    anchor = PGFnortheast;
  else if (counta < 274)
    anchor = PGFnorth;
  else if (counta < 357)
    anchor = PGFnorthwest;
  else
    anchor = PGFwest;
}

void XWTikzState::computeShortening(XWTikzArrow * a)
{
  arrowTotalLength += a->tipEnd;
  arrowTotalLength -= a->backEnd;
  arrowTotalLength += arrowSep;
  shorteningDistance += a->tipEnd;
  shorteningDistance += arrowSep;
  shorteningDistance -= a->backEnd;
}

void XWTikzState::constructPath()
{
  int k = 0;
  int state = 0;
  QPointF p,cp;
  for (int i = 0; i < operations.size(); i++)
  {
    switch (operations[i])
    {
      default:
        state = operations[i];
        break;

      case XW_TIKZ_MOVETO:
        p = points[k++];
        cp = p;
        driver->moveTo(p);
        break;

      case XW_TIKZ_LINETO:
        p = points[k++];
        if (state != XW_TIKZ_INSIDE && roundedCorners != 0)
        {
          QPointF p0 = points[k - 2];
          if (i == 1)
            p = lineAtDistance(roundedCorners,p,p0);
          else
          {
            QPointF c = lineAtDistance(roundedCorners,p0,p);
            addRoundedCorner(cp,p0,c);
            cp = c;
            p = lineAtDistance(roundedCorners,p,p0);
          }
        }
        cp = p;
        driver->lineTo(p);
        break;

      case XW_TIKZ_CURVETO:
        {
          QPointF p0 = points[k - 1];
          QPointF c1 = points[k++];
          QPointF c2 = points[k++];
          p = points[k++];
          if (state != XW_TIKZ_INSIDE && roundedCorners != 0)
          {
            if (i == 1)
              p = lineAtDistance(roundedCorners,p,c2);
            else
            {
              QPointF c = lineAtDistance(roundedCorners,p0,c1);
              addRoundedCorner(cp,c1,c);
              cp = c;
              p = lineAtDistance(roundedCorners,p,p0);
            }
          }
          cp = p;
          driver->curveTo(c1,c2,p);          
        }
        break;

      case XW_TIKZ_CLOSE:
        driver->closePath();
        break;
    }
  }
}

QPointF XWTikzState::curveAtTime(double t,
                                 const QPointF & startpoint,
                                 const QPointF & c1, 
                                 const QPointF & c2,
                                 const QPointF & endpoint)
{
  double xc = endpoint.x();
  double yc = endpoint.y();
  double xb = c2.x();
  double yb = c2.y();
  double xa = c1.x();
  double ya = c1.y();

  double x = (1 - t) * startpoint.x() + t * xa;
  double y = (1 - t) * startpoint.y() + t * ya;
  xa = (1 - t) * xa + t * xb;
  ya = (1 - t) * ya + t * yb;
  xb = (1 - t) * xb + t * xc;
  yb = (1 - t) * yb + t * yc;

  x = (1 - t) * x + t * xa;
  y = (1 - t) * y + t * ya;
  xa = (1 - t) * xa + t * xb;
  ya = (1 - t) * ya + t * yb;

  xb = x;
  yb = y;

  x = (1 - t) * x + t * xa;
  y = (1 - t) * y + t * ya;

  curveXA = xa;
  curveYA = ya;
  curveXB = xb;
  curveYB = yb;
  curveXC = xc;
  curveYC = yc;

  QPointF p(x,y);
  return p;
}

void XWTikzState::curveBetweenTime(double s, double t,QPointF & startpoint,
                        QPointF & c1,QPointF & c2,
                        QPointF & endpoint,bool ignoremoveto)
{
  if (s > t)
  {
    s = 1 - s;
    t = 1 - t;
    curveBetweenTime(t,s,t,endpoint,c2,c1,startpoint,ignoremoveto);
  }
  else
    curveBetweenTime(t,s,t,startpoint,c1,c2,endpoint,ignoremoveto);

  operations << XW_TIKZ_OUTSIDE;
}

void XWTikzState::curveBetweenTime(double t,double ts,double tt,
                      const QPointF & startpoint,const QPointF & c1, 
                      const QPointF & c2,const QPointF & endpoint,
                      bool ignoremoveto)
{
  QPointF b = startpoint - t * startpoint + t * c1;
  QPointF c = c1 - t * c1;
  QPointF a = b + t * (t * c2) - t * b + t * c;

  QPointF p = curveAtTime(t,startpoint,c1,c2,endpoint);
  if (t == tt)
  {
    double s = 1 - ts / tt;
    curveBetweenTime(s,ts,tt,p,a,b,c,ignoremoveto);
  }
  else
  {
    if (ignoremoveto)
      curveTo(a,b,startpoint);
    else
    {
      moveTo(p);
      curveTo(a,b,startpoint);
    }
  }

  operations << XW_TIKZ_INSIDE;
}

double XWTikzState::curviLinearDistanceToTime(double d)
{
  double x = d;
  if (x < curviLinearLengthC)
  {
    if (x < curviLinearLengthA)
      x = x * curviLinearTimeA / curviLinearLengthA;
    else if (x < curviLinearLengthB)
    {
      double y = curviLinearLengthB - curviLinearLengthA;
      double mathresult = curviLinearTimeA / y;
      double quotb = mathresult;
      y = -mathresult * curviLinearLengthA + curviLinearTimeA;
      double correctb = y;
      x = quotb * x + correctb;
    }
    else
    {
      double y = 0.5 * (curviLinearLengthC - curviLinearLengthB);
      double mathresult = curviLinearTimeA / y;
      double quotc = mathresult;
      y = -quotc * curviLinearLengthB + 2 * curviLinearTimeA;
      double correctc = y;
      x = quotc * x + correctc;
    }
  }
  else if (x < curviLinearLengthD)
  {
    double y = 0.25 * (curviLinearLengthD - curviLinearLengthC);
    double mathresult = curviLinearTimeA / y;
    double quotd = mathresult;
    y = -quotd * curviLinearLengthC + 4 * curviLinearTimeA;
    double correctd = y;
    x = quotd * x + correctd;
  }
  else
  {
    x = x * 8 * curviLinearTimeA / curviLinearLengthD;
  }
  return x;
}

void XWTikzState::curviLinearBezierOrthogonal(double & d,double & f)
{
  double x = curviLinearDistanceToTime(d);
  QPointF p = curveAtTime(x,curviLinearLineA,curviLinearLineB,curviLinearLineC,curviLinearLineD);
  double xc = p.x();
  double yc = p.y();
  double xb = curveXB - curveXA;
  double yb = curveYB - curveYA;
  if (xb < 0.0001)
  {
    if (xb > -0.0001)
    {
      if (yb < 0.0001)
      {
        if (yb > -0.0001)
        {
          double xa = curviLinearLineA.x();
          double ya = curviLinearLineA.y();
          curveXA = xa;
          curveYA = ya;
          xb = curviLinearLineC.x() - xa;
          yb = curviLinearLineC.y() - ya;
          if (xb < 0.0001)
          {
            if (xb > -0.0001)
            {
              if (yb < 0.0001)
              {
                if (yb > -0.0001)
                {
                  xb = curviLinearLineD.x() - xa;
                  yb = curviLinearLineD.y() - ya;
                }
              }
            }
          }
          xb = -xb;
          yb = -yb;
        }
      }
    }
  }

  x = yb;
  double y = -xb;
  QVector2D v(x,y);
  v.normalize();

  x = f * v.x() + xc;
  y = f * v.y() + yc;
  curveXB = xb;
  curveYB = yb;
  d = x;
  f = y;
}

void XWTikzState::curviLinearBezierPolar(double & d,double & f)
{
  double tempdima = d;
  double tempdimb = f;
  QVector2D v(tempdima,tempdimb);
  v.normalize();
  double x = v.x();
  double y = v.y();
  if (tempdima == 0)
  {
    x = 1;
    y = 0;
  }
  if (tempdima < 0)
  {
    x = -x;
    y = -y;
  }
  double ya = -y;
  QTransform trans(x,y,ya,x,0,0);
  trans.translate(-curviLinearLineA.x(), -curviLinearLineA.y());

  double mathresult = veclen(tempdima,tempdimb);
  if (tempdima < 0)
    mathresult = -mathresult;

  x = curviLinearDistanceToTime(mathresult);
  QPointF p = curveAtTime(x,curviLinearLineA,curviLinearLineB,curviLinearLineC,curviLinearLineD);
  p = trans.map(p);
  p = p + curviLinearLineA;
  d = p.x();
  f = p.y();
}

double XWTikzState::decorateCurveLength(const QPointF & c1,
                             const QPointF & c2,
                             const QPointF & c3,
                             const QPointF & c4)
{
  double xa = c4.x() - c1.x();
  double ya = c4.y() - c1.y();
  if (xa < 0)
    xa = -xa;
  if (ya < 0)
    ya = -ya;

  if (xa < 1 && ya < 1)
    return veclen(xa,ya);

  return decorateCurveLeft(c1,c2,c3,c4) + decorateCurveRight(c1,c2,c3,c4);
}

double XWTikzState::decorateCurveLeft(const QPointF & c1,
                             const QPointF & c2,
                             const QPointF & c3,
                             const QPointF & c4)
{
  double xa = 0.5 * c1.x() + 0.5 * c2.x();
  double ya = 0.5 * c1.y() + 0.5 * c2.y();
  double xb = 0.25 * c1.x() + 0.5 * c2.x() + 0.25 * c3.x();
  double yb = 0.25 * c1.y() + 0.5 * c2.y() + 0.25 * c3.y();
  double xc = 0.125 * c1.x() + 0.375 * c2.x() + 0.375 * c3.x() + 0.125 * c4.x();
  double yc = 0.125 * c1.y() + 0.375 * c2.y() + 0.375 * c3.y() + 0.125 * c4.y();

  QPointF p2(xa,ya);
  QPointF p3(xb,yb);
  QPointF p4(xc,yc);

  return decorateCurveLength(c1,p2,p3,p4);
}

double XWTikzState::decorateCurveRight(const QPointF & c1,
                             const QPointF & c2,
                             const QPointF & c3,
                             const QPointF & c4)
{
  double x = 0.125 * c1.x() + 0.375 * c2.x() + 0.375 * c3.x() + 0.125 * c4.x();
  double y = 0.125 * c1.y() + 0.375 * c2.y() + 0.375 * c3.y() + 0.125 * c4.y();
  double xa = 0.25 * c2.x() + 0.5 * c3.x() + 0.25 * c4.x();
  double ya = 0.25 * c2.y() + 0.5 * c3.y() + 0.25 * c4.y();
  double xb = 0.5 * c3.x() + 0.5 * c4.x();
  double yb = 0.5 * c3.y() + 0.5 * c4.y();

  QPointF p1(x,y);
  QPointF p2(xa,ya);
  QPointF p3(xb,yb);

  return decorateCurveLength(p1,p2,p3,c4);
}

double XWTikzState::decorateLineLength(const QPointF & c1,const QPointF & c2)
{
  return veclen(c1.x() - c2.x(),c1.y() - c2.y());
}

void XWTikzState::decorateMoveAlongInputSegmentCurve(double distancetomove,
                                            double currentpathlength,
                                            double & decorateangle,
                                            double & inputsegmenttime,
                                            const QPointF & startpoint,
                                            const QPointF & c1, 
                                            const QPointF & c2,
                                            const QPointF & endpoint)
{
  if (distancetomove == 0)
    return ;

  QPointF p1 = curveAtTime(inputsegmenttime,startpoint,c1,c2,endpoint);
  double xa = p1.x();
  double ya = p1.y();
  double xb = inputsegmenttime;
  double yb = 0.0009765625;
  if (currentpathlength < 128)
    yb = 0.03125;
  else if (currentpathlength < 512)
    yb = 0.015625;
  else if (currentpathlength < 2048)
    yb = 0.00390625;

  int counta = 1;
  double tempdima = 0;
  do
  {
    xb += counta * yb;
    double tmpxa = xa;
    double tmpya = ya;
    double tmpxb = xb;
    double tmpyb = yb;
    double x = (1 - xb) * startpoint.x() + xb * c1.x();
    double y = (1 - xb) * startpoint.y() + xb * c1.y();
    xa = (1 - xb) * c1.x() + xb * c2.x();
    ya = (1 - xb) * c1.y() + xb * c2.y();
    xb = (1 - xb) * c2.x() + xb * endpoint.x();
    yb = (1 - xb) * c2.y() + xb * endpoint.y();

    x = (1 - xb) * x + xb * xa;
    y = (1 - xb) * y + xb * ya;
    xa = (1 - xb) * xa + xb * xa;
    ya = (1 - yb) * ya + xb * ya;
    xb = x;
    yb = y;
    x = (1 - xb) * x + xb * xa;
    y = ( 1 - yb) * y + yb * ya;

    xa = tmpxa;
    ya = tmpya;
    xb = tmpxb;
    yb = tmpyb;

    double xc = xa;
    double yc = ya;
    xa = x;
    ya = y;
    x -= xc;
    y -= yc;

    tempdima = tempdima + counta * veclen(x,y);
    if (counta > 0)
    {
      if (tempdima > distancetomove)
      {
        counta = -counta;
        yb = 0.5 * yb;
      }
    }
    else
    {
      if (tempdima < distancetomove)
      {
        counta = -counta;
        yb = 0.5 * yb;
      }
    }
    
  } while (yb != 0);

  inputsegmenttime = xb;
  
  QPointF p2 = curveAtTime(inputsegmenttime,startpoint,c1,c2,endpoint);
  decorateangle = angleBetweenPoints(p1,p2);
}

void XWTikzState::decorateTransformCurve(double t,
                                const QPointF & startpoint,
                                const QPointF & c1, 
                                const QPointF & c2,
                                const QPointF & endpoint)
{
  isSlopedSet = true;
  isAllowUpsideDown = true;
  transformCurveAtTime(t,startpoint,c1,c2,endpoint);
  transform = decorateTransform * transform;
}

void XWTikzState::decorateTransformLine(const QPointF & c,
                                        double decorateangle,
                                        double decoratedistance)
{
  double x = c.x() + decoratedistance * cos(decorateangle);
  double y = c.y() + decoratedistance * sin(decorateangle);
  transform.translate(x,y);
  transform.rotate(decorateangle);
  transform = decorateTransform * transform;
}

void XWTikzState::doNodes()
{
  for (int i = 0; i < nodes.size(); i++)
  {
    XWTikzShape * node = nodes[i];
    node->setFontSize(fontSize);
    node->init();
    switch (node->nodeType)
    {
      default:
        break;

      case XW_TIKZ_NODE:
      case XW_TIKZ_CIRCUIT_SYMBOL:
        if (!myNode || myNode->nodeType != XW_TIKZ_CHILD)
        {
          node->state->anchorNode();
          node->state->transformNode();
        }
        else
        {
          node->state->anchorChild();
          node->state->transformChild();
          node->state->edgeFromParentFinished = edgeFromParentFinished;
        }
        node->doShape(true);
        break;

      case XW_TIKZ_LABEL:
      case XW_TIKZ_PIN:
      case XW_TIKZ_INFO:
        if (myNode)
        {
          computeAngle();          
          int counta = (int)(labelAngle);
          if (counta == PGFcenter)
          {
            double x = pathLast.x() + myNode->centerPos.x();
            double y = pathLast.y() + myNode->centerPos.y();
            node->state->shift(x, y);
            node->state->anchor = PGFcenter;
          }            
          else
          {
            if (absolute)
            {
              anchor = -1;
              anotherPoint.setX(cos(labelAngle));
              anotherPoint.setX(sin(labelAngle));
              myNode->doShape(false);
              QPointF p = map(myNode->anchorPos);
              node->state->computeDirection();
              double x = p.x() + labelDistance * cos(labelAngle);
              double y = p.y() + labelDistance * sin(labelAngle);
              node->state->shift(x, y);
            }
            else
            {
              if (labelAngle < 360)
              {
                anchor = -1;
                anotherPoint.setX(cos(labelAngle));
                anotherPoint.setX(sin(labelAngle));
              }
              myNode->doShape(false);
              double x = pathLast.x() + labelDistance * cos(labelAngle);
              double y = pathLast.y() + labelDistance * sin(labelAngle);
              node->state->shift(x, y);
              if (myNode->anchorPos == myNode->centerPos)
                node->state->computeDirection();
              else
              {
                QPointF p1 = map(myNode->centerPos);
                QPointF p2 = map(myNode->anchorPos);
                QVector2D v(p1.x() - p2.x(), p1.y() - p2.y());
                v.normalize();
                x = v.x() * labelDistance;
                y = v.y() * labelDistance;
                if (isAutoSet)
                  node->state->setAutoAnchor(y, -x);
              }
            }
          }
        }
        node->doShape(true);  
        if (node->nodeType == XW_TIKZ_PIN)
          graphic->doEveryPinEdge(this);
        break;

      case XW_TIKZ_CHILD:
        anchor = growthParentAnchor;
        node->state->anchorChild();
        node->state->anchorNode();
        node->state->transformChild();
        node->doShape(true);        
        break;
    }

    if (parentNode && 
        myNode && 
        (myNode->nodeType == XW_TIKZ_CHILD || 
         myNode->nodeType == XW_TIKZ_NODE ||
         myNode->nodeType == XW_TIKZ_PIN) &&
        edgeFromParentFinished)
    {
      XWTikzState * state = save(true);
      state->myNode = myNode;
      state->parentNode = parentNode;
      switch (pictureType)
      {
        default:
          break;

        case PGFmindmap:
        case PGFsmallmindmap:
        case PGFlargemindmap:
        case PGFhugemindmap:
          state->endRadius = myNode->centerPos.x() - myNode->westPos.x();
          state->startRadius = parentNode->centerPos.x() - parentNode->westPos.x();
          state->setDecoration(PGFcircleconnectionbar);
          {
            double x = state->startRadius;
            if (x > state->endRadius)
              x = state->endRadius;
            state->decorationSegmentAmplitude = 0.175 * x;
            state->edge_from_parent = &XWTikzState::edgeFromParentCircleConnectionBarSwitch;
          }
          break;
      }
      graphic->doEdgeFromParent(state);
      graphic->doEdgeFromParentPath(state);
      state = restore();
    }
  }
}

void XWTikzState::edgeFromParentAcceptingByArrow()
{
  endArrow = new XWTikzArrowSpecification(graphic,this);
  endArrow->setArrow(PGFarrowdefault);
  lineWidth = 2 * lineWidth + 0.6;
  innerLineWidth = 0.6;
  graphic->doEveryAcceptingByArrow(this);
  QPointF p = getParentAnchor(-1);
  moveTo(p);
  p = getChildAnchor(-1);
  lineTo(p);
}

void XWTikzState::edgeFromParentCircleConnectionBarSwitch()
{
  QPointF pa = getParentAnchor(-1);
  QPointF ca = getChildAnchor(-1);
  moveTo(pa);
  lineTo(ca); 
  resetTransform();
  setFill(false);
  setDraw(false);
  setShadingName(PGFcircleconnectionbarswitchcolor);
  
  pa = getParentAnchor(PGFcenter);
  ca = getChildAnchor(PGFcenter);
  QVector2D v(pa.x() - ca.x(), pa.y() - ca.y());
  v.normalize();
  QPointF vec(v.x(), v.y());
  QPointF p(v.x()*startRadius, v.y()*startRadius);
  QPointF start = pa + p;
  p.setX(-v.x()*endRadius);
  p.setY(-v.y()*endRadius);
  QPointF end = ca + p;
  end = start - end;
  setMatrix(v.x(),v.y(),v.y(),-v.x(),start.x(),start.y());
  p = map(end);
  double length = p.x();
  setMatrix(v.x(),v.y(),-v.y(),v.x(),start.x(),start.y());
  double y = startRadius;
  double ya = endRadius;
  if (y < ya)
    y = ya;
  y *= 0.01992528;
  scale(1,y);

  QPointF pll(-startRadius,-50.1875);
  QPointF pur(1,50.1875);
  XWTikzState * state = save(false);
  state->addRectangle(pll, pur);
  state->setFillColor(fromColor);
  state = restore();

  QPointF cll(length,-50.1875);
  QPointF cur(length - 1,50.1875);
  state = save(false);
  state->addRectangle(cll, cur);
  state->setFillColor(toColor);
  state = restore();

  scale(length * 0.009962,1);
  shift(-50.1875, 0);
}

void  XWTikzState::edgeFromParentDefault()
{
  QPointF p = getParentAnchor(-1);
  moveTo(p);
  p = getChildAnchor(-1);
  lineTo(p);
}

void XWTikzState::edgeFromParentForkDown()
{
  operations << XW_TIKZ_MOVETO;
  QPointF p0 = getParentAnchor(PGFsouth);
  if (points.size() <= 0)
    firstOnPath = p0;
  points << p0;

  double x = p0.x();
  double y = p0.y() - 0.5 * levelDistance;
  QPointF p1(x,y);
  operations << XW_TIKZ_LINETO;
  if (points.size() < 2)
    secondOnPath = p1;
  points << p1;

  QPointF p = getChildAnchor(PGFnorth);
  QPointF p2(p.x(), y);
  QPointF p3(p.x(), p.y());
  operations << XW_TIKZ_LINETO;
  points << p2;
  operations << XW_TIKZ_LINETO;
  points << p3;

  secondLastOnPath = p2;
  lastOnPath = p3;
}

void XWTikzState::edgeFromParentForkLeft()
{
  operations << XW_TIKZ_MOVETO;
  QPointF p0 = getParentAnchor(PGFwest);
  if (points.size() <= 0)
    firstOnPath = p0;
  points << p0;

  double x = p0.x() - 0.5 * levelDistance;
  double y = p0.y();
  QPointF p1(x,y);
  operations << XW_TIKZ_LINETO;
  if (points.size() < 2)
    secondOnPath = p1;
  points << p1;

  QPointF p = getChildAnchor(PGFeast);
  QPointF p2(p.x(), y);
  QPointF p3(p.x(), p.y());
  operations << XW_TIKZ_LINETO;
  points << p2;
  operations << XW_TIKZ_LINETO;
  points << p3;

  secondLastOnPath = p2;
  lastOnPath = p3;
}

void XWTikzState::edgeFromParentForkRight()
{
  operations << XW_TIKZ_MOVETO;
  QPointF p0 = getParentAnchor(PGFeast);
  if (points.size() <= 0)
    firstOnPath = p0;
  points << p0;

  double x = p0.x() + 0.5 * levelDistance;
  double y = p0.y();
  QPointF p1(x,y);
  operations << XW_TIKZ_LINETO;
  if (points.size() < 2)
    secondOnPath = p1;
  points << p1;

  QPointF p = getChildAnchor(PGFwest);
  QPointF p2(p.x(), y);
  QPointF p3(p.x(), p.y());
  operations << XW_TIKZ_LINETO;
  points << p2;
  operations << XW_TIKZ_LINETO;
  points << p3;

  secondLastOnPath = p2;
  lastOnPath = p3;
}

void XWTikzState::edgeFromParentForkUp()
{
  operations << XW_TIKZ_MOVETO;
  QPointF p0 = getParentAnchor(PGFnorth);
  if (points.size() <= 0)
    firstOnPath = p0;
  points << p0;

  double x = p0.x();
  double y = p0.y() + 0.5 * levelDistance;
  QPointF p1(x,y);
  operations << XW_TIKZ_LINETO;
  if (points.size() < 2)
    secondOnPath = p1;
  points << p1;

  QPointF p = getChildAnchor(PGFsouth);
  QPointF p2(p.x(), y);
  QPointF p3(p.x(), p.y());
  operations << XW_TIKZ_LINETO;
  points << p2;
  operations << XW_TIKZ_LINETO;
  points << p3;

  secondLastOnPath = p2;
  lastOnPath = p3;
}

void XWTikzState::edgeFromParentInitialByArrow()
{
  endArrow = new XWTikzArrowSpecification(graphic,this);
  endArrow->setArrow(PGFarrowdefault);
  lineWidth = 2 * lineWidth + 0.6;
  innerLineWidth = 0.6;
  graphic->doEveryInitialByArrow(this);
  QPointF p = getChildAnchor(-1);
  moveTo(p);
  p = getParentAnchor(-1);
  lineTo(p);
}

bool XWTikzState::hitTestLine(const QPointF & p1, const QPointF & p2)
{
  QPointF c1 = map(p1);
  QPointF c2 = map(p2);
  double minx = c1.x();
  if (c2.x() < minx)
    minx = c2.x();

  if (mousePoint.x() < (minx - XW_TIKZ_HIT_DIST))
    return false;

  double maxx = c2.x();
  if (c1.x() > c2.x())
    maxx = c1.x();

  if (mousePoint.x() > (maxx + XW_TIKZ_HIT_DIST))
    return false;

  double miny = c1.y();
  if (c2.y() < miny)
    miny = c2.y();

  if (mousePoint.y() < (miny - XW_TIKZ_HIT_DIST))
    return false;

  double maxy = c2.y();
  if (c1.y() > c2.y())
    maxy = c1.y();

  if (mousePoint.y() > (maxy + XW_TIKZ_HIT_DIST))
    return false;
  
  if (c1.x() == c2.x())
  {
    if (qAbs(mousePoint.x() - c1.x()) <= XW_TIKZ_HIT_DIST)
      return true;

    return false;
  }

  if (c1.y() == c2.y())
  {
    if (qAbs(mousePoint.y() - c1.y()) <= XW_TIKZ_HIT_DIST)
      return true;

    return false;
  }

  double d = qAbs(mousePoint.y() - c1.y() - (mousePoint.x() - c1.x()) * (c2.y() - c1.y()) / (c2.x() - c1.x()));

  return d <= XW_TIKZ_HIT_DIST;
}

bool XWTikzState::hitTestPoint(const QPointF & p)
{
  QPointF c = map(p);
  pathLast = c;
  double x = qAbs(c.x() - mousePoint.x());
  double y = qAbs(c.y() - mousePoint.y());
  return (x <= XW_TIKZ_HIT_DIST) && (y <= XW_TIKZ_HIT_DIST);
}

bool XWTikzState::hitTestLines()
{
  for (int i = 0; i < points.size() - 1; i++)
  {
    QPointF p0 = points[i];
    QPointF p1 = points[i + 1];
    double minx = p0.x();
    if (p1.x() < minx)
      minx = p1.x();
    if (mousePoint.x() < (minx - XW_TIKZ_HIT_DIST))
      continue;

    double maxx = p1.x();
    if (p0.x() > p1.x())
      maxx = p0.x();

    if (mousePoint.x() > (maxx + XW_TIKZ_HIT_DIST))
      continue;

    double miny = p0.y();
    if (p1.y() < miny)
      miny = p1.y();

    if (mousePoint.y() < (miny - XW_TIKZ_HIT_DIST))
      continue;

    double maxy = p1.y();
    if (p0.y() > p1.y())
      maxy = p0.y();

    if (mousePoint.y() > (maxy + XW_TIKZ_HIT_DIST))
      continue;

    if (p0.x() == p1.x())
    {
      if (qAbs(mousePoint.x() - p0.x()) <= XW_TIKZ_HIT_DIST)
        return true;
    }
    else if (p0.y() == p1.y())
    {
      if (qAbs(mousePoint.y() - p0.y()) <= XW_TIKZ_HIT_DIST)
        return true;
    }
    else
    {
      double d = qAbs(mousePoint.y() - p0.y() - (mousePoint.x() - p0.x()) * (p1.y() - p0.y()) / (p1.x() - p0.x()));
      if (d <= XW_TIKZ_HIT_DIST)
        return true;
    }
  }
  return false;
}

void XWTikzState::init()
{
  pictureType = -1;
  circuitsSizeUnit = 7;
  circuitSymbolWidth = 5;
  circuitSymbolHeight = 5;
  annotation = 0;
  logicGateInvertedRadius = 2;
  logicGateInputSep = 3.557;
  logicGateAnchorsUseBoundingBox = false;
  andGateIECSymbol = "&";
  nandGateIECSymbol = "&";
  orGateIECSymbol = "\\geq1";
  norGateIECSymbol = "\\geq1";
  xorGateIECSymbol = "$=1$";
  xnorGateIECSymbol = "$=1$";
  notGateIECSymbol = "1";
  bufferGateIECSymbol = "1";
  logicGateIECSymbolAlign = PGFtop;
  lineWidth = 0.4;
  innerLineWidth = 0;
  anchor = PGFcenter;
  angle = 0;
  textWidth = 85;
  align = PGFleft;
  shape = PGFrectangle;
  fontSize = 10;
  innerXSep = 3.333;
  innerYSep = 3.333;
  outerXSep = 0.2;
  outerYSep = 0.2;
  minHeight = 30;
  minWidth = 40;
  shapeAspect = 2;
  shapeBorderUsesIncircle = true;
  shapeBorderRotate = 0;
  above = 0;
  below = 0;
  left = 0;
  right = 0;
  nodeTime = 0.5;
  isTimeSet = false;
  autoAnchor = 0;
  isAutoSet = false;
  isSwapSet = false;
  isSlopedSet = false;
  isAllowUpsideDown = false;
  location = -1;

  labelAngle = PGFabove;
  labelDistance = 0;
  absolute = true;

  roundedCorners = 0.0;
  xradius = 0.0;
  yradius = 0.0;

  at = 0;

  endIsSet = false;
  startIsSet = false;
  deltaIsSet = false;
  startAngle = 0;
  endAngle = 0;
  deltaAngle = 0;

  xstep = 28.4527559;
  ystep = 28.4527559;

  helpLines = false;

  bend = 0;
  bendPosIsSet = false;
  bendPos = 0;
  bendHeightIsSet = 0;
  bendAtStart = false;
  bendAtEnd = false;

  isDrawSet = false;

  isFillSet = false;
  isFillText = false;
  interiorRule = PGFnonzerorule;

  isClipSet = false;

  isPatternSet = false;
  patternName = -1;
  patternColor = Qt::black;

  isShadeSet = false;
  shadingName = -1;
  shadingAngle = 0;
  topColor = Qt::gray;
  bottomColor = Qt::white;
  middleColor = calulateColor(Qt::gray,0.5,Qt::white);
  leftColor = topColor;
  rightColor = bottomColor;
  ballColor = Qt::blue;
  lowerLeftColor = Qt::white;
  upperLeftColor = Qt::white;
  lowerRightColor = Qt::white;
  upperRightColor = Qt::white;
  innerColor = Qt::gray;
  outerColor = Qt::white;

  coreColor = Qt::white;

  pathFading = -1;
  fitFading = true;

  isDecorateSet = false;
  decoration = -1;
  pre = 0;
  prelength = 0;
  post = 0;
  postlength = 0;
  reversePath = false;
  pathHasCorners = false;
  decorateWidth = 0;
  decoratedAngle = 0;
  decoratedCompletedDistance = 0;
  decoratedRemainingDistance = 0;
  decoratedPathLength = 0;
  decoratedInputSegmentLength = 0;
  decoratedInputSegmentCompletedDistance = 0;
  decoratedInputSegmentRemainingDistance = 0;
  decorationSegmentAmplitude = 2.5;
  decorationSegmentLength = 10;
  decorationSegmentAngle = 45;
  decorationSegmentAspect = 0.5;
  metaDecorationSegmentAmplitude = 2.5;
  metaDecorationSegmentLength = 28.4527559;
  startRadius = 2.5;
  endRadius = 2.5;
  decorationAngle = 20;
  footLength = 10;
  strideLength = 30;
  footSep = 4;
  footAngle = 10;
  footOf = PGFhuman;
  textFormatDelimiters.append(QChar('|'));
  leftIndent = 0;
  rightIndent = 0;
  fitToPath = true;
  fitToPathStretchingSpaces = true;
  pathFromText = false;
  pathFromTextAngle = 0;
  fitTextToPath = false;
  scaleTextToPath = false;
  reverseText = false;
  groupLetters = false;
  shapeStartWidth = 2.5;
  shapeStartHeight = 2.5;
  shapeEndWidth = 2.5;
  shapeEndHeight = 2.5;
  shapeSep = 7.1132;
  betweenOrBy = PGFbetweencenters;
  shapeSloped = true;
  shapeScaled = false;
  shapeEvenlySpread = 0;

  isUseAsBoundingBoxSet = false;

  matrix = false;
  isMatrixAnchorSet = false;
  matrixAnchor = 0;
  columnSep = 0;
  rowSep = 0;
  curColumn = 0;
  curRow = 0;

  level = 0;
  levelDistance = 42.679;
  siblingDistance = 42.679;
  growAngle = -90;
  growLeft = 0;
  growRight = 0;
  missing = false;
  growthParentAnchor = PGFcenter;
  isChildAnchorSet = false;
  childAnchor = anchor;
  isParentAnchorSet = false;
  parentAnchor = anchor;
  currentChild = 0;
  childrenNumber = 0;
  isTransformChildSet = false;

  isContinue = false;
  handler = 0;
  tension = 0.5;
  barWidth = 10;
  barShift = 0;
  barIntervalWidth = 1;
  barIntervalShift = 0.5;
  mark = 0;
  markRepeat = 0;
  markPhase = 0;
  markSize = 2;
  markColor = Qt::black;
  isMarkColorSet = false;
  asNode = true;

  transformShape = false;

  arrowDefault = PGFto;
  startArrow = 0;
  endArrow = 0;
  isEnd = false;
  arrowLength = 0;
  arrowWidth = 0;
  arrowInset = 0;
  arrowAngle = 0;
  arrowLineWidth = 0;
  arrowSep = 0;
  shorteningDistance = 0;
  shortenStart = 0;
  shortenEnd = 0;
  arrowTotalLength = 0;
  arrowScaleLength = 0;
  arrowScaleWidth = 0;
  arrowArc = 180;
  arrowSlant = 0;
  arrowReversed = false;
  arrowHarpoon = false;
  arrowSwap = false;
  isArrowFillSet = false;
  isArrowDrawSet = false;
  arrowLineCap = PGFbutt;
  arrowLineJoin = PGFmiter;
  arrowFlex = 1;
  arrowFlexMode = -1;
  arrowBendMode = -1;
  preciseShortening = false;
  nextTip = 0;

  xVec = 28.4527559;
  yVec = 28.4527559;
  zVec = -20.1191366;

  drawOpacity = 1;
  fillOpacity = 1;

  samples = 25;
  domainStart = -5;
  domainEnd = 5;

  initialText = "start";

  pathMinX = 16000;
  pathMinY = 16000;
  pathMaxX = -16000;
  pathMaxY = -16000;

  aspect = 1.0;
  setAspect(aspect);
  trapeziumLeftAngle = 60;
  trapeziumRightAngle = 60;
  trapeziumStretches = true;
  trapeziumStretchesBody = true;

  regularPolygonSides = 5;

  starPoints = 5;
  starPointHeight = 14.2263779528;
  starPointRatio = 1.5;
  useStarRatio = true;

  isoscelesTriangleApexAngle = 45;
  isoscelesTriangleStretches = true;

  kiteUpperVertexAngle = 120;
  kiteLowerVertexAngle = 60;

  dartTipAngle = 45;
  dartTailAngle = 135;

  circularSectorAngle = 60;

  cylinderUsesCustomFill = true;
  cylinderEndFill = Qt::white;
  cylinderBodyFill = Qt::white;

  cloudPuffs = 10;
  cloudPuffArc = 135;
  cloudIgnoresAspect = true;
  cloudAnchorsUseEllipse = false;

  MGHAfill = -45;
  MGHAaspect = 1.5;
  MGHA = -45;

  starburstPoints = 17;
  starburstPointHeight = 14.2263779528;
  randomStarburst = 100;

  signalPointerAngle = 90;
  signalFrom = PGFnowhere;
  signalFromOpposite = -1;
  signalTo = PGFeast;
  signalToOpposite = -1;

  tapeBendTop = PGFinandout;
  tapeBendBottom = PGFinandout;
  tapeBendHeight = 5;

  magneticTapeTailExtend = 0;
  magneticTapeTail = 0.15;

  singleArrowTipAngle = 90;
  singleArrowHeadExtend = 14.2263779528;
  singleArrowHeadIndent = 0;

  doubleArrowTipAngle = 90;
  doubleArrowHeadExtend = 14.2263779528;
  doubleArrowHeadIndent = 0;

  arrowBoxTipAngle = 90;
  arrowBoxHeadExtend = 3.556594482;
  arrowBoxHeadIndent = 0;
  arrowBoxShaftWidth = 3.556594482;
  arrowBoxNorthArrow.setX(XW_TIKZ_FROM_BORDER);
  arrowBoxNorthArrow.setY(14.2263779528);
  arrowBoxSouthArrow.setX(XW_TIKZ_FROM_BORDER);
  arrowBoxSouthArrow.setY(14.2263779528);
  arrowBoxEastArrow.setX(XW_TIKZ_FROM_BORDER);
  arrowBoxEastArrow.setY(14.2263779528);
  arrowBoxWestArrow.setX(XW_TIKZ_FROM_BORDER);
  arrowBoxWestArrow.setY(14.2263779528);

  rectangleSplitParts = 4;
  rectangleSplitHorizontal = true;
  rectangleSplitIgnoreEmptyParts = true;
  rectangleSplitEmptyPartWidth = 10;
  rectangleSplitEmptyPartHeight = 10;
  rectangleSplitEmptyPartDepth = 0;
  rectangleSplitDrawSplits = true;
  rectangleSplitUseCustomFill = true;

  calloutPointerAnchorAspect = 1;

  calloutRelativePointer.setX(10.06);
  calloutRelativePointer.setY(-10.06);
  isCalloutAbsolutePointer = false;

  calloutPointerShorten = 0;
  calloutPointerWidth = 7.1131889764;
  calloutPointerArc = 15;
  calloutPointerStartSize.setX(XW_TIKZ_CPS_CALLOUT);
  calloutPointerStartSize.setY(0.2);
  calloutPointerEndSize.setX(XW_TIKZ_CPS_CALLOUT);
  calloutPointerEndSize.setY(0.1);
  calloutPointerSegments = 2;

  roundedRectangleArcLength = 180;
  roundedRectangleWestArc = PGFconvex;
  roundedRectangleLeftArc = -1;
  roundedRectangleEastArc = PGFconvex;
  roundedRectangleRightArc = -1;

  chamferedRectangleAngle = 45;
  chamferedRectangleXsep = 6.66;
  chamferedRectangleYsep = 6.66;

  siblingAngle = 20;

  mindmap = -1;
  fromColor = Qt::black;
  toColor = fromColor;
  spyUsing = 0;
  onNode = true;

  initialDistance = 27.27;
  acceptingDistance = 27.27;
  initialAngle = 180;
  acceptingAngle = 0;
  initialAnchor = PGFeast;
  acceptingAnchor = PGFwest;

  shadowScale = 1;
  shadowXShift = 0;
  shadowYShift = 0;

  toIn = 135;
  toOut = 45;
  inLooseness = 1;
  outLooseness = 1;
  inMin = 0;
  inMax = 10000;
  outMin = 0;
  outMax = 10000;
  toBend = 30;
  relative = true;
  isInControlSet = false;
  isOutControlSet = false;

  tokenDistance = 6.45;

  position = 0;
  startPosition = 0;
  endPosition = 0;
  step = 0;
  planeX.setX(1);
  planeX.setY(0);
  planeX.setZ(0);
  planeY.setX(0);
  planeY.setY(1);
  planeY.setZ(0);
  planeOrigin.setX(0);
  planeOrigin.setY(0);
  planeOrigin.setZ(0);

  tikzTimer = &XWTikzState::timerLine;
  before_code = NULL;
  after_code = NULL;

  to_path = &XWTikzState::toPathDefault;
  edge_from_parent = &XWTikzState::edgeFromParentDefault;
  nolinear_map = NULL;

  myNode = 0;
  myBox = 0;
  parentNode = 0;
  edgeFromParentFinished = false;
}

void XWTikzState::initPath()
{
  if (!isPath || (!startArrow && !endArrow))
    return ;

  if (operations.size() < 2 || points.size() < 2)
    return ;

  int n = points.size();
  if (startArrow)
  {
    shorteningDistance = shortenStart;
    arrowTotalLength = 0;
    startArrow->setup(this);
    if (shorteningDistance == 0)
      goto endshorten;

    switch (operations[1])
    {
      default:
        break;

      case XW_TIKZ_LINETO:
        {
          int i = 1;
          while (i < n)
          {
            if (points[i] != firstOnPath)
            {
              secondOnPath = points[i];
              break;
            }
            i++;
          }

          QPointF p = lineAtDistance(shorteningDistance, firstOnPath, secondOnPath);
          points[0] = p;
        }
        break;

      case XW_TIKZ_CURVETO:
        {
          setCurviLinearBezierCurve(firstOnPath,secondOnPath,thirdOnPath,fourthOnPath);
          double t = curviLinearDistanceToTime(shorteningDistance);
          QPointF p = curveAtTime(t,firstOnPath,secondOnPath,thirdOnPath,fourthOnPath);
          points[0] = p;
        }
        break;
    }
  }

endshorten:
  if (endArrow)
  {
    shorteningDistance = shortenEnd;
    arrowTotalLength = 0;
    endArrow->setup(this);
    if (shorteningDistance == 0)
      return ;

    int s = operations.size();
    if (operations[s - 1] != XW_TIKZ_CLOSE)
    {
      switch (operations[s - 1])
      {
        default:
          break;

        case XW_TIKZ_LINETO:
          {
            int i = n - 2;
            while (i > 0)
            {
              if (points[i] != lastOnPath)
              {
                secondLastOnPath = points[i];
                break;
              }
                
              i--;
            }

            QPointF p = lineAtDistance(shorteningDistance, lastOnPath, secondLastOnPath);
            points[n - 1] = p;
          }
          break;

        case XW_TIKZ_CURVETO:
          {
            setCurviLinearBezierCurve(lastOnPath,secondLastOnPath,thirdLastOnPath,fourthLastOnPath);
            double t = curviLinearDistanceToTime(shorteningDistance);
            QPointF p = curveAtTime(t,lastOnPath,secondLastOnPath,thirdLastOnPath,fourthLastOnPath);
            points[n - 1] = p;
          }
          break;
      }
    }
  }
}

void XWTikzState::intersectionsOfCurversLeft(const QPointF & p1,const QPointF & p2,
                                        const QPointF & p3,const QPointF & p4,
                                        const QPointF & p5,const QPointF & p6,
                                        const QPointF & p7,const QPointF & p8)
{
  double x2 = 0.5 * (p1.x() + p2.x());
  double y2 = 0.5 * (p1.y() + p2.y());
  double x3 = 0.25 * p1.x() + 0.5 * p2.x() + 0.25 * p3.x();
  double y3 = 0.25 * p1.y() + 0.5 * p2.y() + 0.25 * p3.y();
  double x4 = 0.125 * p1.x() + 0.375 * p2.x() + 0.375 * p3.x() + 0.125 * p4.x();
  double y4 = 0.125 * p1.y() + 0.375 * p2.y() + 0.375 * p3.y() + 0.125 * p4.y();

  QPointF p21(x2,y2);
  QPointF p31(x3,y3);
  QPointF p41(x4,y4);

  intersectionsOfCurvers(p1,p21,p31,p41,p5,p6,p7,p8);
}

void XWTikzState::intersectionsOfCurversRight(const QPointF & p1,const QPointF & p2,
                                        const QPointF & p3,const QPointF & p4,
                                        const QPointF & p5,const QPointF & p6,
                                        const QPointF & p7,const QPointF & p8)
{
  double x1 = 0.125 * p1.x() + 0.375 * p2.x() + 0.375 * p3.x() + 0.125 * p4.x();
  double y1 = 0.125 * p1.y() + 0.375 * p2.y() + 0.375 * p3.y() + 0.125 * p4.y();
  double x2 = 0.25 * p2.x() + 0.5 * p2.x() + 0.25 * p3.x();
  double y2 = 0.25 * p2.y() + 0.5 * p2.y() + 0.25 * p3.y();
  double x3 = 0.5 * p3.x() + 0.5 * p4.x();
  double y3 = 0.5 * p3.y() + 0.5 * p4.y();

  QPointF p11(x1,y1);
  QPointF p12(x2,y2);
  QPointF p13(x3,y3);
  intersectionsOfCurvers(p11,p12,p13,p4,p5,p6,p7,p8);
}

QPointF XWTikzState::intersectionOfCircles(const QPointF & c1,const QPointF & c2,
                                double r1,double r2,int s)
{
  double xa = c1.x();
  double ya = c1.y();
  double xc = r1;
  double xb = c2.x();
  double yb = c2.y();
  double yc = r2;

  int counta = 1;
  bool scaledown = true;
  while (scaledown)
  {
    scaledown = false;
    if (xc > 50)
      scaledown = true;

    if (yc > 50)
      scaledown = true;

    if (scaledown)
    {
      counta *= 2;
      xa /= 2;
      ya /= 2;      
      xb /= 2;
      yb /= 2;
      xc /= 2;
      yc /= 2;
    }
  }

  double d0 = xb - xa;
  double d1 = yb - ya;
  double d3 = d0 * d0 + d1 * d1;
  double d2 = sqrt(d3);
  double d5 = 1 / d2;
  double d4 = 0.5 * (d3 + xc * xc - yc * yc) * d5;
  double d7 = d4 * d4;
  double d6 = sqrt(xc * xc - d7);
  double d8 = d4 * d5;
  double d9 = d6 * d5;
  double x = 0;
  double y = 0;
  if (s == 1)
  {
    x = xa + d0 * d8 + d1 * d9;
    y = ya + d1 * d8 - d0 * d9;
  }
  else
  {
    x = xa + d0 * d8 - d1 * d9;
    y = ya + d1 * d8 + d0 * d9;
  }  

  x = x * counta;
  y = y * counta;

  QPointF p(x,y);
  return p;
}

void XWTikzState::intersectionsOfCurvers(const QPointF & p1,const QPointF & p2,
                                        const QPointF & p3,const QPointF & p4,
                                        const QPointF & p5,const QPointF & p6,
                                        const QPointF & p7,const QPointF & p8)
{
  double x = 16000;
  double y = 16000;
  double xa = -16000;
  double ya = -16000;

  if (p1.x() < x)
    x = p1.x();

  if (p1.y() < y)
    y = p1.y();

  if (p1.x() > xa)
    xa = p1.x();

  if (p1.y() > ya)
    ya = p1.y();

  if (p2.x() < x)
    x = p2.x();

  if (p2.y() < y)
    y = p2.y();

  if (p2.x() > xa)
    xa = p2.x();

  if (p2.y() > ya)
    ya = p2.y();

  if (p3.x() < x)
    x = p3.x();

  if (p3.y() < y)
    y = p3.y();

  if (p3.x() > xa)
    xa = p3.x();

  if (p3.y() > ya)
    ya = p3.y();

  if (p4.x() < x)
    x = p4.x();

  if (p4.y() < y)
    y = p4.y();

  if (p4.x() > xa)
    xa = p4.x();

  if (p4.y() > ya)
    ya = p4.y();

  double xb = 16000;
  double yb = 16000;
  double xc = -16000;
  double yc = -16000;

  if (p5.x() < xb)
    xb = p5.x();

  if (p5.y() < yb)
    yb = p5.y();

  if (p5.x() > xc)
    xc = p5.x();

  if (p5.y() > yc)
    yc = p5.y();

  if (p6.x() < xb)
    xb = p6.x();

  if (p6.y() < yb)
    yb = p6.y();

  if (p6.x() > xc)
    xc = p6.x();

  if (p6.y() > yc)
    yc = p6.y();

  if (p7.x() < xb)
    xb = p7.x();

  if (p7.y() < yb)
    yb = p7.y();

  if (p7.x() > xc)
    xc = p7.x();

  if (p7.y() > yc)
    yc = p7.y();

  if (p8.x() < xb)
    xb = p8.x();

  if (p8.y() < yb)
    yb = p8.y();

  if (p8.x() > xc)
    xc = p8.x();

  if (p8.y() > yc)
    yc = p8.y();

  if (xa >= xb)
  {
    if (x <= xc)
    {
      if (ya >= yb)
      {
        if (y <= yc)
        {
          double t1 = xc - xb;
          double t2 = yc - yb;
          double t3 = xa - x;
          double t4 = ya - y;
          if (t1 < XW_TIKZ_INTERSECTION_TOLERANCE)
          {
            if (t2 < XW_TIKZ_INTERSECTION_TOLERANCE)
            {
              if (t3 < XW_TIKZ_INTERSECTION_TOLERANCE)
              {
                if (t4 < XW_TIKZ_INTERSECTION_TOLERANCE)
                {
                  x = 0.25 * (x + xa + xb + xc);
                  y = 0.25 * (y + ya + yb + yc);
                  bool dup = false;
                  for (int i = 0; i < intersections.size(); i++)
                  {
                    if ((qAbs(x - intersections[i].x()) <  XW_TIKZ_INTERSECTION_TOLERANCE) && 
                        (qAbs(y - intersections[i].y()) < XW_TIKZ_INTERSECTION_TOLERANCE))
                    {
                      dup = true;
                      break;
                    }
                  }

                  if (!dup)
                  {
                    QPointF p(x,y);
                    intersections << p;
                  }
                }
              }
            }
          }
          else
          {
            intersectionsOfCurversLeft(p5,p6,p7,p8,p1,p2,p3,p4);
            intersectionsOfCurversRight(p5,p6,p7,p8,p1,p2,p3,p4);
          }
        }
      }
    }
  }
}

void XWTikzState::intersectionOfCurveAndLine(const QPointF & p1,const QPointF & p2,
                                  const QPointF & p3,const QPointF & p4,
                                  const QPointF & p5,const QPointF & p8)
{
  double x = 0.666666 * p5.x() + 0.333333 * p8.x();
  double y = 0.666666 * p5.y() + 0.333333 * p8.y();
  QPointF p6(x,y);

  x = 0.333333 * p5.x() + 0.666666 * p8.x();
  y = 0.333333 * p5.y() + 0.666666 * p8.y();
  QPointF p7(x,y);

  intersectionsOfCurvers(p1,p2,p3,p4,p5,p6,p7,p8);
}

void XWTikzState::intersectionOfLineAndCurve(const QPointF & p1,const QPointF & p4,
                                  const QPointF & p5,const QPointF & p6,
                                  const QPointF & p7,const QPointF & p8)
{
  double x = 0.666666 * p1.x() + 0.333333 * p4.x();
  double y = 0.666666 * p1.y() + 0.333333 * p4.y();
  QPointF p2(x,y);

  x = 0.333333 * p1.x() + 0.666666 * p4.x();
  y = 0.333333 * p1.y() + 0.666666 * p4.y();
  QPointF p3(x,y);

  intersectionsOfCurvers(p1,p2,p3,p4,p5,p6,p7,p8);
}

QPointF XWTikzState::intersectionOfLines(const QPointF & p1,const QPointF & p2,
                                         const QPointF & p3,const QPointF & p4)
{
  double xa = p2.x() - p1.x();
  double ya = p1.y() - p2.y();
  double counta = xa;
  if (counta < 0)
    counta = -counta;
  double countb = ya;
  if (countb < 0)
    countb = -countb;
  counta += countb;
  counta /= 65536;
  if (counta > 0)
  {
    xa /= counta;
    ya /= counta;
  }
  double xc = ya * p1.x() + xa * p1.y();

  double xb = p4.x() - p3.x();
  double yb = p3.y() - p4.y();
  counta = xa;
  if (counta < 0)
    counta = -counta;
  countb = yb;
  if (countb < 0)
    countb = -countb;
  counta += countb;
  counta /= 65536;
  if (counta > 0)
  {
    xb /= counta;
    yb /= counta;
  }
  double yc = yb * p3.x() + xb * p3.y();

  QTransform trans(ya,yb,xa,xb,0,0);
  trans = trans.inverted();

  QPointF p(xc,yc);

  return trans.map(p);
}

void XWTikzState::invokeDecorate()
{
  if (!isDecorateSet || operations.isEmpty() || points.isEmpty())
    return ;

  if (operations.size() == 1)
  {
    QPointF p = points[0];
    double x = p.x() + 0.0001;
    p.setX(x);

    operations << XW_TIKZ_LINETO;
    points << p;
  }

  QList<int> operationsA;
  QList<QPointF> pointsA;
  if (reversePath)
  {
    bool mvw = true;
    bool clw = false;
    int k = points.size() - 1;
    for (int i = operations.size() - 1; i > 0; i--)
    {
      switch (operations[i])
      {
        default:
          operationsA << operations[i];
          break;

        case XW_TIKZ_MOVETO:
          if (mvw)
          {
            operationsA << XW_TIKZ_MOVETO;
            pointsA << points[k--];
          }
          if (clw)
          {
            clw = false;
            operationsA << XW_TIKZ_CLOSE;
          }
          mvw = false;
          break;

        case XW_TIKZ_LINETO:
          if (mvw)
          {
            mvw = false;
            operationsA << XW_TIKZ_MOVETO;
            pointsA << points[k--];
          }
          operationsA << XW_TIKZ_LINETO;
          pointsA << points[k--];
          break;

        case XW_TIKZ_CURVETO:
          if (mvw)
          {
            mvw = false;
            operationsA << XW_TIKZ_MOVETO;
            pointsA << points[k--];
          }
          operationsA << XW_TIKZ_CURVETO;
          pointsA << points[k--];
          pointsA << points[k--];
          pointsA << points[k--];
          break;

        case XW_TIKZ_CLOSE:
          clw = true;
          break;
      }
    }
  }
  else
  {
    operationsA = operations;
    pointsA = points;
  }

  decoratedPathFirst = points[0];
  decoratedPathLast = points[points.size() - 1];

  operations.clear();
  points.clear();

  transform.reset();

  graphic->doDecoration(this);

  decoratedRemainingDistance = 0;
  QPointF p1,p2,p3,p4;
  int k = 0;
  for (int i = 0; i < operationsA.size(); i++)
  {
    switch (operationsA[i])
    {
      default:
        break;

      case XW_TIKZ_MOVETO:
        p1 = pointsA[k++];
        break;

      case XW_TIKZ_LINETO:
        p2 = pointsA[k++];
        decoratedRemainingDistance += decorateLineLength(p1,p2);
        p1 = p2;
        break;

      case XW_TIKZ_CURVETO:
        p2 = pointsA[k++];
        p3 = pointsA[k++];
        p4 = pointsA[k++];
        decoratedRemainingDistance += decorateCurveLength(p1,p2,p3,p4);
        p1 = p4;
        break;
    }
  }

  decoratedPathLength = decoratedRemainingDistance;
  decoratedCompletedDistance = 0;

  if (before_code != NULL)
    (this->*(before_code))();

  XWTikzDecoration mydecoration(decoration);

  QPointF mp;
  double inputsegmenttime = 0;
  autoEndOnLength = 0;
  autoCornerOnLength = 0;
  for (int i = 0; i < operationsA.size(); i++)
  {
    inputsegmenttime = 0;
    decoratedInputSegmentLength = 0;
    decoratedAngle = 0;
    decoratedInputSegmentCompletedDistance = 0;
    decoratedInputSegmentRemainingDistance = 0;
    curOperation = operationsA[i];
    switch (curOperation)
    {
      default:
        operations << operationsA[i];
        break;

      case XW_TIKZ_MOVETO:
        p1 = pointsA[k++];
        operations << XW_TIKZ_MOVETO;
        points << p1;
        pathLast = p1;
        p2 = p1;
        p3 = p1;
        p4 = p1;
        mp = p1;
        break;

      case XW_TIKZ_LINETO:
        p4 = pointsA[k++];
        p2.setX(p1.x() + 0.333333 * (p4.x() - p1.x()));
        p2.setY(p1.y() + 0.333333 * (p4.y() - p1.y()));
        p3.setX(p1.x() + 0.666666 * (p4.x() - p1.x()));
        p3.setY(p1.y() + 0.666666 * (p4.y() - p1.y()));
        decorateInputSegmentFirst = p1;
        decorateInputSegmentLast = p4;
        decoratedAngle = angleBetweenPoints(p1,p4);
        decoratedInputSegmentRemainingDistance = decorateLineLength(p1,p4);
        decoratedInputSegmentLength = decoratedInputSegmentRemainingDistance;
        if ((pre > 0) && (prelength > 0) && (decoratedCompletedDistance < prelength))
        {
          XWTikzDecoration predecoration(pre);
          while (decoratedCompletedDistance < prelength)
          {
            decorateTransformLine(p1,decoratedAngle,decoratedInputSegmentCompletedDistance);
            if (predecoration.isNullState())
              predecoration.doInitState(this);
            else
              predecoration.doCurrentState(this);

            decoratedCompletedDistance += decorateWidth;
            decoratedRemainingDistance -= decorateWidth;
            decoratedInputSegmentCompletedDistance += decorateWidth;
            decoratedInputSegmentRemainingDistance -= decorateWidth;
          }
        }
        while (decoratedInputSegmentRemainingDistance > 0)
        {
          if ((post > 0) && (postlength > 0) && (decoratedRemainingDistance <= postlength))
          {
            break;
          }

          decorateTransformLine(p1,decoratedAngle,decoratedInputSegmentCompletedDistance);
          if (mydecoration.isNullState())
            mydecoration.doInitState(this);
          else
            mydecoration.doCurrentState(this);

          decoratedCompletedDistance += decorateWidth;
          decoratedRemainingDistance -= decorateWidth;
          decoratedInputSegmentCompletedDistance += decorateWidth;
          decoratedInputSegmentRemainingDistance -= decorateWidth;
        }
        if ((post > 0) && (postlength > 0) && (decoratedRemainingDistance <= postlength))
        {
          XWTikzDecoration postdecoration(post);
          while (decoratedInputSegmentRemainingDistance > 0)
          {
            decorateTransformLine(p1,decoratedAngle,decoratedInputSegmentCompletedDistance);
            if (postdecoration.isNullState())
              postdecoration.doInitState(this);
            else
              postdecoration.doCurrentState(this);

            decoratedCompletedDistance += decorateWidth;
            decoratedRemainingDistance -= decorateWidth;
            decoratedInputSegmentCompletedDistance += decorateWidth;
            decoratedInputSegmentRemainingDistance -= decorateWidth;
          }
        }
        break;

      case XW_TIKZ_CURVETO:
        p2 = pointsA[k++];
        p3 = pointsA[k++];
        p4 = pointsA[k++];
        decorateInputSegmentFirst = p1;
        decorateInputSegmentLast = p4;
        decoratedAngle = angleBetweenPoints(p1,p4);        
        decoratedInputSegmentRemainingDistance = decorateCurveLength(p1,p2,p3,p4);
        decoratedInputSegmentLength = decoratedInputSegmentRemainingDistance;
        if ((pre > 0) && (prelength > 0) && (decoratedCompletedDistance < prelength))
        {
          XWTikzDecoration predecoration(pre);
          while (decoratedCompletedDistance < prelength)
          {
            if (inputsegmenttime > 1)
              inputsegmenttime = inputsegmenttime - 1;
            decorateTransformCurve(inputsegmenttime,p1,p2,p3,p4);
            if (predecoration.isNullState())
              predecoration.doInitState(this);
            else
              predecoration.doCurrentState(this);

            decoratedCompletedDistance += decorateWidth;
            decoratedRemainingDistance -= decorateWidth;
            decoratedInputSegmentCompletedDistance += decorateWidth;
            decoratedInputSegmentRemainingDistance -= decorateWidth;
            decorateMoveAlongInputSegmentCurve(decorateWidth,
                                               decoratedInputSegmentLength,decoratedAngle,
                                               inputsegmenttime,p1,p2,p3,p4);
          }
        }
        while (decoratedInputSegmentRemainingDistance > 0)
        {
          if ((post > 0) && (postlength > 0) && (decoratedRemainingDistance <= postlength))
          {
            break;
          }
          if (inputsegmenttime > 1)
            inputsegmenttime = inputsegmenttime - 1;
          decorateTransformCurve(inputsegmenttime,p1,p2,p3,p4);
          if (mydecoration.isNullState())
            mydecoration.doInitState(this);
          else
            mydecoration.doCurrentState(this);

          decoratedCompletedDistance += decorateWidth;
          decoratedRemainingDistance -= decorateWidth;
          decoratedInputSegmentCompletedDistance += decorateWidth;
          decoratedInputSegmentRemainingDistance -= decorateWidth;
          decorateMoveAlongInputSegmentCurve(decorateWidth,
                                          decoratedInputSegmentLength,decoratedAngle,
                                          inputsegmenttime,p1,p2,p3,p4);
        }
        if ((post > 0) && (postlength > 0) && (decoratedRemainingDistance <= postlength))
        {
          XWTikzDecoration postdecoration(post);
          while (decoratedInputSegmentRemainingDistance > 0)
          {
            if (inputsegmenttime > 1)
              inputsegmenttime = inputsegmenttime - 1;
            decorateTransformCurve(inputsegmenttime,p1,p2,p3,p4);
            if (postdecoration.isNullState())
              postdecoration.doInitState(this);
            else
              postdecoration.doCurrentState(this);

            decoratedCompletedDistance += decorateWidth;
            decoratedRemainingDistance -= decorateWidth;
            decoratedInputSegmentCompletedDistance += decorateWidth;
            decoratedInputSegmentRemainingDistance -= decorateWidth;
            decorateMoveAlongInputSegmentCurve(decorateWidth,
                                          decoratedInputSegmentLength,decoratedAngle,
                                          inputsegmenttime,p1,p2,p3,p4);
          }
        }
        break;

      case XW_TIKZ_CLOSE:
        p2.setX(p1.x() + 0.333333 * (mp.x() - p4.x()));
        p2.setY(p1.y() + 0.333333 * (mp.y() - p4.y()));
        p3.setX(p1.x() + 0.666666 * (mp.x() - p4.x()));
        p3.setY(p1.y() + 0.666666 * (mp.y() - p4.y()));
        decorateInputSegmentFirst = p4;
        decorateInputSegmentLast = mp;
        decoratedAngle = angleBetweenPoints(p4,mp);
        decoratedInputSegmentRemainingDistance = decorateLineLength(p4,mp);
        decoratedInputSegmentLength = decoratedInputSegmentRemainingDistance;
        while (decoratedInputSegmentRemainingDistance > 0)
        {
          if ((post > 0) && (postlength > 0) && (decoratedInputSegmentRemainingDistance <= postlength))
          {
            break;
          }

          decorateTransformLine(p4,decoratedAngle,decoratedInputSegmentCompletedDistance);
          if (mydecoration.isNullState())
            mydecoration.doInitState(this);
          else
            mydecoration.doCurrentState(this);

          decoratedCompletedDistance += decorateWidth;
          decoratedRemainingDistance -= decorateWidth;
          decoratedInputSegmentCompletedDistance += decorateWidth;
          decoratedInputSegmentRemainingDistance -= decorateWidth;
        }
        if ((post > 0) && (postlength > 0) && (decoratedRemainingDistance <= postlength))
        {
          XWTikzDecoration postdecoration(post);
          while (decoratedInputSegmentRemainingDistance > 0)
          {
            decorateTransformLine(p4,decoratedAngle,decoratedInputSegmentCompletedDistance);
            if (postdecoration.isNullState())
              postdecoration.doInitState(this);
            else
              postdecoration.doCurrentState(this);

            decoratedCompletedDistance += decorateWidth;
            decoratedRemainingDistance -= decorateWidth;
            decoratedInputSegmentCompletedDistance += decorateWidth;
            decoratedInputSegmentRemainingDistance -= decorateWidth;
          }
        }
        break;
    }
  }

  if (decoratedRemainingDistance >= 1)
    mydecoration.doFinalState(this);

  if (after_code != NULL)
    (this->*(after_code))();
}

bool XWTikzState::isLinesIntersect(const QPointF & p1,
                        const QPointF & p2,
                        const QPointF & p3,
                        const QPointF & p4)
{
  double xc = p4.x() - p3.x();
  double yc = p4.y() - p3.y();
  double xa = p2.x() - p1.x();
  double ya = p2.y() - p1.y();
  double xb = p3.x() - p1.x();
  double yb = p3.y() - p1.y();

  int counta = (int)(xa / 16384);
  int countb = (int)(xb / 16384);
  int countc = (int)(ya / 16384);
  int countd = (int)(yb / 16384);
  counta *= countd;
  countc *= countb;
  counta -= countc;
  int tempcnta = counta;
  counta = (int)(xc / 16384);
  countc = (int)(yc / 16384);
  countd *= counta;
  countb *= countc;
  countd -=  countb;
  int tempcntb = countd;
  countb = (int)(xa / 16384);
  countd = (int)(ya / 16384);
  counta *= countd;
  countc *= countb;
  counta -= countc;
  bool s = false;
  bool t = false;
  if (counta != 0)
  {
    if (tempcnta == 0)
      s = true;
    else
    {
      if (tempcnta > 0)
      {
        if (counta >= tempcnta)
          s = true;
      }
      else
      {
        if (counta <= tempcnta)
          s = true;
      }
    }

    if (tempcntb == 0)
      t = true;
    else
    {
      if (tempcntb > 0)
      {
        if (counta >= tempcntb)
          t = true;
      }
      else
      {
        if (counta <= tempcntb)
          t = true;
      }
    }
  }

  return s && t;
}

QPointF XWTikzState::lineAtDistance(double t, const QPointF & p1,const QPointF & p2)
{
  double x = p2.x() - p1.x();
  double y = p2.y() - p1.y();
  double d = sqrt(x * x + y * y);
  x = x / d;
  y = y / d;
  double xa = t * x;
  double ya = t * y;

  x = p1.x() + xa;
  y = p1.y() + ya;

  QPointF p(x,y);

  return p;
}

QPointF XWTikzState::lineAtTime(double t, const QPointF & p1,const QPointF & p2)
{
  double x = p1.x() + t * (p2.x() - p1.x());
  double y = p1.y() + t * (p2.y() - p1.y());

  QPointF p(x,y);

  return p;
}

int XWTikzState::mathDefaultZ()
{
  QDateTime ldt = QDateTime::currentDateTime().toLocalTime();
  int minutesA = ldt.time().hour() * 60 + ldt.time().minute();
  int yearA = ldt.date().year();
  return minutesA * yearA;
}

int XWTikzState::mathGeneratePseudoRandomNumber(int z)
{
  int m = 2147483647;
  int a = 69621;
  int r = 23902;
  int q = 30845;
  int counta = z;
  int countb = z;
  int countc = q;

  counta /= countc;
  counta *= -countc;
  counta += countb;
  countc = a;
  counta *= countc;

  countc = q;
  countb /= countc;
  countc = r;
  countb *= countc;

  counta -= countb;

  if (counta < 0)
  {
    countb = m;
    counta += countb;
  }

  return counta;
}

double XWTikzState::mathrand(int z)
{
  int counta = mathGeneratePseudoRandomNumber(z);
  int countb = counta;
  countb /= 200001;
  countb *= -200001;
  countb += counta;
  countb += 1000000;
  if (countb < 0)
    countb += -1000000;
  else
    countb += 1000000;

  QString str = QString("%1").arg(countb);
  str = str.right(4);
  QString tmp;
  if (countb < 0)
  {
    tmp = "-";
    tmp += str[2];
    tmp += ".";
    tmp += str[3];
  }
  else
  {
    tmp += str[1];
    tmp += ".";
    tmp += str[2];
    tmp += str[3];
  }

  return tmp.toDouble();
}

int XWTikzState::mathRandomInteger(int l, int u, int z)
{
  int counta = l;
  int countb = u;
  int countc = countb;
  if (counta > countb)
  {
    countb = counta;
    counta = countc;
  }
  countc++;
  countc -= counta;
  counta = mathGeneratePseudoRandomNumber(z);
  countb = counta;
  countb /= countc;
  countb *= -countc;
  counta += countb;
  counta += l;
  return counta;
}

double XWTikzState::mathrnd(int z)
{
  int counta = mathGeneratePseudoRandomNumber(z);
  int countb = counta;
  countb /= 100001;
  countb *= -100001;
  countb += counta;
  countb += 1000000;
  QString str = QString("%1").arg(countb);
  str = str.right(2);
  QString tmp2 = str[0];
  QString tmp3 = str[1];
  str = QString("%1.%2").arg(tmp2).arg(tmp3);
  return str.toDouble();
}

void XWTikzState::quadraticCurveTo(const QPointF & c1,const QPointF & endpoint)
{
  fourthLastOnPath = pathLast;
  QPointF p1 = map(c1);
  QPointF p2 = map(endpoint);
  double xb = p2.x();
  double yb = p2.y();
  double xc = 0.6666666 * p1.x();
  double yc = 0.6666666 * p1.y();
  double xa = 0.33333333 * xb + xc;
  double ya = 0.33333333 * yb + yc;
  xc += 0.33333333 * pathLast.x();
  yc += 0.33333333 * pathLast.y();
  
  operations << XW_TIKZ_CURVETO;
  p1.setX(xc);
  p1.setY(yc);
  p2.setX(xa);
  p2.setY(ya);
  pathLast.setX(xb);
  pathLast.setY(yb);

  if (points.size() < 4)
  {
    secondOnPath = p1;
    thirdOnPath = p2;
    fourthOnPath = pathLast;
  }

  points << p1;
  points << p2;
  points << pathLast;

  thirdLastOnPath = p1;
  secondLastOnPath = p2;
  lastOnPath = pathLast;
}

void XWTikzState::rectangle(const QPointF & ll,double w, double h)
{
  QPointF p2,p3,p4;
  p2.setX(ll.x() + w);
  p2.setY(ll.y());
  p3.setX(ll.x() + w);
  p3.setY(ll.y() + h);
  p4.setX(ll.x());
  p4.setY(ll.y() + h);
  pathLast = p3;
  QPointF p1 = map(ll);
  p2 = map(p2);
  p3 = map(p3);
  p4 = map(p4);

  if (points.size() < 2)
  {
    firstOnPath = p3;
    secondOnPath = p2;
  }

  secondLastOnPath = p1;
  lastOnPath =  p4;

  operations << XW_TIKZ_MOVETO;
  points << p3;

  operations << XW_TIKZ_LINETO;
  points << p2;

  operations << XW_TIKZ_LINETO;
  points << p1;

  operations << XW_TIKZ_LINETO;
  points << p4;

  operations << XW_TIKZ_CLOSE;
  operations << XW_TIKZ_MOVETO;
  points << p3;
}

QPointF XWTikzState::rotatePointAround(const QPointF & p,const QPointF & r,double d)
{
  double xc = p.x();
  double yc = p.y();
  double xa = r.x();
  double ya = r.y();
  double xb = xa;
  double yb = ya;
  double x = xc - xa;
  double y = yc - ya;
  double sineangle = sin(d);
  double cosineangle = cos(d);
  xa = cosineangle * x - sineangle * y;
  ya = sineangle * x + cosineangle * y;
  x = xb + xa;
  y = yb + ya;

  QPointF ret(x,y);
  return ret;
}

void XWTikzState::runCodes()
{
  for (int i = 0; i < cmds.size(); i++)
    cmds[i]->doPath(this, false);
}

void XWTikzState::setAutoAnchor(double x, double y)
{
  QVector2D v(x,y);
  v.normalize();

  if (autoAnchor == PGFleft)
  {
    if (v.x() > 0.05)
    {
      if (v.y() > 0.05)
        anchor = PGFsoutheast;
      else if (v.y() < - 0.05)
        anchor = PGFsouthwest;
      else
        anchor = PGFsouth;
    }
    else if (v.x() < -0.05)
    {
      if (v.y() > 0.05)
        anchor = PGFnortheast;
      else if (v.y() < - 0.05)
        anchor = PGFnorthwest;
      else
        anchor = PGFnorth;
    }
    else
    {
      if (v.y() > 0)
        anchor = PGFeast;
      else
        anchor = PGFwest;
    }
  }
  else
  {
    if (v.x() > 0.05)
    {
      if (v.y() > 0.05)
        anchor = PGFnorthwest;
      else if (v.y() < - 0.05)
        anchor = PGFnortheast;
      else
        anchor = PGFnorth;
    }
    else if (v.x() < -0.05)
    {
      if (v.y() > 0.05)
        anchor = PGFsouthwest;
      else if (v.y() < - 0.05)
        anchor = PGFsoutheast;
      else
        anchor = PGFsouth;
    }
    else
    {
      if (v.y() > 0)
        anchor = PGFwest;
      else
        anchor = PGFeast;
    }
  }
}

void XWTikzState::setCurviLinearBezierCurve(const QPointF & startpoint,
                                            const QPointF & c1, 
                                            const QPointF & c2,
                                            const QPointF & endpoint)
{
  curviLinearLineA = startpoint;
  double xa = -curviLinearLineA.x();
  double ya = -curviLinearLineA.y();
  curviLinearLineB = c1;
  double xb = -curviLinearLineB.x();
  double yb = -curviLinearLineB.y();
  double x = curviLinearLineB.x() + xa;
  double y = curviLinearLineB.y() + ya;
  double curvilinearlenab = veclen(x,y);
  curviLinearLineC = c2;
  double xc = -curviLinearLineC.x();
  double yc = -curviLinearLineC.y();
  x = curviLinearLineC.x() + xb;
  y = curviLinearLineC.y() + yb;
  double curvilinearlenbc = veclen(x,y);
  curviLinearLineD = endpoint;
  x = curviLinearLineD.x() + xc;
  y = curviLinearLineD.y() + yc;
  double curvilinearlencd = veclen(x,y);

  x = curvilinearlenab + curvilinearlenbc + curvilinearlencd;
  curviLinearTimeA = 1 / x;
  QPointF p = curveAtTime(curviLinearTimeA,curviLinearLineA,curviLinearLineB,curviLinearLineC,curviLinearLineD);
  xb = -p.x();
  yb = -p.y();
  x = p.x() + xa;
  y = p.y() + ya;
  curviLinearLengthA = veclen(x,y);
  if (curviLinearLengthA > 1)
  {
    curviLinearTimeA = curviLinearTimeA / curviLinearLengthA;
    p = curveAtTime(curviLinearTimeA,curviLinearLineA,curviLinearLineB,curviLinearLineC,curviLinearLineD);
    xb = -p.x();
    yb = -p.y();
    x = p.x() + xa;
    y = p.y() + ya;
    curviLinearLengthA = veclen(x,y);
  }

  p = curveAtTime(2 * curviLinearTimeA,curviLinearLineA,curviLinearLineB,curviLinearLineC,curviLinearLineD);
  xa = -p.x();
  ya = -p.y();
  x = p.x() + xb;
  y = p.y() + yb;
  curviLinearLengthB = veclen(x,y) + curviLinearLengthA;

  p = curveAtTime(4 * curviLinearTimeA,curviLinearLineA,curviLinearLineB,curviLinearLineC,curviLinearLineD);
  xb = -p.x();
  yb = -p.y();
  x = p.x() + xa;
  y = p.y() + ya;
  curviLinearLengthC = veclen(x,y) + curviLinearLengthB;

  p = curveAtTime(8 * curviLinearTimeA,curviLinearLineA,curviLinearLineB,curviLinearLineC,curviLinearLineD);
  x = p.x() + xb;
  y = p.y() + yb;
  curviLinearLengthD = veclen(x,y) + curviLinearLengthC;
}

void XWTikzState::timerCurver()
{
  transformCurveAtTime(nodeTime,fourthLastOnPath,thirdLastOnPath,secondLastOnPath,lastOnPath);
}

void XWTikzState::timerHVLine()
{
  if (nodeTime < 0.5)
  {
    double t = 2 * nodeTime;
    transformLineAtTime(t,thirdLastOnPath,secondLastOnPath);
  }
  else
  {
    double t = 2 * nodeTime - 1;
    transformLineAtTime(t,secondLastOnPath,lastOnPath);
  }
}

void XWTikzState::timerLine()
{
  transformLineAtTime(nodeTime,secondLastOnPath,lastOnPath);
}

void XWTikzState::timerVHLine()
{
  if (nodeTime < 0.5)
  {
    double t = 2 * nodeTime;
    transformLineAtTime(t,thirdLastOnPath,secondLastOnPath);
  }
  else
  {
    double t = 2 * nodeTime - 1;
    transformLineAtTime(t,secondLastOnPath,lastOnPath);
  }
}

void XWTikzState::toPathCurveTo()
{
  graphic->doEveryCurveTo(this);
  QPointF s = toStart->getAngle(toOut,this);
  QPointF t = toTarget->getAngle(toIn,this);
  if (relative)
  {
    QPointF computedstart = outControl;
    if (!isInControlSet)
    {
      QPointF c = s - t;
      double xa = c.x();
      double ya = c.y();
      if (xa < 0)
        xa = -c.x();
      if (ya < 0)
        ya = -c.y();

      QVector2D v(xa,ya);
      v.normalize();
      if (v.x() > v.y())
      {
        int counta = v.x();
        if (counta != 0)
        {
          counta /= 255;
          xa *= 16;
          xa /= counta;
          xa *= 16;
        }
      }
      else
      {
        int counta = v.y();
        if (counta != 0)
        {
          counta /= 255;
          ya *= 16;
          ya /= counta;
          xa = 16 * ya;
        }
      }
      double x = 0.3915 * xa;
      xa = outLooseness * x;
      double xb = inLooseness * x;
      ya = outMin;
      if (xa < ya)
        xa = ya;
      ya = outMax;
      if (xa > ya)
        xa = ya;
      ya = inMin;
      if (xb < ya)
        xb = ya;
      ya = inMax;
      if (xb > ya)
        xb = ya;

      computedstart = s;
      QTransform trans;
      trans.translate(xa * cos(toOut),xa * sin(toOut));
      computedstart = trans.map(computedstart);
    }

    QPointF computedend = inControl;
    if (!isOutControlSet)
    {
      QPointF c = s - t;
      double xa = c.x();
      double ya = c.y();
      if (xa < 0)
        xa = -c.x();
      if (ya < 0)
        ya = -c.y();

      QVector2D v(xa,ya);
      v.normalize();
      if (v.x() > v.y())
      {
        int counta = v.x();
        if (counta != 0)
        {
          counta /= 255;
          xa *= 16;
          xa /= counta;
          xa *= 16;
        }
      }
      else
      {
        int counta = v.y();
        if (counta != 0)
        {
          counta /= 255;
          ya *= 16;
          ya /= counta;
          xa = 16 * ya;
        }
      }
      double x = 0.3915 * xa;
      xa = outLooseness * x;
      double xb = inLooseness * x;
      ya = outMin;
      if (xa < ya)
        xa = ya;
      ya = outMax;
      if (xa > ya)
        xa = ya;
      ya = inMin;
      if (xb < ya)
        xb = ya;
      ya = inMax;
      if (xb > ya)
        xb = ya;

      computedend = t;
      QTransform trans;
      trans.translate(xa * cos(toIn),xa * sin(toIn));
      computedend = trans.map(computedend);
    }

    moveTo(s);
    curveTo(computedstart,computedend,t);
  }
  else
  {
    QTransform trans;
    trans.translate(s.x(),s.y());
    trans.rotate(toOut);
    trans.translate(-s.x(),-s.y());    
    QPointF c = trans.map(t);
    QPointF tostart = toStart->getBorder(c,this);

    trans.reset();
    trans.translate(t.x(),t.y());
    trans.rotate(toIn);
    trans.translate(-t.x(),-t.y());
    c = trans.map(s);
    QPointF totarget = toTarget->getBorder(c,this);

    c = s - t;
    double xa = c.x();
    double ya = c.y();
    if (xa < 0)
      xa = -c.x();
    if (ya < 0)
      ya = -c.y();

    QVector2D v(xa,ya);
    v.normalize();
    if (v.x() > v.y())
    {
      int counta = v.x();
      if (counta != 0)
      {
        counta /= 255;
        xa *= 16;
        xa /= counta;
        xa *= 16;
      }
    }
    else
    {
      int counta = v.y();
      if (counta != 0)
      {
        counta /= 255;
        ya *= 16;
        ya /= counta;
        xa = 16 * ya;
      }
    }
    double x = 0.3915 * xa;
    xa = outLooseness * x;
    double xb = inLooseness * x;
    ya = outMin;
    if (xa < ya)
      xa = ya;
    ya = outMax;
    if (xa > ya)
      xa = ya;
    ya = inMin;
    if (xb < ya)
      xb = ya;
    ya = inMax;
    if (xb > ya)
      xb = ya;

    double firstdistance = xa;
    double seconddistance = xb;

    QPointF a = s + t;
    QVector2D vv(a.x(),a.y());
    vv.normalize();
    double xc = vv.x();
    double yc = vv.y();
    xb = -vv.x();
    double yb = -vv.y();

    trans.reset();
    trans.setMatrix(xc,yc,0,yb,xc,0,s.x(),s.y(),1);
    QPointF computedstart(firstdistance * cos(toOut),firstdistance * sin(toOut));
    computedstart = trans.map(computedstart);
    trans.setMatrix(xc,yc,0,yb,xc,0,t.x(),t.y(),1);
    QPointF computedend(seconddistance * cos(toIn),seconddistance * sin(toIn));
    moveTo(tostart);
    curveTo(computedstart,computedend,totarget);
  }
}

void XWTikzState::toPathDefault()
{
  moveTo(toStart);
  lineTo(toTarget);
}

void XWTikzState::toPathMoveTo()
{
  moveTo(toTarget);
}

void XWTikzState::transformArrow(const QPointF & p1,const QPointF & p2)
{
  double xa = p2.x() - p1.x();
  double ya = p2.y() - p1.y();
  double d = sqrt(xa * xa + ya * ya);
  double x = xa / d;
  double y = ya / d;
  ya = -y;

  QTransform newtrans(x,y,ya,x,p2.x(),p2.y());
  transform = newtrans * transform;
}

void XWTikzState::transformArrowBend()
{
  switch (arrowBendMode)
  {
    default:
      break;

    case PGForthogonal:
      nolinear_map = &XWTikzState::curviLinearBezierOrthogonal;
      break;

    case PGFpolar:
      nolinear_map = &XWTikzState::curviLinearBezierPolar;
      break;
  }

  scale(-1,-1);
}

void XWTikzState::transformArrowCurved(XWTikzArrow * a,
                                  const QPointF & startpoint,
                                  const QPointF & c1, 
                                  const QPointF & c2,
                                  const QPointF & endpoint)
{
  setCurviLinearBezierCurve(startpoint,c1,c2,endpoint);
  if (arrowBendMode <= 0)
    arrowFlexMode = -1;

  switch (arrowFlexMode)
  {
    default:
      transformArrowRigit(a,a->tipEnd, a->backEnd);
      break;

    case PGFflexrot:
      transformArrowRigit(a,a->visualTipEnd,a->visualBackEnd);
      break;

    case PGFbend:
      transformArrowBend();
      break;
  }  
}

void XWTikzState::transformArrowRigit(XWTikzArrow * a,double s, double e)
{
  double x = s;
  double y = (e - x) * arrowFlex + x;
  if (x == y)
  {
    double xb = nextTip + a->backEnd - x;
    xb = curviLinearDistanceToTime(xb);
    QPointF ref;
    double xc,yc;
    if (xb == 0)
    {
      if (curviLinearLineA == curviLinearLineB)
      {
        if (curviLinearLineA == curviLinearLineC)
          ref = curviLinearLineD;
        else
          ref = curviLinearLineC;
        xc = curviLinearLineA.x();
        yc = curviLinearLineA.y();
        x = ref.x() - xc;
        y = ref.y() - yc;
        QVector2D v(x,y);
        v.normalize();
        x = -v.x();
        y = -v.y();
      }
      else
      {
        QPointF p = curveAtTime(xb,curviLinearLineA,curviLinearLineB,curviLinearLineC,curviLinearLineD);
        xc = p.x();
        yc = p.y();
        x = curveXB - curveXA;
        y = curveYB = curveYA;
        QVector2D v(x,y);
        v.normalize();
        x = v.x();
        y = v.y();
      }
    }
    else
    {
      QPointF p = curveAtTime(xb,curviLinearLineA,curviLinearLineB,curviLinearLineC,curviLinearLineD);
      xc = p.x();
      yc = p.y();
      x = curveXB - curveXA;
      y = curveYB - curveYA;
      QVector2D v(x,y);
      v.normalize();
      x = v.x();
      y = v.y();
    }

    QTransform newtrans(x,y,-y,x,xc,yc);
    transform = newtrans * transform;
  }
  else
  {
    double xb = nextTip + a->backEnd;
    double xc = xb;
    xc -= x;
    xb -= y;
    xc = curviLinearDistanceToTime(xc);
    xb = curviLinearDistanceToTime(xb);
    QPointF p1 = curveAtTime(xc,curviLinearLineA,curviLinearLineB,curviLinearLineC,curviLinearLineD);
    QPointF p2 = curveAtTime(xb,curviLinearLineA,curviLinearLineB,curviLinearLineC,curviLinearLineD);
    transformArrow(p1,p2);
  }

  shift(-s, 0);
}

void XWTikzState::transformArrowStraight(XWTikzArrow * a,
                           const QPointF & c1, 
                           const QPointF & c2)
{
  transformArrow(c1, c2);
  shift(-arrowTotalLength,0);   
  shift(-a->backEnd,0);   
}

void XWTikzState::transformChild()
{
  if (isTransformChildSet)
    return ;

  double xc = 0.5 * siblingDistance * (childrenNumber + 1);
  double x = xc * cos(growLeft) + (siblingDistance * currentChild) * cos(growRight);
  double y = xc * sin(growLeft) + (siblingDistance * currentChild) * sin(growRight);
  x += levelDistance * cos(growAngle);
  y += levelDistance * sin(growAngle);
  shift(x,y);
}

void XWTikzState::transformCurveAtTime(double t,const QPointF & startpoint,const QPointF & c1, 
                      const QPointF & c2,const QPointF & endpoint)
{
  QPointF d = curveAtTime(t,startpoint,c1,c2,endpoint);
  shift(d.x(),d.y());
  if (!transformShape)
  {
    QTransform trans(1,0,0,1,d.x(),d.y());
    transform = trans;
  }

  if (isSlopedSet)
  {
    double x = transform.m11() - transform.m21();
    double y = transform.m12() - transform.m22();
    if (!isAllowUpsideDown)
    {
      if (x < 0)
      {
        x = -x;
        y = -y;
      }
    }

    QVector2D v(x,y);
    v.normalize();

    QTransform trans(v.x(),v.y(),-v.y(),v.x(),0,0);
    transform = trans * transform;
  }
}

void XWTikzState::transformLineAtTime(double t, const QPointF & p1,const QPointF & p2)
{
  QPointF d = lineAtTime(t,p1,p2);
  shift(d.x(),d.y());
  if (!transformShape)
  {
    QTransform trans(1,0,0,1,d.x(),d.y());
    transform = trans;
  }

  if (isSlopedSet)
  {
    double x = p2.x() - p1.x();
    double y = p2.y() - p1.y();
    if (!isAllowUpsideDown)
    {
      if (x < 0)
      {
        x = -x;
        y = -y;
      }
    }

    QVector2D v(x,y);
    v.normalize();

    QTransform trans(v.x(),v.y(),-v.y(),v.x(),0,0);
    transform = trans * transform;
  }
}

void XWTikzState::transformTriangle(double xa,double ya,
                                    double xb,double yb,
                                    double x,double y)
{
  double a = xa - x;
  double b = ya - y;
  double c = xb - x;
  double d = yb - y;
  QTransform newtrans(a,b,c,d,x,y);
  transform = newtrans * transform;
}

double XWTikzState::veclen(double xA,double yA)
{
  double x = xA;
  double y = yA;
  if (x < 0)
    x = -x;

  if (y < 0)
    y = -y;

  if (x == 0)
    x = y;
  else
  {
    if (y != 0)
    {
      if (x > y)
      {
        double xa = x;
        x = y;
        y = xa;
      }

      double counta = 0;
      if (y > 10000)
        counta = 1500;
      else if (y > 1000)
        counta = 150;
      else if (y > 100)
        counta = 50;
      else
        counta = 1;

      x /= counta;
      y /= counta;

      x = x / y;
      double xa = x * x;
      double temp = xa;
      x = -0.01019210944 * xa + 0.04453994279;
      x = temp * x - 0.1195159052;
      x = temp * x + 0.4993615349;
      x = temp * x + 1.000012594;
      if (y < 0)
        y = -y;

      x = y * x;

      x *= counta;
    }
  }  

  return x;
}
