/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QtGui>
#include "XWApplication.h"
#include "XWTeXDocument.h"
#include "XWTeXDocumentCanvas.h"
#include "XWFindTextDialog.h"
#include "XWReplaceDialog.h"
#include "XWTeXDocumentCore.h"

struct TeXDocumentPaperMenuInfo
{
  char * label;
  double width;
  double height;
};

#define XW_TEX_DOC_PAPER_MIN 0
#define XW_TEX_DOC_PAPER_MAX 29

static TeXDocumentPaperMenuInfo docPaperMenuInfo[] = {
  { QT_TRANSLATE_NOOP("TeXDocumentCore", "A0"),2383.94, 3370.40},
  { QT_TRANSLATE_NOOP("TeXDocumentCore", "A1"),1683.78, 2383.94},
  { QT_TRANSLATE_NOOP("TeXDocumentCore", "A2"),1190.55, 1683.78},
  { QT_TRANSLATE_NOOP("TeXDocumentCore", "A3"),841.89,  1190.55},
  { QT_TRANSLATE_NOOP("TeXDocumentCore", "A4"),595.28,  841.89},
  { QT_TRANSLATE_NOOP("TeXDocumentCore", "A5"),419.53,  595.28},
  { QT_TRANSLATE_NOOP("TeXDocumentCore", "A6"),297.64,  419.53},
  { QT_TRANSLATE_NOOP("TeXDocumentCore", "A7"),209.76,  297.64},
  { QT_TRANSLATE_NOOP("TeXDocumentCore", "A8"),147.40,  209.76},
  { QT_TRANSLATE_NOOP("TeXDocumentCore", "A9"),104.88,  147.40},
  { QT_TRANSLATE_NOOP("TeXDocumentCore", "B0"),2834.65, 4008.19},
  { QT_TRANSLATE_NOOP("TeXDocumentCore", "B1"),2004.09, 2834.65},
  { QT_TRANSLATE_NOOP("TeXDocumentCore", "B2"),1417.32, 2004.09},
  { QT_TRANSLATE_NOOP("TeXDocumentCore", "B3"),1000.63, 1417.32},
  { QT_TRANSLATE_NOOP("TeXDocumentCore", "B4"),708.66,  1000.63},
  { QT_TRANSLATE_NOOP("TeXDocumentCore", "B5"),498.90,  708.66},
  { QT_TRANSLATE_NOOP("TeXDocumentCore", "B6"),354.33,  498.90},
  { QT_TRANSLATE_NOOP("TeXDocumentCore", "B7"),249.45,  354.33},
  { QT_TRANSLATE_NOOP("TeXDocumentCore", "B8"),175.75,  249.45},
  { QT_TRANSLATE_NOOP("TeXDocumentCore", "B9"),93.54,  175.75},
  { QT_TRANSLATE_NOOP("TeXDocumentCore", "B10"),87.87,   124.72},
  { QT_TRANSLATE_NOOP("TeXDocumentCore", "C5E"),462.05,  649.13},
  { QT_TRANSLATE_NOOP("TeXDocumentCore", "Comm10E"),297.64,  683.15},
  { QT_TRANSLATE_NOOP("TeXDocumentCore", "DLE"),311.81,  623.62},
  { QT_TRANSLATE_NOOP("TeXDocumentCore", "Executive"),540.00,  720.00},
  { QT_TRANSLATE_NOOP("TeXDocumentCore", "Folio"),595.28,  935.43},
  { QT_TRANSLATE_NOOP("TeXDocumentCore", "Ledger"),1224.00, 792.00},
  { QT_TRANSLATE_NOOP("TeXDocumentCore", "Legal"),612.00,  1008.00},
  { QT_TRANSLATE_NOOP("TeXDocumentCore", "Letter"),612.00,  792.00},
  { QT_TRANSLATE_NOOP("TeXDocumentCore", "Tabloid"),792.00, 1224.00}
};

static TeXDocumentPaperMenuInfo docSlideAspect[] =
{
  { QT_TRANSLATE_NOOP("TeXDocumentCore", "16 : 10"),453.54, 283.46},
  { QT_TRANSLATE_NOOP("TeXDocumentCore", "16 : 9"),452.76, 255.12},
  { QT_TRANSLATE_NOOP("TeXDocumentCore", "14 : 9"),396.85, 255.12},
  { QT_TRANSLATE_NOOP("TeXDocumentCore", "1.41 : 1"),420.95, 297.64},
  { QT_TRANSLATE_NOOP("TeXDocumentCore", "5 : 4"),354.33, 283.46},
  { QT_TRANSLATE_NOOP("TeXDocumentCore", "4 : 3"),362.84, 272.13},
  { QT_TRANSLATE_NOOP("TeXDocumentCore", "3 : 2"),382.68, 255.12}
};

struct TeXDocumentZoomMenuInfo
{
    char *label;
    double zoom;
};

static TeXDocumentZoomMenuInfo docZoomMenuInfo[] = {
    { QT_TRANSLATE_NOOP("TeXDocumentCore", "400%"),       400 },
    { QT_TRANSLATE_NOOP("TeXDocumentCore", "200%"),       200 },
    { QT_TRANSLATE_NOOP("TeXDocumentCore", "150%"),       150 },
    { QT_TRANSLATE_NOOP("TeXDocumentCore", "100%"),       100 },
    { QT_TRANSLATE_NOOP("TeXDocumentCore", "50%"),        50 },
    { QT_TRANSLATE_NOOP("TeXDocumentCore", "25%"),        25 },
    { QT_TRANSLATE_NOOP("TeXDocumentCore", "12.5%"),      12.5 },
    { QT_TRANSLATE_NOOP("TeXDocumentCore", "8.33%"),      8.33 },
    { QT_TRANSLATE_NOOP("TeXDocumentCore", "fit page"),  XW_TEX_DOC_ZOOM_PAGE },
    { QT_TRANSLATE_NOOP("TeXDocumentCore", "fit width"), XW_TEX_DOC_ZOOM_WIDTH }
};

struct TeXDocumentFontSizeMenuInfo
{
  char * label;
  int size;
};

static TeXDocumentFontSizeMenuInfo docFontSizeMenuInfo[] = {
    { QT_TRANSLATE_NOOP("TeXDocumentCore", "10pt"),      10 },
    { QT_TRANSLATE_NOOP("TeXDocumentCore", "11pt"),  11 },
    { QT_TRANSLATE_NOOP("TeXDocumentCore", "12pt"), 12 }
};


#define XW_TEX_DOC_MIX_ZOOM_IDX   0
#define XW_TEX_DOC_MAX_ZOOM_IDX   7
#define XW_TEX_DOC_ZOOM_PAGE_IDX  8
#define XW_TEX_DOC_ZOOM_WIDTH_IDX 9

XWTeXDocumentCoreMark::XWTeXDocumentCoreMark()
{
  color = Qt::red;
}

XWTeXDocumentCoreMark::~XWTeXDocumentCoreMark()
{
  while (!points.isEmpty())
    delete points.takeFirst();
}

void XWTeXDocumentCoreMark::draw(QPainter * painter)
{
  if (points.size() <= 0)
		return ;

	QPen pen = painter->pen();
	pen.setWidth(4);
	pen.setCapStyle(Qt::RoundCap);
	pen.setJoinStyle(Qt::RoundJoin);
	pen.setColor(color);
	painter->setPen(pen);
	int i = 0;
	QPoint * s = points[i];
	if (points.size() == 1)
	{
		painter->drawPoint(*s);
		return ;
	}

	i++;
	while (i < points.size())
	{
		QPoint * e = points[i];
		painter->drawLine(*s, *e);
		s = e;
		i++;
	}
}

void XWTeXDocumentCoreMark::markTo(const QPoint & p)
{
  QPoint * pts = new QPoint(p.x(), p.y());
	if (pts)
		points << pts;
}

void XWTeXDocumentCoreMark::scale(double sx, double sy)
{
  for (int i = 0; i < points.size(); i++)
	{
		QPoint * p = points[i];
		p->setX(qRound((double)(p->x()) * sx));
		p->setY(qRound((double)(p->y())) * sy);
	}
}

XWTeXDocumentCoreMarks::XWTeXDocumentCoreMarks()
 :curMark(0)
{}

XWTeXDocumentCoreMarks::~XWTeXDocumentCoreMarks()
{
  while (!marks.isEmpty())
    delete marks.takeFirst();
}

void XWTeXDocumentCoreMarks::draw(QPainter * painter)
{
  for (int i = 0; i < marks.size(); i++)
	{
		XWTeXDocumentCoreMark * m = marks[i];
		m->draw(painter);
	}
}

void XWTeXDocumentCoreMarks::endMark()
{
  curMark = 0;
}

void XWTeXDocumentCoreMarks::markTo(const QPoint & p)
{
  if (!curMark)
	{
		curMark = new XWTeXDocumentCoreMark;
		marks << curMark;
	}

	curMark->markTo(p);
}

void XWTeXDocumentCoreMarks::scale(double sx, double sy)
{
  for (int i = 0; i < marks.size(); i++)
	{
		XWTeXDocumentCoreMark * m = marks[i];
		m->scale(sx, sy);
	}
}

void XWTeXDocumentCoreMarks::setMarkColor(const QColor & color)
{
  curMark = new XWTeXDocumentCoreMark;
	marks << curMark;

	curMark->color = color;
}

XWTeXDocumentCorePage::XWTeXDocumentCorePage(XWTeXDocument * docA)
 : doc(docA),
   pageNo(0),
   xDest(0),
   yDest(0),
   w(0),
   h(0)
{}

XWTeXDocumentCorePage::~XWTeXDocumentCorePage()
{}

void XWTeXDocumentCorePage::draw(QPainter * painter, const QColor & pagecolor, const QRectF & rect)
{
  doc->drawPage(pageNo,painter,pagecolor,rect);
}

bool XWTeXDocumentCorePage::hitTest(const QPointF & p)
{
  return doc->hitTest(pageNo, p);
}

bool XWTeXDocumentCorePage::intersects(const QRect & rect)
{
  QRect r(xDest, yDest, w, h);
	return rect.intersects(r);
}

void XWTeXDocumentCorePage::moveToFirstRow()
{
  doc->moveToFirstRow(pageNo);
}

void XWTeXDocumentCorePage::moveToLastRow()
{
  doc->moveToLastRow(pageNo);
}

bool XWTeXDocumentCorePage::moveToNextChar(bool & m)
{
  return doc->moveToNextChar(pageNo, m);
}

bool XWTeXDocumentCorePage::moveToNextRow()
{
  return doc->moveToNextRow(pageNo);
}

bool XWTeXDocumentCorePage::moveToNextWord(bool & m)
{
  return doc->moveToNextWord(pageNo, m);
}

void XWTeXDocumentCorePage::moveToPageEnd()
{
  doc->moveToPageEnd(pageNo);
}

void XWTeXDocumentCorePage::moveToPageStart()
{
  doc->moveToPageStart(pageNo);
}

bool XWTeXDocumentCorePage::moveToPreviousChar(bool & m)
{
  return doc->moveToPreviousChar(pageNo, m);
}

bool XWTeXDocumentCorePage::moveToPreviousRow()
{
  return doc->moveToPreviousRow(pageNo);
}

bool XWTeXDocumentCorePage::moveToPreviousWord(bool & m)
{
  return doc->moveToPreviousWord(pageNo, m);
}

void XWTeXDocumentCorePage::moveToRowEnd()
{
  doc->moveToRowEnd(pageNo);
}

void XWTeXDocumentCorePage::moveToRowStart()
{
  doc->moveToRowStart(pageNo);
}

void XWTeXDocumentCorePage::selectFirstRow(bool all,
                     double & minX,
                     double & minY,
                     double & maxX,
                     double & maxY)
{
  doc->selectFirstRow(pageNo, all, minX,minY,maxX,maxY);
}

void XWTeXDocumentCorePage::selectLastRow(bool all,
                        double & minX,
                        double & minY,
                        double & maxX,
                        double & maxY)
{
  doc->selectLastRow(pageNo, all, minX,minY,maxX,maxY);
}

bool XWTeXDocumentCorePage::selectNextChar(double & minX,
                        double & minY,
                        double & maxX,
                        double & maxY)
{
  return doc->selectNextChar(pageNo, minX,minY,maxX,maxY);
}

bool XWTeXDocumentCorePage::selectNextRow(double & minX,
                double & minY,
                double & maxX,
                double & maxY)
{
  return doc->selectNextRow(pageNo,minX,minY,maxX,maxY);
}

bool XWTeXDocumentCorePage::selectNextWord(double & minX,
                       double & minY,
                       double & maxX,
                       double & maxY)
{
  return doc->selectNextWord(pageNo,minX,minY,maxX,maxY);
}

void XWTeXDocumentCorePage::selectPage(double & minX, double & minY, double & maxX, double & maxY)
{
  doc->selectPage(pageNo,minX,minY,maxX,maxY);
}

bool XWTeXDocumentCorePage::selectPreviousChar(double & minX,
                                          double & minY,
                                          double & maxX,
                                          double & maxY)
{
  return doc->selectPreviousChar(pageNo,minX,minY,maxX,maxY);
}

bool XWTeXDocumentCorePage::selectPreviousRow(double & minX,
                          double & minY,
                          double & maxX,
                          double & maxY)
{
  return doc->selectPreviousRow(pageNo,minX,minY,maxX,maxY);
}

bool XWTeXDocumentCorePage::selectPreviousWord(double & minX,
                            double & minY,
                            double & maxX,
                            double & maxY)
{
  return doc->selectPreviousWord(pageNo,minX,minY,maxX,maxY);
}

void XWTeXDocumentCorePage::selectRow(double & minX,
                         double & minY,
                         double & maxX,
                         double & maxY)
{
  doc->selectRow(pageNo,minX,minY,maxX,maxY);
}

void XWTeXDocumentCorePage::selectRowEnd(double & minX,
                                   double & minY,
                                   double & maxX,
                                   double & maxY)
{
  doc->selectRowEnd(pageNo,minX,minY,maxX,maxY);
}

void XWTeXDocumentCorePage::selectRowStart(double & minX,
                            double & minY,
                            double & maxX,
                            double & maxY)
{
  doc->selectRowStart(pageNo,minX,minY,maxX,maxY);
}

bool XWTeXDocumentCorePage::selectWord(double & minX,
                                   double & minY,
                                   double & maxX,
                                   double & maxY)
{
  return doc->selectWord(pageNo,minX,minY,maxX,maxY);
}

void XWTeXDocumentCorePage::setSelected(const QRectF & rect)
{
  doc->setSelected(pageNo,rect);
}

XWTeXDocumentCore::XWTeXDocumentCore(XWTeXDocument * docA,QWidget * parent)
 : QScrollArea(parent),
   doc(docA)
{
  setAttribute(Qt::WA_InputMethodEnabled);
  setBackgroundRole(QPalette::NoRole);
	setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	setWidgetResizable(false);
	horizontalScrollBar()->setSingleStep(16);
	verticalScrollBar()->setSingleStep(16);

  canvas = new XWTeXDocumentCanvas(this);
  setWidget(canvas);

  zoomIdx = XW_TEX_DOC_ZOOM_PAGE_IDX;

  direction = TEX_DOC_WD_TLT;
  continuousMode = true;
  marking = false;
	zoomFactor = XW_TEX_DOC_ZOOM_PAGE;

	totalDocW = 0;
	totalDocH = 0;
	topPage = 0;
  xScale = 1.0;
  yScale = 1.0;

  mightStartDrag = false;
  dragging = false;

  ulx = 0;
  uly = 0;
  lrx = 0;
  lry = 0;
  selectPage = 0;
	selectULX = 0;
	selectULY = 0;
	selectLRX = 0;
	selectLRY = 0;
	lastDragLeft = 0;
	lastDragTop = 0;

  cursorOn = false;
  cursorMinX = 0;
  cursorMinY = 0;
  cursorMaxX = 0;
  cursorMaxY = 0;

  markColor = Qt::red;

  paperIndex = 4;
  fontIndex = 0;

	wheelDelta = 0;
  isEnableChange = true;

  findDialog = new XWFindTextDialog(this);
  replaceDialog = new XWReplaceDialog(this);

  setDisplay();

  connect(doc, SIGNAL(cursorChanged(int, double, double, double,double)),
          this, SLOT(updateCursor(int, double, double, double,double)));
  connect(doc, SIGNAL(modificationChanged(bool)), this, SLOT(setFileName(bool)));
  connect(doc, SIGNAL(pagesChanged()), this, SLOT(updatePages()));
  connect(doc, SIGNAL(setSlide()), this, SLOT(setSlideIndex()));
  connect(&cursorBlinkTimer, SIGNAL(timeout()), this, SLOT(repaintCursor()));

  connect(findDialog, SIGNAL(findClicked()), this, SLOT(find()));
	connect(findDialog, SIGNAL(editingFinished()), this, SLOT(find()));
	connect(replaceDialog, SIGNAL(replaceClicked()), this, SLOT(replace()));
}

XWTeXDocumentCore::~XWTeXDocumentCore()
{
  clearPages();
  clearPageMarks();

  if (doc)
  {
    delete doc;
    doc = 0;
  }
}

void XWTeXDocumentCore::addMarkingActions(QMenu * menu)
{
  QList<QColor> colors;
	colors << Qt::red << Qt::green << Qt::blue << Qt::yellow;
	QStringList names;
	names << tr("red") << tr("green") << tr("blue") << tr("yellow");
	QActionGroup * grp = new QActionGroup(menu);
	grp->setExclusive(true);
	for (int i = 0; i < colors.count(); ++i)
	{
		QAction *action = new QAction(names.at(i), this);
		action->setData(colors.at(i));
		action->setIcon(createColorIcon(colors.at(i)));
		grp->addAction(action);
		menu->addAction(action);
	}

	connect(grp, SIGNAL(triggered(QAction*)), this, SLOT(changeMarkColor(QAction*)));
}

bool XWTeXDocumentCore::cvtWindowToDev(int xw, int yw, int & pg, int & xd, int & yd)
{
  QHash<int, XWTeXDocumentCorePage*>::iterator i = pages.begin();
  QPoint p = canvas->pos();
  xw = xw - p.x();
  yw = yw - p.y();
  while (i != pages.end())
	{
		XWTeXDocumentCorePage * page = i.value();
		if (page->contains(xw, yw))
		{
			pg = i.key();
			xd = xw - page->xDest;
			yd = yw - page->yDest;
			return true;
		}

		i++;
	}

   pg = 0;
   xd = yd = 0;
   return false;
}

void XWTeXDocumentCore::endMark()
{
  QHash<int, XWTeXDocumentCoreMarks*>::iterator i = pageMarks.begin();
  while (i != pageMarks.end())
	{
		XWTeXDocumentCoreMarks * m = i.value();
		m->endMark();
		i++;
	}
}

QString XWTeXDocumentCore::getFileName()
{
  return doc->getFileName();
}

QString XWTeXDocumentCore::getInputCodecName()
{
  return doc->getInputCodecName();
}

int XWTeXDocumentCore::getNumPages()
{
  return doc->getNumPages();
}

QString XWTeXDocumentCore::getOutputCodecName()
{
  return doc->getOutputCodecName();
}

QStringList XWTeXDocumentCore::getPapers()
{
  QStringList ret;
	for (int i = XW_TEX_DOC_PAPER_MIN; i <= XW_TEX_DOC_PAPER_MAX; i++)
		ret << tr(docPaperMenuInfo[i].label);

	return ret;
}

QStringList XWTeXDocumentCore::getSlideAspect()
{
  QStringList ret;
	for (int i = 0; i < 6; i++)
		ret << tr(docSlideAspect[i].label);

	return ret;
}

QStringList XWTeXDocumentCore::getFontSizes()
{
  QStringList ret;
	for (int i = 0; i < 3; i++)
		ret << tr(docFontSizeMenuInfo[i].label);

	return ret;
}

QStringList XWTeXDocumentCore::getZooms()
{
  QStringList ret;
	for (int i = XW_TEX_DOC_MIX_ZOOM_IDX; i <= XW_TEX_DOC_ZOOM_WIDTH_IDX; i++)
		ret << tr(docZoomMenuInfo[i].label);

	return ret;
}

void XWTeXDocumentCore::loadFile(const QString & filename)
{
  doc->open(filename);
  setWindowTitle(filename);
  displayPage(1);
}

void XWTeXDocumentCore::markTo(const QPoint & p)
{
  QHash<int, XWTeXDocumentCorePage*>::iterator i = pages.begin();
	XWTeXDocumentCorePage * page = 0;
  QPoint wp = canvas->pos();
  QPoint dp(p.x() - wp.x(), p.y() - wp.y());
  while (i != pages.end())
	{
		page = i.value();
		if (page->contains(dp))
			break;

		i++;
		page = 0;
	}

	if (page)
	{
		XWTeXDocumentCoreMarks* m = 0;
		if (pageMarks.contains(page->pageNo))
			m = pageMarks[page->pageNo];
		else
		{
			m = new XWTeXDocumentCoreMarks;
			m->setMarkColor(markColor);
			pageMarks[page->pageNo] = m;
		}
		m->markTo(dp);
	}

	lastPoint = p;
	canvas->update();
}

void XWTeXDocumentCore::moveSelection(int pg, int x, int y)
{
	if (pg != selectPage)
     return;

  double newSelectULX, newSelectULY, newSelectLRX, newSelectLRY;
  QPoint p = canvas->pos();
  if (lastDragLeft)
  {
     if ((x - p.x()) < selectLRX)
     {
         newSelectULX = x - p.x();
         newSelectLRX = selectLRX;
     }
     else
     {
         newSelectULX = selectLRX;
         newSelectLRX = x - p.x();
         lastDragLeft = false;
     }
  }
  else
  {
     if ((x - p.x()) > selectULX)
     {
         newSelectULX = selectULX;
         newSelectLRX = x - p.x();
     }
     else
     {
         newSelectULX = x - p.x();
         newSelectLRX = selectULX;
         lastDragLeft = true;
     }
  }

  if (lastDragTop)
  {
     if ((y - p.y()) < selectLRY)
     {
        newSelectULY = y - p.y();
        newSelectLRY = selectLRY;
     }
     else
     {
        newSelectULY = selectLRY;
        newSelectLRY = y - p.y();
        lastDragTop = false;
     }
  }
  else
  {
     if ((y - p.y()) > selectULY)
     {
         newSelectULY = selectULY;
         newSelectLRY = y - p.y();
     }
     else
     {
         newSelectULY = y - p.y();
         newSelectLRY = selectULY;
         lastDragTop = true;
     }
  }

  selectPage = pg;
  selectULX = newSelectULX;
  selectULY = newSelectULY;
  selectLRX = newSelectLRX;
  selectLRY = newSelectLRY;
  if (newSelectULX != newSelectLRX && newSelectULY != newSelectLRY)
  {
    XWTeXDocumentCorePage * page = getPage(selectPage);
    double xx = (newSelectULX - page->xDest) / xScale;
    double yy = (newSelectULY - page->yDest) / xScale;
    double w = (newSelectLRX - newSelectULX) / xScale;
    double h = (newSelectLRY - newSelectULY) / yScale;
    QRectF rect(xx,yy,w,h);
    page->setSelected(rect);
  }
  else
    doc->resetSelect();

  canvas->update();
}

void XWTeXDocumentCore::redraw(QPainter * painter, const QRect & r)
{
  if (getNumPages() == 0)
		return ;

	QScrollBar * hbar = horizontalScrollBar();
  QScrollBar * vbar = verticalScrollBar();
  int x = hbar->value();
  int y = vbar->value();
	int j = topPage;
	XWTeXDocumentCorePage* page = 0;
	QRect vr(x, y, viewport()->width(), viewport()->height());
  if (continuousMode)
	{
    switch (direction)
    {
      case TEX_DOC_WD_RTT:
        j = getNumPages() - (x / (((int)(doc->getPageWidth() * yScale) + 0.5) + XW_TEX_DOC_CM_PAGE_SPACING) + 1);
        break;

      case TEX_DOC_WD_LTL:
        j = x / (((int)(doc->getPageWidth() * yScale) + 0.5) + XW_TEX_DOC_CM_PAGE_SPACING) + 1;
        break;

      default:
        j = y / (((int)(doc->getPageHeight() * yScale) + 0.5) + XW_TEX_DOC_CM_PAGE_SPACING) + 1;
        break;
    }

		for (; j <= getNumPages(); j++)
		{
			page = getPage(j);
			if (page->intersects(vr))
				drawPage(painter, page, r);
			else
				break;
		}
	}
	else
	{
    if (topPage < getNumPages())
    {
      page = getPage(topPage + 1);
      if (page->intersects(vr))
    		drawPage(painter, page, r);
    }
    else if (topPage > 1)
    {
      page = getPage(topPage - 1);
      if (page->intersects(vr))
    		drawPage(painter, page, r);
    }

  	page = getPage(topPage);
    if (page->intersects(vr))
  	  drawPage(painter, page, r);
	}
}

void XWTeXDocumentCore::saveToPic(const QString & outname,
                         const QString & fmt)
{
  doc->drawPic(outname,fmt,xScale, yScale,pageColor);
}

void XWTeXDocumentCore::setInputCodec(const QString & name)
{
  doc->setInputCodec(name);
  ensureCursorVisible();
}

void XWTeXDocumentCore::setOutputCodec(const QString & name)
{
  doc->setOutputCodec(name);
}

void XWTeXDocumentCore::back()
{
  doc->back();
  ensureCursorVisible();
}

void XWTeXDocumentCore::copy()
{
  doc->copy();
}

void XWTeXDocumentCore::cut()
{
  doc->cut();
  ensureCursorVisible();
}

void XWTeXDocumentCore::del()
{
  doc->del();
  ensureCursorVisible();
}

void XWTeXDocumentCore::changeMarkColor(QAction* a)
{
  if (!a)
     return ;

	QColor color = qVariantValue<QColor>(a->data());
	setMarkColor(color);
}

void XWTeXDocumentCore::clearMark()
{
  if (pageMarks.contains(selectPage - 1))
  {
  	XWTeXDocumentCoreMarks* m = pageMarks.take(selectPage - 1);
  	delete m;
  }
}

void XWTeXDocumentCore::displayPage(int pg)
{
  displayPage(pg, false);
}

void XWTeXDocumentCore::displayPage(int pg, bool scrollToTop)
{
  if (pg <= 0 || pg > getNumPages())
     return ;

	lastDragLeft = lastDragTop = true;
	int topPageA = pg;
	XWTeXDocumentCorePage * page = getPage(topPageA);
	QScrollBar * hbar = horizontalScrollBar();
  QScrollBar * vbar = verticalScrollBar();
	topPage = topPageA;
  QPoint p = canvas->pos();
  switch (direction)
  {
    case TEX_DOC_WD_TLT:
      if (scrollToTop)
        scrollTo(hbar->value(), page->yDest);
      else
        scrollTo(page->xDest + p.x(), page->yDest + p.y());
      break;

    case TEX_DOC_WD_RTT:
      if (scrollToTop)
        scrollTo(page->xDest + page->w + p.x(), vbar->value());
      else
        scrollTo(page->xDest + page->w + p.x(), page->yDest + p.y());
      break;

    case TEX_DOC_WD_LTL:
      if (scrollToTop)
        scrollTo(page->xDest + p.x(), vbar->value());
      else
        scrollTo(page->xDest + p.x(), page->yDest + p.y());
      break;

    default:
      if (scrollToTop)
        scrollTo(hbar->value(), page->yDest + p.y());
      else
        scrollTo(page->xDest + page->w + p.x(), page->yDest + p.y());
      break;
  }
}

void XWTeXDocumentCore::ensureCursorVisible()
{
  lastDragLeft = lastDragTop = true;
  topPage = selectPage;
  ensureVisible(cursorMinX,cursorMinY);
}

void XWTeXDocumentCore::find()
{
  QString str = findDialog->getText();
	if (str.isEmpty())
	  return ;

	bool casematch = findDialog->isCaseSensitive();
	bool wholewords = findDialog->isWholeWords();
	bool regexpmatch = findDialog->isRegexpMatch();
	if (findDialog->isAll())
	  doc->findAll(str,casematch,wholewords,regexpmatch);
	else
	{
		doc->setFindCondition(str,casematch,wholewords,regexpmatch);
	  doc->findNext();
	}
}

void XWTeXDocumentCore::findNext()
{
  QString str = findDialog->getText();
	if (str.isEmpty())
	  return ;

  bool casematch = findDialog->isCaseSensitive();
	bool wholewords = findDialog->isWholeWords();
	bool regexpmatch = findDialog->isRegexpMatch();
  doc->setFindCondition(str,casematch,wholewords,regexpmatch);
	doc->findNext();
}

void XWTeXDocumentCore::fitToPage()
{
	if (zoomIdx != XW_TEX_DOC_ZOOM_PAGE_IDX)
		zoom(XW_TEX_DOC_ZOOM_PAGE_IDX);
}

void XWTeXDocumentCore::fitToWidth()
{
	if (zoomIdx != XW_TEX_DOC_ZOOM_WIDTH_IDX)
		zoom(XW_TEX_DOC_ZOOM_WIDTH_IDX);
}

void XWTeXDocumentCore::newParagraph()
{
  doc->newParagraph();
  ensureCursorVisible();
}

void XWTeXDocumentCore::paste()
{
  doc->paste();
  ensureCursorVisible();
}

void XWTeXDocumentCore::redo()
{
  doc->redo();
  ensureCursorVisible();
}

void XWTeXDocumentCore::repaintCursor()
{
  cursorOn = !cursorOn;
  canvas->update(cursorMinX,cursorMinY,cursorMaxX - cursorMinX, cursorMaxY - cursorMinY);
}

void XWTeXDocumentCore::replace()
{
  QString str = replaceDialog->getText();
	if (str.isEmpty())
	  return ;

  QString bystr = replaceDialog->getByText();
	bool casematch = findDialog->isCaseSensitive();
	bool wholewords = findDialog->isWholeWords();
	bool regexpmatch = findDialog->isRegexpMatch();
	if (findDialog->isAll())
	  doc->replaceAll(str,bystr, casematch,wholewords,regexpmatch);
	else
	{
		doc->setReplaceCondition(str,bystr, casematch,wholewords,regexpmatch);
	  doc->replaceNext();
	}
}

void XWTeXDocumentCore::replaceNext()
{
  QString str = replaceDialog->getText();
	if (str.isEmpty())
	  return ;

  QString bystr = replaceDialog->getByText();
	bool casematch = findDialog->isCaseSensitive();
	bool wholewords = findDialog->isWholeWords();
	bool regexpmatch = findDialog->isRegexpMatch();
  doc->setReplaceCondition(str,bystr, casematch,wholewords,regexpmatch);
  doc->replaceNext();
}

void XWTeXDocumentCore::save()
{
  doc->save();
}

void XWTeXDocumentCore::saveAs()
{
  QString path = doc->getPath();
  QString filename = QFileDialog::getSaveFileName(this, tr("Save File"), path, ("*.tex"));
  if (!filename.isEmpty())
    doc->saveAs(filename);
}

void XWTeXDocumentCore::scrollTo(int x, int y)
{
  if (x > (totalDocW - viewport()->width()))
  	x = totalDocW - viewport()->width();

  if (x < 0)
  	x = 0;

  if (y > totalDocH - viewport()->height())
  	y = totalDocH - viewport()->height();

  if (y < 0)
  	y = 0;

  QScrollBar * hbar = horizontalScrollBar();
  QScrollBar * vbar = verticalScrollBar();
  hbar->setValue(x);
  vbar->setValue(y);
  canvas->update();
}

void XWTeXDocumentCore::setBlinkingCursorEnabled(bool enable)
{
  cursorOn = true;
  repaintCursor();

  if (enable && QApplication::cursorFlashTime() > 0)
    cursorBlinkTimer.start(QApplication::cursorFlashTime() / 2);
  else
    cursorBlinkTimer.stop();

  cursorOn = enable;
}

void XWTeXDocumentCore::setContinuousMode(bool cm)
{
  if (continuousMode != cm)
	{
   	continuousMode = cm;
    updatePages();
    ensureCursorVisible();
  }
}

void XWTeXDocumentCore::setDirection(int d)
{
  if (direction != d)
  {
    direction = d;
    doc->setDirection(direction);
    ensureCursorVisible();
  }
}

void XWTeXDocumentCore::setDisplay()
{
  QSettings * settings = xwApp->getSettings();
  QString docstyle = settings->value("CurrentDocumentStyle").toString();
  doc->setStyle(docstyle);
  settings->beginGroup("DocumentStyles");
  if (!docstyle.isEmpty())
  {
    settings->beginGroup(docstyle);
    QVariant var = settings->value("PageColor");
    pageColor = var.value<QColor>();

    settings->endGroup();
  }
  settings->endGroup();
  settings->endGroup();
}

void XWTeXDocumentCore::setFileName(bool e)
{
  QString filename = doc->getFileName();
  setWindowModified(e);
  if (e)
  {
    QString title = QString("%1[*]").arg(filename);
  	setWindowTitle(title);
  }
	else
    setWindowTitle(filename);
}

void XWTeXDocumentCore::setFontSize(int idx)
{
  if (fontIndex != idx)
  {
    fontIndex = idx;
    doc->setFontSize(docFontSizeMenuInfo[idx].size);
    ensureCursorVisible();
  }
}

void XWTeXDocumentCore::setLineSkip(double s)
{
  doc->setLineSkip(s);
  ensureCursorVisible();
}

void XWTeXDocumentCore::setMarkColor(const QColor & color)
{
	markColor = color;
	if (marking && selectPage > 0)
	{
		XWTeXDocumentCoreMarks * m = 0;
		if (pageMarks.contains(selectPage - 1))
			m = pageMarks[selectPage - 1];
		else
			m = new XWTeXDocumentCoreMarks;

		m->setMarkColor(markColor);
	}
}

void XWTeXDocumentCore::setPaper(int idx)
{
  if (idx == paperIndex)
    return ;

  paperIndex = idx;
  doc->setPaper(docPaperMenuInfo[idx].width, docPaperMenuInfo[idx].height);
  ensureCursorVisible();
}

void XWTeXDocumentCore::setSlideAspect(int idx)
{
  if (idx == paperIndex)
    return ;

  paperIndex = idx;
  doc->setPaper(docSlideAspect[idx].width, docSlideAspect[idx].height);
  ensureCursorVisible();
}

void XWTeXDocumentCore::setSlideIndex()
{
  paperIndex = 5;
}

void XWTeXDocumentCore::showFindDialog()
{
  findDialog->hideAll(false);
  if (doc->hasSelect())
  {
    QString txt = doc->getCurrentSelected();
    findDialog->setText(txt);
  }
  findDialog->show();
}

void XWTeXDocumentCore::showReplaceDialog()
{
  replaceDialog->hideAll(false);
  if (doc->hasSelect())
  {
    QString txt = doc->getCurrentSelected();
    replaceDialog->setText(txt);
  }
  replaceDialog->show();
}

void XWTeXDocumentCore::undo()
{
  doc->undo();
  ensureCursorVisible();
}

void XWTeXDocumentCore::updateCursor(int pageno, double minx, double miny, double maxx,double maxy)
{
  cursorOn = true;
  repaintCursor();

  selectPage = pageno;
  XWTeXDocumentCorePage * page = getPage(pageno);
  cursorMinX = (int)(minx * xScale + page->xDest + 0.5);
  cursorMinY = (int)(miny * yScale + page->yDest + 0.5);
  cursorMaxX = (int)(maxx * xScale + page->xDest + 0.5);
  cursorMaxY = (int)(maxy * yScale + page->yDest + 0.5);
  cursorOn = true;
}

void XWTeXDocumentCore::zoom(int idx)
{
	if (idx == zoomIdx)
		return ;

	if (idx >= XW_TEX_DOC_MIX_ZOOM_IDX && idx <= XW_TEX_DOC_MAX_ZOOM_IDX)
		zoomFactor = docZoomMenuInfo[idx].zoom;
	else if (idx == XW_TEX_DOC_ZOOM_PAGE_IDX)
		zoomFactor = XW_TEX_DOC_ZOOM_PAGE;
	else if (idx == XW_TEX_DOC_ZOOM_WIDTH_IDX)
		zoomFactor = XW_TEX_DOC_ZOOM_WIDTH;

	zoomIdx = idx;
  updatePages();
  ensureCursorVisible();
}

void XWTeXDocumentCore::zoomIn()
{
	if (zoomIdx > XW_TEX_DOC_MIX_ZOOM_IDX && zoomIdx <= XW_TEX_DOC_MAX_ZOOM_IDX)
		zoom(zoomIdx - 1);
}

void XWTeXDocumentCore::zoomOut()
{
  if (zoomIdx < XW_TEX_DOC_MAX_ZOOM_IDX)
		zoom(zoomIdx + 1);
}

void XWTeXDocumentCore::changeEvent(QEvent *e)
{
  QScrollArea::changeEvent(e);
  if(e->type() == QEvent::ActivationChange)
  {
    if (!isActiveWindow())
      setBlinkingCursorEnabled(false);
  }
  else if (e->type() == QEvent::EnabledChange)
  {
    e->setAccepted(isEnabled());
    isEnableChange = e->isAccepted();
    if (isEnableChange)
      setBlinkingCursorEnabled(true);
    else
      setBlinkingCursorEnabled(false);
  }
}

void XWTeXDocumentCore::contextMenuEvent(QContextMenuEvent *e)
{
  QPoint p = e->pos();
  int xd, yd, pg;
  if (cvtWindowToDev(p.x(), p.y(), pg, xd, yd))
  {
    double x = xd / xScale;
    double y = yd / yScale;
    XWTeXDocumentCorePage * page = pages[pg];
    QPointF pp(x,y);
    if (page->hitTest(pp))
    {
      QMenu * menu = doc->createContextMenu();
      if (menu)
      {
        if (pageMarks.contains(pg))
        {
          menu->addSeparator();
          QAction * act = menu->addAction(tr("Clear mark"));
          connect(act, SIGNAL(triggered()), this, SLOT(clearMark()));
        }

        menu->setAttribute(Qt::WA_DeleteOnClose);
        menu->exec(p);
      }
    }
  }
}

void XWTeXDocumentCore::dragEnterEvent(QDragEnterEvent *e)
{
  QPoint p = e->pos();
  int xd, yd, pg;
  if (cvtWindowToDev(p.x(), p.y(), pg, xd, yd))
  {
    double x = xd / xScale;
    double y = yd / yScale;
    XWTeXDocumentCorePage * page = getPage(pg);
    QPointF pp(x,y);
    if (!doc->canInsertFromMimeData(e->mimeData(), page->pageNo, pp))
      e->ignore();
    else
      e->acceptProposedAction();
  }
  else
    e->ignore();
}

void XWTeXDocumentCore::dragLeaveEvent(QDragLeaveEvent *e)
{
  repaintCursor();
  e->accept();
}

void XWTeXDocumentCore::dragMoveEvent(QDragMoveEvent *e)
{
  QPoint p = e->pos();
  int xd, yd, pg;
  if (cvtWindowToDev(p.x(), p.y(), pg, xd, yd))
  {
    double x = xd / xScale;
    double y = yd / yScale;
    XWTeXDocumentCorePage * page = getPage(pg);
    QPointF pp(x,y);
    if (!doc->canInsertFromMimeData(e->mimeData(), page->pageNo, pp))
      e->ignore();
    else
    {
      repaintCursor();
      e->acceptProposedAction();
    }
  }
  else
    e->ignore();
}

void XWTeXDocumentCore::dropEvent(QDropEvent *e)
{
  QPoint p = e->pos();
  int xd, yd, pg;
  if (cvtWindowToDev(p.x(), p.y(), pg, xd, yd))
  {
    double x = xd / xScale;
    double y = yd / yScale;
    XWTeXDocumentCorePage * page = getPage(pg);
    QPointF pp(x,y);
    if (!doc->canInsertFromMimeData(e->mimeData(), page->pageNo, pp))
      e->ignore();
    else
    {
      doc->insertFromMimeData(e->mimeData(), page->pageNo, pp);
      ensureCursorVisible();
      e->acceptProposedAction();
    }
  }
  else
    e->ignore();
}

void XWTeXDocumentCore::focusInEvent(QFocusEvent *e)
{
  QScrollArea::focusInEvent(e);
  if (e->gotFocus())
    setBlinkingCursorEnabled(true);
  else
    setBlinkingCursorEnabled(false);
}

void XWTeXDocumentCore::focusOutEvent(QFocusEvent *e)
{
  QScrollArea::focusOutEvent(e);
  setBlinkingCursorEnabled(false);
}

void XWTeXDocumentCore::inputMethodEvent(QInputMethodEvent *e)
{
  if (!e->commitString().isEmpty())
  {
    doc->insertText(e->commitString());
    ensureCursorVisible();
  }
}

QVariant XWTeXDocumentCore::inputMethodQuery(Qt::InputMethodQuery property) const
{
  switch(property)
  {
    case Qt::ImMicroFocus:
      {
        QRect r(cursorMinX,cursorMinY,cursorMaxX - cursorMinX, cursorMaxY - cursorMinY);
        return r;
      }
      break;

    case Qt::ImFont:
      return QVariant(doc->getCurrentFont());

    case Qt::ImCursorPosition:
      return QVariant(doc->getHitPos());

    case Qt::ImSurroundingText:
      return QVariant(doc->getCurrentBlockText());

    case Qt::ImCurrentSelection:
      return QVariant(doc->getCurrentSelected());

    case Qt::ImMaximumTextLength:
        return QVariant(); // No limit.

    case Qt::ImAnchorPosition:
        return QVariant(doc->getCurrentStart());

    default:
        return QVariant();
  }
}

void XWTeXDocumentCore::keyPressEvent(QKeyEvent *e)
{
  if (e == QKeySequence::AddTab)
    e->ignore();
  else if (e == QKeySequence::Back)
  {
    back();
    e->accept();
  }
  else if (e == QKeySequence::Bold)
    e->ignore();
  else if (e == QKeySequence::Close)
    e->ignore();
  else if (e == QKeySequence::Copy)
  {
    copy();
    e->accept();
  }
  else if (e == QKeySequence::Cut)
  {
    cut();
    e->accept();
  }
  else if (e == QKeySequence::Delete)
  {
    del();
    e->accept();
  }
  else if (e == QKeySequence::DeleteEndOfLine)
    e->ignore();
  else if (e == QKeySequence::DeleteEndOfWord)
    e->ignore();
  else if (e == QKeySequence::DeleteStartOfWord)
    e->ignore();
  else if (e == QKeySequence::Find)
    e->ignore();
  else if (e == QKeySequence::FindNext)
  {
    findNext();
    e->accept();
  }    
  else if (e == QKeySequence::FindPrevious)
    e->ignore();
  else if (e == QKeySequence::Forward)
    e->ignore();
  else if (e == QKeySequence::HelpContents)
    e->ignore();
  else if (e == QKeySequence::InsertLineSeparator)
    e->ignore();
  else if (e == QKeySequence::InsertParagraphSeparator)
  {
    newParagraph();
    e->accept();
  }
  else if (e == QKeySequence::Italic)
    e->ignore();
  else if (e == QKeySequence::MoveToEndOfBlock)
    e->ignore();
  else if (e == QKeySequence::MoveToEndOfDocument)
  {
    moveToDocEnd();
    e->accept();
  }
  else if (e == QKeySequence::MoveToEndOfLine)
  {
    moveToLineEnd();
    e->accept();
  }
  else if (e == QKeySequence::MoveToNextChar)
  {
    moveToNextChar();
    e->accept();
  }
  else if (e == QKeySequence::MoveToNextLine)
  {
    moveToNextLine();
    e->accept();
  }
  else if (e == QKeySequence::MoveToNextPage)
  {
    moveToNextPage();
    e->accept();
  }
  else if (e == QKeySequence::MoveToNextWord)
  {
    moveToNextWord();
    e->accept();
  }
  else if (e == QKeySequence::MoveToPreviousChar)
  {
    moveToPreChar();
    e->accept();
  }
  else if (e == QKeySequence::MoveToPreviousLine)
  {
    moveToPreLine();
    e->accept();
  }
  else if (e == QKeySequence::MoveToPreviousPage)
  {
    moveToPrePage();
    e->accept();
  }
  else if (e == QKeySequence::MoveToPreviousWord)
  {
    moveToPreWord();
    e->accept();
  }
  else if (e == QKeySequence::MoveToStartOfDocument)
  {
    moveToDocBegin();
    e->accept();
  }
  else if (e == QKeySequence::MoveToStartOfLine)
  {
    moveToLineBegin();
    e->accept();
  }
  else if (e == QKeySequence::New)
    e->ignore();
  else if (e == QKeySequence::NextChild)
    e->ignore();
  else if (e == QKeySequence::Open)
    e->ignore();
  else if (e == QKeySequence::Paste)
  {
    paste();
    e->accept();
  }
  else if (e == QKeySequence::Preferences)
    e->ignore();
  else if (e == QKeySequence::PreviousChild)
    e->ignore();
  else if (e == QKeySequence::Print)
    e->ignore();
  else if (e == QKeySequence::Quit)
    e->ignore();
  else if (e == QKeySequence::Redo)
  {
    redo();
    e->accept();
  }
  else if (e == QKeySequence::Refresh)
    e->ignore();
  else if (e == QKeySequence::Replace)
    e->ignore();
  else if (e == QKeySequence::SaveAs)
    e->ignore();
  else if (e == QKeySequence::Save)
  {
    save();
    e->accept();
  }
  else if (e == QKeySequence::SelectAll)
  {
    selectAll();
    e->accept();
  }
  else if (e == QKeySequence::SelectEndOfBlock)
    e->ignore();
  else if (e == QKeySequence::SelectEndOfDocument)
  {
    selectEndOfDocument();
    e->accept();
  }
  else if (e == QKeySequence::SelectEndOfLine)
  {
    selectEndOfLine();
    e->accept();
  }
  else if (e == QKeySequence::SelectNextChar)
  {
    selectNextChar();
    e->accept();
  }
  else if (e == QKeySequence::SelectNextLine)
  {
    selectNextLine();
    e->accept();
  }
  else if (e == QKeySequence::SelectNextPage)
  {
    selectNextPage();
    e->accept();
  }
  else if (e == QKeySequence::SelectNextWord)
  {
    selectNextWord();
    e->accept();
  }
  else if (e == QKeySequence::SelectPreviousChar)
  {
    selectPreviousChar();
    e->accept();
  }
  else if (e == QKeySequence::SelectPreviousLine)
  {
    selectPreviousLine();
    e->accept();
  }
  else if (e == QKeySequence::SelectPreviousPage)
  {
    selectPreviousPage();
    e->accept();
  }
  else if (e == QKeySequence::SelectPreviousWord)
  {
    selectPreviousWord();
    e->accept();
  }
  else if (e == QKeySequence::SelectStartOfBlock)
    e->ignore();
  else if (e == QKeySequence::SelectStartOfDocument)
  {
    selectStartOfDocument();
    e->accept();
  }
  else if (e == QKeySequence::SelectStartOfLine)
  {
    selectStartOfLine();
    e->accept();
  }
  else if (e == QKeySequence::Underline)
    e->ignore();
  else if (e == QKeySequence::Undo)
  {
    undo();
    e->accept();
  }
  else if (e == QKeySequence::UnknownKey)
    e->ignore();
  else if (e == QKeySequence::WhatsThis)
    e->ignore();
  else if (e == QKeySequence::ZoomIn)
  {
    zoomIn();
    e->accept();
  }
  else if (e == QKeySequence::ZoomOut)
  {
    zoomOut();
    e->accept();
  }
  else
  {
    QString text = e->text();
    if (!text.isEmpty())
    {
      doc->keyInput(text);
      e->accept();
    }
    else
      e->ignore();
  }
}

void XWTeXDocumentCore::mouseDoubleClickEvent(QMouseEvent *e)
{
  if (e->button() != Qt::LeftButton || doc->getNumPages() == 0)
  {
    e->ignore();
    return ;
  }

  mightStartDrag = false;
  dragging = false;
  selectWord();
}

void XWTeXDocumentCore::mouseMoveEvent(QMouseEvent *e)
{
  if (e->button() != Qt::LeftButton || doc->getNumPages() == 0)
  {
    e->ignore();
    return ;
  }

  if (marking)
  {
    markTo(e->pos());
		e->accept();
		return ;
  }

  if (!dragging)
  {
    moveSelection(selectPage, e->x(), e->y());
    e->accept();
		return ;
  }

  if ((e->pos() - dragStartPos).manhattanLength() > QApplication::startDragDistance())
  {
    QMimeData *data = doc->createMimeDataFromSelection();
    QDrag *drag = new QDrag(this);
    drag->setMimeData(data);
    if (drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::MoveAction) == Qt::MoveAction)
      del();
  }

  e->accept();
}

void XWTeXDocumentCore::mousePressEvent(QMouseEvent *e)
{
  if (e->button() != Qt::LeftButton || doc->getNumPages() == 0)
  {
    e->ignore();
    return ;
  }

  int pg, xd, yd;
  if (cvtWindowToDev(e->x(),e->y(), pg, xd, yd))
  {
    double x = xd / xScale;
    double y = yd / yScale;
    QPointF p(x,y);
    XWTeXDocumentCorePage * page = getPage(pg);
    if (page->hitTest(p))
    {
      if (hasSelection())
      {
        dragging = true;
        mightStartDrag = true;
        dragStartPos = e->pos();
      }
      else
      {
        dragging = false;
        selectPage = pg;
        selectULX = xd;
        selectULY = yd;
        selectLRX = xd;
        selectLRY = yd;
      }

      setBlinkingCursorEnabled(true);
    }
    else
      setBlinkingCursorEnabled(false);
  }
  else
    setBlinkingCursorEnabled(false);

  e->accept();
}

void XWTeXDocumentCore::mouseReleaseEvent(QMouseEvent *e)
{
  if (e->button() != Qt::LeftButton || doc->getNumPages() == 0)
  {
    e->ignore();
    return ;
  }

  if (marking)
  {
    endMark();
    e->accept();
    return ;
  }

  if (!dragging)
  {
    e->accept();
    return ;
  }

  doc->resetSelect();

  e->accept();
}

void XWTeXDocumentCore::resizeEvent(QResizeEvent *e)
{
  int topPageA = topPage;
	updatePages();
	displayPage(topPageA);
	topPage = topPageA;

	QScrollArea::resizeEvent(e);
}

void XWTeXDocumentCore::scrollContentsBy(int dx, int dy)
{
  QScrollBar * hbar = horizontalScrollBar();
	QScrollBar * vbar = verticalScrollBar();
  if (!continuousMode)
  {
  	int topPageA = topPage;
  	XWTeXDocumentCorePage * page = 0;
    switch (direction)
    {
      case TEX_DOC_WD_RTT:
      case TEX_DOC_WD_TRT:
        if (hbar->value() >= hbar->maximum() && wheelDelta < 0 && topPageA > 1)
        {
          topPageA--;
          page = getPage(topPageA);
        }
        else if (hbar->value() <= hbar->minimum() && wheelDelta > 0 && topPageA < getNumPages())
        {
          topPageA++;
          page = getPage(topPageA);
        }
        if (page)
          scrollTo(page->xDest + page->w - viewport()->width(), page->yDest);
        break;

      default:
        if (vbar->value() >= vbar->maximum() && wheelDelta < 0 && topPageA < getNumPages())
        {
          topPageA++;
          page = getPage(topPageA);
        }
        else if (vbar->value() <= vbar->minimum() && wheelDelta > 0 && topPageA > 1)
        {
          topPageA--;
          page = getPage(topPageA);
        }
        if (page)
          scrollTo(hbar->value(), page->yDest);
        break;
    }

    topPage = topPageA;
  }
  else
  	scrollTo(hbar->value(), vbar->value());

	QScrollArea::scrollContentsBy(dx, dy);
}

void XWTeXDocumentCore::showEvent(QShowEvent *)
{
  ensureCursorVisible();
}

void XWTeXDocumentCore::timerEvent(QTimerEvent *e)
{
  if (e->timerId() == cursorBlinkTimer.timerId())
  {
    cursorOn = !cursorOn;
    repaintCursor();
  }

  QScrollArea::timerEvent(e);
}

void XWTeXDocumentCore::wheelEvent(QWheelEvent *e)
{
  wheelDelta = e->delta() / 8;
	QAbstractScrollArea::wheelEvent(e);
}

void XWTeXDocumentCore::drawPage(QPainter * painter, XWTeXDocumentCorePage * page, const QRect & r)
{
  QRect pr(page->xDest,page->yDest,page->w,page->h);
  QRect ir = pr.intersected(r);
  painter->fillRect(ir,pageColor);

  painter->save();
  painter->translate(page->xDest, page->yDest);
  painter->scale(xScale,yScale);
  double x = (ir.x() - page->xDest) / xScale;
  double y = (ir.y() - page->yDest) / yScale;
  double w = ir.width() / xScale;
  double h = ir.height() / yScale;

  QRectF rect(x,y,w,h);
  page->draw(painter,pageColor, rect);

  painter->restore();

  if (cursorOn && isEnableChange)
  {
    QRect cr(cursorMinX,cursorMinY,cursorMaxX-cursorMinX,cursorMaxY-cursorMinY);
    if (cr.intersects(r))
    {
      cr = cr.intersected(r);
      QColor c = doc->getCurrentBlockColor();
      painter->fillRect(cr,c);
    }
  }

  if (pageMarks.contains(page->pageNo))
  {
  	XWTeXDocumentCoreMarks * m = pageMarks[page->pageNo];
  	m->draw(painter);
  }
}

XWTeXDocumentCorePage * XWTeXDocumentCore::getPage(int pg)
{
  XWTeXDocumentCorePage * page = 0;
	if (pg <= 0)
		pg = 1;

  if (pg > getNumPages())
    pg = getNumPages();

  if (continuousMode)
  {
    if (pages.contains(pg))
      page = pages[pg];
    else
    {
      page = new XWTeXDocumentCorePage(doc);
      page->pageNo = pg - 1;
      pages[pg] = page;
    }
    page->w = (int)((doc->getPageWidth() * xScale) + 0.5);
    page->h = (int)((doc->getPageHeight() * yScale) + 0.5);

    switch (direction)
    {
      case TEX_DOC_WD_TLT:
      case TEX_DOC_WD_TRT:
        page->xDest = 0;
        page->yDest = (pg - 1) * ((int)((doc->getPageHeight() * yScale) + 0.5) + XW_TEX_DOC_CM_PAGE_SPACING);
        break;

      case TEX_DOC_WD_LTL:
        page->yDest = 0;
        page->xDest = (pg - 1) * ((int)((doc->getPageWidth() * xScale) + 0.5) + XW_TEX_DOC_HM_PAGE_SPACING);
        break;

      default:
        page->yDest = 0;
        page->xDest = (getNumPages() - pg) * ((int)((doc->getPageWidth() * xScale) + 0.5) + XW_TEX_DOC_HM_PAGE_SPACING);
        break;
    }
  }
  else
  {
    if (pages.contains(pg))
      page = pages[pg];
    else
    {
      page = new XWTeXDocumentCorePage(doc);
      page->pageNo = pg - 1;
      pages[pg] = page;

    }

    page->w = (int)((doc->getPageWidth() * xScale) + 0.5);
    page->h = (int)((doc->getPageHeight() * yScale) + 0.5);

    page->yDest = 0;
    switch (direction)
    {
      case TEX_DOC_WD_TLT:
      case TEX_DOC_WD_LTL:
        if (pg <= topPage)
          page->xDest = 0;
        else
          page->xDest = page->w + XW_TEX_DOC_DM_PAGE_SPACING;
        break;

      default:
        if (pg <= topPage)
          page->xDest = page->w + XW_TEX_DOC_DM_PAGE_SPACING;
        else
          page->xDest = 0;
        break;
    }
  }

  return page;
}

void XWTeXDocumentCore::moveToDocBegin()
{
  selectPage = 1;
  XWTeXDocumentCorePage * page = getPage(selectPage);
  page->moveToPageStart();
  ensureCursorVisible();
}

void XWTeXDocumentCore::moveToDocEnd()
{
  selectPage = doc->getNumPages();
  XWTeXDocumentCorePage * page = getPage(selectPage);
  page->moveToPageEnd();
  ensureCursorVisible();
}

void XWTeXDocumentCore::moveToLineBegin()
{
  XWTeXDocumentCorePage * page = getPage(selectPage);
  page->moveToRowStart();
  ensureCursorVisible();
}

void XWTeXDocumentCore::moveToLineEnd()
{
  XWTeXDocumentCorePage * page = getPage(selectPage);
  page->moveToRowEnd();
  ensureCursorVisible();
}

void XWTeXDocumentCore::moveToNextChar()
{
  XWTeXDocumentCorePage * page = getPage(selectPage);
  bool m = false;
  if (!page->moveToNextChar(m))
  {
    if (selectPage < doc->getNumPages())
    {
      selectPage++;
      page = getPage(selectPage);
      if (m)
        page->moveToFirstRow();
      else
        page->moveToNextChar(m);
    }
  }
  ensureCursorVisible();
}

void XWTeXDocumentCore::moveToNextLine()
{
  XWTeXDocumentCorePage * page = getPage(selectPage);
  if (!page->moveToNextRow())
  {
    if (selectPage < doc->getNumPages())
    {
      selectPage++;
      page = getPage(selectPage);
      page->moveToRowStart();
    }
  }
  ensureCursorVisible();
}

void XWTeXDocumentCore::moveToNextPage()
{
  if (selectPage < doc->getNumPages())
  {
    selectPage++;
    XWTeXDocumentCorePage * page = getPage(selectPage);
    page->moveToPageStart();
    ensureCursorVisible();
  }
}

void XWTeXDocumentCore::moveToNextWord()
{
  XWTeXDocumentCorePage * page = getPage(selectPage);
  bool m = false;
  if (!page->moveToNextWord(m))
  {
    if (selectPage < doc->getNumPages())
    {
      selectPage++;
      page = getPage(selectPage);
      if (m)
        page->moveToFirstRow();
      else
        page->moveToNextWord(m);
    }
  }
  ensureCursorVisible();
}

void XWTeXDocumentCore::moveToPreChar()
{
  XWTeXDocumentCorePage * page = getPage(selectPage);
  bool m = false;
  if (!page->moveToPreviousChar(m))
  {
    if (selectPage > 1)
    {
      selectPage--;
      page = getPage(selectPage);
      if (m)
        page->moveToLastRow();
      else
        page->moveToPreviousChar(m);
    }
  }
  ensureCursorVisible();
}

void XWTeXDocumentCore::moveToPreLine()
{
  XWTeXDocumentCorePage * page = getPage(selectPage);
  if (!page->moveToPreviousRow())
  {
    if (selectPage > 1)
    {
      selectPage--;
      page = getPage(selectPage);
      page->moveToRowEnd();
    }
  }
  ensureCursorVisible();
}

void XWTeXDocumentCore::moveToPrePage()
{
  if (selectPage > 1)
  {
    selectPage--;
    XWTeXDocumentCorePage * page = getPage(selectPage);
    page->moveToPageEnd();
    ensureCursorVisible();
  }
}

void XWTeXDocumentCore::moveToPreWord()
{
  XWTeXDocumentCorePage * page = getPage(selectPage);
  bool m = false;
  if (!page->moveToPreviousWord(m))
  {
    if (selectPage > 1)
    {
      selectPage--;
      page = getPage(selectPage);
      if (m)
        page->moveToLastRow();
      else
        page->moveToPreviousWord(m);
    }
  }

  ensureCursorVisible();
}

void XWTeXDocumentCore::selectAll()
{
  doc->selectAll();
  ensureCursorVisible();
}

void XWTeXDocumentCore::selectEndOfLine()
{
  double minx,miny,maxx,maxy;
  XWTeXDocumentCorePage * page = getPage(selectPage);
  page->selectRowEnd(minx,miny,maxx,maxy);
  setSelection(page, minx,miny,maxx,maxy);
  ensureCursorVisible();
}

void XWTeXDocumentCore::selectEndOfDocument()
{
  double minx,miny,maxx,maxy;
  doc->selectEndOfDocument(selectPage - 1, minx,miny,maxx,maxy);
  selectPage = doc->getNumPages();
  XWTeXDocumentCorePage * page = getPage(selectPage);
  setSelection(page, minx,miny,maxx,maxy);
  ensureCursorVisible();
}

void XWTeXDocumentCore::selectNextChar()
{
  double minx,miny,maxx,maxy;
  XWTeXDocumentCorePage * page = getPage(selectPage);
  if (!page->selectNextChar(minx,miny,maxx,maxy))
  {
    if (selectPage < doc->getNumPages())
    {
      selectPage++;
      page = getPage(selectPage);
      page->selectNextChar(minx,miny,maxx,maxy);
    }
  }
  setSelection(page, minx,miny,maxx,maxy);
  ensureCursorVisible();
}

void XWTeXDocumentCore::selectNextLine()
{
  double minx,miny,maxx,maxy;
  XWTeXDocumentCorePage * page = getPage(selectPage);
  if (!page->selectNextRow(minx,miny,maxx,maxy))
  {
    if (selectPage < doc->getNumPages())
    {
      selectPage++;
      page = getPage(selectPage);
      page->selectFirstRow(true, minx,miny,maxx,maxy);
    }
  }
  setSelection(page, minx,miny,maxx,maxy);
  ensureCursorVisible();
}

void XWTeXDocumentCore::selectNextPage()
{
  if (selectPage < doc->getNumPages())
  {
    selectPage++;
    XWTeXDocumentCorePage * page = getPage(selectPage);
    double minx,miny,maxx,maxy;
    page->selectPage(minx,miny,maxx,maxy);
    ensureCursorVisible();
  }
}

void XWTeXDocumentCore::selectNextWord()
{
  double minx,miny,maxx,maxy;
  XWTeXDocumentCorePage * page = getPage(selectPage);
  if (!page->selectNextWord(minx,miny,maxx,maxy))
  {
    if (selectPage < doc->getNumPages())
    {
      selectPage++;
      page = getPage(selectPage);
      page->selectFirstRow(false, minx,miny,maxx,maxy);
    }
  }
  setSelection(page, minx,miny,maxx,maxy);
  ensureCursorVisible();
}

void XWTeXDocumentCore::selectPreviousChar()
{
  double minx,miny,maxx,maxy;
  XWTeXDocumentCorePage * page = getPage(selectPage);
  if (!page->selectPreviousChar(minx,miny,maxx,maxy))
  {
    if (selectPage > 1)
    {
      selectPage--;
      page = getPage(selectPage);
      page->selectPreviousChar(minx,miny,maxx,maxy);
    }
  }
  setSelection(page, minx,miny,maxx,maxy);
  ensureCursorVisible();
}

void XWTeXDocumentCore::selectPreviousLine()
{
  double minx,miny,maxx,maxy;
  XWTeXDocumentCorePage * page = getPage(selectPage);
  if (!page->selectPreviousRow(minx,miny,maxx,maxy))
  {
    if (selectPage > 1)
    {
      selectPage--;
      page = getPage(selectPage);
      page->selectLastRow(true,minx,miny,maxx,maxy);
    }
  }
  setSelection(page, minx,miny,maxx,maxy);
  ensureCursorVisible();
}

void XWTeXDocumentCore::selectPreviousPage()
{
  if (selectPage > 1)
  {
    selectPage--;
    XWTeXDocumentCorePage * page = getPage(selectPage);
    double minx,miny,maxx,maxy;
    page->selectPage(minx,miny,maxx,maxy);
    ensureCursorVisible();
  }
}

void XWTeXDocumentCore::selectPreviousWord()
{
  double minx,miny,maxx,maxy;
  XWTeXDocumentCorePage * page = getPage(selectPage);
  if (!page->selectPreviousWord(minx,miny,maxx,maxy))
  {
    if (selectPage > 1)
    {
      selectPage--;
      page = getPage(selectPage);
      page->selectLastRow(false,minx,miny,maxx,maxy);
    }
  }

  setSelection(page, minx,miny,maxx,maxy);
  ensureCursorVisible();
}

void XWTeXDocumentCore::selectStartOfLine()
{
  double minx,miny,maxx,maxy;

  XWTeXDocumentCorePage * page = getPage(selectPage);
  page->selectRowStart(minx,miny,maxx,maxy);
  setSelection(page, minx,miny,maxx,maxy);
  ensureCursorVisible();
}

void XWTeXDocumentCore::selectStartOfDocument()
{
  double minx,miny,maxx,maxy;
  doc->selectStartOfDocument(selectPage - 1, minx,miny,maxx,maxy);
  selectPage = 1;
  XWTeXDocumentCorePage * page = getPage(selectPage);
  setSelection(page, minx,miny,maxx,maxy);
  ensureCursorVisible();
}

void XWTeXDocumentCore::selectWord()
{
  double minx,miny,maxx,maxy;
  XWTeXDocumentCorePage * page = getPage(selectPage);
  if (!page->selectWord(minx,miny,maxx,maxy))
  {
    if (selectPage < doc->getNumPages())
    {
      selectPage++;
      page = getPage(selectPage);
      page->selectFirstRow(false, minx,miny,maxx,maxy);
    }
  }

  setSelection(page, minx,miny,maxx,maxy);
  ensureCursorVisible();
}

void XWTeXDocumentCore::setSelection(XWTeXDocumentCorePage * page, double & minx,
                                    double & miny, double & maxx, double & maxy)
{
  ulx = (int)(minx * xScale + page->xDest + 0.5);
  uly = (int)(miny * yScale + page->yDest + 0.5);
  lrx = (int)(maxx * xScale + page->xDest + 0.5);
  lry = (int)(maxy * yScale + page->yDest + 0.5);

  if (selectULX == 0 || ulx < selectULX)
    selectULX = ulx;

  if (selectULY == 0 || uly < selectULY)
    selectULY = uly;

  if (lrx > selectLRX)
    selectLRX = lrx;

  if (lry > selectLRY)
    selectLRY = lry;
}

void XWTeXDocumentCore::clearPages()
{
  QHash<int, XWTeXDocumentCorePage*>::iterator i = pages.begin();
  while (i != pages.end())
	{
		XWTeXDocumentCorePage * p = i.value();
		if (p)
			delete p;
		i = pages.erase(i);
	}
}

void XWTeXDocumentCore::clearPageMarks()
{
  QHash<int, XWTeXDocumentCoreMarks*>::iterator i = pageMarks.begin();
  while (i != pageMarks.end())
	{
		XWTeXDocumentCoreMarks * m = i.value();
		if (m)
			delete m;
		i = pageMarks.erase(i);
	}
}

QIcon XWTeXDocumentCore::createColorIcon(QColor color)
{
  QPixmap pixmap(20, 10);
	QPainter painter(&pixmap);
	painter.setPen(Qt::NoPen);
	painter.fillRect(QRect(0, 0, 20, 10), color);
	return QIcon(pixmap);
}

void XWTeXDocumentCore::updatePages()
{
  int drawAreaWidth = viewport()->width();
	int drawAreaHeight = viewport()->height();
	if (zoomFactor == XW_TEX_DOC_ZOOM_PAGE)
  {
    if (continuousMode)
    {
      xScale = (double)drawAreaWidth / doc->getPageWidth();
      yScale = (double)drawAreaHeight / doc->getPageHeight();
    }
    else
    {
      xScale = (double)(drawAreaWidth - XW_TEX_DOC_DM_PAGE_SPACING) / (2 * doc->getPageWidth());
      yScale = (double)drawAreaHeight / doc->getPageHeight();
    }

    if (yScale > xScale)
      yScale = xScale;
    else if (xScale > yScale)
      xScale = yScale;
  }
  else if (zoomFactor == XW_TEX_DOC_ZOOM_WIDTH)
  {
    xScale = (double)drawAreaWidth / doc->getPageWidth();
    yScale = xScale;
  }
  else
  {
    xScale = 0.01 * zoomFactor;
    yScale = 0.01 * zoomFactor;
  }

  if (continuousMode)
  {
    int n = getNumPages();
		if (direction == TEX_DOC_WD_RTT || direction == TEX_DOC_WD_LTL)
    {
			totalDocW = n * ((int)(doc->getPageWidth() * xScale + 0.5)) + (n - 1) * XW_TEX_DOC_HM_PAGE_SPACING;
			totalDocH = (int)(doc->getPageHeight() * yScale + 0.5);
    }
    else
    {
			totalDocW = (int)(doc->getPageWidth() * xScale + 0.5);
			totalDocH = n * ((int)(doc->getPageHeight() * yScale + 0.5)) + (n - 1) * XW_TEX_DOC_CM_PAGE_SPACING;
    }
  }
  else
  {
    totalDocW = 2 * ((int)(doc->getPageWidth() * xScale + 0.5)) + XW_TEX_DOC_DM_PAGE_SPACING;
		totalDocH = (int)(doc->getPageHeight() * yScale + 0.5);
  }

  canvas->adjustSize();
}
