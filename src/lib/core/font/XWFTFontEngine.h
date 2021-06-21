/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWFTFONTENGINE_H
#define XWFTFONTENGINE_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include <QObject>
#include "XWGlobal.h"

#define FTFontCacheSize 16


class XWFTFontFile;
class XWFontFileID;
class XWFTFont;

class XW_FONT_EXPORT XWFTFontEngine : public QObject
{
	Q_OBJECT
	
public:    
	XWFTFontEngine(bool aaA = true);
    ~XWFTFontEngine();
    
    XWFTFont     *getFont(XWFTFontFile *fontFile, 
                          int size,
	                      double efactorA = 1.0,
	                      double slantA = 0.0,
	                      double boldfA = 0.0,
	                      bool boldA = false,
	                      bool rotateA = false);
    XWFTFont     *getFont(XWFTFontFile *fontFile, double *textMat, double *ctm);
    XWFTFontFile *getFontFile(XWFontFileID *id);
    
    // Load fonts.
    XWFTFontFile *load(XWFontFileID *idA, 
                       char *fileName,
                       int   index,
				       char **enc);
    XWFTFontFile *loadCIDFont(XWFontFileID *idA, 
                              char *fileName,
			                  bool deleteFile);
    XWFTFontFile *loadOpenTypeCFFFont(XWFontFileID *idA, 
                                      char *fileName,
				                      bool deleteFile);
    XWFTFontFile *loadOpenTypeT1CFont(XWFontFileID *idA, 
                                      char *fileName,
				                      bool deleteFile, 
				                      char **enc);
    XWFTFontFile *loadTrueTypeFont(XWFontFileID *idA, 
                                   char *fileName,
				                   bool deleteFile,
				                   int *codeToGID, 
				                   int codeToGIDLen);
    XWFTFontFile *loadType1CFont(XWFontFileID *idA, 
                                 char *fileName,
				                 bool deleteFile, 
				                 char **enc);
    XWFTFontFile *loadType1Font(XWFontFileID *idA, 
                                char *fileName,
				                bool deleteFile, 
				                char **enc);
        
private:
	void removeFile(char *fileName);
	
private:
	friend class XWFTFontFile;
    friend class XWFTFont;
    
    FT_Library lib;
	bool aa;
    bool useCIDs;
    XWFTFont *fontCache[FTFontCacheSize];
};

#endif // XWFTFONTENGINE_H
