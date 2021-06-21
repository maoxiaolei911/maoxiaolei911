/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSCOLORSPACE_H
#define XWPSCOLORSPACE_H

#include "XWPSCie.h"
#include "XWPSColor.h"

class XWPSStream;
class XWPSContextState;
class XWPSHalftone;
class XWPSDeviceHalftone;
class XWPSFunction;
class XWPSHTCache;
class XWPSTransferMap;
class XWPSColorSpace;
class XWPSDevice;
class XWPSColorTile;
class XWPSStripBitmap;
class XWPSImagerState;
class XWPSState;
class XWPSPath;
class XWPSClipPath;
class XWPSDeviceClip;
class XWPSDeviceRopTexture;
struct PSStreamImageScaleState;
class XWPSIndexedParams;
class XWPSSeparationParams;
class XWPSDevicePixelParams;
class XWPSDeviceNParams;
class XWPSPatternParams;
class XWPSDeviceMem;
class XWPSDevicePatternAccum;
class XWPSPDFImageWriter;
class XWPSDVIImageWriter;
class XWPSImageCommon;
class XWPSDataImage;
class XWPSPixelImage;
class XWPSImage1;
class XWPSImage2;
class XWPSImage3;
class XWPSImage3X;
class XWPSImage4;
class XWPSImageEnumCommon;
class XWPSImageEnum;
class XWPSImage3Enum;
class XWPSImage3XEnum;
class XWPSBboxImageEnum;
class XWPSPDFImageEnum;
class XWPSDVIImageEnum;

#define USE_ADOBE_CMYK_RGB

struct PSColorSpaceType
{
	PSColorSpaceIndex index;
	bool can_be_base_space;
  bool can_be_alt_space;
  int (XWPSColorSpace::*num_components)();
  XWPSColorSpace * (XWPSColorSpace::*base_space)();
  bool (XWPSColorSpace::*equal)(XWPSColorSpace*);
  void  (XWPSColorSpace::*init_color)(XWPSClientColor*);
  void  (XWPSColorSpace::*restrict_color)(XWPSClientColor*);
  XWPSColorSpace * (XWPSColorSpace::*concrete_space)(const XWPSImagerState*);
  int (XWPSColorSpace::*concretize_color)(const XWPSClientColor *, ushort *, const XWPSImagerState *);
  int (XWPSImagerState::*remap_concrete_color)(const ushort * ,
	                                             XWPSDeviceColor * , 
	                                             XWPSDevice * ,
																							 PSColorSelect );
	int (XWPSImagerState::*remap_color)(XWPSClientColor *, 		                                 
		                                 XWPSColorSpace * ,
		                                 XWPSDeviceColor * ,
		                                 XWPSDevice * ,
																		 PSColorSelect );
	int (XWPSColorSpace::*install_cspace)(XWPSState*);
};

extern PSColorSpaceType ps_color_space_type_DeviceGray;
extern PSColorSpaceType ps_color_space_type_DeviceRGB;
extern PSColorSpaceType ps_color_space_type_DeviceCMYK;
extern PSColorSpaceType ps_color_space_type_DevicePixel;
extern PSColorSpaceType ps_color_space_type_CIEDEFG;
extern PSColorSpaceType ps_color_space_type_CIEDEF;
extern PSColorSpaceType ps_color_space_type_CIEABC;
extern PSColorSpaceType ps_color_space_type_CIEA;
extern PSColorSpaceType ps_color_space_type_DeviceN;
extern PSColorSpaceType ps_color_space_type_Indexed;
extern PSColorSpaceType ps_color_space_type_Separation;
extern PSColorSpaceType ps_color_space_type_Pattern;

class XWPSColorSpace : public XWPSStruct
{
public:
	XWPSColorSpace();
	XWPSColorSpace(PSColorSpaceType * t, void *client_dataA = 0);
	XWPSColorSpace(XWPSColorSpace * src);
	virtual ~XWPSColorSpace();
	
	XWPSColorSpace * altSpaceDeviceN();
	XWPSColorSpace * altSpaceSeparation();
	void assign(XWPSColorSpace * psrc);
	
	XWPSColorSpace * baseSpace();
	XWPSColorSpace * baseSpaceIndexed();
	XWPSColorSpace * baseSpacePattern();
	
	XWPSCie * cieCSCommonABC(XWPSCieABC **ppabc);
	int cieRemapFinish(PSCieCachedVector3 vec3, 
	                   ushort * pconc,
		                 const XWPSImagerState * pis);
	int concretizeCIEA(const XWPSClientColor * pc, 
		                 ushort * pconc, 
		                 const XWPSImagerState * pis);
	int concretizeCIEABC(const XWPSClientColor * pc, 
		     							  ushort * pconc, 
		     							  const XWPSImagerState * pis);
	int concretizeCIEDEF(const XWPSClientColor * pc, 
		     								ushort * pconc, 
		     								const XWPSImagerState * pis);
	int concretizeCIEDEFG(const XWPSClientColor * pc, 
		      							ushort * pconc, 
		      							const XWPSImagerState * pis);
	int concretizeColor(const XWPSClientColor * pc,
			                     ushort * pconc, 
			                     const XWPSImagerState *pis);
	int concretizeDeviceCMYK(const XWPSClientColor * pc,
			                     ushort * pconc, 
			                     const XWPSImagerState *);
	int concretizeDeviceGray(const XWPSClientColor * pc,
			                     ushort * pconc, 
			                     const XWPSImagerState *);
	int concretizeDeviceN(const XWPSClientColor * pc, 
		                      ushort * pconc, 
		                      const XWPSImagerState * pis);
	int concretizeDevicePixel(const XWPSClientColor * pc,
			                     ushort * pconc, 
			                     const XWPSImagerState *);
	int concretizeDeviceRGB(const XWPSClientColor * pc,
			                     ushort * pconc, 
			                     const XWPSImagerState *);
	int concretizeIndexed(const XWPSClientColor * pc, 
			                    ushort * pconc, 
			                    const XWPSImagerState * pis);
	int concretizeNo(const XWPSClientColor * , 
			                    ushort * , 
			                    const XWPSImagerState * );
	int concretizeSeparation(const XWPSClientColor *pc, 
			                     ushort *pconc, 
			                     const XWPSImagerState *pis);
	XWPSColorSpace * concreteSpaceDeviceN(const XWPSImagerState * pis);
	XWPSColorSpace * concreteSpace(const XWPSImagerState*pis);
	XWPSColorSpace * concreteSpaceCIE(const XWPSImagerState * pis);
	XWPSColorSpace * concreteSpaceIndexed(const XWPSImagerState * pis);
	XWPSColorSpace * concreteSpaceNo(const XWPSImagerState * ) {return 0;}
	XWPSColorSpace * concreteSpaceSeparation(const XWPSImagerState * pis);
			                     
	void copy(XWPSColorSpace * src);
	
	bool equal(XWPSColorSpace *pcs2);
	bool equalDeviceN(XWPSColorSpace *pcs2);
	bool equalDevicePixel(XWPSColorSpace *pcs2);
	bool equalIndexed(XWPSColorSpace *pcs2);
	bool equalSeparation(XWPSColorSpace *pcs2);
	
	void freeParams();
	
	XWPSFunction * getDevNFunction();	
	PSColorSpaceIndex getIndex();
	XWPSFunction * getSeprFunction();
			 
	uint  indexedTableSize();
	void init(PSColorSpaceType * pcstype, void *client_dataA = 0);
	void initCIE(XWPSClientColor * pcc);
	void initColor(XWPSClientColor * pcc);
	int  initDevice(int num_components);
	void initDeviceCMYK();
	void initDeviceGray();
	void initDeviceN(XWPSClientColor * pcc);
	void initDevicePixel(int depth);
	void initDeviceRGB();
	void initSeparation(XWPSClientColor * pcc);
	void initFrom(XWPSColorSpace * pcsfrom);
	void initPaint1(XWPSClientColor * pcc);
	void initPaint3(XWPSClientColor * pcc);
	void initPaint4(XWPSClientColor * pcc);
	void initPattern(XWPSClientColor * pcc);
	int  installCIE(XWPSState * pgs);
	int installCIEA(XWPSState * pgs);
	int installCIEABC(XWPSState * pgs);
	int installCIEDEF(XWPSState * pgs);
	int installCIEDEFG(XWPSState * pgs);
	int  installCSpace(XWPSState*pgs);
	int installDeviceN(XWPSState * pgs);
	int installIndexed(XWPSState * pgs);
	int installPattern(XWPSState * pgs);
	int installSeparation(XWPSState * pgs);
	bool isEqual(XWPSColorSpace*) {return true;}	
	
	XWPSColorSpace * noBaseSpace() {return 0;}
	bool notEqual(XWPSColorSpace *) {return false;}
	int noInstallCSpace(XWPSState *) {return 0;}
	int numComponents();
	int numComponents1() {return 1;}
	int numComponents3() {return 3;}
	int numComponents4() {return 4;}
	int numComponentsDeviceN();
	int numComponentsPattern();

	int  remapColor(XWPSImagerState * pis,
	                XWPSClientColor * pcc, 
	                XWPSColorSpace * pcs,
	                XWPSDeviceColor * pdc, 
	                XWPSDevice * dev,
		              PSColorSelect select);
	int  remapConcreteColor(XWPSImagerState * pis, 
	                        const ushort * pconc,
	                       XWPSDeviceColor * pdc, 
	                       XWPSDevice * dev,
			                   PSColorSelect select);
	void restrictCIEA(XWPSClientColor * pcc);
	void restrictCIEABC(XWPSClientColor * pcc);
	void restrictCIEDEF(XWPSClientColor * pcc);
	void restrictCIEDEFG(XWPSClientColor * pcc);
	void restrictColor(XWPSClientColor * pcc);	
	void restrictDeviceN(XWPSClientColor * pcc);
	void restrictDevicePixel(XWPSClientColor * pcc);
	void restrictIndexed(XWPSClientColor * pcc);
	void restrict01Paint1(XWPSClientColor * pcc);
	void restrict01Paint3(XWPSClientColor * pcc);
	void restrict01Paint4(XWPSClientColor * pcc);
	void restrictPattern(XWPSClientColor * pcc);
	
	XWPSColorSpace * sameConcreteSpace(const XWPSImagerState * ) {return this;}
	int setDevNFunction(XWPSFunction *pfn);
	int setSeprFunction(XWPSFunction *pfn);
	
public:
	PSColorSpaceType * type;
	ulong id;
	XWPSContextState * i_ctx_p;
	union
	{
		XWPSDevicePixelParams * pixel;
		XWPSCieDefg          *  defg;
		XWPSCieDef           *  def;
		XWPSCieABC           *  abc;
		XWPSCieA             *  a;
		XWPSSeparationParams  * separation;
		XWPSDeviceNParams     * device_n;
		XWPSIndexedParams     * indexed;
		XWPSPatternParams     * pattern;
	} params;	
};


class XWPSIndexedMap : public XWPSStruct
{
public:
	XWPSIndexedMap();
	XWPSIndexedMap(int nvals);
	XWPSIndexedMap(XWPSColorSpace * pbase_cspace, int nvals);
	~XWPSIndexedMap();
		
	int getLength();
	const char * getTypeName();
	
	int lookupIndex(XWPSIndexedParams *params, int index, float * valuesA);
	
	int tintTransform(XWPSSeparationParams * params, float tint, float *valuesA);
	
public:
	union 
	{
		int (XWPSIndexedParams::*lookup_index)(int, float *);
		int (XWPSSeparationParams::*tint_transform)(float, float *);
  } proc;
    
  void *proc_data;
	uint num_values;
	float *values;
};

class XWPSDevicePixelParams : public XWPSStruct
{
public:
	XWPSDevicePixelParams();
		
public:
	int depth;
};

#define SEPARATION_CACHE_SIZE 360

class XWPSSeparationParams : public XWPSStruct
{
public:
	XWPSSeparationParams();
	~XWPSSeparationParams();
		
	int lookupTint(float tint, float *values);
	
	int mapSeprUsingFunction(float in_val, float *out_vals);
	int mapTintValue(float in_val, float *out_vals);
	
	int usingFunction(float in_val, float *out_vals);
	
public:
	ulong sname;
  XWPSColorSpace * alt_space;
  XWPSIndexedMap *map;
};

class XWPSDeviceNMap : public XWPSStruct
{
public:
	XWPSDeviceNMap();
	~XWPSDeviceNMap();
	
	int getLength();
	const char * getTypeName();
	
	int tintTransform(XWPSDeviceNParams * params,
	                  const float *in, 
	                  float *out,
	                  const XWPSImagerState *pis, 
	                  void *data);
	
public:
	int (XWPSDeviceNParams::*tint_transform)(const float *, float *, const XWPSImagerState *, void *);
  void *tint_transform_data;
	bool cache_valid;
  float tint[PS_CLIENT_COLOR_MAX_COMPONENTS];
  short conc[PS_DEVICE_COLOR_MAX_COMPONENTS];
  bool tint_transform_data_pfn;
};

class XWPSDeviceNParams : public XWPSStruct
{
public:
	XWPSDeviceNParams();
	~XWPSDeviceNParams();
		
	int deviceNRemapTransform(const float *in,
			                       float *out, 
			                       const XWPSImagerState *pis, 
			                       void *data);
	
	int mapDevNUsingFunction(const float *in, 
	                         float *out,
			                     const XWPSImagerState *pis, 
			                     void *data);
			                     
	int usingFunction(const float *in, 
	                  float *out,
			              const XWPSImagerState *pis, 
			              void *data);
			              
	int ztransformDeviceN(const float *in,
		                    float *out, 
		                    const XWPSImagerState *pis, 
		                    void *data);
	
public:
	ulong *names;
  uint num_components;
  XWPSColorSpace * alt_space;
  XWPSDeviceNMap *map;
};

class XWPSIndexedParams : public XWPSStruct
{
public:
	XWPSIndexedParams();
	~XWPSIndexedParams();
		
	int indexedLookup(int index,	 XWPSClientColor *pcc);
	
	int lookupIndexedMap(int index, float *values);
	
	int mapPaletteEntry1(int indx, float *values);
	int mapPaletteEntry3(int indx, float *values);
	int mapPaletteEntry4(int indx, float *values);
	int mapPaletteEntryN(int indx, float *values);
	
public:
	XWPSColorSpace * base_space;
	int hival;
	union 
	{
		XWPSString * table;	
		XWPSIndexedMap *map;
  } lookup;
  bool use_proc;
};

class XWPSPatternParams : public XWPSStruct
{
public:
	XWPSPatternParams();
	~XWPSPatternParams();
		
public:
	bool has_base_space;
  XWPSColorSpace * base_space;
};

class XWPSDeviceColorSpace
{
public:
	XWPSDeviceColorSpace();
	~XWPSDeviceColorSpace();
	
	XWPSColorSpace *deviceCMYK() {return named.CMYK;}
	XWPSColorSpace *deviceGray() {return named.Gray;}
	XWPSColorSpace *deviceRGB() {return named.RGB;}
	
	void free();
	
public:
	struct dcn_ 
	{
		XWPSColorSpace *Gray;
		XWPSColorSpace *RGB;
		XWPSColorSpace *CMYK;
  } named;
  
  XWPSColorSpace *indexed[3];
};

class XWPSImagerStateShared : public XWPSStruct
{
public:
	XWPSImagerStateShared();
	~XWPSImagerStateShared();
	
	XWPSColorSpace *deviceCMYK();
	XWPSColorSpace *deviceGray();
	XWPSColorSpace *deviceRGB();
	
	int getLength();
	const char * getTypeName();
	
public:
	XWPSDeviceColorSpace device_color_spaces;
};

class XWPSTransparencySource : public XWPSStruct
{
public:
	XWPSTransparencySource();
	
	int getLength();
	const char * getTypeName();
	
public:
	float alpha;
};

class XWPSTransparencyGroupParams
{
public:
	XWPSTransparencyGroupParams() : ColorSpace(0),Isolated(false),Knockout(false){}
	
public:
	const XWPSColorSpace *ColorSpace;
  bool Isolated;
  bool Knockout;
};

class XWPSTransparencyMaskParams
{
public:
	XWPSTransparencyMaskParams();
	~XWPSTransparencyMaskParams();
	
	int maskTransferIdentity(float in, float *out, void *proc_data);
	int tfusingFunction(float in_val, float *out, void *proc_data);
	
public:
	PSTransparencyMaskSubtype subtype;
  bool has_Background;
  float Background[PS_CLIENT_COLOR_MAX_COMPONENTS];
  int (XWPSTransparencyMaskParams::*TransferFunction)(float, float *, void *);
  void *TransferFunction_data;
};

class XWPSTransparencyState
{
public:
	XWPSTransparencyState() : saved(0),type(TRANSPARENCY_STATE_Group) {}
	
public:
	XWPSTransparencyState *saved;
  PSTransparencyStateType type;
};

struct XWPSTransferColored
{
	XWPSTransferMap *red;
  XWPSTransferMap *green;
  XWPSTransferMap *blue;
  XWPSTransferMap *gray;
};

class XWPSTransfer
{
public:
	XWPSTransfer();
	~XWPSTransfer();
	
	void copy(XWPSTransfer * other);
	
	void loadTransferMap(float min_value);
	void remap(int t);
	
	void setIndexed(XWPSTransferMap * pmap);
	
public:
	union
	{
		XWPSTransferMap *indexed[4];
		XWPSTransferColored colored;
	} u;
};

class XWPSPatternCache
{
public:
	XWPSPatternCache();
	XWPSPatternCache(uint num_tilesA, ulong max_bitsA);
	~XWPSPatternCache();
	
	void copy(const XWPSPatternCache * from);
	
	void freeEntry(XWPSColorTile * ctile);
	
public:
	XWPSColorTile *tiles;
  uint num_tiles;
  uint tiles_used;
  uint next;
  ulong bits_used;
  ulong max_bits;
  bool own_tiles;
};

#define lum_red_weight	30
#define lum_green_weight	59
#define lum_blue_weight	11
#define lum_all_weights	(lum_red_weight + lum_green_weight + lum_blue_weight)

struct PSColorMapProcs
{
	void (XWPSImagerState::*map_gray)(short, 
		                                XWPSDeviceColor *, 
		                                XWPSDevice *, 
		                                PSColorSelect);
	void (XWPSImagerState::*map_rgb)(ushort, 
		                               ushort, 
		                               ushort, 
		                               XWPSDeviceColor *, 
		                               XWPSDevice *, 
		                               PSColorSelect);
	void (XWPSImagerState::*map_cmyk)(short, 
		                                short, 
		                                short, 
		                                short, 
		                                XWPSDeviceColor *, 
		                                XWPSDevice *, 
		                                PSColorSelect);
	void (XWPSImagerState::*map_rgb_alpha)(short, 
		                                     short, 
		                                     short, 
		                                     short, 
		                                     XWPSDeviceColor *, 
		                                     XWPSDevice *, 
		                                     PSColorSelect);
};

class XWPSImagerState : public XWPSStruct
{
public:
	XWPSImagerState();
	virtual ~XWPSImagerState();
	
	bool   checkTileCache();
	int    checkTileSize(int w, int y, int h,  PSColorSelect select, int *ppx);
	double charFlatness(float default_scale);
	int  cieJCComplete(XWPSColorSpace *pcs);
	void cmapCMYKDirect(short c, 
	                    short m, 
	                    short y, 
	                    short k, 
	                    XWPSDeviceColor * pdc,
                      XWPSDevice * dev, 
                      PSColorSelect select);
	void cmapCMYKToGray(short c, 
	                    short m, 
	                    short y, 
	                    short k, 
	                    XWPSDeviceColor * pdc,
                      XWPSDevice * dev, 
                      PSColorSelect select);
	void cmapCMYKToRGB(short c, 
	                   short m, 
	                   short y, 
	                   short k, 
	                   XWPSDeviceColor * pdc,
                     XWPSDevice * dev, 
                     PSColorSelect select);
	void cmapGrayDirect(short gray, 
	                    XWPSDeviceColor * pdc, 
		                  XWPSDevice * dev, 
		                  PSColorSelect select);
	void cmapGrayToCMYKDirect(short gray, 
	                          XWPSDeviceColor * pdc,
                            XWPSDevice * dev, 
                            PSColorSelect select);
	void cmapGrayToRGBDirect(short gray, 
	                         XWPSDeviceColor * pdc,
                           XWPSDevice * dev, 
                           PSColorSelect select);
	void cmapRGBAlphaDirect(short r, 
	                        short g, 
	                        short b, 
	                        short alphaA, 
	                        XWPSDeviceColor * pdc,
                          XWPSDevice * dev, 
                          PSColorSelect select);
	void cmapRGBAlphaToCMYK(short r, 
	                        short g, 
	                        short b, 
	                        short alphaA,
	                        XWPSDeviceColor * pdc, 
	                        XWPSDevice * dev,
		                      PSColorSelect select);
	void cmapRGBAlpha2GrayDirect(short r, 
	                             short g, 
	                             short b, 
	                             short alpha,
	                             XWPSDeviceColor * pdc, 
	                             XWPSDevice * dev,
			                         PSColorSelect select);
	void cmapRGBDirect(ushort r, 
	                   ushort g, 
	                   ushort b, 
	                   XWPSDeviceColor * pdc,
                     XWPSDevice * dev, 
                     PSColorSelect select);
	void cmapRGBToCMYK(ushort r, 
	                   ushort g, 
	                   ushort b, 
	                   XWPSDeviceColor * pdc,
                     XWPSDevice * dev, 
                     PSColorSelect select);
	void cmapRGBToGrayDirect(ushort r, 
	                         ushort g, ushort b, 
	                         XWPSDeviceColor * pdc,
                           XWPSDevice * dev, 
                           PSColorSelect select);
	short colorCMYKToGray(short c, short m, short y, short k);
	void  colorCMYKToRgb(short c, short m, short y, short k, short * rgb);
	void  colorRGBToCMYK(short r, short g, short b, short * cmyk);
	short colorRGBToGray(short r, short g, short b);
	void ctmOnly(XWPSMatrix * pmat);
	bool currentAccurateCurves() {return accurate_curves;}
	int  currentCurveJoin();
	bool currentDashAdapt();
	uint currentDashLength();
	float currentDashOffset();
	const float * currentDashPattern();
	float currentDotLength();
	bool  currentDotLengthAbsolute();
	float currentFlat() {return flatness;}
	int   currentHalftonePhase(XWPSIntPoint * pphase);
	PSLineCap currentLineCap();
	PSLineJoin currentLineJoin();
	float currentLineWidth();
	XWPSLineParams * currentLineParams() {return &line_params;}
	uint currentLogicalOp() {return log_op;}
	int   currentMatrix(XWPSMatrix * pmat);
	float currentMiterLimit();
	XWPSCieRender * currentColorRendering() {return cie_render;}
	int   currentScreenPhase(XWPSIntPoint * pphase, PSColorSelect select);
	bool  currentStrokeAdjust() {return stroke_adjust;}
	
	int devhtInstall(XWPSDeviceHalftone * pdht,
	                 PSHalftoneType type, 
			             const XWPSDevice * dev);
	XWPSColorSpace *deviceCMYK();
	XWPSColorSpace *deviceGray();
	XWPSColorSpace *deviceRGB();
	int             dotOrientation();
	
	int ensurePatternCache();
	
	const PSColorMapProcs * getCmapProcs(XWPSDevice * dev);	
	const PSColorMapProcs * getCmapProcsDefault(XWPSDevice * dev);
	
	virtual int getLength();
	virtual const char * getTypeName();
	
	virtual int idtransform(double dx, double dy, XWPSPoint * pt);
	
	void initRop() {log_op = lop_default;}
	
	float joinExpansionFactor(PSLineJoin join);
	
	void mapCMYK(short c, 
		           short m, 
		           short y, 
		           short k, 
		           XWPSDeviceColor * pdc, 
		           XWPSDevice * dev, 
		           PSColorSelect select);		           
	void mapGray(short gray, 
		           XWPSDeviceColor * pdc, 
		           XWPSDevice * dev, 
		           PSColorSelect select);
	void mapGrayNo(short pixel, 
		           XWPSDeviceColor * pdc, 
		           XWPSDevice * dev, 
		           PSColorSelect select);
	void mapRGB(ushort r, 
		          ushort g, 
		          ushort b , 
		          XWPSDeviceColor * pdc, 
		          XWPSDevice * dev, 
		          PSColorSelect select);
	void mapRGBAlpha(short r, 
		          short g, 
		          short b , 
		          short a,
		          XWPSDeviceColor * pdc, 
		          XWPSDevice * dev, 
		          PSColorSelect select);
	
	XWPSPatternCache * patternCache() {return pattern_cache;}
	int patternCacheAddEntry(XWPSDevicePatternAccum * padev, 
	                         XWPSColorTile ** pctile);
	virtual int putDeviceParams(XWPSContextState * ctx, 
	                            XWPSDevice *dev,  
	                            XWPSParamList *plist);
	
	void release();
	int remapCIEABC(XWPSClientColor * pc, 
	                 XWPSColorSpace * pcs,
	                 XWPSDeviceColor * pdc, 
	                 XWPSDevice * dev,
		               PSColorSelect select);
	int remapColorDefault(XWPSClientColor * pcc, 
	                      XWPSColorSpace * pcs,
	                      XWPSDeviceColor * pdc, 
	                      XWPSDevice * dev,
		                    PSColorSelect select);
	int remapColorPattern1(XWPSClientColor * pc, 
	                      XWPSColorSpace * pcs,
	                      XWPSDeviceColor * pdc, 
	                      XWPSDevice * dev,
		                    PSColorSelect select);
	int remapColorPattern2(XWPSClientColor * pcc, 
	                      XWPSColorSpace * pcs,
	                      XWPSDeviceColor * pdc, 
	                      XWPSDevice * dev,
		                    PSColorSelect select);
	int remapConcreteDCMYK(const ushort * pconc,
	                       XWPSDeviceColor * pdc, 
	                       XWPSDevice * dev,
			                   PSColorSelect select);	
	int remapConcreteDGray(const ushort * pconc,
	                       XWPSDeviceColor * pdc, 
	                       XWPSDevice * dev,
			                   PSColorSelect select);
	int remapConcreteDeviceN(const ushort * pconc,
	                          XWPSDeviceColor * pdc, 
	                          XWPSDevice * dev,
			                      PSColorSelect select);
	int remapConcreteDPixel(const ushort * pconc,
	                        XWPSDeviceColor * pdc, 
	                        XWPSDevice * dev,
			                    PSColorSelect select);
	int remapConcreteDRGB(const ushort * pconc,
	                       XWPSDeviceColor * pdc, 
	                       XWPSDevice * dev,
			                   PSColorSelect select);
	int remapConcreteSeparation(const ushort * pconc,
	                             XWPSDeviceColor * pdc, 
	                             XWPSDevice * dev,
			                         PSColorSelect select);
	int remapDeviceCMYK(XWPSClientColor * pc, 
	                    XWPSColorSpace * pcs,
	                    XWPSDeviceColor * pdc, 
	                    XWPSDevice * dev,
		                  PSColorSelect select);
	int remapDeviceGray(XWPSClientColor * pc, 
	                    XWPSColorSpace * pcs,
	                    XWPSDeviceColor * pdc, 
	                    XWPSDevice * dev,
		                  PSColorSelect select);
	
	int remapDeviceRGB(XWPSClientColor * pc, 
	                    XWPSColorSpace * pcs,
	                    XWPSDeviceColor * pdc, 
	                    XWPSDevice * dev,
		                  PSColorSelect select);	
	int remapPattern(XWPSClientColor * pc, 
	                    XWPSColorSpace * pcs,
	                    XWPSDeviceColor * pdc, 
	                    XWPSDevice * dev,
		                  PSColorSelect select);
			
	void resetLineParams();
	
	void setAccurateCurves(bool accurate) {accurate_curves = accurate;}
	void setCmapProcs(XWPSDevice * dev);
	void setCurveJoin(int join);
	int  setDash(const float *patternA, uint lengthA, float offsetA);
	void setDashAdapt(bool a);
	int  setDotLength(float length, bool absolute);
	int  setDotOrientation();
	void setEffectiveTransfer();
	void  setEffectiveXFer();
	void setFillAdjust(float adjust_x, float adjust_y);
	void setFlat(float flat);	
	void setLineCap(PSLineCap cap);
	void setLineJoin(PSLineJoin join);
	void setLineWidth(double width);
	void setLogicalOp(uint o) {log_op=o;}
	virtual int  setMatrix(XWPSMatrix * pmat);
	void  setMiterLimit(float limit);
	void setPatternCache(XWPSPatternCache * pcache) 
		{pattern_cache = pcache;}
	virtual int  setScreenPhase(int x, int y, PSColorSelect select);
	void setStrokeAdjust(bool a) {stroke_adjust=a;}
	int  strokeAddImager(XWPSPath *ppath, 
	                     XWPSPath *to_path,
		                   XWPSDevice *dev);
	int  strokePathExpansion(XWPSPath * ppath, XWPSFixedPoint * ppt);
	
public:
	void * client_data;
	XWPSImagerStateShared * shared;
	XWPSLineParams line_params;
	XWPSMatrixFixed ctm;
	uint log_op;
	ushort alpha;
	PSBlendMode blend_mode;
	XWPSTransparencySource opacity, shape;
	bool text_knockout;
	XWPSTransparencyState *transparency_stack;
	bool overprint;
	int overprint_mode;
	float flatness;
	XWPSFixedPoint fill_adjust;
	bool stroke_adjust;
	bool accurate_curves;
	float smoothness;
	
	const PSColorMapProcs * (XWPSImagerState::*get_cmap_procs)(XWPSDevice *);
	
	XWPSHalftone *halftone;
	XWPSIntPoint screen_phase[ps_color_select_count];
	XWPSDeviceHalftone *dev_ht;
	XWPSHTCache *ht_cache;
	XWPSCieRender *cie_render;
	XWPSTransferMap *black_generation;
	XWPSTransferMap *undercolor_removal;
	XWPSTransfer set_transfer;
	XWPSTransfer effective_transfer;
	XWPSCieJointCaches *cie_joint_caches;
	
	const PSColorMapProcs *cmap_procs;
	
	XWPSPatternCache *pattern_cache;
	XWPSContextState * i_ctx_p;
	bool own_pattern_cache;
	
private:
	void makeBitmap(XWPSStripBitmap * pbm, 
	                XWPSDeviceMem * mdev,
	                ulong id);
};

typedef const uchar * (*sample_unpack_proc_t)(uchar *, 
                                              int *, 
                                              const uchar *, 
                                              int ,
		      																		uint , 
		      																		const PSSampleLookup *, 
		      																		int );


typedef int (XWPSImageEnum::*irender_proc_t)(const uchar *, int, uint, int , XWPSDevice *);
	
typedef irender_proc_t (XWPSImageEnum::*ps_image_class_t)();

struct PSImageType
{
	union
	{
		int (XWPSImageCommon::*begin_typed_image)(XWPSDevice * ,
		                                    XWPSImagerState * , 
		                                    XWPSMatrix * ,
		                                    XWPSIntRect * ,
	                                      XWPSDeviceColor * , 
	                                      XWPSClipPath * ,
		                                    XWPSImageEnumCommon ** );
		int (XWPSDataImage::*begin_typed_imaged)(XWPSDevice * ,
		                                    XWPSImagerState * , 
		                                    XWPSMatrix * ,
		                                    XWPSIntRect * ,
	                                      XWPSDeviceColor * , 
	                                      XWPSClipPath * ,
		                                    XWPSImageEnumCommon ** );
		int (XWPSPixelImage::*begin_typed_imagep)(XWPSDevice * ,
		                                    XWPSImagerState * , 
		                                    XWPSMatrix * ,
		                                    XWPSIntRect * ,
	                                      XWPSDeviceColor * , 
	                                      XWPSClipPath * ,
		                                    XWPSImageEnumCommon ** );
		int (XWPSImage1::*begin_typed_image1)(XWPSDevice * ,
		                                    XWPSImagerState * , 
		                                    XWPSMatrix * ,
		                                    XWPSIntRect * ,
	                                      XWPSDeviceColor * , 
	                                      XWPSClipPath * ,
		                                    XWPSImageEnumCommon ** );
		int (XWPSImage2::*begin_typed_image2)(XWPSDevice * ,
		                                    XWPSImagerState * , 
		                                    XWPSMatrix * ,
		                                    XWPSIntRect * ,
	                                      XWPSDeviceColor * , 
	                                      XWPSClipPath * ,
		                                    XWPSImageEnumCommon ** );
		int (XWPSImage3::*begin_typed_image3)(XWPSDevice * ,
		                                    XWPSImagerState * , 
		                                    XWPSMatrix * ,
		                                    XWPSIntRect * ,
	                                      XWPSDeviceColor * , 
	                                      XWPSClipPath * ,
		                                    XWPSImageEnumCommon ** );
		int (XWPSImage3X::*begin_typed_image3X)(XWPSDevice * ,
		                                    XWPSImagerState * , 
		                                    XWPSMatrix * ,
		                                    XWPSIntRect * ,
	                                      XWPSDeviceColor * , 
	                                      XWPSClipPath * ,
		                                    XWPSImageEnumCommon ** );
		int (XWPSImage4::*begin_typed_image4)(XWPSDevice * ,
		                                    XWPSImagerState * , 
		                                    XWPSMatrix * ,
		                                    XWPSIntRect * ,
	                                      XWPSDeviceColor * , 
	                                      XWPSClipPath * ,
		                                    XWPSImageEnumCommon ** );
	} begin_typed_image_;
	
	union
	{                                    
		int (XWPSImageCommon::*source_size)(XWPSImagerState * , XWPSIntPoint * );
		int (XWPSDataImage::*source_sized)(XWPSImagerState * , XWPSIntPoint * );
		int (XWPSPixelImage::*source_sizep)(XWPSImagerState * , XWPSIntPoint * );
		int (XWPSImage1::*source_size1)(XWPSImagerState * , XWPSIntPoint * );
		int (XWPSImage2::*source_size2)(XWPSImagerState * , XWPSIntPoint * );
		int (XWPSImage3::*source_size3)(XWPSImagerState * , XWPSIntPoint * );
		int (XWPSImage3X::*source_size3X)(XWPSImagerState * , XWPSIntPoint * );
		int (XWPSImage4::*source_size4)(XWPSImagerState * , XWPSIntPoint * );
	} source_size_;
	
	union
	{
		int (XWPSImageCommon::*sput)(XWPSStream *, XWPSColorSpace **);
		int (XWPSDataImage::*sputd)(XWPSStream *, XWPSColorSpace **);
		int (XWPSPixelImage::*sputp)(XWPSStream *, XWPSColorSpace **);
		int (XWPSImage1::*sput1)(XWPSStream *, XWPSColorSpace **);
		int (XWPSImage2::*sput2)(XWPSStream *, XWPSColorSpace **);
		int (XWPSImage3::*sput3)(XWPSStream *, XWPSColorSpace **);
		int (XWPSImage3X::*sput3X)(XWPSStream *, XWPSColorSpace **);
		int (XWPSImage4::*sput4)(XWPSStream *, XWPSColorSpace **);
	} sput_;
	
	union
	{
		int (XWPSImageCommon::*sget)(XWPSStream *, XWPSColorSpace *);
		int (XWPSDataImage::*sgetd)(XWPSStream *, XWPSColorSpace *);
		int (XWPSPixelImage::*sgetp)(XWPSStream *, XWPSColorSpace *);
		int (XWPSImage1::*sget1)(XWPSStream *, XWPSColorSpace *);
		int (XWPSImage2::*sget2)(XWPSStream *, XWPSColorSpace *);
		int (XWPSImage3::*sget3)(XWPSStream *, XWPSColorSpace *);
		int (XWPSImage3X::*sget3X)(XWPSStream *, XWPSColorSpace *);
		int (XWPSImage4::*sget4)(XWPSStream *, XWPSColorSpace *);
	} sget_;
	int index;
};

class XWPSImageCommon
{
public:
	XWPSImageCommon();
	virtual ~XWPSImageCommon() {}
	
	int  beginTyped(XWPSState * pgs,  
	                bool uses_color, 
	                XWPSImageEnumCommon ** ppie);
	int  beginTypedImage(XWPSDevice * dev,
		                   XWPSImagerState * pis, 
		                   XWPSMatrix * pmat,
		                   XWPSIntRect * prect,
	                     XWPSDeviceColor * pdcolor, 
	                     XWPSClipPath * pcpath,
		                   XWPSImageEnumCommon **pinfo);
	
	void commonInit();
	
	int sget(XWPSStream *s,	 XWPSColorSpace *cs);
	int sgetNo(XWPSStream *,	 XWPSColorSpace *);
	int sourceSize(XWPSImagerState * pis, XWPSIntPoint * psize);
	int sput(XWPSStream *s, XWPSColorSpace **cs);
	int sputNo(XWPSStream *, XWPSColorSpace **);
	
public:
	PSImageType type;
	XWPSMatrix ImageMatrix;
};

class XWPSDataImage : public XWPSImageCommon
{
public:
	XWPSDataImage();
	virtual ~XWPSDataImage() {}
	
	void dataInit(int num_components);
	
	int dataSourceSize(XWPSImagerState * , XWPSIntPoint * psize);
	
	bool matrixIsDefault();
	void matrixSetDefault();
	
public:
	int Width;
	int Height;
	int BitsPerComponent;
	float Decode[PS_IMAGE_MAX_COMPONENTS * 2];
	bool Interpolate;
};

class XWPSPixelImage : public XWPSDataImage
{
public:
	XWPSPixelImage();
	virtual ~XWPSPixelImage() {}
	
	void pixelInit(XWPSColorSpace * color_space);
	int  pixelImageSGet(XWPSStream *s, XWPSColorSpace *pcs);
	int  pixelImageSPut(XWPSStream *s,  XWPSColorSpace **ppcs, int extra);
	
public:
	PSImageFormat format;
	XWPSColorSpace *ColorSpace;
	bool CombineWithColor;
};

#define MI_ImageMatrix 0x01
#define MI_Decode 0x02
#define MI_Interpolate 0x04
#define MI_adjust 0x08
#define MI_Alpha_SHIFT 4
#define MI_Alpha_MASK 0x3
#define MI_BPC_SHIFT 6
#define MI_BPC_MASK 0xf
#define MI_BITS 10

class XWPSImage1 : public XWPSPixelImage
{
public:
	XWPSImage1();
		                     
	int  beginImage1(XWPSDevice * dev,
									 XWPSImagerState * pis, 
									 XWPSMatrix * pmat,
									 XWPSIntRect * prect,
									 XWPSDeviceColor * pdcolor, 
									 XWPSClipPath * pcpath,
									 XWPSImageEnumCommon ** pinfo);
	void initAdjust(XWPSColorSpace * color_space, bool adjustA);
	void initMask(bool write_1s);
	void initMaskAdjust(bool write_1s, bool adjustA);
	int  image1MaskSGet(XWPSStream *s, XWPSColorSpace *ignore_pcs);
	int  image1MaskSPut(XWPSStream *s,  XWPSColorSpace **ignore_ppcs);
	int  image1SGet(XWPSStream *s,  XWPSColorSpace *pcs);
	int  image1SPut(XWPSStream *s, XWPSColorSpace **ppcs);
	
	void makeBitmapImage(int x, int y, int w, int h);
	
	XWPSImage1 & operator=(XWPSImage1 & other);
	
public:
	bool ImageMask;
	bool adjust;
	PSImageAlpha Alpha;
};

class XWPSimage2Data
{
public:
	XWPSimage2Data(){}
	
public:
	XWPSPoint origin;
  XWPSIntRect bbox;
  XWPSImage1 image;
};

class XWPSImage2 : public XWPSImageCommon
{
public:
	XWPSImage2();
	~XWPSImage2();
	
	int beginImage2(XWPSDevice * dev,
		              XWPSImagerState * pis, 
		              XWPSMatrix * pmat,
		              XWPSIntRect * prect,
	                XWPSDeviceColor * pdcolor, 
	                XWPSClipPath * pcpath,
		              XWPSImageEnumCommon ** pinfo);
	
	int image2SetData(XWPSimage2Data * pid);
	int image2SourceSize(XWPSImagerState * pis, XWPSIntPoint * psize);
	
public:
	XWPSState *DataSource;
  float XOrigin, YOrigin;
  float Width, Height;
  XWPSPath *UnpaintedPath;
  bool PixelCopy;
};


#define IMAGE3X_IMAGETYPE 103

typedef int (*image3_make_mid_proc_t)(XWPSDevice **, XWPSDevice *, int , int );
typedef int (*image3_make_mcde_proc_t)(XWPSDevice *,
	       															XWPSImagerState *,
	       															XWPSMatrix *,
	       															XWPSImageCommon *,
	       															XWPSIntRect *,
	       															XWPSDeviceColor *,
	       															XWPSClipPath *, 
	       															XWPSImageEnumCommon **,
	       															XWPSDevice **, 
	       															XWPSDevice *,
	       															XWPSImageEnumCommon *,
	       															XWPSIntPoint *);
	       															
class XWPSImage3  : public XWPSPixelImage
{
public:
	XWPSImage3();
	
	int beginImage3(XWPSDevice * dev,
									 XWPSImagerState * pis, 
									 XWPSMatrix * pmat,
									 XWPSIntRect * prect,
									 XWPSDeviceColor * pdcolor, 
									 XWPSClipPath * pcpath,
									 XWPSImageEnumCommon ** pinfo);
									 
	int  beginImage3Generic(XWPSDevice * dev,
			                    XWPSImagerState *pis, 
			                    XWPSMatrix *pmat,
			                    XWPSIntRect *prect,
			                    XWPSDeviceColor *pdcolor,
			                    XWPSClipPath *pcpath, 
			                    image3_make_mid_proc_t make_mid,
			                    image3_make_mcde_proc_t make_mcde,
			                    XWPSImageEnumCommon **pinfo);
	
	void image3Init(XWPSColorSpace * color_space,
		 							PSImage3InterleaveType interleave_typeA);
	
public:
	int InterleaveType;
  XWPSDataImage MaskDict;
};

typedef int (*image3x_make_mid_proc_t)(XWPSDevice **, XWPSDevice *, int , int , int);
typedef int (*image3x_make_mcde_proc_t)(XWPSDevice *,
	       																XWPSImagerState *,
	       																XWPSMatrix *,
	       																XWPSImageCommon *,
	       																XWPSIntRect *,
	       																XWPSDeviceColor *,
	       																XWPSClipPath *, 
	       																XWPSImageEnumCommon **,
	       																XWPSDevice **, 
	       																XWPSDevice ** ,
	       																XWPSImageEnumCommon ** ,
	       																XWPSIntPoint * ,
	       																XWPSImage3X *);

class XWPSImage3xMask
{
public:
	XWPSImage3xMask();
	
public:
	int InterleaveType;
  float Matte[PS_CLIENT_COLOR_MAX_COMPONENTS];
  bool has_Matte;
  XWPSDataImage MaskDict;
};


class XWPSImage3XChannelState
{
public:
	XWPSImage3XChannelState();
	~XWPSImage3XChannelState();
	
	int channelNext(XWPSImage3XChannelState *pics2);
	
public:
	XWPSImageEnumCommon *info;
	XWPSDevice *mdev;
	PSImage3InterleaveType InterleaveType;
  int width, height, full_height, depth;
  uchar *data;
  int y;
  int skip;	
  bool own_data;
};


class XWPSImage3XChannelValues
{
public:
	XWPSImage3XChannelValues() {}
	~XWPSImage3XChannelValues();
	
public:
	XWPSMatrix matrix;
  XWPSPoint corner;
  XWPSIntRect rect;
  XWPSImage1 image;
};


class XWPSImage3X : public XWPSPixelImage
{
public:
	XWPSImage3X();
	
	int beginImage3x(XWPSDevice * dev,
		               XWPSImagerState * pis, 
		               XWPSMatrix * pmat,
		               XWPSIntRect * prect,
		               XWPSDeviceColor * pdcolor, 
		               XWPSClipPath * pcpath,
		               XWPSImageEnumCommon ** pinfo);
	
	int beginImage3xGeneric(XWPSDevice * dev,
													XWPSImagerState *pis, 
													XWPSMatrix *pmat,
													XWPSIntRect *prect,
													XWPSDeviceColor *pdcolor,
													XWPSClipPath *pcpath, 
													image3x_make_mid_proc_t make_mid,
													image3x_make_mcde_proc_t make_mcde,
													XWPSImageEnumCommon **pinfo);
	
	int checkImage3xMask(XWPSImage3xMask *pimm,
		   									XWPSImage3XChannelValues *ppcv,
		                    XWPSImage3XChannelValues *pmcv,
		                    XWPSImage3XChannelState *pmcs);
	void image3xInit(XWPSColorSpace * color_space);
	
public:
	XWPSImage3xMask Opacity, Shape;
};

class XWPSImage4  : public XWPSPixelImage
{
public:
	XWPSImage4();
	
	int beginImage4(XWPSDevice * dev,
									XWPSImagerState * pis, 
									XWPSMatrix * pmat,
		              XWPSIntRect * prect,
		              XWPSDeviceColor * pdcolor, 
		              XWPSClipPath * pcpath,
		              XWPSImageEnumCommon ** pinfo);
	
	void image4Init(XWPSColorSpace * color_space);
	int  image4SGet(XWPSStream *s, XWPSColorSpace *pcs);
	int  image4SPut(XWPSStream *s,  XWPSColorSpace **ppcs);
	
public:
	bool MaskColor_is_range;
  uint MaskColor[PS_IMAGE_MAX_COMPONENTS * 2];
};

struct PSImageEnumProcs
{
	union
	{
		int (XWPSImageEnumCommon::*plane_data)(const PSImagePlane *, int, int *);
		int (XWPSImageEnum::*plane_data1)(const PSImagePlane *, int, int *);
		int (XWPSImage3Enum::*plane_data3)(const PSImagePlane *, int, int *);
		int (XWPSImage3XEnum::*plane_data3X)(const PSImagePlane *, int, int *);
		int (XWPSBboxImageEnum::*plane_databbox)(const PSImagePlane *, int, int *);
		int (XWPSPDFImageEnum::*plane_datapdf)(const PSImagePlane *, int, int *);
		int (XWPSDVIImageEnum::*plane_datadvi)(const PSImagePlane *, int, int *);
	}plane_data_;
	
	union
	{
		int (XWPSImageEnumCommon::*end_image)(bool);
		int (XWPSImageEnum::*end_image1)(bool);
		int (XWPSImage3Enum::*end_image3)(bool);
		int (XWPSImage3XEnum::*end_image3X)(bool);
		int (XWPSBboxImageEnum::*end_imagebbox)(bool);
		int (XWPSPDFImageEnum::*end_imagepdf)(bool);
		int (XWPSDVIImageEnum::*end_imagedvi)(bool);
	}end_image_;
	
	union
	{
		int (XWPSImageEnumCommon::*flush)();
		int (XWPSImageEnum::*flush1)();
		int (XWPSImage3Enum::*flush3)();
		int (XWPSImage3XEnum::*flush3X)();
		int (XWPSBboxImageEnum::*flushbbox)();
		int (XWPSPDFImageEnum::*flushpdf)();
		int (XWPSDVIImageEnum::*flushdvi)();
	}flush_;
	
	union
	{
		bool (XWPSImageEnumCommon::*planes_wanted)(uchar*);
		bool (XWPSImageEnum::*planes_wanted1)(uchar*);
		bool (XWPSImage3Enum::*planes_wanted3)(uchar*);
		bool (XWPSImage3XEnum::*planes_wanted3X)(uchar*);
		bool (XWPSBboxImageEnum::*planes_wantedbbox)(uchar*);
		bool (XWPSPDFImageEnum::*planes_wantedpdf)(uchar*);
		bool (XWPSDVIImageEnum::*planes_wanteddvi)(uchar*);
	}planes_wanted_;
};

class XWPSImageEnumCommon
{
public:
	XWPSImageEnumCommon();
	virtual ~XWPSImageEnumCommon();
	
	void commonInit(XWPSDataImage * pic,
			  				  XWPSDevice * devA, 
			  				  int num_componentsA,
			            PSImageFormat formatA);
			  
	int end(bool draw_last);
	int endIgnore(bool ) {return 0;}
			  
	int flush();
	
	int imageData(const uchar ** plane_data, int data_x, uint raster, int height);
	
	int  planeData(const PSImagePlane * planes, int height);
	int  planeDataNo(const PSImagePlane * , int , int *);
	int  planeDataRows(const PSImagePlane * planes, int height,int *rows_used);
	bool planesWanted(uchar *wanted);
	
public:
	PSImageType *image_type;
	PSImageEnumProcs procs;
	
	XWPSDevice *dev;
	ulong id;
	int num_planes;
	int plane_depths[ps_image_max_planes];
	int plane_widths[ps_image_max_planes];
};

class XWPSImageClue
{
public:
	XWPSImageClue();
	
public:
	XWPSDeviceColor dev_color;
  ulong key;
};

#define image_clip_xmin 1
#define image_clip_xmax 2
#define image_clip_ymin 4
#define image_clip_ymax 8
#define image_clip_region 0x10

class XWPSImageEnum : public XWPSImageEnumCommon
{
public:
	XWPSImageEnum();
	XWPSImageEnum(const XWPSPixelImage * pim, const XWPSIntRect * prect);
	~XWPSImageEnum();
	
	irender_proc_t imageClass0Interpolate();
	irender_proc_t imageClass1Simple();
	irender_proc_t imageClass2Fracs();
	irender_proc_t imageClass3Mono();
	irender_proc_t imageClass4Color();
	int image1beginImage(XWPSDevice * devA, 
	          XWPSImagerState * pisA,
		        XWPSMatrix *pmat, 
		        XWPSImageCommon * pic,
		        XWPSDeviceColor * pdcolor, 
		        XWPSClipPath * pcpath);
	
	int image1EndImage(bool draw_last);
	int image1Flush();
	int image1PlaneData(const PSImagePlane * planes, 
	                    int height, 
	                    int *rows_used);
	int imageRenderColor(const uchar *bufferA, 
	                    int data_x,
		                  uint w, 
		                  int h, 
		                  XWPSDevice * devA);
	int imageRenderFrac(const uchar * bufferA, 
	                    int data_x,
		                  uint w, 
		                  int h, 
		                  XWPSDevice * devA);
	int imageRenderInterpolate(const uchar * bufferA,
			 									     int data_x, 
			 									     uint iw, 
			 									     int h, 
			 									     XWPSDevice * devA);
	int imageRenderLandscape(const uchar * bufferA,
			 									     int data_x, 
			 									     uint iw, 
			 									     int h, 
			 									     XWPSDevice * devA);
	int imageRenderMono(const uchar * bufferA, 
	                    int data_x,
		                  uint w, 
		                  int h, 
		                  XWPSDevice * devA);
	int imageRenderSimple(const uchar * buffer, 
	                     int data_x,
		                   uint w, 
		                   int h, 
		                   XWPSDevice * devA);
	int imageRenderSkip(const uchar * bufferA, 
	                    int data_x,
		                  uint w, 
		                  int h, 
		                  XWPSDevice * devA);
	                    
	void scaleMaskColors(int component_index);
	
public:
	uchar bps;
	uchar unpack_bps;
	uchar log2_xbytes;
	uchar spp;
	PSImageAlpha alpha;
	struct mc_ 
	{
		uint values[PS_IMAGE_MAX_COMPONENTS * 2];
		ulong mask, test;	
		bool exact;
  } mask_color;
  uchar use_mask_color;
  uchar spread;
  uchar masked;
  uchar interpolate;
  XWPSMatrix matrix;
  struct r_ 
  {
		int x, y, w, h;
  } rect;
  XWPSFixedPoint x_extent, y_extent;
  sample_unpack_proc_t unpack;
  XWPSImagerState *pis;
  irender_proc_t render;
  XWPSColorSpace *pcs;
  uchar *buffer;
  uint buffer_size;
  uchar *line;	
  uint line_size;
  uint line_width;
  PSImagePosture posture;
  uchar use_rop;
  uchar clip_image;
  uchar slow_loop;
  uchar device_color;
  XWPSFixedRect clip_outer;
  XWPSFixedRect clip_inner;
  ulong log_op;
  long adjust;
  long dxx, dxy;
  XWPSDeviceClip *clip_dev;
  XWPSDeviceRopTexture *rop_dev;
  PSStreamImageScaleState *scaler;
  int y;
  XWPSIntPoint used;
  XWPSFixedPoint cur, prev;
  struct dd_ 
  {
		PSDDAFixedPoint row;
		PSDDAFixedPoint strip;
		PSDDAFixedPoint pixel0;	
  } dda;
  
  int line_xy;
  int xi_next;
  XWPSIntPoint xyi;
  int yci, hci;
  int xci, wci;	
  PSSampleMap map[PS_IMAGE_MAX_COMPONENTS];
  XWPSImageClue clues[256];
  
private:
	void clear();
	static int color_draws_b_w(XWPSDevice * devA, XWPSDeviceColor * pdcolor);
	int copyLandscape(int x0, int x1, bool y_neg, XWPSDevice * devA);
	int copyPortrait(const uchar * data, 
	                 int dx, int raster,
	                 int x, 
	                 int y, 
	                 int w, 
	                 int h,
	                  XWPSDevice * devA);
	
	static void fill_row(uchar *line, int line_x, uint raster, int value);
	
	static void image_simple_expand(uchar * line, 
	                                int line_x, 
	                                uint raster,
		                              const uchar * buffer, 
		                              int data_x, 
		                              uint w,
		    													long xcur, 
		    													long x_extent, 
		    													uchar zero);
	void initColors(int bpsA, 
								  int sppA,
		  					 PSImageFormat formatA, 
		  					 const float *decode ,
		  					 XWPSImagerState * pisA, 
		  					 XWPSDevice * devA,
		  					XWPSColorSpace * pcsA, 
		  					bool * pdcb);
	static void initMap(uchar * map, int map_size, const float *decode);
	bool maskColor12Matches(const short *v, int num_components);
	bool maskColorMatches(const uchar *v, int num_components);
	static void repackBitPlanes(const PSImagePlane *src_planes, 
	                            const ulong *offsets,
		                          int num_planes, 
		                          uchar *buffer, 
		                          int width,
		                          const PSSampleLookup * ptab, 
		                          int spread);
	XWPSDevice * setupImageDevice();
	
	void updateStrip();
};

class XWPSImageEnumPlane
{
public:
	XWPSImageEnumPlane();
	~XWPSImageEnumPlane();
	
public:
	XWPSString row;
	uint pos;
	XWPSString source;
};

class XWPSImageEnums : public XWPSStruct
{
public:
	XWPSImageEnums();
	~XWPSImageEnums();
	
	virtual int getLength();
	virtual const char * getTypeName();
	
	int imageCommonInit(XWPSImageEnumCommon * pie,
	                     const XWPSDataImage * pim, 
	                     XWPSDevice * devA);
	int imageEnumInit(XWPSImageEnumCommon * pie,
		                const XWPSDataImage * pim, 
		                XWPSState *pgs);
	int  imageInit(XWPSImage1 * pim, bool multi, XWPSState * pgs);
	int  imageNext(const uchar * dbytes, uint dsize, uint * pused);
	int  imageNextPlanes(XWPSString *plane_data, uint *used);
	const uchar * imagePlanesWanted();
	void init();
	
public:
	XWPSDevice *dev;
	XWPSImageEnumCommon *info;
	int num_planes;
  int height;
  bool wanted_varies;
  int plane_index;
  int y;
  bool error;
  uchar wanted[ps_image_max_planes];
  uchar client_wanted[ps_image_max_planes];
  XWPSImageEnumPlane planes[ps_image_max_planes];
  PSImagePlane image_planes[ps_image_max_planes];
  
private:
	void beginPlanes();
	void cachePlanes();
	void freeRowBuffers(int num_planesA);
	uint imageBytesPerPlaneRow(int plane);
	void nextPlane();
};

class XWPSImage3Enum : public XWPSImageEnumCommon
{
public:
	XWPSImage3Enum();
	~XWPSImage3Enum();
	
	int image3EndImage(bool draw_last);
	int image3Flush();
	int image3PlaneData(const PSImagePlane * planes, int height, int *rows_used);
	bool image3PlanesWanted(uchar *wanted);
	
	int planesNext();
	
public:
	XWPSDevice *mdev;
	XWPSDevice *pcdev;
	XWPSImageEnumCommon *mask_info;
  XWPSImageEnumCommon *pixel_info;
  PSImage3InterleaveType InterleaveType;
  int num_components;
  int bpc;
  int mask_width, mask_height, mask_full_height;
  int pixel_width, pixel_height, pixel_full_height;
  uchar *mask_data;
  uchar *pixel_data;
  int mask_y;
  int pixel_y;
  int mask_skip;
};

#define NUM_MASKS 2	

class XWPSImage3XEnum : public XWPSImageEnumCommon
{
public:
	XWPSImage3XEnum();
	~XWPSImage3XEnum();
	
	int  image3xEndImage(bool draw_last);
	int  image3xFlush();
	int  image3xPlaneData(const PSImagePlane * planes, int height, int *rows_used);
	bool image3xPlanesWanted(uchar *wanted);
	
public:
	XWPSDevice *pcdev;
	int num_components;
	int bpc;
	XWPSImage3XChannelState mask[NUM_MASKS], pixel;
};

class XWPSBboxImageEnum : public XWPSImageEnumCommon
{
public:
	XWPSBboxImageEnum();
	~XWPSBboxImageEnum();
	
	static int alloc(XWPSImagerState * pis, 
	                  XWPSMatrix * pmat,
		 								XWPSImageCommon * pic, 
		 								XWPSIntRect * prect,
		 								XWPSClipPath * pcpathA,
		 								XWPSBboxImageEnum ** ppbe);
		
	int bboxImageEndImage(bool draw_last);
	int bboxImageFlush();
	int bboxImagePlaneData(const PSImagePlane * planes, 
	                       int heightA,
		                     int *rows_used);
	bool bboxImagePlanesWanted(uchar *wanted);
		                     
	void copyTargetInfo();
	
public:
	XWPSMatrix matrix;
	XWPSClipPath *pcpath;
  XWPSImageEnumCommon *target_info;
  bool params_are_const;
  int x0, x1;
  int y, height;
};

class XWPSPDFImageEnum : public XWPSImageEnumCommon
{
public:
	XWPSPDFImageEnum();
	~XWPSPDFImageEnum();
	
	int pdfImageEndImage(bool draw_last);
	int pdfImageEndImageData(bool draw_last, bool do_image);
	int pdfImageEndImageObject(bool draw_last);
	int pdfImagePlaneData(const PSImagePlane * planes, int height, int *rows_used);
	
public:
	int width;
  int bits_per_pixel;
  int rows_left;
  XWPSPDFImageWriter * writer;
  XWPSMatrix mat;
};

class XWPSDVIImageEnum : public XWPSImageEnumCommon
{
public:
	XWPSDVIImageEnum();
	~XWPSDVIImageEnum();
	
	int dviImageEndImage(bool draw_last);
	int dviImageEndImageData(bool draw_last, bool do_image);
	int dviImageEndImageObject(bool draw_last);
	int dviImagePlaneData(const PSImagePlane * planes, int height, int *rows_used);
	
public:
	int width;
  int bits_per_pixel;
  int rows_left;
  XWPSDVIImageWriter * writer;
  XWPSMatrix mat;
};

#endif //XWPSCOLORSPACE_H
