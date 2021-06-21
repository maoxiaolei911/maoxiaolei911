/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWOTLCONF_H
#define XWOTLCONF_H

#include <QString>
#include "XWObject.h"

class XWLexer;
class XWDVIRef;

class XWOTLConf
{
public:
	XWOTLConf();
	~XWOTLConf();
	
	void close(XWDVIRef * xref, bool finished = true);
	
	XWObject * findConf(XWDVIRef * xref,
	                    const char *conf_name, 
	                    XWObject * rule);
	                    
	static XWObject * findOpt(XWObject *conf, 
	                          const char *opt_tag, 
	                          XWObject * opt_conf);
	                    
	static char * getLanguage(XWObject *conf);
	static XWObject * getRule(XWObject *conf, XWObject * rule);
	static char * getScript(XWObject *conf);
	
	void init(XWDVIRef * xref);
	
private:
	void addRule(XWDVIRef * xref,
	             XWObject *rule, 
	             XWObject *gclass,
	  			 char *first, 
	  			 char *second, 
	  			 char *suffix);
	  			 
	XWObject * parseBlock(XWDVIRef * xref,
	                      XWObject *gclass, 
	                      XWLexer  *lexer, 
	                      XWObject *rule);
	XWObject * parseSubstRule(XWDVIRef * xref,
	                           XWObject *gclass, 
	                           XWLexer * lexer, 
	                           XWObject *substrule);
	XWObject * parseUCCoverage(XWDVIRef * xref,
	                           XWObject *gclass, 
	                           XWLexer * lexer, 
	                           XWObject *coverage);
	                           
	XWObject * readConf(XWDVIRef * xref,
	                    const char *conf_name, 
	                    XWObject * rule);

private:
	XWObject confs;
};

#endif //XWOTLCONF_H

