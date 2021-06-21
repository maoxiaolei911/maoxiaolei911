/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "FontEncodingTables.h"
#include "BuiltinFont.h"

//------------------------------------------------------------------------

BuiltinFontWidths::BuiltinFontWidths(BuiltinFontWidth *widths, int sizeA) 
{
    size = sizeA;
    tab = (BuiltinFontWidth **)malloc(size * sizeof(BuiltinFontWidth *));
    for (int i = 0; i < size; ++i) 
        tab[i] = 0;
        
    for (int i = 0; i < sizeA; ++i) 
    {
        int h = hash(widths[i].name);
        widths[i].next = tab[h];
        tab[h] = &widths[i];
    }
}

BuiltinFontWidths::~BuiltinFontWidths() 
{
    free(tab);
}

bool BuiltinFontWidths::getWidth(char *name, unsigned short *width) 
{
    int h = hash(name);
    BuiltinFontWidth * p = tab[h];
    for (; p; p = p->next) 
    {
        if (!strcmp(p->name, name)) 
        {
            *width = p->width;
            return true;
        }
    }
    return false;
}

int BuiltinFontWidths::hash(char *name) 
{
    unsigned int h = 0;
    char * p = name;
    for (; *p; ++p) 
        h = 17 * h + (int)(*p & 0xff);
        
    return (int)(h % size);
}
