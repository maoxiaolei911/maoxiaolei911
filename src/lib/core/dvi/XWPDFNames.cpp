/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <QByteArray>
#include "XWDVIRef.h"
#include "XWPDFNames.h"

class XWPDFNameEntry
{
public:
	XWPDFNameEntry(char * keyA, int keylenA, XWObject * obj);
	~XWPDFNameEntry();
	
	void close() {closed = 1;}
	void close(XWDVIRef * xref);
	
	XWObject * getObject() {return &object;}
	XWObject * getReference(XWDVIRef * xref, XWObject * ref);
	
	bool isIndirect() {return object.isIndirect();}
	
	void setObject(XWObject * obj);
	
public:
	char  *key;
  	int    keylen;
  	
  	XWObject reference;
  	XWObject object;
  	
  	XWPDFNameEntry * next;
  	int closed;
};

XWPDFNameEntry::XWPDFNameEntry(char * keyA, int keylenA, XWObject * obj)
{
	if (keyA && keylenA > 0)
	{
		key = new char[keylenA + 1];
		memcpy(key, keyA, keylenA);
		keylen = keylenA;
	}
	else
	{
		key = 0;
		keylen = 0;
	}
	
	if (obj)
		object = *obj;
	else
		object.initNull();
		
	reference.initNull();
		
	next = 0;
	closed = 0;
}

XWPDFNameEntry::~XWPDFNameEntry()
{
	if (key)
		delete [] key;
		
	object.free();
}

void XWPDFNameEntry::close(XWDVIRef * xref)
{
	xref->releaseObj(&object);
}

XWObject * XWPDFNameEntry::getReference(XWDVIRef * xref, XWObject * ref)
{
	if (reference.isNull())
		xref->refObj(&object, &reference);
		
	reference.copy(ref);
	
	return ref;
}

void XWPDFNameEntry::setObject(XWObject * obj)
{
	object.free();
	if (obj)
		object = *obj;
	else
		object.initNull();
}

struct named_object
{
  char    * key;
  int       keylen;
  XWObject  * value;
};

static int cmp_key (const void *d1, const void *d2)
{
	named_object *sd1 = (named_object *) d1;
  	named_object *sd2 = (named_object *) d2;
  	int cmp = 0;
  	
  	if (!sd1->key)
    	cmp = -1;
  	else if (!sd2->key)
    	cmp =  1;
  	else 
  	{
    	int keylen = qMin(sd1->keylen, sd2->keylen);
    	cmp = memcmp(sd1->key, sd2->key, keylen);
    	if (!cmp) 
      		cmp = sd1->keylen - sd2->keylen;
  	}

  	return cmp;
}

#define NAME_CLUSTER 4 
static void build_name_tree(XWDVIRef * xref,
                            named_object *first, 
                            long num_leaves, 
                            int is_root, 
                            XWObject * result)
{
	result->initDict(xref);
	if (!is_root) 
	{
		XWObject limits;
		limits.initArray(xref);
    	
    	XWObject obj1;
    	obj1.initString(new XWString(first->key, first->keylen));
    	limits.arrayAdd(&obj1);
    	
    	named_object *last = &first[num_leaves - 1];    	
    	XWObject obj2;
    	obj2.initString(new XWString(last->key , last->keylen));
    	limits.arrayAdd(&obj2);
    	result->dictAdd(qstrdup("Limits"), &limits);
  	}
  	
  	if (num_leaves > 0 && num_leaves <= 2 * NAME_CLUSTER)
  	{
  		XWObject names;
  		names.initArray(xref);
  		XWObject obj1;
  		XWObject obj2;
  		for (int i = 0; i < num_leaves; i++)
  		{
  			named_object *cur = &first[i];
  			obj1.initString(new XWString(cur->key, cur->keylen));
  			names.arrayAdd(&obj1);
  			switch (cur->value->getType())
  			{
  				case XWObject::Array:
  				case XWObject::Dict:
  				case XWObject::Stream:
  				case XWObject::String:
  					{
  						xref->refObj(cur->value, &obj2);
  						names.arrayAdd(&obj2);
  					}
  					break;
  					  					
  				default:
  					cur->value->copy(&obj2);
  					names.arrayAdd(&obj2);
  					break;
  			}
  		}
  		
  		result->dictAdd(qstrdup("Names"), &names);
  	}
  	else if (num_leaves > 0)
  	{
  		XWObject kids;
  		kids.initArray(xref);
  		XWObject subtree;
  		XWObject obj1;
  		for (int i = 0; i < NAME_CLUSTER; i++)
  		{
  			long start = (i*num_leaves) / NAME_CLUSTER;
      		long end   = ((i+1)*num_leaves) / NAME_CLUSTER;
      		build_name_tree(xref, &first[start], (end - start), 0, &subtree);
      		xref->refObj(&subtree, &obj1);
      		kids.arrayAdd(&obj1);
      		xref->releaseObj(&subtree);
  		}
  		
  		result->dictAdd(qstrdup("Kids"), &kids);
  	}
}

XWPDFNames::XWPDFNames()
{
	count = 0;
	table = (XWPDFNameEntry**)malloc(NAME_TABLE_SIZE * sizeof(XWPDFNameEntry*));
	for (int i = 0; i < NAME_TABLE_SIZE; i++)
		table[i] = 0;
		
	itor = 0;
}

XWPDFNames::~XWPDFNames()
{
	for (int i = 0; i < NAME_TABLE_SIZE; i++)
	{
		XWPDFNameEntry * cur = table[i];
		while (cur)
		{
			XWPDFNameEntry * tmp = cur->next;
			delete cur;
			cur = tmp;
		}
	}
	
	free(table);
}

int XWPDFNames::addObject(const void *key, int keylen, XWObject *object)
{
	if (!key || keylen < 1)
	{
		object->free();
		object->initNull();
		return -1;
	}
		
	if (!lookupTable(key, keylen))
	{
		appendTable(key, keylen, object);
		return 0;
	}
	else
	{
		object->free();
		object->initNull();
	}
	
	return -1;
}

void XWPDFNames::appendTable(const void *key, int keylen, XWObject *value)
{
	uint hkey = getHash(key, keylen);
  	XWPDFNameEntry * hent = table[hkey];
  	if (!hent) 
  	{
    	hent = new XWPDFNameEntry((char*)key, keylen, value);
    	table[hkey] = hent;
  	} 
  	else 
  	{
  		XWPDFNameEntry * last = 0;
    	while (hent) 
    	{
      		last = hent;
      		hent = hent->next;
    	}
    	
    	hent = new XWPDFNameEntry((char*)key, keylen, value);
    	last->next = hent;
  	}

  	count++;
}

void XWPDFNames::clearIterator()
{
	if (!itor)
		itor = new Iterator;
		
	itor->index = NAME_TABLE_SIZE;
    itor->curr  = 0;
}

void XWPDFNames::close(XWDVIRef * xref)
{
	for (int i = 0; i < NAME_TABLE_SIZE; i++)
	{
		XWPDFNameEntry * cur = table[i];
		while (cur)
		{
			XWPDFNameEntry * tmp = cur->next;
			cur->close(xref);
			cur = tmp;
		}
	}
	
	if (itor)
		delete itor;
	itor = 0;
}

int XWPDFNames::closeObject(const void *key, int keylen)
{
	XWPDFNameEntry * hent = lookup(key, keylen);
	if (hent)
		hent->close();
		
	return 0;
}

XWObject * XWPDFNames::createTree(XWDVIRef * xref,
	                              long *countA, 
	                              XWPDFNames * filter,
	                              XWObject * result)
{
	named_object * flat = flatTable(countA, filter);
	if (flat)
	{
		qsort(flat, *countA, sizeof(named_object), cmp_key);
		build_name_tree(xref, flat, *countA, 1, result);
		free(flat);
	}
	else
		return 0;
	
	return result;
}

char * XWPDFNames::getKey(int *keylen)
{
	if (!itor)
		setIterator();
		
	XWPDFNameEntry * hent = itor->curr;
  	if (itor && hent) 
  	{
    	*keylen = hent->keylen;
    	return hent->key;
  	} 
  	
  	*keylen = 0;
    return 0;
}

XWObject * XWPDFNames::getVal()
{
	if (!itor)
		setIterator();
		
	XWPDFNameEntry * hent = itor->curr;
  	if (itor && hent) 
    	return hent->getObject();
  	
    return 0;
}

void XWPDFNames::insertTable(const void *key, int keylen, XWObject *value)
{
	if (!key || keylen < 1)
	{
		value->free();
		value->initNull();
		return ;
	}
		
	uint hkey = getHash(key, keylen);
  	XWPDFNameEntry * hent = table[hkey];
  	XWPDFNameEntry * prev = 0;
  	while (hent) 
  	{
    	if (hent->keylen == keylen && !memcmp(hent->key, key, keylen)) 
      		break;
    	prev = hent;
    	hent = hent->next;
  	}
  	
  	if (hent)
  		hent->setObject(value);
  	else
  	{
  		hent = new XWPDFNameEntry((char*)key, keylen, value);
  		if (prev) 
      		prev->next = hent;
    	else 
      		table[hkey] = hent;
    	count++;
  	}
}

XWObject * XWPDFNames::lookupObject(const void *key, 
	                                int keylen)
{
	return lookupTable(key, keylen);
}

XWObject * XWPDFNames::lookupReference(XWDVIRef * xref,
	                                    const void *key, 
	                                   int keylen,
	                                   XWObject * ref)
{
	XWPDFNameEntry * hent = lookup(key, keylen);
	if (hent)
		return hent->getReference(xref, ref);
	
	return 0;
}

XWObject * XWPDFNames::lookupTable(const void *key, int keylen)
{
	XWPDFNameEntry * hent = lookup(key, keylen);
	if (hent)
		return hent->getObject();

  	return 0;
}

bool XWPDFNames::next()
{
	XWPDFNameEntry * hent = itor->curr;
	if (hent)
  		hent = hent->next;
  	while (!hent && ++(itor->index) < NAME_TABLE_SIZE) 
    	hent = table[itor->index];
  	
  	itor->curr = hent;

  	return (hent ? true : false);
}

bool XWPDFNames::setIterator()
{
	if (!itor)
		itor = new Iterator;
		
	for (int i = 0; i < NAME_TABLE_SIZE; i++) 
	{
    	if (table[i]) 
    	{
      		itor->index = i;
      		itor->curr  = table[i];
      		return true;
    	}
  	}
  	
  	return false;
}

named_object * XWPDFNames::flatTable(long *num_entries, 
	                                 XWPDFNames * filter)
{
	named_object * objects = (named_object*)malloc(count * sizeof(named_object));
  	long c = 0;
  	if (setIterator())
  	{
  		int    keylen = 0;
  		do
  		{
  			char * key = getKey(&keylen);
  			if (filter)
  			{
  				XWObject * new_obj = filter->lookupTable(key, keylen);
  				if (!new_obj)
	  				continue;
	  				
	  			XWString * str = new_obj->getString();
	  			key = str->getCString();
				keylen = str->getLength();
  			}
  			
  			XWObject * value = getVal();
  			objects[c].key    = (char *) key;
			objects[c].keylen = keylen;
			objects[c].value  = value;
  			c++;
  		} while (next());
  		
  		clearIterator();
  	}
  	
  	*num_entries = c;
  	objects = (named_object*)realloc(objects, c * sizeof(named_object));

  	return objects;
}

uint XWPDFNames::getHash(const void *key, int keylen)
{
	unsigned int hkey = 0;

  	for (int i = 0; i < keylen; i++) 
    	hkey = (hkey << 5) + hkey + ((char *)key)[i];

  	return (hkey % NAME_TABLE_SIZE);
}

XWPDFNameEntry * XWPDFNames::lookup(const void *key, int keylen)
{
	uint hkey = getHash(key, keylen);
	XWPDFNameEntry * hent = table[hkey];
	while (hent) 
	{
    	if (hent->keylen == keylen && !memcmp(hent->key, key, keylen)) 
      		return hent;
      		
    	hent = hent->next;
  	}

  	return 0;
}

