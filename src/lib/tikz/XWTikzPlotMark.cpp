/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "PGFKeyWord.h"
#include "XWTikzState.h"
#include "XWTikzPlotMark.h"

XWTikzPlotMark::XWTikzPlotMark(int ptypeA)
:ptype(ptypeA)
{}

void XWTikzPlotMark::doMark(XWTikzState * stateA)
{
  switch (ptype)
  {
    default:
      break;

    case PGFball:
      doBall(stateA);
      break;
  }
}

void XWTikzPlotMark::doBall(XWTikzState * stateA)
{
  double r = stateA->getMarkSize();
  stateA->setRadius(r);
  stateA->moveTo(0,0);
  stateA->addEllipse();
  stateA->setShade(true);
}
