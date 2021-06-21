/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef TEXENCODING_H
#define TEXENCODING_H

#include <QString>
#include "XWGlobal.h"

struct Encoding
{
  	char *name;
  	char *vec[256];
};

//释放Encoding，e有可能是Tex标准编码
XW_FONT_EXPORT void freeEncoding(Encoding * e);

XW_FONT_EXPORT extern char ** getBaseEnc(const QString & encname);

//Tex标准编码
XW_FONT_EXPORT extern Encoding * getStaticEncoding();

//读取enc文件，encname可以是标准编码名，返回的名字和向量用delete [] 释放
//null用".notdef"表示
XW_FONT_EXPORT extern bool readTexEnc(const QString & encname, char ** name, char**vec);

#endif //TEXENCODING_H
