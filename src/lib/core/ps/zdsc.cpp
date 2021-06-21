/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <math.h>
#include "XWPSError.h"
#include "XWPSParam.h"
#include "XWPSDSCParser.h"
#include "XWPSContextState.h"

static const char * const dsc_dict_name = "DSC_struct";

typedef struct cmd_list_s {
    int code;	
    const char *comment_name;	
    int (XWPSContextState::*dsc_proc) (XWPSParamList *, const XWPSDSCParser *);
} cmdlist_t;

static const cmdlist_t DSCcmdlist[] = { 
    { CDSC_PSADOBE,	    "Header",		&XWPSContextState::dscadobeHeader },
    { CDSC_CREATOR,	    "Creator",		&XWPSContextState::dsccreator },
    { CDSC_CREATIONDATE,    "CreationDate",	&XWPSContextState::dsccreationDate },
    { CDSC_TITLE,	    "Title",		&XWPSContextState::dsctitle },
    { CDSC_FOR,		    "For",		&XWPSContextState::dscfor },
    { CDSC_BOUNDINGBOX,     "BoundingBox",	&XWPSContextState::dscboundingBox },
    { CDSC_ORIENTATION,	    "Orientation",	&XWPSContextState::dscorientation },
    { CDSC_BEGINDEFAULTS,   "BeginDefaults",	NULL },
    { CDSC_ENDDEFAULTS,     "BeginDefaults",	NULL },
    { CDSC_PAGE,	    "Page",		&XWPSContextState::dscpage },
    { CDSC_PAGES,	    "Pages",		&XWPSContextState::dscpages },
    { CDSC_PAGEORIENTATION, "PageOrientation",  &XWPSContextState::dscpageOrientation },
    { CDSC_PAGEBOUNDINGBOX, "PageBoundingBox",	&XWPSContextState::dscpageBoundingBox },
    { CDSC_EOF,		    "EOF",		NULL },
    { 0,		    "NOP",		NULL } 
};

static const char * const BadCmdlist[] = {
    "%%BeginData:",
    "%%EndData",
    "%%BeginBinary:",
    "%%EndBinary",
    NULL	
};

static int
convert_orient(CDSCOrientation orient)
{
    return (orient == CDSC_LANDSCAPE ? 1 : 0);
}


int XWPSContextState::dscadobeHeader(XWPSParamList *plist, const XWPSDSCParser *pData)
{
	return dscputInt(plist, "EPSF", (int)(pData->epsf? 1: 0));
}

int XWPSContextState::dscboundingBox(XWPSParamList *plist, const XWPSDSCParser *pData)
{
	return dscputBoundingBox(plist, "BoundingBox", pData->bbox);
}

int XWPSContextState::dsccreator(XWPSParamList *plist, const XWPSDSCParser *pData)
{
	return dscPutString(plist, "Creator", pData->dsc_creator );
}

int XWPSContextState::dsccreationDate(XWPSParamList *plist, const XWPSDSCParser *pData)
{
	return dscPutString(plist, "CreationDate", pData->dsc_date );
}

int XWPSContextState::dscorientation(XWPSParamList *plist, const XWPSDSCParser *pData)
{
	return dscputInt(plist, "Orientation", convert_orient((CDSCOrientation)(pData->page_orientation)));
}

int XWPSContextState::dscpage(XWPSParamList *plist, const XWPSDSCParser *pData)
{
	return dscputInt(plist, "PageNum", pData->page[pData->page_count - 1].ordinal );
}

int XWPSContextState::dscpageBoundingBox(XWPSParamList *plist, const XWPSDSCParser *pData)
{
	return dscputBoundingBox(plist, "PageBoundingBox", pData->page_bbox);
}

int XWPSContextState::dscpageOrientation(XWPSParamList *plist, const XWPSDSCParser *pData)
{
	int page_num = pData->page_count;
  if (page_num && pData->page[page_num - 1].orientation != CDSC_ORIENT_UNKNOWN)
		return dscputInt(plist, "PageOrientation",convert_orient((CDSCOrientation)(pData->page[page_num - 1].orientation)));
  else
  	return dscputInt(plist, "Orientation", convert_orient((CDSCOrientation)(pData->page_orientation)));
}

int XWPSContextState::dscpages(XWPSParamList *plist, const XWPSDSCParser *pData)
{
	return dscputInt(plist, "NumPages", pData->page_pages);
}

int XWPSContextState::dscputBoundingBox(XWPSParamList *plist, const char *keyname, const CDSCBBOX *pbbox)
{
	int values[4];
  PSParamIntArray va;

  if (!pbbox)
		return 0;
    
  values[0] = pbbox->llx;
  values[1] = pbbox->lly;
  values[2] = pbbox->urx;
  values[3] = pbbox->ury;
  va.data = values;
  va.size = 4;
  return plist->writeIntArray(this, keyname, &va);
}

int XWPSContextState::dscputInt(XWPSParamList *plist, const char *keyname, int value)
{
    return plist->writeInt(this, keyname, &value);
}

int XWPSContextState::dscPutString(XWPSParamList *plist, const char *keyname, const char *string)
{
	PSParamString str((const uchar*)string, strlen(string));

  return plist->writeString(this, keyname, &str);
}

int XWPSContextState::dsctitle(XWPSParamList *plist, const XWPSDSCParser *pData)
{
	 return dscPutString(plist, "Title", pData->dsc_title );
}

int XWPSContextState::zinitializeDSCParser()
{
	XWPSRef local_ref;
  int code;
  XWPSRef * op = op_stack.getCurrentTop();
  XWPSDSCParser * const data = new XWPSDSCParser;
  local_ref.makeAStruct(PS_A_READONLY | op->space(), data);
  code = dictPutString(op, dsc_dict_name, &local_ref);
  if (code >= 0)
		pop(1);
  return code;
}

int XWPSContextState::zparseDSCComments()
{
#define MAX_DSC_MSG_SIZE (DSC_LINE_LENGTH + 4)
  XWPSRef * opString = op_stack.getCurrentTop();
  XWPSRef * opDict = opString - 1;
  uint ssize;
  int comment_code, code;
  char dsc_buffer[MAX_DSC_MSG_SIZE + 2];
  const cmdlist_t *pCmdList = DSCcmdlist;
  const char * const *pBadList = BadCmdlist;
  XWPSRef * pvalue;
  XWPSDSCParser * dsc_data;
  
  code = opString->checkType(XWPSRef::String);
  if (code < 0)
  	return code;
  	
  code = opDict->checkDictWrite();
  if (code < 0)
  	return code;
  	
  ssize = opString->size();
  if (ssize > MAX_DSC_MSG_SIZE - 2)
		return (int)(XWPSError::RangeCheck);
  
  memcpy(dsc_buffer, opString->getBytes(), ssize);
  dsc_buffer[ssize] = 0x0d;	
  dsc_buffer[ssize + 1] = 0;
  
  while (*pBadList && strncmp(*pBadList, dsc_buffer, strlen(*pBadList)))
        pBadList++;
  
  if (*pBadList) 
    comment_code = 0;	
  else 
  {
     code = opDict->dictFindString(this, dsc_dict_name, &pvalue);
     if (code < 0)
  			return code;
  	
		 dsc_data = (XWPSDSCParser*)(pvalue->getStruct());
     comment_code = dsc_data->scanData(dsc_buffer, ssize + 1);
      if (comment_code < 0)
	    	return comment_code;
  }
  
  while (pCmdList->code && pCmdList->code != comment_code )
		pCmdList++;
    
  if (pCmdList->dsc_proc) 
  {
  	XWPSDictParamList list(false, opDict, 0, (QObject*)0);
  	list.writing = true;
		code = (this->*(pCmdList->dsc_proc))(&list, dsc_data);
		if (code < 0)
	    return code;
  }

  return nameEnterString(pCmdList->comment_name, opString);
}
