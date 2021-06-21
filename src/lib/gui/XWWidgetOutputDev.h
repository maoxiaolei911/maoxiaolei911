/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWWIDGETOUTPUTDEV_H
#define XWWIDGETOUTPUTDEV_H

#include <QHash>
#include <QList>
#include <QPoint>
#include <QScrollArea>
#include <QImage>
#include <QPainter>

#include "XWRasterOutputDev.h"

class XWString;
class XWList;
class XWBitmap;
class XWRasterPattern;
class XWBaseStream;
class XWDoc;
class XWLinks;
class XWLinkDest;
class XWLinkAction;
class XWTextWord;
class XWTextLine;
class XWTextBlock;
class XWTextPage;
class XWDocWindow;

#define ZOOM_PAGE  -1
#define ZOOM_WIDTH -2
#define DEF_ZOOM   125

#define continuousModePageSpacing  2
#define doubleModePageSpacing  2

class XW_GUI_EXPORT XWCoreTile
{
public:
	XWCoreTile();
	~XWCoreTile();
	
	bool contains(const QPoint & p)
		{ return ((p.x() >= xDest) && (p.x() < (xDest + w)) && (p.y() > yDest) && (p.y() < (yDest + h))); }
		
	bool contains(int x, int y)
		{ return ((x >= xDest) && (x < (xDest + w)) && (y > yDest) && (y < (yDest + h)));}
	
	bool intersects(const QRect & rect);
	
	void mark(const QPoint & p1, const QPoint & p2, const QColor & color);
	
	void paint(QPainter * painter);
	
	void xorFill(int x0, 
               int y0, 
               int x1, 
               int y1,
               uchar * paperColor);
	
public:
	int      xDest;
	int      yDest;
	int      w;
	int      h;
	bool     hasMark;
	bool     hasSelected;
	QImage * img;
};

class XW_GUI_EXPORT XWCorePage
{
public:
    XWCorePage();
    ~XWCorePage();
    
    void clear();    
    void clearMark();
    bool contains(const QPoint & p)
			{ return ((p.x() >= xDest) && (p.x() < (xDest + w)) && (p.y() > yDest) && (p.y() < (yDest + h))); }
		
		bool contains(int x, int y)
			{ return ((x >= xDest) && (x < (xDest + w)) && (y > yDest) && (y < (yDest + h)));}
    void createTitles(int tilew, int tileh);
    
    void fillTitle(XWCoreTile * tile);
    
    QImage getImage();
    
    bool intersects(const QRect & rect);
    
    void mark(const QPoint & p1, const QPoint & p2, const QColor & color);
    
    void paint(QPainter * painter, const QRect & rect, int pgno);
    
    void xorFill(int x0, 
                 int y0, 
                 int x1, 
                 int y1,
                 uchar * paperColor);
    
public:
	  int          pageNo;
		int          xDest;
	  int          yDest;
	  int          w;
	  int          h;
	  bool     hasMark;
	  bool     hasSelected;
    XWBitmap   * bitmap;
    XWLinks    * links;
    XWTextPage * text;    
    QList<XWCoreTile*> tiles;
    double ctm[6];
    double ictm[6];
};


class XW_GUI_EXPORT XWWidgetOutputDev : public XWRasterOutputDev
{
public:
		XWWidgetOutputDev(XWDocWindow * scrollAreaA);
    virtual ~XWWidgetOutputDev();
    
    bool canFirstPage();
    bool canLastPage();
    bool canNextPage();
    bool canPrevPage();
    virtual void clear();
    virtual void clearMark();
    void createTitles(int pg, int tw, int th);
    virtual void cvtDevToUser(int pg, int xd, int yd, double *xu, double *yu);
    virtual void cvtDevToWindow(int pg, int xd, int yd, int *xw, int *yw);
    virtual void cvtUserToDev(int pg, double xu, double yu, int *xd, int *yd);
    virtual void cvtUserToWindow(int pg, double xu, double yu, int *xw, int *yw);
    virtual bool cvtWindowToDev(int xw, int yw, int *pg, int *xd, int *yd);
    virtual bool cvtWindowToUser(int xw, int yw, int *pg, double *xu, double *yu);
    
    virtual void displayDest(XWLinkDest *dest, 
                             double zoomA, 
                             int rotateA);
    virtual void displayPage(int topPageA, 
                             double zoomA, 
                             int rotateA,
                             bool scrollToTop);		
		
		virtual XWString * extractText(int pg, 
                                   double xMin, 
                                   double yMin,
		                               double xMax, 
		                               double yMax);
		                               
		virtual bool find(char *s, 
		                  bool caseSensitive, 
		                  bool next, 
		                  bool backward,
		    							bool wholeWord, 
		    							bool onePageOnly);
		virtual XWLinkAction *findLink(int pg, double x, double y);
		virtual bool findU(uint *u, 
		                   int len, 
		                   bool caseSensitive,
		     							 bool next, 
		     							 bool backward, 
		     							 bool wholeWord,
		     							 bool onePageOnly);
		                   
		QString getAuthor();
    QString getCreationDate();
    QString getCreator();
    XWDoc * getDoc() { return doc; }
    int     getDrawAreaHeight() { return drawAreaHeight; }
    int     getDrawAreaWidth() { return drawAreaWidth; }
    QString getFileName();
    bool    getFullScreen() {return fullScreen;}    
    QImage  getImage(int pg);
    QString getKeywords();
    QString getLastModifiedDate();
    int     getMaxPageW() {return maxPageW;}
    int     getNumPages();
    void    getPageAndDest(int pageA, 
                           XWString *destName,
				                   int *pageOut, 
				                   XWLinkDest **destOut);
    int     getPageNum() {return topPage;}
    QString getProducer();
    int     getRotate() { return rotate; }
		bool    getSelection(int * x, int *y, int * w, int * h);
    virtual bool getSelection(int *pg, 
                              double *ulx, 
                              double *uly,
		                          double *lrx, 
		                          double *lry);
    QString getSubject();
    QString getTitle();
    int     getTotalHeight() {return totalDocH;}
    double  getZoom() { return zoom; }
    double  getZoomDPI() { return dpi; }
    virtual bool gotoFirstPage();
    virtual bool gotoLastPage();
    virtual bool gotoNamedDestination(XWString *dest);
    virtual bool gotoNextPage(int inc, bool top);
    virtual bool gotoPrevPage(int dec, bool top, bool bottom);
    
    bool hasSelection() 
    	{return (selectULX != selectLRX && selectULY != selectLRY);}
    	
    bool isContinuousMode() {return continuousMode;}
    bool isMarking() {return marking;}
    
    void markTo(const QPoint & p);
    virtual void moveSelection(int pg, int x, int y);
    
    bool okToAddNotes();
		bool okToChange();
		bool okToCopy();
		bool okToPrint();
		
		void redrawWindow(const QRect & r, bool needUpdate = false);
		
		virtual void scrollDownNextPage(int nLines = 16);
		virtual void scrollDown(int nLines = 16);	
		virtual void scrollLeft(int nCols = 16);	
		virtual void scrollPageDown();
		virtual void scrollPageUp();
		virtual void scrollRight(int nCols = 16);
		virtual void scrollTo(int x, int y);
		virtual void scrollToBottomEdge();
		virtual void scrollToBottomRight();
		virtual void scrollToLeftEdge();	
		virtual void scrollToRightEdge();	
		virtual void scrollToTopEdge();
		virtual void scrollToTopLeft();
		virtual void scrollUp(int nLines = 16);
		virtual void scrollUpPrevPage(int nLines = 16);
		void setContinuousMode(bool cm);
		void setDoubleMode(bool dm);
		virtual void setDoc(XWDoc * docA);
		void setDragPosition(const QPoint & p) {lastPoint = p;}
		void setFullScreen(bool e);
		virtual void setMarkColor(const QColor & color) {markColor = color;}	
		virtual void setMarking(bool e) {marking = e;}
		virtual void setPaperColor(uchar * paperColorA);
		virtual void setReverseVideo(bool reverseVideoA);
		virtual void setRotate(int r);
		virtual void setSelection(int newSelectPage,
		                          int newSelectULX, 
		                          int newSelectULY,
		                          int newSelectLRX, 
		                          int newSelectLRY);
		void setTitle(int tw, int th) {tileW = tw; tileH = th;}
		void setZoom(double v);
		                      
		virtual XWDoc *takeDoc();
		
		virtual void update();
			                			
protected:
	  virtual void createPage(int pg);
              
		virtual void xorRectangle(int pg, 
                              int x0, 
                              int y0, 
                              int x1, 
                              int y1);
	
protected:
	XWDocWindow * scrollArea;
	XWDoc * doc;
	bool fullScreen;	
  bool continuousMode;    
  bool doubleMode;
  bool marking;
  double zoom;   
  int    rotate;
  int    tileH;
  int    tileW;
    
  int    drawAreaWidth; 
  int    drawAreaHeight;
  double maxUnscaledPageW; 
  double maxUnscaledPageH;
  int    maxPageW;  
  int    maxPageH;  
  int    totalDocH;    
  int    topPage;      
  double dpi;
   
    
  int selectPage;
  int selectULX;
  int selectULY;
  int selectLRX; 
  int selectLRY;
  bool   lastDragLeft;
  bool   lastDragTop;

  QHash<int, XWCorePage*> pages;
  
  QColor markColor;
  QPoint lastPoint;
};

#endif // XWWIDGETOUTPUTDEV_H

