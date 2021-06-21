/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <QByteArray>
#include <QBuffer>

#include "XWString.h"
#include "XWApplication.h"
#include "XWFontEncoding.h"
#include "XWPFBParser.h"
#include "XWFontFileType1.h"


XWFontFileType1::XWFontFileType1(QIODevice * fileA,
		 	                     int    fileFinalA,
		 	                      ulong offsetA,
		 	                      ulong lenA)
	:XWFontFile(fileA, fileFinalA, offsetA, lenA)
{
  name = 0;
  encoding = 0;
  parsed = false;
  fontMatrix[0] = 0.001;
  fontMatrix[1] = 0;
  fontMatrix[2] = 0;
  fontMatrix[3] = 0.001;
  fontMatrix[4] = 0;
  fontMatrix[5] = 0;
}

XWFontFileType1::XWFontFileType1(uchar *filedataA, int lenA, bool freeFileDataA)
	:XWFontFile(filedataA, lenA, freeFileDataA)
{
	name = 0;
  encoding = 0;
  parsed = false;
  fontMatrix[0] = 0.001;
  fontMatrix[1] = 0;
  fontMatrix[2] = 0;
  fontMatrix[3] = 0.001;
  fontMatrix[4] = 0;
  fontMatrix[5] = 0;
}

XWFontFileType1::~XWFontFileType1()
{
    if (name) 
        delete [] name;
        
    if (encoding && encoding != type1StandardEncoding) 
    {
        for (int i = 0; i < 256; ++i) 
            delete [] encoding[i];
            
        free(encoding);
    }
}

void XWFontFileType1::decrypt(ushort key, 
	                          uchar *dst, 
	                          const uchar *src,
	                          long skip, 
	                          long lenA)
{
	lenA -= skip;
  	while (skip--)
    	key = (key + *src++) * 52845u + 22719u;
  	while (lenA--) 
  	{
    	uchar c = *src++;
    	*dst++ = (c ^ (key >> 8));
    	key = (key + c) * 52845u + 22719u;
  	}
}

char ** XWFontFileType1::getEncoding()
{
    if (!parsed) 
        parse();
        
    return encoding;
}

void XWFontFileType1::getFontMatrix(double *mat)
{
	int i;

  if (!parsed) {
    parse();
  }
  for (i = 0; i < 6; ++i) {
    mat[i] = fontMatrix[i];
  }
}

int XWFontFileType1::getFontName(char *fontnameA)
{
	long  lengthA = 0;
	seek(0);
	uchar * buffer = getPFBSegment(PFB_SEG_TYPE_ASCII, &lengthA);
	if (buffer == 0 || lengthA == 0)
	{
    	xwApp->error(tr("reading PFB (ASCII part) file failed.\n"));
    	return -1;
    }
    
    XWPFBParser parser((const char*)buffer, lengthA);
    if (!parser.seekOperator("begin"))
    {
    	free(buffer);
    	return -1;
    }
    
    int   fn_found = 0;
    const char *key = 0;
    while (!fn_found && (key = parser.getNextKey()) != 0)
    {
    	if (!strcmp(key, "FontName"))
    	{
    		const char *strval = parser.getString();
    		if (!strval)
    		{
    			free(buffer);
    			return-1;
    		}
    		long lenA = qMin((long)strlen(strval), (long)PFB_NAME_LEN_MAX);
    		strncpy(fontnameA, strval, lenA);
    		fontnameA[lenA] = '\0';
    		fn_found = 1;
    	}
    }
    
    free(buffer);

  	return 0;
}

char * XWFontFileType1::getName()
{
    if (!parsed) 
        parse();
        
    return name;
}

uchar * XWFontFileType1::getPFBSegment(int expected_type, 
                                       long *length)
{
	uchar *buffer = 0;
	long bytesread = 0;
	while (!atEnd())
	{
		int ch = getByte();
		if (ch != 128)
		{
			xwApp->error(tr("not a pfb file?\n"));
			goto badfile;
		}
		
		if (atEnd())
			break;
			
		ch = getByte();
		if (ch != expected_type) 
		{
			long pos = (long)(file->pos());
			pos -= 2;
			file->seek(pos);
      		break;
    	}
    	
    	long slen = 0;
      	for (int i = 0; i < 4; i++)
      	{
      		if (atEnd())
      			goto badfile;
				
      		ch = getByte();
      		slen = slen + (ch << (8*i));
      	}
      	
      	buffer = (uchar*)realloc(buffer, (bytesread + slen) * sizeof(uchar));
      	while (slen > 0)
      	{
      		long rlen = read((char*)(buffer + bytesread), slen);
      		if (rlen < 0) 
      			goto badfile;
			slen -= rlen;
			bytesread += rlen;
      	}
	}
	
	if (bytesread == 0) 
	{
    	xwApp->error(tr("PFB segment length zero?\n"));
    	goto badfile;
  	}
  	
  	buffer = (uchar*)realloc(buffer, (bytesread+1) * sizeof(uchar));
  	buffer[bytesread] = 0;

  	if (length)
    	*length = bytesread;
  	return buffer;
  	
badfile:
	if (buffer)
	   	free(buffer);
	if (expected_type == PFB_SEG_TYPE_ASCII)
		xwApp->error(tr("reading PFB (ASCII part) file failed.\n"));
	else
		xwApp->error(tr("reading PFB (BINARY part) file failed.\n"));
	return 0;
}

bool XWFontFileType1::isPFB(QIODevice * fileA)
{
	if (!fileA)
		return false;
		
	fileA->seek(0);
	
	char c;
	if (!fileA->getChar(&c) || ((uchar)(c) != 128))
		return false;
		
	if (!fileA->getChar(&c) || c > 3)
		return false;
		
	for (int i = 0; i < 4; i++) 
	{
    	if (!fileA->getChar(&c)) 
      		return false;
  	}
  	
  	char sig[15];
  	for (int i = 0; i < 14; i++) 
  	{
    	if (!fileA->getChar(&c)) 
      		return false;
      		
    	sig[i] = c;
  	}
  	
  	if (!memcmp(sig, "%!PS-AdobeFont", 14) || !memcmp(sig, "%!FontType1", 11))
  		return true;
  		
  	if (!memcmp(sig, "%!PS", 4))
  		return true;
  		
  	return false;
}

XWFontFileType1 * XWFontFileType1::load(char *fileName)
{
	QFile * fileA = open(fileName, XWFontSea::Type1);
	if (!fileA)
		return 0;
		
	int lenA = (int)(fileA->size());
	uchar * buf = (uchar*)malloc((lenA + 1) * sizeof(uchar));
	fileA->read((char*)buf, lenA);
	fileA->close();
	delete fileA;
  XWFontFileType1 * ff = new XWFontFileType1(buf, lenA, true);
  ff->undoPFB();
  return ff;
}

XWFontFileType1 * XWFontFileType1::loadPFB(char *fileName, bool deleteFileA)
{
	QFile * fileA = open(fileName, XWFontSea::Type1);
	if (!fileA)
		return 0;
	
	if (!isPFB(fileA))
	{
		fileA->close();
		delete fileA;
		return 0;
	}
	
	int fileFinalA = FONTFILE_CLOSE | FONTFILE_DEL;
	if (deleteFileA)
		fileFinalA |= FONTFILE_REMOVE;
        
    return new XWFontFileType1(fileA, fileFinalA, 0, (ulong)(fileA->size()));
}

XWFontFileType1 * XWFontFileType1::make(QIODevice * fileA, ulong offsetA, ulong lenA)
{
	return new XWFontFileType1(fileA, 0, offsetA, lenA);
}

XWFontFileType1 * XWFontFileType1::make(char *buf, int lenA)
{
  XWFontFileType1 * ff = new XWFontFileType1((uchar*)buf, lenA, false);
  ff->undoPFB();
  return ff;
}

void XWFontFileType1::writeEncoded(char **newEncoding,
		                           FontFileOutputFunc outputFunc, 
		                           void *outputStream)
{
	char buf[512];
  char *line, *line2, *p;
  int i;

  // copy everything up to the encoding
  for (line = (char *)fileData;
       line && strncmp(line, "/Encoding", 9);
       line = getNextLine(line)) ;
  if (!line) {
    // no encoding - just copy the whole font file
    (*outputFunc)(outputStream, (char *)fileData, len);
    return;
  }
  (*outputFunc)(outputStream, (char *)fileData, (int)(line - (char *)fileData));

  // write the new encoding
  (*outputFunc)(outputStream, "/Encoding 256 array\n", 20);
  (*outputFunc)(outputStream,
		"0 1 255 {1 index exch /.notdef put} for\n", 40);
  for (i = 0; i < 256; ++i) {
    if (newEncoding[i]) {
      sprintf(buf, "dup %d /%s put\n", i, newEncoding[i]);
      (*outputFunc)(outputStream, buf, (int)strlen(buf));
    }
  }
  (*outputFunc)(outputStream, "readonly def\n", 13);
  
  // find the end of the encoding data
  //~ this ought to parse PostScript tokens
  if (!strncmp(line, "/Encoding StandardEncoding def", 30)) {
    line = getNextLine(line);
  } else {
    // skip "/Encoding" + one whitespace char,
    // then look for 'def' preceded by PostScript whitespace
    p = line + 10;
    line = NULL;
    for (; p < (char *)fileData + len; ++p) {
      if ((*p == ' ' || *p == '\t' || *p == '\x0a' ||
	   *p == '\x0d' || *p == '\x0c' || *p == '\0') &&
	  p + 4 <= (char *)fileData + len &&
	  !strncmp(p + 1, "def", 3)) {
	line = p + 4;
	break;
      }
    }
  }

  // some fonts have two /Encoding entries in their dictionary, so we
  // check for a second one here
  if (line) {
    for (line2 = line, i = 0;
	 i < 20 && line2 && strncmp(line2, "/Encoding", 9);
	 line2 = getNextLine(line2), ++i) ;
    if (i < 20 && line2) {
      (*outputFunc)(outputStream, line, (int)(line2 - line));
      if (!strncmp(line2, "/Encoding StandardEncoding def", 30)) {
	line = getNextLine(line2);
      } else {
	// skip "/Encoding" + one whitespace char,
	// then look for 'def' preceded by PostScript whitespace
	p = line2 + 10;
	line = NULL;
	for (; p < (char *)fileData + len; ++p) {
	  if ((*p == ' ' || *p == '\t' || *p == '\x0a' ||
	       *p == '\x0d' || *p == '\x0c' || *p == '\0') &&
	      p + 4 <= (char *)fileData + len &&
	      !strncmp(p + 1, "def", 3)) {
	    line = p + 4;
	    break;
	  }
	}
      }
    }

    // copy everything after the encoding
    if (line) {
      (*outputFunc)(outputStream, line, (int)(((char *)fileData + len) - line));
    }
  }
}

char * XWFontFileType1::getNextLine(char *line)
{
    while (line < (char *)fileData + len && *line != '\x0a' && *line != '\x0d') 
        ++line;
        
    if (line < (char *)fileData + len && *line == '\x0d') 
        ++line;
        
    if (line < (char *)fileData + len && *line == '\x0a') 
        ++line;
    
    if (line >= (char *)fileData + len) 
        return (char*)0;
        
    return line;
}

void XWFontFileType1::parse()
{
	char *line, *line1, *p, *p2;
  char buf[256];
  char c;
  int n, code, base, i, j;
  bool gotMatrix;

  gotMatrix = false;
  for (i = 1, line = (char *)fileData;
       i <= 100 && line && (!name || !encoding);
       ++i) {

    // get font name
    if (!name && !strncmp(line, "/FontName", 9)) {
      strncpy(buf, line, 255);
      buf[255] = '\0';
      if ((p = strchr(buf+9, '/')) &&
	  (p = strtok(p+1, " \t\n\r"))) {
	name = qstrdup(p);
      }
      line = getNextLine(line);

    // get encoding
    } else if (!encoding &&
	       !strncmp(line, "/Encoding StandardEncoding def", 30)) {
      encoding = (char **)type1StandardEncoding;
    } else if (!encoding &&
	       !strncmp(line, "/Encoding 256 array", 19)) {
      encoding = (char **)malloc(256 * sizeof(char *));
      for (j = 0; j < 256; ++j) {
	encoding[j] = NULL;
      }
      for (j = 0, line = getNextLine(line);
	   j < 300 && line && (line1 = getNextLine(line));
	   ++j, line = line1) {
	if ((n = (int)(line1 - line)) > 255) {
	  n = 255;
	}
	strncpy(buf, line, n);
	buf[n] = '\0';
	for (p = buf; *p == ' ' || *p == '\t'; ++p) ;
	if (!strncmp(p, "dup", 3)) {
	  while (1) {
	    p += 3;
	    for (; *p == ' ' || *p == '\t'; ++p) ;
	    code = 0;
	    if (*p == '8' && p[1] == '#') {
	      base = 8;
	      p += 2;
	    } else if (*p >= '0' && *p <= '9') {
	      base = 10;
	    } else {
	      break;
	    }
	    for (; *p >= '0' && *p < '0' + base; ++p) {
	      code = code * base + (*p - '0');
	    }
	    for (; *p == ' ' || *p == '\t'; ++p) ;
	    if (*p != '/') {
	      break;
	    }
	    ++p;
	    for (p2 = p; *p2 && *p2 != ' ' && *p2 != '\t'; ++p2) ;
	    if (code >= 0 && code < 256) {
	      c = *p2;
	      *p2 = '\0';
	      encoding[code] = qstrdup(p);
	      *p2 = c;
	    }
	    for (p = p2; *p == ' ' || *p == '\t'; ++p) ;
	    if (strncmp(p, "put", 3)) {
	      break;
	    }
	    for (p += 3; *p == ' ' || *p == '\t'; ++p) ;
	    if (strncmp(p, "dup", 3)) {
	      break;
	    }
	  }
	} else {
	  if (strtok(buf, " \t") &&
	      (p = strtok(NULL, " \t\n\r")) && !strcmp(p, "def")) {
	    break;
	  }
	}
      }
      //~ check for getinterval/putinterval junk

    } else if (!gotMatrix && !strncmp(line, "/FontMatrix", 11)) {
      strncpy(buf, line + 11, 255);
      buf[255] = '\0';
      if ((p = strchr(buf, '['))) {
	++p;
	if ((p2 = strchr(p, ']'))) {
	  *p2 = '\0';
	  for (j = 0; j < 6; ++j) {
	    if ((p = strtok(j == 0 ? p : (char *)NULL, " \t\n\r"))) {
	      fontMatrix[j] = atof(p);
	    } else {
	      break;
	    }
	  }
	}
      }
      gotMatrix = true;

    } else {
      line = getNextLine(line);
    }
  }

  parsed = true;
}

void XWFontFileType1::undoPFB()
{
	bool ok;
  uchar *file2;
  int pos1, pos2, type;
  uint segLen;

  ok = true;
  if (getU8(0, &ok) != 0x80 || !ok) {
    return;
  }
  file2 = (uchar *)malloc(len);
  pos1 = pos2 = 0;
  while (getU8(pos1, &ok) == 0x80 && ok) {
    type = getU8(pos1 + 1, &ok);
    if (type < 1 || type > 2 || !ok) {
      break;
    }
    segLen = getU32LE(pos1 + 2, &ok);
    pos1 += 6;
    if (!ok || !checkRegion(pos1, segLen)) {
      break;
    }
    memcpy(file2 + pos2, fileData + pos1, segLen);
    pos1 += segLen;
    pos2 += segLen;
  }
  if (freeFileData) {
    free(fileData);
  }
  fileData = file2;
  freeFileData = true;
  len = pos2;
}
