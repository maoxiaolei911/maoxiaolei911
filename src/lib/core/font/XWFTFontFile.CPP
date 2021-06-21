/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "XWString.h"
#include "XWApplication.h"
#include "XWFTFontEngine.h"
#include "XWFTFont.h"
#include "XWFontFileID.h"
#include "XWFTFontFile.h"


XWFTFontFile::XWFTFontFile(XWFTFontEngine *engineA,
		                   XWFontFileID *idA,
		                   char *fileNameA, 
		                   bool deleteFileA,
		                   FT_Face faceA,
		                   int *codeToGIDA, 
		                   int codeToGIDLenA,
		                   bool trueTypeA)
{
	engine = engineA;
	id = idA;
    fileName = new XWString(fileNameA);
    deleteFile = deleteFileA;
    refCnt = 0;
    
    face = faceA;
    codeToGID = codeToGIDA;
    codeToGIDLen = codeToGIDLenA;
    trueType = trueTypeA;
}

XWFTFontFile::~XWFTFontFile()
{
	if (deleteFile)
	{
		QString fn = QFile::decodeName(fileName->getCString());
		QFile::remove(fn);
	}
	
    delete fileName;
    delete id;
    
    if (face) 
        FT_Done_Face(face);
    
    if (codeToGID) 
        free(codeToGID);
}

void XWFTFontFile::decRefCnt()
{
	if (!--refCnt) 
        delete this;
}

void XWFTFontFile::incRefCnt()
{
	++refCnt;
}

XWFTFontFile *XWFTFontFile::load(XWFTFontEngine *engineA,
	                             XWFontFileID *idA,
				                 char *fileNameA,
				                 int   index,
				                 char **encA)
{
	FT_Face faceA;

    if (FT_New_Face(engineA->lib, fileNameA, index, &faceA)) 
    {
    	QString msg = QString(tr("fail to load font file \"%1\".\n")).arg(fileNameA);
		xwApp->error(msg);
        return 0;
    }
    
    bool truetypeA = FT_IS_SFNT(faceA) && !(FT_IS_CID_KEYED(faceA));
    if (!encA)
    	return new XWFTFontFile(engineA, idA, fileNameA, false, faceA, 0, 0, truetypeA);
    	
    int * codeToGIDA = (int *)malloc(256 * sizeof(int));
    char *name = 0;
    for (int i = 0; i < 256; ++i) 
    {
        codeToGIDA[i] = 0;
        if ((name = encA[i])) 
            codeToGIDA[i] = (int)FT_Get_Name_Index(faceA, name);
    }

    return new XWFTFontFile(engineA, idA, fileNameA, false, faceA, codeToGIDA, 256, truetypeA);
}

XWFTFontFile *XWFTFontFile::loadCIDFont(XWFTFontEngine *engineA,
				                        XWFontFileID *idA, 
				                        char *fileNameA,
				                        bool deleteFileA,
				                        int *codeToGIDA, 
				                        int codeToGIDLenA)
{
    FT_Face faceA;

    if (FT_New_Face(engineA->lib, fileNameA, 0, &faceA)) 
    {
    	QString msg = QString(tr("fail to load CID font file \"%1\".\n")).arg(fileNameA);
		xwApp->error(msg);
        return 0;
    }

    return new XWFTFontFile(engineA, idA, fileNameA, deleteFileA,
			                faceA, codeToGIDA, codeToGIDLenA, false);
}

XWFTFontFile * XWFTFontFile::loadTrueTypeFont(XWFTFontEngine *engineA,
					                          XWFontFileID *idA,
					                          char *fileNameA,
					                          bool deleteFileA,
					                          int *codeToGIDA,
					                          int codeToGIDLenA)
{
    FT_Face faceA;

    if (FT_New_Face(engineA->lib, fileNameA, 0, &faceA)) 
    {
    	QString msg = QString(tr("fail to load truetype font file \"%1\".\n")).arg(fileNameA);
		xwApp->error(msg);
        return 0;
    }

    return new XWFTFontFile(engineA, idA, fileNameA, deleteFileA,
			                faceA, codeToGIDA, codeToGIDLenA, true);
}

XWFTFontFile * XWFTFontFile::loadType1Font(XWFTFontEngine *engineA,
				                           XWFontFileID *idA, 
				                           char *fileNameA,
				                           bool deleteFileA, 
				                           char **encA)
{
	FT_Face faceA;
	
    if (FT_New_Face(engineA->lib, fileNameA, 0, &faceA)) 
    {
    	QString msg = QString(tr("fail to load type1 font file \"%1\".\n")).arg(fileNameA);
		xwApp->error(msg);
        return 0;
    }
        
    int * codeToGIDA = (int *)malloc(256 * sizeof(int));
    char *name = 0;
    for (int i = 0; i < 256; ++i) 
    {
        codeToGIDA[i] = 0;
        if ((name = encA[i])) 
            codeToGIDA[i] = (int)FT_Get_Name_Index(faceA, name);
    }

    return new XWFTFontFile(engineA, idA, fileNameA, deleteFileA,
			                faceA, codeToGIDA, 256, false);
}

XWFTFont * XWFTFontFile::makeFont(int size,
	                              double efactorA,
	                              double slantA,
	                              double boldfA,
	                              bool boldA,
	                              bool rotateA)
{
	return new XWFTFont(this, size, efactorA, slantA, boldfA, boldA, rotateA);
}

XWFTFont * XWFTFontFile::makeFont(double *mat, double *textMat)
{
    return new XWFTFont(this, mat, textMat);
}
