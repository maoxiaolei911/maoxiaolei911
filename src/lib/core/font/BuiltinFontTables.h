/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef BUILTINFONTTABLES_H
#define BUILTINFONTTABLES_H

#include "BuiltinFont.h"

#define nBuiltinFonts      14
#define nBuiltinFontSubsts 12

extern BuiltinFont builtinFonts[nBuiltinFonts];
extern BuiltinFont *builtinFontSubst[nBuiltinFontSubsts];

extern void initBuiltinFontTables();
extern void freeBuiltinFontTables();

extern int isBuiltinFont(const char *name);

#endif
