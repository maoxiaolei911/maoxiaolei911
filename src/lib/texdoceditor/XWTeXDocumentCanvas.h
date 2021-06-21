/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTEXDOCUMENTCANVAS_H
#define XWTEXDOCUMENTCANVAS_H

#include <QWidget>

class XWTeXDocumentCore;

class XWTeXDocumentCanvas : public QWidget
{
  Q_OBJECT

public:
  XWTeXDocumentCanvas(XWTeXDocumentCore * coreA, QWidget * parent = 0);
  virtual ~XWTeXDocumentCanvas();

public:
	QSize sizeHint() const;

protected:
  virtual void paintEvent(QPaintEvent *e);

protected:
  XWTeXDocumentCore * core;
};

#endif //XWTEXDOCUMENTCANVAS_H
