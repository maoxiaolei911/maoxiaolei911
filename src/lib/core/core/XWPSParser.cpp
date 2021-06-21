/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <ctype.h>
#include <QString>
#include "XWApplication.h"
#include "XWLexer.h"
#include "XWStringUtil.h"
#include "XWPSParser.h"

XWPSParser::XWPSParser(const char * str, 
	                   QObject * parent)
	:QObject(parent),
	 lexer(new XWLexer(str))
{
	endptr = str + strlen(str);
}

XWPSParser::XWPSParser(const char * str, 
	                   int len, 
	                   QObject * parent)
	:QObject(parent),
	 lexer(new XWLexer(str, len))
{
	endptr = str + len;
}

XWPSParser::XWPSParser(const char * start, 
	                   const char * end, 
	                   QObject * parent)
	:QObject(parent),
	 lexer(new XWLexer(start, end))
{
	endptr = end;
}

XWPSParser::~XWPSParser()
{
	if (lexer)
		delete lexer;
}

XWObject * XWPSParser::getObject(XWObject * obj)
{
	obj->initNull();
	
	const char * p = lexer->skipWhite();
	while (p < endptr && *p == '%')
	{
		p = lexer->skipLine();
		p = lexer->skipWhite();
	}
	
	if (p >= endptr)
		return 0;
		
	uchar c = *p;
	XWObject * ret = 0;
	switch (c)
	{
		case '/':
    		ret = parseName(obj);
    		break;
    		
    	case '[': 
    	case '{':
    		obj->initMark();
    		p = lexer->skip(1);
    		ret = obj;
    		break;
    		
    	case '<':
    		if (p + 1 >= endptr)
    			return 0;    		
    		c = *(p + 1);
    		if (c == '<')
    		{
    			obj->initMark();
    			p = lexer->skip(2);
    			ret = obj;
    		}
    		else if (isxdigit(c))
    			ret = parseString(obj);
    		else if (c == '~')
    			ret = parseString(obj);
    		break;
    		
    	case '(':
    		ret = parseString(obj);
    		break;
    		
    	case '>':
    		if (p + 1 >= endptr || *(p+1) != '>')
    			return 0;
    		else
    		{
    			obj->initUnknown((uchar*)(">>"), 2);
    			p = lexer->skip(2);
    			ret = obj;
    		}
    		break;
    		
    	case ']': 
    	case '}':
    		obj->initUnknown(&c, 1);
    		p = lexer->skip(1);
    		ret = obj;
    		break;
    		
    	default:
    		if (c == 't' || c == 'f')
      			ret = parseBoolean(obj);
    		else if (c == 'n')
      			ret = parseNull(obj);
    		else if (c == '+' || c == '-' || isdigit(c) || c == '.')
      			ret = parseNumber(obj);
    		break;
	}
	
	if (!ret) 
    	ret = parseAny(obj);

  	return ret;
}

XWObject * XWPSParser::parseNumber(XWObject * obj)
{
	double v = 0.0;	
	bool isreal = false;
	bool ok = false;
	lexer->getNumber(&v, &isreal, &ok);
	if (!ok)
		return 0;
		
	if (isreal)
		obj->initReal(v);
	else
		obj->initInt((int)v);
		
	return obj;
}

XWObject * XWPSParser::parseString(XWObject * obj)
{
	const char * cur = lexer->getCurPtr();
	if (cur + 2 >= endptr)
		return 0;
		
	if (*cur == '(')
		return parseLiteral(obj);
	else if (*cur == '<' && *(cur+1) == '~')
		return parseASCII85(obj);
	else if (*cur == '<')
		return parseHex(obj);
		
	return 0;
}

XWObject * XWPSParser::parseAny(XWObject * obj)
{
	const char * cur = lexer->getCurPtr();
	const char * p = lexer->getCurPtr();
	while (cur < endptr && !XWLexer::tokenEnd(cur, endptr))
		cur = lexer->skip(1);
		
	int len = cur - p;
	obj->initUnknown((uchar*)p, len);
	
	return obj;
}

XWObject * XWPSParser::parseASCII85(XWObject * obj)
{
	bool ok = false;
	uchar  wbuf[PST_STRING_LEN_MAX];
	int len = 0;
	lexer->getASCII85((char*)wbuf, PST_STRING_LEN_MAX, &len, &ok);
	if (!ok)
		return 0;
    
    obj->initString(new XWString((char*)wbuf, len));
    return obj;
}

unsigned char XWPSParser::escToUC(uchar *valid)
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

int XWPSParser::getXPair()
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

uchar XWPSParser::ostrToUC(uchar *valid)
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

XWObject * XWPSParser::parseBoolean(XWObject * obj)
{
	bool v = false;
	bool ok = true;
	
	lexer->getBool(&v, &ok);
	if (!ok)
		return 0;
		
	obj->initBool(v);
	return obj;
}

XWObject * XWPSParser::parseHex(XWObject * obj)
{
	bool ok = false;
	uchar  wbuf[PST_STRING_LEN_MAX];
	int len = 0;
	lexer->getHexString((char*)wbuf, PST_STRING_LEN_MAX, &len, &ok);
	if (!ok)
		return 0;
    
    obj->initString(new XWString((char*)wbuf, len));
    return obj;
}

XWObject * XWPSParser::parseLiteral(XWObject * obj)
{
	const char * cur = lexer->getCurPtr();
	if (cur + 2 > endptr || *cur != '(')
    	return 0;
    
    cur = lexer->skip(1);
	uchar  wbuf[PST_STRING_LEN_MAX];
	long balance = 1;
	uchar c = 0;
	int len = 0;
	while (cur < endptr && len < PST_STRING_LEN_MAX && balance > 0)
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
    	return 0;
    	
    obj->initString(new XWString((char*)wbuf, len));
    return obj;
}

XWObject * XWPSParser::parseName(XWObject * obj)
{
	const char * cur = lexer->getCurPtr();
	if (*cur != '/')
    	return 0;
    	
  	cur = lexer->skip(1);
	uchar  wbuf[PST_NAME_LEN_MAX+1];
	uchar * p = wbuf;
	uchar c = 0;
	int len = 0;
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
		if (len < PST_NAME_LEN_MAX)
      		*p++ = c;
    	len++;
    	cur = lexer->getCurPtr();
	}
	
	*p = '\0';
    	
	obj->initName((char*)wbuf);
	return obj;
}

XWObject * XWPSParser::parseNull(XWObject * obj)
{
	bool ok = false;
  	lexer->getNull(&ok);
  	if (ok)
  		return obj;
  		
	return 0;
}

