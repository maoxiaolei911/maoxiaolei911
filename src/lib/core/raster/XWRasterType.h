/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWRASTERTYPE_H
#define XWRASTERTYPE_H

#include <math.h>

#include "XWGlobal.h"

#define MAX_COLOR_COMPS 4

#define COLOR_MODE_MONO1 0
#define COLOR_MODE_MONO8 1
#define COLOR_MODE_RGB8  2
#define COLOR_MODE_BGR8  3
#define COLOR_MODE_CMYK8 4

//antialiasing
#define AA_SIZE 4

typedef void (*XWRasterBlendFunc)(uchar *  src, uchar * dest, uchar * blend, int cm);

typedef bool (*XWRasterImageMaskSource)(void *data, uchar * pixel);

typedef bool (*XWRasterImageSource)(void *data, uchar * colorLine, uchar *alphaLine);

struct XW_RASTER_EXPORT GlyphBitmap
{
    int x, y, w, h;
    bool aa;
    uchar * data;	
    bool freeData;
};


#define SCREEN_TYPE_DISPERSED           0
#define SCREEN_TYPE_CLUSTERED           1
#define SCREEN_TYPE_STOCHASTICCLUSTERED 2

struct XW_RASTER_EXPORT ScreenParams
{
    int type;
    int size;
    int dotRadius;
    double gamma;
    double blackThreshold;
    double whiteThreshold;
};

#define LINE_CAP_BUTT       0
#define LINE_CAP_ROUND      1
#define LINE_CAP_PROJECTING 2

#define LINE_JOIN_MITER 0
#define LINE_JOIN_ROUND 1
#define LINE_JOIN_BEVEL 2

#define CLIP_ALLINSIDE  0
#define CLIP_ALLOUTSIDE 1
#define CLIP_PARTIAL    2

inline uchar div255(int x)
{
    return (uchar)((x + (x >> 8) + 0x80) >> 8);
}

inline uchar clip255(int x) 
{
  return x < 0 ? 0 : x > 255 ? 255 : x;
}

inline uchar bgr8R(uchar * bgr8) {return bgr8[2];}
inline uchar bgr8G(uchar * bgr8) {return bgr8[1];}
inline uchar bgr8B(uchar * bgr8) {return bgr8[0];}

inline uchar cmyk8C(uchar * cmyk8) {return cmyk8[0];}
inline uchar cmyk8M(uchar * cmyk8) {return cmyk8[1];}
inline uchar cmyk8Y(uchar * cmyk8) {return cmyk8[2];}
inline uchar cmyk8K(uchar * cmyk8) {return cmyk8[3];}

inline uchar rgb8R(uchar * rgb8) {return rgb8[0];}
inline uchar rgb8G(uchar * rgb8) {return rgb8[1];}
inline uchar rgb8B(uchar * rgb8) {return rgb8[2];}


inline double rasterAbs(double x) {return fabs(x);}
inline double rasterAvg(double x, double y) {return 0.5 * (x + y);}
inline int    rasterCeil(double x) {return (int)ceil(x);}
inline bool   rasterCheckDet(double m11, double m12, double m21, double m22, double epsilon)
{
	return fabs(m11 * m22 - m12 * m21) >= epsilon;
}

inline double rasterDist(double x0, double y0, double x1, double y1)
{
    double dx = x1 - x0;
    double dy = y1 - y0;
    return sqrt(dx * dx + dy * dy);
}

inline int    rasterFloor(double x) {return (int)floor(x);}
inline double rasterPow(double x, double y) {return pow(x, y);}
inline int    rasterRound(double x) {return (int)floor(x + 0.5);}
inline double rasterSqrt(double x) {return sqrt(x);}

inline void colorCopy(uchar * dest, uchar * src)
{
    dest[0] = src[0];
    dest[1] = src[1];
    dest[2] = src[2];
    dest[3] = src[3];
}

inline void colorXor(uchar * dest, uchar * src)
{
    dest[0] ^= src[0];
    dest[1] ^= src[1];
    dest[2] ^= src[2];
    dest[3] ^= src[3];
}

inline int imgCoordMungeLower(double x) 
{
  return floor(x);
}

inline int imgCoordMungeUpper(double x) 
{
  return floor(x) + 1;
}

inline int imgCoordMungeLowerC(double x, bool glyphMode) 
{
  return glyphMode ? (ceil(x + 0.5) - 1) : floor(x);
}

inline int imgCoordMungeUpperC(double x, bool glyphMode) 
{
  return glyphMode ? (ceil(x + 0.5) - 1) : (floor(x) + 1);
}

#endif // XWRASTERTYPE_H

