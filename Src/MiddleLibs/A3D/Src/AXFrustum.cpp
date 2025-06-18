//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#include "AXFrustum.h"
#include "AXCameraBase.h"
#include "AXViewport.h"
#include "AXCollision.h"

///////////////////////////////////////////////////////////////////////////

AXFrustum::AXFrustum()
{
}

AXFrustum::AXFrustum(const AXFrustum& frustum)
{
	int srcLen = (int)frustum.m_aPlanes.size();
	m_aPlanes.reserve(srcLen);

	for (int i = 0; i < srcLen; ++i)
	{
		m_aPlanes[i] = frustum.m_aPlanes[i];
	}
}

//	Initialize object
bool AXFrustum::Init(int iNumPlane)
{
	PLANE plane;
	memset(&plane, 0, sizeof(plane));
	plane.bEnable = true;

	for (int i = 0; i < iNumPlane; i++)
	{
		m_aPlanes.push_back(plane);
	}

	return true;
}

/*	Create a view frustum.

	Return true for success, otherwise return false.

	pCamera: camera object.
	fLeft, fTop, fRight, fBottom: project rectangle of frustum in identity view space.
			-1.0f, 1.0f, 1.0f, -1.0f, means whole view space.
	bZClip: create near and far clip planes on z axis
*/
bool AXFrustum::Init(AXCameraBase* pCamera, float fLeft, float fTop, float fRight, float fBottom, bool bZClip)
{
	if (bZClip)
	{
		m_aPlanes.reserve(6);

		for (int i=0; i < 6; i++)
			m_aPlanes[i].bEnable = true;
	}
	else
	{
		m_aPlanes.reserve(4);

		for (int i=0; i < 4; i++)
			m_aPlanes[i].bEnable = true;
	}

	AXVector3 vLT(fLeft, fTop, 0.5f);
	AXVector3 vLB(fLeft, fBottom, 0.5f);
	AXVector3 vRT(fRight, fTop, 0.5f);
	AXVector3 vRB(fRight, fBottom, 0.5f);

	//	Convert to world space
	pCamera->InvTransform(vLT, vLT);
	pCamera->InvTransform(vLB, vLB);
	pCamera->InvTransform(vRT, vRT);
	pCamera->InvTransform(vRB, vRB);

	AXVector3 vEye = pCamera->GetPos();

	CreateClipPlane(vEye, vLT, vLB, GetPlane(VF_LEFT));
	CreateClipPlane(vEye, vRB, vRT, GetPlane(VF_RIGHT));
	CreateClipPlane(vEye, vRT, vLT, GetPlane(VF_TOP));
	CreateClipPlane(vEye, vLB, vRB, GetPlane(VF_BOTTOM));

	if (bZClip)
	{
		AXVector3 vDir = pCamera->GetDir();

		//	Create near plane
		AXVector3 vPos = vEye + vDir * pCamera->GetZFront();
		AXSPLANE* pPlane = GetPlane(VF_NEAR);

		pPlane->vNormal	= vDir;
		pPlane->fDist	= DotProduct(pPlane->vNormal, vPos);
		pPlane->byType	= AXSPLANE::TYPE_UNKNOWN;
		pPlane->MakeSignBits();
		
		//	Create far plane
		vPos = vEye + vDir * pCamera->GetZBack();
		pPlane = GetPlane(VF_FAR);
		
		pPlane->vNormal	= -vDir;
		pPlane->fDist	= DotProduct(pPlane->vNormal, vPos);
		pPlane->byType	= AXSPLANE::TYPE_UNKNOWN;
		pPlane->MakeSignBits();
	}

	return true;
}

/*	Create a view frustum.

	Return true for success, otherwise return false.

	pView: viewport object.
	iLeft, iTop, iRight, iBottom: project rectangle of frustum on screen.
			0, 0, screen width, screen height, means whole screen.
	bZClip: create near and far clip planes on z axis
*/
bool AXFrustum::Init(AXViewport* pView, int iLeft, int iTop, int iRight, int iBottom, bool bZClip)
{
	if (bZClip)
	{
		m_aPlanes.reserve(6);

		for (int i=0; i < 6; i++)
			m_aPlanes[i].bEnable = true;
	}
	else
	{
		m_aPlanes.reserve(4);

		for (int i=0; i < 4; i++)
			m_aPlanes[i].bEnable = true;
	}

	AXVector3 vLT((float)iLeft, (float)iTop, 0.5f);
	AXVector3 vLB((float)iLeft, (float)iBottom, 0.5f);
	AXVector3 vRT((float)iRight, (float)iTop, 0.5f);
	AXVector3 vRB((float)iRight, (float)iBottom, 0.5f);

	//	Convert to world space
	pView->InvTransform(vLT, vLT);
	pView->InvTransform(vLB, vLB);
	pView->InvTransform(vRT, vRT);
	pView->InvTransform(vRB, vRB);

	AXCameraBase* pCamera = pView->GetCamera();
	AXVector3 vEye = pCamera->GetPos();

	CreateClipPlane(vEye, vLT, vLB, GetPlane(VF_LEFT));
	CreateClipPlane(vEye, vRB, vRT, GetPlane(VF_RIGHT));
	CreateClipPlane(vEye, vRT, vLT, GetPlane(VF_TOP));
	CreateClipPlane(vEye, vLB, vRB, GetPlane(VF_BOTTOM));

	if (bZClip)
	{
		AXVector3 vDir = pCamera->GetDir();

		//	Create near plane
		AXVector3 vPos = vEye + vDir * pCamera->GetZFront();
		AXSPLANE* pPlane = GetPlane(VF_NEAR);

		pPlane->vNormal	= vDir;
		pPlane->fDist	= DotProduct(pPlane->vNormal, vPos);
		pPlane->byType	= AXSPLANE::TYPE_UNKNOWN;
		pPlane->MakeSignBits();
		
		//	Create far plane
		vPos = vEye + vDir * pCamera->GetZBack();
		pPlane = GetPlane(VF_FAR);
		
		pPlane->vNormal	= -vDir;
		pPlane->fDist	= DotProduct(pPlane->vNormal, vPos);
		pPlane->byType	= AXSPLANE::TYPE_UNKNOWN;
		pPlane->MakeSignBits();
	}

	return true;
}

//	Release object
void AXFrustum::Release()
{
	m_aPlanes.clear();
}

//	Create clip plane
void AXFrustum::CreateClipPlane(const AXVector3& v0, const AXVector3& v1, const AXVector3& v2, AXSPLANE* pPlane)
{
	pPlane->CreatePlane(v0, v1, v2);

	pPlane->byType = AXSPLANE::TYPE_UNKNOWN;
	pPlane->MakeSignBits();
}

//	Add a clip plane
bool AXFrustum::AddPlane(const AXSPLANE& Plane)
{
	PLANE p;

	p.Plane		= Plane;
	p.bEnable	= true;

	m_aPlanes.push_back(p);
	return true;
}

//	Add a clip plane
bool AXFrustum::AddPlane(const AXVector3& vNormal, float fDist)
{
	PLANE p;
	
	p.Plane.vNormal	= vNormal;
	p.Plane.fDist	= fDist;
	p.Plane.byType	= AXSPLANE::TYPE_UNKNOWN;
	p.Plane.MakeSignBits();

	p.bEnable = true;

	m_aPlanes.push_back(p);
	return true;
}

/*	Check if AABB is in or out frustum. This functions assume that all frustum
	planes point to frustem inner.

	Return value:

		1: AABB is complete in frustum
		0: AABB cross with frustum
		-1: AABB is complete out of frustum
*/
int AXFrustum::AABBInFrustum(const AXVector3& vMins, const AXVector3& vMaxs)
{
	bool bCross = false;

	for (int i=0; i < (int)m_aPlanes.size(); i++)
	{
		if (!m_aPlanes[i].bEnable)
			continue;

		int iRet = AXCLS::PlaneAABBOverlap(m_aPlanes[i].Plane, vMins, vMaxs);
		if (iRet < 0)
			return -1;
		else if (!iRet)
			bCross = true;
	}
	
	return bCross ? 0 : 1;
}

/*	Check if sphere is in or out frustum. This function assumes that all frustum
	planes point to frustem inner.

	Return value:

		1: sphere is complete in frustum
		0: sphere cross with frustum
		-1: sphere is complete out of frustum
*/
int AXFrustum::SphereInFrustum(const AXVector3& vCenter, float fRadius)
{
	bool bCross = false;

	for (int i=0; i < (int)m_aPlanes.size(); i++)
	{
		if (!m_aPlanes[i].bEnable)
			continue;

		int iRet = AXCLS::PlaneSphereOverlap(m_aPlanes[i].Plane, vCenter, fRadius);
		if (iRet < 0)
			return -1;
		else if (!iRet)
			bCross = true;
	}

	return bCross ? 0 : 1;
}

const AXFrustum& AXFrustum::operator = (const AXFrustum& frustum)
{
	ASSERT(m_aPlanes.size() == frustum.m_aPlanes.size());

	for (int i = 0; i < m_aPlanes.size(); i++)
	{
		m_aPlanes[i] = frustum.m_aPlanes[i];
	}

	return *this;
}

