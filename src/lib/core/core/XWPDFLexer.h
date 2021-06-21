/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPDFLEXER_H
#define XWPDFLEXER_H

#include <QObject>

#include "XWObject.h"
#include "XWStream.h"

#define PDF_TOKBUFSIZE 128

class XWRef;

class XW_CORE_EXPORT XWPDFLexer : public QObject
{
    Q_OBJECT
    
public:   
    XWPDFLexer(XWRef *xref, XWStream *str, QObject * parent = 0);
    XWPDFLexer(XWRef *xref, XWObject *obj, QObject * parent = 0);
    ~XWPDFLexer();
    
    XWObject *getObj(XWObject *obj);
    int getPos()
        { return curStr.isNone() ? -1 : (int)curStr.streamGetPos(); }
    XWStream *getStream()
        { return curStr.isNone() ? (XWStream *)0 : curStr.getStream(); }
    
    static bool isSpace(int c);
    
    void setPos(uint pos, int dir = 0)
        { if (!curStr.isNone()) curStr.streamSetPos(pos, dir); }
    void skipChar() { getChar(); }
    void skipToNextLine();
    
private:
    int getChar();
    
    int lookChar();
    
private:
    XWArray *streams;
    int strPtr;
    XWObject curStr;
    bool freeArray;
    char tokBuf[PDF_TOKBUFSIZE];
};

#endif // XWPDFLEXER_H

