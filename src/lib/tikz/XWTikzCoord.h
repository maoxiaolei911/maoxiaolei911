/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTIKZCOORD_H
#define XWTIKZCOORD_H

#include "XWTikzOperation.h"

class XWTikzState;
class XWTikzGraphic;
class XWTikzExpress;
class XWTikzCoordExpress;
class XWTIKZOptions;

class XWTikzCoord : public XWTikzOperation
{
  Q_OBJECT

public:
  XWTikzCoord(XWTikzGraphic * graphicA, QObject * parent = 0);
  XWTikzCoord(XWTikzGraphic * graphicA, const QString & str, QObject * parent = 0);
  ~XWTikzCoord();

  bool addAction(QMenu & menu);
  void addPoint(XWTikzState * state);

  void doPath(XWTikzState * state, bool showpoint = false);
  void draw(XWTikzState * state);
  void dragTo(XWTikzState * state);
  bool dropTo(XWTikzState * state);

  QPointF   getPoint(XWTikzState * state);
  QVector3D getRelPoint3D(XWTikzState * state);
  QVector3D getPoint3D(XWTikzState * state);  
  QString getText();
  QStringList getVarNames();

  bool hitTest(XWTikzState * state);

  void moveTo(const QVector3D & p);

  void scan(const QString & str, int & len, int & pos);  
  void setCoord(const QString & title);
  void setText(const QString & str);

private slots:
  void setCoord();

private:
  void clear();
  void scanComponent(const QString & str, 
                     int & len, int & pos, 
                     QString & compA,bool & polar,
                     bool & dim);
  void scanRest(const QString & str, 
                     int & len, int & pos, 
                     QString & compA);


private:
  int cs;
  int rel;
  XWTIKZOptions * options;

  struct _CS_Express
  {
    XWTikzExpress * c1;
    XWTikzExpress * c2;
    XWTikzExpress * c3;
  };

  struct _CS_Coord_Express
  {
    XWTikzCoordExpress * coord;
  };

  struct _CS_VA
  {
    XWTikzCoordExpress * v;
    XWTikzExpress * a;
  };

  struct _CS_Barycentric
  {
    int num;
    _CS_VA * vas;
  };

  struct _CS_Node
  {
    XWTikzCoordExpress * node;
    XWTikzExpress * a;
  };

  struct _CS_Tangent
  {
    XWTikzExpress * solution;
    XWTikzCoordExpress * node;
    XWTikzCoordExpress * point;
  };

  struct _CS_Perpendicular
  {
    XWTikzCoordExpress * coord1;
    XWTikzCoordExpress * coord2;
  };

  union _CSComponent
  {
    _CS_Express compEx;
    _CS_Coord_Express coordEx;
    _CS_Barycentric barycentric;
    _CS_Node nodeEx;
    _CS_Tangent tangent;
    _CS_Perpendicular perpendicular;
  } csc;
};

#endif //
