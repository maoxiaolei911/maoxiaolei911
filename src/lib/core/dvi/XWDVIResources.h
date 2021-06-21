/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWDVIRESOURCES_H
#define XWDVIRESOURCES_H

#include <QObject>
#include "XWCMap.h"

#define PDF_RES_FLUSH_IMMEDIATE 1

class XWFontFileSFNT;
class XWFontFileCFF;
struct SFNTCmap4;
struct SFNTCmap12;
class XWSFNTCmap;
class XWOTLGsub;
class XWObject;
class XWDVIRef;
class XWDVICore;
class DVIResource;

class XWDVIResources : public QObject
{
	Q_OBJECT
	
public:
	XWDVIResources(QObject * parent = 0);
	~XWDVIResources();
	
	void close(XWDVIRef * xref, bool finished = true);
	
	XWObject * createCMapStream(XWDVIRef * xref,
	                            XWCMap *cmap, 
	                            int ,
	                            XWObject * obj);
	
	long defineResource(XWDVIRef * xref,
	                    const char *category, 
		    			const char *resname, 
		    			XWObject *object, 
		    			int flags);
		    			
	long findResource(const char *category, const char *resname);
	
	XWObject * getResourceReference(XWDVIRef * xref, long rc_id, XWObject * ref);
	
	XWObject * loadToUnicodeStream(XWDVIRef * xref,
	                               const char *ident,
	                               XWObject * obj);
	
	XWObject * readToUnicodeFile(XWDVIRef * xref, 
	                             const char *cmap_name, 
	                             XWObject * obj);
	                             
	XWObject * otfCreateToUnicodeStream(XWDVIRef * xref,
	                                    const char *font_name,
			     					    int ttc_index,
			                            const char *used_glyphs,
			                            XWObject * obj);
	int otfLoadUnicodeCMap(XWDVICore * core,
	                       XWDVIRef * xref,
	                       const char *map_name, 
	                       int ttc_index,
		                   const char *otl_tags, 
		                   int wmode);
	
private:
	XWObject * createDummyCMap(XWDVIRef * xref, XWObject * obj);
	XWObject * createToUnicodeCMap4(XWDVIRef * xref,
	                                SFNTCmap4 *map,
									const char *cmap_name, 
									XWCMap *cmap_add,
									const char *used_glyphs,
									XWObject * obj);
	XWObject * createToUnicodeCMap12(XWDVIRef * xref,
	                                 SFNTCmap12 * map,
			 						 const char *cmap_name, 
			 						 XWCMap *cmap_add,
			 						 const char *used_glyphs,
			 						 XWObject * obj);
	
	int handleCIDFont(XWFontFileSFNT *sfont,
					  uchar **GIDToCIDMap, 
					  CIDSysInfo *csi);
	ushort handleSubstGlyphs(XWCMap *cmap, 
	                         XWCMap *cmap_add, 
	                         const char *used_glyphs);
	                         
	int loadBaseCMap(const char *cmap_name, 
	                 int wmode,
		             CIDSysInfo *csi, 
		             uchar *GIDToCIDMap,
					 XWSFNTCmap *ttcmap);
	int loadGsub(XWObject *conf, 
	             XWOTLGsub *gsub_list, 
	             XWFontFileSFNT *sfont);
	
private:
	struct ResourceCache
	{
		int           count;
  		int           capacity;
  		DVIResource ** resources;
	};
	
	ResourceCache * resources;
};

#endif // XWDVIRESOURCES_H

