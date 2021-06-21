/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include <QFileInfo>
#include "XWApplication.h"
#include "XWStringUtil.h"
#include "XWFileName.h"
#include "XWPictureSea.h"
#include "XWFontSea.h"
#include "XWDocSea.h"
#include "XWPSError.h"
#include "XWDviPsSpecial.h"
#include "XWPSStream.h"
#include "XWPSIODevice.h"
#include "XWPSContextState.h"

int XWPSContextState::copyErrorString(XWPSRef *fop)
{
	XWPSStream *s;

  for (s = fop->getStream(); s->strm != 0 && s->state->error_string[0] == 0;)
		s = s->strm;
  if (s->state->error_string[0]) 
  {
		int code = errorInfoPutString(s->state->error_string);

		if (code < 0)
	    return code;
		s->state->error_string[0] = 0;
  }
  return (int)(XWPSError::IOError);
}

int XWPSContextState::execFileCleanup()
{
	int code = checkOStack(1);
	if (code < 0)
	    return code;
	    
	XWPSRef * op = op_stack.incCurrentTop(1);
	XWPSRef * esp = exec_stack.getCurrentTop();
  op->assign(esp + 2);
  return zcloseFile();
}

int XWPSContextState::execFileFinish()
{
	int code = checkOStack(1);
	if (code < 0)
	    return code;
	exec_stack.incCurrentTop(-2);
  execFileCleanup();
  return PS_O_POP_ESTACK;
}

int XWPSContextState::fileCleanup()
{
	XWPSRef * esp = exec_stack.getCurrentTop();
	esp[4].makeNull();
	return 0;
}

int XWPSContextState::fileContinue()
{
	XWPSRef * op = op_stack.getCurrentTop();
	XWPSRef * esp = exec_stack.getCurrentTop();
  XWPSRef * pscratch = esp - 2;
  XWPSFileEnum *pfen = (XWPSFileEnum*)(esp[-1].getStruct());
  uint len = pscratch->size();
  uint code =  pfen->next((char *)pscratch->getBytes(), len);

  if (code == ~(uint) 0) 
  {	
		esp = exec_stack.incCurrentTop(-4);
		return PS_O_POP_ESTACK;
  } 
  else if (code > len)
		return (int)(XWPSError::RangeCheck);
  else 
  {
		code = push(&op, 1);
		if (code < 0)
			return code;
		op->assign(pscratch);
		op->setSize(code);
		esp = exec_stack.incCurrentTop(1);
		esp->makeOper(0, &XWPSContextState::fileContinue);
		esp = exec_stack.incCurrentTop(1);
		esp->assign(pscratch + 2);
		return PS_O_PUSH_ESTACK;
  }
}

int XWPSContextState::fileReadString(const uchar *str, uint len, XWPSRef *pfile)
{
	XWPSStream *s = new XWPSStream;
  s->readString(str, len, false);
  s->foreign = 1;
  s->write_id = 0;
  pfile->makeFile(idmemory()->iallocSpace() | PS_A_READONLY, s->read_id, s);
  s->save_close = s->procs.close;
  s->procs.close = &XWPSStream::fileCloseDisable;
  return 0;
}

int XWPSContextState::fileSwitchToRead(XWPSRef * op)
{
	XWPSStream *s = op->getStream();

  if (s->write_id != op->size() || s->file == 0)
		return (int)(XWPSError::InvalidAccess);
	
  if (s->sswitch(false) < 0)
		return (int)(XWPSError::IOError);
			
  s->read_id = s->write_id;	
  s->write_id = 0;
  return 0;
}

int XWPSContextState::fileSwitchToWrite(XWPSRef * op)
{
	XWPSStream *s = op->getStream();

  if (s->read_id != op->size() || s->file == 0)
		return (int)(XWPSError::InvalidAccess);
  
  if (s->sswitch(true) < 0)
		return (int)(XWPSError::IOError);
			
  s->write_id = s->read_id;	
  s->read_id = 0;
  return 0;
}

int XWPSContextState::filterOpen(const char *file_access, 
	                uint buffer_size, 
	                XWPSRef * pfile,
	                PSStreamProc * procs, 
	                const PSStreamTemplate * templat,
	                const PSStreamState * st)
{
	XWPSStream *s;
	XWPSIODevice * iodev = io_device_table[0];
	int code = iodev->fileOpenStream(0, 0, file_access, buffer_size, &s);
	if (code < 0)
	{
		if (st)
		{
			PSStreamState * tst = (PSStreamState*)st;
			delete tst;
		}
  	return code;
  }
  s->stdInit(s->cbuf, s->bsize, procs, (*file_access == 'r' ? PS_STREAM_MODE_READ : PS_STREAM_MODE_WRITE), false);
  s->procs.process = templat->process;
  s->save_close = s->procs.close;
  s->procs.close = &XWPSStream::fileCloseFile;
  PSStreamState *sst = 0;
  
  if (st)
  {
  	sst = (PSStreamState*)st;
  	s->own_ss = true;
  }
  sst->templat = templat;
  s->state = sst;
  if (templat->init != 0)
  	code = (*(templat->init))(sst);
  	
  if (code < 0)
  	return code;
  makeStreamFile(pfile, s, file_access);
  return 0;
}

int XWPSContextState::handleIntc(XWPSRef *pstate, int nstate, op_proc_t cont)
{
	int npush = nstate + 2;

  int code = checkEStack(npush);
  if (code < 0)
  	return code;
  	
  XWPSRef * ep;
  if (nstate)
  {
  	ep = exec_stack.getCurrentTop();
  	memcpyRef(ep + 2, pstate, nstate);
  }
  
  npush--;
  ep = exec_stack.getCurrentTop();
  ep[1].makeOper(0, cont);
  ep = exec_stack.incCurrentTop(npush);
  return PS_O_PUSH_ESTACK;
}

int XWPSContextState::handleReadException(int status, 
	                        XWPSRef * fop,
                          XWPSRef * pstate, 
                          int nstate, 
                          op_proc_t cont)
{
	int npush = nstate + 4;
  XWPSStream *ps;

  switch (status) 
  {
    case INTC:
        return handleIntc(pstate, nstate, cont);
        
    case CALLC:
        break;
        
    default:
        return (int)(XWPSError::IOError);
  }
  
  for (ps = fop->getStream(); ps->strm != 0;)
    ps = ps->strm;
    
  int code = checkEStack(npush);
  if (code < 0)
  	return code;
  	
  XWPSRef * ep;
  if (nstate)
  {
  	ep = exec_stack.getCurrentTop();
    memcpyRef(ep + 2, pstate, nstate);
  }
  ep = exec_stack.getCurrentTop();
  ep[1].makeOper(0, cont);
  
  ep = exec_stack.incCurrentTop(npush);
  ep[-2].makeOper(0, &XWPSContextState::procReadContinue);
  ep[-1].assign(fop);
  ep[-1].clearAttrs(PS_A_EXECUTABLE);
  ep->assign(&((PSStreamProcState *) ps->state)->proc);
  return PS_O_PUSH_ESTACK;
}

int XWPSContextState::handleReadStatus(int ch, 
	                      XWPSRef * fop,
		                    const uint * pindex, 
		                    op_proc_t cont)
{
	switch (ch) 
	{
		default:
	    return copyErrorString(fop);
	    
		case EOFC:
	    return 1;
	    
		case INTC:
		case CALLC:
	    if (pindex) 
	   	{
				XWPSRef index;

				index.makeInt(*pindex);
				return handleReadException(ch, fop, &index, 1, cont);
	    } 
	    else
				return handleReadException(ch, fop, NULL, 0,  cont);
  }
}

int XWPSContextState::handleWriteException(int status, 
	                         XWPSRef * fop,
                           XWPSRef * pstate, 
                           int nstate, 
                           op_proc_t cont)
{
	XWPSStream *ps;
  PSStreamProcState *psst;

  switch (status) 
  {
    case INTC:
      return handleIntc(pstate, nstate, cont);
      
    case CALLC:
      break;
      
    default:
      return (int)(XWPSError::IOError);
  }
  
  for (ps = fop->getStream(); ps->strm != 0;)
    ps = ps->strm;
  psst = (PSStreamProcState *) ps->state;
  XWPSRef * ep;
  {
    int npush = nstate + 6;

    checkEStack(npush);
    if (nstate)
    {
    	ep = exec_stack.getCurrentTop();
      memcpyRef(ep + 2, pstate, nstate);
    }
    
    ep = exec_stack.getCurrentTop();
    ep[1].makeOper(0, cont);
    ep = exec_stack.incCurrentTop(npush);
    ep[-4].makeOper(0, &XWPSContextState::procWriteContinue);
    ep[-3].assign(fop);
    ep[-3].clearAttrs(PS_A_EXECUTABLE);
    ep[-1].makeBool(!psst->eof);
 	}
  ep[-2].assign(&psst->proc);
  ep->assign(&psst->data);
  ep->setSize(psst->index);
  return PS_O_PUSH_ESTACK;
}

int XWPSContextState::handleWriteStatus(int ch, 
	                      XWPSRef * fop,
		                    const uint * pindex, 
		                    op_proc_t cont)
{
	switch (ch) 
	{
		default:
	    return copyErrorString(fop);
	    
		case EOFC:
	    return 1;
	    
		case INTC:
		case CALLC:
	    if (pindex) 
	    {
				XWPSRef index;

				index.makeInt(*pindex);
				return handleWriteException(ch, fop, &index, 1, cont);
	    } 
	    else
				return handleWriteException(ch, fop, NULL, 0,	cont);
  }
}

QIODevice * XWPSContextState::libFOpen(const char *bname)
{
  int code;
  QIODevice * f = 0;
  int l = 1024;
  char buffer[1024];

  XWPSIODevice * iodev = io_device_table[0];
  code = iodev->fopenLibFile(bname, "r", &f,	 buffer, l);
  return f;
}

int XWPSContextState::libFileOpen(const char *fname, 
	                uint len, 
	                uchar * cname, 
	                uint max_clen,
	                 uint * pclen, 
	                 XWPSRef * pfile)
{
	XWPSIODevice * iodev = io_device_table[0];
	XWPSStream *s;
  int code = iodev->fileOpenStream(fname, len, "r", 512, &s);
  char *bname;
  uint blen;

  if (code < 0)
		return code;
  bname = (char *)s->cbuf;
  blen = strlen(bname);
  if (blen > max_clen) 
  {
		s->close();
		return (int)(XWPSError::LimitCheck);
  }
  memcpy(cname, bname, blen);
  *pclen = blen;
  makeStreamFile(pfile, s, "r");
  return 0;
}

void XWPSContextState::makeInvalidFile(XWPSRef * fp)
{
	invalid_file_stream.incRef();
	fp->makeFile(PS_AVM_FOREIGN, ~0, &invalid_file_stream);
}

int  XWPSContextState::makeRFS(XWPSRef * op, XWPSStream *fs, long offset, long length)
{
	XWPSString fname;
  XWPSStream *s;
  int code;

  if (fs->fileName(&fname) < 0)
		return (int)(XWPSError::IOError);
	
  if (fname.data[0] == '%')
		return (int)(XWPSError::InvalidFileAccess);
  
  XWPSIODevice * iodev = io_device_table[0];
  code = iodev->fileOpenStream((const char *)fname.data, fname.size, "r", fs->cbsize, &s);
  if (code < 0)
		return code;
  if (s->readSubfile(offset, length) < 0) 
  {
		s->close();
		return (int)(XWPSError::IOError);
  }
  s->close_at_eod = false;
  makeStreamFile(op, s, "r");
  return 0;
}

int  XWPSContextState::makeRSS(XWPSRef * op, 
	             const uchar * data, 
	             uint size,
	             int , 
	             long offset, 
	             long length, 
	             bool is_bytestring)
{
	XWPSStream *s;
  long left = qMin(length, (long)(size - offset));

  s = new XWPSStream;
  long l = qMax(left, (long)0);
  s->readStringReusable(data + offset, l, false);
  if (is_bytestring)
		s->cbuf_string.data = 0;	/* byte array, not string */
  makeStreamFile(op, s, "r");
  return 0;
}

void XWPSContextState::makeStreamFile(XWPSRef * pfile, XWPSStream * s, const char *access)
{
	uint attrs = (access[1] == '+' ? PS_A_WRITE + PS_A_READ + PS_A_EXECUTE : 0) | iimemoryLocal()->space;

  if (access[0] == 'r') 
  {
		pfile->makeFile(attrs | (PS_A_READ | PS_A_EXECUTE), s->read_id, s);
		s->write_id = 0;
  } 
  else 
  {
		pfile->makeFile(attrs | PS_A_WRITE, s->write_id, s);
		s->read_id = 0;
  }
}

int XWPSContextState::parseFileName(XWPSRef * op, XWPSParsedFileName * pfn)
{
	int code = op->checkReadType(XWPSRef::String);
	if (code < 0)
  	return code;
  return pfn->parseFileName(this, (const char *)op->getBytes(), op->size());
}

int XWPSContextState::parseRealFileName(XWPSRef *op, XWPSParsedFileName *pfn)
{
	int code = op->checkReadType(XWPSRef::String);
	if (code < 0)
  	return code;
  return pfn->parseRealFileName(this, (const char *)op->getBytes(), op->size());
}

int XWPSContextState::procReadContinue()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * opbuf = op - 1;
  XWPSStream *ps;
  PSStreamProcState *ss;

  int code = op->checkFile(&ps);
  if (code < 0)
  	return code;
  	
  code = opbuf->checkReadType(XWPSRef::String);
  if (code < 0)
  	return code;
  	
  while ((ps->end_status = 0, ps->strm) != 0)
        ps = ps->strm;
  ss = (PSStreamProcState *) ps->state;
  ss->data.assign(opbuf);
  ss->index = 0;
  if (opbuf->size() == 0)
    ss->eof = true;
  pop(2);
  return 0;
}

int XWPSContextState::procWriteContinue()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * opbuf = op - 1;
  XWPSStream *ps;
  PSStreamProcState *ss;

  int code = op->checkFile(&ps);
  if (code < 0)
  	return code;
  	
  code = opbuf->checkWriteType(XWPSRef::String);
  while (ps->strm != 0) 
  {
		if (ps->end_status == CALLC)
	    ps->end_status = 0;
		ps = ps->strm;
  }
  ps->end_status = 0;
  ss = (PSStreamProcState *) ps->state;
  ss->data.assign(opbuf);
  ss->index = 0;
  pop(2);
  return 0;
}

int XWPSContextState::writeString(XWPSRef * op, XWPSStream * s)
{
	uchar *data = op->getBytes();
  uint len = op->size();
  uint wlen;
  int status = s->puts(data, len, &wlen);

  switch (status) 
  {
		case INTC:
		case CALLC:
	    op->setBytesPtr(data + wlen);
	    op->setSize(len - wlen);
	    
		default:	
	    return status;
  }
}

int XWPSContextState::zbytesAvailable()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSStream *s;
  long avail;

  int code = op->checkReadType(XWPSRef::File);
  if (code < 0)
  	return code;
  	
  s = op->getStream();
  if (s->read_id != op->size())
  {
  	if (s->read_id == 0 && s->write_id == op->size())
  	{
  		code = fileSwitchToRead(op);
  		if (code < 0)
  			return code;
  	}
  	else
  		s = &invalid_file_stream;
  }
  switch (s->available(&avail)) 
  {
		default:
	    return (int)(XWPSError::IOError);
	    
		case EOFC:
	    avail = -1;
	    
		case 0:
	    ;
  }
  op->makeInt(avail);
  return 0;
}

int XWPSContextState::zcloseFile()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSStream *s;

  int code = op->checkType(XWPSRef::File);
  if (code < 0)
  	return code;
  	
  if (op->fileIsValid(&s)) 
  {	
		int status = s->close();

		if (status != 0 && status != EOFC) 
		{
	    if (s->isWriting())
				return handleWriteStatus(status, op, NULL,  &XWPSContextState::zcloseFile);
	    else
				return handleReadStatus(status, op, NULL, &XWPSContextState::zcloseFile);
		}
  }
  pop(1);
  return 0;
}

int XWPSContextState::zdeleteFile()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSParsedFileName pname;
  int code = parseRealFileName(op, &pname);

  if (code < 0)
		return code;
    
  code = pname.iodev->deleteFile(pname.fname);
  if (code < 0)
		return code;
  pop(1);
  return 0;
}

int XWPSContextState::zecho()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = op->checkType(XWPSRef::Boolean);
  if (code < 0)
  	return code;
  pop(1);
  return 0;
}

int XWPSContextState::zexecFile()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = op->checkTypeAccess(XWPSRef::File, PS_A_EXECUTABLE | PS_A_READ | PS_A_EXECUTE);
  if (code < 0)
  	return code;
  	
  code = checkEStack(4);
  if (code < 0)
  	return code;
  	
  XWPSRef * esp = exec_stack.incCurrentTop(1);
  esp->makeMarkEStack(PS_ES_OTHER, &XWPSContextState::fileCleanup);
  esp = exec_stack.incCurrentTop(1);
  esp->assign(op);
  esp = exec_stack.incCurrentTop(1);
  esp->makeOper(0, &XWPSContextState::execFileFinish);
  return zexec();
}

int XWPSContextState::zfile()
{
	XWPSRef * op = op_stack.getCurrentTop();
  char file_access[3];
  XWPSParsedFileName pname;
  uchar *astr;
  int code;
  XWPSStream *s;

  code = op->checkReadType(XWPSRef::String);
  if (code < 0)
  	return code;
  	
  astr = op->getBytes();
  switch (op->size()) 
  {
		case 2:
	    if (astr[1] != '+')
				return (int)(XWPSError::InvalidFileAccess);
	    file_access[1] = '+';
	    file_access[2] = 0;
	    break;
	    
		case 1:
	    file_access[1] = 0;
	    break;
	    
		default:
			return (int)(XWPSError::InvalidFileAccess);
  }
  switch (astr[0]) 
  {
		case 'r':
		case 'w':
		case 'a':
	    break;
	    
		default:
			return (int)(XWPSError::InvalidFileAccess);
  }
  file_access[0] = astr[0];
  code = parseFileName(op - 1, &pname);
  if (code < 0)
		return code;
		
  if (pname.iodev && pname.iodev->dtype == iodev_dtype_stdio) 
  {
		if (pname.fname)
			return (int)(XWPSError::InvalidFileAccess);
		pname.iodev->state = this;
		code = pname.iodev->open(file_access, &s);
  } 
  else 
  {
		if (pname.iodev == NULL)
	    pname.iodev = getIODevice(0);
		code = zopenFile(&pname, file_access, &s);
  }
    
  if (code < 0)
		return code;
  code = s->setFileName(op[-1].getBytes(), op[-1].size());
  if (code < 0) 
  {
		s->close();
		return (int)(XWPSError::VMError);
  }
  makeStreamFile(op - 1, s, file_access);
  pop(1);
  return code;
}

int XWPSContextState::zfileInit()
{
	XWPSStream *const s = &invalid_file_stream;

  s->init();
  s->readString(0,0,false);
  s->next = s->prev = 0;
  s->read_id = 0;
  s->write_id = 0;
  return 0;
}

int XWPSContextState::zfileName()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSStream *s;
  XWPSString fname;

  int code = op->checkType(XWPSRef::File);
  if (code < 0)
  	return code;
  	
  s = op->getStream();
  if ((s->read_id | s->write_id) != op->size())
		return (int)(XWPSError::InvalidAccess);
  
  if (s->fileName(&fname) < 0) 
  {
		op->makeFalse();
		return 0;
  }
  checkOStack(1);
  code = push(&op, 1);
  if (code < 0)
		return code;
  op[-1].makeString(PS_A_ALL, fname.size, fname.data);
  op->makeTrue();
  return 0;
}

int XWPSContextState::zfileNameDirSeparator()
{
	XWPSRef * op = op_stack.getCurrentTop();
  const char *sepr;

  int code = op->checkReadType(XWPSRef::String);
  if (code < 0)
  	return code;
  	
  code = op[-1].checkReadType(XWPSRef::String);
  if (code < 0)
  	return code;
  	
  
  sepr = "/";
  uchar * p = op[-1].getBytes();
  if (op[-1].size() > 0 && (p[op[-1].size() - 1] == '/'))
  	sepr = "";
  
  op[-1].makeString(PS_AVM_FOREIGN | PS_A_READONLY, strlen(sepr), (uchar *)sepr);
  pop(1);
  return 0;
}

int XWPSContextState::zfileNameForall()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSFileEnum *pfen;
  int code;

  code = op->checkWriteType(XWPSRef::String);
  if (code < 0)
  	return code;
  	
  code = op[-1].checkProc();
  if (code < 0)
  	return code;
  	
  code = op[-2].checkReadType(XWPSRef::String);
  if (code < 0)
  	return code;
  	
  code = checkEStack(7);
  if (code < 0)
  	return code;
  	
  pfen = new XWPSFileEnum((char *)op[-2].getBytes(), op[-2].size());
  XWPSRef * esp = exec_stack.incCurrentTop(1);
  esp->makeMarkEStack(PS_ES_FOR, &XWPSContextState::fileCleanup);
  esp = exec_stack.incCurrentTop(1);
  esp->assign(op - 2);
  esp = exec_stack.incCurrentTop(1);
  esp->assign(op);
  esp = exec_stack.incCurrentTop(1);
  esp->makeStruct(currentSpace() | 0, pfen);
  esp = exec_stack.incCurrentTop(1);
  esp->assign(op - 1);
  pop(3);
  code = fileContinue();
  return (code == PS_O_POP_ESTACK ? PS_O_POP_ESTACK : code);
}

int XWPSContextState::zfileNameListSeparator()
{
	char c = envSep().toAscii();
	uchar str[2];
	str[0] = c;
	str[1] = 0;
	XWPSRef * op = op_stack.getCurrentTop();

  int code = push(&op, 1);
  if (code < 0)
		return code;
  op->makeString(PS_AVM_FOREIGN | PS_A_READONLY, 1, str);
  return 0;
}

int XWPSContextState::zfileNameSplit()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = op->checkReadType(XWPSRef::String);
	if (code < 0)
  	return code;
    
  return (XWPSError::Undefined);
}

int XWPSContextState::zfilePosition()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSStream *s;

  int code = op->checkType(XWPSRef::File);
  if (code < 0)
  	return code;
  	
  s = op->getStream();
  if ((s->read_id | s->write_id) != op->size())
		return (int)(XWPSError::InvalidAccess);
  
  if (!s->canSeek())
		return (int)(XWPSError::IOError);
    
  op->makeInt(s->tell());
  return 0;
}

int XWPSContextState::zflush()
{
  XWPSStream *s;
  int code = zgetStdout(&s);

  if (code < 0)
		return code;
	s->flush();
  return 0;
}

int XWPSContextState::zflushFile()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSStream *s;
  int status;

  int code = op->checkType(XWPSRef::File);
  if (code < 0)
  	return code;
  	
  if (op->fileIsInvalid(&s)) 
  {
		if (op->hasAttr(PS_A_WRITE))
			return (int)(XWPSError::InvalidAccess);
		pop(1);
		return 0;
  }
  status = s->flush();
  if (status == 0 || status == EOFC) 
  {
		pop(1);
		return 0;
  }
  return (s->isWriting() ?	 handleWriteStatus(status, op, NULL, &XWPSContextState::zflushFile) :
	 				handleReadStatus(status, op, NULL, &XWPSContextState::zflushFile));
}

int XWPSContextState::zgetStderr(XWPSStream ** ps)
{
	XWPSStream *s;
  XWPSIODevice *iodev;
  int code;

  if (stdio[2].fileIsValid(&s)) 
  {
		*ps = s;
		return 0;
  }
    
  iodev = findIODevice((const uchar *)"%stderr", 7);
  iodev->state = this;
  code = iodev->open("w", ps);
  return qMin(code, 0);
}

int XWPSContextState::zgetStdin(XWPSStream ** ps)
{
	XWPSStream *s;
  XWPSIODevice *iodev;
  int code;

  if (stdio[0].fileIsValid(&s))
  {
		*ps = s;
		return 0;
  }
  
  iodev = findIODevice((const uchar *)"%stdin", 6);
  iodev->state = this;
  code = iodev->open("r", ps);
  return qMin(code, 0);
}

int XWPSContextState::zgetStdout(XWPSStream ** ps)
{
	XWPSStream *s;
  XWPSIODevice *iodev;
  int code;

  if (stdio[1].fileIsValid(&s)) 
  {
		*ps = s;
		return 0;
  }
  iodev = findIODevice((const uchar *)"%stdout", 7);
  iodev->state = this;
  code = iodev->open("w", ps);
  return qMin(code, 0);
}

int XWPSContextState::zisProcFilter()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSStream *s;
  
  int code = op->checkType(XWPSRef::File);
  if (code < 0)
  	return code;
  	
  s = op->getStream();
  if ((s->read_id | s->write_id) != op->size())
		return (int)(XWPSError::InvalidAccess);

  while (s->strm != 0)
		s = s->strm;
  op->makeBool(s->isProc());
  return 0;
}

int XWPSContextState::zlibFile()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code;
#define MAX_CNAME 200
  uchar cname[MAX_CNAME];
  uint clen;
  XWPSParsedFileName pname;
  XWPSStream *s;

  checkOStack(2);
  code = parseFileName(op, &pname);
  if (code < 0)
		return code;
  
  if (pname.iodev == NULL)
		pname.iodev = getIODevice(0);
  
  if (pname.iodev != getIODevice(0)) 
  {
		code = zopenFile(&pname, "r", &s);
		if (code >= 0) 
		{
	    code = s->setFileName(op->getBytes(), op->size());
	    if (code < 0) 
	    {
				s->close();
				return (int)(XWPSError::VMError);
	    }
		}
		if (code < 0) 
		{
	    code = push(&op, 1);
	    if (code < 0)
				return code;
	    op->makeFalse();
	    return 0;
		}
		makeStreamFile(op, s, "r");
  } 
  else 
  {
		XWPSRef fref;

		code = libFileOpen(pname.fname, pname.len, cname, MAX_CNAME, &clen, &fref);
		if (code >= 0) 
		{
	    s = fref.getStream();
	    code = s->setFileName(cname, clen);
	    if (code < 0) 
	    {
				s->close();
				return (int)(XWPSError::VMError);
	    }
		}
		if (code < 0) 
		{
	    if (code == XWPSError::VMError)
				return code;
	    code = push(&op, 1);
	    if (code < 0)
				return code;
	    op->makeFalse();
	    return 0;
		}
		op->assign(&fref);
  }
  code = push(&op, 1);
  if (code < 0)
		return code;
  op->makeTrue();
  return 0;
}

int XWPSContextState::zopenFile(const XWPSParsedFileName *pfn, const char *file_access, XWPSStream **ps)
{
	XWPSIODevice * iodev = pfn->iodev;

  if (pfn->fname == NULL)	
		return iodev->open(file_access, ps);
  else 
		return iodev->openFile(pfn->fname, pfn->len, file_access, ps);
}

int XWPSContextState::zpeekString()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSStream *s;
  uint len, rlen;

  int code = op[-1].checkReadType(XWPSRef::File);
  if (code < 0)
  	return code;
  	
  s = op[-1].getStream();
  if (s->read_id != op[-1].size())
  {
  	if (s->read_id == 0 && s->write_id == op[-1].size())
  	{
  		code = fileSwitchToRead(op - 1);
  		if (code < 0)
  			return code;
  	}
  	else
  		s = &invalid_file_stream;
  }
  
  code = op->checkWriteType(XWPSRef::String);
  if (code < 0)
  	return code;
  	
  len = op->size();
  while ((rlen = s->bufAvailable()) < len) 
  {
		int status = s->end_status;
		if (len >= s->bsize)
			return (int)(XWPSError::RangeCheck);
	  
		switch (status) 
		{
			case EOFC:
	    	break;
	    	
			case 0:
	    	s->processReadBuf();
	    	continue;
	    	
			default:
	    	return handleReadStatus(status, op - 1, &rlen, &XWPSContextState::zpeekString);
		}
		break;
  }
  if (rlen > len)
		rlen = len;
		
  memcpy(op->getBytes(), s->bufPtr(), rlen);
  op->setSize(rlen);
  op[-1].assign(op);
  op->makeBool((rlen == len ? 1 : 0));
  return 0;
}

int XWPSContextState::zprint()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSStream *s;
  int status;
  XWPSRef rstdout;
  int code;

  code = op->checkReadType(XWPSRef::String);
  if (code < 0)
  	return code;
  	
  code = zgetStdout(&s);
  if (code < 0)
		return code;
  status = writeString(op, s);
  if (status >= 0) 
  {
		pop(1);
		return 0;
   }
 
  makeStreamFile(&rstdout, s, "w");
  code = handleWriteStatus(status, &rstdout, NULL, &XWPSContextState::zwriteString);
  if (code != PS_O_PUSH_ESTACK)
		return code;
    
  code = push(&op, 1);
  if (code < 0)
		return code;
  op->assign(&op[-1]);
  op[-1].assign(&rstdout);
  return code;
}

int XWPSContextState::zread()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSStream *s;
  int ch;

  int code = op->checkReadType(XWPSRef::File);
  if (code < 0)
  	return code;
  	
  s = op->getStream();
  if (s->read_id != op->size())
  {
  	if (s->read_id == 0 && s->write_id == op->size())
  	{
  		code = fileSwitchToRead(op);
  		if (code < 0)
  			return code;
  	}
  	else
  		s = &invalid_file_stream;
  }
  ch = s->getc();
  if (ch >= 0) 
  {
		code = push(&op, 1);
		if (code < 0)
			return code;
		op[-1].makeInt(ch);
		op->makeBool(1);
  } 
  else if (ch == EOFC)
		op->makeBool(0);
  else
		return handleReadStatus(ch, op, NULL, &XWPSContextState::zread);
  return 0;
}

int XWPSContextState::zreadHexString()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = op->checkWriteType(XWPSRef::String);
  if (code < 0)
  	return code;
  	
  if (op->size() > 0)
		*op->getBytes() = 0x10;
  return zreadHexStringAt(op, 0);
}

int XWPSContextState::zreadHexStringAt(XWPSRef * op, uint start)
{
	XWPSStream *s;
  uint len, nread;
  uchar *str;
  int odd;
  PSStreamCursorWrite cw;
  int status;

	int code = op[-1].checkReadType(XWPSRef::File);
  if (code < 0)
  	return code;
  	
  s = op[-1].getStream();
  if (s->read_id != op[-1].size())
  {
  	if (s->read_id == 0 && s->write_id == op[-1].size())
  	{
  		code = fileSwitchToRead(op - 1);
  		if (code < 0)
  			return code;
  	}
  	else
  		s = &invalid_file_stream;
  }
    
  str = op->getBytes();
  len = op->size();
  if (start < len) 
  {
		odd = str[start];
		if (odd > 0xf)
	    odd = -1;
  } 
  else
		odd = -1;
  cw.ptr = str + start - 1;
  cw.limit = str + len - 1;
  for (;;) 
  {
		status = s_hex_process(&s->cursor.r, &cw, &odd, hex_ignore_garbage);
		if (status == 1) 
		{	
	    op[-1].assign(op);
	    op->makeTrue();
	    return 0;
		} 
		else if (status != 0)	
	    break;
	    
		status = s->pgetc();
		if (status < 0)
	    break;
		s->putBack();
  }
  nread = cw.ptr + 1 - str;
  if (status != EOFC) 
  {	
		if (nread < len)
	    str[nread] = (odd < 0 ? 0x10 : odd);
		return handleReadStatus(status, op - 1, &nread,  &XWPSContextState::zreadHexStringContinue);
  }
  
  op[-1].assign(op);
  op[-1].setSize(nread);
  op->makeFalse();
  return 0;
}

int XWPSContextState::zreadHexStringContinue()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code;

  code = op->checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
  	
  if (op->value.intval < 0 || op->value.intval > op[-1].size())
		return (int)(XWPSError::RangeCheck);
	
  code = op[-1].checkWriteType(XWPSRef::String);
  if (code < 0)
  	return code;
  	
  code = zreadHexStringAt(op - 1, (uint) op->value.intval);
  if (code >= 0)
		pop(1);
  return code;
}

int XWPSContextState::zreadLine()
{
	XWPSRef * op = op_stack.getCurrentTop();

  return zreadLineAt(op, 0, false);
}

int XWPSContextState::zreadLineAt(XWPSRef * op, uint count, bool in_eol)
{
	XWPSStream *s;
  int status;
  XWPSString str;

  int code = op[-1].checkReadType(XWPSRef::File);
  if (code < 0)
  	return code;
  	
  s = op[-1].getStream();
  if (s->read_id != op[-1].size())
  {
  	if (s->read_id == 0 && s->write_id == op[-1].size())
  	{
  		code = fileSwitchToRead(op - 1);
  		if (code < 0)
  			return code;
  	}
  	else
  		s = &invalid_file_stream;
  }
  
  code = op->checkWriteType(XWPSRef::String);
  if (code < 0)
  	return code;
  	
  str.data = op->getBytes();
  str.size = op->size();
  status = zreadLineFrom(s, &str, &count, &in_eol);
  switch (status) 
  {
		case 0:
		case EOFC:
	    break;
	    
		case 1:
			return (int)(XWPSError::RangeCheck);
	    
		default:
	    if (count == 0 && !in_eol)
				return handleReadStatus(status, op - 1, NULL,  &XWPSContextState::zreadLine);
	    else 
	    {
				if (in_eol) 
				{
		    	op->setSize(count);
		    	count = 0;
				}
				return handleReadStatus(status, op - 1, &count, &XWPSContextState::zreadLineContinue);
	    }
  }
  op->setSize(count);
  op[-1].assign(op);
  op->makeBool(status == 0);
  return 0;
}

int XWPSContextState::zreadLineContinue()
{
	XWPSRef * op = op_stack.getCurrentTop();
  uint size = op[-1].size();
  uint start;
  int code;

  code = op->checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
  	
  if (op->value.intval < 0 || op->value.intval > size)
		return (int)(XWPSError::RangeCheck);
	
  start = (uint) op->value.intval;
  code = (start == 0 ? zreadLineAt(op - 1, size, true) :  zreadLineAt(op - 1, start, false));
  if (code >= 0)
		pop(1);
  return code;
}

int XWPSContextState::zreadLineFrom(XWPSStream *s, XWPSString *buf, uint *pcount, bool *pin_eol)
{
	return s->readLine(NULL, NULL , NULL, buf,   pcount, pin_eol);
}

int XWPSContextState::zreadString()
{
	XWPSRef * op = op_stack.getCurrentTop();

  return zreadStringAt(op, 0);
}

int XWPSContextState::zreadStringAt(XWPSRef * op, uint start)
{
	XWPSStream *s;
  uint len, rlen;
  int status;

  int code = op[-1].checkReadType(XWPSRef::File);
  if (code < 0)
  	return code;
  	
  s = op[-1].getStream();
  if (s->read_id != op[-1].size())
  {
  	if (s->read_id == 0 && s->write_id == op[-1].size())
  	{
  		code = fileSwitchToRead(op - 1);
  		if (code < 0)
  			return code;
  	}
  	else
  		s = &invalid_file_stream;
  }
  
  code = op->checkWriteType(XWPSRef::String);
  if (code < 0)
  	return code;
  len = op->size();
  status = s->gets(op->getBytes() + start, len - start, &rlen);
  rlen += start;
  switch (status) 
  {
		case EOFC:
		case 0:
	    break;
	    
		default:
	    return handleReadStatus(status, op - 1, &rlen,  &XWPSContextState::zreadStringContinue);
  }
    
  if (len == 0)
		return (int)(XWPSError::RangeCheck);
	
  op->setSize(rlen);
  op[-1].assign(op);
  op->makeBool((rlen == len ? 1 : 0));
  return 0;
}

int XWPSContextState::zreadStringContinue()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code;

  code = op->checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
  	
  if (op->value.intval < 0 || op->value.intval > op[-1].size())
		return (int)(XWPSError::RangeCheck);
  code = zreadStringAt(op - 1, (uint) op->value.intval);
  if (code >= 0)
		pop(1);
  return code;
}

int XWPSContextState::zrenameFile()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSParsedFileName pname1, pname2;
  int code = parseRealFileName(op - 1, &pname1);

  if (code < 0)
		return code;
    
  code = parseRealFileName(op, &pname2);
  if (code < 0 || pname1.iodev != pname2.iodev ||
		(code = pname1.iodev->renameFile(pname1.fname, pname2.fname)) < 0) 
	{
		if (code >= 0)
	    code = (XWPSError::InvalidAccess);
  }
  
  if (code < 0)
		return code;
  pop(2);
  return 0;
}

int XWPSContextState::zresetFile()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSStream *s;

  int code = op->checkType(XWPSRef::File);
  if (code < 0)
  	return code;
  if (op->fileIsValid(&s))
		s->reset();
  pop(1);
  return 0;
}

int XWPSContextState::zreusableStream()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef * source_op = op - 1;
  long length = max_long;
  bool close_source;
  int code;

  code = op->checkType(XWPSRef::Boolean);
  if (code < 0)
  	return code;
  	
  close_source = op->value.boolval;
  if (source_op->hasType(XWPSRef::String)) 
  {
		uint size = source_op->size();

		code = source_op->checkRead();
		if (code < 0)
  		return code;
  	
  	XWPSBytes * b = source_op->value.bytes->arr;
  	if (b)
  		b->incRef();
		code = makeRSS(source_op, source_op->getBytes(),	size, source_op->space(), 0L, size, false);
		source_op->value.pfile->cbuf_bytes = b;
  } 
  else if (source_op->hasType(XWPSRef::AStruct)) 
  {
		source_op->checkRead();
		const char * tname = source_op->getTypeName();
		if (strcmp(tname, "bytes"))
			return (int)(XWPSError::RangeCheck);
				
		XWPSBytes * b = (XWPSBytes*)(source_op->value.pstruct);
		if (b)
		  b->incRef();
		code = makeRSS(source_op,	(const uchar *)(b->arr), b->length, source_op->space(), 0L, b->length, true);
		source_op->value.pfile->cbuf_bytes = b;
  } 
  else 
  {
		long offset = 0;
		XWPSStream *source;
		XWPSStream *s;

		source = source_op->getStream();
		if (source->read_id != source_op->size())
		{
			if (source->read_id == 0 && source->write_id == source_op->size())
			{
				int fcode = fileSwitchToRead(source_op);
				if (fcode < 0)
					return fcode;
			}
			else
				source = &invalid_file_stream;
		}
		s = source;
rs:
		if (s->cbuf_string.data != 0) 
		{
	    long pos = s->tell();
	    long avail = s->bufAvailable() + pos;

	    offset += pos;
	    XWPSBytes * b = s->cbuf_bytes;
	    if (b)
	    	b->incRef();
	    code = makeRSS(source_op, s->cbuf_string.data,   s->cbuf_string.size,0, offset, qMin(avail, length), false);
	    source_op->value.pfile->cbuf_bytes = b;
		} 
		else if (s->file != 0) 
		{
	    if (~s->modes & (PS_STREAM_MODE_READ | PS_STREAM_MODE_SEEK))
				return (int)(XWPSError::IOError);
	    code = makeRFS(source_op, s, offset + s->tell(), length);
		} 
		else if (s->state->templat == &s_SFD_template) 
		{
	    const PSStreamSFDState *const sfd_state =	(const PSStreamSFDState *)s->state;

	    if (sfd_state->eod.size != 0)
				return (int)(XWPSError::RangeCheck);
	    offset += sfd_state->skip_count - s->bufAvailable();
	    if (sfd_state->count != 0) 
	    {
				long left = qMax(sfd_state->count, (long)0) + s->bufAvailable();

				if (left < length)
		    	length = left;
	    }
	    s = s->strm;
	    goto rs;
		}
		else
				return (int)(XWPSError::RangeCheck);
		if (close_source) 
		{
	    XWPSStream *rs = source_op->getStream();

	    rs->strm = source;
	    rs->close_strm = true;
		}
  }
  if (code >= 0)
		pop(1);
  return code;
}

int XWPSContextState::zrsdParams()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSRef *pFilter;
  XWPSRef *pDecodeParms;
  int Intent;
  bool AsyncRead;
  XWPSRef empty_array, filter1_array, parms1_array;
  uint i;
  int code;

  empty_array.makeArray(PS_A_READONLY);
  if (op->dictFindString(this, "Filter", &pFilter) > 0) 
  {
		if (!pFilter->isArray()) 
		{
	    if (!pFilter->hasType(XWPSRef::Name))
				return (int)(XWPSError::TypeCheck);
	    filter1_array.makeArray(PS_A_READONLY, 1, pFilter);
	    pFilter = &filter1_array;
		}
  } 
  else
		pFilter = &empty_array;
		
  if (pFilter != &empty_array &&	op->dictFindString(this, "DecodeParms", &pDecodeParms) > 0) 
  {
		if (pFilter == &filter1_array) 
		{
	    parms1_array.makeArray(PS_A_READONLY, 1, pDecodeParms);
	    pDecodeParms = &parms1_array;
		} 
		else if (!pDecodeParms->isArray())
			return (int)(XWPSError::TypeCheck);
		else if (pFilter->size() != pDecodeParms->size())
			return (int)(XWPSError::RangeCheck);
  } 
  else
		pDecodeParms = 0;
    
  for (i = 0; i < pFilter->size(); ++i) 
  {
		XWPSRef f, fname, dp;

		pFilter->arrayGet(this, (long)i, &f);
		if (!f.hasType(XWPSRef::Name))
			return (int)(XWPSError::TypeCheck);
		nameStringRef(&f, &fname);
		if (fname.size() < 6 || memcmp(fname.getBytes() + fname.size() - 6, "Decode", 6))
			return (int)(XWPSError::RangeCheck);
		if (pDecodeParms) 
		{
	    pDecodeParms->arrayGet(this, (long)i, &dp);
	    if (!(dp.hasType(XWPSRef::Dictionary) || dp.hasType(XWPSRef::Null)))
				return (int)(XWPSError::TypeCheck);
		}
  }
  if ((code = op->dictIntParam(this, "Intent", 0, 3, 0, &Intent)) < 0 ||
			(code = op->dictBoolParam(this, "AsyncRead", false, &AsyncRead)) < 0)
		return code;
  code = push(&op, 1);
  if (code < 0)
		return code;
  op[-1].assign(pFilter);
  if (pDecodeParms)
		op->assign(pDecodeParms);
  else
		op->makeNull();
  return 0;
}

int XWPSContextState::zsetFilePosition()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSStream *s;
  
  int code = op[-1].checkType(XWPSRef::File);
  if (code < 0)
  	return code;
  	
  s = op[-1].getStream();
  if ((s->read_id | s->write_id) != op[-1].size())
		return (int)(XWPSError::InvalidAccess);

  code = op->checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
  	
  if (s->seek(op->value.intval) < 0)
		return (int)(XWPSError::IOError);
    
  pop(2);
  return 0;
}

int XWPSContextState::zstatus()
{
	XWPSRef * op = op_stack.getCurrentTop();

  switch (op->type()) 
  {
		case XWPSRef::File:
	    {
				XWPSStream *s;
				bool b = false;
				if (op->fileIsValid(&s))
					b = true;

				op->makeBool(b);
	    }
	    return 0;
	    
		case XWPSRef::String:
	    {
				XWPSParsedFileName pname;
				int code = parseFileName(op, &pname);

				if (code < 0)
		    	return code;
				code = pname.terminateFileName(this);
				if (code < 0)
		    	return code;
		    	
		    QString fn = QFile::decodeName(pname.fname);
		    XWGhostscriptSea sea;
  			QString afn = sea.findFile(fn);
  			if (afn.isEmpty())
  			{
  				XWFontSea fntsea;
  				afn = fntsea.findFile(fn);  				
  				if (afn.isEmpty())
  				{
  					XWPictureSea picsea;
  					afn = picsea.findFile(fn);
  					if (afn.isEmpty())
  					{
  						XWDviPsSea dvipssea;
  						afn = dvipssea.findFile(fn);
  						if (afn.isEmpty())
  						{
  							XWDocSea dsea;
  							afn = dsea.findFile(fn);
  						}
  					}
  				}
  			}
  			
  			if (afn.isEmpty())
  			{
  				XWFileName fpath(fn);
  				if (fpath.isAbsolute())
  				{
  					QString filename = fpath.fileName();
  					afn = sea.findFile(filename);
  					if (afn.isEmpty())
  					{
  						XWFontSea fntsea;
  						afn = fntsea.findFile(filename);  						
  						if (afn.isEmpty())
  						{
  							XWPictureSea picsea;
  							afn = picsea.findFile(filename);
  							if (afn.isEmpty())
  							{
  								XWDviPsSea dvipssea;
  								afn = dvipssea.findFile(filename);
  								if (afn.isEmpty())
  								{
  									XWDocSea dsea;
  									afn = dsea.findFile(filename);
  								}
  							}
  						}
  					}
  				}
  			}
  
  			if (afn.isEmpty())
  			{
  				op->makeBool(false);
		    	code = 0;
		    	return code;
  			}
  		
		    QFileInfo info(afn);
		    
		    if (info.isFile())
		    {
		    	checkOStack(4);
		    	code = push(&op, 4);
		    	if (code < 0)
						return code;
		    	op[-4].makeInt((info.size() + 1023) / 1024);
		    	op[-3].makeInt(info.size());
		    	op[-2].makeInt(info.lastModified().toTime_t());
		    	op[-1].makeInt(info.created().toTime_t());
		    }
		    else
		    {
		    	op->makeBool(false);
		    	code = 0;
		    }
				return code;
	    }
	    
		default:
	    return op->checkTypeFailed();
  }
}

int XWPSContextState::zunread()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSStream *s;
  ulong ch;

  int code = op[-1].checkReadType(XWPSRef::File);
  if (code < 0)
  	return code;
  	
  s = op[-1].getStream();
  if (s->read_id != op[-1].size())
  {
  	if (s->read_id == 0 && s->write_id == op[-1].size())
  	{
  		code = fileSwitchToRead(op - 1);
  		if (code < 0)
  			return code;
  	}
  	else
  		s = &invalid_file_stream;
  }
  
  code = op->checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
  	
  ch = op->value.intval;
  if (ch > 0xff)
		return (int)(XWPSError::RangeCheck);
  
  if (s->ungetc((uchar) ch) < 0)
		return (int)(XWPSError::IOError);
  pop(2);
  return 0;
}

int XWPSContextState::zwrite()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSStream *s;
  uchar ch;
  int status;

  int code = op[-1].checkWriteType(XWPSRef::File);
  if (code < 0)
  	return code;
  s = op[-1].getStream();
  if (s->write_id != op[-1].size() )
  {
  	code = fileSwitchToWrite(op - 1);
  	if (code < 0)
  		return code;
  }
  
  code = op->checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
  	
  ch = (uchar) op->value.intval;
  status = s->putc((uchar) ch);
  if (status >= 0) 
  {
		pop(2);
		return 0;
  }
  return handleWriteStatus(status, op - 1, NULL, &XWPSContextState::zwrite);
}

int XWPSContextState::zwriteCvp()
{
	XWPSRef * op = op_stack.getCurrentTop();
	return zwriteCvpAt(op, 0, true);
}

int XWPSContextState::zwriteCvpAt(XWPSRef * op, uint start, bool first)
{
	XWPSStream *s;
  uchar str[100];		/* arbitrary */
  XWPSRef rstr;
  uchar *data = str;
  uint len;
  int code, status;

  code = op[-2].checkWriteType(XWPSRef::File);
  if (code < 0)
  	return code;
  s = op[-2].getStream();
  if (s->write_id != op[-2].size() )
  {
  	code = fileSwitchToWrite(op - 2);
  	if (code < 0)
  		return code;
  }
  
  code = op->checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
  	
  code = objCvp(op - 1, str, sizeof(str), &len, (int)op->value.intval,  start);
  if (code == XWPSError::RangeCheck) 
  {
		code = objStringData(op - 1, (const uchar**)&data, &len);
		if (len < start)
			return (int)(XWPSError::RangeCheck);
		data += start;
		len -= start;
  }
  if (code < 0)
		return code;
		
  rstr.makeStringSta(0, len, data);
  status = writeString(&rstr, s);
  XWPSRef * ep = exec_stack.getCurrentTop();
  switch (status) 
  {
		default:
	    return (int)(XWPSError::IOError);
	    
		case 0:
	    break;
	    
		case INTC:
		case CALLC:
	    len = start + len - rstr.size();
	    if (!first)
				op_stack.incCurrentTop(-1);
	    return handleWriteStatus(status, op - 2, &len,  &XWPSContextState::zwriteCvpContinue);
  }
  if (code == 1) 
  {
		if (first)
	    checkOStack(1);
	    
	  ep = exec_stack.incCurrentTop(1);
	  ep->makeOper(0,&XWPSContextState::zwriteCvpContinue);
		if (first)
		{
	    code = push(&op, 1);
	    if (code < 0)
				return code;
	  }
		op_stack.getCurrentTop()->makeInt(start + len);
		return PS_O_PUSH_ESTACK;
  }
  if (first)
		pop(3);
  else	
		pop(4);
  return 0;
}

int XWPSContextState::zwriteCvpContinue()
{
	XWPSRef * op = op_stack.getCurrentTop();

  int code = op->checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
  	
  if (op->value.intval != (uint) op->value.intval)
		return (int)(XWPSError::RangeCheck);
  return zwriteCvpAt(op - 1, (uint) op->value.intval, false);
}

int XWPSContextState::zwriteHexString()
{
	XWPSRef * op = op_stack.getCurrentTop();

  return zwriteHexStringAt(op, 0);
}

int XWPSContextState::zwriteHexStringAt(XWPSRef * op, uint odd)
{
	XWPSStream *s;
  uchar ch;
  uchar *p;
  const char *const hex_digits = "0123456789abcdef";
  uint len;
  int status;

#define MAX_HEX 128
    
  uchar buf[MAX_HEX];

  int code = op[-1].checkWriteType(XWPSRef::File);
  if (code < 0)
  	return code;
  s = op[-1].getStream();
  if (s->write_id != op[-1].size() )
  {
  	code = fileSwitchToWrite(op - 1);
  	if (code < 0)
  		return code;
  }
  
  code = op->checkReadType(XWPSRef::String);
  if (code < 0)
  	return code;
  p = op->getBytes();
  len = op->size();
  while (len) 
  {
		uint len1 = qMin(len, (uint)(MAX_HEX / 2));
		uchar *q = buf;
		uint count = len1;
		XWPSRef rbuf;
		rbuf.makeString(0);

		do 
		{
	    ch = *p++;
	    *q++ = hex_digits[ch >> 4];
	    *q++ = hex_digits[ch & 0xf];
		}	while (--count);
		rbuf.makeStringSta(0, (len1 << 1) - odd, buf + odd);
		status = writeString(&rbuf, s);
		switch (status) 
		{
	    default:
				return (int)(XWPSError::IOError);
				
	    case 0:
				len -= len1;
				odd = 0;
				continue;
				
	    case INTC:
	    case CALLC:
				count = rbuf.getBytes() - buf;
				op->setBytesPtr(op->getBytes() + (count >> 1));
				op->setSize(len - (count >> 1));
				count &= 1;
				return handleWriteStatus(status, op - 1, &count,  &XWPSContextState::zwriteHexStringContinue);
		}
  }
  pop(2);
  return 0;
#undef MAX_HEX
}

int XWPSContextState::zwriteHexStringContinue()
{
	XWPSRef * op = op_stack.getCurrentTop();
  int code;

  code = op->checkType(XWPSRef::Integer);
  if (code < 0)
  	return code;
  	
  if ((op->value.intval & ~1) != 0)
		return (int)(XWPSError::RangeCheck);
  code = zwriteHexStringAt(op - 1, (uint) op->value.intval);
  if (code >= 0)
		pop(1);
  return code;
}

int XWPSContextState::zwriteString()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSStream *s;
  int status;

  int code = op[-1].checkWriteType(XWPSRef::File);
  if (code < 0)
  	return code;
  s = op[-1].getStream();
  if (s->write_id != op[-1].size() )
  {
  	code = fileSwitchToWrite(op - 1);
  	if (code < 0)
  		return code;
  }
  
  code = op->checkReadType(XWPSRef::String);
  if (code < 0)
  	return code;
  status = writeString(op, s);
  if (status >= 0) 
  {
		pop(2);
		return 0;
  }
  return handleWriteStatus(status, op - 1, NULL, &XWPSContextState::zwriteString);
}

int XWPSContextState::zxfilePosition()
{
	XWPSRef * op = op_stack.getCurrentTop();
  XWPSStream *s;

  int code = op->checkType(XWPSRef::File);
  if (code < 0)
  	return code;
  	
  s = op->getStream();
  if ((s->read_id | s->write_id) != op->size())
		return (int)(XWPSError::InvalidAccess);
  
  op->makeInt(s->tell());
  return 0;
}
