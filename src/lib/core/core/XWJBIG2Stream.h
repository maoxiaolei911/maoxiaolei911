/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWJBIG2STREAM_H
#define XWJBIG2STREAM_H

#include "XWObject.h"
#include "XWStream.h"

class XWString;
class XWList;
class JBIG2Segment;
class JBIG2Bitmap;
class JArithmeticDecoder;
class JArithmeticDecoderStats;
class JBIG2HuffmanDecoder;
struct JBIG2HuffmanTable;
class JBIG2MMRDecoder;

class XW_CORE_EXPORT XWJBIG2Stream : public XWFilterStream
{
public:
    XWJBIG2Stream(XWStream *strA, 
                  XWObject * globalsStreamA);
    virtual ~XWJBIG2Stream();
    
    virtual void close();
    
    virtual int getBlock(char *blk, int size);
    virtual int getChar();
    virtual int getKind() { return STREAM_JBIG2; }    
    virtual XWString *getPSFilter(int, const char *);
    
    virtual bool isBinary(bool);
    
    virtual int lookChar();
    
    virtual void reset();
    
private:
    void discardSegment(uint segNum);
    
    JBIG2Segment *findSegment(uint segNum);
    
    void mmrAddPixels(int a1, int blackPixels,
		                  int *codingLine, int *a0i, int w);
		void mmrAddPixelsNeg(int a1, int blackPixels,
		                     int *codingLine, int *a0i, int w);
    
    bool readByte(int *x);
    void readCodeTableSeg(uint segNum, uint length);
    void readEndOfStripeSeg(uint length);
    void readExtensionSeg(uint length);
    JBIG2Bitmap *readGenericBitmap(bool mmr, 
                                   int w, 
                                   int h,
				                   int templ, 
				                   bool tpgdOn,
				                   bool useSkip, 
				                   JBIG2Bitmap *skip,
				                   int *atx, int *aty,
				                   int mmrDataLength);
	JBIG2Bitmap *readGenericRefinementRegion(int w, 
	                                         int h,
					                         int templ, 
					                         bool tpgrOn,
					                         JBIG2Bitmap *refBitmap,
					                         int refDX, int refDY,
					                         int *atx, int *aty);
	void readGenericRefinementRegionSeg(uint segNum, 
	                                    bool imm,
				                        bool lossless, 
				                        uint length,
				                        uint *refSegs,
				                        uint nRefSegs);
    void readGenericRegionSeg(uint segNum, 
                              bool imm,
			                  bool lossless, 
			                  uint length);
    void readHalftoneRegionSeg(uint segNum, 
                               bool imm,
			                   bool lossless, 
			                   uint length,
			                   uint *refSegs, 
			                   uint nRefSegs);
    bool readLong(int *x);
    void readPageInfoSeg(uint length);
    void readPatternDictSeg(uint segNum, uint length);
    void readProfilesSeg(uint length);
    void readSegments();
    bool readSymbolDictSeg(uint segNum, 
                           uint,
			               uint *refSegs, 
			               uint nRefSegs);
    JBIG2Bitmap *readTextRegion(bool huff, 
                                bool refine,
			                    int w, 
			                    int h,
			                    uint numInstances,
			                    uint logStrips,
			                    int numSyms,
			                    JBIG2HuffmanTable *symCodeTab,
			                    uint symCodeLen,
			                    JBIG2Bitmap **syms,
			                    uint defPixel, 
			                    uint combOp,
			                    uint transposed, 
			                    uint refCorner,
			                    int sOffset,
			                    JBIG2HuffmanTable *huffFSTable,
			                    JBIG2HuffmanTable *huffDSTable,
			                    JBIG2HuffmanTable *huffDTTable,
			                    JBIG2HuffmanTable *huffRDWTable,
			                    JBIG2HuffmanTable *huffRDHTable,
			                    JBIG2HuffmanTable *huffRDXTable,
			                    JBIG2HuffmanTable *huffRDYTable,
			                    JBIG2HuffmanTable *huffRSizeTable,
			                    uint templ,
			                    int *atx, int *aty);
	void readTextRegionSeg(uint segNum, 
                           bool imm,
			               bool lossless, 
			               uint length,
			               uint *refSegs, 
			               uint nRefSegs);
    bool readUByte(uint *x);
    bool readULong(uint *x);
    bool readUWord(uint *x);
    void resetGenericStats(uint templ, JArithmeticDecoderStats *prevStats);
    void resetIntStats(int symCodeLen);
    void resetRefinementStats(uint templ, JArithmeticDecoderStats *prevStats);  
    
private:
    XWObject globalsStream;
    uint pageW, pageH, curPageH;
    uint pageDefPixel;
    JBIG2Bitmap *pageBitmap;
    uint defCombOp;
    XWList *segments;		// [JBIG2Segment]
    XWList *globalSegments;	// [JBIG2Segment]
    XWStream *curStr;
    uchar *dataPtr;
    uchar *dataEnd;
    uint byteCounter;

    JArithmeticDecoder *arithDecoder;
    JArithmeticDecoderStats *genericRegionStats;
    JArithmeticDecoderStats *refinementRegionStats;
    JArithmeticDecoderStats *iadhStats;
    JArithmeticDecoderStats *iadwStats;
    JArithmeticDecoderStats *iaexStats;
    JArithmeticDecoderStats *iaaiStats;
    JArithmeticDecoderStats *iadtStats;
    JArithmeticDecoderStats *iaitStats;
    JArithmeticDecoderStats *iafsStats;
    JArithmeticDecoderStats *iadsStats;
    JArithmeticDecoderStats *iardxStats;
    JArithmeticDecoderStats *iardyStats;
    JArithmeticDecoderStats *iardwStats;
    JArithmeticDecoderStats *iardhStats;
    JArithmeticDecoderStats *iariStats;
    JArithmeticDecoderStats *iaidStats;
    JBIG2HuffmanDecoder *huffDecoder;
    JBIG2MMRDecoder *mmrDecoder;
};


#endif // XWJBIG2STREAM_H
