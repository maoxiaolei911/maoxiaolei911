/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWLEXER_H
#define XWLEXER_H

#include <QObject>

#include "XWGlobal.h"

#define LEXER_ERR_NON 0
#define LEXER_ERR_LEN 1
#define LEXER_ERR_

class XW_UTIL_EXPORT XWLexer : public QObject
{
	Q_OBJECT
	
public:
	//词法分析器
	XWLexer(const char * str, 
	        QObject * parent = 0);
	XWLexer(const char * str, 
	        int len, 
	        QObject * parent = 0);
	XWLexer(const char * start, 
	        const char * end, 
	        QObject * parent = 0);
	        
	//是否读到数组尾部
	bool atEnd(int offset) {return (curPtr + offset) >= endPtr;}
	
	//读取一个ASCII85字符串，返回当前位置
	const char * getASCII85(char * buf, 
	                        int buflen, 
	                        int * slen,
	                        bool * ok = 0);
	//读取一个PDF布尔值，返回当前位置
	const char * getBool(bool * v, bool * ok = 0);
	//读取一个字符，字符可以以'#'开始，后面接两个十六进制数字
	int    getChar();
	//读取一个c语言标识符，返回的指针应以delete [] 删除
	char * getCIdent();
	//读取一个c语言字符串(被'"'括起来)，其中的转义字符将再次被转义，
	//返回的指针应以delete [] 删除
	char * getCString();
	//当前位置
	const char * getCurPtr() {return curPtr;}
	//尾部
	const char * getEndPtr() {return endPtr;}
	//读取一个十进制浮点数，返回的指针应以delete [] 删除
	char * getFloatDecimal();
	//读取一个16进制PDF字符串，返回当前位置
	const char * getHexString(char * buf, 
	                          int buflen, 
	                          int * slen,
	                          bool * ok = 0);
	//读取由ascii码'!'至'~'(不含括号)组成的标识符，返回的指针应以delete [] 删除
	char * getIdent(const char * end = 0);
	//读取长度值(pt,in,cm,mm,bp)，如果长度单位以"true"开始，则放大mag倍
	bool   getLength(double *vp, double mag = 0.0);
	//读取一个PDF字符串，返回当前位置
	const char * getLiteralString(char * buf, 
	                              int buflen, 
	                              int * slen,
	                              bool * ok = 0);
	//读取一个以"/"开头的PDF名字，返回当前位置
	const char * getName(char * buf, int buflen, bool * ok = 0);
	//读取一个null，返回当前位置
	const char * getNull(bool * ok = 0);
	//读取一个PS实数，返回当前位置
	const char * getNumber(double * v, bool * isreal, bool * ok);
	//读取一个PDF实数，返回当前位置	
	const char * getNumber(double * v, bool * ok);
	//读取一个由正负号、数字和小数点组成的数，返回的指针应以delete [] 删除
	char * getNumber();
	//读取一个以'@'开头由ascii码'!'至'~'(不含括号)组成的标识符，返回的指针应以delete [] 删除
	char * getOptIdent();
	//读取一个PS转义字符
	int    getPSEscChar();	
	//读取一个PDF字符串，返回当前位置
	const char * getString(char * buf, 
	                       int buflen, 
	                       int * slen,
	                       bool * ok = 0);
	//读取一个无符号数，返回的指针应以delete [] 删除
	char * getUnsigned();
	//读取由ascii码'!'至'~'(不含括号和'=')组成的标识符，返回的指针应以delete [] 删除
	char * getValIdent(const char * end = 0);
	
	//空格字符
	static bool isBlank(int c) {return (c == ' ' || c == '\t');}
	//合法的C语言标识符的起始字符
	static bool isCNonDigits(int c) 
	{
		return (c == '_' || 
	           (c >= 'a' && c <= 'z') || 
	           (c >= 'A' && c <= 'Z'));
	}
	
	//合法的C语言标识符的组成字符
	static bool isCIdentChar(int c) 
	{
		return (isCNonDigits(c) || 
	            (c >= '0' && c <= '9'));
	}
	
	static bool isDelim(int c)
	{
		return (c == '(' || 
		        c == '/' || 
		        c == '<' || 
		        c == '>' || 
		        c == '[' || 
		        c == ']' ||
		        c == '{' || 
		        c == '}' || 
		        c == '%');
	}
	
	static bool isSpace(int c) 
	{
	  	return (c == ' ' || 
	  	        c == '\t' || 
	  	        c == '\f' ||
	  	        c == '\r' || 
	  	        c == '\n' || 
	  	        c == '\0');
	}
	
	static bool isTokenSep(int c) {return (isSpace((c)) || isDelim(c)); }
	
	void   resetState() {tainted = 0;}
	void   restore() {curPtr = savePtr;}
	
	void   save() {savePtr = curPtr;}
	void   setCurPtr(const char * curptrA) {curPtr = curptrA;}
	void   setState() {tainted = 1;}
	const  char * skip(int offset) {curPtr += offset; return curPtr;}
	const char * skip(const char * p);
	const char * skipBlank();
	const char * skipLine();
	//跳过空格和以'%'开始的注释行
	const char * skipWhite();
	
	bool   tokenEnd() {return (curPtr >= endPtr || 
		                         isSpace(*curPtr) || 
		        				 isDelim(*curPtr));}
	static bool tokenEnd(const char * p, const char * e) 
	{
		return (p >= e || 
		        isSpace(*p) || 
		        isDelim(*p));
	}
	
private:
	char * parseGenIdent(const char * end, const char *valid_chars);
	char * parsedString(const char *start, const char *end);
	
	int  readCEscChar(char *r, const char **pp);
	int  readCLitStrC(char *q, int len, const char **pp);
	
private:
	const char * startPtr;
	const char * curPtr;
	const char * endPtr;
	const char * savePtr;
	
	int tainted;
};

#endif // XWLEXER_H
