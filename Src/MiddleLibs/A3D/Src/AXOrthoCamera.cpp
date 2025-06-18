//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#include "AXOrthoCamera.h"
#include "AXFuncs.h"

///////////////////////////////////////////////////////////////////////////

AXOrthoCamera::AXOrthoCamera()
{ 
/*	m_dwClassID = A3D_CID_ORTHOCAMERA;*/
}

//	Initialize camera object
bool AXOrthoCamera::Init(/*A3DDevice* pA3DDevice,*/ float l, float r, float b, float t, float zn, float zf)
{
	if (!AXCameraBase::Init(/*pA3DDevice, */zn, zf))
		return false;

	m_fLeft		= l;
	m_fRight	= r;
	m_fTop		= t;
	m_fBottom	= b;

// 	D3DXMATRIX matProj;
// 	D3DXMatrixOrthoOffCenterLH(&matProj, l, r, b, t, zn, zf);
// 	m_matProjectTM = *(AXMatrix4 *)&matProj;
	m_matProjectTM = a3d_OrthoOffCenterMatrix(l, r, b, t, zn, zf);
	
	a3d_InverseMatrix(m_matProjectTM, &m_matInvProjectTM);

	m_matPostProjectTM.Identity();

	//	Create view frustum
	m_ViewFrustum.Init(6);
	m_WorldFrustum.Init(6);
	CreateViewFrustum();

	return true;
}

//	Release object
void AXOrthoCamera::Release()
{
	AXCameraBase::Release();
}

//	Update project TM
bool AXOrthoCamera::UpdateProjectTM()
{
//	D3DXMATRIX matProj;
// 	D3DXMatrixOrthoOffCenterLH(&matProj, m_fLeft, m_fRight, m_fBottom, m_fTop, m_vFront, m_vBack);
// 	m_matProjectTM = *(AXMatrix4 *)&matProj;

	m_matProjectTM = a3d_OrthoOffCenterMatrix(m_fLeft, m_fRight, m_fBottom, m_fTop, m_vFront, m_vBack);
	
	a3d_InverseMatrix(m_matProjectTM, &m_matInvProjectTM);

	m_matVPTM = m_matViewTM * m_matProjectTM * m_matPostProjectTM;
	a3d_InverseMatrix(m_matVPTM, &m_matInvVPTM);

	CreateViewFrustum();

	return true;
}

//	Create view frustum
bool AXOrthoCamera::CreateViewFrustum()
{
	AXPLANE *p;

	//	Top clip plane
	p = m_ViewFrustum.GetPlane(AXFrustum::VF_TOP);
	p->vNormal.Set(0.0f, -1.0f, 0.0f);
	p->fDist = -m_fTop;

	//	Bottom clip plane
	p = m_ViewFrustum.GetPlane(AXFrustum::VF_BOTTOM);
	p->vNormal.Set(0.0f, 1.0f, 0.0f);
	p->fDist = m_fBottom;

	//	Left clip plane
	p = m_ViewFrustum.GetPlane(AXFrustum::VF_LEFT);
	p->vNormal.Set(1.0f, 0.0f, 0.0f);
	p->fDist = m_fLeft;

	//	Right clip plane
	p = m_ViewFrustum.GetPlane(AXFrustum::VF_RIGHT);
	p->vNormal.Set(-1.0f, 0.0f, 0.0f);
	p->fDist = -m_fRight;

	//	Near clip plane
	p = m_ViewFrustum.GetPlane(AXFrustum::VF_NEAR);
	p->vNormal.Set(0.0f, 0.0f, 1.0f);
	p->fDist = m_vFront;

	//	Far clip plane
	p = m_ViewFrustum.GetPlane(AXFrustum::VF_FAR);
	p->vNormal.Set(0.0f, 0.0f, -1.0f);
	p->fDist = -m_vBack;

	//	Update frustum in world coordinates
	UpdateWorldFrustum();

	return true;
}

//	Update world frustum
bool AXOrthoCamera::UpdateWorldFrustum()
{
	AXSPLANE *p;

	//	Top clip plane
	p = m_WorldFrustum.GetPlane(AXFrustum::VF_TOP);
	p->vNormal = -m_vecUp;
	p->fDist = DotProduct(p->vNormal, m_vecPos + m_vecUp * m_fTop);
	p->byType = AXSPLANE::TYPE_UNKNOWN;
	p->MakeSignBits();

	//	Bottom clip plane
	p = m_WorldFrustum.GetPlane(AXFrustum::VF_BOTTOM);
	p->vNormal = m_vecUp;
	p->fDist = DotProduct(p->vNormal, m_vecPos + m_vecUp * m_fBottom);
	p->byType = AXSPLANE::TYPE_UNKNOWN;
	p->MakeSignBits();

	//	Left clip plane
	p = m_WorldFrustum.GetPlane(AXFrustum::VF_LEFT);
	p->vNormal = m_vecRight;
	p->fDist = DotProduct(p->vNormal, m_vecPos + m_vecRight * m_fLeft);
	p->byType = AXSPLANE::TYPE_UNKNOWN;
	p->MakeSignBits();

	//	Right clip plane
	p = m_WorldFrustum.GetPlane(AXFrustum::VF_RIGHT);
	p->vNormal = m_vecLeft;
	p->fDist = DotProduct(p->vNormal, m_vecPos + m_vecRight * m_fRight);
	p->byType = AXSPLANE::TYPE_UNKNOWN;
	p->MakeSignBits();

	//	Near clip plane
	p = m_WorldFrustum.GetPlane(AXFrustum::VF_NEAR);
	p->vNormal = m_vecDir;
	p->fDist = DotProduct(p->vNormal, m_vecPos + m_vecDir * m_vFront);
	p->byType = AXSPLANE::TYPE_UNKNOWN;
	p->MakeSignBits();

	//	Far clip plane
	p = m_WorldFrustum.GetPlane(AXFrustum::VF_FAR);
	p->vNormal = -m_vecDir;
	p->fDist = DotProduct(p->vNormal, m_vecPos + m_vecDir * m_vBack);
	p->byType = AXSPLANE::TYPE_UNKNOWN;
	p->MakeSignBits();

	return true;
}

//	Set / Get projection parameters
bool AXOrthoCamera::SetProjectionParam(float l, float r, float b, float t, float zn, float zf)
{
	m_fLeft		= l;
	m_fRight	= r;
	m_fTop		= t;
	m_fBottom	= b;
	m_vFront	= zn;
	m_vBack		= zf;

	UpdateProjectTM();
	return true;

	// could not call base camera's setfrontandback, because if zn < 0.0f, it will not update it!!!
	//return SetZFrontAndBack(zn, zf, true);
}

/*	Calculate view frustum's 8 corners' position

	Return number of corners;

	aCorners (out): array used to receive corners position, you can pass NULL to get how
			many corners by return value. 
			corner index is defined as below:
			0 - 3: near plane corners, left-bottom, left-top, right-top, right-bottom
			4 - 7: far plane corners, left-bottom, left-top, right-top, right-bottom
	bInViewSpace: true, positions are in view space; false, in world space
	fNear, fFar: near and far distance.
*/
int AXOrthoCamera::CalcFrustumCorners(AXVector3* aCorners, bool bInViewSpace, float fNear, float fFar)
{
	if (!aCorners)
		return 8;

	AXVector3 vPos, vZ, vX, vY;

	if (bInViewSpace)
	{
		vPos.Clear();
		vZ = AXVector3(0,0,1);//AXVector3::vAxisZ;
		vX = AXVector3(1,0,0);//AXVector3::vAxisX;
		vY = AXVector3(0,1,0);//AXVector3::vAxisY;
	}
	else
	{
		vPos = GetPos();
		vZ = GetDir();
		vX = AXVector3(GetRight(), 0.0f, 0.0f);
		vY = GetUp();
	}

	float fNearPlaneHeight = (GetRight() - GetLeft()) * 0.5f;
	float fNearPlaneWidth = (GetTop() - GetBottom()) * 0.5f;
	float fFarPlaneHeight = fNearPlaneHeight;
	float fFarPlaneWidth = fNearPlaneWidth;

	AXVector3 vNearPlaneCenter = vPos + vZ * fNear;
	AXVector3 vFarPlaneCenter = vPos + vZ * fFar;

	aCorners[0] = vNearPlaneCenter - vX * fNearPlaneWidth - vY * fNearPlaneHeight;
	aCorners[1] = vNearPlaneCenter - vX * fNearPlaneWidth + vY * fNearPlaneHeight;
	aCorners[2] = vNearPlaneCenter + vX * fNearPlaneWidth + vY * fNearPlaneHeight;
	aCorners[3] = vNearPlaneCenter + vX * fNearPlaneWidth - vY * fNearPlaneHeight;

	aCorners[4] = vFarPlaneCenter - vX * fFarPlaneWidth - vY * fFarPlaneHeight;
	aCorners[5] = vFarPlaneCenter - vX * fFarPlaneWidth + vY * fFarPlaneHeight;
	aCorners[6] = vFarPlaneCenter + vX * fFarPlaneWidth + vY * fFarPlaneHeight;
	aCorners[7] = vFarPlaneCenter + vX * fFarPlaneWidth - vY * fFarPlaneHeight;	

	return 8;
}



