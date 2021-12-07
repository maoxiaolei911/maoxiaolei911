/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <string.h>
#include <process.h>
#include <QtDebug>
#include <QtGui>
#include <QApplication>
#include <QByteArray>
#include <QTextCodec>
#include <QTextStream>
#include <QUrl>
#include <QDesktopServices>
#include "XWString.h"
#include "XWApplication.h"
#include "XWFileName.h"
#include "XWMediaSea.h"
#include "XWDocSea.h"
#include "XWPictureSea.h"
#include "XWDocSetting.h"
#include "XWObject.h"
#include "XWDoc.h"
#include "XWPDFDoc.h"
#include "XWDVIDoc.h"
#include "XWLink.h"
#include "XWRaster.h"
#include "XWBitmap.h"
#include "XWRasterOutputDev.h"
#include "XWOutline.h"
#include "XWPSOutputDev.h"
#include "XWTextOutputDev.h"
#include "XWImageOutputDev.h"
#include "XWPaintOutputDev.h"
#include "XWDocCanvas.h"
#include "XWAnimationPlayer.h"
#include "XWMediaPlayer.h"
#include "XWSlide.h"
#include "XWRuler.h"
#include "XWGuiCore.h"
#include "XWSearchWindow.h"
#include "XWReferenceWindow.h"

#define TEXT_CURSOR_WIDTH 1

struct ZoomMenuInfo
{
    char *label;
    double zoom;
};

static ZoomMenuInfo zoomMenuInfo[] = {
    { QT_TRANSLATE_NOOP("GuiCore", "400%"),       400 },
    { QT_TRANSLATE_NOOP("GuiCore", "200%"),       200 },
    { QT_TRANSLATE_NOOP("GuiCore", "150%"),       150 },
    { QT_TRANSLATE_NOOP("GuiCore", "100%"),       100 },
    { QT_TRANSLATE_NOOP("GuiCore", "50%"),        50 },
    { QT_TRANSLATE_NOOP("GuiCore", "25%"),        25 },
    { QT_TRANSLATE_NOOP("GuiCore", "12.5%"),      12.5 },
    { QT_TRANSLATE_NOOP("GuiCore", "8.33%"),      8.33 },
    { QT_TRANSLATE_NOOP("GuiCore", "fit page"),  ZOOM_PAGE },
    { QT_TRANSLATE_NOOP("GuiCore", "fit width"), ZOOM_WIDTH }
};

#define MIX_ZOOM_IDX   0
#define MAX_ZOOM_IDX   7
#define ZOOM_PAGE_IDX  8
#define ZOOM_WIDTH_IDX 9

static const char * slideSrings[] = {
    QT_TRANSLATE_NOOP("GuiCore", "Default"),
    QT_TRANSLATE_NOOP("GuiCore", "Extend"),
    QT_TRANSLATE_NOOP("GuiCore", "Fade"),
    QT_TRANSLATE_NOOP("GuiCore", "Rotate"),
    QT_TRANSLATE_NOOP("GuiCore", "Ink")
};

#define MIN_SLIDE_IDX 0
#define MAX_SLIDE_IDX 4


XWCoreMark::XWCoreMark()
{
	color = Qt::red;
}

XWCoreMark::~XWCoreMark()
{
	while (!points.isEmpty())
    delete points.takeFirst();
}

void XWCoreMark::draw(QPainter * painter)
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

void XWCoreMark::markTo(const QPoint & p)
{
	QPoint * pts = new QPoint(p.x(), p.y());
	if (pts)
		points << pts;
}

void XWCoreMark::scale(double sx, double sy)
{
	for (int i = 0; i < points.size(); i++)
	{
		QPoint * p = points[i];
		p->setX(qRound((double)(p->x()) * sx));
		p->setY(qRound((double)(p->y())) * sy);
	}
}

XWCoreMarks::XWCoreMarks()
	:curMark(0)
{
}

XWCoreMarks::~XWCoreMarks()
{
	while (!marks.isEmpty())
    delete marks.takeFirst();
}

void XWCoreMarks::draw(QPainter * painter)
{
	for (int i = 0; i < marks.size(); i++)
	{
		XWCoreMark * m = marks[i];
		m->draw(painter);
	}
}

void XWCoreMarks::endMark()
{
	curMark = 0;
}

void XWCoreMarks::markTo(const QPoint & p)
{
	if (!curMark)
	{
		curMark = new XWCoreMark;
		marks << curMark;
	}

	curMark->markTo(p);
}

void XWCoreMarks::scale(double sx, double sy)
{
	for (int i = 0; i < marks.size(); i++)
	{
		XWCoreMark * m = marks[i];
		m->scale(sx, sy);
	}
}

void XWCoreMarks::setMarkColor(const QColor & color)
{
	curMark = new XWCoreMark;
	marks << curMark;

	curMark->color = color;
}

XWCorePage::XWCorePage()
	:pageNo(0),
	 xDest(0),
	 yDest(0),
	 w(0),
	 h(0),
	 bitmap(0),
	 links(0),
	 text(0),
	 changed(true)
{
}

XWCorePage::~XWCorePage()
{
  clearContent();
}

void XWCorePage::clearContent()
{
	if (bitmap)
	{
   	delete bitmap;
   	bitmap = 0;
  }

  if (links)
  {
    delete links;
    links = 0;
  }

  if (text)
  {
    delete text;
    text = 0;
  }
}

void XWCorePage::drawContent(QPainter * painter, const QRect & rect)
{
	uchar pixel[4];
	QRect r(xDest, yDest, w, h);
	QRect ir = r.intersected(rect);
	QImage img(ir.width(), ir.height(), QImage::Format_RGB32);
	int xd = ir.left() - xDest;
	int yd = ir.top() - yDest;
	for (int x = 0; x < ir.width(); x++)
	{
		for (int y = 0; y < ir.height(); y++)
		{
			bitmap->getPixel(x + xd, y + yd, pixel);
			img.setPixel(x, y, qRgb(pixel[0], pixel[1], pixel[2]));
		}
	}

	painter->drawImage(QPoint(ir.left(), ir.top()), img);
}

void XWCorePage::drawGrid(QPainter * painter, const QRect & rect)
{
	painter->save();

	QPen pen = painter->pen();
	pen.setWidth(1);
	pen.setColor(Qt::gray);
	painter->setPen(pen);
	painter->setOpacity(0.4);

	QRect r(xDest, yDest, w, h);
	QRect ir = r.intersected(rect);

	int xd = ir.left() + (ir.left() % 5);
	int yd = ir.top() + (ir.top() % 5);

	for (int x = xd; x < ir.width(); x+=5)
		painter->drawLine(x,ir.top(),x,ir.bottom());

	for (int y = yd; y < ir.height(); y+=5)
		painter->drawLine(ir.left(),y,ir.right(),y);

	painter->restore();
}

void XWCorePage::drawSelect(QPainter * painter,
                            int x0,
                            int y0,
                            int x1,
                            int y1,
                            uchar * paperColor)
{
	int sw = x1 - x0 + 1;
	int sh = y1 - y0 + 1;
	QRect sr(x0, y0, sw, sh);
	if (!intersects(sr))
		return ;

	QImage img(sw, sh, QImage::Format_RGB32);
	int xd = x0 - xDest;
	int yd = y0 - yDest;
	uchar pixel[4];
	for (int x = 0; x < sw; x++)
	{
		for (int y = 0; y < sh; y++)
		{
			bitmap->getPixel(x + xd, y + yd, pixel);
			img.setPixel(x, y, qRgb(pixel[0] ^ paperColor[0], pixel[1] ^ paperColor[1], pixel[2]) ^ paperColor[2]);
		}
	}

	painter->drawImage(QPoint(x0, y0), img);
}

QImage XWCorePage::getImage()
{
	uchar pixel[4];
	QImage image(bitmap->getWidth(), bitmap->getHeight(), QImage::Format_RGB32);
	for (int i = 0; i < bitmap->getWidth(); i++)
	{
		for (int j = 0; j < bitmap->getHeight(); j++)
		{
			bitmap->getPixel(i, j, pixel);
			image.setPixel(i, j, qRgb(pixel[0], pixel[1], pixel[2]));
		}
	}

	return image;
}

bool XWCorePage::intersects(const QRect & rect)
{
	QRect r(xDest, yDest, w, h);
	return rect.intersects(r);
}


XWGuiCore::XWGuiCore(bool isRefA, QWidget * parent)
    : QScrollArea(parent)
{
	setViewportMargins(0,0,0,0);
	setBackgroundRole(QPalette::NoRole);
	setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	setWidgetResizable(false);
	horizontalScrollBar()->setSingleStep(16);
	verticalScrollBar()->setSingleStep(16);
	isRef = isRefA;
	canvas = new XWDocCanvas(this);
	dev = 0;
	setWidget(canvas);
	connectCanvas();

	sildeMSec = 5 * 60 * 1000;
	historyBLen = 0;
	historyFLen = 0;
	historyCur = DOC_HISTORYSIZE - 1;
	for (int i = 0; i < DOC_HISTORYSIZE; i++)
		history << 0;

	searchContent = 0;
	searchContentLen = 0;
	caseSensitive = false;
	wholeWord = true;
	onePageOnly = true;
	searchBackward = false;

	doc = 0;
	fullScreen = false;
	continuousMode = true;
	doubleMode = false;
	horiMode = true;
	rightToLeft = false;
	marking = false;
	zoomFactor = ZOOM_PAGE;
	zoomIdx = ZOOM_PAGE_IDX;
	rotate = 0;
	tileH = 16;
	tileW = -1,
	maxUnscaledPageW = 0;
	maxUnscaledPageH = 0;
	maxPageW = 0;
	maxPageH = 0;
	totalDocW = 0;
	totalDocH = 0;
	topPage = 0;
	dpi = 72;
	selectPage = 0;
	selectULX = 0;
	selectULY = 0;
	selectLRX = 0;
	selectLRY = 0;
	lastDragLeft = 0;
	lastDragTop = 0;

	colorMode = COLOR_MODE_RGB8;
  bitmapRowPad = 4;
  reverseVideo = false;
  bitmapTopDown = true;
  allowAntialias = true;
	pageColor[0] = 0xff;
  pageColor[1] = 0xff;
  pageColor[2] = 0xff;
  pageColor[3] = 0xff;
	markColor = Qt::red;
	locking = false;

	wheelDelta = 0;

	drawGrid = false;

	showRuler = false;
  fakeWidget = new QWidget(this);
	fakeWidget->setBackgroundRole(QPalette::Window);
	fakeWidget->setFixedSize(RULER_BREADTH,RULER_BREADTH);
	topRuler = new XWRuler(XWRuler::Top,this);
	leftRuler = new XWRuler(XWRuler::Left,this);

	fakeWidget->setVisible(showRuler);
	topRuler->setVisible(showRuler);
	leftRuler->setVisible(showRuler);

	layout = new QGridLayout;
	layout->setSpacing(0);
	layout->setMargin(0);
	layout->addWidget(fakeWidget,0,0);
	layout->addWidget(topRuler,0,1);
	layout->addWidget(leftRuler,1,0);
	layout->addWidget(canvas,1,1);
	setLayout(layout);

	setDisplay();
}

XWGuiCore::~XWGuiCore()
{
	if (searchContent)
		delete [] searchContent;
  clearPages();
  clearPageMarks();
}

void XWGuiCore::addMarkingActions(QMenu * menu)
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

bool XWGuiCore::canFirstPage()
{
	return canPrevPage();
}

bool XWGuiCore::canLastPage()
{
	return canNextPage();
}

bool XWGuiCore::canNextPage()
{
	if (!doc || doc->getNumPages() <= 1 || topPage == doc->getNumPages())
		return false;

	if (doubleMode && ((doc->getNumPages() <= 2) ||
		(topPage == (doc->getNumPages() - 1))))
	{
		return false;
	}

	return true;
}

bool XWGuiCore::canPrevPage()
{
	if (!doc || (doc->getNumPages() <= 1) || topPage == 1)
		return false;

	if (doubleMode && (topPage == 2))
		return false;

	return true;
}

void XWGuiCore::cvtDevToUser(int pg, int xd, int yd, double *xu, double *yu)
{
	if (pages.contains(pg))
	{
		XWCorePage * page = pages[pg];
    *xu = page->ictm[0] * xd + page->ictm[2] * yd + page->ictm[4];
    *yu = page->ictm[1] * xd + page->ictm[3] * yd + page->ictm[5];
	}
	else
	{
		*xu = 0;
		*yu = 0;
	}
}

void XWGuiCore::cvtDevToWindow(int pg, int xd, int yd, int *xw, int *yw)
{
	if (pages.contains(pg))
	{
		XWCorePage * page = pages[pg];
		*xw = xd + page->xDest;
		*yw = yd + page->yDest;
	}
	else
	{
		*xw = 0;
		*yw = 0;
	}
}

void XWGuiCore::cvtUserToDev(int pg, double xu, double yu, int *xd, int *yd)
{
	if (pages.contains(pg))
	{
		XWCorePage * page = pages[pg];
		*xd = (int)(page->ctm[0] * xu + page->ctm[2] * yu + page->ctm[4] + 0.5);
    *yd = (int)(page->ctm[1] * xu + page->ctm[3] * yu + page->ctm[5] + 0.5);
	}
	else
	{
		*xd = 0;
		*yd = 0;
	}
}

void XWGuiCore::cvtUserToWindow(int pg, double xu, double yu, int *xw, int *yw)
{
	if (pages.contains(pg))
	{
		XWCorePage * page = pages[pg];
		*xw = page->xDest + (int)(page->ctm[0] * xu + page->ctm[2] * yu + page->ctm[4] + 0.5);
    *yw = page->yDest + (int)(page->ctm[1] * xu + page->ctm[3] * yu + page->ctm[5] + 0.5);
	}
	else
	{
		*xw = 0;
		*yw = 0;
	}
}

bool XWGuiCore::cvtWindowToDev(int xw, int yw, int *pg, int *xd, int *yd)
{
	QHash<int, XWCorePage*>::iterator i = pages.begin();
  while (i != pages.end())
	{
		XWCorePage * page = i.value();
		if (page->contains(xw, yw))
		{
			*pg = i.key();
			*xd = xw - page->xDest;
			*yd = yw - page->yDest;
			return true;
		}

		i++;
	}

   *pg = 0;
   *xd = *yd = 0;
   return false;
}

bool XWGuiCore::cvtWindowToUser(int xw, int yw, int *pg, double *xu, double *yu)
{
	QHash<int, XWCorePage*>::iterator i = pages.begin();
  while (i != pages.end())
	{
		XWCorePage * page = i.value();
		if (page->contains(xw, yw))
		{
			*pg = i.key();
			xw -= page->xDest;
      yw -= page->yDest;
      *xu = page->ictm[0] * xw + page->ictm[2] * yw + page->ictm[4];
      *yu = page->ictm[1] * xw + page->ictm[3] * yw + page->ictm[5];
			return true;
		}

		i++;
	}

  *pg = 0;
  *xu = *yu = 0;
  return false;
}

void XWGuiCore::endMark()
{
	QHash<int, XWCoreMarks*>::iterator i = pageMarks.begin();
  while (i != pageMarks.end())
	{
		XWCoreMarks * m = i.value();
		m->endMark();
		i++;
	}
}

void XWGuiCore::extractImage(const QString & basename)
{
	int pgs = getNumPages();
	if (pgs <= 0)
		return ;

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	QByteArray ba = QFile::encodeName(basename);
	XWImageOutputDev * imgout = new XWImageOutputDev(ba.data(), true);
	doc->displayPages(imgout, 1, pgs, 72, 72, 0, false, false, false);
	delete imgout;
	QApplication::restoreOverrideCursor();
}

bool XWGuiCore::find(const QString & content,
	                     bool  caseSensitiveA,
                       bool  nextA,
                       bool  backwardA,
                       bool wholeWordA,
		                   bool onePageOnlyA)
{
	if (content.length() <= 0)
		return false;

	if (searchContent)
		delete [] searchContent;

	searchContentLen = content.length();
	searchContent = new uint[searchContentLen];
	for (int i = 0; i < searchContentLen; i++)
		searchContent[i] = content[i].unicode();

	caseSensitive = caseSensitiveA;
	searchBackward = backwardA;
	wholeWord = wholeWordA;
	onePageOnly = onePageOnlyA;
	return findU(searchContent, searchContentLen, caseSensitive, nextA, backwardA, wholeWord, onePageOnlyA);
}

bool XWGuiCore::find(char *s,
                       bool caseSensitiveA,
                       bool nextA,
                       bool backwardA,
                       bool wholeWordA,
		                   bool onePageOnlyA)
{
	if (!s || strlen(s) < 1)
		return false;

	caseSensitive = caseSensitiveA;
	searchBackward = backwardA;
	wholeWord = wholeWordA;
	onePageOnly = onePageOnlyA;

	return find(s, nextA);
}

XWLinkAction * XWGuiCore::findLink(int pg, double x, double y)
{
	XWCorePage * page = pages[pg];
  if (page)
     return page->links ? page->links->find(x, y) : (XWLinkAction *)0;

  return 0;
}

bool XWGuiCore::findU(uint *u,
                      int len,
                      bool caseSensitiveA,
		                  bool nextA,
		                  bool backwardA,
		                  bool wholeWordA,
		                  bool onePageOnlyA)
{
	caseSensitive = caseSensitiveA;
	searchBackward = backwardA;
	wholeWord = wholeWordA;
	onePageOnly = onePageOnlyA;

	return findU(u, len, nextA);
}

QImage  XWGuiCore::getImage(int pg)
{
	if (pg < 1)
		pg = 1;

	if (pg > getNumPages())
		pg = getNumPages();

	XWCorePage* page = getPage(pg);
	fillPage(page);
	if (!page->bitmap)
		return QImage(page->w, page->h, QImage::Format_RGB32);

	return page->getImage();
}

int XWGuiCore::getNumPages()
{
	if (!doc || !doc->isOk())
		return 0;

	return doc->getNumPages();
}

void XWGuiCore::getPageAndDest(int pageA,
                               XWString *destName,
				                       int *pageOut,
				                       XWLinkDest **destOut)
{
	*pageOut = pageA;
  *destOut = 0;
  if (!doc)
  	return ;

  if (destName && (*destOut = doc->findDest(destName)))
  {
   	if ((*destOut)->isPageRef())
   	{
     		ObjRef pageRef = (*destOut)->getPageRef();
     		*pageOut = doc->findPage(pageRef.num, pageRef.gen);
   	}
   	else
      		*pageOut = (*destOut)->getPageNum();
  }

  if (*pageOut <= 0)
   	*pageOut = 1;

  if (*pageOut > doc->getNumPages())
   	*pageOut = doc->getNumPages();
}

bool XWGuiCore::getSelection(int *pg,
                             double *ulx,
                             double *uly,
		                         double *lrx,
		                         double *lry)
{
	if (selectULX == selectLRX || selectULY == selectLRY)
     return false;
  *pg = selectPage;
  cvtDevToUser(selectPage, selectULX, selectULY, ulx, uly);
  cvtDevToUser(selectPage, selectLRX, selectLRY, lrx, lry);
  return true;
}

QPixmap XWGuiCore::getSelectionPix()
{
	QPixmap pix;
	int w = selectLRX - selectULX;
  int h = selectLRY - selectULY;
  int pg = getPageNum();
  int x = 0;
  int y = 0;
  if (!cvtWindowToDev(selectULX, selectULY, &pg, &x, &y))
  	return pix;

  pix = QPixmap(w, h);
  QPainter painter(&pix);
	XWPaintOutputDev * paintout = new XWPaintOutputDev(&painter,colorMode, bitmapRowPad, reverseVideo, pageColor, bitmapTopDown, allowAntialias);
  paintout->setArea(0, 0, x, y, w, h);
	XWDoc * doc = getDoc();
	paintout->startDoc(doc->getXRef());
	paintout->startPage(pg, NULL);
	doc->displayPage(paintout, pg, dpi, dpi, 0, true, true, true);
	delete paintout;
	return pix;
}

QString XWGuiCore::getSelectionStr()
{
	int pg;
  double ulx, uly, lrx, lry;
  QString str;
  if (getSelection(&pg, &ulx, &uly, &lrx, &lry))
  {
  	XWString * currentSelection = extractText(pg, ulx, uly, lrx, lry);
  	if (currentSelection)
  	{
  		str = currentSelection->toQString();
  		delete currentSelection;
  	}
  }

  return str;
}

QStringList XWGuiCore::getSlides()
{
	QStringList ret;
	for (int i = MIN_SLIDE_IDX; i <= MAX_SLIDE_IDX; i++)
		ret << tr(slideSrings[i]);

	return ret;
}

QStringList XWGuiCore::getZooms()
{
	QStringList ret;
	for (int i = MIX_ZOOM_IDX; i <= ZOOM_WIDTH_IDX; i++)
		ret << tr(zoomMenuInfo[i].label);

	return ret;
}

bool XWGuiCore::gotoNamedDestination(XWString *dest)
{
	if (!doc)
     return false;

  XWLinkDest * d = doc->findDest(dest);
  if (!d)
     return false;

  displayDest(d);
  delete d;
  return true;
}

bool XWGuiCore::gotoNextPage(int inc, bool top)
{
	if (!doc)
     return false;

	XWCorePage* page = 0;
	int topPageA = topPage;
	if (!canNextPage())
	{
		if (slideTimer.isActive())
			slideTimer.stop();

		if (doubleMode && topPageA == (doc->getNumPages() - 1))
		{
    	topPageA++;
    	page = getPage(topPageA);
    }
  }
  else
  {
  	int pg = topPageA + inc;
  	if (pg > doc->getNumPages())
      pg = doc->getNumPages();

  	if (topPageA != pg)
  		addToHist(pg);

  	topPageA = pg;
  	page = getPage(topPageA);
  }

  if (page)
  {
  	topPage = topPageA;
  	QScrollBar * hbar = horizontalScrollBar();
  	if (top && !horiMode)
  		scrollTo(hbar->value(), page->yDest);
  	else if (!rightToLeft)
  		scrollTo(page->xDest, page->yDest);
  	else
  		scrollTo(page->xDest + page->w - viewport()->width(), page->yDest);
  }

  return true;
}

bool XWGuiCore::gotoPrevPage(int dec, bool top, bool bottom)
{
	if (!doc)
     return false;

	XWCorePage* page = 0;
	int topPageA = topPage;
	if (!canPrevPage())
	{
		if (doubleMode && topPageA == 2)
		{
    	topPageA--;
    	page = getPage(topPageA);
    }
	}
	else
	{
		int pg = topPageA - dec;
  	if (pg < 1)
     	pg = 1;

    if (topPageA != pg)
  		addToHist(pg);

  	topPageA = pg;
  	page = getPage(topPageA);
	}

  if (page)
  {
  	topPage = topPageA;
  	QScrollBar * hbar = horizontalScrollBar();
  	if (top && !horiMode)
  		scrollTo(hbar->value(), page->yDest);
  	else if (bottom)
  		scrollTo(hbar->value(), page->yDest + page->h - viewport()->height());
  	else if (!rightToLeft)
  		scrollTo(page->xDest, page->yDest);
  	else
  		scrollTo(page->xDest + page->w - viewport()->width(), page->yDest);
  }

  return true;
}

void XWGuiCore::markTo(const QPoint & p)
{
	if (!doc)
     return ;

	QHash<int, XWCorePage*>::iterator i = pages.begin();
	XWCorePage * page = 0;
  while (i != pages.end())
	{
		page = i.value();
		if (page->contains(p))
			break;

		i++;
		page = 0;
	}

	if (page)
	{
		XWCoreMarks* m = 0;
		if (pageMarks.contains(page->pageNo))
			m = pageMarks[page->pageNo];
		else
		{
			m = new XWCoreMarks;
			m->setMarkColor(markColor);
			pageMarks[page->pageNo] = m;
		}
		m->markTo(p);
	}

	lastPoint = p;
	canvas->update();
}

void XWGuiCore::moveSelection(int pg, int x, int y)
{
	if (!doc)
     return ;

	if (pg != selectPage)
     return;

  double newSelectULX, newSelectULY, newSelectLRX, newSelectLRY;
  if (lastDragLeft)
  {
     if (x < selectLRX)
     {
         newSelectULX = x;
         newSelectLRX = selectLRX;
     }
     else
     {
         newSelectULX = selectLRX;
         newSelectLRX = x;
         lastDragLeft = false;
     }
  }
  else
  {
     if (x > selectULX)
     {
         newSelectULX = selectULX;
         newSelectLRX = x;
     }
     else
     {
         newSelectULX = x;
         newSelectLRX = selectULX;
         lastDragLeft = true;
     }
  }

  if (lastDragTop)
  {
     if (y < selectLRY)
     {
        newSelectULY = y;
        newSelectLRY = selectLRY;
     }
     else
     {
        newSelectULY = selectLRY;
        newSelectLRY = y;
        lastDragTop = false;
     }
  }
  else
  {
     if (y > selectULY)
     {
         newSelectULY = selectULY;
         newSelectLRY = y;
     }
     else
     {
         newSelectULY = y;
         newSelectLRY = selectULY;
         lastDragTop = true;
     }
  }

  setSelection(selectPage, newSelectULX, newSelectULY, newSelectLRX, newSelectLRY);
}

bool XWGuiCore::okToAddNotes()
{
	if (!doc)
     return false;

	return doc->okToAddNotes(false);
}

bool XWGuiCore::okToChange()
{
	if (!doc)
     return false;

	return doc->okToChange(false);
}

bool XWGuiCore::okToCopy()
{
	if (!doc)
     return false;

	return doc->okToCopy(false);
}

bool XWGuiCore::okToPrint()
{
	if (!doc)
     return false;

	return doc->okToPrint(false);
}

void XWGuiCore::play(int min, int idx)
{
	XWSlide * slide = 0;
	switch (idx)
	{
		case 0:
			slide = new XWSlide(this);
			break;

		case 1:
			slide = new ExtendedSlide(this);
			break;

		case 2:
			slide = new XWFadeSlide(this);
			break;

		case 3:
			slide = new XWRotateSlide(this);
			break;

		case 4:
			slide = new XWInkSlide(this);
			break;

		default:
			return ;
			break;
	}

	canvas = slide;
	setWidget(canvas);
	layout->addWidget(canvas,1,1);
	connectCanvas();
	continuousMode = false;
	doubleMode = false;
	fullScreen = true;
	zoomIdx = ZOOM_PAGE_IDX;
	zoomFactor = ZOOM_PAGE;
	sildeMSec = min * 60 * 1000;

	connect(&slideTimer, SIGNAL(timeout()), slide, SLOT(start()));
	slideTimer.start(sildeMSec);
}

void XWGuiCore::print(QPrinter * printer)
{
	if (!printer)
		return ;

	int firstpage = printer->fromPage();
	int lastpage = printer->toPage();
	if (firstpage == 0)
		firstpage = 1;

	if (lastpage == 0)
		lastpage = getNumPages();

	if (firstpage > lastpage)
	{
		int t = firstpage;
		firstpage = lastpage;
		lastpage = t;
	}

	QPainter * painter = new QPainter(printer);
	XWPaintOutputDev * paintout = new XWPaintOutputDev(painter);
	XWDoc * doc = getDoc();
	paintout->startDoc(doc->getXRef());
	paintout->startPage(0, NULL);
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	painter->begin(printer);

	XWDocSetting docsetting;
	bool crop = docsetting.getPSCrop();
	for (int pg = firstpage; pg <= lastpage; pg++)
	{
		locking = true;
		doc->displayPage(paintout, pg, printer->logicalDpiX(), printer->logicalDpiY(), 0, true, crop, true);
		paintout->startPage(0, NULL);
		locking = false;
		if (!printer->newPage())
			break;
	}

	painter->end();
	delete painter;
	delete paintout;

	QApplication::restoreOverrideCursor();
}

void XWGuiCore::redraw(QPainter * painter, const QRect & r)
{
	if (getNumPages() == 0)
		return ;

	QScrollBar * hbar = horizontalScrollBar();
  QScrollBar * vbar = verticalScrollBar();
  int x = hbar->value();
  int y = vbar->value();
	QList<int> pgs;
	int j = topPage;
	XWCorePage* page = 0;
	QRect vr(x, y, viewport()->width(), viewport()->height());
	if (continuousMode)
	{
		if (horiMode)
		{
			if (doubleMode)
			{
				j = 2 * (x / (maxPageW + continuousModePageSpacing)) + 1;
				if (rightToLeft)
				{
					j = getNumPages() - j;
					if (0 == (j % 2))
						j--;
				}
			}
			else
			{
				j = x / (maxPageW + continuousModePageSpacing) + 1;
				if (rightToLeft)
					j = getNumPages() - j;
			}
		}
		else
		{
			if (doubleMode)
				j = 2 * (y / (maxPageH + continuousModePageSpacing)) + 1;
			else
				j = y / (maxPageH + continuousModePageSpacing) + 1;
		}

		for (; j <= getNumPages(); j++)
		{
			page = getPage(j);
			if (page->intersects(vr))
			{
				pgs << j;
				drawPage(painter, page, r);
			}
			else
				break;
		}
	}
	else
	{
    if (doubleMode)
    {
      if (topPage < getNumPages())
      {
        pgs << topPage + 1;
        page = getPage(topPage + 1);
        if (page->intersects(vr))
      		drawPage(painter, page, r);
      }
      else if (topPage > 1)
      {
        pgs << topPage - 1;
        page = getPage(topPage - 1);
        if (page->intersects(vr))
      		drawPage(painter, page, r);
      }

      pgs << topPage;
    	page = getPage(topPage);
      if (page->intersects(vr))
    	  drawPage(painter, page, r);
    }
    else
    {
      pgs << topPage;
  		page = getPage(topPage);
  		drawPage(painter, page, r);
    }
	}

	QHash<int, XWCorePage*>::iterator i = pages.begin();
	int ss = 0;
	int topPageA = topPage;
	while (i != pages.end())
	{
		XWCorePage * page = i.value();
		if (!pgs.contains(page->pageNo))
		{
			locking = true;
			i = pages.erase(i);
			delete page;
			locking = false;
			continue;
		}
		else if (continuousMode || doubleMode)
		{
			if (page->intersects(vr))
			{
				QRect tmpr(page->xDest, page->yDest, page->w, page->h);
				QRect ir = tmpr.intersected(vr);
				if ((ir.width() * ir.height()) > ss)
				{
					topPageA = page->pageNo;
					ss = ir.width() * ir.height();
				}
			}
		}

		i++;
	}

  topPage = topPageA;
  emitGoto();
}

void XWGuiCore::saveDVIToPS(const QString & outname, const QList<int> & pages)
{
	QString tmpfilename = xwApp->getTmpFile();
	QFile * tmpfile = new QFile(tmpfilename);
	if (!tmpfile->open(QIODevice::WriteOnly))
	{
		delete tmpfile;
		return ;
	}

	QString dvin = QString("%1.dvi").arg(tmpfilename);
	QString pdfn = QString("%1.pdf").arg(tmpfilename);

	QByteArray dviba = dvin.toLocal8Bit();
	QByteArray pdfba = pdfn.toLocal8Bit();

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	XWDVIDoc * dvidoc = (XWDVIDoc*)(getDoc());
	dvidoc->saveToPDF(tmpfile, dviba.data(), pdfba.data(), QString(), pages, 0, 0);
	tmpfile->close();
	delete tmpfile;

	XWPDFDoc * pdfdoc = new XWPDFDoc;
	QTextCodec * codec = QTextCodec::codecForLocale();
	QByteArray ba = codec->fromUnicode(tmpfilename);
	XWString * fn = new XWString(ba.data(), ba.size());
	if (!pdfdoc->load(fn, 0, 0, 0))
	{
		delete pdfdoc;
	  QApplication::restoreOverrideCursor();
		return ;
	}

	QByteArray localname = QFile::encodeName(outname);
	int firstpage = 1;
	int lastpage = pdfdoc->getNumPages();
	XWPSOutputDev * psout = new XWPSOutputDev(localname.data(), pdfdoc, firstpage, lastpage, psModePS);

	if (psout->isOk())
	{
		XWDocSetting docsetting;
		bool crop = docsetting.getPSCrop();
		for (int i = firstpage; i <= lastpage; i++)
		{
			pdfdoc->displayPage(psout, i, 72, 72, 0, true, crop, true);
		}
	}

	delete psout;
	delete pdfdoc;
	QFile::remove(tmpfilename);
	QApplication::restoreOverrideCursor();
}

void XWGuiCore::saveToImage(const QString & outname,
                              const QString & fmt,
                              const QList<int> & pages)
{
	if (!doc)
     return ;

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	XWPaintOutputDev * paintout = new XWPaintOutputDev(0,colorMode, bitmapRowPad, reverseVideo, pageColor, bitmapTopDown, allowAntialias);
	XWDoc * doc = getDoc();
	paintout->startDoc(doc->getXRef());
	paintout->startPage(0, NULL);
	QString suffix = fmt.toLower();
	suffix.insert(0, ".");
	QByteArray fmtba = fmt.toAscii();
	for (int i = 0; i < pages.size(); i++)
	{
		int pg = pages.at(i);
		QImage * img = new QImage(maxPageW, maxPageH, QImage::Format_RGB32);
		QPainter * painter = new QPainter(img);
		paintout->setPainter(painter);
		doc->displayPage(paintout, pg, dpi, dpi, 0, true, true, true);
		paintout->startPage(0, NULL);
		delete painter;
		QString filename = QString("%1%2%3").arg(outname).arg(pg).arg(suffix);
		if (!img->save(filename, fmtba.constData(), 0))
		{
			delete img;
			break;
		}

		delete img;
	}

	delete paintout;

	QApplication::restoreOverrideCursor();
}

void XWGuiCore::saveToImage(const QString & outname,
                            const QString & fmt,
                            const QColor & bg,
                            bool reverseVideoA)
{
	if (!doc)
     return ;

  int w = selectLRX - selectULX;
  int h = selectLRY - selectULY;
  int pg = getPageNum();
  int x = 0;
  int y = 0;
  if (!cvtWindowToDev(selectULX, selectULY, &pg, &x, &y))
  	return ;

  QImage img;
  QString suffix = fmt.toLower();
  QByteArray fmtba = fmt.toAscii();
  QString filename = QString("%1.%2").arg(outname).arg(suffix);
  if (QFile::exists(filename))
  {
  	img = QImage(filename, fmtba.constData());
  	if (img.width() != w || img.height() != h)
  		img = QImage(w, h, QImage::Format_RGB32);
  }
  else
  	img = QImage(w, h, QImage::Format_RGB32);

  QPainter painter(&img);
	XWPaintOutputDev * paintout = new XWPaintOutputDev(&painter,colorMode, bitmapRowPad, reverseVideoA, pageColor, bitmapTopDown, allowAntialias);
  if (bg.isValid() && bg != Qt::white)
  {
  	uchar pc[4];
  	pc[0] = (uchar)(bg.red());
		pc[1] = (uchar)(bg.green());
		pc[2] = (uchar)(bg.blue());
		paintout->setPaperColor(pc);
  }
  paintout->setArea(0, 0, x, y, w, h);

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	XWDoc * doc = getDoc();
	paintout->startDoc(doc->getXRef());
	paintout->startPage(pg, NULL);
	doc->displayPage(paintout, pg, dpi, dpi, 0, true, true, true);

	img.save(filename, fmtba.constData(), 0);

	delete paintout;

	QApplication::restoreOverrideCursor();
}

void XWGuiCore::saveToPs(const QString & outname, const QList<int> & pages)
{
	if (!doc)
     return ;

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	QByteArray localname = QFile::encodeName(outname);
	int firstpage = pages.first();
	int lastpage = pages.last();
	XWDoc * doc = getDoc();
	XWPSOutputDev * psout = new XWPSOutputDev(localname.data(), doc, firstpage, lastpage, psModePS);

	if (psout->isOk())
	{
		XWDocSetting docsetting;
		bool crop = docsetting.getPSCrop();
		for (int i = 0; i < pages.size(); i++)
		{
			int pg = pages.at(i);
			doc->displayPage(psout, pg, 72, 72, 0, true, crop, true);
		}
	}

	delete psout;

	QApplication::restoreOverrideCursor();
}

void XWGuiCore::saveToText(const QString & outname, const QList<int> & pages)
{
	if (!doc)
     return ;

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	QByteArray localname = QFile::encodeName(outname);
	XWDoc * doc = getDoc();
	XWTextOutputDev * txtout = new XWTextOutputDev(localname.data(), true, 0, false, false);
	if (txtout->isOk())
	{
		for (int i = 0; i < pages.size(); i++)
		{
			int pg = pages.at(i);
			doc->displayPage(txtout, pg, 72, 72, 0, false, true, false);
		}
	}

	delete txtout;

	QApplication::restoreOverrideCursor();
}

void XWGuiCore::setDoc(XWDoc * docA)
{
	if (doc)
	{
		locking = true;
		clearPageMarks();
		delete doc;
		doc = 0;
		if (dev)
		{
  		delete dev;
  		dev = 0;
  	}

  	locking = false;
	}

	topPage = 0;
	selectPage = 0;
	doc = docA;
	maxUnscaledPageW = 0;
	maxUnscaledPageH = 0;
	maxPageW = 0;
  maxPageH = 0;
  totalDocW = 0;
  totalDocH = 0;
  history.clear();
	historyBLen = 0;
	historyFLen = 0;
	historyCur = DOC_HISTORYSIZE - 1;
	for (int i = 0; i < DOC_HISTORYSIZE; i++)
		history << 0;

	if (doc)
	{
		if (doc->getNumPages() > 0)
			topPage = 1;

		for (int i = 1; i <= doc->getNumPages(); ++i)
  	{
  		double w = doc->getPageCropWidth(i);
   		double h = doc->getPageCropHeight(i);
   		if (doc->getPageRotate(i) == 90 || doc->getPageRotate(i) == 270)
   		{
   			double t = w;
   			w = h;
   			h = t;
   		}

   		if (w > maxUnscaledPageW)
   			maxUnscaledPageW = w;

   		if (h > maxUnscaledPageH)
   			maxUnscaledPageH = h;
  	}

  	updatePages();
	}
	else
	{
		canvas->adjustSize();
		canvas->update();
	}
}

void XWGuiCore::setCurrentPage(int cur)
{
	if (!doc)
     return ;

  if (cur <= 0)
  	cur = 1;

  if (cur > getNumPages())
  	cur = getNumPages();

  topPage = cur;
	int num = getNumPages();
	emit currentChanged(cur, num);
}

void XWGuiCore::setFullScreen(bool e)
{
	if (!doc)
     return ;

	if (fullScreen != e)
	{
		fullScreen = e;
		if (e)
		{
			horiMode = false;
			zoomFactor = ZOOM_PAGE;
			zoomIdx = ZOOM_PAGE_IDX;
			continuousMode = false;
		}
	}
}

void XWGuiCore::setSelection(int newSelectPage,
		                         int newSelectULX,
		                         int newSelectULY,
		                         int newSelectLRX,
		                         int newSelectLRY)
{
	if (!doc)
     return ;

	selectPage = newSelectPage;
  selectULX = newSelectULX;
  selectULY = newSelectULY;
  selectLRX = newSelectLRX;
  selectLRY = newSelectLRY;
	canvas->update();
}

void XWGuiCore::changeMarkColor(QAction* a)
{
	if (!a || !doc)
     return ;

	QColor color = qVariantValue<QColor>(a->data());
	setMarkColor(color);
}

void XWGuiCore::clearMark()
{
	if (!doc || selectPage <= 0)
     return ;

  if (pageMarks.contains(selectPage))
  {
  	XWCoreMarks* m = pageMarks.take(selectPage);
  	delete m;
  }
}

void XWGuiCore::copy()
{
	if (!doc)
     return ;

	int pg;
  double ulx, uly, lrx, lry;
  if (getSelection(&pg, &ulx, &uly, &lrx, &lry))
  {
  	XWString * currentSelection = extractText(pg, ulx, uly, lrx, lry);
  	if (currentSelection)
  	{
  		QString str = currentSelection->toQString();
  		QApplication::clipboard()->setText(str);
  		delete currentSelection;
  	}
  }
}

void XWGuiCore::displayDest(XWString *namedDest)
{
	if (!doc || !namedDest)
     return ;

	XWLinkDest *dest = doc->findDest(namedDest);
	displayDest(dest);
}

void XWGuiCore::displayDest(XWLinkDest *dest)
{
	if (!doc || !dest)
     return ;

	int topPageA = topPage;
	if (dest->isPageRef())
  {
      ObjRef pageRef = dest->getPageRef();
      topPageA = doc->findPage(pageRef.num, pageRef.gen);
  }
  else
      topPageA = dest->getPageNum();

  if (topPageA <= 0)
      topPageA = 1;

  if (topPageA > doc->getNumPages())
  	topPageA = doc->getNumPages();

  XWCorePage * page = getPage(topPageA);
  fillPage(page);
  if (!page->bitmap)
  	return ;

  int scrollXA = page->xDest;
  int scrollYA = page->yDest;
  int dx, dy;
  double zoomFactorA = zoomFactor;
  switch (dest->getKind())
  {
     case LINK_DEST_XYZ:
     		zoomFactorA = dest->getZoom();
     		if (zoomFactorA != zoomFactor && zoomFactorA != 0.0)
     		{
     			if (zoomFactorA > 400)
     				zoomFactorA = 400;

     			if (zoomFactorA < 8.33)
     				zoomFactorA = 8.33;

     			zoomFactor = zoomFactorA;
     			updatePages();
     			page = getPage(topPageA);
  				scrollXA = page->xDest;
  				scrollYA = page->yDest;
     		}
     		cvtUserToWindow(topPageA, dest->getLeft(), dest->getTop(), &dx, &dy);
     		if (dest->getChangeLeft())
     			scrollXA = dx;
     		if (dest->getChangeTop())
     			scrollYA = dy;
     		topPage = topPageA;
     		scrollTo(scrollXA, scrollYA);
     		break;

     case LINK_DEST_FIT:
     case LINK_DEST_FITB:
     		zoomFactorA = ZOOM_PAGE;
     		if (zoomFactorA != zoomFactor)
     		{
     			zoomFactor = zoomFactorA;
     			updatePages();
     			page = getPage(topPageA);
  				scrollXA = page->xDest;
  				scrollYA = page->yDest;
  				emit zoomChanged(ZOOM_PAGE_IDX);
     		}
     		topPage = topPageA;
     		scrollTo(scrollXA, scrollYA);
     		break;

     case LINK_DEST_FITH:
     		cvtUserToWindow(topPageA, dest->getLeft(), dest->getTop(), &dx, &dy);
     		if (dest->getChangeTop())
     			scrollYA = dy;
     		topPage = topPageA;
     		scrollTo(scrollXA, scrollYA);
     		break;

     case LINK_DEST_FITBH:
     		zoomFactorA = ZOOM_WIDTH;
     		if (zoomFactorA != zoomFactor)
     		{
     			zoomFactor = zoomFactorA;
     			updatePages();
     			page = getPage(topPageA);
  				scrollXA = page->xDest;
  				scrollYA = page->yDest;
  				emit zoomChanged(ZOOM_WIDTH_IDX);
     		}
     		cvtUserToWindow(topPageA, dest->getLeft(), dest->getTop(), &dx, &dy);
     		if (dest->getChangeTop())
     			scrollYA = dy;
     		topPage = topPageA;
     		scrollTo(scrollXA, scrollYA);
     		break;

     case LINK_DEST_FITV:
     		zoomFactorA = ZOOM_PAGE;
     		if (zoomFactorA != zoomFactor)
     		{
     			zoomFactor = zoomFactorA;
     			updatePages();
     			page = getPage(topPageA);
  				scrollXA = page->xDest;
  				scrollYA = page->yDest;
  				emit zoomChanged(ZOOM_PAGE_IDX);
     		}
     		cvtUserToWindow(topPageA, dest->getLeft(), dest->getTop(), &dx, &dy);
     		if (dest->getChangeLeft())
     			scrollXA = dx;
     		topPage = topPageA;
     		scrollTo(scrollXA, scrollYA);
     		break;

     case LINK_DEST_FITBV:
     		zoomFactorA = ZOOM_PAGE;
     		if (zoomFactorA != zoomFactor)
     		{
     			zoomFactor = zoomFactorA;
     			updatePages();
     			page = getPage(topPageA);
  				scrollXA = page->xDest;
  				scrollYA = page->yDest;
  				emit zoomChanged(ZOOM_PAGE_IDX);
     		}
     		cvtUserToWindow(topPageA, dest->getLeft(), dest->getTop(), &dx, &dy);
     		if (dest->getChangeLeft())
     			scrollXA = dx;
     		topPage = topPageA;
     		scrollTo(scrollXA, scrollYA);
     		break;

     case LINK_DEST_FITR:
     		zoomToRect(topPageA, dest->getLeft(), dest->getTop(), dest->getRight(), dest->getBottom());
     		break;

     default:
     		topPage = topPageA;
     		scrollTo(scrollXA, scrollYA);
      	break;
  }

  addToHist(topPageA);
}

void XWGuiCore::displayPage(int pg)
{
	displayPage(pg, false);
}

void XWGuiCore::displayPage(int pg, bool scrollToTop)
{
	if (!doc || pg <= 0 || pg > getNumPages())
     return ;

	lastDragLeft = lastDragTop = true;
	if (pg != topPage)
		addToHist(pg);

	int topPageA = pg;
	XWCorePage * page = getPage(topPageA);
	QScrollBar * hbar = horizontalScrollBar();
	topPage = topPageA;
  if (scrollToTop && !horiMode)
  	scrollTo(hbar->value(), page->yDest);
  else if (!rightToLeft)
  	scrollTo(page->xDest, page->yDest);
  else
  	scrollTo(page->xDest + page->w - viewport()->width(), page->yDest);
}

void XWGuiCore::displayPage(int pg, double xMin, double yMin, double xMax, double yMax)
{
	int newSelectULX = 0;
	int newSelectULY = 0;
	int newSelectLRX = 0;
	int newSelectLRY = 0;
	cvtDevToWindow(pg, (int)floor(xMin), (int)floor(yMin), &newSelectULX, &newSelectULY);
	cvtDevToWindow(pg, (int)ceil(xMax), (int)ceil(yMax), &newSelectLRX, &newSelectLRY);
  setSelection(pg,  newSelectULX, newSelectULY, newSelectLRX, newSelectLRY);
  displayPage(pg);
}

void XWGuiCore::doAction(XWLinkAction *action)
{
	if (!doc || !action)
     return ;

	XWLinkDest *dest = 0;
  	XWString *namedDest = 0;
  	char *s = 0;
  	XWString * fileName = 0;
  	XWString *actionName = 0;
  	XWObject movieAnnot, obj1, obj2;
  	XWString * cmd = 0;
  	XWDocSetting setting;

  	XWDoc * doc = getDoc();
  	int kind = action->getKind();
  	switch (kind)
  	{
  		case LINK_ACTION_GOTO:
  		case LINK_ACTION_GOTOR:
  			if (kind == LINK_ACTION_GOTO)
  			{
  				if ((dest = ((XWLinkGoTo *)action)->getDest()))
  					dest = dest->copy();
  				else if ((namedDest = ((XWLinkGoTo *)action)->getNamedDest()))
  					namedDest = namedDest->copy();

  				if (namedDest)
  				{
      				dest = doc->findDest(namedDest);
      				delete namedDest;
    			}
    			if (dest)
    			{
      				displayDest(dest);
      				delete dest;
    			}
  			}
  			else
  			{
  				dest = ((XWLinkGoToR *)action)->getDest();
  				if (!dest)
  					namedDest = ((XWLinkGoToR *)action)->getNamedDest();

  				fileName = ((XWLinkGoToR *)action)->getFileName();
  				if (fileName)
  					doDoc(fileName, dest, namedDest);
  			}
  			break;

  		case LINK_ACTION_LAUNCH:
  			fileName = ((XWLinkLaunch *)action)->getFileName();
  			s = fileName->getCString();
    		if (!strcmp(s + fileName->getLength() - 4, ".pdf") ||
				!strcmp(s + fileName->getLength() - 4, ".PDF") ||
				!strcmp(s + fileName->getLength() - 4, ".dvi") ||
				!strcmp(s + fileName->getLength() - 4, ".DVI") ||
				!strcmp(s + fileName->getLength() - 4, ".odvi") ||
				!strcmp(s + fileName->getLength() - 4, ".ODVI") ||
				!strcmp(s + fileName->getLength() - 4, ".mps") ||
				!strcmp(s + fileName->getLength() - 4, ".MPS"))
			{
				doDoc(fileName, 0, 0);
			}
			else
				system(fileName->getCString());
  			break;

  		case LINK_ACTION_URI:
  			cmd = setting.getURLCommand();
  			fileName = ((XWLinkURI *)action)->getURI();
			if (cmd)
				runCommand(cmd, fileName);
  			else
  			{
  				QString fn = fileName->toQString();
  				QUrl url(fn);
  				QString path = url.path();
  				if (path.endsWith(".pdf", Qt::CaseInsensitive) ||
  					path.endsWith(".dvi", Qt::CaseInsensitive) ||
  					path.endsWith(".odvi", Qt::CaseInsensitive) ||
  					path.endsWith(".mps", Qt::CaseInsensitive))
				{
					doDoc(fileName, 0, 0);
				}
				else
				{
					QString fn = fileName->toQString();
					QDesktopServices::openUrl(url);
				}
			}
  			break;

  		case LINK_ACTION_NAMED:
  			actionName = ((XWLinkNamed *)action)->getName();
  			if (!actionName->cmp("NextPage"))
  				gotoNextPage();
  			else if (!actionName->cmp("PrevPage"))
  				gotoPrevPage();
  			else if (!actionName->cmp("FirstPage"))
  				gotoFirstPage();
  			else if (!actionName->cmp("LastPage"))
  				gotoLastPage();
  			else if (!actionName->cmp("GoBack"))
  				goBackward();
  			else if (!actionName->cmp("GoForward"))
  				goForward();
  			break;

  		case LINK_ACTION_MOVIE:
  			if (((XWLinkMovie *)action)->hasAnnotRef())
  			{
  				doc->getXRef()->fetch(((XWLinkMovie *)action)->getAnnotRef()->num,
			    						((XWLinkMovie *)action)->getAnnotRef()->gen,
			    						&movieAnnot, 0);
  			}
  			else
  			{
  				int topPage = getPageNum();
  				doc->getCatalog()->getPage(topPage)->getAnnots(&obj1);
      			if (obj1.isArray())
      			{
      				for (int i = 0; i < obj1.arrayGetLength(); ++i)
      				{
	  					if (obj1.arrayGet(i, &movieAnnot)->isDict())
	  					{
	    					if (movieAnnot.dictLookup("Subtype", &obj2)->isName("Movie"))
	    					{
	      						obj2.free();
	      						break;
	    					}
	    					obj2.free();
	  					}
	  					movieAnnot.free();
					}
					obj1.free();
      			}
  			}

  			if (movieAnnot.isDict())
  			{
  				if (movieAnnot.dictLookup("Movie", &obj1)->isDict())
  				{
  					if (obj1.dictLookup("F", &obj2))
  					{
  						fileName = XWLinkAction::getFileSpecName(&obj2);
  						if (fileName)
  						{
  							doMovie(fileName);
  							delete fileName;
  						}
	  					obj2.free();
  					}
  				}
  				obj1.free();
  			}
  			movieAnnot.free();
  			break;

  		default:
  			break;
  	}
}

void XWGuiCore::doOutline(XWOutlineItem *item)
{
	if (!doc || !item)
     return ;

	if (item->getAction())
    doAction(item->getAction());
}

void XWGuiCore::find()
{
	XWSearchWindow d(this, this);
	d.exec();
}

void XWGuiCore::findNext()
{
	if (!searchContent || searchContentLen < 1)
		return ;

	findU(searchContent, searchContentLen, caseSensitive, true, false, wholeWord, onePageOnly);
}

void XWGuiCore::findPrev()
{
	if (!searchContent || searchContentLen < 1)
		return ;

	findU(searchContent, searchContentLen, caseSensitive, true, true, wholeWord, onePageOnly);
}

void XWGuiCore::fitToPage()
{
	if (!doc)
     return ;

	if (zoomIdx != ZOOM_PAGE_IDX)
		zoom(ZOOM_PAGE_IDX);
}

void XWGuiCore::fitToWidth()
{
	if (!doc)
     return ;

	if (zoomIdx != ZOOM_WIDTH_IDX)
		zoom(ZOOM_WIDTH_IDX);
}

void XWGuiCore::goBackward()
{
	if (!doc || historyBLen <= 1)
     return ;

	if (--historyCur < 0)
    	historyCur = DOC_HISTORYSIZE - 1;

	historyBLen--;
	++historyFLen;
	int pg = history[historyCur];
	displayPage(pg, true);
}

void XWGuiCore::goForward()
{
	if (!doc || historyFLen == 0)
     return ;

	if (++historyCur == DOC_HISTORYSIZE)
    	historyCur = 0;

  	++historyBLen;
  	--historyFLen;
  	int pg = history[historyCur];
	displayPage(pg, true);
}

void XWGuiCore::gotoDest(XWString * destName)
{
	if (!doc)
     return ;

	int pg = 0;
  XWLinkDest *dest;
  getPageAndDest(1, destName, &pg, &dest);
  if (dest)
  {
   	displayDest(dest);
   	delete dest;
  }
}

void XWGuiCore::gotoFirstPage()
{
	if (!doc)
     return ;

	if (canFirstPage())
		displayPage(1, true);
	else if (doubleMode && topPage == 2)
	{
		topPage--;
		displayPage(1, true);
	}
}

void XWGuiCore::gotoLastPage()
{
	if (!doc)
     return ;

	if (canvas->isPlaying())
		slideTimer.stop();

	if (canLastPage())
		displayPage(getNumPages(), true);
	else if (doubleMode && topPage == getNumPages() - 1)
	{
		topPage++;
		displayPage(getNumPages(), true);
	}
}

void XWGuiCore::gotoNextPage()
{
	gotoNextPage(1, true);
}

void XWGuiCore::gotoPrevPage()
{
	gotoPrevPage(1, true, false);
}

void XWGuiCore::setDisplay()
{
	if (isRef)
	{
/*		if (xwApp->hasSetting("RefWinBackgroundColor"))
		{
			QVariant v = xwApp->getSetting("RefWinBackgroundColor");
			QColor color = v.value<QColor>();
			QPalette newPalette = palette();
			newPalette.setBrush(QPalette::Base, color);
			setPalette(newPalette);
			canvas->setPalette(newPalette);
		}*/

		if (xwApp->hasSetting("RefWinOpacity"))
		{
			double op = xwApp->getSetting("RefWinOpacity").toDouble();
			if (op >= 0.0 && op <= 1.0)
			{
				setWindowOpacity(op);
				canvas->setWindowOpacity(op);
			}
		}

		if (xwApp->hasSetting("ReverseVideoR"))
			reverseVideo = xwApp->getSetting("ReverseVideoR").toBool();

		if (xwApp->hasSetting("PageColorR"))
		{
			QVariant v = xwApp->getSetting("PageColorR");
			QColor pageColorA = v.value<QColor>();
			pageColor[0] = (uchar)(pageColorA.red());
			pageColor[1] = (uchar)(pageColorA.green());
			pageColor[2] = (uchar)(pageColorA.blue());
		}

		if (xwApp->hasSetting("ZoomR"))
			zoomFactor = xwApp->getSetting("ZoomR").toDouble();

		if (xwApp->hasSetting("RotateR"))
			rotate = xwApp->getSetting("RotateR").toInt();
	}
	else
	{
/*		if (xwApp->hasSetting("MainWinBackgroundColor"))
		{
			QVariant v = xwApp->getSetting("MainWinBackgroundColor");
			QColor color = v.value<QColor>();
			QPalette newPalette = palette();
			newPalette.setBrush(QPalette::Base, color);
			setPalette(newPalette);
			canvas->setPalette(newPalette);
		}*/

		if (xwApp->hasSetting("MainWinOpacity"))
		{
			double op = xwApp->getSetting("MainWinOpacity").toDouble();
			if (op >= 0.0 && op <= 1.0)
			{
				setWindowOpacity(op);
				canvas->setWindowOpacity(op);
			}
		}

		if (xwApp->hasSetting("ReverseVideo"))
			reverseVideo = xwApp->getSetting("ReverseVideo").toBool();

		if (xwApp->hasSetting("PageColor"))
		{
			QVariant v = xwApp->getSetting("PageColor");
			QColor pageColorA = v.value<QColor>();

			pageColor[0] = (uchar)(pageColorA.red());
			pageColor[1] = (uchar)(pageColorA.green());
			pageColor[2] = (uchar)(pageColorA.blue());
		}

		if (xwApp->hasSetting("Zoom"))
			zoomFactor = xwApp->getSetting("Zoom").toDouble();

		if (xwApp->hasSetting("Rotate"))
			rotate = xwApp->getSetting("Rotate").toInt();

		if (dev)
		{
			locking = true;
			delete dev;
			dev = 0;
			locking = false;
		}
	}

	if (doc)
	{
		int topPageA = topPage;
		updatePages();
		displayPage(topPageA);
	}
}

void XWGuiCore::scrollDown(int nLines)
{
	if (!doc)
     return ;

	if (getNumPages() == 0)
		return ;

	QScrollBar * hbar = horizontalScrollBar();
  QScrollBar * vbar = verticalScrollBar();
  if (!continuousMode && vbar->value() >= vbar->maximum())
  	gotoNextPage();
  else
  	scrollTo(hbar->value() , vbar->value() + nLines);
}

void XWGuiCore::scrollDownNextPage(int nLines)
{
	if (!doc)
     return ;

	if (getNumPages() == 0)
		return ;

  XWCorePage * page = 0;
  int topPageA = topPage;
  if (canNextPage())
  {
  	topPageA++;
  	page = getPage(topPageA);
  }
  else if (doubleMode && topPageA < doc->getNumPages())
  {
  	topPageA++;
  	page = getPage(topPageA);
  }

  if (page)
  {
  	QScrollBar * hbar = horizontalScrollBar();
  	scrollTo(hbar->value(), page->yDest + nLines);
  	addToHist(topPageA);
  }
}

void XWGuiCore::scrollLeft(int nCols)
{
	if (!doc)
     return ;

	if (getNumPages() == 0)
		return ;

	QScrollBar * hbar = horizontalScrollBar();
  QScrollBar * vbar = verticalScrollBar();
  scrollTo(hbar->value() - nCols , vbar->value());
}

void XWGuiCore::scrollPageDown()
{
	if (!doc)
     return ;

	if (getNumPages() == 0)
		return ;

	QScrollBar * vbar = verticalScrollBar();
	QScrollBar * hbar = horizontalScrollBar();
	if (!continuousMode && vbar->value() == vbar->maximum())
		gotoNextPage(1, true);
	else
		scrollTo(hbar->value(), vbar->value() + viewport()->height());
}

void XWGuiCore::scrollPageUp()
{
	if (!doc)
     return ;

	if (getNumPages() == 0)
		return ;

	QScrollBar * vbar = verticalScrollBar();
	QScrollBar * hbar = horizontalScrollBar();
	if (!continuousMode && vbar->value() == 0) {
    gotoPrevPage(1, false, true);
  } else {
    scrollTo(hbar->value(), vbar->value() - viewport()->height());
  }
}

void XWGuiCore::scrollRight(int nCols)
{
	if (!doc)
     return ;

	if (getNumPages() == 0)
		return ;

	QScrollBar * hbar = horizontalScrollBar();
  QScrollBar * vbar = verticalScrollBar();
  scrollTo(hbar->value() + nCols , vbar->value());
}

void XWGuiCore::scrollTo(int x, int y)
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
  xwApp->showErrs();
	xwApp->showWarns();
}

void XWGuiCore::scrollToBottomEdge()
{
	if (!doc)
     return ;

	if (getNumPages() == 0 || topPage <= 0)
		return ;

	XWCorePage * page = getPage(topPage);
	QScrollBar * hbar = horizontalScrollBar();
	scrollTo(hbar->value(), page->yDest + page->h - viewport()->height());
}

void XWGuiCore::scrollToBottomRight()
{
	if (!doc)
     return ;

	if (getNumPages() == 0)
		return ;

	int topPageA = topPage;
	if (doubleMode)
	{
		if (rightToLeft && (0 == (topPage % 1)))
			topPageA--;
		else if (!rightToLeft && (1 == (topPage % 1)))
			topPageA++;
	}

	XWCorePage * page = getPage(topPageA);
	scrollTo(page->xDest + page->w - viewport()->width(), page->yDest + page->h - viewport()->height());
}

void XWGuiCore::scrollToLeftEdge()
{
	if (!doc)
     return ;

	if (getNumPages() == 0)
		return ;

	QScrollBar * vbar = verticalScrollBar();
  scrollTo(0, vbar->value());
}

void XWGuiCore::scrollToRightEdge()
{
	if (!doc)
     return ;

	if (getNumPages() == 0)
		return ;

	int topPageA = topPage;
	if (doubleMode)
	{
		if (rightToLeft && (0 == (topPageA % 1)))
			topPageA--;
		else if (!rightToLeft && (1 == (topPageA % 1)))
			topPageA++;
	}

	XWCorePage * page = getPage(topPageA);
	QScrollBar * vbar = verticalScrollBar();
  scrollTo(page->xDest + page->w - viewport()->width(), vbar->value());
}

void XWGuiCore::scrollToTopEdge()
{
	if (!doc)
     return ;

	if (getNumPages() == 0)
		return ;

	XWCorePage * page = getPage(topPage);
	QScrollBar * hbar = horizontalScrollBar();
	scrollTo(hbar->value(), page->yDest);
}

void XWGuiCore::scrollToTopLeft()
{
	if (!doc)
     return ;

	if (getNumPages() == 0)
		return ;

	int topPageA = topPage;
	if (doubleMode)
	{
		if (rightToLeft && (1 == (topPageA % 1)))
			topPageA++;
		else if (!rightToLeft && (0 == (topPageA % 1)))
			topPageA--;
	}

	XWCorePage * page = getPage(topPageA);
  scrollTo(page->xDest, page->yDest);
}

void XWGuiCore::scrollUp(int nLines)
{
	if (!doc)
     return ;

	if (getNumPages() == 0)
		return ;

	QScrollBar * hbar = horizontalScrollBar();
  QScrollBar * vbar = verticalScrollBar();
  if (!continuousMode && vbar->value() == 0)
  	gotoPrevPage(1, false, true);
  else
		scrollTo(hbar->value(), vbar->value() - nLines);
}

void XWGuiCore::scrollUpPrevPage(int nLines)
{
	if (!doc)
     return ;

	if (getNumPages() == 0)
		return ;

	if (!continuousMode)
     gotoPrevPage(1, false, true);
  else if (topPage == 1)
  	scrollUp(nLines);
  else
  {
  	int topPageA = topPage;
  	topPageA--;
  	XWCorePage * page = getPage(topPageA);
  	QScrollBar * hbar = horizontalScrollBar();
    scrollTo(hbar->value(), page->yDest + page->h - nLines);
  }
}

void XWGuiCore::setContinuousMode(bool cm)
{
	if (continuousMode != cm)
	{
   	continuousMode = cm;
   	int topPageA = topPage;
		updatePages();
		displayPage(topPageA);
  }
}

void XWGuiCore::setDoubleMode(bool dm)
{
	if (doubleMode != dm)
	{
   	doubleMode = dm;
   	int topPageA = topPage;
		updatePages();
		displayPage(topPageA);
  }
}

void XWGuiCore::setDragPosition(const QPoint & p)
{
	lastPoint = p;
	if (showRuler)
	{
		topRuler->setCursorPos(lastPoint);
		leftRuler->setCursorPos(lastPoint);
	}
}

void XWGuiCore::setDrawGrid(bool e)
{
	if (drawGrid != e)
	{
		drawGrid = e;
		canvas->update();
	}
}

void XWGuiCore::setHoriMode(bool hm)
{
	if (horiMode != hm)
	{
		horiMode = hm;
		continuousMode = true;
		int topPageA = topPage;
		updatePages();
		displayPage(topPageA);
	}
}

void XWGuiCore::setMarkColor(const QColor & color)
{
	if (!doc)
     return ;

	markColor = color;
	if (marking && selectPage > 0)
	{
		XWCoreMarks * m = 0;
		if (pageMarks.contains(selectPage))
			m = pageMarks[selectPage];
		else
			m = new XWCoreMarks;

		m->setMarkColor(markColor);
	}
}

void XWGuiCore::setRightToLeft(bool e)
{
	if (rightToLeft != e)
	{
		rightToLeft = e;
		int topPageA = topPage;
		updatePages();
		displayPage(topPageA);
	}
}

void XWGuiCore::setRotate(int rotateA)
{
	if (rotate != rotateA)
	{
		rotate = rotateA;
		int topPageA = topPage;
		updatePages();
		displayPage(topPageA);
	}
}

void XWGuiCore::showRule(bool e)
{
	if (e != showRuler)
	{
		showRuler = e;
	  fakeWidget->setVisible(showRuler);
	  topRuler->setVisible(showRuler);
	  leftRuler->setVisible(showRuler);
	  if (showRuler)
	    setViewportMargins(RULER_BREADTH,RULER_BREADTH,0,0);
	  else
	    setViewportMargins(0,0,0,0);
	}	
}

void XWGuiCore::stop()
{
	slideTimer.stop();
	zoomIdx = ZOOM_PAGE_IDX;
	canvas = new XWDocCanvas(this);
	setWidget(canvas);
	connectCanvas();
	fullScreen = false;
}

void XWGuiCore::zoom(int idx)
{
	if (!doc)
     return ;

	if (idx == zoomIdx)
		return ;

	if (idx >= MIX_ZOOM_IDX && idx <= MAX_ZOOM_IDX)
		zoomFactor = zoomMenuInfo[idx].zoom;
	else if (idx == ZOOM_PAGE_IDX)
		zoomFactor = ZOOM_PAGE;
	else if (idx == ZOOM_WIDTH_IDX)
		zoomFactor = ZOOM_WIDTH;

	zoomIdx = idx;
	int topPageA = topPage;
	updatePages();
	displayPage(topPageA);
}

void XWGuiCore::zoomIn()
{
	if (canvas->isPlaying())
		return ;

	if (zoomIdx > MIX_ZOOM_IDX && zoomIdx <= MAX_ZOOM_IDX)
		zoom(zoomIdx - 1);
}

void XWGuiCore::zoomOut()
{
	if (canvas->isPlaying())
		return ;

	if (zoomIdx < MAX_ZOOM_IDX)
		zoom(zoomIdx + 1);
}

void XWGuiCore::computePageMax()
{
	maxPageW = 0;
	maxPageH = 0;
	for (int i = 1; i <= doc->getNumPages(); ++i)
	{
		int w = (int)((doc->getPageCropWidth(i) * dpi) / 72 + 0.5);
		int h = (int)((doc->getPageCropHeight(i) * dpi) / 72 + 0.5);
		int rot = rotate + doc->getPageRotate(i);
		if (rot >= 360)
	 			rot -= 360;
		else if (rot < 0)
	 			rot += 360;

	 	if (rot == 90 || rot == 270)
	 	{
	 		int t = w;
	 		w = h;
	 		h = t;
		}

		if (w > maxPageW)
			maxPageW = w;

		if (h > maxPageH)
			maxPageH = h;
	}
}

void XWGuiCore::connectCanvas()
{
	connect(canvas, SIGNAL(fullScreenChanged(bool)), this, SIGNAL(fullScreenChanged(bool)));
	connect(canvas, SIGNAL(copyAvailable(bool)), this, SIGNAL(copyAvailable(bool)));
}

bool XWGuiCore::copyEnable()
{
	return (hasSelection() && okToCopy());
}

QIcon XWGuiCore::createColorIcon(QColor color)
{
	QPixmap pixmap(20, 10);
	QPainter painter(&pixmap);
	painter.setPen(Qt::NoPen);
	painter.fillRect(QRect(0, 0, 20, 20), color);
	return QIcon(pixmap);
}

void XWGuiCore::drawPage(QPainter * painter, XWCorePage * page, const QRect & r)
{
	fillPage(page);
	if (!page->bitmap)
		return ;

	locking = true;
	page->drawContent(painter, r);
	if (hasSelection())
		page->drawSelect(painter, selectULX, selectULY, selectLRX, selectLRY, pageColor);

	if (drawGrid)
	  page->drawGrid(painter, r);

	if (pageMarks.contains(page->pageNo))
	{
		XWCoreMarks * m = pageMarks[page->pageNo];
		m->draw(painter);
	}

	locking = false;
}

void XWGuiCore::emitGoto()
{
	int cur = getPageNum();
	int num = getNumPages();
	emit currentChanged(cur, num);

	emit firstPageAvailable(cur > 1);
	emit lastPageAvailable(cur < num);
	emit backwardAvailable(historyBLen > 1);
	emit forwardAvailable(historyFLen != 0);
	emit nextPageAvailable(canNextPage());
	emit prevPageAvailable(canPrevPage());
}

XWString * XWGuiCore::extractText(int pg,
                                  double xMin,
                                  double yMin,
		                              double xMax,
		                              double yMax)
{
	int x0, y0, x1, y1, t;
  XWCorePage *page = pages[pg];
  XWString *s = 0;
  if (page->text)
  {
     cvtUserToDev(pg, xMin, yMin, &x0, &y0);
     cvtUserToDev(pg, xMax, yMax, &x1, &y1);
     if (x0 > x1)
     {
         t = x0; x0 = x1; x1 = t;
     }
     if (y0 > y1)
     {
         t = y0; y0 = y1; y1 = t;
     }

     s = page->text->getText(x0, y0, x1, y1);
  }
  else
  {
     XWTextOutputDev * textOut = new XWTextOutputDev(NULL, true, 0, false, false);
     if (textOut->isOk())
     {
         doc->displayPage(textOut, pg, dpi, dpi, rotate, false, true, false);
         textOut->cvtUserToDev(xMin, yMin, &x0, &y0);
         textOut->cvtUserToDev(xMax, yMax, &x1, &y1);
         if (x0 > x1)
         {
	         t = x0; x0 = x1; x1 = t;
         }
         if (y0 > y1)
         {
	         t = y0; y0 = y1; y1 = t;
         }
         s = textOut->getText(x0, y0, x1, y1);
     }
     else
      	s = new XWString();
     delete textOut;
  }
	return s;
}

void XWGuiCore::fillPage(XWCorePage * page)
{
	if (!locking && (!page->bitmap || page->changed))
	{
		locking = true;
		if (page->bitmap)
			page->clearContent();

		if (!dev)
		{
			dev = new XWRasterOutputDev(colorMode, bitmapRowPad, reverseVideo, pageColor, bitmapTopDown, allowAntialias);
			dev->startDoc(doc->getXRef());
		}

		doc->displayPage(dev, page->pageNo, dpi, dpi, rotate, false, true, false);
		memcpy(page->ctm, dev->getDefCTM(), 6 * sizeof(double));
  	memcpy(page->ictm, dev->getDefICTM(), 6 * sizeof(double));
  	page->bitmap = dev->takeBitmap();
  	doc->processLinks(dev, page->pageNo);
  	page->links = doc->getLinks(page->pageNo);
		dev->startPage(0, NULL);
		page->changed = false;
		locking = false;
	}
}

void XWGuiCore::fillPageText(XWCorePage * page)
{
	if (!locking && !page->text)
	{
		locking = true;
		XWTextOutputDev * tdev = new XWTextOutputDev(NULL, true, 0, false, false);
		doc->displayPage(tdev, page->pageNo, dpi, dpi, rotate, false, true, false);
  	page->text = tdev->takeText();
  	memcpy(page->ctm, tdev->getDefCTM(), 6 * sizeof(double));
  	memcpy(page->ictm, tdev->getDefICTM(), 6 * sizeof(double));
  	if (!page->links)
  		page->links = doc->getLinks(page->pageNo);
  	delete tdev;
  	locking = false;
  	xwApp->showErrs();
		xwApp->showWarns();
  }
}

bool XWGuiCore::find(char *s, bool next)
{
	int len = strlen(s);
  uint * u = (uint *)malloc(len * sizeof(uint));
  for (int i = 0; i < len; ++i)
     u[i] = (uint)(s[i] & 0xff);

  bool ret = findU(u, len, next);
  free(u);
  return ret;
}

bool XWGuiCore::findU(uint *u, int len, bool next)
{
	if (len == 0)
    return false;

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  bool startAtTop = false;
  bool startAtLast = false;
  bool stopAtLast = false;
  double xMin, yMin, xMax, yMax;
  xMin = yMin = xMax = yMax = 0;
  int newSelectULX = 0;
	int newSelectULY = 0;
	int newSelectLRX = 0;
	int newSelectLRY = 0;
  int pg = topPage;
  if (next)
  {
      startAtLast = true;
  }
  else if (selectULX != selectLRX && selectULY != selectLRY)
  {
     pg = selectPage;
     if (searchBackward)
     {
        xMin = selectULX - 1;
        yMin = selectULY - 1;
     }
     else
     {
        xMin = selectULX + 1;
        yMin = selectULY + 1;
     }
  }
  else
  {
     startAtTop = true;
  }

  XWCorePage *page = getPage(pg);
  fillPageText(page);
  if (!page->text)
  {
  	QApplication::restoreOverrideCursor();
  	return false;
  }

  if (page->text->findText(u, len, startAtTop, true, startAtLast, false,
			   									 caseSensitive, searchBackward, wholeWord,
			   									 &xMin, &yMin, &xMax, &yMax))
  {
      goto found;
  }

  if (!onePageOnly)
  {
      for (pg = searchBackward ? pg - 1 : pg + 1; searchBackward ? pg >= 1 : pg <= doc->getNumPages(); pg += searchBackward ? -1 : 1)
      {
      	page = getPage(pg);
      	fillPageText(page);
      	if (!page->text)
      	{
      		QApplication::restoreOverrideCursor();
      		return false;
      	}

        if (page->text->findText(u, len, true, true, false, false,
			    										    caseSensitive, searchBackward, wholeWord,
			    										    &xMin, &yMin, &xMax, &yMax))
         {
	         goto found;
         }
      }

      for (pg = searchBackward ? doc->getNumPages() : 1; searchBackward ? pg > topPage : pg < topPage; pg += searchBackward ? -1 : 1)
      {
         page = getPage(pg);
      	 fillPageText(page);
      	 if (!page->text)
      	 {
      	 	QApplication::restoreOverrideCursor();
      	 	return false;
      		}

         if (page->text->findText(u, len, true, true, false, false,
			    										    caseSensitive, searchBackward, wholeWord,
			    										    &xMin, &yMin, &xMax, &yMax))
         {
	         goto found;
         }
      }
  }

  if (!startAtTop)
  {
     xMin = yMin = xMax = yMax = 0;
     if (next)
     {
         stopAtLast = true;
     }
     else
     {
         stopAtLast = false;
         xMax = selectLRX;
         yMax = selectLRY;
     }

     if (page->text->findText(u, len, true, false, false, stopAtLast,
			     										caseSensitive, searchBackward, wholeWord,
			     										&xMin, &yMin, &xMax, &yMax))
     {
         goto found;
     }
  }

  QApplication::restoreOverrideCursor();
  return false;

found:
	cvtDevToWindow(pg, (int)floor(xMin), (int)floor(yMin), &newSelectULX, &newSelectULY);
	cvtDevToWindow(pg, (int)ceil(xMax), (int)ceil(yMax), &newSelectLRX, &newSelectLRY);
  setSelection(pg,  newSelectULX, newSelectULY, newSelectLRX, newSelectLRY);
  copy();
  displayPage(pg);
  QApplication::restoreOverrideCursor();
  return true;
}

XWCorePage * XWGuiCore::getPage(int pg)
{
	XWCorePage * page = 0;
	if (pg <= 0 || pg > getNumPages())
		return page;

  if (continuousMode)
  {
    if (pages.contains(pg))
      page = pages[pg];
    else
    {
      page = new XWCorePage;
  		page->pageNo = pg;
  		pages[pg] = page;

      page->w = (int)((doc->getPageCropWidth(pg) * dpi) / 72 + 0.5);
    	page->h = (int)((doc->getPageCropHeight(pg) * dpi) / 72 + 0.5);
    	int rot = rotate + doc->getPageRotate(pg);
    	if (rot >= 360)
    		rot -= 360;
    	else if (rot < 0)
    		rot += 360;

    	if (rot == 90 || rot == 270)
    	{
    		int t = page->w;
    		page->w = page->h;
    		page->h = t;
    	}

      if (horiMode)
  		{
  			page->yDest = 0;
  			if (doubleMode)
  			{
  				if (0 == (pg % 2))
  					page->yDest = maxPageH + doubleModePageSpacing;

  				if (1 == (pg % 2))
  						pg++;

  				if (rightToLeft)
  				{
  					int n = getNumPages();
  					if (1 == (n % 2))
  						n++;

  					pg = n - pg;
  					page->xDest = (pg / 2) * (maxPageW + horiModePageSpacing);
  				}
  				else
  					page->xDest = (pg / 2 - 1) * (maxPageW + horiModePageSpacing);
  			}
  			else
  			{
  				if (rightToLeft)
  					page->xDest = (getNumPages() - pg) * (maxPageW + horiModePageSpacing);
  				else
  					page->xDest = (pg - 1) * (maxPageW + horiModePageSpacing);
  			}
  		}
  		else
  		{
  			page->xDest = 0;
  			if (doubleMode)
  			{
  				if (1 == (pg % 2))
  				{
  					if (rightToLeft)
  						page->xDest = maxPageW + doubleModePageSpacing;

  					pg++;
  				}
  				else
  				{
  					if (!rightToLeft)
  						page->xDest = maxPageW + doubleModePageSpacing;
  				}

  				page->yDest = (pg / 2 - 1) * (maxPageH + continuousModePageSpacing);
  			}
  			else
  				page->yDest = (pg - 1) * (maxPageH + continuousModePageSpacing);
  		}
    }
  }
  else
  {
    if (pages.contains(pg))
      page = pages[pg];
    else
    {
      page = new XWCorePage;
  		page->pageNo = pg;
  		pages[pg] = page;

      page->w = (int)((doc->getPageCropWidth(pg) * dpi) / 72 + 0.5);
    	page->h = (int)((doc->getPageCropHeight(pg) * dpi) / 72 + 0.5);
    	int rot = rotate + doc->getPageRotate(pg);
    	if (rot >= 360)
    		rot -= 360;
    	else if (rot < 0)
    		rot += 360;

    	if (rot == 90 || rot == 270)
    	{
    		int t = page->w;
    		page->w = page->h;
    		page->h = t;
    	}
    }

    if (doubleMode)
		{
			if (horiMode)
			{
				page->xDest = 0;
        if (pg <= topPage)
          page->yDest = 0;
        else
					page->yDest = maxPageH + doubleModePageSpacing;
			}
			else
			{
				page->yDest = 0;
				if (rightToLeft)
				{
          if (pg <= topPage)
            page->xDest = maxPageW + doubleModePageSpacing;
					else
						page->xDest = 0;
				}
				else
				{
          if (pg <= topPage)
            page->xDest = 0;
					else
						page->xDest = maxPageW + doubleModePageSpacing;
				}
			}
		}
		else
		{
			page->xDest = 0;
			page->yDest = 0;
		}
  }

	return page;
}

void XWGuiCore::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape ||
		 e->key() == Qt::Key_Backspace)
	{
		if (fullScreen)
			emit fullScreenChanged(false);
	}

	QScrollArea::keyPressEvent(e);
}

void XWGuiCore::mousePressEvent(QMouseEvent *e)
{
	if (e->button() == Qt::LeftButton)
	{
		if (fullScreen)
			gotoNextPage();
	}

	if (showRuler)
	{
		topRuler->setCursorPos(e->pos());
		leftRuler->setCursorPos(e->pos());
	}

	QScrollArea::mousePressEvent(e);
}

void XWGuiCore::moveToDocBegin()
{
	gotoFirstPage();
}

void XWGuiCore::moveToDocEnd()
{
	gotoLastPage();
}

void XWGuiCore::moveToLineBegin()
{
	if (rightToLeft)
		scrollToTopEdge();
	else
		scrollToLeftEdge();
}

void XWGuiCore::moveToLineEnd()
{
	if (rightToLeft)
		scrollToBottomEdge();
	else
		scrollToRightEdge();
}

void XWGuiCore::moveToNextChar()
{
	scrollRight(2);
}

void XWGuiCore::moveToNextLine()
{
	scrollDown(16);
}

void XWGuiCore::moveToPreChar()
{
	scrollLeft(2);
}

void XWGuiCore::moveToNextPage()
{
	if (canvas->isPlaying())
		slideTimer.stop();
	gotoNextPage();
	if (canvas->isPlaying() && canNextPage())
		slideTimer.start(sildeMSec);
}

void XWGuiCore::moveToPreLine()
{
	scrollUp(16);
}

void XWGuiCore::moveToPrePage()
{
	gotoPrevPage();
}

void XWGuiCore::resizeEvent(QResizeEvent * e)
{
	if (!doc)
		return ;

	int topPageA = topPage;
	updatePages();
	displayPage(topPageA);
	topPage = topPageA;

	QScrollArea::resizeEvent(e);
}

void XWGuiCore::scrollContentsBy(int dx, int dy)
{
	if (!doc)
     return ;

	QScrollBar * hbar = horizontalScrollBar();
	QScrollBar * vbar = verticalScrollBar();
  if (!continuousMode)
  {
  	int topPageA = topPage;
  	XWCorePage * page = 0;
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
  	{
  		topPage = topPageA;
  		if (rightToLeft)
  			scrollTo(page->xDest + page->w - viewport()->width(), page->yDest);
  		else
  			scrollTo(hbar->value(), page->yDest);
  	}
  }
  else
  	scrollTo(hbar->value(), vbar->value());

	QScrollArea::scrollContentsBy(dx, dy);
}

void XWGuiCore::setDocInfo()
{
	XWObject info, obj1;
	XWString * str = 0;
	QString nv(tr("Unknow"));
	QString v = nv;
	QString tmp;
	doc->getDocInfo(&info);
	if (info.isDict() && info.dictLookup("Author", &obj1)->isString())
	{
		str = obj1.getString();
		v = str->toQString();
	}
	info.free();
	obj1.free();
	emit authorChanged(v);
	v = nv;

	doc->getDocInfo(&info);
	if (info.isDict() && info.dictLookup("CreationDate", &obj1)->isString())
	{
		str = obj1.getString();
		if (str)
		{
  			tmp = str->toQString();
  			tmp.remove(0, 2);
  			tmp = tmp.left(14);
  			QDateTime d = QDateTime::fromString(tmp, "yyyyMMddhhmmss");
  			v = d.toString("yyyy-MM-dd hh:mm:ss");
  		}
	}
	info.free();
	obj1.free();
	emit creationDateChanged(v);
	v = nv;

	doc->getDocInfo(&info);
	if (info.isDict() && info.dictLookup("Creator", &obj1)->isString())
	{
		str = obj1.getString();
		if (str)
  			v = str->toQString();
	}
	info.free();
	obj1.free();
	emit creatorChanged(v);
	v = nv;

	str = doc->getFileName();
	if (str)
	{
		tmp = str->toQString();
		XWFileName fn(tmp);
  	if (fn.isLocal())
  		v = tmp;
  	else
  	{
  		QUrl url(tmp);
  		v = url.toString(QUrl::RemoveAuthority | QUrl::RemoveQuery);
  	}
	}
	emit fileNameChanged(v);
	v = nv;

	doc->getDocInfo(&info);
	if (info.isDict() && info.dictLookup("Keywords", &obj1)->isString())
	{
		str = obj1.getString();
		if (str)
  		v = str->toQString();
	}
	info.free();
	obj1.free();
	emit keywordsChanged(v);
	v = nv;

	doc->getDocInfo(&info);
	if (info.isDict() && info.dictLookup("LastModifiedDate", &obj1)->isString())
	{
		str = obj1.getString();
		if (str)
		{
  			tmp = str->toQString();
  			tmp.remove(0, 2);
  			tmp = tmp.left(14);
  			QDateTime d = QDateTime::fromString(tmp, "yyyyMMddhhmmss");
  			v = d.toString("yyyy-MM-dd hh:mm:ss");
  		}
	}
	info.free();
	obj1.free();
	emit lastModifiedDateChanged(v);
	v = nv;

	doc->getDocInfo(&info);
	if (info.isDict() && info.dictLookup("Producer", &obj1)->isString())
	{
		str = obj1.getString();
		if (str)
  			v = str->toQString();
	}
	info.free();
	obj1.free();
	emit producerChanged(v);
	v = nv;

	doc->getDocInfo(&info);
	if (info.isDict() && info.dictLookup("Subject", &obj1)->isString())
	{
		str = obj1.getString();
		if (str)
  			v = str->toQString();
	}
	info.free();
	obj1.free();
	emit subjectChanged(v);
	v = nv;

	doc->getDocInfo(&info);
	if (info.isDict() && info.dictLookup("Title", &obj1)->isString())
	{
		str = obj1.getString();
		if (str)
		{
  			v = str->toQString();
  			emit titleChanged(v);
  		}
	}
	info.free();
	obj1.free();
	v = nv;
}


void XWGuiCore::showContextMenu(const QPoint &)
{
}

void XWGuiCore::wheelEvent(QWheelEvent * e)
{
	wheelDelta = e->delta() / 8;
	QAbstractScrollArea::wheelEvent(e);
}

void XWGuiCore::zoomToRect(int pg,
	                         double ulx,
	                         double uly,
			 										 double lrx,
			 										 double lry)
{
	int x0, y0, x1, y1, u, sx, sy;
  double rx, ry, t;
  cvtUserToWindow(pg, ulx, uly, &x0, &y0);
  cvtUserToWindow(pg, lrx, lry, &x1, &y1);
  if (x0 > x1)
  {
    u = x0;
    x0 = x1;
    x1 = u;
  }
  if (y0 > y1)
  {
    u = y0;
    y0 = y1;
    y1 = u;
  }
	int drawAreaWidth = viewport()->width();
	int drawAreaHeight = viewport()->height();
	rx = (double)drawAreaWidth / (double)(x1 - x0);
  ry = (double)drawAreaHeight / (double)(y1 - y0);
  XWCorePage* page = 0;
  if (rx < ry)
  {
    zoomFactor = rx * (dpi / (0.01 * 72));
    updatePages();
    sx = (int)(rx * x0);
    t = (drawAreaHeight * (x1 - x0)) / drawAreaWidth;
    sy = (int)(rx * (y0 + y1 - t) / 2);
    if (continuousMode)
    {
      if ((page = getPage(pg)) && page->w < maxPageW)
      {
				sx += (int)(0.5 * rx * (maxPageW - page->w));
      }
      u = (pg - 1) * continuousModePageSpacing;
      sy += (int)(rx * (page->yDest - u)) + u;
    }
  }
  else
  {
    zoomFactor = ry * (dpi / (0.01 * 72));
    updatePages();
    t = (drawAreaWidth * (y1 - y0)) / drawAreaHeight;
    sx = (int)(ry * (x0 + x1 - t) / 2);
    sy = (int)(ry * y0);
    if (continuousMode)
    {
      if ((page = getPage(pg)) && page->w < maxPageW)
      {
				sx += (int)(0.5 * rx * (maxPageW - page->w));
      }
      u = (pg - 1) * continuousModePageSpacing;
      sy += (int)(ry * (page->yDest - u)) + u;
    }
  }
  topPage = pg;
  scrollTo(sx, sy);
}

void XWGuiCore::updatePages()
{
	if (!doc)
		return ;

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	int drawAreaWidth = viewport()->width();
	int drawAreaHeight = viewport()->height();
	if (zoomFactor == ZOOM_PAGE)
	{
		if (doubleMode)
		{
			if (horiMode)
			{
				int hDPI = 72 * drawAreaWidth / maxUnscaledPageW;
				int vDPI = 72 * (drawAreaHeight - doubleModePageSpacing) / (2 * maxUnscaledPageH);
				dpi = (hDPI < vDPI) ? hDPI : vDPI;
			}
			else
			{
				int hDPI = 72 * (drawAreaWidth - doubleModePageSpacing) / (2 * maxUnscaledPageW);
				int vDPI = 72 * drawAreaHeight / maxUnscaledPageH;
				dpi = (hDPI < vDPI) ? hDPI : vDPI;
			}
		}
		else
		{
			int hDPI = 72 * drawAreaWidth / maxUnscaledPageW;
			int vDPI = 72 * drawAreaHeight / maxUnscaledPageH;
			dpi = (hDPI < vDPI) ? hDPI : vDPI;
		}
	}
	else if (zoomFactor == ZOOM_WIDTH)
	{
		int uw = doc->getPageCropWidth(topPage);
   	int uh = doc->getPageCropHeight(topPage);
   	int rot = rotate + doc->getPageRotate(topPage);
   	if (rot >= 360)
    	rot -= 360;
   	else if (rot < 0)
    	rot += 360;

    if (rot == 90 || rot == 270)
  	{
   		double ut = uw;
   		uw = uh;
   		uh = ut;
  	}

  	dpi = 72 * drawAreaWidth / uw;
	}
	else
		dpi = 0.01 * zoomFactor * 72;

	computePageMax();
	if (continuousMode)
	{
		int n = getNumPages();
		if (horiMode)
		{
			if (doubleMode)
			{
				if (1 == (n % 2))
					n++;

				n /= 2;

				totalDocW = maxPageW * n + (n - 1) * horiModePageSpacing;
				totalDocH = 2 * maxPageH + doubleModePageSpacing;
			}
			else
			{
				totalDocW = n * maxPageW + (n - 1) * horiModePageSpacing;
				totalDocH = maxPageH;
			}
		}
		else
		{
			if (doubleMode)
			{
				totalDocW = 2 * maxPageW + doubleModePageSpacing;
				if (1 == (n % 2))
					n++;

				n /= 2;
				totalDocH = maxPageH * n + (n - 1) * continuousModePageSpacing;
			}
			else
			{
				totalDocW = maxPageW;
				totalDocH = n * maxPageH + (n - 1) * continuousModePageSpacing;
			}
		}
	}
	else
	{
		if (doubleMode)
		{
			if (horiMode)
			{
				totalDocW = maxPageW;
				totalDocH = 2 * maxPageH + doubleModePageSpacing;
			}
			else
			{
				totalDocW = 2 * maxPageW + doubleModePageSpacing;
				totalDocH = maxPageH;
			}
		}
		else
		{
			totalDocW = maxPageW;
			totalDocH = maxPageH;
		}
	}

	QHash<int, XWCorePage*>::iterator i = pages.begin();
	while (i != pages.end())
	{
		XWCorePage * page = i.value();
		page->changed = true;
		i++;
	}

	canvas->adjustSize();
	emit fileLoaded();
	setDocInfo();
	QApplication::restoreOverrideCursor();
}

void XWGuiCore::addToHist(int pg)
{
	if (++historyCur == DOC_HISTORYSIZE)
		historyCur = 0;

	history[historyCur] = pg;
	if (historyBLen < DOC_HISTORYSIZE)
      	++historyBLen;

    historyFLen = 0;
    emit backwardAvailable(true);
    emit forwardAvailable(false);
}

void XWGuiCore::clearPageMarks()
{
	QHash<int, XWCoreMarks*>::iterator i = pageMarks.begin();
  while (i != pageMarks.end())
	{
		XWCoreMarks * m = i.value();
		if (m)
			delete m;
		i = pageMarks.erase(i);
	}
}

void XWGuiCore::clearPages()
{
	QHash<int, XWCorePage*>::iterator i = pages.begin();
  while (i != pages.end())
	{
		XWCorePage * p = i.value();
		if (p)
			delete p;
		i = pages.erase(i);
	}
}

void XWGuiCore::doDoc(XWString * filename,
	                    XWLinkDest *dest,
	                    XWString *namedDest)
{
	if (!filename)
		return ;

	QString fn = filename->toQString();
	XWDocSea sea;
	XWDoc * doc = 0;
	if (fn.contains(".pdf", Qt::CaseInsensitive))
	{
		fn = sea.findPdf(fn);
		if (fn.isEmpty())
			return ;

		QByteArray ba = QFile::encodeName(fn);
		XWString * fileNameA = new XWString(ba.data(), ba.size());
		XWPDFDoc * pdfdoc = new XWPDFDoc;
		if (!pdfdoc->load(fileNameA, 0, 0, 0))
		{
			delete pdfdoc;
			return ;
		}

		doc = pdfdoc;
	}
	else
	{
		int pg = -1;
		QString fn = filename->toQString();
		XWDVIDoc * dvidoc = 0;
		if (dest)
		{
			pg = dest->getPageNum();
			dvidoc = XWDVIDoc::findDoc(fn, pg);
			dest = 0;
		}
		else if (namedDest)
			dvidoc = XWDVIDoc::findDoc(fn, namedDest);
		else
			dvidoc = XWDVIDoc::findDoc(fn, pg);

		doc = dvidoc;
		if (!doc)
			return ;
	}

	XWReferenceWindow * win = new XWReferenceWindow(doc);
	if (dest)
		win->displayDest(dest);
	else if (namedDest)
		win->displayDest(namedDest);
	else
		win->displayPage(1);

	win->show();
}

void XWGuiCore::doMovie(XWString * filename)
{
	if (!filename)
		return ;

	XWDocSetting setting;
	XWString * cmd = setting.getMovieCommand();
	if (cmd)
	{
		runCommand(cmd, filename);
		return ;
	}

	QString fn = filename->toQString();
	XWMediaSea sea;
	bool movie = false;
	if (fn.contains(".mng", Qt::CaseInsensitive))
	{
		movie = true;
		fn = sea.findMNG(fn);
	}
	else if (fn.contains(".gif", Qt::CaseInsensitive))
	{
		movie = true;
		fn = sea.findGIF(fn);
	}
	else
		fn = sea.findFile(fn);

	if (fn.isEmpty())
		return ;

	if (movie)
	{
		XWAnimationPlayer player(this);
		player.setFileName(fn);
		player.show();
	}
	else
	{
		XWMediaPlayer player(this);
		player.setFile(fn);
		player.show();
	}
}

XWString * XWGuiCore::mungeURL(XWString *url)
{
	static char *allowed = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                         "abcdefghijklmnopqrstuvwxyz"
                         "0123456789"
                         "-_.~/?:@&=+,#%";

	XWString * newURL = new XWString();
	char buf[4];
  	for (int i = 0; i < url->getLength(); ++i)
  	{
    	char c = url->getChar(i);
    	if (strchr(allowed, c))
      		newURL->append(c);
    	else
    	{
      		sprintf(buf, "%%%02x", c & 0xff);
      		newURL->append(buf);
    	}
  	}
  	return newURL;
}

void XWGuiCore::runCommand(XWString *cmdFmt, XWString *arg)
{
	if (!cmdFmt)
		return ;

	char * s = strstr(cmdFmt->getCString(), "%s");
	XWString *cmd = 0;
	if (s)
	{
		cmd = mungeURL(arg);
    	cmd->insert(0, cmdFmt->getCString(),
		s - cmdFmt->getCString());
    	cmd->append(s + 2);
	}
	else
		cmd = cmdFmt->copy();

	system(cmd->getCString());
  	delete cmd;
}

void XWGuiCore::scaleMarks(double sx, double sy)
{
	QHash<int, XWCoreMarks*>::iterator i = pageMarks.begin();
  while (i != pageMarks.end())
	{
		XWCoreMarks * m = i.value();
		m->scale(sx, sy);
		i++;
	}
}
