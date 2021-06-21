/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <QByteArray>
#include "XWNameToCharCode.h"


struct NameToCharCodeEntry 
{
    char *name;
    unsigned int   c;
};


XWNameToCharCode::XWNameToCharCode()
{
    size = 31;
    len = 0;
    tab = (NameToCharCodeEntry *)malloc(size * sizeof(NameToCharCodeEntry));
    for (int i = 0; i < size; ++i) 
        tab[i].name = 0;
}

XWNameToCharCode::~XWNameToCharCode()
{
    for (int i = 0; i < size; ++i) 
    {
        if (tab[i].name) 
            delete [] tab[i].name;
    }
    free(tab);
}

void XWNameToCharCode::add(char *name, unsigned int c)
{
    if (len >= size / 2) 
    {
        int oldSize = size;
        NameToCharCodeEntry * oldTab = tab;
        size = 2*size + 1;
        tab = (NameToCharCodeEntry *)malloc(size * sizeof(NameToCharCodeEntry));
        for (int h = 0; h < size; ++h) 
            tab[h].name = 0;
            
        for (int i = 0; i < oldSize; ++i) 
        {
            if (oldTab[i].name) 
            {
	            int h = hash(oldTab[i].name);
	            while (tab[h].name) 
	            {
	                if (++h == size) 
	                    h = 0;
	            }
	            tab[h] = oldTab[i];
            }
        }
        free(oldTab);
    }
    
    int h = hash(name);
    while (tab[h].name && strcmp(tab[h].name, name)) 
    {
        if (++h == size) 
            h = 0;
    }
    if (!tab[h].name) 
        tab[h].name = qstrdup(name);
    tab[h].c = c;

    ++len;
}

int  XWNameToCharCode::lookup(char *name)
{
    int h = hash(name);
    while (tab[h].name) 
    {
        if (!strcmp(tab[h].name, name)) 
            return tab[h].c;
            
        if (++h == size) 
            h = 0;
    }
    return 0;
}

int XWNameToCharCode::hash(char *name)
{
    uint h = 0;
    for (char * p = name; *p; ++p) 
        h = 17 * h + (int)(*p & 0xff);
    return (int)(h % size);
}

