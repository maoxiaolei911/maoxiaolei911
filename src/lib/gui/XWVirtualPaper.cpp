/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWVirtualPaper.h"

XWPaperMesh::XWPaperMesh()
{
    mass = 5;
    numOfMassPerRow = 0;
    numOfMassPerCol = 0;
    numOfVertex = 25;
    vertexes = 0;
        
    stiffness = 100.0;
    numOfEdge = 56;
    edges = 0;
    
    numOfAux = 0;
    auxEdges = 0;
}

XWPaperMesh::~XWPaperMesh()
{
    clear();
}

void XWPaperMesh::clear()
{
    if (vertexes)
        delete [] vertexes;
        
    if (edges)
        delete [] edges;
        
    if (auxEdges)
        delete [] auxEdges;
        
    numOfVertex = 0;
    vertexes = 0;
    numOfEdge = 0;
    edges = 0;
    numOfAux = 0;
    auxEdges = 0;
}

void XWPaperMesh::setVertexNumber(int numOfMassPerRowA, int numOfMassPerColA)
{
    clear();
    numOfMassPerRow = numOfMassPerRowA;
    numOfMassPerCol = numOfMassPerColA;
    numOfVertex = numOfMassPerRow * numOfMassPerCol;
    vertexes = new Vertex[numOfVertex];
    for (int i = 0; i < numOfVertex; i++)
    {
        vertexes[i].x = 0;
        vertexes[i].y = 0;
        vertexes[i].z = 0;
        vertexes[i].s = 0;
        vertexes[i].t = 0;   
    }
    numOfEdge = numOfMassPerRow * (numOfMassPerCol - 1) + 
                (numOfMassPerRow - 1) * numOfMassPerCol + 
                (numOfMassPerRow - 1) * (numOfMassPerCol - 1);
    edges = new Edge[numOfEdge];
    
    int k = 0;
    for (int i = 0; i < numOfMassPerRow; i++)
    {
        for (int j = 0; j < (numOfMassPerCol - 1); j++)
        {
            edges[k].vi = i * numOfMassPerCol + j;
            edges[k].vj = i * (numOfMassPerCol + j + 1;
            if (i == 0)
            {
                edges[k].vr = numOfMassPerCol + j;
                edges[k].vl = -1;
            }
            else if (i == (numOfMassPerRow - 1))
            {
                edges[k].vr = -1;
                edges[k].vl = (i - 1) * numOfMassPerCol + j;
            }
            else
            {
                edges[k].vr = (i + 1) * numOfMassPerCol + j + 1;
                edges[k].vl = (i - 1) * numOfMassPerCol + j + 1;
            }
                
            k++;
        }
    }
    
    for (int i = 0; i < (numOfMassPerRow - 1); i++)
    {
        for (int j = 0; j < numOfMassPerCol; j ++)
        {
            edges[k].vi = i * numOfMassPerCol + j;
            edges[k].vj = (i + 1) * numOfMassPerCol + j;
            if (j == 0)
            {
                edges[k].vr = -1;
                edges[k].vl = i * numOfMassPerCol + 1;
            }
            else if (j == (numOfMassPerCol - 1))
            {
                edges[k].vr = (i + 1) * numOfMassPerCol + j - 1;
                edges[k].vl = -1;
            }
            else
            {
                edges[k].vr = i * numOfMassPerCol + j + 1;
                edges[k].vl = i * numOfMassPerCol + j - 1;
            }
                
            k++;
        }
    }
    
    for (int i = 0; i < (numOfMassPerRow - 1); i++)
    {
        for (int j = 0; j < (numOfMassPerCol - 1); j++)
        {
            edges[k].vi = i * numOfMassPerCol + j + 1;
            edges[k].vj = (i + 1) * numOfMassPerCol + j;
            edges[k].vr = i * numOfMassPerCol + j;
            edges[k].vl = (i + 1) * numOfMassPerCol + j + 1;
            k++;
        }
    }
}

void XWPaperMesh::setVertexLoc(const XWVector3D & topLeftA, 
                               const XWVector3D & topRightA,
                               const XWVector3D & bottomLeftA,
                               const XWVector3D & bottomRightA)
{
    XWVector3D * l = new XWVector3D[numOfMassPerRow];
    XWVector3D * r = new XWVector3D[numOfMassPerRow]; 
    TextureCood * lt = new TextureCood[numOfMassPerRow];
    TextureCood * rt = new TextureCood[numOfMassPerRow];
    
    int k = numOfMassPerRow - 1;
    for (int i = 0; i < numOfMassPerRow; i++)
    {
        l[i].x = bottomLeftA.x + (k - i) * (topLeftA.x - bottomLeftA.x) / k;
        l[i].y = bottomLeftA.y + (k - i) * (topLeftA.y - bottomLeftA.y) / k;
        l[i].z = bottomLeftA.z + (k - i) * (topLeftA.z - bottomLeftA.z) / k;
        r[i].x = bottomRightA.x + (k - i) * (topRightA.x - bottomRightA.x) / k;
        r[i].y = bottomRightA.y + (k - i) * (topRightA.y - bottomRightA.y) / k;
        r[i].z = bottomRightA.z + (k - i) * (topRightA.z - bottomRightA.z) / k;
        lt[i].s = 0.00;
        lt[i].t = (k - i) / k;
        rt[i].s = 1.00;
        rt[i].t = (k - i) / k;
        
    }
    
    k = numOfMassPerCol - 1;
    for (int i = 0; i < numOfMassPerRow; i++)
    {
        for (int j = 0; j < numOfMassPerCol; j++)
        {
            vertexes[i * j].x = l[i].x + j * (r[i].x - l[i].x) / k;
            vertexes[i * j].y = l[i].y + j * (r[i].y - l[i].y) / k;
            vertexes[i * j].z = l[i].z + j * (r[i].z - l[i].z) / k;
            vertexes[i * j].s = lt[i].s + j * (rt[i].s - lt[i].s) / k;
            vertexes[i * j].t = lt[i].t + j * (rt[i].t - lt[i].t) / k;
        }
    }
    
    XWVector3D t = topRightA - topLeftA;
    XWVector3D l = bottomLeftA - topLeftA;
    planeNormal = t.cross(l);
    
    delete [] l;
    delete [] r;
    delete [] lt;
    delete [] rt;
}


XWVirtualPaper::PXWVirtualPaper()
     : damp(0),
       gravity(0,-9.8,0),
       stiffness(100000),
       mass(0),
       cataRatio(0.95),
       minimumEdgeLength(0.02),
       phi(0.2), 
       psi(0.1), 
       auxEdgeRatio(0.5),
       sigma(1.1), 
       epsilon(0.1),
       subdivision(true), 
       numSubdivision(1), 
       maxOfMasses(0),
       numOfMasses(0),
       masses(0),
       maxOfSprings(0),
       numOfSprings(0),
       springs(0)
{
}

XWVirtualPaper::~XWVirtualPaper()
{
    clear();
}

void XWVirtualPaper::clear()
{
    if (masses)
        delete [] masses;
    
    maxOfMasses = 0;
    numOfMasses = 0;
    masses = 0;
    
    if (springs)
        delete [] springs;
    
    maxOfSprings = 0;
    numOfSprings = 0;
    springs = 0;
    
    numOfMassPerRow = 0;
    numOfMassPerCol = 0;
    
    usedMasses.clear();
    spareMasses.clear();
    usedSprings.clear();
    spareSprings.clear();
}

void XWVirtualPaper::init(XWPaperMesh * mesh)
{
    clear();
    
    numOfMasses = mesh->getNumOfVertex();
    maxOfMasses = numOfMasses * 100;
    mass = mesh->getMass();
    
    masses = new XWMass[maxOfMasses];
    float x, y, z, s, t;
    for (int i = 0; i < numOfMasses; i++)
    {
        mesh->getVertex(i, &x, &y, &z, &s, &t);
        masses[i].x0.x = x;
        masses[i].xt.x = x;
        masses[i].x0.y = y;
        masses[i].xt.y = y;
        masses[i].x0.z = z;
        masses[i].xt.z = z;
        masses[i].s = s;
        masses[i].t = t;
        masses[i].m = mass / numOfMasses;
        insertUsedMass(i);
    }
    
    for (int i = numOfMasses; i < maxOfMasses; i++)
        insertSpareMass(i);
    
    numOfSprings = mesh->getNumOfEdge();
    maxOfSprings = numOfSprings * 100;
    springs = new XWSpring[maxOfSprings];
    stiffness = mesh->getStiffness();
    
    int vi, vj, vr, vl;
    for (int i = 0; i < numOfSprings; i++)
    {
        mesh->getEdge(i, &vi, &vj, &vr, &vl);
        springs[i].vi = vi;
        springs[i].vj = vj;
        springs[i].vr = vr;
        springs[i].vl = vl;
        springs[i].l0 = (masses[vi].x0 - masses[vj].x0).length();
        springs[i].stiffness = stiffness;
        
        insertUsedSpring(i);
    }
    
    int nAux = mesh->getNumOfAuxEdge();
    for (int i = 0; i < nAux; i++)
    {
        mesh->getAuxEdge(i, &vi, &vj);
        springs[numOfSprings + i].vi = vi;
        springs[numOfSprings + i].vj = vj;
        springs[numOfSprings + i].l0 = (masses[vi].x0 - masses[vj].x0).length();
        springs[numOfSprings + i].stiffness = stiffness/springs[i].l0;
        springs[numOfSprings + i].breakable = false;
        insertUsedSpring(numOfSprings + i);
    }
    
    numOfSprings += nAux;
    
    for (int i = numOfSprings; i < numOfSprings; i++)
        inserSpareSpring(i);
    
    numOfMassPerRow = mesh->numOfMassPerRow;
    numOfMassPerCol = mesh->numOfMassPerCol;
}

void XWVirtualPaper::setStatic(int e, bool flag)
{
    int r = 0;
    int l = 0;
    switch (e)
    {
//        case STATIC_TOP:
//            r = 0;
//            break;
            
//        case STATIC_BOTTOM:
//            r = numOfMassPerRow - 1;
//            break;
            
        case STATIC_LEFT:
            l = 0;
            break;
            
        case STATIC_RIGHT:
            l = numOfMassPerCol - 1;
            break;
            
        default:
            break;
    }
    
    if (r >= 0)
    {
        int start = r * numOfMassPerCol;
        int end = r * numOfMassPerCol + numOfMassPerCol - 1;
        for (int i = start; i < end; i++)
            masses[i].bConstrained = flag;
    }
    else if (l >= 0)
    {
        for (int i = 0; i < numOfMassPerRow; i++)
            masses[i * numOfMassPerCol + l].bConstrained = flag;
    }
}

void XWVirtualPaper::updateTurn(XWPaperMesh * mesh, float dt, int timeStepSubdivision)
{
    if(timeStepSubdivision == 0)
        return ;
                
    h = dt / double(timeStepSubdivision);
	for(int i = 0; i < timeStepSubdivision; i++)
	{
        calcForceAndJacobian();
        calcVelocityChange();
        calcVelocity();
        calcLocation();
        computeNormal();
    }
    
    breakAndFoldEdges();
}

void XWVirtualPaper::breakAndFoldEdges()
{
    int num = getNumOfUsedSprings();
    
    float nij, nrl;
    float lij, lrl;
    float eL0, eLt, rE;
    for(int i = 0; i < num; i++)
    {
        if(!springs[i].breakable) 
            continue;
            
        int vi = springs[i].vi;
        int vj = springs[i].vj;
        int vr = springs[i].vr;
        int vl = springs[i].vl;
        nij = (masses[vi].n).dot(masses[vj].n);
        if(vr >= 0 && vl >= 0) 
            nrl = (masses[vr].n).dot(masses[vl].n);
        else 
            nrl = 1.0;
        
        eL0 = springs[i].l0;
        lij = eLt = (masses[vi].xt - masses[vj].xt).length();
        if(vr>=0 && vl>= 0) 
            lrl = (masses[vr].xt - masses[vl].xt).length();
        else 
            lrl = lij;
        rE = eLt/eL0; 
        float prob1 = 1.0;
        prob1 *= (1.0 - rE) * (1.0 - lrl / (lij + lrl)) * (1 + phi * (nrl - nij) - phi * phi * nij * nrl)/4.0; 
        
        if(eLt > minimumEdgeLength && rE < cataRatio) 
        {
            if(float(rand()%10000)/10000.0 < prob1) 
                breakEdge(i);           
        }
    }
}

void XWVirtualPaper::breakEdge(int eIdx)
{
    int idx = getUsedSpring(eIdx);
    if(!springs[idx].breakable) 
        return;
        
    int en = getSpareSpring();
    if(en < 0) 
        return;
        
    int vn = getSpareMass();
    if (vn < 0)
        return ;
        
    insertUsedMass(vn);
    springs[idx].vn = vn;
        
    int vi = springs[idx].vi;
    int vj = springs[idx].vj;
    int vr = springs[idx].vr;
    int vl = springs[idx].vl;
    
    masses[vn].n  = (masses[vi].n + masses[vj].n ).unitize();
    
    float l0 = springs[idx].l0;
    float l  = (masses[vi].xt - masses[vj].xt).length();
    float ln = ((masses[vi].xt + masses[vi].n) - (masses[vj].xt + masses[vj].n)).length();
    float height = 0.0;
    float psi = 1;
    if(ln > l) 
        height = psi * 0.5 * sqrt(l0*l0 - l*l);
    else 
        height = -psi * 0.5*sqrt(l0*l0 - l*l);
    float alpha = masses[vi].m / (masses[vi].m + masses[vj].m);
    float beta = 1.0 - alpha;
    
    masses[vn].x0 = (masses[vi].x0 + masses[vj].x0) * 0.5;
    masses[vn].xt = (masses[vi].xt + masses[vj].xt) * 0.5 + masses[vn].n * height;
    masses[vn].s  = (masses[vi].s + masses[vj].s) * 0.5;
    masses[vn].t  = (masses[vi].t + masses[vj].t) * 0.5;
    masses[vn].v.set(0, 0, 0);
    float mi = masses[vi].m; 
    float mvi = mi * masses[vi].v;
    float mj = masses[vj].m; 
    float mvj = mj * masses[vj].v;
    
    float mr;
    XWVector3D mvr;
    if(vr >= 0) 
    { 
        mr = masses[vr].m; 
        mvr = mr * masses[vr].v; 
    } 
    else 
    { 
        mr = 0.0; 
        mvr.set(0,0,0); 
    }
    
    float ml;
    XWVector3D mvl;
    if(vl >=0 ) 
    { 
        ml = masses[vl].m; 
        mvl = ml * masses[vl].v; 
    } 
    else 
    { 
        ml = 0.0; 
        mvl.set(0,0,0); 
    }
    
    float totalMass = mi + mj + ml + mr;
    XWVector3D totalMomentum = mvi + mvj + mvl + mvr;
    float di, dj, dl, dr;
    di = ( masses[vi].xt - masses[vn].xt ).length();
    dj = ( masses[vj].xt - masses[vn].xt ).length();
    if(vr >= 0) 
        dr = (masses[vr].xt - masses[vn].xt).length(); 
    else dr = 0.0;
        
    if( vl >=0 ) 
        dl = (masses[vl].xt - masses[vn].xt).length(); 
    else 
        dl = 0.0;

    float distanceSum = di + dj + dl + dr;
    float mp = totalMass - (di * mi + dj * mj + dr * mr + dl * ml) / distanceSum;
    float Mi = di * mi / distanceSum;
    float Mj = dj * mj / distanceSum;
    float Ml = dl * ml / distanceSum;
    float Mr = dr * mr / distanceSum;

    masses[vn].m = mp;
    masses[vn].f = gravity * masses[i].m;
    masses[vn].Jii.set(0,0,0,0,0,0,0,0,0);
    masses[vi].m = Mi;
    masses[vj].m = Mj;
    if(vr >= 0) 
        masses[vr].m = Mr;
    if(vl >= 0) 
        masses[vl].m = Ml;

    XWVector3D MVI = Mi * masses[vi].v;
    XWVector3D MVJ = Mj * masses[vj].v;
    XWVector3D MVR, MVL;
    if(vr >= 0) 
        MVR = Mr * masses[vr].v; 
    else 
        MVR.set(0,0,0);
        
    if(vl >=0 ) 
        MVL = Ml * masses[vl].v; 
    else 
        MVL.set(0,0,0);
        
    XWVector3D newMomentum = MVI + MVJ + MVR + MVL;
    masses[vn].v = (totalMomentum - newMomentum) * (1.0 / mp);
    springs[idx].breakable = false;
    springs[idx].l0 = auxEdgeRatio * l + (1 - auxEdgeRatio) * springs[eIdx].l0;
    insertUsedSpring(en);
    setEdge(en, vi, vn, vr, vl, true);
    en = getSpareSpring();
    if (en < 0)
        return ;
    insertUsedSpring(en);
    setEdge(en, vn, vj, vr, vl, true);
    if(vr >= 0) 
    {
        en = getSpareSpring();
        if (en < 0)
            return ;
        insertUsedSpring(en);
        setEdge(en, vr, vn, vj, vi, true);
    }
    if(vl >= 0) 
    {
        en = getSpareSpring();
        if (en < 0)
            return ;
        insertUsedSpring(en);
        setEdge(en, vn, vl, vj, vi, true);
    }
 
    en = findEdge(vi,vr);
    if(en >= 0) 
        updateEdge(en, vj, vn);
    en = findEdge(vi,vl);
    if(en >= 0) 
        updateEdge(en, vj, vn);
    en = findEdge(vj,vr);
    if(en >= 0) 
        updateEdge(en, vi, vn);
    en = findEdge(vj,vl);
    if(en >= 0) 
        updateEdge(en, vi, vn);
}

void XWVirtualPaper::calcForceAndJacobian()
{
	int v1, v2;
	XWVector3D xij;
	XWVector3D force;
	float forceMag, len, lenSqr;
	
	int ne = getNumOfUsedSprings();
	for(int i = 0; i < ne; i++)
	{
	    int k = getUsedSpring(i);
	    v1 = springs[k].vi;
		v2 = springs[k].vj;
		force = xij= masses[v1].xt - masses[v2].xt;
		len= xij.length();
		lenSqr = len*len;

		if(len!=0.0) 
		    forceMag = (len - springs[k].l0) * (springs[k].stiffness) / len;
		else 
		    forceMag = 0.0;
		    
		force = force * forceMag;
		XWVector3D dampForce = damp * (masses[v1].v - masses[v2].v);
		masses[v1].f = masses[v1].f - force - dampForce - 0.1 * masses[v1].v;
		masses[v2].f = masses[v2].f + force + dampForce - 0.1 * masses[v2].v;

		XWMatrix33 tM;
		float x,y,z;
		xij.get(x,y,z);
		float tM00, tM01, tM02, tM11, tM12, tM22;
		tM00 = x * x / lenSqr; 
		tM01 = x * y / lenSqr; 
		tM02 = x * z / lenSqr;
		tM11 = y * y / lenSqr, 
		tM12 = y * z / lenSqr;
		tM22 = z * z / lenSqr;

		tM.set(tM00, tM01, tM02,
			   tM01, tM11, tM12,
			   tM02, tM12, tM22);

		float J00, J01, J02, J11, J12, J22;
		
		if(len > springs[k].l0) 
		{
			float factor1 = springs[k].stiffness * ( 1.0 - springs[k].l0) / len;
			float factor2 = (springs[k].stiffness - factor1);
			
			J00 = factor2 * tM00 + factor1;
			J01 = factor2 * tM01;
			J02 = factor2 * tM02;
			J11 = factor2 * tM11 + factor1;
			J12 = factor2 * tM12;
			J22 = factor2 * tM22 + factor1;			
		}
		else 
		{
			J00 = springs[k].stiffness * tM00;
			J01 = springs[k].stiffness * tM01;
			J02 = springs[k].stiffness * tM02;
			J11 = springs[k].stiffness * tM11;
			J12 = springs[k].stiffness * tM12;
			J22 = springs[k].stiffness * tM22;
		}
		
		springs[k].Jij.set(J00 + damp, J01, J02,
				           J01, J11 + damp,J12,
				           J02, J12, J22 + damp);
		masses[v1].Jii = masses[v1].Jii - springs[k].Jij;
		masses[v2].Jii = masses[v2].Jii - springs[k].Jij;
	}
	
	int nv = getNumOfUsedMasses();
	for(int i = 0; i < nv; i++) 
	{
	    int k = getUsedMass(i);
		if(masses[k].bConstrained) 
			masses[k].f.set(0,0,0);
	}
}

void XWVirtualPaper::calcLocation()
{
    int nv = getNumOfUsedMasses();
	for(int i = 0; i < nv; i++)
	{ 
	    int k = getUsedMass(i);
        masses[k].xt = masses[k].xt + masses[k].v * h;
    }
}

void XWVirtualPaper::calcVelocity()
{
    int nv = getNumOfUsedMasses();
	for(int i = 0; i < nv; i++)
	{ 
	    int k = getUsedMass(i);
        masses[k].v = masses[k].v + masses[k].dv * h;
    }
}

void XWVirtualPaper::calcVelocityChange()
{
    double hSqr = h*h;
    int nv = getNumOfUsedMasses();
	for(int i = 0; i < nv; i++)
    {
        int k = getUsedMass(i);
        double m = masses[k].m;
        XWMatrix33 Mass(m,0,0,0,m,0,0,0,m);
        masses[k].W = (Mass -  masses[k].Jii * hSqr);

        masses[k].Winv = masses[k].W.getInvert();

        masses[k].pi = masses[k].f * h;
        masses[k].dv.set(0,0,0);
    }
    
    XWVector3D JV, velDiff;
    int ne = getNumOfUsedSprings();
	for(int i = 0; i < ne; i++)
	{
	    int k = getUsedSpring(i);
        int v1 = springs[k].vi;
        int v2 = springs[k].vj;
        velDiff = masses[v1].v - masses[v2].v;
        JV = springs[k].Jij * velDiff;
        masses[v1].pi = masses[v1].pi - (JV * hSqr);
        masses[v2].pi = masses[v2].pi + (JV * hSqr);
    }
    
    for(int iter = 0; iter < 3; iter++) 
    {
	    for(int i = 0; i < nv; i++)
        {
            int k = getUsedMass(i); 
            masses[k].right = masses[k].pi;
        }
            
        if(iter > 0) 
        {
	        for(int i = 0; i < ne; i++) 
            {
                int k = getUsedSpring(i);
                int v1 = springs[k].vi;
                int v2 = springs[k].vj;
                
                JV = springs[k].Jij * masses[v2].dv;
                masses[v1].right = masses[v1].right + (JV * hSqr);
                JV = springs[i].Jij * masses[v1].dv;
                masses[v2].right = masses[v2].right + (JV * hSqr);
            }
        }
        
	    for(int i = 0; i < nv; i++)
        {
            int k = getUsedMass(i);  
            if(masses[k].bConstrained) 
                masses[k].dv.set(0,0,0);
            else 
                masses[k].dv = masses[k].Winv * masses[k].right;
        }
    }
}

void XWVirtualPaper::computeNormal()
{
    int nv = getNumOfUsedMasses();
    for(int i = 0; i < nv; i++) 
    {
        int k = getUsedMass(i);
        masses[k].n.set(0,0,0);
    }
    
    int ne = getNumOfUsedSprings();
    for(int i = 0; i < ne; i++) 
    {
        int k = getUsedSpring(i);
        if(!springs[k].breakable) 
            continue;
            
        int vi = springs[k].vi;
        int vj = springs[k].vj;
        int vr = springs[k].vr;
        int vl = springs[k].vl;

        double x0, x1, x2, y0, y1, y2, z0, z1, z2;
        XWVector3D v1, v2, n;

        if(vr >= 0) 
        {
            masses[vi].xt.get(x0, y0, z0);
            masses[vr].xt.get(x1, y1, z1);
            masses[vj].xt.get(x2, y2, z2);
            v1.set(x1 - x0, y1 - y0, z1 - z0);
            v2.set(x2 - x0, y2 - y0, z2 - z0);
            n = v1.cross(v2);
            n.unitize();
            masses[vi].n = masses[vi].n + n;
            masses[vj].n = masses[vj].n + n;
            masses[vr].n = masses[vr].n + n;
            if(vl >= 0) 
                masses[vl].n = masses[vl].n + n;
        }
        if(vl >= 0) 
        {
            masses[vi].xt.get(x0, y0, z0);
            masses[vj].xt.get(x1, y1, z1);
            masses[vl].xt.get(x2, y2, z2);
            v1.set(x1 - x0, y1 - y0, z1 - z0);
            v2.set(x2 - x0, y2 - y0, z2 - z0);
            n = v1.cross(v2);
            n.unitize();
            masses[vi].n = masses[vi].n + n;
            masses[vj].n = masses[vj].n + n;
            masses[vl].n = masses[vl].n + n;
            if(vr >= 0) 
                masses[vr].n = masses[vr].n + n;            
        }       
    }
    
    for(int i = 0; i < nv; i++) 
    {
        int k = getUsedMass(i);
        masses[k].n.unitize();
    }
}

int XWVirtualPaper::findEdge(int vi, int vj)
{
    int ne = getNumOfUsedSprings();
    for(int i = 0; i < ne; i++) 
    {
        int k = getUsedSpring(i);
        if(!springs[k].breakable) 
            continue;
            
        if(springs[k].vi == vi && springs[k].vj == vj) 
            return k;
            
        if(springs[k].vi == vj && springs[k].vj == vi) 
            return k;
    }
    return -1;
}

void XWVirtualPaper::recoverEdge()
{
    int ne = getNumOfUsedSprings();
    int eij, ein, enj, ern, enl;
    float nrl;    
    for(int i = 0; i < ne; i++) 
    {
        int eij = getUsedSpring(i);
        if (springs[eij].breakable)
            continue;
            
        int vi = springs[eij].vi;
        int vj = springs[eij].vj;
        int vr = springs[eij].vr;
        int vl = springs[eij].vl;
        int vn = springs[eij].vn;
        ein = findEdge(vi, vn);
        enj = findEdge(vj, vn);
        ern = findEdge(vr, vn);
        enl = findEdge(vl, vn);
        if (springs[ein].breakable && 
            springs[enj].breakable && 
            (ern == -1 || springs[ern].breakable) && 
            (enl == -1 || springs[enl].breakable))
        {
            if (ern >= 0 && enl >= 0)
                nrl = masses[vr].n.dot(masses[vl].n);
            else 
                nrl = 1.0;
                
            float beta = (1 - nrl) *0.5;
            if (beta > epsilon)
            {
                float mi = masses[vi].m;
                float mj = masses[vj].m;
                float mr, ml;
                if (vl >= 0)
                    mr = masses[vl].m;
                else
                    mr = 0.0;
                    
                if (vl >= 0)
                    ml = masses[vl].m;
                else
                    ml = 0.0;
                    
                float di, dj, dl, dr;
                di = (masses[vi].xt - masses[vn].xt).length();
		        dj = (masses[vj].xt - masses[vn].xt).length();
		        if(vr >=0 ) 
		            dr = (masses[vr].xt - masses[vn].xt).length();
		        else 
		            dr = 0.0;
		            
		        if(vl >=0 ) 
		            dl = ( masses[vl].xt - masses[vn].xt ).length();
		        else 
		            dl = 0.0;
		            
		        float sum = di * mi + dj * mj + dl * ml + dr * mr;
                masses[vi].m = sum / di;
                masses[vj].m = sum / dj;
                if (vl >= 0)
                    masses[vl].m = sum / dl;
                    
                if (vr >= 0)
                    masses[vr].m = sum / dr;
                    
                float totalMass = mi + mj + ml + mr;
                XWVector3D mvn = masses[vn].m * masses[vn].v;
                masses[vi].v = masses[vi].v + mvn / totalMass;
                masses[vj].v = masses[vj].v + mvn / totalMass;
                if (vl >= 0)
                    masses[vl].v = masses[vl].v + mvn / totalMass;
                    
                if (vr >= 0)
                    masses[vr].v = masses[vr].v + mvn / totalMass;
                    
                springs[eij].breakable = true;
                springs[eij].vn = -1;
                removeUsedSprings(ein);
                inserSpareSpring(ein);
                removeUsedSprings(enj);
                inserSpareSpring(enj);
                if (ern >= 0)
                {
                    removeUsedSprings(ern);
                    inserSpareSpring(ern);
                }
                
                if (enl >= 0)
                {
                    removeUsedSprings(enl);
                    inserSpareSpring(enl);
                }
                
                removeUsedMass(vn);
                insertUsedMass(vn);
            }
        }
    }
}

void XWVirtualPaper::setEdge(int idx, 
                             int vi, 
                             int vj, 
                             int vr, 
                             int vl, 
                             bool breakableFlag)
{
    springs[idx].vi = vi;
    springs[idx].vj = vj;
    springs[idx].vr = vr;
    springs[idx].vl = vl;
    springs[idx].l0 = (masses[vi].x0 - masses[vj].x0).length();
    springs[idx].stiffness = stiffness;
    springs[idx].breakable = breakableFlag;
}

void XWVirtualPaper::updateEdge(int idx, int vprev, int vnew)
{
    if(springs[idx].vl == vprev) 
        springs[idx].vl = vnew;
    else if(springs[idx].vr == vprev) 
        springs[idx].vr = vnew;
}

