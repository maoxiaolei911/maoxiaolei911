/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWUtil.h"
#include "XWDes.h"

static const uchar bitIP[] = {57, 49, 41, 33, 25, 17,  9,  1,
                             59, 51, 43, 35, 27, 19, 11,  3,
                             61, 53, 45, 37, 29, 21, 13,  5,
                             63, 55, 47, 39, 31, 23, 15,  7,
                             56, 48, 40, 32, 24, 16,  8,  0,
                             58, 50, 42, 34, 26, 18, 10,  2,
                             60, 52, 44, 36, 28, 20, 12,  4,
                             62, 54, 46, 38, 30, 22, 14,  6};
                             
static const uchar bitCP[] = {39,  7, 47, 15, 55, 23, 63, 31,
                             38,  6, 46, 14, 54, 22, 62, 30,
                             37,  5, 45, 13, 53, 21, 61, 29,
                             36,  4, 44, 12, 52, 20, 60, 28,
                             35,  3, 43, 11, 51, 19, 59, 27,
                             34,  2, 42, 10, 50, 18, 58, 26,
                             33,  1, 41,  9, 49, 17, 57, 25,
                             32,  0, 40,  8, 48, 16, 56, 24};
                             
static const uchar bitEXP[] = {31, 0, 1, 2, 3, 4, 3, 4, 5, 6, 7, 8, 7, 8, 9,10,
                              11,12,11,12,13,14,15,16,15,16,17,18,19,20,19,20,
                              21,22,23,24,23,24,25,26,27,28,27,28,29,30,31,0};
                              
static const uchar bitPM[] = {15, 6,19,20,28,11,27,16, 0,14,22,25, 4,17,30, 9,
                             1, 7,23,13,31,26, 2, 8,18,12,29, 5,21,10, 3,24};
                             
static const uchar sbox[][64] = {
                {14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
                 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
                 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
                 15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13},
                 
                 {15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
                  3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
                  0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
                  13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9},
                  
                 {10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
                  13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
                  13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
                  1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12},
                  
                 {7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
                  13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
                  10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
                  3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14},
                  
                 {2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
                  14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
                  4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
                  11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3},
                  
                 {12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
                  10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
                  9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
                  4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13},
                  
                 {4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
                  13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
                  1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
                  6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12},
                  
                 {13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
                  1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
                  7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
                  2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11}};
                  
static const uchar bitPMC1[] = {56, 48, 40, 32, 24, 16,  8,
                               0, 57, 49, 41, 33, 25, 17,
                               9,  1, 58, 50, 42, 34, 26,
                               18, 10,  2, 59, 51, 43, 35,
                               62, 54, 46, 38, 30, 22, 14,
                               6, 61, 53, 45, 37, 29, 21,
                               13,  5, 60, 52, 44, 36, 28,
                               20, 12,  4, 27, 19, 11,  3};
                             
static const uchar bitPMC2[] = {13, 16, 10, 23,  0,  4,
                               2, 27, 14,  5, 20,  9,
                               22, 18, 11,  3, 25,  7,
                               15,  6, 26, 19, 12,  1,
                               40, 51, 30, 36, 46, 54,
                               29, 39, 50, 44, 32, 47,
                               43, 48, 38, 55, 33, 52,
                               45, 41, 49, 35, 28, 31};
                               
static const uchar bitDisplace[] = {1,1,2,2, 2,2,2,2, 1,2,2,2, 2,2,2,1};

XWDes::XWDes(QObject * parent)
    :QObject(parent)
{
}

QString XWDes::decrypt(const QString & txt, const QString & key)
{
    QByteArray k = key.toUtf8();
    QByteArray t = txt.toAscii();
    
    QByteArray res = decrypt(t, k);
    
    return QString::fromUtf8(res.data());
}

QByteArray XWDes::decrypt(const QByteArray & txt, const QByteArray & key)
{
    QByteArray t = QByteArray::fromHex(txt);
    return decryptArray(t, key);
}

QString XWDes::doubleDecrypt(const QString & txt, 
                             const QString & key1, 
                             const QString & key2)
{
    QByteArray k1 = key1.toUtf8();
    QByteArray k2 = key2.toUtf8();
    
    QByteArray t = txt.toAscii();
    QByteArray res = doubleDecrypt(t, k1, k2);
    
    return QString::fromUtf8(res.data());
}

QByteArray XWDes::doubleDecrypt(const QByteArray & txt, 
                                const QByteArray & key1, 
                                const QByteArray & key2)
{
    QByteArray res = decrypt(txt, key2);
    
    return decrypt(res, key1);
}

QString  XWDes::doubleEncrypt(const QString & txt, 
                              const QString & key1, 
                              const QString & key2)
{
    QByteArray k1 = key1.toUtf8();
    QByteArray k2 = key2.toUtf8();
    QByteArray t = txt.toUtf8();
    QByteArray res = doubleEncrypt(t, k1, k2);
    
    return QString(res);
}

QByteArray XWDes::doubleEncrypt(const QByteArray & txt, 
                                const QByteArray & key1, 
                                const QByteArray & key2)
{
    QByteArray res = encrypt(txt, key1);
    
    res = encrypt(res, key2);
    return res;
}

QString XWDes::encrypt(const QString & txt, const QString & key)
{
    QByteArray t = txt.toAscii();
    QByteArray k = key.toUtf8();
    
    QByteArray res = encrypt(t, k);
    
    return QString(res);
}

QByteArray XWDes::encrypt(const QByteArray & txt, const QByteArray & key)
{
    QByteArray res = encryptArray(txt, key);
    return res.toHex();
}

QString XWDes::tripDecrypt(const QString & txt, 
                           const QString & key1, 
                           const QString & key2,
                           const QString & key3)
{
    QByteArray k1 = key1.toUtf8();
    QByteArray k2 = key2.toUtf8();
    QByteArray k3 = key3.toUtf8();
    
    QByteArray t = txt.toAscii();
    
    QByteArray res = tripDecrypt(t, k1, k2, k3);
    
    return QString::fromUtf8(res.data());
}

QByteArray XWDes::tripDecrypt(const QByteArray & txt, 
                              const QByteArray & key1, 
                              const QByteArray & key2,
                              const QByteArray & key3)
{
    QByteArray res = decrypt(txt, key3);
    
    res = decrypt(res, key2);
    
    return decrypt(res, key1);
}

QString XWDes::tripEncrypt(const QString & txt, 
                           const QString & key1, 
                           const QString & key2,
                           const QString & key3)
{
    QByteArray k1 = key1.toUtf8();
    QByteArray k2 = key2.toUtf8();
    QByteArray k3 = key3.toUtf8();
    QByteArray t = txt.toUtf8();
    
    QByteArray res = tripEncrypt(t, k1, k2, k3);
    
    return QString(res);
}

QByteArray XWDes::tripEncrypt(const QByteArray & txt, 
                              const QByteArray & key1, 
                              const QByteArray & key2,
                              const QByteArray & key3)
{
    QByteArray res = encrypt(txt, key1);
    res = encrypt(res, key2);
    res = encrypt(res, key3);
    
    return res;
}

void XWDes::cycleMove(QByteArray & inData, uchar byte)
{
    for (int i = 0; i < byte; i++)
    {
        inData[0] = (((uchar)inData[0]) << 1) | (((uchar)inData[1]) >> 7);
        inData[1] = (((uchar)inData[1]) << 1) | (((uchar)inData[2]) >> 7);
        inData[2] = (((uchar)inData[2]) << 1) | (((uchar)inData[3]) >> 7);
        inData[3] = (((uchar)inData[3]) << 1) | ((((uchar)inData[0]) & 0x10) >> 4);
        inData[0] = ((uchar)inData[0]) & 0x0f;
    }
}

void XWDes::conversePermutation(QByteArray & inData)
{
    permutation(bitCP, inData);
}

QByteArray XWDes::decryptArray(const QByteArray & txt, const QByteArray & key)
{
    QByteArray k = key;
    padKey(k);
    
    QByteArray KeyByte = k;
    if (k.size() > 8)
        KeyByte = KeyByte.left(8);
        
    makeKey(KeyByte, m_subKey);
    
    QByteArray ret;
    
    QByteArray StrByte(8, 0);
    QByteArray OutByte(8, 0);
    
    int m = txt.size() / 8;
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < 8; j++)
            StrByte[j] = txt[i * 8 + j];
            
        desData(Decry, StrByte, OutByte);
        ret += OutByte;
    }
    
    while (ret.at(ret.size() - 1) == 0)
        ret.remove(ret.size() - 1, 1);
        
    return ret;
}

void XWDes::desData(DestMode mode, 
                    const QByteArray & inData, 
                    QByteArray & outData)
{
    for (int i = 0; i < 8; i++)
        outData[i] = inData[i];
        
    initPermutation(outData);
    
    QByteArray temp(4, 0);
    QByteArray buf(4, 0);
    if (mode == Encry)
    {
        for (int i = 0; i < 16; i++)
        {
            for (int j = 0; j < 4; j++)
                temp[j] = outData[j];
                
            for (int j = 0; j < 4; j++)
                outData[j] = outData[j + 4];
                
            encry(outData, &m_subKey[i][0], buf);
            
            for (int j = 0; j < 4; j++)
                outData[j + 4] = temp[j] ^ buf[j];
        }
        
        for (int j = 0; j < 4; j++)
            temp[j] = outData[j + 4];
            
        for (int j = 0; j < 4; j++)
            outData[j + 4] = outData[j];
            
        for (int j = 0; j < 4; j++)
            outData[j] = temp[j];
    }
    else if (mode == Decry)
    {
        for (int i = 15; i > -1; i--)
        {
            for (int j = 0; j < 4; j++)
                temp[j] = outData[j];
                
            for (int j = 0; j < 4; j++)
                outData[j] = outData[j + 4];
                
            encry(outData, &m_subKey[i][0], buf);
            
            for (int j = 0; j < 4; j++)
                outData[j + 4] = temp[j] ^ buf[j];
        }
        
        for (int j = 0; j < 4; j++)
            temp[j] = outData[j + 4];
            
        for (int j = 0; j < 4; j++)
            outData[j + 4] = outData[j];
            
        for (int j = 0; j < 4; j++)
            outData[j] = temp[j];        
    }

    conversePermutation(outData);
}

void XWDes::encry(const QByteArray & inData, 
                  uchar * subKey, 
                  QByteArray & outData)
{
    QByteArray outBuf(6, 0);
    expand(inData, outBuf);
    for (int i = 0; i < 6; i++)
        outBuf[i] = outBuf[i] ^ subKey[i];
        
    QByteArray buf(8, 0);
    buf[0] = ((uchar)outBuf[0]) >> 2;
    buf[1] = (((uchar)(outBuf[0] & 0x03)) << 4) | ((uchar)(outBuf[1]) >> 4);
    buf[2] = (((uchar)(outBuf[1] & 0x0f)) << 2) | ((uchar)(outBuf[2]) >> 6);
    buf[3] = ((uchar)outBuf[2]) & 0x3f;
    buf[4] = ((uchar)outBuf[3]) >> 2;
    buf[5] = (((uchar)(outBuf[3] & 0x03)) << 4) | (((uchar)outBuf[4]) >> 4);
    buf[6] = (((uchar)(outBuf[4] & 0x0f)) << 2) | (((uchar)outBuf[5]) >> 6);
    buf[7] = ((uchar)outBuf[5]) & 0x3f;
    
    for (int i = 0; i < 8; i++)
        buf[i] = si(i, (uchar)buf[i]);
        
    for (int i = 0; i < 4; i++)
        outBuf[i] = (((uchar)buf[i * 2]) << 4) | ((uchar)(buf[i * 2 + 1]));
        
    permutation(outBuf);
    
    for (int i = 0; i < 4; i++)
        outData[i] = outBuf[i];
}

QByteArray XWDes::encryptArray(const QByteArray & txt, const QByteArray & key)
{
    QByteArray k = key;
    QByteArray t = txt;
    
    padKey(k);
    padText(t);
    
    QByteArray KeyByte = k;
    if (k.size() > 8)
        KeyByte = KeyByte.left(8);
            
    makeKey(KeyByte, m_subKey);
    
    QByteArray ret;
    
    QByteArray StrByte(8, 0);
    QByteArray OutByte(8, 0);
    
    int m = t.size() / 8;
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < 8; j++)
            StrByte[j] = t[i * 8 + j];
            
        desData(Encry, StrByte, OutByte);
        for (int j = 0; j < 8; j++)
            ret += OutByte[j];
    }
    
    return ret;
}

void XWDes::expand(const QByteArray & inData, QByteArray & outData)
{
    expand(bitEXP, inData, outData);
}

void XWDes::expand(const uchar * table, 
                   const QByteArray & inData, 
                   QByteArray & outData)
{
    outData.fill(0, 6);
    
    for (int i = 0; i < 48; i++)
    {
        if ((((uchar)inData[table[i] >> 3]) & (1 << (7 - (table[i] & 0x07)))))
            outData[((uchar)i) >> 3] = outData[((uchar)i) >> 3] | (1 << (7 - (i & 0x07)));
    }
}

void XWDes::initPermutation(QByteArray & inData)
{
    permutation(bitIP, inData);
}

void XWDes::makeKey(const QByteArray & inKey, uchar  outKey[][6])
{
    QByteArray outData56(7, 0);
    permutationChoose1(inKey, outData56);
    
    QByteArray key28l(4, 0);
    key28l[0] = ((uchar)outData56[0]) >> 4;
    key28l[1] = (((uchar)outData56[0]) << 4) | (((uchar)outData56[1]) >> 4);
    key28l[2] = (((uchar)outData56[1]) << 4) | (((uchar)outData56[2]) >> 4);
    key28l[3] = (((uchar)outData56[2]) << 4) | (((uchar)outData56[3]) >> 4);
    
    QByteArray key28r(4, 0);
    key28r[0] = outData56[3] & 0x0f;
    key28r[1] = outData56[4];
    key28r[2] = outData56[5];
    key28r[3] = outData56[6];
    
    QByteArray key56o(7, 0);
    for (int i = 0; i < 16; i++)
    {
        cycleMove(key28l, bitDisplace[i]);
        cycleMove(key28r, bitDisplace[i]);
        
        key56o[0] = (((uchar)key28l[0]) << 4) | (((uchar)key28l[1]) >> 4);
        key56o[1] = (((uchar)key28l[1]) << 4) | (((uchar)key28l[2]) >> 4);
        key56o[2] = (((uchar)key28l[2]) << 4) | (((uchar)key28l[3]) >> 4);
        key56o[3] = (((uchar)key28l[3]) << 4) | ((uchar)key28r[0]);
        key56o[4] = key28r[1];
        key56o[5] = key28r[2];
        key56o[6] = key28r[3];
        
        permutationChoose2(key56o, &outKey[i][0]);
    }
}

void XWDes::padKey(QByteArray & key)
{
    int len = key.size();
    if (len < 8)
    {
        key.resize(8);
        for (int i = len; i < 8; i++)
            key[i] = 0;
    }
}

void XWDes::padText(QByteArray & txt)
{
    while ((txt.size() % 8) != 0)
        txt.append((char)0);
}

void XWDes::permutation(QByteArray & inData)
{
    QByteArray newData(4, 0);
    for (int i = 0; i < 32; i++)
    {
        if ((((uchar)inData[bitPM[i] >> 3]) & (1 << (7 - (bitPM[i] & 0x7)))))
            newData[((uchar)i) >> 3] = newData[((uchar)i) >> 3] | (1 << (7 - (i & 0x7)));
    }
    
    for (int i = 0; i < 4; i++)
        inData[i] = newData[i];
}

void XWDes::permutation(const uchar * table, QByteArray & inData)
{
    QByteArray newData(8, 0);
    
    for (int i = 0; i < 64; i++)
    {
        if ((((uchar)inData[table[i] >> 3]) & (1 << (7 - (table[i] & 0x7)))))
            newData[((uchar)i) >> 3] = newData[((uchar)i) >> 3] | (1 << (7 - (i & 0x7)));
    }
    
    for (int i = 0; i < 8; i++)
        inData[i] = newData[i];
}

void XWDes::permutationChoose1(const QByteArray & inData, QByteArray & outData)
{
    outData.fill(0, 7);
    
    for (int i = 0; i < 56; i++)
    {
        if ((((uchar)inData[bitPMC1[i] >> 3]) & (1 << (7 - (bitPMC1[i] & 0x07)))) != 0)
            outData[((uchar)i) >> 3] = outData[((uchar)i) >> 3] | (1 << (7 - (i & 0x07)));
    }
}

void XWDes::permutationChoose2(const QByteArray & inData, uchar * outData)
{
    for (int i = 0; i < 6; i++)
        outData[i] = 0;
    
    for (int i = 0; i < 48; i++)
    {
        if ((((uchar)inData[bitPMC2[i] >> 3]) & (1 << (7 - (bitPMC2[i] & 0x07)))))
            outData[((uchar)i) >> 3] = outData[((uchar)i) >> 3] | (1 << (7 - (i & 0x07)));
    }
}

uchar XWDes::si(uchar s, uchar inByte)
{
    uchar c = ((inByte & 0x20) | ((inByte & 0x1e) >> 1) | ((inByte & 0x01) << 4));
    
    return sbox[s][c] & 0x0f;
}

