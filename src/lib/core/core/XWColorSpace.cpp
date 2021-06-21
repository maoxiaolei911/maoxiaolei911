/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stddef.h>
#include <math.h>
#include <string.h>
#include "XWApplication.h"
#include "XWObject.h"
#include "XWArray.h"
#include "XWColorSpace.h"

static char *ColorSpaceModeNames[] = 
{
  "DeviceGray",
  "CalGray",
  "DeviceRGB",
  "CalRGB",
  "DeviceCMYK",
  "Lab",
  "ICCBased",
  "Indexed",
  "Separation",
  "DeviceN",
  "Pattern"
};

#define COLORSPACEMODES ((sizeof(ColorSpaceModeNames) / sizeof(char *)))

XWColorSpace::XWColorSpace()
{
	overprintMask = 0x0f;
}

XWColorSpace::~XWColorSpace()
{
}

const char * XWColorSpace::getColorSpaceModeName(int idx)
{
    return ColorSpaceModeNames[idx];
}

int  XWColorSpace::getNumColorSpaceModes()
{
    return COLORSPACEMODES;
}

void XWColorSpace::getDefaultRanges(double *decodeLow, double *decodeRange, int)
{
    for (int i = 0; i < getNComps(); ++i) 
    {
        decodeLow[i] = 0;
        decodeRange[i] = 1;
    }
}

XWColorSpace * XWColorSpace::parse(XWObject *csObj, int recursion)
{
  XWColorSpace *cs;
  XWObject obj1;

  if (recursion > 8) 
  {
    xwApp->error("Loop detected in color space objects");
    return NULL;
  }
  cs = NULL;
  if (csObj->isName()) 
  {
    if (csObj->isName("DeviceGray") || csObj->isName("G")) 
    {
      cs = new XWDeviceGrayColorSpace();
    } 
    else if (csObj->isName("DeviceRGB") || csObj->isName("RGB")) 
    {
      cs = new XWDeviceRGBColorSpace();
    } 
    else if (csObj->isName("DeviceCMYK") || csObj->isName("CMYK")) 
    {
      cs = new XWDeviceCMYKColorSpace();
    } 
    else if (csObj->isName("Pattern")) 
    {
      cs = new XWPatternColorSpace(NULL);
    } 
  } 
  else if (csObj->isArray() && csObj->arrayGetLength() > 0) 
  {
    csObj->arrayGet(0, &obj1);
    if (obj1.isName("DeviceGray") || obj1.isName("G")) 
    {
      cs = new XWDeviceGrayColorSpace();
    } 
    else if (obj1.isName("DeviceRGB") || obj1.isName("RGB")) 
    {
      cs = new XWDeviceRGBColorSpace();
    } 
    else if (obj1.isName("DeviceCMYK") || obj1.isName("CMYK")) 
    {
      cs = new XWDeviceCMYKColorSpace();
    } 
    else if (obj1.isName("CalGray")) 
    {
      cs = XWCalGrayColorSpace::parse(csObj->getArray(), recursion);
    } 
    else if (obj1.isName("CalRGB")) 
    {
      cs = XWCalRGBColorSpace::parse(csObj->getArray(), recursion);
    } 
    else if (obj1.isName("Lab")) 
    {
      cs = XWLabColorSpace::parse(csObj->getArray(), recursion);
    } 
    else if (obj1.isName("ICCBased")) 
    {
      cs = XWICCBasedColorSpace::parse(csObj->getArray(), recursion);
    } 
    else if (obj1.isName("Indexed") || obj1.isName("I")) 
    {
      cs = XWIndexedColorSpace::parse(csObj->getArray(), recursion);
    } 
    else if (obj1.isName("Separation")) 
    {
      cs = XWSeparationColorSpace::parse(csObj->getArray(), recursion);
    } 
    else if (obj1.isName("DeviceN")) 
    {
      cs = XWDeviceNColorSpace::parse(csObj->getArray(), recursion);
    } 
    else if (obj1.isName("Pattern")) 
    {
      cs = XWPatternColorSpace::parse(csObj->getArray(), recursion);
    } 
    obj1.free();
  } 
  
  return cs;
}

XWDeviceGrayColorSpace::XWDeviceGrayColorSpace()
{
}

XWDeviceGrayColorSpace::~XWDeviceGrayColorSpace()
{
}

XWColorSpace * XWDeviceGrayColorSpace::copy()
{
    return new XWDeviceGrayColorSpace();
}

void XWDeviceGrayColorSpace::getCMYK(CoreColor *color, CoreCMYK *cmyk)
{
    cmyk->c = cmyk->m = cmyk->y = 0;
    cmyk->k = clip01(COLOR_COMP1 - color->c[0]);
}

void  XWDeviceGrayColorSpace::getDefaultColor(CoreColor *color)
{
    color->c[0] = 0;
}

void XWDeviceGrayColorSpace::getGray(CoreColor *color, CoreGray *gray)
{
    *gray = clip01(color->c[0]);
}

void XWDeviceGrayColorSpace::getRGB(CoreColor *color,  CoreRGB *rgb)
{
    rgb->r = rgb->g = rgb->b = clip01(color->c[0]);
}

XWCalGrayColorSpace::XWCalGrayColorSpace()
{
    whiteX = whiteY = whiteZ = 1;
    blackX = blackY = blackZ = 0;
    gamma = 1;
}

XWCalGrayColorSpace::~XWCalGrayColorSpace()
{
}

XWColorSpace * XWCalGrayColorSpace::copy()
{
    XWCalGrayColorSpace * cs = new XWCalGrayColorSpace();
    cs->whiteX = whiteX;
    cs->whiteY = whiteY;
    cs->whiteZ = whiteZ;
    cs->blackX = blackX;
    cs->blackY = blackY;
    cs->blackZ = blackZ;
    cs->gamma = gamma;
    return cs;
}

void XWCalGrayColorSpace::getCMYK(CoreColor *color, CoreCMYK *cmyk)
{
    cmyk->c = cmyk->m = cmyk->y = 0;
    cmyk->k = clip01(COLOR_COMP1 - color->c[0]);
}

void XWCalGrayColorSpace::getDefaultColor(CoreColor *color)
{
    color->c[0] = 0;
}

void XWCalGrayColorSpace::getGray(CoreColor *color, CoreGray *gray)
{
    *gray = clip01(color->c[0]);
}

void XWCalGrayColorSpace::getRGB(CoreColor *color,  CoreRGB *rgb)
{
    rgb->r = rgb->g = rgb->b = clip01(color->c[0]);
}

XWColorSpace * XWCalGrayColorSpace::parse(XWArray *arr, int )
{
  XWCalGrayColorSpace *cs;
  XWObject obj1, obj2, obj3;

  if (arr->getLength() < 2) 
  {
    xwApp->error("Bad CalGray color space");
    return NULL;
  }
  arr->get(1, &obj1);
  if (!obj1.isDict()) 
  {
    xwApp->error("Bad CalGray color space");
    obj1.free();
    return NULL;
  }
  cs = new XWCalGrayColorSpace();
  if (obj1.dictLookup("WhitePoint", &obj2)->isArray() && obj2.arrayGetLength() == 3) 
  {
    obj2.arrayGet(0, &obj3);
    cs->whiteX = obj3.getNum();
    obj3.free();
    obj2.arrayGet(1, &obj3);
    cs->whiteY = obj3.getNum();
    obj3.free();
    obj2.arrayGet(2, &obj3);
    cs->whiteZ = obj3.getNum();
    obj3.free();
  }
  obj2.free();
  if (obj1.dictLookup("BlackPoint", &obj2)->isArray() && obj2.arrayGetLength() == 3) 
  {
    obj2.arrayGet(0, &obj3);
    cs->blackX = obj3.getNum();
    obj3.free();
    obj2.arrayGet(1, &obj3);
    cs->blackY = obj3.getNum();
    obj3.free();
    obj2.arrayGet(2, &obj3);
    cs->blackZ = obj3.getNum();
    obj3.free();
  }
  obj2.free();
  if (obj1.dictLookup("Gamma", &obj2)->isNum()) 
  {
    cs->gamma = obj2.getNum();
  }
  obj2.free();
  obj1.free();
  return cs;
}

XWDeviceRGBColorSpace::XWDeviceRGBColorSpace()
{
}

XWDeviceRGBColorSpace::~XWDeviceRGBColorSpace()
{
}

XWColorSpace * XWDeviceRGBColorSpace::copy()
{
    return new XWDeviceRGBColorSpace();
}

void XWDeviceRGBColorSpace::getCMYK(CoreColor *color, CoreCMYK *cmyk)
{
    int c = clip01(COLOR_COMP1 - color->c[0]);
    int m = clip01(COLOR_COMP1 - color->c[1]);
    int y = clip01(COLOR_COMP1 - color->c[2]);
    int k = c;
    if (m < k) 
        k = m;
        
    if (y < k)
        k = y;
        
    cmyk->c = c - k;
    cmyk->m = m - k;
    cmyk->y = y - k;
    cmyk->k = k;
}

void XWDeviceRGBColorSpace::getDefaultColor(CoreColor *color)
{
    color->c[0] = 0;
    color->c[1] = 0;
    color->c[2] = 0;
}

void XWDeviceRGBColorSpace::getGray(CoreColor *color, CoreGray *gray)
{
    *gray = clip01((int)(0.3  * color->c[0] + 0.59 * color->c[1] + 0.11 * color->c[2] + 0.5));
}

void XWDeviceRGBColorSpace::getRGB(CoreColor *color,  CoreRGB *rgb)
{
    rgb->r = clip01(color->c[0]);
    rgb->g = clip01(color->c[1]);
    rgb->b = clip01(color->c[2]);
}

XWCalRGBColorSpace::XWCalRGBColorSpace()
{
    whiteX = whiteY = whiteZ = 1;
    blackX = blackY = blackZ = 0;
    gammaR = gammaG = gammaB = 1;
    mat[0] = 1; mat[1] = 0; mat[2] = 0;
    mat[3] = 0; mat[4] = 1; mat[5] = 0;
    mat[6] = 0; mat[7] = 0; mat[8] = 1;
}

XWCalRGBColorSpace::~XWCalRGBColorSpace()
{
}

XWColorSpace * XWCalRGBColorSpace::copy()
{
    XWCalRGBColorSpace * cs = new XWCalRGBColorSpace();
    cs->whiteX = whiteX;
    cs->whiteY = whiteY;
    cs->whiteZ = whiteZ;
    cs->blackX = blackX;
    cs->blackY = blackY;
    cs->blackZ = blackZ;
    cs->gammaR = gammaR;
    cs->gammaG = gammaG;
    cs->gammaB = gammaB;
    for (int i = 0; i < 9; ++i) 
        cs->mat[i] = mat[i];
    return cs;
}

void XWCalRGBColorSpace::getCMYK(CoreColor *color, CoreCMYK *cmyk)
{
    int c = clip01(COLOR_COMP1 - color->c[0]);
    int m = clip01(COLOR_COMP1 - color->c[1]);
    int y = clip01(COLOR_COMP1 - color->c[2]);
    int k = c;
    if (m < k) 
        k = m;
        
    if (y < k)
        k = y;
        
    cmyk->c = c - k;
    cmyk->m = m - k;
    cmyk->y = y - k;
    cmyk->k = k;
}

void XWCalRGBColorSpace::getDefaultColor(CoreColor *color)
{
    color->c[0] = 0;
    color->c[1] = 0;
    color->c[2] = 0;
}

void XWCalRGBColorSpace::getGray(CoreColor *color, CoreGray *gray)
{
  *gray = clip01((int)(0.299 * color->c[0] +
				0.587 * color->c[1] +
				0.114 * color->c[2] + 0.5));
}

void XWCalRGBColorSpace::getRGB(CoreColor *color,  CoreRGB *rgb)
{
    rgb->r = clip01(color->c[0]);
    rgb->g = clip01(color->c[1]);
    rgb->b = clip01(color->c[2]);
}

XWColorSpace * XWCalRGBColorSpace::parse(XWArray *arr, int )
{
  XWCalRGBColorSpace *cs;
  XWObject obj1, obj2, obj3;
  int i;

  if (arr->getLength() < 2) {
    xwApp->error("Bad CalRGB color space");
    return NULL;
  }
  arr->get(1, &obj1);
  if (!obj1.isDict()) {
    xwApp->error("Bad CalRGB color space");
    obj1.free();
    return NULL;
  }
  cs = new XWCalRGBColorSpace();
  if (obj1.dictLookup("WhitePoint", &obj2)->isArray() &&
      obj2.arrayGetLength() == 3) {
    obj2.arrayGet(0, &obj3);
    cs->whiteX = obj3.getNum();
    obj3.free();
    obj2.arrayGet(1, &obj3);
    cs->whiteY = obj3.getNum();
    obj3.free();
    obj2.arrayGet(2, &obj3);
    cs->whiteZ = obj3.getNum();
    obj3.free();
  }
  obj2.free();
  if (obj1.dictLookup("BlackPoint", &obj2)->isArray() &&
      obj2.arrayGetLength() == 3) {
    obj2.arrayGet(0, &obj3);
    cs->blackX = obj3.getNum();
    obj3.free();
    obj2.arrayGet(1, &obj3);
    cs->blackY = obj3.getNum();
    obj3.free();
    obj2.arrayGet(2, &obj3);
    cs->blackZ = obj3.getNum();
    obj3.free();
  }
  obj2.free();
  if (obj1.dictLookup("Gamma", &obj2)->isArray() &&
      obj2.arrayGetLength() == 3) {
    obj2.arrayGet(0, &obj3);
    cs->gammaR = obj3.getNum();
    obj3.free();
    obj2.arrayGet(1, &obj3);
    cs->gammaG = obj3.getNum();
    obj3.free();
    obj2.arrayGet(2, &obj3);
    cs->gammaB = obj3.getNum();
    obj3.free();
  }
  obj2.free();
  if (obj1.dictLookup("Matrix", &obj2)->isArray() &&
      obj2.arrayGetLength() == 9) {
    for (i = 0; i < 9; ++i) {
      obj2.arrayGet(i, &obj3);
      cs->mat[i] = obj3.getNum();
      obj3.free();
    }
  }
  obj2.free();
  obj1.free();
  return cs;
}

XWDeviceCMYKColorSpace::XWDeviceCMYKColorSpace()
{
}

XWDeviceCMYKColorSpace::~XWDeviceCMYKColorSpace()
{
}

XWColorSpace * XWDeviceCMYKColorSpace::copy()
{
    return new XWDeviceCMYKColorSpace();
}

void XWDeviceCMYKColorSpace::getCMYK(CoreColor *color, CoreCMYK *cmyk)
{
    cmyk->c = clip01(color->c[0]);
    cmyk->m = clip01(color->c[1]);
    cmyk->y = clip01(color->c[2]);
    cmyk->k = clip01(color->c[3]);
}

void XWDeviceCMYKColorSpace::getDefaultColor(CoreColor *color)
{
    color->c[0] = 0;
    color->c[1] = 0;
    color->c[2] = 0;
    color->c[3] = COLOR_COMP1;
}

void XWDeviceCMYKColorSpace::getGray(CoreColor *color, CoreGray *gray)
{
    *gray = clip01((int)(COLOR_COMP1 - color->c[3]
				- 0.3  * color->c[0]
				- 0.59 * color->c[1]
				- 0.11 * color->c[2] + 0.5));
}

void XWDeviceCMYKColorSpace::getRGB(CoreColor *color,  CoreRGB *rgb)
{
  double c, m, y, k, c1, m1, y1, k1, r, g, b, x;

  c = colToDbl(color->c[0]);
  m = colToDbl(color->c[1]);
  y = colToDbl(color->c[2]);
  k = colToDbl(color->c[3]);
  c1 = 1 - c;
  m1 = 1 - m;
  y1 = 1 - y;
  k1 = 1 - k;
  // this is a matrix multiplication, unrolled for performance
  //                        C M Y K
  x = c1 * m1 * y1 * k1; // 0 0 0 0
  r = g = b = x;
  x = c1 * m1 * y1 * k;  // 0 0 0 1
  r += 0.1373 * x;
  g += 0.1216 * x;
  b += 0.1255 * x;
  x = c1 * m1 * y  * k1; // 0 0 1 0
  r += x;
  g += 0.9490 * x;
  x = c1 * m1 * y  * k;  // 0 0 1 1
  r += 0.1098 * x;
  g += 0.1020 * x;
  x = c1 * m  * y1 * k1; // 0 1 0 0
  r += 0.9255 * x;
  b += 0.5490 * x;
  x = c1 * m  * y1 * k;  // 0 1 0 1
  r += 0.1412 * x;
  x = c1 * m  * y  * k1; // 0 1 1 0
  r += 0.9294 * x;
  g += 0.1098 * x;
  b += 0.1412 * x;
  x = c1 * m  * y  * k;  // 0 1 1 1
  r += 0.1333 * x;
  x = c  * m1 * y1 * k1; // 1 0 0 0
  g += 0.6784 * x;
  b += 0.9373 * x;
  x = c  * m1 * y1 * k;  // 1 0 0 1
  g += 0.0588 * x;
  b += 0.1412 * x;
  x = c  * m1 * y  * k1; // 1 0 1 0
  g += 0.6510 * x;
  b += 0.3137 * x;
  x = c  * m1 * y  * k;  // 1 0 1 1
  g += 0.0745 * x;
  x = c  * m  * y1 * k1; // 1 1 0 0
  r += 0.1804 * x;
  g += 0.1922 * x;
  b += 0.5725 * x;
  x = c  * m  * y1 * k;  // 1 1 0 1
  b += 0.0078 * x;
  x = c  * m  * y  * k1; // 1 1 1 0
  r += 0.2118 * x;
  g += 0.2119 * x;
  b += 0.2235 * x;
  rgb->r = clip01(dblToCol(r));
  rgb->g = clip01(dblToCol(g));
  rgb->b = clip01(dblToCol(b));
}

static double xyzrgb[3][3] = {
  {  3.240449, -1.537136, -0.498531 },
  { -0.969265,  1.876011,  0.041556 },
  {  0.055643, -0.204026,  1.057229 }
};

XWLabColorSpace::XWLabColorSpace()
{
    whiteX = whiteY = whiteZ = 1;
    blackX = blackY = blackZ = 0;
    aMin = bMin = -100;
    aMax = bMax = 100;
}

XWLabColorSpace::~XWLabColorSpace()
{
}

XWColorSpace * XWLabColorSpace::copy()
{
    XWLabColorSpace * cs = new XWLabColorSpace();
    cs->whiteX = whiteX;
    cs->whiteY = whiteY;
    cs->whiteZ = whiteZ;
    cs->blackX = blackX;
    cs->blackY = blackY;
    cs->blackZ = blackZ;
    cs->aMin = aMin;
    cs->aMax = aMax;
    cs->bMin = bMin;
    cs->bMax = bMax;
    cs->kr = kr;
    cs->kg = kg;
    cs->kb = kb;
    return cs;
}

void XWLabColorSpace::getCMYK(CoreColor *color, CoreCMYK *cmyk)
{
    CoreRGB rgb;
    getRGB(color, &rgb);
    int c = clip01(COLOR_COMP1 - rgb.r);
    int m = clip01(COLOR_COMP1 - rgb.g);
    int y = clip01(COLOR_COMP1 - rgb.b);
    int k = c;
    if (m < k)
        k = m;
        
    if (y < k) 
        k = y;
        
    cmyk->c = c - k;
    cmyk->m = m - k;
    cmyk->y = y - k;
    cmyk->k = k;
}

void XWLabColorSpace::getDefaultColor(CoreColor *color)
{
    color->c[0] = 0;
    if (aMin > 0) 
        color->c[1] = dblToCol(aMin);
    else if (aMax < 0) 
        color->c[1] = dblToCol(aMax);
    else 
        color->c[1] = 0;
        
    if (bMin > 0) 
        color->c[2] = dblToCol(bMin);
    else if (bMax < 0) 
        color->c[2] = dblToCol(bMax);
    else 
        color->c[2] = 0;
}

void XWLabColorSpace::getDefaultRanges(double *decodeLow, double *decodeRange, int)
{
    decodeLow[0] = 0;
    decodeRange[0] = 100;
    decodeLow[1] = aMin;
    decodeRange[1] = aMax - aMin;
    decodeLow[2] = bMin;
    decodeRange[2] = bMax - bMin;
}

void XWLabColorSpace::getGray(CoreColor *color, CoreGray *gray)
{
  CoreRGB rgb;
  getRGB(color, &rgb);
  *gray = clip01((int)(0.299 * rgb.r +
				0.587 * rgb.g +
				0.114 * rgb.b + 0.5));
}

void XWLabColorSpace::getRGB(CoreColor *color,  CoreRGB *rgb)
{
  double X, Y, Z;
  double t1, t2;
  double r, g, b;

  // convert L*a*b* to CIE 1931 XYZ color space
  t1 = (colToDbl(color->c[0]) + 16) / 116;
  t2 = t1 + colToDbl(color->c[1]) / 500;
  if (t2 >= (6.0 / 29.0)) {
    X = t2 * t2 * t2;
  } else {
    X = (108.0 / 841.0) * (t2 - (4.0 / 29.0));
  }
  X *= whiteX;
  if (t1 >= (6.0 / 29.0)) {
    Y = t1 * t1 * t1;
  } else {
    Y = (108.0 / 841.0) * (t1 - (4.0 / 29.0));
  }
  Y *= whiteY;
  t2 = t1 - colToDbl(color->c[2]) / 200;
  if (t2 >= (6.0 / 29.0)) {
    Z = t2 * t2 * t2;
  } else {
    Z = (108.0 / 841.0) * (t2 - (4.0 / 29.0));
  }
  Z *= whiteZ;

  // convert XYZ to RGB, including gamut mapping and gamma correction
  r = xyzrgb[0][0] * X + xyzrgb[0][1] * Y + xyzrgb[0][2] * Z;
  g = xyzrgb[1][0] * X + xyzrgb[1][1] * Y + xyzrgb[1][2] * Z;
  b = xyzrgb[2][0] * X + xyzrgb[2][1] * Y + xyzrgb[2][2] * Z;
  rgb->r = dblToCol(pow(clip01(r * kr), 0.5));
  rgb->g = dblToCol(pow(clip01(g * kg), 0.5));
  rgb->b = dblToCol(pow(clip01(b * kb), 0.5));
}

XWColorSpace * XWLabColorSpace::parse(XWArray *arr, int )
{
  XWLabColorSpace *cs;
  XWObject obj1, obj2, obj3;

  if (arr->getLength() < 2) {
    xwApp->error("Bad Lab color space");
    return NULL;
  }
  arr->get(1, &obj1);
  if (!obj1.isDict()) {
    xwApp->error("Bad Lab color space");
    obj1.free();
    return NULL;
  }
  cs = new XWLabColorSpace();
  if (obj1.dictLookup("WhitePoint", &obj2)->isArray() &&
      obj2.arrayGetLength() == 3) {
    obj2.arrayGet(0, &obj3);
    cs->whiteX = obj3.getNum();
    obj3.free();
    obj2.arrayGet(1, &obj3);
    cs->whiteY = obj3.getNum();
    obj3.free();
    obj2.arrayGet(2, &obj3);
    cs->whiteZ = obj3.getNum();
    obj3.free();
  }
  obj2.free();
  if (obj1.dictLookup("BlackPoint", &obj2)->isArray() &&
      obj2.arrayGetLength() == 3) {
    obj2.arrayGet(0, &obj3);
    cs->blackX = obj3.getNum();
    obj3.free();
    obj2.arrayGet(1, &obj3);
    cs->blackY = obj3.getNum();
    obj3.free();
    obj2.arrayGet(2, &obj3);
    cs->blackZ = obj3.getNum();
    obj3.free();
  }
  obj2.free();
  if (obj1.dictLookup("Range", &obj2)->isArray() &&
      obj2.arrayGetLength() == 4) {
    obj2.arrayGet(0, &obj3);
    cs->aMin = obj3.getNum();
    obj3.free();
    obj2.arrayGet(1, &obj3);
    cs->aMax = obj3.getNum();
    obj3.free();
    obj2.arrayGet(2, &obj3);
    cs->bMin = obj3.getNum();
    obj3.free();
    obj2.arrayGet(3, &obj3);
    cs->bMax = obj3.getNum();
    obj3.free();
  }
  obj2.free();
  obj1.free();

  cs->kr = 1 / (xyzrgb[0][0] * cs->whiteX +
		xyzrgb[0][1] * cs->whiteY +
		xyzrgb[0][2] * cs->whiteZ);
  cs->kg = 1 / (xyzrgb[1][0] * cs->whiteX +
		xyzrgb[1][1] * cs->whiteY +
		xyzrgb[1][2] * cs->whiteZ);
  cs->kb = 1 / (xyzrgb[2][0] * cs->whiteX +
		xyzrgb[2][1] * cs->whiteY +
		xyzrgb[2][2] * cs->whiteZ);

  return cs;
}

XWICCBasedColorSpace::XWICCBasedColorSpace(int nCompsA, 
	                                       XWColorSpace *altA, 
	                                       ObjRef *iccProfileStreamA)
{
    nComps = nCompsA;
    alt = altA;
    iccProfileStream = *iccProfileStreamA;
    rangeMin[0] = rangeMin[1] = rangeMin[2] = rangeMin[3] = 0;
    rangeMax[0] = rangeMax[1] = rangeMax[2] = rangeMax[3] = 1;
}

XWICCBasedColorSpace::~XWICCBasedColorSpace()
{
    if (alt)
        delete alt;
}

XWColorSpace * XWICCBasedColorSpace::copy()
{
    XWICCBasedColorSpace * cs = new XWICCBasedColorSpace(nComps, alt->copy(), &iccProfileStream);
    for (int i = 0; i < 4; ++i) 
    {
        cs->rangeMin[i] = rangeMin[i];
        cs->rangeMax[i] = rangeMax[i];
    }
    return cs;
}

void XWICCBasedColorSpace::getCMYK(CoreColor *color, CoreCMYK *cmyk)
{
    alt->getCMYK(color, cmyk);
}

void XWICCBasedColorSpace::getDefaultColor(CoreColor *color)
{
    for (int i = 0; i < nComps; ++i) 
    {
        if (rangeMin[i] > 0) 
            color->c[i] = dblToCol(rangeMin[i]);
        else if (rangeMax[i] < 0) 
            color->c[i] = dblToCol(rangeMax[i]);
        else 
            color->c[i] = 0;
    }
}

void XWICCBasedColorSpace::getDefaultRanges(double *decodeLow, double *decodeRange, int maxImgPixel)
{
    alt->getDefaultRanges(decodeLow, decodeRange, maxImgPixel);
}

void XWICCBasedColorSpace::getGray(CoreColor *color, CoreGray *gray)
{
    alt->getGray(color, gray);
}

void XWICCBasedColorSpace::getRGB(CoreColor *color,  CoreRGB *rgb)
{
    alt->getRGB(color, rgb);
}

XWColorSpace * XWICCBasedColorSpace::parse(XWArray *arr, int recursion)
{
  XWICCBasedColorSpace *cs;
  ObjRef iccProfileStreamA;
  int nCompsA;
  XWColorSpace *altA;
  XWDict *dict;
  XWObject obj1, obj2, obj3;
  int i;

  if (arr->getLength() < 2) {
    xwApp->error("Bad ICCBased color space");
    return NULL;
  }
  arr->getNF(1, &obj1);
  if (obj1.isRef()) {
    iccProfileStreamA = obj1.getRef();
  } else {
    iccProfileStreamA.num = 0;
    iccProfileStreamA.gen = 0;
  }
  obj1.free();
  arr->get(1, &obj1);
  if (!obj1.isStream()) {
    xwApp->error("Bad ICCBased color space (stream)");
    obj1.free();
    return NULL;
  }
  dict = obj1.streamGetDict();
  if (!dict->lookup("N", &obj2)->isInt()) {
    xwApp->error("Bad ICCBased color space (N)");
    obj2.free();
    obj1.free();
    return NULL;
  }
  nCompsA = obj2.getInt();
  obj2.free();
  if (nCompsA > 4) {
    xwApp->error("ICCBased color space with too many ({0:d} > 4) components");
    nCompsA = 4;
  }
  if (dict->lookup("Alternate", &obj2)->isNull() ||
      !(altA = XWColorSpace::parse(&obj2, recursion + 1))) {
    switch (nCompsA) {
    case 1:
      altA = new XWDeviceGrayColorSpace();
      break;
    case 3:
      altA = new XWDeviceRGBColorSpace();
      break;
    case 4:
      altA = new XWDeviceCMYKColorSpace();
      break;
    default:
      xwApp->error("Bad ICCBased color space - invalid N");
      obj2.free();
      obj1.free();
      return NULL;
    }
  }
  obj2.free();
  cs = new XWICCBasedColorSpace(nCompsA, altA, &iccProfileStreamA);
  if (dict->lookup("Range", &obj2)->isArray() &&
      obj2.arrayGetLength() == 2 * nCompsA) {
    for (i = 0; i < nCompsA; ++i) {
      obj2.arrayGet(2*i, &obj3);
      cs->rangeMin[i] = obj3.getNum();
      obj3.free();
      obj2.arrayGet(2*i+1, &obj3);
      cs->rangeMax[i] = obj3.getNum();
      obj3.free();
    }
  }
  obj2.free();
  obj1.free();
  return cs;
}

XWIndexedColorSpace::XWIndexedColorSpace(XWColorSpace *baseA, int indexHighA)
{
    base = baseA;
    indexHigh = indexHighA;
    lookup = new uchar[(indexHigh + 1) * base->getNComps()];
    overprintMask = base->getOverprintMask();
}

XWIndexedColorSpace::~XWIndexedColorSpace()
{
    if (base)
        delete base;
        
    if (lookup)
        delete [] lookup;
}

XWColorSpace * XWIndexedColorSpace::copy()
{
    XWIndexedColorSpace * cs = new XWIndexedColorSpace(base->copy(), indexHigh);
    memcpy(cs->lookup, lookup, (indexHigh + 1) * base->getNComps() * sizeof(uchar));
    return cs;
}

void XWIndexedColorSpace::getCMYK(CoreColor *color, CoreCMYK *cmyk)
{
    CoreColor color2;
    base->getCMYK(mapColorToBase(color, &color2), cmyk);
}

void XWIndexedColorSpace::getDefaultColor(CoreColor *color)
{
    color->c[0] = 0;
}

void XWIndexedColorSpace::getDefaultRanges(double *decodeLow, double *decodeRange, int maxImgPixel)
{
    decodeLow[0] = 0;
    decodeRange[0] = maxImgPixel;
}

void XWIndexedColorSpace::getGray(CoreColor *color, CoreGray *gray)
{
    CoreColor color2;
    base->getGray(mapColorToBase(color, &color2), gray);
}

void XWIndexedColorSpace::getRGB(CoreColor *color,  CoreRGB *rgb)
{
    CoreColor color2;
    base->getRGB(mapColorToBase(color, &color2), rgb);
}

CoreColor * XWIndexedColorSpace::mapColorToBase(CoreColor *color, CoreColor *baseColor)
{
    double low[COLOR_MAX_COMPS], range[COLOR_MAX_COMPS];
    int n = base->getNComps();
    base->getDefaultRanges(low, range, indexHigh);
    
    uchar * p = &lookup[(int)(colToDbl(color->c[0]) + 0.5) * n];
    for (int i = 0; i < n; ++i)
        baseColor->c[i] = dblToCol(low[i] + (p[i] / 255.0) * range[i]);
        
    return baseColor;
}

XWColorSpace * XWIndexedColorSpace::parse(XWArray *arr, int recursion)
{
	XWIndexedColorSpace *cs;
  XWColorSpace *baseA;
  int indexHighA;
  XWObject obj1;
  int x;
  char *s;
  int n, i, j;

  if (arr->getLength() != 4) {
    xwApp->error("Bad Indexed color space");
    goto err1;
  }
  arr->get(1, &obj1);
  if (!(baseA = XWColorSpace::parse(&obj1, recursion + 1))) {
    xwApp->error("Bad Indexed color space (base color space)");
    goto err2;
  }
  obj1.free();
  if (!arr->get(2, &obj1)->isInt()) {
    xwApp->error("Bad Indexed color space (hival)");
    delete baseA;
    goto err2;
  }
  indexHighA = obj1.getInt();
  if (indexHighA < 0 || indexHighA > 255) {
    // the PDF spec requires indexHigh to be in [0,255] -- allowing
    // values larger than 255 creates a security hole: if nComps *
    // indexHigh is greater than 2^31, the loop below may overwrite
    // past the end of the array
    xwApp->error("Bad Indexed color space (invalid indexHigh value)");
    delete baseA;
    goto err2;
  }
  obj1.free();
  cs = new XWIndexedColorSpace(baseA, indexHighA);
  arr->get(3, &obj1);
  n = baseA->getNComps();
  if (obj1.isStream()) {
    obj1.streamReset();
    for (i = 0; i <= indexHighA; ++i) {
      for (j = 0; j < n; ++j) {
	if ((x = obj1.streamGetChar()) == EOF) {
	  xwApp->error("Bad Indexed color space (lookup table stream too short)");
	  cs->indexHigh = indexHighA = i - 1;
	}
	cs->lookup[i*n + j] = (uchar)x;
      }
    }
    obj1.streamClose();
  } else if (obj1.isString()) {
    if (obj1.getString()->getLength() < (indexHighA + 1) * n) {
      xwApp->error("Bad Indexed color space (lookup table string too short)");
      cs->indexHigh = indexHighA = obj1.getString()->getLength() / n - 1;
    }
    s = obj1.getString()->getCString();
    for (i = 0; i <= indexHighA; ++i) {
      for (j = 0; j < n; ++j) {
	cs->lookup[i*n + j] = (uchar)*s++;
      }
    }
  } else {
    xwApp->error("Bad Indexed color space (lookup table)");
    goto err3;
  }
  obj1.free();
  return cs;

 err3:
  delete cs;
 err2:
  obj1.free();
 err1:
  return NULL;
}

XWSeparationColorSpace::XWSeparationColorSpace(XWString *nameA, XWColorSpace *altA, XWFunction *funcA)
{
    name = nameA;
    alt = altA;
    func = funcA;
    nonMarking = !name->cmp("None");
    if (!name->cmp("Cyan")) 
    {
    	overprintMask = 0x01;
  	} 
  	else if (!name->cmp("Magenta")) 
  	{
    	overprintMask = 0x02;
  	} 
  	else if (!name->cmp("Yellow")) 
  	{
    	overprintMask = 0x04;
  	} 
  	else if (!name->cmp("Black")) 
  	{
    	overprintMask = 0x08;
  	}
}

XWSeparationColorSpace::XWSeparationColorSpace(XWString *nameA, 
                                               XWColorSpace *altA,
			                                         XWFunction *funcA, 
			                                         bool nonMarkingA,
			                                         uint overprintMaskA)
{
	name = nameA;
  alt = altA;
  func = funcA;
  nonMarking = nonMarkingA;
  overprintMask = overprintMaskA;
}

XWSeparationColorSpace::~XWSeparationColorSpace()
{
    if (name)
        delete name;
        
    if (alt)
        delete alt;
        
    if (func)
        delete func;
}

XWColorSpace * XWSeparationColorSpace::copy()
{
    return new XWSeparationColorSpace(name->copy(), alt->copy(), func->copy(),
				     nonMarking, overprintMask);
}

void XWSeparationColorSpace::getCMYK(CoreColor *color, CoreCMYK *cmyk)
{
    double x = colToDbl((int)(color->c[0]));
    double c[COLOR_MAX_COMPS];
    func->transform(&x, c);
    CoreColor color2;
    for (int i = 0; i < alt->getNComps(); ++i) 
        color2.c[i] = dblToCol(c[i]);
        
    alt->getCMYK(&color2, cmyk);
}

void XWSeparationColorSpace::getDefaultColor(CoreColor *color)
{
    color->c[0] = COLOR_COMP1;
}

void XWSeparationColorSpace::getGray(CoreColor *color, CoreGray *gray)
{
    double x = colToDbl((int)(color->c[0]));
    double c[COLOR_MAX_COMPS];
    func->transform(&x, c);
    CoreColor color2;
    for (int i = 0; i < alt->getNComps(); ++i) 
        color2.c[i] = dblToCol(c[i]);
        
    alt->getGray(&color2, gray);
}

void XWSeparationColorSpace::getRGB(CoreColor *color,  CoreRGB *rgb)
{
    double x = colToDbl((int)(color->c[0]));
    double c[COLOR_MAX_COMPS];
    func->transform(&x, c);
    CoreColor color2;
    for (int i = 0; i < alt->getNComps(); ++i)
        color2.c[i] = dblToCol(c[i]);
        
    alt->getRGB(&color2, rgb);
}

XWColorSpace *XWSeparationColorSpace::parse(XWArray *arr, int recursion)
{
	XWSeparationColorSpace *cs;
  XWString *nameA;
  XWColorSpace *altA;
  XWFunction *funcA;
  XWObject obj1;

  if (arr->getLength() != 4) {
    xwApp->error("Bad Separation color space");
    goto err1;
  }
  if (!arr->get(1, &obj1)->isName()) {
    xwApp->error("Bad Separation color space (name)");
    goto err2;
  }
  nameA = new XWString(obj1.getName());
  obj1.free();
  arr->get(2, &obj1);
  if (!(altA = XWColorSpace::parse(&obj1, recursion + 1))) {
    xwApp->error("Bad Separation color space (alternate color space)");
    goto err3;
  }
  obj1.free();
  arr->get(3, &obj1);
  if (!(funcA = XWFunction::parse(&obj1))) {
    goto err4;
  }
  obj1.free();
  cs = new XWSeparationColorSpace(nameA, altA, funcA);
  return cs;

 err4:
  delete altA;
 err3:
  delete nameA;
 err2:
  obj1.free();
 err1:
  return NULL;
}

XWDeviceNColorSpace::XWDeviceNColorSpace(int nCompsA, 
	                                       XWString **namesA, 
	                                       XWColorSpace *altA, 
	                                       XWFunction *funcA)
{
    nComps = nCompsA;
    alt = altA;
    func = funcA;
    nonMarking = true;
  overprintMask = 0;
    for (int i = 0; i < nComps; i++)
    {
        names[i] = namesA[i];
    		if (names[i]->cmp("None")) 
    		{
      		nonMarking = false;
    		}
    		if (!names[i]->cmp("Cyan")) 
    		{
      		overprintMask |= 0x01;
    		} 
    		else if (!names[i]->cmp("Magenta")) 
    		{
      		overprintMask |= 0x02;
    		} 
    		else if (!names[i]->cmp("Yellow")) 
    		{
      		overprintMask |= 0x04;
    		} 
    		else if (!names[i]->cmp("Black")) 
    		{
      		overprintMask |= 0x08;
    		} 
    		else 
    		{
      		overprintMask = 0x0f;
    		}
     }
}

XWDeviceNColorSpace::XWDeviceNColorSpace(int nCompsA, 
                                         XWString **namesA,
		       							                 XWColorSpace *altA, 
		       							                 XWFunction *funcA,
		       							                 bool nonMarkingA, 
		       							                 uint overprintMaskA)
{
	nComps = nCompsA;
  alt = altA;
  func = funcA;
  nonMarking = nonMarkingA;
  overprintMask = overprintMaskA;
  for (int i = 0; i < nComps; ++i) 
  {
    names[i] = namesA[i]->copy();
  }
}

XWDeviceNColorSpace::~XWDeviceNColorSpace()
{
    if (alt)
        delete alt;
        
    for (int i = 0; i < nComps; i++)
    {
        if (names[i])
            delete names[i];
    }
    
    if (func)
        delete func;
}

XWColorSpace *XWDeviceNColorSpace::copy()
{
    return new XWDeviceNColorSpace(nComps, names, alt->copy(), func->copy(), nonMarking, overprintMask);
}

void XWDeviceNColorSpace::getCMYK(CoreColor *color, CoreCMYK *cmyk)
{
    double x[COLOR_MAX_COMPS];
    for (int i = 0; i < nComps; ++i)
        x[i] = colToDbl(color->c[i]);
        
    double c[COLOR_MAX_COMPS];
    func->transform(x, c);
    
    CoreColor color2;
    for (int i = 0; i < alt->getNComps(); ++i)
        color2.c[i] = dblToCol(c[i]);
        
    alt->getCMYK(&color2, cmyk);
}

void XWDeviceNColorSpace::getDefaultColor(CoreColor *color)
{
    for (int i = 0; i < nComps; ++i)
        color->c[i] = COLOR_COMP1;
}

void XWDeviceNColorSpace::getGray(CoreColor *color, CoreGray *gray)
{
    double x[COLOR_MAX_COMPS];
    for (int i = 0; i < nComps; ++i)
        x[i] = colToDbl(color->c[i]);
        
    double c[COLOR_MAX_COMPS];
    func->transform(x, c);
    
    CoreColor color2;
    for (int i = 0; i < alt->getNComps(); ++i)
        color2.c[i] = dblToCol(c[i]);
        
    alt->getGray(&color2, gray);
}

void XWDeviceNColorSpace::getRGB(CoreColor *color,  CoreRGB *rgb)
{
    double x[COLOR_MAX_COMPS];
    for (int i = 0; i < nComps; ++i)
        x[i] = colToDbl(color->c[i]);
        
    double c[COLOR_MAX_COMPS];
    func->transform(x, c);
    
    CoreColor color2;
    for (int i = 0; i < alt->getNComps(); ++i)
        color2.c[i] = dblToCol(c[i]);
        
    alt->getRGB(&color2, rgb);
}

XWColorSpace *XWDeviceNColorSpace::parse(XWArray *arr, int recursion)
{
	XWDeviceNColorSpace *cs;
  int nCompsA;
  XWString *namesA[COLOR_MAX_COMPS];
  XWColorSpace *altA;
  XWFunction *funcA;
  XWObject obj1, obj2;
  int i;

  if (arr->getLength() != 4 && arr->getLength() != 5) {
    xwApp->error("Bad DeviceN color space");
    goto err1;
  }
  if (!arr->get(1, &obj1)->isArray()) {
    xwApp->error("Bad DeviceN color space (names)");
    goto err2;
  }
  nCompsA = obj1.arrayGetLength();
  if (nCompsA > COLOR_MAX_COMPS) {
    xwApp->error("DeviceN color space with too many ({0:d} > {1:d}) components");
    nCompsA = COLOR_MAX_COMPS;
  }
  for (i = 0; i < nCompsA; ++i) {
    if (!obj1.arrayGet(i, &obj2)->isName()) {
      xwApp->error("Bad DeviceN color space (names)");
      obj2.free();
      goto err2;
    }
    namesA[i] = new XWString(obj2.getName());
    obj2.free();
  }
  obj1.free();
  arr->get(2, &obj1);
  if (!(altA = XWColorSpace::parse(&obj1, recursion + 1))) {
    xwApp->error("Bad DeviceN color space (alternate color space)");
    goto err3;
  }
  obj1.free();
  arr->get(3, &obj1);
  if (!(funcA = XWFunction::parse(&obj1))) {
    goto err4;
  }
  obj1.free();
  cs = new XWDeviceNColorSpace(nCompsA, namesA, altA, funcA);
  return cs;

 err4:
  delete altA;
 err3:
  for (i = 0; i < nCompsA; ++i) {
    delete namesA[i];
  }
 err2:
  obj1.free();
 err1:
  return NULL;
}

XWPatternColorSpace::XWPatternColorSpace(XWColorSpace *underA)
{
    under = underA;
}

XWPatternColorSpace::~XWPatternColorSpace()
{
    if (under)
        delete under;
}

XWColorSpace * XWPatternColorSpace::copy()
{
    return new XWPatternColorSpace(under ? under->copy() : (XWColorSpace *)0);
}

void XWPatternColorSpace::getCMYK(CoreColor *, CoreCMYK *cmyk)
{
    cmyk->c = cmyk->m = cmyk->y = 0;
    cmyk->k = 1;
}

void XWPatternColorSpace::getGray(CoreColor *, CoreGray *gray)
{
    *gray = 0;
}

void XWPatternColorSpace::getRGB(CoreColor *,  CoreRGB *rgb)
{
    rgb->r = rgb->g = rgb->b = 0;
}

XWColorSpace *XWPatternColorSpace::parse(XWArray *arr, int recursion)
{
  XWPatternColorSpace *cs;
  XWColorSpace *underA;
  XWObject obj1;

  if (arr->getLength() != 1 && arr->getLength() != 2) {
    xwApp->error("Bad Pattern color space");
    return NULL;
  }
  underA = NULL;
  if (arr->getLength() == 2) {
    arr->get(1, &obj1);
    if (!(underA = XWColorSpace::parse(&obj1, recursion + 1))) {
      xwApp->error("Bad Pattern color space (underlying color space)");
      obj1.free();
      return NULL;
    }
    obj1.free();
  }
  cs = new XWPatternColorSpace(underA);
  return cs;
}
