/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPDFFILE_H
#define XWPDFFILE_H

#include <QString>
#include <QHash>
#include <QIODevice>
#include "XWGlobal.h"

class XWObject;
class XWDVIRef;

class XW_DVI_EXPORT XWPDFFile
{
public:
	static bool checkForPDF(QIODevice * fileA);
	static int checkVersion(QIODevice * fileA);
	
	bool checkLabel(long n, int g)
	{
		return (n > 0 && n < num_obj && 
		       ((xrefTable[n].type == 1 && xrefTable[n].gen == g) || 
		       (xrefTable[n].type == 2 && !g)));
	}
	
	void close();
	static void closeAll();
	
	static XWObject * derefObj(XWDVIRef * xrefA, XWObject *obj, XWObject * ref);
	
	XWObject * getCatalog() {return &catalog;}	
	XWObject * getPage(long *page_p, 
	                   long *count_p, 
	                   PDFRect *bbox, 
	                   XWObject *resources_p,
	                   XWObject * obj);
	int getVersion() {return version;}
	                     
	static XWObject * importObject(XWDVIRef * xrefA, 
	                               XWObject *object, 
	                               XWObject * ref);
		
	static XWPDFFile * open(XWDVIRef * xrefA, 
	                        const char * ident, 
	                        QIODevice * fileA);
	                     
private:
	XWPDFFile(XWDVIRef * xrefA, 
	          QIODevice * fileA, 
	          int versionA);
	~XWPDFFile();
	          
	int backupLine();
	
	void extendXRef(ulong new_size);

	long findXRef();
	
	XWObject * getObject(int obj_num, 
	                     int obj_gen, 
	                     XWObject * obj);
	                     
	static XWObject * importIndirect(XWDVIRef  * xrefA,
	                                 XWObject * object, 
	                                 XWObject *ref);
	
	int nextObjectOffset(int obj_num);
	
	XWObject * parseTrailer(XWObject * obj);
	ulong parseXRefStmField(char **p, 
	                        int length, 
	                        ulong def);
	int parseXRefStmSubsec(char **p, 
	                       long *length,
		                   int *W, 
		                   int wsum,
		                   long first, 
		                   long size);
	int parseXRefStream(long xref_pos, XWObject *trailerA);
	int parseXRefTable(long xref_pos);
	
	XWObject * readObject(int obj_num, 
	                      int obj_gen,
						  int offset, 
						  int limit,
						  XWObject * obj);
	bool readObjStm(int num);
	XWObject * readXRef();
	
	void setFile(QIODevice * fileA) {file = fileA;}
	void setXRef(XWDVIRef  * xrefA) {xref = xrefA;}
	
private:
	XWDVIRef  * xref;
	QIODevice * file;
	int         version;
	DVIRefEntry * xrefTable;
	long          num_obj;
    long ** objstmData;
    
	XWObject      trailer;
  	XWObject      catalog;
  	
  	static QHash<QString, XWPDFFile*> pdfFiles;
};

#endif //XWPDFFILE_H
