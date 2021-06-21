/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#include <ctype.h>
#include <string.h>
#include <QMutex>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QMultiHash>
#include <QProcess>
#include <QTextCodec>

#include "XWConst.h"
#include "XWUtil.h"
#include "XWStringUtil.h"
#include "XWFileName.h"
#include "XWPathSea.h"
#include "XWFontSea.h"

#define KPSE_BITMAP_TOLERANCE(r) ((r) / 500.0 + 1)

static QMutex fontSeaMutex;

#ifdef Q_OS_WIN
static char * systemFontDirs[] = {
	"C:/WINDOWS/Fonts",
	"C:/WINNT/Fonts",
	NULL
};
#elif defined(Q_OS_MAC)
static char * systemFontDirs[] = {
	"~/Library/Fonts",
	"/Library/Fonts",
	"/Network/Library/Fonts",
	"/System/Library/Fonts",
	"/System Folder/Fonts",
	NULL
};
#else
static char * systemFontDirs[] = {
	"/usr/share/fonts",
	"/usr/local/fonts",
	"/usr/lib/X11/fonts",
	"/usr/openwin/lib/X11/fonts",
	"/usr/X11R6/lib/X11/fonts",
	"/usr/share/ghostscript/fonts",
	"/usr/local/share/ghostscript/fonts",
	NULL
};

#endif

static PathSeaParams fontSeaParams[] =
{
    {"bitmap font", "mktexpk",
     "--mfmode;$MAKETEX_MODE;--bdpi;$MAKETEX_BASE_DPI;--mag;$MAKETEX_MAG;--dpi;$KPATHSEA_DPI",
     0, "GLYPHFONTS;TEXFONTS", 0, 0, true, true},
    {"afm", 0, 0, "afm", "AFMFONTS;TEXFONTS", ".afm", 0, true, false},
    {"bdf", 0, 0, "bdf", 0, ".bdf", 0, true, true},
    {"cff", 0, 0, "cff", 0, ".cff", 0, true, true},
    {"cmap files", 0, 0, "cmap", "CMAPFONTS;TEXFONTS", ".cmap", 0, true, false},
    {"enc files", 0, 0, "enc", "ENCFONTS;TEXFONTS", ".enc", 0, true, false},
    {"gf", 0, 0, "gf", "GFFONTS;GLYPHFONTS;TEXFONTS", "gf", 0, true, true},
    {"glyphlist", 0, 0, "glyphlist", "GLYPHLIST", ".txt", 0, true, false},
    {"lig files", 0, 0, "lig", "LIGFONTS;TEXFONTS", ".lig", 0, true, false},
    {"map", 0, 0, "map", "TEXFONTMAPS;TEXFONTS", ".map", 0, true, false},
    {"mf", "mktexmf", 0, "mf", "MFINPUTS", ".mf", 0, true, false},
    {"misc fonts", 0, 0, "misc", "MISCFONTS;TEXFONTS", 0, 0, false, true},    
    {"ofm", "mkofm", 0, "ofm", "OFMFONTS;TEXFONTS", ".ofm", ".tfm", true, true},
    {"opentype fonts", 0, 0, "opentype", "OPENTYPEFONTS;TEXFONTS", ".otf;.ttf;.ttc;", 0, true, true},
    {"opl", 0, 0, "opl", "OPLFONTS;TEXFONTS", ".opl", ".pl", true, false},
    {"otl", 0, 0, "otl", "OPLFONTS;TEXFONTS", ".opl", ".otl", true, false},
    {"ovf", 0, 0, "ovf", "OVFFONTS;TEXFONTS", ".ovf", 0, true, true},
    {"ovp", 0, 0, "ovp", "OVPFONTS;TEXFONTS", ".ovp", 0, true, false},
    {"pcf", 0, 0, "pcf", 0, ".pcf", 0, true, true},
    {"pfm", 0, 0, "pfm", 0, ".pfm", 0, true, true},
    {"pfr", 0, 0, "pfr", 0, ".pfr", 0, true, true},
    {"pk", "mktexpk",
     "--mfmode;$MAKETEX_MODE;--bdpi;$MAKETEX_BASE_DPI;--mag;$MAKETEX_MAG;--dpi;$KPATHSEA_DPI",
     "pk", "PKFONTS;TEXPKS;GLYPHFONTS;TEXFONTS", ".pk", 0, true, true},
    {"pl", 0, 0, "pl", "PLFONTS;TEXFONTS", ".pl", ".opl", true, false},
    {"rpl", 0, 0, "rpl", "TTF2TFMINPUTS;TEXPSHEADERS;DVIPSHEADERS", ".rpl", 0, true, false},
    {"sfd", 0, 0, "sfd", "SFDFONTS;TEXFONTS", ".sfd", 0, true, false},
    {"tfm", "mktextfm", 0, "tfm", "TFMFONTS;TEXFONTS", ".tfm", ".ofm", true, true},
    {"Troff fonts", 0, 0, "troff", "TRFONTS", 0, 0, false, true},
    {"truetype fonts", 0, 0, "truetype", "TTFONTS;TEXFONTS", ".ttf;.ttc", 0, true, true},
    {"type0 fonts", 0, 0, "type0", 0, 0, 0, false, true},
    {"type1 fonts", 0, 0, "type1", "T1FONTS;T1INPUTS;TEXFONTS;TEXPSHEADERS;PSHEADERS", ".pfa;.pfb;.ps", 0, true, true},
    {"type2 fonts", 0, 0, "type2", 0, 0, 0, false, true},
    {"type3 fonts", 0, 0, "type3", 0, 0, 0, false, true},
    {"type42 fonts", 0, 0, "type42", "T42FONTS;TEXFONTS", 0, 0, false, true},
    {"vf", 0, 0, "vf", "VFFONTS;TEXFONTS", ".vf", 0, true, true},
    {"vpl", 0, 0, "vpl", 0, ".vpl", 0, true, true},
    {"windows fonts", 0, 0, "windows", 0, ".fnt;.fon", 0, true, true}
};

static int magStep(int n,  int bdpi)
{
    double t;
    int step = 0;
    int neg = 0;

    if (n < 0)
    {
        neg = 1;
        n = -n;
    }

    if (n & 1)
    {
        n &= ~1;
        t = 1.095445115;
    }
    else
        t = 1.0;

    while (n > 8)
    {
        n -= 8;
        t = t * 2.0736;
    }

    while (n > 0)
    {
        n -= 2;
        t = t * 1.2;
    }

    step = (int)(0.5 + (neg ? bdpi / t : bdpi * t));
    return step;
}

#undef ABS /* be safe */
#define ABS(expr) ((expr) < 0 ? -(expr) : (expr))

#define MAGSTEP_MAX 40

static int magStepFix(int dpi,  int bdpi,  int * m_ret)
{
    int m = 0;
    int mdpi = -1;
    int real_dpi = 0;
    int sign = dpi < bdpi ? -1 : 1;
    for (m = 0; !real_dpi && m < MAGSTEP_MAX; m++)
    {
        mdpi = magStep (m * sign, bdpi);
        if (ABS(mdpi - dpi) <= 1)
            real_dpi = mdpi;
        else if ((mdpi - dpi) * sign > 0)
            real_dpi = dpi;
    }

    if (m_ret)
        *m_ret = real_dpi == (mdpi ? (m - 1) * sign : 0);

    return real_dpi ? real_dpi : dpi;
}

XWFontSeaPrivate::XWFontSeaPrivate(QObject * parent)
    :XWPathSea("fonts", (int)(XWFontSea::Last), "fonts.cnf", "fonts.lsr", parent),
     resolutions(0),
     mapPos(-1),
     makeTexDiscardErrors(false)
{

}

XWFontSeaPrivate::~XWFontSeaPrivate()
{
    if (resolutions)
        free(resolutions);
}

QString XWFontSeaPrivate::findFile(const QString & name,
                                   int format,
                                   bool must_exist)
{
    bool use_fontmaps = (format == XWFontSea::TFM ||
                         format == XWFontSea::GF ||
                         format == XWFontSea::PK ||
                         format == XWFontSea::OFM);

    FormatInfo * f = &formatInfo[format];
    if (f->path.isEmpty())
        initFormat(format);

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
            if (use_fontmaps)
            {
                QStringList mapped_names = fontMapLookup(tmp);
                if (!mapped_names.isEmpty())
                    target << mapped_names;
            }
        }
    }

    if (name_has_suffix_already || !f->suffixSearchOnly)
    {
        target << name;
        if (use_fontmaps)
        {
            QStringList mapped_names = fontMapLookup(name);
            if (!mapped_names.isEmpty())
                target << mapped_names;
        }
    }

    QString ret = pathSearchList(f->path, target, false);
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

    if (ret.isEmpty() && must_exist)
        ret = makeTex(format, name);

    return ret;
}

QString XWFontSeaPrivate::findGlyph(const QString & passed_fontname,
                                    int dpi,
                                    XWFontSea::FileFormat format,
                                    GlyphFileType * glyph_file)
{
	QString fontname = passed_fontname;
	if (!passed_fontname.isEmpty())
    	putEnv("KPATHSEA_NAME", passed_fontname);
    QString ret = tryResolution(passed_fontname, dpi, format, glyph_file);
    GlyphSourceType source = Normal;
    if (ret.isEmpty())
    {
        source = Alias;
        ret = tryFontMap(fontname, dpi, format, glyph_file);
        if (ret.isEmpty())
        {
            XWFileName fn(fontname);
            if (!fn.isAbsolute(true))
            {
                source = MakeTex;
                putEnv("KPATHSEA_DPI", dpi);
                ret = makeTex(format, fontname);
            }
        }

        if (!ret.isEmpty() && glyph_file)
        {
            glyph_file->dpi = dpi;
            glyph_file->name = fontname;
        }
        else 
        {
        	if (resolutions)
            	ret = tryFallbackResolutions(fontname, dpi, format, glyph_file);
            if (!ret.isEmpty() && !font.isEmpty())
            {
                source = FallBack;
                putEnv("KPATHSEA_NAME", font);
                ret = tryResolution(font, dpi, format, glyph_file);
                if (ret.isEmpty() && resolutions)
                    ret = tryFallbackResolutions(font, dpi, format, glyph_file);
            }
        }
    }

    if (glyph_file)
        glyph_file->source = source;

    return ret;
}

QStringList XWFontSeaPrivate::fontMapLookup(const QString & key)
{
    if (map.size() == 0)
        readAllMaps();

    QStringList ret;
    if (map.size() == 0)
        return ret;

    QMultiHash<QString, QString>::iterator i = map.find(key);
    while (i != map.end() && i.key() == key)
    {
        ret << i.value();
        i++;
    }

    XWFileName finfo(key);
    QString suffix = finfo.ext(false);
    if (ret.isEmpty())
    {
        if (!suffix.isEmpty())
        {
            QString base_key = finfo.baseName();
            i = map.find(base_key);
            while (i != map.end() && i.key() == base_key)
            {
                ret << i.value();
                i++;
            }
        }
    }
    
    if (!ret.isEmpty() && !suffix.isEmpty())
    {
    	QStringList tmplist;
    	for (int i = 0; i < ret.size(); i++)
    	{
    		if (ret.at(i).lastIndexOf(QChar('.')) == -1)
    		{
    			QString tmp = QString("%1.%2").arg(ret.at(i)).arg(suffix);
    			tmplist << tmp;
    		}
    		else
    			tmplist << ret.at(i);
    	}
    	
    	ret = tmplist;
    }

    return ret;
}

void XWFontSeaPrivate::initProg(const QString & prefix,
                                int dpi,
                                const QString & mode,
                                const QString & fallback)
{
    QString var = QString("%1MAKEPK").arg(prefix);
    QString makepk = getEnv(var);
    if (!makepk.isEmpty())
    {
        setProgramEnabled((int)(XWFontSea::PK), true, Environment);
        setProgramEnabled((int)(XWFontSea::Any), true, Environment);
        formatInfo[XWFontSea::PK].program = makepk;
        formatInfo[XWFontSea::Any].program = makepk;
    }
    var = QString("%1FONTS").arg(prefix);
    QString font_var = getEnv(var);
    formatInfo[XWFontSea::PK].overridePath = font_var;
    formatInfo[XWFontSea::GF].overridePath = font_var;
    formatInfo[XWFontSea::Any].overridePath = font_var;
    formatInfo[XWFontSea::TFM].overridePath = font_var;

    var = QString("%1SIZES").arg(prefix);
    QString size_var = getEnv(var);
    initFallbackResolutions(size_var);
    font = fallback;

    putEnv("MAKETEX_BASE_DPI", dpi);
    if (mode.isEmpty())
        putEnv("MAKETEX_MODE", "/");
    else
        putEnv("MAKETEX_MODE", mode);
}

QString XWFontSeaPrivate::initFormat(int format)
{
    FormatInfo * f = &formatInfo[format];
    if (!(f->path.isEmpty()))
        return f->path;
        
    bool isfont = (format != XWFontSea::CMAP && 
                   format != XWFontSea::ENC && 
                   format != XWFontSea::GlyphList && 
                   format != XWFontSea::LIG && 
                   format != XWFontSea::Map && 
                   format != XWFontSea::MF && 
                   format != XWFontSea::OFM && 
                   format != XWFontSea::OPL && 
                   format != XWFontSea::OVF && 
                   format != XWFontSea::OVP && 
                   format != XWFontSea::PL && 
                   format != XWFontSea::SFD && 
                   format != XWFontSea::TFM && 
                   format != XWFontSea::VF && 
                   format != XWFontSea::VPL);

    QString extrapath;
    if (isfont)
    {
    	for (int i = 0; systemFontDirs[i]; i++)
    	{
    		QString tmp = QString::fromAscii(systemFontDirs[i]);
    		QDir dir(tmp);
    		if (dir.exists())
    		{
    			tmp = dir.absolutePath();
    			tmp += "//";
    			if (i > 0)
    				extrapath.append(envSep());
    				
    			extrapath.append(tmp);
    		}
    	}
    }
    
#ifdef Q_OS_WIN
    QString adobe = "c:/Program Files/Adobe";
#else
	QString adobe = "/user/share/Adobe";
#endif

	if (format == XWFontSea::CMAP || isfont)
	{
    QDir d(adobe);
    if (d.exists())
    {
        QStringList ds = d.entryList(QDir::AllDirs);
        for (int i = 0; i < ds.size(); i++)
        {
            QString sd = ds.at(i);
            if (sd.startsWith("Acrobat", Qt::CaseInsensitive) ||
                sd.startsWith("Reader", Qt::CaseInsensitive))
            {
            	if (format == XWFontSea::CMAP)
            	{
                	QString ssd = QString("%1/%2/Resource/CMap//").arg(adobe).arg(sd);
                	if (!extrapath.isEmpty())
                		extrapath.append(envSep());
                	extrapath += ssd;
                }
                else if (isfont)
                {
                	QString ssd = QString("%1/%2/Resource/CIDFont//").arg(adobe).arg(sd);
                	if (!extrapath.isEmpty())
                		extrapath.append(envSep());
                	extrapath += ssd;
                	extrapath.append(envSep());
                	ssd = QString("%1/%2/Resource/Font//").arg(adobe).arg(sd);
                	extrapath += ssd;
                }
            }
        }
    }
  }
  
  initFormatByParams(format, &fontSeaParams[format], extrapath);

  return f->path;
}

QString XWFontSeaPrivate::makeTex(int format,
                                  const QString & base)
{
    FormatInfo spec = formatInfo[format];
    if (spec.type.isEmpty())
    {
        initFormat(format);
        spec = formatInfo[format];
    }

    QString ret;
    if (!spec.program.isEmpty() && spec.programEnabled)
    {
        if (base[0] == QChar('-'))
            return QString();

        for (int i = 0; i < base.length(); i++)
        {
            if (!(base[i].isLetterOrNumber()) &&
                base[i] != QChar('-') &&
                base[i] != QChar('+') &&
                base[i] != QChar('_') &&
                base[i] != QChar('.') &&
                !isDirSep(base[i]))
            {
                return QString();
            }
        }

        if (format == XWFontSea::GF ||
            format == XWFontSea::PK ||
            format == XWFontSea::Any)
        {
            setMakeTexMag();
        }

        QStringList args;
        for (int i = 0; i < spec.argv.size(); i++)
        {
            QString tmp = spec.argv.at(i);
            tmp = varExpand(tmp);
            if (!tmp.isEmpty())
            	args << tmp;
        }

        args << base;

        QProcess myprocess;
        myprocess.start(spec.program, args);
        if (!myprocess.waitForStarted())
            return QString();

        if (!myprocess.waitForFinished())
            return QString();

        QByteArray res = myprocess.readAll();
        QTextCodec * codec = QTextCodec::codecForLocale();
        ret = codec->toUnicode(res);
    }

    return ret;
}

void XWFontSeaPrivate::makeTexOption(const QString & fmtname,  bool value)
{
    XWFontSea::FileFormat fmt = XWFontSea::Last;
    if (0 == fmtname.compare("pk"), Qt::CaseInsensitive)
        fmt = XWFontSea::PK;
    else if (0 == fmtname.compare("tfm"), Qt::CaseInsensitive)
        fmt = XWFontSea::TFM;
    else if (0 == fmtname.compare("ofm"), Qt::CaseInsensitive)
        fmt = XWFontSea::OFM;

    if (fmt != XWFontSea::Last)
        setProgramEnabled(fmt, value, CmdLine);
}

QString XWFontSeaPrivate::tryFallbackResolutions(const QString & fontname,
                                                 int dpi,
                                                 XWFontSea::FileFormat format,
                                                 GlyphFileType * glyph_file)
{
    int s = 0;
    int closest_diff = CNST_INT_MAX;
    int loc = 0;
    for (s = 0; resolutions[s] != 0; s++)
    {
        int this_diff = qAbs(resolutions[s] - dpi);
        if (this_diff < closest_diff)
        {
            closest_diff = this_diff;
            loc = s;
        }
    }

    if (s == 0)
        return QString();

    int max_loc = s;
    int lower_loc = loc - 1;
    int upper_loc = loc + 1;
    QString ret;
    for (;;)
    {
        int fallback = resolutions[loc];
        if (fallback != dpi)
            ret = tryResolution(fontname, fallback, format, glyph_file);

        if (!ret.isEmpty())
            break;

        int lower_diff = lower_loc > -1 ? dpi - resolutions[lower_loc] : CNST_INT_MAX;
        int upper_diff = upper_loc < max_loc ? resolutions[upper_loc] - dpi : CNST_INT_MAX;
        if (lower_diff == CNST_INT_MAX && upper_diff == CNST_INT_MAX)
            break;

        if (lower_diff < upper_diff)
        {
            loc = lower_loc;
            lower_loc--;
        }
        else
        {
            loc = upper_loc;
            upper_loc++;
        }
    }

    return ret;
}

QString XWFontSeaPrivate::tryFontMap(QString & fontname,
                                     int dpi,
                                     XWFontSea::FileFormat format,
                                     GlyphFileType * glyph_file)
{
    QStringList mapped_names = fontMapLookup(fontname);
    QString ret;
    if (!mapped_names.isEmpty())
    {
    	QString first_name = mapped_names[0];
        for (int i = 0; i < mapped_names.size(); i++)
        {
            QString mapped_name = mapped_names.at(i);
            if (!mapped_name.isEmpty())
            	putEnv("KPATHSEA_NAME", mapped_name);
            ret = tryResolution(mapped_name, dpi, format, glyph_file);
            if (!ret.isEmpty())
            {
                mapPos = i;
                break;
            }
        }
        
        if (!ret.isEmpty())
        	fontname = ret;
        else
        {
        	mapped_names = fontMapLookup(first_name);
        	if (!mapped_names.isEmpty())
        		fontname = first_name;
        }
    }

    return ret;
}

QString XWFontSeaPrivate::tryFormat(XWFontSea::FileFormat format)
{
    QStringList bitmap_specs;
    bitmap_specs << "$KPATHSEA_NAME.$KPATHSEA_DPI$KPATHSEA_FORMAT"
                 << "dpi$KPATHSEA_DPI/$KPATHSEA_NAME.$KPATHSEA_FORMAT";

    QString path = formatInfo[format].path;
    if (path.isEmpty())
        path = initFormat(format);

    QStringList sfx = formatInfo[format].suffix;
    if (!sfx.isEmpty())
    	putEnv("KPATHSEA_FORMAT", sfx.at(0));
    	
    bool must_exist = false;
    QString ret;
    for (int i = 0; i < bitmap_specs.size(); i++)
    {
        QString spec = bitmap_specs.at(i);
        QString name = varExpand(spec);
        ret = pathSearch(path, name, must_exist);
        if (!ret.isEmpty())
            break;

        must_exist = true;
        ret = pathSearch(path, name, must_exist);
    }

    return ret;
}

QString XWFontSeaPrivate::tryResolution(const QString & fontname,
                                        int dpi,
                                        XWFontSea::FileFormat format,
                                        GlyphFileType * glyph_file)
{
    QString ret = trySize(fontname, dpi, format, glyph_file);
    if (ret.isEmpty())
    {
        int tolerance = KPSE_BITMAP_TOLERANCE(dpi);
        int lower_bound = (int) (dpi - tolerance) < 0 ? 0 : (int)(dpi - tolerance);
        int upper_bound = (int)(dpi + tolerance);

        for (int r = lower_bound; ret.isEmpty() && r <= upper_bound; r++)
        {
            if (r != dpi)
                ret = trySize(fontname, r, format, glyph_file);
        }
    }

    return ret;
}

QString XWFontSeaPrivate::trySize(const QString & fontname,
                                  int dpi,
                                  XWFontSea::FileFormat format,
                                  GlyphFileType * glyph_file)
{
    bool try_gf = format == XWFontSea::GF || format == XWFontSea::Any;
    bool try_pk = format == XWFontSea::PK || format == XWFontSea::Any;
    putEnv("KPATHSEA_DPI", dpi);
    QString ret;
    if (try_pk)
        ret = tryFormat(XWFontSea::PK);

    XWFontSea::FileFormat format_found = XWFontSea::Last;
    if (!ret.isEmpty())
        format_found = XWFontSea::PK;
    else
    {
        if (try_gf)
            ret = tryFormat(XWFontSea::GF);

        if (!ret.isEmpty())
            format_found = XWFontSea::GF;
    }

    if (glyph_file)
    {
        glyph_file->name = fontname;
        glyph_file->dpi = dpi;
        glyph_file->format = format_found;
    }

    return ret;
}

void XWFontSeaPrivate::initFallbackResolutions(const QString & envvar)
{
    QString size_var = envVar(envvar, "TEXSIZES");
    QString size_str = getEnv(size_var);
    QString default_sizes = resolutionsString;
    QString xsize_list = expandDefault(size_str, default_sizes);
    int *last_resort_sizes = 0;
    int size_count = 0;
    if (!xsize_list.isEmpty())
    {
        QChar * size_list = xsize_list.data();
        QChar * size = 0;
        for (size = pathElement(size_list); size; size = pathElement(0))
        {
            if (size->isNull())
                continue;

            QString tmp(size);
            int s = tmp.toInt();

            if (!size_count || s >= last_resort_sizes[size_count - 1])
            {
                size_count++;
                last_resort_sizes = (int*)realloc(last_resort_sizes, size_count * sizeof(int));
                last_resort_sizes[size_count - 1] = s;
            }
        }
    }

    size_count++;
    last_resort_sizes = (int*)realloc(last_resort_sizes, size_count * sizeof(int));
    last_resort_sizes[size_count - 1] = 0;
    resolutions = last_resort_sizes;
}

void XWFontSeaPrivate::parseMapFile(const QString & map_filename)
{
    QFile file(map_filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        int p = line.indexOf(QChar('%'));
        if (p == -1)
            p = line.indexOf("@c");

        if (p != -1)
            line = line.left(p);

        QChar * l = (QChar*)(line.data());
        while (!l->isNull() && l->isSpace())
            l++;

        QString filename = token(l);
        if (!filename.isEmpty())
        {
            QString alias = token (l + filename.length());
            if (0 == filename.compare("include", Qt::CaseInsensitive))
            {
                if (!alias.isEmpty())
                {
                    QString include_fname = pathSearch(mapPath, alias, false);
                    if (!include_fname.isEmpty())
                        parseMapFile(include_fname);
                }
            }
            else if (!alias.isEmpty())
                map.insert(alias, filename);
        }
    }

    file.close();
}

void XWFontSeaPrivate::readAllMaps()
{
    mapPath = initFormat(XWFontSea::Map);
    QStringList filenames = allPathSearch(mapPath, "texfonts.map");
    for (int i = 0; i < filenames.size(); i++)
        parseMapFile(filenames.at(i));
}

void XWFontSeaPrivate::setMakeTexMag()
{
    QString dpi_str = getEnv("KPATHSEA_DPI");
    QString bdpi_str = getEnv ("MAKETEX_BASE_DPI");
    int dpi = 0;
    if (!dpi_str.isEmpty())
        dpi = dpi_str.toInt();

    int bdpi = 0;
    if (!bdpi_str.isEmpty())
        bdpi = bdpi_str.toInt();

    int m = 0;
    (void)magStepFix(dpi, bdpi, &m);
    QString q;
    if (m == 0)
        q = QString("%1+%2/%3").arg(dpi / bdpi).arg(dpi % bdpi).arg(bdpi);
    else
    {
        if (m < 0)
        {
            m *= -1;
            q = QString("magstep\\(-%1.%2\\)").arg(m / 2).arg((m & 1) * 5);
        }
        else
            q = QString("magstep\\(%1.%2\\)").arg(m / 2).arg((m & 1) * 5);
    }

    putEnv("MAKETEX_MAG", q);
}

QString XWFontSeaPrivate::token(const QChar * str)
{
    while (!str->isNull() && str->isSpace())
        str++;

    const QChar * start = str;
    while (!str->isNull() && !str->isSpace())
        str++;

    int len = str - start;
    QString ret(start, len);
    return ret;
}

static XWFontSeaPrivate * fontSea = 0;

XWFontSea::XWFontSea(QObject * parent)
    :QObject(parent)
{
	if (!fontSea)
		fontSea = new XWFontSeaPrivate;
}

XWFontSea::XWFontSea(const QString & prefix,
                     int dpi,
                     const QString & mode,
                     const QString & fallback,
                     QObject * parent)
    :QObject(parent)
{

    fontSea->initProg(prefix, dpi, mode, fallback);
}

void XWFontSea::addConfig(const QString & key, 
                          const QString & value, 
                          const QString & prog)
{
	fontSea->addConfig(key, value, prog);
}

void XWFontSea::addFile(const QString & path)
{
	fontSea->addFile(path);
}

QStringList XWFontSea::allAfm()
{
	return allFileNames(AFM);
}

QStringList XWFontSea::allBdf()
{
	return allFileNames(BDF);
}

QStringList XWFontSea::allCff()
{
	return allFileNames(CFF);
}

QStringList XWFontSea::allCMap()
{
	return allFileNames(CMAP);
}

QStringList XWFontSea::allEnc()
{
	return allFileNames(ENC);
}

QStringList XWFontSea::allFileNames(XWFontSea::FileFormat format)
{
	return fontSea->allFileNames(&fontSeaParams[format]);
}

QStringList XWFontSea::allFreeType()
{
	QStringList ret;
	QStringList list = allTrueType();
	if (!list.isEmpty())
		ret << list;
		
	list = allFileNames(OpenType);
	if (!list.isEmpty())
		ret << list;
		
	list = allType1();
	if (!list.isEmpty())
		ret << list;
		
	list = allWin();
	if (!list.isEmpty())
		ret << list;
		
	list = allBdf();
	if (!list.isEmpty())
		ret << list;
		
	list = allCff();
	if (!list.isEmpty())
		ret << list;
		
	list = allPcf();
	if (!list.isEmpty())
		ret << list;
		
	list = allPfr();
	if (!list.isEmpty())
		ret << list;
		
	return ret;
}

QStringList XWFontSea::allGf()
{
	return allFileNames(GF);
}

QStringList XWFontSea::allGlyphList()
{
	return allFileNames(GlyphList);
}

QStringList XWFontSea::allLig()
{
	return allFileNames(LIG);
}

QStringList XWFontSea::allMap()
{
	return allFileNames(Map);
}

QStringList XWFontSea::allMf()
{
	return allFileNames(MF);
}

QStringList XWFontSea::allOfm()
{
	return allFileNames(OFM);
}

QStringList XWFontSea::allOpl()
{
	return allFileNames(OPL);
}

QStringList XWFontSea::allOvf()
{
	return allFileNames(OVF);
}

QStringList XWFontSea::allOvp()
{
	return allFileNames(OVP);
}

QStringList XWFontSea::allPcf()
{
	return allFileNames(PCF);
}

QStringList XWFontSea::allPfm()
{
	return allFileNames(PFM);
}

QStringList XWFontSea::allPfr()
{
	return allFileNames(PFR);
}

QStringList XWFontSea::allPk()
{
	return allFileNames(PK);
}

QStringList XWFontSea::allPl()
{
	return allFileNames(PL);
}

QStringList XWFontSea::allSfd()
{
	return allFileNames(SFD);
}

QStringList XWFontSea::allTfm()
{
	return allFileNames(TFM);
}

QStringList XWFontSea::allTrueType()
{
	return allFileNames(TrueType);
}

QStringList XWFontSea::allType1()
{
	return allFileNames(Type1);
}

QStringList XWFontSea::allVf()
{
	return allFileNames(VF);
}
	
QStringList XWFontSea::allVpl()
{
	return allFileNames(VPL);
}

QStringList XWFontSea::allWin()
{
	return allFileNames(WinFont);
}
	
bool XWFontSea::bitmapTolerance(double dpi1, double dpi2)
{
    int tolerance = KPSE_BITMAP_TOLERANCE (dpi2);
    int lower_bound = (int) (dpi2 - tolerance) < 0 ? 0 : dpi2 - tolerance;
    int upper_bound = dpi2 + tolerance;

    return lower_bound <= dpi1 && dpi1 <= upper_bound;
}

void XWFontSea::createAllDefaultDir(const QString & topdir)
{
	int format = AFM;
	while (format < Last)
	{
		createDefaultDir((FileFormat)format, topdir);
		format++;
	}
}

void XWFontSea::createDefaultDir(XWFontSea::FileFormat format,
	                           const QString & topdir)
{
	fontSea->createDefaultDir(&fontSeaParams[format], topdir);
}

QString XWFontSea::findAfm(const QString & name)
{
	return findFile(name, AFM, false);
}

QStringList XWFontSea::findAll(const QString & name,
	                           XWFontSea::FileFormat format)
{
	QStringList ret = fontSea->findAll(format, name);
	return ret;
}

QStringList XWFontSea::findAll(const QStringList & names,
	                           XWFontSea::FileFormat format)
{
	QStringList ret = fontSea->findAll(format, names);
	return ret;
}
	                    
QString XWFontSea::findBdf(const QString & name)
{
	return findFile(name, BDF, false);
}

QString XWFontSea::findCff(const QString & name)
{
	return findFile(name, CFF, false);
}

QString XWFontSea::findCMap(const QString & name)
{
    return findFile(name, CMAP, false);
}

QString XWFontSea::findEnc(const QString & name)
{
    return findFile(name, ENC, false);
}

QString XWFontSea::findFile(const QString & name)
{
	if (name.isEmpty())
	{
        return QString();
      }
  QString ret;
    int tmpfmt = fontSea->findFormat(name);
	if (tmpfmt != -1)
	{
		ret = fontSea->findFile(name, tmpfmt, false);
		return ret;
	}
      
    for (int i = AFM; i < Last; i++)
    {
    	ret = fontSea->findFile(name, (FileFormat)i, false);
    	if (!ret.isEmpty())
    		break;
    }
    return ret;
}

QString XWFontSea::findFile(const QString & name,
                            XWFontSea::FileFormat format,
                            bool must_exist)
{
    if (name.isEmpty())
    {
        return QString();
      }

    QString ret = fontSea->findFile(name, (int)format, must_exist);
    return ret;
}

XWFontSea::FileFormat XWFontSea::findFormat(const QString & name)
{
	int tmpfmt = fontSea->findFormat(name);
    if (tmpfmt == -1)
    {
        return Last;
      }

    XWFontSea::FileFormat format = (XWFontSea::FileFormat)tmpfmt;
    QFile * fp = 0;
    char buf[200];
    int r = 0;
    switch (format)
    {
    	case TrueType:
    		fp = openTrueType(name);
    		if (fp)
    		{
    			fp->read(buf, 4);
    			r = memcmp(buf, "true", 4);
    			if (r != 0)
    				r = memcmp(buf, "\0\1\0\0", 4);
    				
    			if (r != 0)
    				r = memcmp(buf, "ttcf", 4);
    				
    			if (r != 0)
    			{
    				r = memcmp(buf, "OTTO", 4);
    				if (r == 0)
    					format = OpenType;
    			}
    		}
    		break;
    		
    	case Type1:
    		fp = openType1(name);
    		if (fp)
    		{
    			fp->read(buf, 21);
    			char * p = buf;
    			r = memcmp(p + 6, "%!PS-AdobeFont", 14);
    			if (r != 0)
    				r = memcmp(p + 6, "%!FontType1", 11);
    				
    			if (r != 0)
    				r = memcmp(p + 6, "%!PS", 4);
    		}
    		break;
    		
    	case OpenType:
    		fp = openType1(name);
    		if (fp)
    		{
    			fp->read(buf, 4);
    			r = memcmp(buf, "OTTO", 4);
    			if (r != 0)
    			{
    				r = memcmp(buf, "true", 4);
    				if (r != 0)
    					r = memcmp(buf, "\0\1\0\0", 4);
    				
    				if (r != 0)
    					r = memcmp(buf, "ttcf", 4);
    					
    				if (r == 0)
    					format = TrueType;
    			}
    		}
    		break;
    		
    	case CMAP:
    		fp = openCMap(name);
    		if (fp)
    		{
    			fp->readLine(buf, 128);
    			char * p = buf;
    			r = memcmp(p, "%!PS", 4);
    			if (r == 0)
    			{
    				for (p += 4; *p && !isspace(*p); p++);
    				for ( ; *p && (*p == ' ' || *p == '\t'); p++);
    				
    				r = memcmp(p, "Resource-CMap", strlen("Resource-CMap"));
    			}
    		}
    	default:
    		break;
    }
    
    if (fp)
    {
    	fp->close();
    	delete fp;
    }
    
    if (r != 0)
    	format = Last;
    return format;
}

QString XWFontSea::findGf(const QString & fontname,
                          int dpi,
                          GlyphFileType * glyph_file)
{
    if (fontname.isEmpty())
    {
        return QString();
      }

    QString ret = fontSea->findGlyph(fontname, dpi, GF, glyph_file);
    return ret;
}

QString XWFontSea::findGlyph(const QString & fontname,
                             int dpi,
                             XWFontSea::FileFormat format,
                             GlyphFileType * glyph_file)
{
    if (fontname.isEmpty())
    {
        return QString();
      }

    QString ret = fontSea->findGlyph(fontname, dpi, format, glyph_file);
    return ret;
}

QString XWFontSea::findGlyphList(const QString & name)
{
    QString tmpname = name;
    if (tmpname.isEmpty())
        tmpname = fontSea->varValue("GLYPHLIST");

    if (tmpname.isEmpty())
        tmpname = "glyphlist.txt";

    return findFile(tmpname, GlyphList, false);
}

QString XWFontSea::findLig(const QString & name)
{
    return findFile(name, LIG, false);
}

QString XWFontSea::findMap(const QString & name)
{
    return findFile(name, Map, false);
}

QString XWFontSea::findMf(const QString & name)
{
	return findFile(name, MF, false);
}

QString XWFontSea::findMisc(const QString & name)
{
	return findFile(name, MiscFonts, false);
}

QString XWFontSea::findOfm(const QString & name)
{
	return findFile(name, OFM, false);
}

QString XWFontSea::findOpenType(const QString & name)
{
	return findFile(name, OpenType, false);
}

QString XWFontSea::findOpl(const QString & name)
{
	return findFile(name, OPL, false);
}

QString XWFontSea::findOtl(const QString & name)
{
	return findFile(name, OTL, false);
}

QString XWFontSea::findOvf(const QString & name)
{
	return findFile(name, OVF, false);
}

QString XWFontSea::findOvp(const QString & name)
{
	return findFile(name, OVP, false);
}

QString XWFontSea::findPcf(const QString & name)
{
	return findFile(name, PCF, false);
}

QString XWFontSea::findPfm(const QString & name)
{
	return findFile(name, PFM, false);
}

QString XWFontSea::findPfr(const QString & name)
{
	return findFile(name, PFR, false);
}

QString XWFontSea::findPk(const QString & fontname,
                          int dpi,
                          GlyphFileType * glyph_file)
{
    if (fontname.isEmpty())
    {
        return QString();
      }

    QString ret = fontSea->findGlyph(fontname, dpi, PK, glyph_file);
    return ret;
}

QString XWFontSea::findPl(const QString & name)
{
	return findFile(name, PL, false);
}

QString XWFontSea::findRPL(const QString & name)
{
	return findFile(name, RPL, false);
}

QString XWFontSea::findSfd(const QString & name)
{
    return findFile(name, SFD, false);
}

QString XWFontSea::findTfm(const QString & name)
{
	return findFile(name, TFM, false);
}

QString XWFontSea::findTroff(const QString & name)
{
	return findFile(name, TroffFont, false);
}

QString XWFontSea::findTrueType(const QString & name)
{
	return findFile(name, TrueType, false);
}

QString XWFontSea::findType0(const QString & name)
{
	return findFile(name, Type0, false);
}

QString XWFontSea::findType1(const QString & name)
{
	return findFile(name, Type1, false);
}

QString XWFontSea::findType2(const QString & name)
{
	return findFile(name, Type2, false);
}

QString XWFontSea::findType3(const QString & name)
{
	return findFile(name, Type3, false);
}

QString XWFontSea::findType42(const QString & name)
{
	return findFile(name, Type42, false);
}

QString XWFontSea::findVf(const QString & name)
{
	return findFile(name, VF, false);
}

QString XWFontSea::findVpl(const QString & name)
{
	return findFile(name, VPL, false);
}

QString XWFontSea::findWin(const QString & name)
{
	return findFile(name, WinFont, false);
}

QStringList XWFontSea::getConfigDir(FileFormat format)
{
	return fontSea->getConfigDir(&fontSeaParams[format]);
}

QStringList XWFontSea::getSuffixes(XWFontSea::FileFormat format)
{
	return fontSea->getSuffixes(&fontSeaParams[format]);
}

QStringList XWFontSea::getSystemDir()
{
	QStringList ret;
	for (int i = 0; systemFontDirs[i]; i++)
  {
  	QString tmp = QString::fromAscii(systemFontDirs[i]);
  	QDir dir(tmp);
  	if (dir.exists())
  	{
  		tmp = dir.absolutePath();
  		ret << tmp;
  	}
  }
  
  return ret;
}

QStringList XWFontSea::getSystemFiles()
{
	QStringList filters;
	filters << "*.ttf";
	filters << "*.ttc";
	filters << "*.pfa";
	filters << "*.pfb";
	filters << "*.ps";
	filters << "*.fnt";
//	filters << "*.fon";
	filters << "*.bdf";
	filters << "*.cff";
	filters << "*.pcf";
	filters << "*.pfr";
	filters << "*.pfm";
	
	QStringList ret;
	for (int i = 0; systemFontDirs[i]; i++)
  {
  	QString tmp = QString::fromAscii(systemFontDirs[i]);
  	QDir dir(tmp);
  	if (dir.exists())
  	{
  		QStringList files = dir.entryList(filters, QDir::Files);
  		QString path = dir.absolutePath();
  		for (int j = 0; j < files.size(); j++)
  		{
  			QString f = files[j];
  			QString tmp = QString("%1/%2").arg(path).arg(f);
  			ret << tmp;
  		}
  	}
  }
  
  return ret;
}

QStringList XWFontSea::getTopDirs()
{
	return fontSea->getTopDirs();
}

void XWFontSea::init()
{
	if (!fontSea)
		fontSea = new XWFontSeaPrivate;
}

void XWFontSea::initProg(const QString & prefix,
                         int dpi,
                         const QString & mode,
                         const QString & fallback)
{
	fontSea->initProg(prefix, dpi, mode, fallback);
}

void XWFontSea::insertFile(const QString & passedfname)
{
	fontSea->insertFile(passedfname);
}

void XWFontSea::makeTexOption(const QString & fmtname,  bool value)
{
    fontSea->makeTexOption(fmtname, value);
}

QFile * XWFontSea::openAfm(const QString & filename)
{
	return openFile(filename, AFM);
}

QFile * XWFontSea::openBdf(const QString & filename)
{
	return openFile(filename, BDF);
}

QFile * XWFontSea::openCff(const QString & filename)
{
	return openFile(filename, CFF);
}

QFile * XWFontSea::openCMap(const QString & filename)
{
    return openFile(filename, CMAP);
}

QFile * XWFontSea::openEnc(const QString & filename)
{
    return openFile(filename, ENC);
}

QFile * XWFontSea::openFile(const QString & filename)
{
	return fontSea->openFile(filename);
}

QFile * XWFontSea::openFile(const QString & filename,
                            XWFontSea::FileFormat format)
{
    QFile * ret = fontSea->openFile((int)format, filename);
  return ret;
}

QFile * XWFontSea::openGlyphList(const QString & name)
{
    QString fullname = findGlyphList(name);
    if (fullname.isEmpty())
        return 0;
        
    QFile * ret = new QFile(fullname);
    if (!ret)
        return 0;

    if (!ret->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        delete ret;
        ret = 0;
    }
    return ret;
}

QFile * XWFontSea::openGf(const QString & filename)
{
	return openFile(filename, GF);
}

QFile * XWFontSea::openLig(const QString & filename)
{
    return openFile(filename, LIG);
}

QFile * XWFontSea::openMap(const QString & filename)
{
    return openFile(filename, Map);
}

QFile * XWFontSea::openMf(const QString & filename)
{
	return openFile(filename, MF);
}

QFile * XWFontSea::openMisc(const QString & filename)
{
	return openFile(filename, MiscFonts);
}

QFile * XWFontSea::openOfm(const QString & filename)
{
	return openFile(filename, OFM);
}

QFile * XWFontSea::openOpenType(const QString & filename)
{
	return openFile(filename, OpenType);
}

QFile * XWFontSea::openOpl(const QString & filename)
{
	return openFile(filename, OPL);
}

QFile * XWFontSea::openOtl(const QString & filename)
{
	return openFile(filename, OTL);
}

QFile * XWFontSea::openOvf(const QString & filename)
{
	return openFile(filename, OVF);
}

QFile * XWFontSea::openOvp(const QString & filename)
{
	return openFile(filename, OVP);
}

QFile * XWFontSea::openPcf(const QString & filename)
{
	return openFile(filename, PCF);
}

QFile * XWFontSea::openPfm(const QString & filename)
{
	return openFile(filename, PFM);
}

QFile * XWFontSea::openPfr(const QString & filename)
{
	return openFile(filename, PFR);
}

QFile * XWFontSea::openPk(const QString & filename)
{
	return openFile(filename, PK);
}

QFile * XWFontSea::openPl(const QString & filename)
{
	return openFile(filename, PL);
}

QFile * XWFontSea::openRPL(const QString & filename)
{
	return openFile(filename, RPL);
}

QFile * XWFontSea::openSfd(const QString & filename)
{
    return openFile(filename, SFD);
}

QFile * XWFontSea::openTFM(const QString & filename)
{
	return openFile(filename, TFM);
}

QFile * XWFontSea::openTroff(const QString & filename)
{
	return openFile(filename, TroffFont);
}

QFile * XWFontSea::openTrueType(const QString & filename)
{
	return openFile(filename, TrueType);
}

QFile * XWFontSea::openType0(const QString & filename)
{
	return openFile(filename, Type0);
}

QFile * XWFontSea::openType1(const QString & filename)
{
	return openFile(filename, Type1);
}

QFile * XWFontSea::openType2(const QString & filename)
{
	return openFile(filename, Type2);
}

QFile * XWFontSea::openType3(const QString & filename)
{
	return openFile(filename, Type3);
}

QFile * XWFontSea::openType42(const QString & filename)
{
	return openFile(filename, Type42);
}

QFile * XWFontSea::openVf(const QString & filename)
{
	return openFile(filename, VF);
}

QFile * XWFontSea::openVpl(const QString & filename)
{
	return openFile(filename, VPL);
}

QFile * XWFontSea::openWin(const QString & filename)
{
	return openFile(filename, WinFont);
}

void XWFontSea::quit()
{
}

void XWFontSea::removeConfig(const QString & key)
{
	fontSea->removeConfig(key);
}

void XWFontSea::setDirs(FileFormat format, 
                        const QString & dirs)
{
	PathSeaParams * params = &fontSeaParams[format];
	QString tmp = QString::fromAscii(params->envs);
	QStringList envs = tmp.split(";", QString::SkipEmptyParts);
	if (envs.isEmpty())
		return ;
		
	tmp = envs[0];
	
	fontSea->setDirs(tmp, dirs);
}

void XWFontSea::setGlyphList(const QString & s)
{
	fontSea->addConfig("GLYPHLIST", s);
}

QString XWFontSea::varExpand(const QString & s)
{
    return fontSea->varExpand(s);
}

QString XWFontSea::varValue(const QString & s)
{
    return fontSea->varValue(s);
}

void XWFontSea::save()
{
	fontSea->save();
}

void XWFontSea::updateDBs()
{
	fontSea->updateDBs();
}
