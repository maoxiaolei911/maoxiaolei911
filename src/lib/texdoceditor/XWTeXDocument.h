/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTEXDOCUMENT_H
#define XWTEXDOCUMENT_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QList>
#include <QHash>
#include <QRectF>
#include <QFont>
#include <QPainter>
#include <QUndoStack>
#include <QStack>
#include <QTextDocument>
#include "XWTeXDocumentType.h"

class QMimeData;
class QMenu;
class QAction;
class QPainter;
class XWTeXDocumentObject;
class XWTeXDocumentBlock;
class XWTeXDocumentPage;
class XWTeXDocumentCursor;
class XWTeXDocSearhList;

class XW_TEXDOCUMENT_EXPORT XWTeXDocument : public QObject
{
  Q_OBJECT

public:
  XWTeXDocument(QObject * parent = 0);
  virtual ~XWTeXDocument();

  virtual void addSelect(XWTeXDocumentBlock * block);
  virtual void append(XWTeXDocumentObject * obj);
  virtual void append(XWTeXDocumentObject * sobj,XWTeXDocumentObject*eobj);

  virtual void back() = 0;
          void bfseries();
  virtual void breakPage() = 0;

  virtual bool canInsertFromMimeData(const QMimeData * /*source*/,
                                        int /*pageno*/,
                                        const QPointF & /*p*/) {return false;}

  virtual QMenu * createContextMenu() = 0;
  virtual QMimeData * createMimeDataFromSelection() = 0;
  virtual XWTeXDocumentObject * createObject(const QString & /*key*/,
                                             const QString & /*str*/,
                                             int & /*pos*/,
                                             QObject * /*parent*/) = 0;


  void decItemLevel() {itemLevel--;}
  virtual void del() = 0;
  void drawPage(int pageno,QPainter * painter, const QColor & pagecolor, const QRectF & rect);
  void drawPic(const QString & outname, const QString & fmt,double xscale, double yscale, const QColor & pagecolor);

  void find(XWTeXDocSearhList * listA);
  void findNext();
  void footnoteSize();

  int getBibitemCount() {return ++bibitemCounter;}
  virtual double getBottom() {return bottom;}
  int getChapterCount() {return ++chapterCounter;}
  double getColumSep() {return columSep;}
  int getCurrentBibitemCount() {return bibitemCounter;}
  XWTeXDocumentBlock * getCurrentBlock();
  QColor  getCurrentBlockColor();
  QString getCurrentBlockText();
  int getCurrentChapterCount() {return chapterCounter;}
  QFont getCurrentFont();
  int getCurrentFontSize() {return currentSize;}
  int getCurrentItemCount() {return itemCounter;}
  XWTeXDocumentPage * getCurrentPage();
  int getCurrentParagraphCount() {return paragraphCounter;}
  int getCurrentSectionCount() {return sectionCounter;}
  QString getCurrentSelected();
  int getCurrentStart();
  int getCurrentSubparagraphCount() {return subparagraphCounter;}
  int getCurrentSubsectionCount() {return subsectionCounter;}
  int getCurrentSubsubsectionCount() {return subsubsectionCounter;}
  int getCurrentPageNumber() {return curPage;}
  XWTeXDocumentCursor * getCursor(XWTeXDocumentBlock * block);
  double getCursorWidth() {return cursorWidth;}
  int getDirection() {return direction;}
  QString getFileName() {return fileName;}
  QTextDocument::FindFlags getFindFlags() {return findFlags;}
  QString getFindString() {return findString;}
  double getHalfCursorWidth() {return cursorWidth/2;}
  XWTeXDocumentObject * getHead() {return head;}
  int  getHitPos();
  QString getInputCodecName();
  bool getItalic() {return italic;}
  int getItemCount() {return ++itemCounter;}
  int getItemLevel() {return itemLevel;}
  XWTeXDocumentObject * getLast() {return last;}
  virtual double getLeft() {return left;}
  int getLineMax() {return lineMax;}
  double getLineSkip() {return lineSkip;}
  XWTeXDocumentPage * getNewPage();
  QString getOutputCodecName();
  int getNumPages() {return lastPage + 1;}
  double getPageHeight() {return pageHeight;}
  double getPageWidth() {return pageWidth;}
  int getParagraphCount() {return ++paragraphCounter;}
  int getPartCount() {return ++partCounter;}
  QString getPath();
  QString getReplaceString() {return replaceString;}
  virtual double getRight() {return right;}
  int getSectionCount() {return ++sectionCounter;}
  QString getStyle() {return docStyle;}
  int getSubparagraphCount() {return ++subparagraphCounter;}
  int getSubsectionCount() {return ++subsectionCounter;}
  int getSubsubsectionCount() {return ++subsubsectionCounter;}
  virtual double getTop() {return top;}
  int getWeight() {return weight;}

  bool hasSelect();
  virtual bool hitTest(int pageno,const QPointF & p);
  void huge();
  void Huge();

  void incItemLevel() {itemLevel++;}
  virtual void insertFromMimeData(const QMimeData * /*source*/,
                                        int /*pageno*/,
                                        const QPointF & /*p*/) = 0;
  virtual void insert(XWTeXDocumentObject * obj);
  virtual void insertChildren(XWTeXDocumentObject*obj, 
                                         XWTeXDocumentObject*sobj, 
                                         XWTeXDocumentObject*eobj);
  void insertText(const QString & str);
  bool isAllSelect() {return allSelect;}
  bool isAppendix() {return isappendix;}
  bool isCurrentBlock(XWTeXDocumentBlock * block);
  bool isInPreamble() {return inPreamble;}
  bool isRegexpMatch() {return isRegexpFind;}
  bool isShowPreamble() {return showPreamble;}
  bool isTwocolumn() {return twocolumn;}

  virtual void keyInput(const QString & str);

  void large();
  void Large();
  void LARGE();
  void normal();
  void normalSize();

  void moveToFirstRow(int pageno);
  void moveToLastRow(int pageno);
  bool moveToNextChar(int pageno, bool & m);
  bool moveToNextRow(int pageno);
  bool moveToNextWord(int pageno, bool & m);
  void moveToPageEnd(int pageno);
  void moveToPageStart(int pageno);
  bool moveToPreviousChar(int pageno, bool & m);
  bool moveToPreviousRow(int pageno);
  bool moveToPreviousWord(int pageno, bool & m);
  void moveToRowEnd(int pageno);
  void moveToRowStart(int pageno);

  virtual void newParagraph()  = 0;

  virtual void open(const QString & filename);

          void popItalic();
          void popSize();
          void popWeight();

  virtual void redo();
  virtual void remove(XWTeXDocumentObject * obj);
  virtual void removeChildren(XWTeXDocumentObject**sobj, 
                                           XWTeXDocumentObject**eobj,
                                           XWTeXDocumentObject**obj);
          void replace(XWTeXDocSearhList * listA);
          void replaceNext();
          void resetBibitemCounter() {bibitemCounter=0;}
          void resetChapterCounter() {chapterCounter=0;}
          void resetItemCounter() {itemCounter=0;}
          void resetParagraphCounter() {paragraphCounter=0;}
          void resetSectionCounter() {sectionCounter=0;}
  virtual void resetSides();
  virtual void resetSelect();
          void resetSubparagraphCounter() {subparagraphCounter=0;}
          void resetSubsectionCounter() {subsectionCounter=0;}
          void resetSubsubsectionCounter() {subsubsectionCounter=0;}
          void restoreTwoColumn() {twocolumn = savedtwocolumn;}

  virtual void save();
  virtual void saveAs(const QString & filename);
  void savedTwoColumn() {savedtwocolumn = twocolumn; twocolumn = false;}
  void scriptSize();
  void selectAll();
  void selectEndOfDocument(int pageno, double & minX, double & minY, double & maxX, double & maxY);
  void selectFirstRow(int pageno,bool all, double & minX, double & minY, double & maxX, double & maxY);
  void selectLastRow(int pageno,bool all, double & minX, double & minY, double & maxX, double & maxY);
  bool selectNextChar(int pageno,double & minX, double & minY, double & maxX, double & maxY);
  bool selectNextRow(int pageno,double & minX, double & minY, double & maxX, double & maxY);
  bool selectNextWord(int pageno,double & minX, double & minY, double & maxX, double & maxY);
  void selectPage(int pageno,double & minX, double & minY, double & maxX, double & maxY);
  bool selectPreviousChar(int pageno,double & minX, double & minY, double & maxX, double & maxY);
  bool selectPreviousRow(int pageno,double & minX, double & minY, double & maxX, double & maxY);
  bool selectPreviousWord(int pageno,double & minX, double & minY, double & maxX, double & maxY);
  void selectRow(int pageno,double & minX, double & minY, double & maxX, double & maxY);
  void selectRowEnd(int pageno,double & minX, double & minY, double & maxX, double & maxY);
  void selectRowStart(int pageno,double & minX, double & minY, double & maxX, double & maxY);
  void selectStartOfDocument(int pageno, double & minX, double & minY, double & maxX, double & maxY);
  bool selectWord(int pageno,double & minX, double & minY, double & maxX, double & maxY);
  void setAppendix() {isappendix=true;}
  void setCurrentCursor(XWTeXDocumentCursor *c) {curCursor = c;}
  void setCursor(double minx, double miny, double maxx,double maxy);
  void setDirection(int d);
  void setFindCondition(const QString & str, bool casesensitivity, bool wholeword, bool regexpA);
  void setFileName(const QString & filename);
  void setFontSize(int s);
  void setHead(XWTeXDocumentObject * h) {head = h;}
  void setInputCodec(const QString & name);
  void setItalic(bool e);
  void setLast(XWTeXDocumentObject*l) {last = l;}
  void setLineSkip(double s);
  void setOutputCodec(const QString & name);
  void setPaper(double w, double h);
  void setReplaceCondition(const QString & str, const QString & bystr, 
                          bool casesensitivity, bool wholeword, 
                          bool regexpA);
  void setSavedBottom(double b);
  void setSavedTop(double t);
  void setSelected(int pageno, const QRectF & rect);
  void setShowPreamble(bool s);
  void setStyle(const QString & s) {docStyle = s;}
  void setTwoColumn(bool e);
  void small();

  void tiny();

  virtual void undo();

signals:
  void cursorChanged(int pageno, double minx, double miny, double maxx,double maxy);
  void modificationChanged(bool m);
  void pagesChanged();
  void sectionChanged();
  void setSlide();

public slots:
   virtual void copy() = 0;
   virtual void cut() = 0;

   void displayFormular();
   void formular();

   virtual void paste() = 0;

   void text();

   void pstricksConnection();
   void pstricksLabels();
   void pstricksNode();
   void pstricksObject();
   void pstricksOptionArc();
   void pstricksOptionArrow();
   void pstricksOptionCoordSystem();
   void pstricksOptionDot();
   void pstrickOptionFill();
   void pstricksOptionGrid();
   void pstricksOptionLine();
   void pstricksOptionMatrix();
   void pstricksOptionMisc();
   void pstrickOptionNode();
   void pstricksOptionPlot();
   void pstricksOptionTree();
   void pstrickPut();
   void pstricksRepetion();
   void pstricksTextBox();
   void pstricksTreeNode();

   void tikzExecuteAt();
   void tikzOptionAutomata();
   void tikzOptionBackground();
   void tikzOptionCalendar();
   void tikzOptionChain();
   void tikzOptionCircuit();
   void tikzOptionCircuitElectrical();
   void tikzOptionCircuitLogic();
   void tikzOptionDecorate();
   void tikzOptionFill();
   void tikzOptionFit();
   void tikzOptionLine();
   void tikzOptionLSystem();
   void tikzOptionMatrix();
   void tikzOptionMindmap();
   void tikzOptionNode();
   void tikzOptionNodeShape();
   void tikzOptionPath();
   void tikzOptionPetriNet();
   void tikzOptionPlot();
   void tikzOptionShadow();
   void tikzOptionShape();
   void tikzOptionSpy();
   void tikzOptionTo();
   void tikzOptionTransform();
   void tikzOptionThrough();
   void tikzOptionTree();
   void tikzOptionTurtle();
   void tikzPathAction();
   void tikzPathOperate();
   void tikzStyleGate();
   void tikzStyleGeneral();
   void tikzStyleShape();

protected:
  virtual void clearCursors();
  virtual void clearPages();
  virtual void clearSelected();

      void createPSTricksMenu(QMenu * menu);
      void createPSTricksOptionMenu(QMenu * menu);
      void createTikzPathMenu(QMenu * menu);
      void createTikzOptionMenu(QMenu * menu);

      void pageClear();

  virtual void setup(const QString & , int & , int &) = 0;

protected:
  XWTeXDocumentObject * head;
  XWTeXDocumentObject * last;
  bool isModified;
  double pageWidth,savedPageWidth;
  double pageHeight,savedPageHeight;
  bool  twocolumn;
  int savedtwocolumn;
  int direction;

  double merginWidth;
  double lineSkip;
  double columSep;

  double top;
  double bottom;
  double left;
  double right;
  double savedTop;
  double savedBottom;

  int normalsize;
  int currentSize;
  bool italic;
  int weight;

  bool showPreamble;
  bool inPreamble;
  bool isappendix;

  int partCounter;
  int chapterCounter;
  int sectionCounter;
  int subsectionCounter;
  int subsubsectionCounter;
  int paragraphCounter;
  int subparagraphCounter;

  int itemCounter, itemLevel;
  int bibitemCounter;

  QStack<int> sizeStack;
  QStack<bool> italicStack;
  QStack<int> weightStack;

  double cursorWidth;

  bool allSelect;

  int lineMax;

  int maxPageNum;
  int lastPage;
  int curPage;
  bool isTitlePage;

  XWTeXDocumentPage** pages;
  QHash<XWTeXDocumentBlock*,XWTeXDocumentCursor*> cursors;
  QList<XWTeXDocumentBlock*> selected;
  QList<XWTeXDocumentObject*> selectedObjs;
  int selectType;

  XWTeXDocumentCursor * curCursor;

  QUndoStack  * undoStack;
  QString fileName;
  QString docStyle;
  QByteArray inCodecName;
	QByteArray outCodecName;

  QString findString;
  QString replaceString;
  bool isRegexpFind;
  QTextDocument::FindFlags findFlags;
};

#endif //XWTEXDOCUMENT_H
