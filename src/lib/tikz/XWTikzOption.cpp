/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include <QRegExp>
#include <QAction>
#include "PGFKeyWord.h"
#include "XWPGFPool.h"
#include "XWTeXBox.h"
#include "tikzcolor.h"
#include "XWTikzGraphic.h"
#include "XWTikzCommand.h"
#include "XWTikzCoord.h"
#include "XWTikzTextBox.h"
#include "XWTikzNode.h"
#include "XWTikzState.h"
#include "XWTikzExpress.h"
#include "XWTikzUndoCommand.h"
#include "XWTikzOption.h"


XWTikzKey::XWTikzKey(XWTikzGraphic * graphicA, int idA, QObject * parent)
:XWTikzOperation(graphicA, idA,parent)
{}

void XWTikzKey::doCompute(XWTikzState * state)
{
  switch (keyWord)
  {
    default:
      break;

    case PGFbendatstart:
      state->setBendAtStart();
      break;

    case PGFbendatend:
      state->setBendAtEnd();
      break;

    case PGFshiftonly:
      state->shiftOnly();
      break;

    case PGFresetcm:
      state->resetTransform();
      break;

    case PGFtransformshape:
      state->setTransformShape();
      break;

    case PGFcoordinate:
    case PGFrectangle:
    case PGFcircle:
    case PGFellipse:
    case PGFdiamond:
    case PGFstar:
    case PGFregularpolygon:
    case PGFtrapezium:
    case PGFsemicircle:
    case PGFisoscelestriangle:
    case PGFkite:
    case PGFdart:
    case PGFcircularsector:
    case PGFcylinder:
    case PGFforbiddensign:
    case PGFcorrectforbiddensign:
    case PGFstarburst:
    case PGFcloud:
    case PGFsignal:
    case PGFtape:
    case PGFmagnifyingglass:
    case PGFmagnetictape:
    case PGFellipsecallout:
    case PGFrectanglecallout:
    case PGFcloudcallout:
    case PGFsinglearrow:
    case PGFdoublearrow:
    case PGFarrowbox:
    case PGFcrossout:
    case PGFstrikeout:
    case PGFroundedrectangle:
    case PGFchamferedrectangle:
    case PGFcirclesplit:
    case PGFcirclesolidus:
    case PGFrectanglesplit:
    case PGFellipsesplit:
      state->setShape(keyWord);
      break;

    case PGFmindmap:
    case PGFsmallmindmap:
    case PGFlargemindmap:
    case PGFhugemindmap:
      state->setMindmap(keyWord);
      break;

    case PGFcircuit:
    case PGFcircuits:
    case PGFcircuitee:
      state->setPictureType(PGFcircuit);
      break;

    case PGFcircuiteeIEC:
    case PGFcircuitlogic:
    case PGFcircuitlogicIEC:
    case PGFcircuitlogicUS:
    case PGFcircuitlogicCDH:
      state->setPictureType(keyWord);
      break;
  }
}

void XWTikzKey::doPath(XWTikzState * state, bool)
{
  switch (keyWord)
  {
    default:
      break;

    case PGFclip:
      state->setClip(true);
      break;

    case PGFsharpcorners:
      state->setRoundedCorners(0);
      break;

    case PGFbendatstart:
      state->setBendAtStart();
      break;

    case PGFbendatend:
      state->setBendAtEnd();
      break;

    case PGFultrathin:
      state->setLineWidth(0.1);
      break;

    case PGFverythin:
      state->setLineWidth(0.2);
      break;

    case PGFthin:
      state->setLineWidth(0.4);
      break;

    case PGFsemithick:
      state->setLineWidth(0.6);
      break;

    case PGFthick:
      state->setLineWidth(0.8);
      break;

    case PGFverythick:
      state->setLineWidth(1.2);
      break;

    case PGFultrathick:
      state->setLineWidth(1.6);
      break;

    case PGFsolid:
    case PGFdotted:
    case PGFdenselydotted:
    case PGFlooselydotted:
    case PGFdashed:
    case PGFdenselydashed:
    case PGFlooselydashed:
    case PGFdashdotted:
    case PGFdenselydashdotted:
    case PGFlooselydashdotted:
    case PGFdashdotdotted:
    case PGFdenselydashdotdotted:
    case PGFlooselydashdotdotted:
      state->setDash(keyWord);
      break;

    case PGFnonzerorule:
      state->setFillRule(PGFnonzerorule);
      break;

    case PGFevenoddrule:
      state->setFillRule(PGFevenoddrule);
      break;

    case PGFmatrix:
      state->setMatrix();
      break;
      
    case PGFshiftonly:
      state->shiftOnly();
      break;

    case PGFresetcm:
      state->resetTransform();
      break;

    case PGFtransformshape:
      state->setTransformShape();
      break;

    case PGFswap:
      state->setSwap();
      break;

    case PGFsloped:
      state->setSwap();
      break;

    case PGFallowupsidedown:
      state->setAllowUpsideDown();
      break;

    case PGFmidway:
      state->setPos(0.5);
      break;

    case PGFnearstart:
      state->setPos(0.25);
      break;

    case PGFnearend:
      state->setPos(0.75);
      break;

    case PGFverynearstart:
      state->setPos(0.125);
      break;

    case PGFverynearend:
      state->setPos(0.875);
      break;

    case PGFatstart:
      state->setPos(0);
      break;

    case PGFatend:
      state->setPos(1);
      break;

    case PGFbehindpath:
    case PGFinfrontofpath:
      state->setLocation(keyWord);
      break;

    case PGFsharpplot:
    case PGFsmooth:
    case PGFsmoothcycle:
    case PGFconstplot:
    case PGFconstplotmarkleft:
    case PGFconstplotmarkright:
    case PGFjumpmarkleft:
    case PGFjumpmarkright:
    case PGFycomb:
    case PGFxcomb:
    case PGFpolarcomb:
    case PGFybar:
    case PGFxbar:
    case PGFybarinterval:
    case PGFxbarinterval:
    case PGFonlymarks:
      state->setPlotHandler(keyWord);
      break;

    case PGFtransparent:
      state->setOpacity(0);
      break;

    case PGFultranearlytransparent:
      state->setOpacity(0.05);
      break;

    case PGFverynearlytransparent:
      state->setOpacity(0.1);
      break;

    case PGFnearlytransparent:
      state->setOpacity(0.25);
      break;

    case PGFsemitransparent:
      state->setOpacity(0.5);
      break;

    case PGFnearlyopaque:
      state->setOpacity(0.75);
      break;

    case PGFverynearlyopaque:
      state->setOpacity(0.9);
      break;

    case PGFultranearlyopaque:
      state->setOpacity(0.95);
      break;

    case PGFopaque:
      state->setOpacity(1);
      break;

    case PGFedgefromparentforkdown:
      state->doEdgeFromParentForkDown();
      break;

    case PGFedgefromparentforkup:
      state->doEdgeFromParentForkUp();
      break;

    case PGFedgefromparentforkleft:
      state->doEdgeFromParentForkLeft();
      break;

    case PGFedgefromparentforkright:
      state->doEdgeFromParentForkRight();
      break;

    case PGFgrowcyclic:
      state->growCyclic();
      break;

    case PGFcoordinate:
    case PGFrectangle:
    case PGFcircle:
    case PGFellipse:
    case PGFdiamond:
    case PGFstar:
    case PGFregularpolygon:
    case PGFtrapezium:
    case PGFsemicircle:
    case PGFisoscelestriangle:
    case PGFkite:
    case PGFdart:
    case PGFcircularsector:
    case PGFcylinder:
    case PGFforbiddensign:
    case PGFcorrectforbiddensign:
    case PGFstarburst:
    case PGFcloud:
    case PGFsignal:
    case PGFtape:
    case PGFmagnifyingglass:
    case PGFmagnetictape:
    case PGFellipsecallout:
    case PGFrectanglecallout:
    case PGFcloudcallout:
    case PGFsinglearrow:
    case PGFdoublearrow:
    case PGFarrowbox:
    case PGFcrossout:
    case PGFstrikeout:
    case PGFroundedrectangle:
    case PGFchamferedrectangle:
    case PGFcirclesplit:
    case PGFcirclesolidus:
    case PGFrectanglesplit:
    case PGFellipsesplit:
      state->setShape(keyWord);
      break;

    case PGFconcept:
      state->setFill(true);
      graphic->doEveryConcept(state);
      if (state->getLevel() == 0)
        graphic->doRootConcept(state);
      else
        graphic->doLevelConcept(state);
      break;

    case PGFextraconcept:
      {
        QColor c = calulateColor(Qt::black,0.5);
        state->setDrawColor(c);
        state->setLevel(2);
        graphic->doLevelConcept(state);
      }
      break;

    case PGFconceptconnection:
      state->setLineWidth(2.8453);
      state->setShortenStart(5.6906);
      state->setShortenEnd(5.6906);
      state->setLineCap(PGFcap);
      {
        QColor c = calulateColor(Qt::black,0.5);
        state->setDrawColor(c);
      }
      break;

    case PGFmindmap:
    case PGFsmallmindmap:
    case PGFlargemindmap:
    case PGFhugemindmap:
      state->setMindmap(keyWord);
      break;

    case PGFannotation:
      state->setShape(PGFrectangle);
      state->seiMinimumHeight(0);
      state->seiMinimumHeight(0);
      state->setTextWidth(99.5846);
      state->setOuterXSep(4.2679);
      state->setOuterYSep(4.2679);
      state->setInnerXSep(2.8453);
      state->setInnerYSep(2.8453);
      state->setRoundedCorners(4);
      state->setFontSize(5);
      break;

    case PGFconnectspies:
      {
        XWTikzPathText p(graphic,PGFspyconnectionpath);
        p.doPath(state,false);
      }
      break;

    case PGFlineto:
    case PGFmoveto:
    case PGFcurveto:
    case PGFcircleconnectionbar:
    case PGFmarkings:
    case PGFzigzag:
    case PGFsaw:
    case PGFrandomsteps:
    case PGFstraightzigzag:
    case PGFbent:
    case PGFsnake:
    case PGFcoil:
    case PGFbumps:
      state->setDecoration(keyWord);
      break;

    case PGFcircuit:
    case PGFcircuits:
    case PGFcircuitee:
      state->setPictureType(PGFcircuit);
      break;

    case PGFhugecircuitsymbols:
      state->setCircuitSizeUnit(10);
      break;

    case PGFlargecircuitsymbols:
      state->setCircuitSizeUnit(8);
      break;

    case PGFmediumcircuitsymbols:
      state->setCircuitSizeUnit(7);
      break;

    case PGFsmallcircuitsymbols:
      state->setCircuitSizeUnit(6);
      break;

    case PGFtinycircuitsymbols:
      state->setCircuitSizeUnit(5);
      break;

    case PGFpointup:
      state->rotate(90);
      break;

    case PGFpointdown:
      state->rotate(-90);
      break;

    case PGFpointleft:
      state->rotate(-180);
      break;

    case PGFpointright:
      state->rotate(180);
      break;

    case PGFcircuitsymbolopen:
      state->setDraw(true);
      break;

    case PGFcircuitsymbolfilled:
      state->setDraw(true);
      state->setFillColor(Qt::black);
      break;

    case PGFcircuitsymbollines:
      state->setDraw(true);
      break;

    case PGFcircuitsymbolwires:
      state->setDraw(true);
      break;

    case PGFcircuiteeIEC:
    case PGFcircuitlogic:
    case PGFcircuitlogicIEC:
    case PGFcircuitlogicUS:
    case PGFcircuitlogicCDH:
      state->setPictureType(keyWord);
      break;

    case PGFcanvasisplane:
      state->setPlane();
      break;

    case PGFstatewithoutoutput:
    case PGFstate:
      state->setShape(PGFcircle);
      state->setDraw(true);
      state->seiMinimumWidth(25);
      state->seiMinimumHeight(25);
      graphic->doEveryState(state);
      break;

    case PGFstatewithoutput:
      state->setShape(PGFcirclesplit);
      state->setDraw(true);
      state->seiMinimumWidth(25);
      state->seiMinimumHeight(25);
      graphic->doEveryState(state);
      break;

    case PGFacceptingbydouble:
      state->acceptingByDouble();
      break;

    case PGFinitialbydiamond:
      state->setShape(PGFdiamond);
      break;

    case PGFinitialabove:
      state->initialByArrow();
      state->setInitialAngle(90);
      state->setInitialAnchor(PGFsouth);
      break;

    case PGFinitialbelow:
      state->initialByArrow();
      state->setInitialAngle(270);
      state->setInitialAnchor(PGFnorth);
      break;

    case PGFinitialleft:
      state->initialByArrow();
      state->setInitialAngle(180);
      state->setInitialAnchor(PGFeast);
      break;

    case PGFinitialright:
      state->initialByArrow();
      state->setInitialAngle(0);
      state->setInitialAnchor(PGFwest);
      break;

    case PGFacceptingabove:
      state->acceptingByArrow();
      state->setAcceptingAngle(90);
      state->setAcceptingAnchor(PGFsouth);
      break;

    case PGFacceptingbelow:
      state->acceptingByArrow();
      state->setAcceptingAngle(270);
      state->setAcceptingAnchor(PGFnorth);
      break;

    case PGFacceptingleft:
      state->acceptingByArrow();
      state->setAcceptingAngle(180);
      state->setAcceptingAnchor(PGFeast);
      break;

    case PGFacceptingright:
      state->acceptingByArrow();
      state->setAcceptingAngle(0);
      state->setAcceptingAnchor(PGFwest);
      break;

    case PGFaccepting:
      state->acceptingByDouble();
      break;

    case PGFinitial:
      state->initialByArrow();
      break;

    case PGFentity:
      state->setShape(PGFrectangle);
      state->setDraw(true);
      state->seiMinimumHeight(20);
      state->seiMinimumWidth(40);
      graphic->doEveryEntity(state);
      break;

    case PGFrelationship:
      state->setShape(PGFdiamond);
      state->setDraw(true);
      state->seiMinimumHeight(15);
      state->seiMinimumWidth(15);
      state->setInnerXSep(1);
      state->setInnerYSep(1);
      graphic->doEveryRelationship(state);
      break;

    case PGFattribute:
      state->setShape(PGFellipse);
      state->setDraw(true);
      state->seiMinimumHeight(15);
      state->seiMinimumWidth(15);
      state->setInnerXSep(1);
      state->setInnerYSep(1);
      graphic->doEveryAttribute(state);
      break;

    case PGFtomoveto:
      state->setMoveTo();
      break;

    case PGFtolineto:
      state->setLineTo();
      break;

    case PGFtocurveto:
      state->setCurveTo();
      break;

    case PGFloop:
      state->setInLooseness(8);
      state->setOutLooseness(8);
      state->setInMin(14.22638);
      state->setOutMin(14.22638);
      state->setLoop();
      graphic->doEveryLoop(state);
      break;

    case PGFloopright:
      state->setAnchor(PGFright);
      state->setIn(-15);
      state->setOut(15);
      state->setInLooseness(8);
      state->setOutLooseness(8);
      state->setInMin(14.22638);
      state->setOutMin(14.22638);
      state->setLoop();
      graphic->doEveryLoop(state);
      break;

    case PGFloopabove:
      state->setAnchor(PGFabove);
      state->setIn(75);
      state->setOut(105);
      state->setInLooseness(8);
      state->setOutLooseness(8);
      state->setInMin(14.22638);
      state->setOutMin(14.22638);
      state->setLoop();
      graphic->doEveryLoop(state);
      break;

    case PGFloopleft:
      state->setAnchor(PGFleft);
      state->setIn(165);
      state->setOut(195);
      state->setInLooseness(8);
      state->setOutLooseness(8);
      state->setInMin(14.22638);
      state->setOutMin(14.22638);
      state->setLoop();
      graphic->doEveryLoop(state);
      break;

    case PGFloopbelow:
      state->setAnchor(PGFbelow);
      state->setIn(285);
      state->setOut(255);
      state->setInLooseness(8);
      state->setOutLooseness(8);
      state->setInMin(14.22638);
      state->setOutMin(14.22638);
      state->setLoop();
      graphic->doEveryLoop(state);
      break;

    case PGFplace:
      state->setShape(PGFcircle);
      state->setDraw(true);
      state->setInnerXSep(0);
      state->setInnerYSep(0);
      state->seiMinimumWidth(21.5);
      state->seiMinimumHeight(21.5);
      graphic->doEveryPlace(state);
      break;

    case PGFeverytransition:
      state->setShape(PGFrectangle);
      state->setDraw(true);
      state->setInnerXSep(0);
      state->setInnerYSep(0);
      state->seiMinimumWidth(28.908);
      state->seiMinimumHeight(28.908);
      graphic->doEveryTransition(state);
      break;

    case PGFpre:
      {
        XWTikzArrowSpecification * arrow = new XWTikzArrowSpecification(graphic,state);
        arrow->setArrow(PGFarrowdefault);
        state->setStartArrow(arrow);
        state->setShortenStart(1);
      }
      break;

    case PGFpost:
      {
        XWTikzArrowSpecification * arrow = new XWTikzArrowSpecification(graphic,state);
        arrow->setArrow(PGFarrowdefault);
        state->setEndArrow(arrow);
        state->setShortenEnd(1);
      }
      break;

    case PGFpreandpost:
      {
        XWTikzArrowSpecification * arrow = new XWTikzArrowSpecification(graphic,state);
        arrow->setArrow(PGFarrowdefault);
        state->setStartArrow(arrow);
        state->setShortenStart(1);
        arrow = new XWTikzArrowSpecification(graphic,state);
        arrow->setArrow(PGFarrowdefault);
        state->setEndArrow(arrow);
        state->setShortenEnd(1);
      }
      break;

    case PGFtoken:
      state->setToken(1);
      state->setFillColor(Qt::black);      
      break;
  }
}

void XWTikzKey::dragTo(XWTikzState * state)
{
  doCompute(state);
}

bool XWTikzKey::dropTo(XWTikzState * state)
{
  doCompute(state);
  return false;
}

QString XWTikzKey::getText()
{
  QString ret = getPGFString(keyWord);
  return ret;
}

QString XWTikzKey::getTips(XWTikzState * state)
{
  hitTest(state);
  return getText();
}

XWTikzValue::XWTikzValue(XWTikzGraphic * graphicA, int idA, QObject * parent)
:XWTikzOperation(graphicA, idA,parent)
{
  v.expv = 0;
  v.coordv = 0;
}

void XWTikzValue::doCompute(XWTikzState * state)
{
  switch (keyWord)
  {
    default:
      break;

    case PGFradius:
      if (v.expv->isNum())
        state->setRadius(state->getXVec() * v.expv->getResult(state));
      else
        state->setRadius(v.expv->getResult(state));
      break;

    case PGFxradius:
      if (v.expv->isNum())
        state->setXRadius(state->getXVec() * v.expv->getResult(state));
      else
        state->setXRadius(v.expv->getResult(state));
      break;

    case PGFyradius:
      if (v.expv->isNum())
        state->setYRadius(state->getYVec() * v.expv->getResult(state));
      else
        state->setYRadius(v.expv->getResult(state));
      break;

    case PGFat:
      state->setAt(v.coordv);
      break;

    case PGFstartangle:
      state->setStartAngle(v.expv->getResult(state));
      break;

    case PGFendangle:
      state->setEndAngle(v.expv->getResult(state));
      break;

    case PGFdeltaangle:
      state->setDeltaAngle(v.expv->getResult(state));
      break;

    case PGFbend:
      state->setBend(v.coordv);
      break;

    case PGFbendpos:
      state->setBendPos(v.expv->getResult(state));
      break;

    case PGFparabolaheight:
      state->setBendHeight(v.expv->getResult(state));
      break;

    case PGFnameprefix:
      state->setNamePrefix(text);
      break;

    case PGFnamesuffix:
      state->setNameSuffix(text);
      break;

    case PGFshape:
      state->setShape((int)(v.expv->getResult(state)));
      break;

    case PGFinnersep:      
      state->setInnerXSep(v.expv->getResult(state));
      state->setInnerYSep(v.expv->getResult(state));
      break;

    case PGFinnerxsep:      
      state->setInnerXSep(v.expv->getResult(state));
      break;

    case PGFinnerysep:
      state->setInnerYSep(v.expv->getResult(state));
      break;

    case PGFoutersep:      
      state->setOuterXSep(v.expv->getResult(state));
      state->setOuterYSep(v.expv->getResult(state));
      break;

    case PGFouterxsep:      
      state->setOuterXSep(v.expv->getResult(state));
      break;

    case PGFouterysep:
      state->setOuterYSep(v.expv->getResult(state));
      break;

    case PGFminimumheight:
      state->seiMinimumHeight(v.expv->getResult(state));
      break;

    case PGFminimumwidth:
      state->seiMinimumWidth(v.expv->getResult(state));
      break;

    case PGFminimumsize:
      state->seiMinimumHeight(v.expv->getResult(state));
      state->seiMinimumWidth(v.expv->getResult(state));
      break;

    case PGFshapeaspect:
      state->setShapeAspect(v.expv->getResult(state));
      break;

    case PGFshapeborderusesincircle:
      state->setShapeBorderUsesIncircle((bool)(v.expv->getResult(state)));
      break;

    case PGFshapeborderrotate:
      state->setShapeBorderRotate(v.expv->getResult(state));
      break;

    case PGFsamples:
      state->setSamples(v.expv->getResult(state));
      break;

    case PGFx:
      state->setXVec(v.expv->getResult(state));
      break;

    case PGFy:
      state->setYVec(v.expv->getResult(state));
      break;

    case PGFz:
      state->setZVec(v.expv->getResult(state));
      break;

    case PGFshift:
      {
        QPointF d = v.coordv->getPoint(state);
        state->shift(d.x(),d.y());
      }
      break;

    case PGFxshift:
      state->shift(v.expv->getResult(state),0);
      break;

    case PGFyshift:
      state->shift(0,v.expv->getResult(state));
      break;

    case PGFscale:
      state->scale(v.expv->getResult(state),v.expv->getResult(state));
      break;

    case PGFxscale:
      state->scale(v.expv->getResult(state),1);
      break;

    case PGFyscale:
      state->scale(1,v.expv->getResult(state));
      break;

    case PGFxslant:
      state->slant(v.expv->getResult(state),0);
      break;

    case PGFyslant:
      state->slant(0,v.expv->getResult(state));
      break;

    case PGFrotate:
      state->rotate(v.expv->getResult(state));
      break;

    case PGFnamepath:
    case PGFname:
      {
        QString n = v.expv->getText();
        graphic->registNamed(n);
      }
      break;
  }
}

void XWTikzValue::doPath(XWTikzState * state, bool)
{
  switch (keyWord)
  {
    default:
      break;

    case PGFarrowdefault:
      state->setArrowDefault((int)v.expv->getResult(state));
      break;

    case PGFroundedcorners:
      {
        double r = v.expv->getResult(state);
        if (r == 0)
          r = 4;
        state->setRoundedCorners(r);
      }      
      break;

    case PGFradius:
      if (v.expv->isNum())
        state->setRadius(state->getXVec() * v.expv->getResult(state));
      else
        state->setRadius(v.expv->getResult(state));
      break;

    case PGFxradius:
      if (v.expv->isNum())
        state->setXRadius(state->getXVec() * v.expv->getResult(state));
      else
        state->setXRadius(v.expv->getResult(state));
      break;

    case PGFyradius:
      if (v.expv->isNum())
        state->setYRadius(state->getYVec() * v.expv->getResult(state));
      else
        state->setYRadius(v.expv->getResult(state));
      break;

    case PGFat:
      state->setAt(v.coordv);
      break;

    case PGFstartangle:
      state->setStartAngle(v.expv->getResult(state));
      break;

    case PGFendangle:
      state->setEndAngle(v.expv->getResult(state));
      break;

    case PGFdeltaangle:
      state->setDeltaAngle(v.expv->getResult(state));
      break;

    case PGFstep:
      {
        QPointF d = v.coordv->getPoint(state);
        state->setStep(d.x(),d.y());
      }
      break;

    case PGFxstep:
      if (v.expv->isNum())
        state->setXStep(state->getXVec() * v.expv->getResult(state));
      else
        state->setXStep(v.expv->getResult(state));
      break;

    case PGFystep:
      if (v.expv->isNum())
        state->setYStep(state->getYVec() * v.expv->getResult(state));
      else
        state->setYStep(v.expv->getResult(state));
      break;

    case PGFbend:
      state->setBend(v.coordv);
      break;

    case PGFbendpos:
      state->setBendPos(v.expv->getResult(state));
      break;

    case PGFparabolaheight:
      state->setBendHeight(v.expv->getResult(state));
      break;

    case PGFlinewidth:
      state->setLineWidth(v.expv->getResult(state));
      break;

    case PGFlinecap:
      state->setLineCap((int)(v.expv->getResult(state)));
      break;

    case PGFlinejoin:
      state->setLineJoin((int)(v.expv->getResult(state)));
      break;

    case PGFmiterlimit:
      state->setMiterLimit(v.expv->getResult(state));
      break;

    case PGFdashphase:
      state->setDashPhase(v.expv->getResult(state));
      break;

    case PGFdrawopacity:
      state->setDrawOpacity(v.expv->getResult(state));
      break;

    case PGFfillopacity:
      state->setFillOpacity(v.expv->getResult(state));
      break;

    case PGFopacity:
      state->setDrawOpacity(v.expv->getResult(state));
      state->setFillOpacity(v.expv->getResult(state));
      break;

    case PGFnodecontents:
      state->setNodeContents(text);
      break;

    case PGFnameprefix:
      state->setNamePrefix(text);
      break;

    case PGFnamesuffix:
      state->setNameSuffix(text);
      break;

    case PGFshape:
      state->setShape((int)(v.expv->getResult(state)));
      break;

    case PGFinnersep:      
      state->setInnerXSep(v.expv->getResult(state));
      state->setInnerYSep(v.expv->getResult(state));
      break;

    case PGFinnerxsep:      
      state->setInnerXSep(v.expv->getResult(state));
      break;

    case PGFinnerysep:
      state->setInnerYSep(v.expv->getResult(state));
      break;

    case PGFoutersep:      
      state->setOuterXSep(v.expv->getResult(state));
      state->setOuterYSep(v.expv->getResult(state));
      break;

    case PGFouterxsep:      
      state->setOuterXSep(v.expv->getResult(state));
      break;

    case PGFouterysep:
      state->setOuterYSep(v.expv->getResult(state));
      break;

    case PGFminimumheight:
      state->seiMinimumHeight(v.expv->getResult(state));
      break;

    case PGFminimumwidth:
      state->seiMinimumWidth(v.expv->getResult(state));
      break;

    case PGFminimumsize:
      state->seiMinimumHeight(v.expv->getResult(state));
      state->seiMinimumWidth(v.expv->getResult(state));
      break;

    case PGFshapeaspect:
      state->setShapeAspect(v.expv->getResult(state));
      break;

    case PGFshapeborderusesincircle:
      state->setShapeBorderUsesIncircle((bool)(v.expv->getResult(state)));
      break;

    case PGFshapeborderrotate:
      state->setShapeBorderRotate(v.expv->getResult(state));
      break;

    case PGFsamples:
      state->setSamples(v.expv->getResult(state));
      break;

    case PGFx:
      state->setXVec(v.expv->getResult(state));
      break;

    case PGFy:
      state->setYVec(v.expv->getResult(state));
      break;

    case PGFz:
      state->setZVec(v.expv->getResult(state));
      break;

    case PGFshift:
      {
        QPointF d = v.coordv->getPoint(state);
        state->shift(d.x(),d.y());
      }
      break;

    case PGFxshift:
      state->shift(v.expv->getResult(state),0);
      break;

    case PGFyshift:
      state->shift(0,v.expv->getResult(state));
      break;

    case PGFscale:
      state->scale(v.expv->getResult(state),v.expv->getResult(state));
      break;

    case PGFxscale:
      state->scale(v.expv->getResult(state),1);
      break;

    case PGFyscale:
      state->scale(1,v.expv->getResult(state));
      break;

    case PGFxslant:
      state->slant(v.expv->getResult(state),0);
      break;

    case PGFyslant:
      state->slant(0,v.expv->getResult(state));
      break;

    case PGFrotate:
      state->rotate(v.expv->getResult(state));
      break;

    case PGFshading:
      state->setShadingName((int)(v.expv->getResult(state)));
      break;

    case PGFshadingangle:
      state->setShadingAngle(v.expv->getResult(state));
      break;

    case PGFpathfading:
      state->setPathFading((int)(v.expv->getResult(state)));
      break;

    case PGFfitfading:
      state->setFitFading((bool)(v.expv->getResult(state)));
      break;

    case PGFpattern:
      state->setPatternName((int)(v.expv->getResult(state)));
      break;

    case PGFfadingangle:
      state->setFadingRoate(v.expv->getResult(state));
      break;

    case PGFscopefading:
      state->setPathFading((int)(v.expv->getResult(state)));
      break;

    case PGFdoubledistance:
      {
        double w = state->getLineWidth();
        double iw = v.expv->getResult(state);
        state->setLineWidth(iw + 2 * w);
        state->setInnerLineWidth(iw);
      }
      break;

    case PGDdoubledistancebetweenlinecenters:
      {
        double w = state->getLineWidth();
        double iw = v.expv->getResult(state);
        state->setLineWidth(iw + w);
        state->setInnerLineWidth(iw - w);
      }
      break;

    case PGFnamepath:
    case PGFname:
      {
        QString n = v.expv->getText();
        graphic->registNamed(n);
      }
      break;

    case PGFsortby:
      {
        QString n = v.expv->getText();
        graphic->setSortBy(n);
      }
      break;

    case PGFpos:
      state->setPos(v.expv->getResult(state));
      break;

    case PGFauto:
      state->setAuto((int)(v.expv->getResult(state)));
      break;

    case PGFabsolute:
      state->setAbsolute((bool)(v.expv->getResult(state)));
      break;

    case PGFlabelposition:
    case PGFpinposition:
      state->setLabelAngle(v.expv->getResult(state));
      break;

    case PGFlabeldistance:
    case PGFpindistance:
      state->setLabelDistance(v.expv->getResult(state));
      break;

    case PGFmatrixanchor:
      state->setMatrixAnchor((int)(v.expv->getResult(state)));
      break;

    case PGFleveldistance:
      state->setLevelDistance(v.expv->getResult(state));
      break;

    case PGFsiblingdistance:
      state->setSiblingDistance(v.expv->getResult(state));
      break;

    case PGFgrow:
      state->setGrow((int)(v.expv->getResult(state)));
      break;

    case PGFgrowopposite: 
      state->setGrowOpposite((int)(v.expv->getResult(state)));
      break;

    case PGFmissing:
      state->setMissing((bool)(v.expv->getResult(state)));
      break;

    case PGFgrowthparentanchor:
      state->setGrowthParentAnchor((int)(v.expv->getResult(state)));
      break;

    case PGFchildanchor:
      state->setParentAnchor((int)(v.expv->getResult(state)));
      break;

    case PGFparentanchor:
      state->setChildAnchor((int)(v.expv->getResult(state)));
      break;

    case PGFmark:
      state->setMark((int)(v.expv->getResult(state)));
      break;

    case PGFmarkrepeat:
      state->setMarkRepeat((int)(v.expv->getResult(state)));
      break;

    case PGFmarkphase:
      state->setMarkPhase((int)(v.expv->getResult(state)));
      break;

    case PGFmarksize:
      state->setMarkSize(v.expv->getResult(state));
      break;

    case PGFraise:
      state->setRaise(v.expv->getResult(state));
      break;

    case PGFdecorate:
      state->setDecorate((bool)(v.expv->getResult(state)));
      break;

    case PGFdecoration:
      state->setDecoration((int)(v.expv->getResult(state)));
      break;

    case PGFmirror:
      state->setMirror((bool)(v.expv->getResult(state)));
      break;

    case PGFpre:
      state->setPre((int)(v.expv->getResult(state)));
      break;

    case PGFprelength:
      state->setPrelength(v.expv->getResult(state));
      break;

    case PGFpost:
      state->setPost((int)(v.expv->getResult(state)));
      break;

    case PGFpostlength:
      state->setPostlength(v.expv->getResult(state));
      break;

    case PGFaspect:
      state->setAspect(v.expv->getResult(state));
      break;

    case PGFtrapeziumleftangle:
      state->setTrapeziumLeftAngle(v.expv->getResult(state));
      break;

    case PGFtrapeziumrightangle:
      state->setTrapeziumRightAngle(v.expv->getResult(state));
      break;

    case PGFtrapeziumangle:
      state->setTrapeziumLeftAngle(v.expv->getResult(state));
      state->setTrapeziumRightAngle(v.expv->getResult(state));
      break;

    case PGFtrapeziumstretches:
      state->setTrapeziumStretches((bool)(v.expv->getResult(state)));
      break;

    case PGFtrapeziumstretchesbody:
      state->setTrapeziumStretchesBody((bool)(v.expv->getResult(state)));
      break;

    case PGFregularpolygonsides:
      state->setRegularPolygonSides(v.expv->getResult(state));
      break;

    case PGFstarpoints:
      state->setStarPoints((int)v.expv->getResult(state));
      break;

    case PGFstarpointheight:
      state->setStarPointHeight(v.expv->getResult(state));
      break;

    case PGFstarpointratio:
      state->setStarPointRatio(v.expv->getResult(state));
      break;

    case PGFisoscelestriangleapexangle:
      state->setIsoscelesTriangleApexAngle(v.expv->getResult(state));
      break;

    case PGFisoscelestrianglestretches:
      state->setIsoscelesTriangleStretches(v.expv->getResult(state));
      break;

    case PGFkiteuppervertexangle:
      state->setKiteUpperVertexAngle(v.expv->getResult(state));
      break;

    case PGFkitelowervertexangle:
      state->setKiteLowerVertexAngle(v.expv->getResult(state));
      break;

    case PGFdarttipangle:
      state->setDartTipAngle(v.expv->getResult(state));
      break;

    case PGFdarttailangle:
      state->setDartTailAngle(v.expv->getResult(state));
      break;

    case PGFcircularsectorangle:
      state->setCircularSectorAngle(v.expv->getResult(state));
      break;

    case PGFcylinderusescustomfill:
      state->setCylinderUsesCustomFill((bool)(v.expv->getResult(state)));
      break;

    case PGFcloudpuffs:
      state->setCloudPuffs(v.expv->getResult(state));
      break;

    case PGFcloudpuffarc:
      state->setCloudPuffArc(v.expv->getResult(state));
      break;

    case PGFcloudignoresaspect:
      state->setCloudIgnoresAspect((bool)(v.expv->getResult(state)));
      break;

    case PGFMGHAfill:
      state->setMGHAfill(v.expv->getResult(state));
      break;

    case PGFMGHAaspect:
      state->setMGHAaspect((bool)(v.expv->getResult(state)));
      break;

    case PGFstarburstpoints:
      state->setStarburstPoints((int)v.expv->getResult(state));
      break;

    case PGFstarburstpointheight:
      state->setStarburstPointHeight(v.expv->getResult(state));
      break;

    case PGFrandomstarburst:
      state->setRandomStarburst((int)v.expv->getResult(state));
      break;

    case PGFsignalpointerangle:
      state->setSignalPointerAngle(v.expv->getResult(state));
      break;

    case PGFtapebendtop:
      state->setTapeBendTop((int)(v.expv->getResult(state)));
      break;

    case PGFtapebendbottom:
      state->setTapeBendBottom((int)(v.expv->getResult(state)));
      break;

    case PGFtapebendheight:
      state->setTapeBendHeight(v.expv->getResult(state));
      break;

    case PGFmagnetictapetailextend:
      state->setMagneticTapeTailExtend(v.expv->getResult(state));
      break;

    case PGFmagnetictapetail:
      state->setMagneticTapeTail(v.expv->getResult(state));
      break;

    case PGFsinglearrowtipangle:
      state->setSingleArrowTipAngle(v.expv->getResult(state));
      break;

    case PGFsinglearrowheadextend:
      state->setSingleArrowHeadExtend(v.expv->getResult(state));
      break;

    case PGFsinglearrowheadindent:
      state->setSingleArrowHeadIndent(v.expv->getResult(state));
      break;

    case PGFdoublearrowtipangle:
      state->setDoubleArrowTipAngle(v.expv->getResult(state));
      break;

    case PGFdoublearrowheadextend:
      state->setDoubleArrowHeadExtend(v.expv->getResult(state));
      break;

    case PGFdoublearrowheadindent:
      state->setDoubleArrowHeadIndent(v.expv->getResult(state));
      break;

    case PGFarrowboxtipangle:
      state->setArrowBoxTipAngle(v.expv->getResult(state));
      break;

    case PGFarrowboxheadextend:
      state->setArrowBoxHeadExtend(v.expv->getResult(state));
      break;

    case PGFarrowboxheadindent:
      state->setArrowBoxHeadIndent(v.expv->getResult(state));
      break;

    case PGFarrowboxshaftwidth:
      state->setArrowBoxShaftWidth(v.expv->getResult(state));
      break;

    case PGFarrowboxeastarrow:
      {
        QPointF p = v.coordv->getPoint(state);
        state->setArrowBoxEastArrow(p);
      }
      break;

    case PGFarrowboxnortharrow:
      {
        QPointF p = v.coordv->getPoint(state);
        state->setArrowBoxNorthArrow(p);
      }
      break;

    case PGFarrowboxsoutharrow:
      {
        QPointF p = v.coordv->getPoint(state);
        state->setArrowBoxSouthArrow(p);
      }
      break;

    case PGFarrowboxwestarrow:
      {
        QPointF p = v.coordv->getPoint(state);
        state->setArrowBoxWestArrow(p);
      }
      break;

    case PGFrectanglesplitparts:
      state->setRectangleSplitParts((int)(v.expv->getResult(state)));
      break;

    case PGFrectanglesplithorizontal:
      state->setRectangleSplitHorizontal((bool)(v.expv->getResult(state)));
      break;

    case PGFrectanglesplitignoreemptyparts:
      state->setRectangleSplitIgnoreEmptyParts((bool)(v.expv->getResult(state)));
      break;

    case PGFrectanglesplitemptypartwidth:
      state->setRectangleSplitEmptyPartWidth(v.expv->getResult(state));
      break;

    case PGFrectanglesplitemptypartheight:
      state->setRectangleSplitEmptyPartHeight(v.expv->getResult(state));
      break;

    case PGFrectanglesplitemptypartdepth:
      state->setRectangleSplitEmptyPartDepth(v.expv->getResult(state));
      break;

    case PGFrectanglesplitdrawsplits:
      state->setRectangleSplitDrawSplits((bool)(v.expv->getResult(state)));
      break;
      
    case PGFrectanglesplitusecustomfill:
      state->setRectangleSplitUseCustomFill((bool)(v.expv->getResult(state)));
      break;

    case PGFcalloutrelativepointer:
      {
        QPointF p = v.coordv->getPoint(state);
        state->setCalloutRelativePointer(p);
      }
      break;

    case PGFcalloutpointershorten:
      state->setCalloutPointerShorten(v.expv->getResult(state));
      break;

    case PGFcalloutpointerwidth:
      state->setCalloutPointerWidth(v.expv->getResult(state));
      break;

    case PGFcalloutpointerarc:
      state->setCalloutPointerArc(v.expv->getResult(state));
      break;

    case PGFcalloutpointerstartsize:
      {
        QPointF p = v.coordv->getPoint(state);
        state->setCalloutPointerStartSize(p);
      }
      break;

    case PGFcalloutpointerendsize:
      {
        QPointF p = v.coordv->getPoint(state);
        state->setCalloutPointerEndSize(p);
      }
      break;

    case PGFcalloutpointersegments:
      state->setCalloutPointerSegments((int)(v.expv->getResult(state)));
      break;

    case PGFroundedrectanglearclength:
      state->setRoundedRectangleArcLength(v.expv->getResult(state));
      break;

    case PGFroundedrectanglewestarc:
      state->setRoundedRectangleWestArc((int)(v.expv->getResult(state)));
      break;

    case PGFroundedrectangleleftarc:
      state->setRoundedRectangleLeftArc((int)(v.expv->getResult(state)));
      break;

    case PGFroundedrectangleeastarc:
      state->setRoundedRectangleEastArc((int)(v.expv->getResult(state)));
      break;

    case PGFroundedrectanglerightarc:
      state->setRoundedRectangleRightArc((int)(v.expv->getResult(state)));
      break;

    case PGFchamferedrectangleangle:
      state->setChamferedRectangleAngle(v.expv->getResult(state));
      break;

    case PGFchamferedrectanglexsep:
      state->setChamferedRectangleXsep(v.expv->getResult(state));
      break;

    case PGFchamferedrectangleysep:
      state->setChamferedRectangleYsep(v.expv->getResult(state));
      break;

    case PGFchamferedrectanglesep:
      {
        QPointF p = v.coordv->getPoint(state);
        state->setChamferedRectangleXsep(p.x());
        state->setChamferedRectangleYsep(p.y());
      }
      break;

    case PGFclockwisefrom:
      state->clockwiseFrom(v.expv->getResult(state));
      break;

    case PGFcounterclockwisefrom:
      state->counterClockwiseFrom(v.expv->getResult(state));
      break;

    case PGFsiblingangle:
      state->setSiblingAngle(v.expv->getResult(state));
      break;

    case PGFsize:
      {
        double s = v.expv->getResult(state);
        state->seiMinimumWidth(s);
        state->seiMinimumHeight(s);
      }
      break;

    case PGFheight:
      state->seiMinimumHeight(v.expv->getResult(state));
      break;

    case PGFwidth:
      state->seiMinimumWidth(v.expv->getResult(state));
      break;

    case PGFmagnification:
      {
        double s = v.expv->getResult(state);
        state->scale(s,s);
      }
      break;

    case PGFmarkconnectionnode:
      state->setMarkNode(text);
      break;

    case PGFshortenend:
      state->setShortenEnd(v.expv->getResult(state));
      break;

    case PGFshortenstart:
      state->setShortenStart(v.expv->getResult(state));
      break;

    case PGFcircuitsymbolunit:
      state->setCircuitSizeUnit(v.expv->getResult(state));
      break;

    case PGFlogicgateinvertedradius:
      state->setLogicGateInvertedRadius(v.expv->getResult(state));
      break;

    case PGFlogicgateinputsep:
      state->setLogicGateInputSep(v.expv->getResult(state));
      break;

    case PGFlogicgateanchorsuseboundingbox:
      state->setLogicGateAnchorsUseBoundingBox((bool)v.expv->getResult(state));
      break;

    case PGFlogicgateIECsymbolalign:
      state->setLogicGateIECSymbolAlign((int)v.expv->getResult(state));
      break;

    case PGFandgateIECsymbol:
      state->setAndGateIECSymbol(text);
      break;

    case PGFnandgateIECsymbol:
      state->setNAndGateIECSymbol(text);
      break;

    case PGForgateIECsymbol:
      state->setOrGateIECSymbol(text);
      break;

    case PGFnorgateIECsymbol:
      state->setNorGateIECSymbol(text);
      break;

    case PGFxorgateIECsymbol:
      state->setXorGateIECSymbol(text);
      break;

    case PGFxnorgateIECsymbol:
      state->setXNorGateIECSymbol(text);
      break;

    case PGFnotgateIECsymbol:
      state->setNotGateIECSymbol(text);
      break;

    case PGFbuffergateIECsymbol:
      state->setBufferGateIECSymbol(text);
      break;

    case PGFplaneorigin:
      {
        QPointF p = v.coordv->getPoint(state);
        state->setPlaneOrigin(p);
      }
      break;

    case PGFplanex:
      {
        QPointF p = v.coordv->getPoint(state);
        state->setPlaneX(p);
      }
      break;

    case PGFplaney:
      {
        QPointF p = v.coordv->getPoint(state);
        state->setPlaneY(p);
      }
      break;

    case PGFcanvasisxyplaneatz:
      {
        state->setPlane(1,0,0,1,0,0);
      }
      break;

    case PGFcanvasisyxplaneatz:
      {
        state->setPlane(0,1,1,0,0,0);
      }
      break;

    case PGFcanvasisxzplaneaty:
      {
        double x = v.expv->getResult(state);
        state->setPlane(1,x,0,x,0,x);
      }
      break;

    case PGFcanvasiszxplaneaty:
      {
        double x = v.expv->getResult(state);
        state->setPlane(0,x,x,1,0,x);
      }
      break;

    case PGFcanvasisyzplaneatx:
      {
        double x = v.expv->getResult(state);
        state->setPlane(x,1,x,0,x,0);
      }
      break;

    case PGFcanvasiszyplaneatx:
      {
        double x = v.expv->getResult(state);
        state->setPlane(x,0,x,1,x,0);
      }
      break;

    case PGFinitialtext:
      state->setInitialText(text);
      break;

    case PGFacceptingtext:
      state->setAcceptingText(text);
      break;

    case PGFinitialdistance:
      state->setInitialDistance(v.expv->getResult(state));
      break;

    case PGFacceptingdistance:
      state->setAcceptingDistance(v.expv->getResult(state));
      break;

    case PGFinitialwhere:
      {
        int a = (int)v.expv->getResult(state);
        switch (a)
        {
          default:
            break;

          case PGFabove:
            state->setInitialAngle(90);
            state->setInitialAnchor(PGFsouth);
            break;

          case PGFbelow:
            state->setInitialAngle(270);
            state->setInitialAnchor(PGFnorth);
            break;

          case PGFleft:
            state->setInitialAngle(180);
            state->setInitialAnchor(PGFeast);
            break;

          case PGFright:
            state->setInitialAngle(0);
            state->setInitialAnchor(PGFwest);
            break;
        }
      }
      break;

    case PGFacceptingwhere:
      {
        int a = (int)v.expv->getResult(state);
        switch (a)
        {
          default:
            break;

          case PGFabove:
            state->setAcceptingAngle(90);
            state->setAcceptingAnchor(PGFsouth);
            break;

          case PGFbelow:
            state->setAcceptingAngle(270);
            state->setAcceptingAnchor(PGFnorth);
            break;

          case PGFleft:
            state->setAcceptingAngle(180);
            state->setAcceptingAnchor(PGFeast);
            break;

          case PGFright:
            state->setAcceptingAngle(0);
            state->setAcceptingAnchor(PGFwest);
            break;
        }
      }
      break;

    case PGFshadowscale:
      state->setShadowScale(v.expv->getResult(state));
      break;

    case PGFshadowxshift:
      state->setShadowXShift(v.expv->getResult(state));
      break;

    case PGFshadowyshift:
      state->setShadowYShift(v.expv->getResult(state));
      break;

    case PGFbendangle:
      state->setBendAngle(v.expv->getResult(state));
      break;

    case PGFbendleft:
      state->setBendLeft(v.expv->getResult(state));
      break;

    case PGFbendright:
      state->setBendRight(v.expv->getResult(state));
      break;

    case PGFrelative:
      state->setRelative((bool)v.expv->getResult(state));
      break;

    case PGFin:
      state->setIn(v.expv->getResult(state));
      break;

    case PGFout:
      state->setOut(v.expv->getResult(state));
      break;

    case PGFinlooseness:
      state->setInLooseness(v.expv->getResult(state));
      break;

    case PGFoutlooseness:
      state->setOutLooseness(v.expv->getResult(state));
      break;

    case PGFlooseness:
      state->setInLooseness(v.expv->getResult(state));
      state->setOutLooseness(v.expv->getResult(state));
      break;

    case PGFincontrol:
      {
        QPointF p = v.coordv->getPoint(state);
        state->setInControl(p);
      }
      break;

    case PGFoutcontrol:
      {
         QPointF p = v.coordv->getPoint(state);
        state->setOutControl(p);
      }
      break;

    case PGFinmindistance:
      state->setInMin(v.expv->getResult(state));
      break;

    case PGFinmaxdistance:
      state->setInMax(v.expv->getResult(state));
      break;

    case PGFindistance:
      state->setInMin(v.expv->getResult(state));
      state->setInMax(v.expv->getResult(state));
      break;

    case PGFoutmindistance:
      state->setOutMin(v.expv->getResult(state));
      break;

    case PGFoutmaxdistance:
      state->setOutMax(v.expv->getResult(state));
      break;

    case PGFoutdistance:
      state->setOutMin(v.expv->getResult(state));
      state->setOutMax(v.expv->getResult(state));
      break;

    case PGFmindistance:
      state->setInMin(v.expv->getResult(state));
      state->setOutMin(v.expv->getResult(state));
      break;

    case PGFmaxdistance:
      state->setInMax(v.expv->getResult(state));
      state->setOutMax(v.expv->getResult(state));
      break;

    case PGFdistance:
      state->setInMin(v.expv->getResult(state));
      state->setInMax(v.expv->getResult(state));
      state->setOutMin(v.expv->getResult(state));
      state->setOutMax(v.expv->getResult(state));
      break;

    case PGFtokendistance:
      state->setTokenDistance(v.expv->getResult(state));
      break;

    case PGFtokens:
      {
        int num = (int)(v.expv->getResult(state));
        state->setChildrenNumber(num);
        for (int i = 1; i <= num; i++)
        {
          XWTikzState * newstate = state->saveNode(0,XW_TIKZ_NODE); 
          newstate->setToken(i);
          newstate->setFillColor(Qt::black);
        }
      }
      break;

    case PGFtension:
      state->setTension(v.expv->getResult(state));
      break;

    case PGFbarwidth:
      state->setBarWidth(v.expv->getResult(state));
      break;

    case PGFbarshift:
      state->setBarShift(v.expv->getResult(state));
      break;

    case PGFbarintervalwidth:
      state->setBarIntervalWidth(v.expv->getResult(state));
      break;

    case PGFbarintervalshift:
      state->setBarIntervalShift(v.expv->getResult(state));
      break;

    case PGFfootlength:
      state->setFootLength(v.expv->getResult(state));
      break;

    case PGFstridelength:
      state->setStrideLength(v.expv->getResult(state));
      break;

    case PGFfootsep:
      state->setFootSep(v.expv->getResult(state));
      break;

    case PGFfootangle:
      state->setFootAngle(v.expv->getResult(state));
      break;

    case PGFfootof:
      state->setFootOf((int)(v.expv->getResult(state)));
      break;

    case PGFleftindent:
      state->setLeftIndent(v.expv->getResult(state));
      break;

    case PGFrightindent:
      state->setRightIndent(v.expv->getResult(state));
      break;

    case PGFfittopath:
      state->setFitToPath((bool)(v.expv->getResult(state)));
      break;

    case PGFfittopathstretchingspaces:
      state->setFitToPathStretchingSpaces((bool)(v.expv->getResult(state)));
      break;

    case PGFtext:
      state->setText(text);
      break;

    case PGFshapewidth:
      state->setShapeStartWidth(v.expv->getResult(state));
      state->setShapeEndWidth(v.expv->getResult(state));
      break;

    case PGFshapeheight:
      state->setShapeStartHeight(v.expv->getResult(state));
      state->setShapeEndHeight(v.expv->getResult(state));
      break;

    case PGFshapesize:
      state->setShapeStartWidth(v.expv->getResult(state));
      state->setShapeEndWidth(v.expv->getResult(state));
      state->setShapeStartHeight(v.expv->getResult(state));
      state->setShapeEndHeight(v.expv->getResult(state));
      break;

    case PGFshapesloped:
      state->setShapeSloped((bool)(v.expv->getResult(state)));
      break;

    case PGFshapescaled:
      state->setShapeScaled((bool)(v.expv->getResult(state)));
      break;

    case PGFshapestartwidth:
      state->setShapeStartWidth(v.expv->getResult(state));
      break;

    case PGFshapestartheight:
      state->setShapeStartHeight(v.expv->getResult(state));
      break;

    case PGFshapestartsize:
      state->setShapeStartWidth(v.expv->getResult(state));
      state->setShapeStartHeight(v.expv->getResult(state));
      break;

    case PGFshapeendwidth:
      state->setShapeEndWidth(v.expv->getResult(state));
      break;

    case PGFshapeendheight:
      state->setShapeEndHeight(v.expv->getResult(state));
      break;

    case PGFshapeendsize:
      state->setShapeEndWidth(v.expv->getResult(state));
      state->setShapeEndHeight(v.expv->getResult(state));
      break;
  }
}

QString XWTikzValue::getCoord()
{
  return v.coordv->getText();
}

QString XWTikzValue::getExpress()
{
  return v.expv->getText();
}

QPointF XWTikzValue::getPoint(XWTikzState * state)
{
  QPointF d = v.coordv->getPoint(state);
  return d;
}

QString XWTikzValue::getText()
{
  QString ret = getPGFString(keyWord);
  switch (keyWord)
  {
    case PGFroundedcorners:
    case PGFstep:
      {
        ret += "=";
        QString tmp = v.coordv->getText();
        ret += tmp;
      }
      break;

    case PGFshift:    
    case PGFcalloutrelativepointer:
    case PGFcalloutabsolutepointer:
    case PGFchamferedrectanglesep:
    case PGFplaneorigin:
    case PGFplanex:
    case PGFplaney:
    case PGFincontrol:
    case PGFoutcontrol:
      {
        ret += "={";
        QString tmp = v.coordv->getText();
        ret += tmp;
        ret += "}";
      }
      break;

    case PGFlinecap:
    case PGFlinejoin:
      {
        ret += "=";
        int k = (int)(v.expv->getValue());
        QString tmp = getPGFString(k);
        ret += tmp;
      }
      break;

    case PGFcalloutpointerstartsize:
    case PGFcalloutpointerendsize:
      {
        ret += "=";
        QString tmp = v.coordv->getText();
        if (tmp.startsWith("(-1,")) //XW_TIKZ_CPS_CALLOUT
        {
          int s = tmp.indexOf(",");
          int e = tmp.length() - 1;
          tmp = tmp.mid(s + 1, e - s - 1);
          tmp += " of callout";
        }
        else if (tmp.startsWith("(-2,"))//XW_TIKZ_CPS_SINGLE
        {
          int s = tmp.indexOf(",");
          int e = tmp.length() - 1;
          tmp = tmp.mid(s + 1, e - s - 1);
        }
        else//XW_TIKZ_CPS_AND
        {
          int s = tmp.indexOf(",");
          QString v1 = tmp.mid(1,s - 1);
          int e = tmp.length() - 1;
          QString v2 = tmp.mid(s + 1, e - s - 1);
          tmp = QString("%1 and %2").arg(v1).arg(v2);
        }
        ret += tmp;
      }
      break;

    case PGFarrowboxeastarrow:
    case PGFarrowboxnortharrow:
    case PGFarrowboxsoutharrow:
    case PGFarrowboxwestarrow:
      {
        ret += "=";
        QString tmp = v.coordv->getText();
        int s = tmp.indexOf(",") + 1;
        int e = tmp.length() - 1;
        tmp = tmp.mid(s, e - s);
        if (tmp.startsWith("(-2,"))
          tmp += " from center";
        ret += tmp;
      }
      break;

    case PGFmarkconnectionnode:
    case PGFnodecontents:
    case PGFnameprefix:
    case PGFnamesuffix:
    case PGFandgateIECsymbol:
    case PGFnandgateIECsymbol:
    case PGForgateIECsymbol:
    case PGFnorgateIECsymbol:
    case PGFxorgateIECsymbol:
    case PGFxnorgateIECsymbol:
    case PGFnotgateIECsymbol:
    case PGFbuffergateIECsymbol:
    case PGFinitialtext:
    case PGFacceptingtext:
    case PGFtext:
      ret += "=";
      ret += text;
      break;

    case PGFarrowdefault:
    case PGFinitialwhere:
    case PGFacceptingwhere:
    case PGFfootof:
      {
        ret += "=";
        XWTikzState state;
        int a = (int)v.expv->getResult(&state);
        QString tmp = getPGFString(a);
        ret += tmp;
      }
      break;

    default:
       {
         ret += "=";
         QString tmp = v.expv->getText();         
         ret += tmp;
       }
      break;
  }

  return ret;
}

QString XWTikzValue::getTips(XWTikzState * state)
{
  hitTest(state);
  return getText();
}

double XWTikzValue::getValue()
{
  return v.expv->getValue();
}

void XWTikzValue::scan(const QString & str, int & len, int & pos)
{
  if (keyWord == PGFshiftonly)
    return ;

  QString value;  
  switch (keyWord)
  {
    case PGFroundedcorners:
      scanValue(str,len,pos,value);
      if (value.isEmpty())
        value = "4pt";
      v.expv = new XWTikzExpress(graphic,value,this);
      break;

    case PGFstep:
      scanValue(str,len,pos,value);
      if (value[0] == QChar('('))
        v.coordv = new XWTikzCoord(graphic,value,this);
      else
      {
        QString tmp = QString("(%1,%2)").arg(value).arg(value);
        v.coordv = new XWTikzCoord(graphic,tmp,this);
      }
      break;

    case PGFshift:
    case PGFcalloutrelativepointer:
    case PGFcalloutabsolutepointer:
    case PGFchamferedrectanglesep:
    case PGFplaneorigin:
    case PGFplanex:
    case PGFplaney:
    case PGFincontrol:
    case PGFoutcontrol:
      scanValue(str,len,pos,value);
      v.coordv = new XWTikzCoord(graphic,value,this);
      break;

    case PGFlinecap:
    case PGFlinejoin:
    case PGFanchor:
      scanValue(str,len,pos,value);
      v.expv->setValue(lookupPGFID(value));
      break;

    case PGFbaseline:
      scanValue(str,len,pos,value);
      if (value[0] == QChar('('))
        v.coordv = new XWTikzCoord(graphic,value,this);
      else
        v.expv = new XWTikzExpress(graphic,value,this);
      break;

    case PGFnodecontents:
    case PGFnameprefix:
    case PGFnamesuffix:
    case PGFmarkconnectionnode:
    case PGFandgateIECsymbol:
    case PGFnandgateIECsymbol:
    case PGForgateIECsymbol:
    case PGFnorgateIECsymbol:
    case PGFxorgateIECsymbol:
    case PGFxnorgateIECsymbol:
    case PGFnotgateIECsymbol:
    case PGFbuffergateIECsymbol:
    case PGFinitialtext:
    case PGFacceptingtext:
    case PGFtext:
      scanValue(str,len,pos,text);
      break;

    case PGFcalloutpointerstartsize:
    case PGFcalloutpointerendsize:
      scanValue(str,len,pos,value);
      {
        QString ss;
        if (value.endsWith("of callout"))
        {
          int i = 0;
          while (!value[i].isSpace())
            i++;

          value = value.left(i);
          ss = QString("(-1,%1)").arg(value);
        }
        else if (value.contains("and"))
        {
          QStringList list = value.split("and");
          ss = QString("(%1,%2)").arg(list[0]).arg(list[1]);
        }
        else
        {
          ss = QString("(-2,%1)").arg(value);
        }
        v.coordv = new XWTikzCoord(graphic,ss,this);
      }
      break;

    case PGFarrowboxeastarrow:
    case PGFarrowboxnortharrow:
    case PGFarrowboxsoutharrow:
    case PGFarrowboxwestarrow:
      scanValue(str,len,pos,value);
      if (value.endsWith("from center"))//XW_TIKZ_FROM_CENTER
      {
        int i = value.indexOf("from center");
        value = value.left(i);
        QString ss = QString("(-2,%2)").arg(value);
        v.coordv = new XWTikzCoord(graphic,ss,this);
      }
      else//XW_TIKZ_FROM_BORDER
      {
        QString ss = QString("(-1,%2)").arg(value);
        v.coordv = new XWTikzCoord(graphic,ss,this);
      }
      break;

    case PGFarrowdefault:
    case PGFlabelposition:
    case PGFpinposition:
    case PGFinitialwhere:
    case PGFacceptingwhere:
    case PGFfootof:
      scanValue(str,len,pos,value);
      if (value[0].isLetter())
      {
        int id = lookupPGFID(value);
        value = QString("%1").arg(id);
      }
      v.expv = new XWTikzExpress(graphic,value,this);
      break;

    default:
      scanValue(str,len,pos,value);
      if (value.isEmpty())
        value = "true";
      v.expv = new XWTikzExpress(graphic,value,this);
      break;
  }
}

void XWTikzValue::setCoord(const QString & str)
{
  if (!v.coordv)
    v.coordv = new XWTikzCoord(graphic,str,this);
  else
    v.coordv->setText(str);
}

void XWTikzValue::setExpress(const QString & str)
{
  if (!v.expv)
    v.expv = new XWTikzExpress(graphic,str,this);
  else
    v.expv->setText(str);
}

void XWTikzValue::setValue(double r)
{
  if (!v.expv)
    v.expv = new XWTikzExpress(graphic,this);
  v.expv->setValue(r);
}

XWTikzDomain::XWTikzDomain(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, PGFdomain,parent),
start(-5),
end(5)
{}

void XWTikzDomain::doCompute(XWTikzState * state)
{
  state->setDomain(start,end);
}

void XWTikzDomain::doPath(XWTikzState * state, bool)
{
  state->setDomain(start,end);
}

void XWTikzDomain::dragTo(XWTikzState * state)
{
  doPath(state);
}

QString XWTikzDomain::getEnd()
{
  QString ret = QString("%1").arg(end);
  return ret;
}

QString XWTikzDomain::getStart()
{
  QString ret = QString("%1").arg(start);
  return ret;
}

QString XWTikzDomain::getText()
{
  QString ret = QString("%1:%2").arg(start).arg(end);
  return ret;
}

void XWTikzDomain::scan(const QString & str, int & len, int & pos)
{
  while (pos < len && str[pos].isSpace())
    pos++;
  int i = pos;
  while (str[pos] != QChar(':'))
    pos++;

  QString tmp = str.mid(i,pos);
  start = tmp.toDouble();

  while (str[pos].isSpace())
    pos++;

  i = pos;
  while (pos < len && str[pos] != QChar(',') && str[pos] != QChar(']'))
    pos++;

  tmp = str.mid(i,pos);
  tmp = tmp.simplified();
  end = tmp.toDouble();
  if (str[pos] == QChar(','))
    pos++;
}

void XWTikzDomain::setEnd(const QString & str)
{
  end = str.toDouble();
}

void XWTikzDomain::setStart(const QString & str)
{
  start = str.toDouble();
}

XWTikzSamplesAt::XWTikzSamplesAt(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, PGFsamplesat,parent)
{}

void XWTikzSamplesAt::doCompute(XWTikzState * state)
{
  state->setSamplesAt(samples);
}

void XWTikzSamplesAt::doPath(XWTikzState * state, bool)
{
  state->setSamplesAt(samples);
}

void XWTikzSamplesAt::dragTo(XWTikzState * state)
{
  doPath(state);
}

QString XWTikzSamplesAt::getText()
{
  QString ret = "{";
  for (int i = 0; i < samples.size(); i++)
  {
    QString d = QString("%1").arg(samples.at(i));
    ret += d;
    if (i < samples.size() - 1)
      ret += ",";
  }

  ret += "}";

  return ret;
}

void XWTikzSamplesAt::scan(const QString & str, int & len, int & pos)
{
  while (str[pos].isSpace())
    pos++;

  if (str[pos] == QChar('{'))
    pos++;

  while (pos < len)
  {
    if (str[pos] == QChar('}'))
    {
      pos++;
      break;
    }
    else if (str[pos].isSpace() || str[pos] == QChar(','))
      pos++;
    else
    {
      int i = pos;
      while (str[pos] != QChar(',') && str[pos] != QChar('}'))
        pos++;

      QString tmp = str.mid(i,pos);
      tmp = tmp.simplified();
      double d = tmp.toDouble();
      samples << d;
    }    
  }
}

void XWTikzSamplesAt::setSamples(const QString & str)
{
  samples.clear();
  int len = str.length();
  int pos = 0;
  scan(str,len,pos);
}

XWTikzColor::XWTikzColor(XWTikzGraphic * graphicA, int idA, QObject * parent)
:XWTikzOperation(graphicA, idA,parent),
C1(0),
P(1),
C2(0)
{}

void XWTikzColor::doPath(XWTikzState * state, bool)
{
  QColor color = getColor();
  switch (keyWord)
  {
    default:
      break;
      
    case PGFcolor:
    case PGFfilldraw:
      state->setColor(color);
      break;

    case PGFdraw:
      state->setDrawColor(color);
      break;

    case PGFfill:    
      state->setFillColor(color);
      break;

    case PGFconceptcolor:
      state->setConceptColor(color);
      break;

    case PGFpatterncolor:
      state->setPatternColor(color);
      break;

    case PGFtopcolor:
      state->setTopColor(color);
      break;

    case PGFmiddlecolor:
      state->setMiddleColor(color);
      break;

    case PGFbottomcolor:
      state->setBottomColor(color);
      break;

    case PGFleftcolor:
      state->setLeftColor(color);
      break;

    case PGFrightcolor:
      state->setRightColor(color);
      break;

    case PGFballcolor:
      state->setBallColor(color);
      break;

    case PGFlowerleft:
      state->setLowerLeftColor(color);
      break;

    case PGFlowerright:
      state->setLowerRightColor(color);
      break;

    case PGFupperleft:
      state->setUpperLeftColor(color);
      break;

    case PGFupperright:
      state->setUpperRightColor(color);
      break;

    case PGFinnercolor:
      state->setInnerColor(color);
      break;

    case PGFoutercolor:
      state->setOuterColor(color);
      break;

    case PGFdouble:
      {
        double w = state->getLineWidth();
        state->setLineWidth(2 * w + 0.6);
        state->setInnerLineWidth(0.6);
        state->setCoreColor(color);
      }
      break;

    case PGFtext:
      state->setTextColor(color);
      break;

    case PGFcylinderendfill:
      state->setCylinderEndFill(color);
      break;

    case PGFcylinderbodyfill:
      state->setCylinderBodyFill(color);
      break;

    case PGFlogicgateIECsymbolcolor:
      state->setLogicGateIECSymbolColor(color);
      break;
  }
}

QColor XWTikzColor::getColor()
{
  QColor color1 = tikzEnumToColor(C1);

  double r1 = color1.redF();
  double g1 = color1.greenF();
  double b1 = color1.blueF();

  QColor color;
  if (C2 > 0)
  {
    QColor color2 = tikzEnumToColor(C2);
    double r2 = color2.redF();
    double g2 = color2.greenF();
    double b2 = color2.blueF();

    double r = r1 * P + r2 * (1 - P);
    double g = g1 * P + g2 * (1 - P);
    double b = b1 * P + b2 * (1 - P);
    color.setRedF(r);
    color.setGreenF(g);
    color.setBlueF(b);
  }
  else
  {
    double r = r1 * P + 1-P;
    double g = g1 * P + 1-P;
    double b = b1 * P + 1-P;
    color.setRedF(r);
    color.setGreenF(g);
    color.setBlueF(b);
  }
  return color;
}

void XWTikzColor::getColor(int & c1A,double & pA,int & c2A)
{
  c1A = C1;
  pA = P;
  c2A = C2;
}

QString XWTikzColor::getText()
{
  QString ret;
  if (keyWord > 0)
  {
    ret = getPGFString(keyWord);
    ret += "=";
  }
  
  QString c1 = getPGFString(C1);
  ret += c1;
  int p = (int)(P * 100);
  if (P != 1)
  {
    QString tmp = QString("!%1").arg(p);
    ret += tmp;
  }

  if (C2 > 0)
  {
    QString c2 = getPGFString(C2);
    QString tmp = QString("!%1").arg(c2);
    ret += tmp;
  }
  return ret;
}

void XWTikzColor::scan(const QString & str, int & len, int & pos)
{
  int i = pos;
  while (str[pos].isLetter())
    pos++;

  QString key = str.mid(i,pos);
  C1 = lookupPGFID(key);
  if (str[pos] == QChar('!'))
  {
    pos++;
    i = pos;
    while (str[pos].isNumber())
      pos++;
    key = str.mid(i,pos);
    P = (double)(key.toInt()) / 100.00;
    if (str[pos] == QChar('!'))
      pos++;
  }

  if (pos < len)
  {
    i = pos;
    while (str[pos].isLetter())
      pos++;
    key = str.mid(i,pos);
    C2 = lookupPGFID(key);
  }
}

void XWTikzColor::setColor(int c1A,double pA,int c2A)
{
  C1 = c1A;
  P = pA;
  C2 = c2A;
}

XWTikzSwitchColor::XWTikzSwitchColor(XWTikzGraphic * graphicA, int idA, QObject * parent)
:XWTikzOperation(graphicA, idA,parent)
{
  from = new XWTikzColor(graphicA,-1,this);
  to = new XWTikzColor(graphicA,-1,this);
}

void XWTikzSwitchColor::doPath(XWTikzState * state, bool)
{
  QColor f = from->getColor();
  QColor t = to->getColor();
  state->switchColor(f,t);
}

QString XWTikzSwitchColor::getText()
{
  QString ret = getPGFString(keyWord);
  ret += "= from ";
  QString tmp = from->getText();
  ret += tmp;
  ret += " to ";
  tmp = to->getText();
  ret += tmp;
  return ret;
}

void XWTikzSwitchColor::scan(const QString & str, int & len, int & pos)
{
  QString value;
  scanValue(str,len,pos,value);
  value.remove("from");
  int i = value.indexOf("to");
  QString c = value.left(i);
  c = c.simplified();
  int l = c.length();
  int p = 0;
  from->scan(c,l,p);
  i += 3;
  while (value[i].isSpace())
    i++;
  c = value.mid(i,-1);
  c = c.simplified();
  l = c.length();
  p = 0;
  to->scan(c,l,p);
}

XWTikzColoredTokens::XWTikzColoredTokens(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, PGFcoloredtokens,parent)
{}

void XWTikzColoredTokens::doPath(XWTikzState * state, bool)
{
  state->setChildrenNumber(colors.size());
  for (int i = 0; i < colors.size(); i++)
  {
    XWTikzState * newstate = state->saveNode(0,XW_TIKZ_NODE);
    newstate->setToken(i + 1);
    QColor c = colors[i]->getColor();
    newstate->setFillColor(c);
  }
}

QString XWTikzColoredTokens::getText()
{
  QString ret = "colored tokens={";
  for (int i = 0; i < colors.size(); i++)
  {
    QString tmp = colors[i]->getText();
    ret += tmp;
    if (i < colors.size() - 1)
      ret += ",";
  }
  ret += "}";
  return ret;
}

void XWTikzColoredTokens::scan(const QString & str, int & len, int & pos)
{
  while (pos < len && str[pos].isSpace())
    pos++;

  if (str[pos] == QChar('{'))
    pos++;

  while (pos < len)
  {
    if (str[pos] == QChar('}'))
    {
      pos++;
      break;
    }
    else if (str[pos].isSpace())
      pos++;
    else
    {
      XWTikzColor * c = new XWTikzColor(graphic, -1, this);
      c->scan(str,len,pos);
      colors << c;
    }
  }
}

XWTikzDashPattern::XWTikzDashPattern(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, PGFdashpattern,parent)
{}

void XWTikzDashPattern::doPath(XWTikzState * state, bool)
{
  state->setDashPattern(pattern);
}

void XWTikzDashPattern::dragTo(XWTikzState * state)
{
  doPath(state);
}

QString XWTikzDashPattern::getText()
{
  QString ret="dash pattern=";
  bool on = true;
  for (int i = 0; i < pattern.size(); i++)
  {
    if (on)
    {
      QString tmp = QString("on %1 ").arg(pattern[i]);
      on = false;
    }
    else
    {
      QString tmp = QString("off %1 ").arg(pattern[i]);
      on = true;
    }
  }

  return ret;
}

void XWTikzDashPattern::scan(const QString & str, int & len, int & pos)
{
  while (pos < len)
  {
    if (str[pos].isSpace())
      pos++;
    else if (str[pos] == QChar(',') || 
        str[pos] == QChar(']') || 
        str[pos] == QChar('}'))
    {
      pos++;
      break;
    }
    else if (str[pos] == QChar('o'))
    {
      pos++;
      if (str[pos] == QChar('n'))
        pos++;
      else
      pos += 2;
    }
    else
    {
      int i = pos;
      while (str[pos].isDigit() || str[pos] == QChar('.'))
        pos++;

      QString tmp = str.mid(i,pos);
      double v = tmp.toDouble();
      pattern << v;
      pos += 2;
    }
    
  }
}

XWTikzArrowKey::XWTikzArrowKey(XWTikzGraphic * graphicA, int idA, QObject * parent)
:XWTikzOperation(graphicA, idA,parent)
{}

void XWTikzArrowKey::doPath(XWTikzState * state, bool )
{
  switch (keyWord)
  {
    default:
      break;

    case PGFreversed:
      state->setArrowReversed(true);
      break;

    case PGFharpoon:
      state->setArrowHarpoon(true);
      break;

    case PGFswap:
      state->setArrowSwap(true);
      break;

    case PGFleft:
      state->setArrowHarpoon(true);
      break;

    case PGFright:
      state->setArrowHarpoon(true);
      state->setArrowSwap(true);
      break;

    case PGFopen:
      state->setArrowFill(false);
      break;

    case PGFround:
      state->setArrowLineCap(PGFround);
      state->setArrowLineJoin(PGFround);
      break;

    case PGFsharp:
      state->setArrowLineCap(PGFbutt);
      state->setArrowLineJoin(PGFmiter);
      break;

    case PGFquick:
    case PGFbend:
      state->setArrowFlexMode(keyWord);
      break;
  }
}

QString XWTikzArrowKey::getText()
{
  QString ret = getPGFString(keyWord);
  return ret;
}

XWTikzArrowValue::XWTikzArrowValue(XWTikzGraphic * graphicA, int idA, QObject * parent)
:XWTikzOperation(graphicA, idA,parent)
{
  v.expv = 0;
  v.coordv = 0;
}

void XWTikzArrowValue::doPath(XWTikzState * state, bool )
{
  switch (keyWord)
  {
    default:
      break;

    case PGFscale:
      state->setArrowScaleLength(v.expv->getResult(state));
      state->setArrowScaleWidth(v.expv->getResult(state));
      break;

    case PGFscalelength:
      state->setArrowScaleLength(v.expv->getResult(state));
      break;

    case PGFscalewidth:
      state->setArrowScaleWidth(v.expv->getResult(state));
      break;

    case PGFarc:
      state->setArrowArc(v.expv->getResult(state));
      break;

    case PGFslant:
      state->setArrowSlant(v.expv->getResult(state));
      break;

    case PGFlinecap:
      state->setArrowLineCap((int)(v.expv->getResult(state)));
      break;

    case PGFlinejoin:
      state->setArrowLineJoin((int)(v.expv->getResult(state)));
      break;

    case PGFflex:
    case PGFflexrot:
      state->setArrowFlex(v.expv->getResult(state));
      state->setArrowFlexMode(keyWord);
      break;

    case PGFanglelength:
      state->angleSetupPrime(v.expv->getResult(state));
      break;

    case PGFcapangle:
      {
        double x = 0.5 * v.expv->getResult(state);
        x = 0.5 / tan(x);
        state->setArrowLength(state->lineWidthDependent(0,x,0));
      }
  }
}

QString XWTikzArrowValue::getText()
{
  QString ret = getPGFString(keyWord);
  ret += "=";
  QString tmp = v.expv->getText();
  ret += tmp;
  return ret;
}

void XWTikzArrowValue::scan(const QString & str, int & len, int & pos)
{
  QString value;
  scanValue(str,len,pos,value);
  v.expv = new XWTikzExpress(graphic,value,this);
}

XWTikzArrowColor::XWTikzArrowColor(XWTikzGraphic * graphicA, int idA, QObject * parent)
:XWTikzColor(graphicA, idA,parent)
{}

void XWTikzArrowColor::doPath(XWTikzState * state, bool )
{
  QColor color = getColor();
  switch (keyWord)
  {
    default:
      break;
      
    case PGFcolor:
      state->setArrowDrawColor(color);
      break;

    case PGFfill:
      state->setArrowFillColor(color);
      break;
  }
}

XWTikzArrowDependent::XWTikzArrowDependent(XWTikzGraphic * graphicA, int idA, QObject * parent)
:XWTikzOperation(graphicA, idA,parent),
 angle(0),
 dimension(0),
 firstFactor(0),
 lastFactor(0)
{}

void XWTikzArrowDependent::doPath(XWTikzState * state, bool)
{
  double a = 0;
  if (angle)
    a = angle->getResult(state);
  double d = 0;
  if (dimension)
    d = dimension->getResult(state);
  double f = 0;
  if (firstFactor)
    f = firstFactor->getResult(state);
  double l = 0;
  if (lastFactor)
    l = lastFactor->getResult(state);
  switch (keyWord)
  {
    default:
      break;

    case PGFlength:
      state->setArrowLength(state->lineWidthDependent(d,f,l));
      break;

    case PGFwidth:
      state->setArrowWidth(state->lineWidthDependent(d,f,l));
      break;

    case PGFwidthlength:
      state->setArrowWidth(state->lengthDependent(d,f,l));
      break;

    case PGFinset:
      state->setArrowInset(state->lineWidthDependent(d,f,l));
      break;

    case PGFinsetlength:
      state->setArrowInset(state->lengthDependent(d,f,l));
      break;

    case PGFangle:
      state->angleSetup(a,d,f,l);
      break;

    case PGFlinewidth:
      state->setArrowLineWidth(state->lineWidthDependent(d,f,l));
      break;

    case PGFlinewidthlength:
      state->setArrowLineWidth(state->lengthDependent(d,f,l));
      break;

    case PGFsep:
      state->setArrowSep(state->lineWidthDependent(d,f,l));
      break;
  }
}

QString XWTikzArrowDependent::getText()
{
  QString ret = getPGFString(keyWord);
  if (angle)
  {
    ret += "=";
    QString tmp = angle->getText();
    ret += tmp;
    if (dimension)  
      ret += ":";
  }

  if (dimension)  
  {
    if (!angle)
      ret += "=";
    QString tmp = dimension->getText();
    ret += tmp;

    if (firstFactor)
    {
      ret += " ";
      QString tmp = firstFactor->getText();
      ret += tmp;

      if (lastFactor)
      {
        ret += " ";
        tmp = lastFactor->getText();
        ret += tmp;
      }
    }
  }  

  return ret;
}

void XWTikzArrowDependent::scan(const QString & str, int & , int & pos)
{
  while (str[pos].isSpace())
    pos++;

  if (str[pos] == QChar('='))
  {
    pos++;
    while (str[pos].isSpace())
      pos++;
    int i = pos;
    while (!str[pos].isSpace() && str[pos] != QChar(':'))
      pos++;

    QString v;
    if (str[pos] == QChar(':'))
    {
      i++;
      v = str.mid(i,pos - i);
      angle = new XWTikzExpress(graphic,v,this);
      pos++;
      while (str[pos].isSpace())
        pos++;
      i = pos;
      while (!str[pos].isSpace())
        pos++;
    }
    v = str.mid(i,pos - i);
    dimension = new XWTikzExpress(graphic,v,this);
    pos++;
    if (str[pos].isSpace())
    {
      pos++;
      if (str[pos].isDigit())
      {
        i = pos;
        while (!str[pos].isSpace() && str[pos] != QChar(']'))
          pos++;
        v = str.mid(i,pos - i);
        firstFactor = new XWTikzExpress(graphic,v,this);
        if (str[pos].isSpace())
        {
          pos++;
          if (str[pos].isDigit())
          {
            i = pos;
            while (!str[pos].isSpace() && str[pos] != QChar(']'))
              pos++;
            v = str.mid(i,pos - i);
            lastFactor = new XWTikzExpress(graphic,v,this);
          }
        }
      }
    }
  }
}

XWTikzArrowTipSpecification::XWTikzArrowTipSpecification(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, PGFarrow,parent),
arrow(PGFarrowdefault)
{}

void XWTikzArrowTipSpecification::doPath(XWTikzState * state, bool showpoint)
{
  for (int i = 0; i < ops.size(); i++)
    ops[i]->doPath(state,showpoint);

  state->drawArrow(arrow);
}

QString XWTikzArrowTipSpecification::getText()
{
  QString ret = getPGFString(arrow);
  if (ops.size() > 0)
  {
    ret += "[";
    for (int i = 0; i < ops.size(); i++)
    {
      QString tmp = ops[i]->getText();
      ret += tmp;
      if (i < (ops.size() - 1))
        ret += ",";
    }
    ret += "]";
  }

  return ret;
}

void XWTikzArrowTipSpecification::scan(const QString & str, int & len, int & pos)
{
  while (str[pos].isSpace())
    pos++;

  int i = pos;
  while (str[pos] == QChar(',') && str[pos] == QChar('-') && str[pos] == QChar(']'))
    pos++;
  if (pos == i)
    pos++;

  QString key = str.mid(i, pos - i);
  arrow = lookupPGFID(key);

  while (str[pos].isSpace())
    pos++;
  if (str[pos] == QChar('['))
  {
    pos++;
    while (pos < len)
    {
      if (str[pos] == QChar(']'))
      {
        pos++;
        break;
      }

      XWTikzOperation * op = scanOption(str,len,pos,this);
      if (op)
        ops << op;
    }
  }
}

XWTikzOperation * XWTikzArrowTipSpecification::scanOption(const QString & str, int & len, int & pos,XWTikzOperation * parent)
{
  QString key;
  scanKey(str,len,pos,key);
  int id = lookupPGFID(key);
  XWTikzOperation * op = 0;
  switch (id)
  {
    default:
      break;

    case PGFlength:
    case PGFinset:
    case PGFinsetlength:
    case PGFlinewidth:
    case PGFlinewidthlength:
    case PGFwidth:
    case PGFwidthlength:
    case PGFangle:
    case PGFsep:
      op = new XWTikzArrowDependent(parent->graphic,id,parent);
      op->scan(str,len,pos);
      break;

    case PGFfill:
    case PGFcolor:
      op = new XWTikzArrowColor(parent->graphic,id,parent);
      op->scan(str,len,pos);
      break;

    case PGFanglelength:
    case PGFscale:
    case PGFscalelength:
    case PGFscalewidth:
    case PGFarc:
    case PGFslant:
    case PGFlinecap:
    case PGFlinejoin:
    case PGFflex:
    case PGFflexrot:
    case PGFcapangle:
      op = new XWTikzArrowValue(parent->graphic,id,parent);
      op->scan(str,len,pos);
      break;

    case PGFbutt:
    case PGFmiter:
    case PGFround:
    case PGFsharp:
    case PGFswap:
    case PGFreversed:
    case PGFharpoon:
    case PGFleft:
    case PGFright:
    case PGFopen:
    case PGFquick:
    case PGFbend:
      op = new XWTikzArrowKey(parent->graphic,id,parent);
      break;
  }

  while (str[pos].isSpace())
    pos++;

  if (str[pos] == QChar(','))
    pos++;

  return op;
}

void XWTikzArrowTipSpecification::setup(XWTikzState * state)
{
  for (int i = 0; i < ops.size(); i++)
    ops[i]->doPath(state,false);

  state->setupArrow(arrow);
}

XWTikzArrowSpecification::XWTikzArrowSpecification(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, PGFtips,parent)
{}

void XWTikzArrowSpecification::doPath(XWTikzState * state, bool showpoint)
{
  for (int i = 0; i < ops.size(); i++)
    ops[i]->doPath(state,showpoint);

  for (int i = 0; i < tips.size(); i++)
    tips[i]->doPath(state,showpoint);
}

int XWTikzArrowSpecification::getArrow()
{
  if (tips.size() == 0)
    return -1;

  return tips[tips.size() - 1]->getArrow();
}

QString XWTikzArrowSpecification::getText()
{
  QString ret;
  if (ops.size() > 0)
  {
    ret += "[";
    for (int i = 0; i < ops.size(); i++)
    {
      QString tmp = ops[i]->getText();
      ret += tmp;
      if (i < (ops.size() - 1))
        ret += ",";
    }
    ret += "]";
  }

  if (tips.size() > 0)
  {
    for (int i = 0; i < tips.size(); i++)
    {
      QString tmp = tips[i]->getText();
      ret += tmp;
    }
  }

  return ret;
}

void XWTikzArrowSpecification::scan(const QString & str, int & len, int & pos)
{
  while (str[pos].isSpace())
    pos++;

  if (str[pos] == QChar('['))
  {
    pos++;
    while (pos < len)
    {
      if (str[pos] == QChar(']'))
      {
        pos++;
        break;
      }

      XWTikzOperation * op = XWTikzArrowTipSpecification::scanOption(str,len,pos,this);
      if (op)
        ops << op;
    }

    while (str[pos].isSpace())
      pos++;
  }

  if (str[pos] == QChar('{'))
  {
    pos++;
    while (str[pos].isSpace())
      pos++;
  }

  while (pos < len)
  {
    if (str[pos] == QChar('-') || str[pos] == QChar(',') || str[pos] == QChar(']'))
      break;

    if (str[pos] == QChar('}'))
    {
      pos++;
      break;
    }

    XWTikzArrowTipSpecification * tip = new XWTikzArrowTipSpecification(graphic,this);
    tips << tip;
    tip->scan(str,len,pos);
  }

  while (str[pos].isSpace())
    pos++;

  if (str[pos] == QChar(','))
    pos++;
}

void XWTikzArrowSpecification::setArrow(int a)
{
  if (tips.size() == 0)
  {
    XWTikzArrowTipSpecification * tip = new XWTikzArrowTipSpecification(graphic,this);
    tips << tip;
  }

  tips[tips.size() - 1]->setArrow(a);
}

void XWTikzArrowSpecification::setup(XWTikzState * state)
{
  for (int i = 0; i < ops.size(); i++)
    ops[i]->doPath(state,false);

  for (int i = 0; i < tips.size(); i++)
    tips[i]->setup(state);
}

XWTikzArrows::XWTikzArrows(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, PGFarrows,parent),
startArrow(0),
endArrow(0)
{}

void XWTikzArrows::doPath(XWTikzState * state, bool)
{
  state->setEndArrow(endArrow);
  state->setStartArrow(startArrow);
}

int XWTikzArrows::getEndArrow()
{
  if (!endArrow)
    return -1;

  return endArrow->getArrow();
}

int XWTikzArrows::getStartArrow()
{
  if (!startArrow)
    return -1;

  return startArrow->getArrow();
}

QString XWTikzArrows::getText()
{
  QString ret;
  if (startArrow && startArrow)
    ret = "arrows=";

  if (startArrow)
  {
    QString tmp = startArrow->getText();
    ret += tmp;
  }

  if (endArrow)
  {
    ret += "-";
    QString tmp = endArrow->getText();
    ret += tmp;
  }

  return ret;
}

void XWTikzArrows::scan(const QString & str, int & len, int & pos)
{
  while (str[pos].isSpace())
    pos++;

  if (str[pos] != QChar('-'))
  {
    startArrow = new XWTikzArrowSpecification(graphic,this);
    startArrow->scan(str,len,pos);

    while (str[pos].isSpace())
      pos++;
  }

  if (str[pos] == QChar('-'))
  {
    pos++;
    endArrow = new XWTikzArrowSpecification(graphic,this);
    endArrow->scan(str,len,pos);
  }

  while (str[pos].isSpace())
    pos++;

  if (str[pos] == QChar(','))
    pos++;
}

void XWTikzArrows::setEndArrow(int a)
{
  if (!endArrow)
    endArrow = new XWTikzArrowSpecification(graphic,this);

  endArrow->setArrow(a);
}

void XWTikzArrows::setStartArrow(int a)
{
  if (!startArrow)
    startArrow = new XWTikzArrowSpecification(graphic,this);

  startArrow->setArrow(a);
}

XWTikzAnnotationArrow::XWTikzAnnotationArrow(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, PGFannotationarrow,parent),
endArrow(0)
{
  endArrow = new XWTikzArrowSpecification(graphic,this);
  endArrow->setArrow(PGFdirectionee);
}

void XWTikzAnnotationArrow::doPath(XWTikzState * state, bool)
{
  state->setAnnotationArrow(endArrow);
}

QString XWTikzAnnotationArrow::getText()
{
  QString ret = "annotation arrow";
  return ret;
}

XWTikzCurrentDirectionArrow::XWTikzCurrentDirectionArrow(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, PGFcurrentdirectionarrow,parent),
endArrow(0)
{
  endArrow = new XWTikzArrowSpecification(graphic,this);
  endArrow->setArrow(PGFdirectionee);
}

void XWTikzCurrentDirectionArrow::doPath(XWTikzState * state, bool)
{
  state->setCurrentDirectionArrow(endArrow);
}

QString XWTikzCurrentDirectionArrow::getText()
{
  QString ret = "current direction arrow";
  return ret;
}

XWTikzAround::XWTikzAround(XWTikzGraphic * graphicA, int idA, QObject * parent)
:XWTikzOperation(graphicA, idA,parent),
expv(0),
coordv(0)
{}

void XWTikzAround::doCompute(XWTikzState * state)
{
  doPath(state);
}

void XWTikzAround::doPath(XWTikzState * state, bool)
{
  switch (keyWord)
  {
    default:
      break;

    case PGFscalearound:
      {
        QPointF d = coordv->getPoint(state);
        state->shift(d.x(),d.y());
        state->scale(expv->getResult(state),expv->getResult(state));
        state->shift(-d.x(),-d.y());
      }
      break;

    case PGFrotatearound:
      {
        QPointF d = coordv->getPoint(state);
        state->shift(d.x(),d.y());
        state->rotate(expv->getResult(state));
        state->shift(-d.x(),-d.y());
      }
      break;
  }
}

void XWTikzAround::dragTo(XWTikzState * state)
{
  doPath(state);
}

bool XWTikzAround::dropTo(XWTikzState * state)
{
  doPath(state);
  return false;
}

QString XWTikzAround::getCoord()
{
  return coordv->getText();
}

QString XWTikzAround::getExpress()
{
  return expv->getText();
}

QString XWTikzAround::getText()
{
  QString ret = getPGFString(keyWord);
  ret += "={";
  QString tmp = expv->getText();
  ret += tmp;
  ret += ":";
  tmp = coordv->getText();
  ret += tmp;
  ret += "}";
  return ret;
}

QString XWTikzAround::getTips(XWTikzState * state)
{
  doPath(state);
  return getText();
}

bool XWTikzAround::hitTest(XWTikzState * state)
{
  doPath(state);
  return false;
}

void XWTikzAround::scan(const QString & str, int & len, int & pos)
{
  while (pos < len && str[pos] != QChar('{'))
    pos++;

  pos++;
  int i = 0;
  while (pos < len && str[pos] != QChar(':'))
    pos++;
  QString v = str.mid(i,pos);
  expv = new XWTikzExpress(graphic,v,this);
  while (pos < len && str[pos] != QChar('('))
    pos++;
  i = pos;
  while (pos < len && str[pos] != QChar(')'))
    pos++;
  pos++;
  v = str.mid(i,pos);
  coordv = new XWTikzCoord(graphic,v,this);
  while (pos < len && str[pos] != QChar('}'))
    pos++;
  pos++;
}

void XWTikzAround::setCoord(const QString & str)
{
  if (!coordv)
    coordv = new XWTikzCoord(graphic,str,this);
  else
    coordv->setText(str);
}

void XWTikzAround::setExpress(const QString & str)
{
  if (!expv)
    expv = new XWTikzExpress(graphic,str,this);
  else
    expv->setText(str);
}

XWTikzcm::XWTikzcm(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, PGFcm,parent),
a(0),
b(0),
c(0),
d(0),
coord(0)
{}

void XWTikzcm::doCompute(XWTikzState * state)
{
  doPath(state);
}

void XWTikzcm::doPath(XWTikzState * state, bool)
{
  QPointF v = coord->getPoint(state);
  state->setMatrix(a->getResult(state),
                    b->getResult(state),
                    c->getResult(state),
                    d->getResult(state),
                    v.x(),v.y());
}

void XWTikzcm::dragTo(XWTikzState * state)
{
  doPath(state);
}

bool XWTikzcm::dropTo(XWTikzState * state)
{
  doPath(state);
  return false;
}

QString XWTikzcm::getA()
{
  return a->getText();
}

QString XWTikzcm::getB()
{
  return b->getText();
}

QString XWTikzcm::getC()
{
  return c->getText();
}

QString XWTikzcm::getCoord()
{
  return coord->getText();
}

QString XWTikzcm::getD()
{
  return d->getText();
}

QString XWTikzcm::getText()
{
  QString ret="cm={";
  QString tmp = a->getText();
  ret += tmp;
  ret += ",";
  tmp = b->getText();
  ret += tmp;
  ret += ",";
  tmp = c->getText();
  ret += tmp;
  ret += ",";
  tmp = d->getText();
  ret += tmp;
  ret += ",";
  tmp = coord->getText();
  ret += tmp;
  ret += "}";
  return ret;
}

QString XWTikzcm::getTips(XWTikzState * state)
{
  doPath(state);
  return getText();
}

bool XWTikzcm::hitTest(XWTikzState * state)
{
  doPath(state);
  return false;
}

void XWTikzcm::scan(const QString & str, int & len, int & pos)
{
  while (str[pos] != QChar('{'))
    pos++;
  pos++;
  QString value;
  scanValue(str,len,pos,value);
  a = new XWTikzExpress(graphic,value,this);
  scanValue(str,len,pos,value);
  b = new XWTikzExpress(graphic,value,this);
  scanValue(str,len,pos,value);
  c = new XWTikzExpress(graphic,value,this);
  scanValue(str,len,pos,value);
  d = new XWTikzExpress(graphic,value,this);
  while (str[pos] != QChar('('))
    pos++;
  int i = pos;
  while (str[pos] != QChar(')'))
    pos++;
  pos++;
  value = str.mid(i,pos);
  coord = new XWTikzCoord(graphic,value,this);
  while (str[pos] != QChar('}'))
    pos++;
  pos++;
}

void XWTikzcm::setA(const QString & str)
{
  if (!a)
    a = new XWTikzExpress(graphic,this);
  a->setText(str);
}

void XWTikzcm::setB(const QString & str)
{
  if (!b)
    b = new XWTikzExpress(graphic,this);
  b->setText(str);
}

void XWTikzcm::setC(const QString & str)
{
  if (!c)
    c = new XWTikzExpress(graphic,this);
  c->setText(str);
}

void XWTikzcm::setCoord(const QString & str)
{
  if (!coord)
    coord = new XWTikzCoord(graphic,this);
  coord->setText(str);
}

void XWTikzcm::setD(const QString & str)
{
  if (!d)
    d = new XWTikzExpress(graphic,this);
  d->setText(str);
}

XWTikzOf::XWTikzOf(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, PGFof,parent)
{}

void XWTikzOf::doPath(XWTikzState * , bool)
{
  graphic->getIntersections(path1,path2);
}

QString XWTikzOf::getText()
{
  QString ret = QString("of=%1 and %2").arg(path1).arg(path2);
  return ret;
}

void XWTikzOf::scan(const QString & str, int & len, int & pos)
{
  QString paths;
  scanValue(str,len,pos,paths);
  int i = paths.indexOf(" and ");
  path1 = paths.mid(0,i);
  i += 5;
  path2 = paths.mid(i,-1);
}

XWTikzBy::XWTikzBy(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, PGFby,parent)
{}

void XWTikzBy::doPath(XWTikzState * , bool)
{
  graphic->setBy(names);
}

QString XWTikzBy::getText()
{
  QString tmp = names.join(",");
  QString ret = QString("by={%1}").arg(tmp);
  return ret;
}

void XWTikzBy::scan(const QString & str, int & len, int & pos)
{
  while (pos < len && str[pos].isSpace())
    pos++;

  if (str[pos] == QChar('{'))
    pos++;

  int i = pos;
  while (pos < len && str[pos] != QChar('}'))
    pos++;

  QString tmp = str.mid(i,pos);
  names = tmp.split(QChar(','));
  pos++;
  while (pos < len && str[pos].isSpace())
    pos++;
  if (str[pos] == QChar(','))
    pos++;
}

XWTikzList::XWTikzList(XWTikzGraphic * graphicA, int idA, QObject * parent)
:XWTikzOperation(graphicA, idA,parent)
{}

void XWTikzList::doPath(XWTikzState * state, bool)
{
  switch (keyWord)
  {
    default:
      break;

    case PGFcolumnsep:
      {
        QString str = list[0];
        XWTikzExpress exp(graphic,str);
        double v = exp.getResult(state);
        state->setColumnSep(v);
      }
      break;

    case PGFrowsep:
      {
        QString str = list[0];
        XWTikzExpress exp(graphic,str);
        double v = exp.getResult(state);
        state->setRowSep(v);
      }
      break;

    case PGFrectanglesplitpartalign:
      {
        QList<int> l;
        for (int i = 0; i < list.size(); i++)
        {
          int k = lookupPGFID(list[i]);
          l << k;
        }

        state->setRectangleSplitPartAlign(l);
      }
      break;

    case PGFrectanglesplitpartfill:
      {
        QList<QColor> l;
        for (int i = 0; i < list.size(); i++)
        {
          int pos = 0;
          int len = list[i].length();
          XWTikzColor c(graphic,-1);
          c.scan(list[i],len,pos);

          QColor qc = c.getColor();
          l << qc;
        }

        state->setRectangleSplitPartFill(l);
      }
      break;

    case PGFarrowboxarrows:
      {
        state->resetArrowBoxArrows();
        for (int i = 0; i < list.size(); i++)
        {
          int k = list[i].indexOf(":");
          QString key = list[i].left(k);
          QString value = list[i].mid(k + 1);
          k = lookupPGFID(key);
          int id = 0;
          switch (k)
          {
            default:
              break;

            case PGFnorth:
              id = PGFarrowboxnortharrow;
              break;

            case PGFsouth:
              id = PGFarrowboxsoutharrow;
              break;

            case PGFeast:
              id = PGFarrowboxeastarrow;
              break;

            case PGFwest:
              id = PGFarrowboxwestarrow;
              break;
          }

          XWTikzValue v(graphic,id,this);
          int pos = 0;
          int len = value.length();
          v.scan(value,len,pos);
          v.doPath(state);
        }
      }
      break;

    case PGFchamferedrectanglecorners:
      {
        QList<int> cs;
        for (int i = 0; i < list.size(); i++)
          cs << lookupPGFID(list[i]);
        state->setChamferedRectangleCorners(cs);
      }
      break;

    case PGFstructuredtokens:
      {
        state->setChildrenNumber(list.size());
        for (int i = 0; i < list.size(); i++)
        {
          XWTikzTextBox * box = new XWTikzTextBox(graphic,state);
          int len = list[i].length();
          int pos = 0;
          box->scan(list[i], len, pos);
          XWTikzState * newstate = state->saveNode(box,XW_TIKZ_NODE);
          newstate->setFillColor(Qt::black);
          newstate->setToken(i + 1);
        }
      }
      break;
  }
}

QString XWTikzList::getText()
{
  QString tmp = list.join(",");
  QString key = getPGFString(getKeyWord());
  QString ret = QString("%1={%2}").arg(key).arg(tmp);
  return ret;
}

void XWTikzList::scan(const QString & str, int & len, int & pos)
{
  while (pos < len && str[pos].isSpace())
    pos++;

  if (str[pos] == QChar('{'))
  {
    pos++;

    int i = pos;
    while (pos < len && str[pos] != QChar('}'))
      pos++;

    QString tmp = str.mid(i,pos);
    list = tmp.split(QChar(','));
    pos++;

    while (pos < len && str[pos].isSpace())
      pos++;
  }
  else
  {
    int i = pos;
    while (pos < len && str[pos] != QChar(']') && str[pos] != QChar('}'))
      pos++;

    QString tmp = str.mid(i,pos);
    list << tmp;
  }
  
  if (str[pos] == QChar(','))
    pos++;
}

XWTikzAndValue::XWTikzAndValue(XWTikzGraphic * graphicA, int idA, QObject * parent)
:XWTikzOperation(graphicA, idA,parent),
 value1(-1),
 value2(-1)
{}

void XWTikzAndValue::doPath(XWTikzState * state, bool )
{
  switch (keyWord)
  {
    default:
      break;

    case PGFsignalfrom:
      state->setSignalFrom(value1);
      state->setSignalFromOpposite(value2);
      break;

    case PGFsignalto:
      state->setSignalTo(value1);
      state->setSignalToOpposite(value2);
      break;
  }
}

QString XWTikzAndValue::getText()
{
  QString key = getPGFString(getKeyWord());
  QString ret = QString("%1=%2").arg(key).arg(value1);
  if (value2 > 0)
    ret = QString("%1 and %2").arg(ret).arg(value2);
  return ret;
}

void XWTikzAndValue::scan(const QString & str, int & len, int & pos)
{
  while (pos < len && str[pos].isSpace())
    pos++;

  int i = pos;
  while (!str[pos].isSpace())
    pos++;

  QString key = str.mid(i,pos);
  value1 = lookupPGFID(key);

  while (str[pos].isSpace())
    pos++;

  if (str[pos] == QChar('a'))
  {
    pos += 4;
    while (str[pos].isSpace())
      pos++;
    i = pos;
    while ((pos < len) && 
         str[pos] != QChar(',') && 
         str[pos] != QChar(']') && 
         str[pos] != QChar('}'))
      pos++;
    key = str.mid(i,pos);
    value2 = lookupPGFID(key);

    while (str[pos].isSpace())
      pos++;
  }
  if (str[pos] == QChar(','))
    pos++;
}

XWTikzThreePoint::XWTikzThreePoint(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, PGFgrowviathreepoints,parent),
one(0),
two(0),
three(0)
{}

void XWTikzThreePoint::doCompute(XWTikzState * state)
{
  doPath(state,false);
}

void XWTikzThreePoint::doPath(XWTikzState * state, bool )
{
  QPointF p1;
  if (one)
    p1 = one->getPoint(state);

  QPointF p2;
  if (two)
    p2 = two->getPoint(state);

  QPointF p3;
  if (three)
    p3 = three->getPoint(state);

  state->growViaThreePoints(p1,p2,p3);
}

QString XWTikzThreePoint::getText()
{
  QString p1("(0,1)");
  if (one)
    p1 = one->getText();

  QString p2("(1,0)");
  if (two)
    p2 = two->getText();

  QString p3("(1,0)");
  if (three)
    p3 = three->getText();

  QString ret = QString("grow via three points={one child at %1 and two children at %2 and %3}").arg(p1).arg(p2).arg(p3);
  return ret;
}

void XWTikzThreePoint::scan(const QString & str, int & , int & pos)
{
  pos = str.indexOf("(", pos + 13);
  int i = pos;
  pos = str.indexOf(")", pos + 1);
  QString p1 = str.mid(i, pos - i);
  one = new XWTikzCoord(graphic,p1,this);

  pos = str.indexOf("(", pos + 1);
  i = pos;
  pos = str.indexOf(")", pos + 1);
  QString p2 = str.mid(i, pos - i);
  two = new XWTikzCoord(graphic,p2,this);

  pos = str.indexOf("(", pos + 1);
  i = pos;
  pos = str.indexOf(")", pos + 1);
  QString p3 = str.mid(i, pos - i);
  three = new XWTikzCoord(graphic,p3,this);

  pos = str.indexOf("}", pos + 1);
  pos++;
  while (str[pos].isSpace())
    pos++;

  if (str[pos] == QChar(','))
    pos++;
}

XWTikzCodes::XWTikzCodes(XWTikzGraphic * graphicA, int id, QObject * parent)
:XWTikzOperation(graphicA, id,parent),
cur(-1)
{}

void XWTikzCodes::doPath(XWTikzState * state, bool showpoint)
{
  for (int i = 0; i < cmds.size(); i++)
    cmds[i]->doPath(state,showpoint);
}

QPointF XWTikzCodes::getPoint(XWTikzState * stateA)
{
  if (cur >= 0 && cur < cmds.size())
    return cmds[cur]->getPoint(stateA);
  return QPointF();
}

bool XWTikzCodes::isMe(const QString & nameA,XWTikzState * state)
{
  cur = -1;
  for (int i = 0; i < cmds.size(); i++)
  {
    if (cmds[i]->isMe(nameA,state))
    {
      cur = i;
      return true;
    }
  }

  return false;
}

void XWTikzCodes::scanCommands(const QString & str, int & len, int & pos)
{
  while (str[pos].isSpace())
    pos++;
  pos++;
  while (pos < len)
  {
    if (str[pos] == QChar('}'))
    {
      pos++;
      break;
    }
    else if (str[pos].isSpace())
      pos++;
    else if (str[pos] == QChar('%'))
      XWTeXBox::skipComment(str,len,pos);
    else
    {
      if (str[pos] == QChar('\\'))
      {
        QString key = XWTeXBox::scanControlSequence(str,len,pos);
        int id = lookupPGFID(key);
        XWTikzCommand * cmd = createPGFObject(graphic,0,id,this);
        cmds << cmd;
        cmd->scan(str,len,pos);
      }
      else
        pos++;
    }
  }
  while (str[pos].isSpace())
    pos++;
  if (str[pos] == QChar(','))
    pos++;
}

XWTikzAtPositionWidth::XWTikzAtPositionWidth(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzCodes(graphicA,PGFatpositionwidth,parent),
postion(0)
{}

void XWTikzAtPositionWidth::doPath(XWTikzState * state, bool )
{
  state->setStartPosition(0);
  state->setEndPosition(0);
  state->setAtPosition(postion);
  state->setCodes(cmds);
}

QString XWTikzAtPositionWidth::getText()
{
  QString p = postion->getText();
  QString code;
  for (int i = 0; i < cmds.size(); i++)
  {
    if (i > 0 && i < cmds.size() - 1)
      code += ";\n";
    QString tmp = cmds[i]->getText();
    code += tmp;
  }

  QString ret = QString("at position %1 width {%2}").arg(p).arg(code);
  return ret;
}

void XWTikzAtPositionWidth::scan(const QString & str, int & len, int & pos)
{
  pos += 2;
  while (str[pos].isSpace())
    pos++;
  pos += 8;
  int i = pos;
  while (str[pos] != QChar('w'))
    pos++;
  QString p = str.mid(i, pos - i);
  p = p.simplified();
  postion = new XWTikzExpress(graphic,p,this);

  pos += 4;
  scanCommands(str,len,pos);
}


XWTikzBetweenPositionsWidth::XWTikzBetweenPositionsWidth(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzCodes(graphicA,PGFbetweenpositionsandstepwidth,parent),
startPos(0),
endPos(0),
step(0)
{}

void XWTikzBetweenPositionsWidth::doPath(XWTikzState * state, bool )
{
  state->setAtPosition(0);
  state->setStartPosition(startPos);
  state->setEndPosition(endPos);
}

QString XWTikzBetweenPositionsWidth::getText()
{
  QString sp = startPos->getText();
  QString ep = endPos->getText();
  QString t = step->getText();
  QString code;
  for (int i = 0; i < cmds.size(); i++)
  {
    if (i > 0 && i < cmds.size() - 1)
      code += ";\n";
    QString tmp = cmds[i]->getText();
    code += tmp;
  }

  QString ret = QString("between positions %1 and %2 step %3 width {%4}").arg(sp).arg(ep).arg(t).arg(code);
  return ret;
}

void XWTikzBetweenPositionsWidth::scan(const QString & str, int & len, int & pos)
{
  pos += 7;
  while (str[pos].isSpace())
    pos++;
  pos += 9;
  int i = pos;
  while (str[pos] != QChar('a'))
    pos++;
  QString p = str.mid(i, pos - i);
  p = p.simplified();
  startPos = new XWTikzExpress(graphic,p,this);

  pos += 3;
  i = pos;
  while (str[pos] != QChar('s'))
    pos++;
  p = str.mid(i, pos - i);
  p = p.simplified();
  endPos = new XWTikzExpress(graphic,p,this);

  pos += 4;
  i = pos;
  while (str[pos] != QChar('w'))
    pos++;
  p = str.mid(i, pos - i);
  p = p.simplified();
  step = new XWTikzExpress(graphic,p,this);

  pos += 4;
  scanCommands(str,len,pos);
}

XWTikzMark::XWTikzMark(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, PGFmark,parent),
mark(0)
{}

void XWTikzMark::doPath(XWTikzState * state, bool )
{
  state->addCode(mark);
}

QString XWTikzMark::getText()
{
  QString tmp = mark->getText();
  QString ret = QString("mark=%1").arg(tmp);
  return ret;
}

bool XWTikzMark::isMe(const QString & nameA,XWTikzState * state)
{
  return mark->isMe(nameA,state);
}

void XWTikzMark::scan(const QString & str, int & len, int & pos)
{
  while (str[pos].isSpace())
    pos++;

  if (str[pos] == QChar('a'))
    mark = new XWTikzAtPositionWidth(graphic,this);
  else
    mark = new XWTikzBetweenPositionsWidth(graphic,this);
  mark->scan(str,len,pos);
}

XWTikzSize::XWTikzSize(XWTikzGraphic * graphicA, int id, QObject * parent)
:XWTikzOperation(graphicA, id,parent),
width(0),
height(0)
{}

void XWTikzSize::doPath(XWTikzState * state, bool)
{
  switch (keyWord)
  {
    default:
      break;

    case PGFcircuitsymbolsize:
      state->setCircuitSymbolSize(width->getResult(state),height->getResult(state));
      break;
  }
}

void XWTikzSize::getSize(QString & w, QString & h)
{
  w = width->getText();
  h = height->getText();
}

QString XWTikzSize::getText()
{
  QString ret = getPGFString(keyWord);
  ret += " width ";
  QString tmp = width->getText();
  ret += tmp;
  ret += " height ";
  tmp = height->getText();
  ret += tmp;
  return ret;
}

void XWTikzSize::scan(const QString & str, int & , int & pos)
{
  while (str[pos].isSpace())
    pos++;
  if (str[pos] == QChar('w'))
    pos += 5;

  int i = pos;
  while (str[pos] != QChar('h'))
    pos++;

  QString w = str.mid(i, pos - i);
  width = new XWTikzExpress(graphic,w,this);

  if (str[pos] == QChar('h'))
    pos += 6;

  i = pos;
  while (str[pos] != QChar(',') && str[pos] != QChar(']') && str[pos] != QChar('}'))
    pos++;
  
  QString h = str.mid(i, pos - i);
  height = new XWTikzExpress(graphic,h,this);
}

void XWTikzSize::setSize(const QString & w, const QString & h)
{
  if (!width)
    width = new XWTikzExpress(graphic,this);
  width->setText(w);
  if (!height)
    height = new XWTikzExpress(graphic,this);
  height->setText(h);
}

 XWTikzCircuitDeclareUnit::XWTikzCircuitDeclareUnit(XWTikzGraphic * graphicA, QObject * parent)
 :XWTikzOperation(graphicA, PGFcircuitdeclareunit,parent)
 {}

QString XWTikzCircuitDeclareUnit::getText()
{
  QString ret = QString("circuit declare unit={%1}{%2}").arg(name).arg(unit);
  return ret;
}

void XWTikzCircuitDeclareUnit::scan(const QString & str, int & , int & pos)
{
  while (str[pos].isSpace())
    pos++;

  pos++;
  int i = pos;
  while (str[pos] != QChar('}'))
    pos++;

  name = str.mid(i, pos - i);
  name = name.simplified();
  pos++;
  while (str[pos].isSpace())
    pos++;
  pos++;  
  i = pos;
  while (str[pos] != QChar('}'))
    pos++;
  unit = str.mid(i, pos - i);
  unit = unit.simplified();
  pos++;

  graphic->setUnit(name,unit);
  QString tmp = QString("%1'").arg(name);
  graphic->setUnit(tmp,unit);
  tmp = QString("%1 sloped").arg(name);
  graphic->setUnit(tmp,unit);
  tmp = QString("%1' sloped").arg(name);
  graphic->setUnit(tmp,unit);
}

XWTikzInpus::XWTikzInpus(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, PGFinputs,parent)
{}

void XWTikzInpus::append(const QChar & c)
{
  inputs << c;
}

void XWTikzInpus::doPath(XWTikzState * state, bool)
{
  state->setInputs(inputs);
}

QString XWTikzInpus::getText()
{
  QString ret = "inpus={";
  for (int i= 0; i < inputs.size(); i++)
    ret.append(inputs[i]);
  ret += "}";

  return ret;
}

QChar XWTikzInpus::remove()
{
  return inputs.takeLast();
}

void XWTikzInpus::scan(const QString & str, int & len, int & pos)
{
  while (str[pos].isSpace())
    pos++;

  if (str[pos] == QChar('{'))
    pos++;

  while (pos < len)
  {
    if (str[pos] == QChar('}'))
    {
      pos++;
      break;
    }
    else if (str[pos].isSpace())
      pos++;
    else
    {
      inputs << str[pos];
      pos++;
    }
  }
}

int  XWTikzInpus::size()
{
  return inputs.size();
}

XWTikzLogicGateInpus::XWTikzLogicGateInpus(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, PGFlogicgateinputs,parent)
{}

void XWTikzLogicGateInpus::doPath(XWTikzState * state, bool)
{
  state->setInputs(inputs);
}

QString XWTikzLogicGateInpus::getText()
{
  QString ret = "logic gate inputs={";
  for (int i= 0; i < inputs.size(); i++)
  {
    if (inputs[i] == QChar('i'))
      ret += "inverted";
    else
      ret += "normal";

    if (i < inputs.size() - 1)
      ret += ",";
  }

  ret += "}";

  return ret;
}

void XWTikzLogicGateInpus::scan(const QString & str, int & len, int & pos)
{
  while (str[pos].isSpace())
    pos++;

  if (str[pos] == QChar('{'))
    pos++;

  while (pos < len)
  {
    if (str[pos] == QChar('}'))
    {
      pos++;
      break;
    }
    else if (str[pos].isSpace())
      pos++;
    else if (str[pos] == QChar(','))
      pos++;
    else
    {
      if (str[pos] == QChar('n'))
      {
        inputs << QChar('n');
        pos += 6;
      }
      else
      {
        inputs << QChar('i');
        pos += 8;
      }
    }
  }
}

XWTikzControls::XWTikzControls(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, PGFcontrols,parent),
 in(0),
 out(0)
{
  in = new XWTikzCoord(graphic,this);
  out = new XWTikzCoord(graphic,this);
}

void XWTikzControls::doPath(XWTikzState * state, bool )
{
  QPointF p = in->getPoint(state);
  state->setInControl(p);
  p = out->getPoint(state);
  state->setOutControl(p);
}

QString XWTikzControls::getText()
{
  QString ret = "controls=";
  QString tmp = in->getText();
  ret += tmp;
  ret += " and ";
  tmp = out->getText();
  ret += tmp;
  return ret;
}

void XWTikzControls::scan(const QString & str, int & len, int & pos)
{
  in->scan(str, len, pos);
  while (str[pos].isSpace())
    pos++;

  pos += 3;
  out->scan(str, len, pos);
}

XWTikzShapeSep::XWTikzShapeSep(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, PGFshapesep,parent),
 sep(0),
 between(PGFbetweencenters)
{}

void XWTikzShapeSep::doPath(XWTikzState * state, bool showpoint)
{
  state->setShapeSep(sep->getResult(state));
  state->setBetweenOrBy(between);
}

QString XWTikzShapeSep::getText()
{
  QString ret = "shape sep = {";
  QString tmp = sep->getText();
  ret += tmp;
  ret += ",";
  tmp = getPGFString(between);
  ret += tmp;
  ret += "}";
  return ret;
}

void XWTikzShapeSep::scan(const QString & str, int & len, int & pos)
{
  while (str[pos].isSpace())
    pos++;
  if (str[pos] == QChar('{'))
    pos++;
  QString v;
  scanValue(str, len, pos, v);
  sep = new XWTikzExpress(graphic,v,this);
  v.clear();
  scanValue(str, len, pos, v);
  if (!v.isEmpty())
    between = lookupPGFID(v);
}

XWTikzShapeEvenlySpread::XWTikzShapeEvenlySpread(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, PGFshapeevenlyspread,parent),
spread(0),
by(PGFbycenters)
{}

void XWTikzShapeEvenlySpread::doPath(XWTikzState * state, bool)
{
  state->setShapeEvenlySpread(spread);
  state->setBetweenOrBy(by);
}

QString XWTikzShapeEvenlySpread::getText()
{
  QString tmp = getPGFString(by);
  QString ret = QString("shape evenly spread={%1,%2}").arg(spread).arg(tmp);
  return ret;
}

void XWTikzShapeEvenlySpread::scan(const QString & str, int & len, int & pos)
{
  while (str[pos].isSpace())
    pos++;
  if (str[pos] == QChar('{'))
    pos++;
  QString v;
  scanValue(str, len, pos, v);
  spread = v.toInt();
  scanValue(str, len, pos, v);
  if (!v.isEmpty())
    by = lookupPGFID(v);
}
