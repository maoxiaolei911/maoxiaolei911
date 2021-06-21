/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <QtDebug>
#include "XWString.h"
#include "XWApplication.h"
#include "XWRasterFontEngine.h"
#include "XWRasterFont.h"
#include "XWFontFileID.h"
#include "XWRasterFontFile.h"


XWRasterFontFile::XWRasterFontFile(XWFontFileID *idA, 
                                   char *fileNameA,
		                           bool deleteFileA)
{
    id = idA;
    fileName = new XWString(fileNameA);
    deleteFile = deleteFileA;
    refCnt = 0;
}

XWRasterFontFile::~XWRasterFontFile()
{
	if (deleteFile)
	{
		QString fn = QFile::decodeName(fileName->getCString());
		QFile::remove(fn);
	}
	
    delete fileName;
    delete id;
}

void XWRasterFontFile::decRefCnt()
{
    if (!--refCnt) 
        delete this;
}

void XWRasterFontFile::incRefCnt()
{
    ++refCnt;
}

XWRasterFTFontFile::XWRasterFTFontFile(XWRasterFontEngine *engineA,
		                               XWFontFileID *idA,
		                               char *fileNameA, 
		                               bool deleteFileA,
		                               FT_Face faceA,
		                               int *codeToGIDA, 
		                               int codeToGIDLenA,
		                               bool trueTypeA,
		                               bool type1A)
    :XWRasterFontFile(idA, fileNameA, deleteFileA)
{
    engine = engineA;
    face = faceA;
    codeToGID = codeToGIDA;
    codeToGIDLen = codeToGIDLenA;
    trueType = trueTypeA;
    type1 = type1A;
}

XWRasterFTFontFile::~XWRasterFTFontFile()
{
    if (face) 
        FT_Done_Face(face);
    
    if (codeToGID) 
        free(codeToGID);
}

XWRasterFontFile *XWRasterFTFontFile::loadCIDFont(XWRasterFontEngine *engineA,
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

    return new XWRasterFTFontFile(engineA, idA, fileNameA, deleteFileA,
			                      faceA, codeToGIDA, codeToGIDLenA, false, false);
}

XWRasterFontFile * XWRasterFTFontFile::loadTrueTypeFont(XWRasterFontEngine *engineA,
					                                    XWFontFileID *idA,
					                                    char *fileNameA,
					                                    int fontNum,
					                                    bool deleteFileA,
					                                    int *codeToGIDA,
					                                    int codeToGIDLenA)
{
    FT_Face faceA;

    if (FT_New_Face(engineA->lib, fileNameA, fontNum, &faceA)) 
    {
    	QString msg = QString(tr("fail to load truetype font file \"%1\".\n")).arg(fileNameA);
		xwApp->error(msg);
        return 0;
    }

    return new XWRasterFTFontFile(engineA, idA, fileNameA, deleteFileA,
			                      faceA, codeToGIDA, codeToGIDLenA, true, false);
}

XWRasterFontFile * XWRasterFTFontFile::loadType1Font(XWRasterFontEngine *engineA,
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

    return new XWRasterFTFontFile(engineA, idA, fileNameA, deleteFileA,
			      faceA, codeToGIDA, 256, false, true);
}

XWRasterFont * XWRasterFTFontFile::makeFont(double *mat, double *textMat)
{
    XWRasterFont * font = new XWRasterFTFont(this, mat, textMat);
    font->initCache();
    return font;
}

