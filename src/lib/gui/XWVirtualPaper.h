/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#ifndef XWVIRTUALPAPER_H
#define XWVIRTUALPAPER_H

#include <QQueue>
#include "XWPhysic.h"

class XWPaperMesh
{
public:
    XWPaperMesh();
    ~XWPaperMesh();
    
    void clear();
    
    void  getAuxEdge(int i, int * vi, int * vj);
          {
              *vi = auxEdges[i].vi;
              *vj = auxEdges[i].vj;
          }
    void  getEdge(int i, int * vi, int * vj, int * vr, int * vl)
          {
              *vi = edges[i].vi;
              *vj = edges[i].vj;
              *vr = edges[i].vr;
              *vl = edges[i].vl;
          }
    float getMass() {return mass;}
    int   getNumOfAuxEdge() {return numOfAux;}
    int   getNumOfEdge() {return numOfEdge;}
    int   getNumOfVertex() {return numOfVertex;}
    void  getVertex(int i, float * x, float * y, float * z, float * s, float * t)
          {
              *x = vertexes[i].x;
              *y = vertexes[i].y;
              *z = vertexes[i].z;
              *s = vertexes[i].s;
              *t = vertexes[i].t;
          }
    float getStiffness() {return stiffness;}
    
    void setVertexNumber(int numOfMassPerRowA, int numOfMassPerColA);    
    void setVertexLoc(const XWVector3D & topLeftA, 
                      const XWVector3D & topRightA,
                      const XWVector3D & bottomLeftA,
                      const XWVector3D & bottomRightA);
    void setMass(float massA) {mass = massA;}
    void setStiffness(float stiffnessA) {stiffness = stiffnessA;}
    
public:
    float mass;
    int numOfMassPerRow;
    int numOfMassPerCol;
    int numOfVertex;
    
    struct TextureCood
    {
        float s, t;
    }
    
    struct Vertex
    {
        float x, y, z;
        float s, t;
    };
    
    Vertex * vertexes;
    
    int numOfEdge;
    float stiffness;
    
    struct Edge
    {
        int vi, vj, vr, vl;
    };
    
    Edge * edges;
    
    int numOfAux;
    struct AuxEdge
    {
        int vi, vj;
    };
    
    AuxEdge * auxEdges;
    
    XWVector3D planeNormal;
};

//#define STATIC_TOP 0
//#define STATIC_BOTTOM 1
#define STATIC_LEFT 2
#define STATIC_RIGHT 3


class XWVirtualPaper
{
public:
    XWVirtualPaper();
    ~XWVirtualPaper();
    
    void clear();
    
    int getNumOfSpareMasses() {return spareMasses.size();}
    int getNumOfSpareSprings {return usedSprings.size();}
    int getNumOfUsedMasses() {return usedMasses.size();}
    int getNumOfUsedSprings() {return usedSprings.size();}
    int getSpareMass() { return spareMasses.size() > 0 ? spareMasses.dequeue() : -1;}
    int getSpareSpring() { return spareSprings.size() > 0 ? spareSprings.dequeue() : -1;}
    int getUsedMass(int i)  {return usedMasses[i];}
    int getUsedSpring(int i) {return usedSprings[i];}
    
    void init(XWPaperMesh * mesh);
    
    void insertSpareMass(int i) {spareMasses.enqueue(i);}
    void inserSpareSpring(int i) {spareSprings.enqueue(i);}
    void insertUsedMass(int i) {usedMasses.enqueue(i);}    
    void insertUsedSpring(int i) {usedSprings.enqueue(i);}
    
    void setDamp(float dCoeff) {damp = dCoeff;}
    void setGravity(float gx, float gy, float gz) {gravity.set(gx,gy,gz);}
    void setStatic(int e, bool flag);
    
    void removeUsedMass(int i) {usedMasses.removeAll(i);}
    void removeUsedSprings(int i) {usedSprings.removeAll(i);}
    
    void updateTurn(XWPaperMesh * mesh, float dt, int timeStepSubdivision = 10);
    
public:
    float damp; //减震系数
	XWVector3D gravity; //重力加速度
	float stiffness; //弹性系数
	float mass; //质量
	
	float cataRatio; //弹簧压缩率
	float minimumEdgeLength; //弯折前的最小弹簧长度
	float phi; //控制弹簧是否弯折的参数
	float psi; //控制弹簧弯折长度的参数
	float auxEdgeRatio; //控制辅助边长度(长度为auxEdgeRatio*lt + (1-auxEdgeRatio)*l0)的参数
	float sigma; //控制折皱处平滑程度的参数
	float epsilon; //控制弯折弹簧复原的参数
	
	bool subdivision;
	int  numSubdivision;
	
	int maxOfMasses;
	int numOfMasses;
	XWMass * masses;
	
	int maxOfSprings;
	int numOfSprings;
	XWSpring * springs;
	
	int numOfMassPerRow;
    int numOfMassPerCol;
    
    QQueue<int> usedMasses;
    QQueue<int> spareMasses;
    QQueue<int> usedSprings;
    QQueue<int> spareSprings;
	
private:
    void breakAndFoldEdges();
    void breakEdge(int eIdx);
    
    void calcForceAndJacobian();
    void calcLocation();
    void calcVelocity();
    void calcVelocityChange();
    void computeNormal();
    
    int findEdge(int vi, int vj);
    
    void recoverEdge();
    
    void setEdge(int idx, 
                 int vi, 
                 int vj, 
                 int vr, 
                 int vl, 
                 bool breakableFlag);
                 
    void updateEdge(int idx, int vprev, int vnew);
};


#endif // XWVIRTUALPAPER_H
