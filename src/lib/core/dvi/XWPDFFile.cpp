/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include "XWLexer.h"
#include "XWApplication.h"
#include "XWObject.h"
#include "XWDVIRef.h"
#include "XWDVIParser.h"
#include "XWPDFFile.h"

QHash<QString, XWPDFFile*> XWPDFFile::pdfFiles;

XWPDFFile::XWPDFFile(XWDVIRef * xrefA, 
	                 QIODevice * fileA,
	                 int versionA)
	:xref(xrefA),
	 file(fileA),
	 xrefTable(0),
	 num_obj(0),
	 version(versionA),
	 objstmData(0)
{
	trailer.initNull();
	catalog.initNull();
}

XWPDFFile::~XWPDFFile()
{
	if (objstmData)
	{	
		for (long i = 0; i < num_obj; i++)
		{
			if (objstmData[i])
				delete [] objstmData;
		}
	
		free(objstmData);
	}
	
	if (xrefTable)
	{
		for (int i = 0; i < num_obj; i++)
		{
			xrefTable[i].direct.free();
			xrefTable[i].indirect.free();
		}
		free(xrefTable);
	}
		
	trailer.free();
	catalog.free();
}

bool XWPDFFile::checkForPDF(QIODevice * fileA)
{
	return checkVersion(fileA) >= 0;
}

int XWPDFFile::checkVersion(QIODevice * fileA)
{
	char c;
	fileA->getChar(&c);
	if (c != '%')
		return -1;
		
	char buf[9];
	fileA->read(buf, 9);
	buf[8] = '\0';
	uchar minor = 0;
	if (sscanf(buf, "%%PDF-1.%hhu", &minor) == 1)
		return minor;
		
	return -1;
}

void XWPDFFile::close()
{
	file = 0;
}

void XWPDFFile::closeAll()
{
	QHash<QString, XWPDFFile*>::const_iterator i = pdfFiles.constBegin();
	while (i != pdfFiles.constEnd())
	{
		delete i.value();
		++i;
	}
	
	pdfFiles.clear();
}

XWObject * XWPDFFile::derefObj(XWDVIRef * xrefA, XWObject *obj, XWObject * ref)
{
	ref->initNull();
	if (obj)
		obj->copy(ref);
				
	int count = PDF_OBJ_MAX_DEPTH;
	while (ref->isIndirect() && --count)
	{
		XWPDFFile * pf = (XWPDFFile*)(ref->pf);
		int  obj_num = ref->label;
    	int  obj_gen = ref->generation;
    	xrefA->releaseObj(ref);
    	if (!pf)
    		break;
    	pf->getObject(obj_num, obj_gen, ref);
	}
	
	if (ref->isNull())
		return 0;
	
	return ref;
}

XWObject * XWPDFFile::getPage(long *page_p, 
	                          long *count_p, 
	                          PDFRect *bbox, 
	                          XWObject *resources_p,
	                          XWObject * obj)
{
	long page_no = *page_p;
	XWObject tmp;
	catalog.dictLookupNF("Pages", &tmp);
	XWObject page_tree, resources, box, rotate;
	if (!derefObj(xref, &tmp, &page_tree))
	{
		tmp.free();
		goto error;
	}
		
	tmp.free();
		
	if (!page_tree.isDict())
	{
		page_tree.free();
		goto error;
	}
	
	long page_idx = -1;
	{
		page_tree.dictLookupNF("Count", &tmp);
		XWObject tmp1;
		if (!derefObj(xref, &tmp, &tmp1))
		{
			tmp.free();
			goto error;
		}
		tmp.free();
		if (!tmp1.isNum())
		{
			tmp1.free();
			goto error;
		}
		
		long count = (long)(tmp1.getNum());
		page_idx = page_no + (page_no >= 0 ? -1 : count);
		if (page_idx < 0 || page_idx >= count)
		{
			QString msg = QString("page %1 does not exist.\n").arg(page_no);
			xwApp->warning(msg);
			goto error_silent;
		}
		page_no = page_idx+1;
    	*page_p = page_no;
    	*count_p = count;
	}
	
	{
		int depth = PDF_OBJ_MAX_DEPTH;
    	long kids_length = 1, i = 0;
    	XWObject media_box, crop_box, kids, tmp1;
    	media_box.initNull();
    	crop_box.initNull();
    	kids.initNull();
    	while (--depth && i != kids_length)
    	{
    		page_tree.dictLookupNF("MediaBox", &tmp);
    		if (derefObj(xref, &tmp, &tmp1))
    		{
    			tmp.free();
    			if (!media_box.isNull())
    				media_box.free();    				
    			media_box = tmp1;
    		}
    		else
    			tmp.free();
    		
    		page_tree.dictLookupNF("CropBox", &tmp);
    		if (derefObj(xref, &tmp, &tmp1))
    		{
    			tmp.free();
    			if (!crop_box.isNull())
    				crop_box.free();
    			crop_box = tmp1;
    		}
    		else
    			tmp.free();
    		
    		page_tree.dictLookupNF("Rotate", &tmp);
    		if (derefObj(xref, &tmp, &tmp1))
    		{
    			tmp.free();
    			if (!rotate.isNull())
    				rotate.free();    				
    			rotate = tmp1;
    		}
    		else
    			tmp.free();
    		    		
    		page_tree.dictLookupNF("Resources", &tmp);
    		if (derefObj(xref, &tmp, &tmp1))
    		{
    			tmp.free();
    			if (!resources.isNull())
    				resources.free();
    			resources = tmp1;
    		}
    		else
    			tmp.free();
    		
    		page_tree.dictLookupNF("Kids", &tmp);
    		if (!derefObj(xref, &tmp, &kids))
    		{
    			tmp.free();
    			break;
    		}
    		else
    			tmp.free();
    		
    		if (!kids.isArray())
    		{
    			media_box.free();
    			crop_box.free();
    			kids.free();
    			goto error;
    		}
    		
    		kids_length = kids.arrayGetLength();
      		for (i = 0; i < kids_length; i++)
      		{
      			xref->releaseObj(&page_tree);
      			kids.arrayGetNF(i, &tmp);
      			derefObj(xref, &tmp, &page_tree);
      			tmp.free();
      			if (!page_tree.isDict())
      			{
      				media_box.free();
    				crop_box.free();
    				kids.free();
    				goto error;
      			}
      			
      			page_tree.dictLookupNF("Count", &tmp);
      			derefObj(xref, &tmp, &tmp1);
      			tmp.free();
      			long count = 0;      			
      			if (tmp1.isNum())
      			{
      				count = (long)(tmp1.getNum());
      				tmp1.initNull();
      			}
      			else if (tmp1.isNull())
      				count = 1;
      			else
      			{
      				media_box.free();
    				crop_box.free();
    				kids.free();
    				goto error;
      			}
      			
      			if (page_idx < count)
	  				break;

				page_idx -= count;
      		}
      		
      		kids.free();
      		kids.initNull();
    	}
    	
    	if (!depth || kids_length == i) 
    	{
    		media_box.free();
    		crop_box.free();
      		goto error;
    	}
    	
    	if (!crop_box.isNull())
      		box = crop_box;
    	else
    	{
    		page_tree.dictLookupNF("ArtBox", &tmp);
    		if (!derefObj(xref, &tmp, &box))
    		{
    			tmp.free();
    			page_tree.dictLookupNF("TrimBox", &tmp);
    			if (!derefObj(xref, &tmp, &box))
    			{
    				page_tree.dictLookupNF("BleedBox", &tmp);
    				if (derefObj(xref, &tmp, &box))
    				{
    					box = media_box;
    					media_box.initNull();
    				}
    				tmp.free();
    			}
    			else
    				tmp.free();
    		}
    		else
    			tmp.free();
  		}
  		
  		media_box.free();
	}
	
	if (!box.isArray() || box.arrayGetLength() != 4 || !resources.isDict())
    	goto error;
    	
    if (rotate.isNum()) 
	   rotate.initNull();
  	else if (!rotate.isNull())
    	goto error;
    	
    {
    	XWObject tmp1;
    	tmp1.initNull();
    	for (int i = 4; i--; ) 
    	{
    		box.arrayGetNF(i, &tmp);
    		derefObj(xref, &tmp, &tmp1);
    		tmp.free();
    		if (!tmp1.isNum())
    			goto error;
      		double x = tmp1.getNum();
      		switch (i) 
      		{
      			case 0: bbox->llx = x; break;
      			case 1: bbox->lly = x; break;
      			case 2: bbox->urx = x; break;
      			case 3: bbox->ury = x; break;
      		}
    	}
    }
    
    xref->releaseObj(&box);

  	if (resources_p)
    	*resources_p = resources;
  	else
    	resources.free();

  	*obj = page_tree;
  	return obj;
  	
error:
  	xwApp->warning("cannot parse document. Broken PDF file?\n");
error_silent:  	
    box.free();    	
    rotate.free();
    resources.free();
    page_tree.free();

  	return 0;
}

XWObject * XWPDFFile::importObject(XWDVIRef * xrefA,
	                               XWObject *object, 
	                               XWObject * ref)
{
	ref->initNull();
	switch (object->getType())
	{
		case XWObject::Indirect:
			if (object->pf)
				importIndirect(xrefA, object, ref);
			else
				object->copy(ref); 
			break;
			
		case XWObject::Stream:
  			{
  				XWObject dictA;
  				dictA.initDict(object->streamGetDict());
  				XWObject tmp;
  				tmp.initNull();
  				importObject(xrefA, &dictA, &tmp);
  				dictA.free();
  				if (tmp.isNull())
  					return 0;
  					
  				ref->initStream(0, xrefA);
  				ref->streamGetDict()->merge(tmp.getDict());
  				tmp.free();
  				QByteArray ba = object->streamReadAll();
  				ref->streamAdd(ba.constData(), ba.size());
  			}
    		break;
    		
    	case XWObject::Dict:
    		ref->initDict(xrefA);
    		{
    			int len = object->dictGetLength();
    			XWObject obj, tmp;
    			for (int i = 0; i < len; i++)
    			{
    				char * key = object->dictGetKey(i);
    				object->dictGetValNF(i, &obj);
    				if (importObject(xrefA, &obj, &tmp))
    					ref->dictAdd(qstrdup(key), &tmp);	
    				obj.free();
    			}
    		}
    		break;
    		
    	case XWObject::Array:
    		ref->initArray(xrefA);
    		{
    			XWObject obj, tmp;
    			for (int i = 0; i < object->arrayGetLength(); i++)
    			{
    				object->arrayGetNF(i, &tmp);
    				if (importObject(xrefA, &tmp, &obj))
    					ref->arrayAdd(&obj);
    				tmp.free();
    			}
    		}
    		break;
    		
    	default:
    		object->copy(ref);
    		break;
	}
	
	return ref;
}

XWPDFFile * XWPDFFile::open(XWDVIRef * xrefA,
	                        const char * ident, 
	                        QIODevice * fileA)
{
	XWPDFFile * pf = 0;
	QString identA(ident);
	if (pdfFiles.contains(identA))
	{
		pf = pdfFiles[identA];
		pf->setFile(fileA);
		pf->setXRef(xrefA);
		return pf;
	}
	
	int versionA = checkVersion(fileA);
	if (versionA < 0)
		return 0;
		
	pf = new XWPDFFile(xrefA, fileA, versionA);
	if (!pf->readXRef())
	{
		delete pf;
		return 0;
	}
	
	pdfFiles[identA] = pf;
	return pf;
}

int XWPDFFile::backupLine()
{
	char ch = -1;
	if (file->pos() > 1)
	{
		do 
		{
			qint64 pos = file->pos();
			pos -= 2;
			if (pos <= 0)
				break;
				
			file->seek(pos);
    	} while (file->getChar(&ch) && (ch != '\n' && ch != '\r' ));
	}
	
	if (ch < 0) 
    	return 0;

  	return 1;
}

void XWPDFFile::extendXRef(ulong new_size)
{
	xrefTable = (DVIRefEntry*)realloc(xrefTable, new_size * sizeof(DVIRefEntry));
	objstmData = (long**)realloc(objstmData, new_size * sizeof(long*));
  	for (ulong i = num_obj; i < new_size; i++) 
  	{
    	xrefTable[i].direct.initNull();
    	xrefTable[i].indirect.initNull();
    	xrefTable[i].type     = 0;
    	xrefTable[i].offset = -1;
    	xrefTable[i].gen = -1;
    	objstmData[i] = 0;
  	}
  	num_obj = new_size;
}

long XWPDFFile::findXRef()
{
	long xref_pos = 0;
  	int  tries = 10;
  	
  	char buf[1024];
  	do 
  	{
    	if (!backupLine()) 
    	{
      		tries = 0;
      		break;
    	}
    	
    	qint64 currentpos = file->pos();
    	file->read(buf, strlen("startxref"));
    	file->seek(currentpos);
    	tries--;
  	} while (tries > 0 && strncmp(buf, "startxref", strlen("startxref")));
  	
  	if (tries <= 0)
    	return 0;
    	
    file->readLine(buf, 1024);
    file->readLine(buf, 1024);
    {
    	char * start = buf;
    	char * end   = start + strlen(buf);
    	XWLexer lexer(start, end);
    	lexer.skipWhite();
    	char * number   = lexer.getNumber();
    	if (number)
    	{
    		xref_pos = (long) atof(number);
    		delete [] number;
    	}
  	}

  	return xref_pos;
}

XWObject * XWPDFFile::getObject(int obj_num, 
	                            int obj_gen, 
	                            XWObject * obj)
{
	if (!checkLabel(obj_num, obj_gen)) 
	{
		QString msg = QString("trying to read nonexistent or deleted object: %1 %2\n")
		                      .arg((ulong)obj_num).arg((ulong)obj_gen);
    	xwApp->warning(msg);
    	obj->initNull();
    	return 0;
  	}
  	
  	if (!xrefTable[obj_num].direct.isNull())
  	{
  		xrefTable[obj_num].direct.copy(obj);
  		return obj;
  	}
  	
  	XWObject result;
  	result.initNull();
  	if (xrefTable[obj_num].type == 1)
  	{
  		int offset = xrefTable[obj_num].offset;
    	int limit  = nextObjectOffset(obj_num);
    	readObject(obj_num, obj_gen, offset, limit, &result);
  	}
  	else
  	{
  		int objstm_num = xrefTable[obj_num].offset;
    	int index = xrefTable[obj_num].gen;
    	
    	if (objstm_num >= num_obj || xrefTable[objstm_num].type != 1)
    		goto badobj;
    		
    	if (xrefTable[objstm_num].direct.isNull() && !readObjStm(objstm_num))
    		goto badobj;
    		
    	long * data = objstmData[objstm_num];
    	long n = *(data++);
    	long first = *(data++);
    	if (index >= n || data[2*index] != obj_num)
      		goto badobj;
      		
      	ulong length = xrefTable[objstm_num].direct.streamGetLength();
    	char * p = xrefTable[objstm_num].direct.streamDataPtr() + first + data[2*index+1];
    	char * q = p + (index == n-1 ? length : first+data[2*index+3]);
    	XWLexer lexer(p, q);
    	XWDVIParser parser(xref, &lexer, this);
    	if (!parser.parsePDFObject(&result))
      		goto badobj;
  	}
  	
  	if (!xrefTable[obj_num].direct.isNull())
  		xref->releaseObj(&(xrefTable[obj_num].direct));
  	
  	result.copy(obj);
  	xrefTable[obj_num].direct = result;
  	return obj;
  	
badobj:
	xwApp->warning("could not read object from object stream.\n");
	obj->initNull();
  	return 0;
}

XWObject * XWPDFFile::importIndirect(XWDVIRef * xrefA,
	                                 XWObject * object, 
	                                 XWObject *ref)
{
	XWPDFFile * pf = (XWPDFFile*)(object->pf);
	int obj_num = object->label;
  	int obj_gen = object->generation;
  	if (!pf->checkLabel((long)obj_num, (long)obj_gen))
  	{
  		ref->initNull();
  		return ref;
  	}
  		
  	if (pf->xrefTable[obj_num].indirect.isNone())
  	{
  		xwApp->error("loop in object hierarchy detected. Broken PDF file?\n");
  		ref->initNull();
  		return 0;
  	}
  	
  	XWObject obj, obj1;
  	pf->getObject(obj_num, obj_gen, &obj);
  	pf->xrefTable[obj_num].indirect.initNone();
  	
  	importObject(xrefA, &obj, &obj1);
  	obj.free();
  	xrefA->refObj(&obj1, &obj);
  	obj1.free();
  	
  	pf->xrefTable[obj_num].indirect = obj;  	
  	obj.copy(ref);  	
  	return ref;
}

int XWPDFFile::nextObjectOffset(int obj_num)
{
	int  next = (int)(file->size());
	int curr = xrefTable[obj_num].offset;
	for (long i = 0; i < num_obj; i++) 
	{
    	if (xrefTable[i].type == 1 && 
        	xrefTable[i].offset > curr &&
        	xrefTable[i].offset < next)
      		next = xrefTable[i].offset;
  	}

  	return  next;
}

XWObject * XWPDFFile::parseTrailer(XWObject * obj)
{
	char buf[1024];
	qint64 len = 0;
	if ((len = file->read(buf, 1024)) == 0 || 
		strncmp(buf, "trailer", strlen("trailer")))
	{
		QString msg = QString("no trailer.  Are you sure this is a PDF file?\n"
		                      "buffer:->%1<-\n").arg(buf);
		xwApp->warning(msg);
		return 0;
	}
	
	char *p = buf + strlen("trailer");
	XWLexer lexer(p, buf + 1024);
	lexer.skipWhite();
	XWDVIParser parser(xref, &lexer, this);
	return parser.parsePDFDict(obj);
}

ulong XWPDFFile::parseXRefStmField(char **p, 
	                               int length, 
	                               ulong def)
{
	ulong val = 0;

  	if (!length)
    	return def;

  	while (length--) 
  	{
    	val <<= 8;
    	val |= (unsigned char) *((*p)++);
  	}

  	return val;
}

int XWPDFFile::parseXRefStmSubsec(char **p, 
	                              long *length,
		                          int *W, 
		                          int wsum,
		                          long first, 
		                          long size)
{
	if ((*length -= wsum*size) < 0)
    	return -1;
    	
    if (num_obj < first+size)
    	extendXRef((ulong)(first+size));
    	
    DVIRefEntry * e = xrefTable + first;
    while (size--)
    {
    	uchar type = (uchar)parseXRefStmField(p, W[0], 1);
    	
    	if (type > 2)
      		xwApp->warning("unknown cross-reference stream entry type.\n");
    	else if (!W[1] || (type != 1 && !W[2]))
      		return -1;
      		
      	int field2 = (int) parseXRefStmField(p, W[1], 0);
    	int field3 = (int)parseXRefStmField(p, W[2], 0);

    	if (!e->offset) 
    	{
      		e->type   = type;
      		e->offset = field2;
      		e->gen = field3;	
      	}
    	e++;
    }
    
    return 0;
}

int XWPDFFile::parseXRefStream(long xref_pos, XWObject *trailerA)
{
	XWObject tmp, xrefstm, size_obj, W_obj, index;
	if (!readObject(0, 0, xref_pos, file->size(), &tmp) || !tmp.isStream())
	{
		tmp.free();
		goto badstream;
	}

	tmp.streamUncompress(xref, &xrefstm);
	tmp.free();
  	
  	trailerA->initDict(xrefstm.streamGetDict());
  	trailerA->refcount++;
  	trailerA->dictLookupNF("Size", &size_obj);
  	if (!size_obj.isNum())
  	{
  		size_obj.free();
  		goto badstream;
  	}  		
  	int size = size_obj.isInt() ? size_obj.getInt() : (int)size_obj.getReal();
  	size_obj.free();
  	
  	long length = (long)(xrefstm.streamGetLength());
  	trailerA->dictLookupNF("W", &W_obj);
  	if (!W_obj.isArray() || W_obj.arrayGetLength() != 3)
  	{
  		W_obj.free();
  		goto badstream;
  	}
  		
  	int W[3];
  	int wsum = 0;
  	for (int i = 0; i < 3; i++)
  	{
  		W_obj.arrayGetNF(i, &tmp);
  		if (!tmp.isNum())
  		{
  			W_obj.free();
      		goto badstream;
      	}
    	wsum += (W[i] = tmp.isInt() ? tmp.getInt() : (int)tmp.getReal());
  	}
  	W_obj.free();
  	
  	char *p = xrefstm.streamDataPtr();
  	trailerA->dictLookupNF("Index", &index);
  	if (!index.isNull())
  	{
  		int index_len = 0;
    	if (!index.isArray() || ((index_len = index.arrayGetLength()) % 2 ))
    	{
    		index.free();
      		goto badstream;
      	}
      	
      	int i = 0;
      	XWObject first, sizeobj;
    	while (i < index_len) 
    	{
    		index.arrayGetNF(i++, &first);
    		index.arrayGetNF(i++, &sizeobj);
    		
      		if (!first.isNum() || 
      			!sizeobj.isNum() || 
	  			parseXRefStmSubsec(&p, &length, W, wsum,
			                        first.isInt() ? (long)first.getInt() : (long)first.getReal(),
			       			        sizeobj.isInt() ? (long)sizeobj.getInt() : (long)sizeobj.getReal()))
			{
				index.free();
				goto badstream;
			}
    	}
    	index.free();
  	}
  	else if (parseXRefStmSubsec(&p, &length, W, wsum, 0, size))
      	goto badstream;
      	
    xrefstm.free();
    return 1;
    
badstream:
	xwApp->warning("cannot parse cross-reference stream.\n");	
	xrefstm.free();
	trailerA->free();
	trailerA->initNull();
	return 0;
}

int XWPDFFile::parseXRefTable(long xref_pos)
{
	file->seek(xref_pos);
	char buf[1024];
	file->readLine(buf, 1024);
	if (memcmp(buf, "xref", 4))
		return 0;
		
	for (;;)
	{
		qint64 current_pos = file->pos();
		if (file->readLine(buf, 1024) <= 0)
		{
			xwApp->warning("premature end of PDF file while parsing xref table.\n");
      		return -1;
		}
		
		if (!strncmp(buf, "trailer", 7))
		{
			file->seek(current_pos);
			break;
		}
		
		ulong first, size;
		sscanf(buf, "%lu %lu", &first, &size);
    	if ((ulong)num_obj < first + size) 
      		extendXRef(first + size);
      		
      	for (ulong i = first; i < first + size; i++)
      	{
      		file->read(buf, 20);
      		buf[19] = 0;
      		ulong offset = 0UL; 
      		int obj_gen = 0; 
      		int flag = 0;
      		int r = sscanf(buf, "%010lu %05u %c", &offset, &obj_gen, &flag);
      		if ( r != 3 || 
      			((flag != 'n' && flag != 'f') || 
      			(flag == 'n' && (offset >= file->size() || 
      			(offset > 0 && offset < 4))))) 
      		{
      			QString msg = QString("invalid xref table entry [%lu]. PDF file is corrupt...\n").arg(i);
        		xwApp->warning(msg);
        		return -1;
      		}
      		if (!xrefTable[i].offset) 
      		{
				xrefTable[i].type = (flag == 'n');
				xrefTable[i].offset = (int)offset;
				xrefTable[i].gen = obj_gen;	
      		}
      	}
	}
	
	return  1;
}

XWObject * XWPDFFile::readObject(int obj_num, 
	                             int obj_gen,
						         int offset, 
						         int limit,
						         XWObject * obj)
{
	int length = limit - offset;
	if (length <= 0)
	{
		obj->initNull();
    	return 0;
    }
    	
    char * buffer = new char[length + 1];
    file->seek(offset);
    file->read(buffer, length);
    char * p      = buffer;
  	char * endptr = p + length;
  	XWLexer lexer(p, endptr);
  	{
  		const char * q = lexer.skipWhite();
  		char * sp = lexer.getUnsigned();
  		if (!sp) 
  		{
      		delete [] buffer;
      		obj->initNull();
      		return 0;
    	}
    	
    	ulong n = strtoul(sp, NULL, 10);
    	delete [] sp;
    	
    	q = lexer.skipWhite();
    	
    	sp = lexer.getUnsigned();
    	if (!sp) 
    	{
      		delete [] buffer;
      		obj->initNull();
      		return 0;
    	}
    	
    	ulong g = strtoul(sp, NULL, 10);
    	delete [] sp;
    	
    	if (obj_num && (n != (ulong)obj_num || g != (ulong)obj_gen)) 
    	{
      		delete [] buffer;
      		obj->initNull();
      		return 0;
    	}
  	}
  	
  	p = (char*)lexer.skipWhite();
  	if (memcmp(p, "obj", 3)) 
  	{
    	xwApp->warning("didn't find \"obj\".\n");
    	delete [] buffer;
    	obj->initNull();
      	return 0;
  	}
  	
  	p = (char*)lexer.skip(3);
  	
  	XWDVIParser parse(xref, &lexer, this);
  	XWObject * result = parse.parsePDFObject(obj);
  	p = (char*)lexer.skipWhite();
  	if (memcmp(p, "endobj", 6)) 
  	{
    	xwApp->warning("didn't find \"endobj\".\n");
    	if (result)
    	{
    		result->free();
    		result->initNull();
      	}
      	
    	result = 0;
  	}
  	
  	delete [] buffer;

  	return result;
}

bool XWPDFFile::readObjStm(int num)
{
	int offset = xrefTable[num].offset;
  	int gen = xrefTable[num].gen;
  	int limit = nextObjectOffset(num);
  	
  	XWObject tmp, objstm, type, n_obj, first_obj;
  	if (!readObject(num, gen, offset, limit, &tmp) || 
  		!tmp.isStream())
  	{
  		tmp.free();
  		goto badobjstm;
  	}
  		
  	bool ret = tmp.streamUncompress(xref, &objstm) >= 0 ? true : false;
  	tmp.free();
  	
  	if (!ret)
  	{
  		xwApp->error("only one FlateDecode supported.\n");
  		return false;
  	}
  	
  	XWDict * dict = objstm.streamGetDict();
  	dict->lookupNF("Type", &type);
  	if (!type.isName("ObjStm"))
  	{
  		type.free();
  		goto badobjstm;
  	}
  	type.free();
  	
  	dict->lookupNF("N", &n_obj);
  	if (!n_obj.isNum())
  	{
  		n_obj.free();
  		goto badobjstm;  		
  	}
  	int n = n_obj.isInt() ? n_obj.getInt() : (int)(n_obj.getReal());
  	dict->lookupNF("First", &first_obj);
  	if (!first_obj.isNum())
  	{
  		first_obj.free();
  		goto badobjstm; 
  	}
  	ulong first = first_obj.isInt() ? first_obj.getInt() : (ulong)(first_obj.getReal());
  	if (first >= xrefTable[num].direct.streamGetLength())
    	goto badobjstm;
    	
    long * header = new long[2*(n+1)];
    objstmData[num] = header;
    *(header++) = n;
  	*(header++) = first;
  	
  	char * data = objstm.streamDataPtr();
  	char data1 = data[first];
  	data[first] = 0;
  	char * p = data;
  	int i = 2*n;
  	char * q = 0;
  	while (i--) 
  	{
    	*(header++) = strtoul(p, &q, 10);
    	if (q == p)
      		goto badobjstm;
    	p = q;
  	}
  	data[first] = data1;
  	char * endptr = data+first;
  	while (p < endptr && (XWLexer::isSpace(*p) || *p == '%')) 
    {
        if (*p == '%')
        {
            while (p < endptr && *p != '\n' && *p != '\r')
        		p++;
    		if (p < endptr && *p == '\r')
        		p++;
    		if (p < endptr && *p == '\n')
        		p++;
        }
        else
            p++;
    }
    
  	if (p != endptr)
  		goto badobjstm;
  		
    xrefTable[num].direct = objstm;
    return true;
  	
badobjstm:
	xwApp->warning("cannot parse object stream.\n");
  	objstm.free();
    	
    if (objstmData[num])
    	delete [] objstmData[num];
    	
    objstmData[num] = 0;
  	return false;
}

XWObject * XWPDFFile::readXRef()
{
	XWObject maintrailer, xrefstm, prev, encrypt, tmp, new_version;
	
	long xref_pos = findXRef();
	if (!xref_pos)
		goto badfile;
		
	maintrailer.initNull();
	while (xref_pos)
	{
		int res = parseXRefTable(xref_pos);
    	if (res > 0)
    	{
    		if (!parseTrailer(&trailer))
    			goto badfile;
    			
    		if (maintrailer.isNull())
    			trailer.copy(&maintrailer);
    			
    		trailer.dictLookupNF("XRefStm", &xrefstm);
    		if (!xrefstm.isNull())
    		{
    			XWObject new_trailer;
    			if (xrefstm.isNum() && 
    				parseXRefStream(xrefstm.isInt() ? (long)xrefstm.getInt() : (long)xrefstm.getReal(), &new_trailer))
    				xref->releaseObj(&new_trailer);
    			new_trailer.free();
    		}
    		
    		xrefstm.free();
    	}
    	else if (!res && parseXRefStream(xref_pos, &trailer))
    	{
    		if (maintrailer.isNull())
    			trailer.copy(&maintrailer);
    		else
    			goto badfile;
    			
    		trailer.dictLookupNF("Prev", &prev);
    		if (prev.isNum())
    			xref_pos = prev.isInt() ? (long)prev.getInt() : (long)prev.getReal();
    		else if (prev.isNull())
    			goto badfile;
    		else
    			xref_pos = 0;
    		prev.free();
    	}
    	
    	trailer.free();
    	trailer.initNull();
	}
	
	maintrailer.dictLookupNF("Encrypt", &encrypt);
	if (!encrypt.isNull())
	{
		encrypt.free();
		xwApp->warning("pdf document is encrypted.\n");
		goto errorfile;
	}
	
	maintrailer.dictLookupNF("Root", &tmp);
	derefObj(xref, &tmp, &catalog);
	tmp.free();
	if (!catalog.isDict())
	{
		xwApp->warning("cannot read PDF document catalog. Broken PDF file?\n");
      	goto errorfile;
	}
	
	catalog.dictLookupNF("Version", &tmp);
	derefObj(xref, &tmp, &new_version);
	tmp.free();
	int minor;
	if (!new_version.isName() || sscanf(new_version.getName(), "1.%hhu", &minor) != 1)
	{
		new_version.free();
		xwApp->warning("illegal Version entry in document catalog. Broken PDF file?\n");
		goto badfile;
	}
	
	new_version.free();
	if (version < minor)
		version = minor;
	
	trailer = maintrailer;
	return &trailer;
	
badfile:
	xwApp->warning("error while parsing PDF file.\n");
	
errorfile:
	maintrailer.free();
  	return 0;
}
