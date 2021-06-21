/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QMutex>
#include <QFileInfo>
#include "XWHelpSea.h"

static QMutex helpSeaMutex;

static PathSeaParams helpSeaParams[] =
{
    {"dvi", 0, 0, "dvi", "DVIDIR", ".dvi", ".odvi", true, true},
    {"pdf", 0, 0, "pdf", "PDFDIR", ".pdf", 0, true, true}
};

XWHelpSeaPrivate::XWHelpSeaPrivate(QObject * parent)
    :XWPathSea("help", (int)(XWHelpSea::Last), "help.cnf", "help.lsr", parent)
{
}

QString XWHelpSeaPrivate::initFormat(int format)
{
    FormatInfo * f = &formatInfo[format];
    if (!(f->path.isEmpty()))
        return f->path;

    initFormatByParams(format, &helpSeaParams[format]);

    return f->path;
}

static XWHelpSeaPrivate * helpSea = 0;

XWHelpSea::XWHelpSea(QObject * parent)
    :QObject(parent)
{
	if (!helpSea)
		helpSea = new XWHelpSeaPrivate;
}

void XWHelpSea::addConfig(const QString & key,
                          const QString & value,
                          const QString & prog)
{
    helpSea->addConfig(key, value, prog);
}

void XWHelpSea::addFile(const QString & path)
{
	helpSea->addFile(path);
}

QStringList XWHelpSea::allDvi()
{
    return helpSea->allFileNames(&helpSeaParams[DVI]);
}

QStringList XWHelpSea::allPdf()
{
    return helpSea->allFileNames(&helpSeaParams[PDF]);
}

void XWHelpSea::createAllDefaultDir(const QString & topdir)
{
	int format = DVI;
	while (format < Last)
	{
		createDefaultDir((FileFormat)format, topdir);
		format++;
	}
}

void XWHelpSea::createDefaultDir(XWHelpSea::FileFormat format,
	                             const QString & topdir)
{
	helpSea->createDefaultDir(&helpSeaParams[format], topdir);
}

QStringList XWHelpSea::findAll(const QString & name,
	                           XWHelpSea::FileFormat format)
{
	QStringList ret = helpSea->findAll(format, name);
	return ret;
}

QStringList XWHelpSea::findAll(const QStringList & names,
	                           XWHelpSea::FileFormat format)
{
	QStringList ret = helpSea->findAll(format, names);
	return ret;
}
	                    
QString XWHelpSea::findDvi(const QString & name)
{
    return findFile(name, DVI, false);
}

QString XWHelpSea::findFile(const QString & name,
                            XWHelpSea::FileFormat format,
                            bool must_exist)
{
    if (name.isEmpty())
    {
        return QString();
      }

    QString ret = helpSea->findFile(name, format, must_exist);
	return ret;
}

XWHelpSea::FileFormat XWHelpSea::findFormat(const QString & name)
{
	int tmpfmt = helpSea->findFormat(name);
    if (tmpfmt == -1)
    {
        return Last;
      }

    XWHelpSea::FileFormat format = (XWHelpSea::FileFormat)tmpfmt;
    return format;
}

QString XWHelpSea::findPdf(const QString & name)
{
    return findFile(name, PDF, false);
}

QStringList XWHelpSea::getConfigDir(XWHelpSea::FileFormat format)
{
	return helpSea->getConfigDir(&helpSeaParams[format]);
}

QStringList XWHelpSea::getSuffixes(XWHelpSea::FileFormat format)
{
	return helpSea->getSuffixes(&helpSeaParams[format]);
}

QStringList XWHelpSea::getTopDirs()
{
	return helpSea->getTopDirs();
}

void XWHelpSea::init()
{
	if (!helpSea)
		helpSea = new XWHelpSeaPrivate;
}

void XWHelpSea::insertFile(const QString & passedfname)
{
	helpSea->insertFile(passedfname);
}

QFile * XWHelpSea::openDvi(const QString & filename)
{
    return openFile(filename, DVI);
}

QFile * XWHelpSea::openFile(const QString & filename)
{
    QFile * ret = helpSea->openFile(filename);
    return ret;
}

QFile * XWHelpSea::openFile(const QString & filename,
                           XWHelpSea::FileFormat format)
{
    QFile * ret = helpSea->openFile((int)format, filename);
    return ret;
}

QFile * XWHelpSea::openPdf(const QString & filename)
{
    return openFile(filename, PDF);
}

void XWHelpSea::quit()
{
}

void XWHelpSea::removeConfig(const QString & key)
{
	helpSea->removeConfig(key);
}

void XWHelpSea::setDirs(XWHelpSea::FileFormat format, 
                       const QString & dirs)
{
	PathSeaParams * params = &helpSeaParams[format];
	QString tmp = QString::fromAscii(params->envs);
	QStringList envs = tmp.split(";", QString::SkipEmptyParts);
	if (envs.isEmpty())
		return ;
		
	tmp = envs[0];
	
	helpSea->setDirs(tmp, dirs);
}

QString XWHelpSea::varExpand(const QString & s)
{
    return helpSea->varExpand(s);
}

QString XWHelpSea::varValue(const QString & s)
{
    return helpSea->varValue(s);
}

void XWHelpSea::save()
{
	helpSea->save();
}

void XWHelpSea::updateDBs()
{
	helpSea->updateDBs();
}
