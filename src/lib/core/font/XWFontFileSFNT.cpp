/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWApplication.h"
#include "XWNumberUtil.h"
#include "XWUtil.h"
#include "XWFontFileCFF.h"
#include "XWSFNTType.h"
#include "XWFontFileSFNT.h"

#define PDFUNIT(v) (round((1000.0*(v))/(head->unitsPerEm),1))

#define FIXEDWIDTH (1 << 0)  /* Fixed-width font */
#define SERIF      (1 << 1)  /* Serif font */
#define SYMBOLIC   (1 << 2)  /* Symbolic font */
#define SCRIPT     (1 << 3)  /* Script font */
#define STANDARD   (1 << 5)  /* Uses the Adobe Standard Character Set */
#define ITALIC     (1 << 6)  /* Italic */
#define ALLCAP     (1 << 16) /* All-cap font */
#define SMALLCAP   (1 << 17) /* Small-cap font */
#define FORCEBOLD  (1 << 18) /* Force bold at small text sizes */

static unsigned char padbytes[4] = {0, 0, 0, 0};

XWFontFileSFNT::XWFontFileSFNT(QIODevice * fileA,
		 	                   int    fileFinalA,
		 	                   ulong offsetA,
		 	       			   ulong lenA)
	:XWFontFile(fileA, fileFinalA, offsetA, lenA)
{
	ulong tagA = getULong();
	if (tagA == SFNT_TRUETYPE)
		type = SFNT_TYPE_TRUETYPE;
	else if (tagA == SFNT_OPENTYPE)
		type = SFNT_TYPE_OPENTYPE;
	else if (tagA == SFNT_POSTSCRIPT)
		type = SFNT_TYPE_POSTSCRIPT;
	else if (tagA == SFNT_TTC)
		type = SFNT_TYPE_TTC;
	else 
		type = -1;
		
	directory = 0;
}

XWFontFileSFNT::XWFontFileSFNT(QIODevice * fileA,
		 	                   int    fileFinalA,
		 	                   ulong offsetA,
		 	       			   ulong lenA,
		 	                   int  indexA)
	:XWFontFile(fileA, fileFinalA, offsetA, lenA)
{
	ft_face = 0;
	loc = 0;
	ulong rdata_pos = getULong();
	ulong map_pos   = getULong();
	seek(map_pos + 0x18);
	ulong tags_pos = map_pos + getUShort();
  	seek(tags_pos);
  	ushort tags_num = getUShort();
  	ulong tagA = 0;
  	ulong types_pos = 0;
  	ushort types_num = 0;
  	int i = 0;
  	for (; i <= tags_num; i++)
  	{
  		tagA = getULong();
  		types_num = getUShort();
  		types_pos = tags_pos + getUShort();
  		if (tagA == 0x73666e74UL)
  			break;
  	}
  	
  	if (i <= tags_num)
  	{
  		seek(types_pos);
  		if (indexA <= types_num)
  		{
  			ulong res_pos = 0;
  			for (int i = 0; i <= types_num; i++)
  			{
  				getUShort();
  				getUShort();
  				res_pos = getULong();
  				getULong();
  				if (i == indexA) 
  					break;
  			}
  			
  			type = SFNT_TYPE_DFONT;
  			offset = (res_pos & 0x00ffffffUL) + rdata_pos + 4;
  		}
  		else
  			type = -1;
  	}
  	else
  		type = -1;
  		
	directory = 0;
}

XWFontFileSFNT::XWFontFileSFNT(int fileFinalA)
	:XWFontFile(0, fileFinalA, 0, 0)
{
	ft_face = 0;
	loc = 0;
	directory = 0;
}

XWFontFileSFNT::~XWFontFileSFNT()
{
	if (directory)
		delete directory;
}

bool XWFontFileSFNT::createFontFileStream(uchar ** streamdata, 
	                                      long * streamlen)
{
	if (!file && !ft_face)
		return false;
		
	if (!streamdata)
		return false;
		
	uchar * dataA = 0;
	long   lenA = 0;
	char   buf[1024];
	char * p  = (char *)buf;
  	p += XWFontFile::putULong(p, directory->version);
  	p += XWFontFile::putUShort(p, directory->num_kept_tables);
  	int sr = max2floor(directory->num_kept_tables) * 16;
  	p += XWFontFile::putUShort(p, sr);
  	p += XWFontFile::putUShort(p, log2floor(directory->num_kept_tables));
  	p += XWFontFile::putUShort(p, directory->num_kept_tables * 16 - sr);
  		
  	dataA = (uchar*)malloc(12 * sizeof(uchar));
  	lenA = 12;
  	memcpy(dataA, buf, 12);
  	
  	long offsetA = 12 + 16 * directory->num_kept_tables;
  	for (int i = 0; i < directory->num_tables; i++)
  	{
  		if (directory->flags[i] & SFNT_TABLE_REQUIRED) 
  		{
      		if ((offsetA % 4) != 0) 
				offsetA += 4 - (offsetA % 4);

      		p = (char *)buf;
      		memcpy(p, directory->tables[i].tag, 4);
      		p += 4;
      		p += XWFontFile::putULong(p, directory->tables[i].checksum);
      		p += XWFontFile::putULong(p, offsetA);
      		p += XWFontFile::putULong(p, directory->tables[i].length);
      		dataA = (uchar*)realloc(dataA, (lenA + 16) * sizeof(uchar));
  			memcpy(dataA + lenA, buf, 16);
      		lenA += 16;

      		offsetA += directory->tables[i].length;
    	}
  	}
  	
  	offsetA = 12 + 16 * directory->num_kept_tables;
  	for (int i = 0; i < directory->num_tables; i++)
  	{
  		if (directory->flags[i] & SFNT_TABLE_REQUIRED)
  		{
  			if ((offsetA % 4) != 0) 
  			{
				long length  = 4 - (offsetA % 4);
				dataA = (uchar*)realloc(dataA, (lenA + length) * sizeof(uchar));
  				memcpy(dataA + lenA, padbytes, length);
      			lenA += length;
				offsetA += length;
      		}
      		
      		if (!(directory->tables[i].data))
      		{
      			long length = directory->tables[i].length;
				seek(directory->tables[i].offset); 
				while (length > 0)
				{
					long nb_read = read(buf, qMin((long)length, (long)1024));
					if (nb_read > 0)
					{
						dataA = (uchar*)realloc(dataA, (lenA + nb_read) * sizeof(uchar));
						memcpy(dataA + lenA, buf, nb_read);
						lenA += nb_read;
					}
					else
						break;
					
					length -= nb_read;
				}
      		}
      		else
      		{
      			dataA = (uchar*)realloc(dataA, (lenA + directory->tables[i].length) * sizeof(uchar));
      			memcpy(dataA + lenA, directory->tables[i].data, directory->tables[i].length);
      			lenA += directory->tables[i].length;
				free(directory->tables[i].data);
				directory->tables[i].data = 0;
      		}
      		
      		offsetA += directory->tables[i].length;
  		}
  	}
  	
  	*streamdata = dataA;
  	if (streamlen)
  		*streamlen = lenA;
  		
  	return true;
}

int XWFontFileSFNT::findTableIndex(const char *tagA)
{
	return directory->findTableIndex(tagA);
}

ulong XWFontFileSFNT::findTableLen(const char *tagA)
{
	return directory->findTableLen(tagA);
}

ulong XWFontFileSFNT::findTablePos(const char *tagA)
{
	return directory->findTablePos(tagA);
}

bool XWFontFileSFNT::getTTFontDesc(int *embed, 
	                               int stemv, 
	                               int typeA,
	                               double * Ascent,
	                               double * Descent,
	                               double * StemV,
	                               double * CapHeight,
	                               double * XHeight,
	                               double * AvgWidth,
	                               double * FontBBox,
	                               double * ItalicAngle,
	                               int    * Flags,
	                               uchar  * panose)
{
	XWTTOs2Table * os2  = readOs2Table();
	XWTTHeadTable * head = readHeadTable();
	XWTTPostTable * post = readPostTable();
	if (!post) 
	{
    	delete os2;
    	delete head;
    	return false;
  	}
  	
  	if (*embed)
  	{
  		if (os2->fsType == 0x0000 || (os2->fsType & 0x0008))
  			*embed = 1;
  		else if (os2->fsType & 0x0004)
  			*embed = 1;
  		else
  			*embed = 0;
  	}
  	
  	*Ascent = PDFUNIT(os2->sTypoAscender);  	
  	*Descent = PDFUNIT(os2->sTypoDescender);
  	if (stemv < 0)
  		stemv = (os2->usWeightClass/65.)*(os2->usWeightClass/65.)+50;
	*StemV = stemv;
	*CapHeight = 0.0;
	*XHeight = 0.0;
	if (os2->version == 0x0002)
	{
		*CapHeight = PDFUNIT(os2->sCapHeight);
		*XHeight = PDFUNIT(os2->sxHeight);
	}
	else
		*CapHeight = PDFUNIT(os2->sTypoAscender);
		
	*AvgWidth = 0.0;
	if (os2->xAvgCharWidth != 0)
		*AvgWidth = PDFUNIT(os2->xAvgCharWidth);
		
	FontBBox[0] = PDFUNIT(head->xMin);
	FontBBox[1] = PDFUNIT(head->yMin);
	FontBBox[2] = PDFUNIT(head->xMax);
	FontBBox[3] = PDFUNIT(head->yMax);
	*ItalicAngle = fixed(post->italicAngle);
	int flag = SYMBOLIC;
	if (os2->fsSelection & (1 << 0))
    	flag |= ITALIC;
  	if (os2->fsSelection & (1 << 5))
    	flag |= FORCEBOLD;
  	if (((os2->sFamilyClass >> 8) & 0xff) != 8)
    	flag |= SERIF;
  	if (((os2->sFamilyClass >> 8) & 0xff) == 10)
    	flag |= SCRIPT;
  	if (post->isFixedPitch)
    	flag |= FIXEDWIDTH;
    	
    *Flags = flag;
    if (typeA == 0)
    {
    	panose[0] = os2->sFamilyClass >> 8;
    	panose[1] = os2->sFamilyClass & 0xff;
    	memcpy(panose+2, os2->panose, 10);
    }
    
    delete head;
    delete os2;
    delete post;
    return true;
}

ushort XWFontFileSFNT::getTTName(char *dest, 
	                             ushort destlen,
	                             ushort plat_id, 
	                             ushort enco_id,
	     			             ushort lang_id, 
	     			             ushort name_id)
{
	ulong name_offset = locateTable("name");
	if (getUShort()) 
	{
		xwApp->error(tr("expecting zero.\n"));
		return 0;
	}
	
	ushort num_names = getUShort();
  	ushort string_offset = getUShort();
  	ushort i = 0;
  	ushort length = 0;
  	for (; i < num_names;i++) 
  	{
    	ushort p_id = getUShort();
    	ushort e_id = getUShort();
    	ushort l_id = getUShort();
    	ushort n_id = getUShort();
    	length = getUShort();
    	ushort offsetA = getUShort();
    	if ((p_id == plat_id) && (e_id == enco_id) && 
			(lang_id == 0xffffu || l_id == lang_id) && 
			(n_id == name_id)) 
		{
      		if (length > destlen - 1) 
      		{
				xwApp->warning(tr("\n** Notice: Name string too long. Truncating **\n"));
				length = destlen - 1;
      		}
      		
      		seek(name_offset + string_offset + offsetA);
      		read(dest, length);
      		dest[length] = '\0';
      		break;
    	}
  	}
  	
  	if (i == num_names) 
    	length = 0;

  	return length;
}

ushort XWFontFileSFNT::getTTPSFontName(char *dest, ushort destlen)
{
	ushort namelen = 0;
	if (ft_face)
	{
		const char* name = FT_Get_Postscript_Name(ft_face);
    namelen = strlen(name);
    if (namelen > destlen - 1) 
    {
      	strncpy(dest, name, destlen - 1);
      	dest[destlen] = 0;
    }
    else
      strcpy(dest, name);
	}
	else
	{
  	if ((namelen = getTTName(dest, destlen, 1, 0, 0, 6)) != 0 ||
       (namelen = getTTName(dest, destlen, 3, 1, 0x409u, 6)) != 0 ||
       (namelen = getTTName(dest, destlen, 3, 5, 0x412u, 6)) != 0)
    	return namelen;

  	xwApp->warning(tr("\n** no valid PostScript name available **\n"));
  	if ((namelen = getTTName(dest, destlen, 1, 0, 0xffffu, 6)) == 0) 
    	namelen = getTTName(dest, destlen, 1, 0, 0, 1);
	}
  return namelen;
}

bool XWFontFileSFNT::isDFont(QIODevice * fp)
{
	fp->seek(0);
	getUnsignedQuad(fp);
	ulong pos = getUnsignedQuad(fp) + 0x18;
  	fp->seek(pos);
  	pos += getUnsignedPair(fp);
  	fp->seek(pos);
  	int n = getUnsignedPair(fp);
  	for (int i = 0; i <= n; i++) 
  	{
    	if (getUnsignedQuad(fp) == 0x73666e74UL) /* "sfnt" */
      		return true;
    	getUnsignedQuad(fp);
  	}
  	
  	return false;
}

XWFontFileSFNT  * XWFontFileSFNT::load(char *fileName, 
	                                   int indexA, 
	                                   bool deleteFileA)
{
	int fileFinalA = FONTFILE_CLOSE | FONTFILE_DEL;
	if (deleteFileA)
		fileFinalA |= FONTFILE_REMOVE;
		
	XWFontFileSFNT * sfnt = 0;
	QFile * fileA = open(fileName, XWFontSea::TrueType);
	if (!fileA)
	{
		fileA = open(fileName, XWFontSea::OpenType);			
		if (!fileA)
			return 0;			
		sfnt = new XWFontFileSFNT(fileA, fileFinalA, 0, (ulong)(fileA->size()));
	}
	else 
	{
		if (isDFont(fileA))
			sfnt = new XWFontFileSFNT(fileA, fileFinalA, 0, (ulong)(fileA->size()), indexA);
		else
			sfnt = new XWFontFileSFNT(fileA, fileFinalA, 0, (ulong)(fileA->size()));
	}	
	
	ulong offsetA = 0;
	switch (sfnt->type)
	{
		case SFNT_TYPE_TTC:
			offsetA = sfnt->readTTCOffset(indexA);
    		if (offsetA == 0) 
    		{
      			delete sfnt;
				return 0;
    		}
    		break;
    		
    	case SFNT_TYPE_TRUETYPE:
  		case SFNT_TYPE_POSTSCRIPT:
    		offsetA = 0;
    		break;
    		
    	case SFNT_TYPE_DFONT:
    		offsetA = sfnt->offset;
    		break;
    		
    	default:
    		delete sfnt;
			return 0;
    		break;
	}
	
	sfnt->readTableDirectory(offsetA);
	return sfnt;
}

XWFontFileSFNT  * XWFontFileSFNT::load(FT_Face face,
	                                     int accept_types, 
	                                     bool deleteFileA)
{
	if (!face || !FT_IS_SFNT(face))
    return 0;
    
	int fileFinalA = FONTFILE_CLOSE | FONTFILE_DEL;
	if (deleteFileA)
		fileFinalA |= FONTFILE_REMOVE;
		
	XWFontFileSFNT * sfnt = new XWFontFileSFNT(fileFinalA);
	sfnt->ft_face = face;
  sfnt->type = 0;
  
  ulong type = sfnt->ftunsignedQuad();
  
  if (type == SFNT_TRUETYPE || type == SFNT_MAC_TRUE) 
    sfnt->type = SFNT_TYPE_TRUETYPE;
  else if (type == SFNT_OPENTYPE) 
    sfnt->type = SFNT_TYPE_OPENTYPE;
  else if (type == SFNT_POSTSCRIPT) 
    sfnt->type = SFNT_TYPE_POSTSCRIPT;
  else if (type == SFNT_TTC) 
    sfnt->type = SFNT_TYPE_TTC;

  if ((sfnt->type & accept_types) == 0) 
  {
    delete sfnt;
    return 0;
  }
  
  sfnt->directory = NULL;

  return sfnt;
}

XWFontFileSFNT  * XWFontFileSFNT::loadCIDType0(char *fileName, 
	                                          bool deleteFileA)
{
	QFile * fileA = open(fileName, XWFontSea::OpenType);
	if (!fileA)
		return 0;
	
	int fileFinalA = FONTFILE_CLOSE | FONTFILE_DEL;
	if (deleteFileA)
		fileFinalA |= FONTFILE_REMOVE;
		
	XWFontFileSFNT * sfnt = new XWFontFileSFNT(fileA, fileFinalA, 0, (ulong)(fileA->size()));
	long offsetA = 0;
	if ((sfnt->type != SFNT_TYPE_POSTSCRIPT) || 
		(sfnt->readTableDirectory(0) < 0) || 
		(((offsetA = sfnt->findTablePos("CFF ")) <= 0)))
	{
		delete sfnt;
		return 0;
	}
		
	return sfnt;
}

XWFontFileSFNT  * XWFontFileSFNT::loadCIDType2(char *fileName, 
	                                          int indexA, 
	                                          bool deleteFileA)
{
	int fileFinalA = FONTFILE_CLOSE | FONTFILE_DEL;
	if (deleteFileA)
		fileFinalA |= FONTFILE_REMOVE;
		
	QFile * fileA = open(fileName, XWFontSea::TrueType);
	if (!fileA)
		return 0;
	
	XWFontFileSFNT * sfnt = 0;
	if (isDFont(fileA))
		sfnt = new XWFontFileSFNT(fileA, fileFinalA, 0, (ulong)(fileA->size()), indexA);
	else
		sfnt = new XWFontFileSFNT(fileA, fileFinalA, 0, (ulong)(fileA->size()));
		
	ulong offsetA = 0;
	switch (sfnt->type)
	{
		case SFNT_TYPE_TTC:
    		offsetA = sfnt->readTTCOffset(indexA);
    		break;
    		
  		case SFNT_TYPE_TRUETYPE:
    		if (indexA > 0) 
    		{
      			delete sfnt;
      			return 0;
    		} 
    		else 
      			offsetA = 0;
    		break;
    	
  		case SFNT_TYPE_DFONT:
    		offsetA = sfnt->offset;
    		break;
    	
  		default:
    		delete sfnt;
      		return 0;
    		break;
	}
	
	if (sfnt->readTableDirectory(offsetA) < 0)
	{
		delete sfnt;
		return 0;
	}
	
	return sfnt;
}

XWFontFileSFNT * XWFontFileSFNT::loadTrueType(char *fileName, 
	                                          int indexA, 
	                                          bool deleteFileA)
{
	int fileFinalA = FONTFILE_CLOSE | FONTFILE_DEL;
	if (deleteFileA)
		fileFinalA |= FONTFILE_REMOVE;
		
	QFile * fileA = open(fileName, XWFontSea::TrueType);
	if (!fileA)
		return 0;
	
	XWFontFileSFNT * sfnt = 0;
	if (isDFont(fileA))
		sfnt = new XWFontFileSFNT(fileA, fileFinalA, 0, (ulong)(fileA->size()), indexA);
	else
		sfnt = new XWFontFileSFNT(fileA, fileFinalA, 0, (ulong)(fileA->size()));
		
	if (sfnt->type != SFNT_TYPE_TRUETYPE &&
        sfnt->type != SFNT_TYPE_TTC &&
        sfnt->type != SFNT_TYPE_DFONT)
	{
		delete sfnt;
		sfnt = 0;
		return 0;
	}
	
	if (sfnt->type == SFNT_TYPE_TTC)
	{
		ulong offsetA = sfnt->readTTCOffset(indexA);
		if (offsetA == 0)
		{
			delete sfnt;
			sfnt = 0;
		}
		else
			sfnt->readTableDirectory(offsetA);
	}
	else
		sfnt->readTableDirectory(sfnt->offset);
	
	return sfnt;
}

XWFontFileSFNT * XWFontFileSFNT::loadType1C(char *fileName, 
	                                       bool deleteFileA)
{
	QFile * fileA = open(fileName, XWFontSea::OpenType);
	if (!fileA)
		return 0;
	
	int fileFinalA = FONTFILE_CLOSE | FONTFILE_DEL;
	if (deleteFileA)
		fileFinalA |= FONTFILE_REMOVE;
		
	XWFontFileSFNT * sfnt = new XWFontFileSFNT(fileA, fileFinalA, 0, (ulong)(fileA->size()));
	if ((sfnt->type != SFNT_TYPE_POSTSCRIPT) || 
		(sfnt->readTableDirectory(0) < 0) || 
		(((sfnt->findTablePos("CFF ")) < 1)))
	{
		delete sfnt;
		return 0;
	}
	
	return sfnt;
}

ulong XWFontFileSFNT::locateTable(const char *tagA)
{
	ulong offsetA = findTablePos(tagA);
	seek(offsetA);
	return offsetA;
}

XWFontFileCFF * XWFontFileSFNT::makeCIDType0(int indexA)
{
	if (type != SFNT_TYPE_POSTSCRIPT)
		return 0;
		
	ulong offsetA = findTablePos("CFF ");
	if (offsetA == 0)
		return 0;
		
	ulong lenA = (ulong)(file->size()) - offsetA;		
	XWFontFileCFF * cff = XWFontFileCFF::make(file, 0, offsetA, lenA, indexA);
	if (!cff)
		return 0;
		
	if (!cff->isCIDFont())
	{
		delete cff;
		return 0;
	}
	
	return cff;
}

XWFontFileCFF * XWFontFileSFNT::makeType1C(int indexA)
{
	if (type != SFNT_TYPE_POSTSCRIPT)
		return 0;
		
	ulong offsetA = findTablePos("CFF ");
	if (offsetA < 1)
		return 0;
		
	ulong lenA = (ulong)(file->size()) - offsetA;
	XWFontFileCFF * cff = XWFontFileCFF::make(file, 0, offsetA, lenA, indexA);
	if (cff->isCIDFont())
	{
		delete cff;
		cff = 0;
	}
	
	return cff;
}

XWSFNTCmap * XWFontFileSFNT::readCmap(ushort platform, ushort encoding)
{
	ulong offsetA = locateTable("cmap");
  	getUShort();
  	ushort n_subtabs = getUShort();
  	
  	ushort i = 0;
  	for (; i < n_subtabs; i++) 
  	{
    	ushort p_id = getUShort();
    	ushort e_id = getUShort();
    	if (p_id != platform || e_id != encoding)
      		getULong();
    	else 
    	{
      		offsetA += getULong();
      		break;
    	}
  	}

  	if (i == n_subtabs)
    	return 0;
    	
    seek(offsetA); 
    XWSFNTCmap * ret = new XWSFNTCmap(this, platform, encoding);
    if (ret->map == 0)
    {
    	delete ret;
    	xwApp->warning(tr("unrecognized cmap subtable format.\n"));
    	return 0;
    }
    
    return ret;
}

XWTTHeadTable * XWFontFileSFNT::readHeadTable()
{
	locateTable("head");
	return new XWTTHeadTable(this);
}

XWTTHHeaTable * XWFontFileSFNT::readHHeaTable()
{
	locateTable("hhea");
	return new XWTTHHeaTable(this);
}

TTLongMetrics * XWFontFileSFNT::readLongMetrics(ushort numGlyphs, 
	                                            ushort numLongMetrics, 
	                                            ushort numExSideBearings)
{
  	ushort gid, last_adv = 0;
  	short  last_esb = 0;

  	TTLongMetrics * m = (TTLongMetrics*)malloc(numGlyphs * sizeof(TTLongMetrics));
  	for (gid = 0; gid < numGlyphs; gid++) 
  	{
    	if (gid < numLongMetrics)
      		last_adv = getUShort();
    	if (gid < numLongMetrics + numExSideBearings)
      		last_esb = getShort();
    	m[gid].advance     = last_adv;
    	m[gid].sideBearing = last_esb;
  	}

  	return m;
}

XWTTMaxpTable * XWFontFileSFNT::readMaxpTable()
{
	locateTable("maxp");
	return new XWTTMaxpTable(this);
}

XWTTOs2Table * XWFontFileSFNT::readOs2Table()
{
	if (findTablePos("OS/2") > 0)
	{
		locateTable("OS/2");
		return new XWTTOs2Table(this);
	}
	
	return new XWTTOs2Table;
}

XWTTPostTable * XWFontFileSFNT::readPostTable()
{
	locateTable("post");
	XWTTPostTable * ret = new XWTTPostTable(this);
	if (ret->Version == 0x00025000UL)
		xwApp->warning(tr("TrueType 'post' version 2.5 found (deprecated)\n"));
	else if (ret->Version == 0)
	{
		delete ret;
		ret = 0;
		xwApp->warning(tr("invalid TrueType 'post' table.\n"));
	}
	
	return ret;
}

ulong  XWFontFileSFNT::readTTCOffset(int ttc_idx)
{
	if (!file && !ft_face)
		return -1;
		
	seek(4);
	
	getULong();
  	ulong  num_dirs = getULong();
  	if (ttc_idx < 0 || ttc_idx > (int)num_dirs - 1)
  	{
    	xwApp->error(tr("invalid TTC index number.\n"));
    	return -1;
    }

  	seek(12 + ttc_idx * 4);
  	ulong offsetA = getULong();

  	return offsetA;
}

XWTTVHeaTable * XWFontFileSFNT::readVHeaTable()
{
	locateTable("vhea");
	return new XWTTVHeaTable(this);
}

XWTTVORGTable * XWFontFileSFNT::readVORGTable()
{
	ulong offsetA = findTablePos("VORG");
	if (offsetA == 0)
		return 0;
		
	offsetA = locateTable("VORG");
	if (getUShort() != 1 || getUShort() != 0)
	{
     	xwApp->error("unsupported VORG version.\n");
     	return 0;
    }
      
	return new XWTTVORGTable(this);
}

int XWFontFileSFNT::requireTable(const char *tagA, int must_exist)
{
	return directory->requireTable(tagA, must_exist);
}


bool XWFontFileSFNT::seek(ulong offsetA)
{
	if (ft_face)
	{
		loc = offsetA;
		return true;
	}
	
	return file->seek(offsetA);
}


void XWFontFileSFNT::setTable(const char *tagA, void *data, ulong length)
{
	directory->setTable(tagA, data, length);
}

unsigned XWFontFileSFNT::log2floor(unsigned n)
{
	unsigned val = 0;

  	while (n > 1) 
  	{
    	n /= 2;
    	val++;
  	}

  	return val;
}

unsigned XWFontFileSFNT::max2floor(unsigned n)
{
	int val = 1;

  	while (n > 1) 
  	{
    	n   /= 2;
    	val *= 2;
  	}

  	return val;
}

int XWFontFileSFNT::readTableDirectory(ulong offsetA)
{
	if (directory)
		delete directory;
		
	seek(offsetA);
	directory = new XWSFNTTableDirectory(this, offset);
	return 0;
}


