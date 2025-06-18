//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#include "AXCamera.h"
#include "AXFuncs.h"

AXCamera::AXCamera()
{ 
	//m_dwClassID = A3D_CID_CAMERA;
}

//	fov is in radian!
bool AXCamera::Init(/*A3DDevice* pA3DDevice, */float vFov, float vFront, float vBack, float vRatio)
{
	if (!AXCameraBase::Init(/*pA3DDevice, */vFront, vBack))
		return false;

	m_vFOV		= vFov;
	m_vFOVSet	= vFov;
	m_vRatio	= vRatio;

//  	D3DXMATRIX matProj;
//  	D3DXMatrixPerspectiveFovLH(&matProj, vFov, vRatio, vFront, vBack);
//  	m_matProjectTM = *(AXMatrix4*) &matProj;
	m_matProjectTM = a3d_PerspectiveFovMatrix(vFov, vRatio, vFront, vBack);

	a3d_InverseMatrix(m_matProjectTM, &m_matInvProjectTM);

	m_matPostProjectTM.Identity();
	
	//	Create frustums
	m_ViewFrustum.Init(6);
	m_WorldFrustum.Init(6);
	CreateViewFrustum();

	return true;
}

void AXCamera::Release()
{
	AXCameraBase::Release();
}

bool AXCamera::UpdateProjectTM()
{
// 		D3DXMATRIX matProj;
//  	D3DXMatrixPerspectiveFovLH(&matProj, m_vFOV, m_vRatio, m_vFront, m_vBack);
//  	m_matProjectTM = *(AXMatrix4*) &matProj;
	m_matProjectTM = a3d_PerspectiveFovMatrix(m_vFOV, m_vRatio, m_vFront, m_vBack);

	a3d_InverseMatrix(m_matProjectTM, &m_matInvProjectTM);

	m_matVPTM = m_matViewTM * m_matProjectTM * m_matPostProjectTM;
	a3d_InverseMatrix(m_matVPTM, &m_matInvVPTM);

	CreateViewFrustum();
	return true;
}

//	Apply camera to device
bool AXCamera::Active()
{
	return false;
}

bool AXCamera::Update()
{
	if (m_vFOVSet != m_vFOV)
	{
		if (m_vFOV > m_vFOVSet)
		{
			m_vFOV -= 0.01f;	//	Zoom in;
			if (m_vFOV < m_vFOVSet)
				m_vFOV = m_vFOVSet;
		}
		else
		{
			m_vFOV += 0.01f;	//	Zoom out;
			if (m_vFOV > m_vFOVSet)
				m_vFOV = m_vFOVSet;
		}

		UpdateProjectTM();
	}

	return true;
}

//	Create view frustum in view coordinates
bool AXCamera::CreateViewFrustum()
{
	AXPLANE *p1, *p2;

	//	Top and bottom clip planes
	float c, s, fAngle = m_vFOV * 0.5f;

	c = (float)cos(fAngle);
	s = (float)sin(fAngle);

	p1 = m_ViewFrustum.GetPlane(AXFrustum::VF_TOP);
	p2 = m_ViewFrustum.GetPlane(AXFrustum::VF_BOTTOM);

	p1->vNormal.Set(0.0f, -c, s);
	p2->vNormal.Set(0.0f, c, s);

	p1->fDist = p2->fDist = 0.0f;

	//	Left and right clip planes
	fAngle = (float)(tan(fAngle) * m_vRatio);

	c = (float)cos(atan(fAngle));
	s = (float)sin(atan(fAngle));

	p1 = m_ViewFrustum.GetPlane(AXFrustum::VF_LEFT);
	p2 = m_ViewFrustum.GetPlane(AXFrustum::VF_RIGHT);

	p1->vNormal.Set(c, 0.0f, s);
	p2->vNormal.Set(-c, 0.0f, s);

	p1->fDist = p2->fDist = 0.0f;

	//	Near and Far clipping plane
	p1 = m_ViewFrustum.GetPlane(AXFrustum::VF_NEAR);
	p2 = m_ViewFrustum.GetPlane(AXFrustum::VF_FAR);

	p1->vNormal.Set(0.0f, 0.0f, 1.0f);
	p2->vNormal.Set(0.0f, 0.0f, -1.0f);

	p1->fDist = p2->fDist = 0.0f;

	//	Update frustum in world coordinates
	UpdateWorldFrustum();

	return true;
}

//	Update view frustum and translate it into world coordinates
bool AXCamera::UpdateWorldFrustum()
{
	AXSPLANE *pw, *pv;

	//	Translate left plane
	pw = m_WorldFrustum.GetPlane(AXFrustum::VF_LEFT);
	pv = m_ViewFrustum.GetPlane(AXFrustum::VF_LEFT);

	pw->vNormal	= a3d_ViewToWorld(pv->vNormal, m_matViewTM);
	pw->fDist	= DotProduct(pw->vNormal, m_vecPos);
	pw->byType	= AXSPLANE::TYPE_UNKNOWN;
	pw->MakeSignBits();

	//	Translate right plane
	pw = m_WorldFrustum.GetPlane(AXFrustum::VF_RIGHT);
	pv = m_ViewFrustum.GetPlane(AXFrustum::VF_RIGHT);

	pw->vNormal	= a3d_ViewToWorld(pv->vNormal, m_matViewTM);
	pw->fDist	= DotProduct(pw->vNormal, m_vecPos);
	pw->byType	= AXSPLANE::TYPE_UNKNOWN;
	pw->MakeSignBits();

	//	Translate top plane
	pw = m_WorldFrustum.GetPlane(AXFrustum::VF_TOP);
	pv = m_ViewFrustum.GetPlane(AXFrustum::VF_TOP);

	pw->vNormal	= a3d_ViewToWorld(pv->vNormal, m_matViewTM);
	pw->fDist	= DotProduct(pw->vNormal, m_vecPos);
	pw->byType	= AXSPLANE::TYPE_UNKNOWN;
	pw->MakeSignBits();

	//	Translate bottom plane
	pw = m_WorldFrustum.GetPlane(AXFrustum::VF_BOTTOM);
	pv = m_ViewFrustum.GetPlane(AXFrustum::VF_BOTTOM);

	pw->vNormal	= a3d_ViewToWorld(pv->vNormal, m_matViewTM);
	pw->fDist	= DotProduct(pw->vNormal, m_vecPos);
	pw->byType	= AXSPLANE::TYPE_UNKNOWN;
	pw->MakeSignBits();

	//	Translate near plane
	pw = m_WorldFrustum.GetPlane(AXFrustum::VF_NEAR);

	AXVector3 vPos = m_vecPos + m_vecDir * m_vFront;

	pw->vNormal	= m_vecDir;
	pw->fDist	= DotProduct(pw->vNormal, vPos);
	pw->byType	= AXSPLANE::TYPE_UNKNOWN;
	pw->MakeSignBits();

	//	Transform far plane
	pw = m_WorldFrustum.GetPlane(AXFrustum::VF_FAR);

	vPos = m_vecPos + m_vecDir * m_vBack;

	pw->vNormal	= -m_vecDir;
	pw->fDist	= DotProduct(pw->vNormal, vPos);
	pw->byType	= AXSPLANE::TYPE_UNKNOWN;
	pw->MakeSignBits();

	return true;
}

//	Set / Get projection parameters
bool AXCamera::SetProjectionParam(float fFOV, float fFront, float fBack, float fRatio)
{
	if (fFOV > 0.0f)
	{
		m_vFOV = fFOV;
		m_vFOVSet = m_vFOV; // we will lock this when force set projection param
	}

	if (fRatio > 0.0f)
		m_vRatio = fRatio;

	return SetZFrontAndBack(fFront, fBack, true);
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
int AXCamera::CalcFrustumCorners(AXVector3* aCorners, bool bInViewSpace, float fNear, float fFar)
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
		vX = GetRight();
		vY = GetUp();
	}

	float fFOV = GetFOV();
	float fAspect = GetRatio();
	float fNearPlaneHeight = tanf(fFOV * 0.5f) * fNear;
	float fNearPlaneWidth = fNearPlaneHeight * fAspect;
	float fFarPlaneHeight = tanf(fFOV * 0.5f) * fFar;
	float fFarPlaneWidth = fFarPlaneHeight * fAspect;

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

void AXCamera::SetMirror(const AXCameraBase* pOrgCamera, const AXMirrorParam& MirrorParam)
{
	if (pOrgCamera->GetType() != ACT_PERSPECTIVE)
	{
		return;
	}

	//	Set projection
	afloat32 fFOV, fFront, fBack, fRatio;
	((AXCamera*)pOrgCamera)->GetProjectionParam(&fFOV, &fFront, &fBack, &fRatio);
	SetProjectionParam(fFOV, fFront, fBack, fRatio);

	//	Set view
	AXMatrix4 matMirror = a3d_MirrorMatrix(MirrorParam.vMirrorOrg, MirrorParam.vMirrorNormal);
	AXVector3 vMirrorCameraPos = matMirror.TransformCoord(pOrgCamera->GetPos());
	AXVector3 vMirrorCameraDir = pOrgCamera->GetDir() * matMirror;
	AXVector3 vMirrorCameraUp = pOrgCamera->GetUp() * matMirror;
	SetDirAndUp(vMirrorCameraDir, vMirrorCameraUp);
	SetPos(vMirrorCameraPos);

	m_bMirrored = atrue;
}


//////////////////////////////////////////////////////////////////////////
//	AXCameraOffCenter
//////////////////////////////////////////////////////////////////////////

AXCameraOffCenter::AXCameraOffCenter()
	: AXCameraBase()
	, m_fLeft(-1.0f)
	, m_fRight(1.0f)
	, m_fBottom(-1.0f)
	, m_fTop(1.0f)
{}

abool AXCameraOffCenter::Init(afloat32 fLeft, afloat32 fRight, afloat32 fBottom, afloat32 fTop, afloat32 fFront, afloat32 fBack)
{
	if (!AXCameraBase::Init(fFront, fBack))
	{
		return afalse;
	}

	m_fLeft = fLeft;
	m_fRight = fRight;
	m_fBottom = fBottom;
	m_fTop = fTop;

	m_matProjectTM = a3d_PerspectiveOffCenterMatrix(m_fLeft, m_fRight, m_fBottom, m_fTop, m_vFront, m_vBack);

	a3d_InverseMatrix(m_matProjectTM, &m_matInvProjectTM);

	m_matPostProjectTM.Identity();

	//	Create frustums
	m_ViewFrustum.Init(6);
	m_WorldFrustum.Init(6);
	CreateViewFrustum();

	return atrue;
}

void AXCameraOffCenter::Release()
{
	AXCameraBase::Release();
}

abool AXCameraOffCenter::SetProjectionParam(afloat32 fLeft, afloat32 fRight, afloat32 fBottom, afloat32 fTop, afloat32 fFront, afloat32 fBack)
{
	m_fLeft = fLeft;
	m_fRight = fRight;
	m_fBottom = fBottom;
	m_fTop = fTop;

	return SetZFrontAndBack(fFront, fBack, atrue);
}

abool AXCameraOffCenter::UpdateProjectTM()
{
	m_matProjectTM = a3d_PerspectiveOffCenterMatrix(m_fLeft, m_fRight, m_fBottom, m_fTop, m_vFront, m_vBack);

	a3d_InverseMatrix(m_matProjectTM, &m_matInvProjectTM);

	m_matVPTM = m_matViewTM * m_matProjectTM * m_matPostProjectTM;
	a3d_InverseMatrix(m_matVPTM, &m_matInvVPTM);

	CreateViewFrustum();
	return atrue;
}

abool AXCameraOffCenter::CreateViewFrustum()
{
	AXPLANE *p1, *p2;

	p1 = m_ViewFrustum.GetPlane(AXFrustum::VF_TOP);
	p2 = m_ViewFrustum.GetPlane(AXFrustum::VF_BOTTOM);

	p1->vNormal = a3d_Normalize(AXVector3(0, -m_vFront, m_fTop));
	p2->vNormal = a3d_Normalize(AXVector3(0, m_vFront, -m_fBottom));

	p1->fDist = p2->fDist = 0.0f;

	p1 = m_ViewFrustum.GetPlane(AXFrustum::VF_LEFT);
	p2 = m_ViewFrustum.GetPlane(AXFrustum::VF_RIGHT);

	p1->vNormal = a3d_Normalize(AXVector3(m_vFront, 0, -m_fLeft));
	p2->vNormal = a3d_Normalize(AXVector3(-m_vFront, 0, m_fRight));

	p1->fDist = p2->fDist = 0.0f;

	//	Near and Far clipping plane
	p1 = m_ViewFrustum.GetPlane(AXFrustum::VF_NEAR);
	p2 = m_ViewFrustum.GetPlane(AXFrustum::VF_FAR);

	p1->vNormal.Set(0.0f, 0.0f, 1.0f);
	p2->vNormal.Set(0.0f, 0.0f, -1.0f);

	p1->fDist = p2->fDist = 0.0f;

	//	Update frustum in world coordinates
	UpdateWorldFrustum();

	return atrue;
}

abool AXCameraOffCenter::UpdateWorldFrustum()
{
	AXSPLANE *pw, *pv;

	//	Translate left plane
	pw = m_WorldFrustum.GetPlane(AXFrustum::VF_LEFT);
	pv = m_ViewFrustum.GetPlane(AXFrustum::VF_LEFT);

	pw->vNormal = a3d_ViewToWorld(pv->vNormal, m_matViewTM);
	pw->fDist = DotProduct(pw->vNormal, m_vecPos);
	pw->byType = AXSPLANE::TYPE_UNKNOWN;
	pw->MakeSignBits();

	//	Translate right plane
	pw = m_WorldFrustum.GetPlane(AXFrustum::VF_RIGHT);
	pv = m_ViewFrustum.GetPlane(AXFrustum::VF_RIGHT);

	pw->vNormal = a3d_ViewToWorld(pv->vNormal, m_matViewTM);
	pw->fDist = DotProduct(pw->vNormal, m_vecPos);
	pw->byType = AXSPLANE::TYPE_UNKNOWN;
	pw->MakeSignBits();

	//	Translate top plane
	pw = m_WorldFrustum.GetPlane(AXFrustum::VF_TOP);
	pv = m_ViewFrustum.GetPlane(AXFrustum::VF_TOP);

	pw->vNormal = a3d_ViewToWorld(pv->vNormal, m_matViewTM);
	pw->fDist = DotProduct(pw->vNormal, m_vecPos);
	pw->byType = AXSPLANE::TYPE_UNKNOWN;
	pw->MakeSignBits();

	//	Translate bottom plane
	pw = m_WorldFrustum.GetPlane(AXFrustum::VF_BOTTOM);
	pv = m_ViewFrustum.GetPlane(AXFrustum::VF_BOTTOM);

	pw->vNormal = a3d_ViewToWorld(pv->vNormal, m_matViewTM);
	pw->fDist = DotProduct(pw->vNormal, m_vecPos);
	pw->byType = AXSPLANE::TYPE_UNKNOWN;
	pw->MakeSignBits();

	//	Translate near plane
	pw = m_WorldFrustum.GetPlane(AXFrustum::VF_NEAR);

	AXVector3 vPos = m_vecPos + m_vecDir * m_vFront;

	pw->vNormal = m_vecDir;
	pw->fDist = DotProduct(pw->vNormal, vPos);
	pw->byType = AXSPLANE::TYPE_UNKNOWN;
	pw->MakeSignBits();

	//	Transform far plane
	pw = m_WorldFrustum.GetPlane(AXFrustum::VF_FAR);

	vPos = m_vecPos + m_vecDir * m_vBack;

	pw->vNormal = -m_vecDir;
	pw->fDist = DotProduct(pw->vNormal, vPos);
	pw->byType = AXSPLANE::TYPE_UNKNOWN;
	pw->MakeSignBits();

	return atrue;
}

void AXCameraOffCenter::SetMirror(const AXCameraBase* pOrgCamera, const AXMirrorParam& MirrorParam)
{
	if (pOrgCamera->GetType() != ACT_PERSPECTIVE)
	{
		return;
	}

	//	1.Set projection
	const AXVector3& vHostCameraPos = pOrgCamera->GetPos();
	AXVector3 vMirrorPointToEye = a3d_Normalize(vHostCameraPos - MirrorParam.vMirrorOrg);
	AXVector3 vMirrorNormal = a3d_Normalize(MirrorParam.vMirrorNormal);
	AXVector3 vMirrorCameraPos = vHostCameraPos;
	if (DotProduct(MirrorParam.vMirrorNormal, vMirrorPointToEye) >= 0.0f)
	{
		AXMatrix4 matMirror = a3d_MirrorMatrix(MirrorParam.vMirrorOrg, vMirrorNormal);
		vMirrorCameraPos = matMirror.TransformCoord(vHostCameraPos);
	}

	//	计算投影屏幕的角点
	AXVector3 vMirrorScreenNormal = -vMirrorNormal;
	AXVector3 vMirrorScreenUp(0.0f, 1.0f, 0.0f);
	if (abs((abs(DotProduct(vMirrorScreenUp, vMirrorScreenNormal)) - 1.0f)) <= 0.0001f)
	{
		vMirrorScreenUp = AXVector3(1.0f, 0.0f, 0.0f);
	}
	AXVector3 vMirrorScreenRight = a3d_Normalize(CrossProduct(vMirrorScreenNormal, vMirrorScreenUp));
	vMirrorScreenUp = a3d_Normalize(CrossProduct(vMirrorScreenRight, vMirrorScreenNormal));
	AXVector3 vMirrorScreenLB = MirrorParam.vMirrorOrg - (vMirrorScreenUp + vMirrorScreenRight) * MirrorParam.SBParam.fMirrorRadius;
	AXVector3 vMirrorScreenRB = MirrorParam.vMirrorOrg + (vMirrorScreenRight - vMirrorScreenUp) * MirrorParam.SBParam.fMirrorRadius;
	AXVector3 vMirrorScreenLT = MirrorParam.vMirrorOrg + (vMirrorScreenUp - vMirrorScreenRight) * MirrorParam.SBParam.fMirrorRadius;

	//	重新设置相机位置，避免相机距离屏幕太近而造成极限透视
	afloat32 fDistFromEyeToScreen = -DotProduct(vMirrorScreenLB - vMirrorCameraPos, vMirrorScreenNormal);
	if (fDistFromEyeToScreen < MirrorParam.SBParam.fMinNearClip)
	{
		vMirrorCameraPos = vMirrorCameraPos + vMirrorScreenNormal * (MirrorParam.SBParam.fMinNearClip - fDistFromEyeToScreen);
	}

	//	Compute the screen corner vectors
	AXVector3 vEyeToLB = vMirrorScreenLB - vMirrorCameraPos;
	AXVector3 vEyeToRB = vMirrorScreenRB - vMirrorCameraPos;
	AXVector3 vEyeToLT = vMirrorScreenLT - vMirrorCameraPos;

	//	Find the distance from the eye to screen plane
	afloat32 fDist = -DotProduct(vEyeToLB, vMirrorScreenNormal);

	//	Find the extent of the perpendicular projection
	//	Set the screen as the near clip plane
	afloat32 fLeft = DotProduct(vMirrorScreenRight, vEyeToLB);
	afloat32 fRight = DotProduct(vMirrorScreenRight, vEyeToRB);
	afloat32 fBottom = DotProduct(vMirrorScreenUp, vEyeToLB);
	afloat32 fTop = DotProduct(vMirrorScreenUp, vEyeToLT);

	SetProjectionParam(fLeft, fRight, fBottom, fTop, fDist, pOrgCamera->GetZBack());

	//	2.Set view
	SetPos(vMirrorCameraPos);
	SetDirAndUp(vMirrorNormal, vMirrorScreenUp);

	m_bMirrored = atrue;
}
