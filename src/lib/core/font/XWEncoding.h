/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWENCODING_H
#define XWENCODING_H


#include <QObject>
#include <QByteArray>
#include <QString>

class XWDifference
{
public:
	XWDifference(long codeA);
	~XWDifference();
	
	void add(char * encA);
	
public:
	long code;
	long count;
	char ** encs;
};

class XWEncoding
{
public:
	friend class XWEncodingCache;
	
	XWEncoding();
	~XWEncoding();
	
	void addUsedChars(const char *is_usedA);
	
	XWEncoding * getBaseEnc() {return baseenc;}
	int    getDCount() {return dcount;}
	XWDifference * getDifference(int i);
	char ** getEncoding() {return glyphs;}
	int     getFlags() {return flags;}
	char  * getIdent() {return ident;}
	char  * getName() {return enc_name;}
	char  * getResource() {return resource;}
	char  * getIsUsed() {return is_used;}
	
	bool isPredefined();
	
	void makeDifferences(int pdf_ver);
	
	void usedByType3();
	
private:
	char     * ident;
  	char     * enc_name;
  	int        flags;
  	XWEncoding * baseenc;
  	
  	char * resource;
  	int    dcount;
  	XWDifference ** differences;
  	
  	char     * glyphs[256];
  	char       is_used[256];
};

class XWEncodingCache : public QObject
{
	Q_OBJECT
    
public:
	XWEncodingCache(QObject * parent = 0);
	~XWEncodingCache();
	
	void addUsedChars(int encoding_id, const char *is_used);
	
	int findResource(char *enc_name);
	
	XWEncoding     * get(int enc_id);
	XWDifference **  getDifferences(int enc_id,
	                                int pdf_ver, 
	                                int * dcountA);
	char ** getEncoding(int enc_id);
	char *  getName(int enc_id);
	char *  getResource(int enc_id);
	
	int isPredefined(int enc_id);
	
	void usedByType3(int enc_id);
	
private:
	int newEncoding(const char *ident,
			        char *baseenc_name, 
			        int flags);
			        
private:
	int  count;
  	int  capacity;
  	
  	XWEncoding ** encodings;
};

#endif // XWENCODING_H
