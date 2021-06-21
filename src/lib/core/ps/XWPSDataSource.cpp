/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <string.h>
#include "XWPSError.h"
#include "XWPSStream.h"
#include "XWPSFunction.h"
#include "XWPSTransferMap.h"
#include "XWPSDataSource.h"

XWPSDataSource::XWPSDataSource()
{
	type = data_source_type_none;
	access = 0;
	data.str.data = 0;
	data.str.size = 0;
	data.strm = 0;
}

XWPSDataSource::~XWPSDataSource()
{
}

int XWPSDataSource::accessBytes(ulong start, uint length, uchar * buf, uchar ** ptr)
{
	const uchar *p = data.str.data + start;

  if (ptr)
		*ptr = (uchar*)p;
  else
		memcpy(buf, p, length);
  return 0;
}

int XWPSDataSource::accessCalc(ulong start, uint length,  uchar *buf, uchar **ptr)
{
	XWPSFunctionPtCr * pfn = (XWPSFunctionPtCr *)(fn);
  PSStreamSFDState st;
  XWPSStream s;
  const PSStreamTemplate * templat = &s_SFD_template;

  (*(templat->set_defaults))((PSStreamState *)&st);
  st.skip_count = start;
  s.writeString(buf, length, false);
  s.procs.process = templat->process;
  s.state = (PSStreamState *)&st;
  if (templat->init)
		(*(templat->init))((PSStreamState *)&st);
  pfn->calcPut(&s);
  if (ptr)
		*ptr = buf;
  return 0;
}

int XWPSDataSource::accessStream(ulong start, uint length, uchar * buf, uchar ** ptr)
{
	XWPSStream *s = data.strm;
  const uchar *p;

  if (start >= s->position &&	(p = start - s->position + s->cbuf) + length <=	s->cursor.r.limit + 1) 
  {
		if (ptr)
	    *ptr = (uchar*)p;
		else
	    memcpy(buf, p, length);
  } 
  else 
  {
		uint nread;
		int code = s->seek(start);

		if (code < 0)
	    return (int)(XWPSError::RangeCheck);
		code = s->gets(buf, length, &nread);
		if (code < 0)
	    return (int)(XWPSError::RangeCheck);
		if (nread != length)
	    return (int)(XWPSError::RangeCheck);
		if (ptr)
	    *ptr = buf;
   }
   return 0;
}

int XWPSDataSource::accessString(ulong start, uint length, uchar * buf, uchar ** ptr)
{
	const uchar *p = data.str.data + start;

  if (ptr)
		*ptr = (uchar*)p;
  else
		memcpy(buf, p, length);
  return 0;
}

int XWPSDataSource::accessOnly(ulong start, uint length,uchar *buf, uchar **ptr)
{
	return (this->*access)(start, length, buf, ptr);
}

int XWPSDataSource::checkBPF(int bpf)
{
	if (isArray())
		return 2;
    
  switch (bpf) 
  {
    case 2: case 4: case 8:
			return bpf;
			
    default:
			return (int)(XWPSError::RangeCheck);
  }
}

void XWPSDataSource::initBytes(const uchar* bytes, uint len)
{
	type = data_source_type_bytes;
  data.str.data = bytes;
  data.str.size = len;
  access = &XWPSDataSource::accessBytes;
}

void XWPSDataSource::initFloats(const float* floats, uint count)
{
	type = data_source_type_floats;
	data.str.data = (uchar *)floats;
  data.str.size = count * sizeof(float);
  access = &XWPSDataSource::accessBytes;
}

void XWPSDataSource::initStream(XWPSStream * s)
{
	type = data_source_type_stream;
	data.strm = s;
	access = &XWPSDataSource::accessStream;
}

void XWPSDataSource::initString2(const uchar* bytes, uint len)
{
	type = data_source_type_string;
	data.str.data = bytes;
	data.str.size = len;
	access = &XWPSDataSource::accessString;
}

int XWPSDataSource::transferMapAccess(ulong start, uint length, uchar *buf, uchar **ptr)
{
	XWPSTransferMap *map = (XWPSTransferMap *)data.str.data;
  uint i;

  if (ptr)
		*ptr = buf;
  for (i = 0; i < length; ++i)
		buf[i] = frac2byte(map->values[(uint)start + i]);
  return 0;
}

int XWPSDataSource::transferMapAccessSigned(ulong start, uint length, uchar *buf, uchar **ptr)
{
	XWPSTransferMap *map = (XWPSTransferMap *)data.str.data;
  uint i;

  *ptr = buf;
  for (i = 0; i < length; ++i)
		buf[i] = (uchar)((frac2float(map->values[(uint)start + i]) + 1) * 127.5 + 0.5);
  return 0;
}
