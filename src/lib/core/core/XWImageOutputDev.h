/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWIMAGEOUTPUTDEV_H
#define XWIMAGEOUTPUTDEV_H

#include <stdio.h>
#include "XWOutputDev.h"

class XWGraphixState;

class XW_CORE_EXPORT XWImageOutputDev : public XWOutputDev
{
public:
    XWImageOutputDev(const char *fileRootA, 
                     bool dumpJPEGA);
    virtual ~XWImageOutputDev();
    
    virtual void drawImage(XWGraphixState *, 
                           XWObject *, 
                           XWStream *str,
			 			   int width, 
			 			   int height, 
			 			   XWImageColorMap *colorMap,
			               int *, 
			               bool inlineImg);
    virtual void drawImageMask(XWGraphixState *, 
                               XWObject *, 
                               XWStream *str,
			     			   int width, 
			     			   int height, 
			     			   bool ,
			                   bool inlineImg);
			     
    virtual bool interpretType3Chars() { return false; }
    virtual bool isOk() { return ok; }
    
    virtual bool needNonText() { return true; }
    
    virtual void tilingPatternFill(XWGraphixState *state, 
                                   XWGraphix *gfx, 
                                   XWObject *str,
				 													 int paintType, 
				 													 XWDict *resDict,
				 													 double *mat, 
				 													 double *bbox,
				                           int x0, 
				                           int y0, 
				                           int x1, 
				                           int y1,
				 											     double xStep, 
				 											     double yStep);
    
    virtual bool upsideDown() { return true; }
    virtual bool useTilingPatternFill() { return true; }
    virtual bool useDrawChar() { return false; }
    
private:
	char *fileRoot;
	char *fileName;
	bool dumpJPEG;
	int imgNum;
	bool ok;
};

#endif // XWIMAGEOUTPUTDEV_H

