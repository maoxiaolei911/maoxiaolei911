/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSDICT_H
#define XWPSDICT_H

#include <QObject>
#include "XWPSRefStack.h"

#define dict_max_non_huge ((uint)(PS_MAX_ARRAY_SIZE / 2 + 1))

#define dict_name_index_hash(nidx) (nidx)

#define dict_hash_mod_rem(hash, size) ((hash) % (size))
#define dict_hash_mod_mask(hash, size) ((hash) & ((size) - 1))
#define dict_hash_mod_small(hash, size) dict_hash_mod_rem(hash, size)
#define dict_hash_mod_inline_small(hash, size) dict_hash_mod_rem(hash, size)
#define dict_hash_mod_large(hash, size)\
  (size > dict_max_non_huge ? dict_hash_mod_rem(hash, size) :\
   dict_hash_mod_mask(hash, size))
#define dict_hash_mod_inline_large(hash, size) dict_hash_mod_mask(hash, size)
#define dict_hash_mod(h, s) dict_hash_mod_large(h, s)

class XWPSContextState;
class XWPSDictStack;

class XWPSDict
{
public:
	XWPSDict();
	XWPSDict(uint size);
	~XWPSDict();
	
	XWPSRef * accessRef() {return &values;}
	
	int createContents(uint size, bool pack);
	int createUnpackedKeys(uint asize);
	
	int decRef() { return --refCount; }
	
	XWPSRef * getKeys();
	ushort  * getKeysPacked();
	ushort  * getKeysWritablePacked();
	uint getLength();
	uint getMaxLength();
	static uint getMaxSize();
	XWPSRef * getValues();
	ushort  * getValuesPacked();
	ushort  * getValuesWritablePacked();
	
	bool isPacked();
	
	static uint roundSize(uint rsize);
	static uint roundSizeLarge(uint rsize);
	static uint roundSizeSmall(uint rsize);
	
	int  incRef() { return ++refCount; }
	
	ushort npairs();
	ushort nslots();
	
	int  search(XWPSContextState * ctx, 
	            XWPSRef * pkey,
	            uint nidx, 
	            ushort kpack, 
	            uint hash, 
	            int ktype, 
	            XWPSRef ** ppvalue);
	
	void setMaxLength(uint siz);
	
	int unpack(XWPSContextState * ctx, XWPSDictStack * pds);
	int valueIndex(XWPSRef * pvalue);
	
public:
	int refCount;
	
	XWPSRef values;
	XWPSRef keys;
	XWPSRef count;
	XWPSRef maxlength;
};

class XWPSDictStack : public QObject
{
	Q_OBJECT 
	
public:
	XWPSDictStack(QObject * parent = 0);
	~XWPSDictStack();
	
	void allowExpansion(bool expand);
	
	void cleanup();
	uint count();
	uint countToMark();
	
	int extend(uint request);
	
	bool dictIsPermanent(XWPSRef * pdref);
	
	XWPSRef * findNameByIndex(XWPSContextState * ctx, uint nidx);
	XWPSRef * findNameByIndex(XWPSContextState * ctx, uint nidx, uint & htemp);
	bool      findNameByIndexTop(uint nidx, uint & htemp, XWPSRef ** pvslot);
	
	XWPSRef * getBottom() {return stack.getBottom();}
	XWPSDict * getBottomDict(uint i);
	XWPSRef * getCurrentTop() {return stack.getCurrentTop();}
	int       getDefSpace() {return def_space;}
	uint      getExtensionSize() {return stack.getExtensionSize();}
	uint      getMinSize() {return min_size;}
	XWPSRefStack * getStack() {return &stack;}
	XWPSRef * getSystemDict() {return &system_dict;}
	XWPSRef * getTop() {return stack.getTop();}
	XWPSRef * getUserDict();
	int       getUserDictIndex() {return userdict_index;}
	
	XWPSRef * incCurrentTop(int c);
	XWPSRef * index(long idx);
	void init(XWPSRef *pblock_array,
	          uint bot_guard, 
	          uint top_guard, 
	          XWPSRef *pguard_value,
	          XWPSRefStackParams *paramsA);
	          
	long maxCount();
	
	void pop(uint count);
	int  popBlock();
	int  push(uint count);
	
	void restoreFixStack(bool is_estack);
	
	void setCurrentTop(XWPSRef * np) {stack.setCurrentTop(np);}
	void setErrorCodes(int underflow_errorA, int overflow_errorA);
	int  setMargin(uint marginA);
	int  setMaxCount(long nmax);	
	void setMinSize(int s) {min_size=s;}
	void setRequested(uint i) {stack.setRequested(i);}
	void setSystemDict(XWPSRef * pdict);
	void setTop();
	void setUserDictIndex(int i) {userdict_index=i;}
	int  store(XWPSRef *parray, 
	           uint count,
					   uint skip, 
					   int age, 
					   bool check);
	int  storeCheck(XWPSRef *parray, uint count, uint skip);
	
public:
	XWPSRefStack stack;
	uint min_size;
	int userdict_index;
	int def_space;
	const ushort *top_keys;
	uint top_npairs;
	XWPSRef *top_values;
	XWPSRef system_dict;
};

#endif //XWPSDICT_H
