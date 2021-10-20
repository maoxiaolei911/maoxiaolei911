/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTIKZGRAPHIC_H
#define XWTIKZGRAPHIC_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>
#include <QHash>
#include <QPointF>
#include <QUndoStack>
#include <QPainter>
#include <QMenu>

#define XW_TIKZ_S_GRAPHIC   0
#define XW_TIKZ_S_OPERATION 1
#define XW_TIKZ_S_PATH      2
#define XW_TIKZ_S_SCOPE     3

class XWPDFDriver;
class XWTikzState;
class XWTikzCommand;
class XWTIKZOptions;
class XWTikzNode;
class XWTikzCoord;
class XWTikzOperation;

class XWTikzGraphic : public QObject
{
  Q_OBJECT

public:
  XWTikzGraphic(QObject * parent = 0);
  ~XWTikzGraphic();

  bool addMenuAction(QMenu & menu);

  void back();

  void cut();

  void del();
  void doChildAnchor(XWTikzState * state);
  void doCopy(XWTikzState * state);
  void doDecoration(XWTikzState * state);
  void doEdgeFromParent(XWTikzState * state);
  void doEdgeFromParentPath(XWTikzState * state);
  void doEveryAcceptingByArrow(XWTikzState * state);
  void doEveryAttribute(XWTikzState * state);
  void doEveryChild(XWTikzState * state);
  void doEveryChildNode(XWTikzState * state);
  void doEveryCircuitAnnotation(XWTikzState * state);
  void doEveryCircuitSymbol(XWTikzState * state);
  void doEveryConcept(XWTikzState * state);
  void doEveryEdge(XWTikzState * state);
  void doEveryEntity(XWTikzState * state);
  void doEveryInfo(XWTikzState * state);
  void doEveryInitialByArrow(XWTikzState * state);
  void doEveryLabel(XWTikzState * state);
  void doEveryMark(XWTikzState * state);
  void doEveryMatrix(XWTikzState * state);
  void doEveryMindmap(XWTikzState * state);
  void doEveryNode(XWTikzState * state);
  void doEveryPin(XWTikzState * state);
  void doEveryPinEdge(XWTikzState * state);
  void doEveryRelationship(XWTikzState * state);
  void doEveryShadow(XWTikzState * state);
  void doEveryShape(XWTikzState * state);
  void doEveryState(XWTikzState * state);
  void doGraphic(XWPDFDriver * driver);
  void doLevel(XWTikzState * state);
  void doLevelConcept(XWTikzState * state);
  void doLevelNumber(XWTikzState * state);
  void doOperation(XWPDFDriver * driver);
  void doParentAnchor(XWTikzState * state);
  void doPath(XWPDFDriver * driver);
  void doPath(XWTikzState * state);
  void doRootConcept(XWTikzState * state);
  void doScope(XWPDFDriver * driver);
  void doSpyConnection(XWTikzState * state);
  void doSpyNode(XWTikzState * state);  
  void doState(XWTikzState * state);
  void doToPath(XWTikzState * state);
  void dragTo(XWPDFDriver * driver,const QPointF & p);
  void dropTo(const QPointF & p);

  int getAnchorPosition();
  XWTikzOperation * getCurrentOperation();
  XWTikzCoord * getCurrentPoint();
  int getCursorPosition();
  QString getCurrentText();
  int    getCurrentScope() {return curScope;}
  double getGridStep() {return gridStep;}
  double getHeight();
  QPointF getIntersection(int s);
  void   getIntersections(const QString & nameA,const QString & nameB);
  double getMaxX() {return maxX;}
  double getMaxY() {return maxY;}
  double getMinX() {return minX;}
  double getMinY() {return minY;}
  QString getLocalPath();
  QPointF getNodeAnchor(const QString & nameA,int a);
  QPointF getNodeAngle(const QString & nameA,double a);
  QPointF getPathBoundboxCenter();
  QPointF getPoint(const QString & nameA);
  QVector3D getPoint3D(const QString & nameA);
  QString getSelectedText();
  QString getText();
  QString getTips(const QPointF & p);
  QString getUnit(const QString & nameA);
  double getWidth();
  void goToEnd();
  void goToNext();
  void goToNextOperation();
  void goToNextPath();
  void goToNextScope();
  void goToOperationEnd();  
  void goToOperationStart();
  void goToPathEnd();
  void goToPathStart();
  void goToPrevious();
  void goToPreviousOperation();
  void goToPreviousPath();
  void goToPreviousScope();
  void goToStart();

  void hitTest(const QPointF & p);

  void insert(int i, XWTikzCommand * cmd);
  void insertText(const QString & str);
  bool isUnit(const QString & nameA);

  void keyInput(const QString & str);

  void moveDown();
  void moveDownBig();
  void moveDownSmall();
  void moveLeft();
  void moveLeftBig();
  void moveLeftHuge();
  void moveLeftSmall();
  void moveRight();
  void moveRightBig();
  void moveRightHuge();
  void moveRightSmall();
  void moveUp();
  void moveUpBig();
  void moveUpSmall();

  void newPar();

  void paste(const QString & str);
  void push(QUndoCommand * c);

  void registNamed(const QString & n);

  void scan(const QString & str);
  void setBy(const QStringList & b);
  void setCoord(const QString & nameA, XWTikzCoord * p);
  void setGridStep(double s) {gridStep=s;}
  void setScope(int s) {curScope=s;}  
  void setSize(double minxA, double minyA,double maxxA,double maxyA);
  void setSortBy(const QString & nameA);
  void setUnit(const QString & nameA, const QString & u);

  XWTikzCommand * takeAt(int i);

  void updateCursor(double minxA,double minyA,double maxxA,double maxyA);

signals:
  void changed();
  void cursorChanged(double minxA,double minyA,double maxxA,double maxyA);
  void viewChanged();

public slots:
  void addArc();
  void addCircle();
  void addCircleSolidus();
  void addCircleSplit();
  void addCoordinate();
  void addCoordinateCommand();
  void addCosine();
  void addCurveTo();
  void addCycle();
  void addEdge();
  void addEllipse();
  void addEllipseSplit();
  void addGrid();
  void addHVLines();
  void addLineTo();
  void addMoveTo();
  void addNode();
  void addNodeCommand();
  void addParabola();
  void addPath(int keywordA);
  void addPlotCoordinates();
  void addPlotFile();
  void addPlotFunction();
  void addRectangle();
  void addRectangleSplit();
  void addScope();
  void addSine();
  void addSpy();
  void addVHLines();

  void initUnits();

  void newTikz();
  void newTikzpicture();

  void redo();

  void undo();

private:
  void reset();

private:
  int curScope;
  int keyWord,cur;
  double gridStep;
  double minX, minY, maxX, maxY;  
  int namedPath1,namedPath2,sortBy;
  QTransform transform;
  QPointF lastPoint;
  QUndoStack  * undoStack;
  XWTIKZOptions * options;
  QHash<QString,int> names;
  QHash<QString,QString> units;
  QList<XWTikzCommand*> cmds;
  QList<QPointF> intersections;
  QStringList by;
};

#endif //XWTIKZGRAPHIC_H
