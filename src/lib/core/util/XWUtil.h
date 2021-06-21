/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
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
//从文件中读取一行，以'\\'结尾的行将被连接为一个单行，返回的指针应以free释放
XW_UTIL_EXPORT char * getLine(QIODevice * file);
XW_UTIL_EXPORT bool   getLine(char **bufferp, QIODevice *file);
	
//读取一个字节
XW_UTIL_EXPORT char  getSignedByte(QIODevice * file);
XW_UTIL_EXPORT uchar getUnsignedByte(QIODevice * file);

//读取一个16位整数
XW_UTIL_EXPORT short  getSignedPair(QIODevice * file);
XW_UTIL_EXPORT ushort getUnsignedPair(QIODevice * file);

//读取一个24位整数
XW_UTIL_EXPORT long   getSignedTriple(QIODevice * file);
XW_UTIL_EXPORT ulong  getUnsignedTriple(QIODevice * file);

//读取一个32位整数
XW_UTIL_EXPORT long   getSignedQuad(QIODevice * file);
XW_UTIL_EXPORT ulong  getUnsignedQuad(QIODevice * file);
 
//想文件中写入一个串
XW_UTIL_EXPORT void puts(QFile * f, const QString & s, QTextCodec * codec = 0);

//想文件中写入一个字符
inline void putc(QFile * f, QChar c, QTextCodec * codec = 0)
{
    QString s(c);
    puts(f, s, codec);
}

/****************************************************************************
 env
 ****************************************************************************/
//取环境变量的值，如果不存在，返回缺省值
XW_UTIL_EXPORT QString envVar(const QString & envvar, const QString & defaut);

//取环境变量的值，如果不存在，返回空串
XW_UTIL_EXPORT QString  getEnv(const QString & envvar);

//获得相对路径sdir的绝对路径，pdir为参照路径。如sdir为'../../media'，pdir
//为'c:/doc/latex/help'，则结果为'c:/media'
XW_UTIL_EXPORT QString getAbsoluteDir(const QString & pdir, const QString & sdir);
//获得绝对路径sdir的相对路径，pdir为参照路径。如sdir为'c:/doc/latex/help/dvi'，pdir
//为'c:/doc/latex/help'，则结果为'./dvi'
XW_UTIL_EXPORT QString getRelativeDir(const QString & pdir, const QString & sdir);

//如果文件路径中包含空格，就将路径用双引号括起来
XW_UTIL_EXPORT QString normalizeQuotes(const char * name);
XW_UTIL_EXPORT QString normalizeQuotes(const QString & name);

//设置环境变量
XW_UTIL_EXPORT void putEnv(const QString & varname, int value);
XW_UTIL_EXPORT void putEnv(const QString & varname, const QString & value);

//系统是否为中、日、韩语言
XW_UTIL_EXPORT bool isCJK();

#endif // XWUTIL_H
