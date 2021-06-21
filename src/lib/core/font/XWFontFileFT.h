/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWFONTFILEFT_H
#define XWFONTFILEFT_H


#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TYPE1_TABLES_H
#include FT_TRUETYPE_TABLES_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_IMAGE_H
#include FT_BITMAP_H
#include FT_BBOX_H

#include <math.h>
#include <QObject>
#include <QString>
#include <QIODevice>
#include <QFile>
#include <QByteArray>
#include <QHash>

#include "XWGlobal.h"


struct _FTFontInfo;
typedef struct _FTFontInfo FTFontInfo;

struct _Lig;
typedef struct _Lig Lig;

struct _Kern;

typedef struct _Kern Kern;

struct _FTFontPtr;
typedef struct _FTFontPtr FTFontPtr;

struct _Pcc;
typedef struct _Pcc Pcc;

struct _StringList;
typedef struct _StringList StringList;


struct _FTFontInfo
{
	FTFontInfo * next;
	long charcode;
	ushort glyphindex;
	short incode;
	short outcode;
	char *adobename;
	short width;
	short llx, lly, urx, ury;
	
	Lig *ligs;
	bool rligs;
  	Kern *kerns;
  	bool rkerns;
  	FTFontPtr *kern_equivs;
  	bool rptrs;
  	bool constructed;
  	Pcc *pccs;
  	bool rpccs;
  	long wptr, hptr, dptr, iptr;
};

struct _Lig
{
  	Lig *next;
  	char *succ, *sub;
  	short op, boundleft;
};

struct _Kern
{
  	Kern *next;
  	char *succ;
  	short delta;
};

struct _FTFontPtr
{
  	_FTFontPtr *next;
  	FTFontInfo *ch;
};


struct _Pcc
{
  	Pcc *next;
  	char *partname;
  	short xoffset, yoffset;
};


struct _StringList
{
  	StringList* next;
  	char *old_name;
  	char *new_name;
  	bool single_replacement;
};

struct Encoding;

class XW_FONT_EXPORT XWFontFileFT : public QObject
{
	Q_OBJECT
	
public:
	friend class XWTFMCreator;
	
	enum EncodingScheme
	{
  		encUnicode,
  		encMac,
  		encFontSpecific
	};
	
	~XWFontFileFT();
	
	void assignChars();
	
	void codeToAdobeName(long code, char * buf);
	
	void doType3(int base_dpi,
                 int vDPI,
                 double point_size,
                 char * usedchars,
                 double *FontBBox,
                 double * Widths,
                 double *FontMatrix,
                 int    * FirstChar,
                 int    * LastChar);
	
	FTFontInfo * findAdobe(char *p);
	FTFontInfo * findGlyph(ushort g);
	FTFontInfo * findMappedAdobe(char *p, 
	                             FTFontInfo**array);
	
	const uchar * getCharProcStream(long c, 
	                                int base_dpi,
	                                long *lenA);
	
	int    getBC() {return bc;}
	short  getBoundaryChar() {return boundaryChar;}
	double getCapHeight() {return capHeight;}
	char * getCodingScheme();
	long   getDesignSizeFixed();
	int    getEC() {return ec;}
	double getEFactor() {return efactor;}
	char * getFamilyName();
	int    getFontDir() {return fontDir;}
	int    getFontLevel() {return fontLevel;}
	FTFontInfo * getInEncPtr(int i) {return inencptrs[i];}
	FTFontInfo ** getInEncPtrs() {return inencptrs;}
	char * getLigName() {return ligName;}
	FTFontInfo ** getLowerCase() {return lowercase;}
	short * getNextOut() {return nextout;}
	int getNumCmaps() {return face->num_charmaps;}
	int getNumGlyphs() {return face->num_glyphs;}
	FTFontInfo ** getOutEncPtrs() {return outencptrs;}
	double getSlant() {return slant;}
	long * getSubfontCode() {return sfCode;}
	void   getTFMParam(long * tparam, bool vf);
	FTFontInfo ** getUpperCase() { return uppercase;}
	
	void handleReencoding();
	bool hasGlyphNames() {return FT_HAS_GLYPH_NAMES(face);}
	bool hasHorizontal() {return FT_HAS_HORIZONTAL(face);}
	bool hasKerning() {return FT_HAS_KERNING(face);}
	bool hasMultipleMasters() {return FT_HAS_MULTIPLE_MASTERS(face);}
	bool hasPSGlyphNames() {return FT_Has_PS_Glyph_Names(face);}
	bool hasSubfontLigs() {return (subfontLigs && ligName != 0 && ligSubID != 0); }
	bool hasVertical() {return FT_HAS_VERTICAL(face);}
	
	bool isCIDKeyed() {return FT_IS_CID_KEYED(face);}
	bool isFixedWidth() {return FT_IS_FIXED_WIDTH(face);}
	bool isOnlyRange() {return onlyRange;}
	bool isScalable() {return FT_IS_SCALABLE(face);}
	bool isSFNT() {return FT_IS_SFNT(face);}
	bool isSmallCaps() {return smallcaps;}
	bool isTricky() {return FT_IS_TRICKY(face);}
	
	static XWFontFileFT * loadTexFont(const char * texnameA);
	static XWFontFileFT * loadType3(const char * texnameA, 
	                                int base_dpi,
	                                int vDPI,
	                                double point_size,
	                                char * usedchars,
	                                double *FontBBox,
	                                double * Widths,
	                                double *FontMatrix,
	                                int    * FirstChar,
	                                int    * LastChar);
	
	FTFontInfo * newChar();
	Kern * newKern();
	Lig  * newLig();
	Pcc  * newPcc();
	StringList * newStringList();
	
	Encoding * readEncoding(const QString & enc, bool ignoreligkern);
	void readFT();
	void readSubfontLigs();
	void releaseFTFontPtr(FTFontPtr * fprt);
	
	long scale(long what) 
		{ return ((what / 1000) * 0x100000) + (((what % 1000) * 0x100000) + 500) / 1000;}
	void setBold(bool boldA) {bold = boldA;}
	void setBoldf(double v) {boldf = v;}
	bool setCharMap(int pidA, int eidA);
	void setCharSize(double point_size, int hDPI, int vDPI);
	void setEFactor(double efactorA) {efactor = efactorA;}
	void setInEncName(const QString & inencnameA) {inEncName = inencnameA;}
	void setOutEncName(const QString & outencnameA) {outEncName = outencnameA;}
	void setResolution(int hDPI, int vDPI);
	void setRotate(bool rotateA) {rotate = rotateA;}
	void setSlant(double slantA) {slant = slantA;}
	void setSmallCaps(bool smallcapsA) {smallcaps = smallcapsA;}
	void setSubFont(const QString & subfont, 
	                const QString & subfontid);
	void setSubFontLig(const QString & subfont, 
	                   const QString & subfontid);
	void setSubFontLigs(bool e) {subfontLigs = e;}
	void setYOfsset(double y_offsetA) {y_offset = y_offsetA;}
	
	int transform(int x, int y, float ef, float sl)
	{
		double acc = ef * x + sl * y;
  		return (int)(acc >= 0 ? floor(acc + 0.5) : ceil(acc - 0.5));
	}
	
	void upMap();
		
protected:
	XWFontFileFT(FT_Face faceA);
	
	void addKern(char *s1, char *s2);
	void addToStream(uchar *rowptr, long rowbytes);
	
	void checkLigKern(char *s);
	
	void getLigKernDefaults();
	
	char * getToken(char **bufferp, 
	                ulong *offsetp, 
	                QIODevice *f,
         			bool ignoreligkern, 
         			bool init);
	
	char * paramString(char **curp);
	
	void readKern();	
	void releaseChar(FTFontInfo * ti);
	void releaseKern(Kern * kern);
	void releaseLig(Lig * lig);
	void releasePcc(Pcc * pcc);
	void rmKern(char *s1, 
	            char *s2,
                FTFontInfo *ti);
	Kern * rmKernMatch(Kern *k, char *s);
	
	//	void replaceGlyphs();
	
protected:
  	FT_Face    face;
  	int pid;
  	int eid;
  	
  	FTFontInfo ** inencptrs;
	
	FTFontInfo * charList;
	
	char * codingScheme;
	
	EncodingScheme currentEncodingScheme;
	
  	long   sfCode[256];
  	
  	short unitsPerEm;
  	float italicAngle;
  	char  fixedPitch;
  	int   fontDir;
  	double capHeight;
  	
  	short xheight;
  	short fontSpace;
  	
  	double size;
  	double efactor;
  	double slant;
  	double boldf;
  	bool bold;
  	bool rotate;
  	double y_offset;
  	bool smallcaps;
  	
  	bool onlyRange;
  	bool subfontLigs;
  	char * ligName;
  	char * ligSubID;
  	
  	QString inEncName;
    QString outEncName;
    
  	Encoding * inencoding;
  	Encoding * outencoding; 
  	FTFontInfo  ** outencptrs; 
  	short * nextout;
  	FTFontInfo ** uppercase;
  	FTFontInfo ** lowercase;
  	
  	StringList * replacements;
	char * replacementName;
	short boundaryChar;
	
	bool sawLigKern;
	
	int bc;
	int ec;
	int fontLevel;
	
	long    curChar;
	uchar * stream;
	long    streamLength;
	
	FT_Matrix matrix1;
    FT_Matrix matrix2;
  	
  	QHash<int, QString> uniCodeToNameHash;  	
  	QHash<int, QString> macCodeToNameHash;
};

#endif // XWFONTFILEFT_H

