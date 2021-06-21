/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
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
	
	//分析PFB文件中的编码，返回的向量以delete [] 删除，".notdef"用空指针表示
	int getEncoding(char **enc_vec);
	
		  long   getIV() {return type == PFB_TYPE_INTEGER ? lval : -1;}
		  long   getLen() {return len;}
	//读取PFB文件中下一个关键字
	const char * getNextKey();	
		  bool   getOp(const char *op);
	const char * getString();
		  char * getSV() {return token;}
		  //读取PFB文件中下一个token
	      int    getToken();
	      //读取PFB文件中最多max个数值
		  int    getValues(double *value, int max);
		  
		  //当前token是否为指定的名字
		  bool matchName(const char * name);
		  //当前token是否为指定的操作
		  bool matchOp(const char *op);
	
	bool seekOperator(const char *op);
	//词法解析跳过lenA个字符
	const char * skip(long lenA);
	//当前位置
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