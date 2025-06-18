//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#ifndef _AXCOLLISION_H_
#define _AXCOLLISION_H_

#include "AXTypes.h"
#include "AXGeometry.h"

///////////////////////////////////////////////////////////////////////////

class AXCLS
{
public:
	static bool RayToAABB3(const AXVector3& vStart, const AXVector3& vDelta, const AXVector3& vMins, const AXVector3& vMaxs, AXVector3& vPoint, float* pfFraction, AXVector3& vNormal);
	static bool RayToAABB2(const AXVector3& vStart, const AXVector3& vDelta, float vMins[2], float vMaxs[2], AXVector3& vPoint);
	static bool RayToOBB3(const AXVector3& vStart, const AXVector3& vDelta, const AXOBB& OBB, AXVector3& vPoint, float* pfFraction, AXVector3& vNormal);
	static bool RayToTriangle(const AXVector3& vStart, const AXVector3& vDelta, const AXVector3& v0,
						   const AXVector3& v1, const AXVector3& v2, AXVector3& vPoint, bool b2Sides, float* pfFraction=NULL);
	static bool RayToPlane(const AXVector3& vStart, const AXVector3& vDelta, const AXVector3& vPlaneNormal, float fDist, bool b2Sides, AXVector3& vHitPos, float* pfFraction);
	static bool OBBToQuadrangle(const AXOBB& OBB, const AXVector3& ov0, const AXVector3& ov1, const AXVector3& ov2,
							 const AXVector3& ov3, const AXVector3& vNormal, float* pfFraction);
	static int AABBToPlane(const AXVector3& vStart, const AXVector3& vEnd, const AXVector3& vExts, const AXSPLANE& Plane, float* pfFraction);

	static int PlaneAABBOverlap(const AXVector3& vNormal, float fDist, const AXVector3& vOrigin, const AXVector3& vExtents);
	static int PlaneAABBOverlap(const AXSPLANE& Plane, const AXVector3& _vMins, const AXVector3& _vMaxs);
	static int PlaneSphereOverlap(const AXSPLANE& Plane, const AXVector3& vCenter, float fRadius);
	static bool TriangleAABBOverlap(const AXVector3& _v0, const AXVector3& _v1, const AXVector3& _v2, const AXSPLANE& Plane, const AXAABB& AABB);
	static bool AABBAABBOverlap(const AXVector3& vCenter1, const AXVector3& vExt1, const AXVector3& vCenter2, const AXVector3& vExt2);
	static bool RaySphereOverlap(const AXVector3& vStart, const AXVector3& vDelta, const AXVector3& vOrigin, float fRadius);
	static bool OBBOBBOverlap(const AXOBB& obb1, const AXOBB& obb2);
	static bool OBBOBBOverlap(const AXVector3& vExt1, const AXVector3& vExt2, const AXMatrix4& mat);
	static bool AABBOBBOverlap(const AXVector3& vCenter, const AXVector3& vExts, const AXOBB& obb);
	static bool AABBSphereOverlap(const AXAABB& aabb, const AXVector3& vCenter, float fRadius);
	static bool OBBSphereOverlap(const AXOBB& obb, const AXVector3& vCenter, float fRadius);
	static abool RayToSphere(const AXVector3& vStart, const AXVector3& vDelta, const AXVector3& vSphereCenter, afloat32 fRadius, AXVector3& vHitPos, afloat32* pfFraction = NULL);

	///////////////////////////////////////////////////////////////////////////


	/*	Check whether a plane collision with a sphere.

		Return value:
		
			-1: sphere is behide of plane
			0: sphere cross with plane
			1: sphere is in front of plane

		vNormal: plane's normal.
		fDist: d parameter of plane equation
		vCenter: sphere's center
		fRadius: sphere's radius
	*/
	static inline int PlaneSphereOverlap(AXVector3& vNormal, float fDist, AXVector3& vCenter, float fRadius)
	{
		float fDelta = DotProduct(vCenter, vNormal) - fDist;

		if (fDelta > fRadius)
			return 1;
		else if (fDelta < -fRadius)
			return -1;
		else
			return 0;
	}

	/*	AABB-OBB overlap test using the separating axis theorem.

		Return true if boxes overlap.

		aabb: aabb's information
		obb: obb's inforamtion
	*/
	static inline bool AABBOBBOverlap(const AXAABB& aabb, const AXOBB& obb)
	{
		return AXCLS::AABBOBBOverlap(aabb.Center, aabb.Extents, obb);
	}

};

#endif	//	_AXCOLLISION_H_

