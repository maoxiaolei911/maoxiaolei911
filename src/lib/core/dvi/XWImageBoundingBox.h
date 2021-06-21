/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWIMAGEBOUNDINGBOX_H
#define XWIMAGEBOUNDINGBOX_H

#include <QString>
#include <QIODevice>
#include "XWApplication.h"

class XWImageBoundingBox : public XWApplication
{
	Q_OBJECT
    
public:
	XWImageBoundingBox(QCoreApplication * app);
	
	QString getProductID();
	
	QString getProductName();
	
	const char * getProductName8();
	
	QString getVersion();
	
	const char * getVersion8();
	
	void start(int & argc, char**argv);
		            
private:
	void doTime(QIODevice * fp);
	void doJPG(QIODevice * in, 
	           QIODevice * out,
	           const char * fname,
	           bool compat_mode);
	void doMPS(QIODevice * in, 
	           QIODevice * out,
	           const char * fname,
	           bool compat_mode);
	void doPDF(QIODevice * in, 
	           QIODevice * out,
	           const char * fname,
	           bool compat_mode);
	void doPNG(QIODevice * in, 
	           QIODevice * out,
	           const char * fname,
	           bool compat_mode);
	           
	void showHelp();
	void showMsg(const QString & msg);
	void showVersion();	
	           
	void writeXBb(QIODevice * fp, 
	              const char * fname,
	              double bbllx_f, 
	              double bblly_f,
		            double bburx_f, 
		            double bbury_f,
		            int pdf_version, 
		            long pagecount,
		            bool compat_mode = false);
	
};

#endif //XWIMAGEBOUNDINGBOX_H
