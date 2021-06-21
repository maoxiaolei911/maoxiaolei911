/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSIODEVICE_H
#define XWPSIODEVICE_H


#include "XWGlobal.h"
#include "XWPathSea.h"
#include "XWPSType.h"
#include <QString>
#include <QStringList>

class QIODevice;
class XWPSStream;
class XWPSParamList;
class XWPSContextState;
class XWPSIODevice;
class XWPSStdin;
class XWPSStdout;
class XWPSStderr;
class XWPSOs;

class XWGhostscriptSea : public QObject
{
	Q_OBJECT
    
public:
	enum FileFormat
	{
		Lib = 0,
		Font,
		Last
	};
	
	XWGhostscriptSea(QObject * parent = 0);
	
	QStringList allLib();
	QStringList allFileNames(FileFormat format);
	QStringList allFonts();
	
	QString findFile(const QString & name);
	QString findFile(const QString & name,
	                 FileFormat format,
                     bool must_exist);
	FileFormat findFormat(const QString & name);
        
    QFile * openFile(const QString & filename);
    QFile * openFile(const QString & filename, FileFormat format);
};

class XWGhostscriptSeaPrivate : public XWPathSea
{
    Q_OBJECT

public:
    XWGhostscriptSeaPrivate(QObject * parent = 0);

    QString initFormat(int format);
};

extern const char iodev_dtype_stdio[];

class XWPSFileEnum: public XWPSStruct
{
public:
	XWPSFileEnum(const char *pat, uint pl);
	~XWPSFileEnum();
	
	int getLength();
	const char * getTypeName();
	uint next(char *ptr, uint maxlen);
	
public:
	QString pattern;
	QStringList files;
	int fileidx;
};

struct PSIODeviceProcs
{
	union
	{
		int (XWPSIODevice::*init)();
		int (XWPSStdin::*initstdin)();
		int (XWPSStdout::*initstdout)();
		int (XWPSStderr::*initstderr)();
		int (XWPSOs::*initos)();
	}init_;
	
	union
	{
		int (XWPSIODevice::*open_device)(const char *, XWPSStream **);
		int (XWPSStdin::*open_devicestdin)(const char *, XWPSStream **);
		int (XWPSStdout::*open_devicestdout)(const char *, XWPSStream **);
		int (XWPSStderr::*open_devicestderr)(const char *, XWPSStream **);
		int (XWPSOs::*open_deviceos)(const char *, XWPSStream **);
	}open_device_;
	
	union
	{
		int (XWPSIODevice::*open_file)(const char *, 
		                             uint ,
	                               const char *, 
	                               XWPSStream **);
		int (XWPSStdin::*open_filestdin)(const char *, 
		                             uint ,
	                               const char *, 
	                               XWPSStream **);
		int (XWPSStdout::*open_filestdout)(const char *, 
		                             uint ,
	                               const char *, 
	                               XWPSStream **);
		int (XWPSStderr::*open_filestderr)(const char *, 
		                             uint ,
	                               const char *, 
	                               XWPSStream **);
		int (XWPSOs::*open_fileos)(const char *, 
		                             uint ,
	                               const char *, 
	                               XWPSStream **);
	}open_file_;
	
	union
	{
		int (XWPSIODevice::*fopen)(const char *, 
	                           const char *,
	                           QIODevice **, 
	                           char *, 
	                           uint );
		int (XWPSStdin::*fopenstdin)(const char *, 
	                           const char *,
	                           QIODevice **, 
	                           char *, 
	                           uint);
		int (XWPSStdout::*fopenstdout)(const char *, 
	                           const char *,
	                           QIODevice **, 
	                           char *, 
	                           uint);
		int (XWPSStderr::*fopenstderr)(const char *, 
	                           const char *,
	                           QIODevice **, 
	                           char *, 
	                           uint);
		int (XWPSOs::*fopenos)(const char *, 
	                           const char *,
	                           QIODevice **, 
	                           char *, 
	                           uint);
	}fopen_;
	
	union
	{
		int (XWPSIODevice::*fclose)(QIODevice *);
		int (XWPSStdin::*fclosestdin)(QIODevice *);
		int (XWPSStdout::*fclosestdout)(QIODevice *);
		int (XWPSStderr::*fclosestderr)(QIODevice *);
		int (XWPSOs::*fcloseos)(QIODevice *);
	}fclose_;
	
	union
	{
		int (XWPSIODevice::*delete_file)(const char *);
		int (XWPSStdin::*delete_filestdin)(const char *);
		int (XWPSStdout::*delete_filestdout)(const char *);
		int (XWPSStderr::*delete_filestderr)(const char *);
		int (XWPSOs::*delete_fileos)(const char *);
	}delete_file_;
	
	union
	{
		int (XWPSIODevice::*rename_file)(const char *, const char *);
		int (XWPSStdin::*rename_filestdin)(const char *, const char *);
		int (XWPSStdout::*rename_filestdout)(const char *, const char *);
		int (XWPSStderr::*rename_filestderr)(const char *, const char *);
		int (XWPSOs::*rename_fileos)(const char *, const char *);
	}rename_file_;
	
	union
	{
		int (XWPSIODevice::*get_params)(XWPSParamList*);
		int (XWPSStdin::*get_paramsstdin)(XWPSParamList*);
		int (XWPSStdout::*get_paramsstdout)(XWPSParamList*);
		int (XWPSStderr::*get_paramsstderr)(XWPSParamList*);
		int (XWPSOs::*get_paramsos)(XWPSParamList*);
	}get_params_;
	
	union
	{
		int (XWPSIODevice::*put_params)(XWPSParamList*);
		int (XWPSStdin::*put_paramsstdin)(XWPSParamList*);
		int (XWPSStdout::*put_paramsstdout)(XWPSParamList*);
		int (XWPSStderr::*put_paramsstderr)(XWPSParamList*);
		int (XWPSOs::*put_paramsos)(XWPSParamList*);
	}put_params_;
};

class XWPSIODevice
{
public:
	XWPSIODevice(const char * n, const char *t);
	virtual ~XWPSIODevice();
	
	int deleteFile(const char *fname);
	int deleteFileNo(const char *);
	
	int fclose(QIODevice *file);
	int fcloseNo(QIODevice *);
	int fileOpenStream(const char *fname, 
	                   uint len, 
	                   const char *file_access,
		                uint buffer_size, 
		                XWPSStream ** ps);
	int fopen(const char *fname, 
	          const char *access,
	          QIODevice **pfile, 
	          char *rfname, 
	          uint rnamelen);
	int  fopenLibFile(const char *bname,
	                  const char *ignore_access, 
	                  QIODevice ** pfile,
	                  char *rfname, 
	                  uint rnamelen);
	int fopenNo(const char *, 
	          const char *,
	          QIODevice **, 
	          char *, 
	          uint );
	          
	int getParams(XWPSParamList*plist);
	int getParamsNo(XWPSParamList*) {return 0;}
	
	int init();
	int initNo() {return 0;}
	
	int open(const char *access, XWPSStream **ps);
	int openNo(const char *, XWPSStream **);
	int openFile(const char *fname, 
		            uint namelen,
	              const char *access, 
	              XWPSStream **ps);
	int openFileNo(const char *, 
		            uint ,
	              const char *, 
	              XWPSStream **);
	              
	int putParams(XWPSParamList *plist);
	int putParamsNo(XWPSParamList *) {return 0;}
	              
	int renameFile(const char *from, const char *to);
	int renameFileNo(const char *, const char *);
	
public:
	const char *dname;
	const char *dtype;
	PSIODeviceProcs procs;
  void *state;
};

class XWPSParsedFileName
{
public:
	XWPSParsedFileName();
	~XWPSParsedFileName();
	
	int parseFileName(XWPSContextState * ctx, const char *pname, uint lenA);
	int parseRealFileName(XWPSContextState * ctx, const char *pname,	uint lenA);
	
	int terminateFileName(XWPSContextState * ctx);
	
public:
	XWPSIODevice *iodev;
  const char *fname;
  uint len;
  bool own_fn;
};

class XWPSStdin : public XWPSIODevice
{
public:
	XWPSStdin();
	~XWPSStdin();
	
	int openStdin(const char *access, XWPSStream **ps);
	
public:
	QFile * file;
	XWPSStream * strm;
};

class XWPSStdout : public XWPSIODevice
{
public:
	XWPSStdout();
	~XWPSStdout();
	
	int openStdout(const char *access, XWPSStream **ps);
	
public:
	QFile * file;
	XWPSStream * strm;
};

class XWPSStderr : public XWPSIODevice
{
public:
	XWPSStderr();
	~XWPSStderr();
	
	int openStderr(const char *access, XWPSStream **ps);
	
public:
	QFile * file;
	XWPSStream * strm;
};

class XWPSOs : public XWPSIODevice
{
public:
	XWPSOs();
	~XWPSOs();
	
	int deleteFileOS(const char *fname);
	
	int fcloseOS(QIODevice *fileA);
	int fopenOS(const char *fname, 
	          const char *access,
	          QIODevice **pfile, 
	          char *rfname, 
	          uint rnamelen);
	          
	int getParamsOS(XWPSParamList*plist);
	
	int openFileOS(const char *fname, 
		            uint namelen,
	              const char *access, 
	              XWPSStream **ps);
	
	int renameFileOS(const char *from, const char *to);
	          
public:
	QFile * file;
};

#endif //XWPSIODEVICE_H
