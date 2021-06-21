/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWNumberUtil.h"
#include "XWObject.h"
#include "PGFKeyWord.h"
#include "XWPDFDriver.h"
#include "tikzcolor.h"
#include "XWTikzState.h"
#include "XWTikzFading.h"

XWTikzFading::XWTikzFading(XWPDFDriver * driverA,
                           int ftypeA,
                           const QTransform & transformA)
:driver(driverA),
 ftype(ftypeA),
 transform(transformA)
{}

void XWTikzFading::doFading(XWTikzState * state)
{
  switch (ftype)
  {
    default:
      break;

    case PGFeast:
      doEast(state);
      break;

    case PGFwest:
      doWest(state);
      break;

    case PGFnorth:
      doNorth(state);
      break;

    case PGFsouth:
      doSouth(state);
      break;

    case PGFcirclewithfuzzyedge10percent:
      circleWithFuzzyEdge10Percent(state);
      break;

	  case PGFcirclewithfuzzyedge15percent:
      circleWithFuzzyEdge15Percent(state);
      break;

	  case PGFcirclewithfuzzyedge20percent:
      circleWithFuzzyEdge20Percent(state);
      break;

	  case PGFfuzzyring15percent:
      doFuzzyRing15Percent(state);
      break;
  }
}

void XWTikzFading::circleWithFuzzyEdge10Percent(XWTikzState * state)
{
  QString fname("circle with fuzzy edge 10 percent");
  XWObject ref;
  if (!driver->hasXObject(fname,&ref))
  {
    double w = state->pathMaxX - state->pathMinX;
    double h = state->pathMaxY - state->pathMinY;
    XWObject bbox;
    driver->initBBox(0,0,w,h,&bbox);
    driver->beginFading(fname,&bbox,0,0,&ref);
    QString sname("shading circle with fuzzy edge 10 percent");
    XWObject obj;
    if (!driver->hasXObject(sname,&obj))
    {
      double funs[4][4];
      funs[0][0] = 0;
      QColor rgb = calulateColor(Qt::black,0);
      funs[0][1] = round(rgb.redF(), 0.001);
      funs[0][2] = round(rgb.greenF(), 0.001);
      funs[0][3] = round(rgb.blueF(), 0.001);

      funs[1][0] = 22.584375;
      funs[1][1] = round(rgb.redF(), 0.001);
      funs[1][2] = round(rgb.greenF(), 0.001);
      funs[1][3] = round(rgb.blueF(), 0.001);

      rgb = calulateColor(Qt::black,1);
      funs[2][0] = 25.09375;
      funs[2][1] = round(rgb.redF(), 0.001);
      funs[2][2] = round(rgb.greenF(), 0.001);
      funs[2][3] = round(rgb.blueF(), 0.001);

      funs[3][0] = 50.1875;
      funs[3][1] = round(rgb.redF(), 0.001);
      funs[3][2] = round(rgb.greenF(), 0.001);
      funs[3][3] = round(rgb.blueF(), 0.001);

      QPointF p(0,0);      
      driver->shadingRadial(sname,p,funs,4,&obj);
    }
    
    driver->useXObject(sname);
    driver->endXForm();
  }

  useFading(fname, state,&ref);
}

void XWTikzFading::circleWithFuzzyEdge15Percent(XWTikzState * state)
{
  QString fname("circle with fuzzy edge 15 percent");
  XWObject ref;
  if (!driver->hasXObject(fname,&ref))
  {
    double w = state->pathMaxX - state->pathMinX;
    double h = state->pathMaxY - state->pathMinY;
    XWObject bbox;
    driver->initBBox(0,0,w,h,&bbox);
    driver->beginFading(fname,&bbox,0,0,&ref);
    QString sname("shading circle with fuzzy edge 15 percent");
    XWObject obj;
    if (!driver->hasXObject(sname,&obj))
    {
      double funs[4][4];
      funs[0][0] = 0;
      QColor rgb = calulateColor(Qt::black,0);
      funs[0][1] = round(rgb.redF(), 0.001);
      funs[0][2] = round(rgb.greenF(), 0.001);
      funs[0][3] = round(rgb.blueF(), 0.001);

      funs[1][0] = 21.580625;
      funs[1][1] = round(rgb.redF(), 0.001);
      funs[1][2] = round(rgb.greenF(), 0.001);
      funs[1][3] = round(rgb.blueF(), 0.001);

      rgb = calulateColor(Qt::black,1);
      funs[2][0] = 25.09375;
      funs[2][1] = round(rgb.redF(), 0.001);
      funs[2][2] = round(rgb.greenF(), 0.001);
      funs[2][3] = round(rgb.blueF(), 0.001);

      funs[3][0] = 50.1875;
      funs[3][1] = round(rgb.redF(), 0.001);
      funs[3][2] = round(rgb.greenF(), 0.001);
      funs[3][3] = round(rgb.blueF(), 0.001);

      QPointF p(0,0);
      driver->shadingRadial(sname,p,funs,4,&obj);
    }
    
    driver->useXObject(sname);
    driver->endXForm();
  }

  useFading(fname, state,&ref);
}

void XWTikzFading::circleWithFuzzyEdge20Percent(XWTikzState * state)
{
  QString fname("circle with fuzzy edge 20 percent");
  XWObject ref;
  if (!driver->hasXObject(fname,&ref))
  {
    double w = state->pathMaxX - state->pathMinX;
    double h = state->pathMaxY - state->pathMinY;
    XWObject bbox;
    driver->initBBox(0,0,w,h,&bbox);
    driver->beginFading(fname,&bbox,0,0,&ref);
    QString sname("shading circle with fuzzy edge 20 percent");
    XWObject obj;
    if (!driver->hasXObject(sname,&obj))
    {
      double funs[4][4];
      funs[0][0] = 0;
      QColor rgb = calulateColor(Qt::black,0);
      funs[0][1] = round(rgb.redF(), 0.001);
      funs[0][2] = round(rgb.greenF(), 0.001);
      funs[0][3] = round(rgb.blueF(), 0.001);

      funs[1][0] = 20.075;
      funs[1][1] = round(rgb.redF(), 0.001);
      funs[1][2] = round(rgb.greenF(), 0.001);
      funs[1][3] = round(rgb.blueF(), 0.001);

      rgb = calulateColor(Qt::black,1);
      funs[2][0] = 25.09375;
      funs[2][1] = round(rgb.redF(), 0.001);
      funs[2][2] = round(rgb.greenF(), 0.001);
      funs[2][3] = round(rgb.blueF(), 0.001);

      funs[3][0] = 50.1875;
      funs[3][1] = round(rgb.redF(), 0.001);
      funs[3][2] = round(rgb.greenF(), 0.001);
      funs[3][3] = round(rgb.blueF(), 0.001);

      QPointF p(0,0);      
      driver->shadingRadial(sname,p,funs,4,&obj);
    }
    
    driver->useXObject(sname);
    driver->endXForm();
  }

  useFading(fname, state,&ref);
}

void XWTikzFading::doEast(XWTikzState * state)
{
  QString fname("east");
  XWObject ref;
  if (!driver->hasXObject(fname,&ref))
  {
    double w = state->pathMaxX - state->pathMinX;
    double h = state->pathMaxY - state->pathMinY;
    XWObject bbox;
    driver->initBBox(0,0,w,h,&bbox);
    driver->beginFading(fname,&bbox,0,0,&ref);
    QString sname("shading east");
    XWObject obj;
    if (!driver->hasXObject(sname,&obj))
    {
      double funs[4][4];
      funs[0][0] = 0;
      QColor rgb = calulateColor(Qt::black,0);
      funs[0][1] = round(rgb.redF(), 0.001);
      funs[0][2] = round(rgb.greenF(), 0.001);
      funs[0][3] = round(rgb.blueF(), 0.001);

      funs[1][0] = 25.09375;
      funs[1][1] = round(rgb.redF(), 0.001);
      funs[1][2] = round(rgb.greenF(), 0.001);
      funs[1][3] = round(rgb.blueF(), 0.001);

      rgb = calulateColor(Qt::black,1);
      funs[2][0] = 75.28125;
      funs[2][1] = round(rgb.redF(), 0.001);
      funs[2][2] = round(rgb.greenF(), 0.001);
      funs[2][3] = round(rgb.blueF(), 0.001);

      funs[3][0] = 100.375;
      funs[3][1] = round(rgb.redF(), 0.001);
      funs[3][2] = round(rgb.greenF(), 0.001);
      funs[3][3] = round(rgb.blueF(), 0.001);

      driver->shadingHoriVert(sname,true,100.375,funs,4,&obj);
    }
    
    driver->useXObject(sname);
    driver->endXForm();
  }

  useFading(fname, state,&ref);
}

void XWTikzFading::doFuzzyRing15Percent(XWTikzState * state)
{
  QString fname("fuzzy ring 15 percent");
  XWObject ref;
  if (!driver->hasXObject(fname,&ref))
  {
    double w = state->pathMaxX - state->pathMinX;
    double h = state->pathMaxY - state->pathMinY;
    XWObject bbox;
    driver->initBBox(0,0,w,h,&bbox);
    driver->beginFading(fname,&bbox,0,0,&ref);
    QString sname("shading fuzzy ring 15 percent");
    XWObject obj;
    if (!driver->hasXObject(sname,&obj))
    {
      double funs[5][4];
      funs[0][0] = 0;
      QColor rgb = calulateColor(Qt::black,1);
      funs[0][1] = round(rgb.redF(), 0.001);
      funs[0][2] = round(rgb.greenF(), 0.001);
      funs[0][3] = round(rgb.blueF(), 0.001);

      funs[1][0] = 21.3296875;
      funs[1][1] = round(rgb.redF(), 0.001);
      funs[1][2] = round(rgb.greenF(), 0.001);
      funs[1][3] = round(rgb.blueF(), 0.001);

      rgb = calulateColor(Qt::black,0);
      funs[2][0] = 23.21171875;
      funs[2][1] = round(rgb.redF(), 0.001);
      funs[2][2] = round(rgb.greenF(), 0.001);
      funs[2][3] = round(rgb.blueF(), 0.001);

      rgb = calulateColor(Qt::black,1);
      funs[3][0] = 25.09375;
      funs[3][1] = round(rgb.redF(), 0.001);
      funs[3][2] = round(rgb.greenF(), 0.001);
      funs[3][3] = round(rgb.blueF(), 0.001);

      funs[4][0] = 50.1875;
      funs[4][1] = round(rgb.redF(), 0.001);
      funs[4][2] = round(rgb.greenF(), 0.001);
      funs[4][3] = round(rgb.blueF(), 0.001);

      QPointF p(0,0);
      driver->shadingRadial(sname,p,funs,5,&obj);
    }
    
    driver->useXObject(sname);
    driver->endXForm();
  }

  useFading(fname, state,&ref);
}

void XWTikzFading::doNorth(XWTikzState * state)
{
  QString fname("north");
  XWObject ref;
  if (!driver->hasXObject(fname,&ref))
  {
    double w = state->pathMaxX - state->pathMinX;
    double h = state->pathMaxY - state->pathMinY;
    XWObject bbox;
    driver->initBBox(0,0,w,h,&bbox);
    driver->beginFading(fname,&bbox,0,0,&ref);
    QString sname("shading north");
    XWObject obj;
    if (!driver->hasXObject(sname,&obj))
    {
      double funs[4][4];
      funs[0][0] = 0;
      QColor rgb = calulateColor(Qt::black,0);
      funs[0][1] = round(rgb.redF(), 0.001);
      funs[0][2] = round(rgb.greenF(), 0.001);
      funs[0][3] = round(rgb.blueF(), 0.001);

      funs[1][0] = 25.09375;
      funs[1][1] = round(rgb.redF(), 0.001);
      funs[1][2] = round(rgb.greenF(), 0.001);
      funs[1][3] = round(rgb.blueF(), 0.001);

      rgb = calulateColor(Qt::black,1);
      funs[2][0] = 75.28125;
      funs[2][1] = round(rgb.redF(), 0.001);
      funs[2][2] = round(rgb.greenF(), 0.001);
      funs[2][3] = round(rgb.blueF(), 0.001);

      funs[3][0] = 100.375;
      funs[3][1] = round(rgb.redF(), 0.001);
      funs[3][2] = round(rgb.greenF(), 0.001);
      funs[3][3] = round(rgb.blueF(), 0.001);

      driver->shadingHoriVert(sname,false,100.375,funs,4,&obj);
    }
    
    driver->useXObject(sname);
    driver->endXForm();
  }

  useFading(fname, state,&ref);
}

void XWTikzFading::doSouth(XWTikzState * state)
{
  QString fname("south");
  XWObject ref;
  if (!driver->hasXObject(fname,&ref))
  {
    double w = state->pathMaxX - state->pathMinX;
    double h = state->pathMaxY - state->pathMinY;
    XWObject bbox;
    driver->initBBox(0,0,w,h,&bbox);
    driver->beginFading(fname,&bbox,0,0,&ref);
    QString sname("shading south");
    XWObject obj;
    if (!driver->hasXObject(sname,&obj))
    {
      double funs[4][4];
      funs[0][0] = 0;
      QColor rgb = calulateColor(Qt::black,1);
      funs[0][1] = round(rgb.redF(), 0.001);
      funs[0][2] = round(rgb.greenF(), 0.001);
      funs[0][3] = round(rgb.blueF(), 0.001);

      funs[1][0] = 25.09375;
      funs[1][1] = round(rgb.redF(), 0.001);
      funs[1][2] = round(rgb.greenF(), 0.001);
      funs[1][3] = round(rgb.blueF(), 0.001);

      rgb = calulateColor(Qt::black,0);
      funs[2][0] = 75.28125;
      funs[2][1] = round(rgb.redF(), 0.001);
      funs[2][2] = round(rgb.greenF(), 0.001);
      funs[2][3] = round(rgb.blueF(), 0.001);

      funs[3][0] = 100.375;
      funs[3][1] = round(rgb.redF(), 0.001);
      funs[3][2] = round(rgb.greenF(), 0.001);
      funs[3][3] = round(rgb.blueF(), 0.001);

      driver->shadingHoriVert(sname,false,100.375,funs,4,&obj);
    }
    
    driver->useXObject(sname);
    driver->endXForm();
  }

  useFading(fname, state,&ref);
}

void XWTikzFading::doWest(XWTikzState * state)
{
  QString fname("west");
  XWObject ref;
  if (!driver->hasXObject(fname,&ref))
  {
    double w = state->pathMaxX - state->pathMinX;
    double h = state->pathMaxY - state->pathMinY;
    XWObject bbox;
    driver->initBBox(0,0,w,h,&bbox);
    driver->beginFading(fname,&bbox,0,0,&ref);
    QString sname("shading west");
    XWObject obj;
    if (!driver->hasXObject(sname,&obj))
    {
      double funs[4][4];
      funs[0][0] = 0;
      QColor rgb = calulateColor(Qt::black,1);
      funs[0][1] = round(rgb.redF(), 0.001);
      funs[0][2] = round(rgb.greenF(), 0.001);
      funs[0][3] = round(rgb.blueF(), 0.001);

      funs[1][0] = 25.09375;
      funs[1][1] = round(rgb.redF(), 0.001);
      funs[1][2] = round(rgb.greenF(), 0.001);
      funs[1][3] = round(rgb.blueF(), 0.001);

      rgb = calulateColor(Qt::black,0);
      funs[2][0] = 75.28125;
      funs[2][1] = round(rgb.redF(), 0.001);
      funs[2][2] = round(rgb.greenF(), 0.001);
      funs[2][3] = round(rgb.blueF(), 0.001);

      funs[3][0] = 100.375;
      funs[3][1] = round(rgb.redF(), 0.001);
      funs[3][2] = round(rgb.greenF(), 0.001);
      funs[3][3] = round(rgb.blueF(), 0.001);

      driver->shadingHoriVert(sname,true,100.375,funs,4,&obj);
    }
    
    driver->useXObject(sname);
    driver->endXForm();
  }

  useFading(fname, state,&ref);
}

void XWTikzFading::useFading(const QString & fname,XWTikzState * state,XWObject * stm)
{
  double dx = 0.5 * (state->pathMinX - state->pathMaxX);
  double dy = 0.5 * (state->pathMinY - state->pathMaxY);

  QTransform transform1(1,0,0,1,dx,dy);
  QTransform trans = transform * transform1;
  driver->setMatrix(trans.m11(),trans.m12(),trans.m21(),trans.m22(),trans.dx(),trans.dy());
  driver->useFading(fname,stm);
  trans = trans.inverted();
  driver->setMatrix(trans.m11(),trans.m12(),trans.m21(),trans.m22(),trans.dx(),trans.dy());
}
