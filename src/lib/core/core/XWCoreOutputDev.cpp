/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWObject.h"
#include "XWTextOutputDev.h"
#include "XWCoreOutputDev.h"

XWCoreOutputDev::XWCoreOutputDev(int colorModeA, 
                                int bitmapRowPadA,
		                        bool reverseVideoA, 
		                        quint8 *  paperColorA,
		                        bool incrementalUpdateA,
		                        CoreOutRedrawCbk redrawCbkA,
		                        void *redrawCbkDataA)
    :XWRasterOutputDev(colorModeA, bitmapRowPadA, reverseVideoA, paperColorA)
{
    incrementalUpdate = incrementalUpdateA;
    redrawCbk = redrawCbkA;
    redrawCbkData = redrawCbkDataA;
}

void XWCoreOutputDev::clear()
{
    startDoc(0);
    startPage(0, 0);
}

void XWCoreOutputDev::dump()
{
    if (incrementalUpdate) 
    {
        int x0, y0, x1, y1;
        getModRegion(&x0, &y0, &x1, &y1);
        clearModRegion();
        if (x1 >= x0 && y1 >= y0) 
        {
            (*redrawCbk)(redrawCbkData, x0, y0, x1, y1, false);
        }
    }
}

void XWCoreOutputDev::endPage()
{
    XWRasterOutputDev::endPage();
    if (!incrementalUpdate) 
    {
        (*redrawCbk)(redrawCbkData, 0, 0, getBitmapWidth(), getBitmapHeight(), true);
    }
}

