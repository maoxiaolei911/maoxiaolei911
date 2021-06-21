/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWAES_H
#define XWAES_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QBuffer>

#include "XWGlobal.h"

/* AES加解密
 *
 */
class XW_UTIL_EXPORT XWAes : public QObject
{
    Q_OBJECT
    
public:
    XWAes(QObject * parent = 0);
    ~XWAes() {}
    
    QByteArray decrypt128(const QByteArray & txt, const QByteArray & key);
    QString decrypt128(const QString & txt, const QString & key);
    QByteArray decrypt192(const QByteArray & txt, const QByteArray & key);
    QString decrypt192(const QString & txt, const QString & key);
    QByteArray decrypt256(const QByteArray & txt, const QByteArray & key);
    QString decrypt256(const QString & txt, const QString & key);
    
    QByteArray encrypt128(const QByteArray & txt, const QByteArray & key);
    QString encrypt128(const QString & txt, const QString & key);
    QByteArray encrypt192(const QByteArray & txt, const QByteArray & key);
    QString encrypt192(const QString & txt, const QString & key);
    QByteArray encrypt256(const QByteArray & txt, const QByteArray & key);
    QString encrypt256(const QString & txt, const QString & key);
    
private:
    void decrypt128(const QByteArray & buffer, 
                    const quint64 * key, 
                    QByteArray & out);
    void decrypt192(const QByteArray & buffer, 
                    const quint64 *  key, 
                    QByteArray & out);
    void decrypt256(const QByteArray & buffer, 
                    const quint64 *  key, 
                    QByteArray & out);
                       
    void decryptCBC128(QBuffer & source, 
                       qint64 count, 
                       const QByteArray & key, 
                       const QByteArray & inibuffer,
                       QBuffer & dest);
    void decryptCBC128(QBuffer & source, 
                       qint64 count, 
                       const quint64 * key, 
                       const QByteArray & inibuffer,
                       QBuffer & dest);
    void decryptCBC192(QBuffer & source, 
                       qint64 count, 
                       const QByteArray & key, 
                       const QByteArray & inibuffer,
                       QBuffer & dest);
    void decryptCBC192(QBuffer & source, 
                       qint64 count, 
                       const quint64 * key, 
                       const QByteArray & inibuffer,
                       QBuffer & dest);
    void decryptCBC256(QBuffer & source, 
                       qint64 count, 
                       const QByteArray & key, 
                       const QByteArray & inibuffer,
                       QBuffer & dest);
    void decryptCBC256(QBuffer & source, 
                       qint64 count, 
                       const quint64 * key, 
                       const QByteArray & inibuffer,
                       QBuffer & dest);
    void decryptECB128(QBuffer & source, 
                       qint64 count, 
                       const QByteArray & key, 
                       QBuffer & dest);
    void decryptECB128(QBuffer & source, 
                       qint64 count, 
                       const quint64 * key, 
                       QBuffer & dest);
    void decryptECB192(QBuffer & source, 
                       qint64 count, 
                       const QByteArray & key, 
                       QBuffer & dest);
    void decryptECB192(QBuffer & source, 
                       qint64 count, 
                       const quint64 * key, 
                       QBuffer & dest);
    void decryptECB256(QBuffer & source, 
                       qint64 count, 
                       const QByteArray & key, 
                       QBuffer & dest);
    void decryptECB256(QBuffer & source, 
                       qint64 count, 
                       const quint64 * key, 
                       QBuffer & dest);
                       
    void encrypt128(const QByteArray & buffer, 
                    const quint64 * key, 
                    QByteArray & out);
    void encrypt192(const QByteArray & buffer, 
                    const quint64 * key, 
                    QByteArray & out);
    void encrypt256(const QByteArray & buffer, 
                    const quint64 * key, 
                    QByteArray & out);
                       
    void encryptCBC128(QBuffer & source, 
                       qint64 count, 
                       const QByteArray & key, 
                       const QByteArray & inibuffer,
                       QBuffer & dest);
    void encryptCBC128(QBuffer & source, 
                       qint64 count, 
                       const quint64 * key, 
                       const QByteArray & inibuffer,
                       QBuffer & dest);
    void encryptCBC192(QBuffer & source, 
                       qint64 count, 
                       const QByteArray & key, 
                       const QByteArray & inibuffer,
                       QBuffer & dest);
    void encryptCBC192(QBuffer & source, 
                       qint64 count, 
                       const quint64 * key, 
                       const QByteArray & inibuffer,
                       QBuffer & dest);
    void encryptCBC256(QBuffer & source, 
                       qint64 count, 
                       const QByteArray & key, 
                       const QByteArray & inibuffer,
                       QBuffer & dest);
    void encryptCBC256(QBuffer & source, 
                       qint64 count, 
                       const quint64 * key, 
                       const QByteArray & inibuffer,
                       QBuffer & dest);
    void encryptECB128(QBuffer & source, 
                       qint64 count, 
                       const QByteArray & key, 
                       QBuffer & dest);
    void encryptECB128(QBuffer & source, 
                       qint64 count, 
                       const quint64 * key, 
                       QBuffer & dest);
    void encryptECB192(QBuffer & source, 
                       qint64 count, 
                       const QByteArray & key, 
                       QBuffer & dest);
    void encryptECB192(QBuffer & source, 
                       qint64 count, 
                       const quint64 * key, 
                       QBuffer & dest);
    void encryptECB256(QBuffer & source, 
                       qint64 count, 
                       const QByteArray & key, 
                       QBuffer & dest);
    void encryptECB256(QBuffer & source, 
                       qint64 count, 
                       const quint64 * key, 
                       QBuffer & dest);
    
    
    void expandKeyForDecrypt128(quint64 * ekey);
    void expandKeyForDecrypt192(quint64 * ekey);
    void expandKeyForDecrypt256(quint64 * ekey);
    void expandKeyForDecrypt128(const QByteArray & key, quint64 * ekey);
    void expandKeyForDecrypt192(const QByteArray & key, quint64 * ekey);
    void expandKeyForDecrypt256(const QByteArray & key, quint64 * ekey);
    void expandKeyForEncrypt128(const QByteArray & key, quint64 * ekey);
    void expandKeyForEncrypt192(const QByteArray & key, quint64 * ekey);
    void expandKeyForEncrypt256(const QByteArray & key, quint64 * ekey);
    
    QByteArray fromHex(const QByteArray & str);
    
    QByteArray toHex(const QByteArray & str);
    
private:
    static const int bufferSize;
    static const int key128Size;
    static const int key192Size;
    static const int key256Size;
    
    static const int expandedKey128Size;
    static const int expandedKey192Size;
    static const int expandedKey256Size;
};


#endif // XWAES_H

