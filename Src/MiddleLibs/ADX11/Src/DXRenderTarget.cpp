//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com

#include "DXRenderTarget.h"
#include "DXRenderer.h"
#include "ALog.h"
#include "A3DMacros.h"
#include "DXBuffer.h"

///////////////////////////////////////////////////////////////////////////

CDXRenderTarget::CDXRenderTarget() :
m_iWidth(0),
m_iHeight(0),
m_iNumRT(0),
m_fmtDepth(DXGI_FORMAT_UNKNOWN),
m_fClrDepth(1.0f),
m_uClrStencil(0),
m_pDepthBuf(NULL),
m_pDepthView(NULL)
{
	memset(&m_aColorRTs, 0, sizeof(m_aColorRTs));
}

CDXRenderTarget::~CDXRenderTarget()
{
}

void CDXRenderTarget::Release()
{
	A3DINTERFACERELEASE(m_pDepthView);
	A3DINTERFACERELEASE(m_pDepthBuf);

	for (int i = 0; i < m_iNumRT; i++)
	{
		COLOR_RT& rt = m_aColorRTs[i];

		A3DINTERFACERELEASE(rt.pRTView);
		A3DINTERFACERELEASE(rt.pRT);
	}

	m_iNumRT = 0;
}

bool CDXRenderTarget::Init(int width, int height, int iNumRT, DXGI_FORMAT* aFmts, DXGI_FORMAT fmtDepth)
{
	ASSERT(iNumRT > 0 && iNumRT <= MAX_SIM_RT_NUM);
	ASSERT(width > 0 && height > 0);

	m_iWidth = width;
	m_iHeight = height;
	m_iNumRT = iNumRT;
	m_fmtDepth = fmtDepth;

	for (int i = 0; i < iNumRT; i++)
	{
		COLOR_RT& rt = m_aColorRTs[i];
		rt.pRT = nullptr;
		rt.pRTView = nullptr;
		rt.fmt = aFmts[i];
		rt.color.Set(0.0f, 0.0f, 0.0f, 1.0f);
	}

	//	Create color targets
	for (int i = 0; i < iNumRT; i++)
	{
		if (!CreateColorTarget(i))
			return false;
	}

	//	Create depth target
	if (fmtDepth != DXGI_FORMAT_UNKNOWN)
	{
		if (!CreateDepthBuffer())
			return false;
	}

	return true;
}

bool CDXRenderTarget::CreateColorTarget(int slot)
{
	COLOR_RT& rt = m_aColorRTs[slot];

	//	Fill texture desc
	D3D11_TEXTURE2D_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Width = m_iWidth;
	desc.Height = m_iHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = rt.fmt;
	desc.SampleDesc.Count = 1;
	//	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	//	desc.CPUAccessFlags = 0;
	//	desc.MiscFlags = 0;
	
	//	Create texture
	HRESULT hr = DXRenderer::Instance()->GetDevice()->CreateTexture2D(&desc, NULL, &rt.pRT);
	if (FAILED(hr))
	{
		a_OutputLog("CDXRenderTarget::CreateColorTarget, failed to call CreateTexture2D, slot [%d] !", slot);
		return false;
	}

	//	Create render target view
	hr = DXRenderer::Instance()->GetDevice()->CreateRenderTargetView(rt.pRT, NULL, &rt.pRTView);
	if (FAILED(hr))
	{
		a_OutputLog("CDXRenderTarget::CreateColorTarget, failed to call CreateRenderTargetView, slot [%d] !", slot);
		return false;
	}

	return true;
}

bool CDXRenderTarget::CreateDepthBuffer()
{
	//	Fill texture desc
	D3D11_TEXTURE2D_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Width = m_iWidth;
	desc.Height = m_iHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = m_fmtDepth;
	desc.SampleDesc.Count = 1;
	//	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	//	desc.CPUAccessFlags = 0;
	//	desc.MiscFlags = 0;

	//	Create back buffer
	HRESULT hr = DXRenderer::Instance()->GetDevice()->CreateTexture2D(&desc, NULL, &m_pDepthBuf);
	if (FAILED(hr))
	{
		a_OutputLog("CDXRenderTarget::CreateDepthBuffer, failed to call CreateTexture2D !");
		return false;
	}

	//	Create back buffer view
	hr = DXRenderer::Instance()->GetDevice()->CreateDepthStencilView(m_pDepthBuf, NULL, &m_pDepthView);
	if (FAILED(hr))
	{
		a_OutputLog("CDXRenderTarget::CreateDepthBuffer, failed to call CreateDepthStencilView !");
		return false;
	}

	return true;
}

bool CDXRenderTarget::Resize(int width, int height)
{
	if (m_iWidth == width && m_iHeight == height)
		return true;

	//	Release current objects and views
	ReleaseObjectsAndViews();

	m_iWidth = width;
	m_iHeight = height;

	//	Create color targets
	for (int i = 0; i < m_iNumRT; i++)
	{
		if (!CreateColorTarget(i))
			return false;
	}

	//	Create depth target
	if (m_fmtDepth != DXGI_FORMAT_UNKNOWN)
	{
		if (!CreateDepthBuffer())
			return false;
	}

	return true;
}

void CDXRenderTarget::ReleaseObjectsAndViews()
{
	A3DINTERFACERELEASE(m_pDepthView);
	A3DINTERFACERELEASE(m_pDepthBuf);

	for (int i = 0; i < m_iNumRT; i++)
	{
		COLOR_RT& rt = m_aColorRTs[i];

		A3DINTERFACERELEASE(rt.pRTView);
		A3DINTERFACERELEASE(rt.pRT);
	}
}

void CDXRenderTarget::Apply(auint32 uClearFlags)
{
	ASSERT(m_iNumRT);

	ID3D11DeviceContext* pDevContext = DXRenderer::Instance()->GetDeviceContext();

	//	apply render targets
	ID3D11RenderTargetView* aRTViews[MAX_SIM_RT_NUM];
	for (int i = 0; i < m_iNumRT; i++)
		aRTViews[i] = m_aColorRTs[i].pRTView;

	pDevContext->OMSetRenderTargets(m_iNumRT, aRTViews, m_pDepthView);

	//	Clear targets
	ClearTargets(uClearFlags);
}

void CDXRenderTarget::ClearTargets(auint32 uClearFlags)
{
	ID3D11DeviceContext* pDevContext = DXRenderer::Instance()->GetDeviceContext();

	//	clear color targets
	if (uClearFlags & DXRenderer::CLEAR_COLOR)
	{
		for (int i = 0; i < m_iNumRT; i++)
		{
			const COLOR_RT& rt = m_aColorRTs[i];
			pDevContext->ClearRenderTargetView(rt.pRTView, rt.color.c);
		}
	}

	if (m_pDepthView)
	{
		//	clear depth target if depth buffer exists
		UINT uFlags = 0;
		if (uClearFlags & DXRenderer::CLEAR_DEPTH) uFlags |= D3D11_CLEAR_DEPTH;
		if (uClearFlags & DXRenderer::CLEAR_STENCIL) uFlags |= D3D11_CLEAR_STENCIL;

		if (uFlags)
		{
			pDevContext->ClearDepthStencilView(m_pDepthView, uFlags, m_fClrDepth, m_uClrStencil);
		}
	}
}

void CDXRenderTarget::SetClearColor(int slot, const A3DCOLORVALUE& color)
{
	ASSERT(slot >= 0 && slot < m_iNumRT);
	m_aColorRTs[slot].color = color;
}

const CDXRenderTarget::COLOR_RT& CDXRenderTarget::GetColorRT(int slot)
{
	ASSERT(slot >= 0 && slot < m_iNumRT);
	return m_aColorRTs[slot];
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement CDXRTSuite
//
///////////////////////////////////////////////////////////////////////////

CDXRTSuite::CDXRTSuite(CDXRenderTarget* pRT, CDXUABuffer** aUABufs, int iUABufNum)
{
	m_pRT = pRT;
	m_iUABufNum = iUABufNum;

	int iNumRT = pRT ? pRT->GetColorRTNum() : 0;
	ASSERT(iNumRT + iUABufNum <= MAX_SIM_RT_NUM);

	if (aUABufs)
	{
		for (int i = 0; i < iUABufNum; i++)
		{
			m_aUABufs[i] = aUABufs[i];
		}
	}
}

CDXRTSuite::~CDXRTSuite()
{
}

void CDXRTSuite::Apply(auint32 uClearFlags)
{
	ID3D11DeviceContext* pDevContext = DXRenderer::Instance()->GetDeviceContext();

	//	render targets
	ID3D11RenderTargetView* aRTViews[MAX_SIM_RT_NUM];
	ID3D11RenderTargetView** ppRTVs = nullptr;
	ID3D11DepthStencilView* pDSV = nullptr;
	int iNumRT = 0;

	if (m_pRT)
	{
		iNumRT = m_pRT->GetColorRTNum();
		ppRTVs = aRTViews;
		pDSV = m_pRT->GetDepthView();

		for (int i = 0; i < iNumRT; i++)
		{
			aRTViews[i] = m_pRT->GetColorRT(i).pRTView;
		}
	}

	//	UAVs
	ID3D11UnorderedAccessView* aUAViews[MAX_SIM_RT_NUM];
	ID3D11UnorderedAccessView** ppUAVs = nullptr;

	if (m_aUABufs)
	{
		ppUAVs = aUAViews;

		for (int i = 0; i < m_iUABufNum; i++)
		{
			aUAViews[i] = m_aUABufs[i]->GetUAV();
		}
	}

	//	apply to device
	pDevContext->OMSetRenderTargetsAndUnorderedAccessViews(iNumRT, ppRTVs,
		pDSV, iNumRT, m_iUABufNum, ppUAVs, nullptr);

	//	Clear targets
	if (m_pRT)
	{
		m_pRT->ClearTargets(uClearFlags);
	}
}


