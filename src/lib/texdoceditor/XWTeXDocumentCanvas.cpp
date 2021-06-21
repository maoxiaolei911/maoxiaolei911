/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWTeXDocumentCore.h"
#include "XWTeXDocumentCanvas.h"

XWTeXDocumentCanvas::XWTeXDocumentCanvas(XWTeXDocumentCore * coreA, QWidget * parent)
 : QWidget(parent),
   core(coreA)
{
  setMouseTracking(true);
}

XWTeXDocumentCanvas::~XWTeXDocumentCanvas()
{}

QSize XWTeXDocumentCanvas::sizeHint() const
{
	int w = core->getTotalWidth();
	int h = core->getTotalHeight();
	return QSize(w, h);
}

void XWTeXDocumentCanvas::paintEvent(QPaintEvent *e)
{
  QPainter painter(this);
  core->redraw(&painter, e->rect());
}
