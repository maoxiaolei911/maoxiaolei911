/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "XWPSDSCParser.h"

#define MAXSTR 256

#define COMPARE(p,str) (strncmp((const char *)(p), (str), sizeof(str)-1)==0)
#define IS_DSC(line, str) (COMPARE((line), (str)))

#define IS_WHITE(ch) (((ch)==' ') || ((ch)=='\t'))
#define IS_EOL(ch) (((ch)=='\r') || ((ch)=='\n'))
#define IS_WHITE_OR_EOL(ch) (IS_WHITE(ch) || IS_EOL(ch))
#define IS_BLANK(str) (IS_EOL(str[0]))
#define NOT_DSC_LINE(str) (((str)[0]!='%') || ((str)[1]!='%'))

#define DSC_START  (data_offset + data_index - line_length)
#define DSC_END  (data_offset + data_index)

#define CDSC_PROPAGATE	10
#define CDSC_NEEDMORE 11
static const int dsc_severity[] = {
    CDSC_ERROR_WARN, 	/* CDSC_MESSAGE_BBOX */
    CDSC_ERROR_WARN, 	/* CDSC_MESSAGE_EARLY_TRAILER */
    CDSC_ERROR_WARN, 	/* CDSC_MESSAGE_EARLY_EOF */
    CDSC_ERROR_ERROR, 	/* CDSC_MESSAGE_PAGE_IN_TRAILER */
    CDSC_ERROR_ERROR, 	/* CDSC_MESSAGE_PAGE_ORDINAL */
    CDSC_ERROR_ERROR, 	/* CDSC_MESSAGE_PAGES_WRONG */
    CDSC_ERROR_ERROR, 	/* CDSC_MESSAGE_EPS_NO_BBOX */
    CDSC_ERROR_ERROR, 	/* CDSC_MESSAGE_EPS_PAGES */
    CDSC_ERROR_WARN, 	/* CDSC_MESSAGE_NO_MEDIA */
    CDSC_ERROR_WARN, 	/* CDSC_MESSAGE_ATEND */
    CDSC_ERROR_INFORM, 	/* CDSC_MESSAGE_DUP_COMMENT */
    CDSC_ERROR_INFORM, 	/* CDSC_MESSAGE_DUP_TRAILER */
    CDSC_ERROR_WARN, 	/* CDSC_MESSAGE_BEGIN_END */
    CDSC_ERROR_INFORM, 	/* CDSC_MESSAGE_BAD_SECTION */
    CDSC_ERROR_INFORM,  /* CDSC_MESSAGE_LONG_LINE */
    CDSC_ERROR_WARN, 	/* CDSC_MESSAGE_INCORRECT_USAGE */
    0
};

#define DSC_MAX_ERROR ((sizeof(dsc_severity) / sizeof(int))-2)

const CDSCMEDIA dsc_known_media[CDSC_KNOWN_MEDIA] = {
    /* These sizes taken from Ghostscript gs_statd.ps */
    {"11x17", 792, 1224, 0, NULL, NULL},
    {"A3", 842, 1190, 0, NULL, NULL},
    {"A4", 595, 842, 0, NULL, NULL},
    {"A5", 421, 595, 0, NULL, NULL},
    {"B4", 709, 1002, 0, NULL, NULL}, /* ISO, but not Adobe standard */
    {"B5", 501, 709, 0, NULL, NULL},  /* ISO, but not Adobe standard */
    {"Ledger", 1224, 792, 0, NULL, NULL},
    {"Legal", 612, 1008, 0, NULL, NULL},
    {"Letter", 612, 792, 0, NULL, NULL},
    {"Note", 612, 792, 0, NULL, NULL},
    {NULL, 0, 0, 0, NULL, NULL}
};

enum CDSC_SCAN_SECTION {
    scan_none = 0,
    scan_comments = 1,
    scan_pre_preview = 2,
    scan_preview = 3,
    scan_pre_defaults = 4,
    scan_defaults = 5,
    scan_pre_prolog = 6,
    scan_prolog = 7,
    scan_pre_setup = 8,
    scan_setup = 9,
    scan_pre_pages = 10,
    scan_pages = 11,
    scan_pre_trailer = 12,
    scan_trailer = 13,
    scan_eof = 14
};

static const char * const dsc_scan_section_name[15] = {
 "Type", "Comments", 
 "pre-Preview", "Preview",
 "pre-Defaults", "Defaults",
 "pre-Prolog", "Prolog",
 "pre-Setup", "Setup",
 "pre-Page", "Page",
 "pre-Trailer", "Trailer",
 "EOF"
};


XWPSDSCParser::XWPSDSCParser()
	:XWPSStruct()
{
	dsc = 0;
  ctrld = 0;
  pjl = 0;
  epsf = 0;	
  pdf = 0;
  preview = 0;	
  dsc_version = 0;
  language_level = 0;
  document_data = 0;	
  begincomments = 0;
  endcomments = 0;
  beginpreview = 0;
  endpreview = 0;
  begindefaults = 0;
  enddefaults = 0;
  beginprolog = 0;
  endprolog = 0;
  beginsetup = 0;
  endsetup = 0;
  begintrailer = 0;
  endtrailer = 0;
  
  page = 0;
  page_count = 0;
  page_pages = 0;
  page_order = 0;
  page_orientation = 0;
  viewer_orientation = 0;
  media_count = 0;	
  media  = 0;
  page_media = 0;
  bbox = 0;	
  page_bbox = 0;
  doseps = 0;	
  dsc_title = 0;
  dsc_creator = 0;
  dsc_date = 0;
  dsc_for = 0;

  max_error = 0;	
  severity = 0;	
  caller_data = 0;	
  id = 0;		
  scan_section = 0;	
  doseps_end = 0;	
  page_chunk_length = 0;
  file_length = 0;
  skip_document = 0;
  skip_bytes = 0;	
  skip_lines = 0;
  skip_pjl = 0;	
  begin_font_count = 0;
  begin_feature_count = 0;
  begin_resource_count = 0;	
  begin_procset_count = 0;
  memset(data, 0, CDSC_DATA_LENGTH);
  data_length = 0;
  data_index = 0;
  data_offset = 0;
  eof = 0;
  line = 0;
  line_length = 0;
  eol = 0;
  last_cr = 0;
  line_count = 0;
  long_line = 0;
  memset(last_line, 0, 256);
  string_head = 0;
  string = 0;
  dsc_error_fn = &XWPSDSCParser::errorHandler;
  init2();
}

XWPSDSCParser::~XWPSDSCParser()
{
	if (page)
	{
		for (int i=0; i < page_count; i++) 
  	{
			if (page[i].bbox)
	    	free(page[i].bbox);
			if (page[i].viewer_orientation)
	    	free(page[i].viewer_orientation);
  	}
		
		free(page);
		page = 0;
	}
	
	if (media)
	{
		for (int i=0; i < media_count; i++) 
		{
	    if (media[i]) 
	    {
				if (media[i]->mediabox)
		    	free(media[i]->mediabox);
				free(media[i]);
	    }
		}
		free(media);
		media = 0;
	}
	
	if (bbox)
		free(bbox);
		
	if (page_bbox)
		free(page_bbox);
		
  if (doseps)
		free(doseps);
		
	string = string_head;
  while (string != (CDSCSTRING *)NULL) 
  {
		if (string->data)
	    free(string->data);
		string_head = string;
		string = string->next;
		free(string_head);
  }
}

int XWPSDSCParser::addMedia(CDSCMEDIA *mediaA)
{
	CDSCMEDIA **newmedia_array;
  CDSCMEDIA *newmedia;

  newmedia_array = (CDSCMEDIA **)malloc((media_count + 1) * sizeof(CDSCMEDIA *));
  if (newmedia_array == NULL)
		return CDSC_ERROR;
  
  if (media != NULL) 
  {
		memcpy(newmedia_array, media, media_count * sizeof(CDSCMEDIA *));
		free(media);
  }
  media = newmedia_array;

  newmedia = media[media_count] = (CDSCMEDIA *)malloc(sizeof(CDSCMEDIA));
  if (newmedia == NULL)
		return CDSC_ERROR;
  newmedia->name = NULL;
  newmedia->width = 595.0;
  newmedia->height = 842.0;
  newmedia->weight = 80.0;
  newmedia->colour = NULL;
  newmedia->type = NULL;
  newmedia->mediabox = NULL;

  media_count++;

  if (mediaA->name) 
  {
		newmedia->name = new char[strlen(mediaA->name)] ;
		if (newmedia->name == NULL)
	    return CDSC_ERROR;
  }
  newmedia->width = mediaA->width;
  newmedia->height = mediaA->height;
  newmedia->weight = mediaA->weight;
  if (mediaA->colour) 
  {
		newmedia->colour = allocString(mediaA->colour, strlen(mediaA->colour));
    if (newmedia->colour == NULL)
	    return CDSC_ERROR;
  }
  if (mediaA->type) 
  {
		newmedia->type = allocString(mediaA->type, strlen(mediaA->type));
		if (newmedia->type == NULL)
	    return CDSC_ERROR;
  }
  
  newmedia->mediabox = NULL;

  if (mediaA->mediabox) 
  {
		newmedia->mediabox = (CDSCBBOX *)malloc(sizeof(CDSCBBOX));
		if (newmedia->mediabox == NULL)
	    return CDSC_ERROR;
		*newmedia->mediabox = *mediaA->mediabox;
  }
  return CDSC_OK;
}

int XWPSDSCParser::addPage(int ordinal, char *label)
{
	page[page_count].begin = 0;
  page[page_count].end = 0;
  page[page_count].label = allocString(label, strlen(label)+1);
  page[page_count].ordinal = ordinal;
  page[page_count].media = NULL;
  page[page_count].bbox = NULL;

  page_count++;
  if (page_count >= page_chunk_length) 
  {
		CDSCPAGE *new_page = (CDSCPAGE *)malloc((CDSC_PAGE_CHUNK+page_count) * sizeof(CDSCPAGE));
		if (new_page == NULL)
	    return CDSC_ERROR;
		memcpy(new_page, page,  page_count * sizeof(CDSCPAGE));
		free(page);
		page= new_page;
		page_chunk_length = CDSC_PAGE_CHUNK+page_count;
  }
  return CDSC_OK;
}

int XWPSDSCParser::errorHandler(void *, 
	                unsigned int ,
		              const char *, 
		              unsigned int )
{
	return CDSC_OK;
}

int XWPSDSCParser::getLength()
{
	return sizeof(XWPSDSCParser);
}

const char * XWPSDSCParser::getTypeName()
{
	return "dscparser";
}

int  XWPSDSCParser::scanData(const char *dataA, int length)
{
	int bytes_read;
  int code = 0;

  if (id == CDSC_NOTDSC)
		return CDSC_NOTDSC;
		
  id = CDSC_OK;
  if (eof)
		return CDSC_OK;	

  if (length == 0) 
		eof = TRUE;

  do 
 	{
		if (id == CDSC_NOTDSC)
	    break;

		if (length != 0) 
		{
	    if (data_length > CDSC_DATA_LENGTH/2) 
	    {
				memmove(data, data + data_index,  data_length - data_index);
				data_offset += data_index;
				data_length -= data_index;
				data_index = 0;
	    }
	    
	    bytes_read = qMin(length, (int)(CDSC_DATA_LENGTH - data_length));
	    memcpy(data + data_length, dataA, bytes_read);
	    data_length += bytes_read;
	    dataA += bytes_read;
	    length -= bytes_read;
		}
		if (scan_section == scan_none) 
		{
	    code = scanType();
	    if (code == CDSC_NEEDMORE) 
	    {
				code = CDSC_OK;
				break;
	    }
	    id = code;
		}

    if (code == CDSC_NOTDSC) 
    {
	    id = CDSC_NOTDSC;
	    break;
		}

		while ((code = readLine()) > 0) 
		{
	    if (id == CDSC_NOTDSC)
				break;
	    if (doseps_end && (data_offset + data_index > doseps_end)) 
				return CDSC_OK;	
	    if (eof)
				return CDSC_OK;
	    if (skip_document)
				continue;	
	    if (skip_lines)
				continue;
	    if (IS_DSC(line, "%%BeginData:"))
				continue;
	    if (IS_DSC(line, "%%BeginBinary:"))
				continue;
	    if (IS_DSC(line, "%%EndDocument"))
				continue;
	    if (IS_DSC(line, "%%EndData"))
				continue;
	    if (IS_DSC(line, "%%EndBinary"))
				continue;

	    do 
	    {
				switch (scan_section) 
				{
		    		case scan_comments:
							code = scanComments();
							break;
							
		    		case scan_pre_preview:
		    		case scan_preview:
							code = scanPreview();
							break;
							
		    		case scan_pre_defaults:
		    		case scan_defaults:
							code = scanDefaults();
							break;
							
		    		case scan_pre_prolog:
		    		case scan_prolog:
							code = scanProlog();
							break;
							
		    		case scan_pre_setup:
		    		case scan_setup:
							code = scanSetup();
							break;
							
		    		case scan_pre_pages:
		    		case scan_pages:
							code = scanPage();
							break;
							
		    		case scan_pre_trailer:
		    		case scan_trailer:
							code = scanTrailer();
							break;
							
		    		case scan_eof:
							code = CDSC_OK;
							break;
							
		    		default:
							code = CDSC_ERROR;
				}
	    } while (code == CDSC_PROPAGATE);

	    if (code == CDSC_NEEDMORE) 
	    {
				code = CDSC_OK;
				break;
	    }
	    if (code == CDSC_NOTDSC) 
	    {
				id = CDSC_NOTDSC;
				break;
	    }
		}
  } while (length != 0);

  return (code < 0) ? code : id;
}

void XWPSDSCParser::setErrorFunction(int (XWPSDSCParser::*fn)(void *, uint , const char *, uint ))
{
	dsc_error_fn = fn;
}

int  XWPSDSCParser::setPageBbox(uint page_number, int llx, int lly, int urx, int ury)
{
	CDSCBBOX *bbox;
  if (page_number >= page_count)
		return CDSC_ERROR;
  
  bbox = page[page_number].bbox;
  if (bbox == NULL)
		page[page_number].bbox = bbox = (CDSCBBOX *)malloc(sizeof(CDSCBBOX));
  if (bbox == NULL)
		return CDSC_ERROR;
  bbox->llx = llx;
  bbox->lly = lly;
  bbox->urx = urx;
  bbox->ury = ury;
  return CDSC_OK;
}

char * XWPSDSCParser::addLine(const char *lineA, unsigned int len)
{
	char *newline;
  unsigned int i;
  while (len && (IS_WHITE(*lineA))) 
  {
		len--;
		lineA++;
  }
  newline = allocString(lineA, len);
  if (newline == NULL)
		return NULL;

  for (i=0; i<len; i++) 
  {
		if (newline[i] == '\r') 
		{
	    newline[i]='\0';
	    break;
		}
		if (newline[i] == '\n') 
		{
	    newline[i]='\0';
	    break;
		}
  }
  return newline;
}

char * XWPSDSCParser::allocString(const char *str, int len)
{
	char *p;
  if (string_head == NULL) 
  {
		string_head = (CDSCSTRING *)malloc(sizeof(CDSCSTRING));
		if (string_head == NULL)
	    return NULL;
		string = string_head;
		string->next = NULL;
		string->data = (char *)malloc(CDSC_STRING_CHUNK);
		if (string->data == NULL) 
		{
	    reset();
	    return NULL;
		}
		string->index = 0;
		string->length = CDSC_STRING_CHUNK;
  }
  if (string->index + len + 1 > string->length) 
  {
		CDSCSTRING *newstring = (CDSCSTRING *)malloc(sizeof(CDSCSTRING));
		if (newstring == NULL) 
	    return NULL;
    newstring->next = NULL;
		newstring->length = 0;
		newstring->index = 0;
		newstring->data = (char *)malloc(CDSC_STRING_CHUNK);
		if (newstring->data == NULL) 
		{
	    free(newstring);
	    return NULL;
		}
		newstring->length = CDSC_STRING_CHUNK;
		string->next = newstring;
		string = newstring;
  }
  
  if (string->index + len + 1 > string->length)
		return NULL;
  
  p = string->data + string->index;
  memcpy(p, str, len);
  *(p+len) = '\0';
  string->index += len + 1;
  return p;
}

int XWPSDSCParser::checkMatch()
{
	int rc = 0;
  const char *font = "Font";
  const char *feature = "Feature";
  const char *resource = "Resource";
  const char *procset = "ProcSet";

  if (!rc)
		rc = checkMatchType(font, begin_font_count);
  if (!rc)
		rc = checkMatchType(feature, begin_feature_count);
  if (!rc)
		rc = checkMatchType(resource, begin_resource_count);
  if (!rc)
		rc = checkMatchType(procset, begin_procset_count);

  begin_font_count = 0;
  begin_feature_count = 0;
  begin_resource_count = 0;
  begin_procset_count = 0;
  return rc;
}

int XWPSDSCParser::checkMatchPrompt(const char *str, int count)
{
	if (count != 0) 
	{
		char buf[MAXSTR+MAXSTR];
		if (line_length < (unsigned int)(sizeof(buf)/2-1))  
		{
	    strncpy(buf, line, line_length);
	    buf[line_length] = '\0';
		}
		sprintf(buf+strlen(buf), "\n%%%%Begin%.40s: / %%%%End%.40s\n", str, str);
		return error(CDSC_MESSAGE_BEGIN_END, buf, strlen(buf));
  }
  return CDSC_RESPONSE_CANCEL;
}

int XWPSDSCParser::checkMatchType(const char *str, int count)
{
	if (checkMatchPrompt(str, count) == CDSC_RESPONSE_IGNORE_ALL)
		return CDSC_NOTDSC;
  return CDSC_OK;
}

char * XWPSDSCParser::copyString(char *str, 
	                  unsigned int slen, 
	                  char *lineA, 
	                  unsigned int len, 
	                  unsigned int *offset)
{
	int quoted = false;
  int instring=0;
  unsigned int newlength = 0;
  unsigned int i = 0;
  unsigned char ch;
  if (len > slen)
		len = slen-1;
  while ( (i<len) && IS_WHITE(lineA[i]))
		i++;
  if (lineA[i]=='(') 
  {
		quoted = true;
		instring++;
		i++; 
  }
  while (i < len) 
  {
		str[newlength] = ch = lineA[i];
		i++;
		if (quoted) 
		{
	    if (ch == '(')
		    instring++;
	    if (ch == ')')
		    instring--;
	    if (instring==0)
		    break;
		}
		else if (ch == ' ')
	    break;

		if (ch == '\r')
	    break;
		if (ch == '\n')
	    break;
		else if ( (ch == '\\') && (i+1 < len) ) 
		{
	    ch = lineA[i];
	    if ((ch >= '0') && (ch <= '9')) 
	    {
				int j = 3;
				ch = 0;
				while (j && (i < len) && lineA[i]>='0' && lineA[i]<='7') 
				{
		    	ch = (unsigned char)((ch<<3) + (lineA[i]-'0'));
		    	i++;
		    	j--;
				}
				str[newlength] = ch;
	    }
	    else if (ch == '(') 
	    {
				str[newlength] = ch;
				i++;
	    }
	    else if (ch == ')') 
	    {
				str[newlength] = ch;
				i++;
	    }
	    else if (ch == 'b') 
	    {
				str[newlength] = '\b';
				i++;
	    }
	    else if (ch == 'f') 
	    {
				str[newlength] = '\b';
				i++;
	    }
	    else if (ch == 'n') 
	    {
				str[newlength] = '\n';
				i++;
	    }
	    else if (ch == 'r') 
	    {
				str[newlength] = '\r';
				i++;
	    }
	    else if (ch == 't') 
	    {
				str[newlength] = '\t';
				i++;
	    }
	    else if (ch == '\\') 
	    {
				str[newlength] = '\\';
				i++;
	    }
		}
		newlength++;
  }
  str[newlength] = '\0';
  if (offset != (unsigned int *)NULL)
     *offset = i;
  return str;
}

int XWPSDSCParser::dscStricmp(const char *s, const char *t)
{
	while (toupper(*s) == toupper(*t)) 
	{
		if (*s == '\0')
	    return 0;
   	s++;
		t++; 
  }
  return (toupper(*s) - toupper(*t));
}

int XWPSDSCParser::error(unsigned int explanation, char *lineA, unsigned int line_len)
{
	if (dsc_error_fn)
		(this->*dsc_error_fn)(caller_data, explanation, lineA, line_len);
  return CDSC_RESPONSE_CANCEL;
}

ulong XWPSDSCParser::getDWord(const unsigned char *buf)
{
	ulong dw;
  dw = (ulong)buf[0];
  dw += ((ulong)buf[1])<<8;
  dw += ((ulong)buf[2])<<16;
  dw += ((ulong)buf[3])<<24;
  return dw;
}

int XWPSDSCParser::getInt(const char *lineA, unsigned int len, unsigned int *offset)
{
	char newline[MAXSTR];
  int newlength = 0;
  unsigned int i = 0;
  unsigned char ch;

  len = qMin(len, sizeof(newline)-1);
  while ((i<len) && IS_WHITE(lineA[i]))
		i++;
  while (i < len) 
  {
		newline[newlength] = ch = lineA[i];
		if (!(isdigit(ch) || (ch=='-') || (ch=='+')))
	    break;  /* not part of an integer number */
		i++;
		newlength++;
  }
  while ((i<len) && IS_WHITE(lineA[i]))
		i++;
  newline[newlength] = '\0';
  if (offset != (unsigned int *)NULL)
     *offset = i;
  return atoi(newline);
}

float XWPSDSCParser::getReal(const char *lineA, unsigned int len, unsigned int *offset)
{
	char newline[MAXSTR];
  int newlength = 0;
  unsigned int i = 0;
  unsigned char ch;

  len = qMin(len, sizeof(newline)-1);
  while ((i<len) && IS_WHITE(lineA[i]))
		i++;
  while (i < len) 
  {
		newline[newlength] = ch = lineA[i];
		if (!(isdigit(ch) || (ch=='.') || (ch=='-') || (ch=='+')  || (ch=='e') || (ch=='E')))
	    break;
		i++;
		newlength++;
  }
  while ((i<len) && IS_WHITE(lineA[i]))
		i++;

  newline[newlength] = '\0';

  if (offset != (unsigned int *)NULL)
     *offset = i;
  return (float)atof(newline);
}

uint XWPSDSCParser:: getWord(const unsigned char *buf)
{
	uint w;
  w = (uint)buf[0];
  w |= (uint)(buf[1]<<8);
  return w;
}

void XWPSDSCParser::init2()
{
	reset();

  string_head = (CDSCSTRING *)malloc(sizeof(CDSCSTRING));
  string = string_head;
  string->next = NULL;
  string->data = (char *)malloc(CDSC_STRING_CHUNK);
  string->index = 0;
  string->length = CDSC_STRING_CHUNK;
	
  page = (CDSCPAGE *)malloc(CDSC_PAGE_CHUNK * sizeof(CDSCPAGE));
  page_chunk_length = CDSC_PAGE_CHUNK;
  page_count = 0;
	
  line = NULL;
  data_length = 0;
  data_index = data_length;
}

bool XWPSDSCParser::isSection(char *lineA)
{
	if ( !((lineA[0]=='%') && (lineA[1]=='%')) )
		return false;
  if (IS_DSC(lineA, "%%BeginPreview"))
		return true;
  if (IS_DSC(lineA, "%%BeginDefaults"))
		return true;
  if (IS_DSC(lineA, "%%BeginProlog"))
		return true;
  if (IS_DSC(lineA, "%%BeginSetup"))
		return true;
  if (IS_DSC(lineA, "%%Page:"))
		return true;
  if (IS_DSC(lineA, "%%Trailer"))
		return true;
  if (IS_DSC(lineA, "%%EOF"))
		return true;
  return false;
}

int  XWPSDSCParser::parseBoundingBox(CDSCBBOX** pbbox, int offset)
{
	unsigned int i, n;
  int llx, lly, urx, ury;
  float fllx, flly, furx, fury;
  char *p;
  
  if ((*pbbox != NULL) && (scan_section == scan_comments)) 
  {
		int rc = error(CDSC_MESSAGE_DUP_COMMENT, line, line_length);
		switch (rc) 
		{
	    case CDSC_RESPONSE_OK:
	    case CDSC_RESPONSE_CANCEL:
				return CDSC_OK;	
	    case CDSC_RESPONSE_IGNORE_ALL:
				return CDSC_NOTDSC;
		}
  }
  
  if ((*pbbox != NULL) && (scan_section == scan_pages)) 
  {
		int rc = error(CDSC_MESSAGE_DUP_COMMENT, line, line_length);
		switch (rc) 
		{
	    case CDSC_RESPONSE_OK:
	    case CDSC_RESPONSE_CANCEL:
				return CDSC_OK;
	    case CDSC_RESPONSE_IGNORE_ALL:
				return CDSC_NOTDSC;
		}
  }
  if ((*pbbox != NULL) && (scan_section == scan_trailer)) 
  {
		int rc = error(CDSC_MESSAGE_DUP_TRAILER, line, line_length);
		switch (rc) 
		{
	    case CDSC_RESPONSE_OK:
	    case CDSC_RESPONSE_CANCEL:
				break;
	    case CDSC_RESPONSE_IGNORE_ALL:
				return CDSC_NOTDSC;
		}
  }
  if (*pbbox != NULL) 
  {
		free(*pbbox);
		*pbbox = NULL;
  }

  while (IS_WHITE(line[offset]))
		offset++;
  p = line + offset;
    
  if (COMPARE(p, "atend")) 
  {
		int rc = error(CDSC_MESSAGE_ATEND, line, line_length);
		switch (rc) 
		{
	    case CDSC_RESPONSE_OK:
				break;
	    case CDSC_RESPONSE_CANCEL:
				break;
	    case CDSC_RESPONSE_IGNORE_ALL:
				return CDSC_NOTDSC;
		}
  }
  else if (COMPARE(p, "(atend)")) 
  {
  }
  else 
  {
  	lly = urx = ury = 0;
		n = offset;
		llx = getInt(line+n, line_length-n, &i);
		n += i;
		if (i)
	    lly = getInt(line+n, line_length-n, &i);
		n += i;
		if (i)
	    urx = getInt(line+n, line_length-n, &i);
		n += i;
		if (i)
	    ury = getInt(line+n, line_length-n, &i);
		if (i) 
		{
	    *pbbox = (CDSCBBOX *)malloc(sizeof(CDSCBBOX));
	    if (*pbbox == NULL)
				return CDSC_ERROR;
	    (*pbbox)->llx = llx;
	    (*pbbox)->lly = lly;
	    (*pbbox)->urx = urx;
	    (*pbbox)->ury = ury;
		}
		else 
		{
	    int rc = error(CDSC_MESSAGE_BBOX, line, line_length);
	    switch (rc) 
	    {
	      case CDSC_RESPONSE_OK:
					flly = furx = fury = 0.0;
					n = offset;
					n += i;
					fllx = getReal(line+n, line_length-n, &i);
					n += i;
					if (i)
		    		flly = getReal(line+n, line_length-n, &i);
					n += i;
					if (i)
		    		furx = getReal(line+n, line_length-n, &i);
					n += i;
					if (i)
		    		fury = getReal(line+n, line_length-n, &i);
					if (i) 
					{
		    		*pbbox = (CDSCBBOX *)malloc(sizeof(CDSCBBOX));
		    		if (*pbbox == NULL)
							return CDSC_ERROR;
						(*pbbox)->llx = (int)fllx;
						(*pbbox)->lly = (int)flly;
						(*pbbox)->urx = (int)(furx+0.999);
						(*pbbox)->ury = (int)(fury+0.999);
					}
					return CDSC_OK;
					
	    	case CDSC_RESPONSE_CANCEL:
					return CDSC_OK;
					
	    	case CDSC_RESPONSE_IGNORE_ALL:
					return CDSC_NOTDSC;
	  	}
		}
  }
  return CDSC_OK;
}

int  XWPSDSCParser::parseDocumentMedia()
{
	unsigned int i, n;
  CDSCMEDIA lmedia;
  bool blank_line;

  if (IS_DSC(line, "%%DocumentMedia:"))
		n = 16;
  else if (IS_DSC(line, "%%+"))
		n = 3;
  else
		return CDSC_ERROR;
		
  blank_line = true;
  for (i=n; i<line_length; i++) 
  {
		if (!IS_WHITE_OR_EOL(line[i])) 
		{
	    blank_line = false;
	    break;
		}
  }

  if (!blank_line) 
  {
		char name[MAXSTR];
		char colour[MAXSTR];
		char type[MAXSTR];
		lmedia.name = lmedia.colour = lmedia.type = (char *)NULL;
		lmedia.width = lmedia.height = lmedia.weight = 0;
		lmedia.mediabox = (CDSCBBOX *)NULL;
		lmedia.name = copyString(name, sizeof(name),line+n, line_length-n, &i);
		n+=i;
		if (i)
	    lmedia.width = getReal(line+n, line_length-n, &i);
		n+=i;
		if (i)
	    lmedia.height = getReal(line+n, line_length-n, &i);
		n+=i;
		if (i)
	    lmedia.weight = getReal(line+n, line_length-n, &i);
		n+=i;
		if (i)
	    lmedia.colour = copyString(colour, sizeof(colour), line+n, line_length-n, &i);
		n+=i;
		if (i)
	    lmedia.type = copyString(type, sizeof(type), line+n, line_length-n, &i);

		if (i==0)
	    unknown();
		else 
		{
	    if (addMedia(&lmedia))
				return CDSC_ERROR;
		}
  }
  return CDSC_OK;
}

int  XWPSDSCParser::parseMedia(const CDSCMEDIA **page_media)
{
	char media_name[MAXSTR];
  int n = IS_DSC(line, "%%+") ? 3 : 12; 
  unsigned int i;

  if (copyString(media_name, sizeof(media_name)-1, line+n, line_length-n, NULL)) 
  {
		for (i=0; i<media_count; i++) 
		{
	    if (media[i]->name && (dscStricmp(media_name, media[i]->name) == 0)) 
			{
				*page_media = media[i];
				return CDSC_OK;
	    }
		}
  }
  unknown();
  return CDSC_OK;
}

int  XWPSDSCParser::parseOrder()
{
	char *p;
  if ((page_order != CDSC_ORDER_UNKNOWN) && 
			(scan_section == scan_comments)) 
	{
		int rc = error(CDSC_MESSAGE_DUP_COMMENT, line, line_length);
		switch (rc) 
		{
	    case CDSC_RESPONSE_OK:
	    case CDSC_RESPONSE_CANCEL:
				return CDSC_OK;
				
	    case CDSC_RESPONSE_IGNORE_ALL:
				return CDSC_NOTDSC;
		}
  }
  
  if ((page_order != CDSC_ORDER_UNKNOWN) && 
			(scan_section == scan_trailer)) 
	{
		int rc = error(CDSC_MESSAGE_DUP_TRAILER, line, line_length);
		switch (rc) 
		{
	    case CDSC_RESPONSE_OK:
	    case CDSC_RESPONSE_CANCEL:
				break;
				
	    case CDSC_RESPONSE_IGNORE_ALL:
				return CDSC_NOTDSC;
		}
  }

  p = line + (IS_DSC(line, "%%+") ? 3 : 13);
  while (IS_WHITE(*p))
		p++;
  if (COMPARE(p, "atend")) 
  {
		int rc = error(CDSC_MESSAGE_ATEND, line, line_length);
		switch (rc) 
		{
	    case CDSC_RESPONSE_OK:
				break;
				
	    case CDSC_RESPONSE_CANCEL:
				break;
	    case CDSC_RESPONSE_IGNORE_ALL:
				return CDSC_NOTDSC;
		}
  }
  else if (COMPARE(p, "(atend)")) 
  {
  }
  else if (COMPARE(p, "Ascend")) 
		page_order = CDSC_ASCEND;
  else if (COMPARE(p, "Descend")) 
		page_order = CDSC_DESCEND;
  else if (COMPARE(p, "Special")) 
		page_order = CDSC_SPECIAL;
  else 
		unknown();
		
  return CDSC_OK;
}

int XWPSDSCParser::parseOrientation(unsigned int *porientation, int offset)
{
	char *p;
  if ((page_orientation != CDSC_ORIENT_UNKNOWN) && 
			(scan_section == scan_comments)) 
	{
		int rc = error(CDSC_MESSAGE_DUP_COMMENT, line, line_length);
		switch (rc) 
		{
	    case CDSC_RESPONSE_OK:
	    case CDSC_RESPONSE_CANCEL:
				return CDSC_OK;
				
	    case CDSC_RESPONSE_IGNORE_ALL:
				return CDSC_NOTDSC;
		}
  }
  
  if ((page_orientation != CDSC_ORIENT_UNKNOWN) && 
			(scan_section == scan_trailer)) 
	{
		int rc = error(CDSC_MESSAGE_DUP_TRAILER, line, line_length);
		switch (rc) 
		{
	    case CDSC_RESPONSE_OK:
	    case CDSC_RESPONSE_CANCEL:
				break;
				
	    case CDSC_RESPONSE_IGNORE_ALL:
				return CDSC_NOTDSC;		
		}
  }
  p = line + offset;
  while (IS_WHITE(*p))
		p++;
  if (COMPARE(p, "atend")) 
  {
		int rc = error(CDSC_MESSAGE_ATEND, line, line_length);
		switch (rc) 
		{
	    case CDSC_RESPONSE_OK:
				break;
				
	    case CDSC_RESPONSE_CANCEL:
				break;
				
	    case CDSC_RESPONSE_IGNORE_ALL:
				return CDSC_NOTDSC;
		}
  }
  else if (COMPARE(p, "(atend)")) 
  {
  }
  else if (COMPARE(p, "Portrait")) 
		*porientation = CDSC_PORTRAIT;
  else if (COMPARE(p, "Landscape")) 
		*porientation = CDSC_LANDSCAPE;
  else 
		unknown();
  return CDSC_OK;
}

int XWPSDSCParser:: parsePage()
{
	char *p;
  unsigned int i;
  char page_label[MAXSTR];
  char *pl;
  int page_ordinal;
  int page_number;

  p = line + 7;
  pl = copyString(page_label, sizeof(page_label), p, line_length-7, &i);
  if (pl == NULL)
		return CDSC_ERROR;
  p += i;
  page_ordinal = atoi(p);

  if ( (page_ordinal == 0) || (strlen(page_label) == 0) ||
       (page_count &&  (page_ordinal != page[page_count-1].ordinal+1)) ) 
  {
		int rc = error(CDSC_MESSAGE_PAGE_ORDINAL, line, line_length);
		switch (rc) 
		{
	    case CDSC_RESPONSE_OK:
				return CDSC_OK;
				
	    case CDSC_RESPONSE_CANCEL:
				break;
				
	    case CDSC_RESPONSE_IGNORE_ALL:
				return CDSC_NOTDSC;
		}
  }

  page_number = page_count;
  addPage(page_ordinal, page_label);
  page[page_number].begin = DSC_START;
  page[page_number].end = DSC_START;

  if (page[page_number].label == NULL)
		return CDSC_ERROR;
	
  return CDSC_OK;
}

int  XWPSDSCParser::parsePages()
{
	int ip, io; 
  unsigned int i;
  char *p;
  int n;
  if ((page_pages != 0) && (scan_section == scan_comments)) 
  {
		int rc = error(CDSC_MESSAGE_DUP_COMMENT, line, line_length);
		switch (rc) 
		{
	    case CDSC_RESPONSE_OK:
	    case CDSC_RESPONSE_CANCEL:
				return CDSC_OK;	
	    case CDSC_RESPONSE_IGNORE_ALL:
				return CDSC_NOTDSC;
		}
  }
  
  if ((page_pages != 0) && (scan_section == scan_trailer)) 
  {
		int rc = error(CDSC_MESSAGE_DUP_TRAILER, line, line_length);
		switch (rc) 
		{
	    case CDSC_RESPONSE_OK:
	    case CDSC_RESPONSE_CANCEL:
				break;
	    case CDSC_RESPONSE_IGNORE_ALL:
				return CDSC_NOTDSC;
		}
  }

  n = IS_DSC(line, "%%+") ? 3 : 8;
  while (IS_WHITE(line[n]))
		n++;
  p = line + n;
  if (COMPARE(p, "atend")) 
  {
		int rc = error(CDSC_MESSAGE_ATEND, line, line_length);
		switch (rc) 
		{
	    case CDSC_RESPONSE_OK:
				break;
	    case CDSC_RESPONSE_CANCEL:
				break;
	    case CDSC_RESPONSE_IGNORE_ALL:
				return CDSC_NOTDSC;
		}
  }
  else if (COMPARE(p, "(atend)")) 
  {
  }
  else 
  {
		ip = getInt(line+n, line_length-n, &i);
     if (i) 
     {
	    	n+=i;
	    	page_pages = ip;
	    	io = getInt(line+n, line_length-n, &i);
	    	if (i) 
	    	{
					if (page_order == CDSC_ORDER_UNKNOWN)
		    		switch (io) 
		    		{
							case -1:
			    			page_order = CDSC_DESCEND;
			    			break;
			    			
							case 0:
			    			page_order = CDSC_SPECIAL;
			    			break;
			    			
							case 1:
			    			page_order = CDSC_ASCEND;
			    			break;
		    		}
	    	}
			}
			else 
			{
	    	int rc = error(CDSC_MESSAGE_INCORRECT_USAGE, line, line_length);
	    	switch (rc) 
	    	{
					case CDSC_RESPONSE_OK:
					case CDSC_RESPONSE_CANCEL:
		    		break;
					case CDSC_RESPONSE_IGNORE_ALL:
		    		return CDSC_NOTDSC;
	    	}
			}
  }
  return CDSC_OK;
}

int  XWPSDSCParser::parseViewerOrientation(CDSCCTM **pctm)
{
	CDSCCTM ctm;
  unsigned int i, n;

  if (*pctm != NULL) 
  {
		free(*pctm);
		*pctm = NULL;
  }

  n = IS_DSC(line, "%%+") ? 3 : 20;
  while (IS_WHITE(line[n]))
		n++;

  ctm.xy = ctm.yx = ctm.yy = 0.0;
  ctm.xx = getReal(line+n, line_length-n, &i);
  n += i;
  if (i)
    ctm.xy = getReal(line+n, line_length-n, &i);
  n += i;
  if (i)
    ctm.yx = getReal(line+n, line_length-n, &i);
  n += i;
  if (i)
     ctm.yy = getReal(line+n, line_length-n, &i);
  if (i==0) 
		unknown();
  else 
  {
		*pctm = (CDSCCTM *)malloc(sizeof(CDSCCTM));
		if (*pctm == NULL)
	    return CDSC_ERROR;
		**pctm = ctm;
  }
  return CDSC_OK;
}

int  XWPSDSCParser::readDoSeps()
{
	if ((doseps = (CDSCDOSEPS *)malloc(sizeof(CDSCDOSEPS))) == NULL)
		return CDSC_ERROR;
	
  doseps->ps_begin = getDWord((const unsigned char *)(line+4));
  doseps->ps_length = getDWord((const unsigned char *)(line+8));
  doseps->wmf_begin = getDWord((const unsigned char *)(line+12));
  doseps->wmf_length = getDWord((const unsigned char *)(line+16));
  doseps->tiff_begin = getDWord((const unsigned char *)(line+20));
  doseps->tiff_length = getDWord((const unsigned char *)(line+24));
  doseps->checksum = getWord((const unsigned char *)(line+28));
	
  doseps_end = doseps->ps_begin + doseps->ps_length;

  data_index -= line_length - 30;
  line_count = 0;
  skip_bytes = doseps->ps_begin - 30;

  return CDSC_OK;
}

int  XWPSDSCParser::readLine()
{
	char *p, *last;
  line = NULL;

  if (eof) 
  {
		line = data + data_index;
		line_length = data_length - data_index;
		data_index = data_length;
		return line_length;
  }

  if (skip_bytes) 
  {
		int cnt = qMin(skip_bytes, (int)(data_length - data_index));
		skip_bytes -= cnt;
		data_index += cnt;
		if (skip_bytes != 0)
	    return 0;
  }

  do 
  {
		line = data + data_index;
		last = data + data_length;
		if (data_index == data_length) 
		{
	    line_length = 0;
	    return 0;
		}
		if (eol) 
		{
	    line_count++;
	    if (skip_lines)
				skip_lines--;
		}
	  
		if (last_cr && line[0] == '\n') 
		{
	    data_index++;
	    line++;
		}
		last_cr = false;
		eol = false;
		for (p = line; p < last; p++) 
		{
	    if (*p == '\r') 
	    {
				p++;
				if ((p<last) && (*p == '\n'))
		    	p++;
				else
		    	last_cr = true;
				eol = true;
				break;
	    }
	    if (*p == '\n') 
	    {
				p++;
				eol = true;
				break;
	    }
	    if (*p == '\032') 
				eol = true;
		}
		if (eol == false) 
		{
	    if (data_length - data_index < sizeof(data)/2) 
	    {
				line_length = 0;
				return 0;
	    }
		}
		data_index += line_length = (p - line);
  } while (skip_lines && line_length);

  if (line_length == 0)
		return 0;
	
  if ((line[0]=='%') && (line[1]=='%'))  
  {
		if ((skip_document) && line_length &&	COMPARE(line, "%%EndDocument")) 
	    skip_document--;

		if (COMPARE(line, "%%BeginData:")) 
		{
	    char begindata[MAXSTR+1];
	    int cnt;
	    const char *numberof, *bytesorlines;
	    memcpy(begindata, line, line_length);
	    begindata[line_length] = '\0';
	    numberof = strtok(begindata+12, " \r\n");
	    strtok(NULL, " \r\n");
	    bytesorlines = strtok(NULL, " \r\n");
	    if (bytesorlines == NULL)
				bytesorlines = "Bytes";
	   
	    if ( (numberof == NULL) || (bytesorlines == NULL) ) 
	    {
				int rc = error(CDSC_MESSAGE_INCORRECT_USAGE, line, line_length);
				switch (rc) 
				{
		    	case CDSC_RESPONSE_OK:
		    	case CDSC_RESPONSE_CANCEL:
						break;
		    	case CDSC_RESPONSE_IGNORE_ALL:
						return 0;
				}
	    }
	    else 
	    {
				cnt = atoi(numberof);
				if (cnt) 
				{
		    	if (bytesorlines && (dscStricmp(bytesorlines, "Lines")==0)) 
		    	{
						if (skip_lines == 0) 
			    		skip_lines = cnt+1;
		    	}
		    	else 
		    	{
						if (skip_bytes == 0) 
			    		skip_bytes = cnt;
		    	}
				}
	    }
		}
		else if (COMPARE(line, "%%BeginBinary:")) 
		{
	    unsigned long cnt = atoi(line + 14);
	    if (skip_bytes == 0) 
				skip_bytes = cnt;
		}
  }
	
  if ((line[0]=='%') && (line[1]=='%') &&	COMPARE(line, "%%BeginDocument:")) 
		skip_document++;

  if (!eol && !long_line && (data_length - data_index)>DSC_LINE_LENGTH) 
  {
		error(CDSC_MESSAGE_LONG_LINE, line, line_length);
    long_line = TRUE;
  }
	
  return line_length;
}

void XWPSDSCParser::reset()
{
	unsigned int i;
  id = CDSC_OK;
  dsc = false;
  ctrld = false;
  pjl = false;
  epsf = false;
  pdf = false;
  epsf = false;
  preview = CDSC_NOPREVIEW;
  language_level = 0;
  document_data = CDSC_DATA_UNKNOWN;
  dsc_version = NULL;
  begincomments = 0;
  endcomments = 0;
  beginpreview = 0;
  endpreview = 0;
  begindefaults = 0;
  enddefaults = 0;
  beginprolog = 0;
  endprolog = 0;
  beginsetup = 0;
  endsetup = 0;
  begintrailer = 0;
  endtrailer = 0;
	
  for (i=0; i < page_count; i++) 
  {
		if (page[i].bbox)
	    free(page[i].bbox);
		if (page[i].viewer_orientation)
	    free(page[i].viewer_orientation);
  }
  
  if (page)
		free(page);
  page = NULL;
	
  page_chunk_length = 0;
  page_count = 0;
  page_pages = 0;
  page_order = CDSC_ORDER_UNKNOWN;
  page_orientation = CDSC_ORIENT_UNKNOWN;
  if (viewer_orientation)
		free(viewer_orientation);
  
  viewer_orientation = NULL;
  page_media = NULL;

  if (media) 
  {
		for (i=0; i < media_count; i++) 
		{
	    if (media[i]) 
	    {
				if (media[i]->mediabox)
		    	free(media[i]->mediabox);
				free(media[i]);
	    }
		}
		free(media);
  }
  media = NULL;
  media_count = 0;
  if (bbox)
		free(bbox);
		
  bbox = NULL;
  if (page_bbox)
		free(page_bbox);
		
  page_bbox = NULL;
  if (doseps)
		free(doseps);
  doseps = NULL;
	
  doseps_end = 0;
  dsc_title = NULL;
  dsc_creator = NULL;
  dsc_date = NULL;
  dsc_for = NULL;
	
  file_length = 0;

  severity = dsc_severity;
  max_error = DSC_MAX_ERROR;
	
  skip_pjl = 0;
  scan_section = scan_none;
  skip_bytes = 0;
  skip_document = 0;
  skip_bytes = 0;
  skip_lines = 0;
  begin_font_count = 0;
  begin_feature_count = 0;
  begin_resource_count = 0;
  begin_procset_count = 0;

  string = string_head;
  while (string != (CDSCSTRING *)NULL) 
  {
		if (string->data)
	    free(string->data);
		string_head = string;
		string = string->next;
		free(string_head);
  }
  string_head = NULL;
  string = NULL;
	
  data_length = 0;
  data_index = 0;
  data_offset = 0;
  eof = 0;
	
  line = 0;
  line_length = 0;
  eol = 0;
  last_cr = FALSE;
  line_count = 1;
  long_line = FALSE;
}

void XWPSDSCParser::saveLine()
{
	int len = qMin(sizeof(last_line), line_length);
  memcpy(last_line, line, len);
}

int  XWPSDSCParser::scanComments()
{
	char *lineA = line;
  bool continued = false;
  id = CDSC_OK;
  if (IS_DSC(lineA, "%%EndComments")) 
  {
		id = CDSC_ENDCOMMENTS;
		endcomments = DSC_END;
		scan_section = scan_pre_preview;
		return CDSC_OK;
  }
  else if (IS_DSC(lineA, "%%BeginComments")) 
		id = CDSC_BEGINCOMMENTS;
  else if (isSection(lineA)) 
  {
		endcomments = DSC_START;
		scan_section = scan_pre_preview;
		return CDSC_PROPAGATE;
  }
  else if (lineA[0] == '%' && IS_WHITE_OR_EOL(lineA[1])) 
  {
		endcomments = DSC_START;
		scan_section = scan_pre_preview;
		return CDSC_PROPAGATE;
  }
  else if (lineA[0] != '%') 
  {
		id = CDSC_OK;
		endcomments = DSC_START;
		scan_section = scan_pre_preview;
		return CDSC_PROPAGATE;
  }
  else if (IS_DSC(lineA, "%%Begin")) 
  {
		endcomments = DSC_START;
		scan_section = scan_pre_preview;
		return CDSC_PROPAGATE;
  }

  if (IS_DSC(lineA, "%%+")) 
  {
		lineA = last_line;
		continued = true;
  }
  else
		saveLine();

  if (IS_DSC(lineA, "%%Pages:")) 
  {
		id = CDSC_PAGES;
		if (parsePages() != 0)
	    return CDSC_ERROR;
  }
  else if (IS_DSC(lineA, "%%Creator:")) 
  {
		id = CDSC_CREATOR;
		dsc_creator = addLine(line+10, line_length-10);
		if (dsc_creator==NULL)
	    return CDSC_ERROR;
  }
  else if (IS_DSC(lineA, "%%CreationDate:")) 
  {
		id = CDSC_CREATIONDATE;
		dsc_date = addLine(line+15, line_length-15);
		if (dsc_date==NULL)
	    return CDSC_ERROR;
  }
  else if (IS_DSC(lineA, "%%Title:")) 
  {
		id = CDSC_TITLE;
		dsc_title = addLine(line+8, line_length-8);
		if (dsc_title==NULL)
	    return CDSC_ERROR;
  }
  else if (IS_DSC(lineA, "%%For:")) 
  {
		id = CDSC_FOR;
		dsc_for = addLine(line+6, line_length-6);
		if (dsc_for==NULL)
	    return CDSC_ERROR;
  }
  else if (IS_DSC(lineA, "%%LanguageLevel:")) 
  {
		unsigned int n = continued ? 3 : 16;
		unsigned int i;
		int ll;
		id = CDSC_LANGUAGELEVEL;
		ll = getInt(line+n, line_length-n, &i);
		if (i) 
		{
	    if ( (ll==1) || (ll==2) || (ll==3) )
				language_level = ll;
	    else 
				unknown();
		}
		else 
	    unknown();
  }
  else if (IS_DSC(lineA, "%%BoundingBox:")) 
  {
		id = CDSC_BOUNDINGBOX;
		if (parseBoundingBox(&(bbox), continued ? 3 : 14))
	    return CDSC_ERROR;
  }
  else if (IS_DSC(lineA, "%%Orientation:")) 
  {
		id = CDSC_ORIENTATION;
		if (parseOrientation(&(page_orientation), continued ? 3 : 14))
	    return CDSC_ERROR;
  }
  else if (IS_DSC(lineA, "%%PageOrder:")) 
  {
		id = CDSC_PAGEORDER;
		if (parseOrder())
	    return CDSC_ERROR;
  }
  else if (IS_DSC(lineA, "%%DocumentMedia:")) 
  {
		id = CDSC_DOCUMENTMEDIA;
		if (parseDocumentMedia())
	    return CDSC_ERROR;
  }
  else if (IS_DSC(lineA, "%%DocumentPaperSizes:")) 
  {
		unsigned int n = continued ? 3 : 21;
		unsigned int count = 0;
		unsigned int i = 1;
		char name[MAXSTR];
		char *p;
		id = CDSC_DOCUMENTPAPERSIZES;
		while (i && (line[n]!='\r') && (line[n]!='\n')) 
		{
	    p = copyString(name, sizeof(name)-1,line+n, line_length-n, &i);
	    if (i && p) 
	    {
				const CDSCMEDIA *m = dsc_known_media;
				if (count >= media_count) 
				{
		    	CDSCMEDIA lmedia;
		    	lmedia.name = p;
		    	lmedia.width = 595.0;
		    	lmedia.height = 842.0;
		    	lmedia.weight = 80.0;
		    	lmedia.colour = NULL;
		    	lmedia.type = NULL;
		    	lmedia.mediabox = NULL;
		    	if (addMedia(&lmedia))
						return CDSC_ERROR;
				}
				else
		    	media[count]->name = allocString(p, strlen(p));
				while (m && m->name) 
				{
		    	if (dscStricmp(p, m->name)==0) 
		    	{
						media[count]->width = m->width;
						media[count]->height = m->height;
						break;
		    	}
		    	m++;
				}
	    }
	    n+=i;
	    count++;
		}
  }
  else if (IS_DSC(lineA, "%%DocumentPaperForms:")) 
  {
		unsigned int n = continued ? 3 : 21;
		unsigned int count = 0;
		unsigned int i = 1;
		char type[MAXSTR];
		char *p;
		id = CDSC_DOCUMENTPAPERFORMS;
		while (i && (line[n]!='\r') && (line[n]!='\n')) 
		{
	    p = copyString(type, sizeof(type)-1,line+n, line_length-n, &i);
	    if (i && p) 
	    {
				if (count >= media_count) 
				{
		    	CDSCMEDIA lmedia;
		    	lmedia.name = NULL;
		    	lmedia.width = 595.0;
		    	lmedia.height = 842.0;
		    	lmedia.weight = 80.0;
		    	lmedia.colour = NULL;
		    	lmedia.type = p;
		    	lmedia.mediabox = NULL;
		    	if (addMedia(&lmedia))
						return CDSC_ERROR;
				}
				else
		    	media[count]->type = allocString(p, strlen(p));
	    }
	    n+=i;
	    count++;
		}
  }
  else if (IS_DSC(lineA, "%%DocumentPaperColors:")) 
  {
		unsigned int n = continued ? 3 : 22;
		unsigned int count = 0;
		unsigned int i = 1;
		char colour[MAXSTR];
		char *p;
		id = CDSC_DOCUMENTPAPERCOLORS;
		while (i && (line[n]!='\r') && (line[n]!='\n')) 
		{
	    p = copyString(colour, sizeof(colour)-1, line+n, line_length-n, &i);
	    if (i && p) 
	    {
				if (count >= media_count) 
				{
		    	CDSCMEDIA lmedia;
		    	lmedia.name = NULL;
		    	lmedia.width = 595.0;
		    	lmedia.height = 842.0;
		    	lmedia.weight = 80.0;
		    	lmedia.colour = p;
		    	lmedia.type = NULL;
		    	lmedia.mediabox = NULL;
		    	if (addMedia(&lmedia))
						return CDSC_ERROR;
				}
				else
		    	media[count]->colour = allocString(p, strlen(p));
	    }
	    n+=i;
	    count++;
		}
  }
  else if (IS_DSC(lineA, "%%DocumentPaperWeights:")) 
  {
		unsigned int n = continued ? 3 : 23;
		unsigned int count = 0;
		unsigned int i = 1;
		float w;
		id = CDSC_DOCUMENTPAPERWEIGHTS;
		while (i && (line[n]!='\r') && (line[n]!='\n')) 
		{
	    w = getReal(line+n, line_length-n, &i);
	    if (i) 
	    {
				if (count >= media_count) 
				{
		    	CDSCMEDIA lmedia;
		    	lmedia.name = NULL;
		    	lmedia.width = 595.0;
		    	lmedia.height = 842.0;
		    	lmedia.weight = w;
		    	lmedia.colour = NULL;
		    	lmedia.type = NULL;
		    	lmedia.mediabox = NULL;
		    	if (addMedia(&lmedia))
						return CDSC_ERROR;
				}
				else
		    	media[count]->weight = w;
	    }
	    n+=i;
	    count++;
		}
  }
  else if (IS_DSC(lineA, "%%DocumentData:")) 
  {
		unsigned int n = continued ? 3 : 15;
		char *p = line + n;
		id = CDSC_DOCUMENTDATA;
		if (COMPARE(p, "Clean7Bit"))
	    document_data = CDSC_CLEAN7BIT;
		else if (COMPARE(p, "Clean8Bit"))
	    document_data = CDSC_CLEAN8BIT;
		else if (COMPARE(p, "Binary"))
	    document_data = CDSC_BINARY;
		else
	    unknown();
  }
  else if (IS_DSC(lineA, "%%Requirements:")) 
		id = CDSC_REQUIREMENTS;
  else if (IS_DSC(lineA, "%%DocumentNeededFonts:")) 
		id = CDSC_DOCUMENTNEEDEDFONTS;
  else if (IS_DSC(lineA, "%%DocumentSuppliedFonts:")) 
		id = CDSC_DOCUMENTSUPPLIEDFONTS;
  else if (line[0] == '%' && IS_WHITE_OR_EOL(line[1])) 
		id = CDSC_OK;
  else 
  {
		id = CDSC_UNKNOWNDSC;
		unknown();
  }

  endcomments = DSC_END;
  return CDSC_OK;
}

int  XWPSDSCParser::scanDefaults()
{
	char *lineA = line;
  id = CDSC_OK;

  if (scan_section == scan_pre_defaults) 
  {
		if (IS_BLANK(lineA))
	    return CDSC_OK;	
		else if (IS_DSC(lineA, "%%BeginDefaults")) 
		{
	    id = CDSC_BEGINDEFAULTS;
	    begindefaults = DSC_START;
	    enddefaults = DSC_END;
	    scan_section = scan_defaults;
	    return CDSC_OK;
		}
		else 
		{
	    scan_section = scan_pre_prolog;
	    return CDSC_PROPAGATE;
		}
  }

  if (NOT_DSC_LINE(lineA)) 
  {
  }
  else if (IS_DSC(lineA, "%%BeginPreview")) 
  {
  }
  else if (IS_DSC(lineA, "%%BeginDefaults")) 
  {
  }
  else if (isSection(lineA)) 
  {
		enddefaults = DSC_START;
		scan_section = scan_pre_prolog;
		return CDSC_PROPAGATE;
  }
  else if (IS_DSC(lineA, "%%EndDefaults")) 
  {
		id = CDSC_ENDDEFAULTS;
		enddefaults = DSC_END;
		scan_section = scan_pre_prolog;
		return CDSC_OK;
  }
  else if (IS_DSC(lineA, "%%PageMedia:")) 
  {
		id = CDSC_PAGEMEDIA;
		parseMedia(&page_media);
  }
  else if (IS_DSC(lineA, "%%PageOrientation:")) 
  {
		id = CDSC_PAGEORIENTATION;
		if (parseOrientation(&(page_orientation), 18))
	    return CDSC_ERROR;
  }
  else if (IS_DSC(lineA, "%%PageBoundingBox:")) 
  {
		id = CDSC_PAGEBOUNDINGBOX;
		if (parseBoundingBox(&(page_bbox), 18))
	    return CDSC_ERROR;
  }
  else if (IS_DSC(lineA, "%%ViewerOrientation:")) 
  {
		id = CDSC_VIEWERORIENTATION;
		if (parseViewerOrientation(&viewer_orientation))
	    return CDSC_ERROR;
  }
  else 
  {
		id = CDSC_UNKNOWNDSC;
		unknown();
  }
  enddefaults = DSC_END;
  return CDSC_OK;
}

int  XWPSDSCParser::scanPage()
{
	char *lineA = line;
  id = CDSC_OK;

  if (scan_section == scan_pre_pages) 
  {
		if (IS_DSC(lineA, "%%Page:")) 
	    scan_section = scan_pages;
		else  
		{
	    unsigned long *last;
	    if (endsetup != 0)
				last = &endsetup;
	    else if (endprolog != 0)
				last = &endprolog;
	    else if (enddefaults != 0)
				last = &enddefaults;
	    else if (endpreview != 0)
				last = &endpreview;
	    else if (endcomments != 0)
				last = &endcomments;
	    else
				last = &begincomments;
	    *last = DSC_START;
	    if (IS_DSC(lineA, "%%Trailer") || IS_DSC(lineA, "%%EOF")) 
	    {
				scan_section = scan_pre_trailer;
				return CDSC_PROPAGATE;
	    }
	    return CDSC_OK;
		}
  }

  if (NOT_DSC_LINE(lineA)) 
  {
  }
  else if (IS_DSC(lineA, "%%Page:")) 
  {
		id = CDSC_PAGE;
		if (page_count) 
		{
	    page[page_count-1].end = DSC_START;
	    if (checkMatch())
				return CDSC_NOTDSC;
		}

		if (parsePage() != 0)
	    return CDSC_ERROR;

		return CDSC_OK;
  }
  else if (IS_DSC(lineA, "%%BeginPreview")) 
  {
  }
  else if (IS_DSC(lineA, "%%BeginDefaults")) 
  {
  }
  else if (IS_DSC(lineA, "%%BeginProlog")) 
  {
  }
  else if (IS_DSC(lineA, "%%BeginSetup")) 
  {
  }
  else if (isSection(lineA)) 
  {
		if (IS_DSC(lineA, "%%Trailer")) 
		{
	    page[page_count-1].end = DSC_START;
	    if (file_length) 
	    {
				if ((!doseps && ((DSC_END + 32768) < file_length)) ||
		    	 ((doseps) && ((DSC_END + 32768) < doseps_end))) 
		   	{
		    	int rc = error(CDSC_MESSAGE_EARLY_TRAILER, line, line_length);
		    	switch (rc) 
		    	{
						case CDSC_RESPONSE_OK:
			    		break;
			    		
						case CDSC_RESPONSE_CANCEL:
			    		scan_section = scan_pre_trailer;
			    		if (checkMatch())
								return CDSC_NOTDSC;
			    		return CDSC_PROPAGATE;
						case CDSC_RESPONSE_IGNORE_ALL:
			    		return CDSC_NOTDSC;
		    	}
				}
	    }
	    else 
	    {
				scan_section = scan_pre_trailer;
				if (checkMatch())
		    	return CDSC_NOTDSC;
				return CDSC_PROPAGATE;
	    }
		}
		else if (IS_DSC(lineA, "%%EOF")) 
		{
	    page[page_count-1].end = DSC_START;
	    if (file_length) 
	    {
				if ((DSC_END+100 < file_length) || (doseps && (DSC_END + 100 < doseps_end))) 
				{
		    	int rc = error(CDSC_MESSAGE_EARLY_EOF, line, line_length);
		    	switch (rc) 
		    	{
						case CDSC_RESPONSE_OK:
			    		break;
			    		
						case CDSC_RESPONSE_CANCEL:
			    		scan_section = scan_eof;
			    		eof = true;
			    		if (checkMatch())
								return CDSC_NOTDSC;
			    		return CDSC_PROPAGATE;
			    		
						case CDSC_RESPONSE_IGNORE_ALL:
			    		return CDSC_NOTDSC;		
		    	}
				}
	    }
	    else 
	    {
				if (checkMatch())
		    	return CDSC_NOTDSC;
				return CDSC_OK;
	    }
		}
		else 
		{
	    int rc = error(CDSC_MESSAGE_BAD_SECTION, line, line_length);
	    if (rc == CDSC_RESPONSE_IGNORE_ALL)
				return CDSC_NOTDSC;
		}
  }
  else if (IS_DSC(lineA, "%%PageTrailer")) 
		id = CDSC_PAGETRAILER;
  else if (IS_DSC(lineA, "%%BeginPageSetup")) 
		id = CDSC_BEGINPAGESETUP;
  else if (IS_DSC(lineA, "%%EndPageSetup")) 
		id = CDSC_ENDPAGESETUP;
  else if (IS_DSC(lineA, "%%PageMedia:")) 
  {
		id = CDSC_PAGEMEDIA;
		parseMedia(&(page[page_count-1].media));
  }
  else if (IS_DSC(lineA, "%%PaperColor:")) 
		id = CDSC_PAPERCOLOR;
  else if (IS_DSC(lineA, "%%PaperForm:")) 
		id = CDSC_PAPERFORM;
  else if (IS_DSC(lineA, "%%PaperWeight:")) 
		id = CDSC_PAPERWEIGHT;
  else if (IS_DSC(lineA, "%%PaperSize:")) 
  {
    bool found_media = false;
		int i;
		int n = 12;
		char buf[MAXSTR];
		buf[0] = '\0';
		copyString(buf, sizeof(buf)-1, line+n, line_length-n, NULL);
 		for (i=0; i<(int)media_count; i++) 
 		{
	    if (media[i] && media[i]->name && (dscStricmp(buf, media[i]->name)==0)) 
	    {
				page_media = media[i];
				found_media = true;
				break;
	    }
		}
		if (!found_media) 
		{
	    const CDSCMEDIA *m = dsc_known_media;
	    while (m->name) 
	    {
				if (dscStricmp(buf, m->name)==0) 
				{
		    	page[page_count-1].media = m;
		    	break;
				}
				m++;
	    }
	    if (m->name == NULL)
				unknown();
		}
  }
  else if (IS_DSC(lineA, "%%PageOrientation:")) 
  {
		id = CDSC_PAGEORIENTATION;
		if (parseOrientation(&(page[page_count-1].orientation) ,18))
	    return CDSC_NOTDSC;
  }
  else if (IS_DSC(lineA, "%%PageBoundingBox:")) 
  {
		id = CDSC_PAGEBOUNDINGBOX;
		if (parseBoundingBox(&page[page_count-1].bbox, 18))
	    return CDSC_NOTDSC;
  }
  else if (IS_DSC(lineA, "%%ViewerOrientation:")) 
  {
		id = CDSC_VIEWERORIENTATION;
		if (parseViewerOrientation(&page[page_count-1].viewer_orientation))
	    return CDSC_ERROR;
  }
  else if (IS_DSC(lineA, "%%BeginFont:")) 
  {
		id = CDSC_BEGINFONT;
		begin_font_count++;
  }
  else if (IS_DSC(lineA, "%%EndFont")) 
  {
		id = CDSC_BEGINFONT;
		begin_font_count--;
  }
  else if (IS_DSC(lineA, "%%BeginFeature:")) 
  {
		id = CDSC_BEGINFEATURE;
		begin_feature_count++;
  }
  else if (IS_DSC(lineA, "%%EndFeature")) 
  {
		id = CDSC_ENDFEATURE;
		begin_feature_count--;
  }
  else if (IS_DSC(lineA, "%%BeginResource:")) 
  {
		id = CDSC_BEGINRESOURCE;
		begin_resource_count++;
  }
  else if (IS_DSC(lineA, "%%EndResource")) 
  {
		id = CDSC_ENDRESOURCE;
		begin_resource_count--;
  }
  else if (IS_DSC(lineA, "%%BeginProcSet:")) 
  {
		id = CDSC_BEGINPROCSET;
		begin_procset_count++;
  }
  else if (IS_DSC(lineA, "%%EndProcSet")) 
  {
		id = CDSC_ENDPROCSET;
		begin_procset_count--;
  }
  else if (IS_DSC(lineA, "%%IncludeFont:")) 
		id = CDSC_INCLUDEFONT;
  else 
  {
		id = CDSC_UNKNOWNDSC;
		unknown();
  }

  page[page_count-1].end = DSC_END;
  return CDSC_OK;
}

int  XWPSDSCParser::scanPreview()
{
	char *lineA = line;
  id = CDSC_OK;

  if (scan_section == scan_pre_preview) 
  {
		if (IS_BLANK(lineA))
	    return CDSC_OK;
		else if (IS_DSC(lineA, "%%BeginPreview")) 
		{
	    id = CDSC_BEGINPREVIEW;
	    beginpreview = DSC_START;
	    endpreview = DSC_END;
	    scan_section = scan_preview;
	    return CDSC_OK;
		}
		else 
		{
	    scan_section = scan_pre_defaults;
	    return CDSC_PROPAGATE;
		}
  }

  if (IS_DSC(lineA, "%%BeginPreview")) 
  {
  }
  else if (isSection(lineA)) 
  {
		endpreview = DSC_START;
		scan_section = scan_pre_defaults;
		return CDSC_PROPAGATE;
  }
  else if (IS_DSC(lineA, "%%EndPreview")) 
  {
		id = CDSC_ENDPREVIEW;
		endpreview = DSC_END;
		scan_section = scan_pre_defaults;
		return CDSC_OK;
  }
  else if (lineA[0] == '%' && lineA[1] != '%') 
  {
  }
  else 
  {
		id = CDSC_UNKNOWNDSC;
		unknown();
  }

  endpreview = DSC_END;
  return CDSC_OK;
}

int  XWPSDSCParser::scanProlog()
{
	char *lineA = line;
  id = CDSC_OK;

  if (scan_section == scan_pre_prolog) 
  {
    if (isSection(lineA) && (!IS_DSC(lineA, "%%BeginProlog"))) 
    {
	    scan_section = scan_pre_setup;
	    return CDSC_PROPAGATE;
		}
		id = CDSC_BEGINPROLOG;
		beginprolog = DSC_START;
		endprolog = DSC_END;
		scan_section = scan_prolog;
		if (IS_DSC(lineA, "%%BeginProlog"))
	    return CDSC_OK;
  }
   
  if (NOT_DSC_LINE(lineA)) 
  {
  }
  else if (IS_DSC(lineA, "%%BeginPreview")) 
  {
  }
  else if (IS_DSC(lineA, "%%BeginDefaults")) 
  {
  }
  else if (IS_DSC(lineA, "%%BeginProlog")) 
  {
  }
  else if (isSection(lineA)) 
  {
		endprolog = DSC_START;
		scan_section = scan_pre_setup;
		if (checkMatch())
	    return CDSC_NOTDSC;
		return CDSC_PROPAGATE;
  }
  else if (IS_DSC(lineA, "%%EndProlog")) 
  {
		id = CDSC_ENDPROLOG;
		endprolog = DSC_END;
		scan_section = scan_pre_setup;
		if (checkMatch())
	    return CDSC_NOTDSC;
		return CDSC_OK;
  }
  else if (IS_DSC(lineA, "%%BeginFont:")) 
  {
		id = CDSC_BEGINFONT;
		begin_font_count++;
  }
  else if (IS_DSC(lineA, "%%EndFont")) 
  {
		id = CDSC_ENDFONT;
		begin_font_count--;
  }
  else if (IS_DSC(lineA, "%%BeginFeature:")) 
  {
		id = CDSC_BEGINFEATURE;
		begin_feature_count++;
  }
  else if (IS_DSC(lineA, "%%EndFeature")) 
  {
		id = CDSC_ENDFEATURE;
		begin_feature_count--;
  }
  else if (IS_DSC(lineA, "%%BeginResource:")) 
  {
		id = CDSC_BEGINRESOURCE;
		begin_resource_count++;
  }
  else if (IS_DSC(lineA, "%%EndResource")) 
  {
		id = CDSC_ENDRESOURCE;
		begin_resource_count--;
  }
  else if (IS_DSC(lineA, "%%BeginProcSet:")) 
  {
		id = CDSC_BEGINPROCSET;
		begin_procset_count++;
  }
  else if (IS_DSC(lineA, "%%EndProcSet")) 
  {
		id = CDSC_ENDPROCSET;
		begin_procset_count--;
  }
  else 
  {
		id = CDSC_UNKNOWNDSC;
		unknown();
  }

  endprolog = DSC_END;
  return CDSC_OK;
}

int  XWPSDSCParser::scanSetup()
{
	char *lineA = line;
  id = CDSC_OK;

  if (scan_section == scan_pre_setup) 
  {
		if (IS_BLANK(lineA))
	    return CDSC_OK;
		else if (IS_DSC(lineA, "%%BeginSetup")) 
		{
	    id = CDSC_BEGINSETUP;
	    beginsetup = DSC_START;
	    endsetup = DSC_END;
	    scan_section = scan_setup;
	    return CDSC_OK;
		}
		else 
		{
	    scan_section = scan_pre_pages;
	    return CDSC_PROPAGATE;
		}
  }

  if (NOT_DSC_LINE(lineA)) 
  {
  }
  else if (IS_DSC(lineA, "%%BeginPreview")) 
  {
  }
  else if (IS_DSC(lineA, "%%BeginDefaults")) 
  {
  }
  else if (IS_DSC(lineA, "%%BeginProlog")) 
  {
  }
  else if (IS_DSC(lineA, "%%BeginSetup")) 
  {
  }
  else if (isSection(lineA)) 
  {
		endsetup = DSC_START;
		scan_section = scan_pre_pages;
		if (checkMatch())
	    return CDSC_NOTDSC;
		return CDSC_PROPAGATE;
  }
  else if (IS_DSC(lineA, "%%EndSetup")) 
  {
		id = CDSC_ENDSETUP;
		endsetup = DSC_END;
		scan_section = scan_pre_pages;
		if (checkMatch())
	    return CDSC_NOTDSC;
		return CDSC_OK;
  }
  else if (IS_DSC(lineA, "%%BeginFeature:")) 
  {
		id = CDSC_BEGINFEATURE;
		begin_feature_count++;
  }
  else if (IS_DSC(lineA, "%%EndFeature")) 
  {
		id = CDSC_ENDFEATURE;
		begin_feature_count--;
  }
  else if (IS_DSC(lineA, "%%Feature:")) 
		id = CDSC_FEATURE;
  else if (IS_DSC(lineA, "%%BeginResource:")) 
  {
		id = CDSC_BEGINRESOURCE;
		begin_resource_count++;
  }
  else if (IS_DSC(lineA, "%%EndResource")) 
  {
		id = CDSC_ENDRESOURCE;
		begin_resource_count--;
  }
  else if (IS_DSC(lineA, "%%PaperColor:")) 
		id = CDSC_PAPERCOLOR;
  else if (IS_DSC(lineA, "%%PaperForm:")) 
		id = CDSC_PAPERFORM;
  else if (IS_DSC(lineA, "%%PaperWeight:")) 
		id = CDSC_PAPERWEIGHT;
  else if (IS_DSC(lineA, "%%PaperSize:")) 
  {
    bool found_media = false;
		int i;
		int n = 12;
		char buf[MAXSTR];
		buf[0] = '\0';
		id = CDSC_PAPERSIZE;
		copyString(buf, sizeof(buf)-1, line+n, line_length-n, NULL);
 		for (i=0; i<(int)media_count; i++) 
 		{
	    if (media[i] && media[i]->name && (dscStricmp(buf, media[i]->name)==0)) 
	    {
				page_media = media[i];
				found_media = true;
				break;
	    }
		}
		if (!found_media) 
		{
	    const CDSCMEDIA *m = dsc_known_media;
	    while (m->name) 
	    {
				if (dscStricmp(buf, m->name)==0) 
				{
		    	page_media = m;
		    	break;
				}
				m++;
	    }
	    if (m->name == NULL)
				unknown();
		}
  }
  else 
  {
		id = CDSC_UNKNOWNDSC;
		unknown();
  }

  endsetup = DSC_END;
  return CDSC_OK;
}

int  XWPSDSCParser::scanTrailer()
{
	char *lineA = line;
  bool continued = false;
  id = CDSC_OK;

  if (scan_section == scan_pre_trailer) 
  {
		if (IS_DSC(lineA, "%%Trailer")) 
		{
	    id = CDSC_TRAILER;
	    begintrailer = DSC_START;
	    endtrailer = DSC_END;
	    scan_section = scan_trailer;
	    return CDSC_OK;
		}
		else if (IS_DSC(lineA, "%%EOF")) 
		{
	    id = CDSC_EOF;
	    begintrailer = DSC_START;
	    endtrailer = DSC_END;
	    scan_section = scan_trailer;
	    return CDSC_OK;
		}
		else 
		{
	    if (beginsetup)
				endsetup = DSC_END;
	    else if (beginprolog)
				endprolog = DSC_END;
	    else 
	    {
	    }
	    return CDSC_OK;
		}
  }
   
  if (IS_DSC(lineA, "%%+")) 
  {
		lineA = last_line;
		continued = true;
  }
  else
		saveLine();

  if (NOT_DSC_LINE(lineA)) 
  {
  }
  else if (IS_DSC(line, "%%EOF")) 
		id = CDSC_EOF;
  else if (IS_DSC(line, "%%Trailer")) 
  {
		id = CDSC_TRAILER;
		begintrailer = DSC_START;
  }
  else if (IS_DSC(lineA, "%%Pages:")) 
  {
		id = CDSC_PAGES;
		if (parsePages() != 0)
	    return CDSC_ERROR;
  }
  else if (IS_DSC(lineA, "%%BoundingBox:")) 
  {
		id = CDSC_BOUNDINGBOX;
		if (parseBoundingBox(&(bbox), continued ? 3 : 14))
	    return CDSC_ERROR;
  }
  else if (IS_DSC(lineA, "%%Orientation:")) 
  {
		id = CDSC_ORIENTATION;
		if (parseOrientation(&(page_orientation), continued ? 3 : 14))
	    return CDSC_ERROR;
  }
  else if (IS_DSC(lineA, "%%PageOrder:")) 
  {
		id = CDSC_PAGEORDER;
		if (parseOrder())
	    return CDSC_ERROR;
  }
  else if (IS_DSC(lineA, "%%DocumentMedia:")) 
  {
		id = CDSC_DOCUMENTMEDIA;
		if (parseDocumentMedia())
	    return CDSC_ERROR;
  }
  else if (IS_DSC(line, "%%Page:")) 
  {
		id = CDSC_PAGE;
		unknown();
		if (page_count) 
		{
	    int rc = error(CDSC_MESSAGE_PAGE_IN_TRAILER, line, line_length);
	    switch (rc) 
	    {
				case CDSC_RESPONSE_OK:
		    	scan_section = scan_pre_pages;
		    	if (page_count)
						page[page_count-1].end = DSC_START;
		    	break;
		    	
				case CDSC_RESPONSE_CANCEL:
		    	break;
				case CDSC_RESPONSE_IGNORE_ALL:
		    	return CDSC_NOTDSC;
	    }
		}
  }
  else if (IS_DSC(lineA, "%%DocumentNeededFonts:")) 
		id = CDSC_DOCUMENTNEEDEDFONTS;
  else if (IS_DSC(lineA, "%%DocumentSuppliedFonts:")) 
		id = CDSC_DOCUMENTSUPPLIEDFONTS;
  else 
  {
		id = CDSC_UNKNOWNDSC;
		unknown();
  }

  endtrailer = DSC_END;
  return CDSC_OK;
}

int  XWPSDSCParser::scanType()
{
	unsigned char *p;
  unsigned char *lineA = (unsigned char *)(data + data_index);
  int length = data_length - data_index;
  
  if (length == 0)
		return CDSC_NEEDMORE;

  if (skip_pjl) 
  {
		while (length >= 2) 
		{
	    while (length && !IS_EOL(lineA[0])) 
	    {
				lineA++;
				data_index++;
				length--;
	    }
	    while ((length >= 2) && IS_EOL(lineA[0]) && IS_EOL(lineA[1])) 
	    {
				lineA++;
				data_index++;
				length--;
	    }
	    if (length < 2)
				return CDSC_NEEDMORE;

	    if (IS_EOL(lineA[0]) && lineA[1]=='%') 
	    {
				lineA++;
				data_index++;
				length--;
				skip_pjl = false;
				break;
	    }
	    else 
	    {
				data_index++;
				return CDSC_NEEDMORE;
	    }
		}
		if (skip_pjl)
	    return CDSC_NEEDMORE;
  }

  if (length == 0)
		return CDSC_NEEDMORE;

  if (lineA[0] == '\004') 
  {
		lineA++;
		data_index++;
		length--;
		ctrld = true;
  }

  if (lineA[0] == '\033') 
  {
		if (length < 9)
	    return CDSC_NEEDMORE;
		if (COMPARE(lineA, "\033%-12345X")) 
		{
	    skip_pjl = true; 
	    pjl = true;
	    data_index += 9;
	    return scanType();
		}
  }

  if ((lineA[0]==0xc5) && (length < 4))
		return CDSC_NEEDMORE;
  if ((lineA[0]==0xc5) && (lineA[1]==0xd0) &&  (lineA[2]==0xd3) && (lineA[3]==0xc6) ) 
  {
		if (length < 30)
	    return CDSC_NEEDMORE;
		line = (char *)lineA;
		if (readDoSeps())
	    return CDSC_ERROR;
  }
  else 
  {
		if (length < 2)
	    return CDSC_NEEDMORE;
		if ((lineA[0] == '%') && (lineA[1] == 'P')) 
		{
	    if (length < 5)
	      return CDSC_NEEDMORE;
	    if (COMPARE(lineA, "%PDF-")) 
	    {
				pdf = true;
				scan_section = scan_comments;
				return CDSC_OK;
	    }
		}
  }

  if (readLine() <= 0)
		return CDSC_NEEDMORE;
	
  dsc_version = addLine((const char *)lineA, line_length);
  if (COMPARE(lineA, "%!PS-Adobe")) 
  {
		dsc = true;
		begincomments = DSC_START;
		if (dsc_version == NULL)
	    return CDSC_ERROR;
		p = lineA + 14;
		while (IS_WHITE(*p))
	    p++;
		if (COMPARE(p, "EPSF-"))
	    epsf = true;
		scan_section = scan_comments;
		return CDSC_PSADOBE;
  }
  if (COMPARE(lineA, "%!")) 
  {
		scan_section = scan_comments;
		return CDSC_NOTDSC;
  }

  scan_section = scan_comments;
  return CDSC_NOTDSC;	
}

void XWPSDSCParser::sectionJoin(unsigned long begin, unsigned long *pend, unsigned long **pplast)
{
	if (begin)
		**pplast = begin;
  if (*pend > begin)
		*pplast = pend;
}

void XWPSDSCParser::unknown()
{
}
