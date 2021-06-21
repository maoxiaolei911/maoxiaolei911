/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWDVIPARSER_H
#define XWDVIPARSER_H

#include <QObject>

class XWLexer;
class XWObject;
class XWDVICore;
class XWDVIRef;
class XWPDFFile;

class XWDVIParser : public QObject
{
	Q_OBJECT
	
public:
	XWDVIParser(XWDVIRef * xrefA, 
	            XWLexer   * lexerA,
	            QObject * parent = 0);
	XWDVIParser(XWDVIRef * xrefA, 
	            XWLexer   * lexerA,
	            XWPDFFile * pfA,
	            QObject * parent = 0);
	XWDVIParser(XWDVICore * coreA,
	            XWDVIRef * xrefA, 
	            XWLexer   * lexerA,
	            XWPDFFile * pfA,
	            QObject * parent = 0);
	~XWDVIParser();
	
	XWObject * parsePDFArray(XWObject * obj);
	XWObject * parsePDFBoolean(XWObject * obj);
	XWObject * parsePDFDict(XWObject * obj);
	XWObject * parsePDFName(XWObject * obj);
	XWObject * parsePDFNull(XWObject * obj);
	XWObject * parsePDFNumber(XWObject * obj);
	XWObject * parsePDFObject(XWObject * obj);
	XWObject * parsePDFString(XWObject * obj);
	XWObject * parsePDFTaintedDict(XWObject * obj);
	
private:
	XWObject * parsePDFHexString(XWObject * obj);
	XWObject * parsePDFLiteralString(XWObject * obj);	
	XWObject * parsePDFReference(XWObject * obj);
	XWObject * parsePDFStream(XWObject *dict, XWObject * obj);
	
	XWObject * tryPDFReference(const char **endptrA, XWObject * obj);
	
private:
	XWDVICore * core;
	XWDVIRef  * xref;
	XWLexer   * lexer;
	XWPDFFile * pf;
	int tainted;
	const char * endptr;
};

#endif //XWDVIPARSER_H
