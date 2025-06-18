//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#include "AXCameraBase.h"
//#include "A3DPI.h"
#include "AXFuncs.h"
#include "AXCollision.h"
#include "AXAssist.h"
//#include "AMemory.h"

///////////////////////////////////////////////////////////////////////////

AXCameraBase::AXCameraBase()
{
	m_bFirstFrame = atrue;
	m_bMirrored = afalse;
}

AXCameraBase::~AXCameraBase()
{
}

//	Initlaize object
bool AXCameraBase::Init(float vFront, float vBack)
{
	m_vFront	= vFront;
	m_vBack		= vBack;

	m_matPostProjectTM.Identity();

	m_vecPos	= AXVector3(0.0f);
	m_vecDir	= AXVector3(0.0f, 0.0f, 1.0f);
	m_vecDirH	= AXVector3(0.0f, 0.0f, 1.0f);
	m_vecUp		= AXVector3(0.0f, 1.0f, 0.0f);
	m_vecRight	= AXVector3(1.0f, 0.0f, 0.0f);
	m_vecLeft	= AXVector3(-1.0f, 0.0f, 0.0f);

	m_matViewTM.Identity();
	m_preMatViewTM.Identity();
	m_preMatProjectTM.Identity();
	m_preMatVPTM.Identity();
	
	return true;
}


//	Tick every frame
void AXCameraBase::BeginRender(aint32 nDeltaTime, abool bClearPreMatrix)
{
	if (m_bFirstFrame || bClearPreMatrix)
	{
		m_preMatViewTM = m_matViewTM;
		m_preMatProjectTM = m_matProjectTM;
		m_preMatVPTM = m_matVPTM;

		m_bFirstFrame = afalse;
	}

}

void AXCameraBase::EndRender()
{
	m_preMatViewTM = m_matViewTM;
	m_preMatProjectTM = m_matProjectTM;
	m_preMatVPTM = m_matVPTM;
}

void AXCameraBase::SetDirAndUp(const AXVector3& vecDir, const AXVector3& vecUp)
{
	m_vecDir = Normalize(vecDir);
	m_vecUp = Normalize(vecUp);
	m_vecRight = Normalize(CrossProduct(m_vecUp, m_vecDir));
	m_vecUp = Normalize(CrossProduct(m_vecDir, m_vecRight));
	m_vecLeft = -m_vecRight;

	m_vecDirH = m_vecDir;
	m_vecDirH.y = 0.0f;
	m_vecDirH = Normalize(m_vecDirH);
	m_vecLeftH = m_vecLeft;
	m_vecLeftH.y = 0.0f;
	m_vecLeftH = Normalize(m_vecLeftH);

	m_vDeg = (float)RAD2DEG(atan2(-m_vecDir.z, m_vecDir.x));
	m_vPitch = (float)RAD2DEG(asin(m_vecDir.y));
	UpdateViewTM();
}

// Set a camera's view matrix using an existing view matrix
bool AXCameraBase::SetViewTM(const AXMatrix4& matView)
{
	AXMatrix4 matInv;

	m_matViewTM = matView;

	m_matVPTM = m_matViewTM * m_matProjectTM * m_matPostProjectTM;
	a3d_InverseMatrix(m_matVPTM, &m_matInvVPTM);

	AXVector3 vecDir, vecUp, vecPos;
	vecDir.x = matView.m[0][2];
	vecDir.y = matView.m[1][2];
	vecDir.z = matView.m[2][2];
	vecUp.x = matView.m[0][1];
	vecUp.y = matView.m[1][1];
	vecUp.z = matView.m[2][1];

	vecPos = matView.GetRow(3);

	matInv = matView;
	matInv._41 = matInv._42 = matInv._43 = 0.0f;
	matInv = a3d_Transpose(matInv);
	vecPos = matInv.TransformCoord(-1.0f * vecPos);
	
	SetDirAndUp(vecDir, vecUp);
	SetPos(vecPos);
	return true;
}

bool AXCameraBase::UpdateViewTM()
{
	m_matViewTM = a3d_ViewMatrix(m_vecPos, m_vecDir, m_vecUp, 0.0f);

	m_matVPTM = m_matViewTM * m_matProjectTM * m_matPostProjectTM;
	a3d_InverseMatrix(m_matVPTM, &m_matInvVPTM);

	UpdateWorldFrustum();
	return true;
}

bool AXCameraBase::SetProjectionTM(const AXMatrix4& matProjection)
{
	m_matProjectTM = matProjection;
	a3d_InverseMatrix(m_matProjectTM, &m_matInvProjectTM);

	m_matVPTM = m_matViewTM * m_matProjectTM * m_matPostProjectTM;
	a3d_InverseMatrix(m_matVPTM, &m_matInvVPTM);

	CreateViewFrustum();
	return true;
}

//FIXME!! 用新的Render接口实现
/*
bool AXCameraBase::Active()
{
	if (!m_pA3DDevice)
		return true;

	if (m_bMirrored)
		m_pA3DDevice->SetFaceCull(A3DCULL_CW);
	else
		m_pA3DDevice->SetFaceCull(A3DCULL_CCW);
	
	m_pA3DDevice->SetViewMatrix(GetViewTM());
	m_pA3DDevice->SetProjectionMatrix(m_matProjectTM * m_matPostProjectTM);

	return true;
}
*/

bool AXCameraBase::MoveFront(float vDistance)
{
	m_vecPos = m_vecPos + m_vecDirH * vDistance;
	UpdateViewTM();
	return true;
}

bool AXCameraBase::MoveBack(float vDistance)
{
	m_vecPos = m_vecPos - m_vecDirH * vDistance;
	UpdateViewTM();
	return true;
}

bool AXCameraBase::MoveLeft(float vDistance)
{
	m_vecPos = m_vecPos + m_vecLeftH * vDistance;
	UpdateViewTM();
	return true;
}

bool AXCameraBase::MoveRight(float vDistance)
{
	m_vecPos = m_vecPos - m_vecLeftH * vDistance;
	UpdateViewTM();
	return true;
}

bool AXCameraBase::DegDelta(float vDelta)
{
	m_vDeg += vDelta;
	UpdateDirAndUp();
	UpdateViewTM();
	return true;
}

bool AXCameraBase::PitchDelta(float vDelta)
{
	m_vPitch += vDelta;
	m_vPitch = ax_Clamp(m_vPitch, -89.0f, 89.0f);

	UpdateDirAndUp();
	UpdateViewTM();
	return true;
}

bool AXCameraBase::Move(const AXVector3& vecDelta)
{
	m_vecPos = m_vecPos + vecDelta;
	UpdateViewTM();
	return true;
}

const AXVector3& AXCameraBase::GetPos() const
{
	return m_vecPos;
}

bool AXCameraBase::UpdateDirAndUp()
{
	m_vecDir.x = (float)(cos(DEG2RAD(m_vDeg)) * cos(DEG2RAD(m_vPitch)));
	m_vecDir.y = (float)sin(DEG2RAD(m_vPitch));
	m_vecDir.z = (float)(-sin(DEG2RAD(m_vDeg)) * cos(DEG2RAD(m_vPitch)));

	m_vecRight = Normalize(CrossProduct(AXVector3(0.0f, 1.0f, 0.0f), m_vecDir));
	m_vecUp = Normalize(CrossProduct(m_vecDir, m_vecRight));
	m_vecLeft = -m_vecRight;

	m_vecDirH = m_vecDir;
	m_vecDirH.y = 0.0f;
	m_vecDirH = Normalize(m_vecDirH);
	m_vecLeftH = m_vecLeft;
	m_vecLeftH.y = 0.0f;
	m_vecLeftH = Normalize(m_vecLeftH);
	return true;
}

/*	Transform one 3D world space vector into projection plane coordinates;
	the coordinated ranges are 
	return true if this point should be clipped;
	return false if not;
*/
bool AXCameraBase::Transform(const AXVector3& vecIn, AXVector3& vecOut)
{
	vecOut = m_matVPTM.TransformCoord(vecIn);

	if (vecOut.x < -1.0f || vecOut.y < -1.0f ||
		vecOut.x > 1.0f || vecOut.y > 1.0f ||
		vecOut.z < 0.0f || vecOut.z > 1.0f )
		return true;
	else
		return false;
}

/*
	Transform one projection plane coordinates into world space coordinates;
*/
bool AXCameraBase::InvTransform(const AXVector3& vecIn, AXVector3& vecOut)
{
	vecOut = m_matInvVPTM.TransformCoord(vecIn);
	return true;
}

/* 
	Rotate the camera around an axis in world space;
*/
bool AXCameraBase::TurnAroundAxis(const AXVector3& vecPos, const AXVector3& vecAxis, float vDeltaRad)
{
	AXMatrix4 matRotate = a3d_RotateAxis(vecPos, vecAxis, vDeltaRad);
	AXVector3 vecOrigin = matRotate.TransformCoord(AXVector3(0.0f));
	m_vecPos = matRotate.TransformCoord(m_vecPos);
	m_vecDir = matRotate.TransformCoord(m_vecDir) - vecOrigin;
	m_vecUp  = matRotate.TransformCoord(m_vecUp) - vecOrigin;

	SetDirAndUp(m_vecDir, m_vecUp);
	return true;
}

bool AXCameraBase::SetDeg(float vDeg)
{
	m_vDeg = vDeg;

	UpdateDirAndUp();
	UpdateViewTM();
	return true;
}

bool AXCameraBase::SetPitch(float vPitch)
{
	m_vPitch = vPitch;
	m_vPitch = ax_Clamp(m_vPitch, -89.0f, 89.0f);

	UpdateDirAndUp();
	UpdateViewTM();
	return true;
}

// Please don't call this function only once per tick, or maybe for several tick,
// so we can save some cpu usage;
bool AXCameraBase::UpdateEar()
{
	//FIXME!! 用新的Sound接口实现
	/*
	AM3DSoundDevice * pAM3DSoundDevice = m_pA3DDevice->GetA3DEngine()->GetAMSoundEngine()->GetAM3DSoundDevice();

	// Now we should adjust the 3d sound device's pos and orientation;
	if (pAM3DSoundDevice)
	{
		pAM3DSoundDevice->SetPosition(m_vecPos);
		pAM3DSoundDevice->SetOrientation(m_vecDir, m_vecUp);
		pAM3DSoundDevice->UpdateChanges();
	}
	*/
	return true;
}
//FIXME!! 用新的Render接口实现
/*
bool AXCameraBase::SetZBias(FLOAT vZBias)
{
	if (!m_pA3DDevice)
		return true;

	AXMatrix4 matProjectTM = m_matProjectTM;
//	matProjectTM._43 += vZBias;
	matProjectTM._33 *= vZBias;
	
	m_pA3DDevice->SetProjectionMatrix(matProjectTM * m_matPostProjectTM);
	return true;
}
*/

//	Set Z front and back distance
bool AXCameraBase::SetZFrontAndBack(float fFront, float fBack, bool bUpdateProjMat/* true */)
{
	if( fFront > 0.0f )
		m_vFront = fFront;
	if( fBack > 0.0f )
		m_vBack		= fBack;

	if (bUpdateProjMat)
		UpdateProjectTM();

	return true;
}


bool AXCameraBase::AABBInViewFrustum(const AXAABB& aabb) 
{ 
	return m_WorldFrustum.AABBInFrustum(aabb) < 0 ? false : true; 
}

bool AXCameraBase::AABBInViewFrustum(const AXVector3& vMins, const AXVector3& vMaxs)
{ 
	return m_WorldFrustum.AABBInFrustum(vMins, vMaxs) < 0 ? false : true; 
}

bool AXCameraBase::SphereInViewFrustum(const AXVector3& vCenter, float fRadius) 
{ 
	return m_WorldFrustum.SphereInFrustum(vCenter, fRadius) < 0 ? false : true; 
}

bool AXCameraBase::PointInViewFrustum(const AXVector3& vPos)
{ 
	return m_WorldFrustum.PointInFrustum(vPos); 
}
