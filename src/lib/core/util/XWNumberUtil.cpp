/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWNumberUtil.h"

long scaleFixWord(long sq, long fw)
{
	int sign = 1;
	if (sq < 0) 
	{
    	sign = -sign;
    	sq = -sq;
  	}
  	
  	if (fw < 0) 
  	{
    	sign = -sign;
    	fw = -fw;
  	}
  	
  	ulong a = ((ulong) sq) >> 16u;
  	ulong b = ((ulong) sq) & 0xffffu;
  	ulong c = ((ulong) fw) >> 16u;
  	ulong d = ((ulong) fw) & 0xffffu;
  	ulong ad = a*d; 
  	ulong bd = b*d; 
  	ulong bc = b*c; 
  	ulong ac = a*c;
  	ulong e = bd >> 16u;
  	ulong f = ad >> 16u;
  	ulong g = ad & 0xffffu;
  	ulong h = bc >> 16u;
  	ulong i = bc & 0xffffu;
  	ulong j = ac >> 16u;
  	ulong k = ac & 0xffffu;
  	ulong result = (e+g+i + (1<<3)) >> 4u;
  	result += (f+h+k) << 12u;
  	result += j << 28u;
  	return (sign > 0) ? result : result * -1L;
}

long toFixWord(double num)
{
	QByteArray ba = QByteArray::number(num, 'f', 7);
    int intpart = 0;
    int i = 0;
    int len = ba.size();
    bool negative = false;
    if (ba[0] == '-')
    {
    	negative = true;
    	i++;
    }
    else if (ba[0] == '+')
    	i++;
    	
    while ((i < len) && (ba[i] != '.'))
    {
        intpart = intpart * 10 + ba[i] - '0';
        i++;
    }
    
    if (intpart >= 2048)
    	intpart = 2046;
    
    long acc = 0;
    if (i < len && ba[i] == '.')
    {
        i++;
        int fractiondigits[8];
        int j = 0;
        while (i < len)
        {
            if (j < 7)
            {
                j++;
                fractiondigits[j] = 2097152 * (ba[i] - '0');
            }
            i++;
        }
        
        while (j > 0)
        {
            acc = fractiondigits[j] + (acc / 10); 
            j--;
        }
        
        acc = (acc + 10) / 20;
        if (acc >= 1048576)
        	acc = 1048575;
    }
    
    acc = intpart * 1048576 + acc;
    
    return negative ? (-acc) : acc;
}

