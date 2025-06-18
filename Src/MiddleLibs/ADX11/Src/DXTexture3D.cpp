//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com

#pragma warning(disable:4005)

#include "DXRenderer.h"
#include "DXTexture3D.h"
#include "DXTexture.h"
#include "AXAssist.h"
#include "ALog.h"
#include "A3DMacros.h"
#include "DirectXTex.h"
#include "ScreenGrab.h"
#include "AMemory.h"
#include <wincodec.h>

///////////////////////////////////////////////////////////////////////////

CDXTexture3D::TEX_DESC::TEX_DESC() :
width(0),
height(0),
depth(0),
fmt(DXGI_FORMAT_UNKNOWN),
iMipLevels(0),
aMips(NULL)
{
}

CDXTexture3D::TEX_DESC::~TEX_DESC()
{
	if (aMips)
	{
		a_free(aMips);
		aMips = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////

CDXTexture3D::CDXTexture3D() :
m_pDXTex(nullptr),
m_uCPUAccess(0),
m_bUAV(false),
m_psrv(nullptr)
{
}

CDXTexture3D::~CDXTexture3D()
{

}

void CDXTexture3D::Release()
{
	for (int i = 0; i < (int)m_uavs.size(); i++)
	{
		A3DINTERFACERELEASE(m_uavs[i]);
	}

	m_uavs.clear();

	A3DINTERFACERELEASE(m_psrv);
	A3DINTERFACERELEASE(m_pDXTex);
}

bool CDXTexture3D::Init(int width, int height, int depth, DXGI_FORMAT fmt, int iMipLevels, auint32 uCPUAccess)
{
	m_uCPUAccess = uCPUAccess;
	m_bUAV = false;

	//	Create texture
	if (!Create(width, height, depth, fmt, iMipLevels))
	{
		a_OutputLog("CDXTexture3D::Init, failed to call Create() !");
		return false;
	}

	return true;
}

bool CDXTexture3D::InitUAV(int width, int height, int depth, DXGI_FORMAT fmt, int iMipLevels)
{
	m_uCPUAccess = 0;
	m_bUAV = true;

	//	Create texture
	if (!Create(width, height, depth, fmt, iMipLevels))
	{
		a_OutputLog("CDXTexture3D::Init, failed to call Create() !");
		return false;
	}

	return true;
}

bool CDXTexture3D::Create(int width, int height, int depth, DXGI_FORMAT fmt, int iMipLevels)
{
	//	Fill texture desc
	D3D11_TEXTURE3D_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Width = width;
	desc.Height = height;
	desc.Depth = depth;
	desc.MipLevels = iMipLevels;
	desc.Format = fmt;
	//desc.MiscFlags = 0;

	if (m_bUAV)
	{
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
	}
	else if (m_uCPUAccess)
	{
		//	Note: D3D11_USAGE_STAGING texture can be only used as resource like a relay station, 
		//	and can not be bound to the rendering pipeline
		//	see detail: http://www.programmersought.com/article/3963513072/
		if (m_uCPUAccess & CPU_DYNAMIC)
		{
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}
		else
		{
			desc.Usage = D3D11_USAGE_STAGING;
			desc.BindFlags = 0;
			desc.CPUAccessFlags = 0;

			if (m_uCPUAccess & CPU_READ)
				desc.CPUAccessFlags |= D3D11_CPU_ACCESS_READ;

			if (m_uCPUAccess & CPU_WRITE)
				desc.CPUAccessFlags |= D3D11_CPU_ACCESS_WRITE;
		}
	}
	else
	{
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
	}

	//	Create texture
	HRESULT hr = DXRenderer::Instance()->GetDevice()->CreateTexture3D(&desc, NULL, &m_pDXTex);
	if (FAILED(hr))
	{
		a_OutputLog("CDXTexture3D::Create, failed to call CreateTexture3D !");
		return false;
	}

	//	Fill texture description
	m_pDXTex->GetDesc(&desc);

	m_desc.width = desc.Width;
	m_desc.height = desc.Height;
	m_desc.depth = desc.Depth;
	m_desc.fmt = desc.Format;
	m_desc.iMipLevels = desc.MipLevels;
	m_desc.aMips = (MIP_INFO*)a_malloc(m_desc.iMipLevels * sizeof(MIP_INFO));

	//	Fill mipmap info
	for (int i = 0; i < m_desc.iMipLevels; i++)
	{
		MIP_INFO& info = m_desc.aMips[i];

		info.iWidth = m_desc.width >> i;
		info.iHeight = m_desc.height >> i;
		info.iDepth = m_desc.depth >> i;
		a_ClampFloor(info.iWidth, 1);
		a_ClampFloor(info.iHeight, 1);

		size_t row_pitch = 0, slice_pitch = 0;
		DirectX::ComputePitch(m_desc.fmt, info.iWidth, info.iHeight, row_pitch, slice_pitch);
		info.iRowPitch = (int)row_pitch;
		info.iSlicePitch = (int)slice_pitch;
	}

	//	Create texture view
	if (!CreateTextureView())
	{
		a_OutputLog("CDXTexture3D::Create, failed to call CreateTextureView() !");
		return false;
	}

	return true;
}

bool CDXTexture3D::CreateTextureView()
{
	//	Staging texture can't create shader resource view, so return directly
	if ((m_uCPUAccess & CPU_READ) || (m_uCPUAccess & CPU_WRITE))
		return true;

	//	create srv
	D3D11_SHADER_RESOURCE_VIEW_DESC vd;
	memset(&vd, 0, sizeof(vd));

	vd.Format = m_desc.fmt;
	vd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
	vd.Texture3D.MostDetailedMip = 0;
	vd.Texture3D.MipLevels = -1;// m_desc.iMipLevels;

	HRESULT hr = DXRenderer::Instance()->GetDevice()->CreateShaderResourceView(m_pDXTex, &vd, &m_psrv);
	if (FAILED(hr))
	{
		a_OutputLog("CDXTexture3D::CreateTextureView, failed to call CreateShaderResourceView() !");
		return false;
	}

	//	create uav
	if (m_bUAV)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc;
		memset(&uav_desc, 0, sizeof(uav_desc));
		uav_desc.Format = m_desc.fmt;
		uav_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;

		for (int i = 0; i < m_desc.iMipLevels; i++)
		{
			uav_desc.Texture3D.MipSlice = i;
			uav_desc.Texture3D.FirstWSlice = 0;
			uav_desc.Texture3D.WSize = m_desc.depth;

			ID3D11UnorderedAccessView* pUAV = nullptr;
			hr = DXRenderer::Instance()->GetDevice()->CreateUnorderedAccessView(m_pDXTex, &uav_desc, &pUAV);
			if (FAILED(hr))
			{
				a_OutputLog("CDXTexture3D::Init, failed to call CreateUnorderedAccessView()!");
				return false;
			}

			m_uavs.push_back(pUAV);
		}
	}

	return true;
}

void CDXTexture3D::CopyFrom(CDXTexture3D* pSrc, int mip/* -1 */)
{
	ASSERT(m_pDXTex && pSrc->GetDXTex());

	const CDXTexture3D::TEX_DESC& src_desc = pSrc->GetDesc();

	//	Check if data format is match
	ASSERT(src_desc.iMipLevels == m_desc.iMipLevels);
	ASSERT(src_desc.fmt == m_desc.fmt);
	ASSERT(src_desc.width == m_desc.width && src_desc.height == m_desc.height);
	ASSERT(src_desc.depth == m_desc.depth);

	ID3D11DeviceContext* pDevContext = DXRenderer::Instance()->GetDeviceContext();

	if (mip < 0)
	{
		//	copy all mips
		pDevContext->CopyResource(m_pDXTex, pSrc->GetDXTex());
	}
	else
	{
		//	only copy specified mip level
		ASSERT(mip < m_desc.iMipLevels && mip < src_desc.iMipLevels);
		UINT riSrc = D3D11CalcSubresource(mip, 0, src_desc.iMipLevels);
		UINT riDst = D3D11CalcSubresource(mip, 0, m_desc.iMipLevels);
		pDevContext->CopySubresourceRegion(m_pDXTex, riDst, 0, 0, 0, pSrc->GetDXTex(), riSrc, NULL);
	}
}

void CDXTexture3D::CopyOut(CDXTexture3D* pDst, int mip/* -1 */)
{
	ASSERT(m_pDXTex && pDst->GetDXTex());

	const CDXTexture3D::TEX_DESC& dst_desc = pDst->GetDesc();

	//	Check if data format is match
	ASSERT(dst_desc.iMipLevels == m_desc.iMipLevels);
	ASSERT(dst_desc.fmt == m_desc.fmt);
	ASSERT(dst_desc.width == m_desc.width && dst_desc.height == m_desc.height);
	ASSERT(dst_desc.depth == m_desc.depth);

	ID3D11DeviceContext* pDevContext = DXRenderer::Instance()->GetDeviceContext();

	if (mip < 0)
	{
		//	copy all mips
		pDevContext->CopyResource(pDst->GetDXTex(), m_pDXTex);
	}
	else
	{
		//	only copy specified mip level
		ASSERT(mip < m_desc.iMipLevels && mip < dst_desc.iMipLevels);
		UINT riDst = D3D11CalcSubresource(mip, 0, dst_desc.iMipLevels);
		UINT riSrc = D3D11CalcSubresource(mip, 0, m_desc.iMipLevels);
		pDevContext->CopySubresourceRegion(pDst->GetDXTex(), riDst, 0, 0, 0, m_pDXTex, riSrc, NULL);
	}
}

void CDXTexture3D::ApplySRVToPS(int iStartSlot)
{
	if (!m_pDXTex || !m_psrv)
	{
		ASSERT(m_pDXTex && m_psrv);
		return;
	}

	DXRenderer::Instance()->GetDeviceContext()->PSSetShaderResources(iStartSlot, 1, &m_psrv);
}

void CDXTexture3D::ApplySRVToCS(int iStartSlot)
{
	if (!m_pDXTex || !m_psrv)
	{
		ASSERT(m_pDXTex && m_psrv);
		return;
	}

	DXRenderer::Instance()->GetDeviceContext()->CSSetShaderResources(iStartSlot, 1, &m_psrv);
}

void CDXTexture3D::ApplyUAVToCS(int iStartSlot, int iSlotNum, int iStartMip)
{
	if (!m_pDXTex || !m_psrv)
	{
		ASSERT(m_pDXTex && m_psrv);
		return;
	}

	ASSERT(iStartMip + iSlotNum <= m_desc.iMipLevels);

	ID3D11UnorderedAccessView** ppUAVs = m_uavs.data() + iStartMip;
	DXRenderer::Instance()->GetDeviceContext()->CSSetUnorderedAccessViews(iStartSlot, iSlotNum, ppUAVs, NULL);
}


