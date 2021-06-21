/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSPARSER_H
#define XWPSPARSER_H

#include <QObject>
#include "XWObject.h"

#define PST_NAME_LEN_MAX   127
#define PST_STRING_LEN_MAX 4096
#define PST_MAX_DIGITS     10
#define PST_TOKEN_LEN_MAX  PST_STRING_LEN_MAX

class XWLexer;

class XW_CORE_EXPORT XWPSParser : public QObject
{
	Q_OBJECT
	
public:
	XWPSParser(const char * str, 
	           QObject * parent = 0);
	XWPSParser(const char * str, 
	           int len, 
	           QObject * parent = 0);
	XWPSParser(const char * start, 
	           const char * end, 
	           QObject * parent = 0);
	~XWPSParser();
	
	XWLexer  * getLexer() {return lexer;}
	
	XWObject * getObject(XWObject * obj);
	
	XWObject * parseNumber(XWObject * obj);
	XWObject * parseString(XWObject * obj);

private:
	unsigned char escToUC(uchar *valid);
	
	int getXPair();
	
	uchar ostrToUC(uchar *valid);
	
	XWObject * parseAny(XWObject * obj);
	XWObject * parseASCII85(XWObject * obj);
	XWObject * parseBoolean(XWObject * obj);
	XWObject * parseHex(XWObject * obj);
	XWObject * parseLiteral(XWObject * obj);
	XWObject * parseName(XWObject * obj);
	XWObject * parseNull(XWObject * obj);
		
private:
	XWLexer * lexer;
	const char * endptr;
};

#endif //XWPSPARSER_H