/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWGLOBAL_H
#define XWGLOBAL_H

#include <QtCore/QtGlobal>

#if defined(XW_DEBUG)
#include <QDebug>
#endif

#if defined(XW_BUILD_UTIL_LIB)
#define XW_UTIL_EXPORT Q_DECL_EXPORT
#else
#define XW_UTIL_EXPORT Q_DECL_IMPORT
#endif


#if defined(XW_BUILD_FONT_LIB)
#define XW_FONT_EXPORT Q_DECL_EXPORT
#else
#define XW_FONT_EXPORT Q_DECL_IMPORT
#endif


#if defined(XW_BUILD_RASTER_LIB)
#define XW_RASTER_EXPORT Q_DECL_EXPORT
#else
#define XW_RASTER_EXPORT Q_DECL_IMPORT
#endif

#if defined(XW_BUILD_CORE_LIB)
#define XW_CORE_EXPORT Q_DECL_EXPORT
#else
#define XW_CORE_EXPORT Q_DECL_IMPORT
#endif


#if defined(XW_BUILD_DVI_LIB)
#define XW_DVI_EXPORT Q_DECL_EXPORT
#else
#define XW_DVI_EXPORT Q_DECL_IMPORT
#endif

#if defined(XW_BUILD_PDF_LIB)
#define XW_PDF_EXPORT Q_DECL_EXPORT
#else
#define XW_PDF_EXPORT Q_DECL_IMPORT
#endif

#if defined(XW_BUILD_PS_LIB)
#define XW_PS_EXPORT Q_DECL_EXPORT
#else
#define XW_PS_EXPORT Q_DECL_IMPORT
#endif

#if defined(XW_BUILD_GUI_LIB)
#define XW_GUI_EXPORT Q_DECL_EXPORT
#else
#define XW_GUI_EXPORT Q_DECL_IMPORT
#endif


#endif // XWGLOBAL_H

