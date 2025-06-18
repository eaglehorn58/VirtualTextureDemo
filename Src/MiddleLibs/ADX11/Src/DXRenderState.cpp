//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#include "DXRenderState.h"
#include "DXRenderer.h"
#include "ALog.h"
#include "A3DMacros.h"

///////////////////////////////////////////////////////////////////////////

CDXRenderState::BLEND::BLEND()
{
    d.AlphaToCoverageEnable = FALSE;
    d.IndependentBlendEnable = FALSE;

    for (int i = 0; i < 8; i++)
    {
        d.RenderTarget[i].BlendEnable = FALSE;
        d.RenderTarget[i].SrcBlend = D3D11_BLEND_ONE;
        d.RenderTarget[i].DestBlend	= D3D11_BLEND_ZERO;
        d.RenderTarget[i].BlendOp= D3D11_BLEND_OP_ADD;
        d.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
        d.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
        d.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        d.RenderTarget[i].RenderTargetWriteMask= D3D11_COLOR_WRITE_ENABLE_ALL;
    }
}

CDXRenderState::DEPTH::DEPTH()
{
    d.DepthEnable = TRUE;
    d.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    d.DepthFunc = D3D11_COMPARISON_LESS;
    d.StencilEnable = FALSE;
    d.StencilReadMask =	D3D11_DEFAULT_STENCIL_READ_MASK;
    d.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
    d.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    d.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    d.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    d.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    d.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    d.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    d.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    d.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
}

CDXRenderState::RASTERIZE::RASTERIZE()
{
    d.FillMode = D3D11_FILL_SOLID;
    d.CullMode = D3D11_CULL_BACK;
    d.FrontCounterClockwise = FALSE;
    d.DepthBias = 0;
    d.SlopeScaledDepthBias = 0.0f;
    d.DepthBiasClamp = 0.0f;
    d.DepthClipEnable = TRUE;
    d.ScissorEnable = FALSE;
    d.MultisampleEnable = FALSE;
    d.AntialiasedLineEnable = FALSE;
}

CDXRenderState::CDXRenderState() :
m_BlendFactor(1.0f),
m_SampleMask(0xffffffff),
m_StencilRef(0),
m_pBlendState(nullptr),
m_pDepthState(nullptr),
m_pRastState(nullptr),
m_pRastStateWF(nullptr),
m_bWireFrameEnable(false)
{
}

CDXRenderState::CDXRenderState(const BLEND* blend, const DEPTH* depth, const RASTERIZE* rast, bool bWireFrameEnable) :
CDXRenderState()
{
	m_bWireFrameEnable = bWireFrameEnable;

    if (blend)
    {
        DXRenderer::Instance()->GetDevice()->CreateBlendState(&blend->d, &m_pBlendState);
    }

    if (depth)
    {
        DXRenderer::Instance()->GetDevice()->CreateDepthStencilState(&depth->d, &m_pDepthState);
    }

    if (rast)
    {
        DXRenderer::Instance()->GetDevice()->CreateRasterizerState(&rast->d, &m_pRastState);

        if (bWireFrameEnable)
        {
            RASTERIZE r2 = *rast;
            r2.d.FillMode = D3D11_FILL_WIREFRAME;
			DXRenderer::Instance()->GetDevice()->CreateRasterizerState(&r2.d, &m_pRastStateWF);
        }
    }
    else
    {
        if (bWireFrameEnable)
        {
            RASTERIZE r2;
            r2.d.FillMode = D3D11_FILL_WIREFRAME;
			DXRenderer::Instance()->GetDevice()->CreateRasterizerState(&r2.d, &m_pRastStateWF);
        }
    }
}

CDXRenderState::~CDXRenderState()
{
    A3DINTERFACERELEASE(m_pBlendState);
    A3DINTERFACERELEASE(m_pDepthState);
    A3DINTERFACERELEASE(m_pRastState);
    A3DINTERFACERELEASE(m_pRastStateWF);
}

void CDXRenderState::Apply()
{
	bool bInWireFrame = DXRenderer::Instance()->IsInWireFrameMode();
	ID3D11DeviceContext* pDevContext = DXRenderer::Instance()->GetDeviceContext();

	pDevContext->OMSetBlendState(m_pBlendState, m_BlendFactor.c, m_SampleMask);
	pDevContext->OMSetDepthStencilState(m_pDepthState, m_StencilRef);
	ID3D11RasterizerState* pRast = (bInWireFrame && m_bWireFrameEnable) ? m_pRastStateWF : m_pRastState;
	pDevContext->RSSetState(pRast);
}
