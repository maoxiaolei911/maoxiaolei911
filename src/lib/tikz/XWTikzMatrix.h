/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTIKZMATRIX_H
#define XWTIKZMATRIX_H

#include <QList>
#include "XWTeXBox.h"

class XWTikzGraphic;
class XWTikzState;
class XWTikzCommand;

class XWTikzCell: public QObject
{
  Q_OBJECT

public:
  XWTikzCell(XWTikzGraphic * graphicA, QObject * parent = 0);

  bool back();

  bool cut();

  bool del();
  void doPath(XWTikzState * stateA, int align, double maxw);
  void dragTo(XWTikzState * stateA);
  bool dropTo(XWTikzState * stateA);

  int     getAnchorPosition();
  int     getCursorPosition();
  QString getCurrentText();
  QString getSelectedText();
  QString getText();
  void    getWidthAndHeight(double & w, double & h);
  void    goToEnd();
  bool    goToNext();
  bool    goToPrevious();
  void    goToStart();

  bool hitTest(double x, double y);

  bool insertText(const QString & str);

  bool keyInput(const QString & str);

  bool newPar();

  bool paste();

  void scan(const QString & str, int & len, int & pos);

private:
  XWTikzGraphic * graphic;
  int cur;
  double width;
  XWTikzState * state;
  QList<XWTikzCommand*> cmds;
};

class XWTikzRow: public QObject
{
  Q_OBJECT

public:
  XWTikzRow(XWTikzGraphic * graphicA, QObject * parent = 0);

  bool back();

  bool cut();

  bool del();
  void doPath(XWTikzState * state, int align, const QList<double> & wlist);
  void dragTo(XWTikzState * state);
  bool dropTo(XWTikzState * state);

  int     getAnchorPosition();
  int     getCursorPosition();
  QString getCurrentText();
  void    getMaxWidthAndHeight(QList<double> & wlist,double & rh);
  double  getNumberOfColumns();
  QString getSelectedText();
  QString getText();
  void    goToEnd();
  bool    goToNext();
  bool    goToPrevious();
  void    goToStart();

  bool hitTest(double x, double y);

  bool insertText(const QString & str);

  bool keyInput(const QString & str);

  bool newPar();

  bool paste();

  void scan(const QString & str, int & len, int & pos);

  void updateCursor(double minx, double miny, double maxx,
                    double maxy, int textposA);
  
private:
  XWTikzGraphic * graphic;
  int cur;
  QList<XWTikzCell*> columns;
};

class XWTikzMatrix : public XWTeXBox
{
  Q_OBJECT

public:
  XWTikzMatrix(XWTikzGraphic * graphicA, QObject * parent = 0);

  bool back();

  bool cut();

  bool del();
  void doPath(XWTikzState * stateA, bool showpoint = false);
  void doContent(XWPDFDriver * driver);
  void dragTo(XWPDFDriver * driver, double xA, double yA);
  void dragTo(XWTikzState * state);
  bool dropTo(XWTikzState * state);

  int     getAnchorPosition();
  int     getCursorPosition();
  QString getCurrentText();
  double getDepth(double maxw);
  double getHeight(double maxw);
  QString getSelectedText();
  QString getText();
  double  getWidth(double maxw);
  void    goToEnd();
  bool    goToNext();
  bool    goToPrevious();
  void    goToStart();

  bool    hitTest(double x, double y);

  bool    insertText(const QString & str);

  bool    keyInput(const QString & str);

  bool    newPar();

  bool    paste();

  void scan(const QString & str, int & len, int & pos);
  void setState(XWTikzState * stateA);
  void setXY(int alignA, double xA,double yA);

  void updateCursor(double minx, double miny, double maxx,
                    double maxy, int textposA);

private:
  void doRows(XWTikzState * stateA, bool showpoint);

  void getDimension();

private:
  XWTikzGraphic * graphic;
  XWTikzState * state;
  int cur;
  int align;
  double width,height,depth;
  QList<XWTikzRow*> rows;
};

#endif //XWTIKZMATRIX_H
