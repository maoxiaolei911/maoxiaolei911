/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWADOBEGLYPHLIST_H
#define XWADOBEGLYPHLIST_H

#include <QMutex>
#include <QObject>
#include <QString>

#include "XWGlobal.h"

class XWAdobeGlyphNameEntry;

#define AGL_MAX_UNICODES 16

class XW_FONT_EXPORT XWAdobeGlyphName
{
public:
	XWAdobeGlyphName(char *glyphname);
	~XWAdobeGlyphName();
	
	static char * chopSuffix(const char *glyphname, char **suffixA);
	
	static const char * suffixToOtltag(const char *suffixA);
	
public:
	char *name;
  	char *suffix;
  	int   n_components;
  	XWAdobeGlyphName *alternate;
  	int   is_predef;
  	long  unicodes[AGL_MAX_UNICODES];
};

#define AGL_TABLE_SIZE 503

class XW_FONT_EXPORT XWAdobeGlyphList : public QObject
{
	Q_OBJECT
	
public:
	struct Iterator 
	{
  		int    index;
  		XWAdobeGlyphNameEntry  * curr;
	};
	
	XWAdobeGlyphList(QObject * parent = 0);
	~XWAdobeGlyphList();
	
	void clearIterator();
	
	char     * getKey(int *keylen);
	int getUnicodes(const char *glyphstr,
		  			long *unicodes, 
		  			int max_unicodes);
	XWAdobeGlyphName * getVal();
		
	int loadListFile(const QString & filename, int is_predef);
	XWAdobeGlyphName * lookupList(const char *glyphname);
	
	static long nameConvertUnicode(const char *glyphname);
	static bool nameIsUnicode(const char *glyphname);
	bool next();
	
	bool setIterator();
	long sputUTF16BE (const char *glyphstr,
		              uchar **dstpp, 
		              uchar *limptr,
		  			  int *fail_count);
	
	static bool UC_is_valid(long ucv)
	{
		if (ucv < 0 || (ucv >= 0x0000D800L && ucv <= 0x0000DFFFL))
    		return false;
    		
  		return true;
	}
	
	static int UC_sput_UTF16BE(long ucv, 
	                           uchar **pp, 
	                           uchar *limptr);
	
private:
	void appendTable(const void *key, int keylen, XWAdobeGlyphName *value);
	
	uint getHash(const void *key, int keylen);
	
	void insertTable(const void *key, int keylen, XWAdobeGlyphName *value);
	
	XWAdobeGlyphNameEntry * lookup(const void *key, int keylen);
	XWAdobeGlyphName * lookupTable(const void *key, int keylen);
	
	long putUnicodeGlyph(const char *name,
		   				 uchar **dstpp, 
		   				 uchar *limptr);
		   				 
	long xtol (const char *start, int len);
	
private:
	long   count;
	XWAdobeGlyphNameEntry ** table;
	Iterator * itor;
	QMutex mutex;
};

#endif // XWADOBEGLYPHLIST_H
