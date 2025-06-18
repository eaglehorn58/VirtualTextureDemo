//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#ifndef _AX_FRUSTUM_H_
#define _AX_FRUSTUM_H_

#include "AXTypes.h"
#include "AXGeometry.h"
#include "vector.h"

///////////////////////////////////////////////////////////////////////////

class AXCameraBase;
class AXViewport;

///////////////////////////////////////////////////////////////////////////

class AXFrustum //: public AMemBase
{
public:		//	Types

	//	Index of view frustum plane
	enum
	{
		VF_LEFT = 0,
		VF_RIGHT,
		VF_TOP,	
		VF_BOTTOM,
		VF_NEAR,
		VF_FAR,
	};

	//	Clip plane
	struct PLANE
	{
		AXSPLANE	Plane;		//	Plane data
		bool		bEnable;	//	Enable flag
	};

public:		//	Constructions and Destructions

	AXFrustum();
	AXFrustum(const AXFrustum& frustum);
	virtual ~AXFrustum() {}

public:		//	Attributes

public:		//	Operaitons

	//	Initialize object
	bool Init(int iNumPlane);
	bool Init(AXCameraBase* pCamera, float fLeft, float fTop, float fRight, float fBottom, bool bZClip);
	bool Init(AXViewport* pView, int iLeft, int iTop, int iRight, int iBottom, bool bZClip);
	//	Release object
	void Release();

	//	Add a clip plane
	bool AddPlane(const AXSPLANE& Plane);
	bool AddPlane(const AXVector3& vNormal, float fDist);

	//	Check if AABB is in or out frustum
	int AABBInFrustum(const AXVector3& vMins, const AXVector3& vMaxs);
	int AABBInFrustum(const AXAABB& aabb) { return AABBInFrustum(aabb.Mins, aabb.Maxs); }
	//	Check if sphere is in or out frustum
	int SphereInFrustum(const AXVector3& vCenter, float fRadius);
	//	Check if point is in or out frustum
	bool PointInFrustum(const AXVector3& vPos) { return SphereInFrustum(vPos, 0.0f) >= 0 ? true : false; }

	//	Get plane's pointer. The return address is temporary, so use it immedialtely
	AXSPLANE* GetPlane(int iIndex) { return &m_aPlanes[iIndex].Plane; }
	//	Get number of clip plane
	int GetPlaneNum() { return (int)m_aPlanes.size(); }
	//	Enable clip plane
	void EnablePlane(int iIndex, bool bEnable) { m_aPlanes[iIndex].bEnable = bEnable; }
	//	Get enable flag
	bool PlaneIsEnable(int iIndex) { return m_aPlanes[iIndex].bEnable; }

	const AXFrustum& operator = (const AXFrustum& frustum);

protected:	//	Attributes

	abase::vector<PLANE>	m_aPlanes;

protected:	//	Operations
	
	//	Create clip plane
	void CreateClipPlane(const AXVector3& v0, const AXVector3& v1, const AXVector3& v2, AXSPLANE* pPlane);
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_AX_FRUSTUM_H_
