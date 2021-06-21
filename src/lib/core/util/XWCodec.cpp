/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ** 这是秀文小书店管理程序的一部分。本软件为商业软件，未经秀文公司许可，任何
 ** 单位和个人不得复制、传播源代码以及二进制文件。
 **
 ****************************************************************************/
#include "XWStringUtil.h"
#include "XWCodec.h"

XWCodec::XWCodec(int _codec, QObject * parent)
    :QObject(parent)
{
    setCodec(_codec);
}

XWCodec::XWCodec(QObject * parent)
    :QObject(parent)
{
    codecType = CODEC_LOCAL;
}

QStringList XWCodec::allCodec()
{
    QStringList ret;
    for (int i = CODEC_UNICODE; i < CODEC_LAST; i++)
    {
        XWCodec codec(i);
        QString s = QString("%1(%2)").arg(codec.codecName()).arg(i);
        
        ret << s;
    }
    
    return ret;
}

QString XWCodec::codecName()
{
    QString ret;
    switch (codecType)
    {
        case CODEC_APPLEROMAN:
            ret = "Apple Roman";
            break;
            
        case CODEC_BIG5:
            ret = "Big5";
            break;
            
        case CODEC_BIG5HKSCS:
            ret = "Big5-HKSCS";
            break;
            
        case CODEC_EUCJP:
            ret = "EUC-JP";
            break;
            
        case CODEC_EUCKR:
            ret = "EUC-KR";
            break;
            
        case CODEC_GB180300:
            ret = "GB18030-0";
            break;
            
        case CODEC_IBM850:
            ret = "IBM 850";
            break;
            
        case CODEC_IBM866:
            ret = "IBM 866";
            break;
            
        case CODEC_IBM874:
            ret = "IBM 874";
            break;
            
        case CODEC_ISO2022JP:
            ret = "ISO 2022-JP";
            break;
            
        case CODEC_ISO88591:
            ret = "ISO 8859-1";
            break;
            
        case CODEC_ISO88592:
            ret = "ISO 8859-2";
            break;
            
        case CODEC_ISO88593:
            ret = "ISO 8859-3";
            break;
            
        case CODEC_ISO88594:
            ret = "ISO 8859-4";
            break;
            
        case CODEC_ISO88595:
            ret = "ISO 8859-5";
            break;
            
        case CODEC_ISO88596:
            ret = "ISO 8859-6";
            break;
            
        case CODEC_ISO88597:
            ret = "ISO 8859-7";
            break;
            
        case CODEC_ISO88598:
            ret = "ISO 8859-8";
            break;
            
        case CODEC_ISO88599:
            ret = "ISO 8859-9";
            break;
            
        case CODEC_ISO885910:
            ret = "ISO 8859-10";
            break;
            
        case CODEC_ISO885913:
            ret = "ISO 8859-13";
            break;
            
        case CODEC_ISO885914:
            ret = "ISO 8859-14";
            break;
            
        case CODEC_ISO885915:
            ret = "ISO 8859-15";
            break;
            
        case CODEC_ISO885916:
            ret = "ISO 8859-16";
            break;
            
        case CODEC_ISCIIBNG:
            ret = "Iscii-Bng";
            break;
            
        case CODEC_ISCIIDEV:
            ret = "Iscii-Dev";
            break;
            
        case CODEC_ISCIIGJR:
            ret = "Iscii-Gjr";
            break;
            
        case CODEC_ISCIIKND:
            ret = "Iscii-Knd";
            break;
            
        case CODEC_ISCIIMLM:
            ret = "Iscii-Mlm";
            break;
            
        case CODEC_ISCIIORI:
            ret = "Iscii-Ori";
            break;
            
        case CODEC_ISCIIPNJ:
            ret = "Iscii-Pnj";
            break;
            
        case CODEC_ISCIITLG:
            ret = "Iscii-Tlg";
            break;
            
        case CODEC_ISCIITML:
            ret = "Iscii-Tml";
            break;
            
        case CODEC_JISX0201:
            ret = "JIS X 0201";
            break;
            
        case CODEC_JISX0208:
            ret = "JIS X 0208";
            break;
            
        case CODEC_KOI8R:
            ret = "KOI8-R";
            break;
            
        case CODEC_KOI8U:
            ret = "KOI8-U";
            break;
            
        case CODEC_MULELAO1:
            ret = "MuleLao-1";
            break;
            
        case CODEC_ROMAN8:
            ret = "ROMAN8";
            break;
            
        case CODEC_SHIFTJIS:
            ret = "Shift-JIS";
            break;
            
        case CODEC_TIS620:
            ret = "TIS-620";
            break;
            
        case CODEC_TSCII:
            ret = "TSCII";
            break;
            
        case CODEC_UTF8:
            ret = "UTF-8";
            break;
            
        case CODEC_UTF16:
            ret = "UTF-16";
            break;
            
        case CODEC_UTF16BE:
            ret = "UTF-16BE";
            break;
            
        case CODEC_UTF16LE:
            ret = "UTF-16LE";
            break;
            
        case CODEC_WINDOWS1250:
            ret = "Windows-1250";
            break;
            
        case CODEC_WINDOWS1251:
            ret = "Windows-1251";
            break;
            
        case CODEC_WINDOWS1252:
            ret = "Windows-1252";
            break;
            
        case CODEC_WINDOWS1253:
            ret = "Windows-1253";
            break;
            
        case CODEC_WINDOWS1254:
            ret = "Windows-1254";
            break;
            
        case CODEC_WINDOWS1255:
            ret = "Windows-1255";
            break;
            
        case CODEC_WINDOWS1256:
            ret = "Windows-1256";
            break;
            
        case CODEC_WINDOWS1257:
            ret = "Windows-1257";
            break;
            
        case CODEC_WINDOWS1258:
            ret = "Windows-1258";
            break;
            
        case CODEC_WINSAMI2:
            ret = "WINSAMI2";
            break;
            
        default:
            break;
    }
    
    return ret;
}


QTextCodec * XWCodec::getCodec()
{
    QTextCodec * ret = 0;
    QString n = codecName();
    if (n.isEmpty())
        return ret;
        
    QByteArray name = n.toAscii();
    ret = QTextCodec::codecForName(name);
    
    return ret;
}

XWCharCode XWCodec::getChar(const QString & s, int &i)
{
    int len = s.length();
    XWCharCode uc = s[i].unicode();
    if ((uc >= 0xd800) && (uc < 0xdc00) && i < (len - 1))
    {
        uint low = s[i+1].unicode();
        if ((low >= 0xdc00) && (low < 0xe000))
        {
            uc = (uc - 0xd800)*0x400 + (low - 0xdc00) + 0x10000;
            ++i;
        }
    }
    
    i++;
    
    return uc;
}

QString XWCodec::getQString(XWCharCode u)
{
    QString ret;
    if (u < 0x10000)
    {
        QChar c((ushort)u);
        ret += c;
    }
    else
    {
        u -= 0x10000;
        ushort high = u / 0x400 + 0xd800;
        ushort low = u % 0x400 + 0xdc00;
                    
        QChar h(high);
        QChar l(low);
        
        ret += h;
        ret += l;
    }
    
    return ret;
}

void XWCodec::setCodec(int _codec)
{
    if ((_codec <= CODEC_LOCAL) || (_codec > CODEC_WINSAMI2))
        codecType = CODEC_LOCAL;
    else
        codecType = _codec;
}

XWCharCode XWCodec::toLocal(XWCharCode u)
{
    XWCharCode ret = u;
    QTextCodec * codec = getCodec();
    if (codec)
    {
        QString s = getQString(u);
        QByteArray ba = codec->fromUnicode(s);
        ret = 0;
        int len = ba.size();
        for (int i = 0; i < len; i++)
            ret = (ret << 8) + (uchar)ba.at(i);
    }
    
    return ret;
}

XWCharCode XWCodec::toUnicode(XWCharCode fc)
{
    XWCharCode ret = fc;
    QTextCodec * codec = getCodec();
    
    if (codec)
    {
        QByteArray ba(7, 0);
        if (fc < 0x100)
            ba[0] = (fc & 0x00FF);
        else if (fc < 0x10000)
        {
            ba[0] = (fc & 0xFF00) >> 8;
            ba[1] = (fc & 0x00FF);
        }
        else if (fc < 0x1000000)
        {
            ba[0] = (fc & 0xFF0000) >> 16;
            ba[1] = (fc & 0x00FF00) >> 8;
            ba[2] = (fc & 0x0000FF);
        }
        else if (fc < 0x100000000)
        {
            ba[0] = (fc & 0xFF000000) >> 24;
            ba[1] = (fc & 0x00FF0000) >> 16;
            ba[2] = (fc & 0x0000FF00) >> 8;
            ba[3] = (fc & 0x000000FF);
        }
        else if (fc < 10000000000)
        {
            ba[0] = (fc & 0xFF00000000) >> 32;
            ba[1] = (fc & 0x00FF000000) >> 24;
            ba[2] = (fc & 0x0000FF0000) >> 16;
            ba[3] = (fc & 0x000000FF00) >> 8;
            ba[4] = (fc & 0x00000000FF);
        }
        else if (fc < 0x1000000000000)
        {
            ba[0] = (fc & 0xFF0000000000) >> 40;
            ba[1] = (fc & 0x00FF00000000) >> 32;
            ba[2] = (fc & 0x0000FF000000) >> 24;
            ba[3] = (fc & 0x000000FF0000) >> 16;
            ba[4] = (fc & 0x00000000FF00) >> 8;
            ba[5] = (fc & 0x0000000000FF);
        }
            
        QString s = codec->toUnicode(ba);
        int i = 0;
        ret = getChar(s, i);
    }
        
    return ret;
}
