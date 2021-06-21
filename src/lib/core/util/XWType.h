/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ** ��������С����������һ���֡������Ϊ��ҵ�����δ�����Ĺ�˾��ɣ��κ�
 ** ��λ�͸��˲��ø��ơ�����Դ�����Լ��������ļ���
 **
 ****************************************************************************/
#ifndef XWTYPE_H
#define XWTYPE_H

#include <QtGlobal>

//byte
typedef char     XWSmallNumber;
typedef char     XWTwoChoices;
typedef char     XWFourChoices;
typedef uchar    XWRealEightBits;
typedef char      XWGroupCode;

//two byte
//typedef ushort  XWASCIICode;
//typedef ushort  XWPackedASCIICode;
//typedef ushort  XWEightBits;
//typedef ushort  XWQuarterWord;
//typedef ushort  XWHyphPointer;
//typedef ushort  XWInternalOcpNumber;
//typedef ushort  XWInternalOcpListNumber;

//four byte
typedef uint    XWCharCode;
typedef XWCharCode XWASCIICode;
typedef XWCharCode XWPackedASCIICode;
typedef XWCharCode XWEightBits;
typedef XWCharCode XWQuarterWord;
typedef XWCharCode XWHyphPointer;
typedef XWCharCode XWInternalOcpNumber;
typedef XWCharCode XWInternalOcpListNumber;


typedef int      XWInteger;
typedef XWInteger  XWScaled;
typedef XWInteger  XWHalfWord;
typedef XWHalfWord XWPointer;
typedef XWInteger  XWPoolPointer;
typedef XWInteger  XWStrNumber;
typedef XWInteger  XWSavePointer;
typedef XWInteger  XWOcpListIndex;
typedef XWInteger  XWOcpLStackIndex;
typedef XWInteger  XWActiveIndex;
typedef XWInteger  XWFontIndex;
typedef XWInteger  XWInternalFontNumber;
typedef XWInteger  XWTriePointer;

typedef int XWCintType;

//glue ratio
typedef double    XWGlueRatio;


enum EndLineFormat
{
    DosEndLine = 0,
    UnixEndLine = 1
};

enum MatchFlag
{
    Exactly = 0x0000,
    CaseInsensitively = 0x0001,
    WholeWords = 0x0002
};

#endif // XWTYPE_H

