/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSTLABOWIDGET_H
#define XWPSTLABOWIDGET_H

#include <QWidget>

class QToolButton;
class XWTikzGraphic;

class PSTLaboWidget : public QWidget
{
  Q_OBJECT

public:
  PSTLaboWidget(XWTikzGraphic * graphicA,QWidget * parent = 0);

protected:
  QToolButton * createButton(const QString & src,const QString & txt,
                             double scale=0.5,double rotate=0,
                             bool bg=false);

protected:
  XWTikzGraphic * graphic;
};

#endif //XWPSTLABOWIDGET_H
