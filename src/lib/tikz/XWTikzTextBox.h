/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTIKZTEXTBOX_H
#define XWTIKZTEXTBOX_H

#include "XWTeXTextBox.h"

class XWTikzGraphic;
class XWTikzState;
class XWTikzNodePart;

class XWTikzTextBox : public XWTeXTextBox
{
  Q_OBJECT

public:
  XWTikzTextBox(XWTikzGraphic * graphicA,QObject * parent = 0);
  ~XWTikzTextBox();

  QString getText();

  void push(QUndoCommand * cmdA);

  void scan(const QString & str, int & len, int & pos);
  void setState(XWTikzState * stateA);

  void updateCursor(double minx, double miny, double maxx,
                    double maxy, int textposA);

private:
  XWTikzGraphic * graphic;
  XWTikzState * state;
  QList<XWTikzNodePart*> nodePart;
};

#endif // XWTIKZTEXTBOX_H
