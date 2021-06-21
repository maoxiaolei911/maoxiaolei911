/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "XWRasterType.h"
#include "XWRasterPattern.h"
#include "XWScreen.h"
#include "XWRasterClip.h"
#include "XWBitmap.h"
#include "XWRasterState.h"

XWRasterState::XWRasterState(int width, 
                             int height, 
                             bool vectorAntialias, 
                             ScreenParams *screenParams)
{
  uchar color[4];
  int i;

  matrix[0] = 1;  matrix[1] = 0;
  matrix[2] = 0;  matrix[3] = 1;
  matrix[4] = 0;  matrix[5] = 0;
  memset(color, 0, sizeof(color));
  strokePattern = new XWSolidColor(color);
  fillPattern = new XWSolidColor(color);
  screen = new XWScreen(screenParams);
  blendFunc = NULL;
  strokeAlpha = 1;
  fillAlpha = 1;
  lineWidth = 0;
  lineCap = LINE_CAP_BUTT;
  lineJoin = LINE_JOIN_MITER;
  miterLimit = 10;
  flatness = 1;
  lineDash = NULL;
  lineDashLength = 0;
  lineDashPhase = 0;
  strokeAdjust = false;
  clip = new XWRasterClip(0, 0, width, height, vectorAntialias);
  softMask = NULL;
  deleteSoftMask = false;
  inNonIsolatedGroup = false;
  for (i = 0; i < 256; ++i) {
    rgbTransferR[i] = (uchar)i;
    rgbTransferG[i] = (uchar)i;
    rgbTransferB[i] = (uchar)i;
    grayTransfer[i] = (uchar)i;
    cmykTransferC[i] = (uchar)i;
    cmykTransferM[i] = (uchar)i;
    cmykTransferY[i] = (uchar)i;
    cmykTransferK[i] = (uchar)i;
  }
  overprintMask = 0xffffffff;
  next = NULL;
}

XWRasterState::XWRasterState(int width, 
                             int height, 
                             bool vectorAntialias, 
                             XWScreen *screenA)
{
  uchar color[4];
  int i;

  matrix[0] = 1;  matrix[1] = 0;
  matrix[2] = 0;  matrix[3] = 1;
  matrix[4] = 0;  matrix[5] = 0;
  memset(color, 0, sizeof(color));
  strokePattern = new XWSolidColor(color);
  fillPattern = new XWSolidColor(color);
  screen = screenA->copy();
  blendFunc = NULL;
  strokeAlpha = 1;
  fillAlpha = 1;
  lineWidth = 0;
  lineCap = LINE_CAP_BUTT;
  lineJoin = LINE_JOIN_MITER;
  miterLimit = 10;
  flatness = 1;
  lineDash = NULL;
  lineDashLength = 0;
  lineDashPhase = 0;
  strokeAdjust = false;
  clip = new XWRasterClip(0, 0, width, height, vectorAntialias);
  softMask = NULL;
  deleteSoftMask = false;
  inNonIsolatedGroup = false;
  for (i = 0; i < 256; ++i) {
    rgbTransferR[i] = (uchar)i;
    rgbTransferG[i] = (uchar)i;
    rgbTransferB[i] = (uchar)i;
    grayTransfer[i] = (uchar)i;
    cmykTransferC[i] = (uchar)i;
    cmykTransferM[i] = (uchar)i;
    cmykTransferY[i] = (uchar)i;
    cmykTransferK[i] = (uchar)i;
  }
  overprintMask = 0xffffffff;
  next = NULL;
}

XWRasterState::XWRasterState(XWRasterState * state)
{
  memcpy(matrix, state->matrix, 6 * sizeof(double));
  strokePattern = state->strokePattern->copy();
  fillPattern = state->fillPattern->copy();
  screen = state->screen->copy();
  blendFunc = state->blendFunc;
  strokeAlpha = state->strokeAlpha;
  fillAlpha = state->fillAlpha;
  lineWidth = state->lineWidth;
  lineCap = state->lineCap;
  lineJoin = state->lineJoin;
  miterLimit = state->miterLimit;
  flatness = state->flatness;
  if (state->lineDash) {
    lineDashLength = state->lineDashLength;
    lineDash = (double *)malloc(lineDashLength * sizeof(double));
    memcpy(lineDash, state->lineDash, lineDashLength * sizeof(double));
  } else {
    lineDash = NULL;
    lineDashLength = 0;
  }
  lineDashPhase = state->lineDashPhase;
  strokeAdjust = state->strokeAdjust;
  clip = state->clip->copy();
  softMask = state->softMask;
  deleteSoftMask = false;
  inNonIsolatedGroup = state->inNonIsolatedGroup;
  memcpy(rgbTransferR, state->rgbTransferR, 256);
  memcpy(rgbTransferG, state->rgbTransferG, 256);
  memcpy(rgbTransferB, state->rgbTransferB, 256);
  memcpy(grayTransfer, state->grayTransfer, 256);
  memcpy(cmykTransferC, state->cmykTransferC, 256);
  memcpy(cmykTransferM, state->cmykTransferM, 256);
  memcpy(cmykTransferY, state->cmykTransferY, 256);
  memcpy(cmykTransferK, state->cmykTransferK, 256);
  overprintMask = state->overprintMask;
  next = NULL;
}

XWRasterState::~XWRasterState()
{
    if (strokePattern)
        delete strokePattern;
        
    if (fillPattern)
        delete fillPattern;
        
    if (screen)
        delete screen;
        
    if (lineDash)
        free(lineDash);
        
    if (clip)
        delete clip;
        
    if (deleteSoftMask && softMask) 
        delete softMask;
}

void XWRasterState::setFillPattern(XWRasterPattern *fillPatternA)
{
    if (fillPattern)
        delete fillPattern;
    fillPattern = fillPatternA;
}

void XWRasterState::setScreen(XWScreen *screenA)
{
    if (screen)
        delete screen;
        
    screen = screenA;
}

void XWRasterState::setLineDash(double *lineDashA, int lineDashLengthA, double lineDashPhaseA)
{
	if (lineDash)
		free(lineDash);
  lineDashLength = lineDashLengthA;
  if (lineDashLength > 0) {
    lineDash = (double *)malloc(lineDashLength * sizeof(double));
    memcpy(lineDash, lineDashA, lineDashLength * sizeof(double));
  } else {
    lineDash = NULL;
  }
  lineDashPhase = lineDashPhaseA;
}

void XWRasterState::setSoftMask(XWBitmap *softMaskA)
{
    if (deleteSoftMask) 
        delete softMask;
    
    softMask = softMaskA;
    deleteSoftMask = true;
}

void XWRasterState::setStrokePattern(XWRasterPattern *strokePatternA)
{
    if (strokePattern)
        delete strokePattern;
    strokePattern = strokePatternA;
}

void XWRasterState::setTransfer(uchar *red, uchar *green, uchar *blue, uchar *gray)
{
	memcpy(rgbTransferR, red, 256);
  memcpy(rgbTransferG, green, 256);
  memcpy(rgbTransferB, blue, 256);
  memcpy(grayTransfer, gray, 256);
  for (int i = 0; i < 256; ++i) 
  {
    cmykTransferC[i] = 255 - rgbTransferR[255 - i];
    cmykTransferM[i] = 255 - rgbTransferG[255 - i];
    cmykTransferY[i] = 255 - rgbTransferB[255 - i];
    cmykTransferK[i] = 255 - grayTransfer[255 - i];
  }
}

