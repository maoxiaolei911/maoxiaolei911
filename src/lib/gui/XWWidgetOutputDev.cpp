/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QApplication>
#include <QScrollBar>
#include <QPainter>
#include <QUrl>
#include <QDateTime>
#include "XWApplication.h"
#include "XWString.h"
#include "XWFileName.h"
#include "XWList.h"
#include "XWRaster.h"
#include "XWBitmap.h"
#include "XWRasterPattern.h"
#include "XWRasterPath.h"
#include "XWDoc.h"
#include "XWLink.h"
#include "XWTextOutputDev.h"
#include "XWDocWindow.h"
#include "XWDocCanvas.h"
#include "XWWidgetOutputDev.h"

XWCoreTile::XWCoreTile()
	:xDest(0),
	 yDest(0),
	 w(0),
	 h(0),
	 hasMark(false),
	 hasSelected(false),
	 img(0)
{
}

XWCoreTile::~XWCoreTile()
{
	if (img)
		delete img;
}

bool XWCoreTile::intersects(const QRect & rect)
{
	QRect r(xDest, yDest, w, h);
	return rect.intersects(r);
}

void XWCoreTile::mark(const QPoint & p1, const QPoint & p2, const QColor & color)
{	
	if (contains(p1) && contains(p2))
	{
		QPoint s(p1.x() - xDest, p1.y() - yDest);
		QPoint e(p2.x() - xDest, p2.y() - yDest);
		QPainter painter(img);
		QPen pen = painter.pen();
		pen.setWidth(4);
		pen.setCapStyle(Qt::RoundCap);
		pen.setJoinStyle(Qt::RoundJoin);
		pen.setColor(color);
		painter.setPen(pen);
		painter.drawLine(s, e);
		hasMark = true;
	}
	else if (contains(p1))
	{
		QPoint s(p1.x() - xDest, p1.y() - yDest);
		QPainter painter(img);
		QPen pen = painter.pen();
		pen.setWidth(4);
		pen.setCapStyle(Qt::RoundCap);
		pen.setJoinStyle(Qt::RoundJoin);
		pen.setColor(color);
		painter.setPen(pen);
		painter.drawPoint(s);
		hasMark = true;
	}
	else if (contains(p2))
	{
		QPoint e(p2.x() - xDest, p2.y() - yDest);
		QPainter painter(img);
		QPen pen = painter.pen();
		pen.setWidth(4);
		pen.setCapStyle(Qt::RoundCap);
		pen.setJoinStyle(Qt::RoundJoin);
		pen.setColor(color);
		painter.setPen(pen);
		painter.drawPoint(e);
		hasMark = true;
	}
}

void XWCoreTile::paint(QPainter * painter)
{
	painter->drawImage(QPoint(xDest, yDest), *img);
}

void XWCoreTile::xorFill(int x0, 
                         int y0, 
                         int x1, 
                         int y1,
                         uchar * paperColor)
{
	x1 = qMin(x1, xDest + w);
	y1 = qMin(y1, yDest + h);
	x0 = qMax(x0, xDest);
	y0 = qMax(y0, yDest);
	QColor pcolor(paperColor[0], paperColor[1], paperColor[2]);
	QRgb prgb = pcolor.rgb();
	for (int i = x0; i < x1; i++)
	{
		for (int j = y0; j < y1; j++)
		{
			QRgb rgb = prgb ^ (img->pixel(i - xDest, j - yDest));
			img->setPixel(i - xDest, j - yDest, rgb);
		}
	}
	
	if (hasSelected)
		hasSelected = false;
	else
		hasSelected = true;
}

XWCorePage::XWCorePage()
	:pageNo(0),
	 xDest(0),
	 yDest(0),
	 w(0),
	 h(0),
	 hasMark(false),
	 hasSelected(false),
	 bitmap(0),
	 links(0),
	 text(0)
{
}

XWCorePage::~XWCorePage()
{
    if (bitmap)
    	delete bitmap;
    	
    if (links) 
        delete links;
    
    if (text)
        delete text;
    
    while (!tiles.isEmpty())
    	delete tiles.takeFirst();
}

void XWCorePage::clear()
{
	while (!tiles.isEmpty())
    delete tiles.takeFirst();
}

void XWCorePage::clearMark()
{
	for (int i = 0; i < tiles.size(); i++)
	{
		XWCoreTile * tile = tiles[i];
		if (tile->hasMark)
			fillTitle(tile);
	}
	
	hasMark = false;
}

void XWCorePage::createTitles(int tilew, int tileh)
{
	while (!tiles.isEmpty())
    delete tiles.takeFirst();

	tilew = qMin(tilew, w);
	tileh = qMin(tileh, h);
  if (tilew >= tileh)
  {
  	int y = yDest;
  	while (y < (yDest + h))
  	{
  		int x = xDest;
  		while (x < (xDest + w))
  		{
  			XWCoreTile * tile = new XWCoreTile;
  			tile->xDest = x;
  			tile->yDest = y;
  			tile->w = qMin(tilew, xDest + w - x);
  			tile->h = qMin(tileh, yDest + h - y);
  			x += tilew;
  		
  			tiles << tile;
  		}
  	
  		y += tileh;
  	}
  }
  else
  {
  	int x = xDest;
  	while (x < (xDest + w))
  	{
  		int y = yDest;
  		while (y < (yDest + h))
  		{
  			XWCoreTile * tile = new XWCoreTile;
  			tile->xDest = x;
  			tile->yDest = y;
  			tile->w = qMin(tilew, xDest + w - x);
  			tile->h = qMin(tileh, yDest + h - y);
  			y += tileh;
  		
  			tiles << tile;
  		}
  		
  		x += tilew;
  	}
  }
}

void XWCorePage::fillTitle(XWCoreTile * tile)
{
	if (tile->img)
		delete tile->img;
		
	tile->img = new QImage(tile->w, tile->h, QImage::Format_RGB32);
	int xd = tile->xDest - xDest;
	int yd = tile->yDest - yDest;
	uchar pixel[4];
	for (int x = 0; x < tile->w; x++)
	{
		for (int y = 0; y < tile->h; y++)
		{
			bitmap->getPixel(x + xd, y + yd, pixel);
			tile->img->setPixel(x, y, qRgb(pixel[0], pixel[1], pixel[2]));
		}
	}
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

void XWCorePage::mark(const QPoint & p1, const QPoint & p2, const QColor & color)
{
	for (int i = 0; i < tiles.size(); i++)
	{
		XWCoreTile * tile = tiles[i];
		if (!tile->img)
			fillTitle(tile);
		tile->mark(p1, p2, color);
		if (tile->hasMark)
			hasMark = true;
	}
}

void XWCorePage::paint(QPainter * painter, const QRect & rect, int pgno)
{
	if ((pgno > 0 && pgno != pageNo && !hasMark && !hasSelected) || 
		  (!intersects(rect) && !hasMark && !hasSelected))
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
		
		for (int i = 0; i < tiles.size(); i++)
		{
			XWCoreTile * tile = tiles[i];
			if (tile->img)
			{
				delete tile->img;
				tile->img = 0;
			}
		}
		
		return ;
	}
	
	for (int i = 0; i < tiles.size(); i++)
	{
		XWCoreTile * tile = tiles[i];
		if (tile->intersects(rect))
		{
			if (!tile->img)
				fillTitle(tile);
			tile->paint(painter);
		}
		else if (tile->img && !tile->hasMark)
		{
			delete tile->img;
			tile->img = 0;
		}
	}
}

void XWCorePage::xorFill(int x0, 
                         int y0, 
                         int x1, 
                         int y1,
                         uchar * paperColor)
{
	QRect r(x0, y0, x1, y1);
	hasSelected = false;
	for (int i = 0; i < tiles.size(); i++)
	{
		XWCoreTile * tile = tiles[i];
		if (tile->intersects(r))
		{
			if (!tile->img)
				fillTitle(tile);
				
			tile->xorFill(x0, y0, x1, y1, paperColor);
			if (tile->hasSelected)
				hasSelected = true;
		}
	}
}

XWWidgetOutputDev::XWWidgetOutputDev(XWDocWindow * scrollAreaA)
	:scrollArea(scrollAreaA),
	 doc(0),
	 fullScreen(false),
	 continuousMode(true),
	 doubleMode(false),
	 marking(false),
	 zoom(ZOOM_WIDTH),
	 rotate(0),
	 tileH(16),
	 tileW(-1),
	 drawAreaWidth(0),
	 drawAreaHeight(0),
	 maxUnscaledPageW(0),
	 maxUnscaledPageH(0),
	 maxPageW(0),
	 totalDocH(0),
	 topPage(0),
	 dpi(72),
	 selectPage(0),
	 selectULX(0),
	 selectULY(0),
	 selectLRX(0),
	 selectLRY(0),
	 lastDragLeft(0),
	 lastDragTop(0)
{
	markColor = Qt::red;
	startDoc(0);
}

XWWidgetOutputDev::~XWWidgetOutputDev()
{
	clear();
	if (doc)
		delete doc;
}

bool XWWidgetOutputDev::canFirstPage()
{
	if (!doc || doc->getNumPages() <= 1)
		return false;
		
	return topPage > 1;
}

bool XWWidgetOutputDev::canLastPage()
{
	return canNextPage();
}

bool XWWidgetOutputDev::canNextPage()
{
	if (!doc || doc->getNumPages() <= 1)
		return false;
		
	return topPage < doc->getNumPages();
}

bool XWWidgetOutputDev::canPrevPage()
{
	if (!doc || doc->getNumPages() <= 1)
		return false;
		
	return topPage > 1;
}

void XWWidgetOutputDev::clear()
{
  QHash<int, XWCorePage*>::iterator i = pages.begin();
  while (i != pages.end())
	{
		XWCorePage * p = i.value();
		if (p)
			delete p;
		i = pages.erase(i);
	}
   
  maxPageW = maxPageH = totalDocH = 0;
}

void XWWidgetOutputDev::clearMark()
{
	QHash<int, XWCorePage*>::iterator i = pages.begin();
  while (i != pages.end())
	{
		XWCorePage * p = i.value();
		if (p)
			p->clearMark();
		i++;
	}
}

void XWWidgetOutputDev::createTitles(int pg, int tw, int th)
{
	XWCorePage* page = pages[pg];
	page->clear();
	page->createTitles(tw, th);
}

void XWWidgetOutputDev::cvtDevToUser(int pg, int xd, int yd, double *xu, double *yu)
{
	if (pages.contains(pg))
	{
		XWCorePage * page = pages[pg];
		xd -= page->xDest;
    yd -= page->yDest;
    *xu = page->ictm[0] * xd + page->ictm[2] * yd + page->ictm[4];
    *yu = page->ictm[1] * xd + page->ictm[3] * yd + page->ictm[5];
	}
	else
	{
		*xu = 0;
		*yu = 0;
	}
}

void XWWidgetOutputDev::cvtDevToWindow(int pg, int xd, int yd, int *xw, int *yw)
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

void XWWidgetOutputDev::cvtUserToDev(int pg, double xu, double yu, int *xd, int *yd)
{
	if (pages.contains(pg))
	{
		XWCorePage * page = pages[pg];
		*xd = (int)(page->xDest + page->ctm[0] * xu + page->ctm[2] * yu + page->ctm[4] + 0.5);
    *yd = (int)(page->yDest + page->ctm[1] * xu + page->ctm[3] * yu + page->ctm[5] + 0.5);
	}
	else
	{
		*xd = 0;
		*yd = 0;
	}
}

void XWWidgetOutputDev::cvtUserToWindow(int pg, double xu, double yu, int *xw, int *yw)
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

bool XWWidgetOutputDev::cvtWindowToDev(int xw, int yw, int *pg, int *xd, int *yd)
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

bool XWWidgetOutputDev::cvtWindowToUser(int xw, int yw, int *pg, double *xu, double *yu)
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

void XWWidgetOutputDev::displayDest(XWLinkDest *dest, 
                                    double zoomA, 
                                    int rotateA)
{
	int topPageA = 0;
  if (dest->isPageRef()) 
  {
      ObjRef pageRef = dest->getPageRef();
      topPageA = doc->findPage(pageRef.num, pageRef.gen);
  } 
  else 
      topPageA = dest->getPageNum();
    
  if (topPageA <= 0 || topPageA > doc->getNumPages()) 
      topPageA = 1;
      
  topPage = topPageA;
  if (zoomA != zoom || rotateA != rotate)
  {
  	zoom = zoomA;
  	rotate = rotateA;
  	update();
  }
  	
  XWCorePage * page = pages[topPage];
	QScrollBar * hbar = scrollArea->horizontalScrollBar();
  QScrollBar * vbar = scrollArea->verticalScrollBar(); 
        
  double scrollXA = hbar->value();
  double scrollYA = continuousMode ? page->yDest : vbar->value();
  int dx, dy;
  switch (dest->getKind())
  {
      case LINK_DEST_XYZ:
         cvtUserToDev(topPageA, dest->getLeft(), dest->getTop(), &dx, &dy);
         scrollXA = dest->getChangeLeft() ? dx : hbar->value();
         if (dest->getChangeTop()) 
             scrollYA = dy;
         else 
         {
             if (topPage <= 0) 
	             scrollYA = 0;
             else if (continuousMode) 
	             scrollYA = vbar->value() - page->yDest;
             else 
	             scrollYA = vbar->value();
         }
           
         if (continuousMode && topPage > 0) 
             scrollYA += page->yDest;
         break;
            
     case LINK_DEST_FIT:
     case LINK_DEST_FITB:
         scrollXA = 0;
         scrollYA = continuousMode ? page->yDest : 0;
         break;
            
     case LINK_DEST_FITH:
     case LINK_DEST_FITBH:
         cvtUserToDev(topPageA, 0, dest->getTop(), &dx, &dy);
         if (continuousMode && topPage > 0) 
             dy += page->yDest;
         scrollXA = 0;
         scrollYA = dy;
         break;
            
     case LINK_DEST_FITV:
     case LINK_DEST_FITBV:
         cvtUserToDev(topPageA, dest->getLeft(), 0, &dx, &dy);
         scrollXA = dx;
         scrollYA = continuousMode ? page->yDest : 0;
         break;
            
     case LINK_DEST_FITR:
         cvtUserToDev(topPageA, dest->getLeft(), dest->getTop(), &dx, &dy);
         if (continuousMode && topPage > 0) 
             dy += page->yDest;
         scrollXA = dx;
         scrollYA = dy;
         break;
  }
        
  scrollTo((int)scrollXA, (int)scrollYA);
}

void XWWidgetOutputDev::displayPage(int topPageA, 
                                    double zoomA, 
                                    int rotateA,
                                    bool scrollToTop)
{
	lastDragLeft = lastDragTop = true;    
	topPage = topPageA;	
	if (zoomA != zoom || rotateA != rotate)
	{
		zoom = zoomA;
		rotate = rotateA;
		update();
	}
	
	XWCorePage * page = pages[topPage];
	QScrollBar * hbar = scrollArea->horizontalScrollBar();
  if (scrollToTop)
  	scrollTo(hbar->value(), page->yDest);
  else
  	scrollTo(page->xDest, page->yDest);
}

XWString * XWWidgetOutputDev::extractText(int pg, 
                                          double xMin, 
                                          double yMin,
		                                      double xMax, 
		                                      double yMax)
{
	if (!doc->okToCopy(true)) 
    	return 0;
  
  int x0, y0, x1, y1, t;
  XWCorePage *page = pages[pg];
  XWString *s = 0;
  if (page)
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
    
  return  s;
}

bool XWWidgetOutputDev::find(char *s, 
		                  bool caseSensitive, 
		                  bool next, 
		                  bool backward,
		    							bool wholeWord, 
		    							bool onePageOnly)
{
	int len = strlen(s);
  uint * u = (uint *)malloc(len * sizeof(uint));
  for (int i = 0; i < len; ++i) 
     u[i] = (uint)(s[i] & 0xff);

  bool ret = findU(u, len, caseSensitive, next, backward, wholeWord, onePageOnly);
  free(u);
  return ret;
}

XWLinkAction * XWWidgetOutputDev::findLink(int pg, double x, double y)
{
  XWCorePage * page = pages[pg];
  if (page) 
     return page->links ? page->links->find(x, y) : (XWLinkAction *)0;
        	
  return 0;
}

bool XWWidgetOutputDev::findU(uint *u, 
		                   int len, 
		                   bool caseSensitive,
		     							 bool next, 
		     							 bool backward, 
		     							 bool wholeWord,
		     							 bool onePageOnly)
{
	if (len == 0) 
    return false;

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    
  bool startAtTop = false;
  bool startAtLast = false;
  bool stopAtLast = false;
  double xMin, yMin, xMax, yMax;
  xMin = yMin = xMax = yMax = 0;
  int pg = topPage;
  if (next) 
  {
      startAtLast = true;
  } 
  else if (selectULX != selectLRX && selectULY != selectLRY) 
  {
     pg = selectPage;
     if (backward) 
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
  
  XWCorePage *page = pages[pg];    
  if (!page->bitmap)
  	createPage(pg);
  	
  if (page->text->findText(u, len, startAtTop, true, startAtLast, false,
			   caseSensitive, backward, wholeWord,
			   &xMin, &yMin, &xMax, &yMax)) 
  {
      goto found;
  }
    
  if (!onePageOnly) 
  {
      XWTextOutputDev * textOut = new XWTextOutputDev(NULL, true, 0, false, false);
      if (!textOut->isOk()) 
      {
         delete textOut;
         goto notFound;
      }
        
      for (pg = backward ? pg - 1 : pg + 1; backward ? pg >= 1 : pg <= doc->getNumPages(); pg += backward ? -1 : 1) 
      {
         doc->displayPage(textOut, pg, dpi, dpi, 0, false, true, false);
         if (textOut->findText(u, len, true, true, false, false,
			    caseSensitive, backward, wholeWord,
			    &xMin, &yMin, &xMax, &yMax)) 
         {
	         delete textOut;
	         goto foundPage;
         }
      }

      for (pg = backward ? doc->getNumPages() : 1; backward ? pg > topPage : pg < topPage; pg += backward ? -1 : 1) 
      {
         doc->displayPage(textOut, pg, dpi, dpi, 0, false, true, false);
         if (textOut->findText(u, len, true, true, false, false,
			    caseSensitive, backward, wholeWord,
			    &xMin, &yMin, &xMax, &yMax)) 
         {
	         delete textOut;
	         goto foundPage;
         }
      }
      delete textOut;
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
			     caseSensitive, backward, wholeWord,
			     &xMin, &yMin, &xMax, &yMax)) 
     {
         goto found;
     }
  }
    
notFound:
  QApplication::restoreOverrideCursor();
  return false;
    
foundPage:  
	page = pages[pg]; 
	if (!page->bitmap)
  	createPage(pg);
  	
  if (!page->text->findText(u, len, true, true, false, false,
			    caseSensitive, backward, wholeWord,
			    &xMin, &yMin, &xMax, &yMax)) 
  {
     goto notFound;
  }
    
found:
  setSelection(pg, (int)floor(xMin), (int)floor(yMin), (int)ceil(xMax), (int)ceil(yMax));
  QApplication::restoreOverrideCursor();
  return true;
}

QString XWWidgetOutputDev::getAuthor()
{
	QString ret;
	if (doc)
	{
		XWObject info, obj1;	
		doc->getDocInfo(&info);
		if (info.isDict() && info.dictLookup("Author", &obj1)->isString())
		{
			XWString * str = obj1.getString();
			if (str)
  				ret = str->toQString();
		}		
		info.free();
		obj1.free();
	}
	
	return ret;
}

QString XWWidgetOutputDev::getCreationDate()
{
	QString ret;
	if (doc)
	{
		XWObject info, obj1;	
		doc->getDocInfo(&info);
		if (info.isDict() && info.dictLookup("CreationDate", &obj1)->isString())
		{
			XWString * str = obj1.getString();
			if (str)
			{
  				QString tmp = str->toQString();
  				tmp.remove(0, 2);
  				tmp = tmp.left(14);
  				QDateTime d = QDateTime::fromString(tmp, "yyyyMMddhhmmss");
  				ret = d.toString("yyyy-MM-dd hh:mm:ss");
  			}
		}		
		info.free();
		obj1.free();
	}
	
	return ret;
}

QString XWWidgetOutputDev::getCreator()
{
	QString ret;
	if (doc)
	{
		XWObject info, obj1;	
		doc->getDocInfo(&info);
		if (info.isDict() && info.dictLookup("Creator", &obj1)->isString())
		{
			XWString * str = obj1.getString();
			if (str)
  				ret = str->toQString();
		}		
		info.free();
		obj1.free();
	}
	
	return ret;
}

QString XWWidgetOutputDev::getFileName()
{
	QString ret;
	if (!doc)
		return ret;
		
	XWString * str = doc->getFileName();
	if (!str)
		return ret;
		
	QString tmp = str->toQString();
	XWFileName fn(tmp);
  if (fn.isLocal())
  	ret = tmp;
  else
  {
  	QUrl url(tmp);
  	ret = url.toString(QUrl::RemoveAuthority | QUrl::RemoveQuery);
  }
  	
  return ret;
}

QImage XWWidgetOutputDev::getImage(int pg)
{
	if (pg < 1)
		pg = 1;
	
	if (pg > getNumPages())
		pg = getNumPages();
		
	XWCorePage* page = pages[pg];
	if (!page->bitmap)
		createPage(pg);
	return page->getImage();
}

QString XWWidgetOutputDev::getKeywords()
{
	QString ret;
	if (doc)
	{
		XWObject info, obj1;	
		doc->getDocInfo(&info);
		if (info.isDict() && info.dictLookup("Keywords", &obj1)->isString())
		{
			XWString * str = obj1.getString();
			if (str)
  				ret = str->toQString();
		}		
		info.free();
		obj1.free();
	}
	
	return ret;
}

QString XWWidgetOutputDev::getLastModifiedDate()
{
	QString ret;
	if (doc)
	{
		XWObject info, obj1;	
		doc->getDocInfo(&info);
		if (info.isDict() && info.dictLookup("LastModifiedDate", &obj1)->isString())
		{
			XWString * str = obj1.getString();
			if (str)
			{
  				QString tmp = str->toQString();
  				tmp.remove(0, 2);
  				tmp = tmp.left(14);
  				QDateTime d = QDateTime::fromString(tmp, "yyyyMMddhhmmss");
  				ret = d.toString("yyyy-MM-dd hh:mm:ss");
  				
  			}
		}		
		info.free();
		obj1.free();
	}
	
	return ret;
}

int XWWidgetOutputDev::getNumPages()
{
	if (!doc)
		return 0;
		
	return doc->getNumPages();
}

void XWWidgetOutputDev::getPageAndDest(int pageA, 
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

QString XWWidgetOutputDev::getProducer()
{
	QString ret;
	if (doc)
	{
		XWObject info, obj1;	
		doc->getDocInfo(&info);
		if (info.isDict() && info.dictLookup("Producer", &obj1)->isString())
		{
			XWString * str = obj1.getString();
			if (str)
  				ret = str->toQString();
		}		
		info.free();
		obj1.free();
	}
	
	return ret;
}

bool XWWidgetOutputDev::getSelection(int * x, int *y, int * w, int * h)
{
	if (hasSelection())
		return false;

	double d = dpi;
	if (dpi == 0)
		dpi = 1;
		
	*x = qRound((double)selectULX / d);
	*y = qRound((double)selectULY / d);
	*w = qRound(((double)(selectLRX - selectULX)) / d);
	*h = qRound(((double)(selectLRY - selectULY)) / d);
	
	return true;
}

bool XWWidgetOutputDev::getSelection(int *pg, 
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

QString XWWidgetOutputDev::getSubject()
{
	QString ret;
	if (doc)
	{
		XWObject info, obj1;	
		doc->getDocInfo(&info);
		if (info.isDict() && info.dictLookup("Subject", &obj1)->isString())
		{
			XWString * str = obj1.getString();
			if (str)
  				ret = str->toQString();
		}		
		info.free();
		obj1.free();
	}
	
	return ret;
}

QString XWWidgetOutputDev::getTitle()
{
	QString ret;
	if (doc)
	{
		XWObject info, obj1;	
		doc->getDocInfo(&info);
		if (info.isDict() && info.dictLookup("Title", &obj1)->isString())
		{
			XWString * str = obj1.getString();
			if (str)
  				ret = str->toQString();
		}		
		info.free();
		obj1.free();
	}
	
	return ret;
}

bool XWWidgetOutputDev::gotoFirstPage()
{
	if (!canFirstPage()) 
    return false;
        
  displayPage(1, zoom, rotate, true);
  return true;
}

bool XWWidgetOutputDev::gotoLastPage()
{
	if (!canLastPage()) 
    return false;
        
  topPage = doc->getNumPages();
  XWCorePage* page = pages[topPage];
  scrollTo(page->xDest, page->yDest);
    
  return true;
}

bool XWWidgetOutputDev::gotoNamedDestination(XWString *dest)
{
	if (!doc) 
     return false;
        
  XWLinkDest * d = doc->findDest(dest);
  if (!d) 
     return false;
    
  displayDest(d, zoom, rotate);
  delete d;
  return true;
}

bool XWWidgetOutputDev::gotoNextPage(int inc, bool top)
{
	if (!canNextPage()) 
    return false;
        
  int pg = 0;
  if ((pg = topPage + inc) > doc->getNumPages()) 
      pg = doc->getNumPages();
  
  topPage = pg;
  XWCorePage* page = pages[topPage];
	QScrollBar * hbar = scrollArea->horizontalScrollBar();
  if (top)
  	scrollTo(hbar->value(), page->yDest);
  else
  	scrollTo(page->xDest, page->yDest);
    
  return true;
}

bool XWWidgetOutputDev::gotoPrevPage(int dec, bool top, bool bottom)
{
	if (!canPrevPage()) 
    return false;
    
  int pg = 0;
  if ((pg = topPage - dec) < 1) 
     pg = 1;
     
  topPage = pg;
  XWCorePage* page = pages[topPage];
	QScrollBar * hbar = scrollArea->horizontalScrollBar();
  if (top)
  	scrollTo(hbar->value(), page->yDest);
  else if (bottom)
  	scrollTo(hbar->value(), page->yDest + page->h);
  else
  	scrollTo(page->xDest, page->yDest);
        
  return true;
}

void XWWidgetOutputDev::markTo(const QPoint & p)
{
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
		page->mark(lastPoint, p, markColor);
		XWDocCanvas * canvas = scrollArea->getCanvas();
		canvas->update();
	}
	
	lastPoint = p;
}

void XWWidgetOutputDev::moveSelection(int pg, int x, int y)
{
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

bool XWWidgetOutputDev::okToAddNotes()
{
	if (!doc)
		return false;
		
	return doc->okToAddNotes(false);
}

bool XWWidgetOutputDev::okToChange()
{
	if (!doc)
		return false;
		
	return doc->okToChange(false);
}

bool XWWidgetOutputDev::okToCopy()
{
	if (!doc)
		return false;
		
	return doc->okToCopy(false);
}

bool XWWidgetOutputDev::okToPrint()
{
	if (!doc)
		return false;
		
	return doc->okToPrint(false);
}

void XWWidgetOutputDev::redrawWindow(const QRect & r, bool )
{
	if (getNumPages() == 0 || topPage < 1)
		return ;
	
	XWDocCanvas * canvas = scrollArea->getCanvas();
	QPainter painter(canvas);
	QHash<int, XWCorePage*>::iterator i = pages.begin();
	while (i != pages.end())
	{
		XWCorePage * page = i.value();
		int pg = topPage;
		if (continuousMode && page->intersects(r))
		{
			if (!(page->bitmap))
				createPage(i.key());
			
			pg = -1;
			
			if (page->pageNo != topPage)
			{
				QRect rect(page->xDest, page->yDest, page->w, page->h);
				QRect tmpr = rect.intersected(r);
				if ((tmpr.height() >= (drawAreaHeight / 2)) && 
					  (tmpr.width() >= (r.width() / 2)))
				{
					scrollArea->setCurrentPage(page->pageNo);
				}
			}
		}
		else if (!doubleMode)
		{
			if (page->pageNo == topPage)
			{
				if (!(page->bitmap))
					createPage(i.key());
			}
		}
		else
		{
			if (0 == (topPage % 2))
			{
				if (page->pageNo == topPage || 
					 page->pageNo == (topPage - 1))
				{
					if (!(page->bitmap))
						createPage(i.key());
						
					pg = page->pageNo;
				}
			}
			else
			{
				if (page->pageNo == topPage || 
					 page->pageNo == (topPage + 1))
				{
					if (!(page->bitmap))
						createPage(i.key());
						
					pg = page->pageNo;
				}
			}
		}
		
		page->paint(&painter, r, pg);
				
		i++;
	}
}

void XWWidgetOutputDev::scrollDownNextPage(int nLines)
{
	if (!continuousMode) 
     gotoNextPage(1, true);
  else if (!canNextPage())
  	return ;
  	
  topPage++;
  XWCorePage * page = pages[topPage];  	
  QScrollBar * hbar = scrollArea->horizontalScrollBar();
  scrollTo(hbar->value(), page->yDest + nLines);
}

void XWWidgetOutputDev::scrollDown(int nLines)
{
	QScrollBar * hbar = scrollArea->horizontalScrollBar();
  QScrollBar * vbar = scrollArea->verticalScrollBar(); 
  scrollTo(hbar->value() , vbar->value() + nLines);
}

void XWWidgetOutputDev::scrollLeft(int nCols)
{
	QScrollBar * hbar = scrollArea->horizontalScrollBar();
  QScrollBar * vbar = scrollArea->verticalScrollBar(); 
  scrollTo(hbar->value() - nCols , vbar->value());
}

void XWWidgetOutputDev::scrollPageDown()
{
	if (!continuousMode) 
    gotoNextPage(1, true);
  else if (!canNextPage())
  	return ;
  	
  topPage++;
  XWCorePage * page = pages[topPage];  
	QScrollBar * hbar = scrollArea->horizontalScrollBar();
	scrollTo(hbar->value(), page->yDest + page->h);
}

void XWWidgetOutputDev::scrollPageUp()
{
	if (!continuousMode) 
     gotoPrevPage(1, false, true);
  else if (!canPrevPage())
  	return ;
  
  topPage--;
  XWCorePage * page = pages[topPage];  
	QScrollBar * hbar = scrollArea->horizontalScrollBar();
	scrollTo(hbar->value(), page->yDest);
}

void XWWidgetOutputDev::scrollRight(int nCols)
{
	QScrollBar * hbar = scrollArea->horizontalScrollBar();
  QScrollBar * vbar = scrollArea->verticalScrollBar(); 
  scrollTo(hbar->value() + nCols , vbar->value());
}

void XWWidgetOutputDev::scrollTo(int x, int y)
{
	QScrollBar * hbar = scrollArea->horizontalScrollBar();
  QScrollBar * vbar = scrollArea->verticalScrollBar(); 
  if (x < hbar->minimum())
  	x = hbar->minimum();
  	
  if (x > hbar->maximum())
  	x = hbar->maximum();
  	
  if (y < vbar->minimum())
  	y = vbar->minimum();
  	
  if (y > vbar->maximum())
  	y = vbar->maximum();
  	
  hbar->setValue(x);
  vbar->setValue(y);
  
  if (!continuousMode)
  {
  	XWDocCanvas * canvas = scrollArea->getCanvas();
  	canvas->update();
  }
}

void XWWidgetOutputDev::scrollToBottomEdge()
{
	XWCorePage * page = pages[topPage];
	QScrollBar * hbar = scrollArea->horizontalScrollBar();
	scrollTo(hbar->value(), page->yDest + page->h);
}

void XWWidgetOutputDev::scrollToBottomRight()
{
	XWCorePage * page = pages[topPage];
	scrollTo(page->xDest + page->w, page->yDest + page->h);
}

void XWWidgetOutputDev::scrollToLeftEdge()
{
	XWCorePage * page = pages[topPage];
	QScrollBar * vbar = scrollArea->verticalScrollBar(); 
  scrollTo(page->xDest, vbar->value());
}

void XWWidgetOutputDev::scrollToRightEdge()
{
	XWCorePage * page = pages[topPage];
	QScrollBar * vbar = scrollArea->verticalScrollBar(); 
  scrollTo(page->xDest + page->w, vbar->value());
}

void XWWidgetOutputDev::scrollToTopEdge()
{
	XWCorePage * page = pages[topPage];
	QScrollBar * hbar = scrollArea->horizontalScrollBar();
	scrollTo(hbar->value(), page->yDest);
}

void XWWidgetOutputDev::scrollToTopLeft()
{
	XWCorePage * page = pages[topPage];
  scrollTo(page->xDest, page->yDest);
}

void XWWidgetOutputDev::scrollUp(int nLines)
{
	QScrollBar * hbar = scrollArea->horizontalScrollBar();
  QScrollBar * vbar = scrollArea->verticalScrollBar();  	
	scrollTo(hbar->value(), vbar->value() - nLines);
}

void XWWidgetOutputDev::scrollUpPrevPage(int nLines)
{
	if (!continuousMode) 
     gotoPrevPage(1, false, true);
  else if (topPage == 1)
  	scrollUp(nLines);
  else 
  {
  	topPage--;
  	XWCorePage * page = pages[topPage]; 	
  	QScrollBar * hbar = scrollArea->horizontalScrollBar();
    scrollTo(hbar->value(), page->yDest + page->h - nLines);
  }
}

void XWWidgetOutputDev::setContinuousMode(bool cm)
{
	if (continuousMode != cm) 
	{
   	continuousMode = cm;
   	update();
  }
}

void XWWidgetOutputDev::setDoubleMode(bool dm)
{
	if (doubleMode != dm) 
	{
   	doubleMode = dm;
   	update();
  }
}

void XWWidgetOutputDev::setDoc(XWDoc * docA)
{
	doc = docA;
	if (doc)
	{
		startDoc(doc->getXRef());
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
    
    update();
	}
	else
	{
		clear();
		startDoc(0);
	}
}

void XWWidgetOutputDev::setFullScreen(bool e)
{
	if (fullScreen != e)
	{
		fullScreen = e;
		doubleMode = false;
		if (e)
		{
			zoom = ZOOM_PAGE;
			continuousMode = false;
		}
		update();
	}
}

void XWWidgetOutputDev::setPaperColor(uchar * paperColorA)
{
	if (paperColorA[0] != paperColor[0] || 
		  paperColorA[1] != paperColor[1] || 
		  paperColorA[2] != paperColor[2])
	{
		XWRasterOutputDev::setPaperColor(paperColorA);
		update();
	}
}

void XWWidgetOutputDev::setReverseVideo(bool reverseVideoA)
{
	if (reverseVideoA != reverseVideo)
	{
		reverseVideo = reverseVideoA;
		update();
	}
}

void XWWidgetOutputDev::setRotate(int r)
{
	if (r != rotate)
	{
		rotate = r;
		update();
	}
}

void XWWidgetOutputDev::setSelection(int newSelectPage,
		                                 int newSelectULX, 
		                                 int newSelectULY,
		                                 int newSelectLRX, 
		                                 int newSelectLRY)
{
	bool haveSel = selectULX != selectLRX && selectULY != selectLRY;
	XWDocCanvas * canvas = scrollArea->getCanvas();
  
	XWCorePage * page = 0;
	if (haveSel)
	{
		page = pages[selectPage];
		xorRectangle(selectPage, selectULX, selectULY, selectLRX, selectLRY);
	}
	
	selectPage = newSelectPage;
  selectULX = newSelectULX;
  selectULY = newSelectULY;
  selectLRX = newSelectLRX;
  selectLRY = newSelectLRY;
  haveSel = selectULX != selectLRX && selectULY != selectLRY;
	if (haveSel)
		xorRectangle(selectPage, selectULX, selectULY, selectLRX, selectLRY);
	
	canvas->update();
}

void XWWidgetOutputDev::setZoom(double v)
{
	if (zoom != v)
		update();
}

XWDoc * XWWidgetOutputDev::takeDoc()
{
	XWDoc * docA = doc;
	doc = 0;
	setDoc(0);
	
	return docA;
}

void XWWidgetOutputDev::update()
{
	if (getNumPages() == 0)
		return ;
		
	clear();
	drawAreaWidth = scrollArea->width();
	drawAreaHeight = scrollArea->height();
	if (doubleMode)
	{
		if (zoom == ZOOM_PAGE)
		{
			int hDPI = ((drawAreaWidth - doubleModePageSpacing) / (2 * maxUnscaledPageW)) * 72;
			int vDPI = drawAreaHeight / maxUnscaledPageH;
			if (continuousMode)
				vDPI = ((drawAreaHeight - continuousModePageSpacing) / maxUnscaledPageH) * 72;
			dpi = (hDPI < vDPI) ? hDPI : vDPI;
		}
		else if (zoom == ZOOM_WIDTH)
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
  		dpi = ((drawAreaWidth - doubleModePageSpacing) / (2 * uw)) * 72;
		}
		else
		 dpi = 0.01 * zoom * 72;
		 
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
			
			XWCorePage * page = new XWCorePage;
			pages[i] = page;
			page->pageNo = i;
			page->w = w;
			page->h = h;
			if (0 == (i % 2))
			{
				page->xDest = maxPageW + doubleModePageSpacing;
				if (continuousMode)
				{
					page->yDest = totalDocH;
					totalDocH += maxPageH;
					if (i < doc->getNumPages()) 
	  				totalDocH += continuousModePageSpacing;
	  		}
	  		else
	  		{
	  			page->yDest = 0;
	  			totalDocH = maxPageH;
	  		}
			}
			else
			{
				page->xDest = 0;				
				if (continuousMode)
					page->yDest = totalDocH;
				else
				{
					page->yDest = 0;
					totalDocH = maxPageH;
				}
			}
				
			page->createTitles(tileW > 0 ? tileW : w, tileH);
		}
		
		maxPageW = 2 * maxPageW + doubleModePageSpacing;
	}
	else
	{
		if (zoom == ZOOM_PAGE)
		{	
			int vDPI = drawAreaHeight / maxUnscaledPageH;
			if (continuousMode)
				vDPI = ((drawAreaHeight - continuousModePageSpacing) / maxUnscaledPageH) * 72;
				
			int hDPI = 72; 
			dpi = (hDPI < vDPI) ? hDPI : vDPI;
		}
		else if (zoom == ZOOM_WIDTH)
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
  		dpi = (drawAreaWidth / uw) * 72;
		}
		else
		 	dpi = 0.01 * zoom * 72;

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
			
			XWCorePage * page = new XWCorePage;
			pages[i] = page;
			page->pageNo = i;
			page->w = w;
			page->h = h;
			page->xDest = 0;
			if (continuousMode)
			{
				page->yDest = totalDocH;
				totalDocH += h;
	  		if (i < doc->getNumPages()) 
	  			totalDocH += continuousModePageSpacing;
			}
			else
			{
				page->yDest = 0;
				if (h > totalDocH)
					totalDocH = h;
			}
			
			if (w > maxPageW) 
	  		maxPageW = w;
	  		
	  	page->createTitles(tileW > 0 ? tileW : w, tileH);
		}
	}
	
	XWDocCanvas * canvas = scrollArea->getCanvas();
	canvas->adjustSize();
	canvas->update();
}

void XWWidgetOutputDev::createPage(int pg)
{
	XWCorePage * page = pages[pg];
	doc->displayPage(this, pg, dpi, dpi, rotate, false, true, false);
	memcpy(page->ctm, getDefCTM(), 6 * sizeof(double));
  memcpy(page->ictm, getDefICTM(), 6 * sizeof(double));
	page->bitmap = takeBitmap();
	page->links = doc->getLinks(pg);
	XWTextOutputDev * textOut = new XWTextOutputDev(NULL, true, 0, false, false);
	doc->displayPage(textOut, pg, dpi, dpi, rotate, false, true, false);
  page->text = textOut->takeText();
  delete textOut;
}

void XWWidgetOutputDev::xorRectangle(int pg, 
                                     int x0, 
                                     int y0, 
                                     int x1, 
                                     int y1)
{
	XWCorePage *page = pages[pg];
	page->xorFill(x0, y0, x1, y1, paperColor);
}
