/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
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
	//�մ�
    XWString();
    //��sA����һ����
    XWString(const char *sA);
    //��sa��ǰlen���ַ�����һ����
    XWString(char * sA, int len);
    //��str�ĵ�idx���ַ���ʼ��ȡlengthA���ַ�����һ����
    XWString(XWString *str, int idx, int lengthA);
    //��str����һ����
    XWString(XWString *str);
    //��str1��str2������������һ����
    XWString(XWString *str1, XWString *str2);
    ~XWString();
    
    //׷���ַ����ַ���
    XWString *append(char c);
    XWString *append(XWString *str);
    XWString *append(const char *str);
    XWString *append(const char *str, int lengthA);
    XWString *appendf(char *fmt, ...);
    XWString *appendfv(char *fmt, va_list argList);
  
    //���
    XWString *clear();
    
    //�Ƚ�
    int cmp(XWString *str);    
    int cmp(const char *sA);
    int cmpN(XWString *str, int n);
    int cmpN(const char *sA, int n);
    
    //����
    XWString * copy() { return new XWString(this); }
    
    //�Ե�i����ɾ��n���ַ�
    XWString *del(int i, int n = 1);
    
    //����һ����ʽ���Ĵ�����ʽ�﷨���£�
    // {<arg>:[<width>][.<precision>]<type>}
    //<arg>�ǲ�������ţ���һ������Ϊ0
    //<width>����������Ҷ��룬��������룬���㲿����'0'���
    //<precision>��ʾС�����Ҳ������λ��
    //<type> ����������֮һ:
    //d, x, o, b -- ʮ���ơ�ʮ�����ơ��˽��ơ�����������
    //ud, ux, uo, ub -- �޷���ʮ���ơ�ʮ�����ơ��˽��ơ�����������
    //ld, lx, lo, lb, uld, ulx, ulo, ulb -- ������
    //f, g -- ʵ��
    //c -- �ַ�
    //s -- �ַ���
    //t -- XWString
    //w -- �ո�
    static XWString *format(char *fmt, ...);
    static XWString *formatv(char *fmt, va_list argList);
    
    static XWString *fromInt(int x);
    
    //ȡ��i���ַ�
    char   getChar(int i) { return s[i]; }    
    //��'\0'��β���ַ���
    char * getCString() { return s; }
    //�����ַ���
    int    getLength() { return length; }    
    
    //����
    XWString *insert(int i, char c);
    XWString *insert(int i, XWString *str);
    XWString *insert(int i, const char *str);
    XWString *insert(int i, const char *str, int lengthA);
    
    //תΪСд
    XWString *lowerCase();
    
    //�޸�
    void setChar(int i, char c) { s[i] = c; }
    void setValue(char * sA, int len);
    
    QString toQString();
    
    //תΪ��д
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

