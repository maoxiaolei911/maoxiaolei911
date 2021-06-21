/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef JARITHMETICDECODER_H
#define JARITHMETICDECODER_H

#include "XWGlobal.h"

class XWStream;

class XW_CORE_EXPORT JArithmeticDecoderStats 
{
public:
    JArithmeticDecoderStats(int contextSizeA);
    ~JArithmeticDecoderStats();
    JArithmeticDecoderStats *copy();
    void reset();
    int  getContextSize() { return contextSize; }
    void copyFrom(JArithmeticDecoderStats *stats);
    void setEntry(uint cx, int i, int mps);

private:
  uchar * cxTab;		// cxTab[cx] = (i[cx] << 1) + mps[cx]
  int contextSize;

  friend class JArithmeticDecoder;
};

class XW_CORE_EXPORT JArithmeticDecoder
{
public:
    JArithmeticDecoder();
    ~JArithmeticDecoder();
    
    void cleanup();
    
    int   decodeBit(uint context, JArithmeticDecoderStats *stats);
    int   decodeByte(uint context, JArithmeticDecoderStats *stats);
    uint  decodeIAID(uint codeLen, JArithmeticDecoderStats *stats);
    bool  decodeInt(int *x, JArithmeticDecoderStats *stats);
    
    uint getByteCounter() { return nBytesRead; }
    
    void resetByteCounter() { nBytesRead = 0; }
    void restart(int dataLenA);
    
    void setStream(XWStream *strA)
        { str = strA; dataLen = 0; limitStream = false; }
    void setStream(XWStream *strA, int dataLenA)
        { str = strA; dataLen = dataLenA; limitStream = true; }
    void start();
		   
private:
    void  byteIn();
    
    int   decodeIntBit(JArithmeticDecoderStats *stats);
    
    uint readByte();
    
private:
    uint buf0, buf1;
    uint c, a;
    int ct;
    
    XWStream *str;
    int dataLen;
  
    uint prev;
    uint nBytesRead;
    bool limitStream;
    
    static uint qeTab[47];
    static int nmpsTab[47];
    static int nlpsTab[47];
    static int switchTab[47];
};

#endif //JARITHMETICDECODER_H