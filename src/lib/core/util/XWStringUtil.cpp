/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWStringUtil.h"


int xtoi(char ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';
        
    if (ch >= 'A' && ch <= 'F')
        return (ch - 'A') + 10;
        
    if (ch >= 'a' && ch <= 'f')
        return (ch - 'a') + 10;

    return -1;
}

int sgetUnsignedPair(unsigned char *s)
{
	int byte = 0;
  	int pair = 0;
  	for (int i = 0; i < 2; i++) 
  	{
    	byte = *(s++);
    	pair = pair*0x100u + byte;
  	}
  	return pair;
}

#define next_byte() (*((*start)++))


char  signedByte(uchar **start, uchar *end)
{
	int byte = 0;
  	if (*start < end) 
  	{
    	byte = next_byte();
    	if (byte >= 0x80) 
      		byte -= 0x100;
  	}
  	
  	return (char)byte;
}

short signedPair(uchar **start, uchar *end)
{
	long pair = 0;
  	if (end - *start > 1) 
  	{
    	for (char i = 0; i < 2; i++) 
      		pair = pair*0x100 + next_byte();
    	
    	if (pair >= 0x8000) 
      		pair -= 0x10000l;
  	} 
  	
  	return (short) pair;
}

long signedQuad(uchar **start, uchar *end)
{
	long quad = 0;
  	if (end-*start > 3) 
  	{
    	uchar byte = next_byte();
    	quad = byte;
    	if (quad >= 0x80) 
      		quad = byte - 0x100;
      		
    	for (char i = 0; i < 3; i++) 
      		quad = quad*0x100 + next_byte();
  	}
  	
  	return quad;
}

long  signedTriple(uchar **start, uchar *end)
{
	long triple = 0;
  	if (end-*start > 2) 
  	{
    	for (char i = 0; i < 3; i++) 
      		triple = triple*0x100 + next_byte();
    	
    	if (triple >= 0x800000l) 
       		triple -= 0x1000000l;
  	}
  	
  	return triple;
}

uchar unsignedByte(uchar **start, uchar *end)
{
	uchar byte = 0;
  	if (*start < end)
    	byte = next_byte();
  	return byte;
}

ushort unsignedPair(uchar **start, uchar *end)
{
	ushort pair = 0;
  	if (end-*start > 1) 
  	{
    	for (char i = 0; i < 2; i++) 
    	{
      		uchar byte = next_byte();
      		pair = pair*0x100u + byte;
    	}
  	}
  	
  	return pair;
}

ulong  unsignedQuad(uchar **start, uchar *end)
{
	ulong quad = 0;
  	if (end-*start > 3) 
  	{
    	for (char i = 0; i < 4; i++) 
      		quad = quad*0x100u + next_byte();
  	}
  	
  	return quad;
}

long unsignedTriple(uchar **start, uchar *end)
{
	long triple = 0;
  	if (end-*start > 2) 
  	{
    	for (char i = 0; i < 3; i++) 
      		triple = triple*0x100u + next_byte();
  	}
  	return triple;
}
