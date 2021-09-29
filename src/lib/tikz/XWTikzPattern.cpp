/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWObject.h"
#include "XWNumberUtil.h"
#include "PGFKeyWord.h"
#include "XWPDFDriver.h"
#include "tikzcolor.h"
#include "XWTikzState.h"
#include "XWTikzPattern.h"

XWTikzPattern::XWTikzPattern(XWPDFDriver * driverA, int ptypeA)
:driver(driverA),
 ptype(ptypeA)
{}

void XWTikzPattern::doPattern(XWTikzState * state)
{
	switch (ptype)
	{
		default:
		  break;

		case PGFhorizontallines:
		  doHorizontalLines(state);
		  break;

		case PGFverticallines:
		  doVerticalLines(state);
		  break;

		case PGFnortheastlines:
		  doNorthEastLines(state);
		  break;

		case PGFnorthwestlines:
		  doNorthWestLines(state);
		  break;

		case PGFgrid:
		  doGrid(state);
		  break;

		case PGFcrosshatch:
		  doCrosshatch(state);
		  break;

		case PGFdots:
		  doDots(state);
		  break;

		case PGFcrosshatchdots:
		  doCrosshatchDots(state);
		  break;

		case PGFfivepointedstars:
		  doFivePointedStars(state);
		  break;

		case PGFsixpointedstars:
		  doSixPointedStars(state);
		  break;

		case PGFbricks:
		  doBricks(state);
		  break;

		case PGFcheckerboard:
		  doCheckerBoard(state);
		  break;

		case PGFcheckerboardlightgray:
		  doCheckerBoardLightGray(state);
		  break;

		case PGFhorizontallineslightgray:
		  doHorizontalLinesLightGray(state);
		  break;

		case PGFhorizontallinesgray:
		  doHorizontalLinesGray(state);
		  break;

		case PGFhorizontallinesdarkgray:
		  doHorizontalLinesDarkGray(state);
		  break;

		case PGFhorizontallineslightblue:
		  doHorizontalLinesLightBlue(state);
		  break;

		case PGFhorizontallinesdarkblue:
		  doHorizontalLinesDarkBlue(state);
		  break;

		case PGFcrosshatchdotsgray:
		  doCrosshatchDotsGray(state);
		  break;

		case PGFcrosshatchdotslightsteelblue:
		  doCrosshatchDotsLightSteelBlue(state);
			break;
	}
}

void XWTikzPattern::doBricks(XWTikzState * state)
{
	QString name("bricks");
	XWObject ref;
	if (!driver->hasPattern(name,&ref))
	{
		state = state->save();
	  driver->beginPattern(name,0,-1,-1,14.226,14.226,11.381,11.381,&ref);
	  state->setLineWidth(0.8);
	  state->moveTo(0,2.845);
	  state->lineTo(11.381,2.845);
		state->moveTo(0,8.536);
	  state->lineTo(11.381,8.536);
		state->moveTo(2.845,0);
	  state->lineTo(2.845,2.845);
		state->moveTo(8.536,2.845);
	  state->lineTo(8.536,8.536);
		state->moveTo(2.845,8.536);
	  state->lineTo(2.845,11.381);
	  state->setDraw(true);
	  state = state->restore();
		driver->endPattern();
	}

	usePattern(name,state);
}

void XWTikzPattern::doCheckerBoard(XWTikzState * state)
{
	QString name("checkerboard");
	XWObject ref;
	if (!driver->hasPattern(name,&ref))
	{
		state = state->save();
	  driver->beginPattern(name,0,0,0,11.381,11.381,11.381,11.381,&ref);
	  QPointF ll(0,0);
		QPointF ur(5.691,5.691);
		state->addRectangle(ll,ur);
		state->addRectangle(ur,ur);
	  state->setFill(true);
	  state = state->restore();
		driver->endPattern();
	}
	
	usePattern(name,state);
}

void XWTikzPattern::doCheckerBoardLightGray(XWTikzState * state)
{
	QString name("checkerboard light gray");
	XWObject ref;
	if (!driver->hasPattern(name,&ref))
	{
		state = state->save();
		driver->beginPattern(name,1,0,0,11.381,11.381,11.381,11.381,&ref);
		QColor rgb = calulateColor(Qt::black,0.1);
		state->setFillColor(rgb);
		QPointF ll1(0,0);
		QPointF ur1(11.666,11.666);
		state->addRectangle(ll1,ur1);
		state = state->restore();
		state = state->save();
		rgb = calulateColor(Qt::black,0.2);
		state->setFillColor(rgb);
		QPointF ll2(0,0);
		QPointF ur2(5.691,5.691);
		state->addRectangle(ll2,ur2);
		state->addRectangle(ur2,ur2);
    state = state->restore();
		driver->endPattern();
	}
  
	driver->usePattern(name);
}

void XWTikzPattern::doCrosshatch(XWTikzState * state)
{
	QString name("crosshatch");
	XWObject ref;
	if (!driver->hasPattern(name,&ref))
	{
		state = state->save();
	  driver->beginPattern(name,0,-1,-1,4,4,3,3,&ref);
	  state->setLineWidth(0.4);
	  state->moveTo(3.1,0);
	  state->lineTo(0,3.1);
		state->moveTo(0,0);
	  state->lineTo(3.1,3.1);
	  state->setDraw(true);
	  state = state->restore();
		driver->endPattern();
	}
	
	usePattern(name,state);
}

void XWTikzPattern::doCrosshatchDots(XWTikzState * state)
{
	QString name("crosshatch dots");
	XWObject ref;
	if (!driver->hasPattern(name,&ref))
	{
		state = state->save();
	  driver->beginPattern(name,0,-1,-1,2.5,2.5,3,3,&ref);
		QPointF a(0.5,0);
    QPointF b(0,0.5);
		QPointF c(0,0);
    state->addEllipse(c,a,b);
		QPointF d(1.5,1.5);
		state->addEllipse(d,a,b);
	  state->setFill(true);
	  state = state->restore();
		driver->endPattern();
	}
	
	usePattern(name,state);
}

void XWTikzPattern::doCrosshatchDotsGray(XWTikzState * state)
{
	QString name("crosshatch dots gray");
	XWObject ref;
	if (!driver->hasPattern(name,&ref))
	{
		state = state->save();
	  driver->beginPattern(name,1,0,0,8,8,8,8,&ref);
		QColor rgb = calulateColor(Qt::black,0.2);
		state->setFillColor(rgb);
		QPointF ll(0,0);
		QPointF ur(8,8);
		state->addRectangle(ll,ur);
		state = state->restore();
		state = state->save();
		rgb = calulateColor(Qt::black,0.1);
		state->setFillColor(rgb);
		QPointF c(2,1.75);
		QPointF a(1,0);
		QPointF b(0,1);
		state->addEllipse(c,a,b);
		QPointF d(6,5.75);
		state->addEllipse(d,a,b);
	  state = state->restore();
		state = state->save();
		rgb = calulateColor(Qt::black,0.7);
		state->setFillColor(rgb);
		QPointF e(2,2.25);
		state->addEllipse(e,a,b);
		QPointF f(6,6.25);
		state->addEllipse(f,a,b);
		state = state->restore();
		driver->endPattern();
	}
	driver->usePattern(name);
}

void XWTikzPattern::doCrosshatchDotsLightSteelBlue(XWTikzState * state)
{
	QString name("crosshatch dots light steel blue");
	XWObject ref;
	if (!driver->hasPattern(name,&ref))
	{
		state = state->save();
	  driver->beginPattern(name,1,0,0,8,8,8,8,&ref);
		QColor rgb = tikzEnumToColor(PGFlightsteelblue);
		rgb = calulateColor(rgb,1.0);
		state->setFillColor(rgb);
		QPointF ll(0,0);
		QPointF ur(8,8);
		state->addRectangle(ll,ur);
		state = state->restore();
		state = state->save();
		rgb = tikzEnumToColor(PGFdarklightsteelblue);
		rgb = calulateColor(rgb,0.1);
		state->setFillColor(rgb);
		QPointF c(2,1.75);
		QPointF a(1,0);
		QPointF b(0,1);
		state->addEllipse(c,a,b);
		QPointF d(6,5.75);
		state->addEllipse(d,a,b);
	  state = state->restore();
		state = state->save();
		rgb = tikzEnumToColor(PGFdarklightsteelblue);
		rgb = calulateColor(rgb,0.7);
		state->setFillColor(rgb);
		QPointF e(2,2.25);
		state->addEllipse(e,a,b);
		QPointF f(6,6.25);
		state->addEllipse(f,a,b);
	  state = state->restore();
		driver->endPattern();
	}
	driver->usePattern(name);
}

void XWTikzPattern::doDots(XWTikzState * state)
{
	QString name("dots");
	XWObject ref;
	if (!driver->hasPattern(name,&ref))
	{
		state = state->save();
	  driver->beginPattern(name,0,-1,-1,1,1,3,3,&ref);
		QPointF a(0.5,0);
    QPointF b(0,0.5);
		QPointF c(0,0);
    state->addEllipse(c,a,b);
	  state->setFill(true);
	  state = state->restore();
		driver->endPattern();
	}
	usePattern(name,state);
}

void XWTikzPattern::doFivePointedStars(XWTikzState * state)
{
	QString name("fivepointed stars");
	XWObject ref;
	if (!driver->hasPattern(name,&ref))
	{
		state = state->save();
	  driver->beginPattern(name,0,0,0,8.536,8.536,8.536,8.536,&ref);
		state->shift(2.845,2.845);
		state->moveTo(2.706,0.879);
		state->lineTo(-2.706,0.879);
		state->lineTo(1.672,-2.302);
		state->lineTo(0,2.845);
		state->lineTo(-1.672,-2.302);
		state->closePath();
	  state->setFill(true);
	  state = state->restore();
		driver->endPattern();
	}
	usePattern(name,state);
}

void XWTikzPattern::doGrid(XWTikzState * state)
{
	QString name("grid");
	XWObject ref;
	if (!driver->hasPattern(name,&ref))
	{
		state = state->save();
	  driver->beginPattern(name,0,-1,-1,4,4,3,3,&ref);
	  state->setLineWidth(0.4);
	  state->moveTo(0,0);
	  state->lineTo(0,3.1);
		state->moveTo(0,0);
	  state->lineTo(3.1,0);
	  state->setDraw(true);
	  state = state->restore();
		driver->endPattern();
	}
	usePattern(name,state);
}

void XWTikzPattern::doHorizontalLines(XWTikzState * state)
{
	QString name("horizontal lines");
	XWObject ref;
	if (!driver->hasPattern(name,&ref))
	{
		state = state->save();
	  driver->beginPattern(name,0,0,0,100,1,100,3,&ref);
	  state->setLineWidth(0.4);
	  state->moveTo(0,0.5);
	  state->lineTo(100,0.5);
	  state->setDraw(true);
	  state = state->restore();
		driver->endPattern();
	}
	usePattern(name,state);
}

void XWTikzPattern::doHorizontalLinesDarkBlue(XWTikzState * state)
{
	QString name("horizontal lines dark blue");
	XWObject ref;
	if (!driver->hasPattern(name,&ref))
	{
		state = state->save();
	  driver->beginPattern(name,1,0,0,100,4,100,4,&ref);
		QColor rgb = calulateColor(Qt::blue,0.9);
		state->setFillColor(rgb);
		QPointF ll(0,0);
		QPointF ur(100,2.5);
		state->addRectangle(ll,ur);
		state = state->restore();
		state = state->save();
		rgb = calulateColor(Qt::blue,0.85);
		state->setFillColor(rgb);
		QPointF ll1(0,2);
		state->addRectangle(ll1,ur);
	  state = state->restore();
		driver->endPattern();
	}
	driver->usePattern(name);
}

void XWTikzPattern::doHorizontalLinesDarkGray(XWTikzState * state)
{
	QString name("horizontal lines dark gray");
	XWObject ref;
	if (!driver->hasPattern(name,&ref))
	{
		state = state->save();
	  driver->beginPattern(name,1,0,0,100,4,100,4,&ref);
		QColor rgb = calulateColor(Qt::black,0.9);
		state->setFillColor(rgb);
		QPointF ll(0,0);
		QPointF ur(100,2.5);
		state->addRectangle(ll,ur);
		state = state->restore();
		state = state->save();
		rgb = calulateColor(Qt::black,0.85);
		state->setFillColor(rgb);
		QPointF ll1(0,2);
		state->addRectangle(ll1,ur);
	  state = state->restore();
		driver->endPattern();
	}
	driver->usePattern(name);
}

void XWTikzPattern::doHorizontalLinesGray(XWTikzState * state)
{
	QString name("horizontal lines gray");
	XWObject ref;
	if (!driver->hasPattern(name,&ref))
	{
		state = state->save();
	  driver->beginPattern(name,1,0,0,100,4,100,4,&ref);
		QColor rgb = calulateColor(Qt::black,0.3);
		state->setFillColor(rgb);
		QPointF ll(0,0);
		QPointF ur(100,2.5);
		state->addRectangle(ll,ur);
		state = state->restore();
		state = state->save();
		rgb = calulateColor(Qt::black,0.35);
		state->setFillColor(rgb);
		QPointF ll1(0,2);
		state->addRectangle(ll1,ur);
	  state = state->restore();
		driver->endPattern();
	}
	driver->usePattern(name);
}

void XWTikzPattern::doHorizontalLinesLightBlue(XWTikzState * state)
{
	QString name("horizontal lines light blue");
	XWObject ref;
	if (!driver->hasPattern(name,&ref))
	{
		state = state->save();
	  driver->beginPattern(name,1,0,0,100,4,100,4,&ref);
		QColor rgb = calulateColor(Qt::blue,0.1);
		state->setFillColor(rgb);
		QPointF ll(0,0);
		QPointF ur(100,2.5);
		state->addRectangle(ll,ur);
		state = state->restore();
		state = state->save();
		rgb = calulateColor(Qt::blue,0.15);
		state->setFillColor(rgb);
		QPointF ll1(0,2);
		state->addRectangle(ll1,ur);
	  state = state->restore();
		driver->endPattern();
	}
	driver->usePattern(name);
}

void XWTikzPattern::doHorizontalLinesLightGray(XWTikzState * state)
{
	QString name("horizontal lines light gray");
	XWObject ref;
	if (!driver->hasPattern(name,&ref))
	{
		state = state->save();
	  driver->beginPattern(name,1,0,0,100,4,100,4,&ref);
		QColor rgb = calulateColor(Qt::black,0.1);
		state->setFillColor(rgb);
		QPointF ll(0,0);
		QPointF ur(100,2.5);
		state->addRectangle(ll,ur);
		state = state->restore();
		state = state->save();
		rgb = calulateColor(Qt::black,0.15);
		state->setFillColor(rgb);
		QPointF ll1(0,2);
		state->addRectangle(ll1,ur);
	  state = state->restore();
		driver->endPattern();
	}
	driver->usePattern(name);
}

void XWTikzPattern::doNorthEastLines(XWTikzState * state)
{
	QString name("north east lines");
	XWObject ref;
	if (!driver->hasPattern(name,&ref))
	{
		state = state->save();
	  driver->beginPattern(name,0,-1,-1,4,4,3,3,&ref);
	  state->setLineWidth(0.4);
	  state->moveTo(0,0);
	  state->lineTo(3.1,3.1);
	  state->setDraw(true);
	  state = state->restore();
		driver->endPattern();
	}
	
	usePattern(name,state);
}

void XWTikzPattern::doNorthWestLines(XWTikzState * state)
{
	QString name("north west lines");
	XWObject ref;
	if (!driver->hasPattern(name,&ref))
	{
		state = state->save();
	  driver->beginPattern(name,0,-1,-1,4,4,3,3,&ref);
	  state->setLineWidth(0.4);
	  state->moveTo(0,3);
	  state->lineTo(3.1,-0.1);
	  state->setDraw(true);
	  state = state->restore();
		driver->endPattern();
	}
	usePattern(name,state);
}

void XWTikzPattern::doSixPointedStars(XWTikzState * state)
{
	QString name("fivepointed stars");
	XWObject ref;
	if (!driver->hasPattern(name,&ref))
	{
		state = state->save();
	  driver->beginPattern(name,0,0,0,8.536,8.536,8.536,8.536,&ref);
		state->shift(2.845,2.845);
		state->moveTo(2.464,1.423);
		state->lineTo(-2.464,1.336);
		state->lineTo(0,-2.845);
		state->closePath();
		state->moveTo(-2.464,-1.423);
		state->lineTo(0,2.845);
		state->lineTo(-2.464,-1.423);
		state->closePath();
		state->setFillRule(PGFnonzerorule);
	  state->setFill(true);
	  state = state->restore();
		driver->endPattern();
	}
	usePattern(name,state);
}

void XWTikzPattern::doVerticalLines(XWTikzState * state)
{
	QString name("vertical lines");
	XWObject ref;
	if (!driver->hasPattern(name,&ref))
	{
		state = state->save();
	  driver->beginPattern(name,0,0,0,1,100,3,100,&ref);
	  state->setLineWidth(0.4);
	  state->moveTo(0.5,0);
	  state->lineTo(0.5,100);
	  state->setDraw(true);
	  state = state->restore();
		driver->endPattern();
	}
	usePattern(name,state);
}

void XWTikzPattern::usePattern(const QString & name,XWTikzState * state)
{
	QString csname("pgfprgb");
	if (!driver->hasColorSpace(csname))
	{
		XWObject cs,obj;
	  driver->initArray(&cs);
	  obj.initName("Pattern");
	  cs.arrayAdd(&obj);
	  obj.initName("DeviceRGB");
	  cs.arrayAdd(&obj);

	  driver->setColorSpace(csname,&cs);
	}

	QColor color = state->getPatternColor();
	driver->usePattern(csname,color,name);
}
