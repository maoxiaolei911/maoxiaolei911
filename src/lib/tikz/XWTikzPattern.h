/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTIKZPATTERN_H
#define XWTIKZPATTERN_H

class XWPDFDriver;
class XWTikzState;
class XWObject;

class XWTikzPattern
{
public:
  XWTikzPattern(XWPDFDriver * driverA, int ptypeA);

  void doPattern(XWTikzState * state);

private:
  void doBricks(XWTikzState * state);
  void doCheckerBoard(XWTikzState * state);
  void doCheckerBoardLightGray(XWTikzState * state);
  void doCrosshatch(XWTikzState * state);
  void doCrosshatchDots(XWTikzState * state);
  void doCrosshatchDotsGray(XWTikzState * state);
  void doCrosshatchDotsLightSteelBlue(XWTikzState * state);
  void doDots(XWTikzState * state);
  void doFivePointedStars(XWTikzState * state);
  void doGrid(XWTikzState * state);
  void doHorizontalLines(XWTikzState * state);
  void doHorizontalLinesDarkBlue(XWTikzState * state);
  void doHorizontalLinesDarkGray(XWTikzState * state);
  void doHorizontalLinesGray(XWTikzState * state);
  void doHorizontalLinesLightBlue(XWTikzState * state);
  void doHorizontalLinesLightGray(XWTikzState * state);
  void doNorthEastLines(XWTikzState * state);
  void doNorthWestLines(XWTikzState * state);
  void doSixPointedStars(XWTikzState * state);
  void doVerticalLines(XWTikzState * state);

  void usePattern(const QString & name,XWTikzState * state);

private:
  XWPDFDriver * driver;
  int ptype;
};

#endif //
