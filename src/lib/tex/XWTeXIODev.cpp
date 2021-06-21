/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <QFile>
#include <QBuffer>
#include <QTextStream>

#include "XWApplication.h"
#include "XWUtil.h"
#include "XWFileName.h"
#include "XWOptionParser.h"
#include "XWDocSea.h"
#include "XWTexSea.h"
#include "XWFontSea.h"
#include "XWPictureSea.h"
#include "XWTeX.h"
#include "XWTeXIODev.h"

#define DUMP_OPTION "fmt"
#define DUMP_EXT ".fmt"
#define INI_PROGRAM "initex"
#define VIR_PROGRAM "virtex"

static int iniversion = 0;
static int haltonerrorp = 1;
static int mltexp = 0;
static int enctexp = 0;
static int shellenabledp = 0;
static int debugformatfile = 0;
static int filelineerrorstylep = 0;
static int parsefirstlinep = 1;
static int eightbitp = 1;

static XWOption long_options[] = 
{
	{ DUMP_OPTION,                 1, 0, 0 },
	{ "help",                      0, 0, 0 },
	{ "ini",                       0, &iniversion, 1 },
	{ "interaction",               1, 0, 0 },
	{ "halt-on-error",             0, &haltonerrorp, 1 },
    { "version",                   0, 0, 0 },
    { "output-comment",            1, 0, 0 },
    { "output-directory",          1, 0, 0 },
    { "shell-escape",              0, &shellenabledp, 1 },
    { "no-shell-escape",           0, &shellenabledp, -1 },
    { "debug-format",              0, &debugformatfile, 1 },
    { "src-specials",              2, 0, 0 },
    { "file-line-error-style",     0, &filelineerrorstylep, 1 },
    { "no-file-line-error-style",  0, &filelineerrorstylep, -1 },
    /* Shorter option names for the above. */
    { "file-line-error",           0, &filelineerrorstylep, 1 },
    { "no-file-line-error",        0, &filelineerrorstylep, -1 },
    { "jobname",                   1, 0, 0 },
    { "parse-first-line",          0, &parsefirstlinep, 1 },
    { "no-parse-first-line",       0, &parsefirstlinep, -1 }
};

XWTeXFile::XWTeXFile(XWTeX   * texA, QObject * parent)
	:QObject(parent),
	 tex(texA),
	 modified(false)
{
	totalPos = 0;
	curPos = 0;
	totalLen = 0;
	fileMode = NoMode;
	parentFile = 0;
	fileNum = 0;
	codec = 0;
}

XWTeXFile::~XWTeXFile()
{	
	QHash<QString, XWTeXFile*>::iterator i = children.begin();
	while (i != children.end())
	{
		XWTeXFile * f = i.value();
		i = children.erase(i);
		delete f;
	}
}

void XWTeXFile::addChild(XWTeXFile * f)
{
	children[f->fileNameInDoc] = f;
}

bool XWTeXFile::atEnd()
{
	return curPos >= text.length();
}

XWTeXFile * XWTeXFile::findFile(const QString & filename)
{
	if (filename == fileNameInDoc)
		return this;
		
	if (children.isEmpty())
		return 0;
		
	QHash<QString, XWTeXFile*>::iterator i = children.begin();
	while (i != children.end())
	{
		XWTeXFile * f = i.value();
		f = f->findFile(filename);
		if (f)
			return f;		
		i++;
	}
	
	return 0;
}

QByteArray XWTeXFile::getByteArray(const QString & txt)
{
	QByteArray b = codec->fromUnicode(txt);
	return b;
}

TeXCategoryCode XWTeXFile::getCategory(qint32 c)
{
	if (c == '\n')
		return EndOfLine;
		
	qint32 cat = tex->catCode(c);
	TeXCategoryCode ret = InvalidChar;
	switch (cat)
	{
		case TEX_ESCAPE:
			ret = Escape;
			break;
			
		case TEX_LEFT_BRACE:
			ret = BeginOfGroup;
			break;
			
		case TEX_RIGHT_BRACE:
			ret = EndingOfGroup;
			break;
			
		case TEX_MATH_SHIFT:
			ret = MathShift;
			break;
			
		case TEX_TAB_MARK:
			ret = AlignmentTab;
			break;
			
		case TEX_CAR_RET:
			ret = EndOfLine;
			break;
			
		case TEX_MAC_PARAM:
			ret = Parameter;
			break;
			
		case TEX_SUP_MARK:
			ret = SuperScript;
			break;
			
		case TEX_SUB_MARK:
			ret = SubScript;
			break;
			
		case TEX_IGNORE:
			ret = IgnoreChar;
			break;
			
		case TEX_SPACER:
			ret = Spacer;
			break;
			
		case TEX_LETTER:
			ret = Letter;
			break;
			
		case TEX_OTHER_CHAR:
			ret = OtherChar;
			break;
			
		case TEX_ACTIVE_CHAR:
			ret = ActiveChar;
			break;
			
		case TEX_COMMENT:
			ret = CommentChar;
			break;
			
		default:
			break;
	}
	
	return ret;
}

TeXCategoryCode XWTeXFile::getCategory()
{
	if (atEnd())
		return EndFile;
		
	qint32 c = getChar();
	curPos--;
	return getCategory(c);
}

qint32 XWTeXFile::getChar()
{
	if (atEnd())
		return 0;
	
	qint32 i = text[curPos].unicode();
	curPos++;
	return i;
}

qint32 XWTeXFile::getControlSequence(qint32 * buf)
{
	TeXCategoryCode cat = getCategory();
	if (cat == Escape)
		getChar();
		
	qint32 ret = 0;
	while (!atEnd())
	{
		buf[ret++] = getChar();
		cat = getCategory();
		if (cat != Letter)
			break;
	}
	
	buf[ret] = 0;
	return ret;
}

QString XWTeXFile::getCoord()
{
	skipSpacerAndComment();
	qint32 c = getChar();
	if (c != '(')
	{
		curPos--;
		return QString();
	}
	
	c = getChar();
	char a = 1;
	QString str;
	while (!atEnd())
	{
		if (c == '(')
			a++;
		else if (c == ')')
		{
			a--;
			if (a == 0)
				break;
		}
		
		str.append(QChar((uint)c));
		c = getChar();
	}
	
	str = str.simplified();
	return str;
}

QString  XWTeXFile::getKey()
{
	skipSpacerAndComment();
	qint32 c = getChar();
	QString str;
	if (c == ']' || c == '}' || c == ')')
		return str;
		
	if (c == ',')
	{
		skipSpacerAndComment();
		c = getChar();
	}
	
	while (!atEnd())
	{
		if (c == '=' || 
			c == ',' || 
			c == ']' || 
			c == '}' || 
			c == ')')
		{	
			curPos--;		
			break;
		}
		str.append(QChar((uint)c));
		c = getChar();
	}
	
	str = str.simplified();
	return str;
}

QString XWTeXFile::getOption()
{
	skipSpacerAndComment();
	qint32 c = getChar();
	QString str;
	if (c != '[')
	{
		curPos--;		
		return str;
	}
	
	skipSpacerAndComment();
	char a = 1;
	TeXCategoryCode cat = getCategory();
	while (!atEnd())
	{
		if (cat == CommentChar)
			skipComment();
		else
		{
			c = getChar();
			if (c == '[')
				a++;
			else if (c == ']')
			{
				a--;
				if (a == 0)
					break;
			}
			
			str.append(QChar((uint)c));
		}
		cat = getCategory();
	}
	
	return str;
}

QString XWTeXFile::getParam()
{
	skipSpacerAndComment();
	TeXCategoryCode cat = getCategory();	
	QString str;
	if (cat != BeginOfGroup)
		return str;
		
	getChar();
	skipSpacerAndComment();
	cat = getCategory();
	char a = 1;
	while (!atEnd())
	{
		if (cat == CommentChar)
			skipComment();
		else if (cat == BeginOfGroup)
		{
			qint32 c = getChar();
			str.append(QChar((uint)c));
			a++;
		}
		else if (cat == EndingOfGroup)
		{
			a--;
			if (a == 0)
				break;
				
			qint32 c = getChar();
			str.append(QChar((uint)c));
		}
		else
		{
			qint32 c = getChar();
			str.append(QChar((uint)c));
		}
		
		cat = getCategory();
	}
	curPos++;
	str = str.simplified();
	return str;
}

qint32  XWTeXFile::getPos()
{
	qint32 p = totalPos + curPos;
	if (children.isEmpty() || fileNum == 0)
		return p;
		
	QHash<QString, XWTeXFile*>::iterator i = children.begin();	
	int k = 0;
	while (i != children.end())
	{
		if (k == fileNum)
			break;
			
		XWTeXFile * f = i.value();
		p += f->totalPos + f->totalLen;
		i++;
		k++;
	}
	
	return p;
}

QString  XWTeXFile::getString(qint32 len)
{
	QString str = text.mid(curPos, len);
	curPos += len;
	return str;
}

qint32  XWTeXFile::getSize()
{
	return (qint32)(text.length());
}

QString XWTeXFile::getText()
{
	QString ret;
	qint32 c = getChar();
	TeXCategoryCode cat = getCategory(c);
	while (!atEnd())
	{
		if (cat == Escape ||  
			  cat == BeginOfGroup || 
			  cat == EndingOfGroup ||  
			  cat == MathShift ||  
			  cat == AlignmentTab ||  
			  cat == CommentChar || 
			  cat == SuperScript || 
			  cat == SubScript)
		{
			curPos--;
			break;
		}
		else if (cat == EndOfLine)
		{
			qint32 cc = getChar();
			cat = getCategory(cc);
			if (cat == EndOfLine ||
				  cat == Escape ||  
			    cat == BeginOfGroup || 
			    cat == EndingOfGroup ||  
			    cat == MathShift ||  
			    cat == AlignmentTab ||  
			    cat == CommentChar || 
			    cat == SuperScript || 
			    cat == SubScript)
			{
				curPos--;
				curPos--;
				break;
			}
			
			ret.append(QChar((uint)(c)));
			ret.append(QChar((uint)(cc)));
		}
		else if (cat == Spacer)
		{
			qint32 cc = getChar();
			cat = getCategory(cc);
			if (cat == Escape || 
				  cat == BeginOfGroup || 
			    cat == EndingOfGroup ||  
			    cat == MathShift ||  
			    cat == AlignmentTab ||  
			    cat == CommentChar || 
			    cat == SuperScript || 
			    cat == SubScript)
			{
				curPos--;
				curPos--;
				break;
			}
			
			ret.append(QChar((uint)(c)));
			ret.append(QChar((uint)(cc)));
		}
		else
			ret.append(QChar((uint)(c)));
		
		c = getChar();
		cat = getCategory(c);
	}
	
	return ret;
}

QString  XWTeXFile::getValue()
{
	skipSpacerAndComment();
	qint32 c = getChar();
	QString ret;
	if (c == ',' || c == ']' || c == '}' || c == ')')
		return ret;
		
	if (c != '=')
		curPos--;
		
	skipSpacerAndComment();
	qint32 pos = curPos;		
	TeXCategoryCode cat = getCategory();
	while (!atEnd())
	{
		switch (cat)
		{
			case BeginOfGroup:
				skipGroup();
				break;
				
			case MathShift:
				skipFormula();
				break;
				
			case CommentChar:
				skipComment();
				break;
				
			default:
				c = getChar();
				if (c == ',' || 
					c == ']' || 
					c == '}' || 
					c == ')')
				{
					goto end;
				}
				else if (c == '[')
				{
					curPos--;
					skipOption();
				}
				break;
		}
		
		cat = getCategory();
	}
	
end:
	ret = text.mid(pos, curPos - pos);
	curPos--;
	return ret;
}

QString XWTeXFile::getWord()
{
	skipSpacerAndComment();
	qint32 c = getChar();
	TeXCategoryCode cat = getCategory(c);
	QString str;
	while (cat == Letter)
	{	
		str.append(QChar((uint)(c)));
		c = getChar();
		cat = getCategory(c);
	}
	curPos--;
	return str;
}

XWTeXFile * XWTeXFile::hitTest(qint32 filepos)
{
	fileNum = 0;
	if (filepos < totalPos || filepos >= (totalPos + totalLen))
		return 0;
	
	if (children.isEmpty())
	{
		curPos = (filepos - totalPos);
		return this;
	}

	QHash<QString, XWTeXFile*>::iterator i = children.begin();
	qint32 pos = 0;
	qint32 p = totalPos;
	while (i != children.end())
	{
		XWTeXFile * f = i.value();
		if (filepos >= f->totalPos && filepos < (f->totalPos + f->totalLen))
			return f->hitTest(filepos);
		else if (filepos < f->totalPos)
			break;
			
		pos += f->totalPos - p;
		p = f->totalPos + f->totalLen;
		
		i++;
		fileNum++;
	}
	
	curPos = (pos + filepos - p);
	return this;
}

qint32 XWTeXFile::insert(const QByteArray & txt)
{
	QString t = codec->toUnicode(txt);
	text.insert(curPos, t);
	curPos += (qint32)(t.length());
	modified = true;
	return t.length();
}

qint32 XWTeXFile::insert(const QString & txt)
{
	text.insert(curPos, txt);
	curPos += (qint32)(txt.length());
	modified = true;
	return (qint32)(txt.length());
}

bool XWTeXFile::load(const QString & filename)
{
	XWDocSea docsea;
	QString fn = docsea.findTex(filename);
	if (fn.isEmpty())
		return false;
		
	QFile fileA(fn);
	if (!fileA.open(QIODevice::ReadOnly))
		return false;
		
	fileName = fn;
	fileNameInDoc = filename;
		
	char buf[1025];
	qint64 len = fileA.read(buf, 1024);
	fileA.seek(0);
	fileMode = NoMode;
	if (len > 2)
	{
		if (buf[0] == 0xe0 || buf[0] == 0x6c)
			fileMode = EBCDICMode;
		else if (buf[0] == 0 && (buf[0] == 0x5c || buf[0] == 0x25))
			fileMode = TwoByteMode;
		else if ((buf[0] == 0x5c || buf[0] == 0x25) && buf[1] == 0)
			fileMode = TwoByteLEMode;
		else
		{
			QByteArray ba = QByteArray::fromRawData(buf, len);
			if (buf[0] == 0x25)
			{
				int l = ba.indexOf("\n");
			  if (l > 2)
			  {
			  	ba = ba.left(l);
			  	if (ba.contains("utf-8") || ba.contains("UTF-8"))
						fileMode = UTF8;
					else if (ba.contains("gb18030-0") || ba.contains("GB18030-0"))
						fileMode = GB18030;
					else if (ba.contains("gbk") || ba.contains("GBK"))
						fileMode = GBK;
					else if (ba.contains("gb") || ba.contains("GB"))
						fileMode = GB;
					else if (ba.contains("big5") || ba.contains("Big5") || ba.contains("BIG5"))
						fileMode = BIG5;
					else if (ba.contains("EUC-JP") || ba.contains("euc-jp"))
						fileMode = EUCJP;
					else if (ba.contains("EUC-KR") || ba.contains("euc-kr"))
						fileMode = EUCKR;
					else if (ba.contains("sjis") || ba.contains("SJIS"))
						fileMode = SJIS;
			  }
			}
			
			if (fileMode == NoMode)
			{
				if (ba.contains("\\usepackage{CJKutf8}"))
					fileMode = UTF8;
			}
				
			if (fileMode == NoMode)
			{
				if (ba.contains("\\begin{CJK}") || ba.contains("\\begin{CJK*}"))
				{
					if (ba.contains("{GBK}"))
						fileMode = GBK;
					else if (ba.contains("{GB}"))
						fileMode = GB;
					else if (ba.contains("{Bg5}"))
						fileMode = BIG5;
					else if (ba.contains("{EUC-JP}"))
						fileMode = EUCJP;
					else if (ba.contains("{KS}"))
						fileMode = EUCKR;
					else if (ba.contains("{HK}"))
						fileMode = HKSCS;
					else if (ba.contains("{KS-HLaTeX}"))
						fileMode = KSLATEX;
					else if (ba.contains("{SJIS}"))
						fileMode = SJIS;
				}
			}
		}
	}
	
	switch (fileMode)
	{
		case TwoByteMode:
			codec = QTextCodec::codecForName("UTF-16BE");
			break;
			
		case TwoByteLEMode:
			codec = QTextCodec::codecForName("UTF-16LE");
			break;
			
		case UTF8:
			codec = QTextCodec::codecForName("UTF-8");
			break;
			
		case GB18030:
			codec = QTextCodec::codecForName("GB18030-0");
			break;
			
		case GBK:
			codec = QTextCodec::codecForName("GBK");
			break;
			
		case GB:
			codec = QTextCodec::codecForName("GB");
			break;
			
		case BIG5:
			codec = QTextCodec::codecForName("Big5");
			break;
			
		case EUCJP:
			codec = QTextCodec::codecForName("EUC-JP");
			break;
			
		case EUCKR:
			codec = QTextCodec::codecForName("EUC-KR");
			break;
			
		case HKSCS:
			codec = QTextCodec::codecForName("Big5-HKSCS");
			break;
			
		case SJIS:
			codec = QTextCodec::codecForName("Shift-JIS");
			break;
			
		default:
			codec = QTextCodec::codecForLocale();
			break;
	}
	
	fileA.seek(0);
	QTextStream inStream(&fileA);
	inStream.setCodec(codec);
	text.clear();
	while (!inStream.atEnd())
	{
		QString line = inStream.readLine(0);
		text.append(line);
		text.append(QChar('\n'));
	}
	
	fileA.close();
	totalLen = text.length();
	return true;
}

QIODevice * XWTeXFile::open()
{
	QIODevice * ret = new QBuffer;	
	ret->open(QBuffer::WriteOnly);
	QTextStream outStream(ret);
	outStream.setCodec(codec);
	outStream << text;
	ret->close();
	ret->open(QBuffer::ReadOnly);
	ret->seek(0);
	return ret;
}

QByteArray XWTeXFile::read(qint32 len)
{
	QString t = text.mid(curPos, len);
	curPos += len;
	QByteArray ret = codec->fromUnicode(t);
	return ret;
}

QString XWTeXFile::readLine()
{
	int i = text.indexOf(QChar('\n'), curPos);	
	i++;
	QString ret = text.mid(curPos, i - curPos);	
	curPos = i;
	return ret;
}

void XWTeXFile::remove(qint32 len)
{
	text.remove(curPos, len);
	modified = true;
}

void XWTeXFile::removeFile(XWTeXFile * f)
{
	QHash<QString, XWTeXFile*>::iterator i = children.begin();
	while (i != children.end())
	{
		if (f == i.value())
		{
			i = children.erase(i);
			delete f;
			break;
		}
		
		i++;
	}
}

qint32 XWTeXFile::replace(qint32 len, const QByteArray & txt)
{
	QString t = codec->toUnicode(txt);
	text.replace(curPos, len, t);
	int l = t.length() - (int)len;
	curPos += (qint32)(t.length());
	modified = true;
	return (qint32)l;
}

qint32 XWTeXFile::replace(qint32 len, const QString & txt)
{
	text.replace(curPos, len, txt);
	int l = txt.length() - (int)len;	
	curPos += (qint32)(txt.length());
	modified = true;
	return (qint32)l;
}

void XWTeXFile::save()
{
	if (modified)
	{
		QString tmp = QString("%1.tmp").arg(fileName);
		QFile f(tmp);
		bool r = false;
		if (f.open(QIODevice::WriteOnly))
		{
			QTextStream outStream(&f);
			outStream.setCodec(codec);
			outStream << text;
			r = true;
			
			f.close();
		}
	
		if (r)
		{
			f.remove(fileName);
			f.rename(fileName);
			modified = false;
		}
		else
			f.remove();
	}
	
	QHash<QString, XWTeXFile*>::iterator i = children.begin();
	while (i != children.end())
	{
		XWTeXFile * f = i.value();
		f->save();
		i++;
	}
}

void XWTeXFile::save(const QString & filename)
{
	if (!fileName.isEmpty())
		return ;
		
	fileName = filename;
	modified = true;
	save();
}

void XWTeXFile::seek(qint32 filepos)
{
	fileNum = 0;
	if (filepos < totalPos || filepos >= (totalPos + totalLen))
		return ;
		
	if (children.isEmpty())
	{
		curPos = (filepos - totalPos);		
		return ;
	}

	QHash<QString, XWTeXFile*>::iterator i = children.begin();
	qint32 pos = 0;
	qint32 p = totalPos;
	while (i != children.end())
	{
		XWTeXFile * f = i.value();
		if (filepos >= f->totalPos && filepos < (f->totalPos + f->totalLen))
		{
			f->seek(filepos);
			return ;
		}
		else if (filepos < f->totalPos)
			break;
			
		pos += f->totalPos - p;
		p = f->totalPos + f->totalLen;
		
		i++;
		fileNum++;
	}
	
	curPos = pos + filepos - p;
}

void XWTeXFile::skipComment()
{
	TeXCategoryCode cat = getCategory();
	while (cat == CommentChar)
	{
		curPos = text.indexOf(QChar('\n'), curPos);
		curPos++;
		cat = getCategory();
	}
}

void XWTeXFile::skipControlSequence()
{
	skipSpacerAndComment();	
	TeXCategoryCode cat = getCategory();
	if (cat == Escape)
	{
		getChar();
		cat = getCategory();
	}
	
	while (!atEnd())
	{
		getChar();
		cat = getCategory();
		if (cat != Letter)
			break;
	}
}

void XWTeXFile::skipCoord()
{
	skipSpacerAndComment();
	qint32 c = getChar();
	if (c != '(')
	{
		curPos--;
		return ;
	}
	
	while (c == '(')
	{
		while (c != ')')
			c = getChar();
			
		skipSpacerAndComment();
		c = getChar();
	}
	
	curPos--;
}

void XWTeXFile::skipFormula()
{
	if (atEnd())
		return ;
		
	qint32 c = getChar();
	TeXCategoryCode cat = getCategory(c);
	if (cat != MathShift)
	{
		curPos--;
		return ;
	}
	
	c = getChar();
	char a = 1;
	cat = getCategory(c);
	if (cat == MathShift)
	{
		a++;
		c = getChar();
		cat = getCategory(c);
	}
	
	while (!atEnd())
	{
		if (cat == MathShift)
		{
			a--;
			if (a == 0)
				break;
		}
		
		c = getChar();
		cat = getCategory(c);
	}
}

void XWTeXFile::skipGroup()
{
	if (atEnd())
		return ;
		
	skipSpacerAndComment();
	qint32 c = getChar();
	TeXCategoryCode cat = getCategory(c);
	if (cat != BeginOfGroup)
	{
		curPos--;
		return ;
	}
	
	int a = 1;
	while (!atEnd())
	{
		c = getChar();
		cat = getCategory(c);
		if (cat == BeginOfGroup)
			a++;
		else if (cat == EndingOfGroup)
			a--;
			
		if (a == 0)
			break;
	}
}

void XWTeXFile::skipOption()
{
	skipSpacerAndComment();
	qint32 c = getChar();
	if (c != '[')
	{
		curPos--;
		return ;
	}
	
	char a = 1;
	while (!atEnd())
	{
		c = getChar();
		if (c == '[')
			a++;
		else if (c == ']')
		{
			a--;
			if (a == 0)
				break;
		}
	}
}

void XWTeXFile::skipSpacer()
{
	if (atEnd())
		return ;
		
	qint32 c = getChar();
	TeXCategoryCode cat = getCategory(c);
	while (cat == Spacer || cat == EndOfLine)
	{
		c = getChar();
		cat = getCategory(c);
	}
	
	curPos--;
}

void XWTeXFile::skipSpacerAndComment()
{
	if (atEnd())
		return ;
	
	qint32 c = getChar();
	TeXCategoryCode cat = getCategory(c);
	while (cat == Spacer || 
	       cat == EndOfLine || 
	       cat == CommentChar)
	{
		if (cat == CommentChar)
		{
			curPos = text.indexOf(QChar('\n'), curPos);
			curPos++;
		}
		
		c = getChar();
		cat = getCategory(c);
	}
	
	curPos--;
}

void XWTeXFile::skipWord()
{
	skipSpacerAndComment();
	qint32 c = getChar();
	TeXCategoryCode cat = getCategory(c);
	while (cat == Letter)
	{	
		c = getChar();
		cat = getCategory(c);
	}
	
	curPos--;
}

void XWTeXFile::updateFilePos(qint32 filepos, 
	                            qint32 l)
{
	if (filepos >= (totalPos + totalLen))
		return ;
		
	if (filepos < totalPos)
		totalPos += l;
	else
		totalLen += l;
		
	QHash<QString, XWTeXFile*>::iterator i = children.begin();
	while (i != children.end())
	{
		XWTeXFile * f = i.value();
		f->updateFilePos(filepos, l);
		i++;
	}
}

XWTeXIODev::XWTeXIODev(QObject * parent)
	:QThread(parent),
	 tex(new XWTeX(this)),
	 etex(false)
{
	dvi_file = 0;
	log_file = 0;
	term_in = 0;
	term_out = 0;
	
	dvi_ptr = 0;
	dvi_offset = 0; 
	dvi_buf_size = 16384;
	half_buf  = dvi_buf_size / 2; 
	dvi_limit = dvi_buf_size; 
	dvi_gone = 0;	
	
	dvi_buf = (uchar*)malloc(dvi_buf_size * sizeof(uchar));
	
	tex->setInteractionOption(TEX_BATCH_MODE);
	ok = false;
}

XWTeXIODev::~XWTeXIODev()
{
	free(dvi_buf);
	
	if (dvi_file)
	{
		dvi_file->close();
		delete dvi_file;
	}
	
	if (term_in)
	{
		term_in->close();
		delete term_in;
	}
	
	if (term_out)
	{
		term_out->close();
		delete term_out;
	}
	
	if (log_file)
	{
		log_file->close();
		delete log_file;
	}
}

void XWTeXIODev::buildFmt(const QString & fmtname, bool etexA)
{
	ok = false;
	tex->setInitVersion(true);
	dumpName = fmtname;
	XWTeXFormatSetting setting;
	setting.setFormat(fmtname);
	mainInputFile = setting.getInput();
	if (mainInputFile.isEmpty())
		mainInputFile = QString("%1.ini").arg(fmtname);
			
	tex->setupBound(fmtname);		
	etex = etexA;	
	if (!log_file)
	{
		QString logname = xwApp->getProductName8();
		logname.remove(QChar(' '));
		logname.append(".log");
		log_file = new QFile(logname);
		log_file->open(QIODevice::WriteOnly);
	}
	else
		log_file->open(QIODevice::WriteOnly | QIODevice::Truncate);
			
	start();
}

void XWTeXIODev::buildFmtNow(const QString & fmtname, bool etexA)
{
	ok = false;
	tex->setInitVersion(true);
	dumpName = fmtname;
	XWTeXFormatSetting setting;
	setting.setFormat(fmtname);
	mainInputFile = setting.getInput();
	if (mainInputFile.isEmpty())
		mainInputFile = QString("%1.ini").arg(fmtname);
			
	tex->setupBound(fmtname);		
	etex = etexA;	
	if (!log_file)
	{
		QString logname = xwApp->getProductName8();
		logname.remove(QChar(' '));
		logname.append(".log");
		log_file = new QFile(logname);
		log_file->open(QIODevice::WriteOnly);
	}
	else
		log_file->open(QIODevice::WriteOnly | QIODevice::Truncate);
			
	dvi_ptr = 0;
	dvi_offset = 0; 
	dvi_buf_size = 16384;
	half_buf  = dvi_buf_size / 2; 
	dvi_limit = dvi_buf_size; 
	dvi_gone = 0;	
			
	if (dvi_file)
		dvi_file->open(QIODevice::WriteOnly | QIODevice::Truncate);
		
	QThread::setTerminationEnabled(true);
	inputFinished = false;
	tex->compile(mainInputFile, etex);
	ok = tex->isOk();
	if (log_file)
		log_file->close();
		
	if (dvi_file)
		dvi_file->close();
}

void XWTeXIODev::compile()
{
	if (mainInputFile.isEmpty() || dumpName.isEmpty())
		return ;
		
	start();
}

bool XWTeXIODev::isIniVersion()
{
	return tex->isIniVersion();
}

bool XWTeXIODev::loadFile(const QString & filename)
{
	ok = false;
	if (tex->isIniVersion())
	{
		XWTexSea texsea;
		mainInputFile = texsea.findFile(filename, false);
	}
	else
	{
		XWDocSea docsea;
		mainInputFile = docsea.findFile(filename, XWDocSea::TEX, false);
	}
	
	if (mainInputFile.isEmpty())
	{
		xwApp->openError(filename, false);
		return false;
	}
	
	XWFileName fn(mainInputFile);
	QString basename = fn.baseName();	
	if (tex->isIniVersion())
		tex->setupBound(basename);	
	else if (dumpName.isEmpty())
	{
		QFile fileA(mainInputFile);
		fileA.open(QIODevice::ReadOnly);
		dumpName = parseFirstLine(&fileA);
		if (dumpName.isEmpty())
			return false;
			
		if (!loadFmt(dumpName))
			return false;
	}
	
	return true;
}

bool  XWTeXIODev::loadFmt(const QString & fmtname)
{
	dumpName.clear();
	ok = false;		
	if (!tex->loadFmt(fmtname))
		return false;
		
	dumpName = fmtname;
	ok = true;
	return true;
}

bool XWTeXIODev::loadFmt()
{
	tex->initialize();
	tex->initOutput();
	return tex->loadFmt();
}

QString XWTeXIODev::parseFirstLine(QIODevice * fileA)
{
	char first_line[1025];
	QString ret;
	if (fileA->readLine(first_line, 1024) > 2)
	{
		if (first_line[0] == '%' && first_line[1] == '&')
		{
			QTextCodec * codec = QTextCodec::codecForLocale();
			char * s = first_line + 2;
			while (ISBLANK(*s))
				s++;
						
			int npart = 0;
			char *part[4];
			while (*s && npart != 3)
			{
				part[npart++] = s;
				while (*s && *s != ' ') 
					s++;
        		while (*s == ' ') 
        			*s++ = '\0';
			}
					
			part[npart] = NULL;
      char ** parse = part;
      if (*parse && **parse != '-')
      {
      	ret = codec->toUnicode(part[0]);
      	parse++;
      }
		}
	}
	return ret;
}

bool XWTeXIODev::parseOptions(int argc, char ** argv)
{
	XWOptionParser parser(argc, argv, &long_options[0], 0);
	int option_index = 0;
	int g = -1;
	while(true)
	{
		g = parser.getOptLong("+", &option_index);
		if (g == -1)
			break;
			
		if (g == '?')
			continue;
			
		if (parser.isArgument(option_index, "progname"))
			continue;
		else if (parser.isArgument(option_index, "jobname"))
		{
			jobName = parser.optArgToQString();
			jobName = normalizeQuotes(jobName);
		}
		else if (parser.isArgument(option_index, DUMP_OPTION))
			dumpName = parser.optArgToQString();
		else if (parser.isArgument(option_index, "output-directory"))
			outputDirectory = parser.optArgToQString();
		else if (parser.isArgument(option_index, "output-comment"))
			outputComment = parser.optArgToQString();
		else if (parser.isArgument(option_index, "src-specials"))
		{
			char * opt_list = parser.getOptArg();
			tex->parseSrcSpecialsOption(opt_list);
		}			
		else if (parser.isArgument(option_index, "interaction"))
		{
			char * optarg = parser.getOptArg();
			tex->parseInterActionOption(optarg);
		}
	}
	
	etex = false;
	option_index = parser.getOptInd();
	if (option_index < argc && (argv[option_index][0] == '*') && iniversion)
	{
		etex = true;
		option_index++;
	}
	
	QTextCodec * codec = QTextCodec::codecForLocale();
	if (option_index < argc && argv[option_index][0] == '&')
	{
		dumpName = codec->toUnicode((const char*)(&(argv[option_index][1])));
		option_index++;
	}
	
	if (option_index < argc)
	{
		mainInputFile = codec->toUnicode((const char*)(&(argv[option_index][0])));
		if (iniversion)
		{
			XWTexSea texsea;
			mainInputFile = texsea.findFile(mainInputFile, false);
		}
		else
		{
			XWDocSea docsea;
			mainInputFile = docsea.findFile(mainInputFile, XWDocSea::TEX, false);
		}
		
		option_index++;
	}
	
	if (mainInputFile.isEmpty())
		return false;
		
	if (parsefirstlinep < 0)
		parsefirstlinep = false;
		
	if (parsefirstlinep && !mainInputFile.isEmpty() && dumpName.isEmpty())
	{
		XWDocSea docsea;
		QFile * fileA = docsea.openTex(mainInputFile);
		if (fileA)
		{
			dumpName = parseFirstLine(fileA);			
			fileA->close();
			delete fileA;
		}
	}
	
	tex->setHaltOnError(haltonerrorp);
	tex->setInitVersion(iniversion);
	tex->setShellEnabled(shellenabledp);
	tex->setFileLineErrorStyle(filelineerrorstylep);
	tex->setParseFirstLine(parsefirstlinep);
	if (!outputComment.isEmpty())
		tex->setOutputComment(outputComment);
	
	if (tex->isIniVersion())
	{
		if (!jobName.isEmpty())
			tex->setupBound(jobName);
		else
		{
			XWFileName fn(mainInputFile);
			QString basename = fn.baseName();
			tex->setupBound(basename);		
		}
		
		return true;
	}
	else if (!dumpName.isEmpty())
		return tex->loadFmt(dumpName);
	
	return false;
}

void XWTeXIODev::setDebugFormatFile(bool e)
{
	tex->setDebugFormatFile(e);
}

void XWTeXIODev::setFileLineErrorStyle(bool e)
{
	tex->setFileLineErrorStyle(e);
}

void XWTeXIODev::setHaltOnError(bool e)
{
	tex->setHaltOnError(e);
}

void XWTeXIODev::setInsertSrcSpecialAuto(bool e)
{
	tex->setInsertSrcSpecialAuto(e);
}

void XWTeXIODev::setInsertSrcSpecialEveryCr(bool e)
{
	tex->setInsertSrcSpecialEveryCr(e);
}

void XWTeXIODev::setInsertSrcSpecialEveryDisplay(bool e)
{
	tex->setInsertSrcSpecialEveryDisplay(e);
}

void XWTeXIODev::setInsertSrcSpecialEveryHBox(bool e)
{
	tex->setInsertSrcSpecialEveryHBox(e);
}

void XWTeXIODev::setInsertSrcSpecialEveryMath(bool e)
{
	tex->setInsertSrcSpecialEveryMath(e);
}

void XWTeXIODev::setInsertSrcSpecialEveryPar(bool e)
{
	tex->setInsertSrcSpecialEveryPar(e);
}

void XWTeXIODev::setInsertSrcSpecialEveryParEnd(bool e)
{
	tex->setInsertSrcSpecialEveryParEnd(e);
}

void XWTeXIODev::setInsertSrcSpecialEveryVBox(bool e)
{
	tex->setInsertSrcSpecialEveryVBox(e);
}

void XWTeXIODev::setInteractionOption(qint32 i)
{
	tex->setInteractionOption(i);
}

void XWTeXIODev::setIniVersion(bool e)
{
	tex->setInitVersion(e);
}

void XWTeXIODev::setMainInputFile(const QString & filename)
{
	mainInputFile = filename;
}

void XWTeXIODev::setOutputComment(const QString & s)
{
	tex->setOutputComment(s);
}

void XWTeXIODev::setOutputDirectory(const QString & s)
{
	outputDirectory = s;
}

void XWTeXIODev::setParseFirstLine(bool e)
{
	tex->setParseFirstLine(e);
}

void XWTeXIODev::setShellEnabled(bool e)
{
	tex->setShellEnabled(e);
}

void XWTeXIODev::setSrcSpecialsP(bool e)
{
	tex->setSrcSpecialsP(e);
}

QIODevice * XWTeXIODev::takeDviFile()
{
	QIODevice * f = dvi_file;
	dvi_file = 0;
	return f;
}

void XWTeXIODev::setInputText(const QString & txt)
{
	QMutexLocker locker(&myMutex);
	inputStr = txt;
	inputFinished = true;
}

void XWTeXIODev::dviBop()
{
	dviOut((uchar)DVI_BOP); 
}

void XWTeXIODev::dviDownRight(qint32 o, 
	                          qint32 w)
{
	if (qAbs(w) >= 0x800000)
  	{
  		dviOut((uchar)(o + 3));
  		dviFour(w); 
  		return;
	}
	
	if (qAbs(w) >= 0x8000)
	{
  		dviOut((uchar)(o + 2));
  		if (w < 0)
  			w = w + 0x1000000;
  		dviOut((uchar)(w / 0x10000)); 
  		w = w % 0x10000; 
  		goto l2;
  	}
  	
	if (qAbs(w) >= 0x80)
	{
  		dviOut((uchar)(o + 1));
  		if (w < 0) 
  			w = w + 0x10000;
  		goto l2;
  	}
  	
	dviOut((uchar)o);
	if (w < 0) 
		w = w + 0x100;
	goto l1;
l2: 
	dviOut((uchar)(w / 0x100));
l1: 
	dviOut((uchar)(w % 0x100));
}

void XWTeXIODev::dviEnd()
{
	qint32 k =  4 + ((dvi_buf_size - dvi_ptr) % 4);
	while (k > 0)
	{
		dviOut((uchar)223); 
		k--;
	}
	if (dvi_limit == half_buf)
		writeDvi(half_buf, dvi_buf_size - 1);
		
	if (dvi_ptr > 0)
		writeDvi(0, dvi_ptr - 1);
}

void XWTeXIODev::dviEop()
{
	dviOut((uchar)DVI_EOP);
}

void XWTeXIODev::dviPut(qint32 , 
	                    qint32 ocmd, 
	                    qint32 oval)
{
	outCmd(ocmd, oval);
}

void XWTeXIODev::dviPutRule(qint32 , 
	                        qint32 ht,
	                        qint32 wd)
{
	dviOut((uchar)DVI_PUTRULE);
	dviFour(ht);
	dviFour(wd);
}

void XWTeXIODev::dviSet(qint32 , 
	                    qint32 ocmd, 
	                    qint32 oval)
{
	outCmd(ocmd, oval);
}

void XWTeXIODev::dviSetRule(qint32 , 
	                        qint32 ht,
	                        qint32 wd)
{
	dviOut((uchar)DVI_SETRULE);
	dviFour(ht);
	dviFour(wd);
}

void  XWTeXIODev::dviTwo(qint32 s)
{
	dviOut((uchar)(s / 256));
	dviOut((uchar)(s % 256));
}

void XWTeXIODev::dviSwap()
{
	if (dvi_limit == dvi_buf_size)
	{
		writeDvi(0, half_buf - 1); 
		dvi_limit = half_buf;
  		dvi_offset = dvi_offset + dvi_buf_size; 
  		dvi_ptr = 0;
	}
	else
	{
		writeDvi(half_buf, dvi_buf_size-1); 
		dvi_limit = dvi_buf_size;
	}
	
	dvi_gone = dvi_gone + half_buf;
}

void XWTeXIODev::dviXXX(qint32 , qint32 l)
{
	if (l < 256)
	{
		dviOut((uchar)DVI_XXX1);
		dviOut(l);
	}
	else
	{
		dviOut((uchar)DVI_XXX4);
		dviFour(l);
	}
}

XWTeXFile * XWTeXIODev::newTeXFile()
{
	return new XWTeXFile(tex);
}

QIODevice * XWTeXIODev::openInput(const QString & filename)
{
	QIODevice * ret = 0;
	XWFileName fn(filename);
	if (fn.isLocal())
	{
		if (tex->isIniVersion())
		{
			XWTexSea sea;
		  fullNameOfFile = sea.findFile(filename, dumpName);
			if (!fullNameOfFile.isEmpty())
		    ret = sea.openFile(fullNameOfFile);
		}
		else
		{
			XWDocSea sea;
		  fullNameOfFile = sea.findFile(filename);
			if (fullNameOfFile.isEmpty())
			{
				XWTexSea tsea;
				fullNameOfFile = tsea.findFile(filename, dumpName);
				if (!fullNameOfFile.isEmpty())
				  ret = tsea.openFile(fullNameOfFile);
			}
			else
			  ret = sea.openTex(fullNameOfFile);
		}

		if (!ret && !outputDirectory.isEmpty())
		{
			QString n = fn.fileName();		
			QString f = QString("%1/%2").arg(outputDirectory).arg(n);	
			if (QFile::exists(f))
			{
				ret = new QFile(f);
				ret->open(QIODevice::ReadOnly);
			}
		}
	
		if (!ret)
		{
			XWPictureSea sea;		
			fullNameOfFile = sea.findFile(filename);
			if (!fullNameOfFile.isEmpty())
			{
				bool is_tmp = false;
				ret = sea.openFile(fullNameOfFile, &is_tmp);
			}
		}
	
		if (!ret)
		{
			XWBibTexSea sea;
			fullNameOfFile = sea.findFile(filename, XWBibTexSea::BST, false);
			if (fullNameOfFile.isEmpty())
				fullNameOfFile = sea.findFile(filename, XWBibTexSea::BIB, false);
			if (!fullNameOfFile.isEmpty())
				ret = sea.openFile(fullNameOfFile);
		}
	
		if (!ret)
		{
			XWMakeIndexSea sea;
			fullNameOfFile = sea.findFile(filename, XWMakeIndexSea::IST, false);
			if (!fullNameOfFile.isEmpty())
				ret = sea.openFile(fullNameOfFile);
		}
	
		if (!ret)
		{
			XWFontSea sea;
			fullNameOfFile = sea.findFile(filename);
			if (!fullNameOfFile.isEmpty())
				ret = sea.openFile(fullNameOfFile);
		}
	}
	else
	{
		emit needFile(filename);
		while (true)
		{
			QMutexLocker locker(&myMutex);
			if (inputFinished)
				break;
		}
	
		inputFinished = false;
		if (!inputStr.isEmpty())
		{
			ret = new QFile(inputStr);
			ret->open(QIODevice::ReadOnly | QIODevice::Text);
		}
	}
	
	return ret;
}

QFile * XWTeXIODev::openOutput(const QString & filename)
{
	XWFileName fn(filename);
	QString n = fn.fileName();
	if (outputDirectory.isEmpty())
		outputDirectory = xwApp->getTmpDir();

	QString outfilename = QString("%1/%2").arg(outputDirectory).arg(n);	
	QFile * f = new QFile(outfilename);
	if (!f->open(QIODevice::WriteOnly | QIODevice::Text))
	{
		delete f;
		f = 0;
	}
	
	return f;
}

void XWTeXIODev::openLogFile()
{
	if (!log_file)
	{
		QString logname = xwApp->getProductName8();
		logname.remove(QChar(' '));
		logname.append(".log");
		log_file = new QFile(logname);
		log_file->open(QIODevice::WriteOnly);
	}
}

void XWTeXIODev::quit(int)
{
	QMutexLocker locker(&myMutex);
	ok = false;
	inputFinished = true;
//	if (log_file)
//		log_file->close();
		
//	if (dvi_file)
//		dvi_file->close();
//	terminate();
}

void XWTeXIODev::run()
{		
	dvi_ptr = 0;
	dvi_offset = 0; 
	dvi_buf_size = 16384;
	half_buf  = dvi_buf_size / 2; 
	dvi_limit = dvi_buf_size; 
	dvi_gone = 0;	
	
	if (log_file)
		log_file->open(QIODevice::WriteOnly | QIODevice::Truncate);
			
	if (dvi_file)
		dvi_file->open(QIODevice::WriteOnly | QIODevice::Truncate);
		
	QThread::setTerminationEnabled(true);
	inputFinished = false;
	tex->compile(mainInputFile, etex);
	ok = tex->isOk();
	if (log_file)
		log_file->close();
		
	if (dvi_file)
		dvi_file->close();
}

QString XWTeXIODev::termInput()
{
	emit requestInput();
	while (true)
	{
		QMutexLocker locker(&myMutex);
		if (inputFinished)
			break;
	}
	
	inputFinished = false;
	return inputStr;
}

void XWTeXIODev::termOut(const QString & s)
{
	emit messageAdded(s);
}

void XWTeXIODev::termPutChar(qint32 c)
{
	QString s(QChar((uint)c));
	termOut(s);
}

void XWTeXIODev::writeDvi(qint32 a, qint32 b)
{
	if (!dvi_file)
		return ;
		
	char * p = (char*)(&dvi_buf[a]);
	qint32 len = b - a + 1;
	dvi_file->write(p, len);
}

XWTeXStdIODev::XWTeXStdIODev(QObject * parent)
	:XWTeXIODev(parent)
{
	QFile * fterm = new QFile(0);
	term_in = fterm;
	fterm->open(stdin, QIODevice::ReadOnly);
	fterm = new QFile(0);
	term_out = fterm;
	fterm->open(stderr, QIODevice::WriteOnly);
}

