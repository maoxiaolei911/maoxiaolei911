/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWUNICODE_H
#define XWUNICODE_H

#include "XWGlobal.h"

class XW_UTIL_EXPORT XWUnicode
{
public:
    XWUnicode(long ucv) : unicode(ucv) {}
    ~XWUnicode() {}

    bool isValid();

    int putUTF16BE(unsigned char **pp, unsigned char * limptr);

public:
    long unicode;
};

inline bool XWUnicode::isValid()
{
    if (unicode < 0 || (unicode >= 0x0000D800L && unicode <= 0x0000DFFFL))
        return false;
    return true;
}

#endif // XWUNICODE_H
