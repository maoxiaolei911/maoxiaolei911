/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <QMutex>
#include <QFile>
#include <QBuffer>
#include "XWUtil.h"
#include "XWApplication.h"
#include "XWFontSea.h"
#include "XWTFMCreator.h"
#include "XWTFMFile.h"


#define TFM_FORMAT 1
#define OFM_FORMAT 2

#define FWBASE ((double) (1<<20))


#define JFM_ID  11
#define JFMV_ID  9
#define IS_JFM(i) ((i) == JFM_ID || (i) == JFMV_ID)


#define SOURCE_TYPE_TFM 0
#define SOURCE_TYPE_JFM 1
#define SOURCE_TYPE_OFM 2

#define MAPTYPE_NONE  0
#define MAPTYPE_CHAR  1
#define MAPTYPE_RANGE 2

#define FONT_DIR_HORIZ 0
#define FONT_DIR_VERT  1

#define ISEVEN(n) (((n)/2)*2==(n))

struct Coverage
{
  	long   first_char;
  	ushort num_chars;
};


class RangeMap 
{
public:
	RangeMap();
	~RangeMap();
	
	long lookupRange(long charcode);
	
public:
  	ushort     num_coverages;
  	Coverage * coverages;
  	ushort   * indices;
};

RangeMap::RangeMap()
{
	num_coverages = 0;
	coverages = 0;
	indices = 0;
}

RangeMap::~RangeMap()
{
	if (coverages)
	{
		free(coverages);
		coverages = 0;
	}
	
	if (indices)
	{
		free(indices);
		indices = 0;
	}
}

long RangeMap::lookupRange(long charcode)
{
	for (long idx = num_coverages - 1; idx >= 0 && charcode >= coverages[idx].first_char; idx--) 
	{
    	if (charcode <= coverages[idx].first_char + coverages[idx].num_chars)
      		return indices[idx];
  	}

  	return -1;
}

class CharMap
{
public:
	CharMap();
	~CharMap();
	
	long lookupChar(long charcode);

public:
  	Coverage coverage;
  	ushort * indices;
};

CharMap::CharMap()
{
	coverage.first_char = 0;
	coverage.num_chars = 0;
	indices = 0;
}

CharMap::~CharMap()
{
	if (indices)
	{
		free(indices);
		indices = 0;
	}
}

long CharMap::lookupChar(long charcode)
{
	if (charcode >= coverage.first_char &&
      	charcode <= coverage.first_char + coverage.num_chars)
    	return indices[charcode - coverage.first_char];

  	return -1;
}

TFMFile::TFMFile(QObject * parent)
	:QObject(parent),
	 id(0),
	 nt(0),
	 level(0),
	 lf(0),
	 lh(0),
	 bc(0),
	 ec(0),
	 nw(0),
	 nh(0),
	 nd(0),
	 ni(0),
	 nl(0),
	 nk(0),
	 np(0),
	 fontdir(0),
	 nco(0),
	 ncw(0),
	 npc(0),
	 header(0),
	 chartypes(0),
	 charinfo(0),
	 width_index(0),
	 height_index(0),
	 depth_index(0),
	 width(0),
	 height(0),
	 depth(0)
{
}

TFMFile::~TFMFile()
{
	if (header)
	{
		free(header);
		header = 0;
	}
		
	if (chartypes)
	{
		free(chartypes);
		chartypes = 0;
	}
		
	if (charinfo)
	{
		free(charinfo);
		charinfo = 0;
	}
		
	if (width_index)
	{
		free(width_index);
		width_index = 0;
	}
		
	if (height_index)
	{
		free(height_index);
		height_index = 0;
	}
		
	if (depth_index)
	{
		free(depth_index);
		depth_index = 0;
	}
		
	if (width)
	{
		free(width);
		width = 0;
	}
		
	if (height)
	{
		free(height);
		height = 0;
	}
		
	if (depth)
	{
		free(depth);
		depth = 0;
	}
}

bool TFMFile::readFile(QIODevice * file)
{
	level = getSignedPair(file);
	file->seek(0);
	if (level != 0)
	{
		level = -1;
		return readTFM(file);
	}
	
	return readOFM(file);
}

bool TFMFile::checkOFMSizeOne(QIODevice *ofm_file)
{
	ulong ofm_size = 14;
  	ofm_size += 2 * (ec - bc + 1);
  	ofm_size += lh;
  	ofm_size += nw;
  	ofm_size += nh;
  	ofm_size += nd;
  	ofm_size += ni;
  	ofm_size += 2 * nl;
  	ofm_size += nk;
  	ofm_size += 2 * ne;
  	ofm_size += np;
  	qint64 ofm_file_size = ofm_file->size();
  	if (lf != ofm_file_size / 4 || lf != ofm_size) 
    	return false;
  	
  	return true;
}

bool TFMFile::checkTFMSize(QIODevice * tfm_file)
{
	ulong expected_size = 6;
	
	qint64 tfm_file_size = tfm_file->size();
	
	if (tfm_file_size < lf * 4) 
    	return false;
    	
    expected_size += (ec - bc + 1);
  	expected_size += lh;
  	expected_size += nw;
  	expected_size += nh;
  	expected_size += nd;
  	expected_size += ni;
  	expected_size += nl;
  	expected_size += nk;
  	expected_size += ne;
  	expected_size += np;
  	if (IS_JFM(id)) 
    	expected_size += nt + 1;
    	
    if (expected_size != lf) 
    {
    	if (tfm_file_size <= expected_size *4) 
    		return false;
  	}
  	
  	return true;
}

void TFMFile::doJFMCharTypeArray(QIODevice *tfm_file)
{
	chartypes = (long*)malloc(65536 * sizeof(long));
  	for (long i = 0; i < 65536; i++) 
    	chartypes[i] = 0;
    	
  	for (long i = 0; i < nt; i++) 
  	{
    	long charcode = getUnsignedPair(tfm_file);
    	long chartype = getUnsignedPair(tfm_file);
    	chartypes[charcode] = chartype;
  	}
}

bool TFMFile::doOFMCharInfoOne(QIODevice *ofm_file)
{
	ulong num_char_infos = ncw / (3 + (npc / 2));
  	ulong num_chars      = ec - bc + 1;
  	
  	if (num_chars != 0)
  	{
  		width_index  = (long*)malloc(num_chars * sizeof(long));
    	height_index = (int*)(num_chars * sizeof(int));
    	depth_index  = (int*)(num_chars * sizeof(int));
    	ulong char_infos_read   = 0;
    	for (ulong i = 0; i < num_chars && char_infos_read < num_char_infos; i++)
    	{
    		width_index[i] = getUnsignedPair(ofm_file);
      		height_index[i] = getUnsignedByte(ofm_file);
      		depth_index[i] = getUnsignedByte(ofm_file);
      		getUnsignedQuad(ofm_file);
      		int repeats = getUnsignedPair(ofm_file);
      		for (int j = 0; j < npc; j++) 
				getUnsignedPair(ofm_file);
				
			if (ISEVEN(npc))
				getUnsignedPair(ofm_file);
				
			char_infos_read++;
      		if (i + repeats > num_chars) 
      		{
				xwApp->error(tr("repeats causes number of characters to be exceeded.\n"));
				return false;
      		}
      		
      		for (int j = 0; j < repeats; j++) 
      		{
				width_index [i+j+1] = width_index [i];
				height_index[i+j+1] = height_index[i];
				depth_index [i+j+1] = depth_index [i];
      		}
      		i += repeats;
    	}
  	}
  	
  	return true;
}

void TFMFile::doOFMCharInfoZero(QIODevice *ofm_file)
{
	ulong num_chars = ec - bc + 1;
  	if (num_chars != 0) 
  	{
    	width_index  = (long*)malloc(num_chars * sizeof(long));
    	height_index = (int*)malloc(num_chars * sizeof(int));
    	depth_index  = (int*)malloc(num_chars * sizeof(int));
    	for (ulong i = 0; i < num_chars; i++) 
    	{
      		width_index [i] = getUnsignedPair(ofm_file);
      		height_index[i] = getUnsignedByte(ofm_file);
      		depth_index [i] = getUnsignedByte(ofm_file);
      		getUnsignedQuad(ofm_file);
    	}
  	}
}

bool TFMFile::getOFMSizes(QIODevice *ofm_file)
{
	level = getSignedQuad(ofm_file);
  	lf = getSignedQuad(ofm_file);
  	lh = getSignedQuad(ofm_file);
  	bc = getSignedQuad(ofm_file);
  	ec = getSignedQuad(ofm_file);
  	if (ec < bc) 
    	return false;
  	
  	nw = getSignedQuad(ofm_file);
  	nh = getSignedQuad(ofm_file);
  	nd = getSignedQuad(ofm_file);
  	ni = getSignedQuad(ofm_file);
  	nl = getSignedQuad(ofm_file);
  	nk = getSignedQuad(ofm_file);
  	ne = getSignedQuad(ofm_file);
  	np = getSignedQuad(ofm_file);
  	fontdir  = getSignedQuad(ofm_file);
    	
    bool ret = true;
  	if (level == 0) 
  	{
  		ncw = 2 * (ec - bc + 1);
    	ret = checkOFMSizeOne(ofm_file);
    }
  	else if (level == 1) 
  	{
    	nco = getSignedQuad(ofm_file);
    	ncw = getSignedQuad(ofm_file);
    	npc = getSignedQuad(ofm_file);
    	ofm_file->seek(4*(nco - lh));
  	} 
  	else 
    	ret = false;

  	return ret;
}

bool TFMFile::getTFMSizes(QIODevice * tfm_file)
{
	{
		long first_hword = getUnsignedPair(tfm_file);
    	if (IS_JFM(first_hword)) 
    	{
      		id = first_hword;
      		nt = getUnsignedPair(tfm_file);
      		lf = getUnsignedPair(tfm_file);
    	} 
    	else 
      		lf = first_hword;
	}
	
	lh = getUnsignedPair(tfm_file);
  	bc = getUnsignedPair(tfm_file);
  	ec = getUnsignedPair(tfm_file);
  	if (ec < bc || ec > 255) 
  		return false;
  	
  	nw = getUnsignedPair(tfm_file);
  	nh = getUnsignedPair(tfm_file);
  	nd = getUnsignedPair(tfm_file);
  	ni = getUnsignedPair(tfm_file);
  	nl = getUnsignedPair(tfm_file);
  	nk = getUnsignedPair(tfm_file);
  	ne = getUnsignedPair(tfm_file);
  	np = getUnsignedPair(tfm_file);
  	ncw = (ec - bc + 1);

  	return checkTFMSize(tfm_file);
}

bool TFMFile::readOFM(QIODevice *ofm_file)
{
	if (!getOFMSizes(ofm_file))
		return false;

  	if (level < 0 || level > 1)
    	return false;

  	if (lh > 0) 
  	{
    	header = (long*)malloc(lh * sizeof(long));
    	readWords(header, lh, ofm_file);
  	}
  	
  	if (level == 0) 
    	doOFMCharInfoZero(ofm_file);
  	else if (level == 1) 
    	doOFMCharInfoOne(ofm_file);
  	
  	if (nw > 0) 
  	{
    	width = (long*)malloc(nw * sizeof(long));
    	readWords(width, nw, ofm_file);
  	}
  	
  	if (nh > 0) 
  	{
    	height = (long*)malloc(nh * sizeof(long));
    	readWords(height, nh, ofm_file);
  	}
  	
  	if (nd > 0) 
  	{
    	depth = (long*)malloc(nd * sizeof(long));
    	readWords(depth, nd, ofm_file);
  	}
  	
  	return true;
}

bool TFMFile::readTFM(QIODevice *tfm_file)
{
	if (!getTFMSizes(tfm_file))
		return false;
		
	if (lh > 0) 
	{
    	header = (long*)malloc(lh * sizeof(long));
    	readWords(header, lh, tfm_file);
  	}
  	
  	if (IS_JFM(id))
  		doJFMCharTypeArray(tfm_file);
  		
  	if (ec - bc + 1 > 0) 
  	{
    	charinfo = (ulong*)malloc((ec - bc + 1) * sizeof(ulong));
    	readUQuads(charinfo, ec - bc + 1, tfm_file);
  	}
  	
  	if (nw > 0) 
  	{
    	width = (long*)malloc(nw * sizeof(long));
    	readWords(width, nw, tfm_file);
  	}
  	
  	if (nh > 0) 
  	{
    	height = (long*)malloc(nh * sizeof(long));
    	readWords(height, nh, tfm_file);
  	}
  	
  	if (nd > 0) 
  	{
    	depth = (long*)malloc(nd * sizeof(long));
    	readWords(depth, nd, tfm_file);
  	}
  	
  	return true;
}

long TFMFile::readUQuads(ulong *quads, long nmemb, QIODevice *fp)
{
	for (long i = 0; i < nmemb; i++) 
    	quads[i] = getUnsignedQuad(fp);

  	return nmemb*4;
}

long TFMFile::readWords(long * words, long nmemb, QIODevice *fp)
{
	for (long i = 0; i < nmemb; i++)
    	words[i] = getSignedQuad(fp);

  	return nmemb*4;
}

FontMetric::FontMetric(QObject * parent)
	:QObject(parent)
{
	designSize = 0;
	codingScheme = 0;
	fontDir = FONT_DIR_HORIZ;
	firstChar = 0;
	lastChar = 0;
	widths = 0;
	heights = 0;
	depths = 0;
	
	charMap.type = MAPTYPE_NONE;
  	charMap.data = 0;
  
	source = SOURCE_TYPE_TFM;
}

FontMetric::~FontMetric()
{
	if (codingScheme)
	{
		free(codingScheme);
		codingScheme = 0;
	}
	
	if (widths)
	{
		free(widths);
		widths = 0;
	}
	
	if (heights)
	{
		free(heights);
		heights = 0;
	}
	
	if (depths)
	{
		free(depths);
		depths = 0;
	}
	
	switch (charMap.type) 
	{
    	case MAPTYPE_CHAR:
    		{
    			CharMap * m = (CharMap*)(charMap.data);
    			delete m;
    			charMap.data = 0;
    		}
      		break;
      		
    	case MAPTYPE_RANGE:
    		{
    			RangeMap * m = (RangeMap*)(charMap.data);
      			delete m;
    			charMap.data = 0;
      		}
      		break;
    }
}

long FontMetric::getDepth(long ch)
{
	long idx = lookupChar(ch);
	if (idx == -1)
		return -1;
		
  	return depths[idx];
}

long FontMetric::getHeight(long ch)
{
	long idx = lookupChar(ch);
	if (idx == -1)
		return -1;
		
  	return heights[idx];
}

long FontMetric::getWidth(long ch)
{
	long idx = lookupChar(ch);
	if (idx == -1)
		return -1;
		
  	return widths[idx];
}

void FontMetric::readOFM(TFMFile * tfm)
{
	unpackOFMArrays(tfm, tfm->ec - tfm->bc + 1);
  	unpackTFMHeader(tfm);
  	firstChar = tfm->bc;
  	lastChar  = tfm->ec;
  	source    = SOURCE_TYPE_OFM;
}

void FontMetric::readTFM(TFMFile * tfm)
{
	firstChar = tfm->bc;
  	lastChar  = tfm->ec;
  	if (IS_JFM(tfm->id))
  	{
  		makeJFMCharMap(tfm);
    	firstChar = 0;
    	lastChar  = 0xFFFFl;
    	fontDir   = (tfm->id == JFMV_ID) ? FONT_DIR_VERT : FONT_DIR_HORIZ;
    	source    = SOURCE_TYPE_JFM;
  	}
  	
  	unpackTFMArrays(tfm);
  	unpackTFMHeader(tfm);
}

long FontMetric::lookupChar(long ch)
{
	long idx = -1;
	if (ch >= firstChar && ch <= lastChar) 
	{
    	switch (charMap.type) 
    	{
    		case MAPTYPE_CHAR:
      			idx = ((CharMap*)(charMap.data))->lookupChar(ch);
      			break;
      			
    		case MAPTYPE_RANGE:
      			idx = ((RangeMap*)(charMap.data))->lookupRange(ch);
      			break;
      			
    		default:
      			idx = ch;
    	}
  	} 

	if (idx < 0)
	{
		QString msg = QString(tr("invalid char: %1\n")).arg(ch);
      	xwApp->error(msg);
      	return -1;
	}
	
	return idx;
}

void FontMetric::makeJFMCharMap(TFMFile *tfm)
{
	if (tfm->nt > 1) 
	{
		CharMap * map = 0;
    	charMap.type = MAPTYPE_CHAR;
    	charMap.data = map = new CharMap;
    	map->coverage.first_char = 0;
    	map->coverage.num_chars  = 0xFFFFu;
    	map->indices = (ushort*)malloc(0x10000L * sizeof(ushort));

    	for (long code = 0; code <= 0xFFFFu; code++) 
      		map->indices[code] = tfm->chartypes[code];
  	} 
  	else 
  	{
    	RangeMap *map = 0;
    	charMap.type = MAPTYPE_RANGE;
    	charMap.data = map = new RangeMap;
    	map->num_coverages = 1;
    	map->coverages     = (Coverage*)malloc(map->num_coverages * sizeof(Coverage));
    	map->coverages[0].first_char = 0;
    	map->coverages[0].num_chars  = 0xFFFFu;
    	map->indices = (ushort*)malloc(sizeof(ushort));
    	map->indices[0] = 0;
  	}
}

int FontMetric::sputBigEndian(char *s, long v, int n)
{
	for (int i = n-1; i >= 0; i--) 
	{
    	s[i] = (char) (v & 0xff);
    	v >>= 8;
  	}

  	return n;
}

void FontMetric::unpackOFMArrays(TFMFile *tfm, ulong num_chars)
{
	widths  = (long*)malloc((tfm->bc + num_chars) * sizeof(long));
  	heights = (long*)malloc((tfm->bc + num_chars) * sizeof(long));
  	depths  = (long*)malloc((tfm->bc + num_chars) * sizeof(long));
  	for (ulong i = 0; i < num_chars; i++) 
  	{
    	widths [tfm->bc + i] = tfm->width [ tfm->width_index [i] ];
    	heights[tfm->bc + i] = tfm->height[ tfm->height_index[i] ];
    	depths [tfm->bc + i] = tfm->depth [ tfm->depth_index [i] ];
  	}
}

void FontMetric::unpackTFMArrays(TFMFile *tfm)
{
	widths  = (long*)malloc(256 * sizeof(long));
  	heights = (long*)malloc(256 * sizeof(long));
  	depths  = (long*)malloc(256 * sizeof(long));
  	for (int i = 0; i < 256; i++) 
  	{
    	widths [i] = 0;
    	heights[i] = 0;
    	depths [i] = 0;
  	}

  	for (long i = tfm->bc; i <= tfm->ec; i++ ) 
  	{
    	long char_info     = tfm->charinfo[i - tfm->bc];
    	long width_index  = (char_info / 16777216ul);
    	long height_index = (char_info / 0x100000ul) & 0xf;
    	long depth_index  = (char_info / 0x10000ul)  & 0xf;
    	widths [i] = tfm->width [width_index];
    	heights[i] = tfm->height[height_index];
    	depths [i] = tfm->depth [depth_index];
  	}
}

bool FontMetric::unpackTFMHeader(TFMFile *tfm)
{
	if (tfm->lh < 12) 
    	codingScheme = 0;
  	else 
  	{
    	int len = (tfm->header[2] >> 24);
    	if (len < 0 || len > 39)
    	{
      		xwApp->error(tr("invalid TFM header.\n"));
      		return false;
      	}
      	
    	if (len > 0) 
    	{
      		codingScheme = (char*)malloc(40 * sizeof(char));
      		char * p = codingScheme;
      		p += sputBigEndian(p, tfm->header[2], 3);
      		for (int i = 1; i <= len / 4; i++) 
				p += sputBigEndian(p, tfm->header[2+i], 4);
				
      		codingScheme[len] = '\0';
    	} 
    	else 
      		codingScheme = 0;
  	}

  	designSize = tfm->header[1];
  	
  	return true;
}

bool XWTFMFile::needMetrics = false;
FontMetric ** XWTFMFile::fms = 0;
int XWTFMFile::numFMs = 0;
int XWTFMFile::maxFMs = 0;
static QHash<QString, int> ids;

static QMutex tfmMutex;

XWTFMFile::XWTFMFile(int idA, QObject * parent)
	:QObject(parent),
	 id(idA),
	 file(0)
{
}

XWTFMFile::XWTFMFile(const char * nameA, QObject * parent)
	:QObject(parent),
	 id(-1),
	 file(0)
{
	tfmMutex.lock();
	if (ids.contains(nameA))
		id = ids[nameA];
	else
	{	
		QString fn(nameA);
		int idx = fn.indexOf(".");
		if (idx > 0)
		{
			if (!fn.endsWith(".tfm", Qt::CaseInsensitive) && 
				  !fn.endsWith(".ofm", Qt::CaseInsensitive))
			{
				fn = fn.left(idx);
			}
		}
		
		XWFontSea sea;
		file = sea.openOfm(fn);
		if (!file)
			file = sea.openTFM(fn);
		
		if (!file)
		{
			XWTFMCreator creater(nameA);
			if (creater.isOK())
			{
				file = new QBuffer;
				file->open(QIODevice::WriteOnly);
				creater.writeTFM(file);
				file->close();
				file->open(QIODevice::ReadOnly);
			}
		}
	
		if (file && needMetrics)
		{
			TFMFile tfm;
			if (tfm.readFile(file))
			{
				FontMetric * fm = new FontMetric;
				if (tfm.level == -1)
					fm->readTFM(&tfm);
				else
					fm->readOFM(&tfm);
				
				metricNeed(numFMs + 1);
				fms[numFMs] = fm;
				id = numFMs;
				ids[nameA] = id;
				numFMs++;
			}
		
			file->seek(0);
		}
		else if (file)
			id = 0;
	}
	
	tfmMutex.unlock();
}

XWTFMFile::~XWTFMFile()
{
	if (file)
	{
		file->close();
		delete file;
	}
}

bool XWTFMFile::atEnd()
{
	return file->atEnd();
}

void XWTFMFile::closeAll()
{
	for (int i = 0; i < numFMs; i++)
	{
		FontMetric * fm = fms[i];
		if (fm)
			delete fm;
	}
	
	if (fms)
		free(fms);
		
	fms = 0;
	numFMs = 0;
	maxFMs = 0;
	ids.clear();
}

uchar XWTFMFile::getByte()
{
	uchar ret = 0;
	if (file)
		file->getChar((char*)(&ret));
	
	return ret;
}

long XWTFMFile::getFWDepth(long ch)
{
	if (id < 0 || id >= numFMs)
		return -1;
		
	return fms[id]->getDepth(ch);
}

long XWTFMFile::getFWHeight(long ch)
{
	if (id < 0 || id >= numFMs)
		return -1;
		
	return fms[id]->getHeight(ch);
}

long XWTFMFile::getFWWidth(long ch)
{
	if (id < 0 || id >= numFMs)
		return -1;
		
	return fms[id]->getWidth(ch);
}

double XWTFMFile::getDepth(long ch)
{
	long d = getFWDepth(ch);
	if (d == -1)
		return 0.0;
		
	return (double)(d / FWBASE);
}

double XWTFMFile::getDesignSize()
{
	if (id < 0 || id >= numFMs)
		return 10.0;
		
	return (double) ((fms[id]->designSize) /FWBASE*(72.0/72.27));
}

double XWTFMFile::getHeight(long ch)
{
	long h = getFWHeight(ch);
	if (h == -1)
		return 0.0;
		
	return (double)(h / FWBASE);
}

long  XWTFMFile::getStringDepth(const uchar *s, long len)
{
	if (id < 0 || id >= numFMs)
		return 0;
		
	FontMetric * fm = fms[id];
	long result = -1;
	if (fm->source == SOURCE_TYPE_JFM)
	{
		for (int i = 0; i < len / 2; i++)
		{
			long ch = (s[2*i] << 8) | s[2*i+1];
      		result += fm->getDepth(ch);
      	}
	}
	else
	{
		for (int i = 0; i < len; i++)
		{
			long tmp = fm->getDepth(s[i]);
			result = qMax(result, tmp);
		}
	}
	
	return result;
}

long XWTFMFile::getStringHeight(const uchar *s, long len)
{
	if (id < 0 || id >= numFMs)
		return 0;
		
	FontMetric * fm = fms[id];
	long result = -1;
	if (fm->source == SOURCE_TYPE_JFM)
	{
		for (int i = 0; i < len / 2; i++)
		{
			long ch = (s[2*i] << 8) | s[2*i+1];
      		result += fm->getHeight(ch);
      	}
	}
	else
	{
		for (int i = 0; i < len; i++)
		{
			long tmp = fm->getHeight(s[i]);
			result = qMax(result, tmp);
		}
	}
	
	return result;
}

long XWTFMFile::getStringWidth(const uchar *s, long len)
{
	if (id < 0 || id >= numFMs)
		return 0;
		
	FontMetric * fm = fms[id];
	long result = -1;
	if (fm->source == SOURCE_TYPE_JFM)
	{
		for (int i = 0; i < len / 2; i++)
		{
			long ch = (s[2*i] << 8) | s[2*i+1];
      		result += fm->getWidth(ch);
      	}
	}
	else
	{
		for (int i = 0; i < len; i++)
			result += fm->getWidth(s[i]);
	}
	
	return result;
}

double XWTFMFile::getWidth(long ch)
{
	long w = getFWWidth(ch);
	if (w == -1)
		return 0.0;
		
	return (double)(w / FWBASE);
}

long XWTFMFile::readSixteen()
{
	long ret = getByte();
	if (ret <= 127)
		ret = ret * 256 + getByte();
	
	return ret;
}

long XWTFMFile::readSixteenUnsigned()
{
	long ret = getByte();
	ret = ret * 256 + getByte();
	return ret;
}

long XWTFMFile::readThirtyTwo()
{
	long ret = getByte();
	if (ret <= 127)
	{
		ret = ret * 256 + getByte();
		ret = ret * 256 + getByte();
		ret = ret * 256 + getByte();
	}
	
	return ret;
}

void XWTFMFile::setNeedMetric(bool e)
{
	needMetrics = e;
}

void XWTFMFile::metricNeed(int n)
{
	if (n > maxFMs)
	{
		maxFMs += 16;
		fms = (FontMetric**)realloc(fms, maxFMs * sizeof(FontMetric*));
	}
}

