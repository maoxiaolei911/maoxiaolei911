/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWBase64.h"

static const char base64Table[] = {
    65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  
    75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  
    85,  86,  87,  88,  89,  90,  97,  98,  99,  100, 
    101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 
    111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 
    121, 122,  48,  49,  50, 51,  52,  53,  54,  55,  
    56,  57,  43,  47};
    
static const char base64DeTable[] = {
    0x3E, 0x7F, 0x7F, 0x7F, 0x3F, 0x34, 0x35, 0x36, 0x37, 0x38, 
    0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 
    0x7F, 0x7F, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 
    0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x7F, 0x7F, 
    0x7F, 0x7F, 0x7F, 0x7F, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 
    0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33};
    
const int XWBase64::inEBufferSize = 45;
const int XWBase64::outEBufferSize = 63;
const int XWBase64::inDBufferSize = 88;
const int XWBase64::outDBufferSize = 66;
    
XWBase64::XWBase64(QObject * parent)
    :QObject(parent)
{
}
    
QByteArray XWBase64::decrypt(const QByteArray & txt)
{
    QBuffer in;
    in.open(QIODevice::ReadWrite);
    in.write(txt.data(), txt.size());
    in.seek(0);
    
    QBuffer out;
    out.open(QIODevice::ReadWrite);
        
    decode(in, out);
    
    out.seek(0);
    
    return out.data();
}

QString  XWBase64::decrypt(const QString & txt)
{
    QByteArray t = txt.toAscii();
    QByteArray ret = decrypt(t);
    
    return QString::fromUtf8(ret.data());
}
    
QByteArray XWBase64::encrypt(const QByteArray & txt)
{
    QBuffer in;
    in.open(QIODevice::ReadWrite);
    in.write(txt.data(), txt.size());
    in.seek(0);
    
    QBuffer out;
    out.open(QIODevice::ReadWrite);
        
    encode(in, out);
    
    out.seek(0);
    
    return out.data();
}

QString XWBase64::encrypt(const QString & txt)
{
    QByteArray t = txt.toUtf8();
    QByteArray ret = encrypt(t);
    
    return QString(ret);
}
    
void XWBase64::decode(QBuffer & in, QBuffer & out)
{
    char inbuffer[inDBufferSize + 1];
    
    QByteArray outbuffer(outDBufferSize, 0);
        
    int count = 0;
    while ((count = in.read(inbuffer, inDBufferSize)) > 0)
    {
        int i = 0;
        int o = 0;
        
        while (i < count)
        {
            if ((inbuffer[i] < 43) | (inbuffer[i] > 122) | (inbuffer[i + 1] < 43) | 
                (inbuffer[i + 1] > 122) | (inbuffer[i + 2] < 43) | (inbuffer[i + 2] > 122) | 
                (inbuffer[i + 3] < 43) | (inbuffer[i + 3] > 122))
            {
                break;
            }
            
            char c1 = base64DeTable[inbuffer[i] - 43];
            char c2 = base64DeTable[inbuffer[i + 1] - 43];
            char c3 = base64DeTable[inbuffer[i + 2] - 43];
            outbuffer[o] = ((c1 << 2) | (c2 >> 4));
            
            o++;
            
            if (inbuffer[i + 2] != '=')
            {
                outbuffer[o] = ((c2 << 4) | (c3 >> 2));
                o++;
                if (inbuffer[i + 3] != '=')
                {
                    outbuffer[o] = ((c3 << 6) | base64DeTable[inbuffer[i + 3] - 43]);
                    o++;
                }
            }
            
            i += 4;
        }
        
        out.write(outbuffer, o);
    }
}
    
void XWBase64::encode(QBuffer & in, QBuffer & out)
{
    char inbuffer[inEBufferSize + 2];
    
    QByteArray outbuffer(outEBufferSize, 0);
    
    int count = 0;
    while ((count = in.read(inbuffer + 1, inEBufferSize)) > 0)
    {
        int i = 1;
        int o = 0;
        
        while (i <= (count - 2))
        {
            char tmp = inbuffer[i] >> 2;
            outbuffer[o] = base64Table[tmp & 0x3f];
            
            tmp = (inbuffer[i] << 4) | (inbuffer[i + 1] >> 4);
            outbuffer[o + 1] = base64Table[tmp & 0x3f];
            
            tmp = (inbuffer[i + 1] << 2) | (inbuffer[i + 2] >> 6);
            outbuffer[o + 2] = base64Table[tmp & 0x3f];
            
            tmp = (inbuffer[i + 2] & 0x3f);
            outbuffer[o + 3] = base64Table[tmp];
            
            i += 3;
            o += 4;
        }
        
        if (i <= count)
        {
            char tmp = (inbuffer[i] >> 2);
            outbuffer[o] = base64Table[tmp & 0x3f];
            if (i == count)
            {
                tmp = (inbuffer[i] << 4) & 0x30;
                outbuffer[o + 1] = base64Table[tmp & 0x3f];
                outbuffer[o + 2] = '=';
            }
            else
            {
                tmp = ((inbuffer[i] << 4) & 0x30) | ((inbuffer[i + 1] >> 4) & 0x0F);
                outbuffer[o + 1] = base64Table[tmp & 0x3f];
                tmp = (inbuffer[i + 1] << 2) & 0x3c;
                outbuffer[o + 2] = base64Table[tmp & 0x3f];
            }
            
            outbuffer[o + 3] = '=';
            
            o += 4;
        }
        
        out.write(outbuffer);
    }
}
