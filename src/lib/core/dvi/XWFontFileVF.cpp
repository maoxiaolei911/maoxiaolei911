/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <QString>
#include "XWConst.h"
#include "XWUtil.h"
#include "XWApplication.h"
#include "XWNumberUtil.h"
#include "XWStringUtil.h"
#include "XWFontSea.h"
#include "XWTFMFile.h"
#include "XWDVICore.h"
#include "XWFontFileVF.h"

#define VF_ALLOC_SIZE  16u

#define VF_ID 202
#define FIX_WORD_BASE 1048576.0
#define TEXPT2PT (72.0/72.27)
#define FW2PT (TEXPT2PT/((double)(FIX_WORD_BASE)))

static XWFontFileVF ** vf_fonts = 0;
static int num_vf_fonts = 0;
static int max_vf_fonts = 0;


static void resize_vf_fonts(int size)
{
  	if (size > max_vf_fonts) 
  	{
    	vf_fonts = (XWFontFileVF**)realloc(vf_fonts, size * sizeof(XWFontFileVF*));
    	for (int i = max_vf_fonts; i < size; i++) 
      		vf_fonts[i] = 0;
    	max_vf_fonts = size;
  	}
}

XWFontFileVF::XWFontFileVF()
{
	tex_name = 0;
	ptsize = 0;
	design_size = 0;
	num_dev_fonts = 0;
	max_dev_fonts = 0;
	dev_fonts = 0;
	ch_pkt = 0;
	pkt_len = 0;
	num_chars = 0;
}

XWFontFileVF::~XWFontFileVF()
{
	if (tex_name)
		delete [] tex_name;
		
	for (uint i = 0; i < num_chars; i++)
	{
		if (ch_pkt[i])
			free(ch_pkt[i]);
	}
	
	if (ch_pkt)
		free(ch_pkt);
		
	if (pkt_len)
		free(pkt_len);
		
	for (int i = 0; i < num_dev_fonts; i++)
	{
		FontDef * one_font = &(dev_fonts[i]);
		if (one_font->directory)
			delete [] one_font->directory;
			
		if (one_font->name)
			delete [] one_font->name;
	}
	
	if (dev_fonts)
		free(dev_fonts);
}

void XWFontFileVF::close()
{
	for (int i = 0; i < num_vf_fonts; i++)
	{
		if (vf_fonts[i])
			delete vf_fonts[i];
	}
	
	if (vf_fonts)
		free(vf_fonts);
	vf_fonts = 0;
	num_vf_fonts = 0;
	max_vf_fonts = 0;
}

int XWFontFileVF::locateFont(XWDVICore * coreA, 
	                         const char *tex_name, 
	                         long ptsizeA)
{
	int i = 0;
	for (; i < num_vf_fonts; i++) 
	{
		XWFontFileVF * font = vf_fonts[i];
    	if (!strcmp(font->tex_name, tex_name) && font->ptsize == ptsizeA) 
      		break;
  	}
  	
  	int thisfont = -1;
  	if (i != num_vf_fonts) 
  	{
    	thisfont = i;
    	XWFontFileVF * font = vf_fonts[i];
    	font->changeCore(coreA, ptsizeA);
    }
    else
    {
    	QString fn(tex_name);
    	XWFontSea sea;
    	QFile * vf_file = sea.openVf(fn);
    	if (!vf_file)
    		vf_file = sea.openOvf(fn);
    		
    	if (!vf_file)
    		return -1;
    		
    	XWFontFileVF * font = new XWFontFileVF;
    	font->tex_name = qstrdup(tex_name);
		font->ptsize = ptsizeA;
    	if (font->readHeader(vf_file))
    	{
    		font->processFile(coreA, vf_file);
    		if (num_vf_fonts >= max_vf_fonts) 
				resize_vf_fonts(max_vf_fonts + VF_ALLOC_SIZE);
					
			thisfont = num_vf_fonts++;
			vf_fonts[thisfont] = font;
    	}
    	else
    		delete font;
    	
    	vf_file->close();
    	delete vf_file;
    }
    
    return thisfont;
}

void XWFontFileVF::setChar(XWDVICore * coreA, long ch, int vf_font)
{
	if (vf_font < 0 || vf_font >= num_vf_fonts)
		return ;
		
	XWFontFileVF * font = vf_fonts[vf_font];
	font->setChar(coreA, ch);
}

void XWFontFileVF::changeCore(XWDVICore * coreA, long ptsizeA)
{
	if (coreA == core && ptsizeA == ptsize)
		return ;
		
	core = coreA;
	ptsize = ptsizeA;
	for (int i = 0; i < num_dev_fonts; i++)
	{
		FontDef* dev_font = dev_fonts + i;
		dev_font->dev_id = coreA->locateFont(dev_font->name, scaleFixWord(ptsize, dev_font->size), true);
	}
}

void XWFontFileVF::doDir(XWDVICore * coreA, uchar **start, uchar *end)
{
	coreA->doDir(unsignedByte(start, end));
}

void XWFontFileVF::doDown(XWDVICore * coreA, long y, long ptsizeA)
{
	coreA->doDown((long) (scaleFixWord(ptsizeA, y)));
}

void XWFontFileVF::doDown1(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA)
{
	doDown(coreA, signedByte(start, end), ptsizeA);
}

void XWFontFileVF::doDown2(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA)
{
	doDown(coreA, signedPair(start, end), ptsizeA);
}

void XWFontFileVF::doDown3(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA)
{
	doDown(coreA, signedTriple(start, end), ptsizeA);
}

void XWFontFileVF::doDown4(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA)
{
	doDown(coreA, signedQuad(start, end), ptsizeA);
}

void XWFontFileVF::doFnt(XWDVICore * coreA, long font_id)
{
	int i = 0;
	for (; i < num_dev_fonts; i++)
	{
		if (font_id == dev_fonts[i].font_id)
      		break;
	}
	
	if (i < num_dev_fonts)
    	coreA->doSetFont(dev_fonts[i].dev_id);
}

void XWFontFileVF::doFnt1(XWDVICore * coreA, uchar **start, uchar *end)
{
	doFnt(coreA, unsignedByte(start, end));
}

void XWFontFileVF::doFnt2(XWDVICore * coreA, uchar **start, uchar *end)
{
	doFnt(coreA, unsignedPair(start, end));
}

void XWFontFileVF::doFnt3(XWDVICore * coreA, uchar **start, uchar *end)
{
	doFnt(coreA, unsignedTriple(start, end));
}

void XWFontFileVF::doFnt4(XWDVICore * coreA, uchar **start, uchar *end)
{
	doFnt(coreA, signedQuad(start, end));
}

void XWFontFileVF::doRight(XWDVICore * coreA, long x, long ptsizeA)
{
	coreA->doRight((long) (scaleFixWord(ptsizeA, x)));
}

void XWFontFileVF::doRight1(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA)
{
	doRight(coreA, signedByte(start, end), ptsizeA);
}

void XWFontFileVF::doRight2(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA)
{
	doRight(coreA, signedPair(start, end), ptsizeA);
}

void XWFontFileVF::doRight3(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA)
{
	doRight(coreA, signedTriple(start, end), ptsizeA);
}

void XWFontFileVF::doRight4(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA)
{
	doRight(coreA, signedQuad(start, end), ptsizeA);
}

void XWFontFileVF::doW(XWDVICore * coreA, long w, long ptsizeA)
{
	coreA->doW((long) (scaleFixWord(ptsizeA, w)));
}

void XWFontFileVF::doW0(XWDVICore * coreA)
{
	coreA->doW0();
}

void XWFontFileVF::doW1(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA)
{
	doW(coreA, signedByte(start, end), ptsizeA);
}

void XWFontFileVF::doW2(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA)
{
	doW(coreA, signedPair(start, end), ptsizeA);
}

void XWFontFileVF::doW3(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA)
{
	doW(coreA, signedTriple(start, end), ptsizeA);
}

void XWFontFileVF::doW4(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA)
{
	doW(coreA, signedQuad(start, end), ptsizeA);
}

void XWFontFileVF::doX(XWDVICore * coreA, long x, long ptsizeA)
{
	coreA->doX((long) (scaleFixWord(ptsizeA, x)));
}

void XWFontFileVF::doX0(XWDVICore * coreA)
{
	coreA->doX0();
}

void XWFontFileVF::doX1(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA)
{
	doX(coreA, signedByte(start, end), ptsizeA);
}

void XWFontFileVF::doX2(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA)
{
	doX(coreA, signedPair(start, end), ptsizeA);
}

void XWFontFileVF::doX3(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA)
{
	doX(coreA, signedTriple(start, end), ptsizeA);
}

void XWFontFileVF::doX4(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA)
{
	doX(coreA, signedQuad(start, end), ptsizeA);
}

void XWFontFileVF::doXXX(XWDVICore * coreA, long len, uchar **start, uchar *end)
{
	if (*start <= end - len) 
	{
    	uchar * buffer = new uchar[len+1];
    	memcpy(buffer, *start, len);
    	buffer[len] = '\0';
    	{
      		uchar *p = buffer;

      		while (p < buffer+len && *p == ' ') 
      			p++;
      		if (!memcmp((char *)p, "Warning:", 8)) 
      		{
      			QString msg = QString::fromLocal8Bit((const char*)p, end - p);
      			xwApp->warning(msg);
      		} 
      		else 
				coreA->doSpecial((const char*)buffer, len);
    	}
    	delete [] buffer;
  	} 
  	else
  		return ;

  	*start += len;
}

void XWFontFileVF::doXXX1(XWDVICore * coreA, uchar **start, uchar *end)
{
	doXXX(coreA, unsignedByte(start, end), start, end);
}

void XWFontFileVF::doXXX2(XWDVICore * coreA, uchar **start, uchar *end)
{
	doXXX(coreA, unsignedPair(start, end), start, end);
}

void XWFontFileVF::doXXX3(XWDVICore * coreA, uchar **start, uchar *end)
{
	doXXX(coreA, unsignedTriple(start, end), start, end);
}

void XWFontFileVF::doXXX4(XWDVICore * coreA, uchar **start, uchar *end)
{
	doXXX(coreA, unsignedQuad(start, end), start, end);
}

void XWFontFileVF::doY(XWDVICore * coreA, long y, long ptsizeA)
{
	coreA->doY((long) (scaleFixWord(ptsizeA, y)));
}

void XWFontFileVF::doY0(XWDVICore * coreA)
{
	coreA->doY0();
}

void XWFontFileVF::doY1(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA)
{
	doY(coreA, signedByte(start, end), ptsizeA);
}

void XWFontFileVF::doY2(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA)
{
	doY(coreA, signedPair(start, end), ptsizeA);
}

void XWFontFileVF::doY3(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA)
{
	doY(coreA, signedTriple(start, end), ptsizeA);
}

void XWFontFileVF::doY4(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA)
{
	doY(coreA, signedQuad(start, end), ptsizeA);
}

void XWFontFileVF::doZ(XWDVICore * coreA, long z, long ptsizeA)
{
	coreA->doZ((long) (scaleFixWord(ptsizeA, z)));
}

void XWFontFileVF::doZ0(XWDVICore * coreA)
{
	coreA->doZ0();
}

void XWFontFileVF::doZ1(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA)
{
	doZ(coreA, signedByte(start, end), ptsizeA);
}

void XWFontFileVF::doZ2(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA)
{
	doZ(coreA, signedPair(start, end), ptsizeA);
}

void XWFontFileVF::doZ3(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA)
{
	doZ(coreA, signedTriple(start, end), ptsizeA);
}

void XWFontFileVF::doZ4(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA)
{
	doZ(coreA, signedQuad(start, end), ptsizeA);
}

void XWFontFileVF::pop(XWDVICore * coreA)
{
	coreA->doPop();
}

void XWFontFileVF::processFile(XWDVICore * coreA, QIODevice *vf_file)
{
	core = coreA;
	int eof = 0;
  	unsigned long font_id = -1;
  	while (!eof)
  	{
  		long code = getUnsignedByte(vf_file);
  		switch (code)
  		{
  			case DVI_FNTDEF1:
      			font_id = getUnsignedByte(vf_file);
      			readFontDef(coreA, vf_file, font_id);
      			break;
      			
      		case DVI_FNTDEF2:
      			font_id = getUnsignedPair(vf_file);
      			readFontDef(coreA, vf_file, font_id);
      			break;
      			
    		case DVI_FNTDEF3:
      			font_id = getUnsignedTriple(vf_file);
      			readFontDef(coreA, vf_file, font_id);
      			break;
      			
    		case DVI_FNTDEF4:
      			font_id = getSignedQuad(vf_file);
      			readFontDef(coreA, vf_file, font_id);
      			break;
      			
      		default:
      			if (code < 242) 
      			{
					long ch = getUnsignedByte(vf_file);
					getUnsignedTriple(vf_file);
					readCharDef(vf_file, code, ch);
					break;
      			}
      			
      			if (code == 242)
      			{
					ulong pkt_lenA = getUnsignedQuad(vf_file);
					ulong ch = getUnsignedQuad(vf_file);
					getUnsignedQuad(vf_file);
					if (ch < 65536L) 
	  					readCharDef(vf_file, pkt_lenA, ch);
					else 
					{
	  					fprintf (stderr, "char=%ld\n", ch);
	  					xwApp->error(tr("long character (>16 bits) in VF file.\nI can't handle long characters!\n"));
	  					return ;
					}
					break;
      			}
      			
      			if (code == DVI_POST) 
      			{
					eof = 1;
					break;
      			}
      			eof = 1;
      			break;
  		}
  	}
}

void XWFontFileVF::push(XWDVICore * coreA)
{
	coreA->doPush();
}

void XWFontFileVF::put1(XWDVICore * coreA, uchar **start, uchar *end)
{
	coreA->doPut(unsignedByte(start, end));
}

void XWFontFileVF::put2(XWDVICore * coreA, uchar **start, uchar *end)
{
	coreA->doPut(unsignedPair(start, end));
}

void XWFontFileVF::put3(XWDVICore * coreA, uchar **start, uchar *end)
{
	coreA->doPut(unsignedTriple(start, end));
}

void XWFontFileVF::put4(XWDVICore * coreA, uchar **start, uchar *end)
{
	coreA->doPut(unsignedQuad(start, end));
}

void XWFontFileVF::putRule(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA)
{
	long height = signedQuad(start, end);
  	long width  = signedQuad(start, end);
  	if (width > 0 && height > 0) 
    	coreA->doRule(scaleFixWord(ptsizeA,width), scaleFixWord(ptsizeA, height));
}

void XWFontFileVF::readCharDef(QIODevice *vf_file, 
	                           ulong pkt_lenA,
			                   unsigned ch)
{
	if (ch >= num_chars) 
    	resize(ch+1);
    	
    if (pkt_lenA > 0) 
    {
    	uchar * pkt = (uchar*)malloc(pkt_lenA * sizeof(uchar));
    	if (vf_file->read((char*)pkt, pkt_lenA) != pkt_lenA)
    	{
    		free(pkt);
      		xwApp->error(tr("VF file ended prematurely.\n"));
      		return ;
      	}
    	ch_pkt[ch] = pkt;
  	}
  	
  	pkt_len[ch] = pkt_lenA;
}

void XWFontFileVF::readFontDef(XWDVICore * coreA, QIODevice *vf_file, long font_id)
{
	if (num_dev_fonts >= max_dev_fonts) 
	{
    	max_dev_fonts += VF_ALLOC_SIZE;
    	dev_fonts = (FontDef*)realloc(dev_fonts, max_dev_fonts * sizeof(FontDef));
  	}
  	
  	FontDef* dev_font = dev_fonts+ num_dev_fonts;
  	dev_font->font_id = font_id;
  	dev_font->checksum = getUnsignedQuad(vf_file);
  	dev_font->size = getUnsignedQuad(vf_file);
  	dev_font->design_size = getUnsignedQuad(vf_file);
  	int dir_length = getUnsignedByte(vf_file);
  	int name_length = getUnsignedByte(vf_file);
  	dev_font->directory = new char[dir_length+1];
  	dev_font->name = new char[name_length+1];
  	if (dir_length > 0)
  		vf_file->read(dev_font->directory, dir_length);
  		
  	if (name_length > 0)
  		vf_file->read(dev_font->name, name_length);
  		
  	(dev_font->directory)[dir_length] = 0;
  	(dev_font->name)[name_length] = 0;
  	num_dev_fonts += 1;
  	XWTFMFile tfm(dev_font->name);
  	dev_font->tfm_id = tfm.getID(); /* must exist */
  	dev_font->dev_id = coreA->locateFont(dev_font->name, scaleFixWord(ptsize, dev_font->size));
}

int XWFontFileVF::readHeader(QIODevice *vf_file)
{
	int result = 1;
	int ch = 0;
	if ((ch = getUnsignedByte(vf_file)) == DVI_PRE &&
        (ch = getUnsignedByte(vf_file)) == VF_ID)
    {
    	ch = getUnsignedByte(vf_file);
    	for (int i = 0; i < ch; i++)
      		getUnsignedByte(vf_file);
      		
      	getUnsignedQuad(vf_file);
      	design_size = getUnsignedQuad(vf_file);
    }
    else
    	result = 0;
    	
    return result;
}

void XWFontFileVF::resize(unsigned size)
{
	if (size > num_chars)
	{
    	size = qMax(size, num_chars+256);
    	ch_pkt = (uchar**)realloc(ch_pkt, size * sizeof(uchar*));
    	pkt_len = (ulong*)realloc(pkt_len, size * sizeof(ulong));
    	
    	for (uint i = num_chars; i < size; i++) 
    	{
      		ch_pkt[i] = 0;
      		pkt_len[i] = 0;
    	}
    	num_chars = size;
  	}
}

void XWFontFileVF::setChar(XWDVICore * coreA, long ch)
{
	int default_font = -1;
    if (num_dev_fonts > 0)
      	default_font = dev_fonts[0].dev_id;
    coreA->initVF(default_font);
    
  	uchar *start = 0;
  	uchar *end = 0;
  	if ((uint)ch < num_chars && (start = ch_pkt[ch]))
  		end = start + pkt_len[ch];
  	
  	while (start && start < end)
  	{
  		uchar opcode = *(start++);
  		switch (opcode)
  		{
  			case DVI_SET1:
	  			set1(coreA, &start, end);
	  			break;
	  			
			case DVI_SET2:
	  			set2(coreA, &start, end);
	  			break;
	  			
			case DVI_SET3:
				set3(coreA, &start, end);
	  			break;
	  			
			case DVI_SET4:
				set4(coreA, &start, end);
	  			break;
	  			
	  		case DVI_SETRULE:
	  			setRule(coreA, &start, end, ptsize);
	  			break;
	  			
			case DVI_PUT1:
	  			put1(coreA, &start, end);
	  			break;
	  			
			case DVI_PUT2:
	  			put2(coreA, &start, end);
	  			break;
	  			
			case DVI_PUT3:
				put3(coreA, &start, end);
	  			break;
	  			
			case DVI_PUT4:
	  			put3(coreA, &start, end);
	  			break;
	  			
			case DVI_PUTRULE:
	  			putRule(coreA, &start, end, ptsize);
	  			break;
	  			
			case DVI_NOP:
	  			break;
	  			
			case DVI_PUSH:
	  			push(coreA);
	  			break;
	  			
			case DVI_POP:
	  			pop(coreA);
	  			break;
	  			
			case DVI_RIGHT1:
	  			doRight1(coreA, &start, end, ptsize);
	  			break;
	  			
			case DVI_RIGHT2:
	  			doRight2(coreA, &start, end, ptsize);
	  			break;
	  			
			case DVI_RIGHT3:
	  			doRight3(coreA, &start, end, ptsize);
	  			break;
	  			
			case DVI_RIGHT4:
	  			doRight4(coreA, &start, end, ptsize);
	  			break;
	  			
			case DVI_W0:
	  			doW0(coreA);
	  			break;
	  			
			case DVI_W1:
	  			doW1(coreA, &start, end, ptsize);
	  			break;
	  			
			case DVI_W2:
	  			doW2(coreA, &start, end, ptsize);
	  			break;
	  			
			case DVI_W3:
	  			doW3(coreA, &start, end, ptsize);
	  			break;
	  			
			case DVI_W4:
	  			doW4(coreA, &start, end, ptsize);
	  			break;
	  			
			case DVI_X0:
	  			doX0(coreA);
	  			break;
	  			
			case DVI_X1:
	  			doX1(coreA, &start, end, ptsize);
	  			break;
	  			
			case DVI_X2:
	  			doX2(coreA, &start, end, ptsize);
	  			break;
	  			
			case DVI_X3:
	  			doX3(coreA, &start, end, ptsize);
	  			break;
	  			
			case DVI_X4:
	  			doX4(coreA, &start, end, ptsize);
	  			break;
	  			
			case DVI_DOWN1:
	  			doDown1(coreA, &start, end, ptsize);
	  			break;
	  			
			case DVI_DOWN2:
	  			doDown2(coreA, &start, end, ptsize);
	  			break;
	  			
			case DVI_DOWN3:
	  			doDown3(coreA, &start, end, ptsize);
	  			break;
	  			
			case DVI_DOWN4:
	  			doDown4(coreA, &start, end, ptsize);
	  			break;
	  			
			case DVI_Y0:
	  			doY0(coreA);
	  			break;
	  			
			case DVI_Y1:
	  			doY1(coreA, &start, end, ptsize);
	  			break;
	  			
			case DVI_Y2:
	  			doY2(coreA, &start, end, ptsize);
	  			break;
	  			
			case DVI_Y3:
	  			doY3(coreA, &start, end, ptsize);
	  			break;
	  			
			case DVI_Y4:
	  			doY4(coreA, &start, end, ptsize);
	  			break;
	  			
			case DVI_Z0:
	  			doZ0(coreA);
	  			break;
	  			
			case DVI_Z1:
	  			doZ1(coreA, &start, end, ptsize);
	  			break;
	  			
			case DVI_Z2:
	  			doZ2(coreA, &start, end, ptsize);
	  			break;
	  			
			case DVI_Z3:
	  			doZ3(coreA, &start, end, ptsize);
	  			break;
	  			
			case DVI_Z4:
	  			doZ4(coreA, &start, end, ptsize);
	  			break;
	  			
			case DVI_FNT1:
	  			doFnt1(coreA, &start, end);
	  			break;
	  			
			case DVI_FNT2:
	  			doFnt2(coreA, &start, end);
	  			break;
	  			
			case DVI_FNT3:
	  			doFnt3(coreA, &start, end);
	  			break;
	  			
			case DVI_FNT4:
	  			doFnt4(coreA, &start, end);
	  			break;
	  			
			case DVI_XXX1:
	  			doXXX1(coreA, &start, end);
	  			break;
	  			
			case DVI_XXX2:
	  			doXXX2(coreA, &start, end);
	  			break;
	  			
			case DVI_XXX3:
	  			doXXX3(coreA, &start, end);
	  			break;
	  			
			case DVI_XXX4:
	  			doXXX4(coreA, &start, end);
	  			break;
	  			
			case DVI_DIR:
	  			doDir(coreA, &start, end);
	  			break;
	  			
	  		default:
	  			if (opcode < DVI_SET1)
	  				set(coreA, opcode);
	  			else if (opcode >= DVI_FNTNUM0 && opcode < DVI_FNT1)
	  				doFnt(coreA, opcode - DVI_FNTNUM0);
	  			break;
  		}
  	}
  	
  	coreA->finishVF();
}

void XWFontFileVF::set(XWDVICore * coreA, long ch)
{
	coreA->doSet(ch);
}

void XWFontFileVF::set1(XWDVICore * coreA, uchar **start, uchar *end)
{
	set(coreA, unsignedByte(start, end));
}

void XWFontFileVF::set2(XWDVICore * coreA, uchar **start, uchar *end)
{
	set(coreA, unsignedPair(start, end));
}

void XWFontFileVF::set3(XWDVICore * coreA, uchar **start, uchar *end)
{
	set(coreA, unsignedTriple(start, end));
}

void XWFontFileVF::set4(XWDVICore * coreA, uchar **start, uchar *end)
{
	set(coreA, unsignedQuad(start, end));
}
	
void XWFontFileVF::setRule(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA)
{
	long height = signedQuad(start, end);
  	long width  = signedQuad(start, end);
  	long s_width = scaleFixWord(ptsizeA, width);
  	if (width > 0 && height > 0) 
    	coreA->doRule(s_width, scaleFixWord(ptsizeA, height));
  	
  	coreA->doRight(s_width);
}

