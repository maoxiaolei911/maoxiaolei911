/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTIKZOPTION_H
#define XWTIKZOPTION_H

#include <QStringList>
#include <QVector>
#include <QColor>
#include "XWTikzOperation.h"

class XWTikzGraphic;
class XWTikzCommand;
class XWTikzState;
class XWTikzExpress;
class XWTikzCoord;

class XWTikzKey : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzKey(XWTikzGraphic * graphicA, int idA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);
  void dragTo(XWTikzState * state);
  bool dropTo(XWTikzState * state);

  QString getText();
  QString getTips(XWTikzState * state);

  bool hitTest(XWTikzState * state);
};

class XWTikzValue : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzValue(XWTikzGraphic * graphicA, int idA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);
  void dragTo(XWTikzState * state);
  bool dropTo(XWTikzState * state);

  QString getCoord();
  QString getExpress();
  QPointF getPoint(XWTikzState * state);
  QString getText();
  QString getTips(XWTikzState * state);
  double getValue();

  bool hitTest(XWTikzState * state);

  void scan(const QString & str, int & len, int & pos);
  void setCoord(const QString & str);
  void setExpress(const QString & str);
  void setValue(double r);

private:
  union _Value
  {
    XWTikzExpress * expv;
    XWTikzCoord * coordv;
  }v;
  QString text;
};

class XWTikzDomain : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzDomain(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);
  void dragTo(XWTikzState * state);

  QString getEnd();
  QString getStart();
  QString getText();

  void scan(const QString & str, int & len, int & pos);
  void setEnd(const QString & str);
  void setStart(const QString & str);

private:
  double start;
  double end;
};

class XWTikzSamplesAt : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzSamplesAt(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);
  void dragTo(XWTikzState * state);

  QString getText();

  void scan(const QString & str, int & len, int & pos);
  void setSamples(const QString & str);

private:
  QList<double> samples;
};

class XWTikzColor : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzColor(XWTikzGraphic * graphicA, int idA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QColor  getColor();
  void    getColor(int & c1A,double & pA,int & c2A);
  QString getText();

  void scan(const QString & str, int & len, int & pos);
  void setColor(int c1A,double pA,int c2A);

private:
  int C1;
  double P;
  int C2;
};

class XWTikzDashPattern : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzDashPattern(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);
  void dragTo(XWTikzState * state);

  QString getText();

  void scan(const QString & str, int & len, int & pos);

private:
  QVector<qreal> pattern;
};

class XWTikzArrows : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzArrows(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);
  void dragTo(XWTikzState * state);

  int getEndArrow() {return earrow;}
  int getStartArrow() {return sarrow;}
  QString getText();

  void scan(const QString & str, int & len, int & pos);
  void setEndArrow(int a) {earrow=a;}
  void setStartArrow(int a) {sarrow=a;}

private:
  int sarrow,earrow;
};

class XWTikzAround : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzAround(XWTikzGraphic * graphicA, int idA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);
  void dragTo(XWTikzState * state);
  bool dropTo(XWTikzState * state);

  QString getCoord();
  QString getExpress();
  QString getText();
  QString getTips(XWTikzState * state);

  bool hitTest(XWTikzState * state);

  void scan(const QString & str, int & len, int & pos);
  void setCoord(const QString & str);
  void setExpress(const QString & str);

private:
  XWTikzExpress * expv;
  XWTikzCoord * coordv;
};

class XWTikzcm : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzcm(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);
  void dragTo(XWTikzState * state);
  bool dropTo(XWTikzState * state);

  QString getA();
  QString getB();
  QString getC();
  QString getCoord();
  QString getD();
  QString getText();
  QString getTips(XWTikzState * state);

  bool hitTest(XWTikzState * state);

  void scan(const QString & str, int & len, int & pos);
  void setA(const QString & str);
  void setB(const QString & str);
  void setC(const QString & str);
  void setCoord(const QString & str);
  void setD(const QString & str);

private:
  XWTikzExpress * a;
  XWTikzExpress * b;
  XWTikzExpress * c;
  XWTikzExpress * d;
  XWTikzCoord * coord;
};

class XWTikzOf : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzOf(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getFirst() {return path1;}
  QString getSecond() {return path2;}
  QString getText();

  void scan(const QString & str, int & len, int & pos);
  void setPath1(const QString & pa) {path1=pa;}
  void setPath2(const QString & pb) {path2=pb;}

private:
  QString path1;
  QString path2;
};

class XWTikzBy : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzBy(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);
  QString getText();

  void scan(const QString & str, int & len, int & pos);

private:
  QStringList names;
};


class XWTikzList : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzList(XWTikzGraphic * graphicA, int idA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);
  QString getText();

  void scan(const QString & str, int & len, int & pos);

private:
  QStringList list;
};

class XWTikzAndValue : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzAndValue(XWTikzGraphic * graphicA, int idA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);
  QString getText();

  void scan(const QString & str, int & len, int & pos);

private:
  int value1,value2;
};

class XWTikzThreePoint : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzThreePoint(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);
  QString getText();

  void scan(const QString & str, int & len, int & pos);

private:
  XWTikzCoord * one;
  XWTikzCoord * two;
  XWTikzCoord * three;
};

class XWTikzCodes : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzCodes(XWTikzGraphic * graphicA, int id, QObject * parent = 0);

  virtual void doPath(XWTikzState * state, bool showpoint = false);

  QPointF getPoint(XWTikzState * stateA);

  bool isMe(const QString & nameA,XWTikzState * state);

  void scanCommands(const QString & str, int & len, int & pos);

protected:
  int cur;
  QList<XWTikzCommand*> cmds;
};

class XWTikzAtPositionWidth : public XWTikzCodes
{
  Q_OBJECT

public:
  XWTikzAtPositionWidth(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();

  void scan(const QString & str, int & len, int & pos);

private:
  XWTikzExpress * postion;
};

class XWTikzBetweenPositionsWidth : public XWTikzCodes
{
  Q_OBJECT

public:
  XWTikzBetweenPositionsWidth(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();

  void scan(const QString & str, int & len, int & pos);

private:
  XWTikzExpress * startPos;
  XWTikzExpress * endPos;
  XWTikzExpress * step;
};

class XWTikzMark : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzMark(XWTikzGraphic * graphicA, QObject * parent = 0);

  void doPath(XWTikzState * state, bool showpoint = false);

  QString getText();

  bool isMe(const QString & nameA,XWTikzState * state);

  void scan(const QString & str, int & len, int & pos);

private:
  XWTikzCodes * mark;
};

#endif //XWTIKZOPTION_H
