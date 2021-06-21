/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWDES_H
#define XWDES_H

#include <QObject>
#include <QString>
#include <QByteArray>

#include "XWGlobal.h"

class XW_UTIL_EXPORT XWDes : public QObject
{
    Q_OBJECT
    
public:
    XWDes(QObject * parent = 0);
    ~XWDes() {}
    
    QString    decrypt(const QString & txt, const QString & key);
    QByteArray decrypt(const QByteArray & txt, const QByteArray & key);
    QString    doubleDecrypt(const QString & txt, 
                             const QString & key1, 
                             const QString & key2);
    QByteArray doubleDecrypt(const QByteArray & txt, 
                             const QByteArray & key1, 
                             const QByteArray & key2);
    
    QString    doubleEncrypt(const QString & txt, 
                             const QString & key1, 
                             const QString & key2);
    QByteArray doubleEncrypt(const QByteArray & txt, 
                             const QByteArray & key1, 
                             const QByteArray & key2);
    QString    encrypt(const QString & txt, const QString & key);
    QByteArray encrypt(const QByteArray & txt, const QByteArray & key);
    
    QString    tripDecrypt(const QString & txt, 
                           const QString & key1, 
                           const QString & key2,
                           const QString & key3);
    QByteArray tripDecrypt(const QByteArray & txt, 
                           const QByteArray & key1, 
                           const QByteArray & key2,
                           const QByteArray & key3);
    QString    tripEncrypt(const QString & txt, 
                           const QString & key1, 
                           const QString & key2,
                           const QString & key3);
    QByteArray tripEncrypt(const QByteArray & txt, 
                           const QByteArray & key1, 
                           const QByteArray & key2,
                           const QByteArray & key3);
private:
    enum DestMode
    {
        Encry = 0,
        Decry = 1
    };
    
    void cycleMove(QByteArray & inData, uchar byte);
    void conversePermutation(QByteArray & inData);
    
    QByteArray decryptArray(const QByteArray & txt, const QByteArray & key);
    void desData(DestMode mode, 
                 const QByteArray & inData, 
                 QByteArray & outData);
    
    void encry(const QByteArray & inData, 
               uchar * subKey, 
               QByteArray & outData);
    QByteArray encryptArray(const QByteArray & txt, const QByteArray & key);
    void expand(const QByteArray & inData, QByteArray & outData);
    void expand(const uchar * table, 
                const QByteArray & inData, 
                QByteArray & outData);
    
    void initPermutation(QByteArray & inData);
    
    void makeKey(const QByteArray & inKey, uchar  outKey[][6]);
    
    void padKey(QByteArray & key);
    void padText(QByteArray & txt);
    
    void permutation(QByteArray & inData);
    void permutation(const uchar * table, QByteArray & inData);
    void permutationChoose1(const QByteArray & inData, QByteArray & outData);
    void permutationChoose2(const QByteArray & inData, uchar * outData);
    
    uchar si(uchar s, uchar inByte);
    
private:
    uchar m_subKey[16][6];
};

#endif // XWDES_H

