/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <string.h>
//#include <QFile>
//#include <QTextStream>
#include "XWPSError.h"
#include "XWPSRef.h"
#include "XWPSName.h"


//static QFile * pshashfile = 0;
//static QTextStream * pshashstream = 0;
  
static uchar nt_1char_names[NT_1CHAR_SIZE] = {
	0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
  16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
  32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
  48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
  64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
  80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
  96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,
 112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127
};

static uchar hash_permutation[256] =  {
	1, 87, 49, 12, 176, 178, 102, 166, 121, 193, 6, 84, 249, 230, 44, 163,
   14, 197, 213, 181, 161, 85, 218, 80, 64, 239, 24, 226, 236, 142, 38, 200,
   110, 177, 104, 103, 141, 253, 255, 50, 77, 101, 81, 18, 45, 96, 31, 222,
   25, 107, 190, 70, 86, 237, 240, 34, 72, 242, 20, 214, 244, 227, 149, 235,
   97, 234, 57, 22, 60, 250, 82, 175, 208, 5, 127, 199, 111, 62, 135, 248,
   174, 169, 211, 58, 66, 154, 106, 195, 245, 171, 17, 187, 182, 179, 0, 243,
   132, 56, 148, 75, 128, 133, 158, 100, 130, 126, 91, 13, 153, 246, 216, 219,
   119, 68, 223, 78, 83, 88, 201, 99, 122, 11, 92, 32, 136, 114, 52, 10,
   138, 30, 48, 183, 156, 35, 61, 26, 143, 74, 251, 94, 129, 162, 63, 152,
   170, 7, 115, 167, 241, 206, 3, 150, 55, 59, 151, 220, 90, 53, 23, 131,
   125, 173, 15, 238, 79, 95, 89, 16, 105, 137, 225, 224, 217, 160, 37, 123,
   118, 73, 2, 157, 46, 116, 9, 145, 134, 228, 207, 212, 202, 215, 69, 229,
   27, 188, 67, 124, 168, 252, 42, 4, 29, 108, 21, 247, 19, 205, 39, 203,
   233, 40, 186, 147, 198, 192, 155, 33, 164, 191, 98, 204, 165, 180, 117, 76,
   140, 36, 210, 172, 41, 54, 159, 8, 185, 232, 113, 196, 231, 47, 146, 120,
   51, 65, 28, 144, 254, 221, 93, 189, 194, 139, 112, 43, 71, 109, 184, 209
};

XWPSNameString::~XWPSNameString()
{
	if (!foreign_string)
	{
		if (string_bytes)
		  delete [] string_bytes;
		string_bytes = 0;
	}
}

XWPSNameTable::XWPSNameTable(ulong countA, QObject * parent)
	:QObject(parent),
	 free(0),
	 sub_next(0),
	 perm_count(0),
	 sub_count(0),
	 max_sub_count(0),
	 name_string_attrs(0)
{
	memset(hash, 0, NT_HASH_SIZE * sizeof(uint));
	for (int i = 0; i <= (max_name_index / nt_sub_size); i++)
	{
		sub[i].names = 0;
		sub[i].strings = 0;
	}
	
	max_sub_count =	((countA - 1) | nt_sub_index_mask) >> nt_log2_sub_size;
	name_string_attrs = PS_AVM_SYSTEM | PS_A_READONLY;
	for (int i = 0; i < NT_1CHAR_FIRST + NT_1CHAR_SIZE; i += nt_sub_size)
		allocSub();
		
	for (int i = -1; i < NT_1CHAR_SIZE; i++)
	{
		uint ncnt = NT_1CHAR_FIRST + i;
		uint nidx = name_count_to_index(ncnt);
		XWPSName *pname = namesIndexPtr(nidx);
		XWPSNameString *pnstr = namesIndexString(nidx);
		if (i < 0)
		{
	    pnstr->string_bytes = nt_1char_names;
			pnstr->string_size = 0;
		}
		else
		{
	    pnstr->string_bytes = nt_1char_names + i;
			pnstr->string_size = 1;
		}
		pnstr->foreign_string = 1;
		pnstr->mark = 1;
		pname->pvalue = 0;
	}
	
	perm_count = NT_1CHAR_FIRST + NT_1CHAR_SIZE;
	namesTraceFinish();
}

XWPSNameTable::~XWPSNameTable()
{
	for (int i = 0; i <= (max_name_index / nt_sub_size); i++)
	{
		if (sub[i].names)
			delete sub[i].names;
			
		if (sub[i].strings)
			delete sub[i].strings;
			
		sub[i].names = 0;
		sub[i].strings = 0;
	}
}

int XWPSNameTable::namesEnterString(const char *str, XWPSRef * pref)
{
	return namesRef((const uchar *)str, strlen(str), pref, 0);
}

int  XWPSNameTable::namesFromString(XWPSRef * psref, XWPSRef * pnref)
{
	int exec = psref->hasAttr(PS_A_EXECUTABLE);
  int code = namesRef(psref->getBytes(), psref->size(), pnref, 1);
  if (code < 0)
		return code;
  if (exec)
		pnref->setAttrs(PS_A_EXECUTABLE);
  return code;
}

uint XWPSNameTable::namesIndex(XWPSRef * pnref)
{
#if EXTEND_NAMES
	return (((XWPSNameSubTable *)(pnref->getName() - (pnref->size() & nt_sub_index_mask)))->high_index + pnref->size());
#else
	return pnref->size();
#endif
}

void XWPSNameTable::namesIndexRef(uint nidx, XWPSRef * pnref)
{
	XWPSName * n = namesIndexPtr(nidx);
	pnref->makeName(nidx, n);
}

XWPSNameStringSubTable * XWPSNameTable::namesIndexStringSubTable(uint index)
{
	return sub[index >> nt_log2_sub_size].strings;
}

XWPSNameSubTable * XWPSNameTable::namesIndexSubTable(uint index)
{
	return sub[index >> nt_log2_sub_size].names;
}

int  XWPSNameTable::namesRef(const uchar *ptr, uint size, XWPSRef *pref, int enterflag)
{
	XWPSName *pname;
  XWPSNameString *pnstr;
  uint nidx;
  uint *phash;
  switch (size)
  {
  	case 0:
  		nidx = name_count_to_index(1);
			pname = namesIndexPtr(nidx);
			goto mkn;
  		
  	case 1:
  		if (*ptr < NT_1CHAR_SIZE) 
  		{
	    	uint h = *ptr + NT_1CHAR_FIRST;
	    	nidx = name_count_to_index(h);
	    	pname = namesIndexPtr(nidx);
	    	goto mkn;
			}
			
		default:
			{
				uint h;
				const uchar *p = ptr;
				uint n = size;
				h = hash_permutation[*p++];
				while (--n > 0)
					h = (h << 8) | hash_permutation[(uchar)h ^ *p++];
				phash = hash + (h & (NT_HASH_SIZE - 1));
			}
			break;
  }
  
  for (nidx = *phash; nidx != 0; nidx = pnstr->nameNextIndex())
  {
  	pnstr = namesIndexString(nidx);
  	if (pnstr->string_size == size && !memcmp(ptr, pnstr->string_bytes, size)) 
  	{
	    pname = namesIndexPtr(nidx);
	    goto mkn;
		}
  }
  
  if (enterflag < 0)
  	return (int)(XWPSError::Undefined);
  		
  if (size > max_name_string)
		return (int)(XWPSError::LimitCheck);
			
  nidx = free;
  if (nidx == 0) 
  {
		int code = allocSub();

		if (code < 0)
	    return code;
		nidx = free;
  }
  pnstr = namesIndexString(nidx);
  if (enterflag == 1) 
  {
		uchar *cptr = new uchar[size + 1];
		memcpy(cptr, ptr, size);
		pnstr->string_bytes = cptr;
		pnstr->foreign_string = 0;
  } 
  else 
  {
  	uchar *cptr = new uchar[size + 1];
		memcpy(cptr, ptr, size);
		pnstr->string_bytes = cptr;
		pnstr->foreign_string = 0;
		
		//pnstr->string_bytes = (uchar*)ptr;
		//pnstr->foreign_string = (enterflag == 0 ? 1 : 0);
  }
  
  pnstr->string_size = size;
  pname = namesIndexPtr(nidx);
  pname->pvalue = 0;
  free = pnstr->nameNextIndex();
  pnstr->setNameNextIndex(*phash);
  *phash = nidx;
  
mkn:
  pref->makeName(nidx, pname);
  
/*
  if (!pshashfile)
  {
  	pshashfile = new QFile("d:/work/src/test/pshash.txt");
  	pshashfile->open(QIODevice::WriteOnly | QIODevice::Text);
  	pshashstream = new QTextStream(pshashfile);
  }
  
  char buffff[256];
  memcpy(buffff, ptr, size);
  buffff[size] = 0;
  *pshashstream << buffff << "  ---> nidx " << nidx << "  size " << pref->size() << "\n";
*/
  return 0;
}

void XWPSNameTable::namesInvalidateValueCache(XWPSRef * pnref)
{
	pnref->getName()->invalidate();
}

bool XWPSNameTable::namesMarkIndex(uint nidx)
{
	XWPSNameString *pnstr = namesIndexString(nidx);
  if (pnstr->mark)
		return false;
  pnstr->mark = 1;
  return true;
}

uint XWPSNameTable::namesNextValidIndex(uint nidx)
{
	XWPSNameStringSubTable *ssub = sub[nidx >> nt_log2_sub_size].strings;
  XWPSNameString *pnstr;

  do 
  {
		++nidx;
		if ((nidx & nt_sub_index_mask) == 0)
		{
	    for (;; nidx += nt_sub_size) 
	    {
				if ((nidx >> nt_log2_sub_size) >= sub_count)
		    	return 0;
				ssub = sub[nidx >> nt_log2_sub_size].strings;
				if (ssub != 0)
		    	break;
	    }
	  }
		pnstr = &ssub->strings[nidx & nt_sub_index_mask];
  }  while (pnstr->string_bytes == 0);
  
  return nidx;
}

XWPSName * XWPSNameTable::namesRefSubTable(XWPSRef * pnref)
{
	return (pnref->getName() - (pnref->size() & nt_sub_index_mask));
}

XWPSNameString * XWPSNameTable::namesString(XWPSRef * pnref)
{
	return namesIndexString(namesIndex(pnref));
}

void XWPSNameTable::namesStringRef(XWPSRef * pnref, XWPSRef * psref)
{
	XWPSNameString *pnstr = namesString(pnref);
	psref->makeString(pnstr->foreign_string ? PS_AVM_FOREIGN | PS_A_READONLY : name_string_attrs); 
	psref->setSize(pnstr->string_size); 
	psref->value.bytes->ptr = pnstr->string_bytes;
}

void XWPSNameTable::namesTraceFinish()
{
	uint *phash = &hash[0];
  for (uint i = 0; i < NT_HASH_SIZE; phash++, i++)
  {
  	uint prev = 0;
  	XWPSNameString *pnprev = 0;
		uint nidx = *phash;
		while (nidx != 0)
		{
			XWPSNameString *pnstr = namesIndexString(nidx);
	    uint next = pnstr->nameNextIndex();
	    if (pnstr->mark) 
	    {
				prev = nidx;
				pnprev = pnstr;
	    } 
	    else 
	    {
	    	if (pnstr->string_bytes)
	    	{
	    		if (!pnstr->foreign_string)
						delete [] pnstr->string_bytes;
	    	}
	    	
				pnstr->string_bytes = 0;
				pnstr->string_size = 0;
				if (prev == 0)
		    	*phash = next;
				else
		    	pnprev->setNameNextIndex(next);
	    }
	    nidx = next;
		}
  }
  
  free = 0;
  for (uint i = sub_count; i--;) 
  {
		XWPSNameSubTable *subA = sub[i].names;
//		XWPSNameStringSubTable *ssubA = sub[i].strings;

		if (subA != 0) 
	    nameScanSub(i, true);
		if (i == 0)
	    break;
  }
  sub_next = 0;
}

void XWPSNameTable::namesUnmarkAll()
{
	XWPSNameStringSubTable *ssub;
	for (uint si = 0; si < sub_count; ++si)
	{
		if ((ssub = sub[si].strings) != 0) 
		{
	    for (uint i = 0; i < nt_sub_size; ++i)
	    {
				if (name_index_to_count((si << nt_log2_sub_size) + i) >= perm_count)
		    	ssub->strings[i].mark = 0;
		  }
		}
	}
}

int XWPSNameTable::allocSub()
{
  uint sub_index = sub_next;
  for (;; ++sub_index)
  {
  	if (sub_index > max_sub_count)
  		return (int)(XWPSError::LimitCheck);
  			
  	if (sub[sub_index].names == 0)
	    break;
  }
  
  sub_next = sub_index + 1;
  if (sub_next > sub_count)
		sub_count = sub_next;
		
	XWPSNameSubTable *subA = new XWPSNameSubTable;
  XWPSNameStringSubTable *ssubA = new XWPSNameStringSubTable;
  		
#if name_extension_bits > 0
    subA->high_index = (sub_index >> (16 - nt_log2_sub_size)) << 16;
#endif
	sub[sub_index].names = subA;
  sub[sub_index].strings = ssubA;
  nameScanSub(sub_index, false);
  return 0;
}

uint XWPSNameTable::nameHash(const uchar * ptr, uint size)
{
	const uchar *p = ptr;
	uint n = size;
	uint h = hash_permutation[*p++];
	while (--n > 0)
		h = (h << 8) | hash_permutation[(uchar)h ^ *p++];
	
	return h;
}

void XWPSNameTable::nameScanSub(uint sidx, bool free_empty)
{
	XWPSNameStringSubTable *ssub = sub[sidx].strings;
  uint f = free;
  uint nbase = sidx << nt_log2_sub_size;
  uint ncnt = nbase + (nt_sub_size - 1);
  bool keep = !free_empty;

  if (ssub == 0)
		return;
		
  if (nbase == 0)
  {
		nbase = 1; 
		keep = true;
	}
    
  for (;; --ncnt) 
  {
		uint nidx = name_count_to_index(ncnt);
		XWPSNameString *pnstr = &ssub->strings[nidx & nt_sub_index_mask];

		if (pnstr->mark)
	    keep = true;
		else 
		{
	    pnstr->setNameNextIndex(f);
	    f = nidx;
		}
		if (ncnt == nbase)
	    break;
  }
  if (keep)
		free = f;
  else 
  {
		if (sub[sidx].names)
			delete sub[sidx].names;
			
		if (sub[sidx].strings)
			delete sub[sidx].strings;
			
		sub[sidx].names = 0;
		sub[sidx].strings = 0;
		
		if (sidx == sub_count - 1) 
		{
	    do 
	    {
				--sidx;
	    } while (sub[sidx].names == 0);
	    
	    sub_count = sidx + 1;
	    if (sub_next > sidx)
				sub_next = sidx;
		} 
		else if (sub_next == sidx)
	    sub_next--;
  }
}
