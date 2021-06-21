/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTEXTFINDER_H
#define XWTEXTFINDER_H

#include <QThread>
#include <QList>
#include <QRegExp>

#include "XWTeXGuiType.h"

class QMdiArea;
class XWTeXTextEdit;

class XW_TEXGUI_EXPORT XWTextFinder : public QThread
{
    Q_OBJECT
	
public:
	XWTextFinder(QObject * parent = 0);

    void find(QMdiArea * area, const QString & pathA,
              const QString & str, bool casesensitivity, 
              bool wholeword, bool regexpA);

    void replace(QMdiArea * area, const QString & pathA,
                 const QString & str, const QString & bystr, 
                 bool casesensitivity, bool wholeword, bool regexpA);

    XWTeXTextEdit * takeNewEditor();

protected:
    void run();

private:
    XWTeXTextEdit * findMdiChild(const QString & fileName);
    void findOrReplaceInPatn(const QString & pathA);
    void findOrReplaceInFile(const QString & fileName);

private:
    QMdiArea * mdiArea;
    QString path;
    QString findString;
	QString replaceString;
	QRegExp regexp;
    bool isReplace;
    bool caseSensitivity;
    bool wholeWord;
    bool isRegexpFind;
    QList<XWTeXTextEdit*> edits;
};

#endif // XWTEXTFINDER_H
