/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <QMutex>
#include "XWFontSea.h"
#include "XWApplication.h"
#include "XWSubfont.h"

static QMutex subfontMutex;

class XWSubfontRec
{
public:
	XWSubfontRec();
	
	long getCode(long c) {return vector[c];}
	
	bool read(const char *lbuf);
	
public:
	long vector[256];
};

XWSubfontRec::XWSubfontRec()
{
	for (int i = 0; i < 256; i++)
		vector[i] = -1;
}

bool XWSubfontRec::read(const char *lbuf)
{
#define IS_TOKSEP(c) ((c) == '\0' || isspace((c)))
	char  *p = (char *) lbuf;
	while (*p && isspace(*p))
		p++;
		
	long v2 = 0;
	int  curpos = 0;
	while (*p)
	{
		int repos = 0; 
		char * q = p;
		long v1 = strtol(p, &q, 0);
		if (q == p || (!IS_TOKSEP(*q) && *q != ':' && *q != '_'))
      		return false;
      		
      	switch (*q)
      	{
      		case  ':':
      			if (v1 < 0 || v1 > 0xff) 
        			return false;
      			repos = 1;
      			q++;
      			break;
      			
      		case  '_':
      			p  = q + 1;
      			v2 = strtol(p, &q, 0);
      			if (v1 < 0 || v1 > 0xffffL || v2 < 0 || v2 > 0xffffL) 
        			return false;
      			else if (q == p || !IS_TOKSEP(*q)) 
        			return false;
      			break;
      			
    		default:
      			if (v1 < 0 || v1 > 0xffffL) 
        			return false;
      			v2 = v1;
      			break;
      	}
      	
      	if (repos)
      		curpos = v1;
      	else
      	{
      		if (v2 < v1 || curpos + (v2 - v1) > 0xff) 
        		return false;
      		for (long c = v1; c <= v2; c++) 
      		{
        		if (vector[curpos] != -1) 
          			return false;
        		vector[curpos++] = c;
      		}
      	}
      	
      	p = q;
      	while (*p && isspace(*p))
      		p++;
	}
	
	return  true;
}


class XWSubfontFile
{
public:
	XWSubfontFile(const QString & identA);
	~XWSubfontFile();
	
	QString getCurSubID()
	{
		if (cur_sub_id == -1)
			return QString();
			
		QString ret(sub_id[cur_sub_id]);
		return ret;
	}
	
	QString getFirstSubID()
	{
		cur_sub_id = 0;
		QString ret(sub_id[cur_sub_id]);
		return ret;
	}
	
	QString getNextSubID()
	{
		cur_sub_id++;
		if (cur_sub_id > num_subfonts)
			return QString();
			
		QString ret(sub_id[cur_sub_id]);
		return ret;
	}
	
	char ** getSubfontIds(int *num_ids)
	{
		if (num_ids)
			*num_ids = num_subfonts;
			
		return sub_id;
	}
	
	int loadRecord(const QString & subfont_id);
	static long lookupRecord(int rec_idA, unsigned char c)
	{
		return records[rec_idA]->getCode(c);
	}
	
	static void releaseRecords();

	bool scanFile(QFile *fp);

public:
	QString ident;
	int cur_sub_id;
	char ** sub_id;
	int   * rec_id;
	int     max_subfonts;
  	int     num_subfonts;
  	
  	static XWSubfontRec ** records;
	static int numRecords;
	static int maxRecords;
  	
private:
	char * readLine(char *buf, int buf_len, QFile *fp);
};


XWSubfontRec ** XWSubfontFile::records = 0;
int XWSubfontFile::numRecords = 0;
int XWSubfontFile::maxRecords = 0;

XWSubfontFile::XWSubfontFile(const QString & identA)
	:ident(identA),
	 cur_sub_id(-1),
	 sub_id(0),
	 rec_id(0),
	 max_subfonts(0),
	 num_subfonts(0)
{
}

XWSubfontFile::~XWSubfontFile()
{
	if (sub_id) 
	{
    	for (int i = 0; i < num_subfonts; i++) 
    	{
      		if (sub_id[i])
        		free(sub_id[i]);
    	}
    	free(sub_id);
  	}
  	
  	if (rec_id)
    	free(rec_id);
}

int XWSubfontFile::loadRecord(const QString & subfont_id)
{
	int i = 0;
	for (; i < num_subfonts; i++)
	{
		if (subfont_id == sub_id[i])
			break; 
	}
	
	if (i == num_subfonts)
		return -1;
		
	if (rec_id[i] >= 0)
		return rec_id[i];
		
	XWFontSea sea;
    QFile * fp = sea.openSfd(ident);
    char * p = 0;
    char * q = 0;
    char line_buf[4096];
    int ret = -1;
    while ((p = readLine(line_buf, 4096, fp)))
    {
    	while (*p && isspace(*p))
    		p++;
    	if (*p == 0)
      		continue;
      		
      	q = p;
      	while (*p && !isspace(*p))
      		p++;
      		
    	*p = '\0'; 
    	p++;
    	if (subfont_id == q)
    	{
    		if (numRecords >= maxRecords)
    		{
    			maxRecords += 16;
        		records = (XWSubfontRec **)(realloc)(records, maxRecords * sizeof(XWSubfontRec*));
    		}
    		
    		XWSubfontRec * r = new XWSubfontRec;
    		if (!r->read(p))
    		{
    			delete r;
    			fp->close();
    			delete fp;
    			return -1;
    		}
    		
    		ret = numRecords++;
    		records[ret] = r;
    	}
    }
    fp->close();
    delete fp;
    rec_id[i] = ret;
    return ret;
}

void XWSubfontFile::releaseRecords()
{
	if (records)
	{
		for (int i = 0; i < numRecords; i++)
			delete records[i];
		
		free(records);
		records = 0;
		numRecords = 0;
		maxRecords = 0;
	}
}

bool XWSubfontFile::scanFile(QFile *fp)
{
	fp->seek(0);
	char * p = 0;
	char line_buf[4096];
	int n = 0;
	while ((p = readLine(line_buf, 4096, fp)) != NULL)
	{
		while (*p && isspace(*p))
			p++;
			
    	if (*p == 0)
      		continue;
      		
      	n = 0; 
      	char * q = p;
      	while (*p && !isspace(*p))
      	{
      		p++; 
      		n++;
      	}
      	
    	char * id = (char*)malloc((n + 1) * sizeof(char));
    	memcpy(id, q, n); id[n] = '\0';
    	if (num_subfonts >= max_subfonts) 
    	{
      		max_subfonts += 16;
      		sub_id = (char**)realloc(sub_id, max_subfonts * sizeof(char *));
    	}
    	sub_id[num_subfonts] = id;
    	num_subfonts++;
	}
	
	rec_id = (int*)malloc(num_subfonts * sizeof(int));
	for (n = 0; n < num_subfonts; n++)
		rec_id[n] = -1;
		
	return true;
}

char * XWSubfontFile::readLine(char *buf, int buf_len, QFile *fp)
{
	int n = 0, c = 0;
	char *p = buf;
	while (buf_len - n > 0 && (0 <= fp->readLine(p, buf_len - n)))
	{
		char * q = p;
		char * r = strchr(q, '\n');
		if (r)
			*r = '\0';
			
		c++;
    	r = strchr(q, '#');
    	if (r) 
    	{
      		*r = ' ';
      		*(r + 1) = '\0';
    	}
    	
    	if (strlen(q) == 0)
      		break;
      		
      	n += strlen(q);
    	q += strlen(q) - 1;
    	if (*q != '\\')
      		break;
      	else
      	{
      		n -= 1;
      		p  = buf + n;
      	}
	}
	
	return  (c > 0 ? buf : NULL);
}

XWSubfontFile ** XWSubfont::files = 0;
int  XWSubfont::numFiles = 0;
int  XWSubfont::maxFiles = 0;

XWSubfont::XWSubfont(QObject * parent)
	:QObject(parent)
{
	fileID = -1;
	recID = -1;
}

XWSubfont::XWSubfont(int id, QObject * parent)
	:QObject(parent)
{
	fileID = id;
	recID = -1;
}

XWSubfont::XWSubfont(const QString & sfd_name, QObject * parent)
	:QObject(parent)
{
	fileID = findFile(sfd_name);
	recID = -1;
}

void XWSubfont::getCode(long * sf_code)
{
	if (fileID == -1 || recID == -1 || !sf_code)
		return ;
		
	for (int i = 0; i < 256; i++)
		sf_code[i] = lookupRecord(recID, i);
}

QString XWSubfont::getCurSubID()
{
	if (fileID == -1)
		return QString();
		
	return files[fileID]->getCurSubID();
}

QString XWSubfont::getFirstSubID()
{
	if (fileID == -1)
		return QString();
		
	return files[fileID]->getFirstSubID();
}

QString XWSubfont::getNextSubID()
{
	if (fileID == -1)
		return QString();
		
	return files[fileID]->getNextSubID();
}
	
char ** XWSubfont::getSubfontIds(int *num_ids)
{
	if (fileID == -1)
		return 0;
		
	return files[fileID]->getSubfontIds(num_ids);
}

int XWSubfont::loadRecord(const QString & subfont_id)
{
	if (fileID == -1 || subfont_id.isEmpty())
		return -1;
		
	XWSubfontFile * file = files[fileID];
	recID = file->loadRecord(subfont_id);
	return recID;
}

long XWSubfont::lookupRecord(int rec_id, uchar c)
{
	if (rec_id == -1)
		return -1;
		
	if (recID != rec_id)
		recID = rec_id;
		
	return XWSubfontFile::lookupRecord(recID, c);
}

void XWSubfont::releaseSFD()
{
	XWSubfontFile::releaseRecords();
	if (files)
	{
		for (int i = 0; i < numFiles; i++)
			delete files[i];
			
		free(files);
		files = 0;
		numFiles = 0;
		maxFiles = 0;
	}
}

bool XWSubfont::setSFDName(const QString & sfd_name)
{
	fileID = findFile(sfd_name);
	return isOK();
}

int XWSubfont::findFile(const QString & sfd_name)
{
	subfontMutex.lock();
	if (sfd_name.isEmpty())
	{
		subfontMutex.unlock();
		return -1;
	}
		
	int id = -1;
	for (int i = 0; i < numFiles; i++) 
	{
		XWSubfontFile * file = files[i];
    	if (file->ident == sfd_name) 
    	{
      		id = i;
      		break;
    	}
  	}
  	
  	if (id < 0)
  	{
  		if (numFiles >= maxFiles) 
  		{
      		maxFiles += 8;
      		files = (XWSubfontFile**)realloc(files, maxFiles * sizeof(XWSubfontFile*));
    	}
    	
    	XWSubfontFile * file = new XWSubfontFile(sfd_name);
    	XWFontSea sea;
    	QFile * fp = sea.openSfd(sfd_name);
    	if (!fp)
    	{
    		delete file;
    		xwApp->openError(sfd_name, false);
    		subfontMutex.unlock();
    		return -1;
    	}
    	
    	if (!file->scanFile(fp))
    	{
    		delete file;
    		fp->close();
    		delete fp;
    		QString msg = QString(tr("error in file '%1'.\n")).arg(sfd_name);
    		xwApp->error(msg);
    		subfontMutex.unlock();
    		return -1;
    	}
    	
    	id = numFiles++;
    	files[id] = file;
    	fp->close();
    	delete fp;
  	}
  	subfontMutex.unlock();
  	return id;
}

