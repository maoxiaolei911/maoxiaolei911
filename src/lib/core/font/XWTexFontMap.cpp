/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <QByteArray>

#include "XWApplication.h"
#include "XWLexer.h"
#include "XWFontSea.h"
#include "XWTexSea.h"
#include "XWSubfont.h"
#include "XWTexFontMap.h"

static bool no_embed = false;
static FT_Library ftlib;
static int fileCount = 0;

#ifndef ISBLANK
#define ISBLANK(c) ((c) == ' ' || (c) == '\t')
#endif

#define CID_MAPREC_CSI_DELIM '/'

static void
skip_blank (char **pp, char *endptr)
{
  	char  *p = *pp;
  	if (!p || p >= endptr)
    	return;
  	for ( ; p < endptr && ISBLANK(*p); p++);
  	*pp = p;
}


static char * 
readline(char *buf, int buf_len, QIODevice *fp)
{
	if (fp->readLine(buf, buf_len) <= 0)
		return 0;
		
	char * p = buf;
	if (p[0] == '\r' || p[0] == '\n')
		p[0] = '\0';
		
	char * q = strchr(p, '%');
	
	if (q)
    	*q = '\0';
    	
  	return  p;
}

static char *
parse_string_value (char **pp, char *endptr)
{
	char  *q = 0, *p = *pp;
	
	if (!p || p >= endptr)
    	return  0;
    	
    if (*p == '"')
    {
    	XWLexer lexer(p, endptr);
    	q = lexer.getCString();
    	p = (char*)(lexer.getCurPtr());
    }
    else
    {
    	int n = 0;
    	while (p < endptr && !isspace(*p))
    	{
    		p++;
    		n++;
    	}
    	
    	if (n == 0)
      		return  0;
    	q = new char[n + 1];
    	memcpy(q, *pp, n); q[n] = '\0';
    }
    
    *pp = p;
  	return  q;
}

static char *
parse_integer_value (char **pp, char *endptr, int base)
{
	char *p = *pp;
	if (!p || p >= endptr)
    	return  0;
    	
    int has_sign = 0, has_prefix = 0;
    if (*p == '-' || *p == '+') 
    {
    	p++; 
    	has_sign = 1;
  	}
  	
  	if ((base == 0 || base == 16) && 
  		p + 2 <= endptr && 
      	p[0] == '0' && p[1] == 'x') 
    {
    	p += 2; 
    	has_prefix = 1;
  	}
  	
  	if (base == 0) 
  	{
    	if (has_prefix)
      		base = 16;
    	else if (p < endptr && *p == '0')
      		base = 8;
    	else 
      		base = 10;
  	}
  	
#define ISDIGIT_WB(c,b) ( \
  ((b) <= 10 && (c) >= '0' && (c) < '0' + (b)) || \
  ((b) >  10 && ( \
      ((c) >= '0' && (c) <= '9') || \
      ((c) >= 'a' && (c) < 'a' + ((b) - 10)) || \
      ((c) >= 'A' && (c) < 'A' + ((b) - 10)) \
    ) \
  ) \
)
	int n = 0;
	while (p < endptr && ISDIGIT_WB(*p, base))
	{
		p++;
		n++;
	}
	
  	if (n == 0)
    	return  0;
    	
  	if (has_sign)
    	n += 1;
    	
  	if (has_prefix)
    	n += 2;
    	
    char * q = new char[n + 1];
  	memcpy(q, *pp, n); q[n] = '\0';

  	*pp = p;
  	return  q;
}

static char * chopSFDName(const char *tex_name, char **sfd_name)
{
	*sfd_name = 0;
  	char * p = strchr((char *) tex_name, '@');
  	if (!p || p[1] == '\0' || p == tex_name) 
    	return  0;
    	
    int m = (int) (p - tex_name);
  	p++;
  	char * q = strchr(p, '@');
  	if (!q || q == p) 
    	return 0;
    	
  	int n = (int) (q - p);
  	q++;

  	int len = strlen(tex_name) - n;
  	char * fontname = new char[len+1];
  	memcpy(fontname, tex_name, m);
  	fontname[m] = '\0';
  	if (*q)
    	strcat(fontname, q);

  	*sfd_name = new char[n+1];
  	memcpy(*sfd_name, p, n);
  	(*sfd_name)[n] = '\0';

  	return  fontname;
}


static char *
substr (char **str, char stop)
{
  	char * endptr = strchr(*str, stop);
  	if (!endptr || endptr == *str)
    	return 0;
  	char * sstr = new char[endptr-(*str)+1];
  	memcpy(sstr, *str, endptr-(*str));
  	sstr[endptr-(*str)] = '\0';

  	*str = endptr+1;
  	return sstr;
}

XWTexFontMapOpt::XWTexFontMapOpt()
	:slant(0.0),
	 extend(1.0),
	 bold(0.0),
	 mapc(-1),
	 flags(0),
	 otl_tags(0),
	 tounicode(0),
	 design_size(-1.0),
	 charcoll(0),
	 index(0),
	 style(FONTMAP_STYLE_NONE),
	 stemv(-1),
	 boldb(false),
	 usevf(false),
	 vfont(0),
	 pid(3),
	 eid(1),
	 rotate(false),
	 y_offset(0.25),
	 smallcaps(false),
	 subfontligs(false),
	 ligname(0),
	 ligsubfontid(0),
	 inencname(0),
	 outencname(0),
	 size(10.0),
	 ftface(0),
	 glyphwidths(0)
{
}

XWTexFontMapOpt::XWTexFontMapOpt(XWTexFontMapOpt * src)
{
	if (src)
	{
		slant = src->slant;
		extend = src->extend;
		bold = src->bold;
		mapc = src->mapc;
		flags = src->flags;
		design_size = src->design_size;
		index = src->index;
		style = src->style;
		stemv = src->stemv;
		boldb = src->boldb;
		usevf = src->usevf;
		pid = src->pid;
		eid = src->eid;
		rotate = src->rotate;
		y_offset = src->y_offset;
		smallcaps = src->smallcaps;
		subfontligs = src->subfontligs;
		size = src->size;
		if (src->otl_tags)
			otl_tags = qstrdup(src->otl_tags);
		else
			otl_tags = 0;
			
		if (src->tounicode)
			tounicode = qstrdup(tounicode);
		else
			tounicode = 0;
			
		if (src->charcoll)
			charcoll = qstrdup(src->charcoll);
		else
			charcoll = 0;
			
		if (src->vfont)
			vfont = qstrdup(src->vfont);
		else
			vfont = 0;
			
		if (src->ligname)
			ligname = qstrdup(src->ligname);
		else
			ligname = 0;
			
		if (src->ligsubfontid)
			ligsubfontid = qstrdup(src->ligsubfontid);
		else
			ligsubfontid = 0;
			
		if (src->inencname)
			inencname = qstrdup(src->inencname);
		else
			inencname = 0;
			
		if (src->outencname)
			outencname = qstrdup(src->outencname);
		else
			outencname = 0;
			
		ftface = src->ftface;
		glyphwidths = src->glyphwidths;
		src->ftface = 0;
		src->glyphwidths = 0;
	}
	else
	{
		slant = 0.0;
	 	extend = 1.0;
	 	bold = 0.0;
	 	mapc = -1;
	 	flags = 0;
	 	otl_tags = 0;
	 	tounicode = 0;
	 	design_size = -1.0;
	 	charcoll = 0;
	 	index = 0;
	 	style = FONTMAP_STYLE_NONE;
	 	stemv = -1;
	 	boldb = false;
	 	usevf = false;
	 	vfont = 0;
	 	pid = 3;
	 	eid = 1;
	 	rotate = false;
	 	y_offset = 0.25;
	 	smallcaps = false;
	 	subfontligs = false;
	 	ligname = 0;
	 	ligsubfontid = 0;
	 	inencname = 0;
	 	outencname = 0;
	 	size = 10.0;
	 	ftface = 0;
		glyphwidths = 0;
	}
}

XWTexFontMapOpt::~XWTexFontMapOpt()
{
	if (otl_tags)
		delete [] otl_tags;
		
	if (tounicode)
		delete [] tounicode;
		
	if (charcoll)
		delete [] charcoll;
		
	if (vfont)
		delete [] vfont;
		
	if (ligname)
		delete [] ligname;
		
	if (ligsubfontid)
		delete [] ligsubfontid;
		
	if (inencname)
		delete [] inencname;
		
	if (outencname)
		delete [] outencname;
		
	if (ftface)
		FT_Done_Face(ftface);
		
	if (glyphwidths)
		delete [] glyphwidths;
}

char * XWTexFontMapOpt::stripOptions(const char *map_name)
{
	char * p = (char *) map_name;
	char * next = 0;
	if (*p == ':' && isdigit(*(p+1))) 
	{
    	index = (int) strtoul(p+1, &next, 10);
    	if (*next == ':')
      		p = next + 1;
  	}
  	
  	if (*p == '!')
  	{
  		if (*(++p) == '\0')
  			return 0;
  		flags |= FONTMAP_OPT_NOEMBED;
  	}
  	
  	int   have_csi = 0, have_style = 0;
  	char * font_name = 0;
  	if ((next = strchr(p, CID_MAPREC_CSI_DELIM)) != NULL) 
  	{
    	if (next == p)
      		return 0;
    	font_name = substr(&p, CID_MAPREC_CSI_DELIM);
    	have_csi  = 1;
  	} 
  	else if ((next = strchr(p, ',')) != NULL) 
  	{
    	if (next == p)
      		return 0;
    	font_name = substr(&p, ',');
    	have_style = 1;
  	} 
  	else 
    	font_name = qstrdup(p);
    	
    if (have_csi) 
    {
    	if ((next = strchr(p, ',')) != NULL) 
    	{
      		charcoll = substr(&p, ',');
      		have_style = 1;
    	} 
    	else if (p[0] == '\0') 
    	{
      		delete [] font_name;
      		return 0;
    	} 
    	else 
      		charcoll = qstrdup(p);
  	}
  	
  	if (have_style) 
  	{
    	if (!strncmp(p, "BoldItalic", 10)) 
    	{
      		if (*(p+10))
      		{
      			delete [] font_name;
      			return 0;
        	}
      		style = FONTMAP_STYLE_BOLDITALIC;
    	} 
    	else if (!strncmp(p, "Bold", 4)) 
    	{
      		if (*(p+4))
      		{
      			delete [] font_name;
      			return 0;
        	}
      		style = FONTMAP_STYLE_BOLD;
    	} 
    	else if (!strncmp(p, "Italic", 6)) 
    	{
      		if (*(p+6))
      		{
      			delete [] font_name;
      			return 0;
        	}
      		style = FONTMAP_STYLE_ITALIC;
    	}
  	}
  	
  	if (no_embed)
  		flags |= FONTMAP_OPT_NOEMBED;

  	return font_name;
}

XWTexFontMap::XWTexFontMap(QObject * parent)
	:QObject(parent),
	 map_name(0),
	 font_name(0),
	 enc_name(0)
{
	charmap.sfd_name   = 0;
	charmap.subfont_id = 0;
	
	opt = new XWTexFontMapOpt;
}

XWTexFontMap::XWTexFontMap(const XWTexFontMap * src, QObject * parent)
	:QObject(parent),
	 map_name(0),
	 font_name(0),
	 enc_name(0)
{
	charmap.sfd_name   = 0;
	charmap.subfont_id = 0;
	
	if (src->map_name)
		map_name = qstrdup(src->map_name);
		
	if (src->font_name)
		font_name = qstrdup(src->font_name);
		
	if (src->enc_name)
		enc_name = qstrdup(src->enc_name);
		
	if (src->charmap.sfd_name)
		charmap.sfd_name = qstrdup(src->charmap.sfd_name);
		
	if (src->charmap.subfont_id)
		charmap.subfont_id = qstrdup(src->charmap.subfont_id);
		
	opt = new XWTexFontMapOpt(src->opt);
}

XWTexFontMap::XWTexFontMap(const char *name, 
	                         const char *path, 
	                         int index, 
	                         FT_Face face, 
	                         int layout_dir, 
	                         QObject * parent)
	:QObject(parent)
{
	char * fontmap_key = new char[strlen(name) + 3];
  sprintf(fontmap_key, "%s/%c", name, layout_dir == 0 ? 'H' : 'V');
  opt = new XWTexFontMapOpt;
  
  enc_name  = qstrdup(layout_dir == 0 ? "Identity-H" : "Identity-V");
  font_name = (path != 0) ? qstrdup(path) : 0;
  opt->index = index;
  opt->ftface = face;
  if (layout_dir != 0)
    opt->flags |= FONTMAP_OPT_VERT;
    
  fillInDefaults(fontmap_key);
  delete [] fontmap_key;
}

XWTexFontMap::~XWTexFontMap()
{
	if (map_name)
		delete [] map_name;
		
	if (font_name)
		delete [] font_name;
		
	if (enc_name)
		delete [] enc_name;
		
	if (charmap.sfd_name)
		delete [] charmap.sfd_name;
		
	if (charmap.subfont_id)
		delete [] charmap.subfont_id;
		
	if (opt)
		delete opt;
}

int XWTexFontMap::isPDFMMapLine(const char *mline)
{
	if (strchr(mline, '"') || strchr(mline, '<'))
    	return -1;

  	char * p = (char *) mline;
  	char * endptr = p + strlen(mline);

  	skip_blank(&p, endptr);

	int n = 0;
  	while (p < endptr) 
  	{
    	if (*p == '-') 
    		return 1;
    		
    	n++;
    	while (p < endptr && !ISBLANK(*p))
    		p++;
    	skip_blank(&p, endptr);
  	}

  	return (n == 2 ? 0 : 1);
}

int XWTexFontMap::readLine(const char *mline, long mline_len, int format)
{
	char * p = (char *) mline;
  	char * endptr = p + mline_len;

  	skip_blank(&p, endptr);
  	if (p >= endptr)
    	return -1;

  	char * q = parse_string_value(&p, endptr);
  	if (!q)
    	return -1;
    	
    int error = 0;
    if (format > 0)
    	error = parseMapdefDPM(p, endptr);
  	else
    	error = parseMapdefDPS(p, endptr);
  	if (!error) 
  	{
    	char * sfd_nameA = 0;
    	char * fnt_nameA = chopSFDName(q, &sfd_nameA);
    	if (fnt_nameA && sfd_nameA) 
    	{
      		if (!font_name) 
        		font_name = fnt_nameA;
      		else 
        		delete [] fnt_nameA;
        		
      		if (charmap.sfd_name)
        		delete [] charmap.sfd_name;
      		charmap.sfd_name = sfd_nameA;
    	}
    	fillInDefaults(q);
  	}
  	delete [] q;

  	return  error;
}

int XWTexFontMap::readSetting(const char *tfm_name)
{
	XWTeXFontSetting setting;
	QString tfmname(tfm_name);
	QString infix = setting.setTexName(tfmname);
	QString subfont = setting.getSubFont();
	map_name = qstrdup(tfm_name);
	if (!subfont.isEmpty() && !infix.isEmpty())
	{
		tfmname = tfmname.left(tfmname.length() - infix.length()); 
		tfmname += QString("@%1@").arg(subfont);
		QByteArray ba = tfmname.toAscii();
		map_name = qstrdup(ba.constData());
		ba = subfont.toAscii();
		charmap.sfd_name = qstrdup(ba.constData());
		ba = infix.toAscii();
		charmap.subfont_id = qstrdup(ba.constData());
	}
			
	QString font = setting.getFont();
	if (!font.isEmpty())
	{
		QByteArray ba = font.toAscii();
		font_name = qstrdup(ba.constData());
	}
	else
		font_name = qstrdup(tfm_name);
		
	QString enc = setting.getInEncoding();
	if (!enc.isEmpty() && enc != "default" && enc != "none")
	{
		QByteArray ba = enc.toAscii();
		enc_name = qstrdup(ba.constData());
	}
		
	opt->slant = setting.getSlant();
	opt->extend = setting.getEFactor();
	if (setting.isBold() && setting.isItalic())
		opt->style = FONTMAP_STYLE_BOLDITALIC;
	else if (setting.isItalic())
		opt->style = FONTMAP_STYLE_ITALIC;
	else if (setting.isBold())
		opt->style = FONTMAP_STYLE_BOLD;
			
	opt->bold = setting.getBold();
			
	QString coll = setting.getCollection();
	if (!coll.isEmpty())
	{
		QByteArray ba = coll.toAscii();
		opt->charcoll = qstrdup(ba.constData());
	}
		
	opt->index = setting.getIndex();
	opt->stemv = setting.getStemV();
	opt->boldb = setting.isBold();
	opt->usevf = setting.useVF();
	QString vf = setting.getVF();
	if (!vf.isEmpty())
	{
		QByteArray ba = vf.toAscii();
		opt->vfont = qstrdup(ba.constData());
	}
		
	opt->pid = setting.getPlatID();
	opt->eid = setting.getEncodingID();
	opt->rotate = setting.isRotate();
	opt->y_offset = setting.getYOffset();
	opt->smallcaps = setting.useSmallCaps();
	opt->subfontligs = setting.isSubfontLigs();
	opt->mapc = setting.getPlane();
		
	QString ligsubid;
    QString ligname = setting.getLigName(ligsubid);
    if (!ligname.isEmpty())
    {
    	QByteArray ba = ligname.toAscii();
		opt->ligname = qstrdup(ba.constData());
    }
    	
    if (!ligsubid.isEmpty())
    {
    	QByteArray ba = ligsubid.toAscii();
		opt->ligsubfontid = qstrdup(ba.constData());
    }
    	
    QString inencname = setting.getInEncoding();
    if (!inencname.isEmpty() && inencname != "default" && inencname != "none")
    {
    	QByteArray ba = inencname.toAscii();
			opt->inencname = qstrdup(ba.constData());
    }
    	
    QString outencname = setting.getOutEncoding();
    if (!outencname.isEmpty())
    {
    	QByteArray ba = outencname.toAscii();
		opt->outencname = qstrdup(ba.constData());
    }
    	
    opt->size = setting.getSize();
		
	QString cmap = setting.getCMap();
	if (!cmap.isEmpty())
	{
		QByteArray ba = cmap.toAscii();
		opt->tounicode = qstrdup(ba.constData());
	}
		
	if (no_embed || setting.isNoEmbed())
		opt->flags |= FONTMAP_OPT_NOEMBED;
			
	if (setting.getWMode() == 1)
		opt->flags |= FONTMAP_OPT_VERT;
	else if (setting.getWMode() == 0)
		opt->flags &= ~FONTMAP_OPT_VERT;
		
	if (!subfont.isEmpty() && !enc.isEmpty())
	{
		if ((enc_name == "Identity-H" || enc_name == "Identity-V") && 
		    (subfont.contains("Uni") || 
		     subfont.contains("UBig") || 
		     subfont.contains("UBg") || 
		     subfont.contains("UGB") || 
		     subfont.contains("UKS") || 
		     subfont.contains("UJIS")))
		{
			opt->charcoll = new char[4];
			strcpy(opt->charcoll, "UCS");
		}
	}
	
	if (font_name) 
  {
   	char * tmp = opt->stripOptions(font_name);
   	if (tmp) 
   	{
     		delete [] font_name;
     		font_name = tmp;
   	}
  }
		
	return 0;
}

int XWTexFontMap::readTTFontsMapLine(const char *mline, long mline_len)
{
	char * p = (char *) mline;
  	char * endptr = p + mline_len;

  	skip_blank(&p, endptr);
  	if (p >= endptr)
    	return -1;
    	
    char * q = parse_string_value(&p, endptr);
  	if (!q)
    	return -1;
    	    	
    map_name  = qstrdup(q);
    font_name = chopSFDName(q, &(charmap.sfd_name));
    delete [] q;
    
    if (font_name)
    {
    	delete [] font_name;
    	font_name = 0;
    }
    
    skip_blank(&p, endptr);
    font_name = parse_string_value(&p, endptr);
    if (!font_name)
    	return -1;
    	
    skip_blank(&p, endptr);
    char buf[256];
    while (p < endptr)
    {
    	q = p;
    	while (p < endptr && p[0] != '=' && !isspace(p[0]))
    		p++;
    		
    	int n = p - q;
    	if (n == 0)
    		break;
    		
    	if (p[0] == '=')
    		p++;    	
    	else
    	{    		
    		skip_blank(&p, endptr);
    		if (p[0] == '=')
    			p++;
    	}
    	
    	skip_blank(&p, endptr);    		
    	char * t = p;
    	if (!strncmp(q, "Slant", n))
    	{
    		while (p < endptr && (p[0] == '.' || isdigit(p[0])))
    			p++;
    			
    		if (p > t)
    		{
    			n = p - t;
    			strncpy(buf, t, n);
    			buf[n] = '\0';
    			opt->slant = atof(buf);
    		}
    	}
    	else if (!strncmp(q, "Extend", n))
    	{
    		while (p < endptr && (p[0] == '.' || isdigit(p[0])))
    			p++;
    			
    		if (p > t)
    		{
    			n = p - t;
    			strncpy(buf, t, n);
    			buf[n] = '\0';
    			opt->extend = atof(buf);
    		}
    	}
    	else if (!strncmp(q, "Fontindex", n))
    	{
    		while (p < endptr && isdigit(p[0]))
    			p++;
    			
    		if (p > t)
    		{
    			n = p - t;
    			strncpy(buf, t, n);
    			buf[n] = '\0';
    			opt->index = atoi(buf);
    		}
    	}
    	else if (!strncmp(q, "Pid", n))
    	{
    		while (p < endptr && isdigit(p[0]))
    			p++;
    			
    		if (p > t)
    		{
    			n = p - t;
    			strncpy(buf, t, n);
    			buf[n] = '\0';
    			opt->pid = atoi(buf);
    		}
    	}
    	else if (!strncmp(q, "Eid", n))
    	{
    		while (p < endptr && isdigit(p[0]))
    			p++;
    			
    		if (p > t)
    		{
    			n = p - t;
    			strncpy(buf, t, n);
    			buf[n] = '\0';
    			opt->eid = atoi(buf);
    		}
    	}
    	else if (!strncmp(q, "Y-Offset", n))
    	{
    		while (p < endptr && (p[0] == '.' || isdigit(p[0])))
    			p++;
    			
    		if (p > t)
    		{
    			n = p - t;
    			strncpy(buf, t, n);
    			buf[n] = '\0';
    			opt->y_offset = atof(buf);
    		}
    	}
    	else if (!strncmp(q, "Rotate", n))
    	{
    		while (p < endptr && isalpha(p[0]))
    			p++;
    			
    		if (p > t)
    		{
    			n = p - t;
    			strncpy(buf, t, n);
    			buf[n] = '\0';
    			if (!strcmp(buf, "Yes"))
    				opt->rotate = true;
    		}
    	}
    	else if (!strncmp(q, "Encoding", n))
    	{
    		while (p < endptr && !isspace(p[0]))
    			p++;
    			
    		if (p > t)
    		{
    			n = p - t;
    			enc_name = new char[n + 1];
    			strncpy(enc_name, t, n);
    			enc_name[n] = '\0';
    		}
    	}
    	else
    	{
    		while (p < endptr && !isspace(p[0]))
    			p++;
    	}
    	
    	skip_blank(&p, endptr);
    }
    
    return 0;
}

void XWTexFontMap::toSetting(XWTeXFontSetting * setting)
{
	char * map_nameA = qstrdup(map_name);
    char * sfd_nameA = 0;
    char * fnt_nameA = chopSFDName(map_nameA, &sfd_nameA);
    if (fnt_nameA && sfd_nameA)
    {
    	QString texname(fnt_nameA);			
		setting->setTexName(texname);
		QString sfd(sfd_nameA);
		setting->setSubFont(sfd);
		delete [] sfd_nameA;
		delete [] fnt_nameA;
	}
	else
	{
		QString texname(map_nameA);
		setting->setTexName(texname);
	}
			
	delete [] map_nameA;
			
	if (font_name)
	{
		QString fnt(font_name);
		setting->setFont(fnt);
	}
	
	if (enc_name)
	{
		QString enc(enc_name);
		setting->setInEncoding(enc);
	}
	
	setting->setSlant(opt->slant);
	setting->setEFactor(opt->extend);
	setting->setBoldF(opt->bold);
	
	if (opt->tounicode)
	{
		QString cmap(opt->tounicode);
		setting->setCMap(cmap);
	}
	
	if (opt->charcoll)
	{
		QString coll(opt->charcoll);
		setting->setCollection(coll);
	}
	
	setting->setIndex(opt->index);
	
	if (opt->style == FONTMAP_STYLE_BOLDITALIC)
	{
		setting->setBold(true);
		setting->setItalic(true);
	}
	else if (opt->style == FONTMAP_STYLE_ITALIC)
	{
		setting->setBold(false);
		setting->setItalic(true);
	}
	else if (opt->style == FONTMAP_STYLE_BOLD)
	{
		setting->setBold(true);
		setting->setItalic(false);
	}
	else
	{
		setting->setBold(false);
		setting->setItalic(false);
	}
	
	if (opt->flags & FONTMAP_OPT_VERT)
		setting->setWMode(1);
	else
		setting->setWMode(0);
		
	if (opt->flags & FONTMAP_OPT_NOEMBED)
		setting->setNoEmbed(true);
	else
		setting->setNoEmbed(false);
}

void XWTexFontMap::fillInDefaults(const char *tex_name)
{
	if (enc_name && (!strcmp(enc_name, "default") || !strcmp(enc_name, "none")))
	{
		delete [] enc_name;
		enc_name = 0;
	}
	
	if (font_name && (!strcmp(font_name, "default") || !strcmp(font_name, "none")))
	{
		delete [] font_name;
		font_name = 0;
	}
	
	if (!font_name)
		font_name = qstrdup(tex_name);
		
	map_name = qstrdup(tex_name);
	
	if (charmap.sfd_name && enc_name && !opt->charcoll)
	{
		if ((!strcmp(enc_name, "Identity-H") ||
         	!strcmp(enc_name, "Identity-V")) && 
         	(strstr(charmap.sfd_name, "Uni")  ||
          	strstr(charmap.sfd_name, "UBig") ||
          	strstr(charmap.sfd_name, "UBg")  ||
          	strstr(charmap.sfd_name, "UGB")  ||
          	strstr(charmap.sfd_name, "UKS")  ||
          	strstr(charmap.sfd_name, "UJIS")))
        {
        	opt->charcoll = qstrdup("UCS");
        }
	}
}

int XWTexFontMap::parseMapdefDPM(const char *mapdef, char *endptr)
{
	char  *p = (char *) mapdef;
	skip_blank(&p, endptr);
	
	if (p < endptr && *p != '-') 
	{ 
    	enc_name = parse_string_value(&p, endptr);
    	skip_blank(&p, endptr);
  	}
  	
  	if (p < endptr && *p != '-') 
  	{ 
    	font_name = parse_string_value(&p, endptr);
    	skip_blank(&p, endptr);
  	}
  	
  	if (font_name) 
  	{
    	char * tmp = opt->stripOptions(font_name);
    	if (tmp) 
    	{
      		delete [] font_name;
      		font_name = tmp;
    	}
  	}
  	
  	skip_blank(&p, endptr);
  	char * q = 0;
  	long v = 0;
  	while (p + 1 < endptr && *p != '\r' && *p != '\n' && *p == '-')
  	{
  		char mopt = p[1];
  		p += 2; 
  		skip_blank(&p, endptr);
    	switch (mopt)
    	{
    		case  's':
    			{
    				XWLexer lexer(p, endptr);
    				q = lexer.getFloatDecimal();
    				p = (char*)(lexer.getCurPtr());
      				if (!q) 
      				{
        				xwApp->warning(tr("missing a number value for 's' option.\n"));
        				return  -1;
      				}
      				opt->slant = atof(q);
      				delete [] q;
    			}
    			break;
    			
    		case  'e':
    			{
    				XWLexer lexer(p, endptr);
    				q = lexer.getFloatDecimal();
    				p = (char*)(lexer.getCurPtr());
      				if (!q) 
      				{
        				xwApp->warning(tr("missing a number value for 'e' option.\n"));
        				return  -1;
      				}
      				
      				opt->extend = atof(q);
      				if (opt->extend <= 0.0) 
      				{
      					QString msg = QString(tr("invalid value for 'e' option: %1\n")).arg(q);
        				xwApp->warning(msg);
        				delete [] q;
        				return  -1;
      				}
      				delete [] q;
    			}
    			break;
    			
    		case  'b':
    			{
    				XWLexer lexer(p, endptr);
    				q = lexer.getFloatDecimal();
    				p = (char*)(lexer.getCurPtr());
      				if (!q) 
      				{
        				xwApp->warning(tr("missing a number value for 'b' option.\n"));
        				return  -1;
      				}
      				
      				opt->bold = atof(q);
      				if (opt->bold <= 0.0) 
      				{
      					QString msg = QString(tr("invalid value for 'b' option: %1\n")).arg(q);
        				xwApp->warning(msg);
        				delete [] q;
        				return  -1;
      				}
      				delete [] q;
    			}
    			break;
    			
    		case  'r':
    			break;
    			
    		case  'i':
    			q = parse_integer_value(&p, endptr, 10);
      			if (!q) 
      			{
        			xwApp->warning(tr("Missing TTC index number...\n"));
        			return  -1;
      			}
      			opt->index = atoi(q);
      			if (opt->index < 0) 
      			{
      				QString msg = QString(tr("invalid TTC index number: %1\n")).arg(q);
        			xwApp->warning(msg);
        			delete [] q;
        			return  -1;
      			}
      			delete [] q;
    			break;
    			
    		case  'p':
    			q = parse_integer_value(&p, endptr, 0);
      			if (!q) 
      			{
        			xwApp->warning(tr("missing a number for 'p' option.\n"));
        			return  -1;
      			}
      			v = strtol(q, NULL, 0);
      			if (v < 0 || v > 16)
      			{
      				QString msg = QString(tr("invalid value for option 'p': %1\n")).arg(q);
        			xwApp->warning(msg);
        		}
      			else 
        			opt->mapc = v << 16;
      			delete [] q;
    			break;
    			
    		case  'u':
    			q = parse_string_value(&p, endptr);
      			if (q)
        			opt->tounicode = q;
      			else 
      			{
        			xwApp->warning(tr("missing string value for option 'u'.\n"));
        			return  -1;
      			}
    			break;
    			
    		case  'v':
    			q = parse_integer_value(&p, endptr, 10);
      			if (!q) 
      			{
        			xwApp->warning(tr("missing a number for 'v' option.\n"));
        			return  -1;
      			}
      			opt->stemv = strtol(q, NULL, 0);
      			delete [] q;
    			break;
    			
    		case  'm':
    			if (p + 4 <= endptr && p[0] == '<' && p[3] == '>')
    			{
    				p++;
        			q = parse_integer_value(&p, endptr, 16);
        			if (!q) 
        			{
          				xwApp->warning(tr("invalid value for option 'm'.\n"));
          				return  -1;
        			} 
        			else if (p < endptr && *p != '>') 
        			{
        				QString msg = QString(tr("invalid value for option 'm': %1\n")).arg(q);
          				xwApp->warning(msg);
          				delete [] q;
          				return  -1;
        			}
        			v = strtol(q, NULL, 16);
        			opt->mapc = ((v << 8) & 0x0000ff00L);
        			delete [] q; 
        			p++;
    			}
    			else if (p + 4 <= endptr && !memcmp(p, "sfd:", strlen("sfd:")))
    			{
    				p += 4; 
    				skip_blank(&p, endptr);
        			q  = parse_string_value(&p, endptr);
        			if (!q) 
        			{
          				xwApp->warning(tr("missing value for option 'm'.\n"));
          				return  -1;
        			}
        			char * r  = strchr(q, ',');
        			if (!r) 
        			{
        				QString msg = QString(tr("invalid value for option 'm': %1\n")).arg(q);
          				xwApp->warning(msg);
          				delete [] q;
          				return  -1;
        			}
        			*r = 0; 
        			r++; 
        			skip_blank(&r, r + strlen(r));
        			if (*r == '\0') 
        			{
          				QString msg = QString(tr("invalid value for option 'm': %1\n")).arg(q);
          				xwApp->warning(msg);
          				delete [] q;
          				return  -1;
        			}
        			charmap.sfd_name   = qstrdup(q);
        			charmap.subfont_id = qstrdup(r);
        			delete [] q;
    			}
    			else if (p + 4 < endptr && !memcmp(p, "pad:", strlen("pad:")))
    			{
    				p += 4; 
    				skip_blank(&p, endptr);
        			q  = parse_integer_value(&p, endptr, 16);
        			if (!q) 
        			{
          				xwApp->warning(tr("invalid value for option 'm'.\n"));
          				return  -1;
        			} 
        			else if (p < endptr && !isspace(*p)) 
        			{
          				QString msg = QString(tr("invalid value for option 'm': %1\n")).arg(q);
          				xwApp->warning(msg);
          				delete [] q;
          				return  -1;
        			}
        			v = strtol(q, NULL, 16);
        			opt->mapc = ((v << 8) & 0x0000ff00L);
        			delete [] q;
    			}
    			else
    			{
    				xwApp->warning(tr("invalid value for option 'm'.\n"));
        			return  -1;
    			}
    			break;
    			
    		case 'w':
    			if (!enc_name || strcmp(enc_name, "unicode")) 
    			{
        			xwApp->warning(tr("Fontmap option 'w' meaningless for encoding other than \"unicode\".\n"));
        			return  -1;
      			}
      			q  = parse_integer_value(&p, endptr, 10);
      			if (!q) 
      			{
        			xwApp->warning(tr("missing wmode value...\n"));
        			return  -1;
      			}
      			if (atoi(q) == 1)
        			opt->flags |= FONTMAP_OPT_VERT;
      			else if (atoi(q) == 0)
        			opt->flags &= ~FONTMAP_OPT_VERT;
      			else 
      			{
      				QString msg = QString(tr("invalid value for option 'w': %1\n")).arg(q);
        			xwApp->warning(msg);
      			}
      			delete [] q;
    			break;
    			
    		default:
    			{
    				QString msg = QString(tr("unrecognized font map option: '%1'\n")).arg(mopt);
    				xwApp->warning(msg);
      				return  -1;
      			}
      			break;
    	}
    	
    	skip_blank(&p, endptr);
  	}
  	
  	if (p < endptr && *p != '\r' && *p != '\n') 
  	{
  		QString msg = QString(tr("invalid char in fontmap line: %1\n")).arg(*p);
    	xwApp->warning(msg);
    	return  -1;
  	}

  	return  0;
}

int XWTexFontMap::parseMapdefDPS(const char *mapdef, char *endptr)
{
	char *p = (char *)mapdef;	
	skip_blank(&p, endptr);
	
	char * q = 0;
	if (p < endptr) 
	{
    	q = parse_string_value(&p, endptr);
    	if (q) 
    		delete [] q;
    	skip_blank(&p, endptr);
  	} 
  	else 
  	{
    	xwApp->warning(tr("missing a PostScript font name.\n"));
    	return -1;
  	}

  	if (p >= endptr) 
  		return 0;
  		
  	while (p < endptr && *p != '\r' && *p != '\n' && (*p == '<' || *p == '"'))
  	{
  		switch (*p)
  		{
  			case '<':
  				if (++p < endptr && *p == '[') 
  					p++;
  				
  				skip_blank(&p, endptr);
      			if ((q = parse_string_value(&p, endptr))) 
      			{
        			int n = strlen(q);
        			if (n > 4 && strncmp(q+n-4, ".enc", 4) == 0)
          				enc_name = q;
        			else
          				font_name = q;
      			}
      			skip_blank(&p, endptr);
  				break;
  				
  			case '"':
  				if ((q = parse_string_value(&p, endptr)))
  				{
  					char *r = q, *e = q+strlen(q);
  					char * t = 0;
  					skip_blank(&r, e);
  					while (r < e)
  					{
  						XWLexer lexer(r, e);
  						char * s = lexer.getFloatDecimal();
  						r = (char*)(lexer.getCurPtr());
  						if (s)
  						{
  							skip_blank(&r, e);
  							if ((t = parse_string_value(&r, e)))
  							{
  								if (strcmp(t, "SlantFont") == 0)
                					opt->slant = atof(s);
              					else if (strcmp(r, "ExtendFont") == 0)
                					opt->extend = atof(s);
              					delete [] t;
  							}
  							delete [] s;
  						}
  						else if ((s = parse_string_value(&r, e)))
  							delete [] s;
  							
  						skip_blank(&r, e);
  					}
  					
  					delete [] q;
  				}
  				skip_blank(&p, endptr);
  				break;
  				
  			default:
      			{
      				QString msg = QString(tr("found an invalid entry: %1\n")).arg(p);
      				xwApp->warning(msg);
      				return -1;
      			}
      			break;
  		}
  		
  		skip_blank(&p, endptr);
  	}
  	
  	if (p < endptr && *p != '\r' && *p != '\n') 
  	{
  		QString msg = QString(tr("invalid char in fontmap line: %1\n")).arg(*p);
      	xwApp->warning(msg);
    	return -1;
  	}

  	return  0;
}


class XWTexFontMapEntry
{
public:
	XWTexFontMapEntry(const char * keyA, 
	                  int keylenA, 
	                  XWTexFontMap * maprecA);
	                      
	~XWTexFontMapEntry();
	
public:
	char  *key;
  	int    keylen;
  	XWTexFontMap * value;
  	
  	XWTexFontMapEntry * next;
};

XWTexFontMapEntry::XWTexFontMapEntry(const char * keyA, 
	                                 int keylenA, 
	                                 XWTexFontMap * maprecA)
{
	if (keyA && keylenA > 0)
	{
		key = new char[keylenA + 1];
		memcpy(key, keyA, keylenA);
		keylen = keylenA;
	}
	else
	{
		key = 0;
		keylen = 0;
	}
	
	value = maprecA;
	next = 0;
}

XWTexFontMapEntry::~XWTexFontMapEntry()
{
	if (value)
		delete value;
		
	if (key)
		delete [] key;
}

XWTexFontMaps::XWTexFontMaps(QObject * parent)
	:QObject(parent),
	 count(0)
{
	table = (XWTexFontMapEntry**)malloc(MAP_TABLE_SIZE * sizeof(XWTexFontMapEntry*));
	for (int i = 0; i < MAP_TABLE_SIZE; i++)
		table[i] = 0;
		
	itor = 0;
}

XWTexFontMaps::~XWTexFontMaps()
{
	for (int i = 0; i < MAP_TABLE_SIZE; i++)
	{
		XWTexFontMapEntry * cur = table[i];
		while (cur)
		{
			XWTexFontMapEntry * tmp = cur->next;
			delete cur;
			cur = tmp;
		}
	}
	
	free(table);
	
	if (itor)
		delete itor;
		
	XWSubfont::releaseSFD();
}

int XWTexFontMaps::appendRecord(const char *kp, const XWTexFontMap *vp)
{
	if (!kp || !vp || vp->invalid())
	{
		xwApp->warning(tr("invalid fontmap record...\n"));
    	return -1;
	}
	
	char *sfd_name = 0;
	char * fnt_name = chopSFDName(kp, &sfd_name);
	if (fnt_name && sfd_name)
	{
		QString tmp(sfd_name);		
		XWSubfont subfont(tmp);
		if (!subfont.isOK())
		{
			delete [] fnt_name;
			delete [] sfd_name;
			return -1;
		}
		
		int n = 0;
		char **subfont_ids = subfont.getSubfontIds(&n);
		while (n-- > 0)
		{
			char * tfm_name = makeSubfontName(kp, sfd_name, subfont_ids[n]);
      		if (!tfm_name)
        		continue;
        		
        	XWTexFontMap * mrec = lookupTable(tfm_name, strlen(tfm_name));
        	if (!mrec) 
        	{
        		mrec = new XWTexFontMap;
        		if (vp)
        		{
        			if (vp->font_name)
        				mrec->font_name = qstrdup(vp->font_name);
        				
        			if (vp->enc_name)
        				mrec->enc_name = qstrdup(vp->enc_name);
        				
        			if (mrec->opt)
        			{
        				delete mrec->opt;
        				mrec->opt = 0;
        			}
        			
        			if (vp->opt)
        				mrec->opt = new XWTexFontMapOpt(vp->opt);
        		}
        		mrec->map_name = qstrdup(kp);
        		mrec->charmap.sfd_name = qstrdup(sfd_name);
        		mrec->charmap.subfont_id = qstrdup(subfont_ids[n]);
        		insertTable(tfm_name, strlen(tfm_name), mrec);
      		}
      		delete [] tfm_name;
		}
		
		delete [] fnt_name;
    	delete [] sfd_name;
	}
	
	XWTexFontMap * mrec = lookupTable(kp, strlen(kp));
  	if (!mrec) 
  	{
    	mrec = new XWTexFontMap(vp);
    	if (mrec->map_name && !strcmp(kp, mrec->map_name)) 
    	{
      		delete [] mrec->map_name;
      		mrec->map_name = 0;
    	}
    	insertTable(kp, strlen(kp), mrec);
  	}
  	
  	return  0;
}

void XWTexFontMaps::enableNoEmbed()
{
	no_embed = true;
}

QString XWTexFontMaps::getSysFontFile(const char *ps_name,
	                                    const char *fam_name, 
                                      const char *sty_name)
{
	if (fileCount == 0 && FT_Init_FreeType(&ftlib))
	{
		xwApp->error(tr("fail to init freetype lib.\n"));
		return false;
	}
	
	fileCount++;
	XWFontSea fontsea;
	QStringList files =	fontsea.getSystemFiles();
	for (int i = 0; i < files.size(); i++)
	{
		QString f = files[i];
		QByteArray ba = QFile::encodeName(f);
		FT_Face faceA;
    if (FT_New_Face(ftlib, ba.constData(), 0, &faceA))
    	continue;
    	
    if (ps_name && FT_Get_Postscript_Name(faceA))
    {
    	const char * t = FT_Get_Postscript_Name(faceA);
    	if (0 == qstrcmp(ps_name, t))
    	{
    		FT_Done_Face(faceA);
    		return f;
    	}
    }
    	
    if (fam_name && faceA->family_name)
    {
    	if (0 == qstrcmp(fam_name, (const char*)(faceA->family_name)))
    	{
    		if (!sty_name)
    		{
    			FT_Done_Face(faceA);
    			return f;
    		}
    		
    		if (faceA->style_name)
    		{
    			if (0 == qstrcmp(fam_name, (const char*)(faceA->style_name)))
    			{
    				FT_Done_Face(faceA);
    				return f;
    			}
    		}
    	}
    }
    
    FT_Done_Face(faceA);
	}
	
	return QString();
}

int XWTexFontMaps::insertRecord(const char *kp, const XWTexFontMap *vp)
{
	if (!kp || !vp || vp->invalid())
	{
		xwApp->warning(tr("invalid fontmap record...\n"));
    	return -1;
	}
	
	char *sfd_name = 0;
	char * fnt_name = chopSFDName(kp, &sfd_name);
	if (fnt_name && sfd_name)
	{
		QString tmp(sfd_name);		
		XWSubfont subfont(tmp);
		if (!subfont.isOK())
		{
			delete [] fnt_name;
			delete [] sfd_name;
			return -1;
		}
		
		int n = 0;
		char **subfont_ids = subfont.getSubfontIds(&n);
		while (n-- > 0)
		{
			char * tfm_name = makeSubfontName(kp, sfd_name, (const char*)(&subfont_ids[n]));
      		if (!tfm_name)
        		continue;
        		
        	XWTexFontMap * mrec = lookupTable(tfm_name, strlen(tfm_name));
        	if (!mrec) 
        	{
        		mrec = new XWTexFontMap;
        		if (vp)
        		{
        			if (vp->font_name)
        				mrec->font_name = qstrdup(vp->font_name);
        				
        			if (vp->enc_name)
        				mrec->enc_name = qstrdup(vp->enc_name);
        				
        			if (mrec->opt)
        			{
        				delete mrec->opt;
        				mrec->opt = 0;
        			}
        			
        			if (vp->opt)
        				mrec->opt = new XWTexFontMapOpt(vp->opt);
        		}
        		mrec->map_name = qstrdup(kp);
        		mrec->charmap.sfd_name = qstrdup(sfd_name);
        		mrec->charmap.subfont_id = qstrdup(subfont_ids[n]);
        		insertTable(tfm_name, strlen(tfm_name), mrec);
      		}
      		delete [] tfm_name;
		}
		
		delete [] fnt_name;
    	delete [] sfd_name;
	}
	
	XWTexFontMap * mrec = new XWTexFontMap(vp);
    if (mrec->map_name && !strcmp(kp, mrec->map_name)) 
    {
    	delete [] mrec->map_name;
     	mrec->map_name = 0;
    }
    insertTable(kp, strlen(kp), mrec);
    
    return 0;
}

int XWTexFontMaps::loadFile(const char *filename, int mode, bool tosetting)
{
	QString fn = QFile::decodeName(filename);
		
	XWFontSea sea;
	QFile * fp = sea.openMap(fn);
	if (!fp)
		return -1;
		
	int  error = 0;
	char * p = 0;
	char buf[1024];
	long llen = 0;
	long lpos  = 0;
	int format = 0;
	XWTeXFontSetting setting;
	while (!error && (p = readline(buf, 1024, fp)) != 0)
	{
		lpos++;
    	llen = strlen(buf);
    	char * endptr = p + llen;

    	skip_blank(&p, endptr);
    	if (p == endptr)
      		continue;
      		
      	int m = XWTexFontMap::isPDFMMapLine(p);
      	if (format * m < 0)
      		continue;
      	else
      		format += m;
      		
      	XWTexFontMap * mrec = new XWTexFontMap;
      	error = mrec->readLine(p, llen, format);
      	if (error)
      	{
      		QString msg = QString(tr("invalid map record in fontmap line %1 from %2.\n"
      		                          "-- Ignore the current input buffer: %3\n"))
      		                        .arg(lpos).arg(fn).arg(p);
			xwApp->warning(msg);
			delete mrec;
			continue;
      	}
      	
      	if (tosetting)
      		mrec->toSetting(&setting);
      	
      	switch (mode)
      	{
      		case FONTMAP_RMODE_REPLACE:
        		insertRecord(mrec->map_name, mrec);
        		break;
        		
      		case FONTMAP_RMODE_APPEND:
        		appendRecord(mrec->map_name, mrec);
        		break;
        		
      		case FONTMAP_RMODE_REMOVE:
        		removeRecord(mrec->map_name);
        		break;
      	}
      	
      	delete mrec;
	}
	
	fp->close();
	delete fp;
	
	return error;
}

int XWTexFontMaps::loadNativeFont(const char *ps_name,
                                  const char *fam_name, 
                                  const char *sty_name,
                                  int layout_dir)
{
	int error = 0;
	if (ps_name[0] == '[')
		error = loadNativeFontFromPath(ps_name, layout_dir);
	else
	{
		QString f = getSysFontFile(ps_name, fam_name, sty_name);
		if (!f.isEmpty())
		{
			QByteArray ba = QFile::encodeName(f);
			FT_Face face;
    	if (FT_New_Face(ftlib, ba.constData(), 0, &face))
    		error = -1;
    	else
    		insertNativeRecord(ps_name, NULL, 0, face, layout_dir);
		}
		else
			error = -1;
	}
	
	return error;
}

int XWTexFontMaps::loadTTFontsMap(const char *filename, bool tosetting)
{
	QString fn = QFile::decodeName(filename);
		
	XWFontSea sea;
	QFile * fp = sea.openMap(fn);
	if (!fp)
		return -1;
		
	int  error = 0;
	char * p = 0;
	char buf[1024];
	long llen = 0;
	XWTeXFontSetting setting;
	while (!error && (p = readline(buf, 1024, fp)) != 0)
	{
		llen   = strlen(buf);
    	char * endptr = p + llen;
    	skip_blank(&p, endptr);
    	if (p == endptr || p[0] == '%' || p[0] == '#')
      		continue;
      		
      	XWTexFontMap * mrec = new XWTexFontMap;
      	error = mrec->readTTFontsMapLine(p, llen);
      	if (error)
      	{
			delete mrec;
			continue;
      	}
      	
      	if (tosetting)
      		mrec->toSetting(&setting);
      		
      	appendRecord(mrec->map_name, mrec);
      	delete mrec;
	}
	
	fp->close();
	delete fp;
	
	return error;
}

XWTexFontMap * XWTexFontMaps::lookupRecord(const char *tfm_name)
{
	if (!tfm_name)
		return 0;
		
	XWTexFontMap * mrec = lookupTable(tfm_name, strlen(tfm_name));
	if (!mrec)
	{
		mrec = new XWTexFontMap;
		mrec->readSetting(tfm_name);		
		appendRecord(mrec->map_name, mrec);
		delete mrec;
		mrec = lookupTable(tfm_name, strlen(tfm_name));
	}
	
	return mrec;
}

int XWTexFontMaps::removeRecord(const char *kp)
{
	if (!kp)
    	return  -1;
    
	char * sfd_name = 0;
	char * fnt_name = chopSFDName(kp, &sfd_name);
  	if (fnt_name && sfd_name)
  	{
  		QString tmp(sfd_name);
		
		XWSubfont subfont(tmp);
		if (!subfont.isOK())
		{
			delete [] fnt_name;
			delete [] sfd_name;
			return -1;
		}
		
		int n = 0;
		char **subfont_ids = subfont.getSubfontIds(&n);
		while (n-- > 0)
		{
			char * tfm_name = makeSubfontName(kp, sfd_name, subfont_ids[n]);
      		if (!tfm_name)
        		continue;
        		
        	removeTable(tfm_name, strlen(tfm_name));
      		delete [] tfm_name;
		}
		
		delete [] fnt_name;
    	delete [] sfd_name;
  	}
  	
  	removeTable(kp, strlen(kp));
  	
  	return  0;
}

void XWTexFontMaps::appendTable(const char *key, int keylen, XWTexFontMap *value)
{
	uint hkey = getHash(key, keylen);
  	XWTexFontMapEntry * hent = table[hkey];
  	if (!hent) 
  	{
    	hent = new XWTexFontMapEntry(key, keylen, value);
    	table[hkey] = hent;
  	} 
  	else 
  	{
  		XWTexFontMapEntry * last = 0;
    	while (hent) 
    	{
      		last = hent;
      		hent = hent->next;
    	}
    	
    	hent = new XWTexFontMapEntry(key, keylen, value);
    	last->next = hent;
  	}

  	count++;
}

uint XWTexFontMaps::getHash(const char *key, int keylen)
{
	unsigned int hkey = 0;

  	for (int i = 0; i < keylen; i++) 
    	hkey = (hkey << 5) + hkey + ((char *)key)[i];

  	return (hkey % MAP_TABLE_SIZE);
}

void XWTexFontMaps::insertNativeRecord(const char *name, 
	                                     const char *path, 
	                                     int index, 
	                                     FT_Face face, 
	                                     int layout_dir)
{
	XWTexFontMap * mrec = new XWTexFontMap(name, path, index, face, layout_dir);
	insertRecord(mrec->map_name, mrec);
	delete mrec;
}

void XWTexFontMaps::insertTable(const char *key, int keylen, XWTexFontMap *value)
{
	if (!key || keylen < 1)
		return ;
		
	uint hkey = getHash(key, keylen);
  	XWTexFontMapEntry * hent = table[hkey];
  	XWTexFontMapEntry * prev = 0;
  	while (hent) 
  	{
    	if (hent->keylen == keylen && !memcmp(hent->key, key, keylen)) 
      		break;
    	prev = hent;
    	hent = hent->next;
  	}
  	
  	if (hent)
  	{
  		if (hent->value)
  			delete hent->value;
  		hent->value = value;
  	}
  	else
  	{
  		hent = new XWTexFontMapEntry((char*)key, keylen, value);
  		if (prev) 
      		prev->next = hent;
    	else 
      		table[hkey] = hent;
    	count++;
  	}
}

bool XWTexFontMaps::loadNativeFontFromPath(const char *ps_name, int layout_dir)
{
	if (fileCount == 0 && FT_Init_FreeType(&ftlib))
	{
		xwApp->error(tr("fail to init freetype lib.\n"));
		return false;
	}
	
	fileCount++;
	
	char *filename = new char[strlen(ps_name)];
	char *q = filename;
  int  index = 0;
  const char * p = 0;
  FT_Face face = NULL;
  
#ifdef Q_OS_WIN
   for (p = ps_name + 1; *p && *p != ']'; ++p)
   {
   		if (*p == ':')
   		{
   			if (p == (ps_name + 2) && isalpha(*(p-1)) && (*(p+1) == '/' || *(p+1) == '\\'))
   				*q++ = *p;
   			else
   				break;
   		}
   		else
   			*q++ = *p;
   }
#else
   for (p = ps_name + 1; *p && *p != ':' && *p != ']'; ++p)
    	*q++ = *p;
#endif
	
	*q = 0;
  if (*p == ':')
  {
  	++p;
    while (*p && *p != ']')
      index = index * 10 + *p++ - '0';
  }
  
  XWFontSea fontsea;
  QString ftname(filename);
  QString fullname = fontsea.findFile(ftname);
  if (fullname.isEmpty())
  {
     QString msg = QString(tr("cannot find font file '%1'.\n")).arg(ftname);
     xwApp->error(msg);
     delete [] filename;
     return false;
  }
  
  if (FT_New_Face(ftlib, q, index, &face))
  {
  	delete [] filename;
  	QString msg = QString(tr("fail to load font file '%1'.\n")).arg(fullname);
    xwApp->error(msg);
    return false;
  }
  
  insertNativeRecord(ps_name, filename, index, face, layout_dir);
  delete [] filename;
  
  return true;
}

XWTexFontMapEntry * XWTexFontMaps::lookup(const char *key, int keylen)
{
	uint hkey = getHash(key, keylen);
	XWTexFontMapEntry * hent = table[hkey];
	while (hent) 
	{
    	if (hent->keylen == keylen && !memcmp(hent->key, key, keylen)) 
      		return hent;
      		
    	hent = hent->next;
  	}

  	return 0;
}

XWTexFontMap * XWTexFontMaps::lookupTable(const char *key, int keylen)
{
	XWTexFontMapEntry * hent = lookup(key, keylen);
	if (hent)
		return hent->value;

  	return 0;
}

char * XWTexFontMaps::makeSubfontName(const char *map_name, 
	                                  const char *sfd_name, 
	                                  const char *sub_id)
{
	const char * p = strchr(map_name, '@');
  	if (!p || p == map_name)
    	return  NULL;
    	
  	int m = (int) (p - map_name);
  	const char * q = strchr(p + 1, '@');
  	if (!q || q == p + 1)
    	return  0;
  	int n = (int) (q - p) + 1; /* including two '@' */
  	if (strlen(sfd_name) != n - 2 || memcmp(p + 1, sfd_name, n - 2))
    	return  0;
  	char * tfm_name = new char[strlen(map_name) - n + strlen(sub_id) + 1];
  	memcpy(tfm_name, map_name, m);
  	tfm_name[m] = '\0';
  	strcat(tfm_name, sub_id);
  	if (q[1])
    	strcat(tfm_name, q + 1);

  	return  tfm_name;
}

void XWTexFontMaps::removeTable(const char *key, int keylen)
{
	uint hkey = getHash(key, keylen);
  	XWTexFontMapEntry * hent = table[hkey];
  	XWTexFontMapEntry * prev = 0;
  	while (hent) 
  	{
    	if (hent->keylen == keylen && !memcmp(hent->key, key, keylen)) 
      		break;
      		
    	prev = hent;
    	hent = hent->next;
  	}
  	
  	if (hent) 
  	{
    	if (prev) 
      		prev->next = hent->next;
    	else 
      		table[hkey] = hent->next;
    	
    	delete hent;
    	count--;
    	return ;
  	}
}

