/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSREFSTACK_H
#define XWPSREFSTACK_H

#include <QObject>
#include "XWPSRef.h"

class XWPSRefStackParams
{
public:
	XWPSRefStackParams();
	XWPSRefStackParams(XWPSRefStackParams * other);
	XWPSRefStackParams(uint bot_guardA,
	                   uint top_guardA,
	                   uint block_sizeA,
	                   uint data_sizeA,
	                   XWPSRef * pguard_value);
	
public:
	uint bot_guard;
	uint top_guard;
	uint block_size;
	uint data_size;
	XWPSRef guard_value;
	int underflow_error;
	int overflow_error;
	bool allow_expansion;
};

class XWPSRefStackBlock
{
public:
	XWPSRefStackBlock() {}
	
public:
	XWPSRef next;
	XWPSRef used;
};

#define stack_block_refs (sizeof(XWPSRefStackBlock) / sizeof(XWPSRef))

  
#ifndef MAX_OSTACK
#  define MAX_OSTACK 800
#endif

#define MIN_BLOCK_OSTACK 16

#ifndef MAX_ESTACK
#  define MAX_ESTACK 250
#endif

#define MIN_BLOCK_ESTACK 8

#define ES_HEADROOM 20

#ifndef MAX_DSTACK
#  define MAX_DSTACK 20
#endif

#define MIN_BLOCK_DSTACK 3

#define OS_GUARD_UNDER 10
#define OS_GUARD_OVER 10
#define OS_REFS_SIZE(body_size)\
  (stack_block_refs + OS_GUARD_UNDER + (body_size) + OS_GUARD_OVER)

#define ES_GUARD_UNDER 1
#define ES_GUARD_OVER 10
#define ES_REFS_SIZE(body_size)\
  (stack_block_refs + ES_GUARD_UNDER + (body_size) + ES_GUARD_OVER)

#define DS_REFS_SIZE(body_size)\
  (stack_block_refs + (body_size))

class XWPSRefStackEnum
{
public:
	XWPSRefStackEnum() : block(0), ptr(0), size(0) {}
	XWPSRefStackEnum(XWPSRefStackBlock * blockA, XWPSRef * ptrA, uint sizeA) 
	  : block(blockA), ptr(ptrA), size(sizeA) {}
		
	void begin(XWPSRefStack * pstack);
	
	bool next();
		
public:
	XWPSRefStackBlock *block;
  XWPSRef *ptr;
  uint size;
};

class XWPSRefStack : public QObject
{
	Q_OBJECT 
	
public:
	friend class XWPSRefStackEnum;
	
	XWPSRefStack(QObject * parent = 0);
	~XWPSRefStack();
	
	void allowExpansion(bool expand);
		
	void cleanup();
	uint count() {return (p + 1 - bot + extension_used);}
	uint countToMark();
	
	int extend(uint request);
	
	XWPSRef * getBottom() {return bot;}
	XWPSDict * getBottomDict(uint i);
	XWPSRef * getCurrentTop() {return p;}
	uint      getExtensionSize() {return extension_size;}
	XWPSRef * getTop() {return top;}
	
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
	          
	void setCurrentTop(XWPSRef * np) {p = np;}
	void setErrorCodes(int underflow_errorA, int overflow_errorA);
	int  setMargin(uint marginA);
	int  setMaxCount(long nmax);
	void setRequested(uint i) {requested=i;}
	int  store(XWPSRef *parray, 
	           uint countA,
					   uint skip, 
					   int age, 
					   bool check);
	int  storeCheck(XWPSRef *parray, uint count, uint skip);
	
private:
	void initBlock(XWPSRef *psb, uint used);
	int  pushBlock(uint keep, uint add);
	
public:
	XWPSRef * p;
	XWPSRef * bot;
	XWPSRef * top;
	XWPSRef   current;
	uint extension_size;
	uint extension_used;
	XWPSRef max_stack;
	uint requested;
	uint margin;
	uint body_size;
	XWPSRefStackParams *params;
};

#define PS_ES_OTHER 0
#define PS_ES_SHOW 1
#define PS_ES_FOR 2
#define PS_ES_STOPPED 3

class XWPSExecStack : public QObject
{
	Q_OBJECT 
	
public:
	XWPSExecStack(QObject * parent = 0);
	~XWPSExecStack();
	
	void allowExpansion(bool expand);
	
	void checkCache();
	void cleanup();
	void clearCache() {current_file = 0;}
	uint count();
	uint countToMark();
	XWPSRef * currentFile() {return current_file;}
	
	int extend(uint request);
	
	XWPSRef * getBottom() {return stack.getBottom();}
	XWPSDict * getBottomDict(uint i);
	XWPSRef * getCurrentTop() {return stack.getCurrentTop();}
	uint      getExtensionSize() {return stack.getExtensionSize();}
	XWPSRefStack * getStack() {return &stack;}
	XWPSRef * getTop() {return stack.getTop();}
	
	XWPSRef * incCurrentTop(int c);
	XWPSRef * index(long idx);
	void init(XWPSRef *pblock_array,
	          uint bot_guard, 
	          uint top_guard, 
	          XWPSRef *pguard_value,
	          XWPSRefStackParams *paramsA);
	          
	long maxCount();
	
	void restoreFixStack(bool is_estack);
	          
	void pop(uint count);
	int  popBlock();
	int  push(uint count);
	          
	void setCache(XWPSRef * f) {current_file=f;}
	void setCurrentTop(XWPSRef * np) {stack.setCurrentTop(np);}
	void setErrorCodes(int underflow_errorA, int overflow_errorA);
	int  setMargin(uint marginA);
	int  setMaxCount(long nmax);
	void setRequested(uint i) {stack.setRequested(i);}
	int  store(XWPSRef *parray, 
	           uint count,
					   uint skip, 
					   int age, 
					   bool check);
	int  storeCheck(XWPSRef *parray, uint count, uint skip);
	
private:
	XWPSRefStack stack;
	XWPSRef *current_file;
};

class XWPSOpStack : public QObject
{
	Q_OBJECT 
	
public:
	XWPSOpStack(QObject * parent = 0);
	~XWPSOpStack();
	
	void allowExpansion(bool expand);
	
	void cleanup();
	uint count();
	uint countToMark();
	
	int extend(uint request);
	
	XWPSRef * getBottom() {return stack.getBottom();}
	XWPSDict * getBottomDict(uint i);
	XWPSRef * getCurrentTop() {return stack.getCurrentTop();}
	uint      getExtensionSize() {return stack.getExtensionSize();}
	XWPSRefStack * getStack() {return &stack;}
	XWPSRef * getTop() {return stack.getTop();}
	
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
	void setRequested(uint i) {stack.setRequested(i);}
	int  store(XWPSRef *parray, 
	           uint count,
					   uint skip, 
					   int age, 
					   bool check);
	int  storeCheck(XWPSRef *parray, uint count, uint skip);
	
private:
	XWPSRefStack stack;
};

#endif //XWPSREFSTACK_H