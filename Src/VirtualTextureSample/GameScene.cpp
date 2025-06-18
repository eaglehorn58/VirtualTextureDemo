//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#include "Global.h"
#include "GameScene.h"
#include "TestMesh.h"
#include "VTMainWindow.h"
#include "VTextureSystem.h"
#include "VTexturePageRequest.h"
#include "VTextureAtlas.h"
#include "DXRenderState.h"
#include "DXRenderer.h"
#include "AXViewport.h"
#include "AXCameraBase.h"
#include "DXRenderTarget.h"
#include "A3DMacros.h"
#include "DXBuffer.h"

CGameScene::CGameScene() :
m_pRT(nullptr),
m_pTestMesh(nullptr)
{
	//	render states for Z-pre pass
	CDXRenderState::BLEND blend;
	blend.d.RenderTarget[0].RenderTargetWriteMask = 0;	//	disable color buffer
	m_prsZPrePass = new CDXRenderState(&blend, nullptr, nullptr, false);

	//	render state for lighting pass
	CDXRenderState::DEPTH depth;
	depth.d.DepthFunc = D3D11_COMPARISON_EQUAL;
	depth.d.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	m_prsLight = new CDXRenderState(nullptr, &depth, nullptr, false);
}

CGameScene::~CGameScene()
{
	A3DSAFEDELETE(m_prsZPrePass);
	A3DSAFEDELETE(m_prsLight);
}

bool CGameScene::Init()
{
	int width = DXRenderer::Instance()->GetScreenWidth();
	int height = DXRenderer::Instance()->GetScreenHeight();
	DXGI_FORMAT fmt = DXGI_FORMAT_R8G8B8A8_UNORM;

	m_pRT = new CDXRenderTarget;
	if (!m_pRT->Init(width, height, 1, &fmt, DXGI_FORMAT_UNKNOWN))
	{
		glb_OutputLog("CGameScene::Init, Failed to splatting target 1 !");
		return false;
	}

	//	Create test mesh
	m_pTestMesh = new CTestMesh;
	if (!m_pTestMesh || !m_pTestMesh->Init())
	{
		glb_OutputLog("CGameScene::Init, failed to test mesh !");
		return false;
	}

	return true;
}

void CGameScene::Release()
{
	A3DRELEASE(m_pTestMesh);
	A3DRELEASE(m_pRT);
}

void CGameScene::OnWindowResize(int width, int height)
{
	if (m_pRT)
	{
		m_pRT->Resize(width, height);
	}
}

void CGameScene::Tick(float time)
{
	//	load requested VPages
	CVTextureSystem* pVTSys = g_pMainWnd->GetVTSystem();
	pVTSys->GetVTAtlas()->LoadRequestedPages();
}

bool CGameScene::Render(AXCameraBase* pCamera)
{
	DXRenderer* pRenderer = DXRenderer::Instance();
	ID3D11DeviceContext* pDevContext = pRenderer->GetDeviceContext();

	CVTextureSystem* pVTSys = g_pMainWnd->GetVTSystem();

	//--------------------------------------
	//	Uppdate VT atlas
	//--------------------------------------
	pVTSys->GetVTAtlas()->RenderAtlas();

	//--------------------------------------
	//	Z-Pre pass
	//--------------------------------------
	m_prsZPrePass->Apply();

	//	Render test mesh
	m_pTestMesh->RenderZPrePass(pCamera);

	//--------------------------------------
	//	Light pass
	//--------------------------------------
	//	Prepare VT page request UA buffer
	pVTSys->BeforePageIDRequestRendering();
	ID3D11UnorderedAccessView* pUAV = pVTSys->GetVTPageRequest()->GetUABuffer()->GetUAV();

	//	Set render target and UAV
	const CDXRenderTarget::COLOR_RT& rt = m_pRT->GetColorRT(0);
	pDevContext->OMSetRenderTargetsAndUnorderedAccessViews(1, &rt.pRTView,
		pRenderer->GetDepthView(), 1, 1, &pUAV, nullptr);

	pDevContext->ClearRenderTargetView(rt.pRTView, rt.color.c);

	//	set render state for lighting pass
	m_prsLight->Apply();

	//	Render test mesh
	m_pTestMesh->Render(pCamera);

	//	Restore render targets
	ID3D11RenderTargetView* pBackBuf = pRenderer->GetBackBufferView();
	pDevContext->OMSetRenderTargets(1, &pBackBuf, pRenderer->GetDepthView());

	//--------------------------------------
	//	Collect VPages IDs from UAV
	//--------------------------------------
	pVTSys->AfterPageIDRequestRendering();

	//	Copy to back buffer
	pRenderer->GetDeviceContext()->CopyResource(pRenderer->GetBackBuffer(), m_pRT->GetColorRT(0).pRT);

	return true;
}



