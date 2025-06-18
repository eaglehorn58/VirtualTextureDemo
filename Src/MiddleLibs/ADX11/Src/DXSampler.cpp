//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#include "DXSampler.h"
#include "DXRenderer.h"
#include "ALog.h"
#include "A3DMacros.h"

///////////////////////////////////////////////////////////////////////////

CDXSampler::SAMPLER_STATE::SAMPLER_STATE()
{
	//	default sampler states
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.MipLODBias = 0;
	sd.MaxAnisotropy = 1;
	sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sd.BorderColor[0] = 1.0f;
	sd.BorderColor[1] = 1.0f;
	sd.BorderColor[2] = 1.0f;
	sd.BorderColor[3] = 1.0f;
	sd.MinLOD = -FLT_MAX;
	sd.MaxLOD = FLT_MAX;
}

///////////////////////////////////////////////////////////////////////////

CDXSampler::CDXSampler() :
m_pSampler(nullptr)
{
}

CDXSampler::CDXSampler(const SAMPLER_STATE& s)
{
	DXRenderer::Instance()->GetDevice()->CreateSamplerState(&s.sd, &m_pSampler);
}

CDXSampler::CDXSampler(FILTER filter, ADDRESS addr, int iAnisotropy)
{
	SAMPLER_STATE s;

	if (filter == FILTER_POINT)
		s.sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	else if (filter == FILTER_LINEAR)
		s.sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	else
		s.sd.Filter = D3D11_FILTER_ANISOTROPIC;

	if (addr == ADDR_WRAP)
	{
		s.sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		s.sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		s.sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	}
	else if (addr == ADDR_MIRROR)
	{
		s.sd.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
		s.sd.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
		s.sd.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	}
	else if (addr == ADDR_CLAMP)
	{
		s.sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		s.sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		s.sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	}
	else
	{
		//	ADDR_BORDER
		s.sd.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		s.sd.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		s.sd.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	}

	s.sd.MaxAnisotropy = iAnisotropy;

	DXRenderer::Instance()->GetDevice()->CreateSamplerState(&s.sd, &m_pSampler);
}

CDXSampler::~CDXSampler()
{
	A3DINTERFACERELEASE(m_pSampler);
}

void CDXSampler::ApplyToPS(int slot)
{
	ASSERT(m_pSampler);
	DXRenderer::Instance()->GetDeviceContext()->PSSetSamplers(slot, 1, &m_pSampler);
}

void CDXSampler::ApplyToVS(int slot)
{
	ASSERT(m_pSampler);
	DXRenderer::Instance()->GetDeviceContext()->VSSetSamplers(slot, 1, &m_pSampler);
}

void CDXSampler::ApplyToCS(int slot)
{
	ASSERT(m_pSampler);
	DXRenderer::Instance()->GetDeviceContext()->CSSetSamplers(slot, 1, &m_pSampler);
}