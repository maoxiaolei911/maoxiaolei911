/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWDVICOLORSPACES_H
#define XWDVICOLORSPACES_H

#include <QObject>

class XWObject;
class XWDVIDev;
class XWDVIRef;

class DVIColorSpace;

class XWDVIColorSpaces : public QObject
{
	Q_OBJECT
	
public:
	XWDVIColorSpaces(QObject * parent = 0);
	~XWDVIColorSpaces();
	
	void close(XWDVIRef * xref, bool finished = true);
	
	XWObject * getColorSpaceReference(XWDVIRef * xref, int cspc_id, XWObject * ref);
	
	static int iccpCheckColorSpace(int colortype, 
	                               const void *profile, 
	                               long proflen);
	static XWObject * iccpGetRenderingIntent(const void *profile, 
	                                         long proflen,
	                                         XWObject * obj);
	                                         
	int iccpLoadProfile(XWDVIRef * xref,
	                    XWDVIDev * devA, 
	                    const char *ident,
		                const void *profile, 
		                long proflen);
	
private:
	int defineResource(const char *ident, 
	                   int subtype, 
	                   void *cdata, 
	                   XWObject *resource);
	                   
	int findResource(const char *ident, int type, const void *cdata);
			     
private:
	int  count;
  	int  capacity;
  	DVIColorSpace ** colorSpaces;
};

#endif // XWDVICOLORSPACES_H

