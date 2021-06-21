/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <ctype.h>
#include "XWStringUtil.h"
#include "XWApplication.h"
#include "XWLexer.h"
#include "XWObject.h"
#include "XWDVIRef.h"
#include "XWDVICore.h"
#include "XWPDFFile.h"
#include "XWDVISpecial.h"
#include "XWDVIParser.h"

#ifndef PDF_NAME_LEN_MAX
#define PDF_NAME_LEN_MAX 128
#endif

#ifndef PDF_STRING_LEN_MAX
#define PDF_STRING_LEN_MAX 65535
#endif

#define STRING_BUFFER_SIZE PDF_STRING_LEN_MAX+1

static char sbuf[PDF_STRING_LEN_MAX+1];

XWDVIParser::XWDVIParser(XWDVIRef * xrefA, 
	                     XWLexer   * lexerA,
	                     QObject * parent)
	:QObject(parent),
	 core(0),
	 xref(xrefA),
	 lexer(lexerA),
	 pf(0)
{
	endptr = lexer->getEndPtr();
}

XWDVIParser::XWDVIParser(XWDVIRef * xrefA, 
	                     XWLexer   * lexerA,
	                     XWPDFFile * pfA,
	                     QObject * parent)
	:QObject(parent),
	 core(0),
	 xref(xrefA),
	 lexer(lexerA),
	 pf(pfA)
{
	endptr = lexer->getEndPtr();
}

XWDVIParser::XWDVIParser(XWDVICore * coreA,
	                     XWDVIRef * xrefA, 
	                     XWLexer   * lexerA,
	                     XWPDFFile * pfA,
	                     QObject * parent)
	:QObject(parent),
	 core(coreA),
	 xref(xrefA),
	 lexer(lexerA),
	 pf(pfA)
{
	endptr = lexer->getEndPtr();
}

XWDVIParser::~XWDVIParser()
{
}

XWObject * XWDVIParser::parsePDFArray(XWObject * obj)
{
	XWObject obj1;
	const char * p = lexer->skipWhite();
	if (p + 2 > endptr || p[0] != '[') 
    	goto badarr;
    	
    p = lexer->skip(1);
    obj->initArray(xref);
    p = lexer->skipWhite();    
    while (p < endptr && p[0] != ']')
    {
    	if (!parsePDFObject(&obj1))
    		goto badarr;
    		
    	obj->arrayAdd(&obj1);
    	p = lexer->skipWhite();
    }
	
	p = lexer->skip(1);
	return obj;
	
badarr:
	obj->free();
	obj->initNull();
	xwApp->warning(tr("could not find an array object.\n"));
	return 0;
}

XWObject * XWDVIParser::parsePDFBoolean(XWObject * obj)
{
	bool v = true;
	bool ok = true;	
	lexer->getBool(&v, &ok);
	if (ok)
	{
		obj->initBool(v);
		return obj;
	}

    obj->initNull();
    return 0;
}

XWObject * XWDVIParser::parsePDFDict(XWObject * obj)
{
	const char * p = lexer->skipWhite();
	if (p + 4 > endptr || p[0] != '<' || p[1] != '<') 
    	return 0;
    	
  	p = lexer->skip(2);
  	obj->initDict(xref);
  	p = lexer->skipWhite();
  	XWObject key, value;
  	while (p < endptr && p[0] != '>')
  	{
  		p = lexer->skipWhite();
  		if (!parsePDFName(&key))
  			goto baddict;
  			
  		p = lexer->skipWhite();
  		if (!parsePDFObject(&value))
  		{
  			key.free();
  			goto baddict;
  		}
  		
  		obj->dictAdd(qstrdup(key.getName()), &value);
  		key.free();
  		p = lexer->skipWhite();
  	}
  	
  	p = lexer->skip(2);
  	return obj;
  	
baddict:
	obj->free();
	obj->initNull();
	xwApp->warning(tr("could not find a key in dictionary object.\n"));
	return 0;
}

XWObject * XWDVIParser::parsePDFName(XWObject * obj)
{
    char  name[PDF_NAME_LEN_MAX+1];
    bool ok = false;
    lexer->getName(name, PDF_NAME_LEN_MAX, &ok);
    if (ok)
    {
		obj->initName(name);
  		return obj;
  	}

    obj->initNull();
    return 0;
}

XWObject * XWDVIParser::parsePDFNull(XWObject * obj)
{
    bool ok = false;
    obj->initNull();
  	lexer->getNull(&ok);
  	if (ok)
  		return obj;
  		
    return 0;
}

XWObject * XWDVIParser::parsePDFNumber(XWObject * obj)
{
	double v = 0.0;
	bool ok = false;
  	lexer->getNumber(&v, &ok);
  	if (ok)
  	{
  		obj->initReal(v);
  		return obj;
  	}
  	
    obj->initNull();
    return 0;
}

XWObject * XWDVIParser::parsePDFObject(XWObject * obj)
{
	const char *nextptr = 0;
	XWObject * result = 0;
	const char * p = lexer->skipWhite();
	if (p >= endptr)
	{
		obj->initNull();
		xwApp->warning(tr("could not find any valid object.\n"));
    	return 0;
	}
	
	switch (p[0])
	{
		case '<':
			if (p[1] != '<')
				result = parsePDFHexString(obj);
			else
			{
				result = parsePDFDict(obj);
				p = lexer->skipWhite();
				if (result && p <= endptr - 15 && !memcmp(p, "stream", 6))
				{
					XWObject dict;
					obj->copy(&dict);
					obj->free();
					result = parsePDFStream(&dict, obj);
					dict.free();
				}
			}
			break;
			
		case '(':
    		result = parsePDFString(obj);
    		break;
    		
    	case '[':
    		result = parsePDFArray(obj);
    		break;
    		
    	case '/':
    		result = parsePDFName(obj);
    		break;
    		
    	case 'n':
    		result = parsePDFNull(obj);
    		break;
    		
    	case 't': 
    	case 'f':
    		result = parsePDFBoolean(obj);
    		break;
    		
    	case '+': 
    	case '-': 
    	case '.':
    		result = parsePDFNumber(obj);
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
  			if (pf && (result = tryPDFReference(&nextptr, obj))) 
  				lexer->setCurPtr(nextptr);
    		else 
      			result = parsePDFNumber(obj);
    		break;
    		
    	case '@':
    		result = parsePDFReference(obj);
    		break;
    		
    	default:
    		xwApp->warning(tr("unknown PDF object type.\n"));
    		obj->initNull();
    		result = 0;
    		break;
	}
	
	return result;
}

XWObject * XWDVIParser::parsePDFString(XWObject * obj)
{
	const char * p = lexer->skipWhite();
	if (p + 2 <= endptr)
	{
		if (p[0] == '(')
      		return parsePDFLiteralString(obj);
    	else if (p[0] == '<' && (p[1] == '>' || isxdigit(p[1]))) 
    	{
      		return parsePDFHexString(obj);
    	}
	}
	
	xwApp->warning(tr("could not find a string object.\n"));
	obj->initNull();
    return 0;
}

XWObject * XWDVIParser::parsePDFTaintedDict(XWObject * obj)
{
	lexer->setState();
  	XWObject * result = parsePDFDict(obj);
  	lexer->resetState();
  		
  	return result;
}

XWObject * XWDVIParser::parsePDFHexString(XWObject * obj)
{
	int len = 0;
	bool ok = false;
	lexer->getHexString(sbuf, PDF_STRING_LEN_MAX, &len, &ok);
	if (ok)
	{
  		lexer->skip(1);
  		obj->initString(new XWString(sbuf, len));
  		return obj;
  	}
  	
  	obj->initNull();
  	return 0;
}

XWObject * XWDVIParser::parsePDFLiteralString(XWObject * obj)
{
	int len = 0;
	bool ok = false;
	lexer->getLiteralString(sbuf, PDF_STRING_LEN_MAX, &len, &ok);
	if (ok)
	{
  		obj->initString(new XWString(sbuf, len));
  		return obj;
  	}
  	
  	obj->initNull();
  	return 0;
}

XWObject * XWDVIParser::parsePDFReference(XWObject * obj)
{
	if (!core)
		return 0;
		
	XWObject *result = 0;
	lexer->save();
	lexer->skipWhite();
	char * name = lexer->getOptIdent();
	if (name)
	{
		if (!core->lookupReference(name, obj))
		{
			QString msg = QString(tr("could not find the named reference (@%1).\n")).arg(name);
			xwApp->warning(msg);
      		lexer->restore();
		}
		else
			result = obj;
		
		delete [] name;
	}
	else
	{
		xwApp->warning(tr("could not find a reference name.\n"));
    	lexer->restore();
    	obj->initNull();
    	result = 0;
	}
	
	return result;
}

XWObject * XWDVIParser::parsePDFStream(XWObject *dict, XWObject * obj)
{
	const char * p = lexer->skipWhite();
	if (p + 6 > endptr || strncmp(p, "stream", 6))
	{
		obj->initNull();
		return 0;
	}
		
	p = lexer->skip(6);
	if (p < endptr && p[0] == '\n') 
    	p = lexer->skip(1);
  	else if (p + 1 < endptr && (p[0] == '\r' && p[1] == '\n')) 
    	p = lexer->skip(2);
    	
    long stream_length = -1;
    {
    	XWObject tmp;
    	dict->dictLookupNF("Length", &tmp);
    	if (!tmp.isNull())
    	{
    		XWObject tmp2;
    		XWPDFFile::derefObj(xref, &tmp, &tmp2);
    		if (tmp2.isInt())
    			stream_length = tmp2.getInt();
    		else if (tmp2.isReal())
    			stream_length = (long)tmp2.getReal();
    			
    		tmp.free();
    		tmp2.free();
    	}
    	else
    	{
    		obj->initNull();
    		return 0;
    	}
    }
    
    if (stream_length < 0 || p + stream_length > endptr)
    	return 0;
    	
    {
    	XWObject filters;
    	dict->dictLookupNF("Filter", &filters);
    	if (!filters.isNull() && stream_length > 10)
    		obj->initStream(STREAM_COMPRESS, xref);
    	else
    		obj->initStream(0, xref);
    }
    obj->streamGetDict()->merge(dict->getDict());
    obj->streamAdd(p, stream_length);
    p = lexer->skip(stream_length);
    
    {
    	if (p < endptr && p[0] == '\r')
      		p = lexer->skip(1);
    	if (p < endptr && p[0] == '\n')
      		p = lexer->skip(1);

    	if (p + 9 > endptr || memcmp(p, "endstream", 9)) 
    	{
    		obj->free();
    		obj->initNull();
      		return 0;
    	}
    	p = lexer->skip(9);
    }
    
    return obj;
}

XWObject * XWDVIParser::tryPDFReference(const char **endptrA, XWObject * obj)
{
	if (endptrA)
    	*endptrA = lexer->getCurPtr();
    	
    const char * p = lexer->skipWhite();
    if (p > endptr - 5 || !isdigit(p[0])) 
    {
    	obj->initNull();
    	return 0;
    }
    	
    int id = 0;
  	int gen = 0;
    while (!XWLexer::isSpace(p[0]))
   	{
    	if (p >= endptr || !isdigit(p[0])) 
    	{
    		obj->initNull();
      		return 0;
      	}
    	
    	id = id * 10 + (p[0] - '0');
    	p = lexer->skip(1);
  	}
  	
  	p = lexer->skipWhite();
  	if (p >= endptr || !isdigit(p[0])) 
  	{
  		obj->initNull();
      	return 0;
    }
      	
    while (!XWLexer::isSpace(p[0]))
   	{
    	if (p >= endptr || !isdigit(p[0])) 
      		return 0;
    	
    	gen = gen * 10 + (p[0] - '0');
    	p = lexer->skip(1);
  	}
  	
  	p = lexer->skipWhite();
  	
  	if (p >= endptr || p[0] != 'R')
    	return 0;
    	
  	p = lexer->skip(1);
  	if (!lexer->tokenEnd())
  	{
  		obj->initNull();
    	return 0;
    }
    
  	if (endptrA)
    	*endptrA = p;
    	
    obj->initIndirect(pf, id, gen);

  	return obj;
}

