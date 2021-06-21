/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWDVIENCODING_H
#define XWDVIENCODING_H

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QList>
#include <QHash>

#include "XWObject.h"

class XWDVIRef;
class XWDVICore;

class XWDVIEncoding
{
public:
	XWDVIEncoding(int enc_idA);
	~XWDVIEncoding();
	
	void addUsedChars(const char *is_used);
	
	void createResource(XWDVIRef * xref);
	void createToUnicodeCMap(XWDVICore * core, XWDVIRef * xref);
	
	static XWObject * createToUnicodeCMap(XWDVICore * core,
	                                      XWDVIRef * xref,
	                                      const char *enc_nameA,
                                          char **enc_vecA, 
                                          const char *is_usedA,
                                          XWObject * obj);
	
	void flush(XWDVIRef * xref, bool finished = true);
	
	char    ** getEncoding();
	int        getID() {return enc_id;}
	char     * getName();
	XWObject * getObj() {return &resource;}
	XWObject * getTounicode() {return &tounicode;}
	
	bool isPredefined();
	
	void usedByType3();
	
public:
	int enc_id;	
	XWDVIEncoding * baseenc;	
  	XWObject tounicode;
  	XWObject resource;
};

class XWDVIEncodingCache
{
public:
	XWDVIEncodingCache();
	~XWDVIEncodingCache();
	
	void addUsedChars(int enc_id, const char *is_used, int page_no);
	
	void close(XWDVIRef * xref, bool finished = true);
	void complete(XWDVICore * core, XWDVIRef * xref, bool finished = true);
	void completePage(XWDVICore * core, XWDVIRef * xref, long page_no);
	
	int findResource(char *enc_name);
	
	XWDVIEncoding * get(int enc_id);
	char ** getEncoding(int enc_id);	
	XWObject *  getEncodingObj(int enc_id);
	char *  getName(int enc_id);
	
	XWObject *  getTounicode(int enc_id);
	
	int isPredefined(int enc_id);
	
	void load(XWDVIRef * xref, long page_no);
	
	void usedByType3(int enc_id);
	
private:
	int  count;
  	int  capacity;
  	
  	XWDVIEncoding ** encodings;
  	
  	struct PageEncodings
  	{
  		QList<int> ids;
  	};
  	
  	QHash<long, PageEncodings*> pageEncodings;
};

#endif // XWDVIENCODING_H
