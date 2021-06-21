/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ** 这是秀文小书店管理程序的一部分。本软件为商业软件，未经秀文公司许可，任何
 ** 单位和个人不得复制、传播源代码以及二进制文件。
 **
 ****************************************************************************/
#ifndef XWCODEC_H
#define XWCODEC_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTextCodec>
#include <QByteArray>

#define CODEC_LOCAL      -1
#define CODEC_UNICODE     0
#define CODEC_APPLEROMAN  1
#define CODEC_BIG5        2
#define CODEC_BIG5HKSCS   3
#define CODEC_EUCJP       4
#define CODEC_EUCKR       5
#define CODEC_GB180300    6
#define CODEC_IBM850      7
#define CODEC_IBM866      8
#define CODEC_IBM874      9
#define CODEC_ISO2022JP   10
#define CODEC_ISO88591    11
#define CODEC_ISO88592    12
#define CODEC_ISO88593    13
#define CODEC_ISO88594    14
#define CODEC_ISO88595    15
#define CODEC_ISO88596    16
#define CODEC_ISO88597    17
#define CODEC_ISO88598    18
#define CODEC_ISO88599    19
#define CODEC_ISO885910   20
#define CODEC_ISO885913   21
#define CODEC_ISO885914   22
#define CODEC_ISO885915   23
#define CODEC_ISO885916   24
#define CODEC_ISCIIBNG    25
#define CODEC_ISCIIDEV    26
#define CODEC_ISCIIGJR    27
#define CODEC_ISCIIKND    28
#define CODEC_ISCIIMLM    29
#define CODEC_ISCIIORI    30
#define CODEC_ISCIIPNJ    31
#define CODEC_ISCIITLG    32
#define CODEC_ISCIITML    33
#define CODEC_JISX0201    34
#define CODEC_JISX0208    35
#define CODEC_KOI8R       36
#define CODEC_KOI8U       37
#define CODEC_MULELAO1    38
#define CODEC_ROMAN8      39
#define CODEC_SHIFTJIS    40
#define CODEC_TIS620      41
#define CODEC_TSCII       42
#define CODEC_UTF8        43
#define CODEC_UTF16       44
#define CODEC_UTF16BE     45
#define CODEC_UTF16LE     46
#define CODEC_WINDOWS1250 47
#define CODEC_WINDOWS1251 48
#define CODEC_WINDOWS1252 49
#define CODEC_WINDOWS1253 50
#define CODEC_WINDOWS1254 51
#define CODEC_WINDOWS1255 52
#define CODEC_WINDOWS1256 53
#define CODEC_WINDOWS1257 54
#define CODEC_WINDOWS1258 55
#define CODEC_WINSAMI2    56
#define CODEC_LAST        57

#include "XWGlobal.h"
#include "XWType.h"

class XW_UTIL_EXPORT XWCodec : public QObject
{
    Q_OBJECT
    
public:
           XWCodec(int _codec, QObject * parent = 0);
           XWCodec(QObject * parent = 0);
           ~XWCodec() {}
           
    static QStringList allCodec();
        
           bool    canUnicode() {return (codecType != CODEC_LOCAL); }
           int     codec() {return codecType;}
           QString codecName();
    
    static XWCharCode getChar(const QString & s, int &i);
    static QString    getQString(XWCharCode u);
    
           void setCodec(int _codec);
    
           XWCharCode toLocal(XWCharCode u);
           XWCharCode toUnicode(XWCharCode fc);
    
           QTextCodec * getCodec();
    
private:
    int codecType;
};

#endif // XWCODEC_H
