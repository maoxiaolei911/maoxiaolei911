/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWSTRING_H
#define XWSTRING_H

#include <stdarg.h>
#include <string.h>
#include <QString>
#include <QByteArray>

#include "XWGlobal.h"

class XW_UTIL_EXPORT XWString
{
public:
	//空串
    XWString();
    //由sA构造一个串
    XWString(const char *sA);
    //由sa的前len个字符构造一个串
    XWString(char * sA, int len);
    //从str的第idx个字符开始，取lengthA个字符构造一个串
    XWString(XWString *str, int idx, int lengthA);
    //由str构造一个串
    XWString(XWString *str);
    //将str1和str2连接起来构造一个串
    XWString(XWString *str1, XWString *str2);
    ~XWString();
    
    //追加字符或字符串
    XWString *append(char c);
    XWString *append(XWString *str);
    XWString *append(const char *str);
    XWString *append(const char *str, int lengthA);
    XWString *appendf(char *fmt, ...);
    XWString *appendfv(char *fmt, va_list argList);
  
    //清空
    XWString *clear();
    
    //比较
    int cmp(XWString *str);    
    int cmp(const char *sA);
    int cmpN(XWString *str, int n);
    int cmpN(const char *sA, int n);
    
    //复制
    XWString * copy() { return new XWString(this); }
    
    //自第i个起删除n个字符
    XWString *del(int i, int n = 1);
    
    //创建一个格式化的串。格式语法如下：
    // {<arg>:[<width>][.<precision>]<type>}
    //<arg>是参数的序号，第一个参数为0
    //<width>是域宽，正数右对齐，负数左对齐，不足部分用'0'填充
    //<precision>表示小数点右侧的数字位数
    //<type> 是下列情形之一:
    //d, x, o, b -- 十进制、十六进制、八进制、二进制整数
    //ud, ux, uo, ub -- 无符号十进制、十六进制、八进制、二进制整数
    //ld, lx, lo, lb, uld, ulx, ulo, ulb -- 长整数
    //f, g -- 实数
    //c -- 字符
    //s -- 字符串
    //t -- XWString
    //w -- 空格
    static XWString *format(char *fmt, ...);
    static XWString *formatv(char *fmt, va_list argList);
    
    static XWString *fromInt(int x);
    
    //取第i个字符
    char   getChar(int i) { return s[i]; }    
    //以'\0'结尾的字符串
    char * getCString() { return s; }
    //串中字符数
    int    getLength() { return length; }    
    
    //插入
    XWString *insert(int i, char c);
    XWString *insert(int i, XWString *str);
    XWString *insert(int i, const char *str);
    XWString *insert(int i, const char *str, int lengthA);
    
    //转为小写
    XWString *lowerCase();
    
    //修改
    void setChar(int i, char c) { s[i] = c; }
    void setValue(char * sA, int len);
    
    QString toQString();
    
    //转为大写
    XWString *upperCase();
    
private:
    static void formatDouble(double x, 
                             char *buf, 
                             int bufSize, 
                             int prec,
			                 bool trim, 
			                 char **p, 
			                 int *len);
    static void formatInt(long x, 
                          char *buf, 
                          int bufSize,
			              bool zeroFill, 
			              int width, 
			              int base,
			              char **p, 
			              int *len);
	static void formatUInt(unsigned long x, 
	                       char *buf, 
	                       int bufSize,
			               bool zeroFill, 
			               int width, 
			               int base,
			               char **p, 
			               int *len);
			
    bool resize(int length1);
    
    int size(int len);
    
private:
    int    length;
    char * s;
};


#endif //XWSTRING_H

