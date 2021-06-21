/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWDVIDOC_H
#define XWDVIDOC_H

#include <QIODevice>
#include <QString>
#include <QStringList>
#include <QList>
#include "XWDoc.h"
#include "XWDVIType.h"

class XWDVICore;
class XWString;

class XW_DVI_EXPORT XWDVIDoc : public XWDoc
{
    Q_OBJECT
    
public:
    XWDVIDoc(QObject * parent = 0);
    ~XWDVIDoc();
    
    void close(bool finished = true);
    
    void displayPage(XWOutputDev *out, 
                     int page,
		             double hDPI, 
		             double vDPI, 
		             int rotate,
		             bool useMediaBox, 
		             bool crop, 
		             bool printing,
		             bool (*abortCheckCbk)(void *data) = 0,
		             void *abortCheckCbkData = 0);
	void displayPage(int page);
	void displayPageSlice(XWOutputDev *out, 
	                      int page,
			              double hDPI, 
			              double vDPI, 
			              int rotate,
			              bool useMediaBox, 
			              bool crop, 
			              bool printing,
			              int sliceX, 
			              int sliceY, 
			              int sliceW, 
			              int sliceH,
			              bool (*abortCheckCbk)(void *data) = 0,
			              void *abortCheckCbkData = 0);
        
    static XWDVIDoc * findDoc(const QString & filename, int pg);
    static XWDVIDoc * findDoc(const QString & filename, XWString * namedDest);
    
    DVIFontDef * getFontDefs();
    int          getNumberOfDefFont();
    
    bool load(const QString & filename, QIODevice * outfileA = 0);
    bool load(QIODevice * fileA, int mpmode, QIODevice * outfileA = 0);
	void loadFontMapFile(const char  *filename);
	
	void saveToPDF(QIODevice * outfileA,
	               char *dviname, 
	               char *pdfname,
	               const QString & thumbbasename,
	               const QList<int> & pages,
	               char * owner_pw,
	               char * user_pw);

	void scanSpecials(int page_no, 
										double *page_width, 
										double *page_height,
                   	double *x_offset, 
                   	double *y_offset, 
                   	char   *landscape,
	                  unsigned *minorversion,
		                int *do_enc, 
		                unsigned *key_bits, 
		                unsigned *permission, 
		                char *owner_pw, 
		                char *user_pw);
	void setAnnotGrow(int annot_grow);
	void setBookMarkOpen(int i);
	void setCIDFontFixedPitch(bool e);
	void setCompressionLevel(int level);
	void setDecimalDigits(int i);
	void setDPI(int hDPIA, int vDPIA);
	void setEncryption(bool e);
	void setFontMapFirstMatch(bool e);
	void setIgnoreColors(int i);
	void setMag(double m);
	void setNoDestRemove(bool e);
	void setOffset(double hoff, double voff);
	void setPaperSize(double w, double h);
	void setPDFVersion(int v);
	void setThumbBaseName(const QString & thumbbasename);
  void setTPICTransFill(bool e);
    
protected:
	
		void doBop();
    void doDir();
    void doDown1();
    void doDown2();
    void doDown3();
    void doDown4();
    void doFntDef();
    void doFnt1();
    void doFnt2();
    void doFnt3();
    void doFnt4();
    void doFntDef1();
    void doFntDef2();
    void doFntDef3();
    void doFntDef4();
    static void doNativeFontDef(QIODevice * fileA, int scanning);
    void doPage(int page_no);
    void doPut1();
    void doPut2();
    void doPut3();
    void doPut4();
    void doPutRule();
    void doRight1();
    void doRight2();
    void doRight3();
    void doRight4();
    void doSet1();
    void doSet2();
    void doSet3();
    void doSet4();
    void doSetRule();
    void doW1();
    void doW2();
    void doW3();
    void doW4();
    void doX1();
    void doX2();
    void doX3();
    void doX4();
    void doXXX(ulong size);
    void doXXX1();
    void doXXX2();
    void doXXX3();
    void doXXX4();
    void doY1();
    void doY2();
    void doY3();
    void doY4();
    void doZ1();
    void doZ2();
    void doZ3();
    void doZ4();
    
    long findPost();
    
    bool getDVIFonts(long post_location);
    bool getDVIInfo(long post_location);
    bool getPageInfo(long post_location);
    
    static bool loadPage(QIODevice * in, 
                         QIODevice * out, 
                         int pg, 
                         XWString *namedDest);
    
    bool readFontRecord(long tex_id);
    bool readNativeFontRecord(long tex_id);
    static QIODevice * readFromX(QIODevice * fileA);
    
protected:
	XWDVICore * core;
	ulong * pageLoc;
    long    numPages;
   int processing_page;
   char linear;
};

#endif //XWDVIDOC_H

