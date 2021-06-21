/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSNAME_H
#define XWPSNAME_H

#include <QObject>

#ifndef EXTEND_NAMES		/* # of bits beyond 16 */
#define EXTEND_NAMES 0
#endif

#define NT_LOG2_SUB_SIZE (8 + (EXTEND_NAMES / 2))
#define NT_SUB_SIZE (1 << NT_LOG2_SUB_SIZE)
#define NT_SUB_INDEX_MASK (NT_SUB_SIZE - 1)

#define NT_1CHAR_SIZE 128
#define NT_1CHAR_FIRST 2

#define NAME_COUNT_TO_INDEX_FACTOR 23
#define name_count_to_index(cnt)\
  (((cnt) & (-NT_SUB_SIZE)) +\
   (((cnt) * NAME_COUNT_TO_INDEX_FACTOR) & NT_SUB_INDEX_MASK))

#define NAME_INDEX_TO_COUNT_FACTOR 1959
#define name_index_to_count(nidx)\
  (((nidx) & (-NT_SUB_SIZE)) +\
   (((nidx) * NAME_INDEX_TO_COUNT_FACTOR) & NT_SUB_INDEX_MASK))
   
#if EXTEND_NAMES
#define name_extension_bits 6
#else
#define name_extension_bits 0
#endif

#define name_string_size_bits (14 - name_extension_bits)
#define max_name_string ((1 << name_string_size_bits) - 1)

#define NT_HASH_SIZE (1024 << (EXTEND_NAMES / 2)) 

#define nt_log2_sub_size NT_LOG2_SUB_SIZE
#define nt_sub_size (1 << nt_log2_sub_size)
#define nt_sub_index_mask (nt_sub_size - 1)

#define max_name_extension_bits 6
#if EXTEND_NAMES > max_name_extension_bits
#undef EXTEND_NAMES
#define EXTEND_NAMES max_name_extension_bits
#endif

#define max_name_index (uint)((0x10000 << EXTEND_NAMES) - 1)

#define max_name_count max_name_index


class XWPSRef;

class XWPSNameString
{
public:
	XWPSNameString() : next_index(0),
		                 foreign_string(0),
		                 mark(0),
		                 string_size(0),
		                 string_bytes(0) {}
	~XWPSNameString();
		
	unsigned nameNextIndex() {return next_index;}
	
	void setNameNextIndex(unsigned next) {next_index=next;}
	
public:
	unsigned next_index:16 + name_extension_bits;
	unsigned foreign_string:1;
	unsigned mark:1;
	unsigned string_size:name_string_size_bits;
	uchar *string_bytes;
};

class XWPSNameStringSubTable
{
public:
	XWPSNameStringSubTable() {}
	
public:
	XWPSNameString strings[NT_SUB_SIZE];
};

class XWPSName
{
public:
	XWPSName() : pvalue(0) {}
		
		
	XWPSRef * getValue() {return pvalue;}
	bool isNoDefn() {return pvalue==(XWPSRef*)0;}
	bool isOther() {return pvalue==(XWPSRef*)1;}
	bool isValid() {return pvalue > (XWPSRef*)1;}
	
	void invalidate() {pvalue=(XWPSRef*)1;}
	
public:
	XWPSRef * pvalue;
};

class XWPSNameSubTable
{
public:
	XWPSNameSubTable() {}
	
public:
	XWPSName names[NT_SUB_SIZE];
#ifdef EXTEND_NAMES
    uint high_index;		/* sub-table base index & (-1 << 16) */
#endif
};

class XWPSNameTable : public QObject
{
	Q_OBJECT 
	
public:
	XWPSNameTable(ulong countA = 0, QObject * parent = 0);
	~XWPSNameTable();
	
	int  namesEnterString(const char *str, XWPSRef * pref);
	int  namesFromString(XWPSRef * psref, XWPSRef * pnref);
	uint namesIndex(XWPSRef * pnref);
	XWPSName * namesIndexPtr(uint nidx);
	void namesIndexRef(uint nidx, XWPSRef * pnref);	
	XWPSNameString * namesIndexString(uint nidx);
	XWPSNameStringSubTable * namesIndexStringSubTable(uint index);
	XWPSNameSubTable * namesIndexSubTable(uint index);
	void namesInvalidateValueCache(XWPSRef * pnref);
	bool namesMarkIndex(uint nidx);
	uint namesNextValidIndex(uint nidx);
	int  namesRef(const uchar *ptr, uint size, XWPSRef *pref, int enterflag);
	XWPSName * namesRefSubTable(XWPSRef * pnref);
	XWPSNameString * namesString(XWPSRef * pnref);
	void namesStringRef(XWPSRef * pnref, XWPSRef * psref);
	void namesTraceFinish();
	void namesUnmarkAll();
	
private:
	int allocSub();
	uint nameHash(const uchar * ptr, uint size);
	void nameScanSub(uint sidx, bool free_empty);
	
private:
	uint free;
	uint sub_next;
	uint perm_count;
	uint sub_count;
	uint max_sub_count;
	uint name_string_attrs;
	uint hash[NT_HASH_SIZE];
	struct sub_
	{
		XWPSNameSubTable *names;
		XWPSNameStringSubTable *strings;
	} sub[max_name_index / nt_sub_size + 1];
};

inline XWPSName * XWPSNameTable::namesIndexPtr(uint nidx)
{return (sub[nidx >> nt_log2_sub_size].names->names +(nidx & nt_sub_index_mask));}

inline XWPSNameString * XWPSNameTable::namesIndexString(uint nidx)
{return (sub[nidx >> nt_log2_sub_size].strings->strings + (nidx & nt_sub_index_mask));}

#endif //XWPSNAME_H
