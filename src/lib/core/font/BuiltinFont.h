/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef BUILTINFONT_H
#define BUILTINFONT_H

#include "XWGlobal.h"

struct BuiltinFont;
class BuiltinFontWidths;

//------------------------------------------------------------------------

struct XW_FONT_EXPORT BuiltinFont 
{
    char *name;
    char **defaultBaseEnc;
    short ascent;
    short descent;
    short bbox[4];
    BuiltinFontWidths *widths;
};

//------------------------------------------------------------------------

struct XW_FONT_EXPORT BuiltinFontWidth 
{
    char *name;
    unsigned short width;
    BuiltinFontWidth *next;
};

class XW_FONT_EXPORT BuiltinFontWidths 
{
public:
    BuiltinFontWidths(BuiltinFontWidth *widths, int sizeA);
    ~BuiltinFontWidths();
    bool getWidth(char *name, unsigned short *width);

private:
    int hash(char *name);

private:
    BuiltinFontWidth **tab;
    int size;
};

#endif
