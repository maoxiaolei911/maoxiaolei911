/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSDATASOURCE_H
#define XWPSDATASOURCE_H

#include "XWPSType.h"

class XWPSStream;
class XWPSDataSource;

enum PSDataSourceType
{
	data_source_type_none = -1,
	data_source_type_string = 0,
  data_source_type_bytes,
  data_source_type_floats,
  data_source_type_stream
};

typedef int (XWPSDataSource::*ds_access_t)(ulong start, uint length,uchar *buf, uchar **ptr);

class XWPSDataSource
{
public:	
	XWPSDataSource();
	~XWPSDataSource();		
	
	int accessBytes(ulong start, uint length, uchar * buf, uchar ** ptr);
	int accessCalc(ulong start, uint length,  uchar *buf, uchar **ptr);
	int accessStream(ulong start, uint length, uchar * buf, uchar ** ptr);
	int accessString(ulong start, uint length, uchar * buf, uchar ** ptr);
	
	int accessOnly(ulong start, uint length,uchar *buf, uchar **ptr);
	
	int checkBPF(int bpf);
	
	void initBytes(const uchar* bytes, uint len);
	void initFloats(const float* floats, uint count);
	void initStream(XWPSStream * s);
	void initString2(const uchar* bytes, uint len);
	bool isArray() {return type == data_source_type_floats;}
	bool isStream() {return type == data_source_type_stream;}
	
	int transferMapAccess(ulong start, uint length, uchar *buf, uchar **ptr);
	int transferMapAccessSigned(ulong start, uint length, uchar *buf, uchar **ptr);
	
public:
	ds_access_t access;
  PSDataSourceType type;
  union d_ 
  {
		PSConstString str;
		XWPSStream *strm;
  } data;
  
  void * fn;
};


#endif //XWPSDATASOURCE_H
