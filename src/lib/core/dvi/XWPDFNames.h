/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPDFNAMES_H
#define XWPDFNAMES_H

#include "XWObject.h"

#define NAME_TABLE_SIZE 503

class XWPDFNameEntry;
class XWDVIRef;
struct named_object;

class XWPDFNames
{
public:	
	struct Iterator 
	{
  		int    index;
  		XWPDFNameEntry  * curr;
	};

	XWPDFNames();
	~XWPDFNames();
	
	int  addObject(const void *key, int keylen, XWObject *object);
	void appendTable(const void *key, int keylen, XWObject *value);
	
	void close(XWDVIRef * xref);
	void clearIterator();
	int  closeObject(const void *key, int keylen);
	XWObject * createTree(XWDVIRef * xref,
	                      long *countA, 
	                      XWPDFNames * filter,
	                      XWObject * result);
	
	char     * getKey(int *keylen);
	XWObject * getVal();
	
	void insertTable(const void *key, int keylen, XWObject *value);
	
	XWObject * lookupObject(const void *key, 
	                        int keylen);
	                        
	XWObject * lookupReference(XWDVIRef * xref,
	                           const void *key, 
	                           int keylen,
	                           XWObject * ref);
	XWObject * lookupTable(const void *key, int keylen);
	                           
	bool next();
	
	bool setIterator();
	
	int tableSize() {return count;}
	
private:
	named_object * flatTable(long *num_entries, XWPDFNames * filter);
	
	uint getHash(const void *key, int keylen);
	
	XWPDFNameEntry * lookup(const void *key, int keylen);
	

private:
	long   count;
	XWPDFNameEntry ** table;
	Iterator * itor;
};


#endif //XWPDFNAMES_H
