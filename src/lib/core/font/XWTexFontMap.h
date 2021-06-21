/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTEXFONTMAP_H
#define XWTEXFONTMAP_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TYPE1_TABLES_H
#include FT_TRUETYPE_TABLES_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_IMAGE_H
#include FT_BITMAP_H
#include FT_BBOX_H

#include <math.h>
#include <QObject>
#include <QString>
#include <QIODevice>
#include <QFile>

#include "XWGlobal.h"

#define MAP_TABLE_SIZE 503

#define FONTMAP_RMODE_REPLACE  0
#define FONTMAP_RMODE_APPEND  '+'
#define FONTMAP_RMODE_REMOVE  '-'

#define FONTMAP_OPT_NOEMBED (1 << 1)
#define FONTMAP_OPT_VERT    (1 << 2)

#define FONTMAP_STYLE_NONE       0
#define FONTMAP_STYLE_BOLD       1
#define FONTMAP_STYLE_ITALIC     2
#define FONTMAP_STYLE_BOLDITALIC 3

class XWTeXFontSetting;
class XWTexFontMapEntry;

class XW_FONT_EXPORT XWTexFontMapOpt
{
public:
	XWTexFontMapOpt();
	XWTexFontMapOpt(XWTexFontMapOpt * src);
	~XWTexFontMapOpt();
	
	char * stripOptions(const char *map_name);
	
public:
	double slant;
	double extend;
	double bold;
	long   mapc;
	long   flags;
	char  *otl_tags;
	char  *tounicode;
	double design_size;
	char  *charcoll;
	int    index;
	int    style;
	int    stemv;
	bool   boldb;
	bool   usevf;
	char * vfont;
	int    pid;
	int    eid;
	bool   rotate;
	double y_offset;
	bool   smallcaps;
	bool   subfontligs;
	char * ligname;
	char * ligsubfontid;
	char * inencname;
	char * outencname;
	double  size;
	FT_Face ftface;
  unsigned short *glyphwidths;
};

class XW_FONT_EXPORT XWTexFontMap : public QObject
{
	Q_OBJECT
	
public:
	XWTexFontMap(QObject * parent = 0);
	XWTexFontMap(const XWTexFontMap * src, QObject * parent = 0);
	XWTexFontMap(const char *name, 
	             const char *path, 
	             int index, 
	             FT_Face face, 
	             int layout_dir, QObject * parent = 0);
	             
	~XWTexFontMap();
	
	XWTexFontMapOpt * getOpt() {return opt;}
	char * getVFont() {return opt->vfont;}
	
	bool invalid() const {return (!map_name || !font_name);}
	static int isPDFMMapLine(const char *mline);
	
	int readLine(const char *mline, long mline_len, int format);
	int readSetting(const char *tfm_name);
	int readTTFontsMapLine(const char *mline, long mline_len);
	
	void toSetting(XWTeXFontSetting * setting);
	
	bool useVF() {return opt->usevf;}
	
public:
	char  *map_name;
  	char  *font_name;
  	char  *enc_name;
  	
  	struct 
  	{
    	char  *sfd_name;
    	char  *subfont_id;
  	} charmap;
  	
  	XWTexFontMapOpt * opt;
  	
private:
	void fillInDefaults(const char *tex_name);
	
	int parseMapdefDPM(const char *mapdef, char *endptr);
	int parseMapdefDPS(const char *mapdef, char *endptr);
};

class XW_FONT_EXPORT XWTexFontMaps : public QObject
{
	Q_OBJECT
	
public:
	struct Iterator 
	{
  		int    index;
  		XWTexFontMapEntry  * curr;
	};
	
	XWTexFontMaps(QObject * parent = 0);
	~XWTexFontMaps();
	
	int appendRecord(const char *kp, const XWTexFontMap *vp);
	
	static void enableNoEmbed();
	
	QString getSysFontFile(const char *ps_name,
	                       const char *fam_name, 
                         const char *sty_name);
	
	int insertRecord(const char *kp, const XWTexFontMap *vp);
	
	int loadFile(const char *filename, int mode, bool tosetting = false);
	int loadNativeFont(const char *ps_name,
                     const char *fam_name, 
                     const char *sty_name,
                     int layout_dir);
	int loadTTFontsMap(const char *filename, bool tosetting = false);
	XWTexFontMap * lookupRecord(const char *tfm_name);
	
	int removeRecord(const char *kp);
	
private:
	void appendTable(const char *key, int keylen, XWTexFontMap *value);
	
	uint getHash(const char *key, int keylen);
	
	void insertNativeRecord(const char *name, 
	                        const char *path, 
	                        int index, 
	                        FT_Face face, 
	                        int layout_dir);
	void insertTable(const char *key, int keylen, XWTexFontMap *value);
	
	bool loadNativeFontFromPath(const char *ps_name, int layout_dir);
	
	XWTexFontMapEntry * lookup(const char *key, int keylen);
	XWTexFontMap * lookupTable(const char *key, int keylen);
	
	char * makeSubfontName(const char *map_name, 
	                       const char *sfd_name, 
	                       const char *sub_id);
	                       
	void removeTable(const char *key, int keylen);
	
private:
	long   count;
	XWTexFontMapEntry ** table;
	Iterator * itor;
};

#endif // XWTEXFONTMAP_H
