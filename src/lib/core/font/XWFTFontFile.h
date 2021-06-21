/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWFTFONTFILE_H
#define XWFTFONTFILE_H

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
class XWFTFontEngine;
class XWFontFileID;
class XWFTFont;

class XW_FONT_EXPORT XWFTFontFile: public QObject 
{
	Q_OBJECT
	
public:
    virtual ~XWFTFontFile();
    
    void decRefCnt();
    
    XWFontFileID *getID() { return id; }
    
    void incRefCnt();
    
    static XWFTFontFile *load(XWFTFontEngine *engineA,
    						  XWFontFileID *idA,
				              char *fileNameA,
				              int   index,
				              char **encA);
    static XWFTFontFile *loadCIDFont(XWFTFontEngine *engineA,
				                     XWFontFileID *idA, 
				                     char *fileNameA,
				                     bool deleteFileA,
				                     int *codeToCIDA, 
				                     int codeToGIDLenA);
    static XWFTFontFile *loadTrueTypeFont(XWFTFontEngine *engineA,
					                      XWFontFileID *idA,
					                      char *fileNameA,
					                      bool deleteFileA,
					                      int *codeToGIDA,
					                      int codeToGIDLenA);
	static XWFTFontFile *loadType1Font(XWFTFontEngine *engineA,
				                       XWFontFileID *idA, 
				                       char *fileNameA,
				                       bool deleteFileA, 
				                       char **encA);
	XWFTFont * makeFont(int size,
	                    double efactorA,
	                    double slantA,
	                    double boldfA,
	                    bool boldA,
	                    bool rotateA);
	XWFTFont * makeFont(double *mat, double *textMat);
	
private:
    XWFTFontFile(XWFTFontEngine *engineA,
		         XWFontFileID *idA,
		         char *fileNameA, 
		         bool deleteFileA,
		         FT_Face faceA,
		         int *codeToGIDA, 
		         int codeToGIDLenA,
		         bool trueTypeA);

private:
	XWFTFontEngine *engine;
    XWFontFileID *id;
    XWString *fileName;
    bool deleteFile;
    int refCnt;
    
    FT_Face face;
    int *codeToGID;
    int codeToGIDLen;
    bool trueType;

    friend class XWFTFont;
};

#endif // XWFTFONTFILE_H

