/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include <QTextCodec>
#include <QRegExp>
#include <QFont>
#include <QFontDatabase>
#include "XWFontSetting.h"
#include "XWPage.h"
#include "XWGraphix.h"
#include "XWCoreFont.h"
#include "XWLink.h"
#include "XWPreScanOutputDev.h"

XWPreScanOutputDev::XWPreScanOutputDev()
{
	clearStats();
}

bool XWPreScanOutputDev::axialShadedFill(XWGraphixState *state, XWAxialShading *shading)
{
	if (shading->getColorSpace()->getMode() != COLOR_SPACE_DEVICEGRAY &&
      shading->getColorSpace()->getMode() != COLOR_SPACE_CALGRAY) {
    gray = false;
  }
  mono = false;
  if (state->getFillOpacity() != 1 ||
      state->getBlendMode() != BLEND_NORMAL) {
    transparency = true;
  }
  return true;
}

void XWPreScanOutputDev::beginStringOp(XWGraphixState *state)
{
	int render;
  XWCoreFont *font;
  double m11, m12, m21, m22;
  bool simpleTTF;

  render = state->getRender();
  if (!(render & 1)) {
    check(state->getFillColorSpace(), state->getFillColor(),
	  state->getFillOpacity(), state->getBlendMode());
  }
  if ((render & 3) == 1 || (render & 3) == 2) {
    check(state->getStrokeColorSpace(), state->getStrokeColor(),
	  state->getStrokeOpacity(), state->getBlendMode());
  }

  font = state->getFont();
  state->getFontTransMat(&m11, &m12, &m21, &m22);
  //~ this should check for external fonts that are non-TrueType
  simpleTTF = fabs(m11 + m22) < 0.01 &&
              m11 > 0 &&
              fabs(m12) < 0.01 &&
              fabs(m21) < 0.01 &&
              fabs(state->getHorizScaling() - 1) < 0.001 &&
              (font->getType() == fontTrueType ||
	       font->getType() == fontTrueTypeOT);
  if (simpleTTF) {
    //~ need to create a FoFiTrueType object, and check for a Unicode cmap
  }
  if (state->getRender() != 0 || !simpleTTF) {
    gdi = false;
  }
}

void XWPreScanOutputDev::beginTransparencyGroup(XWGraphixState *, 
                                                double *,
				                                XWColorSpace *,
				                                bool , 
				                                bool ,
				                                bool )
{
	transparency = true;
  	gdi = false;
}

bool XWPreScanOutputDev::beginType3Char(XWGraphixState *, 
                                        double , 
                                        double ,
			                            double , 
			                            double ,
			                            uint , 
			                            uint *, int)
{
	return false;
}

void XWPreScanOutputDev::clearStats()
{
	mono = true;
  gray = true;
  transparency = false;
  patternImgMask = false;
  gdi = true;
}

void XWPreScanOutputDev::clip(XWGraphixState *)
{
}

void XWPreScanOutputDev::drawImage(XWGraphixState *state, 
                                   XWObject *, 
                                   XWStream *str,
			                       int width, 
			                       int height, 
			                       XWImageColorMap *colorMap,
			                       int *, 
			                       bool inlineImg)
{
	XWColorSpace *colorSpace;
  int i, j;

  colorSpace = colorMap->getColorSpace();
  if (colorSpace->getMode() == COLOR_SPACE_INDEXED) {
    colorSpace = ((XWIndexedColorSpace *)colorSpace)->getBase();
  }
  if (colorSpace->getMode() == COLOR_SPACE_DEVICEGRAY ||
      colorSpace->getMode() == COLOR_SPACE_CALGRAY) {
    if (colorMap->getBits() > 1) {
      mono = false;
    }
  } else {
    gray = false;
    mono = false;
  }
  if (state->getFillOpacity() != 1 ||
      state->getBlendMode() != BLEND_NORMAL) {
    transparency = true;
  }
  gdi = false;

  if (inlineImg) {
    str->reset();
    j = height * ((width * colorMap->getNumPixelComps() *
		   colorMap->getBits() + 7) / 8);
    for (i = 0; i < j; ++i)
      str->getChar();
    str->close();
  }
}

void XWPreScanOutputDev::drawImageMask(XWGraphixState *state, 
                                       XWObject *, 
                                       XWStream *str,
			                           int width, 
			                           int height, 
			                           bool ,
			                           bool inlineImg)
{
	int i, j;

  check(state->getFillColorSpace(), state->getFillColor(),
	state->getFillOpacity(), state->getBlendMode());
  if (state->getFillColorSpace()->getMode() == COLOR_SPACE_PATTERN) {
    patternImgMask = true;
  }
  gdi = false;

  if (inlineImg) {
    str->reset();
    j = height * ((width + 7) / 8);
    for (i = 0; i < j; ++i)
      str->getChar();
    str->close();
  }
}

void XWPreScanOutputDev::drawMaskedImage(XWGraphixState *state, 
                                         XWObject *, 
                                         XWStream *,
			                             int , 
			                             int ,
			                             XWImageColorMap *colorMap,
			                             XWStream *, 
			                             int , 
			                             int ,
			                             bool )
{
	XWColorSpace *colorSpace;

  colorSpace = colorMap->getColorSpace();
  if (colorSpace->getMode() == COLOR_SPACE_INDEXED) {
    colorSpace = ((XWIndexedColorSpace *)colorSpace)->getBase();
  }
  if (colorSpace->getMode() == COLOR_SPACE_DEVICEGRAY ||
      colorSpace->getMode() == COLOR_SPACE_CALGRAY) {
    if (colorMap->getBits() > 1) {
      mono = false;
    }
  } else {
    gray = false;
    mono = false;
  }
  if (state->getFillOpacity() != 1 ||
      state->getBlendMode() != BLEND_NORMAL) {
    transparency = true;
  }
  gdi = false;
}

void XWPreScanOutputDev::drawSoftMaskedImage(XWGraphixState *, 
                                             XWObject *, 
                                             XWStream *,
				                             int , 
				                             int ,
				                             XWImageColorMap *colorMap,
				                             XWStream *,
				                             int , 
				                             int ,
				                              XWImageColorMap *)
{
	XWColorSpace *colorSpace;

  colorSpace = colorMap->getColorSpace();
  if (colorSpace->getMode() == COLOR_SPACE_INDEXED) {
    colorSpace = ((XWIndexedColorSpace *)colorSpace)->getBase();
  }
  if (colorSpace->getMode() != COLOR_SPACE_DEVICEGRAY &&
      colorSpace->getMode() != COLOR_SPACE_CALGRAY) {
    gray = false;
  }
  mono = false;
  transparency = true;
  gdi = false;
}

void XWPreScanOutputDev::endPage()
{
}

void XWPreScanOutputDev::endStringOp(XWGraphixState *)
{
}

void XWPreScanOutputDev::endType3Char(XWGraphixState *)
{
}

void XWPreScanOutputDev::eoClip(XWGraphixState *)
{
}

void XWPreScanOutputDev::eoFill(XWGraphixState *state)
{
	check(state->getFillColorSpace(), state->getFillColor(),
			state->getFillOpacity(), state->getBlendMode());
}

void XWPreScanOutputDev::fill(XWGraphixState *state)
{
	check(state->getFillColorSpace(), state->getFillColor(),
			state->getFillOpacity(), state->getBlendMode());
}

bool XWPreScanOutputDev::functionShadedFill(XWGraphixState *state, XWFunctionShading *shading)
{
	if (shading->getColorSpace()->getMode() != COLOR_SPACE_DEVICEGRAY &&
      shading->getColorSpace()->getMode() != COLOR_SPACE_CALGRAY) {
    gray = false;
  }
  mono = false;
  if (state->getFillOpacity() != 1 ||
      state->getBlendMode() != BLEND_NORMAL) {
    transparency = true;
  }
  return true;
}

bool XWPreScanOutputDev::radialShadedFill(XWGraphixState *state, XWRadialShading *shading)
{
	if (shading->getColorSpace()->getMode() != COLOR_SPACE_DEVICEGRAY &&
      shading->getColorSpace()->getMode() != COLOR_SPACE_CALGRAY) {
    gray = false;
  }
  mono = false;
  if (state->getFillOpacity() != 1 ||
      state->getBlendMode() != BLEND_NORMAL) {
    transparency = true;
  }
  return true;
}

void XWPreScanOutputDev::startPage(int , XWGraphixState *)
{
}

void XWPreScanOutputDev::stroke(XWGraphixState *state)
{
	check(state->getStrokeColorSpace(), state->getStrokeColor(),
		  state->getStrokeOpacity(), state->getBlendMode());
		  
	double *dash = 0;
  	int dashLen = 0;
  	double dashStart = 0;
  	state->getLineDash(&dash, &dashLen, &dashStart);
  	if (dashLen != 0) 
    	gdi = false;
}

void XWPreScanOutputDev::tilingPatternFill(XWGraphixState *state, 
                                   XWGraphix *gfx, 
                                   XWObject *str,
				                           int paintType, 
				                           XWDict *resDict,
				                           double *mat, 
				                           double *bbox,
				                           int , 
				                           int , 
				                           int , 
				                           int ,
				                           double , 
				                           double )
{
	if (paintType == 1) {
    gfx->drawForm(str, resDict, mat, bbox);
  } else {
    check(state->getFillColorSpace(), state->getFillColor(),
	  state->getFillOpacity(), state->getBlendMode());
  }
}

void XWPreScanOutputDev::check(XWColorSpace *colorSpace, 
	                           CoreColor *color,
	                           double opacity, 
	                           int blendMode)
{
	if (colorSpace->getMode() == COLOR_SPACE_PATTERN) 
	{
    	mono = false;
    	gray = false;
    	gdi = false;
  	} 
  	else 
  	{
  		CoreRGB rgb;
    	colorSpace->getRGB(color, &rgb);
    	if (rgb.r != rgb.g || rgb.g != rgb.b || rgb.b != rgb.r) 
    	{
      		mono = false;
      		gray = false;
    	} 
    	else if (!((rgb.r == 0 && rgb.g == 0 && rgb.b == 0) ||
					(rgb.r == COLOR_COMP1 && 
					rgb.g == COLOR_COMP1 && rgb.b == COLOR_COMP1))) 
		{
      		mono = false;
    	}
  	}
  	
  	if (opacity != 1 || blendMode != BLEND_NORMAL) 
    	transparency = true;
}

