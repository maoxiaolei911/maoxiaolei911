/****************************************************************************
 **
 ** ��Ȩ���� (C) 2006-2007 ���Ĺ�˾.
 **
 ****************************************************************************/
#ifndef XWTIKZTYPE_H
#define XWTIKZTYP_H

#include <QtGlobal>

#if defined(XW_BUILD_TIKZ_LIB)
#define XW_TIKZ_EXPORT Q_DECL_EXPORT
#else
#define XW_TIKZ_EXPORT Q_DECL_IMPORT
#endif

#define XW_TIKZ_UNIT_PT 0
#define XW_TIKZ_UNIT_CM 1
#define XW_TIKZ_UNIT_MM 2


#define XW_TIKZ_CS_NULL    0
#define XW_TIKZ_CS_CANVAS  1
#define XW_TIKZ_CS_XY      2
#define XW_TIKZ_CS_XYZ     3
#define XW_TIKZ_CS_POLAR_CANVAS   4
#define XW_TIKZ_CS_POLAR_CANVAS_XY   5
#define XW_TIKZ_CS_POLAR_XYZ 6
#define XW_TIKZ_CS_POLAR_XYZ_XY 7
#define XW_TIKZ_CS_BARYCENTRIC 8
#define XW_TIKZ_CS_NODE_ANCHOR 9
#define XW_TIKZ_CS_NODE_ANGLE 10
#define XW_TIKZ_CS_TANGENT 11
#define XW_TIKZ_CS_PERPENDICUPAR_HV 12
#define XW_TIKZ_CS_PERPENDICUPAR_VH 13
#define XW_TIKZ_CS_INTERSECTION 14
#define XW_TIKZ_CS_NAME 15
#define XW_TIKZ_CS_CALCU 16

#define XW_TIKZ_INTERSECTION_HV 0
#define XW_TIKZ_INTERSECTION_VH 1

#endif //XWTIKZTYP_H
