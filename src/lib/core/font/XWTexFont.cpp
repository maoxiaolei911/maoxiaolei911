/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "XWNumberUtil.h"
#include "FontEncodingTables.h"
#include "BuiltinFontTables.h"
#include "XWFontFileCFF.h"
#include "XWCFFType.h"
#include "XWT1Type.h"
#include "XWFontFileSFNT.h"
#include "XWSFNTType.h"
#include "XWFontFileType1.h"
#include "XWFontFileFT.h"
#include "XWFontFilePK.h"
#include "XWTFMFile.h"
#include "XWTexFontMap.h"
#include "XWAdobeGlyphList.h"
#include "XWFontCache.h"
#include "XWTexFont.h"

static struct
{
  const char *name;
  int   must_exist;
} required_table[] = {
  {"OS/2", 0}, {"head", 1}, {"hhea", 1}, {"loca", 1}, {"maxp", 1},
  {"name", 1}, {"glyf", 1}, {"hmtx", 1}, {"fpgm", 0}, {"cvt ", 0},
  {"prep", 0}, {"cmap", 1}, {NULL, 0}
};


XWTexFont::XWTexFont()
	:ident(0),
	 subtype(0),
	 map_name(0),
	 encoding_id(-1),
	 index(0),
	 fontname(0),
	 usedchars(0),
	 flags(0),
	 point_size(0),
	 design_size(0),
	 t3Type(0)
{
	ft_face = 0;
	ft_to_gid = 0;
	memset(uniqueID, 0, 7);
	
	dict.Type = 0;
	dict.Subtype = 0;
	dict.Name = 0;
	dict.BaseFont = 0;
	dict.FirstChar = 0;
	dict.LastChar = 0;
	dict.Widths = 0;
	
	desc.Type = 0;
	desc.FontName = 0;
	desc.Flags = 0;
	for (int i = 0; i < 4; i++)
	{
		dict.FontBBox[i] = 0.0;
		desc.FontBBox[i] = 0.0;
	}
	desc.ItalicAngle = 0.0;
	desc.Ascent = 0.0;
	desc.Descent = 0.0;
	desc.CapHeight = 0.0;
	desc.XHeight = 0.0;
	desc.StemV = 0.0;
	desc.AvgWidth = 0.0;
	locking = false;
}

XWTexFont::XWTexFont(const char * identA,
	                 const char * map_nameA,
	                 int    encoding_idA,
	                 int    indexA,
	                 double point_sizeA)
{
	ident = qstrdup(identA);
	subtype = -1;
	map_name = qstrdup(map_nameA);
	encoding_id = encoding_idA;
	index = indexA;
	fontname = 0;
	usedchars = 0;
	flags = 0;
	point_size = point_sizeA;
	design_size = 0;
	t3Type = 0;
	loaded = false;
	ft_face = 0;
	ft_to_gid = 0;
	memset(uniqueID, 0, 7);
	
	dict.Type = 0;
	dict.Subtype = 0;
	dict.Name = 0;
	dict.BaseFont = 0;
	dict.FirstChar = 0;
	dict.LastChar = 0;
	dict.Widths = 0;
	
	desc.Type = 0;
	desc.FontName = 0;
	desc.Flags = 0;
	for (int i = 0; i < 4; i++)
	{
		dict.FontBBox[i] = 0.0;
		desc.FontBBox[i] = 0.0;
	}
	desc.ItalicAngle = 0.0;
	desc.Ascent = 0.0;
	desc.Descent = 0.0;
	desc.CapHeight = 0.0;
	desc.XHeight = 0.0;
	desc.StemV = 0.0;
	desc.AvgWidth = 0.0;
	locking = false;
}

XWTexFont::~XWTexFont()
{
	if (ident)
		delete [] ident;
		
	if (map_name)
		delete [] map_name;
		
	if (fontname)
		delete [] fontname;
		
	if (usedchars)
		delete [] usedchars;
		
	if (dict.Type)
		delete [] dict.Type;
		
	if (dict.Subtype)
		delete [] dict.Subtype;
		
	if (dict.Name)
		delete [] dict.Name;
		
	if (dict.BaseFont)
		delete [] dict.BaseFont;
		
	if (dict.Widths)
		delete [] dict.Widths;
		
	if (desc.Type)
		delete [] desc.Type;
		
	if (desc.FontName)
		delete [] desc.FontName;
}

char * XWTexFont::getUsedChars()
{
	if (!usedchars)
	{
		usedchars = new char[256];
    memset(usedchars, 0, 256 * sizeof(char));
	}
	return usedchars;
}

int XWTexFont::getParam(int param_type)
{
	double param = 0.0;
	switch (param_type) 
	{
  		case PDF_FONT_PARAM_DESIGN_SIZE:
    		param = design_size;
    		break;
    		
  		case PDF_FONT_PARAM_POINT_SIZE:
    		param = point_size;
    		break;
  		default:
    		break;
  	}
  	return param;
}

char * XWTexFont::getUniqueTag()
{
	if (uniqueID[0] == '\0') 
    	makeUniqueTag(uniqueID);
  	return uniqueID;
}

XWFontFileFT * XWTexFont::loadFT(int hDPI, int vDPI)
{
	locking = true;
	if (!dict.Widths)
	{
		dict.Widths = new double[256];			
		memset(dict.Widths, 0, 256 * sizeof(double));	
	}
	
	XWFontFileFT * ret = XWFontFileFT::loadType3(map_name, 
		                                         hDPI, vDPI,
		                                         point_size, usedchars, 
		                                         dict.FontBBox, dict.Widths,
		                                         dict.FontMatrix, &(dict.FirstChar),
		                                         &(dict.LastChar));
	if (ret)
		loaded = true;
	locking = false;
	return ret;
}

XWFontFilePK * XWTexFont::loadPK(int base_dpi)
{
	locking = true;
	if (!dict.Widths)
	{
		dict.Widths = new double[256];				
		memset(dict.Widths, 0, 256 * sizeof(double));	
	}
		
	XWFontFilePK * ret = XWFontFilePK::load(ident, base_dpi, 
		                                    point_size, usedchars, 
		                                    dict.FontBBox, dict.Widths,
		                                    dict.FontMatrix, &(dict.FirstChar),
		                                    &(dict.LastChar));
	if (ret)
		loaded = true;
		
	locking = false;
	return ret;
}

bool XWTexFont::loadTrueType(uchar ** streamdata, 
	                         long * streamlen)
{
	locking = true;
	if (!dict.Widths)
	{
		dict.Widths = new double[256];		
		memset(dict.Widths, 0, 256 * sizeof(double));	
	}
		
	XWFontFileSFNT *sfont = 0;
	if (ft_face)
		sfont = XWFontFileSFNT::load(ft_face, SFNT_TYPE_TTC | SFNT_TYPE_TRUETYPE, false);
	else
		sfont = XWFontFileSFNT::loadTrueType(ident, index, false);
	if (!sfont)
	{
		locking = false;
		return false;
	}
		
	char ** enc_vec = 0;
	int error = 0;
	if (encoding_id < 0)
    	error = doBuiltinEncoding(sfont);
    else 
    {
    	XWFontCache cache(true);
    	enc_vec  = cache.getEncoding(encoding_id);
    	error = doCustomEncoding(enc_vec, sfont);
  	}
  	
  	if (error)
  	{
  		delete sfont;
  		locking = false;
  		return false;
  	}
  	
  	int embedding = getFlag(PDF_FONT_FLAG_NOEMBED) ? 0 : 1;
  	if (!embedding) 
  	{
    	delete sfont;
    	locking = false;
  		return true;
  	}
  	
  	for (int i = 0; required_table[i].name != NULL; i++) 
  	{
    	if (sfont->requireTable(required_table[i].name, required_table[i].must_exist) < 0) 
    	{
      		delete sfont;
      		locking = false;
      		return false;
    	}
  	}
  	
  	sfont->createFontFileStream(streamdata, streamlen);
  	
  	loaded = true;
  	delete sfont;
  	locking = false;
  	return true;
}

bool  XWTexFont::loadType1(char ** enc_vec, 
	                         char * fullname, 
	                         uchar ** streamdata, 
	                         long * streamlen)
{
	locking = true;
	if (!dict.Widths)
	{
		dict.Widths = new double[256];		
		memset(dict.Widths, 0, 256 * sizeof(double));	
	}
	
	XWFontFileCFF * cffont = XWFontFileCFF::loadType1(ident, enc_vec, 0);
	if (!cffont)
	{
		locking = false;
		return false;
	}

    char * uniqueTag = getUniqueTag();
  	sprintf(fullname, "%6s+%s", uniqueTag, fontname);
  	cffont->setName(fullname);
  	
  	getFontAttr(cffont);
  	
  	double defaultwidth = 0.0;
  	if (cffont->privDictKnow(0, "defaultWidthX"))
  		defaultwidth = cffont->privDictGet(0, "defaultWidthX", 0);
  		
  	double nominalwidth = 0.0;
  	if (cffont->privDictKnow(0, "nominalWidthX"))
  		nominalwidth = cffont->privDictGet(0, "nominalWidthX", 0);
  		
  	ushort num_glyphs =  1;
  	XWCFFCharsets * charset = 0;
#define MAX_GLYPHS 1024
  	ushort * GIDMap = (ushort*)malloc(MAX_GLYPHS * sizeof(ushort));
  	{
  		XWCFFIndex * cstrings = cffont->getCStrings();
  		XWCFFEncoding * encoding = new XWCFFEncoding;
  		cffont->setEncoding(encoding);
  		encoding->format      = 1;
    	encoding->num_entries = 0;
    	encoding->data.range1 = (CFFRange1*)malloc(256 * sizeof(CFFRange1));
    	encoding->num_supps   = 0;
    	encoding->supp        = (CFFMap*)malloc(256 * sizeof(CFFMap));
    	
    	charset = new XWCFFCharsets;
    	charset->format      = 0;
    	charset->num_entries = 0;
    	charset->data.glyphs = (ushort*)malloc(MAX_GLYPHS * sizeof(ushort));
    	
    	long gid = cffont->lookupGlyph(".notdef");
    	GIDMap[0] = (ushort)gid;
    	
    	int prev = -2;
    	for (int code = 0; code <= 0xff; code++)
    	{
    		char * glyph = enc_vec[code];
    		if (!usedchars[code])
				continue;
				
			if (!glyph || !strcmp(glyph, ".notdef"))
			{
				usedchars[code] = 0;
				continue;
			}
			
			gid = cffont->lookupGlyph(glyph);
			if (gid < 1 || gid >= cstrings->count)
			{
				usedchars[code] = 0;
				continue;
			}
			
			int duplicate = 0;
			for (; duplicate < code; duplicate++) 
			{
				if (usedchars[duplicate] && enc_vec[duplicate] && !strcmp(enc_vec[duplicate], glyph))
	  				break;
      		}
      		
      		ushort sid = cffont->addString(glyph, 1);
      		if (duplicate < code)
      		{
      			encoding->supp[encoding->num_supps].code  = duplicate;
				encoding->supp[encoding->num_supps].glyph = sid;
				encoding->num_supps += 1;
      		}
      		else
      		{
      			GIDMap[num_glyphs] = (ushort) gid;
				charset->data.glyphs[charset->num_entries] = sid;
				charset->num_entries += 1;
				if (code != prev + 1) 
				{
	  				encoding->num_entries += 1;
	  				encoding->data.range1[encoding->num_entries-1].first  = code;
	  				encoding->data.range1[encoding->num_entries-1].n_left = 0;
				} 
				else 
	  				encoding->data.range1[encoding->num_entries-1].n_left += 1;
	  				
				prev = code;
				num_glyphs++;
      		}
    	}
    	
    	if (encoding->num_supps > 0) 
    	{
      		encoding->format |= 0x80;
    	} 
    	else 
    	{
      		free(encoding->supp);
      		encoding->supp = 0;
    	}
  	}
  	
  	long offset = 0L;
  	{
  		XWCFFIndex * cstrings = cffont->getCStrings();
  		XWCFFIndex * cstring = new XWCFFIndex((ushort)(cstrings->count));
    	cstring->data      = 0;
    	cstring->offset[0] = 1;
    	
    	long dstlen_max = 0L;
    	XWT1GInfo gm;
    	XWCFFIndex * subrs = cffont->getSubrs(0);
    	int have_seac = 0;
    	for (ushort gid = 0; gid < num_glyphs; gid++)
    	{
    		if (offset + CS_STR_LEN_MAX >= dstlen_max)
    		{
    			dstlen_max += CS_STR_LEN_MAX * 2;
				cstring->data = (uchar*)realloc(cstring->data, dstlen_max * sizeof(uchar));
    		}
    		
    		ushort gid_orig = GIDMap[gid];

      		uchar * dstptr   = cstring->data + cstring->offset[gid] - 1;
      		uchar * srcptr   = cstrings->data + cstrings->offset[gid_orig] - 1;
      		long    srclen   = cstrings->offset[gid_orig + 1] - cstrings->offset[gid_orig];

      		offset  += gm.convertCharString(dstptr, CS_STR_LEN_MAX,
					   						srcptr, srclen, subrs, 
					   						defaultwidth, nominalwidth);
      		cstring->offset[gid + 1] = offset + 1;
      		if (gm.use_seac)
      		{
      			char * achar_name = standardEncoding[gm.seac.achar];
				ushort achar_gid  = cffont->lookupGlyph(achar_name);
				char * bchar_name = standardEncoding[gm.seac.bchar];
				ushort bchar_gid  = cffont->lookupGlyph(bchar_name);
				if (achar_gid < 0)
					continue;
					
				if (bchar_gid < 0)
					continue;
					
				int i = 0;
				for (; i < num_glyphs; i++) 
				{
	  				if (GIDMap[i] == achar_gid)
	    				break;
				}
				
				if (i == num_glyphs)
				{
					GIDMap[num_glyphs++] = achar_gid;
	  				charset->data.glyphs[charset->num_entries] = cffont->getSID(achar_name);
	  				charset->num_entries += 1;
				}
				
				for (i = 0; i < num_glyphs; i++) 
				{
	  				if (GIDMap[i] == bchar_gid)
	    			break;
				}
				
				if (i == num_glyphs)
				{
					GIDMap[num_glyphs++] = bchar_gid;
	  				charset->data.glyphs[charset->num_entries] = cffont->getSID(bchar_name);
	  				charset->num_entries += 1;
				}
				
				have_seac = 1;
      		}
      		dict.Widths[gid] = gm.wx;
    	}
    	
    	cstring->count = num_glyphs;
    	
    	cffont->setSubrs(0, 0);

    	cffont->setCString(cstring);

    	cffont->setCharsets(charset);
  	}
  	
  	cffont->topDictUpdate();
  	cffont->privDictUpdate(0);
  	cffont->updateString();
  	addMetrics(cffont, enc_vec,num_glyphs);
  	offset = writeFontFile(cffont, streamdata, streamlen);
  	delete cffont;
  	
  	free(GIDMap);
  	loaded = true;
  	locking = false;
  	return true;
}

bool XWTexFont::loadType1C(char ** enc_vec, 
	                         char * fullname, 
	                         uchar ** streamdata, 
	                         long * streamlen)
{
	locking = true;
	if (!dict.Widths)
	{
		dict.Widths = new double[256];		
		memset(dict.Widths, 0, 256 * sizeof(double));	
	}
		
	XWFontFileSFNT * sfont = 0;
	if (ft_face)
		sfont = XWFontFileSFNT::load(ft_face, SFNT_TYPE_POSTSCRIPT, false);
	else
		sfont = XWFontFileSFNT::loadType1C(ident, false);
	if (!sfont)
	{
		locking = false;
		return false;
	}
		
	XWFontFileCFF * cffont = sfont->makeType1C(0);
	if (!cffont)
	{
		delete sfont;
		locking = false;
		return false;
	}
	
	cffont->readCharsets();
	if (encoding_id < 0)
    	cffont->readEncoding();
    	
    cffont->readPrivate();
    cffont->readSubrs();
    
    cffont->newString();
    XWCFFCharsets * charset = new XWCFFCharsets;
  	charset->format      = 0;
  	charset->num_entries = 0;
  	charset->data.glyphs = (ushort*)malloc(256 * sizeof(ushort));
  	
  	XWCFFEncoding * encoding = new XWCFFEncoding;
  	encoding->format      = 1;
  	encoding->num_entries = 0;
  	encoding->data.range1 = (CFFRange1*)malloc(255 * sizeof(CFFRange1));
  	encoding->num_supps   = 0;
  	encoding->supp        = (CFFMap*)malloc(255 * sizeof(CFFMap));
  	
  	long offset = (long) cffont->topDictGet("CharStrings", 0);
  	cffont->seek(offset);
  	XWCFFIndex * cs_idx = cffont->getIndexHeader();
  	offset   = cffont->tellPosition();
  	ushort cs_count = cs_idx->count;
  	if (cs_count < 2) 
  	{
    	delete encoding;
    	delete charset;
    	delete cffont;
    	delete sfont;
    	locking = false;
    	return false;
  	}
  	
  	XWCFFIndex * charstrings = new XWCFFIndex((ushort)257);
  	ulong max_len           = 2 * CS_STR_LEN_MAX;
  	charstrings->data = (uchar*)malloc(max_len * sizeof(uchar));
  	ulong charstring_len    = 0;
    
    if (cffont->privDictKnow(0, "StdVW"))
    	desc.StemV = cffont->privDictGet(0, "StdVW", 0);
    	
    double default_width = CFF_DEFAULTWIDTHX_DEFAULT;
    if (cffont->privDictKnow(0, "defaultWidthX"))
    	default_width = cffont->privDictGet(0, "defaultWidthX", 0);
    	
    double nominal_width = CFF_NOMINALWIDTHX_DEFAULT;
    if (cffont->privDictKnow(0, "nominalWidthX"))
    	nominal_width = cffont->privDictGet(0, "nominalWidthX", 0);
    	
    uchar * data = (uchar*)malloc(CS_STR_LEN_MAX * sizeof(uchar));
    ulong size = cs_idx->offset[1] - cs_idx->offset[0];
  	if (size > CS_STR_LEN_MAX) 
  	{
    	delete encoding;
    	delete charset;
    	delete cffont;
    	delete sfont;
    	free(data);
    	delete charstrings;
    	locking = false;
    	return false;
  	}
  	
  	charstrings->offset[0] = charstring_len + 1;
  	sfont->seekAbsolute(offset + cs_idx->offset[0] - 1);
  	sfont->read((char*)data, size);
  	XWCSGInfo ginfo;
  	XWCFFIndex * gsubr = cffont->getGsubr();
  	XWCFFIndex * subrs = cffont->getSubrs(0);
  	charstring_len += ginfo.copyCharString(charstrings->data + charstring_len,
				       					   max_len - charstring_len,
				       					   data, size,
				       					   gsubr, subrs,
				       					   default_width, nominal_width);
  	double notdef_width = ginfo.wx;
  	
  	XWFontCache cache(true);
  	if (encoding_id < 0)
  	{
  		for (ushort code = 0; code < 256; code++)
  		{
  			if (usedchars[code])
  			{
  				ushort gid = cffont->lookupEncoding(code);
					enc_vec[code] = cffont->getString(cffont->lookupChartsetsInverse(gid));
  			}
  		}
  	}
  	
  	long num_glyphs = 1;
  	for (ushort code = 0; code < 256; code++)
  	{
  		dict.Widths[code] = notdef_width;
  		if (!usedchars[code] || !enc_vec[code] || !strcmp(enc_vec[code], ".notdef"))
      		continue;
      		
      	ushort sid_orig = cffont->getSID(enc_vec[code]);
    	ushort sid      = sid_orig < CFF_STDSTR_MAX ? sid_orig : cffont->addString(enc_vec[code], 0);
    		
    	int j = 0;
    	for (; j < charset->num_entries; j++) 
    	{
      		if (sid == charset->data.glyphs[j]) 
      		{
				/* Already have this glyph. */
				encoding->supp[encoding->num_supps].code  = code;
				encoding->supp[encoding->num_supps].glyph = sid;
				usedchars[code] = 0; /* Used but multiply-encoded. */
				encoding->num_supps += 1;
				break;
      		}
    	}
    	
    	if (j < charset->num_entries)
    		continue;
    		
    	ushort gid = cffont->lookupChartsets(sid_orig);
    	if (gid == 0)
    	{
    		usedchars[code] = 0;
    		continue;
    	}
    	
    	size = cs_idx->offset[gid+1] - cs_idx->offset[gid];
    	if (size > CS_STR_LEN_MAX) 
    	{
      		delete encoding;
    		delete charset;
    		delete cffont;
    		delete sfont;
    		delete cs_idx;
    		free(data);
    		delete charstrings;
    		if (encoding_id < 0 && enc_vec)
    		{
    			for (int i = 0; i < 256; i++)
    				delete [] enc_vec[i];
    				
    			free(enc_vec);
    		}
    		locking = false;
    		return false;
    	}

    	if (charstring_len + CS_STR_LEN_MAX >= max_len) 
    	{
      		max_len = charstring_len + 2 * CS_STR_LEN_MAX;
      		charstrings->data = (uchar*)realloc(charstrings->data, max_len * sizeof(uchar));
    	}
    	charstrings->offset[num_glyphs] = charstring_len + 1;
    	cffont->seekAbsolute(offset + cs_idx->offset[gid] - 1);
    	sfont->read((char*)data, size);
    	charstring_len += ginfo.copyCharString(charstrings->data + charstring_len,
					 						   max_len - charstring_len,
					 						   data, size,
					 						   gsubr, subrs,
					 						   default_width, nominal_width);
    	dict.Widths[code] = ginfo.wx;
    	charset->data.glyphs[charset->num_entries] = sid;
    	charset->num_entries  += 1;
    	num_glyphs++;
  	}
  	
  	free(data);
  	if (encoding->num_supps > 0)
    	encoding->format |= 0x80;
    else
    {
    	free(encoding->supp);
    	encoding->supp = 0;
    }
    
    for (ushort code = 0; code < 256; code++)
    {
    	if (!usedchars[code] || !enc_vec[code]   || !strcmp(enc_vec[code], ".notdef"))
      		continue;
    	encoding->data.range1[encoding->num_entries].first  = code;
    	encoding->data.range1[encoding->num_entries].n_left = 0;
    	code++;
    	while (code < 256 && usedchars[code] && enc_vec[code] && strcmp(enc_vec[code], ".notdef")) 
    	{
      		encoding->data.range1[encoding->num_entries].n_left += 1;
      		code++;
    	}
    	encoding->num_entries += 1;
    }
      	
  	delete cs_idx;
  	
  	charstrings->offset[num_glyphs] = charstring_len + 1;
  	charstrings->count = num_glyphs;
  	charstring_len     = charstrings->size();
  	cffont->setNumGlyphs(num_glyphs);
  	cffont->setCharsets(charset);
  	cffont->setEncoding(encoding);
  	cffont->setGsub(new XWCFFIndex((ushort)0));
  	cffont->setSubrs(0, 0);
  	cffont->setFlag(FONTTYPE_FONT);
  	cffont->topDictUpdate();
  	cffont->privDictUpdate(0);
  	cffont->updateString();
  	cffont->topDictRemove("UniqueID");
  	cffont->topDictRemove("XUID");
  	
  	if (!cffont->topDictKnow("Encoding"))
    	cffont->topDictAdd("Encoding", 1);
    	
    XWCFFIndex * topdict = new XWCFFIndex((ushort)1);
    char * buf[1024];
    topdict->offset[1] = cffont->topDictPack((uchar *)buf, 1024) + 1;
    
    cffont->privDictRemove(0, "Subrs");
    long private_size = cffont->privDictPack(0, (uchar *)buf, 1024);
    long stream_data_len = 4;
    char * uniqueTag = getUniqueTag();
  	sprintf(fullname, "%6s+%s", uniqueTag, fontname);
  	stream_data_len += cffont->setName(fullname);
  	
  	stream_data_len += topdict->size();
  	XWCFFIndex * string = cffont->getStrings();
  	if (string)
  		stream_data_len += string->size();
  	gsubr = cffont->getGsubr();
  	if (gsubr)
  		stream_data_len += gsubr->size();
  		
  	stream_data_len += 2 + (encoding->num_entries)*2 + 1 + (encoding->num_supps)*3;
  	stream_data_len += 1 + (charset->num_entries)*2;
  	stream_data_len += charstring_len;
  	stream_data_len += private_size;
  	uchar * stream_data_ptr = (uchar*)malloc(stream_data_len * sizeof(uchar));
  	offset = 0;
  	offset += cffont->putHeader(stream_data_ptr + offset, stream_data_len - offset);
  	offset += cffont->packName(stream_data_ptr + offset, stream_data_len - offset);
  	long topdict_offset = offset;
  	offset += topdict->size();
  	offset += cffont->packString(stream_data_ptr + offset, stream_data_len - offset);
  	offset += cffont->packGsubr(stream_data_ptr + offset, stream_data_len - offset);
  	cffont->topDictSet("Encoding", 0, offset);
  	offset += cffont->packEncoding(stream_data_ptr + offset, stream_data_len - offset);
  	cffont->topDictSet("charset", 0, offset);
  	offset += cffont->packCharsets(stream_data_ptr + offset, stream_data_len - offset);
  	cffont->topDictSet("CharStrings", 0, offset);
  	offset += charstrings->pack(stream_data_ptr + offset, charstring_len);
  	delete charstrings;
  	
  	cffont->topDictSet("Private", 1, offset);
  	if (private_size > 0)
    	private_size = cffont->privDictPack(0, stream_data_ptr + offset, private_size);
  	cffont->topDictSet("Private", 0, private_size);
  	offset += private_size;
  	
  	topdict->data = (uchar*)malloc((topdict->offset[1] - 1) * sizeof(uchar));
  	cffont->topDictPack(topdict->data, topdict->offset[1] - 1);
  	topdict->pack(stream_data_ptr + topdict_offset, topdict->size());
  	delete topdict;
  	
  	addSimpleMetrics(cffont, num_glyphs);
  	delete cffont;
  	delete sfont;
  	
  	if (streamdata)
  		*streamdata = stream_data_ptr;
  	else
  		free(stream_data_ptr);
  		
  	if (streamlen)
  		*streamlen = offset;
  		
  	loaded = true;
  	locking = false;
  	return true;
}

void XWTexFont::makeUniqueTag(char *tag)
{
	static char first = 1;

  	if (first) 
  	{
    	srand(time(NULL));
    	first = 0;
  	}

  	for (int i = 0; i < 6; i++) 
  	{
    	char ch = rand() % 26;
    	tag[i] = ch + 'A';
  	}
  	tag[6] = '\0';
}

int XWTexFont::openFT()
{
	locking = true;
	XWFontFileFT * ft = XWFontFileFT::loadTexFont(map_name);
	if (!ft)
	{
		locking = false;
		return -1;
	}
		
	setFontName(ident);
	if (encoding_id >= 0)
	{
		XWFontCache cache(true);
		cache.encodingUsedByType3(encoding_id);
	}
	
	subtype = PDF_FONT_FONTTYPE_TYPE3;
	t3Type = PDF_FONT_TYPE3_FT;
	if (!dict.Type)
		dict.Type = qstrdup("Font");
		
	if (!dict.Subtype)
		dict.Subtype = qstrdup("Type3");
	locking = false;
	return 0;
}

int XWTexFont::openPK(int base_dpi)
{
	locking = true;
	if (!XWFontFilePK::open(ident, base_dpi, getParam(PDF_FONT_PARAM_POINT_SIZE)))
	{
		locking = false;
		return -1;
	}
		
	setFontName(ident);
	if (encoding_id >= 0)
	{
		XWFontCache cache(true);
		cache.encodingUsedByType3(encoding_id);
	}
	
	subtype = PDF_FONT_FONTTYPE_TYPE3;
	t3Type = PDF_FONT_TYPE3_PK;
	if (!dict.Type)
		dict.Type = qstrdup("Font");
		
	if (!dict.Subtype)
		dict.Subtype = qstrdup("Type3");
	locking = false;
	return 0;
}

int XWTexFont::openTrueType()
{
	locking = true;
	XWFontFileSFNT * sfont = 0;
	if (ft_face)
		sfont = XWFontFileSFNT::load(ft_face, SFNT_TYPE_TTC | SFNT_TYPE_TRUETYPE, false);
	else
		sfont = XWFontFileSFNT::loadTrueType(ident, index, false);
			
	if (!sfont)
	{
		locking = false;
		return -1;
	}
		
	desc.Flags = 0;
	XWFontCache cache(true);
	int embedding = getFlag(PDF_FONT_FLAG_NOEMBED) ? 0 : 1;
	if (!sfont->getTTFontDesc(&embedding, -1, 1, 
		                      &(desc.Ascent), &(desc.Descent),
		                      &(desc.StemV), &(desc.CapHeight),
		                      &(desc.XHeight), &(desc.AvgWidth),
		                      desc.FontBBox, &(desc.ItalicAngle),
		                      &(desc.Flags), desc.panose))
	{
		delete sfont;
		locking = false;
		return -1;
	}
	
	if (!embedding)
	{
		if (encoding_id >= 0 && !cache.isPredefinedEncoding(encoding_id))
		{
			delete sfont;
			locking = false;
			return -1;
		}
		else
		{
			setFlags(PDF_FONT_FLAG_NOEMBED);
			desc.Flags &= (1 << 2);
			desc.Flags |= (1 << 5);
		}
	}
	
	{
		char  fontnameA[256];
		memset(fontnameA, 0, 256);
    	int lengthA = sfont->getTTPSFontName(fontnameA, 255);
    	if (lengthA < 1) 
    	{
      		lengthA = qMin((int)strlen(ident), (int)255);
      		strncpy(fontnameA, ident, lengthA);
    	}
    	fontnameA[lengthA] = '\0';
    	for (int n = 0; n < lengthA; n++) 
    	{
      		if (fontnameA[n] == 0) 
        		memmove(fontnameA + n, fontnameA + n + 1, lengthA - n - 1);
    	}
    	
    	setFontName(fontnameA);
	}
	
	delete sfont;
	subtype = PDF_FONT_FONTTYPE_TRUETYPE;
	
	if (!dict.Type)
		dict.Type = qstrdup("Font");
		
	if (!dict.Subtype)
		dict.Subtype = qstrdup("TrueType");
	
	locking = false;
	return 0;
}

int XWTexFont::openType1()
{
	locking = true;
	if (isBuiltinFont(ident))
	{
		setFontName(ident);
    	setSubtype(PDF_FONT_FONTTYPE_TYPE1);
    	setFlags(PDF_FONT_FLAG_NOEMBED|PDF_FONT_FLAG_BASEFONT);
	}
	else
	{
		XWFontFileType1 * t1 = XWFontFileType1::loadPFB(ident, false);
		if (!t1)
		{
			locking = false;
			return -1;
		}
			
		char fontnameA[128];
		memset(fontnameA, 0, 128);
		if (t1->getFontName(fontnameA) < 0)
		{
			locking = false;
			return -1;
		}
			
		setFontName(fontnameA);
    	setSubtype(PDF_FONT_FONTTYPE_TYPE1);
	}
	
	if (!dict.Type)
		dict.Type = qstrdup("Font");
		
	if (!dict.Subtype)
		dict.Subtype = qstrdup("Type1");
	
	locking = false;
	return 0;
}

int XWTexFont::openType1C()
{
	locking = true;
	XWFontFileSFNT  * sfont = 0;
	if (ft_face)
		sfont = XWFontFileSFNT::load(ft_face, SFNT_TYPE_POSTSCRIPT, false);
	else
		sfont = XWFontFileSFNT::loadType1C(ident, false);
	if (!sfont)
	{
		locking = false;
		return -1;
	}
		
	XWFontFileCFF * cffont = sfont->makeType1C(0);
	if (!cffont)
	{
		locking = false;
		return -1;
	}
		
	fontname = cffont->getName();	
	delete cffont;
	desc.Flags = 0;
	setSubtype(PDF_FONT_FONTTYPE_TYPE1C);
	int embedding  = getFlag(PDF_FONT_FLAG_NOEMBED) ? 0 : 1;
	if (!sfont->getTTFontDesc(&embedding, -1, 1, 
		                      &(desc.Ascent), &(desc.Descent),
		                      &(desc.StemV), &(desc.CapHeight),
		                      &(desc.XHeight), &(desc.AvgWidth),
		                      desc.FontBBox, &(desc.ItalicAngle),
		                      &(desc.Flags), desc.panose))
	{
		delete sfont;
		locking = false;
		return -1;
	}
	
	delete sfont;
	
	if (!dict.Type)
		dict.Type = qstrdup("Font");
		
	if (!dict.Subtype)
		dict.Subtype = qstrdup("Type1");
		
	locking = false;
	return 0;
}

void XWTexFont::setFontName(const char *fontnameA)
{
	if (fontname)
		delete [] fontname;
		
	fontname = qstrdup(fontnameA);
}

void XWTexFont::setMapName(const char * n)
{
	if (map_name)
		delete [] map_name;
		
	map_name = qstrdup(n);
}

void XWTexFont::addSimpleMetrics(XWFontFileCFF *cffont, ushort num_glyphs)
{
	double scaling = 1;
	if (cffont->topDictKnow("FontMatrix"))
    	scaling = 1000 * cffont->topDictGet("FontMatrix", 0);
    	
    if (num_glyphs <= 1)
    {
    	dict.FirstChar = 0;
    	dict.LastChar = 0;
    	dict.Widths[0] = 0.0;
    }
    else
    {
    	dict.FirstChar = 255;
    	dict.LastChar = 0;
    	
    	for (int code = 0; code < 256; code++) 
    	{
      		if (usedchars[code]) 
      		{
				if (code < dict.FirstChar) 
					dict.FirstChar = code;
				if (code > dict.LastChar)  
					dict.LastChar  = code;
      		}
    	}
    	
    	if (dict.FirstChar > dict.LastChar)
    		return ;
    		
    	XWTFMFile tfm(map_name);
  		int tfm_id = tfm.getID();  
  		double width = 0.0;
  		for (int code = dict.FirstChar; code <= dict.LastChar; code++)
  		{
  			if (usedchars[code])
  			{
  				if (tfm_id < 0)
  					width = scaling * dict.Widths[code];
  				else
  					width = 1000. * tfm.getWidth(code);
  				dict.Widths[code] = round(width, 1.0);
  			}
  			else
  				dict.Widths[code] = 0.0;
  		}
    }
}

void XWTexFont::addMetrics(XWFontFileCFF *cffont, 
	                       char **enc_vec, 
	                       long num_glyphs)
{
	if (!cffont->topDictKnow("FontBBox"))
		return ;
		
	double scaling = 1;
	if (cffont->topDictKnow("FontMatrix"))
    	scaling = 1000 * cffont->topDictGet("FontMatrix", 0);
    	
    for (char i = 0; i < 4; i++)
    {
    	double val = cffont->topDictGet("FontBBox", i);
    	desc.FontBBox[i] = round(val, 1.0);
    }
    
    if (num_glyphs <= 1)
    {
    	dict.FirstChar = 0;
    	dict.LastChar = 0;
    	dict.Widths[0] = 0.0;
    }
    else
    {
    	dict.FirstChar = 255;
    	dict.LastChar = 0;
    	
    	for (int code = 0; code < 256; code++) 
    	{
      		if (usedchars[code]) 
      		{
				if (code < dict.FirstChar) 
					dict.FirstChar = code;
				if (code > dict.LastChar)  
					dict.LastChar  = code;
      		}
    	}
    	
    	if (dict.FirstChar > dict.LastChar)
    		return ;
    		
    	XWTFMFile tfm(map_name);
  		int tfm_id = tfm.getID();  
  		double width = 0.0;
  		for (int code = dict.FirstChar; code <= dict.LastChar; code++)
  		{
  			if (usedchars[code])
  			{
  				if (tfm_id < 0)
  					width = scaling * dict.Widths[cffont->lookupGlyph(enc_vec[code])];
  				else
  					width = 1000. * tfm.getWidth(code);
  				dict.Widths[code] = round(width, 1.0);
  			}
  			else
  				dict.Widths[code] = 0.0;
  		}
    }
}

int  XWTexFont::aglDecomposeGlyphName(char *glyphname, 
	                                  char **nptrs, 
	                                  int size, 
	                                  char **suffix)
{
	char * p = glyphname;
	char * q = strchr(p, '.');
	if (!q)
    	*suffix = 0;
  	else 
  	{
    	*q = '\0'; q++;
    	*suffix = q;
  	}
  	
  	nptrs[0] = p;
  	int n = 1;
  	for (; p && *p; n++) 
  	{
    	p = strchr(p, '_');
    	if (!p || p[1] == '\0')
      		break;
    	if (n >= size)
      		return -1;
    	*p = '\0'; p++;
    	nptrs[n] = p;
  	}

  	return  n;
}

void XWTexFont::cleanGlyphMapper(GlyphMapper *gm)
{
	if (gm->gsub)
    	delete gm->gsub;
  	if (gm->codetogid)
    	delete gm->codetogid;
  	if (gm->nametogid)
    	delete gm->nametogid;

  	gm->gsub = 0;
  	gm->codetogid = 0;
  	gm->nametogid = 0;
  	gm->sfont = 0;
}

int  XWTexFont::composeGlyph(ushort *glyphs, 
	                         int n_glyphs,
                             const char *feat, 
                             GlyphMapper *gm, 
                             ushort *gid)
{
	int   error = 0;
  	char  t[5] = {' ', ' ', ' ', ' ', 0};
  	if (!feat || feat[0] == '\0')
  		error = selectGsub("(?lig|lig?|?cmp|cmp?|frac|afrc)", gm);
  	else
  	{
  		if (strlen(feat) > 4)
      		error = -1;
    	else 
    	{
      		memcpy(t, feat, strlen(feat));
      		error = selectGsub(t, gm);
    	}
  	}
  	
  	if (!error)
    	error = gm->gsub->applyLig(glyphs, n_glyphs, gid);

  	return  error;
}

int  XWTexFont::composeUChar(long *unicodes, 
	                         int n_unicodes,
                             const char *feat, 
                             GlyphMapper *gm, 
                             ushort *gid)
{
	int error = 0;
	
	if (!gm->codetogid)
    	return  -1;

  	ushort * gids = new ushort[n_unicodes];
  	for (int i = 0; !error && i < n_unicodes; i++) 
  	{
    	gids[i] = gm->codetogid->lookup(unicodes[i]);
    	error   = (gids[i] == 0) ? -1 : 0;
  	}

  	if (!error)
    	error = composeGlyph(gids, n_unicodes, feat, gm, gid);

  	delete [] gids;

  	return  error;
}

int XWTexFont::doBuiltinEncoding(XWFontFileSFNT *sfont)
{
	XWSFNTCmap * ttcm = sfont->readCmap(TT_MAC, TT_MAC_ROMAN);
	if (!ttcm)
		return -1;
		
	char * cmap_table = (char*)malloc(274 * sizeof(char));
  	memset(cmap_table, 0, 274);
  	
  	XWFontFile::putUShort(cmap_table, 0);
  	XWFontFile::putUShort(cmap_table+2,  1);
  	XWFontFile::putUShort(cmap_table+4,  TT_MAC);
  	XWFontFile::putUShort(cmap_table+6,  TT_MAC_ROMAN);
  	XWFontFile::putULong(cmap_table+8,  12);
  	XWFontFile::putUShort(cmap_table+12, 0);
  	XWFontFile::putUShort(cmap_table+14, 262);
  	XWFontFile::putUShort(cmap_table+16, 0);
  		
  	XWTTGlyphs glyphs;
  	int count = 1;
  	ushort idx = 0;
  	for (int code = 0; code < 256; code++)
  	{
  		if (!usedchars[code])
      		continue;
      		
      	ushort gid = ttcm->lookup(code);
      	if (gid == 0)
      		idx = 0;
      	idx = glyphs.findGlyph(gid);
      	if (idx == 0)
        	idx  = glyphs.addGlyph(gid, count);
        	
        cmap_table[18+code] = idx & 0xff;
    	count++;
  	}
  	
  	delete ttcm;
  	if (glyphs.buildTables(sfont) < 0) 
  	{
    	free(cmap_table);
    	return  -1;
  	}
  	
  	for (int code = 0; code < 256; code++) 
  	{
    	if (usedchars[code]) 
    	{
      		idx = glyphs.getIndex((ushort) cmap_table[18+code]);
      		dict.Widths[code] = round(1000.0*(glyphs.gd[idx].advw)/(glyphs.emsize), 1);
    	} 
    	else 
      		dict.Widths[code] = 0.0;
  	}
  	doWidths();
  	sfont->setTable("cmap", cmap_table, 274);

  	return  0;
}

#define is_comp(n) (strchr((n), '_') != NULL)

int XWTexFont::doCustomEncoding(char **encoding, 
	                            XWFontFileSFNT *sfont)
{
	GlyphMapper gm;
	int error = setupGlyphMapper(&gm, sfont);
  	if (error)
  		return -1;
  		
  	char * cmap_table = (char*)malloc(274 * sizeof(char));
  	memset(cmap_table, 0, 274);
  	XWFontFile::putUShort(cmap_table, 0);
  	XWFontFile::putUShort(cmap_table+2, 1);
  	XWFontFile::putUShort(cmap_table+4,  TT_MAC);
  	XWFontFile::putUShort(cmap_table+6,  TT_MAC_ROMAN);
  	XWFontFile::putULong(cmap_table+8,  12);
  	XWFontFile::putUShort(cmap_table+12, 0);
  	XWFontFile::putUShort(cmap_table+14, 262);
  	XWFontFile::putUShort(cmap_table+16, 0);   
  		
  	XWTTGlyphs glyphs;
  	int count = 1;
  	ushort idx = 0;
  	ushort gid = 0;
  	for (int code = 0; code < 256; code++)
  	{
  		if (!usedchars[code])
      		continue;
      		
      	if (!encoding[code] || !strcmp(encoding[code], ".notdef"))
      		idx = 0;
      	else
      	{
      		if (is_comp(encoding[code]))
        		error = findComposite(encoding[code], &gid, &gm);
      		else
        		error = resolveGlyph(encoding[code], &gid, &gm);
        		
        	idx = glyphs.findGlyph(gid);
      		if (idx == 0)
      		{
      			idx = glyphs.addGlyph(gid, count);
        		count++;
      		}
      	}
      	
      	cmap_table[18 + code] = idx & 0xff;
  	}
  	
  	cleanGlyphMapper(&gm);
  	if (glyphs.buildTables(sfont) < 0)
  	{
  		free(cmap_table);
    	return  -1;
  	}
  	
  	for (int code = 0; code < 256; code++) 
  	{
    	if (usedchars[code]) 
    	{
      		int idx = glyphs.getIndex((ushort) cmap_table[18+code]);
      		dict.Widths[code] = round(glyphs.gd[idx].advw, 1.0);
    	} 
    	else 
      		dict.Widths[code] = 0.0;
  	}
  	
  	doWidths();
  	
  	sfont->setTable("cmap", cmap_table, 274);

  	return  0;
}

void XWTexFont::doWidths()
{
	dict.FirstChar = 255;
	dict.LastChar = 0;
	for (int code = 0; code < 256; code++) 
	{
    	if (usedchars[code]) 
    	{
      		if (code < dict.FirstChar) 
      			dict.FirstChar = code;
      		if (code > dict.LastChar)  
      			dict.LastChar  = code;
    	}
  	}
  	
  	if (dict.FirstChar > dict.LastChar)
  		return ;
  		  		
  	XWTFMFile tfm(map_name);
  	int tfm_id = tfm.getID();  
  	int i = 0;		
  	for (int code = dict.FirstChar; code <= dict.LastChar; code++)
  	{
  		if (usedchars[code])
  		{
  			if (tfm_id < 0)
  				dict.Widths[code] = round(dict.Widths[code], 1.0);
  			else
  				dict.Widths[code] = round(tfm.getWidth(code), 1.0);
  		}
  		else
  			dict.Widths[code] = 0.0;
  			
  		i++;
  	}
  	
  	if (i == 0)
  	{
  		delete [] dict.Widths;
  		dict.Widths = 0;
  	}
}

int XWTexFont::findComposite(const char *glyphname, 
	                         ushort *gid, 
	                         GlyphMapper *gm)
{
	int error = findPostTable(glyphname, gid, gm);
  	if (!error)
    	return  0;
    
    char * gname = qstrdup(glyphname);	
    ushort    gids[32];
  	char     *nptrs[32];
  	char *suffix = 0;
    memset(gids, 0, 32 * sizeof(ushort));
  	int n_comp = aglDecomposeGlyphName(gname, nptrs, 32, &suffix);
  	error = 0;
  	for (int i = 0; !error && i < n_comp; i++)
  		error = resolveGlyph(nptrs[i], &gids[i], gm);
  		
  	if (!error) 
  	{
    	if (suffix &&
        	(!strcmp(suffix, "liga") || !strcmp(suffix, "dlig") ||
         	!strcmp(suffix, "hlig") || !strcmp(suffix, "frac") ||
         	!strcmp(suffix, "ccmp") || !strcmp(suffix, "afrc"))) 
        {
      		error = composeGlyph(gids, n_comp, suffix, gm, gid);
    	} 
    	else 
    	{
      		error = composeGlyph(gids, n_comp, 0, gm, gid);
      		if (!error && suffix)
        		error = selectGlyph(*gid, suffix, gm, gid);
    	}
  	}
  	
  	delete [] gname;

  	return  error;
}

int XWTexFont::findParanoiac(const char *glyphname, 
	                         ushort *gid, 
	                         GlyphMapper *gm)
{
	XWFontCache cache(true);
	XWAdobeGlyphName * agln = cache.aglLookupList(glyphname);
	ushort idx   = 0U;
  	int    error = 0;
  	while (agln && idx == 0)
  	{
  		if (agln->suffix)
  		{
  			error = findParanoiac(agln->name, &idx, gm);
      		if (error)
        		return error;
        		
        	error = selectGlyph(idx, agln->suffix, gm, &idx);
      		if (error)
      			error = 0;
  		}
  		else
  		{
  			if (agln->n_components == 1)
        		idx = gm->codetogid->lookup(agln->unicodes[0]);
        	else if (agln->n_components > 1)
        	{
        		error = composeUChar(agln->unicodes, agln->n_components, 0, gm, &idx);
        	}
  		}
  		
  		agln = agln->alternate;
  	}
  	
  	*gid = idx;
  	return (idx == 0 ? -1 : 0);
}

int XWTexFont::findPostTable(const char *glyph_name, 
	                         ushort *gid, 
	                         GlyphMapper *gm)
{
	if (!gm->nametogid)
    	return -1;

  	*gid = gm->nametogid->lookup(glyph_name);
  	return (*gid == 0 ? -1 : 0);
}

void XWTexFont::getFontAttr(XWFontFileCFF *cffont)
{
	static const char *L_c[] = 
	{
    	"H", "P", "Pi", "Rho", NULL
  	};
  	
  	static const char *L_d[] = 
  	{
    	"p", "q", "mu", "eta", NULL
  	};
  	
  	static const char *L_a[] = 
  	{
    	"b", "h", "lambda", NULL
  	};
  	
  	double defaultwidth = 500.0;
  	double nominalwidth = 0.0;
  	if (cffont->topDictKnow("FontBBox")) 
  	{
    	/* Default values */
    	desc.CapHeight = desc.Ascent = cffont->topDictGet("FontBBox", 3);
    	desc.Descent = cffont->topDictGet("FontBBox", 1);
  	} 
  	else 
  	{
    	desc.CapHeight =  680.0;
    	desc.Ascent    =  690.0;
    	desc.Descent   = -190.0;
  	}
  	
  	if (cffont->privDictKnow(0, "StdVW"))
  		desc.StemV = cffont->privDictGet(0, "StdVW", 0);
  	else
  		desc.StemV = 88.0;
  		
  	desc.Flags = 0;
  	
  	if (cffont->topDictKnow("ItalicAngle")) 
  	{
    	desc.ItalicAngle = cffont->topDictGet("ItalicAngle", 0);
    	if (desc.ItalicAngle != 0.0)
      		desc.Flags |= FONT_FLAG_ITALIC;
  	} 
  	else 
    	desc.ItalicAngle = 0.0;
    	
    XWCFFIndex * cstrings = cffont->getCStrings();
    XWCFFIndex * subrs = cffont->getSubrs(0);
    long gid = cffont->lookupGlyph("space");
    XWT1GInfo gm;
    if (gid >= 0 && gid < cstrings->count) 
    {
    	gm.getMetrics(cstrings->data + cstrings->offset[gid] - 1, 
    	              cstrings->offset[gid+1] - cstrings->offset[gid], subrs);
    	defaultwidth = gm.wx;
  	}
  	
  	for (int i = 0; L_c[i] != NULL; i++) 
  	{
    	gid = cffont->lookupGlyph(L_c[i]);
    	if (gid >= 0 && gid < cstrings->count) 
    	{
      		gm.getMetrics(cstrings->data + cstrings->offset[gid] - 1,
		      			  cstrings->offset[gid+1] - cstrings->offset[gid], subrs);
      		desc.CapHeight = gm.bbox.ury;
      		break;
    	}
  	}
  	
  	for (int i = 0; L_d[i] != NULL; i++) 
  	{
    	gid = cffont->lookupGlyph(L_d[i]);
    	if (gid >= 0 && gid < cstrings->count) 
    	{
      		gm.getMetrics(cstrings->data + cstrings->offset[gid] - 1,
			 			  cstrings->offset[gid+1] - cstrings->offset[gid], subrs);
      		desc.Descent = gm.bbox.lly;
      		break;
    	}
  	}
  	
  	for (int i = 0; L_a[i] != NULL; i++) 
  	{
    	gid = cffont->lookupGlyph(L_a[i]);
    	if (gid >= 0 && gid < cstrings->count) 
    	{
      		gm.getMetrics(cstrings->data + cstrings->offset[gid] - 1,
			 			  cstrings->offset[gid+1] - cstrings->offset[gid], subrs);
      		desc.Ascent = gm.bbox.ury;
      		break;
    	}
  	}
  	
  	if (defaultwidth != 0.0) 
  	{
    	cffont->privDictAdd(0, "defaultWidthX", 1);
    	cffont->privDictSet(0, "defaultWidthX", 0, defaultwidth);
  	}
  	
  	if (nominalwidth != 0.0) 
  	{
    	cffont->privDictAdd(0, "nominalWidthX", 1);
    	cffont->privDictSet(0, "nominalWidthX", 0, nominalwidth);
  	}
  	
  	if (cffont->privDictKnow(0, "ForceBold") && 
  		cffont->privDictGet(0, "ForceBold", 0)) 
  	{
    	desc.Flags |= FONT_FLAG_FORCEBOLD;
  	}
  	
  	if (cffont->privDictKnow(0, "IsFixedPitch") && 
  		cffont->privDictGet(0, "IsFixedPitch", 0)) 
  	{
    	desc.Flags |= FONT_FLAG_FIXEDPITCH;
  	}
  	
  	if (fontname && !strstr(fontname, "Sans")) 
    	desc.Flags |= FONT_FLAG_SERIF;
    	
  	if (fontname &&  strstr(fontname, "Caps")) 
    	desc.Flags |= FONT_FLAG_SMALLCAP;
    	
  	desc.Flags |= FONT_FLAG_SYMBOLIC;
}

int XWTexFont::selectGlyph(ushort in, 
	                       const char *suffix, 
	                       GlyphMapper *gm, 
	                       ushort *out)
{
	char * s = qstrdup(suffix);
	char * q = (char *) XWAdobeGlyphName::suffixToOtltag(s);
	int error = 0;
	if (q)
	{
		error = selectGsub(q, gm);
		if (!error)
      		error = gm->gsub->apply(&in);
	}
	else
	{
		char t[5];
		if (strlen(s) > 4)
			error = -1;
		else if (strlen(s) == 4)
			error = selectGsub(s, gm);
		else
		{
			memset(t, ' ', 4); 
			t[4] = '\0';
      		memcpy(t, s, strlen(s));
      		error = selectGsub(t, gm);
		}
		
		if (!error)
			error = gm->gsub->apply(&in);
		else
		{
			for (q = s + strlen(s) - 1; q > s && *q >= '0' && *q <= '9'; q--);
			
			if (q == s)
        		error = -1;
        	else
        	{
        		int n = atoi(q + 1) - 1; 
        		q[1] = '\0';
        		if (strlen(s) > 4)
          			error = -1;
        		else 
        		{
          			memset(t, ' ', 4); 
          			t[4] = '\0';
          			memcpy(t, s, strlen(s));
          			error = selectGsub(s, gm);
          			if (!error)
            			error = gm->gsub->applyAlt(n, &in);
        		}
        	}
		}
	}
	
	delete [] s;

  	*out = in;
  	return  error;
}

int  XWTexFont::resolveGlyph(const char *glyphname, 
	                         ushort *gid, 
	                         GlyphMapper *gm)
{
	int error = findPostTable(glyphname, gid, gm);
  	if (!error)
    	return  0;

  	if (!gm->codetogid)
    	return  -1;
    	
    char *suffix = 0;
    char * name = XWAdobeGlyphName::chopSuffix(glyphname, &suffix);
  	if (!name)
  		error = -1;
  	else if (XWAdobeGlyphList::nameIsUnicode(name)) 
  	{
    	long ucv  = XWAdobeGlyphList::nameConvertUnicode(name);
    	*gid = gm->codetogid->lookup(ucv);
    	error = (*gid == 0) ? -1 : 0;
  	} 
  	else 
    	error = findParanoiac(name, gid, gm);
    	
  	if (!error && suffix)
  	{
  		error = selectGlyph(*gid, suffix, gm, gid);
  		if (error)
  			error = 0;
  	}
  	
  	if (suffix)
    	delete [] suffix;
    	
  	if (name)
    	delete [] name;

  	return  error;
}

int XWTexFont::selectGsub(const char *feat, GlyphMapper *gm)
{
	if (!feat || *feat == 0 || !gm || !gm->gsub)
    	return  -1;
    	
    int idx = gm->gsub->select("*", "*", feat);
  	if (idx >= 0)
    	return  0;
    	
    int error = gm->gsub->addFeat("*", "*", feat, gm->sfont);
  	if (!error) 
  	{
    	idx = gm->gsub->select("*", "*", feat);
    	return  (idx >= 0 ? 0 : -1);
  	}

  	return  -1;
}

int  XWTexFont::setupGlyphMapper(GlyphMapper *gm, XWFontFileSFNT *sfont)
{
	gm->sfont     = sfont;
  	gm->nametogid = sfont->readPostTable();
  	gm->codetogid = sfont->readCmap(TT_WIN, TT_WIN_UCS4);
  	if (!gm->codetogid)
    	gm->codetogid = sfont->readCmap(TT_WIN, TT_WIN_UNICODE);

  	if (!gm->nametogid && !gm->codetogid)
    	return -1;

  	gm->gsub = new XWOTLGsub;

  	return 0;
}

long XWTexFont::writeFontFile(XWFontFileCFF *cffont, 
	                          uchar ** streamdata, 
	                          long * streamlen)
{
	XWCFFIndex * topdict = new XWCFFIndex((ushort)1);
	if (!cffont->topDictKnow("CharStrings"))
    	cffont->topDictAdd("CharStrings", 1);
  	if (!cffont->topDictKnow("charset"))
    	cffont->topDictAdd("charset", 1);
  	if (!cffont->topDictKnow("Encoding"))
    	cffont->topDictAdd("Encoding", 1);
    	
    uchar wbuf[1024];
  	long private_size = cffont->privDictPack(0, wbuf, 1024);
  	if (private_size > 0 && !cffont->topDictKnow("Private"))
    	cffont->topDictAdd("Private", 2);
  	topdict->offset[1] = cffont->topDictPack(wbuf, 1024) + 1;
  	
  	long charstring_len = cffont->cstringSize();
  	long stream_data_len = 4; /* header size */
  	stream_data_len += cffont->nameSize();
  	stream_data_len += topdict->size();
  	stream_data_len += cffont->stringSize();
  	stream_data_len += cffont->gsubrSize();
  	XWCFFEncoding * encoding = cffont->getEncoding();
  	XWCFFCharsets * charsets = cffont->getCharsets();
  	stream_data_len += 2 + (encoding->num_entries)*2 + 1 + (encoding->num_supps)*3;
  	stream_data_len += 1 + (charsets->num_entries)*2;
  	stream_data_len += charstring_len;
  	stream_data_len += private_size;
  	
  	uchar * stream_data_ptr = (uchar*)malloc(stream_data_len * sizeof(uchar));
  	
  	long offset = 0;  	
  	offset += cffont->putHeader(stream_data_ptr + offset, stream_data_len - offset);
  	offset += cffont->packName(stream_data_ptr + offset, stream_data_len - offset);
  	long topdict_offset = offset;
  	offset += topdict->size();
  	offset += cffont->packString(stream_data_ptr + offset, stream_data_len - offset);
  	offset += cffont->packGsubr(stream_data_ptr + offset, stream_data_len - offset);
  	cffont->topDictSet("Encoding", 0, offset);
  	offset += cffont->packEncoding(stream_data_ptr + offset, stream_data_len - offset);
  	cffont->topDictSet("charset", 0, offset);
  	offset += cffont->packCharsets(stream_data_ptr + offset, stream_data_len - offset);
  	cffont->topDictSet("CharStrings", 0, offset);
  	offset += cffont->packCString(stream_data_ptr + offset, charstring_len);
  	if (private_size > 0) 
  	{
    	private_size = cffont->privDictPack(0, stream_data_ptr + offset, private_size);
    	cffont->topDictSet("Private", 1, offset);
    	cffont->topDictSet("Private", 0, private_size);
  	}
  	offset += private_size;
  	topdict->data = (uchar*)malloc((topdict->offset[1] - 1) * sizeof(uchar));
  	cffont->topDictPack(topdict->data, topdict->offset[1] - 1);
  	topdict->pack(stream_data_ptr + topdict_offset, topdict->size());
  	delete topdict;
  	
  	if (streamdata)
  		*streamdata = stream_data_ptr;
  	else
  		free(stream_data_ptr);
  		
  	if (streamlen)
  		*streamlen = offset;
  		
  	return offset;
}

#define CACHE_ALLOC_SIZE 16u

XWTexFontCache::XWTexFontCache()
	:count(0),
	 capacity(0),
	 fonts(0)
{
	capacity = CACHE_ALLOC_SIZE;
  	fonts    = (XWTexFont**)malloc(capacity * sizeof(XWTexFont*));
}

XWTexFontCache::~XWTexFontCache()
{
	if (fonts)
	{
		for (int i = 0; i < count; i++)
		{
			if (fonts[i])
				delete fonts[i];
		}
		
		free(fonts);
	}
}

int XWTexFontCache::add(XWTexFont * font)
{
	int font_id = count;
	if (count >= capacity) 
	{
		capacity += CACHE_ALLOC_SIZE;
		fonts     = (XWTexFont**)realloc(fonts, capacity * sizeof(XWTexFont*));
    }
    fonts[count] = font;
    count++;
    return font_id;
}

int XWTexFontCache::find(const char *texname,
	                     const char *fontname,
		       	         double font_scale, 
		       	         int    encoding_id,
		       	         XWTexFontMap *mrec,
		       	         int base_dpi)
{
	int font_id = -1;
	int found = 0;
	for (int i = 0; i < count; i++)
	{
		XWTexFont * font = fonts[i];
		switch (font->getSubtype())
		{
			case PDF_FONT_FONTTYPE_TYPE1:
      		case PDF_FONT_FONTTYPE_TYPE1C:
      		case PDF_FONT_FONTTYPE_TRUETYPE:
      			if (!strcmp(fontname, font->ident) && encoding_id == font->encoding_id) 
      			{
          			if (mrec && mrec->opt->index == font->index)
            			found = 1;
				}
      			break;
      			
      		case PDF_FONT_FONTTYPE_TYPE3:
      			if (!strcmp(fontname, font->ident) && 
      				font_scale == font->point_size) 
      			{
	  				found = 1;
				}
      			break;
      			
      		default:
      			break;
		}
		
		if (found)
		{
			font_id = i;
			break;
		}
	}
	
	if (!found)
	{
		font_id = count;
		int index = (mrec && mrec->opt->index) ? mrec->opt->index : 0;
		XWTexFont * font = new XWTexFont(fontname, texname, encoding_id, index, font_scale);
		font->ft_face = mrec->opt->ftface;
		
		if (font->openTrueType() < 0 && 
			font->openType1() < 0 && 
			font->openType1C() < 0 && 
			font->openPK(base_dpi) < 0 && 
			font->openFT() < 0)
		{
			delete font;
			font_id = -1;
		}
		else
			font_id = add(font);
	}
	
	return  font_id;
}

XWTexFont * XWTexFontCache::get(int font_id)
{
	if (font_id < 0 || font_id >= count)
		return 0;
		
	return fonts[font_id];
}

int XWTexFontCache::getFontEncoding(int font_id)
{
	if (font_id < 0 || font_id >= count)
		return -1;
		
	XWTexFont * font = fonts[font_id];
	return font->getEncoding();
}

int XWTexFontCache::getFontSubtype(int font_id)
{
	if (font_id < 0 || font_id >= count)
		return -1;
		
	XWTexFont * font = fonts[font_id];
	return font->getSubtype();
}

char * XWTexFontCache::getFontUsedChars(int font_id)
{
	if (font_id < 0 || font_id >= count)
		return 0;
	
	XWTexFont * font = fonts[font_id];
	return font->getUsedChars();
}

int XWTexFontCache::getFontWMode(int )
{
	return 0;
}

