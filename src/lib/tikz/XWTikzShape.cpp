/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QPointF>
#include <QVector2D>
#include <QList>
#include <math.h>
#include "PGFKeyWord.h"
#include "XWPDFDriver.h"
#include "XWTikzArrow.h"
#include "XWTikzState.h"
#include "XWTikzOption.h"
#include "XWTikzNode.h"
#include "XWTikzTextBox.h"
#include "XWTikzShape.h"

XWTikzShape::XWTikzShape(XWPDFDriver*driverA,
                         XWTeXBox * boxA,
                         XWTikzState * stateA,
                         int nt)
:driver(driverA),
 box(boxA),
 state(stateA),
 nodeType(nt),
 boxWidth(0),
 boxHeight(0),
 boxDepth(0),
 radius(0),
 option(0)
{
  sh_beforebackground = NULL;
  sh_foreground = NULL;
}

XWTikzShape::~XWTikzShape()
{}

double XWTikzShape::angleOnEllipse(const QPointF & p, const QPointF & ur)
{
  double xa = ur.x();
  double ya = ur.y();
  double x = p.x();
  double y = p.y();
  double tempdima = 1;
  double mathresult = 0;
  if (x != 0)
  {
    tempdima = x;
    mathresult = xa / tempdima;
    tempdima = mathresult;
  }

  if (y == 0)
    tempdima = 1;
  else
  {
    mathresult = y / ya;
    tempdima = mathresult * tempdima;
    mathresult = atan(tempdima);
  }

  tempdima = mathresult;
  if (tempdima < 0)
    tempdima += 360;

  if (x < 0)
  {
    if (y == 0)
      tempdima = 180;
    else
    {
      if (y < 0)
        tempdima += 180;
      else
        tempdima -= 180;
    }
  }
  else
  {
    if (x == 0)
    {
      if (y < 0)
        tempdima = 270;
      else
        tempdima = 90;
    }
    else
    {
      if (y == 0)
        tempdima = 0;
    }
  }

  return tempdima;
}

void XWTikzShape::doShape(bool dopath)
{
  if (box)
  {
    boxWidth = box->getWidth(state->textWidth);
    boxHeight = box->getHeight(state->textWidth);
    boxDepth = box->getDepth(state->textWidth);
  }

  if (boxWidth == 0)
    boxWidth = state->minWidth;
  if (boxHeight == 0)
    boxHeight = state->minHeight;
  centerPoint.setX(0.5 * boxWidth);
  centerPoint.setY(0.5 * boxHeight - 0.5 * boxDepth);
  basePoint.setX(centerPoint.x());
  basePoint.setY(0);
  midPoint.setX(centerPoint.x());
  midPoint.setY(5);
  referencePoint = centerPoint;

  switch (state->shape)
  {
    default:
      rectangleShape(dopath);
      break;

    case PGFcircle:
      circleShape(dopath);
      break;

    case PGFcoordinate:
      coordinateShape(dopath);
      break;

    case PGFrectangle:
      rectangleShape(dopath);
      break;

    case PGFcircularsector:
      circularSectorShape(dopath);
      break;

    case PGFcylinder:
      cylinderShape(dopath);
      break;

    case PGFdart:
      dartShape(dopath);
      break;

    case PGFdiamond:
      diamondShape(dopath);
      break;

    case PGFellipse:
      ellipseShape(dopath);
      break;

    case PGFisoscelestriangle:
      isosceleTriangleShape(dopath);
      break;

    case PGFkite:
      kiteShape(dopath);
      break;

    case PGFregularpolygon:
      regularPolygonShape(dopath);
      break;

    case PGFsemicircle:
      semicircleShape(dopath);
      break;

    case PGFstar:
      starShape(dopath);
      break;

    case PGFtrapezium:
      trapeziumShape(dopath);
      break;

    case PGFcloud:
      cloudShape(dopath);
      break;

    case PGFcorrectforbiddensign:
      correctForbiddenSignShape(dopath);
      break;

    case PGFforbiddensign:
      forbiddenSignShape(dopath);
      break;
    
    case PGFmagnetictape:
      magneticTapeShape(dopath);
      break;

    case PGFmagnifyingglass:
      magnifyingGlassShape(dopath);
      break;

    case PGFsignal:
      signalShape(dopath);
      break;

    case PGFstarburst:
      starburstShape(dopath);
      break;

    case PGFtape:
      tapeShape(dopath);
      break;

    case PGFarrowbox:
      arrowBoxShape(dopath);
      break;

    case PGFdoublearrow:
      doubleArrowShape(dopath);
      break;

    case PGFsinglearrow:
      singleArrowShape(dopath);
      break;

    case PGFcirclesolidus:
      circleSolidusShape(dopath);
      break;

    case PGFcirclesplit:
      circleSplitShape(dopath);
      break;

    case PGFellipsesplit:
      ellipseSplitShape(dopath);
      break;

    case PGFrectanglesplit:
      rectangleSplitShape(dopath);
      break;

    case PGFcloudcallout:
      cloudCalloutShape(dopath);
      break;

    case PGFellipsecallout:
      ellipseCalloutShape(dopath);
      break;

    case PGFrectanglecallout:
      rectangleCalloutShape(dopath);
      break;

    case PGFchamferedrectangle:
      chamferedRectangleShape(dopath);
      break;

    case PGFcrossout:
      crossoutShape(dopath);
      break;

    case PGFroundedrectangle:
      roundedRectangleShape(dopath);
      break;

    case PGFstrikeout:
      strikeoutShape(dopath);
      break;

    case PGFcircleee:
      circleeeShape(dopath);
      break;

    case PGFcurrentdirection:
    case PGFcurrentdirectionreversed:
    case PGFdirectionee:
      directioneeShape(dopath);
      break;

    case PGFresistorIEC:
    case PGFvarinductorIEC:
    case PGFrectangleee:
      rectangleeeShape(dopath);
      break;

    case PGFvarresistorIEC:
      varresistorIECShape(dopath);
      break;

    case PGFinductorIEC:
      inductorIECShape(dopath);
      break;

    case PGFcapacitorIEC:
      capacitorIECShape(dopath);
      break;

    case PGFgroundIEC:
      groundIECShape(dopath);
      break;

    case PGFbatteryIEC:
      batteryIECShape(dopath);
      break;

    case PGFbreakdowndiodeIEC:
    case PGFvarbreakdowndiodeIEC:
      breakdowndiodeIECShape(dopath);
      break;

    case PGFcontactIEC:
    case PGFconnectionIEC:
    case PGFbulbIEC:
    case PGFcurrentsourceIEC:
    case PGFvoltagesourceIEC:
    case PGFamperemeter:
    case PGFvoltmeter:
    case PGFohmmeter:
    case PGFacsource:
    case PGFdcsource:
    case PGFgenericcircleIEC:
      genericcircleIECShape(dopath);
      break;

    case PGFdiodeIEC:
    case PGFvardiodeIEC:
    case PGFZenerdiodeIEC:
    case PGFvarZenerdiodeIEC:
    case PGFSchottkydiodeIEC:
    case PGFvarSchottkydiodeIEC:
    case PGFbackwarddiodeIEC:
    case PGFvarbackwarddiodeIEC:
    case PGFgenericdiodeIEC:
      genericdiodeIECShape(dopath);
      break;

    case PGFmakecontactIEC:
      makecontactIECShape(dopath);
      break;

    case PGFvarmakecontactIEC:
      varmakecontactIECShape(dopath);
      break;

    case PGFbreakcontactIEC:
      breakcontactIECShape(dopath);
      break;

    case PGFandgateIEC:
      andGateIECShape(dopath);
      break;

    case PGFnandgateIEC:
      nandGateIECShape(dopath);
      break;

    case PGForgateIEC:
      orGateIECShape(dopath);
      break;

    case PGFnorgateIEC:
      norGateIECShape(dopath);
      break;

    case PGFxorgateIEC:
      xorGateIECShape(dopath);
      break;

    case PGFxnorgateIEC:
      xnorGateIECShape(dopath);
      break;

    case PGFbuffergateIEC:
      bufferGateIECShape(dopath);
      break;

    case PGFnotgateIEC:
      notGateIECShape(dopath);
      break;

    case PGFandgateUS:
      andGateUSShape(dopath);
      break;

    case PGFnandgateUS:
      nandGateUSShape(dopath);
      break;

    case PGForgateUS:
      orGateUSShape(dopath);
      break;

    case PGFnorgateUS:
      norGateUSShape(dopath);
      break;

    case PGFxorgateUS:
      xorGateUSShape(dopath);
      break;

    case PGFxnorgateUS:
      xnorGateUSShape(dopath);
      break;

    case PGFnotgateUS:
      notGateUSShape(dopath);
      break;

    case PGFbuffergateUS:
      bufferGateUSShape(dopath);
      break;

    case PGFandgateCDH:
      andGateCDHShape(dopath);
      break;

    case PGFnandgateCDH:
      nandGateCDHShape(dopath);
      break;
  }

  if (dopath && state->annotation)
    doAnnotation();
}

void XWTikzShape::init()
{
  setCircuitShape();

  switch (state->shape)
  {
    default:
      break;

    case PGFcurrentdirection:
    case PGFcurrentdirectionreversed:
      state->setDraw(true);
      state->setFillColor(Qt::black);
      option = new XWTikzCurrentDirectionArrow(state->graphic,state);
      option->doPath(state,false);
      state->minWidth = 0.5 * state->circuitsSizeUnit + 0.3 * state->lineWidth;
      state->minHeight = 0.5 * state->circuitsSizeUnit + 0.3 * state->lineWidth;
      state->setTransformShape();
      if (state->shape == PGFcurrentdirectionreversed)
        state->rotate(180);
      break;

    case PGFresistorIEC:
      state->setDraw(true);
      state->setCircuitSymbolSize(4,1);
      state->setTransformShape();
      break;

    case PGFvarresistorIEC:
      state->setDraw(true);
      state->setCircuitSymbolSize(4.8,0.8);
      state->setTransformShape();
      state->outerXSep = 0;
      state->outerYSep = 0;
      state->setLineCap(PGFround);
      break;

    case PGFinductorIEC:
      state->setDraw(true);
      state->setCircuitSymbolSize(4,0.5);
      state->setTransformShape();
      state->outerXSep = 0;
      state->outerYSep = 0;
      state->setLineCap(PGFround);
      break;

    case PGFvarinductorIEC:
      state->setDraw(true);
      state->setFillColor(Qt::black);
      state->setCircuitSymbolSize(4,1);
      state->setTransformShape();
      break;

    case PGFcapacitorIEC:
      state->setDraw(true);
      state->setCircuitSymbolSize(0.5,2);
      state->setTransformShape();
      break;

    case PGFgroundIEC:
      state->setDraw(true);
      state->setCircuitSymbolSize(0.75,2);
      state->setTransformShape();
      break;

    case PGFbatteryIEC:
      state->setDraw(true);
      state->setCircuitSymbolSize(0.5,2.5);
      state->setTransformShape();
      break;

    case PGFdiodeIEC:
    case PGFvardiodeIEC:
      state->setDraw(true);
      if (state->shape == PGFvardiodeIEC)
        state->setFillColor(Qt::black);
      state->setCircuitSymbolSize(1.25,1.25);
      state->setTransformShape();
      sh_beforebackground = &XWTikzShape::beforeBgDiodeIEC;
      break;

    case PGFZenerdiodeIEC:
    case PGFvarZenerdiodeIEC:
      state->setDraw(true);
      if (state->shape == PGFvarZenerdiodeIEC)
        state->setFillColor(Qt::black);
      state->setCircuitSymbolSize(1.25,1.25);
      state->setTransformShape();
      sh_beforebackground = &XWTikzShape::beforeBgZenerDiodeIEC;
      break;

    case PGFSchottkydiodeIEC:
    case PGFvarSchottkydiodeIEC:
      state->setDraw(true);
      if (state->shape == PGFvarSchottkydiodeIEC)
        state->setFillColor(Qt::black);
      state->setCircuitSymbolSize(1.25,1.25);
      state->setTransformShape();
      sh_beforebackground = &XWTikzShape::beforeBgSchottkyDiodeIEC;
      break;

    case PGFtunneldiodeIEC:
    case PGFvartunneldiodeIEC:
      state->setDraw(true);
      if (state->shape == PGFvartunneldiodeIEC)
        state->setFillColor(Qt::black);
      state->setCircuitSymbolSize(1.25,1.25);
      state->setTransformShape();
      sh_beforebackground = &XWTikzShape::beforeBgTunnelDiodeIEC;
      break;

    case PGFbackwarddiodeIEC:
    case PGFvarbackwarddiodeIEC:
      state->setDraw(true);
      if (state->shape == PGFvarbackwarddiodeIEC)
        state->setFillColor(Qt::black);
      state->setCircuitSymbolSize(1.25,1.25);
      state->setTransformShape();
      sh_beforebackground = &XWTikzShape::beforeBgBackwardDiodeIEC;
      break;

    case PGFbreakdowndiodeIEC:
    case PGFvarbreakdowndiodeIEC:
      state->setDraw(true);
      if (state->shape == PGFvarbreakdowndiodeIEC)
        state->setFillColor(Qt::black);
      state->setCircuitSymbolSize(2.5,1.25);
      state->setTransformShape();
      break;

    case PGFcontactIEC:
      state->setDraw(true);
      state->setFillColor(Qt::black);
      state->setCircuitSymbolSize(0.5,0.5);
      state->outerXSep = 0.25 * state->lineWidth;
      state->outerYSep = 0.25 * state->lineWidth;
      break;

    case PGFconnectionIEC:
      state->setDraw(true);
      state->setCircuitSymbolSize(1,1);
      state->outerXSep = 0.25 * state->lineWidth;
      state->outerYSep = 0.25 * state->lineWidth;
      break;

    case PGFbulbIEC:
      state->setDraw(true);
      state->setCircuitSymbolSize(1.5,1.5);
      state->setTransformShape();
      sh_beforebackground = &XWTikzShape::beforeBgBulbIEC;
      break;

    case PGFcurrentsourceIEC:
      state->setDraw(true);
      state->setCircuitSymbolSize(2,2);
      state->setTransformShape();
      sh_beforebackground = &XWTikzShape::beforeBgCurrentSourceIEC;
      break;

    case PGFvoltagesourceIEC:
      state->setDraw(true);
      state->setCircuitSymbolSize(2,2);
      state->setTransformShape();
      sh_beforebackground = &XWTikzShape::beforeBgVoltageSourceIEC;
      break;

    case PGFmakecontactIEC:
      state->setDraw(true);
      state->setCircuitSymbolSize(2,1);
      state->setTransformShape();
      state->outerXSep = 0;
      state->outerYSep = 0;
      state->setLineCap(PGFround);
      break;

    case PGFvarmakecontactIEC:
      state->setDraw(true);
      state->setCircuitSymbolSize(2.4,1);
      state->setTransformShape();
      state->outerXSep = 0;
      state->outerYSep = 0;
      state->setLineCap(PGFround);
      break;

    case PGFbreakcontactIEC:
      state->setDraw(true);
      state->setCircuitSymbolSize(2,1);
      state->setTransformShape();
      state->outerXSep = 0;
      state->outerYSep = 0;
      state->setLineCap(PGFround);
      break;

    case PGFamperemeter:
      state->setDraw(true);
      state->setCircuitSymbolSize(2,2);
      state->setTransformShape();
      option = new XWTikzLabel(state->graphic,PGFinfo,state);
      {
        QString str("center:A");
        int len = str.length();
        int pos = 0;
        option->scan(str,len,pos);
      }
      option->doPath(state,false);
      break;

    case PGFvoltmeter:
      state->setDraw(true);
      state->setCircuitSymbolSize(2,2);
      state->setTransformShape();
      option = new XWTikzLabel(state->graphic,PGFinfo,state);
      {
        QString str("center:V");
        int len = str.length();
        int pos = 0;
        option->scan(str,len,pos);
      }
      option->doPath(state,false);
      break;

    case PGFohmmeter:
      state->setDraw(true);
      state->setCircuitSymbolSize(2,2);
      state->setTransformShape();
      option = new XWTikzLabel(state->graphic,PGFinfo,state);
      {
        QString str("center:$\\Omega$");
        int len = str.length();
        int pos = 0;
        option->scan(str,len,pos);
      }
      option->doPath(state,false);
      break;

    case PGFacsource:
      state->setDraw(true);
      state->setCircuitSymbolSize(2,2);
      state->setTransformShape();
      sh_beforebackground = &XWTikzShape::beforeBgACSource;
      break;

    case PGFdcsource:
      state->setDraw(true);
      state->setCircuitSymbolSize(2,2);
      state->setTransformShape();
      sh_beforebackground = &XWTikzShape::beforeBgDCSource;
      break;

    case PGFandgateIEC:
      state->setDraw(true);
      state->setInnerXSep(5);
      state->setInnerYSep(5);
      state->setCircuitSymbolSize(2.5,4);
      state->setTransformShape();
      state->logicGateInvertedRadius = 0.25 * state->circuitsSizeUnit;
      box = new XWTikzTextBox(state->graphic,state);
      {
        int len = state->andGateIECSymbol.length();
        int pos = 0;
        box->scan(state->andGateIECSymbol,len,pos);
      }
      break;

    case PGFnandgateIEC:
      state->setDraw(true);
      state->setInnerXSep(5);
      state->setInnerYSep(5);
      state->setCircuitSymbolSize(2.5,4);
      state->setTransformShape();
      state->logicGateInvertedRadius = 0.25 * state->circuitsSizeUnit;
      box = new XWTikzTextBox(state->graphic,state);
      {
        int len = state->nandGateIECSymbol.length();
        int pos = 0;
        box->scan(state->nandGateIECSymbol,len,pos);
      }
      break;

    case PGForgateIEC:
      state->setDraw(true);
      state->setInnerXSep(5);
      state->setInnerYSep(5);
      state->setCircuitSymbolSize(2.5,4);
      state->setTransformShape();
      state->logicGateInvertedRadius = 0.25 * state->circuitsSizeUnit;
      box = new XWTikzTextBox(state->graphic,state);
      {
        int len = state->orGateIECSymbol.length();
        int pos = 0;
        box->scan(state->orGateIECSymbol,len,pos);
      }
      break;

    case PGFnorgateIEC:
      state->setDraw(true);
      state->setInnerXSep(5);
      state->setInnerYSep(5);
      state->setCircuitSymbolSize(2.5,4);
      state->setTransformShape();
      state->logicGateInvertedRadius = 0.25 * state->circuitsSizeUnit;
      box = new XWTikzTextBox(state->graphic,state);
      {
        int len = state->norGateIECSymbol.length();
        int pos = 0;
        box->scan(state->norGateIECSymbol,len,pos);
      }
      break;

    case PGFxorgateIEC:
      state->setDraw(true);
      state->setInnerXSep(5);
      state->setInnerYSep(5);
      state->setCircuitSymbolSize(2.5,4);
      state->setTransformShape();
      state->logicGateInvertedRadius = 0.25 * state->circuitsSizeUnit;
      box = new XWTikzTextBox(state->graphic,state);
      {
        int len = state->xorGateIECSymbol.length();
        int pos = 0;
        box->scan(state->xorGateIECSymbol,len,pos);
      }
      break;

    case PGFxnorgateIEC:
      state->setDraw(true);
      state->setInnerXSep(5);
      state->setInnerYSep(5);
      state->setCircuitSymbolSize(2.5,4);
      state->setTransformShape();
      state->logicGateInvertedRadius = 0.25 * state->circuitsSizeUnit;
      box = new XWTikzTextBox(state->graphic,state);
      {
        int len = state->xnorGateIECSymbol.length();
        int pos = 0;
        box->scan(state->xnorGateIECSymbol,len,pos);
      }
      break;

    case PGFbuffergateIEC:
      state->setDraw(true);
      state->setInnerXSep(5);
      state->setInnerYSep(5);
      state->setCircuitSymbolSize(2.5,4);
      state->setTransformShape();
      state->logicGateInvertedRadius = 0.25 * state->circuitsSizeUnit;
      box = new XWTikzTextBox(state->graphic,state);
      {
        int len = state->bufferGateIECSymbol.length();
        int pos = 0;
        box->scan(state->bufferGateIECSymbol,len,pos);
      }
      break;

    case PGFnotgateIEC:
      state->setDraw(true);
      state->setInnerXSep(5);
      state->setInnerYSep(5);
      state->setCircuitSymbolSize(2.5,4);
      state->setTransformShape();
      state->logicGateInvertedRadius = 0.25 * state->circuitsSizeUnit;
      box = new XWTikzTextBox(state->graphic,state);
      {
        int len = state->notGateIECSymbol.length();
        int pos = 0;
        box->scan(state->notGateIECSymbol,len,pos);
      }
      break;

    case PGFandgateUS:
    case PGFnandgateUS:
    case PGForgateUS:
    case PGFnorgateUS:
    case PGFxorgateUS:
    case PGFxnorgateUS:
    case PGFandgateCDH:
    case PGFnandgateCDH:
      state->setDraw(true);
      state->setCircuitSymbolSize(0,2.5);
      state->setTransformShape();
      state->logicGateInvertedRadius = 0.25 * state->circuitsSizeUnit;
      break;

    case PGFnotgateUS:
    case PGFbuffergateUS:
      state->setDraw(true);
      state->setCircuitSymbolSize(0,2.5);
      state->setTransformShape();
      state->logicGateInvertedRadius = 0.25 * state->circuitsSizeUnit;
      if (state->inputs.size() == 0)
        state->inputs << QChar('n');
      break;
  }
}

void XWTikzShape::mapPos()
{
  centerPos = state->map(centerPos);
  anchorPos = state->map(anchorPos);
  westPos = state->map(westPos);
  eastPos = state->map(eastPos);
  northPos = state->map(northPos);
  southPos = state->map(southPos);
}

QPointF XWTikzShape::pointBorderEllipse(const QPointF & pd, const QPointF & ur)
{
  double xa = ur.x();
  double ya = ur.y();
  double x = xa;
  double y = ya;
  if (xa == ya)
  {
    QVector2D v(pd.x(),pd.y());
    v.normalize();
    x = v.x() * xa;
    y = v.y() * ya;
  }
  else
  {
    if (xa < ya)
    {
      int countb = qRound(ya);//qRound(ya / 65536);
      x = x / countb;
      y = y / countb;
      double xc = x;
      double yc = 1;//8192;
      //y = 0.125 * y;
      countb = qRound(y);
      yc = yc / countb;
      x = pd.x();
      y = pd.y();
      y = yc * y;
      y = xc * y;
      QVector2D v(x,y);
      v.normalize();
      x = v.x() * xa;
      y = v.y() * ya;
    }
    else
    {
      int countb = qRound(xa);//qRound(xa / 65536);
      x = x / countb;
      y = y / countb;
      double yc = y;
      double xc = 1;//8192;
      //x = 0.125 * x;
      countb = qRound(x);
      xc = xc / countb;
      x = pd.x();
      y = pd.y();
      y = yc * y;
      y = xc * y;
      QVector2D v(x,y);
      v.normalize();
      x = v.x() * xa;
      y = v.y() * ya;
    }
  }

  QPointF ret(x,y);
  return ret;
}

QPointF XWTikzShape::pointBorderRectangle(const QPointF & pd, const QPointF & ur)
{
  double xb = ur.x();
  double yb = ur.y();
  double xa = pd.x();
  double ya = pd.y();

  double x = xa;
  double y = ya;
  if (x < 0)
    x = -x;

  if (y < 0)
    y = -y;

  double xc = x;//0.125 * x;
  double yc = y;//0.125 * y;
  int counta = qRound(xc);
  int countb = qRound(yc);
  if (countb < counta)
  {
    if (counta < 255)
    {
      y = yb;
      x = 0;
    }
    else
    {
      xc = 1 / counta;//8192 / counta;
      y = xc * y;
      y = xb * y;
      if (y < yb)
        x = xb;
      else
      {
        if (countb < 255)
        {
          x = xb;
          y = 0;
        }
        else
        {
          yc = 1 / countb;//8192 / countb;
          x = yc * x;
          x = yb * x;
          y = yb;
        }
      }
    }
  }
  else
  {
    if (countb < 255)
    {
      x = xb;
      y = 0;
    }
    else
    {
      yc = 1 / countb;//8192 / countb;
      x = yc * x;
      x = yb * x;
      if (x < xb)
        y = yb;
      else
      {
        if (counta < 255)
        {
          y = yb;
          x = 0;
        }
        else
        {
          xc = 1 / counta;//8192 / counta;
          y = xc * y;
          y = xb * y;
          x = xb;
        }
      }
    }
  }

  if (xa < 0)
    x = -x;

  if (ya < 0)
    y = -y;

  QPointF ret(x,y);
  return ret;
}

QPointF XWTikzShape::pointShapeBorder(double x, double y)
{
  QTransform trans = state->transform.inverted();
  QPointF p(x,y);
  QPointF pa = trans.map(p);
  double xb = 10;
  if (pa.x() < 1)
  {
    if (pa.x() > -1)
      xb = 10 * pa.x();
  }
  double yb = 10;
  if (pa.y() < 1)
  {
    if (pa.y() > -1)
      yb = 10 * pa.y();
  }

  double tmp = xb * xb + yb * yb;
  if (tmp < 0.04)
    return centerPos;

  return p;
}

void XWTikzShape::setFontSize(int s)
{
  if (box)
    box->setFontSize(s);
}

void XWTikzShape::circleShape(bool dopath)
{
  circleAnchor();
  if (dopath)
  {
    state = state->save(false);
    if (state->outerXSep < state->outerYSep)
      radius = radius - state->outerYSep;
    else
      radius = radius - state->outerXSep;
    state->moveTo(centerPoint);
    state->addCircle(centerPoint,radius);  
    state = state->restore();
    doContent();
  }
}

void XWTikzShape::coordinateShape(bool dopath)
{
  anchorPos = centerPoint;
  westPos = centerPoint;
  eastPos = centerPoint;
  northPos = centerPoint;
  southPos = centerPoint;
  if (dopath)
    state->moveTo(anchorPos);
}

void XWTikzShape::rectangleShape(bool dopath)
{
  rectangleAnchor();
  if (dopath)
  {
    double xa = southWest.x() + state->outerXSep;
    double ya = southWest.y() + state->outerYSep;
    double xb = northEast.x() - state->outerXSep;
    double yb = northEast.y() - state->outerYSep;
    centerPoint.setX(0.5 * southWest.x() + 0.5 * northEast.x());
    centerPoint.setY(0.5 * southWest.y() + 0.5 * northEast.y());
    QPointF ll(xa,ya);
    QPointF ur(xb,yb);

    state = state->save(false);
    state->moveTo(ll);
    state->addRectangle(ll,ur);
    state = state->restore();
    doContent();
  }
}

QPointF XWTikzShape::pointIntersectionOfLineAndArc(const QPointF & p1,
                                        const QPointF & p2,
                                        const QPointF & c,
                                        double sa,double ea,
                                        double xr,double yr)
{
  double x = state->angleBetweenPoints(p1,p2);
  double s = sa;
  double ss = (int)s % 360;
  if (ss < 0)
    ss += 360;

  double e = ea;
  double ee = (int)e % 360;
  if (ee < 0)
    ee += 360;

  if (ee < ss)
    x = ((int)x + 180) % 360;
  int m = 360;
  double n = (s + e) / 2;
  double xarcradius = xr;
  double yarcradius = yr;
  while (true)
  {
    double p = (s + e) / 2;
    if (p == s)
      break;

    QPointF oo(xarcradius * cos(p),yarcradius * sin(p));
    QPointF oc = c + oo;
    double mathresult = state->angleBetweenPoints(p2,oc);
    if (ee < ss)
    {
      mathresult += 180;
      mathresult = (int)mathresult % 360;
    }
    double q = mathresult;
    if (x > 335)
    {
      if (q < 45)
        q += 360;
    }

    if (x == q)
      break;

    if (x < q)
      e = p;
    else
      s = p;

    mathresult = qAbs(x - q);
    if (mathresult < m)
    {
      m = mathresult;
      n = p;
    }
  }

  QPointF ret(xarcradius * cos((double)n),yarcradius * sin((double)n));
  ret = ret + c;
  return ret;
}

void XWTikzShape::circleAnchor()
{
  double ya = 0.5 * boxHeight + 0.5 * boxDepth + state->innerYSep;
  double xa = 0.5 * boxWidth + state->innerXSep;
  radius = sqrt(xa * xa + ya * ya);
  double x = xa / radius;
  double y = ya / radius;
  if (x > y)
    radius = x * xa;
  else
    radius = y * ya;

  if (radius < 0.5 * state->minWidth)
    radius = 0.5 * state->minWidth;

  if (radius < 0.5 * state->minHeight)
    radius = 0.5 * state->minHeight;

  if (state->outerXSep < state->outerYSep)
    radius = radius + state->outerYSep;
  else
    radius = radius + state->outerXSep;

  centerPos = centerPoint;
  westPos.setX(x - radius - state->left);
  westPos.setY(y);
  eastPos.setX(x + radius + state->right);
  eastPos.setY(y);
  northPos.setX(x);
  northPos.setY(y + radius + state->above);
  southPos.setX(x);
  southPos.setY(y - radius - state->below);
  switch (state->anchor)
  {
    default:
      {
        double xa = state->anotherPoint.x();
        double ya = state->anotherPoint.y();
        QPointF pd(xa,ya);
        QPointF ur(radius,radius);

        QPointF p = pointBorderEllipse(pd,ur);
        x = p.x() + centerPoint.x();
        y = p.y() + centerPoint.y();
      }
      break;

    case PGFtext:
      x = 0;
      y = 0;
      break;

    case PGFlower:
      x = lowerPoint.x();
      y = lowerPoint.y();
      break;

    case PGFmid:
      y = 0.5 * 4.3;
      break;

    case PGFbase:
      y = 0;
      break;

    case PGFnorth:
    case PGFabove:
      y = northPos.y();
      break;

    case PGFsouth:
    case PGFbelow:
      y = southPos.y();
      break;

    case PGFwest:
    case PGFleft:
      x = westPos.x();
      break;

    case PGFeast:
    case PGFright:
      x = eastPos.x();
      break;

    case PGFmidwest:
      x = x - radius;
      y = 0.5 * 4.3;
      break;

    case PGFmideast:
      x = x + radius;
      y = 0.5 * 4.3;
      break;

    case PGFbasewest:
      x = x - radius;
      y = 0;
      break;

    case PGFbaseeast:
      x = x + radius;
      y = 0;
      break;

    case PGFnorthwest:
    case PGFbelowright:
      x = x - 0.707107 * radius;
      y = y + 0.707107 * radius;
      break;

    case PGFsouthwest:
    case PGFaboveright:
      x = x - 0.707107 * radius;
      y = y - 0.707107 * radius;
      break;

    case PGFnortheast:
    case PGFbelowleft:
      x = x + 0.707107 * radius;
      y = y + 0.707107 * radius;
      break;

    case PGFsoutheast:
    case PGFaboveleft:
      x = x + 0.707107 * radius;
      y = y - 0.707107 * radius;
      break;
  }

  anchorPos.setX(x);
  anchorPos.setY(y);
}

void XWTikzShape::doContent()
{
  if (!driver || !box)
    return ;

  if (state->isFillText)
  {
    driver->setFillColor(state->fillColor);
    driver->fillText();
  }

  QTransform transA = state->transform;
  if (box->isTextBox())
    driver->setTextMatrix(transA.m11(),transA.m12(),transA.m21(),transA.m22(),transA.dx(),transA.dy());
  else
    driver->setMatrix(transA.m11(),transA.m12(),transA.m21(),transA.m22(),transA.dx(),transA.dy());
   
  box->setXY(state->align,centerPoint.x() - 0.5 * boxWidth,centerPoint.y() + 0.5 * boxHeight);
  box->doContent(driver); 
}

void XWTikzShape::rectangleAnchor()
{
  double x,y;
  x = boxWidth + 2 * state->innerXSep;
  if (x < state->minWidth)
    x = state->minWidth;
  x = 0.5 * x + 0.5 * boxWidth + state->outerXSep;
  y = boxHeight + boxDepth + 2 * state->innerYSep;
  if (y < state->minHeight)
    y = state->minHeight;
  y = 0.5 * y - 0.5 * boxDepth + 0.5 * boxHeight + state->outerYSep;
  northEast.setX(x);
  northEast.setY(y);

  x = boxWidth + 2 * state->innerXSep;
  if (x < state->minWidth)
    x = state->minWidth;
  x = -0.5 * x + 0.5 * boxWidth - state->outerXSep;
  y = boxHeight + boxDepth + 2 * state->innerYSep;
  if (y < state->minHeight)
    y = state->minHeight;
  y = -0.5 * y - 0.5 * boxDepth + 0.5 * boxHeight - state->outerYSep;
  southWest.setX(x);
  southWest.setY(y);

  centerPos.setX(0.5 * southWest.x() + 0.5 * northEast.x());
  centerPos.setY(0.5 * southWest.y() + 0.5 * northEast.y());

  westPos.setX(southWest.x() - state->left);
  westPos.setY(0.5 * southWest.y() + 0.5 * northEast.y());
  eastPos.setX(northEast.x());
  eastPos.setY(0.5 * southWest.y() + 0.5 * northEast.y());
  northPos.setX(0.5 * northEast.x() + 0.5 * southWest.x());
  northPos.setY(northEast.y() + state->above);
  southPos.setX(0.5 * southWest.x() + 0.5 * northEast.x());
  southPos.setY(southWest.y() - state->below);

  switch (state->anchor)
  {
    default:
      {
        double xb = state->anotherPoint.x();
        double yb = state->anotherPoint.y();
        double xa = southWest.x();
        double ya = southWest.y();
        double xc = 0.5 * (northEast.x() - xa);
        double yc = 0.5 * (northEast.y() - ya);
        xa += xc;
        ya += yc;
        QPointF pd(xb,yb);
        QPointF ur(xc,yc);
        QPointF p = pointBorderRectangle(pd,ur);
        x = p.x() + xa;
        y = p.y() + ya;
      }
      break;

    case PGFtext:
      anchorPos.setX(0);
      anchorPos.setY(0);
      break;

    case PGFcenter:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = 0.5 * southWest.y() + 0.5 * northEast.y();
      break;

    case PGFmid:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = 0.5 * 4.3;
      break;

    case PGFbase:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = 0;
      break;

    case PGFnorth:
    case PGFabove:
      x = 0.5 * northEast.x() + 0.5 * southWest.x();
      y = northEast.y() + state->above;
      break;

    case PGFsouth:
    case PGFbelow:
      x = 0.5 * southWest.x() + 0.5 * northEast.x();
      y = southWest.y() - state->below;
      break;

    case PGFwest:
    case PGFleft:
      x = southWest.x() - state->left;
      y = 0.5 * southWest.y() + 0.5 * northEast.y();
      break;

    case PGFmidwest:
      x = southWest.x();
      y = 0.5 * 4.3;
      break;

    case PGFbasewest:
      x = southWest.x();
      y = 0;
      break;

    case PGFnorthwest:
      x = southWest.x();
      y = northEast.y();
      break;

    case PGFsouthwest:
      x = southWest.x();
      y = southWest.y();
      break;

    case PGFeast:
    case PGFright:
      x = northEast.x();
      y = 0.5 * southWest.y() + 0.5 * northEast.y();
      break;

    case PGFmideast:
      x = northEast.x();
      y = 0.5 * 4.3;
      break;

    case PGFbaseeast:
      x = northEast.x();
      y = 0;
      break;

    case PGFnortheast:
      x = northEast.x();
      y = northEast.y();
      break;

    case PGFsoutheast:
      x = northEast.x();
      y = southWest.y();
      break;
  }

  anchorPos.setX(x);
  anchorPos.setY(y);
}

void XWTikzShape::setCircuitShape()
{
  switch (state->shape)
  {
    default:
      break;

    case PGFresistor:
      state->shape = PGFresistorIEC;
      break;

    case PGFinductor:
      state->shape = PGFinductorIEC;
      break;

	  case PGFcapacitor:
      state->shape = PGFcapacitorIEC;
      break;

    case PGFcontact:
      state->shape = PGFcontactIEC;
      break;

    case PGFground:
      state->shape = PGFgroundIEC;
      break;

	  case PGFbattery:
      state->shape = PGFbatteryIEC;
      break;

    case PGFdiode:
      state->shape = PGFdiodeIEC;
      break;

    case PGFZenerdiode:
      state->shape = PGFZenerdiodeIEC;
      break;

    case PGFtunneldiode:
      state->shape = PGFtunneldiodeIEC;
      break;

    case PGFbackwarddiode:
      state->shape = PGFbackwarddiodeIEC;
      break;

    case PGFSchottkydiode:
      state->shape = PGFSchottkydiodeIEC;
      break;

    case PGFbreakdowndiode:
      state->shape = PGFbreakdowndiodeIEC;
      break;

	  case PGFbulb:
      state->shape = PGFbulbIEC;
      break;

    case PGFvoltagesource:
      state->shape = PGFvoltagesourceIEC;
      break;

	  case PGFcurrentsource:
      state->shape = PGFcurrentsourceIEC;
      break;

	  case PGFmakecontact:
      state->shape = PGFmakecontactIEC;
      break;

	  case PGFbreakcontact:
      state->shape = PGFbreakcontactIEC;
      break;

    case PGFandgate:
      {
        switch (state->pictureType)
        {
          default:
            state->shape = PGFandgateUS;
            break;

          case PGFcircuitlogicCDH:
            state->shape = PGFandgateCDH;
            break;

          case PGFcircuitlogicIEC:
            state->shape = PGFandgateIEC;
            break;
        }
      }
      break;

    case PGFnandgate:
      {
        switch (state->pictureType)
        {
          default:
            state->shape = PGFnandgateUS;
            break;

          case PGFcircuitlogicCDH:
            state->shape = PGFnandgateCDH;
            break;

          case PGFcircuitlogicIEC:
            state->shape = PGFnandgateIEC;
            break;
        }
      }
      break;

    case PGForgate:
      {
        switch (state->pictureType)
        {
          default:
            state->shape = PGForgateUS;
            break;

          case PGFcircuitlogicIEC:
            state->shape = PGForgateIEC;
            break;
        }
      }
      break;

    case PGFnorgate:
      {
        switch (state->pictureType)
        {
          default:
            state->shape = PGFnorgateUS;
            break;

          case PGFcircuitlogicIEC:
            state->shape = PGFnorgateIEC;
            break;
        }
      }
      break;

    case PGFxorgate:
      {
        switch (state->pictureType)
        {
          default:
            state->shape = PGFxorgateUS;
            break;

          case PGFcircuitlogicIEC:
            state->shape = PGFxorgateIEC;
            break;
        }
      }
      break;

    case PGFxnorgate:
      {
        switch (state->pictureType)
        {
          default:
            state->shape = PGFxnorgateUS;
            break;

          case PGFcircuitlogicIEC:
            state->shape = PGFxnorgateIEC;
            break;
        }
      }
      break;

    case PGFnotgate:
      {
        switch (state->pictureType)
        {
          default:
            state->shape = PGFnotgateUS;
            break;

          case PGFcircuitlogicIEC:
            state->shape = PGFnotgateIEC;
            break;
        }
      }
      break;

    case PGFbuffergate:
      {
        switch (state->pictureType)
        {
          default:
            state->shape = PGFbuffergateUS;
            break;

          case PGFcircuitlogicIEC:
            state->shape = PGFbuffergateIEC;
            break;
        }
      }
      break;
  }
}

void XWTikzShape::doBeforeBackground()
{
  if (sh_beforebackground != NULL)
    (this->*(sh_beforebackground))();    
}

void XWTikzShape::doForeground()
{
  if (sh_foreground != NULL)
    (this->*(sh_foreground))();
}
