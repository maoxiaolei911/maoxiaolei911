/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWSTREAM_H
#define XWSTREAM_H

#include <stdio.h>
#include <QIODevice>

#include "XWObject.h"

#define STREAM_FILE       0
#define STREAM_ASCIIHEX   1
#define STREAM_ASCII85    2
#define STREAM_LZW        3
#define STREAM_RUNLENGTH  4
#define STREAM_CCITTFAX   5
#define STREAM_DCT        6
#define STREAM_FLATE      7
#define STREAM_JBIG2      8
#define STREAM_JPX        9
#define STREAM_WEIRD      10

#define STREAM_CS_NONE        0
#define STREAM_CS_DEVICEGRAY  1
#define STREAM_CS_DEVICERGB   2
#define STREAM_CS_DEVICECMYK  3

class XWBaseStream;

enum CryptAlgorithm {
  cryptRC4,
  cryptAES,
  cryptAES256
};


class XW_CORE_EXPORT XWStream
{
public:
    XWStream();
    virtual ~XWStream() {}
    
    XWStream * addFilters(XWObject *dict);
    
    virtual void close();
    
    int  decRef() {return --ref;}
    
    virtual XWBaseStream *getBaseStream() = 0;
    virtual int getBlock(char *blk, int size);
    virtual int    getChar() = 0;
    virtual XWDict * getDict() = 0;
    virtual void getImageParams(int *, int *) {}
    virtual int    getKind() = 0;
    virtual char * getLine(char *buf, int size);
    virtual XWStream *getNextStream() { return 0; }
    virtual int    getPos() = 0;
    virtual XWString * getPSFilter(int, const char *);
    virtual int    getRawChar();
    virtual XWStream *getUndecodedStream() = 0;
    
    int  incRef() {return ++ref;}
    virtual bool isBinary(bool last = true) = 0;
    virtual bool isEncoder() { return false; }
    
    virtual int lookChar() = 0;
            
    virtual void reset() = 0;
    
    virtual void setPos(uint pos, int dir = 0) = 0;
    
private:
    XWStream * makeFilter(const char *name, XWStream *str, XWObject *params);
    
private:
    int ref;
};

class XW_CORE_EXPORT XWBaseStream : public XWStream
{
public:
	XWBaseStream();
    XWBaseStream(XWObject * dictA);
    virtual ~XWBaseStream();
    
    virtual XWBaseStream *getBaseStream() { return this; }
    virtual XWDict * getDict() { return dict.getDict(); }
    virtual XWString * getFileName() { return 0; }
    virtual uint getStart() = 0;
    virtual XWStream *getUndecodedStream() { return this; }
    
    virtual bool isBinary(bool last = true) { return last; }
       
    virtual XWStream *makeSubStream(uint start, bool limited, uint length, XWObject *dictA) = 0;
    virtual void moveStart(int delta) = 0;
    
    void setDict(XWObject * dictA);
    virtual void setPos(uint pos, int dir = 0) = 0;
    
private:
    XWObject dict;
};

class XW_CORE_EXPORT XWFilterStream : public XWStream
{
public:
    XWFilterStream(XWStream * strA);
    virtual ~XWFilterStream();
    
    virtual void close();
    
    virtual XWBaseStream *getBaseStream() { return str->getBaseStream(); }
    virtual XWDict *getDict() { return str->getDict(); }
    virtual XWStream *getNextStream() { return str; }
    virtual int getPos() { return str->getPos(); }
    virtual XWStream *getUndecodedStream() { return str->getUndecodedStream(); }
    
    virtual void setPos(uint, int);
    
protected:
    XWStream * str;
};

class XW_CORE_EXPORT XWImageStream
{
public:
    XWImageStream(XWStream *strA, 
                  int widthA, 
                  int nCompsA, 
                  int nBitsA);
    ~XWImageStream();
    
    uchar *getLine();
    bool    getPixel(uchar *pix);
    
    void reset();
    
    void skipLine();
    
private:
    XWStream *str;			// base stream
  int width;			// pixels per line
  int nComps;			// components per pixel
  int nBits;			// bits per component
  int nVals;			// components per line
  int inputLineSize;		// input line buffer size
  char *inputLine;		// input line buffer
  uchar *imgLine;		// line buffer
  int imgIdx;			// current index in imgLine
};

class XW_CORE_EXPORT XWStreamPredictor
{
public:
    XWStreamPredictor(XWStream *strA, 
                      int predictorA,
		              int widthA, 
		              int nCompsA, 
		              int nBitsA);
    ~XWStreamPredictor();
    
    int getBlock(char *blk, int size);
    int getChar();
    
    bool isOk() { return ok; }

    int lookChar();
    
private:
    bool getNextLine();
    
private:
    XWStream *str; // base stream
    int predictor;		 // predictor
    int width;			 // pixels per line
    int nComps;			 // components per pixel
    int nBits;			 // bits per component
    int nVals;			 // components per line
    int pixBytes;		 // bytes per pixel
    int rowBytes;		 // bytes per line
    uchar *predLine;	 // line buffer
    int predIdx;		 // current index in predLine
    bool ok;
};

#define FILE_STREAM_BUFSIZE 256

class XW_CORE_EXPORT XWFileStream : public XWBaseStream
{
public:
	XWFileStream(QIODevice *fA);
    XWFileStream(QIODevice *fA, 
                 uint startA, 
                 bool limitedA,
	             uint lengthA, 
	             XWObject *dictA);
    virtual ~XWFileStream();
    
    virtual void close();
    
    virtual int   getBlock(char *blk, int size);
    virtual int   getChar()
                   { return (bufPtr >= bufEnd && !fillBuf()) ? EOF : (*bufPtr++ & 0xff); }
                   	
	QIODevice * getDev() {return f;}
	
    virtual int   getKind() { return STREAM_FILE; }
    virtual int   getPos() { return bufPos + (bufPtr - buf); }  
    virtual uint  getStart() { return start; }
    
    virtual int lookChar()
            { return (bufPtr >= bufEnd && !fillBuf()) ? EOF : (*bufPtr & 0xff); }
    
    virtual XWStream *makeSubStream(uint startA, 
                                    bool limitedA,
				                    uint lengthA, 
				                    XWObject *dictA);
    virtual void moveStart(int delta);
    
    virtual void reset();
    
    virtual void setPos(uint pos, int dir = 0);

private:
    bool fillBuf();
    
private:
    QIODevice *f;
    uint start;
    bool limited;
    uint length;
    uint bufPos;
    int savePos;
    bool saved;
    char *bufPtr;
    char *bufEnd;
    char buf[FILE_STREAM_BUFSIZE];
};

class XW_CORE_EXPORT XWMemStream : public XWBaseStream
{
public:
    XWMemStream(char *bufA, 
                uint startA, 
                uint lengthA, 
                XWObject *dictA);
    virtual ~XWMemStream();
    
    virtual void close();
        
    virtual int  getBlock(char *blk, int size);
    virtual int  getChar()
                 { return (bufPtr < bufEnd) ? (*bufPtr++ & 0xff) : EOF; }    
    virtual int  getKind() { return STREAM_WEIRD; }
    virtual int  getPos() { return (int)(bufPtr - buf); }    
    virtual uint getStart() { return start; }
    
    virtual int lookChar()
                { return (bufPtr < bufEnd) ? (*bufPtr & 0xff) : EOF; }
                    
    virtual XWStream *makeSubStream(uint startA, 
                                    bool limited,
				                    uint lengthA, 
				                    XWObject *dictA);
    virtual void moveStart(int delta);
    
    virtual void reset();
    
    virtual void setPos(uint pos, int dir = 0);

private:
    char *buf;
    uint start;
    uint length;
    bool needFree;
    char *bufEnd;
    char *bufPtr;
};

class XW_CORE_EXPORT XWEmbedStream : public XWBaseStream
{
public:
    XWEmbedStream(XWStream *strA, 
                  XWObject *dictA, 
                  bool limitedA, 
                  uint lengthA);
    virtual ~XWEmbedStream();
            
    virtual int  getBlock(char *blk, int size);
    virtual int  getChar();   
    virtual int  getKind() { return str->getKind(); }
    virtual int  getPos() { return str->getPos(); }    
    virtual uint getStart();
    
    virtual int lookChar();
    
    virtual XWStream *makeSubStream(uint, 
                                    bool,
				                    uint, 
				                    XWObject *);
    virtual void moveStart(int);
    
    virtual void reset() {}
    
    virtual void setPos(uint, int dir);

private:
    XWStream *str;
    bool limited;
    uint length;
};


class XW_CORE_EXPORT XWASCIIHexStream : public XWFilterStream
{
public:
    XWASCIIHexStream(XWStream *strA);
    virtual ~XWASCIIHexStream();
            
    virtual int    getChar()
                     { int c = lookChar(); buf = EOF; return c; }
    virtual int    getKind() { return STREAM_ASCIIHEX; }
    virtual XWString *getPSFilter(int psLevel, const char *indent);
    
    virtual bool isBinary(bool);
    
    virtual int lookChar();
        
    virtual void reset();
    
private:
    int buf;
    bool eof;
};

class XW_CORE_EXPORT XWASCII85Stream : public XWFilterStream
{
public:
    XWASCII85Stream(XWStream *strA);
    virtual ~XWASCII85Stream();
            
    virtual int    getChar()
                     { int ch = lookChar(); ++index; return ch; }
    virtual int    getKind() { return STREAM_ASCII85; }
    virtual XWString *getPSFilter(int psLevel, const char *indent);
    virtual bool isBinary(bool);
    
    virtual int lookChar();
        
    virtual void reset();
    
private:
    int index, n;
    bool eof;
    int c[5];
    int b[4];
};

class XW_CORE_EXPORT XWLZWStream : public XWFilterStream
{
public:
    XWLZWStream(XWStream *strA, 
                int predictor, 
                int columns, 
                int colors,
	            int bits, 
	            int earlyA);
    virtual ~XWLZWStream();
    
    virtual int getBlock(char *blk, int size);
    virtual int getChar();
    virtual int getKind() { return STREAM_LZW; }
    virtual XWString *getPSFilter(int psLevel, const char *indent);
    virtual int getRawChar();    
    
    virtual bool isBinary(bool);
    
    virtual int lookChar();
    
    virtual void reset();
    
private:
    void clearTable();
    
    int getCode();
    
    bool processNextCode();
    
private:
    XWStreamPredictor  *pred;
    int early;			// early parameter
    bool eof;			// true if at eof
    int inputBuf;			// input buffer
    int inputBits;		// number of bits in input buffer
    struct              // decoding table
    {			
        int length;
        int head;
        uchar tail;
    } table[4097];
    int nextCode;			// next code to be used
    int nextBits;			// number of bits in next code word
    int prevCode;			// previous code used in stream
    int newChar;			// next char to be added to table
    uchar seqBuf[4097];		// buffer for current sequence
    int seqLength;		// length of current sequence
    int seqIndex;			// index into current sequence
    bool first;			// first code after a table clear
};

class XW_CORE_EXPORT XWRunLengthStream : public XWFilterStream
{
public:
    XWRunLengthStream(XWStream *strA);
    virtual ~XWRunLengthStream();
    
    virtual int getBlock(char *blk, int size);
    virtual int getChar()
            { return (bufPtr >= bufEnd && !fillBuf()) ? EOF : (*bufPtr++ & 0xff); }
    virtual int getKind() { return STREAM_RUNLENGTH; }
    virtual XWString *getPSFilter(int psLevel, const char *indent);
    
    virtual bool isBinary(bool);
    
    virtual int lookChar()
            { return (bufPtr >= bufEnd && !fillBuf()) ? EOF : (*bufPtr & 0xff); }
    
    virtual void reset();

private:
    bool fillBuf();
    
private:
    char buf[128];		// buffer
    char *bufPtr;			// next char to read
    char *bufEnd;			// end of buffer
    bool eof;    
};

class XW_CORE_EXPORT XWCCITTFaxStream : public XWFilterStream
{
public:
    XWCCITTFaxStream(XWStream *strA, 
                     int encodingA, 
                     bool endOfLineA,
		             bool byteAlignA, 
		             int columnsA, 
		             int rowsA,
		             bool endOfBlockA, 
		             bool blackA);
    virtual ~XWCCITTFaxStream();
    
    virtual int getChar()
                { int c = lookChar(); buf = EOF; return c; }
    virtual int getKind() { return STREAM_CCITTFAX; }
    virtual XWString *getPSFilter(int psLevel, const char *indent);
    
    virtual bool isBinary(bool);
    
    virtual int lookChar();    
    
    virtual void reset();
  
private:
	void addPixels(int a1, int blackPixels);
	void addPixelsNeg(int a1, int blackPixels);
	
    void eatBits(int n) { if ((inputBits -= n) < 0) inputBits = 0; }
    
    short getBlackCode();    
    short getTwoDimCode();
    short getWhiteCode();
    
    short lookBits(int n);
  
private:
    int encoding;			// 'K' parameter
  bool endOfLine;		// 'EndOfLine' parameter
  bool byteAlign;		// 'EncodedByteAlign' parameter
  int columns;			// 'Columns' parameter
  int rows;			// 'Rows' parameter
  bool endOfBlock;		// 'EndOfBlock' parameter
  bool black;			// 'BlackIs1' parameter
  bool eof;			// true if at eof
  bool nextLine2D;		// true if next line uses 2D encoding
  int row;			// current row
  uint inputBuf;		// input buffer
  int inputBits;		// number of bits in input buffer
  int *codingLine;		// coding line changing elements
  int *refLine;			// reference line changing elements
  int a0i;			// index into codingLine
  bool err;			// error on current line
  int outputBits;		// remaining ouput bits
  int buf;			// character buffer
};

struct DCTCompInfo
{
    int id;
    int hSample, vSample;
    int quantTable;
    int prevDC;
};
    
struct DCTScanInfo
{
    bool comp[4];
    int  numComps;
    int  dcHuffTable[4];
    int  acHuffTable[4];
    int firstCoeff, lastCoeff;
    int ah, al;
};
    
struct DCTHuffTable
{
    uchar firstSym[17];
    ushort firstCode[17];
    ushort numCodes[17];
    uchar sym[256];
};

class XW_CORE_EXPORT XWDCTStream : public XWFilterStream
{
public:
    XWDCTStream(XWStream *strA, 
                int colorXformA);
    virtual ~XWDCTStream();
    
    virtual void close();
    
    virtual int getChar();
    virtual int getKind() { return STREAM_DCT; }
    virtual XWString *getPSFilter(int psLevel, const char *indent);
    XWStream *getRawStream() { return str; }
    
    virtual bool isBinary(bool);
    
    virtual int lookChar();
    
    virtual void reset();
    
private:
    void decodeImage();
    
    bool readAdobeMarker();
    int  readAmp(int size);
    bool readBaselineSOF();
    int  readBit();  
    bool readDataUnit(DCTHuffTable *dcHuffTable,
	                  DCTHuffTable *acHuffTable,
		              int *prevDC, int data[64]);
    bool readHeader();
    bool readHuffmanTables();
    int  readHuffSym(DCTHuffTable *table);
    bool readJFIFMarker();
    int  readMarker();
    bool readMCURow();
    bool readProgressiveDataUnit(DCTHuffTable *dcHuffTable,
	                             DCTHuffTable *acHuffTable,
				                 int *prevDC, int data[64]);
    bool readProgressiveSOF();
    bool readQuantTables();    
    bool readRestartInterval();    
    void readScan();
    bool readScanInfo();
    bool readTrailer();    
    int  read16();
    void restart();
    
    void transformDataUnit(ushort *quantTable, int dataIn[64], uchar dataOut[64]);
    
private:
    bool progressive;		// set if in progressive mode
    bool interleaved;		// set if in interleaved mode
    int width, height;		// image size
    int mcuWidth, mcuHeight;	// size of min coding unit, in data units
    int bufWidth, bufHeight;	// frameBuf size
    DCTCompInfo compInfo[4];	// info for each component
    DCTScanInfo scanInfo;		// info for the current scan
    int numComps;			// number of components in image
    int colorXform;		// color transform: -1 = unspecified
	        			//                   0 = none
			        	//                   1 = YUV/YUVK -> RGB/CMYK
    bool gotJFIFMarker;		// set if APP0 JFIF marker was present
    bool gotAdobeMarker;		// set if APP14 Adobe marker was present
    int restartInterval;		// restart interval, in MCUs
    ushort quantTables[4][64];	// quantization tables
    int numQuantTables;		// number of quantization tables
    DCTHuffTable dcHuffTables[4];	// DC Huffman tables
    DCTHuffTable acHuffTables[4];	// AC Huffman tables
    int numDCHuffTables;		// number of DC Huffman tables
    int numACHuffTables;		// number of AC Huffman tables
    uchar *rowBuf[4][32];	// buffer for one MCU (non-progressive mode)
    int *frameBuf[4];		// buffer for frame (progressive mode)
    int comp, x, y, dy;		// current position within image/MCU
    int restartCtr;		// MCUs left until restart
    int restartMarker;		// next restart marker
    int eobRun;			// number of EOBs left in the current run
    int inputBuf;			// input buffer for variable length codes
    int inputBits;		// number of valid bits in input buffer
};


#define flateWindow          32768
#define flateMask            (flateWindow-1)
#define flateMaxHuffman         15
#define flateMaxCodeLenCodes    19
#define flateMaxLitCodes       288
#define flateMaxDistCodes       30


// Huffman code table entry
struct FlateCode {
  ushort len;			// code length, in bits
  ushort val;			// value represented by this code
};

struct FlateHuffmanTab {
  FlateCode *codes;
  int maxLen;
};

// Decoding info for length and distance code words
struct FlateDecode {
  int bits;			// # extra bits
  int first;			// first length/distance
};

class XW_CORE_EXPORT XWFlateStream : public XWFilterStream
{
public:
    XWFlateStream(XWStream *strA, 
                  int predictor, 
                  int columns,
	              int colors, 
	              int bits);
    virtual ~XWFlateStream();
    
    virtual int getBlock(char *blk, int size);
    virtual int getChar();
    virtual int getKind() { return STREAM_FLATE; }
    virtual XWString *getPSFilter(int psLevel, const char *indent);
    virtual int getRawChar();
    
    virtual bool isBinary(bool);
    
    virtual int lookChar();
    
    virtual void reset();

private:
    void compHuffmanCodes(int *lengths, int n, FlateHuffmanTab *tab);
    
    int getCodeWord(int bits);
    int getHuffmanCodeWord(FlateHuffmanTab *tab);
    
    void loadFixedCodes();
    
    bool readDynamicCodes();
    void readSome();
    
    bool startBlock();

private:
    XWStreamPredictor *pred;	// predictor
    uchar buf[flateWindow];	// output data buffer
    int index;			// current index into output buffer
    int remain;			// number valid bytes in output buffer
    int codeBuf;			// input buffer
    int codeSize;			// number of bits in input buffer
    int				// literal and distance code lengths
        codeLengths[flateMaxLitCodes + flateMaxDistCodes];
    FlateHuffmanTab litCodeTab;	// literal code table
    FlateHuffmanTab distCodeTab;	// distance code table
    bool compressedBlock;	// set if reading a compressed block
    int blockLen;			// remaining length of uncompressed block
    bool endOfBlock;		// set when end of block is reached
    bool eof;			// set when end of stream is reached

    static int			// code length code reordering
        codeLenCodeMap[flateMaxCodeLenCodes];
    static FlateDecode		// length decoding info
        lengthDecode[flateMaxLitCodes-257];
    static FlateDecode		// distance decoding info
        distDecode[flateMaxDistCodes];
    static FlateHuffmanTab	// fixed literal code table
        fixedLitCodeTab;
    static FlateHuffmanTab	// fixed distance code table
        fixedDistCodeTab;
};

class XW_CORE_EXPORT XWEOFStream : public XWFilterStream
{
public:
    XWEOFStream(XWStream *strA);
    virtual ~XWEOFStream();
    
    virtual int getChar() { return EOF; }
    virtual int getKind() { return STREAM_WEIRD; }
    
    virtual bool isBinary(bool) { return false; }
    
    virtual int lookChar() { return EOF; }
    
    virtual void reset() {}
};


class XWBufStream: public XWFilterStream 
{
public:

  XWBufStream(XWStream *strA, int bufSizeA);
  virtual ~XWBufStream();
  virtual int getKind() { return STREAM_WEIRD; }
  virtual void reset();
  virtual int getChar();
  virtual int lookChar();
  virtual XWString *getPSFilter(int, const char *)
    { return NULL; }
  virtual bool isBinary(bool last);

  int lookChar(int idx);

private:

  int *buf;
  int bufSize;
};


class XW_CORE_EXPORT XWFixedLengthEncoder: public XWFilterStream 
{
public:
  	XWFixedLengthEncoder(XWStream *strA, 
  	                     int lengthA);
  	virtual ~XWFixedLengthEncoder();
  	
  	virtual int getChar();
  	virtual int getKind() { return STREAM_WEIRD; }
  	virtual XWString *getPSFilter(int, const char *) { return NULL; }
  	
  	virtual bool isBinary(bool);
  	virtual bool isEncoder() { return true; }
  	
  	virtual int lookChar();
  	
  	virtual void reset();

private:
  	int length;
  	int count;
};

class XW_CORE_EXPORT XWASCIIHexEncoder: public XWFilterStream 
{
public:
  	XWASCIIHexEncoder(XWStream *strA);
  	virtual ~XWASCIIHexEncoder();
  	virtual int getChar()
    	{ return (bufPtr >= bufEnd && !fillBuf()) ? EOF : (*bufPtr++ & 0xff); }
  	virtual int getKind() { return STREAM_WEIRD; }
  	virtual XWString *getPSFilter(int, const char *) { return NULL; }
  	
  	virtual bool isBinary(bool) { return false; }
  	virtual bool isEncoder() { return true; }
  	
  	virtual int lookChar()
    	{ return (bufPtr >= bufEnd && !fillBuf()) ? EOF : (*bufPtr & 0xff); }
    		
  	virtual void reset();

private:
	bool fillBuf();
	
private:
  	char buf[4];
  	char *bufPtr;
  	char *bufEnd;
  	int lineLen;
  	bool eof;
};

class XW_CORE_EXPORT XWASCII85Encoder: public XWFilterStream 
{
public:
  	XWASCII85Encoder(XWStream *strA);
  	virtual ~XWASCII85Encoder();
  	
  	virtual int getChar()
    	{ return (bufPtr >= bufEnd && !fillBuf()) ? EOF : (*bufPtr++ & 0xff); }
    		
  	virtual int getKind() { return STREAM_WEIRD; }
  	
  	virtual bool isBinary(bool) { return false; }
  	virtual bool isEncoder() { return true; }
  	virtual int lookChar()
    	{ return (bufPtr >= bufEnd && !fillBuf()) ? EOF : (*bufPtr & 0xff); }
  	virtual XWString *getPSFilter(int, const char *) { return NULL; }
  	
  	virtual void reset();

private:
	bool fillBuf();
	
private:
  	char buf[8];
  	char *bufPtr;
  	char *bufEnd;
  	int lineLen;
  	bool eof;
};

class XW_CORE_EXPORT XWRunLengthEncoder: public XWFilterStream 
{
public:
  	XWRunLengthEncoder(XWStream *strA);
  	virtual ~XWRunLengthEncoder();
  	
  	virtual int getChar()
    	{ return (bufPtr >= bufEnd && !fillBuf()) ? EOF : (*bufPtr++ & 0xff); }
  	virtual int getKind() { return STREAM_WEIRD; }
  	virtual XWString *getPSFilter(int, const char *) { return NULL; }
  	
  	virtual bool isBinary(bool) { return true; }
  	virtual bool isEncoder() { return true; }
  	
  	virtual int lookChar()
    	{ return (bufPtr >= bufEnd && !fillBuf()) ? EOF : (*bufPtr & 0xff); }
  	
  	virtual void reset();

private:
	bool fillBuf();
	
private:
  	char buf[131];
  	char *bufPtr;
  	char *bufEnd;
  	char *nextEnd;
  	bool eof;
};

#endif // XWSTREAM_H

