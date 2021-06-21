/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWPhysic.h"

XWMatrix33  XWMatrix33::operator + (const XWMatrix33 & mat) const
{
    return XWMatrix33(
                m[0][0] + mat.m[0][0],  m[0][1] + mat.m[0][1],  m[0][2] + mat.m[0][2],
                m[1][0] + mat.m[1][0],  m[1][1] + mat.m[1][1],  m[1][2] + mat.m[1][2],
                m[2][0] + mat.m[2][0],  m[2][1] + mat.m[2][1],  m[2][2] + mat.m[2][2]);
}

XWMatrix33  XWMatrix33::operator + (const XWVector3D & diag) const
{
    float x,y,z;
    diag.get(x,y,z);
    return XWMatrix33(m[0][0] + x, m[0][1], m[0][2],
                      m[1][0], m[1][1] + y, m[1][2],
                      m[2][0], m[2][1], m[2][2] + z);
}

XWMatrix33  XWMatrix33::operator - (const XWMatrix33 & mat) const
{
    return XWMatrix33(
                m[0][0] - mat.m[0][0],  m[0][1] - mat.m[0][1],  m[0][2] - mat.m[0][2],
                m[1][0] - mat.m[1][0],  m[1][1] - mat.m[1][1],  m[1][2] - mat.m[1][2],
                m[2][0] - mat.m[2][0],  m[2][1] - mat.m[2][1],  m[2][2] - mat.m[2][2]);
}

XWMatrix33  XWMatrix33::operator * (const XWMatrix33 & mat) const
{
    return XWMatrix33(
                m[0][0]*mat.m[0][0] + m[0][1]*mat.m[1][0] + m[0][2]*mat.m[2][0],
                m[0][0]*mat.m[0][1] + m[0][1]*mat.m[1][1] + m[0][2]*mat.m[2][1],
                m[0][0]*mat.m[0][2] + m[0][1]*mat.m[1][2] + m[0][2]*mat.m[2][2],

                m[1][0]*mat.m[0][0] + m[1][1]*mat.m[1][0] + m[1][2]*mat.m[2][0],
                m[1][0]*mat.m[0][1] + m[1][1]*mat.m[1][1] + m[1][2]*mat.m[2][1],
                m[1][0]*mat.m[0][2] + m[1][1]*mat.m[1][2] + m[1][2]*mat.m[2][2],

                m[2][0]*mat.m[0][0] + m[2][1]*mat.m[1][0] + m[2][2]*mat.m[2][0],
                m[2][0]*mat.m[0][1] + m[2][1]*mat.m[1][1] + m[2][2]*mat.m[2][1],
                m[2][0]*mat.m[0][2] + m[2][1]*mat.m[1][2] + m[2][2]*mat.m[2][2]);
}

XWVector3D  XWMatrix33::operator * (const XWVector3D & v) const
{
    return XWVector3D(getRow(0).dot(v), getRow(1).dot(v), getRow(2).dot(v));
}

XWMatrix33  XWMatrix33::operator * (float s) const
{
    return XWMatrix33(m[0][0]*s, m[0][1]*s, m[0][2]*s,
                      m[1][0]*s, m[1][1]*s, m[1][2]*s,
                      m[2][0]*s, m[2][1]*s, m[2][2]*s);
}

XWMatrix33  XWMatrix33::operator / (float s)    const
{
    if (s)  
        s = 1.0f / s;
        
    return XWMatrix33(m[0][0]*s, m[0][1]*s, m[0][2]*s,
                      m[1][0]*s, m[1][1]*s, m[1][2]*s,
                      m[2][0]*s, m[2][1]*s, m[2][2]*s);
}

XWMatrix33 & XWMatrix33::operator += (const XWMatrix33 & mat)
{
    m[0][0] += mat.m[0][0];     
    m[0][1] += mat.m[0][1];     
    m[0][2] += mat.m[0][2];
    m[1][0] += mat.m[1][0];     
    m[1][1] += mat.m[1][1];     
    m[1][2] += mat.m[1][2];
    m[2][0] += mat.m[2][0];     
    m[2][1] += mat.m[2][1];     
    m[2][2] += mat.m[2][2];
    return  *this;
}

XWMatrix33 & XWMatrix33::operator -= (const XWMatrix33 & mat)
{
    m[0][0] -= mat.m[0][0];     
    m[0][1] -= mat.m[0][1];     
    m[0][2] -= mat.m[0][2];
    m[1][0] -= mat.m[1][0];     
    m[1][1] -= mat.m[1][1];     
    m[1][2] -= mat.m[1][2];
    m[2][0] -= mat.m[2][0];     
    m[2][1] -= mat.m[2][1];     
    m[2][2] -= mat.m[2][2];
    return  *this;
}

XWMatrix33 & XWMatrix33::operator *= (const XWMatrix33 & mat)
{
    XWVector3D TempRow;
    XWVector3D TempRow = getRow(0);
    m[0][0] = TempRow[0] * mat.m[0][0] + TempRow[1] * mat.m[1][0] + TempRow[2] * mat.m[2][0];
    m[0][1] = TempRow[0] * mat.m[0][1] + TempRow[1] * mat.m[1][1] + TempRow[2] * mat.m[2][1];
    m[0][2] = TempRow[0] * mat.m[0][2] + TempRow[1] * mat.m[1][2] + TempRow[2] * mat.m[2][2];

    TempRow = getRow(1);
    m[1][0] = TempRow[0] * mat.m[0][0] + TempRow[1] * mat.m[1][0] + TempRow[2] * mat.m[2][0];
    m[1][1] = TempRow[0] * mat.m[0][1] + TempRow[1] * mat.m[1][1] + TempRow[2] * mat.m[2][1];
    m[1][2] = TempRow[0] * mat.m[0][2] + TempRow[1] * mat.m[1][2] + TempRow[2] * mat.m[2][2];

    TempRow = getRow(2);
    m[2][0] = TempRow[0] * mat.m[0][0] + TempRow[1] * mat.m[1][0] + TempRow[2] * mat.m[2][0];
    m[2][1] = TempRow[0] * mat.m[0][1] + TempRow[1] * mat.m[1][1] + TempRow[2] * mat.m[2][1];
    m[2][2] = TempRow[0] * mat.m[0][2] + TempRow[1] * mat.m[1][2] + TempRow[2] * mat.m[2][2];
    return  *this;
}

XWMatrix33 & XWMatrix33::operator *= (float s)
{
    m[0][0] *= s;   
    m[0][1] *= s;   
    m[0][2] *= s;
    m[1][0] *= s;   
    m[1][1] *= s;   
    m[1][2] *= s;
    m[2][0] *= s;   
    m[2][1] *= s;   
    m[2][2] *= s;
    return  *this;
}

XWMatrix33 & XWMatrix33::operator /= (float s)
{
    if (s)  
        s = 1.0f / s;
        
    m[0][0] *= s;   
    m[0][1] *= s;   
    m[0][2] *= s;
    m[1][0] *= s;   
    m[1][1] *= s;   
    m[1][2] *= s;
    m[2][0] *= s;   
    m[2][1] *= s;   
    m[2][2] *= s;
    return  *this;
}

XWMatrix33 XWMatrix33::getInvert()
{
    double det = determinant();
    double OneOverDet;
    if(det < 0.00001f && det > -0.00001f)
    {
        OneOverDet = 0.0f;
        return XWMatrix33(1, 0, 0, 0, 1, 0, 0, 0, 1);
    }
    else 
        OneOverDet = 1.0f / det;
        
    XWMatrix33 Temp;
    Temp.m[0][0] = +(m[1][1] * m[2][2] - m[2][1] * m[1][2]) * OneOverDet;
    Temp.m[1][0] = -(m[1][0] * m[2][2] - m[2][0] * m[1][2]) * OneOverDet;
    Temp.m[2][0] = +(m[1][0] * m[2][1] - m[2][0] * m[1][1]) * OneOverDet;
    Temp.m[0][1] = -(m[0][1] * m[2][2] - m[2][1] * m[0][2]) * OneOverDet;
    Temp.m[1][1] = +(m[0][0] * m[2][2] - m[2][0] * m[0][2]) * OneOverDet;
    Temp.m[2][1] = -(m[0][0] * m[2][1] - m[2][0] * m[0][1]) * OneOverDet;
    Temp.m[0][2] = +(m[0][1] * m[1][2] - m[1][1] * m[0][2]) * OneOverDet;
    Temp.m[1][2] = -(m[0][0] * m[1][2] - m[1][0] * m[0][2]) * OneOverDet;
    Temp.m[2][2] = +(m[0][0] * m[1][1] - m[1][0] * m[0][1]) * OneOverDet;

    return Temp;            
}

