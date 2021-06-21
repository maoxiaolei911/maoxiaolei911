/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWFONTFILEID_H
#define XWFONTFILEID_H

#include "XWGlobal.h"

class XW_FONT_EXPORT XWFontFileID
{
public:
	XWFontFileID();
    XWFontFileID(const char * nameA);
    XWFontFileID(int numA, int genA);
    virtual ~XWFontFileID();
    
    int getSubstIdx() { return substIdx; }
    
    virtual bool matches(XWFontFileID *id);
    
    void setSubstIdx(int substIdxA) { substIdx = substIdxA; }
    
protected:
	int num;
	int gen;
	int substIdx;
	char * name;
};


#endif // XWFONTFILEID_H

