/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "XWStringUtil.h"
#include "XWApplication.h"
#include "XWLexer.h"

#define C_QUOTE  '"'
#define C_ESCAPE '\\'

#define Q_TERM          0
#define Q_CONT         -1
#define Q_ERROR_UNTERM -1
#define Q_ERROR_INVAL  -2
#define Q_ERROR_BUFF   -3

#define DDIGITS_MAX 10

#ifndef isodigit
#define isodigit(c) ((c) >= '0' && (c) <= '7')
#endif

static const char *val_valid_chars =
    "!\"#$&'*+,-./0123456789:;?@ABCDEFGHIJKLMNOPQRSTUVWXYZ\\^_`abcdefghijklmnopqrstuvwxyz|~";
    
static const char *ident_valid_chars =
    "!\"#$&'*+,-.0123456789:;=?@ABCDEFGHIJKLMNOPQRSTUVWXYZ\\^_`abcdefghijklmnopqrstuvwxyz|~";

static const char *_ukeys[] = {
        "pt", "in", "cm", "mm", "bp", "px", NULL
};

XWLexer::XWLexer(const char * str, 
	             QObject * parent)
	:QObject(parent),
	 startPtr(str),
	 curPtr(str)
{
	endPtr = strlen(str) + str;
	savePtr = curPtr;
	tainted = 0;
}

XWLexer::XWLexer(const char * str, 
	             int len, 
	             QObject * parent)
	:QObject(parent),
	 startPtr(str),
	 curPtr(str)
{
	endPtr = len + str;
	savePtr = curPtr;
	tainted = 0;
}

XWLexer::XWLexer(const char * start, 
	             const char * end, 
	             QObject * parent)
	:QObject(parent),
	 startPtr(start),
	 curPtr(start),
	 endPtr(end)
{
	savePtr = curPtr;
	tainted = 0;
}

const char * XWLexer::getASCII85(char * buf, 
	                             int buflen, 
	                             int * slen,
	                             bool * ok)
{
	if (slen)
		*slen = 0;
			
	if (ok)
		*ok = false;
			
	if (curPtr >= endPtr || *curPtr != '<')
		return curPtr;
		
	curPtr += 2;
	
	int c[5];
  	int b[4];
  	int index = 0;
  	int n = 0;
  	bool eof = false;
  	int len = 0;
  	while (curPtr < endPtr && len < buflen)
  	{
  		if (index >= n)
  		{
  			if (eof)
  				break;
  				
  			index = 0;
  			do 
  			{
      			c[0] = *curPtr;
      			curPtr++;
    		} while (isSpace(c[0]));
    			
    		if (c[0] == '~' || curPtr >= endPtr)
    		{
    			eof = true;
      			n = 0;
      			break;
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
	  					c[k] = *curPtr;
      					curPtr++;
					} while (isSpace(c[k]));
					
					if (c[k] == '~' || curPtr >= endPtr)
	  					break;
    			}
    			
    			n = k - 1;
    			if (k < 5 && (c[k] == '~' || curPtr >= endPtr))
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
  		
  		buf[len++] = (uchar)(b[index]);
  		++index;
  	}
  	
  	if (*curPtr != '~')
    	return curPtr;
    	
    curPtr += 2;
    if (slen)
		*slen = len;
			
	if (ok)
		*ok = true;
    
    return curPtr;
}

const char * XWLexer::getBool(bool * v, bool * ok)
{
	if (ok)
      	*ok = true;
      	
	if (curPtr + 4 <= endPtr && !strncmp(curPtr, "true", 4)) 
	{
    	if (curPtr + 4 == endPtr || isTokenSep(curPtr[4])) 
    	{
      		curPtr += 4;
      		if (v)
      			*v = true;
      			
      		return curPtr;
    	}
  	} 
  	else if (curPtr + 5 <= endPtr && !strncmp(curPtr, "false", 5)) 
  	{
    	if (curPtr + 5 == endPtr || isTokenSep(curPtr[5])) 
    	{
      		curPtr += 5;
      		if (v)
      			*v = false;
      			
      		return curPtr;
    	}
  	}
      			
  	if (ok)
      	*ok = false;
      	
//    xwApp->warning(tr("not a boolean object.\n"));
      	
    return curPtr;
}

int XWLexer::getChar()
{
	int   ch = 0;
  	if (curPtr[0] == '#') 
  	{
    	if (curPtr + 2 >= endPtr) 
    	{
      		curPtr = endPtr;
      		return -1;
    	}
    	
    	if (!isxdigit(curPtr[1]) || !isxdigit(curPtr[2])) 
    	{
      		curPtr += 3;
      		return -1;
    	}
    	
    	ch   = (xtoi(curPtr[1]) << 4);
    	ch  += xtoi(curPtr[2]);
    	curPtr += 3;
  	} 
  	else 
  	{
    	ch = curPtr[0];
    	curPtr++;
  	}

  	return ch;
}

char * XWLexer::getCIdent()
{
    if (curPtr >= endPtr || !isCNonDigits(*curPtr))
        return 0;
        
    int n = 0;
    const char * p = curPtr;
    while (curPtr < endPtr && isCIdentChar(*curPtr))
    {
    	curPtr++; 
    	n++;
    }
    
    char * q = new char[n + 1];
    if (!q)
        return 0;
        
    memcpy(q, p, n); 
    q[n] = '\0';
    return  q;
}

char * XWLexer::getCString()
{
	const char  *p = curPtr;
    if (p >= endPtr || p[0] != C_QUOTE)
        return 0;

    p++;
    int l = readCLitStrC(0, 0, &p);
    char  *q = 0;
    if (l >= 0) 
    {
        q = new char[l + 1];
        p = curPtr + 1;
        l = readCLitStrC(q, l + 1, &p);
    }

    curPtr = p;
    return  q;
}

char * XWLexer::getFloatDecimal()
{
	const char * p = curPtr;
    if (p >= endPtr)
        return 0;
        
    if (p[0] == '+' || p[0] == '-')
        p++;
        
    int s = 0; 
    int n = 0;
    for (; p < endPtr && s >= 0; )
    {
        switch (p[0]) 
        {
            case '+': 
            case '-':
                if (s != 2)
                    s = -1;
                else 
                {
                    s = 3; 
                    p++;
                }
                break;
            
            case '.':
                if (s > 0)
                    s = -1;
                else 
                {
                    s = 1; 
                    p++;
                }
                break;
                
            case '0': 
            case '1': 
            case '2': 
            case '3': 
            case '4':
            case '5': 
            case '6': 
            case '7': 
            case '8': 
            case '9':
                n++; 
                p++;
            break;
            
            case 'E': 
            case 'e':
                if (n == 0 || s == 2)
                    s = -1;
                else 
                {
                    s = 2; 
                    p++;
                }
                break;
                
            default:
                s = -1;
                break;
        }
    }
    
    char * q = 0;
    if (n != 0) 
    {
        n = (int) (p - curPtr);
        q = new char[n + 1];
        if (!q)
            return 0;
        memcpy(q, curPtr, n); 
        q[n] = '\0';
    }

    curPtr = p;
    return  q;
}

const char * XWLexer::getHexString(char * buf, 
	                               int buflen, 
	                               int * slen,
	                               bool * ok)
{
	if (ok)
      	*ok = false;
      	
    skipWhite();      	
	if (curPtr >= endPtr || curPtr[0] != '<')
		return curPtr;
		
	curPtr++;
	long len = 0;
	while (curPtr < endPtr && curPtr[0] != '>' && len < buflen)
	{
		skipWhite();
		if (curPtr >= endPtr || curPtr[0] == '>')
      		break;
      		
      	int ch = (xtoi(curPtr[0]) << 4);
    	curPtr++;
    	if (curPtr < endPtr && curPtr[0] != '>') 
    	{
      		ch += xtoi(curPtr[0]);
      		curPtr++;
    	}
    	buf[len++] = (ch & 0xff);
	}
	
	if (slen)
		*slen = len;
	if (curPtr >= endPtr) 
	{
    	xwApp->warning(tr("premature end of input hex string.\n"));
		return curPtr;
  	} 
  	else if (curPtr[0] != '>') 
  	{
    	QString msg = QString(tr("pdf string length too long. (limit: %1)\n")).arg(buflen);
		xwApp->warning(msg);
		return curPtr;
  	}

  	curPtr++;
  	if (ok)
      	*ok = true;
  	return curPtr;
}

char * XWLexer::getIdent(const char * end)
{
	if (!end)
		end = endPtr;
		
	return parseGenIdent(end, ident_valid_chars);
}


#define K_UNIT__PT  0
#define K_UNIT__IN  1
#define K_UNIT__CM  2
#define K_UNIT__MM  3
#define K_UNIT__BP  4
#define K_UNIT__PX  5

bool XWLexer::getLength(double *vp, double mag)
{
	char * q = getFloatDecimal();
  	if (!q) 
  	{
    	*vp = 0.0;
    	return  false;
  	}
  	
  	double v = atof(q);
  	delete [] q;
  	
  	q = getCIdent();
  	bool ret = true;
  	double u = 1.0;
  	if (q)
  	{
  		char * oldq = q;
  		if (strlen(q) > strlen("true") &&
        	!memcmp(q, "true", strlen("true")))
        {
        	u /= (mag != 0.0 ? mag : 1.0);
        	q += strlen("true");
        }
        
        int k = 0;
        for (; _ukeys[k] && strcmp(_ukeys[k], q); k++);
        {
        	switch (k)
        	{
        		case K_UNIT__PT: 
        			u *= 72.0 / 72.27; 
        			break;
        			
    			case K_UNIT__IN: 
    				u *= 72.0; 
    				break;
    				
    			case K_UNIT__CM: 
    				u *= 72.0 / 2.54 ; 
    				break;
    				
    			case K_UNIT__MM: 
    				u *= 72.0 / 25.4 ; 
    				break;
    				
    			case K_UNIT__BP: 
    				u *= 1.0 ; 
    				break;
    				
    			case K_UNIT__PX: 
    				u *= 1.0 ; 
    				break;
    				
    			default:
    				{
    					QString msg = QString(tr("unknown unit of measure: "));
        				msg.append(q);
        				msg.append("\n");
        				xwApp->warning(msg);
      				}
      				ret = false;
      				break;
        	}
        }
        
        delete [] oldq;
  	}
  	
  	*vp = v * u;
  	return  ret;
}

const char * XWLexer::getLiteralString(char * buf, 
	                                   int buflen, 
	                                   int * slen,
	                                   bool * ok)
{
	if (ok)
		*ok = false;
		
	skipWhite();
	if (curPtr >= endPtr || curPtr[0] != '(')
		return curPtr;
		
	curPtr++;
	int ch, op_count = 0, len = 0;
	while (curPtr < endPtr)
	{
		ch = curPtr[0];
    	if (ch == ')' && op_count < 1)
      		break;
      		
      	if (tainted) 
      	{
      		if (curPtr + 1 < endPtr && (ch & 0x80)) 
      		{
				if (len + 2 >= buflen) 
					goto longstr;
				buf[len++] = curPtr[0];
				buf[len++] = curPtr[1];
				curPtr += 2;
				continue;
      		}
    	}
    	
    	if (len + 1 >= buflen) 
    		goto longstr;
    		
    	switch (ch) 
    	{
    		case '\\':
      			ch = getPSEscChar();
      			if (ch >= 0)
					buf[len++] = (ch & 0xff);
      			break;
      			
    		case '\r':
      			curPtr++;
      			if ((curPtr < endPtr) && (curPtr[0] == '\n'))
					curPtr++;
      			buf[len++] = '\n';
      			break;
      			
    		default:
      			if (ch == '(')
					op_count++;
      			else if (ch == ')')
					op_count--;
      			buf[len++] = ch;
      			curPtr++;
      			break;
    	}
	}
	
	if (slen)
		*slen = len;
	
	if (op_count > 0 || curPtr >= endPtr || curPtr[0] != ')') 
	{
    	xwApp->warning(tr("unbalanced parens/truncated PDF literal string.\n"));
    	return curPtr;
  	}

  	curPtr++;
  	if (ok)
		*ok = true;
  	return curPtr;
	
longstr:
	QString msg = QString(tr("pdf string length too long. (limit: %1)\n")).arg(buflen);
	xwApp->warning(msg);
	return curPtr;
}

const char * XWLexer::getName(char * buf, int buflen, bool * ok)
{
	if (ok)
		*ok = false;
		
	skipWhite();
	if (curPtr >= endPtr || *curPtr != '/') 
    	goto badname;
    	
    curPtr++;
  	int len = 0;
  	while (curPtr < endPtr && !isTokenSep(curPtr[0]))
  	{
  		int ch = getChar();
    	if (ch < 0 || ch > 0xff) 
      		xwApp->warning(tr("invalid char in PDF name object. (ignored)\n"));
    	else if (ch == 0) 
      		xwApp->warning(tr("null char not allowed in PDF name object. (ignored)\n"));
    	else if (len < buflen) 
      		buf[len++] = ch;
    	else 
    	{
    		QString msg = QString(tr("pdf name length too long. (>= %1 bytes, truncated)\n")).arg(buflen);
			xwApp->warning(msg);
    	}
  	}
  	
  	if (len < 1) 
    	goto badname;
    	
    if (ok)
		*ok = true;
    	
  	buf[len] = '\0';
  	return curPtr;
	
badname:
	xwApp->warning(tr("could not find a name object.\n"));
    return curPtr;
}

const char * XWLexer::getNull(bool * ok)
{
	if (ok)
		*ok = true;
		
	skipWhite();
  	if (curPtr + 4 > endPtr) 
    	goto badnull;    	
  	else if (curPtr + 4 < endPtr && !isTokenSep(curPtr[4])) 
    	goto badnull;
  	else if (!strncmp(curPtr, "null", 4)) 
  	{
    	curPtr += 4;
      	return curPtr;
  	}
  	
badnull:
	if (ok)
		*ok = false;
//	xwApp->warning(tr("not a null object.\n"));
    return curPtr;
}

const char * XWLexer::getNumber(double * v, bool * isreal, bool * ok)
{
	if (ok)
		*ok = false;
		
	if (isreal)
		*isreal = false;
		
	char * cur = 0;
	long lval = strtol(curPtr, (char **)(&cur), 10);
	if (*cur == '.' || *cur == 'e' || *cur == 'E')
	{
		double dval = strtod(curPtr, (char **)(&cur));		
		if (tokenEnd(cur, endPtr))
		{
			curPtr = (const char *)cur;
			if (v)
				*v = dval;
				
			if (isreal)
				*isreal = true;
				
			if (ok)
				*ok = true;
		}
	}
	else if (curPtr != cur && tokenEnd(cur, endPtr))
	{
		curPtr = (const char *)cur;		
		if (v)
			*v = lval;
				
		if (ok)
			*ok = true;
	}
	else if (lval >= 2 && lval <= 36 && *cur == '#' && isalnum(*++cur) &&
	         (lval != 16 || (cur[1] != 'x' && cur[1] != 'X')))
	{
		lval = strtol(cur, (char **)(&cur), lval);		
		if (tokenEnd(cur, endPtr))
		{
			curPtr = (const char *)cur;			
			if (v)
				*v = lval;
				
			if (ok)
				*ok = true;
		}
	}
	
	return curPtr;
}

const char * XWLexer::getNumber(double * v, bool * ok)
{
	if (ok)
		*ok = true;
		
	unsigned long ipart = 0, dpart = 0;
  	int      nddigits = 0, sign = 1;
  	int      has_dot = 0;
  	static double ipot[DDIGITS_MAX+1] = {
    	1.0,
    	0.1,
    	0.01,
    	0.001,
    	0.0001,
    	0.00001,
    	0.000001,
    	0.0000001,
    	0.00000001,
    	0.000000001,
    	0.0000000001
  	};
  	
  	const char * p = skipWhite();
  	if (p >= endPtr || (!isdigit(p[0]) && p[0] != '.' && p[0] != '+' && p[0] != '-')) 
    	goto badnum;
  	
  	if (p[0] == '-') 
  	{
    	if (p + 1 >= endPtr) 
      		goto badnum;
      		
    	sign = -1;
    	p++;
  	} 
  	else if (p[0] == '+') 
  	{
    	if (p + 1 >= endPtr) 
      		goto badnum;
    	sign =  1;
    	p++;
  	}
  	
  	while (p < endPtr && !isTokenSep(p[0]))
  	{
  		if (p[0] == '.')
  		{
  			if (has_dot) 
  				goto badnum;
      		else 
				has_dot = 1;
  		}
  		else if (isdigit(p[0]))
  		{
  			if (has_dot) 
  			{
				if (nddigits == DDIGITS_MAX) 
	  				xwApp->warning(tr("number with more than 10 fractional digits.\n"));
				else if (nddigits < DDIGITS_MAX) 
				{
	  				dpart = dpart * 10 + p[0] - '0';
	  				nddigits++;
				}
      		} 
      		else 
				ipart = ipart * 10 + p[0] - '0';
  		}
  		else
  			goto badnum;
  			
  		p++;
  	}
  	
  	curPtr = p;
  	if (v)
  		*v = (double)(sign * (((double)ipart) + dpart * ipot[nddigits]));
  	return curPtr;
  	
badnum:
	if (ok)
		*ok = false;
	xwApp->warning(tr("could not find a numeric object.\n"));
    return curPtr;
}

char * XWLexer::getNumber()
{
	skipWhite();
    const char * p = curPtr;
    if (p < endPtr && (*p == '+' || *p == '-'))
        p++;
        
    while (p < endPtr && isdigit(*p))
        p++;
    if (p < endPtr && *p == '.') 
    {
        p++;
        while (p < endPtr && isdigit(*p))
            p++;
    }
    
    char * number = parsedString(curPtr, p);

    curPtr = p;
    return number;
}

char * XWLexer::getOptIdent()
{
	if (curPtr < endPtr && *curPtr == '@') 
    {
        curPtr++;
        return getIdent();
    }

    return 0;
}

int XWLexer::getPSEscChar()
{
	const char * p = curPtr + 1;
	int   ch = -1;
  	switch (p[0])
  	{
  		case 'n': 
  			ch = '\n'; 
  			p++; 
  			break;
  			
  		case 'r': 
  			ch = '\r'; 
  			p++; 
  			break;
  			
  		case 't': 
  			ch = '\t'; 
  			p++; 
  			break;
  			
  		case 'b': 
  			ch = '\b'; 
  			p++; 
  			break;
  			
  		case 'f': 
  			ch = '\f'; 
  			p++; 
  			break;
  			
  		case '\n':
    		ch = -1;
    		p++;
    		break;
    		
  		case '\r':
    		ch = -1;
    		p++;
    		if (p < endPtr && p[0] == '\n')
      			p++;
    		break;
    		
    	default:
    		if (p[0] == '\\' || p[0] == '('  || p[0] == ')') 
    		{
      			ch = p[0];
      			p++;
    		} 
    		else if (isodigit(p[0])) 
    		{
      			ch = 0;
      			for (int i = 0; i < 3 && p < endPtr && isodigit(p[0]); i++) 
      			{
        			ch = (ch << 3) + (p[0] - '0');
					p++;
      			}
      			ch = (ch & 0xff);
    		} 
    		else 
    		{
      			ch = ((unsigned char) p[0]);
      			p++;
    		}
    		break;
  	}
  	
  	curPtr = p;
  	return ch;
}

const char * XWLexer::getString(char * buf, 
	                            int buflen, 
	                            int * slen,
	                            bool * ok)
{
	skipWhite();
	if (curPtr + 2 <= endPtr)
	{
		if (curPtr[0] == '(')
      		return getLiteralString(buf, buflen, slen, ok);
    	else if (curPtr[0] == '<' && (curPtr[1] == '>' || isxdigit(curPtr[1]))) 
    	{
      		return getHexString(buf, buflen, slen, ok);
    	}
    	else if (curPtr[0] == '<' && curPtr[1] == '~')
    		return getASCII85(buf, buflen, slen, ok);
	}
	
	xwApp->warning(tr("could not find a string object.\n"));
    return curPtr;
}

char * XWLexer::getUnsigned()
{
	skipWhite();
    const char * p = curPtr;
    for (; p < endPtr; p++)
    {
        if (!isdigit(*p))
            break;
    }
    
    char * number = parsedString(curPtr, p);
    
    curPtr = p;
    return number;
}

char * XWLexer::getValIdent(const char * end)
{
	if (!end)
		end = endPtr;
		
	return parseGenIdent(end, val_valid_chars);
}

const char * XWLexer::skip(const char * p)
{
	int len = strlen(p);
	curPtr += len;
	return curPtr;
}

const char * XWLexer::skipBlank()
{
    if (!curPtr || curPtr >= endPtr)
        return curPtr;
        
    while (curPtr < endPtr && isBlank(*curPtr))
    	curPtr++;
    	
    return curPtr;
}

const char * XWLexer::skipLine()
{
	while (curPtr < endPtr && *curPtr != '\n' && *curPtr != '\r')
        curPtr++;
    if (curPtr < endPtr && *curPtr == '\r')
        curPtr++;
    if (curPtr < endPtr && *curPtr == '\n')
        curPtr++;
        
    return curPtr;
}

const char * XWLexer::skipWhite()
{
	while (curPtr < endPtr && (isSpace(*curPtr) || *curPtr == '%')) 
    {
        if (*curPtr == '%')
            skipLine();
        else
            curPtr++;
    }
    
    return curPtr;
}

char * XWLexer::parseGenIdent(const char * end, const char *valid_chars)
{
	const char * p = 0;
    for (p = curPtr; p < end; p++) 
    {
        if (!strchr(valid_chars, *p))
            break;
    }
    char * ident = parsedString(curPtr, p);

    curPtr = p;
    return ident;
}

char * XWLexer::parsedString(const char *start, const char *end)
{
	char *result = 0;
    int len = end - start;
    if (len > 0) 
    {
        result = new char[len + 1];
        if (!result)
            return 0;
            
        memcpy(result, start, len);
        result[len] = '\0';
    }

    return result;
}

int XWLexer::readCEscChar(char *r, const char **pp)
{
	int   c = 0, l = 1;
    const char *p = *pp;

    switch (p[0]) 
    {
        case 'a' : 
            c = '\a'; 
            p++; 
            break;
            
        case 'b' : 
            c = '\b'; 
            p++; 
            break;
            
        case 'f' : 
            c = '\f'; 
            p++; 
            break;
            
        case 'n' : 
            c = '\n'; 
            p++; 
            break;
            
        case 'r' : 
            c = '\r'; 
            p++; 
            break;
            
        case 't' : 
            c = '\t'; 
            p++; 
            break;
            
        case 'v' : 
            c = '\v'; 
            p++; 
            break;
            
        case '\\': 
        case '?': 
        case '\'': 
        case '\"':
            c = p[0]; 
            p++;
            break;
            
        case '\n': 
            l = 0; 
            p++; 
            break;
            
        case '\r':
            {
                p++;
                if (p < endPtr && p[0] == '\n')
                    p++;
                l = 0;
            }
            break;
            
        case '0': 
        case '1': 
        case '2': 
        case '3':
        case '4': 
        case '5': 
        case '6': 
        case '7':
            {
                int i;
                for (c = 0, i = 0; 
                     i < 3 && p < endPtr && p[0] >= '0' && p[0] <= '7'; 
                     i++, p++)
                {
                    c = (c << 3) + (p[0] - '0');
                }
            }
            break;
            
        case 'x':
            {
                int i;
                for (c = 0, i = 0, p++; 
                     i < 2 && p < endPtr && isxdigit(p[0]); 
                     i++, p++)
                {
                    c = (c << 4) + 
                        (isdigit(p[0]) ? 
                        p[0] - '0' : (islower(p[0]) ? 
                        p[0] - 'a' + 10: p[0] - 'A' + 10));
                }
            }
            break;
            
        default:
        	{
        		QString msg = QString(tr("unknown escape char sequence: "));
        		QChar ch(c);
        		msg.append(ch);
        		msg.append("\n");
        		xwApp->warning(msg);
        	}
            l = 0; 
            p++;
            break;
    }

    if (r)
        *r = (char) c;
    *pp  = p;
    return  l;
}

int XWLexer::readCLitStrC(char *q, int len, const char **pp)
{
	const char * p = *pp;
    int    l = 0;
    int    s = Q_CONT;
    while (s == Q_CONT && p < endPtr)
    {
    	switch (p[0])
    	{
    		case C_QUOTE:
            	s = Q_TERM; 
            	p++;
            	break;
            
            case C_ESCAPE:
                if (q && l == len)
                    s = Q_ERROR_BUFF;
                else 
                {
                    p++;
                    l += readCEscChar(q ? &q[l] : NULL, &p);
                }
                break;
                
            case '\n': 
            case '\r':
                s = Q_ERROR_INVAL;
                break;
                
            default:
                if (q && l == len)
                    s = Q_ERROR_BUFF;
                else 
                {
                    if (!q)
                        l++;
                    else
                        q[l++] = p[0];
                    p++;
                }
                break;
    	}
    }
    
    if (s == Q_TERM) 
    {
        if (q && l == len)
        	s = Q_ERROR_BUFF;
        else if (q)
            q[l++] = '\0';
    }

    *pp = p;
    return ((s == Q_TERM) ? l : s);
}

