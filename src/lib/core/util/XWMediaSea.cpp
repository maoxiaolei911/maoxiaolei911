/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QMutex>
#include "XWMediaSea.h"

static QMutex mediaSeaMutex;

static PathSeaParams mediaSeaParams[] = 
{
	{"cd", 0, 0, "cd", "CDDIR", ".cda", 0, true, true},
	{"dvd", 0, 0, "dvd", "DVDDIR", ".vob;.ifo;.ac3;.dts", 0, true, true},
	{"flash", 0, 0, "flash", "FLASHDIR", ".flv;.swf", 0, true, true},
	{"gif", 0, 0, "gif", "GIFDIR", ".gif", 0, true, true},
	{"mng", 0, 0, "mng", "MNGDIR", ".mng", 0, true, true},
	{"mpegaudio", 0, 0, "mpegaudio", "MPEGAUDIODIR", ".mpa;.mp2;.m1a;'m2a;.m4a;.aac", 0, true, true},
	{"mpegvideo", 0, 0, "mpegvideo", "MPEGVIDEODIR", ".mpg;.mpeg;.mpe;.m1v;'m2v;.mp", 0, true, true},
	{"mp3", 0, 0, "mp3", "MP3DIR", ".mp3", 0, true, true},
	{"quicktime", 0, 0, "quicktime", "QUICKTIMEDIR", ".mov;.qt;.mr;3gp;.3gpp;.3g2", 0, true, true},
	{"real", 0, 0, "real", "REALDIR", ".rm;.ram;.rmvb;.rpm;.ra;.rp;.sm", 0, true, true},
	{"winaudio", 0, 0, "winaudio", "WINAUDIODIR", ".wma;.wav;.aif;.aifc", 0, true, true},
	{"winvideo", 0, 0, "winvideo", "WINVIDEODIR", ".avi;.wmv;.wmp;.wm", 0, true, true}
};


XWMediaSeaPrivate::XWMediaSeaPrivate(QObject * parent)
	:XWPathSea("media", (int)(XWMediaSea::Last), "media.cnf", "media.lsr", parent)
{
}

QString XWMediaSeaPrivate::initFormat(int format)
{
	FormatInfo * f = &formatInfo[format];
    if (!(f->path.isEmpty()))
        return f->path;

    initFormatByParams(format, &mediaSeaParams[format]);

    return f->path;
}

static XWMediaSeaPrivate * mediaSea = 0;

XWMediaSea::XWMediaSea(QObject * parent)
	:QObject(parent)
{
	if (!mediaSea)
		mediaSea = new XWMediaSeaPrivate;
}

void XWMediaSea::addConfig(const QString & key,
                           const QString & value,
                           const QString & prog)
{
	mediaSea->addConfig(key, value, prog);
}

void XWMediaSea::addFile(const QString & path)
{
	mediaSea->addFile(path);
}

QStringList XWMediaSea::allCD()
{
	return allFileNames(CD);
}

QStringList XWMediaSea::allDVD()
{
	return allFileNames(DVD);
}

QStringList XWMediaSea::allFileNames(XWMediaSea::FileFormat format)
{
	return mediaSea->allFileNames(&mediaSeaParams[format]);
}

QStringList XWMediaSea::allFlash()
{
	return allFileNames(Flash);
}

QStringList XWMediaSea::allGIF()
{
	return allFileNames(GIF);
}

QStringList XWMediaSea::allMNG()
{
	return allFileNames(MNG);
}

QStringList XWMediaSea::allMpegAudio()
{
	return allFileNames(MpegAudio);
}

QStringList XWMediaSea::allMpegVideo()
{
	return allFileNames(MpegVideo);
}

QStringList XWMediaSea::allMP3()
{
	return allFileNames(MP3);
}

QStringList XWMediaSea::allQuickTime()
{
	return allFileNames(QuickTime);
}

QStringList XWMediaSea::allReal()
{
	return allFileNames(Real);
}

QStringList XWMediaSea::allWinAudio()
{
	return allFileNames(WinAudio);
}

QStringList XWMediaSea::allWinVideo()
{
	return allFileNames(WinVideo);
}

void XWMediaSea::createAllDefaultDir(const QString & topdir)
{
	int format = CD;
	while (format < Last)
	{
		createDefaultDir((FileFormat)format, topdir);
		format++;
	}
}

void XWMediaSea::createDefaultDir(XWMediaSea::FileFormat format,
	                              const QString & topdir)
{
	mediaSea->createDefaultDir(&mediaSeaParams[format], topdir);
}

QStringList XWMediaSea::findAll(const QString & name,
	                            FileFormat format)
{
	QStringList ret = mediaSea->findAll(format, name);
	return ret;
}

QStringList XWMediaSea::findAll(const QStringList & names,
	                            FileFormat format)
{
	QStringList ret = mediaSea->findAll(format, names);
	return ret;
}
	                    
QString XWMediaSea::findCD(const QString & name)
{
	return findFile(name, CD, false);
}

QString XWMediaSea::findDVD(const QString & name)
{
	return findFile(name, DVD, false);
}

QString XWMediaSea::findFile(const QString & name)
{
	if (name.isEmpty())
	{
        return QString();
      }
        
	QString ret = mediaSea->findFile(name);
	return ret;
}

QString XWMediaSea::findFile(const QString & name,
                             XWMediaSea::FileFormat format,
                             bool must_exist)
{
	if (name.isEmpty())
	{
        return QString();
      }
    
    QString ret = mediaSea->findFile(name, format, must_exist);
	return ret;
}

QString XWMediaSea::findFlash(const QString & name)
{
	return findFile(name, Flash, false);
}

XWMediaSea::FileFormat findFormat(const QString & name)
{
	int tmpfmt = mediaSea->findFormat(name);
    if (tmpfmt == -1)
    {
        return XWMediaSea::Last;
        }

    XWMediaSea::FileFormat format = (XWMediaSea::FileFormat)tmpfmt;
    return format;
}

QString XWMediaSea::findGIF(const QString & name)
{
	return findFile(name, GIF, false);
}

QString XWMediaSea::findMNG(const QString & name)
{
	return findFile(name, MNG, false);
}

QString XWMediaSea::findMpegAudio(const QString & name)
{
	return findFile(name, MpegAudio, false);
}

QString XWMediaSea::findMpegVideo(const QString & name)
{
	return findFile(name, MpegVideo, false);
}

QString XWMediaSea::findMP3(const QString & name)
{
	return findFile(name, MP3, false);
}

QString XWMediaSea::findQuickTime(const QString & name)
{
	return findFile(name, QuickTime, false);
}

QString XWMediaSea::findReal(const QString & name)
{
	return findFile(name, Real, false);
}

QString XWMediaSea::findWinAudio(const QString & name)
{
	return findFile(name, WinAudio, false);
}

QString XWMediaSea::findWinVideo(const QString & name)
{
	return findFile(name, WinVideo, false);
}

QStringList XWMediaSea::getConfigDir(XWMediaSea::FileFormat format)
{
	return mediaSea->getConfigDir(&mediaSeaParams[format]);
}

QStringList XWMediaSea::getSuffixes(FileFormat format)
{
	return mediaSea->getSuffixes(&mediaSeaParams[format]);
}

QStringList XWMediaSea::getTopDirs()
{
	return mediaSea->getTopDirs();
}

void XWMediaSea::init()
{
	if (!mediaSea)
		mediaSea = new XWMediaSeaPrivate;
}

void XWMediaSea::insertFile(const QString & passedfname)
{
	mediaSea->insertFile(passedfname);
}

QFile * XWMediaSea::openFile(const QString & filename, FileFormat format)
{
	QFile * ret = mediaSea->openFile((int)format, filename);
	return ret;
}

void XWMediaSea::quit()
{
}

void XWMediaSea::removeConfig(const QString & key)
{
	mediaSea->removeConfig(key);
}

void XWMediaSea::setDirs(XWMediaSea::FileFormat format, 
                         const QString & dirs)
{
	PathSeaParams * params = &mediaSeaParams[format];
	QString tmp = QString::fromAscii(params->envs);
	QStringList envs = tmp.split(";", QString::SkipEmptyParts);
	if (envs.isEmpty())
		return ;
		
	tmp = envs[0];
	
	mediaSea->setDirs(tmp, dirs);
}

QString XWMediaSea::varExpand(const QString & s)
{
	return mediaSea->varExpand(s);
}

QString XWMediaSea::varValue(const QString & s)
{
	return mediaSea->varValue(s);
}

void XWMediaSea::save()
{
	mediaSea->save();
}

void XWMediaSea::updateDBs()
{
	mediaSea->updateDBs();
}
