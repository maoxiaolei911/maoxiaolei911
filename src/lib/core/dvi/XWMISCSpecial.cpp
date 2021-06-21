/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdio.h>
#include <QTextCodec>
#include "XWNumberUtil.h"
#include "XWLexer.h"
#include "XWApplication.h"
#include "XWPictureSea.h"
#include "XWDVICore.h"
#include "XWDVIDev.h"
#include "XWMPost.h"
#include "XWMISCSpecial.h"

#define SP_MISC_PSBOX       0

SpecialHandler miscHandler[] = {
	{"postscriptbox", SP_MISC_PSBOX}
};

XWMISCSpecial::XWMISCSpecial(XWDVICore * coreA,
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

int XWMISCSpecial::check(XWLexer * lexerA)
{
	const char * p = lexerA->skipWhite();
	if (!strncmp(p, "postscriptbox", 13))
	{
		lexerA->skip(13);
		return SP_MISC_PSBOX;
	}
		
	return -1;
}

bool XWMISCSpecial::exec(int, int)
{
	int error = doPostScriptBox();
	return error ? false : true;
}

int XWMISCSpecial::doPostScriptBox()
{
	const char * p = lexer->skipBlank();
	const char * endptr = lexer->getEndPtr();
	if (p >= endptr) 
	{
    	xwApp->warning(tr("no width/height/filename given for postscriptbox special.\n"));
    	return  -1;
  	}
  	
  	char  buf[512];
  	int len = (int) (endptr - p);
  	len = qMin(511, len);
  	memcpy(buf, p, len);
  	buf[len] = '\0';
  	
  	TransformInfo ti;
  	transform_info_clear(&ti);
  	char filename[256];
  	if (sscanf(buf, "{%lfpt}{%lfpt}{%255[^}]}", &ti.width, &ti.height, filename) != 3) 
  	{
    	xwApp->warning(tr("syntax error in postscriptbox special?\n"));
    	return  -1;
  	}
  	
  	p = lexer->skip(len);
  	
  	ti.width  *= 72.0 / 72.27;
  	ti.height *= 72.0 / 72.27;
  	
  	QTextCodec * codec = QTextCodec::codecForLocale();
	QString fn = codec->toUnicode(filename);
	XWPictureSea sea;
	bool is_tmp = false;
	QFile * fp = sea.openFile(fn, &is_tmp);
	if (!fp)
	{
		QString msg = QString(tr("fail to find image file \"%1\".\n")).arg(fn);
		xwApp->warning(msg);
		return -1;
	}
	
	ti.flags |= (INFO_HAS_WIDTH|INFO_HAS_HEIGHT);

  	for (;;) 
  	{
    	if (fp->readLine(buf, 512) <= 0)
      		break;
      		
      if (buf[0] == '\n')
      	continue;
      		
      	p = buf;
      		
    	if (XWMPost::scanBBox((char**)&p, (char*)(p + strlen(p)), &ti.bbox) >= 0) 
    	{
      		ti.flags |= INFO_HAS_USER_BBOX;
      		break;
    	}
  	}
  	
  	fp->close();
  	if (is_tmp)
  		fp->remove();
  		
  	delete fp;
  	
  	int form_id = core->findImageResource(filename, 0, 0);
  	if (form_id < 0) 
  	{
  		QString msg = QString(tr("failed to load image file: %1")).arg(fn);
    	xwApp->warning(msg);
    	return  -1;
  	}

  	dev->putImage(xref, form_id, &ti, x_user, y_user);

  	return  0;
}
