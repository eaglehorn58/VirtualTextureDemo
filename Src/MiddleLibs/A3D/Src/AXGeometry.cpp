//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


//	#include "A3DPI.h"
#include "AXGeometry.h"
#include "AXMatrix.h"
#include "AXFuncs.h"
//	#include "ATempMemBuffer.h"

///////////////////////////////////////////////////////////////////////////

#define ROTATE(a,i,j,k,l) g=a.m[i][j]; h=a.m[k][l]; a.m[i][j]=(float)(g-s*(h+g*tau)); a.m[k][l]=(float)(h+s*(g-h*tau));

///////////////////////////////////////////////////////////////////////////

static AXMatrix3 _GetConvarianceMatrix(const AXVector3* aVertPos, int nVertCount)
{
	int i;
	AXMatrix3 Cov;

	double S1[3];
	double S2[3][3];

	S1[0] = S1[1] = S1[2] = 0.0;
	S2[0][0] = S2[1][0] = S2[2][0] = 0.0;
	S2[0][1] = S2[1][1] = S2[2][1] = 0.0;
	S2[0][2] = S2[1][2] = S2[2][2] = 0.0;

	// get center of mass
	for(i=0; i<nVertCount; i++)
	{
		S1[0] += aVertPos[i].x;
		S1[1] += aVertPos[i].y;
		S1[2] += aVertPos[i].z;

		S2[0][0] += aVertPos[i].x * aVertPos[i].x;
		S2[1][1] += aVertPos[i].y * aVertPos[i].y;
		S2[2][2] += aVertPos[i].z * aVertPos[i].z;
		S2[0][1] += aVertPos[i].x * aVertPos[i].y;
		S2[0][2] += aVertPos[i].x * aVertPos[i].z;
		S2[1][2] += aVertPos[i].y * aVertPos[i].z;
	}

	float n = (float)nVertCount;
	// now get covariances
	Cov.m[0][0] = (float)(S2[0][0] - S1[0]*S1[0] / n) / n;
	Cov.m[1][1] = (float)(S2[1][1] - S1[1]*S1[1] / n) / n;
	Cov.m[2][2] = (float)(S2[2][2] - S1[2]*S1[2] / n) / n;
	Cov.m[0][1] = (float)(S2[0][1] - S1[0]*S1[1] / n) / n;
	Cov.m[1][2] = (float)(S2[1][2] - S1[1]*S1[2] / n) / n;
	Cov.m[0][2] = (float)(S2[0][2] - S1[0]*S1[2] / n) / n;
	Cov.m[1][0] = Cov.m[0][1];
	Cov.m[2][0] = Cov.m[0][2];
	Cov.m[2][1] = Cov.m[1][2];

	return Cov;
}

static void _GetEigenVectors(AXMatrix3* vout, AXVector3* dout, AXMatrix3 a)
{
	int n = 3;
	int j,iq,ip,i;
	double tresh,theta,tau,t,sm,s,h,g,c;
	int nrot;
	AXVector3 b;
	AXVector3 z;
	AXMatrix3 v;
	AXVector3 d;

	v = a3d_IdentityMatrix3();
	for(ip=0; ip<n; ip++) 
	{
		b.m[ip] = a.m[ip][ip];
		d.m[ip] = a.m[ip][ip];
		z.m[ip] = 0.0;
	}
  
	nrot = 0;

	for(i=0; i<50; i++)
	{
		sm=0.0;
		for(ip=0; ip<n; ip++) for(iq=ip+1;iq<n;iq++) sm+=fabs(a.m[ip][iq]);
		if( sm == 0.0 )
		{
			v.Transpose();
			*vout = v;
			*dout = d;
			return;
		}
      
		if (i < 3) tresh = 0.2 * sm / (n*n);
		else tresh=0.0;
      
		for(ip=0; ip<n; ip++) 
		{
			for(iq=ip+1; iq<n; iq++)
			{
				g = 100.0*fabs(a.m[ip][iq]);
				if( i>3 && fabs(d.m[ip])+g==fabs(d.m[ip]) && fabs(d.m[iq])+g==fabs(d.m[iq]) )
					a.m[ip][iq]=0.0;
				else if (fabs(a.m[ip][iq])>tresh)
				{
					h = d.m[iq]-d.m[ip];
					if (fabs(h)+g == fabs(h)) 
						t=(a.m[ip][iq])/h;
					else
					{
						theta=0.5*h/(a.m[ip][iq]);
						t=1.0/(fabs(theta)+sqrt(1.0+theta*theta));
						if (theta < 0.0) t = -t;
					}
					c=1.0/sqrt(1+t*t);
					s=t*c;
					tau=s/(1.0+c);
					h=t*a.m[ip][iq];
					z.m[ip] -= (float)h;
					z.m[iq] += (float)h;
					d.m[ip] -= (float)h;
					d.m[iq] += (float)h;
					a.m[ip][iq]=0.0;
					for(j=0;j<ip;j++) { ROTATE(a,j,ip,j,iq); } 
					for(j=ip+1;j<iq;j++) { ROTATE(a,ip,j,j,iq); } 
					for(j=iq+1;j<n;j++) { ROTATE(a,ip,j,iq,j); } 
					for(j=0;j<n;j++) { ROTATE(v,j,ip,j,iq); } 
					nrot++;
				}
			}
		}
	    for(ip=0;ip<n;ip++)
		{
			b.m[ip] += z.m[ip];
			d.m[ip] = b.m[ip];
			z.m[ip] = 0.0;
		}
    }

	v.Transpose();
	*vout = v;
	*dout = d;
	return;
}

//	return an OBB extracing from the vertices;
static AXMatrix3 _GetOBBOrientation(const AXVector3* aVertPos, int nVertCount)
{
	AXMatrix3 Cov;

	if (nVertCount <= 0)
		return a3d_IdentityMatrix3();

	Cov = _GetConvarianceMatrix(aVertPos, nVertCount);

	// now get eigenvectors
	AXMatrix3 Evecs;
	AXVector3 Evals;
	_GetEigenVectors(&Evecs, &Evals, Cov);

	return Evecs;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement AXAABB
//	
///////////////////////////////////////////////////////////////////////////

//	Add a vertex to aabb
void AXAABB::AddVertex(const AXVector3& v)
{
	for (int i=0; i < 3; i++)
	{
		if (v.m[i] < Mins.m[i])
			Mins.m[i] = v.m[i];
	
		if (v.m[i] > Maxs.m[i])
			Maxs.m[i] = v.m[i];
	}
}

//	Expand aabb
void AXAABB::Build(const AXOBB& obb)
{
	Clear();
	a3d_ExpandAABB(Mins, Maxs, obb);
	CompleteCenterExts();
}

//	Expand aabb
void AXAABB::Merge(const AXAABB& subAABB)
{
	Mins = a3d_VecMin(Mins, subAABB.Mins);
	Maxs = a3d_VecMax(Maxs, subAABB.Maxs);
	CompleteCenterExts();
}

//	Check whether another aabb is in this aabb
bool AXAABB::IsAABBIn(const AXAABB& aabb) const
{
	AXVector3 vDelta = aabb.Center - Center;

	vDelta.x = (float)fabs(vDelta.x);
	if (vDelta.x + aabb.Extents.x > Extents.x)
		return false;

	vDelta.y = (float)fabs(vDelta.y);
	if (vDelta.y + aabb.Extents.y > Extents.y)
		return false;

	vDelta.z = (float)fabs(vDelta.z);
	if (vDelta.z + aabb.Extents.z > Extents.z)
		return false;

	return true;
}

bool AXAABB::IsIntersect(const AXAABB& aabb) const
{
	if (Mins.x > aabb.Maxs.x || Maxs.x < aabb.Mins.x ||
		Mins.y > aabb.Maxs.y || Maxs.y < aabb.Mins.y ||
		Mins.z > aabb.Maxs.z || Maxs.z < aabb.Mins.z)
	{
		return false;
	}

	return true;
}

//	Build AABB from vertices
void AXAABB::Build(const AXVector3* aVertPos, int iNumVert)
{
	Clear();

	if (iNumVert <= 0)
		return;

	for (int i=0; i < iNumVert; i++)
	{
		Maxs = a3d_VecMax(Maxs, aVertPos[i]);
		Mins = a3d_VecMin(Mins, aVertPos[i]);
	}

	CompleteCenterExts();
}

//	Get vertices of aabb
void AXAABB::GetVertices(AXVector3* aVertPos, unsigned short* aIndices, bool bWire) const
{
	AXOBB obb;

	obb.Center	= Center;
	obb.Extents = Extents;
	obb.XAxis.Set(1.0f, 0.0f, 0.0f);
	obb.YAxis.Set(0.0f, 1.0f, 0.0f);
	obb.ZAxis.Set(0.0f, 0.0f, 1.0f);
	obb.CompleteExtAxis();

	obb.GetVertices(aVertPos, aIndices, bWire);
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement AXOBB
//	
///////////////////////////////////////////////////////////////////////////

//	Check whether a point is in this obb
bool AXOBB::IsPointIn(const AXVector3& v) const
{
	AXVector3 vd = v - Center;

	//	Transfrom point to obb space
	float d = DotProduct(XAxis, vd);
	if (d < -Extents.x || d > Extents.x)
		return false;

	d = DotProduct(YAxis, vd);
	if (d < -Extents.y || d > Extents.y)
		return false;

	d = DotProduct(ZAxis, vd);
	if (d < -Extents.z || d > Extents.z)
		return false;

	return true;
}



//	Clear obb
void AXOBB::Clear()
{
	memset(this, 0, sizeof (AXOBB));
}

//	Build obb from two obbs
void AXOBB::Build(const AXOBB& obb1, const AXOBB obb2)
{
	Clear();

	AXVector3 aVertPos[16];
	obb1.GetVertices(&aVertPos[0], NULL, true);
	obb2.GetVertices(&aVertPos[8], NULL, true);
	
	AXMatrix3 matTransform = _GetOBBOrientation(aVertPos, 16);
	//	For matTransform is orthogonal, so the inverse matrix is just rotate it;
 	matTransform.Transpose();

	AXVector3 vecMin = aVertPos[0] * matTransform;
	AXVector3 vecMax = vecMin;
	
	for (int i=1; i < 16; i++)
	{
		AXVector3 vecThis = aVertPos[i] * matTransform;

		vecMax = a3d_VecMax(vecMax, vecThis);
		vecMin = a3d_VecMin(vecMin, vecThis);
	}

	matTransform.Transpose();

	AXVector3 row0 = matTransform.GetRow(0);
	AXVector3 row1 = matTransform.GetRow(1);
	AXVector3 row2 = matTransform.GetRow(2);

	Center	= 0.5f * (vecMax + vecMin) * matTransform;
	XAxis	= Normalize(row0);
	YAxis	= Normalize(row1);
	ZAxis	= Normalize(row2);
	Extents = 0.5f * (vecMax - vecMin);

	CompleteExtAxis();
}

//	Build obb from vertices
void AXOBB::Build(const AXVector3* aVertPos, int nVertCount)
{
	Clear();

	if (nVertCount <= 0)
		return;

	AXMatrix3 matTransform = _GetOBBOrientation(aVertPos, nVertCount);

	//	For matTransform is orthogonal, so the inverse matrix is just rotate it;
 	matTransform.Transpose();

	AXVector3 vecMax = AXVector3(aVertPos[0].x, aVertPos[0].y, aVertPos[0].z) * matTransform;
	AXVector3 vecMin = vecMax;
	
	for (int i=1; i < nVertCount; i++)
	{
		AXVector3 vecThis = AXVector3(aVertPos[i].x, aVertPos[i].y, aVertPos[i].z) * matTransform;

		vecMax = a3d_VecMax(vecMax, vecThis);
		vecMin = a3d_VecMin(vecMin, vecThis);
	}

	matTransform.Transpose();

	AXVector3 row0 = matTransform.GetRow(0);
	AXVector3 row1 = matTransform.GetRow(1);
	AXVector3 row2 = matTransform.GetRow(2);

	Center	= 0.5f * (vecMax + vecMin) * matTransform;
	XAxis	= Normalize(row0);
	YAxis	= Normalize(row1);
	ZAxis	= Normalize(row2);
	Extents = 0.5f * (vecMax - vecMin);

	CompleteExtAxis();
}

//	Build obb from aabb
void AXOBB::Build(const AXAABB& aabb)
{
	Center	= aabb.Center;
	XAxis.Set(1.0f, 0.0f, 0.0f);
	YAxis.Set(0.0f, 1.0f, 0.0f);
	ZAxis.Set(0.0f, 0.0f, 1.0f);
	Extents	= aabb.Extents;

	CompleteExtAxis();
}

//	Get vertices of obb
void AXOBB::GetVertices(AXVector3* aVertPos, unsigned short* aIndices, bool bWire) const
{
	static unsigned short indexTriangle[] =
	{
		0, 1, 3, 3, 1, 2, 
		2, 1, 6, 6, 1, 5, 
		5, 1, 4, 4, 1, 0,
		0, 3, 4, 4, 3, 7, 
		3, 2, 7, 7, 2, 6, 
		7, 6, 4, 4, 6, 5
	};

	static unsigned short indexWire[] =
	{
		0, 1, 1, 2, 2, 3, 3, 0, 
		0, 4, 1, 5, 2, 6, 3, 7,
		4, 5, 5, 6, 6, 7, 7, 4
	};

	if (aVertPos)
	{
		//	Up 4 vertex;
		//	Left Up corner;
		aVertPos[0] = Center - ExtX + ExtY + ExtZ;
		//	right up corner;
		aVertPos[1] = aVertPos[0] + 2.0f * ExtX;
		//	right bottom corner;
		aVertPos[2] = aVertPos[1] - 2.0f * ExtZ;
		//	left bottom corner;
		aVertPos[3] = aVertPos[2] - 2.0f * ExtX;

		//	Down 4 vertex;
		//	Left up corner;
		aVertPos[4] = Center - ExtX - ExtY + ExtZ;
		//	right up corner;
		aVertPos[5] = aVertPos[4] + 2.0f * ExtX;
		//	right bottom corner;
		aVertPos[6] = aVertPos[5] - 2.0f * ExtZ;
		//	left bottom corner;
		aVertPos[7] = aVertPos[6] - 2.0f * ExtX;
	}

	if (aIndices)
	{
		if (bWire)
			memcpy(aIndices, indexWire, sizeof (unsigned short) * 24);
		else
			memcpy(aIndices, indexTriangle, sizeof (unsigned short) * 36);
	}
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement AXPLANE and AXSPLANE
//	
///////////////////////////////////////////////////////////////////////////

//	Create plane from 3 points
bool AXPLANE::CreatePlane(const AXVector3& v1, const AXVector3& v2, const AXVector3& v3)
{
	AXVector3 d1 = v2 - v1;
	AXVector3 d2 = v3 - v1;

	vNormal.CrossProduct(d1, d2);
	vNormal.Normalize();
	fDist = DotProduct(vNormal, v1);

	return true;
}

//	Make plane type
void AXSPLANE::MakeType()
{
	if (vNormal.x == 1.0f)
		byType = TYPE_PX;
	else if (vNormal.x == -1.0f)
		byType = TYPE_NX;
	else if (vNormal.y == 1.0f)
		byType = TYPE_PY;
	else if (vNormal.y == -1.0f)
		byType = TYPE_NY;
	else if (vNormal.z == 1.0f)
		byType = TYPE_PZ;
	else if (vNormal.z == -1.0f)
		byType = TYPE_NZ;
	else
	{
		byType = TYPE_MAJORX;

		//	Get normal's major axis
		float fTemp, fMax = (float)fabs(vNormal.x);
		
		fTemp = (float)fabs(vNormal.y);
		if (fMax < fTemp)
		{
			fMax = fTemp;
			byType = TYPE_MAJORY;
		}
		
		if (fMax < (float)fabs(vNormal.z))
			byType = TYPE_MAJORZ;
	}
}

//	Make plane sign-bits
void AXSPLANE::MakeSignBits()
{
	bySignBits = 0;

	if (vNormal.x < 0.0f)
		bySignBits |= SIGN_NX;
	
	if (vNormal.y < 0.0f)
		bySignBits |= SIGN_NY;

	if (vNormal.z < 0.0f)
		bySignBits |= SIGN_NZ;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement AXCAPSULE
//	
///////////////////////////////////////////////////////////////////////////

//	Check whether a point is in this capsule
bool AXCAPSULE::IsPointIn(const AXVector3& vPos)
{
	AXVector3 vDelta = vPos - vCenter;

	//	The capped cylinder Is a sphere ?
	if (fHalfLen == 0.0f)
	{
		if (DotProduct(vDelta, vDelta) <= fRadius * fRadius)
			return true;

		return false;
	}

	float fpx = (float)fabs(DotProduct(vDelta, vAxisX));
	if (fpx > fRadius)
		return false;

	float fpz = (float)fabs(DotProduct(vDelta, vAxisZ));
	if (fpz > fRadius)
		return false;

	if (fpx * fpx + fpz * fpz > fRadius * fRadius)
		return false;

	float fpy = DotProduct(vDelta, vAxisY);
	if (fabs(fpy) < fHalfLen)
		return true;

	if (fpy > 0.0f)
		vDelta = vPos - (vCenter + vAxisY * fHalfLen);
	else
		vDelta = vPos - (vCenter - vAxisY * fHalfLen);

	if (DotProduct(vDelta, vDelta) <= fRadius * fRadius)
		return true;

	return false;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement AXCYLINDER
//	
///////////////////////////////////////////////////////////////////////////

//	Check whether a point is in this cylinder
bool AXCYLINDER::IsPointIn(const AXVector3& vPos)
{
	AXVector3 vDelta = vPos - vCenter;

	float fpx = (float)fabs(DotProduct(vDelta, vAxisX));
	if (fpx > fRadius)
		return false;

	float fpz = (float)fabs(DotProduct(vDelta, vAxisZ));
	if (fpz > fRadius)
		return false;

	if (fpx * fpx + fpz * fpz > fRadius * fRadius)
		return false;

	float fpy = DotProduct(vDelta, vAxisY);
	if (fabs(fpy) > fHalfLen)
		return false;

	return true;
}


///////////////////////////////////////////////////////////////////////////
//	
//	Implement global functions
//	
///////////////////////////////////////////////////////////////////////////

//	Create a indexed sphere mesh
bool a3d_CreateIndexedSphere(const AXVector3& vPos, float fRadius, int iRow, int iCol, 
							 AXVector3* aVerts, int iMaxVertNum, unsigned short* aIndices, int iMaxIdxNum)
{
	a_ClampFloor(iRow, 3);
	a_ClampFloor(iCol, 3);

	int iNumVert = 2 + (iRow - 1) * iCol;
	if (iNumVert > iMaxVertNum)
	{
		ASSERT(iNumVert <= iMaxVertNum);
		return false;
	}

	int iNumIndex = (iRow - 1) * iCol * 6;
	if (iNumIndex > iMaxIdxNum)
	{
		ASSERT(iNumIndex <= iMaxIdxNum);
		return false;
	}

	float fRowAngle = A3D_PI / iRow;
	float fColAngle = A3D_2PI / iCol;
	int i, j;

	//	Pre-calculate cos and sin value
	float* aCos = (float*)malloc(sizeof(float) * iCol);
	float* aSin = (float*)malloc(sizeof(float) * iCol);

	for (i=0; i < iCol; i++)
	{
		float fAngle = fColAngle * i;
		aCos[i] = cosf(fAngle);
		aSin[i] = sinf(fAngle);
	}

	AXVector3* pv = aVerts;

	//	Top center vertex
	pv->Set(vPos.x, vPos.y + fRadius, vPos.z);
	pv++;

	//	Body vertices
	for (i=1; i < iRow; i++)
	{
		float fAngle = A3D_PI * 0.5f - i * fRowAngle;
		float y = fRadius * sinf(fAngle);
		float fProject = (float)fabs(fRadius * cosf(fAngle));

		for (j=0; j < iCol; j++)
		{
			float x = fProject * aCos[j];
			float z = fProject * aSin[j];

			pv->Set(vPos.x + x, vPos.y + y, vPos.z + z);
			pv++;
		}
	}

	free(aCos);
	free(aSin);
	aCos = aSin = NULL;

	//	Bottom center vertex
	pv->Set(vPos.x, vPos.y - fRadius, vPos.z);
	pv++;

	int iVertStart = 0;
	unsigned short* pi = aIndices;

	//	Top cap indices
	for (i=0; i < iCol; i++)
	{
		int iBaseIdx = iVertStart + 1;
		j = (i + 1) % iCol;

		*pi++ = (unsigned short)iVertStart;
		*pi++ = (unsigned short)(iBaseIdx + j);
		*pi++ = (unsigned short)(iBaseIdx + i);
	}

	//	Body indices
	int i1 = iVertStart + 1;
	int i2 = iVertStart + 1 + iCol;

	for (i=0; i < iRow-2; i++)
	{
		for (j=0; j < iCol; j++)
		{
			int k = (j + 1) % iCol;

			*pi++ = (unsigned short)(i1 + j);
			*pi++ = (unsigned short)(i1 + k);
			*pi++ = (unsigned short)(i2 + k);

			*pi++ = (unsigned short)(i1 + j);
			*pi++ = (unsigned short)(i2 + k);
			*pi++ = (unsigned short)(i2 + j);
		}

		i1 += iCol;
		i2 += iCol;
	}

	//	Bottom cap indices
	for (i=0; i < iCol; i++)
	{
		int iLastIdx = iNumVert - 1;
		int iBaseIdx = iLastIdx - iCol;
		j = (i + 1) % iCol;

		*pi++ = (unsigned short)(iBaseIdx + i);
		*pi++ = (unsigned short)(iBaseIdx + j);
		*pi++ = (unsigned short)(iLastIdx);
	}

	ASSERT(iNumVert == pv - aVerts);
	ASSERT(iNumIndex == pi - aIndices);

	return true;
}

//	Create a indexed taper mesh
bool a3d_CreateIndexedTaper(const AXVector3& vPos, const AXVector3& vDir,
							float fRadius, float fHeight, int iNumSeg, bool bHasBottom, 
							AXVector3* aVerts, int iMaxVertNum, unsigned short* aIndices, int iMaxIdxNum)
{
	a_ClampFloor(iNumSeg, 3);

	int iNumVert = iNumSeg + 1;
	if (iNumVert > iMaxVertNum)
	{
		ASSERT(iNumVert <= iMaxVertNum);
		return false;
	}

	int iNumIndex = iNumSeg * 3;
	if (bHasBottom)
		iNumIndex += (iNumSeg - 2) * 3;

	if (iNumIndex > iMaxIdxNum)
	{
		ASSERT(iNumIndex <= iMaxIdxNum);
		return false;
	}

	//	Calculate x and y axis of taper, vDir is treated as z axis
	AXVector3 vX, vY;

	if (1.0f - fabs(vDir.y) < 0.0001f)
		vX = AXVector3(1,0,0);//AXVector3::vAxisX;
	else
		vX = a3d_Normalize(CrossProduct(AXVector3(0,1,0)/*AXVector3::vAxisY*/, vDir));

	vY = a3d_Normalize(CrossProduct(vDir, vX));

	int i;
	AXVector3* pv = aVerts;

	//	Top vertex
	pv->Set(vPos.x, vPos.y, vPos.z);
	pv++;

	AXVector3 p1 = vPos + vDir * fHeight;

	float fDelta = 360.0f / iNumSeg;
	float fDegree = 0.0f;

	//	Bottom vertices
	for (i=0; i < iNumSeg; i++)
	{
		float fRad = DEG2RAD(fDegree);
		fDegree += fDelta;

		AXVector3 vt = vX * (float)cosf(fRad) + vY * (float)sinf(fRad);
		vt *= fRadius;

		pv->Set(p1.x + vt.x, p1.y + vt.y, p1.z + vt.z);
		pv++;
	}

	unsigned short* pi = aIndices;

	//	Bottom bottom cap indices
	if (bHasBottom)
	{
		for (i=2; i < iNumSeg; i++)
		{
			*pi++ = 1;
			*pi++ = 1 + i - 1;
			*pi++ = 1 + i;
		}
	}

	//	Build sides indices
	for (i=0; i < iNumSeg; i++)
	{
		int j = (i+1) % iNumSeg;
		*pi++ = 0;
		*pi++ = 1 + j;
		*pi++ = 1 + i;
	}

	ASSERT(iNumVert == pv - aVerts);
	ASSERT(iNumIndex == pi - aIndices);

	return true;
}

//	Create a indexed box that has 8 vertices and 36 indices
//	vExtent: half border length
bool a3d_CreateIndexedBox(const AXVector3& vPos, const AXVector3& vX,
									 const AXVector3& vY, const AXVector3& vZ, const AXVector3& vExtent,
									 AXVector3* aVerts, unsigned short* aIndices)
{
	//	Indices for flat box
	static const unsigned short aSrcIndices[36] =
	{
		0, 1, 2, 0, 2, 3, 
		1, 5, 6, 1, 6, 2,
		2, 6, 7, 2, 7, 3,
		3, 7, 4, 3, 4, 0,
		4, 5, 1, 4, 1, 0,
		5, 7, 6, 5, 4, 7,
	};

	memcpy(aIndices, aSrcIndices, 36 * sizeof (unsigned short));

	float fx2 = vExtent.x * 2.0f;
	float fy2 = vExtent.y * 2.0f;
	float fz2 = vExtent.z * 2.0f;

	aVerts[0] = vPos + vZ * vExtent.z + vY * vExtent.y - vX * vExtent.x;
	aVerts[1] = aVerts[0] + vX * fx2;
	aVerts[2] = aVerts[1] - vZ * fz2;
	aVerts[3] = aVerts[0] - vZ * fz2;
	aVerts[4] = aVerts[0] - vY * fy2;
	aVerts[5] = aVerts[4] + vX * fx2;
	aVerts[6] = aVerts[5] - vZ * fz2;
	aVerts[7] = aVerts[4] - vZ * fz2;

	return true;
}

/*
#define OBB_MERGE_METHOD 2
AXOBB AXOBBMergeHelper::Merge(AXOBBMergeHelper const* apOBBHelper, aint32 nHelper, AXOBB const* apOBB, aint32 nOBB)
{
	if (nOBB == 0)
	{
		AXOBB nullObb;
		nullObb.Clear();
		return nullObb;
	}


	AXOBBMergeHelper obbHelper;

	auto Method1 = [&]
	{
		for (aint32 i = 0; i < nOBB; i++)
		{
			AXOBBMergeHelper const& o = apOBBHelper[i];
			obbHelper.firstOrder += o.firstOrder;
			obbHelper.secondOrder0 += o.secondOrder0;
			obbHelper.secondOrder1 += o.secondOrder1;
			obbHelper.nVertexCount += o.nVertexCount;
		}
	};

	auto Method2 = [&]
	{
		for (aint32 i = 0; i < nOBB; i++)
		{
			// 		aint32 nVertexCount = apOBBHelper[i].nVertexCount;
			AXOBB const& obb = apOBB[i];

			AXMatrix3 eigenVectors(
				obb.XAxis.x, obb.YAxis.x, obb.ZAxis.x,
				obb.XAxis.y, obb.YAxis.y, obb.ZAxis.y,
				obb.XAxis.z, obb.YAxis.z, obb.ZAxis.z);

			AXMatrix3 tranposedEigenVectors = eigenVectors;
			tranposedEigenVectors.Transpose();

			AXMatrix3 eigenValues(
				obb.Extents.x, 0, 0,
				0, obb.Extents.y, 0,
				0, 0, obb.Extents.z
				);

			AXMatrix3 covariance = eigenVectors * eigenValues * tranposedEigenVectors;

			DV3 m = obb.Center;

			aint32 nVertexCount = 1; // 
			DV3 firstOrder = m * nVertexCount;
			DV3 secondOrder0 = nVertexCount * DV3(covariance.m[0][0], covariance.m[1][1], covariance.m[2][2]) + (1. / nVertexCount) * m * m;
			DV3 secondOrder1 = nVertexCount * DV3(covariance.m[0][1], covariance.m[1][2], covariance.m[0][2]) + (1. / nVertexCount) * DV3(m.x * m.y, m.y * m.z, m.z * m.x);
			obbHelper.nVertexCount += nVertexCount;
			obbHelper.firstOrder += firstOrder;
			obbHelper.secondOrder0 += secondOrder0;
			obbHelper.secondOrder1 += secondOrder1;
		}
	};

#if (OBB_MERGE_METHOD == 1)

	if (nHelper != nOBB)
	{
		Method2();
	}
	else
	{
		Method1();
	}
#elif (OBB_MERGE_METHOD == 2)
	Method2();
#endif

	
	AXMatrix3 finalCovariance;

	afloat64 inverseN = 1. / obbHelper.nVertexCount;

	DV3 firstOrder = obbHelper.firstOrder;
	DV3 secondOrder0 = obbHelper.secondOrder0;
	DV3 secondOrder1 = obbHelper.secondOrder1;

	finalCovariance.m[0][0] = afloat32((secondOrder0.x - firstOrder.x * firstOrder.x * inverseN) * inverseN);
	finalCovariance.m[1][1] = afloat32((secondOrder0.y - firstOrder.y * firstOrder.y * inverseN) * inverseN);
	finalCovariance.m[2][2] = afloat32((secondOrder0.z - firstOrder.z * firstOrder.z * inverseN) * inverseN);

	finalCovariance.m[0][1] = afloat32((secondOrder1.x - firstOrder.x * firstOrder.x * inverseN) * inverseN);
	finalCovariance.m[1][2] = afloat32((secondOrder1.y - firstOrder.y * firstOrder.z * inverseN) * inverseN);
	finalCovariance.m[0][2] = afloat32((secondOrder1.z - firstOrder.x * firstOrder.z * inverseN) * inverseN);

	finalCovariance.m[1][0] = finalCovariance.m[0][1];
	finalCovariance.m[2][0] = finalCovariance.m[0][2];
	finalCovariance.m[2][1] = finalCovariance.m[1][2];
	
	AXMatrix3 Evecs;
	AXVector3 Evals;
	_GetEigenVectors(&Evecs, &Evals, finalCovariance);

	AXMatrix3 matTransform = Evecs;
	//	For matTransform is orthogonal, so the inverse matrix is just rotate it;
	matTransform.Transpose();

	ADyncArray<AXVector3> aVertPos;
	aVertPos.SetNum(8 * nOBB);
	for (aint32 i = 0; i < nOBB; i++)
	{
		apOBB[i].GetVertices(aVertPos.GetData() + i * 8, nullptr, true);
	}

	AXVector3 vecMax = AXVector3(aVertPos[0].x, aVertPos[0].y, aVertPos[0].z) * matTransform;
	AXVector3 vecMin = vecMax;

	for (int i = 1; i < aVertPos.Num(); i++)
	{
		AXVector3 vecThis = AXVector3(aVertPos[i].x, aVertPos[i].y, aVertPos[i].z) * matTransform;

		vecMax = a3d_VecMax(vecMax, vecThis);
		vecMin = a3d_VecMin(vecMin, vecThis);
	}

	matTransform.Transpose();

	AXVector3 row0 = matTransform.GetRow(0);
	AXVector3 row1 = matTransform.GetRow(1);
	AXVector3 row2 = matTransform.GetRow(2);

	AXOBB finalOBB;
	finalOBB.Center = 0.5f * (vecMax + vecMin) * matTransform;
	finalOBB.XAxis = Normalize(row0);
	finalOBB.YAxis = Normalize(row1);
	finalOBB.ZAxis = Normalize(row2);
	finalOBB.Extents = 0.5f * (vecMax - vecMin);

	finalOBB.CompleteExtAxis();
	return finalOBB;
}

AXOBBMergeHelper AXOBBMergeHelper::Create(const AXVector3* aVertPos, int nVertCount)
{
	AXOBBMergeHelper obb;
	obb.nVertexCount = nVertCount;
	for (aint32 i = 0; i < nVertCount; i++)
	{
		DV3 v = aVertPos[i];
		obb.firstOrder += v;
		obb.secondOrder0 += v * v;
		obb.secondOrder1 += DV3(v.x * v.y, v.y * v.z, v.z * v.x);
	}
	return obb;
}

*/
