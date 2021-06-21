/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <math.h>
#include "XWUtil.h"
#include "XWApplication.h"
#include "XWFontSea.h"
#include "XWTFMFile.h"
#include "XWFontFilePK.h"

#ifdef  ROUND
#undef  ROUND
#endif

#define ROUND(n,acc) (floor(((double)n)/(acc)+0.5)*(acc)) 


#define PK_XXX1  240
#define PK_XXX2  241
#define PK_XXX3  242
#define PK_XXX4  243
#define PK_YYY   244
#define PK_POST  245
#define PK_NO_OP 246
#define PK_PRE   247


#ifndef PKFONT_DPI_DEFAULT
#define PKFONT_DPI_DEFAULT 600u
#endif


static int
p_itoa(long value, char *buf)
{
  	char *p = buf;
	int   sign = 0;
  	if (value < 0) 
  	{
    	*p++  = '-';
    	value = -value;
    	sign  = 1;
  	} 

  	int ndigits = 0;
  	do 
  	{
    	p[ndigits++] = (value % 10) + '0';
    	value /= 10;
  	} while (value != 0);

    for (int i = 0; i < ndigits / 2 ; i++) 
    {
     	char tmp = p[i];
      	p[i] = p[ndigits-i-1];
      	p[ndigits-i-1] = tmp;
    }
    
  	p[ndigits] = '\0';

  	return  (sign ? ndigits + 1 : ndigits);
}

static int
p_dtoa(double value, int prec, char *buf)
{
  	const long p[10] = { 
  		1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000 };
  		
  	char *c = buf;
  	int n = 0;
  	if (value < 0) 
  	{
    	value = -value;
    	*c++ = '-';
    	n = 1;
  	} 

  	long i = (long) value;
  	long f = (long) ((value-i)*p[prec] + 0.5);

  	if (f == p[prec]) 
  	{
    	f = 0;
    	i++;
  	}

  	if (i) 
  	{
    	int m = p_itoa(i, c);
    	c += m;
    	n += m;
  	} 
  	else if (!f) 
  	{
    	*(c = buf) = '0';
    	n = 1;
  	}

  	if (f) 
  	{
    	int j = prec;

    	*c++ = '.';

    	while (j--) 
    	{
      		c[j] = (f % 10) + '0';
      		f /= 10;
    	}
    	c += prec-1;
    	n += 1+prec;

    	while (*c == '0') 
    	{
      		c--;
      		n--;
    	}
  	}

  	*(++c) = 0;

  	return n;
}

static int
sprint_number (char *buf, double value)
{
  	int lenA = p_dtoa(value, 8, buf);
  	buf[lenA] = '\0'; /* xxx_sprint_xxx NULL terminates strings. */

  	return  lenA;
}

XWFontFilePK::XWFontFilePK(QIODevice * fileA,
		 	               int    fileFinalA,
		 	               ulong offsetA,
		 	               ulong lenA,
		 	               double pix2charuA)
	:XWFontFile(fileA, fileFinalA, offsetA, lenA),
	 pix2charu(pix2charuA),
	 stream(0),
	 streamLength(0),
	 curChar(-1),
	 nVals(-1),
	 inputLineSize(0),
	 inputLine(0),
	 imgLine(0),
	 imgLineSize(0),
	 imgIdx(-1)
{
}

XWFontFilePK::~XWFontFilePK()
{
	if (stream)
		free(stream);
				
	if (imgLine)
		free(imgLine);
	
	QHash<long, PKHeader*>::iterator i = charHeaders.begin();
	while (i != charHeaders.end())
	{
		PKHeader * pkh = i.value();
		if (pkh)
			delete pkh;
		i = charHeaders.erase(i);
	}
}

void XWFontFilePK::drawChar(QPainter * painter,
                            int c, 
                            int * x,
                            int * y, 
                            int squareSize)
{
	int tmpx = *x;
	int tmpy = *y;
	if (!charHeaders.contains(c))
		return ;
	
	if (stream)
		free(stream);
				
	if (imgLine)
		free(imgLine);
		
	stream = 0;
	streamLength = 0;
	curChar = c;
	inputLine = 0;
	imgLine = 0;
	
	PKHeader * pkh = charHeaders[c];
	if (pkh->bm_wd != 0 && pkh->bm_ht != 0 && pkh->pkt_len > 0)
	{
		uchar * pkt_ptr = (uchar*)malloc((pkh->pkt_len) * sizeof(uchar));
		file->seek(pkh->pos);
  		file->read((char*)pkt_ptr, pkh->pkt_len);
		if (pkh->dyn_f == 14)
  			decodeBitmap(pkh->bm_wd, pkh->bm_ht, pkt_ptr, pkh->pkt_len);
        else
        	decodePacked(pkh->bm_wd, pkh->bm_ht, pkh->dyn_f, pkh->run_color, pkt_ptr, pkh->pkt_len);
        	
        free(pkt_ptr);
        
        initImg(pkh->bm_wd);
        
        QImage img = toQImage(pkh->bm_wd, pkh->bm_ht, pkh->wd);
        tmpx += ((squareSize - img.width()) / 2);
        tmpy += ((squareSize - img.height()) / 2);
        painter->drawImage(tmpx, tmpy, img);
	}
}

void XWFontFilePK::drawChar(QPainter * painter,
                            int c, 
                            int x,
                            int y)
{
	if (!charHeaders.contains(c))
		return ;
		
	if (stream)
		free(stream);
		
	if (imgLine)
		free(imgLine);
		
	stream = 0;
	streamLength = 0;
	curChar = c;
	inputLine = 0;
	imgLine = 0;
	
	PKHeader * pkh = charHeaders[c];
	if (pkh->bm_wd != 0 && pkh->bm_ht != 0 && pkh->pkt_len > 0)
	{
		uchar * pkt_ptr = (uchar*)malloc((pkh->pkt_len) * sizeof(uchar));
		file->seek(pkh->pos);
  		file->read((char*)pkt_ptr, pkh->pkt_len);
		if (pkh->dyn_f == 14)
  			decodeBitmap(pkh->bm_wd, pkh->bm_ht, pkt_ptr, pkh->pkt_len);
        else
        	decodePacked(pkh->bm_wd, pkh->bm_ht, pkh->dyn_f, pkh->run_color, pkt_ptr, pkh->pkt_len);
        	
        free(pkt_ptr);
        initImg(pkh->bm_wd);
        QImage img = toQImage(pkh->bm_wd, pkh->bm_ht, pkh->wd);
        painter->drawImage(x, y, img);
	}
}

int XWFontFilePK::getNumGlyphs()
{
	return charHeaders.size();
}

const uchar * XWFontFilePK::getCharProcStream(long c, long *lenA)
{
	if (c == curChar)
	{
		if (lenA)
			*lenA = streamLength;
			
		return stream;
	}
		
	if (!charHeaders.contains(c))
	{
		if (lenA)
			*lenA = 0;
		return 0;
	}
	
	if (stream)
		free(stream);
		
	stream = 0;
	streamLength = 0;
	curChar = c;
	
	PKHeader * pkh = charHeaders[c];
	double charwidth = ROUND(1000.0 * pkh->wd / (((double) (1<<20))*pix2charu), 0.1);
	long llx = -pkh->bm_hoff;
  	long lly =  pkh->bm_voff - pkh->bm_ht;
  	long urx =  pkh->bm_wd - pkh->bm_hoff;
  	long ury =  pkh->bm_voff;
  	
  	char buf[1024];
  	int lnA = sprint_number(buf, charwidth);
  	lnA += sprintf(buf + lnA, " 0 %ld %ld %ld %ld d1\n", llx, lly, urx, ury);
  	addToStream((uchar*)buf, lnA);
  	if (pkh->bm_wd != 0 && pkh->bm_ht != 0 && pkh->pkt_len > 0)
  	{
  		uchar * pkt_ptr = (uchar*)malloc((pkh->pkt_len) * sizeof(uchar));
  		file->seek(pkh->pos);
  		file->read((char*)pkt_ptr, pkh->pkt_len);
  		lnA = sprintf(buf, "q\n%ld 0 0 %ld %ld %ld cm\n", pkh->bm_wd, pkh->bm_ht, llx, lly);
  		addToStream((uchar*)buf, lnA);
  		lnA = sprintf(buf, "BI\n/W %ld\n/H %ld\n/IM true\n/BPC 1\nID ", pkh->bm_wd, pkh->bm_ht);
  		addToStream((uchar*)buf, lnA);
  		if (pkh->dyn_f == 14)
  			decodeBitmap(pkh->bm_wd, pkh->bm_ht, pkt_ptr, pkh->pkt_len);
        else
        	decodePacked(pkh->bm_wd, pkh->bm_ht, pkh->dyn_f, pkh->run_color, pkt_ptr, pkh->pkt_len);
        	
        lnA = sprintf(buf, "\nEI\nQ");
        addToStream((uchar*)buf, lnA);
        free(pkt_ptr);
  	}
  	
  	if (lenA)
		*lenA = streamLength;
			
	return stream;
}

XWFontFilePK * XWFontFilePK::load(const char * name, 
	                              int base_dpi,
	                              double point_size)
{
	XWTFMFile tfm(name);
	if (tfm.getID() < 0)
		return 0;
		
	double design_size = tfm.getDesignSize();
	unsigned  dpi = base_dpi;
	if (design_size > 0.0)
    	dpi  = (unsigned) ROUND(base_dpi * point_size / design_size, 1.0);
    	
    XWFontSea sea(QString(), dpi, QString(), QString());
    QString fn(name);
    GlyphFileType file_info;
    QString fullname = sea.findGlyph(fn, dpi, XWFontSea::PK, &file_info);
    if (fullname.isEmpty())
    	fullname = sea.findFile(fn, XWFontSea::PK, false);
    	
    if (fullname.isEmpty())
    {
    	QString msg = QString(tr("fail to find pk font \"%1\" at %2dpi.\n")).arg(fn).arg(dpi);
    	xwApp->warning(msg);
    	return 0;
    }
    
    QFile * fileA = new QFile(fullname);
    if (!fileA->open(QIODevice::ReadOnly))
    {
    	delete fileA;
    	return 0;
    }
    
    double pix2charuA  = 72. * 1000. / ((double) base_dpi) / point_size;
    
    XWFontFilePK *ff = new XWFontFilePK(fileA, FONTFILE_CLOSE, 0, (ulong)(fileA->size()), pix2charuA);
    uchar opcode;
    while (fileA->getChar((char*)(&opcode)) && opcode != PK_POST)
    {
    	if (opcode < 240)
    	{
    		PKHeader * h = new PKHeader;
			if ((opcode & 4) == 0)
			{
				h->pkt_len = (opcode & 3) * 0x100U + getUnsignedByte(fileA);
    			h->chrcode = getUnsignedByte(fileA);
    			h->wd = getUnsignedTriple(fileA);
    			h->dx = getUnsignedByte(fileA) << 16;
    			h->dy = 0L;
    			h->bm_wd    = getUnsignedByte(fileA);
    			h->bm_ht    = getUnsignedByte(fileA);
    			h->bm_hoff  = getSignedByte(fileA);
    			h->bm_voff  = getSignedByte(fileA);
    			h->pkt_len -= 8;
			}
			else if ((opcode & 7) == 7)
			{
				h->pkt_len = getUnsignedQuad(fileA);
    			h->chrcode = getSignedQuad(fileA);
    			h->wd = getSignedQuad(fileA);
    			h->dx = getSignedQuad(fileA);
    			h->dy = getSignedQuad(fileA);
    			h->bm_wd    = getSignedQuad(fileA);
    			h->bm_ht    = getSignedQuad(fileA);
    			h->bm_hoff  = getSignedQuad(fileA);
    			h->bm_voff  = getSignedQuad(fileA);
    			h->pkt_len -= 28;
			}
			else
			{
				h->pkt_len = (opcode & 3) * 0x10000UL + getUnsignedPair(fileA);
    			h->chrcode = getUnsignedByte(fileA);
    			h->wd = getUnsignedTriple(fileA);
    			h->dx = getUnsignedPair(fileA) << 16;
    			h->dy = 0x0L;
    			h->bm_wd    = getUnsignedPair(fileA);
    			h->bm_ht    = getUnsignedPair(fileA);
    			h->bm_hoff  = getSignedPair(fileA);
    			h->bm_voff  = getSignedPair(fileA);
    			h->pkt_len -= 13;
			}
	
			h->dyn_f     = opcode / 16;
  			h->run_color = (opcode & 8) ? 1 : 0;
  			h->pos = fileA->pos();
  			ff->doSkip(h->pkt_len);
  			ff->charHeaders[h->chrcode] = h;
    	}
    	else
    	{
    		switch (opcode) 
			{
      			case PK_NO_OP: 
      				break;
      				
      			case PK_XXX1: 
      				ff->doSkip(getUnsignedByte(fileA));   
      				break;
      				
      			case PK_XXX2: 
      				ff->doSkip(getUnsignedPair(fileA));   
      				break;
      				
      			case PK_XXX3: 
      				ff->doSkip(getUnsignedTriple(fileA)); 
      				break;
      				
      			case PK_XXX4: 
      				ff->doSkip(getUnsignedQuad(fileA));   
      				break;
      				
      			case PK_YYY:  
      				ff->doSkip(4);  
      				break;
      				
      			case PK_PRE:  
      				ff->doPreamble(); 
      				break;
      		}
    	}
    }
    
    return ff;
}

XWFontFilePK * XWFontFilePK::load(const char * name, 
	                              int base_dpi,
	                              double point_size,
	                              char * usedchars,
	                              double *FontBBox,
	                              double * Widths,
	                              double *FontMatrix,
	                              int    * FirstChar,
	                              int    * LastChar)
{
	XWTFMFile tfm(name);
	if (tfm.getID() < 0)
		return 0;
		
	double design_size = tfm.getDesignSize();
	unsigned  dpi = base_dpi;
	if (design_size > 0.0)
    	dpi  = (unsigned) ROUND(base_dpi * point_size / design_size, 1.0);
    	
    XWFontSea sea(QString(), dpi, QString(), QString());
    QString fn(name);
    GlyphFileType file_info;
    QString fullname = sea.findGlyph(fn, dpi, XWFontSea::PK, &file_info);
    if (fullname.isEmpty())
    	fullname = sea.findFile(fn, XWFontSea::PK, false);
    if (fullname.isEmpty())
    {
    	QString msg = QString(tr("fail to find pk font \"%1\" at %2dpi.\n")).arg(fn).arg(dpi);
    	xwApp->warning(msg);
    	return 0;
    }
    
    QFile * fileA = new QFile(fullname);
    if (!fileA->open(QIODevice::ReadOnly))
    {
    	delete fileA;
    	return 0;
    }
    
    double pix2charuA  = 72. * 1000. / ((double) base_dpi) / point_size;
    
    XWFontFilePK *ff = new XWFontFilePK(fileA, FONTFILE_CLOSE, 0, (ulong)(fileA->size()), pix2charuA);
    FontBBox[0] = FontBBox[1] =  HUGE_VAL;
  	FontBBox[2] = FontBBox[3] =  -HUGE_VAL;
  	char charavail[256];
  	memset(charavail, 0, 256);
    uchar opcode;
	while (fileA->getChar((char*)(&opcode)) && opcode != PK_POST)
	{
		if (opcode < 240)
		{
			PKHeader * h = new PKHeader;
			if ((opcode & 4) == 0)
			{
				h->pkt_len = (opcode & 3) * 0x100U + getUnsignedByte(fileA);
    			h->chrcode = getUnsignedByte(fileA);
    			h->wd = getUnsignedTriple(fileA);
    			h->dx = getUnsignedByte(fileA) << 16;
    			h->dy = 0L;
    			h->bm_wd    = getUnsignedByte(fileA);
    			h->bm_ht    = getUnsignedByte(fileA);
    			h->bm_hoff  = getSignedByte(fileA);
    			h->bm_voff  = getSignedByte(fileA);
    			h->pkt_len -= 8;
			}
			else if ((opcode & 7) == 7)
			{
				h->pkt_len = getUnsignedQuad(fileA);
    			h->chrcode = getSignedQuad(fileA);
    			h->wd = getSignedQuad(fileA);
    			h->dx = getSignedQuad(fileA);
    			h->dy = getSignedQuad(fileA);
    			h->bm_wd    = getSignedQuad(fileA);
    			h->bm_ht    = getSignedQuad(fileA);
    			h->bm_hoff  = getSignedQuad(fileA);
    			h->bm_voff  = getSignedQuad(fileA);
    			h->pkt_len -= 28;
			}
			else
			{
				h->pkt_len = (opcode & 3) * 0x10000UL + getUnsignedPair(fileA);
    			h->chrcode = getUnsignedByte(fileA);
    			h->wd = getUnsignedTriple(fileA);
    			h->dx = getUnsignedPair(fileA) << 16;
    			h->dy = 0x0L;
    			h->bm_wd    = getUnsignedPair(fileA);
    			h->bm_ht    = getUnsignedPair(fileA);
    			h->bm_hoff  = getSignedPair(fileA);
    			h->bm_voff  = getSignedPair(fileA);
    			h->pkt_len -= 13;
			}
	
			h->dyn_f     = opcode / 16;
  			h->run_color = (opcode & 8) ? 1 : 0;
  			h->pos = fileA->pos();
  			ff->doSkip(h->pkt_len);
  			if (!usedchars[h->chrcode & 0xff])
  			{
  				Widths[h->chrcode & 0xff] = 0;
  				ff->doSkip(h->pkt_len);
    			delete h;
  			}
  			else
  			{
  				ff->charHeaders[h->chrcode] = h;  	
  				double charwidth = ROUND(1000.0 * h->wd / (((double) (1<<20))*pix2charuA), 0.1);
    			Widths[h->chrcode & 0xff] = charwidth;
    			FontBBox[0] = qMin((double)FontBBox[0], (double)(-(h->bm_hoff)));
        		FontBBox[1] = qMin((double)FontBBox[1], (double)(h->bm_voff - h->bm_ht));
        		FontBBox[2] = qMax((double)FontBBox[2], (double)(h->bm_wd - h->bm_hoff));
        		FontBBox[3] = qMax((double)FontBBox[3], (double)(h->bm_voff));
    		}
    		
    		charavail[h->chrcode & 0xff] = 1;
		}
		else
		{
			switch (opcode) 
			{
      			case PK_NO_OP: 
      				break;
      				
      			case PK_XXX1: 
      				ff->doSkip(getUnsignedByte(fileA));   
      				break;
      				
      			case PK_XXX2: 
      				ff->doSkip(getUnsignedPair(fileA));   
      				break;
      				
      			case PK_XXX3: 
      				ff->doSkip(getUnsignedTriple(fileA)); 
      				break;
      				
      			case PK_XXX4: 
      				ff->doSkip(getUnsignedQuad(fileA));   
      				break;
      				
      			case PK_YYY:  
      				ff->doSkip(4);  
      				break;
      				
      			case PK_PRE:  
      				ff->doPreamble(); 
      				break;
      		}
		}
	}
	
	*FirstChar = 255; 
	*LastChar = 0;
	
	for (int code = 0; code < 256; code++)
	{
		if (usedchars[code])
		{
			if (code < *FirstChar) 
				*FirstChar = code;
				
			if (code > *LastChar)  
				*LastChar  = code;
		}
	}
	
	if (*FirstChar > *LastChar)
	{
		delete ff;
		return 0;
	}
	
	FontMatrix[0] = 0.001 * pix2charuA;
	FontMatrix[1] = 0.0;
	FontMatrix[2] = 0.0;
	FontMatrix[3] = 0.001 * pix2charuA;
	FontMatrix[4] = 0.0;
	FontMatrix[5] = 0.0;
	
    return ff;
}

bool XWFontFilePK::open(const char * name, 
	                    int base_dpi,
	                    double point_size)
{
	XWTFMFile tfm(name);
	if (tfm.getID() < 0)
		return false;
		
	double design_size = tfm.getDesignSize();
	unsigned  dpi = base_dpi;
	if (design_size > 0.0)
    	dpi  = (unsigned) ROUND(base_dpi * point_size / design_size, 1.0);
    	
    XWFontSea sea(QString(), dpi, QString(), QString());
    QString fn(name);
    GlyphFileType file_info;
    QString fullname = sea.findGlyph(fn, dpi, XWFontSea::PK, &file_info);
    if (fullname.isEmpty())
    	fullname = sea.findFile(fn, XWFontSea::PK, false);
    if (fullname.isEmpty())
    	return false;
    	
    QFile fileA(fullname);
    if (!fileA.open(QIODevice::ReadOnly))
    	return false;
    	
    return true;
}

void XWFontFilePK::addToStream(uchar *rowptr, long rowbytes)
{
	stream = (uchar*)realloc(stream, (streamLength + rowbytes) * sizeof(uchar));
	memcpy(stream + streamLength, rowptr, rowbytes);
	streamLength += rowbytes;
}

int XWFontFilePK::decodeBitmap(long wd, 
	                           long ht,
                               uchar *dp, 
                               long pl)
{
	const static unsigned char mask[8] = {
    	0x80u, 0x40u, 0x20u, 0x10u, 0x08u, 0x04u, 0x02u, 0x01u
  	};
  	
  	if (pl < (wd * ht + 7) / 8)
  		return -1;
  		
  	long rowbytes = (wd + 7) / 8;
  	uchar * rowptr = (uchar*)malloc(rowbytes * sizeof(uchar));
  	memset(rowptr, 0, rowbytes);
  	int j = 0;
  	for (int i = 0; i < ht * wd; i++)
  	{
  		uchar c = dp[i / 8] & mask[i % 8];
  		if (c == 0)
      		rowptr[j / 8] |= mask[i % 8];
      		
      	j++;
    	if (j == wd)
    	{
    		addToStream(rowptr, rowbytes);
    		memset(rowptr, 0, rowbytes);
      		j = 0;
    	}
  	}
  	
  	if (rowptr)
  		free(rowptr);
  		
  	return  0;
}

int XWFontFilePK::decodePacked(long wd, 
	                           long ht, 
	                           int dyn_f, 
	                           int run_color, 
	                           uchar *dp, 
	                           long pl)
{
	long rowbytes = (wd + 7) / 8;
  	uchar * rowptr = (uchar*)malloc(rowbytes * sizeof(uchar));
  	long i = 0, np = 0;
  	long run_count = 0, repeat_count = 0;
  	long nbits = 0;
  	for (np = 0, i = 0; i < ht; i++)
  	{
  		repeat_count = 0;
    	memset(rowptr, 0xff, rowbytes);
    	long rowbits_left = wd;
    	if (run_count > 0)
    	{
    		nbits = qMin(rowbits_left, run_count);
      		switch (run_color) 
      		{
      			case  0:
        			rowbits_left -= fillBlackRun(rowptr, 0, nbits);
        			break;
        			
      			case  1:
        			rowbits_left -= fillWhiteRun(nbits);
        			break;
      		}
      		run_count -= nbits;
    	}
    	
    	while (np / 2 < pl && rowbits_left > 0)
    	{
    		int nyb = (np % 2) ? dp[np/2] & 0x0f : (dp[np/2] >> 4) & 0x0f;
    		if (nyb == 14)
    		{
    			np++;
    			repeat_count = packedNum(&np, dyn_f, dp, pl);
    		}
    		else if (nyb == 15)
    		{
    			np++;
    			repeat_count = 1;
    		}
    		else
    		{
    			run_count = packedNum(&np, dyn_f, dp, pl);
    			nbits = qMin(rowbits_left, run_count);
        		run_color  = !run_color;
        		run_count -= nbits;
        		switch (run_color) 
        		{
        			case  0:
          				rowbits_left -= fillBlackRun(rowptr, wd - rowbits_left, nbits);
          				break;
          				
        			case  1:
          				rowbits_left -= fillWhiteRun(nbits);
          				break;
        		}
    		}
    	}
    	addToStream(rowptr, rowbytes);
    	
    	for ( ; i < ht && repeat_count > 0; repeat_count--, i++)
    		addToStream(rowptr, rowbytes);
  	}
  	
  	if (rowptr)
  		free(rowptr);
  		
  	return 0;
}

void XWFontFilePK::doPreamble()
{
	char c;
	if (file->getChar(&c) && c == 89)
	{
		doSkip(getUnsignedByte(file));
		doSkip(16);
	}
}

void XWFontFilePK::doSkip(unsigned long length)
{
	char c;
	while (length-- > 0)
    	file->getChar(&c);
}

long XWFontFilePK::fillBlackRun(uchar *dp, long left, long run_count)
{
	const static unsigned char mask[8] = {
    	127u, 191u, 223u, 239u, 247u, 251u, 253u, 254u
  	};
  	long  right = left + run_count - 1;
  	for ( ; left <= right; left++) 
    	dp[left / 8] &= mask[left % 8];
    	
  	return  run_count;
}

uchar * XWFontFilePK::getImgLine()
{
	inputLine += inputLineSize;
	if (inputLine > (stream + streamLength))
		return 0;
		
	uchar * p = inputLine;
	for (int i = 0; i < nVals; i += 8)
	{
		int c = *p++;
    imgLine[i+0] = (uchar)((c >> 7) & 1);
    imgLine[i+1] = (uchar)((c >> 6) & 1);
    imgLine[i+2] = (uchar)((c >> 5) & 1);
    imgLine[i+3] = (uchar)((c >> 4) & 1);
    imgLine[i+4] = (uchar)((c >> 3) & 1);
    imgLine[i+5] = (uchar)((c >> 2) & 1);
    imgLine[i+6] = (uchar)((c >> 1) & 1);
    imgLine[i+7] = (uchar)(c & 1);
	}
	
	return imgLine;
}

bool XWFontFilePK::getPixel(uchar *pix)
{
	if (imgIdx >= nVals) 
	{
    if (!getImgLine()) 
    {
      return false;
    }
    imgIdx = 0;
  }
  
  pix[0] = imgLine[imgIdx++];
  return true;
}

void XWFontFilePK::initImg(long widthA)
{
	nVals = widthA;
	inputLineSize = (nVals + 7) >> 3;
	inputLine = stream;
	imgLineSize = (nVals + 7) & ~7;
	imgLine = (uchar *)malloc(imgLineSize * sizeof(uchar));
	imgIdx = nVals;
}

long XWFontFilePK::packedNum(long *np, int dyn_f, uchar *dp, long pl)
{
	long  nmbr = 0, i = *np;
	
#define get_nyb() ((i % 2) ? dp[i/2] & 0x0f : (dp[i/2] >> 4) & 0x0f)
	
	if (i / 2 == pl)
		return 0;
		
	int nyb = get_nyb(); 
	i++;
	if (nyb == 0)
	{
		int j = 0;
    	do 
    	{
      		if (i / 2 == pl) 
        		break;
        		
      		nyb = get_nyb(); 
      		i++;
      		j++;
    	} while (nyb == 0);
    	
    	nmbr = nyb;
    	
    	while (j-- > 0) 
    	{
      		if (i / 2 == pl) 
        		break;
        		
      		nyb  = get_nyb(); i++;
      		nmbr = nmbr * 16 + nyb;
    	}
    	nmbr += (13 - dyn_f) * 16 + dyn_f - 15;
	}
	else if (nyb <= dyn_f)
		nmbr = nyb;
	else if (nyb < 14)
	{
		if (i / 2 == pl) 
      		return  0;
      		
    	nmbr = (nyb - dyn_f - 1) * 16 + get_nyb() + dyn_f + 1;
    	i++;
	}
	
	*np = i;
  	return  nmbr;
}

QImage XWFontFilePK::toQImage(long widthA, long heightA, long wd)
{
	int charwidth = qRound(ROUND(1000.0 * wd / (((double) (1<<20))*pix2charu), 0.1));
	QImage img(widthA, heightA, QImage::Format_RGB32);
	for (long y = 0; y < heightA; y++)
	{
		uchar * p = getImgLine();
		if (!p)
			break;
			
		for (long x = 0; x < widthA; x++)
		{
			uchar c = *p++;
			if (c & 1)
				img.setPixel(x, y, qRgb(255, 255, 255));
			else
				img.setPixel(x, y, qRgb(0, 0, 0));
		}
	}
	
	img = img.scaledToWidth(charwidth, Qt::SmoothTransformation);
	return img;
}

