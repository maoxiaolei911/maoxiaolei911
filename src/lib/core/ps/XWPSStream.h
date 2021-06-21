/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSSTREAM_H
#define XWPSSTREAM_H

#include "XWPSStreamState.h"

class QIODevice;
class QString;
class XWPSStreamState;
class XWPSStream;
class XWPSCosStream;
class XWPSDevicePDF;

#define PS_STREAM_MODE_READ   1
#define PS_STREAM_MODE_WRITE  2
#define PS_STREAM_MODE_SEEK   4
#define PS_STREAM_MODE_APPEND 8

#define PS_MAX_MIN_LEFT 1

#define PRINT_BINARY_OK 1
#define PRINT_ASCII85_OK 2
#define PRINT_HEX_NOT_OK 4

struct PSStreamProc
{
	int (XWPSStream::*available)(long *);
	int (XWPSStream::*seek)(long);
	void (XWPSStream::*reset)();
	int (XWPSStream::*flush)();
	int (XWPSStream::*close)();
	int  (*process)(PSStreamState*, PSStreamCursorRead * , PSStreamCursorWrite * , bool );
	int (XWPSStream::*switch_mode)(bool);
};

extern PSStreamProc s_filter_read_procs;
extern PSStreamProc s_filter_write_procs;
extern PSStreamProc cos_s_procs;
extern PSStreamProc s_proc_read_procs;
extern PSStreamProc s_proc_write_procs;

class XWPSStream
{
public:	
	XWPSStream();	
	XWPSStream(XWPSCosObject *pcs, XWPSDevicePDF *pdev);
	~XWPSStream();
	
	static XWPSStream * addFilter(XWPSStream **ps, 
	                              PSStreamTemplate *templ, 
	                              PSStreamState *ss,
	                              bool dss);
	
	void appendFile(QIODevice * fileA, 
	               uchar * buf, 
	               uint len, 
	               bool own_cbufA,
								 bool own_fileA);
		
	int available(long *pl);
	
	int     bufAvailable() {return (cursor.r.limit - cursor.r.ptr);}
	int     bufMinLeft();
	uchar * bufPtr() {return cursor.r.ptr + 1;}
	void    bufSkip(uint n) {cursor.r.ptr += n;}
	
	bool canSeek() {return ((modes & PS_STREAM_MODE_SEEK) != 0);}	
	int close();
	
	static int closeFilters(XWPSStream **ps, XWPSStream *targetA);
	
	void copyData(QIODevice *sfile, long count);
	int  cosWriteStreamClose();
		
	int decRef() { return --refCount; }
	void disable();
	
	bool eofp() {return (end_status == EOFC);}
	
	int fileAvailable(long *pl);
	int fileCloseDisable();
	int fileCloseFile();
	int fileCloseFinish() {return 0;}
	int fileName(XWPSString * pfname);
	int fileReadClose();
	int fileReadSeek(long pos);
	int fileSwitch(bool writing);
	int fileWriteClose();
	int fileWriteFlush();
	int fileWriteSeek(long pos);
	int filterClose();
	int filterWriteFlush();
	int flush();
	
	int getc();
	XWPSStreamCursor * getCursor() {return &cursor;}
	short   getEndStatus() {return end_status;}
	uchar   getForeign() {return foreign;}
	int gets(uchar * buf, uint nmax, uint * pn);
	int     getMatrix(XWPSMatrix *pmat);
	int getVariableUint(uint *pw);
	
	int  incRef() { return ++refCount; }
	int  init();
	static int  initFilter(XWPSStream *fs, 
	                       PSStreamState *fss, 
	                       uchar *buf, 
	                       uint bsize,
	                       XWPSStream *target,
	                       bool own_cbufA = true,
	                       bool dss = false);
	static int initState(PSStreamState *st, const PSStreamTemplate *templ);
	
	bool isAppend() {return ((modes & PS_STREAM_MODE_APPEND) != 0);}
	bool isProc();
	bool isReading() {return ((modes & PS_STREAM_MODE_READ) != 0);}
	bool isStdin();
	bool isValid() { return (modes != 0);}  
  bool isWriting() {return ((modes & PS_STREAM_MODE_WRITE) != 0);}
  
  int openStdin(QIODevice * fileA);
  int openStdout(QIODevice * fileA);
  
  int pgetc();
  int  pputc(uchar b);
  int pputs(const char *str);
  void print(const char *format, bool b);
  const char * print(const char *format, int v);
  const char * print(const char *format, int v1, int v2);
  const char * print(const char *format, int v1, int v2, int v3);
  const char * print(const char *format, int v1, int v2, int v3, int v4);
  const char * print(const char *format, double v);
  const char * print(const char *format, double v1, double v2);
  const char * print(const char *format, double v1, double v2, double v3);
  const char * print(const char *format, double v1, double v2, double v3, double v4);
  const char * print(const char *format, 
                     double v1, 
                     double v2, 
                     double v3, 
                     double v4,
                     double v5,
                     double v6);
	const char * print(const char *format, long v);
	const char * print(const char *format, long v1, long v2);
	const char * print(const char *format, long v1, long v2, long v3);
	const char * print(const char *format, const char *str);
	const char * print(const char *format, const char *str1, const char *str2);
	const char * print(const char *format, const char *str1, const char *str2, const char *str3);
  int  processReadBuf();
  int  processWriteBuf(bool last);
  int  pseek(long pos);
  void putBack() {cursor.r.ptr--;}
  int  putBytes(const uchar *ptr, uint count);
  int  putc(uchar c);
  void putc2(int i);
  void putc4(int i);
  void putLoca(ulong offset, int indexToLocFormat);
  int  putMatrix(const XWPSMatrix *pmat);
  void putPad(uint length);
  int  puts(const uchar * str, uint wlen, uint * pn);
  int  puts(const char *str);
  void putULong(ulong v);
  void putUShort(uint v);
  void putVariableUint(uint w);
  
  void readFile(QIODevice * fileA, 
                 uchar * buf, 
                 uint len, 
                 bool own_cbufA = false,
								 bool own_fileA = false);
  int readLine(XWPSStream *s_out, 
               void *readline_data,
	             XWPSString *prompt, 
	             XWPSString * buf,
							 uint * pcount, 
							 bool *pin_eol);
  int  readProc(XWPSRef * sop);
  void readString(const uchar *ptr, uint len, bool own_cbufA = false);
  void readStringReusable(const uchar *ptr, uint len, bool own_cbufA = false);
  
  int  readSubfile(long start, long length);
  void reset();
    
  int scangetc(uchar ** cp, uchar **ep);
  
  int  seek(long pos);
  int  setFileName(const uchar *data, uint size);
  void setReadId(ushort i) {read_id = i;}
  void setWriteId(ushort i) {write_id = i;}
  int  skip(long nskip,long *pskipped);
  
  int sprocWriteFlush();
  
  int  stdClose() {return 0;}
  int  stdInit(uchar * ptr, 
               uint len, 
               PSStreamProc * pp, 
               int modesA = 0, 
               bool own_cbufA = false);
	int  stdNoAvailable(long * pl);
	int  stdNoSeek(long) {return ERRC;}  
	int  stdNull() {return 0;}
	int  stdReadFlush();
	void stdReadReset();
	int  stdSwitchMode(bool) {return ERRC;}  
	int  stdWriteFlush();
	void stdWriteReset();	
	int stringAvailable(long *pl);
	int stringReadSeek(long pos);
	int  stringReusableFlush();
	void stringReusableReset();
	int  stringWriteSeek(long pos);
	
	int  sswitch(bool writing);
  
  long tell();
  
  int ungetc(uchar c);
  
  int  write(const void *ptr, uint count);
  int  writeEncrypted(const void *ptr, uint count);
  void writeFontInfo(const char *key, const XWPSString *pvalue,	int do_write);
  void writeFile(QIODevice * fileA, 
                 uchar * buf, 
                 uint len, 
                 bool own_cbufA = false,
								 bool own_fileA = false);
  void writePositionOnly();
  int  writeProc(XWPSRef * sop);
  void writePSString(const uchar * str, uint size, int print_ok);
  void writeString(const uchar *ptr, uint len, bool own_cbufA = false);
  void writeUid(XWPSUid *puid);
  
public:
	PSStreamTemplate * templat;
	int min_left;
	char error_string[STREAM_MAX_ERROR_STRING + 1];
	
	XWPSStreamCursor cursor;
	
	uchar *cbuf;
	uint bsize;
	uint cbsize;
	short end_status;
	uchar foreign;
	uchar modes;
	XWPSString cbuf_string;
	long position;
	
	PSStreamProc procs;
	
	XWPSStream *strm;
	int is_temp;
	int inline_temp;	
	PSStreamState *state;	
	ushort read_id;
	ushort write_id;
	XWPSStream *prev, *next;
	bool close_strm;
	bool close_at_eod;
	
	int (XWPSStream::*save_close)();
	
	QIODevice * file;
	bool close_file;
	XWPSString file_name;
	uint file_modes;
	long file_offset;
	long file_limit;
	
	bool own_cbuf;
	bool own_file;
	bool own_ss;
	int refCount;
	XWPSBytes * cbuf_bytes;
    
private:
	void compact(bool always);
	
	int  pgetcc(bool closeateod);
	const char * printfScan(const char *format);
	int procInit(XWPSRef * sop, 
	             uint mode,
               const PSStreamTemplate * temp, 
               const PSStreamProc * procsA);
  int  pskip(long nskip, long *pskipped);
		
	int  readBuf(PSStreamCursorWrite * pbuf);	
	
	int  writeBuf(PSStreamCursorRead * pbuf, bool last);
};

#endif //XWPSSTREAM_H
