/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWFILENAME_H
#define XWFILENAME_H

#include <QObject>
#include <QString>

#include "XWGlobal.h"

class XW_UTIL_EXPORT XWFileName : public QObject
{
    Q_OBJECT
    
public:
	//��ָ����·�������׼�ļ�·���������·���ָ�������ȥ��
    XWFileName(const char * _path, QObject * parent = 0);
    XWFileName(const QString & _path, QObject * parent = 0);
    XWFileName(const QChar * _path, QObject * parent = 0);
    ~XWFileName() {}
    
    //�ļ�����������c:/latex/dvipdfmx.pdf�Ļ�����Ϊdvipdfmx
    QString baseName();
    
    //�ļ�����·�����֣������ļ�������c:/latex/dvipdfmx��·������Ϊc:/latex��
    //http://www.xiuwen/product/index��·������Ϊ/product
    QString dirPart();
    
    //����ļ���ִ�У��ò���param���иó��򣬶�������ÿո�ָ�
    bool execute(const QString & param);
    
    //�ļ���չ����hasdotָ���ڷ��ؽ�����Ƿ����'.'
    QString ext(bool hasdot = false);
    //���ļ�������չ��������Ѿ�����չ�����򲻼�
    QString extendFilename(const QString & defaultSuffix);
    
    //���һ��·���ָ�����Ĳ���
    QString fileName();
    QString fullName() {return path;}
    
    //�ļ���·�����֣����ļ���
    QString getPath();
    
    //�ļ�����������
    QString host();
        
    bool ignoreDir();
    bool isAbsolute();
    bool isAbsolute(bool relative);
    bool isExecutable();
    //·���Ƿ����豸����ʼ
    bool isNameBeginWithDev();
    //�Ƿ�Ϊ�����ļ�(���豸����'\\'��file://��ʼ���ļ�·������Ϊ�Ǳ��ص�)
    bool isLocal();
    bool isReadable();
    //�Ƿ�Ϊ����������·���ָ�����ʼ
    bool isUNCName();
    bool isNull() {return path.isEmpty();}
    bool isValid();
    
    //ȥ���ļ���չ��������'.'(����еĻ�)
    QString noExt();
    
    //��ϵͳ�й���������ļ�
    bool open();
    
    QString password();
    int     port();
        
    //��fn�Ƿ�Ϊͬһ�ļ�
    bool    sameFile(const QString & fn);
    QString scheme();
    void    setPath(const QString & _path);
    
    //�ļ���չ���Ƿ�Ϊsuffix
    bool trySuffix(const QString & suffix);
    
    QString userName();
    
private:
    void normalize();
    
private:
    QString path;
};


#endif // XWFILENAME_H
