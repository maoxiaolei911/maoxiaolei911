/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWGUICORE_H
#define XWGUICORE_H

#include <QMutex>
#include <QThread>
#include <QScrollArea>
#include <QString>
#include <QStringList>
#include <QList>
#include <QHash>
#include <QColor>
#include <QIcon>
#include <QTimer>
#include <QPrinter>
#include <QRect>
#include <QPoint>
#include <QPixmap>

#include "XWGlobal.h"

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
class XWRasterOutputDev;
class XWOutlineItem;
class XWSlide;
class QMenu;
class QAction;
class QGridLayout;
class XWDocCanvas;
class XWGuiCore;
class XWRuler;

#define DOC_HISTORYSIZE 100


#define ZOOM_PAGE  -1
#define ZOOM_WIDTH -2
#define DEF_ZOOM   125

#define continuousModePageSpacing  4
#define doubleModePageSpacing      2
#define horiModePageSpacing        4


class XW_GUI_EXPORT XWCoreMark
{
public:
	XWCoreMark();
	~XWCoreMark();
	
	void draw(QPainter * painter);
	
	void markTo(const QPoint & p);
	
	void scale(double sx, double sy);
	
public:
	QColor color;
	QList<QPoint*>  points;
};

class XW_GUI_EXPORT XWCoreMarks
{
public:
	XWCoreMarks();
	~XWCoreMarks();
	
	void draw(QPainter * painter);
	
	void endMark();
	
	void markTo(const QPoint & p);
	
	void scale(double sx, double sy);
	void setMarkColor(const QColor & color);
	
public:
	XWCoreMark * curMark;
	QList<XWCoreMark*>  marks;
};


class XW_GUI_EXPORT XWCorePage
{
public:
    XWCorePage();
    ~XWCorePage();
    
    void clearContent();
    
    bool contains(const QPoint & p)
			{ return ((p.x() >= xDest) && (p.x() < (xDest + w)) && (p.y() > yDest) && (p.y() < (yDest + h))); }
		
		bool contains(int x, int y)
			{ return ((x >= xDest) && (x < (xDest + w)) && (y > yDest) && (y < (yDest + h)));}
    
    void drawContent(QPainter * painter, const QRect & rect);
		void drawGrid(QPainter * painter, const QRect & rect);
    void drawSelect(QPainter * painter, 
                    int x0, 
                    int y0, 
                    int x1, 
                    int y1,
                    uchar * paperColor);
    
    QImage getImage();
        
    bool intersects(const QRect & rect);
    
public:
	  int          pageNo;
		int          xDest;
	  int          yDest;
	  int          w;
	  int          h;
    XWBitmap   * bitmap;
    XWLinks    * links;
    XWTextPage * text;   
    bool         changed;
    double ctm[6];
    double ictm[6];
};

class XW_GUI_EXPORT XWGuiCore : public QScrollArea
{
	Q_OBJECT
	
public:	
	friend class XWDocCanvas;
	
    XWGuiCore(bool isRefA, QWidget * parent = 0);
    virtual ~XWGuiCore();
    
    void addMarkingActions(QMenu * menu);
    bool canFirstPage();
    bool canLastPage();
    bool canNextPage();
    bool canPrevPage();    
    virtual void cvtDevToUser(int pg, int xd, int yd, double *xu, double *yu);
    virtual void cvtDevToWindow(int pg, int xd, int yd, int *xw, int *yw);
    virtual void cvtUserToDev(int pg, double xu, double yu, int *xd, int *yd);
    virtual void cvtUserToWindow(int pg, double xu, double yu, int *xw, int *yw);
    virtual bool cvtWindowToDev(int xw, int yw, int *pg, int *xd, int *yd);
    virtual bool cvtWindowToUser(int xw, int yw, int *pg, double *xu, double *yu);
    
    void endMark();
    virtual void extractImage(const QString & basename);
    
    virtual bool find(const QString & content, 
	                  bool caseSensitiveA, 
                      bool nextA, 
                      bool backwardA,
                      bool wholeWordA,
		              bool onePageOnlyA);
	virtual bool find(char *s, 
                      bool caseSensitiveA, 
                      bool nextA, 
                      bool backwardA,
                      bool wholeWordA,
		                  bool onePageOnlyA);
	virtual XWLinkAction *findLink(int pg, double x, double y);
	virtual bool findU(uint *u, 
                       int len, 
                       bool caseSensitiveA,
		                   bool nextA, 
		                   bool backwardA, 
		                   bool wholeWordA,
		                   bool onePageOnlyA);
		               
  bool getAllowAntialias() {return allowAntialias;}
  XWDocCanvas *    getCanvas() {return canvas;}
  bool getContinuousMode() {return continuousMode;}
  virtual XWDoc *  getDoc() {return doc;}
  bool    getDoubleMode() {return doubleMode;}
  bool getFullScreen() {return fullScreen;}
  bool getHoriMode() {return horiMode;}
  QImage  getImage(int pg);
  bool    getMarking() {return marking;}
  int     getMaxPageW() {return maxPageW;}
  int     getMaxUnscaledPageH() {return maxUnscaledPageH;}
  int     getMaxUnscaledPageW() {return maxUnscaledPageW;}
  virtual int      getNumPages();
  void    getPageAndDest(int pageA, 
                         XWString *destName,
				                 int *pageOut, 
				                 XWLinkDest **destOut);
  virtual int      getPageNum() {return topPage;}
  bool getReverseVideo() {return reverseVideo;}
  bool getRightToLeft() {return rightToLeft;}
  virtual int      getRotate()  { return rotate; }
  virtual bool getSelection(int *pg, 
                              double *ulx, 
                              double *uly,
		                          double *lrx, 
		                          double *lry);
  QPixmap getSelectionPix();
  virtual QString getSelectionStr();
  
  virtual QStringList getSlides();
  int     getTotalHeight() {return totalDocH;}
  int     getTotalWidth() {return totalDocW;}
  virtual double getZoom() {return zoomFactor;}
  double  getDPI() { return dpi; }
  virtual int getZoomIdx() {return zoomIdx;}
  virtual QStringList getZooms();
  virtual bool gotoNamedDestination(XWString *dest);
  virtual bool gotoNextPage(int inc, bool top);
  virtual bool gotoPrevPage(int dec, bool top, bool bottom);
    
  virtual bool hasSelection() 
    	{return (selectULX != selectLRX && selectULY != selectLRY);}
    
  bool isContinuousMode() {return continuousMode;}
  bool isLocking() {return locking;}
  bool isMarking() {return marking;}
  
  void markTo(const QPoint & p);
  virtual void moveSelection(int pg, int x, int y);
    
  virtual bool okToAddNotes();
	virtual bool okToChange();
	virtual bool okToCopy();
	virtual bool okToPrint();
    
  virtual void play(int min, int idx);
  virtual void print(QPrinter * printer);
  
  void redraw(QPainter * painter, const QRect & r);
    
  virtual void saveDVIToPS(const QString & outname, const QList<int> & pages);
  virtual void saveToImage(const QString & outname, 
                           const QString & fmt, 
                           const QList<int> & pages);
	virtual void saveToImage(const QString & outname, 
                           const QString & fmt,
                           const QColor & bg,
                           bool reverseVideoA);
  virtual void saveToPs(const QString & outname, const QList<int> & pages);
  virtual void saveToText(const QString & outname, const QList<int> & pages);
  virtual void setDoc(XWDoc * docA);    
  virtual void setCurrentPage(int cur);
  virtual void setFullScreen(bool e);
  virtual void setSelection(int newSelectPage,
		                        int newSelectULX, 
		                        int newSelectULY,
		                        int newSelectLRX, 
		                        int newSelectLRY);
	void setTile(int tw, int th) {tileW = tw; tileH = th;}
    
signals:
	void authorChanged(const QString & a);
	void backwardAvailable(bool yes);
	void copyAvailable(bool yes);
	void creationDateChanged(const QString & d);
	void creatorChanged(const QString & c);
	void currentChanged(int cur, int num);
		
	void fileLoaded();
	void fileNameChanged(const QString & n);
	void firstPageAvailable(bool yes);
	void forwardAvailable(bool yes);
	void fullScreenChanged(bool);
	
	void keywordsChanged(const QString & kws);
	
	void lastModifiedDateChanged(const QString & d);
	void lastPageAvailable(bool yes);
	void nextPageAvailable(bool yes);
	
	void prevPageAvailable(bool yes);
	void producerChanged(const QString & p);
	
	void rotateChanged(int r);
	
	void subjectChanged(const QString & s);
	
	void titleChanged(const QString & t);
	
	void zoomChanged(int idx);
	
public slots:
	virtual void changeMarkColor(QAction* a);
	virtual void clearMark();
	virtual void copy();
	
	virtual void displayDest(XWString *namedDest);
	virtual void displayDest(XWLinkDest *dest);
	virtual void displayPage(int pg);
	
	virtual void displayPage(int pg, bool scrollToTop);
	virtual void displayPage(int pg, double xMin, double yMin, double xMax, double yMax);
	
	virtual void doAction(XWLinkAction *action);	
	virtual void doOutline(XWOutlineItem *item);
	
	virtual void find();
	virtual void findNext();
	virtual void findPrev();
	
	virtual void fitToPage();
	virtual void fitToWidth();
	
	virtual void goBackward();
	virtual void goForward();
	virtual void gotoDest(XWString * destName);
	virtual void gotoFirstPage();
	virtual void gotoLastPage();	
	virtual void gotoNextPage();
	
	virtual void gotoPrevPage();
	
	virtual void setDisplay();
	
	virtual void scrollDown(int nLines);
	virtual void scrollDownNextPage(int nLines);
	virtual void scrollLeft(int nCols);
	virtual void scrollPageDown();
	virtual void scrollPageUp();
	virtual void scrollRight(int nCols);	
	virtual void scrollTo(int x, int y);
	virtual void scrollToBottomEdge();
	virtual void scrollToBottomRight();
	virtual void scrollToLeftEdge();	
	virtual void scrollToRightEdge();	
	virtual void scrollToTopEdge();
	virtual void scrollToTopLeft();
	virtual void scrollUp(int nLines);
	virtual void scrollUpPrevPage(int nLines);
	virtual void setContinuousMode(bool cm);
	virtual void setDoubleMode(bool dm);
	void setDragPosition(const QPoint & p);
	virtual void setDrawGrid(bool e);
	virtual void setHoriMode(bool hm);
	virtual void setMarkColor(const QColor & color);
	virtual void setMarking(bool e) {marking = e;}
	virtual void setRightToLeft(bool e);
	virtual void setRotate(int rotateA);
	virtual void showRule(bool e);
	virtual void stop();
			
	virtual void zoom(int idx);
	virtual void zoomIn();
	virtual void zoomOut();

protected:
	void computePageMax();
	virtual void  connectCanvas();
	virtual bool  copyEnable();
	virtual QIcon createColorIcon(QColor color);
	
	virtual void drawPage(QPainter * painter, XWCorePage * page, const QRect & r);
	
	virtual void emitGoto();
	virtual XWString * extractText(int pg, 
                                   double xMin, 
                                   double yMin,
		                               double xMax, 
		                               double yMax);
	
	virtual void fillPage(XWCorePage * page);
	virtual void fillPageText(XWCorePage * page);
	virtual bool find(char *s, bool next);
	virtual bool findU(uint *u, int len, bool next);
	
	XWCorePage * getPage(int pg);
		
	virtual void keyPressEvent(QKeyEvent *e);
	
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void moveToDocBegin();
	virtual void moveToDocEnd();
	virtual void moveToLineBegin();
	virtual void moveToLineEnd();
	virtual void moveToNextChar();
	virtual void moveToNextLine();
	virtual void moveToNextPage();
	virtual void moveToPreChar();
	virtual void moveToPreLine();
	virtual void moveToPrePage();
	
	virtual void resizeEvent(QResizeEvent * e);
	
	virtual void scrollContentsBy(int dx, int dy);
	
	virtual void setDocInfo();
	
	virtual void showContextMenu(const QPoint &);
	
	virtual void wheelEvent(QWheelEvent * e);
	
	virtual void zoomToRect(int pg, 
	                        double ulx, 
	                        double uly,
			 										double lrx, 
			 										double lry);
	
	virtual void updatePages();
	
protected:
	void addToHist(int pg);
	
	void clearPageMarks();
	void clearPages();
	
	void doDoc(XWString * filename, 
	           XWLinkDest *dest,
	           XWString *namedDest);
	void doMovie(XWString * filename);
	
	XWString * mungeURL(XWString *url);
		
	void runCommand(XWString *cmdFmt, XWString *arg);
	
	void scaleMarks(double sx, double sy);
    
protected:
	bool isRef;
	XWRasterOutputDev * dev;
  XWDocCanvas * canvas;
    
  QTimer slideTimer;
  int sildeMSec;
    
  int zoomIdx;
    
  int historyBLen;
  int historyFLen;
  int historyCur;
  QList<int>  history;
    
  uint * searchContent;
  int searchContentLen;
  bool wholeWord;
  bool caseSensitive;
  bool searchBackward;
  bool onePageOnly;
  
  XWDoc * doc;
	bool fullScreen;	
  bool continuousMode;    
  bool doubleMode;
  bool horiMode;
  bool rightToLeft;
  bool marking;
  double zoomFactor;   
  int    rotate;
  int    tileH;
  int    tileW;
  
  double maxUnscaledPageW; 
  double maxUnscaledPageH;
  int    maxPageW;  
  int    maxPageH;  
  int    totalDocW;
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
  QColor markColor;
  QPoint lastPoint;
  
  int  colorMode;
  int  bitmapRowPad;
  bool bitmapTopDown;
  bool allowAntialias;
  bool vectorAntialias;
  bool reverseVideo;
  uchar  pageColor[4];

  QHash<int, XWCorePage*> pages;
  QHash<int, XWCoreMarks*> pageMarks;
  
  bool locking;
  int  wheelDelta;
	
	bool drawGrid;
	bool showRuler;
	QWidget * fakeWidget;
	XWRuler * topRuler;
	XWRuler * leftRuler;
	QGridLayout * layout;
};

#endif // XWGUICORE_H

