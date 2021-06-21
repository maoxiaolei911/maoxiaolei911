/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stddef.h>
#include <QTextStream>
#include <zlib.h>
#include "XWObject.h"
#include "XWArray.h"
#include "XWDict.h"
#include "XWStream.h"
#include "XWRef.h"
#include "XWPSParser.h"

char *objTypeNames[NUM_OBJ_TYPES] = {
    "boolean",
    "integer",
    "real",
    "string",
    "name",
    "null",
    "array",
    "dictionary",
    "stream",
    "ref",
    "cmd",
    "error",
    "eof",
    "none"
};

int XWObject::nextLabel = 1;

XWObject * XWObject::copy(XWObject *obj)
{
	*obj = *this;
  	switch (type) 
  	{
  		case String:
    		obj->string = string->copy();
    		break;
    		
  		case Name:
    		obj->name = qstrdup(name);
    		break;
    		
  		case Array:
    		array->incRef();
    		break;
    		
  		case Dict:
    		dict->incRef();
    		break;
    		
  		case Stream:
    		stream->incRef();
    		break;
    		
  		case Cmd:
    		obj->cmd = qstrdup(cmd);
    		break;
    		
    	case Unknown:
    		obj->data = qstrdup(data);
    		break;
    		
  		default:
    		break;
  	}
  	return obj;
}

void * XWObject::dataPtr()
{
	void *p = 0;
	switch (type) 
	{
		case Bool:
			p = (void*)(&booln);
			break;
			
		case Int:
			p = (void*)(&intg);
			break;
			
		case Real:
			p = (void*)(&real);
			break;
			
		case Name:
			p = (void*)name;
			break;
			
		case String:
			p = (void*)(string->getCString());
			break;
			
		case Unknown:
			p = (void*)data;
			break;
			
		default:
			break;
	}
	
	return p;
}

XWObject * XWObject::dictKeys(XWObject * keys)
{
	int len = dict->getLength();
	XWObject obj;
	for (int i = 0; i < len; i++)
	{
		obj.initName(dict->getKey(i));
		keys->arrayAdd(&obj);
	}
	
	return keys;
}

XWObject * XWObject::dictLookupNFOrg(const char *key)
{
	if (!isDict())
		return 0;
		
	return dict->lookupNFOrg(key);
}

void  XWObject::dictMerge(XWObject *dict2)
{
	if (!dict2 || !isDict() || !dict2->isDict())
		return ;
		
	dict->merge(dict2->getDict());
}

XWObject * XWObject::fetch(XWRef *xref, XWObject *obj, int recursion)
{
	return (type == Ref && xref) ? xref->fetch(ref.num, ref.gen, obj, recursion) : copy(obj);
}

void XWObject::free()
{
	switch (type) 
	{
  		case String:
    		delete string;
    		break;
    		
  		case Name:
    		delete [] name;
    		break;
    		
  		case Array:
    		if (!array->decRef()) 
      			delete array;
    		break;
    		
  		case Dict:
    		if (!dict->decRef()) 
      			delete dict;
    		break;
    		
  		case Stream:
    		if (!stream->decRef()) 
    		{
    			if (flags == -1)
      				delete stream;
      			else
      			{
      				XWFileStream * filestr = (XWFileStream*)stream;
					QIODevice * f = filestr->getDev();
					f->close();
					delete f;
					delete stream;
      			}
      		}
    		break;
    		
  		case Cmd:
    		delete [] cmd;
    		break;
    		
    	case Unknown:
    		delete [] data;
    		break;
    		
  		default:
    		break;
  	}
  
  	type = None;
}

long  XWObject::getIV()
{
	long iv = 0;
	switch (type) 
	{
		case Bool:
			iv = booln;
			break;
			
		case Int:
			iv = intg;
			break;
			
		case Real:
			iv = (long)real;
			break;
			
		case String:
			{
				char * p = string->getCString();
				int len = string->getLength();
				
				XWPSParser parse(p, len);
				XWObject obj;
				parse.parseNumber(&obj);
				if (obj.isInt() || obj.isReal())
					iv = obj.getIV();
			}
			break;
			
		default:
			break;
	}
	
	return iv;
}


double XWObject::getRV()
{
	double rv = 0.0;
	switch (type) 
	{
		case Bool:
			rv = booln;
			break;
			
		case Int:
			rv = intg;
			break;
			
		case Real:
			rv = real;
			break;
			
		case String:
			{
				char * p = string->getCString();
				int len = string->getLength();
				
				XWPSParser parse(p, len);
				XWObject obj;
				parse.parseNumber(&obj);
				if (obj.isInt() || obj.isReal())
					rv = obj.getRV();
			}
			break;
			
		default:
			break;
	}
	
	return rv;
}

uchar * XWObject::getSV()
{
	char * sv = 0;
	char  fmt_buf[15];
	switch (type) 
	{
		case Bool:
			if (booln)
				sv = qstrdup("true");
			else
				sv = qstrdup("false");
			break;
			
		case Int:
			{
				sprintf(fmt_buf, "%ld", intg);
				sv = qstrdup(fmt_buf);
			}
			break;
			
		case Real:
			{
				sprintf(fmt_buf, "%.5g", intg);
				sv = qstrdup(fmt_buf);
			}
			break;
			
		case Name:
			sv = qstrdup(name);
			break;
			
		case String:
			{
				char * p = string->getCString();
				sv = qstrdup(p);
			}
			break;
			
		case Unknown:
			sv = qstrdup(data);
			break;
			
		default:
			break;
	}
	
	return (uchar*)sv;
}

char * XWObject::getTypeName() 
{
	return objTypeNames[type];
}

void XWObject::init(ObjectType t)
{
	type = t; 
	label = 0; 
	generation = 0;
	refcount = 1;
	flags = 0;
	
	switch (type) 
	{
  		case String:
    		string = 0;
    		break;
    		
  		case Name:
    		name = 0;
    		break;
    		
  		case Array:
    		array = 0;
    		break;
    		
  		case Dict:
  			dict = 0;
    		break;
    		
  		case Stream:
  			stream = 0;
    		break;
    		
  		case Cmd:
  			cmd = 0;
    		break;
    		
    	case Indirect:
    		pf = 0;
    		break;
    		
    	case Unknown:
    		data = 0;
    		break;
    		
  		default:
    		break;
  	}
}

XWObject * XWObject::initArray(XWRef *xref)
{
    init(Array);
    array = new XWArray(xref);
    return this;
}

XWObject * XWObject::initDict(XWRef *xref)
{
	init(Dict);
  	dict = new XWDict(xref);
  	return this;
}

XWObject * XWObject::initDict(XWDict *dictA)
{
	init(Dict);
  	dict = dictA;
  	dict->incRef();
  	return this;
}

XWObject * XWObject::initIndirect(void * pfA, 
                                  int obj_num, 
                                  int obj_gen)
{
	init(Indirect); 
	label = obj_num;
  	generation = obj_gen;
	pf = pfA; 
	
	return this;
}

XWObject * XWObject::initStream(XWStream *streamA)
{
	init(Stream);
  	stream = streamA;
  	flags = -1;
  	return this;
}

XWObject * XWObject::initStream(int flagsA, XWRef * xrefA)
{
	init(Stream);
	QBuffer * f = new QBuffer;
	f->open(QBuffer::ReadWrite);
	XWFileStream * filestr = new XWFileStream(f); 
	stream = filestr;
	flags = flagsA;
	flags |= OBJ_NO_OBJSTM;
	XWObject obj;
	obj.initDict(xrefA);
	filestr->setDict(&obj);
	
	return this;
}

XWObject * XWObject::initUnknown(uchar * d, int len)
{
	init(Unknown);
	data = new char[len + 1];
	memcpy(data, d, len);
  	data[len] = '\0';
  	
  	return this;
}

void XWObject::labelObj()
{
	if (label == 0)
	{
		label = nextLabel++;
		generation = 0;
	}
}

long XWObject::lengthOf()
{
	long len = 0;
	switch (type)
	{
		case Name:
			if (name)
				len = strlen(name);
			break;
			
		case String:
			if (string)
				len = string->getLength();
			break;
			
		default:
			break;
	}
	
	return len;
}

void XWObject::linkObj(XWObject *obj)
{
	if (obj)
	{
		copy(obj);
		obj->refcount++;
	}
}

void XWObject::print(QTextStream * f)
{
	XWObject obj;
	switch (type)
	{
		case Bool:
			if (booln)
				*f << "true";
			else
				*f << "false";
    		break;
    		
  		case Int:
  			*f << intg;
    		break;
    		
  		case Real:
  			*f << real;
    		break;
    		
  		case String:
  			*f << "(";
  			*f << string->getCString();
  			*f << ")";
    		break;
    		
  		case Name:
  			*f << name;
    		break;
    		
  		case Null:
  			*f << "null";
    		break;
    		
  		case Array:
  			*f << "[";
    		for (int i = 0; i < arrayGetLength(); ++i) 
    		{
      			if (i > 0)
					*f << " ";
      			arrayGetNF(i, &obj);
      			obj.print(f);
      			obj.free();
    		}
    		*f << "]";
    		break;
    		
  		case Dict:
  			*f << "<<";
    		for (int i = 0; i < dictGetLength(); ++i) 
    		{
    			*f << "/";
    			*f << dictGetKey(i);
      			dictGetValNF(i, &obj);
      			obj.print(f);
      			obj.free();
    		}
    		*f << " >>";
    		break;
    		
  		case Stream:
    		*f << "<stream>";
    		break;
    		
  		case Ref:
  			*f << ref.num;
  			*f << " ";
  			*f << ref.gen;
  			*f << " R";
    		break;
    		
  		case Cmd:
  			*f << cmd;
    		break;
    		
  		case Error:
    		*f << "<error>";
    		break;
    		
  		case Eof:
    		*f << "<EOF>";
    		break;
    		
  		case None:
    		*f << "<none>";
    		break;
	}
}

void XWObject::refObj(XWObject * obj)
{
	if (isIndirect())
		linkObj(obj);
	else 
	{
		labelObj();
		obj->initIndirect(0, label, generation);
	}
}

void XWObject::resetNextLabel(int l)
{
	nextLabel = l;
}

void XWObject::streamAdd(const char *buffer, long length)
{
	if (flags == -1)
		return ;
		
	XWFileStream * filestr = (XWFileStream*)stream;
	QIODevice * f = filestr->getDev();
	f->write(buffer, length);
}

int XWObject::streamConcat(XWObject * src)
{
	QByteArray stream_data = src->streamReadAll();
	if (stream_data.size()  == 0)
		return -1;
		
	XWDict * dictA = src->streamGetDict();
	XWObject dp;
	dictA->lookupNF("DecodeParms", &dp);
	if (!dp.isNull())
	{
		dp.free();
		return -1;
	}
	
	XWObject filter;
	dictA->lookupNF("Filter", &filter);
	if (filter.isNull())
		streamAdd(stream_data.constData(), stream_data.size());
	else
	{
		z_stream z;
 		Bytef wbuf[4096];
 		z.zalloc = Z_NULL; z.zfree = Z_NULL; z.opaque = Z_NULL;

  	z.next_in  = (Bytef *) stream_data.data(); z.avail_in  = stream_data.length();
  	z.next_out = (Bytef *) wbuf; z.avail_out = 4096;
  
		if (filter.isName("FlateDecode"))
		{
			if (inflateInit(&z) != Z_OK)
				return -1;
				
			for (;;)
			{
				int status = inflate(&z, Z_NO_FLUSH);
				if (status == Z_STREAM_END)
      		break;
      	else if (status != Z_OK)
      	{
      		inflateEnd(&z);
      		return -1;
      	}
      	
      	if (z.avail_out == 0)
      	{
      		streamAdd((const char*)wbuf, 4096);
      		z.next_out  = wbuf;
      		z.avail_out = 4096;
      	}
			}
			
			if (4096 - z.avail_out > 0)
				streamAdd((const char*)wbuf, 4096 - z.avail_out);
				
			inflateEnd(&z);
		}
		else if (filter.isArray())
		{
			if (filter.arrayGetLength() == 1)
			{
				XWObject filter_name;
				filter.arrayGetNF(0, &filter_name);
				if (filter_name.isName("FlateDecode"))
				{
					if (inflateInit(&z) != Z_OK)
						return -1;
				
					for (;;)
					{
						int status = inflate(&z, Z_NO_FLUSH);
						if (status == Z_STREAM_END)
      				break;
      			else if (status != Z_OK)
      			{
      				inflateEnd(&z);
      				return -1;
      			}
      	
      			if (z.avail_out == 0)
      			{
      				streamAdd((const char*)wbuf, 4096);
      				z.next_out  = wbuf;
      				z.avail_out = 4096;
      			}
					}
			
					if (4096 - z.avail_out > 0)
						streamAdd((const char*)wbuf, 4096 - z.avail_out);
				
					inflateEnd(&z);
				}
				filter_name.free();
			}
			else
			{
				filter.free();
				return -1;
			}
		}
		else
		{
			filter.free();
			return -1;
		}
		
		filter.free();
	}
	
	return 0;
}

char  * XWObject::streamDataPtr()
{
	XWFileStream * filestr = (XWFileStream*)stream;
	QBuffer * f = (QBuffer*)(filestr->getDev());
	return f->buffer().data();
}

QBuffer * XWObject::streamGetIO()
{
	XWFileStream * filestr = (XWFileStream*)stream;
	QIODevice * f = filestr->getDev();
	return (QBuffer*)f;
}

ulong  XWObject::streamGetLength()
{
	XWFileStream * filestr = (XWFileStream*)stream;
	QIODevice * f = filestr->getDev();
	ulong len = (ulong)(f->size());
	return len;
}

QByteArray XWObject::streamReadAll()
{
	XWFileStream * filestr = (XWFileStream*)stream;
	QIODevice * f = filestr->getDev();
	f->seek(0);
	return f->readAll();
}

void XWObject::streamSeek(int pos)
{
	XWFileStream * filestr = (XWFileStream*)stream;
	QBuffer * f = (QBuffer*)(filestr->getDev());
	f->seek(pos);
}

void XWObject::streamSetBuffer(const char * buf, int len)
{
	XWFileStream * filestr = (XWFileStream*)stream;
	QBuffer * f = (QBuffer*)(filestr->getDev());
	f->close();
	f->setData(buf, len);
	f->open(QBuffer::ReadWrite);
}

int  XWObject::streamUncompress(XWRef * xref, XWObject *dst)
{
	dst->initStream(0, xref);
	XWDict * dictA = dst->streamGetDict();
	dictA->merge(streamGetDict());	
	dictA->remove("Length");
	int ret = dst->streamConcat(this);
	return ret;
}

void  XWObject::streamWrite(QIODevice * out)
{
	XWFileStream * filestr = (XWFileStream*)stream;
	QIODevice * f = filestr->getDev();
	f->seek(0);
	char buf[1025];
	while (!f->atEnd())
	{
		qint64 len = f->read(buf, 1024);
		out->write(buf, len);
	}
}

void XWObject::transferLabel(XWObject *src)
{
	label      = src->label;
  	generation = src->generation;
  	src->label      = 0;
  	src->generation = 0;
}

