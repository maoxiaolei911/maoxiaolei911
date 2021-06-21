/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWRASTERFONTFILE_H
#define XWRASTERFONTFILE_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include <QObject>
#include <QString>
#include <QFile>
#include <QBuffer>
#include <QByteArray>
#include <QHash>
#include "XWGlobal.h"

class XWString;
class XWRasterFontEngine;
class XWRasterFont;
class XWFontFileID;

class XW_RASTER_EXPORT XWRasterFontFile : public QObject
{
	Q_OBJECT
	
public:    
    virtual ~XWRasterFontFile();
    
    void decRefCnt();
    
    XWFontFileID *getID() { return id; }
    
    void incRefCnt();
    
    virtual XWRasterFont *makeFont(double *mat, double *textMat) = 0;
    
protected:
    XWRasterFontFile(XWFontFileID *idA, 
                     char *fileNameA,
		             bool deleteFileA);
    
protected:
    XWFontFileID *id;
    XWString *fileName;
    bool deleteFile;
    int refCnt;

    friend class XWRasterFontEngine;
};

class XWRasterFontEngine;


class XW_RASTER_EXPORT XWRasterFTFontFile: public XWRasterFontFile 
{
	Q_OBJECT
	
public:
    virtual ~XWRasterFTFontFile();
    
    
    static XWRasterFontFile *loadCIDFont(XWRasterFontEngine *engineA,
				                         XWFontFileID *idA, 
				                         char *fileNameA,
				                         bool deleteFileA,
				                         int *codeToCIDA, 
				                         int codeToGIDLenA);
    static XWRasterFontFile *loadTrueTypeFont(XWRasterFontEngine *engineA,
					                          XWFontFileID *idA,
					                          char *fileNameA,
					                          int fontNum,
					                          bool deleteFileA,
					                          int *codeToGIDA,
					                          int codeToGIDLenA);
	static XWRasterFontFile *loadType1Font(XWRasterFontEngine *engineA,
				                           XWFontFileID *idA, 
				                           char *fileNameA,
				                           bool deleteFileA, 
				                           char **encA);
				                           
    virtual XWRasterFont *makeFont(double *mat, double *textMat);

private:
    XWRasterFTFontFile(XWRasterFontEngine *engineA,
		               XWFontFileID *idA,
		               char *fileNameA, 
		               bool deleteFileA,
		               FT_Face faceA,
		               int *codeToGIDA, 
		               int codeToGIDLenA,
		               bool trueTypeA,
		               bool type1A);

    XWRasterFontEngine *engine;
    FT_Face face;
    int *codeToGID;
    int codeToGIDLen;
    bool trueType;
    bool type1;

    friend class XWRasterFTFont;
};

#endif // XWRASTERFONTFILE_H

