/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPSDSCPASER_H
#define XWPSDSCPASER_H

#include "XWPSType.h"

#define CDSC_KNOWN_MEDIA 11
#define DSC_LINE_LENGTH 255
#define CDSC_STRING_CHUNK 4096
#define CDSC_PAGE_CHUNK 128	
#define CDSC_DATA_LENGTH 8192

class XWPSDSCParser : public XWPSStruct
{
public:	
	XWPSDSCParser();
	~XWPSDSCParser();
	
	int addMedia(CDSCMEDIA *mediaA);
	int addPage(int ordinal, char *label);
	
	int errorHandler(void *, 
	                unsigned int ,
		              const char *, 
		              unsigned int );
		              
	virtual int getLength();
	virtual const char * getTypeName();
		
	int  scanData(const char *dataA, int length);
	void setErrorFunction(int (XWPSDSCParser::*fn)(void *, uint , const char *, uint ));
	void setLength(ulong len) {file_length = len;}
	int  setPageBbox(uint page_number, int llx, int lly, int urx, int ury);
	
private:
	char * addLine(const char *lineA, unsigned int len);
	char * allocString(const char *str, int len);
	
	int checkMatch();
	int checkMatchPrompt(const char *str, int count);
	int checkMatchType(const char *str, int count);
	char * copyString(char *str, 
	                  unsigned int slen, 
	                  char *lineA, 
	                  unsigned int len, 
	                  unsigned int *offset);
	
	int dscStricmp(const char *s, const char *t);
	
	int error(unsigned int explanation, char *lineA, unsigned int line_len);
	
	ulong getDWord(const unsigned char *buf);
	int   getInt(const char *lineA, unsigned int len, unsigned int *offset);
	float getReal(const char *lineA, unsigned int len, unsigned int *offset);
	uint  getWord(const unsigned char *buf);
	
	void init2();
	bool isSection(char *lineA);
	
	int  parseBoundingBox(CDSCBBOX** pbbox, int offset);
	int  parseDocumentMedia();
	int  parseMedia(const CDSCMEDIA **page_media);
	int  parseOrder();
	int  parseOrientation(unsigned int *porientation, int offset);
	int  parsePage();
	int  parsePages();
	int  parseViewerOrientation(CDSCCTM **pctm);
	
	int  readDoSeps();
	int  readLine();
	void reset();
	
	void saveLine();
	int  scanComments();
	int  scanDefaults();
	int  scanPage();
	int  scanPreview();
	int  scanProlog();
	int  scanSetup();
	int  scanTrailer();
	int  scanType();
	void sectionJoin(unsigned long begin, unsigned long *pend, unsigned long **pplast);
	
	void unknown();
	
public:
	int dsc;
  int ctrld;
  int pjl;
  int epsf;	
  int pdf;
  uint preview;	
  char *dsc_version;
  uint language_level;
  uint document_data;	
  ulong begincomments;
  ulong endcomments;
  ulong beginpreview;
  ulong endpreview;
  ulong begindefaults;
  ulong enddefaults;
  ulong beginprolog;
  ulong endprolog;
  ulong beginsetup;
  ulong endsetup;
  ulong begintrailer;
  ulong endtrailer;
  
  CDSCPAGE *page;
  uint page_count;
  uint page_pages;
  uint page_order;
  uint page_orientation;
  CDSCCTM *viewer_orientation;
  uint media_count;	
  CDSCMEDIA **media;
  const CDSCMEDIA *page_media;
  CDSCBBOX *bbox;	
  CDSCBBOX *page_bbox;
  CDSCDOSEPS *doseps;	
  char *dsc_title;
  char *dsc_creator;
  char *dsc_date;
  char *dsc_for;

  uint max_error;	
  const int *severity;	
  void *caller_data;	
  int id;		
  int scan_section;	
  ulong doseps_end;	
  uint page_chunk_length;
  ulong file_length;
  int skip_document;
  int skip_bytes;	
  int skip_lines;
  int skip_pjl;	
  int begin_font_count;
  int begin_feature_count;
  int begin_resource_count;	
  int begin_procset_count;
  char data[CDSC_DATA_LENGTH];
  uint data_length;
  uint data_index;
  ulong data_offset;
  int eof;
  char *line;
  uint line_length;
  int eol;
  int last_cr;
  uint line_count;
  int long_line;
  char last_line[256];
  CDSCSTRING *string_head;
  CDSCSTRING *string;

  int (XWPSDSCParser::*dsc_error_fn)(void *, uint , const char *, uint );
};

#endif //XWPSDSCPASER_H
