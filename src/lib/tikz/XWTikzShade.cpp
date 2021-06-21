/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include "XWNumberUtil.h"
#include "XWObject.h"
#include "PGFKeyWord.h"
#include "XWPDFDriver.h"
#include "tikzcolor.h"
#include "XWTikzState.h"
#include "XWTikzShade.h"

XWTikzShade::XWTikzShade(XWPDFDriver * driverA,int stypeA, double angleA)
:driver(driverA),
 stype(stypeA),
 angle(angleA)
{}

void XWTikzShade::doShading(XWTikzState * state)
{
  if (stype <= 0)
    return ;

  switch (stype)
  {
    default:
      break;

    case PGFaxis:
      doAxis(state);
      break;

    case PGFball:
      doBall(state);
      break;

    case PGFradial:
      doRadial(state);
      break;

    case PGFcolorwheelwhitecenter:
      doColorWheelWhiteCenter(state);
      break;

    case PGFcolorwheelblackcenter:
      doColorWheelBlackCenter(state);
      break;

    case PGFcolorwheel:
      doColorWheel(state);
      break;

    case PGFbilinearinterpolation:
      doBilinearInterpolation(state);
      break;

    case PGFMandelbrotset:
      doMandelbrotSet(state);
      break;
  }
}

void XWTikzShade::doAxis(XWTikzState * state)
{
  QString sname("axis");
  XWObject ref;
  if (!driver->hasXObject(sname,&ref))
  {
    double funs[5][4];
    funs[0][0] = 0;
    QColor rgb = state->bottomColor;
    funs[0][1] = round(rgb.redF(), 0.001);
    funs[0][2] = round(rgb.greenF(), 0.001);
    funs[0][3] = round(rgb.blueF(), 0.001);

    funs[1][0] = 25.094;
    funs[1][1] = funs[0][1];
    funs[1][2] = funs[0][2];
    funs[1][3] = funs[0][3];

    funs[2][0] = 50.188;
    rgb = state->middleColor;
    funs[2][1] = round(rgb.redF(), 0.001);
    funs[2][2] = round(rgb.greenF(), 0.001);
    funs[2][3] = round(rgb.blueF(), 0.001);

    funs[3][0] = 75.281;
    rgb = state->topColor;
    funs[3][1] = round(rgb.redF(), 0.001);
    funs[3][2] = round(rgb.greenF(), 0.001);
    funs[3][3] = round(rgb.blueF(), 0.001);

    funs[4][0] = 100.375;
    funs[4][1] = funs[3][1];
    funs[4][2] = funs[3][2];
    funs[4][3] = funs[3][3];

    driver->shadingHoriVert(sname,false,100.375,funs,5,&ref);
  }
  shadePath(sname,state);
}

void XWTikzShade::doBall(XWTikzState * state)
{
  QString sname("ball");
  XWObject ref;
  if (!driver->hasXObject(sname,&ref))
  {
    double funs[5][4];
    funs[0][0] = 0;
    QColor color = state->ballColor;
    QColor rgb = calulateColor(color,0.15,Qt::white);
    funs[0][1] = round(rgb.redF(), 0.001);
    funs[0][2] = round(rgb.greenF(), 0.001);
    funs[0][3] = round(rgb.blueF(), 0.001);

    funs[1][0] = 9.034;
    rgb = calulateColor(color,0.75,Qt::white);
    funs[1][1] = round(rgb.redF(), 0.001);
    funs[1][2] = round(rgb.greenF(), 0.001);
    funs[1][3] = round(rgb.blueF(), 0.001);

    funs[2][0] = 18.068;
    rgb = calulateColor(color,0.70,Qt::black);
    funs[2][1] = round(rgb.redF(), 0.001);
    funs[2][2] = round(rgb.greenF(), 0.001);
    funs[2][3] = round(rgb.blueF(), 0.001);

    funs[3][0] = 25.094;
    rgb = calulateColor(color,0.50,Qt::black);
    funs[3][1] = round(rgb.redF(), 0.001);
    funs[3][2] = round(rgb.greenF(), 0.001);
    funs[3][3] = round(rgb.blueF(), 0.001);

    funs[4][0] = 50.188;
    funs[4][1] = 0;
    funs[4][2] = 0;
    funs[4][3] = 0;

    QPointF p(-10.0375,10.0375);
    driver->shadingRadial(sname,p,funs,5,&ref);
  }
  shadePath(sname,state);
}

void XWTikzShade::doBilinearInterpolation(XWTikzState * state)
{
#define BILINEAR_INTERPOLATION "\
25 sub 50 div exch 25 sub 50 div 2 copy neg 1 add exch neg 1 add "
  QString sname("bilinear interpolation");
  XWObject ref;
  if (!driver->hasXObject(sname,&ref))
  {
    QColor llrgb = state->lowerLeftColor;
    QColor lrrgb = state->lowerRightColor;
    QColor ulrgb = state->upperLeftColor;
    QColor urrgb = state->upperRightColor;
    char buf[2000];
    int len = sprintf(buf,BILINEAR_INTERPOLATION);
    len += sprintf(buf + len, "%g mul exch ",round(llrgb.redF(), 0.001));
    len += sprintf(buf + len, "%g mul add mul 4 1 roll ",round(lrrgb.redF(), 0.001));
    len += sprintf(buf + len, "%g  mul exch ",round(urrgb.redF(), 0.001));
    len += sprintf(buf + len, "%g mul add mul 13 1 roll ",round(ulrgb.redF(), 0.001));
    len += sprintf(buf + len, "%g  mul exch ",round(llrgb.greenF(), 0.001));
    len += sprintf(buf + len, "%g mul add mul 4 1 roll ",round(lrrgb.greenF(), 0.001));
    len += sprintf(buf + len, "%g  mul exch ",round(urrgb.greenF(), 0.001));
    len += sprintf(buf + len, "%g mul add mul 7 1 roll ",round(ulrgb.greenF(), 0.001));
    len += sprintf(buf + len, "%g  mul exch ",round(llrgb.blueF(), 0.001));
    len += sprintf(buf + len, "%g mul add mul 4 1 roll ",round(lrrgb.blueF(), 0.001));
    len += sprintf(buf + len, "%g  mul exch ",round(urrgb.blueF(), 0.001));
    len += sprintf(buf + len, "%g mul add mul add\n",round(ulrgb.blueF(), 0.001));
    QPointF ll(0,0);
    QPointF ur(100.375,100.375);
    driver->shadingFunc(sname,ll,ur,buf,len,&ref);
  }
  shadePath(sname,state);
}

void XWTikzShade::doColorWheel(XWTikzState * state)
{
#define COLOR_WHEEL "\
2 copy abs exch abs add 0.0001 ge \
{atan 360.0 div} \
{ pop } \
ifelse  \
1.0 1.0 \
3 2 roll 6.0 mul dup 4 1 roll \
floor cvr  \
dup 5 1 roll \
3 index sub neg \
1.0 3 index sub \
2 index mul \
6 1 roll \
dup 3 index mul neg 1.0 add \
2 index mul \
7 1 roll \
neg 1.0 add \
2 index mul neg 1.0  add \
1 index mul \
7 2 roll \
pop pop \
dup 0.5 le \
{ \
  pop exch pop\
}\
{ dup 1.5 le \
  { \
    pop exch 4 1 roll exch pop\
  }\
  { dup 2.5 le \
    { \
      pop 4 1 roll pop\
    }\
    { dup 3.5 le \
      { \
        pop exch 4 2 roll pop\
      }\
      { dup 4.5 le \
        { \
          pop exch pop 3 -1 roll\
        }\
        { \
          pop 3 1 roll exch pop\
        }\
        ifelse\
      }\
      ifelse\
    }\
    ifelse\
  }\
  ifelse\
}\
ifelse\n"
  QString sname("color wheel");
  XWObject ref;
  if (!driver->hasXObject(sname,&ref))
  {
    QPointF ll(-50.188,-50.188);
    QPointF ur(50.188,50.188);
    driver->shadingFunc(sname,ll,ur,COLOR_WHEEL,strlen(COLOR_WHEEL),&ref);
  }
  shadePath(sname,state);
}

void XWTikzShade::doColorWheelBlackCenter(XWTikzState * state)
{
#define COLOR_WHEEL_BLACK_CENTER "\
2 copy \
2 copy abs exch abs add 0.0001 ge \
{atan 360.0 div} \
{ pop } \
ifelse  \
3 1 roll \
dup mul \
exch dup mul \
add sqrt \
25.0 div \
dup 1.0 ge \
{ pop 1.0 }{} ifelse \
1.0 exch \
3 2 roll 6.0 mul dup 4 1 roll \
floor cvr  \
dup 5 1 roll \
3 index sub neg \
1.0 3 index sub \
2 index mul \
6 1 roll \
dup 3 index mul neg 1.0 add \
2 index mul \
7 1 roll \
neg 1.0 add \
2 index mul neg 1.0  add \
1 index mul \
7 2 roll \
pop pop \
dup 0.5 le \
{ \
  pop exch pop\
}\
{ dup 1.5 le \
  { \
    pop exch 4 1 roll exch pop\
  }\
  { dup 2.5 le \
    { \
      pop 4 1 roll pop\
    }\
    { dup 3.5 le \
      { \
        pop exch 4 2 roll pop\
      }\
      { dup 4.5 le \
        { \
          pop exch pop 3 -1 roll\
        }\
        { \
          pop 3 1 roll exch pop\
        }\
        ifelse\
      }\
      ifelse\
    }\
    ifelse\
  }\
  ifelse\
}\
ifelse\n"
  QString sname("color wheel black center");
  XWObject ref;
  if (!driver->hasXObject(sname,&ref))
  {
    QPointF ll(-50.188,-50.188);
    QPointF ur(50.188,50.188);
    driver->shadingFunc(sname,ll,ur,COLOR_WHEEL_BLACK_CENTER,strlen(COLOR_WHEEL_BLACK_CENTER),&ref);
  }
  shadePath(sname,state);
}

void XWTikzShade::doColorWheelWhiteCenter(XWTikzState * state)
{
#define COLOR_WHEEL_WHITE_CENTER "\
2 copy \
2 copy abs exch abs add 0.0001 ge \
{atan 360.0 div} \
{ pop } \
ifelse \
3 1 roll \
dup mul \
exch dup mul \
add sqrt \
25.0 div \
dup 1.0 ge \
{ pop 1.0 }{} ifelse \
1.0 \
3 2 roll 6.0 mul dup 4 1 roll \
floor cvr  \
dup 5 1 roll \
3 index sub neg \
1.0 3 index sub \
2 index mul \
6 1 roll \
dup 3 index mul neg 1.0 add \
2 index mul \
7 1 roll \
neg 1.0 add \
2 index mul neg 1.0  add \
1 index mul \
7 2 roll \
pop pop \
dup 0.5 le \
{ \
  pop exch pop\
}\
{ dup 1.5 le \
  { \
    pop exch 4 1 roll exch pop\
  }\
  { dup 2.5 le \
    { \
      pop 4 1 roll pop\
    }\
    { dup 3.5 le \
      { \
        pop exch 4 2 roll pop\
      }\
      { dup 4.5 le \
        { \
          pop exch pop 3 -1 roll\
        }\
        { \
          pop 3 1 roll exch pop\
        }\
        ifelse\
      }\
      ifelse\
    }\
    ifelse\
  }\
  ifelse\
}\
ifelse\n"
  QString sname("color wheel white center");
  XWObject ref;
  if (!driver->hasXObject(sname,&ref))
  {
    QPointF ll(-50.188,-50.188);
    QPointF ur(50.188,50.188);
    driver->shadingFunc(sname,ll,ur,COLOR_WHEEL_WHITE_CENTER,strlen(COLOR_WHEEL_WHITE_CENTER),&ref);
  }
  shadePath(sname,state);
}

void XWTikzShade::doMandelbrotSet(XWTikzState * state)
{
#define MANDELBROT_SET "\
12.5 div exch 12.5 div exch \
1 index 1 index \
1 index dup mul \
1 index dup mul \
sub  \
4 index add  \
3 1 roll \
mul 2 mul   \
2 index add  \
1 index dup mul \
1 index dup mul \
sub \
4 index add \
3 1 roll \
mul 2 mul \
2 index add \
1 index dup mul \
1 index dup mul \
sub  \
4 index add  \
3 1 roll \
mul 2 mul   \
2 index add   \
1 index dup mul \
1 index dup mul \
sub    \
4 index add  \
3 1 roll \
mul 2 mul  \
2 index add \
1 index dup mul 1 index dup mul add \
4 lt { \
1 index dup mul \
1 index dup mul \
sub  \
4 index add \
3 1 roll \
mul 2 mul  \
2 index add  \
1 index dup mul \
1 index dup mul \
sub   \
4 index add   \
3 1 roll \
mul 2 mul  \
2 index add \
1 index dup mul \
1 index dup mul \
sub     \
4 index add   \
3 1 roll \
mul 2 mul  \
2 index add  \
1 index dup mul \
1 index dup mul \
sub   \
4 index add \
3 1 roll \
mul 2 mul  \
2 index add   \
1 index dup mul \
1 index dup mul \
sub  \
4 index add  \
3 1 roll \
mul 2 mul \
2 index add \
1 index dup mul \
1 index dup mul \
sub  \
4 index add   \
3 1 roll \
mul 2 mul \
2 index add \
} { pop pop 1000.0 1000.0 } ifelse \
dup mul exch \
dup mul \
add sqrt \
dup 4 1 roll \
2 gt { pop pop 2.0 exch div 1.0 exch sub dup dup} {pop pop 0.0 0.0 0.0} ifelse\n"
  QString sname("Mandelbrot set");
  XWObject ref;
  if (!driver->hasXObject(sname,&ref))
  {
    QPointF ll(-50.188,-50.188);
    QPointF ur(50.188,50.188);
    driver->shadingFunc(sname,ll,ur,MANDELBROT_SET,strlen(MANDELBROT_SET),&ref);
  }
  shadePath(sname,state);
}

void XWTikzShade::doRadial(XWTikzState * state)
{
  QString sname("radial");
  XWObject ref;
  if (!driver->hasXObject(sname,&ref))
  {
    double funs[5][4];
    funs[0][0] = 0;
    QColor rgb = state->innerColor;
    funs[0][1] = round(rgb.redF(), 0.001);
    funs[0][2] = round(rgb.greenF(), 0.001);
    funs[0][3] = round(rgb.blueF(), 0.001);

    funs[1][0] = 25.094;
    rgb = state->outerColor;
    funs[1][1] = round(rgb.redF(), 0.001);
    funs[1][2] = round(rgb.greenF(), 0.001);
    funs[1][3] = round(rgb.blueF(), 0.001);

    funs[2][0] = 50.188;
    rgb = state->outerColor;
    funs[2][1] = round(rgb.redF(), 0.001);
    funs[2][2] = round(rgb.greenF(), 0.001);
    funs[2][3] = round(rgb.blueF(), 0.001);

    QPointF p(0,0);
    driver->shadingRadial(sname,p,funs,3,&ref);
  }
  shadePath(sname,state);
}

void XWTikzShade::shadePath(const QString & shadingname, XWTikzState * state)
{
  double xb = 0.5 * (state->pathMaxX + state->pathMinX);
  double yb = 0.5 * (state->pathMaxY + state->pathMinY);
  double xc = 0.01992528 * (state->pathMaxX - state->pathMinX);
  double yc = 0.01992528 * (state->pathMaxY - state->pathMinY);

  QTransform transform1(1,0,0,1,xb,yb);
  QTransform transform2(xc,0,0,yc,0,0);
  double shsin = sin(angle);
  double shcos = cos(angle);
  double xa = -shcos;
  QTransform transform3(shcos,shsin,xa,shcos,0,0);
  QTransform transform = transform1 * transform2 * transform3;
  driver->gsave();
  driver->setMatrix(transform.m11(),transform.m12(),
                    transform.m21(),transform.m22(),
                    transform.dx(),transform.dy());
  driver->useXObject(shadingname);
  driver->grestore();
}
