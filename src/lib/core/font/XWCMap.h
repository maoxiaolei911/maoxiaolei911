/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWCMAP_H
#define XWCMAP_H

#include <QMutex>
#include <QObject>
#include <QString>
#include <QIODevice>
#include "XWGlobal.h"

#define MEM_ALLOC_SIZE  4096

typedef unsigned short CID;
typedef unsigned short UCV16;

#define CID_MAX_CID  65535
#define CID_MAX      CID_MAX_CID

#define CID_NOTDEF_CHAR   "\0\0"
#define CID_NOTDEF        0

#define UCS_NOTDEF_CHAR   "\377\375"
#define UCS_NOTDEF        0xfffd

#define CMAP_TYPE_IDENTITY    0
#define CMAP_TYPE_CODE_TO_CID 1
#define CMAP_TYPE_TO_UNICODE  2
#define CMAP_TYPE_CID_TO_CODE 3

#define CMAP_PROF_TYPE_INBYTES_MIN  0
#define CMAP_PROF_TYPE_INBYTES_MAX  1
#define CMAP_PROF_TYPE_OUTBYTES_MIN 2
#define CMAP_PROF_TYPE_OUTBYTES_MAX 3

#define MAP_IS_CID      (1 << 0)
#define MAP_IS_NAME     (1 << 1)
#define MAP_IS_CODE     (1 << 2)
#define MAP_IS_NOTDEF   (1 << 3)

#define MAP_IS_UNDEF    0
#define MAP_TYPE_MASK   0x00f

#define MAP_DEFINED(e)  (((e) & MAP_TYPE_MASK) != MAP_IS_UNDEF ? 1 : 0)
#define MAP_TYPE(e)     ((e) & MAP_TYPE_MASK)

#define MAP_LOOKUP_END      0
#define MAP_LOOKUP_CONTINUE (1 << 4)
#define LOOKUP_CONTINUE(f) ((f) & MAP_LOOKUP_CONTINUE)
#define LOOKUP_END(f)      (!LOOKUP_CONTINUE((f)))

struct sbuf;

class XWString;
struct CMapVectorEntry;
class XWCMapCache;
class XWCMapFileReader;
class XWObject;
class XWStream;

typedef struct 
{
  	char *registry;
  	char *ordering;
  	int   supplement;
} CIDSysInfo;

XW_FONT_EXPORT extern CIDSysInfo CSI_IDENTITY;
XW_FONT_EXPORT extern CIDSysInfo CSI_UNICODE;

struct MapDef
{
	int    flag;
	int    len;
	uchar *code;
	MapDef * next;
};

struct MapData
{
	long    pos;
	uchar  *data;
	MapData * prev;
};

struct RangeDef
{
	int dim;
	uchar *codeLo;
	uchar *codeHi;
};

struct CodeSpace
{
	int        num;
   	int        max;
   	RangeDef  *ranges;
};

struct Profile
{
	int minBytesIn;
   	int maxBytesIn;
   	int minBytesOut;
   	int maxBytesOut;
};
	
class XW_FONT_EXPORT XWCMap : public QObject
{
	Q_OBJECT
	
public:
	XWCMap(QObject * parent = 0);
	~XWCMap();
	
	int addBFChar(const uchar *src, 
	              int srcdim,
		          const uchar *dst, 
		          int dstdim);
	int addBFRange(const uchar *srclo, 
	               const uchar *srchi, 
	               int srcdim,
		  		   const uchar *base, 
		  		   int dstdim);
	int addCIDChar(const uchar *src, 
	               int srcdim, 
	               CID dst);
	int addCIDRange(const uchar *srclo, 
	                const uchar *srchi, 
	                int srcdim, 
	                CID base);
	int addCodeSpaceRange(const uchar *codelo, 
	                      const uchar *codehi, 
	                      int dim);
	int addNotDefChar(const uchar *src, 
	                  int srcdim, 
	                  CID dst);
	int addNotDefRange(const uchar *srclo, 
	                   const uchar *srchi, 
	                   int srcdim, 
	                   CID dst);
		          
    long decode(const uchar **inbuf,  
	            long *inbytesleft,
	            uchar **outbuf, 
	            long *outbytesleft);
	void decodeChar(const uchar **inbuf, 
	                long *inbytesleft,
		            uchar **outbuf, 
		            long *outbytesleft);
    void decRefCnt();
    
    uint      getCID(char *s, int len, int *c, int *nUsed);
    CIDSysInfo * getCIDSysInfo();
    XWString * getCollection();
    char * getName();
    int    getProfile(int typeA);
    int    getType();
    XWCMap * getUseCMap();
    int    getWMode() { return wMode; }
    
    void incRefCnt();
    bool isIdentity();
    bool isValid();
    
    bool match(XWString *collectionA, XWString *cMapNameA);
    int  matchCodeSpace(const uchar *c, int dim);
    
    bool parse(QIODevice *fp, XWCMapCache * cache);
    static XWCMap *parse(XWCMapCache *cache, XWString *collectionA, XWObject *obj);
    static XWCMap *parse(XWCMapCache *cache, XWString *collectionA, XWStream *str);
    static XWCMap *parse(XWCMapCache *cache, 
                         XWString *collectionA,
		                 XWString *cMapNameA);
	static int parseCheckSig(QIODevice *fp);
	
	void setCIDSysInfo(const CIDSysInfo *csi);
	void setName(const char *nameA);
	void setType(int typeA);
	void setUseCMap(XWCMap *ucmap);
	void setWMode(int wmodeA) {wMode = wmodeA;}
	
	void writeStream(QIODevice *fp);
    
private:
    XWCMap(XWString *collectionA, 
           XWString *cMapNameA, 
           QObject * parent = 0);
    XWCMap(XWString *collectionA, 
           XWString *cMapNameA, 
           int wModeA, 
           QObject * parent = 0);
    
    void addCIDs(uint start, uint end, uint nBytes, uint firstCID);
    void addCodeSpace(CMapVectorEntry *vec, 
                      uint start, 
                      uint end,
		              uint nBytes);
		              
	int blockCount(MapDef *mtab, int c);
	int bytesConsumed(const uchar *instr, long inbytes);
	
	int checkRange(const uchar *srclo, 
	               const uchar *srchi, 
	               int srcdim,
	     		   const uchar *dst, 
	     		   int dstdim);
	void copyVector(CMapVectorEntry *dest, CMapVectorEntry *src);
	
	int doBFChar(XWCMapFileReader *input, int count);
	int doBFRange(XWCMapFileReader *input, int count);
	int doCIDChar(XWCMapFileReader *input, int count);
	int doCIDRange(XWCMapFileReader *input, int count);
	int doCIDSystemInfo(XWCMapFileReader *input);
	int doCodeSpaceRange(XWCMapFileReader *input, int count);
	int doNotDefChar(XWCMapFileReader *input, int count);
	int doNotDefRange(XWCMapFileReader *input, int count);
	
	void freeCMapVector(CMapVectorEntry *vec);
	
	uchar * getMem(int size);
	
	int  handleCodeArray(XWCMapFileReader *input, 
	                     uchar *codeLo, 
	                     int dim, 
	                     int count);
	                     
	void handleUndefined(const uchar **inbuf,  
	                     long *inbytesleft,
		  				 uchar **outbuf, 
		  				 long *outbytesleft);
		  				 
	int locateTbl(MapDef **cur, 
	              const uchar *code, 
	              int dim);
	              
	MapDef * mapDefNew();
	void mapDefRelease(MapDef *t);
	
	void parse2(XWCMapCache *cache, int (*getCharFunc)(void *), void *data);
	
	int sputx(uchar c, char **s, char *end);
		    
		void useCMap(XWCMapCache *cache, XWObject *obj);
    void useCMap(XWCMapCache *cache, char *useName);
    
    int writeMap(MapDef *mtab, 
	             int count,
	             uchar *codestr, 
	             int depth,
	             sbuf *wbuf, 
	             QIODevice *fp);
    
private:
	int refCnt;
	XWString *cMapName;
	bool isIdent;
    int wMode;
    int cmapType;
    
    struct Cmap0
    {
    	XWString *collection;
    	CMapVectorEntry *vector;
    };
    
    struct Cmap1
    {
    	int    type;
    	CIDSysInfo *CSI;
		XWCMap * useCMap;
		int flags;
	
		MapDef * mapTbl;
		MapData * mapData;
		CodeSpace codeSpace;
		Profile profile;
    };
    
    void * map;
    QMutex mutex;
};


#define cMapCacheSize 4

class XW_FONT_EXPORT XWCMapCache 
{
public:
    XWCMapCache(int typeA = 0);
    ~XWCMapCache();
    
    int add(XWCMap *cmap);
    
    int find(const char *cmap_name);
  
  	XWCMap * get(int id);
    XWCMap * getCMap(XWString *collection, XWString *cMapName);

private:
	int type;
	int num;
  	int max;
    XWCMap **cache;
};


#endif // XWCMAP_H

