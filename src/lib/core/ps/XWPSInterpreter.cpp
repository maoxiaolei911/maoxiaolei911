/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <string.h>
#include "XWApplication.h"
#include "XWPSContextState.h"
#include "XWPSInterpreter.h"
    
XWPSInterpreter::XWPSInterpreter(QObject * parent)
	:QObject(parent)
{
	state = new XWPSContextState;
}


XWPSInterpreter::~XWPSInterpreter()
{
	if (state)
	{
		delete state;
		state = 0;
	}
}

void XWPSInterpreter::close()
{
	state->close();
}

void XWPSInterpreter::resetNextID()
{
	state->resetNextID();
}

void XWPSInterpreter::resetPageNextID(ulong pgno)
{
	state->resetPageNextID(pgno);
}

void XWPSInterpreter::saveNextID()
{
	state->saveNextID();
}

void XWPSInterpreter::savePageNextID(ulong pgno)
{
	state->savePageNextID(pgno);
}

int XWPSInterpreter::toPDF(XWDVICore * coreA,
	             XWDVIRef * xrefA,
	             XWDVIDev * devA,
	             const uchar * buf, 
	             uint len)
{
	return state->toPDF(coreA, xrefA, devA, buf, len);
}

QString XWPSInterpreter::toPDF(const QString & filename, 
	              double pw, 
	              double ph)
{
	return state->toPDF(filename, pw, ph);
}
