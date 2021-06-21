/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPDFOTLCONF_H
#define XWPDFOTLCONF_H

#include "XWObject.h"

class XWLexer;
class XWPDFDriverRef;

class XWPDFOTLConf
{
public:
	XWPDFOTLConf(XWPDFDriverRef * xref);
	~XWPDFOTLConf();
	
	XWObject * findConf(XWPDFDriverRef * xref,
	                    const char *conf_name, 
	                    XWObject * rule);
	                    
	static XWObject * findOpt(XWObject *conf, 
	                          const char *opt_tag, 
	                          XWObject * opt_conf);
	                    
	static char * getLanguage(XWObject *conf);
	static XWObject * getRule(XWObject *conf, XWObject * rule);
	static char * getScript(XWObject *conf);
	
private:
	void addRule(XWPDFDriverRef * xref,
	             XWObject *rule, 
	             XWObject *gclass,
	  			 char *first, 
	  			 char *second, 
	  			 char *suffix);

	void init(XWPDFDriverRef * xref);
	  			 
	XWObject * parseBlock(XWPDFDriverRef * xref,
	                      XWObject *gclass, 
	                      XWLexer  *lexer, 
	                      XWObject *rule);
	XWObject * parseSubstRule(XWPDFDriverRef * xref,
	                           XWObject *gclass, 
	                           XWLexer * lexer, 
	                           XWObject *substrule);
	XWObject * parseUCCoverage(XWPDFDriverRef * xref,
	                           XWObject *gclass, 
	                           XWLexer * lexer, 
	                           XWObject *coverage);
	                           
	XWObject * readConf(XWPDFDriverRef * xref,
	                    const char *conf_name, 
	                    XWObject * rule);

private:
	XWObject confs;
};


#endif //XWPDFOTLCONF_H

