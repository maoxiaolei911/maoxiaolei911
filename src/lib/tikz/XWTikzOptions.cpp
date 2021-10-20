/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <math.h>
#include <QRegExp>
#include <QAction>
#include "XWApplication.h"
#include "PGFKeyWord.h"
#include "XWPGFPool.h"
#include "tikzcolor.h"
#include "XWTikzCoord.h"
#include "XWTikzGraphic.h"
#include "XWTikzState.h"
#include "XWTikzOpacityDialog.h"
#include "XWTikzOptionDialog.h"
#include "XWTikzShapeDialog.h"
#include "XWTikzLineDialog.h"
#include "XWTikzArrowDialog.h"
#include "XWTikzPatternDialog.h"
#include "XWTikzShadeDialog.h"
#include "XWTikzShadowDialog.h"
#include "XWTikzFadingDialog.h"
#include "XWTikzOperationDialog.h"
#include "XWTikzPlotDialog.h"
#include "XWTikzDecorationDialog.h"
#include "XWTikzOption.h"
#include "XWTikzNode.h"
#include "XWTikzTextBox.h"
#include "XWTikzUndoCommand.h"
#include "XWTikzOptions.h"

XWTIKZOptions::XWTIKZOptions(XWTikzGraphic * graphicA, QObject * parent)
 :XWTikzOperation(graphicA, XW_TIKZ_OPTIONS,parent),
  cur(-1)
 {}

XWTIKZOptions::XWTIKZOptions(XWTikzGraphic * graphicA, int idA,QObject * parent)
 :XWTikzOperation(graphicA, idA,parent),
  cur(-1)
 {}

 XWTIKZOptions::XWTIKZOptions(XWTikzGraphic * graphicA, 
                int idA, 
                int subk,
                QObject * parent)
:XWTikzOperation(graphicA, idA,subk,parent),
  cur(-1)
{}

bool XWTIKZOptions::addAction(QMenu & menu, XWTikzState * state)
{
  bool ret = true;
  switch (state->getPictureType())
  {
    default:
      ret = false;
      break;

    case PGFcircuiteeIEC:
      addCircuiteeSymbolAction(menu);
      menu.addSeparator();
      addCircuiteeUnitAction(menu);
      menu.addSeparator();
      addInfoAction(menu);
      menu.addSeparator();
      addPointAction(menu);
      menu.addSeparator();
      addColorAction(menu);
      menu.addSeparator();
      addLineWidthAction(menu);      
      break;

    case PGFcircuitlogicIEC:
    case PGFcircuitlogicUS:
    case PGFcircuitlogicCDH:
      addCircuitLogicSymbolAction(menu);
      menu.addSeparator();
      addInputAction(menu);
      menu.addSeparator();
      addInfoAction(menu);
      menu.addSeparator();
      addPointAction(menu);
      menu.addSeparator();
      addColorAction(menu);
      menu.addSeparator();
      addLineWidthAction(menu);
      break;
  }
  return ret;
}

void XWTIKZOptions::addAnchorAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("anchor"));
  connect(a, SIGNAL(triggered()), this, SLOT(setAnchor()));
  if (xwApp->getLicenseState() == APP_STATE_NORMAL)
  {
    a = menu.addAction(tr("pos"));
    connect(a, SIGNAL(triggered()), this, SLOT(setPos()));
  }  
}

void XWTIKZOptions::addAngleAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("start angle"));
  connect(a, SIGNAL(triggered()), this, SLOT(setStartAngle()));
  a = menu.addAction(tr("end angle"));
  connect(a, SIGNAL(triggered()), this, SLOT(setEndAngle()));
}

void XWTIKZOptions::addArrowsAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("default arrow"));
  connect(a, SIGNAL(triggered()), this, SLOT(setDefaultArrow()));
  a = menu.addAction(tr("arrows"));
  connect(a, SIGNAL(triggered()), this, SLOT(setArrows()));
}

void XWTIKZOptions::addAtAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("at"));
  connect(a, SIGNAL(triggered()), this, SLOT(setAt()));
}

void XWTIKZOptions::addCircuitAction(QMenu & menu)
{
  XWTikzKey * key = findPictureType();
  if (!key)
  {
    QAction * a = menu.addAction(tr("circuit ee IEC"));
    connect(a, SIGNAL(triggered()), this, SLOT(addCircuiteeIEC()));
    a = menu.addAction(tr("circuit logic US"));
    connect(a, SIGNAL(triggered()), this, SLOT(addCircuitLogicUS()));
    a = menu.addAction(tr("circuit logic CDH"));
    connect(a, SIGNAL(triggered()), this, SLOT(addCircuitLogicCDH()));
    a = menu.addAction(tr("circuit logic IEC"));
    connect(a, SIGNAL(triggered()), this, SLOT(addCircuitLogicIEC()));
  }
  else
  {
    if (key->getKeyWord() != PGFcircuiteeIEC)
    {
      QAction * a = menu.addAction(tr("circuit ee IEC"));
      connect(a, SIGNAL(triggered()), this, SLOT(addCircuiteeIEC()));
    }

    if (key->getKeyWord() != PGFcircuitlogicUS)
    {
      QAction * a = menu.addAction(tr("circuit logic US"));
      connect(a, SIGNAL(triggered()), this, SLOT(addCircuitLogicUS()));
    }

    if (key->getKeyWord() != PGFcircuitlogicCDH)
    {
      QAction * a = menu.addAction(tr("circuit logic CDH"));
      connect(a, SIGNAL(triggered()), this, SLOT(addCircuitLogicCDH()));
    }

    if (key->getKeyWord() != PGFcircuitlogicIEC)
    {
      QAction * a = menu.addAction(tr("circuit logic IEC"));
      connect(a, SIGNAL(triggered()), this, SLOT(addCircuitLogicIEC()));
    }
  }
}

void XWTIKZOptions::addCircuiteeSymbolAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("circuit ee symbol"));
  connect(a, SIGNAL(triggered()), this, SLOT(setCircuiteeSymbol()));
}

void XWTIKZOptions::addCircuiteeUnitAction(QMenu & menu)
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (!op)
    return ;

  int kw = op->getKeyWord();
  QAction * a;
  switch (kw)
  {
    default:
      break;

    case PGFresistor:
    case PGFresistorIEC:
    case PGFohmmeter:
      a = menu.addAction(tr("ohm"));
      connect(a, SIGNAL(triggered()), this, SLOT(setohm()));
      a = menu.addAction(tr("ohm'"));
      connect(a, SIGNAL(triggered()), this, SLOT(setohmOppsite()));
      a = menu.addAction(tr("ohm sloped"));
      connect(a, SIGNAL(triggered()), this, SLOT(setohmSloped()));
      a = menu.addAction(tr("ohm' sloped"));
      connect(a, SIGNAL(triggered()), this, SLOT(setohmSlopedOppsite()));
      if (xwApp->getLicenseState() == APP_STATE_NORMAL)
      {
        a = menu.addAction(tr("siemens"));
        connect(a, SIGNAL(triggered()), this, SLOT(setSiemens()));
        a = menu.addAction(tr("siemens'"));
        connect(a, SIGNAL(triggered()), this, SLOT(setSiemensOppsite()));
        a = menu.addAction(tr("siemens sloped"));
        connect(a, SIGNAL(triggered()), this, SLOT(setSiemensSloped()));
        a = menu.addAction(tr("siemens' sloped"));
        connect(a, SIGNAL(triggered()), this, SLOT(setSiemensSlopedOppsite()));
      }
      break;

    case PGFinductor:
    case PGFinductorIEC:
      a = menu.addAction(tr("henry"));
      connect(a, SIGNAL(triggered()), this, SLOT(setHenry()));
      a = menu.addAction(tr("henry'"));
      connect(a, SIGNAL(triggered()), this, SLOT(setHenryOppsite()));
      a = menu.addAction(tr("henry sloped"));
      connect(a, SIGNAL(triggered()), this, SLOT(setHenrySloped()));
      a = menu.addAction(tr("henry' sloped"));
      connect(a, SIGNAL(triggered()), this, SLOT(setHenrySlopedOppsite()));
      break;

    case PGFcapacitor:
    case PGFcapacitorIEC:
      a = menu.addAction(tr("farad"));
      connect(a, SIGNAL(triggered()), this, SLOT(setFarad()));
      a = menu.addAction(tr("farad'"));
      connect(a, SIGNAL(triggered()), this, SLOT(setFaradOppsite()));
      a = menu.addAction(tr("farad sloped"));
      connect(a, SIGNAL(triggered()), this, SLOT(setFaradSloped()));
      a = menu.addAction(tr("farad' sloped"));
      connect(a, SIGNAL(triggered()), this, SLOT(setFaradSlopedOppsite()));
      if (xwApp->getLicenseState() == APP_STATE_NORMAL)
      {
        a = menu.addAction(tr("coulomb"));
        connect(a, SIGNAL(triggered()), this, SLOT(setCoulomb()));
        a = menu.addAction(tr("coulomb'"));
        connect(a, SIGNAL(triggered()), this, SLOT(setCoulombOppsite()));
        a = menu.addAction(tr("coulomb sloped"));
        connect(a, SIGNAL(triggered()), this, SLOT(setCoulombSloped()));
        a = menu.addAction(tr("coulomb' sloped"));
        connect(a, SIGNAL(triggered()), this, SLOT(setCoulombSlopedOppsite()));
      }      
      break;

    case PGFbattery:
    case PGFbatteryIEC:
    case PGFvoltagesource:
    case PGFvoltagesourceIEC:
    case PGFvoltmeter:
    case PGFdcsource:
      a = menu.addAction(tr("volt"));
      connect(a, SIGNAL(triggered()), this, SLOT(setVolt()));
      a = menu.addAction(tr("volt'"));
      connect(a, SIGNAL(triggered()), this, SLOT(setVoltOppsite()));
      a = menu.addAction(tr("volt sloped"));
      connect(a, SIGNAL(triggered()), this, SLOT(setVoltSloped()));
      a = menu.addAction(tr("volt' sloped"));
      connect(a, SIGNAL(triggered()), this, SLOT(setVoltSlopedOppsite()));
      if (xwApp->getLicenseState() == APP_STATE_NORMAL)
      {
        a = menu.addAction(tr("voltampere"));
        connect(a, SIGNAL(triggered()), this, SLOT(setVoltAmpere()));
        a = menu.addAction(tr("voltampere'"));
        connect(a, SIGNAL(triggered()), this, SLOT(setVoltAmpereOppsite()));
        a = menu.addAction(tr("voltampere sloped"));
        connect(a, SIGNAL(triggered()), this, SLOT(setVoltAmpereSloped()));
        a = menu.addAction(tr("voltampere' sloped"));
        connect(a, SIGNAL(triggered()), this, SLOT(setVoltAmpereSlopedOppsite()));
      }      
      break;

    case PGFbulb:
    case PGFbulbIEC:
      a = menu.addAction(tr("watt"));
      connect(a, SIGNAL(triggered()), this, SLOT(setWatt()));
      a = menu.addAction(tr("watt'"));
      connect(a, SIGNAL(triggered()), this, SLOT(setWattOppsite()));
      a = menu.addAction(tr("watt sloped"));
      connect(a, SIGNAL(triggered()), this, SLOT(setWattSloped()));
      a = menu.addAction(tr("watt' sloped"));
      connect(a, SIGNAL(triggered()), this, SLOT(setWattSlopedOppsite()));
      break;

    case PGFcurrentsource:
    case PGFcurrentsourceIEC:
    case PGFamperemeter:
      a = menu.addAction(tr("ampere"));
      connect(a, SIGNAL(triggered()), this, SLOT(setAmpere()));
      a = menu.addAction(tr("ampere'"));
      connect(a, SIGNAL(triggered()), this, SLOT(setAmpereOppsite()));
      a = menu.addAction(tr("ampere sloped"));
      connect(a, SIGNAL(triggered()), this, SLOT(setAmpereSloped()));
      a = menu.addAction(tr("ampere' sloped"));
      connect(a, SIGNAL(triggered()), this, SLOT(setAmpereSlopedOppsite()));
      break;

    case PGFacsource:
      a = menu.addAction(tr("hertz"));
      connect(a, SIGNAL(triggered()), this, SLOT(setHertz()));
      a = menu.addAction(tr("hertz'"));
      connect(a, SIGNAL(triggered()), this, SLOT(setHertzOppsite()));
      a = menu.addAction(tr("hertz sloped"));
      connect(a, SIGNAL(triggered()), this, SLOT(setHertzSloped()));
      a = menu.addAction(tr("hertz' sloped"));
      connect(a, SIGNAL(triggered()), this, SLOT(setHertzSlopedOppsite()));
      a = menu.addAction(tr("volt"));
      connect(a, SIGNAL(triggered()), this, SLOT(setVolt()));
      a = menu.addAction(tr("volt'"));
      connect(a, SIGNAL(triggered()), this, SLOT(setVoltOppsite()));
      a = menu.addAction(tr("volt sloped"));
      connect(a, SIGNAL(triggered()), this, SLOT(setVoltSloped()));
      a = menu.addAction(tr("volt' sloped"));
      connect(a, SIGNAL(triggered()), this, SLOT(setVoltSlopedOppsite()));      
      break;
  }
}

void XWTIKZOptions::addCircuitLogicSymbolAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("circuit logic symbol"));
  connect(a, SIGNAL(triggered()), this, SLOT(setCircuitLogicSymbol()));
}

void XWTIKZOptions::addCircuitSymbolAction(QMenu & menu)
{
  XWTikzKey * key = findCircuitSymbols();
  if (!key)
  {
    QAction * a = menu.addAction(tr("huge circuit symbols"));
    connect(a, SIGNAL(triggered()), this, SLOT(addHugeCircuitSymbols()));
    a = menu.addAction(tr("large circuit symbols"));
    connect(a, SIGNAL(triggered()), this, SLOT(addLargeCircuitSymbols()));
    a = menu.addAction(tr("medium circuit symbols"));
    connect(a, SIGNAL(triggered()), this, SLOT(addMediumCircuitSymbols()));
    a = menu.addAction(tr("small circuit symbols"));
    connect(a, SIGNAL(triggered()), this, SLOT(addSmallCircuitSymbols()));
    a = menu.addAction(tr("tiny circuit symbols"));
    connect(a, SIGNAL(triggered()), this, SLOT(addTinyCircuitSymbols()));
  }
  else
  {
    if (key->getKeyWord() != PGFhugecircuitsymbols)
    {
      QAction * a = menu.addAction(tr("huge circuit symbols"));
      connect(a, SIGNAL(triggered()), this, SLOT(addHugeCircuitSymbols()));
    }

    if (key->getKeyWord() != PGFlargecircuitsymbols)
    {
      QAction * a = menu.addAction(tr("large circuit symbols"));
      connect(a, SIGNAL(triggered()), this, SLOT(addLargeCircuitSymbols()));
    }

    if (key->getKeyWord() != PGFmediumcircuitsymbols)
    {
      QAction * a = menu.addAction(tr("medium circuit symbols"));
      connect(a, SIGNAL(triggered()), this, SLOT(addMediumCircuitSymbols()));
    }

    if (key->getKeyWord() != PGFsmallcircuitsymbols)
    {
      QAction * a = menu.addAction(tr("small circuit symbols"));
      connect(a, SIGNAL(triggered()), this, SLOT(addSmallCircuitSymbols()));
    }

    if (key->getKeyWord() != PGFtinycircuitsymbols)
    {
      QAction * a = menu.addAction(tr("tiny circuit symbols"));
      connect(a, SIGNAL(triggered()), this, SLOT(addTinyCircuitSymbols()));
    }
  }

  if (xwApp->getLicenseState() == APP_STATE_NORMAL)
  {
    QAction * a = menu.addAction(tr("circuit symbols size"));
    connect(a, SIGNAL(triggered()), this, SLOT(setCircuitSymbolSize()));
  }
}

void XWTIKZOptions::addCMAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("transform matrix"));
  connect(a, SIGNAL(triggered()), this, SLOT(setCM()));
}

void XWTIKZOptions::addColorAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("color"));
  connect(a, SIGNAL(triggered()), this, SLOT(setColor()));
  a = menu.addAction(tr("draw color"));
  connect(a, SIGNAL(triggered()), this, SLOT(setDrawColor()));
  a = menu.addAction(tr("fill color"));
  connect(a, SIGNAL(triggered()), this, SLOT(setFillColor()));
}

void XWTIKZOptions::addConceptAction(QMenu & menu)
{
  XWTikzKey * key = findConcept();
  if (!key)
  {
    QAction * a = menu.addAction(tr("concept"));
    connect(a, SIGNAL(triggered()), this, SLOT(addConcept()));
    a = menu.addAction(tr("extra concept"));
    connect(a, SIGNAL(triggered()), this, SLOT(addExtraConcept()));
  }
  else
  {
    if (key->getKeyWord() != PGFconcept)
    {
      QAction * a = menu.addAction(tr("concept"));
      connect(a, SIGNAL(triggered()), this, SLOT(addConcept()));
    }

    if (key->getKeyWord() != PGFextraconcept)
    {
      QAction * a = menu.addAction(tr("extra concept"));
      connect(a, SIGNAL(triggered()), this, SLOT(addExtraConcept()));
    }
  }
}

void XWTIKZOptions::addConceptColorAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("concept color"));
  connect(a, SIGNAL(triggered()), this, SLOT(setConceptColor()));
}

void XWTIKZOptions::addConnectSpiesAction(QMenu & menu)
{
  XWTikzOperation * op = find(PGFconnectspies);
  if (op)
    return ;

  QAction * a = menu.addAction(tr("connect spies"));
  connect(a, SIGNAL(triggered()), this, SLOT(setConnectSpies()));
}

void XWTIKZOptions::addDecorationAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("decoration"));
  connect(a, SIGNAL(triggered()), this, SLOT(setDecoration()));

  if (xwApp->getLicenseState() == APP_STATE_NORMAL)
  {
    int decoration = -1;
    XWTikzOperation * op = find(PGFdecoration);
    if (op)
    {
      XWTIKZOptions * opts = (XWTIKZOptions*)(op);
      XWTikzValue * name = opts->getValue(PGFname);
      if (name)
        decoration = (int)(name->getValue());
      else
      {
        XWTikzKey * key = opts->findDecoration();
        if (key)
          decoration = key->getKeyWord();
      }
    }

    a = menu.addAction(tr("raise"));
    connect(a, SIGNAL(triggered()), this, SLOT(setRaise()));

    a = menu.addAction(tr("pre"));
    connect(a, SIGNAL(triggered()), this, SLOT(setPre()));

    a = menu.addAction(tr("pre length"));
    connect(a, SIGNAL(triggered()), this, SLOT(setPrelength()));

    a = menu.addAction(tr("post"));
    connect(a, SIGNAL(triggered()), this, SLOT(setPost()));

    a = menu.addAction(tr("post length"));
    connect(a, SIGNAL(triggered()), this, SLOT(setPostlength()));
  }
}

void XWTIKZOptions::addDomainAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("domain"));
  connect(a, SIGNAL(triggered()), this, SLOT(setDomain()));
}

void XWTIKZOptions::addDoubleAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("double"));
  connect(a, SIGNAL(triggered()), this, SLOT(setDouble()));
}

void XWTIKZOptions::addDoubleDistanceAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("double distance"));
  connect(a, SIGNAL(triggered()), this, SLOT(setDoubleDistance()));
  a = menu.addAction(tr("double distance between line centers"));
  connect(a, SIGNAL(triggered()), this, SLOT(setDoubleDistanceBetweenLineCenters()));
}

void XWTIKZOptions::addEntityRelationshipAction(QMenu & menu)
{
  XWTikzKey * op = findEntityRelationship();
  QAction * a;
  if (!op)
  {
    a = menu.addAction(tr("entity"));
    connect(a, SIGNAL(triggered()), this, SLOT(addEntity()));
    a = menu.addAction(tr("relationship"));
    connect(a, SIGNAL(triggered()), this, SLOT(addRelationship()));
    a = menu.addAction(tr("attribute"));
    connect(a, SIGNAL(triggered()), this, SLOT(addAttribute()));
  }
  else
  {
    if (op->getKeyWord() != PGFentity)
    {
      a = menu.addAction(tr("entity"));
      connect(a, SIGNAL(triggered()), this, SLOT(addEntity()));
    }

    if (op->getKeyWord() != PGFrelationship)
    {
      a = menu.addAction(tr("relationship"));
      connect(a, SIGNAL(triggered()), this, SLOT(addRelationship()));
    }

    if (op->getKeyWord() != PGFattribute)
    {
      a = menu.addAction(tr("attribute"));
      connect(a, SIGNAL(triggered()), this, SLOT(addAttribute()));
    }
  }
}

void XWTIKZOptions::addInfoAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("info"));
  connect(a, SIGNAL(triggered()), this, SLOT(setInfo()));
  a = menu.addAction(tr("info'"));
  connect(a, SIGNAL(triggered()), this, SLOT(setInfoMissingAngle()));
  a = menu.addAction(tr("info sloped"));
  connect(a, SIGNAL(triggered()), this, SLOT(setInfoSloped()));
  a = menu.addAction(tr("info' sloped"));
  connect(a, SIGNAL(triggered()), this, SLOT(setInfoSlopedMissingAngle()));
}

void XWTIKZOptions::addInputAction(QMenu & menu)
{
  XWTikzCircuitSymbol * op = findCircuitLogicSymbol();
  if (!op)
    return ;

  int kw = op->getKeyWord();
  XWTikzInpus * inpus = (XWTikzInpus*)find(PGFinputs);
  switch (kw)
  {
    default:
      {
        QAction * a = menu.addAction(tr("add normal input"));
        connect(a, SIGNAL(triggered()), this, SLOT(addInputNormal()));
        a = menu.addAction(tr("add inverted input"));
        connect(a, SIGNAL(triggered()), this, SLOT(addInputInverted()));
        if (inpus && inpus->size() > 0)
        {
          a = menu.addAction(tr("remove input"));
          connect(a, SIGNAL(triggered()), this, SLOT(removeInput()));
        }
      }
      break;

    case PGFbuffergate:
    case PGFbuffergateUS:
    case PGFbuffergateIEC:
    case PGFnotgate:
    case PGFnotgateUS:
    case PGFnotgateIEC:
      if (!inpus || inpus->size() == 0)
      {
        QAction * a = menu.addAction(tr("add normal input"));
        connect(a, SIGNAL(triggered()), this, SLOT(addInputNormal()));
        a = menu.addAction(tr("add inverted input"));
        connect(a, SIGNAL(triggered()), this, SLOT(addInputInverted()));
      }
      else
      {
        QAction * a = menu.addAction(tr("remove input"));
        connect(a, SIGNAL(triggered()), this, SLOT(removeInput()));
      }
      break;
  }
}

void XWTIKZOptions::addIntersectionsAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("name path"));
  connect(a, SIGNAL(triggered()), this, SLOT(setNamePath()));
  a = menu.addAction(tr("insections of"));
  connect(a, SIGNAL(triggered()), this, SLOT(setOf()));
  a = menu.addAction(tr("sort by"));
  connect(a, SIGNAL(triggered()), this, SLOT(setSortBy()));
}

void XWTIKZOptions::addLabelAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("label"));
  connect(a, SIGNAL(triggered()), this, SLOT(setLabel()));
  a = menu.addAction(tr("pin"));
  connect(a, SIGNAL(triggered()), this, SLOT(setPin()));
}

void XWTIKZOptions::addLineAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("line width"));
  connect(a, SIGNAL(triggered()), this, SLOT(setLineWidthStyle()));
  a = menu.addAction(tr("dash"));
  connect(a, SIGNAL(triggered()), this, SLOT(setDash()));
  a = menu.addAction(tr("line cap"));
  connect(a, SIGNAL(triggered()), this, SLOT(setLineCap()));
  a = menu.addAction(tr("line join"));
  connect(a, SIGNAL(triggered()), this, SLOT(setLineJoin()));
  a = menu.addAction(tr("miter limit"));
  connect(a, SIGNAL(triggered()), this, SLOT(setMiterLimit()));
}

void XWTIKZOptions::addLineWidthAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("line width"));
  connect(a, SIGNAL(triggered()), this, SLOT(setLineWidth()));
}

void XWTIKZOptions::addMinmapAction(QMenu & menu)
{
  XWTikzKey * key = findMindmap();
  if (!key)
  {
    QAction * a = menu.addAction(tr("mindmap"));
    connect(a, SIGNAL(triggered()), this, SLOT(addMindmap()));
    a = menu.addAction(tr("large mindmap"));
    connect(a, SIGNAL(triggered()), this, SLOT(addLargeMindmap()));
    a = menu.addAction(tr("huge mindmap"));
    connect(a, SIGNAL(triggered()), this, SLOT(addHugeMindmap()));
    a = menu.addAction(tr("small mindmap"));
    connect(a, SIGNAL(triggered()), this, SLOT(addSmallMindmap()));
  }
  else
  {
    if (key->getKeyWord() != PGFmindmap)
    {
      QAction * a = menu.addAction(tr("mindmap"));
      connect(a, SIGNAL(triggered()), this, SLOT(addMindmap()));
    }

    if (key->getKeyWord() != PGFlargemindmap)
    {
      QAction * a = menu.addAction(tr("large mindmap"));
      connect(a, SIGNAL(triggered()), this, SLOT(addLargeMindmap()));
    }

    if (key->getKeyWord() != PGFhugemindmap)
    {
      QAction * a = menu.addAction(tr("huge mindmap"));
      connect(a, SIGNAL(triggered()), this, SLOT(addHugeMindmap()));
    }

    if (key->getKeyWord() != PGFsmallmindmap)
    {
      QAction * a = menu.addAction(tr("small mindmap"));
      connect(a, SIGNAL(triggered()), this, SLOT(addSmallMindmap()));
    }
  }
}

void XWTIKZOptions::addOpacityAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("draw opacity"));
  connect(a, SIGNAL(triggered()), this, SLOT(setDrawOpacity()));
  a = menu.addAction(tr("fill opacity"));
  connect(a, SIGNAL(triggered()), this, SLOT(setFillOpacity()));
}

void XWTIKZOptions::addPathFading(QMenu & menu)
{
  QAction * a = menu.addAction(tr("path fading"));
  connect(a, SIGNAL(triggered()), this, SLOT(setPathFading()));
  a = menu.addAction(tr("fading angle"));
  connect(a, SIGNAL(triggered()), this, SLOT(setFadingAngle()));
}

void XWTIKZOptions::addPatternAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("pattern"));
  connect(a, SIGNAL(triggered()), this, SLOT(setPattern()));
  a = menu.addAction(tr("pattern color"));
  connect(a, SIGNAL(triggered()), this, SLOT(setPatternColor()));
}

void XWTIKZOptions::addPlaneAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("canvas is xy plane at z"));
  connect(a, SIGNAL(triggered()), this, SLOT(setPlaneXYAtZ()));
  a = menu.addAction(tr("canvas is yx plane at z"));
  connect(a, SIGNAL(triggered()), this, SLOT(setPlaneYXAtZ()));
  a = menu.addAction(tr("canvas is xz plane at y"));
  connect(a, SIGNAL(triggered()), this, SLOT(setPlaneXZAtY()));
  a = menu.addAction(tr("canvas is zx plane at y"));
  connect(a, SIGNAL(triggered()), this, SLOT(setPlaneZXAtY()));
  a = menu.addAction(tr("canvas is yz plane at x"));
  connect(a, SIGNAL(triggered()), this, SLOT(setPlaneYZAtX()));
  a = menu.addAction(tr("canvas is zy plane at x"));
  connect(a, SIGNAL(triggered()), this, SLOT(setPlaneZYAtX()));
  if (xwApp->getLicenseState() == APP_STATE_NORMAL)
  {
    menu.addSeparator();
    a = menu.addAction(tr("plane x"));
    connect(a, SIGNAL(triggered()), this, SLOT(setPlaneX()));
    a = menu.addAction(tr("plane y"));
    connect(a, SIGNAL(triggered()), this, SLOT(setPlaneY()));
    a = menu.addAction(tr("plane origin"));
    connect(a, SIGNAL(triggered()), this, SLOT(setPlaneOrigin()));
    a = menu.addAction(tr("canvas is plane"));
    connect(a, SIGNAL(triggered()), this, SLOT(setPlane()));
  }
}

void XWTIKZOptions::addPlotAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("plot handler"));
  connect(a, SIGNAL(triggered()), this, SLOT(setPlotHandler()));
  a = menu.addAction(tr("plot mark"));
  connect(a, SIGNAL(triggered()), this, SLOT(setPlotMark()));
}

void XWTIKZOptions::addPointAction(QMenu & menu)
{
  XWTikzKey * key = findPoint();
  if (!key)
  {
    QAction * a = menu.addAction(tr("point up"));
    connect(a, SIGNAL(triggered()), this, SLOT(addPointUp()));
    a = menu.addAction(tr("point down"));
    connect(a, SIGNAL(triggered()), this, SLOT(addPointDown()));
    a = menu.addAction(tr("point left"));
    connect(a, SIGNAL(triggered()), this, SLOT(addPointLeft()));
    a = menu.addAction(tr("point right"));
    connect(a, SIGNAL(triggered()), this, SLOT(addPointRight()));
  }
  else
  {
    if (key->getKeyWord() != PGFpointup)
    {
      QAction * a = menu.addAction(tr("point up"));
      connect(a, SIGNAL(triggered()), this, SLOT(addPointUp()));
    }

    if (key->getKeyWord() != PGFpointdown)
    {
      QAction * a = menu.addAction(tr("point down"));
      connect(a, SIGNAL(triggered()), this, SLOT(addPointDown()));
    }

    if (key->getKeyWord() != PGFpointleft)
    {
      QAction * a = menu.addAction(tr("point left"));
      connect(a, SIGNAL(triggered()), this, SLOT(addPointLeft()));
    }

    if (key->getKeyWord() != PGFpointright)
    {
      QAction * a = menu.addAction(tr("point right"));
      connect(a, SIGNAL(triggered()), this, SLOT(addPointRight()));
    }
  }
}

void XWTIKZOptions::addRadiusAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("radius"));
  connect(a, SIGNAL(triggered()), this, SLOT(setRadius()));
}

void XWTIKZOptions::addRoundedCornersAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("rounded corners"));
  connect(a, SIGNAL(triggered()), this, SLOT(setRoundedCorners()));
}

void XWTIKZOptions::addRotateAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("rotate"));
  connect(a, SIGNAL(triggered()), this, SLOT(setRotate()));
  a = menu.addAction(tr("rotate around"));
  connect(a, SIGNAL(triggered()), this, SLOT(setRotateAround()));
}

void XWTIKZOptions::addScaleAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("scale"));
  connect(a, SIGNAL(triggered()), this, SLOT(setScale()));
  a = menu.addAction(tr("x scale"));
  connect(a, SIGNAL(triggered()), this, SLOT(setXScale()));
  a = menu.addAction(tr("y scale"));
  connect(a, SIGNAL(triggered()), this, SLOT(setYScale()));
  a = menu.addAction(tr("scale around"));
  connect(a, SIGNAL(triggered()), this, SLOT(setScaleAround()));
}

void XWTIKZOptions::addScopeFading(QMenu & menu)
{
  QAction * a = menu.addAction(tr("scope fading"));
  connect(a, SIGNAL(triggered()), this, SLOT(setScopeFading()));
}

void XWTIKZOptions::addShadeAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("shading"));
  connect(a, SIGNAL(triggered()), this, SLOT(setShade()));
  a = menu.addAction(tr("shading angle"));
  connect(a, SIGNAL(triggered()), this, SLOT(setShadeAngle()));
  a = menu.addAction(tr("top color"));
  connect(a, SIGNAL(triggered()), this, SLOT(setTopColor()));
  a = menu.addAction(tr("middle color"));
  connect(a, SIGNAL(triggered()), this, SLOT(setMiddleColor()));
  a = menu.addAction(tr("bottom color"));
  connect(a, SIGNAL(triggered()), this, SLOT(setBottomColor()));
  a = menu.addAction(tr("left color"));
  connect(a, SIGNAL(triggered()), this, SLOT(setLeftColor()));
  a = menu.addAction(tr("right color"));
  connect(a, SIGNAL(triggered()), this, SLOT(setRightColor()));
  a = menu.addAction(tr("ball color"));
  connect(a, SIGNAL(triggered()), this, SLOT(setBallColor()));
  a = menu.addAction(tr("lower left color"));
  connect(a, SIGNAL(triggered()), this, SLOT(setLowerLeftColor()));
  a = menu.addAction(tr("lower right color"));
  connect(a, SIGNAL(triggered()), this, SLOT(setLowerRightColor()));
  a = menu.addAction(tr("upper left color"));
  connect(a, SIGNAL(triggered()), this, SLOT(setUpperLeftColor()));
  a = menu.addAction(tr("upper right color"));
  connect(a, SIGNAL(triggered()), this, SLOT(setUpperRightColor()));
  a = menu.addAction(tr("inner color"));
  connect(a, SIGNAL(triggered()), this, SLOT(setInnerColor()));
  a = menu.addAction(tr("outer color"));
  connect(a, SIGNAL(triggered()), this, SLOT(setOuterColor()));
}

void XWTIKZOptions::addShadowAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("shadow"));
  connect(a, SIGNAL(triggered()), this, SLOT(setShadow()));
  XWTIKZOptions * op = findShadow();
  if (op)
  {
    menu.addSeparator();
    a = menu.addAction(tr("shadow scale"));
    connect(a, SIGNAL(triggered()), op, SLOT(setShadowScale()));
    a = menu.addAction(tr("shadow xshift"));
    connect(a, SIGNAL(triggered()), op, SLOT(setShadowXShift()));
    a = menu.addAction(tr("shadow yshift"));
    connect(a, SIGNAL(triggered()), op, SLOT(setShadowYShift()));
    menu.addSeparator();
    op->addColorAction(menu);
    op->addOpacityAction(menu);
  }
}

void XWTIKZOptions::addShapeAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("shape"));
  connect(a, SIGNAL(triggered()), this, SLOT(setShape()));
  if (xwApp->getLicenseState() == APP_STATE_NORMAL)
  {
    int shape = PGFrectangle;
    XWTikzValue * op = getValue(PGFshape);
    if (op)
      shape = (int)(op->getValue());
    else
    {
      XWTikzKey * key = findShape();
      if (key)
        shape = key->getKeyWord();
    }

    switch (shape)
    {
      default:
        break;

      case PGFtrapezium:
        a = menu.addAction(tr("internal angle of lower side"));
        connect(a, SIGNAL(triggered()), this, SLOT(setTrapeziumAngle()));
        break;

      case PGFregularpolygon:
        a = menu.addAction(tr("regular polygon sides"));
        connect(a, SIGNAL(triggered()), this, SLOT(setRegularPolygonSides()));
        break;

      case PGFstar:
        a = menu.addAction(tr("star points"));
        connect(a, SIGNAL(triggered()), this, SLOT(setStarPoints()));
        a = menu.addAction(tr("star point height"));
        connect(a, SIGNAL(triggered()), this, SLOT(setStarPointHeight()));
        a = menu.addAction(tr("star point ratio"));
        connect(a, SIGNAL(triggered()), this, SLOT(setStarPointRatio()));
        a = menu.addAction(tr("isosceles triangle apex angle"));
        connect(a, SIGNAL(triggered()), this, SLOT(setIsoscelesTriangleApexAngle()));
        break;

      case PGFkite:
        a = menu.addAction(tr("kite vertex angles"));
        connect(a, SIGNAL(triggered()), this, SLOT(setKiteVertexAngles()));
        break;

      case PGFdart:
        a = menu.addAction(tr("dart angles"));
        connect(a, SIGNAL(triggered()), this, SLOT(setDartAngles()));
        break;

      case PGFcircularsector:
        a = menu.addAction(tr("circular sector angles"));
        connect(a, SIGNAL(triggered()), this, SLOT(setCircularSectorAngle()));
        break;

      case PGFcylinder:
        a = menu.addAction(tr("cylinder end fill color"));
        connect(a, SIGNAL(triggered()), this, SLOT(setCylinderEndFill()));
        a = menu.addAction(tr("cylinder body fill color"));
        connect(a, SIGNAL(triggered()), this, SLOT(setCylinderBodyFill()));
        break;

      case PGFmagnifyingglass:
        a = menu.addAction(tr("magnifying glass handle angle fill"));
        connect(a, SIGNAL(triggered()), this, SLOT(setMagnifyingGlassHandleAngleFill()));
        a = menu.addAction(tr("magnifying glass handle angle aspect"));
        connect(a, SIGNAL(triggered()), this, SLOT(setMagnifyingGlassHandleAngleAspect()));
        break;

      case PGFcloud:
        a = menu.addAction(tr("cloud puff arc"));
        connect(a, SIGNAL(triggered()), this, SLOT(setCloudPuffArc()));
        a = menu.addAction(tr("cloud puffs"));
        connect(a, SIGNAL(triggered()), this, SLOT(setCloudPuffs()));
        break;

      case PGFstarburst:
        a = menu.addAction(tr("starburst points"));
        connect(a, SIGNAL(triggered()), this, SLOT(setStarburstPoints()));
        a = menu.addAction(tr("starburst point height"));
        connect(a, SIGNAL(triggered()), this, SLOT(setStarburstPointHeight()));
        a = menu.addAction(tr("random starburst"));
        connect(a, SIGNAL(triggered()), this, SLOT(setRandomStarburst()));
        break;

      case PGFsignal:
        a = menu.addAction(tr("signal pointer angle"));
        connect(a, SIGNAL(triggered()), this, SLOT(setSignalPointerAngle()));
        break;

      case PGFtape:
        a = menu.addAction(tr("tape bend height"));
        connect(a, SIGNAL(triggered()), this, SLOT(setTapeBendHeight()));
        break;

      case PGFmagnetictape:
        a = menu.addAction(tr("magnetic tape tail extend"));
        connect(a, SIGNAL(triggered()), this, SLOT(setMagneticTapeTailExtend()));
        a = menu.addAction(tr("magnetic tape tail"));
        connect(a, SIGNAL(triggered()), this, SLOT(setMagneticTapeTail()));
        break;

      case PGFsinglearrow:
        a = menu.addAction(tr("single arrow tip angle"));
        connect(a, SIGNAL(triggered()), this, SLOT(setSingleArrowTipAngle()));
        a = menu.addAction(tr("single arrow head extend"));
        connect(a, SIGNAL(triggered()), this, SLOT(setSingleArrowHeadExtend()));
        a = menu.addAction(tr("single arrow head indent"));
        connect(a, SIGNAL(triggered()), this, SLOT(setSingleArrowHeadIndent()));
        break;

      case PGFdoublearrow:
        a = menu.addAction(tr("double arrow tip angle"));
        connect(a, SIGNAL(triggered()), this, SLOT(setDoubleArrowTipAngle()));
        a = menu.addAction(tr("double arrow head extend"));
        connect(a, SIGNAL(triggered()), this, SLOT(setDoubleArrowHeadExtend()));
        a = menu.addAction(tr("double arrow head indent"));
        connect(a, SIGNAL(triggered()), this, SLOT(setDoubleArrowHeadIndent()));
        break;

      case PGFarrowbox:
        a = menu.addAction(tr("arrow box tip angle"));
        connect(a, SIGNAL(triggered()), this, SLOT(setArrowBoxTipAngle()));
        a = menu.addAction(tr("arrow box head extend"));
        connect(a, SIGNAL(triggered()), this, SLOT(setArrowBoxHeadExtend()));
        a = menu.addAction(tr("arrow box head indent"));
        connect(a, SIGNAL(triggered()), this, SLOT(setArrowBoxHeadIndent()));
        a = menu.addAction(tr("arrow box shaft width"));
        connect(a, SIGNAL(triggered()), this, SLOT(setArrowBoxShaftWidth()));
        a = menu.addAction(tr("arrow box north arrow"));
        connect(a, SIGNAL(triggered()), this, SLOT(setArrowBoxNorthArrow()));
        a = menu.addAction(tr("arrow box south arrow"));
        connect(a, SIGNAL(triggered()), this, SLOT(setArrowBoxSouthArrow()));
        a = menu.addAction(tr("arrow box east arrow"));
        connect(a, SIGNAL(triggered()), this, SLOT(setArrowBoxEastArrow()));
        a = menu.addAction(tr("arrow box west arrow"));
        connect(a, SIGNAL(triggered()), this, SLOT(setArrowBoxWestArrow()));
        break;

      case PGFrectanglecallout:
      case PGFellipsecallout:
      case PGFcloudcallout:
        a = menu.addAction(tr("callout relative pointer"));
        connect(a, SIGNAL(triggered()), this, SLOT(setCalloutRelativePointer()));
        a = menu.addAction(tr("callout absolute pointer"));
        connect(a, SIGNAL(triggered()), this, SLOT(setCalloutAbsolutePointer()));
        a = menu.addAction(tr("callout pointer shorten"));
        connect(a, SIGNAL(triggered()), this, SLOT(setCalloutPointerShorten()));
        if (shape == PGFrectanglecallout)
        {
          a = menu.addAction(tr("callout pointer width"));
          connect(a, SIGNAL(triggered()), this, SLOT(setCalloutPointerWidth()));
        }
        else if (shape == PGFellipsecallout)
        {
          a = menu.addAction(tr("callout pointer arc"));
          connect(a, SIGNAL(triggered()), this, SLOT(setCalloutPointerArc()));
        }
        else
        {
          a = menu.addAction(tr("callout pointer segments"));
          connect(a, SIGNAL(triggered()), this, SLOT(setCalloutPointerSegments()));
        }
        break;

      case PGFroundedrectangle:
        a = menu.addAction(tr("rounded rectangle arc length"));
        connect(a, SIGNAL(triggered()), this, SLOT(setRoundedRectangleArcLength()));
        break;

      case PGFchamferedrectangle:
        a = menu.addAction(tr("chamfered rectangle angle"));
        connect(a, SIGNAL(triggered()), this, SLOT(setChamferedRectangleAngle()));
        a = menu.addAction(tr("chamfered rectangle sep"));
        connect(a, SIGNAL(triggered()), this, SLOT(setChamferedRectangleSep()));
        break;
    }

    a = menu.addAction(tr("inner separation"));
    connect(a, SIGNAL(triggered()), this, SLOT(setInnerSep()));
    a = menu.addAction(tr("outer separation"));
    connect(a, SIGNAL(triggered()), this, SLOT(setOuterSep()));
    a = menu.addAction(tr("minimum height and width"));
    connect(a, SIGNAL(triggered()), this, SLOT(setMinimumSize()));
    a = menu.addAction(tr("shape aspect"));
    connect(a, SIGNAL(triggered()), this, SLOT(setShapeAspect()));
    a = menu.addAction(tr("shape border rotate"));
    connect(a, SIGNAL(triggered()), this, SLOT(setShapeBorderRotate()));
  }
}

void XWTIKZOptions::addShiftAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("x shift"));
  connect(a, SIGNAL(triggered()), this, SLOT(setXShift()));
  a = menu.addAction(tr("y shift"));
  connect(a, SIGNAL(triggered()), this, SLOT(setYShift()));
  a = menu.addAction(tr("shift"));
  connect(a, SIGNAL(triggered()), this, SLOT(setShift()));
}

void XWTIKZOptions::addSizeAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("size"));
  connect(a, SIGNAL(triggered()), this, SLOT(setSize()));
}

void XWTIKZOptions::addSlantAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("x slant"));
  connect(a, SIGNAL(triggered()), this, SLOT(setXSlant()));
  a = menu.addAction(tr("y slant"));
  connect(a, SIGNAL(triggered()), this, SLOT(setYSlant()));
}

void XWTIKZOptions::addSpyAction(QMenu & menu)
{
  QAction * a = 0;
  bool s = false;
  XWTikzOperation * op = find(PGFspyusingoutlines);
  if (!op)
  {
    a = menu.addAction(tr("spy using outlines"));
    connect(a, SIGNAL(triggered()), this, SLOT(addSpyUsingOutlines()));
  }
  else
    s = true;

  op = find(PGFspyusingoverlays);
  if (!op)
  {
    a = menu.addAction(tr("spy using overlays"));
    connect(a, SIGNAL(triggered()), this, SLOT(addspyUsingOverlays()));
  }
  else
    s = true;

  if (s)
  {
    a = menu.addAction(tr("magnification"));
    connect(a, SIGNAL(triggered()), this, SLOT(setMagnification()));
    a = menu.addAction(tr("shape"));
    connect(a, SIGNAL(triggered()), this, SLOT(setSpyNodeShape()));
    a = menu.addAction(tr("connect spies"));
    connect(a, SIGNAL(triggered()), this, SLOT(addConnectSpies()));
    if (xwApp->getLicenseState() == APP_STATE_NORMAL)
    {
      a = menu.addAction(tr("size"));
      connect(a, SIGNAL(triggered()), this, SLOT(setSpySize()));
    }
  }
}

void XWTIKZOptions::addStateAction(QMenu & menu)
{
  XWTikzKey * op = findState();
  QAction * a;
  if (!op)
  {
    a = menu.addAction(tr("initial"));
    connect(a, SIGNAL(triggered()), this, SLOT(addInitial()));
    a = menu.addAction(tr("initial above"));
    connect(a, SIGNAL(triggered()), this, SLOT(addInitialAbove()));
    a = menu.addAction(tr("initial below"));
    connect(a, SIGNAL(triggered()), this, SLOT(addInitialBelow()));
    a = menu.addAction(tr("initial left"));
    connect(a, SIGNAL(triggered()), this, SLOT(addInitialLeft()));
    a = menu.addAction(tr("initial right"));
    connect(a, SIGNAL(triggered()), this, SLOT(addInitialRight()));
    a = menu.addAction(tr("initial by diamond"));
    connect(a, SIGNAL(triggered()), this, SLOT(addInitialByDiamond()));
    menu.addSeparator(); 
    a = menu.addAction(tr("accepting"));
    connect(a, SIGNAL(triggered()), this, SLOT(addAccepting()));
    a = menu.addAction(tr("accepting above"));
    connect(a, SIGNAL(triggered()), this, SLOT(addAcceptingAbove()));
    a = menu.addAction(tr("accepting below"));
    connect(a, SIGNAL(triggered()), this, SLOT(addAcceptingBelow()));
    a = menu.addAction(tr("accepting left"));
    connect(a, SIGNAL(triggered()), this, SLOT(addAcceptingLeft()));
    a = menu.addAction(tr("accepting right"));
    connect(a, SIGNAL(triggered()), this, SLOT(addAcceptingRight()));
    a = menu.addAction(tr("accepting by double"));
    connect(a, SIGNAL(triggered()), this, SLOT(addAcceptingByDouble()));
    menu.addSeparator();
    a = menu.addAction(tr("state"));
    connect(a, SIGNAL(triggered()), this, SLOT(addState()));
    a = menu.addAction(tr("state without output"));
    connect(a, SIGNAL(triggered()), this, SLOT(addStateWithoutOutput()));
    a = menu.addAction(tr("state with output"));
    connect(a, SIGNAL(triggered()), this, SLOT(addStateWithOutput()));
  }
  else
  {
    if (op->getKeyWord() != PGFinitial)
    {
      a = menu.addAction(tr("initial"));
      connect(a, SIGNAL(triggered()), this, SLOT(addInitial()));
    }

    if (op->getKeyWord() != PGFinitialabove)
    {
      a = menu.addAction(tr("initial above"));
      connect(a, SIGNAL(triggered()), this, SLOT(addInitialAbove()));
    }

    if (op->getKeyWord() != PGFinitialbelow)
    {
      a = menu.addAction(tr("initial below"));
      connect(a, SIGNAL(triggered()), this, SLOT(addInitialBelow()));
    }

    if (op->getKeyWord() != PGFinitialleft)
    {
      a = menu.addAction(tr("initial left"));
      connect(a, SIGNAL(triggered()), this, SLOT(addInitialLeft()));
    }

    if (op->getKeyWord() != PGFinitialright)
    {
      a = menu.addAction(tr("initial right"));
      connect(a, SIGNAL(triggered()), this, SLOT(addInitialRight()));
    }

    if (op->getKeyWord() != PGFinitialbydiamond)
    {
      a = menu.addAction(tr("initial by diamond"));
      connect(a, SIGNAL(triggered()), this, SLOT(addInitialByDiamond()));
    }

    menu.addSeparator();

    if (op->getKeyWord() != PGFaccepting)
    {
      a = menu.addAction(tr("accepting"));
      connect(a, SIGNAL(triggered()), this, SLOT(addAccepting()));
    }

    if (op->getKeyWord() != PGFacceptingabove)
    {
      a = menu.addAction(tr("accepting above"));
      connect(a, SIGNAL(triggered()), this, SLOT(addAcceptingAbove()));
    }

    if (op->getKeyWord() != PGFacceptingbelow)
    {
      a = menu.addAction(tr("accepting below"));
      connect(a, SIGNAL(triggered()), this, SLOT(addAcceptingBelow()));
    }

    if (op->getKeyWord() != PGFacceptingleft)
    {
      a = menu.addAction(tr("accepting left"));
      connect(a, SIGNAL(triggered()), this, SLOT(addAcceptingLeft()));
    }

    if (op->getKeyWord() != PGFacceptingright)
    {
      a = menu.addAction(tr("accepting right"));
      connect(a, SIGNAL(triggered()), this, SLOT(addAcceptingRight()));
    }

    if (op->getKeyWord() != PGFacceptingbydouble)
    {
      a = menu.addAction(tr("accepting by double"));
      connect(a, SIGNAL(triggered()), this, SLOT(addAcceptingByDouble()));
    }

    menu.addSeparator();

    if (op->getKeyWord() != PGFstate)
    {
      a = menu.addAction(tr("state"));
      connect(a, SIGNAL(triggered()), this, SLOT(addState()));
    }

    if (op->getKeyWord() != PGFstatewithoutoutput)
    {
      a = menu.addAction(tr("state with output"));
      connect(a, SIGNAL(triggered()), this, SLOT(addStateWithoutOutput()));
    }

    if (op->getKeyWord() != PGFstatewithoutoutput)
    {
      a = menu.addAction(tr("state with output"));
      connect(a, SIGNAL(triggered()), this, SLOT(addStateWithOutput()));
    }
  }
}

void XWTIKZOptions::addStepAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("step"));
  connect(a, SIGNAL(triggered()), this, SLOT(setStep()));
}

void XWTIKZOptions::addTextColorAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("text color"));
  connect(a, SIGNAL(triggered()), this, SLOT(setTextColor()));
}

void XWTIKZOptions::addTransformShapeAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("transform shape"));
  connect(a, SIGNAL(triggered()), this, SLOT(addTransformShape()));
  a = menu.addAction(tr("no transform shape"));
  connect(a, SIGNAL(triggered()), this, SLOT(removeTransformShape()));
}

void XWTIKZOptions::addXRadiusAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("x radius"));
  connect(a, SIGNAL(triggered()), this, SLOT(setXRadius()));
}

void XWTIKZOptions::addXYZAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("x-vector"));
  connect(a, SIGNAL(triggered()), this, SLOT(setXVector()));
  a = menu.addAction(tr("y-vector"));
  connect(a, SIGNAL(triggered()), this, SLOT(setYVector()));
  a = menu.addAction(tr("z-vector"));
  connect(a, SIGNAL(triggered()), this, SLOT(setZVector()));
}

void XWTIKZOptions::addYRadiusAction(QMenu & menu)
{
  QAction * a = menu.addAction(tr("y radius"));
  connect(a, SIGNAL(triggered()), this, SLOT(setYRadius()));
}

void XWTIKZOptions::addDomain(const QString & s,const QString & e)
{
  XWTikzDomain * d = new XWTikzDomain(graphic,this);
  d->setStart(s);
  d->setEnd(e);
  ops << d;
  cur = ops.size() - 1;
}

void XWTIKZOptions::addValueCoord(int keywordA, const QString & str)
{
  XWTikzValue * v = new  XWTikzValue(graphic,keywordA,this);
  v->setCoord(str);
  ops << v;
}

void XWTIKZOptions::addValueExpress(int keywordA, const QString & str)
{
  XWTikzValue * v = new  XWTikzValue(graphic,keywordA,this);
  v->setExpress(str);
  ops << v;
}

bool XWTIKZOptions::back(XWTikzState * state)
{
  if (cur < 0 || cur >= ops.size())
    return false;

  return ops[cur]->back(state);
}

bool XWTIKZOptions::del(XWTikzState * state)
{
  if (cur < 0 || cur >= ops.size())
    return false;

  return ops[cur]->del(state);
}

void XWTIKZOptions::doChildAnchor(XWTikzState * state)
{
  XWTikzOperation  * op = find(PGFedgefromparentpath);
  if (op)
  {
    XWTikzOperationText * top = (XWTikzOperationText*)(op);
    top->doChildAnchor(state);
  }
}

void XWTIKZOptions::doCompute(XWTikzState * state)
{
  for (int i= 0; i < ops.size(); i++)
    ops[i]->doCompute(state);
}

void XWTIKZOptions::doDecoration(XWTikzState * state)
{
  XWTikzOperation  * op = find(PGFdecoration);
  if (op)
    op->doPath(state,false);
}

void XWTIKZOptions::doEdgeFromParent(XWTikzState * state)
{
  XWTikzOperation  * op = find(PGFedgefromparent);
  if (op)
    op->doPath(state,false);
}

void XWTIKZOptions::doEdgeFromParentPath(XWTikzState * state)
{
  XWTikzOperation  * op = find(PGFedgefromparentpath);
  if (op)
    op->doPath(state,false);
  else
  {
    XWTikzOperationText edge(graphic, PGFedgefromparentpath);
    edge.doPath(state);
  }
}

void XWTIKZOptions::doEveryAttribute(XWTikzState * state)
{
  XWTikzOperation  * op = find(PGFeveryattribute);
  if (op)
    op->doPath(state,false);
}

void XWTIKZOptions::doEveryEntity(XWTikzState * state)
{
  XWTikzOperation  * op = find(PGFeveryentity);
  if (op)
    op->doPath(state,false);
}

void XWTIKZOptions::doEveryAcceptingByArrow(XWTikzState * state)
{
  XWTikzOperation  * op = find(PGFeveryacceptingbyarrowstyle);
  if (op)
    op->doPath(state,false);
}

void XWTIKZOptions::doEveryChild(XWTikzState * state)
{
  XWTikzOperation  * op = find(PGFeverychild);
  if (op)
    op->doPath(state,false);
}

void XWTIKZOptions::doEveryChildNode(XWTikzState * state)
{
  XWTikzOperation  * op = find(PGFeverychildnode);
  if (op)
    op->doPath(state,false);
}

void XWTIKZOptions::doEveryCircuitAnnotation(XWTikzState * state)
{
   XWTikzOperation  * op = find(PGFeverycircuitannotationstyle);
  if (op)
    op->doPath(state,false);
}

void XWTIKZOptions::doEveryCircuitSymbol(XWTikzState * state)
{
   XWTikzOperation  * op = find(PGFeverycircuitsymbolstyle);
  if (op)
    op->doPath(state,false);
}

void XWTIKZOptions::doEveryConcept(XWTikzState * state)
{
  XWTikzOperation  * op = find(PGFeveryconcept);
  if (op)
    op->doPath(state,false);
}

void XWTIKZOptions::doEveryEdge(XWTikzState * state)
{
  XWTikzOperation  * op = find(PGFeveryedge);
  if (op)
    op->doPath(state,false);
}

void XWTIKZOptions::doEveryInfo(XWTikzState * state)
{
  XWTikzOperation  * op = find(PGFeveryinfo);
  if (op)
    op->doPath(state,false);
}

void XWTIKZOptions::doEveryInitialByArrow(XWTikzState * state)
{
  XWTikzOperation  * op = find(PGFeveryinitialbyarrowstyle);
  if (op)
    op->doPath(state,false);
}

void XWTIKZOptions::doEveryLabel(XWTikzState * state)
{
  XWTikzOperation  * op = find(PGFeverylabel);
  if (op)
    op->doPath(state,false);
}

void XWTIKZOptions::doEveryMark(XWTikzState * state)
{
  XWTikzOperation  * op = find(PGFeverymark);
  if (op)
    op->doPath(state,false);
}

void XWTIKZOptions::doEveryMatrix(XWTikzState * state)
{
  XWTikzOperation  * op = find(PGFeverymatrix);
  if (op)
    op->doPath(state,false);
}

void XWTIKZOptions::doEveryMindmap(XWTikzState * state)
{
  XWTikzOperation  * op = find(PGFeverymindmap);
  if (op)
    op->doPath(state,false);
}

void XWTIKZOptions::doEveryNode(XWTikzState * state)
{
  XWTikzOperation  * op = find(PGFeverynode);
  if (op)
    op->doPath(state,false);
}

void XWTIKZOptions::doEveryPin(XWTikzState * state)
{
  XWTikzOperation  * op = find(PGFeverypin);
  if (op)
    op->doPath(state,false);
}

void XWTIKZOptions::doEveryPinEdge(XWTikzState * state)
{
  XWTikzOperation  * op = find(PGFeverypinedge);
  if (op)
    op->doPath(state,false);
}

void XWTIKZOptions::doEveryRelationship(XWTikzState * state)
{
  XWTikzOperation  * op = find(PGFeveryrelationship);
  if (op)
    op->doPath(state,false);
}

void XWTIKZOptions::doEveryShadow(XWTikzState * state)
{
  XWTikzOperation  * op = find(PGFeveryshadow);
  if (op)
    op->doPath(state,false);
}

void XWTIKZOptions::doEveryShape(XWTikzState * state)
{
  int k = state->getShape();
  if (k > 0)
  {
    XWTikzOperation  * op = find(k);
    if (op)
      op->doPath(state,false);
  }
}

void XWTIKZOptions::doEveryState(XWTikzState * state)
{
   XWTikzOperation  * op = find(PGFeverystatestyle);
  if (op)
    op->doPath(state,false);
}

void XWTIKZOptions::doLevel(XWTikzState * state)
{
  XWTikzOperation  * op = find(PGFlevel);
  if (op)
    op->doPath(state,false);
}

void XWTIKZOptions::doLevelConcept(XWTikzState * state)
{
  int id = 0;
  switch (state->getLevel())
  {
    default:
      break;

    case 1:
      id = PGFlevel1concept;
      break;

    case 2:
      id = PGFlevel2concept;
      break;

    case 3:
      id = PGFlevel3concept;
      break;

    case 4:
      id = PGFlevel4concept;
      break;
  }
  XWTikzOperation  * op = find(id);
  if (op)
    op->doPath(state,false);
  else
  {
    XWTikzLevelConcept c(graphic,id);
    c.doPath(state,false);
  }
}

void XWTIKZOptions::doLevelNumber(XWTikzState * state)
{
  int l = state->getLevel();
  int k = -1;
  switch (l)
  {
    default:
      break;

    case 1:
      k = PGFlevelone;
      break;

    case 2:
      k = PGFleveltwo;
      break;

    case 3:
      k = PGFlevelthree;
      break;

    case 4:
      k = PGFlevelfour;
      break;
  }

  if (k > 0)
  {
    XWTikzOperation  * op = find(k);
    if (op)
      op->doPath(state,false);
  }
}

void XWTIKZOptions::doParentAnchor(XWTikzState * state)
{
  XWTikzOperation  * op = find(PGFedgefromparentpath);
  if (op)
  {
    XWTikzOperationText * top = (XWTikzOperationText*)(op);
    top->doParentAnchor(state);
  }
}

void XWTIKZOptions::doPath(XWTikzState * state, bool showpoint)
{
  for (int i= 0; i < ops.size(); i++)
  {
    switch (ops[i]->getKeyWord())
    {
      default:
        ops[i]->doPath(state,showpoint);
        break;

      case PGFpreactions:
      case PGFpostaction:
      case PGFeverynode:
      case PGFdecoration:
      case PGFeveryrectanglenode:
      case PGFeverycirclenode:
      case PGFeverylabel:
      case PGFeverypin:
      case PGFeverypinedge:
      case PGFeveryedge:
      case PGFeverymatrix:
      case PGFedgefromparent:
      case PGFeverymark:
      case PGFlevel:
      case PGFlevelone:
      case PGFleveltwo:
      case PGFlevelthree:
      case PGFlevelfour:
      case PGFeverychild:
      case PGFeverychildnode:
      case PGFedgefromparentpath:
      case PGFtopath:
      case PGFeveryto:
      case PGFrootconcept:
      case PGFeverymindmap:
      case PGFeveryconcept:
      case PGFlevel1concept:
      case PGFlevel2concept:
      case PGFlevel3concept:
      case PGFlevel4concept:
      case PGFspyusingoutlines:
      case PGFspyusingoverlays:
      case PGFspyscope:
      case PGFconnectspies:
      case PGFspyconnectionpath:
      case PGFeveryinfo:
      case PGFeverycircuitsymbolstyle:
      case PGFeverycircuitannotationstyle:
      case PGFeverystatestyle:
      case PGFstatestyle:
      case PGFeveryinitialbyarrowstyle:
      case PGFeveryacceptingbyarrowstyle:
      case PGFeveryentity:
      case PGFeveryrelationship:
      case PGFeveryattribute:
      case PGFeveryshadow:
      case PGFgeneralshadow:
      case PGFdropshadow:
      case PGFcirculardropshadow:
      case PGFcircularglow:
      case PGFcopyshadow:
      case PGFdoublecopyshadow:
        break;
    }
  }
}

void XWTIKZOptions::doPre(XWTikzState * state)
{
  for (int i= 0; i < ops.size(); i++)
  {
    switch (ops[i]->getKeyWord())
    {
      default:
        break;

      case PGFpreactions:
      case PGFgeneralshadow:
      case PGFdropshadow:
      case PGFcirculardropshadow:
      case PGFcircularglow:
      case PGFcopyshadow:
      case PGFdoublecopyshadow:
        ops[i]->doPath(state, false);
        break;
    }
  }
}

void XWTIKZOptions::doPost(XWTikzState * state)
{
  XWTikzOperation  * op = find(PGFpostaction);
  if (op)
    op->doPath(state,false);
}

void XWTIKZOptions::doRootConcept(XWTikzState * state)
{
  XWTikzOperation  * op = find(PGFrootconcept);
  if (op)
    op->doPath(state,false);
  else
  {
    XWTikzRootConcept r(graphic);
    r.doPath(state,false);
  }
}

void XWTIKZOptions::doSpyConnection(XWTikzState * state)
{
  XWTIKZOptions * spy = this;
  XWTikzOperation  * op = find(PGFspyscope);
  if (!op)
    op = find(PGFspyusingoutlines);
  if (!op)
    op = find(PGFspyusingoverlays);
  if (op)
    spy = (XWTIKZOptions*)(op);
  
  op = spy->find(PGFconnectspies);
  if (op)
    op->doPath(state,false);
}

void XWTIKZOptions::doSpyNode(XWTikzState * state)
{
  XWTikzOperation  * op = find(PGFspyusingoutlines);
  if (op)
    op->doPath(state,false);
  else
  {
    op = find(PGFspyusingoverlays);
    if (op)
      op->doPath(state,false);
  }
}

void XWTIKZOptions::doState(XWTikzState * state)
{
  XWTikzOperation  * op = find(PGFstatestyle);
  if (op)
    op->doPath(state,false);
}

void XWTIKZOptions::doToPath(XWTikzState * state)
{
  XWTikzOperation  * op = find(PGFeveryto);
  if (op)
    op->doPath(state,false);
  op = find(PGFtopath);
  if (op)
    op->doPath(state,false);
  else
  {
    XWTikzOperationText topath(graphic, PGFtopath);
    topath.doPath(state,false);
  }
}

void XWTIKZOptions::dragTo(XWTikzState * state)
{
  for (int i= 0; i < ops.size(); i++)
  {
    switch (ops[i]->getKeyWord())
    {
      default:
        ops[i]->dragTo(state);
        break;

      case PGFpreactions:
      case PGFpostaction:
      case PGFdecoration:
      case PGFeverynode:
      case PGFeveryrectanglenode:
      case PGFeverycirclenode:
      case PGFeverylabel:
      case PGFeverypin:
      case PGFeverypinedge:
      case PGFeveryedge:
      case PGFeverymatrix:
      case PGFedgefromparent:
      case PGFeverymark:
      case PGFlevel:
      case PGFlevelone:
      case PGFleveltwo:
      case PGFlevelthree:
      case PGFlevelfour:
      case PGFeverychild:
      case PGFeverychildnode:
      case PGFedgefromparentpath:
      case PGFtopath:
      case PGFeveryto:
      case PGFeverymindmap:
      case PGFeveryconcept:
      case PGFrootconcept:
      case PGFlevel1concept:
      case PGFlevel2concept:
      case PGFlevel3concept:
      case PGFlevel4concept:
      case PGFspyusingoutlines:
      case PGFspyusingoverlays:
      case PGFspyscope:
      case PGFconnectspies:
      case PGFspyconnectionpath:
      case PGFeveryinfo:
      case PGFeverycircuitsymbolstyle:
      case PGFeverycircuitannotationstyle:
      case PGFeverystatestyle:
      case PGFstatestyle:
      case PGFeveryinitialbyarrowstyle:
      case PGFeveryacceptingbyarrowstyle:
      case PGFeveryentity:
      case PGFeveryrelationship:
      case PGFeveryattribute:
      case PGFeveryshadow:
      case PGFgeneralshadow:
      case PGFdropshadow:
      case PGFcirculardropshadow:
      case PGFcircularglow:
      case PGFcopyshadow:
      case PGFdoublecopyshadow:
        break;
    }
  }
}

bool XWTIKZOptions::dropTo(XWTikzState * state)
{
  if (cur < 0 || cur >= ops.size())
    return false;

  return ops[cur]->dropTo(state);
}

XWTikzcm * XWTIKZOptions::getcm()
{
  XWTikzOperation * op = find(PGFcm);
  return (XWTikzcm*)(op);
}

XWTikzAround * XWTIKZOptions::getAround(int keywordA)
{
  XWTikzOperation * op = find(keywordA);
  return (XWTikzAround*)(op);
}

XWTikzArrows * XWTIKZOptions::getArrows()
{
  XWTikzOperation * op = find(PGFarrows);
  return (XWTikzArrows*)(op);
}

XWTikzColor  * XWTIKZOptions::getColor(int keywordA)
{
  XWTikzOperation * op = find(keywordA);
  return (XWTikzColor*)(op);
}

QString XWTIKZOptions::getContent()
{
  QString ret;
  for (int i = 0; i < ops.size(); i++)
  {
    QString tmp = ops[i]->getText();
    ret += tmp;
    if (i < ops.size() - 1)
      ret += ",";
  }

  return ret;
}

XWTikzDomain * XWTIKZOptions::getDomain()
{
  XWTikzOperation * op = find(PGFdomain);
  return (XWTikzDomain*)(op);
}

XWTikzKey * XWTIKZOptions::getKey(int keywordA)
{
  XWTikzOperation * op = find(keywordA);
  return (XWTikzKey*)(op);
}

QPointF XWTIKZOptions::getPoint(XWTikzState * stateA)
{
  if (cur >= 0 && cur < ops.size())
    return ops[cur]->getPoint(stateA);
  return QPointF();
}

QString XWTIKZOptions::getText()
{
  QString o;
  int len = 0;
  for (int i = 0; i < ops.size(); i++)
  {
    QString tmp = ops[i]->getText();
    o += tmp;
    len += tmp.length();
    if (i < ops.size() - 1)
      o += ",";

    if (len > 60)
    {
      o += "\n";
      len = 0;
    }
  }

  QString ret;
  if (!o.isEmpty())
    ret = QString("[%1]").arg(o);

  return ret;
}

XWTikzValue  * XWTIKZOptions::getValue(int keywordA)
{
  XWTikzOperation * op = find(keywordA);
  return (XWTikzValue*)(op);
}

bool XWTIKZOptions::hasPost()
{
  XWTikzOperation  * op = find(PGFpostaction);
  if (op)
    return true;

  return false;
}

bool XWTIKZOptions::hasPre()
{
  for (int i= 0; i < ops.size(); i++)
  {
    switch (ops[i]->getKeyWord())
    {
      default:
        break;

      case PGFpreactions:
      case PGFgeneralshadow:
      case PGFdropshadow:
      case PGFcirculardropshadow:
      case PGFcircularglow:
      case PGFcopyshadow:
      case PGFdoublecopyshadow:
        return true;
        break;
    }
  }

  return false;
}

bool XWTIKZOptions::hitTest(XWTikzState * state)
{
  cur = -1;
  for (int i= 0; i < ops.size(); i++)
  {
    if (ops[i]->hitTest(state))
    {
      cur = i;
      return true;
    }
  }

  return false;
}

void XWTIKZOptions::insert(int i, XWTikzOperation * opA)
{
  cur = i;
  if (i >= ops.size())
    ops << opA;
  else
    ops.insert(i,opA);
}

bool XWTIKZOptions::isMatrix()
{
  XWTikzOperation * op = find(PGFmatrix);
  return (op != 0);
}

bool XWTIKZOptions::isMe(const QString & nameA,XWTikzState * state)
{
  cur = -1;
  for (int i= 0; i < ops.size(); i++)
  {
    if (ops[i]->isMe(nameA,state))
    {
      cur = i;
      return true;
    }
  }

  return false;
}

bool XWTIKZOptions::keyInput(XWTikzState * state)
{
  if (cur < 0 || cur >= ops.size())
    return false;

  return ops[cur]->keyInput(state);
}

bool XWTIKZOptions::newPar(XWTikzState * state)
{
  if (cur < 0 || cur >= ops.size())
    return false;

  return ops[cur]->newPar(state);
}

bool XWTIKZOptions::paste(XWTikzState * state)
{
  if (cur < 0 || cur >= ops.size())
    return false;

  return ops[cur]->paste(state);
}

void XWTIKZOptions::scan(const QString & str, int & len, int & pos)
{
  while (str[pos].isSpace())
    pos++;

  QChar s('[');
  QChar e(']');
  if (getKeyWord() > 100)
  {
    s = '{';
    e = '}';
  }

  if (str[pos] != s)
    return ;

  pos++;
  QString key;
  QRegExp arrowexp("[\\w\\s]*\\-\\[\\w\\s]*");
  while (pos < len)
  {
    if (str[pos] == e)
    {
      pos++;
      break;
    }
    else if (str[pos] == QChar('%'))
      XWTeXBox::skipComment(str,len,pos);
    else if (str[pos].isSpace())
      pos++;
    else if (str[pos] == QChar(','))
      pos++;
    else
    {
      scanKey(str,len,pos,key);
      int id = lookupPGFID(key);
      switch (id)
      {
        case PGFsharpcorners:
        case PGFbendatstart:
        case PGFbendatend:
        case PGFultrathin:
        case PGFverythin:
        case PGFthin:
        case PGFsemithick:
        case PGFthick:
        case PGFverythick:
        case PGFultrathick:
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
        case PGFnonzerorule:
        case PGFevenoddrule:
        case PGFshiftonly:
        case PGFresetcm:
        case PGFbehindpath:
        case PGFinfrontofpath:
        case PGFtransformshape:
        case PGFswap:
        case PGFsloped:
        case PGFallowupsidedown:
        case PGFmidway:
        case PGFnearstart:
        case PGFnearend:
        case PGFverynearstart:
        case PGFverynearend:
        case PGFatstart:
        case PGFatend:
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
        case PGFedgefromparentforkdown:
        case PGFedgefromparentforkup:
        case PGFedgefromparentforkleft:
        case PGFedgefromparentforkright:
        case PGFgrowcyclic:
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
        case PGFmindmap:
        case PGFsmallmindmap:
        case PGFconcept:
        case PGFextraconcept:
        case PGFconceptconnection:
        case PGFannotation:
        case PGFconnectspies:
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
        case PGFcircuit:
        case PGFcircuits:
        case PGFhugecircuitsymbols:
        case PGFlargecircuitsymbols:
        case PGFmediumcircuitsymbols:
        case PGFsmallcircuitsymbols:
        case PGFtinycircuitsymbols:
        case PGFpointup:
        case PGFpointdown:
        case PGFpointleft:
        case PGFpointright:
        case PGFcircuitsymbolopen:
        case PGFcircuitsymbolfilled:
        case PGFcircuitsymbollines:
        case PGFcircuitsymbolwires:
        case PGFcircuiteeIEC:
        case PGFcircuitlogic:
        case PGFcircuitlogicIEC:
        case PGFcircuitlogicUS:
        case PGFcircuitlogicCDH:
        case PGFcanvasisplane:
        case PGFstatewithoutoutput:
        case PGFstatewithoutput:
        case PGFacceptingbydouble:
        case PGFinitialbydiamond:
        case PGFinitialabove:
        case PGFinitialbelow:
        case PGFinitialleft:
        case PGFinitialright:
        case PGFacceptingabove:
        case PGFacceptingbelow:
        case PGFacceptingleft:
        case PGFacceptingright:
        case PGFstate:
        case PGFaccepting:
        case PGFinitial:
        case PGFentity:
        case PGFrelationship:
        case PGFattribute:
        case PGFkeyattribute:
          {
            XWTikzKey * k = new XWTikzKey(graphic,id,this);
            ops << k;
          }
          break;

        case PGFarrowdefault:
        case PGFbaseline:
        case PGFroundedcorners:
        case PGFradius:
        case PGFxradius:
        case PGFyradius:
        case PGFat:
        case PGFstartangle:
        case PGFendangle:
        case PGFdeltaangle:
        case PGFstep:
        case PGFxstep:
        case PGFystep:
        case PGFbend:
        case PGFbendpos:
        case PGFparabolaheight:
        case PGFlinewidth:
        case PGFlinecap:
        case PGFlinejoin:
        case PGFmiterlimit:
        case PGFdashphase:
        case PGFdrawopacity:
        case PGFfillopacity:
        case PGFopacity:
        case PGFnodecontents:
        case PGFnameprefix:
        case PGFnamesuffix:
        case PGFshape:
        case PGFanchor:
        case PGFangle:
        case PGFinnersep:
        case PGFinnerxsep:
        case PGFinnerysep:
        case PGFoutersep:
        case PGFouterxsep: 
        case PGFouterysep:
        case PGFminimumheight:
        case PGFminimumwidth:
        case PGFminimumsize:
        case PGFshapeaspect:
        case PGFshapeborderusesincircle:
        case PGFshapeborderrotate:
        case PGFsamples:
        case PGFx:
        case PGFy:
        case PGFz:
        case PGFshift:
        case PGFxshift:
        case PGFyshift:
        case PGFscale:
        case PGFxscale:
        case PGFyscale:
        case PGFxslant:
        case PGFyslant:
        case PGFrotate:
        case PGFpattern:
        case PGFshading:
        case PGFshadingangle:
        case PGFpathfading:
        case PGFfitfading:
        case PGFscopefading:
        case PGFdoubledistance:
        case PGDdoubledistancebetweenlinecenters:
        case PGFnamepath:
        case PGFname:
        case PGFpos:
        case PGFauto:
        case PGFlabelposition:
        case PGFabsolute:
        case PGFlabeldistance:
        case PGFpindistance:
        case PGFpinposition:
        case PGFmatrix:
        case PGFmatrixanchor:
        case PGFleveldistance:
        case PGFsiblingdistance:
        case PGFgrow:
        case PGFgrowopposite:        
        case PGFmissing:
        case PGFgrowthparentanchor:
        case PGFchildanchor:
        case PGFparentanchor:
        case PGFmark:
        case PGFmarkrepeat:
        case PGFmarkphase:
        case PGFmarksize:
        case PGFtension:
        case PGFdecorate:
        case PGFraise:
        case PGFmirror:
        case PGFpre:
        case PGFprelength:
        case PGFpost:
        case PGFpostlength:
        case PGFaspect:
        case PGFtrapeziumleftangle:
        case PGFtrapeziumrightangle:
        case PGFtrapeziumangle:
        case PGFtrapeziumstretches:
        case PGFtrapeziumstretchesbody:
        case PGFregularpolygonsides:
        case PGFstarpoints:
        case PGFstarpointheight:
        case PGFstarpointratio:
        case PGFisoscelestriangleapexangle:
        case PGFisoscelestrianglestretches:
        case PGFkiteuppervertexangle:
        case PGFkitelowervertexangle:
        case PGFdarttipangle:
        case PGFdarttailangle:
        case PGFcircularsectorangle:
        case PGFcylinderusescustomfill:
        case PGFcloudpuffs:
        case PGFcloudpuffarc:
        case PGFcloudignoresaspect:
        case PGFMGHAfill:
        case PGFMGHAaspect:
        case PGFstarburstpoints:
        case PGFstarburstpointheight:
        case PGFrandomstarburst:
        case PGFsignalpointerangle:
        case PGFtapebendtop:
        case PGFtapebendbottom:
        case PGFtapebendheight:
        case PGFmagnetictapetailextend:
        case PGFmagnetictapetail:
        case PGFsinglearrowtipangle:
        case PGFsinglearrowheadextend:
        case PGFsinglearrowheadindent:
        case PGFdoublearrowtipangle:
        case PGFdoublearrowheadextend:
        case PGFdoublearrowheadindent:
        case PGFarrowboxtipangle:
        case PGFarrowboxheadextend:
        case PGFarrowboxheadindent:
        case PGFarrowboxshaftwidth:
        case PGFarrowboxeastarrow:
        case PGFarrowboxnortharrow:
        case PGFarrowboxsoutharrow:
        case PGFarrowboxwestarrow:
        case PGFrectanglesplitparts:
        case PGFrectanglesplithorizontal:
        case PGFrectanglesplitignoreemptyparts:
        case PGFrectanglesplitemptypartwidth:
        case PGFrectanglesplitemptypartheight:
        case PGFrectanglesplitemptypartdepth:
        case PGFrectanglesplitdrawsplits:
        case PGFrectanglesplitusecustomfill:
        case PGFcalloutrelativepointer:
        case PGFcalloutabsolutepointer:
        case PGFcalloutpointershorten:
        case PGFcalloutpointerwidth:
        case PGFcalloutpointerarc:
        case PGFcalloutpointerstartsize:
        case PGFcalloutpointerendsize:
        case PGFcalloutpointersegments:
        case PGFroundedrectanglearclength:
        case PGFroundedrectanglewestarc:
        case PGFroundedrectangleleftarc:
        case PGFroundedrectangleeastarc:
        case PGFroundedrectanglerightarc:
        case PGFchamferedrectangleangle:
        case PGFchamferedrectanglexsep:
        case PGFchamferedrectangleysep:
        case PGFchamferedrectanglesep:
        case PGFclockwisefrom:
        case PGFcounterclockwisefrom:
        case PGFsiblingangle:
        case PGFsize:
        case PGFheight:
        case PGFwidth:
        case PGFmagnification:
        case PGFmarkconnectionnode:
        case PGFtips:
        case PGFshortenend:
        case PGFshortenstart:
        case PGFcircuitsymbolunit:
        case PGFlogicgateinvertedradius:
        case PGFlogicgateinputsep:
        case PGFlogicgateanchorsuseboundingbox:
        case PGFlogicgateIECsymbolalign:
        case PGFandgateIECsymbol:
        case PGFnandgateIECsymbol:
        case PGForgateIECsymbol:
        case PGFnorgateIECsymbol:
        case PGFxorgateIECsymbol:
        case PGFxnorgateIECsymbol:
        case PGFnotgateIECsymbol:
        case PGFbuffergateIECsymbol:
        case PGFplaneorigin:
        case PGFplanex:
        case PGFplaney:
        case PGFcanvasisxyplaneatz:
        case PGFcanvasisyxplaneatz:
        case PGFcanvasisxzplaneaty:
        case PGFcanvasiszxplaneaty:
        case PGFcanvasisyzplaneatx:
        case PGFcanvasiszyplaneatx:
        case PGFinitialtext:
        case PGFacceptingtext:
        case PGFinitialdistance:
        case PGFacceptingdistance:
        case PGFinitialwhere:
        case PGFacceptingwhere:
        case PGFshadowscale:
        case PGFshadowxshift:
        case PGFshadowyshift:
          {
            XWTikzValue * v= new XWTikzValue(graphic,id,this);
            ops << v;
            v->scan(str,len,pos);
          }
          break;

        case PGFsignalfrom:
        case PGFsignalto:
          {
            XWTikzAndValue * v= new XWTikzAndValue(graphic,id,this);
            ops << v;
            v->scan(str,len,pos);
          }
          break;

        case PGFcolor:
        case PGFfilldraw:
        case PGFdraw:
        case PGFfill:
        case PGFpatterncolor:
        case PGFtopcolor:
        case PGFmiddlecolor:
        case PGFbottomcolor:
        case PGFleftcolor:
        case PGFrightcolor:
        case PGFballcolor:
        case PGFlowerleft:
        case PGFlowerright:
        case PGFupperleft:
        case PGFupperright:
        case PGFinnercolor:
        case PGFoutercolor:
        case PGFdouble:
        case PGFtext:
        case PGFcylinderendfill:
        case PGFcylinderbodyfill:
        case PGFconceptcolor:
        case PGFlogicgateIECsymbolcolor:
          {
            XWTikzColor * c = new XWTikzColor(graphic,id,this);
            ops << c;
            c->scan(str,len,pos);
          }
          break;

        case PGFdashpattern:
          {
            XWTikzDashPattern * d = new XWTikzDashPattern(graphic,this);
            ops << d;
            d->scan(str,len,pos);
          }
          break;

        case PGFarrows:
          {
            XWTikzArrows * a = new XWTikzArrows(graphic,this);
            ops << a;
            a->scan(str,len,pos);
          }
          break;

        case PGFdomain:
          {
            XWTikzDomain * d = new XWTikzDomain(graphic,this);
            ops << d;
            d->scan(str,len,pos);
          }
          break;

        case PGFsamplesat:
          {
            XWTikzSamplesAt * a = new XWTikzSamplesAt(graphic,this);
            ops << a;
            a->scan(str,len,pos);
          }
          break;

        case PGFscalearound:
        case PGFrotatearound:
          {
            XWTikzAround * a = new XWTikzAround(graphic,id,this);
            ops << a;
            a->scan(str,len,pos);
          }
          break;

        case PGFcm:
          {
            XWTikzcm * cm = new XWTikzcm(graphic,this);
            ops << cm;
            cm->scan(str,len,pos);
          }
          break;

        case PGFcenter:
        case PGFmid:
        case PGFbase:
        case PGFnorth:
        case PGFabove:
        case PGFsouth:
        case PGFbelow:
        case PGFwest:
        case PGFleft:
        case PGFmidwest:
        case PGFbasewest:
        case PGFnorthwest:
        case PGFsouthwest:
        case PGFeast:
        case PGFright:
        case PGFmideast:
        case PGFbaseeast:
        case PGFnortheast:
        case PGFsoutheast:
          {
            XWTikzValue * v= new XWTikzValue(graphic,PGFanchor,this);
            v->setValue(id);
            ops << v;
          }
          break;

        case PGFgrowviathreepoints:
          {
            XWTikzThreePoint * p = new XWTikzThreePoint(graphic,this);
            ops << p;
            p->scan(str,len,pos);
          }
          break;

        case PGFfadingtransform:
          {
            XWTikzFadingTransform * t = new XWTikzFadingTransform(graphic,this);
            ops << t;
            t->scan(str,len,pos);
          }
          break;

        case PGFnameintersections:
          {
            XWTikzNameIntersections * t = new XWTikzNameIntersections(graphic,this);
            ops << t;
            t->scan(str,len,pos);
          }
          break;

        case PGFpreactions:
          {
            XWTikzPreactions * t = new XWTikzPreactions(graphic,this);
            ops << t;
            t->scan(str,len,pos);
          }
          break;

        case PGFpostaction:
          {
            XWTikzPostaction * t = new XWTikzPostaction(graphic,this);
            ops << t;
            t->scan(str,len,pos);
          }
          break;

        case PGFeverynode:
          {
            XWTikzEveryNode * s = new XWTikzEveryNode(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFeveryrectanglenode:
          {
            XWTikzEveryShape * s = new XWTikzEveryShape(graphic,PGFeveryrectanglenode,PGFrectangle,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFeverycirclenode:
          {
            XWTikzEveryShape * s = new XWTikzEveryShape(graphic,PGFeverycirclenode,PGFcircle,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFlabel:
        case PGFpin:
        case PGFinfo:
        case PGFinfomissingangle:
	      case PGFinfosloped:
	      case PGFinfomissinganglesloped:
          {
            XWTikzLabel * s = new XWTikzLabel(graphic,id,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFeverylabel:
          {
            XWTikzEveryLabel * s = new XWTikzEveryLabel(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFeverypin:
          {
            XWTikzEveryPin * s = new XWTikzEveryPin(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFeverypinedge:
          {
            XWTikzEveryPinEdge * s = new XWTikzEveryPinEdge(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFpinedge:
          {
            XWTikzPinEdge * s = new XWTikzPinEdge(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFeveryedge:
          {
            XWTikzEveryEdge * s = new XWTikzEveryEdge(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFlateoptions:
          {
            XWTikzLateOptions * s = new XWTikzLateOptions(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFeverymatrix:
          {
            XWTikzEveryMatrix * s = new XWTikzEveryMatrix(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFeverycell:
          {
            XWTikzEveryCell * s = new XWTikzEveryCell(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFcells:
          {
            XWTikzCells * s = new XWTikzCells(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFnodes:
          {
            XWTikzNodes * s = new XWTikzNodes(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFeveryoddcolumn:
          {
            XWTikzEveryOddColumn * s = new XWTikzEveryOddColumn(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFeveryevencolumn:
          {
            XWTikzEveryEvenColumn * s = new XWTikzEveryEvenColumn(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFeveryoddrow:
          {
            XWTikzEveryOddRow * s = new XWTikzEveryOddRow(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFeveryevenrow:
          {
            XWTikzEveryEvenRow * s = new XWTikzEveryEvenRow(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFeverychild:
          {
            XWTikzEveryChild * s = new XWTikzEveryChild(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFeverychildnode:
          {
            XWTikzEveryChildNode * s = new XWTikzEveryChildNode(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFlevel:
          {
            XWTikzLevel * s = new XWTikzLevel(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFlevelone:
          {
            XWTikzLevelNumber * s = new XWTikzLevelNumber(graphic,PGFlevelone,1,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFleveltwo:
          {
            XWTikzLevelNumber * s = new XWTikzLevelNumber(graphic,PGFleveltwo,2,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFlevelthree:
          {
            XWTikzLevelNumber * s = new XWTikzLevelNumber(graphic,PGFlevelthree,3,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFlevelfour:
          {
            XWTikzLevelNumber * s = new XWTikzLevelNumber(graphic,PGFlevelfour,4,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFmarkindices:
        case PGFcolumnsep:
        case PGFrowsep:
        case PGFrectanglesplitpartalign:
        case PGFrectanglesplitpartfill:
        case PGFchamferedrectanglecorners:
        case PGFarrowboxarrows:
          {
            XWTikzList * s = new XWTikzList(graphic,id,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFeverymark:
          {
            XWTikzEveryMark * s = new XWTikzEveryMark(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFmarkoptions:
          {
            XWTikzMarkOptions * s = new XWTikzMarkOptions(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFdecoration:
          {
            XWTikzDecorations * s = new XWTikzDecorations(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFtransform:
          {
            XWTikzTransform * s = new XWTikzTransform(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFedgefromparent:        
          {
            XWTikzEdgeFromParentStyle * s = new XWTikzEdgeFromParentStyle(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFedgefromparentpath:
        case PGFtopath:
          {
            XWTikzOperationText * s = new XWTikzOperationText(graphic,id,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFeveryto:
          {
            XWTikzEveryTo * s = new XWTikzEveryTo(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFeverymindmap:
          {
            XWTikzEveryMindmap * s = new XWTikzEveryMindmap(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFeveryconcept:
          {
            XWTikzEveryConcept * s = new XWTikzEveryConcept(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFrootconcept:
          {
            XWTikzRootConcept * s = new XWTikzRootConcept(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFlevel1concept:
        case PGFlevel2concept:
        case PGFlevel3concept:
        case PGFlevel4concept:
          {
            XWTikzLevelConcept * s = new XWTikzLevelConcept(graphic,id,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFeveryannotation:
          {
            XWTikzEveryAnnotation * s = new XWTikzEveryAnnotation(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFlens:
          {
            XWTikzLens * s = new XWTikzLens(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFspyconnectionpath:
          {
            XWTikzPathText * s = new XWTikzPathText(graphic,id,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFeveryspyinnode:
          {
            XWTikzEverySpyInNode * s = new XWTikzEverySpyInNode(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFeveryspyonnode:
          {
            XWTikzEverySpyOnNode * s = new XWTikzEverySpyOnNode(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFspyusingoutlines:
        case PGFspyusingoverlays:
          {
            XWTikzSpyUsing * s = new XWTikzSpyUsing(graphic,id,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFcircuithandlesymbol:
          {
            XWTikzCircuitHandleSymbol * s = new XWTikzCircuitHandleSymbol(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFeveryinfo:
          {
            XWTikzEveryInfo * s = new XWTikzEveryInfo(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFcircuitsymbolsize:
          {
            XWTikzSize * s = new XWTikzSize(graphic,id,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFeverycircuitsymbolstyle:
          {
            XWTikzEveryCircuitSymbol * s = new XWTikzEveryCircuitSymbol(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFeverycircuitannotationstyle:
          {
            XWTikzEveryCircuitAnnotation * s = new XWTikzEveryCircuitAnnotation(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFcircuitsymbolopenstyle:
          {
            XWTikzCircuitSymbolOpen * s = new XWTikzCircuitSymbolOpen(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFcircuitsymbolfilledstyle:
          {
            XWTikzCircuitSymbolFilled * s = new XWTikzCircuitSymbolFilled(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFcircuitsymbollinesstyle:
          {
            XWTikzCircuitSymbolLines * s = new XWTikzCircuitSymbolLines(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFcircuitsymbolwiresstyle:
          {
            XWTikzCircuitSymbolWires * s = new XWTikzCircuitSymbolWires(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFcircuitdeclareunit:
          {
            XWTikzCircuitDeclareUnit * s = new XWTikzCircuitDeclareUnit(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFannotationarrow:
          {
            XWTikzAnnotationArrow * s = new XWTikzAnnotationArrow(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFresistor:
        case PGFinductor:
        case PGFcapacitor:
        case PGFcontact:
        case PGFground:
        case PGFbattery:
        case PGFdiode:
        case PGFZenerdiode:
        case PGFtunneldiode:
        case PGFbackwarddiode:
        case PGFSchottkydiode:
        case PGFbreakdowndiode:        
        case PGFbulb:
        case PGFvoltagesource:
        case PGFcurrentsource:
        case PGFmakecontact:
        case PGFbreakcontact:
        case PGFresistorIEC:
        case PGFvarresistorIEC:
        case PGFinductorIEC:
        case PGFvarinductorIEC:
        case PGFcapacitorIEC:
        case PGFcontactIEC:
        case PGFgroundIEC:
        case PGFbatteryIEC:
        case PGFdiodeIEC:
        case PGFvardiodeIEC:
        case PGFZenerdiodeIEC:
        case PGFvarZenerdiodeIEC:
        case PGFtunneldiodeIEC:
        case PGFvartunneldiodeIEC:
        case PGFbackwarddiodeIEC:
        case PGFvarbackwarddiodeIEC:
        case PGFSchottkydiodeIEC:
        case PGFvarSchottkydiodeIEC:
        case PGFbreakdowndiodeIEC:
        case PGFvarbreakdowndiodeIEC:
        case PGFconnectionIEC:
        case PGFbulbIEC:
        case PGFvoltagesourceIEC:
        case PGFcurrentsourceIEC:
        case PGFmakecontactIEC:
        case PGFvarmakecontactIEC:
        case PGFbreakcontactIEC:
        case PGFamperemeter:
        case PGFvoltmeter:
        case PGFohmmeter:
        case PGFacsource:
        case PGFdcsource:
        case PGFcurrentdirection:
	      case PGFcurrentdirectionreversed:
        case PGFandgate:
        case PGFnandgate:
        case PGForgate:
        case PGFnorgate:
        case PGFxorgate:
        case PGFxnorgate:
        case PGFnotgate:
        case PGFbuffergate:
        case PGFandgateIEC:
        case PGFnandgateIEC:
        case PGForgateIEC:
        case PGFnorgateIEC:
        case PGFxorgateIEC:
        case PGFxnorgateIEC:
        case PGFnotgateIEC:
        case PGFbuffergateIEC:
        case PGFandgateUS:
        case PGFnandgateUS:
        case PGForgateUS:
        case PGFnorgateUS:
        case PGFxorgateUS:
        case PGFxnorgateUS:
        case PGFnotgateUS:
        case PGFbuffergateUS:
        case PGFandgateCDH:
        case PGFnandgateCDH:
          {
            XWTikzCircuitSymbol * s = new XWTikzCircuitSymbol(graphic,id,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFdirectioninfo:
        case PGFdirectioninfosouth:
        case PGFlightemitting:
	      case PGFlightemittingsouth:
        case PGFlightDependent:
	      case PGFlightDependentsouth:
        case PGFadjustable:
        case PGFadjustablesouth:
          {
            XWTikzAnnotation * s = new XWTikzAnnotation(graphic,id,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFinputs:
          {
            XWTikzInpus * s = new XWTikzInpus(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFlogicgateinputs:
          {
            XWTikzLogicGateInpus * s = new XWTikzLogicGateInpus(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFeverystatestyle:
        case PGFstatestyle:
          {
            XWTikzEveryState * s = new XWTikzEveryState(graphic,id,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFeveryacceptingbyarrowstyle:
          {
            XWTikzEveryAcceptingByArrow * s = new XWTikzEveryAcceptingByArrow(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFeveryinitialbyarrowstyle:
          {
            XWTikzEveryInitialByArrow * s = new XWTikzEveryInitialByArrow(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFeveryentity:
          {
            XWTikzEveryEntity * s = new XWTikzEveryEntity(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFeveryrelationship:
          {
            XWTikzEveryRelationship * s = new XWTikzEveryRelationship(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFeveryattribute:
          {
            XWTikzEveryAttribute * s = new XWTikzEveryAttribute(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFeveryshadow:
          {
            XWTikzEveryShadow * s = new XWTikzEveryShadow(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFgeneralshadow:
          {
            XWTikzGeneralShadow * s = new XWTikzGeneralShadow(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFdropshadow:
          {
            XWTikzDropShadow * s = new XWTikzDropShadow(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFcirculardropshadow:
          {
            XWTikzCircularDropShadow * s = new XWTikzCircularDropShadow(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFcircularglow:
          {
            XWTikzCircularGlow * s = new XWTikzCircularGlow(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFcopyshadow:
          {
            XWTikzCopyShadow * s = new XWTikzCopyShadow(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        case PGFdoublecopyshadow:
          {
            XWTikzDoubleCopyShadow * s = new XWTikzDoubleCopyShadow(graphic,this);
            ops << s;
            s->scan(str,len,pos);
          }
          break;

        default:
          if (graphic->isUnit(key))
          {
            XWTikzUnit * u = new XWTikzUnit(graphic,key,this);
            ops << u;
            u->scan(str,len,pos);
          }
          else if (arrowexp.exactMatch(key))
          {
            XWTikzArrows * a = new XWTikzArrows(graphic,this);
            ops << a;
            a->scan(str,len,pos);
          }
          else if (key.startsWith("black") || 
                   key.startsWith("blue") ||
                   key.startsWith("brown") ||
                   key.startsWith("cyan") ||
                   key.startsWith("darkgray") ||
                   key.startsWith("gray") ||
                   key.startsWith("green") ||
                   key.startsWith("lightgray") ||
                   key.startsWith("lime") ||
                   key.startsWith("magenta") ||
                   key.startsWith("olive") || 
                   key.startsWith("orange") ||
                   key.startsWith("pink") ||
                   key.startsWith("purple") ||
                   key.startsWith("red") ||
                   key.startsWith("teal") ||
                   key.startsWith("violet") ||
                   key.startsWith("white") ||
                   key.startsWith("yellow") ||
                   key.startsWith("lightsteelblue") ||
                   key.startsWith("darklightsteelblue"))
          {
            XWTikzColor * c = new XWTikzColor(graphic,PGFcolor,this);
            ops << c;
            c->scan(str,len,pos);
          }
          break;
      }

      if (str[pos] == QChar(',') || str[pos] == QChar('='))
        pos++;
    }
  }
}

void XWTIKZOptions::setDouble()
{
  setColor(PGFdouble,tr("double"));
}

void XWTIKZOptions::setDoubleArrowTipAngle()
{
  setExpress(PGFdoublearrowtipangle,tr("double arrow tip angle"),tr("angle:"));
}

void XWTIKZOptions::setDoubleArrowHeadExtend()
{
  setExpress(PGFdoublearrowheadextend,tr("double arrow head extend"),tr("length:"));
}

void XWTIKZOptions::setDoubleArrowHeadIndent()
{
  setExpress(PGFdoublearrowheadindent,tr("double arrow head indent"),tr("length:"));
}

void XWTIKZOptions::setDoubleDistance()
{
  setExpress(PGFdoubledistance,tr("double distance"),tr("distance:"));
}

void XWTIKZOptions::setDoubleDistanceBetweenLineCenters()
{
  setExpress(PGDdoubledistancebetweenlinecenters,tr("double distance between line centers"),tr("distance:"));
}

XWTikzOperation * XWTIKZOptions::takeAt(int i)
{
  cur = i - 1;
  return ops.takeAt(i);
}

void XWTIKZOptions::addAccepting()
{
  XWTikzKey * key = findState();
  QUndoCommand * cmd = 0;
  if (key)
    cmd = new XWTikzSetKey(key, PGFaccepting);
  else 
  {
    key = new XWTikzKey(graphic, PGFaccepting,this);
    cmd = new XWTikzAddOption(this,cur+1,key);
  }
    
  graphic->push(cmd);
}

void XWTIKZOptions::addAcceptingAbove()
{
  XWTikzKey * key = findState();
  QUndoCommand * cmd = 0;
  if (key)
    cmd = new XWTikzSetKey(key, PGFacceptingabove);
  else 
  {
    key = new XWTikzKey(graphic, PGFacceptingabove,this);
    cmd = new XWTikzAddOption(this,cur+1,key);
  }
    
  graphic->push(cmd);
}

void XWTIKZOptions::addAcceptingBelow()
{
  XWTikzKey * key = findState();
  QUndoCommand * cmd = 0;
  if (key)
    cmd = new XWTikzSetKey(key, PGFacceptingbelow);
  else 
  {
    key = new XWTikzKey(graphic, PGFacceptingbelow,this);
    cmd = new XWTikzAddOption(this,cur+1,key);
  }
    
  graphic->push(cmd);
}

void XWTIKZOptions::addAcceptingByDouble()
{
  XWTikzKey * key = findState();
  QUndoCommand * cmd = 0;
  if (key)
    cmd = new XWTikzSetKey(key, PGFacceptingbydouble);
  else 
  {
    key = new XWTikzKey(graphic, PGFacceptingbydouble,this);
    cmd = new XWTikzAddOption(this,cur+1,key);
  }
    
  graphic->push(cmd);
}

void XWTIKZOptions::addAcceptingLeft()
{
  XWTikzKey * key = findState();
  QUndoCommand * cmd = 0;
  if (key)
    cmd = new XWTikzSetKey(key, PGFacceptingleft);
  else 
  {
    key = new XWTikzKey(graphic, PGFacceptingleft,this);
    cmd = new XWTikzAddOption(this,cur+1,key);
  }
    
  graphic->push(cmd);
}

void XWTIKZOptions::addAcceptingRight()
{
  XWTikzKey * key = findState();
  QUndoCommand * cmd = 0;
  if (key)
    cmd = new XWTikzSetKey(key, PGFacceptingright);
  else 
  {
    key = new XWTikzKey(graphic, PGFacceptingright,this);
    cmd = new XWTikzAddOption(this,cur+1,key);
  }
    
  graphic->push(cmd);
}

void XWTIKZOptions::addAttribute()
{
  XWTikzKey * key = findEntityRelationship();
  QUndoCommand * cmd = 0;
  if (key)
    cmd = new XWTikzSetKey(key, PGFattribute);
  else 
  {
    key = new XWTikzKey(graphic, PGFattribute,this);
    cmd = new XWTikzAddOption(this,cur+1,key);
  }
    
  graphic->push(cmd);
}

void XWTIKZOptions::addCircuit()
{
  XWTikzKey * key = new XWTikzKey(graphic, PGFcircuit,this);
  QUndoCommand * cmd = new XWTikzAddOption(this,cur+1,key);
  graphic->push(cmd);
}

void XWTIKZOptions::addCircuiteeIEC()
{
  XWTikzKey * key = findPictureType();
  QUndoCommand * cmd = 0;
  if (key)
    cmd = new XWTikzSetKey(key, PGFcircuiteeIEC);
  else 
  {
    key = new XWTikzKey(graphic, PGFcircuiteeIEC,this);
    cmd = new XWTikzAddOption(this,cur+1,key);
  }
    
  graphic->push(cmd);
}

void XWTIKZOptions::addCircuitLogicCDH()
{
  XWTikzKey * key = findPictureType();
  QUndoCommand * cmd = 0;
  if (key)
    cmd = new XWTikzSetKey(key, PGFcircuitlogicCDH);
  else 
  {
    key = new XWTikzKey(graphic, PGFcircuitlogicCDH,this);
    cmd = new XWTikzAddOption(this,cur+1,key);
  }
    
  graphic->push(cmd);
}

void XWTIKZOptions::addCircuitLogicIEC()
{
  XWTikzKey * key = findPictureType();
  QUndoCommand * cmd = 0;
  if (key)
    cmd = new XWTikzSetKey(key, PGFcircuitlogicIEC);
  else 
  {
    key = new XWTikzKey(graphic, PGFcircuitlogicIEC,this);
    cmd = new XWTikzAddOption(this,cur+1,key);
  }
    
  graphic->push(cmd);
}

void XWTIKZOptions::addCircuitLogicUS()
{
  XWTikzKey * key = findPictureType();
  QUndoCommand * cmd = 0;
  if (key)
    cmd = new XWTikzSetKey(key, PGFcircuitlogicUS);
  else 
  {
    key = new XWTikzKey(graphic, PGFcircuitlogicUS,this);
    cmd = new XWTikzAddOption(this,cur+1,key);
  }
    
  graphic->push(cmd);
}

void XWTIKZOptions::addConcept()
{
  XWTikzKey * key = findConcept();
  QUndoCommand * cmd = 0;
  if (key)
    cmd = new XWTikzSetKey(key, PGFconcept);
  else 
  {
    key = new XWTikzKey(graphic, PGFconcept,this);
    cmd = new XWTikzAddOption(this,cur+1,key);
  }
    
  graphic->push(cmd);
}

void XWTIKZOptions::addConnectSpies()
{
  XWTikzOperation * op = find(PGFspyusingoutlines);
  if (!op)
    op = find(PGFspyusingoverlays);
  if (op)
  {
    XWTIKZOptions * spy = (XWTIKZOptions*)(op);
    XWTikzKey * key = new XWTikzKey(graphic, PGFconnectspies,spy);
    QUndoCommand * cmd = new XWTikzAddOption(spy,spy->cur+1,key);
    graphic->push(cmd);
  }
}

void XWTIKZOptions::addEntity()
{
  XWTikzKey * key = findEntityRelationship();
  QUndoCommand * cmd = 0;
  if (key)
    cmd = new XWTikzSetKey(key, PGFentity);
  else 
  {
    key = new XWTikzKey(graphic, PGFentity,this);
    cmd = new XWTikzAddOption(this,cur+1,key);
  }
    
  graphic->push(cmd);
}

void XWTIKZOptions::addExtraConcept()
{
  XWTikzKey * key = findConcept();
  QUndoCommand * cmd = 0;
  if (key)
    cmd = new XWTikzSetKey(key, PGFextraconcept);
  else 
  {
    key = new XWTikzKey(graphic, PGFextraconcept,this);
    cmd = new XWTikzAddOption(this,cur+1,key);
  }
  graphic->push(cmd);
}

void XWTIKZOptions::addHugeCircuitSymbols()
{
  XWTikzKey * key = findCircuitSymbols();
  QUndoCommand * cmd = 0;
  if (key)
    cmd = new XWTikzSetKey(key, PGFhugecircuitsymbols);
  else 
  {
    key = new XWTikzKey(graphic, PGFhugecircuitsymbols,this);
    cmd = new XWTikzAddOption(this,cur+1,key);
  }
  graphic->push(cmd);
}

void XWTIKZOptions::addHugeMindmap()
{
  XWTikzKey * key = findMindmap();
  QUndoCommand * cmd = 0;
  if (key)
    cmd = new XWTikzSetKey(key, PGFhugemindmap);
  else 
  {
    key = new XWTikzKey(graphic, PGFhugemindmap,this);
    cmd = new XWTikzAddOption(this,cur+1,key);
  }
  graphic->push(cmd);
}

void XWTIKZOptions::addInitial()
{
  XWTikzKey * key = findState();
  QUndoCommand * cmd = 0;
  if (key)
    cmd = new XWTikzSetKey(key, PGFinitial);
  else 
  {
    key = new XWTikzKey(graphic, PGFinitial,this);
    cmd = new XWTikzAddOption(this,cur+1,key);
  }
  graphic->push(cmd);
}

void XWTIKZOptions::addInitialAbove()
{
  XWTikzKey * key = findState();
  QUndoCommand * cmd = 0;
  if (key)
    cmd = new XWTikzSetKey(key, PGFinitialabove);
  else 
  {
    key = new XWTikzKey(graphic, PGFinitialabove,this);
    cmd = new XWTikzAddOption(this,cur+1,key);
  }
  graphic->push(cmd);
}

void XWTIKZOptions::addInitialBelow()
{
  XWTikzKey * key = findState();
  QUndoCommand * cmd = 0;
  if (key)
    cmd = new XWTikzSetKey(key, PGFinitialbelow);
  else 
  {
    key = new XWTikzKey(graphic, PGFinitialbelow,this);
    cmd = new XWTikzAddOption(this,cur+1,key);
  }
  graphic->push(cmd);
}

void XWTIKZOptions::addInitialByDiamond()
{
  XWTikzKey * key = findState();
  QUndoCommand * cmd = 0;
  if (key)
    cmd = new XWTikzSetKey(key, PGFinitialbydiamond);
  else 
  {
    key = new XWTikzKey(graphic, PGFinitialbydiamond,this);
    cmd = new XWTikzAddOption(this,cur+1,key);
  }
  graphic->push(cmd);
}

void XWTIKZOptions::addInitialLeft()
{
  XWTikzKey * key = findState();
  QUndoCommand * cmd = 0;
  if (key)
    cmd = new XWTikzSetKey(key, PGFinitialleft);
  else 
  {
    key = new XWTikzKey(graphic, PGFinitialleft,this);
    cmd = new XWTikzAddOption(this,cur+1,key);
  }
  graphic->push(cmd);
}

void XWTIKZOptions::addInitialRight()
{
  XWTikzKey * key = findState();
  QUndoCommand * cmd = 0;
  if (key)
    cmd = new XWTikzSetKey(key, PGFinitialright);
  else 
  {
    key = new XWTikzKey(graphic, PGFinitialright,this);
    cmd = new XWTikzAddOption(this,cur+1,key);
  }
  graphic->push(cmd);
}

void XWTIKZOptions::addInputInverted()
{
  XWTikzOperation * op = find(PGFinputs);
  if (op)
  {
    QUndoCommand * cmd = new XWTikzAddInput((XWTikzInpus*)op, QChar('i'));
    graphic->push(cmd);
  }
  else
  {
    XWTikzInpus * i = new XWTikzInpus(graphic,this);
    i->append(QChar('i'));
    QUndoCommand * cmd = new XWTikzAddOption(this,cur+1,i);
    graphic->push(cmd);
  }
}

void XWTIKZOptions::addInputNormal()
{
  XWTikzOperation * op = find(PGFinputs);
  if (op)
  {
    QUndoCommand * cmd = new XWTikzAddInput((XWTikzInpus*)op, QChar('n'));
    graphic->push(cmd);
  }
  else
  {
    XWTikzInpus * i = new XWTikzInpus(graphic,this);
    i->append(QChar('n'));
    QUndoCommand * cmd = new XWTikzAddOption(this,cur+1,i);
    graphic->push(cmd);
  }
}

void XWTIKZOptions::addLargeCircuitSymbols()
{
  XWTikzKey * key = findCircuitSymbols();
  QUndoCommand * cmd = 0;
  if (key)
    cmd = new XWTikzSetKey(key, PGFlargecircuitsymbols);
  else 
  {
    key = new XWTikzKey(graphic, PGFlargecircuitsymbols,this);
    cmd = new XWTikzAddOption(this,cur+1,key);
  }
  graphic->push(cmd);
}

void XWTIKZOptions::addLargeMindmap()
{
  XWTikzKey * key = findMindmap();
  QUndoCommand * cmd = 0;
  if (key)
    cmd = new XWTikzSetKey(key, PGFlargemindmap);
  else 
  {
    key = new XWTikzKey(graphic, PGFlargemindmap,this);
    cmd = new XWTikzAddOption(this,cur+1,key);
  }
  graphic->push(cmd);
}

void XWTIKZOptions::addMediumCircuitSymbols()
{
  XWTikzKey * key = findCircuitSymbols();
  QUndoCommand * cmd = 0;
  if (key)
    cmd = new XWTikzSetKey(key, PGFmediumcircuitsymbols);
  else 
  {
    key = new XWTikzKey(graphic, PGFmediumcircuitsymbols,this);
    cmd = new XWTikzAddOption(this,cur+1,key);
  }
  graphic->push(cmd);
}

void XWTIKZOptions::addMindmap()
{
  XWTikzKey * key = findMindmap();
  QUndoCommand * cmd = 0;
  if (key)
    cmd = new XWTikzSetKey(key, PGFmindmap);
  else 
  {
    key = new XWTikzKey(graphic, PGFmindmap,this);
    cmd = new XWTikzAddOption(this,cur+1,key);
  }
  graphic->push(cmd);
}

void XWTIKZOptions::addPointDown()
{
  XWTikzKey * key = findPoint();
  QUndoCommand * cmd = 0;
  if (key)
    cmd = new XWTikzSetKey(key, PGFpointdown);
  else 
  {
    key = new XWTikzKey(graphic, PGFpointdown,this);
    cmd = new XWTikzAddOption(this,cur+1,key);
  }
  graphic->push(cmd);
}

void XWTIKZOptions::addPointLeft()
{
  XWTikzKey * key = findPoint();
  QUndoCommand * cmd = 0;
  if (key)
    cmd = new XWTikzSetKey(key, PGFpointleft);
  else 
  {
    key = new XWTikzKey(graphic, PGFpointleft,this);
    cmd = new XWTikzAddOption(this,cur+1,key);
  }
  graphic->push(cmd);
}

void XWTIKZOptions::addPointRight()
{
  XWTikzKey * key = findPoint();
  QUndoCommand * cmd = 0;
  if (key)
    cmd = new XWTikzSetKey(key, PGFpointright);
  else 
  {
    key = new XWTikzKey(graphic, PGFpointright,this);
    cmd = new XWTikzAddOption(this,cur+1,key);
  }
  graphic->push(cmd);
}

void XWTIKZOptions::addPointUp()
{
  XWTikzKey * key = findPoint();
  QUndoCommand * cmd = 0;
  if (key)
    cmd = new XWTikzSetKey(key, PGFpointup);
  else 
  {
    key = new XWTikzKey(graphic, PGFpointup,this);
    cmd = new XWTikzAddOption(this,cur+1,key);
  }
  graphic->push(cmd);
}

void XWTIKZOptions::addRelationship()
{
  XWTikzKey * key = findEntityRelationship();
  QUndoCommand * cmd = 0;
  if (key)
    cmd = new XWTikzSetKey(key, PGFrelationship);
  else 
  {
    key = new XWTikzKey(graphic, PGFrelationship,this);
    cmd = new XWTikzAddOption(this,cur+1,key);
  }
  graphic->push(cmd);
}

void XWTIKZOptions::addSmallCircuitSymbols()
{
  XWTikzKey * key = findCircuitSymbols();
  QUndoCommand * cmd = 0;
  if (key)
    cmd = new XWTikzSetKey(key, PGFsmallcircuitsymbols);
  else 
  {
    key = new XWTikzKey(graphic, PGFsmallcircuitsymbols,this);
    cmd = new XWTikzAddOption(this,cur+1,key);
  }
  graphic->push(cmd);
}

void XWTIKZOptions::addSmallMindmap()
{
  XWTikzKey * key = findMindmap();
  QUndoCommand * cmd = 0;
  if (key)
    cmd = new XWTikzSetKey(key, PGFsmallmindmap);
  else 
  {
    key = new XWTikzKey(graphic, PGFsmallmindmap,this);
    cmd = new XWTikzAddOption(this,cur+1,key);
  }
  graphic->push(cmd);
}

void XWTIKZOptions::addSpyUsingOutlines()
{
  XWTikzOperation * op = find(PGFspyusingoverlays);
  QUndoCommand * cmd = 0;
  if (op)
    cmd = new XWTikzRenameOperation(op, PGFspyusingoutlines);
  else
  {
    op = new XWTikzSpyUsing(graphic,PGFspyusingoutlines,this);
    cmd = new XWTikzAddOption(this,cur+1,op);
  }
  graphic->push(cmd);
}

void XWTIKZOptions::addspyUsingOverlays()
{
  XWTikzOperation * op = find(PGFspyusingoutlines);
  QUndoCommand * cmd = 0;
  if (op)
    cmd = new XWTikzRenameOperation(op, PGFspyusingoverlays);
  else
  {
    op = new XWTikzSpyUsing(graphic,PGFspyusingoverlays,this);
    cmd = new XWTikzAddOption(this,cur+1,op);
  }
  graphic->push(cmd);
}

void XWTIKZOptions::addState()
{
  XWTikzKey * key = findState();
  QUndoCommand * cmd = 0;
  if (key)
    cmd = new XWTikzSetKey(key, PGFstate);
  else 
  {
    key = new XWTikzKey(graphic, PGFstate,this);
    cmd = new XWTikzAddOption(this,cur+1,key);
  }
  graphic->push(cmd);
}

void XWTIKZOptions::addStateWithoutOutput()
{
  XWTikzKey * key = findState();
  QUndoCommand * cmd = 0;
  if (key)
    cmd = new XWTikzSetKey(key, PGFstatewithoutoutput);
  else 
  {
    key = new XWTikzKey(graphic, PGFstatewithoutoutput,this);
    cmd = new XWTikzAddOption(this,cur+1,key);
  }
  graphic->push(cmd);
}

void XWTIKZOptions::addStateWithOutput()
{
  XWTikzKey * key = findState();
  QUndoCommand * cmd = 0;
  if (key)
    cmd = new XWTikzSetKey(key, PGFstatewithoutput);
  else 
  {
    key = new XWTikzKey(graphic, PGFstatewithoutput,this);
    cmd = new XWTikzAddOption(this,cur+1,key);
  }
  graphic->push(cmd);
}

void XWTIKZOptions::addTinyCircuitSymbols()
{
  XWTikzKey * key = findCircuitSymbols();
  QUndoCommand * cmd = 0;
  if (key)
    cmd = new XWTikzSetKey(key, PGFtinycircuitsymbols);
  else 
  {
    key = new XWTikzKey(graphic, PGFtinycircuitsymbols,this);
    cmd = new XWTikzAddOption(this,cur+1,key);
  }
  graphic->push(cmd);
}

void XWTIKZOptions::addTransformShape()
{
  XWTikzKey * k = getKey(PGFtransformshape);
  if (!k)
  {
    k = new XWTikzKey(graphic,PGFtransformshape,this);
    QUndoCommand * cmd = new XWTikzAddOption(this,cur+1,k);
    graphic->push(cmd);
  }
}

void XWTIKZOptions::removeInput()
{
  XWTikzOperation * op = find(PGFinputs);
  if (op)
  {
    QUndoCommand * cmd = new XWTikzRemoveInput((XWTikzInpus*)op);
    graphic->push(cmd);
  }
}

void XWTIKZOptions::removeTransformShape()
{
  XWTikzKey * k = getKey(PGFtransformshape);
  if (k)
  {
    int i = ops.indexOf(k);
    QUndoCommand * cmd = new XWTikzRemoveOption(this,i);
    graphic->push(cmd);
  }
}

void XWTIKZOptions::setAmpere()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("ampere", "ampere");
}

void XWTIKZOptions::setAmpereOppsite()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("ampere'", "ampere");
}

void XWTIKZOptions::setAmpereSloped()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("ampere sloped", "ampere");
}

void XWTIKZOptions::setAmpereSlopedOppsite()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("ampere' sloped", "ampere");
}

void XWTIKZOptions::setAnchor()
{
  XWTikzValue * r = getValue(PGFanchor);
  XWTikzAnchorDialog dlg;
  if (r)
  {
    int j = (int)(r->getValue());
    dlg.setAnchor(j);
  }

  if (dlg.exec() == QDialog::Accepted)
  {
    int j = dlg.getAnchor();
    QUndoCommand * cmd = 0;
    if (r)
      cmd = new XWTikzSetValue(r,j);
    else
    {
      r = new XWTikzValue(graphic,PGFanchor,this);
      r->setValue(j);
      cmd = new XWTikzAddOption(this,cur+1,r);
    }    
    graphic->push(cmd);
  }
}

void XWTIKZOptions::setAngle()
{
  setExpress(PGFangle,tr("angle"),tr("angle:"));
}

void XWTIKZOptions::setArrowBoxTipAngle()
{
  setExpress(PGFarrowboxtipangle,tr("arrow box tip angle"),tr("angle:"));
}

void XWTIKZOptions::setArrowBoxHeadExtend()
{
  setExpress(PGFarrowboxheadextend,tr("arrow box head extend"),tr("length:"));
}

void XWTIKZOptions::setArrowBoxHeadIndent()
{
  setExpress(PGFarrowboxheadindent,tr("arrow box head indent"),tr("length:"));
}

void XWTIKZOptions::setArrowBoxShaftWidth()
{
  setExpress(PGFarrowboxshaftwidth,tr("arrow box shaft width"),tr("length:"));
}

void XWTIKZOptions::setArrowBoxNorthArrow()
{
  setExpress(PGFarrowboxnortharrow,tr("arrow box north arrow"),tr("distance:"));
}

void XWTIKZOptions::setArrowBoxSouthArrow()
{
  setExpress(PGFarrowboxsoutharrow,tr("arrow box south arrow"),tr("distance:"));
}

void XWTIKZOptions::setArrowBoxEastArrow()
{
  setExpress(PGFarrowboxeastarrow,tr("arrow box east arrow"),tr("distance:"));
}

void XWTIKZOptions::setArrowBoxWestArrow()
{
  setExpress(PGFarrowboxwestarrow,tr("arrow box west arrow"),tr("distance:"));
}

void XWTIKZOptions::setArrows()
{
  XWTikzArrows * a = getArrows();
  XWTikzArrowDialog dlg;
  if (a)
  {
    int i = a->getEndArrow();
    dlg.setEndArrow(i);
    i = a->getStartArrow();
    dlg.setStartArrow(i);
  }

  if (dlg.exec() == QDialog::Accepted)
  {
    int e = dlg.getEndArrow();
    int s = dlg.getStartArrow();
    QUndoCommand * cmd = 0;
    if (a)
      cmd = new XWTikzSetArrow(a,s,e);
    else
    {
      a = new XWTikzArrows(graphic,this);
      a->setEndArrow(e);
      a->setStartArrow(s);
      cmd = new XWTikzAddOption(this,cur+1,a);
    }
    graphic->push(cmd);
  }
}

void XWTIKZOptions::setAt()
{
  setCoord(PGFat,tr("at"));
}

void XWTIKZOptions::setBallColor()
{
  setColor(PGFballcolor,tr("ball color"));
}

void XWTIKZOptions::setBottomColor()
{
  setColor(PGFbottomcolor,tr("bottom color"));
}

void XWTIKZOptions::setCalloutAbsolutePointer()
{
  setCoord(PGFcalloutabsolutepointer, tr("callout absolute pointer"));
}

void XWTIKZOptions::setCalloutRelativePointer()
{
  setCoord(PGFcalloutrelativepointer, tr("callout relative pointer"));
}

void XWTIKZOptions::setCalloutPointerArc()
{
  setExpress(PGFcalloutpointerarc, tr("callout pointer arc"), tr("angle:"));
}

void XWTIKZOptions::setCalloutPointerSegments()
{
  setExpress(PGFcalloutpointersegments, tr("callout pointer segments"), tr("number:"));
}

void XWTIKZOptions::setCalloutPointerShorten()
{
  setExpress(PGFcalloutpointershorten, tr("callout pointer shorten"), tr("distance:"));
}

void XWTIKZOptions::setCalloutPointerWidth()
{
  setExpress(PGFcalloutpointerwidth, tr("callout pointer width"),tr("length:"));
}

void XWTIKZOptions::setChamferedRectangleAngle()
{
  setExpress(PGFchamferedrectangleangle, tr("chamfered rectangle angle"),tr("angle:"));
}

void XWTIKZOptions::setChamferedRectangleSep()
{
  setExpress(PGFchamferedrectanglesep, tr("chamfered rectangle sep"),tr("length:"));
}

void XWTIKZOptions::setCircuiteeSymbol()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  int symbol = -1;
  if (op)
    symbol = op->getKeyWord();

  XWTikzCircuiteeDialog dlg(PGFcircuiteeIEC);
  dlg.setShape(symbol);
  if (dlg.exec() == QDialog::Accepted)
  {
    symbol = dlg.getShape();
    QUndoCommand * cmd = 0;
    if (op)
      cmd = new XWTikzSetKey(op,symbol);
    else
    {
      op = new XWTikzCircuitSymbol(graphic,symbol,this);
      cmd = new XWTikzAddOption(this,cur+1,op);
    }

    graphic->push(cmd);
  }
}

void XWTIKZOptions::setCircuitLogicSymbol()
{
  XWTikzCircuitSymbol * op = findCircuitLogicSymbol();
  int symbol = -1;
  if (op)
    symbol = op->getKeyWord();

  XWTikzCircuitLogicDialog dlg(PGFcircuitlogicUS);
  dlg.setShape(symbol);
  if (dlg.exec() == QDialog::Accepted)
  {
    symbol = dlg.getShape();
    QUndoCommand * cmd = 0;
    if (op)
      cmd = new XWTikzSetKey(op,symbol);
    else
    {
      op = new XWTikzCircuitSymbol(graphic,symbol,this);
      cmd = new XWTikzAddOption(this,cur+1,op);
    }

    graphic->push(cmd);
  }
}

void XWTIKZOptions::setCircuitSymbolSize()
{
  XWTikzSize * s = (XWTikzSize*)(find(PGFcircuitsymbolsize));
  XWTikzDomainDialog dlg(tr("circuit symbol size"),tr("width:"),tr("height:"));
  if (s)
  {
    QString w,h;
    s->getSize(w,h);
    dlg.setStart(w);
    dlg.setEnd(h);
  }

  if (dlg.exec() == QDialog::Accepted)
  {
    QString w = dlg.getStart();
    QString h = dlg.getEnd();
    QUndoCommand * cmd = 0;
    if (s)
      cmd = new XWTikzSetSize(s,w,h);
    else
    {
      s = new XWTikzSize(graphic,PGFcircuitsymbolsize,this);
      s->setSize(w,h);
      cmd = new XWTikzAddOption(this,cur+1,s);
    }    
    graphic->push(cmd);
  }
}

void XWTIKZOptions::setCircuitSymbolUnit()
{
  setExpress(PGFcircuitsymbolunit, tr("circuit symbol unit"),tr("dimension:"));
}

void XWTIKZOptions::setCircularSectorAngle()
{
  setExpress(PGFcircularsectorangle,tr("circular sector angle"),tr("angle:"));
}

void XWTIKZOptions::setCloudPuffArc()
{
  setExpress(PGFcloudpuffarc,tr("cloud puff arc"),tr("angle:"));
}

void XWTIKZOptions::setCloudPuffs()
{
  setExpress(PGFcloudpuffs,tr("cloud puffs"),tr("integer:"));
}

void XWTIKZOptions::setCM()
{
  XWTikzcm * cm = getcm();
  XWTikzTransformDialog dlg;
  if (cm)
  {
    QString str= cm->getA();
    dlg.setA(str);
    str= cm->getB();
    dlg.setB(str);
    str= cm->getC();
    dlg.setC(str);
    str= cm->getD();
    dlg.setD(str);
    str= cm->getCoord();
    dlg.setCoord(str);
  }

  if (dlg.exec() == QDialog::Accepted)
  {
    QString a = dlg.getA();
    QString b = dlg.getB();
    QString c = dlg.getC();
    QString d = dlg.getD();
    QString coord = dlg.getCoord();
    QUndoCommand * cmd = 0;
    if (cm)
      cmd = new XWTikzSetcm(cm,a,b,c,d,coord);
    else
    {
      cm = new XWTikzcm(graphic,this);
      cm->setA(a);
      cm->setB(b);
      cm->setC(c);
      cm->setD(d);
      cm->setCoord(coord);
      cmd = new XWTikzAddOption(this,cur+1,cm);
    }
    graphic->push(cmd);
  }
}

void XWTIKZOptions::setColor()
{
  setColor(PGFcolor,tr("color"));
}

void XWTIKZOptions::setConceptColor()
{
  setColor(PGFconceptcolor,tr("concept color"));
}

void XWTIKZOptions::setConnectSpies()
{
  XWTikzOperation * op = find(PGFconnectspies);
  if (op)
    return ;

  XWTikzKey * key = new XWTikzKey(graphic,PGFconnectspies,this);
  QUndoCommand * cmd = new XWTikzAddOption(this,cur+1,key);
  graphic->push(cmd);
}

void XWTIKZOptions::setCoulomb()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("coulomb", "coulomb");
}

void XWTIKZOptions::setCoulombOppsite()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("coulomb'", "coulomb");
}

void XWTIKZOptions::setCoulombSloped()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("coulomb sloped", "coulomb");
}

void XWTIKZOptions::setCoulombSlopedOppsite()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("coulomb' sloped", "coulomb");
}

void XWTIKZOptions::setCylinderEndFill()
{
  setColor(PGFcylinderendfill,tr("cylinder end fill"));
}

void XWTIKZOptions::setCylinderBodyFill()
{
  setColor(PGFcylinderbodyfill,tr("cylinder body fill"));
}

void XWTIKZOptions::setDartAngles()
{
  setTwoValue(tr("dart angles"), 
              PGFdarttipangle,tr("tip angle"),
              PGFdarttailangle,tr("tail angle"));
}

void XWTIKZOptions::setDash()
{
  XWTikzKey * key = findDash();
  XWTikzDashDialog dlg;
  if (key)
  {
    int d = key->getKeyWord();
    dlg.setDash(d);
  }

  if (dlg.exec() == QDialog::Accepted)
  {
    int d = dlg.getDash();
    QUndoCommand * cmd = 0;
    if (key)
      cmd = new XWTikzSetKey(key,d);
    else
    {
      key = new XWTikzKey(graphic,d,this);
      cmd = new XWTikzAddOption(this,cur+1,key);
    }
    graphic->push(cmd);
  }
}

void XWTIKZOptions::setDecoration()
{
  setDecoration(PGFname);
}

void XWTIKZOptions::setDefaultArrow()
{
  XWTikzValue * a = getValue(PGFarrowdefault);
  XWTikzArrowDialog dlg;
  if (a)
  {
    int i = a->getValue();
    dlg.setEndArrow(i);
    dlg.setStartArrow(i);
  }

  if (dlg.exec() == QDialog::Accepted)
  {
    int e = dlg.getEndArrow();
    int s = dlg.getStartArrow();
    QString ar = QString("%1").arg(s);
    QUndoCommand * cmd = 0;
    if (a)
      cmd = new XWTikzSetExpress(a,ar);
    else
    {
      a = new XWTikzValue(graphic,PGFarrowdefault,this);
      a->setValue(s);
      cmd = new XWTikzAddOption(this,cur+1,a);
    }
    graphic->push(cmd);
  }
}

void XWTIKZOptions::setDomain()
{
  XWTikzDomain * d = getDomain();
  XWTikzDomainDialog dlg(tr("domain"),tr("start:"),tr("end:"));
  if (d)
  {
    QString tmp = d->getStart();
    dlg.setStart(tmp);
    tmp = d->getEnd();
    dlg.setEnd(tmp);
  }

  if (dlg.exec() == QDialog::Accepted)
  {
    QString s = dlg.getStart();
    QString e = dlg.getEnd();
    QUndoCommand * cmd = 0;
    if (d)
      cmd = new XWTikzSetDomain(d,s,e);
    else
    {
      d = new XWTikzDomain(graphic,this);
      d->setStart(s);
      d->setEnd(e);
      cmd = new XWTikzAddOption(this,cur+1,d);
    }    
    graphic->push(cmd);
  }
}

void XWTIKZOptions::setDrawColor()
{
  setColor(PGFdraw,tr("draw color"));
}

void XWTIKZOptions::setDrawOpacity()
{
  XWTikzValue * r = getValue(PGFdrawopacity);

  XWTikzDrawOpacityDialog dlg;
  if (r)
    dlg.setOpacity(r->getValue());

  if (dlg.exec() == QDialog::Accepted)
  {
    double o = dlg.getOpacity();
    QUndoCommand * cmd = 0;
    if (r)
      cmd = new XWTikzSetValue(r,o);
    else
    {
      r = new XWTikzValue(graphic,PGFdrawopacity,this);
      r->setValue(o);
      cmd = new XWTikzAddOption(this,cur+1,r);
    }
    graphic->push(cmd);
  }
}

void XWTIKZOptions::setEndAngle()
{
  setExpress(PGFendangle,tr("arc"),tr("end angle:"));
}

void XWTIKZOptions::setFadingAngle()
{
  setExpress(PGFfadingangle,tr("fading angle"),tr("angle:"));
}

void XWTIKZOptions::setFarad()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("farad", "farad");
}

void XWTIKZOptions::setFaradOppsite()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("farad'", "farad");
}

void XWTIKZOptions::setFaradSloped()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("farad sloped", "farad");
}

void XWTIKZOptions::setFaradSlopedOppsite()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("farad' sloped", "farad");
}

void XWTIKZOptions::setFillColor()
{
  setColor(PGFfill,tr("fill color"));
}

void XWTIKZOptions::setFillOpacity()
{
  XWTikzValue * r = getValue(PGFfillopacity);

  XWTikzFillOpacityDialog dlg;
  if (r)
    dlg.setOpacity(r->getValue());

  if (dlg.exec() == QDialog::Accepted)
  {
    double o = dlg.getOpacity();
    QUndoCommand * cmd = 0;
    if (r)
      cmd = new XWTikzSetValue(r,o);
    else
    {
      r = new XWTikzValue(graphic,PGFfillopacity,this);
      r->setValue(o);
      cmd = new XWTikzAddOption(this,cur+1,r);
    }
    graphic->push(cmd);
  }
}

void XWTIKZOptions::setHenry()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("henry", "henry");
}

void XWTIKZOptions::setHenryOppsite()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("henry'", "henry");
}

void XWTIKZOptions::setHenrySloped()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("henry sloped", "henry");
}

void XWTIKZOptions::setHenrySlopedOppsite()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("henry' sloped", "henry");
}

void XWTIKZOptions::setHertz()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("hertz", "hertz");
}

void XWTIKZOptions::setHertzOppsite()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("hertz'", "hertz");
}

void XWTIKZOptions::setHertzSloped()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("hertz sloped", "hertz");
}

void XWTIKZOptions::setHertzSlopedOppsite()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("hertz' sloped", "hertz");
}

void XWTIKZOptions::setInfo()
{
  setInfo("info",PGFinfo);
}

void XWTIKZOptions::setInfoMissingAngle()
{
  setInfo("info'",PGFinfomissingangle);
}

void XWTIKZOptions::setInfoSloped()
{
  setInfo("info sloped",PGFinfosloped);
}

void XWTIKZOptions::setInfoSlopedMissingAngle()
{
  setInfo("info' sloped",PGFinfomissinganglesloped);
}

void XWTIKZOptions::setInnerColor()
{
  setColor(PGFinnercolor,tr("inner color"));
}

void XWTIKZOptions::setInnerSep()
{
  setExpress(PGFinnersep,tr("inner separation"),tr("dimension:"));
}

void XWTIKZOptions::setIsoscelesTriangleApexAngle()
{
  setExpress(PGFisoscelestriangleapexangle,tr("isosceles triangle apex angle"),tr("angle:"));
}

void XWTIKZOptions::setKiteVertexAngles()
{
  setTwoValue(tr("kite vertex angles"),
                 PGFkiteuppervertexangle,tr("upper vertex angle:"),
                 PGFkitelowervertexangle,tr("lower vertex angle:"));
}

void XWTIKZOptions::setLabel()
{
  setLabel("label", PGFlabel);
}

void XWTIKZOptions::setLeftColor()
{
  setColor(PGFleftcolor,tr("left color"));
}

void XWTIKZOptions::setLineCap()
{
  XWTikzValue * r = getValue(PGFlinecap);
  XWTikzLineCapDialog dlg;
  if (r)
  {
    int j = (int)(r->getValue());
    dlg.setCap(j);
  }

  if (dlg.exec() == QDialog::Accepted)
  {
    int j = dlg.getCap();
    QUndoCommand * cmd = 0;
    if (r)
      cmd = new XWTikzSetValue(r,j);
    else
    {
      r = new XWTikzValue(graphic,PGFlinecap,this);
      r->setValue(j);
      cmd = new XWTikzAddOption(this,cur+1,r);
    }    
    graphic->push(cmd);
  }
}

void XWTIKZOptions::setLineJoin()
{
  XWTikzValue * r = getValue(PGFlinejoin);
  XWTikzLineJoinDialog dlg;
  if (r)
  {
    int j = (int)(r->getValue());
    dlg.setJion(j);
  }

  if (dlg.exec() == QDialog::Accepted)
  {
    int j = dlg.getJion();
    QUndoCommand * cmd = 0;
    if (r)
      cmd = new XWTikzSetValue(r,j);
    else
    {
      r = new XWTikzValue(graphic,PGFlinejoin,this);
      r->setValue(j);
      cmd = new XWTikzAddOption(this,cur+1,r);
    }    
    graphic->push(cmd);
  }
}

void XWTIKZOptions::setLineWidth()
{
  setExpress(PGFlinewidth,tr("line width"),tr("dimension:"));
}

void XWTIKZOptions::setLineWidthStyle()
{
  XWTikzKey * key = findLineWidth();
  XWTikzLineWidthDialog dlg;
  if (key)
  {
    int d = key->getKeyWord();
    dlg.setLineWidth(d);
  }

  if (dlg.exec() == QDialog::Accepted)
  {
    int d = dlg.getLineWidth();
    QUndoCommand * cmd = 0;
    if (key)
      cmd = new XWTikzSetKey(key,d);
    else
    {
      key = new XWTikzKey(graphic,d,this);
      cmd = new XWTikzAddOption(this,cur+1,key);
    }
    graphic->push(cmd);
  }
}

void XWTIKZOptions::setLowerLeftColor()
{
  setColor(PGFlowerleft,tr("lower left color"));
}

void XWTIKZOptions::setLowerRightColor()
{
  setColor(PGFlowerright,tr("lower right color"));
}

void XWTIKZOptions::setMagneticTapeTail()
{
  setExpress(PGFmagnetictapetail,tr("magnetic tape tail"),tr("proportion:"));
}

void XWTIKZOptions::setMagneticTapeTailExtend()
{
  setExpress(PGFmagnetictapetailextend,tr("magnetic tape tail extend"),tr("distance:"));
}

void XWTIKZOptions::setMagnification()
{
  XWTIKZOptions * spy = (XWTIKZOptions*)(find(PGFspyusingoutlines));
  if (!spy)
    spy = (XWTIKZOptions*)(find(PGFspyusingoutlines));

  if (!spy)
    return ;

  spy->setExpress(PGFmagnification,tr("spy"),tr("magnification:"));
}

void XWTIKZOptions::setMagnifyingGlassHandleAngleFill()
{
  setExpress(PGFminimumsize,tr("magnifying glass handle angle fill"),tr("degree:"));
}

void XWTIKZOptions::setMagnifyingGlassHandleAngleAspect()
{
  setExpress(PGFminimumsize,tr("magnifying glass handle angle aspect"),tr("factor:"));
}

void XWTIKZOptions::setMiddleColor()
{
  setColor(PGFmiddlecolor,tr("middle color"));
}

void XWTIKZOptions::setMinimumSize()
{
  setExpress(PGFminimumsize,tr("minimum height and width"),tr("dimension:"));
}

void XWTIKZOptions::setMiterLimit()
{
  setExpress(PGFmiterlimit,tr("miter limit"),tr("miter limit:"));
}

void XWTIKZOptions::setName()
{
  setExpress(PGFnamepath,tr("name"),tr("name:"));
}

void XWTIKZOptions::setNamePath()
{
  setExpress(PGFnamepath,tr("name path"),tr("path name:"));
}

void XWTIKZOptions::setOuterColor()
{
  setColor(PGFoutercolor,tr("outer color"));
}

void XWTIKZOptions::setOuterSep()
{
  setExpress(PGFoutersep,tr("outer separation"),tr("dimension:"));
}

void XWTIKZOptions::setOf()
{
  XWTikzDomainDialog dlg(tr("intersections of"),tr("path name 1:"),tr("path name 2:"));
  XWTikzOf * of = 0;
  XWTikzNameIntersections * ni = (XWTikzNameIntersections*)find((PGFnameintersections));
  if (ni)
  {
    of = (XWTikzOf*)(ni->find(PGFof));
    if (of)
    {
      QString path1 = of->getFirst();
      QString path2 = of->getSecond();
      dlg.setStart(path1);
      dlg.setEnd(path2);
    }
  }
    
  if (dlg.exec() == QDialog::Accepted)
  {
    QString path1 = dlg.getStart();
    QString path2 = dlg.getEnd();
    if (!ni)
    {
      ni = new XWTikzNameIntersections(graphic,this);
      of = new XWTikzOf(graphic,ni);
      of->setPath1(path1);
      of->setPath2(path2);
      ni->ops << of;
      QUndoCommand * cmd = new XWTikzAddOption(this,cur+1,ni);
      graphic->push(cmd);
    }
    else if (!of)
    {
      of = new XWTikzOf(graphic,ni);
      of->setPath1(path1);
      of->setPath2(path2);
      QUndoCommand * cmd = new XWTikzAddOption(ni,ni->cur+1,of);
      graphic->push(cmd);
    }
    else
    {
      XWTikzSetOf * cmd = new XWTikzSetOf(of,path1,path2);
      graphic->push(cmd);
    }
  }
}

void XWTIKZOptions::setohm()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("ohm", "ohm");
}

void XWTIKZOptions::setohmOppsite()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("ohm'", "ohm");
}

void XWTIKZOptions::setohmSloped()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("ohm sloped", "ohm");
}

void XWTIKZOptions::setohmSlopedOppsite()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("ohm' sloped", "ohm");
}

void XWTIKZOptions::setPathFading()
{
  XWTikzValue * r = getValue(PGFpathfading);
  XWTikzFadingDialog dlg;
  if (r)
  {
    int j = (int)(r->getValue());
    dlg.setFading(j);
  }

  if (dlg.exec() == QDialog::Accepted)
  {
    int j = dlg.getFading();
    QUndoCommand * cmd = 0;
    if (r)
      cmd = new XWTikzSetValue(r,j);
    else
    {
      r = new XWTikzValue(graphic,PGFpathfading,this);
      r->setValue(j);
      cmd = new XWTikzAddOption(this,cur+1,r);
    }    
    graphic->push(cmd);
  }
}

void XWTIKZOptions::setPattern()
{
  XWTikzValue * r = getValue(PGFpattern);
  XWTikzPatternDialog dlg;
  if (r)
  {
    int j = (int)(r->getValue());
    dlg.setPattern(j);
  }

  if (dlg.exec() == QDialog::Accepted)
  {
    int j = dlg.getPattern();
    QUndoCommand * cmd = 0;
    if (r)
      cmd = new XWTikzSetValue(r,j);
    else
    {
      r = new XWTikzValue(graphic,PGFpattern,this);
      r->setValue(j);
      cmd = new XWTikzAddOption(this,cur+1,r);
    }    
    graphic->push(cmd);
  }
}

void XWTIKZOptions::setPatternColor()
{
  setColor(PGFpatterncolor,tr("pattern color"));
}

void XWTIKZOptions::setPin()
{
  setLabel("pin", PGFpin);
}

void XWTIKZOptions::setPlane()
{
  XWTikzOperation * op = find(PGFcanvasisplane);
  if (op)
    return ;

  op = new XWTikzKey(graphic, PGFcanvasisplane,this);
  QUndoCommand * cmd = new XWTikzAddOption(this,cur+1,op);
  graphic->push(cmd);
}

void XWTIKZOptions::setPlaneOrigin()
{
  setCoord(PGFplaneorigin,tr("plane origin"));
}

void XWTIKZOptions::setPlaneX()
{
  setCoord(PGFplanex,tr("plane x"));
}

void XWTIKZOptions::setPlaneXYAtZ()
{
  setCoord(PGFcanvasisxyplaneatz,tr("canvas is xy plane at z"));
}

void XWTIKZOptions::setPlaneXZAtY()
{
  setCoord(PGFcanvasisxzplaneaty,tr("canvas is xz plane at y"));
}

void XWTIKZOptions::setPlaneY()
{
  setCoord(PGFplaney,tr("plane y"));
}

void XWTIKZOptions::setPlaneYXAtZ()
{
  setCoord(PGFcanvasisyxplaneatz,tr("canvas is yx plane at z"));
}

void XWTIKZOptions::setPlaneYZAtX()
{
  setCoord(PGFcanvasisyzplaneatx,tr("canvas is yz plane at x"));
}

void XWTIKZOptions::setPlaneZXAtY()
{
  setCoord(PGFcanvasiszxplaneaty,tr("canvas is zx plane at y"));
}

void XWTIKZOptions::setPlaneZYAtX()
{
  setCoord(PGFcanvasiszxplaneaty,tr("canvas is zy plane at x"));
}

void XWTIKZOptions::setPlotHandler()
{
  XWTikzKey * key = findPlotHandler();
  XWTikzPlotHandlerDialog dlg;
  if (key)
  {
    int d = key->getKeyWord();
    dlg.setHandler(d);
  }

  if (dlg.exec() == QDialog::Accepted)
  {
    int d = dlg.getHandler();
    QUndoCommand * cmd = 0;
    if (key)
      cmd = new XWTikzSetKey(key,d);
    else
    {
      key = new XWTikzKey(graphic,d,this);
      cmd = new XWTikzAddOption(this,cur+1,key);
    }
    graphic->push(cmd);
  }
}

void XWTIKZOptions::setPlotMark()
{
  XWTikzValue * r = getValue(PGFmark);
  XWTikzPlotMarkDialog dlg;
  if (r)
  {
    int j = (int)(r->getValue());
    dlg.setMark(j);
  }

  if (dlg.exec() == QDialog::Accepted)
  {
    int j = dlg.getMark();
    QUndoCommand * cmd = 0;
    if (r)
      cmd = new XWTikzSetValue(r,j);
    else
    {
      r = new XWTikzValue(graphic,PGFmark,this);
      r->setValue(j);
      cmd = new XWTikzAddOption(this,cur+1,r);
    }    
    graphic->push(cmd);
  }
}

void XWTIKZOptions::setPos()
{
  setExpress(PGFroundedcorners,tr("place node at"),tr("fraction:"));
}

void XWTIKZOptions::setPost()
{
  setDecoration(PGFpost);
}

void XWTIKZOptions::setPostlength()
{
  setDecorationExpress(PGFpostlength,tr("post length"),tr("dimension:"));
}

void XWTIKZOptions::setPre()
{
  setDecoration(PGFpre);
}

void XWTIKZOptions::setPrelength()
{
  setDecorationExpress(PGFprelength,tr("pre length"),tr("dimension:"));
}

void XWTIKZOptions::setRadius()
{
  setExpress(PGFradius,tr("circle"),tr("radius:"));
}

void XWTIKZOptions::setRaise()
{
  setDecorationExpress(PGFraise,tr("raise"),tr("dimension:"));
}

void XWTIKZOptions::setRandomStarburst()
{
  setExpress(PGFrandomstarburst,tr("random starburst"),tr("integer:"));
}

void XWTIKZOptions::setRegularPolygonSides()
{
  setExpress(PGFregularpolygonsides,tr("regular polygon sides"),tr("integer:"));
}

void XWTIKZOptions::setRightColor()
{
  setColor(PGFrightcolor,tr("right color"));
}

void XWTIKZOptions::setRoundedCorners()
{
  setExpress(PGFroundedcorners,tr("Rounded corners"),tr("inset:"));
}

void XWTIKZOptions::setRoundedRectangleArcLength()
{
  setExpress(PGFroundedcorners,tr("rounded rectangle arc length"),tr("angle:"));
}

void XWTIKZOptions::setRotate()
{
  setExpress(PGFrotate,tr("rotate"),tr("angle:"));
}

void XWTIKZOptions::setRotateAround()
{
  setAround(PGFrotatearound,tr("rotate around"),tr("rotate:"));
}

void XWTIKZOptions::setScale()
{
  setExpress(PGFscale,tr("scale"),tr("scale:"));
}

void XWTIKZOptions::setScaleAround()
{
  setAround(PGFscalearound,tr("scale around"),tr("scale:"));
}

void XWTIKZOptions::setScopeFading()
{
  XWTikzValue * r = getValue(PGFscopefading);
  XWTikzFadingDialog dlg;
  if (r)
  {
    int j = (int)(r->getValue());
    dlg.setFading(j);
  }

  if (dlg.exec() == QDialog::Accepted)
  {
    int j = dlg.getFading();
    QUndoCommand * cmd = 0;
    if (r)
      cmd = new XWTikzSetValue(r,j);
    else
    {
      r = new XWTikzValue(graphic,PGFscopefading,this);
      r->setValue(j);
      cmd = new XWTikzAddOption(this,cur+1,r);
    }    
    graphic->push(cmd);
  }
}

void XWTIKZOptions::setSiemens()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("siemens", "siemens");
}

void XWTIKZOptions::setSiemensOppsite()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("siemens'", "siemens");
}

void XWTIKZOptions::setSiemensSloped()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("siemens sloped", "siemens");
}

void XWTIKZOptions::setSiemensSlopedOppsite()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("siemens' sloped", "siemens");
}

void XWTIKZOptions::setShade()
{
  XWTikzValue * r = getValue(PGFshading);
  XWTikzShadeDialog dlg;
  if (r)
  {
    int j = (int)(r->getValue());
    dlg.setShade(j);
  }

  if (dlg.exec() == QDialog::Accepted)
  {
    int j = dlg.getShade();
    QUndoCommand * cmd = 0;
    if (r)
      cmd = new XWTikzSetValue(r,j);
    else
    {
      r = new XWTikzValue(graphic,PGFshading,this);
      r->setValue(j);
      cmd = new XWTikzAddOption(this,cur+1,r);
    }    
    graphic->push(cmd);
  }
}

void XWTIKZOptions::setShadeAngle()
{
  setExpress(PGFshadingangle,tr("shading angle"),tr("angle:"));
}

void XWTIKZOptions::setShadow()
{
  XWTIKZOptions * op = findShadow();
  XWTikzShadowDialog dlg;
  if (op)
  {
    int s = op->getKeyWord();
    dlg.setShadow(s);
  }

  if (dlg.exec() == QDialog::Accepted)
  {
    int s = dlg.getShadow();
    QUndoCommand * cmd = 0;
    if (op)
      cmd = new XWTikzSetKey(op,s);
    else
    {
      switch (s)
      {
        default:
          break;

        case PGFgeneralshadow:
          op = new XWTikzGeneralShadow(graphic, this);
          break;

        case PGFdropshadow:
          op = new XWTikzDropShadow(graphic, this);
          break;

        case PGFcirculardropshadow:
          op = new XWTikzCircularDropShadow(graphic, this);
          break;

        case PGFcircularglow:
          op = new XWTikzCircularGlow(graphic, this);
          break;

        case PGFcopyshadow:
          op = new XWTikzCopyShadow(graphic, this);
          break;

        case PGFdoublecopyshadow:
          op = new XWTikzDoubleCopyShadow(graphic, this);
          break;
      }

      cmd = new XWTikzAddOption(this,cur+1,op);
    }

    graphic->push(cmd);
  }
}

void XWTIKZOptions::setShadowScale()
{
  setExpress(PGFshadowscale,tr("shadow scale"),tr("factor:"));
}

void XWTIKZOptions::setShadowXShift()
{
  setExpress(PGFshadowxshift,tr("shadow xshift"),tr("dimension:"));
}

void XWTIKZOptions::setShadowYShift()
{
  setExpress(PGFshadowyshift,tr("shadow yshift"),tr("dimension:"));
}

void XWTIKZOptions::setShape()
{
  XWTikzValue * r = getValue(PGFshape);
  XWTikzShapeDialog dlg;
  XWTikzKey * k = 0;
  if (r)
  {
    int j = (int)(r->getValue());
    dlg.setShape(j);
  }
  else
  {
    k = findShape();
    if (k)
    {
      int j = k->getKeyWord();
      dlg.setShape(j);
    }
  }

  if (dlg.exec() == QDialog::Accepted)
  {
    int j = dlg.getShape();
    QUndoCommand * cmd = 0;
    if (r)
      cmd = new XWTikzSetValue(r,j);
    else if (k)
      cmd = new XWTikzSetKey(k,j);
    else
    {
      k = new XWTikzKey(graphic,j,this);
      cmd = new XWTikzAddOption(this,cur+1,k);
    }    
    graphic->push(cmd);
  }
}

void XWTIKZOptions::setShapeAspect()
{
  setExpress(PGFshapeaspect,tr("shape aspect"),tr("aspect ratio:"));
}

void XWTIKZOptions::setShapeBorderRotate()
{
  setExpress(PGFshapeborderrotate,tr("shape border rotate"),tr("angle:"));
}

void XWTIKZOptions::setShift()
{
  setExpress(PGFshift,tr("shift"),tr("shift:"));
}

void XWTIKZOptions::setSignalPointerAngle()
{
  setExpress(PGFsignalpointerangle,tr("signal pointer angle"),tr("angle:"));
}

void XWTIKZOptions::setSingleArrowTipAngle()
{
  setExpress(PGFsinglearrowtipangle,tr("single arrow tip angle"),tr("angle:"));
}

void XWTIKZOptions::setSingleArrowHeadExtend()
{
  setExpress(PGFsinglearrowheadextend,tr("single arrow head extend"),tr("length:"));
}

void XWTIKZOptions::setSingleArrowHeadIndent()
{
  setExpress(PGFsinglearrowheadindent,tr("single arrow head indent"),tr("length:"));
}

void XWTIKZOptions::setSize()
{
  setExpress(PGFshift,tr("size"),tr("size:"));
}

void XWTIKZOptions::setSortBy()
{
  XWTikzExpressDialog dlg(tr("sort intersections"),tr("sort by:"));
  XWTikzValue * v = 0;
  XWTikzNameIntersections * ni = (XWTikzNameIntersections*)(find(PGFnameintersections));
  if (ni)
  {
    v = ni->getValue(PGFsortby);
    if (v)
    {
      QString str = v->getExpress();
      dlg.setExpress(str);
    }
  }
    
  if (dlg.exec() == QDialog::Accepted)
  {
    QString str = dlg.getExpress();
    if (!ni)
    {
      ni = new XWTikzNameIntersections(graphic,this);
      v = new  XWTikzValue(graphic,PGFsortby,ni);
      v->setExpress(str);
      ni->ops << v;
      QUndoCommand * cmd = new XWTikzAddOption(this,cur+1,ni);
      graphic->push(cmd);
    }
    else if (!v)
    {
      v = new  XWTikzValue(graphic,PGFsortby,ni);
      v->setExpress(str);
      QUndoCommand * cmd = new XWTikzAddOption(ni,ni->cur+1,v);
      graphic->push(cmd);
    }
    else
    {
      XWTikzSetExpress * cmd = new XWTikzSetExpress(v,str);
      graphic->push(cmd);
    }
  }
}

void XWTIKZOptions::setSpyNodeShape()
{
  XWTIKZOptions * spy = (XWTIKZOptions*)(find(PGFspyusingoutlines));
  if (!spy)
    spy = (XWTIKZOptions*)(find(PGFspyusingoutlines));

  if (!spy)
    return ;

  spy->setShape();
}

void XWTIKZOptions::setSpySize()
{
  XWTIKZOptions * spy = (XWTIKZOptions*)(find(PGFspyusingoutlines));
  if (!spy)
    spy = (XWTIKZOptions*)(find(PGFspyusingoutlines));

  if (!spy)
    return ;

  spy->setExpress(PGFsize,tr("spy size"),tr("size:"));
}

void XWTIKZOptions::setStarburstPointHeight()
{
  setExpress(PGFstarburstpointheight,tr("starburst point height"),tr("length:"));
}

void XWTIKZOptions::setStarburstPoints()
{
  setExpress(PGFstarburstpoints,tr("starburst points"),tr("integer:"));
}

void XWTIKZOptions::setStarPointHeight()
{
  setExpress(PGFstarpointheight,tr("star point height"),tr("distance:"));
}

void XWTIKZOptions::setStarPointRatio()
{
  setExpress(PGFstarpointratio,tr("star point ratio"),tr("number:"));
}

void XWTIKZOptions::setStarPoints()
{
  setExpress(PGFstarpoints,tr("star points"),tr("integer:"));
}

void XWTIKZOptions::setStartAngle()
{
  setExpress(PGFstartangle,tr("arc"),tr("start angle:"));
}

void XWTIKZOptions::setStep()
{
  setExpress(PGFstep,tr("grid"),tr("step:"));
}

void XWTIKZOptions::setTapeBendHeight()
{
  setExpress(PGFtapebendheight,tr("tape bend height"),tr("length:"));
}

void XWTIKZOptions::setTextColor()
{
  setColor(PGFtext,tr("text color"));
}

void XWTIKZOptions::setTopColor()
{
  setColor(PGFtopcolor,tr("top color"));
}

void XWTIKZOptions::setTrapeziumAngle()
{
  setExpress(PGFtrapeziumangle,tr("internal angle of lower side"),tr("angle:"));
}

void XWTIKZOptions::setUpperLeftColor()
{
  setColor(PGFupperleft,tr("upper left color"));
}

void XWTIKZOptions::setUpperRightColor()
{
  setColor(PGFupperright,tr("upper right color"));
}

void XWTIKZOptions::setVolt()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("volt", "volt");
}

void XWTIKZOptions::setVoltOppsite()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("volt'", "volt");
}

void XWTIKZOptions::setVoltSloped()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("volt sloped", "volt");
}

void XWTIKZOptions::setVoltSlopedOppsite()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("volt' sloped", "volt");
}

void XWTIKZOptions::setVoltAmpere()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("voltampere", "voltampere");
}

void XWTIKZOptions::setVoltAmpereOppsite()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("voltampere'", "voltampere");
}

void XWTIKZOptions::setVoltAmpereSloped()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("voltampere sloped", "voltampere");
}

void XWTIKZOptions::setVoltAmpereSlopedOppsite()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("voltampere' sloped", "voltampere");
}

void XWTIKZOptions::setWatt()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("watt", "watt");
}

void XWTIKZOptions::setWattOppsite()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("watt'", "watt");
}

void XWTIKZOptions::setWattSloped()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("watt sloped", "watt");
}

void XWTIKZOptions::setWattSlopedOppsite()
{
  XWTikzCircuitSymbol * op = findCircuiteeSymbol();
  if (op)
    op->setUnit("watt' sloped", "watt");
}

void XWTIKZOptions::setXRadius()
{
  setExpress(PGFxradius,tr("ellipse"),tr("x radius:"));
}

void XWTIKZOptions::setXScale()
{
  setExpress(PGFxscale,tr("x scale"),tr("factor:"));
}

void XWTIKZOptions::setXShift()
{
  setExpress(PGFxshift,tr("x shift"),tr("dimension:"));
}

void XWTIKZOptions::setXSlant()
{
  setExpress(PGFxslant,tr("x slant"),tr("factor:"));
}

void XWTIKZOptions::setXVector()
{
  setExpress(PGFx,tr("x-vector"),tr("value:"));
}

void XWTIKZOptions::setYRadius()
{
  setExpress(PGFyradius,tr("ellipse"),tr("y radius:"));
}

void XWTIKZOptions::setYScale()
{
  setExpress(PGFyscale,tr("y scale"),tr("factor:"));
}

void XWTIKZOptions::setYShift()
{
  setExpress(PGFyshift,tr("y shift"),tr("dimension:"));
}

void XWTIKZOptions::setYSlant()
{
  setExpress(PGFyslant,tr("y slant"),tr("factor:"));
}

void XWTIKZOptions::setYVector()
{
  setExpress(PGFy,tr("y-vector"),tr("value:"));
}

void XWTIKZOptions::setZVector()
{
  setExpress(PGFz,tr("z-vector"),tr("value:"));
}

void XWTIKZOptions::setAround(int keywordA, 
                       const QString & title,
                       const QString & ltxt)
{
  XWTikzAround * r = getAround(keywordA);
  XWTikzAroundDialog dlg(title,ltxt);
  if (r)
  {
    QString str = r->getExpress();
    dlg.setExpress(str);
    str = r->getCoord();
    dlg.setCoord(str);
  }

  if (dlg.exec() == QDialog::Accepted)
  {
    QString exp = dlg.getExpress();
    QString around = dlg.getCoord();
    QUndoCommand * cmd = 0;
    if (r)
      cmd = new XWTikzSetAround(r,exp,around);
    else
    {
      r = new XWTikzAround(graphic,keywordA,this);
      r->setExpress(exp);
      r->setCoord(around);
      cmd = new XWTikzAddOption(this,cur+1,r);
    }    
    graphic->push(cmd);
  }
}

void XWTIKZOptions::setColor(int keywordA, const QString & title)
{
  XWTikzColor * c = getColor(keywordA);
  XWTikzColorDialog dlg(title);
  if (c)
  {
    int c1A,c2A;double pA;
    c->getColor(c1A,pA,c2A);
    dlg.setColor(c1A,pA,c2A);
  }

  if (dlg.exec() == QDialog::Accepted)
  {
    int c1A,c2A;double pA;
    dlg.getColor(c1A,pA,c2A);
    QUndoCommand * cmd = 0;
    if (c)
      cmd = new XWTikzSetColor(c,c1A,pA,c2A);
    else
    {
      c = new XWTikzColor(graphic,keywordA,this);
      c->setColor(c1A,pA,c2A);
      cmd = new XWTikzAddOption(this,cur+1,c);
    }
    graphic->push(cmd);
  }
}

void XWTIKZOptions::setCoord(int keywordA, const QString & title)
{
  XWTikzValue * v = getValue(keywordA);
  XWTikzCoordDialog dlg(title);
  if (v)
  {
    QString str = v->getCoord();
    dlg.setCoord(str);
  }

  if (dlg.exec() == QDialog::Accepted)
  {
    QString str = dlg.getCoord();
    QUndoCommand * cmd = 0;
    if (v)
      cmd = new XWTikzSetExpress(v,str);
    else
    {
      v = new  XWTikzValue(graphic,keywordA,this);
      v->setCoord(str);
      cmd = new XWTikzAddOption(this,cur+1,v);
    }
    graphic->push(cmd);
  }
}

void XWTIKZOptions::setDecoration(int keywordA)
{
  XWTikzValue * name = 0;
  XWTikzKey * key = 0;
  XWTikzOperation * op = find(PGFdecoration);
  if (op)
  {
    XWTIKZOptions * opts = (XWTIKZOptions*)(op);
    name = opts->getValue(keywordA);
    if (!name && keywordA == PGFname)
      key = opts->findDecoration();
  }

  XWTikzDecorationDialog dlg;
  if (name)
  {
    int d = (int)(name->getValue());
    dlg.setDecoration(d);
  }
  else
  {
    if (key)
      dlg.setDecoration(key->getKeyWord());
  }

  if (dlg.exec() == QDialog::Accepted)
  {
    int d = dlg.getDecoration();
    QUndoCommand * cmd = 0;
    if (name)
      cmd = new XWTikzSetValue(name,d);
    else if (key)
      cmd = new XWTikzSetKey(key,d);
    else
    {
      if (op)
      {
        name = new XWTikzValue(graphic,keywordA,this);
        name->setValue(d);
        cmd = new XWTikzAddOption((XWTIKZOptions*)(op),cur+1,name);
      }
      else
      {
        XWTikzDecorations * ds = new XWTikzDecorations(graphic,this);
        XWTikzValue * de = new XWTikzValue(graphic,PGFdecorate,ds);
        de->setValue(true);
        ds->ops << de;
        name = new XWTikzValue(graphic,keywordA,ds);
        name->setValue(d);
        ds->ops << name;
        cmd = new XWTikzAddOption(this,cur+1,ds);
      }
    }
    graphic->push(cmd);
  }
}

void XWTIKZOptions::setDecorationExpress(int keywordA,
                                    const QString & title,
                                    const QString & ltxt)
{
  XWTikzValue * p = 0;
  XWTikzOperation * op = find(PGFdecoration);
  if (op)
  {
    XWTIKZOptions * opts = (XWTIKZOptions*)(op);
    p = opts->getValue(keywordA);
  }

  XWTikzExpressDialog dlg(title,ltxt);
  if (p)
  {
    QString l = p->getExpress();
    dlg.setExpress(l);
  }

  if (dlg.exec() == QDialog::Accepted)
  {
    QString l = dlg.getExpress();
    QUndoCommand * cmd = 0;
    if (p)
      cmd = new XWTikzSetExpress(p,l);
    else
    {
      if (op)
      {
        p = new XWTikzValue(graphic,keywordA,this);
        p->setExpress(l);
        cmd = new XWTikzAddOption((XWTIKZOptions*)(op),cur+1,p);
      }
      else
      {
        XWTikzDecorations * ds = new XWTikzDecorations(graphic,this);
        p = new XWTikzValue(graphic,keywordA,ds);
        p->setExpress(l);
        ds->ops << p;
        cmd = new XWTikzAddOption(this,cur+1,ds);
      }
    }
    graphic->push(cmd);
  }
}

void XWTIKZOptions::setExpress(int keywordA, 
                       const QString & title,
                       const QString & ltxt)
{
  XWTikzValue * v = getValue(keywordA);
  XWTikzExpressDialog dlg(title,ltxt);
  if (v)
  {
    QString str = v->getExpress();
    dlg.setExpress(str);
  }

  if (dlg.exec() == QDialog::Accepted)
  {
    QString str = dlg.getExpress();
    QUndoCommand * cmd = 0;
    if (v)
      cmd = new XWTikzSetExpress(v,str);
    else
    {
      v = new  XWTikzValue(graphic,keywordA,this);
      v->setExpress(str);
      cmd = new XWTikzAddOption(this,cur+1,v);
    }
    graphic->push(cmd);
  }
}

void XWTIKZOptions::setInfo(const QString & title, int kw)
{
  XWTikzLabel * op = findInfo();
  XWTikzLabelDialog dlg(title);
  if (op)
  {
    QString opt = op->getOptions();
    dlg.setOptions(opt);
    QString a = op->getAngle();
    dlg.setAngle(a);
    QString txt = op->getContent();
    dlg.setText(txt);
  }
  if (dlg.exec() == QDialog::Accepted)
  {
    QString str = dlg.getLabel();
    QUndoCommand * cmd = 0;
    if (op)
      cmd = new XWTikzSetLabel(op,kw,str);
    else
    {
      int len = str.length();
      int pos = 0;
      op = new XWTikzLabel(graphic, kw, this);
      op->scan(str,len,pos);
      cmd = new XWTikzAddOption(this,cur+1,op);
    }
    
    graphic->push(cmd);
  }
}

void XWTIKZOptions::setLabel(const QString & title, int kw)
{
  XWTikzLabel * op = findLabel();
  XWTikzLabelDialog dlg(title);
  if (op)
  {
    QString opt = op->getOptions();
    dlg.setOptions(opt);
    QString a = op->getAngle();
    dlg.setAngle(a);
    QString txt = op->getContent();
    dlg.setText(txt);
  }
  if (dlg.exec() == QDialog::Accepted)
  {
    QString str = dlg.getLabel();
    QUndoCommand * cmd = 0;
    if (op)
      cmd = new XWTikzSetLabel(op,kw,str);
    else
    {
      int len = str.length();
      int pos = 0;
      op = new XWTikzLabel(graphic, kw, this);
      op->scan(str,len,pos);
      cmd = new XWTikzAddOption(this,cur+1,op);
    }
    
    graphic->push(cmd);
  }
}

void XWTIKZOptions::setUnit(const QString & title, const QString & u)
{
  XWTikzUnit * op = findUnit();
  QString v,ou;
  if (op)
    op->getUnit(ou,v);

  XWTikzExpressDialog dlg(title,u);
  dlg.setExpress(v);
  if (dlg.exec() == QDialog::Accepted)
  {
    v = dlg.getExpress();
    QUndoCommand * cmd = 0;
    if (op)
      cmd = new XWTikzSetUnit(op,title,v);
    else
    {
      op = new XWTikzUnit(graphic,title,this);
      op->setValue(v);
      cmd = new XWTikzAddOption(this,cur+1,op);
    }
    graphic->push(cmd);
  }
}

void XWTIKZOptions::setTwoValue(const QString & title,
                           int k1, const QString & title1,
                           int k2,const QString & title2)
{
  XWTikzValue * v1 = getValue(k1);
  XWTikzValue * v2 = getValue(k2);
  XWTikzDomainDialog dlg(title,title1,title2);
  if (v1)
  {
    QString tmp = v1->getExpress();
    dlg.setStart(tmp);
  }

  if (v2)
  {
    QString tmp = v2->getExpress();
    dlg.setEnd(tmp);
  }

  if (dlg.exec() == QDialog::Accepted)
  {
    QString s1 = dlg.getStart();
    QString s2 = dlg.getEnd();
    QUndoCommand * cmd = new QUndoCommand;
    int i = cur+1;
    if (v1)
      new XWTikzSetExpress(v1,s1,cmd);
    else
    {
      v1 = new XWTikzValue(graphic,k1,this);
      v1->setExpress(s1);
      new XWTikzAddOption(this,i++,v1,cmd);
    }

    if (v2)
      new XWTikzSetExpress(v2,s2,cmd);
    else
    {
      v2 = new XWTikzValue(graphic,k2,this);
      v2->setExpress(s2);
      new XWTikzAddOption(this,i++,v2,cmd);
    }
    graphic->push(cmd);
  }
}

void XWTIKZOptions::doPathDefault(XWTikzState * state, bool showpoint)
{
  for (int i = 0; i < ops.size(); i++)
    ops[i]->doPath(state,showpoint);
}

XWTikzOperation * XWTIKZOptions::find(int keywordA)
{
  for (int i = 0; i < ops.size(); i++)
  {
    if (ops[i]->getKeyWord() == keywordA)
      return ops[i];
  }

  return 0;
}

XWTikzKey * XWTIKZOptions::findCircuitSymbols()
{
  for (int i = 0; i < ops.size(); i++)
  {
    int kw = ops[i]->getKeyWord();
    if (kw == PGFhugecircuitsymbols || 
        kw == PGFlargecircuitsymbols ||
        kw == PGFmediumcircuitsymbols ||
        kw == PGFsmallcircuitsymbols ||
        kw == PGFtinycircuitsymbols)
    {
      return (XWTikzKey*)(ops[i]);
    }
  }

  return 0;
}

XWTikzCircuitSymbol * XWTIKZOptions::findCircuiteeSymbol()
{
  for (int i = 0; i < ops.size(); i++)
  {
    int kw = ops[i]->getKeyWord();
    if (kw == PGFresistor || 
        kw == PGFinductor ||
        kw == PGFcapacitor ||
        kw == PGFcontact ||
        kw == PGFground || 
        kw == PGFbattery || 
        kw == PGFdiode || 
        kw == PGFZenerdiode || 
        kw == PGFtunneldiode || 
        kw == PGFbackwarddiode || 
        kw == PGFSchottkydiode || 
        kw == PGFbreakdowndiode || 
        kw == PGFbulb || 
        kw == PGFvoltagesource || 
        kw == PGFcurrentsource || 
        kw == PGFmakecontact || 
        kw == PGFbreakcontact || 
        kw == PGFresistorIEC || 
        kw == PGFinductorIEC ||
        kw == PGFcapacitorIEC ||
        kw == PGFcontactIEC ||
        kw == PGFgroundIEC || 
        kw == PGFbatteryIEC || 
        kw == PGFdiodeIEC || 
        kw == PGFZenerdiodeIEC || 
        kw == PGFtunneldiodeIEC || 
        kw == PGFbackwarddiodeIEC || 
        kw == PGFSchottkydiodeIEC || 
        kw == PGFbreakdowndiodeIEC || 
        kw == PGFbulbIEC || 
        kw == PGFvoltagesourceIEC || 
        kw == PGFcurrentsourceIEC || 
        kw == PGFmakecontactIEC || 
        kw == PGFbreakcontactIEC || 
        kw == PGFamperemeter || 
        kw == PGFvoltmeter || 
        kw == PGFohmmeter || 
        kw == PGFacsource || 
        kw == PGFdcsource)
    {
      return (XWTikzCircuitSymbol*)(ops[i]);
    }
  }

  return 0;
}

XWTikzCircuitSymbol * XWTIKZOptions::findCircuitLogicSymbol()
{
  for (int i = 0; i < ops.size(); i++)
  {
    int kw = ops[i]->getKeyWord();
    if (kw == PGFandgate || 
        kw == PGFnandgate || 
        kw == PGForgate ||
        kw == PGFnorgate ||
        kw == PGFxorgate ||
        kw == PGFxnorgate ||
        kw == PGFnotgate ||
        kw == PGFbuffergate ||
        kw == PGFandgateIEC || 
        kw == PGFnandgateIEC || 
        kw == PGForgateIEC ||
        kw == PGFnorgateIEC ||
        kw == PGFxorgateIEC ||
        kw == PGFxnorgateIEC ||
        kw == PGFnotgateIEC ||
        kw == PGFbuffergateIEC ||
        kw == PGFandgateUS || 
        kw == PGFnandgateUS || 
        kw == PGForgateUS ||
        kw == PGFnorgateUS ||
        kw == PGFxorgateUS ||
        kw == PGFxnorgateUS ||
        kw == PGFnotgateUS ||
        kw == PGFbuffergateUS ||
        kw == PGFandgateCDH || 
        kw == PGFnandgateCDH)
    {
      return (XWTikzCircuitSymbol*)(ops[i]);
    }
  }

  return 0;
}

XWTikzKey * XWTIKZOptions::findConcept()
{
  for (int i = 0; i < ops.size(); i++)
  {
    int kw = ops[i]->getKeyWord();
    if (kw == PGFconcept || 
        kw == PGFextraconcept)
    {
      return (XWTikzKey*)(ops[i]);
    }
  }

  return 0;
}

XWTikzKey * XWTIKZOptions::findDash()
{
  for (int i = 0; i < ops.size(); i++)
  {
    int kw = ops[i]->getKeyWord();
    if (kw == PGFdotted || 
        kw == PGFdenselydotted ||
        kw == PGFlooselydotted ||
        kw == PGFdashed ||
        kw == PGFdenselydashed ||
        kw == PGFlooselydashed ||
        kw == PGFdashdotted ||
        kw == PGFdenselydashdotted ||
        kw == PGFlooselydashdotted ||
        kw == PGFdashdotdotted ||
        kw == PGFdenselydashdotdotted ||
        kw == PGFlooselydashdotdotted)
    {
      return (XWTikzKey*)(ops[i]);
    }
  }

  return 0;
}

XWTikzKey * XWTIKZOptions::findDecoration()
{
  for (int i = 0; i < ops.size(); i++)
  {
    int kw = ops[i]->getKeyWord();
    if (kw == PGFlineto || 
        kw == PGFmoveto ||
        kw == PGFcurveto ||
        kw == PGFcircleconnectionbar ||
        kw == PGFmarkings ||
        kw == PGFzigzag ||
        kw == PGFsaw ||
        kw == PGFrandomsteps ||
        kw == PGFstraightzigzag ||
        kw == PGFbent ||
        kw == PGFsnake ||
        kw == PGFcoil ||
        kw == PGFbumps)
    {
      return (XWTikzKey*)(ops[i]);
    }
  }

  return 0;
}

XWTikzKey * XWTIKZOptions::findEntityRelationship()
{
  for (int i = 0; i < ops.size(); i++)
  {
    int kw = ops[i]->getKeyWord();
    if (kw == PGFentity ||
        kw == PGFrelationship ||
        kw == PGFattribute ||
        kw == PGFkeyattribute)
    {
      return (XWTikzKey*)(ops[i]);
    }
  }

  return 0;
}

XWTikzLabel * XWTIKZOptions::findInfo()
{
  for (int i = 0; i < ops.size(); i++)
  {
    int kw = ops[i]->getKeyWord();
    if (kw == PGFinfo || 
        kw == PGFinfomissingangle ||
        kw == PGFinfosloped ||
        kw == PGFinfomissinganglesloped)
    {
      return (XWTikzLabel*)(ops[i]);
    }
  }

  return 0;
}

XWTikzLabel * XWTIKZOptions::findLabel()
{
  for (int i = 0; i < ops.size(); i++)
  {
    int kw = ops[i]->getKeyWord();
    if (kw == PGFlabel || 
        kw == PGFpin)
    {
      return (XWTikzLabel*)(ops[i]);
    }
  }

  return 0;
}

XWTikzKey * XWTIKZOptions::findLineWidth()
{
  for (int i = 0; i < ops.size(); i++)
  {
    int kw = ops[i]->getKeyWord();
    if (kw == PGFultrathin || 
        kw == PGFverythin ||
        kw == PGFthin ||
        kw == PGFsemithick ||
        kw == PGFthick ||
        kw == PGFverythick ||
        kw == PGFultrathick)
    {
      return (XWTikzKey*)(ops[i]);
    }
  }

  return 0;
}

XWTikzKey * XWTIKZOptions::findMindmap()
{
  for (int i = 0; i < ops.size(); i++)
  {
    int kw = ops[i]->getKeyWord();
    if (kw == PGFmindmap || 
        kw == PGFsmallmindmap ||
        kw == PGFlargemindmap ||
        kw == PGFhugemindmap)
    {
      return (XWTikzKey*)(ops[i]);
    }
  }

  return 0;
}

XWTikzKey * XWTIKZOptions::findPictureType()
{
  for (int i = 0; i < ops.size(); i++)
  {
    int kw = ops[i]->getKeyWord();
    if (kw == PGFcircuit || 
        kw == PGFcircuits ||
        kw == PGFcircuitee ||
        kw == PGFcircuiteeIEC ||
        kw == PGFcircuitlogic ||
        kw == PGFcircuitlogicIEC ||
        kw == PGFcircuitlogicUS ||
        kw == PGFcircuitlogicCDH)
    {
      return (XWTikzKey*)(ops[i]);
    }
  }

  return 0;
}

XWTikzKey * XWTIKZOptions::findPlotHandler()
{
  for (int i = 0; i < ops.size(); i++)
  {
    int kw = ops[i]->getKeyWord();
    if (kw == PGFsharpplot || 
        kw == PGFsmooth ||
        kw == PGFsmoothcycle ||
        kw == PGFconstplot ||
        kw == PGFconstplotmarkleft ||
        kw == PGFconstplotmarkright ||
        kw == PGFjumpmarkleft ||
        kw == PGFjumpmarkright ||
        kw == PGFycomb ||
        kw == PGFxcomb ||
        kw == PGFpolarcomb ||
        kw == PGFybar ||
        kw == PGFxbar ||
        kw == PGFybarinterval ||
        kw == PGFxbarinterval ||
        kw == PGFonlymarks)
    {
      return (XWTikzKey*)(ops[i]);
    }
  }

  return 0;
}

XWTikzKey * XWTIKZOptions::findPoint()
{
  for (int i = 0; i < ops.size(); i++)
  {
    int kw = ops[i]->getKeyWord();
    if (kw == PGFpointup || 
        kw == PGFpointdown ||
        kw == PGFpointleft ||
        kw == PGFpointright)
    {
      return (XWTikzKey*)(ops[i]);
    }
  }

  return 0;
}

XWTIKZOptions * XWTIKZOptions::findShadow()
{
  for (int i = 0; i < ops.size(); i++)
  {
    int kw = ops[i]->getKeyWord();
    if (kw == PGFgeneralshadow || 
        kw == PGFdropshadow ||
        kw == PGFcirculardropshadow ||
        kw == PGFcircularglow || 
        kw == PGFcopyshadow || 
        kw == PGFdoublecopyshadow)
    {
      return (XWTIKZOptions*)(ops[i]);
    }
  }

  return 0;
}

XWTikzKey * XWTIKZOptions::findShape()
{
  for (int i = 0; i < ops.size(); i++)
  {
    int kw = ops[i]->getKeyWord();
    if (kw == PGFcoordinate || 
        kw == PGFrectangle ||
        kw == PGFcircle ||
        kw == PGFellipse ||
        kw == PGFdiamond ||
        kw == PGFstar ||
        kw == PGFregularpolygon ||
        kw == PGFsemicircle ||
        kw == PGFisoscelestriangle ||
        kw == PGFkite ||
        kw == PGFdart ||
        kw == PGFcircularsector ||
        kw == PGFcylinder ||
        kw == PGFforbiddensign ||
        kw == PGFcorrectforbiddensign ||
        kw == PGFstarburst ||
        kw == PGFcloud ||
        kw == PGFsignal ||
        kw == PGFtape ||
        kw == PGFmagnifyingglass ||
        kw == PGFmagnetictape ||
        kw == PGFellipsecallout ||
        kw == PGFrectanglecallout ||
        kw == PGFcloudcallout ||
        kw == PGFsinglearrow ||
        kw == PGFdoublearrow ||
        kw == PGFarrowbox ||
        kw == PGFcrossout ||
        kw == PGFstrikeout ||
        kw == PGFroundedrectangle ||
        kw == PGFchamferedrectangle ||
        kw == PGFcirclesplit ||
        kw == PGFcirclesolidus ||
        kw == PGFrectanglesplit ||
        kw == PGFellipsesplit)
    {
      return (XWTikzKey*)(ops[i]);
    }
  }

  return 0;
}

XWTikzKey * XWTIKZOptions::findState()
{
  for (int i = 0; i < ops.size(); i++)
  {
    int kw = ops[i]->getKeyWord();
    if (kw == PGFstatewithoutoutput || 
        kw == PGFstatewithoutput ||
        kw == PGFacceptingbydouble ||
        kw == PGFinitialbydiamond ||
        kw == PGFinitialabove ||
        kw == PGFinitialbelow ||
        kw == PGFinitialleft ||
        kw == PGFinitialright ||
        kw == PGFacceptingabove ||
        kw == PGFacceptingbelow ||
        kw == PGFacceptingleft ||
        kw == PGFacceptingright ||
        kw == PGFstate ||
        kw == PGFaccepting ||
        kw == PGFinitial)
    {
      return (XWTikzKey*)(ops[i]);
    }
  }

  return 0;
}

XWTikzUnit * XWTIKZOptions::findUnit()
{
  for (int i = 0; i < ops.size(); i++)
  {
    int kw = ops[i]->getKeyWord();
    if (kw == PGFampere || 
        kw == PGFvolt ||
        kw == PGFohm ||
        kw == PGFohmtheotherside ||
        kw == PGFohmsloped ||
        kw == PGFohmtheothersidesloped ||
        kw == PGFsiemens ||
        kw == PGFhenry ||
        kw == PGFfarad ||
        kw == PGFcoulomb ||
        kw == PGFvoltampere ||
        kw == PGFwatt ||
        kw == PGFhertz)
    {
      return (XWTikzUnit*)(ops[i]);
    }
  }

  return 0;
}

QString XWTIKZOptions::getOptions()
{
  QString o;
  int len = 0;
  for (int i = 0; i < ops.size(); i++)
  {
    QString tmp = ops[i]->getText();
    o += tmp;
    len += tmp.length();
    if (i < ops.size() - 1)
      o += ",";

    if (len > 60)
    {
      o += "\n";
      len = 0;
    }
  }

  
  QString ret = getPGFString(keyWord);
  if (!o.isEmpty())
    ret = QString("%1={%2}").arg(ret).arg(o);

  return ret;
}

XWTikzFadingTransform::XWTikzFadingTransform(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFfadingtransform,parent)
{}

void XWTikzFadingTransform::doPath(XWTikzState * state, bool )
{
  for (int i = 0; i < ops.size(); i++)
  {
    XWTikzValue * v = (XWTikzValue*)(ops[i]);
    switch (v->getKeyWord())
    {
      default:
        break;

      case PGFrotate:
        state->setFadingRoate(v->getValue());
        break;

      case PGFshift:
        {
          QPointF p = v->getPoint(state);
          state->setFadingShift(p.x(),p.y());
        }
        break;

      case PGFxshift:
        state->setFadingShift(v->getValue(),0);
        break;

      case PGFyshift:
        state->setFadingShift(0,v->getValue());
        break;

      case PGFscale:
        {
          QPointF p = v->getPoint(state);
          state->setFadingScale(p.x(),p.y());
        }
        break;

      case PGFxscale:
        state->setFadingScale(v->getValue(),0);
        break;

      case PGFyscale:
        state->setFadingScale(0,v->getValue());
        break;

      case PGFxslant:
        state->setFadingSlant(v->getValue(),0);
        break;

      case PGFyslant:
        state->setFadingSlant(0,v->getValue());
        break;
    }
  }
}

QString XWTikzFadingTransform::getText()
{
  return getOptions();
}

void XWTikzFadingTransform::scan(const QString & str, int & len, int & pos)
{
  while (str[pos].isSpace())
    pos++;

  if (str[pos] != QChar('{'))
    return ;

  pos++;
  QString key;
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
      scanKey(str,len,pos,key);
      int id = lookupPGFID(key);
      XWTikzValue * v= new XWTikzValue(graphic,id,this);
      ops << v;
      v->scan(str,len,pos);
    }
  }
}

XWTikzNameIntersections::XWTikzNameIntersections(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFnameintersections,parent)
{}

void XWTikzNameIntersections::doPath(XWTikzState *state , bool showpoint)
{
  for (int i = 0; i < ops.size(); i++)
  {
    switch (ops[i]->getKeyWord())
    {
      default:
        break;

      case PGFof:
      case PGFsortby:
      case PGFby:
        ops[i]->doPath(state,showpoint);
        break;
    }
  }
}

QString XWTikzNameIntersections::getText()
{
  return getOptions();
}

void XWTikzNameIntersections::scan(const QString & str, int & len, int & pos)
{
  while (str[pos].isSpace())
    pos++;

  if (str[pos] != QChar('{'))
    return ;

  pos++;
  QString key;
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
      scanKey(str,len,pos,key);
      int id = lookupPGFID(key);
      switch (id)
      {
        default:
          {
            XWTikzValue * v= new XWTikzValue(graphic,id,this);
            ops << v;
            v->scan(str,len,pos);
          }
          break;

        case PGFof:
          {
            XWTikzOf * o = new XWTikzOf(graphic,this);
            ops << o;
            o->scan(str,len,pos);
          }
          break;

        case PGFby:
          {
            XWTikzBy * b = new XWTikzBy(graphic,this);
            ops << b;
            b->scan(str,len,pos);
          }
          break;
      }
    }
  }
}

XWTikzPreactions::XWTikzPreactions(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFpreactions,parent)
{}

void XWTikzPreactions::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzPreactions::getText()
{
  return getOptions();
}

XWTikzPostaction::XWTikzPostaction(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFpostaction,parent)
{}

void XWTikzPostaction::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzPostaction::getText()
{
  return getOptions();
}

XWTikzEveryNode::XWTikzEveryNode(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFeverynode,parent)
{}

void XWTikzEveryNode::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzEveryNode::getText()
{
  return getOptions();
}

XWTikzEveryShape::XWTikzEveryShape(XWTikzGraphic * graphicA, 
                                   int k, 
                                   int subk, 
                                   QObject * parent)
:XWTIKZOptions(graphicA, k,subk,parent)
{}

void XWTikzEveryShape::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzEveryShape::getText()
{
  return getOptions();
}

XWTikzEveryLabel::XWTikzEveryLabel(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFeverylabel,parent)
{}

void XWTikzEveryLabel::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzEveryLabel::getText()
{
  return getOptions();
}

XWTikzEveryPin::XWTikzEveryPin(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFpin,parent)
{}

void XWTikzEveryPin::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzEveryPin::getText()
{
  return getOptions();
}

XWTikzEveryPinEdge::XWTikzEveryPinEdge(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFeverypinedge,parent)
{}

void XWTikzEveryPinEdge::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzEveryPinEdge::getText()
{
  return getOptions();
}

XWTikzPinEdge::XWTikzPinEdge(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFpinedge,parent)
{}

void XWTikzPinEdge::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzPinEdge::getText()
{
  return getOptions();
}

XWTikzEveryEdge::XWTikzEveryEdge(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFeveryedge,parent)
{}

void XWTikzEveryEdge::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzEveryEdge::getText()
{
  return getOptions();
}

XWTikzLateOptions::XWTikzLateOptions(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFlateoptions,parent)
{}

void XWTikzLateOptions::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzLateOptions::getText()
{
  return getOptions();
}

XWTikzEveryMatrix::XWTikzEveryMatrix(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFeverymatrix,parent)
{}

void XWTikzEveryMatrix::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzEveryMatrix::getText()
{
  return getOptions();
}

XWTikzEveryCell::XWTikzEveryCell(XWTikzGraphic * graphicA, QObject * parent)
:XWTikzOperation(graphicA, PGFeverycell,parent),
 row(0),
 cell(0)
{
  row = new XWTIKZOptions(graphicA,this);
  cell = new XWTIKZOptions(graphicA,this);
}

QString XWTikzEveryCell::getText()
{
  QString r = row->getText();
  if (!r.isEmpty())
  {
    r[0] = QChar('{');
    int i = r.length() - 1;
    r[i] = QChar('}');
  }
  QString c = cell->getText();
  if (!c.isEmpty())
  {
    c[0] = QChar('{');
    int i = c.length() - 1;
    c[i] = QChar('}');
  }

  QString ret = QString("every cell /.style=%1%2").arg(r).arg(c);
  return ret;
}

void XWTikzEveryCell::scan(const QString & str, int & len, int & pos)
{
  row->scan(str,len,pos);
  cell->scan(str,len,pos);
}

XWTikzCells::XWTikzCells(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFcells,parent)
{}

void XWTikzCells::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzCells::getText()
{
  return getOptions();
}

XWTikzNodes::XWTikzNodes(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFnodes,parent)
{}

void XWTikzNodes::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzNodes::getText()
{
  return getOptions();
}

XWTikzEveryOddColumn::XWTikzEveryOddColumn(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFeveryoddcolumn,parent)
{}

void XWTikzEveryOddColumn::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzEveryOddColumn::getText()
{
  return getOptions();
}

XWTikzEveryEvenColumn::XWTikzEveryEvenColumn(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFeveryevencolumn,parent)
{}

void XWTikzEveryEvenColumn::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzEveryEvenColumn::getText()
{
  return getOptions();
}

XWTikzEveryOddRow::XWTikzEveryOddRow(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFeveryoddrow,parent)
{}

void XWTikzEveryOddRow::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzEveryOddRow::getText()
{
  return getOptions();
}

XWTikzEveryEvenRow::XWTikzEveryEvenRow(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFeveryevenrow,parent)
{}

void XWTikzEveryEvenRow::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzEveryEvenRow::getText()
{
  return getOptions();
}

XWTikzEveryChild::XWTikzEveryChild(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFeverychild,parent)
{}

void XWTikzEveryChild::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzEveryChild::getText()
{
  return getOptions();
}

XWTikzEveryChildNode::XWTikzEveryChildNode(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFeverychildnode,parent)
{}

void XWTikzEveryChildNode::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzEveryChildNode::getText()
{
  return getOptions();
}

XWTikzLevel::XWTikzLevel(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFlevel,parent)
{}

void XWTikzLevel::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzLevel::getText()
{
  return getOptions();
}

XWTikzLevelNumber::XWTikzLevelNumber(XWTikzGraphic * graphicA, 
                                   int k, 
                                   int subk, 
                                   QObject * parent)
:XWTIKZOptions(graphicA, k,subk,parent)
{}

void XWTikzLevelNumber::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzLevelNumber::getText()
{
  return getOptions();
}

XWTikzEdgeFromParentStyle::XWTikzEdgeFromParentStyle(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFedgefromparent,parent)
{}

void XWTikzEdgeFromParentStyle::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzEdgeFromParentStyle::getText()
{
  return getOptions();
}

XWTikzEveryMark::XWTikzEveryMark(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFeverymark,parent)
{}

void XWTikzEveryMark::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzEveryMark::getText()
{
  return getOptions();
}

XWTikzMarkOptions::XWTikzMarkOptions(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFmarkoptions,parent)
{}

void XWTikzMarkOptions::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzMarkOptions::getText()
{
  return getOptions();
}

XWTikzDecorations::XWTikzDecorations(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFdecoration,parent)
{}

void XWTikzDecorations::doPath(XWTikzState * state, bool showpoint)
{
  for (int i = 0; i < ops.size(); i++)
  {
    switch (ops[i]->getKeyWord())
    {
      default:
        ops[i]->doPath(state,showpoint);
        break;

      case PGFname:
        {
          XWTikzValue * v = (XWTikzValue*)(ops[i]);
          int d = (int)(v->getValue());
          state->setDecoration(d);
        }
        break;
    }
  }
}

QString XWTikzDecorations::getText()
{
  return getOptions();
}

void XWTikzDecorations::scan(const QString & str, int & len, int & pos)
{
  while (str[pos].isSpace())
    pos++;

  if (str[pos] == QChar('{'))
    XWTIKZOptions::scan(str,len,pos);
  else
  {
    XWTikzValue * v= new XWTikzValue(graphic,PGFname,this);
    ops << v;
    v->scan(str,len,pos);
  }
}

XWTikzTransform::XWTikzTransform(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFtransform,parent)
{}

void XWTikzTransform::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzTransform::getText()
{
  return getOptions();
}

XWTikzEveryTo::XWTikzEveryTo(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFeveryto,parent)
{}

void XWTikzEveryTo::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzEveryTo::getText()
{
  return getOptions();
}

XWTikzEveryMindmap::XWTikzEveryMindmap(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFeverymindmap,parent)
{}

void XWTikzEveryMindmap::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzEveryMindmap::getText()
{
  return getOptions();
}

XWTikzEveryConcept::XWTikzEveryConcept(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFeveryconcept,parent)
{}

void XWTikzEveryConcept::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzEveryConcept::getText()
{
  return getOptions();
}

XWTikzRootConcept::XWTikzRootConcept(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFrootconcept,parent)
{}

void XWTikzRootConcept::doPath(XWTikzState * state, bool showpoint)
{
  if (ops.size() <= 0)
  {
    switch (state->getMindmap())
    {
      default:
        doNormal(state);
        break;

      case PGFsmallmindmap:
        doSmall(state);
        break;

      case PGFlargemindmap:
        doLarge(state);
        break;

      case PGFhugemindmap:
        doHuge(state);
        break;
    }
  }
  else
    doPathDefault(state,showpoint);
}

QString XWTikzRootConcept::getText()
{
  return getOptions();
}

void XWTikzRootConcept::doHuge(XWTikzState * state)
{
  state->seiMinimumHeight(227.6221);
  state->seiMinimumHeight(227.6221);
  state->setTextWidth(199.1693);
  state->setFontSize(20);
}

void XWTikzRootConcept::doLarge(XWTikzState * state)
{
  state->seiMinimumHeight(159.3354);
  state->seiMinimumHeight(159.3354);
  state->setTextWidth(128.0374);
  state->setFontSize(14);
}

void XWTikzRootConcept::doNormal(XWTikzState * state)
{
  state->seiMinimumHeight(113.8110);
  state->seiMinimumHeight(113.8110);
  state->setTextWidth(99.5846);
  state->setFontSize(12);
}

void XWTikzRootConcept::doSmall(XWTikzState * state)
{
  state->seiMinimumHeight(65.4413);
  state->seiMinimumHeight(65.4413);
  state->setTextWidth(59.7508);
  state->setFontSize(8);
}

XWTikzLevelConcept::XWTikzLevelConcept(XWTikzGraphic * graphicA, int id,QObject * parent)
:XWTIKZOptions(graphicA, id,parent)
{}

void XWTikzLevelConcept::doPath(XWTikzState * state, bool showpoint)
{
  if (ops.size() <= 0)
  {
    switch (state->getMindmap())
    {
      default:
        doNormal(state);
        break;

      case PGFsmallmindmap:
        doSmall(state);
        break;

      case PGFlargemindmap:
        doLarge(state);
        break;

      case PGFhugemindmap:
        doHuge(state);
        break;
    }
  }
  else
    doPathDefault(state,showpoint);
}

QString XWTikzLevelConcept::getText()
{
  return getOptions();
}

void XWTikzLevelConcept::doHuge(XWTikzState * state)
{
  switch (state->getLevel())
  {
    default:
      break;

    case 1:
      state->seiMinimumHeight(128.0374);
      state->seiMinimumHeight(128.0374);
      state->setLevelDistance(284.5276);
      state->setTextWidth(113.8110);
      state->setSiblingAngle(60);
      state->setFontSize(12);
      break;

    case 2:
      state->seiMinimumHeight(99.5846);
      state->seiMinimumHeight(99.5846);
      state->setLevelDistance(165.0260);
      state->setTextWidth(85.3583);
      state->setSiblingAngle(60);
      state->setFontSize(10);
      break;

    case 3:
      state->seiMinimumHeight(65.4413);
      state->seiMinimumHeight(65.4413);
      state->setLevelDistance(136.5732);
      state->setTextWidth(56.9055);
      state->setSiblingAngle(30);
      state->setFontSize(8);
      break;

    case 4:
      state->seiMinimumHeight(48.3697);
      state->seiMinimumHeight(48.3697);
      state->setLevelDistance(105.2752);
      state->setTextWidth(39.8339);
      state->setSiblingAngle(30);
      state->setFontSize(7);
      break;
  }
}

void XWTikzLevelConcept::doLarge(XWTikzState * state)
{
  switch (state->getLevel())
  {
    default:
      break;

    case 1:
      state->seiMinimumHeight(91.0488);
      state->seiMinimumHeight(91.0488);
      state->setLevelDistance(199.1693);
      state->setTextWidth(79.6677);
      state->setSiblingAngle(60);
      state->setFontSize(10);
      break;

    case 2:
      state->seiMinimumHeight(69.7093);
      state->seiMinimumHeight(69.7093);
      state->setLevelDistance(113.8110);
      state->setTextWidth(62.5961);
      state->setSiblingAngle(60);
      state->setFontSize(9);
      break;

    case 3:
      state->seiMinimumHeight(46.3780);
      state->seiMinimumHeight(46.3780);
      state->setLevelDistance(96.1703);
      state->setTextWidth(39.8339);
      state->setSiblingAngle(30);
      state->setFontSize(5);
      break;

    case 4:
      state->seiMinimumHeight(36.1350);
      state->seiMinimumHeight(36.1350);
      state->setLevelDistance(73.9772);
      state->setTextWidth(28.4528);
      state->setSiblingAngle(30);
      state->setFontSize(5);
      break;
  }
}

void XWTikzLevelConcept::doNormal(XWTikzState * state)
{
  switch (state->getLevel())
  {
    default:
      break;

    case 1:
      state->seiMinimumHeight(64.0187);
      state->seiMinimumHeight(64.0187);
      state->setLevelDistance(142.2638);
      state->setTextWidth(56.9055);
      state->setSiblingAngle(60);
      state->setFontSize(9);
      break;

    case 2:
      state->seiMinimumHeight(49.7923);
      state->seiMinimumHeight(49.7923);
      state->setLevelDistance(82.5130);
      state->setTextWidth(42.671);
      state->setSiblingAngle(60);
      state->setFontSize(8);
      break;

    case 3:
      state->seiMinimumHeight(32.7207);
      state->seiMinimumHeight(32.7207);
      state->setLevelDistance(68.2866);
      state->setTextWidth(28.4528);
      state->setSiblingAngle(30);
      state->setFontSize(5);
      break;

    case 4:
      state->seiMinimumHeight(25.6075);
      state->seiMinimumHeight(25.6075);
      state->setLevelDistance(52.6376);
      state->setTextWidth(19.9169);
      state->setSiblingAngle(30);
      state->setFontSize(5);
      break;
  }
}

void XWTikzLevelConcept::doSmall(XWTikzState * state)
{
  switch (state->getLevel())
  {
    default:
      break;

    case 1:
      state->seiMinimumHeight(42.6791);
      state->seiMinimumHeight(42.6791);
      state->setLevelDistance(79.6677);
      state->setTextWidth(39.8339);
      state->setSiblingAngle(75);
      state->setFontSize(7);
      break;

    case 2:
      state->seiMinimumHeight(31.2980);
      state->seiMinimumHeight(31.2980);
      state->setLevelDistance(62.5961);
      state->setTextWidth(31.2980);
      state->setSiblingAngle(60);
      state->setFontSize(5);
      break;

    case 3:
    case 4:
      state->seiMinimumHeight(31.2980);
      state->seiMinimumHeight(31.2980);
      state->setLevelDistance(62.5961);
      state->setTextWidth(31.2980);
      state->setSiblingAngle(30);
      state->setFontSize(5);
      break;
      break;
  }
}

XWTikzEveryAnnotation::XWTikzEveryAnnotation(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFeveryannotation,parent)
{}

void XWTikzEveryAnnotation::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzEveryAnnotation::getText()
{
  return getOptions();
}

XWTikzSpyScope::XWTikzSpyScope(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFspyscope,parent)
{}

void XWTikzSpyScope::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzSpyScope::getText()
{
  return getOptions();
}

XWTikzLens::XWTikzLens(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFlens,parent)
{}

void XWTikzLens::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzLens::getText()
{
  return getOptions();
}

XWTikzEverySpyOnNode::XWTikzEverySpyOnNode(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFeveryspyonnode,parent)
{}

void XWTikzEverySpyOnNode::doPath(XWTikzState * state, bool showpoint)
{
  if (state->getSpyUsing() == PGFspyusingoutlines)
  {
    state->setDraw(true);
    state->setFill(false);
    state->setLineWidth(0.2);
  }
  else
  {
    state->setDraw(false);
    state->setFill(true);
    state->setFillOpacity(0.2);
  }
  doPathDefault(state,showpoint);
}

QString XWTikzEverySpyOnNode::getText()
{
  return getOptions();
}

XWTikzEverySpyInNode::XWTikzEverySpyInNode(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFeveryspyinnode,parent)
{}

void XWTikzEverySpyInNode::doPath(XWTikzState * state, bool showpoint)
{
  if (state->getSpyUsing() == PGFspyusingoutlines)
  {
    state->setDraw(true);
    state->setFill(false);
    state->setLineWidth(0.4);
  }
  else
  {
    state->setDraw(false);
    state->setFill(true);
    state->setFillOpacity(0.2);
  }
  doPathDefault(state,showpoint);
}

QString XWTikzEverySpyInNode::getText()
{
  return getOptions();
}

XWTikzSpyUsing::XWTikzSpyUsing(XWTikzGraphic * graphicA, int id, QObject * parent)
:XWTIKZOptions(graphicA, id,parent)
{}

void XWTikzSpyUsing::doPath(XWTikzState * state, bool showpoint)
{
  state->setSpyUsing(getKeyWord());
  if (state->isOnNode())
  {
    XWTikzEverySpyOnNode s(graphic);
    s.doPath(state,showpoint);
  }
  else
  {
    XWTikzEverySpyInNode s(graphic);
    s.doPath(state,showpoint);
  }
  doPathDefault(state,showpoint);
}

QString XWTikzSpyUsing::getText()
{
  return getOptions();
}

XWTikzCircuitHandleSymbol::XWTikzCircuitHandleSymbol(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFcircuithandlesymbol,parent)
{}

void XWTikzCircuitHandleSymbol::doPath(XWTikzState * state, bool showpoint)
{
  state = state->saveNode(0,XW_TIKZ_CIRCUIT_HANDLE_SYMBOL);
  XWTIKZOptions::doPath(state, showpoint);
}

QString XWTikzCircuitHandleSymbol::getText()
{
  return getOptions();
}

XWTikzEveryInfo::XWTikzEveryInfo(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFeveryinfo,parent)
{}

void XWTikzEveryInfo::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzEveryInfo::getText()
{
  return getOptions();
}

XWTikzEveryCircuitSymbol::XWTikzEveryCircuitSymbol(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFeverycircuitsymbolstyle,parent)
{}

void XWTikzEveryCircuitSymbol::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzEveryCircuitSymbol::getText()
{
  return getOptions();
}

XWTikzEveryCircuitAnnotation::XWTikzEveryCircuitAnnotation(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFeverycircuitannotationstyle,parent)
{}

void XWTikzEveryCircuitAnnotation::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzEveryCircuitAnnotation::getText()
{
  return getOptions();
}

XWTikzCircuitSymbolOpen::XWTikzCircuitSymbolOpen(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFcircuitsymbolopenstyle,parent)
{}

void XWTikzCircuitSymbolOpen::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzCircuitSymbolOpen::getText()
{
  return getOptions();
}

XWTikzCircuitSymbolFilled::XWTikzCircuitSymbolFilled(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFcircuitsymbolfilledstyle,parent)
{}

void XWTikzCircuitSymbolFilled::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzCircuitSymbolFilled::getText()
{
  return getOptions();
}

XWTikzCircuitSymbolLines::XWTikzCircuitSymbolLines(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFcircuitsymbollinesstyle,parent)
{}

void XWTikzCircuitSymbolLines::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzCircuitSymbolLines::getText()
{
  return getOptions();
}

XWTikzCircuitSymbolWires::XWTikzCircuitSymbolWires(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFcircuitsymbolwiresstyle,parent)
{}

void XWTikzCircuitSymbolWires::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzCircuitSymbolWires::getText()
{
  return getOptions();
}

XWTikzCircuitSymbol::XWTikzCircuitSymbol(XWTikzGraphic * graphicA, int id, QObject * parent)
:XWTIKZOptions(graphicA, id,parent)
{}

void XWTikzCircuitSymbol::doPath(XWTikzState * state, bool showpoint)
{
  state->setNodeType(keyWord, XW_TIKZ_CIRCUIT_SYMBOL);
  for (int i = 0; i < ops.size(); i++)
    ops[i]->doPath(state,showpoint);
}

QString XWTikzCircuitSymbol::getText()
{
  return getOptions();
}

XWTikzAnnotation::XWTikzAnnotation(XWTikzGraphic * graphicA, int id, QObject * parent)
:XWTIKZOptions(graphicA, id,parent),
 b(false)
{}

void XWTikzAnnotation::doPath(XWTikzState * state, bool showpoint)
{
  if (b)
  {
    XWTIKZOptions::doPath(state,showpoint);
    b = false;
  }
  else
  {
    state->setAnnotation(this);
    b = true;
  }
}

QString XWTikzAnnotation::getText()
{
  return getOptions();
}

XWTikzEveryState::XWTikzEveryState(XWTikzGraphic * graphicA, int id,QObject * parent)
:XWTIKZOptions(graphicA, id,parent)
{}

void XWTikzEveryState::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzEveryState::getText()
{
  return getOptions();
}

XWTikzEveryAcceptingByArrow::XWTikzEveryAcceptingByArrow(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFeveryacceptingbyarrowstyle,parent)
{}

void XWTikzEveryAcceptingByArrow::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzEveryAcceptingByArrow::getText()
{
  return getOptions();
}

XWTikzEveryInitialByArrow::XWTikzEveryInitialByArrow(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFeveryinitialbyarrowstyle,parent)
{}

void XWTikzEveryInitialByArrow::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzEveryInitialByArrow::getText()
{
  return getOptions();
}

XWTikzEveryEntity::XWTikzEveryEntity(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFeveryentity,parent)
{}

void XWTikzEveryEntity::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzEveryEntity::getText()
{
  return getOptions();
}

XWTikzEveryRelationship::XWTikzEveryRelationship(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFeveryrelationship,parent)
{}

void XWTikzEveryRelationship::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzEveryRelationship::getText()
{
  return getOptions();
}

XWTikzEveryAttribute::XWTikzEveryAttribute(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFeveryattribute,parent)
{}

void XWTikzEveryAttribute::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzEveryAttribute::getText()
{
  return getOptions();
}

XWTikzEveryShadow::XWTikzEveryShadow(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFeveryshadow,parent)
{}

void XWTikzEveryShadow::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
}

QString XWTikzEveryShadow::getText()
{
  return getOptions();
}

XWTikzGeneralShadow::XWTikzGeneralShadow(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFgeneralshadow,parent)
{}

void XWTikzGeneralShadow::doPath(XWTikzState * state, bool showpoint)
{
  doPathDefault(state,showpoint);
  state->generalShadow();
  graphic->doEveryShadow(state);
}

QString XWTikzGeneralShadow::getText()
{
  return getOptions();
}

XWTikzDropShadow::XWTikzDropShadow(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFdropshadow,parent)
{}

void XWTikzDropShadow::doPath(XWTikzState * state, bool showpoint)
{
  state->setShadowScale(1);
  state->setShadowXShift(5);
  state->setShadowYShift(-5);
  state->setOpacity(0.5);
  QColor color = calulateColor(PGFblack,50);
  state->setFillColor(color);
  doPathDefault(state,showpoint);
  state->generalShadow();
  graphic->doEveryShadow(state);
}

QString XWTikzDropShadow::getText()
{
  return getOptions();
}

XWTikzCircularDropShadow::XWTikzCircularDropShadow(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFcirculardropshadow,parent)
{}

void XWTikzCircularDropShadow::doPath(XWTikzState * state, bool showpoint)
{
  state->setShadowScale(1.1);
  state->setShadowXShift(3);
  state->setShadowYShift(-3);
  QColor color = calulateColor(PGFblack,50);
  state->setFillColor(color);
  state->setPathFading(PGFcirclewithfuzzyedge15percent);
  doPathDefault(state,showpoint);
  state->generalShadow();
  graphic->doEveryShadow(state);
}

QString XWTikzCircularDropShadow::getText()
{
  return getOptions();
}

XWTikzCircularGlow::XWTikzCircularGlow(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFcircularglow,parent)
{}

void XWTikzCircularGlow::doPath(XWTikzState * state, bool showpoint)
{
  state->setShadowScale(1.25);
  state->setShadowXShift(0);
  state->setShadowYShift(0);
  QColor color = calulateColor(PGFblack,50);
  state->setFillColor(color);
  state->setPathFading(PGFcirclewithfuzzyedge15percent);
  doPathDefault(state,showpoint);
  state->generalShadow();
  graphic->doEveryShadow(state);
}

QString XWTikzCircularGlow::getText()
{
  return getOptions();
}

XWTikzCopyShadow::XWTikzCopyShadow(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFcopyshadow,parent)
{}

void XWTikzCopyShadow::doPath(XWTikzState * state, bool showpoint)
{
  state->setShadowScale(1);
  state->setShadowXShift(5);
  state->setShadowYShift(5);
  state->setDraw(true);
  state->setFill(true);
  doPathDefault(state,showpoint);
  state->generalShadow();
  graphic->doEveryShadow(state);
}

QString XWTikzCopyShadow::getText()
{
  return getOptions();
}

XWTikzDoubleCopyShadow::XWTikzDoubleCopyShadow(XWTikzGraphic * graphicA, QObject * parent)
:XWTIKZOptions(graphicA, PGFdoublecopyshadow,parent)
{}

void XWTikzDoubleCopyShadow::doPath(XWTikzState * state, bool showpoint)
{
  state->setShadowScale(1);
  state->setShadowXShift(10);
  state->setShadowYShift(10);
  state->setDraw(true);
  state->setFill(true);
  doPathDefault(state,showpoint);
  state->generalShadow();
  graphic->doEveryShadow(state);
  graphic->doPath(state);
  state->setShadowXShift(5);
  state->setShadowYShift(5);
  state->setDraw(true);
  state->setFill(true);
  doPathDefault(state,showpoint);
  state->generalShadow();
  graphic->doEveryShadow(state);
}

QString XWTikzDoubleCopyShadow::getText()
{
  return getOptions();
}
