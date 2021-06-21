/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <QtDebug>
#include <QByteArray>
#include <QString>
#include <QIODevice>
#include <QFile>
#include "XWApplication.h"
#include "XWFontFileType1C.h"
#include "XWFontFileTrueType.h"
#include "XWRasterType.h"
#include "XWRasterFontFile.h"
#include "XWFontFileID.h"
#include "XWRasterFont.h"
#include "XWRasterFontEngine.h"


static void fileWrite(void *stream, char *data, int len) 
{
	QIODevice * fp = (QIODevice*)stream;
    fp->write(data, len);
}

XWRasterFontEngine::XWRasterFontEngine(bool aaA)
{
	FT_Init_FreeType(&lib);
	flags = 0;
	aa = aaA;
	FT_Int major, minor, patch;
    FT_Library_Version(lib, &major, &minor, &patch);
    useCIDs = major > 2 || (major == 2 && (minor > 1 || (minor == 1 && patch > 7)));
    
    for (int i = 0; i < EngineFontCacheSize; ++i) 
        fontCache[i] = 0;
}

XWRasterFontEngine::~XWRasterFontEngine()
{
    for (int i = 0; i < EngineFontCacheSize; ++i) 
    {
        if (fontCache[i]) 
            delete fontCache[i];
    }
    
    FT_Done_FreeType(lib);
}

XWRasterFont *XWRasterFontEngine::getFont(XWRasterFontFile *fontFile, double *textMat, double *ctm)
{
  double mat[4];
  XWRasterFont *font;
  int i, j;

  mat[0] = textMat[0] * ctm[0] + textMat[1] * ctm[2];
  mat[1] = -(textMat[0] * ctm[1] + textMat[1] * ctm[3]);
  mat[2] = textMat[2] * ctm[0] + textMat[3] * ctm[2];
  mat[3] = -(textMat[2] * ctm[1] + textMat[3] * ctm[3]);
  if (!rasterCheckDet(mat[0], mat[1], mat[2], mat[3], 0.01)) {
    // avoid a singular (or close-to-singular) matrix
    mat[0] = 0.01;  mat[1] = 0;
    mat[2] = 0;     mat[3] = 0.01;
  }

  font = fontCache[0];
  if (font && font->matches(fontFile, mat, textMat)) {
    return font;
  }
  for (i = 1; i < EngineFontCacheSize; ++i) {
    font = fontCache[i];
    if (font && font->matches(fontFile, mat, textMat)) {
      for (j = i; j > 0; --j) {
	fontCache[j] = fontCache[j-1];
      }
      fontCache[0] = font;
      return font;
    }
  }
  font = fontFile->makeFont(mat, textMat);
  if (fontCache[EngineFontCacheSize - 1]) {
    delete fontCache[EngineFontCacheSize - 1];
  }
  for (j = EngineFontCacheSize - 1; j > 0; --j) {
    fontCache[j] = fontCache[j-1];
  }
  fontCache[0] = font;
  return font;
}

XWRasterFontFile * XWRasterFontEngine::getFontFile(XWFontFileID *id)
{
    for (int i = 0; i < EngineFontCacheSize; ++i) 
    {
        if (fontCache[i]) 
        {
            XWRasterFontFile * fontFile = fontCache[i]->getFontFile();
            if (fontFile && fontFile->getID()->matches(id)) 
	            return fontFile;
        }
    }
    return 0;
}

XWRasterFontFile *XWRasterFontEngine::loadCIDFont(XWFontFileID *idA, 
                                                  char *fileName,
			                                      bool deleteFile)
{
    int *cidToGIDMap = 0;
    int nCIDs = 0;
    XWFontFileType1C *ff = 0;
    if (useCIDs) 
    {
        cidToGIDMap = 0;
        nCIDs = 0;
    } 
    else if ((ff = XWFontFileType1C::load(fileName))) 
    {
        cidToGIDMap = ff->getCIDToGIDMap(&nCIDs);
        delete ff;
    }
    
    XWRasterFontFile * ret = XWRasterFTFontFile::loadCIDFont(this, idA, fileName, deleteFile, cidToGIDMap, nCIDs);
    if (!ret) 
    {
    	if (cidToGIDMap)
        free(cidToGIDMap);
    }
        
    return ret;
}

XWRasterFontFile *XWRasterFontEngine::loadOpenTypeCFFFont(XWFontFileID *idA, 
                                                          char *fileName,
				                                                  bool deleteFile,
				                                                  int *codeToGID,
						      								                        int codeToGIDLen)
{
  XWFontFileTrueType *ff;
  bool isCID;
  int *cidToGIDMap;
  int nCIDs;
  XWRasterFontFile *ret;

  cidToGIDMap = NULL;
  nCIDs = 0;
  isCID = false;
  if (!codeToGID) {
    if (!useCIDs) {
      if ((ff = XWFontFileTrueType::load(fileName))) {
	if (ff->isOpenTypeCFF()) {
	  cidToGIDMap = ff->getCIDToGIDMap(&nCIDs);
	}
	delete ff;
      }
    }
  }
  ret = XWRasterFTFontFile::loadCIDFont(this, idA, fileName, deleteFile,
				      codeToGID ? codeToGID : cidToGIDMap,
				      codeToGID ? codeToGIDLen : nCIDs);
  if (!ret) {
    free(cidToGIDMap);
  }
  return ret;
}

XWRasterFontFile *XWRasterFontEngine::loadOpenTypeT1CFont(XWFontFileID *idA, 
                                                          char *fileName,
				                                          bool deleteFile, 
				                                          char **enc)
{
    return XWRasterFTFontFile::loadType1Font(this, idA, fileName, deleteFile, enc);
}

XWRasterFontFile *XWRasterFontEngine::loadTrueTypeFont(XWFontFileID *idA, 
                                                        char *fileName,
                                                        int fontNum,
				                                        bool deleteFile,
				                                        int *codeToGID, 
				                                        int codeToGIDLen,
				                                        char *)
{
    XWFontFileTrueType * ff = 0;
    if (!(ff = XWFontFileTrueType::load(fileName))) 
    {
        return 0;
    }
    
    QString tmpfilename = xwApp->getTmpFile();
    QFile fp(tmpfilename);
    if (!fp.open(QIODevice::WriteOnly))
    {
    	delete ff;
    	return 0;
    }
    
    ff->writeTTF(&fileWrite, &fp);
    delete ff;
    fp.close();
    QByteArray ba = QFile::encodeName(tmpfilename);    
    XWRasterFontFile * ret = XWRasterFTFontFile::loadTrueTypeFont(this, idA,
					                                              ba.data(),
					                                              fontNum,
					                                              true, codeToGID, codeToGIDLen);
    
    return ret;
}

XWRasterFontFile *XWRasterFontEngine::loadType1CFont(XWFontFileID *idA, 
                                                     char *fileName,
				                                     bool deleteFile, 
				                                     char **enc)
{
    return XWRasterFTFontFile::loadType1Font(this, idA, fileName, deleteFile, enc);
}

XWRasterFontFile * XWRasterFontEngine::loadType1Font(XWFontFileID *idA, 
                                                     char *fileName,
				                                     bool deleteFile, char **enc)
{
    return XWRasterFTFontFile::loadType1Font(this, idA, fileName, deleteFile, enc);
}

void XWRasterFontEngine::removeFile(char *fileName)
{
	if (!fileName)
		return ;
		
	QString fn = QFile::decodeName(fileName);
	QFile::remove(fn);
}

