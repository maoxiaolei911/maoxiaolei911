/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWFONTSETTING_H
#define XWFONTSETTING_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QSettings>
#include "XWGlobal.h"

class QFile;
class XWString;
class XWList;
class XWHash;
class XWNameToCharCode;
class XWCharCodeToUnicode;
class XWCharCodeToUnicodeCache;
class XWUnicodeMap;
class XWUnicodeMapCache;
class XWCMap;
class XWCMapCache;
class XWTexFontMaps;

class SysFontList;

enum SysFontType {
  sysFontPFA,
  sysFontPFB,
  sysFontTTF,
  sysFontTTC
};

enum PSLevel 
{
    psLevel1,
    psLevel1Sep,
    psLevel2,
    psLevel2Sep,
    psLevel3,
    psLevel3Sep
};

class XW_FONT_EXPORT PSFontParam16 
{
public:

  XWString *name;		// PDF font name for psResidentFont16;
				//   char collection name for psResidentFontCC
  int wMode;			// writing mode (0=horiz, 1=vert)
  XWString *psFontName;		// PostScript font name
  XWString *encoding;		// encoding

  PSFontParam16(XWString *nameA, int wModeA,
		            XWString *psFontNameA, 
		            XWString *encodingA);
  ~PSFontParam16();
};

class XW_FONT_EXPORT XWFontSetting
{
public:
	XWFontSetting();
	
	XWString * findCCFontFile(XWString *collection);
	QFile    * findCMapFile(XWString *collection, XWString *cMapName);
	XWString * findFontFile(XWString *fontName);
	XWString * findSystemFontFile(XWString *fontName, 
	                              SysFontType *type,
			                          int *fontNum);
	QFile * findToUnicodeFile(XWString *name);
	
	bool getAntialias();
	XWCharCodeToUnicode * getCIDToUnicode(XWString *collection);
	XWCMap * getCMap(XWString *collection, XWString *cMapName);	
	int            getMacRomanCharCode(char * charName);
  XWString      * getPSResidentFont(XWString *fontName);
  PSFontParam16 * getPSResidentFont16(XWString *fontName, int wMode);
  PSFontParam16 * getPSResidentFontCC(XWString *collection, int wMode);
  XWList        * getPSResidentFonts();
	XWUnicodeMap * getResidentUnicodeMap(XWString *encodingName);
	QSettings    * getSettings() {return settings;}
	XWUnicodeMap * getTextEncoding(XWString *encodingName);
	XWUnicodeMap * getUnicodeMap(XWString *encodingName);
	XWUnicodeMap * getUnicodeMap2(XWString *encodingName);
	QFile *        getUnicodeMapFile(XWString *encodingName);
	XWCharCodeToUnicode * getUnicodeToUnicode(XWString *fontName);
	
	int mapNameToUnicode(char * charName);
	
	static void init();
	static void quit();
	
	QStringList getCidToUnicodes();
	QStringList getCMapDirs();
	QStringList getFonts();
	QString     getFontFile(const QString & fontname);
	QStringList getNameToUnicode();
	QStringList getUnicodeMap();
	QStringList getUnicodeToUnicodes();
	
	void setCidToUnicodes(const QStringList & list);
	void setCMapDirs(const QStringList & list);
	void setFontFile(const QString & fontname, const QString & file);
	void setNameToUnicode(const QStringList & list);
	void setUnicodeMap(const QStringList & list);
	void setUnicodeToUnicodes(const QStringList & list);
	
private:
	QSettings * settings;
};


class XWFontSettingPrivate : public QObject
{
	Q_OBJECT
	
public:
	XWFontSettingPrivate();
	~XWFontSettingPrivate();

public:
	XWNameToCharCode * macRomanReverseMap;	
	XWNameToCharCode * nameToUnicode;
	XWHash *cidToUnicodes;
	XWHash *unicodeToUnicodes;
	XWHash *residentUnicodeMaps;
	XWHash *unicodeMaps;
	XWHash *cMapDirs;
  XWList *psResidentFonts16;
  XWList *psResidentFontsCC;
	bool antialias;
	XWCharCodeToUnicodeCache *cidToUnicodeCache;
  XWCharCodeToUnicodeCache *unicodeToUnicodeCache;
  XWUnicodeMapCache *unicodeMapCache;
  XWCMapCache *cMapCache;
    

	SysFontList * sysFontList;

    
  QSettings * settings;
    
public:
	void parseCIDToUnicode();
	void parseCMapDir();
	void parseNameToUnicode();
	void parsePSResidentFont(const QString & key, XWList * list);
	void parseUnicodeMap();
	void parseUnicodeToUnicode();
};


#endif // XWFONTSETTING_H

