/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTIKZCOMMAND_H
#define XWTIKZCOMMAND_H

#include <QObject>
#include <QString>
#include <QPointF>
#include <QVector3D>
#include <QMenu>

class XWTikzGraphic;
class XWTikzState;
class XWTikzCommand;
class XWTikzOperation;
class XWTikzCoord;
class XWTIKZOptions;
class XWTikzCoordinate;
class XWTikzNode;
class XWTikzMatrix;
class XWTikzScope;

#define XW_TIKZ_GROUP -2

XWTikzCommand * createPGFObject(XWTikzGraphic * graphicA,XWTikzScope *scopeA,int id,QObject * parent);

class XWTikzCommand : public QObject
{
  Q_OBJECT

public:
  XWTikzCommand(XWTikzGraphic * graphicA, XWTikzScope *scopeA,int idA,QObject * parent = 0);
  virtual ~XWTikzCommand();

  virtual void addArc();
  virtual void addCircle();
  virtual void addCoordinate();
  virtual void addCosine();
  virtual void addCurveTo();
  virtual void addCycle();
  virtual void addEdge();
  virtual void addEllipse();
  virtual void addGrid();
  virtual void addHVLines();
  virtual void addLineTo();
  virtual void addMoveTo();
  virtual void addNode();
  virtual bool addOperationAction(QMenu & menu, XWTikzState * state);
  virtual void addParabola();
  virtual void addPathAction(QMenu & menu, XWTikzState * state);
  virtual void addPlotCoordinates();
  virtual void addPlotFile();
  virtual void addPlotFunction();
  virtual void addRectangle();
  virtual void addSine();
  virtual void addSplit(const QString & key);
  virtual void addVHLines();

  virtual bool back(XWTikzState * state);

  virtual bool cut(XWTikzState * state);

  virtual bool del(XWTikzState * state);
  virtual void doChildAnchor(XWTikzState * state);
  virtual void doCopy(XWTikzState * state);
  virtual void doDecoration(XWTikzState * state);
  virtual void doEdgeFromParent(XWTikzState * state);
  virtual void doEdgeFromParentPath(XWTikzState * state);
  virtual void doEveryAcceptingByArrow(XWTikzState * state);
  virtual void doEveryAttribute(XWTikzState * state);
  virtual void doEveryChild(XWTikzState * state);
  virtual void doEveryChildNode(XWTikzState * state);
  virtual void doEveryCircuitAnnotation(XWTikzState * state);
  virtual void doEveryCircuitSymbol(XWTikzState * state);
  virtual void doEveryConcept(XWTikzState * state);
  virtual void doEveryEdge(XWTikzState * state);
  virtual void doEveryEntity(XWTikzState * state);
  virtual void doEveryInfo(XWTikzState * state);
  virtual void doEveryInitialByArrow(XWTikzState * state);
  virtual void doEveryLabel(XWTikzState * state);
  virtual void doEveryMark(XWTikzState * state);
  virtual void doEveryMatrix(XWTikzState * state);
  virtual void doEveryMindmap(XWTikzState * state);
  virtual void doEveryNode(XWTikzState * state);
  virtual void doEveryPin(XWTikzState * state);
  virtual void doEveryPinEdge(XWTikzState * state);
  virtual void doEveryRelationship(XWTikzState * state);
  virtual void doEveryShadow(XWTikzState * state);
  virtual void doEveryShape(XWTikzState * state);
  virtual void doEveryState(XWTikzState * state);
  virtual void doLevel(XWTikzState * state);
  virtual void doLevelConcept(XWTikzState * state);
  virtual void doLevelNumber(XWTikzState * state);
  virtual void doOperation(XWTikzState * state, bool showpoint = false);
  virtual void doParentAnchor(XWTikzState * state);
  virtual void doPath(XWTikzState * state, bool showpoint = false);
  virtual void doPathNoOptions(XWTikzState * state);
  virtual void doRootConcept(XWTikzState * state);
  virtual void doSpyConnection(XWTikzState * state);
  virtual void doSpyNode(XWTikzState * state);  
  virtual void doState(XWTikzState * state);
  virtual void doToPath(XWTikzState * state);
  virtual void dragTo(XWTikzState * state);
  virtual bool dropTo(XWTikzState * state);

  virtual QPointF getAnchor(const QString & nameA,int a,XWTikzState * state);
  virtual int getAnchorPosition();
  virtual QPointF getAngle(const QString & nameA,double a,XWTikzState * state);
  virtual QPointF getCenter(XWTikzState * state);
  virtual XWTikzOperation * getCurrentOperation();
  virtual XWTikzCoord * getCurrentPoint();
  virtual int getCursorPosition();
  virtual QString getCurrentText();
  virtual QList<QPointF> getIntersections(const QList<int> & operationsA,
                                          const QList<QPointF> & pointsA,
                                          XWTikzState * state);
  int getKeyWord() {return keyWord;}
  virtual void    getPath(QList<int> & operationsA,
                          QList<QPointF> & pointsA,
                          XWTikzState * state);
  virtual QPointF getPoint(const QString & nameA,XWTikzState * state);
  virtual QPointF getPoint(XWTikzState * stateA);
  virtual QVector3D getPoint3D(const QString & nameA,XWTikzState * state);
  virtual QString getSelectedText();
  virtual QString getText();
  virtual QString getTips(XWTikzState * state);
  virtual void getWidthAndHeight(double & w, double & h);
  virtual void goToEnd();
  virtual bool goToNext();
  virtual void goToNextPoint();
  virtual void goToOperationEnd();  
  virtual void goToOperationStart();
  virtual void goToPathEnd();
  virtual void goToPathStart();
  virtual bool goToPrevious();
  virtual void goToPreviousPoint();
  virtual void goToStart();

  virtual bool hitTest(XWTikzState * state);

  virtual bool insertText(XWTikzState * state);
  virtual bool isMe(const QString & nameA,XWTikzState * state);

  virtual bool keyInput(XWTikzState * state);

  virtual bool newPar(XWTikzState * state);

  virtual bool paste(XWTikzState * state);

  virtual void scan(const QString & str, int & len, int & pos);

protected:
  XWTikzGraphic * graphic;
  XWTikzScope * scope;
  int keyWord;  
  XWTIKZOptions * options;
};


class XWTikzPath : public XWTikzCommand
{
  Q_OBJECT

public:
  XWTikzPath(XWTikzGraphic * graphicA,XWTikzScope *scopeA,int idA,QObject * parent = 0);
  virtual ~XWTikzPath();

  void addArc();
  void addCircle();
  void addCoordinate();
  void addCosine();
  void addCurveTo();
  void addCycle();
  void addEdge();
  void addEllipse();
  void addGrid();
  void addHVLines();
  void addLineTo();
  void addMoveTo();
  void addNode();
  bool addOperationAction(QMenu & menu, XWTikzState * state);
  void addParabola();
  void addPathAction(QMenu & menu, XWTikzState * state);
  void addPlotCoordinates();
  void addPlotFile();
  void addPlotFunction();
  void addRectangle();
  void addSine();
  void addSplit(const QString & key);
  void addVHLines();

  bool back(XWTikzState * state);

  bool cut(XWTikzState * state);

  bool del(XWTikzState * state);
  void doOperation(XWTikzState * state, bool showpoint = false);
  void doPath(XWTikzState * state, bool showpoint = false);
  void doPathNoOptions(XWTikzState * state);
  void dragTo(XWTikzState * state);
  bool dropTo(XWTikzState * state);

  QPointF getAnchor(const QString & nameA,int a,XWTikzState * state);
  int getAnchorPosition();
  QPointF getAngle(const QString & nameA,double a,XWTikzState * state);
  QPointF getCenter(XWTikzState * state);
  XWTikzOperation * getCurrentOperation();
  XWTikzCoord * getCurrentPoint();
  int getCursorPosition();
  QString getCurrentText();
  QList<QPointF> getIntersections(const QList<int> & operationsA,
                                  const QList<QPointF> & pointsA,
                                  XWTikzState * state);
  void    getPath(QList<int> & operationsA,
                  QList<QPointF> & pointsA,
                  XWTikzState * state);
  QPointF getPoint(const QString & nameA,XWTikzState * state);
  QVector3D getPoint3D(const QString & nameA,XWTikzState * state);
  QString getSelectedText();
  QString getText();
  QString getTips(XWTikzState * state);
  void getWidthAndHeight(double & w, double & h);
  void goToEnd();
  bool goToNext();
  void goToNextPoint();
  void goToOperationEnd();  
  void goToOperationStart();
  void goToPathEnd();
  void goToPathStart();
  bool goToPrevious();
  void goToPreviousPoint();
  void goToStart();

  bool hitTest(XWTikzState * state);

  void insert(int i, XWTikzOperation * opA);
  bool insertText(XWTikzState * state);

  bool keyInput(XWTikzState * state);

  bool newPar(XWTikzState * state);

  bool paste(XWTikzState * state);

  void scan(const QString & str, int & len, int & pos);

  XWTikzOperation * takeAt(int i);

private:
  int cur;
  QList<XWTikzOperation*> ops;
};

class XWTikzScope : public XWTikzCommand
{
  Q_OBJECT

public:
  XWTikzScope(XWTikzGraphic * graphicA,XWTikzScope *scopeA,int idA,QObject * parent = 0);
  ~XWTikzScope();

  void addArc();
  void addCircle();
  void addCoordinate();
  void addCoordinateCommand();
  void addCosine();
  void addCurveTo();
  void addCycle();
  void addEdge();
  void addEllipse();
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
  void addScope();
  void addScopeAction(QMenu & menu, XWTikzState * state);
  void addSine();
  void addSplit(const QString & key);
  void addSpy();
  void addVHLines();

  bool back(XWTikzState * state);

  bool cut(XWTikzState * state);

  bool del(XWTikzState * state);
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
  void doLevel(XWTikzState * state);
  void doLevelConcept(XWTikzState * state);
  void doLevelNumber(XWTikzState * state);
  void doOperation(XWTikzState * state, bool showpoint = false);
  void doParentAnchor(XWTikzState * state);
  void doPath(XWTikzState * state, bool showpoint = false);
  void doPathNoOptions(XWTikzState * state);
  void doRootConcept(XWTikzState * state);
  void doScope(XWTikzState * state);
  void doSpyConnection(XWTikzState * state);
  void doSpyNode(XWTikzState * state);  
  void doState(XWTikzState * state);
  void doToPath(XWTikzState * state);
  void dragTo(XWTikzState * state);
  bool dropTo(XWTikzState * state);

  QPointF getAnchor(const QString & nameA,int a, XWTikzState * state);
  int getAnchorPosition();
  QPointF getAngle(const QString & nameA,double a, XWTikzState * state);
  QPointF getCenter(XWTikzState * state);
  XWTikzOperation * getCurrentOperation();
  XWTikzCoord * getCurrentPoint();
  int getCursorPosition();
  QString getCurrentText();
  void   getIntersections(const QString & nameA,const QString & nameB);
  QList<QPointF> getIntersections(const QList<int> & operationsA,
                                  const QList<QPointF> & pointsA,
                                  XWTikzState * state);
  void    getPath(QList<int> & operationsA,
                  QList<QPointF> & pointsA,
                  XWTikzState * state);
  QPointF getPoint(const QString & nameA,XWTikzState * state);
  QVector3D getPoint3D(const QString & nameA,XWTikzState * state);
  QString getSelectedText();
  QString getText();
  QString getTips(XWTikzState * state);
  void goToEnd();
  bool goToNext();
  bool goToNextOperation();
  bool goToNextPath();
  bool goToNextScope();
  void goToOperationEnd();  
  void goToOperationStart();
  void goToPathEnd();
  void goToPathStart();
  bool goToPrevious();
  bool goToPreviousOperation();
  bool goToPreviousPath();
  bool goToPreviousScope();
  void goToStart();

  bool hitTest(XWTikzState * state);

  void insert(int i, XWTikzCommand * cmd);
  bool insertText(XWTikzState * state);
  bool isMe(const QString & nameA,XWTikzState * state);

  bool keyInput(XWTikzState * state);

  bool newPar(XWTikzState * state);

  bool paste(XWTikzState * state);

  void registNamed(const QString & n);

  void scan(const QString & str, int & len, int & pos);
  void setSortBy(const QString & nameA);

  XWTikzCommand * takeAt(int i);

private:
  int cur;
  int namedPath1,namedPath2,sortBy;
  QHash<QString,int> names;
  QList<XWTikzCommand*> cmds;
};

class XWTikzForeach : public XWTikzCommand
{
  Q_OBJECT

public:
  XWTikzForeach(XWTikzGraphic * graphicA,XWTikzScope *scopeA,QObject * parent = 0);

  bool back(XWTikzState * state);

  bool cut(XWTikzState * state);

  bool del(XWTikzState * state);
  void doOperation(XWTikzState * state, bool showpoint = false);
  void doPath(XWTikzState * state, bool showpoint = false);
  void doPathNoOptions(XWTikzState * state);
  void doScope(XWTikzState * state);
  bool dropTo(XWTikzState * state);

  int getAnchorPosition();
  XWTikzOperation * getCurrentOperation();
  XWTikzCoord * getCurrentPoint();
  int getCursorPosition();
  QString getCurrentText();
  QString getSelectedText();
  QString getText();
  QString getTips(XWTikzState * state);
  void goToEnd();
  bool goToNext();
  bool goToNextOperation();
  bool goToNextPath();
  void goToOperationEnd();  
  void goToOperationStart();
  void goToPathEnd();
  void goToPathStart();
  bool goToPrevious();
  bool goToPreviousOperation();
  bool goToPreviousPath();
  void goToStart();

  bool hitTest(XWTikzState * state);

  bool insertText(XWTikzState * state);

  bool keyInput(XWTikzState * state);

  bool newPar(XWTikzState * state);

  bool paste(XWTikzState * state);

  void scan(const QString & str, int & len, int & pos);

private:
  int cur;
  QStringList variables;
  QStringList list;
  QList<XWTikzCommand*> cmds;
};

class XWTikzCoordinatePath : public XWTikzCommand
{
  Q_OBJECT

public:
  XWTikzCoordinatePath(XWTikzGraphic * graphicA,XWTikzScope *scopeA,QObject * parent = 0);
  XWTikzCoordinatePath(XWTikzGraphic * graphicA,XWTikzScope *scopeA,int idA,QObject * parent = 0);

  virtual void addPathAction(QMenu & menu, XWTikzState * state);

  virtual bool back(XWTikzState * state);

  virtual bool cut(XWTikzState * state);

  virtual bool del(XWTikzState * state);
  virtual void doOperation(XWTikzState * state, bool showpoint = false);
  virtual void doPath(XWTikzState * state, bool showpoint = false);
  virtual void doPathNoOptions(XWTikzState * state);
  virtual void dragTo(XWTikzState * state);
  virtual bool dropTo(XWTikzState * state);

  virtual QPointF getAnchor(const QString & nameA,int a, XWTikzState * state);
  virtual int getAnchorPosition();
  virtual QPointF getAngle(const QString & nameA,double a, XWTikzState * state);
  QPointF getCenter(XWTikzState * state);
  XWTikzOperation * getCurrentOperation();
  XWTikzCoord * getCurrentPoint();
  virtual QString getCurrentText();
  virtual int getCursorPosition();
  virtual QPointF getPoint(XWTikzState * stateA);
  virtual QPointF getPoint(const QString & nameA,XWTikzState * state);
  virtual QVector3D getPoint3D(const QString & nameA,XWTikzState * state);
  virtual QString getSelectedText();
  virtual QString getText();
  virtual QString getTips(XWTikzState * state);
  virtual void getWidthAndHeight(double & w, double & h);
  virtual bool goToNext();
  virtual void goToNextPoint();
  virtual void goToOperationEnd();  
  virtual void goToOperationStart();
  virtual void goToPathEnd();
  virtual void goToPathStart();
  virtual bool goToPrevious();
  virtual void goToPreviousPoint();

  virtual bool hitTest(XWTikzState * state);

  virtual bool insertText(XWTikzState * state);
  virtual bool isMe(const QString & nameA,XWTikzState * state);

  virtual bool keyInput(XWTikzState * state);

  virtual bool newPar(XWTikzState * state);

  virtual bool paste(XWTikzState * state);

  virtual void scan(const QString & str, int & len, int & pos);
  virtual void setAt(const QString & str);
  virtual void setName(const QString & str);

protected:
  XWTikzCoordinate * node;
  XWTikzCoord * coord;
};

class XWTikzNodePath : public XWTikzCoordinatePath
{
  Q_OBJECT

public:
  XWTikzNodePath(XWTikzGraphic * graphicA,XWTikzScope *scopeA,QObject * parent = 0);

  bool addOperationAction(QMenu & menu, XWTikzState * state);

  QString getText();
  QString getTips(XWTikzState * state);

  void scan(const QString & str, int & len, int & pos);
  void setText(const QString & str);
};

class XWTikzMatrixCommand : public XWTikzCommand
{
  Q_OBJECT

public:
  XWTikzMatrixCommand(XWTikzGraphic * graphicA,XWTikzScope *scopeA,QObject * parent = 0);

  bool back(XWTikzState * state);

  bool cut(XWTikzState * state);

  bool del(XWTikzState * state);
  void doPath(XWTikzState * state, bool showpoint = false);
  void dragTo(XWTikzState * state);
  bool dropTo(XWTikzState * state);

  int getAnchorPosition();
  int getCursorPosition();
  QString getCurrentText();
  QString getSelectedText();
  QString getText();
  void goToEnd();
  bool goToNext();
  bool goToPrevious();
  void goToStart();

  bool hitTest(XWTikzState * state);

  bool insertText(XWTikzState * state);

  bool keyInput(XWTikzState * state);

  bool newPar(XWTikzState * state);

  bool paste(XWTikzState * state);

  void scan(const QString & str, int & len, int & pos);

private:
  XWTikzMatrix * matrix;
};

class XWTikzSpy : public XWTikzCommand
{
  Q_OBJECT

public:
  XWTikzSpy(XWTikzGraphic * graphicA,XWTikzScope *scopeA,QObject * parent = 0);

  void addPathAction(QMenu & menu, XWTikzState * state);

  void doCopy(XWTikzState * state);
  void doPath(XWTikzState * state, bool showpoint = false);
  void dragTo(XWTikzState * state);
  bool dropTo(XWTikzState * state);

  QString getText();
  QString getTips(XWTikzState * state);

  bool hitTest(XWTikzState * state);

  void scan(const QString & str, int & len, int & pos);
  void setAt(const QString & str);
  void setOn(const QString & str);

protected:
  XWTikzCoord * on;
  XWTikzNode  * node;
  XWTikzCoord * at;
  XWTikzCoord * curPoint;
};

class XWTikzArrowMarking : public XWTikzCommand
{
  Q_OBJECT

public:
  XWTikzArrowMarking(XWTikzGraphic * graphicA,XWTikzScope *scopeA,QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();

  void scan(const QString & str, int & len, int & pos);

private:
  int arrowTip;
};

#endif //XWTIKZCOMMAND_H
