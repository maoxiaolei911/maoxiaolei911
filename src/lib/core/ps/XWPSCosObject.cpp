/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <QIODevice>
#include "XWPSError.h"
#include "XWPSDevicePDF.h"
#include "XWPSStream.h"
#include "XWPSCosObject.h"

#define DICT_COPY_KEY 1
#define DICT_COPY_VALUE 2
#define DICT_FREE_KEY 4
#define DICT_COPY_ALL (DICT_COPY_KEY | DICT_COPY_VALUE | DICT_FREE_KEY)

XWPSCosValue::XWPSCosValue()
{
	type = COS_VALUE_CONST;
	contents.chars.data = 0;
	contents.chars.size = 0;
}

XWPSCosValue::XWPSCosValue(int i)
{
	type = COS_VALUE_SCALAR;
	char str[20];
	contents.chars.size = sprintf(str, "%d", i);
	contents.chars.data = (uchar*)malloc(contents.chars.size * sizeof(uchar));
	memcpy((void*)contents.chars.data, (void*)str, contents.chars.size * sizeof(uchar));
}

XWPSCosValue::XWPSCosValue(float i)
{
	type = COS_VALUE_SCALAR;
	char str[20];
	contents.chars.size = sprintf(str, "%g", i);
	contents.chars.data = (uchar*)malloc(contents.chars.size * sizeof(uchar));
	memcpy((void*)contents.chars.data, (void*)str, contents.chars.size * sizeof(uchar));
}

XWPSCosValue::XWPSCosValue(const char *str)
{
	type = COS_VALUE_SCALAR;
	contents.chars.size = strlen(str);
	contents.chars.data = 0;
	if (contents.chars.size > 0)
	{
		contents.chars.data = (uchar*)malloc(contents.chars.size * sizeof(uchar));
		memcpy((void*)contents.chars.data, (void*)str, contents.chars.size * sizeof(uchar));
	}
}

XWPSCosValue::XWPSCosValue(const uchar *data, uint size)
{
	type = COS_VALUE_SCALAR;
	contents.chars.size = size;
	contents.chars.data = 0;
	if (size > 0)
	{
		contents.chars.data = (uchar*)malloc(contents.chars.size * sizeof(uchar));
		memcpy((void*)contents.chars.data, (void*)data, contents.chars.size * sizeof(uchar));
	}
}

XWPSCosValue::XWPSCosValue(XWPSCosObject * obj)
{
	type = COS_VALUE_OBJECT;
	contents.object = obj;
}

XWPSCosValue::XWPSCosValue(XWPSCosObject * obj, PSCosValueType t)
{
	type = t;
	contents.object = obj;
}

XWPSCosValue::XWPSCosValue(XWPSCosValue * pvalue)
{
	copy(pvalue);
}

XWPSCosValue::~XWPSCosValue()
{
	myFree();
}

void XWPSCosValue::copy(XWPSCosValue *pvalue)
{
	type = pvalue->type;
	switch (type)
	{
		case COS_VALUE_SCALAR:
		case COS_VALUE_CONST:
			if (pvalue->contents.chars.size > 0)
			{
				contents.chars.size = pvalue->contents.chars.size;
				contents.chars.data = (uchar*)malloc(pvalue->contents.chars.size * sizeof(uchar));
				memcpy((void*)contents.chars.data, (void*)(pvalue->contents.chars.data), pvalue->contents.chars.size * sizeof(uchar));
		  }
			break;
			
		case COS_VALUE_OBJECT:
		case COS_VALUE_RESOURCE:
			contents.object = pvalue->contents.object;
			if (contents.object)
			  contents.object->incRef();
			break;
			
		default:
			break;
	}
}

void XWPSCosValue::myFree()
{
	switch (type)
	{
		case COS_VALUE_SCALAR:
		case COS_VALUE_CONST:
			if (contents.chars.data)
			{
				free(contents.chars.data);
				contents.chars.data = 0;
				contents.chars.size = 0;
			}
			break;
			
		case COS_VALUE_OBJECT:
			if (contents.object)
			{
				if (contents.object->decRef() == 0)
				  delete contents.object;
				contents.object = 0;
			}
			break;
			
		case COS_VALUE_RESOURCE:
			if (contents.object)
			{
				if (contents.object->decRef() == 0)
				  delete contents.object;
				contents.object = 0;
			}
			break;
			
		default:
			break;
	}
}

void XWPSCosValue::objectValue(XWPSCosObject * obj)
{
	myFree();
	type = COS_VALUE_OBJECT;
	contents.object = obj;
}

void XWPSCosValue::resourceValue(XWPSCosObject * obj)
{
	myFree();
	contents.object = obj;
  type = COS_VALUE_RESOURCE;
}

void XWPSCosValue::stringValue(const char *str)
{
	myFree();
	type = COS_VALUE_SCALAR;
	contents.chars.size = strlen(str);
	if (contents.chars.size > 0)
	{
		contents.chars.data = (uchar*)malloc(contents.chars.size * sizeof(uchar));
		memcpy((void*)contents.chars.data, (void*)str, contents.chars.size * sizeof(uchar));
	}
}

void XWPSCosValue::stringValue(const uchar *data, uint size)
{
	myFree();
	type = COS_VALUE_SCALAR;
	contents.chars.size = size;
	if (size > 0)
	{
		contents.chars.data = (uchar*)malloc(contents.chars.size * sizeof(uchar));
		memcpy((void*)contents.chars.data, (void*)data, contents.chars.size * sizeof(uchar));
	}
}

int XWPSCosValue::write(XWPSDevicePDF * pdev)
{
	switch (type)
	{
		case COS_VALUE_SCALAR:
    case COS_VALUE_CONST:
    	pdev->writeValue(contents.chars.data,	contents.chars.size);
    	break;
    	
    case COS_VALUE_RESOURCE:
    		pdev->strm->print("/R%ld", contents.object->id);
    	break;
    	
    case COS_VALUE_OBJECT:
    	if (!contents.object->id)
    		contents.object->write(pdev);
    	else
    		pdev->strm->print("%ld 0 R", contents.object->id);
    	break;
    	
    default:
    		return (int)(XWPSError::Fatal);
    	break;
	}
	
	return 0;
}

int XWPSCosValue::writeObject(XWPSDevicePDF * pdev)
{
	if (isObject() && contents.object->id)
		contents.object->writeObject(pdev);
		
	return 0;
}

XWPSCosStreamPiece::XWPSCosStreamPiece()
	:XWPSCosElement(),
	 position(0),
	 size(0)
{
}

XWPSCosArrayElement::XWPSCosArrayElement()
	:XWPSCosElement(),
	 index(0)
{
}

XWPSCosDictElement::XWPSCosDictElement()
	:XWPSCosElement()
{
	key.data = 0;
	key.size = 0;
	owns_key = false;
}

XWPSCosDictElement::~XWPSCosDictElement()
{
	if (owns_key)
	{
		if (key.data)
		{
			delete [] key.data;
			key.data = 0;
		}
		
		key.size = 0;
	}
}

int XWPSCosDictElement::write(XWPSStream * s, XWPSDevicePDF * pdev)
{
	XWPSStream *save = pdev->strm;
	pdev->strm = s;
  XWPSCosDictElement * pcde = this;
  for (; pcde; pcde = (XWPSCosDictElement*)(pcde->next)) 
  {
		pdev->writeValue(pcde->key.data, pcde->key.size);
		s->pputc(' ');
		pcde->value.write(pdev);
		s->pputc('\n');
  }
  pdev->strm = save;
  return 0;
}

int XWPSCosDictElement::writeObject(XWPSDevicePDF * pdev)
{
	return value.writeObject(pdev);
}

PSCosObjectProcs cos_generic_procs = {
	&XWPSCosObject::releaseGeneric,
	&XWPSCosObject::writeGeneric
};

PSCosObjectProcs cos_array_procs = {
  &XWPSCosObject::releaseArray,
	&XWPSCosObject::writeArray
};

PSCosObjectProcs cos_dict_procs = {
  &XWPSCosObject::releaseDict,
	&XWPSCosObject::writeDict
};

PSCosObjectProcs cos_stream_procs = {
  &XWPSCosObject::releaseStream,
	&XWPSCosObject::writeStream
};

XWPSCosObject::XWPSCosObject(XWPSDevicePDF * devA)
{
	refCount = 1;
	pdev = devA;
	init(&cos_generic_procs);
}

XWPSCosObject::XWPSCosObject(XWPSDevicePDF * devA, PSCosObjectProcs *procs)
{
	refCount = 1;
	pdev = devA;
	init(procs);
}

XWPSCosObject::XWPSCosObject(XWPSDevicePDF * devA, const float *pf, uint size)
{
	refCount = 1;
	pdev = devA;
	init(&cos_array_procs);
	for (uint i = 0; i < size; ++i) 
		arrayAdd(pf[i]);
}

XWPSCosObject::~XWPSCosObject()
{
	(this->*(cos_procs->release))();
}

int XWPSCosObject::arrayAdd(XWPSCosValue *pvalue)
{
	return arrayPut(nextIndex(), pvalue);
}

int XWPSCosObject::arrayAdd(const char *str)
{
	XWPSCosValue v(str);
	return arrayAdd(&v);
}

int XWPSCosObject::arrayAdd(int i)
{
	XWPSCosValue v(i);
	return arrayAdd(&v);
}

int XWPSCosObject::arrayAdd(float r)
{
	XWPSCosValue v(r);
	return arrayAdd(&v);
}

int XWPSCosObject::arrayAdd(XWPSCosObject * pco)
{
	XWPSCosValue v(pco);
	return arrayAdd(&v);
}

int XWPSCosObject::arrayPut(long index, XWPSCosValue *pvalue)
{
	XWPSCosArrayElement **ppcae = (XWPSCosArrayElement**)(&elements);
  XWPSCosArrayElement *next;
  while ((next = *ppcae) != 0 && next->index > index)
  	ppcae = (XWPSCosArrayElement**)(&(next->next));
		
	XWPSCosArrayElement *pcae;
	if (next && next->index == index)
	{
		next->value.myFree();
		pcae = next;
	}
	else
	{
		pcae = new XWPSCosArrayElement;
		pcae->index = index;
		pcae->next = next;
		*ppcae = pcae;
	}
	
	pcae->value.copy(pvalue);
	return 0;
}

int  XWPSCosObject::become(PSCosObjectProcs * cotype)
{
	if (cos_procs != &cos_generic_procs)
		return (int)(XWPSError::TypeCheck);
  cos_procs = cotype;
  return 0;
}

int XWPSCosObject::dictElementsWrite(XWPSDevicePDF * devA)
{
	return elementsWrite(devA->strm, devA);
}

XWPSCosValue * XWPSCosObject::dictFind(const char *key)
{
	return dictFind((const uchar *)key, strlen(key));
}

XWPSCosValue * XWPSCosObject::dictFind(const uchar *key_data, uint key_size)
{
	XWPSCosDictElement *pcde = (XWPSCosDictElement*)elements;
  for (; pcde; pcde = (XWPSCosDictElement*)(pcde->next))
		if (!bytes_compare(key_data, key_size, pcde->key.data, pcde->key.size))
	    return &pcde->value;
    
  return 0;
}

int XWPSCosObject::dictObjectsDetete()
{
	XWPSCosDictElement *pcde = (XWPSCosDictElement*)elements;
	for (; pcde; pcde = (XWPSCosDictElement*)(pcde->next))
	  pcde->value.contents.object->id = 0;
  return 0;
}

int XWPSCosObject::dictObjectsWrite(XWPSDevicePDF * devA)
{
	XWPSCosDictElement *pcde = (XWPSCosDictElement*)elements;
  for (; pcde; pcde = (XWPSCosDictElement*)(pcde->next))
		pcde->writeObject(devA);
  return 0;
}

int XWPSCosObject::dictPut(const char *key, const char *str)
{
	XWPSCosValue value(str);
	return dictPut(key, &value);
}

int XWPSCosObject::dictPut(const char *key, const float *pf, uint size)
{
	XWPSCosObject * pca = new XWPSCosObject(pdev, &cos_array_procs);
	for (uint i = 0; i < size; i++)
		pca->arrayAdd(pf[i]);
	XWPSCosValue v(pca);
	return dictPut(key, &v);
}

int XWPSCosObject::dictPut(const char *key, float r)
{
	XWPSCosValue value(r);
	return dictPut(key, &value);
}

int XWPSCosObject::dictPut(const char *key, int i)
{
	XWPSCosValue value(i);
	return dictPut(key, &value);
}

int XWPSCosObject::dictPut(const char *key,  const uchar *data, uint size)
{
	XWPSCosValue value(data, size);
	return dictPut(key, &value);
}

int XWPSCosObject::dictPut(const char *key, XWPSCosValue *pvalue)
{
	return dictPut((const uchar *)key, strlen(key), pvalue, DICT_COPY_VALUE);
}

int XWPSCosObject::dictPut(const char *key, XWPSCosObject *pc)
{
	XWPSCosValue value(pc);
	return dictPut(key, &value);
}

int XWPSCosObject::dictPut(const uchar *key_data, 
	                         uint key_size,
	                         XWPSCosValue *pvalue)
{
	return dictPut(key_data, key_size, pvalue, DICT_COPY_ALL);
}

int XWPSCosObject::dictPut(const uchar *key_data, 
	                         uint key_size,
		                       const uchar *value_data, 
		                       uint value_size)
{
	XWPSCosValue v(value_data, value_size);
	return dictPut(key_data, key_size, &v);
}

int XWPSCosObject::dictPut(const uchar *key_data, 
	                         uint key_size, 
	                         XWPSCosValue *pvalue, 
	                         int )
{
	XWPSCosDictElement ** ppcde = (XWPSCosDictElement**)(&elements);
	XWPSCosDictElement * next;
	while ((next = *ppcde) != 0 && bytes_compare(next->key.data, next->key.size, key_data, key_size))
		ppcde = (XWPSCosDictElement**)(&(next->next));
	
	XWPSCosDictElement * pcde;
	if (next)
	{
		next->value.myFree();
		pcde = next;
	}
	else
	{
		uchar *copied_key_data = 0;
		copied_key_data = new uchar[key_size];
		memcpy(copied_key_data, key_data, key_size);
			
		pcde = new XWPSCosDictElement;
		pcde->key.data = copied_key_data;
		pcde->key.size = key_size;
		pcde->owns_key = true;
		pcde->next = next;
		*ppcde = pcde;
	}
	pcde->value.copy(pvalue);
  return 0;
}

void XWPSCosObject::init(PSCosObjectProcs *procs)
{
	cos_procs = procs;
	id = 0;
	elements = 0;
	pieces = 0;
	pdev = pdev;
	is_open = true;
	is_graphics = false;
	written = false;
}

int XWPSCosObject::streamAdd(uint size)
{
	XWPSStream *s = pdev->streams->strm;
  long position = s->tell();
  XWPSCosStreamPiece *prev = pieces;
  if (prev != 0 && prev->position + prev->size + size == position) 
		prev->size += size;
  else
  {
  	XWPSCosStreamPiece * pcsp = new XWPSCosStreamPiece;
  	pcsp->position = position - size;
		pcsp->size = size;
		pcsp->next = pieces;
		pieces = pcsp;
  }
  return 0;
}

int XWPSCosObject::streamAddBytes(const uchar *data, uint size)
{
	pdev->streams->strm->write(data, size);
  return streamAdd(size);
}

int XWPSCosObject::streamAddSince(long start_pos)
{
	return streamAdd((uint)(pdev->streams->strm->tell() - start_pos));
}

int XWPSCosObject::streamContentsWrite(XWPSDevicePDF * devA)
{
	QIODevice *sfile = devA->streams->file;
	XWPSStream *s = devA->strm;
	
	devA->streams->strm->flush();
	
	long end_pos = (long)(sfile->pos());
	XWPSCosStreamPiece *pcsp = pieces;
  XWPSCosStreamPiece *last = 0;
  XWPSCosStreamPiece *next = 0;
  for (; pcsp; pcsp = next)
  {
		next = (XWPSCosStreamPiece*)(pcsp->next); 
		pcsp->next = last; 
		last = pcsp;
	}
	
	int code = 0;
	for (pcsp = last, code = 0; pcsp && code >= 0; pcsp = (XWPSCosStreamPiece*)(pcsp->next))
	{
		sfile->seek(pcsp->position);
		s->copyData(sfile, pcsp->size);
	}
	
	for (pcsp = last, last = NULL; pcsp; pcsp = next)
	{
		next = (XWPSCosStreamPiece*)(pcsp->next); 
		pcsp->next = last; 
		last = pcsp;
	}

  sfile->seek(end_pos);
  return 0;
}

int XWPSCosObject::streamElementsWrite(XWPSDevicePDF * devA)
{
	return elementsWrite(devA->strm, devA);
}

long XWPSCosObject::streamLength()
{
	long l = 0;
	XWPSCosStreamPiece * pcsp = pieces;
  for (; pcsp; pcsp = (XWPSCosStreamPiece*)(pcsp->next))
		l += pcsp->size;
  return l;
}

int  XWPSCosObject::write(XWPSDevicePDF * devA)
{
	return (this->*(cos_procs->write))(devA);
}

int XWPSCosObject::writeObject(XWPSDevicePDF * devA)
{
	if (id == 0 || written)
		return (int)(XWPSError::Fatal);
	
  devA->openSeparate(id);
  int code = write(devA);
  devA->endSeparate();
  written = true;
  return code;
}

int XWPSCosObject::elementsWrite(XWPSStream * s, XWPSDevicePDF * devA)
{
	XWPSCosDictElement * h = (XWPSCosDictElement*)elements;
	if (h)
		return h->write(s, devA);
	return 0;
}

long XWPSCosObject::nextIndex()
{
	XWPSCosArrayElement *pcae = (XWPSCosArrayElement*)elements;
	return (pcae ? pcae->index + 1 : 0L);
}

void XWPSCosObject::releaseArray()
{
	XWPSCosElement * cur = elements;
	XWPSCosElement * next = 0;
	for (; cur; cur = next)
	{
		next = cur->next;
		delete cur;
	}
	elements = 0;
}

void XWPSCosObject::releaseDict()
{
	releaseArray();
}

void XWPSCosObject::releaseStream()
{
	XWPSCosStreamPiece *cur;
  XWPSCosStreamPiece *nextA;

  for (cur = pieces; cur; cur = nextA) 
  {
		nextA = (XWPSCosStreamPiece*)(cur->next);
		delete cur;
  }
  pieces = 0;
    
	releaseArray();
}

void XWPSCosObject::reorder()
{
	if (elements == 0 || elements->next == 0)
		return ;
		
  XWPSCosElement * pcae = elements;
  XWPSCosElement * next = 0;
  XWPSCosElement * last = 0;
	for (; pcae; pcae = next)
	{
		next = pcae->next;
		pcae->next = last; 
		last = pcae;
	}
	
	elements = last;
}

int XWPSCosObject::writeArray(XWPSDevicePDF * devA)
{
	XWPSStream *s = devA->strm;
	s->pputs("[");
	reorder();
	XWPSCosArrayElement * first = (XWPSCosArrayElement*)elements;
	XWPSCosArrayElement * pcae = first;
	uint last_index = 0;
	for (; pcae; ++last_index, pcae = (XWPSCosArrayElement*)(pcae->next))
	{
		for (; pcae->index > last_index; ++last_index)
			s->pputs("null\n");
		pcae->value.write(devA);
		s->pputs("\n");
	}
	reorder();
	s->pputs("]");
	return 0;
}

int  XWPSCosObject::writeDict(XWPSDevicePDF * devA)
{
	XWPSStream *s = pdev->strm;
	s->pputs("<<");	
	elementsWrite(s, devA);		
	s->pputs(">>");
	return 0;
}

int XWPSCosObject::writeGeneric(XWPSDevicePDF * )
{
	return (int)(XWPSError::Fatal);
}

int XWPSCosObject::writeStream(XWPSDevicePDF * devA)
{
	XWPSStream *s = devA->strm;
	s->pputs("<<");
	XWPSCosDictElement * h = (XWPSCosDictElement*)elements;
	if (h)
		h->write(s, devA);	
		
	long l = streamLength();
	s->print("/Length %ld>>stream\n", l);
  int code = streamContentsWrite(devA);
  s->pputs("\nendstream\n");

  return code;
}
