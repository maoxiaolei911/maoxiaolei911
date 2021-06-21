/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <QIODevice>
#include "XWPSError.h"
#include "psscantable.h"
#include "XWPSStreamState.h"
#include "XWPSCosObject.h"
#include "XWPSDevicePDF.h"
#include "XWPSStream.h"

const PSStreamTemplate s_no_template = {
    0, 0, 1, 1, 0, 0, 0,0
};

PSStreamProc s_filter_read_procs = {
    &XWPSStream::stdNoAvailable, 
    &XWPSStream::stdNoSeek, 
    &XWPSStream::stdReadReset,
    &XWPSStream::stdReadFlush, 
    &XWPSStream::filterClose,
    0,
    0
};

PSStreamProc s_filter_write_procs = {
    &XWPSStream::stdNoAvailable, 
    &XWPSStream::stdNoSeek, 
    &XWPSStream::stdWriteReset,
    &XWPSStream::filterWriteFlush, 
    &XWPSStream::filterClose,
    0, 
    0
};

PSStreamProc cos_s_procs = {
    &XWPSStream::stdNoAvailable, 
    &XWPSStream::stdNoSeek, 
    &XWPSStream::stdWriteReset,
    &XWPSStream::stdWriteFlush, 
    &XWPSStream::cosWriteStreamClose, 
  	cos_write_stream_process,
  	0};

PSStreamProc s_proc_read_procs = {
    &XWPSStream::stdNoAvailable, 
    &XWPSStream::stdNoSeek, 
    &XWPSStream::stdReadReset,
    &XWPSStream::stdReadFlush, 
    &XWPSStream::stdNull, 
    	0,
    	0
};

PSStreamProc s_proc_write_procs = {
    &XWPSStream::stdNoAvailable, 
    &XWPSStream::stdNoSeek, 
    &XWPSStream::stdWriteReset,
    &XWPSStream::sprocWriteFlush, 
    &XWPSStream::stdNull, 
    0,
    0
};

static int s_string_read_process(PSStreamState * , 
                                 PSStreamCursorRead * ,
		                             PSStreamCursorWrite * , 
		                             bool)
{
	return EOFC;
}

static int s_string_write_process(PSStreamState * , 
                                 PSStreamCursorRead * ,
		                             PSStreamCursorWrite * , 
		                             bool last)
{
    return (last ? EOFC : ERRC);
}

static int s_write_position_process(PSStreamState * , 
                                    PSStreamCursorRead * pr,
			                              PSStreamCursorWrite * , bool)
{
    pr->ptr = pr->limit;
    return 0;
}

static int s_file_write_process(PSStreamState * st, 
                                PSStreamCursorRead * pr,
		                            PSStreamCursorWrite * , 
		                            bool )
{
	uint count = pr->limit - pr->ptr;
	int status = 0;
  if (count != 0) 
  {
  	XWPSStream *s = (XWPSStream *)st;
		QIODevice *file = s->file;
		int written = file->write((char*)(pr->ptr + 1), count);

		if (written < 0)
		{
	    written = 0;
	    status = ERRC;
	  }
		pr->ptr += written;
  }
  
  return status;
}

static int s_file_read_process(PSStreamState * st, 
                               PSStreamCursorRead *,
		                           PSStreamCursorWrite * pw, 
		                           bool )
{
	XWPSStream *s = (XWPSStream *)st;
  QIODevice *file = s->file;
  uint max_count = pw->limit - pw->ptr;
  int status = 1;
  int count;

  if (s->file_limit < max_long) 
  {
		long limit_count = s->file_offset + s->file_limit - (long)(file->pos());

		if (max_count > limit_count)
	    max_count = limit_count, status = EOFC;
  }
  if (file->atEnd())
  {
  	count = 0;
  	status = EOFC;
  }
  else
  {
  	count = file->read((char*)(pw->ptr + 1), max_count);
  	if (count < 0)
  	{
			count = 0;
			status = ERRC;
		}
	}
  pw->ptr += count;
  return status;
}

XWPSStream::XWPSStream()
{
	templat = (PSStreamTemplate*)&s_no_template;
	min_left = 0;
	error_string[0] = 0;
	cbuf = 0;
	bsize = 0;
	cbsize = 0;
	end_status = EOFC;
	foreign = 0;
	modes = 0;
	position = 0;
	procs.available = &XWPSStream::stdNoAvailable;
	procs.seek = &XWPSStream::stdNoSeek;
	procs.reset = &XWPSStream::stdReadReset;
	procs.flush = &XWPSStream::stdReadFlush;
	procs.close = &XWPSStream::stdNull;
	procs.process = 0;
	procs.switch_mode = 0;
	strm = 0;
	is_temp = 0;
	inline_temp = 0;
	state = 0;
	read_id = 1;
	write_id = 1;
	close_strm = false;
	file = 0;
	file_modes = 0;
	file_offset = 0;
	file_limit = 0;
	own_cbuf = false;
	own_file = false;
	own_ss = false;
	refCount = 1;
	cbuf_bytes = 0;
	state = (PSStreamState *)this;
	init();
}

XWPSStream::XWPSStream(XWPSCosObject *pcs, 
	                     XWPSDevicePDF *pdev)
{
	templat = (PSStreamTemplate*)&s_no_template;
	min_left = 0;
	error_string[0] = 0;
	cbuf = 0;
	bsize = 0;
	cbsize = 0;
	end_status = EOFC;
	foreign = 0;
	modes = 0;
	position = 0;
	strm = 0;
	is_temp = 0;
	inline_temp = 0;
	state = 0;
	read_id = 1;
	write_id = 1;
	close_strm = false;
	file = 0;
	close_file = true;
	file_modes = 0;
	file_offset = 0;
	file_limit = 0;
	own_cbuf = false;
	own_file = false;
	init();
	PSCosWriteStreamState *ss = new PSCosWriteStreamState;
  uchar *buf = (uchar*)malloc(512*sizeof(uchar));
  ss->templat = &cos_write_stream_template;
  ss->pcs = pcs;
  ss->pdev = pdev;
  ss->s = this;
  ss->target = pdev->streams->strm;
  refCount = 1;
  cbuf_bytes = 0;
  stdInit(buf, 512, &cos_s_procs, PS_STREAM_MODE_WRITE, true);
  state = (PSStreamState *)ss;
  own_ss = true;
}

XWPSStream::~XWPSStream()
{
	if (isValid() && !is_temp && file != 0)
	  close();
	
	if (strm)
	{
		if (strm->is_temp != 0)
		{
			if (strm->decRef() == 0)
				delete strm;
		}
  	strm = 0;
  }
  
  if (state != (PSStreamState*)this)
  {
  	if (own_ss)
  	  delete state;
  	state = 0;
  	own_ss = false;
  }
  
  if (cbuf_bytes)
	{
		if (cbuf_bytes->arr == cbuf)
		{
			own_cbuf = false;
			cbuf = 0;
		}
		
		if (cbuf_bytes->decRef() == 0)
			delete cbuf_bytes;
		cbuf_bytes = 0;
	}
	
	if (own_cbuf)
	{
		if (cbuf)
			free(cbuf);
		own_cbuf = false;
	}
  
	if (own_file)
	{
		if (file)
		{
			file->close();
			delete file;
			file = 0;
		}
		
		own_file = false;
	}
	
	if (file_name.data)
	{
		delete [] file_name.data;
		file_name.data = 0;
	}
}

XWPSStream * XWPSStream::addFilter(XWPSStream **ps, 
	                                 PSStreamTemplate *templ, 
	                                 PSStreamState *ss,
	                                 bool dss)
{
  uint bsize = qMax(templ->min_in_size, (uint)256);
  if (bsize > (*ps)->bsize && templ->process != s_NullE_template.process) 
  {
		PSStreamTemplate null_template;

		null_template = s_NullE_template;
		null_template.min_in_size = bsize;
		if (addFilter(ps, &null_template, NULL, false) == 0)
	    return 0;
  }
  XWPSStream *es = new XWPSStream;
  uchar *buf =0;
  buf =  (uchar*)malloc((bsize+1) * sizeof(uchar));
  PSStreamState *ess = (ss == 0 ? (PSStreamState *)es : ss);
  ess->templat = templ;
  initFilter(es, ess, buf, bsize, *ps, true, dss);
  *ps = es;
  return es;
}

void XWPSStream::appendFile(QIODevice * fileA, 
	                          uchar * buf, 
	                          uint len, 
	                          bool own_cbufA,
								            bool own_fileA)
{
	writeFile(fileA, buf, len, own_cbufA, own_fileA);
  modes = PS_STREAM_MODE_WRITE + PS_STREAM_MODE_APPEND;
  file_modes = modes;
  file->seek(file->size());
  position = (long)(file->pos());
}

int XWPSStream::available(long *pl)
{
	return (this->*(procs.available))(pl);
}

int XWPSStream::bufMinLeft()
{
	return	(strm == 0 ? (end_status != CALLC ? 0 : 1) : end_status == EOFC || end_status == ERRC ? 0 : 1);
}

int XWPSStream::close()
{
	int code = (this->*(procs.close))();
	if (code < 0)
		return code;
  PSStreamState * st = state;
  if (st)
  {
  	if (st->templat)
  		if (st->templat->release)
  			(*(st->templat->release))(st);
  			
  	if (st != (PSStreamState*)this)
  	{
  		if (own_ss)
  		  delete st;
  		state = (PSStreamState*)this;
  		own_ss = false;
  	}
  }
  	
  disable();
  return code;
}

int XWPSStream::closeFilters(XWPSStream **ps, XWPSStream *targetA)
{
	while (*ps != targetA)
	{
		XWPSStream *s = *ps;
		XWPSStream *next = s->strm;
		int status = s->close();
		if (s->decRef() == 0)
			delete s;
		if (status < 0)
	    return status;
	    	    	  
	  *ps = next;
	}
	
	return 0;
}

void XWPSStream::copyData(QIODevice *sfile, long count)
{
	long left = count;
  char buf[515];

  while (left > 0) 
  {
		uint copy = qMin(left, (long)512);
		sfile->read(buf, 512);
		write(buf, copy);
		left -= copy;
  }
}

int  XWPSStream::cosWriteStreamClose()
{
	PSCosWriteStreamState *ss = (PSCosWriteStreamState *)state;
  int status;

  flush();
  status = closeFilters(&ss->target, ss->pdev->streams->strm);
  return (status < 0 ? status : stdClose());
}

void XWPSStream::disable()
{	
	cbuf = 0;
	bsize = 0;
  end_status = EOFC;
  modes = 0;
  cbuf_string.data = 0;
  cursor.r.ptr = cursor.r.limit = 0;
  cursor.w.limit = 0;
  procs.close = &XWPSStream::stdNull;
  strm = 0;
  templat = (PSStreamTemplate*)&s_no_template;
  state = (PSStreamState*)this;
  own_ss = false;
}

int XWPSStream::fileAvailable(long *pl)
{
	long max_avail = file_limit - tell();
  long buf_avail = bufAvailable();
  *pl = qMin(max_avail, buf_avail);
  if (canSeek()) 
  {
		long pos, end;
		pos = (long)(file->pos());
		if (!file->seek(file->size()))
	    return ERRC;
		end = (long)(file->pos());
		if (!file->seek(pos))
	    return ERRC;
		buf_avail += end - pos;
		*pl = qMin(max_avail, buf_avail);
		if (*pl == 0)
	    *pl = -1;
  } 
  else 
  {
		if (*pl == 0 && file->atEnd())
	    *pl = -1;
  }
  return 0;
}

int XWPSStream::fileCloseDisable()
{
	int code = (this->*save_close)();

  if (code)
		return code;
		
  read_id = write_id = (read_id | write_id) + 1;
  return fileCloseFinish();
}

int XWPSStream::fileCloseFile()
{
	XWPSStream *stemp = strm;
  int code = fileCloseDisable();

  if (code)
		return code;
	
	while (stemp != 0 && stemp->is_temp != 0) 
	{
		XWPSStream *snext = stemp->strm;
		if (stemp->state != (PSStreamState*)stemp)
    {
  	  if (stemp->own_ss)
  	    delete stemp->state;
  	  stemp->state = 0;
  	  stemp->own_ss = false;
    }
  
    if (stemp->cbuf_bytes)
	  {
		  if (stemp->cbuf_bytes->arr == stemp->cbuf)
		  {
			  stemp->own_cbuf = false;
			  stemp->cbuf = 0;
		  }
		
		  if (stemp->cbuf_bytes->decRef() == 0)
			  delete stemp->cbuf_bytes;
		  stemp->cbuf_bytes = 0;
	  }
	
	  if (stemp->own_cbuf)
	  {
		  if (stemp->cbuf)
			  free(stemp->cbuf);
		  stemp->own_cbuf = false;
	  }
		stemp->disable();
		stemp = snext;
  }
  
  if (close_strm && stemp != 0)
		return stemp->close();
  return 0;
}

int XWPSStream::fileName(XWPSString * pfname)
{
	pfname->data = file_name.data;
  if (pfname->data == 0) 
  {
		pfname->size = 0;
		return -1;
  }
  pfname->size = file_name.size - 1;
  return 0;
}

int XWPSStream::fileReadClose()
{
  if (file != 0 && close_file) 
  {
		file->close();
	}
  return 0;
}

int XWPSStream::fileReadSeek(long pos)
{
	uint end = cursor.r.limit - cbuf + 1;
  long offset = pos - position;

  if (offset >= 0 && offset <= end) 
  {
		cursor.r.ptr = cbuf + offset - 1;
		return 0;
  }
  if (pos < 0 || pos > file_limit || !file->seek(file_offset + pos))
		return ERRC;
  cursor.r.ptr = cursor.r.limit = cbuf - 1;
  end_status = 0;
  position = pos;
  return 0;
}

int XWPSStream::fileSwitch(bool writing)
{
	uint s_modes = file_modes;
	if (writing)
	{
		if (!(file_modes & PS_STREAM_MODE_WRITE))
	    return ERRC;
	    
	  long pos = tell();
	  file->seek(pos);
	  
	  if (s_modes & PS_STREAM_MODE_APPEND)
	  	appendFile(file, cbuf, cbsize, own_cbuf, own_file);
	  else
	  {
	  	writeFile(file, cbuf, cbsize, own_cbuf, own_file);
	    position = pos;
	  }
	  modes = s_modes;
	}
	else
	{
		if (!(file_modes & PS_STREAM_MODE_READ))
	    return ERRC;
	    
	  long pos = tell();
	  if (flush() < 0)
	    return ERRC;
		readFile(file, cbuf, cbsize, own_cbuf, own_file);
		modes |= s_modes & PS_STREAM_MODE_APPEND;
		position = pos;
	}
	
	file_modes = s_modes;
	return 0;
}

int XWPSStream::fileWriteClose()
{
	processWriteBuf(true);
  return fileReadClose();
}

int XWPSStream::fileWriteFlush()
{
	int result = processWriteBuf(false);
  return result;
}

int XWPSStream::fileWriteSeek(long pos)
{
	int code = flush();
  if (code < 0)
		return code;
  if (file->seek(pos) != 0)
		return ERRC;
  position = pos;
  return 0;
}

int XWPSStream::filterWriteFlush()
{
	int status = processWriteBuf(false);
  if (status != 0)
		return status;
		
  return strm->flush();
}

int XWPSStream::filterClose()
{
	if (isWriting()) 
	{
		int status = processWriteBuf(true);

		if (status != 0 && status != EOFC)
	    return status;
  }
  
  return stdClose();
}

int XWPSStream::flush()
{
	return ((this->*(procs.flush))());
}

int XWPSStream::getc()
{
	if (cursor.r.limit - cursor.r.ptr > 1)
	{
		++(cursor.r.ptr);
		return (int)(*cursor.r.ptr);
	}
	
	return pgetc();
}

int XWPSStream::getMatrix(XWPSMatrix *pmat)
{
	int b = getc();
  float coeff[6];
  int i;
  int status;
  uint nread;

  if (b < 0)
		return b;
  for (i = 0; i < 4; i += 2, b <<= 2)
		if (!(b & 0xc0))
	    coeff[i] = coeff[i ^ 3] = 0.0;
		else 
		{
	    float value;

	    status = gets((uchar *)&value, sizeof(value), &nread);
	    if (status < 0)
				return status;
	    coeff[i] = value;
	    switch ((b >> 6) & 3) 
	    {
				case 1:
		    	coeff[i ^ 3] = value;
		    	break;
				case 2:
		    	coeff[i ^ 3] = -value;
		    	break;
				case 3:
		    	status = gets((uchar *)&coeff[i ^ 3], sizeof(coeff[0]), &nread);
		    	if (status < 0)
					return status;
	    }
		}
  for (; i < 6; ++i, b <<= 1)
		if (b & 0x80) 
		{
	   	status = gets((uchar *)&coeff[i], sizeof(coeff[0]), &nread);
	   	if (status < 0)
				return status;
		} 
		else
	   	coeff[i] = 0.0;
  pmat->xx = coeff[0];
  pmat->xy = coeff[1];
  pmat->yx = coeff[2];
  pmat->yy = coeff[3];
  pmat->tx = coeff[4];
  pmat->ty = coeff[5];
  return 0;
}

int XWPSStream::gets(uchar * buf, uint nmax, uint * pn)
{
	PSStreamCursorWrite cw;
  int status = 0;
  int min_leftA = bufMinLeft();

  cw.ptr = buf - 1;
  cw.limit = cw.ptr + nmax;
  while (cw.ptr < cw.limit) 
  {
		int left = cursor.r.limit - cursor.r.ptr;

		if (left > min_leftA) 
		{
	    cursor.r.limit -= min_leftA;
	     stream_move(&cursor.r, &cw);
	    cursor.r.limit += min_leftA;
		} 
		else 
		{
	    uint wanted = cw.limit - cw.ptr;
	    PSStreamState *st = state;
	    int c;
	    if (wanted >= bsize >> 2 &&	
	    	  st && wanted >= st->templat->min_out_size &&	
	    	  end_status == 0 && left == 0) 
	    {
				uchar *wptr = cw.ptr;

				cw.limit -= min_leftA;
				status = readBuf(&cw);
				cw.limit += min_leftA;
				cursor.r.ptr = cursor.r.limit = cbuf - 1;
				position += cw.ptr - wptr;
				if (status != 1 || cw.ptr == cw.limit)
		   	 break;
	    }
	    
	    c = pgetc();
	    if (c < 0) 
	    {
				status = c;
				break;
	    }
	    *++(cw.ptr) = c;
		}
  }
  *pn = cw.ptr + 1 - buf;
  return (status >= 0 ? 0 : status);
}

int XWPSStream::getVariableUint(uint *pw)
{
	uint w = 0;
  int shift = 0;
  int ch;

  for (; (ch = getc()) >= 0x80; shift += 7)
		w += (ch & 0x7f) << shift;
  if (ch < 0)
		return (int)(XWPSError::IOError);
  *pw = w + (ch << shift);
  return 0;
}

int XWPSStream::init()
{
	prev = next = 0;
  close_at_eod = true; return 0;
}

int XWPSStream::initFilter(XWPSStream *fs, 
	                         PSStreamState *fss, 
	                         uchar *buf, 
	                         uint bsize,
	                         XWPSStream *target,
	                         bool own_cbufA,
	                         bool dss)
{
	const PSStreamTemplate *templ = fss->templat;
  if (bsize < templ->min_in_size)
		return ERRC;
		
  fs->stdInit(buf, bsize, &s_filter_write_procs, PS_STREAM_MODE_WRITE, own_cbufA);
  fs->procs.process = templ->process;
  fs->state = fss;
  fs->own_ss = dss;
  if (templ->init)
		(templ->init)(fss);
  fs->strm = target;
  return 0;
}

int XWPSStream::initState(PSStreamState *st, const PSStreamTemplate *templ)
{
	st->templat = templ; return 0;
}

bool XWPSStream::isProc()
{
	return (procs.process == s_proc_read_process ||
           procs.process == s_proc_write_process);
}

bool XWPSStream::isStdin()
{
	return (procs.process == s_stdin_read_process);
}

int XWPSStream::openStdin(QIODevice * fileA)
{
	int l = 128;
	cbuf_bytes = new XWPSBytes;
	uchar * tb = (uchar*)malloc(l*sizeof(uchar));
	cbuf_bytes->arr = tb;
	cbuf_bytes->self = true;
	cbuf_bytes->length = l;
	readFile(fileA, tb, l, false, false);
	procs.process = s_stdin_read_process;
	save_close = &XWPSStream::stdNull;
	procs.close = &XWPSStream::fileCloseFile;
	return 0;
}

int XWPSStream::openStdout(QIODevice * fileA)
{
	int l = 128;
	cbuf_bytes = new XWPSBytes;
	uchar * tb = (uchar*)malloc(l*sizeof(uchar));
	cbuf_bytes->arr = tb;
	cbuf_bytes->self = true;
	cbuf_bytes->length = l;
	writeFile(fileA, tb, l, false, false);
	save_close = procs.flush;
	procs.close = &XWPSStream::fileCloseFile;
	return 0;
}

int XWPSStream::pgetc()
{
	return pgetcc(true);
}

int XWPSStream::pputc(uchar b)
{
	for (;;) 
	{
		if (end_status)
	    return end_status;
		if (!(cursor.w.ptr >= cursor.w.limit)) 
		{
	    *++(cursor.w.ptr) = b;
	    return b;
		}
		processWriteBuf(false);
  }
  
  return 0;
}

int XWPSStream::pputs(const char *str)
{
	uint len = strlen(str);
  uint used;
  int status = puts((const uchar *)str, len, &used);
  return (status >= 0 && used == len ? 0 : EOF);
}

void XWPSStream::print(const char *format, bool b)
{
	print(format, (b ? "true" : "false"));
}

const char * XWPSStream::print(const char *format, int v)
{
	const char *fp = printfScan(format);
  char str[25];
  sprintf(str, "%d", v);
  pputs(str);
  return printfScan(fp + 2);
}

const char * XWPSStream::print(const char *format, int v1, int v2)
{
	return print(print(format, v1), v2);
}

const char * XWPSStream::print(const char *format, int v1, int v2, int v3)
{
	return print(print(format, v1), v2, v3);
}

const char * XWPSStream::print(const char *format, int v1, int v2, int v3, int v4)
{
	return print(print(format, v1, v2), v3, v4);
}

const char * XWPSStream::print(const char *format, double v)
{
	const char *fp = printfScan(format);
  char str[50];
  sprintf(str, "%g", v);
  if (strchr(str, 'e')) 
  {
		sprintf(str, (fabs(v) > 1 ? "%1.1f" : "%1.8f"), v);
  }
  pputs(str);
  return printfScan(fp + 2);
}

const char * XWPSStream::print(const char *format, double v1, double v2)
{
	return print(print(format, v1), v2);
}

const char * XWPSStream::print(const char *format, double v1, double v2, double v3)
{
	return print(print(format, v1), v2, v3);
}

const char * XWPSStream::print(const char *format, double v1, double v2, double v3, double v4)
{
	return print(print(format, v1, v2), v3, v4);
}

const char * XWPSStream::print(const char *format, 
                               double v1, 
                               double v2, 
                               double v3, 
                               double v4,
                               double v5,
                               double v6)
{
	return print(print(format, v1, v2, v3), v4, v5, v6);
}

const char * XWPSStream::print(const char *format, long v)
{
	const char *fp = printfScan(format);
  char str[25];
  sprintf(str, "%ld", v);
  pputs(str);
  return printfScan(fp + 3);
}

const char * XWPSStream::print(const char *format, long v1, long v2)
{
	return print(print(format, v1), v2);
}

const char * XWPSStream::print(const char *format, long v1, long v2, long v3)
{
	return print(print(format, v1), v2, v3);
}

const char * XWPSStream::print(const char *format, const char *str)
{
	const char *fp = printfScan(format);
	pputs(str);
  return printfScan(fp + 2);
}

const char * XWPSStream::print(const char *format, const char *str1, const char *str2)
{
	return print(print(format, str1), str2);
}

const char * XWPSStream::print(const char *format, const char *str1, const char *str2, const char *str3)
{
	return print(print(format, str1), str2, str3);
}

int XWPSStream::processReadBuf()
{
  compact(false);
  int status = readBuf(&cursor.w);
  end_status = (status >= 0 ? 0 : status);
  return 0;
}

int XWPSStream::processWriteBuf(bool last)
{
	int status = writeBuf(&cursor.r, last);
  compact(false);
  return (status >= 0 ? 0 : status);
}

int  XWPSStream::pseek(long pos)
{
	return (this->*(procs.seek))(pos);
}

int  XWPSStream::putBytes(const uchar *ptr, uint count)
{
	uint used;
  puts(ptr, count, &used);
  return (int)used;
}

int XWPSStream::putc(uchar c)
{
	if (!(cursor.w.ptr >= cursor.w.limit))
	{
		++(cursor.w.ptr);
		*(cursor.w.ptr) = c;
		return 0;
	}
	
	return pputc(c);
}

void XWPSStream::putc2(int i)
{
	putc((uchar)(i >> 8));
  putc((uchar)i);
}

void XWPSStream::putc4(int i)
{
	putc2(i >> 16);
  putc2(i);
}

void XWPSStream::putLoca(ulong offset, int indexToLocFormat)
{
	if (indexToLocFormat)
		putULong(offset);
  else
		putUShort((uint)(offset >> 1));
}

int XWPSStream::putMatrix(const XWPSMatrix *pmat)
{
	uchar buf[1 + 6 * sizeof(float)];
  uchar *cp = buf + 1;
  uchar b = 0;
  float coeff[6];
  int i;
  uint ignore;

  coeff[0] = pmat->xx;
  coeff[1] = pmat->xy;
  coeff[2] = pmat->yx;
  coeff[3] = pmat->yy;
  coeff[4] = pmat->tx;
  coeff[5] = pmat->ty;
  for (i = 0; i < 4; i += 2) 
  {
		float u = coeff[i], v = coeff[i ^ 3];

		b <<= 2;
		if (u != 0 || v != 0) 
		{
	    memcpy(cp, &u, sizeof(float));
	    cp += sizeof(float);

	    if (v == u)
				b += 1;
	    else if (v == -u)
				b += 2;
	    else 
	    {
				b += 3;
				memcpy(cp, &v, sizeof(float));
				cp += sizeof(float);
	    }
		}
  }
  for (; i < 6; ++i) 
  {
		float v = coeff[i];

		b <<= 1;
		if (v != 0) 
		{
	    ++b;
	    memcpy(cp, &v, sizeof(float));
	    cp += sizeof(float);
		}
  }
  buf[0] = b << 2;
  return puts(buf, cp - buf, &ignore);
}

void XWPSStream::putPad(uint length)
{
	static const uchar pad_to_4[3] = {0, 0, 0};

  write(pad_to_4, (uint)(-length & 3));
}

int XWPSStream::puts(const uchar * str, uint wlen, uint * pn)
{
	uint len = wlen;
  int status = end_status;
  if (status >= 0)
  {
		while (len > 0) 
		{
	  	uint count = cursor.w.limit - cursor.w.ptr;
	  	if (count > 0) 
	  	{
				if (count > len)
		    	count = len;
				memcpy(cursor.w.ptr + 1, str, count);
				cursor.w.ptr += count;
				str += count;
				len -= count;
	   	} 
	   	else 
	   	{
				uchar ch = *str++;

				status = putc(ch);
				if (status < 0)
		    	break;
				len--;
	    }
		}
	}
  *pn = wlen - len;
  return (status >= 0 ? 0 : status);
}

int XWPSStream::puts(const char *str)
{
	uint len = strlen(str);
  uint used;
  int status = puts((const uchar *)str, len, &used);
  return (status >= 0 && used == len ? 0 : EOF);
}

void XWPSStream::putULong(ulong v)
{
	putUShort((uint)(v >> 16));
  putUShort((uint)v);
}

void XWPSStream::putUShort(uint v)
{
	pputc((uchar)(v >> 8));
  pputc((uchar)v);
}

void XWPSStream::putVariableUint(uint w)
{
	for (; w > 0x7f; w >>= 7)
		putc((uchar)(w | 0x80));
  putc(w);
}

void XWPSStream::readFile(QIODevice * fileA, 
                          uchar * buf, 
                          uint len, 
                          bool own_cbufA,
								          bool own_fileA)
{
	static const PSStreamProc p = {
		&XWPSStream::fileAvailable, 
		&XWPSStream::fileReadSeek, 
		&XWPSStream::stdReadReset,
		&XWPSStream::stdReadFlush, 
		&XWPSStream::fileReadClose, 
		s_file_read_process,
		&XWPSStream::fileSwitch};
			
//  long curpos = (long)fileA->pos();
  stdInit(buf, len, (PSStreamProc*)&p, PS_STREAM_MODE_READ + PS_STREAM_MODE_SEEK, own_cbufA);
  file = fileA;
  own_file = own_fileA;
  file_modes = modes;
  file_offset = 0;
  file_limit = max_long;
}

int XWPSStream::readLine(XWPSStream *s_out, 
                         void *,
	                       XWPSString *prompt, 
	                       XWPSString * buf,
	  									   uint * pcount, 
	  									   bool *pin_eol)
{
	uint count = *pcount;
	if (count == 0 && s_out && prompt)
	{
		uint ignore_n;
		int ch = s_out->puts(prompt->data, prompt->size, &ignore_n);
		if (ch < 0)
	    return ch;
	}
	
top:
	if (*pin_eol)
	{
		int ch = pgetcc(false);

		if (ch == EOFC) 
		{
	    *pin_eol = false;
	    return 0;
		} 
		else if (ch < 0)
	    return ch;
		else if (ch != '\n' && ch != '\r')
	    putBack();
		*pin_eol = false;
		return 0;
	}
	
	for (;;)
	{
		int ch = getc();
		if (ch < 0) 
		{
	    *pcount = count;
	    return ch;
		}
		
		switch (ch)
		{
			case '\r':
				*pcount = count;
				*pin_eol = true;
				goto top;
				break;
				
			case '\n':
				*pcount = count;
				return 0;
				break;
				
			default:
				break;
		}
		
		if (count >= buf->size)
		{
			uint nsize = count + qMax(count, (uint)20);					       
			buf->data = (uchar*)realloc(buf->data, nsize+1);
			if (buf->data == 0)
		    return ERRC; /* no better choice */
		    
			buf->size = nsize;
		}
		
		buf->data[count++] = ch;
	}
	
	return 0;
}

int  XWPSStream::readProc(XWPSRef * sop)
{
	procInit(sop, PS_STREAM_MODE_READ, &s_proc_read_template,    &s_proc_read_procs);
  end_status = CALLC;
  return 0;
}

void XWPSStream::readString(const uchar *ptr, uint len, bool own_cbufA)
{
	static const PSStreamProc p = {
	   &XWPSStream::stringAvailable, 
	   &XWPSStream::stringReadSeek, 
	   &XWPSStream::stdReadReset,
	   &XWPSStream::stdReadFlush, 
	   &XWPSStream::stdNull, 
	 	 s_string_read_process,
	 	 0};

    stdInit((uchar *)ptr, len, (PSStreamProc*)&p, PS_STREAM_MODE_READ + PS_STREAM_MODE_SEEK, own_cbufA);
    cbuf_string.data = (uchar *)ptr;
    cbuf_string.size = len;
    end_status = EOFC;
    cursor.r.limit = cursor.w.limit;
}

void XWPSStream::readStringReusable(const uchar *ptr, uint len, bool own_cbufA)
{
	static const PSStreamProc p = {
	 	&XWPSStream::stringAvailable, 
	 	&XWPSStream::stringReadSeek, 
	 	&XWPSStream::stringReusableReset,
	 	&XWPSStream::stringReusableFlush, 
	 	&XWPSStream::stdNull, 
	 	s_string_read_process,
	 	0};

    readString(ptr, len, own_cbufA);
    procs = p;
    close_at_eod = false;
}

void XWPSStream::reset()
{
	(this->*(procs.reset))();
}

int XWPSStream::readSubfile(long start, long length)
{
	if (file == 0 || modes != PS_STREAM_MODE_READ + PS_STREAM_MODE_SEEK ||
			file_offset != 0 || file_limit != max_long ||
	   ((position < start || position > start + length) &&
	   seek(start) < 0))
		return ERRC;
  position -= start;
  file_offset = start;
  file_limit = length;
  return 0;
}

int XWPSStream::scangetc(uchar ** cp, uchar **ep)
{
	int c = 0;
	if (*cp >= *ep)
	{
		cursor.r.ptr = *cp;
		inline_temp = pgetc();
		*cp = cursor.r.ptr;
		*ep = cursor.r.limit;
		c = inline_temp;
	}
	else
	{
		uchar * p = *cp;
		p += 1;
		c = *p;
		*cp = p;
	}
	
	return c;
}

int XWPSStream::seek(long pos)
{
	return pseek(pos);
}

int  XWPSStream::setFileName(const uchar *data, uint size)
{
	if (file_name.data)
	{
		delete [] file_name.data;
		file_name.data = 0;
	}
	
	if (size <= 0)
		return 0;
		
	file_name.data = new uchar[size + 1];
	memcpy(file_name.data, data, size);
  file_name.data[size] = 0;
  file_name.size = size + 1;
  return 0;
}

int  XWPSStream::skip(long nskip,long *pskipped)
{
	return pskip(nskip, pskipped);
}

int XWPSStream::sprocWriteFlush()
{
	int result = processWriteBuf(false);
  PSStreamProcState * ss = (PSStreamProcState *)state;

  return (result < 0 || ss->index == 0 ? result : CALLC);
}

int  XWPSStream::stdInit(uchar * ptr, 
                       uint len, 
                       PSStreamProc * pp, 
                       int modesA, 
                       bool own_cbufA)
{
	templat = (PSStreamTemplate*)&s_no_template;
	cbuf = ptr;
	modes = modesA;
	bsize = cbsize = len;
	cursor.r.ptr = cursor.r.limit = cursor.w.ptr = ptr - 1;	
	cursor.w.limit = ptr - 1 + len;
	end_status = 0;
	foreign = 0;
	position = 0;
	strm = 0;
	is_temp = 0;
	procs = *pp;
	state = (PSStreamState *)this;
	own_ss = false;
	file = 0;
	own_cbuf = own_cbufA;
	cbuf_string.data = 0;
	cbuf_string.size = 0;
	return 0;
}

int XWPSStream::stdNoAvailable(long * pl)
{
	*pl = -1;
  return 0;
}

int XWPSStream::stdReadFlush()
{
	while (1)
	{
		cursor.r.ptr = cursor.r.limit = cbuf - 1;
		if (end_status)
			break;
			
		processReadBuf();
	}
	
	return (end_status == EOFC ? 0 : end_status);
}

void XWPSStream::stdReadReset()
{
	cursor.r.ptr = cursor.r.limit = cbuf - 1;
}

int XWPSStream::stdWriteFlush()
{
	return processWriteBuf(false);
}

void XWPSStream::stdWriteReset()
{
	cursor.w.ptr = cbuf - 1;
}

int  XWPSStream::sswitch(bool writing)
{
	if (procs.switch_mode == 0)
		return ERRC;
	
	return (this->*(procs.switch_mode))(writing);
}

int XWPSStream::stringAvailable(long * pl)
{
	*pl = bufAvailable();
  if (*pl == 0 && close_at_eod)
		*pl = -1;
  return 0;
}

int XWPSStream::stringReadSeek(long pos)
{
	if (pos < 0 || pos > bsize)
		return ERRC;
  cursor.r.ptr = cbuf + pos - 1;
  cursor.r.limit = cbuf + bsize - 1;
  return 0;
}

int XWPSStream::stringReusableFlush()
{
	cursor.r.ptr = cursor.r.limit = cbuf + bsize - 1;
	return 0;
}

void XWPSStream::stringReusableReset()
{
	cursor.r.ptr = cbuf - 1;
  cursor.r.limit = cursor.r.ptr + bsize;
}

int  XWPSStream::stringWriteSeek(long pos)
{
	if (pos < 0 || pos > bsize)
		return ERRC;
  cursor.w.ptr = cbuf + pos - 1;
  return 0;
}

long XWPSStream::tell()
{
  uchar *ptr = (isWriting() ? cursor.w.ptr : cursor.r.ptr);

  return (ptr == 0 ? 0 : ptr + 1 - cbuf) + position;
}

int XWPSStream::ungetc(uchar c)
{
	if (!isReading() || cursor.r.ptr < cbuf || *(cursor.r.ptr) != c)
		return ERRC;
  cursor.r.ptr--;
  return 0;
}

int XWPSStream::write(const void *ptr, uint count)
{
	uint used = 0;
  puts((const uchar *)ptr, count, &used);
  return (int)used;
}

int  XWPSStream::writeEncrypted(const void *ptr, uint count)
{
	const uchar *const data = (const uchar*)ptr;
  ushort s = crypt_charstring_seed;
  uchar buf[50];
  uint left, n;
  int code = 0;

  for (left = count; left > 0; left -= n) 
  {
		n = qMin(left, sizeof(buf));
		ps_type1_encrypt(buf, data + count - left, n, &s);
		code = write(buf, n);
  }
  return code;
}

void XWPSStream::writeFile(QIODevice * fileA, 
                           uchar * buf, 
                           uint len, 
                           bool own_cbufA,
								           bool own_fileA)
{
	static const PSStreamProc p = {
		&XWPSStream::stdNoAvailable, 
		&XWPSStream::fileWriteSeek, 
		&XWPSStream::stdWriteReset,
		&XWPSStream::fileWriteFlush, 
		&XWPSStream::fileWriteClose, 
		s_file_write_process,
		&XWPSStream::fileSwitch};

    stdInit(buf, len, (PSStreamProc*)&p, PS_STREAM_MODE_WRITE + PS_STREAM_MODE_SEEK, own_cbufA);
    file = fileA;
    own_file = own_fileA;
    file_modes = modes;
    file_offset = 0;
    file_limit = max_long;	/* ibid. */
}

void XWPSStream::writeFontInfo(const char *key, const XWPSString *pvalue,	int do_write)
{
	if (do_write) 
	{
		print("\n/%s ", key);
		writePSString(pvalue->data, pvalue->size, PRINT_HEX_NOT_OK);
		pputs(" def");
  }
}

void XWPSStream::writePositionOnly()
{
	static uchar discard_buf[50];
  writeString(discard_buf, sizeof(discard_buf), false);
  procs.process = s_write_position_process;
}

int  XWPSStream::writeProc(XWPSRef * sop)
{
	return procInit(sop, PS_STREAM_MODE_WRITE, &s_proc_write_template, &s_proc_write_procs);
}

void XWPSStream::writePSString(const uchar * str, uint size, int print_ok)
{
	const PSStreamTemplate *templ;
  PSStreamAXEState ss;
  PSStreamState *st = NULL;
    
  if (print_ok & PRINT_BINARY_OK)
  {
  	pputc('(');
		for (uint i = 0; i < size; ++i) 
		{
	    uchar ch = str[i];
	    switch (ch) 
	    {
				case char_CR:
		    	pputs("\\r");
		    	continue;
		    	
				case char_EOL:
		    	pputs("\\n");
		    	continue;
		    	
				case '(':
				case ')':
				case '\\':
		    	pputc('\\');
	    }
	    pputc(ch);
		}
		pputc(')');
		return;
  }
  
	uint added = 0;
  
  for (uint i = 0; i < size; ++i)
  {
  	uchar ch = str[i];
		if (ch == 0 || ch >= 127)
	    added += 3;
		else if (strchr("()\\\n\r\t\b\f", ch) != 0)
	    ++added;
		else if (ch < 32)
	    added += 3;
  }
  
  if (added < size || (print_ok & PRINT_HEX_NOT_OK))
  {
  	templ = &s_PSSE_template;
  	pputc('(');
  }
  else
  {
  	templ = &s_AXE_template;
		st = (PSStreamState *) & ss;
		ss.EndOfData = true;
		ss.count = 0;
		pputc('<');
  }
  
  {
		uchar buf[100];		/* size is arbitrary */
		PSStreamCursorRead r;
		PSStreamCursorWrite w;
		int status = 0;

		r.ptr = (uchar*)(str - 1);
		r.limit = r.ptr + size;
		w.limit = buf + sizeof(buf) - 1;
		do 
		{
	    w.ptr = buf;  w.ptr--;
	    status = (*(templ->process))(st, &r, &w, true);
	    write(buf, (uint) (w.ptr + 1 - buf));
		}
		while (status == 1);
  }
}

void XWPSStream::writeString(const uchar *ptr, uint len, bool own_cbufA)
{
	static const PSStreamProc p = {
		&XWPSStream::stdNoAvailable, 
		&XWPSStream::stringWriteSeek, 
		&XWPSStream::stdWriteReset,
		&XWPSStream::stdNull, 
		&XWPSStream::stdNull, 
		s_string_write_process,
		0};

    stdInit((uchar*)ptr, len, (PSStreamProc*)&p, PS_STREAM_MODE_WRITE + PS_STREAM_MODE_SEEK, own_cbufA);
    cbuf_string.data = (uchar*)ptr;
    cbuf_string.size = len;
}

void XWPSStream::writeUid(XWPSUid *puid)
{
	if (puid->isUniqueID())
		print("/UniqueID %ld def\n", puid->id);
  else if (puid->isXUID()) 
  {
		uint n = puid->XUIDSize();
		pputs("/XUID [");
		long * v = puid->XUIDValues();
		for (uint i = 0; i < n; ++i)
	    print("%ld ", v[i]);
		pputs("] readonly def\n");
  }
}

void XWPSStream::compact(bool always)
{
	if (cursor.r.ptr >= cbuf && (always || end_status >= 0)) 
	{
		uint dist = cursor.r.ptr + 1 - cbuf;

		memmove(cbuf, cursor.r.ptr + 1, (uint)(cursor.r.limit - cursor.r.ptr));
		cursor.r.ptr = cbuf - 1;
		cursor.r.limit -= dist;
		position += dist;
  }
}

int XWPSStream::pgetcc(bool closeateod)
{
	int status, left;
  int min_leftA = bufMinLeft();

  while (status = end_status, left = cursor.r.limit - cursor.r.ptr, left <= min_leftA && status >= 0)
		processReadBuf();
  
  if (left <= min_leftA &&	(left == 0 || (status != EOFC && status != ERRC))) 
  {
		compact(true);
		if (status == EOFC && closeateod && close_at_eod) 
		{
	    status = close();
	    if (status == 0)
				status = EOFC;
	    end_status = status;
		}
		return status;
  }
  return *++(cursor.r.ptr);
}

const char * XWPSStream::printfScan(const char *format)
{
	const char *fp = format;
  for (; *fp != 0; ++fp) 
  {
		if (*fp == '%') 
		{
	    if (fp[1] != '%')
				break;
	    ++fp;
		}
		putc(*fp);
  }
  return fp;
}

int XWPSStream::procInit(XWPSRef * sop, 
	             uint mode,
               const PSStreamTemplate * temp, 
               const PSStreamProc * procsA)
{
	stdInit(NULL, 0, (PSStreamProc*)procsA, mode, false);
	PSStreamProcState * ss = new PSStreamProcState;
	state = (PSStreamState*)ss;
	procs.process = temp->process;
	ss->index = 0;
	ss->proc.assign(sop);
	ss->data.makeString(PS_A_ALL, 0);
	own_ss = true;
	ss->templat = (PSStreamTemplate*)temp;
	ss->min_left = 1;
	ss->eof = 0;
	return 0;
}

int XWPSStream::pskip(long nskip, long *pskipped)
{
	long n = nskip;
  int min_leftA;

  if (nskip < 0 || !isReading()) 
  {
		*pskipped = 0;
		return ERRC;
  }
  
  if (canSeek()) 
  {
		long pos = tell();
		int code = seek(pos + n);

		*pskipped = tell() - pos;
		return code;
  }
  
  min_leftA = bufMinLeft();
  while (bufAvailable() < n + min_leftA) 
  {
		n -= bufAvailable();
		cursor.r.ptr = cursor.r.limit;
		if (end_status) 
		{
	    *pskipped = nskip - n;
	    return end_status;
		}
		int code = getc();
		if (code < 0) 
		{
	    *pskipped = nskip - n;
	    return code;
		}
		--n;
  }
  cursor.r.ptr += n;
  *pskipped = nskip;
  return 0;
}

int XWPSStream::readBuf(PSStreamCursorWrite * pbuf)
{
	XWPSStream *s_prev = 0;
	XWPSStream *curr = this;
  int status = 0;
  for (;;)
  {
  	XWPSStream *s_strm = 0;
  	for (;;)
  	{
  		PSStreamCursorRead cr;
	    PSStreamCursorRead *pr;
	    PSStreamCursorWrite *pw;
	    int left;
	    bool eof;
	    
	    s_strm = curr->strm;
	    if (s_strm == 0)
	    {
	    	cr.ptr = 0;
	    	cr.limit = 0;
				pr = &cr;
				left = 0;
				eof = false;
	    }
	    else 
	    {
				pr = &s_strm->cursor.r;
				left = s_strm->bufMinLeft();
				left = qMin(left, pr->limit - pr->ptr);
				pr->limit -= left;
				eof = s_strm->end_status == EOFC;
	    }
	    
	    pw = (s_prev == 0 ? pbuf : &curr->cursor.w);
	    status = (*(curr->procs.process))(curr->state, pr, pw, eof);
	    pr->limit += left;
	    if (s_strm == 0 || status != 0)
				break;
	    status = s_strm->end_status;
	    if (status < 0)
				break;
				
			do
			{
				XWPSStream *ahead = curr->strm;
    		curr->strm = s_prev; 
    		s_prev = curr; 
    		curr = ahead;
    	} while(0);
    	if (curr)
    		curr->compact(false);
  	}
  	
  	if (s_strm != 0 && 
  		  status == EOFC && 
  		  curr->cursor.r.ptr >= curr->cursor.r.limit && 
  		  curr->close_at_eod) 
  	{
	    int cstat = curr->close();

	    if (cstat != 0)
				status = cstat;
		}
		
		curr->end_status = (status >= 0 ? 0 : status);
		if (s_prev == 0)
	    return status;
	    
	  do
	  {
	  	XWPSStream *back = s_prev->strm;
    	s_prev->strm = curr; 
    	curr = s_prev; 
    	s_prev = back;
    } while(0);
  }
  
  return 0;
}

int XWPSStream::writeBuf(PSStreamCursorRead * pbuf, bool last)
{
	XWPSStream *s_prev = 0;
  XWPSStream *curr = this;
  int depth = 0;
  int status;
  
  for (;;)
  {
  	for (;;)
  	{
  		XWPSStream *s_strm = curr->strm;
	    PSStreamCursorWrite cw;
	    PSStreamCursorRead *pr;
	    PSStreamCursorWrite *pw;
	    
	    bool end = last && (s_prev == 0 || (depth <= 1 && s_prev->end_status == EOFC));
	    if (s_strm == 0)
	    {
				cw.ptr = 0;
				cw.limit = 0; 
				pw = &cw;
			}
	    else
				pw = &s_strm->cursor.w;
	    if (s_prev == 0)
				pr = pbuf;
	    else
				pr = &curr->cursor.r;
				
			status = curr->end_status;
	    if (status >= 0)
	    {
	    	status = (*(curr->procs.process))(curr->state, pr, pw, end);
	    	if (status == 0 && end)
		    	status = EOFC;
				if (status == EOFC || status == ERRC)
		    	curr->end_status = status;
	    }
	    
	    if (s_strm == 0 || (status < 0 && status != EOFC))
				break;
	    if (status != 1)
	    {
	    	if (!end || !s_strm->is_temp)
		    	break;
	    }
	    
	    status = s_strm->end_status;
	    if (status < 0)
				break;
	    if (!curr->is_temp)
				++depth;
				
			do
			{
				XWPSStream *ahead = curr->strm;
    		curr->strm = s_prev; 
    		s_prev = curr; 
    		curr = ahead;
    	} while(0);
    	if (curr)
    		curr->compact(false);
  	}
  	
  	curr->end_status = (status >= 0 ? 0 : status);
		if (status < 0 || s_prev == 0)
		{
			while (s_prev)
			{
				do
	  		{
	  			XWPSStream *back = s_prev->strm;
    			s_prev->strm = curr; 
    			curr = s_prev; 
    			s_prev = back;    			
    		} while(0);
    		
    		if (status >= 0)
		    	curr->end_status = 0;
				else if (status == ERRC)
		    	curr->end_status = ERRC;
			}
			
			return status;
		}
		
		do
	  {
	  	XWPSStream *back = s_prev->strm;
    	s_prev->strm = curr; 
    	curr = s_prev; 
    	s_prev = back;    			
    } while(0);
    
    if (!curr->is_temp)
	    --depth;
  }
  
  return 0;
}
