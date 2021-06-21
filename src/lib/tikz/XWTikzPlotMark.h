/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTIKZPLOTMARK_H
#define XWTIKZPLOTMARK_H

class XWTikzState;

class XWTikzPlotMark
{
public:
  XWTikzPlotMark(int ptypeA);

  void doMark(XWTikzState * stateA);

private:
  void doBall(XWTikzState * stateA);

private:
  int ptype;
};

#endif //XWTIKZPLOTMARK_H
