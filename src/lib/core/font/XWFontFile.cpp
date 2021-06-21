/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWUtil.h"
#include "XWString.h"
#include "XWFontSetting.h"
#include "XWDocSea.h"
#include "XWFontFile.h"

class Reader
{
public:

  virtual ~Reader() {}

  // Read one byte.  Returns -1 if past EOF.
  virtual int getByte(int pos) = 0;

  // Read a big-endian unsigned 16-bit integer.  Fills in *val and
  // returns true if successful.
  virtual bool getU16BE(int pos, int *val) = 0;

  // Read a big-endian unsigned 32-bit integer.  Fills in *val and
  // returns true if successful.
  virtual bool getU32BE(int pos, uint *val) = 0;

  // Read a little-endian unsigned 32-bit integer.  Fills in *val and
  // returns true if successful.
  virtual bool getU32LE(int pos, uint *val) = 0;

  // Read a big-endian unsigned <size>-byte integer, where 1 <= size
  // <= 4.  Fills in *val and returns true if successful.
  virtual bool getUVarBE(int pos, int size, uint *val) = 0;

  // Compare against a string.  Returns true if equal.
  virtual bool cmp(int pos, const char *s) = 0;
};

class MemReader: public Reader 
{
public:

  static MemReader *make(char *bufA, int lenA);
  virtual ~MemReader();
  virtual int getByte(int pos);
  virtual bool getU16BE(int pos, int *val);
  virtual bool getU32BE(int pos, uint *val);
  virtual bool getU32LE(int pos, uint *val);
  virtual bool getUVarBE(int pos, int size, uint *val);
  virtual bool cmp(int pos, const char *s);

private:

  MemReader(char *bufA, int lenA);

  char *buf;
  int len;
};

MemReader *MemReader::make(char *bufA, int lenA) 
{
  return new MemReader(bufA, lenA);
}

MemReader::MemReader(char *bufA, int lenA) 
{
  buf = bufA;
  len = lenA;
}

MemReader::~MemReader() 
{
}

int MemReader::getByte(int pos) 
{
  if (pos < 0 || pos >= len) {
    return -1;
  }
  return buf[pos] & 0xff;
}

bool MemReader::getU16BE(int pos, int *val) 
{
  if (pos < 0 || pos > len - 2) 
  {
    return false;
  }
  *val = ((buf[pos] & 0xff) << 8) + (buf[pos+1] & 0xff);
  return true;
}

bool MemReader::getU32BE(int pos, uint *val) 
{
  if (pos < 0 || pos > len - 4) 
  {
    return false;
  }
  *val = ((buf[pos] & 0xff) << 24) +
         ((buf[pos+1] & 0xff) << 16) +
         ((buf[pos+2] & 0xff) << 8) +
         (buf[pos+3] & 0xff);
  return true;
}

bool MemReader::getU32LE(int pos, uint *val) 
{
  if (pos < 0 || pos > len - 4) 
  {
    return false;
  }
  *val = (buf[pos] & 0xff) +
         ((buf[pos+1] & 0xff) << 8) +
         ((buf[pos+2] & 0xff) << 16) +
         ((buf[pos+3] & 0xff) << 24);
  return true;
}

bool MemReader::getUVarBE(int pos, int size, uint *val) 
{
  int i;

  if (size < 1 || size > 4 || pos < 0 || pos > len - size) 
  {
    return false;
  }
  *val = 0;
  for (i = 0; i < size; ++i) 
 	{
    *val = (*val << 8) + (buf[pos + i] & 0xff);
  }
  return true;
}

bool MemReader::cmp(int pos, const char *s) 
{
  int n;

  n = (int)strlen(s);
  if (pos < 0 || len < n || pos > len - n) 
  {
    return false;
  }
  return !memcmp(buf + pos, s, n);
}


class FileReader: public Reader {
public:

  static FileReader *make(char *fileName);
  virtual ~FileReader();
  virtual int getByte(int pos);
  virtual bool getU16BE(int pos, int *val);
  virtual bool getU32BE(int pos, uint *val);
  virtual bool getU32LE(int pos, uint *val);
  virtual bool getUVarBE(int pos, int size, uint *val);
  virtual bool cmp(int pos, const char *s);

private:

  FileReader(FILE *fA);
  bool fillBuf(int pos, int len);

  FILE *f;
  char buf[1024];
  int bufPos, bufLen;
};

FileReader *FileReader::make(char *fileName) {
  FILE *fA;

  if (!(fA = fopen(fileName, "rb"))) 
  {
    return NULL;
  }
  return new FileReader(fA);
}

FileReader::FileReader(FILE *fA) 
{
  f = fA;
  bufPos = 0;
  bufLen = 0;
}

FileReader::~FileReader() 
{
  fclose(f);
}

int FileReader::getByte(int pos) 
{
  if (!fillBuf(pos, 1)) 
  {
    return -1;
  }
  return buf[pos - bufPos] & 0xff;
}

bool FileReader::getU16BE(int pos, int *val) 
{
  if (!fillBuf(pos, 2)) 
  {
    return false;
  }
  *val = ((buf[pos - bufPos] & 0xff) << 8) + (buf[pos - bufPos + 1] & 0xff);
  return true;
}

bool FileReader::getU32BE(int pos, uint *val) 
{
  if (!fillBuf(pos, 4)) 
  {
    return false;
  }
  *val = ((buf[pos - bufPos] & 0xff) << 24) +
         ((buf[pos - bufPos + 1] & 0xff) << 16) +
         ((buf[pos - bufPos + 2] & 0xff) << 8) +
         (buf[pos - bufPos + 3] & 0xff);
  return true;
}

bool FileReader::getU32LE(int pos, uint *val) 
{
  if (!fillBuf(pos, 4)) 
  {
    return false;
  }
  *val = (buf[pos - bufPos] & 0xff) +
         ((buf[pos - bufPos + 1] & 0xff) << 8) +
         ((buf[pos - bufPos + 2] & 0xff) << 16) +
         ((buf[pos - bufPos + 3] & 0xff) << 24);
  return true;
}

bool FileReader::getUVarBE(int pos, int size, uint *val) 
{
  int i;

  if (size < 1 || size > 4 || !fillBuf(pos, size)) 
  {
    return false;
  }
  *val = 0;
  for (i = 0; i < size; ++i) 
 	{
    *val = (*val << 8) + (buf[pos - bufPos + i] & 0xff);
  }
  return true;
}

bool FileReader::cmp(int pos, const char *s) 
{
  int n;

  n = (int)strlen(s);
  if (!fillBuf(pos, n)) 
  {
    return false;
  }
  return !memcmp(buf - bufPos + pos, s, n);
}

bool FileReader::fillBuf(int pos, int len) 
{
  if (pos < 0 || len < 0 || len > (int)sizeof(buf) ||
      pos > INT_MAX - (int)sizeof(buf)) 
  {
    return false;
  }
  if (pos >= bufPos && pos + len <= bufPos + bufLen) 
  {
    return true;
  }
  if (fseek(f, pos, SEEK_SET)) 
  {
    return false;
  }
  bufPos = pos;
  bufLen = (int)fread(buf, 1, sizeof(buf), f);
  if (bufLen < len) 
  {
    return false;
  }
  return true;
}


class StreamReader: public Reader 
{
public:

  static StreamReader *make(int (*getCharA)(void *data), void *dataA);
  virtual ~StreamReader();
  virtual int getByte(int pos);
  virtual bool getU16BE(int pos, int *val);
  virtual bool getU32BE(int pos, uint *val);
  virtual bool getU32LE(int pos, uint *val);
  virtual bool getUVarBE(int pos, int size, uint *val);
  virtual bool cmp(int pos, const char *s);

private:

  StreamReader(int (*getCharA)(void *data), void *dataA);
  bool fillBuf(int pos, int len);

  int (*getChar)(void *data);
  void *data;
  int streamPos;
  char buf[1024];
  int bufPos, bufLen;
};

StreamReader *StreamReader::make(int (*getCharA)(void *data), void *dataA) 
{
  return new StreamReader(getCharA, dataA);
}

StreamReader::StreamReader(int (*getCharA)(void *data), void *dataA) 
{
  getChar = getCharA;
  data = dataA;
  streamPos = 0;
  bufPos = 0;
  bufLen = 0;
}

StreamReader::~StreamReader() 
{
}

int StreamReader::getByte(int pos) 
{
  if (!fillBuf(pos, 1)) 
  {
    return -1;
  }
  return buf[pos - bufPos] & 0xff;
}

bool StreamReader::getU16BE(int pos, int *val) 
{
  if (!fillBuf(pos, 2)) 
  {
    return false;
  }
  *val = ((buf[pos - bufPos] & 0xff) << 8) + (buf[pos - bufPos + 1] & 0xff);
  return true;
}

bool StreamReader::getU32BE(int pos, uint *val) 
{
  if (!fillBuf(pos, 4)) 
  {
    return false;
  }
  *val = ((buf[pos - bufPos] & 0xff) << 24) +
         ((buf[pos - bufPos + 1] & 0xff) << 16) +
         ((buf[pos - bufPos + 2] & 0xff) << 8) +
         (buf[pos - bufPos + 3] & 0xff);
  return true;
}

bool StreamReader::getU32LE(int pos, uint *val) 
{
  if (!fillBuf(pos, 4)) 
  {
    return false;
  }
  *val = (buf[pos - bufPos] & 0xff) +
         ((buf[pos - bufPos + 1] & 0xff) << 8) +
         ((buf[pos - bufPos + 2] & 0xff) << 16) +
         ((buf[pos - bufPos + 3] & 0xff) << 24);
  return true;
}

bool StreamReader::getUVarBE(int pos, int size, uint *val) 
{
  int i;

  if (size < 1 || size > 4 || !fillBuf(pos, size)) 
  {
    return false;
  }
  *val = 0;
  for (i = 0; i < size; ++i) 
 	{
    *val = (*val << 8) + (buf[pos - bufPos + i] & 0xff);
  }
  return true;
}

bool StreamReader::cmp(int pos, const char *s) 
{
  int n;

  n = (int)strlen(s);
  if (!fillBuf(pos, n)) 
  {
    return false;
  }
  return !memcmp(buf - bufPos + pos, s, n);
}

bool StreamReader::fillBuf(int pos, int len) 
{
  int c;

  if (pos < 0 || len < 0 || len > (int)sizeof(buf) ||
      pos > INT_MAX - (int)sizeof(buf)) 
  {
    return false;
  }
  if (pos < bufPos) 
  {
    return false;
  }

  // if requested region will not fit in the current buffer...
  if (pos + len > bufPos + (int)sizeof(buf)) 
  {

    // if the start of the requested data is already in the buffer, move
    // it to the start of the buffer
    if (pos < bufPos + bufLen) 
    {
      bufLen -= pos - bufPos;
      memmove(buf, buf + (pos - bufPos), bufLen);
      bufPos = pos;

    // otherwise discard data from the
    // stream until we get to the requested position
    } 
    else 
    {
      bufPos += bufLen;
      bufLen = 0;
      while (bufPos < pos) 
     {
				if ((c = (*getChar)(data)) < 0) 
				{
	  			return false;
				}
				++bufPos;
      }
    }
  }

  // read the rest of the requested data
  while (bufPos + bufLen < pos + len) 
  {
    if ((c = (*getChar)(data)) < 0) 
    {
      return false;
    }
    buf[bufLen++] = (char)c;
  }

  return true;
}


static FoFiIdentifierType identify(Reader *reader);
static FoFiIdentifierType identifyOpenType(Reader *reader);
static FoFiIdentifierType identifyCFF(Reader *reader, int start);


FoFiIdentifierType XWFontFileIdentifier::identifyMem(char *file, int len) 
{
  MemReader *reader;
  FoFiIdentifierType type;

  if (!(reader = MemReader::make(file, len))) 
  {
    return fofiIdError;
  }
  type = identify(reader);
  delete reader;
  return type;
}


FoFiIdentifierType XWFontFileIdentifier::identifyFile(char *fileName) 
{
  FileReader *reader;
  FoFiIdentifierType type;

  if (!(reader = FileReader::make(fileName))) 
  {
    return fofiIdError;
  }
  type = identify(reader);
  delete reader;
  return type;
}


FoFiIdentifierType XWFontFileIdentifier::identifyStream(int (*getChar)(void *data), void *data) 
{
  StreamReader *reader;
  FoFiIdentifierType type;

  if (!(reader = StreamReader::make(getChar, data))) 
  {
    return fofiIdError;
  }
  type = identify(reader);
  delete reader;
  return type;
}


static FoFiIdentifierType identify(Reader *reader) 
{
  uint n;

  //----- PFA
  if (reader->cmp(0, "%!PS-AdobeFont-1") ||
      reader->cmp(0, "%!FontType1")) 
  {
    return fofiIdType1PFA;
  }

  //----- PFB
  if (reader->getByte(0) == 0x80 &&
      reader->getByte(1) == 0x01 &&
      reader->getU32LE(2, &n)) 
  {
    if ((n >= 16 && reader->cmp(6, "%!PS-AdobeFont-1")) || (n >= 11 && reader->cmp(6, "%!FontType1"))) 
		{
      return fofiIdType1PFB;
    }
  }

  //----- TrueType
  if ((reader->getByte(0) == 0x00 &&
       reader->getByte(1) == 0x01 &&
       reader->getByte(2) == 0x00 &&
       reader->getByte(3) == 0x00) ||
      (reader->getByte(0) == 0x74 &&	// 'true'
       reader->getByte(1) == 0x72 &&
       reader->getByte(2) == 0x75 &&
       reader->getByte(3) == 0x65)) 
  {
    return fofiIdTrueType;
  }
  if (reader->getByte(0) == 0x74 &&	// 'ttcf'
      reader->getByte(1) == 0x74 &&
      reader->getByte(2) == 0x63 &&
      reader->getByte(3) == 0x66) 
  {
    return fofiIdTrueTypeCollection;
  }

  //----- OpenType
  if (reader->getByte(0) == 0x4f &&	// 'OTTO
      reader->getByte(1) == 0x54 &&
      reader->getByte(2) == 0x54 &&
      reader->getByte(3) == 0x4f) 
  {
    return identifyOpenType(reader);
  }

  //----- CFF
  if (reader->getByte(0) == 0x01 &&
      reader->getByte(1) == 0x00) 
  {
    return identifyCFF(reader, 0);
  }
  // some tools embed CFF fonts with an extra whitespace char at the
  // beginning
  if (reader->getByte(1) == 0x01 &&
      reader->getByte(2) == 0x00) 
  {
    return identifyCFF(reader, 1);
  }

  return fofiIdUnknown;
}

static FoFiIdentifierType identifyOpenType(Reader *reader) 
{
  FoFiIdentifierType type;
  uint offset;
  int nTables, i;

  if (!reader->getU16BE(4, &nTables)) 
  {
    return fofiIdUnknown;
  }
  for (i = 0; i < nTables; ++i) 
 	{
    if (reader->cmp(12 + i*16, "CFF ")) 
    {
      if (reader->getU32BE(12 + i*16 + 8, &offset) && offset < (uint)INT_MAX) 
      {
				type = identifyCFF(reader, (int)offset);
				if (type == fofiIdCFF8Bit) 
				{
	  			type = fofiIdOpenTypeCFF8Bit;
				} 
				else if (type == fofiIdCFFCID) 
				{
	  			type = fofiIdOpenTypeCFFCID;
				}
				return type;
      }
      return fofiIdUnknown;
    }
  }
  return fofiIdUnknown;
}


static FoFiIdentifierType identifyCFF(Reader *reader, int start) 
{
  uint offset0, offset1;
  int hdrSize, offSize0, offSize1, pos, endPos, b0, n, i;

  //----- read the header
  if (reader->getByte(start) != 0x01 || reader->getByte(start + 1) != 0x00) 
  {
    return fofiIdUnknown;
  }
  if ((hdrSize = reader->getByte(start + 2)) < 0) 
  {
    return fofiIdUnknown;
  }
  if ((offSize0 = reader->getByte(start + 3)) < 1 || offSize0 > 4) 
  {
    return fofiIdUnknown;
  }
  pos = start + hdrSize;
  if (pos < 0) 
  {
    return fofiIdUnknown;
  }

  //----- skip the name index
  if (!reader->getU16BE(pos, &n)) 
  {
    return fofiIdUnknown;
  }
  if (n == 0) 
  {
    pos += 2;
  } 
  else 
  {
    if ((offSize1 = reader->getByte(pos + 2)) < 1 || offSize1 > 4) 
    {
      return fofiIdUnknown;
    }
    if (!reader->getUVarBE(pos + 3 + n * offSize1, offSize1, &offset1) || offset1 > (uint)INT_MAX) 
    {
      return fofiIdUnknown;
    }
    pos += 3 + (n + 1) * offSize1 + (int)offset1 - 1;
  }
  if (pos < 0) 
  {
    return fofiIdUnknown;
  }

  //----- parse the top dict index
  if (!reader->getU16BE(pos, &n) || n < 1) 
  {
    return fofiIdUnknown;
  }
  if ((offSize1 = reader->getByte(pos + 2)) < 1 || offSize1 > 4) 
  {
    return fofiIdUnknown;
  }
  if (!reader->getUVarBE(pos + 3, offSize1, &offset0) ||
      offset0 > (uint)INT_MAX ||
      !reader->getUVarBE(pos + 3 + offSize1, offSize1, &offset1) ||
      offset1 > (uint)INT_MAX ||
      offset0 > offset1) 
  {
    return fofiIdUnknown;
  }
  pos = pos + 3 + (n + 1) * offSize1 + (int)offset0 - 1;
  endPos = pos + 3 + (n + 1) * offSize1 + (int)offset1 - 1;
  if (pos < 0 || endPos < 0 || pos > endPos) 
  {
    return fofiIdUnknown;
  }
  
  //----- parse the top dict, look for ROS as first entry
  // for a CID font, the top dict starts with:
  //     <int> <int> <int> ROS
  for (i = 0; i < 3; ++i) 
  {
    b0 = reader->getByte(pos++);
    if (b0 == 0x1c) 
    {
      pos += 2;
    } 
    else if (b0 == 0x1d) 
    {
      pos += 4;
    } 
    else if (b0 >= 0xf7 && b0 <= 0xfe) 
    {
      pos += 1;
    } 
    else if (b0 < 0x20 || b0 > 0xf6) 
    {
      return fofiIdCFF8Bit;
    }
    if (pos >= endPos || pos < 0) 
    {
      return fofiIdCFF8Bit;
    }
  }
  if (pos + 1 < endPos &&
      reader->getByte(pos) == 12 &&
      reader->getByte(pos + 1) == 30) 
  {
    return fofiIdCFFCID;
  } 
  else 
  {
    return fofiIdCFF8Bit;
  }
}

XWFontFile::XWFontFile()
{
	file = 0;
	fileData = 0;
	freeFileData = false;
	fileFinal = 0;
  	nGlyphs = 0;
  	offset = 0;
  	len = 0;
  	ok = false;
  	
  ft_face = 0;
	loc = 0;
	ft_to_gid = 0;
}

XWFontFile::XWFontFile(QIODevice * fileA,
		 	           int    fileFinalA,
		 	           ulong  offsetA,
		 	           ulong  lenA)
{
	file = fileA;
	fileData = 0;
	freeFileData = false;
	fileFinal = fileFinalA;
  nGlyphs = 0;
  offset = offsetA;
  len = lenA;
  ok = false;
  ft_face = 0;
	loc = 0;
	ft_to_gid = 0;
  	if (file)
  		seek(0);
}

XWFontFile::XWFontFile(uchar *filedataA, int lenA, bool freeFileDataA)
{
	file = 0;
	fileData = filedataA;
	freeFileData = freeFileDataA;
	fileFinal = 0;
  nGlyphs = 0;
  offset = 0;
  len = lenA;
  ok = false;
  ft_face = 0;
	loc = 0;
	ft_to_gid = 0;
}

XWFontFile::~XWFontFile()
{
  	if (file)
  	{
  		if (fileFinal & FONTFILE_CLOSE)
  			file->close();
  			
  		if (fileFinal & FONTFILE_REMOVE)
  		{
  			QFile * f = (QFile*)file;
  			f->remove();
  		}
  			
  		if (fileFinal & FONTFILE_DEL)
  			delete file;
  	}
  	
  	if (freeFileData && fileData)
  		free(fileData);
  	
  	if (ft_to_gid)
  		delete [] ft_to_gid;
}

bool XWFontFile::checkRegion(ulong pos, ulong size)
{
	return pos >= 0 &&
         pos + size >= pos &&
         pos + size <= len;
}

ulong XWFontFile::ftread(uchar* buf, ulong lenA)
{
	ulong length = lenA;
	if (0 == FT_Load_Sfnt_Table(ft_face, 0, loc, buf, &length))
		loc += len;
		
	return length;
}

char  XWFontFile::ftsignedByte()
{
	int b = ftunsignedByte();
  if (b >= 0x80)
   	b -= 0x100;
  return (char)b;
}

short XWFontFile::ftsignedPair()
{
	int p = ftunsignedPair();
  if (p >= 0x8000U)
    p -= 0x10000U;
    
  return (short)p;
}

long  XWFontFile::ftsignedQuad()
{
	int byte = ftunsignedByte();
  	long quad = byte;
  	if (quad >= 0x80) 
    	quad = byte - 0x100;
    	
  	for (int i = 0; i < 3; i++) 
    	quad = quad * 0x100 + ftunsignedByte();
    	
  	return quad;
}

uchar XWFontFile::ftunsignedByte()
{
	uchar byte = 0;
	if (!ft_face)
		return byte;
		
  ulong length = 1;
  if (0 == FT_Load_Sfnt_Table(ft_face, 0, loc, &byte, &length))
  	loc += 1;
  return byte;
}

ushort XWFontFile::ftunsignedPair()
{
	uchar buf[2];
  ulong length = 2;
  
  if (0 == FT_Load_Sfnt_Table(ft_face, 0, loc, buf, &length))
  	loc += 2;
  	
  return (ushort)((unsigned)buf[0] << 8) + buf[1];
}

ulong  XWFontFile::ftunsignedQuad()
{
	uchar buf[4];
  ulong length = 4;
  if (0 == FT_Load_Sfnt_Table(ft_face, 0, loc, buf, &length))
  	loc += 4;
  	
  return ((ulong)buf[0] << 24) + ((ulong)buf[1] << 16)
       + ((ulong)buf[2] << 8) + (ulong)buf[3];
}

uchar XWFontFile::getByte()
{
	if (ft_face)
		return ftunsignedByte();
		
	if (fileData)
	{
		uchar x = fileData[loc++];
		return x;
	}
		
	return getUnsignedByte(file);
}

uchar XWFontFile::getCard8()
{
	if (ft_face)
		return ftunsignedByte();
		
	if (fileData)
	{
		uchar x = fileData[loc++];
		return x;
	}
		
	return getUnsignedByte(file);
}

ushort XWFontFile::getCard16()
{
	if (ft_face)
		return ftunsignedPair();
		
	if (fileData)
	{
		int x = fileData[loc++];
		x = (x << 8) + fileData[loc++];
		return (ushort)x;
	}
		
	return getUnsignedPair(file);
}

char XWFontFile::getChar()
{
	if (ft_face)
		return ftsignedByte();
		
	if (fileData)
	{
		int x = fileData[loc++];
		if (x >= 0x80) 
    	x -= 0x100;
    	
  	return (char)x;
	}
		
	return getSignedByte(file);
}

long XWFontFile::getLong()
{
	if (ft_face)
		return ftsignedQuad();
		
	if (fileData)
	{
		long x;
  	x = fileData[loc++];
  	if (x >= 0x80) 
    	x = x - 0x100;
  	x = (x << 8) + fileData[loc++];
  	x = (x << 8) + fileData[loc++];
  	x = (x << 8) + fileData[loc++];
  	return x;
	}
		
	return getSignedQuad(file);
}

ulong XWFontFile::getOffset(long n)
{
	ulong v = 0;
	while (n-- > 0)
    	v = v * 0x100u + getUnsignedByte(file);

  	return v;
}

short XWFontFile::getShort()
{
	if (ft_face)
		return ftsignedPair();
		
	if (fileData)
	{
		long x = fileData[loc++];
  	x = (x << 8) + fileData[loc++];
  	if (x >= 0x8000) 
    	x -= 0x10000l;
  	return (short)x;
	}
		
	return getSignedPair(file);
}

XWString * XWFontFile::getString(ulong offsetA, ulong lenA)
{
	seek(offsetA);
	char * buf = new char[lenA + 1];
	read(buf, lenA);
	XWString * ret = new XWString(buf, lenA);
	delete [] buf;
	return ret;
}

char XWFontFile::getS8(int pos, bool * ok)
{
	if (fileData)
	{
		int x;

  	if (pos < 0 || pos >= len) 
  	{
    	*ok = false;
    	return 0;
  	}
  	x = fileData[pos];
  	if (x & 0x80) 
  	{
    	x |= ~0xff;
  	}
  	return x;
	}
	
	*ok = seek(pos);
	if (!(*ok)) 
    	return -1;
    	
  if (ft_face)
		return ftsignedByte();
  	
  return getSignedByte(file);
}

short XWFontFile::getS16BE(int pos, bool * ok)
{
	if (fileData)
	{
		int x;

  	if (pos < 0 || pos+1 >= len || pos > INT_MAX - 1) 
  	{
    	*ok = false;
    	return 0;
  	}
  	x = fileData[pos];
  	x = (x << 8) + fileData[pos+1];
  	if (x & 0x8000) 
  	{
    	x |= ~0xffff;
  	}
  	return x;
	}
	
	*ok = seek(pos);
	if (!(*ok))  
    	return -1;
    	
  if (ft_face)
		return ftsignedPair();
  	
  	return getSignedPair(file);
}

long XWFontFile::getS32BE(int pos, bool * ok)
{
	if (fileData)
	{
		int x;

  	if (pos < 0 || pos+3 >= len || pos > INT_MAX - 3) 
  	{
    	*ok = false;
    	return 0;
  	}
  	x = fileData[pos];
  	x = (x << 8) + fileData[pos+1];
 	 	x = (x << 8) + fileData[pos+2];
  	x = (x << 8) + fileData[pos+3];
  	if (x & 0x80000000) 
  	{
    	x |= ~0xffffffff;
  	}
  	return x;
	}
	
	*ok = seek(pos);
	if (!(*ok)) 
    	return -1;
    	
  if (ft_face)
		return ftsignedQuad();
  	
  	return getSignedQuad(file);
}

ulong  XWFontFile::getULong()
{
	if (ft_face)
		return ftunsignedQuad();
		
	if (fileData)
	{
		uint x;
  	x = fileData[loc++];
  	x = (x << 8) + fileData[loc++];
  	x = (x << 8) + fileData[loc++];
  	x = (x << 8) + fileData[loc++];
  	return x;
	}
		
	return getUnsignedQuad(file);
}

ushort XWFontFile::getUShort()
{
	if (ft_face)
		return ftunsignedPair();
		
	if (fileData)
	{
		int x = fileData[loc++];
		x = (x << 8) + fileData[loc++];
		return (ushort)x;
	}
		
	return getUnsignedPair(file);
}

uchar XWFontFile::getU8(int pos, bool * ok)
{
	if (fileData)
	{
		if (pos < 0 || pos >= len) 
		{
    	*ok = false;
    	return 0;
  	}
  	return fileData[pos];
	}
	
	*ok = seek(pos);
	if (!(*ok)) 
    	return 0;
  	
  if (ft_face)
		return ftunsignedByte();
		
  	return getUnsignedByte(file);
}

int XWFontFile::getU16BE(int pos, bool * ok)
{
	if (fileData)
	{
		int x;

  	if (pos < 0 || pos+1 >= len || pos > INT_MAX - 1) 
  	{
    	*ok = false;
    	return 0;
  	}
  	x = fileData[pos];
  	x = (x << 8) + fileData[pos+1];
  	return x;
	}
	
	*ok = seek(pos);
	if (!(*ok)) 
    	return 0;
    	
  if (ft_face)
		return ftunsignedPair();
  	
  	return (int)getUnsignedPair(file);
}

ulong XWFontFile::getU32BE(int pos, bool * ok)
{
	if (fileData)
	{
		uint x;

  	if (pos < 0 || pos+3 >= len || pos > INT_MAX - 3) 
  	{
    	*ok = false;
    	return 0;
  	}
  	x = fileData[pos];
  	x = (x << 8) + fileData[pos+1];
  	x = (x << 8) + fileData[pos+2];
  	x = (x << 8) + fileData[pos+3];
  	return x;
	}
	
	*ok = seek(pos);
	if (!(*ok)) 
    	return 0;
    	
  if (ft_face)
		return ftunsignedQuad();
  	
  	return getUnsignedQuad(file);
}

uint XWFontFile::getU32LE(int pos, bool *ok)
{
	uint x = 0;
	if (fileData)
	{
		if (pos < 0 || pos+3 >= len || pos > INT_MAX - 3) 
		{
    	*ok = false;
    	return 0;
  	}
  	x = fileData[pos+3];
  	x = (x << 8) + fileData[pos+2];
  	x = (x << 8) + fileData[pos+1];
  	x = (x << 8) + fileData[pos];
  	return x;
	}
	
	*ok = seek(pos + 3);
	if (!(*ok)) 
    	return 0;
  
  x = getByte();
  *ok = seek(pos + 2);
  x = (x << 8) + getByte();
  *ok = seek(pos + 1);
  x = (x << 8) + getByte();
  *ok = seek(pos);
  x = (x << 8) + getByte();
  return x;
}

ulong XWFontFile::getUVarBE(int pos, int size, bool * ok)
{
	uint x = 0;
  int i = 0;
	if (fileData)
	{
		if (pos < 0 || pos + size > len || pos > INT_MAX - size) 
		{
    	*ok = false;
    	return 0;
  	}
  	
  	for (; i < size; ++i) 
  	{
    	x = (x << 8) + fileData[pos + i];
  	}
  	return x;
	}
	
	*ok = seek(pos);
	if (!(*ok)) 
    	return 0;
  
  	for (; i < size; ++i) 
    	x = (x << 8) + getByte();
  	return x;
}

void XWFontFile::output(FontFileOutputFunc outputFunc, 
                        void *outputStream, 
                        ulong offsetA, 
                        ulong lenA)
{
	seek(offsetA);
	char * buf = new char[lenA + 1];
	read(buf, lenA);
	(*outputFunc)(outputStream, buf, lenA);
	delete [] buf;
}

int XWFontFile::putBigEndian(void *s, long q, int n)
{
	char * p = (char *) s;
  	for (int i = n - 1; i >= 0; i--) 
  	{
    	p[i] = (char) (q & 0xff);
    	q >>= 8;
  	}

  	return n;
}

long XWFontFile::read(char* buf, long lenA)
{
	if (ft_face)
		return (long)(ftread((uchar*)buf, lenA));
		
	if (fileData)
	{
		memcpy(buf, fileData, lenA);
		return lenA;
	}
		
	return (long)(file->read(buf, lenA));
}

bool XWFontFile::seekAbsolute(ulong offsetA)
{
	if (ft_face || fileData)
	{
		loc = offsetA;
		return true;
	}
		
	 return file->seek(offsetA);
}

bool XWFontFile::seek(ulong offsetA)
{
	if (ft_face || fileData)
	{
		loc = offsetA;
		return true;
	}
		
	return file->seek(offsetA + offset);
}

ulong XWFontFile::tellPosition()
{
	if (ft_face || fileData)
		return loc;
		
	return ((ulong)(file->pos()));
}

QFile * XWFontFile::open(char *fileName, XWFontSea::FileFormat fmtA)
{
	QString fn = QFile::decodeName(fileName);
	XWFontSea sea;
	QFile * ret = sea.openFile(fn, fmtA);
	if (!ret && !fn.contains(QChar('.')) && !fn.contains(QChar('/')))
	{
		XWString * cstr = new XWString(fileName, strlen(fileName));
		XWFontSetting fontsetting;
		XWString * filename = fontsetting.findFontFile(cstr);
		if (!filename)
		{
			SysFontType sysFontType;
			int fontNum = 0;
			filename = fontsetting.findSystemFontFile(cstr, &sysFontType, &fontNum);
		}
		
		if (filename)
		{
			fn = filename->toQString();		
			switch (fmtA)
			{
				case XWFontSea::TrueType:
					if (fn.endsWith(".ttf", Qt::CaseInsensitive) || 
					  fn.endsWith(".ttc", Qt::CaseInsensitive))
					{
						ret = sea.openFile(fn, fmtA);
					}
					break;
				
				case XWFontSea::OpenType:
					if (fn.endsWith(".ttf", Qt::CaseInsensitive) || 
					  	fn.endsWith(".ttc", Qt::CaseInsensitive) ||
					  	fn.endsWith(".otf", Qt::CaseInsensitive))
					{
						ret = sea.openFile(fn, fmtA);
					}
					break;
				
				case XWFontSea::Type1:
					if (fn.endsWith(".pfa", Qt::CaseInsensitive) || 
					  	fn.endsWith(".pfb", Qt::CaseInsensitive) ||
					  	fn.endsWith(".ps", Qt::CaseInsensitive))
					{
						ret = sea.openFile(fn, fmtA);
					}
					break;
				
				default:
					break;
			}
		
			delete filename;
			filename = 0;
		}
	
		delete cstr;
	}
		
	return ret;
}

