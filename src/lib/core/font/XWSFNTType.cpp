/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWCMap.h"
#include "XWFontCache.h"
#include "XWSFNTType.h"

XWSFNTTableDirectory::XWSFNTTableDirectory()
{
	version = 0;
	num_tables = 0;
	search_range = 0;
	entry_selector = 0;
	range_shift = 0;
	num_kept_tables = 0;
	flags = 0;
	tables = 0;
}

XWSFNTTableDirectory::XWSFNTTableDirectory(XWFontFileSFNT * fileA, ulong offsetA)
{
	version      = fileA->getULong();
  	num_tables   = fileA->getUShort();
  	search_range = fileA->getUShort();
  	entry_selector = fileA->getUShort();
  	range_shift    = fileA->getUShort();
  	flags  = (char*)malloc(num_tables * sizeof(char));
  	tables = (SFNTTable*)malloc(num_tables * sizeof(SFNTTable));
  	
  	for (int i = 0; i < num_tables; i++) 
  	{
    	ulong u_tag = fileA->getULong();

    	convertTag(tables[i].tag, u_tag);
    	tables[i].checksum = fileA->getULong();
    	tables[i].offset    = fileA->getULong() + offsetA;
    	tables[i].length    = fileA->getULong();
    	tables[i].data      = 0;

    	flags[i] = 0;
  	}
  	
  	num_kept_tables = 0;
}

XWSFNTTableDirectory::~XWSFNTTableDirectory()
{
	if (flags)
		free(flags);
		
	if (tables)
	{
		for (int i = 0; i < num_tables; i++)
		{
			if (tables[i].data)
				free(tables[i].data);
		}
			
		free(tables);
	}
}

ulong XWSFNTTableDirectory::calcCheckSum(void *data, ulong length)
{
	ulong  chksum = 0;
  	int    count  = 0;
  	uchar * p      = (uchar *) data;
  	uchar * endptr = p + length;
  	while (p < endptr) 
  	{
    	chksum += (p[0] << (8 * ( 3 - count)));
    	count   = ((count + 1) & 3);
    	p++;
  	}

  	return chksum;
}

int XWSFNTTableDirectory::findTableIndex(const char *tagA)
{
	for (int i = 0; i < num_tables; i++) 
	{
    	if (!memcmp(tables[i].tag, tagA, 4))
      		return i;
  	}

  	return -1;
}

ulong XWSFNTTableDirectory::findTableLen(const char *tagA)
{
	ulong length = 0;
	int idx = findTableIndex(tagA);
	if (idx >= 0)
		length = tables[idx].length;
		
	return length;
}

ulong XWSFNTTableDirectory::findTablePos(const char *tagA)
{
	ulong offsetA = 0;
	int idx = findTableIndex(tagA);
	if (idx >= 0)
		offsetA = tables[idx].offset;
		
	return offsetA;
}

int XWSFNTTableDirectory::requireTable(const char *tagA, int must_exist)
{
	int idx = findTableIndex(tagA);
  	if (idx < 0) 
  	{
    	if (must_exist)
      		return -1;
  	} 
  	else 
  	{
    	flags[idx] |= SFNT_TABLE_REQUIRED;
    	num_kept_tables++;
  	}

  	return 0;
}

void XWSFNTTableDirectory::setTable(const char *tagA, void *data, ulong length)
{
	int idx = findTableIndex(tagA);

  	if (idx < 0) 
  	{
    	idx = num_tables;
    	num_tables++;
    	tables = (SFNTTable*)realloc(tables, num_tables * sizeof(SFNTTable));
    	memcpy(tables[idx].tag, tagA, 4);
  	}

  	tables[idx].checksum = calcCheckSum(data, length);
  	tables[idx].offset    = 0L;
  	tables[idx].length    = length;
  	tables[idx].data      = data;
}

XWSFNTCmap::XWSFNTCmap()
{
	platform = 0;
	encoding = 0;
	format = 0;
	language = 0;
	map = 0;
}

XWSFNTCmap::XWSFNTCmap(XWFontFileSFNT * fileA, ushort platformA, ushort encodingA)
{
	platform = platformA;
    encoding = encodingA;
    
    map = 0;
	format = fileA->getUShort();
	ulong lenA = 0;
	if (format <= 6)
	{
		lenA = fileA->getUShort();
		language = fileA->getUShort();
	}
	else
	{
		if (fileA->getUShort() != 0)
			return ;
		
		lenA = fileA->getULong();
		language = fileA->getULong();
	}
	
	switch (format)
	{
		case 0:
			{
				SFNTCmap0 * map0 = (SFNTCmap0*)malloc(sizeof(SFNTCmap0));
    			map = map0;
    			for (int i = 0; i < 256; i++)
    				map0->glyphIndexArray[i] = fileA->getByte();
    		}
    		break;
    		
  		case 2:
  			{
  				SFNTCmap2 * map2 = (SFNTCmap2*)malloc(sizeof(SFNTCmap2));
  				map = map2;
  				int i = 0;
  				for (; i < 256; i++)
    				map2->subHeaderKeys[i] = fileA->getUShort();
    				
    			int n = 0; 
    			i = 0;
    			for (; i < 256; i++) 
    			{
    				map2->subHeaderKeys[i] /= 8;
    				if (n < map2->subHeaderKeys[i])
      					n = map2->subHeaderKeys[i];
  				}
  				n += 1;
  				
  				map2->subHeaders = (SFNTSubHeader*)malloc(n * sizeof(SFNTSubHeader)); 
  				for (i = 0; i < n; i++) 
  				{
    				map2->subHeaders[i].firstCode     = fileA->getUShort();
    				map2->subHeaders[i].entryCount    = fileA->getUShort();
    				map2->subHeaders[i].idDelta       = fileA->getShort();
    				map2->subHeaders[i].idRangeOffset = fileA->getUShort();
    				if (map2->subHeaders[i].idRangeOffset != 0)
      					map2->subHeaders[i].idRangeOffset -= (2 + (n - i - 1) * 8);
  				}
  				
  				n = (ushort) (lenA - 518 - n * 8) / 2;

  				map2->glyphIndexArray = (ushort*)malloc(n * sizeof(ushort));
  				for (i = 0; i < n; i++)
    				map2->glyphIndexArray[i] = fileA->getUShort();
    		}
    		break;
    		
  		case 4:
    		{
    			SFNTCmap4 * map4 = (SFNTCmap4*)malloc(sizeof(SFNTCmap4));
    			map = map4;
    			
    			ushort segCount = 0;
    			map4->segCountX2    = segCount = fileA->getUShort();
  				map4->searchRange   = fileA->getUShort();
  				map4->entrySelector = fileA->getUShort();
  				map4->rangeShift    = fileA->getUShort();
  				segCount /= 2;

  				map4->endCount = (ushort*)malloc(segCount * sizeof(ushort));
  				for (int i = 0; i < segCount; i++)
    				map4->endCount[i] = fileA->getUShort();

  				map4->reservedPad = fileA->getUShort();
  				map4->startCount = (ushort*)malloc(segCount * sizeof(ushort));
  				for (int i = 0; i < segCount; i++)
    				map4->startCount[i] = fileA->getUShort();

  				map4->idDelta = (ushort*)malloc(segCount * sizeof(ushort));
  				for (int i = 0; i < segCount; i++)
    				map4->idDelta[i] = fileA->getUShort();

  				map4->idRangeOffset = (ushort*)malloc(segCount * sizeof(ushort));
  				for (int i = 0; i < segCount; i++)
    				map4->idRangeOffset[i] = fileA->getUShort();

  				int n = (lenA - 16 - 8 * segCount) / 2;
  				if (n == 0)
    				map4->glyphIndexArray = 0;
  				else 
  				{
    				map4->glyphIndexArray = (ushort*)malloc(n * sizeof(ushort));
    				for (int i = 0; i < n; i++)
      					map4->glyphIndexArray[i] = fileA->getUShort();
  				}
    		}
    		break;
    		
  		case 6:
  			{
  				SFNTCmap6 * map6 = (SFNTCmap6*)malloc(sizeof(SFNTCmap6));
  				map = map6;
  				map6->firstCode       = fileA->getUShort();
  				map6->entryCount      = fileA->getUShort();
  				map6->glyphIndexArray = (ushort*)malloc(map6->entryCount * sizeof(ushort));
  
  				for (int i = 0; i < map6->entryCount; i++)
    				map6->glyphIndexArray[i] = fileA->getUShort();
  			}
    		break;
    		
  		case 12:
    		{
    			SFNTCmap12 * map12 = (SFNTCmap12*)malloc(sizeof(SFNTCmap12));
    			map = map12;
    			map12->nGroups = fileA->getULong();
  				map12->groups  = (SFNTCharGroup*)malloc(map12->nGroups * sizeof(SFNTCharGroup));

  				for (ulong i = 0; i < map12->nGroups; i++) 
  				{
    				map12->groups[i].startCharCode = fileA->getULong();
    				map12->groups[i].endCharCode   = fileA->getULong();
    				map12->groups[i].startGlyphID  = fileA->getULong();
  				}
    		}
    		break;
    		
  		default:
    		break;
	}
}

XWSFNTCmap::~XWSFNTCmap()
{
	if (map)
	{
		switch (format)
		{
			case 2:
				{
					SFNTCmap2 * map2 = (SFNTCmap2*)map;
					if (map2->subHeaders)
						free(map2->subHeaders);
						
					if (map2->glyphIndexArray)
						free(map2->glyphIndexArray);
				}
				break;
				
			case 4:
				{
					SFNTCmap4 * map4 = (SFNTCmap4*)map;
					if (map4->endCount)
						free(map4->endCount);
								
					if (map4->startCount)
						free(map4->startCount);
								
					if (map4->idDelta)
						free(map4->idDelta);
								
					if (map4->idRangeOffset)
						free(map4->idRangeOffset);
								
					if (map4->glyphIndexArray)
						free(map4->glyphIndexArray);
				}
				break;
				
			case 6:
				{
					SFNTCmap6 * map6 = (SFNTCmap6*)(map);
					if (map6->glyphIndexArray)
						free(map6->glyphIndexArray);
				}
				break;
						
			case 12:
				{
					SFNTCmap12 * map12 = (SFNTCmap12*)(map);
					if (map12->groups)
						free(map12->groups);
				}
				break;
						
			default:
				break;
		}
		
		free(map);
	}
}

void XWSFNTCmap::loadCMap4(uchar *GIDToCIDMap, XWCMap *cmap)
{
	SFNTCmap4 * map4 = (SFNTCmap4*)map;
	ushort segCount = map4->segCountX2 / 2;
	uchar wbuf[1204];
	ushort gid = 0;
	ushort cid = 0;
	for (long i = segCount - 1; i >= 0 ; i--)
	{
		ushort c0 = map4->startCount[i];
    	ushort c1 = map4->endCount[i];
    	ushort d  = map4->idRangeOffset[i] / 2 - (segCount - i);
    	for (ushort j = 0; j <= c1 - c0; j++)
    	{
    		ushort ch = c0 + j;
    		if (map4->idRangeOffset[i] == 0) 
				gid = (ch + map4->idDelta[i]) & 0xffff;
      		else 
				gid = (map4->glyphIndexArray[j+d] + map4->idDelta[i]) & 0xffff;
				
				if (gid != 0 && gid != 0xffff)
				{
					if (GIDToCIDMap)
					{
						cid = ((GIDToCIDMap[2*gid] << 8)|GIDToCIDMap[2*gid+1]);
					}
					else
						cid = gid;
				
					wbuf[0] = 0;
					wbuf[1] = 0;
					wbuf[2] = (ch >> 8) & 0xff;
					wbuf[3] =  ch & 0xff;
					cmap->addCIDChar(wbuf, 4, cid);
				}
    	}
	}
}

void XWSFNTCmap::loadCMap12(uchar *GIDToCIDMap, XWCMap *cmap)
{
	SFNTCmap12 * map12 = (SFNTCmap12*)map;
	ushort  gid, cid;
	uchar wbuf[1204];
	for (ulong i = 0; i < map12->nGroups; i++)
	{
		for (ulong ch = map12->groups[i].startCharCode; ch <= map12->groups[i].endCharCode; ch++)
		{
			long  d = ch - map12->groups[i].startCharCode;
      		gid = (ushort) ((map12->groups[i].startGlyphID + d) & 0xffff);
      		if (GIDToCIDMap)
      			cid = ((GIDToCIDMap[2*gid] << 8)|GIDToCIDMap[2*gid+1]);
      		else
      			cid = gid;
      		
      		wbuf[0] = (ch >> 24) & 0xff;
      		wbuf[1] = (ch >> 16) & 0xff;
      		wbuf[2] = (ch >>  8) & 0xff;
      		wbuf[3] = ch & 0xff;
      		cmap->addCIDChar(wbuf, 4, cid);
		}
	}
}

ushort XWSFNTCmap::lookup(long cc)
{
	if (cc > 0xffffL && format < 12) 
    	return 0;
    	
    ushort gid = 0;
  	switch (format) 
  	{
  		case 0:
  			{
  				SFNTCmap0 * map0 = (SFNTCmap0 * )map;
  				gid = (cc > 255) ? 0 : map0->glyphIndexArray[cc];
  			}
    		break;
    		
  		case 2:
  			{
  				SFNTCmap2 *map2 = (SFNTCmap2 *)map;
    			int hi = (cc >> 8) & 0xff;
  				int lo = cc & 0xff;
  	
  				ushort i = map2->subHeaderKeys[hi];

  				ushort firstCode  = map2->subHeaders[i].firstCode;
  				ushort entryCount = map2->subHeaders[i].entryCount;
  				short idDelta     = map2->subHeaders[i].idDelta;
  				ushort idRangeOffset = map2->subHeaders[i].idRangeOffset / 2;
  	
  				if (lo >= firstCode && lo < firstCode + entryCount) 
  				{
    				idRangeOffset += lo - firstCode;
    				gid = map2->glyphIndexArray[idRangeOffset];
    				if (gid != 0)
      					gid = (gid + idDelta) & 0xffff;
  				}
    		}
    		break;
    		
  		case 4:
    		{
    			SFNTCmap4 *map4 = (SFNTCmap4 *)map;
    			ushort segCount = 0;
				ushort i = segCount = map4->segCountX2 / 2;
  				while (i-- > 0 &&  cc <= map4->endCount[i]) 
  				{
    				if (cc >= map4->startCount[i]) 
    				{
      					if (map4->idRangeOffset[i] == 0) 
							gid = (cc + map4->idDelta[i]) & 0xffff;
      					else 
      					{
							ushort j  = map4->idRangeOffset[i] - (segCount - i) * 2;
							j  = (cc - map4->startCount[i]) + (j / 2);
							gid = map4->glyphIndexArray[j];
							if (gid != 0)
	  							gid = (gid + map4->idDelta[i]) & 0xffff;
      					}
      					break;
    				}
  				}
    		}
    		break;
    		
  		case 6:
    		{
    			SFNTCmap6 *map6 = (SFNTCmap6 *)map;
    			ushort idx = cc - map6->firstCode; 
  				if (idx < map6->entryCount)
    				gid = map6->glyphIndexArray[idx];
    		}
    		break;
    		
  		case 12:
    		{
    			SFNTCmap12 *map12 = (SFNTCmap12 *)map;
    			ulong i = map12->nGroups;
    			ulong cccc = (ulong)cc;
  				while (i-- >= 0 && cccc <= map12->groups[i].endCharCode) 
  				{
    				if (cccc >= map12->groups[i].startCharCode) 
    				{
      					gid = (ushort) ((cccc - map12->groups[i].startCharCode + map12->groups[i].startGlyphID) & 0xffff);
      					break;
    				}
  				}
    		}
    		break;
    		
  		default:
    		break;
  	}

  	return gid;
}


#define NUM_GLYPH_LIMIT        65534
#define TABLE_DATA_ALLOC_SIZE  40960
#define GLYPH_ARRAY_ALLOC_SIZE 256

#define ARG_1_AND_2_ARE_WORDS     (1 << 0)
#define ARGS_ARE_XY_VALUES        (1 << 1)
#define ROUND_XY_TO_GRID          (1 << 2)
#define WE_HAVE_A_SCALE           (1 << 3)
#define RESERVED                  (1 << 4)
#define MORE_COMPONENT            (1 << 5)
#define WE_HAVE_AN_X_AND_Y_SCALE  (1 << 6)
#define WE_HAVE_A_TWO_BY_TWO      (1 << 7)
#define WE_HAVE_INSTRUCTIONS      (1 << 8)
#define USE_MY_METRICS            (1 << 9)

static int 
glyf_cmp (const void *v1, const void *v2)
{
  	TTGlyphDesc * sv1 = (TTGlyphDesc *) v1;
  	TTGlyphDesc * sv2 = (TTGlyphDesc *) v2;
	
	int cmp = 0;
  	if (sv1->gid == sv2->gid)
    	cmp = 0;
  	else if (sv1->gid < sv2->gid)
    	cmp = -1;
  	else
    	cmp = 1;
    
  	return cmp;
}

XWTTGlyphs::XWTTGlyphs()
{
	num_glyphs  = 0;
  	max_glyphs  = 0;
  	last_gid    = 0;
  	emsize      = 1;
  	default_advh = 0;
  	default_tsb  = 0;
  	gd = 0;
  	used_slot = (uchar*)malloc(8192 * sizeof(uchar));
  	memset(used_slot, 0, 8192);
  	addGlyph(0, 0);
}

XWTTGlyphs::~XWTTGlyphs()
{
	if (gd)
	{
		for (ushort idx = 0; idx < num_glyphs; idx++) 
		{
			if (gd[idx].data)
	  			free(gd[idx].data);
      	}
      	free(gd);
	}
	
	if (used_slot)
		free(used_slot);
}

ushort XWTTGlyphs::addGlyph(ushort gid, ushort new_gid)
{
	if (!(used_slot[new_gid/8] & (1 << (7 - (new_gid % 8))))) 
	{
    	if (num_glyphs+1 >= NUM_GLYPH_LIMIT)
      		return 0;

    	if (num_glyphs >= max_glyphs) 
    	{
      		max_glyphs += GLYPH_ARRAY_ALLOC_SIZE;
      		gd = (TTGlyphDesc*)realloc(gd, max_glyphs * sizeof(TTGlyphDesc));
    	}
    	
    	gd[num_glyphs].gid  = new_gid;
    	gd[num_glyphs].ogid = gid;
    	gd[num_glyphs].length = 0;
    	gd[num_glyphs].data   = 0;
    	used_slot[new_gid/8] |= (1 << (7 - (new_gid % 8)));
    	num_glyphs += 1;
  	}

  	if (new_gid > last_gid) 
    	last_gid = new_gid;

  	return new_gid;
}

int XWTTGlyphs::buildTables(XWFontFileSFNT * sfont)
{
	if (num_glyphs > NUM_GLYPH_LIMIT)
		return -1;
		
	XWTTHeadTable * head = sfont->readHeadTable();
	XWTTHHeaTable * hhea = sfont->readHHeaTable();
	XWTTMaxpTable * maxp = sfont->readMaxpTable();
	if (hhea->metricDataFormat != 0)
	{
		delete head;
		delete hhea;
		delete maxp;
		return -1;
	}
	
	emsize = head->unitsPerEm;
	sfont->locateTable("hmtx");
	TTLongMetrics * hmtx = sfont->readLongMetrics(maxp->numGlyphs, 
	                                              hhea->numOfLongHorMetrics, 
	                                              hhea->numOfExSideBearings);
	XWTTOs2Table  * os2 = sfont->readOs2Table();
  	default_advh = os2->sTypoAscender - os2->sTypoDescender;
  	default_tsb  = default_advh - os2->sTypoAscender;
  	
  	TTLongMetrics * vmtx = 0;
  	if (sfont->findTablePos("vmtx") > 0) 
  	{
    	XWTTVHeaTable * vhea = sfont->readVHeaTable();
    	sfont->locateTable("vmtx");
    	vmtx = sfont->readLongMetrics(maxp->numGlyphs, vhea->numOfLongVerMetrics, vhea->numOfExSideBearings);
    	delete vhea;
  	}
  	
  	sfont->locateTable("loca");
  	ulong * location = new ulong[maxp->numGlyphs + 1];
  	if (head->indexToLocFormat == 0) 
  	{
    	for (ushort i = 0; i <= maxp->numGlyphs; i++)
      		location[i] = 2*((ulong)(sfont->getUShort()));
  	} 
  	else if (head->indexToLocFormat == 1) 
  	{
    	for (ushort i = 0; i <= maxp->numGlyphs; i++)
      		location[i] = sfont->getULong();
  	} 
  	else 
  	{
    	delete head;
		delete hhea;
		delete maxp;
		delete os2;
		if (vmtx)
			free(vmtx);
			
		free(hmtx);
		delete [] location;
		return -1;
  	}
  	
  	ushort * w_stat = new ushort[emsize+2];
  	memset(w_stat, 0, sizeof(ushort)*(emsize+2));
  	ulong offset = sfont->locateTable("glyf");
  	for (long i = 0; i < NUM_GLYPH_LIMIT; i++)
  	{
  		if (i >= num_glyphs)
      		break;
      		
      	ushort gid = gd[i].ogid;
    	if (gid >= maxp->numGlyphs)
    	{
    		delete head;
			delete hhea;
			delete maxp;
			delete os2;
			if (vmtx)
				free(vmtx);
			
			free(hmtx);
			delete [] location;
			delete [] w_stat;
			return -1;
    	}
    	
    	ulong loc = location[gid];
    	ulong len = location[gid+1] - loc;
    	gd[i].advw = hmtx[gid].advance;
    	gd[i].lsb  = hmtx[gid].sideBearing;
    	if (vmtx) 
    	{
      		gd[i].advh = vmtx[gid].advance;
      		gd[i].tsb  = vmtx[gid].sideBearing;
    	} 
    	else 
    	{
      		gd[i].advh = default_advh;
      		gd[i].tsb  = default_tsb;
    	}
    	gd[i].length = len;
    	gd[i].data   = 0;
    	if (gd[i].advw <= emsize) 
      		w_stat[gd[i].advw] += 1;
    	else 
      		w_stat[emsize+1]   += 1;
      		
      	if (len == 0)
      		continue;
      	else if (len < 10)
      	{
      		delete head;
			delete hhea;
			delete maxp;
			delete os2;
			if (vmtx)
				free(vmtx);
			
			free(hmtx);
			delete [] location;
			delete [] w_stat;
			return -1;
      	}
      	
      	uchar * p = (uchar*)malloc(len * sizeof(uchar));
      	gd[i].data = p;
    	uchar * endptr = p + len;

    	sfont->seek(offset+loc);
    	short number_of_contours = sfont->getShort();
    	p += XWFontFile::putShort(p, number_of_contours);

    	gd[i].llx = sfont->getShort();
    	gd[i].lly = sfont->getShort();
    	gd[i].urx = sfont->getShort();
    	gd[i].ury = sfont->getShort();
    	
    	if (!vmtx)
    		gd[i].tsb = default_advh - default_tsb - gd[i].ury;
    		
    	p += XWFontFile::putShort(p, gd[i].llx);
    	p += XWFontFile::putShort(p, gd[i].lly);
    	p += XWFontFile::putShort(p, gd[i].urx);
    	p += XWFontFile::putShort(p, gd[i].ury);
    		
    	sfont->read((char*)p, len - 10);
    	
    	if (number_of_contours < 0)
    	{
    		ushort flags, cgid, new_gid;
    		do
    		{
    			if (p >= endptr)
    			{
    				delete head;
					delete hhea;
					delete maxp;
					delete os2;
					if (vmtx)
						free(vmtx);
					free(hmtx);
					delete [] location;
					delete [] w_stat;
					return -1;
    			}
    			
    			flags = ((*p) << 8)| *(p+1);
				p += 2;
				cgid  = ((*p) << 8)| *(p+1);
				
				if (cgid >= maxp->numGlyphs)
				{
					delete head;
					delete hhea;
					delete maxp;
					delete os2;
					if (vmtx)
						free(vmtx);
					free(hmtx);
					delete [] location;
					delete [] w_stat;
					return -1;
				}
				
				new_gid = findGlyph(cgid);
				if (new_gid == 0) 
	  				new_gid = addGlyph(cgid, findEmptySlot());
	  				
				p += XWFontFile::putUShort(p, new_gid);    			
    			p += (flags & ARG_1_AND_2_ARE_WORDS) ? 4 : 2;
    			if (flags & WE_HAVE_A_SCALE)
    				p += 2;
    			else if (flags & WE_HAVE_AN_X_AND_Y_SCALE)
    				p += 4;
    			else if (flags & WE_HAVE_A_TWO_BY_TWO)
    				p += 8;
    		} while(flags & MORE_COMPONENT);
    	}
  	}
  	
  	delete [] location;
  	free(hmtx);
  	if (vmtx)
  		free(vmtx);
  		
  	{
    	int max_count = -1;

    	dw = gd[0].advw;
    	for (int i = 0; i < emsize + 1; i++) 
    	{
      		if (w_stat[i] > max_count) 
      		{
				max_count = w_stat[i];
				dw = i;
      		}
    	}
  	}
  	
  	delete [] w_stat;
  	qsort(gd, num_glyphs, sizeof(TTGlyphDesc), glyf_cmp);
  	
  	{
  		ulong glyf_table_size = 0UL;
    	int num_hm_known = 0;
    	ushort last_advw = gd[num_glyphs - 1].advw;
    	for (int i = num_glyphs - 1; i >= 0; i--)
    	{
    		int padlen = (gd[i].length % 4) ? (4 - (gd[i].length % 4)) : 0;
      		glyf_table_size += gd[i].length + padlen;
      		if (!num_hm_known && last_advw != gd[i].advw) 
      		{
				hhea->numOfLongHorMetrics = gd[i].gid + 2;
				num_hm_known = 1;
      		}
    	}
    	
    	if (!num_hm_known) 
      		hhea->numOfLongHorMetrics = 1;
      		
    	ulong hmtx_table_size = hhea->numOfLongHorMetrics * 2 + (last_gid + 1) * 2;
    	ulong loca_table_size = 0;
    	if (glyf_table_size < 0x20000UL) 
    	{
      		head->indexToLocFormat = 0;
      		loca_table_size = (last_gid + 2)*2;
    	} 
    	else 
    	{
      		head->indexToLocFormat = 1;
      		loca_table_size = (last_gid + 2)*4;
    	}

		char * p = (char*)malloc(hmtx_table_size * sizeof(char));
		char * hmtx_table_data = p;
		char * q = (char*)malloc(loca_table_size * sizeof(char));
    	char * loca_table_data = q;
    	char * glyf_table_data = (char*)malloc(glyf_table_size * sizeof(char));
    	
    	offset = 0UL; 
    	ushort prev = 0;
    	for (ushort i = 0; i < num_glyphs; i++)
    	{
    		long gap = (long) gd[i].gid - prev - 1;
    		for (long j = 1; j <= gap; j++)
    		{
    			if (prev + j == hhea->numOfLongHorMetrics - 1) 
	  				p += XWFontFile::putUShort(p, last_advw);
				else if (prev + j < hhea->numOfLongHorMetrics) 
	  				p += XWFontFile::putUShort(p, 0);
				p += XWFontFile::putShort(p, 0);
				if (head->indexToLocFormat == 0) 
	  				q += XWFontFile::putUShort(q, (ushort) (offset/2));
				else 
	  				q += XWFontFile::putULong(q, offset);
    		}
    		
    		int padlen = (gd[i].length % 4) ? (4 - (gd[i].length % 4)) : 0;
      		if (gd[i].gid < hhea->numOfLongHorMetrics)
      			p += XWFontFile::putUShort(p, gd[i].advw);
      				
      		p += XWFontFile::putShort(p, gd[i].lsb);
      		if (head->indexToLocFormat == 0) 
				q += XWFontFile::putUShort(q, (ushort)(offset/2));
      		else 
				q += XWFontFile::putULong(q, offset);
      		
      		memset(glyf_table_data + offset, 0, gd[i].length + padlen);
      		memcpy(glyf_table_data + offset, gd[i].data, gd[i].length);
      		offset += gd[i].length + padlen;
      		prev    = gd[i].gid;
      		/* free data here since it consume much memory */
      		if (gd[i].data)
      			free(gd[i].data);
      		gd[i].length = 0; 
      		gd[i].data = 0;
    	}
    	
    	if (head->indexToLocFormat == 0) 
      		q += XWFontFile::putUShort(q, (ushort) (offset/2));
    	else 
      		q += XWFontFile::putULong(q, offset);

    	sfont->setTable("hmtx", (char *) hmtx_table_data, hmtx_table_size);
    	sfont->setTable("loca", (char *) loca_table_data, loca_table_size);
    	sfont->setTable("glyf", (char *) glyf_table_data, glyf_table_size);
  	}
  	
  	head->checkSumAdjustment = 0;
  	maxp->numGlyphs          = last_gid + 1;
  	
  	sfont->setTable("maxp", maxp->pack(), TT_MAXP_TABLE_SIZE);
  	sfont->setTable("hhea", hhea->pack(), TT_HHEA_TABLE_SIZE);
  	sfont->setTable("head", head->pack(), TT_HEAD_TABLE_SIZE);
  	delete maxp;
  	delete hhea;
  	delete head;
  	delete os2;

  	return 0;
}

ushort XWTTGlyphs::findGlyph(ushort gid)
{
	ushort new_gid = 0;
	for (ushort idx = 0; idx < num_glyphs; idx++) 
	{
    	if (gid == gd[idx].ogid) 
    	{
      		new_gid = gd[idx].gid;
      		break;
    	}
  	}

  	return new_gid;
}

ushort XWTTGlyphs::getIndex(ushort gid)
{
	ushort idx = 0;
	for (; idx < num_glyphs; idx++) 
	{
    	if (gid == gd[idx].gid)
      		break;
  	}
  	
  	if (idx == num_glyphs)
    	idx = 0;

  	return idx;
}

int XWTTGlyphs::getMetrics(XWFontFileSFNT *sfont)
{
	XWTTHeadTable * head = sfont->readHeadTable();
	XWTTHHeaTable * hhea = sfont->readHHeaTable();
	XWTTMaxpTable * maxp = sfont->readMaxpTable();
	if (hhea->metricDataFormat != 0)
	{
		delete head;
		delete hhea;
		delete maxp;
		return -1;
	}
	
	emsize = head->unitsPerEm;
	
	sfont->locateTable("hmtx");
  	TTLongMetrics * hmtx = sfont->readLongMetrics(maxp->numGlyphs, 
  	                                              hhea->numOfLongHorMetrics, 
  	                                              hhea->numOfExSideBearings);

  	XWTTOs2Table  * os2 = sfont->readOs2Table();
  	default_advh = os2->sTypoAscender - os2->sTypoDescender;
  	default_tsb  = default_advh - os2->sTypoAscender;
  	
  	TTLongMetrics * vmtx = 0;
  	if (sfont->findTablePos("vmtx") > 0) 
  	{
    	XWTTVHeaTable * vhea = sfont->readVHeaTable();
    	sfont->locateTable("vmtx");
    	vmtx = sfont->readLongMetrics(maxp->numGlyphs, vhea->numOfLongVerMetrics, vhea->numOfExSideBearings);
    	delete vhea;
  	} 
  	
  	sfont->locateTable("loca");
  	ulong * location = new ulong[maxp->numGlyphs + 1];
  	if (head->indexToLocFormat == 0) 
  	{
    	for (ushort i = 0; i <= maxp->numGlyphs; i++)
      		location[i] = 2*((ulong)(sfont->getUShort()));
  	} 
  	else if (head->indexToLocFormat == 1) 
  	{
    	for (ushort i = 0; i <= maxp->numGlyphs; i++)
      		location[i] = sfont->getULong();
  	} 
  	else 
  	{
    	delete head;
		delete hhea;
		delete maxp;
		delete os2;
		delete [] location;
		if (vmtx)
			free(vmtx);
		free(hmtx);
		return -1;
  	}

  	ushort * w_stat = new ushort[emsize+2];
  	memset(w_stat, 0, sizeof(ushort)*(emsize+2));
  	ulong offset = sfont->locateTable("glyf");
  	for (ushort i = 0; i < num_glyphs; i++)
  	{
  		ushort gid = gd[i].ogid;
    	if (gid >= maxp->numGlyphs)
    	{
      		delete head;
			delete hhea;
			delete maxp;
			delete os2;
			delete [] location;
			delete [] w_stat;
			if (vmtx)
				free(vmtx);
			free(hmtx);
			return -1;
      	}
      	
      	ulong loc = location[gid];
    	ulong len = location[gid+1] - loc;
    	gd[i].advw = hmtx[gid].advance;
    	gd[i].lsb  = hmtx[gid].sideBearing;
    	if (vmtx) 
    	{
      		gd[i].advh = vmtx[gid].advance;
      		gd[i].tsb  = vmtx[gid].sideBearing;
    	} 
    	else 
    	{
      		gd[i].advh = default_advh;
      		gd[i].tsb  = default_tsb;
    	}
    	gd[i].length = len;
    	gd[i].data   = 0;

    	if (gd[i].advw <= emsize) 
      		w_stat[gd[i].advw] += 1;
    	else 
      		w_stat[emsize+1]   += 1;

    	if (len == 0) 
      		continue;
    	else if (len < 10) 
    	{
      		delete head;
			delete hhea;
			delete maxp;
			delete os2;
			delete [] location;
			delete [] w_stat;
			if (vmtx)
				free(vmtx);
			free(hmtx);
			return -1;
    	}

    	sfont->seek(offset+loc);
    	sfont->getShort();
    	
    	gd[i].llx = sfont->getShort();
    	gd[i].lly = sfont->getShort();
    	gd[i].urx = sfont->getShort();
    	gd[i].ury = sfont->getShort();
    	
    	if (!vmtx)
    		gd[i].tsb = default_advh - default_tsb - gd[i].ury;
  	}
  	
  	delete head;
	delete hhea;
	delete maxp;
	delete os2;
	delete [] location;
	
	if (vmtx)
		free(vmtx);
	free(hmtx);
		
	{
    	int max_count = -1;

    	dw = gd[0].advw;
    	for (ushort i = 0; i < emsize + 1; i++) 
    	{
      		if (w_stat[i] > max_count) 
      		{
				max_count = w_stat[i];
				dw = i;
      		}
    	}
  	}
		
	delete [] w_stat;
	return 0;
}

ushort XWTTGlyphs::findEmptySlot()
{
	ushort gid = 0;
	for (; gid < NUM_GLYPH_LIMIT; gid++) 
	{
    	if (!(used_slot[gid/8] & (1 << (7 - (gid % 8)))))
      		break;
  	}
  	
  	if (gid == NUM_GLYPH_LIMIT)
  		gid = 0;

  	return gid;
}


static const char *const
macglyphorder[258] = {
  /* 0x0000 */
  ".notdef", ".null", "nonmarkingreturn", "space", "exclam", "quotedbl",
  "numbersign", "dollar", "percent", "ampersand", "quotesingle",
  "parenleft", "parenright", "asterisk", "plus", "comma",
  /* 0x0010 */
  "hyphen", "period", "slash", "zero", "one", "two", "three", "four",
  "five", "six", "seven", "eight", "nine", "colon", "semicolon", "less",
  /* 0x0020 */
  "equal", "greater", "question", "at", "A", "B", "C", "D",
  "E", "F", "G", "H", "I", "J", "K", "L",
  /* 0x0030 */
  "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X",
  "Y", "Z", "bracketleft", "backslash",
  /* 0x0040 */
  "bracketright", "asciicircum", "underscore", "grave",
  "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l",
  /* 0x0050 */
  "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x",
  "y", "z", "braceleft", "bar",
  /* 0x0060 */
  "braceright", "asciitilde", "Adieresis", "Aring", "Ccedilla",
  "Eacute", "Ntilde", "Odieresis", "Udieresis", "aacute", "agrave",
  "acircumflex", "adieresis", "atilde", "aring", "ccedilla",
  /* 0x0070 */
  "eacute", "egrave", "ecircumflex", "edieresis", "iacute", "igrave",
  "icircumflex", "idieresis", "ntilde", "oacute", "ograve", "ocircumflex",
  "odieresis", "otilde", "uacute", "ugrave",
  /* 0x0080 */
  "ucircumflex", "udieresis", "dagger", "degree", "cent", "sterling",
  "section", "bullet", "paragraph", "germandbls", "registered",
  "copyright", "trademark", "acute", "dieresis", "notequal",
  /* 0x0090 */
  "AE", "Oslash", "infinity", "plusminus", "lessequal",	"greaterequal",
  "yen", "mu", "partialdiff", "summation", "product", "pi", "integral",
  "ordfeminine", "ordmasculine", "Omega",
  /* 0x00a0 */
  "ae", "oslash", "questiondown", "exclamdown", "logicalnot", "radical",
  "florin", "approxequal", "Delta", "guillemotleft", "guillemotright",
  "ellipsis", "nonbreakingspace", "Agrave", "Atilde", "Otilde",
  /* 0x00b0 */
  "OE", "oe", "endash", "emdash", "quotedblleft", "quotedblright",
  "quoteleft", "quoteright", "divide", "lozenge", "ydieresis",
  "Ydieresis", "fraction", "currency", "guilsinglleft", "guilsinglright",
  /* 0x00c0 */
  "fi", "fl", "daggerdbl", "periodcentered", "quotesinglbase",
  "quotedblbase", "perthousand", "Acircumflex",	 "Ecircumflex", "Aacute",
  "Edieresis", "Egrave", "Iacute", "Icircumflex", "Idieresis", "Igrave",
  /* 0x00d0 */
  "Oacute", "Ocircumflex", "apple", "Ograve", "Uacute", "Ucircumflex",
  "Ugrave", "dotlessi", "circumflex", "tilde", "macron", "breve",
  "dotaccent", "ring", "cedilla", "hungarumlaut",
  /* 0x00e0 */
  "ogonek", "caron", "Lslash", "lslash", "Scaron", "scaron", "Zcaron",
  "zcaron", "brokenbar", "Eth", "eth", "Yacute", "yacute", "Thorn",
  "thorn", "minus",
  /* 0x00f0 */
  "multiply", "onesuperior", "twosuperior", "threesuperior", "onehalf",
  "onequarter", "threequarters", "franc", "Gbreve", "gbreve", "Idotaccent",
  "Scedilla", "scedilla", "Cacute", "cacute", "Ccaron",
  /* 0x0100 */
  "ccaron", "dcroat"
};

XWTTPostTable::XWTTPostTable()
{
	Version = 0;
	italicAngle = 0;
	underlinePosition = 0;
	underlineThickness = 0;
	isFixedPitch = 0;
	minMemType42 = 0;
	maxMemType42 = 0;
	minMemType1 = 0;
	maxMemType1 = 0;
	numberOfGlyphs = 0;
	glyphNamePtr = 0;
	names = 0;
	count = 0;
}

XWTTPostTable::XWTTPostTable(XWFontFileSFNT * sfont)
{
	Version            = sfont->getULong();
  	italicAngle        = sfont->getULong();
  	underlinePosition  = sfont->getShort();
  	underlineThickness = sfont->getShort();
  	isFixedPitch       = sfont->getULong();
  	minMemType42       = sfont->getULong();
  	maxMemType42       = sfont->getULong();
  	minMemType1        = sfont->getULong();
  	maxMemType1        = sfont->getULong();

  	numberOfGlyphs    = 0;
  	glyphNamePtr      = 0;
  	count             = 0;
  	names             = 0;

  	if (Version == 0x00010000UL) 
  	{
    	numberOfGlyphs  = 258;
    	glyphNamePtr    = (char **) macglyphorder;
  	} 
  	else if (Version == 0x00020000UL) 
  	{
    	if (readV2Names(sfont) < 0)
    		Version = 0;
  	} 
}

XWTTPostTable::~XWTTPostTable()
{
	if (glyphNamePtr && glyphNamePtr != macglyphorder)
		free(glyphNamePtr);
		
	if (names)
	{
		for (ushort i = 0; i < count; i++) 
		{
      		if (names[i])
				delete [] names[i];
    	}
    	free(names);
	}
}

ushort XWTTPostTable::lookup(const char *glyphname)
{
	for (ushort gid = 0; gid < count; gid++) 
	{
    	if (glyphNamePtr[gid] && !strcmp(glyphname, glyphNamePtr[gid])) 
      		return  gid;
  	}

  	return 0;
}

int XWTTPostTable::readV2Names(XWFontFileSFNT *sfont)
{
	numberOfGlyphs = sfont->getUShort();
	ushort * indices = new ushort[numberOfGlyphs];
  	count = 0;
  	
  	for (ushort i = 0; i < numberOfGlyphs; i++) 
  	{
    	ushort idx = sfont->getUShort();
    	if (idx >= 258) 
    	{
      		if (idx > 32767) 
				idx = 0;
      		count++;
    	}
    	indices[i] = idx;
  	}

  	if (count < 1) 
    	names = 0;
  	else 
  	{
    	names = (char**)malloc(count * sizeof(char *));
    	for (ushort i = 0; i < count; i++) 
    	{
      		int len = sfont->getByte();
      		if (len > 0) 
      		{
				names[i] = new char[len + 1];
				sfont->read(names[i], len);
				names[i][len] = 0;
      		} 
      		else 
				names[i] = 0;
    	}
  	}

  	glyphNamePtr = (char**)malloc(numberOfGlyphs * sizeof(char *));
  	for (ushort i = 0; i < numberOfGlyphs; i++) 
  	{
    	ushort idx = indices[i];
    	if (idx < 258) 
      		glyphNamePtr[i] = (char *) macglyphorder[idx];
    	else if (idx - 258 < count) 
      		glyphNamePtr[i] = names[idx - 258];
    	else 
    	{
      		delete [] indices;
      		return -1;
    	}
  	}
  	
  	delete [] indices;

  	return 0;
}

XWTTHeadTable::XWTTHeadTable()
{
	version = 0;
	fontRevision = 0;
	checkSumAdjustment = 0;
  	magicNumber = 0;
  	flags = 0;
  	unitsPerEm = 0;
  	xMin = 0;
  	yMin = 0;
  	xMax = 0;
  	yMax = 0;
  	macStyle = 0;
  	lowestRecPPEM = 0;
  	fontDirectionHint = 0;
  	indexToLocFormat = 0;
  	glyphDataFormat = 0;
}

XWTTHeadTable::XWTTHeadTable(XWFontFileSFNT * sfont)
{
	version = sfont->getULong();
  	fontRevision = sfont->getULong();
  	checkSumAdjustment = sfont->getULong();
  	magicNumber = sfont->getULong();
  	flags = sfont->getUShort();
  	unitsPerEm = sfont->getUShort();
  	for (char i = 0; i < 8; i++) 
    	created[i] = sfont->getByte();
  	
  	for (char i = 0; i < 8; i++) 
    	modified[i] = sfont->getByte();
    	
  	xMin = sfont->getShort();
  	yMin = sfont->getShort();
  	xMax = sfont->getShort();
  	yMax = sfont->getShort();
  	macStyle = sfont->getShort();
  	lowestRecPPEM = sfont->getShort();
  	fontDirectionHint = sfont->getShort();
  	indexToLocFormat = sfont->getShort();
  	glyphDataFormat = sfont->getShort();
}

char * XWTTHeadTable::pack()
{
	char * p = 0;
	char * data = 0;
	p = data = new char[TT_HEAD_TABLE_SIZE];
  	p += XWFontFile::putULong(p, version);
  	p += XWFontFile::putULong(p, fontRevision);
  	p += XWFontFile::putULong(p, checkSumAdjustment);
  	p += XWFontFile::putULong(p, magicNumber);
  	p += XWFontFile::putUShort(p, flags);
  	p += XWFontFile::putUShort(p, unitsPerEm);
  	for (char i = 0; i < 8; i++) 
    	*(p++) = created[i];
  	
  	for (char i = 0; i < 8; i++) 
    	*(p++) = modified[i];
    	
  	p += XWFontFile::putShort(p, xMin);
  	p += XWFontFile::putShort(p, yMin);
  	p += XWFontFile::putShort(p, xMax);
  	p += XWFontFile::putShort(p, yMax);
  	p += XWFontFile::putUShort(p, macStyle);
  	p += XWFontFile::putUShort(p, lowestRecPPEM);
  	p += XWFontFile::putShort(p, fontDirectionHint);
  	p += XWFontFile::putShort(p, indexToLocFormat);
  	p += XWFontFile::putShort(p, glyphDataFormat);

  	return data;
}

XWTTMaxpTable::XWTTMaxpTable()
{
	version = 0;
  	numGlyphs = 0;
  	maxPoints = 0;
  	maxContours = 0;
  	maxComponentPoints = 0;
  	maxComponentContours = 0;
  	maxZones = 0;
  	maxTwilightPoints = 0;
  	maxStorage = 0;
  	maxFunctionDefs = 0;
  	maxInstructionDefs = 0;
  	maxStackElements = 0;
  	maxSizeOfInstructions = 0;
  	maxComponentElements = 0;
  	maxComponentDepth = 0;
}

XWTTMaxpTable::XWTTMaxpTable(XWFontFileSFNT * sfont)
{
	version = sfont->getULong();
  	numGlyphs = sfont->getUShort();
  	maxPoints = sfont->getUShort();
  	maxContours = sfont->getUShort();
  	maxComponentPoints = sfont->getUShort();
  	maxComponentContours = sfont->getUShort();
  	maxZones = sfont->getUShort();
  	maxTwilightPoints = sfont->getUShort();
  	maxStorage = sfont->getUShort();
  	maxFunctionDefs = sfont->getUShort();
  	maxInstructionDefs = sfont->getUShort();
  	maxStackElements = sfont->getUShort();
  	maxSizeOfInstructions = sfont->getUShort();
  	maxComponentElements = sfont->getUShort();
  	maxComponentDepth = sfont->getUShort();
}

char * XWTTMaxpTable::pack()
{
	char * p = 0;
	char * data = 0;
	p = data = new char[TT_MAXP_TABLE_SIZE];
	p += XWFontFile::putULong(p, version);
  	p += XWFontFile::putUShort(p, numGlyphs);
  	p += XWFontFile::putUShort(p, maxPoints);
  	p += XWFontFile::putUShort(p, maxContours);
  	p += XWFontFile::putUShort(p, maxComponentPoints);
  	p += XWFontFile::putUShort(p, maxComponentContours);
  	p += XWFontFile::putUShort(p, maxZones);
  	p += XWFontFile::putUShort(p, maxTwilightPoints);
  	p += XWFontFile::putUShort(p, maxStorage);
  	p += XWFontFile::putUShort(p, maxFunctionDefs);
  	p += XWFontFile::putUShort(p, maxInstructionDefs);
  	p += XWFontFile::putUShort(p, maxStackElements);
  	p += XWFontFile::putUShort(p, maxSizeOfInstructions);
  	p += XWFontFile::putUShort(p, maxComponentElements);
  	p += XWFontFile::putUShort(p, maxComponentDepth);

  	return data;
}

XWTTHHeaTable::XWTTHHeaTable()
{
	version = 0;
  	ascent = 0;
  	descent = 0;
  	lineGap = 0;
  	advanceWidthMax = 0;
  	minLeftSideBearing = 0;
  	minRightSideBearing = 0;
  	xMaxExtent = 0;
  	caretSlopeRise = 0;
  	caretSlopeRun = 0;
  	caretOffset = 0;
  	metricDataFormat = 0;
  	numOfLongHorMetrics = 0;
  	numOfExSideBearings = 0;
}

XWTTHHeaTable::XWTTHHeaTable(XWFontFileSFNT * sfont)
{
	version = sfont->getULong();
  	ascent  = sfont->getShort();
  	descent = sfont->getShort();
  	lineGap = sfont->getShort();
  	advanceWidthMax     = sfont->getUShort();
  	minLeftSideBearing  = sfont->getShort();
  	minRightSideBearing = sfont->getShort();
  	xMaxExtent     = sfont->getShort();
  	caretSlopeRise = sfont->getShort();
  	caretSlopeRun  = sfont->getShort();
  	caretOffset    = sfont->getShort();
  	for(char i = 0; i < 4; i++) 
    	reserved[i] = sfont->getShort();
    	
  	metricDataFormat = sfont->getShort();
  	if (metricDataFormat != 0)
  		return ;
  		
  	numOfLongHorMetrics = sfont->getUShort();

  	ulong len = sfont->findTableLen("hmtx");
  	numOfExSideBearings = (ushort)((len - numOfLongHorMetrics * 4) / 2);
}

char * XWTTHHeaTable::pack()
{
	char * p = 0;
	char * data = 0;
	p = data = new char[TT_HHEA_TABLE_SIZE];
	p += XWFontFile::putULong(p, version);
	p += XWFontFile::putShort(p, ascent);
  	p += XWFontFile::putShort(p, descent);
  	p += XWFontFile::putShort(p, lineGap);
  	p += XWFontFile::putUShort(p, advanceWidthMax);
  	p += XWFontFile::putShort(p, minLeftSideBearing);
  	p += XWFontFile::putShort(p, minRightSideBearing);
  	p += XWFontFile::putShort(p, xMaxExtent);
  	p += XWFontFile::putShort(p, caretSlopeRise);
  	p += XWFontFile::putShort(p, caretSlopeRun);
  	p += XWFontFile::putShort(p, caretOffset);
  	for (char i = 0; i < 4; i++) 
    	p += XWFontFile::putShort(p, reserved[i]);
  	p += XWFontFile::putShort(p, metricDataFormat);
  	p += XWFontFile::putUShort(p, numOfLongHorMetrics);

  	return data;
}

XWTTVHeaTable::XWTTVHeaTable()
{
	version = 0;
  	vertTypoAscender = 0;
  	vertTypoDescender = 0;
  	vertTypoLineGap = 0;
  	advanceHeightMax = 0;
  	minTopSideBearing = 0;
  	minBottomSideBearing = 0;
  	yMaxExtent = 0;
  	caretSlopeRise = 0;
  	caretSlopeRun = 0;
  	caretOffset = 0;
  	metricDataFormat = 0;
  	numOfLongVerMetrics = 0;
  	numOfExSideBearings = 0;
}

XWTTVHeaTable::XWTTVHeaTable(XWFontFileSFNT * sfont)
{
	version = sfont->getULong();
  	vertTypoAscender = sfont->getShort();
  	vertTypoDescender = sfont->getShort();
  	vertTypoLineGap = sfont->getShort();
  	advanceHeightMax = sfont->getShort();
  	minTopSideBearing = sfont->getShort();
  	minBottomSideBearing = sfont->getShort();
  	yMaxExtent = sfont->getShort();
  	caretSlopeRise = sfont->getShort();
  	caretSlopeRun = sfont->getShort();
  	caretOffset = sfont->getShort();
  	for(char i = 0; i < 4; i++) 
    	reserved[i] = sfont->getShort();
    	
  	metricDataFormat = sfont->getShort();
  	numOfLongVerMetrics = sfont->getUShort();

  	ulong len = sfont->findTableLen("vmtx");
  	numOfExSideBearings = (ushort)((len - numOfLongVerMetrics * 4) / 2);
}

XWTTVORGTable::XWTTVORGTable()
{
	defaultVertOriginY = 0;
	numVertOriginYMetrics = 0;
	vertOriginYMetrics = 0;
}

XWTTVORGTable::XWTTVORGTable(XWFontFileSFNT * sfont)
{
	defaultVertOriginY    = sfont->getShort();
    numVertOriginYMetrics = sfont->getUShort();
    vertOriginYMetrics    = (TTVertOriginYMetrics*)malloc(numVertOriginYMetrics * sizeof(TTVertOriginYMetrics));
    for (ushort i = 0; i < numVertOriginYMetrics; i++) 
    {
    	vertOriginYMetrics[i].glyphIndex  = sfont->getUShort();
    	vertOriginYMetrics[i].vertOriginY = sfont->getShort();
    }
}

XWTTVORGTable::~XWTTVORGTable()
{
	if (vertOriginYMetrics)
		free(vertOriginYMetrics);
}

XWTTOs2Table::XWTTOs2Table()
{
	version = 0;
  	xAvgCharWidth = 0;  
  	usWeightClass = 400U;  
  	usWidthClass = 0;   
  	fsType = 0;
  	ySubscriptXSize = 0;        
  	ySubscriptYSize = 0;      
  	ySubscriptXOffset = 0;
  	ySubscriptYOffset = 0;      
  	ySuperscriptXSize = 0;      
  	ySuperscriptYSize = 0;      
  	ySuperscriptXOffset = 0;    
  	ySuperscriptYOffset = 0;    
  	yStrikeoutSize = 0; 
  	yStrikeoutPosition = 0;     
  	sFamilyClass = 0;   
  	for (char i = 0; i < 10; i++)
  		panose[i] = 0;
  		
  	ulUnicodeRange1 = 0;
  	ulUnicodeRange2 = 0;
  	ulUnicodeRange3 = 0;
  	ulUnicodeRange4 = 0;
  	fsSelection = 0;    
  	usFirstCharIndex = 0;
  	usLastCharIndex = 0;
  	sTypoAscender = 880;
  	sTypoDescender = -120;
  	sTypoLineGap = 0;
  	usWinAscent = 0;
  	usWinDescent = 0;
  	ulCodePageRange1 = 0;
  	ulCodePageRange2 = 0;
  	/* version 0x0002 */
  	sxHeight = 0;
  	sCapHeight = 0;
  	usDefaultChar = 0;
  	usBreakChar = 0;
  	usMaxContext = 0;
}

XWTTOs2Table::XWTTOs2Table(XWFontFileSFNT * sfont)
{
	version       = sfont->getUShort();
    xAvgCharWidth = sfont->getShort();
    usWeightClass = sfont->getUShort();
    usWidthClass  = sfont->getUShort();
    fsType        = sfont->getShort();
    ySubscriptXSize   = sfont->getShort();
    ySubscriptYSize   = sfont->getShort();
    ySubscriptXOffset = sfont->getShort();
    ySubscriptYOffset = sfont->getShort();
    ySuperscriptXSize = sfont->getShort();
    ySuperscriptYSize = sfont->getShort();
    ySuperscriptXOffset = sfont->getShort();
    ySuperscriptYOffset = sfont->getShort();
    yStrikeoutSize      = sfont->getShort();
    yStrikeoutPosition  = sfont->getShort();
    sFamilyClass        = sfont->getShort();
    for (char i = 0; i < 10; i++) 
    	panose[i] = sfont->getByte();
    	
    ulUnicodeRange1 = sfont->getULong();
    ulUnicodeRange2 = sfont->getULong();
    ulUnicodeRange3 = sfont->getULong();
    ulUnicodeRange4 = sfont->getULong();
    for (char i = 0; i < 4; i++) 
    	achVendID[i] = sfont->getChar();
    	
    fsSelection      = sfont->getUShort();
    usFirstCharIndex = sfont->getUShort();
    usLastCharIndex  = sfont->getUShort();
    sTypoAscender    = sfont->getShort();
    sTypoDescender   = sfont->getShort();
    sTypoLineGap     = sfont->getShort();
    usWinAscent      = sfont->getUShort();
    usWinDescent     = sfont->getUShort();
    ulCodePageRange1 = sfont->getULong();
    ulCodePageRange2 = sfont->getULong();
    if (version == 0x0002) 
    {
      	sxHeight      = sfont->getShort();
      	sCapHeight    = sfont->getShort();
      	usDefaultChar = sfont->getUShort();
      	usBreakChar   = sfont->getUShort();
      	usMaxContext  = sfont->getUShort();
    }
    else
    {
    	sxHeight = 0;
    	sCapHeight = 0;
    	usDefaultChar = 0;
    	usBreakChar = 0;
    	usMaxContext = 0;
    }
}


#define FLAG_NOT (1 << 0)
#define FLAG_AND (1 << 1)

XWOTLOpt::XWOTLOpt()
	:rule(0)
{
}

XWOTLOpt::~XWOTLOpt()
{
	if (rule)
		releaseTree(rule);
}

int XWOTLOpt::match(const char *tag)
{
	if (!rule)
		return 1;
		
	return matchExpr(rule, tag);
}

int XWOTLOpt::parse(const char *optstr)
{
	if (optstr) 
	{
    	char * p      = (char *) optstr;
    	char * endptr = p + strlen(optstr);
    	rule = parseExpr(&p, endptr);
  	}

  	return 0;
}

int XWOTLOpt::matchExpr(XWOTLOpt::BTNode *expr, const char *key)
{
	int retval = 1;
  	if (expr) 
  	{
    	if (!expr->left && !expr->right) 
    	{
      		for (int i = 0; i < 4; i++) 
      		{
				if (expr->data[i] != '?' && expr->data[i] != key[i]) 
				{
	  				retval = 0;
	  				break;
				}
      		}
    	} 
    	else 
    	{
      		if (expr->left) 
				retval  = matchExpr(expr->left, key);
      		
      		if (expr->right) 
      		{
				if (retval && (expr->flag & FLAG_AND)) /* and */
	  				retval &= matchExpr(expr->right, key);
				else if (!retval && !(expr->flag & FLAG_AND)) /* or */
	  				retval  = matchExpr(expr->right, key);
      		}
    	}
    	
    	if (expr->flag & FLAG_NOT) /* not */
      		retval = retval ? 0 : 1;

  	}

  	return retval;
}

XWOTLOpt::BTNode * XWOTLOpt::newTree()
{
	BTNode * expr = (BTNode*)malloc(sizeof(BTNode));
	expr->flag  = 0;
  	expr->left  = 0;
  	expr->right = 0;
  	memset(expr->data, 0, 4);

  	return expr;
}

XWOTLOpt::BTNode * XWOTLOpt::parseExpr(char **pp, char *endptr)
{
	if (*pp >= endptr)
    	return 0;
    	
    BTNode * root = 0;
    BTNode * curr = 0;
    root = curr = newTree();
    
    while (*pp < endptr)
    {
    	switch (**pp)
    	{
    		case '!':
      			if (curr->flag & 2)
        			curr->flag &= ~FLAG_NOT;
      			else
        			curr->flag |=  FLAG_NOT;
      			(*pp)++;
      			break;
      			
      		case '(':
      			(*pp)++;
      			if (*pp < endptr) 
      			{
        			BTNode * expr = parseExpr(pp, endptr);
        			if (!expr) 
            			return 0;
            			
        			if (**pp != ')') 
            			return 0;
            		
        			curr->left  = expr->left;
        			curr->right = expr->right;
        			memcpy(curr->data, expr->data, 4);

        			free(expr);
      			} 
      			else 
      			{
        			releaseTree(root);
        			return 0;
      			}
      			(*pp)++;
      			break;
      			
      		case ')':
      			return root;
      			break;
      			
      		case '|': 
      		case '&':
      			if (*pp >= endptr) 
      			{
        			releaseTree(root);
        			return 0;
      			} 
      			else 
      			{
        			BTNode * tmp = newTree();
        			tmp->left  = root;
        			tmp->right = curr = newTree();
        			if (**pp == '&')
          				tmp->flag = 1;
        			else
          				tmp->flag = 0;
        			root = tmp;
      			}
      			(*pp)++;
      			break;
      			
    		case '*':
      			memset(curr->data, '?', 4);
      			(*pp)++;
      			break;
      			
      		default:
      			if (*pp + 4 <= endptr) 
      			{
        			for (int i = 0; i < 4; i++) 
        			{
            			if (**pp == ' '   || **pp == '?' || isalpha(**pp) || isdigit(**pp))
                			curr->data[i] = **pp;
            			else if (**pp == '_')
                			curr->data[i] = ' ';
            			else 
            			{
                			releaseTree(root);
                			return 0;
            			}
            			(*pp)++;
        			}
      			} 
      			else 
      			{
        			releaseTree(root);
        			return 0;
      			}
      			break;
    	}
    }
    
    return root;
}

void XWOTLOpt::releaseTree(XWOTLOpt::BTNode *tree)
{
	if (tree) 
	{
    	if (tree->left)
      		releaseTree(tree->left);
    	if (tree->right)
      		releaseTree(tree->right);
    	free(tree);
  	}
}

XWOTLGsub::XWOTLGsub()
{
	num_gsubs = 0;
	selected    = -1;
}

XWOTLGsub::~XWOTLGsub()
{
	for (int i = 0; i < num_gsubs; i++) 
	{
    	OTLGsubTab * gsub = &gsubs[i];

    	if (gsub->script)
      		free(gsub->script);
    	if (gsub->language)
      		free(gsub->language);
    	if (gsub->feature)
      		free(gsub->feature);

    	for (int j = 0; j < gsub->num_subtables; j++) 
    	{
      		OTLGsubSubTab * subtab = &(gsub->subtables[j]);
      		switch ((int) subtab->LookupType) 
      		{
      			case OTL_GSUB_TYPE_SINGLE:
        			releaseOTLGsubSingle(subtab);
        			break;
        			
      			case OTL_GSUB_TYPE_ALTERNATE:
        			releaseOTLGsubAlternate(subtab);
        			break;
        			
      			case OTL_GSUB_TYPE_LIGATURE:
        			releaseOTLGsubLigature(subtab);
        			break;
        			
      			default:
        			break;
      		}
    	}
    	
    	if (gsub->subtables)
    		free(gsub->subtables);
  	}
}

int XWOTLGsub::addFeat(const char *script,
                       const char *language,
                       const char *feature,
                       XWFontFileSFNT *sfont)
{
	if (num_gsubs > GSUB_LIST_MAX) 
    	return -1;
  	
  	OTLGsubTab * gsub = 0;
  	int i = 0;
  	for (; i < num_gsubs; i++) 
  	{
    	gsub = &gsubs[i];
    	if (!strcmp(script,   gsub->script) && 
        	!strcmp(language, gsub->language) && 
        	!strcmp(feature,  gsub->feature)) 
        {
      		selected = i;
      		return 0;
    	}
  	}
  	
  	gsub = &gsubs[i];
  	gsub->script   = (char*)malloc((strlen(script)+1) * sizeof(char));
  	strcpy(gsub->script,   script);
  	gsub->language = (char*)malloc((strlen(language)+1) * sizeof(char));
  	strcpy(gsub->language, language);
  	gsub->feature  = (char*)malloc((strlen(feature) +1) * sizeof(char));
  	strcpy(gsub->feature,  feature);
  	gsub->num_subtables = 0;
  	gsub->subtables = 0;

  	int retval = readOTLGsubFeat(gsub, sfont);
  	if (retval >= 0) 
  	{
    	selected = i;
    	num_gsubs++;
  	} 
  	else 
  	{
    	free(gsub->script);
    	gsub->script = 0;
    	free(gsub->language);
    	gsub->language = 0;
    	free(gsub->feature);
    	gsub->feature = 0;
  	}
  
  	return retval;
}

int XWOTLGsub::apply(ushort *gid)
{
	if (!gid)
		return -1;
		
	int i = selected;
  	if (i < 0 || i >= num_gsubs) 
    	return -1;
  	
  	OTLGsubTab * gsub = &gsubs[i];
	int retval = -1;
  	for (int j = 0; retval < 0 && j < gsub->num_subtables; j++) 
  	{
    	OTLGsubSubTab * subtab = &(gsub->subtables[j]);
    	switch ((int) subtab->LookupType)
    	{
    		case OTL_GSUB_TYPE_SINGLE:
      			retval = applyOTLGsubSingle(subtab, gid);
      			break;
      			
    		default:
      			break;
    	}
  	}

  	return retval;
}

int XWOTLGsub::applyAlt(ushort alt_idx, ushort *gid)
{
	int retval = -1;
	if (!gid)
    	return retval;

  	int i = selected;
  	if (i < 0 || i >= num_gsubs) 
    	return -1;
    	
  	OTLGsubTab * gsub = &gsubs[i];

  	for (int j = 0; retval < 0 && j < gsub->num_subtables; j++) 
  	{
    	OTLGsubSubTab * subtab = &(gsub->subtables[j]);
    	switch ((int) subtab->LookupType)
    	{
    		case OTL_GSUB_TYPE_ALTERNATE:
      			retval = applyOTLGsubAlternate(subtab, alt_idx, gid);
      			break;
      			
    		default:
      			break;
    	}
  	}

  	return retval;
}

int XWOTLGsub::applyLig(ushort *gid_in, 
	                    ushort num_gids, 
	                    ushort *gid_out)
{
	int retval = -1;
	if (!gid_out)
    	return retval;

  	int i = selected;
  	if (i < 0 || i >= num_gsubs) 
    	return -1;
  	
  	OTLGsubTab * gsub = &gsubs[i];

  	for (int j = 0; retval < 0 && j < gsub->num_subtables; j++) 
  	{
    	OTLGsubSubTab * subtab = &(gsub->subtables[j]);
    	switch ((int) subtab->LookupType)
    	{
    		case OTL_GSUB_TYPE_LIGATURE:
      			retval = applyOTLGsubLigature(subtab, gid_in, num_gids, gid_out);
      			break;
      			
    		default:
      			break;
    	}
  	}

  	return retval;
}

int XWOTLGsub::select(const char *script,
                      const char *language,
                      const char *feature)
{
	for (int i = 0; i < num_gsubs; i++) 
	{
    	OTLGsubTab * gsub = &gsubs[i];
    	if (!strcmp(gsub->script,   script) && 
        	!strcmp(gsub->language, language) && 
        	!strcmp(gsub->feature,  feature)) 
        {
      		selected = i;
      		return i;
    	}
  	}

  	selected = -1;

  	return -1;
}

int XWOTLGsub::applyOTLGsubAlternate(OTLGsubSubTab *subtab, 
	                                 ushort alt_idx, 
	                                 ushort *gid)
{
	if (subtab->SubstFormat == 1) 
	{
    	OTLGsubAlternate1 * data = subtab->table.alternate1;
    	long idx  = lookupCLTCoverage(&data->coverage, *gid);
    	if (idx < 0 || idx >= data->AlternateSetCount)
      		return  -1;
    	else 
    	{
      		OTLGsubAltSet * altset = &(data->AlternateSet[idx]);
      		if (alt_idx >= altset->GlyphCount)
        		return  -1;
      		else 
      		{
        		*gid = altset->Alternate[alt_idx];
        		return  0;
      		}
    	}
  	}

  	return -1;
}

int XWOTLGsub::applyOTLGsubLigature(OTLGsubSubTab *subtab,
                         	        ushort *gid_in,  
                         	        ushort num_gids,
                         	        ushort *gid_out)
{
	if (!gid_in || num_gids < 1)
    	return -1;

  	if (subtab->SubstFormat == 1) 
  	{
    	OTLGsubLigature1 * data = subtab->table.ligature1;
    	long idx  = lookupCLTCoverage(&data->coverage, gid_in[0]);
    	if (idx >= 0 && idx < data->LigSetCount) 
    	{
      		OTLGsubLigSet * ligset = &(data->LigatureSet[idx]);
      		for (ushort j = 0; j < ligset->LigatureCount; j++) 
      		{
        		if (!glyphSeqCmp(&gid_in[1], num_gids - 1, 
    			                 ligset->Ligature[j].Component,
                			     ligset->Ligature[j].CompCount - 1)) 
                {
          			*gid_out = ligset->Ligature[j].LigGlyph;
          			return 0; /* found */
        		}
      		}
    	}
  	}

  	return -1;
}

int XWOTLGsub::applyOTLGsubSingle(OTLGsubSubTab *subtab, ushort *gid)
{
	if (subtab->SubstFormat == 1) 
	{
    	OTLGsubSingle1 * data = (subtab->table).single1;
    	long idx  = lookupCLTCoverage(&data->coverage, *gid);
    	if (idx >= 0) 
    	{
      		*gid += data->DeltaGlyphID;
      		return 0;
    	}
  	} 
  	else if (subtab->SubstFormat == 2) 
  	{
    	OTLGsubSingle2 * data = (subtab->table).single2;
    	long idx  = lookupCLTCoverage(&data->coverage, *gid);
    	if (idx >= 0 && idx < data->GlyphCount) 
    	{
      		*gid = (data->Substitute)[idx];
      		return 0;
    	}
  	}

  	return -1;
}

int XWOTLGsub::glyphSeqCmp(ushort *glyph_seq0, 
	                       ushort n_glyphs0,
                           ushort *glyph_seq1, 
                           ushort n_glyphs1)
{
	if (n_glyphs0 != n_glyphs1)
    	return n_glyphs0 - n_glyphs1;

  	for (ushort i = 0; i < n_glyphs0; i++) 
  	{
    	if (glyph_seq0[i] != glyph_seq1[i])
      		return glyph_seq0[i] - glyph_seq1[i];
  	}

  	return 0;
}

long XWOTLGsub::lookupCLTCoverage(CLTCoverage *cov, ushort gid)
{
	switch (cov->format) 
	{
  		case 1: /* list */
    		for (ushort i = 0; i < cov->count; i++) 
    		{
      			if (cov->list[i] > gid) 
        			break;
      			else if (cov->list[i] == gid) 
        			return i;
    		}
    		break;
    		
  		case 2: /* range */
    		for (ushort i = 0; i < cov->count; i++) 
    		{
      			if (gid < cov->range[i].Start) 
        			break;
      			else if (gid <= cov->range[i].End) 
        			return (cov->range[i].StartCoverageIndex + gid - cov->range[i].Start);
    		}
    		break;
    		
  		default:
    		break;
  	}

  	return -1;
}

long XWOTLGsub::readCLTCoverage(CLTCoverage *cov, XWFontFileSFNT *sfont)
{
	cov->format = sfont->getUShort();
  	cov->count  = sfont->getUShort();
  	long len = 4;

  	switch (cov->format) 
  	{
  		case 1: /* list */
    		if (cov->count == 0)
      			cov->list = 0;
    		else 
    		{
      			cov->list = (ushort*)malloc(cov->count * sizeof(ushort));
      			for (ushort i = 0; i < cov->count; i++) 
        			cov->list[i] = sfont->getUShort();
    		}
    		cov->range = 0;
    		len += 2 * cov->count;
    		break;
    		
  		case 2: /* range */
    		if (cov->count == 0)
      			cov->range = 0;
    		else 
    		{
      			cov->range = (CLTRange*)malloc(cov->count * sizeof(CLTRange));
      			for (ushort i = 0; i < cov->count; i++) 
        			len += readCLTRange(&(cov->range[i]), sfont);
    		}
    		cov->list = 0;
    		break;
    		
  		default:
  			break;
  	}

  	return len;
}

long XWOTLGsub::readCLTFeatureTable(CLTFeatureTable *tab, XWFontFileSFNT *sfont)
{
	tab->FeatureParams = sfont->getUShort();
  	long len  = 2;
  	len += readCLTNumberList(&tab->LookupListIndex, sfont);

  	return len;
}

long XWOTLGsub::readCLTLangSysTable(CLTLangSysTable *tab, XWFontFileSFNT *sfont)
{
	tab->LookupOrder     = sfont->getUShort();
  	tab->ReqFeatureIndex = sfont->getUShort();
  	long len  = 4;
  	len += readCLTNumberList(&tab->FeatureIndex, sfont);

  	return len;
}

long XWOTLGsub::readCLTLookupTable(CLTLookupTable *tab, XWFontFileSFNT *sfont)
{
	tab->LookupType = sfont->getUShort();
  	tab->LookupFlag = sfont->getUShort();
  	long len  = 4;
  	len += readCLTNumberList(&tab->SubTableList, sfont);

  	return len;
}

long XWOTLGsub::readCLTNumberList(CLTNumberList *list, XWFontFileSFNT *sfont)
{
	list->count = sfont->getUShort();

  	if (list->count == 0)
    	list->value = 0;
  	else 
  	{
    	list->value = (ushort*)malloc(list->count * sizeof(ushort));
    	for (long i = 0; i < list->count; i++) 
      		list->value[i] = sfont->getUShort();
  	}

  	return (2 + 2 * list->count);
}

long XWOTLGsub::readCLTRange(CLTRange *rec, XWFontFileSFNT *sfont)
{
	rec->Start = sfont->getUShort();
  	rec->End   = sfont->getUShort();
  	rec->StartCoverageIndex = sfont->getUShort();

  	return 6;
}

long XWOTLGsub::readCLTRecord(CLTRecord *rec, XWFontFileSFNT *sfont)
{
	for (char i = 0; i < 4; i++) 
    	rec->tag[i] = sfont->getChar();
  	
  	rec->tag[4] = '\0';
  	rec->offset = sfont->getUShort();

  	return 6;
}

long XWOTLGsub::readCLTRecordList(CLTRecordList *list, XWFontFileSFNT *sfont)
{
	list->count = sfont->getUShort();
  	long len = 2;

  	if (list->count == 0)
    	list->record = 0;
  	else 
  	{
    	list->record = (CLTRecord*)malloc(list->count * sizeof(CLTRecord));
    	for (long i = 0; i < list->count; i++) 
      		len += readCLTRecord(&(list->record[i]), sfont);
  	}

  	return len;
}

long XWOTLGsub::readCLTScriptTable(CLTScriptTable *tab, XWFontFileSFNT *sfont)
{
	tab->DefaultLangSys = sfont->getUShort();
  	long len  = 2;
  	len += readCLTRecordList(&tab->LangSysRecord, sfont);

  	return len;
}

long XWOTLGsub::readOTLGsubAlternate(OTLGsubSubTab *subtab, XWFontFileSFNT *sfont)
{
	ulong offset = sfont->tellPosition();

  	subtab->LookupType  = OTL_GSUB_TYPE_ALTERNATE;
  	subtab->SubstFormat = sfont->getUShort();
  	if (subtab->SubstFormat != 1) 
    	return -1;

  	long len  = 2;
  	OTLGsubAlternate1 * data = 0;
  	subtab->table.alternate1 = data = (OTLGsubAlternate1*)malloc(sizeof(OTLGsubAlternate1));

  	ushort cov_offset = sfont->getUShort();
  	len += 2;
  	CLTNumberList altset_offsets;
  	len += readCLTNumberList(&altset_offsets, sfont);
  	data->AlternateSetCount = altset_offsets.count;
  	if (data->AlternateSetCount == 0) 
  	{
    	data->AlternateSet    = 0;
    	data->coverage.count  = 0;
    	data->coverage.format = 0;
    	data->coverage.list   = 0;
    	return  len;
  	}
  	data->AlternateSet = (OTLGsubAltSet*)malloc(data->AlternateSetCount * sizeof(OTLGsubAltSet));
  	for (ushort i = 0; i < data->AlternateSetCount; i++) 
  	{
    	OTLGsubAltSet *altset = &(data->AlternateSet[i]);    	
    	ulong altset_offset = offset + altset_offsets.value[i];
    	sfont->seek(altset_offset);
    	altset->GlyphCount = sfont->getUShort();
    	len += 2;
    	if (altset->GlyphCount == 0) 
    	{
      		altset->Alternate = 0;
      		break;
    	}
    	altset->Alternate = (ushort*)malloc(altset->GlyphCount * sizeof(ushort));
    	for (ushort j = 0; j < altset->GlyphCount; j++) 
    	{
      		altset->Alternate[j] = sfont->getUShort();
      		len += 2;
    	}
  	}
  	releaseCLTNumberList(&altset_offsets);

  	sfont->seek(offset + cov_offset);
  	len += readCLTCoverage(&data->coverage, sfont);

  	return  len;
}

int XWOTLGsub::readOTLGsubFeat(OTLGsubTab *gsub, XWFontFileSFNT *sfont)
{
	ulong gsub_offset = sfont->findTablePos("GSUB");
  	if (gsub_offset == 0)
    	return -1;
    	
    XWOTLOpt script;
    script.parse(gsub->script);
    XWOTLOpt language;
    language.parse(gsub->language);
    XWOTLOpt feature;
    feature.parse(gsub->feature);
    
    uchar feat_bits[8192];
    memset(feat_bits, 0, 8192);
    
    sfont->seek(gsub_offset);
    OTLGsubHeader head;
  	readOTLGsubHeader(&head, sfont);
  	
  	ulong offset = gsub_offset + head.ScriptList;
  	sfont->seek(offset);
  	CLTRecordList script_list;
  	readCLTRecordList(&script_list, sfont);
  	
#define SET_BIT(b,p) do {\
  (b)[(p)/8] |= (1<<(7-((p) % 8)));\
} while (0)
#define BIT_SET(b,p) (((b)[(p)/8]) & (1 << (7-((p)%8))))

	for (long script_idx = 0; script_idx < script_list.count; script_idx++)
	{
		if (script.match(script_list.record[script_idx].tag))
		{
			offset = gsub_offset + head.ScriptList + script_list.record[script_idx].offset;
      		sfont->seek(offset);
      		
      		CLTScriptTable script_tab;
      		readCLTScriptTable(&script_tab, sfont);
      		
      		if (language.match("dflt") && script_tab.DefaultLangSys != 0) 
      		{
        		sfont->seek(offset + script_tab.DefaultLangSys);
        		CLTLangSysTable langsys_tab;
        		readCLTLangSysTable(&langsys_tab, sfont);
        		if (feature.match("____") && langsys_tab.ReqFeatureIndex != 0xFFFF)
        		{
          			SET_BIT(feat_bits, langsys_tab.ReqFeatureIndex);
          		}
          		
        		for (long feat_idx = 0; feat_idx < langsys_tab.FeatureIndex.count; feat_idx++) 
        		{
          			SET_BIT(feat_bits, langsys_tab.FeatureIndex.value[feat_idx]);
        		}
        		
        		releaseCLTLangSysTable(&langsys_tab);
      		}
      		
      		for (long langsys_idx = 0; langsys_idx < script_tab.LangSysRecord.count; langsys_idx++)
      		{
        		CLTRecord * langsys_rec = &(script_tab.LangSysRecord.record[langsys_idx]);
        		if (language.match(langsys_rec->tag)) 
        		{
          			sfont->seek(offset + langsys_rec->offset);
          			CLTLangSysTable langsys_tab;
          			readCLTLangSysTable(&langsys_tab, sfont);
          			if (feature.match("____") || langsys_tab.ReqFeatureIndex != 0xFFFF)
          			{
            			SET_BIT(feat_bits, langsys_tab.ReqFeatureIndex);
            		}
          			for (long feat_idx = 0; feat_idx < langsys_tab.FeatureIndex.count; feat_idx++) 
          			{
            			SET_BIT(feat_bits, langsys_tab.FeatureIndex.value[feat_idx]);
          			}
          			releaseCLTLangSysTable(&langsys_tab);
        		}
      		}
      		releaseCLTScriptTable(&script_tab);
		}
	}
	
	offset = gsub_offset + head.FeatureList;
  	sfont->seek(offset);
  	
  	CLTRecordList feature_list;
  	readCLTRecordList(&feature_list, sfont);
  	
  	offset = gsub_offset + head.LookupList;
  	sfont->seek(offset);
  	CLTNumberList lookup_list;
  	readCLTNumberList(&lookup_list, sfont);
  	
  	OTLGsubSubTab * subtab = 0;
  	ushort num_subtabs = 0;
  	for (long feat_idx = 0; feat_idx < feature_list.count; feat_idx++)
  	{
  		if (BIT_SET(feat_bits, feat_idx)  && 
  			(feature.match(feature_list.record[feat_idx].tag)))
  		{
  			offset = gsub_offset + head.FeatureList + feature_list.record[feat_idx].offset;
      		sfont->seek(offset);
      		CLTFeatureTable feature_table;
      		readCLTFeatureTable(&feature_table, sfont);
      		if (feature_table.FeatureParams != 0) 
      			return -1;
      			
      		for (long i = 0; i < feature_table.LookupListIndex.count; i++)
      		{
      			long ll_idx = feature_table.LookupListIndex.value[i];
        		if (ll_idx >= lookup_list.count)
          			return -1;
          			
          		offset = gsub_offset + head.LookupList + (lookup_list.value)[ll_idx];
        		sfont->seek(offset);
        		CLTLookupTable lookup_table;
        		readCLTLookupTable(&lookup_table, sfont);
        		
        		if (lookup_table.LookupType != OTL_GSUB_TYPE_SINGLE    &&
            		lookup_table.LookupType != OTL_GSUB_TYPE_ALTERNATE &&
            		lookup_table.LookupType != OTL_GSUB_TYPE_LIGATURE  &&
            		lookup_table.LookupType != OTL_GSUB_TYPE_ESUBST) 
            	{
          			continue;
        		}
        		
        		subtab = (OTLGsubSubTab*)realloc(subtab, (num_subtabs + lookup_table.SubTableList.count) * sizeof(OTLGsubSubTab));
        		long n_st = 0;
        		long st_idx = 0;
        		long r = 0;
        		for (; st_idx < lookup_table.SubTableList.count; st_idx++)
        		{
        			offset = gsub_offset + head.LookupList + lookup_list.value[ll_idx] + (lookup_table.SubTableList.value)[st_idx];
          			sfont->seek(offset);
          			switch ((int) lookup_table.LookupType)
          			{
          				case OTL_GSUB_TYPE_SINGLE:
            				r = readOTLGsubSingle(&subtab[num_subtabs + n_st], sfont);
            				if (r > 0)
              					n_st++;
            				break;
            				
            			case OTL_GSUB_TYPE_ALTERNATE:
            				r = readOTLGsubAlternate(&subtab[num_subtabs + n_st], sfont);
            				if (r > 0)
              					n_st++;
            				break;
            				
            			case OTL_GSUB_TYPE_LIGATURE:
            				r = readOTLGsubLigature(&subtab[num_subtabs + n_st], sfont);
            				if (r > 0)
              					n_st++;
            				break;
            				
            			case OTL_GSUB_TYPE_ESUBST:
            				{
            					ushort SubstFormat = sfont->getUShort();
              					if (SubstFormat != 1)
                					break;
                					
                				ushort ExtensionLookupType = sfont->getUShort();
              					ulong  ExtensionOffset     = sfont->getULong();
              					sfont->seek(offset + ExtensionOffset);
              					switch (ExtensionLookupType)
              					{
              						case OTL_GSUB_TYPE_SINGLE:
                						r = readOTLGsubSingle(&subtab[num_subtabs + n_st], sfont);
                						if (r > 0)
                  							n_st++;
                						break;

              						case OTL_GSUB_TYPE_ALTERNATE:
                						r = readOTLGsubAlternate(&subtab[num_subtabs + n_st], sfont);
                						if (r > 0)
                  							n_st++;
              							break;

              						case OTL_GSUB_TYPE_LIGATURE:
                						r = readOTLGsubLigature(&subtab[num_subtabs + n_st], sfont);
                						if (r > 0)
                  							n_st++;
                						break;
              					}
            				}
            				break;
            				
            			default:
            				break;
          			}
        		}
        		num_subtabs += n_st;
        		releaseCLTLookupTable(&lookup_table);
      		}
      		releaseCLTFeatureTable(&feature_table);
  		}
  	}
  	
  	releaseCLTNumberList(&lookup_list);
  	releaseCLTRecordList(&feature_list);
  	releaseCLTRecordList(&script_list);
  	
  	if (subtab != 0) 
  	{
    	gsub->num_subtables = num_subtabs;
    	gsub->subtables     = subtab;
  	} 
  	else 
    	return -1;

  	return 0;
}

long XWOTLGsub::readOTLGsubHeader(OTLGsubHeader *head, XWFontFileSFNT *sfont)
{
	head->version     = sfont->getULong ();
  	head->ScriptList  = sfont->getUShort();
  	head->FeatureList = sfont->getUShort();
  	head->LookupList  = sfont->getUShort();

  	return 10;
}

long XWOTLGsub::readOTLGsubLigature(OTLGsubSubTab *subtab, XWFontFileSFNT *sfont)
{
	ulong offset = sfont->tellPosition();

  	subtab->LookupType  = OTL_GSUB_TYPE_LIGATURE;
  	subtab->SubstFormat = sfont->getUShort();
  	if (subtab->SubstFormat != 1) 
    	return -1;

  	long len  = 2;
  	OTLGsubLigature1 * data = 0;
  	subtab->table.ligature1 = data = (OTLGsubLigature1*)malloc(sizeof(OTLGsubLigature1));

  	ushort cov_offset = sfont->getUShort();
  	len += 2;
  	CLTNumberList ligset_offsets;
  	len += readCLTNumberList(&ligset_offsets, sfont);
  	data->LigSetCount = ligset_offsets.count;
  	if (data->LigSetCount == 0) 
  	{
    	data->LigatureSet    = 0;
    	data->coverage.count  = 0;
    	data->coverage.format = 0;
    	data->coverage.list   = 0;
    	return len;
  	}
  	
  	data->LigatureSet = (OTLGsubLigSet*)malloc(data->LigSetCount * sizeof(OTLGsubLigSet));
  	for (ushort i = 0; i < data->LigSetCount; i++) 
  	{
    	OTLGsubLigSet * ligset = &(data->LigatureSet[i]);

    	ulong ligset_offset = offset + ligset_offsets.value[i];
    	sfont->seek(ligset_offset);
    	CLTNumberList ligset_tab;
    	len += readCLTNumberList(&ligset_tab, sfont);

    	ligset->LigatureCount = ligset_tab.count;
    	if (ligset_tab.count == 0) 
    	{
      		ligset->Ligature = 0;
      		break;
    	}
    	
    	ligset->Ligature = (OTLGsubLigTab*)malloc(ligset_tab.count * sizeof(OTLGsubLigTab));
    	for (ushort j = 0; j < ligset_tab.count; j++) 
    	{
      		sfont->seek(ligset_offset + ligset_tab.value[j]);
      		ligset->Ligature[j].LigGlyph = sfont->getUShort();
      		ligset->Ligature[j].CompCount = sfont->getUShort();
      		if (ligset->Ligature[j].CompCount == 0) 
      		{
        		ligset->Ligature[j].Component = 0;
        		break;
      		}
      		
      		ligset->Ligature[j].Component = (ushort*)malloc((ligset->Ligature[j].CompCount - 1) * sizeof(ushort));
      		ushort count = 0;
      		for (; count < ligset->Ligature[j].CompCount - 1; count++) 
        		ligset->Ligature[j].Component[count] = sfont->getUShort();
      		len += 4 + count * 2;
    	}
    	releaseCLTNumberList(&ligset_tab);
  	}
  	releaseCLTNumberList(&ligset_offsets);

  	sfont->seek(offset + cov_offset);
  	len += readCLTCoverage(&data->coverage, sfont);

  	return len;
}

long XWOTLGsub::readOTLGsubSingle(OTLGsubSubTab *subtab, XWFontFileSFNT *sfont)
{
	ulong offset = sfont->tellPosition();

  	subtab->LookupType  = OTL_GSUB_TYPE_SINGLE;
  	subtab->SubstFormat = sfont->getUShort();
  	long len = 2;

  	if (subtab->SubstFormat == 1) 
  	{
    	OTLGsubSingle1 *data = 0;

    	subtab->table.single1 = data = (OTLGsubSingle1*)malloc(sizeof(OTLGsubSingle1*));
    	ushort cov_offset         = sfont->getUShort();
    	data->DeltaGlyphID = sfont->getShort();
    	len += 4;

    	sfont->seek(offset + cov_offset);
    	len += readCLTCoverage(&data->coverage, sfont);

  	} 
  	else if (subtab->SubstFormat == 2) 
  	{
    	OTLGsubSingle2 *data = 0;
    	subtab->table.single2 = data = (OTLGsubSingle2*)malloc(sizeof(OTLGsubSingle2));
    	ushort cov_offset = sfont->getUShort();
    	data->GlyphCount = sfont->getUShort();
    	len += 4;

    	if (data->GlyphCount == 0)
      		data->Substitute = 0;
    	else 
    	{
      		data->Substitute = (ushort*)malloc(data->GlyphCount * sizeof(ushort));
      		for (ushort count = 0; count < data->GlyphCount; count++) 
        		data->Substitute[count] = sfont->getUShort();
      		len += 2 * data->GlyphCount;
    	}

    	sfont->seek(offset + cov_offset);
    	len += readCLTCoverage(&data->coverage, sfont);

  	} 

  	return len;
}

void XWOTLGsub::releaseCLTCoverage(CLTCoverage *cov)
{
	if (cov) 
	{
    	switch (cov->format) 
    	{
    		case 1: /* list */
      			if (cov->list)
        			free(cov->list);
      			cov->list = 0;
      			break;
      			
    		case 2: /* range */
      			if (cov->range)
        			free(cov->range);
      			cov->range = 0;
      			break;
      			
    		default:
      			break;
    	}
  	}
  	cov->count = 0;
}

void XWOTLGsub::releaseCLTFeatureTable(CLTFeatureTable *tab)
{
	if (tab)
    	releaseCLTNumberList(&tab->LookupListIndex);
}

void XWOTLGsub::releaseCLTLangSysTable(CLTLangSysTable *tab)
{
	if (tab)
    	releaseCLTNumberList(&tab->FeatureIndex);
}

void XWOTLGsub::releaseCLTLookupTable(CLTLookupTable *tab)
{
	if (tab)
    	releaseCLTNumberList(&tab->SubTableList);
}

void XWOTLGsub::releaseCLTNumberList(CLTNumberList *list)
{
	if (list) 
	{
    	if (list->value)
      		free(list->value);
    	list->value = 0;
    	list->count = 0;
  	}
}

void XWOTLGsub::releaseCLTRecordList(CLTRecordList *list)
{
	if (list) 
	{
    	if (list->record)
      		free(list->record);
    	list->record = 0;
    	list->count  = 0;
  	}
}

void XWOTLGsub::releaseCLTScriptTable(CLTScriptTable *tab)
{
	if (tab)
    	releaseCLTRecordList(&tab->LangSysRecord);
}

void XWOTLGsub::releaseOTLGsubAlternate(OTLGsubSubTab *subtab)
{
	if (subtab) 
	{
    	OTLGsubAlternate1 * data = subtab->table.alternate1;
    	if (data && data->AlternateSet) 
    	{
      		for (ushort i = 0; i < data->AlternateSetCount; i++) 
      		{
        		OTLGsubAltSet * altset = &(data->AlternateSet[i]);
        		if (altset->Alternate)
          			free(altset->Alternate);
        		altset->Alternate = 0;
      		}
      		free(data->AlternateSet);
    	}
    	releaseCLTCoverage(&data->coverage);
    	data->AlternateSet = 0;
    	free(data);
    	subtab->table.alternate1 = 0;
  	}
}

void XWOTLGsub::releaseOTLGsubLigature(OTLGsubSubTab *subtab)
{
	if (subtab) 
	{
    	OTLGsubLigature1 * data = subtab->table.ligature1;
    	if (data && data->LigatureSet) 
    	{
      		for (ushort i = 0; i < data->LigSetCount; i++) 
      		{
        		OTLGsubLigSet * ligset = &(data->LigatureSet[i]);
        		for (ushort j = 0; j < ligset->LigatureCount; j++) 
        		{
          			if (ligset->Ligature[j].Component)
            			free(ligset->Ligature[j].Component);
          			ligset->Ligature[j].Component = 0;
        		}
        		free(ligset->Ligature);
        		ligset->Ligature = 0;
      		}
      		free(data->LigatureSet);
    	}
    	releaseCLTCoverage(&data->coverage);
    	data->LigatureSet = 0;
    	free(data);
    	subtab->table.ligature1 = 0;
  	}
}

void XWOTLGsub::releaseOTLGsubSingle(OTLGsubSubTab *subtab)
{
	if (subtab) 
	{
    	switch((int) subtab->SubstFormat) 
    	{
    		case 1:
      		{
        		OTLGsubSingle1 * data = subtab->table.single1;
        		if (data) 
        		{
          			releaseCLTCoverage(&data->coverage);
          			free(data);
        		}
        		subtab->table.single1 = 0;
      		}
    		break;
    		
    	case 2:
      		{
        		OTLGsubSingle2 * data = subtab->table.single2;
        		if (data) 
        		{
          			if (data->Substitute)
            			free(data->Substitute);
          			releaseCLTCoverage(&data->coverage);
          			free(data);
        		}
        		subtab->table.single2 = 0;
      		}
    		break;
    		
    	default:
      		break;
    	}
  	}
}

