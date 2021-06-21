/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWCOREFONT_H
#define XWCOREFONT_H

#include <QObject>
#include <QString>
#include "XWString.h"
#include "XWObject.h"

class XWDict;
class XWCMap;
class XWCharCodeToUnicode;
class XWFontFileTrueType;
struct CoreFontCIDWidths;
struct Base14FontMapEntry;

enum CoreFontType 
{
  //----- Gfx8BitFont
  fontUnknownType,
  fontType1,
  fontType1C,
  fontType1COT,
  fontType3,
  fontTrueType,
  fontTrueTypeOT,
  //----- GfxCIDFont
  fontCIDType0,
  fontCIDType0C,
  fontCIDType0COT,
  fontCIDType2,
  fontCIDType2OT
};


struct XW_CORE_EXPORT CoreFontCIDWidthExcep 
{
    uint first;			// this record applies to
    uint last;			//   CIDs <first>..<last>
    double width;			// char width
};

struct XW_CORE_EXPORT CoreFontCIDWidthExcepV 
{
    uint first;			// this record applies to
    uint last;			//   CIDs <first>..<last>
    double height;		// char height
    double vx, vy;		// origin position
};

struct XW_CORE_EXPORT CoreFontCIDWidths 
{
    double defWidth;		// default char width
    double defHeight;		// default char height
    double defVY;			// default origin position
    CoreFontCIDWidthExcep *exceps;	// exceptions
    int nExceps;			// number of valid entries in exceps
    CoreFontCIDWidthExcepV *	// exceptions for vertical font
        excepsV;
    int nExcepsV;			// number of valid entries in excepsV
};


enum CoreFontLocType {
  fontLocEmbedded,		// font embedded in PDF file
  fontLocExternal,		// external font file
  fontLocResident		// font resident in PS printer
};

class XWFontLoc 
{
public:

  XWFontLoc();
  ~XWFontLoc();

  CoreFontLocType locType;
  CoreFontType fontType;
  ObjRef embFontID;		// embedded stream obj ID
				//   (if locType == gfxFontLocEmbedded)
  XWString *path;		// font file path
				//   (if locType == gfxFontLocExternal)
				// PS font name
				//   (if locType == gfxFontLocResident)
  int fontNum;			// for TrueType collections
				//   (if locType == gfxFontLocExternal)
  XWString *encoding;		// PS font encoding, only for 16-bit fonts
				//   (if locType == gfxFontLocResident)
  int wMode;			// writing mode, only for 16-bit fonts
				//   (if locType == gfxFontLocResident)
  int substIdx;			// substitute font index
				//   (if locType == gfxFontLocExternal,
				//   and a Base-14 substitution was made)
};

#define fontFixedWidth (1 << 0)
#define fontSerif      (1 << 1)
#define fontSymbolic   (1 << 2)
#define fontItalic     (1 << 6)
#define fontBold       (1 << 18)


class XW_CORE_EXPORT XWCoreFont
{
public:
    XWCoreFont(char *tagA, 
               ObjRef idA, 
               XWString *nameA,
	  					 CoreFontType typeA, 
	  					 ObjRef embFontIDA);
    virtual ~XWCoreFont();
    
    double getAscent() { return ascent; }
    double getDescent() { return descent; }
    bool getEmbeddedFontID(ObjRef *embID)
        { *embID = embFontID; return embFontID.num >= 0; }
    XWString * getEmbeddedFontName() { return embFontName; }
    int        getFlags() { return flags; }
    double   * getFontBBox() { return fontBBox; }
    double   * getFontMatrix() { return fontMat; }
    ObjRef   * getID() { return &id; }
    XWString * getName() { return name; }
    virtual int getNextChar(char *s, 
                            int len, uint *code,
			                uint *u, 
			                int uSize, 
			                int *uLen,
			                double *dx, 
			                double *dy, 
			                double *ox, 
			                double *oy) = 0;
    XWString * getTag() { return tag; }
    CoreFontType getType() { return type; }
    virtual int getWMode() { return 0; }
    
    bool isBold() { return flags & fontBold; }
    virtual bool isCIDFont() { return false; }
    bool isFixedWidth() { return flags & fontFixedWidth; }
    bool isItalic() { return flags & fontItalic; }
    bool isOk() { return ok; }
    bool isSerif() { return flags & fontSerif; }
    bool isSymbolic() { return flags & fontSymbolic; }
    
    static XWFontLoc *locateBase14Font(XWString *base14Name);
    XWFontLoc *locateFont(XWRef *xref, bool ps);
    
    static XWCoreFont *makeFont(XWRef *xref, char *tagA, ObjRef idA, XWDict *fontDict);
    
    bool matches(const char *tagA) { return !tag->cmp(tagA); }
    
    char *readEmbFontFile(XWRef *xref, int *len);
    
    void setAscent(double a) {ascent = a;}
    void setBold() {flags |= fontBold;}
    void setDescent(double d) {descent = d;}
    void setFixedWidth() {flags |= fontFixedWidth;}
    void setFontBBox(double * bbox);
    void setFontMatrix(double * m);
    void setItalic() {flags |= fontItalic;}
    void setMissingWidth(double w) {missingWidth = w;}
    void setOK() {ok = true;}
    void setSerif() {flags |= fontSerif;}
    void setSymbolic() {flags |= fontSymbolic;}
    
protected:
    static XWFontLoc *getExternalFont(XWString *path, bool cid);
    static CoreFontType getFontType(XWRef *xref, XWDict *fontDict, ObjRef *embID);
    
    void readFontDescriptor(XWRef *xref, XWDict *fontDict);
    XWCharCodeToUnicode *readToUnicodeCMap(XWDict *fontDict, 
                                           int nBits,
				                           XWCharCodeToUnicode *ctu);
    
protected:
    XWString *tag;			// PDF font tag
    ObjRef id;
    XWString *name;		// font name
    CoreFontType type;		// type of font
    int flags;			// font descriptor flags
    XWString *embFontName;
    ObjRef embFontID;
    double fontMat[6];		// font matrix (Type 3 only)
    double fontBBox[4];		// font bounding box (Type 3 only)
    double missingWidth;		// "default" width
    double ascent;		// max height above baseline
    double descent;		// max depth below baseline
    bool ok;
};

class XW_CORE_EXPORT XWCore8BitFont: public XWCoreFont 
{
public:
    XWCore8BitFont(XWRef *xref, 
                  char *tagA, 
                  ObjRef idA, 
                  XWString *nameA,
	     				 		CoreFontType typeA, 
	     				 		ObjRef embFontIDA, 
	     				 		XWDict *fontDict);

    virtual ~XWCore8BitFont();

    virtual int getNextChar(char *s, 
                            int , 
                            uint *code,
			                uint *u, 
			                int uSize, 
			                int *uLen,
			                double *dx, 
			                double *dy, 
			                double *ox, 
			                double *oy);

    char *  getCharName(int code) { return enc[code]; }
    XWObject *getCharProc(int code, XWObject *proc);
    XWDict *getCharProcs();
    int   * getCodeToGIDMap(XWFontFileTrueType *ff);
    char ** getEncoding() { return enc; }
    bool    getHasEncoding() { return hasEncoding; }
    XWDict *getResources();
    XWCharCodeToUnicode * getToUnicode();
    bool    getUsesMacRomanEnc() { return usesMacRomanEnc; }
    double  getWidth(uchar c) { return widths[c]; }
    
private:
		Base14FontMapEntry *base14;
    char *enc[256];	
    char encFree[256];	
    XWCharCodeToUnicode *ctu;
    bool hasEncoding;
    bool usesMacRomanEnc;
    double widths[256];		// character widths
    XWObject charProcs;		// Type 3 CharProcs dictionary
    XWObject resources;		// Type 3 Resources dictionary
    
    friend class XWCoreFont;
};


class XW_CORE_EXPORT XWCoreCIDFont: public XWCoreFont 
{
public:
    XWCoreCIDFont(XWRef *xref, 
                 char *tagA, 
                 ObjRef idA, 
                 XWString *nameA,
	     					 CoreFontType typeA, 
	     					 ObjRef embFontIDA, 
	     					 XWDict *fontDict);

    virtual ~XWCoreCIDFont();
    
    int   * getCIDToGID() { return cidToGID; }
    int        getCIDToGIDLen() { return cidToGIDLen; }
    XWString * getCollection();
    virtual int getNextChar(char *s, 
                            int len, 
                            uint *code,
			                uint *u, 
			                int uSize, 
			                int *uLen,
			                double *dx, 
			                double *dy, 
			                double *ox, 
			                double *oy);
	XWCharCodeToUnicode * getToUnicode();
	virtual int getWMode();
	
    virtual bool isCIDFont() { return true; }

private:
	XWString *collection;
    XWCMap *cMap;
    XWCharCodeToUnicode *ctu;
    bool ctuUsesCharCode;
    CoreFontCIDWidths widths;
    int *cidToGID;
    int cidToGIDLen;
};

class XW_CORE_EXPORT XWCoreFontDict 
{
public:
    XWCoreFontDict(XWRef *xref, ObjRef *fontDictRef, XWDict *fontDict);
    virtual ~XWCoreFontDict();
	
    XWCoreFont *getFont(int i) { return fonts[i]; }
    int getNumFonts() { return numFonts; }
    
    virtual XWCoreFont *lookup(const char *tag);

protected:
    XWCoreFont **fonts;		// list of fonts
    int numFonts;			// number of fonts
};


#endif // XWCOREFONT_H

