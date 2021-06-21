/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <malloc.h>
#include <QDateTime>
#include <QTextStream>
#include "XWApplication.h"
#include "XWTeX.h"

qint32 XWTeX::addOrSub(qint32 x, 
	                   qint32 y, 
	                   qint32 max_answer, 
	                   bool negative)
{
	qint32 a = 0;
	if (negative)
		y = -y;
		
	if (x >= 0)
	{
		if (y <= (max_answer - x))
			a = x + y;
		else
			numError(a);
	}
	else if (y >= (-max_answer - x))
		a = x + y;
	else
		numError(a);
	
	return a;
}

qint32 XWTeX::badness(qint32 t, qint32 s)
{
	if (t == 0)
		return 0;
		
	if (s <= 0)
		return TEX_INF_BAD;
		
	qint32 r = t;
	if (t <= 7230584)
		r = (t * 297) / s;
	else if (s >= 1663497)
		r = t / (s / 297);
		
	if (r > 1290)
		return TEX_INF_BAD;
		
	return ((r * r * r + 131072) / 262144);
}

qint32 XWTeX::fract(qint32 x, 
	                qint32 n, 
	                qint32 d, 
	                qint32 max_answer)
{
	bool negative;
	qint32 a, f, h, r, t;
	
	if (d == 0)
		goto too_big;
		
	a = 0;
	if (d > 0)
		negative = false;
	else
	{
		d = -d;
		negative = true;
	}
	
	if (x < 0)
	{
		x = -x;
		negative = !negative;
	}
	else if (x == 0)
		goto done;
		
	if (n < 0)
	{
		n = -n; 
		negative = !negative;
	}
	
	t = n / d;
	if (t > (max_answer / x))
		goto too_big;
		
	a = t * x; 
	n = n - t * d;
	if (n == 0)
		goto found;
		
	t = x / d;
	if (t > ((max_answer - a) / n))
		goto too_big;
		
	a = a + t * n; 
	x = x - t * d;
	if (x == 0)
		goto found;
		
	if (x < n)
	{
		t = x; 
		x = n; 
		n = t;
	}
	
	f = 0; 
	r = (d / 2) - d; 
	h = -r;	
	while (true)
	{
		if (odd(n))
		{
			r = r + x;
			if (r >= 0)
			{
				r = r - d; 
				f++;
			}
		}
		
		n = n / 2;
		if (n == 0)
			goto found1;
		if (x < h)
			x = x + x;
		else
		{
			t = x - d; 
			x = t + x; 
			f = f + n;
			if (x < n)
			{
				if (x == 0)
					goto found1;
					
				t = x; 
				x = n; 
				n = t;
			}
		}
	}
	
found1:
	if (f > (max_answer - a))
		goto too_big;
		
	a = a + f;
	
found:
	if (negative)
		a = -a;
	
	goto done;
	
too_big: 
	numError(a);
	
done:
	return a;
}

qint32 XWTeX::multAndAdd(qint32 n, qint32 x, qint32 y, qint32 max_answer)
{
	if (n < 0)
	{
		x = -x;
		n = -n;
	}
	
	if (n == 0)
		return y;
		
	if (((x <= ((max_answer - y) / n)) && (-x <= ((max_answer + y) / n))))
		return n*x+y;
		
	arith_error = true;
	return 0;
}

char XWTeX::normMin(qint32 h)
{
	if (h <= 0)
		return (char)1;
	else if (h >= 63)
		return (char)63;
		
	return (char)h;
}

qint32 XWTeX::quotient(qint32 n, qint32 d)
{
	bool negative;
	qint32 a;
	if (d == 0)
		numError(a);
	else
	{
		if (d > 0)
			negative = false;
		else
		{
			d = -d; 
			negative = true;
		}
		
		if (n < 0)
		{
			n = -n; 
			negative = !negative;
		}
		
		a = n / d; 
		n = n - a * d; 
		d = n - d;
		if ((d + n) >= 0)
			a++;
			
		if (negative)
			a = -a;
	}
	
	return a;
}

qint32 XWTeX::xnOverD(qint32 x, qint32 n, qint32 d)
{
	bool positive = true;
	if (x < 0)
	{
		x = -x;
		positive = false;
	}
	
	qint32 t = (x % 32768) * n;
	qint32 u = (x / 32768) * n + (t / 32768);
	qint32 v = (u % d) * 32768 + (t % 32768);
	if ((u / d) >= 32768)
		arith_error = true;
	else
		u = 32768 * (u / d) + (v / d);
		
	if (positive)
	{
		remainder = v % d;
		return u;
	}
	
	remainder = -(v % d);
	return -u;
}

qint32 XWTeX::xOverN(qint32 x, qint32 n)
{
	bool negative = false;
	if (n == 0)
	{
		arith_error = true; 
		remainder = x;
		return 0;
	}
	
	if (n < 0)
	{
		x = -x;
		n = -n;
		negative = true;
	}
	
	qint32 ret = 0;
	if (x >= 0)
	{
		ret = x / n; 
		remainder = x % n;
	}
	else
	{
		ret = -((-x) / n); 
		remainder = -((-x) % n);
	}
	
	if (negative)
		remainder = -remainder;
		
	return ret;
}
