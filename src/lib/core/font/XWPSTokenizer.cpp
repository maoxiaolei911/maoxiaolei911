/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "XWPSTokenizer.h"

static char specialChars[256] = {
  1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0,   // 0x
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // 1x
  1, 0, 0, 0, 0, 2, 0, 0, 2, 2, 0, 0, 0, 0, 0, 2,   // 2x
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0,   // 3x
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // 4x
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 0,   // 5x
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // 6x
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 0,   // 7x
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // 8x
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // 9x
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // ax
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // bx
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // cx
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // dx
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // ex
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0    // fx
};

XWPSTokenizer::XWPSTokenizer(int (*getCharFuncA)(void *), void *dataA)
{
    getCharFunc = getCharFuncA;
    data = dataA;
    charBuf = -1;
}

bool XWPSTokenizer::getToken(char *buf, int size, int *length)
{
    bool comment = false;
    int c = EOF;
    while (1) 
    {
        if ((c = getChar()) == EOF) 
        {
            buf[0] = '\0';
            *length = 0;
            return false;
        }
        
        if (comment) 
        {
            if (c == '\x0a' || c == '\x0d') 
	            comment = false;
	            
        } 
        else if (c == '%') 
            comment = true;
        else if (specialChars[c] != 1) 
            break;
    }
    
    int i = 0;
    buf[i++] = c;
    if (c == '(') 
    {
        bool backslash = false;
        while ((c = lookChar()) != EOF) 
        {
            if (i < size - 1) 
	            buf[i++] = c;
            
            getChar();
            
            if (c == '\\') 
	            backslash = true;
            else if (!backslash && c == ')') 
	            break;
            else 
	            backslash = false;
        }
    } 
    else if (c == '<') 
    {
        while ((c = lookChar()) != EOF) 
        {
            getChar();
            if (i < size - 1 && specialChars[c] != 1) 
	            buf[i++] = c;
            
            if (c == '>') 
	            break;
        }
    } 
    else if (c != '[' && c != ']') 
    {
        while ((c = lookChar()) != EOF && !specialChars[c]) 
        {
            getChar();
            if (i < size - 1) 
	            buf[i++] = c;
        }
    }
    buf[i] = '\0';
    *length = i;

    return true;
}

int XWPSTokenizer::lookChar()
{
    if (charBuf < 0) 
        charBuf = (*getCharFunc)(data);
        
    return charBuf;
}

int XWPSTokenizer::getChar()
{
    if (charBuf < 0) 
        charBuf = (*getCharFunc)(data);
        
    int c = charBuf;
    charBuf = -1;
    return c;
}

