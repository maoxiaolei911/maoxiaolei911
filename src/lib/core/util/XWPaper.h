/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPAPER_H
#define XWPAPER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QPrinter>

#include "XWGlobal.h"

class XW_UTIL_EXPORT XWPaper : public QObject
{
    Q_OBJECT
    
public:
	//A4
    XWPaper(QObject * parent = 0);
    //specA为"AX、BX、C5E、Comm10E、DLE、Executive、Folio、Ledger、Legal、Letter、Tabloid"
    XWPaper(const QString & specA, QObject * parent = 0);
    XWPaper(qreal w, qreal h, QObject * parent = 0);
    ~XWPaper() {}
    
	QPrinter::Orientation getOrientation() {return orientation;}
		
	QString getSpec() {return spec;}
	static QStringList getPaperSpecs();

    qreal pixelHeight(int resolutionY) {return height * resolutionY / 72;}
    qreal pixelWidth(int resolutionX) {return width * resolutionX / 72;}
    qreal pixelXOffset(int resolutionX) {return xOffset * resolutionX / 72;}
    qreal pixelYOffset(int resolutionY) {return yOffset * resolutionY / 72;}
    qreal ptHeight() {return height;}
    qreal ptWidth() {return width;}
    qreal ptXOffset() {return xOffset;}
    qreal ptYOffset() {return yOffset;}
        
    void setOrientation(QPrinter::Orientation o) {orientation = o;}
    void setPaperSize(const char * specA);
    void setPaperSize(const QString & specA);
    void setPaperSize(qreal w, qreal h);
    void setXOffset(qreal xoff) {xOffset = xoff;}
    void setYOffset(qreal yoff) {yOffset = yoff;}
    
	QPrinter::PageSize size() {return format;}
        
private:
	void findOrientation();
	void findSize();
	void findSpec();
	
private:
    qreal width;
    qreal height;
    qreal xOffset;
    qreal yOffset;
    
    QPrinter::PageSize format;
    QPrinter::Orientation orientation;
    	
    QString spec;
};


#endif // XWPAPER_H
