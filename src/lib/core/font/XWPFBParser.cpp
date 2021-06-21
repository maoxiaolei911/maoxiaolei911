/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include "XWStringUtil.h"
#include "XWApplication.h"
#include "XWLexer.h"
#include "FontEncodingTables.h"
#include "TexEncoding.h"
#include "XWPFBParser.h"

XWPFBParser::XWPFBParser(const char * start, 
	                     long lenA, 
	                     QObject * parent)
	:QObject(parent)
{
	lexer = new XWLexer(start, lenA);
	type = PFB_TYPE_UNKNOWN;
	len = 0;
	token = 0;
	endptr = start + lenA;
}

XWPFBParser::XWPFBParser(const char * start, 
	            		 const char * end, 
	            		 QObject * parent)
	:QObject(parent)
{
	lexer = new XWLexer(start, end);
	type = PFB_TYPE_UNKNOWN;
	len = 0;
	token = 0;
	endptr = end;
}

XWPFBParser::~XWPFBParser()
{
	if (lexer)
		delete lexer;
	
	if (token)
		delete [] token;
}

bool XWPFBParser::getBoolean()
{
	getToken();
	if (type == PFB_TYPE_BOOLEAN)
		return len == 4 ? true : false;

	return false;
}

int XWPFBParser::getEncoding(char **enc_vec)
{
	const char * p = lexer->getCurPtr();
	if (p >= endptr)
		return -1;
		
	getToken();	
	
	char ** enc = 0;		
	if (matchOp("StandardEncoding"))
		enc = standardEncoding;
	else if (matchOp("ISOLatin1Encoding"))
		enc = ISOLatin1Encoding;
	else if (matchOp("ExpertEncoding"))
		enc = expertEncoding;
	else
		enc = getBaseEnc(token);
		
	if (enc && enc_vec)
	{
		for (int code = 0; code < 256; code++)
		{
			if (enc[code] != NULL && strcmp(enc[code], ".notdef") != 0)
				enc_vec[code] = qstrdup(enc[code]);
			else
				enc_vec[code] = 0;
		}
	}
	else
	{
		seekOperator("array");
		int code = 0;
		p = lexer->getCurPtr();
		while (p < endptr)
		{
			getToken();
			p = lexer->getCurPtr();
			if (matchOp("def") || matchOp("readonly"))
				break;
			else if (!matchOp("dup"))
				continue;
			
			getToken();	
			p = lexer->getCurPtr();
			if (type != PFB_TYPE_INTEGER || lval < 0 || lval > 255)
				continue;
			
			code = lval;
			if (enc_vec)
				enc_vec[code] = 0;
				
			getToken();
			p = lexer->getCurPtr();
			if (type != PFB_TYPE_NAME)
				continue;
			if (enc_vec)
			{
				if (enc_vec[code])
					delete [] enc_vec[code];
						
				enc_vec[code] = qstrdup(token);
			}
			
			getToken();
			p = lexer->getCurPtr();
			if (!matchOp("put"))
			{
				if (enc_vec[code])
					delete [] enc_vec[code];
				enc_vec[code] = 0;
			}
		}
	}
	
	return 0;
}

const char * XWPFBParser::getNextKey()
{
	const char * p = lexer->getCurPtr();
	while (p < endptr)
	{
		getToken();
		p = lexer->getCurPtr();
		if (type == PFB_TYPE_NAME)
			return token;
	}
	
	return 0;
}

bool XWPFBParser::getOp(const char *op)
{
	getToken();
	if (matchOp(op))
		return true;
		
	return false;
}

const char * XWPFBParser::getString()
{
	getToken();
	if (type == PFB_TYPE_STRING || type == PFB_TYPE_NAME)
		return token;
	
	return 0;
}

int XWPFBParser::getToken()
{
	type = PFB_TYPE_UNKNOWN;
	len = 0;
	if (token)
		delete [] token;
	token = 0;
	lval = -1;
	const char * p = lexer->skipWhite();
	while (p < endptr && *p == '%')
	{
		p = lexer->skipLine();
		p = lexer->skipWhite();
	}
	
	if (p >= endptr)
	{
		type = PFB_TYPE_END;
		return type;
	}
		
	uchar c = (uchar)(*p);
	switch (c)
	{
		case '/':
			parseName();
			break;
			
		case '[': 
    	case '{':
    		type = PFB_TYPE_MARK;
    		token = new char[2];
    		token[0] = c;
    		token[1] = '\0';
    		p = lexer->skip(1);
    		len = 1;
    		break;
    		
    	case '<':
    		if (p + 1 >= endptr)
    		{
    			p = lexer->skip(1);
    			type = PFB_TYPE_END;
    			len = 1;
    		}
    		else
    		{
    			c = (uchar)(*(p + 1));
    			if (c == '<')
    			{
    				type = PFB_TYPE_MARK;
    				p = lexer->skip(2);
    				token = qstrdup("<<");
    				len = 2;
    			}
    			else
    			{
    				if (isxdigit(c))
    					parseString();
    				else if (c == '~')
    					parseString();
    			}
    		}
    		break;
    		
    	case '(':
    		parseString();
    		break;
    		
    	case '>':
    		if (p + 1 >= endptr || *(p+1) != '>')
    		{
    			p = lexer->skip(1);
    			xwApp->error("unexpected end of ASCII hex string marker.\n");
    			type = PFB_TYPE_END;
    			len = 1;
    		}
    		else
    		{
    			token = qstrdup(">>");
    			len = 2;
    			p = lexer->skip(2);
    		}
    		break;
    		
    	case ']': 
    	case '}':
    		token = new char[2];
    		token[0] = c;
    		token[1] = '\0';
    		p = lexer->skip(1);
    		len = 1;
    		break;
    		
    	default:
    		if (c == 't' || c == 'f')
      			parseBoolean();
    		else if (c == 'n')
      			parseNull();
    		else if (c == '+' || c == '-' || isdigit(c) || c == '.')
      			parseNumber();
    		break;
	}
	
	if (!token && type == PFB_TYPE_UNKNOWN) 
		parseAny();
		
	return type;
}

int XWPFBParser::getValues(double *value, int max)
{
	const char * p = lexer->getCurPtr();
	if (p >= endptr)
		return -1;
		
	int argn = 0;
	getToken();
	p = lexer->getCurPtr();
	if ((type == PFB_TYPE_INTEGER || type == PFB_TYPE_REAL) && max > 0)
	{
		if (type == PFB_TYPE_INTEGER)
			value[0] = (double) lval;
		else
			value[0] = dval;
    	argn = 1;
	}
	else if (type == PFB_TYPE_MARK)
	{
		while (p < endptr)
		{
			getToken();
			if ((type == PFB_TYPE_INTEGER || type == PFB_TYPE_REAL)  && argn < max)
			{
				if (type == PFB_TYPE_INTEGER)
					value[argn] = (double) lval;
				else
					value[argn] = dval;
			}
			else
				break;
			
			p = lexer->getCurPtr();
			argn++;
		}
		
		if (!matchOp("]") && !matchOp("}"))
			argn = -1;
	}
	else if (type == PFB_TYPE_BOOLEAN)
	{
		value[0] = getBoolean();
		argn = 1;
	}
	
	return argn;
}

bool XWPFBParser::matchName(const char * name)
{
	if (type != PFB_TYPE_NAME || !token)
		return false;
		
	return (0 == strncmp(token, name, strlen(name)));
}

bool XWPFBParser::matchOp(const char *op)
{
	if (type != PFB_TYPE_UNKNOWN || !token)
		return false;
		
	return (0 == strncmp(token, op, strlen(op)));
}

bool XWPFBParser::seekOperator(const char *op)
{
	const char * p = lexer->getCurPtr();
	while (p < endptr)
	{
		getToken();
		if (matchOp(op))
			return true;
		p = lexer->getCurPtr();
	}
	
	return false;
}

const char * XWPFBParser::skip(long lenA)
{
	return lexer->skip(lenA);
}

const char * XWPFBParser::tellPos(char ** endptrA)
{
	const char * p = lexer->getCurPtr();
	if (endptrA)
		*endptrA = (char*)endptr;
		
	return p;
}

uchar XWPFBParser::escToUC(uchar *valid)
{
	const char * cur = lexer->getCurPtr();	
	uchar escaped = *cur;
  	*valid    = 1;  	
  	uchar unescaped = 0;  	
  	switch (escaped)
  	{
  		case '\\': 
  		case ')': 
  		case '(':
    		unescaped = escaped;
    		cur = lexer->skip(1);
    		break;
    		
    	case 'n': 
    		unescaped = '\n'; 
    		cur = lexer->skip(1);
    		break;
    		
  		case 'r': 
  			unescaped = '\r'; 
  			cur = lexer->skip(1);
  			break;
  			
  		case 't': 
  			unescaped = '\t'; 
  			cur = lexer->skip(1);
  			break;
  			
  		case 'b': 
  			unescaped = '\b'; 
  			cur = lexer->skip(1);
  			break;
  			
  		case 'f': 
  			unescaped = '\f'; 
  			cur = lexer->skip(1);
  			break;
  			
  		case '\r':
    		unescaped = 0;
    		*valid    = 0;
    		if ((cur < endptr - 1) && cur[1] == '\n')
    			cur = lexer->skip(2);
    		else
    			cur = lexer->skip(1);
    		break;
    		
  		case '\n':
    		unescaped = 0;
    		*valid    = 0;
    		cur = lexer->skip(1);
    		break;
    		
    	
  		default:
    		unescaped = ostrToUC(valid);
    		break;
  	}
  	
  	return unescaped;
}

int XWPFBParser::getXPair()
{
	const char * p = lexer->getCurPtr();
	int hi = xtoi(*p);
  	if (hi < 0)
    	return hi;
    	
    p = lexer->skip(1);
    int lo = xtoi(*p);
  	if (lo < 0)
    	return lo;
    	
  	p = lexer->skip(1);
  	return ((hi << 4)| lo);
}

uchar XWPFBParser::ostrToUC(uchar *valid)
{
	const char * cur = lexer->getCurPtr();
	const char * inbuf = cur;
	uint val = 0;
	while (cur < endptr && 
	       cur < inbuf + 3 && 
	 		(*cur >= '0' && *cur <= '7'))
	{
		val = (val << 3) | (*cur - '0');
    	cur = lexer->skip(1);
	}
	
	if (val > 255 || cur == inbuf)
    	*valid = 0;
  	else
    	*valid = 1;

  	return (uchar) val;
}

void XWPFBParser::parseAny()
{
	const char * cur = lexer->getCurPtr();
	const char * p = lexer->getCurPtr();
	while (cur < endptr && !XWLexer::tokenEnd(cur, endptr))
		cur = lexer->skip(1);
		
	len = cur - p;
	token = new char[len + 1];
	memcpy(token, p, len);
  	token[len] = '\0';
}

void XWPFBParser::parseBoolean()
{
	bool ok = false;
	bool v = false;
	
	lexer->getBool(&v, &ok);
	if (!ok)
		return ;
		
	type = PFB_TYPE_BOOLEAN;
	if (v)
	{
		token = qstrdup("true");
		len = 4;
	}
	else
	{
		token = qstrdup("false");
		len = 5;
	}
}

void XWPFBParser::parseASCII85()
{
	bool ok = false;
	uchar  wbuf[PFB_STRING_LEN_MAX];
	lexer->getASCII85((char*)wbuf, PFB_STRING_LEN_MAX, &len, &ok);
	if (!ok)
		return ;
    	
    type = PFB_TYPE_STRING;
    token = new char[len + 1];
    memcpy(token, wbuf, len);
    token[len] = '\0';
}

void XWPFBParser::parseHex()
{
	bool ok = false;
	uchar  wbuf[PFB_STRING_LEN_MAX];
	lexer->getHexString((char*)wbuf, PFB_STRING_LEN_MAX, &len, &ok);
	if (!ok)
		return ;
    	
    type = PFB_TYPE_STRING;
    token = new char[len + 1];
    memcpy(token, wbuf, len);
    token[len] = '\0';
}

void XWPFBParser::parseLiteral()
{
	const char * cur = lexer->getCurPtr();
	if (cur + 2 > endptr || *cur != '(')
    	return ;
    
    cur = lexer->skip(1);
	uchar  wbuf[PFB_STRING_LEN_MAX];
	long balance = 1;
	uchar c = 0;
	while (cur < endptr && len < PFB_STRING_LEN_MAX && balance > 0)
	{
		c = *cur;
		cur = lexer->skip(1);
		switch (c)
		{
			case '\\':
      			{
					uchar valid;
					uchar unescaped = escToUC(&valid);
					if (valid)
	  					wbuf[len++] = unescaped;
      			}
      			break;
      			
      		case '(':
      			balance++;
      			wbuf[len++] = '(';
      			break;
      			
      		case ')':
      			balance--;
      			if (balance > 0)
					wbuf[len++] = ')';
      			break;
      			
      		case '\r':
      			if (cur < endptr && *cur == '\n')
					cur++;
      			wbuf[len++] = '\n';
      			break;
      			
    		default:
      			wbuf[len++] = c;
		}
		
		cur = lexer->getCurPtr();
	}
	
	if (c != ')')
    	return ;
    	
    type = PFB_TYPE_STRING;
    token = new char[len + 1];
    memcpy(token, wbuf, len);
    token[len] = '\0';
}

void XWPFBParser::parseName()
{
	const char * cur = lexer->getCurPtr();
	if (*cur != '/')
    	return ;
  	cur = lexer->skip(1);
	uchar  wbuf[PFB_NAME_LEN_MAX+1];
	uchar * p = wbuf;
	uchar c = 0;
	while (!XWLexer::tokenEnd(cur, endptr))
	{
		c = *cur;
		cur = lexer->skip(1);
		if (c == '#')
		{
			if (cur + 2 >= endptr)
				break;
				
			int val = getXPair();
			if (val <= 0)
			{
				cur = lexer->getCurPtr();
				continue;
			}
			
			c = (uchar) val;
		}
		if (len < PFB_NAME_LEN_MAX)
      		*p++ = c;
    	len++;
    	cur = lexer->getCurPtr();
	}
	
	*p = '\0';
		
	type = PFB_TYPE_NAME;
	token = new char[len + 1];
    memcpy(token, wbuf, len);
    token[len] = '\0';
}

void XWPFBParser::parseNumber()
{
	double v = 0.0;	
	bool isreal = false;
	bool ok = false;
	lexer->getNumber(&v, &isreal, &ok);
	if (!ok)
		return ;
		
	if (isreal)
	{
		type = PFB_TYPE_REAL;
		dval = v;
	}
	else
	{
		type = PFB_TYPE_INTEGER;
		lval = (long)v;
	}
}

void XWPFBParser::parseNull()
{
	bool ok = false;
	lexer->getNull(&ok);
	if (!ok)
		return ;
		
	type = PFB_TYPE_NULL;
	token = qstrdup("null");
}

void XWPFBParser::parseString()
{
	const char * p = lexer->getCurPtr();
	if (p + 2 >= endptr)
		return ;
		
	if (*p == '(')
		parseLiteral();
	else if (*p == '<' && *(p+1) == '~')
		parseASCII85();
	else if (*p == '<')
		parseHex();
}


