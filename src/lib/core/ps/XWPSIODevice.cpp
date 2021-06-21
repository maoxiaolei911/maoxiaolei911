/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QFile>
#include <QRegExp>
#include "XWFileName.h"
#include "XWPictureSea.h"
#include "XWFontSea.h"
#include "XWDocSea.h"
#include "XWPSError.h"
#include "XWPSParam.h"
#include "XWPSStream.h"
#include "XWPSContextState.h"
#include "XWDviPsSpecial.h"
#include "XWPSIODevice.h"

const char iodev_dtype_stdio[] = "Special";

static PathSeaParams GhostscriptSeaParams[] = 
{
	{"lib", 0, 0, "lib", "GS_LIB", "ps;.eps", 0, true, true},
	{"font", 0, 0, "fonts", "GS_FONTPATH", ".GS;.OSF;.ATB;.ATM;.OS2;.SGI;.Sol;.Ult;.pfa;.pfb;.ttf", 0, true, false}
};

XWGhostscriptSeaPrivate::XWGhostscriptSeaPrivate(QObject * parent)
	:XWPathSea("Ghostscript", (int)(XWGhostscriptSea::Last), "Ghostscript.cnf", "Ghostscript.lsr", parent)
{
}

QString XWGhostscriptSeaPrivate::initFormat(int format)
{
	FormatInfo * f = &formatInfo[format];
    if (!(f->path.isEmpty()))
        return f->path;

    initFormatByParams(format, &GhostscriptSeaParams[format]);

    return f->path;
}


static XWGhostscriptSeaPrivate * GhostscriptSea = 0;

XWGhostscriptSea::XWGhostscriptSea(QObject * parent)
	:QObject(parent)
{
	if (!GhostscriptSea)
		GhostscriptSea = new XWGhostscriptSeaPrivate;
}

QStringList XWGhostscriptSea::allLib()
{
	return allFileNames(Lib);
}

QStringList XWGhostscriptSea::allFileNames(XWGhostscriptSea::FileFormat format)
{
	return GhostscriptSea->allFileNames(&GhostscriptSeaParams[format]);
}

QStringList XWGhostscriptSea::allFonts()
{
	return allFileNames(Font);
}

QString XWGhostscriptSea::findFile(const QString & name)
{
	if (name.isEmpty())
	{
    return QString();
  }
        
  QString ret = GhostscriptSea->findFile(name);
	return ret;
}

QString XWGhostscriptSea::findFile(const QString & name,
	                 XWGhostscriptSea::FileFormat format,
                     bool must_exist)
{
	if (name.isEmpty())
	{
        return QString();
      }

    QString ret = GhostscriptSea->findFile(name, format, must_exist);
	return ret;
}

XWGhostscriptSea::FileFormat XWGhostscriptSea::findFormat(const QString & name)
{
	int tmpfmt = GhostscriptSea->findFormat(name);
    if (tmpfmt == -1)
    {
        return XWGhostscriptSea::Last;
        }

    XWGhostscriptSea::FileFormat format = (XWGhostscriptSea::FileFormat)tmpfmt;
    return format;
}

QFile * XWGhostscriptSea::openFile(const QString & filename)
{
	int tmpfmt = GhostscriptSea->findFormat(filename);
	if (tmpfmt == -1)
	{
		return 0;
	}
		
	QString fullname = findFile(filename, (XWGhostscriptSea::FileFormat)tmpfmt, false);
	if (fullname.isEmpty())
	{
		return 0;
	}
		
	QFile * fp = new QFile(fullname);
	if (!fp->open(QIODevice::ReadOnly))
	{
		delete fp;
		fp = 0;
	}
	
	return fp;
}

QFile * XWGhostscriptSea::openFile(const QString & filename, XWGhostscriptSea::FileFormat format)
{
	QFile * ret = GhostscriptSea->openFile((int)format, filename);
	return ret;
}

XWPSFileEnum::XWPSFileEnum(const char *pat, uint )
	:XWPSStruct()
{
	fileidx = 0;
  pattern  = QFile::decodeName(pat);
  int p = pattern.lastIndexOf(QChar('/'));
  QString str = pattern;
  if (p > 0)
  	str = pattern.mid(p + 1);
  
  QRegExp reg(str);
  
  XWGhostscriptSea sea;
  QStringList fs = sea.allFileNames(XWGhostscriptSea::Lib);
  for (int i = 0; i < fs.size(); i++)
  {
  	str = fs[i];
  	if (reg.exactMatch(str))
  		files << str;
  }
}

XWPSFileEnum::~XWPSFileEnum()
{
}

int XWPSFileEnum::getLength()
{
	return sizeof(XWPSFileEnum);
}

const char * XWPSFileEnum::getTypeName()
{
	return "fileenum";
}

uint XWPSFileEnum::next(char *ptr, uint )
{
	if (fileidx >= files.size())
	{
		ptr[0] = 0;
		return ~(uint)0;
	}
	
	QString str = files[fileidx++];
	XWGhostscriptSea sea;
	QString fn = sea.findFile(str);
	
	if (fn.isEmpty())
	{
		ptr[0] = 0;
		return ~(uint)0;
	}
	
	QByteArray ba = QFile::encodeName(fn);
	memcpy(ptr, ba.data(), ba.size());
	return ba.size();
}

XWPSIODevice::XWPSIODevice(const char * n, const char *t)
	:dname(n),
	 dtype(t),
	 state(0)
{
	procs.init_.init = &XWPSIODevice::initNo;
	procs.open_device_.open_device = &XWPSIODevice::openNo;
	procs.open_file_.open_file = &XWPSIODevice::openFileNo;
	procs.fopen_.fopen = &XWPSIODevice::fopenNo;
	procs.fclose_.fclose = &XWPSIODevice::fcloseNo;
	procs.delete_file_.delete_file = &XWPSIODevice::deleteFileNo;
	procs.rename_file_.rename_file = &XWPSIODevice::renameFileNo;
	procs.get_params_.get_params = &XWPSIODevice::getParamsNo;
	procs.put_params_.put_params = &XWPSIODevice::putParamsNo;
}

XWPSIODevice::~XWPSIODevice()
{
}

int XWPSIODevice::deleteFile(const char *fname)
{
	return (this->*(procs.delete_file_.delete_file))(fname);
}

int XWPSIODevice::deleteFileNo(const char *)
{
	return (int)(XWPSError::InvalidFileAccess);
}

int XWPSIODevice::fclose(QIODevice *file)
{
	return (this->*(procs.fclose_.fclose))(file);
}

int XWPSIODevice::fcloseNo(QIODevice *)
{
	return (int)(XWPSError::IOError);
}

int XWPSIODevice::fileOpenStream(const char *fname, 
	                   uint len, 
	                   const char *file_access,
		                uint buffer_size, 
		                XWPSStream ** ps)
{
	uchar *buffer;
  XWPSStream *s;
  QIODevice * file = 0;
  if (buffer_size == 0)
		buffer_size = 512;
		
	s = new XWPSStream;
	s->cbuf_bytes = new XWPSBytes;
	buffer = (uchar*)malloc(buffer_size * sizeof(uchar));
	s->cbuf_bytes->arr = buffer;
	s->cbuf_bytes->length = buffer_size;
	s->cbuf_bytes->self = true;
	if (fname != 0)
	{
		char *file_name = (char *)buffer;
		char fmode[4];
		int code;

		memcpy(file_name, fname, len);
		file_name[len] = 0;
		strcpy(fmode, file_access);
		code = fopen(file_name, fmode, &file, (char *)buffer, buffer_size);
		if (code < 0)
		{
			delete s;
			return code;
		}
		
		switch (fmode[0])
		{
			case 'a':
				s->appendFile(file, buffer, buffer_size, false, true);
				break;
				
			case 'r':
				s->readFile(file, buffer, buffer_size, false, true);
				break;
				
			case 'w':
				s->writeFile(file, buffer, buffer_size, false, true);
				break;
		}
		
		if (fmode[1] == '+')
	    s->file_modes |= PS_STREAM_MODE_READ | PS_STREAM_MODE_WRITE;
		s->save_close = s->procs.close;
		s->procs.close = &XWPSStream::fileCloseFile;
	}
	else
	{
		s->cbuf = buffer;
		s->bsize = s->cbsize = buffer_size;
	}
	
	*ps = s;
  return 0;
}

int XWPSIODevice::fopen(const char *fname, 
	          const char *access,
	          QIODevice **pfile, 
	          char *rfname, 
	          uint rnamelen)
{
	return (this->*(procs.fopen_.fopen))(fname, access, pfile, rfname, rnamelen);
}

int  XWPSIODevice::fopenLibFile(const char *bname,
	                  const char *, 
	                  QIODevice ** pfile,
	                  char *rfname, 
	                  uint rnamelen)
{
	char fmode[3];
//  int len = strlen(bname);
//  uint pi;

  strcpy(fmode, "r");
  
  QString fn = QFile::decodeName(bname);
  XWGhostscriptSea sea;
  QString afn = sea.findFile(fn);  
  if (afn.isEmpty())
  {
  	return (int)(XWPSError::UndefinedFileName);
  }
  
  QByteArray ba = QFile::encodeName(afn);
  memcpy(rfname, ba.data(), ba.size());
  rnamelen = ba.size();
  return fopen(rfname, fmode, pfile, NULL, rnamelen);
}

int XWPSIODevice::fopenNo(const char *, 
	          const char *,
	          QIODevice **, 
	          char *, 
	          uint )
{
	return (int)(XWPSError::InvalidFileAccess);
}

int XWPSIODevice::getParams(XWPSParamList*plist)
{
	PSParamString ts;
  int code;

  ts.fromString(dtype);
  code = plist->writeName((XWPSContextState*)state, "Type", &ts);
  if (code < 0)
		return code;
	return (this->*(procs.get_params_.get_params))(plist);
}

int XWPSIODevice::init()
{
	return (this->*(procs.init_.init))();
}

int XWPSIODevice::open(const char *access, XWPSStream **ps)
{
	return (this->*(procs.open_device_.open_device))(access, ps);
}

int XWPSIODevice::openNo(const char *, XWPSStream **)
{
	return (int)(XWPSError::InvalidFileAccess);
}

int XWPSIODevice::openFile(const char *fname, 
		            uint namelen,
	              const char *access, 
	              XWPSStream **ps)
{
	return (this->*(procs.open_file_.open_file))(fname, namelen, access, ps);
}

int XWPSIODevice::openFileNo(const char *, 
		            uint ,
	              const char *, 
	              XWPSStream **)
{
	return (int)(XWPSError::InvalidFileAccess);
}

int XWPSIODevice::putParams(XWPSParamList *plist)
{
	return (this->*(procs.put_params_.put_params))(plist);
}

int XWPSIODevice::renameFile(const char *from, const char *to)
{
	return (this->*(procs.rename_file_.rename_file))(from, to);
}

int XWPSIODevice::renameFileNo(const char *, const char *)
{
	return (int)(XWPSError::InvalidFileAccess);
}

XWPSParsedFileName::XWPSParsedFileName()
	:iodev(0),
	 fname(0),
	 len(0),
	 own_fn(false)
{
}

XWPSParsedFileName::~XWPSParsedFileName()
{
	if (own_fn)
	{
		if (fname)
		{
			delete [] fname;
			fname = 0;
		}
	}
}

int XWPSParsedFileName::parseFileName(XWPSContextState * ctx, const char *pname, uint lenA)
{
	uint dlen;
  const char *pdelim;;
  
	if (pname[0] != '%') 
	{	
		iodev = NULL;
		fname = pname;
		len = lenA;
		own_fn = false;
		return 0;
  }
  
  pdelim = (const char *)memchr((const void *)(pname + 1), '%', lenA - 1);
  if (pdelim == NULL)	
		dlen = lenA;
  else if (pdelim[1] == 0) 
  {
		pdelim = NULL;
		dlen = lenA;
  } 
  else 
  {
		dlen = pdelim - pname;
		pdelim++, lenA--;
  }
    
  iodev = ctx->findIODevice((const uchar *)pname, dlen);
  if (iodev == 0)
		return (int)(XWPSError::UndefinedFileName);
	
  fname = pdelim;
  len = lenA - dlen;
  own_fn = false;
  return 0;
}

int XWPSParsedFileName::parseRealFileName(XWPSContextState * ctx, const char *pname,	uint lenA)
{
	int code = parseFileName(ctx, pname, lenA);

  if (code < 0)
		return code;
  if (len == 0)
  {
		return (int)(XWPSError::InvalidFileAccess);
	}
	
  return terminateFileName(ctx);
}

int XWPSParsedFileName::terminateFileName(XWPSContextState * ctx)
{
	uint lenA = len;
  char *fnameA;

  if (iodev == NULL)	
		iodev = ctx->getIODevice(0);
	
  if (own_fn)
		return 0;	
		
  fnameA = new char[lenA + 1];
  memcpy(fnameA, fname, lenA);
  fnameA[lenA] = 0;
  own_fn = true;
  fname = fnameA;
  len = lenA + 1;	
  return 0;
}

XWPSStdin::XWPSStdin()
	:XWPSIODevice("%stdin%", iodev_dtype_stdio)
{
	procs.open_device_.open_devicestdin = &XWPSStdin::openStdin;
	file = new QFile;
	file->open(stdin, QIODevice::ReadOnly);
	strm = 0;
}

XWPSStdin::~XWPSStdin()
{
	if (file)
		delete file;
	file = 0;
	strm = 0;
}

int XWPSStdin::openStdin(const char *, XWPSStream **ps)
{
	strm = new XWPSStream;
	strm->openStdin(file);
	*ps = strm;
	return 0;
}

XWPSStdout::XWPSStdout()
	:XWPSIODevice("%stdout%", iodev_dtype_stdio)
{
	procs.open_device_.open_devicestdout = &XWPSStdout::openStdout;
	file = new QFile;
	file->open(stdout, QIODevice::WriteOnly);
	strm = 0;
}

XWPSStdout::~XWPSStdout()
{
	if (file)
		delete file;
	file = 0;
	strm = 0;
}

int XWPSStdout::openStdout(const char *, XWPSStream **ps)
{
	strm = new XWPSStream;	
	strm->openStdout(file);
	*ps = strm;
	return 0;
}

XWPSStderr::XWPSStderr()
	:XWPSIODevice("%stderr%", iodev_dtype_stdio)
{
	procs.open_device_.open_devicestderr = &XWPSStderr::openStderr;
	file = new QFile;
	file->open(stdout, QIODevice::WriteOnly);
	strm = 0;
}

XWPSStderr::~XWPSStderr()
{
	if (file)
		delete file;
	file = 0;
	strm = 0;
}

int XWPSStderr::openStderr(const char *, XWPSStream **ps)
{
	strm = new XWPSStream;	
	strm->openStdout(file);
	*ps = strm;
	return 0;
}

XWPSOs::XWPSOs()
	:XWPSIODevice("%os%", "FileSystem")
{
	file = 0;
	procs.open_file_.open_fileos = &XWPSOs::openFileOS;
	procs.fopen_.fopenos = &XWPSOs::fopenOS;
	procs.fclose_.fcloseos = &XWPSOs::fcloseOS;
	procs.delete_file_.delete_fileos = &XWPSOs::deleteFileOS;
	procs.rename_file_.rename_fileos = &XWPSOs::renameFileOS;
	procs.get_params_.get_paramsos = &XWPSOs::getParamsOS;
}

XWPSOs::~XWPSOs()
{
}

int XWPSOs::deleteFileOS(const char *fname)
{
	QString fn = QFile::decodeName(fname);
	QFile::remove(fn);
	return 0;
}

int XWPSOs::fcloseOS(QIODevice *fileA)
{
	fileA->close();
	return 0;
}

int XWPSOs::fopenOS(const char *fname, 
	          const char *access,
	          QIODevice ** pfile, 
	          char *rfname, 
	          uint )
{	
	QString fn = QFile::decodeName(fname);
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
  	return (int)(XWPSError::InvalidFileAccess);
  	
  
	file = new QFile(afn);
	*pfile = file;
	QIODevice::OpenMode m = QIODevice::NotOpen;
	QString a(access);
	if (a.contains('r'))
		m |= QIODevice::ReadOnly;
	if (a.contains('w'))
		m |= QIODevice::WriteOnly;			
	if (a.contains('a'))
		m |= QIODevice::Append;
	if (a.contains('+'))
		m |= QIODevice::ReadWrite;
	file->open(m);
	
	if (rfname != NULL)
		strcpy(rfname, fname);
  return 0;
}

int XWPSOs::getParamsOS(XWPSParamList*plist)
{
	int code;
  int i0 = 0, i2 = 2;
  bool btrue = true, bfalse = false;
  int BlockSize;
  long Free, LogicalSize;

  BlockSize = 1024;
  LogicalSize = 2000000000 / BlockSize;	
  Free = LogicalSize * 3 / 4;	

    if ((code = plist->writeBool((XWPSContextState*)state, "HasNames", &btrue)) < 0 ||
			(code = plist->writeInt((XWPSContextState*)state, "BlockSize", &BlockSize)) < 0 ||
			(code = plist->writeLong((XWPSContextState*)state, "Free", &Free)) < 0 ||
			(code = plist->writeInt((XWPSContextState*)state, "InitializeAction", &i0)) < 0 ||
			(code = plist->writeBool((XWPSContextState*)state, "Mounted", &btrue)) < 0 ||
			(code = plist->writeBool((XWPSContextState*)state, "Removable", &bfalse)) < 0 ||
			(code = plist->writeBool((XWPSContextState*)state, "Searchable", &btrue)) < 0 ||
			(code = plist->writeInt((XWPSContextState*)state, "SearchOrder", &i2)) < 0 ||
			(code = plist->writeBool((XWPSContextState*)state, "Writeable", &btrue)) < 0 ||
			(code = plist->writeLong((XWPSContextState*)state, "LogicalSize", &LogicalSize)) < 0)
		return code;
  return 0;
}

int XWPSOs::openFileOS(const char *fname, 
		            uint namelen,
	              const char *access, 
	              XWPSStream **ps)
{
	return fileOpenStream(fname, namelen, access, 512, ps);
}

int XWPSOs::renameFileOS(const char *from, const char *to)
{
	QString fn = QFile::decodeName(from);
	QString tn = QFile::decodeName(to);
	QFile::rename(fn, tn);
	return 0;
}
