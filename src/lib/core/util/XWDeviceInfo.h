/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWDEVICEINFO_H
#define XWDEVICEINFO_H

#include <QObject>
#include <QString>

#include "XWGlobal.h"

class XW_UTIL_EXPORT XWDeviceInfo : public QObject
{
    Q_OBJECT
    
public:    
    XWDeviceInfo(const QString & devPath, QObject * parent = 0);
    XWDeviceInfo(QObject * parent = 0);
    ~XWDeviceInfo() {}
    
    QString devicePath();
    QString driver();
    
    QString serialNumber();
    void    setDevPath(const QString & devPath);
    
    QString type();
    
private:
    QString m_devPath;
    QString m_driver;
    
    QString m_serialNumber;
    
    QString m_type;
};

inline QString XWDeviceInfo::devicePath()
{
    return m_devPath;
}

inline QString XWDeviceInfo::driver()
{
    return m_driver;
}

inline QString XWDeviceInfo::serialNumber()
{
    return m_serialNumber;
}

inline QString XWDeviceInfo::type()
{
    return m_type;
}

#endif // XWDEVICEINFO_H

