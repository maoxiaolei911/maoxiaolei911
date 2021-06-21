/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSTRANSFERMAP_H
#define XWPSTRANSFERMAP_H

#include "XWPSType.h"

class XWPSTransferMap;

typedef float (XWPSTransferMap::*mapping_proc)(float);

class XWPSTransferMap : public XWPSStruct
{
public:
	XWPSTransferMap();
	XWPSTransferMap(XWPSTransferMap * other);
	
	virtual int getLength();
	virtual const char * getTypeName();
	
	float identityTransfer(float value) {return value;}
	bool  isIdentity();
	
	void loadTransferMap(float min_value);
	
	short mapColorFrac(short cf);
	float mappedTransfer(float value);
	short mapping(short cf);
	
	float nullTransfer(float value) {return value;}
	
	void setIdentityTransfer();
	void setType(PSTransferMapType t);
	
	XWPSTransferMap & operator=(const XWPSTransferMap & other);
	
private:	
	static short colorFracMap(short cv, const short * valuesA);	
	
public:
	long id;
	short values[transfer_map_size];
	mapping_proc proc;
	PSTransferMapType type;
};

#endif //XWPSTRANSFERMAP_H
