/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTFMFILE_H
#define XWTFMFILE_H

#include <QObject>
#include <QIODevice>
#include <QString>
#include <QHash>
#include "XWGlobal.h"

class FontMetric;

class XW_FONT_EXPORT XWTFMFile : public QObject
{
	Q_OBJECT
	
public:
	XWTFMFile(int idA, QObject * parent = 0);
    XWTFMFile(const char * nameA, QObject * parent = 0);
    ~XWTFMFile();
    
    bool atEnd();
    
    static void closeAll();
    
    uchar getByte();
    
    long getFWDepth(long ch);
    long getFWHeight(long ch);
    long getFWWidth(long ch);
    int  getID() {return id;}
    
    double getDepth(long ch);
    double getDesignSize();
    double getHeight(long ch);
    long   getStringDepth(const uchar *s, long len);
    long   getStringHeight(const uchar *s, long len);
    long   getStringWidth(const uchar *s, long len);
    double getWidth(long ch);
    
    long readSixteen();
    long readSixteenUnsigned();
    long readThirtyTwo();
    
    static void setNeedMetric(bool e);
    
private:
	static void metricNeed(int n);

private:
	int id;
	QIODevice * file;
	
	static bool needMetrics;
	static FontMetric ** fms;
	static int numFMs;
	static int maxFMs;
};

class TFMFile : public QObject
{
	Q_OBJECT
	
public:
	friend class FontMetric;
	
	TFMFile(QObject * parent = 0);
	~TFMFile();
	
	bool readFile(QIODevice * file);
	
public:
	int     id;
	int     nt;
	long    level;
	long    lf;
  	long    lh;
  	long    bc;
  	long    ec;
  	long    nw;
  	long    nh;
  	long    nd;
  	long    ni;
  	long    nl;
  	long    nk;
  	long    ne;
  	long    np;
  	long    fontdir;
  	long    nco;
  	long    ncw;
  	long    npc;
  	
  	
  	long * header;
  	long * chartypes;
  	ulong * charinfo;
  	long * width_index;
  	int  * height_index;
  	int  * depth_index;
  	long * width;
  	long * height;
  	long * depth;
  	
private:
	bool checkOFMSizeOne(QIODevice *ofm_file);
	bool checkTFMSize(QIODevice * tfm_file);
	
	void doJFMCharTypeArray(QIODevice *tfm_file);
	bool doOFMCharInfoOne(QIODevice *ofm_file);
	void doOFMCharInfoZero(QIODevice *ofm_file);
	
	bool getOFMSizes(QIODevice *ofm_file);
	bool getTFMSizes(QIODevice * tfm_file);
	                 
	bool readOFM(QIODevice *ofm_file);
	bool readTFM(QIODevice *tfm_file);
	             
	static long readUQuads(ulong *quads, long nmemb, QIODevice *fp);
	static long readWords(long * words, long nmemb, QIODevice *fp);
};

class FontMetric : public QObject
{
	Q_OBJECT
	
public:
	friend class TFMFile;
	
	FontMetric(QObject * parent = 0);
	~FontMetric();
	
	long getDepth(long ch);
	long getHeight(long ch);
	long getWidth(long ch);
	
	void readOFM(TFMFile * tfm);
	void readTFM(TFMFile * tfm);
	
public:
  	long   designSize;
  	char * codingScheme;
  	int    fontDir;
  	long   firstChar, lastChar;
  	long * widths;
  	long * heights;
  	long * depths;

  	struct 
  	{
    	int   type;
    	void *data;
  	} charMap;

  	int source;
  	
private:
	long lookupChar(long ch);
	
	void makeJFMCharMap(TFMFile *tfm);
	
	int sputBigEndian(char *s, long v, int n);
	
	void unpackOFMArrays(TFMFile *tfm, ulong num_chars);
	void unpackTFMArrays(TFMFile *tfm);
	bool unpackTFMHeader(TFMFile *tfm);
};

#endif // XWTFMFILE_H
