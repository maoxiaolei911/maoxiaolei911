/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTEXDOCUMENTTYPE_H
#define XWTEXDOCUMENTTYPE_H

#include <QtGlobal>

#if defined(XW_BUILD_TEXDOCEDITORCORE_LIB)
#define XW_TEXDOCUMENT_EXPORT Q_DECL_EXPORT
#else
#define XW_TEXDOCUMENT_EXPORT Q_DECL_IMPORT
#endif

#define TEX_DOC_WD_TLT 0 //normal
#define TEX_DOC_WD_RTT 1 //Vertical CJK
#define TEX_DOC_WD_LTL 2 //Mongolian
#define TEX_DOC_WD_TRT 3 //Hebrew, Arabic and Syriac

#endif //XWTEXDOCUMENTTYPE_H
