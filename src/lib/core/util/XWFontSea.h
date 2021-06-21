/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWFONTSEA_H
#define XWFONTSEA_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QFile>

#include "XWGlobal.h"
#include "XWPathSea.h"

enum GlyphSourceType
{
    Normal = 0,
    Alias,
    MakeTex,
    FallBack,
};

struct XW_UTIL_EXPORT GlyphFileType
{
    QString name;
    int dpi;
    int format;
    GlyphSourceType source;
};

class XW_UTIL_EXPORT XWFontSea : public QObject
{
    Q_OBJECT

public:
    enum FileFormat
    {
        Any = 0,
        AFM,
        BDF,
        CFF,
        CMAP,
        ENC,
        GF,
        GlyphList,
        LIG,
        Map,
        MF,
        MiscFonts,        
        OFM,
        OpenType,
        OPL,
        OTL,        
        OVF,
        OVP,
        PCF,
        PFM,
        PFR,
        PK,
        PL,
        RPL,
        SFD,
        TFM,
        TroffFont,
        TrueType,
        Type0,
        Type1,
        Type2,
        Type3,
        Type42,
        VF,
        VPL,
        WinFont,
        Last
    };
    
    XWFontSea(QObject * parent = 0);
    XWFontSea(const QString & prefix,
              int dpi,
              const QString & mode,
              const QString & fallback,
              QObject * parent = 0);

	void addConfig(const QString & key, 
                   const QString & value, 
                   const QString & prog = QString());
	void addFile(const QString & path);
	QStringList allAfm();
	QStringList allBdf();
	QStringList allCff();
	QStringList allCMap();
	QStringList allEnc();
	QStringList allFileNames(FileFormat format);
	QStringList allFreeType();
	QStringList allGf();
	QStringList allGlyphList();
	QStringList allLig();
	QStringList allMap();
	QStringList allMf();
	QStringList allOfm();
	QStringList allOpl();
	QStringList allOvf();
	QStringList allOvp();
	QStringList allPcf();
	QStringList allPfm();
	QStringList allPfr();
	QStringList allPk();
	QStringList allPl();
	QStringList allSfd();
	QStringList allTfm();
	QStringList allTrueType();
	QStringList allType1();
	QStringList allVf();
	QStringList allVpl();
	QStringList allWin();
	
    static bool bitmapTolerance(double dpi1, double dpi2);
    
    void createAllDefaultDir(const QString & topdir = QString());
	void createDefaultDir(FileFormat format,
	                      const QString & topdir = QString());

	QString findAfm(const QString & name);
	QStringList findAll(const QString & name,
	                    FileFormat format);
	QStringList findAll(const QStringList & names,
	                    FileFormat format);
	QString findBdf(const QString & name);
	QString findCff(const QString & name);
    QString findCMap(const QString & name);
    QString findEnc(const QString & name);
    QString findFile(const QString & name);
    QString findFile(const QString & name,
                     FileFormat format,
                     bool must_exist);
    FileFormat findFormat(const QString & name);
    QString findGf(const QString & fontname,
                   int dpi,
                   GlyphFileType * glyph_file);
    QString findGlyph(const QString & fontname,
                      int dpi,
                      FileFormat format,
                      GlyphFileType * glyph_file);
    QString findGlyphList(const QString & name = QString());
    QString findLig(const QString & name);
    QString findMap(const QString & name);
    QString findMf(const QString & name);
    QString findMisc(const QString & name);
    QString findOfm(const QString & name);
    QString findOpenType(const QString & name);
    QString findOpl(const QString & name);
    QString findOtl(const QString & name);
    QString findOvf(const QString & name);
    QString findOvp(const QString & name);
    QString findPcf(const QString & name);
    QString findPfm(const QString & name);
    QString findPfr(const QString & name);
    QString findPk(const QString & fontname,
                   int dpi,
                   GlyphFileType * glyph_file);
    QString findPl(const QString & name);
    QString findRPL(const QString & name);
    QString findSfd(const QString & name);
    QString findTfm(const QString & name);
    QString findTroff(const QString & name);
    QString findTrueType(const QString & name);
    QString findType0(const QString & name);
    QString findType1(const QString & name);
    QString findType2(const QString & name);
    QString findType3(const QString & name);
    QString findType42(const QString & name);
    QString findVf(const QString & name);
    QString findVpl(const QString & name);
    QString findWin(const QString & name);
    
    QStringList getConfigDir(FileFormat format);
    QStringList getSuffixes(FileFormat format);
    QStringList getSystemDir();
    QStringList getSystemFiles();
    QStringList getTopDirs();
    
    static void init();
    
    void initProg(const QString & prefix,
                  int dpi,
                  const QString & mode,
                  const QString & fallback);
    void insertFile(const QString & passedfname);

    void makeTexOption(const QString & fmtname,  bool value);

	QFile * openAfm(const QString & filename);
	QFile * openBdf(const QString & filename);
	QFile * openCff(const QString & filename);
    QFile * openCMap(const QString & filename);
    QFile * openEnc(const QString & filename);
    QFile * openFile(const QString & filename);
    QFile * openFile(const QString & filename, FileFormat format);
    QFile * openGf(const QString & filename);
    QFile * openGlyphList(const QString & name = QString());
    QFile * openLig(const QString & filename);
    QFile * openMap(const QString & filename);
    QFile * openMf(const QString & filename);
    QFile * openMisc(const QString & filename);
    QFile * openOfm(const QString & filename);
    QFile * openOpenType(const QString & filename);
    QFile * openOpl(const QString & filename);
    QFile * openOtl(const QString & filename);
    QFile * openOvf(const QString & filename);
    QFile * openOvp(const QString & filename);
    QFile * openPcf(const QString & filename);
    QFile * openPfm(const QString & filename);
    QFile * openPfr(const QString & filename);
    QFile * openPk(const QString & filename);
    QFile * openPl(const QString & filename);
    QFile * openRPL(const QString & filename);
    QFile * openSfd(const QString & filename);
    QFile * openTFM(const QString & filename);
    QFile * openTroff(const QString & filename);
    QFile * openTrueType(const QString & filename);
    QFile * openType0(const QString & filename);
    QFile * openType1(const QString & filename);
    QFile * openType2(const QString & filename);
    QFile * openType3(const QString & filename);
    QFile * openType42(const QString & filename);
    QFile * openVf(const QString & filename);
    QFile * openVpl(const QString & filename);
    QFile * openWin(const QString & filename);
    
    void removeConfig(const QString & key);
    
    static void quit();
    
    void setDirs(FileFormat format, 
                 const QString & dirs = QString());
    void setGlyphList(const QString & s);

    QString varExpand(const QString & s);
    QString varValue(const QString & s);
    
public slots:
	void save();
	void updateDBs();
    
signals:
	void directoryChangrd(const QString & path);
};

class XWFontSeaPrivate : public XWPathSea
{
    Q_OBJECT

public:
    XWFontSeaPrivate(QObject * parent = 0);
    ~XWFontSeaPrivate();
	
    QString findFile(const QString & name,
                     int format,
                     bool must_exist);
    QString findGlyph(const QString & passed_fontname,
                      int dpi,
                      XWFontSea::FileFormat format,
                      GlyphFileType * glyph_file);

    QStringList fontMapLookup(const QString & key);
	
    QString initFormat(int format);
    void initProg(const QString & prefix,
                  int dpi,
                  const QString & mode,
                  const QString & fallback);

    QString makeTex(int format,
                    const QString & base);
    void makeTexOption(const QString & fmtname,  bool value);

    QString tryFallbackResolutions(const QString & fontname,
                                   int dpi,
                                   XWFontSea::FileFormat format,
                                   GlyphFileType * glyph_file);
    QString tryFontMap(QString & fontname,
                       int dpi,
                       XWFontSea::FileFormat format,
                       GlyphFileType * glyph_file);
    QString tryFormat(XWFontSea::FileFormat format);
    QString tryResolution(const QString & fontname,
                          int dpi,
                          XWFontSea::FileFormat format,
                          GlyphFileType * glyph_file);
    QString trySize(const QString & fontname,
                    int dpi,
                    XWFontSea::FileFormat format,
                    GlyphFileType * glyph_file);

public:
    int *   resolutions;
    int mapPos;
    bool makeTexDiscardErrors;
    QString font;
    QString resolutionsString;

    QString mapPath;
    QMultiHash<QString, QString> map;


private:
    void initFallbackResolutions(const QString & envvar);

    void parseMapFile(const QString & map_filename);

    void readAllMaps();

    void setMakeTexMag();

    QString token(const QChar * str);
};

#endif //XWFONTSEA_H

