//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#include "AXViewport.h"
//#include "A3DPI.h"
#include "A3DMacros.h"
#include "AXFuncs.h"
#include "AXCameraBase.h"

AXViewport::AXViewport()
:m_pA3DCamera(NULL),
 m_ClearColor(0.2f, 0.2f, 0.2f, 1.0f)
{

}

AXViewport::~AXViewport()
{
}

bool AXViewport::Init(A3DVIEWPORTPARAM * pParam)
{
	m_pA3DCamera = NULL;

	return SetParam(pParam);
}

bool AXViewport::SetParam(A3DVIEWPORTPARAM * pParam)
{
	m_ViewportParam = *pParam;

	//Construct the view scale transform matrix;
	m_matViewScale.Identity();
	m_matViewScale._11 = 1.0f * m_ViewportParam.Width / 2;
	m_matViewScale._22 = -1.0f * m_ViewportParam.Height / 2;
	m_matViewScale._33 = m_ViewportParam.MaxZ - m_ViewportParam.MinZ;
	m_matViewScale._41 = 1.0f * m_ViewportParam.X + m_ViewportParam.Width / 2;
	m_matViewScale._42 = 1.0f * m_ViewportParam.Y + m_ViewportParam.Height / 2;
	m_matViewScale._43 = m_ViewportParam.MinZ;
	
	a3d_InverseTM(m_matViewScale, &m_matInvViewScale);
	return true;
}

bool AXViewport::Release()
{
	return true;
}

//FIXME!! 用新的Render接口实现
/*
bool AXViewport::Active()
{
	// first adjust viewport param
	m_pA3DDevice->GetA3DEngine()->SetActiveViewport(this);
	m_pA3DDevice->SetViewport(&m_ViewportParam);

	// then adjust camera param if needed
	if( m_pA3DCamera )
	{
		m_pA3DDevice->GetA3DEngine()->SetActiveCamera(m_pA3DCamera);
		m_pA3DCamera->Active();
	}

	return true;
}
*/

bool AXViewport::SetCamera(AXCameraBase* pA3DCamera)
{
	m_pA3DCamera = pA3DCamera;
	return true;
}

//FIXME!! 用新的Render接口实现
/*
bool AXViewport::ClearDevice()
{
	auint32	dwFlags = 0;
	
	if( m_bClearColor )
	{
		dwFlags |= D3DCLEAR_TARGET;
	}

	if( m_bClearZ )
	{
		dwFlags |= D3DCLEAR_ZBUFFER;
	}

	if( NULL == dwFlags )
		return true;

// 	if FAILED(m_pA3DDevice->Clear(dwFlags, m_colorClear, 1.0f, 0) )
// 		return false;

	return true;
}
*/

/*
	Transform a 3D point from world to screen coordinates;
	return true if that point should be clipped;
	else return false;

	the vecOut is x, y, z, w, and the real x', y', z' should be x/w, y/w, z/w;
*/
bool AXViewport::Transform(const AXVector3& vecIn, AXVector3& vecOut)
{
	if( !m_pA3DCamera )
		return false;

	bool		bClipped;
	AXVector3	vecView;
								 
	bClipped = m_pA3DCamera->Transform(vecIn, vecView);

	if( vecView.z < m_ViewportParam.MinZ || vecView.z > m_ViewportParam.MaxZ )
		bClipped = true;

	vecOut = m_matViewScale.TransformCoord(vecView);
	return bClipped;
}

/*
	Transform a screen point to world space coordinates;
*/
bool AXViewport::InvTransform(const AXVector3& vecIn, AXVector3& vecOut)
{
	if( !m_pA3DCamera )
		return false;

	AXVector3	vecView;
			
	vecView = m_matInvViewScale.TransformCoord(vecIn);

	m_pA3DCamera->InvTransform(vecView, vecOut);
	return true;
}
