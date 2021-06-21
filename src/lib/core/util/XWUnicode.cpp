/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWUnicode.h"


#define UC_DEBUG     3
#define UC_DEBUG_STR "UC"


#define UC_REPLACEMENT_CHAR 0x0000FFFDL

#define UC_SUR_SHIFT      10
#define UC_SUR_MASK       0x3FFUL
#define UC_SUR_LOW_START  0xDC00UL
#define UC_SUR_HIGH_START 0xD800UL

int XWUnicode::putUTF16BE(unsigned char **pp, unsigned char * limptr)
{
    int count = 0;
    long ucv = unicode;
    unsigned char *p = *pp;
    if (ucv >= 0 && ucv <= 0xFFFF)
    {
    	if (p + 2 >= limptr)
    		return -1;
    		
        p[0] = (ucv >> 8) & 0xff;
        p[1] = ucv & 0xff;
        count = 2;
    }
    else if (ucv >= 0x010000L && ucv <= 0x10FFFFL)
    {
    	if (p + 4 >= limptr)
    		return -1;
    		
        ucv  -= 0x00010000L;
        unsigned short high = (ucv >> UC_SUR_SHIFT) + UC_SUR_HIGH_START;
        unsigned short low  = (ucv &  UC_SUR_MASK)  + UC_SUR_LOW_START;
        p[0] = (high >> 8) & 0xff;
        p[1] = (high & 0xff);
        p[2] = (low >> 8) & 0xff;
        p[3] = (low & 0xff);
        count = 4;
    }
    else
    {
    	if (p + 2 >= limptr)
    		return -1;
    		
        p[0] = (UC_REPLACEMENT_CHAR >> 8) & 0xff;
        p[1] = (UC_REPLACEMENT_CHAR & 0xff);
        count = 2;
    }
    
    *pp += count;
    return count;
}


