/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <malloc.h>
#include <QCoreApplication>
#include <QDateTime>
#include <QTextStream>
#include "XWApplication.h"
#include "XWFileName.h"
#include "XWTexSea.h"
#include "XWTeXIODev.h"
#include "XWTeX.h"


QString XWTeX::getTexString(qint32 s)
{
	QString ret;
	if (s < 0)
		return ret;
		
	qint32 len = str_start(s + 1) - str_start(s);
	for (qint32 i = 0; i < len; i++) 
		ret.append(QChar((uint)(str_pool[i + str_start(s)])));
		
	return ret;
}

bool XWTeX::loadPoolFile(const QString & filename)
{
	XWXWTexSea sea;
	QFile * poolfile = sea.openFile(filename, XWXWTexSea::Pool);
	if (!poolfile)
	{
		QString msg = QString(tr("Fail to open pool file %1.\n")).arg(filename);
		dev->termOut(msg);
		return false;
	}
	
	makeString();
	char buf[100];
	while (!poolfile->atEnd())
	{
		int l = (int)(poolfile->readLine(buf, 99));
		l--;
		if (l <= 0)
			break;
			
		if ((pool_ptr + l + string_vacancies) > pool_size)
		{
			print(tr("! You have to increase POOLSIZE."));
			poolfile->close();
			delete poolfile;
			return false;
		}
		
		for (int k = 0; k < l; k++)
			appendChar(buf[k]);
			
		makeString();
	}
	
	poolfile->close();
	delete poolfile;
	return true;
}

qint32 XWTeX::makeTexString(const QString & s)
{
	if (pool_ptr + s.length() >= pool_size)
	{
		QString msg = QString("string pool overflow[%1 bytes]").arg(pool_size);
		dev->termOut(msg);
		return TEX_TOO_BIG_CHAR;
	}
	
	for (qint32 i = 0; i < s.length(); i++)
		str_pool[pool_ptr++] = s[i].unicode();
		
	qint32 ret = makeString();
	qint32 temp_str = searchString(ret);
	if (temp_str > 0)
	{
		ret = temp_str; 
		flushString();
	}
	
	return ret;
}

void XWTeX::endName()
{
	if ((str_ptr + 3) > max_strings)
	{
		overFlow(tr("number of strings"), max_strings - init_str_ptr);
		return ;
	}
	
	if (!strRoom(6))
		return ;
	
	bool must_quote = false;
	qint32 s, t, j, temp_str;
	if (area_delimiter != 0)
	{
		s = str_start(str_ptr);
    t = str_start(str_ptr) + area_delimiter;
    j = s;
    while (!must_quote && (j != t))
    {
    	must_quote = str_pool[j] == ' '; 
    	j++;
    }
    
    if (must_quote)
    {
    	for (j = (pool_ptr - 1); j >= t; j--)
    		str_pool[j + 2] = str_pool[j];
    		
    	str_pool[t + 1] = '"';
    	
    	for (j = (t - 1); j >= s; j--)
    		str_pool[j + 1] = str_pool[j];
    		
    	str_pool[s] = '"';
    	if (ext_delimiter != 0)
    		ext_delimiter = ext_delimiter + 2;
    		
    	area_delimiter = area_delimiter + 2;
    	pool_ptr = pool_ptr + 2;
    }
	}
	
	s = str_start(str_ptr) + area_delimiter;
	if (ext_delimiter == 0)
		t = pool_ptr;
	else
		t = str_start(str_ptr) + ext_delimiter - 1;
		
	must_quote = false;
  j = s;
  while (!must_quote && (j != t))
  {
  	must_quote = str_pool[j]== '"';
  	j++;
  }
  
  if (must_quote)
  {
  	for (j = (pool_ptr - 1); j >= t; j--)
  		str_pool[j + 2] = str_pool[j];
  		
  	str_pool[t + 1] = '"';
  	
  	for (j = (t - 1); j >= s; j--)
  		str_pool[j + 1] = str_pool[j];
  		
  	str_pool[s] = '"';
  	if (ext_delimiter != 0)
  		ext_delimiter = ext_delimiter + 2;
  		
  	pool_ptr = pool_ptr + 2;
  }
  
  if (ext_delimiter != 0)
  {
  	s = str_start(str_ptr) + ext_delimiter - 1;
    t = pool_ptr;
    must_quote = false;
    j = s;
    while (!must_quote && (j != t))
    {
    	must_quote = str_pool[j]== '"';
  		j++;
    }
    
    if (must_quote)
    {
    	str_pool[t + 1] = '"';
    	for (j = (t - 1); j >= s; j--)
  			str_pool[j + 1] = str_pool[j];
  			
  		str_pool[s] = '"';
  		pool_ptr = pool_ptr + 2;
    }
  }
  
  if (area_delimiter == 0)
  	cur_area = TEX_TOO_BIG_CHAR;
  else
  {
  	cur_area = str_ptr;
  	str_start(str_ptr+1) = str_start(str_ptr) + area_delimiter; 
  	str_ptr++;
  	temp_str = searchString(cur_area);
  	if (temp_str > 0)
  	{
  		cur_area = temp_str;
    	str_ptr--;
    	for (j = str_start(str_ptr + 1); j <= (pool_ptr-1); j++)
    		str_pool[j-area_delimiter] = str_pool[j];
    		
    	pool_ptr = pool_ptr - area_delimiter;
  	}
  }
  
  if (ext_delimiter == 0)
  {
  	cur_ext = TEX_TOO_BIG_CHAR;
  	cur_name = slowMakeString();
  }
  else
  {
  	cur_name = str_ptr;
  	str_start(str_ptr + 1) = str_start(str_ptr) + ext_delimiter - area_delimiter - 1;
  	str_ptr++; 
  	cur_ext = makeString();
  	str_ptr--;
  	temp_str = searchString(cur_name);
  	if (temp_str > 0)
  	{
  		cur_name = temp_str;
      str_ptr--;
      for (j = str_start(str_ptr+1); j <= (pool_ptr-1); j++)
      	str_pool[j - ext_delimiter+area_delimiter + 1] = str_pool[j];
      	
      pool_ptr = pool_ptr - ext_delimiter + area_delimiter + 1;
  	}
  	
  	cur_ext = slowMakeString();
  }
}

qint32  XWTeX::getJobName()
{
	qint32 ret = cur_name;
	if (!(dev->jobName.isEmpty()))
		ret = makeTexString(dev->jobName);
		
	return ret;
}

qint32 XWTeX::getOutputFile()
{
	QString fn;
	if (ini_version)
	{
		XWXWTexSea sea;
		QStringList tmplist = sea.getConfigDir(XWXWTexSea::Format);
		fn = tmplist[0];
		fn += "/";
		fn += dev->jobName;
		fn += ".afmt";
	}
	else
	{
		XWFileName n(dev->mainInputFile);
		fn = n.baseName();
		fn += ".dvi";
	}
	
	return makeTexString(fn);
}

qint32 XWTeX::idLookup(qint32 j, qint32 l)
{
	qint32 h = buffer[j];
	qint32 i = 1;
	buffered_cs[0] = escapeChar();
	buffered_cs_len = l + 1;
	for (qint32 k = j + 1; k < (j + l); k++)
	{
		h = h + h + buffer[k];
		buffered_cs[i++] = buffer[k];
		while (h >= TEX_HASH_PRIME)
			h = h - TEX_HASH_PRIME;
	}
	
	qint32 p = h + TEX_HASH_BASE;
	while (true)
	{
		if (newText(p) > 0)
		{
			if (length(newText(p)) == l)
			{
				if (strEqBuf(newText(p), j))
					goto found;
			}
		}
		
		if (newNext(p) == 0)
		{
			if (no_new_control_sequence)
				p = TEX_UNDEFINED_CONTROL_SEQUENCE;
			else
			{
				if (newText(p) > 0)
				{
					do
					{
						if (hashIsFull())
						{
							overFlow(tr("hash size"), TEX_HASH_SIZE);
							return -1;
						}
						hash_used--;
					} while (newText(hash_used) != 0);
					
					setNext(p, hash_used); 
					p = hash_used;
				}
				
				if (!strRoom(l))
					return -1;
				qint32 d = curLength();
				while (pool_ptr > str_start(str_ptr))
				{
					pool_ptr--; 
					str_pool[pool_ptr+l] = str_pool[pool_ptr];
				}
				
				for (qint32 k = j; k < j+l; k++)
					appendChar(buffer[k]);
					
				qint32 newstring = makeString();
				setText(p, newstring); 
				pool_ptr = pool_ptr + d;
#ifdef XW_TEX_STAT
				cs_count++;
#endif //XW_TEX_STAT
			}
			
			goto found;
		}
		
		p = newNext(p);
	}
	
found: 
	return p;
}

bool XWTeX::isNewSource(qint32 srcfilename, qint32 lineno)
{
	QString d = getTexString(srcfilename);
	return (d != last_source_name  || lineno != last_lineno);
}

qint32 XWTeX::length(qint32 s)
{
	qint32 ret = 0;
	if (s <= 0)
		return ret;
		
	if ( s >= 0x10000) 
		ret = str_start(s + 1) - str_start(s);
	else if (s >= 0x20 && s <0x7F) 
		ret = 1;
	else if (s <= 0x7F) 
		ret = 3;
	else if ( s < 0x100) 
		ret = 4;
	else
		ret = 8;
		
	return ret;
}

qint32 XWTeX::makeFullNameString()
{
	qint32 ret = -1;
	if (dev->fullNameOfFile.isEmpty())
		return ret;
		
	ret = makeTexString(dev->fullNameOfFile);
	return ret;
}

qint32 XWTeX::makeNameString()
{
	if ((pool_ptr + name_length > pool_size) || 
		(str_ptr == max_strings) || 
		(curLength() > 0))
	{
		return '?';
	}
	
	qint32 k = 1;
	for (; k <= name_length; k++)
		appendChar(name_of_file[k]);
		
	qint32 ret = makeString();
	k = 1;
	name_in_progress = true;
	beginName();
  stop_at_space = false;
  while ((k <= name_length) && (moreName(name_of_file[k])))
  	k++;
  	
  stop_at_space = true;
  endName();
  name_in_progress = false;
	
	return ret;
}

qint32 XWTeX::makeSrcSpecial(qint32 srcfilename, qint32 lineno)
{
	qint32 oldpoolptr = pool_ptr;
	QString filename = getTexString(srcfilename);
	QString s = QString("src:%1 %2").arg(lineno).arg(filename);
	for (qint32 i = 0; i < s.length(); i++)
		str_pool[pool_ptr++] = s[i].unicode();
		
	return (oldpoolptr);
}

qint32 XWTeX::makeString()
{
	if (str_ptr == max_strings)
	{
		overFlow(tr("number of strings"), max_strings - init_str_ptr);
		return -1;
	}
	
	str_ptr++;
	str_start(str_ptr) = pool_ptr;
	return str_ptr - 1;
}

bool XWTeX::moreName(qint32 c)
{
	if (c == ' ' && stop_at_space && (!quoted_filename))
		return false;
		
	if (c == '"')
	{
		quoted_filename = ! quoted_filename;
		return true;
	}
		
	strRoom(1); 
	appendChar(c);
	if ((c == '\\') || (c == '/') || c == ':')
	{
		area_delimiter = curLength(); 
		ext_delimiter = 0;
	}
	else if (c == '.')
		ext_delimiter = curLength();
		
	return true;
}

void XWTeX::packBufferedName(qint32 n, qint32 a, qint32 b)
{
	qint32 k, j, c;
	if (n + b - a + 1 + 5 > TEX_FILE_NAME_SIZE)
		b = a + TEX_FILE_NAME_SIZE - n - 1 - 5;
		
	if (name_of_file)
		free(name_of_file);
		
	name_of_file = (qint32*)malloc((1 + n + (b - a + 1) + 5 + 4) * sizeof(qint32));
	k = 0;
	for (j = 1; j <= n; j++)
	{
		c = TEX_format_default[j];
		if (c != '"')
		{
			k++;
			if (k <= TEX_FILE_NAME_SIZE)
				name_of_file[k] = c;
		}
	}
	
	for (j = a; j <= b; j++)
	{
		c = buffer[j];
		if (c != '"')
		{
			k++;
			if (k <= TEX_FILE_NAME_SIZE)
				name_of_file[k] = c;
		}
	}
	
	for (j = 17; j <= 21; j++)
	{
		c = TEX_format_default[j];
		if (c != '"')
		{
			k++;
			if (k <= TEX_FILE_NAME_SIZE)
				name_of_file[k] = TEX_format_default[j];
		}
	}
	
	if (k <= TEX_FILE_NAME_SIZE)
		name_length = k;
	else
		name_length = TEX_FILE_NAME_SIZE;
		
	for (k = (name_length + 1); k <= TEX_FILE_NAME_SIZE; k++)
		name_of_file[k] = ' ';
}

void XWTeX::packFileName(qint32 n, qint32 a, qint32 e)
{	
	qint32 tmp = 0;
	if (a >= 0)
		tmp += length(a);
		
	if (n >= 0)
		tmp += length(n);
		
	if (e >= 0)
		tmp += length(e);
		
	if (tmp == 0)
		return ;
		
	if (name_of_file)
		free(name_of_file);	
		
	name_of_file = (qint32*)malloc((tmp + 4) * sizeof(qint32));		
	qint32 k, j, c;
	k = 0;
	if (a > 0)
	{
		for (j = str_start(a); j <= str_start(a + 1) - 1; j++)
		{
			c = str_pool[j];
			if (c != '"')
			{
				k++;
				if (k <= TEX_FILE_NAME_SIZE)
					name_of_file[k] = c;
			}
		}
	}
	
	if (n > 0)
	{
		for (j = str_start(n); j <= str_start(n + 1) - 1; j++)
		{
			c = str_pool[j];
			if (c != '"')
			{
				k++;
				if (k < TEX_FILE_NAME_SIZE)
					name_of_file[k] = c;
			}
		}
	}
	
	if (e > 0)
	{
		for (j = str_start(e); j <= str_start(e + 1) - 1; j++)
		{
			c = str_pool[j];
			if (c != '"')
			{
				k++;
				if (k <= TEX_FILE_NAME_SIZE)
					name_of_file[k] = c;
			}
		}
	}
	
	if (k <= TEX_FILE_NAME_SIZE)
		name_length = k;
	else
		name_length = TEX_FILE_NAME_SIZE;
		
	name_of_file[name_length + 1] = 0;
}

void XWTeX::packJobName(qint32 s)
{
	cur_area = TEX_TOO_BIG_CHAR; 
	cur_ext = s;
	cur_name = job_name; 
	packCurName();
}

void XWTeX::rememberSourceInfo(qint32 srcfilename, qint32 lineno)
{
	last_source_name = getTexString(srcfilename);
  	last_lineno = lineno;
}

qint32 XWTeX::searchString(qint32 search)
{
	qint32 result = 0; 
	qint32 len = length(search);
	if (len == 0)
		return TEX_TOO_BIG_CHAR;
		
	qint32 s = search - 1;
	while (s > 65535)
	{
		if (length(s) == len)
		{
			if (strEqStr(s,search))
			{
				result = s;
				break;
			}
		}
		
		s--;
	}
	
	return result;
}

qint32 XWTeX::slowMakeString()
{
	qint32 t = makeString(); 
	qint32 s = searchString(t);
	if (s > 0)
	{
		flushString(); 
		return s;
	}
	
	return t;
}

bool XWTeX::strEqBuf(qint32 s, qint32 k)
{
	qint32 j = str_start(s);
	while (j < str_start(s+1))
	{
		if (str_pool[j] != buffer[k])
			return false;
			
		j++;
		k++;
	}
	
	return true;
}

bool XWTeX::strEqStr(qint32 s, qint32 t)
{
	if (length(s) != length(t))
		return false;
		
	if (length(s) == 1)
	{
		if (s < 65536)
		{
			if (t < 65536)
			{
				if (s != t)
					return false;
			}
			else 
			{
				if (s != str_pool[str_start(t)])
					return false;
			}
		}
		else
		{
			if (t < 65536)
			{
				if (str_pool[str_start(s)] != t)
					return false;
			}
			else
			{
				if (str_pool[str_start(s)] != str_pool[str_start(t)])
					return false;
			}
		}
	}
	else
	{
		qint32 j = str_start(s); 
		qint32 k = str_start(t);
		while (j < str_start(s + 1))
		{
			if (str_pool[j] != str_pool[k])
				return false;
				
			j++;
			k++;
		}
	}
	
	return true;
}

bool XWTeX::strRoom(qint32 l)
{
	if ((pool_ptr + l) > pool_size)
	{
		overFlow(tr("pool size"), pool_size - init_pool_ptr);
		return false;
	}
	
	return true;
}
