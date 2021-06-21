/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWSTRINGUTIL_H
#define XWSTRINGUTIL_H

#include <QString>
#include <QStringList>
#include <QByteArray>

#include "XWGlobal.h"
#include "XWType.h"


#ifndef isodigit
#define isodigit(c) ((c) >= '0' && (c) <= '7')
#endif

inline bool operator == (QChar ch, char c)
{
    QChar a(c);
    
    return (a == ch);
}

inline bool operator == (char c, QChar ch)
{
    QChar a(c);
    
    return (a == ch);
}

inline bool operator == (QChar ch, int c)
{
    QChar a(c);
    
    return (a == ch);
}

inline bool operator == (int c, QChar ch)
{
    QChar a(c);
    
    return (a == ch);
}

inline bool operator == (QChar ch, ushort c)
{
    QChar a(c);
    
    return (a == ch);
}

inline bool operator == (ushort c, QChar ch)
{
    QChar a(c);
    
    return (a == ch);
}

inline bool operator != (QChar ch, char c)
{
    QChar a(c);
    
    return (a != ch);
}

inline bool operator != (char c, QChar ch)
{
    QChar a(c);
    
    return (a != ch);
}

inline bool operator != (QChar ch, int c)
{
    QChar a(c);
    
    return (a != ch);
}

inline bool operator != (int c, QChar ch)
{
    QChar a(c);
    
    return (a != ch);
}

inline bool operator != (QChar ch, ushort c)
{
    QChar a(c);
    
    return (a != ch);
}

inline bool operator != (ushort c, QChar ch)
{
    QChar a(c);
    
    return (a != ch);
}

inline bool operator <= (QChar ch, ushort c)
{
    QChar a(c);
    
    return (a <= ch);
}

inline bool operator <= (ushort c, QChar ch)
{
    QChar a(c);
    
    return (a <= ch);
}


inline bool atStringEnd(const QChar * p)
{
    return (!p || p[0].isNull());
}


inline QChar envSep()
{
#ifdef Q_OS_WIN
    return QChar(';');
#else
    return QChar(':');
#endif
}

inline void downCasify(char * s)
{
    if (s)
    {
        char  *p = s;
        while (*p != 0)
        {
            if (*p >= 'A' && *p <= 'Z')
            {
                *p = *p - 'A' + 'a';
            }
            
            p++;
        }
    }
}

inline bool isBlank(char c) 
{
	return (c == ' '  || 
	        c == '\t' || 
	        c == '\v');
}

inline bool isBlank(QChar ch) 
{
	return isBlank(ch.toAscii());
}


inline bool isDevSep(const QChar & c)
{
#ifdef Q_OS_WIN
    return (c == QChar(':'));
#else
    return false;
#endif
}

inline bool isDigit(XWASCIICode c)
{
    QChar ch(c);
    return ch.isDigit();
}


inline bool isDirSep(int c)
{
    QChar a(c);
    
    return ((a == QChar('/')) || (a == QChar('\\')));
}

inline bool isDirSep(const QChar & c)
{
    return (c == QChar('/') || c == QChar('\\'));
}

inline bool isEnvSep(const QChar & c)
{
#ifdef Q_OS_WIN
    return (c == QChar(';'));
#else
    return (c == QChar(':'));
#endif
}

inline bool isEqual(int unicode, char c)
{
    QChar u(unicode);
    QChar a(c);
    
    return (u == c);
}

inline bool isHex(char c)
{    
    return (((c >= '0') && (c <= '9')) || 
            ((c >= 'a') && (c <= 'f')) || 
            ((c >= 'A') && (c <= 'F')));
}

inline bool isHex(QChar ch)
{
    return isHex(ch.toAscii());
}

inline bool isHex(XWASCIICode ch)
{
    QChar cha(ch);
    char c = cha.toAscii();
    return isHex(c);
}

inline bool isOctal(QChar ch)
{
    char c = ch.toAscii();
    return ((c >= '0') && (c <= '7'));
}

inline bool isOctal(XWASCIICode ch)
{
    return ((ch >= '0') && (ch <= '7'));
}

inline bool isSpace(char c) 
{
	return (c == ' '  || 
	        c == '\t' || 
	        c == '\v' || 
	        c == '\n' || 
	        c == '\r' || 
	        c == '\f' || 
	        c == '\0');
}

inline bool isSpace(QChar ch) 
{
	return isSpace(ch.toAscii());
}

inline bool isVarBeginDelimiter(const QChar & c)
{
    return (c == QChar('{'));
}

inline bool isVarEndDelimiter(const QChar & c)
{
    return (c == QChar('}'));
}

inline bool isVarChar(const QChar & c)
{
    return (!isDevSep(c) && 
            !isDirSep(c) && 
            !isEnvSep(c) && 
            c != QChar('$') && 
            c != QChar('.') && 
            !isVarBeginDelimiter(c) && 
            !isVarEndDelimiter(c));
}

inline bool isVarStart(const QChar & c)
{
    return (c == QChar('$'));
}

int XW_UTIL_EXPORT sgetUnsignedPair(unsigned char *s);

int XW_UTIL_EXPORT xtoi(char ch);

char   XW_UTIL_EXPORT signedByte(uchar **start, uchar *end);
short  XW_UTIL_EXPORT signedPair(uchar **start, uchar *end);
long   XW_UTIL_EXPORT signedQuad(uchar **start, uchar *end);
long   XW_UTIL_EXPORT signedTriple(uchar **start, uchar *end);
uchar  XW_UTIL_EXPORT unsignedByte(uchar **start, uchar *end);
ushort XW_UTIL_EXPORT unsignedPair(uchar **start, uchar *end);
ulong  XW_UTIL_EXPORT unsignedQuad(uchar **start, uchar *end);
long   XW_UTIL_EXPORT unsignedTriple(uchar **start, uchar *end);

#endif // XWSTRINGUTIL_H
