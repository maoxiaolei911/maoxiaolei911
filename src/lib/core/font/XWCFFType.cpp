/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include "cffstdstr.h"
#include "XWCFFType.h"

XWCFFIndex::XWCFFIndex()
{
	count = 0;
	offsize = 0;
	offset = 0;
	data = 0;
}

XWCFFIndex::XWCFFIndex(ushort countA)
{
	count = countA;
  	offsize = 0;
  	if (count > 0) 
  	{
    	offset = (ulong*)malloc((count + 1) * sizeof(ulong));
    	offset[0] = 1;
  	} 
  	else 
    	offset = 0;
  	
  	data = 0;
}

XWCFFIndex::XWCFFIndex(XWFontFileCFF * fileA, 
	                   bool onlyheader)
{
	count = fileA->getCard16();
	if (count > 0)
	{
		offsize = fileA->getCard8();
		offset = (ulong*)malloc((count + 1) * sizeof(ulong));
		for (ushort i = 0; i < count+1; i++) 
      		offset[i] = fileA->getOffset(offsize);
      		
      	if (!onlyheader)
      	{
      		long lengthA = offset[count] - offset[0];
      		data = (uchar*)malloc(lengthA * sizeof(uchar));
      		long offsetA  = 0;
      		long nb_read = 0;
      		while (lengthA > 0) 
      		{
      			nb_read = fileA->read((char*)(data + offsetA), lengthA);
      			offsetA += nb_read;
      			lengthA -= nb_read;
    		}
      	}
      	else
      		data = 0;
	}
	else
	{
		offsize = 0;
    	offset = 0;
    	data = 0;
	}
}

XWCFFIndex::~XWCFFIndex()
{
	if (offset)
		free(offset);
		
	if (data)
		free(data);
}

ushort XWCFFIndex::addString(const char *str, int unique)
{
	long len = strlen(str);
	if (unique) 
	{
    	/* TODO: do binary search to speed things up */
    	for (ushort idx = 0; idx < CFF_STDSTR_MAX; idx++) 
    	{
      		if (cff_stdstr[idx] && !strcmp(cff_stdstr[idx], str))
				return idx;
    	}
    	
    	for (ushort idx = 0; idx < count; idx++) 
    	{
      		ulong size   = offset[idx+1] - offset[idx];
      		ulong offsetA = offset[idx];
      		if (size == len && !memcmp(data+offsetA-1, str, len))
				return (idx + CFF_STDSTR_MAX);
    	}
  	}
  	
  	long offsetA = count > 0 ? offset[count] : 1;
  	offset = (ulong*)realloc(offset, (count+2) * sizeof(ulong));
  	if (count == 0)
    	offset[0] = 1;
  	ushort idx = count;
  	count += 1;
  	offset[count] = offsetA + len;
  	data = (uchar*)realloc(data, (offsetA+len-1) * sizeof(uchar));
  	memcpy(data + offsetA - 1, str, len);

  	return (idx + CFF_STDSTR_MAX);
}

char * XWCFFIndex::getName(int indexA)
{
	ulong len = offset[indexA + 1] - offset[indexA];
	char * fontnameA = new char[len + 1];
	memcpy(fontnameA, data + offset[indexA] - 1, len);
  	fontnameA[len] = '\0';

  	return fontnameA;
}

long XWCFFIndex::getSID(char *str)
{
	if (!str)
		return -1;

    for (int i = 0; i < count; i++) 
    {
      	if ((strlen(str) == (offset[i+1] - offset[i])) && 
	  		!memcmp(str, data + offset[i] - 1, strlen(str)))
			return (i + CFF_STDSTR_MAX);
  	}

  	for (int i = 0; i < CFF_STDSTR_MAX; i++) 
  	{
    	if (!strcmp(str, cff_stdstr[i]))
      		return i;
  	}

  	return -1;
}

char * XWCFFIndex::getString(ushort id)
{
	char * result = 0;
    id -= CFF_STDSTR_MAX;
    if (id < count) 
    {
    	long len = offset[id + 1] - offset[id];
    	result = new char[len + 1];
    	memmove(result, data + offset[id] - 1, len);
    	result[len] = '\0';
    }

  	return result;
}

int XWCFFIndex::matchString(const char *str, ushort sid)
{
    ushort i = sid - CFF_STDSTR_MAX;
    if (i >= count)
    	return 0;
      		
    if (strlen(str) == offset[i+1] - offset[i])
    	return (!memcmp(str, data + offset[i]-1, strlen(str))) ? 1 : 0;

  	return 0;
}

long XWCFFIndex::pack(uchar *dest, long destlen)
{
	if (count < 1) 
	{
    	if (destlen < 2)
      		return -1;
      		
    	memset(dest, 0, 2);
    	return 2;
  	}
  	
  	long len = size();
  	long datalen = offset[count] - 1;
  	if (destlen < len)
  		return -1;
  		
  	*(dest++) = (count >> 8) & 0xff;
  	*(dest++) = count & 0xff;

  	if (datalen < 0xffUL) 
  	{
    	offsize = 1;
    	*(dest++) = 1;
    	for (int i = 0; i <= count; i++) 
      		*(dest++) = (uchar)(offset[i] & 0xff);
  	} 
  	else if (datalen < 0xffffUL) 
  	{
    	offsize = 2;
    	*(dest++) = 2;
    	for (int i = 0; i <= count; i++) 
    	{
      		*(dest++) = (uchar) ((offset[i] >> 8) & 0xff);
      		*(dest++) = (uchar) (offset[i] & 0xff);
    	}
  	} 
  	else if (datalen < 0xffffffUL) 
  	{
    	offsize = 3;
    	*(dest++)    = 3;
    	for (int i = 0; i <= count; i++) 
    	{
      		*(dest++) = (uchar)((offset[i] >> 16) & 0xff);
      		*(dest++) = (uchar)((offset[i] >> 8) & 0xff);
      		*(dest++) = (uchar)(offset[i] & 0xff);
    	}
  	} 
  	else 
  	{
    	offsize = 4;
    	*(dest++)    = 4;
    	for (int i = 0; i <= count; i++) 
    	{
      		*(dest++) = (uchar)((offset[i] >> 24) & 0xff);
      		*(dest++) = (uchar)((offset[i] >> 16) & 0xff);
      		*(dest++) = (uchar)((offset[i] >> 8) & 0xff);
      		*(dest++) = (uchar)(offset[i] & 0xff);
    	}
  	}

  	memmove(dest, data, offset[count] - 1);

  	return len;
}

long XWCFFIndex::setName(char * nameA)
{
	count   = 1;
  	offsize = 1;
  	
  	if (offset)
  		free(offset);  		
  	offset = (ulong*)malloc(2 * sizeof(ulong));
  	offset[0] = 1;
  	offset[1] = strlen(nameA) + 1;
  	
  	if (data)
  		free(data);  		
  	data = (uchar*)malloc(strlen(nameA) * sizeof(uchar));
  	memmove(data, nameA, strlen(nameA));
  	return 5 + strlen(nameA);
}

long XWCFFIndex::size()
{
	if (count > 0) 
	{
    	ulong datalen = offset[count] - 1;
    	if (datalen < 0xffUL) 
      		offsize = 1;
    	else if (datalen < 0xffffUL) 
      		offsize = 2;
    	else if (datalen < 0xffffffUL) 
      		offsize = 3;
    	else 
      		offsize = 4;
    	
    	return (3 + offsize * (count + 1) + datalen);
  	} 
    	
    return 2;
}

XWCFFHeader::XWCFFHeader()
	:major(0),
	 minor(0),
	 hdr_size(0),
	 offsize(0)
{
}

XWCFFHeader::XWCFFHeader(XWFontFileCFF * fileA)
{
	major    = fileA->getCard8();
  	minor    = fileA->getCard8();
  	hdr_size = fileA->getCard8();
  	offsize  = fileA->getCard8();
  	fileA->seek(hdr_size);
}

long XWCFFHeader::put(uchar *dest)
{
	*(dest++) = major;
  	*(dest++) = minor;
  	*(dest++) = 4;
  	*(dest++) = 4;
  	offsize = 4;

  	return 4;
}

#define CFF_PARSE_OK                0
#define CFF_ERROR_PARSE_ERROR      -1
#define CFF_ERROR_STACK_OVERFLOW   -2
#define CFF_ERROR_STACK_UNDERFLOW  -3
#define CFF_ERROR_STACK_RANGECHECK -4

#define DICT_ENTRY_MAX 16

#define CFF_DICT_STACK_LIMIT 64

static int    stack_top = 0;
static double arg_stack[CFF_DICT_STACK_LIMIT];

#define CFF_LAST_DICT_OP1 22
#define CFF_LAST_DICT_OP2 39
#define CFF_LAST_DICT_OP (CFF_LAST_DICT_OP1 + CFF_LAST_DICT_OP2)


static struct {
  const char *opname;
  int   argtype;
} dict_operator[CFF_LAST_DICT_OP] = {
  	{"version",     CFF_TYPE_SID},
  	{"Notice",      CFF_TYPE_SID},
  	{"FullName",    CFF_TYPE_SID},
  	{"FamilyName",  CFF_TYPE_SID},
  	{"Weight",      CFF_TYPE_SID},
  	{"FontBBox",    CFF_TYPE_ARRAY},
  	{"BlueValues",       CFF_TYPE_DELTA},
  	{"OtherBlues",       CFF_TYPE_DELTA},
  	{"FamilyBlues",      CFF_TYPE_DELTA},
  	{"FamilyOtherBlues", CFF_TYPE_DELTA},
  	{"StdHW",            CFF_TYPE_NUMBER},
  	{"StdVW",            CFF_TYPE_NUMBER},
  	{NULL, -1},  /* first byte of two-byte operator */
  	/* Top */
  	{"UniqueID",    CFF_TYPE_NUMBER},
  	{"XUID",        CFF_TYPE_ARRAY},
  	{"charset",     CFF_TYPE_OFFSET},
  	{"Encoding",    CFF_TYPE_OFFSET},
  	{"CharStrings", CFF_TYPE_OFFSET},
  	{"Private",     CFF_TYPE_SZOFF}, /* two numbers (size and offset) */
  	/* Private */
  	{"Subrs",         CFF_TYPE_OFFSET},
  	{"defaultWidthX", CFF_TYPE_NUMBER},
  	{"nominalWidthX", CFF_TYPE_NUMBER},
  	/* Operator 2 */
  	{"Copyright",          CFF_TYPE_SID},
  	{"IsFixedPitch",       CFF_TYPE_BOOLEAN},
  	{"ItalicAngle",        CFF_TYPE_NUMBER},
  	{"UnderlinePosition",  CFF_TYPE_NUMBER},
  	{"UnderlineThickness", CFF_TYPE_NUMBER},
  	{"PaintType",      CFF_TYPE_NUMBER},
  	{"CharstringType", CFF_TYPE_NUMBER},
  	{"FontMatrix",     CFF_TYPE_ARRAY},
  	{"StrokeWidth",    CFF_TYPE_NUMBER},
  	{"BlueScale", CFF_TYPE_NUMBER},
  	{"BlueShift", CFF_TYPE_NUMBER},
  	{"BlueFuzz",  CFF_TYPE_NUMBER},
  	{"StemSnapH", CFF_TYPE_DELTA},
  	{"StemSnapV", CFF_TYPE_DELTA},
  	{"ForceBold", CFF_TYPE_BOOLEAN},
  	{NULL, -1},
  	{NULL, -1},
  	{"LanguageGroup",     CFF_TYPE_NUMBER},
  	{"ExpansionFactor",   CFF_TYPE_NUMBER},
  	{"InitialRandomSeed", CFF_TYPE_NUMBER},
  	{"SyntheticBase", CFF_TYPE_NUMBER},
  	{"PostScript",    CFF_TYPE_SID},
  	{"BaseFontName",  CFF_TYPE_SID},
  	{"BaseFontBlend", CFF_TYPE_DELTA}, /* MMaster ? */
  	{NULL, -1},
  	{NULL, -1},
  	{NULL, -1},
  	{NULL, -1},
  	{NULL, -1},
  	{NULL, -1},
  	/* CID-Keyed font */
  	{"ROS",             CFF_TYPE_ROS}, /* SID SID number */
  	{"CIDFontVersion",  CFF_TYPE_NUMBER},
  	{"CIDFontRevision", CFF_TYPE_NUMBER},
  	{"CIDFontType",     CFF_TYPE_NUMBER},
  	{"CIDCount",        CFF_TYPE_NUMBER},
  	{"UIDBase",         CFF_TYPE_NUMBER},
  	{"FDArray",         CFF_TYPE_OFFSET},
  	{"FDSelect",        CFF_TYPE_OFFSET},
  	{"FontName",        CFF_TYPE_SID},
};

XWCFFDict::XWCFFDict()
{
	max     = DICT_ENTRY_MAX;
  	count   = 0;
  	entries = (CFFDictEntry*)malloc(max * sizeof(CFFDictEntry));
}

XWCFFDict::XWCFFDict(uchar *dataA, uchar *endptr)
{
	max     = DICT_ENTRY_MAX;
  	count   = 0;
  	entries = (CFFDictEntry*)malloc(max * sizeof(CFFDictEntry));
	int status = CFF_PARSE_OK;
  	stack_top = 0;
  	
  	while (dataA < endptr && status == CFF_PARSE_OK)
  	{
  		if (*dataA < 22)
  			add(&dataA, endptr, &status);
  		else if (*dataA == 30)
  		{
  			if (stack_top < CFF_DICT_STACK_LIMIT) 
  			{
				arg_stack[stack_top] = getReal(&dataA, endptr, &status);
				stack_top++;
      		} 
      		else 
				status = CFF_ERROR_STACK_OVERFLOW;
  		}
  		else if (*dataA == 255 || (*dataA >= 22 && *dataA <= 27))
  			dataA++;
  		else
  		{
  			if (stack_top < CFF_DICT_STACK_LIMIT) 
  			{
				arg_stack[stack_top] = getInteger(&dataA, endptr, &status);
				stack_top++;
      		} 
      		else 
				status = CFF_ERROR_STACK_OVERFLOW;
  		}
  	}
  	
  	if (stack_top != 0)
  		stack_top = 0;
  		
  	ok = status == CFF_PARSE_OK;
}

XWCFFDict::~XWCFFDict()
{
	if (entries)
	{
		for (int i = 0; i < count; i++)
		{
			if (entries[i].values)
				free(entries[i].values);
		}
		
		free(entries);
	}
}

void XWCFFDict::add(const char *key, int countA)
{
	int id = 0;
	for (; id < CFF_LAST_DICT_OP; id++) 
	{
    	if (key && dict_operator[id].opname && strcmp(dict_operator[id].opname, key) == 0)
      		break;
  	}

  	if (id == CFF_LAST_DICT_OP)
    	return ;

  	for (int i = 0; i < count; i++) 
  	{
    	if (entries[i].id == id) 
    	{
      		if (entries[i].count != countA)
				return;
      		return;
    	}
  	}

  	if (count + 1 >= max) 
  	{
    	max += 8;
    	entries = (CFFDictEntry*)realloc(entries, max * sizeof(CFFDictEntry));
  	}

  	entries[count].id    = id;
  	entries[count].key   = (char *) dict_operator[id].opname;
  	entries[count].count = countA;
  	if (countA > 0) 
  	{
    	entries[count].values = (double *)malloc(countA * sizeof(double));
    	memset(entries[count].values, 0, sizeof(double) * countA);
  	} 
  	else 
    	entries[count].values = 0;
  	
  	count += 1;
}

double XWCFFDict::get(const char *key, int idx)
{
	double value = 0.0;
	for (int i = 0; i < count; i++) 
	{
    	if (strcmp(key, entries[i].key) == 0)
    	{
      		if (entries[i].count > idx)
				value = entries[i].values[idx];
      		break;
    	}
  	}

  	return value;
}

int XWCFFDict::known(const char *key)
{
	for (int i = 0; i < count; i++) 
	{
    	if (key && strcmp(key, entries[i].key) == 0 && entries[i].count > 0)
      		return 1;
  	}

  	return 0;
}

long XWCFFDict::pack(uchar *dest, long destlen)
{
	long len = 0;
	for (int i = 0; i < count; i++) 
	{
    	if (!strcmp(entries[i].key, "ROS")) 
    	{
      		len += putEntry(&entries[i], dest, destlen);
      		break;
    	}
  	}
  	
  	for (int i = 0; i < count; i++) 
  	{
    	if (strcmp(entries[i].key, "ROS")) 
      		len += putEntry(&entries[i], dest+len, destlen-len);
  	}

  	return len;
}

void XWCFFDict::remove(const char *key)
{
	for (int i = 0; i < count; i++) 
	{
    	if (key && strcmp(key, entries[i].key) == 0) 
    	{
      		entries[i].count = 0;
      		if (entries[i].values)
				free(entries[i].values);
      		entries[i].values = 0;
    	}
  	}
}

void XWCFFDict::set(const char *key, int idx, double value)
{
	for (int i = 0 ; i < count; i++) 
	{
    	if (strcmp(key, entries[i].key) == 0) 
    	{
      		if (entries[i].count > idx)
				entries[i].values[idx] = value;
      		break;
    	}
  	}
}

void XWCFFDict::update(XWFontFileCFF * fileA)
{
	for (int i = 0; i < count; i++) 
	{
    	if (entries[i].count > 0) 
    	{
      		int id = entries[i].id;
      		if (dict_operator[id].argtype == CFF_TYPE_SID) 
      		{
				char * str = fileA->getString(entries[i].values[0]);
				entries[i].values[0] = fileA->addString(str, 1);
				delete [] str;
      		} 
      		else if (dict_operator[id].argtype == CFF_TYPE_ROS) 
      		{
				char * str = fileA->getString(entries[i].values[0]);
				entries[i].values[0] = fileA->addString(str, 1);
				delete [] str;
				str = fileA->getString(entries[i].values[1]);
				entries[i].values[1] = fileA->addString(str, 1);
				delete [] str;
      		}
    	}
  	}
}

void XWCFFDict::add(uchar **dataA, 
	                uchar *endptr, 
	                int *status)
{
	int id = **dataA;
  	if (id == 0x0c) 
  	{
    	*dataA += 1;
    	if (*dataA >= endptr || (id = **dataA + CFF_LAST_DICT_OP1) >= CFF_LAST_DICT_OP) 
    	{
      		*status = CFF_ERROR_PARSE_ERROR;
      		return;
    	}
  	} 
  	else if (id >= CFF_LAST_DICT_OP1) 
  	{
    	*status = CFF_ERROR_PARSE_ERROR;
    	return;
  	}

  	int argtype = dict_operator[id].argtype;
  	if (dict_operator[id].opname == NULL || argtype < 0) 
  	{
    	*status = CFF_ERROR_PARSE_ERROR;
    	return;
  	} 
  	else if (stack_top < 1) 
  	{
    	*status = CFF_ERROR_STACK_UNDERFLOW;
    	return;
  	}

  	if (count >= max) 
  	{
    	max += DICT_ENTRY_MAX;
    	entries = (CFFDictEntry*)realloc(entries, max * sizeof(CFFDictEntry));
  	}

  	entries[count].id = id;
  	entries[count].key = (char *) dict_operator[id].opname;
  	if (argtype == CFF_TYPE_NUMBER || 
      	argtype == CFF_TYPE_BOOLEAN ||
      	argtype == CFF_TYPE_SID ||
      	argtype == CFF_TYPE_OFFSET) 
    {
    	stack_top--;
    	entries[count].count  = 1;
    	entries[count].values = (double*)malloc(sizeof(double));
    	entries[count].values[0] = arg_stack[stack_top];
  	} 
  	else 
  	{
    	entries[count].count  = stack_top;
    	entries[count].values = (double*)malloc(stack_top * sizeof(double));
    	while (stack_top > 0) 
    	{
      		stack_top--;
      		entries[count].values[stack_top] = arg_stack[stack_top];
    	}
  	}

  	count += 1;
  	*dataA += 1;
}

double XWCFFDict::getInteger(uchar **dataA, 
	                         uchar *endptr, 
	                         int *status)
{
	long result = 0;
	
  	uchar b0 = *(*dataA)++;
  	if (b0 == 28 && *dataA < endptr - 2)
  	{
  		uchar b1 = *(*dataA)++;
    	uchar b2 = *(*dataA)++;
    	result = b1*256+b2;
    	if (result > 0x7fffL)
      		result -= 0x10000L;
  	}
  	else if (b0 == 29 && *dataA < endptr - 4)
  	{
  		result = *(*dataA)++;
    	if (result > 0x7f)
      		result -= 0x100;
    	for (int i = 0; i < 3; i++) 
    	{
      		result = result*256+(**dataA);
      		*dataA += 1;
    	}
  	}
  	else if (b0 >= 32 && b0 <= 246)
  		result = b0 - 139;
  	else if (b0 >= 247 && b0 <= 250)
  	{
  		uchar b1 = *(*dataA)++;
    	result = (b0-247)*256+b1+108;
  	}
  	else if (b0 >= 251 && b0 <= 254)
  	{
  		uchar b1 = *(*dataA)++;
    	result = -(b0-251)*256-b1-108;
  	}
  	else
  		*status = CFF_ERROR_PARSE_ERROR;
  		
  	return (double) result;
}

double XWCFFDict::getReal(uchar **dataA, 
	                      uchar *endptr, 
	                      int *status)
{
	double result = 0.0;
	
	if (**dataA != 30 || *dataA >= endptr -1) 
	{
    	*status = CFF_ERROR_PARSE_ERROR;
    	return 0.0;
  	}
  	
  	*dataA += 1;
  	
  	int nibble = 0, pos = 0;
  	int len = 0, fail = 0;
  	
  	char buf[1024];
  	while ((! fail) && len < 1022 && *dataA < endptr)
  	{
  		if (pos % 2) 
  		{
      		nibble = **dataA & 0x0f;
      		*dataA += 1;
    	} 
    	else 
      		nibble = (**dataA >> 4) & 0x0f;
      		
      	if (nibble >= 0x00 && nibble <= 0x09) 
      		buf[len++] = nibble + '0';
    	else if (nibble == 0x0a) 
      		buf[len++] = '.';
    	else if (nibble == 0x0b || nibble == 0x0c) 
    	{ /* E, E- */
      		buf[len++] = 'e';
      		if (nibble == 0x0c)
				buf[len++] = '-';
    	} 
    	else if (nibble == 0x0e) 
      		buf[len++] = '-';
    	else if (nibble == 0x0d) 
    		fail = 0;
    	else if (nibble == 0x0f) 
    	{ /* end */
      		buf[len++] = '\0';
      		if (((pos % 2) == 0) && (**dataA != 0xff)) 
				fail = 1;
      		break;
    	} 
    	else 
      		fail = 1;
    	pos++;
  	}
  	
  	if (fail || nibble != 0x0f) 
    	*status = CFF_ERROR_PARSE_ERROR;
  	else 
  	{
    	char *s = 0;
    	result = strtod(buf, &s);
    	if (*s != 0) 
      		*status = CFF_ERROR_PARSE_ERROR;
  	}

  	return result;
}

long XWCFFDict::packInteger(uchar *dest, 
	                        long destlen, 
	                        long value)
{
	long len = 0;
	if (value >= -107 && value <= 107) 
	{
    	if (destlen < 1)
      		return -1;
    	dest[0] = (value + 139) & 0xff;
    	len = 1;
  	} 
  	else if (value >= 108 && value <= 1131) 
  	{
    	if (destlen < 2)
      		return -1;
    	value = 0xf700u + value - 108;
    	dest[0] = (value >> 8) & 0xff;
    	dest[1] = value & 0xff;
    	len = 2;
  	} 
  	else if (value >= -1131 && value <= -108) 
  	{
    	if (destlen < 2)
      		return -1;
    	value = 0xfb00u - value - 108;
    	dest[0] = (value >> 8) & 0xff;
    	dest[1] = value & 0xff;
    	len = 2;
  	} 
  	else if (value >= -32768 && value <= 32767) 
  	{ /* shortint */
    	if (destlen < 3)
      		return -1;
    	dest[0] = 28;
    	dest[1] = (value >> 8) & 0xff;
    	dest[2] = value & 0xff;
    	len = 3;
  	} 
  	else 
  	{ /* longint */
    	if (destlen < 5)
      		return -1;
    	dest[0] = 29;
    	dest[1] = (value >> 24) & 0xff;
    	dest[2] = (value >> 16) & 0xff;
    	dest[3] = (value >> 8) & 0xff;
    	dest[4] = value & 0xff;
    	len = 5;
  	}

  	return len;
}

long XWCFFDict::packReal(uchar *dest, 
	                     long destlen, 
	                     double value)
{
	if (destlen < 2)
    	return -1;
    	
    dest[0] = 30;

  	if (value == 0.0) 
  	{
    	dest[1] = 0x0f;
    	return 2;
  	}
  	
  	int pos = 2;
  	if (value < 0.0) 
  	{
    	dest[1] = 0xe0;
    	value *= -1.0;
    	pos++;
  	}
  	
  	char buffer[32];
  	
  	sprintf(buffer, "%.13g", value);

  	for (int i = 0; buffer[i] != '\0'; i++) 
  	{
    	uchar ch = 0;
    	if (buffer[i] == '.') 
      		ch = 0x0a;
    	else if (buffer[i] >= '0' && buffer[i] <= '9') 
      		ch = buffer[i] - '0';
    	else if (buffer[i] == 'e') 
      		ch = (buffer[++i] == '-' ? 0x0c : 0x0b);
    	else 
      		return -1;

    	if (destlen < pos/2 + 1)
      		return -1;

    	if (pos % 2) 
      		dest[pos/2] += ch;
    	else 
      		dest[pos/2] = (ch << 4);
    	pos++;
  	}

  	if (pos % 2) 
  	{
    	dest[pos/2] += 0x0f;
    	pos++;
  	} 
  	else 
  	{
    	if (destlen < pos/2 + 1)
      		return -1;
    	dest[pos/2] = 0xff;
    	pos += 2;
  	}

  	return pos/2;
}

long XWCFFDict::putEntry(XWCFFDict::CFFDictEntry *de,
				         uchar *dest, 
				         long destlen)
{
	long len = 0;
	if (de->count > 0) 
	{
    	int id = de->id;
    	int type = CFF_TYPE_NUMBER;
    	if (dict_operator[id].argtype == CFF_TYPE_OFFSET || 
			dict_operator[id].argtype == CFF_TYPE_SZOFF) 
		{
      		type = CFF_TYPE_OFFSET;
    	}
    	
    	for (int i = 0; i < de->count; i++) 
    	{
      		len += putNumber(de->values[i],
				 			 dest + len,
				 			 destlen-len, type);
    	}
    	
    	if (id >= 0 && id < CFF_LAST_DICT_OP1) 
    	{
      		if (len + 1 > destlen)
				return -1;
      		dest[len++] = id;
    	} 
    	else if (id >= 0 && id < CFF_LAST_DICT_OP) 
    	{
      		if (len + 2 > destlen)
				return -1;
      		dest[len++] = 12;
      		dest[len++] = id - CFF_LAST_DICT_OP1;
    	} 
    	else 
      		return -1;
  	}

  	return len;
}

long XWCFFDict::putNumber(double value,
				          uchar *dest, 
				          long destlen,
				          int type)
{
	double nearint = floor(value+0.5);
	long   len = 0;
	if (type == CFF_TYPE_OFFSET) 
	{
    	long lvalue = (long) value;
    	if (destlen < 5)
      		return -1;
    	dest[0] = 29;
    	dest[1] = (lvalue >> 24) & 0xff;
    	dest[2] = (lvalue >> 16) & 0xff;
    	dest[3] = (lvalue >>  8) & 0xff;
    	dest[4] = lvalue         & 0xff;
    	len = 5;
  	} 
  	else if (value > CFF_INT_MAX || 
  		     value < CFF_INT_MIN || 
  		     (fabs(value - nearint) > 1.0e-5)) 
  	{ /* real */
    	len = packReal(dest, destlen, value);
  	} 
  	else 
    	len = packInteger(dest, destlen, (long) nearint);

  	return len;
}


XWCFFEncoding::XWCFFEncoding()
{
	format = 0;
	num_entries = 0;
	data.range1 = 0;
	num_supps = 0;
	supp = 0;
}

XWCFFEncoding::~XWCFFEncoding()
{
	switch (format & (~0x80))
	{
		case 0:
      		if (data.codes)
				free(data.codes);
      		break;
      		
      	case 1:
      		if (data.range1)
				free(data.range1);
      		break;
      		
      	default:
      		break;
	}
	
	if (format & 0x80)
	{
		if (supp)
			free(supp);
	}
}

ushort XWCFFEncoding::lookup(XWFontFileCFF * fileA, uchar code)
{
	ushort gid = 0;
	ushort i = 0;
  	switch (format & (~0x80)) 
  	{
  		case 0:
    		for (; i < num_entries; i++) 
    		{
      			if (code == data.codes[i]) 
      			{
					gid = i + 1;
					break;
      			}
    		}
    		break;
    		
  		case 1:
    		for (; i < num_entries; i++) 
    		{
      			if (code >= data.range1[i].first && 
	  				code <= data.range1[i].first + data.range1[i].n_left) 
	  			{
        			gid += code - data.range1[i].first + 1;
        			break;
      			}
      			gid += data.range1[i].n_left + 1;
    		}
    		if (i == num_entries)
      			gid = 0;
    		break;
    		
  		default:
    		return 0;
  	}

  	if (gid == 0 && (format & 0x80)) 
  	{
    	if (!supp)
      		return 0;
    	
    	for (int i = 0; i < num_supps; i++) 
    	{
      		if (code == supp[i].code) 
      		{
				gid = fileA->lookupChartsets(supp[i].glyph);
				break;
      		}
    	}
  	}

  	return gid;
}

long XWCFFEncoding::pack(uchar *dest, long destlen)
{
	long len = 0;
	dest[len++] = format;
  	dest[len++] = num_entries;
  	switch (format & (~0x80)) 
  	{
  		case 0:
    		if (destlen < len + num_entries)
      			return -1;
    		for (int i = 0; i < num_entries; i++) 
      			dest[len++] = data.codes[i];
    		break;
    		
  		case 1:
    		{
      			if (destlen < len + num_entries * 2)
					return -1;
      			for (int i = 0; i < num_entries; i++) 
      			{
					dest[len++] = data.range1[i].first & 0xff;
					dest[len++] = data.range1[i].n_left;
      			}
    		}
    		break;
    		
  		default:
    		return -1;
    		break;
  	}

  	if (format & 0x80) 
  	{
    	if (destlen < len + num_supps*3 + 1)
      		return -1;
      		
    	dest[len++] = num_supps;
    	for (int i = 0; i< num_supps; i++) 
    	{
      		dest[len++] = supp[i].code;
      		dest[len++] = (supp[i].glyph >> 8) & 0xff;
      		dest[len++] = supp[i].glyph & 0xff;
    	}
  	}

  	return len;
}

long XWCFFEncoding::read(XWFontFileCFF * fileA)
{
	long length = 1;
	format = fileA->getCard8();
	switch (format & (~0x80))
	{
		case 0:
    		num_entries = fileA->getCard8();
    		data.codes = (uchar*)malloc(num_entries * sizeof(uchar));
    		for (int i = 0; i < num_entries; i++) 
      			data.codes[i] = fileA->getCard8();
      		length += num_entries + 1;
    		break;
    		
    	case 1:
    		{
      			num_entries = fileA->getCard8();
      			data.range1 = (CFFRange1*)malloc(num_entries * sizeof(CFFRange1));
      			CFFRange1* range = data.range1;
      			for (int i = 0; i < num_entries; i++) 
      			{
					range[i].first = fileA->getCard8();
					range[i].n_left = fileA->getCard8();
      			}
      			
      			length += num_entries * 2 + 1;
    		}
    		break;
    		
  		default:
    		break;
	}
	
	if (format & 0x80) 
	{
    	num_supps = fileA->getCard8();
    	supp = (CFFMap*)malloc(num_supps * sizeof(CFFMap));
    	for (int i = 0;i < num_supps; i++) 
    	{
      		supp[i].code = fileA->getCard8();
      		supp[i].glyph = fileA->getCard16();
    	}
    	
    	length += num_supps * 3 + 1;
  	} 
  	
  	return length;
}

XWCFFCharsets::XWCFFCharsets()
{
	format = 0;
	num_entries = 0;
	data.range2 = 0;
}

XWCFFCharsets::~XWCFFCharsets()
{
	switch (format)
	{
		case 0:
      		if (data.glyphs)
				free(data.glyphs);
      		break;
      		
    	case 1:
      		if (data.range1)
				free(data.range1);
      		break;
      		
    	case 2:
      		if (data.range2)
				free(data.range2);
      		break;
      		
    	default:
      		break;
	}
}

ushort XWCFFCharsets::lookup(ushort cid)
{
	ushort gid = 0;
  	switch (format) 
  	{
  		case 0:
    		for (ushort i = 0; i <num_entries; i++) 
    		{
      			if (cid == data.glyphs[i]) 
      			{
					gid = i + 1;
					return gid;
      			}
    		}
    		break;
    		
  		case 1:
    		for (ushort i = 0; i < num_entries; i++) 
    		{
      			if (cid >= data.range1[i].first &&
	  				cid <= data.range1[i].first + data.range1[i].n_left) 
	  			{
        			gid += cid - data.range1[i].first + 1;
					return gid;
      			}
      			gid += data.range1[i].n_left + 1;
    		}
    		break;
    		
  		case 2:
    		for (ushort i = 0; i < num_entries; i++) 
    		{
      			if (cid >= data.range2[i].first && 
	  				cid <= data.range2[i].first + data.range2[i].n_left) 
	  			{
        			gid += cid - data.range2[i].first + 1;
					return gid;
      			}
      			gid += data.range2[i].n_left + 1;
    		}
    		break;
    		
  		default:
  			break;
  	}

  	return 0;
}

ushort XWCFFCharsets::lookup(XWFontFileCFF * fileA, const char *glyph)
{
	ushort gid = 0;
  	switch (format) 
  	{
  		case 0:
    		for (ushort i = 0; i < num_entries; i++) 
    		{
      			gid++;
      			if (fileA->matchString(glyph, data.glyphs[i]))
					return gid;
    		}
    		break;
    		
  		case 1:
    		for (ushort i = 0; i < num_entries; i++) 
    		{
      			for (ushort n = 0; n <= data.range1[i].n_left; n++) 
      			{
					gid++;
					if (fileA->matchString(glyph, (ushort)(data.range1[i].first + n))) 
	  					return gid;
      			}
    		}
    		break;
    		
  		case 2:
    		for (ushort i = 0; i < num_entries; i++) 
    		{
      			for (ushort n = 0; n <= data.range2[i].n_left; n++) 
      			{
					gid++;
					if (fileA->matchString(glyph, (ushort)(data.range2[i].first + n))) 
	  					return gid;
      			}
    		}
    		break;
    		
  		default:
    		break;
  	}

  	return 0;
}

ushort XWCFFCharsets::lookupInverse(ushort gid)
{
	ushort sid = 0;
	ushort i = 0;
  	switch (format) 
  	{
  		case 0:
    		if (gid - 1 >= num_entries)
      			return 0;
    		sid = data.glyphs[gid - 1];
    		break;
    		
  		case 1:
    		for (; i < num_entries; i++) 
    		{
      			if (gid <= data.range1[i].n_left + 1) 
      			{
					sid = gid + data.range1[i].first - 1;
        			break;
      			}
      			gid -= data.range1[i].n_left + 1;
    		}
    		if (i == num_entries)
      			return 0;
    		break;
    		
  		case 2:
    		for (; i < num_entries; i++) 
    		{
      			if (gid <= data.range2[i].n_left + 1) 
      			{
					sid = gid + data.range2[i].first - 1;
        			break;
      			}
      			gid -= data.range2[i].n_left + 1;
    		}
    		if (i == num_entries)
      			return 0;
    		break;
    		
  		default:
    		return 0;
    		break;
  	}

  	return sid;
}

long XWCFFCharsets::pack(uchar *dest, long destlen)
{
	long len = 0;
	dest[len++] = format;
  	switch (format) 
  	{
  		case 0:
    		if (destlen < len + num_entries * 2)
      			return -1;
    		for (ushort i = 0; i < num_entries; i++) 
    		{
      			ushort sid = data.glyphs[i]; /* or CID */
      			dest[len++] = (sid >> 8) & 0xff;
      			dest[len++] = sid & 0xff;
    		}
    		break;
    		
  		case 1:
    		{
      			if (destlen < len + num_entries*3)
					return -1;
      			for (int i = 0; i < num_entries; i++) 
      			{
					dest[len++] = (data.range1[i].first >> 8) & 0xff;
					dest[len++] = data.range1[i].first & 0xff;
					dest[len++] = data.range1[i].n_left;
      			}
    		}
    		break;
    		
  		case 2:
    		{
      			if (destlen < len + num_entries*4)
					return -1;
      			for (int i = 0;i < num_entries; i++) 
      			{
					dest[len++] = (data.range2[i].first >> 8) & 0xff;
					dest[len++] = data.range2[i].first & 0xff;
					dest[len++] = (data.range2[i].n_left >> 8) & 0xff;
					dest[len++] = data.range2[i].n_left & 0xff;
      			}
    		}
    		break;
    		
  		default:
    		return -1;
    		break;
  	}

  	return len;
}

long XWCFFCharsets::read(XWFontFileCFF * fileA)
{
	format = fileA->getCard8();	
	ushort countA = fileA->getNumGlyphs() - 1;
	long length = 1;
	switch (format)
	{
		case 0:
    		num_entries = countA; /* no .notdef */
    		data.glyphs = (ushort*)malloc(num_entries * sizeof(ushort));
    		length += num_entries * 2;
    		for (ushort i = 0; i < num_entries; i++) 
      			data.glyphs[i] = fileA->getCard16();
    		countA = 0;
    		break;
    		
    	case 1:
    		{
    			CFFRange1 * ranges = 0;
      			while (countA > 0 && num_entries < fileA->getNumGlyphs()) 
      			{
					ranges = (CFFRange1*)realloc(ranges, (num_entries + 1) * sizeof(CFFRange1));
					ranges[num_entries].first = fileA->getCard16();
					ranges[num_entries].n_left = fileA->getCard8();
					countA -= ranges[num_entries].n_left + 1; /* no-overrap */
					num_entries += 1;
					data.range1 = ranges;
      			}
      			
      			length += num_entries * 3;
    		}
    		break;
    	
  		case 2:
    		{
      			CFFRange2 *ranges = NULL;
      			while (countA > 0 && num_entries < fileA->getNumGlyphs()) 
      			{
					ranges = (CFFRange2*)realloc(ranges, (num_entries + 1) * sizeof(CFFRange2));
					ranges[num_entries].first = fileA->getCard16();
					ranges[num_entries].n_left = fileA->getCard16();
					countA -= ranges[num_entries].n_left + 1;
					num_entries += 1;
      			}
      			data.range2 = ranges;
      			length += num_entries * 4;
    		}
    		break;
    		
  		default:
    		break;
	}
	
	return length;
}

XWCFFFDSelect::XWCFFFDSelect()
{
	format = 0;
	num_entries = 0;
	data.ranges = 0;
}

XWCFFFDSelect::~XWCFFFDSelect()
{
	if (format == 0)
	{
		if (data.fds)
			free(data.fds);
	}
	else if (format == 3)
	{
		if (data.ranges)
			free(data.ranges);
	}
}

uchar XWCFFFDSelect::lookup(ushort gid)
{
	uchar fd = 0xff;
	switch (format) 
	{
  		case 0:
    		fd = data.fds[gid];
    		break;
    		
  		case 3:
    		{
      			if (gid == 0) 
					fd = data.ranges[0].fd;
      			else 
      			{
					ushort i = 1;
					for (; i < num_entries; i++) 
					{
	  					if (gid < data.ranges[i].first)
	    					break;
					}
					fd = data.ranges[i-1].fd;
      			}
    		}
    		break;
    		
  		default:
    		break;
  	}

  	return fd;
}

long XWCFFFDSelect::pack(XWFontFileCFF * fileA, uchar *dest, long destlen)
{
	long len = 0;
	dest[len++] = format;
  	switch (format) 
  	{
  		case 0:
    		if (num_entries != fileA->getNumGlyphs())
      			return -1;
    		if (destlen < len + num_entries)
      			return -1;
      			
    		for (ushort i = 0; i < num_entries;i++) 
      			dest[len++] = data.fds[i];
    		break;
    		
  		case 3:
    		{
      			if (destlen < len + 2)
					return -1;
      			len += 2;
      			for (ushort i = 0; i < num_entries; i++) 
      			{
					if (destlen < len + 3)
	  					return -1;
					dest[len++] = (data.ranges[i].first >> 8) & 0xff;
					dest[len++] = data.ranges[i].first & 0xff;
					dest[len++] = data.ranges[i].fd;
      			}
      			if (destlen < len + 2)
					return -1;
      			dest[len++]  = (fileA->getNumGlyphs() >> 8) & 0xff;
      			dest[len++]  = fileA->getNumGlyphs() & 0xff;
      			dest[1] = ((len/3 - 1) >> 8) & 0xff;
      			dest[2] = (len/3 - 1) & 0xff;
    		}
    		break;
    		
  		default:
    		break;
  	}

  	return len;
}

long XWCFFFDSelect::read(XWFontFileCFF * fileA)
{
	format = fileA->getCard8();
	long length = 1;
	switch (format)
	{
		case 0:
    		num_entries = fileA->getNumGlyphs();
    		data.fds = (uchar*)malloc(num_entries * sizeof(uchar));
    		for (ushort i = 0; i < num_entries; i++) 
      			data.fds[i] = fileA->getCard8();
      		length += num_entries;
    		break;
    		
  		case 3:
    		{
      			CFFRange3 *ranges = 0;
      			num_entries = fileA->getCard16();
      			data.ranges = ranges = (CFFRange3*)malloc(num_entries * sizeof(CFFRange3));
      			for (ushort i = 0; i < num_entries; i++) 
      			{
					ranges[i].first = fileA->getCard16();
					ranges[i].fd = fileA->getCard8();
      			}
      			if (ranges[0].first != 0)
					return -1;
					
      			if (fileA->getNumGlyphs() != fileA->getCard16())
					return -1;
					
				length += num_entries * 3 + 4;
    		}
    		break;
    		
  		default:
    		break;
	}
	
	return length;
}

#define CS_BUFFER_ERROR -3
#define CS_STACK_ERROR  -2
#define CS_PARSE_ERROR  -1
#define CS_PARSE_OK      0
#define CS_PARSE_END     1
#define CS_SUBR_RETURN   2
#define CS_CHAR_END      3

static int cs_status = CS_PARSE_ERROR;

#define DST_NEED(a,b) {if ((a) < (b)) { cs_status = CS_BUFFER_ERROR ; return ; }}
#define SRC_NEED(a,b) {if ((a) < (b)) { cs_status = CS_PARSE_ERROR  ; return ; }}
#define NEED(a,b)     {if ((a) < (b)) { cs_status = CS_STACK_ERROR  ; return ; }}
	
static int cs_num_stems = 0;
static int cs_phase     = 0;

static int cs_nest      = 0;

static int    cs_have_width = 0;
static double cs_width      = 0.0;

static int    cs_stack_top = 0;
static double cs_arg_stack[CS_ARG_STACK_MAX];
static double cs_trn_array[CS_TRANS_ARRAY_MAX];

#define cs_hstem      1
/*      RESERVED      2 */
#define cs_vstem      3
#define cs_vmoveto    4
#define cs_rlineto    5
#define cs_hlineto    6
#define cs_vlineto    7
#define cs_rrcurveto  8
/*      cs_closepath  9  : TYPE1 */
#define cs_callsubr   10
#define cs_return     11
#define cs_escape     12
/*      cs_hsbw       13 : TYPE1 */
#define cs_endchar    14
/*      RESERVED      15 */
/*      RESERVED      16 */
/*      RESERVED      17 */
#define cs_hstemhm    18
#define cs_hintmask   19
#define cs_cntrmask   20
#define cs_rmoveto    21
#define cs_hmoveto    22
#define cs_vstemhm    23
#define cs_rcurveline 24
#define cs_rlinecurve 25
#define cs_vvcurveto  26
#define cs_hhcurveto  27
/*      SHORTINT      28 : first byte of shortint*/
#define cs_callgsubr  29
#define cs_vhcurveto  30
#define cs_hvcurveto  31


#define cs_dotsection 0
/*      cs_vstem3     1 : TYPE1 */
/*      cs_hstem3     2 : TYPE1 */
#define cs_and        3
#define cs_or         4
#define cs_not        5
/*      cs_seac       6 : TYPE1 */
/*      cs_sbw        7 : TYPE1 */
/*      RESERVED      8  */
#define cs_abs        9
#define cs_add        10
#define cs_sub        11
#define cs_div        12
/*      RESERVED      13 */
#define cs_neg        14
#define cs_eq         15
/*      cs_callothersubr 16 : TYPE1 */
/*      cs_pop           17 : TYPE1 */
#define cs_drop       18
/*      RESERVED      19 */
#define cs_put        20
#define cs_get        21
#define cs_ifelse     22 
#define cs_random     23
#define cs_mul        24
/*      RESERVED      25 */
#define cs_sqrt       26
#define cs_dup        27
#define cs_exch       28
#define cs_index      29
#define cs_roll       30
/*      cs_setcurrentpoint 31 : TYPE1 */
/*      RESERVED      32 */
/*      RESERVED      33 */
#define cs_hflex      34
#define cs_flex       35
#define cs_hflex1     36
#define cs_flex1      37

XWCSGInfo::XWCSGInfo()
{
	init();
}

long XWCSGInfo::copyCharString (uchar *dst, 
	                            long dstlen,
		    			        uchar *src, 
		    			        long srclen,
		                        XWCFFIndex *gsubr, 
		                        XWCFFIndex *subr,
		    			        double default_width, 
		    			        double nominal_width)
{
	uchar *save = dst;
	init();
	
	doCharString(&dst, dst + dstlen, &src, src + srclen, gsubr, subr);

    flags = 0; /* not used */
    if (cs_have_width) 
    	wx = nominal_width + cs_width;
    else 
    	wx = default_width;

  	return (long)(dst - save);
}

void XWCSGInfo::clearStack(uchar **dest, uchar *limit)
{
	for (int i = 0; i < cs_stack_top; i++)
	{
		double value  = cs_arg_stack[i];
    	long ivalue = (long) floor(value+0.5);
    	if (value >= 0x8000L || value <= (-0x8000L - 1))
    		return ;
    	else if (fabs(value - ivalue) > 3.0e-5)
    	{
    		DST_NEED(limit, *dest + 5);
      		*(*dest)++ = 255;
      		ivalue = (long) floor(value); /* mantissa */
      		*(*dest)++ = (ivalue >> 8) & 0xff;
      		*(*dest)++ = ivalue & 0xff;
      		ivalue = (long)((value - ivalue) * 0x10000l); /* fraction */
      		*(*dest)++ = (ivalue >> 8) & 0xff;
      		*(*dest)++ = ivalue & 0xff;
    	}
    	else if (ivalue >= -107 && ivalue <= 107)
    	{
    		DST_NEED(limit, *dest + 1);
      		*(*dest)++ = ivalue + 139;
    	}
    	else if (ivalue >= 108 && ivalue <= 1131) 
    	{
      		DST_NEED(limit, *dest + 2);
      		ivalue = 0xf700u + ivalue - 108;
      		*(*dest)++ = (ivalue >> 8) & 0xff;
      		*(*dest)++ = ivalue & 0xff;
    	} 
    	else if (ivalue >= -1131 && ivalue <= -108) 
    	{
      		DST_NEED(limit, *dest + 2);
      		ivalue = 0xfb00u - ivalue - 108;
      		*(*dest)++ = (ivalue >> 8) & 0xff;
      		*(*dest)++ = ivalue & 0xff;
    	} 
    	else if (ivalue >= -32768 && ivalue <= 32767) 
    	{ /* shortint */
      		DST_NEED(limit, *dest + 3);
      		*(*dest)++ = 28;
      		*(*dest)++ = (ivalue >> 8) & 0xff;
      		*(*dest)++ = (ivalue) & 0xff;
    	} 
    	else 
    		return ;
	}
	
	cs_stack_top = 0;
}

void XWCSGInfo::doCharString(uchar **dest, 
	                         uchar *limit,
	                         uchar **data, 
	                         uchar *endptr,
	                         XWCFFIndex *gsubr_idx, 
	                         XWCFFIndex *subr_idx)
{
	if (cs_nest > CS_SUBR_NEST_MAX)
		return ;
		
	cs_nest++;
	uchar * subr = 0;
	long  len = 0;
  	while (*data < endptr && cs_status == CS_PARSE_OK)
  	{
  		uchar b0 = **data;
  		if (b0 == 255)
  			getFixed(data, endptr);
  		else if (b0 == cs_return)
  			cs_status = CS_SUBR_RETURN;
  		else if (b0 == cs_callgsubr)
  		{
  			if (cs_stack_top < 1) 
				cs_status = CS_STACK_ERROR;
      		else 
      		{
				cs_stack_top--;
				getSubr(&subr, &len, gsubr_idx, (long) cs_arg_stack[cs_stack_top]);
				if (*dest + len > limit)
					return ;
				doCharString(dest, limit, &subr, subr + len, gsubr_idx, subr_idx);
				*data += 1;
      		}
  		}
  		else if (b0 == cs_callsubr)
  		{
  			if (cs_stack_top < 1) 
				cs_status = CS_STACK_ERROR;
      		else 
      		{
				cs_stack_top--;
				getSubr(&subr, &len, subr_idx, (long) cs_arg_stack[cs_stack_top]);
				if (limit < *dest + len)
					return ;
				doCharString(dest, limit, &subr, subr + len, gsubr_idx, subr_idx);
				*data += 1;
      		}
  		}
  		else if (b0 == cs_escape)
  			doOperator2(dest, limit, data, endptr);
  		else if (b0 < 32 && b0 != 28)
  			doOperator1(dest, limit, data, endptr);
  		else if ((b0 <= 22 && b0 >= 27) || b0 == 31)
  			cs_status = CS_PARSE_ERROR;
  		else
  			getInteger(data, endptr);
  	}
  	
  	if (cs_status == CS_SUBR_RETURN)
  		cs_status = CS_PARSE_OK;
  	else if (cs_status < CS_PARSE_OK)
  		return ;
  		
  	cs_nest--;
}

void XWCSGInfo::doOperator1(uchar **dest, 
	                        uchar *limit, 
	                        uchar **data, 
	                        uchar *endptr)
{
	uchar op = **data;
  	*data += 1;
  	switch (op)
  	{
  		case cs_hstemhm:
  		case cs_vstemhm:
  		case cs_hstem:
  		case cs_vstem:
    		if (cs_phase == 0 && (cs_stack_top % 2)) 
    		{
      			cs_have_width = 1;
      			cs_width = cs_arg_stack[0];
    		}
    		cs_num_stems += cs_stack_top/2;
    		clearStack(dest, limit);
    		DST_NEED(limit, *dest + 1);
    		*(*dest)++ = op;
    		cs_phase = 1;
    		break;
    		
    	case cs_hintmask:
  		case cs_cntrmask:
    		if (cs_phase < 2) 
    		{
      			if (cs_phase == 0 && (cs_stack_top % 2)) 
      			{
					cs_have_width = 1;
					cs_width = cs_arg_stack[0];
      			}
      			cs_num_stems += cs_stack_top/2;
    		}
    		clearStack(dest, limit);
    		DST_NEED(limit, *dest + 1);
    		*(*dest)++ = op;
    		if (cs_num_stems > 0) 
    		{
      			int masklen = (cs_num_stems + 7) / 8;
      			DST_NEED(limit, *dest + masklen);
      			SRC_NEED(endptr, *data + masklen);
      			memmove(*dest, *data, masklen);
      			*data += masklen;
      			*dest += masklen;
    		}
    		cs_phase = 2;
    		break;
    		
    	case cs_rmoveto:
    		if (cs_phase == 0 && (cs_stack_top % 2)) 
    		{
      			cs_have_width = 1;
      			cs_width = cs_arg_stack[0];
    		}
    		clearStack(dest, limit);
    		DST_NEED(limit, *dest + 1);
    		*(*dest)++ = op;
    		cs_phase = 2;
    		break;
    		
    	case cs_hmoveto:
  		case cs_vmoveto:
    		if (cs_phase == 0 && (cs_stack_top % 2) == 0) 
    		{
      			cs_have_width = 1;
      			cs_width = cs_arg_stack[0];
    		}
    		clearStack(dest, limit);
    		DST_NEED(limit, *dest + 1);
    		*(*dest)++ = op;
    		cs_phase = 2;
    		break;
    		
    	case cs_endchar:
    		if (cs_stack_top == 1) 
    		{
      			cs_have_width = 1;
      			cs_width = cs_arg_stack[0];
      			clearStack(dest, limit);
    		} 
    		else if (cs_stack_top == 4 || cs_stack_top == 5) 
    		{
      			cs_status = CS_PARSE_ERROR;
      			return;
    		} 
    		DST_NEED(limit, *dest + 1);
    		*(*dest)++ = op;
    		cs_status = CS_CHAR_END;
    		break;
    		
    	case cs_rlineto:
  		case cs_hlineto:
  		case cs_vlineto:
  		case cs_rrcurveto:
  		case cs_rcurveline:
  		case cs_rlinecurve:
  		case cs_vvcurveto:
  		case cs_hhcurveto:
  		case cs_vhcurveto:
  		case cs_hvcurveto:
    		if (cs_phase < 2) 
    		{
      			cs_status = CS_PARSE_ERROR;
      			return;
    		}
    		clearStack(dest, limit);
    		DST_NEED(limit, *dest + 1);
    		*(*dest)++ = op;
    		break;
    		
    	case cs_return:
  		case cs_callgsubr:
  		case cs_callsubr:
    		break;
    		
  		default:
    		cs_status = CS_PARSE_ERROR;
    		break;
  	}
}

void XWCSGInfo::doOperator2(uchar **dest, 
	                        uchar *limit, 
	                        uchar **data, 
	                        uchar *endptr)
{
	uchar op;
  	*data += 1;
  	SRC_NEED(endptr, *data + 1);
  	op = **data;
  	*data += 1;

  	switch(op)
  	{
  		case cs_dotsection:
    		cs_status = CS_PARSE_ERROR;
    		return;
    		break;
    		
  		case cs_hflex:
  		case cs_flex:
  		case cs_hflex1:
  		case cs_flex1:
    		if (cs_phase < 2) 
    		{
      			cs_status = CS_PARSE_ERROR;
      			return;
    		}
    		clearStack(dest, limit);
    		DST_NEED(limit, *dest + 2);
    		*(*dest)++ = cs_escape;
    		*(*dest)++ = op;
    		break;
    		
    	case cs_and:
    		NEED(cs_stack_top, 2);
    		cs_stack_top--;
    		if (cs_arg_stack[cs_stack_top] && cs_arg_stack[cs_stack_top-1]) 
      			cs_arg_stack[cs_stack_top-1] = 1.0;
    		else 
      			cs_arg_stack[cs_stack_top-1] = 0.0;
    		break;
    		
  		case cs_or:
    		NEED(cs_stack_top, 2);
    		cs_stack_top--;
    		if (cs_arg_stack[cs_stack_top] || cs_arg_stack[cs_stack_top-1]) 
      			cs_arg_stack[cs_stack_top-1] = 1.0;
    		else 
      			cs_arg_stack[cs_stack_top-1] = 0.0;
    		break;
    		
  		case cs_not:
    		NEED(cs_stack_top, 1);
    		if (cs_arg_stack[cs_stack_top-1]) 
      			cs_arg_stack[cs_stack_top-1] = 0.0;
    		else 
      			cs_arg_stack[cs_stack_top-1] = 1.0;
    		break;
    		
  		case cs_abs:
    		NEED(cs_stack_top, 1);
    		cs_arg_stack[cs_stack_top-1] = fabs(cs_arg_stack[cs_stack_top-1]);
    		break;
    		
  		case cs_add:
    		NEED(cs_stack_top, 2);
    		cs_arg_stack[cs_stack_top-2] += cs_arg_stack[cs_stack_top-1];
    		cs_stack_top--;
    		break;
    		
  		case cs_sub:
    		NEED(cs_stack_top, 2);
    		cs_arg_stack[cs_stack_top-2] -= cs_arg_stack[cs_stack_top-1];
    		cs_stack_top--;
    		break;
    		
  		case cs_div:
    		NEED(cs_stack_top, 2);
    		cs_arg_stack[cs_stack_top-2] /= cs_arg_stack[cs_stack_top-1];
    		cs_stack_top--;
    		break;
    		
  		case cs_neg:
    		NEED(cs_stack_top, 1);
    		cs_arg_stack[cs_stack_top-1] *= -1.0;
    		break;
    		
  		case cs_eq:
    		NEED(cs_stack_top, 2);
    		cs_stack_top--;
    		if (cs_arg_stack[cs_stack_top] == cs_arg_stack[cs_stack_top-1]) 
      			cs_arg_stack[cs_stack_top-1] = 1.0;
    		else 
      			cs_arg_stack[cs_stack_top-1] = 0.0;
    		break;
    		
  		case cs_drop:
    		NEED(cs_stack_top, 1);
    		cs_stack_top--;
    		break;
    		
  		case cs_put:
    		NEED(cs_stack_top, 2);
    		{
      			int idx = (int)cs_arg_stack[--cs_stack_top];
      			NEED(CS_TRANS_ARRAY_MAX, idx);
      			cs_trn_array[idx] = cs_arg_stack[--cs_stack_top];
    		}
    		break;
    		
  		case cs_get:
    		NEED(cs_stack_top, 1);
    		{
      			int idx = (int)cs_arg_stack[cs_stack_top-1];
      			NEED(CS_TRANS_ARRAY_MAX, idx);
      			cs_arg_stack[cs_stack_top-1] = cs_trn_array[idx];
    		}
    		break;
    		
  		case cs_ifelse:
    		NEED(cs_stack_top, 4);
    		cs_stack_top -= 3;
    		if (cs_arg_stack[cs_stack_top+1] > cs_arg_stack[cs_stack_top+2]) 
      			cs_arg_stack[cs_stack_top-1] = cs_arg_stack[cs_stack_top];
    		break;
    		
  		case cs_mul:
    		NEED(cs_stack_top, 2);
    		cs_arg_stack[cs_stack_top-2] = cs_arg_stack[cs_stack_top-2] * cs_arg_stack[cs_stack_top-1];
    		cs_stack_top--;
    		break;
    		
  		case cs_sqrt:
    		NEED(cs_stack_top, 1);
    		cs_arg_stack[cs_stack_top-1] = sqrt(cs_arg_stack[cs_stack_top-1]);
    		break;
    		
  		case cs_dup:
    		NEED(cs_stack_top, 1);
    		NEED(CS_ARG_STACK_MAX, cs_stack_top+1);
    		cs_arg_stack[cs_stack_top] = cs_arg_stack[cs_stack_top-1];
    		cs_stack_top++;
    		break;
    		
  		case cs_exch:
    		NEED(cs_stack_top, 2);
    		{
      			double save = cs_arg_stack[cs_stack_top-2];
      			cs_arg_stack[cs_stack_top-2] = cs_arg_stack[cs_stack_top-1];
      			cs_arg_stack[cs_stack_top-1] = save;
    		}
    		break;
    		
  		case cs_index:
    		NEED(cs_stack_top, 2);
    		{
      			int idx = (int)cs_arg_stack[cs_stack_top-1];
      			if (idx < 0) 
					cs_arg_stack[cs_stack_top-1] = cs_arg_stack[cs_stack_top-2];
      			else 
      			{
					NEED(cs_stack_top, idx+2);
					cs_arg_stack[cs_stack_top-1] = cs_arg_stack[cs_stack_top-idx-2];
      			}
    		}
    		break;
    		
  		case cs_roll:
    		NEED(cs_stack_top, 2);
    		{
      			int J = (int)cs_arg_stack[--cs_stack_top];
      			int N = (int)cs_arg_stack[--cs_stack_top];
      			NEED(cs_stack_top, N);
      			if (J > 0) 
      			{
					J = J % N;
					while (J-- > 0) 
					{
	  					double save = cs_arg_stack[cs_stack_top-1];
	  					int i = cs_stack_top - 1;
	  					while (i > cs_stack_top-N) 
	  					{
	    					cs_arg_stack[i] = cs_arg_stack[i-1];
	    					i--;
	  					}
	  					cs_arg_stack[i] = save;
					}
      			} 
      			else 
      			{
					J = (-J) % N;
					while (J-- > 0) 
					{
	  					double save = cs_arg_stack[cs_stack_top-N];
	  					int i = cs_stack_top - N;
	  					while (i < cs_stack_top-1) 
	  					{
	    					cs_arg_stack[i] = cs_arg_stack[i+1];
	    					i++;
	  					}
	  					cs_arg_stack[i] = save;
					}
      			}
    		}
    		break;
    		
  		case cs_random:
    		NEED(CS_ARG_STACK_MAX, cs_stack_top+1);
    		cs_arg_stack[cs_stack_top++] = 1.0;
    		break;
    		
  		default:
    		cs_status = CS_PARSE_ERROR;
    		break;
  	}
}

void XWCSGInfo::getFixed(uchar **data, uchar *endptr)
{
	*data += 1;

  	SRC_NEED(endptr, *data + 4);

  	long ivalue = *(*data) * 0x100 + *(*data+1);
  	double rvalue = (ivalue > 0x7fffL) ? (ivalue - 0x10000L) : ivalue;
  	ivalue = *(*data+2) * 0x100 + *(*data+3);
  	rvalue += ((double) ivalue) / 0x10000L;

  	NEED(CS_ARG_STACK_MAX, cs_stack_top+1);
  	cs_arg_stack[cs_stack_top++] = rvalue;
  	*data += 4;
}

void XWCSGInfo::getInteger(uchar **data, uchar *endptr)
{
	long result = 0;
  	uchar b0 = **data, b1, b2;

  	*data += 1;

  	if (b0 == 28) 
  	{
    	SRC_NEED(endptr, *data + 2);
    	b1 = **data;
    	b2 = *(*data+1);
    	result = b1*256+b2;
    	if (result > 0x7fff)
      		result -= 0x10000L;
    	*data += 2;
  	} 
  	else if (b0 >= 32 && b0 <= 246) 
  	{
    	result = b0 - 139;
  	} 
  	else if (b0 >= 247 && b0 <= 250) 
  	{ 
    	SRC_NEED(endptr, *data + 1);
    	b1 = **data;
    	result = (b0-247)*256+b1+108;
    	*data += 1;
  	} 
  	else if (b0 >= 251 && b0 <= 254) 
  	{
    	SRC_NEED(endptr, *data + 1);
    	b1 = **data;
    	result = -(b0-251)*256-b1-108;
    	*data += 1;
  	} 
  	else 
  	{
    	cs_status = CS_PARSE_ERROR;
    	return;
  	}

  	NEED(CS_ARG_STACK_MAX, cs_stack_top+1);
  	cs_arg_stack[cs_stack_top++] = (double) result;
}

void XWCSGInfo::getSubr(uchar **subr, 
	                    long *len, 
	                    XWCFFIndex *subr_idx, 
	                    long id)
{
	if (subr_idx == NULL)
		return ;

  	ushort count = subr_idx->count;
  	if (count < 1240) 
    	id += 107;
  	else if (count < 33900) 
    	id += 1131;
  	else 
    	id += 32768;

  	if (id > count)
  		return ;

  	*len = (subr_idx->offset)[id + 1] - (subr_idx->offset)[id];
  	*subr = subr_idx->data + (subr_idx->offset)[id] - 1;
}

void XWCSGInfo::init()
{
	cs_status = CS_PARSE_OK;
  	cs_nest   = 0;
  	cs_phase  = 0;
  	cs_num_stems = 0;
  	cs_stack_top = 0;
  	cs_width = 0.0;
  	cs_have_width = 0;
}

