/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <malloc.h>
#include <QByteArray>
#include <QLocale>
#include <QIODevice>
#include <QFile>
#include <QTextCodec>
#include <QTextStream>

#include "XWStringUtil.h"
#include "XWUtil.h"

char * getLine(QIODevice * file)
{
	ulong linelen = 1024;
  	char * buffer = (char *)malloc(linelen * sizeof(char));
  	ulong len = 0;
  	char * p = buffer;
  	while (1)
  	{
  		qint64 n = file->readLine(p, 1024);
  		if (n <= 0)
  		{
  			p[0] = '\0';
  			break;
  		}
  		
  		if (n == 1 && p[0] == '\n')
  		{
  			p[0] = '\0';
  			continue;
  		}
  			
  		len += n;
  		
  		if (p[n - 2] != '\\')
  		{
  			p[n - 1] = '\0';
  			return buffer;
  		}
  			
    	if (len == linelen - 1)
    	{
      		linelen += 1024;
      		buffer = (char *)realloc(buffer, linelen * sizeof(char));
    	}
    	
    	p += n - 2;
  	}
  	
  	return 0;
}

bool getLine(char **bufferp, QIODevice *file)
{
	*bufferp = getLine(file);

  	if (!(*bufferp && **bufferp))
    	return false;

  	return true;
}

char getSignedByte(QIODevice * file)
{
	int byte = getUnsignedByte(file);
	if (byte >= 0x80) 
    	byte -= 0x100;
    	
  	return (char)byte;
}

uchar getUnsignedByte(QIODevice * file)
{
	char ch = 0;
	if (!file->getChar(&ch))
		ch = -1;
		
	return (uchar)ch;
}

short getSignedPair(QIODevice * file)
{
	long pair = 0;
  	for (int i = 0; i < 2; i++) 
    	pair = pair * 0x100 + getUnsignedByte(file);
  	
    if (pair >= 0x8000) 
    	pair -= 0x10000l;
    	
  	return (short)pair;
}

ushort getUnsignedPair(QIODevice * file)
{
	int  pair = 0;
	for (int i = 0; i < 2; i++) 
	{
    	int byte = getUnsignedByte(file);
    	pair = pair * 0x100u + byte;
  	}
  	return (ushort)pair;
}

long getSignedTriple(QIODevice * file)
{
	long triple = 0;
  	for (int i = 0; i < 3; i++) 
    	triple = triple * 0x100 + getUnsignedByte(file);
  	
  	if (triple >= 0x800000l) 
    	triple -= 0x1000000l;
  	return triple;
}

ulong getUnsignedTriple(QIODevice * file)
{
	ulong triple = 0;
  	for (int i = 0; i < 3; i++) 
    	triple = triple * 0x100u + getUnsignedByte(file);
    	
  	return triple;
}

long getSignedQuad(QIODevice * file)
{
  	int byte = getUnsignedByte(file);
  	long quad = byte;
  	if (quad >= 0x80) 
    	quad = byte - 0x100;
    	
  	for (int i = 0; i < 3; i++) 
    	quad = quad * 0x100 + getUnsignedByte(file);
    	
  	return quad;
}

ulong getUnsignedQuad(QIODevice * file)
{
	unsigned long quad = 0;
  	for (int i = 0; i < 4; i++) 
    	quad = quad*0x100u + getUnsignedByte(file);
  	
  	return quad;
}

void puts(QFile * f, const QString & s, QTextCodec * codec)
{
    if (!f)
        return;
    
    if (!codec)
        codec = QTextCodec::codecForLocale();
            
    QByteArray ba = codec->fromUnicode(s);
    f->write(ba.data(), ba.size());
    f->flush();
}

QString envVar(const QString & envvar, const QString & defaut)
{
    QTextCodec * codec = QTextCodec::codecForLocale();
    QByteArray ba = codec->fromUnicode(envvar);
    QByteArray envvalue = qgetenv(ba.constData());
    if (envvalue.isEmpty())
        return defaut;
        
    return envvar;
}

QString getEnv(const QString & envvar)
{
    QTextCodec * codec = QTextCodec::codecForLocale();
    QByteArray ba = codec->fromUnicode(envvar);
    QByteArray envvalue = qgetenv(ba.constData());
    if (envvalue.isEmpty())
        return QString();
        
    QString ret = codec->toUnicode(envvalue);
    return ret;
}

QString getAbsoluteDir(const QString & pdir, const QString & sdir)
{
	const QChar * sp = sdir.data();
	QString ret = sdir;
	if (sp[0] != QChar('.'))
		return ret;
		
	ret = pdir;
	if (sdir.length() == 1 || (isDirSep(sp[1]) && sdir.length() == 2))
		return ret;
		
	QString tmpdir = pdir;
	while (sp[0] == QChar('.') && sp[1] == QChar('.'))
	{
		sp += 2;
		while (isDirSep(sp[0]))
			sp++;
			
		int idx = tmpdir.lastIndexOf(QChar('/'));
		if (idx < 0)
		{
			if (isDevSep(pdir[1]))
				tmpdir = tmpdir.left(2);
			else
				tmpdir.clear();
			break;
		}
			
		tmpdir = tmpdir.left(idx);
	}
	
	QString tmp(sp);
	
	if (tmpdir.isEmpty())
		ret = QString("/%1").arg(tmp);
	else
		ret = QString("%1/%2").arg(tmpdir).arg(tmp);
	
	return ret;
}

QString getRelativeDir(const QString & pdir, const QString & sdir)
{
	if (pdir.isEmpty() || sdir.isEmpty())
		return QString();
		
	QString tmppdir = pdir;
	while (isDirSep(tmppdir[tmppdir.length() - 1]))
		tmppdir.remove(tmppdir.length() - 1, 1);
		
	QString tmpsdir = sdir;
	if (isDevSep(tmppdir[1]))
		tmppdir[0] = tmppdir[0].toLower();
		
	if (isDevSep(tmpsdir[1]))
		tmpsdir[0] = tmpsdir[0].toLower();
		
	QChar * pp = tmppdir.data();
	QChar * sp = tmpsdir.data();
	QString ret = tmpsdir;
	if (pp[0] != sp[0])
		return ret;
		
	if (tmpsdir == tmppdir)
		ret = "./";
	else if (tmpsdir.length() > tmppdir.length() && 
		       tmpsdir.startsWith(tmppdir) && 
		       isDirSep(tmpsdir[tmppdir.length()]))
	{
		int i = tmppdir.length();
		QString tmp = tmpsdir.mid(i);
		ret = QString(".%1").arg(tmp);
	}
	else
	{
		const QChar * osp = sp;
		const QChar * opp = pp;
		while (!sp[0].isNull() && !pp[0].isNull() && sp[0] == pp[0])
		{
			sp++;
			pp++;
		}
		
		if (isDirSep(pp[0]))
			pp--;
		
		while (pp > opp && !isDirSep(pp[0]))
			pp--;
			
		if (isDirSep(sp[0]))
			sp--;
		
		while (sp > osp && !isDirSep(sp[0]))
			sp--;
			
		QString tmp(sp + 1);
		
		QString v;
		while (!pp[0].isNull())
		{
			if (isDirSep(pp[0]))
				v += "../";
				
			pp++;
		}
		
		ret = QString("%1%2").arg(v).arg(tmp);
	}
	
	return ret;
}

QString normalizeQuotes(const char * name)
{
	if (!name || name[0] == '\0')
		return QString();
		
	QTextCodec * codec = QTextCodec::codecForLocale();		
	QString tmp = codec->toUnicode(name);
	return normalizeQuotes(tmp);
}

QString normalizeQuotes(const QString & name)
{
	QString tmp = name;
	tmp = tmp.trimmed();
	int idx = tmp.indexOf(QChar(' '));
	bool must_quote = (idx != -1);
	QString ret;
	if (must_quote)
		ret.append(QChar('"'));
		
	QChar * q = tmp.data();
	bool quoted = false;
	while (!q->isNull())
	{
		if (q[0] == QChar('"'))
			quoted = !quoted;
		else
			ret += q[0];
			
		q++;
	}
	
	if (must_quote)
		ret.append(QChar('"'));
		
	return ret;
}

void putEnv(const QString & varname, int value)
{
    QString str = QString("%1").arg(value);
    putEnv(varname, str);
}

void putEnv(const QString & varname, const QString & value)
{
    QTextCodec * codec = QTextCodec::codecForLocale();
    QByteArray name = codec->fromUnicode(varname);
    QByteArray v = codec->fromUnicode(value);
    qputenv(name.constData(), v);
}

bool isCJK()
{
    QLocale::Language l = QLocale::system().language();
        
    return ((l == QLocale::Chinese) || 
            (l == QLocale::Japanese) || 
            (l == QLocale::Korean));
}
