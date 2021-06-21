/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWFONTCACHE_H
#define XWFONTCACHE_H

#include <QObject>
#include <QString>

#include "XWGlobal.h"
#include "XWCMap.h"

class XWTexFontMapOpt;
class XWTexFontMap;
class XWCIDFont;
class XWAdobeGlyphName;
class XWTexFont;
class XWEncoding;
class XWType0Font;

class XW_FONT_EXPORT XWFontCache : public QObject
{
	Q_OBJECT
	
public:
	XWFontCache(bool pdfA, QObject * parent = 0);
	
	int   addCMap(XWCMap * cmap);
	int   addTexFont(XWTexFont * font);
	int   aglGetUnicodes(const char *glyphstr,
			       		 long *unicodes, 
			       		 int max_uncodes);
	XWAdobeGlyphName * aglLookupList(const char *glyphname);
	long  aglSPutUTF16BE(const char *name,
			       	     uchar **dstpp,
			       		 uchar *limptr, 
			       		 int *num_fails);
			       
	int appendFontMapRecord(const char *kp, const XWTexFontMap *vp);
	
	static void close();
	
	void encodingAddUsedChars(int encoding_id, const char *is_used);
	XWEncoding * encodingGet(int enc_id);
	void encodingUsedByType3(int enc_id);
	
	int findCIDFont(const char *map_name, 
	                CIDSysInfo *cmap_csi, 
	                XWTexFontMapOpt *fmap_opt,
	                int pdf_ver = 4);
	int findCMap(const char *cmap_name);
	int findEncoding(char *enc_name);
	int findTexFont(const char *texname,
	                const char *fontname,
		       	    double font_scale, 
		       	    int    encoding_id,
		       	    XWTexFontMap *mrec,
		       	    int base_dpi);
	int findType0Font(const char *map_name, 
	                 int cmap_id, 
	                 XWTexFontMapOpt *fmap_opt,
	                 int pdf_ver = 4);
	                
	XWCIDFont   * getCIDFont(int fnt_id);
	XWCMap      * getCMap(int id);
	char       ** getEncoding(int enc_id);
	char        * getEncodingName(int enc_id);
	XWTexFont   * getTexFont(int id);
	int           getTexFontEncoding(int font_id);
	int           getTexFontSubtype(int font_id);
	char        * getTexFontUsedChars(int font_id);
	int           getTexFontWMode(int font_id);
	XWType0Font * getType0Font(int id);
	                
	static void init();
	int insertFontMapRecord(const char  *kp, const XWTexFontMap *mrec);
	int isPredefinedEncoding(int enc_id);
	
	int loadFontMapFile(const char  *filename, int mode);
	int loadNativeFont(const char *ps_name,
                     const char *fam_name, 
                     const char *sty_name,
                     int layout_dir);
	XWTexFontMap * lookupFontMapRecord(const char  *kp);
	
	int removeFontmapRecord(const char  *kp);
	
private:
	bool pdf;
};

#endif // XWFONTCACHE_H
