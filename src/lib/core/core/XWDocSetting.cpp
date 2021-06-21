/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <QMutex>
#include <QByteArray>
#include <QFile>
#include <QTextCodec>
#include <QRegExp>
#include "XWString.h"
#include "XWList.h"
#include "XWHash.h"
#include "XWStringUtil.h"
#include "XWDocSea.h"
#include "XWPaper.h"
#include "XWUnicodeMap.h"
#include "XWFontSetting.h"
#include "XWDocSetting.h"

static QMutex docSettingMutex;

XWDocSettingPrivate::XWDocSettingPrivate()
{
	psFile = 0;
	psPaperWidth = defPaperWidth;
  	psPaperHeight = defPaperHeight;
  	psImageableLLX = psImageableLLY = 0;
  	psImageableURX = psPaperWidth;
  	psImageableURY = psPaperHeight;
  	psUncompressPreloadedImages = false;
  	psCrop = true;
  	psExpandSmaller = false;
  	psShrinkLarger = true;
  	psCenter = true;
  	psDuplex = false;
  	psLevel = psLevel2;
  	psEmbedType1 = true;
  	psEmbedTrueType = true;
  	psEmbedCIDPostScript = true;
  	psEmbedCIDTrueType = true;
  	psFontPassthrough = false;
  	psPreload = false;
  	psOPI = false;
  	psASCIIHex = false;
  	overprintPreview = false;
  	textEncoding = new XWString("UTF-8");
#ifdef Q_OS_WIN
  	textEOL = eolDOS;
#elif defined(Q_OS_MAC)
	textEOL = eolMac;
#else
	textEOL = eolUnix;
#endif
  	textPageBreaks = true;
  	textKeepTinyChars = false;
  	vectorAntialias = true;
  	strokeAdjust = true;
  	urlCommand = 0;
  	movieCommand = 0;
  	mapNumericCharNames = true;
  	mapUnknownCharNames = false;
  	printCommands = false;
  	
  	plugins = new XWList();
  	securityHandlers = new XWList();
  	
  	psRasterResolution = 300;
  	psRasterMono = false;
  	antialiasPrinting = false;
  	screenType = SCREEN_UNSET;
  	screenSize = -1;
  	screenDotRadius = -1;
  	screenGamma = 1.0;
  	screenBlackThreshold = 0.0;
  	screenWhiteThreshold = 1.0;
  	
  	parseCommand("psFile", &psFile);
  	parsePSPaperSize();
  	parsePSImageableArea();
  	
  	parseYesNo("overprintPreview", overprintPreview);
  	parseYesNo("psUncompressPreloadedImages", psUncompressPreloadedImages);
  	parseYesNo("psCrop", psCrop);
  	parseYesNo("psExpandSmaller", psExpandSmaller);
  	parseYesNo("psShrinkLarger", psShrinkLarger);
  	parseYesNo("psCenter", psCenter);
  	parseYesNo("psDuplex", psDuplex);
  	parsePSLevel();
  	parseYesNo("psEmbedType1Fonts", psEmbedType1);
  	parseYesNo("psEmbedTrueTypeFonts", psEmbedTrueType);
  	parseYesNo("psEmbedCIDPostScriptFonts", psEmbedCIDPostScript);
  	parseYesNo("psEmbedCIDTrueTypeFonts", psEmbedCIDTrueType);
  	parseYesNo("psFontPassthrough", psFontPassthrough);
  	parseYesNo("psPreload", psPreload);
  	parseYesNo("psOPI", psOPI);
  	parseYesNo("psASCIIHex", psASCIIHex);
  	parseCommand("textEncoding", &textEncoding);
  	parseTextEOL();
  	parseYesNo("textPageBreaks", textPageBreaks);
  	parseYesNo("textKeepTinyChars", textKeepTinyChars);
  	parseYesNo("vectorAntialias", vectorAntialias);
  	parseYesNo("strokeAdjust", strokeAdjust);
  	parseCommand("urlCommand", &urlCommand);
  	parseCommand("movieCommand", &movieCommand);
  	parseYesNo("mapNumericCharNames", mapNumericCharNames);
  	parseYesNo("mapUnknownCharNames", mapUnknownCharNames);
  	parseYesNo("printCommands", printCommands);
  	
  	pdfVersion = 4;
  	parseInt("pdfVersion", pdfVersion);
  	pdfCompressLevel = 9;
  	parseInt("pdfCompressLevel", pdfCompressLevel);
  	pdfKeyBits = 40;
  	parseInt("pdfKeyBits", pdfKeyBits);
  	pdfPermission = 0x003C;
  	parseInt("pdfPermission", pdfPermission);
  	parsePDFPaperSize();
  	pdfEncrypt = false;
  	parseYesNo("pdfEncrypt", pdfEncrypt);
  	pdfMag = 1.0;
  	parseDouble("pdfMag", pdfMag);
  	pdfPrecision = 2;
  	parseInt("pdfPrecision", pdfPrecision);
  	pdfIgnoreColors = false;
  	parseYesNo("pdfIgnoreColors", pdfIgnoreColors);
  	pdfAnnotGrow = 0.0;
  	parseDouble("pdfAnnotGrow", pdfAnnotGrow);
  	pdfCIDFontFixedPitch = false;
  	parseYesNo("pdfCIDFontFixedPitch", pdfCIDFontFixedPitch);
  	pdfMapFirst = false;
  	parseYesNo("pdfMapFirst", pdfMapFirst);
  	pdfNoDestRemove = true;
  	parseYesNo("pdfNoDestRemove", pdfNoDestRemove);
  	pdfTPICTransparent = true;
  	parseYesNo("pdfTPICTransparent", pdfTPICTransparent);
  	parseScreen();
  	parseYesNo("psRasterMono", psRasterMono);
  	parseYesNo("antialiasPrinting", antialiasPrinting);
}

XWDocSettingPrivate::~XWDocSettingPrivate()
{
	if (psFile) 
    	delete psFile;
  	
  	if (textEncoding)
  		delete textEncoding;
  		
  	if (urlCommand) 
    	delete urlCommand;
  	
  	if (movieCommand) 
    	delete movieCommand;
    	
    delete securityHandlers;
  	delete plugins;
}

void XWDocSettingPrivate::parseCommand(const QString & var, XWString **val)
{
	XWDocSea sea;
	QString p = sea.varValue(var);
	if (p.isEmpty())
		return ;
		
	if (p[0] == QChar('"'))
		p.remove(0, 1);
		
	if (p[p.length() - 1] == QChar('"'))
		p.remove(p.length() - 1, 1);
		
	QTextCodec * codec = QTextCodec::codecForLocale();
	QByteArray pba = codec->fromUnicode(p);
	if (*val)
		delete *val;
		
	*val = new XWString(pba.constData());
}

void XWDocSettingPrivate::parseDouble(const QString & var, double & v)
{
	XWDocSea sea;
	QString p = sea.varValue(var);
	if (p.isEmpty())
		return ;
		
	bool ok = true;
	double tmpv = p.toDouble(&ok);
	if (ok)
		v = tmpv;
}

void XWDocSettingPrivate::parseInt(const QString & var, int & v)
{
	XWDocSea sea;
	QString p = sea.varValue(var);
	if (p.isEmpty())
		return ;
		
	bool ok = true;
	int tmpv = p.toInt(&ok);
	if (ok)
		v = tmpv;
}

void XWDocSettingPrivate::parsePDFPaperSize()
{
	pdfPaperWidth = 595.0;
	pdfPaperHeight = 842.0;
	pdfHoff = 72.0;
	pdfVoff = 72.0;
	XWDocSea sea;
	QString p = sea.varValue("pdfPaperSize");
	if (p.isEmpty())
		return ;
		
	QStringList list = p.split(QRegExp("\\s+"), QString::SkipEmptyParts);
	if (list.size() == 4)
	{
		if (list.size() == 4)
		{
			bool okw = true;
			bool okh = true;
			double tmpw =list.at(0).toDouble(&okw);
			double tmph =list.at(1).toDouble(&okh);
			bool okhoff = true;
			bool okvoff = true;
			double tmphoff = list.at(2).toDouble(&okhoff);
			double tmpvoff = list.at(3).toDouble(&okvoff);
			if (okw && okh && okhoff && okvoff)
			{
				pdfPaperWidth = tmpw;
    			pdfPaperHeight = tmph;
    			pdfHoff = tmphoff;
    			pdfVoff = tmpvoff;
			}
		}
	}
	else if (list.size() == 3)
	{
		XWPaper paper(list.at(0));
		pdfPaperWidth = paper.ptWidth();
    	pdfPaperHeight = paper.ptHeight();
    	bool okhoff = true;
		bool okvoff = true;
    	double tmphoff = list.at(1).toDouble(&okhoff);
		double tmpvoff = list.at(2).toDouble(&okvoff);
		if (okhoff && okvoff)
		{
			pdfHoff = tmphoff;
    		pdfVoff = tmpvoff;
		}
    }
}

void XWDocSettingPrivate::parsePSImageableArea()
{
	XWDocSea sea;
	QString p = sea.varValue("psImageableArea");
	if (p.isEmpty())
		return ;
		
	QStringList list = p.split(QRegExp("\\s+"), QString::SkipEmptyParts);
	if (list.size() != 4)
		return ;
		
	psImageableLLX = list.at(0).toInt();
  	psImageableLLY = list.at(1).toInt();
  	psImageableURX = list.at(2).toInt();
  	psImageableURY = list.at(3).toInt();
}

void XWDocSettingPrivate::parsePSLevel()
{
	XWDocSea sea;
	QString p = sea.varValue("psLevel");
	if (p.isEmpty())
		return ;
		
	if (p == "level1") 
    	psLevel = psLevel1;
  	else if (p == "level1sep") 
    	psLevel = psLevel1Sep;
  	else if (p == "level2") 
    	psLevel = psLevel2;
  	else if (p == "level2sep") 
    	psLevel = psLevel2Sep;
  	else if (p == "level3") 
    	psLevel = psLevel3;
  	else if (p == "level3Sep") 
    	psLevel = psLevel3Sep;
}

void XWDocSettingPrivate::parsePSPaperSize()
{
	XWDocSea sea;
	QString p = sea.varValue("psPaperSize");
	if (p.isEmpty())
		return ;
		
	if (p[0].isDigit())
	{
		QStringList list = p.split(QRegExp("\\s+"), QString::SkipEmptyParts);
		if (list.size() == 2)
		{
			bool okw = true;
			bool okh = true;
			int tmpw =list.at(0).toInt(&okw);
			int tmph =list.at(1).toInt(&okh);
			if (okw && okh)
			{
				psPaperWidth = tmpw;
    			psPaperHeight = tmph;
			}
		}
	}
	else
	{
		XWPaper paper(p);
		psPaperWidth = qRound(paper.ptWidth());
    	psPaperHeight = qRound(paper.ptHeight());
    }
}

void XWDocSettingPrivate::parseScreen()
{
	XWDocSea sea;
	QString tmp = sea.varValue("screenType");
	if (tmp == "dispersed")
		screenType = SCREEN_DISPERSED;
	else if (tmp == "clustered")
		screenType = SCREEN_CLUSTERED;
	else if (tmp == "stochasticClustered")
		screenType = SCREEN_STOCHASTICCLUSTERED;
		
	tmp = sea.varValue("screenSize");
	if (!tmp.isEmpty())
		screenSize = tmp.toInt();
		
	tmp = sea.varValue("screenDotRadius");
	if (!tmp.isEmpty())
		screenDotRadius = tmp.toInt();
		
	tmp = sea.varValue("screenGamma");
	if (!tmp.isEmpty())
		screenDotRadius = tmp.toDouble();
		
	tmp = sea.varValue("screenBlackThreshold");
	if (!tmp.isEmpty())
		screenBlackThreshold = tmp.toDouble();
		
	tmp = sea.varValue("screenWhiteThreshold");
	if (!tmp.isEmpty())
		screenWhiteThreshold = tmp.toDouble();
		
	tmp = sea.varValue("psRasterResolution");
	if (!tmp.isEmpty())
		psRasterResolution = tmp.toDouble();
}

void XWDocSettingPrivate::parseTextEOL()
{
	XWDocSea sea;
	QString p = sea.varValue("textEOL");
	if (p.isEmpty())
		return ;
		
	if (p == "unix")
		textEOL = eolUnix;
	else if (p == "dos")
		textEOL = eolDOS;
	else if (p == "mac")
		textEOL = eolMac;
}

void XWDocSettingPrivate::parseYesNo(const QString & var, bool & v)
{
	XWDocSea sea;
	QString p = sea.varValue(var);
	if (p.isEmpty())
		return ;
		
	if (p[0] == QChar('y') || p[0] == QChar('Y'))
		v = true;
	else
		v = false;
}

static XWDocSettingPrivate * docSetting = 0;

XWDocSetting::XWDocSetting()
{
	if (!docSetting)
		docSetting = new XWDocSettingPrivate;
}

bool XWDocSetting::getAntialiasPrinting()
{
	bool ret = docSetting->antialiasPrinting;
	return ret;
}

bool XWDocSetting::getMapNumericCharNames()
{
	bool ret = docSetting->mapNumericCharNames;
	return ret;
}

bool XWDocSetting::getMapUnknownCharNames()
{
	bool ret = docSetting->mapUnknownCharNames;
	return ret;
}

XWString * XWDocSetting::getMovieCommand()
{
	XWString * ret = docSetting->movieCommand;
	return ret;
}

bool XWDocSetting::getOverprintPreview()
{
	bool ret = docSetting->overprintPreview;
	return ret;
}

double XWDocSetting::getPDFAnnotGrow()
{
	double ret = docSetting->pdfAnnotGrow;
	return ret;
}

bool XWDocSetting::getPDFCIDFontFixedPitch()
{
	bool ret = docSetting->pdfCIDFontFixedPitch;
	return ret;
}

int XWDocSetting::getPDFCompressLevel()
{
	int ret = docSetting->pdfCompressLevel;
	return ret;
}

bool XWDocSetting::getPDFEncrypt()
{
	bool ret = docSetting->pdfEncrypt;
	return ret;
}

double XWDocSetting::getPDFHoff()
{
	double ret = docSetting->pdfHoff;
	return ret;
}

bool XWDocSetting::getPDFIgnoreColors()
{
	bool ret = docSetting->pdfIgnoreColors;
	return ret;
}

int XWDocSetting::getPDFKeyBits()
{
	int ret = docSetting->pdfKeyBits;
	return ret;
}

double XWDocSetting::getPDFMag()
{
	double ret = docSetting->pdfMag;
	return ret;
}

bool XWDocSetting::getPDFMapFirst()
{
	bool ret = docSetting->pdfMapFirst;
	return ret;
}

bool XWDocSetting::getPDFNoDestRemove()
{
	bool ret = docSetting->pdfNoDestRemove;
	return ret;
}

double XWDocSetting::getPDFPaperHeight()
{
	double ret = docSetting->pdfPaperHeight;
	return ret;
}

double XWDocSetting::getPDFPaperWidth()
{
	double ret = docSetting->pdfPaperWidth;
	return ret;
}

int XWDocSetting::getPDFPermission()
{
	int ret = docSetting->pdfPermission;
	return ret;
}

int XWDocSetting::getPDFPrecision()
{
	int ret = docSetting->pdfPrecision;
	return ret;
}

bool XWDocSetting::getPDFTPICTransparent()
{	
	bool ret = docSetting->pdfTPICTransparent;
	return ret;
}

int XWDocSetting::getPDFVersion()
{
	int ret = docSetting->pdfVersion;
	return ret;
}

double XWDocSetting::getPDFVoff()
{
	double ret = docSetting->pdfVoff;
	return ret;
}

bool XWDocSetting::getPrintCommands()
{
	int ret = docSetting->printCommands;
	return ret;
}

bool XWDocSetting::getPSASCIIHex()
{
	bool ret = docSetting->psASCIIHex;
	return ret;
}

bool XWDocSetting::getPSCenter()
{
	bool ret = docSetting->psCenter;
	return ret;
}

bool XWDocSetting::getPSCrop()
{
	bool ret = docSetting->psCrop;
	return ret;
}

bool XWDocSetting::getPSDuplex()
{
	bool ret = docSetting->psDuplex;
	return ret;
}

bool XWDocSetting::getPSEmbedCIDTrueType()
{
	bool ret = docSetting->psEmbedCIDTrueType;
	return ret;
}

bool XWDocSetting::getPSEmbedCIDPostScript()
{
	bool ret = docSetting->psEmbedCIDPostScript;
	return ret;
}

bool XWDocSetting::getPSEmbedTrueType()
{
	bool ret = docSetting->psEmbedTrueType;
	return ret;
}

bool XWDocSetting::getPSEmbedType1()
{
	bool ret = docSetting->psEmbedType1;
	return ret;
}

bool XWDocSetting::getPSExpandSmaller()
{
	bool ret = docSetting->psExpandSmaller;
	return ret;
}

XWString * XWDocSetting::getPSFile()
{
	XWString * ret = 0;
	if (docSetting->psFile)
		ret = docSetting->psFile->copy();
	return ret;
}

bool XWDocSetting::getPSFontPassthrough()
{
	bool ret = docSetting->psFontPassthrough;
	return ret;
}

void XWDocSetting::getPSImageableArea(int *llx, int *lly, int *urx, int *ury)
{
	*llx = docSetting->psImageableLLX;
  	*lly = docSetting->psImageableLLY;
  	*urx = docSetting->psImageableURX;
  	*ury = docSetting->psImageableURY;
}

PSLevel XWDocSetting::getPSLevel()
{
	PSLevel ret = docSetting->psLevel;
	return ret;
}

bool XWDocSetting::getPSOPI()
{
	bool ret = docSetting->psOPI;
	return ret;
}

int XWDocSetting::getPSPaperHeight()
{
	int ret = docSetting->psPaperHeight;
	return ret;
}

int XWDocSetting::getPSPaperWidth()
{
	int ret = docSetting->psPaperWidth;
	return ret;
}

bool XWDocSetting::getPSPreload()
{
	bool ret = docSetting->psPreload;
	return ret;
}

double XWDocSetting::getPSRasterResolution()
{
	bool ret = docSetting->psRasterResolution;
	return ret;
}

bool XWDocSetting::getPSRasterMono()
{
	bool ret = docSetting->psRasterMono;
	return ret;
}

bool XWDocSetting::getPSShrinkLarger()
{
	bool ret = docSetting->psShrinkLarger;
	return ret;
}

bool XWDocSetting::getPSUncompressPreloadedImages()
{
	bool ret = docSetting->psUncompressPreloadedImages;
	return ret;
}

double XWDocSetting::getScreenBlackThreshold()
{
	double ret = docSetting->screenBlackThreshold;
	return ret;
}

int XWDocSetting::getScreenDotRadius()
{
	int ret = docSetting->screenDotRadius;
	return ret;
}

double XWDocSetting::getScreenGamma()
{
	double ret = docSetting->screenGamma;
	return ret;
}

int  XWDocSetting::getScreenSize()
{
	int ret = docSetting->screenSize;
	return ret;
}

int  XWDocSetting::getScreenType()
{
	int ret = docSetting->screenType;
	return ret;
}

double XWDocSetting::getScreenWhiteThreshold()
{
	double ret = docSetting->screenWhiteThreshold;
	return ret;
}

bool XWDocSetting::getStrokeAdjust()
{
	bool ret = docSetting->strokeAdjust;
	return ret;
}

XWString * XWDocSetting::getTextEncodingName()
{
	XWString * ret = docSetting->textEncoding->copy();
	return ret;
}

XWUnicodeMap * XWDocSetting::getTextEncoding()
{
	XWFontSetting fontsetting;
	XWUnicodeMap * ret = fontsetting.getTextEncoding(docSetting->textEncoding);
	return ret;
}

EndOfLineKind XWDocSetting::getTextEOL()
{
	EndOfLineKind ret = docSetting->textEOL;
	return ret;
}

bool XWDocSetting::getTextKeepTinyChars()
{
	bool ret = docSetting->textKeepTinyChars;
	return ret;
}

bool XWDocSetting::getTextPageBreaks()
{
	bool ret = docSetting->textPageBreaks;
	return ret;	
}

XWString * XWDocSetting::getURLCommand()
{
	XWString * ret = docSetting->urlCommand;
	return ret;	
}

bool XWDocSetting::getVectorAntialias()
{
	bool ret = docSetting->vectorAntialias;
	return ret;	
}

void XWDocSetting::init()
{
	if (!docSetting)
		docSetting = new XWDocSettingPrivate;
}

void XWDocSetting::quit()
{
	docSettingMutex.lock();
	if (docSetting)
		delete docSetting;
		
	docSetting = 0;
	docSettingMutex.unlock();
}

void XWDocSetting::save()
{
	XWDocSea sea;
	sea.save();
}

void XWDocSetting::setMapNumericCharNames(bool map)
{
	docSetting->mapNumericCharNames = map;
	setBool("mapNumericCharNames", map);
}

void XWDocSetting::setMapUnknownCharNames(bool map)
{
	docSetting->mapUnknownCharNames = map;
	setBool("mapUnknownCharNames", map);
}

void XWDocSetting::setMovieCommand(const QString & cmd)
{
	if (docSetting->movieCommand)
			delete docSetting->movieCommand;
			
	docSetting->movieCommand = 0;
		
	XWDocSea sea;
	if (cmd.isEmpty())
	{
		sea.removeConfig("movieCommand");
		return ;
	}
	
	QTextCodec * codec = QTextCodec::codecForLocale();
	QByteArray pba = codec->fromUnicode(cmd);
	docSetting->movieCommand = new XWString(pba.constData());
		
	QString tmp = cmd;
	if (tmp[0] != QChar('"'))
		tmp.insert(0, QChar('"'));
		
	if (tmp[tmp.length() - 1] != QChar('"'))
		tmp.append(QChar('"'));
		
	sea.addConfig("movieCommand", tmp);
}

void XWDocSetting::setPDFAnnotGrow(double g)
{
	docSetting->pdfAnnotGrow = g;
	QString v = QString("%1").arg(g);
	XWDocSea sea;
	sea.addConfig("pdfAnnotGrow", v);
}

void XWDocSetting::setPDFCIDFontFixedPitch(bool e)
{
	docSetting->pdfCIDFontFixedPitch = e;
	setBool("pdfCIDFontFixedPitch", e);
}

void XWDocSetting::setPDFCompressLevel(int level)
{
	docSetting->pdfCompressLevel = level;
	QString v = QString("%1").arg(level);
	XWDocSea sea;
	sea.addConfig("pdfCompressLevel", v);
}

void XWDocSetting::setPDFEncrypt(bool e)
{
	docSetting->pdfEncrypt = e;
	setBool("pdfEncrypt", e);
}

void XWDocSetting::setPDFIgnoreColors(bool e)
{
	docSetting->pdfIgnoreColors = e;
	setBool("pdfIgnoreColors", e);
}

void XWDocSetting::setPDFKeyBits(int bits)
{
	docSetting->pdfKeyBits = bits;
	QString v = QString("%1").arg(bits);
	XWDocSea sea;
	sea.addConfig("pdfKeyBits", v);
}

void XWDocSetting::setPDFMag(double m)
{
	docSetting->pdfMag = m;
	QString v = QString("%1").arg(m);
	XWDocSea sea;
	sea.addConfig("pdfMag", v);
}

void XWDocSetting::setPDFMapFirst(bool e)
{
	docSetting->pdfMapFirst = e;
	setBool("pdfMapFirst", e);
}

void XWDocSetting::setPDFNoDestRemove(bool e)
{
	docSetting->pdfNoDestRemove = e;
	setBool("pdfNoDestRemove", e);
}

void XWDocSetting::setPDFPaperSize(const QString & size)
{
	if (size.isEmpty())
	{
		return ;
	}
		
	QStringList list = size.split(QRegExp("\\s+"), QString::SkipEmptyParts);
	if (list.size() == 4)
	{
		if (list.size() == 4)
		{
			bool okw = true;
			bool okh = true;
			double tmpw =list.at(0).toDouble(&okw);
			double tmph =list.at(1).toDouble(&okh);
			bool okhoff = true;
			bool okvoff = true;
			double tmphoff = list.at(2).toDouble(&okhoff);
			double tmpvoff = list.at(3).toDouble(&okvoff);
			if (okw && okh && okhoff && okvoff)
			{
				docSetting->pdfPaperWidth = tmpw;
    			docSetting->pdfPaperHeight = tmph;
    			docSetting->pdfHoff = tmphoff;
    			docSetting->pdfVoff = tmpvoff;
			}
		}
	}
	else if (list.size() == 3)
	{
		XWPaper paper(list.at(0));
		docSetting->pdfPaperWidth = paper.ptWidth();
    	docSetting->pdfPaperHeight = paper.ptHeight();
    	bool okhoff = true;
		bool okvoff = true;
    	double tmphoff = list.at(1).toDouble(&okhoff);
		double tmpvoff = list.at(2).toDouble(&okvoff);
		if (okhoff && okvoff)
		{
			docSetting->pdfHoff = tmphoff;
    		docSetting->pdfVoff = tmpvoff;
		}
    }
    
	XWDocSea sea;
	sea.addConfig("pdfPaperSize", size);
}

void XWDocSetting::setPDFPermission(int p)
{
	docSetting->pdfPermission = p;
	QString v = QString("%1").arg(p);
	XWDocSea sea;
	sea.addConfig("pdfPermission", v);
}

void XWDocSetting::setPDFPrecision(int preci)
{
	docSetting->pdfPrecision = preci;
	QString v = QString("%1").arg(preci);
	XWDocSea sea;
	sea.addConfig("pdfPrecision", v);
}

void XWDocSetting::setPDFTPICTransparent(bool e)
{
	docSetting->pdfTPICTransparent = e;
	setBool("pdfTPICTransparent", e);
}

void XWDocSetting::setPDFVersion(int version)
{
	docSetting->pdfVersion = version;
	QString v = QString("%1").arg(version);
	XWDocSea sea;
	sea.addConfig("pdfVersion", v);
}

void XWDocSetting::setPrintCommands(bool printCommandsA)
{
	docSetting->printCommands = printCommandsA;
	setBool("printCommands", printCommandsA);
}

void XWDocSetting::setPSASCIIHex(bool hex)
{
	docSetting->psASCIIHex = hex;
	setBool("psASCIIHex", hex);
}

void XWDocSetting::setPSCenter(bool center)
{
	docSetting->psCenter = center;
	setBool("psCenter", center);
}

void XWDocSetting::setPSCrop(bool crop)
{
	docSetting->psCrop = crop;
	setBool("psDuplex", crop);
}

void XWDocSetting::setPSDuplex(bool duplex)
{
	docSetting->psDuplex = duplex;
	setBool("psDuplex", duplex);
}

void XWDocSetting::setPSEmbedCIDPostScript(bool embed)
{
	docSetting->psEmbedCIDPostScript = embed;
	setBool("psEmbedCIDPostScriptFonts", embed);
}

void XWDocSetting::setPSEmbedCIDTrueType(bool embed)
{
	docSetting->psEmbedCIDTrueType = embed;
	setBool("psEmbedCIDTrueTypeFonts", embed);
}

void XWDocSetting::setPSEmbedTrueType(bool embed)
{
	docSetting->psEmbedTrueType = embed;
	setBool("psEmbedTrueTypeFonts", embed);
}

void XWDocSetting::setPSEmbedType1(bool embed)
{
	docSetting->psEmbedType1 = embed;
	setBool("psEmbedType1Fonts", embed);
}

void XWDocSetting::setPSExpandSmaller(bool expand)
{
	docSetting->psExpandSmaller = expand;
	setBool("psExpandSmaller", expand);
}

void XWDocSetting::setPSFile(const QString & file)
{
	if (docSetting->psFile)
		delete docSetting->psFile;
	docSetting->psFile = 0;
	XWDocSea sea;
	
	if (file.isEmpty())
	{
		sea.removeConfig("psFile");
		return ;
	}
	
	QTextCodec * codec = QTextCodec::codecForLocale();
	QByteArray pba = codec->fromUnicode(file);
	docSetting->psFile = new XWString(pba.constData());
	
	sea.addConfig("psFile", file);
}

void XWDocSetting::setPSImageableArea(int llx, int lly, int urx, int ury)
{
	docSetting->psImageableLLX = llx;
	docSetting->psImageableLLY = lly;
	docSetting->psImageableURX = urx;
	docSetting->psImageableURY = ury;
	QString v = QString("%1 %2 %3 %4").arg(llx).arg(lly).arg(urx).arg(ury);
	XWDocSea sea;
	sea.addConfig("psImageableArea", v);
}

void XWDocSetting::setPSLevel(PSLevel level)
{
	docSetting->psLevel = level;
	QString v;
	switch (level)
	{
		case psLevel1:
			v = "level1";
			break;
			
		case psLevel1Sep:
			v = "level1sep";
			break;
			
		case psLevel2:
			v = "level2";
			break;
			
		case psLevel2Sep:
			v = "level2sep";
			break;
			
		case psLevel3:
			v = "level3";
			break;
			
		case psLevel3Sep:
			v = "level3Sep";
			break;
	}
	
	XWDocSea sea;
	sea.addConfig("psLevel", v);
}

void XWDocSetting::setPSOPI(bool opi)
{
	docSetting->psOPI = opi;
	setBool("psOPI", opi);
}

void XWDocSetting::setPSPaperSize(const QString & size)
{
	if (size.isEmpty())
	{
		return ;
	}
		
	if (size[0].isDigit())
	{
		QStringList list = size.split(QRegExp("\\s+"), QString::SkipEmptyParts);
		if (list.size() == 2)
		{
			bool okw = true;
			bool okh = true;
			int tmpw =list.at(0).toInt(&okw);
			int tmph =list.at(1).toInt(&okh);
			if (okw && okh)
			{
				docSetting->psPaperWidth = tmpw;
    			docSetting->psPaperHeight = tmph;
			}
		}
	}
	else
	{
		XWPaper paper(size);
		docSetting->psPaperWidth = qRound(paper.ptWidth());
    	docSetting->psPaperHeight = qRound(paper.ptHeight());
    }
    
	XWDocSea sea;
	sea.addConfig("psPaperSize", size);
}

void XWDocSetting::setPSPreload(bool preload)
{
	docSetting->psPreload = preload;
	setBool("psPreload", preload);
}

void XWDocSetting::setPSShrinkLarger(bool shrink)
{
	docSetting->psShrinkLarger = shrink;
	setBool("psShrinkLarger", shrink);
}

void XWDocSetting::setScreenBlackThreshold(double thresh)
{
	docSetting->screenBlackThreshold = thresh;
	QString v = QString("%1").arg(thresh);
	XWDocSea sea;
	sea.addConfig("screenBlackThreshold", v);
}

void XWDocSetting::setScreenDotRadius(int r)
{
	docSetting->screenDotRadius = r;
	QString v = QString("%1").arg(r);
	XWDocSea sea;
	sea.addConfig("screenDotRadius", v);
}

void XWDocSetting::setScreenGamma(double gamma)
{
	docSetting->screenGamma = gamma;
	QString v = QString("%1").arg(gamma);
	XWDocSea sea;
	sea.addConfig("screenGamma", v);
}

void XWDocSetting::setScreenSize(int size)
{
	docSetting->screenSize = size;
	QString v = QString("%1").arg(size);
	XWDocSea sea;
	sea.addConfig("screenSize", v);
}

void XWDocSetting::setScreenType(int t)
{
	docSetting->screenType = t;
	QString v;
	switch (t)
	{
		case SCREEN_DISPERSED:
			v = "dispersed";
			break;
			
		case SCREEN_CLUSTERED:
			v = "clustered";
			break;
			
		case SCREEN_STOCHASTICCLUSTERED:
			v = "stochasticClustered";
			break;
			
		default:
			break;
	}
	
	XWDocSea sea;
	if (v.isEmpty())
		sea.removeConfig("screenType");
	else
		sea.addConfig("screenType", v);
}

void XWDocSetting::setScreenWhiteThreshold(double thresh)
{
	docSetting->screenWhiteThreshold = thresh;
	QString v = QString("%1").arg(thresh);
	XWDocSea sea;
	sea.addConfig("screenWhiteThreshold", v);
}

void XWDocSetting::setStrokeAdjust(bool e)
{
	docSetting->strokeAdjust = e;
	setBool("strokeAdjust", e);
}

void XWDocSetting::setTextEncoding(const QString & encodingName)
{
	if (docSetting->textEncoding)
		delete docSetting->textEncoding;
		
	docSetting->textEncoding = 0;
	QTextCodec * codec = QTextCodec::codecForLocale();
	QByteArray pba = codec->fromUnicode(encodingName);
	docSetting->textEncoding = new XWString(pba.constData());
	XWDocSea sea;
	sea.addConfig("textEncoding", encodingName);
}

void XWDocSetting::setTextEOL(EndOfLineKind kind)
{
	docSetting->textEOL = kind;
	QString v;
	switch (kind)
	{
		case eolUnix:
			v = "unix";
			break;
			
		case eolDOS:
			v = "dos";
			break;
			
		case eolMac:
			v = "mac";
			break;
	}
	
	XWDocSea sea;
	sea.addConfig("textEOL", v);
}

void XWDocSetting::setTextKeepTinyChars(bool keep)
{
	docSetting->textKeepTinyChars = keep;
	setBool("textKeepTinyChars", keep);
}

void XWDocSetting::setTextPageBreaks(bool pageBreaks)
{
	docSetting->textPageBreaks = pageBreaks;
	setBool("textPageBreaks", pageBreaks);
}

void XWDocSetting::setUrlCommand(const QString & cmd)
{
	if (docSetting->urlCommand)
		delete docSetting->urlCommand;
		
	docSetting->urlCommand = 0;
	
	XWDocSea sea;
	if (cmd.isEmpty())
	{
		sea.removeConfig("urlCommand");
		return ;
	}
	
	QTextCodec * codec = QTextCodec::codecForLocale();
	QByteArray pba = codec->fromUnicode(cmd);
	docSetting->urlCommand = new XWString(pba.constData());
		
	QString tmp = cmd;
	if (tmp[0] != QChar('"'))
		tmp.insert(0, QChar('"'));
		
	if (tmp[tmp.length() - 1] != QChar('"'))
		tmp.append(QChar('"'));
		
	sea.addConfig("urlCommand", tmp);
}

void XWDocSetting::setVectorAntialias(bool e)
{
	docSetting->vectorAntialias = e;
	setBool("vectorAntialias", e);
}

void XWDocSetting::setBool(const QString & var, bool e)
{
	XWDocSea sea;
	if (e)
		sea.addConfig(var, "yes");
	else
		sea.addConfig(var, "no");
}

