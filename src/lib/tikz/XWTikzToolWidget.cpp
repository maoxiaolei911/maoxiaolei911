/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QtGui>
#include "PGFKeyWord.h"
#include "XWPDFDriver.h"
#include "XWTikzState.h"
#include "XWTikzGraphic.h"
#include "pstlaboWidget.h"
#include "XWTikzToolWidget.h"

XWTikzToolBox::XWTikzToolBox(XWTikzGraphic * graphicA,QWidget * parent)
:QToolBox(parent),
 graphic(graphicA)
{
  XWTikzToolsWidget * widget = new XWTikzPathActionWidget(graphicA,this);
  addItem(widget,tr("path action"));

  widget = new XWTikzPathOperationWidget(graphicA,this);
  addItem(widget,tr("path operation"));

  PSTLaboWidget * w = new PSTLaboWidget(graphicA,this);
  addItem(w,tr("chemical"));
}

XWTikzToolsWidget::XWTikzToolsWidget(XWTikzGraphic * graphicA,QWidget * parent)
:QWidget(parent),
 graphic(graphicA)
{}

QToolButton * XWTikzToolsWidget::createButton(const QString & src,const QString & txt)
{
  XWPDFDriver driver(100,100);
  XWTikzGraphic graphicA;
  graphicA.scan(src);
  graphicA.doGraphic(&driver);

  QPixmap pix(100,100);
  QPainter painter(&pix);
  driver.display(&painter);

  QSize iconSize(100,100);
  QToolButton *button = new QToolButton;
  button->setIconSize(iconSize);
  button->setText(txt);
  button->setToolTip(txt);

  button->setIcon(QIcon(pix));
  button->setAutoRaise(true);
  return button;
}

XWTikzPathActionWidget::XWTikzPathActionWidget(XWTikzGraphic * graphicA,QWidget * parent)
:XWTikzToolsWidget(graphicA,parent)
{
  QGridLayout *layout = new QGridLayout;

  QToolButton * button = createPathButton();
  layout->addWidget(button,0,0);
  connect(button, SIGNAL(clicked()), this, SIGNAL(addPath()));
/*
  button = createDrawButton();
  layout->addWidget(button,0,1);
  connect(button, SIGNAL(clicked()), this, SIGNAL(addDraw()));

  button = createFillButton();
  layout->addWidget(button,0,2);
  connect(button, SIGNAL(clicked()), this, SIGNAL(addFill()));

  button = createFillDrawButton();
  layout->addWidget(button,0,3);
  connect(button, SIGNAL(clicked()), this, SIGNAL(addFillDraw()));

  button = createClipButton();
  layout->addWidget(button,1,0);
  connect(button, SIGNAL(clicked()), this, SIGNAL(addClip()));
*/
  button = createNodeButton();
  layout->addWidget(button,0,1);
  connect(button, SIGNAL(clicked()), graphic, SIGNAL(addNodeCommand()));

  button = createCoordinateButton();
  layout->addWidget(button,0,2);
  connect(button, SIGNAL(clicked()), graphic, SIGNAL(addCoordinateCommand()));

  button = createScopeButton();
  layout->addWidget(button,0,3);
  connect(button, SIGNAL(clicked()), graphic, SIGNAL(addScope()));
/*
  button = createShadeButton();
  layout->addWidget(button,2,0);
  connect(button, SIGNAL(clicked()), graphic, SIGNAL(addShade()));

  button = createShadeDrawButton();
  layout->addWidget(button,2,1);
  connect(button, SIGNAL(clicked()), graphic, SIGNAL(addShadeDraw()));

  button = createPatternButton();
  layout->addWidget(button,2,2);
  connect(button, SIGNAL(clicked()), graphic, SIGNAL(addPattern()));
*/
  button = createSpyButton();
  layout->addWidget(button,1,0);
  connect(button, SIGNAL(clicked()), graphic, SIGNAL(addSpy()));

  setLayout(layout);
}

void XWTikzPathActionWidget::addClip()
{
  graphic->addPath(PGFclip);
}

void XWTikzPathActionWidget::addDraw()
{
  graphic->addPath(PGFdraw);
}

void XWTikzPathActionWidget::addFill()
{
  graphic->addPath(PGFfill);
}

void XWTikzPathActionWidget::addFillDraw()
{
  graphic->addPath(PGFfilldraw);
}

void XWTikzPathActionWidget::addPath()
{
  graphic->addPath(PGFpath);
}

void XWTikzPathActionWidget::addPattern()
{
  graphic->addPath(PGFpattern);
}

void XWTikzPathActionWidget::addShade()
{
  graphic->addPath(PGFshade);
}

void XWTikzPathActionWidget::addShadeDraw()
{
  graphic->addPath(PGFshadedraw);
}

void XWTikzPathActionWidget::addSpy()
{
  graphic->addSpy();
}

QToolButton * XWTikzPathActionWidget::createClipButton()
{
#define XW_TIKZ_SRC_CLIP "\
\\begin{tikzpicture}\
\\draw[clip] (1.2,1.76) circle [radius=1cm];\
\\fill[red] (2.2,1.76) circle [radius=1cm];\
\\end{tikzpicture}"

  return createButton(XW_TIKZ_SRC_CLIP,tr("\\clip"));
}

QToolButton * XWTikzPathActionWidget::createCoordinateButton()
{
#define XW_TIKZ_SRC_COORDINATE "\\tikz \\filldraw[fill=black!75,dotted] (1.76,1.76) circle [radius=1.5mm];"

  return createButton(XW_TIKZ_SRC_COORDINATE,tr("\\coordinate"));
}

QToolButton * XWTikzPathActionWidget::createDrawButton()
{
#define XW_TIKZ_SRC_DRAW "\\tikz \\draw (1.76,1.76) circle [radius=1.5cm];"

  return createButton(XW_TIKZ_SRC_DRAW,tr("\\draw"));
}

QToolButton * XWTikzPathActionWidget::createFillButton()
{
#define XW_TIKZ_SRC_FILL "\\tikz \\fill[color=red!20] (1.76,1.76) circle [radius=1.5cm];"

  return createButton(XW_TIKZ_SRC_FILL,tr("\\fill"));
}

QToolButton * XWTikzPathActionWidget::createFillDrawButton()
{
#define XW_TIKZ_SRC_FILLDRAW "\\tikz \\filldraw[draw=black,fill=red!20] (1.76,1.76) circle [radius=1.5cm];"
  
  return createButton(XW_TIKZ_SRC_FILLDRAW,tr("\\filldraw"));
}

QToolButton * XWTikzPathActionWidget::createNodeButton()
{
#define XW_TIKZ_SRC_NODE "\\tikz \\draw (1.76,1.7) node[double,draw=black,fill=red!20] {\\Large{node}};"

  return createButton(XW_TIKZ_SRC_NODE,tr("\\node"));
}

QToolButton * XWTikzPathActionWidget::createPathButton()
{
#define XW_TIKZ_SRC_PATH "\\tikz \\path[draw=black!50,dotted] (1.76,1.76) circle [radius=1.5cm];"

  return createButton(XW_TIKZ_SRC_PATH,tr("\\path"));
}

QToolButton * XWTikzPathActionWidget::createPatternButton()
{
#define XW_TIKZ_SRC_PATTERN "\\tikz \\pattern[pattern=fivepointed stars] (1.76,1.76) circle [radius=1.5cm];"

  return createButton(XW_TIKZ_SRC_PATTERN,tr("\\pattern"));
}

QToolButton * XWTikzPathActionWidget::createScopeButton()
{
#define XW_TIKZ_SRC_SCOPE "\\tikz \\draw (1.76,1.7) node {scope};"

  return createButton(XW_TIKZ_SRC_SCOPE,tr("\\scope"));
}

QToolButton * XWTikzPathActionWidget::createShadeButton()
{
#define XW_TIKZ_SRC_SHADE "\\tikz \\shade[shading=ball] (1.76,1.76) circle [radius=1.5cm];"

  return createButton(XW_TIKZ_SRC_SHADE,tr("\\shade"));
}

QToolButton * XWTikzPathActionWidget::createShadeDrawButton()
{
#define XW_TIKZ_SRC_SHADEDRAW "\\tikz \\shadedraw[shading=ball] (1.76,1.76) circle [radius=1.5cm];"

  return createButton(XW_TIKZ_SRC_SHADEDRAW,tr("\\shadedraw"));
}

QToolButton * XWTikzPathActionWidget::createSpyButton()
{
#define XW_TIKZ_SRC_SPY "\\tikz \\draw (1.76,1.7) node[shape=magnifying glass,draw=black] {\\Large{node}};"

  return createButton(XW_TIKZ_SRC_SPY,tr("\\spy"));
}

XWTikzPathOperationWidget::XWTikzPathOperationWidget(XWTikzGraphic * graphicA,QWidget * parent)
:XWTikzToolsWidget(graphicA,parent)
{
  QGridLayout *layout = new QGridLayout;

  QToolButton * button = createMoveToButton();
  layout->addWidget(button,0,0);
  connect(button, SIGNAL(clicked()), graphic, SIGNAL(addMoveTo()));

  button = createLineToButton();
  layout->addWidget(button,0,1);
  connect(button, SIGNAL(clicked()), graphic, SIGNAL(addLineTo()));

  button = createRectangleButton();
  layout->addWidget(button,0,2);
  connect(button, SIGNAL(clicked()), graphic, SIGNAL(addRectangle()));

  button = createCircleButton();
  layout->addWidget(button,0,3);
  connect(button, SIGNAL(clicked()), graphic, SIGNAL(addCircle()));

  button = createEllipseButton();
  layout->addWidget(button,1,0);
  connect(button, SIGNAL(clicked()), graphic, SIGNAL(addEllipse()));

  button = createParabolaButton();
  layout->addWidget(button,1,1);
  connect(button, SIGNAL(clicked()), graphic, SIGNAL(addParabola()));

  button = createSineButton();
  layout->addWidget(button,1,2);
  connect(button, SIGNAL(clicked()), graphic, SIGNAL(addSine()));

  button = createCosineButton();
  layout->addWidget(button,1,3);
  connect(button, SIGNAL(clicked()), graphic, SIGNAL(addCosine()));

  button = createCycleButton();
  layout->addWidget(button,2,0);
  connect(button, SIGNAL(clicked()), graphic, SIGNAL(addCycle()));

  button = createHVLinesButton();
  layout->addWidget(button,2,1);
  connect(button, SIGNAL(clicked()), graphic, SIGNAL(addHVLines()));

  button = createVHLinesButton();
  layout->addWidget(button,2,2);
  connect(button, SIGNAL(clicked()), graphic, SIGNAL(addVHLines()));

  button = createGridButton();
  layout->addWidget(button,2,3);
  connect(button, SIGNAL(clicked()), graphic, SIGNAL(addGrid()));

  button = createNodeButton();
  layout->addWidget(button,3,0);
  connect(button, SIGNAL(clicked()), graphic, SIGNAL(addNode()));

  button = createCircleSplitButton();
  layout->addWidget(button,3,1);
  connect(button, SIGNAL(clicked()), graphic, SIGNAL(addCircleSplit()));

  button = createCircleSolidusButton();
  layout->addWidget(button,3,2);
  connect(button, SIGNAL(clicked()), graphic, SIGNAL(addCircleSolidus()));

  button = createEllipseSplitButton();
  layout->addWidget(button,3,3);
  connect(button, SIGNAL(clicked()), graphic, SIGNAL(addEllipseSplit()));

  button = createRectangleSplitButton();
  layout->addWidget(button,4,0);
  connect(button, SIGNAL(clicked()), graphic, SIGNAL(addRectangleSplit()));

  button = createCoordinateButton();
  layout->addWidget(button,4,1);
  connect(button, SIGNAL(clicked()), graphic, SIGNAL(addCoordinate()));

  button = createPlotFunctionButton();
  layout->addWidget(button,4,2);
  connect(button, SIGNAL(clicked()), graphic, SIGNAL(addPlotFunction()));

  button = createArcButton();
  layout->addWidget(button,4,3);
  connect(button, SIGNAL(clicked()), graphic, SIGNAL(addArc()));

  button = createCurveToButton();
  layout->addWidget(button,5,0);
  connect(button, SIGNAL(clicked()), graphic, SIGNAL(addCurveTo()));

  button = createPlotCoordinatesButton();
  layout->addWidget(button,5,1);
  connect(button, SIGNAL(clicked()), graphic, SIGNAL(addPlotCoordinates()));

  button = createPlotFileButton();
  layout->addWidget(button,5,2);
  connect(button, SIGNAL(clicked()), graphic, SIGNAL(addPlotFile()));

  button = createPlotFunctionButton();
  layout->addWidget(button,5,3);
  connect(button, SIGNAL(clicked()), graphic, SIGNAL(addPlotFunction()));

  button = createEdgeButton();
  layout->addWidget(button,6,0);
  connect(button, SIGNAL(clicked()), graphic, SIGNAL(addEdge()));

  setLayout(layout);
}

QToolButton * XWTikzPathOperationWidget::createArcButton()
{
#define XW_TIKZ_SRC_ARC "\\tikz[radius=1cm] \\draw (1.7,1) arc[start angle=0,end angle=270];"

  return createButton(XW_TIKZ_SRC_ARC,tr("arc"));
}

QToolButton * XWTikzPathOperationWidget::createCircleButton()
{
#define XW_TIKZ_SRC_CIRCLE "\\tikz \\draw (1.76,1.76) circle [radius=1.5cm];"

  return createButton(XW_TIKZ_SRC_CIRCLE,tr("circle"));
}

QToolButton * XWTikzPathOperationWidget::createCircleSolidusButton()
{
#define XW_TIKZ_SRC_CIRCLESOLIDUS "\\tikz \\draw (1.5,1.5) node[circle solidus,double,draw=black,fill=red!20] {text\\nodepart{text,lower}\ntext};"

  return createButton(XW_TIKZ_SRC_CIRCLESOLIDUS,tr("circle solidus"));
}

QToolButton * XWTikzPathOperationWidget::createCircleSplitButton()
{
#define XW_TIKZ_SRC_CIRCLESPLIT "\\tikz \\draw (1.5,1.5) node[circle split,double,draw=black,fill=red!20] {text\\nodepart{text,lower}\ntext};"

  return createButton(XW_TIKZ_SRC_CIRCLESPLIT,tr("circle split"));
}

QToolButton * XWTikzPathOperationWidget::createCoordinateButton()
{
#define XW_TIKZ_SRC_COORD "\\tikz \\filldraw[fill=black!75,dotted] (1.76,1.76) circle [radius=1.5mm];"

  return createButton(XW_TIKZ_SRC_COORD,tr("coordinate"));
}

QToolButton * XWTikzPathOperationWidget::createCosineButton()
{
#define XW_TIKZ_SRC_COS "\\tikz \\draw (0.5,0.5) cos (3,3);"

  return createButton(XW_TIKZ_SRC_COS,tr("cos"));
}

QToolButton * XWTikzPathOperationWidget::createCurveToButton()
{
#define XW_TIKZ_SRC_CURVE "\\tikz \\draw (0.5,0.5) .. controls (1,1) and (2,2) .. (3,0);"

  return createButton(XW_TIKZ_SRC_CURVE,tr("curve to"));
}

QToolButton * XWTikzPathOperationWidget::createCycleButton()
{
#define XW_TIKZ_SRC_CYCLE "\\tikz \\draw (0.5,0.5) -- (3,0.5) .. (3,3) cycle;"

  return createButton(XW_TIKZ_SRC_CYCLE,tr("cycle"));
}

QToolButton * XWTikzPathOperationWidget::createEdgeButton()
{
#define XW_TIKZ_SRC_EDGE "\
\\begin{tikzpicture} \
\\node (a) at (0.5,0.5) {$A$};\
\\node (b) at (3,3) {$B$} edge[->] node[above] {edge} (a);\
\\end{tikzpicture}" 

  return createButton(XW_TIKZ_SRC_EDGE,tr("edge"));
}

QToolButton * XWTikzPathOperationWidget::createEllipseButton()
{
#define XW_TIKZ_SRC_ELLIPSE "\\tikz \\draw (1.76,1.76) ellipse [x radius=1.5cm,y radius=1cm];"

  return createButton(XW_TIKZ_SRC_ELLIPSE,tr("ellipse"));
}

QToolButton * XWTikzPathOperationWidget::createEllipseSplitButton()
{
#define XW_TIKZ_SRC_ELLIPSESPLIT "\\tikz \\draw (1.5,1.5) node[ellipse split,double,draw=black,fill=red!20] {text\\nodepart{text,lower}\ntext};"

  return createButton(XW_TIKZ_SRC_ELLIPSESPLIT,tr("ellipse split"));
}

QToolButton * XWTikzPathOperationWidget::createGridButton()
{
#define XW_TIKZ_SRC_GRID "\\tikz \\draw (0.5,0.5) grid[step=3mm] (3,3);"

  return createButton(XW_TIKZ_SRC_GRID,tr("grid"));
}

QToolButton * XWTikzPathOperationWidget::createHVLinesButton()
{
#define XW_TIKZ_SRC_HV "\\tikz \\draw (0.5,0.5) -| (3,3);"

  return createButton(XW_TIKZ_SRC_HV,tr("-|"));
}

QToolButton * XWTikzPathOperationWidget::createLineToButton()
{
#define XW_TIKZ_SRC_LINETO "\\tikz \\draw (0.5,0.5) -- (3,3);"

  return createButton(XW_TIKZ_SRC_LINETO,tr("line to"));
}

QToolButton * XWTikzPathOperationWidget::createMoveToButton()
{
#define XW_TIKZ_SRC_MOVETO "\\tikz \\path[fill=black!75] (1.76,1.76) circle [radius=1.5mm];"

  return createButton(XW_TIKZ_SRC_MOVETO,tr("move to"));
}

QToolButton * XWTikzPathOperationWidget::createNodeButton()
{
#define XW_TIKZ_SRC_NODEOP "\\tikz \\draw (1.5,1.5) node[double,draw=black,fill=red!20] {\\Large{node}};"

  return createButton(XW_TIKZ_SRC_NODEOP,tr("node"));
}

QToolButton * XWTikzPathOperationWidget::createParabolaButton()
{
#define XW_TIKZ_SRC_PARABOLA "\\tikz \\draw (0.5,0.5) parabola (3,3);"

  return createButton(XW_TIKZ_SRC_PARABOLA,tr("parabola"));
}

QToolButton * XWTikzPathOperationWidget::createPlotCoordinatesButton()
{
#define XW_TIKZ_SRC_PLOTCOORD "\\tikz \\draw (1.75,1.7) node[shape=circle,dotted] {coordinates};"
  
  return createButton(XW_TIKZ_SRC_PLOTCOORD,tr("plot coordinates"));
}

QToolButton * XWTikzPathOperationWidget::createPlotFileButton()
{
#define XW_TIKZ_SRC_PLOTFILE "\\tikz \\draw (1.75,1.7) node[shape=circle,dotted] {file};"

  return createButton(XW_TIKZ_SRC_PLOTFILE,tr("plot file"));
}

QToolButton * XWTikzPathOperationWidget::createPlotFunctionButton()
{
#define XW_TIKZ_SRC_PLOTFUNC "\\tikz \\draw (1.75,1.7) node[shape=circle,dotted] {function};"

  return createButton(XW_TIKZ_SRC_PLOTFUNC,tr("plot function"));
}

QToolButton * XWTikzPathOperationWidget::createRectangleButton()
{
#define XW_TIKZ_SRC_RECTANGLE "\\tikz \\draw (0.5,0.5) rectangle (3,3);"

  return createButton(XW_TIKZ_SRC_RECTANGLE,tr("rectangle"));
}

QToolButton * XWTikzPathOperationWidget::createRectangleSplitButton()
{
#define XW_TIKZ_SRC_RECTANGLESPLIT "\\tikz \\draw (1.5,1.5) node[ellipse split,double,draw=black,fill=red!20] {text\\nodepart{text,lower}\ntext};"

  return createButton(XW_TIKZ_SRC_RECTANGLESPLIT,tr("rectangle split"));
}

QToolButton * XWTikzPathOperationWidget::createSineButton()
{
#define XW_TIKZ_SRC_SIN "\\tikz \\draw (0.5,0.5) sin (3,3);"

  return createButton(XW_TIKZ_SRC_SIN,tr("sin"));
}

QToolButton * XWTikzPathOperationWidget::createVHLinesButton()
{
#define XW_TIKZ_SRC_VH "\\tikz \\draw (0.5,0.5) |- (3,3);"

  return createButton(XW_TIKZ_SRC_VH,tr("|-"));
}
