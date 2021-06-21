/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPDFPARSER_H
#define XWPDFPARSER_H

#include <QObject>

#include "XWPDFLexer.h"

class XW_CORE_EXPORT XWPDFParser : QObject
{
    Q_OBJECT
    
public:   
    XWPDFParser(XWRef *xrefA, 
                XWPDFLexer *lexerA, 
                bool allowStreamsA, 
                QObject * parent = 0);
    virtual ~XWPDFParser();
    
    int getPos() { return lexer->getPos(); }
    
    XWObject *getObj(XWObject *obj, 
                     bool simpleOnly = false,
                     uchar *fileKey = NULL,
		 								 CryptAlgorithm encAlgorithm = cryptRC4, 
		 								 int keyLength = 0,
		 								 int objNum = 0, 
		 								 int objGen = 0, 
		 								 int recursion = 0);
		                
    XWStream *getStream() { return lexer->getStream(); }
    
private:
    XWStream *makeStream(XWObject *dict, 
                         uchar *fileKey,
		     								 CryptAlgorithm encAlgorithm, 
		     								 int keyLength,
		                     int objNum, 
		                     int objGen, 
		                     int recursion);
		                    
    void shift();
		     
private:
    XWRef *xref;	
    XWPDFLexer *lexer;
    bool allowStreams;	
    XWObject buf1, buf2;
    int inlineImg;	
};

#endif // XWPDFPARSER_H

