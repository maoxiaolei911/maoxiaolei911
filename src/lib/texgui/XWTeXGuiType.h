/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWTEXGUITYPE_H
#define XWTEXGUITYPE_H

#include <QtGlobal>
#include <QObject>
#include <QHash>
#include <QList>
#include <QString>

#if defined(XW_BUILD_TEXGUI_LIB)
#define XW_TEXGUI_EXPORT Q_DECL_EXPORT
#else
#define XW_TEXGUI_EXPORT Q_DECL_IMPORT
#endif

class XW_TEXGUI_EXPORT XWTeXHash : public QObject
{
	Q_OBJECT
	
public: 
	XWTeXHash(QObject * parent = 0);
	XWTeXHash(qint32 minstr, QObject * parent = 0);
	~XWTeXHash();
	
	qint32  addString(qint32 * buf, qint32 l);
	qint32  addString(const QString & str);
	
	QString getString(qint32 s);
	
	bool   loadFile(int   maxstrings, 
	                int   poolsize,
	                const QString & filename);
	qint32 lookupID(qint32 * buf, qint32 l);
	qint32 lookupID(const QString & str);

private:
	void clear();
	
	qint32 length(qint32 s);
			
private:
	struct Bucket
	{
		qint32   len;
		qint32 * str;
	};

	Bucket * tab;
	qint32 * pool;
	qint32 * start;
	
	int   minString;
	int   maxStrings;
	int   poolSize;
	
	int   strPtr;
	int   poolPtr;
};

inline qint32 XWTeXHash::length(qint32 s) 
{
	return (start[s + 1] - start[s]); 
}

QString XW_TEXGUI_EXPORT breakToLines(int len, const QString & str);

#endif //XWTEXGUITYPE_H
