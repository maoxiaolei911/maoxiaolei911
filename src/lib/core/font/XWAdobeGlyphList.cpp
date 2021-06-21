/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>

#include "XWFontSea.h"
#include "XWApplication.h"
#include "XWLexer.h"

#include "XWAdobeGlyphList.h"


#define UC_REPLACEMENT_CHAR 0x0000FFFDL

#define UC_SUR_SHIFT      10
#define UC_SUR_MASK       0x3FFUL
#define UC_SUR_LOW_START  0xDC00UL
#define UC_SUR_HIGH_START 0xD800UL


#define IS_PUA(u) (((u) >= 0x00E000L && (u) <= 0x00F8FFL) || \
  ((u) >= 0x0F0000L && (u) <= 0x0FFFFDL) || \
  ((u) >= 0x100000L && (u) <= 0x10FFFDL) \
)

static const char * const modifiers[] = {
  "acute", "breve", "caron", "cedilla", "circumflex",
  "dieresis", "dotaccent", "grave", "hungarumlaut",
  "macron", "ogonek", "ring", "tilde", "commaaccent", 
  "slash",

  /* The following entries are not accent nor something
   * but PS font may have those "small" version...
   */
  "ampersand", "exclam", "exclamdown",
  "question","questiondown",
  NULL
};

static int skip_capital (char **p, char *endptr)
{
    long slen = 0;
    long len = (long) (endptr - (*p));

    if (len >= 2 && 
        ((**p == 'A' && *(*p+1) == 'E') ||
        (**p == 'O' && *(*p+1) == 'E'))) 
    {
        *p  += 2;
        slen = 2;
    } 
    else if (len >= 3 &&
            **p     == 'E' &&
            *(*p+1) == 't' &&
            *(*p+2) == 'h') 
    {
        *p  += 3;
        slen = 3;
    } 
    else if (len >= 5 &&
            **p     == 'T' &&
            *(*p+1) == 'h' &&
            *(*p+2) == 'o' &&
            *(*p+3) == 'r' &&
            *(*p+4) == 'n') 
    {
        *p  += 5;
        slen = 5;
    } 
    else if (len >= 1 && **p >= 'A' && **p <= 'Z') 
    {
        *p  += 1;
        slen = 1;
    }

    return slen;
}

static int
skip_modifier (char **p, char *endptr)
{
    long slen = 0;
    long len = (long) (endptr - (*p));
    for (int i = 0; modifiers[i] != NULL; i++) 
    {
        if ((len >= (long)strlen(modifiers[i]) && !memcmp(*p, modifiers[i], len))) 
        {
            slen = strlen(modifiers[i]);
            *p  += slen;
            break;
        }
    }

    return slen;
}

static int is_smallcap (const char *glyphname)
{
    if (!glyphname)
        return 0;
    
    char * p   = (char *) glyphname;
    long len = strlen(glyphname);
    if (len < 6 || strcmp(p + len - 5, "small"))
        return 0;

    char * endptr = p + len - 5;
    len -= 5;
    long slen = skip_modifier(&p, endptr);
    if (slen == len)
        return 1;
    else if (slen > 0) 
        return 0;

    len -= skip_capital(&p, endptr);
    if (len == 0) 
        return 1;

    while (len > 0) 
    {
        slen = skip_modifier(&p, endptr);
        if (slen == 0)
            return 0;
        len -= slen;
    }

    return 1;
}

#define SUFFIX_LIST_MAX  16
#define AGL_VAR_SMCP_IDX 0
static struct {
  const char   *key;
  const char   *otl_tag;
  const char   *suffixes[SUFFIX_LIST_MAX];
} var_list[] = {
  {"small"       , "smcp", {"sc", NULL}},
  {"swash"       , "swsh", {NULL}},
  {"superior"    , "sups", {NULL}},
  {"inferior"    , "sinf", {NULL}},
  {"numerator"   , "numr", {NULL}},
  {"denominator" , "dnom", {NULL}},
  {"oldstyle"    , "onum", {NULL}},

  /* The following only used by TeX, there are no
   * corresponding OTL feat. tag.
   */
  {"display" , NULL, {NULL}},
  {"text"    , NULL, {NULL}},
  {"big"     , NULL, {NULL}},
  {"bigg"    , NULL, {NULL}},
  {"Big"     , NULL, {NULL}},
  {"Bigg"    , NULL, {NULL}},
  {NULL, NULL, {NULL}}
};

static int agl_guess_name(const char *glyphname)
{
    if (is_smallcap(glyphname))
        return AGL_VAR_SMCP_IDX;

    int len = strlen(glyphname);
    for (int i = 1; var_list[i].key != NULL; i++) 
    {
        if (len > (int)strlen(var_list[i].key) && 
            !strcmp(glyphname+len-strlen(var_list[i].key), var_list[i].key)) 
        {
            return i;
        }
    }

    return -1;
}

XWAdobeGlyphName::XWAdobeGlyphName(char *glyphname)
    :name(0),
     suffix(0),
     n_components(0),
     alternate(0),
     is_predef(0)
{
	if (!glyphname)
		return ;
		
    char * suffixA = strchr(glyphname, '.');
    if (suffixA)
    {
        int n = strlen(glyphname) - strlen(suffixA);
        if (suffixA[1] != '\0') 
        {
            suffix = new char[strlen(suffixA)];
            strcpy(suffix, suffixA + 1);
        }
        
        name = new char[n+1];
        memcpy(name, glyphname, n);
        name[n] = '\0';
    }
    else if (is_smallcap(glyphname))
    {
        int n = strlen(glyphname) - 5;
        suffix = new char[3];
        strcpy(suffix, "sc");
        name   = new char[n+1];
        for (int i = 0; i < n; i++) 
            name[i] = isupper(glyphname[i]) ? (glyphname[i] + 32) : glyphname[i];
                
        name[n] = '\0';
    }
    else
    {
        int var_idx = agl_guess_name(glyphname);
        int n = 0;
        if (var_idx < 0 || !var_list[var_idx].key) 
        {
            n = strlen(glyphname);
        } 
        else 
        {
            n = strlen(glyphname) - strlen(var_list[var_idx].key);
            if (var_list[var_idx].suffixes[0])
                suffix = qstrdup(var_list[var_idx].suffixes[0]);
            else 
                suffix = qstrdup(var_list[var_idx].key);
        }
        name    = new char[n+1];
        memcpy(name, glyphname, n);
        name[n] = '\0';
    }
}

XWAdobeGlyphName::~XWAdobeGlyphName()
{
    if (name)
        delete [] name;
        
    if (suffix)
        delete [] suffix;
}

char * XWAdobeGlyphName::chopSuffix(const char *glyphname, char **suffixA)
{
    char * nameA = 0; 
    const char * p = strchr(glyphname, '.');
    if (p) 
    {
        int len = strlen(glyphname) - strlen(p);
        if (len < 1) 
        {
            nameA = 0;
            *suffixA = new char[strlen(glyphname)];
            strcpy(*suffixA, glyphname+1);
        } 
        else 
        {
            p++;
            nameA = new char[len + 1];
            strncpy(nameA, glyphname, len);
            nameA[len] = '\0';
            if (p[0] == '\0') 
                *suffixA = 0;
            else 
            {
                *suffixA = new char[strlen(p) + 1];
                strcpy(*suffixA, p);
            }
        }
    } 
    else 
    {
        nameA = new char[strlen(glyphname) + 1];
        strcpy(nameA, glyphname);
        *suffixA = 0;
    }

    return nameA;
}

const char * XWAdobeGlyphName::suffixToOtltag(const char *suffixA)
{
    for (int i = 0; var_list[i].key; i++) 
    {
        for (int j = 0; var_list[i].suffixes[j]; j++) 
        {
            if (!strcmp(suffixA, var_list[i].suffixes[j]))
                return var_list[i].otl_tag; 
        }
        
        if (!strcmp(suffixA, var_list[i].key))
            return var_list[i].otl_tag;
            
        if (var_list[i].otl_tag && !strcmp(suffixA, var_list[i].otl_tag))
            return var_list[i].otl_tag;
    }
  
    return 0;
}

class XWAdobeGlyphNameEntry
{
public:
    XWAdobeGlyphNameEntry(char * keyA, 
                          int keylenA, 
                          XWAdobeGlyphName * aglA);
                          
    ~XWAdobeGlyphNameEntry();
    
public:
    char  *key;
    int    keylen;
    XWAdobeGlyphName * value;
    
    XWAdobeGlyphNameEntry * next;
};

XWAdobeGlyphNameEntry::XWAdobeGlyphNameEntry(char * keyA, 
                                             int keylenA, 
                                             XWAdobeGlyphName * aglA)
{
    if (keyA && keylenA > 0)
    {
        key = new char[keylenA + 1];
        memcpy(key, keyA, keylenA);
        keylen = keylenA;
    }
    else
    {
        key = 0;
        keylen = 0;
    }
    
    value = aglA;
    next = 0;
}

XWAdobeGlyphNameEntry::~XWAdobeGlyphNameEntry()
{
    XWAdobeGlyphName *aglA = value;
    while (aglA)
    {
        XWAdobeGlyphName * next = aglA->alternate;
        delete aglA;
        aglA = next;
    }
    
    if (key)
        delete [] key;
}

XWAdobeGlyphList::XWAdobeGlyphList(QObject * parent)
    :QObject(parent)
{
    table = (XWAdobeGlyphNameEntry**)malloc(AGL_TABLE_SIZE * sizeof(XWAdobeGlyphNameEntry*));
    for (int i = 0; i < AGL_TABLE_SIZE; i++)
        table[i] = 0;
        
    itor = 0;
    loadListFile("texglyphlist.txt", 0);
    loadListFile("pdfglyphlist.txt", 1);
    loadListFile("glyphlist.txt", 1);
}

XWAdobeGlyphList::~XWAdobeGlyphList()
{
    for (int i = 0; i < AGL_TABLE_SIZE; i++)
    {
        XWAdobeGlyphNameEntry * cur = table[i];
        while (cur)
        {
            XWAdobeGlyphNameEntry * tmp = cur->next;
            delete cur;
            cur = tmp;
        }
    }
    
    free(table);
    
    if (itor)
        delete itor;
}

void XWAdobeGlyphList::clearIterator()
{
  if (!itor)
    itor = new Iterator;
        
  itor->index = AGL_TABLE_SIZE;
  itor->curr  = 0;
}

char * XWAdobeGlyphList::getKey(int *keylen)
{
    if (!itor)
        setIterator();
        
    XWAdobeGlyphNameEntry * hent = itor->curr;
    if (itor && hent) 
    {
        *keylen = hent->keylen;
        return hent->key;
    } 
    
    *keylen = 0;
    return 0;
}

int XWAdobeGlyphList::getUnicodes(const char *glyphstr,
                                  long *unicodes, 
                                  int max_unicodes)
{
    int countA = 0;
    char * p      = (char *) glyphstr;
    char * endptr = strchr(p, '.');
    if (!endptr)
        endptr = p + strlen(p);

    while (p < endptr)
    {
        char * delim = strchr(p, '_');
        if (delim == p)
        {
          return -1;
        }
        else if (!delim || delim > endptr)
            delim = endptr;
            
        long sub_len = (long) (delim - p);
        char * name = new char[sub_len+1];
        memcpy(name, p, sub_len);
        name[sub_len] = '\0';
        
        if (nameIsUnicode(name)) 
        {
            p  = name;
            if (p[1] != 'n') 
            { /* uXXXXXXXX */
                if (countA >= max_unicodes) 
                {
                    delete [] name;
                    return -1;
                }
                p++;
                unicodes[count++] = xtol(p, strlen(p));
            } 
            else 
            {
                p += 3;
                while (*p != '\0') 
                {
                    if (countA >= max_unicodes) 
                    {
                        delete [] name;
                        return -1;
                    }
                    unicodes[count++] = xtol(p, 4);
                    p += 4;
                }
            }
        }
        else
        {
            XWAdobeGlyphName * agln1 = lookupList(name);
            if (!agln1 || (agln1->n_components == 1 && IS_PUA(agln1->unicodes[0]))) 
            {
                XWAdobeGlyphName agln0(name);
                agln1 = lookupList(agln0.name);
            }
            
            if (agln1) 
            {
                if (count + agln1->n_components > max_unicodes) 
                {
                    delete [] name;
                    return -1;
                }
                
                for (int i = 0; i < agln1->n_components; i++) 
                    unicodes[count++] = agln1->unicodes[i];
            }
            else
            {
                delete [] name;
                return -1;
            }
        }
        
        delete [] name;
        p = delim + 1;
    }
    return countA;
}

XWAdobeGlyphName * XWAdobeGlyphList::getVal()
{
    if (!itor)
        setIterator();
        
    XWAdobeGlyphNameEntry * hent = itor->curr;
    if (itor && hent) 
        return hent->value;
    
    return 0;
}

int XWAdobeGlyphList::loadListFile(const QString & filename, int is_predef)
{
    XWFontSea sea;
    QFile * fp = sea.openGlyphList(filename);
    if (!fp)
    {
      return 0;
      }
        
    char  wbuf[1024];
    int   n_unicodes = 0;
    long  unicodes[AGL_MAX_UNICODES];
    while (fp->readLine(wbuf, 1024) > 0)
    {
    	if (wbuf[0] == '\n')
    		continue;
    		
    	char * r = strchr(wbuf, '\n');
    	if (r)
				*r = '\0';
        const char * endptr = wbuf + strlen(wbuf);
        XWLexer lexer(wbuf, endptr);
        const char * p = lexer.skipWhite();
        if (!p || p[0] == '#' || p >= endptr)
            continue;
            
        const char * nextptr = strchr(p, ';');
        if (!nextptr || nextptr == p)
            continue;
            
        char * name = lexer.getIdent(nextptr);
        p = lexer.skipWhite();
        
        if (!name || p[0] != ';') 
        {
            QString msg = QString(tr("invalid AGL entry: %1\n")).arg(wbuf);
            xwApp->warning(msg);
            if (name)
                delete [] name;
            continue;
        }
        
        p = lexer.skip(1);
        p = lexer.skipWhite();
        
        n_unicodes = 0;
        while (p < endptr && 
               ((p[0]  >= '0' && 
               p[0] <= '9') || 
                (p[0]  >= 'A' && p[0] <= 'F'))) 
        {

            if (n_unicodes >= AGL_MAX_UNICODES) 
            {
                xwApp->warning(tr("too many Unicode values.\n"));
                break;
            }
            unicodes[n_unicodes++] = strtol(p, (char**)(&nextptr), 16);

            lexer.setCurPtr(nextptr);
            p = lexer.skipWhite();
        }
        
        if (n_unicodes == 0) 
        {
            QString msg = QString(tr("AGL entry ignored (no mapping): %1\n")).arg(wbuf);
            xwApp->warning(msg);
            if (name)
                delete [] name;
            continue;
        }
        
        XWAdobeGlyphName * agln = new XWAdobeGlyphName(name);
        agln->is_predef = is_predef;
        agln->n_components = n_unicodes;
        for (int i = 0; i < n_unicodes; i++) 
            agln->unicodes[i] = unicodes[i];
            
        XWAdobeGlyphName * duplicate = lookupTable(name, strlen(name));
        if (!duplicate)
            appendTable(name, strlen(name), agln);
        else 
        {
            while (duplicate->alternate)
                duplicate = duplicate->alternate;
            duplicate->alternate = agln;
        }
        
        delete [] name;
        count++;
    }
    
    fp->close();
    delete fp;
    return count;
}

XWAdobeGlyphName * XWAdobeGlyphList::lookupList(const char *glyphname)
{
    return lookupTable(glyphname, strlen(glyphname));
}

long XWAdobeGlyphList::nameConvertUnicode(const char *glyphname)
{
    if (!nameIsUnicode(glyphname))
        return -1;
        
    if (strlen(glyphname) > 7 && *(glyphname+7) != '.') 
        return -1;
        
    char * p = (char *) (glyphname + 1);
    if (glyphname[1] == 'n')
        p = (char *) (glyphname + 3);
        
    long ucv = 0;
    while (*p != '\0' && *p != '.') 
    {
        if (!isdigit(*p) && (*p < 'A' || *p > 'F')) 
            return -1;
            
        ucv <<= 4;
        ucv += isdigit(*p) ? *p - '0' : *p - 'A' + 10;
        p++;
    }

    if (!UC_is_valid(ucv)) 
        ucv = -1;

    return ucv;
}

bool XWAdobeGlyphList::nameIsUnicode(const char *glyphname)
{
    if (!glyphname)
        return false;
    
    const char * suffix = strchr(glyphname, '.');
    int len = (int) (suffix ? suffix - glyphname : strlen(glyphname));
    if (len >= 7 && (len - 3) % 4 == 0 && !strncmp(glyphname, "uni", 3))
    {
        char c = glyphname[3];
        if (isdigit(c) || (c >= 'A' && c <= 'F'))
            return true;
        else
            return false;
    }
    else if (len <= 7 && len >= 5 && glyphname[0] == 'u')
    {
        for (int i = 1; i < len - 1; i++) 
        {
            char c = glyphname[i];
            if (!isdigit(c) && (c < 'A' || c > 'F'))
                return false;
        }
        return true;
    }
    
    return false;
}

bool XWAdobeGlyphList::next()
{
    XWAdobeGlyphNameEntry * hent = itor->curr;
    if (hent)
        hent = hent->next;
    while (!hent && ++itor->index < AGL_TABLE_SIZE) 
        hent = table[itor->index];
    
    itor->curr = hent;

    return (hent ? true : false);
}

bool XWAdobeGlyphList::setIterator()
{
    if (!itor)
        itor = new Iterator;
        
    for (int i = 0; i < AGL_TABLE_SIZE; i++) 
    {
        if (table[i]) 
        {
            itor->index = i;
            itor->curr  = table[i];
            return true;
        }
    }
    
    return false;
}

long XWAdobeGlyphList::sputUTF16BE (const char *glyphstr,
                                    uchar **dstpp, 
                                    uchar *limptr,
                                    int *fail_count)
{
    char * p      = (char *) glyphstr;
    char * endptr = strchr(p, '.');
    if (!endptr)
        endptr = p + strlen(p);
        
    long  len   = 0;
    int   countA = 0;
    while (p < endptr)
    {
        char * delim = strchr(p, '_');
        if (delim == p)
        {
            QString msg = QString(tr("invalid glyph name component in \"%1\".\n")).arg(glyphstr);
            xwApp->warning(msg);
            countA++;
            if (fail_count)
                *fail_count = countA;
            return len;
        }
        else if (!delim || delim > endptr)
            delim = endptr;
            
        long sub_len = (long) (delim - p);

        char * name = new char[sub_len+1];
        memcpy(name, p, sub_len);
        name[sub_len] = '\0';
        
        if (nameIsUnicode(name)) 
        {
            sub_len = putUnicodeGlyph(name, dstpp, limptr);
            if (sub_len > 0)
                len += sub_len;
            else 
                countA++;
        }
        else
        {
            XWAdobeGlyphName * agln1 = lookupList(name);
            if (!agln1 || (agln1->n_components == 1 &&
                IS_PUA(agln1->unicodes[0])))
            {
                XWAdobeGlyphName agln0(name);
                agln1 = lookupList(agln0.name);
            }
            
            if (agln1) 
            {
                for (int i = 0; i < agln1->n_components; i++) 
                    len += UC_sput_UTF16BE (agln1->unicodes[i], dstpp, limptr);
            }
            else
            {
                QString msg = QString(tr("no Unicode mapping for glyph name \"%1\" found.\n")).arg(name);
                xwApp->warning(msg);
                countA++;
            }
        }
        
        delete [] name;
        p = delim + 1;
    }
    
    if (fail_count)
        *fail_count = countA;
    return len;
}

int XWAdobeGlyphList::UC_sput_UTF16BE(long ucv, 
                                      uchar **pp, 
                                      uchar *limptr)
{
    int countA = 0;
    unsigned char *p = *pp;

    if (ucv >= 0 && ucv <= 0xFFFF) 
    {
    	if (p + 2 >= limptr)
    		return -1;
    		
        p[0] = (ucv >> 8) & 0xff;
        p[1] = ucv & 0xff;
        countA = 2;
    } 
    else if (ucv >= 0x010000L && ucv <= 0x10FFFFL) 
    {
    	if (p + 4 >= limptr)
    		return -1;
    		
        ucv  -= 0x00010000L;
        ushort high = (ucv >> UC_SUR_SHIFT) + UC_SUR_HIGH_START;
        ushort low  = (ucv &  UC_SUR_MASK)  + UC_SUR_LOW_START;
        p[0] = (high >> 8) & 0xff;
        p[1] = (high & 0xff);
        p[2] = (low >> 8) & 0xff;
        p[3] = (low & 0xff);
        countA = 4;
    } 
    else 
    {
    	if (p + 2 >= limptr)
    		return -1;
    		
        p[0] = (UC_REPLACEMENT_CHAR >> 8) & 0xff;
        p[1] = (UC_REPLACEMENT_CHAR & 0xff);
        countA = 2;
    }

    *pp += countA;
    return countA;
}

void XWAdobeGlyphList::appendTable(const void *key, int keylen, XWAdobeGlyphName *value)
{
    uint hkey = getHash(key, keylen);
    XWAdobeGlyphNameEntry * hent = table[hkey];
    if (!hent) 
    {
        hent = new XWAdobeGlyphNameEntry((char*)key, keylen, value);
        table[hkey] = hent;
    } 
    else 
    {
        XWAdobeGlyphNameEntry * last = 0;
        while (hent) 
        {
            last = hent;
            hent = hent->next;
        }
        
        hent = new XWAdobeGlyphNameEntry((char*)key, keylen, value);
        last->next = hent;
    }

    count++;
}

uint XWAdobeGlyphList::getHash(const void *key, int keylen)
{
    unsigned int hkey = 0;

    for (int i = 0; i < keylen; i++) 
        hkey = (hkey << 5) + hkey + ((char *)key)[i];

    return (hkey % AGL_TABLE_SIZE);
}

void XWAdobeGlyphList::insertTable(const void *key, int keylen, XWAdobeGlyphName *value)
{
    if (!key || keylen < 1)
        return ;
        
    uint hkey = getHash(key, keylen);
    XWAdobeGlyphNameEntry * hent = table[hkey];
    XWAdobeGlyphNameEntry * prev = 0;
    while (hent) 
    {
        if (hent->keylen == keylen && !memcmp(hent->key, key, keylen)) 
            break;
        prev = hent;
        hent = hent->next;
    }
    
    if (hent)
    {
        if (hent->value)
            delete hent->value;
        hent->value = value;
    }
    else
    {
        hent = new XWAdobeGlyphNameEntry((char*)key, keylen, value);
        if (prev) 
            prev->next = hent;
        else 
            table[hkey] = hent;
        count++;
    }
}

XWAdobeGlyphNameEntry * XWAdobeGlyphList::lookup(const void *key, int keylen)
{
    uint hkey = getHash(key, keylen);
    XWAdobeGlyphNameEntry * hent = table[hkey];
    while (hent) 
    {
        if (hent->keylen == keylen && !memcmp(hent->key, key, keylen)) 
            return hent;
            
        hent = hent->next;
    }

    return 0;
}

XWAdobeGlyphName * XWAdobeGlyphList::lookupTable(const void *key, int keylen)
{
    XWAdobeGlyphNameEntry * hent = lookup(key, keylen);
    if (hent)
        return hent->value;

    return 0;
}

long XWAdobeGlyphList::putUnicodeGlyph(const char *name,
                                       uchar **dstpp, 
                                       uchar *limptr)
{
    char * p   = (char *) name;
    long ucv = 0;
	long len = 0;
    if (p[1] != 'n') 
    {
        p   += 1;
        ucv  = xtol(p, strlen(p));
        len += UC_sput_UTF16BE (ucv, dstpp, limptr);
    } 
    else 
    {
        p += 3;
        while (*p != '\0') 
        {
            ucv  = xtol(p, 4);
            len += UC_sput_UTF16BE (ucv, dstpp, limptr);
            p   += 4;
        }
    }

    return len;
}

long XWAdobeGlyphList::xtol(const char *start, int len)
{
    long v = 0;
    while (len-- > 0) 
    {
        v <<= 4;
        if (isdigit(*start)) 
            v += *start - '0';
        else if (*start >= 'A' && *start <= 'F') 
            v += *start - 'A' + 10;
        else 
            return -1;
        start++;
    }

    return v;
}


