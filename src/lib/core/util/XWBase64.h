/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWBASE64_H
#define XWBASE64_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QBuffer>

#include "XWGlobal.h"

class XW_UTIL_EXPORT XWBase64 : public QObject
{
    Q_OBJECT
    
public:
    XWBase64(QObject * parent = 0);
    ~XWBase64() {}
    
    QByteArray decrypt(const QByteArray & txt);
    QString    decrypt(const QString & txt);
    
    QByteArray encrypt(const QByteArray & txt);
    QString    encrypt(const QString & txt);
    
private:
    void decode(QBuffer & in, QBuffer & out);
    void encode(QBuffer & in, QBuffer & out);
    
private:
    static const int inEBufferSize;
    static const int outEBufferSize;
    static const int inDBufferSize;
    static const int outDBufferSize;
};

#endif // XWBASE64_H

