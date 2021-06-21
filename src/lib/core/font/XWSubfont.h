/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWSUBFONT_H
#define XWSUBFONT_H

#include <QObject>
#include <QString>
#include <QFile>
#include "XWGlobal.h"

class XWSubfontFile;

class XW_FONT_EXPORT XWSubfont : public QObject
{
	Q_OBJECT
	
public:
	XWSubfont(QObject * parent = 0);
	XWSubfont(int id, QObject * parent = 0);
    XWSubfont(const QString & sfd_name, QObject * parent = 0);
    ~XWSubfont() {}
    
    void getCode(long * sf_code);
    
    QString getCurSubID();	
    int getFileID() {return fileID;}
    QString getFirstSubID();
	QString getNextSubID();    
    char ** getSubfontIds(int *num_ids);
    
    bool isOK() {return fileID != -1;}
    
    int  loadRecord(const QString & subfont_id);
    long lookupRecord(int rec_id, uchar c);
    
    static void releaseSFD();
    
    bool setSFDName(const QString & sfd_name);
    
private:
	int findFile(const QString & sfd_name);
	
private:
	int fileID;
	int recID;
	
	static XWSubfontFile ** files;
	static int   numFiles;
	static int   maxFiles;
};

#endif // XWSUBFONT_H

