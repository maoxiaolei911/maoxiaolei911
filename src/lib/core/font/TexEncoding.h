/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
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

//�ͷ�Encoding��e�п�����Tex��׼����
XW_FONT_EXPORT void freeEncoding(Encoding * e);

XW_FONT_EXPORT extern char ** getBaseEnc(const QString & encname);

//Tex��׼����
XW_FONT_EXPORT extern Encoding * getStaticEncoding();

//��ȡenc�ļ���encname�����Ǳ�׼�����������ص����ֺ�������delete [] �ͷ�
//null��".notdef"��ʾ
XW_FONT_EXPORT extern bool readTexEnc(const QString & encname, char ** name, char**vec);

#endif //TEXENCODING_H
