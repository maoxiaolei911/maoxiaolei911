/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWDOCSETTING_H
#define XWDOCSETTING_H

#include <QObject>
#include <QString>
#include <QStringList>
#include "XWGlobal.h"
#include "XWFontSetting.h"

#define defPaperWidth  595
#define defPaperHeight 842

class QFile;
class XWString;
class XWList;
class XWHash;
class XWUnicodeMap;

enum EndOfLineKind
{
    eolUnix,			// LF
    eolDOS,			// CR+LF
    eolMac			// CR
};

#define SCREEN_UNSET               0
#define SCREEN_DISPERSED           1
#define SCREEN_CLUSTERED           2
#define SCREEN_STOCHASTICCLUSTERED 3

class XW_CORE_EXPORT XWDocSetting
{
public:
	XWDocSetting();
	
	bool getAntialiasPrinting();
	bool getMapNumericCharNames();
	bool getMapUnknownCharNames();
	XWString *getMovieCommand();
	bool   getOverprintPreview();
	double getPDFAnnotGrow();
	bool   getPDFCIDFontFixedPitch();
	int    getPDFCompressLevel();
	bool   getPDFEncrypt();
	double getPDFHoff();
	bool   getPDFIgnoreColors();
	int    getPDFKeyBits();
	double getPDFMag();
	bool   getPDFMapFirst();
	bool   getPDFNoDestRemove();
	double getPDFPaperHeight();
	double getPDFPaperWidth();
	int    getPDFPermission();
	int    getPDFPrecision();
	bool   getPDFTPICTransparent();
	int    getPDFVersion();
	double getPDFVoff();
	bool getPrintCommands();
	bool getPSASCIIHex();
	bool getPSCenter();
	bool getPSCrop();
	bool getPSDuplex();
	bool getPSEmbedCIDTrueType();
	bool getPSEmbedCIDPostScript();
	bool getPSEmbedTrueType();
	bool getPSEmbedType1();
	bool getPSExpandSmaller();
	XWString * getPSFile();
	bool getPSFontPassthrough();
	void getPSImageableArea(int *llx, int *lly, int *urx, int *ury);
	PSLevel getPSLevel();
	bool getPSOPI();
	int  getPSPaperHeight();
	int  getPSPaperWidth();
	bool getPSPreload();
	double getPSRasterResolution();
	bool   getPSRasterMono();
	bool getPSShrinkLarger();
	bool getPSUncompressPreloadedImages();	
	double getScreenBlackThreshold();
	int  getScreenDotRadius();
	double getScreenGamma();
	int  getScreenSize();
	int  getScreenType();
	double getScreenWhiteThreshold();
	bool getStrokeAdjust();
	XWString * getTextEncodingName();
	XWUnicodeMap * getTextEncoding();
	EndOfLineKind getTextEOL();
	bool getTextKeepTinyChars();
	bool getTextPageBreaks();
	XWString *getURLCommand();
	bool getVectorAntialias();

	static void init();
	static void quit();

	void save();
	
	void setMapNumericCharNames(bool map);
  	void setMapUnknownCharNames(bool map);
  	void setMovieCommand(const QString & cmd);
  	void setPDFAnnotGrow(double g);
  	void setPDFCIDFontFixedPitch(bool e);
  	void setPDFCompressLevel(int level);
  	void setPDFEncrypt(bool e);
  	void setPDFIgnoreColors(bool e);
  	void setPDFKeyBits(int bits);
  	void setPDFMag(double m);
  	void setPDFMapFirst(bool e);
  	void setPDFNoDestRemove(bool e);
  	void setPDFPaperSize(const QString & size);
  	void setPDFPermission(int p);
  	void setPDFPrecision(int preci);
  	void setPDFTPICTransparent(bool e);
  	void setPDFVersion(int version);
	void setPrintCommands(bool printCommandsA);
	void setPSASCIIHex(bool hex);
	void setPSCenter(bool center);
	void setPSCrop(bool crop);
	void setPSDuplex(bool duplex);
	void setPSEmbedCIDPostScript(bool embed);
	void setPSEmbedCIDTrueType(bool embed);
	void setPSEmbedTrueType(bool embed);
	void setPSEmbedType1(bool embed);
	void setPSExpandSmaller(bool expand);
	void setPSFile(const QString & file);
	void setPSImageableArea(int llx, int lly, int urx, int ury);
	void setPSLevel(PSLevel level);
	void setPSOPI(bool opi);
	void setPSPaperSize(const QString & size);
	void setPSPreload(bool preload);
	void setPSShrinkLarger(bool shrink);
	void setScreenBlackThreshold(double thresh);
	void setScreenDotRadius(int r);
	void setScreenGamma(double gamma);
	void setScreenSize(int size);
	void setScreenType(int t);
	void setScreenWhiteThreshold(double thresh);
	void setStrokeAdjust(bool e);
	void setTextEncoding(const QString & encodingName);
	void setTextEOL(EndOfLineKind kind);
	void setTextKeepTinyChars(bool keep);
	void setTextPageBreaks(bool pageBreaks);
	void setUrlCommand(const QString & cmd);
	void setVectorAntialias(bool e);
	
	
private:
	void setBool(const QString & var, bool e);
};


class XWDocSettingPrivate : public QObject
{
	Q_OBJECT

public:
	XWDocSettingPrivate();
	~XWDocSettingPrivate();

public:
	XWString *psFile;
	int psPaperWidth;
	int psPaperHeight;
	int psImageableLLX,
        psImageableLLY,
        psImageableURX,
        psImageableURY;
		bool psUncompressPreloadedImages;
		
    bool psCrop;
    bool psExpandSmaller;
    bool psShrinkLarger;
    bool psCenter;
    bool psDuplex;
    PSLevel psLevel;
    bool psEmbedType1;
    bool psEmbedTrueType;
    bool psEmbedCIDPostScript;
    bool psEmbedCIDTrueType;
    bool psFontPassthrough;
    bool psPreload;
    bool psOPI;
    bool psASCIIHex;
    XWString *textEncoding;
    EndOfLineKind textEOL;
    bool textPageBreaks;
    bool textKeepTinyChars;
    bool vectorAntialias;
    bool strokeAdjust;
    XWString *urlCommand;
    XWString *movieCommand;
    bool mapNumericCharNames;
    bool mapUnknownCharNames;
    bool printCommands;
    bool overprintPreview;
    
    int    pdfVersion;
    int    pdfCompressLevel;
    int    pdfKeyBits;
    int    pdfPermission;
    double pdfPaperWidth;
	double pdfPaperHeight;
	double pdfHoff;
	double pdfVoff;
	bool   pdfEncrypt;
	double pdfMag;
	int    pdfPrecision;
	bool   pdfIgnoreColors;
	double pdfAnnotGrow;
	bool   pdfCIDFontFixedPitch;
	bool   pdfMapFirst;
	bool   pdfNoDestRemove;
	bool   pdfTPICTransparent;
    
    XWList *plugins;
    XWList *securityHandlers;
    
    double psRasterResolution;
    bool   psRasterMono;
    bool   antialiasPrinting;
    int screenType;
    int screenSize;
    int screenDotRadius;
    double screenGamma;
    double screenBlackThreshold;
    double screenWhiteThreshold;
    
private:
	void parseCommand(const QString & var, XWString **val);
	void parseDouble(const QString & var, double & v);
	void parseInt(const QString & var, int & v);
	void parsePDFPaperSize();
	void parsePSImageableArea();
	void parsePSLevel();
	void parsePSPaperSize();
	void parseScreen();
	void parseTextEOL();
	void parseYesNo(const QString & var, bool & v);
};


#endif // XWDOCSETTING_H

