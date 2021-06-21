/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWCHARACTERWIDGET_H
#define XWCHARACTERWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QDialog>
#include <QString>
#include <QList>
#include "XWDVIType.h"

class QPainter;
class VFFont;
class XWFontFileID;
class XWFTFontFile;
class XWFTFontEngine;
class XWFTFont;
class XWFontFilePK;

class XWCharacterWidget : public QWidget
{
	Q_OBJECT

public:
	enum DevFontType
    {
    	FontFT,
    	FontPK,
    	FontVF
    };
    
	XWCharacterWidget(QWidget * parent = 0);	
	~XWCharacterWidget();
	
	void displayFTFont(const QString & name, 
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
	                   int subfontid = -1);
	void displayFTFont(const QString & name, 
	                   int index, 
	                   int map,
	                   double size,
	                   double efactorA,
	                   double slantA,
	                   double boldfA,
	                   bool boldA,
	                   bool rotateA,
	                   const QString & encname,
	                   int subfontid = -1);
	                   
	void displayPKFont(const QString & name);
	void displayVFFont(const QString & name, const QString & tfmname);
	
	void drawChar(int ch, 
	              QPainter * painter, 
	              int x, 
	              int y);
	
	XWFTFont * getFTFont();
	int        getNumGlyphs() const;
	double getSize() {return fontSize;}
	QString getType() {return typeText;}
	
	int locateFont(const char *tfm_name, 
	               long ptsize);
	               
	void setFont(int font_id) {currentFont = font_id;}
	QSize sizeHint() const;
	
protected:
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void paintEvent(QPaintEvent *event);

private:
	void displayFTFont(const QString & name, 
	                   int index, 
	                   double size,
	                   double efactorA,
	                   double slantA,
	                   double boldfA,
	                   bool boldA,
	                   bool rotateA,
	                   const QString & encname,
	                   int subfontid = -1);
	                   
	int  findFTFont(const char * name, 
	                int index,
	                double size,
	                double efactorA,
	                double slantA,
	                double boldfA,
	                bool boldA,
	                bool rotateA,
	                char * encname,
	                int subfontid = -1);
	int  findPKFont(const char * name,
	                int base_dpi,
	                double font_scale);
	int  findVFFont(const char *name, 
	                long ptsize,
	                int  tmfid);
	                
	void needMoreFonts(int n);
		
private:
	ulong unitNum;
    ulong unitDen;
    double dvi2pts;
    
	DVILoadedFont * loadedFonts;
    int numLoadedFonts;
    int maxLoadedFonts;
    int currentFont;
    
    struct DevFont
    {
    	DevFontType type;
    	long   ptsize;
    	int    tfmID;
    	int    subfontID;
    	char * name;
    	void * font;
    	int    index;
    };
    
    DevFont * devFonts;
    int numDevFonts;
    int maxDevFonts;
    int devID;
    double fontSize;
    QString typeText;
    
    XWFTFontEngine * fontEngine;
    
	int columns;
	int squareSize;
	int lastKey;
};

#endif // XWCHARACTERWIDGET_H
