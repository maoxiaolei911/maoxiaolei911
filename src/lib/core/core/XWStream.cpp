/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>
#include "XWApplication.h"
#include "XWObject.h"
#include "XWPDFLexer.h"
#include "XWGraphixState.h"
#include "XWStream.h"
#include "XWJBIG2Stream.h"
#include "XWJPXStream.h"
#include "XWCCITT.h"

XWStream::XWStream()
     :ref(1)
{
}

XWStream * XWStream::addFilters(XWObject *dict)
{
  XWObject obj, obj2;
  XWObject params, params2;
  XWStream *str;
  int i;

  str = this;
  dict->dictLookup("Filter", &obj);
  if (obj.isNull()) {
    obj.free();
    dict->dictLookup("F", &obj);
  }
  dict->dictLookup("DecodeParms", &params);
  if (params.isNull()) {
    params.free();
    dict->dictLookup("DP", &params);
  }
  if (obj.isName()) {
    str = makeFilter(obj.getName(), str, &params);
  } else if (obj.isArray()) {
    for (i = 0; i < obj.arrayGetLength(); ++i) {
      obj.arrayGet(i, &obj2);
      if (params.isArray())
	params.arrayGet(i, &params2);
      else
	params2.initNull();
      if (obj2.isName()) {
	str = makeFilter(obj2.getName(), str, &params2);
      } else {
	xwApp->error("Bad filter name");
	str = new XWEOFStream(str);
      }
      obj2.free();
      params2.free();
    }
  } else if (!obj.isNull()) {
    xwApp->error("Bad 'Filter' attribute in stream");
  }
  obj.free();
  params.free();

  return str;
}

void XWStream::close()
{
}

int XWStream::getBlock(char *buf, int size)
{
	int n, c;

  n = 0;
  while (n < size) {
    if ((c = getChar()) == EOF) {
      break;
    }
    buf[n++] = (char)c;
  }
  return n;
}

char * XWStream::getLine(char *buf, int size)
{
  int i;
  int c;

  if (lookChar() == EOF || size < 0)
    return NULL;
  for (i = 0; i < size - 1; ++i) {
    c = getChar();
    if (c == EOF || c == '\n')
      break;
    if (c == '\r') {
      if ((c = lookChar()) == '\n')
	getChar();
      break;
    }
    buf[i] = c;
  }
  buf[i] = '\0';
  return buf;
}

XWString * XWStream::getPSFilter(int, const char *)
{
    return new XWString();
}

int XWStream::getRawChar()
{
    return EOF;
}

XWStream * XWStream::makeFilter(const char *name, XWStream *str, XWObject *params)
{
  int pred;			// parameters
  int colors;
  int bits;
  int early;
  int encoding;
  bool endOfLine, byteAlign, endOfBlock, black;
  int columns, rows;
  int colorXform;
  XWObject globals, obj;

  if (!strcmp(name, "ASCIIHexDecode") || !strcmp(name, "AHx")) {
    str = new XWASCIIHexStream(str);
  } else if (!strcmp(name, "ASCII85Decode") || !strcmp(name, "A85")) {
    str = new XWASCII85Stream(str);
  } else if (!strcmp(name, "LZWDecode") || !strcmp(name, "LZW")) {
    pred = 1;
    columns = 1;
    colors = 1;
    bits = 8;
    early = 1;
    if (params->isDict()) {
      params->dictLookup("Predictor", &obj);
      if (obj.isInt())
	pred = obj.getInt();
      obj.free();
      params->dictLookup("Columns", &obj);
      if (obj.isInt())
	columns = obj.getInt();
      obj.free();
      params->dictLookup("Colors", &obj);
      if (obj.isInt())
	colors = obj.getInt();
      obj.free();
      params->dictLookup("BitsPerComponent", &obj);
      if (obj.isInt())
	bits = obj.getInt();
      obj.free();
      params->dictLookup("EarlyChange", &obj);
      if (obj.isInt())
	early = obj.getInt();
      obj.free();
    }
    str = new XWLZWStream(str, pred, columns, colors, bits, early);
  } else if (!strcmp(name, "RunLengthDecode") || !strcmp(name, "RL")) {
    str = new XWRunLengthStream(str);
  } else if (!strcmp(name, "CCITTFaxDecode") || !strcmp(name, "CCF")) {
    encoding = 0;
    endOfLine = false;
    byteAlign = false;
    columns = 1728;
    rows = 0;
    endOfBlock = true;
    black = false;
    if (params->isDict()) {
      params->dictLookup("K", &obj);
      if (obj.isInt()) {
	encoding = obj.getInt();
      }
      obj.free();
      params->dictLookup("EndOfLine", &obj);
      if (obj.isBool()) {
	endOfLine = obj.getBool();
      }
      obj.free();
      params->dictLookup("EncodedByteAlign", &obj);
      if (obj.isBool()) {
	byteAlign = obj.getBool();
      }
      obj.free();
      params->dictLookup("Columns", &obj);
      if (obj.isInt()) {
	columns = obj.getInt();
      }
      obj.free();
      params->dictLookup("Rows", &obj);
      if (obj.isInt()) {
	rows = obj.getInt();
      }
      obj.free();
      params->dictLookup("EndOfBlock", &obj);
      if (obj.isBool()) {
	endOfBlock = obj.getBool();
      }
      obj.free();
      params->dictLookup("BlackIs1", &obj);
      if (obj.isBool()) {
	black = obj.getBool();
      }
      obj.free();
    }
    str = new XWCCITTFaxStream(str, encoding, endOfLine, byteAlign,
			     columns, rows, endOfBlock, black);
  } else if (!strcmp(name, "DCTDecode") || !strcmp(name, "DCT")) {
    colorXform = -1;
    if (params->isDict()) {
      if (params->dictLookup("ColorTransform", &obj)->isInt()) {
	colorXform = obj.getInt();
      }
      obj.free();
    }
    str = new XWDCTStream(str, colorXform);
  } else if (!strcmp(name, "FlateDecode") || !strcmp(name, "Fl")) {
    pred = 1;
    columns = 1;
    colors = 1;
    bits = 8;
    if (params->isDict()) {
      params->dictLookup("Predictor", &obj);
      if (obj.isInt())
	pred = obj.getInt();
      obj.free();
      params->dictLookup("Columns", &obj);
      if (obj.isInt())
	columns = obj.getInt();
      obj.free();
      params->dictLookup("Colors", &obj);
      if (obj.isInt())
	colors = obj.getInt();
      obj.free();
      params->dictLookup("BitsPerComponent", &obj);
      if (obj.isInt())
	bits = obj.getInt();
      obj.free();
    }
    str = new XWFlateStream(str, pred, columns, colors, bits);
  } else if (!strcmp(name, "JBIG2Decode")) {
    if (params->isDict()) {
      params->dictLookup("JBIG2Globals", &globals);
    }
    str = new XWJBIG2Stream(str, &globals);
    globals.free();
  } else if (!strcmp(name, "JPXDecode")) {
    str = new XWJPXStream(str);
  } else {
    str = new XWEOFStream(str);
  }
  return str;
}

XWBaseStream::XWBaseStream()
{
}

XWBaseStream::XWBaseStream(XWObject * dictA)
{
	dict = *dictA;
}

XWBaseStream::~XWBaseStream()
{
    dict.free();
}

void XWBaseStream::setDict(XWObject * dictA)
{
	dict = *dictA;
}

XWFilterStream::XWFilterStream(XWStream * strA)
    :str(strA)
{
}

XWFilterStream::~XWFilterStream()
{
}

void XWFilterStream::close()
{
    str->close();
}

void XWFilterStream::setPos(uint, int)
{
}

XWImageStream::XWImageStream(XWStream *strA,
                             int widthA,
                             int nCompsA,
                             int nBitsA)
    :str(strA),
     width(widthA),
     nComps(nCompsA),
     nBits(nBitsA)
{
	int imgLineSize;
  nVals = width * nComps;
  inputLineSize = (nVals * nBits + 7) >> 3;
  if (nVals > INT_MAX / nBits - 7) {
    // force a call to gmallocn(-1,...), which will throw an exception
    inputLineSize = -1;
  }
  inputLine = (char *)malloc(inputLineSize * sizeof(char));
  if (nBits == 8) {
    imgLine = (uchar *)inputLine;
  } else {
    if (nBits == 1) {
      imgLineSize = (nVals + 7) & ~7;
    } else {
      imgLineSize = nVals;
    }
    if (width > INT_MAX / nComps) {
      // force a call to gmallocn(-1,...), which will throw an exception
      imgLineSize = -1;
    }
    imgLine = (uchar *)malloc(imgLineSize * sizeof(uchar));
  }
  imgIdx = nVals;
}

XWImageStream::~XWImageStream()
{
  if (imgLine != (uchar *)inputLine) {
    free(imgLine);
  }
  if (inputLine)
  free(inputLine);
}

uchar * XWImageStream::getLine()
{
  ulong buf, bitMask;
  int bits;
  int c;
  int i;
  char *p;

  if (str->getBlock(inputLine, inputLineSize) != inputLineSize) {
    return NULL;
  }
  if (nBits == 1) {
    p = inputLine;
    for (i = 0; i < nVals; i += 8) {
      c = *p++;
      imgLine[i+0] = (uchar)((c >> 7) & 1);
      imgLine[i+1] = (uchar)((c >> 6) & 1);
      imgLine[i+2] = (uchar)((c >> 5) & 1);
      imgLine[i+3] = (uchar)((c >> 4) & 1);
      imgLine[i+4] = (uchar)((c >> 3) & 1);
      imgLine[i+5] = (uchar)((c >> 2) & 1);
      imgLine[i+6] = (uchar)((c >> 1) & 1);
      imgLine[i+7] = (uchar)(c & 1);
    }
  } else if (nBits == 8) {
    // special case: imgLine == inputLine
  } else {
    bitMask = (1 << nBits) - 1;
    buf = 0;
    bits = 0;
    p = inputLine;
    for (i = 0; i < nVals; ++i) {
      if (bits < nBits) {
	buf = (buf << 8) | (*p++ & 0xff);
	bits += 8;
      }
      imgLine[i] = (uchar)((buf >> (bits - nBits)) & bitMask);
      bits -= nBits;
    }
  }
  return imgLine;
}

bool XWImageStream::getPixel(uchar *pix)
{
  int i;

  if (imgIdx >= nVals) {
    if (!getLine()) {
      return false;
    }
    imgIdx = 0;
  }
  for (i = 0; i < nComps; ++i) {
    pix[i] = imgLine[imgIdx++];
  }
  return true;
}

void XWImageStream::reset()
{
    str->reset();
}

void XWImageStream::skipLine()
{
  str->getBlock(inputLine, inputLineSize);
}

XWStreamPredictor::XWStreamPredictor(XWStream *strA,
                                     int predictorA,
                                     int widthA,
                                     int nCompsA,
                                     int nBitsA)
    :str(strA),
     predictor(predictorA),
     width(widthA),
     nComps(nCompsA),
     nBits(nBitsA)
{
	predLine = 0;
    ok = false;
  nVals = width * nComps;
  pixBytes = (nComps * nBits + 7) >> 3;
  rowBytes = ((nVals * nBits + 7) >> 3) + pixBytes;
  if (width <= 0 || nComps <= 0 || nBits <= 0 ||
      nComps > COLOR_MAX_COMPS ||
      nBits > 16 ||
      width >= INT_MAX / nComps ||      // check for overflow in nVals 
      nVals >= (INT_MAX - 7) / nBits) { // check for overflow in rowBytes
    return;
  }
  predLine = (uchar *)malloc(rowBytes * sizeof(uchar));
  memset(predLine, 0, rowBytes);
  predIdx = rowBytes;

  ok = true;
}

XWStreamPredictor::~XWStreamPredictor()
{
    if (predLine)
        free(predLine);
}

int XWStreamPredictor::getBlock(char *blk, int size)
{
	int n, m;

  n = 0;
  while (n < size) {
    if (predIdx >= rowBytes) {
      if (!getNextLine()) {
	break;
      }
    }
    m = rowBytes - predIdx;
    if (m > size - n) {
      m = size - n;
    }
    memcpy(blk + n, predLine + predIdx, m);
    predIdx += m;
    n += m;
  }
  return n;
}

int XWStreamPredictor::getChar()
{
    if (predIdx >= rowBytes)
    {
        if (!getNextLine())
            return EOF;
    }
    return predLine[predIdx++];
}

int XWStreamPredictor::lookChar()
{
    if (predIdx >= rowBytes)
    {
        if (!getNextLine())
            return EOF;
    }
    return predLine[predIdx];
}

bool XWStreamPredictor::getNextLine()
{
  int curPred;
  uchar upLeftBuf[COLOR_MAX_COMPS * 2 + 1];
  int left, up, upLeft, p, pa, pb, pc;
  int c;
  ulong inBuf, outBuf, bitMask;
  int inBits, outBits;
  int i, j, k, kk;

  // get PNG optimum predictor number
  if (predictor >= 10) {
    if ((curPred = str->getRawChar()) == EOF) {
      return false;
    }
    curPred += 10;
  } else {
    curPred = predictor;
  }

  // read the raw line, apply PNG (byte) predictor
  memset(upLeftBuf, 0, pixBytes + 1);
  for (i = pixBytes; i < rowBytes; ++i) {
    for (j = pixBytes; j > 0; --j) {
      upLeftBuf[j] = upLeftBuf[j-1];
    }
    upLeftBuf[0] = predLine[i];
    if ((c = str->getRawChar()) == EOF) {
      if (i > pixBytes) {
	// this ought to return false, but some (broken) PDF files
	// contain truncated image data, and Adobe apparently reads the
	// last partial line
	break;
      }
      return false;
    }
    switch (curPred) {
    case 11:			// PNG sub
      predLine[i] = predLine[i - pixBytes] + (uchar)c;
      break;
    case 12:			// PNG up
      predLine[i] = predLine[i] + (uchar)c;
      break;
    case 13:			// PNG average
      predLine[i] = ((predLine[i - pixBytes] + predLine[i]) >> 1) +
	            (uchar)c;
      break;
    case 14:			// PNG Paeth
      left = predLine[i - pixBytes];
      up = predLine[i];
      upLeft = upLeftBuf[pixBytes];
      p = left + up - upLeft;
      if ((pa = p - left) < 0)
	pa = -pa;
      if ((pb = p - up) < 0)
	pb = -pb;
      if ((pc = p - upLeft) < 0)
	pc = -pc;
      if (pa <= pb && pa <= pc)
	predLine[i] = left + (uchar)c;
      else if (pb <= pc)
	predLine[i] = up + (uchar)c;
      else
	predLine[i] = upLeft + (uchar)c;
      break;
    case 10:			// PNG none
    default:			// no predictor or TIFF predictor
      predLine[i] = (uchar)c;
      break;
    }
  }

  // apply TIFF (component) predictor
  if (predictor == 2) {
    if (nBits == 1) {
      inBuf = predLine[pixBytes - 1];
      for (i = pixBytes; i < rowBytes; i += 8) {
	// 1-bit add is just xor
	inBuf = (inBuf << 8) | predLine[i];
	predLine[i] ^= inBuf >> nComps;
      }
    } else if (nBits == 8) {
      for (i = pixBytes; i < rowBytes; ++i) {
	predLine[i] += predLine[i - nComps];
      }
    } else {
      memset(upLeftBuf, 0, nComps + 1);
      bitMask = (1 << nBits) - 1;
      inBuf = outBuf = 0;
      inBits = outBits = 0;
      j = k = pixBytes;
      for (i = 0; i < width; ++i) {
	for (kk = 0; kk < nComps; ++kk) {
	  if (inBits < nBits) {
	    inBuf = (inBuf << 8) | (predLine[j++] & 0xff);
	    inBits += 8;
	  }
	  upLeftBuf[kk] = (uchar)((upLeftBuf[kk] +
				    (inBuf >> (inBits - nBits))) & bitMask);
	  inBits -= nBits;
	  outBuf = (outBuf << nBits) | upLeftBuf[kk];
	  outBits += nBits;
	  if (outBits >= 8) {
	    predLine[k++] = (uchar)(outBuf >> (outBits - 8));
	    outBits -= 8;
	  }
	}
      }
      if (outBits > 0) {
	predLine[k++] = (uchar)((outBuf << (8 - outBits)) +
				 (inBuf & ((1 << (8 - outBits)) - 1)));
      }
    }
  }

  // reset to start of line
  predIdx = pixBytes;

  return true;
}

XWFileStream::XWFileStream(QIODevice *fA)
	:f(fA),
	 start(0),
     limited(0),
     length(0),
     bufPos(0),
     savePos(0),
     saved(false)
{
	bufPtr = bufEnd = buf;
}

XWFileStream::XWFileStream(QIODevice *fA,
                           uint startA,
                           bool limitedA,
                           uint lengthA,
                           XWObject *dictA)
    :XWBaseStream(dictA),
     f(fA),
     start(startA),
     limited(limitedA),
     length(lengthA),
     bufPos(startA),
     savePos(0),
     saved(false)
{
    bufPtr = bufEnd = buf;
}

XWFileStream::~XWFileStream()
{
    close();
}

void XWFileStream::close()
{
    if (saved)
    {
        f->seek(savePos);
        saved = false;
    }
}

int XWFileStream::getBlock(char *blk, int size)
{
	int n, m;

  n = 0;
  while (n < size) {
    if (bufPtr >= bufEnd) {
      if (!fillBuf()) {
	break;
      }
    }
    m = (int)(bufEnd - bufPtr);
    if (m > size - n) {
      m = size - n;
    }
    memcpy(blk + n, bufPtr, m);
    bufPtr += m;
    n += m;
  }
  return n;
}

XWStream * XWFileStream::makeSubStream(uint startA,
                                       bool limitedA,
                                       uint lengthA,
                                       XWObject *dictA)
{
    return new XWFileStream(f, startA, limitedA, lengthA, dictA);
}

void XWFileStream::moveStart(int delta)
{
    start += delta;
    bufPtr = bufEnd = buf;
    bufPos = start;
}

void XWFileStream::reset()
{
    savePos = f->pos();
    f->seek(start);
    saved = true;
    bufPtr = bufEnd = buf;
    bufPos = start;
}

void XWFileStream::setPos(uint pos, int dir)
{
    if (dir >= 0)
    {
        f->seek(pos);
        bufPos = pos;
    }
    else
    {
        uint size = f->size();
        f->seek(size);
        if (pos > size)
            pos = size;

        pos = size - pos;
        f->seek(pos);
        bufPos = f->pos();
    }

    bufPtr = bufEnd = buf;
}

bool XWFileStream::fillBuf()
{
    bufPos += bufEnd - buf;
    bufPtr = bufEnd = buf;
    if (limited && bufPos >= start + length)
        return false;

    uint n = FILE_STREAM_BUFSIZE;
    if (limited && bufPos + FILE_STREAM_BUFSIZE > start + length)
        n = start + length - bufPos;
        
    if (f->atEnd())
    	bufPtr = bufEnd + 1;
    else
    {
    	n = f->read(buf, n);
    	bufEnd = buf + n;
    }
    
    if (bufPtr >= bufEnd)
        return false;

    return true;
}

XWMemStream::XWMemStream(char *bufA, 
	                     uint startA, 
	                     uint lengthA, 
	                     XWObject *dictA)
    :XWBaseStream(dictA),
     buf(bufA),
     start(startA),
     length(lengthA),
     needFree(false)
{
    bufEnd = buf + start + length;
    bufPtr = buf + start;
}

XWMemStream::~XWMemStream()
{
    if (needFree && buf)
        delete [] buf;
}

void XWMemStream::close()
{
}

int  XWMemStream::getBlock(char *blk, int size)
{
	int n;

  if (size <= 0) {
    return 0;
  }
  if (bufEnd - bufPtr < size) {
    n = (int)(bufEnd - bufPtr);
  } else {
    n = size;
  }
  memcpy(blk, bufPtr, n);
  bufPtr += n;
  return n;
}

XWStream * XWMemStream::makeSubStream(uint startA,
                                      bool limited,
                                      uint lengthA,
                                      XWObject *dictA)
{
    uint newLength = lengthA;
    if (!limited || startA + lengthA > start + length)
        newLength = start + length - startA;

    XWMemStream * subStr = new XWMemStream(buf, startA, newLength, dictA);
    return subStr;
}

void XWMemStream::moveStart(int delta)
{
    start += delta;
    length -= delta;
    bufPtr = buf + start;
}

void XWMemStream::reset()
{
    bufPtr = buf + start;
}

void XWMemStream::setPos(uint pos, int dir)
{
    uint i = 0;
    if (dir >= 0)
        i = pos;
    else
        i = start + length - pos;

    if (i < start)
        i = start;
    else if (i > start + length)
        i = start + length;
    bufPtr = buf + i;
}

XWEmbedStream::XWEmbedStream(XWStream *strA,
                             XWObject *dictA,
                             bool limitedA,
                             uint lengthA)
    :XWBaseStream(dictA),
     str(strA),
     limited(limitedA),
     length(lengthA)
{
}

XWEmbedStream::~XWEmbedStream()
{
}

int  XWEmbedStream::getBlock(char *blk, int size)
{
	if (size <= 0) {
    return 0;
  }
  if (limited && length < (uchar)size) {
    size = (int)length;
  }
  return str->getBlock(blk, size);
}

int XWEmbedStream::getChar()
{
    if (limited && !length)
        return EOF;

    --length;
    return str->getChar();
}

uint XWEmbedStream::getStart()
{
    return 0;
}

int XWEmbedStream::lookChar()
{
    if (limited && !length)
        return EOF;

    return str->lookChar();
}

XWStream *XWEmbedStream::makeSubStream(uint,
                                       bool,
                                       uint,
                                       XWObject *)
{
    return 0;
}

void XWEmbedStream::moveStart(int)
{
    return ;
}

void XWEmbedStream::setPos(uint, int)
{
    return ;
}

XWASCIIHexStream::XWASCIIHexStream(XWStream *strA)
    :XWFilterStream(strA),
     buf(EOF),
     eof(false)
{
}

XWASCIIHexStream::~XWASCIIHexStream()
{
    if (str)
        delete str;
}

XWString * XWASCIIHexStream::getPSFilter(int psLevel, const char *indent)
{
    XWString *s = 0;
    if (psLevel < 2)
        return s;

    if (!(s = str->getPSFilter(psLevel, indent)))
        return 0;

    s->append(indent)->append("/ASCIIHexDecode filter\n");
    return s;
}

bool XWASCIIHexStream::isBinary(bool)
{
    return str->isBinary(false);
}

int XWASCIIHexStream::lookChar()
{
    if (buf != EOF)
        return buf;
        
    if (eof)
    {
        buf = EOF;
        return EOF;
    }

    int c1 = 0;
    do
    {
        c1 = str->getChar();
    } while (isspace(c1));
    
    if (c1 == '>')
    {
        eof = true;
        buf = EOF;
        return buf;
    }

    int c2 = 0;
    do
    {
        c2 = str->getChar();
    } while (isspace(c2));
    
    if (c2 == '>')
    {
        eof = true;
        c2 = '0';
    }

    int x = 0;
    if (c1 >= '0' && c1 <= '9')
        x = (c1 - '0') << 4;
    else if (c1 >= 'A' && c1 <= 'F')
        x = (c1 - 'A' + 10) << 4;
    else if (c1 >= 'a' && c1 <= 'f')
        x = (c1 - 'a' + 10) << 4;
    else if (c1 == EOF)
    {
        eof = true;
        x = 0;
    }
    else
        x = 0;

    if (c2 >= '0' && c2 <= '9')
        x += c2 - '0';
    else if (c2 >= 'A' && c2 <= 'F')
        x += c2 - 'A' + 10;
    else if (c2 >= 'a' && c2 <= 'f')
        x += c2 - 'a' + 10;
    else if (c2 == EOF)
    {
        eof = true;
        x = 0;
    }

    buf = x & 0xff;
    return buf;
}

void XWASCIIHexStream::reset()
{
    str->reset();
    buf = EOF;
    eof = false;
}

XWASCII85Stream::XWASCII85Stream(XWStream *strA)
    :XWFilterStream(strA),
     index(0),
     n(0),
     eof(false)
{
}

XWASCII85Stream::~XWASCII85Stream()
{
    if (str)
        delete str;
}

XWString *XWASCII85Stream::getPSFilter(int psLevel, const char *indent)
{
    XWString *s = 0;
    if (psLevel < 2)
        return s;

    if (!(s = str->getPSFilter(psLevel, indent)))
        return 0;

    s->append(indent)->append("/ASCII85Decode filter\n");
    return s;
}

bool XWASCII85Stream::isBinary(bool)
{
    return str->isBinary(false);
}

int XWASCII85Stream::lookChar()
{
    if (index >= n)
    {
        if (eof)
            return EOF;
            
        index = 0;
        do
        {
            c[0] = str->getChar();
        } while (XWPDFLexer::isSpace(c[0]));
        
        if (c[0] == '~' || c[0] == EOF)
        {
            eof = true;
            n = 0;
            return EOF;
        }
        else if (c[0] == 'z')
        {
            b[0] = b[1] = b[2] = b[3] = 0;
            n = 4;
        }
        else
        {
            int k = 1;
            for (; k < 5; ++k)
            {
                do
                {
                    c[k] = str->getChar();
                } while (XWPDFLexer::isSpace(c[k]));
                
                if (c[k] == '~' || c[k] == EOF)
                    break;
            }
            n = k - 1;
            if (k < 5 && (c[k] == '~' || c[k] == EOF))
            {
                for (++k; k < 5; ++k)
                c[k] = 0x21 + 84;
                eof = true;
            }
            int t = 0;
            for (k = 0; k < 5; ++k)
                t = t * 85 + (c[k] - 0x21);
            for (k = 3; k >= 0; --k)
            {
                b[k] = (int)(t & 0xff);
                t >>= 8;
            }
        }
    }
    return b[index];
}

void XWASCII85Stream::reset()
{
    str->reset();
    index = n = 0;
    eof = false;
}

XWLZWStream::XWLZWStream(XWStream *strA,
                         int predictor,
                         int columns,
                         int colors,
                         int bits,
                         int earlyA)
    :XWFilterStream(strA)
{
    pred = 0;
    if (predictor != 1)
    {
        pred = new XWStreamPredictor(this, predictor, columns, colors, bits);
        if (!pred->isOk())
        {
            delete pred;
            pred = 0;
        }
    }

    early = earlyA;
    eof = false;
    inputBits = 0;
    clearTable();
}

XWLZWStream::~XWLZWStream()
{
    if (pred)
        delete pred;

    if (str)
        delete str;
}

int XWLZWStream::getBlock(char *blk, int size)
{
	int n, m;

  if (pred) {
    return pred->getBlock(blk, size);
  }
  if (eof) {
    return 0;
  }
  n = 0;
  while (n < size) {
    if (seqIndex >= seqLength) {
      if (!processNextCode()) {
	break;
      }
    }
    m = seqLength - seqIndex;
    if (m > size - n) {
      m = size - n;
    }
    memcpy(blk + n, seqBuf + seqIndex, m);
    seqIndex += m;
    n += m;
  }
  return n;
}

int XWLZWStream::getChar()
{
    if (pred)
        return pred->getChar();

    if (eof)
        return EOF;

    if (seqIndex >= seqLength)
    {
        if (!processNextCode())
            return EOF;
    }

    return seqBuf[seqIndex++];
}

XWString * XWLZWStream::getPSFilter(int psLevel, const char *indent)
{
    XWString *s = 0;
    if (psLevel < 2 || pred)
        return s;

    if (!(s = str->getPSFilter(psLevel, indent)))
        return NULL;

    s->append(indent)->append("<< ");
    if (!early)
        s->append("/EarlyChange 0 ");
    s->append(">> /LZWDecode filter\n");
    return s;
}

int XWLZWStream::getRawChar()
{
    if (eof)
        return EOF;

    if (seqIndex >= seqLength)
    {
        if (!processNextCode())
            return EOF;
    }
    return seqBuf[seqIndex++];
}

bool XWLZWStream::isBinary(bool)
{
    return str->isBinary(true);
}

int XWLZWStream::lookChar()
{
    if (pred)
        return pred->lookChar();

    if (eof)
        return EOF;

    if (seqIndex >= seqLength)
    {
        if (!processNextCode())
            return EOF;
    }

    return seqBuf[seqIndex];
}

void XWLZWStream::reset()
{
    str->reset();
    eof = false;
    inputBits = 0;
    clearTable();
}

void XWLZWStream::clearTable()
{
    nextCode = 258;
    nextBits = 9;
    seqIndex = seqLength = 0;
    first = true;
}

int XWLZWStream::getCode()
{
    while (inputBits < nextBits)
    {
        char c = str->getChar();
        if (eof)
            return EOF;
        inputBuf = (inputBuf << 8) | (c & 0xff);
        inputBits += 8;
    }
    int code = (inputBuf >> (inputBits - nextBits)) & ((1 << nextBits) - 1);
    inputBits -= nextBits;
    return code;
}

bool XWLZWStream::processNextCode()
{
    if (eof)
        return false;

start:
    int code = getCode();
    if (code == EOF || code == 257)
    {
        eof = true;
        return false;
    }

    if (code == 256)
    {
        clearTable();
        goto start;
    }

    if (nextCode >= 4097)
        clearTable();

    int nextLength = seqLength + 1;
    if (code < 256)
    {
        seqBuf[0] = (uchar)code;
        seqLength = 1;
    }
    else if (code < nextCode)
    {
        seqLength = table[code].length;
        int j = code;
        for (int i = seqLength - 1; i > 0; --i)
        {
            seqBuf[i] = table[j].tail;
            j = table[j].head;
        }
        seqBuf[0] = j;
    }
    else if (code == nextCode)
    {
        seqBuf[seqLength] = newChar;
        ++seqLength;
    }
    else
    {
        eof = true;
        return false;
    }

    newChar = seqBuf[0];
    if (first)
        first = false;
    else
    {
        table[nextCode].length = nextLength;
        table[nextCode].head = prevCode;
        table[nextCode].tail = newChar;
        ++nextCode;
        if (nextCode + early == 512)
            nextBits = 10;
        else if (nextCode + early == 1024)
            nextBits = 11;
        else if (nextCode + early == 2048)
            nextBits = 12;
    }

    prevCode = code;
    seqIndex = 0;
    return true;
}

XWRunLengthStream::XWRunLengthStream(XWStream *strA)
    :XWFilterStream(strA)
{
    bufPtr = bufEnd = buf;
    eof = false;
}

XWRunLengthStream::~XWRunLengthStream()
{
    if (str)
        delete str;
}

int XWRunLengthStream::getBlock(char *blk, int size)
{
	int n, m;

  n = 0;
  while (n < size) {
    if (bufPtr >= bufEnd) {
      if (!fillBuf()) {
	break;
      }
    }
    m = (int)(bufEnd - bufPtr);
    if (m > size - n) {
      m = size - n;
    }
    memcpy(blk + n, bufPtr, m);
    bufPtr += m;
    n += m;
  }
  return n;
}

XWString * XWRunLengthStream::getPSFilter(int psLevel, const char *indent)
{
    XWString *s = 0;
    if (psLevel < 2)
        return s;

    if (!(s = str->getPSFilter(psLevel, indent)))
        return 0;

    s->append(indent)->append("/RunLengthDecode filter\n");
    return s;
}

bool XWRunLengthStream::isBinary(bool)
{
    return str->isBinary(true);
}

void XWRunLengthStream::reset()
{
    str->reset();
    bufPtr = bufEnd = buf;
    eof = false;
}

bool XWRunLengthStream::fillBuf()
{
    if (eof)
        return false;

    int c = str->getChar();
    if (c == 0x80 || c == EOF)
    {
        eof = true;
        return false;
    }

    int n = 0;
    if (c < 0x80)
    {
        n = c + 1;
        for (int i = 0; i < n; ++i)
            buf[i] = (char)str->getChar();
    }
    else
    {
        n = 0x101 - c;
        c = str->getChar();
        for (int i = 0; i < n; ++i)
            buf[i] = (char)c;
    }

    bufPtr = buf;
    bufEnd = buf + n;
    return true;
}

XWCCITTFaxStream::XWCCITTFaxStream(XWStream *strA,
                                   int encodingA,
                                   bool endOfLineA,
                                   bool byteAlignA,
                                   int columnsA,
                                   int rowsA,
                                   bool endOfBlockA,
                                   bool blackA)
    :XWFilterStream(strA)
{
  encoding = encodingA;
  endOfLine = endOfLineA;
  byteAlign = byteAlignA;
  columns = columnsA;
  if (columns < 1) {
    columns = 1;
  } else if (columns > INT_MAX - 2) {
    columns = INT_MAX - 2;
  }
  rows = rowsA;
  endOfBlock = endOfBlockA;
  black = blackA;
  // 0 <= codingLine[0] < codingLine[1] < ... < codingLine[n] = columns
  // ---> max codingLine size = columns + 1
  // refLine has one extra guard entry at the end
  // ---> max refLine size = columns + 2
  codingLine = (int *)malloc((columns + 1) * sizeof(int));
  refLine = (int *)malloc((columns + 2) * sizeof(int));

  eof = false;
  row = 0;
  nextLine2D = encoding < 0;
  inputBits = 0;
  codingLine[0] = columns;
  a0i = 0;
  outputBits = 0;

  buf = EOF;
}

XWCCITTFaxStream::~XWCCITTFaxStream()
{
    if (str)
        delete str;

    if (refLine)
        free(refLine);

    if (codingLine)
        free(codingLine);
}

XWString * XWCCITTFaxStream::getPSFilter(int psLevel, const char *indent)
{
  XWString *s;
  char s1[50];

  if (psLevel < 2) {
    return NULL;
  }
  if (!(s = str->getPSFilter(psLevel, indent))) {
    return NULL;
  }
  s->append(indent)->append("<< ");
  if (encoding != 0) {
    sprintf(s1, "/K %d ", encoding);
    s->append(s1);
  }
  if (endOfLine) {
    s->append("/EndOfLine true ");
  }
  if (byteAlign) {
    s->append("/EncodedByteAlign true ");
  }
  sprintf(s1, "/Columns %d ", columns);
  s->append(s1);
  if (rows != 0) {
    sprintf(s1, "/Rows %d ", rows);
    s->append(s1);
  }
  if (!endOfBlock) {
    s->append("/EndOfBlock false ");
  }
  if (black) {
    s->append("/BlackIs1 true ");
  }
  s->append(">> /CCITTFaxDecode filter\n");
  return s;
}

bool XWCCITTFaxStream::isBinary(bool)
{
    return str->isBinary(true);
}

int XWCCITTFaxStream::lookChar()
{
  int code1, code2, code3;
  int b1i, blackPixels, i, bits;
  bool gotEOL;

  if (buf != EOF) {
    return buf;
  }

  // read the next row
  if (outputBits == 0) {

    // if at eof just return EOF
    if (eof) {
      return EOF;
    }

    err = false;

    // 2-D encoding
    if (nextLine2D) {
      for (i = 0; codingLine[i] < columns; ++i) {
	refLine[i] = codingLine[i];
      }
      refLine[i++] = columns;
      refLine[i] = columns;
      codingLine[0] = 0;
      a0i = 0;
      b1i = 0;
      blackPixels = 0;
      // invariant:
      // refLine[b1i-1] <= codingLine[a0i] < refLine[b1i] < refLine[b1i+1]
      //                                                             <= columns
      // exception at left edge:
      //   codingLine[a0i = 0] = refLine[b1i = 0] = 0 is possible
      // exception at right edge:
      //   refLine[b1i] = refLine[b1i+1] = columns is possible
      while (codingLine[a0i] < columns) {
	code1 = getTwoDimCode();
	switch (code1) {
	case twoDimPass:
	  addPixels(refLine[b1i + 1], blackPixels);
	  if (refLine[b1i + 1] < columns) {
	    b1i += 2;
	  }
	  break;
	case twoDimHoriz:
	  code1 = code2 = 0;
	  if (blackPixels) {
	    do {
	      code1 += code3 = getBlackCode();
	    } while (code3 >= 64);
	    do {
	      code2 += code3 = getWhiteCode();
	    } while (code3 >= 64);
	  } else {
	    do {
	      code1 += code3 = getWhiteCode();
	    } while (code3 >= 64);
	    do {
	      code2 += code3 = getBlackCode();
	    } while (code3 >= 64);
	  }
	  addPixels(codingLine[a0i] + code1, blackPixels);
	  if (codingLine[a0i] < columns) {
	    addPixels(codingLine[a0i] + code2, blackPixels ^ 1);
	  }
	  while (refLine[b1i] <= codingLine[a0i] && refLine[b1i] < columns) {
	    b1i += 2;
	  }
	  break;
	case twoDimVertR3:
	  addPixels(refLine[b1i] + 3, blackPixels);
	  blackPixels ^= 1;
	  if (codingLine[a0i] < columns) {
	    ++b1i;
	    while (refLine[b1i] <= codingLine[a0i] && refLine[b1i] < columns) {
	      b1i += 2;
	    }
	  }
	  break;
	case twoDimVertR2:
	  addPixels(refLine[b1i] + 2, blackPixels);
	  blackPixels ^= 1;
	  if (codingLine[a0i] < columns) {
	    ++b1i;
	    while (refLine[b1i] <= codingLine[a0i] && refLine[b1i] < columns) {
	      b1i += 2;
	    }
	  }
	  break;
	case twoDimVertR1:
	  addPixels(refLine[b1i] + 1, blackPixels);
	  blackPixels ^= 1;
	  if (codingLine[a0i] < columns) {
	    ++b1i;
	    while (refLine[b1i] <= codingLine[a0i] && refLine[b1i] < columns) {
	      b1i += 2;
	    }
	  }
	  break;
	case twoDimVert0:
	  addPixels(refLine[b1i], blackPixels);
	  blackPixels ^= 1;
	  if (codingLine[a0i] < columns) {
	    ++b1i;
	    while (refLine[b1i] <= codingLine[a0i] && refLine[b1i] < columns) {
	      b1i += 2;
	    }
	  }
	  break;
	case twoDimVertL3:
	  addPixelsNeg(refLine[b1i] - 3, blackPixels);
	  blackPixels ^= 1;
	  if (codingLine[a0i] < columns) {
	    if (b1i > 0) {
	      --b1i;
	    } else {
	      ++b1i;
	    }
	    while (refLine[b1i] <= codingLine[a0i] && refLine[b1i] < columns) {
	      b1i += 2;
	    }
	  }
	  break;
	case twoDimVertL2:
	  addPixelsNeg(refLine[b1i] - 2, blackPixels);
	  blackPixels ^= 1;
	  if (codingLine[a0i] < columns) {
	    if (b1i > 0) {
	      --b1i;
	    } else {
	      ++b1i;
	    }
	    while (refLine[b1i] <= codingLine[a0i] && refLine[b1i] < columns) {
	      b1i += 2;
	    }
	  }
	  break;
	case twoDimVertL1:
	  addPixelsNeg(refLine[b1i] - 1, blackPixels);
	  blackPixels ^= 1;
	  if (codingLine[a0i] < columns) {
	    if (b1i > 0) {
	      --b1i;
	    } else {
	      ++b1i;
	    }
	    while (refLine[b1i] <= codingLine[a0i] && refLine[b1i] < columns) {
	      b1i += 2;
	    }
	  }
	  break;
	case EOF:
	  addPixels(columns, 0);
	  eof = true;
	  break;
	default:
	  addPixels(columns, 0);
	  err = true;
	  break;
	}
      }

    // 1-D encoding
    } else {
      codingLine[0] = 0;
      a0i = 0;
      blackPixels = 0;
      while (codingLine[a0i] < columns) {
	code1 = 0;
	if (blackPixels) {
	  do {
	    code1 += code3 = getBlackCode();
	  } while (code3 >= 64);
	} else {
	  do {
	    code1 += code3 = getWhiteCode();
	  } while (code3 >= 64);
	}
	addPixels(codingLine[a0i] + code1, blackPixels);
	blackPixels ^= 1;
      }
    }

    // check for end-of-line marker, skipping over any extra zero bits
    // (if EncodedByteAlign is true and EndOfLine is false, there can
    // be "false" EOL markers -- i.e., if the last n unused bits in
    // row i are set to zero, and the first 11-n bits in row i+1
    // happen to be zero -- so we don't look for EOL markers in this
    // case)
    gotEOL = false;
    if (!endOfBlock && row == rows - 1) {
      eof = true;
    } else if (endOfLine || !byteAlign) {
      code1 = lookBits(12);
      if (endOfLine) {
	while (code1 != EOF && code1 != 0x001) {
	  eatBits(1);
	  code1 = lookBits(12);
	}
      } else {
	while (code1 == 0) {
	  eatBits(1);
	  code1 = lookBits(12);
	}
      }
      if (code1 == 0x001) {
	eatBits(12);
	gotEOL = true;
      }
    }

    // byte-align the row
    // (Adobe apparently doesn't do byte alignment after EOL markers
    // -- I've seen CCITT image data streams in two different formats,
    // both with the byteAlign flag set:
    //   1. xx:x0:01:yy:yy
    //   2. xx:00:1y:yy:yy
    // where xx is the previous line, yy is the next line, and colons
    // separate bytes.)
    if (byteAlign && !gotEOL) {
      inputBits &= ~7;
    }

    // check for end of stream
    if (lookBits(1) == EOF) {
      eof = true;
    }

    // get 2D encoding tag
    if (!eof && encoding > 0) {
      nextLine2D = !lookBits(1);
      eatBits(1);
    }

    // check for end-of-block marker
    if (endOfBlock && !endOfLine && byteAlign) {
      // in this case, we didn't check for an EOL code above, so we
      // need to check here
      code1 = lookBits(24);
      if (code1 == 0x001001) {
	eatBits(12);
	gotEOL = true;
      }
    }
    if (endOfBlock && gotEOL) {
      code1 = lookBits(12);
      if (code1 == 0x001) {
	eatBits(12);
	if (encoding > 0) {
	  lookBits(1);
	  eatBits(1);
	}
	if (encoding >= 0) {
	  for (i = 0; i < 4; ++i) {
	    code1 = lookBits(12);
	    if (code1 != 0x001) {
	      xwApp->error("Bad RTC code in CCITTFax stream");
	    }
	    eatBits(12);
	    if (encoding > 0) {
	      lookBits(1);
	      eatBits(1);
	    }
	  }
	}
	eof = true;
      }

    // look for an end-of-line marker after an error -- we only do
    // this if we know the stream contains end-of-line markers because
    // the "just plow on" technique tends to work better otherwise
    } else if (err && endOfLine) {
      while (1) {
	code1 = lookBits(13);
	if (code1 == EOF) {
	  eof = true;
	  return EOF;
	}
	if ((code1 >> 1) == 0x001) {
	  break;
	}
	eatBits(1);
      }
      eatBits(12); 
      if (encoding > 0) {
	eatBits(1);
	nextLine2D = !(code1 & 1);
      }
    }

    // set up for output
    if (codingLine[0] > 0) {
      outputBits = codingLine[a0i = 0];
    } else {
      outputBits = codingLine[a0i = 1];
    }

    ++row;
  }

  // get a byte
  if (outputBits >= 8) {
    buf = (a0i & 1) ? 0x00 : 0xff;
    outputBits -= 8;
    if (outputBits == 0 && codingLine[a0i] < columns) {
      ++a0i;
      outputBits = codingLine[a0i] - codingLine[a0i - 1];
    }
  } else {
    bits = 8;
    buf = 0;
    do {
      if (outputBits > bits) {
	buf <<= bits;
	if (!(a0i & 1)) {
	  buf |= 0xff >> (8 - bits);
	}
	outputBits -= bits;
	bits = 0;
      } else {
	buf <<= outputBits;
	if (!(a0i & 1)) {
	  buf |= 0xff >> (8 - outputBits);
	}
	bits -= outputBits;
	outputBits = 0;
	if (codingLine[a0i] < columns) {
	  ++a0i;
	  outputBits = codingLine[a0i] - codingLine[a0i - 1];
	} else if (bits > 0) {
	  buf <<= bits;
	  bits = 0;
	}
      }
    } while (bits);
  }
  if (black) {
    buf ^= 0xff;
  }
  return buf;
}

void XWCCITTFaxStream::reset()
{
  int code1;

  str->reset();
  eof = false;
  row = 0;
  nextLine2D = encoding < 0;
  inputBits = 0;
  codingLine[0] = columns;
  a0i = 0;
  outputBits = 0;
  buf = EOF;

  // skip any initial zero bits and end-of-line marker, and get the 2D
  // encoding tag
  while ((code1 = lookBits(12)) == 0) {
    eatBits(1);
  }
  if (code1 == 0x001) {
    eatBits(12);
    endOfLine = true;
  }
  if (encoding > 0) {
    nextLine2D = !lookBits(1);
    eatBits(1);
  }
}

inline void XWCCITTFaxStream::addPixels(int a1, int blackPixels)
{
	if (a1 > codingLine[a0i]) {
    if (a1 > columns) {
      err = true;
      a1 = columns;
    }
    if ((a0i & 1) ^ blackPixels) {
      ++a0i;
    }
    codingLine[a0i] = a1;
  }
}

inline void XWCCITTFaxStream::addPixelsNeg(int a1, int blackPixels)
{
	if (a1 > codingLine[a0i]) {
    if (a1 > columns) {
      err = true;
      a1 = columns;
    }
    if ((a0i & 1) ^ blackPixels) {
      ++a0i;
    }
    codingLine[a0i] = a1;
  } else if (a1 < codingLine[a0i]) {
    if (a1 < 0) {
      err = true;
      a1 = 0;
    }
    while (a0i > 0 && a1 <= codingLine[a0i - 1]) {
      --a0i;
    }
    codingLine[a0i] = a1;
  }
}

short XWCCITTFaxStream::getBlackCode()
{
  short code;
  CCITTCode *p;
  int n;

  code = 0; // make gcc happy
  if (endOfBlock) {
    code = lookBits(13);
    if (code == EOF) {
      return 1;
    }
    if ((code >> 7) == 0) {
      p = &blackTab1[code];
    } else if ((code >> 9) == 0 && (code >> 7) != 0) {
      p = &blackTab2[(code >> 1) - 64];
    } else {
      p = &blackTab3[code >> 7];
    }
    if (p->bits > 0) {
      eatBits(p->bits);
      return p->n;
    }
  } else {
    for (n = 2; n <= 6; ++n) {
      code = lookBits(n);
      if (code == EOF) {
	return 1;
      }
      if (n < 6) {
	code <<= 6 - n;
      }
      p = &blackTab3[code];
      if (p->bits == n) {
	eatBits(n);
	return p->n;
      }
    }
    for (n = 7; n <= 12; ++n) {
      code = lookBits(n);
      if (code == EOF) {
	return 1;
      }
      if (n < 12) {
	code <<= 12 - n;
      }
      if (code >= 64) {
	p = &blackTab2[code - 64];
	if (p->bits == n) {
	  eatBits(n);
	  return p->n;
	}
      }
    }
    for (n = 10; n <= 13; ++n) {
      code = lookBits(n);
      if (code == EOF) {
	return 1;
      }
      if (n < 13) {
	code <<= 13 - n;
      }
      p = &blackTab1[code];
      if (p->bits == n) {
	eatBits(n);
	return p->n;
      }
    }
  }
  
  eatBits(1);
  return 1;
}

short XWCCITTFaxStream::getTwoDimCode()
{
  int code;
  CCITTCode *p;
  int n;

  code = 0; // make gcc happy
  if (endOfBlock) {
    if ((code = lookBits(7)) != EOF) {
      p = &twoDimTab1[code];
      if (p->bits > 0) {
	eatBits(p->bits);
	return p->n;
      }
    }
  } else {
    for (n = 1; n <= 7; ++n) {
      if ((code = lookBits(n)) == EOF) {
	break;
      }
      if (n < 7) {
	code <<= 7 - n;
      }
      p = &twoDimTab1[code];
      if (p->bits == n) {
	eatBits(n);
	return p->n;
      }
    }
  }
  
  return EOF;
}

short XWCCITTFaxStream::getWhiteCode()
{
  short code;
  CCITTCode *p;
  int n;

  code = 0; // make gcc happy
  if (endOfBlock) {
    code = lookBits(12);
    if (code == EOF) {
      return 1;
    }
    if ((code >> 5) == 0) {
      p = &whiteTab1[code];
    } else {
      p = &whiteTab2[code >> 3];
    }
    if (p->bits > 0) {
      eatBits(p->bits);
      return p->n;
    }
  } else {
    for (n = 1; n <= 9; ++n) {
      code = lookBits(n);
      if (code == EOF) {
	return 1;
      }
      if (n < 9) {
	code <<= 9 - n;
      }
      p = &whiteTab2[code];
      if (p->bits == n) {
	eatBits(n);
	return p->n;
      }
    }
    for (n = 11; n <= 12; ++n) {
      code = lookBits(n);
      if (code == EOF) {
	return 1;
      }
      if (n < 12) {
	code <<= 12 - n;
      }
      p = &whiteTab1[code];
      if (p->bits == n) {
	eatBits(n);
	return p->n;
      }
    }
  }
  
  eatBits(1);
  return 1;
}

short XWCCITTFaxStream::lookBits(int n)
{
 int c;

  while (inputBits < n) {
    if ((c = str->getChar()) == EOF) {
      if (inputBits == 0) {
	return EOF;
      }
      // near the end of the stream, the caller may ask for more bits
      // than are available, but there may still be a valid code in
      // however many bits are available -- we need to return correct
      // data in this case
      return (inputBuf << (n - inputBits)) & (0xffffffff >> (32 - n));
    }
    inputBuf = (inputBuf << 8) + c;
    inputBits += 8;
  }
  return (inputBuf >> (inputBits - n)) & (0xffffffff >> (32 - n));
}


// IDCT constants (20.12 fixed point format)
#define dctCos1    4017     // cos(pi/16)
#define dctSin1     799     // sin(pi/16)
#define dctCos3    3406     // cos(3*pi/16)
#define dctSin3    2276     // sin(3*pi/16)
#define dctCos6    1567     // cos(6*pi/16)
#define dctSin6    3784     // sin(6*pi/16)
#define dctSqrt2   5793     // sqrt(2)
#define dctSqrt1d2 2896     // sqrt(2) / 2

// color conversion parameters (16.16 fixed point format)
#define dctCrToR   91881    //  1.4020
#define dctCbToG  -22553    // -0.3441363
#define dctCrToG  -46802    // -0.71413636
#define dctCbToB  116130    //  1.772

// clip [-256,511] --> [0,255]
#define dctClipOffset 256
static uchar dctClip[768];
static int dctClipInit = 0;
// zig zag decode map
static int dctZigZag[64] = {
   0,
   1,  8,
  16,  9,  2,
   3, 10, 17, 24,
  32, 25, 18, 11, 4,
   5, 12, 19, 26, 33, 40,
  48, 41, 34, 27, 20, 13,  6,
   7, 14, 21, 28, 35, 42, 49, 56,
  57, 50, 43, 36, 29, 22, 15,
  23, 30, 37, 44, 51, 58,
  59, 52, 45, 38, 31,
  39, 46, 53, 60,
  61, 54, 47,
  55, 62,
  63
};


XWDCTStream::XWDCTStream(XWStream *strA, 
	                     int colorXformA)
    :XWFilterStream(strA)
{
  int i, j;

  colorXform = colorXformA;
  progressive = interleaved = false;
  width = height = 0;
  mcuWidth = mcuHeight = 0;
  numComps = 0;
  comp = 0;
  x = y = dy = 0;
  for (i = 0; i < 4; ++i) {
    for (j = 0; j < 32; ++j) {
      rowBuf[i][j] = NULL;
    }
    frameBuf[i] = NULL;
  }

  if (!dctClipInit) {
    for (i = -256; i < 0; ++i)
      dctClip[dctClipOffset + i] = 0;
    for (i = 0; i < 256; ++i)
      dctClip[dctClipOffset + i] = i;
    for (i = 256; i < 512; ++i)
      dctClip[dctClipOffset + i] = 255;
    dctClipInit = 1;
  }
}

XWDCTStream::~XWDCTStream()
{
    close();
    if (str)
        delete str;
}

void XWDCTStream::close()
{
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 32; ++j)
        {
            if (rowBuf[i][j])
                delete [] rowBuf[i][j];
            rowBuf[i][j] = 0;
        }
        if (frameBuf[i])
            delete [] frameBuf[i];

        frameBuf[i] = 0;
    }
    XWFilterStream::close();
}

int XWDCTStream::getChar()
{
  int c;

  if (y >= height) {
    return EOF;
  }
  if (progressive || !interleaved) {
    c = frameBuf[comp][y * bufWidth + x];
    if (++comp == numComps) {
      comp = 0;
      if (++x == width) {
	x = 0;
	++y;
      }
    }
  } else {
    if (dy >= mcuHeight) {
      if (!readMCURow()) {
	y = height;
	return EOF;
      }
      comp = 0;
      x = 0;
      dy = 0;
    }
    c = rowBuf[comp][dy][x];
    if (++comp == numComps) {
      comp = 0;
      if (++x == width) {
	x = 0;
	++y;
	++dy;
	if (y == height) {
	  readTrailer();
	}
      }
    }
  }
  return c;
}

XWString * XWDCTStream::getPSFilter(int psLevel, const char *indent)
{
    XWString *s = 0;
    if (psLevel < 2)
        return 0;

    if (!(s = str->getPSFilter(psLevel, indent)))
        return 0;

    s->append(indent)->append("<< >> /DCTDecode filter\n");
    return s;
}

bool XWDCTStream::isBinary(bool)
{
    return str->isBinary(true);
}

int XWDCTStream::lookChar()
{
  if (y >= height) {
    return EOF;
  }
  if (progressive || !interleaved) {
    return frameBuf[comp][y * bufWidth + x];
  } else {
    if (dy >= mcuHeight) {
      if (!readMCURow()) {
	y = height;
	return EOF;
      }
      comp = 0;
      x = 0;
      dy = 0;
    }
    return rowBuf[comp][dy][x];
  }
}

void XWDCTStream::reset()
{
  int i, j;

  str->reset();

  progressive = interleaved = false;
  width = height = 0;
  numComps = 0;
  numQuantTables = 0;
  numDCHuffTables = 0;
  numACHuffTables = 0;
  gotJFIFMarker = false;
  gotAdobeMarker = false;
  restartInterval = 0;

  if (!readHeader()) {
    y = height;
    return;
  }

  // compute MCU size
  if (numComps == 1) {
    compInfo[0].hSample = compInfo[0].vSample = 1;
  }
  mcuWidth = compInfo[0].hSample;
  mcuHeight = compInfo[0].vSample;
  for (i = 1; i < numComps; ++i) {
    if (compInfo[i].hSample > mcuWidth) {
      mcuWidth = compInfo[i].hSample;
    }
    if (compInfo[i].vSample > mcuHeight) {
      mcuHeight = compInfo[i].vSample;
    }
  }
  mcuWidth *= 8;
  mcuHeight *= 8;

  // figure out color transform
  if (colorXform == -1) {
    if (numComps == 3) {
      if (gotJFIFMarker) {
	colorXform = 1;
      } else if (compInfo[0].id == 82 && compInfo[1].id == 71 &&
		 compInfo[2].id == 66) { // ASCII "RGB"
	colorXform = 0;
      } else {
	colorXform = 1;
      }
    } else {
      colorXform = 0;
    }
  }

  if (progressive || !interleaved) {

    // allocate a buffer for the whole image
    bufWidth = ((width + mcuWidth - 1) / mcuWidth) * mcuWidth;
    bufHeight = ((height + mcuHeight - 1) / mcuHeight) * mcuHeight;
    if (bufWidth <= 0 || bufHeight <= 0 ||
	bufWidth > INT_MAX / bufWidth / (int)sizeof(int)) {
      y = height;
      return;
    }
    for (i = 0; i < numComps; ++i) {
      frameBuf[i] = (int *)malloc(bufWidth * bufHeight * sizeof(int));
      memset(frameBuf[i], 0, bufWidth * bufHeight * sizeof(int));
    }

    // read the image data
    do {
      restartMarker = 0xd0;
      restart();
      readScan();
    } while (readHeader());

    // decode
    decodeImage();

    // initialize counters
    comp = 0;
    x = 0;
    y = 0;

  } else {

    // allocate a buffer for one row of MCUs
    bufWidth = ((width + mcuWidth - 1) / mcuWidth) * mcuWidth;
    for (i = 0; i < numComps; ++i) {
      for (j = 0; j < mcuHeight; ++j) {
	rowBuf[i][j] = (uchar *)malloc(bufWidth * sizeof(uchar));
      }
    }

    // initialize counters
    comp = 0;
    x = 0;
    y = 0;
    dy = mcuHeight;

    restartMarker = 0xd0;
    restart();
  }
}

void XWDCTStream::decodeImage()
{
  int dataIn[64];
  uchar dataOut[64];
  ushort *quantTable;
  int pY, pCb, pCr, pR, pG, pB;
  int x1, y1, x2, y2, x3, y3, x4, y4, x5, y5, cc, i;
  int h, v, horiz, vert, hSub, vSub;
  int *p0, *p1, *p2;

  for (y1 = 0; y1 < bufHeight; y1 += mcuHeight) {
    for (x1 = 0; x1 < bufWidth; x1 += mcuWidth) {
      for (cc = 0; cc < numComps; ++cc) {
	quantTable = quantTables[compInfo[cc].quantTable];
	h = compInfo[cc].hSample;
	v = compInfo[cc].vSample;
	horiz = mcuWidth / h;
	vert = mcuHeight / v;
	hSub = horiz / 8;
	vSub = vert / 8;
	for (y2 = 0; y2 < mcuHeight; y2 += vert) {
	  for (x2 = 0; x2 < mcuWidth; x2 += horiz) {

	    // pull out the coded data unit
	    p1 = &frameBuf[cc][(y1+y2) * bufWidth + (x1+x2)];
	    for (y3 = 0, i = 0; y3 < 8; ++y3, i += 8) {
	      dataIn[i]   = p1[0];
	      dataIn[i+1] = p1[1];
	      dataIn[i+2] = p1[2];
	      dataIn[i+3] = p1[3];
	      dataIn[i+4] = p1[4];
	      dataIn[i+5] = p1[5];
	      dataIn[i+6] = p1[6];
	      dataIn[i+7] = p1[7];
	      p1 += bufWidth * vSub;
	    }

	    // transform
	    transformDataUnit(quantTable, dataIn, dataOut);

	    // store back into frameBuf, doing replication for
	    // subsampled components
	    p1 = &frameBuf[cc][(y1+y2) * bufWidth + (x1+x2)];
	    if (hSub == 1 && vSub == 1) {
	      for (y3 = 0, i = 0; y3 < 8; ++y3, i += 8) {
		p1[0] = dataOut[i] & 0xff;
		p1[1] = dataOut[i+1] & 0xff;
		p1[2] = dataOut[i+2] & 0xff;
		p1[3] = dataOut[i+3] & 0xff;
		p1[4] = dataOut[i+4] & 0xff;
		p1[5] = dataOut[i+5] & 0xff;
		p1[6] = dataOut[i+6] & 0xff;
		p1[7] = dataOut[i+7] & 0xff;
		p1 += bufWidth;
	      }
	    } else if (hSub == 2 && vSub == 2) {
	      p2 = p1 + bufWidth;
	      for (y3 = 0, i = 0; y3 < 16; y3 += 2, i += 8) {
		p1[0] = p1[1] = p2[0] = p2[1] = dataOut[i] & 0xff;
		p1[2] = p1[3] = p2[2] = p2[3] = dataOut[i+1] & 0xff;
		p1[4] = p1[5] = p2[4] = p2[5] = dataOut[i+2] & 0xff;
		p1[6] = p1[7] = p2[6] = p2[7] = dataOut[i+3] & 0xff;
		p1[8] = p1[9] = p2[8] = p2[9] = dataOut[i+4] & 0xff;
		p1[10] = p1[11] = p2[10] = p2[11] = dataOut[i+5] & 0xff;
		p1[12] = p1[13] = p2[12] = p2[13] = dataOut[i+6] & 0xff;
		p1[14] = p1[15] = p2[14] = p2[15] = dataOut[i+7] & 0xff;
		p1 += bufWidth * 2;
		p2 += bufWidth * 2;
	      }
	    } else {
	      i = 0;
	      for (y3 = 0, y4 = 0; y3 < 8; ++y3, y4 += vSub) {
		for (x3 = 0, x4 = 0; x3 < 8; ++x3, x4 += hSub) {
		  p2 = p1 + x4;
		  for (y5 = 0; y5 < vSub; ++y5) {
		    for (x5 = 0; x5 < hSub; ++x5) {
		      p2[x5] = dataOut[i] & 0xff;
		    }
		    p2 += bufWidth;
		  }
		  ++i;
		}
		p1 += bufWidth * vSub;
	      }
	    }
	  }
	}
      }

      // color space conversion
      if (colorXform) {
	// convert YCbCr to RGB
	if (numComps == 3) {
	  for (y2 = 0; y2 < mcuHeight; ++y2) {
	    p0 = &frameBuf[0][(y1+y2) * bufWidth + x1];
	    p1 = &frameBuf[1][(y1+y2) * bufWidth + x1];
	    p2 = &frameBuf[2][(y1+y2) * bufWidth + x1];
	    for (x2 = 0; x2 < mcuWidth; ++x2) {
	      pY = *p0;
	      pCb = *p1 - 128;
	      pCr = *p2 - 128;
	      pR = ((pY << 16) + dctCrToR * pCr + 32768) >> 16;
	      *p0++ = dctClip[dctClipOffset + pR];
	      pG = ((pY << 16) + dctCbToG * pCb + dctCrToG * pCr +
		    32768) >> 16;
	      *p1++ = dctClip[dctClipOffset + pG];
	      pB = ((pY << 16) + dctCbToB * pCb + 32768) >> 16;
	      *p2++ = dctClip[dctClipOffset + pB];
	    }
	  }
	// convert YCbCrK to CMYK (K is passed through unchanged)
	} else if (numComps == 4) {
	  for (y2 = 0; y2 < mcuHeight; ++y2) {
	    p0 = &frameBuf[0][(y1+y2) * bufWidth + x1];
	    p1 = &frameBuf[1][(y1+y2) * bufWidth + x1];
	    p2 = &frameBuf[2][(y1+y2) * bufWidth + x1];
	    for (x2 = 0; x2 < mcuWidth; ++x2) {
	      pY = *p0;
	      pCb = *p1 - 128;
	      pCr = *p2 - 128;
	      pR = ((pY << 16) + dctCrToR * pCr + 32768) >> 16;
	      *p0++ = 255 - dctClip[dctClipOffset + pR];
	      pG = ((pY << 16) + dctCbToG * pCb + dctCrToG * pCr +
		    32768) >> 16;
	      *p1++ = 255 - dctClip[dctClipOffset + pG];
	      pB = ((pY << 16) + dctCbToB * pCb + 32768) >> 16;
	      *p2++ = 255 - dctClip[dctClipOffset + pB];
	    }
	  }
	}
      }
    }
  }
}

bool XWDCTStream::readAdobeMarker()
{
    int length = read16();
    if (length < 14)
        return false;

    char buf[12];
    int c = 0;
    for (int i = 0; i < 12; ++i)
    {
        if ((c = str->getChar()) == EOF)
            return false;
        buf[i] = c;
    }
    
    if (strncmp(buf, "Adobe", 5))
    	return false;

    colorXform = buf[11];
    gotAdobeMarker = true;
    for (int i = 14; i < length; ++i)
    {
        if (str->getChar() == EOF)
            return false;
    }

    return true;
}

int  XWDCTStream::readAmp(int size)
{
    int amp = 0;
    int bit = 0;
    for (int bits = 0; bits < size; ++bits)
    {
        if ((bit = readBit()) == EOF)
            return 9999;
        amp = (amp << 1) + bit;
    }

    if (amp < (1 << (size - 1)))
        amp -= (1 << size) - 1;
    return amp;
}

bool XWDCTStream::readBaselineSOF()
{
  int length;
  int prec;
  int i;
  int c;

  length = read16();
  prec = str->getChar();
  height = read16();
  width = read16();
  numComps = str->getChar();
  if (numComps <= 0 || numComps > 4) {
    numComps = 0;
    return false;
  }
  if (prec != 8) {
    return false;
  }
  for (i = 0; i < numComps; ++i) {
    compInfo[i].id = str->getChar();
    c = str->getChar();
    compInfo[i].hSample = (c >> 4) & 0x0f;
    compInfo[i].vSample = c & 0x0f;
    compInfo[i].quantTable = str->getChar();
    if (compInfo[i].hSample < 1 || compInfo[i].hSample > 4 ||
	compInfo[i].vSample < 1 || compInfo[i].vSample > 4) {
      return false;
    }
    if (compInfo[i].quantTable < 0 || compInfo[i].quantTable > 3) {
      return false;
    }
  }
  progressive = false;
  return true;
}

int XWDCTStream::readBit()
{
  int bit;
  int c, c2;

  if (inputBits == 0) {
    if ((c = str->getChar()) == EOF)
      return EOF;
    if (c == 0xff) {
      do {
	c2 = str->getChar();
      } while (c2 == 0xff);
      if (c2 != 0x00) {
	return EOF;
      }
    }
    inputBuf = c;
    inputBits = 8;
  }
  bit = (inputBuf >> (inputBits - 1)) & 1;
  --inputBits;
  return bit;
}

bool XWDCTStream::readDataUnit(DCTHuffTable *dcHuffTable,
                                  DCTHuffTable *acHuffTable,
                                  int *prevDC, int data[64])
{
  int run, size, amp;
  int c;
  int i, j;

  if ((size = readHuffSym(dcHuffTable)) == 9999) {
    return false;
  }
  if (size > 0) {
    if ((amp = readAmp(size)) == 9999) {
      return false;
    }
  } else {
    amp = 0;
  }
  data[0] = *prevDC += amp;
  for (i = 1; i < 64; ++i) {
    data[i] = 0;
  }
  i = 1;
  while (i < 64) {
    run = 0;
    while ((c = readHuffSym(acHuffTable)) == 0xf0 && run < 0x30) {
      run += 0x10;
    }
    if (c == 9999) {
      return false;
    }
    if (c == 0x00) {
      break;
    } else {
      run += (c >> 4) & 0x0f;
      size = c & 0x0f;
      amp = readAmp(size);
      if (amp == 9999) {
	return false;
      }
      i += run;
      if (i < 64) {
	j = dctZigZag[i++];
	data[j] = amp;
      }
    }
  }
  return true;
}

bool XWDCTStream::readHeader()
{
  bool doScan;
  int n;
  int c = 0;
  int i;

  // read headers
  doScan = false;
  while (!doScan) {
    c = readMarker();
    switch (c) {
    case 0xc0:			// SOF0 (sequential)
    case 0xc1:			// SOF1 (extended sequential)
      if (!readBaselineSOF()) {
	return false;
      }
      break;
    case 0xc2:			// SOF2 (progressive)
      if (!readProgressiveSOF()) {
	return false;
      }
      break;
    case 0xc4:			// DHT
      if (!readHuffmanTables()) {
	return false;
      }
      break;
    case 0xd8:			// SOI
      break;
    case 0xd9:			// EOI
      return false;
    case 0xda:			// SOS
      if (!readScanInfo()) {
	return false;
      }
      doScan = true;
      break;
    case 0xdb:			// DQT
      if (!readQuantTables()) {
	return false;
      }
      break;
    case 0xdd:			// DRI
      if (!readRestartInterval()) {
	return false;
      }
      break;
    case 0xe0:			// APP0
      if (!readJFIFMarker()) {
	return false;
      }
      break;
    case 0xee:			// APP14
      if (!readAdobeMarker()) {
	return false;
      }
      break;
    case EOF:
      return false;
    default:
      // skip APPn / COM / etc.
      if (c >= 0xe0) {
	n = read16() - 2;
	for (i = 0; i < n; ++i) {
	  str->getChar();
	}
      } else {
	return false;
      }
      break;
    }
  }

  return true;
}

bool XWDCTStream::readHuffmanTables()
{
  DCTHuffTable *tbl;
  int length;
  int index;
  ushort code;
  uchar sym;
  int i;
  int c;

  length = read16() - 2;
  while (length > 0) {
    index = str->getChar();
    --length;
    if ((index & 0x0f) >= 4) {
      return false;
    }
    if (index & 0x10) {
      index &= 0x0f;
      if (index >= numACHuffTables)
	numACHuffTables = index+1;
      tbl = &acHuffTables[index];
    } else {
      index &= 0x0f;
      if (index >= numDCHuffTables)
	numDCHuffTables = index+1;
      tbl = &dcHuffTables[index];
    }
    sym = 0;
    code = 0;
    for (i = 1; i <= 16; ++i) {
      c = str->getChar();
      tbl->firstSym[i] = sym;
      tbl->firstCode[i] = code;
      tbl->numCodes[i] = c;
      sym += c;
      code = (code + c) << 1;
    }
    length -= 16;
    for (i = 0; i < sym; ++i)
      tbl->sym[i] = str->getChar();
    length -= sym;
  }
  return true;
}

int  XWDCTStream::readHuffSym(DCTHuffTable *table)
{
  ushort code;
  int bit;
  int codeBits;

  code = 0;
  codeBits = 0;
  do {
    // add a bit to the code
    if ((bit = readBit()) == EOF) {
      return 9999;
    }
    code = (code << 1) + bit;
    ++codeBits;

    // look up code
    if (code < table->firstCode[codeBits]) {
      break;
    }
    if (code - table->firstCode[codeBits] < table->numCodes[codeBits]) {
      code -= table->firstCode[codeBits];
      return table->sym[table->firstSym[codeBits] + code];
    }
  } while (codeBits < 16);

  return 9999;
}

bool XWDCTStream::readJFIFMarker()
{
    int length = read16();
    length -= 2;
    char buf[5];
    int c = 0;
    if (length >= 5)
    {
        for (int i = 0; i < 5; ++i)
        {
            if ((c = str->getChar()) == EOF)
                return false;

            buf[i] = c;
        }

        length -= 5;
        if (!memcmp(buf, "JFIF\0", 5))
            gotJFIFMarker = true;
    }

    while (length > 0)
    {
        if (str->getChar() == EOF)
            return false;

        --length;
    }

    return true;
}

int  XWDCTStream::readMarker()
{
    int c = 0;
    do
    {
        do
        {
            c = str->getChar();
        } while (c != 0xff && c != EOF);

        do
        {
            c = str->getChar();
        } while (c == 0xff);
    } while (c == 0x00);

    return c;
}

bool XWDCTStream::readMCURow()
{
  int data1[64];
  uchar data2[64];
  uchar *p1, *p2;
  int pY, pCb, pCr, pR, pG, pB;
  int h, v, horiz, vert, hSub, vSub;
  int x1, x2, y2, x3, y3, x4, y4, x5, y5, cc, i;
  int c;

  for (x1 = 0; x1 < width; x1 += mcuWidth) {

    // deal with restart marker
    if (restartInterval > 0 && restartCtr == 0) {
      c = readMarker();
      if (c != restartMarker) {
	return false;
      }
      if (++restartMarker == 0xd8)
	restartMarker = 0xd0;
      restart();
    }

    // read one MCU
    for (cc = 0; cc < numComps; ++cc) {
      h = compInfo[cc].hSample;
      v = compInfo[cc].vSample;
      horiz = mcuWidth / h;
      vert = mcuHeight / v;
      hSub = horiz / 8;
      vSub = vert / 8;
      for (y2 = 0; y2 < mcuHeight; y2 += vert) {
	for (x2 = 0; x2 < mcuWidth; x2 += horiz) {
	  if (!readDataUnit(&dcHuffTables[scanInfo.dcHuffTable[cc]],
			    &acHuffTables[scanInfo.acHuffTable[cc]],
			    &compInfo[cc].prevDC,
			    data1)) {
	    return false;
	  }
	  transformDataUnit(quantTables[compInfo[cc].quantTable],
			    data1, data2);
	  if (hSub == 1 && vSub == 1) {
	    for (y3 = 0, i = 0; y3 < 8; ++y3, i += 8) {
	      p1 = &rowBuf[cc][y2+y3][x1+x2];
	      p1[0] = data2[i];
	      p1[1] = data2[i+1];
	      p1[2] = data2[i+2];
	      p1[3] = data2[i+3];
	      p1[4] = data2[i+4];
	      p1[5] = data2[i+5];
	      p1[6] = data2[i+6];
	      p1[7] = data2[i+7];
	    }
	  } else if (hSub == 2 && vSub == 2) {
	    for (y3 = 0, i = 0; y3 < 16; y3 += 2, i += 8) {
	      p1 = &rowBuf[cc][y2+y3][x1+x2];
	      p2 = &rowBuf[cc][y2+y3+1][x1+x2];
	      p1[0] = p1[1] = p2[0] = p2[1] = data2[i];
	      p1[2] = p1[3] = p2[2] = p2[3] = data2[i+1];
	      p1[4] = p1[5] = p2[4] = p2[5] = data2[i+2];
	      p1[6] = p1[7] = p2[6] = p2[7] = data2[i+3];
	      p1[8] = p1[9] = p2[8] = p2[9] = data2[i+4];
	      p1[10] = p1[11] = p2[10] = p2[11] = data2[i+5];
	      p1[12] = p1[13] = p2[12] = p2[13] = data2[i+6];
	      p1[14] = p1[15] = p2[14] = p2[15] = data2[i+7];
	    }
	  } else {
	    i = 0;
	    for (y3 = 0, y4 = 0; y3 < 8; ++y3, y4 += vSub) {
	      for (x3 = 0, x4 = 0; x3 < 8; ++x3, x4 += hSub) {
		for (y5 = 0; y5 < vSub; ++y5)
		  for (x5 = 0; x5 < hSub; ++x5)
		    rowBuf[cc][y2+y4+y5][x1+x2+x4+x5] = data2[i];
		++i;
	      }
	    }
	  }
	}
      }
    }
    --restartCtr;

    // color space conversion
    if (colorXform) {
      // convert YCbCr to RGB
      if (numComps == 3) {
	for (y2 = 0; y2 < mcuHeight; ++y2) {
	  for (x2 = 0; x2 < mcuWidth; ++x2) {
	    pY = rowBuf[0][y2][x1+x2];
	    pCb = rowBuf[1][y2][x1+x2] - 128;
	    pCr = rowBuf[2][y2][x1+x2] - 128;
	    pR = ((pY << 16) + dctCrToR * pCr + 32768) >> 16;
	    rowBuf[0][y2][x1+x2] = dctClip[dctClipOffset + pR];
	    pG = ((pY << 16) + dctCbToG * pCb + dctCrToG * pCr + 32768) >> 16;
	    rowBuf[1][y2][x1+x2] = dctClip[dctClipOffset + pG];
	    pB = ((pY << 16) + dctCbToB * pCb + 32768) >> 16;
	    rowBuf[2][y2][x1+x2] = dctClip[dctClipOffset + pB];
	  }
	}
      // convert YCbCrK to CMYK (K is passed through unchanged)
      } else if (numComps == 4) {
	for (y2 = 0; y2 < mcuHeight; ++y2) {
	  for (x2 = 0; x2 < mcuWidth; ++x2) {
	    pY = rowBuf[0][y2][x1+x2];
	    pCb = rowBuf[1][y2][x1+x2] - 128;
	    pCr = rowBuf[2][y2][x1+x2] - 128;
	    pR = ((pY << 16) + dctCrToR * pCr + 32768) >> 16;
	    rowBuf[0][y2][x1+x2] = 255 - dctClip[dctClipOffset + pR];
	    pG = ((pY << 16) + dctCbToG * pCb + dctCrToG * pCr + 32768) >> 16;
	    rowBuf[1][y2][x1+x2] = 255 - dctClip[dctClipOffset + pG];
	    pB = ((pY << 16) + dctCbToB * pCb + 32768) >> 16;
	    rowBuf[2][y2][x1+x2] = 255 - dctClip[dctClipOffset + pB];
	  }
	}
      }
    }
  }
  return true;
}

bool XWDCTStream::readProgressiveDataUnit(DCTHuffTable *dcHuffTable,
                                             DCTHuffTable *acHuffTable,
                                             int *prevDC, int data[64])
{
  int run, size, amp, bit, c;
  int i, j, k;

  // get the DC coefficient
  i = scanInfo.firstCoeff;
  if (i == 0) {
    if (scanInfo.ah == 0) {
      if ((size = readHuffSym(dcHuffTable)) == 9999) {
	return false;
      }
      if (size > 0) {
	if ((amp = readAmp(size)) == 9999) {
	  return false;
	}
      } else {
	amp = 0;
      }
      data[0] += (*prevDC += amp) << scanInfo.al;
    } else {
      if ((bit = readBit()) == 9999) {
	return false;
      }
      data[0] += bit << scanInfo.al;
    }
    ++i;
  }
  if (scanInfo.lastCoeff == 0) {
    return true;
  }

  // check for an EOB run
  if (eobRun > 0) {
    while (i <= scanInfo.lastCoeff) {
      j = dctZigZag[i++];
      if (data[j] != 0) {
	if ((bit = readBit()) == EOF) {
	  return false;
	}
	if (bit) {
	  data[j] += 1 << scanInfo.al;
	}
      }
    }
    --eobRun;
    return true;
  }

  // read the AC coefficients
  while (i <= scanInfo.lastCoeff) {
    if ((c = readHuffSym(acHuffTable)) == 9999) {
      return false;
    }

    // ZRL
    if (c == 0xf0) {
      k = 0;
      while (k < 16 && i <= scanInfo.lastCoeff) {
	j = dctZigZag[i++];
	if (data[j] == 0) {
	  ++k;
	} else {
	  if ((bit = readBit()) == EOF) {
	    return false;
	  }
	  if (bit) {
	    data[j] += 1 << scanInfo.al;
	  }
	}
      }

    // EOB run
    } else if ((c & 0x0f) == 0x00) {
      j = c >> 4;
      eobRun = 0;
      for (k = 0; k < j; ++k) {
	if ((bit = readBit()) == EOF) {
	  return false;
	}
	eobRun = (eobRun << 1) | bit;
      }
      eobRun += 1 << j;
      while (i <= scanInfo.lastCoeff) {
	j = dctZigZag[i++];
	if (data[j] != 0) {
	  if ((bit = readBit()) == EOF) {
	    return false;
	  }
	  if (bit) {
	    data[j] += 1 << scanInfo.al;
	  }
	}
      }
      --eobRun;
      break;

    // zero run and one AC coefficient
    } else {
      run = (c >> 4) & 0x0f;
      size = c & 0x0f;
      if ((amp = readAmp(size)) == 9999) {
	return false;
      }
      j = 0; // make gcc happy
      for (k = 0; k <= run && i <= scanInfo.lastCoeff; ++k) {
	j = dctZigZag[i++];
	while (data[j] != 0 && i <= scanInfo.lastCoeff) {
	  if ((bit = readBit()) == EOF) {
	    return false;
	  }
	  if (bit) {
	    data[j] += 1 << scanInfo.al;
	  }
	  j = dctZigZag[i++];
	}
      }
      data[j] = amp << scanInfo.al;
    }
  }

  return true;
}

bool XWDCTStream::readProgressiveSOF()
{
  int length;
  int prec;
  int i;
  int c;

  length = read16();
  prec = str->getChar();
  height = read16();
  width = read16();
  numComps = str->getChar();
  if (numComps <= 0 || numComps > 4) {
    numComps = 0;
    return false;
  }
  if (prec != 8) {
    return false;
  }
  for (i = 0; i < numComps; ++i) {
    compInfo[i].id = str->getChar();
    c = str->getChar();
    compInfo[i].hSample = (c >> 4) & 0x0f;
    compInfo[i].vSample = c & 0x0f;
    compInfo[i].quantTable = str->getChar();
    if (compInfo[i].hSample < 1 || compInfo[i].hSample > 4 ||
	compInfo[i].vSample < 1 || compInfo[i].vSample > 4) {
      return false;
    }
    if (compInfo[i].quantTable < 0 || compInfo[i].quantTable > 3) {
      return false;
    }
  }
  progressive = true;
  return true;
}

bool XWDCTStream::readQuantTables()
{
  int length, prec, i, index;

  length = read16() - 2;
  while (length > 0) {
    index = str->getChar();
    prec = (index >> 4) & 0x0f;
    index &= 0x0f;
    if (prec > 1 || index >= 4) {
      return false;
    }
    if (index == numQuantTables) {
      numQuantTables = index + 1;
    }
    for (i = 0; i < 64; ++i) {
      if (prec) {
	quantTables[index][dctZigZag[i]] = read16();
      } else {
	quantTables[index][dctZigZag[i]] = str->getChar();
      }
    }
    if (prec) {
      length -= 129;
    } else {
      length -= 65;
    }
  }
  return true;
}

bool XWDCTStream::readRestartInterval()
{
    int length = read16();
    if (length != 4)
        return false;

    restartInterval = read16();
    return true;
}

void XWDCTStream::readScan()
{
  int data[64];
  int x1, y1, dx1, dy1, x2, y2, y3, cc, i;
  int h, v, horiz, vert, vSub;
  int *p1;
  int c;

  if (scanInfo.numComps == 1) {
    for (cc = 0; cc < numComps; ++cc) {
      if (scanInfo.comp[cc]) {
	break;
      }
    }
    dx1 = mcuWidth / compInfo[cc].hSample;
    dy1 = mcuHeight / compInfo[cc].vSample;
  } else {
    dx1 = mcuWidth;
    dy1 = mcuHeight;
  }

  for (y1 = 0; y1 < height; y1 += dy1) {
    for (x1 = 0; x1 < width; x1 += dx1) {

      // deal with restart marker
      if (restartInterval > 0 && restartCtr == 0) {
	c = readMarker();
	if (c != restartMarker) {
	  xwApp->error("Bad DCT data: incorrect restart marker");
	  return;
	}
	if (++restartMarker == 0xd8) {
	  restartMarker = 0xd0;
	}
	restart();
      }

      // read one MCU
      for (cc = 0; cc < numComps; ++cc) {
	if (!scanInfo.comp[cc]) {
	  continue;
	}

	h = compInfo[cc].hSample;
	v = compInfo[cc].vSample;
	horiz = mcuWidth / h;
	vert = mcuHeight / v;
	vSub = vert / 8;
	for (y2 = 0; y2 < dy1; y2 += vert) {
	  for (x2 = 0; x2 < dx1; x2 += horiz) {

	    // pull out the current values
	    p1 = &frameBuf[cc][(y1+y2) * bufWidth + (x1+x2)];
	    for (y3 = 0, i = 0; y3 < 8; ++y3, i += 8) {
	      data[i] = p1[0];
	      data[i+1] = p1[1];
	      data[i+2] = p1[2];
	      data[i+3] = p1[3];
	      data[i+4] = p1[4];
	      data[i+5] = p1[5];
	      data[i+6] = p1[6];
	      data[i+7] = p1[7];
	      p1 += bufWidth * vSub;
	    }

	    // read one data unit
	    if (progressive) {
	      if (!readProgressiveDataUnit(
		       &dcHuffTables[scanInfo.dcHuffTable[cc]],
		       &acHuffTables[scanInfo.acHuffTable[cc]],
		       &compInfo[cc].prevDC,
		       data)) {
		return;
	      }
	    } else {
	      if (!readDataUnit(&dcHuffTables[scanInfo.dcHuffTable[cc]],
				&acHuffTables[scanInfo.acHuffTable[cc]],
				&compInfo[cc].prevDC,
				data)) {
		return;
	      }
	    }

	    // add the data unit into frameBuf
	    p1 = &frameBuf[cc][(y1+y2) * bufWidth + (x1+x2)];
	    for (y3 = 0, i = 0; y3 < 8; ++y3, i += 8) {
	      p1[0] = data[i];
	      p1[1] = data[i+1];
	      p1[2] = data[i+2];
	      p1[3] = data[i+3];
	      p1[4] = data[i+4];
	      p1[5] = data[i+5];
	      p1[6] = data[i+6];
	      p1[7] = data[i+7];
	      p1 += bufWidth * vSub;
	    }
	  }
	}
      }
      --restartCtr;
    }
  }
}

bool XWDCTStream::readScanInfo()
{
  int length;
  int id, c;
  int i, j;

  length = read16() - 2;
  scanInfo.numComps = str->getChar();
  if (scanInfo.numComps <= 0 || scanInfo.numComps > 4) {
    scanInfo.numComps = 0;
    return false;
  }
  --length;
  if (length != 2 * scanInfo.numComps + 3) {
    return false;
  }
  interleaved = scanInfo.numComps == numComps;
  for (j = 0; j < numComps; ++j) {
    scanInfo.comp[j] = false;
  }
  for (i = 0; i < scanInfo.numComps; ++i) {
    id = str->getChar();
    // some (broken) DCT streams reuse ID numbers, but at least they
    // keep the components in order, so we check compInfo[i] first to
    // work around the problem
    if (id == compInfo[i].id) {
      j = i;
    } else {
      for (j = 0; j < numComps; ++j) {
	if (id == compInfo[j].id) {
	  break;
	}
      }
      if (j == numComps) {
	return false;
      }
    }
    scanInfo.comp[j] = true;
    c = str->getChar();
    scanInfo.dcHuffTable[j] = (c >> 4) & 0x0f;
    scanInfo.acHuffTable[j] = c & 0x0f;
  }
  scanInfo.firstCoeff = str->getChar();
  scanInfo.lastCoeff = str->getChar();
  if (scanInfo.firstCoeff < 0 || scanInfo.lastCoeff > 63 ||
      scanInfo.firstCoeff > scanInfo.lastCoeff) {
    return false;
  }
  c = str->getChar();
  scanInfo.ah = (c >> 4) & 0x0f;
  scanInfo.al = c & 0x0f;
  return true;
}

bool XWDCTStream::readTrailer()
{
    int c = readMarker();
    if (c != 0xd9)
        return false;

    return true;
}

int  XWDCTStream::read16()
{
    int c1 = 0;
    if ((c1 = str->getChar()) == EOF)
        return EOF;

    int c2 = 0;
    if ((c2 = str->getChar()) == EOF)
        return EOF;

    return (c1 << 8) + c2;
}

void XWDCTStream::restart()
{
    inputBits = 0;
    restartCtr = restartInterval;
    for (int i = 0; i < numComps; ++i)
        compInfo[i].prevDC = 0;

    eobRun = 0;
}

void XWDCTStream::transformDataUnit(ushort *quantTable, int dataIn[64], uchar dataOut[64])
{
  int v0, v1, v2, v3, v4, v5, v6, v7, t;
  int *p;
  int i;

  // dequant
  for (i = 0; i < 64; ++i) {
    dataIn[i] *= quantTable[i];
  }

  // inverse DCT on rows
  for (i = 0; i < 64; i += 8) {
    p = dataIn + i;

    // check for all-zero AC coefficients
    if (p[1] == 0 && p[2] == 0 && p[3] == 0 &&
	p[4] == 0 && p[5] == 0 && p[6] == 0 && p[7] == 0) {
      t = (dctSqrt2 * p[0] + 512) >> 10;
      p[0] = t;
      p[1] = t;
      p[2] = t;
      p[3] = t;
      p[4] = t;
      p[5] = t;
      p[6] = t;
      p[7] = t;
      continue;
    }

    // stage 4
    v0 = (dctSqrt2 * p[0] + 128) >> 8;
    v1 = (dctSqrt2 * p[4] + 128) >> 8;
    v2 = p[2];
    v3 = p[6];
    v4 = (dctSqrt1d2 * (p[1] - p[7]) + 128) >> 8;
    v7 = (dctSqrt1d2 * (p[1] + p[7]) + 128) >> 8;
    v5 = p[3] << 4;
    v6 = p[5] << 4;

    // stage 3
    t = (v0 - v1+ 1) >> 1;
    v0 = (v0 + v1 + 1) >> 1;
    v1 = t;
    t = (v2 * dctSin6 + v3 * dctCos6 + 128) >> 8;
    v2 = (v2 * dctCos6 - v3 * dctSin6 + 128) >> 8;
    v3 = t;
    t = (v4 - v6 + 1) >> 1;
    v4 = (v4 + v6 + 1) >> 1;
    v6 = t;
    t = (v7 + v5 + 1) >> 1;
    v5 = (v7 - v5 + 1) >> 1;
    v7 = t;

    // stage 2
    t = (v0 - v3 + 1) >> 1;
    v0 = (v0 + v3 + 1) >> 1;
    v3 = t;
    t = (v1 - v2 + 1) >> 1;
    v1 = (v1 + v2 + 1) >> 1;
    v2 = t;
    t = (v4 * dctSin3 + v7 * dctCos3 + 2048) >> 12;
    v4 = (v4 * dctCos3 - v7 * dctSin3 + 2048) >> 12;
    v7 = t;
    t = (v5 * dctSin1 + v6 * dctCos1 + 2048) >> 12;
    v5 = (v5 * dctCos1 - v6 * dctSin1 + 2048) >> 12;
    v6 = t;

    // stage 1
    p[0] = v0 + v7;
    p[7] = v0 - v7;
    p[1] = v1 + v6;
    p[6] = v1 - v6;
    p[2] = v2 + v5;
    p[5] = v2 - v5;
    p[3] = v3 + v4;
    p[4] = v3 - v4;
  }

  // inverse DCT on columns
  for (i = 0; i < 8; ++i) {
    p = dataIn + i;

    // check for all-zero AC coefficients
    if (p[1*8] == 0 && p[2*8] == 0 && p[3*8] == 0 &&
	p[4*8] == 0 && p[5*8] == 0 && p[6*8] == 0 && p[7*8] == 0) {
      t = (dctSqrt2 * dataIn[i+0] + 8192) >> 14;
      p[0*8] = t;
      p[1*8] = t;
      p[2*8] = t;
      p[3*8] = t;
      p[4*8] = t;
      p[5*8] = t;
      p[6*8] = t;
      p[7*8] = t;
      continue;
    }

    // stage 4
    v0 = (dctSqrt2 * p[0*8] + 2048) >> 12;
    v1 = (dctSqrt2 * p[4*8] + 2048) >> 12;
    v2 = p[2*8];
    v3 = p[6*8];
    v4 = (dctSqrt1d2 * (p[1*8] - p[7*8]) + 2048) >> 12;
    v7 = (dctSqrt1d2 * (p[1*8] + p[7*8]) + 2048) >> 12;
    v5 = p[3*8];
    v6 = p[5*8];

    // stage 3
    t = (v0 - v1 + 1) >> 1;
    v0 = (v0 + v1 + 1) >> 1;
    v1 = t;
    t = (v2 * dctSin6 + v3 * dctCos6 + 2048) >> 12;
    v2 = (v2 * dctCos6 - v3 * dctSin6 + 2048) >> 12;
    v3 = t;
    t = (v4 - v6 + 1) >> 1;
    v4 = (v4 + v6 + 1) >> 1;
    v6 = t;
    t = (v7 + v5 + 1) >> 1;
    v5 = (v7 - v5 + 1) >> 1;
    v7 = t;

    // stage 2
    t = (v0 - v3 + 1) >> 1;
    v0 = (v0 + v3 + 1) >> 1;
    v3 = t;
    t = (v1 - v2 + 1) >> 1;
    v1 = (v1 + v2 + 1) >> 1;
    v2 = t;
    t = (v4 * dctSin3 + v7 * dctCos3 + 2048) >> 12;
    v4 = (v4 * dctCos3 - v7 * dctSin3 + 2048) >> 12;
    v7 = t;
    t = (v5 * dctSin1 + v6 * dctCos1 + 2048) >> 12;
    v5 = (v5 * dctCos1 - v6 * dctSin1 + 2048) >> 12;
    v6 = t;

    // stage 1
    p[0*8] = v0 + v7;
    p[7*8] = v0 - v7;
    p[1*8] = v1 + v6;
    p[6*8] = v1 - v6;
    p[2*8] = v2 + v5;
    p[5*8] = v2 - v5;
    p[3*8] = v3 + v4;
    p[4*8] = v3 - v4;
  }

  // convert to 8-bit integers
  for (i = 0; i < 64; ++i) {
    dataOut[i] = dctClip[dctClipOffset + 128 + ((dataIn[i] + 8) >> 4)];
  }
}


int XWFlateStream::codeLenCodeMap[flateMaxCodeLenCodes] = {
  16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
};


FlateDecode XWFlateStream::lengthDecode[flateMaxLitCodes-257] = {
  {0,   3},
  {0,   4},
  {0,   5},
  {0,   6},
  {0,   7},
  {0,   8},
  {0,   9},
  {0,  10},
  {1,  11},
  {1,  13},
  {1,  15},
  {1,  17},
  {2,  19},
  {2,  23},
  {2,  27},
  {2,  31},
  {3,  35},
  {3,  43},
  {3,  51},
  {3,  59},
  {4,  67},
  {4,  83},
  {4,  99},
  {4, 115},
  {5, 131},
  {5, 163},
  {5, 195},
  {5, 227},
  {0, 258},
  {0, 258},
  {0, 258}
};

FlateDecode XWFlateStream::distDecode[flateMaxDistCodes] = {
  { 0,     1},
  { 0,     2},
  { 0,     3},
  { 0,     4},
  { 1,     5},
  { 1,     7},
  { 2,     9},
  { 2,    13},
  { 3,    17},
  { 3,    25},
  { 4,    33},
  { 4,    49},
  { 5,    65},
  { 5,    97},
  { 6,   129},
  { 6,   193},
  { 7,   257},
  { 7,   385},
  { 8,   513},
  { 8,   769},
  { 9,  1025},
  { 9,  1537},
  {10,  2049},
  {10,  3073},
  {11,  4097},
  {11,  6145},
  {12,  8193},
  {12, 12289},
  {13, 16385},
  {13, 24577}
};

static FlateCode flateFixedLitCodeTabCodes[512] = {
  {7, 0x0100},
  {8, 0x0050},
  {8, 0x0010},
  {8, 0x0118},
  {7, 0x0110},
  {8, 0x0070},
  {8, 0x0030},
  {9, 0x00c0},
  {7, 0x0108},
  {8, 0x0060},
  {8, 0x0020},
  {9, 0x00a0},
  {8, 0x0000},
  {8, 0x0080},
  {8, 0x0040},
  {9, 0x00e0},
  {7, 0x0104},
  {8, 0x0058},
  {8, 0x0018},
  {9, 0x0090},
  {7, 0x0114},
  {8, 0x0078},
  {8, 0x0038},
  {9, 0x00d0},
  {7, 0x010c},
  {8, 0x0068},
  {8, 0x0028},
  {9, 0x00b0},
  {8, 0x0008},
  {8, 0x0088},
  {8, 0x0048},
  {9, 0x00f0},
  {7, 0x0102},
  {8, 0x0054},
  {8, 0x0014},
  {8, 0x011c},
  {7, 0x0112},
  {8, 0x0074},
  {8, 0x0034},
  {9, 0x00c8},
  {7, 0x010a},
  {8, 0x0064},
  {8, 0x0024},
  {9, 0x00a8},
  {8, 0x0004},
  {8, 0x0084},
  {8, 0x0044},
  {9, 0x00e8},
  {7, 0x0106},
  {8, 0x005c},
  {8, 0x001c},
  {9, 0x0098},
  {7, 0x0116},
  {8, 0x007c},
  {8, 0x003c},
  {9, 0x00d8},
  {7, 0x010e},
  {8, 0x006c},
  {8, 0x002c},
  {9, 0x00b8},
  {8, 0x000c},
  {8, 0x008c},
  {8, 0x004c},
  {9, 0x00f8},
  {7, 0x0101},
  {8, 0x0052},
  {8, 0x0012},
  {8, 0x011a},
  {7, 0x0111},
  {8, 0x0072},
  {8, 0x0032},
  {9, 0x00c4},
  {7, 0x0109},
  {8, 0x0062},
  {8, 0x0022},
  {9, 0x00a4},
  {8, 0x0002},
  {8, 0x0082},
  {8, 0x0042},
  {9, 0x00e4},
  {7, 0x0105},
  {8, 0x005a},
  {8, 0x001a},
  {9, 0x0094},
  {7, 0x0115},
  {8, 0x007a},
  {8, 0x003a},
  {9, 0x00d4},
  {7, 0x010d},
  {8, 0x006a},
  {8, 0x002a},
  {9, 0x00b4},
  {8, 0x000a},
  {8, 0x008a},
  {8, 0x004a},
  {9, 0x00f4},
  {7, 0x0103},
  {8, 0x0056},
  {8, 0x0016},
  {8, 0x011e},
  {7, 0x0113},
  {8, 0x0076},
  {8, 0x0036},
  {9, 0x00cc},
  {7, 0x010b},
  {8, 0x0066},
  {8, 0x0026},
  {9, 0x00ac},
  {8, 0x0006},
  {8, 0x0086},
  {8, 0x0046},
  {9, 0x00ec},
  {7, 0x0107},
  {8, 0x005e},
  {8, 0x001e},
  {9, 0x009c},
  {7, 0x0117},
  {8, 0x007e},
  {8, 0x003e},
  {9, 0x00dc},
  {7, 0x010f},
  {8, 0x006e},
  {8, 0x002e},
  {9, 0x00bc},
  {8, 0x000e},
  {8, 0x008e},
  {8, 0x004e},
  {9, 0x00fc},
  {7, 0x0100},
  {8, 0x0051},
  {8, 0x0011},
  {8, 0x0119},
  {7, 0x0110},
  {8, 0x0071},
  {8, 0x0031},
  {9, 0x00c2},
  {7, 0x0108},
  {8, 0x0061},
  {8, 0x0021},
  {9, 0x00a2},
  {8, 0x0001},
  {8, 0x0081},
  {8, 0x0041},
  {9, 0x00e2},
  {7, 0x0104},
  {8, 0x0059},
  {8, 0x0019},
  {9, 0x0092},
  {7, 0x0114},
  {8, 0x0079},
  {8, 0x0039},
  {9, 0x00d2},
  {7, 0x010c},
  {8, 0x0069},
  {8, 0x0029},
  {9, 0x00b2},
  {8, 0x0009},
  {8, 0x0089},
  {8, 0x0049},
  {9, 0x00f2},
  {7, 0x0102},
  {8, 0x0055},
  {8, 0x0015},
  {8, 0x011d},
  {7, 0x0112},
  {8, 0x0075},
  {8, 0x0035},
  {9, 0x00ca},
  {7, 0x010a},
  {8, 0x0065},
  {8, 0x0025},
  {9, 0x00aa},
  {8, 0x0005},
  {8, 0x0085},
  {8, 0x0045},
  {9, 0x00ea},
  {7, 0x0106},
  {8, 0x005d},
  {8, 0x001d},
  {9, 0x009a},
  {7, 0x0116},
  {8, 0x007d},
  {8, 0x003d},
  {9, 0x00da},
  {7, 0x010e},
  {8, 0x006d},
  {8, 0x002d},
  {9, 0x00ba},
  {8, 0x000d},
  {8, 0x008d},
  {8, 0x004d},
  {9, 0x00fa},
  {7, 0x0101},
  {8, 0x0053},
  {8, 0x0013},
  {8, 0x011b},
  {7, 0x0111},
  {8, 0x0073},
  {8, 0x0033},
  {9, 0x00c6},
  {7, 0x0109},
  {8, 0x0063},
  {8, 0x0023},
  {9, 0x00a6},
  {8, 0x0003},
  {8, 0x0083},
  {8, 0x0043},
  {9, 0x00e6},
  {7, 0x0105},
  {8, 0x005b},
  {8, 0x001b},
  {9, 0x0096},
  {7, 0x0115},
  {8, 0x007b},
  {8, 0x003b},
  {9, 0x00d6},
  {7, 0x010d},
  {8, 0x006b},
  {8, 0x002b},
  {9, 0x00b6},
  {8, 0x000b},
  {8, 0x008b},
  {8, 0x004b},
  {9, 0x00f6},
  {7, 0x0103},
  {8, 0x0057},
  {8, 0x0017},
  {8, 0x011f},
  {7, 0x0113},
  {8, 0x0077},
  {8, 0x0037},
  {9, 0x00ce},
  {7, 0x010b},
  {8, 0x0067},
  {8, 0x0027},
  {9, 0x00ae},
  {8, 0x0007},
  {8, 0x0087},
  {8, 0x0047},
  {9, 0x00ee},
  {7, 0x0107},
  {8, 0x005f},
  {8, 0x001f},
  {9, 0x009e},
  {7, 0x0117},
  {8, 0x007f},
  {8, 0x003f},
  {9, 0x00de},
  {7, 0x010f},
  {8, 0x006f},
  {8, 0x002f},
  {9, 0x00be},
  {8, 0x000f},
  {8, 0x008f},
  {8, 0x004f},
  {9, 0x00fe},
  {7, 0x0100},
  {8, 0x0050},
  {8, 0x0010},
  {8, 0x0118},
  {7, 0x0110},
  {8, 0x0070},
  {8, 0x0030},
  {9, 0x00c1},
  {7, 0x0108},
  {8, 0x0060},
  {8, 0x0020},
  {9, 0x00a1},
  {8, 0x0000},
  {8, 0x0080},
  {8, 0x0040},
  {9, 0x00e1},
  {7, 0x0104},
  {8, 0x0058},
  {8, 0x0018},
  {9, 0x0091},
  {7, 0x0114},
  {8, 0x0078},
  {8, 0x0038},
  {9, 0x00d1},
  {7, 0x010c},
  {8, 0x0068},
  {8, 0x0028},
  {9, 0x00b1},
  {8, 0x0008},
  {8, 0x0088},
  {8, 0x0048},
  {9, 0x00f1},
  {7, 0x0102},
  {8, 0x0054},
  {8, 0x0014},
  {8, 0x011c},
  {7, 0x0112},
  {8, 0x0074},
  {8, 0x0034},
  {9, 0x00c9},
  {7, 0x010a},
  {8, 0x0064},
  {8, 0x0024},
  {9, 0x00a9},
  {8, 0x0004},
  {8, 0x0084},
  {8, 0x0044},
  {9, 0x00e9},
  {7, 0x0106},
  {8, 0x005c},
  {8, 0x001c},
  {9, 0x0099},
  {7, 0x0116},
  {8, 0x007c},
  {8, 0x003c},
  {9, 0x00d9},
  {7, 0x010e},
  {8, 0x006c},
  {8, 0x002c},
  {9, 0x00b9},
  {8, 0x000c},
  {8, 0x008c},
  {8, 0x004c},
  {9, 0x00f9},
  {7, 0x0101},
  {8, 0x0052},
  {8, 0x0012},
  {8, 0x011a},
  {7, 0x0111},
  {8, 0x0072},
  {8, 0x0032},
  {9, 0x00c5},
  {7, 0x0109},
  {8, 0x0062},
  {8, 0x0022},
  {9, 0x00a5},
  {8, 0x0002},
  {8, 0x0082},
  {8, 0x0042},
  {9, 0x00e5},
  {7, 0x0105},
  {8, 0x005a},
  {8, 0x001a},
  {9, 0x0095},
  {7, 0x0115},
  {8, 0x007a},
  {8, 0x003a},
  {9, 0x00d5},
  {7, 0x010d},
  {8, 0x006a},
  {8, 0x002a},
  {9, 0x00b5},
  {8, 0x000a},
  {8, 0x008a},
  {8, 0x004a},
  {9, 0x00f5},
  {7, 0x0103},
  {8, 0x0056},
  {8, 0x0016},
  {8, 0x011e},
  {7, 0x0113},
  {8, 0x0076},
  {8, 0x0036},
  {9, 0x00cd},
  {7, 0x010b},
  {8, 0x0066},
  {8, 0x0026},
  {9, 0x00ad},
  {8, 0x0006},
  {8, 0x0086},
  {8, 0x0046},
  {9, 0x00ed},
  {7, 0x0107},
  {8, 0x005e},
  {8, 0x001e},
  {9, 0x009d},
  {7, 0x0117},
  {8, 0x007e},
  {8, 0x003e},
  {9, 0x00dd},
  {7, 0x010f},
  {8, 0x006e},
  {8, 0x002e},
  {9, 0x00bd},
  {8, 0x000e},
  {8, 0x008e},
  {8, 0x004e},
  {9, 0x00fd},
  {7, 0x0100},
  {8, 0x0051},
  {8, 0x0011},
  {8, 0x0119},
  {7, 0x0110},
  {8, 0x0071},
  {8, 0x0031},
  {9, 0x00c3},
  {7, 0x0108},
  {8, 0x0061},
  {8, 0x0021},
  {9, 0x00a3},
  {8, 0x0001},
  {8, 0x0081},
  {8, 0x0041},
  {9, 0x00e3},
  {7, 0x0104},
  {8, 0x0059},
  {8, 0x0019},
  {9, 0x0093},
  {7, 0x0114},
  {8, 0x0079},
  {8, 0x0039},
  {9, 0x00d3},
  {7, 0x010c},
  {8, 0x0069},
  {8, 0x0029},
  {9, 0x00b3},
  {8, 0x0009},
  {8, 0x0089},
  {8, 0x0049},
  {9, 0x00f3},
  {7, 0x0102},
  {8, 0x0055},
  {8, 0x0015},
  {8, 0x011d},
  {7, 0x0112},
  {8, 0x0075},
  {8, 0x0035},
  {9, 0x00cb},
  {7, 0x010a},
  {8, 0x0065},
  {8, 0x0025},
  {9, 0x00ab},
  {8, 0x0005},
  {8, 0x0085},
  {8, 0x0045},
  {9, 0x00eb},
  {7, 0x0106},
  {8, 0x005d},
  {8, 0x001d},
  {9, 0x009b},
  {7, 0x0116},
  {8, 0x007d},
  {8, 0x003d},
  {9, 0x00db},
  {7, 0x010e},
  {8, 0x006d},
  {8, 0x002d},
  {9, 0x00bb},
  {8, 0x000d},
  {8, 0x008d},
  {8, 0x004d},
  {9, 0x00fb},
  {7, 0x0101},
  {8, 0x0053},
  {8, 0x0013},
  {8, 0x011b},
  {7, 0x0111},
  {8, 0x0073},
  {8, 0x0033},
  {9, 0x00c7},
  {7, 0x0109},
  {8, 0x0063},
  {8, 0x0023},
  {9, 0x00a7},
  {8, 0x0003},
  {8, 0x0083},
  {8, 0x0043},
  {9, 0x00e7},
  {7, 0x0105},
  {8, 0x005b},
  {8, 0x001b},
  {9, 0x0097},
  {7, 0x0115},
  {8, 0x007b},
  {8, 0x003b},
  {9, 0x00d7},
  {7, 0x010d},
  {8, 0x006b},
  {8, 0x002b},
  {9, 0x00b7},
  {8, 0x000b},
  {8, 0x008b},
  {8, 0x004b},
  {9, 0x00f7},
  {7, 0x0103},
  {8, 0x0057},
  {8, 0x0017},
  {8, 0x011f},
  {7, 0x0113},
  {8, 0x0077},
  {8, 0x0037},
  {9, 0x00cf},
  {7, 0x010b},
  {8, 0x0067},
  {8, 0x0027},
  {9, 0x00af},
  {8, 0x0007},
  {8, 0x0087},
  {8, 0x0047},
  {9, 0x00ef},
  {7, 0x0107},
  {8, 0x005f},
  {8, 0x001f},
  {9, 0x009f},
  {7, 0x0117},
  {8, 0x007f},
  {8, 0x003f},
  {9, 0x00df},
  {7, 0x010f},
  {8, 0x006f},
  {8, 0x002f},
  {9, 0x00bf},
  {8, 0x000f},
  {8, 0x008f},
  {8, 0x004f},
  {9, 0x00ff}
};

FlateHuffmanTab XWFlateStream::fixedLitCodeTab = {
  flateFixedLitCodeTabCodes, 9
};

static FlateCode flateFixedDistCodeTabCodes[32] = {
  {5, 0x0000},
  {5, 0x0010},
  {5, 0x0008},
  {5, 0x0018},
  {5, 0x0004},
  {5, 0x0014},
  {5, 0x000c},
  {5, 0x001c},
  {5, 0x0002},
  {5, 0x0012},
  {5, 0x000a},
  {5, 0x001a},
  {5, 0x0006},
  {5, 0x0016},
  {5, 0x000e},
  {0, 0x0000},
  {5, 0x0001},
  {5, 0x0011},
  {5, 0x0009},
  {5, 0x0019},
  {5, 0x0005},
  {5, 0x0015},
  {5, 0x000d},
  {5, 0x001d},
  {5, 0x0003},
  {5, 0x0013},
  {5, 0x000b},
  {5, 0x001b},
  {5, 0x0007},
  {5, 0x0017},
  {5, 0x000f},
  {0, 0x0000}
};

FlateHuffmanTab XWFlateStream::fixedDistCodeTab = {
  flateFixedDistCodeTabCodes, 5
};

XWFlateStream::XWFlateStream(XWStream *strA,
                             int predictor,
                             int columns,
                             int colors,
                             int bits)
     :XWFilterStream(strA)
{
	pred = 0;
    if (predictor != 1)
    {
        pred = new XWStreamPredictor(this, predictor, columns, colors, bits);
        if (!pred->isOk())
        {
            delete pred;
            pred = 0;
        }
    }

    litCodeTab.codes = 0;
    distCodeTab.codes = 0;
    memset(buf, 0, flateWindow);
}

XWFlateStream::~XWFlateStream()
{
    if (litCodeTab.codes != fixedLitCodeTab.codes)
        delete [] litCodeTab.codes;

    if (distCodeTab.codes != fixedDistCodeTab.codes)
        delete [] distCodeTab.codes;

    if (pred)
        delete pred;

    if (str)
        delete str;
}

int XWFlateStream::getBlock(char *blk, int size)
{
	int n;

  if (pred) {
    return pred->getBlock(blk, size);
  }

  n = 0;
  while (n < size) {
    if (endOfBlock && eof) {
      break;
    }
    if (remain == 0) {
      readSome();
    }
    while (remain && n < size) {
      blk[n++] = buf[index];
      index = (index + 1) & flateMask;
      --remain;
    }
  }
  return n;
}

int XWFlateStream::getChar()
{
    if (pred)
        return pred->getChar();

    while (remain == 0)
    {
        if (endOfBlock && eof)
            return EOF;
        readSome();
    }
    int c = buf[index];
    index = (index + 1) & flateMask;
    --remain;
    return c;
}

XWString * XWFlateStream::getPSFilter(int psLevel, const char *indent)
{
    XWString *s = 0;
    if (psLevel < 3 || pred)
        return 0;

    if (!(s = str->getPSFilter(psLevel, indent)))
        return 0;

    s->append(indent)->append("<< >> /FlateDecode filter\n");
    return s;
}

int XWFlateStream::getRawChar()
{
    while (remain == 0)
    {
        if (endOfBlock && eof)
            return EOF;
        readSome();
    }
    int c = buf[index];
    index = (index + 1) & flateMask;
    --remain;
    return c;
}

bool XWFlateStream::isBinary(bool)
{
    return str->isBinary(true);
}

int XWFlateStream::lookChar()
{
    if (pred)
        return pred->lookChar();

    while (remain == 0)
    {
        if (endOfBlock && eof)
            return EOF;
        readSome();
    }
    int c = buf[index];
    return c;
}

void XWFlateStream::reset()
{
    index = 0;
    remain = 0;
    codeBuf = 0;
    codeSize = 0;
    compressedBlock = false;
    endOfBlock = true;
    eof = true;

    str->reset();

    endOfBlock = eof = true;
    int cmf = str->getChar();
    int flg = str->getChar();
    if (cmf == EOF || flg == EOF)
        return;

    if ((cmf & 0x0f) != 0x08)
        return;

    if ((((cmf << 8) + flg) % 31) != 0)
        return;

    if (flg & 0x20)
        return;

    eof = false;
}

void XWFlateStream::compHuffmanCodes(int *lengths, int n, FlateHuffmanTab *tab)
{
  int tabSize, len, code, code2, skip, val, i, t;

  // find max code length
  tab->maxLen = 0;
  for (val = 0; val < n; ++val) {
    if (lengths[val] > tab->maxLen) {
      tab->maxLen = lengths[val];
    }
  }

  // allocate the table
  tabSize = 1 << tab->maxLen;
  tab->codes = (FlateCode *)malloc(tabSize * sizeof(FlateCode));

  // clear the table
  for (i = 0; i < tabSize; ++i) {
    tab->codes[i].len = 0;
    tab->codes[i].val = 0;
  }

  // build the table
  for (len = 1, code = 0, skip = 2;
       len <= tab->maxLen;
       ++len, code <<= 1, skip <<= 1) {
    for (val = 0; val < n; ++val) {
      if (lengths[val] == len) {

	// bit-reverse the code
	code2 = 0;
	t = code;
	for (i = 0; i < len; ++i) {
	  code2 = (code2 << 1) | (t & 1);
	  t >>= 1;
	}

	// fill in the table entries
	for (i = code2; i < tabSize; i += skip) {
	  tab->codes[i].len = (ushort)len;
	  tab->codes[i].val = (ushort)val;
	}

	++code;
      }
    }
  }
}

int XWFlateStream::getCodeWord(int bits)
{
    int c = 0;
    while (codeSize < bits)
    {
        if ((c = str->getChar()) == EOF)
            return EOF;

        codeBuf |= (c & 0xff) << codeSize;
        codeSize += 8;
    }
    c = codeBuf & ((1 << bits) - 1);
    codeBuf >>= bits;
    codeSize -= bits;
    return c;
}

int XWFlateStream::getHuffmanCodeWord(FlateHuffmanTab *tab)
{
    int c = 0;
    while (codeSize < tab->maxLen)
    {
        if ((c = str->getChar()) == EOF)
            break;
        codeBuf |= (c & 0xff) << codeSize;
        codeSize += 8;
    }
    FlateCode * code = &tab->codes[codeBuf & ((1 << tab->maxLen) - 1)];
    if (codeSize == 0 || codeSize < code->len || code->len == 0)
        return EOF;
    codeBuf >>= code->len;
    codeSize -= code->len;
    return (int)code->val;
}

void XWFlateStream::loadFixedCodes()
{
    litCodeTab.codes = fixedLitCodeTab.codes;
    litCodeTab.maxLen = fixedLitCodeTab.maxLen;
    distCodeTab.codes = fixedDistCodeTab.codes;
    distCodeTab.maxLen = fixedDistCodeTab.maxLen;
}

bool XWFlateStream::readDynamicCodes()
{
  int numCodeLenCodes;
  int numLitCodes;
  int numDistCodes;
  int codeLenCodeLengths[flateMaxCodeLenCodes];
  FlateHuffmanTab codeLenCodeTab;
  int len, repeat, code;
  int i;

  codeLenCodeTab.codes = NULL;

  // read lengths
  if ((numLitCodes = getCodeWord(5)) == EOF) {
    goto err;
  }
  numLitCodes += 257;
  if ((numDistCodes = getCodeWord(5)) == EOF) {
    goto err;
  }
  numDistCodes += 1;
  if ((numCodeLenCodes = getCodeWord(4)) == EOF) {
    goto err;
  }
  numCodeLenCodes += 4;
  if (numLitCodes > flateMaxLitCodes ||
      numDistCodes > flateMaxDistCodes ||
      numCodeLenCodes > flateMaxCodeLenCodes) {
    goto err;
  }

  // build the code length code table
  for (i = 0; i < flateMaxCodeLenCodes; ++i) {
    codeLenCodeLengths[i] = 0;
  }
  for (i = 0; i < numCodeLenCodes; ++i) {
    if ((codeLenCodeLengths[codeLenCodeMap[i]] = getCodeWord(3)) == -1) {
      goto err;
    }
  }
  compHuffmanCodes(codeLenCodeLengths, flateMaxCodeLenCodes, &codeLenCodeTab);

  // build the literal and distance code tables
  len = 0;
  repeat = 0;
  i = 0;
  while (i < numLitCodes + numDistCodes) {
    if ((code = getHuffmanCodeWord(&codeLenCodeTab)) == EOF) {
      goto err;
    }
    if (code == 16) {
      if ((repeat = getCodeWord(2)) == EOF) {
	goto err;
      }
      repeat += 3;
      if (i + repeat > numLitCodes + numDistCodes) {
	goto err;
      }
      for (; repeat > 0; --repeat) {
	codeLengths[i++] = len;
      }
    } else if (code == 17) {
      if ((repeat = getCodeWord(3)) == EOF) {
	goto err;
      }
      repeat += 3;
      if (i + repeat > numLitCodes + numDistCodes) {
	goto err;
      }
      len = 0;
      for (; repeat > 0; --repeat) {
	codeLengths[i++] = 0;
      }
    } else if (code == 18) {
      if ((repeat = getCodeWord(7)) == EOF) {
	goto err;
      }
      repeat += 11;
      if (i + repeat > numLitCodes + numDistCodes) {
	goto err;
      }
      len = 0;
      for (; repeat > 0; --repeat) {
	codeLengths[i++] = 0;
      }
    } else {
      codeLengths[i++] = len = code;
    }
  }
  compHuffmanCodes(codeLengths, numLitCodes, &litCodeTab);
  compHuffmanCodes(codeLengths + numLitCodes, numDistCodes, &distCodeTab);

	if (codeLenCodeTab.codes)
  free(codeLenCodeTab.codes);
  return true;

err:
	if (codeLenCodeTab.codes)
  free(codeLenCodeTab.codes);
  return false;
}

void XWFlateStream::readSome()
{
  int code1, code2;
  int len, dist;
  int i, j, k;
  int c;

  if (endOfBlock) {
    if (!startBlock())
      return;
  }

  if (compressedBlock) {
    if ((code1 = getHuffmanCodeWord(&litCodeTab)) == EOF)
      goto err;
    if (code1 < 256) {
      buf[index] = code1;
      remain = 1;
    } else if (code1 == 256) {
      endOfBlock = true;
      remain = 0;
    } else {
      code1 -= 257;
      code2 = lengthDecode[code1].bits;
      if (code2 > 0 && (code2 = getCodeWord(code2)) == EOF)
	goto err;
      len = lengthDecode[code1].first + code2;
      if ((code1 = getHuffmanCodeWord(&distCodeTab)) == EOF)
	goto err;
      code2 = distDecode[code1].bits;
      if (code2 > 0 && (code2 = getCodeWord(code2)) == EOF)
	goto err;
      dist = distDecode[code1].first + code2;
      i = index;
      j = (index - dist) & flateMask;
      for (k = 0; k < len; ++k) {
	buf[i] = buf[j];
	i = (i + 1) & flateMask;
	j = (j + 1) & flateMask;
      }
      remain = len;
    }

  } else {
    len = (blockLen < flateWindow) ? blockLen : flateWindow;
    for (i = 0, j = index; i < len; ++i, j = (j + 1) & flateMask) {
      if ((c = str->getChar()) == EOF) {
	endOfBlock = eof = true;
	break;
      }
      buf[j] = c & 0xff;
    }
    remain = i;
    blockLen -= len;
    if (blockLen == 0)
      endOfBlock = true;
  }

  return;

err:
  endOfBlock = eof = true;
  remain = 0;
}

bool XWFlateStream::startBlock()
{
  int blockHdr;
  int c;
  int check;

  // free the code tables from the previous block
  if (litCodeTab.codes != fixedLitCodeTab.codes) {
    free(litCodeTab.codes);
  }
  litCodeTab.codes = NULL;
  if (distCodeTab.codes != fixedDistCodeTab.codes) {
    free(distCodeTab.codes);
  }
  distCodeTab.codes = NULL;

  // read block header
  blockHdr = getCodeWord(3);
  if (blockHdr & 1)
    eof = true;
  blockHdr >>= 1;

  // uncompressed block
  if (blockHdr == 0) {
    compressedBlock = false;
    if ((c = str->getChar()) == EOF)
      goto err;
    blockLen = c & 0xff;
    if ((c = str->getChar()) == EOF)
      goto err;
    blockLen |= (c & 0xff) << 8;
    if ((c = str->getChar()) == EOF)
      goto err;
    check = c & 0xff;
    if ((c = str->getChar()) == EOF)
      goto err;
    check |= (c & 0xff) << 8;
    if (check != (~blockLen & 0xffff))
      xwApp->error("Bad uncompressed block length in flate stream");
    codeBuf = 0;
    codeSize = 0;

  // compressed block with fixed codes
  } else if (blockHdr == 1) {
    compressedBlock = true;
    loadFixedCodes();

  // compressed block with dynamic codes
  } else if (blockHdr == 2) {
    compressedBlock = true;
    if (!readDynamicCodes()) {
      goto err;
    }

  // unknown block type
  } else {
    goto err;
  }

  endOfBlock = false;
  return true;

err:
  endOfBlock = eof = true;
  return false;
}

XWEOFStream::XWEOFStream(XWStream *strA)
    :XWFilterStream(strA)
{
}

XWEOFStream::~XWEOFStream()
{
    if (str)
        delete str;
}

XWBufStream::XWBufStream(XWStream *strA, int bufSizeA)
	: XWFilterStream(strA)
{
	bufSize = bufSizeA;
  buf = (int *)malloc(bufSize * sizeof(int));
}

XWBufStream::~XWBufStream()
{
	free(buf);
  delete str;
}

void XWBufStream::reset()
{
	int i;

  str->reset();
  for (i = 0; i < bufSize; ++i) {
    buf[i] = str->getChar();
  }
}

int XWBufStream::getChar()
{
	int c, i;

  c = buf[0];
  for (i = 1; i < bufSize; ++i) {
    buf[i-1] = buf[i];
  }
  buf[bufSize - 1] = str->getChar();
  return c;
}

int XWBufStream::lookChar()
{
	return buf[0];
}

int XWBufStream::lookChar(int idx)
{
	return buf[idx];
}

bool XWBufStream::isBinary(bool)
{
	return str->isBinary(true);
}

XWFixedLengthEncoder::XWFixedLengthEncoder(XWStream *strA, 
  	                                       int lengthA)
	:XWFilterStream(strA),
	 length(lengthA),
	 count(0)
{
}

XWFixedLengthEncoder::~XWFixedLengthEncoder()
{
	if (str && str->isEncoder())
    	delete str;
}

int XWFixedLengthEncoder::getChar()
{
	if (length >= 0 && count >= length)
    	return EOF;
    	
  	++count;
  	return str->getChar();
}

bool XWFixedLengthEncoder::isBinary(bool)
{
	return str->isBinary(true);
}

int XWFixedLengthEncoder::lookChar()
{
	if (length >= 0 && count >= length)
    	return EOF;
    	
  	return str->getChar();
}

void XWFixedLengthEncoder::reset()
{
	str->reset();
  	count = 0;
}

XWASCIIHexEncoder::XWASCIIHexEncoder(XWStream *strA)
	:XWFilterStream(strA)
{
	bufPtr = bufEnd = buf;
  	lineLen = 0;
  	eof = false;
}

XWASCIIHexEncoder::~XWASCIIHexEncoder()
{
	if (str && str->isEncoder()) 
    	delete str;
}

void XWASCIIHexEncoder::reset()
{
	str->reset();
  	bufPtr = bufEnd = buf;
  	lineLen = 0;
  	eof = false;
}

bool XWASCIIHexEncoder::fillBuf()
{
	static char *hex = "0123456789abcdef";
  	if (eof) 
    	return false;
  	
  	bufPtr = bufEnd = buf;
  	int c = EOF;
  	if ((c = str->getChar()) == EOF) 
  	{
    	*bufEnd++ = '>';
    	eof = true;
  	} 
  	else 
  	{
    	if (lineLen >= 64) 
    	{
      		*bufEnd++ = '\n';
      		lineLen = 0;
    	}
    	
    	*bufEnd++ = hex[(c >> 4) & 0x0f];
    	*bufEnd++ = hex[c & 0x0f];
    	lineLen += 2;
  	}
  	
  	return true;
}

XWASCII85Encoder::XWASCII85Encoder(XWStream *strA)
	:XWFilterStream(strA)
{
	bufPtr = bufEnd = buf;
  	lineLen = 0;
  	eof = false;
}

XWASCII85Encoder::~XWASCII85Encoder()
{
	if (str && str->isEncoder())
    	delete str;
}

void XWASCII85Encoder::reset()
{
	str->reset();
  	bufPtr = bufEnd = buf;
  	lineLen = 0;
  	eof = false;
}

bool XWASCII85Encoder::fillBuf()
{
	uint t;
  char buf1[5];
  int c0, c1, c2, c3;
  int n, i;

  if (eof) {
    return false;
  }
  c0 = str->getChar();
  c1 = str->getChar();
  c2 = str->getChar();
  c3 = str->getChar();
  bufPtr = bufEnd = buf;
  if (c3 == EOF) {
    if (c0 == EOF) {
      n = 0;
      t = 0;
    } else {
      if (c1 == EOF) {
	n = 1;
	t = c0 << 24;
      } else if (c2 == EOF) {
	n = 2;
	t = (c0 << 24) | (c1 << 16);
      } else {
	n = 3;
	t = (c0 << 24) | (c1 << 16) | (c2 << 8);
      }
      for (i = 4; i >= 0; --i) {
	buf1[i] = (char)(t % 85 + 0x21);
	t /= 85;
      }
      for (i = 0; i <= n; ++i) {
	*bufEnd++ = buf1[i];
	if (++lineLen == 65) {
	  *bufEnd++ = '\n';
	  lineLen = 0;
	}
      }
    }
    *bufEnd++ = '~';
    *bufEnd++ = '>';
    eof = true;
  } else {
    t = (c0 << 24) | (c1 << 16) | (c2 << 8) | c3;
    if (t == 0) {
      *bufEnd++ = 'z';
      if (++lineLen == 65) {
	*bufEnd++ = '\n';
	lineLen = 0;
      }
    } else {
      for (i = 4; i >= 0; --i) {
	buf1[i] = (char)(t % 85 + 0x21);
	t /= 85;
      }
      for (i = 0; i <= 4; ++i) {
	*bufEnd++ = buf1[i];
	if (++lineLen == 65) {
	  *bufEnd++ = '\n';
	  lineLen = 0;
	}
      }
    }
  }
  return true;
}

XWRunLengthEncoder::XWRunLengthEncoder(XWStream *strA)
	:XWFilterStream(strA)
{
	bufPtr = bufEnd = nextEnd = buf;
  	eof = false;
}

XWRunLengthEncoder::~XWRunLengthEncoder()
{
	if (str && str->isEncoder())
    	delete str;
}

void XWRunLengthEncoder::reset()
{
	str->reset();
  	bufPtr = bufEnd = nextEnd = buf;
  	eof = false;
}

bool XWRunLengthEncoder::fillBuf()
{
	int c, c1, c2;
  int n;

  // already hit EOF?
  if (eof)
    return false;

  // grab two bytes
  if (nextEnd < bufEnd + 1) {
    if ((c1 = str->getChar()) == EOF) {
      eof = true;
      return false;
    }
  } else {
    c1 = bufEnd[0] & 0xff;
  }
  if (nextEnd < bufEnd + 2) {
    if ((c2 = str->getChar()) == EOF) {
      eof = true;
      buf[0] = 0;
      buf[1] = c1;
      bufPtr = buf;
      bufEnd = &buf[2];
      return true;
    }
  } else {
    c2 = bufEnd[1] & 0xff;
  }

  // check for repeat
  c = 0; // make gcc happy
  if (c1 == c2) {
    n = 2;
    while (n < 128 && (c = str->getChar()) == c1)
      ++n;
    buf[0] = (char)(257 - n);
    buf[1] = c1;
    bufEnd = &buf[2];
    if (c == EOF) {
      eof = true;
    } else if (n < 128) {
      buf[2] = c;
      nextEnd = &buf[3];
    } else {
      nextEnd = bufEnd;
    }

  // get up to 128 chars
  } else {
    buf[1] = c1;
    buf[2] = c2;
    n = 2;
    while (n < 128) {
      if ((c = str->getChar()) == EOF) {
	eof = true;
	break;
      }
      ++n;
      buf[n] = c;
      if (buf[n] == buf[n-1])
	break;
    }
    if (buf[n] == buf[n-1]) {
      buf[0] = (char)(n-2-1);
      bufEnd = &buf[n-1];
      nextEnd = &buf[n+1];
    } else {
      buf[0] = (char)(n-1);
      bufEnd = nextEnd = &buf[n+1];
    }
  }
  bufPtr = buf;
  return true;
}

