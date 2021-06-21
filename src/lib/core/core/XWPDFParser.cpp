/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtDebug>
#include "XWApplication.h"
#include "XWObject.h"
#include "XWArray.h"
#include "XWDict.h"
#include "XWDecrypt.h"
#include "XWPDFParser.h"
#include "XWRef.h"

#define recursionLimit 500

XWPDFParser::XWPDFParser(XWRef *xrefA, 
	                     XWPDFLexer *lexerA, 
	                     bool allowStreamsA,
	                     QObject * parent)
	:QObject(parent)
{
  xref = xrefA;
  lexer = lexerA;
  inlineImg = 0;
  allowStreams = allowStreamsA;
  lexer->getObj(&buf1);
  lexer->getObj(&buf2);
}

XWPDFParser::~XWPDFParser()
{
  buf1.free();
  buf2.free();
  if (lexer)
  	delete lexer;
}

XWObject * XWPDFParser::getObj(XWObject *obj, 
                     					 bool simpleOnly,
                     					 uchar *fileKey,
		 								 					 CryptAlgorithm encAlgorithm, 
		 								 					 int keyLength,
		 								 					 int objNum, 
		 								 					 int objGen, 
		 								 					 int recursion)
{
  char *key;
  XWStream *str;
  XWObject obj2;
  int num;
  XWDecryptStream *decrypt;
  XWString *s, *s2;
  int c;

  // refill buffer after inline image data
  if (inlineImg == 2) {
    buf1.free();
    buf2.free();
    lexer->getObj(&buf1);
    lexer->getObj(&buf2);
    inlineImg = 0;
  }

  // array
  if (!simpleOnly && recursion < recursionLimit && buf1.isCmd("[")) {
    shift();
    obj->initArray(xref);
    while (!buf1.isCmd("]") && !buf1.isEOF())
      obj->arrayAdd(getObj(&obj2, false, fileKey, encAlgorithm, keyLength,
			   objNum, objGen, recursion + 1));
    if (buf1.isEOF())
      xwApp->error(tr("End of file inside array"));
    shift();

  // dictionary or stream
  } else if (!simpleOnly && recursion < recursionLimit && buf1.isCmd("<<")) {
    shift();
    obj->initDict(xref);
    while (!buf1.isCmd(">>") && !buf1.isEOF()) {
      if (!buf1.isName()) {
	xwApp->error(tr("Dictionary key must be a name object"));
	shift();
      } else {
	key = qstrdup(buf1.getName());
	shift();
	if (buf1.isEOF() || buf1.isError()) {
		if (key)
	  delete [] key;
	  break;
	}
	obj->dictAdd(key, getObj(&obj2, false,
				 fileKey, encAlgorithm, keyLength,
				 objNum, objGen, recursion + 1));
      }
    }
    if (buf1.isEOF())
      xwApp->error(tr("End of file inside dictionary"));
    // stream objects are not allowed inside content streams or
    // object streams
    if (allowStreams && buf2.isCmd("stream")) {
      if ((str = makeStream(obj, fileKey, encAlgorithm, keyLength,
			    objNum, objGen, recursion + 1))) {
	obj->initStream(str);
      } else {
	obj->free();
	obj->initError();
      }
    } else {
      shift();
    }

  // indirect reference or integer
  } else if (buf1.isInt()) {
    num = buf1.getInt();
    shift();
    if (buf1.isInt() && buf2.isCmd("R")) {
      obj->initRef(num, buf1.getInt());
      shift();
      shift();
    } else {
      obj->initInt(num);
    }

  // string
  } else if (buf1.isString() && fileKey) {
    s = buf1.getString();
    s2 = new XWString();
    obj2.initNull();
    decrypt = new XWDecryptStream(new XWMemStream(s->getCString(), 0,
					      s->getLength(), &obj2),
				fileKey, encAlgorithm, keyLength,
				objNum, objGen);
    decrypt->reset();
    while ((c = decrypt->getChar()) != EOF) {
      s2->append((char)c);
    }
    delete decrypt;
    obj->initString(s2);
    shift();

  // simple object
  } else {
    buf1.copy(obj);
    shift();
  }

  return obj;
}

XWStream * XWPDFParser::makeStream(XWObject *dict, 
                         					 uchar *fileKey,
		     								           CryptAlgorithm encAlgorithm, 
		     								           int keyLength,
		                               int objNum, 
		                               int objGen, 
		                               int recursion)
{
  XWObject obj;
  XWBaseStream *baseStr;
  XWStream *str;
  uint pos, endPos, length;

  // get stream start position
  lexer->skipToNextLine();
  if (!(str = lexer->getStream())) {
    return NULL;
  }
  pos = str->getPos();

  // get length
  dict->dictLookup("Length", &obj, recursion);
  if (obj.isInt()) {
    length = (uint)obj.getInt();
    obj.free();
  } else {
    xwApp->error(tr("Bad 'Length' attribute in stream"));
    obj.free();
    return NULL;
  }

  // check for length in damaged file
  if (xref && xref->getStreamEnd(pos, &endPos)) {
    length = endPos - pos;
  }

  // in badly damaged PDF files, we can run off the end of the input
  // stream immediately after the "stream" token
  if (!lexer->getStream()) {
    return NULL;
  }
  baseStr = lexer->getStream()->getBaseStream();

  // skip over stream data
  lexer->setPos(pos + length);

  // refill token buffers and check for 'endstream'
  shift();  // kill '>>'
  shift();  // kill 'stream'
  if (buf1.isCmd("endstream")) {
    shift();
  } else {
    xwApp->error(tr("Missing 'endstream'"));
    // kludge for broken PDF files: just add 5k to the length, and
    // hope its enough
    length += 5000;
  }

  // make base stream
  str = baseStr->makeSubStream(pos, true, length, dict);

  // handle decryption
  if (fileKey) {
    str = new XWDecryptStream(str, fileKey, encAlgorithm, keyLength,
			    objNum, objGen);
  }

  // get filters
  str = str->addFilters(dict);

  return str;
}

void XWPDFParser::shift()
{
 if (inlineImg > 0) {
    if (inlineImg < 2) {
      ++inlineImg;
    } else {
      // in a damaged content stream, if 'ID' shows up in the middle
      // of a dictionary, we need to reset
      inlineImg = 0;
    }
  } else if (buf2.isCmd("ID")) {
    lexer->skipChar();		// skip char after 'ID' command
    inlineImg = 1;
  }
  buf1.free();
  buf1 = buf2;
  if (inlineImg > 0)		// don't buffer inline image data
    buf2.initNull();
  else
    lexer->getObj(&buf2);
}

