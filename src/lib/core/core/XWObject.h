/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWOBJECT_H
#define XWOBJECT_H

#include <stdio.h>
#include <string.h>
#include <QByteArray>
#include <QIODevice>
#include <QBuffer>

#include "XWGlobal.h"
#include "XWString.h"

class QTextStream;
class XWRef;
class XWArray;
class XWDict;
class XWStream;

#define STREAM_COMPRESS (1 << 0)

#define OBJ_NO_ENCRYPT  (1 << 1)
#define OBJ_NO_OBJSTM   (1 << 0)


struct XW_CORE_EXPORT ObjRef 
{
    int num;
    int gen;
};

#define NUM_OBJ_TYPES 14

class XW_CORE_EXPORT XWObject
{
public:
	enum ObjectType
	{
		Bool = 0,
		Int,
		Real,
		String,
		Name,
		Null,
		Array,	
		Dict,
		Stream,
		Ref,
		Cmd,
		Error,
		Eof,
		None,
		Indirect,
		Mark,
		Unknown
	};
	
    XWObject() 
       : type(None), 
       	 label(0), 
       	 generation(0), 
       	 refcount(1),
       	 flags(0) {}
    XWObject(ObjectType t) 
        : type(t) , 
          label(0), 
          generation(0), 
          refcount(1),
          flags(0) {}
    	
    void       arrayAdd(XWObject *elem);
    XWObject * arrayGet(int i, XWObject *obj);
    int        arrayGetLength();
    XWObject * arrayGetNF(int i, XWObject *obj);
    XWObject * arrayGetOrg(int i);
    
    XWObject * copy(XWObject *obj);
    
    void     * dataPtr();
    void       dictAdd(char * key, XWObject *val);
    char     * dictGetKey(int i);
    int        dictGetLength();
    XWObject * dictGetVal(int i, XWObject *obj);
    XWObject * dictGetValNF(int i, XWObject *obj);
    XWObject * dictGetValOrg(int i);
    bool       dictIs(char *dictType);
    XWObject * dictKeys(XWObject * keys);
    XWObject * dictLookup(const char *key, XWObject *obj, int recursion = 0);
    XWObject * dictLookupNF(const char *key, XWObject *obj);
    XWObject * dictLookupNFOrg(const char *key);
    void       dictMerge(XWObject *dict2);
    void       dictRemove(const char *key);
    
    XWObject * fetch(XWRef *xref, XWObject *obj, int recursion = 0);
    void free();
    
    XWArray  * getArray() { return array; }
    bool       getBool() { return booln; }
    char     * getCmd() { return cmd; }
    XWDict   * getDict() { return dict; }
    int getFlags() {return flags;}
    int        getInt() { return intg; }
    long       getIV();
    XWObject * getLink();
    char     * getName() { return name; }
    double     getNum() { return type == Int ? (double)intg : real; }
    double     getReal() { return real; }
    ObjRef     getRef() { return ref; }
    int        getRefGen() { return ref.gen; }
    int        getRefNum() { return ref.num; }
    double     getRV();
    XWStream * getStream() { return stream; }
    XWString * getString() { return string; }
    uchar *     getSV();
    ObjectType getType() { return type; }
    char * getTypeName();
    
    void init(ObjectType t);
    
    XWObject *initArray(XWRef *xref);
    
    XWObject *initBool(bool boolnA)
    { init(Bool); booln = boolnA; return this; }
    
    XWObject * initCmd(char *cmdA)
    {init(Cmd); cmd = qstrdup(cmdA); return this; }
    
    XWObject *initDict(XWRef *xref);
  	XWObject *initDict(XWDict *dictA);
  
  	XWObject *initEOF()
    { init(Eof); return this; }	
    
  	XWObject *initError()
    { init(Error); return this; }
    
    XWObject *initIndirect(void * pfA, 
                           int obj_num, 
                           int obj_gen);
    
    XWObject *initInt(int intgA)
    { init(Int); intg = intgA; return this; }
    
    XWObject *initMark()
    {init(Mark); return this;}
    
    XWObject *initName(const char *nameA)
    { init(Name); name = qstrdup(nameA); return this; }
    
    XWObject *initNone()
    { init(None); return this; }
    
  	XWObject *initNull()
    { init(Null); return this; }
    
    XWObject *initReal(double realA)
    { init(Real); real = realA; return this; }
    
    XWObject *initRef(int numA, int genA)
    { init(Ref); ref.num = numA; ref.gen = genA; return this; }
    
    XWObject * initStream(XWStream *streamA);
    XWObject * initStream(int flagsA, XWRef * xrefA);
    
    XWObject *initString(XWString *stringA)
    { init(String); string = stringA; return this; }
    
    XWObject * initUnknown(uchar * d, int len);
    
    bool isArray() { return type == Array; }
    bool isBool() { return type == Bool; }
    bool isCmd() { return type == Cmd; }
    bool isCmd(const char *cmdA)
    { return type == Cmd && !qstrcmp(cmd, cmdA); }
    bool isDict() { return type == Dict; }
    bool isDict(char *dictType);
    bool isEOF() { return type == Eof; }
    bool isError() { return type == Error; }
    bool isIndirect() {return type == Indirect;}
    bool isInt() { return type == Int; }
    bool isMark() { return type == Mark; }
    bool isName() { return type == Name; }
    bool isName(const char *nameA)
    { return type == Name && !qstrcmp(name, nameA); }
    bool isNone() { return type == None; }
    bool isNull() { return type == Null; }
    bool isNum() { return type == Int || type == Real; }
    bool isReal() { return type == Real; }
    bool isRef() { return type == Ref; }
    bool isStream() { return type == Stream; }
    bool isStream(const char *dictType);
    bool isString() { return type == String; }
    bool isUnknown() {return type == Unknown;}
    
    void labelObj();
    long lengthOf();
    void linkObj(XWObject *obj);
    
    void print(QTextStream * f);
    
    void refObj(XWObject * obj);
    static void resetNextLabel(int l);
    
    void     streamAdd(const char *buffer, long length);
    void     streamClose();
    int      streamConcat(XWObject * src);
    char   * streamDataPtr();
    QBuffer* streamGetIO();
    int      streamGetChar();
    XWDict * streamGetDict();
    ulong    streamGetLength();
    char   * streamGetLine(char *buf, int size);
    uint     streamGetPos();
    bool     streamIs(const char *dictType);
    int      streamLookChar();
    QByteArray streamReadAll();
    void     streamReset();
    void     streamSeek(int pos);
    void     streamSetBuffer(const char * buf, int len);
    void     streamSetPos(uint pos, int dir = 0);
    int      streamUncompress(XWRef * xref, XWObject *dst);
    void     streamWrite(QIODevice * out);
    
    void transferLabel(XWObject *src);
                        
public:
    ObjectType type; 
    
    int label;
    int generation;
    int refcount;
    int flags;
    
    union 
    {
    	bool       booln;
    	int        intg;
    	double     real;
    	XWString * string;
    	char     * name;
    	XWArray  * array;
    	XWDict   * dict;
    	XWStream * stream;
    	ObjRef     ref;
    	char     * cmd;
    	void     * pf;
    	char     * data;
    };
    
    static int nextLabel;
};

#include "XWArray.h"

inline void XWObject::arrayAdd(XWObject *elem)
  { array->add(elem); }

inline XWObject *XWObject::arrayGet(int i, XWObject *obj)
  { return array->get(i, obj); }

inline int XWObject::arrayGetLength()
  { return array->getLength(); }

inline XWObject *XWObject::arrayGetNF(int i, XWObject *obj)
  { return array->getNF(i, obj); }

inline XWObject * XWObject::arrayGetOrg(int i)
{
	return array->getValOrg(i);
}

#include "XWDict.h"

inline void XWObject::dictAdd(char *key, XWObject *val)
  { dict->add(key, val); }

inline char * XWObject::dictGetKey(int i)
  { return dict->getKey(i); }

inline int XWObject::dictGetLength()
  { return dict->getLength(); }

inline XWObject * XWObject::dictGetVal(int i, XWObject *obj)
  { return dict->getVal(i, obj); }

inline XWObject * XWObject::dictGetValNF(int i, XWObject *obj)
  { return dict->getValNF(i, obj); }

inline XWObject * XWObject::dictGetValOrg(int i)
{
	return dict->getValOrg(i);
}

inline bool XWObject::dictIs(char *dictType)
  { return dict->is(dictType); }

inline XWObject * XWObject::dictLookup(const char *key, XWObject *obj, int recursion)
  { return dict->lookup(key, obj, recursion); }

inline XWObject * XWObject::dictLookupNF(const char *key, XWObject *obj)
  { return dict->lookupNF(key, obj); }

inline bool XWObject::isDict(char *dictType)
  { return type == Dict && dictIs(dictType); }

inline void XWObject::dictRemove(const char *key)
{ dict->remove(key);}

#include "XWStream.h"

inline bool XWObject::streamIs(const char *dictType)
  { return stream->getDict()->is(dictType); }

inline bool XWObject::isStream(const char *dictType)
  { return type == Stream && streamIs(dictType); }

inline void XWObject::streamReset()
  { stream->reset(); }

inline void XWObject::streamClose()
  { stream->close(); }

inline int XWObject::streamGetChar()
  { return stream->getChar(); }

inline int XWObject::streamLookChar()
  { return stream->lookChar(); }

inline char * XWObject::streamGetLine(char *buf, int size)
  { return stream->getLine(buf, size); }

inline uint XWObject::streamGetPos()
  { return stream->getPos(); }

inline void XWObject::streamSetPos(uint pos, int dir)
  { stream->setPos(pos, dir); }

inline XWDict * XWObject::streamGetDict()
  { return stream->getDict(); }

#endif // XWOBJECT_H

