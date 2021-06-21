/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWFONTFILETYPE1C_H
#define XWFONTFILETYPE1C_H


#include "XWFontFile.h"

class XWString;

struct XW_FONT_EXPORT Type1CIndex 
{
    int pos;			// absolute position in file
    int len;			// length (number of entries)
    int offSize;			// offset size
    int startPos;			// position of start of index data - 1
    int endPos;			// position one byte past end of the index
};

struct XW_FONT_EXPORT Type1CIndexVal 
{
    int pos;			// absolute position in file
    int len;			// length, in bytes
};

struct XW_FONT_EXPORT Type1CTopDict 
{
    int firstOp;

    int versionSID;
    int noticeSID;
    int copyrightSID;
    int fullNameSID;
    int familyNameSID;
    int weightSID;
    int isFixedPitch;
    double italicAngle;
    double underlinePosition;
    double underlineThickness;
    int paintType;
    int charstringType;
    double fontMatrix[6];
    bool hasFontMatrix;		// CID fonts are allowed to put their
				//   FontMatrix in the FD instead of the
				//   top dict
    int uniqueID;
    double fontBBox[4];
    double strokeWidth;
    int charsetOffset;
    int encodingOffset;
    int charStringsOffset;
    int privateSize;
    int privateOffset;

    // CIDFont entries
    int registrySID;
    int orderingSID;
    int supplement;
    int fdArrayOffset;
    int fdSelectOffset;
};

#define type1CMaxBlueValues 14
#define type1CMaxOtherBlues 10
#define type1CMaxStemSnap   12

struct XW_FONT_EXPORT Type1CPrivateDict 
{
    double fontMatrix[6];
    bool hasFontMatrix;
    int blueValues[type1CMaxBlueValues];
    int nBlueValues;
    int otherBlues[type1CMaxOtherBlues];
    int nOtherBlues;
    int familyBlues[type1CMaxBlueValues];
    int nFamilyBlues;
    int familyOtherBlues[type1CMaxOtherBlues];
    int nFamilyOtherBlues;
    double blueScale;
    int blueShift;
    int blueFuzz;
    double stdHW;
    bool hasStdHW;
    double stdVW;
    bool hasStdVW;
    double stemSnapH[type1CMaxStemSnap];
    int nStemSnapH;
    double stemSnapV[type1CMaxStemSnap];
    int nStemSnapV;
    bool forceBold;
    bool hasForceBold;
    double forceBoldThreshold;
    int languageGroup;
    double expansionFactor;
    int initialRandomSeed;
    int subrsOffset;
    double defaultWidthX;
    bool defaultWidthXFP;
    double nominalWidthX;
    bool nominalWidthXFP;
};

struct XW_FONT_EXPORT Type1COp 
{
    bool isNum;			// true -> number, false -> operator
    bool isFP;			// true -> floating point number, false -> int
    union 
    {
        double num;			// if num is true
        int op;			// if num is false
    };
};

struct XW_FONT_EXPORT Type1CEexecBuf 
{
    FontFileOutputFunc outputFunc;
    void *outputStream;
    bool ascii;			// ASCII encoding?
    ushort r1;			// eexec encryption key
    int line;			// number of eexec chars left on current line
};


class XW_FONT_EXPORT XWFontFileType1C : public XWFontFile
{
public:    
	virtual ~XWFontFileType1C();
	
    void convertToCIDType0(char *psName, 
                           int *codeMap, 
                           int nCodes,
				                   FontFileOutputFunc outputFunc,
				                   void *outputStream);
    void convertToType0(char *psName, 
                        int *codeMap, 
                        int nCodes,
				                FontFileOutputFunc outputFunc,
				                void *outputStream);
    void convertToType1(char *psName, 
                        char **newEncoding, 
                        bool ascii,
		                FontFileOutputFunc outputFunc, 
		                void *outputStream);
		      
    int *  getCIDToGIDMap(int *nCIDs);
    char   ** getEncoding();
    void getFontMatrix(double *mat);
    XWString * getGlyphName(int gid);
    char   *  getName();
    
    static XWFontFileType1C *load(char *fileName);
    
    static XWFontFileType1C *make(char *buf, int lenA);
    
private:
    XWFontFileType1C(QIODevice * fileA,
		 	         int    fileFinalA,
		 	         ulong offsetA,
		 	         ulong lenA);
		XWFontFileType1C(uchar *filedataA, int lenA, bool freeFileDataA);
    
    void buildEncoding();
    
    void cvtGlyph(int offsetA, 
                  int nBytes, 
                  XWString *charBuf,
		          Type1CIndex *subrIdx, 
		          Type1CPrivateDict *pDict,
		          bool top);
    void cvtGlyphWidth(bool useOp, 
                       XWString *charBuf,
		               Type1CPrivateDict *pDict);
    void cvtNum(double x, bool isFP, XWString *charBuf);
    
    void eexecCvtGlyph(Type1CEexecBuf *eb, 
                       char *glyphName,
		               int offsetA, 
		               int nBytes,
		               Type1CIndex *subrIdx,
		               Type1CPrivateDict *pDict);
    void eexecWrite(Type1CEexecBuf *eb, char *s);
    void eexecWriteCharstring(Type1CEexecBuf *eb, uchar *s, int n);
    
    int    getDeltaFPArray(double *arr, int maxLen);
    int    getDeltaIntArray(int *arr, int maxLen);
    void   getIndex(int pos, Type1CIndex *idx, bool *ok);
    void   getIndexVal(Type1CIndex *idx, 
                       int i, 
                       Type1CIndexVal *val, 
                       bool *ok);
    int    getOp(int pos, bool charstring, bool *ok);
    char * getString(int sid, char *buf, bool *ok);
    
    bool parse();
    
    bool readCharset();
    void readFD(int offsetA, int length, Type1CPrivateDict *pDict);
    void readFDSelect();
    void readPrivateDict(int offsetA, int length, Type1CPrivateDict *pDict);
    void readTopDict();
    
    void writePSString(char *s, FontFileOutputFunc outputFunc, void *outputStream);
    
private:
    XWString *name;
    char **encoding;

    Type1CIndex nameIdx;
    Type1CIndex topDictIdx;
    Type1CIndex stringIdx;
    Type1CIndex gsubrIdx;
    Type1CIndex charStringsIdx;

    Type1CTopDict topDict;
    Type1CPrivateDict *privateDicts;

    int nFDs;
    uchar *fdSelect;
    ushort *charset;
    int gsubrBias;

    bool parsedOk;

    Type1COp ops[49];
    int nOps;	
    int nHints;	
    bool firstOp;
    bool openPath;
};

#endif // XWFONTFILETYPE1C_H

