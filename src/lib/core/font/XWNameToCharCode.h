/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWNAMETOCHARCODE_H
#define XWNAMETOCHARCODE_H


struct NameToCharCodeEntry;

class XWNameToCharCode
{
public:
    XWNameToCharCode();
    ~XWNameToCharCode();

    void add(char *name, unsigned int c);
    
    int  lookup(char *name);

private:
    int hash(char *name);

private:
    NameToCharCodeEntry *tab;
    int size;
    int len;
};

#endif // XWNAMETOCHARCODE_H
