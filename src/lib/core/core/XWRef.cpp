/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include "XWObject.h"
#include "XWStream.h"
#include "XWPDFLexer.h"
#include "XWPDFParser.h"
#include "XWDict.h"
#include "XWRef.h"

XWRef::XWRef()
{
}

uint XWRef::strToUnsigned(char *s)
{
  uint x, d;
  char *p;

  x = 0;
  for (p = s; *p && isdigit(*p & 0xff); ++p) {
    d = *p - '0';
    if (x > (UINT_MAX - d) / 10) {
      break;
    }
    x = 10 * x + d;
  }
  return x;
}

