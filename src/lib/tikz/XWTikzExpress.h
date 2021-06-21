/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTIKZEXPRESS_H
#define XWTIKZEXPRESS_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>
#include <QPointF>
#include <QVector3D>
#include <QVector4D>

#include "XWTikzType.h"

class XWTikzGraphic;
class XWTikzState;

class XW_TIKZ_EXPORT XWTikzExpress : public QObject
{
   Q_OBJECT

public:
   XWTikzExpress(XWTikzGraphic * graphicA,QObject * parent = 0);
   XWTikzExpress(XWTikzGraphic * graphicA,const QString & str, QObject * parent = 0);
   XWTikzExpress(XWTikzExpress * other, QObject * parent = 0);

   double getResult(XWTikzState * state);
   QString getText();
   QStringList getVarNames();
   double getValue() {return result;}

   bool isDim();
   bool isNum();
   bool isVar() {return hasVar;}

   void setText(const QString & str);
   void setValue(double v);

private:
  void calculate(XWTikzState * state);

  void scan(const QString & str, int & len, int & pos);

private:
   XWTikzGraphic * graphic;
   bool rescan;
   bool hasVar;
   double result;
   QStringList vars;
   QList<QPointF> postExpress;
   QString text;
};

class XW_TIKZ_EXPORT XWTikzCoordExpress : public QObject
{
   Q_OBJECT

public:
   XWTikzCoordExpress(XWTikzGraphic * graphicA,QObject * parent = 0);
   XWTikzCoordExpress(XWTikzGraphic * graphicA,const QString & str, QObject * parent = 0);
   XWTikzCoordExpress(XWTikzCoordExpress * other,QObject * parent = 0);

   QVector3D getResult(XWTikzState * state);
   QString getText();

   void setText(const QString & str);

private:
   void calculate(XWTikzState * state);

   void scan(const QString & str, int & len, int & pos);

private:
   XWTikzGraphic * graphic;
   bool rescan;
   bool hasVar;
   QVector3D result;
   QStringList vars;
   QList<QVector4D> postExpress;
   QString text;
};

#endif //XWTIKZEXPRESS_H
