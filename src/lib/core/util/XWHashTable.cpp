/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWHashTable.h"

XWHashTable::XWHashTable(int size, QObject * parent)
    :QObject(parent),
     tableSize(size),
     count(0),
     table(0)
{
}

XWHashTable::~XWHashTable()
{
    clear();
}

bool XWHashTable::append(XWHashKey * key, XWHashValue * value)
{
    if (!key)
    {
        release(0, value);
        return false;
    }
    
    XWHashEntry * n = new XWHashEntry(key, value);
    if (!n)
    {
    	release(key, value);
        return false;
    }
    
    quint32 hkey = key->hash() % tableSize;
    XWHashEntry * en = table[hkey];
    if (!en)
        table[hkey] = n;
    else
    {
        XWHashEntry * last = 0;
        while (en)
        {
            last = en;
            en = en->getNext();
        }
        
        last->setNext(n);
    }
    
    return true;
}

bool XWHashTable::append(const QString & key, XWHashValue * value, HashType ht)
{        
    return append(key, ht, value);
}

bool XWHashTable::append(const QString & key, const QString & value, HashType ht)
{
    XWStringVaule * v = new XWStringVaule(value);
    if (!v)
        return false;
        
    return append(key, ht, v);
}

bool XWHashTable::append(const QString & key, int value, HashType ht)
{
    XWIntegerValue * v = new XWIntegerValue(value);
    if (!v)
        return false;
    
    return append(key, ht, v);
}

bool XWHashTable::append(const QString & key, const QStringList & value, HashType ht)
{        
    XWStringListValue * v = new XWStringListValue(value);
    if (!v)
        return false;
        
    return append(key, ht, v);
}

bool XWHashTable::append(const char * key, int len, XWHashValue * value)
{
    if (!key)
    {
        if (value->isShared())
            value->release();
        else
            delete value;
        return false;
    }
    
    XWBytesKey * k = new XWBytesKey(key, len);
    if (!k)
    {
        if (value->isShared())
            value->release();
        else
            delete value;
        return false;
    }
    
    XWHashEntry * n = new XWHashEntry(k, value);
    if (!n)
    {
        delete k;
        if (value->isShared())
            value->release();
        else
            delete value;
        return false;
    }
    
    quint32 hkey = hash(key, len);
    XWHashEntry * en = table[hkey];
    if (!en)
        table[hkey] = n;
    else
    {
        XWHashEntry * last = 0;
        while (en)
        {
            last = en;
            en = en->getNext();
        }
        
        last->setNext(n);
    }
    
    count++;
    
    return true;
}

bool XWHashTable::append(const char * key, int klen, const char * value, int vlen)
{
    XWBytesVaule * v = new XWBytesVaule(value, vlen);
    if (!v)
        return false;
        
    return append(key, klen, v);
}

bool XWHashTable::append(int key, XWHashValue * value)
{
    XWIntegerKey * k = new XWIntegerKey(key);
    if (!k)
    {
        if (value->isShared())
            value->release();
        else
            delete value;
        return false;
    }
        
    XWHashEntry * n = new XWHashEntry(k, value);
    if (!n)
    {
        delete k;
        if (value->isShared())
            value->release();
        else
            delete value;
        return false;
    }
    
    quint32 hkey = (quint32)(key % tableSize);
    XWHashEntry * en = table[hkey];
    if (!en)
        table[hkey] = n;
    else
    {
        XWHashEntry * last = 0;
        while (en)
        {
            last = en;
            en = en->getNext();
        }
        
        last->setNext(n);
    }
    count++;
    
    return true;
}

bool XWHashTable::append(int key, const QString & value)
{
    XWStringVaule * v = new XWStringVaule(value);
    if (!v)
        return false;
        
    return append(key, v);
}

bool XWHashTable::append(int key, const QStringList & value)
{
    XWStringListValue * v = new XWStringListValue(value);
    if (!v)
        return false;
        
    return append(key, v);
}

bool XWHashTable::append(int key, int value)
{
    XWIntegerValue * v = new XWIntegerValue(value);
    if (!v)
        return false;
    
    return append(key, v);
}

void XWHashTable::clear()
{
    if (!table)
        return ;
        
    for (int i = 0; i < tableSize; i++)
    {
        XWHashEntry * en = table[i];
        while (en)
        {
            XWHashEntry * tmp = en->getNext();
            delete en;
            en = tmp;
        }
    }
        
    free(table);
    table = 0;
    tableSize = 0;
    
    count = 0;
}

XWHashEntry * XWHashTable::getEntry(int n)
{
    if (hasEntry(n))
        return table[n];
        
    return 0;
}

quint32 XWHashTable::hash(const QString & key)
{
    quint32 hkey = 0;
    
    for (int i = 0; i < key.length(); i++)
        hkey = hkey + hkey + key[i].unicode();
        
    return hkey;
}

bool XWHashTable::hasEntry(int n)
{
    if ((n < 0) || (n >= tableSize))
        return false;
        
    return (0 != table[n]);
}

bool XWHashTable::init(int size)
{
    clear();
    table = (XWHashEntry **)malloc(size * sizeof(XWHashEntry *));
    if (!table)
        return false;
        
    tableSize = size;
    count = 0;
        
    for (int i = 0; i < tableSize; i++)
        table[i] = 0;
        
    return true;
}

bool XWHashTable::insert(XWHashKey * key, XWHashValue * value)
{
    if (!key)
        return false;
        
    quint32 hkey = key->hash() % tableSize;
    XWHashEntry * en = table[hkey];
    XWHashEntry * pre = 0;
    
    while (en)
    {
        XWHashKey * k = en->getKey();
        if (k && k->isEqual(key))
            break;
            
        pre = en;
        en = en->getNext();
    }
    
    if (en)
        en->setValue(value);
    else
    {
        en = new XWHashEntry(key, value);
        if (!en)
            return false;
            
        if (pre)
            pre->setNext(en);
        else
            table[hkey] = en;
            
        count++;
    }
    
    return true;
}

bool XWHashTable::insert(const QString & key, XWHashValue * value, HashType ht)
{
    return insert(key, ht, value);
}

bool XWHashTable::insert(const QString & key, const QString & value, HashType ht)
{
    XWStringVaule * v = new XWStringVaule(value);
    if (!v)
        return false;
    
    return insert(key, ht, v);
}

bool XWHashTable::insert(const QString & key, int value, HashType ht);
{
    XWIntegerValue * v = new XWIntegerValue(value);
    if (!v)
        return false;
        
    return insert(key, ht, v);
}

bool XWHashTable::insert(const QString & key, const QStringList & value, HashType ht)
{
    XWStringListValue * v = new XWStringListValue(value);
    if (!v)
        return false;
    
    return insert(key, ht, v);
}

bool XWHashTable::insert(const char * key, int klen, XWHashValue * value)
{
    if (!key)
    {
        if (value->isShared())
            value->release();
        else
            delete value;
        return false;
    }
    
    quint32 hkey = hash(key, len);
    XWHashEntry * pre = 0;
    XWHashEntry * en = table[hkey];
    while (en)
    {
        XWBytesKey * k = static_cast<XWBytesKey*>(en->getKey());
        if (k && !memcpy(key, (char*)(k->value), len))
            break;
            
        pre = en;
        en = en->getNext();
    }
    
    if (en)
        en->setValue(value);
    else
    {
        XWBytesKey * k = new XWBytesKey(key, len);
        if (!k)
        {
            if (value->isShared())
                value->release();
            else
                delete value;
            return false;
        }
            
        en = new XWHashEntry(k, value);
        if (!en)
        {
            delete k;
            if (value->isShared())
                value->release();
            else
                delete value;
            return false;
        }
            
        if (pre)
            pre->setNext(en);
        else
            table[hkey] = en;
            
        count++;
    }
    
    return true;
}

bool XWHashTable::insert(const char * key, int klen, const char * value, int vlen)
{
    XWBytesVaule * v = new XWBytesVaule(value, vlen);
    if (!v)
        return false;
    
    return insert(key, klen, v);
}

bool XWHashTable::insert(int key, XWHashValue * value)
{
    quint32 hkey = (quint32)(key % tableSize);
    XWHashEntry * pre = 0;
    XWHashEntry * en = table[hkey];
    while (en)
    {
        XWIntegerKey * k = static_cast<XWIntegerKey*>(en->getKey());
        if (k && k->isEqual(key))
            break;
          
        pre = en;
        en = en->getNext();
    }
    
    if (en)
        en->setValue(value);
    else
    {
        XWIntegerKey * k = new XWIntegerKey(key);
        if (!k)
        {
            if (value->isShared())
                value->release();
            else
                delete value;
            return false;
        }
            
        en = new XWHashEntry(k, value);
        if (!en)
        {
            delete k;
            if (value->isShared())
                value->release();
            else
                delete value;
            return false;
        }
            
        if (pre)
            pre->setNext(en);
        else
            table[hkey] = en;
            
        count++;
    }
    
    return true;
}

bool XWHashTable::insert(int key, const QString & value)
{
    XWStringVaule * v = new XWStringVaule(value);
    if (!v)
        return false;
    
    return insert(key, v);
}

bool XWHashTable::insert(int key, const QStringList & value)
{
    XWStringListValue * v = new XWStringListValue(value);
    if (!v)
        return false;
    
    return insert(key, v);
}

bool XWHashTable::insert(int key, int value)
{
    XWIntegerValue * v = new XWIntegerValue(value);
    if (!v)
        return false;
        
    return insert(key, v);
}

XWHashValue * XWHashTable::lookup(XWHashKey * key)
{
    if (!key)
        return 0;
        
    quint32 hkey = key->hash() % tableSize;
    XWHashEntry * en = table[hkey];
    while (en)
    {
        XWHashKey * k = en->getKey();
        if (k && k->isEqual(key))
            return en->getValue();
            
        en = en->getNext();
    }
    
    return 0;
}

XWHashValue * XWHashTable::lookup(const QString & key, HashType ht)
{
    if (key.isEmpty())
        return 0;
        
    quint32 hkey = hash(key, ht);
    XWHashEntry * en = table[hkey];
    while (en)
    {
        XWStringKey * k = static_cast<XWStringKey*>(en->getKey());
        if (k && k->isEqual(key))
            return en->getValue();
            
        en = en->getNext();
    }
    
    return 0;
}

XWHashValue * XWHashTable::lookup(const char * key, int klen)
{
    if (!key)
        return 0;
        
    quint32 hkey = hash(key, klen);
    XWHashEntry * en = table[hkey];
    while (en)
    {
        XWBytesKey * k = static_cast<XWBytesKey*>(en->getKey());
        if (k && !memcpy(key, (char*)(k->value), klen))
            return en->getValue();
            
        en = en->getNext();
    }
    
    return 0;
}

XWHashValue * XWHashTable::lookup(int key)
{
    quint32 hkey = (quint32)(key % tableSize);
    XWHashEntry * en = table[hkey];
    while (en)
    {
        XWIntegerKey * k = static_cast<XWIntegerKey*>(en->getKey());
        if (k && k->isEqual(key))
            return en->getValue();
            
        en = en->getNext();
    }
    
    return 0;
}

QStringList XWHashTable::lookupAll(const QString & key, HashType ht)
{
    if (key.isEmpty())
        return QStringList();
        
    quint32 hkey = hash(key, ht);
    XWHashEntry * en = table[hkey];
    QString ret;
    while (en)
    {
        XWStringKey * k = static_cast<XWStringKey*>(en->getKey());
        if (k && k->isEqual(key))
        {
            XWStringVaule * v = static_cast<XWStringVaule*>(en->getValue());
            if (v)
                ret << v->value;
        }
        
        en = en->getNext();
    }
    
    return ret;
}

quint32 XWHashTable::lowerHash(const QString & key)
{
    quint32 hkey = 0;
    for (int i = 0; i < key.size(); i++)
        hkey = hkey + hkey + key[i].toLower().unicode();
        
    return hkey;
}

void XWHashTable::remove(XWHashKey * key)
{
    if (!key)
        return ;
        
    quint32 hkey = key->hash() % tableSize;
    XWHashEntry * en = table[hkey];
    XWHashEntry * pre = 0;
    while (en)
    {
        XWHashKey * k = en->getKey();
        if (k && k->isEqual(key))
            break;
            
        pre = en;
        en = en->getNext();
    }
    
    if (en)
    {
        if (pre)
            pre->setNext(en->getNext());
        else
            table[hkey] = en->getNext();
                    
        delete en;
        count--;
    }
}

void XWHashTable::remove(const QString & key, HashType ht)
{
    if (key.isEmpty())
        return ;
        
    quint32 hkey = hash(key, ht);
    XWHashEntry * en = table[hkey];
    XWHashEntry * pre = 0;
    while (en)
    {
        XWStringKey * k = static_cast<XWStringKey*>(en->getKey());
        if (k && k->isEqual(key))
            break;
            
        pre = en;
        en = en->getNext();
    }
    
    if (en)
    {
        if (pre)
            pre->setNext(en->getNext());
        else
            table[hkey] = en->getNext();
            
        delete en;
        count--;
    }
}

void XWHashTable::remove(const char * key, int klen)
{
    if (!key)
        return ;
        
    quint32 hkey = hash(key, klen);
    XWHashEntry * en = table[hkey];
    XWHashEntry * pre = 0;
    while (en)
    {
        XWStringKey * k = static_cast<XWStringKey*>(en->getKey());
        if (k && !memcpy(key, (char*)(k->value), klen))
            break;
            
        pre = en;
        en = en->getNext();
    }
    
    if (en)
    {
        if (pre)
            pre->setNext(en->getNext());
        else
            table[hkey] = en->getNext();
            
        delete en;
        count--;
    }
}

void XWHashTable::remove(int key)
{        
    quint32 hkey = (quint32)(key % tableSize);
    XWHashEntry * en = table[hkey];
    XWHashEntry * pre = 0;
    while (en)
    {
        XWIntegerKey * k = static_cast<XWIntegerKey*>(en->getKey());
        if (k && k->isEqual(key))
            break;
            
        pre = en;
        en = en->getNext();
    }
    
    if (en)
    {
        if (pre)
            pre->setNext(en->getNext());
        else
            table[hkey] = en->getNext();
            
        delete en;
        count--;
    }
}

quint32 XWHashTable::upperHash(const QString & key)
{
    quint32 hkey = 0;
    for (int i = 0; i < key.size(); i++)
        hkey = hkey + hkey + key[i].toUpper().unicode();
        
    return n;
}

bool XWHashTable::append(const QString & key, HashType ht, XWHashValue * value)
{
    if (key.isEmpty())
    {
        if (value->isShared())
            value->release();
        else
            delete value;
        return false;
    }
    
    XWStringKey * k = new XWStringKey(key);
    if (!k)
    {
        if (value->isShared())
            value->release();
        else
            delete value;
        return false;
    }
        
    XWHashEntry * n = new XWHashEntry(k, value);
    if (!n)
    {
        delete k;
        if (value->isShared())
            value->release();
        else
            delete value;
        return false;
    }
    
    quint32 hkey = hash(key, ht);
    XWHashEntry * en = table[hkey];
    if (!en)
        table[hkey] = n;
    else
    {
        XWHashEntry * last = 0;
        while (en)
        {
            last = en;
            en = en->getNext();
        }
        
        last->setNext(n);
    }
    
    count++;
    
    return true;
}

quint32 XWHashTable::hash(const char * key, int len)
{
    quint32 hkey = 0;
    for (int i = 0; i < len; i++) 
        hkey = (hkey << 5) + hkey + key[i];

    return hkey;
}

quint32 XWHashTable::hash(const QString & key, HashType t)
{
    quint32 hkey = 0;
    switch (t)
    {
        case NormalHash:
            hkey = hash(key);
            break;
            
        case LowerHash:
            hkey = lowerHash(key);
            break;
            
        case UpperHash:
            hkey = upperHash(key);
            break;
    }
    
    return (hkey % tableSize);
}

bool XWHashTable::insert(const QString & key, HashType ht, XWHashValue * value)
{
    if (key.isEmpty())
    {
        if (value->isShared())
            value->release();
        else
            delete value;
        return false;
    }
        
    quint32 hkey = hash(key, ht);
    XWHashEntry * pre = 0;
    XWHashEntry * en = table[hkey];
    while (en)
    {
        XWStringKey * k = static_cast<XWStringKey*>(en->getKey());
        if (k && k->isEqual(key))
            break;
          
        pre = en;
        en = en->getNext();
    }
    
    if (en)
        en->setValue(value);
    else
    {
        XWStringKey * k = new XWStringKey(key);
        if (!k)
        {
            if (value->isShared())
                value->release();
            else
                delete value;
            return false;
        }
            
        en = new XWHashEntry(k, value);
        if (!en)
        {
            delete k;
            if (value->isShared())
                value->release();
            else
                delete value;
            return false;
        }
            
        if (pre)
            pre->setNext(en);
        else
            table[hkey] = en;
            
        count++;
    }
    
    return true;
}

void XWHashTable::release(XWHashKey * key, XWHashValue * value)
{
	if (key)
		delete key;
		
	if (value->isShared())
        value->release();
    else
       delete value;
}

XWHashTableIter::XWHashTableIter()
    :index(0),
     curr(0),
     hash(0)
{
}

void XWHashTableIter::clear()
{
    index = hash->getTableSize();
    curr = 0;
    hash = 0;
}

XWHashKey * XWHashTableIter::getKey()
{
    if (curr)
        return curr->getKey();
        
    return 0;
}

bool XWHashTableIter::getNext()
{
    XWHashEntry * hent = curr;
    hent = hent->getNext();
    
    while ((!hent) && (++index < hash->getTableSize()))
        hent = hash->getEntry(index);
        
    curr = hent;
    
    return (0 != hent);
}

XWHashValue * XWHashTableIter::getValue()
{
    if (curr)
        return curr->getValue();
        
    return 0;
}

bool XWHashTableIter::setHash(XWHashTable * _hash)
{
    if (!_hash)
        return false;
        
    for (int i = 0; i < _hash->getTableSize(); i++)
    {
        if (_hash->hasEntry(n))
        {
            index = i;
            curr = _hash->getEntry(i);
            hash  = _hash;
            
            return true;
        }
    }
    
    return false;
}

