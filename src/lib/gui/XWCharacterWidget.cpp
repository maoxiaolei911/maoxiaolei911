/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <QtGui>
#include "XWConst.h"
#include "XWUtil.h"
#include "XWApplication.h"
#include "XWNumberUtil.h"
#include "XWStringUtil.h"
#include "XWFontSea.h"
#include "XWSubfont.h"
#include "XWTFMFile.h"
#include "XWFontCache.h"
#include "XWFontFileID.h"
#include "XWFTFontFile.h"
#include "XWFTFontEngine.h"
#include "XWFTFont.h"
#include "XWFontFilePK.h"
#include "XWTexFontMap.h"
#include "XWDVICore.h"
#include "XWCharacterWidget.h"

class VFFont
{
public:
	VFFont(XWCharacterWidget * widgetA);
	~VFFont();
	
	void drawChar(QPainter * painter,
                  long c, 
                  long x,
                  long y, 
                  long w,
                  long h,
                  long squareSize);
                  
	void drawChar(QPainter * painter, 
	              long ch,
	              long x, 
	              long y);
	              
	int getNumGlyphs();
	
	bool load(const QString & name, long ptsizeA);
	
private:
	void clearState();
	
	void doDown(long y);
	
	void doDown1(uchar **start, uchar *end);
	void doDown2(uchar **start, uchar *end);
	void doDown3(uchar **start, uchar *end);
	void doDown4(uchar **start, uchar *end);
	
	void doFnt(long font_id);
	void doFnt1(uchar **start, uchar *end);
	void doFnt2(uchar **start, uchar *end);
	void doFnt3(uchar **start, uchar *end);
	void doFnt4(uchar **start, uchar *end);
	
	void doMoveTo(long x, long y);
	
	void doRight(long x);
	
	void doRight1(uchar **start, uchar *end);
	void doRight2(uchar **start, uchar *end);
	void doRight3(uchar **start, uchar *end);
	void doRight4(uchar **start, uchar *end);
	
	void doPut(long ch, 
	           QPainter * painter, 
	           long x, 
	           long y);
	           
	void doRule(QPainter * painter, 
	            long x, 
	            long y, 
	            long width, 
	            long height);
	void doSet(long ch, 
	           QPainter * painter,
	           long x, 
	           long y);
	void doW(long ch);
	void doW0();
	void doW1(uchar **start, uchar *end);
	void doW2(uchar **start, uchar *end);
	void doW3(uchar **start, uchar *end);
	void doW4(uchar **start, uchar *end);
	void doX(long x);
	void doX0();
	void doX1(uchar **start, uchar *end);
	void doX2(uchar **start, uchar *end);
	void doX3(uchar **start, uchar *end);
	void doX4(uchar **start, uchar *end);
	void doXXX(long len, uchar **start, uchar *end);
	void doXXX1(uchar **start, uchar *end);
	void doXXX2(uchar **start, uchar *end);
	void doXXX3(uchar **start, uchar *end);
	void doXXX4(uchar **start, uchar *end);
	void doY(long y);
	void doY0();
	void doY1(uchar **start, uchar *end);
	void doY2(uchar **start, uchar *end);
	void doY3(uchar **start, uchar *end);
	void doY4(uchar **start, uchar *end);
	void doZ(long z);
	void doZ0();
	void doZ1(uchar **start, uchar *end);
	void doZ2(uchar **start, uchar *end);
	void doZ3(uchar **start, uchar *end);
	void doZ4(uchar **start, uchar *end);
	            
	void pop();
	bool processFile(QIODevice *vf_file);
	void push();
	void putRule(uchar **start, 
	             uchar *end,
	             QPainter * painter, 
	             long x, 
	             long y);
	void put1(uchar **start, 
	          uchar *end, 
	          QPainter * painter, 
	          long x, 
	          long y);
	void put2(uchar **start, 
	          uchar *end, 
	          QPainter * painter, 
	          long x, 
	          long y);
	void put3(uchar **start, 
	          uchar *end, 
	          QPainter * painter, 
	          long x, 
	          long y);
	void put4(uchar **start, 
	          uchar *end, 
	          QPainter * painter, 
	          long x, 
	          long y);
	
	bool readCharDef(QIODevice *vf_file, 
	                 ulong pkt_lenA,
			         unsigned ch);
	void readFontDef(QIODevice *vf_file, long font_id);
	bool readHeader(QIODevice *vf_file);
	void resize(unsigned size);
	
	void setRule(uchar **start, 
	            uchar *end,
	            QPainter * painter, 
	            long x, 
	            long y);
	void set1(uchar **start, 
	          uchar *end,
	          QPainter * painter, 
	          long x, 
	          long y);
	void set2(uchar **start, 
	          uchar *end,
	          QPainter * painter, 
	          long x, 
	          long y);
	void set3(uchar **start, 
	          uchar *end,
	          QPainter * painter, 
	          long x, 
	          long y);
	void set4(uchar **start, 
	          uchar *end,
	          QPainter * painter, 
	          long x, 
	          long y);
	
private:
	XWCharacterWidget * widget;
	ulong unitNum;
    ulong unitDen;
    double dvi2pts;
    
	struct FontDef
	{
		long font_id;
		ulong checksum;
		ulong size;
		ulong design_size;
		char *directory;
		char *name;
		int tfm_id;
		int dev_id;
	};
	
	long ptsize;
	ulong design_size;
	int num_dev_fonts;
	int max_dev_fonts;
	
	FontDef * dev_fonts;
	uchar **ch_pkt;
  	ulong *pkt_len;
  	uint num_chars;
  	
  	DVIState dviState;
    DVIState dviStack[DVI_STACK_DEPTH_MAX];
    int  dviStackDepth;
  	int currentFont;
};

VFFont::VFFont(XWCharacterWidget * widgetA)
{
	widget = widgetA;
	unitNum = 25400000;
  	unitDen = 473628672;
  	dvi2pts  = (double) unitNum / (double) unitDen;
  	dvi2pts  *= (72.0 / 254000.0);
  	
	ptsize = 0;
	design_size = 0;
	num_dev_fonts = 0;
	max_dev_fonts = 0;
	dev_fonts = 0;
	ch_pkt = 0;
	pkt_len = 0;
	num_chars = 0;
	currentFont = -1;
}

VFFont::~VFFont()
{
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

void VFFont::drawChar(QPainter * painter,
                      long c, 
                      long x,
                      long y, 
                      long w,
                      long h,
                      long squareSize)
{
	clearState();
	int dpi = painter->device()->logicalDpiX();
	double factor = ((double)(72) / (double)(dpi));
	factor = factor / dvi2pts; 
	int tmpx = x;
	tmpx = qRound((double)tmpx * factor);
	int tmpy = y;
	tmpy = qRound((double)tmpy * factor);
	squareSize = qRound((double)(squareSize) * factor);
	tmpx += ((squareSize - w) / 2);
  tmpy += ((squareSize - h) / 2);
  drawChar(painter, c, tmpx, tmpy);
}

void VFFont::drawChar(QPainter * painter, 
	                  long ch, 
	                  long x, 
	                  long y)
{
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
	  			set1(&start, end, painter, x, y);
	  			break;
	  			
			case DVI_SET2:
	  			set2(&start, end, painter, x, y);
	  			break;
	  			
			case DVI_SET3:
				set3(&start, end, painter, x, y);
	  			break;
	  			
			case DVI_SET4:
				set4(&start, end, painter, x, y);
	  			break;
	  			
	  		case DVI_SETRULE:
	  			setRule(&start, end, painter, x, y);
	  			break;
	  			
			case DVI_PUT1:
	  			put1(&start, end, painter, x, y);
	  			break;
	  			
			case DVI_PUT2:
	  			put2(&start, end, painter, x, y);
	  			break;
	  			
			case DVI_PUT3:
				put3(&start, end, painter, x, y);
	  			break;
	  			
			case DVI_PUT4:
	  			put3(&start, end, painter, x, y);
	  			break;
	  			
			case DVI_PUTRULE:
	  			putRule(&start, end, painter, x, y);
	  			break;
	  			
			case DVI_NOP:
	  			break;
	  			
			case DVI_PUSH:
	  			push();
	  			break;
	  			
			case DVI_POP:
	  			pop();
	  			break;
	  			
			case DVI_RIGHT1:
	  			doRight1(&start, end);
	  			break;
	  			
			case DVI_RIGHT2:
	  			doRight2(&start, end);
	  			break;
	  			
			case DVI_RIGHT3:
	  			doRight3(&start, end);
	  			break;
	  			
			case DVI_RIGHT4:
	  			doRight4(&start, end);
	  			break;
	  			
			case DVI_W0:
	  			doW0();
	  			break;
	  			
			case DVI_W1:
	  			doW1(&start, end);
	  			break;
	  			
			case DVI_W2:
	  			doW2(&start, end);
	  			break;
	  			
			case DVI_W3:
	  			doW3(&start, end);
	  			break;
	  			
			case DVI_W4:
	  			doW4(&start, end);
	  			break;
	  			
			case DVI_X0:
	  			doX0();
	  			break;
	  			
			case DVI_X1:
	  			doX1(&start, end);
	  			break;
	  			
			case DVI_X2:
	  			doX2(&start, end);
	  			break;
	  			
			case DVI_X3:
	  			doX3(&start, end);
	  			break;
	  			
			case DVI_X4:
	  			doX4(&start, end);
	  			break;
	  			
			case DVI_DOWN1:
	  			doDown1(&start, end);
	  			break;
	  			
			case DVI_DOWN2:
	  			doDown2(&start, end);
	  			break;
	  			
			case DVI_DOWN3:
	  			doDown3(&start, end);
	  			break;
	  			
			case DVI_DOWN4:
	  			doDown4(&start, end);
	  			break;
	  			
			case DVI_Y0:
	  			doY0();
	  			break;
	  			
			case DVI_Y1:
	  			doY1(&start, end);
	  			break;
	  			
			case DVI_Y2:
	  			doY2(&start, end);
	  			break;
	  			
			case DVI_Y3:
	  			doY3(&start, end);
	  			break;
	  			
			case DVI_Y4:
	  			doY4(&start, end);
	  			break;
	  			
			case DVI_Z0:
	  			doZ0();
	  			break;
	  			
			case DVI_Z1:
	  			doZ1(&start, end);
	  			break;
	  			
			case DVI_Z2:
	  			doZ2(&start, end);
	  			break;
	  			
			case DVI_Z3:
	  			doZ3(&start, end);
	  			break;
	  			
			case DVI_Z4:
	  			doZ4(&start, end);
	  			break;
	  			
			case DVI_FNT1:
	  			doFnt1(&start, end);
	  			break;
	  			
			case DVI_FNT2:
	  			doFnt2(&start, end);
	  			break;
	  			
			case DVI_FNT3:
	  			doFnt3(&start, end);
	  			break;
	  			
			case DVI_FNT4:
	  			doFnt4(&start, end);
	  			break;
	  			
			case DVI_XXX1:
	  			doXXX1(&start, end);
	  			break;
	  			
			case DVI_XXX2:
	  			doXXX2(&start, end);
	  			break;
	  			
			case DVI_XXX3:
	  			doXXX3(&start, end);
	  			break;
	  			
			case DVI_XXX4:
	  			doXXX4(&start, end);
	  			break;
	  			
			case DVI_DIR:
				dviState.d = unsignedByte(&start, end) ? 1 : 0;
	  			break;
	  			
	  		default:
	  			if (opcode < DVI_SET1)
	  				doSet(opcode, painter, x, y);
	  			else if (opcode >= DVI_FNTNUM0 && opcode < DVI_FNT1)
	  				doFnt(opcode - DVI_FNTNUM0);
	  			break;
  		}
  	}
}

int VFFont::getNumGlyphs()
{
	return num_chars;
}

bool VFFont::load(const QString & name, long ptsizeA)
{
	ptsize = ptsizeA;
	XWFontSea sea;
    QFile * vf_file = sea.openVf(name);
    
    if (!vf_file)
    	vf_file = sea.openOvf(name);
    	
    if (!vf_file)
    	return false;
    	
    if (!readHeader(vf_file))
    	return false;
    	
    return processFile(vf_file);
}

void VFFont::clearState()
{
	dviState.h = 0;
  	dviState.v = 0; 
  	dviState.w = 0; 
  	dviState.x = 0; 
  	dviState.y = 0; 
  	dviState.z = 0; 
  	dviState.d = 0;
  	dviStackDepth = 0;
  	currentFont    = -1;
}
void VFFont::doDown(long y)
{
	y = scaleFixWord(ptsize, y);
	if (!dviState.d) 
   		dviState.v += y;
	else 
   		dviState.h -= y;
}
	
void VFFont::doDown1(uchar **start, uchar *end)
{
	doDown(signedByte(start, end));
}

void VFFont::doDown2(uchar **start, uchar *end)
{
	doDown(signedPair(start, end));
}

void VFFont::doDown3(uchar **start, uchar *end)
{
	doDown(signedTriple(start, end));
}

void VFFont::doDown4(uchar **start, uchar *end)
{
	doDown(signedQuad(start, end));
}
	
void VFFont::doFnt(long font_id)
{
	int i = 0;
	for (; i < num_dev_fonts; i++)
	{
		if (font_id == dev_fonts[i].font_id)
      		break;
	}
	
	if (i < num_dev_fonts)
	{
		currentFont = i;
    	widget->setFont(dev_fonts[i].dev_id);
    }
    else
    {
    	currentFont = -1;
    	widget->setFont(-1);
    }
}

void VFFont::doFnt1(uchar **start, uchar *end)
{
	doFnt(unsignedByte(start, end));
}

void VFFont::doFnt2(uchar **start, uchar *end)
{
	doFnt(unsignedPair(start, end));
}

void VFFont::doFnt3(uchar **start, uchar *end)
{
	doFnt(unsignedTriple(start, end));
}

void VFFont::doFnt4(uchar **start, uchar *end)
{
	doFnt(signedQuad(start, end));
}

void VFFont::doMoveTo(long x, long y)
{
	dviState.h = x;
	dviState.v = y;
}

void VFFont::doPut(long ch, 
	               QPainter * painter, 
	               long x, 
	               long y)
{
	int dpi = painter->device()->logicalDpiX();
	double rate = (double)dpi / 72.0;
	x += qRound(dviState.h * dvi2pts * rate);
	y += qRound(dviState.v * dvi2pts * rate);
	widget->drawChar(ch, painter, x, y);
}

void VFFont::doRight(long x)
{
	x = scaleFixWord(ptsize, x);
	if (!dviState.d) 
   		dviState.h += x;
	else 
   		dviState.v += x;
}
	
void VFFont::doRight1(uchar **start, uchar *end)
{
	doRight(signedByte(start, end));
}

void VFFont::doRight2(uchar **start, uchar *end)
{
	doRight(signedPair(start, end));
}

void VFFont::doRight3(uchar **start, uchar *end)
{
	doRight(signedTriple(start, end));
}

void VFFont::doRight4(uchar **start, uchar *end)
{
	doRight(signedQuad(start, end));
}

void VFFont::doRule(QPainter * painter, 
	                long x, 
	                long y, 
	                long width, 
	                long height)
{
  	doMoveTo(dviState.h, dviState.v);
  	double llx, lly, urx, ury;
  	int dpi = painter->device()->logicalDpiX();
	double rate = (double)dpi / 72.0;
  	if (!dviState.d)
  	{
  		llx =  dvi2pts * dviState.h + x;
    	lly =  dvi2pts * dviState.v + y;
    	urx =  dvi2pts * width;
    	ury =  dvi2pts * height;
    }
    else
    {
    	llx =  dvi2pts * dviState.h * rate + x;
    	lly =  dvi2pts * (dviState.v - width) + y;
    	urx =  dvi2pts * height;
    	ury =  dvi2pts * width;
    }
    	
    llx *= rate;
    lly *= rate;
    urx *= rate;
    ury *= rate;
  	QRectF r(llx, lly, urx, ury);
  	painter->save();
  	QColor color = painter->pen().color();
  	painter->setBrush(QBrush(color));
  	painter->fillRect(r, color);
  	painter->restore();
}

void VFFont::doSet(long ch, 
	               QPainter * painter, 
	               long x, 
	               long y)
{
	int dpi = painter->device()->logicalDpiX();
	double rate = (double)dpi / 72.0;
	x += qRound(dviState.h * dvi2pts * rate);
	y += qRound(dviState.v * dvi2pts * rate);
	widget->drawChar(ch, painter, x, y);
	if (currentFont < 0)
		return ;
		
	FontDef * font = &dev_fonts[currentFont];
	if (font->tfm_id < 0)
		return ;
		
	XWTFMFile tfm(font->tfm_id);
	long width = tfm.getFWWidth(ch);
  	width = scaleFixWord(font->size, width);
  	if (!dviState.d) 
    	dviState.h += width;
  	else 
    	dviState.v += width;
}

void VFFont::doW(long ch)
{
	dviState.w = scaleFixWord(ptsize, ch);
  	if (!dviState.d) 
   		dviState.h += dviState.w;
	else 
   		dviState.v += dviState.w;
}

void VFFont::doW0()
{
	if (!dviState.d) 
   		dviState.h += dviState.w;
	else 
   		dviState.v += dviState.w;
}

void VFFont::doW1(uchar **start, uchar *end)
{
	doW(signedByte(start, end));
}

void VFFont::doW2(uchar **start, uchar *end)
{
	doW(signedPair(start, end));
}

void VFFont::doW3(uchar **start, uchar *end)
{
	doW(signedTriple(start, end));
}

void VFFont::doW4(uchar **start, uchar *end)
{
	doW(signedQuad(start, end));
}

void VFFont::doX(long x)
{
	dviState.x = scaleFixWord(ptsize, x);
  	if (!dviState.d) 
   		dviState.h += dviState.x;
	else 
   		dviState.v += dviState.x;
}

void VFFont::doX0()
{
	if (!dviState.d) 
   		dviState.h += dviState.x;
	else 
   		dviState.v += dviState.x;
}

void VFFont::doX1(uchar **start, uchar *end)
{
	doX(signedByte(start, end));
}

void VFFont::doX2(uchar **start, uchar *end)
{
	doX(signedPair(start, end));
}

void VFFont::doX3(uchar **start, uchar *end)
{
	doX(signedTriple(start, end));
}

void VFFont::doX4(uchar **start, uchar *end)
{
	doX(signedQuad(start, end));
}

void VFFont::doXXX(long len, uchar **start, uchar *)
{
	*start += len;
}

void VFFont::doXXX1(uchar **start, uchar *end)
{
	doXXX(unsignedByte(start, end), start, end);
}

void VFFont::doXXX2(uchar **start, uchar *end)
{
	doXXX(unsignedPair(start, end), start, end);
}

void VFFont::doXXX3(uchar **start, uchar *end)
{
	doXXX(unsignedTriple(start, end), start, end);
}

void VFFont::doXXX4(uchar **start, uchar *end)
{
	doXXX(unsignedQuad(start, end), start, end);
}

void VFFont::doY(long y)
{
	dviState.y = scaleFixWord(ptsize, y);
  	if (!dviState.d) 
   		dviState.v += dviState.y;
	else 
   		dviState.h -= dviState.y;
}

void VFFont::doY0()
{
	if (!dviState.d) 
   		dviState.v += dviState.y;
	else 
   		dviState.h -= dviState.y;
}

void VFFont::doY1(uchar **start, uchar *end)
{
	doY(signedByte(start, end));
}

void VFFont::doY2(uchar **start, uchar *end)
{
	doY(signedPair(start, end));
}

void VFFont::doY3(uchar **start, uchar *end)
{
	doY(signedTriple(start, end));
}

void VFFont::doY4(uchar **start, uchar *end)
{
	doY(signedQuad(start, end));
}

void VFFont::doZ(long z)
{
	dviState.z = scaleFixWord(ptsize, z);
  	if (!dviState.d) 
   		dviState.v += dviState.z;
	else 
   		dviState.h -= dviState.z;
}

void VFFont::doZ0()
{
	if (!dviState.d) 
   		dviState.v += dviState.z;
	else 
   		dviState.h -= dviState.z;
}

void VFFont::doZ1(uchar **start, uchar *end)
{
	doZ(signedByte(start, end));
}

void VFFont::doZ2(uchar **start, uchar *end)
{
	doZ(signedPair(start, end));
}

void VFFont::doZ3(uchar **start, uchar *end)
{
	doZ(signedTriple(start, end));
}

void VFFont::doZ4(uchar **start, uchar *end)
{
	doZ(signedQuad(start, end));
}

void VFFont::pop()
{
	if (dviStackDepth <= 0)
    	return ;

  	dviState = dviStack[--dviStackDepth];
  	doMoveTo(dviState.h, dviState.v);
}
	
bool VFFont::processFile(QIODevice *vf_file)
{
	int eof = 0;
  	unsigned long font_id = -1;
  	while (!eof)
  	{
  		long code = getUnsignedByte(vf_file);
  		switch (code)
  		{
  			case DVI_FNTDEF1:
      			font_id = getUnsignedByte(vf_file);
      			readFontDef(vf_file, font_id);
      			break;
      			
      		case DVI_FNTDEF2:
      			font_id = getUnsignedPair(vf_file);
      			readFontDef(vf_file, font_id);
      			break;
      			
    		case DVI_FNTDEF3:
      			font_id = getUnsignedTriple(vf_file);
      			readFontDef(vf_file, font_id);
      			break;
      			
    		case DVI_FNTDEF4:
      			font_id = getSignedQuad(vf_file);
      			readFontDef(vf_file, font_id);
      			break;
      			
      		default:
      			if (code < 242) 
      			{
					long ch = getUnsignedByte(vf_file);
					getUnsignedTriple(vf_file);
					if (!readCharDef(vf_file, code, ch))
						return false;
					break;
      			}
      			
      			if (code == 242)
      			{
					ulong pkt_lenA = getUnsignedQuad(vf_file);
					ulong ch = getUnsignedQuad(vf_file);
					getUnsignedQuad(vf_file);
					if (ch < 65536L) 
					{
	  					if (!readCharDef(vf_file, pkt_lenA, ch))
	  						return false;
	  				}
					else
	  					return false;
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
  	
  	return true;
}

void VFFont::push()
{
	if (dviStackDepth >= DVI_STACK_DEPTH_MAX)
		return ;

  	dviStack[dviStackDepth++] = dviState;
}

void VFFont::putRule(uchar **start, 
	                 uchar *end,
	                 QPainter * painter, 
	                 long x, 
	                 long y)
{
	long height = signedQuad(start, end);
  	long width  = signedQuad(start, end);
  	if (width > 0 && height > 0) 
    	doRule(painter, x, y, scaleFixWord(ptsize, width), scaleFixWord(ptsize, height));
}

void VFFont::put1(uchar **start, 
	              uchar *end, 
	              QPainter * painter, 
	              long x, 
	              long y)
{
	doPut(unsignedByte(start, end), painter, x, y);
}

void VFFont::put2(uchar **start, 
	              uchar *end, 
	              QPainter * painter, 
	              long x, 
	              long y)
{
	doPut(unsignedPair(start, end), painter, x, y);
}

void VFFont::put3(uchar **start, 
	              uchar *end, 
	              QPainter * painter, 
	              long x, 
	              long y)
{
	doPut(unsignedTriple(start, end), painter, x, y);
}

void VFFont::put4(uchar **start, 
	              uchar *end, 
	              QPainter * painter, 
	              long x, 
	              long y)
{
	doPut(unsignedQuad(start, end), painter, x, y);
}

bool VFFont::readCharDef(QIODevice *vf_file, 
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
      		return false;
      	}
    	ch_pkt[ch] = pkt;
  	}
  	
  	pkt_len[ch] = pkt_lenA;
  	return true;
}

void VFFont::readFontDef(QIODevice *vf_file, long font_id)
{
	if (num_dev_fonts >= max_dev_fonts) 
	{
    	max_dev_fonts += 16;
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
  	dev_font->dev_id = widget->locateFont(dev_font->name, scaleFixWord(ptsize, dev_font->size));
}

bool VFFont::readHeader(QIODevice *vf_file)
{
	bool result = false;
	int ch = 0;
	if ((ch = getUnsignedByte(vf_file)) == DVI_PRE &&
        (ch = getUnsignedByte(vf_file)) == 202)
    {
    	ch = getUnsignedByte(vf_file);
    	for (int i = 0; i < ch; i++)
      		getUnsignedByte(vf_file);
      		
      	getUnsignedQuad(vf_file);
      	design_size = getUnsignedQuad(vf_file);
      	result = true;
    }
    	
    return result;
}

void VFFont::resize(unsigned size)
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

void VFFont::setRule(uchar **start, 
	                 uchar *end,
	                 QPainter * painter, 
	                 long x, 
	                 long y)
{
	long height = signedQuad(start, end);
  	long width  = signedQuad(start, end);
  	long s_width = scaleFixWord(ptsize, width);
  	if (width > 0 && height > 0) 
    	doRule(painter, x, y, s_width, scaleFixWord(ptsize, height));
  	
  	doRight(s_width);
}

void VFFont::set1(uchar **start, 
	              uchar *end,
	              QPainter * painter, 
	              long x, 
	              long y)
{
	doSet(unsignedByte(start, end), painter, x, y);
}

void VFFont::set2(uchar **start, 
	              uchar *end,
	              QPainter * painter, 
	              long x, 
	              long y)
{
	doSet(unsignedPair(start, end), painter, x, y);
}

void VFFont::set3(uchar **start, 
	              uchar *end,
	              QPainter * painter, 
	              long x, 
	              long y)
{
	doSet(unsignedTriple(start, end), painter, x, y);
}

void VFFont::set4(uchar **start, 
	              uchar *end,
	              QPainter * painter, 
	              long x, 
	              long y)
{
	doSet(unsignedQuad(start, end), painter, x, y);
}

XWCharacterWidget::XWCharacterWidget(QWidget * parent)
	:QWidget(parent)
{
	setMouseTracking(true);
	unitNum = 25400000;
  	unitDen = 473628672;
  	dvi2pts  = (double) unitNum / (double) unitDen;
  	dvi2pts  *= (72.0 / 254000.0);
  	
	loadedFonts = 0;
	numLoadedFonts = 0;
	maxLoadedFonts = 0;
	
	currentFont = -1;
	
	devFonts = 0;
	numDevFonts = 0;
	maxDevFonts = 0;
	
	devID = -1;
	fontSize = 10.0;
	fontEngine = new XWFTFontEngine(true);
	
	squareSize = 24;
	columns = 16;
	lastKey = -1;
}

XWCharacterWidget::~XWCharacterWidget()
{
	if (loadedFonts)
		free(loadedFonts);
		
	if (devFonts)
	{
		for (int i = 0; i < numDevFonts; i++)
		{
			if (devFonts[i].name)
				delete devFonts[i].name;
				
			if (devFonts[i].type == FontPK)
			{
				XWFontFilePK * pkfont = (XWFontFilePK*)(devFonts[i].font);
				delete pkfont;
			}
			else if (devFonts[i].type == FontVF)
			{
				VFFont * vffont = (VFFont*)(devFonts[i].font);
				delete vffont;
			}
		}
	}
	
	if (fontEngine)
		delete fontEngine;
}

void XWCharacterWidget::displayFTFont(const QString & name, 
	                                  int index, 
	                                  int pid,
	                                  int eid,
	                                  double size,
	                                  double efactorA,
	                                  double slantA,
	                                  double boldfA,
	                                  bool boldA,
	                                  bool rotateA,
	                                  const QString & encname,
	                                  int subfontid)
{
	displayFTFont(name, index, size, efactorA, slantA, boldfA, boldA, rotateA, encname, subfontid);	
	XWFTFont * ftfont = getFTFont();
	if (ftfont)
		ftfont->setCharMap(pid, eid);
	adjustSize();
    update();
}

void XWCharacterWidget::displayFTFont(const QString & name, 
	                                  int index, 
	                                  int map,
	                                  double size,
	                                  double efactorA,
	                                  double slantA,
	                                  double boldfA,
	                                  bool boldA,
	                                  bool rotateA,
	                                  const QString & encname,
	                                  int subfontid)
{
	displayFTFont(name, index, size, efactorA, slantA, boldfA, boldA, rotateA, encname, subfontid);	
	XWFTFont * ftfont = getFTFont();
	if (ftfont)
	{
		ftfont->setCharMap(map);
	}
	adjustSize();
    update();
}

void XWCharacterWidget::displayPKFont(const QString & name)
{
	typeText = "Packed File Format";
	QByteArray ba = name.toAscii();
	double size = 10;
	XWTFMFile tfm(ba.constData());
	if (tfm.getID() >= 0)
		size = tfm.getDesignSize();
		
	fontSize = size;
	
	devID = findPKFont(ba.constData(), logicalDpiX(), size);	
	int pixelsize = qRound((size * (double)logicalDpiX()) / 72.0);
	if (pixelsize < 24)
		squareSize = 24;
	else
		squareSize = pixelsize + 10;
		
	adjustSize();
    update();
}

void XWCharacterWidget::displayVFFont(const QString & name, 
	                                  const QString & tfmname)
{
	typeText = "Virtual Font";
	QByteArray ba = name.toAscii();
	QByteArray batfm = tfmname.toAscii();
	devID = -1;
	squareSize = 24;
	if (batfm.size() > 0)
	{
		XWTFMFile tfm(batfm.constData());
		if (tfm.getID() >= 0)
		{
			double size = tfm.getDesignSize();
			fontSize = size;			
			long ptsize = qRound(size / dvi2pts);
			devID = findVFFont(ba.constData(), ptsize, tfm.getID());
			int pixelsize = qRound((size * (double)logicalDpiX()) / 72.0);
			if (pixelsize < 24)
				squareSize = 24;
			else
				squareSize = pixelsize + 10;
		}
	}
	
	adjustSize();
    update();
}

void XWCharacterWidget::drawChar(int ch, 
	                             QPainter * painter, 
	                             int x, 
	                             int y)
{
	if (currentFont < 0)
		return ;
		
	DVILoadedFont * font  = &loadedFonts[currentFont];
	if (font->tfmId < 0 || font->fontId < 0)
		return ;
			
	if (font->subfontId >= 0)
	{
		XWSubfont subfont;
  		ch = subfont.lookupRecord(font->subfontId, (uchar)ch);
	}
	
	DevFont * devfont = &devFonts[font->fontId];
	switch (devfont->type)
	{
		case FontFT:
			{	
				XWFTFont * ftfont = (XWFTFont*)(devfont->font);
				ftfont->drawChar(painter, ch, x, y);
			}
			break;
			
		case FontPK:
			{
				XWFontFilePK * pkfont = (XWFontFilePK*)(devfont->font);
				pkfont->drawChar(painter, ch, x, y);
			}
			break;
			
		case FontVF:
			{
				VFFont * vffont = (VFFont*)(devfont->font);
				vffont->drawChar(painter, ch, x, y);
			}
			break;
	}
}

XWFTFont * XWCharacterWidget::getFTFont()
{
	if (devID == -1)
		return 0;
		
	DevFont * font = &devFonts[devID];
	if (font->type != FontFT)
		return 0;
		
	XWFTFont * ftfont = (XWFTFont*)(font->font);
	return ftfont;
}

int XWCharacterWidget::getNumGlyphs() const
{
	if (devID == -1)
		return 0;
		
	DevFont * font = &devFonts[devID];
	if (font->subfontID >= 0)
		return 256;
		
	XWFTFont * ftfont = 0;
	XWFontFilePK * pkfont = 0;
	VFFont * vffont = 0;
	int ret = 0;
	switch (font->type)
	{
		case FontFT:
			ftfont = (XWFTFont*)(font->font);
			ret = ftfont->getNumGlyphs();
			break;
					
		case FontPK:
			pkfont = (XWFontFilePK*)(font->font);
			ret = pkfont->getNumGlyphs();
			break;
					
		case FontVF:
			vffont = (VFFont*)(font->font);
			ret = vffont->getNumGlyphs();
			break;
	}
	
	return ret;
}

int XWCharacterWidget::locateFont(const char *tfm_name, 
	                                 long ptsize)
{
	if (!tfm_name || strlen(tfm_name) < 1)
		return -1;
		
	XWTFMFile tfm(tfm_name);
	for (int i = 0; i < numLoadedFonts; i++)
	{
		if (loadedFonts[i].tfmId == tfm.getID() && 
			loadedFonts[i].size == ptsize)
		{
			return i;
		}
	}
	
	XWFontCache fcache(true);
	XWTexFontMap * mrec = fcache.lookupFontMapRecord(tfm_name);
	int subfont_id = -1;
	if (mrec && mrec->charmap.sfd_name && mrec->charmap.subfont_id) 
	{
		QString n(mrec->charmap.sfd_name);
		QString id(mrec->charmap.subfont_id);
		XWSubfont subfont(n);
    	subfont_id = subfont.loadRecord(id);
  	}
  	
  	needMoreFonts(1);
  	int cur_id = numLoadedFonts;
  	loadedFonts[cur_id].tfmId     = tfm.getID();
  	loadedFonts[cur_id].subfontId = subfont_id;
  	loadedFonts[cur_id].size      = ptsize;
  	  	
  	char * name = 0;
  	if (mrec && mrec->font_name) 
    	name = mrec->font_name;
  	else 
    	name = (char*)tfm_name;

    int font_id = -1;
  	if (!mrec || (mrec && mrec->useVF() && mrec->getVFont()))
  	{
  		if (!mrec)
  			font_id = findVFFont(tfm_name, ptsize, tfm.getID());
  		else
  			font_id = findVFFont(mrec->getVFont(), ptsize, tfm.getID());
  	}
  	else
  	{
  		XWFontSea sea;
  		QString fn(name); 
  		if (sea.findFormat(fn) != XWFontSea::PK)
  		{
  			font_id = findFTFont(name, 
  			                     mrec->opt->index,
  			                     ptsize * dvi2pts, 
  			                     mrec->opt->extend,
  		                         mrec->opt->slant,
  		                         mrec->opt->bold,
  		                         mrec->opt->boldb,
  		                         mrec->opt->rotate,
  		                         mrec->opt->outencname,
  		                         subfont_id);
			if (font_id >= 0)
			{
				XWFTFont * ftfont = (XWFTFont*)(&devFonts[font_id]);
				ftfont->setCharMap(mrec->opt->pid, mrec->opt->eid);
			}
  		}
  		else
  			font_id = findPKFont(name, logicalDpiX(), ptsize * dvi2pts);
  	}
  		
  	if (font_id < 0)
  		return -1;
  	  	
  	loadedFonts[cur_id].fontId = font_id;
  	numLoadedFonts++;
  	return  cur_id;
}

QSize XWCharacterWidget::sizeHint() const
{
	if (getNumGlyphs() <= 256)
		return QSize(columns*squareSize, (256/columns)*squareSize);	
	
	return QSize(columns*squareSize, (65536/columns)*squareSize);
}

void XWCharacterWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (devID < 0)
		return ;
		
	DevFont * font = &devFonts[devID];		
	QPoint widgetPosition = mapFromGlobal(event->globalPos());
	uint key = (widgetPosition.y()/squareSize)*columns + widgetPosition.x()/squareSize;
	if (font->subfontID >= 0)
	{
		XWSubfont subfont;
  		key = (uint)(subfont.lookupRecord(font->subfontID, (uchar)key));
	}
	
	QString text = QString(tr("Value: 0x%1")).arg(key, 0, 16);
	text += QString("  ");
	text += QChar(key);
	QToolTip::showText(event->globalPos(), text, this);
}

void XWCharacterWidget::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		lastKey = (event->y()/squareSize)*columns + event->x()/squareSize;
		update();
	}
	else
		QWidget::mousePressEvent(event);
}

void XWCharacterWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.fillRect(event->rect(), QBrush(Qt::white));
	QRect redrawRect = event->rect();
    int beginRow = redrawRect.top()/squareSize;
    int endRow = redrawRect.bottom()/squareSize;
    int beginColumn = redrawRect.left()/squareSize;
    int endColumn = redrawRect.right()/squareSize;
    
    painter.setPen(QPen(Qt::gray));
	for (int row = beginRow; row <= endRow; ++row)
	{
		for (int column = beginColumn; column <= endColumn; ++column)
			painter.drawRect(column*squareSize, row*squareSize, squareSize, squareSize);
	}
	
	if (devID < 0)
		return ;
		
	DevFont * font = &devFonts[devID];
	XWFTFont * ftfont = 0;
	XWFontFilePK * pkfont = 0;
	VFFont * vffont = 0;
	switch (font->type)
	{
		case FontFT:
			ftfont = (XWFTFont*)(font->font);
			break;
					
		case FontPK:
			pkfont = (XWFontFilePK*)(font->font);
			break;
					
		case FontVF:
			vffont = (VFFont*)(font->font);
			break;
	}
		
	int x = beginColumn * squareSize;
	int y = beginRow * squareSize;
	painter.setPen(QPen(Qt::black));
	for (int row = beginRow; row <= endRow; ++row)
	{
		x = beginColumn * squareSize;
		for (int column = beginColumn; column <= endColumn; ++column)
		{
			int key = row*columns + column;
			int ch = key;
			if (font->subfontID >= 0)
			{
				XWSubfont subfont;
  				ch = subfont.lookupRecord(font->subfontID, (uchar)ch);
			}
			
			painter.setClipRect(column*squareSize, row*squareSize, squareSize, squareSize);
			switch (font->type)
			{
				case FontFT:
					ftfont->drawChar(&painter, ch, &x, &y, squareSize);
					break;
					
				case FontPK:
					pkfont->drawChar(&painter, ch, &x, &y, squareSize);
					break;
					
				case FontVF:
					if (font->tfmID >= 0)
					{
						XWTFMFile tfm(font->tfmID);
						long pixelw = tfm.getFWWidth(ch);
						long pixelh = tfm.getFWHeight(ch);
						if (pixelw > 0 && pixelh > 0)
							vffont->drawChar(&painter, ch, (long)x, (long)y, pixelw, pixelh, squareSize);
					}
					break;
			}
			
			x += squareSize;
		}
		
		y += squareSize;
	}
}

void XWCharacterWidget::displayFTFont(const QString & name, 
	                                  int index, 
	                                  double size,
	                                  double efactorA,
	                                  double slantA,
	                                  double boldfA,
	                                  bool boldA,
	                                  bool rotateA,
	                                  const QString & encname,
	                                  int subfontid)
{
	QByteArray ba = name.toAscii();
	fontSize = size;
	XWFontSea sea;
  	switch (sea.findFormat(name))
  	{
  		case XWFontSea::BDF:
  			typeText = "Bitmap Distribution Format";
  			break;
  			
  		case XWFontSea::CFF:
  			typeText = "Compact Font Format";
  			break;
  			
  		case XWFontSea::OpenType:
  			typeText = "OpenType";
  			break;
  			
  		case XWFontSea::PCF:
  			typeText = "Portable Compiled Format";
  			break;
  			
  		case XWFontSea::PFR:
  			typeText = "Portable Font Resource";
  			break;
  			
  		case XWFontSea::TrueType:
  			typeText = "TrueType";
  			break;
  			
  		case XWFontSea::Type1:
  			typeText = "PostScript Type1";
  			break;
  			
  		case XWFontSea::WinFont:
  			typeText = "Microsoft Windows Font File Format";
  			break;
  			
  		default:
  			typeText = "Supported by FreeType2";
  			break;
  	}
  	
	char * enc = 0;
	QByteArray baenc;
	if (!encname.isEmpty())
	{
		baenc = encname.toAscii();
		enc = baenc.data();
	}
	devID = findFTFont(ba.constData(), index, size, efactorA, slantA, boldfA, boldA, rotateA, enc, subfontid);
	int pixelsize = qRound((size * (double)logicalDpiX()) / 72.0);
	if (pixelsize < 24)
		squareSize = 24;
	else
		squareSize = pixelsize + 10;
}

int  XWCharacterWidget::findFTFont(const char * name, 
								   int index,
	                               double size,
	                               double efactorA,
	                               double slantA,
	                               double boldfA,
	                               bool boldA,
	                               bool rotateA,
	                               char * encname,
	                               int subfontid)
{
	QString qname(name);
	QString strname = QString("%1%2").arg(qname).arg(index);
	char ** encA = 0;
	int enc_id = -1;
	if (encname)
	{
		XWFontCache fcache(true);
		enc_id = fcache.findEncoding(encname);
		if (enc_id >= 0)
			encA = fcache.getEncoding(enc_id);
	}
	
	if (encA)
		strname += QString("%1").arg(enc_id);
	strname += QString("%1").arg(index);
	QByteArray ba = strname.toAscii();
	char * tmpname = qstrdup(ba.constData());
	
	int pixelsize = qRound((size * (double)logicalDpiX()) / 72.0);
	for (int i = 0; i < numDevFonts; i++)
	{
		DevFont * font = (DevFont*)(&devFonts[i]);
		if (font->type != FontFT)
			continue;
			
		if (!strcmp(tmpname, font->name) && 
			 (font->ptsize == pixelsize) && 
			 font->index == index)
		{
			font->subfontID = subfontid;
			return i;
		}
	}
	
	XWFontFileID * id = new XWFontFileID(tmpname);
  	XWFTFontFile * ftfontFile = fontEngine->getFontFile(id);
  	if (ftfontFile)
  	{
  		delete id;
  		id = 0;
  	}
  	else
  	{
  		XWFontSea sea;
  		QString fn = sea.findFile(qname);
  		if (fn.isEmpty())
  		{
  			delete id;
  			delete [] tmpname;
  			return -1;
  		}
  		
  		ba = QFile::encodeName(fn);
  		ftfontFile = fontEngine->load(id, ba.data(), index, encA);
  		if (!ftfontFile)
  		{
  			delete id;
  			delete [] tmpname;
  			return -1;
  		}
  	}
  	
  	XWFTFont * ftfont = fontEngine->getFont(ftfontFile, 
  				                            pixelsize, 
  				                            efactorA,
  				                            slantA,
  				                            boldfA,
  				                            boldA,
  				                            rotateA);

	if (numDevFonts >= maxDevFonts) 
  	{
  		maxDevFonts += 16;
    	devFonts    = (DevFont*)realloc(devFonts, maxDevFonts * sizeof(DevFont));
  	}
  	
  	int cur_id = numDevFonts++;
  	devFonts[cur_id].type = FontFT;
  	devFonts[cur_id].ptsize = pixelsize;
  	devFonts[cur_id].tfmID = -1;
  	devFonts[cur_id].subfontID = subfontid;
  	devFonts[cur_id].name = tmpname;
  	devFonts[cur_id].font = ftfont;
  	devFonts[cur_id].index = index;
  	return cur_id;
}

int  XWCharacterWidget::findPKFont(const char * name,
	                               int base_dpi,
	                               double font_scale)
{
	long ptsize = qRound(font_scale / dvi2pts);
	for (int i = 0; i < numDevFonts; i++)
	{
		DevFont * font = (DevFont*)(&devFonts[i]);
		if (font->type != FontPK)
			continue;
			
		if (!strcmp(name, font->name) && (font->ptsize == ptsize))
		{
			return i;
		}
	}
	
	XWFontFilePK * pkfont = XWFontFilePK::load(name, base_dpi, font_scale);
	if (!pkfont)
		return -1;
		
	if (numDevFonts >= maxDevFonts) 
  	{
  		maxDevFonts += 16;
    	devFonts    = (DevFont*)realloc(devFonts, maxDevFonts * sizeof(DevFont));
  	}
  			
  	int cur_id = numDevFonts++;
  	devFonts[cur_id].type = FontPK;
  	devFonts[cur_id].ptsize = ptsize;
  	devFonts[cur_id].tfmID = -1;
  	devFonts[cur_id].subfontID = -1;
  	devFonts[cur_id].name = qstrdup(name);
  	devFonts[cur_id].font = pkfont;
  	devFonts[cur_id].index = 0;
  	return cur_id;
}

int  XWCharacterWidget::findVFFont(const char *name, 
	                               long ptsize,
	                               int  tmfid)
{
	for (int i = 0; i < numDevFonts; i++)
	{
		DevFont * font = (DevFont*)(&devFonts[i]);
		if (font->type != FontVF)
			continue;
			
		if (!strcmp(name, font->name) && (font->ptsize == ptsize))
		{
			return i;
		}
	}
	
	VFFont * vffont = new VFFont(this);
	if (!vffont->load(name, ptsize))
	{
		delete vffont;
		return -1;
	}
	
	if (numDevFonts >= maxDevFonts) 
  	{
  		maxDevFonts += 16;
    	devFonts    = (DevFont*)realloc(devFonts, maxDevFonts * sizeof(DevFont));
  	}
  	
  	int cur_id = numDevFonts++;
  	devFonts[cur_id].type = FontVF;
  	devFonts[cur_id].ptsize = ptsize;
  	devFonts[cur_id].tfmID = tmfid;
  	devFonts[cur_id].subfontID = -1;
  	devFonts[cur_id].name = qstrdup(name);
  	devFonts[cur_id].font = vffont;
  	devFonts[cur_id].index = 0;
  	
  	return cur_id;
}

void XWCharacterWidget::needMoreFonts(int n)
{
	if (numLoadedFonts + n > maxLoadedFonts) 
	{
    	maxLoadedFonts += 16;
    	loadedFonts = (DVILoadedFont*)realloc(loadedFonts, maxLoadedFonts * sizeof(DVILoadedFont));
  	}
}

