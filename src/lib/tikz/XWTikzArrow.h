/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTIKZARROW_H
#define XWTIKZARROW_H

class XWTikzState;

class XWTikzArrow
{
public:
  XWTikzArrow();

  void doArrow(XWTikzState * state);

  void initPath(XWTikzState * state, bool isend);

private:
  void barArrow(XWTikzState * state);

  void latexArrow(XWTikzState * state);

  void stealthArrow(XWTikzState * state);

  void toArrow(XWTikzState * state);
  void toReversedArrow(XWTikzState * state);

public:
  int shape;
};

#endif //XWTIKZARROW_H
