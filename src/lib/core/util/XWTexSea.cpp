/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QMutex>
#include <QCoreApplication>
#include <QDir> 
#include <QTextStream> 
#include "XWUtil.h" 
#include "XWStringUtil.h" 
#include "XWApplication.h"
#include "XWPathSea.h" 
#include "XWFileName.h"
#include "XWOptionParser.h" 
#include "XWDocSea.h" 
#include "XWTexSea.h" 

static QMutex texSeaMutex;

static PathSeaParams texSeaParams[] = 
{
	{"tex", "mktextex", 0, 0, "TEXINPUTS", ".tex", ".cls;.clo;.sty;.ini;.ltx", false, false},
	{"ocp", "mkocp", 0, "ocp", "OCPINPUTS", ".ocp", 0, true, true},
	{"otp", 0, 0, "opt", "OTPINPUTS", ".otp", 0, true, false}
};

XWTexSeaPrivate::XWTexSeaPrivate(QObject * parent)
	:XWPathSea("tex", (int)(XWTexSea::Last), "texmf.cnf", "tex.lsr", parent)
{
}

QString XWTexSeaPrivate::findFmtFile(const QString & name, const QString & fmt)
{
	QString ret;
	FormatInfo * f = &formatInfo[XWTexSea::TEX];
	if (fmt != curFmt)
	{
		curFmt = fmt;
		QString var = QString("TEXINPUTS.%1").arg(curFmt);
		QString tmp = varValue(var);
		if (tmp.isEmpty())
			return ret;
			
		f->path = getAbsoluteDir(configDir, tmp);
		f->path = braceExpand(f->path);
		
		PathSeaParams * params = &texSeaParams[XWTexSea::TEX];
		tmp = QString::fromAscii(params->suffixes);
		f->suffix = tmp.split(";", QString::SkipEmptyParts);
		tmp = QString::fromAscii(params->altsuffixes);
		f->altSuffix = tmp.split(";", QString::SkipEmptyParts);
		f->suffixSearchOnly = params->suffixonly;
    f->binMode = params->binmode;
	}
	
	ret = findFile(name, XWTexSea::TEX, false);
	return ret;
}

QString XWTexSeaPrivate::initFormat(int format)
{
	FormatInfo * f = &formatInfo[format];
	if (!(f->path.isEmpty()))
		return f->path;
		
	initFormatByParams(format, &texSeaParams[format]);
	
	return f->path;
}

static XWTexSeaPrivate * texSea = 0;

XWTexSea::XWTexSea(QObject * parent)
	:QObject(parent)
{
	if (!texSea)
		texSea = new XWTexSeaPrivate;
}

void XWTexSea::addConfig(const QString & key,
                          const QString & value,
                          const QString & prog)
{
    texSea->addConfig(key, value, prog);
}

void XWTexSea::addFile(const QString & path)
{
	texSea->addFile(path);
}

void XWTexSea::createAllDefaultDir(const QString & topdir)
{
	int format = TEX;
	while (format < Last)
	{
		createDefaultDir((FileFormat)format, topdir);
		format++;
	}
}

void XWTexSea::createDefaultDir(XWTexSea::FileFormat format,
	                            const QString & topdir)
{
	texSea->createDefaultDir(&texSeaParams[format], topdir);
}

QStringList XWTexSea::findAll(const QString & name)
{
	QStringList ret = texSea->findAll(TEX,name);
	return ret;
}

QString XWTexSea::findFile(const QString & name, const QString & fmt)
{
	QString ret = texSea->findFmtFile(name, fmt);
	return ret;
}

QString XWTexSea::findFile(const QString & name,
                           bool must_exist)
{
	QString ret = texSea->findFile(name, TEX, must_exist);
	return ret;
}

QStringList XWTexSea::getConfigDir(XWTexSea::FileFormat format)
{
	return texSea->getConfigDir(&texSeaParams[format]);
}

QStringList XWTexSea::getTopDirs()
{
	return texSea->getTopDirs();
}

void XWTexSea::init()
{
	if (!texSea)
		texSea = new XWTexSeaPrivate;
}

void XWTexSea::insertFile(const QString & passedfname)
{
	texSea->insertFile(passedfname);
}

QFile * XWTexSea::openFile(const QString & filename)
{
	QFile * ret = 0;
	XWFileName fn(filename);
	if (fn.isAbsolute() && fn.isReadable())
	{
		ret = new QFile(filename);
		ret->open(QIODevice::ReadOnly | QIODevice::Text);
	}
	else
		ret = texSea->openFile(filename);
	return ret;
}

QFile * XWTexSea::openFile(const QString & filename, XWTexSea::FileFormat format)
{
	QFile * ret = 0;
	XWFileName fn(filename);
	if (fn.isAbsolute() && fn.isReadable())
	{
		ret = new QFile(filename);
		ret->open(QIODevice::ReadOnly | QIODevice::Text);
	}
	else
		ret = texSea->openFile((int)format, filename);
	return ret;
}

void XWTexSea::quit()
{
}

void XWTexSea::removeConfig(const QString & key)
{
	texSea->removeConfig(key);
}

void XWTexSea::setDirs(XWTexSea::FileFormat format, 
                       const QString & dirs)
{
	PathSeaParams * params = &texSeaParams[format];
	QString tmp = QString::fromAscii(params->envs);
	QStringList envs = tmp.split(";", QString::SkipEmptyParts);
	if (envs.isEmpty())
		return ;
		
	tmp = envs[0];
	
	texSea->setDirs(tmp, dirs);
}

QString XWTexSea::varExpand(const QString & s)
{
	return texSea->varExpand(s);
}

QString XWTexSea::varValue(const QString & s)
{
	return texSea->varValue(s);
}

void XWTexSea::save()
{
	texSea->save();
}

void XWTexSea::updateDBs()
{
	texSea->updateDBs();
}

static PathSeaParams bibTexSeaParams[] = 
{
	{"bib", 0, 0, "bib", "BIBINPUTS;TEXBIB", ".bib", 0, true, false},
	{"bst", 0, 0, "bst", "BSTINPUTS", ".bst", 0, true, false}
};

XWBibTexSeaPrivate::XWBibTexSeaPrivate(QObject * parent)
	:XWPathSea("bibtex", (int)(XWBibTexSea::Last), "bibtex.cnf", "bibtex.lsr", parent)
{
}

QString XWBibTexSeaPrivate::initFormat(int format)
{
	FormatInfo * f = &formatInfo[format];
	if (!(f->path.isEmpty()))
		return f->path;
		
	initFormatByParams(format, &bibTexSeaParams[format]);
	
	return f->path;
}

static XWBibTexSeaPrivate * bibTexSea = 0;

XWBibTexSea::XWBibTexSea(QObject * parent)
	:QObject(parent)
{
	if (!bibTexSea)
		bibTexSea = new XWBibTexSeaPrivate;
}

void XWBibTexSea::addConfig(const QString & key, 
                            const QString & value, 
                            const QString & prog)
{
	bibTexSea->addConfig(key, value, prog);
}

void XWBibTexSea::addFile(const QString & path)
{
	bibTexSea->addFile(path);
}

QStringList XWBibTexSea::allFileNames(XWBibTexSea::FileFormat format)
{
	return bibTexSea->allFileNames(&bibTexSeaParams[format]);
}

void XWBibTexSea::createAllDefaultDir(const QString & topdir)
{
	int format = BIB;
	while (format < Last)
	{
		createDefaultDir((FileFormat)format, topdir);
		format++;
	}
}

void XWBibTexSea::createDefaultDir(XWBibTexSea::FileFormat format,
	                               const QString & topdir)
{
	bibTexSea->createDefaultDir(&bibTexSeaParams[format], topdir);
}

QStringList XWBibTexSea::findAll(const QString & name,
	                             XWBibTexSea::FileFormat format)
{
	QStringList ret = bibTexSea->findAll(format, name);
	return ret;
}

QStringList XWBibTexSea::findAll(const QStringList & names,
	                             XWBibTexSea::FileFormat format)
{
	QStringList ret = bibTexSea->findAll(format, names);
	return ret;
}

QString XWBibTexSea::findFile(const QString & name,
	                          XWBibTexSea::FileFormat format,
                              bool must_exist)
{
	QString ret = bibTexSea->findFile(name, format, must_exist);
	return ret;
}

XWBibTexSea::FileFormat XWBibTexSea::findFormat(const QString & name)
{
	int tmpfmt = bibTexSea->findFormat(name);
    if (tmpfmt == -1)
    {
        return XWBibTexSea::Last;
        }

    XWBibTexSea::FileFormat format = (XWBibTexSea::FileFormat)tmpfmt;
    return format;
}

QStringList XWBibTexSea::getConfigDir(XWBibTexSea::FileFormat format)
{
	return bibTexSea->getConfigDir(&bibTexSeaParams[format]);
}

QStringList XWBibTexSea::getSuffixes(XWBibTexSea::FileFormat format)
{
	return bibTexSea->getSuffixes(&bibTexSeaParams[format]);
}

QStringList XWBibTexSea::getTopDirs()
{
	return bibTexSea->getTopDirs();
}

void XWBibTexSea::init()
{
	if (!bibTexSea)
		bibTexSea = new XWBibTexSeaPrivate;
}

void XWBibTexSea::insertFile(const QString & passedfname)
{
	bibTexSea->insertFile(passedfname);
}

QFile * XWBibTexSea::openFile(const QString & filename)
{
	QFile * ret = bibTexSea->openFile(filename);
	return ret;
}

QFile * XWBibTexSea::openFile(const QString & filename, XWBibTexSea::FileFormat format)
{
	QFile * ret = bibTexSea->openFile((int)format, filename);
	return ret;
}

void XWBibTexSea::quit()
{
}

void XWBibTexSea::removeConfig(const QString & key)
{
	bibTexSea->removeConfig(key);
}

void XWBibTexSea::setDirs(XWBibTexSea::FileFormat format, 
                          const QString & dirs)
{
	PathSeaParams * params = &bibTexSeaParams[format];
	QString tmp = QString::fromAscii(params->envs);
	QStringList envs = tmp.split(";", QString::SkipEmptyParts);
	if (envs.isEmpty())
		return ;
		
	tmp = envs[0];
	
	bibTexSea->setDirs(tmp, dirs);
}

QString XWBibTexSea::varExpand(const QString & s)
{
	return bibTexSea->varExpand(s);
}

QString XWBibTexSea::varValue(const QString & s)
{
	return bibTexSea->varValue(s);
}

void XWBibTexSea::save()
{
	bibTexSea->save();
}

void XWBibTexSea::updateDBs()
{
	bibTexSea->updateDBs();
}

static PathSeaParams makeIndexSeaParams[] = 
{
	{"ist", 0, 0, 0, "TEXINDEXSTYLE;INDEXSTYLE", ".ist", 0, true, false}
};

XWMakeIndexSeaPrivate::XWMakeIndexSeaPrivate(QObject * parent)
	:XWPathSea("makeindex", (int)(XWMakeIndexSea::Last), "makeindex.cnf", "makeindex.lsr", parent)
{
}

QString XWMakeIndexSeaPrivate::initFormat(int format)
{
	FormatInfo * f = &formatInfo[format];
	if (!(f->path.isEmpty()))
		return f->path;
		
	initFormatByParams(format, &makeIndexSeaParams[format]);
	
	return f->path;
}

static XWMakeIndexSeaPrivate * makeIndexSea = 0;

XWMakeIndexSea::XWMakeIndexSea(QObject * parent)
	:QObject(parent)
{
	if (!makeIndexSea)
		makeIndexSea = new XWMakeIndexSeaPrivate;
}

void XWMakeIndexSea::addConfig(const QString & key, 
                               const QString & value, 
                               const QString & prog)
{
	makeIndexSea->addConfig(key, value, prog);
}

void XWMakeIndexSea::addFile(const QString & path)
{
	makeIndexSea->addFile(path);
}

QStringList XWMakeIndexSea::allFileNames(XWMakeIndexSea::FileFormat format)
{
	return makeIndexSea->allFileNames(&makeIndexSeaParams[format]);
}

void XWMakeIndexSea::createAllDefaultDir(const QString & topdir)
{
	int format = IST;
	while (format < Last)
	{
		createDefaultDir((FileFormat)format, topdir);
		format++;
	}
}

void XWMakeIndexSea::createDefaultDir(XWMakeIndexSea::FileFormat format,
	                                  const QString & topdir)
{
	makeIndexSea->createDefaultDir(&makeIndexSeaParams[format], topdir);
}

QStringList XWMakeIndexSea::findAll(const QString & name,
	                                XWMakeIndexSea::FileFormat format)
{
	QStringList ret = makeIndexSea->findAll(format, name);
	return ret;
}

QStringList XWMakeIndexSea::findAll(const QStringList & names,
	                                XWMakeIndexSea::FileFormat format)
{
	QStringList ret = makeIndexSea->findAll(format, names);
	return ret;
}

QString XWMakeIndexSea::findFile(const QString & name,
	                             XWMakeIndexSea::FileFormat format,
                                 bool must_exist)
{
	QString ret = makeIndexSea->findFile(name, format, must_exist);
	return ret;
}

XWMakeIndexSea::FileFormat XWMakeIndexSea::findFormat(const QString & name)
{
	int tmpfmt = makeIndexSea->findFormat(name);
    if (tmpfmt == -1)
    {
        return XWMakeIndexSea::Last;
        }

    XWMakeIndexSea::FileFormat format = (XWMakeIndexSea::FileFormat)tmpfmt;
    return format;
}

QStringList XWMakeIndexSea::getConfigDir(XWMakeIndexSea::FileFormat format)
{
	return makeIndexSea->getConfigDir(&makeIndexSeaParams[format]);
}

QStringList XWMakeIndexSea::getSuffixes(XWMakeIndexSea::FileFormat format)
{
	return makeIndexSea->getSuffixes(&makeIndexSeaParams[format]);
}

QStringList XWMakeIndexSea::getTopDirs()
{
	return makeIndexSea->getTopDirs();
}

void XWMakeIndexSea::init()
{
	if (!makeIndexSea)
		makeIndexSea = new XWMakeIndexSeaPrivate;
}

void XWMakeIndexSea::insertFile(const QString & passedfname)
{
	makeIndexSea->insertFile(passedfname);
}

QFile * XWMakeIndexSea::openFile(const QString & filename)
{
	QFile * ret = makeIndexSea->openFile(filename);
	return ret;
}

QFile * XWMakeIndexSea::openFile(const QString & filename, XWMakeIndexSea::FileFormat format)
{
	QFile * ret = makeIndexSea->openFile((int)format, filename);
	return ret;
}

void XWMakeIndexSea::quit()
{
}

void XWMakeIndexSea::removeConfig(const QString & key)
{
	makeIndexSea->removeConfig(key);
}

void XWMakeIndexSea::setDirs(XWMakeIndexSea::FileFormat format, 
                             const QString & dirs)
{
	PathSeaParams * params = &makeIndexSeaParams[format];
	QString tmp = QString::fromAscii(params->envs);
	QStringList envs = tmp.split(";", QString::SkipEmptyParts);
	if (envs.isEmpty())
		return ;
		
	tmp = envs[0];
	
	makeIndexSea->setDirs(tmp, dirs);
}

QString XWMakeIndexSea::varExpand(const QString & s)
{
	return makeIndexSea->varExpand(s);
}

QString XWMakeIndexSea::varValue(const QString & s)
{
	return makeIndexSea->varValue(s);
}

void XWMakeIndexSea::save()
{
	makeIndexSea->save();
}

void XWMakeIndexSea::updateDBs()
{
	makeIndexSea->updateDBs();
}


static PathSeaParams xwtexSeaParams[] = 
{
	{"texbin", 0, 0, "bin", "BINDIR", 0, 0, true, true},
	{"fmt", 0, 0, "fmt", "TEXFORMATS;TEXMFINI", ".afmt", ".efmt;.ofmt;.eofmt;.fmt", true, true},
	{"font", 0, 0, "bin", "XWFONTMAPDIR", ".ini", 0, true, false},
	{"format", 0, 0, "fmt", "XWFORMATDIR", ".ini", 0, true, false},
	{"language", 0, 0, "bin", "XWLANGUAGEDIR", ".ini", 0, true, false},
	{"pool", 0, 0, "pool", "XWTEXPOOL", ".pool", 0, true, false}
};

XWXWTexSeaPrivate::XWXWTexSeaPrivate(QObject * parent)
	:XWPathSea("xwtex", (int)(XWXWTexSea::Last), "xwtex.cnf", "xwtex.lsr", parent)
{
}

QString XWXWTexSeaPrivate::initFormat(int format)
{
	FormatInfo * f = &formatInfo[format];
	if (!(f->path.isEmpty()))
		return f->path;
		
	initFormatByParams(format, &xwtexSeaParams[format]);
	
	return f->path;
}

static XWXWTexSeaPrivate * xwtexSea = 0;

XWXWTexSea::XWXWTexSea(QObject * parent)
	:QObject(parent)
{
	if (!xwtexSea)
		xwtexSea = new XWXWTexSeaPrivate;
}

void XWXWTexSea::addConfig(const QString & key, 
                            const QString & value, 
                            const QString & prog)
{
	xwtexSea->addConfig(key, value, prog);
}

void XWXWTexSea::addFile(const QString & path)
{
	xwtexSea->addFile(path);
}

QStringList XWXWTexSea::allFileNames(XWXWTexSea::FileFormat format)
{
	return xwtexSea->allFileNames(&xwtexSeaParams[format]);
}

void XWXWTexSea::createAllDefaultDir(const QString & topdir)
{
	int format = Bin;
	while (format < Last)
	{
		createDefaultDir((FileFormat)format, topdir);
		format++;
	}
}

void XWXWTexSea::createDefaultDir(XWXWTexSea::FileFormat format,
	                              const QString & topdir)
{
	xwtexSea->createDefaultDir(&xwtexSeaParams[format], topdir);
}

QStringList XWXWTexSea::findAll(const QString & name,
	                            XWXWTexSea::FileFormat format)
{
	QStringList ret = xwtexSea->findAll(format, name);
	return ret;
}

QStringList XWXWTexSea::findAll(const QStringList & names,
	                            XWXWTexSea::FileFormat format)
{
	QStringList ret = xwtexSea->findAll(format, names);
	return ret;
}

QString XWXWTexSea::findFile(const QString & name,
	                         XWXWTexSea::FileFormat format,
                             bool must_exist)
{
	QString ret = xwtexSea->findFile(name, format, must_exist);
	return ret;
}

XWXWTexSea::FileFormat XWXWTexSea::findFormat(const QString & name)
{
	int tmpfmt = xwtexSea->findFormat(name);
    if (tmpfmt == -1)
    {
        return Last;
      }

    XWXWTexSea::FileFormat format = (XWXWTexSea::FileFormat)tmpfmt;
    return format;
}

QStringList XWXWTexSea::getConfigDir(XWXWTexSea::FileFormat format)
{
	return xwtexSea->getConfigDir(&xwtexSeaParams[format]);
}

void XWXWTexSea::init()
{
	if (!xwtexSea)
		xwtexSea = new XWXWTexSeaPrivate;
}

QFile * XWXWTexSea::openFile(const QString & filename)
{
	QFile * ret = xwtexSea->openFile(filename);
	return ret;
}

QFile * XWXWTexSea::openFile(const QString & filename, XWXWTexSea::FileFormat format)
{
	QFile * ret = xwtexSea->openFile((int)format, filename);
	return ret;
}

void XWXWTexSea::quit()
{
}

void XWXWTexSea::removeConfig(const QString & key)
{
	xwtexSea->removeConfig(key);
}

void XWXWTexSea::setDirs(XWXWTexSea::FileFormat format, 
                         const QString & dirs)
{
	PathSeaParams * params = &xwtexSeaParams[format];
	QString tmp = QString::fromAscii(params->envs);
	QStringList envs = tmp.split(";", QString::SkipEmptyParts);
	if (envs.isEmpty())
		return ;
		
	tmp = envs[0];
	
	xwtexSea->setDirs(tmp, dirs);
}

QString XWXWTexSea::varExpand(const QString & s)
{
	return xwtexSea->varExpand(s);
}

QString XWXWTexSea::varValue(const QString & s)
{
	return xwtexSea->varValue(s);
}

void XWXWTexSea::save()
{
	xwtexSea->save();
}

void XWXWTexSea::updateDBs()
{
	xwtexSea->updateDBs();
}

class XWTeXFontSettingPrivate
{
public:
	XWTeXFontSettingPrivate();
	~XWTeXFontSettingPrivate();
	
	QStringList allFonts()
	{return groups;}
	
	void beginGroup(const QString & key)
	{settings->beginGroup(key);}
	
	bool contains(const QString & key)
	 {return settings->contains(key);}
	 
	void endGroup()
	{settings->endGroup();}
	
	void remove(const QString & texname)
	{settings->remove(texname);}
	
	QString setTexName(const QString & name);
	
	void setBool(const QString & key, bool v)
	{settings->setValue(key, v);}
	
	void setDouble(const QString & key, double v)
	{settings->setValue(key, v);}
	
	void setInt(const QString & key, int v)
	{settings->setValue(key, v);}
	
	void setString(const QString & key, const QString & v)
	{settings->setValue(key, v);}
	
	void setSubFont(const QString & name);
	
	bool    toBool(const QString & key)
	{return settings->value(key).toBool();}
	
	double  toDouble(const QString & key)
	{return settings->value(key).toDouble();}
	
	int    toInt(const QString & key)
	{return settings->value(key).toInt();}
	
	QString toString(const QString & key)
	{return settings->value(key).toString();}
	
public:
	bool hasGroup;
	QSettings * settings;
	QStringList groups;
	QStringList subfontKeys;
};

XWTeXFontSettingPrivate::XWTeXFontSettingPrivate()
{
	hasGroup =  false;
	QString bindir = QCoreApplication::applicationDirPath();
  QString inifile = QString("%1/texfonts.ini").arg(bindir);
  settings = new QSettings(inifile, QSettings::IniFormat);
  groups = settings->childGroups();
  for (int i = 0; i < groups.size(); i++)
  {
  	QString key = groups[i];
  	settings->beginGroup(key);
  	if (settings->contains("subfont"))
  		subfontKeys << key;
  	
  	settings->endGroup();
  }
}

XWTeXFontSettingPrivate::~XWTeXFontSettingPrivate()
{
	if (hasGroup)
		settings->endGroup();
}

void XWTeXFontSettingPrivate::setSubFont(const QString & name)
{
	QString group = settings->group();
	if (!subfontKeys.contains(group))
		subfontKeys << group;
	
	setString("subfont", name);
}

QString XWTeXFontSettingPrivate::setTexName(const QString & name)
{
	if (hasGroup)
		settings->endGroup();
		
	QString ret;
	hasGroup = false;
	for (int i = 0; i < subfontKeys.size(); i++)
	{
		QString key = subfontKeys[i];
		if (name.length() > key.length() && name.startsWith(key))
		{
			QString tmp = name;
			int idx = tmp.lastIndexOf(QChar('.'));				
			if (idx > -1)
				tmp = tmp.left(idx);
			if (tmp.length() - key.length() == 2)
			{
				settings->beginGroup(key);
			  hasGroup = true;
			  tmp.remove(0, key.length());
			  ret = tmp;
			}
		}
		
		if (hasGroup)
			break;
	}
		
	if (!hasGroup)
	{
		settings->beginGroup(name);
	  hasGroup = true;
	  if (!groups.contains(name))
			groups << name;
	}
	
	return ret;
}

static XWTeXFontSettingPrivate * fontSettings = 0;

XWTeXFontSetting::XWTeXFontSetting(QObject * parent)
	:QObject(parent)
{
	if (!fontSettings)
		fontSettings = new XWTeXFontSettingPrivate;
}

XWTeXFontSetting::~XWTeXFontSetting()
{
}

QStringList XWTeXFontSetting::allFonts()
{
	return fontSettings->allFonts();
}

double  XWTeXFontSetting::getBold()
{
	if (fontSettings->contains("boldf"))
		return fontSettings->toDouble("boldf");
		
	return 0.0;
}

double XWTeXFontSetting::getCapHeight()
{
	if (fontSettings->contains("capheight"))
		return fontSettings->toDouble("capheight");
		
	return 0.8;
}

QString XWTeXFontSetting::getCMap()
{
	if (fontSettings->contains("cmap"))
		return fontSettings->toString("cmap");
		
	return QString();
}

QString XWTeXFontSetting::getCollection()
{
	if (fontSettings->contains("collection"))
		return fontSettings->toString("collection");
		
	return QString();
}

QString XWTeXFontSetting::getDescription()
{
	if (fontSettings->contains("description"))
		return fontSettings->toString("description");
		
	return QString();
}

double  XWTeXFontSetting::getEFactor()
{
	if (fontSettings->contains("efactor"))
		return fontSettings->toDouble("efactor");
		
	return 1.0;
}

int XWTeXFontSetting::getEncodingID()
{
	if (fontSettings->contains("eid"))
		return fontSettings->toInt("eid");
		
	return 1;
}

QString XWTeXFontSetting::getFont()
{
	if (fontSettings->contains("font"))
		return fontSettings->toString("font");
		
	return QString();
}

int XWTeXFontSetting::getIndex()
{
	if (fontSettings->contains("index"))
		return fontSettings->toInt("index");
		
	return 0;
}

QString XWTeXFontSetting::getInEncoding()
{
	if (fontSettings->contains("inencname"))
		return fontSettings->toString("inencname");
		
	return QString();
}

double XWTeXFontSetting::getItalicAngle()
{
	if (fontSettings->contains("italic-angle"))
		return fontSettings->toDouble("italic-angle");
		
	return 0.0;
}

int XWTeXFontSetting::getLevel()
{
	if (fontSettings->contains("level"))
		return fontSettings->toInt("level");
		
	return 0;
}

QString XWTeXFontSetting::getLigName(QString & infix)
{
	if (!fontSettings->contains("ligname"))
		return QString();
		
	QString name = fontSettings->toString("ligname");	
	QString sfd;
	if (!name.isEmpty())
	{
		fontSettings->beginGroup(name);
		sfd = getSubFont();
		if (fontSettings->contains("subfontid"))
			infix = fontSettings->toString("subfontid");
		
		if (infix.isEmpty())
			infix = "00";
		
		fontSettings->endGroup();
	}
	
	return sfd;
}

QString XWTeXFontSetting::getMetricName()
{
	if (fontSettings->contains("metric"))
		return fontSettings->toString("metric");
		
	return QString();
}

QString XWTeXFontSetting::getOutEncoding()
{
	if (fontSettings->contains("outencname"))
		return fontSettings->toString("outencname");
		
	return QString();
}

int XWTeXFontSetting::getPlane()
{
	if (fontSettings->contains("plane"))
		return fontSettings->toInt("plane");
		
	return -1;
}

int XWTeXFontSetting::getPlatID()
{
	if (fontSettings->contains("pid"))
		return fontSettings->toInt("pid");
		
	return 3;
}

double  XWTeXFontSetting::getSize()
{
	if (fontSettings->contains("size"))
		return fontSettings->toDouble("size");
		
	return 10.0;
}

double  XWTeXFontSetting::getSlant()
{
	if (fontSettings->contains("slant"))
		return fontSettings->toDouble("slant");
		
	return 0.0;
}

int XWTeXFontSetting::getStemV()
{
	if (fontSettings->contains("stemv"))
		return fontSettings->toInt("stemv");
		
	return -1;
}

QString XWTeXFontSetting::getSubFont()
{
	if (fontSettings->contains("subfont"))
		return fontSettings->toString("subfont");
		
	return QString();
}

QString XWTeXFontSetting::getVF()
{
	if (fontSettings->contains("vfont"))
		return fontSettings->toString("vfont");
		
	return QString();
}

int XWTeXFontSetting::getWMode()
{
	if (fontSettings->contains("wmode"))
		return fontSettings->toInt("wmode");
		
	return 0;
}

double  XWTeXFontSetting::getYOffset()
{
	if (fontSettings->contains("yoffset"))
		return fontSettings->toDouble("yoffset");
		
	return 0.25;
}

void XWTeXFontSetting::init()
{
	if (!fontSettings)
		fontSettings = new XWTeXFontSettingPrivate;
}

bool XWTeXFontSetting::isBold()
{
	if (fontSettings->contains("bold"))
		return fontSettings->toBool("bold");
		
	return false;
}

bool XWTeXFontSetting::isItalic()
{
	if (fontSettings->contains("italic"))
		return fontSettings->toBool("italic");
		
	return false;
}

bool XWTeXFontSetting::isNoEmbed()
{
	if (fontSettings->contains("noembed"))
		return fontSettings->toBool("noembed");
		
	return true;
}

bool XWTeXFontSetting::isRotate()
{
	if (fontSettings->contains("rotate"))
		return fontSettings->toBool("rotate");
		
	return false;
}

bool XWTeXFontSetting::isSubfontLigs()
{
	if (fontSettings->contains("subfontligs"))
		return fontSettings->toBool("subfontligs");
		
	return false;
}

bool XWTeXFontSetting::isToUnicode()
{
	if (fontSettings->contains("tounicode"))
		return fontSettings->toBool("tounicode");
		
	return true;
}

bool XWTeXFontSetting::useSmallCaps()
{
	if (fontSettings->contains("smallcaps"))
		return fontSettings->toBool("smallcaps");
		
	return false;
}

bool XWTeXFontSetting::useVF()
{
	if (fontSettings->contains("usevf"))
		return fontSettings->toBool("usevf");
		
	return false;
}

void XWTeXFontSetting::remove(const QString & texname)
{
	fontSettings->remove(texname);
}

void XWTeXFontSetting::setBold(bool e)
{
	fontSettings->setBool("bold", e);
}

void XWTeXFontSetting::setBoldF(double f)
{
	fontSettings->setDouble("boldf", f);
}

void XWTeXFontSetting::setCapHeight(double c)
{
	fontSettings->setDouble("capheight", c);
}

void XWTeXFontSetting::setCMap(const QString & name)
{
	fontSettings->setString("cmap", name);
}

void XWTeXFontSetting::setCollection(const QString & name)
{
	fontSettings->setString("collection", name);
}

void XWTeXFontSetting::setDescription(const QString & str)
{
	fontSettings->setString("description", str);
}

void XWTeXFontSetting::setEFactor(double factor)
{
	if (factor < 0.01)
		factor = 0.01;
	fontSettings->setDouble("efactor", factor);
}

void XWTeXFontSetting::setEncodingID(int id)
{
	fontSettings->setInt("eid", id);
}

void XWTeXFontSetting::setFont(const QString & name)
{
	fontSettings->setString("font", name);
}

void XWTeXFontSetting::setIndex(int idx)
{
	int i = idx;
	if (i < 0)
		i = 0;
		
	fontSettings->setInt("index", i);
}

void XWTeXFontSetting::setItalic(bool e)
{
	fontSettings->setBool("italic", e);
}

void XWTeXFontSetting::setInEncoding(const QString & name)
{
	fontSettings->setString("inencname", name);
}

void XWTeXFontSetting::setItalicAngle(double a)
{
	fontSettings->setDouble("italic-angle", a);
}

void XWTeXFontSetting::setLevel(int l)
{
	fontSettings->setInt("level", l);
}

void XWTeXFontSetting::setLigName(const QString & name, 
	                              const QString & subfont,
	                              const QString & subfontid)
{
	fontSettings->setString("ligname", name);
	fontSettings->beginGroup(name);
	setSubFont(subfont);
	fontSettings->setString("subfontid", subfontid);
	fontSettings->endGroup();
}

void XWTeXFontSetting::setMetricName(const QString & name)
{
	fontSettings->setString("metric", name);
}

void XWTeXFontSetting::setNoEmbed(bool e)
{
	fontSettings->setBool("noembed", e);
}

void XWTeXFontSetting::setOutEncoding(const QString & name)
{
	fontSettings->setString("outencname", name);
}

void XWTeXFontSetting::setPlane(const QString & p)
{
	fontSettings->setString("plane", p);
}

void XWTeXFontSetting::setPlatID(int id)
{
	fontSettings->setInt("pid", id);
}

void XWTeXFontSetting::setRotate(bool e)
{
	fontSettings->setBool("rotate", e);
}

void XWTeXFontSetting::setSize(double s)
{
	fontSettings->setDouble("size", s);
}

void XWTeXFontSetting::setSlant(double s)
{
	fontSettings->setDouble("slant", s);
}

void XWTeXFontSetting::setSmallCaps(bool e)
{
	fontSettings->setBool("smallcaps", e);
}

void XWTeXFontSetting::setStemV(int v)
{
	fontSettings->setInt("stemv", v);
}

void XWTeXFontSetting::setSubFont(const QString & name)
{
	fontSettings->setSubFont(name);
}

void XWTeXFontSetting::setSubfontLigs(bool e)
{
	fontSettings->setBool("subfontligs", e);
}

QString XWTeXFontSetting::setTexName(const QString & name)
{
	return fontSettings->setTexName(name);
}

void XWTeXFontSetting::setToUnicode(bool e)
{
	fontSettings->setBool("tounicode", e);
}

void XWTeXFontSetting::setVF(bool e, const QString & vf)
{
	fontSettings->setBool("usevf", e);
	fontSettings->setString("vfont", vf);
}

void XWTeXFontSetting::setWMode(int wm)
{
	fontSettings->setInt("wmode", wm);
}

void XWTeXFontSetting::setYOfsset(double o)
{
	fontSettings->setDouble("yoffset", o);
}

XWTeXFormatSetting::XWTeXFormatSetting(QObject * parent)
	:QObject(parent),
	 hasGroup(false)
{
	QString bindir = QCoreApplication::applicationDirPath();
    QString inifile = QString("%1/formats.ini").arg(bindir);
    settings = new QSettings(inifile, QSettings::IniFormat, this);
}

XWTeXFormatSetting::~XWTeXFormatSetting()
{
	if (hasGroup)
		settings->endGroup();
		
	delete settings;
}

QStringList XWTeXFormatSetting::allFormats()
{
	return settings->childGroups();
}

QString XWTeXFormatSetting::getArguments()
{
	if (settings->contains("arguments"))
		return settings->value("arguments").toString();
		
	return QString();
}

QString XWTeXFormatSetting::getAttributes()
{
	if (settings->contains("attributes"))
		return settings->value("attributes").toString();
		
	return QString();
}

QString XWTeXFormatSetting::getCompiler()
{
	if (settings->contains("compiler"))
		return settings->value("compiler").toString();
		
	return QString();
}

QString XWTeXFormatSetting::getDescription()
{
	if (settings->contains("description"))
		return settings->value("description").toString();
		
	return QString();
}

QString XWTeXFormatSetting::getFmtName()
{
	if (settings->contains("fmt"))
		return settings->value("fmt").toString();
		
	QString tmp = getInput();
	if (tmp.isEmpty())
		return QString();
		
	int idx = tmp.lastIndexOf(QChar('.'));
	if (idx != -1)
		tmp = tmp.left(idx);
		
	tmp += ".fmt";
	return tmp;
}

QString XWTeXFormatSetting::getInput()
{
	if (settings->contains("input"))
		return settings->value("input").toString();
		
	return QString();
}

QString XWTeXFormatSetting::getPreload()
{
	if (settings->contains("preload"))
		return settings->value("preload").toString();
		
	return QString();
}

void XWTeXFormatSetting::remove(const QString & name)
{
	settings->remove(name);
}

void XWTeXFormatSetting::setFormat(const QString & name)
{
	if (hasGroup)
		settings->endGroup();
	settings->beginGroup(name);
	hasGroup = true;
}

void XWTeXFormatSetting::setArguments(const QString & name)
{
	if (name.isEmpty())
		settings->remove("arguments");
	else
		settings->setValue("arguments", name);
}

void XWTeXFormatSetting::setAttributes(const QString & name)
{
	if (name.isEmpty())
		settings->remove("attributes");
	else
		settings->setValue("attributes", name);
}

void XWTeXFormatSetting::setBound(const QString & var, qint32 v)
{
	settings->setValue(var, v);
}

void XWTeXFormatSetting::setCompiler(const QString & name)
{
	if (name.isEmpty())
		settings->remove("compiler");
	else
		settings->setValue("compiler", name);
}

void XWTeXFormatSetting::setDescription(const QString & name)
{
	if (name.isEmpty())
		settings->remove("description");
	else
		settings->setValue("description", name);
}

void XWTeXFormatSetting::setFmtName(const QString & name)
{
	if (name.isEmpty())
		settings->remove("fmt");
	else
		settings->setValue("fmt", name);
}

void XWTeXFormatSetting::setInput(const QString & name)
{
	if (name.isEmpty())
		settings->remove("input");
	else
		settings->setValue("input", name);
}

void XWTeXFormatSetting::setPreload(const QString & name)
{
	if (name.isEmpty())
		settings->remove("preload");
	else
		settings->setValue("preload", name);
}

qint32 XWTeXFormatSetting::setupBoundVar(const QString & v, qint32 dflt)
{
	qint32 ret = dflt;
	if (settings->contains(v))
	{
		bool ok = true;
		ret = (qint32)(settings->value(v).toInt(&ok));
		if (!ok)
			ret = dflt;
	}
	
	return ret;
}

XWTeXLanguageSetting::XWTeXLanguageSetting(QObject * parent)
	:QObject(parent),
	 hasGroup(false)
{
	QString bindir = QCoreApplication::applicationDirPath();
    QString inifile = QString("%1/languages.ini").arg(bindir);
    settings = new QSettings(inifile, QSettings::IniFormat, this);
}

XWTeXLanguageSetting::~XWTeXLanguageSetting()
{
	if (hasGroup)
		settings->endGroup();
}

QStringList XWTeXLanguageSetting::allLanguages()
{
	return settings->childGroups();
}

QString XWTeXLanguageSetting::getHyphenation()
{
	if (settings->contains("hyphenation"))
		return settings->value("hyphenation").toString();
		
	return QString();
}

int XWTeXLanguageSetting::getLeftHyphenMin()
{
	if (settings->contains("lefthyphenmin"))
		return settings->value("lefthyphenmin").toInt();
		
	return 1;
}

QString XWTeXLanguageSetting::getLoader()
{
	if (settings->contains("loader"))
		return settings->value("loader").toString();
		
	return QString();
}

QString XWTeXLanguageSetting::getPatterns()
{
	if (settings->contains("patterns"))
		return settings->value("patterns").toString();
		
	return QString();
}

int XWTeXLanguageSetting::getRightHyphenMin()
{
	if (settings->contains("righthyphenmin"))
		return settings->value("righthyphenmin").toInt();
		
	return 1;
}

QString XWTeXLanguageSetting::getSpecial()
{
	if (settings->contains("special"))
		return settings->value("special").toString();
		
	return QString();
}

QString XWTeXLanguageSetting::getSynonyms()
{
	if (settings->contains("synonyms"))
		return settings->value("synonyms").toString();
		
	return QString();
}

void XWTeXLanguageSetting::remove(const QString & name)
{
	settings->remove(name);
}

void XWTeXLanguageSetting::setHyphenation(const QString & name)
{
	if (name.isEmpty())
		settings->remove("hyphenation");
	else
		settings->setValue("hyphenation", name);
}

void XWTeXLanguageSetting::setLanguage(const QString & name)
{
	if (hasGroup)
		settings->endGroup();
	settings->beginGroup(name);
	hasGroup = true;
}

void XWTeXLanguageSetting::setLeftHyphenMin(int i)
{
	settings->setValue("lefthyphenmin", i);
}

void XWTeXLanguageSetting::setLoader(const QString & name)
{
	if (name.isEmpty())
		settings->remove("loader");
	else
		settings->setValue("loader", name);
}

void XWTeXLanguageSetting::setPatterns(const QString & name)
{
	if (name.isEmpty())
		settings->remove("patterns");
	else
		settings->setValue("patterns", name);
}

void XWTeXLanguageSetting::setRightHyphenMin(int i)
{
	settings->setValue("righthyphenmin", i);
}

void XWTeXLanguageSetting::setSpecial(const QString & name)
{
	if (name.isEmpty())
		settings->remove("special");
	else
		settings->setValue("special", name);
}

void XWTeXLanguageSetting::setSynonyms(const QString & name)
{
	if (name.isEmpty())
		settings->remove("synonyms");
	else
		settings->setValue("synonyms", name);
}

