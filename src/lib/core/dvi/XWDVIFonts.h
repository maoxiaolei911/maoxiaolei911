/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWDVIFONTS_H
#define XWDVIFONTS_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include <QList>
#include <QHash>

#include "XWObject.h"

class XWCMap;
class XWCIDFont;
class XWType0Font;
class XWCIDWidths;
class XWTexFont;
class XWDVIRef;
class XWDVICore;

struct PageFonts
{
	QList<int> ids;
};

class XWDVICIDFont
{
public:
	friend class XWDVICIDFontCache;
	
	XWDVICIDFont(XWDVIRef * xref, int font_idA);
	~XWDVICIDFont();
	
	void doFont(XWDVICore * core, XWDVIRef * xref);
	
	void flush(XWDVIRef * xref, bool finished = true);
	
	CIDSysInfo  * getCIDSysInfo();
	int           getEmbedding();
	int           getFlag(int mask);
	const char  * getFontName();
	char        * getIdent();
	int           getOptIndex();
	int           getParentID(int wmode);
	XWObject *    getResource(XWDVIRef * xref, XWObject * obj);
	int           getSubtype();
	
	bool isACCFont();
	bool isBaseFont();
	bool isUCSFont();

private:
	XWObject * getCIDWidths(XWDVIRef * xref, 
	                        XWCIDWidths * w, 
	                        XWObject * w_array);
	
private:
	int font_id;
	
  	XWObject indirect;
  	XWObject fontdict;
  	XWObject descriptor;
};

class XWDVICIDFontCache
{
public:
	XWDVICIDFontCache();
	~XWDVICIDFontCache();
	
	void close(XWDVICore * core, XWDVIRef * xref, bool finished = true);
	
	int find(XWDVIRef * xref,
	         const char *map_name, 
	         CIDSysInfo *cmap_csi, 
	         XWTexFontMapOpt *fmap_opt);
	
	XWDVICIDFont * get(int font_id);
	
	void load(XWDVICore * core, XWDVIRef * xref, long page_no);
	
private:
	int       num;
  	int       max;
  	XWDVICIDFont **fonts;
};

class XWDVIType0Font
{
public:
	friend class XWDVIType0FontCache;
	
	XWDVIType0Font(XWDVIRef * xref, 
	               int font_idA,
	               XWDVICIDFont * descendantA);
	~XWDVIType0Font();
	
	void doFont(XWDVICore * core);
	
	void flush(XWDVIRef * xref, bool finished = true);
	
	FT_Face    getFtFace();
	ushort  * getFtToGid();
	
	XWObject * getResource(XWDVIRef * xref, XWObject * obj);
	char     * getUsedChars();
	int        getWMode();
	
	void setToUnicode(XWObject *cmap_ref);
	
private:
	void addToUnicode(XWDVICore * core, XWType0Font * font);
	
private:
	int font_id;
	
	XWDVICIDFont * descendant;
	
  	XWObject indirect;
  	XWObject fontdict;
  	XWObject descriptor;
};

class XWDVIType0FontCache
{
public:
	XWDVIType0FontCache();
	~XWDVIType0FontCache();
	
	void addReference(XWDVIRef * xref, int id);
	
	void close(XWDVICore * core, XWDVIRef * xref, bool finished = true);
	
	int find(XWDVIRef * xref, 
	         const char *map_name, 
	         int cmap_id, 
	         XWTexFontMapOpt *fmap_opt);
	         
	XWDVIType0Font * get(int id);
	
	void load(XWDVICore * core, XWDVIRef * xref, long page_no);
	
	void setToUnicode(int t0t1id, XWObject *cmap_ref);
	
private:
	int           count;
  	int           capacity;
  	XWDVIType0Font **fonts;
  	
  	XWDVICIDFontCache * cidCache;
  	
  	QHash<long, PageFonts*> pageFonts;
};

class XWDVIFont
{
public:
	friend class XWDVIFontCache;
	
	XWDVIFont(XWDVIRef * xref, int font_idA, int type0_idA);
	~XWDVIFont();
	
	void doFont(XWDVICore * core, 
	            XWDVIRef * xref, 
	            int hDPI, 
	            int vDPI);
	
	void flush(XWDVICore * core, XWDVIRef * xref, bool finished = true);
	
	int    getEncoding();	
	char * getFontName();
	FT_Face   getFtFace(XWDVICore * core);
	ushort  * getFtToGid(XWDVICore * core);
	char * getIdent();
	XWObject * getReference(XWDVICore * core, XWDVIRef * xref, XWObject * obj);
	int    getSubtype();
	int    getType0ID() {return type0_id;}
	char * getUsedChars(XWDVICore * core);
	int    getWMode(XWDVICore * core);
	
private:
	void doTrueType(XWDVIRef * xref, XWTexFont * font);
	void doType1(XWDVICore * core, XWDVIRef * xref, XWTexFont * font);
	void doType3(XWDVIRef * xref, 
	             XWTexFont * font, 
	             int hDPI, 
	             int vDPI);
	
	void   getDescriptor(XWDVIRef * xrefA, XWTexFont * font);
	void   getResource(XWDVIRef * xrefA, XWTexFont * font);
	
	int tryLoadToUnicodeCMap(XWDVICore * core, XWDVIRef * xref, XWTexFont * font);
	
private:
	int font_id;
	int type0_id;
	int encoding_id;
  
  	XWObject reference;
  	XWObject resource;
  	XWObject descriptor;
};

class XWDVIFontCache
{
public:
	XWDVIFontCache();
	~XWDVIFontCache();
	
	void close(XWDVICore * core, XWDVIRef * xref, int hDPI, int vDPI, bool finished = true);
	
	int    find(XWDVICore * core, 
	            XWDVIRef * xref, 
				int hDPI,
	            const char *tex_name,
		       	double font_scale, 
		       	XWTexFontMap *mrec);
		       	
	XWDVIFont * getFont(int font_id);		       	
	int        getFontEncoding(int font_id);
	XWObject * getFontReference(XWDVICore * core, 
	                            XWDVIRef * xref, 
	                            int font_id, 
	                            XWObject * obj);
	int        getFontSubtype(int font_id);
	char     * getFontUsedChars(XWDVICore * core, int font_id);
	int        getFontWMode(XWDVICore * core, int font_id);
	
	void load(XWDVICore * core, 
	          XWDVIRef * xref, 
	          long page_no, 
	          int hDPI, 
	          int vDPI);
	
private:
	int       count;
  	int       capacity;
  	XWDVIFont **fonts;
  	
  	QHash<long, PageFonts*> pageFonts;
};

#endif // XWDVIFONTS_H

