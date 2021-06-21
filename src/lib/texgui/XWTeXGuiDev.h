/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTEXGUIDEV_H
#define XWTEXGUIDEV_H

#include <QByteArray>
#include <QRectF>
#include <QPointF>
#include <QHash>
#include <QList>
#include <QStack>

#include "XWDVIType.h"
#include "XWTeXIODev.h"
#include "XWTeXGuiType.h"

class QBuffer;
class XWDVIRef;
class XWDVICore;
class XWDoc;

class XW_TEXGUI_EXPORT XWTeXGuiDev : public XWTeXIODev
{
	Q_OBJECT
	
public:
	XWTeXGuiDev(XWDoc * docA,
	            QObject * parent = 0);
	virtual ~XWTeXGuiDev();
	
	bool atEnd();
	
	void clearPages();
	virtual void compile(); 
		
	void doPage(int page_no);
	
	void endInput();
	
	QByteArray      getByteArray(const QString & txt);
	TeXCategoryCode getCategory();
	qint32          getChar();
	qint32          getControlSequence(qint32 * buf);
	QString         getCoord();
	TeXFileMode     getFileMode();	
	DVIFontDef *    getFontDefs();
	QString         getKey();
	int             getNumberOfDefFont();
	QString         getOption();
	QString         getParam();
	qint32          getPos();
	qint32          getSize();
	QString         getString(qint32 filepos, qint32 len);
	QString         getText();
	QString         getValue();
	QString         getWord();
			
	qint32 insert(qint32 filepos, 
	              const QByteArray & txt);
	qint32 insert(qint32 filepos, 
	              const QString & txt);
	qint32 insertFile(qint32 filepos, const QString & filename);
	
	bool loadFile(const QString & filename);
	bool loadFmt(const QString & fmtname);
	                               	
	QByteArray read(qint32 filepos, qint32 len);
	QString    readLine();
	void   remove(qint32 filepos, 
	              qint32 len);
	qint32 removeFile(qint32 filepos, const QString & filename);
	qint32 replace(qint32 filepos, 
	               qint32 len, 
	               const QByteArray & txt);
	qint32 replace(qint32 filepos, 
	               qint32 len, 
	               const QString & txt);
	
	void save();
	void save(const QString & filename);
	void saveToDVI(QIODevice * outfileA);
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
	bool seek(qint32 filepos);
	void setCIDFontFixedPitch(bool e);
	void setDecimalDigits(int i);
	void setDPI(int hDPIA, int vDPIA);
	void setFontMapFirstMatch(bool e);
	void setIgnoreColors(int i);
	void setMag(double m);
	void setOffset(double hoff, double voff);
	void setPaperSize(double w, double h);
  void setTPICTransFill(bool e);
	void skipComment();
	void skipControlSequence();
	void skipCoord();
	void skipFormula();
	void skipGroup();
	void skipOption();
	void skipSpacer();
	void skipSpacerAndComment();
	void skipWord();
	
protected:
			void clear();
			
			void doBop();
    	void doDir();
    	void doDown(long y);
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
    	void doNativeFontDef(int scanningA);
    	void doPop();
    	void doPush();
    	void doPut(long ch);
    	void doPut1();
    	void doPut2();
    	void doPut3();
    	void doPut4();
    	void doPutRule();
    	void doRight(long x);
    	void doRight1();
    	void doRight2();
    	void doRight3();
    	void doRight4();
    	void doSet(long ch);
    	void doSet1();
    	void doSet2();
    	void doSet3();
    	void doSet4();
    	void doSetRule();
    	void doString(const uchar *s, int len);
    	void doW(long ch);
    	void doW0();
    	void doW1();
    	void doW2();
    	void doW3();
    	void doW4();
    	void doX(long ch);
    	void doX0();
    	void doX1();
    	void doX2();
    	void doX3();
    	void doX4();
    	void doXXX(ulong size);
    	void doXXX1();
    	void doXXX2();
    	void doXXX3();
    	void doXXX4();
    	void doY(long ch);
    	void doY0();
    	void doY1();
    	void doY2();
    	void doY3();
    	void doY4();
    	void doZ(long ch);
    	void doZ0();
    	void doZ1();
    	void doZ2();
    	void doZ3();
    	void doZ4();
	
	long  findPost();
	
	bool    getDVIFonts(long post_location);
	bool    getDVIInfo(long post_location);	
	bool    getPageInfo(long post_location);
	
	virtual QIODevice * openInput(const QString & filename);
		
	bool readFontRecord(long tex_id);
	bool readNativeFontRecord(long tex_id);
	
	virtual void run();
				                    
protected slots:	
	virtual void updateFilePos(qint32 filepos, qint32 l);
		
protected:
	XWDoc * doc;
	
	XWDVIRef * xref;
	XWDVICore * dviCore;
    
	ulong * pageLoc;
  long    numPages;
		
	XWTeXFile * mainFile;
	
	XWTeXFile * curFile;
	QStack<XWTeXFile*>  fileStack;
};

#endif //XWTEXGUIDEV_H

