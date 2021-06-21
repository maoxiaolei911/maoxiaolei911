/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#if defined(Q_OS_MAC)
#include <CoreFoundation/CoreFoundation.h>
#include <ApplicationServices/ApplicationServices.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <QMutex>
#include <QCoreApplication>
#include <QByteArray>
#include <QFile>
#include <QTextCodec>
#include <QDir> 
#include "XWUtil.h"
#include "XWString.h"
#include "XWList.h"
#include "XWHash.h"
#include "XWStringUtil.h"
#include "XWApplication.h"
#include "XWFontSea.h"
#include "XWNameToCharCode.h"
#include "XWCharCodeToUnicode.h"
#include "XWUnicodeMap.h"
#include "XWCMap.h"
#include "BuiltinFontTables.h"
#include "FontEncodingTables.h"
#include "NameToUnicodeTable.h"
#include "UnicodeMapTables.h"
#include "UTF8.h"
#include "XWFontSetting.h"

#define cidToUnicodeCacheSize     4
#define unicodeToUnicodeCacheSize 4

static QMutex fontSettingMutex;

static struct {
  char *name;
  char *t1FileName;
  char *ttFileName;
} displayFontTab[] = {
  {"Courier",               "n022003l.pfb", "cour.ttf"},
  {"Courier-Bold",          "n022004l.pfb", "courbd.ttf"},
  {"Courier-BoldOblique",   "n022024l.pfb", "courbi.ttf"},
  {"Courier-Oblique",       "n022023l.pfb", "couri.ttf"},
  {"Helvetica",             "n019003l.pfb", "arial.ttf"},
  {"Helvetica-Bold",        "n019004l.pfb", "arialbd.ttf"},
  {"Helvetica-BoldOblique", "n019024l.pfb", "arialbi.ttf"},
  {"Helvetica-Oblique",     "n019023l.pfb", "ariali.ttf"},
  {"Symbol",                "s050000l.pfb", NULL},
  {"Times-Bold",            "n021004l.pfb", "timesbd.ttf"},
  {"Times-BoldItalic",      "n021024l.pfb", "timesbi.ttf"},
  {"Times-Italic",          "n021023l.pfb", "timesi.ttf"},
  {"Times-Roman",           "n021003l.pfb", "times.ttf"},
  {"ZapfDingbats",          "d050000l.pfb", NULL},
  {NULL}
};


#ifdef Q_OS_WIN
static const char *displayFontDirs[] = {
  "c:/windows/fonts",
  "c:/winnt/fonts",
  NULL
};
#elif defined(Q_OS_MAC)
static const char *displayFontDirs[] = {
  "/Library/Fonts",
	"/Network/Library/Fonts",
	"/System/Library/Fonts",
	"/System Folder/Fonts",
  NULL
};
#else
static const char *displayFontDirs[] = {
  "/usr/share/ghostscript/fonts",
  "/usr/local/share/ghostscript/fonts",
  "/usr/share/fonts/default/Type1",
  "/usr/share/fonts/default/ghostscript",
  "/usr/share/fonts/type1/gsfonts",
  NULL
};
#endif


class SysFontInfo 
{
public:
	SysFontInfo(XWString *nameA, 
  						bool boldA, 
              bool italicA,
	      			XWString *pathA, 
	      			SysFontType typeA, 
	      			int fontNumA);
  ~SysFontInfo();
  bool match(SysFontInfo *fi);
  bool match(XWString *nameA, bool boldA, bool italicA);
  
public:

  XWString *name;
  bool bold;
  bool italic;
  XWString *path;
  SysFontType type;
  int fontNum;			// for TrueType collections
};

SysFontInfo::SysFontInfo(XWString *nameA, 
  											 bool boldA, 
              					 bool italicA,
	      								 XWString *pathA, 
	      								 SysFontType typeA, 
	      								 int fontNumA) 
{
  name = nameA;
  bold = boldA;
  italic = italicA;
  path = pathA;
  type = typeA;
  fontNum = fontNumA;
}

SysFontInfo::~SysFontInfo() 
{
	if (name)
  	delete name;
  	
  if (path)
  	delete path;
}

bool SysFontInfo::match(SysFontInfo *fi) 
{
  return !qstricmp(name->getCString(), fi->name->getCString()) &&
         						 bold == fi->bold && italic == fi->italic;
}

bool SysFontInfo::match(XWString *nameA, bool boldA, bool italicA) 
{
  return !qstricmp(name->getCString(), nameA->getCString()) &&
         						 bold == boldA && italic == italicA;
}

class SysFontList
{
public:
	SysFontList();
  	~SysFontList();
  	
  	SysFontInfo *find(XWString *name);
  	
private:
	XWString * remap(XWString * name, bool boldA, bool italicA);
	
	void scanSysFonts();
								  
private:
	XWList *fonts;
};

SysFontList::SysFontList()
{
	fonts = new XWList();

	scanSysFonts();
  
}

SysFontList::~SysFontList()
{
	deleteXWList(fonts, SysFontInfo);
}

XWString * SysFontList::remap(XWString * name, bool boldA, bool italicA)
{
	XWString * alias = 0;
#ifdef Q_OS_WIN
  QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Shared Tools\\Font Mapping",
                                         QSettings::NativeFormat);
  	
  QString key = name->toQString();
  if (settings.contains(key))
  {
  	QString fn = settings.value(key).toString();
  	if (boldA)
  		fn += "Bold";
  			
  	if (italicA)
  		fn += "Italic";
  			
  	QByteArray ba = fn.toLocal8Bit();
		alias = new XWString(ba.data(), ba.size());
  }
#endif //
	return alias;
}

void SysFontList::scanSysFonts()
{
#ifdef Q_OS_WIN
		QString winFontDir;
  	QDir wd("c:/WINDOWS/Fonts");
  	if (wd.exists())
  		winFontDir = "c:/WINDOWS/Fonts";
  	else
  		winFontDir = "c:/WINNT/Fonts";
  	
  	QSettings * settings = new QSettings("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts",
                                         QSettings::NativeFormat);
	QStringList keys = settings->allKeys();
	if (keys.isEmpty())
	{
		delete settings;
		settings = new QSettings("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Fonts",
                                         QSettings::NativeFormat);
		keys = settings->allKeys();
	}
	
	SysFontType typeA = sysFontTTF;
	for (int i = 0; i < keys.size(); i++)
	{
		QString keyA = keys[i];
		if (!keyA.contains("(TrueType)"))
			continue ;
			
		bool boldA = false;	
		if (keyA.contains("Bold"))
			boldA = true;
		
		bool italicA = false;
		if (keyA.contains("Italic"))
			italicA = true;
		
		QString fn = settings->value(keyA).toString();
		if (fn.endsWith(".ttc", Qt::CaseInsensitive))
			typeA = sysFontTTC;
		else
			typeA = sysFontTTF;
			
		QString tmpfnf = QString("%1/%2").arg(winFontDir).arg(fn);
	
		QString tmp = keyA;
		tmp.remove("(TrueType)");
		tmp.remove(QRegExp("\\s+"));
	
		QByteArray ba = tmp.toLocal8Bit();
		XWString * nameA = new XWString(ba.data(), ba.size());
		ba = QFile::encodeName(tmpfnf);
		XWString * fileNameA = new XWString(ba.data(), ba.size());

  		SysFontInfo * fi = new SysFontInfo(nameA, boldA, italicA, fileNameA, typeA, i);
  		fonts->append(fi);
	}
	
	delete settings;
#endif //Q_OS_WIN
}


SysFontInfo *SysFontList::find(XWString *name)
{
	XWString *name2;
  bool bold, italic;
  SysFontInfo *fi;
  char c;
  int n, i;

  name2 = name->copy();

  // remove space, comma, dash chars
  i = 0;
  while (i < name2->getLength()) 
  {
    c = name2->getChar(i);
    if (c == ' ' || c == ',' || c == '-') 
    {
      name2->del(i);
    } 
    else 
    {
      ++i;
    }
  }
  n = name2->getLength();

  // remove trailing "MT" (Foo-MT, Foo-BoldMT, etc.)
  if (n > 2 && !strcmp(name2->getCString() + n - 2, "MT")) 
  {
    name2->del(n - 2, 2);
    n -= 2;
  }

  // look for "Regular"
  if (n > 7 && !strcmp(name2->getCString() + n - 7, "Regular")) 
  {
    name2->del(n - 7, 7);
    n -= 7;
  }

  // look for "Italic"
  if (n > 6 && !strcmp(name2->getCString() + n - 6, "Italic")) 
  {
    name2->del(n - 6, 6);
    italic = true;
    n -= 6;
  } 
  else {
    italic = false;
  }

  // look for "Bold"
  if (n > 4 && !strcmp(name2->getCString() + n - 4, "Bold")) 
  {
    name2->del(n - 4, 4);
    bold = true;
    n -= 4;
  } 
  else 
  {
    bold = false;
  }

  // remove trailing "MT" (FooMT-Bold, etc.)
  if (n > 2 && !strcmp(name2->getCString() + n - 2, "MT")) 
  {
    name2->del(n - 2, 2);
    n -= 2;
  }

  // remove trailing "PS"
  if (n > 2 && !strcmp(name2->getCString() + n - 2, "PS")) 
  {
    name2->del(n - 2, 2);
    n -= 2;
  }

  // remove trailing "IdentityH"
  if (n > 9 && !strcmp(name2->getCString() + n - 9, "IdentityH")) 
  {
    name2->del(n - 9, 9);
    n -= 9;
  }

  // search for the font
  fi = NULL;
  	
  XWString * alias = remap(name2, bold, italic);  	
  fi = 0;
  for (i = 0; i < fonts->getLength(); ++i) 
  {
   	fi = (SysFontInfo *)fonts->get(i);
   	if (fi->match(name2, bold, italic)) 
      break;
      		
    if (alias && fi->match(alias, bold, italic))
     	break;
     	
    if (fi->match(name2, false, italic)) 
      break;
      
    if (fi->match(name2, false, false)) 
      break;
      	
   	fi = 0;
  }

  delete name2;
  	if (alias)
  		delete alias;
  	return fi;
}

PSFontParam16::PSFontParam16(XWString *nameA, 
	                           int wModeA,
			                       XWString *psFontNameA, 
			                       XWString *encodingA) 
{
  name = nameA;
  wMode = wModeA;
  psFontName = psFontNameA;
  encoding = encodingA;
}

PSFontParam16::~PSFontParam16() 
{
	if (name)
  	delete name;
  	
  if (psFontName)
  	delete psFontName;
  	
  if (encoding)
  	delete encoding;
}

XWFontSettingPrivate::XWFontSettingPrivate()
{
	QString bindir = QCoreApplication::applicationDirPath();
    QString inifile = QString("%1/fontsetting.ini").arg(bindir);
    settings = new QSettings(inifile, QSettings::IniFormat, this);
    	
    initBuiltinFontTables();
    macRomanReverseMap = new XWNameToCharCode();
    for (int i = 255; i >= 0; --i)
    {
        if (macRomanEncoding[i])
            macRomanReverseMap->add(macRomanEncoding[i], i);
    }

    nameToUnicode = new XWNameToCharCode();
    cidToUnicodes = new XWHash(true);
    unicodeToUnicodes = new XWHash(true);
    residentUnicodeMaps = new XWHash();
    unicodeMaps = new XWHash(true);
	cMapDirs = new XWHash(true);
	
  	psResidentFonts16 = new XWList();
  	psResidentFontsCC = new XWList();
    antialias = true;
    cidToUnicodeCache = new XWCharCodeToUnicodeCache(cidToUnicodeCacheSize);
    unicodeToUnicodeCache = new XWCharCodeToUnicodeCache(unicodeToUnicodeCacheSize);
    unicodeMapCache = new XWUnicodeMapCache();
    cMapCache = new XWCMapCache(0);

    for (int i = 0; nameToUnicodeTab[i].name; ++i)
        nameToUnicode->add(nameToUnicodeTab[i].name, nameToUnicodeTab[i].u);

    XWUnicodeMap * map = new XWUnicodeMap("Latin1", false, latin1UnicodeMapRanges, latin1UnicodeMapLen);
    residentUnicodeMaps->add(map->getEncodingName(), map);
    map = new XWUnicodeMap("ASCII7", false, ascii7UnicodeMapRanges, ascii7UnicodeMapLen);
    residentUnicodeMaps->add(map->getEncodingName(), map);
    map = new XWUnicodeMap("Symbol", false, symbolUnicodeMapRanges, symbolUnicodeMapLen);
    residentUnicodeMaps->add(map->getEncodingName(), map);
    map = new XWUnicodeMap("ZapfDingbats", false, zapfDingbatsUnicodeMapRanges, zapfDingbatsUnicodeMapLen);
    residentUnicodeMaps->add(map->getEncodingName(), map);
    map = new XWUnicodeMap("UTF-8", true, &mapUTF8);
    residentUnicodeMaps->add(map->getEncodingName(), map);
    map = new XWUnicodeMap("UCS-2", true, &mapUCS2);
    residentUnicodeMaps->add(map->getEncodingName(), map);
    parseNameToUnicode();
    parseCIDToUnicode();
    parseUnicodeToUnicode();
    parseUnicodeMap();
    parseCMapDir();
    parsePSResidentFont("psResidentFont16", psResidentFonts16);
    parsePSResidentFont("psResidentFontCC", psResidentFontsCC);
    
    if (settings->contains("antialias"))
    	antialias = settings->value("antialias").toBool();
    	

	sysFontList = new SysFontList;
}

XWFontSettingPrivate::~XWFontSettingPrivate()
{
	freeBuiltinFontTables();
	if (macRomanReverseMap)
		delete macRomanReverseMap;
		
	if (nameToUnicode)
		delete nameToUnicode;
		
	deleteXWHash(cidToUnicodes, XWString);
	deleteXWHash(unicodeToUnicodes, XWString);
    deleteXWHash(residentUnicodeMaps, XWUnicodeMap);
    deleteXWHash(unicodeMaps, XWString);
    
    XWHashIter *iter = 0;
    XWString * key = 0;
  	XWList *list = 0;
  	cMapDirs->startIter(&iter);
  	while (cMapDirs->getNext(&iter, &key, (void **)&list)) 
  	{
    	deleteXWList(list, XWString);
  	}
    
  	deleteXWList(psResidentFonts16, PSFontParam16);
  	deleteXWList(psResidentFontsCC, PSFontParam16);
  	
  	if (cidToUnicodeCache)
  		delete cidToUnicodeCache;
  		
  	if (unicodeToUnicodeCache)
  		delete unicodeToUnicodeCache;
  		
  	if (unicodeMapCache)
  		delete unicodeMapCache;
  		
  	if (cMapCache)
  		delete cMapCache;
  		

	if (sysFontList)
		delete sysFontList;
}

void XWFontSettingPrivate::parseCIDToUnicode()
{
	XWFontSea sea;
	settings->beginGroup("cidToUnicode");
	QStringList allkeys = settings->allKeys();
	QTextCodec * codec = QTextCodec::codecForLocale();
	for (int i = 0; i < allkeys.size(); i++)
	{
		QString c = allkeys[i];
		QString f = settings->value(c).toString();
			
		f = sea.findCMap(f);
		if (f.isEmpty())
			continue;
			
		QByteArray cba = codec->fromUnicode(c);
    	QByteArray fba = QFile::encodeName(f);
    	
    	XWString * collection = new XWString(cba.data(), cba.size());
    	XWString * name = new XWString(fba.data(), fba.size());
    	XWString * old = (XWString *)cidToUnicodes->remove(collection);
    	if (old)
        	delete old;

    	cidToUnicodes->add(collection, name);
	}
	
	settings->endGroup();
}

void XWFontSettingPrivate::parseCMapDir()
{
	settings->beginGroup("cMapDir");
	QStringList cmaplist = settings->allKeys();
	QString bindir = QCoreApplication::applicationDirPath();
	QChar envsep = envSep();
	for (int i = 0; i < cmaplist.size(); i++)
	{
		QString cmap = cmaplist[i];
		QString cmapvalue = settings->value(cmap).toString();
		QByteArray cba = QFile::encodeName(cmap);
		XWString * collection = new XWString(cba.constData());
		XWList * list = (XWList*)cMapDirs->lookup(collection);
		if (!list)
    	{
    		list = new XWList;
    		cMapDirs->add(collection, list);
    	}
    	else
    		delete collection;
    	
		QStringList vlist = cmapvalue.split(envsep, QString::SkipEmptyParts);
		for (int j = 0; j < vlist.size(); j++)
		{
			QString dvalue = vlist[j];
			dvalue = getAbsoluteDir(bindir, dvalue);
    		QByteArray dba = QFile::encodeName(dvalue);
    		XWString * dir = new XWString(dba.data(), dba.size());
    		list->append(dir);
    	}
	}
	
	settings->endGroup();
}

void XWFontSettingPrivate::parseNameToUnicode()
{
	XWFontSea sea;
	settings->beginGroup("nameToUnicode");
	QStringList flist = settings->allKeys();
	char buf[256];
    int  line = 1;
    int  u = 0;
    char *tok1, *tok2;
	for (int i = 0; i < flist.size(); i++)
	{
		QString filename = flist[i];
		QFile * f = sea.openMap(filename);
    	if (!f)
        	continue;
        	
        line = 1;
        u = 0;
        while (f->readLine(buf, sizeof(buf)) > 0)
    	{
    		if (buf[0] == '\n')
    			continue;
    			
        	tok1 = strtok(buf, " \t\r\n");
        	tok2 = strtok(NULL, " \t\r\n");
        	if (tok1 && tok2)
        	{
            	sscanf(tok1, "%x", &u);
            	nameToUnicode->add(tok2, u);
        	}
        	else
        	{
            	QString msg = QString(tr("bad line in map file '%1'(%2)\n")).arg(filename).arg(line);
            	xwApp->error(msg);
        	}

        	++line;
    	}

    	f->close();
    	delete f;
	}
	
	settings->endGroup();
}

void XWFontSettingPrivate::parsePSResidentFont(const QString & key, XWList * list)
{
	settings->beginGroup(key);
	QStringList flist = settings->allKeys();
	QTextCodec * codec = QTextCodec::codecForLocale();
	for (int i = 0; i < flist.size(); i++)
	{
		QString fontvar = flist[i];
		QString tmp = settings->value(fontvar).toString();
		QStringList tmplist = tmp.split(QRegExp("\\s+"), QString::SkipEmptyParts);
		if (tmplist.size() != 3)
			continue;
				
		int wMode = 0;
		if (tmplist.at(0) == "V")
			wMode = 1;
				
		QByteArray cba = codec->fromUnicode(fontvar);
		QByteArray fnba = codec->fromUnicode(tmplist.at(1));
		QByteArray enba = codec->fromUnicode(tmplist.at(2));
			
		XWString * n = new XWString(cba.data(), cba.size());
		XWString * psn = new XWString(fnba.data(), fnba.size());
		XWString * en = new XWString(enba.data(), enba.size());
			
		PSFontParam16 * param = new PSFontParam16(n, wMode, psn, en);
		list->append(param);
	}
	settings->endGroup();
}

void XWFontSettingPrivate::parseUnicodeMap()
{
    XWFontSea sea;
    settings->beginGroup("unicodeMap");
	QStringList enclist = settings->allKeys();
	for (int i = 0; i < enclist.size(); i++)
	{
		QString encvar = enclist[i];
		QString filevar = settings->value(encvar).toString();
		filevar = sea.findMap(filevar);
    	if (filevar.isEmpty())
        	continue;
        
        QByteArray encba = encvar.toAscii();
    	QByteArray fileba = QFile::encodeName(filevar);

    	XWString * encodingName = new XWString(encba.constData());
    	XWString * name = new XWString(fileba.constData());
    	XWString * old = (XWString *)unicodeMaps->remove(encodingName);
    	if (old)
        	delete old;

    	unicodeMaps->add(encodingName, name);
	}
	
	settings->endGroup();
}

void XWFontSettingPrivate::parseUnicodeToUnicode()
{
    XWFontSea sea;
    settings->beginGroup("unicodeToUnicode");
	QStringList fontlist = settings->allKeys();
    for (int i = 0; i < fontlist.size(); i++)
    {
    	QString fontvar = fontlist[i];
    	QString filevar = settings->value(fontvar).toString();
    	filevar = sea.findMap(filevar);
    	if (filevar.isEmpty())
    		continue;

    	QByteArray fontba = fontvar.toAscii();
    	QByteArray fileba = QFile::encodeName(filevar);
    	XWString * font = new XWString(fontba.constData());
    	XWString * file = new XWString(fileba.constData());
    	XWString * old = (XWString *)unicodeToUnicodes->remove(font);
    	if (old)
        	delete old;

    	unicodeToUnicodes->add(font, file);
    }
    
    settings->endGroup();
}

static XWFontSettingPrivate * fontSetting = 0;

XWFontSetting::XWFontSetting()
{
	if (!fontSetting)
		fontSetting = new XWFontSettingPrivate;
		
	settings = fontSetting->settings;
}

XWString * XWFontSetting::findCCFontFile(XWString *collection)
{
	settings->beginGroup("fontFileCC");
	QString name = collection->toQString();
	XWString * path = 0;
	
	if (settings->contains(name))
	{
		QString fn = settings->value(name).toString();
		if (!fn.isEmpty())
		{
			XWFontSea sea;
			fn = sea.findFile(fn);
			if (!fn.isEmpty())
			{
				QByteArray ba = QFile::encodeName(fn);
				path = new XWString(ba.constData());
			}
		}
	}
	
	settings->endGroup();
	return path;
}

QFile * XWFontSetting::findCMapFile(XWString *collection, XWString *cMapName)
{
	XWFontSea sea;
	QFile * f = 0;
	QString qn = QFile::decodeName(cMapName->getCString());
	XWList * list = (XWList*)(fontSetting->cMapDirs->lookup(collection));
	if (!list)
		f =  sea.openCMap(qn);
	else
	{
		for (int i = 0; i < list->getLength(); ++i)
		{
			XWString * dir = (XWString *)list->get(i);
			QString qfn = QFile::decodeName(dir->getCString());
			qfn += QString("/%1").arg(qn);
			f = sea.openCMap(qfn);
			if (f)
				break;
		}
	}
	return f;
}

XWString * XWFontSetting::findFontFile(XWString *fontName)
{
	QString name = fontName->toQString();
	settings->beginGroup("fontFile");
	XWString * ret = 0;
	if (settings->contains(name))
	{
		QString qn = settings->value(name).toString();
		XWFontSea sea;
		qn = sea.findFile(qn);	
	
		if (!qn.isEmpty())
		{
			QByteArray ba = QFile::encodeName(qn);
			ret = new XWString(ba.constData());
		}	
	}
	settings->endGroup();
	return ret;
}

XWString * XWFontSetting::findSystemFontFile(XWString *fontName, 
	                                           SysFontType *type,
			                                       int *fontNum)
{
	if (!fontSetting->sysFontList)
	{
		return 0;
	}
		
	SysFontInfo *fi = fontSetting->sysFontList->find(fontName);
	XWString *path = 0;
	if (fi)
	{
		path = fi->path->copy();
    *type = fi->type;
    *fontNum = fi->fontNum;
	}
	return path;
}

QFile * XWFontSetting::findToUnicodeFile(XWString *name)
{
	XWFontSea sea;
	QString qn = QFile::decodeName(name->getCString());
	QFile * ret = sea.openCMap(qn);
	return ret;
}

bool XWFontSetting::getAntialias()
{
	bool ret = fontSetting->antialias;
	return ret;
}

XWCharCodeToUnicode * XWFontSetting::getCIDToUnicode(XWString *collection)
{
	XWCharCodeToUnicode *ctu = fontSetting->cidToUnicodeCache->getCharCodeToUnicode(collection);
	if (!ctu)
	{
		XWString * fileName = (XWString *)(fontSetting->cidToUnicodes->lookup(collection));
		if (fileName)
			ctu = XWCharCodeToUnicode::parseCIDToUnicode(fileName, collection);
			
		if (ctu)	
			fontSetting->cidToUnicodeCache->add(ctu);
	}
	
	return ctu;
}

XWCMap * XWFontSetting::getCMap(XWString *collection, XWString *cMapName)
{
	XWCMap * cMap = fontSetting->cMapCache->getCMap(collection, cMapName);
	return cMap;
}

int XWFontSetting::getMacRomanCharCode(char *charName)
{
	int ret = fontSetting->macRomanReverseMap->lookup(charName);
	return ret;
}

XWString * XWFontSetting::getPSResidentFont(XWString *fontName)
{
	settings->beginGroup("psResidentFont");
	QString name = fontName->toQString();
	XWString * path = 0;
	
	if (settings->contains(name))
	{
		QString fn = settings->value(name).toString();
		if (!fn.isEmpty())
		{
			XWFontSea sea;
			fn = sea.findFile(fn);
			if (!fn.isEmpty())
			{
				QByteArray ba = QFile::encodeName(fn);
				path = new XWString(ba.constData());
			}
		}
	}
	
	settings->endGroup();
	return path;
}

PSFontParam16 * XWFontSetting::getPSResidentFont16(XWString *fontName, int wMode)
{
	PSFontParam16 *p = NULL;
	for (int i = 0; i < fontSetting->psResidentFonts16->getLength(); ++i) 
	{
    p = (PSFontParam16 *)(fontSetting->psResidentFonts16->get(i));
    if (!(p->name->cmp(fontName)) && p->wMode == wMode) 
    {
      break;
    }
    p = NULL;
  }
  return p;
}

PSFontParam16 * XWFontSetting::getPSResidentFontCC(XWString *collection, int wMode)
{
	PSFontParam16 *p = NULL;
	for (int i = 0; i < fontSetting->psResidentFontsCC->getLength(); ++i) 
	{
    p = (PSFontParam16 *)(fontSetting->psResidentFontsCC->get(i));
    if (!(p->name->cmp(collection)) && p->wMode == wMode) 
    {
      break;
    }
    p = NULL;
  }
  return p;
}

XWList * XWFontSetting::getPSResidentFonts()
{
	settings->beginGroup("psResidentFont");
	XWList * names = new XWList();
	QStringList keys = settings->allKeys();	
	settings->endGroup();
	for (int i = 0; i < keys.size(); i++)
	{
		QString key = keys[i];
		QByteArray ba = QFile::encodeName(key);
		XWString * name = new XWString(ba.data());
		names->append(name);
	}
	return names;
}

XWUnicodeMap * XWFontSetting::getResidentUnicodeMap(XWString *encodingName)
{
	XWUnicodeMap * map = (XWUnicodeMap *)(fontSetting->residentUnicodeMaps->lookup(encodingName));
	if (map) 
    	map->incRefCnt();
 	return map;
}

XWUnicodeMap * XWFontSetting::getTextEncoding(XWString *encodingName)
{
	XWUnicodeMap * ret = getUnicodeMap2(encodingName);
 	return ret;
}

XWUnicodeMap * XWFontSetting::getUnicodeMap(XWString *encodingName)
{
	XWUnicodeMap * ret = getUnicodeMap2(encodingName);
 	return ret;
}

XWUnicodeMap * XWFontSetting::getUnicodeMap2(XWString *encodingName)
{
	XWUnicodeMap *map = getResidentUnicodeMap(encodingName);
	if (!map)
		map = fontSetting->unicodeMapCache->getUnicodeMap(encodingName);
	return map;
}

QFile * XWFontSetting::getUnicodeMapFile(XWString *encodingName)
{
	XWString * fileName = (XWString *)(fontSetting->unicodeMaps->lookup(encodingName));
	if (!fileName)
	{
		return 0;
	}
		
	QString fn = QFile::decodeName(fileName->getCString());
	QFile * f = new QFile(fn);
	if (!f)
	{
		return 0;
	}
		
	if (!f->open(QIODevice::ReadOnly))
	{
		delete f;
		f = 0;
	}
	return f;
}

XWCharCodeToUnicode * XWFontSetting::getUnicodeToUnicode(XWString *fontName)
{
	XWHashIter *iter = 0;
  	XWString * fontPattern = 0;
  	XWString * fileName = 0;
  	
  	XWHash * unicodeToUnicodes = fontSetting->unicodeToUnicodes;
  	while (unicodeToUnicodes->getNext(&iter, &fontPattern, (void **)&fileName)) 
  	{
    	if (strstr(fontName->getCString(), fontPattern->getCString())) 
    	{
      		unicodeToUnicodes->killIter(&iter);
      		break;
    	}
    	fileName = 0;
  	}
  	
  	XWCharCodeToUnicode * ctu = 0;
  	if (fileName) 
  	{
    	if (!(ctu = fontSetting->unicodeToUnicodeCache->getCharCodeToUnicode(fileName))) 
    	{
      		if ((ctu = XWCharCodeToUnicode::parseUnicodeToUnicode(fileName))) 
				fontSetting->unicodeToUnicodeCache->add(ctu);
    	}
  	} 
  	return ctu;
}

int XWFontSetting::mapNameToUnicode(char * charName)
{
	int ret = fontSetting->nameToUnicode->lookup(charName);
	return ret;
}

void XWFontSetting::init()
{
	if (!fontSetting)
		fontSetting = new XWFontSettingPrivate;
}

void XWFontSetting::quit()
{
	fontSettingMutex.lock();
	if (fontSetting)
		delete fontSetting;
		
	fontSetting = 0;
	fontSettingMutex.unlock();
}

QStringList XWFontSetting::getCidToUnicodes()
{
	QStringList ret;
	settings->beginGroup("cidToUnicode");
	QStringList allkeys = settings->allKeys();
	for (int i = 0; i < allkeys.size(); i++)
	{
		QString c = allkeys[i];
		QString f = settings->value(c).toString();
		QString tmp = QString("%1,%2").arg(c).arg(f);
		ret << tmp;
	}
	settings->endGroup();
	return ret;
}

QStringList XWFontSetting::getCMapDirs()
{
	QStringList ret;
	settings->beginGroup("cMapDir");
	QStringList allkeys = settings->allKeys();
	for (int i = 0; i < allkeys.size(); i++)
	{
		QString c = allkeys[i];
		QString f = settings->value(c).toString();
		QString tmp = QString("%1,%2").arg(c).arg(f);
		ret << tmp;
	}
	settings->endGroup();
	return ret;
}

QStringList XWFontSetting::getFonts()
{
	settings->beginGroup("fontFile");
	QStringList ret = settings->allKeys();
	settings->endGroup();
	return ret;
}

QString XWFontSetting::getFontFile(const QString & fontname)
{
	QString ret;
	settings->beginGroup("fontFile");
	if (settings->contains(fontname))
		ret = settings->value(fontname).toString();
	settings->endGroup();
	return ret;
}

QStringList XWFontSetting::getNameToUnicode()
{
	settings->beginGroup("psNamedFont16");
	QStringList ret = settings->allKeys();
	settings->endGroup();
	return ret;
}

QStringList XWFontSetting::getUnicodeMap()
{
	QStringList ret;
	settings->beginGroup("unicodeMap");
	QStringList allkeys = settings->allKeys();
	for (int i = 0; i < allkeys.size(); i++)
	{
		QString c = allkeys[i];
		QString f = settings->value(c).toString();
		QString tmp = QString("%1,%2").arg(c).arg(f);
		ret << tmp;
	}
	settings->endGroup();
	return ret;
}

QStringList XWFontSetting::getUnicodeToUnicodes()
{
	QStringList ret;
	settings->beginGroup("unicodeToUnicode");
	QStringList allkeys = settings->allKeys();
	for (int i = 0; i < allkeys.size(); i++)
	{
		QString c = allkeys[i];
		QString f = settings->value(c).toString();
		QString tmp = QString("%1,%2").arg(c).arg(f);
		ret << tmp;
	}
	settings->endGroup();
	return ret;
}

void XWFontSetting::setCidToUnicodes(const QStringList & list)
{
	if (fontSetting->cidToUnicodes)
		deleteXWHash(fontSetting->cidToUnicodes, XWString);
		
	fontSetting->cidToUnicodes = new XWHash(true);
	if (list.size() == 0)
	{
		settings->remove("cidToUnicode");
		return ;
	}
	
	settings->beginGroup("cidToUnicode");
	for (int i = 0; i < list.size(); i++)
	{
		QStringList tmp = list[i].split(",");
		if (tmp.size() == 2)
			settings->setValue(tmp.at(0), tmp.at(1));
	}
	settings->endGroup();
	fontSetting->parseCIDToUnicode();
}

void XWFontSetting::setCMapDirs(const QStringList & list)
{
	if (fontSetting->cMapDirs)
		deleteXWHash(fontSetting->cMapDirs, XWString);
		
	fontSetting->cMapDirs = new XWHash(true);
	if (list.size() == 0)
	{
		settings->remove("cMapDir");
		return ;
	}
	
	settings->beginGroup("cMapDir");
	for (int i = 0; i < list.size(); i++)
	{
		QStringList tmp = list[i].split(",");
		if (tmp.size() == 2)
			settings->setValue(tmp.at(0), tmp.at(1));
	}
	settings->endGroup();
	fontSetting->parseCMapDir();
}

void XWFontSetting::setFontFile(const QString & fontname, const QString & file)
{
	settings->beginGroup("fontFile");
	settings->setValue(fontname, file);
	settings->endGroup();
}

void XWFontSetting::setNameToUnicode(const QStringList & list)
{
	if (fontSetting->nameToUnicode)
		delete fontSetting->nameToUnicode;
	fontSetting->nameToUnicode = new XWNameToCharCode();
	if (list.size() == 0)
	{
		settings->remove("nameToUnicode");
		return ;
	}
	
	settings->beginGroup("nameToUnicode");
	for (int i = 0; i < list.size(); i++)
	{
		QString file = list[i];
		settings->setValue(file, 1);
	}
	settings->endGroup();
	fontSetting->parseNameToUnicode();
}

void XWFontSetting::setUnicodeMap(const QStringList & list)
{
	if (fontSetting->unicodeMaps)
		deleteXWHash(fontSetting->unicodeMaps, XWString);
	fontSetting->unicodeMaps = new XWHash(true);
	if (list.size() == 0)
	{
		settings->remove("unicodeMap");
		return ;
	}
	
	settings->beginGroup("unicodeMap");
	for (int i = 0; i < list.size(); i++)
	{
		QStringList tmp = list[i].split(",");
		if (tmp.size() == 2)
			settings->setValue(tmp.at(0), tmp.at(1));
	}
	settings->endGroup();
	fontSetting->parseUnicodeMap();
}

void XWFontSetting::setUnicodeToUnicodes(const QStringList & list)
{
	if (fontSetting->unicodeToUnicodes)
		deleteXWHash(fontSetting->unicodeToUnicodes, XWString);
	fontSetting->unicodeToUnicodes = new XWHash(true);
	if (list.size() == 0)
	{
		settings->remove("setUnicodeToUnicodes");
		return ;
	}
	
	settings->beginGroup("unicodeToUnicode");
	for (int i = 0; i < list.size(); i++)
	{
		QStringList tmp = list[i].split(",");
		if (tmp.size() == 2)
			settings->setValue(tmp.at(0), tmp.at(1));
	}
	settings->endGroup();
	fontSetting->parseUnicodeToUnicode();
}

