/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWUTIL_H
#define XWUTIL_H

#include <QString>

#include "XWGlobal.h"

class QIODevice;
class QFile;
class QTextCodec;

/****************************************************************************
 file
 ****************************************************************************/
//���ļ��ж�ȡһ�У���'\\'��β���н�������Ϊһ�����У����ص�ָ��Ӧ��free�ͷ�
XW_UTIL_EXPORT char * getLine(QIODevice * file);
XW_UTIL_EXPORT bool   getLine(char **bufferp, QIODevice *file);
	
//��ȡһ���ֽ�
XW_UTIL_EXPORT char  getSignedByte(QIODevice * file);
XW_UTIL_EXPORT uchar getUnsignedByte(QIODevice * file);

//��ȡһ��16λ����
XW_UTIL_EXPORT short  getSignedPair(QIODevice * file);
XW_UTIL_EXPORT ushort getUnsignedPair(QIODevice * file);

//��ȡһ��24λ����
XW_UTIL_EXPORT long   getSignedTriple(QIODevice * file);
XW_UTIL_EXPORT ulong  getUnsignedTriple(QIODevice * file);

//��ȡһ��32λ����
XW_UTIL_EXPORT long   getSignedQuad(QIODevice * file);
XW_UTIL_EXPORT ulong  getUnsignedQuad(QIODevice * file);
 
//���ļ���д��һ����
XW_UTIL_EXPORT void puts(QFile * f, const QString & s, QTextCodec * codec = 0);

//���ļ���д��һ���ַ�
inline void putc(QFile * f, QChar c, QTextCodec * codec = 0)
{
    QString s(c);
    puts(f, s, codec);
}

/****************************************************************************
 env
 ****************************************************************************/
//ȡ����������ֵ����������ڣ�����ȱʡֵ
XW_UTIL_EXPORT QString envVar(const QString & envvar, const QString & defaut);

//ȡ����������ֵ����������ڣ����ؿմ�
XW_UTIL_EXPORT QString  getEnv(const QString & envvar);

//������·��sdir�ľ���·����pdirΪ����·������sdirΪ'../../media'��pdir
//Ϊ'c:/doc/latex/help'������Ϊ'c:/media'
XW_UTIL_EXPORT QString getAbsoluteDir(const QString & pdir, const QString & sdir);
//��þ���·��sdir�����·����pdirΪ����·������sdirΪ'c:/doc/latex/help/dvi'��pdir
//Ϊ'c:/doc/latex/help'������Ϊ'./dvi'
XW_UTIL_EXPORT QString getRelativeDir(const QString & pdir, const QString & sdir);

//����ļ�·���а����ո񣬾ͽ�·����˫����������
XW_UTIL_EXPORT QString normalizeQuotes(const char * name);
XW_UTIL_EXPORT QString normalizeQuotes(const QString & name);

//���û�������
XW_UTIL_EXPORT void putEnv(const QString & varname, int value);
XW_UTIL_EXPORT void putEnv(const QString & varname, const QString & value);

//ϵͳ�Ƿ�Ϊ�С��ա�������
XW_UTIL_EXPORT bool isCJK();

#endif // XWUTIL_H
