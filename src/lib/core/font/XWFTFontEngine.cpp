/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <QByteArray>
#include <QString>
#include <QIODevice>
#include <QFile>
#include "XWApplication.h"
#include "XWFontFileType1C.h"
#include "XWFontFileTrueType.h"
#include "XWFontFileID.h"
#include "XWFTFont.h"
#include "XWFTFontFile.h"
#include "XWFTFontEngine.h"


static void fileWrite(void *stream, char *data, int len) 
{
	QIODevice * fp = (QIODevice*)stream;
    fp->write(data, len);
}

XWFTFontEngine::XWFTFontEngine(bool aaA)
{
	FT_Init_FreeType(&lib);
	aa = aaA;
	FT_Int major, minor, patch;
    FT_Library_Version(lib, &major, &minor, &patch);
    useCIDs = major > 2 || (major == 2 && (minor > 1 || (minor == 1 && patch > 7)));
    
    for (int i = 0; i < FTFontCacheSize; ++i) 
        fontCache[i] = 0;
}

XWFTFontEngine::~XWFTFontEngine()
{
    for (int i = 0; i < FTFontCacheSize; ++i) 
    {
        if (fontCache[i]) 
            delete fontCache[i];
    }
    
    FT_Done_FreeType(lib);
}

XWFTFont *XWFTFontEngine::getFont(XWFTFontFile *fontFile, 
                                  int size,
	                              double efactorA,
	                              double slantA,
	                              double boldfA,
	                              bool boldA,
	                              bool rotateA)
{
	double mat[4];
	mat[0] = 1;
    mat[1] = 0;
    mat[2] = 0;
    mat[3] = size;
    
    double textMat[4];
    
    textMat[0] = 1;
    textMat[1] = 0;
    textMat[2] = 0;
    textMat[3] = 1;
    
    XWFTFont *font = fontCache[0];
    if (font && font->matches(fontFile, mat, textMat)) 
    {
    	font->setMatrix(efactorA, slantA, boldfA, boldA, rotateA);
        return font;
    }
        
    for (int i = 1; i < FTFontCacheSize; ++i) 
    {
        font = fontCache[i];
        if (font && font->matches(fontFile, mat, textMat)) 
        {
            for (int j = i; j > 0; --j) 
	            fontCache[j] = fontCache[j-1];
            fontCache[0] = font;
            font->setMatrix(efactorA, slantA, boldfA, boldA, rotateA);
            return font;
        }
    }
    
    font = fontFile->makeFont(size, efactorA, slantA, boldfA, boldA, rotateA);
    if (fontCache[FTFontCacheSize - 1]) 
        delete fontCache[FTFontCacheSize - 1];
    
    for (int j = FTFontCacheSize - 1; j > 0; --j) 
        fontCache[j] = fontCache[j-1];
        
    fontCache[0] = font;
    return font;
}

XWFTFont *XWFTFontEngine::getFont(XWFTFontFile *fontFile, double *textMat, double *ctm)
{
    double mat[4];
    mat[0] = textMat[0] * ctm[0] + textMat[1] * ctm[2];
    mat[1] = -(textMat[0] * ctm[1] + textMat[1] * ctm[3]);
    mat[2] = textMat[2] * ctm[0] + textMat[3] * ctm[2];
    mat[3] = -(textMat[2] * ctm[1] + textMat[3] * ctm[3]);
    if (fabs(mat[0] * mat[3] - mat[1] * mat[2]) < 0.01)
    {
        mat[0] = 0.01;  
        mat[1] = 0;
        mat[2] = 0;     
        mat[3] = 0.01;
    }
    
    XWFTFont *font = fontCache[0];
    if (font && font->matches(fontFile, mat, textMat)) 
        return font;
        
    for (int i = 1; i < FTFontCacheSize; ++i) 
    {
        font = fontCache[i];
        if (font && font->matches(fontFile, mat, textMat)) 
        {
            for (int j = i; j > 0; --j) 
	            fontCache[j] = fontCache[j-1];
            fontCache[0] = font;
            return font;
        }
    }
    
    font = fontFile->makeFont(mat, textMat);
    if (fontCache[FTFontCacheSize - 1]) 
        delete fontCache[FTFontCacheSize - 1];
    
    for (int j = FTFontCacheSize - 1; j > 0; --j) 
        fontCache[j] = fontCache[j-1];
        
    fontCache[0] = font;
    return font;
}

XWFTFontFile * XWFTFontEngine::getFontFile(XWFontFileID *id)
{
    for (int i = 0; i < FTFontCacheSize; ++i) 
    {
        if (fontCache[i]) 
        {
            XWFTFontFile * fontFile = fontCache[i]->getFontFile();
            if (fontFile && fontFile->getID()->matches(id)) 
	            return fontFile;
        }
    }
    return 0;
}

XWFTFontFile *XWFTFontEngine::load(XWFontFileID *idA, 
                                   char *fileName,
                                   int   index,
				                   char **enc)
{
	return XWFTFontFile::load(this, idA, fileName, index, enc);
}

XWFTFontFile *XWFTFontEngine::loadCIDFont(XWFontFileID *idA, 
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
    else 
    {
        cidToGIDMap = 0;
        nCIDs = 0;
    }
    
    XWFTFontFile * ret = XWFTFontFile::loadCIDFont(this, idA, fileName, deleteFile, cidToGIDMap, nCIDs);
    if (!ret) 
    {
    	if (deleteFile)
    		removeFile(fileName);
        free(cidToGIDMap);
    }
        
    return ret;
}

XWFTFontFile *XWFTFontEngine::loadOpenTypeCFFFont(XWFontFileID *idA, 
	                                              char *fileName,
				                                  bool deleteFile)
{
    int *cidToGIDMap = NULL;
    int nCIDs = 0;
    XWFontFileTrueType * ff= 0;
    if (!useCIDs) 
    {
        if ((ff = XWFontFileTrueType::load(fileName))) 
        {
            if (ff->isOpenTypeCFF()) 
	            cidToGIDMap = ff->getCIDToGIDMap(&nCIDs);
            delete ff;
        }
    }
    
    XWFTFontFile * ret = XWFTFontFile::loadCIDFont(this, idA, fileName, deleteFile, cidToGIDMap, nCIDs);
    if (!ret) 
    {
    	if (deleteFile)
    		removeFile(fileName);
        free(cidToGIDMap);
    }
        
    return ret;
}

XWFTFontFile *XWFTFontEngine::loadOpenTypeT1CFont(XWFontFileID *idA, 
                                                  char *fileName,
				                                  bool deleteFile, 
				                                  char **enc)
{
    return XWFTFontFile::loadType1Font(this, idA, fileName, deleteFile, enc);
}

XWFTFontFile *XWFTFontEngine::loadTrueTypeFont(XWFontFileID *idA, 
                                               char *fileName,
				                               bool deleteFile,
				                               int *codeToGID, 
				                               int codeToGIDLen)
{
    XWFontFileTrueType * ff = 0;
    if (!(ff = XWFontFileTrueType::load(fileName))) 
    {
    	if (deleteFile)
    		removeFile(fileName);
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
    XWFTFontFile * ret = XWFTFontFile::loadTrueTypeFont(this, idA,
					                                    ba.data(),
					                                    true, codeToGID, codeToGIDLen);
    
    return ret;
}

XWFTFontFile *XWFTFontEngine::loadType1CFont(XWFontFileID *idA, 
                                             char *fileName,
				                             bool deleteFile, 
				                             char **enc)
{
    return XWFTFontFile::loadType1Font(this, idA, fileName, deleteFile, enc);
}

XWFTFontFile * XWFTFontEngine::loadType1Font(XWFontFileID *idA, 
                                             char *fileName,
				                             bool deleteFile, char **enc)
{
    return XWFTFontFile::loadType1Font(this, idA, fileName, deleteFile, enc);
}

void XWFTFontEngine::removeFile(char *fileName)
{
	if (!fileName)
		return ;
		
	QString fn = QFile::decodeName(fileName);
	QFile::remove(fn);
}
