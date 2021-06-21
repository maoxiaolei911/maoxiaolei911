/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include "XWApplication.h"
#include "XWObject.h"
#include "XWStream.h"
#include "XWPDFLexer.h"
#include "XWPDFParser.h"
#include "XWDict.h"
#include "XWPDFXRef.h"


#define permPrint    (1<<2)
#define permChange   (1<<3)
#define permCopy     (1<<4)
#define permNotes    (1<<5)
#define defPermFlags 0xfffc

#define xrefSearchSize 1024

class XWPDFObjectStream
{
public:
    XWPDFObjectStream(XWPDFXRef *xref, int objStrNumA);
    ~XWPDFObjectStream();
    
    XWObject * getObject(int objIdx, int objNum, XWObject *obj);
    int        getObjStrNum() { return objStrNum; }
    
    bool isOk() { return ok; }
    
private:
    int objStrNum;
    int nObjects;
    XWObject *objs;
    int *objNums;
    bool ok;
};

XWPDFObjectStream::XWPDFObjectStream(XWPDFXRef *xref, int objStrNumA)
{
  XWStream *str;
  XWPDFParser *parser;
  int *offsets;
  XWObject objStr, obj1, obj2;
  int first, i;

  objStrNum = objStrNumA;
  nObjects = 0;
  objs = NULL;
  objNums = NULL;
  ok = false;

  if (!xref->fetch(objStrNum, 0, &objStr)->isStream()) {
    goto err1;
  }

  if (!objStr.streamGetDict()->lookup("N", &obj1)->isInt()) {
    obj1.free();
    goto err1;
  }
  nObjects = obj1.getInt();
  obj1.free();
  if (nObjects <= 0) {
    goto err1;
  }

  if (!objStr.streamGetDict()->lookup("First", &obj1)->isInt()) {
    obj1.free();
    goto err1;
  }
  first = obj1.getInt();
  obj1.free();
  if (first < 0) {
    goto err1;
  }

  // this is an arbitrary limit to avoid integer overflow problems
  // in the 'new Object[nObjects]' call (Acrobat apparently limits
  // object streams to 100-200 objects)
  if (nObjects > 1000000) {
    xwApp->error("Too many objects in an object stream");
    goto err1;
  }
  objs = new XWObject[nObjects];
  objNums = (int *)malloc(nObjects * sizeof(int));
  offsets = (int *)malloc(nObjects * sizeof(int));

  // parse the header: object numbers and offsets
  objStr.streamReset();
  obj1.initNull();
  str = new XWEmbedStream(objStr.getStream(), &obj1, true, first);
  parser = new XWPDFParser(xref, new XWPDFLexer(xref, str), false);
  for (i = 0; i < nObjects; ++i) {
    parser->getObj(&obj1, true);
    parser->getObj(&obj2, true);
    if (!obj1.isInt() || !obj2.isInt()) {
      obj1.free();
      obj2.free();
      delete parser;
      free(offsets);
      goto err1;
    }
    objNums[i] = obj1.getInt();
    offsets[i] = obj2.getInt();
    obj1.free();
    obj2.free();
    if (objNums[i] < 0 || offsets[i] < 0 ||
	(i > 0 && offsets[i] < offsets[i-1])) {
      delete parser;
      free(offsets);
      goto err1;
    }
  }
  while (str->getChar() != EOF) ;
  delete parser;

  // skip to the first object - this shouldn't be necessary because
  // the First key is supposed to be equal to offsets[0], but just in
  // case...
  for (i = first; i < offsets[0]; ++i) {
    objStr.getStream()->getChar();
  }

  // parse the objects
  for (i = 0; i < nObjects; ++i) {
    obj1.initNull();
    if (i == nObjects - 1) {
      str = new XWEmbedStream(objStr.getStream(), &obj1, false, 0);
    } else {
      str = new XWEmbedStream(objStr.getStream(), &obj1, true,
			    offsets[i+1] - offsets[i]);
    }
    parser = new XWPDFParser(xref, new XWPDFLexer(xref, str), false);
    parser->getObj(&objs[i]);
    while (str->getChar() != EOF) ;
    delete parser;
  }

  free(offsets);
  ok = true;

 err1:
  objStr.free();
}

XWPDFObjectStream::~XWPDFObjectStream()
{
  int i;

  if (objs) {
    for (i = 0; i < nObjects; ++i) {
      objs[i].free();
    }
    delete[] objs;
  }
  
  if (objNums)
  	free(objNums);
}

XWObject * XWPDFObjectStream::getObject(int objIdx, int objNum, XWObject *obj)
{
  if (objIdx < 0 || objIdx >= nObjects || objNum != objNums[objIdx]) {
    return obj->initNull();
  }
  return objs[objIdx].copy(obj);
}

XWPDFXRef::XWPDFXRef(XWBaseStream *strA, bool repair)
{
  uint pos;
  XWObject obj;

  ok = true;
  size = 0;
  last = -1;
  entries = NULL;
  streamEnds = NULL;
  streamEndsLen = 0;
  objStr = NULL;

  encrypted = false;
  permFlags = defPermFlags;
  ownerPasswordOk = false;

  str = strA;
  start = str->getStart();

  // if the 'repair' flag is set, try to reconstruct the xref table
  if (repair) {
    if (!(ok = constructXRef())) {
      return;
    }

  // if the 'repair' flag is not set, read the xref table
  } else {

    // read the trailer
    pos = getStartXref();
    if (pos == 0) {
      ok = false;
      return;
    }

    // read the xref table
    while (readXRef(&pos)) ;
    if (!ok) {
      return;
    }
  }

  // get the root dictionary (catalog) object
  trailerDict.dictLookupNF("Root", &obj);
  if (obj.isRef()) {
    rootNum = obj.getRefNum();
    rootGen = obj.getRefGen();
    obj.free();
  } else {
    obj.free();
    if (!(ok = constructXRef())) {
      return;
    }
  }

  // now set the trailer dictionary's xref pointer so we can fetch
  // indirect objects from it
  trailerDict.getDict()->setXRef(this);
}

XWPDFXRef::~XWPDFXRef()
{
	if (entries)
  	free(entries);
  trailerDict.free();
  if (streamEnds) {
    free(streamEnds);
  }
  if (objStr) {
    delete objStr;
  }
}

XWObject * XWPDFXRef::fetch(int num, int gen, XWObject *obj, int recursion)
{
	XWPDFXRefEntry *e;
  XWPDFParser *parser;
  XWObject obj1, obj2, obj3;

  // check for bogus ref - this can happen in corrupted PDF files
  if (num < 0 || num >= size) {
    goto err;
  }

  e = &entries[num];
  switch (e->type) {

  case xrefEntryUncompressed:
    if (e->gen != gen) {
      goto err;
    }
    obj1.initNull();
    parser = new XWPDFParser(this,
	       new XWPDFLexer(this,
		 str->makeSubStream(start + e->offset, false, 0, &obj1)),
	       true);
    parser->getObj(&obj1, true);
    parser->getObj(&obj2, true);
    parser->getObj(&obj3, true);
    if (!obj1.isInt() || obj1.getInt() != num ||
	!obj2.isInt() || obj2.getInt() != gen ||
	!obj3.isCmd("obj")) {
      obj1.free();
      obj2.free();
      obj3.free();
      delete parser;
      goto err;
    }
    parser->getObj(obj, false, encrypted ? fileKey : (uchar *)NULL,
		   encAlgorithm, keyLength, num, gen, recursion);
    obj1.free();
    obj2.free();
    obj3.free();
    delete parser;
    break;

  case xrefEntryCompressed:
#if 0 // Adobe apparently ignores the generation number on compressed objects
    if (gen != 0) {
      goto err;
    }
#endif
    if (e->offset >= (uint)size ||
	entries[e->offset].type != xrefEntryUncompressed) {
      xwApp->error("Invalid object stream");
      goto err;
    }
    if (!objStr || objStr->getObjStrNum() != (int)e->offset) {
      if (objStr) {
	delete objStr;
      }
      objStr = new XWPDFObjectStream(this, e->offset);
      if (!objStr->isOk()) {
	delete objStr;
	objStr = NULL;
	goto err;
      }
    }
    objStr->getObject(e->gen, num, obj);
    break;

  default:
    goto err;
  }

  return obj;

 err:
  return obj->initNull();
}

XWObject * XWPDFXRef::getDocInfo(XWObject *obj)
{
  return trailerDict.dictLookup("Info", obj);
}

XWObject * XWPDFXRef::getDocInfoNF(XWObject *obj)
{
   return trailerDict.dictLookupNF("Info", obj);
}

bool  XWPDFXRef::getStreamEnd(uint streamStart, uint *streamEnd)
{
  int a, b, m;

  if (streamEndsLen == 0 ||
      streamStart > streamEnds[streamEndsLen - 1]) {
    return false;
  }

  a = -1;
  b = streamEndsLen - 1;
  // invariant: streamEnds[a] < streamStart <= streamEnds[b]
  while (b - a > 1) {
    m = (a + b) / 2;
    if (streamStart <= streamEnds[m]) {
      b = m;
    } else {
      a = m;
    }
  }
  *streamEnd = streamEnds[b];
  return true;
}

bool XWPDFXRef::okToAddNotes(bool ignoreOwnerPW)
{
  return (!ignoreOwnerPW && ownerPasswordOk) || (permFlags & permNotes);
}

bool XWPDFXRef::okToChange(bool ignoreOwnerPW)
{
  return (!ignoreOwnerPW && ownerPasswordOk) || (permFlags & permChange);
}

bool XWPDFXRef::okToCopy(bool ignoreOwnerPW)
{
  return (!ignoreOwnerPW && ownerPasswordOk) || (permFlags & permCopy);
}

bool XWPDFXRef::okToPrint(bool ignoreOwnerPW)
{
  return (!ignoreOwnerPW && ownerPasswordOk) || (permFlags & permPrint);
}

void XWPDFXRef::setEncryption(int permFlagsA, 
                              bool ownerPasswordOkA,
		                      uchar *fileKeyA, 
		                      int keyLengthA, 
		                      int encVersionA,
		                      CryptAlgorithm encAlgorithmA)
{
  int i;

  encrypted = true;
  permFlags = permFlagsA;
  ownerPasswordOk = ownerPasswordOkA;
  if (keyLengthA <= 32) {
    keyLength = keyLengthA;
  } else {
    keyLength = 32;
  }
  for (i = 0; i < keyLength; ++i) {
    fileKey[i] = fileKeyA[i];
  }
  encVersion = encVersionA;
  encAlgorithm = encAlgorithmA;
}

bool XWPDFXRef::constructXRef()
{
  XWPDFParser *parser;
  XWObject newTrailerDict, obj;
  char buf[256];
  uint pos;
  int num, gen;
  int newSize;
  int streamEndsSize;
  char *p;
  int i;
  bool gotRoot;

	if (entries)
  	free(entries);
  size = 0;
  entries = NULL;

  gotRoot = false;
  streamEndsLen = streamEndsSize = 0;

  str->reset();
  while (1) {
    pos = str->getPos();
    if (!str->getLine(buf, 256)) {
      break;
    }
    p = buf;

    // skip whitespace
    while (*p && XWPDFLexer::isSpace(*p & 0xff)) ++p;

    // got trailer dictionary
    if (!strncmp(p, "trailer", 7)) {
      obj.initNull();
      parser = new XWPDFParser(NULL,
		 new XWPDFLexer(NULL,
		   str->makeSubStream(pos + 7, false, 0, &obj)),
		 false);
      parser->getObj(&newTrailerDict);
      if (newTrailerDict.isDict()) {
	newTrailerDict.dictLookupNF("Root", &obj);
	if (obj.isRef()) {
	  rootNum = obj.getRefNum();
	  rootGen = obj.getRefGen();
	  if (!trailerDict.isNone()) {
	    trailerDict.free();
	  }
	  newTrailerDict.copy(&trailerDict);
	  gotRoot = true;
	}
	obj.free();
      }
      newTrailerDict.free();
      delete parser;

    // look for object
    } else if (isdigit(*p & 0xff)) {
      num = atoi(p);
      if (num > 0) {
	do {
	  ++p;
	} while (*p && isdigit(*p & 0xff));
	if (isspace(*p & 0xff)) {
	  do {
	    ++p;
	  } while (*p && isspace(*p & 0xff));
	  if (isdigit(*p & 0xff)) {
	    gen = atoi(p);
	    do {
	      ++p;
	    } while (*p && isdigit(*p & 0xff));
	    if (isspace(*p & 0xff)) {
	      do {
		++p;
	      } while (*p && isspace(*p & 0xff));
	      if (!strncmp(p, "obj", 3)) {
		if (num >= size) {
		  newSize = (num + 1 + 255) & ~255;
		  if (newSize < 0) {
		    xwApp->error("Bad object number");
		    return false;
		  }
		  entries = (XWPDFXRefEntry *)
		      realloc(entries, newSize * sizeof(XWPDFXRefEntry));
		  for (i = size; i < newSize; ++i) {
		    entries[i].offset = 0xffffffff;
		    entries[i].type = xrefEntryFree;
		  }
		  size = newSize;
		}
		if (entries[num].type == xrefEntryFree ||
		    gen >= entries[num].gen) {
		  entries[num].offset = pos - start;
		  entries[num].gen = gen;
		  entries[num].type = xrefEntryUncompressed;
		  if (num > last) {
		    last = num;
		  }
		}
	      }
	    }
	  }
	}
      }

    } else if (!strncmp(p, "endstream", 9)) {
      if (streamEndsLen == streamEndsSize) {
	streamEndsSize += 64;
	streamEnds = (uint *)realloc(streamEnds, streamEndsSize * sizeof(uint));
      }
      streamEnds[streamEndsLen++] = pos;
    }
  }

  if (gotRoot)
    return true;

  xwApp->error("Couldn't find trailer dictionary");
  return false;
}

uint XWPDFXRef::getStartXref()
{
  char buf[xrefSearchSize+1];
  char *p;
  int c, n, i;

  // read last xrefSearchSize bytes
  str->setPos(xrefSearchSize, -1);
  for (n = 0; n < xrefSearchSize; ++n) {
    if ((c = str->getChar()) == EOF) {
      break;
    }
    buf[n] = c;
  }
  buf[n] = '\0';

  // find startxref
  for (i = n - 9; i >= 0; --i) {
    if (!strncmp(&buf[i], "startxref", 9)) {
      break;
    }
  }
  if (i < 0) {
    return 0;
  }
  for (p = &buf[i+9]; isspace(*p & 0xff); ++p) ;
  lastXRefPos = strToUnsigned(p);

  return lastXRefPos;
}

bool XWPDFXRef::readXRef(uint *pos)
{
  XWPDFParser *parser;
  XWObject obj;
  bool more;

  // start up a parser, parse one token
  obj.initNull();
  parser = new XWPDFParser(NULL,
	     new XWPDFLexer(NULL,
	       str->makeSubStream(start + *pos, false, 0, &obj)),
	     true);
  parser->getObj(&obj, true);

  // parse an old-style xref table
  if (obj.isCmd("xref")) {
    obj.free();
    more = readXRefTable(parser, pos);

  // parse an xref stream
  } else if (obj.isInt()) {
    obj.free();
    if (!parser->getObj(&obj, true)->isInt()) {
      goto err1;
    }
    obj.free();
    if (!parser->getObj(&obj, true)->isCmd("obj")) {
      goto err1;
    }
    obj.free();
    if (!parser->getObj(&obj)->isStream()) {
      goto err1;
    }
    more = readXRefStream(obj.getStream(), pos);
    obj.free();

  } else {
    goto err1;
  }

  delete parser;
  return more;

 err1:
  obj.free();
  delete parser;
  ok = false;
  return false;
}

bool XWPDFXRef::readXRefStream(XWStream *xrefStr, uint *pos)
{
  XWDict *dict;
  int w[3];
  bool more;
  XWObject obj, obj2, idx;
  int newSize, first, n, i;

  dict = xrefStr->getDict();

  if (!dict->lookupNF("Size", &obj)->isInt()) {
    goto err1;
  }
  newSize = obj.getInt();
  obj.free();
  if (newSize < 0) {
    goto err1;
  }
  if (newSize > size) {
    entries = (XWPDFXRefEntry *)realloc(entries, newSize * sizeof(XWPDFXRefEntry));
    for (i = size; i < newSize; ++i) {
      entries[i].offset = 0xffffffff;
      entries[i].type = xrefEntryFree;
    }
    size = newSize;
  }

  if (!dict->lookupNF("W", &obj)->isArray() ||
      obj.arrayGetLength() < 3) {
    goto err1;
  }
  for (i = 0; i < 3; ++i) {
    if (!obj.arrayGet(i, &obj2)->isInt()) {
      obj2.free();
      goto err1;
    }
    w[i] = obj2.getInt();
    obj2.free();
    if (w[i] < 0 || w[i] > 4) {
      goto err1;
    }
  }
  obj.free();

  xrefStr->reset();
  dict->lookupNF("Index", &idx);
  if (idx.isArray()) {
    for (i = 0; i+1 < idx.arrayGetLength(); i += 2) {
      if (!idx.arrayGet(i, &obj)->isInt()) {
	idx.free();
	goto err1;
      }
      first = obj.getInt();
      obj.free();
      if (!idx.arrayGet(i+1, &obj)->isInt()) {
	idx.free();
	goto err1;
      }
      n = obj.getInt();
      obj.free();
      if (first < 0 || n < 0 ||
	  !readXRefStreamSection(xrefStr, w, first, n)) {
	idx.free();
	goto err0;
      }
    }
  } else {
    if (!readXRefStreamSection(xrefStr, w, 0, newSize)) {
      idx.free();
      goto err0;
    }
  }
  idx.free();

  dict->lookupNF("Prev", &obj);
  if (obj.isInt()) {
    *pos = (uint)obj.getInt();
    more = true;
  } else {
    more = false;
  }
  obj.free();
  if (trailerDict.isNone()) {
    trailerDict.initDict(dict);
  }

  return more;

 err1:
  obj.free();
 err0:
  ok = false;
  return false;
}

bool XWPDFXRef::readXRefStreamSection(XWStream *xrefStr, 
                                      int *w, 
                                      int first, 
                                      int n)
{
  uint offset;
  int type, gen, c, newSize, i, j;

  if (first + n < 0) {
    return false;
  }
  if (first + n > size) {
    for (newSize = size ? 2 * size : 1024;
	 first + n > newSize && newSize > 0;
	 newSize <<= 1) ;
    if (newSize < 0) {
      return false;
    }
    entries = (XWPDFXRefEntry *)realloc(entries, newSize * sizeof(XWPDFXRefEntry));
    for (i = size; i < newSize; ++i) {
      entries[i].offset = 0xffffffff;
      entries[i].type = xrefEntryFree;
    }
    size = newSize;
  }
  for (i = first; i < first + n; ++i) {
    if (w[0] == 0) {
      type = 1;
    } else {
      for (type = 0, j = 0; j < w[0]; ++j) {
	if ((c = xrefStr->getChar()) == EOF) {
	  return false;
	}
	type = (type << 8) + c;
      }
    }
    for (offset = 0, j = 0; j < w[1]; ++j) {
      if ((c = xrefStr->getChar()) == EOF) {
	return false;
      }
      offset = (offset << 8) + c;
    }
    for (gen = 0, j = 0; j < w[2]; ++j) {
      if ((c = xrefStr->getChar()) == EOF) {
	return false;
      }
      gen = (gen << 8) + c;
    }
    if (entries[i].offset == 0xffffffff) {
      switch (type) {
      case 0:
	entries[i].offset = offset;
	entries[i].gen = gen;
	entries[i].type = xrefEntryFree;
	break;
      case 1:
	entries[i].offset = offset;
	entries[i].gen = gen;
	entries[i].type = xrefEntryUncompressed;
	break;
      case 2:
	entries[i].offset = offset;
	entries[i].gen = gen;
	entries[i].type = xrefEntryCompressed;
	break;
      default:
	return false;
      }
      if (i > last) {
	last = i;
      }
    }
  }

  return true;
}

bool XWPDFXRef::readXRefTable(XWPDFParser *parser, uint *pos)
{
  XWPDFXRefEntry entry;
  bool more;
  XWObject obj, obj2;
  uint pos2;
  int first, n, newSize, i;

  while (1) {
    parser->getObj(&obj, true);
    if (obj.isCmd("trailer")) {
      obj.free();
      break;
    }
    if (!obj.isInt()) {
      goto err1;
    }
    first = obj.getInt();
    obj.free();
    if (!parser->getObj(&obj, true)->isInt()) {
      goto err1;
    }
    n = obj.getInt();
    obj.free();
    if (first < 0 || n < 0 || first + n < 0) {
      goto err1;
    }
    if (first + n > size) {
      for (newSize = size ? 2 * size : 1024;
	   first + n > newSize && newSize > 0;
	   newSize <<= 1) ;
      if (newSize < 0) {
	goto err1;
      }
      entries = (XWPDFXRefEntry *)realloc(entries, newSize * sizeof(XWPDFXRefEntry));
      for (i = size; i < newSize; ++i) {
	entries[i].offset = 0xffffffff;
	entries[i].type = xrefEntryFree;
      }
      size = newSize;
    }
    for (i = first; i < first + n; ++i) {
      if (!parser->getObj(&obj, true)->isInt()) {
	goto err1;
      }
      entry.offset = (uint)obj.getInt();
      obj.free();
      if (!parser->getObj(&obj, true)->isInt()) {
	goto err1;
      }
      entry.gen = obj.getInt();
      obj.free();
      parser->getObj(&obj, true);
      if (obj.isCmd("n")) {
	entry.type = xrefEntryUncompressed;
      } else if (obj.isCmd("f")) {
	entry.type = xrefEntryFree;
      } else {
	goto err1;
      }
      obj.free();
      if (entries[i].offset == 0xffffffff) {
	entries[i] = entry;
	// PDF files of patents from the IBM Intellectual Property
	// Network have a bug: the xref table claims to start at 1
	// instead of 0.
	if (i == 1 && first == 1 &&
	    entries[1].offset == 0 && entries[1].gen == 65535 &&
	    entries[1].type == xrefEntryFree) {
	  i = first = 0;
	  entries[0] = entries[1];
	  entries[1].offset = 0xffffffff;
	}
	if (i > last) {
	  last = i;
	}
      }
    }
  }

  // read the trailer dictionary
  if (!parser->getObj(&obj)->isDict()) {
    goto err1;
  }

  // get the 'Prev' pointer
  obj.getDict()->lookupNF("Prev", &obj2);
  if (obj2.isInt()) {
    pos2 = (uint)obj2.getInt();
    if (pos2 != *pos) {
      *pos = pos2;
      more = true;
    } else {
      xwApp->error("Infinite loop in xref table");
      more = false;
    }
  } else if (obj2.isRef()) {
    // certain buggy PDF generators generate "/Prev NNN 0 R" instead
    // of "/Prev NNN"
    pos2 = (uint)obj2.getRefNum();
    if (pos2 != *pos) {
      *pos = pos2;
      more = true;
    } else {
      xwApp->error("Infinite loop in xref table");
      more = false;
    }
  } else {
    more = false;
  }
  obj2.free();

  // save the first trailer dictionary
  if (trailerDict.isNone()) {
    obj.copy(&trailerDict);
  }

  // check for an 'XRefStm' key
  if (obj.getDict()->lookup("XRefStm", &obj2)->isInt()) {
    pos2 = (uint)obj2.getInt();
    readXRef(&pos2);
    if (!ok) {
      obj2.free();
      goto err1;
    }
  }
  obj2.free();

  obj.free();
  return more;

 err1:
  obj.free();
  ok = false;
  return false;
}

