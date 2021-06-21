/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTIKZTOOLWIDGET_H
#define XWTIKZTOOLWIDGET_H

#include <QWidget>
#include <QToolBox>

class QToolButton;
class XWTikzGraphic;

class XWTikzToolBox : public QToolBox
{
   Q_OBJECT

public:
   XWTikzToolBox(XWTikzGraphic * graphicA,QWidget * parent = 0);

private:
  XWTikzGraphic * graphic;
};


class XWTikzToolsWidget : public QWidget
{
  Q_OBJECT

public:
   XWTikzToolsWidget(XWTikzGraphic * graphicA,QWidget * parent = 0);

protected:
   QToolButton * createButton(const QString & src,const QString & txt);

protected:
   XWTikzGraphic * graphic;
};

class XWTikzPathActionWidget : public XWTikzToolsWidget
{
   Q_OBJECT

public:
   XWTikzPathActionWidget(XWTikzGraphic * graphicA,QWidget * parent = 0);

private slots:
  void addClip();
  void addDraw();
  void addFill();
  void addFillDraw();
  void addPath();
  void addPattern();
  void addShade();
  void addShadeDraw();
  void addSpy();

private:
  QToolButton * createClipButton();
  QToolButton * createCoordinateButton();
  QToolButton * createDrawButton();
  QToolButton * createFillButton();
  QToolButton * createFillDrawButton();
  QToolButton * createNodeButton();
  QToolButton * createPathButton();
  QToolButton * createPatternButton();
  QToolButton * createScopeButton();
  QToolButton * createShadeButton();
  QToolButton * createShadeDrawButton();
  QToolButton * createSpyButton();
};

class XWTikzPathOperationWidget : public XWTikzToolsWidget
{
   Q_OBJECT

public:
   XWTikzPathOperationWidget(XWTikzGraphic * graphicA,QWidget * parent = 0);

private:
  QToolButton * createArcButton();
  QToolButton * createCircleButton();
  QToolButton * createCircleSolidusButton();
  QToolButton * createCircleSplitButton();
  QToolButton * createCoordinateButton();
  QToolButton * createCosineButton();
  QToolButton * createCurveToButton();
  QToolButton * createCycleButton();
  QToolButton * createEdgeButton();
  QToolButton * createEllipseButton();
  QToolButton * createEllipseSplitButton();
  QToolButton * createGridButton();
  QToolButton * createHVLinesButton();
  QToolButton * createLineToButton();
  QToolButton * createMoveToButton();
  QToolButton * createNodeButton();
  QToolButton * createParabolaButton();
  QToolButton * createPlotCoordinatesButton();
  QToolButton * createPlotFileButton();
  QToolButton * createPlotFunctionButton();
  QToolButton * createRectangleButton();
  QToolButton * createRectangleSplitButton();
  QToolButton * createSineButton();
  QToolButton * createVHLinesButton();
};

#endif //XWTIKZTOOLWIDGET_H
