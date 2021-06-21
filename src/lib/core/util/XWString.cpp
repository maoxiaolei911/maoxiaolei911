/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <QTextStream>
#include "XWString.h"


union GStringFormatArg 
{
    int i;
    unsigned int ui;
    long l;
    unsigned long ul;
    double f;
    char c;
    char *s;
    XWString *gs;
};

enum GStringFormatType 
{
  fmtIntDecimal,
  fmtIntHex,
  fmtIntOctal,
  fmtIntBinary,
  fmtUIntDecimal,
  fmtUIntHex,
  fmtUIntOctal,
  fmtUIntBinary,
  fmtLongDecimal,
  fmtLongHex,
  fmtLongOctal,
  fmtLongBinary,
  fmtULongDecimal,
  fmtULongHex,
  fmtULongOctal,
  fmtULongBinary,
  fmtDouble,
  fmtDoubleTrim,
  fmtChar,
  fmtString,
  fmtGString,
  fmtSpace
};

static char *formatStrings[] = {
  "d", "x", "o", "b", "ud", "ux", "uo", "ub",
  "ld", "lx", "lo", "lb", "uld", "ulx", "ulo", "ulb",
  "f", "g",
  "c",
  "s",
  "t",
  "w",
  NULL
};

XWString::XWString()
{
	s = NULL;
  resize(length = 0);
  s[0] = '\0';
}

XWString::XWString(const char *sA)
{
  int n = (int)strlen(sA);

  s = NULL;
  resize(length = n);
  memcpy(s, sA, n + 1);
}

XWString::XWString(char * sA, int len)
{
  s = NULL;
  resize(length = len);
  memcpy(s, sA, length * sizeof(char));
  s[length] = '\0';
}

XWString::XWString(XWString *str, int idx, int lengthA)
{
  s = NULL;
  resize(length = lengthA);
  memcpy(s, str->getCString() + idx, length);
  s[length] = '\0';
}

XWString::XWString(XWString *str)
{
  s = NULL;
  resize(length = str->getLength());
  memcpy(s, str->getCString(), length + 1);
}

XWString::XWString(XWString *str1, XWString *str2)
{
  int n1 = str1->getLength();
  int n2 = str2->getLength();

  s = NULL;
  resize(length = n1 + n2);
  memcpy(s, str1->getCString(), n1);
  memcpy(s + n1, str2->getCString(), n2 + 1);
}

XWString::~XWString()
{
	if (s)
  	delete[] s;
}

XWString * XWString::append(char c)
{
  resize(length + 1);
  s[length++] = c;
  s[length] = '\0';
  return this;
}

XWString *XWString::append(XWString *str)
{
  int n = str->getLength();

  resize(length + n);
  memcpy(s + length, str->getCString(), n + 1);
  length += n;
  return this;
}

XWString *XWString::append(const char *str)
{
  int n = (int)strlen(str);

  resize(length + n);
  memcpy(s + length, str, n + 1);
  length += n;
  return this;
}

XWString *XWString::append(const char *str, int lengthA)
{
  resize(length + lengthA);
  memcpy(s + length, str, lengthA);
  length += lengthA;
  s[length] = '\0';
  return this;
}

XWString * XWString::appendf(char *fmt, ...)
{
  va_list argList;

  va_start(argList, fmt);
  appendfv(fmt, argList);
  va_end(argList);
  return this;
}

XWString * XWString::appendfv(char *fmt, va_list argList)
{
  GStringFormatArg *args;
  int argsLen, argsSize;
  GStringFormatArg arg;
  int idx, width, prec;
  bool reverseAlign, zeroFill;
  GStringFormatType ft;
  char buf[65];
  int len, i;
  const char *p0, *p1;
  char *str;

  argsLen = 0;
  argsSize = 8;
  args = (GStringFormatArg *)malloc(argsSize * sizeof(GStringFormatArg));

  p0 = fmt;
  while (*p0) {
    if (*p0 == '{') {
      ++p0;
      if (*p0 == '{') {
	++p0;
	append('{');
      } else {

	// parse the format string
	if (!(*p0 >= '0' && *p0 <= '9')) {
	  break;
	}
	idx = *p0 - '0';
	for (++p0; *p0 >= '0' && *p0 <= '9'; ++p0) {
	  idx = 10 * idx + (*p0 - '0');
	}
	if (*p0 != ':') {
	  break;
	}
	++p0;
	if (*p0 == '-') {
	  reverseAlign = true;
	  ++p0;
	} else {
	  reverseAlign = false;
	}
	width = 0;
	zeroFill = *p0 == '0';
	for (; *p0 >= '0' && *p0 <= '9'; ++p0) {
	  width = 10 * width + (*p0 - '0');
	}
	if (width < 0) {
	  width = 0;
	}
	if (*p0 == '.') {
	  ++p0;
	  prec = 0;
	  for (; *p0 >= '0' && *p0 <= '9'; ++p0) {
	    prec = 10 * prec + (*p0 - '0');
	  }
	} else {
	  prec = 0;
	}
	for (ft = (GStringFormatType)0;
	     formatStrings[ft];
	     ft = (GStringFormatType)(ft + 1)) {
	  if (!strncmp(p0, formatStrings[ft], strlen(formatStrings[ft]))) {
	    break;
	  }
	}
	if (!formatStrings[ft]) {
	  break;
	}
	p0 += strlen(formatStrings[ft]);
	if (*p0 != '}') {
	  break;
	}
	++p0;

	// fetch the argument
	if (idx > argsLen) {
	  break;
	}
	if (idx == argsLen) {
	  if (argsLen == argsSize) {
	    argsSize *= 2;
	    args = (GStringFormatArg *)realloc(args, argsSize * sizeof(GStringFormatArg));
	  }
	  switch (ft) {
	  case fmtIntDecimal:
	  case fmtIntHex:
	  case fmtIntOctal:
	  case fmtIntBinary:
	  case fmtSpace:
	    args[argsLen].i = va_arg(argList, int);
	    break;
	  case fmtUIntDecimal:
	  case fmtUIntHex:
	  case fmtUIntOctal:
	  case fmtUIntBinary:
	    args[argsLen].ui = va_arg(argList, uint);
	    break;
	  case fmtLongDecimal:
	  case fmtLongHex:
	  case fmtLongOctal:
	  case fmtLongBinary:
	    args[argsLen].l = va_arg(argList, long);
	    break;
	  case fmtULongDecimal:
	  case fmtULongHex:
	  case fmtULongOctal:
	  case fmtULongBinary:
	    args[argsLen].ul = va_arg(argList, ulong);
	    break;
	  case fmtDouble:
	  case fmtDoubleTrim:
	    args[argsLen].f = va_arg(argList, double);
	    break;
	  case fmtChar:
	    args[argsLen].c = (char)va_arg(argList, int);
	    break;
	  case fmtString:
	    args[argsLen].s = va_arg(argList, char *);
	    break;
	  case fmtGString:
	    args[argsLen].gs = va_arg(argList, XWString *);
	    break;
	  }
	  ++argsLen;
	}

	// format the argument
	arg = args[idx];
	switch (ft) {
	case fmtIntDecimal:
	  formatInt(arg.i, buf, sizeof(buf), zeroFill, width, 10, &str, &len);
	  break;
	case fmtIntHex:
	  formatInt(arg.i, buf, sizeof(buf), zeroFill, width, 16, &str, &len);
	  break;
	case fmtIntOctal:
	  formatInt(arg.i, buf, sizeof(buf), zeroFill, width, 8, &str, &len);
	  break;
	case fmtIntBinary:
	  formatInt(arg.i, buf, sizeof(buf), zeroFill, width, 2, &str, &len);
	  break;
	case fmtUIntDecimal:
	  formatUInt(arg.ui, buf, sizeof(buf), zeroFill, width, 10,
		     &str, &len);
	  break;
	case fmtUIntHex:
	  formatUInt(arg.ui, buf, sizeof(buf), zeroFill, width, 16,
		     &str, &len);
	  break;
	case fmtUIntOctal:
	  formatUInt(arg.ui, buf, sizeof(buf), zeroFill, width, 8, &str, &len);
	  break;
	case fmtUIntBinary:
	  formatUInt(arg.ui, buf, sizeof(buf), zeroFill, width, 2, &str, &len);
	  break;
	case fmtLongDecimal:
	  formatInt(arg.l, buf, sizeof(buf), zeroFill, width, 10, &str, &len);
	  break;
	case fmtLongHex:
	  formatInt(arg.l, buf, sizeof(buf), zeroFill, width, 16, &str, &len);
	  break;
	case fmtLongOctal:
	  formatInt(arg.l, buf, sizeof(buf), zeroFill, width, 8, &str, &len);
	  break;
	case fmtLongBinary:
	  formatInt(arg.l, buf, sizeof(buf), zeroFill, width, 2, &str, &len);
	  break;
	case fmtULongDecimal:
	  formatUInt(arg.ul, buf, sizeof(buf), zeroFill, width, 10,
		     &str, &len);
	  break;
	case fmtULongHex:
	  formatUInt(arg.ul, buf, sizeof(buf), zeroFill, width, 16,
		     &str, &len);
	  break;
	case fmtULongOctal:
	  formatUInt(arg.ul, buf, sizeof(buf), zeroFill, width, 8, &str, &len);
	  break;
	case fmtULongBinary:
	  formatUInt(arg.ul, buf, sizeof(buf), zeroFill, width, 2, &str, &len);
	  break;
	case fmtDouble:
	  formatDouble(arg.f, buf, sizeof(buf), prec, false, &str, &len);
	  break;
	case fmtDoubleTrim:
	  formatDouble(arg.f, buf, sizeof(buf), prec, true, &str, &len);
	  break;
	case fmtChar:
	  buf[0] = arg.c;
	  str = buf;
	  len = 1;
	  reverseAlign = !reverseAlign;
	  break;
	case fmtString:
	  str = arg.s;
	  len = (int)strlen(str);
	  reverseAlign = !reverseAlign;
	  break;
	case fmtGString:
	  str = arg.gs->getCString();
	  len = arg.gs->getLength();
	  reverseAlign = !reverseAlign;
	  break;
	case fmtSpace:
	  str = buf;
	  len = 0;
	  width = arg.i;
	  break;
	}

	// append the formatted arg, handling width and alignment
	if (!reverseAlign && len < width) {
	  for (i = len; i < width; ++i) {
	    append(' ');
	  }
	}
	append(str, len);
	if (reverseAlign && len < width) {
	  for (i = len; i < width; ++i) {
	    append(' ');
	  }
	}
      }

    } else if (*p0 == '}') {
      ++p0;
      if (*p0 == '}') {
	++p0;
      }
      append('}');
      
    } else {
      for (p1 = p0 + 1; *p1 && *p1 != '{' && *p1 != '}'; ++p1) ;
      append(p0, (int)(p1 - p0));
      p0 = p1;
    }
  }

  free(args);
  return this;
}
    
XWString * XWString::clear()
{
  s[length = 0] = '\0';
  resize(0);
  return this;
}

int XWString::cmp(XWString *str)
{
  int n1, n2, i, x;
  char *p1, *p2;

  n1 = length;
  n2 = str->length;
  for (i = 0, p1 = s, p2 = str->s; i < n1 && i < n2; ++i, ++p1, ++p2) {
    x = *p1 - *p2;
    if (x != 0) {
      return x;
    }
  }
  return n1 - n2;
}

int XWString::cmp(const char *sA)
{
  int n1, i, x;
  const char *p1, *p2;

  n1 = length;
  for (i = 0, p1 = s, p2 = sA; i < n1 && *p2; ++i, ++p1, ++p2) {
    x = *p1 - *p2;
    if (x != 0) {
      return x;
    }
  }
  if (i < n1) {
    return 1;
  }
  if (*p2) {
    return -1;
  }
  return 0;
}

int XWString::cmpN(XWString *str, int n)
{
  int n1, n2, i, x;
  char *p1, *p2;

  n1 = length;
  n2 = str->length;
  for (i = 0, p1 = s, p2 = str->s;
       i < n1 && i < n2 && i < n;
       ++i, ++p1, ++p2) {
    x = *p1 - *p2;
    if (x != 0) {
      return x;
    }
  }
  if (i == n) {
    return 0;
  }
  return n1 - n2;
}

int XWString::cmpN(const char *sA, int n)
{
  int n1, i, x;
  const char *p1, *p2;

  n1 = length;
  for (i = 0, p1 = s, p2 = sA; i < n1 && *p2 && i < n; ++i, ++p1, ++p2) {
    x = *p1 - *p2;
    if (x != 0) {
      return x;
    }
  }
  if (i == n) {
    return 0;
  }
  if (i < n1) {
    return 1;
  }
  if (*p2) {
    return -1;
  }
  return 0;
}
    
XWString * XWString::del(int i, int n)
{
  int j;

  if (i >= 0 && n > 0 && i + n > 0) {
    if (i + n > length) {
      n = length - i;
    }
    for (j = i; j <= length - n; ++j) {
      s[j] = s[j + n];
    }
    resize(length -= n);
  }
  return this;
}

XWString * XWString::format(char *fmt, ...)
{
  va_list argList;
  XWString *s;

  s = new XWString();
  va_start(argList, fmt);
  s->appendfv(fmt, argList);
  va_end(argList);
  return s;
}

XWString * XWString::formatv(char *fmt, va_list argList)
{
  XWString *s;

  s = new XWString();
  s->appendfv(fmt, argList);
  return s;
}

XWString * XWString::fromInt(int x)
{
  char buf[24]; // enough space for 64-bit ints plus a little extra
  char *p;
  int len;

  formatInt(x, buf, sizeof(buf), false, 0, 10, &p, &len);
  return new XWString(p, len);
}

XWString *XWString::insert(int i, char c)
{
  int j;

  resize(length + 1);
  for (j = length + 1; j > i; --j)
    s[j] = s[j-1];
  s[i] = c;
  ++length;
  return this;
}

XWString *XWString::insert(int i, XWString *str)
{
  int n = str->getLength();
  int j;

  resize(length + n);
  for (j = length; j >= i; --j)
    s[j+n] = s[j];
  memcpy(s+i, str->getCString(), n);
  length += n;
  return this;
}

XWString *XWString::insert(int i, const char *str)
{
  int n = (int)strlen(str);
  int j;

  resize(length + n);
  for (j = length; j >= i; --j)
    s[j+n] = s[j];
  memcpy(s+i, str, n);
  length += n;
  return this;
}

XWString *XWString::insert(int i, const char *str, int lengthA)
{
  int j;

  resize(length + lengthA);
  for (j = length; j >= i; --j)
    s[j+lengthA] = s[j];
  memcpy(s+i, str, lengthA);
  length += lengthA;
  return this;
}

XWString * XWString::lowerCase()
{
  int i;

  for (i = 0; i < length; ++i) {
    if (isupper(s[i]))
      s[i] = tolower(s[i]);
  }
  return this;
}

void XWString::setValue(char * sA, int len)
{
    if (s)
        delete [] s;
    
    length = len;
    s = sA;    
}

QString XWString::toQString()
{
	QString ret;	
	if (length <= 0)
		return ret;

	QByteArray ba(s, length);
	QTextStream stream(ba, QIODevice::ReadOnly);
	ret = stream.readAll();
	return ret;
}

XWString * XWString::upperCase()
{
  int i;

  for (i = 0; i < length; ++i) {
    if (islower(s[i]))
      s[i] = toupper(s[i]);
  }
  return this;
}

void XWString::formatDouble(double x, 
                               char *buf, 
                               int bufSize, 
                               int prec,
			                   bool trim, 
			                   char **p, 
			                   int *len)
{
  bool neg, started;
  double x2;
  int d, i, j;

  if ((neg = x < 0)) {
    x = -x;
  }
  x = floor(x * pow(10.0, prec) + 0.5);
  i = bufSize;
  started = !trim;
  for (j = 0; j < prec && i > 1; ++j) {
    x2 = floor(0.1 * (x + 0.5));
    d = (int)floor(x - 10 * x2 + 0.5);
    if (started || d != 0) {
      buf[--i] = '0' + d;
      started = true;
    }
    x = x2;
  }
  if (i > 1 && started) {
    buf[--i] = '.';
  }
  if (i > 1) {
    do {
      x2 = floor(0.1 * (x + 0.5));
      d = (int)floor(x - 10 * x2 + 0.5);
      buf[--i] = '0' + d;
      x = x2;
    } while (i > 1 && x);
  }
  if (neg) {
    buf[--i] = '-';
  }
  *p = buf + i;
  *len = bufSize - i;
}

void XWString::formatInt(long x, 
                            char *buf, 
                            int bufSize,
			                bool zeroFill, 
			                int width, 
			                int base,
			                char **p, 
			                int *len)
{
  static char vals[17] = "0123456789abcdef";
  bool neg;
  int start, i, j;

  i = bufSize;
  if ((neg = x < 0)) {
    x = -x;
  }
  start = neg ? 1 : 0;
  if (x == 0) {
    buf[--i] = '0';
  } else {
    while (i > start && x) {
      buf[--i] = vals[x % base];
      x /= base;
    }
  }
  if (zeroFill) {
    for (j = bufSize - i; i > start && j < width - start; ++j) {
      buf[--i] = '0';
    }
  }
  if (neg) {
    buf[--i] = '-';
  }
  *p = buf + i;
  *len = bufSize - i;
}

void XWString::formatUInt(unsigned long x, 
	                         char *buf, 
	                         int bufSize,
			                 bool zeroFill, 
			                 int width, 
			                 int base,
			                 char **p, 
			                 int *len)
{
   static char vals[17] = "0123456789abcdef";
  int i, j;

  i = bufSize;
  if (x == 0) {
    buf[--i] = '0';
  } else {
    while (i > 0 && x) {
      buf[--i] = vals[x % base];
      x /= base;
    }
  }
  if (zeroFill) {
    for (j = bufSize - i; i > 0 && j < width; ++j) {
      buf[--i] = '0';
    }
  }
  *p = buf + i;
  *len = bufSize - i;
}

inline bool XWString::resize(int length1)
{
  char *s1;

  if (!s) {
    s = new char[size(length1)];
  } else if (size(length1) != size(length)) {
    s1 = new char[size(length1)];
    if (length1 < length) {
      memcpy(s1, s, length1);
      s1[length1] = '\0';
    } else {
      memcpy(s1, s, length + 1);
    }
    delete[] s;
    s = s1;
  }
  
  return true;
}

inline int XWString::size(int len)
{
    int delta = 8;
    for (; delta < len && delta < 0x100000; delta <<= 1) ;
    return (len + delta) & ~(delta - 1);
}
