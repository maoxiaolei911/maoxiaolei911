/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWPFBPARSER_H
#define XWPFBPARSER_H

#include <QObject>

#define PFB_NAME_LEN_MAX   127
#define PFB_STRING_LEN_MAX 4096
#define PFB_MAX_DIGITS     10
#define PFB_TOKEN_LEN_MAX  PFB_STRING_LEN_MAX

#define PFB_TYPE_END       -2
#define PFB_TYPE_UNKNOWN   -1
#define PFB_TYPE_NULL       0
#define PFB_TYPE_BOOLEAN    1
#define PFB_TYPE_INTEGER    2
#define PFB_TYPE_REAL       3
#define PFB_TYPE_STRING     5
#define PFB_TYPE_NAME       6
#define PFB_TYPE_MARK       7


class XWLexer;

class XWPFBParser : public QObject
{
	Q_OBJECT
	
public:
	XWPFBParser(const char * start, 
	            long lenA, 
	            QObject * parent = 0);
	XWPFBParser(const char * start, 
	            const char * end, 
	            QObject * parent = 0);
	~XWPFBParser();
	
	bool getBoolean();
	
	//����PFB�ļ��еı��룬���ص�������delete [] ɾ����".notdef"�ÿ�ָ���ʾ
	int getEncoding(char **enc_vec);
	
		  long   getIV() {return type == PFB_TYPE_INTEGER ? lval : -1;}
		  long   getLen() {return len;}
	//��ȡPFB�ļ�����һ���ؼ���
	const char * getNextKey();	
		  bool   getOp(const char *op);
	const char * getString();
		  char * getSV() {return token;}
		  //��ȡPFB�ļ�����һ��token
	      int    getToken();
	      //��ȡPFB�ļ������max����ֵ
		  int    getValues(double *value, int max);
		  
		  //��ǰtoken�Ƿ�Ϊָ��������
		  bool matchName(const char * name);
		  //��ǰtoken�Ƿ�Ϊָ���Ĳ���
		  bool matchOp(const char *op);
	
	bool seekOperator(const char *op);
	//�ʷ���������lenA���ַ�
	const char * skip(long lenA);
	//��ǰλ��
	const char * tellPos(char ** endptrA);
	
private:
	uchar escToUC(uchar *valid);
	
	int   getXPair();
	
	uchar ostrToUC(uchar *valid);
	
	void parseAny();
	void parseASCII85();
	void parseBoolean();
	void parseHex();
	void parseLiteral();
	void parseName();
	void parseNumber();
	void parseNull();
	
	void parseString();
	            
private:
	XWLexer * lexer;
	int type;
	int len;
	char * token;
	union
	{
		double dval;
		long   lval;
	};
	
	const char * endptr;
};

#endif //XWPFBPARSER_H