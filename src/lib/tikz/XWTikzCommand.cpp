/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QFileDialog>
#include "PGFKeyWord.h"
#include "XWPGFPool.h"
#include "XWTeXBox.h"
#include "XWTikzGraphic.h"
#include "XWTikzArrow.h"
#include "XWTikzNode.h"
#include "XWTikzState.h"
#include "XWTikzCoord.h"
#include "XWTikzOptions.h"
#include "XWTikzMatrix.h"
#include "XWTikzOperationDialog.h"
#include "XWTikzOptionDialog.h"
#include "XWTikzCommandDialog.h"
#include "XWTikzUndoCommand.h"
#include "XWTikzCommand.h"

XWTikzCommand * createPGFObject(XWTikzGraphic * graphicA,XWTikzScope *scopeA,int id,QObject * parent)
{
  XWTikzCommand * obj = 0;

  switch (id)
  {
    case PGFpath:
    case PGFdraw:
    case PGFfill:
    case PGFfilldraw:
    case PGFpattern:
    case PGFshade:
    case PGFshadedraw:
    case PGFclip:
      obj = new XWTikzPath(graphicA,scopeA,id,parent);
      break;

    case PGFscope:
      obj = new XWTikzScope(graphicA,scopeA,id,parent);
      break;

    case PGFforeach:
      obj = new XWTikzForeachCmd(graphicA,scopeA,parent);
      break;

    case PGFnode:
      obj = new XWTikzNodePath(graphicA,scopeA,parent);
      break;

    case PGFcoordinate:
      obj = new XWTikzCoordinatePath(graphicA,scopeA,parent);
      break;

    case PGFmatrix:
      obj = new XWTikzMatrixCommand(graphicA,scopeA,parent);
      break;

    case PGFspy:
      obj = new XWTikzSpy(graphicA,scopeA,parent);
      break;

    case PGFarrow:
      obj = new XWTikzArrowMarking(graphicA,scopeA,parent);
      break;

    default:
      break;
  }

  return obj;
}

XWTikzCommand::XWTikzCommand(XWTikzGraphic * graphicA,XWTikzScope *scopeA,int idA,QObject * parent)
: QObject(parent),
 graphic(graphicA),
 scope(scopeA),
 keyWord(idA),
 options(0)
{
  options = new XWTIKZOptions(graphicA,this);
}

XWTikzCommand::~XWTikzCommand()
{}

void XWTikzCommand::addArc()
{}

void XWTikzCommand::addCircle()
{}

void XWTikzCommand::addCoordinate()
{}

void XWTikzCommand::addCosine()
{}

void XWTikzCommand::addCurveTo()
{}

void XWTikzCommand::addCycle()
{}

void XWTikzCommand::addHVLines()
{}

void XWTikzCommand::addEdge()
{}

void XWTikzCommand::addEllipse()
{}

void XWTikzCommand::addGrid()
{}

void XWTikzCommand::addLineTo()
{}

void XWTikzCommand::addMoveTo()
{}

void XWTikzCommand::addNode()
{}

void XWTikzCommand::addParabola()
{}

bool XWTikzCommand::addOperationAction(QMenu &, XWTikzState *)
{
  return false;
}

void XWTikzCommand::addPlotCoordinates()
{}

void XWTikzCommand::addPlotFile()
{}

void XWTikzCommand::addPlotFunction()
{}

void XWTikzCommand::addRectangle()
{}

void XWTikzCommand::addPathAction(QMenu &, XWTikzState *)
{}

void XWTikzCommand::addSine()
{}

void XWTikzCommand::addSplit(const QString & )
{}

void XWTikzCommand::addVHLines()
{}

bool XWTikzCommand::back(XWTikzState * )
{
  return false;
}

bool XWTikzCommand::cut(XWTikzState * )
{
  return false;
}

bool XWTikzCommand::del(XWTikzState *)
{
  return false;
}

void XWTikzCommand::doCopy(XWTikzState * state)
{
  doPath(state, false);
}

void XWTikzCommand::doDecoration(XWTikzState * )
{}

void XWTikzCommand::doEdgeFromParent(XWTikzState * )
{}

void XWTikzCommand::doEdgeFromParentPath(XWTikzState *)
{}

void XWTikzCommand::doEveryAcceptingByArrow(XWTikzState *)
{}

void XWTikzCommand::doEveryAttribute(XWTikzState * )
{}

void XWTikzCommand::doEveryChild(XWTikzState *)
{}

void XWTikzCommand::doEveryChildNode(XWTikzState *)
{}

void XWTikzCommand::doEveryCircuitAnnotation(XWTikzState *)
{}

void XWTikzCommand::doEveryCircuitSymbol(XWTikzState *)
{}

void XWTikzCommand::doEveryConcept(XWTikzState * )
{}

void XWTikzCommand::doEveryCurveTo(XWTikzState *)
{}

void XWTikzCommand::doEveryEdge(XWTikzState *)
{}

void XWTikzCommand::doEveryEntity(XWTikzState * )
{}

void XWTikzCommand::doEveryInfo(XWTikzState * )
{}

void XWTikzCommand::doEveryInitialByArrow(XWTikzState *)
{}

void XWTikzCommand::doEveryLabel(XWTikzState * )
{}

void XWTikzCommand::doEveryLoop(XWTikzState *)
{}

void XWTikzCommand::doEveryMark(XWTikzState * )
{}

void XWTikzCommand::doEveryMatrix(XWTikzState * )
{}

void XWTikzCommand::doEveryMindmap(XWTikzState * )
{}

void XWTikzCommand::doEveryNode(XWTikzState * )
{}

void XWTikzCommand::doEveryPin(XWTikzState * )
{}

void XWTikzCommand::doEveryPinEdge(XWTikzState * )
{}

void XWTikzCommand::doEveryPlace(XWTikzState * )
{}

void XWTikzCommand::doEveryRelationship(XWTikzState * )
{}

void XWTikzCommand::doEveryShadow(XWTikzState * )
{}

void XWTikzCommand::doEveryShape(XWTikzState * )
{}

void XWTikzCommand::doEveryState(XWTikzState *)
{}

void XWTikzCommand::doEveryToken(XWTikzState *)
{}

void XWTikzCommand::doEveryTransition(XWTikzState *)
{}

void XWTikzCommand::doLevel(XWTikzState * )
{}

void XWTikzCommand::doLevelConcept(XWTikzState *)
{}

void XWTikzCommand::doLevelNumber(XWTikzState * )
{}

void XWTikzCommand::doOperation(XWTikzState * , bool )
{}

void XWTikzCommand::doPath(XWTikzState *, bool)
{}

void XWTikzCommand::doPathNoOptions(XWTikzState *)
{}

void XWTikzCommand::doRootConcept(XWTikzState * )
{}

void XWTikzCommand::doSpyConnection(XWTikzState * )
{}

void XWTikzCommand::doState(XWTikzState * )
{}

void XWTikzCommand::doSpyNode(XWTikzState * )
{}

void XWTikzCommand::doToPath(XWTikzState * )
{
}

void XWTikzCommand::dragTo(XWTikzState *)
{}

bool XWTikzCommand::dropTo(XWTikzState *)
{
  return false;
}

int XWTikzCommand::getAnchorPosition()
{
  return 0;
}

QPointF XWTikzCommand::getAnchor(const QString & ,int ,XWTikzState * )
{
  return QPointF();
}

QPointF XWTikzCommand::getAngle(const QString & ,double , XWTikzState * )
{
  return QPointF();
}

QPointF XWTikzCommand::getBorder(const QString & ,const QPointF & ,XWTikzState * )
{
  return QPointF();
}

QPointF XWTikzCommand::getCenter(XWTikzState *)
{
  return QPointF();
}

XWTikzOperation * XWTikzCommand::getCurrentOperation()
{
  return 0;
}

XWTikzCoord * XWTikzCommand::getCurrentPoint()
{
  return 0;
}

int XWTikzCommand::getCursorPosition()
{
  return 0;
}

QString XWTikzCommand::getCurrentText()
{
  return QString();
}

QList<QPointF> XWTikzCommand::getIntersections(const QList<int> & ,
                                          const QList<QPointF> & ,
                                          XWTikzState * )
{
  QList<QPointF> ret;
  return ret;
}

void XWTikzCommand::getPath(QList<int> & ,
                          QList<QPointF> & ,
                          XWTikzState * )
{
}

QPointF XWTikzCommand::getPoint(XWTikzState * )
{
  return QPointF();
}

QPointF XWTikzCommand::getPoint(const QString & ,XWTikzState *)
{
  return QPointF();
}

QVector3D XWTikzCommand::getPoint3D(const QString & ,XWTikzState * )
{
  return QVector3D();
}

double XWTikzCommand::getRadius(const QString & ,XWTikzState * )
{
  return 0.0;
}

QString XWTikzCommand::getSelectedText()
{
  return QString();
}

QString XWTikzCommand::getText()
{
  return QString();
}

QString XWTikzCommand::getTips(XWTikzState *)
{
  return QString();
}

void XWTikzCommand::getWidthAndHeight(double &, double &)
{}

void XWTikzCommand::goToEnd()
{}

bool XWTikzCommand::goToNext()
{
  return false;
}

void XWTikzCommand::goToNextPoint()
{}

void XWTikzCommand::goToPathEnd()
{}

void XWTikzCommand::goToPathStart()
{}

void XWTikzCommand::goToOperationEnd()
{}

void XWTikzCommand::goToOperationStart()
{}

bool XWTikzCommand::goToPrevious()
{
  return false;
}

void XWTikzCommand::goToPreviousPoint()
{}

void XWTikzCommand::goToStart()
{}

bool XWTikzCommand::hitTest(XWTikzState *)
{
  return false;
}

bool XWTikzCommand::insertText(XWTikzState *)
{
  return false;
}

bool XWTikzCommand::isMe(const QString & ,XWTikzState * )
{
  return false;
}

bool XWTikzCommand::keyInput(XWTikzState * )
{
  return false;
}

bool XWTikzCommand::newPar(XWTikzState * )
{
  return false;
}

bool XWTikzCommand::paste(XWTikzState *)
{
  return false;
}

void XWTikzCommand::scan(const QString & , int & , int &)
{}

XWTikzPath::XWTikzPath(XWTikzGraphic * graphicA,XWTikzScope *scopeA,int idA,QObject * parent)
:XWTikzCommand(graphicA,scopeA,idA,parent),
cur(-1)
{}

XWTikzPath::~XWTikzPath()
{}

void XWTikzPath::addArc()
{
  if (ops.size() == 0)
    addMoveTo();

  XWTikzArcDialog dlg;
  if (dlg.exec() == QDialog::Accepted)
  {
    XWTikzArc * a = new XWTikzArc(graphic,this);
    QString tmp = dlg.getEndAngle();    
    a->setEndAngle(tmp);
    tmp = dlg.getStartAngle();   
    a->setStartAngle(tmp);
    XWTikzAddOperation * cmd = new XWTikzAddOperation(this,cur + 1,a);
    graphic->push(cmd);
  }
}

void XWTikzPath::addCircle()
{
  if (ops.size() == 0)
    addMoveTo();

  XWTikzCircleDialog dlg;
  if (dlg.exec() == QDialog::Accepted)
  {
    XWTikzEllipse * c = new XWTikzEllipse(graphic,PGFcircle,this);
    QString tmp = dlg.getRadius();    
    c->setRadius(tmp);
    tmp = dlg.getAt();   
    if (!tmp.isEmpty())
      c->setAt(tmp);
    XWTikzAddOperation * cmd = new XWTikzAddOperation(this,cur + 1,c);
    graphic->push(cmd);
  }
}

void XWTikzPath::addCoordinate()
{
  if (ops.size() == 0)
    addMoveTo();

  XWTikzNameDialog dlg(tr("Coordinate"));
  if (dlg.exec() == QDialog::Accepted)
  {
    QString tmp = dlg.getName();
    XWTikzCoordinate * c = new XWTikzCoordinate(graphic,this);
    c->setName(tmp);
    XWTikzAddOperation * cmd = new XWTikzAddOperation(this,cur + 1,c);
    graphic->push(cmd);
  }
}

void XWTikzPath::addCosine()
{
  if (ops.size() == 0)
    addMoveTo();

  XWTikzCoordDialog dlg(tr("Cosine"));
  if (dlg.exec() == QDialog::Accepted)
  {
    QString tmp = dlg.getCoord();
    XWTikzCosine * s = new XWTikzCosine(graphic,this);
    s->setCoord(tmp);
    XWTikzAddOperation * cmd = new XWTikzAddOperation(this,cur + 1,s);
    graphic->push(cmd);
  }
}

void XWTikzPath::addCurveTo()
{
  if (ops.size() == 0)
    addMoveTo();

  XWTikzCurveToDialog dlg;
  if (dlg.exec() == QDialog::Accepted)
  {
    XWTikzCurveTo * c = new XWTikzCurveTo(graphic,this);
    QString tmp = dlg.getC1();    
    c->setC1(tmp);
    tmp = dlg.getC2();   
    if (!tmp.isEmpty())
      c->setC2(tmp);
    tmp = dlg.getEnd();   
    c->setEndPoint(tmp);
    XWTikzAddOperation * cmd = new XWTikzAddOperation(this,cur + 1,c);
    graphic->push(cmd);
  }
}

void XWTikzPath::addCycle()
{
  if (ops.size() < 1)
    addMoveTo();

  XWTikzCycle * c = new XWTikzCycle(graphic,this);
  XWTikzAddOperation * cmd = new XWTikzAddOperation(this,cur + 1,c);
  graphic->push(cmd);
}

void XWTikzPath::addEdge()
{
  if (ops.size() < 1)
    addMoveTo();

  XWTikzCoordDialog dlg(tr("Edge to"));
  if (dlg.exec() == QDialog::Accepted)
  {
    QString tmp = dlg.getCoord();
    XWTikzEdge * e = new XWTikzEdge(graphic,this);
    e->setCoord(tmp);
    XWTikzAddOperation * cmd = new XWTikzAddOperation(this,cur + 1,e);
    graphic->push(cmd);
  }
}

void XWTikzPath::addEllipse()
{
  if (ops.size() == 0)
    addMoveTo();

  XWTikzEllipseDialog dlg;
  if (dlg.exec() == QDialog::Accepted)
  {
    XWTikzEllipse * c = new XWTikzEllipse(graphic,PGFellipse,this);
    QString tmp = dlg.getXRadius();    
    c->setXRadius(tmp);
    tmp = dlg.getYRadius();    
    c->setYRadius(tmp);
    tmp = dlg.getAt();   
    if (!tmp.isEmpty())
      c->setAt(tmp);
    XWTikzAddOperation * cmd = new XWTikzAddOperation(this,cur + 1,c);
    graphic->push(cmd);
  }
}

void XWTikzPath::addGrid()
{
  if (ops.size() == 0)
    addMoveTo();

  XWTikzGridDialog dlg;
  if (dlg.exec() == QDialog::Accepted)
  {
    XWTikzGrid * g = new XWTikzGrid(graphic,this);
    QString tmp = dlg.getCorner();    
    g->setCorner(tmp);
    tmp = dlg.getStep();    
    if (!tmp.isEmpty())
      g->setStep(tmp);
    XWTikzAddOperation * cmd = new XWTikzAddOperation(this,cur + 1,g);
    graphic->push(cmd);
  }
}

void XWTikzPath::addHVLines()
{
  if (ops.size() == 0)
    addMoveTo();

  XWTikzCoordDialog dlg(tr("Horizontal then vertical"));
  if (dlg.exec() == QDialog::Accepted)
  {
    QString tmp = dlg.getCoord();
    XWTikzHVLinesTo * lines = new XWTikzHVLinesTo(graphic,this);
    lines->setCoord(tmp);
    XWTikzAddOperation * cmd = new XWTikzAddOperation(this,cur + 1,lines);
    graphic->push(cmd);
  }
}

void XWTikzPath::addLineTo()
{
  if (ops.size() == 0)
    addMoveTo();

  XWTikzCoordDialog dlg(tr("Line to"));
  if (dlg.exec() == QDialog::Accepted)
  {
    QString tmp = dlg.getCoord();
    XWTikzLineTo * line = new XWTikzLineTo(graphic,this);
    line->setCoord(tmp);
    XWTikzAddOperation * cmd = new XWTikzAddOperation(this,cur + 1,line);
    graphic->push(cmd);
  }
}

void XWTikzPath::addMoveTo()
{
  XWTikzCoordDialog dlg(tr("Move to"));
  if (dlg.exec() == QDialog::Accepted)
  {
    QString tmp = dlg.getCoord();
    XWTikzCoord * coord = new XWTikzCoord(graphic,tmp,this);
    XWTikzAddOperation * cmd = new XWTikzAddOperation(this,cur + 1,coord);
    graphic->push(cmd);
  }
}

void XWTikzPath::addNode()
{
  if (ops.size() == 0)
    addMoveTo();

  XWTikzNodeDialog dlg;
  if (dlg.exec() == QDialog::Accepted)
  {
    XWTikzNode * n = new XWTikzNode(graphic,this);
    QString tmp = dlg.getName();    
    n->setName(tmp);
    tmp = dlg.getText();    
    n->setText(tmp);
    XWTikzAddOperation * cmd = new XWTikzAddOperation(this,cur + 1,n);
    graphic->push(cmd);
  }
}

bool XWTikzPath::addOperationAction(QMenu & menu, XWTikzState * state)
{
  if (cur < 0 || cur >= ops.size())
    return false;

  options->doPath(state, false);
  return ops[cur]->addAction(menu, state);
}

void XWTikzPath::addParabola()
{
  if (ops.size() == 0)
    addMoveTo();

  XWTikzParabolaDialog dlg;
  if (dlg.exec() == QDialog::Accepted)
  {
    XWTikzParabola * p = new XWTikzParabola(graphic,this);
    QString tmp = dlg.getBend();    
    if (!tmp.isEmpty())
      p->setBend(tmp);
    tmp = dlg.getEnd();
    p->setEnd(tmp);
    XWTikzAddOperation * cmd = new XWTikzAddOperation(this,cur + 1,p);
    graphic->push(cmd);
  }
}

void XWTikzPath::addPathAction(QMenu & menu, XWTikzState * state)
{
  options->doCompute(state);
  QMenu * submenu = menu.addMenu(tr("draw"));
  options->addLineWidthAction(*submenu);
  options->addArrowsAction(*submenu);
  options->addRoundedCornersAction(*submenu);
  options->addDoubleDistanceAction(*submenu);
  menu.addSeparator();
  submenu = menu.addMenu(tr("decoration"));
  options->addDecorationAction(*submenu);
  menu.addSeparator();
  submenu = menu.addMenu(tr("color"));
  options->addColorAction(*submenu);
  options->addOpacityAction(*submenu);
  options->addDoubleAction(*submenu);
  options->addTextColorAction(*submenu);
  menu.addSeparator();
  submenu = menu.addMenu(tr("pattern"));
  options->addPatternAction(*submenu);
  menu.addSeparator();
  submenu = menu.addMenu(tr("shade"));
  options->addShadeAction(*submenu);
  menu.addSeparator();
  submenu = menu.addMenu(tr("fading"));
  options->addPathFading(*submenu);
  menu.addSeparator();
  submenu = menu.addMenu(tr("shadow"));
  options->addShadowAction(*submenu);
  menu.addSeparator();
  options->addDomainAction(menu);
  menu.addSeparator();
  submenu = menu.addMenu(tr("intersection"));
  options->addIntersectionsAction(*submenu);
}

void XWTikzPath::addPlotCoordinates()
{
  XWTikzPlotCoordsDialog dlg;
  if (dlg.exec() == QDialog::Accepted)
  {
    XWTikzPlotCoordinates * p = new XWTikzPlotCoordinates(graphic,this);
    QString tmp = dlg.getCoords();    
    p->setCoords(tmp);
    XWTikzAddOperation * cmd = new XWTikzAddOperation(this,cur + 1,p);
    graphic->push(cmd);
  }
}

void XWTikzPath::addPlotFile()
{
  QString path = graphic->getLocalPath();
  QString filename = QFileDialog::getOpenFileName(0, tr("Open File"),path,tr("plots (*.table)"));
  if (!filename.isEmpty())
  {
    XWTikzPlotFile * p = new XWTikzPlotFile(graphic,this);
    p->setFileName(filename);
    XWTikzAddOperation * cmd = new XWTikzAddOperation(this,cur + 1,p);
    graphic->push(cmd);
  }
}

void XWTikzPath::addPlotFunction()
{
  XWTikzPlotFunctionDialog dlg;
  if (dlg.exec() == QDialog::Accepted)
  {
    XWTikzPlotFunction * p = new XWTikzPlotFunction(graphic,this);
    QString tmp = dlg.getFunction();    
    p->setExpress(tmp);
    tmp = dlg.getStart();  
    QString e = dlg.getEnd();
    p->setDomain(tmp,e);
    XWTikzAddOperation * cmd = new XWTikzAddOperation(this,cur + 1,p);
    graphic->push(cmd);
  }
}

void XWTikzPath::addRectangle()
{
  if (ops.size() == 0)
    addMoveTo();

  XWTikzCoordDialog dlg(tr("Rectangle"));
  if (dlg.exec() == QDialog::Accepted)
  {
    QString tmp = dlg.getCoord();
    XWTikzRectangle * r = new XWTikzRectangle(graphic,this);
    r->setCoord(tmp);
    XWTikzAddOperation * cmd = new XWTikzAddOperation(this,cur + 1,r);
    graphic->push(cmd);
  }
}

void XWTikzPath::addSine()
{
  if (ops.size() == 0)
    addMoveTo();

  XWTikzCoordDialog dlg(tr("Sine"));
  if (dlg.exec() == QDialog::Accepted)
  {
    QString tmp = dlg.getCoord();
    XWTikzSine * s = new XWTikzSine(graphic,this);
    s->setCoord(tmp);
    XWTikzAddOperation * cmd = new XWTikzAddOperation(this,cur + 1,s);
    graphic->push(cmd);
  }
}

void XWTikzPath::addSplit(const QString & key)
{
  if (ops.size() == 0)
    addMoveTo();

  XWTikzDomainDialog dlg(key,tr("part 1:"),tr("part 2:"));
  if (dlg.exec() == QDialog::Accepted)
  {
    QString part1 = dlg.getStart();
    QString part2 = dlg.getEnd();
    QString str = QString("[shape=%1] {%1\n\\nodepart{text,lower}\n%2}").arg(key).arg(part1).arg(part2);
    XWTikzNode * n = new XWTikzNode(graphic,this);
    int len = str.length();
    int pos = 0;
    n->scan(str,len,pos);
    XWTikzAddOperation * cmd = new XWTikzAddOperation(this,cur + 1,n);
    graphic->push(cmd);
  }
}

void XWTikzPath::addVHLines()
{
  if (ops.size() == 0)
    addMoveTo();

  XWTikzCoordDialog dlg(tr("Vertical then horizontal"));
  if (dlg.exec() == QDialog::Accepted)
  {
    QString tmp = dlg.getCoord();
    XWTikzVHLinesTo * lines = new XWTikzVHLinesTo(graphic,this);
    lines->setCoord(tmp);
    XWTikzAddOperation * cmd = new XWTikzAddOperation(this,cur + 1,lines);
    graphic->push(cmd);
  }
}

bool XWTikzPath::back(XWTikzState * state)
{
  if (cur < 0 || cur >= ops.size())
    return false;

  options->doCompute(state);
  bool ret = false;
  switch (graphic->getCurrentScope())
  {
    default:
      break;

    case XW_TIKZ_S_PATH:
      if (cur > 0)
      {
        ret = true;
        XWTikzRemoveOperation * cmd = new XWTikzRemoveOperation(this,cur - 1);          
        graphic->push(cmd);
      }
      break;

    case XW_TIKZ_S_OPERATION:
      ret = ops[cur]->back(state);
      break;
  }
  return ret;
}

bool XWTikzPath::cut(XWTikzState * state)
{
  if (cur < 0 || cur >= ops.size())
    return false;

  options->doCompute(state);
  return ops[cur]->cut(state);
}

bool XWTikzPath::del(XWTikzState * state)
{
  if (cur < 0 || cur >= ops.size())
    return false;

  options->doCompute(state);
  bool ret = false;
  switch (graphic->getCurrentScope())
  {
    default:
      break;

    case XW_TIKZ_S_PATH:
      {
        ret = true;
        XWTikzRemoveOperation * cmd = new XWTikzRemoveOperation(this,cur);
        graphic->push(cmd);
      }
      break;

    case XW_TIKZ_S_OPERATION:
      ret = ops[cur]->del(state);
      break;
  }
  return ret;
}

void XWTikzPath::doOperation(XWTikzState * state, bool showpoint)
{
  if (cur < 0 || cur >= ops.size())
    return ;

  state = state->save(true);

  state->setDraw(true);
  state->setFill(false);
  state->setPattern(false);
  state->setShade(false);
  state->setClip(false);
  options->doPath(state,showpoint);

  for (int i = 0; i <= cur; i++)
    ops[i]->doPath(state,showpoint);

  state = state->restore();
}

void XWTikzPath::doPath(XWTikzState * state, bool showpoint)
{
  if (options->hasPre())
  {
    state = state->save(true);

    options->doPre(state);
    for (int i = 0; i < ops.size(); i++)
      ops[i]->doPath(state,showpoint);

    state = state->restore();
  }
  
  state = state->save(true);
  switch (keyWord)
  {
    default:
      break;

    case PGFdraw:
      state->setDraw(true);
      break;

    case PGFfill:
      state->setFill(true);
      break;

    case PGFfilldraw:
      state->setDraw(true);
      state->setFill(true);
      break;

    case PGFpattern:
      state->setPattern(true);
      break;

    case PGFshade:
      state->setShade(true);
      break;

    case PGFshadedraw:
      state->setShade(true);
      state->setDraw(true);
      break;

    case PGFclip:
      state->setClip(true);
      break;
  }

  options->doPath(state,showpoint);
  for (int i = 0; i < ops.size(); i++)
    ops[i]->doPath(state,showpoint);

  state = state->restore();

  if (options->hasPost())
  {
    state = state->save(true);

    options->doPost(state);
    for (int i = 0; i < ops.size(); i++)
      ops[i]->doPath(state,showpoint);

    state = state->restore();
  }
}

void XWTikzPath::doPathNoOptions(XWTikzState * state)
{
  state = state->save(true);
  for (int i = 0; i < ops.size(); i++)
    ops[i]->doPath(state,false);
  state = state->restore();
}

void XWTikzPath::dragTo(XWTikzState * state)
{
  state = state->save(true);

  options->doCompute(state);
  state->setDraw(true);
  state->setFill(false);
  state->setPattern(false);
  state->setShade(false);
  state->setClip(false);
  state->setDash(PGFdotted);
  state->setLineWidth(0.1);
  state->setDrawColor(Qt::lightGray);  
  switch (graphic->getCurrentScope())
  {
    default:
      for (int i = 0; i < ops.size(); i++)
        ops[i]->dragTo(state);
      break;

    case XW_TIKZ_S_OPERATION:
      for (int i = 0; i <= cur; i++)
        ops[i]->dragTo(state);
      break;
  }

  state = state->restore();
}

bool XWTikzPath::dropTo(XWTikzState * state)
{
  options->doCompute(state);
  bool ret = false;
  switch (graphic->getCurrentScope())
  {
    default:
      for (int i = 0; i < ops.size(); i++)
      {
        ret = ops[i]->dropTo(state);
        if (ret)
          break;
      }
      break;

    case XW_TIKZ_S_OPERATION:
      if (cur >= 0 && cur < ops.size())
        ret = ops[cur]->dropTo(state);
      break;
  }
  return ret;
}

QPointF XWTikzPath::getAnchor(const QString & nameA,int a,XWTikzState * state)
{
  options->doCompute(state);
  QPointF ret;
  for (int i = 0; i < ops.size(); i++)
  {
    ops[i]->doPath(state,false);
    if (ops[i]->isMe(nameA,state))
    {
      ret = ops[i]->getAnchor(a,state);
      break;
    }
  }
  return ret;
}

int XWTikzPath::getAnchorPosition()
{
  if (cur < 0 || cur >= ops.size())
    return 0;

  return ops[cur]->getAnchorPosition();
}

QPointF XWTikzPath::getAngle(const QString & nameA,double a,XWTikzState * state)
{
  options->doCompute(state);
  QPointF ret;
  for (int i = 0; i < ops.size(); i++)
  {
    ops[i]->doPath(state,this);
    if (ops[i]->isMe(nameA,state))
    {
      ret = ops[i]->getAngle(a,state);
      break;
    }
  }
  return ret;
}

QPointF XWTikzPath::getBorder(const QString & nameA,const QPointF & p,XWTikzState * state)
{
  options->doCompute(state);
  QPointF ret;
  for (int i = 0; i < ops.size(); i++)
  {
    ops[i]->doPath(state,this);
    if (ops[i]->isMe(nameA,state))
    {
      ret = ops[i]->getBorder(p,state);
      break;
    }
  }
  return ret;
}

QPointF XWTikzPath::getCenter(XWTikzState * state)
{
  options->doCompute(state);
  QPointF ret;
  for (int i = 0; i < ops.size(); i++)
    ops[i]->doCompute(state);
  state->computePath();
  return state->getCenter();
}

XWTikzOperation * XWTikzPath::getCurrentOperation()
{
  if (cur < 0 || cur >= ops.size())
    return 0;

  return ops[cur];
}

XWTikzCoord * XWTikzPath::getCurrentPoint()
{
  if (cur < 0 || cur >= ops.size())
    return 0;

  return ops[cur]->getCurrentPoint();
}

QList<QPointF> XWTikzPath::getIntersections(const QList<int> & operationsA,
                                  const QList<QPointF> & pointsA,
                                  XWTikzState * state)
{
  options->doCompute(state);
  for (int i = 0; i < ops.size(); i++)
    ops[i]->doPath(state,false);

  QList<QPointF> ret = state->getIntersections(operationsA,pointsA);
  return ret;
}

void XWTikzPath::getPath(QList<int> & operationsA,
                         QList<QPointF> & pointsA,
                         XWTikzState * state)
{
  options->doCompute(state);
  for (int i = 0; i < ops.size(); i++)
    ops[i]->doPath(state,false);

  state->getPath(operationsA,pointsA);
}

int XWTikzPath::getCursorPosition()
{
  if (cur < 0 || cur >= ops.size())
    return 0;

  return ops[cur]->getCursorPosition();
}

QString XWTikzPath::getCurrentText()
{
  if (cur < 0 || cur >= ops.size())
    return QString();

  return ops[cur]->getCurrentText();
}

QPointF XWTikzPath::getPoint(const QString & nameA, XWTikzState * state)
{
  options->doCompute(state);
  QPointF ret;
  for (int i = 0; i < ops.size(); i++)
  {
    ops[i]->doPath(state,false);
    if (ops[i]->isMe(nameA,state))
    {
      if (nameA.contains("."))
        ret = ops[i]->getAnchor(state);
      else
        ret = state->getCurrentPoint();
      break;
    }
  }
  return ret;
}

QVector3D XWTikzPath::getPoint3D(const QString & nameA,XWTikzState * state)
{
  options->doCompute(state);
  QVector3D ret;
  for (int i = 0; i < ops.size(); i++)
  {
    ops[i]->doPath(state,false);
    if (ops[i]->isMe(nameA,state))
    {
      if (nameA.contains("."))
      {
        QPointF p = ops[i]->getAnchor(state);
        ret.setX(p.x());
        ret.setY(p.y());
      }
      else
      {
        XWTikzCoord * c = state->getCurrentCoord();
        if (c)
          ret = c->getPoint3D(state);
      }
      break;
    }
  }
  return ret;
}

double XWTikzPath::getRadius(const QString & nameA,XWTikzState * state)
{
  options->doCompute(state);
  double ret;
  for (int i = 0; i < ops.size(); i++)
  {
    ops[i]->doPath(state,this);
    if (ops[i]->isMe(nameA,state))
    {
      ret = ops[i]->getRadius(state);
      break;
    }
  }
  return ret;
}

QString XWTikzPath::getSelectedText()
{
  if (cur < 0 || cur >= ops.size())
    return QString();

  return ops[cur]->getSelectedText();
}

QString XWTikzPath::getText()
{
  QString ret;
  switch (keyWord)
  {
    default:
      break;

    case PGFpath:
      ret = "\\path";
      break;

    case PGFdraw:
      ret = "\\draw";
      break;

    case PGFfill:
      ret = "\\fill";
      break;

    case PGFfilldraw:
      ret = "\\filldraw";
      break;

    case PGFpattern:
      ret = "\\pattern";
      break;

    case PGFshade:
      ret = "\\shade";
      break;

    case PGFshadedraw:
      ret = "\\shadedraw";
      break;

    case PGFclip:
      ret = "\\clip";
      break;
  }

  QString tmp = options->getText();
  ret += tmp;

  for (int i = 0; i < ops.size(); i++)
  {
    tmp = ops[i]->getText();
    ret += tmp;    
  }

  ret += ";";

  return ret;
}

QString XWTikzPath::getTips(XWTikzState * state)
{
  if (cur < 0 || cur >= ops.size())
    return QString();

  options->doCompute(state);
  QString ret;
  switch (graphic->getCurrentScope())
  {
    case XW_TIKZ_S_OPERATION:
      ret = ops[cur]->getTips(state);
      break;

    default:
      break;
  }
  return ret;
}

void XWTikzPath::getWidthAndHeight(double & w, double & h)
{
  w = 0;
  h = 0;
  XWTikzState state;
  options->doCompute(&state);
  for (int i = 0; i < ops.size(); i++)
    ops[i]->doCompute(&state);

  state.computePath();
  w = state.getWidth();
  h = state.getHeight();
}

void XWTikzPath::goToEnd()
{
  if (ops.size() == 0)
    return ;

  cur = ops.size() - 1;
  ops[cur]->goToEnd();
}

bool XWTikzPath::goToNext()
{
  if (cur >= ops.size() -1)
    return false;

  cur++;
  ops[cur]->goToStart();
  return true;
}

void XWTikzPath::goToNextPoint()
{
  if (cur < 0 || cur >= ops.size())
    return ;

  ops[cur]->goToNext();
}

void XWTikzPath::goToOperationEnd()
{
  if (cur < 0 || cur >= ops.size())
    return ;

  ops[cur]->goToEnd();
}

void XWTikzPath::goToOperationStart()
{
  if (cur < 0 || cur >= ops.size())
    return ;

  ops[cur]->goToStart();
}

void XWTikzPath::goToPathEnd()
{
  goToEnd();
}

void XWTikzPath::goToPathStart()
{
  goToStart();
}

bool XWTikzPath::goToPrevious()
{
  if (cur <= 0)
    return false;

  cur--;
  ops[cur]->goToEnd();
  return true;
}

void XWTikzPath::goToPreviousPoint()
{
  if (cur < 0 || cur >= ops.size())
    return ;

  ops[cur]->goToPrevious();
}

void XWTikzPath::goToStart()
{
  if (ops.size() <= 0)
    return ;

  cur = 0;
  ops[cur]->goToStart();
}

bool XWTikzPath::hitTest(XWTikzState * state)
{
  options->doCompute(state);
  bool ret = false;
  switch (graphic->getCurrentScope())
  {
    default:
      cur = -1;
      for (int i = 0; i < ops.size(); i++)
      {
        ret = ops[i]->hitTest(state);
        if (ret)
        {
          cur = i;
          break;
        }
      }
      break;

    case XW_TIKZ_S_OPERATION:
      if (cur >= 0 && cur < ops.size())
        ret = ops[cur]->hitTest(state);
      break;
  }
  
  return ret;
}

void XWTikzPath::insert(int i, XWTikzOperation * opA)
{
  cur = i;
  if (i >= ops.size())
    ops << opA;
  else
    ops.insert(i,opA);
}

bool XWTikzPath::insertText(XWTikzState * state)
{
  if (cur < 0 || cur >= ops.size())
    return false;

  options->doCompute(state);
  return ops[cur]->insertText(state);
}

bool XWTikzPath::keyInput(XWTikzState * state)
{
  if (cur < 0 || cur >= ops.size())
    return false;

  options->doCompute(state);
  return ops[cur]->keyInput(state);
}

bool XWTikzPath::newPar(XWTikzState * state)
{
  if (cur < 0 || cur >= ops.size())
    return false;

  options->doCompute(state);
  return ops[cur]->newPar(state);
}

bool XWTikzPath::paste(XWTikzState * state)
{
  if (cur < 0 || cur >= ops.size())
    return false;

  options->doCompute(state);
  return ops[cur]->paste(state);
}

void XWTikzPath::scan(const QString & str, int & len, int & pos)
{
  options->scan(str,len,pos);
  XWTikzOperation * obj = 0;
  while (pos < len)
  {
    if (str[pos].isSpace())
      pos++;
    else if (str[pos] == QChar('%'))
      XWTeXBox::skipComment(str,len,pos);
    else if (str[pos] == QChar(';'))
    {
      pos++;
      break;
    }      
    else
    {
      int c = str[pos].unicode();
      switch (c)
      {
        case '(':
          obj = new XWTikzCoord(graphic,this);
          break;

        case '|':
          pos += 2;
          obj = new XWTikzVHLinesTo(graphic,this);
          break;

        case '\\':
          obj = new XWTikzMacro(graphic,this);
          break;

        case '-':
          pos++;
          if (str[pos] == QChar('|'))
          {
            pos++;
            obj = new XWTikzHVLinesTo(graphic,this);
          }            
          else
          {
            pos++;
            while (str[pos].isSpace())
              pos++;
            if (str[pos] == QChar('(') || str[pos] == QChar('+'))
              obj = new XWTikzLineTo(graphic,this);
            else
            {
              pos += 4;
              XWTIKZOptions * opts = new XWTIKZOptions(graphic);
              opts->scan(str,len,pos);
              while (str[pos].isSpace())
                pos++;
              if (str[pos] == QChar('c'))
              {
                pos += 11;
                obj = new XWTikzPlotCoordinates(graphic,true,opts,this);
              }                
              else if (str[pos] == QChar('('))
                obj = new XWTikzPlotFunction(graphic,true,opts,this);
              else
              {
                pos += 4;
                obj = new XWTikzPlotFile(graphic,true,opts,this);
              }
            }
          }
          break;

        case 'a':
          pos += 3;
          obj = new XWTikzArc(graphic,this);
          break;

        case 'c':
          pos++;
          if (str[pos] == QChar('i'))
          {
            pos += 5;
            obj = new XWTikzEllipse(graphic,PGFcircle,this);
          }
          else if (str[pos] == QChar('o'))
          {
            pos++;
            if (str[pos] == QChar('n'))
            {
              pos += 6;
              obj = new XWTikzCurveTo(graphic,this);
            }
            else if (str[pos] == QChar('s'))
            {
              pos++;
              obj = new XWTikzCosine(graphic,this);
            }      
            else
            {
              pos += 8;
              obj = new XWTikzCoordinate(graphic,this);
            }      
          }
          else if (str[pos] == QChar('h'))
          {
            pos += 4;
            obj = new XWTikzChild(graphic,this);
          }
          else
          {
            pos += 4;
            obj = new XWTikzCycle(graphic,this);
          }            
          break;

        case 'd':
          pos += 8;
          obj = new XWTikzDecorate(graphic,this);
          break;

        case 'e':
          pos++;
          if (str[pos] == QChar('d'))
          {
            pos += 3;
            while (str[pos].isSpace())
              pos++;
            obj = new XWTikzEdge(graphic,this);
          }
          else
          {
            pos += 6;
            obj = new XWTikzEllipse(graphic,PGFellipse,this);
          }
          break;

        case 'f':
          pos += 7;
          obj = new XWTikzForeach(graphic,scope,this);
          break;

        case 'g':
          pos += 4;
          obj = new XWTikzGrid(graphic,this);
          break;

        case 'n':
          pos += 4;
          obj = new XWTikzNode(graphic,this);
          break;

        case 'p':
          pos++;
          if (str[pos] == QChar('l'))
          {
            pos += 3;
            XWTIKZOptions * opts = new XWTIKZOptions(graphic);
            opts->scan(str,len,pos);
            while (str[pos].isSpace())
              pos++;
            if (str[pos] == QChar('c'))
            {
              pos += 11;
              obj = new XWTikzPlotCoordinates(graphic,false,opts,this);
            }                
            else if (str[pos] == QChar('('))
              obj = new XWTikzPlotFunction(graphic,false,opts,this);
            else
            {
              pos += 4;
              obj = new XWTikzPlotFile(graphic,false,opts,this);
            }
          }
          else
          {
            pos += 7;
            obj = new XWTikzParabola(graphic,this);
          }          
          break;

        case 's':
          pos += 3;
          obj = new XWTikzSine(graphic,this);
          break;

        case 't':
          pos += 2;
          obj = new XWTikzTo(graphic,this);
          break;

        case 'r':
          pos += 9;
          obj = new XWTikzRectangle(graphic,this);
          break;

        case '[':
          obj = new XWTIKZOptions(graphic,this);
          break;

        case '{':
          obj = new XWTikzLocal(graphic,this);
          break;

        default:
          break;
      }

      if (obj)
      {
        ops << obj;
        obj->scan(str,len,pos);
      }
    }
  }

  cur = ops.size() - 1;
}

XWTikzOperation * XWTikzPath::takeAt(int i)
{
  cur = i - 1;
  return ops.takeAt(i);
}

XWTikzScope::XWTikzScope(XWTikzGraphic * graphicA,XWTikzScope *scopeA,int idA,QObject * parent)
:XWTikzCommand(graphicA,scopeA,idA,parent),
cur(-1),
namedPath1(-1),
namedPath2(-1),
sortBy(-1)
{}

XWTikzScope::~XWTikzScope()
{}

void XWTikzScope::addArc()
{
  if ((cur >= 0) && (cur < (cmds.size() - 1)))
    cmds[cur]->addArc();
}

void XWTikzScope::addCircle()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addCircle();
}

void XWTikzScope::addCoordinateCommand()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)) && 
      ((cmds[cur]->getKeyWord() == PGFscope) ||
       (cmds[cur]->getKeyWord() == XW_TIKZ_GROUP)))
  {
    XWTikzScope * s = (XWTikzScope*)(cmds[cur]);
    s->addCoordinateCommand();
    return ;
  }

  XWTikzCoordinateCommandDialog dlg;
  if (dlg.exec() == QDialog::Accepted)
  {
    XWTikzCoordinatePath * node = new XWTikzCoordinatePath(graphic,this,this);
    QString tmp = dlg.getName();
    node->setName(tmp);
    tmp = dlg.getCoord();
    node->setAt(tmp);
    XWTikzAddScopePath * cmd = new XWTikzAddScopePath(this,cur + 1,node);
    graphic->push(cmd);
  }
}

void XWTikzScope::addCoordinate()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addCoordinate();
}

void XWTikzScope::addCosine()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addCosine();
}

void XWTikzScope::addCurveTo()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addCurveTo();
}

void XWTikzScope::addCycle()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addCycle();
}

void XWTikzScope::addEdge()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addEdge();
}

void XWTikzScope::addEllipse()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addEllipse();
}

void XWTikzScope::addGrid()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addGrid();
}

void XWTikzScope::addHVLines()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addHVLines();
}

void XWTikzScope::addLineTo()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addLineTo();
}

void XWTikzScope::addMoveTo()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addMoveTo();
}

void XWTikzScope::addNode()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addNode();
}

void XWTikzScope::addNodeCommand()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)) && 
      ((cmds[cur]->getKeyWord() == PGFscope) ||
       (cmds[cur]->getKeyWord() == XW_TIKZ_GROUP)))
  {
    XWTikzScope * s = (XWTikzScope*)(cmds[cur]);
    s->addNodeCommand();
    return ;
  }

  XWTikzNodeCommandDialog dlg;
  if (dlg.exec() == QDialog::Accepted)
  {
    XWTikzNodePath * node = new XWTikzNodePath(graphic,this,this);
    QString tmp = dlg.getName();
    node->setName(tmp);
    tmp = dlg.getCoord();
    node->setAt(tmp);
    tmp = dlg.getText();
    node->setText(tmp);
    XWTikzAddScopePath * cmd = new XWTikzAddScopePath(this,cur + 1,node);
    graphic->push(cmd);
  }
}

void XWTikzScope::addParabola()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addParabola();
}

void XWTikzScope::addPath(int keywordA)
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)) && 
      ((cmds[cur]->getKeyWord() == PGFscope) ||
       (cmds[cur]->getKeyWord() == XW_TIKZ_GROUP)))
  {
    XWTikzScope * s = (XWTikzScope*)(cmds[cur]);
    s->addPath(keywordA);
    return ;
  }

  XWTikzPath * path = new XWTikzPath(graphic,this,keywordA,this);
  int index = cur + 1;
  XWTikzAddScopePath * cmd = new XWTikzAddScopePath(this,index,path);
  graphic->push(cmd);
}

void XWTikzScope::addPlotCoordinates()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addPlotCoordinates();
}

void XWTikzScope::addPlotFile()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addPlotFile();
}

void XWTikzScope::addPlotFunction()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addPlotFunction();
}

void XWTikzScope::addRectangle()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addRectangle();
}

void XWTikzScope::addScope()
{
  XWTikzScope * s = new XWTikzScope(graphic,this,PGFscope,this);
  XWTikzAddScopePath * cmd = new XWTikzAddScopePath(this,cur + 1,s);
  graphic->push(cmd);
}

void XWTikzScope::addScopeAction(QMenu & menu, XWTikzState * state)
{
  options->doCompute(state);
  QMenu * submenu = 0;
  switch (state->getPictureType())
  {
    default:
      submenu = menu.addMenu(tr("draw"));
      options->addLineAction(*submenu);
      options->addArrowsAction(*submenu);
      options->addRoundedCornersAction(*submenu);
      options->addDoubleDistanceAction(*submenu);
      menu.addSeparator();
      submenu = menu.addMenu(tr("decoration"));
      options->addDecorationAction(*submenu);
      menu.addSeparator();
      submenu = menu.addMenu(tr("color"));
      options->addColorAction(*submenu);
      options->addOpacityAction(*submenu);
      options->addDoubleAction(*submenu);
      submenu = menu.addMenu(tr("pattern"));
      options->addPatternAction(*submenu);
      menu.addSeparator();
      submenu = menu.addMenu(tr("shade"));
      options->addShadeAction(*submenu);
      menu.addSeparator();
      submenu = menu.addMenu(tr("fading"));
      options->addPathFading(*submenu);
      options->addScopeFading(*submenu);
      menu.addSeparator();
      submenu = menu.addMenu(tr("node"));
      options->addShapeAction(*submenu);
      options->addAnchorAction(*submenu);
      menu.addSeparator();
      submenu = menu.addMenu(tr("spy"));
      options->addSpyAction(*submenu);
      submenu = menu.addMenu(tr("transform"));
      options->addScaleAction(*submenu);
      options->addShiftAction(*submenu);
      options->addRotateAction(*submenu);
      options->addSlantAction(*submenu);
      menu.addSeparator();
      options->addDomainAction(menu);
      menu.addSeparator();
      options->addCircuitAction(menu);
      menu.addSeparator();
      submenu = menu.addMenu(tr("3D"));
      options->addPlaneAction(*submenu);
      break;

    case PGFmindmap:
      options->addConceptColorAction(menu);
      menu.addSeparator();
      options->addOpacityAction(menu);
      menu.addSeparator();
      options->addShadeAction(menu);
      menu.addSeparator();
      options->addPathFading(menu);
      options->addScopeFading(menu);
      break;

    case PGFcircuit:
    case PGFcircuiteeIEC:
    case PGFcircuitlogic:
    case PGFcircuitlogicIEC:
    case PGFcircuitlogicUS:
    case PGFcircuitlogicCDH:
      options->addCircuitAction(menu); 
      menu.addSeparator();
      options->addCircuitSymbolAction(menu);
      menu.addSeparator();
      submenu = menu.addMenu(tr("draw"));
      options->addLineAction(*submenu);
      submenu = menu.addMenu(tr("color"));
      options->addColorAction(*submenu);
      options->addOpacityAction(*submenu);
      break;
  }
}

void XWTikzScope::addSine()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addSine();
}

void XWTikzScope::addSplit(const QString & key)
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addSplit(key);
}

void XWTikzScope::addSpy()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)) && 
      ((cmds[cur]->getKeyWord() == PGFscope) ||
       (cmds[cur]->getKeyWord() == XW_TIKZ_GROUP)))
  {
    XWTikzScope * s = (XWTikzScope*)(cmds[cur]);
    s->addSpy();
    return ;
  }

  XWTikzTwoCoordDialog dlg(tr("spy"),tr("on"), tr("at"));
  if (dlg.exec() == QDialog::Accepted)
  {
    XWTikzSpy * spy = new XWTikzSpy(graphic,this,this);
    QString on = dlg.getCoord1();
    QString at = dlg.getCoord2();
    spy->setOn(on);
    spy->setAt(at);
    int index = cur + 1;
    XWTikzAddScopePath * cmd = new XWTikzAddScopePath(this,index,spy);
    graphic->push(cmd);
  }  
}

void XWTikzScope::addVHLines()
{
  if ((cur >= 0) && (cur <= (cmds.size() - 1)))
    cmds[cur]->addVHLines();
}

bool XWTikzScope::back(XWTikzState * state)
{
  if (cur < 0 || cur >= cmds.size())
    return false;

  options->doCompute(state);
  bool ret = false;
  switch (graphic->getCurrentScope())
  {
    default:
      ret = cmds[cur]->back(state);
      break;

    case XW_TIKZ_S_SCOPE:
      if (cur > 0)
      {
        ret = true;
        XWTikzRemoveScopePath * cmd = new XWTikzRemoveScopePath(this,cur - 1);          
        graphic->push(cmd);
      }
      break;
  }
  return ret;
}

bool XWTikzScope::cut(XWTikzState * state)
{
  if (cur < 0 || cur >= cmds.size())
    return false;

  options->doCompute(state);
  return cmds[cur]->cut(state);
}

bool XWTikzScope::del(XWTikzState * state)
{
  if (cur < 0 || cur >= cmds.size())
    return false;

  options->doCompute(state);
  bool ret = false;
  switch (graphic->getCurrentScope())
  {
    default:
      ret = cmds[cur]->del(state);
      break;

    case XW_TIKZ_S_SCOPE:
      {
        ret = true;
        XWTikzRemoveScopePath * cmd = new XWTikzRemoveScopePath(this,cur);
        graphic->push(cmd);
      }
      break;
  }
  return ret;
}

void XWTikzScope::doCopy(XWTikzState * state)
{
  options->doPath(state,false);
  QList<XWTikzCommand*> spies;
  int tcur = cur;
  for (int i = 0; i < cmds.size(); i++)
  {
    cur = i;
    if (cmds[i]->getKeyWord() == PGFscope || 
        cmds[i]->getKeyWord() == XW_TIKZ_GROUP)
    {
      XWTikzScope * s = (XWTikzScope*)(cmds[i]);
      s->doCopy(state);
    }
    else if (cmds[i]->getKeyWord() == PGFspy)
      spies << cmds[i];
    else
      cmds[i]->doPath(state,false);
  }
  cur = tcur;
}

void XWTikzScope::doDecoration(XWTikzState * state)
{
  options->doDecoration(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doDecoration(state);
}

void XWTikzScope::doEdgeFromParent(XWTikzState * state)
{
  options->doEdgeFromParent(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEdgeFromParent(state);
}

void XWTikzScope::doEdgeFromParentPath(XWTikzState * state)
{
  options->doEdgeFromParentPath(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEdgeFromParentPath(state);
}

void XWTikzScope::doEveryAcceptingByArrow(XWTikzState * state)
{
  options->doEveryAcceptingByArrow(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryAcceptingByArrow(state);
}

void XWTikzScope::doEveryAttribute(XWTikzState * state)
{
  options->doEveryAttribute(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryAttribute(state);
}

void XWTikzScope::doEveryChild(XWTikzState * state)
{
  options->doEveryChild(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryChild(state);
}

void XWTikzScope::doEveryChildNode(XWTikzState * state)
{
  options->doEveryChildNode(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryChildNode(state);
}

void XWTikzScope::doEveryCircuitAnnotation(XWTikzState * state)
{
  options->doEveryCircuitAnnotation(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryCircuitAnnotation(state);
}

void XWTikzScope::doEveryCircuitSymbol(XWTikzState * state)
{
  options->doEveryCircuitSymbol(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryCircuitSymbol(state);
}

void XWTikzScope::doEveryConcept(XWTikzState * state)
{
  options->doEveryConcept(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryConcept(state);
}

void XWTikzScope::doEveryCurveTo(XWTikzState * state)
{
  options->doEveryCurveTo(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryCurveTo(state);
}

void XWTikzScope::doEveryEdge(XWTikzState * state)
{
  options->doEveryEdge(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryEdge(state);
}

void XWTikzScope::doEveryEntity(XWTikzState * state)
{
  options->doEveryEntity(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryEntity(state);
}

void XWTikzScope::doEveryInfo(XWTikzState * state)
{
  options->doEveryInfo(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryInfo(state);
}

void XWTikzScope::doEveryInitialByArrow(XWTikzState * state)
{
  options->doEveryInitialByArrow(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryInitialByArrow(state);
}

void XWTikzScope::doEveryLabel(XWTikzState * state)
{
  options->doEveryLabel(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryLabel(state);
}

void XWTikzScope::doEveryLoop(XWTikzState * state)
{
  options->doEveryLoop(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryLoop(state);
}

void XWTikzScope::doEveryMark(XWTikzState * state)
{
  options->doEveryMark(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryMark(state);
}

void XWTikzScope::doEveryMatrix(XWTikzState * state)
{
  options->doEveryMatrix(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryMatrix(state);
}

void XWTikzScope::doEveryMindmap(XWTikzState * state)
{
  options->doEveryMindmap(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryMindmap(state);
}

void XWTikzScope::doEveryNode(XWTikzState * state)
{
  options->doEveryNode(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryNode(state);
}

void XWTikzScope::doEveryPin(XWTikzState * state)
{
  options->doEveryPin(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryPin(state);
}

void XWTikzScope::doEveryPinEdge(XWTikzState * state)
{
  options->doEveryPinEdge(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryPinEdge(state);
}

void XWTikzScope::doEveryPlace(XWTikzState * state)
{
  options->doEveryPlace(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryPlace(state);
}

void XWTikzScope::doEveryRelationship(XWTikzState * state)
{
  options->doEveryRelationship(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryRelationship(state);
}

void XWTikzScope::doEveryShadow(XWTikzState * state)
{
  options->doEveryShadow(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryShadow(state);
}

void XWTikzScope::doEveryShape(XWTikzState * state)
{
  options->doEveryShape(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryShape(state);
}

void XWTikzScope::doEveryState(XWTikzState * state)
{
  options->doEveryState(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryState(state);
}

void XWTikzScope::doEveryToken(XWTikzState * state)
{
  options->doEveryToken(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryToken(state);
}

void XWTikzScope::doEveryTransition(XWTikzState * state)
{
  options->doEveryTransition(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doEveryTransition(state);
}

void XWTikzScope::doLevel(XWTikzState * state)
{
  options->doLevel(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doLevel(state);
}

void XWTikzScope::doLevelConcept(XWTikzState * state)
{
  options->doLevelConcept(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doLevelConcept(state);
}

void XWTikzScope::doLevelNumber(XWTikzState * state)
{
  options->doLevelNumber(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doLevelNumber(state);
}

void XWTikzScope::doOperation(XWTikzState * state, bool showpoint)
{
  if (cur < 0 || cur >= cmds.size())
    return ;

  state = state->save();

  options->doPath(state,showpoint);
  cmds[cur]->doOperation(state,showpoint);

  state = state->restore();
}

void XWTikzScope::doPath(XWTikzState * state, bool showpoint)
{
  if (cur < 0 || cur >= cmds.size())
    return ;

  state = state->save();

  options->doPath(state,showpoint);
  cmds[cur]->doPath(state,showpoint);  

  state = state->restore();
}

void XWTikzScope::doPathNoOptions(XWTikzState * state)
{
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doPath(state,false);
}

void XWTikzScope::doRootConcept(XWTikzState * state)
{
  options->doRootConcept(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doRootConcept(state);
}

void XWTikzScope::doScope(XWTikzState * state)
{
  state = state->save();

  options->doPath(state);
  int tcur = cur;
  QList<XWTikzCommand*> spies;
  for (int i = 0; i < cmds.size(); i++)
  {
    cur = i;
    if (cmds[i]->getKeyWord() == PGFscope || 
        cmds[i]->getKeyWord() == XW_TIKZ_GROUP)
    {
      XWTikzScope * s = (XWTikzScope*)(cmds[i]);
      s->doScope(state);
    }
    else if (cmds[i]->getKeyWord() == PGFspy)
      spies << cmds[i];
    else
      cmds[i]->doPath(state,false);
  }

  cur = tcur;

  for (int i = 0; i < spies.size(); i++)
    spies[i]->doPath(state,false);

  state = state->restore();
}

void XWTikzScope::doToPath(XWTikzState * state)
{
  options->doToPath(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doToPath(state);
}

void XWTikzScope::doSpyConnection(XWTikzState * state)
{
  options->doSpyConnection(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doSpyConnection(state);
}

void XWTikzScope::doSpyNode(XWTikzState * state)
{
  options->doSpyNode(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doSpyNode(state);
}

void XWTikzScope::doState(XWTikzState * state)
{
  options->doState(state);
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->doState(state);
}

void XWTikzScope::dragTo(XWTikzState * state)
{
  if (cur < 0 || cur >= cmds.size())
    return ;

  options->doCompute(state);
  cmds[cur]->dragTo(state);
}

bool XWTikzScope::dropTo(XWTikzState * state)
{
  options->doCompute(state);
  bool ret = false;
  switch (graphic->getCurrentScope())
  {
    case XW_TIKZ_S_OPERATION:
    case XW_TIKZ_S_PATH:
      if (cur >= 0 && cur < cmds.size())
       ret = cmds[cur]->dropTo(state);
       break;

    default:
      for (int i = 0; i < cmds.size(); i++)
      {
        ret = cmds[i]->dropTo(state);
        if (ret)
          break;
      }
      break;
  }
  return ret;
}

QPointF XWTikzScope::getAnchor(const QString & nameA,int a, XWTikzState * state)
{
  options->doCompute(state);
  QString n = nameA;
  if (nameA.contains("."))
  {
    int index = nameA.indexOf(".");
    n = nameA.left(index);
  }

  QPointF ret;
  if (names.contains(n))
  {
    int i = names[n];
    ret = cmds[i]->getAnchor(nameA,a,state);
  }

  return ret;
}

int XWTikzScope::getAnchorPosition()
{
  if (cur < 0 || cur >= cmds.size())
    return 0;

  return cmds[cur]->getAnchorPosition();
}

QPointF XWTikzScope::getAngle(const QString & nameA,double a, XWTikzState * state)
{
  options->doCompute(state);
  QString n = nameA;
  if (nameA.contains("."))
  {
    int index = nameA.indexOf(".");
    n = nameA.left(index);
  }

  QPointF ret;
  if (names.contains(n))
  {
    int i = names[n];
    ret = cmds[i]->getAngle(nameA,a,state);
  }
  return ret;
}

QPointF XWTikzScope::getBorder(const QString & nameA,const QPointF & p,XWTikzState * state)
{
  options->doCompute(state);
  QString n = nameA;
  if (nameA.contains("."))
  {
    int index = nameA.indexOf(".");
    n = nameA.left(index);
  }

  QPointF ret;
  if (names.contains(n))
  {
    int i = names[n];
    ret = cmds[i]->getBorder(nameA,p,state);
  }
  return ret;
}

QPointF XWTikzScope::getCenter(XWTikzState * state)
{
  QPointF ret;
  if (cur < 0 || cur >= cmds.size())
    return ret;

  options->doCompute(state);
  return cmds[cur]->getCenter(state);
}

XWTikzOperation * XWTikzScope::getCurrentOperation()
{
  if (cur < 0 || cur >= cmds.size())
    return 0;

  return cmds[cur]->getCurrentOperation();
}

XWTikzCoord * XWTikzScope::getCurrentPoint()
{
  if (cur < 0 || cur >= cmds.size())
    return 0;

  return cmds[cur]->getCurrentPoint();
}

int XWTikzScope::getCursorPosition()
{
  if (cur < 0 || cur >= cmds.size())
    return 0;

  return cmds[cur]->getCursorPosition();
}

QString XWTikzScope::getCurrentText()
{
  if (cur < 0 || cur >= cmds.size())
    return QString();

  return cmds[cur]->getCurrentText();
}

void XWTikzScope::getIntersections(const QString & nameA,const QString & nameB)
{
  if (names.contains(nameA))
  {
    namedPath1 = names[nameA];
    if (cmds[namedPath1]->getKeyWord() == PGFscope || 
      cmds[namedPath1]->getKeyWord() == XW_TIKZ_GROUP)
    {
      XWTikzScope * s = (XWTikzScope*)(cmds[namedPath1]);
      s->getIntersections(nameA,nameB);
    }
  }
  else
    namedPath1 = -1;

  if (names.contains(nameB))
  {
    namedPath2 = names[nameB];
    if (cmds[namedPath2]->getKeyWord() == PGFscope || 
      cmds[namedPath2]->getKeyWord() == XW_TIKZ_GROUP)
    {
      XWTikzScope * s = (XWTikzScope*)(cmds[namedPath2]);
      s->getIntersections(nameA,nameB);
    }
  }
  else
    namedPath2 = -1;
}

QList<QPointF> XWTikzScope::getIntersections(const QList<int> & operationsA,
                                  const QList<QPointF> & pointsA,
                                  XWTikzState * state)
{
  state = state->save();
  QList<QPointF> ret;

  if (namedPath1 > -1)
    ret = cmds[namedPath1]->getIntersections(operationsA,pointsA,state);
  else if (namedPath2 > -1)
    ret = cmds[namedPath2]->getIntersections(operationsA,pointsA,state);

  state = state->restore();
  return ret;
}

void XWTikzScope::getPath(QList<int> & operationsA,
                  QList<QPointF> & pointsA,
                  XWTikzState * state)
{
  options->doPath(state,false);
  if (namedPath2 > -1)
    cmds[namedPath2]->getPath(operationsA,pointsA,state);
  else if (namedPath1 > -1)
    cmds[namedPath1]->getPath(operationsA,pointsA,state);
}

QPointF XWTikzScope::getPoint(const QString & nameA, XWTikzState * state)
{
  options->doCompute(state);
  QString n = nameA;
  if (nameA.contains("."))
  {
    int index = nameA.indexOf(".");
    n = nameA.left(index);
  }

  QPointF ret;
  if (names.contains(n))
  {
    int i = names[n];
    ret = cmds[i]->getPoint(nameA,state);
  }

  return ret;
}

QVector3D XWTikzScope::getPoint3D(const QString & nameA,XWTikzState * state)
{
  options->doCompute(state);
  QString n = nameA;
  if (nameA.contains("."))
  {
    int index = nameA.indexOf(".");
    n = nameA.left(index);
  }

  QVector3D ret;
  if (names.contains(n))
  {
    int i = names[nameA];
    ret = cmds[i]->getPoint3D(nameA,state);
  }

  return ret;
}

double XWTikzScope::getRadius(const QString & nameA,XWTikzState * state)
{
  options->doCompute(state);
  QString n = nameA;
  if (nameA.contains("."))
  {
    int index = nameA.indexOf(".");
    n = nameA.left(index);
  }

  double ret;
  if (names.contains(n))
  {
    int i = names[n];
    ret = cmds[i]->getRadius(nameA,state);
  }

  return ret;
}

QString XWTikzScope::getSelectedText()
{
  if (cur < 0 || cur >= cmds.size())
    return QString();

  return cmds[cur]->getSelectedText();
}

QString XWTikzScope::getText()
{
  QString ret;
  if (cmds.size() > 0)
  {
    if (keyWord == PGFscope)
    {
      QString ops = options->getText();
      QString body;
      for (int i = 0; i < cmds.size(); i++)
      {
        QString tmp = cmds[i]->getText();
        body += tmp;
        body += "\n";
      }

      if (ops.isEmpty())
        ret = QString("\\begin{scope}\n%1\\end{scope}\n").arg(body);
      else
        ret = QString("\\begin{scope}%1\n%2\\end{scope}\n").arg(ops).arg(body);
    }
    else
    {
      ret = "{\n";
      for (int i = 0; i < cmds.size(); i++)
      {
        QString tmp = cmds[i]->getText();
        ret += tmp;
        ret += "\n";
      }

      ret += "}\n";
    }
  }
  return ret;
}

QString XWTikzScope::getTips(XWTikzState * state)
{
  options->doCompute(state);
  QString ret;
  switch (graphic->getCurrentScope())
  {
    case XW_TIKZ_S_SCOPE:
      break;

    default:
      if (cur >= 0 && cur < cmds.size())
        ret = cmds[cur]->getTips(state);
      break;
  }
  return ret;
}

void XWTikzScope::goToEnd()
{
  if (cmds.size() <= 0)
    return ;

  cur = cmds.size() - 1;
  cmds[cur]->goToEnd();
}

bool XWTikzScope::goToNext()
{
  if (cur >= cmds.size() - 1)
    return false;

  cur++;
  cmds[cur]->goToStart();
  return true;
}

bool XWTikzScope::goToNextOperation()
{
  if (cur < 0 || cur >= cmds.size())
    return false;

  switch (cmds[cur]->getKeyWord())
  {
    default:
      break;

    case PGFpath:
    case PGFdraw:
    case PGFfill:
    case PGFfilldraw:
    case PGFpattern:
    case PGFshade:
    case PGFshadedraw:
    case PGFclip:
      if (!cmds[cur]->goToNext())
        return goToNext();
      break;

    case PGFscope:
    case XW_TIKZ_GROUP:
      {
        XWTikzScope * sc = (XWTikzScope*)(cmds[cur]);
        if (!sc->goToNextOperation())
          return goToNext();
      }
      break;
  }

  return true;
}

bool XWTikzScope::goToNextPath()
{
  if (cur >= (cmds.size() - 1))
    return false;

  cur++;
  cmds[cur]->goToStart();
  return true;
}

bool XWTikzScope::goToNextScope()
{
  int i = cur;
  while (i < (cmds.size() - 1))
  {
    i++;
    if (cmds[i]->getKeyWord() == PGFscope || 
        cmds[i]->getKeyWord() == XW_TIKZ_GROUP)
    {
      cur = i;
      return true;
    }  
  }

  return false;
}

void XWTikzScope::goToOperationEnd()
{
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->goToOperationEnd();
}

void XWTikzScope::goToOperationStart()
{
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->goToOperationStart();
}

void XWTikzScope::goToPathEnd()
{
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->goToPathEnd();
}

void XWTikzScope::goToPathStart()
{
  if (cur < 0 || cur >= cmds.size())
    return ;

  cmds[cur]->goToPathStart();
}

bool XWTikzScope::goToPrevious()
{
  if (cur <= 0)
    return false;

  cur--;
  cmds[cur]->goToEnd();
  return true;
}

bool XWTikzScope::goToPreviousOperation()
{
  if (cur < 0 || cur >= cmds.size())
    return false;

  switch (cmds[cur]->getKeyWord())
  {
    default:
      if (!cmds[cur]->goToPrevious())
        return goToPrevious();
      break;

    case PGFscope:
    case XW_TIKZ_GROUP:
      {
        XWTikzScope * sc = (XWTikzScope*)(cmds[cur]);
        if (!sc->goToPreviousOperation())
          return goToPrevious();
      }
      break;

    case PGFtikzset:
    case PGFtikzstyle:
      break;
  }

  return true;
}

bool XWTikzScope::goToPreviousPath()
{
  if (cur <= 0)
    return false;

  cur--;
  cmds[cur]->goToEnd();
  return true;
}

bool XWTikzScope::goToPreviousScope()
{
  int i = cur;
  while (i > 0)
  {
    i--;
    if (cmds[i]->getKeyWord() == PGFscope || 
       cmds[i]->getKeyWord() == XW_TIKZ_GROUP)
    {
      cmds[i]->goToEnd();
      cur = i;
      return true;
    }      
  }

  return false;
}

void XWTikzScope::goToStart()
{
  if (cmds.size() == 0)
    return ;

  cur = 0;
  cmds[cur]->goToStart();
}

bool XWTikzScope::hitTest(XWTikzState * state)
{
  options->doCompute(state);
  bool ret = false;
  switch (graphic->getCurrentScope())
  {
    case XW_TIKZ_S_OPERATION:
    case XW_TIKZ_S_PATH:
      if (cur >= 0 && cur < cmds.size())
        ret = cmds[cur]->hitTest(state);
      break;

    default:
      cur = -1;
      for (int i = 0; i < cmds.size(); i++)
      {
        ret = cmds[i]->hitTest(state);
        if (ret)
        {
          cur = i;
          break;
        }
      }
      break;
  }
  return ret;
}

void XWTikzScope::insert(int i, XWTikzCommand * cmd)
{
  cur = i;
  if (i >= cmds.size())
    cmds << cmd;
  else
    cmds.insert(i,cmd);
}

void XWTikzScope::insertSrc(const QString & src)
{
  if (src.length() <= 0)
    return ;

  if ((cur >= 0) && (cur <= (cmds.size() - 1)) && 
      ((cmds[cur]->getKeyWord() == PGFscope) ||
       (cmds[cur]->getKeyWord() == XW_TIKZ_GROUP)))
  {
    XWTikzScope * s = (XWTikzScope*)(cmds[cur]);
    s->insertSrc(src);
    return ;
  }

  QUndoCommand *cmd = new QUndoCommand;
  int len = src.length();
  int pos = 0;
  XWTikzCommand * obj = 0;
  while (pos < len)
  {
    if (src[pos] == QChar('\\'))
    {
      QString key = XWTeXBox::scanControlSequence(src,len,pos);
      int id = lookupPGFID(key);
      if (id == PGFbegin)
      {
        key = XWTeXBox::scanEnviromentName(src,len,pos);
        id = lookupPGFID(key);
      }
      obj = createPGFObject(graphic,this,id,this);
      obj->scan(src,len,pos);
      XWTikzAddScopePath * c = new XWTikzAddScopePath(this,cur + 1,obj,cmd);
    }
    else  if (src[pos] == QChar('{'))
    {
      obj = new XWTikzScope(graphic,this,XW_TIKZ_GROUP,this);
      obj->scan(src,len,pos);
      XWTikzAddScopePath * c = new XWTikzAddScopePath(this,cur + 1,obj,cmd);
    }
    else if (src[pos] == QChar('%'))
      XWTeXBox::skipComment(src,len,pos);
    else
      pos++;
  }

  graphic->push(cmd);
}

bool XWTikzScope::insertText(XWTikzState * state)
{
  if (cur < 0 || cur >= cmds.size())
    return false;

  options->doCompute(state);
  return cmds[cur]->insertText(state);
}

bool XWTikzScope::isMe(const QString & nameA,XWTikzState * state)
{
  if (options->isMe(nameA,state))
    return true;

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

bool XWTikzScope::keyInput(XWTikzState * state)
{
  if (cur < 0 || cur >= cmds.size())
    return false;

  options->doCompute(state);
  return cmds[cur]->keyInput(state);
}

bool XWTikzScope::newPar(XWTikzState * state)
{
  if (cur < 0 || cur >= cmds.size())
    return false;

  options->doCompute(state);
  return cmds[cur]->newPar(state);
}

bool XWTikzScope::paste(XWTikzState * state)
{
  if (cur < 0 || cur >= cmds.size())
    return false;

  options->doCompute(state);
  return cmds[cur]->paste(state);
}

void XWTikzScope::registNamed(const QString & n)
{
  QString nameA = n;
  if (n.contains("."))
  {
    int index = n.indexOf(".");
    nameA = n.left(index);
  }

  XWTikzState state(graphic,0,false);
  options->doPath(&state,false);
  QString prefix = state.getNamePrefix();
  if (!prefix.isEmpty())
  {
    prefix += "-";
    nameA.insert(0,prefix);
  }

  QString suffix = state.getNameSuffix();
  if (!suffix.isEmpty())
  {
    suffix.insert(0,"-");
    nameA += suffix;
  }

  names[nameA] = cur;
  if (cmds[cur]->getKeyWord() == PGFscope || 
      cmds[cur]->getKeyWord() == XW_TIKZ_GROUP)
  {
    XWTikzScope * s = (XWTikzScope*)(cmds[cur]);
    s->registNamed(n);
  }
}

void XWTikzScope::scan(const QString & str, int & len, int & pos)
{
  cur = -1;
  options->scan(str,len,pos);
  if (keyWord == PGFscope)
  {
    while (pos < len)
    {
      if (str[pos].isSpace())
        pos++;
      else if (str[pos] == QChar('%'))
        XWTeXBox::skipComment(str,len,pos);
      else
      {
        if (str[pos] == QChar('\\'))
        {
          QString key = XWTeXBox::scanControlSequence(str,len,pos);
          int id = lookupPGFID(key);
          if (id == PGFend)
          {
            key = XWTeXBox::scanEnviromentName(str,len,pos);
            return ;
          }
          
          if (id == PGFbegin)
          {
            key = XWTeXBox::scanEnviromentName(str,len,pos);
            id = lookupPGFID(key);
          }
          XWTikzCommand * cmd = createPGFObject(graphic,this,id,this);
          cmds << cmd;
          cur++;
          cmd->scan(str,len,pos);
        }
        else  if (str[pos] == QChar('{'))
        {
          XWTikzCommand * cmd = new XWTikzScope(graphic,this,XW_TIKZ_GROUP,this);
          cmds << cmd;
          cur++;
          cmd->scan(str,len,pos);
        }
        else
          pos++;
      }      
    }
  }
  else
  {
    pos++;
    while (pos < len)
    {
      if (str[pos] == QChar('}'))
      {
        pos++;
        return ;
      }
      else if (str[pos] == QChar('%'))
        XWTeXBox::skipComment(str,len,pos);
      else if (str[pos].isSpace())
        pos++;
      else
      {
        if (str[pos] == QChar('\\'))
        {
          QString key = XWTeXBox::scanControlSequence(str,len,pos);
          int id = lookupPGFID(key);
          if (id == PGFbegin)
          {
            key = XWTeXBox::scanEnviromentName(str,len,pos);
            id = lookupPGFID(key);
          }
          XWTikzCommand * cmd = createPGFObject(graphic,this,id,this);
          cmds << cmd;
          cur++;
          cmd->scan(str,len,pos);
        }
        else
          pos++;
      }      
    }
  }
}

void XWTikzScope::setSortBy(const QString & nameA)
{
  if (names.contains(nameA))
  {
    sortBy = names[nameA];
    if (cmds[sortBy]->getKeyWord() == PGFscope || 
      cmds[sortBy]->getKeyWord() == XW_TIKZ_GROUP)
    {
      XWTikzScope * s = (XWTikzScope*)(cmds[sortBy]);
      s->setSortBy(nameA);
    }
  }
  else
    sortBy = -1;
}

XWTikzCommand * XWTikzScope::takeAt(int i)
{
  cur = i - 1;
  return cmds.takeAt(i);
}

XWTikzForeachCmd::XWTikzForeachCmd(XWTikzGraphic * graphicA,XWTikzScope *scopeA,QObject * parent)
:XWTikzCommand(graphicA,scopeA,PGFforeach,parent),
 body(0)
{
  body = new XWTikzForeach(graphicA,scopeA);
}

bool XWTikzForeachCmd::back(XWTikzState * state)
{
  return body->back(state);
}

bool XWTikzForeachCmd::cut(XWTikzState * state)
{
  return body->cut(state);
}

bool XWTikzForeachCmd::del(XWTikzState * state)
{
  return body->del(state);
}

void XWTikzForeachCmd::doCopy(XWTikzState * state)
{
  body->doCopy(state);
}

void XWTikzForeachCmd::doOperation(XWTikzState * state, bool showpoint)
{
  body->doOperation(state,showpoint);
}

void XWTikzForeachCmd::doPath(XWTikzState * state, bool showpoint)
{
  body->doPath(state,showpoint);
}

void XWTikzForeachCmd::doPathNoOptions(XWTikzState * state)
{
  body->doPathNoOptions(state);
}

bool XWTikzForeachCmd::dropTo(XWTikzState * state)
{
  return body->dropTo(state);
}

int XWTikzForeachCmd::getAnchorPosition()
{
  return body->getAnchorPosition();
}

XWTikzOperation * XWTikzForeachCmd::getCurrentOperation()
{
  return body->getCurrentOperation();
}

XWTikzCoord * XWTikzForeachCmd::getCurrentPoint()
{
  return body->getCurrentPoint();
}

int XWTikzForeachCmd::getCursorPosition()
{
  return body->getCursorPosition();
}

QString XWTikzForeachCmd::getCurrentText()
{
  return body->getCurrentText();
}

QString XWTikzForeachCmd::getSelectedText()
{
  return body->getSelectedText();
}

QString XWTikzForeachCmd::getText()
{
  QString ret = body->getText();
  ret.insert(0,QChar('\\'));
  return ret;
}

QString XWTikzForeachCmd::getTips(XWTikzState * state)
{
  return body->getTips(state);
}

void XWTikzForeachCmd::goToEnd()
{
  body->goToEnd();
}

bool XWTikzForeachCmd::goToNext()
{
  return body->goToNext();
}

bool XWTikzForeachCmd::goToNextOperation()
{
  return body->goToNextOperation();
}

bool XWTikzForeachCmd::goToNextPath()
{
  return body->goToNextPath();
}

void XWTikzForeachCmd::goToOperationEnd()
{
  body->goToOperationEnd();
}

void XWTikzForeachCmd::goToOperationStart()
{
  body->goToOperationStart();
}

void XWTikzForeachCmd::goToPathEnd()
{
  body->goToPathEnd();
}

void XWTikzForeachCmd::goToPathStart()
{
  body->goToPathStart();
}

bool XWTikzForeachCmd::goToPrevious()
{
  return body->goToPrevious();
}

bool XWTikzForeachCmd::goToPreviousOperation()
{
  return body->goToPreviousOperation();
}

bool XWTikzForeachCmd::goToPreviousPath()
{
  return body->goToPreviousPath();
}

void XWTikzForeachCmd::goToStart()
{
  body->goToStart();
}

bool XWTikzForeachCmd::hitTest(XWTikzState * state)
{
  return body->hitTest(state);
}

bool XWTikzForeachCmd::insertText(XWTikzState * state)
{
  return body->insertText(state);
}

bool XWTikzForeachCmd::keyInput(XWTikzState * state)
{
  return body->keyInput(state);
}

bool XWTikzForeachCmd::newPar(XWTikzState * state)
{
  return body->newPar(state);
}

bool XWTikzForeachCmd::paste(XWTikzState * state)
{
  return body->paste(state);
}

void XWTikzForeachCmd::scan(const QString & str, int & len, int & pos)
{
  body->scan(str,len,pos);
}

XWTikzCoordinatePath::XWTikzCoordinatePath(XWTikzGraphic * graphicA,XWTikzScope *scopeA,QObject * parent)
:XWTikzCommand(graphicA,scopeA,PGFcoordinate,parent),
node(0),
coord(0)
{
  node = new XWTikzCoordinate(graphicA,this);
  coord = new XWTikzCoord(graphicA,this);
}

XWTikzCoordinatePath::XWTikzCoordinatePath(XWTikzGraphic * graphicA,XWTikzScope *scopeA,int idA,QObject * parent)
:XWTikzCommand(graphicA,scopeA,idA,parent),
node(0),
coord(0)
{}

void XWTikzCoordinatePath::addPathAction(QMenu & menu, XWTikzState * state)
{
  options->doCompute(state);
  QAction * a = menu.addAction(tr("add child"));
  connect(a, SIGNAL(triggered()), node, SLOT(addChild()));
}

bool XWTikzCoordinatePath::back(XWTikzState * state)
{
  options->doCompute(state);
  return node->back(state);
}

bool XWTikzCoordinatePath::cut(XWTikzState * state)
{
  options->doCompute(state);
  return node->cut(state);
}

bool XWTikzCoordinatePath::del(XWTikzState * state)
{
  options->doCompute(state);
  return node->del(state);
}

void XWTikzCoordinatePath::doOperation(XWTikzState * state, bool showpoint)
{
  state = state->save();

  options->doPath(state,showpoint);
  coord->doPath(state,false);
  node->doPath(state,showpoint);

  state = state->restore();
}

void XWTikzCoordinatePath::doPath(XWTikzState * state, bool showpoint)
{
  if (options->hasPre())
  {
    state = state->save();

    options->doPre(state);
    coord->doPath(state,false);
    node->doPath(state,false);

    state = state->restore();
  }

  state = state->save();

  options->doPath(state,showpoint);
  if (graphic->getCurrentScope() == XW_TIKZ_S_PATH)
    coord->doPath(state,true);
  else
    coord->doPath(state,false);
  node->doPath(state,showpoint);
  
  state = state->restore();
}

void XWTikzCoordinatePath::doPathNoOptions(XWTikzState * state)
{
  state = state->save();
  coord->doPath(state,false);
  node->doPath(state,false);
  state = state->restore();
}

void XWTikzCoordinatePath::dragTo(XWTikzState * state)
{
  if (coord != graphic->getCurrentPoint())
    return ;

  options->doCompute(state);
  if (graphic->getCurrentScope() == XW_TIKZ_S_OPERATION)
    node->dragTo(state);
  else
  {
    if (graphic->getCurrentPoint() == coord)
      coord->dragTo(state);
    else
      node->dragTo(state);
  }
}

bool XWTikzCoordinatePath::dropTo(XWTikzState * state)
{
  options->doCompute(state);
  bool ret = false;
  if (graphic->getCurrentScope() == XW_TIKZ_S_OPERATION)
    ret = node->dropTo(state);
  else
  {
    if (graphic->getCurrentPoint() == coord)
      ret = coord->dropTo(state);
    else
      ret = node->dropTo(state);
  }
  return ret;
}

QPointF XWTikzCoordinatePath::getAnchor(const QString & nameA,int a, XWTikzState * state)
{
  options->doCompute(state);
  coord->doCompute(state);
  QPointF ret;
  if (node->isMe(nameA,state))
    ret = node->getAnchor(a,state);
  return ret;
}

int XWTikzCoordinatePath::getAnchorPosition()
{
  return node->getAnchorPosition();
}

QPointF XWTikzCoordinatePath::getAngle(const QString & nameA,double a, XWTikzState * state)
{
  options->doCompute(state);
  coord->doCompute(state);
  QPointF ret;
  if (node->isMe(nameA,state))
    ret = node->getAngle(a,state);

  return ret;
}

QPointF XWTikzCoordinatePath::getBorder(const QString & nameA,const QPointF & p,XWTikzState * state)
{
  options->doCompute(state);
  coord->doCompute(state);
  QPointF ret;
  if (node->isMe(nameA,state))
    ret = node->getBorder(p,state);

  return ret;
}

QPointF XWTikzCoordinatePath::getCenter(XWTikzState * state)
{
  options->doCompute(state);
  coord->doCompute(state);
  node->doCompute(state);
  return state->getCenter();
}

XWTikzOperation * XWTikzCoordinatePath::getCurrentOperation()
{
  XWTikzOperation * op = coord->getCurrentPoint();
  if (op)
    return op;
  return node;
}

XWTikzCoord * XWTikzCoordinatePath::getCurrentPoint()
{
  return coord->getCurrentPoint();
}

QString XWTikzCoordinatePath::getCurrentText()
{
  return node->getCurrentText();
}

int XWTikzCoordinatePath::getCursorPosition()
{
  return node->getCursorPosition();
}

QPointF XWTikzCoordinatePath::getPoint(XWTikzState * stateA)
{
  return coord->getPoint(stateA);
}

QPointF XWTikzCoordinatePath::getPoint(const QString & nameA, XWTikzState * state)
{
  options->doCompute(state);
  coord->doCompute(state);
  QPointF ret;
  if (node->isMe(nameA,state))
  {
    if (nameA.contains("."))
      ret = node->getAnchor(state);
    else
      ret = coord->getPoint(state);
  }

  return ret;
}

QVector3D XWTikzCoordinatePath::getPoint3D(const QString & nameA, XWTikzState * state)
{
  options->doCompute(state);
  coord->doCompute(state);
  QVector3D ret;
  if (node->isMe(nameA,state))
  {
    if (nameA.contains("."))
    {
      QPointF p = node->getAnchor(state);
      ret.setX(p.x());
      ret.setY(p.y());
    }
    else
      ret = coord->getPoint3D(state);
  }
  
  return ret;
}

double XWTikzCoordinatePath::getRadius(const QString & nameA,XWTikzState * state)
{
  options->doCompute(state);
  coord->doCompute(state);
  double ret;
  if (node->isMe(nameA,state))
    ret = node->getRadius(state);

  return ret;
}

QString XWTikzCoordinatePath::getSelectedText()
{
  return node->getSelectedText();
}

QString XWTikzCoordinatePath::getText()
{
  QString ret = "\\";
  QString tmp = node->getTextForPath();
  tmp = tmp.simplified();
  ret += tmp;
  ret += " at ";
  tmp = coord->getText();
  ret += tmp;
  ret += ";";
  return ret;
}

QString XWTikzCoordinatePath::getTips(XWTikzState * state)
{
  QString ret = node->getTips(state);
  ret += " at ";

  QString tmp = coord->getTips(state);
  ret += tmp;

  return ret;
}

void XWTikzCoordinatePath::getWidthAndHeight(double & w, double & h)
{
  XWTikzState state;
  options->doCompute(&state);
  coord->doCompute(&state);
  if (node)
    node->doCompute(&state);
  w = state.getWidth();
  h = state.getHeight();
}

bool XWTikzCoordinatePath::goToNext()
{
  return node->goToNext();
}

void XWTikzCoordinatePath::goToNextPoint()
{
  node->goToNext();
}

void XWTikzCoordinatePath::goToOperationEnd()
{
  node->goToOperationEnd();
}

void XWTikzCoordinatePath::goToOperationStart()
{
  node->goToOperationStart();
}

void XWTikzCoordinatePath::goToPathEnd()
{
  node->goToPathEnd();
}

void XWTikzCoordinatePath::goToPathStart()
{
  node->goToPathStart();
}

bool XWTikzCoordinatePath::goToPrevious()
{
  return node->goToPrevious();
}

void XWTikzCoordinatePath::goToPreviousPoint()
{
  node->goToPrevious();
}

bool XWTikzCoordinatePath::hitTest(XWTikzState * state)
{
  options->doCompute(state);
  bool ret = false;
  if (graphic->getCurrentScope() == XW_TIKZ_S_OPERATION)
    ret = node->hitTest(state);
  else
  {
    ret = coord->hitTest(state);
    if (!ret)
      ret = node->hitTest(state);
  }
  return ret;
}

bool XWTikzCoordinatePath::insertText(XWTikzState * state)
{
  options->doCompute(state);
  return node->insertText(state);
}

bool XWTikzCoordinatePath::isMe(const QString & nameA,XWTikzState * state)
{
  return node->isMe(nameA,state);
}

bool XWTikzCoordinatePath::keyInput(XWTikzState * state)
{
  options->doCompute(state);
  return node->keyInput(state);
}

bool XWTikzCoordinatePath::newPar(XWTikzState * state)
{
  options->doCompute(state);
  return node->newPar(state);
}

bool XWTikzCoordinatePath::paste(XWTikzState * state)
{
  options->doCompute(state);
  return node->paste(state);
}

void XWTikzCoordinatePath::scan(const QString & str, int & len, int & pos)
{
  node->scan(str,len,pos);
  while (str[pos].isSpace())
    pos++;

  if (str[pos] == QChar('a'))
    pos += 2;

  coord->scan(str,len,pos);
  while (str[pos].isSpace())
    pos++;

  pos++;
}

void XWTikzCoordinatePath::setAt(const QString & str)
{
  coord->setText(str);
}

void XWTikzCoordinatePath::setName(const QString & str)
{
  node->setName(str);
}

XWTikzNodePath::XWTikzNodePath(XWTikzGraphic * graphicA,XWTikzScope *scopeA,QObject * parent)
:XWTikzCoordinatePath(graphicA,scopeA,PGFnode,parent)
{
  node = new XWTikzNode(graphicA,this);
  coord = new XWTikzCoord(graphicA,this);
}

bool XWTikzNodePath::addOperationAction(QMenu & menu, XWTikzState * state)
{
  options->doCompute(state);
  node->addAction(menu, state);
  options->addShadowAction(menu);
  return true;
}

QString XWTikzNodePath::getText()
{
  QString ret = "\\";
  QString tmp = node->getTextForPath();
  tmp = tmp.simplified();
  ret += tmp;
  ret += " at ";
  tmp = coord->getText();
  ret += tmp;
  XWTikzNode * n = (XWTikzNode*)(node);
  tmp = n->getContent();
  ret += tmp;
  ret += ";";

  return ret;
}

QString XWTikzNodePath::getTips(XWTikzState * state)
{
  options->doCompute(state);
  QString ret = coord->getTips(state);
  if (ret.isEmpty())
    ret = node->getTips(state);
  return ret;
}

void XWTikzNodePath::scan(const QString & str, int & len, int & pos)
{
  node->scan(str,len,pos);
  if (str[pos] == QChar('a'))
    pos += 2;

  coord->scan(str,len,pos);
  node->scanText(str,len,pos);

  while (str[pos].isSpace())
    pos++;

  pos++;
}

void XWTikzNodePath::setText(const QString & str)
{
  node->setText(str);
}

XWTikzMatrixCommand::XWTikzMatrixCommand(XWTikzGraphic * graphicA,XWTikzScope *scopeA,QObject * parent)
:XWTikzCommand(graphicA,scopeA,PGFmatrix,parent),
matrix(0)
{
  matrix = new XWTikzMatrix(graphicA,this);
}

bool XWTikzMatrixCommand::back(XWTikzState * state)
{
  options->doCompute(state);
  return matrix->back();
}

bool XWTikzMatrixCommand::cut(XWTikzState * state)
{
  options->doCompute(state);
  return matrix->cut();
}

bool XWTikzMatrixCommand::del(XWTikzState * state)
{
  options->doCompute(state);
  return matrix->del();
}

void XWTikzMatrixCommand::doPath(XWTikzState * state, bool showpoint)
{
  matrix->doPath(state,showpoint);
}

void XWTikzMatrixCommand::dragTo(XWTikzState * state)
{
  options->doCompute(state);
  matrix->dragTo(state);
}

bool XWTikzMatrixCommand::dropTo(XWTikzState * state)
{
  options->doCompute(state);
  return matrix->dropTo(state);
}

int XWTikzMatrixCommand::getAnchorPosition()
{
  return matrix->getAnchorPosition();
}

int XWTikzMatrixCommand::getCursorPosition()
{
  return matrix->getCursorPosition();
}

QString XWTikzMatrixCommand::getCurrentText()
{
  return matrix->getCurrentText();
}

QString XWTikzMatrixCommand::getSelectedText()
{
  return matrix->getCurrentText();
}

QString XWTikzMatrixCommand::getText()
{
  QString ret = "\\matrix";
  QString tmp = options->getText();
  ret += tmp;
  tmp = matrix->getText();
  ret += tmp;
  ret += ";";
  return ret;
}

void XWTikzMatrixCommand::goToEnd()
{
  matrix->goToEnd();
}

bool XWTikzMatrixCommand::goToNext()
{
  return matrix->goToNext();
}

bool XWTikzMatrixCommand::goToPrevious()
{
  return matrix->goToPrevious();
}

void XWTikzMatrixCommand::goToStart()
{
  matrix->goToStart();
}

bool XWTikzMatrixCommand::hitTest(XWTikzState * state)
{
  options->doCompute(state);
  QPointF mp = state->getMousePoint();
  return matrix->hitTest(mp.x(),mp.y());
}

bool XWTikzMatrixCommand::insertText(XWTikzState * state)
{
  options->doCompute(state);
  QString str = state->getText();
  return matrix->insertText(str);
}

bool XWTikzMatrixCommand::keyInput(XWTikzState * state)
{
  options->doCompute(state);
  QString str = state->getText();
  return matrix->keyInput(str);
}

bool XWTikzMatrixCommand::newPar(XWTikzState * state)
{
  options->doCompute(state);
  return matrix->newPar();
}

bool XWTikzMatrixCommand::paste(XWTikzState * state)
{
  options->doCompute(state);
  return matrix->paste();
}

void XWTikzMatrixCommand::scan(const QString & str, int & len, int & pos)
{
  options->scan(str,len,pos);
  while (str[pos].isSpace())
    pos++;
  pos++;
  matrix->scan(str,len,pos);
  while (str[pos].isSpace())
    pos++;

  pos++;
}

XWTikzSpy::XWTikzSpy(XWTikzGraphic * graphicA,XWTikzScope *scopeA,QObject * parent)
:XWTikzCommand(graphicA,scopeA,PGFspy,parent),
 on(0),
 node(0),
 at(0)
{
  on = new XWTikzCoord(graphicA,this);
  node = new XWTikzNode(graphicA,this);
}

void XWTikzSpy::addPathAction(QMenu & menu, XWTikzState * state)
{
  options->doPath(state, false);
  options->addColorAction(menu);
  options->addOpacityAction(menu);
  options->addSizeAction(menu);
  options->addConnectSpiesAction(menu);
}

void XWTikzSpy::doCopy(XWTikzState *)
{
  return ;
}

void XWTikzSpy::doPath(XWTikzState * state, bool showpoint)
{
  XWTikzState * onstate = new XWTikzState(false,state);
  XWTikzState * instate = new XWTikzState(false,state);
  //on
  state = state->save();
  state->resetTransform();
  state->setOnNode(true);
  state->setColor(Qt::black);
  state->setDash(PGFsolid);
  state->setLineWidth(0.4);
  state->setOpacity(1);
  state->setLineCap(PGFbutt);
  state->setLineJoin(PGFmiter);
  state->setInnerXSep(0);
  state->setInnerYSep(0);
  state->setOuterXSep(0);
  state->setOuterYSep(0);
  options->doPath(state, showpoint);  
  graphic->doSpyNode(state);
  state = state->save();
  state->moveTo(on);
  state->copy(onstate);
  state->setClip(true);
  node->doPath(state,showpoint);  
  XWTeXBox * box = node->getBox();
  state = state->restore();
  if (scope)
    scope->doCopy(state); 
  else
    graphic->doCopy(state); 
  state = state->restore();

  //in
  state = state->save();
  state->resetTransform();
  state->setOnNode(false);
  state->setColor(Qt::black);
  state->setDash(PGFsolid);
  state->setLineWidth(0.4);
  state->setOpacity(1);
  state->setLineCap(PGFbutt);
  state->setLineJoin(PGFmiter);
  state->setInnerXSep(0);
  state->setInnerYSep(0);
  state->setOuterXSep(0);
  state->setOuterYSep(0);
  options->doPath(state, showpoint);  
  graphic->doSpyNode(state);
  state = state->save();
  if (at)
    state->moveTo(at);
  else
    state->moveTo(on);
  state->copy(instate);
  state->setAnchor(PGFcenter);
  state->setClip(true);
  node->doPath(state,showpoint);
  state = state->restore();
  if (scope)
    scope->doCopy(state); 
  else
    graphic->doCopy(state);
  state = state->restore();

  if (at)
  {
    state = state->save(); 
    XWPDFDriver*driverA = state->getDriver();
    onstate->transformNode();
    instate->transformNode();
    XWTikzShape * onnode = new XWTikzShape(driverA,box,onstate,XW_TIKZ_NODE);
    XWTikzShape * innode = new XWTikzShape(driverA,box,instate,XW_TIKZ_NODE);
    state->setParentNode(onnode);
    state->setChildNode(innode);
    options->doPath(state, showpoint);
    state->setColor(Qt::black);
    state->setDash(PGFsolid);
    state->setLineWidth(0.4);
    state->setOpacity(1);
    state->setLineCap(PGFbutt);
    state->setLineJoin(PGFmiter);
    graphic->doSpyConnection(state);
    state = state->restore();
  }
}

void XWTikzSpy::dragTo(XWTikzState * state)
{
  if (!curPoint)
    return ;

  curPoint->dragTo(state);
}

bool XWTikzSpy::dropTo(XWTikzState * state)
{
  if (!curPoint)
    return false;

  return curPoint->dropTo(state);
}

QString XWTikzSpy::getText()
{
  QString spy = "\\spy ";
  QString tmp = options->getText();
  spy += tmp;
  spy += " on ";
  tmp = on->getText();
  spy += tmp;
  spy += " in ";
  tmp = node->getText();
  spy += tmp;
  if (at)
  {
    spy += " at ";
    tmp = at->getText();
    spy += tmp;
  }

  spy += ";";
  return spy;
}

QString XWTikzSpy::getTips(XWTikzState * )
{
  QString spy = "spy on " ;
  QString tmp = on->getText();
  spy += tmp;
  if (at)
  {
    spy += " at ";
    tmp = at->getText();
    spy += tmp;
  }

  return spy;
}

bool XWTikzSpy::hitTest(XWTikzState * state)
{
  curPoint = 0;
  bool ret = on->hitTest(state);
  if (ret)
    curPoint = on;
  else
  {
    if (at)
    {
      ret = at->hitTest(state);
      if (ret)
        curPoint = at;
    }
  }

  return ret;
}

void XWTikzSpy::scan(const QString & str, int & len, int & pos)
{
  options->scan(str,len,pos);
  while (str[pos].isSpace())
    pos++;
  pos += 2;
  on->scan(str,len,pos);

  while (str[pos].isSpace())
    pos++;

  pos += 2;

  while (str[pos].isSpace())
    pos++;

  pos += 4;
  node->scan(str,len,pos);

  while (str[pos].isSpace())
    pos++;

  if (str[pos] == QChar('a'))
  {
    pos += 2;
    at = new XWTikzCoord(graphic,this);
    at->scan(str,len,pos);

    while (str[pos].isSpace())
      pos++;
  }

  pos++;
}

void XWTikzSpy::setAt(const QString & str)
{
  if (str.isEmpty())
  {
    if (at)
    {
      delete at;
      at = 0;
    }    
  }
  else
  {
    if (!at)
      at = new XWTikzCoord(graphic,this);
    at->setText(str);
  }
}

void XWTikzSpy::setOn(const QString & str)
{
  on->setText(str);
}

XWTikzArrowMarking::XWTikzArrowMarking(XWTikzGraphic * graphicA,XWTikzScope *scopeA,QObject * parent)
:XWTikzCommand(graphicA,scopeA,PGFarrow,parent),
arrowTip(-1)
{}

void XWTikzArrowMarking::doPath(XWTikzState * state, bool showpoint)
{
  state = state->save();
  options->doPath(state, showpoint);
  XWTikzArrow arrow(arrowTip);
  arrow.setup(state);
  arrow.draw(state);
  state = state->restore();
}

QString XWTikzArrowMarking::getText()
{
  QString ret = "\\arrow";
  QString tmp = options->getText();
  ret += tmp;
  tmp = getPGFString(arrowTip);
  ret += QString(" {%1};").arg(arrowTip);
  return ret;
}

void XWTikzArrowMarking::scan(const QString & str, int & len, int & pos)
{
  options->scan(str, len,pos);
  while (str[pos].isSpace())
    pos++;
  pos++;
  int i = pos;
  while (str[pos] != QChar('}'))
    pos++;
  QString a = str.mid(i,pos - i);
  a = a.simplified();
  pos++;
  while (str[pos].isSpace())
    pos++;
  if (str[pos] == QChar(';'))
    pos++;

  arrowTip = lookupPGFID(a);
}
