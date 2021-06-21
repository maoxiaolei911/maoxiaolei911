/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <QPair>
#include <QList>
#include <QtEndian>
#include "XWNumberUtil.h"
#include "XWUtil.h"
#include "XWFontSea.h"
#include "XWTexFontMap.h"
#include "XWApplication.h"
#include "NameToUnicodeTable.h"
#include "NameToMacTable.h"
#include "XWSubfont.h"
#include "LigKern.h"
#include "CaseTable.h"
#include "TexEncoding.h"
#include "XWFontFileFT.h"


#undef PI
#define PI 3.14159265358979323846264338327


#ifdef  ROUND
#undef  ROUND
#endif

#define ROUND(n,acc) (floor(((double)n)/(acc)+0.5)*(acc)) 

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

static FT_Library lib;
static int fileCount = 0;

static char default_codingscheme[] = "Unspecified";

XWFontFileFT::XWFontFileFT(FT_Face faceA)
	:face(faceA)
{
	fileCount++;
	pid = 3;
	eid = 1;
	inencptrs = 0;
	charList = 0;
	codingScheme = 0;
	currentEncodingScheme = encFontSpecific;
	for (int i = 0; i < 256; i++)
        sfCode[i] = -1;
        
    unitsPerEm = face->units_per_EM;
    if (unitsPerEm == 0)
    	unitsPerEm = 1;
    unitsPerEm = 2 * unitsPerEm + unitsPerEm / 2;
    italicAngle = 0.0;
    fixedPitch = isFixedWidth();
    fontDir = 0;
    capHeight = 0.8;
    xheight = 0;
    fontSpace = 0;
    size = 10.0;
    efactor = 1.0;
    slant = 0.0;
    boldf = 0.0;
    bold = false;
    rotate = false;
    y_offset = 0.25;
    
    smallcaps = false;
    
    onlyRange = false;
    subfontLigs = false;
    ligName = 0;
    ligSubID = 0;
    
    inencoding = 0;
    outencoding = 0;
    outencptrs = 0;
    nextout = 0;
    uppercase = 0;
    lowercase = 0;

    replacements = 0;
    replacementName = 0;
    boundaryChar = -1;

    sawLigKern = false;
    bc = 0;
    ec = 0;
    fontLevel = -1;
    
    stream = 0;
    streamLength = 0;
    curChar = -1;
    
    if (isSFNT())
    {
        TT_Header * tthead = (TT_Header * )FT_Get_Sfnt_Table(face, ft_sfnt_head);
        if (tthead)
        {
            if (tthead->Font_Direction == -1)
                fontDir = 2;
        }

        TT_Postscript * ttpost = (TT_Postscript * )FT_Get_Sfnt_Table(face, ft_sfnt_post);
        if (ttpost)
            italicAngle = ttpost->italicAngle / 65536.0;

        TT_OS2 *os2 = (TT_OS2 *)FT_Get_Sfnt_Table(face, ft_sfnt_os2);
        if (os2)
            xheight = (os2->sxHeight * 1000) / unitsPerEm;
    }
    
    for (int i = 0; nameToUnicodeTab[i].name; i++)
    {
        QString name(nameToUnicodeTab[i].name);
        uniCodeToNameHash[nameToUnicodeTab[i].u] = name;
    }

    for (int i = 0; nameToMacTab[i].name; i++)
    {
        QString name(nameToMacTab[i].name);
        macCodeToNameHash[nameToMacTab[i].u] = name;
    }
}

XWFontFileFT::~XWFontFileFT()
{
	if (inencptrs)
        free(inencptrs);
        
    if (charList)
    {
        FTFontInfo * cur = charList;
        while (cur)
        {
            FTFontInfo * tmp = cur->next;
            releaseChar(cur);
            cur = tmp;
        }
    }
    
    if (codingScheme)
        delete [] codingScheme;

    if (ligName)
        delete [] ligName;
        
    if (ligSubID)
    	delete [] ligSubID;
        
    freeEncoding(inencoding);
    freeEncoding(outencoding);
    if (outencptrs)
        free(outencptrs);
        
    if (nextout)
        free(nextout);

    if (uppercase)
        free(uppercase);

    if (lowercase)
        free(lowercase);

    if (replacements)
    {
        StringList * cur = replacements;
        while (cur)
        {
            StringList * tmp = cur->next;
            delete cur;
            cur = tmp;
        }
    }

    if (replacementName)
        free(replacementName);
        
    if (stream)
    	free(stream);
    
	if (face) 
    	FT_Done_Face(face);
    	
	fileCount--;
    if (fileCount <= 0)
    	FT_Done_FreeType(lib);
}

void XWFontFileFT::assignChars()
{
	char ** p = outencoding->vec;
    for (int i = 0; i <= 0xFF; i++, p++)
    {
        FTFontInfo *ti = findMappedAdobe(*p, inencptrs);
        if (ti)
        {
            if (ti->outcode >= 0)
                nextout[i] = ti->outcode;
            ti->outcode = i;
            outencptrs[i] = ti;
        }
    }

    for (int i = 0; i <= 0xFF; i++)
    {
        FTFontInfo *ti = inencptrs[i];
        if (ti && ti->charcode >= 0 &&
            ti->charcode <= 0xFF &&
            ti->outcode < 0 &&
            outencptrs[ti->charcode] == 0)
        {
            ti->outcode = ti->charcode;
            outencptrs[ti->charcode] = ti;
        }
    }

    int nextfree = 0x80;
    for (int i = 0; i <= 0xFF; i++)
    {
        FTFontInfo *ti = inencptrs[i];
        if (ti && ti->outcode < 0)
        {
             while (outencptrs[nextfree])
            {
                nextfree = (nextfree + 1) & 0xFF;
                if (nextfree == 0x80)
                    goto finishup;        /* all slots full */
            }
            ti->outcode = nextfree;
            outencptrs[nextfree] = ti;
        }
    }

finishup:
    for (int i = 0; i <= 0xFF; i++)
    {
        FTFontInfo *ti = inencptrs[i];
        if (ti && ti->outcode >= 0)
        {
            int k = -1;
            while (nextout[ti->outcode] >= 0)
            {
                int j = nextout[ti->outcode];
                nextout[ti->outcode] = k;
                k = ti->outcode;
                ti->outcode = j;
            }
            nextout[ti->outcode] = k;
        }
    }
}

void XWFontFileFT::codeToAdobeName(long code, char * buf)
{
	if (currentEncodingScheme == encFontSpecific)
    {
        sprintf(buf, ".%c0x%lx", (code >= 0x1000000) ? 'g' : 'c', (code & 0xFFFFFF));
        return ;
    }

    if (currentEncodingScheme == encUnicode && uniCodeToNameHash.contains((int)code))
    {
        QString name = uniCodeToNameHash[code];
        QByteArray ba = name.toAscii();
        strncpy(buf, ba.constData(), ba.size());
        return ;
    }
    else if (currentEncodingScheme == encMac && macCodeToNameHash.contains((int)code))
    {
        QString name = macCodeToNameHash[code];
        QByteArray ba = name.toAscii();
        strncpy(buf, ba.constData(), ba.size());
        return ;
    }

    sprintf(buf, ".%c0x%lx", (code >= 0x1000000) ? 'g' : 'c', (code & 0xFFFFFF));
}

void XWFontFileFT::doType3(int base_dpi,
                           int vDPI,
                           double point_size,
                           char * usedchars,
                           double *FontBBox,
                           double * Widths,
                           double *FontMatrix,
                           int    * FirstChar,
                           int    * LastChar)
{
	FontBBox[0] = FontBBox[1] =  HUGE_VAL;
  	FontBBox[2] = FontBBox[3] =  -HUGE_VAL;
    FT_GlyphSlot slot = 0;
    FT_BBox  bbox;
    FT_UInt   glyph_index;
    FT_ULong charcode = FT_Get_First_Char(face, &glyph_index);
    double pix2charu  = 72. * 1000. / ((double) base_dpi) / point_size;
    FT_Glyph aglyph;
    while ( glyph_index != 0 )
    {
    	if (!usedchars[charcode & 0xff])
    	{
    		charcode = FT_Get_Next_Char(face, charcode, &glyph_index );
    		continue;
    	}
    		
    	if (FT_IS_SCALABLE(face))
    	{
        	FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
        	slot = face->glyph;

        	if (efactor != 1.0 || slant != 0.0 )
            	FT_Outline_Transform(&(slot->outline), &matrix1);

        	if (rotate)
            	FT_Outline_Transform(&(slot->outline), &matrix2);

        	if (bold)
            	FT_Outline_Embolden(&(slot->outline), (int)(boldf * 64));
            
        	FT_Render_Glyph(slot, ft_render_mode_normal ); 

        	FT_Outline_Get_BBox(&(slot->outline), &bbox);
    	}
    	else
    	{
        	FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
        	slot = face->glyph;
        	FT_GlyphSlot_Own_Bitmap(slot);
        	if (bold)
            	FT_Bitmap_Embolden(lib, &(slot->bitmap), (int)(boldf * 64), (int)(boldf * 64));
        	
        	FT_Get_Glyph(slot, &aglyph);
        	FT_Glyph_Get_CBox(aglyph, FT_GLYPH_BBOX_GRIDFIT, &bbox);
    	}
    
    	long wd = 0;
    	if (rotate)
    		wd = slot->metrics.vertAdvance * 1000 / unitsPerEm;
    	else
        	wd = transform(slot->metrics.horiAdvance * 1000 / unitsPerEm, 0, efactor, slant);        
    	wd = scale(wd);
    	double charwidth = ROUND(1000.0 * wd / (((double) (1<<20))*pix2charu), 0.1);
    	long bm_hoff = 5 - bbox.xMin / 64;
    	long bm_voff = 5 - bbox.yMin / 64;
    	long bm_wd = (bbox.xMax + 63) / 64 - bbox.xMin / 64 + 10;
    	long bm_ht = (bbox.yMax + 63) / 64 - bbox.yMin / 64 + 10;
    	bm_voff = bm_ht - bm_voff - 2;
    	Widths[charcode & 0xff] = charwidth;
    	FontBBox[0] = qMin(FontBBox[0], (double)(-bm_hoff));
        FontBBox[1] = qMin(FontBBox[1], (double)(bm_voff - bm_ht));
        FontBBox[2] = qMax(FontBBox[2], (double)(bm_wd - bm_hoff));
        FontBBox[3] = qMax(FontBBox[3], (double)bm_voff);
        
        if (!FT_IS_SCALABLE(face))
  			FT_Done_Glyph(aglyph);
  			
  		charcode = FT_Get_Next_Char(face, charcode, &glyph_index );
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
		else
			Widths[code] = 0;
	}
	
	FontMatrix[0] = 0.001 * pix2charu;
	FontMatrix[1] = 0.0;
	FontMatrix[2] = 0.0;
	FontMatrix[3] = 0.001 * pix2charu;
	FontMatrix[4] = 0.0;
	FontMatrix[5] = 0.0;
}

FTFontInfo * XWFontFileFT::findAdobe(char *p)
{
	if (!p)
        return 0;

    long l = -1;
    char c = '\0', d = '\0';
    if (p[0] == '.' &&
        (c = p[1]) && (c == 'c' || c == 'g') &&
        (d = p[2]) && '0' <= d && d <= '9')
        l = strtol(p + 2, NULL, 0);

    FTFontInfo * ti = charList;
    for (; ti; ti = ti->next)
    {
        if (l >= 0)
        {
            if (c == 'c')
            {
                if (ti->charcode == l)
                    return ti;
            }
            else
            {
                if (ti->glyphindex == l)
                    return ti;
            }
        }
        else if (strcmp(p, ti->adobename) == 0)
            return ti;
    }

    return 0;
}

FTFontInfo * XWFontFileFT::findGlyph(ushort g)
{
	FTFontInfo * ti = charList;
    while (ti)
    {
        if (g == ti->glyphindex)
            return ti;

        ti = ti->next;
    }

    return 0;
}

FTFontInfo * XWFontFileFT::findMappedAdobe(char *p, 
	                                       FTFontInfo**array)
{
	if (!p)
        return 0;

    long l = -1;
    char c = '\0', d = '\0';
    if (p[0] == '.' &&
        (c = p[1]) && (c == 'c' || c == 'g') &&
        (d = p[2]) && '0' <= d && d <= '9')
        l = strtol(p + 2, NULL, 0);

    FTFontInfo * ti = 0;
    for (int i = 0; i < 256; i++)
    {
        if ((ti = array[i]))
        {
            if (l >= 0)
            {
                if (c == 'c')
                {
                    if (ti->charcode == l)
                    return ti;
                }
                else
                {
                    if (ti->glyphindex == l)
                        return ti;
                }
            }
            else if (strcmp(p, ti->adobename) == 0)
                return ti;
        }
    }

    return 0;
}

const uchar * XWFontFileFT::getCharProcStream(long c, 
	                                          int base_dpi, 
	                                          long *lenA)
{
	if (c == curChar)
	{
		if (lenA)
			*lenA = streamLength;
			
		return stream;
	}
	
	if (stream)
		free(stream);
		
	stream = 0;
	streamLength = 0;
	curChar = c;

    FT_GlyphSlot slot = 0;
    FT_BBox  bbox;
    
    FT_UInt glyph_index = FT_Get_Char_Index(face, (FT_ULong)curChar);
    if (glyph_index == 0)
    {
    	if (lenA)
			*lenA = 0;
		return 0;
    }
    
    FT_Glyph aglyph;
	if (FT_IS_SCALABLE(face))
    {
        FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
        slot = face->glyph;

        if (efactor != 1.0 || slant != 0.0 )
            FT_Outline_Transform(&(slot->outline), &matrix1);

        if (rotate)
            FT_Outline_Transform(&(slot->outline), &matrix2);

        if (bold)
            FT_Outline_Embolden(&(slot->outline), (int)(boldf * 64));
            
        FT_Render_Glyph(slot, ft_render_mode_normal ); 

        FT_Outline_Get_BBox(&(slot->outline), &bbox);
    }
    else
    {
        FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
        slot = face->glyph;        
        FT_GlyphSlot_Own_Bitmap(slot);
        if (bold)
            FT_Bitmap_Embolden(lib, &(slot->bitmap), (int)(boldf * 64), (int)(boldf * 64));
        
        FT_Get_Glyph(slot, &aglyph);
        FT_Glyph_Get_CBox(aglyph, FT_GLYPH_BBOX_GRIDFIT, &bbox);
    }
    
    long wd = 0;
    if (rotate)
    	wd = slot->metrics.vertAdvance * 1000 / unitsPerEm;
    else
        wd = transform(slot->metrics.horiAdvance * 1000 / unitsPerEm, 0, efactor, slant);        
    wd = scale(wd);
    double pix2charu  = 72. * 1000. / ((double) base_dpi) / size;
    double charwidth = ROUND(1000.0 * wd / (((double) (1<<20))*pix2charu), 0.1);
    long bm_hoff = 5 - bbox.xMin / 64;
    long bm_voff = 5 - bbox.yMin / 64;
    long bm_wd = (bbox.xMax + 63) / 64 - bbox.xMin / 64 + 10;
    long bm_ht = (bbox.yMax + 63) / 64 - bbox.yMin / 64 + 10;
    bm_voff = bm_ht - bm_voff - 2;
    long llx = -bm_hoff;
  	long lly =  bm_voff - bm_ht;
  	long urx =  bm_wd - bm_hoff;
  	long ury =  bm_voff;
  	char buf[1024];
  	int lnA = sprint_number(buf, charwidth);
  	lnA += sprintf(buf + lnA, " 0 %ld %ld %ld %ld d1\n", llx, lly, urx, ury);
  	addToStream((uchar*)buf, lnA);
  	if (bm_wd != 0 && bm_ht != 0)
  	{
  		lnA = sprintf(buf, "q\n%ld 0 0 %ld %ld %ld cm\n", bm_wd, bm_ht, llx, lly);
  		addToStream((uchar*)buf, lnA);
  		
  		switch (slot->bitmap.pixel_mode)
  		{
  			case FT_PIXEL_MODE_GRAY:
  				lnA = sprintf(buf, "BI\n/W %ld\n/H %ld\n/CS /Gray\n/BPC 8\nID ", bm_wd, bm_ht);
  				break;
  				
  			case FT_PIXEL_MODE_GRAY2:
  				lnA = sprintf(buf, "BI\n/W %ld\n/H %ld\n//CS /Gray\n/BPC 2\nID ", bm_wd, bm_ht);
  				break;
  				
  			case FT_PIXEL_MODE_GRAY4:
  				lnA = sprintf(buf, "BI\n/W %ld\n/H %ld\n//CS /Gray\n/BPC 4\nID ", bm_wd, bm_ht);
  				break;
  				
  			case FT_PIXEL_MODE_LCD:
  			case FT_PIXEL_MODE_LCD_V:
  				lnA = sprintf(buf, "BI\n/W %ld\n/H %ld\n/CS /RGB\n/BPC 8\nID ", bm_wd, bm_ht);
  				break;
  				
  			default:
  				lnA = sprintf(buf, "BI\n/W %ld\n/H %ld\n/IM true\n/BPC 1\nID ", bm_wd, bm_ht);
  				break;
  		}
  		
  		addToStream((uchar*)buf, lnA);
  		lnA = slot->bitmap.width * slot->bitmap.rows;
  		addToStream((uchar*)(slot->bitmap.buffer), lnA);
  		lnA = sprintf(buf, "\nEI\nQ");
        addToStream((uchar*)buf, lnA);
  	}
  	
  	if (!FT_IS_SCALABLE(face))
  		FT_Done_Glyph(aglyph);
  		
  	if (lenA)
		*lenA = streamLength;
			
	return stream;
}

char * XWFontFileFT::getCodingScheme()
{
	if (!codingScheme)
    {
    	FT_CharMap cm = face->charmap;
        switch (cm->encoding)
        {
            case FT_ENCODING_UNICODE:
                codingScheme = qstrdup("Unicode");
                break;

            case FT_ENCODING_MS_SYMBOL:
                codingScheme = qstrdup("MS Symbol");
                break;

            case FT_ENCODING_SJIS:
                codingScheme = qstrdup("SJIS");
                break;

            case FT_ENCODING_GB2312:
                codingScheme = qstrdup("GB2312");
                break;

            case FT_ENCODING_BIG5:
                codingScheme = qstrdup("Big5");
                break;

            case FT_ENCODING_WANSUNG:
                codingScheme = qstrdup("WANSUNG");
                break;

            case FT_ENCODING_JOHAB:
                codingScheme = qstrdup("JOHAB");
                break;

            case FT_ENCODING_ADOBE_LATIN_1:
                codingScheme = qstrdup("Adobe Latin1");
                break;

            case FT_ENCODING_ADOBE_STANDARD:
                codingScheme = qstrdup("Adobe Standard");
                break;

            case FT_ENCODING_ADOBE_EXPERT:
                codingScheme = qstrdup("Adobe Expert");
                break;

            case FT_ENCODING_ADOBE_CUSTOM:
                codingScheme = qstrdup("Adobe Custom");
                break;
    
            case FT_ENCODING_APPLE_ROMAN:
                codingScheme = qstrdup("Apple Roman");
                break;

            default:
                codingScheme = qstrdup(default_codingscheme);
                break;
        }
    }
    
    return codingScheme;
}

long XWFontFileFT::getDesignSizeFixed()
{
	return toFixWord(size);
}

char * XWFontFileFT::getFamilyName()
{
	return (char*)(face->family_name);
}

void XWFontFileFT::getTFMParam(long * tparam, bool vf)
{
	double Slant = slant - efactor * tan(italicAngle * (PI / 180.0));	
	tparam[0] = (long)(0x100000L * Slant + 0.5);
	tparam[1] = scale((long)fontSpace);
	if (vf)
	{
        tparam[2] = (fixedPitch ? 0 : scale((long)transform(200, 0, efactor, slant)));
        tparam[3] = (fixedPitch ? 0 : scale((long)transform(100, 0, efactor, slant)));
        tparam[4] = scale((long)xheight);
        tparam[5] = scale((long)(transform(1000, 0, efactor, slant)));
        long exspace = fixedPitch ? fontSpace : (long)transform(111, 0, efactor, slant);
        tparam[6] = scale(exspace);
	}
	else
	{
        tparam[2] = (fixedPitch ? 0 : scale((long)(300 * efactor + 0.5)));
        tparam[3] = (fixedPitch ? 0 : scale((long)(100 * efactor + 0.5)));
        tparam[4] = scale((long)xheight);
        tparam[5] = scale((long)(1000 * efactor + 0.5));
	}
}

void XWFontFileFT::handleReencoding()
{
	if (inEncName.endsWith(".enc", Qt::CaseInsensitive))
    {
        inencoding = readEncoding(inEncName, true);
        if (!inencoding)
            return ;

        for (int i = 0; i <= 0xFF; i++)
        {
            FTFontInfo * ti = inencptrs[i];
            if (ti)
            {
                ti->incode = -1;
                inencptrs[i] = 0;
            }
        }

        for (int i = 0; i <= 0xFF; i++)
        {
            char * p = inencoding->vec[i];
            if (p && *p)
            {
                FTFontInfo * ti = findAdobe(p);
                if (ti->incode >= 0)
                {
                	delete [] p;
                    inencoding->vec[i] = qstrdup(".notdef");
                    continue;
                }

                if (ti->charcode >= 0)
                {
                    ti->incode = i;
                    inencptrs[i] = ti;
                }
            }

            if (!codingScheme)
                codingScheme = qstrdup(inencoding->name);
        }
    }

    if (outEncName.endsWith(".enc", Qt::CaseInsensitive))
        outencoding = readEncoding(outEncName, false);
    else
        outencoding = readEncoding(QString(), false);
}

XWFontFileFT * XWFontFileFT::loadTexFont(const char * texnameA)
{
	if (fileCount == 0 && FT_Init_FreeType(&lib))
	{
		xwApp->error(tr("fail to init freetype lib.\n"));
		return 0;
	}
	
	XWTexFontMap mrec;
	mrec.readSetting(texnameA);
    if (!(mrec.font_name))
    {
        QString msg = QString(tr("the tex font '%1' has not been configured.\n")).arg(texnameA);
        xwApp->error(msg);
        return 0;
    }
    
    XWFontSea fontsea;
    QString ftname(mrec.font_name);
    QString fullname = fontsea.findFile(ftname);
    if (fullname.isEmpty())
    {
        QString msg = QString(tr("cannot find font file '%1'(%2).\n")).arg(ftname).arg(texnameA);
        xwApp->error(msg);
        return 0;
    }
    
    int fontindex = 0;
    if (mrec.opt)
    	fontindex = mrec.opt->index;
    QByteArray ba = QFile::encodeName(fullname);
    
    FT_Face faceA;
    if (FT_New_Face(lib, ba.constData(), fontindex, &faceA))
    {
        QString msg = QString(tr("fail to load font file '%1'(%2).\n")).arg(fullname).arg(texnameA);
        xwApp->error(msg);
        return 0;
    }
    
    XWFontFileFT * ff = new XWFontFileFT(faceA);
    if (mrec.charmap.sfd_name && mrec.charmap.subfont_id)
    {
    	QString subfont(mrec.charmap.sfd_name);
    	QString subfontid(mrec.charmap.subfont_id);
    	ff->setSubFont(subfont, subfontid);
    }
    
    ff->setCharMap(mrec.opt->pid, mrec.opt->eid);
    
    ff->setBoldf(mrec.opt->bold);    	
    ff->setBold(mrec.opt->boldb);
    
    ff->setEFactor(mrec.opt->extend);
    ff->setSlant(mrec.opt->slant);
    
    ff->setRotate(mrec.opt->rotate);
    ff->setYOfsset(mrec.opt->y_offset);
    ff->setSmallCaps(mrec.opt->smallcaps);
    
    ff->setSubFontLigs(mrec.opt->subfontligs);
    if (mrec.opt->ligname && mrec.opt->ligsubfontid)
    {
    	QString ligname(mrec.opt->ligname);
    	QString ligsubid(mrec.opt->ligsubfontid);
    	ff->setSubFontLig(ligname, ligsubid);
    }
    
    if (mrec.opt->inencname)
    {
    	QString inencnameA(mrec.opt->inencname);
    	ff->setInEncName(inencnameA);
    }
    
    if (mrec.opt->outencname)
    {
    	QString outencnameA(mrec.opt->outencname);
    	ff->setOutEncName(outencnameA);
    }
    
    ff->setCharSize(mrec.opt->size, 72, 72);
    
    return ff;
}

XWFontFileFT * XWFontFileFT::loadType3(const char * texnameA, 
                                       int base_dpi,
                                       int vDPI,
                                       double point_size,
                                       char * usedchars,
                                       double *FontBBox,
                                       double * Widths,
                                       double *FontMatrix,
                                       int    * FirstChar,
                                       int    * LastChar)
{
	XWFontFileFT * ff = loadTexFont(texnameA);
    if (!ff)
        return 0;
		
	ff->setCharSize(point_size, base_dpi, vDPI);

	ff->doType3(base_dpi, vDPI, point_size, usedchars, FontBBox, Widths, FontMatrix, FirstChar, LastChar);
	if (*FirstChar > *LastChar)
	{
		delete ff;
		return 0;
	}
	
    return ff;
}

FTFontInfo * XWFontFileFT::newChar()
{
	FTFontInfo * ti = new FTFontInfo;
    ti->next = charList;
    ti->charcode = -1;
    ti->glyphindex = -1;
    ti->incode = -1;
    ti->outcode = -1;
    ti->adobename = NULL;

    ti->width = -1;
    ti->llx = -1;
    ti->lly = -1;
    ti->urx = -1;
    ti->ury = -1;

    ti->ligs = NULL;
    ti->rligs = true;
    ti->kerns = NULL;
    ti->rkerns = true;
    ti->kern_equivs = NULL;
    ti->rptrs = true;
    ti->pccs = NULL;
    ti->rpccs = true;

    ti->constructed = false;

    ti->wptr = 0;
    ti->hptr = 0;
    ti->dptr = 0;
    ti->iptr = 0;

    charList = ti;

    return ti;
}

Kern * XWFontFileFT::newKern()
{
	Kern * ret = new Kern;
    ret->next = 0;
    ret->succ = 0;
    ret->delta = 0;

    return ret;
}

Lig  * XWFontFileFT::newLig()
{
	Lig * ret = new Lig;
    ret->next = 0;
    ret->succ = 0;
    ret->sub = 0;
    ret->op = 0;
    ret->boundleft = 0;

    return ret;
}

Pcc  * XWFontFileFT::newPcc()
{
	Pcc *ret = new Pcc;
    ret->next = 0;
    ret->partname = 0;
    ret->xoffset = 0;
    ret->yoffset = 0;

    return ret;
}

StringList * XWFontFileFT::newStringList()
{
	StringList * ret = new StringList;
    ret->next = 0;
    ret->old_name = 0;
    ret->new_name = 0;
    ret->single_replacement = false;

    return ret;
}

Encoding * XWFontFileFT::readEncoding(const QString & enc, bool ignoreligkern)
{
	Encoding *e = new Encoding;
    char ** baseEnc = getBaseEnc(enc);
    if (baseEnc)
    {
        QByteArray ba = enc.toAscii();
        e->name = qstrdup(ba.constData());
        for (int i = 0; i < 256; i++)
        {
            if (baseEnc[i] != NULL)
                e->vec[i] = qstrdup(baseEnc[i]);
            else
                e->vec[i] = 0;
        }
    }
    else if (!enc.isEmpty())
    {
        XWFontSea fontsea;
        QFile * enc_file = fontsea.openEnc(enc);
        if (!enc_file)
        {
            delete e;
            xwApp->openError(enc, false);
            return 0;
        }

        char * buffer = 0;
        char numbuf[9];
        ulong offset = 0;
        char * p = getToken(&buffer, &offset, enc_file, ignoreligkern, true);
        if (!p || *p != '/' || p[1] == '\0')
        {
            QString msg = QString(tr("first token in encoding must be literal encoding name(%1).\n"))
                     .arg(enc_file->fileName());
            xwApp->error(msg);
            goto endenc;
        }

        e->name = qstrdup(p + 1);
        delete [] p;

        p = getToken(&buffer, &offset, enc_file, ignoreligkern, false);
        if (!p || strcmp(p, "["))
        {
            QString msg = QString(tr("second token in encoding must be mark ([) token(%1).\n"))
                         .arg(enc_file->fileName());
            xwApp->error(msg);
            goto endenc;
        }
        delete [] p;

        for (int i = 0; i < 256; i++)
        {
            p = getToken(&buffer, &offset, enc_file, ignoreligkern, false);
            if (!p || *p != '/' || p[1] == 0)
            {
                QString msg = QString(tr("tokens 3 to 257 in encoding must be literal names(%1).\n"))
                           .arg(enc_file->fileName());
                xwApp->error(msg);
                goto endenc;
            }

            char c = p[2];
            if (p[1] == '.' && (c == 'c' || c == 'g') && '0' <= p[3] && p[3] <= '9')
            {
                char * q = 0;
                long l = strtol(p + 3, &q, 0);
                if (*q != '\0' || l < 0 || l > 0x16FFFF)
                {
                    QString msg = QString(tr("invalid encoding token(%1).\n")).arg(enc_file->fileName());
                    xwApp->error(msg);
                    goto endenc;
                }

                sprintf(numbuf, ".%c0x%lx", c, l);
                e->vec[i] = qstrdup(numbuf);
            }
            else
                e->vec[i] = qstrdup(p + 1);

            delete [] p;
        }

        p = getToken(&buffer, &offset, enc_file, ignoreligkern, false);
        if (!p || strcmp(p, "]"))
        {
            QString msg = QString(tr("token 258 in encoding must be make-array(])(%1).\n"))
                                  .arg(enc_file->fileName());
            xwApp->error(msg);
            goto endenc;
        }

        delete [] p;

        while (getLine(&buffer, enc_file))
        {
            for (p = buffer; *p; p++)
            {
            	if (*p == '%')
            	{
                	if (ignoreligkern == false)
                    	checkLigKern(p);
                	*p = '\0';
                	break;
            	}
            }
        }

        if (ignoreligkern == false && sawLigKern == false)
            getLigKernDefaults();

        p = 0;

endenc:
        if (p)
            delete [] p;

        if (buffer)
            free(buffer);

        enc_file->close();
        delete enc_file;
    }
    else
    {
    	delete e;
        e = 0;
        if (ignoreligkern == false)
        {
            e = getStaticEncoding();
            getLigKernDefaults();
        }
    }

    return e;
}

void XWFontFileFT::readFT()
{
	FTFontInfo * ti = 0;
    QList< QPair<FT_ULong, FT_UInt> > pairs;
    QPair<FT_ULong, FT_UInt> pair;
    int j = -1;
    if (onlyRange)
    {
        for (int i = 0; i < 256; i++)
        {
            pair.first = sfCode[i];
            if (sfCode[i] > 0)
            {
                if (j == -1)
                {
                    bc = i;
                    j = i;
                }

                ec = i;

                pair.second = FT_Get_Char_Index(face, sfCode[i]);
                if (pair.second != 0)
                    pairs << pair;
            }
        }
    }
    else
    {
        FT_UInt   gindex = 0;
        FT_ULong  charcode = FT_Get_First_Char( face, &gindex);
        bc = (int)charcode;
        while (gindex != 0)
        {
            pair.first = charcode;
            if (bc > (int)charcode)
                bc = (int)charcode;
            pair.second = gindex;
            pairs << pair;
            charcode = FT_Get_Next_Char(face, charcode, &gindex);
            if (charcode > ec)
                ec = charcode;
        }
    }

    int n = ec - bc + 1;
    if (n > 256 || ec > 255 || bc > 255)
    {
        fontLevel = 0;
        subfontLigs = false;
    }
    else
    {
    	fontLevel = -1;
        n = 256;
        j = bc;
        if (!onlyRange)
        {
            ti = newChar();
            ti->charcode = -1;
            ti->adobename = qstrdup(".notdef");

            ti = newChar();
            ti->charcode = -1;
            ti->adobename = qstrdup("||");

            outencptrs = (FTFontInfo**)malloc(n * sizeof(FTFontInfo*));
            nextout = (short*)malloc(n * sizeof(short));
            uppercase = (FTFontInfo**)malloc(n * sizeof(FTFontInfo*));
            lowercase = (FTFontInfo**)malloc(n * sizeof(FTFontInfo*));
            for (int i = 0; i < n; i++)
            {
                outencptrs[i] = 0;
                nextout[i] = -1;
                uppercase[i] = 0;
                lowercase[i] = 0;
            }
        }
    }

    inencptrs = (FTFontInfo**)malloc(n * sizeof(FTFontInfo*));
    for (int i = 0; i < n; i++)
        inencptrs[i] = 0;

    FT_GlyphSlot slot = 0;
    FT_BBox  bbox;
    char nbuf[100];
    FT_Glyph aglyph;
    for (int i = 0; i < pairs.size(); i++)
    {
        FT_UInt glyph_index = pairs.at(i).second;
        if (FT_IS_SCALABLE(face))
        {
            FT_Load_Glyph(face, glyph_index, FT_LOAD_NO_BITMAP);
            slot = face->glyph;

            if (efactor != 1.0 || slant != 0.0 )
                FT_Outline_Transform(&(slot->outline), &matrix1);

            if (rotate)
                FT_Outline_Transform(&(slot->outline), &matrix2);

            if (bold)
                FT_Outline_Embolden(&(slot->outline), (int)(boldf * 64));

            FT_Outline_Get_BBox(&(slot->outline), &bbox);
        }
        else
        {
            FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
            slot = face->glyph;
            FT_GlyphSlot_Own_Bitmap(slot);
            if (bold)
                FT_Bitmap_Embolden(lib, &(slot->bitmap), (int)(boldf * 64), (int)(boldf * 64));
            
            FT_Get_Glyph(slot, &aglyph);
            FT_Glyph_Get_CBox(aglyph, FT_GLYPH_BBOX_GRIDFIT, &bbox);
        }
        
        nbuf[0] = '\0';

        if (FT_HAS_GLYPH_NAMES(face))
            FT_Get_Glyph_Name(face, glyph_index, nbuf, 100);
        else
            codeToAdobeName(pairs.at(i).first, nbuf);

        FTFontInfo * ti = newChar();
        ti->charcode = pairs.at(i).first;
        ti->glyphindex = glyph_index;
        if (nbuf[0] != '\0')
            ti->adobename = qstrdup(nbuf);

        ti->llx = bbox.xMin * 1000 / unitsPerEm;
        ti->lly = bbox.yMin * 1000 / unitsPerEm;
        ti->urx = bbox.xMax * 1000 / unitsPerEm;
        ti->ury = bbox.yMax * 1000 / unitsPerEm;
        if (rotate)
        {
            ti->llx += (slot->metrics.vertBearingY - bbox.xMin) * 1000 / unitsPerEm;
            ti->lly -= 1000 * y_offset;
            ti->urx += (slot->metrics.vertBearingY - bbox.xMin) * 1000 / unitsPerEm;
            ti->ury -= 1000 * y_offset;
        }

        if (ti->lly > 0)
            ti->lly = 0;
        if (ti->ury < 0)
            ti->ury = 0;
        if (rotate)
            ti->width = slot->metrics.vertAdvance * 1000 / unitsPerEm;
        else
            ti->width = transform(slot->metrics.horiAdvance * 1000 / unitsPerEm, 0, efactor, slant);

        if (fontLevel != -1)
            inencptrs[ti->charcode - bc] = ti;
        else if (j < 256)
        {
            inencptrs[j] = ti;
            ti->incode = j;
        }
        j++;
        if (!FT_IS_SCALABLE(face))
  			FT_Done_Glyph(aglyph);
    }

    if (!onlyRange && fontLevel == -1)
    {
        FTFontInfo * Ti = 0;
        if (!findAdobe("Germandbls") && NULL != (Ti = findAdobe("S")))
        {
            ti = newChar();
            ti->charcode = pairs.size() | 0x1000000;
            ti->glyphindex = pairs.size();
            ti->adobename = qstrdup("Germandbls");
            ti->width = Ti->width << 1;
            ti->llx = Ti->llx;
            ti->lly = Ti->lly;
            ti->urx = Ti->width + Ti->urx;
            ti->ury = Ti->ury;
            ti->kerns = Ti->kerns;
            ti->rkerns = false;

            Pcc * npcc = newPcc();
            npcc->partname = qstrdup("S");
            Pcc * nqcc  = newPcc();
            nqcc->partname = qstrdup("S");
            nqcc->xoffset = Ti->width;
            npcc->next = nqcc;
            ti->pccs = npcc;
            ti->constructed = true;
        }

        readKern();
    }
    
    if (xheight == 0)
    {
        if (NULL != (ti = findAdobe("x")))
            xheight = ti->ury;
        else if (pid == 3 && eid == 1 && 0 != (ti = findAdobe(".c0x78")))
            xheight = ti->ury;
        else
            xheight = 400;
    }

    if (0 != (ti = findAdobe("space")))
        fontSpace = ti->width;
    else if (NULL != (ti = findAdobe(".c0x20")))
        fontSpace = ti->width;
    else
        fontSpace = transform(500, 0, efactor, slant);
}

void XWFontFileFT::readSubfontLigs()
{
	if (!hasSubfontLigs())
		return ;
		
	QString subfont(ligName);
	QString subfontid(ligSubID);
	XWSubfont sfd;
    if (!sfd.setSFDName(subfont) || (-1 == sfd.loadRecord(subfontid)))
        return ;
        
    sfd.getCode(sfCode);
}

void XWFontFileFT::releaseFTFontPtr(FTFontPtr * fprt)
{
	FTFontPtr * cur = fprt;
    while (cur)
    {
        FTFontPtr * tmp = cur->next;
        delete cur;
        cur = tmp;
    }
}

bool XWFontFileFT::setCharMap(int pidA, int eidA)
{
	pid = pidA;
	eid = eidA;
	for (int i = 0; i < face->num_charmaps; i++)
    {
        FT_CharMap charmap = face->charmaps[i];
        if ((charmap->platform_id == pid) && (charmap->encoding_id == eid))
        {
            FT_CharMap cm = charmap;
            if (cm->encoding == FT_ENCODING_UNICODE)
                currentEncodingScheme = encUnicode;
            else if (cm->encoding == FT_ENCODING_APPLE_ROMAN)
                currentEncodingScheme = encMac;
            else
                currentEncodingScheme = encFontSpecific;

            FT_Set_Charmap(face, cm);
            return true;
        }
    }
    
    QString msg = QString(tr("invalid charmap.(pid = %1, eid = %2)\n")).arg(pid).arg(eid);
    xwApp->warning(msg);
    return false;
}

void XWFontFileFT::setCharSize(double point_size, int hDPI, int vDPI)
{
	if (FT_Set_Char_Size(face, point_size * 64, point_size * 64, hDPI, vDPI))
		return; 
		
	size = point_size;
	matrix1.xx = (FT_Fixed)(floor(efactor * 1024) * (1L<<16)/1024);
    matrix1.xy = (FT_Fixed)(floor(slant * 1024) * (1L<<16)/1024);
    matrix1.yx = (FT_Fixed)0;
    matrix1.yy = (FT_Fixed)(1L<<16);
    if (rotate)
    {
        matrix2.xx = 0;
        matrix2.yx = 1L << 16;
        matrix2.xy = -matrix2.yx;
        matrix2.yy = matrix2.xx;
    }
}

void XWFontFileFT::setResolution(int hDPI, int vDPI)
{
	FT_Set_Char_Size(face, size * 64, size * 64, hDPI, vDPI);
}

void XWFontFileFT::setSubFont(const QString & subfont, 
	                          const QString & subfontid)
{
	if (subfont.isEmpty())
		return ;
		
	onlyRange = true;
	QByteArray ba = subfont.toAscii();
	ba.insert(0, "CJK-");
    if (ba.size() >= 39)
        ba.resize(39);
        
    if (codingScheme)
    	delete [] codingScheme;
    	
    codingScheme = new char[ba.size() + 1];
    qstrncpy(codingScheme, ba.constData(), ba.size());
    
    XWSubfont sfd;
    if (!sfd.setSFDName(subfont) || (-1 == sfd.loadRecord(subfontid)))
        return ;
        
    sfd.getCode(sfCode);
}

void XWFontFileFT::setSubFontLig(const QString & subfont, 
	                             const QString & subfontid)
{
	if (ligName)
	{
		delete [] ligName;
		ligName = 0;
	}
		
	if (ligSubID)
	{
		delete [] ligSubID;
		ligSubID = 0;
	}
	
	if (!subfont.isEmpty() && !subfontid.isEmpty())
	{
		QByteArray ba = subfont.toAscii();
		ligName = new char[ba.size() + 1];
    	qstrncpy(ligName, ba.constData(), ba.size());
    	
    	ba = subfontid.toAscii();
    	ligSubID = new char[ba.size() + 1];
    	qstrncpy(ligSubID, ba.constData(), ba.size());
    	subfontLigs = true;
	}
	else
		subfontLigs = false;
}

void XWFontFileFT::upMap()
{
	if (!smallcaps)
		return ;
		
	FTFontInfo * ti = 0;
    FTFontInfo * Ti = 0;
    char lwr[50];
    for (Ti = charList; Ti; Ti = Ti->next)
    {
        char * p = Ti->adobename;
        if (isupper(*p))
        {
            char * q = lwr;
            for (; *p; p++)
                *q++ = tolower(*p);
            *q = '\0';

            if (NULL != (ti = findMappedAdobe(lwr, inencptrs)))
            {
                for (int i = ti->outcode; i >= 0; i = nextout[i])
                    uppercase[i] = Ti;
                for (int i = Ti->outcode; i >= 0; i = nextout[i])
                    lowercase[i] = ti;
            }
        }
    }

    if (0 != (ti = findMappedAdobe("germandbls", inencptrs)))
    {
        if (0 != (Ti = findMappedAdobe("S", inencptrs)))
        {
            for (int i = ti->outcode; i >= 0; i = nextout[i])
                uppercase[i] = ti;
            ti->incode = -1;
            ti->width = Ti->width << 1;
            ti->llx = Ti->llx;
            ti->lly = Ti->lly;
            ti->urx = Ti->width + Ti->urx;
            ti->ury = Ti->ury;
            if (ti != Ti && ti->kerns && ti->rkerns)
                releaseKern(ti->kerns);
            ti->kerns = Ti->kerns;
            ti->rkerns = false;

            Pcc * npcc = newPcc();
            npcc->partname = qstrdup("S");
            Pcc * nqcc = newPcc();
            nqcc->partname = qstrdup("S");
            nqcc->xoffset = Ti->width;
            npcc->next = nqcc;
            if (ti != Ti && ti->pccs && ti->rpccs)
                releasePcc(ti->pccs);
            ti->pccs = npcc;
            ti->constructed = true;
        }
    }

    for (int i = 0; caseTab[i].upper; i++)
    {
        if ((ti = findMappedAdobe(caseTab[i].lower, inencptrs)))
        {
            for (int j = ti->outcode; j >= 0; j = nextout[j])
                uppercase[j] = findMappedAdobe(caseTab[i].upper, inencptrs);
        }
    }
}

void XWFontFileFT::addKern(char *s1, char *s2)
{
	FTFontInfo *ti1 = findAdobe(s1);
    FTFontInfo *ti2 = findAdobe(s2);

    if (ti1 && ti2 && !ti1->kerns)
    {
        FTFontPtr *ap = new FTFontPtr;
        ap->next = ti2->kern_equivs;
        ap->ch = ti1;
        ti2->kern_equivs = ap;
    }
}

void XWFontFileFT::addToStream(uchar *rowptr, long rowbytes)
{
	stream = (uchar*)realloc(stream, (streamLength + rowbytes) * sizeof(uchar));
	memcpy(stream + streamLength, rowptr, rowbytes);
	streamLength += rowbytes;
}

void XWFontFileFT::checkLigKern(char *s)
{
	char * os = qstrdup(s);
    char * orig_s = s;
    s++;
    while (isspace(*s))
        s++;

    if (strncmp(s, "LIGKERN", 7) == 0)
    {
        sawLigKern = true;
        s += 7;
        while (isspace(*s))
            s++;
        char * pos = s;
        char *mlist[5];
        ulong offset[5];
        int n = 0;
        while (*pos)
        {
            for (n = 0; n < 5;)
            {
                if (*pos == '\0')
                    break;
                offset[n] = pos - orig_s;
                mlist[n] = paramString(&pos);
                if (strcmp(mlist[n], ";") == 0)
                    break;
                n++;
            }

            if (n == 3 && strcmp(mlist[1], "{}") == 0)
                rmKern(mlist[0], mlist[2], 0);
            else if (n == 3 && strcmp(mlist[1], "<>") == 0)
                addKern(mlist[0], mlist[2]);
            else if (n == 3 && strcmp(mlist[0], "||") == 0 && strcmp(mlist[1], "=") == 0)
            {
                FTFontInfo *ti = findAdobe("||");
                if (boundaryChar != -1)
                {
                    delete [] os;
                    return ;
                }

                if (sscanf(mlist[2], "%d", &n) != 1)
                {
                    delete [] os;
                    return ;
                }

                if (n < 0 || n > 0xFF)
                {
                    delete [] os;
                    return ;
                }

                boundaryChar = n;
                if (ti == 0)
                {
                    delete [] os;
                    return ;
                }

                ti->outcode = n;
            }
            else if (n == 4)
            {
                int op = -1;
                for (n = 0; encligops[n]; n++)
                {
                    if (strcmp(mlist[2], encligops[n]) == 0)
                    {
                        op = n;
                        break;
                    }
                }

                if (op < 0)
                {
                    delete [] os;
                    return ;
                }

                FTFontInfo *ti = findAdobe(mlist[0]);
                if (0 != ti)
                {
                    if (findAdobe(mlist[1]))
                        rmKern(mlist[0], mlist[1], ti);

                    if (strcmp(mlist[3], "||") == 0)
                    {
                        delete [] os;
                        return ;
                    }

                    if (!fixedPitch)
                    {
                        Lig *lig = ti->ligs;
                        for (; lig; lig = lig->next)
                        {
                            if (strcmp(lig->succ, mlist[1]) == 0)
                                break;
                        }

                        if (lig == 0)
                        {
                            lig = newLig();
                            lig->succ = qstrdup(mlist[1]);
                            lig->next = ti->ligs;
                            ti->ligs = lig;
                        }

                        lig->sub = qstrdup(mlist[3]);
                        lig->op = op;
                        if (strcmp(mlist[1], "||") == 0)
                        {
                            lig->boundleft = 1;
                            if (strcmp(mlist[0], "||") == 0)
                            {
                                delete [] os;
                                return ;
                            }
                        }
                        else
                            lig->boundleft = 0;
                    }
                }
            }
            else
            {
                delete [] os;
                return ;
            }
        }
    }

    delete [] os;
}

void XWFontFileFT::getLigKernDefaults()
{
	for (int i = 0; staticligkern[i]; i++)
    {
        char * buffer = qstrdup(staticligkern[i]);
        checkLigKern(buffer);
        delete [] buffer;
    }
}

char * XWFontFileFT::getToken(char **bufferp, 
	                          ulong *offsetp, 
	                          QIODevice *f,
         			          bool ignoreligkern, 
         			          bool init)
{
	static char *curp;
    if (init)
        curp = 0;

    char *p = 0;
    char *q = 0;
    char tempchar = 0;
    while (1)
    {
        while (curp == NULL || *curp == '\0')
        {
            if (*bufferp)
                free(*bufferp);

            *bufferp = 0;

            if (!getLine(bufferp, f))
            {
                if (*bufferp)
                    free(*bufferp);

                *bufferp = 0;

                xwApp->error(tr("premature end in encoding file.\n"));
                return 0;
            }

            curp = *bufferp;

            for (p = *bufferp; *p; p++)
            {
                if (*p == '%')
                {
                    if (ignoreligkern == false)
                        checkLigKern(p);
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
                     *curp == '-' || *curp == '_' || *curp == '.' ||
                     ('0' <= *curp && *curp <= '9') ||
                     ('a' <= *curp && *curp <= 'z') ||
                     ('A' <= *curp && *curp <= 'Z'))
            {
                q = curp++;
                while (*curp == '-' || *curp == '_' || *curp == '.' ||
                        ('0' <= *curp && *curp <= '9') ||
                        ('a' <= *curp && *curp <= 'z') ||
                        ('A' <= *curp && *curp <= 'Z'))
                    curp++;
            }
            else
                q = curp;

            tempchar = *curp;
            *curp = '\0';
            p = qstrdup(q);
            *curp = tempchar;
            return p;
        }
    }
    
    return 0;
}

char * XWFontFileFT::paramString(char **curp)
{
	char * p = *curp;
    while (*p && !isspace(*p))
        p++;
    char * q = *curp;
    if (*p != '\0')
        *p++ = '\0';
    while (isspace(*p))
        p++;
    *curp = p;
    return q;
}

void XWFontFileFT::readKern()
{
	if (!isSFNT())
        return ;

    FT_ULong tag = FT_MAKE_TAG('k', 'e', 'r', 'n');
    FT_ULong len = 0;
    if (FT_Load_Sfnt_Table(face, tag, 0, NULL, &len))
        return ;

    QByteArray tab;
    tab.resize(len);
    FT_Load_Sfnt_Table(face, tag, 0, reinterpret_cast<uchar *>(tab.data()), &len);
    const uchar *table = reinterpret_cast<const uchar *>(tab.constData());
    ushort version = qFromBigEndian<quint16>(table);
    if (version != 0)
        return ;

    ushort numTables = qFromBigEndian<quint16>(table + 2);
    int offset = 4;
    QList< QPair<uint, int> > pairs;
    QPair<uint, int> pair;
    for(int i = 0; i < numTables; ++i)
    {
        if (offset + 6 > tab.size())
            goto end;

        const uchar *header = table + offset;
        version = qFromBigEndian<quint16>(header);
        ushort length = qFromBigEndian<quint16>(header+2);
        ushort coverage = qFromBigEndian<quint16>(header+4);
        if(version == 0 && coverage == 0x0001)
        {
            if (offset + length > tab.size())
                goto end;

            const uchar *data = table + offset + 6;
            ushort nPairs = qFromBigEndian<quint16>(data);
            if(nPairs * 6 + 8 > length - 6)
                goto end;

            int off = 8;
            for(int i = 0; i < nPairs; ++i)
            {
                pair.first = (((uint)qFromBigEndian<quint16>(data+off)) << 16) + qFromBigEndian<quint16>(data+off+2);
                pair.second = (int)qFromBigEndian<quint16>(data+off+4);
                pairs << pair;
            }
        }

        offset += length;
    }

end:
    for (int i = 0; i < pairs.size(); i++)
    {
        uint tmp = pairs.at(i).first;
        ushort left = (ushort)(tmp >> 16);
        ushort right = (ushort)(tmp && 0xffff);
        ushort value = pairs.at(i).second;
        FTFontInfo * ti_l = findGlyph(left);
        if (!ti_l)
            continue;
        FTFontInfo * ti_r = findGlyph(right);
        if (!ti_r)
            continue;

        Kern * nk = newKern();
        nk->succ = qstrdup(ti_r->adobename);
        nk->delta = transform(value * 1000 / unitsPerEm, 0, efactor, slant);
        nk->next = ti_l->kerns;
        ti_l->kerns = nk;
    }
}

void XWFontFileFT::releaseChar(FTFontInfo * ti)
{
	if (ti)
    {
        if (ti->adobename)
            delete [] ti->adobename;

        if (ti->rligs && ti->ligs)
            releaseLig(ti->ligs);

        if (ti->rkerns && ti->kerns)
            releaseKern(ti->kerns);

        if (ti->rptrs && ti->kern_equivs)
            releaseFTFontPtr(ti->kern_equivs);

        if (ti->rpccs && ti->pccs)
            releasePcc(ti->pccs);

        delete ti;
    }
}

void XWFontFileFT::releaseKern(Kern * kern)
{
	Kern * cur = kern;
    while (cur)
    {
        Kern * tmp = cur->next;
        if (cur->succ)
            delete [] cur->succ;

        delete cur;
        cur = tmp;
    }
}

void XWFontFileFT::releaseLig(Lig * lig)
{
	Lig * cur = lig;
    while (cur)
    {
        Lig * tmp = cur->next;
        if (cur->succ)
            delete [] cur->succ;

        if (cur->sub)
            delete [] cur->sub;

        delete cur;
        cur = tmp;
    }
}

void XWFontFileFT::releasePcc(Pcc * pcc)
{
	Pcc * cur = pcc;
    while (cur)
    {
        Pcc * tmp = cur->next;
        if (cur->partname)
            delete [] cur->partname;
        delete cur;
        cur = tmp;
    }
}

void XWFontFileFT::rmKern(char *s1, 
	                      char *s2,
                          FTFontInfo *ti)
{
	if (ti == NULL)
    {
        if (strcmp(s1, "*") == 0)
        {
            for (ti = charList; ti; ti = ti->next)
                rmKern(s1, s2, ti);
            return;
        }
        else
        {
            ti = findAdobe(s1);
            if (ti == 0)
                return;
        }
    }

    if (strcmp(s2, "*") == 0)
    {
        if (ti->kerns && ti->rkerns)
            releaseKern(ti->kerns);
        ti->kerns = 0;
    }
    else
        ti->kerns = rmKernMatch(ti->kerns, s2);
}

Kern * XWFontFileFT::rmKernMatch(Kern *k, char *s)
{
	while (k && strcmp(k->succ, s) == 0)
        k = k->next;

    if (k)
    {
        Kern * curk = k;
        for (; curk; curk = curk->next)
        {
            while (curk->next && strcmp(curk->next->succ, s) == 0)
            {
                Kern * tmp = curk->next;
                curk->next = curk->next->next;
                delete tmp;
            }
        }
    }
    return k;
}

/*
void XWFontFileFT::replaceGlyphs()
{
    StringList * sl = replacements;
    StringList * sl_old = 0;
    while (sl)
    {
        FTFontInfo * ti = findAdobe(sl->old_name);
        if (ti)
        {
            if (ti->adobename)
                delete [] ti->adobename;
            ti->adobename = qstrdup(sl->new_name);
        }
        else
        {
            if (!sl_old)
            {
                if (replacements)
                    delete replacements;
                replacements = sl->next;
            }
            else
            {
                if (sl_old->next)
                    delete sl_old->next;

                sl_old->next = sl->next;
            }
        }

        sl_old = sl;
        sl = sl->next;
    }
}
*/
