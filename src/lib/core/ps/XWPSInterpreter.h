/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSINTERPRETER_H
#define XWPSINTERPRETER_H

#include <QObject>
#include <QString>

#include "XWGlobal.h"
    
#define PS_RUNINIT   1
#define PS_RUNFLUSH  2
#define PS_RUNBUFFER 4

class XWDVIRef;
class XWDVICore;
class XWDVIDev;
class XWPSContextState;

class XW_PS_EXPORT XWPSInterpreter : public QObject
{
	Q_OBJECT 
	
public:
	XWPSInterpreter(QObject * parent = 0);
	~XWPSInterpreter();
	
	void close();
	
	void resetNextID();
	void resetPageNextID(ulong pgno);
	
	void saveNextID();
	void savePageNextID(ulong pgno);
	
	int toPDF(XWDVICore * coreA,
	             XWDVIRef * xrefA,
	             XWDVIDev * devA,
	             const uchar * buf, 
	             uint len);
	QString toPDF(const QString & filename, 
	              double pw, 
	              double ph);
	
private:
	XWPSContextState * state;
};

#endif //XWPSINTERPRETER_H
