/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSPARAM_H
#define XWPSPARAM_H

#include <QObject>
#include "XWPSRef.h"

class XWPSContextState;
class XWPSRefStack;
class XWPSParamList;
class XWPSCParamList;
class XWPSCosObject;
class XWPSStream;
struct PSStreamCFState;
struct PSStreamDCTState;
struct PSJPEGCompressData;
class XWObject;

class PSParamCollection
{
public:
	PSParamCollection() : list(0), size(0), persistent(true) {}
	~PSParamCollection();
		
public:
	XWPSParamList *list;
  uint size;
  bool persistent;
};

union PSParamValue
{
	bool b;
	int i;
	long l;
	float f;
	PSParamString * s;
	PSParamString * n;
	PSParamIntArray * ia;
	PSParamFloatArray * fa;
	PSParamStringArray * sa;
	PSParamStringArray * na;
	PSParamCollection * d;
};

class PSParamTypedValue
{
public:
	PSParamTypedValue();
	PSParamTypedValue(PSParamType t);
	PSParamTypedValue(const bool b);
	PSParamTypedValue(const int i);
	PSParamTypedValue(const long l);
	PSParamTypedValue(const float f);
	PSParamTypedValue(const int * ia, uint size);
	PSParamTypedValue(const PSParamIntArray * ia);
	PSParamTypedValue(const float * fa, uint size);
	PSParamTypedValue(const PSParamFloatArray * fa);
	PSParamTypedValue(const PSParamString * s, bool n = false);
	PSParamTypedValue(const PSParamStringArray * sa, bool n = false);
	~PSParamTypedValue();
	
	void clear();
	
	void reset(PSParamType t);
	
public:
	PSParamValue value;
  PSParamType type;
};

union PSCParamValue
{
	bool b;
	int i;
	long l;
	float f;
	PSParamString * s;
	PSParamString * n;
	PSParamIntArray * ia;
	PSParamFloatArray * fa;
	PSParamStringArray * sa;
	PSParamStringArray * na;
	XWPSCParamList * d;
};

class PSCParam
{
public:
	PSCParam();
	~PSCParam();
	
	void clear();
	
	void reset(PSParamType t);
		
public:
	PSCParam *next;
  PSParamString key;
  PSCParamValue value;
  PSParamType type;
  float *alternate_typed_data;
};

struct PSIParamLoc
{
	XWPSRef *pvalue;
  int *presult;
};

class XWPSParamEnumerator
{
public:
	XWPSParamEnumerator() : intval(0), longval(0), pvoid(0), pchar(0) {}
	
public:
	int intval;
  long longval;
  void *pvoid;
  char *pchar;
};

class XWPSParamList : public QObject
{
	Q_OBJECT 
	
public:
	XWPSParamList(QObject * parent = 0);
	virtual ~XWPSParamList() {}
	
	virtual int antiAliasBits(XWPSContextState * ctx, 
	                          const char * param_name, 
	                          int *pa);
	
	virtual int beginTransmit(XWPSContextState *,
	                          const char * , 
	                          PSParamCollection * , 
	                          PSParamCollectionType ) {return 0;}
	                          
	        
	virtual int checkBool(XWPSContextState * ctx, 
	                      const char * pname, 
	                      bool value,
		                    bool defined);
	virtual int checkBytes(XWPSContextState * ctx, 
	                       const char * pname, 
	                       const uchar * str,
		  								   uint size, 
		  								   bool defined);
	virtual int checkLong(XWPSContextState * ctx, 
	                      const char * pname, 
	                      long value,
		                    bool defined);
	virtual int checkPassword(XWPSContextState * ctx, const PSPassword * ppass);
	virtual int commit() {return 0;}
	virtual int copy(XWPSContextState * ctx, XWPSParamList * plfrom);
	
	virtual int endTransmit(XWPSContextState * ,
	                        const char * , 
	                        PSParamCollection * ) {return 0;}
	
	        int getParams(XWPSContextState * ctx, const PSStreamCFState * ss, bool all);
	        int sDCTDGetParams(XWPSContextState * ctx, const PSStreamDCTState * ss, bool all);
	        int sDCTEGetParams(XWPSContextState * ctx, const PSStreamDCTState * ss, bool all);
	virtual int getPolicy(XWPSContextState *,const char *) {return 0;}
	        int sDCTGetQuantizationTables(XWPSContextState * ctx, 
	                                      const PSStreamDCTState * pdct, 
	                                      const PSStreamDCTState * defaults,
			                                   bool is_encode);
					int sDCTGetHuffmanTables(XWPSContextState * ctx,
	   															 const PSStreamDCTState * pdct, 
	   															 const PSStreamDCTState * defaults,
			                             bool is_encode);
					int sDCTGetParams(XWPSContextState * ctx, 
					                  const PSStreamDCTState * ss,
		                        const PSStreamDCTState * defaults);
					int sDCTByteParams(XWPSContextState * ctx, 
					                   const char * key, 
					                   int start,
		                         int count, 
		                         quint8 * pvals);
					int quantParams(XWPSContextState * ctx, 
					                const char * key, 
					                int count,
	                        quint16 * pvals, 
	                        float QFactor);
					int sDCTPutParams(XWPSContextState * ctx, PSStreamDCTState * pdct);
					int sDCTPutQuantizationTables(XWPSContextState * ctx, 
					                              PSStreamDCTState * pdct,
			                                  bool is_encode);
					int sDCTPutHuffmanTables(XWPSContextState * ctx, 
					                         PSStreamDCTState * pdct,
			                             bool is_encode);
					int sDCTDPutParams(XWPSContextState * ctx, PSStreamDCTState * pdct);
					int sDCTEPutParams(XWPSContextState * ctx, PSStreamDCTState * pdct);
	
	virtual int mediaSize(XWPSContextState *ctx, 
	                      const char * pname,
				                const float *res, 
				                PSParamFloatArray * pa);
	
	virtual int nextKey(XWPSContextState *, XWPSParamEnumerator * ,PSParamString *) {return 0;}
	
	virtual int putBool(XWPSContextState * ctx, 
	                    const char * param_name,
	                    bool * pval, 
	                    int ecode);
	virtual int putEnum(XWPSContextState * ctx, 
	                    const char * param_name,
	                    int *pvalue, 
	                    const char *const pnames[], 
	                    int ecode);
	virtual int putInt(XWPSContextState * ctx, 
	                   const char * param_name,
	                   int *pval, 
	                   int ecode);
	virtual int putLong(XWPSContextState * ctx, 
	                    const char * param_name,
	                    long *pval, 
	                    int ecode);
	        int putParams(XWPSContextState * ctx, PSStreamCFState * ss);
	
	virtual int readBool(XWPSContextState * ctx, const char * pkey, bool * pvalue);
	virtual int readFloat(XWPSContextState * ctx, const char * pkey, float *pvalue);
	virtual int readFloatArray(XWPSContextState * ctx, const char * pkey, PSParamFloatArray * pvalue);
	virtual int readInt(XWPSContextState * ctx, const char * pkey, int *pvalue);
	virtual int readIntArray(XWPSContextState * ctx, const char * pkey, PSParamIntArray * pvalue);
	virtual int readItems(XWPSContextState * ctx, void * obj, PSParamItem * items);
	virtual int readLong(XWPSContextState * ctx, const char * pkey, long *pvalue);
	virtual int readName(XWPSContextState * ctx, const char * pkey,	PSParamString * pvalue);
	virtual int readNameArray(XWPSContextState * ctx, const char * pkey, PSParamStringArray * pvalue);
	virtual int readNull(XWPSContextState * ctx, const char * pkey);
	virtual int readPassword(XWPSContextState * ctx, const char *kstr, PSPassword * ppass);
	virtual int readRequestedTyped(XWPSContextState * ctx, const char* pkey, PSParamTypedValue * pvalue);	
	virtual int readString(XWPSContextState * ctx, const char* pkey, PSParamString * pvalue);
	virtual int readStringArray(XWPSContextState * ctx, const char* pkey, PSParamStringArray * pvalue);
	virtual int request(XWPSContextState *, const char *) {return 0;}
	virtual int requested(XWPSContextState *, const char *)  {return -1;}
	
	void setPersistentKeys(bool persistent) {persistent_keys=persistent;}
	virtual int signalError(XWPSContextState *, const char *, int)  {return 0;}
	                          
	virtual int transmit(XWPSContextState *, const char * , PSParamTypedValue * )  {return 0;}
	
	virtual int writeBool(XWPSContextState * ctx, const char * pkey, const bool * pvalue);
	virtual int writeFloat(XWPSContextState * ctx, const char * pkey, const float *pvalue);
	virtual int writeFloatArray(XWPSContextState * ctx, 
	                    const char * pkey, 
	                    const PSParamFloatArray * pvalue);
	virtual int writeFloatArray(XWPSContextState * ctx, const char *key, const float *values, int count);
	virtual int writeFloatValues(XWPSContextState * ctx, 
	                     const char * pkey, 
	                     const float *values, 
	                     uint size, bool persistent);
	virtual int writeInt(XWPSContextState * ctx, const char * pkey, const int *pvalue);
	virtual int writeIntArray(XWPSContextState * ctx, const char * pkey, const PSParamIntArray * pvalue);
	virtual int writeIntValues(XWPSContextState * ctx, 
	                   const char * pkey, 
	                   const int *values, 
	                   uint size, 
	                   bool persistent);
	virtual int writeItems(XWPSContextState * ctx, void *obj, void *default_obj, PSParamItem * items);
	virtual int writeLong(XWPSContextState * ctx, const char * pkey, const long *pvalue);
	virtual int writeName(XWPSContextState * ctx, const char * pkey, const PSParamString * pvalue);
	virtual int writeNameArray(XWPSContextState * ctx, const char * pkey, const PSParamStringArray * pvalue);
	virtual int writeNull(XWPSContextState * ctx, const char * pkey);
	virtual int writePassword(XWPSContextState * ctx, const char *kstr, const PSPassword * ppass);
	virtual int writeString(XWPSContextState * ctx, const char * pkey, const PSParamString * pvalue);
	virtual int writeStringArray(XWPSContextState * ctx, const char * pkey, const PSParamStringArray * pvalue);
	
protected:
	int coerceTyped(PSParamTypedValue * pvalue, PSParamType req_type);
	
	int dcteGetSamples(XWPSContextState * ctx, 
	                   const char *  key, 
	                   int num_colors,
                     const PSJPEGCompressData * jcdp, 
                     bool is_vert, 
                     bool all);
	int dctePutSamples(XWPSContextState * ctx, 
	                   const char * key, 
	                   int num_colors,
		                 PSJPEGCompressData * jcdp, 
		                 bool is_vert);
	int findHuffValues(JHUFF_TBL ** table_ptrs, 
	                     int num_tables,
	                     const quint8 counts[16], 
	                     const quint8 * values, 
	                     int codes_size);
	
public:
	bool persistent_keys;
};

class XWPSCParamList : public XWPSParamList
{
	Q_OBJECT 
	
public:
	XWPSCParamList(QObject * parent = 0);
	virtual ~XWPSCParamList();
	
	virtual int beginTransmit(XWPSContextState * ctx, 
	                  const char * pkey, 
	                  PSParamCollection * pvalue, 
	                  PSParamCollectionType coll_typeA);
	
	virtual int endTransmit(XWPSContextState * ctx, const char * pkey, PSParamCollection * pvalue);
	
	virtual int getPolicy(XWPSContextState *, const char *);
	
	virtual int nextKey(XWPSContextState *ctx, XWPSParamEnumerator * penum, PSParamString *key);
	
	void read() {writing=false;}	
	virtual int request(XWPSContextState * ctx, const char *pkey);
	virtual int requested(XWPSContextState * ctx, const char *pkey);
	
	void setTarget(XWPSParamList * t) {target=t;}
	virtual int signalError(XWPSContextState *, const char * pkey, int code);
	
	virtual int transmit(XWPSContextState * ctx, const char * pkey, PSParamTypedValue * pvalue);
	
	void write() {writing=true;}	
	
protected:
	PSCParam * add(const char * pkey);
	
	virtual int beginRead(XWPSContextState * ctx, 
	                      const char * pkey, 
	                      PSParamCollection * pvalue, 
	                      PSParamCollectionType coll_typeA);	               
	virtual int beginWrite(XWPSContextState * ctx, 
	                       const char * pkey, 
	                       PSParamCollection * pvalue, 
	                       PSParamCollectionType coll_typeA);
	               
	virtual int endRead(XWPSContextState * ctx, const char * pkey, PSParamCollection * pvalue);
	virtual int endWrite(XWPSContextState * ctx, const char * pkey, PSParamCollection * pvalue);	
	
	PSCParam * find(const char * pkey, bool any);
	
	virtual int  read(XWPSContextState * ctx, const char * pkey, PSParamTypedValue * pvalue);	 
	virtual int  readGetPolicy(XWPSContextState * , const char *);            
	virtual int  readNextKey(XWPSParamEnumerator * penum, PSParamString *key);     
	virtual int  readSignalError(XWPSContextState * , const char *, int code);
	void release();
	
	virtual int write(XWPSContextState * ctx, const char * pkey, PSParamTypedValue * pvalue);	
	int write(const char * pkey, void *pvalue, PSParamType type);
	virtual int writeRequest(XWPSContextState * ctx, const char *pkey);
	virtual int writeRequested(XWPSContextState * ctx, const char *pkey);
	
public:
	 bool writing;
	PSCParam *head;
  XWPSParamList *target;
  uint count;
  bool any_requested;
  PSParamCollectionType coll_type;
};

class XWPSIParamList : public XWPSParamList
{
	Q_OBJECT 
	
public:
	XWPSIParamList(QObject * parent = 0);
	virtual ~XWPSIParamList();
	
	virtual int beginTransmit(XWPSContextState * ctx, 
	                          const char * pkey, 
	                          PSParamCollection * pvalue, 
	                          PSParamCollectionType coll_typeA);
	
	virtual int commit();
	
	virtual int endTransmit(XWPSContextState * ctx, const char * pkey, PSParamCollection * pvalue);
	virtual int enumerate(XWPSContextState *, 
	                      XWPSParamEnumerator * ,
		                    PSParamString * , 
		                  XWPSRef::RefType * ) {return 0;}
		                  	
	virtual int getPolicy(XWPSContextState * ctx, const char * pkey);
	
	virtual int nextKey(XWPSContextState *ctx, XWPSParamEnumerator * penum, PSParamString *key);
	
	void read() {writing=false;}
	virtual int read(XWPSContextState * , XWPSRef * , PSIParamLoc * ) {return 0;}
	virtual int request(XWPSContextState * ctx, const char *pkey);
	virtual int requested(XWPSContextState * ctx, const char *pkey);
	
	virtual int signalError(XWPSContextState * ctx, const char * pkey, int code);
	
	virtual int transmit(XWPSContextState * ctx, const char * pkey, PSParamTypedValue * pvalue);
	
	void write() {writing=true;}	
	virtual int write(XWPSContextState * , XWPSRef * , XWPSRef * ) {return 0;}
	
protected:
	virtual int beginRead(XWPSContextState * ctx, 
	                       const char * pkey, 
	                       PSParamCollection * pvalue, 
	                       PSParamCollectionType coll_typeA);
	virtual int beginWrite(XWPSContextState * ctx, 
	                       const char * pkey, 
	                       PSParamCollection * pvalue, 
	                       PSParamCollectionType coll_typeA);	
	                       
	virtual int endRead(XWPSContextState * ctx, const char * pkey, PSParamCollection * pvalue);	
	virtual int endWrite(XWPSContextState * ctx, const char * pkey, PSParamCollection * pvalue);	
	
	int read(XWPSContextState * ctx, const char * pkey, PSParamTypedValue * pvalue);
	int readInit(uint countA, XWPSRef * ppolicies, bool require_allA);
	int refArrayParamRequested(XWPSContextState * ctx, 
	                           const char * pkey,
			                       XWPSRef *pvalue, 
			                       uint size);
	int refParamKey(XWPSContextState * ctx, const char * pkey, XWPSRef * pkref);
	int refParamRead(XWPSContextState * ctx, 
	                 const char * pkey, 
	                 PSIParamLoc * ploc,
	                 int type);
	int refParamReadArray(XWPSContextState * ctx, const char * pkey, PSIParamLoc * ploc);
	int refParamReadFloatArray(XWPSContextState * ctx, const char * pkey, PSParamFloatArray * pvalue);
	int refParamReadIntArray(XWPSContextState * ctx, const char * pkey, PSParamIntArray * pvalue);
	int refParamReadStringArray(XWPSContextState * ctx, const char * pkey, PSParamStringArray * pvalue);
	int refParamReadStringValue(XWPSContextState * ctx, PSIParamLoc * ploc, PSParamString * pvalue);
	int refParamRequested(XWPSContextState * ctx, const char *pkey);
	int refParamWrite(XWPSContextState * ctx, const char * pkey, XWPSRef * pvalue);
	int refParamWriteNameValue(XWPSContextState * ctx, XWPSRef * pref, PSParamString * pvalue);
	int refParamWriteStringValue(XWPSContextState * ctx, XWPSRef * pref, PSParamString * pvalue);
	int refParamWriteTypedArray(XWPSContextState * ctx, 
	                            const char * pkey, 
	                            void *pvalue, 
	                            uint count,
	                            PSParamType t);	
	int refToKey(XWPSContextState * ctx, XWPSRef * pref, PSParamString * key);
	
	int write(XWPSContextState * ctx, const char * pkey, PSParamTypedValue * pvalue);
	int writeInit(XWPSRef * pwanted);
	virtual int writeRequested(XWPSContextState * ctx, const char *pkey);
	
public:
	bool writing;	
	XWPSRef policies;
	bool require_all;			
	XWPSRef wanted;	
	int *results;
	uint count;
	bool int_keys;
};

class XWPSDictParamList : public XWPSIParamList
{
	Q_OBJECT 
	
public:
	XWPSDictParamList(bool a,
	                  XWPSRef * pdict = 0,
	                  XWPSRef * pwanted = 0,
	                  QObject * parent = 0);
	XWPSDictParamList(bool a,
	                  XWPSRef * pdict = 0,
	                  XWPSRef * ppolicies = 0,
	                  bool require_allA = false,
	                  QObject * parent = 0);
	
	int enumerate(XWPSContextState * ctx, 
	              XWPSParamEnumerator * penum,
		            PSParamString * key, 
		            XWPSRef::RefType * type);
		            	
	XWPSRef * getDict() {return &dict;}
	
	int read(XWPSContextState * ctx, XWPSRef * pkey, PSIParamLoc * ploc);
		
	int write(XWPSContextState * ctx, XWPSRef * pkey, XWPSRef * pvalue);
	
public:
	bool array;
	XWPSRef dict;
};

class XWPSArrayParamList : public XWPSIParamList
{
	Q_OBJECT 
	
public:
	XWPSArrayParamList(XWPSRef * botA, 
	                   uint countA = 0,
		                 XWPSRef * ppolicies = 0, 
		                 bool require_allA = false, 
		                 QObject * parent = 0);
	
	int enumerate(XWPSContextState * ctx, 
	              XWPSParamEnumerator * penum,
		            PSParamString * key, 
		            XWPSRef::RefType * type);
	
	int read(XWPSContextState * ctx, XWPSRef * pkey, PSIParamLoc * ploc);
	            	
public:
	XWPSRef *bot;
  XWPSRef *top;
};

class XWPSStackParamList : public XWPSIParamList
{
	Q_OBJECT 
	
public:
	XWPSStackParamList(XWPSRefStack * stack,
	                   XWPSRef * pwanted = 0,	                   
	                   QObject * parent = 0);
	XWPSStackParamList(XWPSRefStack * stack,
	                    uint skipA = 0, 
	                    XWPSRef * ppolicies = 0, 
	                    bool require_allA = false,	                   
	                   QObject * parent = 0);
	
	int enumerate(XWPSContextState * ctx, 
	              XWPSParamEnumerator * penum,
		            PSParamString * key, 
		            XWPSRef::RefType * type);
		                  	
	int read(XWPSContextState * ctx, XWPSRef * pkey, PSIParamLoc * ploc);
	
	int write(XWPSContextState * , XWPSRef * pkey, XWPSRef * pvalue);
	
public:
	XWPSRefStack *pstack;
  uint skip;
};

struct PSPrinterParams
{
	const char *prefix;
  const char *suffix;
  const char *item_prefix;
  const char *item_suffix;
  int print_ok;
};

class XWPSPrinterParamList : public XWPSParamList
{
	Q_OBJECT 
	
public:
	XWPSPrinterParamList(XWPSStream * s,
	                     PSPrinterParams * ppp,
	                     QObject * parent = 0);
	~XWPSPrinterParamList();
	
	void release();
	
	int transmit(XWPSContextState * ctx, const char * pkey, PSParamTypedValue * pvalue);
	
public:
	XWPSStream *strm;
  PSPrinterParams params;
  bool any;
};

class XWPSCosParamListWriter : public XWPSParamList
{
	Q_OBJECT 
	
public:
	XWPSCosParamListWriter(XWPSCosObject *pcdA,
				                 int print_okA,
				                 QObject * parent = 0);
	~XWPSCosParamListWriter();
	
	int transmit(XWPSContextState * ctx, const char * pkey, PSParamTypedValue * pvalue);
	
public:
	XWPSCosObject *pcd;
  int print_ok;
};

class XWPSObjectParamListWriter : public XWPSParamList
{
	Q_OBJECT 
	
public:
	XWPSObjectParamListWriter(XWObject *pcdA,
				                 int print_okA,
				                 QObject * parent = 0);
	~XWPSObjectParamListWriter();
	
	int transmit(XWPSContextState * ctx, const char * pkey, PSParamTypedValue * pvalue);
	
public:
	XWObject *pcd;
  int print_ok;
};

#endif //XWPSPARAM_H
