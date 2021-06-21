/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWFONTFILEPK_H
#define XWFONTFILEPK_H

#include <QHash>
#include <QPainter>
#include <QColor>
#include <QImage>

#include "XWFontFile.h"

class XW_FONT_EXPORT XWFontFilePK : public XWFontFile
{
public:
	virtual ~XWFontFilePK();
	
	void drawChar(QPainter * painter,
                  int c, 
                  int * x,
                  int * y, 
                  int squareSize);
	
	void drawChar(QPainter * painter,
                  int c, 
                  int x,
                  int y);
                        
	int    getNumGlyphs();
	const uchar * getCharProcStream(long c, long *lenA);
	
	static XWFontFilePK * load(const char * name, 
	                           int base_dpi,
	                           double point_size);
	                           
	static XWFontFilePK * load(const char * name, 
	                           int base_dpi,
	                           double point_size,
	                           char * usedchars,
	                           double *FontBBox,
	                           double * Widths,
	                           double *FontMatrix,
	                           int    * FirstChar,
	                           int    * LastChar);
	                           
	static bool open(const char * name, 
	                 int base_dpi,
	                 double point_size);
		
private:
	XWFontFilePK(QIODevice * fileA,
		 	     int    fileFinalA,
		 	     ulong offsetA,
		 	     ulong lenA,
		 	     double pix2charuA);
		
	void addToStream(uchar *rowptr, long rowbytes);
                           	  
	int decodeBitmap(long wd, 
	                 long ht,
                     uchar *dp, 
                     long pl);
	int decodePacked(long wd, 
	                 long ht, 
	                 int dyn_f, 
	                 int run_color, 
	                 uchar *dp, 
	                 long pl);
	void doPreamble();
	void doSkip(unsigned long length);
	                 
	long fillBlackRun(uchar *dp, long left, long run_count);
	long fillWhiteRun(long run_count) {return  run_count;}
	
	uchar * getImgLine();
	bool    getPixel(uchar *pix);
	
	void initImg(long widthA);
	
	long packedNum(long *np, int dyn_f, uchar *dp, long pl);
	
	QImage toQImage(long widthA, long heightA, long wd);
	
private:
	double pix2charu;
	uchar * stream;
	long   streamLength;
	long    curChar;
	long nVals;
	long inputLineSize;
	uchar * inputLine;
	uchar *imgLine;
	long imgLineSize;
	long imgIdx;
	
	struct PKHeader
	{
		ulong pos;
  		ulong pkt_len;
		long  chrcode;
  		long  wd, dx, dy;
  		long  bm_wd, bm_ht, bm_hoff, bm_voff;
  		int   dyn_f, run_color;
	};
	
	QHash<long, PKHeader*> charHeaders;
};

#endif // XWFONTFILEPK_H

