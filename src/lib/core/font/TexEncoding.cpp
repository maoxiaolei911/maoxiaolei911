/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <QCoreApplication>
#include <QFile>
#include <QByteArray>
#include "XWUtil.h"
#include "XWFontSea.h"
#include "XWApplication.h"
#include "FontEncodingTables.h"
#include "TexEncoding.h"

Encoding staticencoding =
{
  "TeX text",
  {"Gamma", "Delta", "Theta", "Lambda",
   "Xi", "Pi", "Sigma", "Upsilon",
   "Phi", "Psi", "Omega", "arrowup",
   "arrowdown", "quotesingle", "exclamdown", "questiondown",

   "dotlessi", "dotlessj", "grave", "acute",
   "caron", "breve", "macron", "ring",
   "cedilla", "germandbls", "ae", "oe",
   "oslash", "AE", "OE", "Oslash",

   "space", "exclam", "quotedbl", "numbersign",
   "dollar", "percent", "ampersand", "quoteright",
   "parenleft", "parenright", "asterisk", "plus",
   "comma", "hyphen", "period", "slash",

   "zero", "one", "two", "three",
   "four", "five", "six", "seven",
   "eight", "nine", "colon", "semicolon",
   "less", "equal", "greater", "question",

   "at", "A", "B", "C",
   "D", "E", "F", "G",
   "H", "I", "J", "K",
   "L", "M", "N", "O",

   "P", "Q", "R", "S",
   "T", "U", "V", "W",
   "X", "Y", "Z", "bracketleft",
   "backslash", "bracketright", "circumflex", "underscore",

   "quoteleft", "a", "b", "c",
   "d", "e", "f", "g",
   "h", "i", "j", "k",
   "l", "m", "n", "o",

   "p", "q", "r", "s",
   "t", "u", "v", "w",
   "x", "y", "z", "braceleft",
   "bar", "braceright", "tilde", "dieresis",

   ".notdef", ".notdef", ".notdef", ".notdef",
   ".notdef", ".notdef", ".notdef", ".notdef",
   ".notdef", ".notdef", ".notdef", ".notdef",
   ".notdef", ".notdef", ".notdef", ".notdef",

   ".notdef", ".notdef", ".notdef", ".notdef",
   ".notdef", ".notdef", ".notdef", ".notdef",
   ".notdef", ".notdef", ".notdef", ".notdef",
   ".notdef", ".notdef", ".notdef", ".notdef",
   
   ".notdef", ".notdef", ".notdef", ".notdef",
   ".notdef", ".notdef", ".notdef", ".notdef",
   ".notdef", ".notdef", ".notdef", ".notdef",
   ".notdef", ".notdef", ".notdef", ".notdef",

   ".notdef", ".notdef", ".notdef", ".notdef",
   ".notdef", ".notdef", ".notdef", ".notdef",
   ".notdef", ".notdef", ".notdef", ".notdef",
   ".notdef", ".notdef", ".notdef", ".notdef",

   ".notdef", ".notdef", ".notdef", ".notdef",
   ".notdef", ".notdef", ".notdef", ".notdef",
   ".notdef", ".notdef", ".notdef", ".notdef",
   ".notdef", ".notdef", ".notdef", ".notdef",

   ".notdef", ".notdef", ".notdef", ".notdef",
   ".notdef", ".notdef", ".notdef", ".notdef",
   ".notdef", ".notdef", ".notdef", ".notdef",
   ".notdef", ".notdef", ".notdef", ".notdef",

   ".notdef", ".notdef", ".notdef", ".notdef",
   ".notdef", ".notdef", ".notdef", ".notdef",
   ".notdef", ".notdef", ".notdef", ".notdef",
   ".notdef", ".notdef", ".notdef", ".notdef",

   ".notdef", ".notdef", ".notdef", ".notdef",
   ".notdef", ".notdef", ".notdef", ".notdef",
   ".notdef", ".notdef", ".notdef", ".notdef",
   ".notdef", ".notdef", ".notdef", ".notdef",
  }
};


void freeEncoding(Encoding * e)
{
	if (e && (e != &staticencoding))
    {
        delete [] e->name;
        for (int i = 0; i < 256; i++)
        {
            char * p = e->vec[i];
            if (p)
                delete [] p;
        }

        delete e;
    }
}

char ** getBaseEnc(const QString & encname)
{
	char ** baseEnc = 0;
	if (encname == "WinAnsiEncoding")
		baseEnc = winAnsiEncoding;
	else if (encname == "MacRomanEncoding")
		baseEnc = macRomanEncoding;
	else if (encname == "MacExpertEncoding")
		baseEnc = macExpertEncoding;
	else if (encname == "Symbol")
		baseEnc = symbolEncoding;
	else if (encname == "Standard")
		baseEnc = standardEncoding;
	else if (encname == "Expert")
		baseEnc = expertEncoding;
	else if (encname == "ZapfDingbats")
		baseEnc = zapfDingbatsEncoding;
		
	return baseEnc;
}

#define TEXENC_PREMATURE 0
#define TEXENC_FIRST     1
#define TEXENC_SECOND    2
#define TEXENC_THREE     3
#define TEXENC_INVALID   4
#define TEXENC_LAST      5

static const char *encerror_strings[] = {
	QT_TRANSLATE_NOOP("texenc", "premature end in encoding file "),
	QT_TRANSLATE_NOOP("texenc", "first token in encoding must be literal encoding name"),
	QT_TRANSLATE_NOOP("texenc", "second token in encoding must be mark ([) token"),
	QT_TRANSLATE_NOOP("texenc", "tokens 3 to 257 in encoding must be literal names"),
	QT_TRANSLATE_NOOP("texenc", "invalid encoding token"),
	QT_TRANSLATE_NOOP("texenc", "token 258 in encoding must be make-array (])")
};

static QString getErrStr(int type)
{
	return qApp->translate("texenc", encerror_strings[type]);
}

static char * getToken(char **bufferp, ulong *offsetp, QFile *f, bool init)
{
	static char *curp;
	
	if (init)
    	curp = NULL;
    	
    char *p, *q;
    while (1)
    {
    	while (curp == NULL || *curp == '\0')
    	{
      		if (*bufferp)
        		free(*bufferp);
        		
        	*bufferp = 0;

      		if (!getLine(bufferp, f))
      		{
      			QString msg = getErrStr(TEXENC_PREMATURE);
      			if (*bufferp)
        			free(*bufferp);
        		*bufferp = 0;
      			msg += QString("'%1'.\n").arg(f->fileName());
      			xwApp->error(msg);
      			return 0;
        	}

      		curp = *bufferp;

      		for (p = *bufferp; *p; p++)
      		{
        		if (*p == '%')
        		{
          			*p = '\0';
          			break;
        		}
    		}
    	}
    	
    	while (isspace(*curp))
      		curp++;

    	*offsetp = curp - *bufferp;

    	if (*curp)
    	{
      		if (*curp == '[' || *curp == ']' || *curp == '{' || *curp == '}')
        		q = curp++;
      		else if (*curp == '/' ||
               		*curp == '-' || 
               		*curp == '_' || 
               		*curp == '.' || 
               		('0' <= *curp && *curp <= '9') ||
               		('a' <= *curp && *curp <= 'z') ||
               		('A' <= *curp && *curp <= 'Z'))
      		{
        		q = curp++;
        		while (*curp == '-' || 
        		       *curp == '_' || 
        		       *curp == '.' || 
               			('0' <= *curp && *curp <= '9') ||
               			('a' <= *curp && *curp <= 'z') || 
               			('A' <= *curp && *curp <= 'Z'))
                {
          			curp++;
          	    }
      		}
      		else
        		q = curp;

      		char tempchar = *curp;
      		*curp = '\0';
      		p = qstrdup(q);
      		*curp = tempchar;
      		return p;
    	}
    }
    
    return 0;
}

Encoding * getStaticEncoding()
{
	return &staticencoding;
}

bool readTexEnc(const QString & encname, char ** name, char**vec)
{
	int ret = false;
	char ** baseEnc = getBaseEnc(encname);
    if (baseEnc)
    {
    	if (name)
    	{
    		QByteArray ba = encname.toAscii();
    		*name = qstrdup(ba.constData()); 
    	}
    	
    	for (int i = 0; i < 256; i++)
    	{
    		if (baseEnc[i] != NULL)
    			vec[i] = qstrdup(baseEnc[i]);
    		else
    			vec[i] = qstrdup(".notdef");
    	}
    	
    	ret = true;
    }
    else if (!encname.isEmpty())
    {
    	XWFontSea fontsea;    	
    	QFile * enc_file = fontsea.openEnc(encname);
        if (!enc_file)
        {
            xwApp->openError(encname, false);
            return false;
        }
        
        char * buffer = 0;
        char numbuf[9];
        ulong offset = 0;
        char * p = getToken(&buffer, &offset, enc_file, true);
        if (!p || *p != '/' || p[1] == '\0')
        {
        	QString msg = getErrStr(TEXENC_FIRST);
        	msg += QString("(%1).\n").arg(enc_file->fileName());
            xwApp->error(msg);
            goto endenc;
        }
        
        if (name)
        	*name = qstrdup(p + 1);
        	
        delete [] p;
        p = getToken(&buffer, &offset, enc_file, false);
        if (!p || strcmp(p, "["))
        {
        	QString msg = getErrStr(TEXENC_SECOND);
        	msg += QString("(%1).\n").arg(enc_file->fileName());
            xwApp->error(msg);
            goto endenc;
        }
        delete [] p;
        
        for (int i = 0; i < 256; i++)
        {
        	p = getToken(&buffer, &offset, enc_file, false);
            if (!p || *p != '/' || p[1] == 0)
            {
                QString msg = getErrStr(TEXENC_THREE);
        		msg += QString("(%1).\n").arg(enc_file->fileName());
            	xwApp->error(msg);
            	ret = false;
            	goto endenc;
            }
            
            char c = p[2];
            if (p[1] == '.' && (c == 'c' || c == 'g') && '0' <= p[3] && p[3] <= '9')
            {
                char * q = 0;
                long l = strtol(p + 3, &q, 0);
                if (*q != '\0' || l < 0 || l > 0x16FFFF)
                {
                	QString msg = getErrStr(TEXENC_INVALID);
                    msg += QString("(%1).\n").arg(enc_file->fileName());
                    xwApp->error(msg);
                    goto endenc;
                }

                sprintf(numbuf, ".%c0x%lx", c, l);
                vec[i] = qstrdup(numbuf);
            }
            else
                vec[i] = qstrdup(p + 1);

            delete [] p;
        }
        
        p = 0;
        ret = true;
        
endenc:
		if (p)
            delete [] p;
            
        if (buffer)
        	free(buffer);

        enc_file->close();
        delete enc_file;
    }
    
    return ret;
}

