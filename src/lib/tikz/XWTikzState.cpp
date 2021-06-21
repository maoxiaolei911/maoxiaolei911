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
#include "XWPDFDriver.h"
#include "tikzcolor.h"
#include "XWTikzExpress.h"
#include "XWTikzCoord.h"
#include "XWTikzGraphic.h"
#include "XWTikzCommand.h"
#include "XWTikzExpress.h"
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
  isTarget = true;
  coords << p;
  QPointF p1 = p->getPoint(this);
  addEdge(p1);
}

void XWTikzState::addEdge(const QPointF & p)
{
  toTarget = transform.map(p);
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

void XWTikzState::addPoint(XWTikzCoord * p)
{
  coords << p;
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

void XWTikzState::addShift(double xA,double yA)
{
  double x = xA + transform.dx();
  double y = yA + transform.dy();
  transform.translate(x,y);
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

void XWTikzState::anchorMatrix()
{
  if (isMatrixAnchorSet)
    anchor = matrixAnchor;
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
  transform.translate(x,y);
}

void XWTikzState::closePath()
{
  operations << XW_TIKZ_CLOSE;
}

void XWTikzState::concat(XWTikzState * newstate)
{
  transform = transform * newstate->transform;
}

void XWTikzState::copy(XWTikzState * newstate,bool n)
{
  newstate->graphic = graphic;
  newstate->driver = driver;
  newstate->isPath = isPath;
  newstate->curLabel = curLabel++;
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
    newstate->isUseAsBoundingBoxSet = isUseAsBoundingBoxSet;
    newstate->startArrow = startArrow;
    newstate->endArrow = endArrow;
    newstate->leftExtend = leftExtend;
    newstate->rightExtend = rightExtend;
    newstate->shortenStartAdditional = shortenStartAdditional;
    newstate->shortenEndAdditional = shortenEndAdditional;

    newstate->drawOpacity = drawOpacity;
    newstate->fillOpacity = fillOpacity;

    newstate->isMatrixAnchorSet = isMatrixAnchorSet;
    newstate->matrixAnchor = matrixAnchor;

    newstate->isContinue = isContinue;
    newstate->handler = handler;
    newstate->tension = tension;
    newstate->mark = mark;
    newstate->markRepeat = markRepeat;
    newstate->markPhase = markPhase;
    newstate->markSize = markSize;

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
  newstate->edgeFromParent = edgeFromParent;

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

  newstate->pathType = pathType;

  newstate->mindmap = mindmap;

  newstate->tikzTimer = tikzTimer;

  newstate->transform = transform;
  newstate->transformShape = transformShape;

  newstate->toStart = toStart;
  newstate->toTarget = toTarget;

  newstate->position = position;
  newstate->startPosition = startPosition;
  newstate->endPosition = endPosition;
  newstate->step = step;

  newstate->markNode = markNode;

  newstate->to_path = to_path;
}

void XWTikzState::copyTransform(XWTikzState * newstate)
{
  newstate->transform = transform;
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
  coords << c1;
  QPointF p1 = c1->getPoint(this);
  coords.removeLast();
  coords << endpoint;
  QPointF p3 = endpoint->getPoint(this);
  coords << c2;
  QPointF p2 = c2->getPoint(this);
  coords.removeLast();
  curveTo(p1,p2,p3);
  tikzTimer = &XWTikzState::timerCurver;
}

void XWTikzState::curveTo(XWTikzCoord * c1,XWTikzCoord * endpoint)
{
  coords << c1;
  QPointF p1 = c1->getPoint(this);
  coords.removeLast();
  coords << endpoint;
  QPointF p3 = endpoint->getPoint(this);
  curveTo(p1,p1,p3);
}

void XWTikzState::curveTo(const QPointF & c1,const QPointF & c2,const QPointF & endpoint)
{
  QPointF p1 = transform.map(c1);
  QPointF p2 = transform.map(c2);
  QPointF p3 = transform.map(endpoint);

  operations << XW_TIKZ_CURVETO;
  points << p1;
  points << p2;
  points << p3;
  pathLast = p3;
  toStart = pathLast;
}

void XWTikzState::cycle()
{
  closePath();
}

QPointF XWTikzState::doAnchor(XWTikzState * stateA, XWTeXBox * box)
{
  XWTikzShape node(driver,box,this,XW_TIKZ_NODE);
  node.doShape(false);
  QPointF ret = node.anchorPos;
  if (stateA)
  {
    if (stateA->isTarget)
    {
      stateA->toTarget = ret;
      stateA->endRadius = node.centerPos.x() - node.westPos.x();
    }
    else
    {
      stateA->toStart = ret;
      stateA->startRadius = node.centerPos.x() - node.westPos.x();
    }
  }
  return ret;
}

void XWTikzState::doEdgeFromParentForkDown()
{
  edgeFromParent = true;
  to_path = &XWTikzState::edgeFromParentForkDown;
}

void XWTikzState::doEdgeFromParentForkLeft()
{
  edgeFromParent = true;
  to_path = &XWTikzState::edgeFromParentForkLeft;
}

void XWTikzState::doEdgeFromParentForkRight()
{
  edgeFromParent = true;
  to_path = &XWTikzState::edgeFromParentForkRight;
}

void XWTikzState::doEdgeFromParentForkUp()
{
  edgeFromParent = true;
  to_path = &XWTikzState::edgeFromParentForkUp;
}

void XWTikzState::doEdgeFromParentPath()
{
  graphic->doEdgeFromParentPath(this);
}

void XWTikzState::doToPath()
{
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

void XWTikzState::flush()
{
  if (driver && !operations.isEmpty() && !points.isEmpty())
  {
    invokeDecorate();
    initPath();
    if (edgeFromParent)
      doEdgeFromParentPath();
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
          driver->nonzeroFill();
        break;

      default:
        if (isClipSet)
          driver->evenoddClip();
        if (isFillSet)
          driver->evenoddFill();
        break;
    }

    if (!dashPattern.isEmpty())
      driver->setLineDash(dashPattern,dashPhase);

    if (isDrawSet)
    {
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

    if (isPath)
      drawArrows();

    if (isFillText)
      driver->fillText();

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

    doNodes();
  }

  dashPattern.clear();
  operations.clear();
  points.clear();
  coords.clear();
  plotStream.clear();
}

QPointF XWTikzState::getCenterPoint()
{
  if (at)
    return at->getPoint(this);

  return coords.last()->getPoint(this);
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

double XWTikzState::getValue(const QString & nameA)
{
  if (values.contains(nameA))
    return values[nameA];

  return 1.0;
}

void XWTikzState::getWidthAndHeight(double & w, double & h)
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

  w = pathMaxX - pathMinX;
  h = pathMaxY - pathMinY;
}

void XWTikzState::growCyclic()
{
  double a = siblingAngle * (-0.5 - 0.5 * childrenNumber + currentChild);
  transform.rotate(a);
  double x = levelDistance;
  double y = transform.dy();
  transform.translate(x,y);
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
}

bool XWTikzState::hitTestArc()
{
  points.clear();
  QPointF p = coords.last()->getPoint(this);
  pathLast = transform.map(p);
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

bool XWTikzState::hitTestChild(XWTeXBox * box)
{
  anchorChild();
  transformChild();
  shape = PGFcoordinate;
  XWTikzShape node(0,box,this,XW_TIKZ_CHILD);
  node.doShape(false);
  pathLast = node.anchorPos;
  double x = qAbs(node.anchorPos.x() - mousePoint.x());
  double y = qAbs(node.anchorPos.y() - mousePoint.y());
  return (x <= XW_TIKZ_HIT_DIST) && (y <= XW_TIKZ_HIT_DIST);
}

bool XWTikzState::hitTestCoordinate(XWTeXBox * box)
{
  anchorNode();
  transformNode();
  shape = PGFcoordinate;
  XWTikzShape node(0,box,this,XW_TIKZ_NODE);
  node.doShape(false);
  pathLast = node.anchorPos;
  double x = qAbs(node.anchorPos.x() - mousePoint.x());
  double y = qAbs(node.anchorPos.y() - mousePoint.y());
  return (x <= XW_TIKZ_HIT_DIST) && (y <= XW_TIKZ_HIT_DIST);
}

bool XWTikzState::hitTestCosine(XWTikzCoord * p)
{
  points.clear();
  coords << p;
  QPointF pp = coords.last()->getPoint(this);
  pathLast = transform.map(pp);
  points << pathLast;
  addCosine(pp);
  return hitTestLines();
}

bool XWTikzState::hitTestCurve(XWTikzCoord * c1,XWTikzCoord * c2)
{
  points.clear();
  tikzTimer = &XWTikzState::timerCurver;
  XWTikzCoord * endpoint = coords.last();
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
  return box->hitTest(p.x(),p.y()) ;
}

bool XWTikzState::hitTestParabola(XWTikzCoord * b, XWTikzCoord * e)
{
  points.clear();
  QPointF p = coords.last()->getPoint(this);
  pathLast = transform.map(p);
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
  pathLast = transform.map(pp);
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
  return box->hitTest(p.x(),p.y()) ;
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

void XWTikzState::inverted()
{
  transform = transform.inverted();
}

void XWTikzState::lineTo(XWTikzCoord * p)
{
  coords << p;
  QPointF p1 = p->getPoint(this);
  lineTo(p1);
  tikzTimer = &XWTikzState::timerLine;
}

void XWTikzState::lineTo(const QPointF & p)
{
  QPointF p1 = transform.map(p);
  pathLast = p1;
  toStart = pathLast;
  operations << XW_TIKZ_LINETO;
  points << p1;
}

void XWTikzState::lineTo(double xA, double yA)
{
  QPointF p(xA,yA);
  lineTo(p);
}

void XWTikzState::lineToHV(XWTikzCoord * p)
{
  coords << p;
  QPointF p2 = p->getPoint(this);
  QPointF p0 = coords[coords.size() - 2]->getPoint(this);
  QPointF p1;
  p1.setX(p2.x());
  p1.setY(p0.y());

  lineTo(p1);
  lineTo(p2);

  tikzTimer = &XWTikzState::timerHVLine;
}

void XWTikzState::lineToHV(const QPointF & p)
{
  QPointF p2 = transform.map(p);
  QPointF p0 = pathLast;
  QPointF p1;
  p1.setX(p2.x());
  p1.setY(p0.y());

  operations << XW_TIKZ_LINETO;
  points << p1;

  pathLast = p2;
  toStart = pathLast;
  operations << XW_TIKZ_LINETO;
  points << p2;

  tikzTimer = &XWTikzState::timerHVLine;
}

void XWTikzState::lineToVH(XWTikzCoord * p)
{
  coords << p;
  QPointF p2 = p->getPoint(this);
  QPointF p0 = coords[coords.size() - 2]->getPoint(this);
  QPointF p1;
  p1.setX(p0.x());
  p1.setY(p2.y());

  lineTo(p1);
  lineTo(p2);
  tikzTimer = &XWTikzState::timerVHLine;
}

void XWTikzState::lineToVH(const QPointF & p)
{
  QPointF p2 = transform.map(p);
  QPointF p0 = pathLast;
  QPointF p1;
  p1.setX(p0.x());
  p1.setY(p2.y());

  operations << XW_TIKZ_LINETO;
  points << p1;

  pathLast = p2;
  toStart = pathLast;
  operations << XW_TIKZ_LINETO;
  points << p2;
}

QRectF XWTikzState::map(const QRectF & rectA)
{
  return transform.mapRect(rectA);
}

QPointF XWTikzState::map(const QPointF & pA)
{
  return transform.map(pA);
}

void XWTikzState::map(double x1,double y1, double * x2, double * y2)
{
  QPointF p1(x1,y1);
  QPointF p2 = transform.map(p1);
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

void XWTikzState::moveTo(double xA, double yA, bool istarget)
{
  QPointF p(xA,yA);
  moveTo(p,istarget);
}

void XWTikzState::moveTo(XWTikzCoord * p, bool istarget)
{
  coords << p;
  QPointF p0 = p->getPoint(this);
  moveTo(p0,istarget);
}

void XWTikzState::moveTo(const QPointF & p, bool istarget)
{
  QPointF p1 = transform.map(p);
  pathLast = p1;
  toTarget = pathLast;
  if (!istarget)
    toStart = pathLast;
  operations << XW_TIKZ_MOVETO;
  points << p1;
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

XWTikzState * XWTikzState::save(bool ispathA)
{
  if (driver)
    driver->gsave();
  XWTikzState * newstate = new XWTikzState(ispathA);
  copy(newstate,false);
  newstate->saved = this;
  
  return newstate;
}

XWTikzState * XWTikzState::saveNode(XWTeXBox * boxA,int nt)
{
  XWTikzState * newstate = new XWTikzState(false,this);
  copy(newstate,true);
  newstate->isPath = false;
  newstate->currentChild = childrenNumber++;
  XWTikzShape * node = new XWTikzShape(driver,boxA,newstate,nt);
  newstate->myNode = node;
  newstate->myBox = boxA;
  switch (nt)
  {
    default:
      graphic->doEveryNode(newstate);
      graphic->doEveryShape(newstate);
      if (myNode)
        graphic->doEveryChildNode(newstate);
      break;

    case XW_TIKZ_LABEL:      
      graphic->doEveryLabel(newstate);
      newstate->setEdgeFromParent(false);
      break;

    case XW_TIKZ_PIN:
      graphic->doEveryPin(newstate);
      graphic->doEveryPinEdge(newstate);
      newstate->setEdgeFromParent(true);
      break;

    case XW_TIKZ_CHILD:
      newstate->level = level + 1; 
      graphic->doEveryChild(newstate);
      graphic->doEveryChildNode(newstate);
      newstate->setEdgeFromParent(true);
      break;

    case XW_TIKZ_EDGE:
      graphic->doEdgeFromParent(newstate);
      newstate->setEdgeFromParent(true);
      break;
  }

  nodes << node;
  return newstate;
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

void XWTikzState::setChamferedRectangleCorners(const QList<int> & cs)
{
  chamferedRectangleCorners = cs;
}

void XWTikzState::setCodes(const QList<XWTikzCommand*> & cmdsA)
{
  cmds = cmdsA;
}

void XWTikzState::setColor(const QColor & c)
{
  if (driver)
  {
    driver->setFillColor(c);
    driver->setStrokeColor(c);
    isFillText = true;
  }
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
  if (driver)
    driver->setStrokeColor(c);

  isDrawSet = true;
}

void XWTikzState::setFadingRoate(double d)
{
  fadingTransform.rotate(d);
}

void XWTikzState::setFadingScale(double sx,double sy)
{
  fadingTransform.scale(sx,sy);
}

void XWTikzState::setFadingShift(double dx,double dy)
{
  fadingTransform.translate(dx,dy);
}

void XWTikzState::setFadingSlant(double sx,double sy)
{
  fadingTransform.shear(sx,sy);
}

void XWTikzState::setFillColor(const QColor & c)
{
  if (driver)
    driver->setFillColor(c);

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

void XWTikzState::setLineWidth(double w)
{
  if (driver)
    driver->setLineWidth(w);
}

void XWTikzState::setMatrix(double a,double b,
                 double c,double d,
                 double dx, double dy)
{
  transform.setMatrix(a,b,0,c,d,0,dx,dy,1);
}

void XWTikzState::setMindmap(int m)
{
  mindmap = m;
  isDecorateSet = true;
  isFillSet = true;
  decoration = PGFcircleconnectionbar;
  to_path = &XWTikzState::toPathDefault;
}

void XWTikzState::setMirror(bool e)
{
  decorateTransform.setMatrix(1,0,0,e?-1:1,0,0,0,0,1);
}

void XWTikzState::setMiterLimit(double v)
{
  if (driver)
    driver->setMiterLimit(v);
}

void XWTikzState::setNodeContents(const QString & str)
{
  if (myBox)
    myBox->setContents(str);
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

void XWTikzState::setPos(double p)
{
  nodeTime = p;
  isTimeSet = true;
}

void XWTikzState::setRaise(double r)
{
  double dx = decorateTransform.dx();
  double dy = decorateTransform.dy() + r;
  decorateTransform.translate(dx,dy);
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
  driver->setFillColor(c);
  isFillText = true;
}

void XWTikzState::setTextOpacity(double o)
{
  fillOpacity = o;
  isFillText = true;
}

void XWTikzState::setTransform(const QTransform & transA)
{
  transform = transA;
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
  transform.translate(dx,dy);
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
  transform.shear(sx,sy);
}

QPointF XWTikzState::tangent(const QPointF & nc, 
                  const QPointF & ne,
                  const QPointF & p, 
                  int s)
{
  QPointF nnc = transform.map(nc);
  QPointF pp = transform.map(p);

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
  isTarget = true;
  if (!p)
    toTarget = points[0];
  else
  {
    QPointF pp = p->getPoint(this);
    toTarget = transform.map(pp);
  }

  graphic->doToPath(this);
}

void XWTikzState::transformColumn()
{
  double x = transform.dx() + columnSep;
  double y = transform.dy();
  transform.translate(x,y);
}

void XWTikzState::transformRow(double h)
{
  double x = transform.dx();
  double y = transform.dy() - rowSep - h;
  transform.translate(x,y);
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
  QPointF p = transform.map(c);
  double xc = p.x();
  double yc = p.y();

  p = transform.map(a);
  double xa = p.x() + xc;
  double ya = p.y() + yc;
  operations << XW_TIKZ_MOVETO;
  QPointF p1(xa,ya);
  points << p1;

  operations << XW_TIKZ_INSIDE;

  p = transform.map(b);
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
    p = transform.map(p);
    operations << XW_TIKZ_MOVETO;
    points << p;
    p.setX(ur.x());
    p.setY(y);
    p = transform.map(p);
    operations << XW_TIKZ_LINETO;
    points << p;
    y += ystep;
  }

  y -= 0.01;
  if (y < ur.y())
  {
    p.setX(ll.x());
    p.setY(y);
    p = transform.map(p);
    operations << XW_TIKZ_MOVETO;
    points << p;
    p.setX(ur.x());
    p.setY(y);
    p = transform.map(p);
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
    p = transform.map(p);
    operations << XW_TIKZ_MOVETO;
    points << p;
    p.setX(x);
    p.setY(ur.y());
    p = transform.map(p);
    operations << XW_TIKZ_LINETO;
    points << p;
    x += xstep;
  }

  x -= 0.01;
  if (x < ur.x())
  {
    p.setX(x);
    p.setY(ll.y());
    p = transform.map(p);
    operations << XW_TIKZ_MOVETO;
    points << p;
    p.setX(x);
    p.setY(ur.y());
    p = transform.map(p);
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

void XWTikzState::anchorLabel()
{
  int counta = (int)(labelAngle);
  if (counta == PGFcenter)
    anchor = PGFcenter;
  else
  {
    computeAngle();
    angle = labelAngle;    
    anchor = -1;
  }
}

void XWTikzState::anchorNode()
{
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

    QVector2D v(transform.m11(),transform.m12());
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

  p = transform.map(p);

  xa = pathLast.x() + p.x();
  double ya = pathLast.y() + p.y();

  p.setX(xr * cos(la));
  p.setY(yr * sin(la));
  p = transform.map(p);

  xb = pathLast.x() - p.x();
  double yb = pathLast.y() - p.y();

  p.setX(xr * cos(lb));
  p.setY(yr * sin(lb));
  p = transform.map(p);
  xb = xb + p.x();
  yb = yb + p.y();

  if (lb > la)
    lb -= 90;
  else
    lb += 90;

  p.setX(tmpa * cos(lb));
  p.setY(tmpb * sin(lb));
  p = transform.map(p);

  double xc = xb + p.x();
  double yc = yb + p.y();

  pathLast.setX(xb);
  pathLast.setY(yb);
  
  QPointF p1(xa,ya);
  QPointF p2(xc,yc);

  operations << XW_TIKZ_CURVETO;
  points << p1;
  points << p2;
  points << pathLast;
  toStart = pathLast;
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

void XWTikzState::computeDirection(const QPointF & b,const QPointF & c)
{
  int counta = (int)(labelAngle);
  if (counta == PGFcenter)
    return ;

  computeAngle();

  if (absolute || transformShape || b == c)
  {
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
  else
  {
    QVector2D v(c.x() - b.x(),c.y() - b.y());
    v.normalize();

    double x = v.y();
    double y = -v.x();

    if (x > 0.05)
    {
      if (y > 0.05)
        anchor = PGFsoutheast;
      else if (y < - 0.05)
        anchor = PGFsouthwest;
      else
        anchor = PGFsouth;
    }
    else if (x < -0.05)
    {
      if (y > 0.05)
        anchor = PGFnortheast;
      else if (y < - 0.05)
        anchor = PGFnorthwest;
      else
        anchor = PGFnorth;
    }
    else
    {
      if (y > 0)
        anchor = PGFeast;
      else
        anchor = PGFwest;
    }
  }  
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

    if (cp.x() < pathMinX)
      pathMinX = cp.x();
    if (cp.x() > pathMaxX)
      pathMaxX = cp.x();
    if (cp.y() < pathMinY)
      pathMinY = p.y();
    if (cp.y() > pathMaxY)
      pathMaxY = cp.y();
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
  transform *= decorateTransform;
}

void XWTikzState::decorateTransformLine(const QPointF & c,
                                        double decorateangle,
                                        double decoratedistance)
{
  double x = c.x() + decoratedistance * cos(decorateangle);
  double y = c.y() + decoratedistance * sin(decorateangle);
  transform.translate(x,y);
  transform.rotate(decorateangle);
  transform *= decorateTransform;
}

void XWTikzState::defaultCode()
{}

void XWTikzState::doNodes()
{
  int tmpanchor = 0;
  double tmpangle = 0;
  for (int i = 0; i < nodes.size(); i++)
  {
    XWTikzShape * node = nodes[i];
    node->setFontSize(fontSize);
    switch (node->nodeType)
    {
      default:
        break;

      case XW_TIKZ_NODE:
        node->state->anchorNode();
        node->state->transformNode();
        node->doShape(true);
        break;

      case XW_TIKZ_LABEL:
        anchorLabel();
        if (myNode)
        {
          myNode->doShape(false);
          node->state->computeDirection(myNode->anchorPos,myNode->centerPos);
          node->state->shift(myNode->anchorPos.x(),myNode->anchorPos.y());
        }
        node->state->transformLabel();
        node->doShape(true);
        break;

      case XW_TIKZ_PIN:
        anchorLabel();
        if (myNode)
        {
          graphic->doParentAnchor(myNode->state);
          myNode->doShape(false);
          node->state->toStart = myNode->anchorPos;
          node->state->startRadius = myNode->centerPos.x() - myNode->westPos.x();
          node->state->computeDirection(myNode->anchorPos,myNode->centerPos);
          node->state->shift(myNode->anchorPos.x(),myNode->anchorPos.y());
        }
        tmpanchor = node->state->anchor;
        tmpangle = node->state->angle;
        node->state->anchorChild();
        graphic->doChildAnchor(node->state);
        node->doShape(false);
        node->state->toTarget = node->anchorPos;
        node->state->endRadius = node->centerPos.x() - node->westPos.x();
        node->state->anchor = tmpanchor;
        node->state->angle = tmpangle;
        node->state->transformLabel();
        node->doShape(true);
        break;

      case XW_TIKZ_CHILD:
        anchor = growthParentAnchor;
        anchorParent();
        if (myNode)
        {
          graphic->doParentAnchor(myNode->state);
          myNode->doShape(false);
          node->state->toStart = myNode->anchorPos;
          node->state->startRadius = myNode->centerPos.x() - myNode->westPos.x();
        }
        tmpanchor = node->state->anchor;
        tmpangle = node->state->angle;
        node->state->anchorChild();
        graphic->doChildAnchor(node->state);
        node->doShape(false);
        node->state->toTarget = node->anchorPos;
        node->state->endRadius = node->centerPos.x() - node->westPos.x();
        node->state->anchor = tmpanchor;
        node->state->angle = tmpangle;
        node->state->anchorChild();
        node->state->transformChild();
        node->doShape(true);
        break;

      case XW_TIKZ_EDGE:
        nodes[i + 1]->state->setEdgeFromParent(true);
        break;
    }
  }
}

void XWTikzState::drawArrows()
{
  if (points.size() < 2)
    return ;

  QTransform tmp = transform;

  QPointF p0 = points[1];
  QPointF p1 = points[0];
  transformArrow(p0,p1);
  XWTikzArrow arrow;
  arrow.shape = startArrow;
  arrow.doArrow(this);

  int size = points.size();
  p0 = points[size - 2];
  p1 = points[size - 1];
  transformArrow(p0,p1);
  arrow.shape = endArrow;
  arrow.doArrow(this);

  transform = tmp;
}

void XWTikzState::edgeFromParentForkDown()
{
  operations << XW_TIKZ_MOVETO;
  points << toStart;

  double x = toStart.x();
  double y = toStart.y() - 0.5 * levelDistance;
  QPointF p1(x,y);
  operations << XW_TIKZ_LINETO;
  points << p1;

  QPointF p2(toTarget.x(), y);
  QPointF p3(toTarget.x(), toTarget.y());
  operations << XW_TIKZ_LINETO;
  points << p2;
  operations << XW_TIKZ_LINETO;
  points << p3;
}

void XWTikzState::edgeFromParentForkLeft()
{
  operations << XW_TIKZ_MOVETO;
  points << toStart;

  double x = toStart.x() - 0.5 * levelDistance;
  double y = toStart.y();
  QPointF p1(x,y);
  operations << XW_TIKZ_LINETO;
  points << p1;

  QPointF p2(toTarget.x(), y);
  QPointF p3(toTarget.x(), toTarget.y());
  operations << XW_TIKZ_LINETO;
  points << p2;
  operations << XW_TIKZ_LINETO;
  points << p3;
}

void XWTikzState::edgeFromParentForkRight()
{
  operations << XW_TIKZ_MOVETO;
  points << toStart;

  double x = toStart.x() + 0.5 * levelDistance;
  double y = toStart.y();
  QPointF p1(x,y);
  operations << XW_TIKZ_LINETO;
  points << p1;

  QPointF p2(toTarget.x(), y);
  QPointF p3(toTarget.x(), toTarget.y());
  operations << XW_TIKZ_LINETO;
  points << p2;
  operations << XW_TIKZ_LINETO;
  points << p3;
}

void XWTikzState::edgeFromParentForkUp()
{
  operations << XW_TIKZ_MOVETO;
  points << toStart;

  double x = toStart.x();
  double y = toStart.y() + 0.5 * levelDistance;
  QPointF p1(x,y);
  operations << XW_TIKZ_LINETO;
  points << p1;

  QPointF p2(toTarget.x(), y);
  QPointF p3(toTarget.x(), toTarget.y());
  operations << XW_TIKZ_LINETO;
  points << p2;
  operations << XW_TIKZ_LINETO;
  points << p3;
}

bool XWTikzState::hitTestLine(const QPointF & p1, const QPointF & p2)
{
  QPointF c1 = transform.map(p1);
  QPointF c2 = transform.map(p2);
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
  QPointF c = transform.map(p);
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
  lineWidth = 0.4;
  innerLineWidth = 0;
  anchor = PGFcenter;
  angle = 0;
  textWidth = 85;
  align = 0;
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

  labelAngle = PGFcenter;
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

  isUseAsBoundingBoxSet = false;

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
  edgeFromParent = false;
  currentChild = 0;
  childrenNumber = 0;

  isContinue = false;
  handler = 0;
  tension = 0.5;
  mark = 0;
  markRepeat = 0;
  markPhase = 0;
  markSize = 2;

  transformShape = false;

  startArrow = 0;
  endArrow = 0;

  leftExtend = 0;
  rightExtend = 0;
  shortenStartAdditional = 0;
  shortenEndAdditional = 0;

  xVec = 28.4527559;
  yVec = 28.4527559;
  zVec = -20.1191366;

  drawOpacity = 1;
  fillOpacity = 1;

  samples = 25;
  domainStart = -5;
  domainEnd = 5;

  pathMinX = 16000;
  pathMinY = 16000;
  pathMaxX = -16000;
  pathMaxY = -16000;

  pathType = XW_TIKZ_PATH_BEFOREBACKGROUND;
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
  isTarget = false;

  mindmap = -1;
  spyUsing = 0;
  onNode = true;

  position = 0;
  startPosition = 0;
  endPosition = 0;
  step = 0;

  tikzTimer = &XWTikzState::timerLine;
  before_code = &XWTikzState::defaultCode;
  after_code = &XWTikzState::defaultCode;

  to_path = &XWTikzState::toPathDefault;

  myNode = 0;
  myBox = 0;
}

void XWTikzState::initPath()
{
  XWTikzArrow arrow;
  arrow.shape = endArrow;
  arrow.initPath(this,true);

  arrow.shape = startArrow;
  arrow.initPath(this,false);

  if (points.size() > 1)
  {
    QPointF p0 = points[0];
    QPointF p1 = points[1];
    p0 = lineAtDistance(-leftExtend + shortenStartAdditional,p0,p1);
    points[0] = p0;

    int size = points.size();
    p0 = points[size - 2];
    p1 = points[size - 1];
    p1 = lineAtDistance(rightExtend + shortenEndAdditional,p0,p1);
    points[size - 1] = p1;
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
  QPointF p1 = transform.map(c1);
  QPointF p2 = transform.map(endpoint);
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

  points << p1;
  points << p1;
  points << pathLast;
  toStart = pathLast;
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
  toStart = pathLast;
  QPointF p1 = transform.map(ll);
  p2 = transform.map(p2);
  p3 = transform.map(p3);
  p4 = transform.map(p4);

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

void XWTikzState::timerCurver()
{
  if (points.size() < 4)
    return ;

  int pos = points.size() - 1;
  QPointF p4 = points[pos];
  QPointF p3 = points[pos-1];
  QPointF p2 = points[pos-2];
  QPointF p1 = points[pos-3];

  transformCurveAtTime(nodeTime,p1,p2,p3,p4);
}

void XWTikzState::timerHVLine()
{
  if (points.size() < 3)
    return ;

  int pos = points.size() - 1;
  QPointF p3 = points[pos];
  QPointF p2 = points[pos-1];
  QPointF p1 = points[pos-2];

  if (nodeTime < 0.5)
  {
    double t = 2 * nodeTime;
    transformLineAtTime(t,p1,p2);
  }
  else
  {
    double t = 2 * nodeTime - 1;
    transformLineAtTime(t,p2,p3);
  }
}

void XWTikzState::timerLine()
{
  if (points.size() < 2)
    return ;

  int pos = points.size() - 1;
  QPointF p2 = points[pos];
  QPointF p1 = points[pos-1];

  transformLineAtTime(nodeTime,p1,p2);
}

void XWTikzState::timerVHLine()
{
  if (points.size() < 3)
    return ;

  int pos = points.size() - 1;
  QPointF p3 = points[pos];
  QPointF p2 = points[pos-1];
  QPointF p1 = points[pos-2];

  if (nodeTime < 0.5)
  {
    double t = 2 * nodeTime;
    transformLineAtTime(t,p1,p2);
  }
  else
  {
    double t = 2 * nodeTime - 1;
    transformLineAtTime(t,p2,p3);
  }
}

void XWTikzState::toPathDefault()
{
  QTransform oldtranform = transform;
  transform.reset();
  lineTo(toTarget);
  transform = oldtranform;
}

void XWTikzState::transformArrow(const QPointF & p1,const QPointF & p2)
{
  double xa = p2.x() - p1.x();
  double ya = p2.y() - p1.y();
  double d = sqrt(xa * xa + ya * ya);
  double x = xa / d;
  double y = ya / d;
  ya = -y;

  transform.setMatrix(x,y,0,ya,x,0,p2.x(),p2.y(),1);
}

void XWTikzState::transformChild()
{
  double xc = 0.5 * siblingDistance * (childrenNumber + 1);
  double x = xc * cos(growLeft) + (siblingDistance * currentChild) * cos(growRight);
  double y = xc * sin(growLeft) + (siblingDistance * currentChild) * sin(growRight);
  x += levelDistance * cos(growAngle);
  y += levelDistance * sin(growAngle);
  x += transform.dx();
  y += transform.dy();
  transform.translate(x,y);
}

void XWTikzState::transformCurveAtTime(double t,const QPointF & startpoint,const QPointF & c1, 
                      const QPointF & c2,const QPointF & endpoint)
{
  QPointF d = curveAtTime(t,startpoint,c1,c2,endpoint);
  transform.translate(d.x(),d.y());
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

void XWTikzState::transformLabel()
{
  if ((int)(labelAngle) == PGFcenter)
    return ;
    
  double dx = labelDistance * cos(labelAngle) + transform.dx();
  double dy = labelDistance * sin(labelAngle) + transform.dy();
  transform.translate(dx,dy);
}

void XWTikzState::transformLineAtTime(double t, const QPointF & p1,const QPointF & p2)
{
  QPointF d = lineAtTime(t,p1,p2);
  transform.translate(d.x(),d.y());
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
      transform.translate(p.x(),p.y());
  }
  else
    (this->*(tikzTimer))();
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
