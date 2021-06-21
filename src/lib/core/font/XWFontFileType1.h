/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWFONTFILETYPE1_H
#define XWFONTFILETYPE1_H

#include "XWFontFile.h"


#define PFB_SEG_TYPE_ASCII  1
#define PFB_SEG_TYPE_BINARY 2

#define T1_EEKEY   55665u
#define T1_CHARKEY 4330u

class XW_FONT_EXPORT XWFontFileType1 : public XWFontFile
{
	Q_OBJECT
	
public:   
	virtual ~XWFontFileType1();
	
	void decrypt(ushort key, 
	             uchar *dst, 
	             const uchar *src,
	             long skip, 
	             long lenA);
	                    
    char **getEncoding();
    void   getFontMatrix(double *mat);
    int    getFontName(char *fontnameA);
    char * getName();
    
    uchar * getPFBSegment(int expected_type, 
                          long *length);
    
    static bool isPFB(QIODevice * fileA);
    
    static XWFontFileType1 *load(char *fileName);
    static XWFontFileType1 *loadPFB(char *fileName, bool deleteFileA);
    
    static XWFontFileType1 *make(QIODevice * fileA, ulong offsetA, ulong lenA);
    static XWFontFileType1 *make(char *buf, int lenA);
    
    void writeEncoded(char **newEncoding,
		              FontFileOutputFunc outputFunc, 
		              void *outputStream);
    
private:
    XWFontFileType1(QIODevice * fileA,
		 	        int    fileFinalA,
		 	        ulong offsetA,
		 	        ulong lenA);
		XWFontFileType1(uchar *filedataA, int lenA, bool freeFileDataA);
    
    char *getNextLine(char *line);
    
    void parse();
    
    void undoPFB();
    
private:
	  bool undo;
    char *name;
    char **encoding;
    bool parsed;    
    double fontMatrix[6];
};

#endif // XWFONTFILETYPE1_H

