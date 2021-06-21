/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWCOLORSPACE_H
#define XWCOLORSPACE_H

#include "XWObject.h"
#include "XWFunction.h"

class XWArray;
class XWCoreFont;
class XWPDFRectangle;
class XWShading;

#define COLOR_SPACE_DEVICEGRAY 0
#define COLOR_SPACE_CALGRAY    1
#define COLOR_SPACE_DEVICERGB  2
#define COLOR_SPACE_CALRGB     3
#define COLOR_SPACE_DEVICECMYK 4
#define COLOR_SPACE_LAB        5
#define COLOR_SPACE_ICCBASED   6
#define COLOR_SPACE_INDEXED    8
#define COLOR_SPACE_SEPARATION 9
#define COLOR_SPACE_DEVICEN    10
#define COLOR_SPACE_PATTERN    11


#define COLOR_COMP1 0x10000
#define COLOR_MAX_COMPS 32


struct XW_CORE_EXPORT CoreColor
{
    int c[COLOR_MAX_COMPS];
};

typedef int CoreGray;


struct XW_CORE_EXPORT CoreRGB 
{
    int r, g, b;
};


struct XW_CORE_EXPORT CoreCMYK 
{
    int c, m, y, k;
};


inline int dblToCol(double x) 
{
    return (int)(x * COLOR_COMP1);
}

inline double colToDbl(int x) 
{
    return (double)x / (double)COLOR_COMP1;
}

inline int byteToCol(uchar x)
{
    return (int)((x << 8) + x + (x >> 7));
}

inline uchar colToByte(int x)
{
    return (uchar)(((x << 8) - x + 0x8000) >> 16);
}


class XW_CORE_EXPORT XWColorSpace
{
public:
    XWColorSpace();
    virtual ~XWColorSpace();
    
    int    clip01(int x)
           {return (x < 0) ? 0 : (x > COLOR_COMP1) ? COLOR_COMP1 : x;}
    double clip01(double x)
           {return (x < 0) ? 0 : (x > 1) ? 1 : x;}
           
    virtual XWColorSpace *copy() = 0;
        
    virtual void   getCMYK(CoreColor *color, CoreCMYK *cmyk) = 0;
    static  const  char * getColorSpaceModeName(int idx);
    virtual void   getDefaultColor(CoreColor *color) = 0;
    virtual void   getDefaultRanges(double *decodeLow, double *decodeRange, int);
    virtual void   getGray(CoreColor *color, CoreGray *gray) = 0;
    virtual int    getMode() = 0;
    virtual int    getNComps() = 0;
    static  int    getNumColorSpaceModes();
    virtual void   getRGB(CoreColor *color,  CoreRGB *rgb) = 0;     
    
    uint getOverprintMask() { return overprintMask; }  
    
    virtual bool isNonMarking() { return false; } 
    
    static XWColorSpace *parse(XWObject *csObj, int recursion = 0);
    
protected:
  uint overprintMask;
};

class XW_CORE_EXPORT XWDeviceGrayColorSpace : public XWColorSpace
{
public:
    XWDeviceGrayColorSpace();
    virtual ~XWDeviceGrayColorSpace();
    
    virtual XWColorSpace *copy();
    
    virtual void   getCMYK(CoreColor *color, CoreCMYK *cmyk);
    virtual void   getDefaultColor(CoreColor *color);
    virtual void   getGray(CoreColor *color, CoreGray *gray);
    virtual int    getMode() { return COLOR_SPACE_DEVICEGRAY; }
    virtual int    getNComps() { return 1; }
    virtual void   getRGB(CoreColor *color,  CoreRGB *rgb);
};

class XW_CORE_EXPORT XWCalGrayColorSpace : public XWColorSpace
{
public:
    XWCalGrayColorSpace();
    virtual ~XWCalGrayColorSpace();
    
    virtual XWColorSpace *copy();
    
    double getBlackX() { return blackX; }
  	double getBlackY() { return blackY; }
  	double getBlackZ() { return blackZ; }
    virtual void getCMYK(CoreColor *color, CoreCMYK *cmyk);
    virtual void getDefaultColor(CoreColor *color);
    double getGamma() { return gamma; }
    virtual void getGray(CoreColor *color, CoreGray *gray);
    virtual int  getMode() { return COLOR_SPACE_CALGRAY; }
    virtual int  getNComps() { return 1; }
    virtual void getRGB(CoreColor *color,  CoreRGB *rgb);
    double getWhiteX() { return whiteX; }
  	double getWhiteY() { return whiteY; }
  	double getWhiteZ() { return whiteZ; }
        
    static XWColorSpace * parse(XWArray *arr, int recursion);
    
public:
    double whiteX, whiteY, whiteZ;
    double blackX, blackY, blackZ;
    double gamma;
};

class XW_CORE_EXPORT XWDeviceRGBColorSpace : public XWColorSpace
{
public:
    XWDeviceRGBColorSpace();
    virtual ~XWDeviceRGBColorSpace();
    
    virtual XWColorSpace *copy();
    
    virtual void getCMYK(CoreColor *color, CoreCMYK *cmyk);
    virtual void getDefaultColor(CoreColor *color);
    virtual void getGray(CoreColor *color, CoreGray *gray);
    virtual int  getMode() { return COLOR_SPACE_DEVICERGB; }
    virtual int  getNComps() { return 3; }
    virtual void getRGB(CoreColor *color,  CoreRGB *rgb);
};

class XW_CORE_EXPORT XWCalRGBColorSpace : public XWColorSpace
{
public:
    XWCalRGBColorSpace();
    virtual ~XWCalRGBColorSpace();
    
    virtual XWColorSpace *copy();
    
    double getBlackX() { return blackX; }
  	double getBlackY() { return blackY; }
  	double getBlackZ() { return blackZ; }
    virtual void getCMYK(CoreColor *color, CoreCMYK *cmyk);
    virtual void getDefaultColor(CoreColor *color);
    double getGammaR() { return gammaR; }
  	double getGammaG() { return gammaG; }
  	double getGammaB() { return gammaB; }
    virtual void getGray(CoreColor *color, CoreGray *gray);
    double * getMatrix() { return mat; }
    virtual int  getMode() { return COLOR_SPACE_CALRGB; }
    virtual int  getNComps() { return 3; }
    virtual void getRGB(CoreColor *color,  CoreRGB *rgb);
    double getWhiteX() { return whiteX; }
  	double getWhiteY() { return whiteY; }
  	double getWhiteZ() { return whiteZ; }
        
    static XWColorSpace *parse(XWArray *arr, int recursion);
    
public:
    double whiteX, whiteY, whiteZ;
    double blackX, blackY, blackZ;
    double gammaR, gammaG, gammaB;
    double mat[9];
};

class XW_CORE_EXPORT XWDeviceCMYKColorSpace : public XWColorSpace
{
public:
    XWDeviceCMYKColorSpace();
    virtual ~XWDeviceCMYKColorSpace();
    
    virtual XWColorSpace *copy();
    
    virtual void getCMYK(CoreColor *color, CoreCMYK *cmyk);
    virtual void getDefaultColor(CoreColor *color);
    virtual void getGray(CoreColor *color, CoreGray *gray);
    virtual int  getMode() { return COLOR_SPACE_DEVICECMYK; }
    virtual int  getNComps() { return 4; }
    virtual void getRGB(CoreColor *color,  CoreRGB *rgb);
};

class XW_CORE_EXPORT XWLabColorSpace : public XWColorSpace
{
public:
    XWLabColorSpace();
    virtual ~XWLabColorSpace();
    
    virtual XWColorSpace *copy();
    
    double getAMin() { return aMin; }
  	double getAMax() { return aMax; }
    double getBlackX() { return blackX; }
  	double getBlackY() { return blackY; }
  	double getBlackZ() { return blackZ; }
  	double getBMax() { return bMax; }
  	double getBMin() { return bMin; }
    virtual void getCMYK(CoreColor *color, CoreCMYK *cmyk);
    virtual void getDefaultColor(CoreColor *color);
    virtual void getDefaultRanges(double *decodeLow, double *decodeRange, int);
    virtual void getGray(CoreColor *color, CoreGray *gray);
    virtual int  getMode() { return COLOR_SPACE_LAB; }
    virtual int  getNComps() { return 3; }
    virtual void getRGB(CoreColor *color,  CoreRGB *rgb);
    double getWhiteX() { return whiteX; }
  	double getWhiteY() { return whiteY; }
  	double getWhiteZ() { return whiteZ; }
    
    static XWColorSpace *parse(XWArray *arr, int recursion);
    
public:
    double whiteX, whiteY, whiteZ;
    double blackX, blackY, blackZ;
    double aMin, aMax, bMin, bMax;
    double kr, kg, kb;
};

class XW_CORE_EXPORT XWICCBasedColorSpace : public XWColorSpace
{
public:
    XWICCBasedColorSpace(int nCompsA, XWColorSpace *altA, ObjRef *iccProfileStreamA);
    virtual ~XWICCBasedColorSpace();
    
    virtual XWColorSpace *copy();
    
    XWColorSpace *getAlt() { return alt; }
    virtual void getCMYK(CoreColor *color, CoreCMYK *cmyk);
    virtual void getDefaultColor(CoreColor *color);
    virtual void getDefaultRanges(double *decodeLow, double *decodeRange, int maxImgPixel);
    virtual void getGray(CoreColor *color, CoreGray *gray);
    virtual int  getMode() { return COLOR_SPACE_ICCBASED; }
    virtual int  getNComps() { return nComps; }
    virtual void getRGB(CoreColor *color,  CoreRGB *rgb);
    
    static XWColorSpace *parse(XWArray *arr, int recursion);
    
public:
    int nComps;
    XWColorSpace *alt;
    double rangeMin[4];
    double rangeMax[4];
    ObjRef iccProfileStream;
};

class XW_CORE_EXPORT XWIndexedColorSpace : public XWColorSpace
{
public:
    XWIndexedColorSpace(XWColorSpace *baseA, int indexHighA);
    virtual ~XWIndexedColorSpace();
    
    virtual XWColorSpace *copy();
    
    XWColorSpace *getBase() { return base; }
    virtual void getCMYK(CoreColor *color, CoreCMYK *cmyk);
    virtual void getDefaultColor(CoreColor *color);
    virtual void getDefaultRanges(double *decodeLow, double *decodeRange, int maxImgPixel);
    virtual void getGray(CoreColor *color, CoreGray *gray);
    int getIndexHigh() { return indexHigh; }
    uchar *getLookup() { return lookup; }
    virtual int  getMode() { return COLOR_SPACE_INDEXED; }
    virtual int  getNComps() { return 1; }
    virtual void getRGB(CoreColor *color,  CoreRGB *rgb);
    
    CoreColor * mapColorToBase(CoreColor *color, CoreColor *baseColor);
    
    static XWColorSpace *parse(XWArray *arr, int recursion);
    
public:
    XWColorSpace *base;
    int indexHigh;
    uchar * lookup;
};

class XW_CORE_EXPORT XWSeparationColorSpace : public XWColorSpace
{
public:
    XWSeparationColorSpace(XWString *nameA, XWColorSpace *altA, XWFunction *funcA);
    XWSeparationColorSpace(XWString *nameA, 
                           XWColorSpace *altA,
			                     XWFunction *funcA, 
			                     bool nonMarkingA,
			                     uint overprintMaskA);
    virtual ~XWSeparationColorSpace();
    
    virtual XWColorSpace *copy();
    
    XWColorSpace *getAlt() { return alt; }
    virtual void getCMYK(CoreColor *color, CoreCMYK *cmyk);
    virtual void getDefaultColor(CoreColor *color);
    XWFunction *getFunc() { return func; }
    virtual void getGray(CoreColor *color, CoreGray *gray);
    virtual int  getMode() { return COLOR_SPACE_SEPARATION; }
    XWString *getName() { return name; }
    virtual int  getNComps() { return 1; }
    virtual void getRGB(CoreColor *color,  CoreRGB *rgb);
    
    static XWColorSpace *parse(XWArray *arr, int recursion);
    
public:
    XWString *name;
    XWColorSpace *alt;
    XWFunction *func;
    bool nonMarking;
};


class XW_CORE_EXPORT XWDeviceNColorSpace : public XWColorSpace
{
public:
    XWDeviceNColorSpace(int nCompsA, 
                        XWString **namesA, 
                        XWColorSpace *altA, 
                        XWFunction *funcA);
    XWDeviceNColorSpace(int nCompsA, 
                        XWString **namesA,
		       							XWColorSpace *altA, 
		       							XWFunction *funcA,
		       							bool nonMarkingA, 
		       							uint overprintMaskA);
    virtual ~XWDeviceNColorSpace();
    
    virtual XWColorSpace *copy();
    
    XWColorSpace *getAlt() { return alt; }
    virtual void getCMYK(CoreColor *color, CoreCMYK *cmyk);
    XWString *getColorantName(int i) { return names[i]; }
    virtual void getDefaultColor(CoreColor *color);
    virtual void getGray(CoreColor *color, CoreGray *gray);
    virtual int  getMode() { return COLOR_SPACE_DEVICEN; }
    virtual int  getNComps() { return nComps; }
    virtual void getRGB(CoreColor *color,  CoreRGB *rgb);
    XWFunction *getTintTransformFunc() { return func; }
    
    static XWColorSpace *parse(XWArray *arr, int recursion);
    
public:
    int nComps;
    XWString * names[COLOR_MAX_COMPS];
    XWColorSpace * alt;
    XWFunction *func;
    bool nonMarking;
};

class XW_CORE_EXPORT XWPatternColorSpace : public XWColorSpace
{
public:
    XWPatternColorSpace(XWColorSpace *underA);
    virtual ~XWPatternColorSpace();
    
    virtual XWColorSpace *copy();
    
    virtual void getCMYK(CoreColor *, CoreCMYK *cmyk);
    virtual void getDefaultColor(CoreColor *) {}
    virtual void getGray(CoreColor *, CoreGray *gray);
    virtual int  getMode() { return COLOR_SPACE_PATTERN; }
    virtual int  getNComps() { return 0; }
    virtual void getRGB(CoreColor *,  CoreRGB *rgb);
    XWColorSpace *getUnder() { return under; }
    
    static XWColorSpace *parse(XWArray *arr, int recursion);
    
public:
    XWColorSpace *under;
};


#endif //XWCOLORSPACE_H
