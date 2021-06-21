/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWFONTFILE_H
#define XWFONTFILE_H

#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_TRUETYPE_TABLES_H
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <search.h>
#include <QObject>
#include <QString>
#include <QIODevice>
#include <QFile>
#include <QByteArray>

#include "XWGlobal.h"
#include "XWFontSea.h"

typedef void (*FontFileOutputFunc)(void *stream, char *data, int len);

#define FONTFILE_CLOSE    0X00000001
#define FONTFILE_REMOVE   0X00000002
#define FONTFILE_DEL      0X00000004

class XWString;

enum FoFiIdentifierType 
{
  fofiIdType1PFA,		// Type 1 font in PFA format
  fofiIdType1PFB,		// Type 1 font in PFB format
  fofiIdCFF8Bit,		// 8-bit CFF font
  fofiIdCFFCID,			// CID CFF font
  fofiIdTrueType,		// TrueType font
  fofiIdTrueTypeCollection,	// TrueType collection
  fofiIdOpenTypeCFF8Bit,	// OpenType wrapper with 8-bit CFF font
  fofiIdOpenTypeCFFCID,		// OpenType wrapper with CID CFF font
  fofiIdUnknown,		// unknown type
  fofiIdError			// error in reading the file
};

class XWFontFileIdentifier 
{
public:

  static FoFiIdentifierType identifyMem(char *file, int len);
  static FoFiIdentifierType identifyFile(char *fileName);
  static FoFiIdentifierType identifyStream(int (*getChar)(void *data),
					   void *data);
};


class XW_FONT_EXPORT XWFontFile : public QObject
{
	Q_OBJECT
	
public:
	virtual ~XWFontFile();
	
	bool atEnd() {return file->atEnd();}
	
	bool checkRegion(ulong pos, ulong size);
	
	ulong ftread(uchar* buf, ulong lenA);
	char  ftsignedByte();
	short ftsignedPair();
	long  ftsignedQuad();
	uchar ftunsignedByte();
	ushort ftunsignedPair();
	ulong  ftunsignedQuad();
	
    uchar  getByte();
    uchar  getCard8();
    ushort getCard16();    
    char   getChar();
    QIODevice * getFile() {return file;}
    long   getLong();
    int    getNumGlyphs() {return nGlyphs;}    
    ulong  getOffset(long n);
    short getShort();
    XWString * getString(ulong offsetA, ulong lenA);
    char   getS8(int pos, bool * ok);
    short  getS16BE(int pos, bool * ok);
    long   getS32BE(int pos, bool * ok);
    ulong  getULong();
    ushort getUShort();
    uchar  getU8(int pos, bool * ok);
    int    getU16BE(int pos, bool * ok);
	ulong  getU32BE(int pos, bool * ok);
	uint getU32LE(int pos, bool *ok);
	ulong  getUVarBE(int pos, int size, bool * ok);
    
    bool isOK() {return ok;}
    
    void output(FontFileOutputFunc outputFunc, 
                void *outputStream, 
                ulong offsetA, 
                ulong lenA);
                
	static int putBigEndian(void *s, long q, int n);
	static int putLong(void *s, long q) {return putBigEndian(s, q, 4);}
	static int putShort(void *s, short q) {return putBigEndian(s, q, 2);}
	static int putULong(void *s, ulong q) {return putBigEndian(s, (long)q, 4);}
	static int putUShort(void *s, ushort q) {return putBigEndian(s, q, 2);}
    
    long read(char* buf, long lenA);
    
    ulong tellPosition();
    
    bool    seekAbsolute(ulong offsetA);
    virtual bool seek(ulong offsetA);
    
protected:
	XWFontFile();
	XWFontFile(QIODevice * fileA,
		 	   int    fileFinalA,
		 	   ulong  offsetA,
		 	   ulong  lenA);
	XWFontFile(uchar *filedataA, int lenA, bool freeFileDataA);
		 	   
	static QFile * open(char *fileName, XWFontSea::FileFormat fmtA);
		 
protected:
  	QIODevice * file;
  	uchar *fileData;
  	bool freeFileData;
  	int   fileFinal;
  	int  nGlyphs;
  	
  	ulong  offset;
  	ulong  len;
  	bool ok;
  	
  	FT_Face ft_face;
  	long    loc;
  	ushort * ft_to_gid;
};


#endif // XWFONTFILE_H

