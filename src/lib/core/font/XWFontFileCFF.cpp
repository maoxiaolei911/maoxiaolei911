/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWApplication.h"
#include "XWFontFileType1.h"
#include "XWPFBParser.h"
#include "cffstdstr.h"
#include "XWCFFType.h"
#include "XWFontFileCFF.h"

XWFontFileCFF::XWFontFileCFF()
	:XWFontFile(0, 0, 0, 0)
{
	fontname = 0;
	name = 0;
	topDict = 0;
	string = 0;
	gsubr = 0;
	encoding = 0;
	charsets = 0;
	fdselect = 0;
	cstrings = 0;
	fdarray = 0;
	privateDict = 0;
	subrs = 0;
	gsubr_offset = 0;
	num_fds = 0;
	_string = 0;
	filter = 0;
	index = 0;
	ft_to_gid = 0;
}

XWFontFileCFF::XWFontFileCFF(QIODevice * fileA,
		 	                 int    fileFinalA,
		 	                 ulong offsetA,
		 	                 ulong lenA,
		 	                 int  indexA)
	:XWFontFile(fileA, fileFinalA, offsetA, lenA)
{
	fontname = 0;
	name = 0;
	topDict = 0;
	string = 0;
	gsubr = 0;
	encoding = 0;
	charsets = 0;
	fdselect = 0;
	cstrings = 0;
	fdarray = 0;
	privateDict = 0;
	subrs = 0;
	gsubr_offset = 0;
	num_fds = 0;
	_string = 0;
	filter = 0;
	index = indexA;
	ft_to_gid = 0;
	seek(0);
	header = new XWCFFHeader(this);
	if (header->offsize < 1 || header->offsize > 4)
	{
    	xwApp->error(tr("invalid offsize data.\n"));
    	return;
    }
  	
	name = new XWCFFIndex(this, false);
	if (index > name->count - 1) 
  	{
    	xwApp->warning(tr("invalid CFF fontset index number.\n"));
    	return ;
  	}
  
	fontname = name->getName(index);
	
	XWCFFIndex tdidx(this, false);
	if (indexA > tdidx.count - 1)
	{
    	xwApp->error(tr("CFF Top DICT not exist...\n"));
    	return ;
    }
    
	topDict = new XWCFFDict(tdidx.data + tdidx.offset[indexA] - 1, tdidx.data + tdidx.offset[indexA + 1] - 1);
	if (!topDict->isOK())
	{
		xwApp->error(tr("parsing CFF Top DICT data failed...\n"));
		return ;
	}
	
	string = new XWCFFIndex(this, false);
	gsubr_offset = (ulong)(file->pos() - offsetA);
	offsetA = (ulong)(topDict->get("CharStrings", 0));
	seek(offsetA);
	nGlyphs = getCard16();
	
	if (topDict->known("ROS")) 
    	flag |= FONTTYPE_CIDFONT;
  	else 
    	flag |= FONTTYPE_FONT;
    	
    if (topDict->known("Encoding")) 
    {
    	offsetA = (ulong)(topDict->get("Encoding", 0));
    	if (offsetA == 0) 
    		flag |= ENCODING_STANDARD;
    	else if (offsetA == 1) 
      		flag |= ENCODING_EXPERT;
  	} 
  	else 
    	flag |= ENCODING_STANDARD;
    	
    if (topDict->known("charset")) 
    {
    	offsetA = (ulong)(topDict->get("charset", 0));
    	if (offsetA == 0) 
      		flag |= CHARSETS_ISOADOBE;
    	else if (offsetA == 1) 
      		flag |= CHARSETS_EXPERT;
    	else if (offsetA == 2) 
      		flag |= CHARSETS_EXPSUB;
  	} 
  	else 
    	flag |= CHARSETS_ISOADOBE;

  	seek(gsubr_offset);
  	
  	ok = true;
}

XWFontFileCFF::~XWFontFileCFF()
{
	if (fontname)
		delete [] fontname;
		
	if (header)
		delete header;
		
	if (name)
		delete name;
		
	if (topDict)
		delete topDict;
		
	if (string)
		delete string;
		
	if (gsubr)
		delete gsubr;
		
	if (encoding)
		delete encoding;
		
	if (charsets)
		delete charsets;
		
	if (fdselect)
		delete fdselect;
		
	if (cstrings)
		delete cstrings;
		
	if (fdarray)
	{
		for (int i = 0; i < num_fds; i++)
		{
			if (fdarray[i])
				delete fdarray[i];
		}
		
		free(fdarray);
	}
	
	if (privateDict)
	{
		for (int i = 0; i < num_fds; i++)
		{
			if (privateDict[i])
				delete privateDict[i];
		}
		
		free(privateDict);
	}
	
	if (subrs)
	{
		for (int i = 0; i < num_fds; i++)
		{
			if (subrs[i])
				delete subrs[i];
		}
		
		free(subrs);
	}
	
	if (_string)
		delete _string;
}

ushort XWFontFileCFF::addString(const char *str, int unique)
{
	if (_string == 0)
    	_string = new XWCFFIndex((ushort)0);
    	
    return _string->addString(str, unique);
}

void XWFontFileCFF::createFDArray(int c)
{
	fdarray = (XWCFFDict**)malloc(c * sizeof(XWCFFDict*));
	for (int i = 0; i < c; i++)
		fdarray[i] = new XWCFFDict;
		
	num_fds = 1;
}

long XWFontFileCFF::cstringSize()
{
	if (!cstrings)
		return 0;
		
	return cstrings->size();
}

void XWFontFileCFF::fdarrayAdd(int i, const char *key, int countA)
{
	if (i < 0 || i >= num_fds)
		return ;
		
	fdarray[i]->add(key, countA);
}

long XWFontFileCFF::fdarrayPack(int i, uchar *dest, long destlen)
{
	if (i < 0 || i >= num_fds)
		return 0;
		
	return fdarray[i]->pack(dest, destlen);
}

void XWFontFileCFF::fdarrayRemove(int i, const char * key)
{
	if (i < 0 || i >= num_fds)
		return ;
		
	fdarray[i]->remove(key);
}

void XWFontFileCFF::fdarraySet(int i, const char *key, int idx, double value)
{
	if (i < 0 || i >= num_fds)
		return ;
		
	fdarray[i]->set(key, idx, value);
}

XWCFFIndex * XWFontFileCFF::getIndexHeader()
{
	return new XWCFFIndex(this, true);
}

char * XWFontFileCFF::getName()
{
	if (!name)
		return 0;
		
	return name->getName(index);
}

char * XWFontFileCFF::getOrdering()
{
	ushort sidA = (ushort)(topDictGet("ROS", 1));
	return getString(sidA);
}

char * XWFontFileCFF::getRegistry()
{
	ushort sidA = (ushort)topDictGet("ROS", 0);
	return getString(sidA);
}

long  XWFontFileCFF::getSID(char *str)
{
	if (string)
		return string->getSID(str);
		
	return -1;
}

char * XWFontFileCFF::getString(ushort id)
{
	char * result = 0;
	if (id < CFF_STDSTR_MAX) 
    	result = qstrdup(cff_stdstr[id]);
	else if (string)
		result = string->getString(id);
		
	return result;
}

int XWFontFileCFF::getSupplement()
{
	return (int)topDictGet("ROS", 2);
}

long XWFontFileCFF::gsubrSize()
{
	if (!gsubr)
		return 0;
		
	return gsubr->size();
}

XWFontFileCFF * XWFontFileCFF::loadType1(char *fileName,
	                                     char **enc_vec, 
	                                     int mode)
{
	XWFontFileType1 * filet1 = XWFontFileType1::loadPFB(fileName, false);
	if (!filet1)
		return 0;
		
	filet1->seek(0);
	long lengthA = 0;
	uchar * buffer = filet1->getPFBSegment(PFB_SEG_TYPE_ASCII, &lengthA);
  	if (buffer == 0 || lengthA == 0) 
  	{
  		delete filet1;
    	return 0;
    }
    	
    XWFontFileCFF * cff = new XWFontFileCFF;
    cff->flag = FONTTYPE_FONT;
    cff->header = new XWCFFHeader;
    cff->header->major = 1;
    cff->header->minor = 0;
    cff->header->hdr_size = 4;
  	cff->header->offsize  = 4;
  	cff->name = new XWCFFIndex((ushort)1);
  	cff->topDict  = new XWCFFDict;
  	cff->gsubr    = new XWCFFIndex((ushort)0);
  	cff->privateDict  = (XWCFFDict**)malloc(sizeof(XWCFFDict*));
  	cff->privateDict[0] = new XWCFFDict();
  	cff->subrs = (XWCFFIndex**)malloc(sizeof(XWCFFIndex*));
  	cff->subrs[0] = 0;
  	cff->num_fds  = 1;
  	cff->_string = new XWCFFIndex((ushort)0);
  	
  	uchar * startA = buffer;
  	uchar * endA = buffer + lengthA;
  	if (parsePart1(cff, startA, endA, enc_vec) < 0) 
  	{
    	delete cff;
    	free(buffer);
    	delete filet1;
    	return 0;
  	}
  	free(buffer);
  	buffer = filet1->getPFBSegment(PFB_SEG_TYPE_BINARY, &lengthA);
  	if (buffer == 0 || lengthA == 0) 
    {
    	delete cff;
    	delete filet1;
    	return 0;
    }
    else
    	filet1->decrypt(T1_EEKEY, buffer, buffer, 0, lengthA);
    	
    startA = buffer + 4; 
    endA = buffer + lengthA;    
    if (parsePart2(filet1, cff, startA, endA, mode) < 0) 
    {
    	delete cff;
    	free(buffer);
    	delete filet1;
    	return 0;
  	}
  	free(buffer);
	delete filet1;
  	cff->updateString();

  	return cff;
}

ushort XWFontFileCFF::lookupChartsets(ushort cid)
{
	if (flag & (CHARSETS_ISOADOBE|CHARSETS_EXPERT|CHARSETS_EXPSUB)) 
	{
    	xwApp->error(tr("Predefined CFF charsets not supported yet.\n"));
    	return 0;
  	} 
  	else if (charsets == NULL) 
  	{
    	xwApp->error(tr("charsets data not available.\n"));
    	return 0;
  	}

  	if (cid == 0) 
    	return 0;
    	
    ushort gid = charsets->lookup(cid);
//  	if (gid == 0)
//  		xwApp->error(tr("failt to look up gid(charsets format error).\n"));
  		
  	return gid;
}

ushort XWFontFileCFF::lookupChartsetsInverse(ushort gid)
{
	if (flag & (CHARSETS_ISOADOBE|CHARSETS_EXPERT|CHARSETS_EXPSUB)) 
	{
    	xwApp->error(tr("Predefined CFF charsets not supported yet.\n"));
    	return 0;
  	} 
  	else if (charsets == NULL) 
  	{
    	xwApp->error(tr("charsets data not available.\n"));
    	return 0;
  	}

  	if (gid == 0) 
    	return 0;
    	
    ushort sid = charsets->lookupInverse(gid);
//  	if (sid == 0)
//  		xwApp->error(tr("failt to look up sid(charsets format error or invalid gid).\n"));
  		
  	return sid;
}

ushort XWFontFileCFF::lookupEncoding(ushort code)
{
	if (flag & (ENCODING_STANDARD|ENCODING_EXPERT)) 
	{
    	xwApp->error(tr("Predefined CFF encoding not supported yet.\n"));
    	return 0;
  	} 
  	else if (encoding == 0) 
  	{
    	xwApp->error(tr("encoding data not available.\n"));
    	return 0;
  	}
  	
  	ushort gid = encoding->lookup(this, code);
//  	if (gid == 0)
//  		xwApp->error(tr("failt to look up gid(encoding format error or no CFF supplementary encoding data).\n"));
  		
  	return gid;
}

uchar  XWFontFileCFF::lookupFDSelect(ushort gid)
{
	if (fdselect == 0)
	{
    	xwApp->error(tr("FDSelect not available.\n"));
    	return 0;
    }

  	if (gid >= getNumGlyphs())
  	{
    	xwApp->error(tr("invalid glyph index.\n"));
    	return 0;
    }
    
    uchar fd = fdselect->lookup(gid);
//    if (fd >= num_fds)
//    	xwApp->error(tr("invalid Font DICT index.\n"));
    	
    return fd;
}

ushort XWFontFileCFF::lookupGlyph(const char *glyph)
{
	if (flag & (CHARSETS_ISOADOBE|CHARSETS_EXPERT|CHARSETS_EXPSUB)) 
	{
    	xwApp->error(tr("Predefined CFF charsets not supported yet.\n"));
    	return 0;
  	} 
  	else if (charsets == 0) 
  	{
    	xwApp->error(tr("charsets data not available.\n"));
    	return 0;
  	}
  	
  	if (!glyph || !strcmp(glyph, ".notdef")) 
    	return 0;
    	
    ushort gid = charsets->lookup(this, glyph);
//    if (gid == 0)
//  		xwApp->error(tr("failt to look up gid(charsets format error).\n"));
  		
  	return gid;
}

XWFontFileCFF * XWFontFileCFF::make(QIODevice * fileA, 
	                                int fileFinalA,
	                                ulong offsetA,
		 	                        ulong lenA,
		 	                        int  indexA)
{
	return new XWFontFileCFF(fileA, fileFinalA, offsetA, lenA, indexA);
}

int XWFontFileCFF::matchString(const char *str, ushort sid)
{
	if (sid < CFF_STDSTR_MAX) 
    	return ((!strcmp(str, cff_stdstr[sid])) ? 1 : 0);
	else if (string)
		return string->matchString(str, sid);
		
	return 0;
}

long XWFontFileCFF::nameSize()
{
	if (!name)
		return 0;
		
	return name->size();
}

void XWFontFileCFF::newString()
{
	if (_string)
		delete _string;
		
	_string = new XWCFFIndex((ushort)0);
}

long XWFontFileCFF::packCharsets(uchar *dest, long destlen)
{
	if (flag & HAVE_STANDARD_CHARSETS || charsets == 0)
    	return 0;

  	if (destlen < 1)
    	return -1;
    	
    long len = charsets->pack(dest, destlen);
    if (len < 0)
    	xwApp->error(tr("fail to pack charsets(format error or buffer overflow).\n"));
    	
    return len;
}

long XWFontFileCFF::packCString(uchar *dest, long destlen)
{
	if (!cstrings)
		return 0;
		
	return cstrings->pack(dest, destlen);
}

long XWFontFileCFF::packEncoding(uchar *dest, long destlen)
{
	if (flag & HAVE_STANDARD_ENCODING || encoding == 0)
    	return 0;
    	
    if (destlen < 2)
    	return -1;
    	
    long len = encoding->pack(dest, destlen);
    if (len < 0)
    	xwApp->error(tr("fail to pack encoding(format error or buffer overflow).\n"));
    	
    return len;
}

long XWFontFileCFF::packFDSelect(uchar *dest, long destlen)
{
	if (fdselect == 0)
    	return 0;

  	if (destlen < 1)
    	return -1;
    	
    long len = fdselect->pack(this, dest, destlen);
    if (len < 0)
    	xwApp->error(tr("fail to pack fdselect(format error or buffer overflow).\n"));
    	
    return len;
}

long XWFontFileCFF::packGsubr(uchar *dest, long destlen)
{
	if (!gsubr)
		return 0;
		
	return gsubr->pack(dest, destlen);
}

long XWFontFileCFF::packName(uchar *dest, long destlen)
{
	if (!name)
		return 0;
		
	return name->pack(dest, destlen);
}

long XWFontFileCFF::packString(uchar *dest, long destlen)
{
	if (!string)
		return 0;
		
	return string->pack(dest, destlen);
}

void XWFontFileCFF::privDictAdd(int i, const char *key, int countA)
{
	if (i < 0 || i >= num_fds)
		return ;
		
	privateDict[i]->add(key, countA);
}

double XWFontFileCFF::privDictGet(int i, const char * key, int idx)
{
	if (i < 0 || i >= num_fds || !privateDict[i])
		return 0;
		
	return privateDict[i]->get(key, idx);
}

bool XWFontFileCFF::privDictKnow(int i, const char * key)
{
	if (i < 0 || i >= num_fds || !privateDict[i])
		return false;
		
	return privateDict[i]->known(key) != 0;
}

long XWFontFileCFF::privDictPack(int i, uchar *dest, long destlen)
{
	if (i < 0 || i >= num_fds || !privateDict[i])
		return 0;
		
	return privateDict[i]->pack(dest, destlen);
}

void XWFontFileCFF::privDictRemove(int i, const char * key)
{
	if (i < 0 || i >= num_fds || !privateDict[i])
		return ;
		
	privateDict[i]->remove(key);
}

void XWFontFileCFF::privDictSet(int i, const char *key, int idx, double value)
{
	if (i < 0 || i >= num_fds || !privateDict[i])
		return ;
		
	privateDict[i]->set(key, idx, value);
}

void XWFontFileCFF::privDictUpdate(int i)
{
	if (i < 0 || i >= num_fds || !privateDict[i])
		return ;
		
	privateDict[i]->update(this);
}

long XWFontFileCFF::putHeader(uchar *dest, long destlen)
{
	if (destlen < 4)
	{
    	xwApp->error(tr("not enough space available...\n"));
    	return -1;
    }
    
    return header->put(dest);
}

long XWFontFileCFF::readCharsets()
{
	if (!topDict)
	{
    	xwApp->error(tr("Top DICT not available.\n"));
    	return -1;
    }

  	if (!topDict->known("charset")) 
  	{
    	flag |= CHARSETS_ISOADOBE;
    	charsets = 0;
    	return 0;
  	}
  	
  	long offsetA = (long)topDict->get("charset", 0);
  	if (offsetA == 0) 
  	{
    	flag |= CHARSETS_ISOADOBE;
    	charsets = 0;
    	return 0;
  	} 
  	else if (offsetA == 1) 
  	{
    	flag |= CHARSETS_EXPERT;
    	charsets = 0;
    	return 0;
  	} 
  	else if (offsetA == 2) 
  	{
    	flag |= CHARSETS_EXPSUB;
    	charsets = 0;
    	return 0;
  	}

  	seek(offsetA);
  	charsets = new XWCFFCharsets;  		
  	return charsets->read(this);
}

long XWFontFileCFF::readEncoding()
{
	if (!topDict)
	{
		xwApp->error(tr("Top DICT data not found.\n"));
		return -1;
	}
	
	if (!topDict->known("Encoding")) 
	{
    	flag |= ENCODING_STANDARD;
    	encoding = 0;
    	return 0;
  	}
  	
  	long offsetA = (long)topDict->get("Encoding", 0);
  	if (offsetA == 0) 
  	{
    	flag |= ENCODING_STANDARD;
    	encoding = 0;
    	return 0;
  	} 
  	else if (offsetA == 1) 
  	{
    	flag |= ENCODING_EXPERT;
    	encoding = 0;
    	return 0;
  	}
  	
  	seek(offsetA);
  	encoding = new XWCFFEncoding;  	
  	return encoding->read(this);
}

long XWFontFileCFF::readFDArray()
{
	if (topDict == 0)
	{
    	xwApp->error(tr("Top DICT not found.\n"));
    	return -1;
    }

  	if (!(flag & FONTTYPE_CIDFONT))
    	return 0;
    	
    long len = 0;
    
    long offsetA = (long)(topDict->get("FDArray", 0));
  	seek(offsetA);
  	XWCFFIndex idx(this, false);
  	num_fds = (ushort)(idx.count);
  	fdarray = (XWCFFDict**)malloc(idx.count * sizeof(XWCFFDict*));
  	for (uchar i = 0; i < idx.count; i++) 
  	{
    	uchar *data = idx.data + (idx.offset)[i] - 1;
    	long size = (idx.offset)[i + 1] - (idx.offset)[i];
    	if (size > 0) 
      		fdarray[i] = new XWCFFDict(data, data+size);
    	else 
      		fdarray[i] = 0;
  	}
  	
  	len = idx.size();
  	return len;
}

long XWFontFileCFF::readFDSelect()
{
	if (!topDict)
	{
    	xwApp->error(tr("Top DICT not available.\n"));
    	return -1;
    }

  	if (!(flag & FONTTYPE_CIDFONT))
    	return 0;
    	
    long offsetA = (long) topDict->get("FDSelect", 0);
    seek(offsetA);
    fdselect = new XWCFFFDSelect;    	
    return fdselect->read(this);
}

long XWFontFileCFF::readPrivate()
{
	long len = 0;
	long size = 0;
	if (flag & FONTTYPE_CIDFONT) 
	{
    	if (fdarray == 0)
      		readFDArray();

    	privateDict = (XWCFFDict**)malloc(num_fds * sizeof(XWCFFDict*));
    	for (uchar i = 0; i < num_fds; i++) 
    	{
      		if (fdarray[i] != 0 && 
      			fdarray[i]->known("Private") && 
	  			(size = (long) fdarray[i]->get("Private", 0)) > 0) 
	  		{
				long offsetA = (long) fdarray[i]->get("Private", 1);
				seek(offsetA);
				uchar * data = (uchar*)malloc(size * sizeof(uchar));
				if (read((char*)data, size) != size)
				{
					free(data);
	  				xwApp->error(tr("reading file failed.\n"));
	  				return -1;
	  				
	  			}
				privateDict[i] = new XWCFFDict(data, data+size);
				free(data);
				len += size;
      		} 
      		else 
				privateDict[i] = 0;
    	}
  	} 
  	else 
  	{
    	num_fds = 1;
    	privateDict = (XWCFFDict**)malloc(1 * sizeof(XWCFFDict*));
    	if (topDict->known("Private") && (size = (long) topDict->get("Private", 0)) > 0) 
		{
      		long offsetA = (long) topDict->get("Private", 1);
      		seek(offsetA);
      		uchar * data = (uchar*)malloc(size * sizeof(uchar));
      		if (read((char*)data, size) != size)
      		{
				free(data);
	  			xwApp->error(tr("reading file failed.\n"));
	  			return -1;
			}
      		privateDict[0] = new XWCFFDict(data, data+size);
      		free(data);
      		len += size;
    	} 
    	else 
    	{
      		privateDict[0] = 0;
      		len = 0;
    	}
  	}

  	return len;
}

long XWFontFileCFF::readSubrs()
{
	if ((flag & FONTTYPE_CIDFONT) && fdarray == 0) 
    	readFDArray();

  	if (privateDict == 0)
    	readPrivate();

  	if (gsubr == 0) 
  	{
    	seek(gsubr_offset);
    	gsubr = new XWCFFIndex(this, false);
  	}
  	
  	long len = 0;
  	subrs = (XWCFFIndex**)malloc(num_fds * sizeof(XWCFFIndex*));
  	if (flag & FONTTYPE_CIDFONT)
  	{
  		for (uchar i = 0; i < num_fds; i++) 
  		{
      		if (privateDict[i] == 0 || !privateDict[i]->known("Subrs")) 
				subrs[i] = 0;
      		else 
      		{
				long offsetA = (long)(fdarray[i]->get("Private", 1));
				offsetA += (long)(privateDict[i]->get("Subrs", 0));
				seek(offsetA);
				subrs[i] = new XWCFFIndex(this, false);
				len += subrs[i]->size();
      		}
    	}
  	}
  	else
  	{
  		if (privateDict[0] == NULL || !privateDict[0]->known("Subrs")) 
      		subrs[0] = 0;
    	else 
    	{
      		long offsetA = (long)(topDict->get("Private", 1));
      		offsetA += (long)(privateDict[0]->get("Subrs", 0));
      		seek(offsetA);
      		subrs[0] = new XWCFFIndex(this, false);
      		len += subrs[0]->size();
    	}
  	}
  	
  	return len;
}

void XWFontFileCFF::setCharsets(XWCFFCharsets * cs)
{
	if (charsets)
		delete charsets;
		
	charsets = cs;
}

void XWFontFileCFF::setCString(XWCFFIndex * idx)
{
	if (cstrings)
		delete cstrings;
		
	cstrings = idx;
}

void XWFontFileCFF::setEncoding(XWCFFEncoding * enc)
{
	if (encoding)
		delete encoding;
		
	encoding = enc;
}

void XWFontFileCFF::setFDSelect(XWCFFFDSelect * fd)
{
	if (fdselect)
		delete fdselect;
		
	fdselect = fd;
}

void XWFontFileCFF::setGsub(XWCFFIndex * g)
{
	if (gsubr)
		delete gsubr;
		
	gsubr = g;
}

long XWFontFileCFF::setName(char *nameA)
{
	if (strlen(nameA) > 127)
	{
    	xwApp->error(tr("FontName string length too large...\n"));
    	return -1;
    }
    
    return name->setName(nameA);
}

void XWFontFileCFF::setSubrs(int i, XWCFFIndex * idx)
{
	if (i < 0 || i >= num_fds)
		return ;
		
	if (subrs[i])
		delete subrs[i];
		
	subrs[i] = idx;
	if (idx == 0 && num_fds <= 1)
	{
		free(subrs);
		subrs = 0;
	}
}

long XWFontFileCFF::stringSize()
{
	if (!string)
		return 0;
		
	return string->size();
}

void XWFontFileCFF::topDictAdd(const char *key, int countA)
{
	if (!topDict)
		return;
		
	topDict->add(key, countA);
}

double XWFontFileCFF::topDictGet(const char *key, int idx)
{
	if (!topDict)
		return 0;
		
	return topDict->get(key, idx);
}

bool XWFontFileCFF::topDictKnow(const char *key)
{
	if (!topDict)
		return false;
		
	return topDict->known(key) != 0;
}

long XWFontFileCFF::topDictPack(uchar *dest, long destlen)
{
	if (!topDict)
		return 0;
		
	return topDict->pack(dest, destlen);
}

void XWFontFileCFF::topDictRemove(const char *key)
{
	if (!topDict)
		return;
		
	topDict->remove(key);
}

void XWFontFileCFF::topDictSet(const char *key, int idx, double value)
{
	if (!topDict)
		return;
		
	topDict->set(key, idx, value);
}

void XWFontFileCFF::topDictUpdate()
{
	if (!topDict)
		return;
		
	topDict->update(this);
}

void XWFontFileCFF::updateString()
{
	if (string)
		delete string;
		
	string = _string;
	_string = 0;
}

int XWFontFileCFF::parseCharStrings(XWFontFileType1 * filet1,
	                                XWFontFileCFF * font, 
	                                XWPFBParser * parser, 
	                                int lenIV, 
	                                int mode)
{
	int typeA = parser->getToken();
	long countA = parser->getIV();
	if (typeA != PFB_TYPE_INTEGER || countA < 0 || countA > CFF_GLYPH_MAX)
		return -1;
		
	XWCFFIndex * charstringsA = 0;
	long max_size = 0;
	if (mode != 1) 
	{
    	charstringsA = new XWCFFIndex((ushort)countA);
    	max_size    = CS_STR_LEN_MAX;
    	charstringsA->data = (uchar*)malloc(max_size * sizeof(uchar));
  	}
  	
  	font->cstrings = charstringsA;

  	XWCFFCharsets * charsetA = font->charsets = new XWCFFCharsets;
  	charsetA->format = 0;
  	charsetA->num_entries = countA-1;
  	charsetA->data.glyphs = (ushort*)malloc((countA-1) * sizeof(ushort));
  	memset(charsetA->data.glyphs, 0, sizeof(ushort)*(countA-1));
  	if (font->ft_to_gid)
  		delete [] font->ft_to_gid;
  	font->ft_to_gid = new ushort[countA];
  	long offsetA      = 0;
  	long have_notdef = 0;
  	parser->seekOperator("begin");
  	long  gid;
  	for (long i = 0; i < countA; i++)
  	{
  		typeA = parser->getToken();
  		char * glyph_name = (char *)(parser->getSV());
  		if (typeA == PFB_TYPE_NAME)
  		{
  			if (!glyph_name)
  				return -1;
  			else if (!strcmp(glyph_name, ".notdef"))
  			{
  				gid = 0;
        		have_notdef = 1;
  			}
  			else if (have_notdef)
  				gid = i;
  			else if (i == countA - 1)
  				return -1;
  			else
  				gid = i+1;
  		}	
  		else if (typeA == PFB_TYPE_UNKNOWN && !strcmp(glyph_name, "end"))
  			break;
  		else
  			return -1;
  			
  		font->ft_to_gid[i] = gid;
  			
  		if (gid > 0)
      		charsetA->data.glyphs[gid-1] = font->addString(glyph_name, 0);
      		
      	typeA = parser->getToken();
      	if (typeA != PFB_TYPE_INTEGER || 
      		parser->getIV() < 0 || 
      		parser->getIV() > CS_STR_LEN_MAX)
      		return -1;
      	long lenA = parser->getIV();
      	
      	typeA = parser->getToken();
      	if (!parser->matchOp("RD") && 
      		!parser->matchOp("-|") &&
			!parser->seekOperator("readstring"))
		{
			return -1;
		}
		
		char * endA = 0;
		const char * startA = parser->tellPos(&endA);
		if (startA + lenA + 1 >= endA) 
      		return -1;
    	
    	if (mode != 1)
    	{
    		if (offsetA + lenA >= max_size)
    		{
    			max_size += qMax((long)lenA, (long)CS_STR_LEN_MAX);
				charstringsA->data = (uchar*)realloc(charstringsA->data, max_size * sizeof(uchar));
    		}
    		
    		if (gid == 0)
    		{
    			if (lenIV >= 0)
    			{
    				memmove(charstringsA->data + lenA - lenIV, charstringsA->data, offsetA);
	  				for (long j = 1; j <= i; j++) 
	    				charstringsA->offset[j] += lenA - lenIV;
    			}
    			else
    			{
    				memmove(charstringsA->data + lenA, charstringsA->data, offsetA);
	  				for (long j = 1; j <= i; j++) 
	    				charstringsA->offset[j] += lenA;
    			}
    		}
    	}
    	
    	startA = parser->skip(1);
    	if (mode != 1)
    	{
    		if (lenIV >= 0)
    		{
    			long offs = gid ? offsetA : 0;
				charstringsA->offset[gid] = offs + 1;
				filet1->decrypt(T1_CHARKEY, charstringsA->data+offs, (const uchar*)startA, lenIV, lenA);
				offsetA += lenA - lenIV;
    		}
    		else
    		{
    			if (gid == 0) 
    			{
	  				charstringsA->offset[gid] = 1;
	  				memcpy(&charstringsA->data[0], startA, lenA);
				} 
				else 
				{
	  				charstringsA->offset[gid] = offsetA + 1;
	  				memcpy(&charstringsA->data[offsetA], startA, lenA);
				}
				offsetA += lenA;
    		}
    	}
    	
    	parser->skip(lenA);
    	typeA = parser->getToken();
    	if ((!parser->matchOp("ND") && !parser->matchOp("|-")))
    		return -1;
  	}
  	
  	if (mode != 1)
    	charstringsA->offset[countA] = offsetA + 1;
  	font->nGlyphs = countA;

  	return 0;
}

int XWFontFileCFF::parsePart1(XWFontFileCFF * font,
	                          uchar * startA,
	                          uchar * endA,
	                          char **enc_vec)
{
	XWPFBParser parser((const char*)startA, (const char*)endA);
	if (!parser.seekOperator("begin"))
		return -1;
		
	double argv[127];
  	int    argn = 0;
  	const char * key = 0;
  	while ((key = parser.getNextKey()) != 0)
  	{
  		if (!strcmp(key, "Encoding"))
  		{
  			if (parser.getEncoding(enc_vec) < 0)
  				return -1;
  		}
  		else if (!strcmp(key, "FontName"))
  		{
  			const char * strval = parser.getString();
  			if (!strval)
  				return -1;
  				
  			font->setName((char*)strval);
  		}
  		else if (!strcmp(key, "FontType"))
  		{
  			argn = parser.getValues(argv, 1);
			if (argn != 1 || argv[0] != 1.0)
				return -1;
  		}
  		else if (!strcmp(key, "ItalicAngle") || 
  			     !strcmp(key, "StrokeWidth") ||
	             !strcmp(key, "PaintType"))
  		{
  			char * tmpkey = qstrdup(key);
  			argn = parser.getValues(argv, 1);
  			if (argn != 1)
  			{
  				delete [] tmpkey;
				return -1;
			}
				
			if (argv[0] != 0.0)
			{
				font->topDictAdd(tmpkey, 1);
				font->topDictSet(tmpkey, 0, argv[0]);
			}
			
			delete [] tmpkey;
  		}
  		else if (!strcmp(key, "UnderLinePosition") ||
	       		 !strcmp(key, "UnderLineThickness"))
		{
			char * tmpkey = qstrdup(key);
			argn = parser.getValues(argv, 1);
  			if (argn != 1)
  			{
  				delete [] tmpkey;
				return -1;
			}
			font->topDictAdd(tmpkey, 1);
      		font->topDictSet(tmpkey, 0, argv[0]);
      		delete [] tmpkey;
		}
		else if (!strcmp(key, "FontBBox"))
		{
			char * tmpkey = qstrdup(key);
			argn = parser.getValues(argv, 4);
  			if (argn != 4)
  			{
  				delete [] tmpkey;
				return -1;
			}
				
			font->topDictAdd(tmpkey, 4);
      		while (argn-- > 0) 
				font->topDictSet(tmpkey, argn, argv[argn]);
			delete [] tmpkey;
		}
		else if (!strcmp(key, "FontMatrix"))
		{
			char * tmpkey = qstrdup(key);
			argn = parser.getValues(argv, 6);
  			if (argn != 6)
  			{
  				delete [] tmpkey;
				return -1;
			}
				
			if (argv[0] != 0.001 || 
				argv[1] != 0.0 || 
				argv[2] != 0.0 ||
	  			argv[3] != 0.001 || 
	  			argv[4] != 0.0 || 
	  			argv[5] != 0.0)
	  		{
	  			font->topDictAdd(tmpkey, 6);
				while (argn-- > 0) 
	  				font->topDictSet(tmpkey, argn, argv[argn]);
	  		}
	  		
	  		delete [] tmpkey;
		}
		else if (!strcmp(key, "version")  || 
			     !strcmp(key, "Notice") ||
	             !strcmp(key, "FullName") || 
	             !strcmp(key, "FamilyName") ||
	       		 !strcmp(key, "Weight")   || 
	       		 !strcmp(key, "Copyright"))
	    {
	    	char * tmpkey = qstrdup(key);
	    	const char * strval = parser.getString();
	    	ushort sid = 0;

			font->topDictAdd(tmpkey, 1);
			if ((sid = font->getSID((char*)strval)) == CFF_STRING_NOTDEF)
	  			sid = font->addString(strval, 0);
			font->topDictSet(tmpkey, 0, sid);
			delete [] tmpkey;
	    }
	    else if (!strcmp(key, "IsFixedPitch"))
	    {
	    	char * tmpkey = qstrdup(key);
	    	bool tmpbool = parser.getBoolean();
	    	if (tmpbool)
	    	{
	    		font->privDictAdd(0, tmpkey, 1);
				font->privDictSet(0, tmpkey, 0, 1);
	    	}
	    	
	    	delete [] tmpkey;
	    }
  	}
  	
  	return 0;
}

int XWFontFileCFF::parsePart2(XWFontFileType1 * filet1,
	                          XWFontFileCFF * font,
	                          uchar * startA,
	                      	  uchar * endA,
	                          int mode)
{
	XWPFBParser parser((const char*)startA, (const char*)endA);
	const char * key = 0;
	int lenIV = 4;
	int    argn = 0;
	double argv[127];
	while ((key = parser.getNextKey()) != 0)
	{
		if (!strcmp(key, "Subrs"))
		{
			if (parseSubrs(filet1, font, &parser, lenIV, mode) < 0) 
				return -1;
		}
		else if (!strcmp(key, "CharStrings"))
		{
			if (parseCharStrings(filet1, font, &parser, lenIV, mode) < 0) 
				return -1;
		}
		else if (!strcmp(key, "lenIV"))
		{
			argn = parser.getValues(argv, 1);
			if (argn != 1)
				return -1;
			lenIV = (int)argv[0];
		}
		else if (!strcmp(key, "BlueValues") ||
	       		 !strcmp(key, "OtherBlues") ||
	       		 !strcmp(key, "FamilyBlues") ||
	       		 !strcmp(key, "FamilyOtherBlues") ||
	       		 !strcmp(key, "StemSnapH") ||
	       		 !strcmp(key, "StemSnapV"))
	    {
	    	char * tmpkey = qstrdup(key);
	    	argn = parser.getValues(argv, 127);
	    	if (argn < 0)
	    	{
	    		delete [] tmpkey;
	    		return -1;
	    	}
	    		
	    	font->privDictAdd(0, tmpkey, argn);
	    	while (argn-- > 0)
	    	{
	    		font->privDictSet(0, tmpkey, argn, (argn == 0) ? argv[argn] : argv[argn] - argv[argn-1]);
	    	}
	    	delete [] tmpkey;
	    }
	    else if (!strcmp(key, "StdHW") ||
	       		 !strcmp(key, "StdVW") ||
	       		 !strcmp(key, "BlueScale") ||
	       		 !strcmp(key, "BlueShift") ||
	       		 !strcmp(key, "BlueFuzz")  ||
	       		 !strcmp(key, "LanguageGroup") ||
	       		 !strcmp(key, "ExpansionFactor"))
	    {
	    	char * tmpkey = qstrdup(key);
	    	argn = parser.getValues(argv, 1);
			if (argn != 1)
			{
				delete [] tmpkey;
				return -1;
			}
			font->privDictAdd(0, tmpkey, 1);
			font->privDictSet(0, tmpkey, 0, argv[0]);
			delete [] tmpkey;
	    }
	    else if (!strcmp(key, "ForceBold"))
	    {
	    	char * tmpkey = qstrdup(key);
	    	argn = parser.getValues(argv, 1);
			if (argn != 1)
			{
				delete [] tmpkey;
				return -1;
			}
			if (argv[0] != 0)
			{
				font->privDictAdd(0, tmpkey, 1);
				font->privDictSet(0, tmpkey, 0, 1);
			}
			delete [] tmpkey;
	    }
	}
	
	return 0;
}

int XWFontFileCFF::parseSubrs(XWFontFileType1 * filet1,
	                          XWFontFileCFF * font, 
	                          XWPFBParser * parser, 
	                          int lenIV, 
	                          int mode)
{
	int typeA = parser->getToken();
	long countA = parser->getIV();
	if (typeA != PFB_TYPE_INTEGER || countA < 0)
		return -1;
		
	if (countA == 0)
	{
		font->setSubrs(0, 0);
		return 0;
	}
		
	if (!parser->getOp("array"))
		return -1;
		
	long max_size = 0;
    uchar * dataA = 0;
    long * offsetsA = 0;
    long * lengthsA = 0;
    if (mode != 1)
    {
    	max_size = CS_STR_LEN_MAX;
    	dataA     = (uchar*)malloc(max_size * sizeof(uchar));
    	offsetsA  = (long*)malloc(countA * sizeof(long));
    	lengthsA  = (long*)malloc(countA * sizeof(long));
    	memset(offsetsA, 0, sizeof(long)*countA);
    	memset(lengthsA, 0, sizeof(long)*countA);
    }
    
    long offsetA = 0;
    for (long i = 0; i < countA;)
    {
    	typeA = parser->getToken();    	
    	if (parser->matchOp("ND") || 
    		parser->matchOp("|-") || 
    		parser->matchOp("def"))
    		break;
    	else if (!parser->matchOp("dup"))
    		continue;
    		
    	typeA = parser->getToken();
    	if (typeA != PFB_TYPE_INTEGER || 
    		parser->getIV() < 0 || 
    		parser->getIV() >= countA)
    	{
    		if (dataA)
    			free(dataA);
    			
    		if (offsetsA)
    			free(offsetsA);
    			
    		if (lengthsA)
    			free(lengthsA);
    			
    		return -1;
    	}
    	long idx = parser->getIV();
    	
    	typeA = parser->getToken();
    	if (typeA != PFB_TYPE_INTEGER || 
    		parser->getIV() < 0 || 
    		parser->getIV() > CS_STR_LEN_MAX)
    	{
    		if (dataA)
    			free(dataA);
    			
    		if (offsetsA)
    			free(offsetsA);
    			
    		if (lengthsA)
    			free(lengthsA);
    			
    		return -1;
    	}
    	long lenA = parser->getIV();
    	
    	typeA = parser->getToken();
    	if (!parser->matchOp("RD") && 
    		!parser->matchOp("-|") && 
			!parser->seekOperator("readstring"))
		{
			if (dataA)
    			free(dataA);
    			
    		if (offsetsA)
    			free(offsetsA);
    			
    		if (lengthsA)
    			free(lengthsA);
    			
    		return -1;
		}
		
		char * endA = 0;
		const char * startA = parser->tellPos(&endA);
		startA = parser->skip(1);
    	if (startA + lenA >= endA) 
    	{
      		if (dataA)
    			free(dataA);
    			
    		if (offsetsA)
    			free(offsetsA);
    			
    		if (lengthsA)
    			free(lengthsA);
    			
    		return -1;
    	}
    	
    	if (mode != 1)
    	{
    		if (offsetA + lenA >= max_size)
    		{
    			max_size += CS_STR_LEN_MAX;
				dataA = (uchar*)realloc(dataA, max_size * sizeof(uchar));
    		}
    		
    		if (lenIV >= 0)
    		{
    			filet1->decrypt(T1_CHARKEY, dataA+offsetA, (const uchar*)startA, lenIV, lenA);
    			offsetsA[idx] = offsetA;
				offsetA += (lengthsA[idx] = lenA - lenIV);
    		}
    		else if (lenA > 0)
    		{
    			offsetsA[idx] = offsetA;
				lengthsA[idx] = lenA;
				memcpy(&dataA[offsetA], startA, lenA);
				offsetA += lenA;
    		}
    	}
    	
    	startA = parser->skip(lenA);
    	i++;
    }
    
    if (mode != 1)
    {
    	if (font->subrs[0] == 0)
    	{
    		XWCFFIndex * subrsA = font->subrs[0] = new XWCFFIndex(countA);
    		subrsA->data = (uchar*)malloc(offsetA * sizeof(uchar));
    		offsetA = 0;
      		for (long i = 0; i < countA; i++) 
      		{
        		subrsA->offset[i] = offsetA + 1;
        		if (lengthsA[i] > 0) 
        		{
	  				memcpy(subrsA->data + offsetA, dataA + offsetsA[i], lengthsA[i]);
	  				offsetA += lengthsA[i];
        		}
      		}
      		subrsA->offset[countA] = offsetA + 1;
    	}
    	
    	if (dataA)
    		free(dataA);
    			
    	if (offsetsA)
    		free(offsetsA);
    			
    	if (lengthsA)
    		free(lengthsA);
    }
    
    return 0;
}

