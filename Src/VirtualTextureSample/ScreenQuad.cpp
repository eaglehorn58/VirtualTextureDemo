//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#include "Global.h"
#include "ScreenQuad.h"
#include "DXVertexShader.h"
#include "DXPixelShader.h"
#include "DXTexture.h"
#include "DXUATexture.h"
#include "DXSampler.h"
#include "DXRenderState.h"
#include "DXRenderer.h"
#include "A3DMacros.h"
#include "AXViewport.h"

///////////////////////////////////////////////////////////////////////////

CScreenQuad::CScreenQuad() :
m_bFullScreen(true),
m_rcQuad(0, 0, 100, 100),
m_pvsFull(NULL),
m_pvsQuad(NULL)
{
	//	Load vertex shader
	m_pvsFull = new CDXVertexShader;
	m_pvsQuad = new CDXVertexShader;
	if (!m_pvsFull->Init("Shaders/ScreenQuad_vs.hlsl", "vs_fullquad", NULL) ||
		!m_pvsQuad->Init("Shaders/ScreenQuad_vs.hlsl", "vs_quad", &m_vsMate))
	{
		MyAssert(0);
		glb_OutputLog("CScreenQuad::Init, Failed to create vertex shader !");
	}
}

CScreenQuad::~CScreenQuad()
{
	A3DRELEASE(m_pvsFull);
	A3DRELEASE(m_pvsQuad);
}

void CScreenQuad::ApplyVS()
{
	if (m_bFullScreen)
	{
	//	m_pvsFull->UpdateParams(&m_vsMate, 0);
		m_pvsFull->Apply();
	}
	else
	{
		m_vsMate.data.vRect.x = (float)m_rcQuad.left;
		m_vsMate.data.vRect.y = (float)m_rcQuad.top;
		m_vsMate.data.vRect.z = (float)m_rcQuad.right;
		m_vsMate.data.vRect.w = (float)m_rcQuad.bottom;

		A3DVIEWPORTPARAM* pvp = DXRenderer::Instance()->GetCurViewport()->GetParam();
		m_vsMate.data.fScnWid = pvp->Width;
		m_vsMate.data.fScnHei = pvp->Height;

		m_pvsQuad->UpdateParams(&m_vsMate, 0);
		m_pvsQuad->Apply();
	}
}

bool CScreenQuad::Render()
{
	ID3D11DeviceContext* pDevContext = DXRenderer::Instance()->GetDeviceContext();

	//	Apply vertex shader
	ApplyVS();

	pDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	pDevContext->Draw(4, 0);

	return true;
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement of CTexScreenQuad
//
///////////////////////////////////////////////////////////////////////////

CTexScreenQuad::CTexScreenQuad() :
CScreenQuad()
{
	//	Load internal pixel shader
	m_pps = new CDXPixelShader;
	if (!m_pps->Init("Shaders/ScreenQuad_ps.hlsl", "ps_main", NULL))
	{
		MyAssert(0);
		glb_OutputLog("CTexScreenQuad::Init, Failed to create pixel shader !");
	}

	//	internal render states
	CDXRenderState::DEPTH ds;
	ds.d.DepthEnable = FALSE;
	ds.d.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	m_pRS = new CDXRenderState(NULL, &ds, NULL, true);
	//	internal sampler
	m_pSampler = new CDXSampler(CDXSampler::FILTER_LINEAR, CDXSampler::ADDR_CLAMP);
}

CTexScreenQuad::~CTexScreenQuad()
{
	A3DSAFEDELETE(m_pRS);
	A3DSAFEDELETE(m_pSampler);
	A3DRELEASE(m_pps);
}

bool CTexScreenQuad::Render(CDXTexture* pTex)
{
	MyAssert(pTex);

	pTex->ApplyToPS(0);
	m_pSampler->ApplyToPS(0);
	m_pps->Apply();
	m_pRS->Apply();

	return CScreenQuad::Render();
}

bool CTexScreenQuad::Render(CDXUATexture* pTex)
{
	MyAssert(pTex);

	pTex->ApplySRVToPS(0);
	m_pSampler->ApplyToPS(0);
	m_pps->Apply();
	m_pRS->Apply();

	return CScreenQuad::Render();
}

