/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWFONTFILETRUETYPE_H
#define XWFONTFILETRUETYPE_H

#include "XWFontFile.h"

class XWString;
class XWHash;

struct TrueTypeTable 
{
    uint tag;
    uint checksum;
    int  offset;
    int  origOffset;
    int  len;
};

struct TrueTypeCmap 
{
    int platform;
    int encoding;
    int offset;
    int len;
    int fmt;
};

struct TrueTypeLoca 
{
    int idx;
    int origOffset;
    int newOffset;
    int len;
};


class XW_FONT_EXPORT XWFontFileTrueType : public XWFontFile
{
public:   
	virtual ~XWFontFileTrueType();
	
    void convertToCIDType0(char *psName, 
                           int *cidMap, 
                           int nCIDs,
				                   FontFileOutputFunc outputFunc,
				                   void *outputStream);
    void convertToCIDType2(char *psName, 
                           int *cidMap, 
                           int nCIDs,
			               bool needVerticalMetrics,
			               FontFileOutputFunc outputFunc, 
			               void *outputStream);
    void convertToType0(char *psName, 
                        int *cidMap, 
                        int nCIDs,
		                    bool needVerticalMetrics,
		                    FontFileOutputFunc outputFunc, 
		                    void *outputStream);
    void convertToType0(char *psName, 
                        int *cidMap, 
                        int nCIDs,
				                FontFileOutputFunc outputFunc,
				                void *outputStream);
    void convertToType1(char *psName, 
                        char **newEncoding, 
                        bool ascii,
		                FontFileOutputFunc outputFunc, 
		                void *outputStream);
    void convertToType42(char *psName, 
                         char **encoding,
		                 int *codeToGID,
		                 FontFileOutputFunc outputFunc, 
		                 void *outputStream);
		       
    int findCmap(int platform, int encoding);
    
    bool    getCFFBlock(char **start, int *length);
    int    *getCIDToGIDMap(int *nCIDs);
    int     getCmapEncoding(int i);
    int     getCmapPlatform(int i);
    int     getEmbeddingRights();
    void    getFontMatrix(double *mat);
    int     getNumCmaps();
    
    bool isOpenTypeCFF() { return openTypeCFF; }
    
    static XWFontFileTrueType *load(char *fileName);
    
    static XWFontFileTrueType *make(char *buf, int lenA);
    ushort mapCodeToGID(int i, int c);
    int    mapNameToGID(char *name);
    
    void writeTTF(FontFileOutputFunc outputFunc, 
                  void *outputStream,
		          char *name = 0, 
		          ushort *codeToGID = 0);
    
protected:
	XWFontFileTrueType(QIODevice * fileA,
		 	           int    fileFinalA,
		 	           ulong offsetA,
		 	           ulong lenA);
	XWFontFileTrueType(uchar *filedataA, int lenA, bool freeFileDataA);
    
    uint computeTableChecksum(int offsetA, int length);
    uint computeTableChecksum(uchar *data, int length);
    void cvtCharStrings(char **encoding,
		                int *codeToGID,
		                FontFileOutputFunc outputFunc,
		                void *outputStream);
    void cvtEncoding(char **encoding,
		             FontFileOutputFunc outputFunc,
		             void *outputStream);
    void cvtSfnts(FontFileOutputFunc outputFunc,
			            void *outputStream, 
			            XWString *name,
			    				bool needVerticalMetrics,
			    				int *maxUsedGlyph);
		          
    void dumpString(int offsetA, 
                    int length,
		            FontFileOutputFunc outputFunc,
		            void *outputStream);
    void dumpString(uchar *s, 
                    int length,
		            FontFileOutputFunc outputFunc,
		            void *outputStream);
		            
    void parse();
    
    void readPostTable();
    
    int seekTable(char *tag);
    
private:
    TrueTypeTable *tables;
    int nTables;
    TrueTypeCmap *cmaps;
    int nCmaps;
    int nGlyphs;
    int locaFmt;
    int bbox[4];
    
    XWHash *nameToGID;
    
    bool openTypeCFF;
    bool parsedOk;
};

#endif // XWFONTFILETRUETYPE_H

