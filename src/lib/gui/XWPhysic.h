/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWPHYSIC_H
#define XWPHYSIC_H

#include <math.h>

class XWVector3D
{
public:
    XWVector3D() : x(0), y(0), z(0) {}
    XWVector3D(float xA, float yA, float zA) : x(xA), y(yA), z(zA) {}
    XWVector3D(const XWVector3D & v)
    {
        x = v.x; y = v.y; z = v.z;
    }
    ~XWVector3D() {}
    
    XWVector3D & operator= (const XWVector3D & v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }
    
    XWVector3D operator+ (const XWVector3D & v)
    {
        return XWVector3D(x + v.x, y + v.y, z + v.z);
    }
    
    XWVector3D operator- (const XWVector3D & v)
    {
        return XWVector3D(x - v.x, y - v.y, z - v.z);
    }
    
    XWVector3D operator* (float value)
    {
        return XWVector3D(x * value, y * value, z * value);
    }
    
    XWVector3D operator/ (float value)
    {
        return XWVector3D(x / value, y / value, z / value);
    }
    
    XWVector3D& operator+= (const XWVector3D & v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }
    
    XWVector3D& operator-= (const XWVector3D & v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }
    
    XWVector3D& operator*= (float value)
    {
        x *= value;
        y *= value;
        z *= value;
        return *this;
    }
    
    XWVector3D& operator/= (float value)
    {
        x /= value;
        y /= value;
        z /= value;
        return *this;
    }
    
    XWVector3D operator- ()
    {
        return XWVector3D(-x, -y, -z);
    }
    
    double operator[](int idx) const
    {
        switch(idx) 
        {
            case 0: 
                return x;
                break;
                
            case 1: 
                return y;
                break;
                
            case 2: 
                return z;
                break;
                
            default: 
                return 0;
                break;
        }
    }
    
    XWVector3D cross(const XWVector3D &v)
    {
        return XWVector3D(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }
    
    float distanceFromPlane(const XWVector3D &p, const XWVector3D &n)
    {
        float d = (-p.x * n.x - p.y * n.y - p.z * n.z);
        return (fabs(x * n.x + y * n.y + z * n.z + d) / (n.x * n.x + n.y * n.y + n.z * n.z));
    }
    
    float dot(const XWVector3D &v)
    {
        return (x * v.x + y * v.y + z * v.z);
    }
    
    void get(float * xA, float * yA, float * zA)
    {
        *xA = x; *yA = y; *zA = z;
    }
    
    float length()
    {
        return sqrtf(x*x + y*y + z*z);
    }
    
    void rotate(const XWVector3D &axis, double angle)
    {
        XWVector3D a(axis.x, axis.y, axis.z);
        a.unitize();
        float cosine = cos(angle);
        float sine = sin(angle);
        float dotProd = dot(a);
        
        float rx = a.x * dotProd + (x * (a.y * a.y + a.z * a.z) - 
                   a.x *( a.y * y + a.z * z)) * cosine + (-a.z * y + a.y * z) * sine;
        float ry = a.y * dotProd + (y * (a.x * a.x + a.z * a.z)- 
                   a.y * (a.x * x + a.z * z)) * cosine + (a.z * x + a.x * z) * sine;
        float rz = a.z * dotProd + (z * (a.x * a.x + a.y * a.y) - 
                   a.z * (a.x * x + a.y * y))*cosine + (-a.y * x + a.x * y)*sine;
        
        return XWVector3D(rx, ry, rz);
    }
    
    void set(float xA, float yA, float zA)
    {
        x = xA; y = yA; z = zA;
    }
    
    void unitize()
    {
        float length = this->length();

        if (length == 0)
            return;

        x /= length;
        y /= length;
        z /= length;
    }
    
    XWVector3D unit()
    {
        float length = this->length();

        if (length == 0)
            return *this;
        
        return XWVector3D(x / length, y / length, z / length);
    }
    
public:
    float x, y, z;
};

class XWMatrix33
{
public:
    XWMatrix33()
    {
        identity();
    }
    XWMatrix33(float m00, 
               float m01, 
               float m02, 
               float m10, 
               float m11, 
               float m12, 
               float m20, 
               float m21, 
               float m22)
    {
        set(m00, m01, m02, m10, m11, m12, m20, m21, m22);
    }
    
    XWMatrix33(const XWMatrix33 & v)
    {
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                m[i][j] = v.m[i][j];
    }
    ~XWMatrix33() {}
    
    XWMatrix33  operator + (const XWMatrix33 & mat) const;
    XWMatrix33	operator + (const XWVector3D & diag) const;
    XWMatrix33	operator - (const XWMatrix33 & mat)	const;
    XWMatrix33	operator * (const XWMatrix33 & mat) const;
    XWVector3D	operator * (const XWVector3D & v) const;
    XWMatrix33	operator * (float s) const;
    friend XWMatrix33 operator*(float s, const XWMatrix33& mat) 
           {
				return XWMatrix33(s*mat.m[0][0], s*mat.m[0][1],	s*mat.m[0][2],
								  s*mat.m[1][0], s*mat.m[1][1],	s*mat.m[1][2],
								  s*mat.m[2][0], s*mat.m[2][1],	s*mat.m[2][2]);
	       }
	
    XWMatrix33	operator / (float s)	const;
    
    friend XWMatrix33 operator/(float s, const XWMatrix33& mat) 
            {
				return XWMatrix33(s / mat.m[0][0], s / mat.m[0][1], s / mat.m[0][2],
								  s / mat.m[1][0], s / mat.m[1][1],	s / mat.m[1][2],
								  s / mat.m[2][0], s / mat.m[2][1],	s / mat.m[2][2]);
	        }
    XWMatrix33 & operator += (const XWMatrix33 & mat);
    XWMatrix33 & operator -= (const XWMatrix33 & mat);
    XWMatrix33 & operator *= (const XWMatrix33 & mat);
    XWMatrix33 & operator *= (float s);
    XWMatrix33 & operator /= (float s);
    
    void clear()
    {
        set(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    }
    
    void copy(const XWMatrix33 & v)
    {
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                m[i][j] = v.m[i][j];
    }
    
    float determinant() const
    {
        return (m[0][0] * m[1][1] * m[2][2] + 
                m[0][1] * m[1][2] * m[2][0] + 
                m[0][2] * m[1][0] * m[2][1]) -  
               (m[2][0] * m[1][1] * m[0][2] + 
                m[2][1] * m[1][2] * m[0][0] + 
                m[2][2] * m[1][0] * m[0][1]);
    }
        
    void getCol(const int c, XWVector3D& p) const
    {
        p.set(m[0][c], m[1][c], m[2][c]);
    }
    
    XWVector3D getCol(const int c) const
    {
        return XWVector3D(m[0][c], m[1][c], m[2][c]);
    }
    
    XWMatrix33 getInvert();
    
    void getRow(const int r, XWVector3D& p)	const
    {
        p.set(m[r][0], m[r][1], m[r][2]);
    }
    
    XWVector3D getRow(const int r) const
    {
        return XWVector3D(m[r][0], m[r][1], m[r][2]);
    }
    
    void identity()
    {
        set(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    }
    
    void scale(const XWVector3D& p)
    {
        m[0][0] *= p.x;    m[0][1] *= p.x;    m[0][2] *= p.x;
        m[1][0] *= p.y;    m[1][1] *= p.y;    m[1][2] *= p.y;
        m[2][0] *= p.z;    m[2][1] *= p.z;    m[2][2] *= p.z;
    }
    
    void scale(double sx, double sy, double sz)
    {
        m[0][0] *= sx;  m[0][1] *= sx;  m[0][2] *= sx;
        m[1][0] *= sy;  m[1][1] *= sy;  m[1][2] *= sy;
        m[2][0] *= sz;  m[2][1] *= sz;  m[2][2] *= sz;
    }
    
    void scale(double s)
    {
        m[0][0] *= s;   m[0][1] *= s;   m[0][2] *= s;
        m[1][0] *= s;   m[1][1] *= s;   m[1][2] *= s;
        m[2][0] *= s;   m[2][1] *= s;   m[2][2] *= s;
    }
    
    void set(float m00, 
             float m01, 
             float m02, 
             float m10, 
             float m11, 
             float m12, 
             float m20, 
             float m21, 
             float m22)
    {
        m[0][0] = m00;
        m[0][1] = m01;
        m[0][2] = m02;
        
        m[1][0] = m10;
        m[1][1] = m11;
        m[1][2] = m12;
        
        m[2][0] = m20;
        m[2][1] = m21;
        m[2][2] = m22;
    }
    
    void setCol(const int c, const XWVector3D& p)
    {
        m[0][c] = p.x;	m[1][c] = p.y;	m[2][c] = p.z;
    }
    
    void setRow(const int r, const XWVector3D & p)
    {
        m[r][0] = p.x;	m[r][1] = p.y;	m[r][2] = p.z;
    }
    
    void setScale(const XWVector3D & v)
    {
        m[0][0] = v.x; m[1][1] = v.y; m[2][2] = v.z;
    }
    
    void setScale(float sx, float sy, float sz)
    {
        m[0][0] = sx;   m[1][1] = sy;   m[2][2] = sz;
    }
    
    double	trace()
    {
        return m[0][0] + m[1][1] + m[2][2];
    }
    
public:
    float m[3][3];
};

class XWMass
{
public:
    XWMass() 
        :m(0),          
         xt(0, 0, 0),
         v(0),
         f(0, 0, 0),
         constrained(false),
         s(0.0f),
         t(0.0),
         x0(0, 0, 0), 
         dv(0, 0, 0),
         n(0, 0, 0),
         pi(0,0,0), 
         right(0,0,0),
         Jii(1,0,0,0,1,0,0,0,1), 
         W(1,0,0,0,1,0,0,0,1), 
         Winv(1,0,0,0,1,0,0,0,1) {}
    ~XWMass() {}
    
public:
    float m; //质量        
    XWVector3D xt; //当前位置    
    XWVector3D v; //速度        
    XWVector3D f; //力
    bool constrained; //是否被固定
    
    float s, t; //纹理坐标
    
    XWVector3D x0; //初始位置
    XWVector3D dv; //速度变化    
    XWVector3D n; //法向量
    
    XWVector3D pi, right; //for 积分
    XWMatrix33 Jii, W, Winv;
};

class XWSpring
{
public:
    XWSpring() 
     : l0(0), 
       stiffness(0), 
       vi(-1), 
       vj(-1), 
       breakable(true),
       vr(-1), 
       vl(-1),
       vn(-1),
       Jij(1,0,0,0,1,0,0,0,1) 
       {}
    ~XWSpring() {}
    
public:
    float l0; //长度
	float stiffness; //弹性系数
	int vi, vj;  //连接的质点
	
	bool breakable;
	int vr, vl; //四边形网格中的左右质点
	
	int vn; //新质点;
	
	XWMatrix33 Jij;
};

#endif // XWPHYSIC_H

