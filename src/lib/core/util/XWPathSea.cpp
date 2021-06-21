/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <QCoreApplication>
#include <malloc.h>
#include <stdlib.h>
#include <QFileInfo>
#include <QDir>
#include <QByteArray>
#include <QTextCodec>

#include "XWStringUtil.h"
#include "XWFileName.h"
#include "XWUtil.h"
#include "XWApplication.h"
#include "XWCnf.h"
#include "XWFileDB.h"
#include "XWVariable.h"
#include "XWPathSea.h"

static bool nameBeginsWithDevice(QChar * elt)
{
	if (!elt || elt[0].isNull())
		return false;

	return isDevSep(elt[1]);
}

static bool isUNCName(QChar * elt)
{
	if (!elt || elt[0].isNull() || elt[1].isNull() || elt[2].isNull())
		return false;

	return (isDirSep(elt[0]) &&
	        isDirSep(elt[1]) &&
	        elt[2].isLetterOrNumber());
}

static int normalizePath(QChar * elt)
{
	int i = 0;
	int ret = 0;
	if (nameBeginsWithDevice(elt))
	{
		if (elt[0] >= QChar('A') && elt[0] <= QChar('Z'))
        	elt[0] = elt[0].toLower();

        for (i = 2; isDirSep(elt[i]); ++i);

        if (i > 3)
        {
        	QChar * p = elt + 3;
        	int k = 0;
        	while (!(p[k + i].isNull()))
        	{
        		p[k] = p[k + i];
        		k++;
        	}

        	p[k] = QChar('\0');
        }

        ret = 2;
	}
	else if (isUNCName(elt))
	{
		for (ret = 2; (!(elt[ret].isNull())) && !isDirSep(elt[ret]); ++ret);
		for (i = ret; (!(elt[i].isNull())) && isDirSep(elt[i]); ++i);

		if (i > ret + 1)
		{
			QChar * p = elt + ret + 1;
			int k = 0;
			while (!(p[k + i].isNull()))
        	{
        		p[k] = p[k + i];
        		k++;
        	}

        	p[k] = QChar('\0');
		}
	}
	else
	{
		for (ret = 0; isDirSep(elt[ret]); ++ret);
	}

	return ret;
}

static bool isReadableFile(const QString & name)
{
	QFileInfo info(name);
	return (info.isFile() && info.isReadable());
}

static bool isAbsolutePath(const QChar * filename,
                           bool relative_ok)
{
	bool absolute = isDirSep(filename[0]) || nameBeginsWithDevice((QChar * )filename);
	bool explicit_relative = relative_ok &&
	                         (filename[0] == QChar('.') && (isDirSep(filename[1])
                              || (filename[1] == QChar('.') &&
                              isDirSep(filename[2]))));

	return absolute || explicit_relative;
}

struct LListElt
{
    QString * str;
    bool moved;
    LListElt * next;
};

struct CacheEntry
{
    QString * key;
    LListElt ** value;
};

class XWPathCache
{
public:
	XWPathCache(XWPathSea * seaA);
	~XWPathCache();

	QString pathExpand(const QString & path);
	QStringList pathSearch(const QString & path,
	                       const QString & name,
                           bool must_exist,
                           bool all);

	QStringList search(const QString & path,
	                   const QString & original_name,
                       bool must_exist,
                       bool all);
	QStringList searchList(const QString & path,
	                       const QStringList & names,
                           bool must_exist,
                           bool all);

private:
	void cache(const QChar * key, LListElt ** value);
	LListElt ** cached(const QChar * key);

	void checkedDirListAdd(LListElt ** l, const QChar * dir);

	int  dirLinks(const QChar * fn, int nlinks);
	void dirListAdd(LListElt ** l, const QChar * dir);
	QStringList dirListSearch(LListElt ** dirs,
	                          const QString &name,
	                          bool search_all);
	QStringList dirListSearchList(LListElt ** dirs,
	                              const QStringList &names,
	                              bool search_all);

	void doSubdir(LListElt ** l,
                  const QChar* elt,
                  int elt_length,
                  const QChar * post);

	LListElt ** elementDirs(QChar * elt);
	void expandElt(LListElt ** l, const QChar * elt, int start);
	
	void freeLList(LListElt * l);

	void llistAdd(LListElt ** l, const QString & str);
	void llistFloat(LListElt ** l, LListElt * mover);

private:
	XWPathSea * sea;
    CacheEntry * theCache;
    int          cachLength;
    bool         firstSearch;

    QHash<QString, int> linkTable;
};

XWPathCache::XWPathCache(XWPathSea * seaA)
	:sea(seaA),
	 theCache(0),
	 cachLength(0),
	 firstSearch(true)
{
}

XWPathCache::~XWPathCache()
{
	if (theCache)
	{
		for (int i = 0; i < cachLength; i++)
		{
			CacheEntry * entry = &theCache[i];
			if (entry->key)
				delete entry->key;
			LListElt ** listelts = theCache[i].value;
			freeLList(listelts[0]);
		}
		
		free(theCache);
	}
}

QString XWPathCache::pathExpand(const QString & path)
{
	const QChar * xpath = path.data();
	QChar * elt = 0;
	QString ret;
	int len = 0;
	for (elt = sea->pathElement(xpath); elt; elt = sea->pathElement(0))
	{
		if (elt[0] == QChar('!') && elt[1] == QChar('!'))
      		elt += 2;

      	LListElt ** dirs = elementDirs(elt);
      	if (dirs && *dirs)
      	{
      		LListElt * dir = *dirs;
      		while (dir)
      		{
      			QChar * thedir = (QChar*)(dir->str->data());
      			int dirlen = dir->str->length();
      			if (dirlen == 1 ||
      				(dirlen == 3 &&
      				nameBeginsWithDevice(thedir) &&
      				isDirSep(thedir[2])))
                {
                	ret += *(dir->str);
                	ret += envSep();
                	len += dirlen + 1;
                }
                else
                {
                	ret += *(dir->str);
                	len += dirlen;
                }

                ret.resize(len);
                ret[len - 1] = envSep();
                dir = dir->next;
      		}
      	}
	}

	if (len != 0)
    	ret[len - 1] = QChar('\0');
  	return ret;
}

QStringList XWPathCache::pathSearch(const QString & path,
	                                const QString & name,
                                    bool must_exist,
                                    bool all)
{
	QStringList ret;
	const QChar * xpath = path.data();
	QChar * elt = 0;
	bool done = false;
	for (elt = sea->pathElement(xpath); !done && elt; elt = sea->pathElement(0))
	{
		bool allow_disk_search = true;
		if (elt[0] == QChar('!') && elt[1] == QChar('!'))
		{
			allow_disk_search = false;
      		elt += 2;
		}

		normalizePath(elt);

		QStringList found;
		{
			QString tmp(elt);
			found = sea->fileDB->search(name, tmp, all);
		}

		if (allow_disk_search &&
			(found.isEmpty() ||
			(must_exist && found.at(0).isEmpty())))
		{
			LListElt ** dirs = elementDirs(elt);
			if (dirs && *dirs)
			{
				if (found.isEmpty())
					found = dirListSearch(dirs, name, all);
			}
		}

		if (!found.isEmpty() && !(found.at(0).isEmpty()))
		{
			if (all)
				ret << found;
			else
			{
				ret << found.at(0);
				done = true;
			}
		}
	}

	return ret;
}

QStringList XWPathCache::search(const QString & path,
	                            const QString & original_name,
                                bool must_exist,
                                bool all)
{
	QString xname = sea->expand(original_name);
	QChar * name = xname.data();
	bool absolute_p = isAbsolutePath(name, true);
	QStringList ret;
	if (absolute_p)
	{
		QFileInfo info(xname);
		if (info.isFile() && info.isReadable())
			ret << xname;
	}
	else
		ret = pathSearch(path, xname, must_exist, all);

	if (firstSearch)
		firstSearch = false;

	return ret;
}

QStringList XWPathCache::searchList(const QString & path,
	                                const QStringList & names,
                                    bool must_exist,
                                    bool all)
{
	QStringList ret;
  	bool all_absolute = true;
	for (int i = 0; i < names.size(); i++)
	{
		QString name = names.at(i);
		QChar * namep = name.data();
		if (isAbsolutePath(namep, true))
		{
			if (isReadableFile(name))
				ret << name;
				
			if (!all)
				goto out;
		}
		else
			all_absolute = false;
	}

	if (all_absolute)
		goto out;

	const QChar * xpath = path.data();
	QChar * elt = 0;
	bool done = false;
	for (elt = sea->pathElement(xpath); !done && elt; elt = sea->pathElement(0))
	{
		bool allow_disk_search = true;
		if (elt[0] == QChar('!') && elt[1] == QChar('!'))
		{
			allow_disk_search = false;
      		elt += 2;
		}

		normalizePath(elt);

		QStringList found;
		{
			QString tmp(elt);
			found = sea->fileDB->searchList(names, tmp, all);
		}

		if (allow_disk_search &&
			(found.isEmpty() ||
			(must_exist && found.at(0).isEmpty())))
		{
			LListElt ** dirs = elementDirs(elt);
			if (dirs && *dirs)
			{
				if (found.isEmpty())
					found = dirListSearchList(dirs, names, all);
			}
		}

		if (!found.isEmpty() && !(found.at(0).isEmpty()))
		{
			if (all)
				ret << found;
			else
			{
				ret << found.at(0);
				done = true;
			}
		}
	}

out:
	if (firstSearch)
		firstSearch = false;

	return ret;
}

void XWPathCache::cache(const QChar * key, LListElt ** value)
{
	cachLength++;
	theCache = (CacheEntry*)realloc(theCache, cachLength * sizeof(CacheEntry));
	theCache[cachLength - 1].key = new QString(key);
  	theCache[cachLength - 1].value = value;
}

LListElt ** XWPathCache::cached(const QChar * key)
{
	QString tmp(key);
	for (int i = 0; i < cachLength; i++)
    {
        if (0 == (theCache[i].key)->compare(tmp, Qt::CaseInsensitive))
            return theCache[i].value;
    }

    return 0;
}

void XWPathCache::checkedDirListAdd(LListElt ** l, const QChar * dir)
{
	QString tmp(dir);
	QFileInfo info(tmp);
	if (info.isDir())
		dirListAdd(l, dir);
}

int XWPathCache::dirLinks(const QChar * fn, int nlinks)
{
	QString tmp(fn);
	int ret = nlinks;
	if (linkTable.contains(tmp))
		ret = linkTable[tmp];
	else
	{
#ifdef Q_OS_WIN
		linkTable[tmp] = nlinks;
#else
		QTextCodec * codec = QTextCodec::codecForLocale();
		QByteArray ba = codec->fromUnicode(tmp);
		const char * fd = ba.data();
		struct stat stats;
		if (stat (fd, &stats) == 0 && S_ISDIR (stats.st_mode))
        	nlinks = stats.st_nlink;
        else
        	nlinks = -1;

        linkTable[tmp] = nlinks;
#endif
	}

	return ret;
}

void XWPathCache::dirListAdd(LListElt ** l, const QChar * dir)
{
	QString tmp(dir);
	int p = tmp.length() - 1;
	QChar lastChar = dir[p];
	if (isDirSep(lastChar) || isDevSep(lastChar))
		llistAdd(l, tmp);
	else
	{
		QString str = QString("%1/").arg(tmp);
		llistAdd(l, str);
	}
}

QStringList XWPathCache::dirListSearch(LListElt ** dirs,
	                                   const QString &name,
	                                   bool search_all)
{
	QStringList ret;
	LListElt * elt = *dirs;
	while (elt)
	{
		QString potential = *(elt->str);
		potential += name;
		if (isReadableFile(potential))
		{
			ret << potential;
			llistFloat(dirs, elt);
			if (!search_all)
            	return ret;
		}

		elt = elt->next;
	}

	return ret;
}

QStringList XWPathCache::dirListSearchList(LListElt ** dirs,
	                                       const QStringList &names,
	                                       bool search_all)
{
	QStringList ret;
	LListElt * elt = *dirs;
	while (elt)
	{
		for (int i = 0; i < names.size(); i++)
		{
			QString tmp = names.at(i);
			QChar * name = (QChar*)(tmp.data());
			if (isAbsolutePath(name, true))
				continue;

			QString potential = *(elt->str);
			potential += tmp;
			if (isReadableFile(potential))
			{
				ret << potential;
				llistFloat(dirs, elt);
				if (!search_all)
            		return ret;
			}
		}

		elt = elt->next;
	}

	return ret;
}

void XWPathCache::doSubdir(LListElt ** l,
                           const QChar* elt,
                           int elt_length,
                           const QChar * post)
{
	QString name(elt, elt_length);
	QDir dir(name);

	if (!dir.isReadable())
		return ;

	if (post->isNull())
		dirListAdd(l, name.data());
	else
	{
		QString tmp(post);
		int len = name.length();
		name += tmp;
		const QChar * tmpelt = (const QChar *)(name.data());
		expandElt(l, tmpelt, elt_length);
		name = name.left(len);
	}

	int nlinks = 2;
	QStringList dirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	for (int i = 0; i < dirs.size(); i++)
	{
		name += dirs.at(i);
		int links = dirLinks(name.data(), 0);

		int potential_len = name.length();
		nlinks++;
		name += "/";
		if (!(post->isNull()))
		{
			QString tmp(post);
			name += tmp;
			const QChar * tmpelt = name.data();
			expandElt(l, tmpelt, potential_len);
			name = name.left(potential_len);
		}

#ifdef Q_OS_WIN
		if (links == 0 || links > 2)
#else
		if (links > 2)
#endif
		{
			const QChar * tmpelt = (const QChar * )(name.data());
			doSubdir(l, tmpelt, potential_len, post);
		}
		else if (post->isNull())
			dirListAdd(l, name.data());

		name = name.left(elt_length);
	}

#ifdef Q_OS_WIN
	dirLinks(name.data(), nlinks);
#endif
}

LListElt ** XWPathCache::elementDirs(QChar * elt)
{
	if (elt->isNull())
		return 0;

	LListElt ** ret = cached(elt);
	if (ret)
    	return ret;

    ret = (LListElt**)malloc(sizeof(LListElt*));
    *ret = 0;
    expandElt(ret, elt, normalizePath(elt));

    cache(elt, ret);
    return ret;
}

void XWPathCache::expandElt(LListElt ** l, const QChar * elt, int start)
{
	const QChar * dir = elt + start;
	while (!(dir->isNull()))
	{
		if (isDirSep(dir[0]))
		{
			if (isDirSep(dir[1]))
			{
				const QChar * post = dir + 1;
				while (isDirSep(post[0]))
				    post++;

				doSubdir(l, elt, dir - elt + 1, post);

				return;
			}
		}

		dir++;
	}

	checkedDirListAdd(l, elt);
}

void XWPathCache::freeLList(LListElt * l)
{
	while (l)
	{
		LListElt * tmp = l->next;
		if (l->str)
			delete l->str;
		free(l);
		l = tmp;
	}
}

void XWPathCache::llistAdd(LListElt ** l, const QString & str)
{
	LListElt * new_elt = (LListElt*)malloc(sizeof(LListElt));
	new_elt->str = new QString(str);
	new_elt->moved = false;
	new_elt->next = 0;

	LListElt * e = *l;
	while (e && e->next)
		e = e->next;

	if (!e)
		*l = new_elt;
	else
		e->next = new_elt;
}

void XWPathCache::llistFloat(LListElt ** l, LListElt * mover)
{
	if (!mover || mover->moved)
		return ;

	LListElt * last_moved = 0;
	LListElt * unmoved = *l;
	while (unmoved->moved)
	{
		last_moved = unmoved;
		unmoved = unmoved->next;
	}

	if (unmoved != mover)
	{
		LListElt * before_mover = unmoved;
		LListElt * after_mover = mover->next;
		while (before_mover->next != mover)
			before_mover = before_mover->next;

		before_mover->next = after_mover;
		mover->next = unmoved;
		if (!last_moved)
        	*l = mover;
      	else
      		last_moved->next = mover;
	}

	mover->moved = true;
}

XWPathSea::XWPathSea(int   formatsize,
	                 QObject * parent)
	:QObject(parent)
{
	formatSize = formatsize;
	formatInfo = new FormatInfo[formatSize];
    cnf = new XWCnf;
    variable = new XWVariable(cnf);
    fileDB = new XWFileDB;
    maxTopDir = 0;
    configDir = QCoreApplication::applicationDirPath();
    if (isDevSep(configDir[1]))
		configDir[0] = configDir[0].toLower();
    cache = new XWPathCache(this);

    cnfDirty = false;

    pathSeaElt = 0;
    pathSeaEltAlloc = 0;
    pathSeaPath = 0;
}

XWPathSea::XWPathSea(const QString & type,
	                 int   formatsize,
	                 const QString & cnfname,
	                 const QString & dbname,
	                 QObject * parent)
	:QObject(parent)
{
	seaType = type;
	topDirEnv = type.toUpper();
	topDirEnv += "TOPDIR";
	formatSize = formatsize;
    formatInfo = new FormatInfo[formatSize];
    cnf = new XWCnf(cnfname);
    variable = new XWVariable(cnf);

    maxTopDir = 0;
	configDir = QCoreApplication::applicationDirPath();
    if (isDevSep(configDir[1]))
		configDir[0] = configDir[0].toLower();

    QString f = QString("%1/%2").arg(configDir).arg(cnfname);
    QFile cnffile(f);
    fileDB = new XWFileDB(dbname);
    if (cnffile.exists())
    {
    	cnf->readCnf(configDir);
    	topDir = cnf->get(topDirEnv);
    	if (!topDir.isEmpty())
    	{
    		QChar c = envSep();
    		QStringList tmplist = topDir.split(c, QString::SkipEmptyParts);
    		if (tmplist.size() > 0)
    		{
    			QString tmp;
    			for (int i = 0; i < tmplist.size(); i++)
    			{
    				QString var = tmplist[i];
    				if (isVarStart(var[0]))
    				{
    					QChar * p = var.data();
    					while (!p->isNull() && !p->isDigit())
    						p++;

    					if (!p->isNull())
    					{
    						QString n = QString(p);
    						int tn = n.toInt();
    						if (tn > maxTopDir)
    							maxTopDir = tn;
    					}
    					
    					tmp = varExpand(var);
    				}
    				else
    					tmp = var;
    				
    				tmp = getAbsoluteDir(configDir, tmp);
    				topDirHash[tmp] = var;
    				if (isVarStart(var[0]))
    					var.remove(0, 1);
    				putEnv(var, tmp);
    				QDir d(tmp);
    				if (d.exists())
    				{
    					fileDB->init(tmp);
    				}
    			}
    		}
    	}
    }

    cache = new XWPathCache(this);

    cnfDirty = false;

    pathSeaElt = 0;
    pathSeaEltAlloc = 0;
    pathSeaPath = 0;
}

XWPathSea::~XWPathSea()
{
	if (formatInfo)
		delete [] formatInfo;

	if (fileDB)
		delete fileDB;

	if (cnf)
	{
		save();
		delete cnf;
	}

	if (variable)
		delete variable;

	if (cache)
		delete cache;

	if (pathSeaElt)
		free(pathSeaElt);
}

void XWPathSea::addConfig(const QString & key,
                          const QString & value,
                          const QString & prog)
{
	if (key.isEmpty() || value.isEmpty())
		return ;

	cnf->add(key, value, prog);
	cnfDirty = true;
}

void XWPathSea::addFile(const QString & path)
{
	XWFileName fn(path);
	QString dir = fn.dirPart();
	int format = findFormat(path);
	if (format != -1)
	{
		FormatInfo * f = &formatInfo[format];
		if (!(f->path.contains(dir)))
		{
			if (!(f->path.isEmpty()))
			{
				QChar c = envSep();
				f->path.append(c);
			}

			f->path.append(dir);
			f->path.append("//");
		}
	}

	fileDB->addDir(dir);
	fileDB->insert(path);
}

QStringList XWPathSea::allFileNames()
{
	return fileDB->fileNames();
}

QStringList XWPathSea::allFileNames(const QStringList & suffixes)
{
	return fileDB->fileNames(suffixes);
}

QStringList XWPathSea::allFileNames(PathSeaParams * params)
{
	QStringList ses = getSuffixes(params);
	if (ses.isEmpty())
		return ses;

	return allFileNames(ses);
}

QStringList XWPathSea::allPathSearch(const QString & path,
	                                 const QString & name)
{
	return cache->search(path, name, true, true);
}

QStringList XWPathSea::allPathSearchList(const QString & path,
                                         const QStringList & names)
{
	return cache->searchList(path, names, true, true);
}

QString XWPathSea::braceExpand(const QString & path)
{
	QString tmppath = varExpand(path);
	QChar * xpath = (QChar*)(tmppath.data());
	QChar * elt = 0;
	QString ret;
	for (elt = pathElement(xpath); elt; elt = pathElement(0))
	{
		QString tmp(elt);
		QString expansion = braceExpandElement(tmp);
		ret += expansion;
		ret += envSep();
	}

	ret = expandDot(ret.data());
	return ret;
}

QStringList XWPathSea::braceExpand(const QChar ** text)
{
	QStringList result;
	QStringList partial;
	const QChar * p;
	for (p = *text; !p->isNull() && p[0] != QChar('}'); ++p)
	{
		if (isEnvSep(p[0]) || p[0] == QChar(','))
		{
			expandAppend(partial, *text, p);
			result << partial;
			*text = p + 1;
			partial.clear();
		}
		else if (p[0] == QChar('{'))
		{
			expandAppend(partial, *text, p);
            ++p;
            QStringList recurse = braceExpand(&p);
            concatElements(partial, recurse);
            *text = p + 1;
		}
		else if (p[0] == QChar('$'))
		{
			if (p[1] == QChar('{'))
			{
                for (p += 2; p[0] != QChar('}'); ++p);
            }
		}
	}

	expandAppend(partial, *text, p);
	result << partial;
    *text = p;
    return result;
}

QString XWPathSea::braceExpandElement(const QString & elt)
{
	const QChar * e = elt.data();
	QStringList expansions = braceExpand(&e);
	QString ret;
	for (int i = 0; i < expansions.size(); i++)
	{
		QString tmp = expansions.at(i);
		QString x = expand(tmp);
		if (x != tmp)
			x = braceExpandElement(x);

		ret += x;
		ret += envSep();
	}

	return ret;
}

void XWPathSea::concatElements(QStringList & target,
                               const QStringList & more)
{
	if (more.size() == 0)
        return;

    if (target.size() == 0)
    {
        target << more;
        return;
    }

    QStringList newTarget;
    for (int j = 0; j < more.size(); j++)
    {
        for (int i = 0; i < target.size(); i++)
        {
            QString tmp = target.at(i);
            tmp += more.at(j);
            newTarget << tmp;
        }
    }

    target = newTarget;
}

QString XWPathSea::createDefaultTopDir()
{
	int idx = configDir.indexOf(QChar('/'));
	QString top = configDir;
	if (idx != -1)
		top = configDir.left(idx + 1);

	top += seaType;
	QDir d(top);
	if (!d.exists())
		d.mkpath(top);

	return top;
}

void XWPathSea::createDefaultDir(PathSeaParams * params,
	                             const QString & topdir)
{
	if (!params || !params->type || !params->subdir || !params->envs)
		return ;

	QString top = topdir;
	if (top.isEmpty())
		top = createDefaultTopDir();

	if (top.isEmpty())
		return ;

	if (isDevSep(top[1]))
		top[0] = top[0].toLower();

	QString subdir = QString::fromAscii(params->subdir);
	QString tmpdir = QString("%1/%2").arg(top).arg(subdir);
	QDir d(tmpdir);
	if (!d.exists())
		d.mkpath(tmpdir);

	QString envvar = QString::fromAscii(params->envs);
	QStringList envs = envvar.split(";", QString::SkipEmptyParts);
	if (envs.isEmpty())
		return ;

	envvar = envs[0];

	QString newdirs = QString("%1//").arg(tmpdir);
	QStringList dirs = getConfigDir(params);
	for (int i = 0; i < dirs.size(); i++)
	{
		QString tmp = dirs[i];
		if (tmp == newdirs)
			return ;

		int idx = tmp.indexOf("//");
		if (idx > 0)
		{
			QString d = tmp.left(idx);
			if (newdirs.contains(d), Qt::CaseInsensitive)
				return ;
		}
		else
		{
			idx = tmp.indexOf("/{");
			if (idx > 0)
			{
				idx += 2;
				idx = tmp.indexOf(subdir, idx, Qt::CaseInsensitive);
				if (idx > 0)
					return ;
			}
		}
	}

	if (!dirs.isEmpty())
	{
		QChar c = envSep();
		QString sep(envSep());
		QString ds = dirs.join(sep);
		newdirs.append(c);
		newdirs += ds;
	}

	setDirs(envvar, newdirs);
}

QString XWPathSea::expand(const QString & s)
{
	if (!cache)
		return QString();

	QString var_expansion = varExpand(s);
  	QString tilde_expansion = tildeExpand(var_expansion);

  	return tilde_expansion;
}

void XWPathSea::expandAppend(QStringList & partial,
                             const QChar * text,
                             const QChar * p)
{
	int len = p - text;
	QString newstr(text, len);
	QStringList tmp;
    tmp << newstr;
    concatElements(partial, tmp);
}

void XWPathSea::expandDefault(FormatInfo * info,
                              const QString & tryPath,
                              const QString & source)
{
	if (!tryPath.isEmpty())
	{
		info->rawPath = tryPath;
        info->path = expandDefault(tryPath, info->path);
        info->pathSource = source;
	}
}

QString XWPathSea::expandDefault(const QString & path,
                                 const QString & fallback)
{
	QString expansion;
	if (path.isEmpty())
		expansion = fallback;
	else if (isEnvSep(path[0]))
	{
		if (path[1].isNull())
			expansion = fallback;
		else
			expansion = QString("%1%2").arg(fallback).arg(path);
	}
	else if (isEnvSep(path[path.length() - 1]))
		expansion = QString("%1%2").arg(path).arg(fallback);
	else
	{
		const QChar * loc = path.data();
		const QChar * xpath = loc;
		while (!loc->isNull())
		{
			if (isEnvSep(loc[0]) && isEnvSep(loc[1]))
          		break;

          	if (!loc->isNull())
          	{
          		expansion = QString(xpath, loc - xpath + 1);
          		expansion += fallback;
          		expansion += QString(loc + 1);
          	}
          	else
          		expansion = path;

          	loc++;
		}
	}

	return expansion;
}

QString XWPathSea::expandDot(QChar * path)
{
	QString kpse_dot = getEnv("KPSE_DOT");
	QString ret;
	if (kpse_dot.isEmpty())
	{
		ret = QString(path);
		return ret;
	}

	QChar * elt = 0;
	for (elt = pathElement(path); elt; elt = pathElement(0))
	{
		if (isAbsolutePath(elt, false) ||
			(elt[0] == QChar('!') &&
			elt[1] == QChar('!')))
		{
			ret += QString(elt);
		}
		else if (elt[0] == QChar('.') && elt[1].isNull())
		{
			ret += kpse_dot;
		}
		else if (elt[0] == QChar('.') && isDirSep(elt[1]))
		{
			ret += kpse_dot;
			ret += QString(elt + 1);
		}
		else
		{
			ret += kpse_dot;
			ret += "/";
			ret += QString(elt);
		}

		ret += envSep();
	}

	return ret;
}

QChar * XWPathSea::fileNameComponent(const QChar * p)
{
	if (!cache)
		return 0;

	return element(p, false);
}

QStringList XWPathSea::findAll(int format,
	                            const QString & name)
{
	if (format >= formatSize)
		return QStringList();

	FormatInfo * f = &formatInfo[format];
	if (f->path.isEmpty())
		initFormat(format);

	return allPathSearch(f->path, name);
}

QStringList XWPathSea::findAll(int format,
                               const QStringList & names)
{
	if (format >= formatSize)
		return QStringList();

	FormatInfo * f = &formatInfo[format];
	if (f->path.isEmpty())
		initFormat(format);

	return allPathSearchList(f->path, names);
}

QString XWPathSea::findFile(const QString & name)
{
	int tmpfmt = findFormat(name);
	if (tmpfmt == -1)
	{
		XWFileName fn(name);		
		if (!fn.isLocal())
			return xwApp->getFile(name);
		
		if (fn.isAbsolute(false) && fn.isReadable())
			return name;
			
		QString ret;
		QString envv = getEnv("CUR_DOC_PATH");
		if (!envv.isEmpty())
		{
			ret = QString("%1/%2").arg(envv).arg(name);
			QFileInfo fileinfo(ret);
			if (fileinfo.isFile() && fileinfo.isReadable())
				return ret;
				
			if ( seaType == "picture" || seaType == "media")
			{
				if (seaType == "picture")
					ret = QString("%1/images/%2").arg(envv).arg(name);
				else 
					ret = QString("%1/media/%2").arg(envv).arg(name);
				
				fileinfo.setFile(ret);
				if (fileinfo.isFile() && fileinfo.isReadable())
					return ret;
			}
		}
		
		return QString();
	}

	return findFile(name, tmpfmt, false);
}

QString XWPathSea::findFile(const QString & name,
	                        int format,
                            bool must_exist)
{
	if (format < 0 || format >= formatSize)
		return QString();

	FormatInfo * f = &formatInfo[format];
	if (f->path.isEmpty())
		initFormat(format);

	return findFile(f, name, must_exist);
}

QString XWPathSea::findFile(FormatInfo * f,
	                        const QString & name,
	                        bool must_exist)
{
	XWFileName fn(name);
	QString ret;
	if (!fn.isLocal())
		ret = xwApp->getFile(name);
	else if (fn.isAbsolute(false) && fn.isReadable())
		ret = name;
	else
	{
		QString envv = getEnv("CUR_DOC_PATH");
		if (!envv.isEmpty())
		{
			ret = QString("%1/%2").arg(envv).arg(name);
			QFileInfo fileinfo(ret);
			if (fileinfo.isFile() && fileinfo.isReadable())
				return ret;
				
			if ( seaType == "picture" || seaType == "media")
			{
				if (seaType == "picture")
					ret = QString("%1/images/%2").arg(envv).arg(name);
				else 
					ret = QString("%1/media/%2").arg(envv).arg(name);
				
				fileinfo.setFile(ret);
				if (fileinfo.isFile() && fileinfo.isReadable())
					return ret;
			}
		}
		
		QString e = fn.fileName();
		bool name_has_suffix_already = false;
		if (!f->suffix.isEmpty())
		{
			QStringList tmplist = f->suffix;
			for (int i = 0; i < tmplist.size(); i++)
			{
				QString tmp = tmplist.at(i);
				name_has_suffix_already = name.endsWith(tmp, Qt::CaseInsensitive);
				if (name_has_suffix_already)
					break;
			}
		}

		if (!name_has_suffix_already && !f->altSuffix.isEmpty())
		{
			QStringList tmplist = f->altSuffix;
			for (int i = 0; i < tmplist.size(); i++)
			{
				QString tmp = tmplist.at(i);
				name_has_suffix_already = name.endsWith(tmp, Qt::CaseInsensitive);
				if (name_has_suffix_already)
					break;
			}
		}

		QStringList target;
		if (!name_has_suffix_already && !f->suffix.isEmpty())
		{
			QStringList tmplist = f->suffix;
			for (int i = 0; i < tmplist.size(); i++)
			{
				QString tmp = QString("%1%2").arg(name).arg(tmplist.at(i));
				target << tmp;
			}
		}

		if (name_has_suffix_already || !f->suffixSearchOnly)
			target << name;

		ret = pathSearchList(f->path, target, false);
		if (ret.isEmpty() && must_exist)
		{
			target.clear();
			if (!name_has_suffix_already && f->suffixSearchOnly)
			{
				QStringList tmplist = f->suffix;
				for (int i = 0; i < tmplist.size(); i++)
				{
					QString tmp = QString("%1%2").arg(name).arg(tmplist.at(i));
					target << tmp;
				}
			}

			if (name_has_suffix_already || !f->suffixSearchOnly)
				target << name;

			ret = pathSearchList(f->path, target, true);
		}
	}
	
	if (!ret.isEmpty() && (seaType == "doc" || seaType == "help"))
	{
		QFileInfo fileinfo(ret);
		QString curpath = fileinfo.path();
		putEnv("CUR_DOC_PATH", curpath);
	}
	
    return ret;
}

int XWPathSea::findFormat(const QString & filename)
{
	int ret = 0;
	bool found = false;
	while (!found && ret < formatSize)
	{
		FormatInfo * f = &formatInfo[ret];
		if (f->path.isEmpty())
			initFormat(ret);

		if (f->suffix.isEmpty() && f->altSuffix.isEmpty())
		{
			ret++;
			continue;
		}

		if (!f->suffix.isEmpty())
		{
			for (int i = 0; i < f->suffix.size(); i++)
			{
				if (filename.endsWith(f->suffix.at(i), Qt::CaseInsensitive))
				{
					found = true;
					break;
				}
			}
		}

		if (found)
			break;

		if (!f->altSuffix.isEmpty())
		{
			for (int i = 0; i < f->altSuffix.size(); i++)
			{
				if (filename.endsWith(f->altSuffix.at(i), Qt::CaseInsensitive))
				{
					found = true;
					break;
				}
			}
		}

		if (found)
			break;

		ret++;
	}

	if (found)
		return ret;

	return -1;
}

QStringList XWPathSea::getConfigDir(PathSeaParams * params)
{
	QChar c = envSep();
	QString s = QString::fromAscii(params->subdir);
  QStringList tmplist = topDir.split(c, QString::SkipEmptyParts);
  QStringList ret;
  if (tmplist.size() > 0)
  {
  	QString tmp;
    for (int i = 0; i < tmplist.size(); i++)
    {
    	QString var = tmplist[i];
    	if (isVarStart(var[0]))
    		tmp = varExpand(var);
    	else
    		tmp = var;
    				
    	tmp = getAbsoluteDir(configDir, tmp);
    	tmp = QString("%1/%2").arg(tmp).arg(s);
    	ret << tmp;
    }
  }
  else
  {
  	QString tmp = QCoreApplication::applicationDirPath();
  	int i = tmp.lastIndexOf(QChar('/'));
  	if (i > 0)
  		tmp = tmp.left(i);
  	tmp = QString("%1/%2").arg(tmp).arg(s);
  	ret << tmp;
  }
  	
	return ret;
}

QStringList XWPathSea::getSuffixes(PathSeaParams * params)
{
	QStringList ret;
	if (!params)
		return ret;

	if (params->suffixes)
	{
		QString tmp = QString::fromAscii(params->suffixes);
		ret = tmp.split(";", QString::SkipEmptyParts);
	}

	if (params->altsuffixes)
	{
		QString tmp = QString::fromAscii(params->altsuffixes);
		QStringList tmplist = tmp.split(";", QString::SkipEmptyParts);
		ret << tmplist;
	}

	return ret;
}

QStringList XWPathSea::getTopDirs()
{
	QStringList ret;
	QHash<QString, QString>::const_iterator i = topDirHash.constBegin();
	while (i != topDirHash.constEnd())
	{
		ret << i.key();
		++i;
	}
	
	return ret;
}

QString XWPathSea::initFormat(int)
{
	return QString();
}

void XWPathSea::initFormatByParams(int fmt,
                                  PathSeaParams * params,
                                  const QString & extrapath)
{
	if (fmt < 0 || fmt >= formatSize)
		return ;

	FormatInfo * f = &formatInfo[fmt];
	if (params->type)
		f->type = QString::fromAscii(params->type);

	if (params->maketex)
	{
		QString mktex = QString::fromAscii(params->maketex);
		QStringList args;
		if (params->args)
		{
			QString tmp = QString::fromAscii(params->args);
			args = tmp.split(";", QString::SkipEmptyParts);
		}

		initMakeTex(fmt, mktex, args);
	}

	QString path;
	if (!extrapath.isEmpty())
	{
		QStringList tmplist = extrapath.split(envSep(), QString::SkipEmptyParts);
		for (int i = 0; i < tmplist.size(); i++)
		{
			QString tmp = tmplist.at(i);
			if (isDevSep(tmp[1]))
				tmp[0] = tmp[0].toLower();
				
			if (!path.isEmpty())
				path.append(envSep());
			path += tmp;
				
			int p = tmp.length() - 1;
			if (tmp[p] == QChar('/'))
				tmp.remove(p, 1);
			p = tmp.length() - 1;
			if (tmp[p] == QChar('/'))
				tmp.remove(p, 1);
			
			fileDB->addDir(tmp);
		}
	}

	QStringList envs;
	if (params->envs)
	{
		QString tmp = QString::fromAscii(params->envs);
		envs = tmp.split(";", QString::SkipEmptyParts);
		for (int i = 0; i < envs.size(); i++)
		{
			tmp = envs.at(i);
			tmp = varValue(tmp);
			if (tmp.isEmpty())
				continue;
				
			tmp = getAbsoluteDir(configDir, tmp);
			if (!path.isEmpty())
				path.append(envSep());
				
			path += tmp;
		}
	}
	initPath(f, path, envs);
	if (params->suffixes)
	{
		QString tmp = QString::fromAscii(params->suffixes);
		f->suffix = tmp.split(";", QString::SkipEmptyParts);
	}

	if (params->altsuffixes)
	{
		QString tmp = QString::fromAscii(params->altsuffixes);
		f->altSuffix = tmp.split(";", QString::SkipEmptyParts);
	}
	f->suffixSearchOnly = params->suffixonly;
    f->binMode = params->binmode;
}

void XWPathSea::initMakeTex(int fmt,
                            const QString & dflt_prog,
                            const QStringList & args)
{
	if (fmt < 0 || fmt >= formatSize)
		return ;

	FormatInfo * f = &formatInfo[fmt];
	QString prog = f->program;
	if (prog.isEmpty())
		prog = dflt_prog;

	QString PROG = prog.toUpper();
	QString progval = varValue(PROG);
	f->program = prog;
	f->argv = args;
	if (!progval.isEmpty())
		setProgramEnabled(fmt, progval[0] == QChar('1'), ClientCnf);
}

void XWPathSea::initPath(FormatInfo * info,
                         const QString & defaultPath,
                         const QStringList & envList)
{
	info->defaultPath = defaultPath;
	QString programname = qApp->applicationName();
	QString var;
    for (int i = 0; i < envList.size(); i++)
    {
    	QString envname = envList.at(i);
    	if (var.isEmpty())
    	{
    		QString evar = QString("%1.%2").arg(envname).arg(programname);
    		QString env_value = getEnv(evar);
    		if (!env_value.isEmpty())
    			var = env_value;
    		else
    		{
    			evar = QString("%1_%2").arg(envname).arg(programname);
    			env_value = getEnv(evar);
    			if (!env_value.isEmpty())
    				var = env_value;
    			else
    			{
    				env_value = getEnv(envname);
    				if (!env_value.isEmpty())
    					var = env_value;
    			}
    		}
    	}

    	if (!var.isEmpty())
    		break;
    }

    info->path = info->rawPath = info->defaultPath;
    info->pathSource = "compile-time XWPathSea.h";
    expandDefault(info, info->clientPath, "program config file");
    if (!var.isEmpty())
    {
        QString env_value = getEnv(var);
        expandDefault(info, env_value, " environment variable");
    }

    expandDefault(info, info->overridePath, "application override variable");
    info->path = braceExpand(info->path);
}

void XWPathSea::insertFile(const QString & passedfname)
{
	if (passedfname.isEmpty())
		return ;

	fileDB->insert(passedfname);
}

QFile * XWPathSea::openFile(const QString & name)
{
	QFile * ret = 0;
	int tmpfmt = findFormat(name);
	if (tmpfmt == -1)
	{
		XWFileName fn(name);
		if (fn.isAbsolute() && fn.isReadable())
		{
			ret = new QFile(name);
			ret->open(QIODevice::ReadOnly);
		}
		
		return ret;
	}

	return openFile(tmpfmt, name);
}

QFile * XWPathSea::openFile(int fmt,
	                        const QString & name)
{
	QFile * ret = 0;
	if (name.isEmpty())
		return ret;
		
	if (fmt < 0 || fmt >= formatSize)
	{
		XWFileName fn(name);
		if (fn.isAbsolute() && fn.isReadable())
		{
			ret = new QFile(name);
			ret->open(QIODevice::ReadOnly);
		}
		
		return ret;
	}
		
	FormatInfo * f = &formatInfo[fmt];
	if (f->path.isEmpty())
			initFormat(fmt);
			
	QString fullname = findFile(f, name, false);
    if (fullname.isEmpty())
        return 0;

    ret = new QFile(fullname);
    if (!ret)
        return 0;

    bool b = false;
    if (f->binMode)
        b = ret->open(QIODevice::ReadOnly);
    else
        b = ret->open(QIODevice::ReadOnly | QIODevice::Text);

    if (!b)
    {
        delete ret;
        ret = 0;
    }

    return ret;
}

QChar * XWPathSea::pathElement(const QChar * p)
{
	return element(p, true);
}

QString XWPathSea::pathExpand(const QString & path)
{
	QString xpath = braceExpand(path);
	return cache->pathExpand(xpath);
}

QString XWPathSea::pathSearch(const QString & path,
                              const QString & name,
                     		  bool must_exist)
{
	QStringList tmplist = cache->search(path, name, must_exist, false);
	if (tmplist.isEmpty())
		return QString();

	return tmplist[0];
}

QString XWPathSea::pathSearchList(const QString & path,
                                  const QStringList & names,
                     		      bool must_exist)
{
	QStringList tmplist = cache->searchList(path, names, must_exist, false);
	if (tmplist.isEmpty())
		return QString();

	return tmplist[0];
}

void XWPathSea::removeConfig(const QString & key,
	                         const QString & prog)
{
	if (key.isEmpty())
		return ;

	cnf->remove(key, prog);
	cnfDirty = true;
}

void XWPathSea::save()
{
	if (cnfDirty)
		cnf->save(configDir);
}

void XWPathSea::setDirs(const QString & envvar,
   	                    const QString & dirs)
{	
	if (envvar.isEmpty())
		return ;

	cnfDirty = true;
	if (dirs.isEmpty())
	{
		removeConfig(envvar);
		return ;
	}

	QChar c = envSep();
	QString newdirs;
	QStringList tmplist = dirs.split(c, QString::SkipEmptyParts);
	tmplist.sort();
	QString tmptop;
	for (int i = 0; i < tmplist.size(); i++)
	{
		QString tmp = tmplist[i];
		if (isDevSep(tmp[1]))
			tmp[0] = tmp[0].toLower();
			
		bool newtop = true;
		if (!topDirHash.isEmpty())
		{
			if (topDirHash.contains(tmp))
			{
				newtop = false;
				tmp = topDirHash[tmp];
			}
			else
			{
				int idx = 0;
				int len = tmp.length();
				while (idx < len)
				{
					if (isDirSep(tmp[idx]))
					{
						QString top = tmp.left(idx);
						if (topDirHash.contains(top))
						{
							QString var = topDirHash[top];
							tmp.replace(0, top.length(), var);
							newtop = false;
							break;
						}
					}

					idx++;
				}
			}
		}

		if (newtop)
		{
			QString top = tmp;
			int idx = top.indexOf("//");
			if (idx < 0)
				idx = top.indexOf("/{");
			if (idx > 0)
				top = top.left(idx);
				
			if (idx > 0)
			{
				int count = top.count(QChar('/'));
				idx = top.lastIndexOf("/");
				if (count > 1 && idx > 0)
					top = top.left(idx);
			}
			QString var = QString("%1%2").arg(topDirEnv).arg(++maxTopDir);
			QString retop = getRelativeDir(configDir, top);
			addConfig(var, retop);
			putEnv(var, top);
			var.insert(0, "$");
			topDirHash[top] = var;
			if (!tmptop.isEmpty())
				tmptop += c;

			tmptop += var;
			tmp.replace(0, top.length(), var);
		}

		if (!newdirs.contains(tmp))
		{
			if (!newdirs.isEmpty())
				newdirs += c;
			newdirs += tmp;
		}
	}

	if (!tmptop.isEmpty())
	{
		if (tmptop[tmptop.length() - 1] == c)
			tmptop.remove(tmptop.length() - 1, 1);
			
		if (!topDir.isEmpty())
			topDir.append(c);
			
		topDir += tmptop;

		addConfig(topDirEnv, topDir);
	}

	if (!newdirs.isEmpty())
	{
		if (newdirs[newdirs.length() - 1] == c)
			newdirs.remove(newdirs.length() - 1, 1);

		addConfig(envvar, newdirs);
	}
	else
		removeConfig(envvar);
}

void XWPathSea::setProgramEnabled(int fmt,
                                  bool value,
                                  XWPathSea::SrcType level)
{
	if (fmt < 0 || fmt >= formatSize)
		return ;

	FormatInfo * f = &formatInfo[fmt];
	if (level >= f->programEnableLevel)
	{
		f->programEnabled = value;
    	f->programEnableLevel = level;
	}
}

QString XWPathSea::tildeExpand(const QString & name)
{
	QString expansion;
	if (name.isEmpty())
		return expansion;
    
    if (name[0] != QChar('~'))
        expansion = name;
    else
    {
    	QString home = QDir::homePath();
    	if (home.isEmpty())
    		home = QDir::currentPath();

		if (isDirSep(home[0]) && isDirSep(home[1]))
    		home.remove(0, 1);
    			
    	QString tmpname = name;
    	if (isDirSep(name[1]))
    		tmpname.remove(0, 2);
    	else
    		tmpname.remove(0, 1);
    		
    	if (isDirSep(home[home.length() - 1]))    		
    		expansion = QString("%1%2").arg(home).arg(tmpname);
    	else
    		expansion = QString("%1/%2").arg(home).arg(tmpname);
    }

    return expansion;
}

void XWPathSea::updateDBs()
{
	QHash<QString, QString>::iterator i = topDirHash.begin();
	while (i != topDirHash.end())
	{
		QString top = i.key();
		QDir d(top);
		if (d.exists())
			fileDB->update(top);

		i++;
	}
}

QString XWPathSea::varExpand(const QString & s)
{
	return variable->varExpand(s);
}

QString XWPathSea::varValue(const QString & s)
{
	return variable->varValue(s);
}

QChar * XWPathSea::element(const QChar * passed_path,  bool env_p)
{
	if (passed_path)
    	pathSeaPath = passed_path;
    else if (!pathSeaPath)
    	return 0;

    const QChar * p = pathSeaPath;
    int brace_level = 0;
    while (!(p[0].isNull())  &&
           !(brace_level == 0 &&
           (env_p ? isEnvSep(p[0]) : isDirSep(p[0]))))
    {
    	if (p[0] == QChar('{'))
    		++brace_level;
    	else if (p[0] == QChar('}'))
    		--brace_level;
    	++p;
  	}

  	int len = p - pathSeaPath;
  	if (len + 1 > pathSeaEltAlloc)
    {
      	pathSeaEltAlloc = len + 1;
      	pathSeaElt = (QChar *)realloc(pathSeaElt, pathSeaEltAlloc * sizeof(QChar));
    }

    for (int i = 0; i < len; i++)
    	pathSeaElt[i] = pathSeaPath[i];

    pathSeaElt[len] = QChar('\0');
  	QChar * ret = pathSeaElt;

  	if (pathSeaPath[len].isNull())
    	pathSeaPath = 0;
  	else
    	pathSeaPath += len + 1;

  	return ret;
}
