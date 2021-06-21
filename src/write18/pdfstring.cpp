/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdio.h>
#include <QCoreApplication>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextCodec>

static void usage(void)
{
	printf("Usage 1 : pdfstring filename.out\n");
	printf("Usage 2 : pdfstring -u filename.out\n"
	       "\t Use -u option, filename.out can not be overwrited in next latex run\n");
}

int main(int argc, char **argv)
{
	QCoreApplication app(argc, argv);
	
	QStringList arguments = QCoreApplication::arguments();
	arguments.takeFirst();
	if (arguments.isEmpty())
	{
		usage();
		return 0;
	}
	
	if (0 == arguments[0].compare("-h", Qt::CaseInsensitive))
	{
		usage();
		arguments.takeFirst();
		if (arguments.isEmpty())
			return 0;
	}
	
	bool w_bk_relax = false;
	if (0 == arguments[0].compare("-u", Qt::CaseInsensitive))
	{
		w_bk_relax = true;
		arguments.takeFirst();
		if (arguments.isEmpty())
		{
			usage();
			return 0;
		}
	}
		
	QString filename = arguments[0];
	if (!filename.endsWith(".out", Qt::CaseInsensitive))
		filename += ".out";
	
	QByteArray env = qgetenv("OUTPUT_PATH");
	QString inname;
	QString outname;
  if (!env.isEmpty())
  {
		QTextCodec * codec = QTextCodec::codecForLocale();
		QString outputpath = codec->toUnicode(env);
		inname = QString("%1/%2").arg(outputpath).arg(filename);
		outname = QString("%1/%2.tmp").arg(outputpath).arg(filename);
	}
	else
	{
		inname = filename;
		outname = QString("%1.tmp").arg(filename);
	}
	
	QFile infile(inname);
	if (!infile.open(QIODevice::ReadOnly | QIODevice::Text))
		return 0;
		
	QByteArray bk("\\let\\WriteBookmarks\\relax\n");
	qint64 RDBUF = 16384;
	QByteArray line = infile.readLine(RDBUF);
	if (line.startsWith(bk))
	{
		infile.close();
		return 0;
	}
	
	QByteArray lbk("\\\n");
	if (line.endsWith(lbk))
	{
		line.replace(line.length() - 2, 2, " ");
		QByteArray t = infile.readLine(RDBUF);
		while (t.endsWith(lbk))
		{
			t.replace(t.length() - 2, 2, " ");
			line += t;
			t = infile.readLine(RDBUF);
		}
		
		line += t;
	}
	
	int i = line.indexOf('{', 0);
	if (i > 0)
	{
		i++;
		int b = 1;
		int l = line.length();
		while (i < l)
		{
			if (line[i] == '{')
				b++;
				
			if (line[i] == '}')
				b--;
				
			if (b == 0)
				break;
				
			i++;
		}
		i = line.indexOf('{', i);
	}
		
	if (i <= 0)
	{
		infile.close();
		return 0;
	}
	
	QByteArray be("\\376\\377");	
	int j = line.indexOf(be, 0);	
	i++;
	if (i == j)
	{
		infile.close();
		return 0;
	}
	
	QFile outfile(outname);
	if (!outfile.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		infile.close();
		return 1;
	}
	
	if(w_bk_relax)
		outfile.write(bk);
	
	char buf[10];
	do
	{
		j = i;
		QByteArray r;
		int l = line.length();
		int balance = 0;
		while (i < l)
		{
			if (line[i] == '^')
			{
				i++;
				if (line[i] == '^')
				{
					i++;
					char c = line[i++];
					char cc = line[i];
					uchar chr;
					if (c <= '9')
						chr = c - '0';
					else
						chr = c - 'a' + 10;
						
					if (cc <= '9')
						chr = 16 * chr + cc - '0';
					else
						chr = 16 * chr + cc - 'a' + 10;
						
					r.append((char)chr);
				}
				else
					r.append(line[i]);
			}
			else
			{
				if (line[i] == '{')
						balance++;
						
				if (line[i] == '}')
				{
					if (balance == 0)
				 		break;
				  		
				 	balance--;
				}
				  
				r.append(line[i]);
			}
			
			i++;
		}
		
		QByteArray odata = line.left(j);
		outfile.write(odata);
		outfile.write(be);
		
		QString ucs = QString::fromUtf8(r.data(), r.length());
		QChar * p = ucs.data();
		while (!p->isNull())
		{
			ushort low = (p->unicode() & 0xff);
			ushort hi = ((p->unicode() >> 8) & 0xff);
			l = sprintf(buf, "\\%03o\\%03o", hi, low);
			outfile.write(buf, l);
			p++;
		}
		
		odata = line.mid(i);
		outfile.write(odata);
		
		line = infile.readLine(RDBUF);
		while (line == "\n")
		  line = infile.readLine(RDBUF);
		if (line.endsWith(lbk))
		{
			line.replace(line.length() - 2, 2, " ");
		  QByteArray t = infile.readLine(RDBUF);
		  while (t.endsWith(lbk))
		  {
			  t.replace(t.length() - 2, 2, " ");
			  line += t;
			  t = infile.readLine(RDBUF);
		  }
		
		  line += t;
		}
		i = line.indexOf('{', 0);
		if (i > 0)
		{
			i++;
		  int b = 1;
		  l = line.length();
		  while (i < l)
		  {
			  if (line[i] == '{')
				  b++;
				
			  if (line[i] == '}')
				  b--;
				
			  if (b == 0)
				  break;
				  
				i++;
		  }
		  i = line.indexOf('{', i);
		}
		i++;
	} while (!line.isEmpty());
	
	infile.close();
	outfile.close();
	if (infile.remove())
		outfile.rename(inname);
	else
		outfile.remove();
	return app.exec();
}
