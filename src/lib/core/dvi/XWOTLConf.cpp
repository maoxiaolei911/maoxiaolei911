/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWLexer.h"
#include "XWApplication.h"
#include "XWAdobeGlyphList.h"
#include "XWFontSea.h"
#include "XWFontCache.h"
#include "XWDVIRef.h"
#include "XWOTLConf.h"

#define MAX_UNICODES 16

XWOTLConf::XWOTLConf()
{
	confs.initNull();
}

XWOTLConf::~XWOTLConf()
{
	confs.free();
}

void XWOTLConf::close(XWDVIRef * xref, bool finished)
{
	if (!confs.isNull())
	{
		if (finished)
			xref->releaseObj(&confs);
		else
		{
			XWObject obj;
			confs.copy(&obj);
			xref->releaseObj(&obj);
		}
	}
}

XWObject * XWOTLConf::findConf(XWDVIRef * xref,
	                           const char *conf_name,
	                           XWObject * rule)
{
	rule->initNull();
	if (!confs.isNull())
		confs.dictLookupNF(conf_name, rule);
	else
		confs.initDict(xref);
	
	if (rule->isNull())
	{
		XWObject obj;
		readConf(xref, conf_name, &obj);
		if (!obj.isNull())
		{
			confs.dictAdd(qstrdup(conf_name), &obj);			
			XWObject script;
			obj.dictLookupNF("script", &script);
			
			XWObject language;
			obj.dictLookupNF("language", &language);
			
			XWObject options;
			obj.dictLookupNF("option", &options);
			
			if (script.isNull())
			{
				script.initString(new XWString("*", 1));
				obj.dictAdd(qstrdup("script"), &script);
				QString msg = QString("script unspecified in \"%s\"...\n").arg(conf_name);
				xwApp->warning(msg);
			}
			
			if (language.isNull())
			{
				script.initString(new XWString("dflt", 4));
				obj.dictAdd(qstrdup("language"), &script);
				QString msg = QString("language unspecified in \"%s\"...\n").arg(conf_name);
				xwApp->warning(msg);
			}
			
			if (!options.isNull())
			{
				XWObject optkeys;
				optkeys.initArray(xref);
				options.dictKeys(&optkeys);
				int num_opts = optkeys.arrayGetLength();
				XWObject key, opt, obj1;
				for (int i = 0; i < num_opts; i++)
				{
					optkeys.arrayGetNF(i, &key);
					options.dictLookupNF(key.getName(), &opt);
					key.free();
					opt.dictLookupNF("script", &obj1);
					if (obj1.isNull())
					{
						script.copy(&obj1);
						opt.dictAdd(qstrdup("script"), &obj1);
					}
					else
						obj1.free();
						
					opt.dictLookupNF("language", &obj1);
					if (obj1.isNull())
					{
						language.copy(&obj);
						opt.dictAdd(qstrdup("language"), &obj1);
					}
					else
						obj1.free();
						
					opt.free();
				}
				
				optkeys.free();
			}
			
			script.free();
			language.free();
			options.free();
		}
		
		obj.copy(rule);
	}
	
	return rule;
}

XWObject * XWOTLConf::findOpt(XWObject *conf, 
	                          const char *opt_tag, 
	                          XWObject * opt_conf)
{
	XWObject options;
	conf->dictLookupNF(opt_tag, &options);
	if (!options.isNull())
		options.dictLookupNF(opt_tag, opt_conf);
	else
		return 0;
	
	options.free();
	return opt_conf;
}

char * XWOTLConf::getLanguage(XWObject *conf)
{
	XWObject * language = conf->dictLookupNFOrg("language");
	if (language && language->isString())
		return language->getString()->getCString();
		
	return 0;
}

XWObject * XWOTLConf::getRule(XWObject *conf, XWObject * rule)
{
	return conf->dictLookupNF("rule", rule);
}

char * XWOTLConf::getScript(XWObject *conf)
{
	XWObject * script = conf->dictLookupNFOrg("script");
	if (script && script->isString())
		return script->getString()->getCString();
		
	return 0;
}

void XWOTLConf::init(XWDVIRef * xref)
{
	if (!confs.isNull())
	{
		confs.free();
		confs.initNull();
	}
		
	confs.initDict(xref);
}

void XWOTLConf::addRule(XWDVIRef * xref,
	                    XWObject *rule, 
	                    XWObject *gclass,
	  			        char *first, 
	  			        char *second, 
	  			        char *suffix)
{
	XWObject glyph1, glyph2;
	
	if (first[0] == '@')
	{
		gclass->dictLookupNF(&first[1], &glyph1);
		if (glyph1.isNull())
		{
			QString msg = QString("no glyph class \"%1\" found.\n").arg(&first[1]);
			xwApp->warning(msg);
			return ;
		}
	}
	else
	{
		XWFontCache fcache(true);
		long unicodes[MAX_UNICODES];
		long n_unicodes = fcache.aglGetUnicodes(first, unicodes, MAX_UNICODES);
		if (n_unicodes < 1) 
		{
			QString msg = QString("failed to convert glyph \"%s\" to Unicode sequence.\n").arg(first);
			xwApp->warning(msg);
      		return;
    	}
    	
    	glyph1.initArray(xref);
    	for (int i = 0; i < n_unicodes; i++) 
    	{
    		glyph2.initInt((long)(unicodes[i]));
    		glyph1.arrayAdd(&glyph2);
    	}
	}
	
	if (second[0] == '@')
	{
		gclass->dictLookupNF(&second[1], &glyph2);
		if (glyph2.isNull())
		{
			glyph1.free();
			QString msg = QString("no glyph class \"%s\" found.\n").arg(&second[1]);
			xwApp->warning(msg);
      		return;
		}
	}
	else
	{
		long unicodes[MAX_UNICODES];
		XWFontCache fcache(true);
		long n_unicodes = fcache.aglGetUnicodes(second, unicodes, 16);
		if (n_unicodes < 1) 
		{
			glyph1.free();
			QString msg = QString("failed to convert glyph \"%s\" to Unicode sequence.\n").arg(second);
			xwApp->warning(msg);
      		return;
    	}
    	
    	glyph2.initArray(xref);
    	XWObject obj;
    	for (int i = 0; i < n_unicodes; i++)
    	{
    		obj.initInt((long)(unicodes[i]));
    		glyph2.arrayAdd(&obj);
    	}
	}
	
	if (suffix)
	{
		XWObject obj;
		obj.initString(new XWString(suffix, strlen(suffix)));
		rule->arrayAdd(&obj);
	}
	else
	{
		XWObject obj;
		obj.initNull();
		rule->arrayAdd(&obj);
	}
	
	rule->arrayAdd(&glyph1);
	rule->arrayAdd(&glyph2);
}

XWObject * XWOTLConf::parseBlock(XWDVIRef * xref,
	                             XWObject *gclass, 
	                             XWLexer  *lexer, 
	                             XWObject *rule)
{
	const char * p = lexer->skipWhite();
	const char * endptr = lexer->getEndPtr();
	if (p < endptr && *p == '{')
    	p = lexer->skip(1);
    	
    p = lexer->skipWhite();
    if (p >= endptr)
    	return 0;
    	
    rule->initDict(xref);
    while (p < endptr && *p != '}')
    {
    	p = lexer->skipWhite();
    	if (p >= endptr)
      		break;
      		
      	if (*p == '#') 
      	{
      		while (p < endptr) 
      		{
				if (*p == '\r' || *p == '\n') 
				{
	  				p = lexer->skip(1);
	  				break;
				}
				p = lexer->skip(1);
      		}
      		continue;
    	} 
    	else if (*p == ';') 
    	{
      		p = lexer->skip(1);
      		continue;
    	}
    	
    	p = lexer->skipWhite();
    	char * token = lexer->getCIdent();
    	if (!token)
      		break;
      		
      	if (!strcmp(token, "script") || !strcmp(token, "language"))
      	{
      		p = lexer->skipWhite();
      		int len = 0;
      		while (p + len < endptr && *(p + len) != ';') 
				len++;
				
			if (len > 0) 
			{
				char * tmp = new char[len+1];
				memset(tmp, 0, len+1);
				for (int i = 0; i < len; i++) 
				{
	  				if (!isspace(*p))
	    				tmp[i] = *p;
	  				p = lexer->skip(1);
				}
				
				XWObject obj;
				obj.initString(new XWString(tmp, strlen(tmp)));
				rule->dictAdd(qstrdup(token), &obj);

				delete [] tmp;
      		}
      	}
      	else if (!strcmp(token, "option"))
      	{
      		XWObject opt_dict;
      		rule->dictLookupNF("option", &opt_dict);
      		if (opt_dict.isNull()) 
      		{
      			opt_dict.initDict(xref);
      			rule->dictAdd(qstrdup("option"), &opt_dict);
      		}
      		
      		p = lexer->skipWhite();
      		char * tmp = lexer->getCIdent();
      		p = lexer->skipWhite();
      		XWObject opt_rule;
      		parseBlock(xref, gclass, lexer, &opt_rule);
      		opt_dict.dictAdd(qstrdup(tmp), &opt_rule);
      		delete [] tmp;
      	}
      	else if (!strcmp(token, "prefered") ||
	       		 !strcmp(token, "required") ||
	       		 !strcmp(token, "optional"))
	    {
	    	p = lexer->skipWhite();
      		if (p >= endptr || *p != '{')
      		{
				xwApp->error("syntax error (1)\n");
				rule->free();
				rule->initNull();
				return 0;
			}
			
			XWObject rule_block;
			parseSubstRule(xref, gclass, lexer, &rule_block);
			
			XWObject subst;
			rule->dictLookupNF("rule", &subst);
			if (subst.isNull()) 
			{
				subst.initArray(xref);
				rule->dictAdd(qstrdup("rule"), &subst);
      		}
      		XWObject obj;
      		obj.initInt(token[0]);
      		subst.arrayAdd(&obj);
      		subst.arrayAdd(&rule_block);
	    }
	    else if (token[0] == '@')
	    {
	    	p = lexer->skipWhite();
	    	p = lexer->skip(1);
	    	p = lexer->skipWhite();
	    	
	    	XWObject coverage;
	    	parseUCCoverage(xref, gclass, lexer, &coverage);
	    	gclass->dictAdd(qstrdup(&token[1]), &coverage);
	    }
	    
	    delete [] token;
    	p = lexer->skipWhite();
    }
    
    if (p < endptr && *p == '}')
    	p = lexer->skip(1);
    	
  	return rule;
}

XWObject * XWOTLConf::parseSubstRule(XWDVIRef * xref,
	                                 XWObject *gclass, 
	                                 XWLexer * lexer, 
	                                 XWObject *substrule)
{
	const char * p = lexer->skipWhite();
	const char * endptr = lexer->getEndPtr();
	if (p < endptr && *p == '{')
    	p = lexer->skip(1);
    	
    p = lexer->skipWhite();
    if (p >= endptr)
    	return 0;
    	
    substrule->initArray(xref);
    XWObject obj;    
    while (p < endptr && *p != '}')
    {
    	p = lexer->skipWhite();
    	if (p >= endptr)
      		break;
      		
      	if (*p == '#') 
      	{
      		while (p < endptr) 
      		{
				if (*p == '\r' || *p == '\n') 
				{
	  				p = lexer->skip(1);
	  				break;
				}
				p = lexer->skip(1);
      		}
      		continue;
    	} 
    	else if (*p == ';') 
    	{
      		p = lexer->skip(1);
      		continue;
    	}
    	
    	p = lexer->skipWhite();
    	char * token = lexer->getCIdent();
    	if (!token)
      		break;
      		
      	if (!strcmp(token, "assign") || !strcmp(token, "substitute"))
      	{
      		p = lexer->skipWhite();
      		char * first = lexer->getCIdent();
      		if (!first)
      		{
				xwApp->error("syntax error (1)\n");
				delete [] token;
				return 0;
			}
			
			p = lexer->skipWhite();
			char * tmp = lexer->getCIdent();
			if (!tmp || (tmp && strcmp(tmp, "by") && strcmp(tmp, "to")))
			{
				QString msg = QString("syntax error (2): %1\n").arg(p);
				xwApp->error(msg);
				if (tmp)
					delete [] tmp;
				delete [] token;
				delete [] first;
				return 0;
			}
			
			p = lexer->skipWhite();
			char * second = lexer->getCIdent(); /* allows @ */
      		if (!second)
      		{
				xwApp->error("syntax error (3)\n");
				delete [] tmp;
				delete [] token;
				delete [] first;
				return 0;
			}
			
			obj.initName(token);
			substrule->arrayAdd(&obj);
			char * suffix = 0;
			if (p + 1 < endptr && *p == '.')
			{
				p = lexer->skip(1);
				suffix = lexer->getCIdent();
			}
			
			addRule(xref, substrule, gclass, first, second, suffix);
			
			delete [] first;
      		delete [] tmp;
      		delete [] second;
      		if (suffix)
      			delete [] suffix;
      	}
      	else
      	{
      		QString msg = QString("unkown command %s.\n").arg(token);
			xwApp->error(msg);
			delete [] token;
			return 0;
      	}
      	
      	delete [] token;
    	p = lexer->skipWhite();
    }
    
    if (p < endptr && *p == '}')
    	p = lexer->skip(1);
  	return substrule;
}

XWObject * XWOTLConf::parseUCCoverage(XWDVIRef * xref,
	                                  XWObject *gclass, 
	                                  XWLexer * lexer, 
	                                  XWObject *coverage)
{
	const char * p = lexer->getCurPtr();
	const char * endptr = lexer->getEndPtr();
	if (p + 1 >= endptr)
		return 0;
	
	if (*p == '[')
    	p = lexer->skip(1);
    	
    coverage->initArray(xref);
    char * glyphname  = 0;
    long   ucv = 0;
    XWFontCache fcache(true);
    while (p < endptr)
    {
    	p = lexer->skipWhite();
    	switch (*p)
    	{
    		case ']': 
    		case ';':
      			p = lexer->skip(1);
      			return coverage;
      			break;
      			
      		case ',':
      			p = lexer->skip(1);
      			break;
      			
      		case '@':
      			p = lexer->skip(1);
      			{
      				char * glyphclass = lexer->getCIdent();
      				if (!glyphclass)
      					return 0;
      					
      				XWObject cvalues;
      				gclass->dictLookupNF(glyphclass, &cvalues);
      				if (!cvalues.isNull())
      				{
      					XWObject obj;
      					XWObject obj1;
      					int sizeA = cvalues.arrayGetLength();
      					for (int i = 0; i < sizeA; i++) 
      					{
      						cvalues.arrayGetNF(i, &obj);
      						obj.copy(&obj1);
      						obj.free();
      						coverage->arrayAdd(&obj1);
						}
						
						cvalues.free();
      				}
      				else
      				{
      					QString msg = QString("%1 not defined...\n").arg(glyphclass);
      					xwApp->error(msg);
      					delete [] glyphclass;
      					return 0;
      				}
      				
      				delete [] glyphclass;
      			}
      			break;
      			
      		default:
      			{
      				XWObject value;
      				glyphname  = lexer->getCIdent();
      				if (!glyphname)
      				{
      					xwApp->error("Invalid Unicode character specified.\n");
      					return 0;
      				}
      				
      				p = lexer->skipWhite();
      				if (p + 1 < endptr && *p == '-') 
      				{
      					long ucv = -1;
						value.initArray(xref);
						if (fcache.aglGetUnicodes(glyphname, &ucv, 1) != 1)
						{
							delete [] glyphname;
							value.free();
							return 0;
						}
						XWObject obj;
						obj.initInt((int)ucv);
						value.arrayAdd(&obj);
						delete [] glyphname;
						
						p = lexer->skip(1);
						p = lexer->skipWhite();
						glyphname = lexer->getCIdent();
						if (!glyphname)
						{
							value.free();
	  						goto baduc;
	  					}
	  					
	  					if (fcache.aglGetUnicodes(glyphname, &ucv, 1) != 1)
	  					{
	  						value.free();
	  						goto baduc;
	  					}
	  					
	  					obj.initInt((int)ucv);
						value.arrayAdd(&obj);
						delete [] glyphname;
					}
					else
					{
						if (fcache.aglGetUnicodes(glyphname, &ucv, 1) != 1)
	  					{
	  						value.free();
	  						goto baduc;
	  					}
						value.initInt((int)ucv);
						delete [] glyphname;
      				}
      				coverage->arrayAdd(&value);
      			}
      			break;
    	}
    	
    	p = lexer->skipWhite();
    }
    
    return coverage;
    
baduc:
	if (glyphname)
	{
		QString msg = QString("Invalid Unicode char: %1\n").arg(glyphname);
      	xwApp->error(msg);
      	delete [] glyphname;
	}
	
	return 0;
}

XWObject * XWOTLConf::readConf(XWDVIRef * xref,
	                           const char *conf_name, 
	                           XWObject * rule)
{
	rule->initNull();
	QString filename = QString("%1.otl").arg(conf_name);	
	XWFontSea sea;	
	QFile * fp = sea.openOtl(filename);
	if (!fp)
		return 0;
		
	if (fp->size() < 1)
	{
		fp->close();
		delete fp;
		return 0;
	}
	
	QByteArray ba = fp->readAll();
	fp->close();
	delete fp;
	
	XWLexer lexer(ba.constData(), ba.size());
	
	XWObject gclass;
	gclass.initDict(xref);
	parseBlock(xref, &gclass, &lexer, rule);
	gclass.free();
	return rule;
}

