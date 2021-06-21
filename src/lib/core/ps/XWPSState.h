/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSSTATE_H
#define XWPSSTATE_H

#include "XWPSColorSpace.h"

class XWPSPath;
class XWPSClipPath;
class XWPSClipStack;
class XWPSPathEnum;
class XWPSFont;
class XWPSState;
class XWPSTextEnum;
class XWPSTransferMap;
class XWPSCachedFMPair;
class XWPSTextParams;
class XWPSDevice;
class XWPSShading;
class XWPSScreenHalftone;

class XWPSStateParts
{
public:
	XWPSStateParts();
	
	void assignParts(const XWPSStateParts * pfrom);
	void assignParts(const XWPSState * pfrom);
	
public:
	XWPSPath *path;
  XWPSClipPath *clip_path;
  XWPSClipPath *effective_clip_path;
  XWPSColorSpace *color_space;
  XWPSClientColor *ccolor;
  XWPSDeviceColor *dev_color;
};

class XWPSState : public XWPSImagerState
{
public:
	XWPSState(XWPSDevice * dev, XWPSState * pfrom = 0);
	XWPSState(XWPSState * pfrom, PSStateCopyReason reason = copy_for_gstate);
	~XWPSState();
	
	int  arc(float xc, float yc, float r, float ang1, float ang2);
	int  arcAdd(bool clockwise, 
	            float axc, 
	            float ayc, 
	            float arad, 
	            float aang1, 
	            float aang2, 
	            bool add_line);
	int  arcn(float xc, float yc, float r, float ang1, float ang2);
	int  arcTo(float ax1, float ay1, float ax2, float ay2, float arad, float * retxy);
	int  ashowBegin(float ax, 
	                float ay, 
	                const uchar * str, 
	                uint size,
	                XWPSTextEnum ** ppte);
	int  awidthShowBegin(float cx, 
	                     float cy, 
	                     ulong chr,
		                   float ax, 
		                   float ay, 
		                   const uchar * str, uint size,
		                   XWPSTextEnum ** ppte);
	
	int beginTransparencyGroup(const XWPSTransparencyGroupParams *ptgp, const XWPSRect *pbbox);
	int beginTransparencyMask(const XWPSTransparencyMaskParams *ptmp,const XWPSRect *pbbox);
	
	int  charBoxPathBegin(const uchar * str, 
	                      uint size,
		                    bool stroke_path, 
		                    XWPSTextEnum ** ppte);
	int  charPathBegin(const uchar * str, 
	                   uint size, 
	                   bool stroke_path,
		                 XWPSTextEnum ** ppte);
	XWPSCie * cieCSCommon();
	int  cieCSComplete(bool init);
	void cieLoadCommonCache(XWPSCie * pcie);
	int  clip();
	int  clipPath();
	int  clipRestore();
	int  clipSave();
	int  clipToPath();
	int  clipToRectangle(XWPSFixedRect * pbox);
	int  closePath();
	int   colorLoad();
	int   colorWritesPure();
	int   concat(const XWPSMatrix * pmat);
	int   copyGState(XWPSState * pfrom);
	int   copyPage();
	int   cshowBegin(const uchar * str, uint size, XWPSTextEnum ** ppte);
	bool  currentAccurateScreens() {return screen_accurate_screens;}
	float currentAlpha();
	PSBlendMode currentBlendMode() {return blend_mode;}
	int   currentCharMatrix(XWPSMatrix * ptm, bool force);
	XWPSCieJointCaches * currentCieCaches();
	int   currentCMYKColor(float pr4[4]);
	XWPSClientColor * currentColor() {return ccolor;}
	XWPSColorSpace * currentColorSpace() {return color_space;}
	PSColorSpaceIndex currentColorSpaceIndex() {return orig_cspace_index;}
	XWPSDevice * currentDevice() {return device;}
	XWPSColorSpace * currentDeviceCMYKSpace();
	XWPSColorSpace * currentDeviceGraySpace();
	XWPSColorSpace * currentDeviceRGBSpace();
	int   currentFillAdjust(XWPSPoint * adjust);
	XWPSFont * currentFont() {return font;}
	int   currentGray(float *pg);
	int   currentGState(XWPSState * pgs);
	int   currentHalftone(XWPSHalftone ** pht);
	int   currentHSBColor(float *pr3);
	bool  currentLimitClamp() {return clamp_coordinates;}
	uint   currentMinScreenLevels() {return screen_min_screen_levels;}
	float  currentOpacityAlpha() {return opacity.alpha;}
	bool   currentOverprint() {return overprint;}
	int    currentOverprintMode() {return overprint_mode;}
	XWPSPath * currentPath() {return path;}
	int   currentPoint(XWPSPoint * ppt);
	PSRop3 currentRasterOp();
	int   currentRGBColor(float pr3[3]);
	int   currentScreen(XWPSScreenHalftone * phsp);
	int   currentScreenLevels();
	int   currentScreenPhase(XWPSIntPoint * pphase, PSColorSelect select);
	float currentShapeAlpha() {return shape.alpha;}
	float currentSmoothness() {return smoothness;}
	bool  currentSourceTransparent();
	bool  currentTextKnockout() {return text_knockout;}
	bool  currentTextureTransparent();
	int   curveTo(float x1, float y1, 
	              float x2, float y2, 
	              float x3, float y3);
	PSTransparencyStateType currentTransparencyType();
	
	int dashPath();
	int defaultClipBox(XWPSFixedRect * pbox);
	int defaultMatrix(XWPSMatrix * pmat);
	int discardTransparencyLayer();
	int dtransform(float dx, float dy, XWPSPoint * pt);
	
	int effectiveClipPath(XWPSClipPath ** ppcpath);
	int endTransparencyGroup();
	int endTransparencyMask(PSTransparencyChannelSelector csel);
	int eoclip();
	int eofill();
	int eoviewClip();
	int erasePage();
	
	int fill();
	int fillPage();
	int fillPath(XWPSPath * ppath, XWPSDeviceColor * pdevc, int rule, long adjust_x, long adjust_y);
	int flattenPath();
	int flushPage();
	
	XWPSPath * getPath() {return path;}
	XWPSState * getSaved() {return saved;}
	int glyphPathBegin(ulong glyph, 
	                   bool stroke_path,
		                 XWPSTextEnum ** ppte);
	int glyphShowBegin(ulong glyph, XWPSTextEnum ** ppte);
	int glyphWidthBegin(ulong glyph, XWPSTextEnum ** ppte);
	
	int install(XWPSHalftone * pht, XWPSDeviceHalftone * pdht);
	int installCieABC(XWPSCieABC *pcie);
	
	int idtransform(float dx, float dy, XWPSPoint * pt);
	int imagePath(int width, int height, const uchar * data);
	PSInCacheDevice inCacheDevice() {return in_cachedevice;}
	int initClip();
	int initGraphics();
	int initMatrix();
	int initTransparencyMask(PSTransparencyChannelSelector csel);
	int initViewClip();
	int itransform(float x, float y, XWPSPoint * pt);
	
	int kshowBegin(const uchar * str, uint size, XWPSTextEnum ** ppte);
	
	int lineTo(float x, float y);
	void loadTransferMap(XWPSTransferMap * pmap, float min_value);
	XWPSCachedFMPair * lookupFMPair(XWPSFont * pfont);
	void lookupXFont(XWPSCachedFMPair * pair, int );
	
	int moveTo(float x, float y);
	int newPath();
	int nullDevice();
	
	int outputPage(int num_copies, int flush);
	
	int pathEnumCopyInit(XWPSPathEnum * penum, bool copy);
	int pathEnumNext(XWPSPathEnum * penum, XWPSPoint * ppts);
	int putDeviceParams(XWPSContextState * ctx, XWPSParamList * plist);
	
	int rcurveTo(float dx1, float dy1, 
	             float dx2, float dy2, 
	             float dx3, float dy3);
	int rectAppend(const XWPSRect * pr, uint count);
	int rectClip(const XWPSRect * pr, uint count);
	int rectFill(const XWPSRect * pr, uint count);
	int rectStroke(const XWPSRect * pr, uint count, const XWPSMatrix * pmat);
	int remapColor();
	int reversePath();
	int rlineTo(float x, float y);
	int rmoveTo(float x, float y);
	int restore();
	int restoreAll();
	int restoreAllForRestore(XWPSState * savedA);
	int restoreOnly();
	XWPSFont * rootFont() {return root_font;}
	int rotate(float ang);
	
	int  save();
	int  saveForSave(XWPSState ** psaved);
	int  scale(float sx, float sy);
	int  scaleCharMatrix(int sx, int sy);
	void setAccurateScreens(bool accurate) {screen_accurate_screens = accurate;}
	int  setAlpha(float alphaA);
	int  setBbox(float llx, float lly, float urx, float ury);
	int  setBlackGeneration(PSTransferMapType proc);
	int  setBlackGenerationRemap(PSTransferMapType proc, bool remap);
	int  setBlendMode(PSBlendMode mode);
	int  setCharMatrix(const XWPSMatrix * pmat);
	int  setCMYKColor(float c, float m, float y, float k);
	int  setColor(XWPSClientColor * pcc);
	int  setColorRendering(XWPSCieRender * pcrd);
	int  setColorSpace(XWPSColorSpace * pcs);
	int  setColorTransfer(PSTransferMapType red_proc,
		                        PSTransferMapType green_proc, 
		                        PSTransferMapType blue_proc,
		                        PSTransferMapType gray_proc);
	int  setColorTransferRemap(PSTransferMapType red_proc,
			  										 PSTransferMapType green_proc,
			  										 PSTransferMapType blue_proc,
			  										 PSTransferMapType gray_proc, bool remap);
	void  setCurrentFont(XWPSFont * pfont);
	int  setDefaultMatrix(const XWPSMatrix * pmat);
	int  setDevColor();
	void setDeviceColor1();
	int  setDeviceNoErase(XWPSDevice * dev);
	int  setDeviceNoInit(XWPSDevice * dev);
	int  setDeviceOnly(XWPSDevice * dev);
	void setEffectiveTransfer();
	int  setFont(XWPSFont * pfont);
	int  setGray(float gray);
	int  setHalftonePrepare(XWPSHalftone * pht, XWPSDeviceHalftone * pdht);
	int  setHSBColor(float h, float s, float b);
	int  setDotOrientation();
	int  setGState(XWPSState * pfrom);
	int  setInverse();
	void setLimitClamp(bool clamp) {clamp_coordinates=clamp;}
	int  setMatrix(XWPSMatrix * pmat);
	void setMinScreenLevels(uint levels) {screen_min_screen_levels = levels;}
	int  setNullColor();
	int  setOpacityAlpha(float alpha);
	void setOverprint(bool ovp) {overprint = ovp;}
	int  setOverprintMode(int mode);
	int  setRasterOp(PSRop3 rop);
	int  setRGBColor(float r, float g, float b);
	int  setScreen(XWPSScreenHalftone * phsp);
	int  setScreenPhase(int x, int y, PSColorSelect select);
	int  setShapeAlpha(float alpha);
	int  setSmoothness(float smoothnessA);
	int  setSourceTransparent(bool transparent);
	void setStrokeAdjust(bool stroke_adjustA) {stroke_adjust=stroke_adjustA;}
	int  setSubstituteColorSpace(PSColorSpaceIndex csi, XWPSColorSpace *pcs);
	int  setTextKnockout(bool knockout);
	int  setTextureTransparent(bool transparent);
	int  setToCharMatrix();
	int  setTransfer(int t);
	int  setTransferRemap(int t, bool remap);
	int  setUnderColorRemoval(PSTransferMapType proc);
	int  setUnderColorRemovalRemap(PSTransferMapType proc, bool remap);
	int  shfill(XWPSShading * psh);
	int  showBegin(const uchar * str, uint size, XWPSTextEnum ** ppte);
	int  stringWidthBegin(const uchar * str, uint size, XWPSTextEnum ** ppte);
	int  stroke();
	int  strokeAdd(XWPSPath * ppath, XWPSPath * to_path);
	int  strokeFill(XWPSPath * ppath);
	int  strokePath();
	XWPSState * swapSaved(XWPSState * new_saved);
	
	int textBegin(XWPSTextParams * text, XWPSTextEnum ** ppte);
	int transform(float x, float y, XWPSPoint * pt);
	int translate(float dx, float dy);
	int translateToFixed(long px, long py);
	
	void unsetDevColor();	
	void updateCTM(double xt, double yt);
	void updateDevice();
	int  upmergePath();
	int  upathBbox(XWPSRect * pbox, bool include_moveto);
	
	int viewClip();
	int viewClipPath();
	
	int widthShowBegin(float cx, 
	                   float cy, 
	                   ulong chr,
		                 const uchar * str, 
		                 uint size,
		                 XWPSTextEnum ** ppte);
	int xyshowBegin(const uchar * str, 
	                uint size,
		              const float *x_widths, 
		              const float *y_widths,
		              uint widths_size, 
		              XWPSTextEnum ** ppte);
	
public:	
	XWPSState *saved;
	XWPSMatrix ctm_inverse;
  bool ctm_inverse_valid;
  XWPSMatrix ctm_default;
  bool ctm_default_set;
  XWPSPath *path;
  XWPSClipPath *clip_path;
  XWPSClipStack *clip_stack;
  XWPSClipPath *view_clip;
  bool clamp_coordinates;
  ulong effective_clip_id;
  ulong effective_view_clip_id;
  XWPSClipPath *effective_clip_path;
  bool effective_clip_shared;
  PSColorSpaceIndex	orig_cspace_index, orig_base_cspace_index;
  XWPSColorSpace *color_space;
  XWPSDeviceColorSpace device_color_spaces;
  XWPSClientColor *ccolor;
  XWPSDeviceColor *dev_color;
  XWPSFont *font;
  XWPSFont *root_font;
  XWPSMatrixFixed char_tm;
  bool char_tm_valid;
  PSInCacheDevice in_cachedevice;
  PSCharPathMode in_charpath;
  XWPSState *show_gstate;
  int level;
  XWPSDevice *device;
  
  bool screen_accurate_screens;
  uint screen_min_screen_levels;
  
private:
	int alphaBufferBits();
	int alphaBufferInit(long extra_x, long extra_y, int alpha_bits);
	void alphaBufferRelease(bool newpath);
	
	int assignPart(XWPSState * pfrom);
		
	void clearClipStack();
	int clone(XWPSState * pfrom, PSStateCopyReason reason = copy_for_gstate);
	int commonClip(int rule);
	int commonViewClip(int rule);
	int copy(XWPSState * pfrom, PSStateCopyReason reason = copy_for_copygstate);
	
	int fillWithRule(int rule);
	void freeContents();
	
	void popTransparencyStack();
	int pushTransparencyStack(PSTransparencyStateType type);
	
	void scaleDashPattern(float scale);
	int  scalePaths(int log2_scale_x, int log2_scale_y, bool do_path);
	
	void setDefault();
};

class XWPSStatus
{
public:
	XWPSStatus();
	
	int addDxDy(int dxA, int dyA, int countA);
	
	int getPixel(int x, int y);
	
	int traceFrom(int x0, int y0, int detect);
	
public:
	XWPSState *pgs;
	const uchar *data;
	int width, height, raster;
	int dx, dy;
	int count;
	int outline_scale;
	int step;
};

#endif //XWPSSTATE_H
