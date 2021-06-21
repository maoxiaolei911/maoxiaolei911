/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef TEXTYPE_H
#define TEXTYPE_H

enum TeXCategoryCode
{
	Escape = 0,
	BeginOfGroup = 1,
	EndingOfGroup = 2,
	MathShift = 3,
	AlignmentTab = 4,
	EndOfLine = 5,
	Parameter = 6,
	SuperScript = 7,
	SubScript = 8,
	IgnoreChar = 9,
	Spacer = 10,
	Letter = 11,
	OtherChar = 12,
	ActiveChar = 13,
	CommentChar = 14,
	InvalidChar = 15,
	EndFile
};
	
enum TeXFileMode
{
	NoMode = 0,
	OneByteMode,
	EBCDICMode,
	TwoByteMode,
	TwoByteLEMode,
	UTF8,
	UTF16,
	UTF32,
	UTF32BE,
	UTF32LE,
	GB18030,	
	GBK,
	GB,
	BIG5,
	EUCJP,
	EUCKR,
	HKSCS,
	KSLATEX,
	SJIS
};
	
#endif //TEXTYPE_H


