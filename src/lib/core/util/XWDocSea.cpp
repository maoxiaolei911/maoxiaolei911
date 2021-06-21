/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QMutex>
#include <QFileInfo>
#include "XWUtil.h"
#include "XWDocSea.h"

static QMutex docSeaMutex;

static PathSeaParams docSeaParams[] =
{
    {"dvi", 0, 0, "dvi", "DVIDIR", ".dvi", ".dvx;.odvx;.odvi", true, true},
    {"pdf", 0, 0, "pdf", "PDFDIR", ".pdf", 0, true, true},
    {"tex", 0, 0, "tex", "TEXDIR", ".tex", ".log;.toc;.lof;.lot;.aux;.idx;.ind;.ilg;.log", false, false},
    {"html", 0, 0, "html", "HTMLDIR", ".html", ".htm;.mht", true, false},
    {"xml", 0, 0, "xml", "XMLDIR", ".xml", 0, true, false},
    {"doc", 0, 0, "doc", "DOCDIR", ".doc", 0, true, true},
    {"dot", 0, 0, "dot", "DOTDIR", ".dot", 0, true, true},
    {"ppt", 0, 0, "ppt", "PPTDIR", ".ppt", 0, true, true},
    {"rtf", 0, 0, "rtf", "RTFDIR", ".rtf", 0, true, false},
    {"xls", 0, 0, "xls", "XLSDIR", ".xls", 0, true, true},
    {"txt", 0, 0, "txt", "TXTDIR", ".txt", 0, false, false},
    {"ps", 0, 0, "ps", "PSDIR", ".ps", 0, true, false}
};

XWDocSeaPrivate::XWDocSeaPrivate(QObject * parent)
    :XWPathSea("doc", (int)(XWDocSea::Last), "doc.cnf", "doc.lsr", parent)
{
}

QString XWDocSeaPrivate::initFormat(int format)
{
    FormatInfo * f = &formatInfo[format];
    if (!(f->path.isEmpty()))
        return f->path;

    initFormatByParams(format, &docSeaParams[format]);

    return f->path;
}

static XWDocSeaPrivate * docSea = 0;

XWDocSea::XWDocSea(QObject * parent)
    :QObject(parent)
{
	if (!docSea)
		docSea = new XWDocSeaPrivate;
}

void XWDocSea::addConfig(const QString & key,
                          const QString & value,
                          const QString & prog)
{
    docSea->addConfig(key, value, prog);
}

void XWDocSea::addFile(const QString & path)
{
	docSea->addFile(path);
}

QStringList XWDocSea::allDvi()
{
    return allFileNames(DVI);
}

QStringList XWDocSea::allFileNames(XWDocSea::FileFormat format)
{
	return docSea->allFileNames(&docSeaParams[format]);
}

QStringList XWDocSea::allHtml()
{
    return allFileNames(HTML);
}

QStringList XWDocSea::allPdf()
{
    return allFileNames(PDF);
}

QStringList XWDocSea::allTex()
{
    return allFileNames(TEX);
}

void XWDocSea::createAllDefaultDir(const QString & topdir)
{
	int format = DVI;
	while (format < Last)
	{
		createDefaultDir((FileFormat)format, topdir);
		format++;
	}
}

void XWDocSea::createDefaultDir(XWDocSea::FileFormat format,
	                           const QString & topdir)
{
	docSea->createDefaultDir(&docSeaParams[format], topdir);
}

QStringList XWDocSea::findAll(const QString & name,
	                          XWDocSea::FileFormat format)
{
	QStringList ret = docSea->findAll(format, name);
	return ret;
}

QStringList XWDocSea::findAll(const QStringList & names,
	                          XWDocSea::FileFormat format)
{
	QStringList ret = docSea->findAll(format, names);
	return ret;
}
	                    
QString XWDocSea::findDvi(const QString & name)
{
    return findFile(name, DVI, false);
}

QString XWDocSea::findFile(const QString & name)
{
	if (name.isEmpty())
	{
        return QString();
      }
        
    QString ret = docSea->findFile(name);
    return ret;
}

QString XWDocSea::findFile(const QString & name,
                           XWDocSea::FileFormat format,
                           bool must_exist)
{
    if (name.isEmpty())
    {
        return QString();
      }
        
    QString ret = docSea->findFile(name, format, must_exist);
    return ret;
}

XWDocSea::FileFormat XWDocSea::findFormat(const QString & name)
{
	int tmpfmt = docSea->findFormat(name);
    if (tmpfmt == -1)
    {
        return Last;
      }

    XWDocSea::FileFormat format = (XWDocSea::FileFormat)tmpfmt;
    return format;
}

QString XWDocSea::findPdf(const QString & name)
{
    return findFile(name, PDF, false);
}

QString XWDocSea::findTex(const QString & name)
{
    return findFile(name, TEX, false);
}

QString XWDocSea::findHtml(const QString & name)
{
    return findFile(name, HTML, false);
}

QStringList XWDocSea::getConfigDir(XWDocSea::FileFormat format)
{
	return docSea->getConfigDir(&docSeaParams[format]);
}

QStringList XWDocSea::getSuffixes(XWDocSea::FileFormat format)
{
	return docSea->getSuffixes(&docSeaParams[format]);
}

QStringList XWDocSea::getTopDirs()
{
	return docSea->getTopDirs();
}

void XWDocSea::init()
{
	if (!docSea)
		docSea = new XWDocSeaPrivate;
}

void XWDocSea::insertFile(const QString & passedfname)
{
	docSea->insertFile(passedfname);
}

QFile * XWDocSea::openDvi(const QString & filename)
{
    return openFile(filename, DVI);
}

QFile * XWDocSea::openFile(const QString & filename)
{
    QFile * ret = docSea->openFile(filename);
    return ret;
}

QFile * XWDocSea::openFile(const QString & filename,
                           XWDocSea::FileFormat format)
{
  QFile * ret = docSea->openFile((int)format, filename);
    return ret;
}

QFile * XWDocSea::openPdf(const QString & filename)
{
    return openFile(filename, PDF);
}

QFile * XWDocSea::openTex(const QString & filename)
{
    return openFile(filename, TEX);
}

QFile * XWDocSea::openHtml(const QString & filename)
{
    return openFile(filename, HTML);
}

void XWDocSea::quit()
{
}

void XWDocSea::removeConfig(const QString & key)
{
	docSea->removeConfig(key);
}

void XWDocSea::setCurDocPath(const QString & filename)
{
	if (filename.isEmpty())
		return ;
		
	QFileInfo fileinfo(filename);
	QString curpath = fileinfo.path();
	putEnv("CUR_DOC_PATH", curpath);
}

void XWDocSea::setDirs(XWDocSea::FileFormat format, 
                       const QString & dirs)
{
	PathSeaParams * params = &docSeaParams[format];
	QString tmp = QString::fromAscii(params->envs);
	QStringList envs = tmp.split(";", QString::SkipEmptyParts);
	if (envs.isEmpty())
		return ;
		
	tmp = envs[0];
	
	docSea->setDirs(tmp, dirs);
}

QString XWDocSea::varExpand(const QString & s)
{
    return docSea->varExpand(s);
}

QString XWDocSea::varValue(const QString & s)
{
    return docSea->varValue(s);
}

void XWDocSea::save()
{
	docSea->save();
}

void XWDocSea::updateDBs()
{
	docSea->updateDBs();
}

