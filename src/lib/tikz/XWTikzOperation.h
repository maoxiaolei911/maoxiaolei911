/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTIKZOPERATION_H
#define XWTIKZOPERATION_H

#include <QObject>
#include <QPointF>
#include <QString>
#include <QList>
#include <QMenu>

class XWTikzState;
class XWTikzGraphic;
class XWTikzCoord;
class XWTIKZOptions;
class XWTikzCoordinate;

#define XW_TIKZ_COORD   -3
#define XW_TIKZ_OPTIONS -4
#define XW_TIKZ_LOCAL   -5
#define XW_TIKZ_NODE_BOX -20
#define XW_TIKZ_TEXT_BOX -21

class XWTikzOperation : public QObject
{
  Q_OBJECT

public:
  XWTikzOperation(XWTikzGraphic * graphicA, int idA, QObject * parent = 0);
  XWTikzOperation(XWTikzGraphic * graphicA, int idA, int subk, QObject * parent = 0);
  virtual ~XWTikzOperation();

  virtual bool addAction(QMenu & menu);
  virtual void addPoint(XWTikzState * state);

  virtual bool back(XWTikzState * state);

  virtual bool cut(XWTikzState * state);
  
  virtual bool del(XWTikzState * state);
  virtual void doPath(XWTikzState * state, bool showpoint = false);
  virtual void doPre(XWTikzState * state, bool showpoint = false);
  virtual void doPost(XWTikzState * state, bool showpoint = false);
  virtual void dragTo(XWTikzState * state);
  virtual bool dropTo(XWTikzState * state);

  virtual QPointF getAnchor(int a, XWTikzState * stateA, XWTikzState * state);
  virtual int getAnchorPosition();
  virtual QPointF getAngle(double a, XWTikzState * stateA, XWTikzState * state);
  virtual XWTikzCoord * getCurrentPoint();
  virtual int getCursorPosition();
  virtual QString getCurrentText();
  int     getKeyWord() {return keyWord;}
  virtual QPointF getPoint(XWTikzState * state);
  virtual QVector3D getPoint3D(XWTikzState * state);
  virtual QString getSelectedText();
  int     getSubkeyWord() {return subkeyWord;}
  virtual QString getText();
  virtual QString getTips(XWTikzState * state);
  virtual void goToEnd();
  virtual bool goToNext();
  virtual bool goToPrevious();
  virtual void goToStart();

  virtual bool hitTest(XWTikzState * state);

  virtual bool insertText(XWTikzState * state);
  virtual bool isMe(const QString & nameA,XWTikzState * state);

  virtual bool keyInput(XWTikzState * state);

  virtual bool newPar(XWTikzState * state);

  virtual bool paste(XWTikzState * state);

  virtual void scan(const QString & str, int & len, int & pos);
  static  void scanKey(const QString & str, int & len, 
                    int & pos, QString & key);
  static  void scanKeyValue(const QString & str, int & len, 
                    int & pos, QString & key, 
                    QString & value);
  static  void scanValue(const QString & str, int & len, 
                    int & pos, QString & value);
          void setKeyword(int k) {keyWord=k;}
          void setSubkeyWord(int k) {subkeyWord=k;}
  static  void skipOption(const QString & str, int & len,int & pos);

public:
  XWTikzGraphic * graphic;
  int keyWord;
  int subkeyWord;
  XWTikzCoord * curPoint;
};

class XWTikzMacro : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzMacro(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  void scan(const QString & str, int & len, int & pos);

private:
  QString name;
};

class XWTikzLocal : public XWTikzOperation
{
   Q_OBJECT

public:
  XWTikzLocal(XWTikzGraphic * graphicA, QObject * parent = 0);

  bool back(XWTikzState * state);

  bool del(XWTikzState * state);
  void doPath(XWTikzState * state, bool showpoint = false);
  void dragTo(XWTikzState * state);
  bool dropTo(XWTikzState * state);

  int getCursorPosition();
  QString getCurrentText();
  QString getSelectedText();
  QString getText();
  QString getTips(XWTikzState * state);

  bool hitTest(XWTikzState * state);

  bool insertText(XWTikzState * state);

  bool keyInput(XWTikzState * state);

  bool newPar(XWTikzState * state);

  bool paste(XWTikzState * state);

  void scan(const QString & str, int & len, int & pos);

private:
  int cur;
  QList<XWTikzOperation*> ops;
};

class XWTikzLineTo : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzLineTo(XWTikzGraphic * graphicA, QObject * parent = 0);

  bool addAction(QMenu & menu);
  void addPoint(XWTikzState * state);

  void doPath(XWTikzState * state, bool showpoint = false);
  void dragTo(XWTikzState * state);
  bool dropTo(XWTikzState * state);

  QPointF getPoint(XWTikzState * state);
  QString getText();

  bool hitTest(XWTikzState * state);

  void scan(const QString & str, int & len, int & pos);
  void setCoord(const QString & str);

private slots:
  void setCoord();

private:
  XWTikzCoord * coord;
};

class XWTikzHVLinesTo : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzHVLinesTo(XWTikzGraphic * graphicA, QObject * parent = 0);

  bool addAction(QMenu & menu);
  void addPoint(XWTikzState * state);

  void doPath(XWTikzState * state, bool showpoint = false);
  void dragTo(XWTikzState * state);
  bool dropTo(XWTikzState * state);

  QPointF getPoint(XWTikzState * state);
  QString getText();

  bool hitTest(XWTikzState * state);

  void scan(const QString & str, int & len, int & pos);
  void setCoord(const QString & str);

private slots:
  void setCoord();

private:
  XWTikzCoord * coord;
};

class XWTikzVHLinesTo : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzVHLinesTo(XWTikzGraphic * graphicA, QObject * parent = 0);

  bool addAction(QMenu & menu);
  void addPoint(XWTikzState * state);

  void doPath(XWTikzState * state, bool showpoint = false);
  void dragTo(XWTikzState * state);
  bool dropTo(XWTikzState * state);

  QPointF getPoint(XWTikzState * state);
  QString getText();

  bool hitTest(XWTikzState * state);

  void scan(const QString & str, int & len, int & pos);
  void setCoord(const QString & str);

private slots:
  void setCoord();

private:
  XWTikzCoord * coord;
};

class XWTikzCurveTo : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzCurveTo(XWTikzGraphic * graphicA, QObject * parent = 0);

  bool addAction(QMenu & menu);
  void addPoint(XWTikzState * state);

  void doPath(XWTikzState * state, bool showpoint = false);
  void dragTo(XWTikzState * state);
  bool dropTo(XWTikzState * state);

  QPointF getPoint(XWTikzState * state);
  QString getText();
  void goToEnd();
  bool goToNext();
  bool goToPrevious();
  void goToStart();

  bool hitTest(XWTikzState * state);

  void scan(const QString & str, int & len, int & pos);
  void setC1(const QString & str);
  void setC2(const QString & str);
  void setEndPoint(const QString & str);

private slots:
  void setC1();
  void setC2();
  void setEndPoint();

private:
  XWTikzCoord * c1;
  XWTikzCoord * c2;
  XWTikzCoord * endPoint;
};

class XWTikzCycle : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzCycle(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();

  bool hitTest(XWTikzState * state);
};

class XWTikzRectangle : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzRectangle(XWTikzGraphic * graphicA, QObject * parent = 0);

  bool addAction(QMenu & menu);
  void addPoint(XWTikzState * state);

  void doPath(XWTikzState * state, bool showpoint = false);
  void dragTo(XWTikzState * state);
  bool dropTo(XWTikzState * state);

  QPointF getPoint(XWTikzState * state);
  QString getText();

  bool hitTest(XWTikzState * state);

  void scan(const QString & str, int & len, int & pos);
  void setCoord(const QString & str);

private slots:
  void setCoord();

private:
  XWTikzCoord * coord;
};

class XWTikzEllipse : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzEllipse(XWTikzGraphic * graphicA, int idA, QObject * parent = 0);

  bool addAction(QMenu & menu);

  void doPath(XWTikzState * state, bool showpoint = false);
  void dragTo(XWTikzState * state);
  bool dropTo(XWTikzState * state);

  QString getText();

  bool hitTest(XWTikzState * state);

  void scan(const QString & str, int & len, int & pos);
  void setAt(const QString & str);
  void setRadius(const QString & str);
  void setXRadius(const QString & str);
  void setYRadius(const QString & str);

private slots:
  void setAt();
  void setRadius();
  void setXRadius();
  void setYRadius();

private:
  XWTIKZOptions * options;
};

class XWTikzArc : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzArc(XWTikzGraphic * graphicA, QObject * parent = 0);

  bool addAction(QMenu & menu);

  void doPath(XWTikzState * state, bool showpoint = false);
  void dragTo(XWTikzState * state);
  bool dropTo(XWTikzState * state);

  QString getText();

  bool hitTest(XWTikzState * state);

  void scan(const QString & str, int & len, int & pos);
  void setEndAngle(const QString & str);
  void setStartAngle(const QString & str);

private slots:
  void setEndAngle();
  void setStartAngle();

private:
  XWTIKZOptions * options;
};

class XWTikzGrid : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzGrid(XWTikzGraphic * graphicA, QObject * parent = 0);

  bool addAction(QMenu & menu);
  void addPoint(XWTikzState * state);

  void doPath(XWTikzState * state, bool showpoint = false);
  void dragTo(XWTikzState * state);
  bool dropTo(XWTikzState * state);

  QPointF getPoint(XWTikzState * state);
  QString getText();

  bool hitTest(XWTikzState * state);

  void scan(const QString & str, int & len, int & pos);
  void setCorner(const QString & str);
  void setStep(const QString & str);

private slots:
  void setCoord();
  void setStep();

private:
  XWTIKZOptions * options;
  XWTikzCoord * coord;
};

class XWTikzParabola : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzParabola(XWTikzGraphic * graphicA, QObject * parent = 0);

  bool addAction(QMenu & menu);
  void addPoint(XWTikzState * state);

  void doPath(XWTikzState * state, bool showpoint = false);
  void dragTo(XWTikzState * state);
  bool dropTo(XWTikzState * state);

  QPointF getPoint(XWTikzState * state);
  QString getText();
  void goToEnd();
  bool goToNext();
  bool goToPrevious();
  void goToStart();

  bool hitTest(XWTikzState * state);

  void scan(const QString & str, int & len, int & pos);
  void setBend(const QString & str);
  void setEnd(const QString & str);

private slots:
  void setBend();
  void setEnd();

private:
  XWTIKZOptions * options;
  XWTikzCoord * bendCoord;
  XWTikzCoord * coord;
};

class XWTikzSine : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzSine(XWTikzGraphic * graphicA, QObject * parent = 0);

  bool addAction(QMenu & menu);
  void addPoint(XWTikzState * state);

  void doPath(XWTikzState * state, bool showpoint = false);
  void dragTo(XWTikzState * state);
  bool dropTo(XWTikzState * state);

  QPointF getPoint(XWTikzState * state);
  QString getText();

  bool hitTest(XWTikzState * state);

  void scan(const QString & str, int & len, int & pos);
  void setCoord(const QString & str);

private slots:
  void setCoord();

private:
  XWTikzCoord * coord;
};

class XWTikzCosine : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzCosine(XWTikzGraphic * graphicA, QObject * parent = 0);

  bool addAction(QMenu & menu);
  void addPoint(XWTikzState * state);

  void doPath(XWTikzState * state, bool showpoint = false);
  void dragTo(XWTikzState * state);
  bool dropTo(XWTikzState * state);

  QPointF getPoint(XWTikzState * state);
  QString getText();

  bool hitTest(XWTikzState * state);

  void scan(const QString & str, int & len, int & pos);
  void setCoord(const QString & str);

private slots:
  void setCoord();

private:
  XWTikzCoord * coord;
};

class XWTikzPlotCoordinates : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzPlotCoordinates(XWTikzGraphic * graphicA, QObject * parent = 0);
  XWTikzPlotCoordinates(XWTikzGraphic * graphicA, bool c,XWTIKZOptions * opt,QObject * parent = 0);

  bool addAction(QMenu & menu);

  void doPath(XWTikzState * state, bool showpoint = false);
  bool dropTo(XWTikzState * state);

  QString getText();
  void goToEnd();
  bool goToNext();
  bool goToPrevious();
  void goToStart();

  bool hitTest(XWTikzState * state);

  void scan(const QString & str, int & len, int & pos);
  void setCoords(const QString & str);

private:
  bool isContinue;
  XWTIKZOptions * options;
  int cur;
  QList<XWTikzCoord *> coords;
};

class XWTikzPlotFile : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzPlotFile(XWTikzGraphic * graphicA, QObject * parent = 0);
  XWTikzPlotFile(XWTikzGraphic * graphicA, bool c,XWTIKZOptions * opt,QObject * parent = 0);

  bool addAction(QMenu & menu);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();

  void scan(const QString & str, int & len, int & pos);
  void setFileName(const QString & str);

private:
  bool isContinue;
  XWTIKZOptions * options;
  QString fileName;
};

class XWTikzPlotFunction : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzPlotFunction(XWTikzGraphic * graphicA, QObject * parent = 0);
  XWTikzPlotFunction(XWTikzGraphic * graphicA, bool c,XWTIKZOptions * opt,QObject * parent = 0);

  bool addAction(QMenu & menu);
  
  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();

  bool hitTest(XWTikzState * state);

  void scan(const QString & str, int & len, int & pos);
  void setDomain(const QString & s,const QString & e);
  void setExpress(const QString & str);

private slots:
  void setDomain();
  void setExpress();

private:
  bool isContinue;
  XWTIKZOptions * options;
  XWTikzCoord * coordExp;
};

class XWTikzTo : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzTo(XWTikzGraphic * graphicA, QObject * parent = 0);

  bool back(XWTikzState * state);

  bool cut(XWTikzState * state);

  bool del(XWTikzState * state);
  void doPath(XWTikzState * state, bool showpoint = false);
  void dragTo(XWTikzState * state);
  bool dropTo(XWTikzState * state);

  int     getAnchorPosition();
  int     getCursorPosition();
  QString getCurrentText();
  QString getSelectedText();
  QString getText();
  void    goToEnd();
  bool    goToNext();
  bool    goToPrevious();
  void    goToStart();

  bool hitTest(XWTikzState * state);

  bool insertText(XWTikzState * state);

  bool keyInput(XWTikzState * state);

  bool newPar(XWTikzState * state);

  bool paste(XWTikzState * state);

  void scan(const QString & str, int & len, int & pos);

private:
  int cur;
  XWTIKZOptions * options;
  XWTikzCoord * coord;
  QList<XWTikzCoordinate*> nodes;
};

class XWTikzDecorate : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzDecorate(XWTikzGraphic * graphicA, QObject * parent = 0);

  bool addAction(QMenu & menu);

  bool back(XWTikzState * state);

  bool cut(XWTikzState * state);

  bool del(XWTikzState * state);
  void doPath(XWTikzState * state, bool showpoint = false);
  void dragTo(XWTikzState * state);
  bool dropTo(XWTikzState * state);

  int     getAnchorPosition();
  int     getCursorPosition();
  QString getCurrentText();
  QString getSelectedText();
  QString getText();
  void    goToEnd();
  bool    goToNext();
  bool    goToPrevious();
  void    goToStart();

  bool hitTest(XWTikzState * state);

  bool insertText(XWTikzState * state);

  bool keyInput(XWTikzState * state);

  bool newPar(XWTikzState * state);

  bool paste(XWTikzState * state);

  void scan(const QString & str, int & len, int & pos);

private:
  int cur;
  XWTIKZOptions * options;
  QList<XWTikzOperation*> ops;
};

#endif //XWTIKZOPERATION_H
