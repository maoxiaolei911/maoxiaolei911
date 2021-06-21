/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWNUMBERUTIL_H
#define XWNUMBERUTIL_H

#include <math.h>
#include <QByteArray>

#include "XWGlobal.h"
#include "XWType.h"


#define CNST_BYTE_NUM_INT 8
#undef M_PI
#define M_PI (4.0*atan(1.0))

inline quint16 bigEndian16(const uchar *src)
{
    return (0 | src[1] | (src[0] * (quint16)0x0100));
}


inline quint32 bigEndian32(const uchar *src)
{
    return (0 | src[3] | (src[2] * (quint32)0x00000100) | (src[1] * (quint32)0x00010000) | (src[0] * (quint32)0x01000000));
}

inline qint32 charToInt32(const char * p)
{
    qint32 ret = ((qint32)(*(p + 3)) & 0x000000FF) << 24;
    ret = ret | (((qint32)(*(p + 2)) & 0x000000FF) << 16);
    ret = ret | (((qint32)(*(p + 1)) & 0x000000FF) << 8);
    ret = ret | (qint32)(*p) & 0x000000FF;
    return ret;
}

inline quint64 charToInt64(const char * p)
{
    quint64 ret = ((quint64)(*(p + 3)) & 0x00000000000000FF) << 24;
    ret = ret | (((quint64)(*(p + 2)) & 0x00000000000000FF) << 16);
    ret = ret | (((quint64)(*(p + 1)) & 0x00000000000000FF) << 8);
    ret = ret | (quint64)(*p) & 0x00000000000000FF;
    return ret;
}

inline QByteArray int64ToChar(quint64 l)
{
    char c[4];
    c[0] = (char)((l & 0x00000000000000FF));
    c[1] = (char)((l & 0x000000000000FF00) >> 8);
    c[2] = (char)((l & 0x0000000000FF0000) >> 16);
    c[3] = (char)((l & 0x00000000FF000000) >> 24);
    
    QByteArray ret(c, 4);
    
    return ret;
}


inline bool odd(XWInteger x)
{
    return (x & 1);
}

inline QByteArray intToByte(int l)
{
    char c[CNST_BYTE_NUM_INT + 1];
        
    c[0] = (uchar)(l & 0x00000000000000FF);
    c[1] = (uchar)((l & 0x000000000000FF00) >> 8);
    c[2] = (uchar)((l & 0x0000000000FF0000) >> 16);
    c[3] = (uchar)((l & 0x00000000FF000000) >> 24);
    c[4] = (uchar)((l & 0x000000FF00000000) >> 32);
    c[5] = (uchar)((l & 0x0000FF0000000000) >> 40);
    c[6] = (uchar)((l & 0x00FF000000000000) >> 48);
    c[7] = (uchar)((l & 0xFF00000000000000) >> 56);
    
    c[CNST_BYTE_NUM_INT] = '\0';
    
    return QByteArray(c, CNST_BYTE_NUM_INT);
}

inline double round(double v)
{
    return (floor(v) + 0.5);
}

inline double round(double n, double acc)
{
    return (floor((n / acc + 0.5)) * acc);
}

inline double roundAt(double v, double acc)
{
    return (round(((double)v) / acc) * acc);
}

//用tex字体缩放因子sq计算fixword表示的长、宽、高
XW_UTIL_EXPORT long scaleFixWord(long sq, long fw);

//将num转换为fixword
XW_UTIL_EXPORT long toFixWord(double num);

#endif // XWNUMBERUTIL_H
