/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef UNICODETYPETABLE_H
#define UNICODETYPETABLE_H

#include <QtGlobal>


extern bool unicodeTypeL(uint c);

extern bool unicodeTypeR(uint c);

extern bool unicodeTypeNum(uint c);

extern bool unicodeTypeAlphaNum(uint c);

extern uint unicodeToUpper(uint c);


#endif
