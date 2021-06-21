/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTFMCREATOR_H
#define XWTFMCREATOR_H

#include <QObject>
#include <QIODevice>

#include "XWGlobal.h"
#include "TexEncoding.h"
#include "XWFontFileFT.h"

class XWFontFileFT;

class XW_FONT_EXPORT XWTFMCreator : public QObject
{
	Q_OBJECT
	
public:
	XWTFMCreator(const char * texnameA, QObject * parent = 0);
	~XWTFMCreator();
	
	bool isOFM();
	bool isOK() {return fontFile != 0;}
	
	void writeTFM(QIODevice * file);
	void writeVF(QIODevice * file);
	
private:
	void buildTFM();
	void buildTitle(char * buf);
	void buildVF();
	
	long checkSum(FTFontInfo ** array);
	
	long * makebcpl(long *p,
                    char *s,
                    int n);
	int mincover(long *what, long d);
	
	void remap(long *what,
               int oldn,
               int newn,
               long *source,
               long *unsort);
	
	void writeArr(long *p, int n, QIODevice * file);
	void writeSArr(long *what, int len, QIODevice * file);
	void write16(short what, QIODevice * file);
	void write32(long what, QIODevice * file);
	
	void corrAndCheck();
	
	void fontAtStep(double s);
	void fontNameStep(char * name);
	
	void kernStep(int c, double k);
	
	void labelStep(int c, bool b = false);
	void ligStep(char * opstr, int c1, int c2);
	
	void mapEndStep(int c);
	void mapFontStep(int c);
	void mapStep(uchar c);
	void moveRightStep(double num);
	void moveUpStep(double num);
	
	bool numbersDiffer()
	{
		return ((fontNumber[curFont].b3 != fontNumber[fontPtr].b3) || 
		        (fontNumber[curFont].b2 != fontNumber[fontPtr].b2) || 
		        (fontNumber[curFont].b1 != fontNumber[fontPtr].b1) || 
		        (fontNumber[curFont].b0 != fontNumber[fontPtr].b0));
	}
	
	void setCharStep(uchar cc);
	void selectFontStep(uchar c);
	void stopStep();
	
	void vfFix(uchar opcode, long num);
	void vfStore(uchar c);
	void voutInt(long x, QIODevice * file);
	void vtitleStep();
	
private:
	int lf;
	int lh;
	int nw;
	int nh;
	int nd;
	int ni;
	int nl;
	int nk;
	int ne;
	int np;
	
	long nco;
	long ncw;
	long npc;
	long nki;
  	long nwi;
  	long nkf;
  	long nwf;
  	long nkm;
  	long nwm;
  	long nkr;
  	long nwr;
  	long nkg;
  	long nwg;
  	long nkp;
  	long nwp;
	
	long * header;
	long * charInfo;
    long * width;
    long * height;
    long * depth;
    long * ligkern;
    long * kerns;
    long * tparam;
    long * italic;
    
    char * texName;
	
	long   nextd;
  	
  	int * char_tag;
  	
  	bool extra_loc_needed;
  	long bchar;
  	long lk_offset;
  	
  	struct LigKern
  	{
  		uchar b0;
  		uchar b1;
  		uchar b2;
  		uchar b3;
  	};
  	
  	LigKern * lig_kern;
  	
  	int minnl;
  	ulong * char_remainder;
  	
  	struct Label
  	{
  		int rr;
  		int cc;
  	};
  	
  	uchar * vf;
  	int vfPtr;
  	
  	int vtitleStart;
  	int vtitleLength;
  	int * packetStart;
  	int * packetLength;
  	
  	uchar * hstack;
  	uchar * vstack;
  	long * wstack;
  	long * xstack;
  	long * ystack;
  	long * zstack;
  	int stackPtr;
  	int hstackPtr;
  	int vstackPtr;
  	
  	int fontPtr;
  	int curFont;
  	int * fnameStart;
  	uchar * fnameLength;
  	int * fareaStart;
  	uchar * fareaLength;
  	
  	struct FontNumber
  	{
  		uchar b0;
  		uchar b1;
  		uchar b2;
  		uchar b3;
  	};
  	
  	FontNumber * fontNumber;
  	long * fontAt;
  	long * fontDSize;
  	
  	XWFontFileFT * fontFile;
};

#endif // XWTFMCREATOR_H
