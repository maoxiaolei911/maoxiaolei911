/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWPSTOKENIZER_H
#define XWPSTOKENIZER_H

class XWPSTokenizer
{
public:
    XWPSTokenizer(int (*getCharFuncA)(void *), void *dataA);
    ~XWPSTokenizer() {}
    
    bool getToken(char *buf, int size, int *length);
    
private:
    int getChar();
    
    int lookChar();
    
private:
    int (*getCharFunc)(void *);
    void *data;
    int charBuf;
};

#endif // XWPSTOKENIZER_H

