/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stddef.h>
#include "XWObject.h"
#include "XWStream.h"
#include "XWGraphixState.h"
#include "XWOutputDev.h"

XWOutputDev::XWOutputDev()
{
}

XWOutputDev::~XWOutputDev()
{
}

bool XWOutputDev::beginType3Char(XWGraphixState *, 
                                 double , 
                                 double ,
			                     double , 
			                     double ,
			                     uint , 
			                     uint *, int)
{
    return false;
}

void XWOutputDev::cvtDevToUser(double dx, double dy, double *ux, double *uy)
{
    *ux = defICTM[0] * dx + defICTM[2] * dy + defICTM[4];
    *uy = defICTM[1] * dx + defICTM[3] * dy + defICTM[5];
}

void XWOutputDev::cvtUserToDev(double ux, double uy, int *dx, int *dy)
{
    *dx = (int)(defCTM[0] * ux + defCTM[2] * uy + defCTM[4] + 0.5);
    *dy = (int)(defCTM[1] * ux + defCTM[3] * uy + defCTM[5] + 0.5);
}

void XWOutputDev::drawImage(XWGraphixState *, 
                            XWObject *, 
                            XWStream *str,
			                int width, 
			                int height, 
			                XWImageColorMap *colorMap,
			                int *, 
			                bool inlineImg)
{
    if (inlineImg) 
    {
        str->reset();
        int j = height * ((width * colorMap->getNumPixelComps() * colorMap->getBits() + 7) / 8);
        for (int i = 0; i < j; ++i)
            str->getChar();
        str->close();
    }
}

void XWOutputDev::drawImageMask(XWGraphixState *, 
                                XWObject *, 
                                XWStream *str,
			                    int width, 
			                    int height, 
			                    bool ,
			                    bool inlineImg)
{
    if (inlineImg) 
    {
        str->reset();
        int j = height * ((width + 7) / 8);
        for (int i = 0; i < j; ++i)
            str->getChar();
        str->close();
    }
}

void XWOutputDev::drawMaskedImage(XWGraphixState *state, 
                                 XWObject *ref, 
                                 XWStream *str,
			                     int width, 
			                     int height,
			                     XWImageColorMap *colorMap,
			                     XWStream *, 
			                     int , 
			                     int ,
			                     bool )
{
    drawImage(state, ref, str, width, height, colorMap, 0, false);
}

void XWOutputDev::drawSoftMaskedImage(XWGraphixState *state, 
                                     XWObject *ref, 
                                     XWStream *str,
				                     int width, 
				                     int height,
				                     XWImageColorMap *colorMap,
				                     XWStream *,
				                     int , 
				                     int ,
				                     XWImageColorMap *)
{
    drawImage(state, ref, str, width, height, colorMap, 0, false);
}

void XWOutputDev::opiBegin(XWGraphixState *, XWDict *)
{
}

void XWOutputDev::opiEnd(XWGraphixState *, XWDict *)
{
}

void XWOutputDev::setDefaultCTM(double *ctm)
{
    for (int i = 0; i < 6; ++i) 
        defCTM[i] = ctm[i];
        
    double det = 1 / (defCTM[0] * defCTM[3] - defCTM[1] * defCTM[2]);
    defICTM[0] = defCTM[3] * det;
    defICTM[1] = -defCTM[1] * det;
    defICTM[2] = -defCTM[2] * det;
    defICTM[3] = defCTM[0] * det;
    defICTM[4] = (defCTM[2] * defCTM[5] - defCTM[3] * defCTM[4]) * det;
    defICTM[5] = (defCTM[1] * defCTM[4] - defCTM[0] * defCTM[5]) * det;
}

void XWOutputDev::setSoftMaskFromImageMask(XWGraphixState *state,
																					 XWObject *ref, 
																					XWStream *str,
					                                int width, 
					                                int height, 
					                                bool invert,
					                                bool inlineImg)
{
	drawImageMask(state, ref, str, width, height, invert, inlineImg);
}

void XWOutputDev::updateAll(XWGraphixState *state)
{
  updateLineDash(state);
  updateFlatness(state);
  updateLineJoin(state);
  updateLineCap(state);
  updateMiterLimit(state);
  updateLineWidth(state);
  updateStrokeAdjust(state);
  updateFillColorSpace(state);
  updateFillColor(state);
  updateStrokeColorSpace(state);
  updateStrokeColor(state);
  updateBlendMode(state);
  updateFillOpacity(state);
  updateStrokeOpacity(state);
  updateFillOverprint(state);
  updateStrokeOverprint(state);
  updateOverprintMode(state);
  updateTransfer(state);
  updateFont(state);
}
