/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTEXDOCUMENTCORE_H
#define XWTEXDOCUMENTCORE_H

#include <QScrollArea>
#include <QString>
#include <QStringList>
#include <QList>
#include <QHash>
#include <QColor>
#include <QIcon>
#include <QRectF>
#include <QRect>
#include <QPointF>
#include <QPoint>
#include <QTimer>
#include "XWTeXDocumentType.h"

class QMenu;
class QPainter;

class XWTeXDocument;
class XWTeXDocumentCanvas;
class XWFindTextDialog;
class XWReplaceDialog;

#define XW_TEX_DOC_ZOOM_PAGE  -1
#define XW_TEX_DOC_ZOOM_WIDTH -2
#define XW_TEX_DOC_DEF_ZOOM   125

#define XW_TEX_DOC_CM_PAGE_SPACING  4
#define XW_TEX_DOC_DM_PAGE_SPACING  2
#define XW_TEX_DOC_HM_PAGE_SPACING  4

class XW_TEXDOCUMENT_EXPORT XWTeXDocumentCoreMark
{
public:
	XWTeXDocumentCoreMark();
	~XWTeXDocumentCoreMark();

	void draw(QPainter * painter);

	void markTo(const QPoint & p);

	void scale(double sx, double sy);

public:
	QColor color;
	QList<QPoint*>  points;
};

class XW_TEXDOCUMENT_EXPORT XWTeXDocumentCoreMarks
{
public:
	XWTeXDocumentCoreMarks();
	~XWTeXDocumentCoreMarks();

	void draw(QPainter * painter);

	void endMark();

	void markTo(const QPoint & p);

	void scale(double sx, double sy);
	void setMarkColor(const QColor & color);

public:
	XWTeXDocumentCoreMark * curMark;
	QList<XWTeXDocumentCoreMark*>  marks;
};

class XW_TEXDOCUMENT_EXPORT XWTeXDocumentCorePage
{
public:
		XWTeXDocumentCorePage(XWTeXDocument * docA);
		~XWTeXDocumentCorePage();

		bool contains(const QPoint & p)
			{ return ((p.x() >= xDest) && (p.x() < (xDest + w)) && (p.y() >= yDest) && (p.y() < (yDest + h))); }

		bool contains(int x, int y)
			{ return ((x >= xDest) && (x < (xDest + w)) && (y >= yDest) && (y < (yDest + h)));}

    void devToWindow(int xd, int yd, int & xw, int & yw) {xw = xd + xDest; yw = yd + yDest;}
		void draw(QPainter * painter, const QColor & pagecolor, const QRectF & rect);

		bool hitTest(const QPointF & p);

		bool intersects(const QRect & rect);

    void moveToFirstRow();
		void moveToLastRow();
		bool moveToNextChar(bool & m);
		bool moveToNextRow();
		bool moveToNextWord(bool & m);
		void moveToPageEnd();
		void moveToPageStart();
		bool moveToPreviousChar(bool & m);
		bool moveToPreviousRow();
		bool moveToPreviousWord(bool & m);
		void moveToRowEnd();
		void moveToRowStart();

		void selectFirstRow(bool all, double & minX, double & minY, double & maxX, double & maxY);
	  void selectLastRow(bool all, double & minX, double & minY, double & maxX, double & maxY);
	  bool selectNextChar(double & minX, double & minY, double & maxX, double & maxY);
	  bool selectNextRow(double & minX, double & minY, double & maxX, double & maxY);
	  bool selectNextWord(double & minX, double & minY, double & maxX, double & maxY);
	  void selectPage(double & minX, double & minY, double & maxX, double & maxY);
	  bool selectPreviousChar(double & minX, double & minY, double & maxX, double & maxY);
	  bool selectPreviousRow(double & minX, double & minY, double & maxX, double & maxY);
	  bool selectPreviousWord(double & minX, double & minY, double & maxX, double & maxY);
	  void selectRow(double & minX, double & minY, double & maxX, double & maxY);
	  void selectRowEnd(double & minX, double & minY, double & maxX, double & maxY);
	  void selectRowStart(double & minX, double & minY, double & maxX, double & maxY);
		bool selectWord(double & minX, double & minY, double & maxX, double & maxY);
	  void setSelected(const QRectF & rect);

		void windowToDev(int xw, int yw, int & xd, int & yd) {xd = xw - xDest; yd = yw - yDest;}

public:
	  XWTeXDocument * doc;
	  int pageNo;
		int xDest;
	  int yDest;
	  int w;
	  int h;
};

class XW_TEXDOCUMENT_EXPORT XWTeXDocumentCore : public QScrollArea
{
  Q_OBJECT

public:
  XWTeXDocumentCore(XWTeXDocument * docA,QWidget * parent = 0);
  virtual ~XWTeXDocumentCore();

	void addMarkingActions(QMenu * menu);

	bool cvtWindowToDev(int xw, int yw, int & pg, int & xd, int & yd);

  XWTeXDocument * document() {return doc;}

	void endMark();

  XWTeXDocumentCanvas * getCanvas() {return canvas;}
	         bool getContinuousMode() {return continuousMode;}
					 int  getDirection() {return direction;}
				 int getFontIndex() {return fontIndex;}
		QString getFileName();
		QString getInputCodecName();
					 bool getMarking() {return marking;}
            int getNumPages();
		QString getOutputCodecName();
						int getPageNum() {return topPage;}
						int getPaperIndex() {return paperIndex;}
					  int getTotalHeight() {return totalDocH;}
						int getTotalWidth() {return totalDocW;}
		static QStringList getPapers();
		static QStringList getSlideAspect();
		static QStringList getFontSizes();
         double getZoom() {return zoomFactor;}
         int getZoomIdx() {return zoomIdx;}
    static QStringList getZooms();

         bool hasSelection()
		 {return (selectULX != selectLRX && selectULY != selectLRY);}

		     bool isContinuousMode() {return continuousMode;}
				 bool isMarking() {return marking;}

				   void loadFile(const QString & filename);

           void markTo(const QPoint & p);
   virtual void moveSelection(int pg, int x, int y);

           void redraw(QPainter * painter, const QRect & r);

   virtual void saveToPic(const QString & outname,
                            const QString & fmt);

public slots:
  virtual void back();
	virtual void changeMarkColor(QAction* a);
  virtual void clearMark();
	virtual void copy();
	virtual void cut();
	virtual void del();

	virtual void displayPage(int pg);
	virtual void displayPage(int pg, bool scrollToTop);
	virtual void displayPage(int pg, double minx, double miny, double maxx,double maxy);

	virtual void ensureCursorVisible();

          void find();
					void findNext();
	virtual void fitToPage();
	virtual void fitToWidth();

	virtual void newParagraph();

	virtual void paste();

  virtual void redo();
					void repaintCursor();
					void replace();

  virtual void save();
  virtual void saveAs();
	virtual void scrollTo(int x, int y);
	        void setBlinkingCursorEnabled(bool enable);
	virtual void setContinuousMode(bool cm);
	virtual void setDirection(int d);
	virtual void setDisplay();
	        void setFileName(bool e);
	virtual void setFontSize(int idx);
	        void setInputCodec(const QString & name);
					void setLineSkip(double s);
	virtual void setMarkColor(const QColor & color);
	virtual void setMarking(bool e) {marking = e;}
	        void setOutputCodec(const QString & name);
	virtual void setPaper(int idx);
	virtual void setSlideAspect(int idx);
	        void setSlideIndex();
					void showFindDialog();
					void showReplaceDialog();

  virtual void undo();
	virtual void updateCursor(int pageno, double minx, double miny, double maxx,double maxy);

	virtual void zoom(int idx);
	virtual void zoomIn();
	virtual void zoomOut();

protected:
	virtual void changeEvent(QEvent *e);
	virtual void contextMenuEvent(QContextMenuEvent *e);
	virtual void dragEnterEvent(QDragEnterEvent *e);
	virtual void dragLeaveEvent(QDragLeaveEvent *e);
	virtual void dragMoveEvent(QDragMoveEvent *e);
	virtual void dropEvent(QDropEvent *e);
  virtual void focusInEvent(QFocusEvent *e);
  virtual void focusOutEvent(QFocusEvent *e);
	virtual void inputMethodEvent(QInputMethodEvent *e);
	QVariant inputMethodQuery(Qt::InputMethodQuery property) const;
	virtual void keyPressEvent(QKeyEvent *e);
	virtual void mouseDoubleClickEvent(QMouseEvent *e);
	virtual void mouseMoveEvent(QMouseEvent *e);
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *e);
  virtual void resizeEvent(QResizeEvent *e);
  virtual void scrollContentsBy(int dx, int dy);
	virtual void showEvent(QShowEvent *);
  virtual void timerEvent(QTimerEvent *e);
  virtual void wheelEvent(QWheelEvent *e);

	virtual void drawPage(QPainter * painter, XWTeXDocumentCorePage * page, const QRect & r);

	XWTeXDocumentCorePage * getPage(int pg);

	virtual void moveToDocBegin();
	virtual void moveToDocEnd();
	virtual void moveToLineBegin();
	virtual void moveToLineEnd();
	virtual void moveToNextChar();
	virtual void moveToNextLine();
	virtual void moveToNextPage();
	virtual void moveToNextWord();
	virtual void moveToPreChar();
	virtual void moveToPreLine();
	virtual void moveToPrePage();
	virtual void moveToPreWord();

  virtual void selectAll();
  virtual void selectEndOfLine();
	virtual void selectEndOfDocument();
	virtual void selectNextChar();
	virtual void selectNextLine();
	virtual void selectNextPage();
	virtual void selectNextWord();
	virtual void selectPreviousChar();
	virtual void selectPreviousLine();
	virtual void selectPreviousPage();
	virtual void selectPreviousWord();
	virtual void selectStartOfLine();
	virtual void selectStartOfDocument();
	virtual void selectWord();
	virtual void setSelection(XWTeXDocumentCorePage * page, double & minx,
		                      double & miny, double & maxx, double & maxy);

private slots:
	void clearPageMarks();
	void clearPages();
	QIcon createColorIcon(QColor color);

	void updatePages();

private:
	XWTeXDocument * doc;
  XWTeXDocumentCanvas * canvas;

  int zoomIdx;
  int direction;
  bool continuousMode;
  bool marking;
  double zoomFactor;

  int    totalDocW;
  int    totalDocH;
  int    topPage;
  double xScale,yScale;

	bool mightStartDrag;
	bool dragging;
	QPoint dragStartPos;

  int ulx,uly,lrx,lry;
  int selectPage;
  int selectULX;
  int selectULY;
  int selectLRX;
  int selectLRY;
  bool   lastDragLeft;
  bool   lastDragTop;
  QColor markColor;
  QPoint lastPoint;

  bool cursorOn;
	int cursorMinX;
	int cursorMinY;
	int cursorMaxX;
	int cursorMaxY;

  QColor pageColor;
	int paperIndex;
	int fontIndex;

  QHash<int, XWTeXDocumentCorePage*> pages;
  QHash<int, XWTeXDocumentCoreMarks*> pageMarks;

  int  wheelDelta;
	bool isEnableChange;

	QTimer cursorBlinkTimer;

  XWFindTextDialog * findDialog;
	XWReplaceDialog  * replaceDialog;
};

#endif //XWTEXDOCUMENTCORE_H
