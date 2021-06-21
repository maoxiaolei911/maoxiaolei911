/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTEXTOUTPUTDEV_H
#define XWTEXTOUTPUTDEV_H

#include <stdio.h>
#include "XWCoreFont.h"
#include "XWOutputDev.h"


class XWString;
class XWList;
class XWCoreFont;
class XWGraphixState;
class XWUnicodeMap;
class XWLink;

class XWTextWord;
class XWTextPool;
class XWTextLine;
class XWTextLineFrag;
class XWTextBlock;
class XWTextFlow;
class XWTextWordList;
class XWTextPage;

typedef void (*XWTextOutputFunc)(void *stream, char *text, int len);

class XW_CORE_EXPORT XWTextFontInfo
{
public:
    XWTextFontInfo(XWGraphixState *state);
    ~XWTextFontInfo();
    
    XWString *getFontName() { return fontName; }

    bool isBold() { return flags & fontBold; }
    bool isFixedWidth() { return flags & fontFixedWidth; }
    bool isItalic() { return flags & fontItalic; }
    bool isSerif() { return flags & fontSerif; }
    bool isSymbolic() { return flags & fontSymbolic; }
    
    bool matches(XWGraphixState *state);

private:
    XWCoreFont *gfxFont;
    XWString *fontName;
    int flags;

    friend class XWTextWord;
    friend class XWTextPage;
};


class XW_CORE_EXPORT XWTextWord
{
public:
    XWTextWord(XWGraphixState *state, 
               int rotA, 
               double x0, 
               double y0,
	           XWTextFontInfo *fontA, 
	           double fontSizeA);
    ~XWTextWord();
    
    void addChar(XWGraphixState *state, 
                 double x, 
                 double y,
		             double dx, 
		             double dy, 
		             int charPosA, 
		             int charLen,
		             int u);
	             
	static int cmpYX(const void *p1, const void *p2);
	
	void   getBBox(double *xMinA, double *yMinA, double *xMaxA, double *yMaxA)
                   { *xMinA = xMin; *yMinA = yMin; *xMaxA = xMax; *yMaxA = yMax; }
	int    getChar(int idx) { return text[idx]; }
	void   getCharBBox(int charIdx, 
	                   double *xMinA, 
	                   double *yMinA, 
	                   double *xMaxA, 
	                   double *yMaxA);
	int    getCharLen() { return charLen; }
	int    getCharPos() { return charPos[0]; }
	void   getColor(double *r, double *g, double *b)
                   { *r = colorR; *g = colorG; *b = colorB; }
	XWTextFontInfo * getFontInfo() { return font; }
	XWString * getFontName() { return font->fontName; }
	double   getFontSize() { return fontSize; }
	int      getLength() { return len; }
	XWLink * getLink() { return link; }
	XWTextWord * getNext() { return next; }
	XWTextWord * getPrev() {return prev;}
	int    getRotation() { return rot; }
	bool   getSpaceAfter() { return spaceAfter; }
	XWString * getText();
	
	bool isUnderlined() { return underlined; }
	
	void merge(XWTextWord *word);
	
	int    primaryCmp(XWTextWord *word);
	double primaryDelta(XWTextWord *word);
	
private:
    int rot;
    double xMin, xMax;
    double yMin, yMax;
    double base;
    uint *text;
    
    double *edge;
    int len;
    int size;
    int * charPos;
    int charLen;
    
    XWTextFontInfo *font;
    double fontSize;
    bool spaceAfter;
    XWTextWord *next;
    XWTextWord *prev;
    
    double colorR, colorG, colorB;
    
    bool underlined;
    XWLink *link;

    friend class XWTextPool;
    friend class XWTextLine;
    friend class XWTextBlock;
    friend class XWTextFlow;
    friend class XWTextWordList;
    friend class XWTextPage;
};

class XW_CORE_EXPORT XWTextPool
{
public:
    XWTextPool();
    ~XWTextPool();
    
    void addWord(XWTextWord *word);
    
    int getBaseIdx(double base);
    
    XWTextWord *getPool(int baseIdx) { return pool[baseIdx - minBaseIdx]; }
    
    void setPool(int baseIdx, XWTextWord *p) { pool[baseIdx - minBaseIdx] = p; }
    
private:
    int minBaseIdx;
    int maxBaseIdx;
    
    XWTextWord **pool;
    XWTextWord *cursor;
    int cursorBaseIdx;
    
    friend class XWTextBlock;
    friend class XWTextPage;
};

class XW_CORE_EXPORT XWTextLine
{
public:
    XWTextLine(XWTextBlock *blkA, int rotA, double baseA);
    ~XWTextLine();
    
    void addWord(XWTextWord *word);
    
    static int cmpXY(const void *p1, const void *p2);
    int cmpYX(XWTextLine *line);    
    void coalesce(XWUnicodeMap *uMap);
    
    void        getBBox(double *xMinA, double *yMinA,
		                double *xMaxA, double *yMaxA)
		        { *xMinA = xMin; *yMinA = yMin; *xMaxA = xMax; *yMaxA = yMax;}
    
    XWTextWord * getLastWord() {return lastWord;}
    XWTextLine * getNext() { return next; }
    XWTextLine * getPrev() {return prev;}
    int          getRotate() { return rot; }
    XWTextBlock * getTextBlock() {return blk;}
    int          getTextLength() {return len;}
    XWTextWord * getWords() { return words; }
    
    bool isHyphenated() { return hyphenated; }
    
    int    primaryCmp(XWTextLine *line);
    double primaryDelta(XWTextLine *line);
    
    int secondaryCmp(XWTextLine *line);
    
private:
    XWTextBlock *blk;
    int rot;
    double xMin, xMax;
    double yMin, yMax;
    double base;
    XWTextWord *words;
    XWTextWord *lastWord;
    
    uint *text;
    
    double *edge;
    int *col;
    int len;
    int convertedLen;
    bool hyphenated;
    XWTextLine *next;
    XWTextLine *prev;
    
    friend class XWTextLineFrag;
    friend class XWTextBlock;
    friend class XWTextFlow;
    friend class XWTextWordList;
    friend class XWTextPage;
};

class XW_CORE_EXPORT XWTextBlock
{
public:
    XWTextBlock(XWTextPage *pageA, int rotA);
    ~XWTextBlock();
    
    void addWord(XWTextWord *word);

    static int cmpXYPrimaryRot(const void *p1, const void *p2);
    static int cmpYXPrimaryRot(const void *p1, const void *p2);
    void coalesce(XWUnicodeMap *uMap, double fixedPitch);
    
    void        getBBox(double *xMinA, double *yMinA,
		                double *xMaxA, double *yMaxA)
		        { *xMinA = xMin; *yMinA = yMin; *xMaxA = xMax; *yMaxA = yMax;}
    XWTextLine  * getLines() { return lines; }
    XWTextBlock * getNext() { return next; }
    
    
    bool isBelow(XWTextBlock *blk);
    
    int primaryCmp(XWTextBlock *blk);
    
    double secondaryDelta(XWTextBlock *blk);
    
    void updatePriMinMax(XWTextBlock *blk);
    
private:
    XWTextPage *page;
    int rot;
    double xMin, xMax;
    double yMin, yMax;
    double priMin, priMax;
    
    XWTextPool *pool;
    
    XWTextLine *lines;
    XWTextLine *curLine;
    int nLines;
    int charCount;
    int col;
    int nColumns;
    
    XWTextBlock *next;
    XWTextBlock *stackNext;

    friend class XWTextLine;
    friend class XWTextLineFrag;
    friend class XWTextFlow;
    friend class XWTextWordList;
    friend class XWTextPage;
};

class XW_CORE_EXPORT XWTextFlow
{
public:
    XWTextFlow(XWTextPage *pageA, XWTextBlock *blk);
    ~XWTextFlow();
    
    void addBlock(XWTextBlock *blk);
    
    bool blockFits(XWTextBlock *blk, XWTextBlock *);
    
    XWTextBlock * getBlocks() { return blocks; }
    XWTextFlow  * getNext() { return next; }
    
private:
    XWTextPage *page;
    double xMin, xMax;
    double yMin, yMax;
    double priMin, priMax;
    XWTextBlock *blocks;
    XWTextBlock *lastBlk;
    XWTextFlow *next;
    
    friend class XWTextWordList;
    friend class XWTextPage;
};

class XW_CORE_EXPORT XWTextWordList
{
public:
    XWTextWordList(XWTextPage *text, bool physLayout);
    ~XWTextWordList();
    
    XWTextWord *get(int idx);
    int getLength();
    
private:
    XWList *words;
};

class XW_CORE_EXPORT XWTextPage
{
public:
    XWTextPage(bool rawOrderA);
    ~XWTextPage();
    
    void addChar(XWGraphixState *state, 
                 double x, 
                 double y,
	             double dx, 
	             double dy,
	             uint c, 
	             int nBytes, 
	             uint *u, 
	             int uLen);
	void addLink(int xMin, int yMin, int xMax, int yMax, XWLink *link);
	void addUnderline(double x0, double y0, double x1, double y1);
	void addWord(XWTextWord *word);
	       
	  void beginActualText(XWGraphixState *state, uint *u, int uLen);
    void beginWord(XWGraphixState *state, double x0, double y0);
    
    void coalesce(bool physLayout, double fixedPitch, bool doHTML);
    
    void dump(void *outputStream, 
              XWTextOutputFunc outputFunc,
	          bool physLayout);
    	    
    void endActualText(XWGraphixState *state);
    void endPage();
    void endWord();
    
    bool findCharRange(int pos, 
                       int length,
		               double *xMin, 
		               double *yMin,
		               double *xMax, 
		               double *yMax);
    bool findText(uint *s, 
                  int len,
		 							bool startAtTop, 
		 							bool stopAtBottom,
		 							bool startAtLast, 
		 							bool stopAtLast,
		 							bool caseSensitive, 
		 							bool backward,
		 							bool wholeWord,
		 							double *xMin, 
		 							double *yMin,
		 							double *xMax, 
		 							double *yMax);
    
    XWTextFlow *getFlows() { return flows; }
    XWString *getText(double xMin, double yMin, double xMax, double yMax);
    
    void incCharCount(int nChars);
    
    XWTextWordList *makeWordList(bool physLayout);
    
    void startPage(XWGraphixState *state);
    
    void updateFont(XWGraphixState *state);
    
private:
    void assignColumns(XWTextLineFrag *frags, int nFrags, bool oneRot);
    
    void clear();
    
    int dumpFragment(uint *text, int len, XWUnicodeMap *uMap, XWString *s);
    
private:
  bool rawOrder;		// keep text in content stream order

  double pageWidth, pageHeight;	// width and height of current page
  XWTextWord *curWord;		// currently active string
  int charPos;			// next character position (within content
				//   stream)
  XWTextFontInfo *curFont;	// current font
  double curFontSize;		// current font size
  int nest;			// current nesting level (for Type 3 fonts)
  int nTinyChars;		// number of "tiny" chars seen so far
  bool lastCharOverlap;	// set if the last added char overlapped the
				//   previous char
  uint *actualText;		// current "ActualText" span
  int actualTextLen;
  double actualTextX0,
         actualTextY0,
         actualTextX1,
         actualTextY1;
  int actualTextNBytes;

  XWTextPool *pools[4];		// a "pool" of TextWords for each rotation
  XWTextFlow *flows;		// linked list of flows
  XWTextBlock **blocks;		// array of blocks, in yx order
  int nBlocks;			// number of blocks
  int primaryRot;		// primary rotation
  bool primaryLR;		// primary direction (true means L-to-R,
				//   false means R-to-L)
  XWTextWord *rawWords;		// list of words, in raw order (only if
				//   rawOrder is set)
  XWTextWord *rawLastWord;	// last word on rawWords list

  XWList *fonts;			// all font info objects used on this
				//   page [TextFontInfo]

  double lastFindXMin,		// coordinates of the last "find" result
         lastFindYMin;
  bool haveLastFind;

  XWList *underlines;		// [TextUnderline]
  XWList *links;			// [TextLink]
    
    friend class XWTextLine;
    friend class XWTextLineFrag;
    friend class XWTextBlock;
    friend class XWTextFlow;
    friend class XWTextWordList;
};

class XW_CORE_EXPORT XWTextOutputDev : public XWOutputDev
{
public:
    XWTextOutputDev(char *fileName, 
                    bool physLayoutA,
										double fixedPitchA, 
										bool rawOrderA,
										bool append);
		            
    XWTextOutputDev(XWTextOutputFunc func, 
                    void *stream,
		                bool physLayoutA, 
		                double fixedPitchA,
		                bool rawOrderA);
		            
    virtual ~XWTextOutputDev();
    
    virtual void beginActualText(XWGraphixState *state, uint *u, int uLen);
    virtual void beginString(XWGraphixState *, XWString *);
    
    virtual void drawChar(XWGraphixState *state, 
                          double x, 
                          double y,
			                    double dx, 
			                    double dy,
			                    double originX, 
			                    double originY,
			                    uint c, 
			                    int nBytes, 
			                    uint *u, 
			                    int uLen);
    
            void enableHTMLExtras(bool doHTMLA) { doHTML = doHTMLA; }
    virtual void endActualText(XWGraphixState *state);
    virtual void endPage();
    virtual void endString(XWGraphixState *);
    virtual void eoFill(XWGraphixState *state);
    
    virtual void fill(XWGraphixState *state);
            bool findCharRange(int pos, 
                               int length,
		                       double *xMin, 
		                       double *yMin,
		                       double *xMax, 
		                       double *yMax);
            bool findText(uint *s, 
                          int len,
			      							bool startAtTop, 
			      							bool stopAtBottom,
			      							bool startAtLast, 
			      							bool stopAtLast,
			      							bool caseSensitive, 
			      							bool backward,
			      							bool wholeWord,
			      							double *xMin, 
			      							double *yMin,
			      							double *xMax, 
			      							double *yMax);
    
    XWString *getText(double xMin, double yMin, double xMax, double yMax);
		   
		virtual void incCharCount(int nChars);
    virtual bool interpretType3Chars() { return false; }
    virtual bool isOk() { return ok; }
    
    XWTextWordList *makeWordList();
    
    virtual bool needNonText() { return false; }
    virtual bool needCharCount() { return true; }
    
    virtual void processLink(XWLink *link, XWCatalog *);
    
    virtual void restoreState(XWGraphixState * state);
    
    virtual void startPage(int pageNum, XWGraphixState *state);
    virtual void stroke(XWGraphixState *state);
    
    XWTextPage *takeText();
    
    virtual void updateFont(XWGraphixState *state);
    virtual bool upsideDown() { return true; }
    virtual bool useDrawChar() { return true; }
    
private:
    XWTextOutputFunc outputFunc;
    void *outputStream;
    bool needClose;
    XWTextPage *text;
    double fixedPitch;
    bool physLayout;
    bool rawOrder;
    bool doHTML;
    bool ok;
};

#endif // XWTEXTOUTPUTDEV_H

