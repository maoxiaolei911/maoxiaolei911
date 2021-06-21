/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
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
	//�ʷ�������
	XWLexer(const char * str, 
	        QObject * parent = 0);
	XWLexer(const char * str, 
	        int len, 
	        QObject * parent = 0);
	XWLexer(const char * start, 
	        const char * end, 
	        QObject * parent = 0);
	        
	//�Ƿ��������β��
	bool atEnd(int offset) {return (curPtr + offset) >= endPtr;}
	
	//��ȡһ��ASCII85�ַ��������ص�ǰλ��
	const char * getASCII85(char * buf, 
	                        int buflen, 
	                        int * slen,
	                        bool * ok = 0);
	//��ȡһ��PDF����ֵ�����ص�ǰλ��
	const char * getBool(bool * v, bool * ok = 0);
	//��ȡһ���ַ����ַ�������'#'��ʼ�����������ʮ����������
	int    getChar();
	//��ȡһ��c���Ա�ʶ�������ص�ָ��Ӧ��delete [] ɾ��
	char * getCIdent();
	//��ȡһ��c�����ַ���(��'"'������)�����е�ת���ַ����ٴα�ת�壬
	//���ص�ָ��Ӧ��delete [] ɾ��
	char * getCString();
	//��ǰλ��
	const char * getCurPtr() {return curPtr;}
	//β��
	const char * getEndPtr() {return endPtr;}
	//��ȡһ��ʮ���Ƹ����������ص�ָ��Ӧ��delete [] ɾ��
	char * getFloatDecimal();
	//��ȡһ��16����PDF�ַ��������ص�ǰλ��
	const char * getHexString(char * buf, 
	                          int buflen, 
	                          int * slen,
	                          bool * ok = 0);
	//��ȡ��ascii��'!'��'~'(��������)��ɵı�ʶ�������ص�ָ��Ӧ��delete [] ɾ��
	char * getIdent(const char * end = 0);
	//��ȡ����ֵ(pt,in,cm,mm,bp)��������ȵ�λ��"true"��ʼ����Ŵ�mag��
	bool   getLength(double *vp, double mag = 0.0);
	//��ȡһ��PDF�ַ��������ص�ǰλ��
	const char * getLiteralString(char * buf, 
	                              int buflen, 
	                              int * slen,
	                              bool * ok = 0);
	//��ȡһ����"/"��ͷ��PDF���֣����ص�ǰλ��
	const char * getName(char * buf, int buflen, bool * ok = 0);
	//��ȡһ��null�����ص�ǰλ��
	const char * getNull(bool * ok = 0);
	//��ȡһ��PSʵ�������ص�ǰλ��
	const char * getNumber(double * v, bool * isreal, bool * ok);
	//��ȡһ��PDFʵ�������ص�ǰλ��	
	const char * getNumber(double * v, bool * ok);
	//��ȡһ���������š����ֺ�С������ɵ��������ص�ָ��Ӧ��delete [] ɾ��
	char * getNumber();
	//��ȡһ����'@'��ͷ��ascii��'!'��'~'(��������)��ɵı�ʶ�������ص�ָ��Ӧ��delete [] ɾ��
	char * getOptIdent();
	//��ȡһ��PSת���ַ�
	int    getPSEscChar();	
	//��ȡһ��PDF�ַ��������ص�ǰλ��
	const char * getString(char * buf, 
	                       int buflen, 
	                       int * slen,
	                       bool * ok = 0);
	//��ȡһ���޷����������ص�ָ��Ӧ��delete [] ɾ��
	char * getUnsigned();
	//��ȡ��ascii��'!'��'~'(�������ź�'=')��ɵı�ʶ�������ص�ָ��Ӧ��delete [] ɾ��
	char * getValIdent(const char * end = 0);
	
	//�ո��ַ�
	static bool isBlank(int c) {return (c == ' ' || c == '\t');}
	//�Ϸ���C���Ա�ʶ������ʼ�ַ�
	static bool isCNonDigits(int c) 
	{
		return (c == '_' || 
	           (c >= 'a' && c <= 'z') || 
	           (c >= 'A' && c <= 'Z'));
	}
	
	//�Ϸ���C���Ա�ʶ��������ַ�
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
	//�����ո����'%'��ʼ��ע����
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
