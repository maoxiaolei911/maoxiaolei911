/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSCOSOBJECT_H
#define XWPSCOSOBJECT_H

#include <QtGlobal>

class XWPSStream;
class XWPSDevicePDF;
class XWPSCosObject;

enum PSCosValueType
{
  COS_VALUE_SCALAR = 0,
  COS_VALUE_CONST,
  COS_VALUE_OBJECT,
  COS_VALUE_RESOURCE
};

struct PSCosString
{
	uchar *data;
	uint   size;
};

class XWPSCosValue
{
public:
	XWPSCosValue();
	XWPSCosValue(int i);
	XWPSCosValue(float i);
	XWPSCosValue(const char *str);
	XWPSCosValue(const uchar *data, uint size);	
	XWPSCosValue(XWPSCosObject * obj);	
	XWPSCosValue(XWPSCosObject * obj, PSCosValueType t);	
	XWPSCosValue(XWPSCosValue * pvalue);
	~XWPSCosValue();
	
	void copy(XWPSCosValue * pvalue);
	
	void myFree();
	
	bool isObject() {return (type >= COS_VALUE_OBJECT);}
	
	void objectValue(XWPSCosObject * obj);
	void resourceValue(XWPSCosObject * obj);
	void stringValue(const char *str);
	void stringValue(const uchar *data, uint size);
			
	int write(XWPSDevicePDF * pdev);
	int writeObject(XWPSDevicePDF * pdev);
	
public:
	PSCosValueType type;
	union vc_ 
	{
		PSCosString chars;
		XWPSCosObject *object;
  } contents;
};

class XWPSCosElement
{
public:
	XWPSCosElement() : next(0) {}
	virtual ~XWPSCosElement() {}
	
public:
	XWPSCosElement * next;
};

class XWPSCosStreamPiece : public XWPSCosElement
{
public:
	XWPSCosStreamPiece();
	
public:
	long position;
	uint size;
};

class XWPSCosArrayElement : public XWPSCosElement
{
public:
	XWPSCosArrayElement();
	
public:
	long index;
	XWPSCosValue value;
};

class XWPSCosDictElement : public XWPSCosElement
{
public:
	XWPSCosDictElement();
	~XWPSCosDictElement();
	
	int write(XWPSStream * s, XWPSDevicePDF * pdev);
	int writeObject(XWPSDevicePDF * pdev);
	
public:
	PSCosString key;
  bool owns_key;
  XWPSCosValue value;
};

struct PSCosObjectProcs
{
	void (XWPSCosObject::*release)();
	int  (XWPSCosObject::*write)(XWPSDevicePDF*);
};

extern PSCosObjectProcs cos_generic_procs;
extern PSCosObjectProcs cos_array_procs;
extern PSCosObjectProcs cos_dict_procs;
extern PSCosObjectProcs cos_stream_procs;

class XWPSCosObject
{
public:
	XWPSCosObject(XWPSDevicePDF * devA);
	XWPSCosObject(XWPSDevicePDF * devA, PSCosObjectProcs *procs);
	XWPSCosObject(XWPSDevicePDF * devA, const float *pf, uint size);
	~XWPSCosObject();
		
	int arrayAdd(XWPSCosValue *pvalue);
	int arrayAdd(const char *str);
	int arrayAdd(int i);
	int arrayAdd(float r);
	int arrayAdd(XWPSCosObject * pco);
	int arrayPut(long index, XWPSCosValue *pvalue);
	
	int  become(PSCosObjectProcs * cotype);
	
	int decRef() { return --refCount; }
	int dictElementsWrite(XWPSDevicePDF * devA);
	XWPSCosValue * dictFind(const char *key);
	XWPSCosValue * dictFind(const uchar *key_data, uint key_size);
	int dictObjectsDetete();
	int dictObjectsWrite(XWPSDevicePDF * devA);
	int dictPut(const char *key, const char *str);
	int dictPut(const char *key, const float *pf, uint size);
	int dictPut(const char *key, float r);
	int dictPut(const char *key, int i);
	int dictPut(const char *key,  const uchar *data, uint size);
	int dictPut(const char *key, XWPSCosValue *pvalue);
	int dictPut(const char *key, XWPSCosObject *pc);
	int dictPut(const uchar *key_data, 
	        uint key_size,
	         XWPSCosValue *pvalue);
	int dictPut(const uchar *key_data, 
	        uint key_size,
		      const uchar *value_data, 
		      uint value_size);
	int dictPut(const uchar *key_data, 
	        uint key_size, 
	        XWPSCosValue *pvalue, 
	        int flags);
	
	int  incRef() { return ++refCount; }
	void init(PSCosObjectProcs *procs);
	
	int streamAdd(uint size);
	int streamAddBytes(const uchar *data, uint size);
	int streamAddSince(long start_pos);	
	int streamContentsWrite(XWPSDevicePDF * devA);
	int streamElementsWrite(XWPSDevicePDF * devA);
	long streamLength();
	
	int  write(XWPSDevicePDF * devA);
	int  writeObject(XWPSDevicePDF * devA);
	

	int elementsWrite(XWPSStream * s, XWPSDevicePDF * devA);
	
	long nextIndex();
	
	void releaseArray();	
	void releaseDict();
	void releaseGeneric() {}
	void releaseStream();
	
	void reorder();
	
	int  writeArray(XWPSDevicePDF * devA);
	int  writeDict(XWPSDevicePDF * devA);
	int  writeGeneric(XWPSDevicePDF * devA);
	int  writeStream(XWPSDevicePDF * devA);
	
public:
	PSCosObjectProcs *cos_procs;
	
	XWPSCosElement * elements;
	long id;
	XWPSCosStreamPiece *pieces;
	XWPSDevicePDF *pdev;
	uchar is_open;
	uchar is_graphics;
	uchar written;
	int refCount;
};

#endif //XWPSCOSOBJECT_H
