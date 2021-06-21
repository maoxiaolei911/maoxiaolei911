/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTYPE0FONT_H
#define XWTYPE0FONT_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include <QMutex>
#include <QObject>
#include "XWGlobal.h"

#define add_to_used_chars2(b,c) {(b)[(c)/8] |= (1 << (7-((c)%8)));}
#define is_used_char2(b,c) (((b)[(c)/8]) & (1 << (7-((c)%8))))


class XWCIDFont;
class XWTexFontMapOpt;

class XW_FONT_EXPORT XWType0Font : public QObject
{
	Q_OBJECT
	
public:
	friend class XWType0FontCache;
	
	XWType0Font(QObject * parent = 0);
	~XWType0Font();
	
	const char * getBaseFont() {return BaseFont;}
	XWCIDFont * getDescendant() {return descendant;}
	const char * getEncoding() {return encoding;}
	const char * getFontName() {return fontname;}
	FT_Face      getFtFace();
	ushort     * getFtToGid();
	char * getUsedChars();
	int    getWMode() {return wmode;}
	
	bool isLocking() {return locking;}

private:
	char    *fontname;
	char    *encoding;
	char    *used_chars;
	XWCIDFont *descendant;
	int      flags;
  	int      wmode;
  	
  	char * BaseFont;
  	bool locking;
};

class XW_FONT_EXPORT XWType0FontCache
{
public:
	XWType0FontCache();
	~XWType0FontCache();
	
	int find(const char *map_name, 
	         int cmap_id, 
	         XWTexFontMapOpt *fmap_opt,
	         int pdf_ver = 4);
	
	XWType0Font * get(int id);
	ushort      * getFtToGid(int id);
	
private:
	int           count;
  	int           capacity;
  	XWType0Font **fonts;
};


#endif // XWTYPE0FONT_H
