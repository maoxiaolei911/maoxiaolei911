/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWDVICORE_H
#define XWDVICORE_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TRUETYPE_TABLES_H
#include <QObject>
#include <QIODevice>
#include <QString>
#include <QStringList>
#include <QHash>
#include <QRectF>
#include "XWGlobal.h"
#include "XWObject.h"
#include "XWDVIType.h"

#define DVI_STACK_DEPTH_MAX  256u

class XWLexer;
class XWCMap;
class XWTexFontMapOpt;
class XWTexFontMap;
class XWDVIRef;
class XWDVICore;
class XWDVIDev;
class XWDVIEncoding;
class XWDVIFont;
class XWDVIType0Font;
class XWDVIResources;
class XWDVIColorSpaces;
class XWOTLConf;
class XWDVIEncodingCache;
class XWDVIType0FontCache;
class XWDVIFontCache;
class XWDVIImageCache;
class XWPDFNames;
#ifdef XW_BUILD_PS_LIB
class XWPSInterpreter;
#endif //XW_BUILD_PS_LIB

class HTMLState
{
public:
	HTMLState();
	~HTMLState();
	
	void clear(XWDVIRef * xrefA);
	
public:
	struct 
	{
    	long  extensions;
  	} opts;

  	XWObject link_dict;
  	char    *baseurl;
  	int      pending_type;
};

class ResourceMap
{
public:
	ResourceMap(const char * keyA, int keylenA, int typeA, int res_idA);
	~ResourceMap();
	
public:
	char  *key;
  	int    keylen;
  	int   type;
  	int   res_id;
  	
  	ResourceMap * next;
};

class ResourceMaps
{
public:
	ResourceMaps();
	~ResourceMaps();
	
	void appendTable(const char * ident, int typeA, int res_idA);
	
	ResourceMap * lookup(const void *key, int keylen);

private:	
	uint getHash(const void *key, int keylen);
		
private:
	int count;
	ResourceMap ** table;
};

class PDFState
{
public:
	PDFState(XWDVIRef * xrefA);
	~PDFState();
	
	void clean(XWDVIRef * xrefA);
	
	int addResource(XWDVICore * coreA, 
	                const char *ident, 
	                int res_id);
	
	int findResource(const char *ident);
	
	int modStrings(const char *kp, XWObject *vp);

	int needReencode(const char *k, XWObject *vp);
	
	XWObject * parsePDFDict(XWDVICore * coreA,
	                        XWDVIRef * xrefA,
	                        XWLexer * lexer, 
	                        XWObject * dict);
	
	int reencodeString(XWCMap *cmap, XWObject *instring);
	
public:
	struct ToUnicode
	{
		int       cmap_id;
  		int       unescape_backslash;
  		XWObject  taintkeys;
	};
	
	XWObject  annot_dict;
	int        lowest_level;
	ResourceMaps * resourcemap;
	ToUnicode cd;
};


#define TPIC_MODE__FILL_SOLID   0
#define TPIC_MODE__FILL_OPACITY 1
#define TPIC_MODE__FILL_SHAPE   2

class TPICState
{
public:
	TPICState();
	~TPICState();
	
	void clear();
	
public:
	struct 
	{
    	int   fill;
  	} mode;
  	
  	double     pen_size;
  	int        fill_shape; /* boolean */
  	double     fill_color;
  	PDFCoord * points;
  	int        num_points;
  	int        max_points;
};

class XW_DVI_EXPORT XWDVICore : public QObject
{
	Q_OBJECT
	
public:
	XWDVICore(XWDVIRef * xrefA, int mpmodeA = 0, QObject * parent = 0);
	~XWDVICore();
	
#ifdef XW_BUILD_PS_LIB
  void addBangSpecial(const char *buffer, long sizeA);
#endif //XW_BUILD_PS_LIB
	void addType0FontReference(int id);
	
	void beginDoc();
	
	void       clear();
	void       clearObjects();
	void       clearState();
	XWObject * createCMapStream(XWCMap *cmap, 
	                            int flagsA,
	                            XWObject * obj);
	
	int  defineImageResource(const char *ident,
			                 int subtype, 
			                 void *info, 
			                 XWObject *resource);
	long defineResource(const char *category, 
		    			const char *resname, 
		    			XWObject *object, 
		    			int flagsA);
	void doBop();
	void doComputeBoxes(int flag) {computeBoxes = flag;} 
	void doDir(uchar d);
	void doDown(long y)
	{
		if (!dviState.d) 
    		dviState.v += y;
  		else 
    		dviState.h -= y;
	}
	void doEop();
	void doFnt(long tex_id);
	void doGlyphArray(QIODevice * file, int yLocsPresent);
	void doLinkAnnot(int flag) {linkAnnot = flag;}
	void doMarkDepth();
	void doMoveTo(long x, long y)
	{
		dviState.h = x;
  		dviState.v = y;
	}
	void doMPSPage(QIODevice * fp);	
	void doPicFile(QIODevice * file);
	void doPop();
	void doPush();
	void doPut(long ch);	
	void doRight(long x)
	{
		if (!dviState.d) 
    		dviState.h += x;
  		else 
    		dviState.v += x;
	}
	void doRule(long width, long height);	
	void doSet(long ch);	
	void doSetFont(int font_id) {currentFont = font_id;}
	void doSpecial(const char *buffer, long size);
	void doString(const uchar *s, int len);	
	void doTagDepth();
	void doUntagDepth();
	void doW(long ch);
	void doW0();
	void doX(long ch);
	void doX0();
	void doY(long ch);
	void doY0();
	void doZ(long ch);
	void doZ0();
	
#ifdef XW_BUILD_PS_LIB
  int doLiteralSpecial(const char *buffer, long sizeA);
#endif //XW_BUILD_PS_LIB
	
	void encodingAddUsedChars(int encoding_id, const char *is_used);
	void encodingComplete(bool finished = true);
	void encodingCompletePage();
	void endDoc(bool finished = true);
	int  encodingFindResource(char * enc_name);
	XWDVIEncoding * encodingGet(int enc_id);
	void encodingUsedByType3(int enc_id);
	
#ifdef XW_BUILD_PS_LIB
  void endBangSpecials();
  bool endScanning() {return endscanning;}
#endif //XW_BUILD_PS_LIB
	
	int  findFontResource(const char *tex_name,
		       	          double font_scale, 
		       	          XWTexFontMap *mrec);
	int  findImageResource(const char *identA, 
	                       long page_noA, 
	                       XWObject *dictA);
	long findResource(const char *category, 
	                  const char *resname);
	int  findType0Font(const char *map_name, 
	                  int cmap_id, 
	                  XWTexFontMapOpt *fmap_opt);
	void finishVF();
	void flushObject(const char *key);
	
	XWObject  *  getColorSpaceReference(int cspc_id, XWObject * ref);
	int          getCurrentFont() {return currentFont;}
	DVIFontDef * getCurrentFontDef();
	XWDVIFont *  getDviFont(int font_id);
	double       getDvi2Pts() {return dvi2pts;}
	char      ** getEncoding(int enc_id);
	char      *  getEncodingName(int enc_id);
	XWObject  *  getEncodingObj(int enc_id);
	XWObject  *  getEncodingTounicode(int enc_id);
	DVIFontDef * getFontDef();
	DVIFontDef * getFontDef(int i);	
	DVIFontDef * getFontDefs() {return defFonts;}
	int          getFontEncoding(int font_id);
	XWObject  *  getFontReference(int font_id, XWObject * obj);
	int          getFontSubtype(int font_id);
	char      *  getFontUsedChars(int font_id);
	int          getFontWMode(int font_id);
	DVIHeader  * getHeader() {return &dviInfo;}	
	int          getHDPI() {return hDPI;}
	HTMLState  * getHTMLState();	
	XWObject   * getImageReference(int id, XWObject * obj);
	char       * getImageResName(int id);
	int          getImageSubtype(int id);
	DVILoadedFont * getLoadedFont(int i);
	double       getMag() {return mag;}
	int          getNumberOfDefFont() {return numDefFonts;}
	int          getNumberOfLoadedFont() {return numLoadedFonts;}
	double       getPageHeight() {return pageHeight;}
	double       getPageWidth() {return pageWidth;}
	PDFState   * getPDFState();
#ifdef XW_BUILD_PS_LIB
	XWPSInterpreter * getPSInterpreter();
#endif //XW_BUILD_PS_LIB
	XWObject   * getResourceReference(long rc_id, XWObject * ref);
	TPICState  * getTPICState();
	XWDVIType0Font * getType0Font(int id);
	int getVDPI() {return vDPI;}
	
	long getPageNo() {return pageNo;}
	void getPos(double * x, double * y);
		
	int iccpLoadProfile(const char *ident,
		                const void *profile, 
		                long proflen);
	void initDev();
	void initVF(int dev_font_id);
	bool isMPs() {return mpMode == 1;}
	int  isPredefinedEncoding(int enc_id);
	
#ifdef XW_BUILD_PS_LIB
	bool lastClose() {return lastclose;}
#endif //XW_BUILD_PS_LIB
	void loadFontMapFile(const char  *filename);
	XWObject * loadToUnicodeStream(const char *ident,
	                               XWObject * obj);	
	int  locateFont(const char *tfm_name, long ptsize, bool vf = false);
	int  locateNativeFont(const char *ps_name, 
	                      const char *fam_name,
                        const char *sty_name, 
                        long ptsize, 
                        int layout_dir);
	XWObject * lookupObject(const char *key, XWObject * obj);
	XWObject * lookupReference(const char *key, XWObject * obj);
		
	XWObject * otfCreateToUnicodeStream(const char *font_name,
			     					    int ttc_index,
			                            const char *used_glyphs,
			                            XWObject * obj);
	int otfLoadUnicodeCMap(const char *map_name, 
	                       int ttc_index,
		                   const char *otl_tags, 
		                   int wmode);
	XWObject * otlFindConf(const char *conf_name, 
	                       XWObject * rule);
	void otlInitConf();
			                            
	void pushObject(const char *key, XWObject *value);
	
	static void quit();
	
	XWObject * readToUnicodeFile(const char *cmap_name, 
	                             XWObject * obj);
	void resetFlags();
	
	int scaleImage(int            id,
                   PDFTMatrix    *M,
                   PDFRect       *r,
                   TransformInfo *p);
	void scanPaperSize(const char *buf, long size);
	int  scanSpecial(double *wd, 
                   double *ht, 
                   double *xo, 
                   double *yo, 
                   char *lm,
	                 unsigned *minorversion,
	                 int *do_enc, 
	                 unsigned *key_bits, 
	                 unsigned *permission, 
	                 char *owner_pw, 
	                 char *user_pw,
	                 const char *buf, 
	                 long size);
	void setCIDFontFixedPitch(bool e);
	void setDecimalDigits(int i);
	void setDPI(int hDPIA, int vDPIA);
	void setFontMapFirstMatch(bool e);
	void setIgnoreColors(int i);
	void setImageAttr(int id, 
	                  long width, 
	                  long height, 
	                  double xdensity, 
	                  double ydensity, 
	                  double llx, 
	                  double lly, 
	                  double urx, 
	                  double ury);
	void setMag(double m) {mag = m;}
	void setOffset(double hoff, double voff);
	void setPageNo(long n);
	void setPaperSize(double w, double h);
    void setTPICTransFill(bool e);
    
#ifdef XW_BUILD_PS_LIB
    void setUseSpecial(bool e);
#endif //XW_BUILD_PS_LIB

    double tellMag() {return totalMag;}
    void   type0T1SetToUnicode(int t0t1id, XWObject *cmap_ref);
    
#ifdef XW_BUILD_PS_LIB
    bool usesSpecial() {return usesspecial;}
#endif //XW_BUILD_PS_LIB
    
private:
	
	void needMoreFonts(int n);
	
private:
	XWDVIRef * xref;	
	int mpMode;  
	
    XWDVIDev * dev;    
    long   optFlags;
    double mag;    
    int    hDPI;
    int    vDPI;
    int    doEncryption;
    uint   keyBits;
    uint   permission;    
    
    double paperWidth;
    double paperHeight;
    double x_offset;
    double y_offset;
    char   landscapeMode;
    
    double pageWidth;
    double pageHeight;
    
    DVIHeader dviInfo;
    double devOriginX;
    double devOriginY;
    
    DVILoadedFont * loadedFonts;
    int numLoadedFonts;
    int maxLoadedFonts;
    
    DVIFontDef * defFonts;
    int       numDefFonts;
    int       maxDefFonts;
    int       computeBoxes;
    int       linkAnnot;
    
    int pageNo;
    
    double dvi2pts;
    double totalMag;
    
    DVIState dviState;
    DVIState dviStack[DVI_STACK_DEPTH_MAX];
    int  dviStackDepth;
    int  currentFont;
    int  currentFontDef;
    int  processingPage;
    int  markedDepth;
    int  taggedDepth;
    
    int savedDVIFont[16];
    int numSavedFonts;
    
    struct PageAttr
    {
    	double width;
    	double height;
    	double xoff;
    	double yoff;
    };
    
    PageAttr * pageAttrs;
    int pageAttrCounter;
    
    QHash<int, int> pageAttrHash;
    
    XWDVIResources * resources;
	XWDVIColorSpaces * colorSpaces;
	XWOTLConf * otlconf;
	XWDVIEncodingCache * encodings;
	XWDVIType0FontCache * type0FontCache;
	XWDVIFontCache * dviFontCache;
	XWDVIImageCache * imageCache;
	XWPDFNames * namedObjects;
	
	HTMLState * _html_state;
	PDFState  * _pdf_stat;
	TPICState * _tpic_state;
	
#ifdef XW_BUILD_PS_LIB
	bool usesspecial;
	bool lastclose;
	bool endscanning;
	XWPSInterpreter * interpreter;
	QIODevice * bangspecialsFile;
#endif //XW_BUILD_PS_LIB
};

#endif //XWDVICORE_H