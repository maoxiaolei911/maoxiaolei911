/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWRASTERFONTENGINE_H
#define XWRASTERFONTENGINE_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include <QObject>
#include "XWGlobal.h"

#define EngineFontCacheSize 16

#define RasterFTNoHinting (1 << 0)

class XWRasterFontFile;
class XWFontFileID;
class XWRasterFont;

class XW_RASTER_EXPORT XWRasterFontEngine : public QObject
{
	Q_OBJECT
	
public:    
	XWRasterFontEngine(bool aaA = true);
    ~XWRasterFontEngine();
    
    XWRasterFont     *getFont(XWRasterFontFile *fontFile, double *textMat, double *ctm);
    XWRasterFontFile *getFontFile(XWFontFileID *id);
    
    // Load fonts.
    XWRasterFontFile *loadCIDFont(XWFontFileID *idA, 
                                  char *fileName,
			                      bool deleteFile);
    XWRasterFontFile *loadOpenTypeCFFFont(XWFontFileID *idA, 
                                          char *fileName,
				                                  bool deleteFile,
				                                  int *codeToGID,
						      								        int codeToGIDLen);
    XWRasterFontFile *loadOpenTypeT1CFont(XWFontFileID *idA, 
                                          char *fileName,
				                          bool deleteFile, 
				                          char **enc);
    XWRasterFontFile *loadTrueTypeFont(XWFontFileID *idA, 
                                       char *fileName,
                                       int fontNum,
				                       bool deleteFile,
				                       int *codeToGID, 
				                       int codeToGIDLen,
				                       char *fontName);
    XWRasterFontFile *loadType1CFont(XWFontFileID *idA, 
                                     char *fileName,
				                     bool deleteFile, 
				                     char **enc);
    XWRasterFontFile *loadType1Font(XWFontFileID *idA, 
                                    char *fileName,
				                    bool deleteFile, 
				                    char **enc);
        
private:
	void removeFile(char *fileName);
	
private:
	friend class XWRasterFTFontFile;
    friend class XWRasterFTFont;
    
    FT_Library lib;
    uint flags;
	bool aa;
    bool useCIDs;
    XWRasterFont *fontCache[EngineFontCacheSize];
};

#endif // XWRASTERFONTENGINE_H

