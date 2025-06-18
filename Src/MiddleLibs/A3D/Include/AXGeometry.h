//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#ifndef _AXGEOMETRY_H_
#define _AXGEOMETRY_H_

#include "ATypes.h"
#include "AXVector.h"
//#include "AMemBase.h"

///////////////////////////////////////////////////////////////////////////

class AXOBB;

/*	Create a indexed sphere mesh. This function build a sphere mesh whose center locates at
	vPos and has radius specified by fRadius. Sphere's segments are specified by iRow (>= 3) 
	and iCol (>= 3). The final mesh will have below vertex number and index number
	vertex number: (2 + (iRow-1) * iCol)
	index number: ((iRow-1) * iCol * 6)
*/
bool a3d_CreateIndexedSphere(const AXVector3& vPos, float fRadius, int iRow, int iCol, 
					AXVector3* aVerts, int iMaxVertNum, unsigned short* aIndices, int iMaxIdxNum);

/*	Create a indexed taper mesh. This function build a taper mesh whose top locates at
	vPos and vDir specified it's height axis. 
	The final mesh will have below vertex number and index number

	if (bHasBottom == true)
	{
		vertex number: (iNumSeg + 1)
		index number: iNumSeg * 3 + (iNumSeg - 2) * 3
	}
	else
	{
		vertex number: (iNumSeg + 1)
		index number: iNumSeg * 3
	}

	vDir: normalized direction from top position to center of bottom
	fHeight: distance between taper top and bottom plane
	fRadius: radius of bottom circle
	iNumSeg: segment of bottom circle, >= 3
	bHasBottom: true, has bottom cap; false, don't has bottom cap
*/
bool a3d_CreateIndexedTaper(const AXVector3& vPos, const AXVector3& vDir,
					float fRadius, float fHeight, int iNumSeg, bool bHasBottom, 
					AXVector3* aVerts, int iMaxVertNum, unsigned short* aIndices, int iMaxIdxNum);

//	Create a indexed box that has 8 vertices and 36 indices
//	vExtent: half border length
bool a3d_CreateIndexedBox(const AXVector3& vPos, const AXVector3& vX,
					const AXVector3& vY, const AXVector3& vZ, const AXVector3& vExtent,
					AXVector3* aVerts, unsigned short* aIndices);

///////////////////////////////////////////////////////////////////////////
//	
//	Class AXAABB
//	
///////////////////////////////////////////////////////////////////////////

//	Axis-Aligned Bounding Box
class AXAABB //: public AMemBase
{
public:		//	Constructors and Destructors

	AXAABB() {}

	AXAABB(const AXAABB& aabb) : 
	Center(aabb.Center),
	Extents(aabb.Extents),
	Mins(aabb.Mins),
	Maxs(aabb.Maxs) {}

	AXAABB(const AXVector3& vMins, const AXVector3& vMaxs) :
	Mins(vMins),
	Maxs(vMaxs),
	Center((vMins + vMaxs) * 0.5f)
	{
		Extents = vMaxs - Center;
	}

public:		//	Attributes

	AXVector3	Center;
	AXVector3	Extents;
	AXVector3	Mins;
	AXVector3	Maxs;

public:		//	Operations

	//	Clear aabb
	void Clear()
	{
		Mins.Set(999999.0f, 999999.0f, 999999.0f);
		Maxs.Set(-999999.0f, -999999.0f, -999999.0f);
	}

	//	Add a vertex to aabb
	void AddVertex(const AXVector3& v);

	//	Build AABB from obb
	void Build(const AXOBB& obb);
	//	Merge two aabb
	void Merge(const AXAABB& subAABB);

	//	Compute Mins and Maxs
	void CompleteMinsMaxs()
	{
		Mins = Center - Extents;
		Maxs = Center + Extents;
	}

	//	Compute Center and Extents
	void CompleteCenterExts()
	{
		Center  = (Mins + Maxs) * 0.5f;
		Extents = Maxs - Center;
	}

	//	Check whether a point is in this aabb
	bool IsPointIn(const AXVector3& v) const
	{
		if (v.x > Maxs.x || v.x < Mins.x ||
			v.y > Maxs.y || v.y < Mins.y ||
			v.z > Maxs.z || v.z < Mins.z)
			return false;

		return true;
	}

	//	Check whether another aabb is in this aabb
	bool IsAABBIn(const AXAABB& aabb) const;

	bool IsIntersect(const AXAABB& aabb) const;

	//	Build AABB from vertices
	void Build(const AXVector3* aVertPos, int iNumVert);
	//	Get vertices of aabb
	void GetVertices(AXVector3* aVertPos, unsigned short* aIndices, bool bWire) const;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class AXOBB
//	
///////////////////////////////////////////////////////////////////////////

//	Oriented Bounding Box
class AXOBB //: public AMemBase
{
public:		//	Constructors and Destructors

	AXOBB() {}
	AXOBB(const AXOBB& obb) : 
	Center(obb.Center),
	XAxis(obb.XAxis),
	YAxis(obb.YAxis),
	ZAxis(obb.ZAxis),
	ExtX(obb.ExtX),
	ExtY(obb.ExtY),
	ExtZ(obb.ExtZ),
	Extents(obb.Extents) {}

public:		//	Attributes

	AXVector3	Center;
	AXVector3	XAxis;
	AXVector3	YAxis;
	AXVector3	ZAxis;
	AXVector3	ExtX;
	AXVector3	ExtY;
	AXVector3	ExtZ;
	AXVector3	Extents;

public:		//	Operations

	//	Check whether a point is in this aabb
	bool IsPointIn(const AXVector3& v) const;

	//	Compute ExtX, ExtY, ExtZ
	void CompleteExtAxis()
	{
		ExtX = XAxis * Extents.x;
		ExtY = YAxis * Extents.y;
		ExtZ = ZAxis * Extents.z;
	}

	//	Clear obb
	void Clear();

	//	Build obb from two obbs
	void Build(const AXOBB& obb1, const AXOBB obb2);
	//	Build obb from vertices
	void Build(const AXVector3* aVertPos, int nVertCount);
	//	Build obb from aabb
	void Build(const AXAABB& aabb);
	//	Get vertices of obb
	void GetVertices(AXVector3* aVertPos, unsigned short* aIndices, bool bWire) const;
};
/*
struct AXOBBMergeHelper
{
	static AXOBB Merge(AXOBBMergeHelper const* apOBBHelper, aint32 nHelper, AXOBB const* apOBB, aint32 nOBB);
	static AXOBBMergeHelper Create(const AXVector3* aVertPos, int nVertCount);

	AXOBBMergeHelper() : firstOrder(0, 0, 0), secondOrder0(0, 0, 0), secondOrder1(0, 0, 0), nVertexCount(0) {}

	struct DV3
	{
		afloat64 x, y, z;
		DV3() = default;
		DV3(AXVector3 const& v) : x(v.x), y(v.y), z(v.z) {}
		DV3(afloat64 x, afloat64 y, afloat64 z) : x(x), y(y), z(z) {}
		void operator+=(DV3 const& r) { x += r.x; y += r.y; z += r.z; }
		friend DV3 operator+(DV3 const& l, DV3 const& r) { return DV3(l.x + r.x, l.y + r.y, l.z + r.z); }
		void operator*=(DV3 const& r) { x *= r.x; y *= r.y; z *= r.z; }
		friend DV3 operator*(DV3 const& l, DV3 const& r) { return DV3(l.x * r.x, l.y * r.y, l.z * r.z); }
		friend DV3 operator*(afloat64 l, DV3 const& r) { return DV3(l * r.x, l * r.y, l * r.z); }
		friend DV3 operator*(DV3 const& l, afloat64 r) { return DV3(l.x * r, l.y * r, l.z * r); }
		AXVector3 ToV3() const { return AXVector3(afloat32(x), afloat32(y), afloat32(z)); }
	};

	DV3 firstOrder; // px, py, pz
	DV3 secondOrder0; // px*px, py*py, pz*pz
	DV3 secondOrder1; // px*py, py*pz, pz*px
	aint32 nVertexCount;
};
*/
///////////////////////////////////////////////////////////////////////////
//	
//	class AXPLANE and AXSPLANE
//	
///////////////////////////////////////////////////////////////////////////

//	Plane
class AXPLANE //: public AMemBase
{
public:		//	Constructors and Destructors

	AXPLANE() {}
	AXPLANE(const AXPLANE& p) : vNormal(p.vNormal) { fDist = p.fDist;	}
	AXPLANE(const AXVector3& n, float d) { vNormal = n; fDist = d; }

public:		//	Attributes

	AXVector3	vNormal;		//	Normal
	float		fDist;			//	d parameter

public:		//	Operations

	//	Create plane from 3 points
	bool CreatePlane(const AXVector3& v1, const AXVector3& v2, const AXVector3& v3);
};

//	Plane with sign
class AXSPLANE : public AXPLANE
{
public:		//	Types

	//	Plane type
	enum
	{
		TYPE_BAD		= -1,	//	Bad plane
		TYPE_PX			= 0,	//	Positive x axis
		TYPE_PY			= 1,	//	Positive y axis
		TYPE_PZ			= 2,	//	Positive z axis
		TYPE_NX			= 3,	//	Negative x axis
		TYPE_NY			= 4,	//	Negative y axis
		TYPE_NZ			= 5,	//	Negative z axis
		TYPE_MAJORX		= 6,	//	Major axis is x
		TYPE_MAJORY		= 7,	//	Major axis is y
		TYPE_MAJORZ		= 8,	//	Major axis is z
		TYPE_UNKNOWN	= 9,	//	unknown
	};

	//	Plane sign flag
	enum
	{
		SIGN_P	= 0,	//	Positive
		SIGN_NX	= 1,	//	x axis is negative
		SIGN_NY = 2,	//	y axis is negative
		SIGN_NZ = 4,	//	z axis is negative
	};

public:		//	Constructors and Destructors

	AXSPLANE() {}
	AXSPLANE(const AXSPLANE& p) { vNormal = p.vNormal; fDist = p.fDist; byType = p.byType; bySignBits = p.bySignBits;	}
	AXSPLANE(const AXVector3& n, float d) : AXPLANE(n, d) { byType = TYPE_UNKNOWN; bySignBits = SIGN_P; }

public:		//	Attributes

	unsigned char	byType;			//	Type of plane
	unsigned char	bySignBits;		//	Sign flags

public:		//	Operations

	//	Make plane type
	void MakeType();
	//	Make plane sign-bits
	void MakeSignBits();
	//	Make plane type and sign-bites
	void MakeTypeAndSignBits()
	{
		MakeType();
		MakeSignBits();
	}
};

///////////////////////////////////////////////////////////////////////////
//	
//	class AXCAPSULE
//	
///////////////////////////////////////////////////////////////////////////

//	Capsule
class AXCAPSULE //: public AMemBase
{
public:		//	Constructors and Destructors

	AXCAPSULE() {}
	AXCAPSULE(const AXCAPSULE& src) :
	vCenter(src.vCenter),
	vAxisX(src.vAxisX),
	vAxisY(src.vAxisY),
	vAxisZ(src.vAxisZ)
	{
		fHalfLen	= src.fHalfLen;
		fRadius		= src.fRadius;
	}

public:		//	Attributes

	AXVector3	vCenter;
	AXVector3	vAxisX;
	AXVector3	vAxisY;
	AXVector3	vAxisZ;
	float		fHalfLen;		//	Half length (on Y axis)
	float		fRadius;		//	Radius

public:		//	Operations

	//	Check whether a point is in this capsule
	bool IsPointIn(const AXVector3& vPos);
};

class AXCONE
{
public:
	AXVector3	vPos;
	AXVector3	vAxisX;
	AXVector3	vAxisY;
	AXVector3	vAxisZ;
	float		fHeight;		//	height (on Y axis)
	float		fRadius;		//	Radius
};

///////////////////////////////////////////////////////////////////////////
//	
//	class AXCYLINDER
//	
///////////////////////////////////////////////////////////////////////////

//	Cylinder
class AXCYLINDER //: public AMemBase
{
public:		//	Constructors and Destructors

	AXCYLINDER() {}
	AXCYLINDER(const AXCYLINDER& src) :
	vCenter(src.vCenter),
	vAxisX(src.vAxisX),
	vAxisY(src.vAxisY),
	vAxisZ(src.vAxisZ)
	{
		fHalfLen	= src.fHalfLen;
		fRadius		= src.fRadius;
	}

public:		//	Attributes

	AXVector3	vCenter;
	AXVector3	vAxisX;
	AXVector3	vAxisY;
	AXVector3	vAxisZ;
	float		fHalfLen;		//	Half length (on Y axis)
	float		fRadius;		//	Radius

public:		//	Operations

	//	Check whether a point is in this cylinder
	bool IsPointIn(const AXVector3& vPos);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


class AXSPHERE
{
public:
	AXVector3 vPos;
	afloat32 fRadius;
};

#endif	//	_AXGEOMETRY_H_
