/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWDVIPSSPECIAL_H
#define XWDVIPSSPECIAL_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QSettings>

#include "XWGlobal.h"
#include "XWPathSea.h"
#include "XWDVISpecial.h"

class XWDviPsSea : public QObject
{
	Q_OBJECT
    
public:
	enum FileFormat
	{
		EPS = 0,
		PS,
		PRO,
		Last
	};
	
	XWDviPsSea(QObject * parent = 0);
	
	QString findFile(const QString & name);
	QString findFile(const QString & name,
	                 FileFormat format,
                     bool must_exist);
	FileFormat findFormat(const QString & name);
        
    QFile * openFile(const QString & filename);
    QFile * openFile(const QString & filename, FileFormat format);
};

class XWDviPsSeaPrivate : public XWPathSea
{
    Q_OBJECT

public:
    XWDviPsSeaPrivate(QObject * parent = 0);

    QString initFormat(int format);
};


class XWDviPsSpecial : public XWDVISpecial
{
	Q_OBJECT
	
public:	
	XWDviPsSpecial(XWDVICore * coreA,
	            XWDVIRef * xrefA,
	            XWDVIDev * devA,
	            const char *buffer, 
	            long sizeA,
		        double x_userA, 
		        double y_userA, 
		        double magA,
		        int typeA,
		        QObject * parent = 0);
	
	static int check(XWLexer * lexerA); 
		
	bool exec(int hDPIA, int vDPIA);
	
protected:
	int copyFile(const char * filename);
	
	int doHeader();
	int doLiteral();
	int doPlotFile();
	int doPS();
	int doSpecial();
	
	char * parseFileName();
};

#endif //XWDVIPSSPECIAL_H