/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWFONTFILEVF_H
#define XWFONTFILEVF_H

#include <QObject>
#include <QIODevice>
#include "XWGlobal.h"

class XWDVICore;

class XW_DVI_EXPORT XWFontFileVF : public QObject
{
	Q_OBJECT
	
public:
	XWFontFileVF();
	~XWFontFileVF();
	
	static void close();
	static int locateFont(XWDVICore * coreA, 
	                      const char *tex_name, 
	                      long ptsizeA);
	                      
	static void setChar(XWDVICore * coreA, long ch, int vf_font);
	
private:
	void changeCore(XWDVICore * coreA, long ptsizeA);
	
	void doDir(XWDVICore * coreA, uchar **start, uchar *end);
	void doDown(XWDVICore * coreA, long y, long ptsizeA);
	void doDown1(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA);
	void doDown2(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA);
	void doDown3(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA);
	void doDown4(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA);
	void doFnt(XWDVICore * coreA, long font_id);
	void doFnt1(XWDVICore * coreA, uchar **start, uchar *end);
	void doFnt2(XWDVICore * coreA, uchar **start, uchar *end);
	void doFnt3(XWDVICore * coreA, uchar **start, uchar *end);
	void doFnt4(XWDVICore * coreA, uchar **start, uchar *end);
	void doRight(XWDVICore * coreA, long x, long ptsizeA);
	void doRight1(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA);
	void doRight2(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA);
	void doRight3(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA);
	void doRight4(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA);
	void doW(XWDVICore * coreA, long w, long ptsizeA);
	void doW0(XWDVICore * coreA);
	void doW1(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA);
	void doW2(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA);
	void doW3(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA);
	void doW4(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA);
	void doX(XWDVICore * coreA, long x, long ptsizeA);
	void doX0(XWDVICore * coreA);
	void doX1(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA);
	void doX2(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA);
	void doX3(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA);
	void doX4(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA);
	void doXXX(XWDVICore * coreA, long len, uchar **start, uchar *end);
	void doXXX1(XWDVICore * coreA, uchar **start, uchar *end);
	void doXXX2(XWDVICore * coreA, uchar **start, uchar *end);
	void doXXX3(XWDVICore * coreA, uchar **start, uchar *end);
	void doXXX4(XWDVICore * coreA, uchar **start, uchar *end);
	void doY(XWDVICore * coreA, long y, long ptsizeA);
	void doY0(XWDVICore * coreA);
	void doY1(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA);
	void doY2(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA);
	void doY3(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA);
	void doY4(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA);
	void doZ(XWDVICore * coreA, long z, long ptsizeA);
	void doZ0(XWDVICore * coreA);
	void doZ1(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA);
	void doZ2(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA);
	void doZ3(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA);
	void doZ4(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA);
	
	void pop(XWDVICore * coreA);
	void processFile(XWDVICore * coreA, QIODevice *vf_file);
	void push(XWDVICore * coreA);
	void put1(XWDVICore * coreA, uchar **start, uchar *end);
	void put2(XWDVICore * coreA, uchar **start, uchar *end);
	void put3(XWDVICore * coreA, uchar **start, uchar *end);
	void put4(XWDVICore * coreA, uchar **start, uchar *end);	
	void putRule(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA);	
	
	void readCharDef(QIODevice *vf_file, 
	                 ulong pkt_lenA,
			         unsigned ch);
	void readFontDef(XWDVICore * coreA, QIODevice *vf_file, long font_id);
	int  readHeader(QIODevice *vf_file);
	void resize(unsigned size);
	
	void setChar(XWDVICore * coreA, long ch);
	
	void set(XWDVICore * coreA, long ch);
	void set1(XWDVICore * coreA, uchar **start, uchar *end);
	void set2(XWDVICore * coreA, uchar **start, uchar *end);
	void set3(XWDVICore * coreA, uchar **start, uchar *end);
	void set4(XWDVICore * coreA, uchar **start, uchar *end);
	void setRule(XWDVICore * coreA, uchar **start, uchar *end, long ptsizeA);
	
private:
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
	
	char *tex_name;
	long ptsize;
	ulong design_size;
	int num_dev_fonts;
	int max_dev_fonts;
	
	FontDef * dev_fonts;
	uchar **ch_pkt;
  	ulong *pkt_len;
  	uint num_chars;
  	
  	XWDVICore * core;
};

#endif //XWFONTFILEVF_H