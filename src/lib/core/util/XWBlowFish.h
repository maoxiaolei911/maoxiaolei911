/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWBLOWFISH_H
#define XWBLOWFISH_H

#include <QObject>
#include <QString>
#include <QByteArray>

#include "XWGlobal.h"

class XW_UTIL_EXPORT XWBlowFish : public QObject
{
    Q_OBJECT
    
public:
    enum CipherMode
    {
        ECB = 0,
        CBC = 1,
        CFB = 2,
        OFB = 3
    };
    
    enum StringMode
    {
        Encode = 0,
        Normal = 1
    };
    
    XWBlowFish(QObject * parent = 0);
    ~XWBlowFish() {}
    
    void burn();
    
    void decBlock(const QByteArray & input, QByteArray & output);
    void decBuffer(int len);
    QByteArray decrypt(const QByteArray & txt, const QByteArray & key);
    QString    decrypt(const QString & txt, const QString & key);
    void decString(const QByteArray & input, QByteArray & output);
    
    void encBlock(const QByteArray & input, QByteArray & output);
    void encBuffer(int len);
    QByteArray encrypt(const QByteArray & txt, const QByteArray & key);
    QString    encrypt(const QString & txt, const QString & key);
    void encString(const QByteArray & input, QByteArray & output);
    
    QString getVersion();
    
    CipherMode getCipherMode();
    StringMode getStringMode();
    
    void initialiseString(const QByteArray & key);
    
    void LoadIVString(const QByteArray & ivstr);
    
    void setCipherMode(const CipherMode mode);
    void setStringMode(const StringMode mode);
    
signals:
    void error(const QString & msg);
    
private:
    struct SingleBytes
    {
        uchar byte3; 
        uchar byte2;
        uchar byte1;
        uchar byte0;
    };
    
    union DoublWord
    {
        qint32         lword;
        SingleBytes  w;
        uchar         fByte[4];
    };
        
    struct BlfCoreCtx
    {
        qint32  p[18];
        qint32  s1[256];
        qint32  s2[256];
        qint32  s3[256];
        qint32  s4[256];
        
        QByteArray byteBuffer;
        qint32  longBuffer[2];
    };
    
    struct BlfCtx
    {
        bool  keyInit;
        bool  ivInit;
        QByteArray  iv;
        QByteArray  ct;
    };
    
    class BlowCore
    {
    public:
        BlowCore() {}
        ~BlowCore() {}
        
        void encrypt();
        void decrypt();
        
    public:
        BlfCoreCtx   ctx;
        DoublWord  * fptrl;
        DoublWord  * fptrr;
    };
    
    void   checkKeys();
    
    char       decodeBase64(char b);
    QByteArray decodeString(const QByteArray & inputStr);
    void   decryptBlockMode();
    
    QByteArray encodeString(QByteArray & inputStr);
    void   encryptBlockMode();
    void   endianDecBlock();
    void   endianEncBlock();
    
    void initArray();
    
    void keySetup(QByteArray & keyArray, int len);
    
private:
    static const int blockSize;
    static const int bufferSize;
    
    
    CipherMode m_cipherMode;
    
    BlfCtx     ctx;
    BlowCore   core;
    
    uchar   m_fbuffer[4105];
    
    uchar * m_ptrBuffer;
    
    
    StringMode m_stringMode;
};

inline XWBlowFish::CipherMode XWBlowFish::getCipherMode()
{
    return m_cipherMode;
}

inline XWBlowFish::StringMode XWBlowFish::getStringMode()
{
    return m_stringMode;
}
    
inline void XWBlowFish::setCipherMode(const CipherMode mode)
{
    m_cipherMode = mode;
}

inline void XWBlowFish::setStringMode(const StringMode mode)
{
    m_stringMode = mode;
}

#endif // XWBLOWFISH_H

