/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTIKZSHADE_H
#define XWTIKZSHADE_H

class XWPDFDriver;
class XWTikzState;

class XWTikzShade
{
public:
  XWTikzShade(XWPDFDriver * driverA,int stypeA, double angleA);

  void doShading(XWTikzState * state);

private:
  void doAxis(XWTikzState * state);
  void doBall(XWTikzState * state);
  void doBilinearInterpolation(XWTikzState * state);
  void doColorWheel(XWTikzState * state);
  void doColorWheelBlackCenter(XWTikzState * state);
  void doColorWheelWhiteCenter(XWTikzState * state);
  void doMandelbrotSet(XWTikzState * state);
  void doRadial(XWTikzState * state);
  
  void shadePath(const QString & shadingname, XWTikzState * state);
private:
  XWPDFDriver * driver;
  int stype;
  double angle;
};

#endif //
