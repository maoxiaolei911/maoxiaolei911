/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QTextCodec>
#include "XWApplication.h"
#include "XWPictureSea.h"
#include "XWDocSea.h"
#include "XWLexer.h"
#include "XWDVIType.h"
#include "XWDVIRef.h"
#include "XWDVICore.h"
#include "XWDVIDev.h"
#include "XWPSInterpreter.h"
#include "XWPSIODevice.h"
#include "XWDviPsSpecial.h"

static PathSeaParams dvipsSeaParams[] = 
{
	{"eps", 0, 0, "eps", "EPSDIR", ".eps", 0, true, true},
	{"ps", 0, 0, "ps", "PSDIR", ".ps", 0, true, false},
	{"pro", 0, 0, "pro", "PRODIR", ".pro", 0, true, true}
};

XWDviPsSeaPrivate::XWDviPsSeaPrivate(QObject * parent)
	:XWPathSea("dvips", (int)(XWDviPsSea::Last), "dvips.cnf", "dvips.lsr", parent)
{
}

QString XWDviPsSeaPrivate::initFormat(int format)
{
	FormatInfo * f = &formatInfo[format];
    if (!(f->path.isEmpty()))
        return f->path;

    initFormatByParams(format, &dvipsSeaParams[format]);

    return f->path;
}

static XWDviPsSeaPrivate * dvipsSea = 0;

XWDviPsSea::XWDviPsSea(QObject * parent)
	:QObject(parent)
{
	if (!dvipsSea)
		dvipsSea = new XWDviPsSeaPrivate;
}

QString XWDviPsSea::findFile(const QString & name)
{
	if (name.isEmpty())
	{
    return QString();
  }
        
  QString ret = dvipsSea->findFile(name);
	return ret;
}

QString XWDviPsSea::findFile(const QString & name,
	                 XWDviPsSea::FileFormat format,
                     bool must_exist)
{
	if (name.isEmpty())
	{
    return QString();
  }

  QString ret = dvipsSea->findFile(name, format, must_exist);
	return ret;
}

XWDviPsSea::FileFormat XWDviPsSea::findFormat(const QString & name)
{
	int tmpfmt = dvipsSea->findFormat(name);
  if (tmpfmt == -1)
  {
    return XWDviPsSea::Last;
  }

  XWDviPsSea::FileFormat format = (XWDviPsSea::FileFormat)tmpfmt;
  return format;
}

QFile * XWDviPsSea::openFile(const QString & filename)
{
	int tmpfmt = dvipsSea->findFormat(filename);
	if (tmpfmt == -1)
	{
		return 0;
	}
		
	QString fullname = findFile(filename, (XWDviPsSea::FileFormat)tmpfmt, false);
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

QFile * XWDviPsSea::openFile(const QString & filename, XWDviPsSea::FileFormat format)
{
	QFile * ret = dvipsSea->openFile((int)format, filename);
	return ret;
}

#define DVI_SP_PLOTFILE 0
#define DVI_SP_PS       1
#define DVI_SP_SPECIAL  2
#define DVI_SP_HEADER   3
#define DVI_SP_LITERAL  4

SpecialHandler dvipsHandler[] = {
	{"ps: plotfile", DVI_SP_PLOTFILE},
	{"ps:", DVI_SP_PS},
	{"\" ", DVI_SP_SPECIAL},
	{"header", DVI_SP_HEADER},
	{"!", DVI_SP_LITERAL}
};

#define numDVIPSOps (sizeof(dvipsHandler) / sizeof(SpecialHandler))

XWDviPsSpecial::XWDviPsSpecial(XWDVICore * coreA,
	            XWDVIRef * xrefA,
	            XWDVIDev * devA,
	            const char *buffer, 
	            long sizeA,
		        double x_userA, 
		        double y_userA, 
		        double magA,
		        int typeA,
		        QObject * parent)
	:XWDVISpecial(coreA, xrefA, devA, buffer, sizeA, x_userA, y_userA, magA, typeA, parent)
{
}

int XWDviPsSpecial::check(XWLexer * lexerA)
{
	int typeA = -1;
	const char * p = lexerA->getCurPtr();
	for (int i = 0; i < numDVIPSOps; i++)
	{
		SpecialHandler * handler = &dvipsHandler[i];
		if (!strncmp(p, handler->key, strlen(handler->key)))
		{
			typeA = handler->type;
			lexerA->skip(strlen(handler->key));
			break;
		}
	}
	
	return typeA;
}

bool XWDviPsSpecial::exec(int , int )
{
	XWPSInterpreter * interpreter = core->getPSInterpreter();
	char buf[200];
	int len = 0;
	int ret = 0;
	if (xref->isScanning())
	  if (!core->usesSpecial())
	  {
	  	copyFile("tex.pro");
		  copyFile("finclude.pro");
		  copyFile("texps.pro");
		  copyFile("special.pro");
		  copyFile("color.pro");
		  copyFile("crop.pro");
		  core->setUseSpecial(true);
		  interpreter->savePageNextID(core->getPageNo());
		  double w = core->getPageWidth();
		  double h = core->getPageHeight();
		  long sw  = qRound(w * 186467.9811);
		  long sh  = qRound(h * 186467.9811);
		  len = sprintf(buf, "TeXDict begin %ld %ld 1000 72 72 (xiuwendvips) @start end ", sw, sh);
		  interpreter->toPDF(core, xref, dev, (const uchar*)buf, len);
	  }
	
	switch (type)
	{
		case DVI_SP_PLOTFILE:
			len = sprintf(buf, "TeXDict begin %f %f a ", x_user, y_user);
			ret = interpreter->toPDF(core, xref, dev, (const uchar*)buf, len);
			ret = doPlotFile();
			len = sprintf(buf, "end ");
			ret = interpreter->toPDF(core, xref, dev, (const uchar*)buf, len);
			break;
			
		case DVI_SP_PS:
			len = sprintf(buf, "TeXDict begin %f %f a ", x_user, y_user);
			ret = interpreter->toPDF(core, xref, dev, (const uchar*)buf, len);
			ret = doPS();
			len = sprintf(buf, "end ");
			ret = interpreter->toPDF(core, xref, dev, (const uchar*)buf, len);
			break;
			
		case DVI_SP_SPECIAL:
			len = sprintf(buf, "TeXDict begin %f %f a ", x_user, y_user);
			ret = interpreter->toPDF(core, xref, dev, (const uchar*)buf, len);
			ret = doSpecial();
			len = sprintf(buf, "end ");
			ret = interpreter->toPDF(core, xref, dev, (const uchar*)buf, len);
			break;
			
		case DVI_SP_HEADER:
			ret = doHeader();
			break;
			
		case DVI_SP_LITERAL:
			ret = doLiteral();
			break;
			
		default:
			return false;
			break;
	}
	
	return ret < 0 ? false : true;
}

int XWDviPsSpecial::copyFile(const char * filename)
{
	QTextCodec * codec = QTextCodec::codecForLocale();
	QString fn = codec->toUnicode(filename);
	if (fn.isEmpty())
		return 0;
		
	XWDviPsSea sea;
	QString afn = sea.findFile(fn);
	if (afn.isEmpty())
	{
		XWPictureSea psea;
		afn = psea.findFile(fn);
	}
	
	if (afn.isEmpty())
	{
		XWDocSea dsea;
		afn = dsea.findFile(fn);
	}
	
	if (afn.isEmpty())
	{
		XWGhostscriptSea gssea;
		afn = gssea.findFile(fn);
	}
	
	if (afn.isEmpty())
		return 0;
	
	QFile * f = new QFile(afn);
	if (!f)
		return 0;
		
	if (!f->open(QIODevice::ReadOnly))
	{
		delete f;
		return 0;
	}
		
	QByteArray ba = f->readAll();
	f->close();
	delete f;
	if (ba.isEmpty())
		return 0;
		
	XWPSInterpreter * interpreter = core->getPSInterpreter();
	return interpreter->toPDF(core, xref, dev, (const uchar*)(ba.data()), ba.size());
}

int XWDviPsSpecial::doHeader()
{
	if (!(xref->isScanning()))
		return 0;

	const char * p = lexer->skipWhite();
	if (*p == '=')
	{
		p = lexer->skip(1);
		p = lexer->skipWhite();
	}
	
	char * filename = parseFileName();
  if (!filename) 
  {
   	xwApp->warning(tr("no filename specified for ps header special.\n"));
   	return  -1;
  }
  
  int code = copyFile(filename);
  delete [] filename;
  return code;
}

int XWDviPsSpecial::doLiteral()
{
	if (!xref->isScanning())
		return 0;
		
	const char * p = lexer->getCurPtr();
	const char * endptr = lexer->getEndPtr();
	long len = endptr - p + 1;
	core->addBangSpecial(p, len);
	return 0;
}

int XWDviPsSpecial::doPlotFile()
{
	core->endBangSpecials();
	
	char * filename = parseFileName();
  if (!filename) 
  {
   	xwApp->warning(tr("no filename specified for plotfile.\n"));
   	return  -1;
  }
  QTextCodec * codec = QTextCodec::codecForLocale();
	QString fn = codec->toUnicode(filename);
	delete [] filename;
	if (fn.isEmpty())
		return 0;
		
	XWPictureSea psea;
	QString afn = psea.findFile(fn);	
	if (afn.isEmpty())
	{
		XWDocSea dsea;
		afn = dsea.findFile(fn);
	}
	
	if (afn.isEmpty())
	{
		XWGhostscriptSea gssea;
		afn = gssea.findFile(fn);
	}
	
	if (afn.isEmpty())
	{
		XWDviPsSea sea;
		afn = sea.findFile(fn);
	}
	
	if (afn.isEmpty())
		return 0;
	
	QFile * f = new QFile(afn);
	if (!f)
		return 0;
		
	if (!f->open(QIODevice::ReadOnly))
	{
		delete f;
		return 0;
	}
		
	QByteArray ba = f->readAll();
	f->close();
	delete f;
	if (ba.isEmpty())
		return 0;
		
	XWPSInterpreter * interpreter = core->getPSInterpreter();
	return interpreter->toPDF(core, xref, dev, (const uchar*)(ba.data()), ba.size());
}

int XWDviPsSpecial::doPS()
{
	core->endBangSpecials();
	const char * p = lexer->skipWhite();	
	const char * endptr = lexer->getEndPtr();
	if (p >= endptr)
    	return  -1;
    	
  if (p + strlen(":[begin]") <= endptr && 
  	 	!strncmp(p, ":[begin]", strlen(":[begin]")))
  {
  	p = lexer->skip(strlen(":[begin]"));
  }
  else if (p + strlen(":[end]") <= p && 
  	!strncmp(p, ":[end]", strlen(":[end]"))) 
  {
  	p = lexer->skip(strlen(":[end]"));
  }
  else if (*p == ':')
  	p = lexer->skip(1);
  
  XWPSInterpreter * interpreter = core->getPSInterpreter();
	if (!interpreter)
		return -1;
		
	long len = endptr - p + 1;
	return interpreter->toPDF(core, xref, dev, (const uchar*)p, len);
}

int XWDviPsSpecial::doSpecial()
{
	core->endBangSpecials();
	const char * p = lexer->getCurPtr();
	const char * endptr = lexer->getEndPtr();
	long len = endptr - p + 1;
	return core->doLiteralSpecial(p, len);
}

char * XWDviPsSpecial::parseFileName()
{
	const char * p = lexer->skipWhite();	
	const char * endptr = lexer->getEndPtr();
	char   qchar = 0;
	if (!p || p >= endptr)
    	return  0;
  	else if (*p == '\"' || *p == '\'')
  	{
    	qchar = *p;
    	p = lexer->skip(1);
    }
  	else 
    	qchar = ' ';
    	
    int n = 0;
    const char * q = p;
    while (p < endptr && *p != qchar)
    {
    	n++;
    	p = lexer->skip(1);
    }
    
    if (qchar != ' ') 
    {
    	if (*p != qchar)
      		return  0;
    	p = lexer->skip(1);
  	}
  	
  	if (!q || n == 0)
    	return  0;
    	
    char * r = new char[n + 1];
  	memcpy(r, q, n); 
  	r[n] = '\0';

  	return  r;
}
