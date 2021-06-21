/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QMutex>
#include <QCoreApplication>
#include <QRegExp>
#include <QImage>
#include <QProcess>
#include "XWApplication.h"
#include "XWFileName.h"
#include "XWPictureSea.h"

static QMutex picSeaMutex;

static PathSeaParams picSeaParams[] = 
{
	{"bmp", 0, 0, "bmp", "BMPDIR", ".bmp;.rle;.dib", 0, true, true},
	{"eps", 0, 0, "eps", "EPSDIR", ".eps", 0, true, true},
	{"exr", 0, 0, "exr", "EXRDIR", ".exr", 0, true, true},
	{"gif", 0, 0, "gif", "GIFDIR", ".gif", 0, true, true},
	{"icon", 0, 0, "icon", "ICONDIR", ".ico", 0, true, true},
	{"iff", 0, 0, "iff", "IFFDIR", ".iff;.tdt", 0, true, true},
	{"jpeg", 0, 0, "jpeg", "JPGDIR", ".jpg;.jpeg;.jpe;.jpf;jpx;.jp2;.j2c;j2k;.jfif", 0, true, true},
	{"metapost", 0, 0, "metapost", "MPDIR", ".mps", 0, true, true},
	{"pbm", 0, 0, "pbm", "PBMDIR", ".pbm;.pgm;.ppm;.pnm;.pfm", 0, true, true},
	{"pcx", 0, 0, "pcx", "PCXDIR", ".pcx", 0, true, true},
	{"pdf", 0, 0, "pdf", "PDFDIR", ".pdf", ".epdf", true, true},
	{"pdp", 0, 0, "pdp", "PDPDIR", ".pdf;.pdp", 0, true, true},
	{"pict", 0, 0, "pict", "PICTDIR", ".pct;.pict", 0, true, true},
	{"png", 0, 0, "png", "PNGDIR", ".png", 0, true, true},
	{"ps", 0, 0, "ps", "PSDIR", ".ps", 0, true, false},
	{"psd", 0, 0, "psd", "PSDDIR", ".psd;.pdd", 0, true, true},
	{"pxr", 0, 0, "pxr", "PXRDIR", ".pxr", 0, true, true},
	{"targa", 0, 0, "targa", "TARGADIR", ".tga;.vda;.icb;.vst", 0, true, true},
	{"tiff", 0, 0, "tiff", "TIFFDIR", ".tif;.tiff", 0, true, true},
	{"xbm", 0, 0, "xbm", "XBMDIR", ".xbm", 0, true, true},
	{"xpm", 0, 0, "xpm", "XPMDIR", ".xpm", 0, true, true}
};

XWPictureSeaPrivate::XWPictureSeaPrivate(QObject * parent)
	:XWPathSea("picture", (int)(XWPictureSea::Last), "picture.cnf", "picture.lsr", parent)
{
	QString bindir = QCoreApplication::applicationDirPath();
    QString inifile = QString("%1/filter.ini").arg(bindir);
    settings = new QSettings(inifile, QSettings::IniFormat, this);
}

XWPictureSeaPrivate::~XWPictureSeaPrivate()
{
	if (settings)
		delete settings;
}

QString XWPictureSeaPrivate::initFormat(int format)
{
	FormatInfo * f = &formatInfo[format];
    if (!(f->path.isEmpty()))
        return f->path;

    initFormatByParams(format, &picSeaParams[format]);

    return f->path;
}

static XWPictureSeaPrivate * picSea = 0;

XWPictureSea::XWPictureSea(QObject * parent)
	:QObject(parent)
{
	if (!picSea)
		picSea = new XWPictureSeaPrivate;
}

void XWPictureSea::addConfig(const QString & key,
                             const QString & value,
                             const QString & prog)
{
	picSea->addConfig(key, value, prog);
}

void XWPictureSea::addFile(const QString & path)
{
	picSea->addFile(path);
}

QStringList XWPictureSea::allBMP()
{
	return allFileNames(BMP);
}

QStringList XWPictureSea::allEPS()
{
	return allFileNames(EPS);
}

QStringList XWPictureSea::allEXR()
{
	return allFileNames(EXR);
}

QStringList XWPictureSea::allFileNames(XWPictureSea::FileFormat format)
{
	return picSea->allFileNames(&picSeaParams[format]);
}

QStringList XWPictureSea::allGIF()
{
	return allFileNames(GIF);
}

QStringList XWPictureSea::allICON()
{
	return allFileNames(ICON);
}

QStringList XWPictureSea::allIFF()
{
	return allFileNames(IFF);
}

QStringList XWPictureSea::allJPEG()
{
	return allFileNames(JPEG);
}

QStringList XWPictureSea::allMP()
{
	return allFileNames(MP);
}

QStringList XWPictureSea::allPBM()
{
	return allFileNames(PBM);
}

QStringList XWPictureSea::allPCX()
{
	return allFileNames(PCX);
}

QStringList XWPictureSea::allPDP()
{
	return allFileNames(PDP);
}

QStringList XWPictureSea::allPICT()
{
	return allFileNames(PICT);
}

QStringList XWPictureSea::allPNG()
{
	return allFileNames(PNG);
}

QStringList XWPictureSea::allPS()
{
	return allFileNames(PS);
}

QStringList XWPictureSea::allPSD()
{
	return allFileNames(PSD);
}

QStringList XWPictureSea::allPXR()
{
	return allFileNames(PXR);
}

QStringList XWPictureSea::allTarga()
{
	return allFileNames(Targa);
}

QStringList XWPictureSea::allXBM()
{
	return allFileNames(XBM);
}

QStringList XWPictureSea::allXPM()
{
	return allFileNames(XPM);
}

void XWPictureSea::createAllDefaultDir(const QString & topdir)
{
	int format = BMP;
	while (format < Last)
	{
		createDefaultDir((FileFormat)format, topdir);
		format++;
	}
}

void XWPictureSea::createDefaultDir(XWPictureSea::FileFormat format,
	                                const QString & topdir)
{
	picSea->createDefaultDir(&picSeaParams[format], topdir);
}

QStringList XWPictureSea::findAll(const QString & name,
	                              XWPictureSea::FileFormat format)
{
	QStringList ret = picSea->findAll(format, name);
	return ret;
}

QStringList XWPictureSea::findAll(const QStringList & names,
	                              XWPictureSea::FileFormat format)
{
	QStringList ret = picSea->findAll(format, names);
	return ret;
}

QString XWPictureSea::findFile(const QString & name)
{
	if (name.isEmpty())
	{
        return QString();
      }
        
    QString ret = picSea->findFile(name);
	return ret;
}
	                    
QString XWPictureSea::findFile(const QString & name,
	                           XWPictureSea::FileFormat format,
                               bool must_exist)
{
	if (name.isEmpty())
	{
        return QString();
      }

    QString ret = picSea->findFile(name, format, must_exist);
	return ret;
}

XWPictureSea::FileFormat XWPictureSea::findFormat(const QString & name)
{
	int tmpfmt = picSea->findFormat(name);
    if (tmpfmt == -1)
    {
        return XWPictureSea::Last;
        }

    XWPictureSea::FileFormat format = (XWPictureSea::FileFormat)tmpfmt;
    return format;
}

QStringList XWPictureSea::getConfigDir(XWPictureSea::FileFormat format)
{
	return picSea->getConfigDir(&picSeaParams[format]);
}

void XWPictureSea::getFilter(FileFormat format, 
	                         QString & cmd,
	                         QString & infilep,
	                         QString & outfilep,
	                         QString & outext)
{
	PathSeaParams * params = &picSeaParams[format];
	QString group = QString::fromAscii(params->type);
	QSettings * settings = picSea->settings;
	settings->beginGroup(group);
	cmd = settings->value("cmd").toString();
	infilep = settings->value("i").toString();
	outfilep = settings->value("o").toString();
	outext = settings->value("oext").toString();
	settings->endGroup();
}

QStringList XWPictureSea::getSuffixes(XWPictureSea::FileFormat format)
{
	return picSea->getSuffixes(&picSeaParams[format]);
}

QStringList XWPictureSea::getTopDirs()
{
	return picSea->getTopDirs();
}

void XWPictureSea::init()
{
	if (!picSea)
		picSea = new XWPictureSeaPrivate;
}

void XWPictureSea::insertFile(const QString & passedfname)
{
	picSea->insertFile(passedfname);
}

QFile * XWPictureSea::openFile(const QString & filename, bool * tmp)
{
	int tmpfmt = picSea->findFormat(filename);
	if (tmpfmt == -1)
	{
		return 0;
	}
		
	QString fullname = findFile(filename, (XWPictureSea::FileFormat)tmpfmt, false);
	if (fullname.isEmpty())
	{
		return 0;
	}
		
	QFile * fp = 0;
	if (!tmp)
	{
		fp = new QFile(fullname);
		if (!fp->open(QIODevice::ReadOnly))
		{
			delete fp;
			fp = 0;
		}
		return fp;
	}
	
	PathSeaParams * params = &picSeaParams[tmpfmt];
	QString group = QString::fromAscii(params->type);
	QSettings * settings = picSea->settings;
	settings->beginGroup(group);
	QString cmd = settings->value("cmd").toString();
	QString inp = settings->value("i").toString();
	QString outp = settings->value("o").toString();
	QString oute = settings->value("oext").toString();
	QStringList arguments;
	QString program;
	
	XWFileName fn(fullname);
	QString bn = fn.baseName();
	QString tmpfile = xwApp->getTmpFile(bn);
	if (!cmd.isEmpty())
	{
		if (!oute.isEmpty())
		{
			if (oute[0] != QChar('.'))
				tmpfile += ".";
				
			tmpfile += oute;
		}
		else
			tmpfile += ".pdf";
				
		QStringList tmplist = cmd.split(QRegExp("\\s+"));
		program = tmplist[0];		
		if (tmplist.size() > 1)
		{
			for (int i = 0; i < tmplist.size(); i++)
			{
				QString tmp = tmplist.at(i);
				if (!inp.isEmpty() && tmp.contains(inp))
				{
					int idx = tmp.indexOf(inp);
					tmp.replace(idx, inp.length(), fullname);
					arguments << tmp;
				}
				else if (outp.isEmpty() && tmp.contains(outp))
				{
					int idx = tmp.indexOf(inp);
					tmp.replace(idx, inp.length(), tmpfile);
					arguments << tmp;
				}
				else
					arguments << tmp;
			}
		}
		else
		{
			arguments << fullname;
			arguments << tmpfile;
		}
	}
	else if (tmpfmt == EPS || tmpfmt == PS)
	{
		tmpfile += ".pdf";
		
#if defined(Q_OS_WIN32)
		program = "gswin32c";
#elif defined(Q_OS_WIN64)
		program = "gswin64c";
#elif defined(Q_OS_OS2)
		program = "gsos2";
#else
		program = "gs";
#endif
		arguments << "-P-" << "-dSAFER" << "-dCompatibilityLevel=1.4" << "-q" << "-dNOPAUSE" << "-dBATCH" 
				  << "-sDEVICE=pdfwrite";
				  
		QString tmpo = QString("-sOutputFile=%1").arg(tmpfile);
		
		arguments << tmpo;
		
		arguments << "-c" << ".setpdfwrite" << "-f" << fullname;
	}
	else if (tmpfmt == GIF || 
		     tmpfmt == PBM || 
		     tmpfmt == TIFF || 
		     tmpfmt == XBM || 
		     tmpfmt == XPM)
	{
		tmpfile += ".png";
		QImage img(fullname);
		img.save(tmpfile, "PNG");
	}
	else
	{
		*tmp = false;
		fp = new QFile(fullname);
		if (!fp->open(QIODevice::ReadOnly))
		{
			delete fp;
			fp = 0;
		}
		settings->endGroup();
		return fp;
	}
	
	*tmp = true;
	if (!program.isEmpty())
	{
		QProcess prog;
		prog.start(program, arguments);
		if (!prog.waitForStarted())
		{
			settings->endGroup();
			return 0;
		}
			
		if (!prog.waitForFinished())
		{
			settings->endGroup();
			return 0;
		}
	}
	
	fp = new QFile(tmpfile);
	if (!fp->open(QIODevice::ReadOnly))
	{
		delete fp;
		fp = 0;
	}
	
	settings->endGroup();
	return fp;
}

QFile * XWPictureSea::openFile(const QString & filename, 
	                           XWPictureSea::FileFormat format)
{
	QFile * ret = picSea->openFile((int)format, filename);
	return ret;
}

void XWPictureSea::quit()
{
}

void XWPictureSea::removeConfig(const QString & key)
{
	picSea->removeConfig(key);
}

void XWPictureSea::setDirs(XWPictureSea::FileFormat format, 
                           const QString & dirs)
{
	PathSeaParams * params = &picSeaParams[format];
	QString tmp = QString::fromAscii(params->envs);
	QStringList envs = tmp.split(";", QString::SkipEmptyParts);
	if (envs.isEmpty())
		return ;
		
	tmp = envs[0];
	
	picSea->setDirs(tmp, dirs);
}

void XWPictureSea::setFilter(XWPictureSea::FileFormat format, 
	                         const QString & cmd,
	                         const QString & infilep,
	                         const QString & outfilep,
	                         const QString & outext)
{
	PathSeaParams * params = &picSeaParams[format];
	QString group = QString::fromAscii(params->type);
	QSettings * settings = picSea->settings;
	settings->beginGroup(group);
	settings->setValue("cmd", cmd);
	settings->setValue("i", infilep);
	settings->setValue("o", outfilep);
	settings->setValue("oext", outext);
	settings->endGroup();
}

QString XWPictureSea::varExpand(const QString & s)
{
	return picSea->varExpand(s);
}

QString XWPictureSea::varValue(const QString & s)
{
	return picSea->varValue(s);
}

void XWPictureSea::save()
{
	picSea->save();
}

void XWPictureSea::updateDBs()
{
	picSea->updateDBs();
}

