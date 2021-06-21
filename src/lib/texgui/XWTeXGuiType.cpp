/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <malloc.h>
#include <QTextStream>
#include <QTextBoundaryFinder>
#include "XWApplication.h"
#include "XWTexSea.h"
#include "TeXKeyWord.h"
#include "XWTeXGuiType.h"

#define GUI_HASH_PRIME 55711

XWTeXHash::XWTeXHash(QObject * parent)
	:QObject(parent),
	 tab(0),
	 pool(0),
	 start(0),
	 minString(0)
{
}

XWTeXHash::XWTeXHash(qint32 minstr, QObject * parent)
	:QObject(parent),
	 tab(0),
	 pool(0),
	 start(0),
	 minString(minstr)
{
}

XWTeXHash::~XWTeXHash()
{
	clear();
}

qint32 XWTeXHash::addString(qint32 * buf, qint32 l)
{
	qint32 ret = strPtr;
	start[strPtr] = poolPtr;
	for (int i = 0; i < l; i++)
	{
		pool[poolPtr] = buf[i];
		poolPtr++;
	}
	
	int h = buf[0];
	for (int i = 1; i < l; i++)
	{
		h = h + h + buf[i];
		while (h >= GUI_HASH_PRIME)
			h = h - GUI_HASH_PRIME;
	}
	
	tab[h].str = (qint32*)realloc(tab[h].str, (tab[h].len + 1) * sizeof(qint32));
	tab[h].str[tab[h].len] = strPtr;
	tab[h].len++;
	strPtr++;
	return ret;
}

qint32  XWTeXHash::addString(const QString & str)
{
	qint32 ret = strPtr;
	start[strPtr] = poolPtr;
	for (int i = 0; i < str.length(); i++)
	{
		pool[poolPtr] = str[i].unicode();
		poolPtr++;
	}
	
	int h = str[0].unicode();
	for (int i = 1; i < str.length(); i++)
	{				
		h = h + h + str[i].unicode();
		while (h >= GUI_HASH_PRIME)
			h = h - GUI_HASH_PRIME;
	}
		
	tab[h].str = (qint32*)realloc(tab[h].str, (tab[h].len + 1) * sizeof(qint32));
	tab[h].str[tab[h].len] = strPtr;
	tab[h].len++;
	strPtr++;
	return ret;
}

QString XWTeXHash::getString(qint32 s)
{
	QString ret;
	s -= minString;
	for (qint32 k = start[s]; k < start[s + 1]; k++)
		ret.append(QChar((uint)(pool[k])));
	return ret;
}

bool XWTeXHash::loadFile(int   maxstrings, 
	                     int   poolsize,
	                     const QString & filename)
{
	XWXWTexSea sea;
	QFile * poolfile = sea.openFile(filename, XWXWTexSea::Pool);
	if (!poolfile)
	{
		xwApp->openError(filename, false);
		return false;
	}
	
	clear();
	strPtr = 0;
	poolPtr = 0;
	poolSize = poolsize;
	pool = (qint32 *)malloc((poolSize + 1) * sizeof(qint32));
	maxStrings = maxstrings;
	
	tab = (Bucket*)malloc((GUI_HASH_PRIME + 1) * sizeof(Bucket));
	for (int i = 0; i <= GUI_HASH_PRIME; i++)
	{
		tab[i].len = 0;
		tab[i].str = 0;
	}
	
	start = (qint32*)malloc((maxStrings + 2) * sizeof(qint32));	
	QTextStream stream(poolfile);	
	while (!stream.atEnd())
	{
		QString line = stream.readLine();
		if (line.length() <= 0)
			continue;
			
		start[strPtr] = poolPtr;
		for (int i = 0; i < line.length(); i++)
		{
			pool[poolPtr] = line[i].unicode();
			poolPtr++;
		}
		
		int h = line[0].unicode();
		for (int i = 1; i < line.length(); i++)
		{
			h = h + h + line[i].unicode();
			while (h >= GUI_HASH_PRIME)
			    h = h - GUI_HASH_PRIME;
		}
		
		tab[h].str = (qint32*)realloc(tab[h].str, (tab[h].len + 1) * sizeof(qint32));
		tab[h].str[tab[h].len] = strPtr;
		tab[h].len++;
		strPtr++;
	}
	
	poolfile->close();
	delete poolfile;
	return true;
}

qint32 XWTeXHash::lookupID(qint32 * buf, qint32 l)
{
	if (l <= 0)
	  return -1;

	int h = buf[0];
	for (int i = 1; i < l; i++)
	{
		h = h + h + buf[i];
		while (h >= GUI_HASH_PRIME)
			h = h - GUI_HASH_PRIME;
	}
				
	for (qint32 j = 0; j < tab[h].len; j++)
	{
		if (length(tab[h].str[j]) == l)
		{
			qint32 m = 0;
			for (qint32 k = start[tab[h].str[j]]; k < start[tab[h].str[j] + 1]; k++)
			{
				if (buf[m] != pool[k])
					break;
				m++;
			}
			
			if (m == l)
				return tab[h].str[j] + minString;
		}
	}
	
	return -1;
}

qint32 XWTeXHash::lookupID(const QString & str)
{
	if (str.length() <= 0)
	  return -1;
		
	qint32 h = str[0].unicode();
	for (int i = 1; i < str.length(); i++)
	{
		h = h + h + str[i].unicode();
		while (h >= GUI_HASH_PRIME)
			h = h - GUI_HASH_PRIME;
	}
				
	for (qint32 j = 0; j < tab[h].len; j++)
	{
		if (length(tab[h].str[j]) == str.length())
		{
			qint32 m = 0;
			for (qint32 k = start[tab[h].str[j]]; k < start[tab[h].str[j] + 1]; k++)
			{
				if (str[m].unicode() != pool[k])
					break;
				m++;
			}
			
			if (m == str.length())
				return tab[h].str[j] + minString;
		}
	}
	
	return -1;
}

void XWTeXHash::clear()
{
	if (tab)
	{
		for (int i = 0; i < maxStrings; i++)
		{
			if (tab[i].str)
				free(tab[i].str);
		}
		
		free(tab);
	}
		
	if (pool)
		free(pool);
		
	if (start)
		free(start);
}

QString breakToLines(int len, const QString & str)
{
	QString ret;
	if ((str.length() + len) <= 78 || str.length() == 0)
	{
		ret = str;
		return ret;
	}
	
	int i = str.lastIndexOf(QChar('\n'));
	QString tmp = str;
	if (i >= 0)
	{
		len += (str.length() - i);
		if (len < 78)
		{
			ret = str;
			return ret;
		}
		
		ret = str.left(i);
		tmp = str.right(str.length() - ret.length());
	}
	
	QTextBoundaryFinder finder(QTextBoundaryFinder::Line, tmp);
	finder.toStart();
	int p0 = 0;
	int p1 = finder.toNextBoundary();	
	while (p1 > 0 )
	{
		if ((len + (p1 - p0)) >= 78 && 
			finder.boundaryReasons() == QTextBoundaryFinder::EndWord)
		{
			len = 0;
			ret += "\n";
		}
		
		ret += tmp.mid(p0, p1 - p0);
		len += p1 - p0;
		p0 = p1;
		p1 = finder.toNextBoundary();
	}
	
	ret += tmp.mid(p0, -1);
	return ret;
}

