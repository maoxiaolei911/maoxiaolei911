/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWCOREOUTPUTDEV_H
#define XWCOREOUTPUTDEV_H

#include "XWRasterType.h"
#include "XWRasterOutputDev.h"

class XWTextPage;


typedef void (*CoreOutRedrawCbk)(void *data, int x0, int y0, int x1, int y1,
				 bool composited);


class XW_CORE_EXPORT XWCoreOutputDev : public XWRasterOutputDev
{
public:
    XWCoreOutputDev(int colorModeA, 
                    int bitmapRowPadA,
		            bool reverseVideoA, 
		            quint8 *  paperColorA,
		            bool incrementalUpdateA,
		            CoreOutRedrawCbk redrawCbkA,
		            void *redrawCbkDataA);
    virtual ~XWCoreOutputDev() {}
    
    void clear();
    
    virtual void dump();
    
    virtual void endPage();
    
private:
    bool incrementalUpdate;
    CoreOutRedrawCbk redrawCbk;
    void *redrawCbkData;
};

#endif // XWCOREOUTPUTDEV_H

