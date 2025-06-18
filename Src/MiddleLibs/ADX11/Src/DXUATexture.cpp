//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#pragma warning(disable:4005)

#include "DXRenderer.h"
#include "DXUATexture.h"
#include "DXTexture.h"
#include "AXAssist.h"
#include "ALog.h"
#include "A3DMacros.h"
#include "DirectXTex.h"
#include "ScreenGrab.h"
#include "AMemory.h"
#include <wincodec.h>

///////////////////////////////////////////////////////////////////////////

CDXUATexture::TEX_DESC::TEX_DESC() :
width(0),
height(0),
fmt(DXGI_FORMAT_UNKNOWN),
iMipLevels(0),
iArraySize(1),
bCubeMap(false),
aMips(NULL)
{
}

CDXUATexture::TEX_DESC::~TEX_DESC()
{
	if (aMips)
	{
		a_free(aMips);
		aMips = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////

CDXUATexture::CDXUATexture() :
m_pDXTex(nullptr),
m_psrv(nullptr)
{
}

CDXUATexture::~CDXUATexture()
{

}

void CDXUATexture::Release()
{
	for (int i = 0; i < (int)m_uavs.size(); i++)
	{
		A3DINTERFACERELEASE(m_uavs[i]);
	}

	m_uavs.clear();

	A3DINTERFACERELEASE(m_psrv);
	A3DINTERFACERELEASE(m_pDXTex);
}

bool CDXUATexture::Init(int width, int height, DXGI_FORMAT fmt, bool bCubeMap, int iMipLevels)
{
	//	Create texture
	if (!Create(width, height, fmt, bCubeMap ? 0 : 1, iMipLevels))
	{
		a_OutputLog("CDXUATexture::Init, failed to call Create() !");
		return false;
	}

	return true;
}

bool CDXUATexture::InitArray(int width, int height, DXGI_FORMAT fmt, int iArraySize, int iMipLevels)
{
	ASSERT(iArraySize >= 1);

	//	Create texture
	if (!Create(width, height, fmt, iArraySize, iMipLevels))
	{
		a_OutputLog("CDXUATexture::InitArray, failed to call Create() !");
		return false;
	}

	return true;
}

bool CDXUATexture::Create(int width, int height, DXGI_FORMAT fmt, int iArraySize, int iMipLevels)
{
	//	Is cubemap ?
	bool bCubeMap = false;
	if (iArraySize <= 0)
	{
		bCubeMap = true;
		iArraySize = 6;
	}

	//	Fill texture desc
	D3D11_TEXTURE2D_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = iMipLevels;
	desc.ArraySize = iArraySize;
	desc.Format = fmt;
	desc.SampleDesc.Count = 1;
	desc.MiscFlags = bCubeMap ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	//desc.CPUAccessFlags = 0;

	//	Create texture
	HRESULT hr = DXRenderer::Instance()->GetDevice()->CreateTexture2D(&desc, NULL, &m_pDXTex);
	if (FAILED(hr))
	{
		a_OutputLog("CDXUATexture::Create, failed to call CreateTexture2D !");
		return false;
	}

	//	Fill texture description
	m_pDXTex->GetDesc(&desc);

	m_desc.width = desc.Width;
	m_desc.height = desc.Height;
	m_desc.fmt = desc.Format;
	m_desc.iMipLevels = desc.MipLevels;
	m_desc.iArraySize = desc.ArraySize;
	m_desc.bCubeMap = bCubeMap;
	m_desc.aMips = (MIP_INFO*)a_malloc(m_desc.iMipLevels * sizeof(MIP_INFO));

	//	Fill mipmap info
	for (int i = 0; i < m_desc.iMipLevels; i++)
	{
		MIP_INFO& info = m_desc.aMips[i];

		info.iWidth = m_desc.width >> i;
		info.iHeight = m_desc.height >> i;
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
		a_OutputLog("CDXUATexture::Create, failed to call CreateTextureView() !");
		return false;
	}

	return true;
}

bool CDXUATexture::CreateTextureView()
{
	//	create srv
	D3D11_SHADER_RESOURCE_VIEW_DESC vd;
	memset(&vd, 0, sizeof(vd));

	vd.Format = m_desc.fmt;

	if (m_desc.bCubeMap)
	{
		vd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		vd.TextureCube.MostDetailedMip = 0;
		vd.TextureCube.MipLevels = -1;// m_desc.iMipLevels;
	}
	else if (m_desc.iArraySize > 1)
	{
		vd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		vd.Texture2DArray.MostDetailedMip = 0;
		vd.Texture2DArray.ArraySize = m_desc.iArraySize;
		vd.Texture2DArray.FirstArraySlice = 0;
		vd.Texture2DArray.MipLevels = -1;// m_desc.iMipLevels;
	}
	else
	{
		vd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		vd.Texture2D.MostDetailedMip = 0;
		vd.Texture2D.MipLevels = -1;// m_desc.iMipLevels;
	}

	HRESULT hr = DXRenderer::Instance()->GetDevice()->CreateShaderResourceView(m_pDXTex, &vd, &m_psrv);
	if (FAILED(hr))
	{
		a_OutputLog("CDXUATexture::CreateTextureView, failed to call CreateShaderResourceView() !");
		return false;
	}

	//	create uav
	D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc;
	memset(&uav_desc, 0, sizeof(uav_desc));
	uav_desc.Format = m_desc.fmt;
	uav_desc.ViewDimension = (m_desc.iArraySize > 1) ? D3D11_UAV_DIMENSION_TEXTURE2DARRAY : D3D11_UAV_DIMENSION_TEXTURE2D;

	for (int i = 0; i < m_desc.iMipLevels; i++)
	{
		if (m_desc.iArraySize > 1)
		{
			uav_desc.Texture2DArray.MipSlice = i;
			uav_desc.Texture2DArray.FirstArraySlice = 0;
			uav_desc.Texture2DArray.ArraySize = m_desc.iArraySize;
		}
		else
		{
			uav_desc.Texture2D.MipSlice = i;
		}

		ID3D11UnorderedAccessView* pUAV = nullptr;
		hr = DXRenderer::Instance()->GetDevice()->CreateUnorderedAccessView(m_pDXTex, &uav_desc, &pUAV);
		if (FAILED(hr))
		{
			a_OutputLog("CDXUATexture::Init, failed to call CreateUnorderedAccessView()!");
			return false;
		}

		m_uavs.push_back(pUAV);
	}

	return true;
}

void CDXUATexture::CopyFrom(int iDstArrayIndex, CDXTexture* pSrc, int iSrcArrayIndex, int mip/* -1 */)
{
	ASSERT(m_pDXTex && pSrc->GetDXTex());

	const CDXTexture::TEX_DESC& src_desc = pSrc->GetDesc();

	//	Check if data format is match
	ASSERT(src_desc.iMipLevels == m_desc.iMipLevels);
	ASSERT(src_desc.fmt == m_desc.fmt);
	ASSERT(src_desc.width == m_desc.width && src_desc.height == m_desc.height);

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
		UINT riSrc = D3D11CalcSubresource(mip, iSrcArrayIndex, src_desc.iMipLevels);
		UINT riDst = D3D11CalcSubresource(mip, iDstArrayIndex, m_desc.iMipLevels);
		pDevContext->CopySubresourceRegion(m_pDXTex, riDst, 0, 0, 0, pSrc->GetDXTex(), riSrc, NULL);
	}
}

void CDXUATexture::CopyOut(int iDstArrayIndex, CDXTexture* pDst, int iSrcArrayIndex, int mip/* -1 */)
{
	ASSERT(m_pDXTex && pDst->GetDXTex());

	const CDXTexture::TEX_DESC& dst_desc = pDst->GetDesc();

	//	Check if data format is match
	ASSERT(dst_desc.iMipLevels == m_desc.iMipLevels);
	ASSERT(dst_desc.fmt == m_desc.fmt);
	ASSERT(dst_desc.width == m_desc.width && dst_desc.height == m_desc.height);

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
		UINT riDst = D3D11CalcSubresource(mip, iSrcArrayIndex, dst_desc.iMipLevels);
		UINT riSrc = D3D11CalcSubresource(mip, iDstArrayIndex, m_desc.iMipLevels);
		pDevContext->CopySubresourceRegion(pDst->GetDXTex(), riDst, 0, 0, 0, m_pDXTex, riSrc, NULL);
	}
}

void CDXUATexture::ApplySRVToPS(int iStartSlot)
{
	ASSERT(m_pDXTex && m_psrv);

	DXRenderer::Instance()->GetDeviceContext()->PSSetShaderResources(iStartSlot, 1, &m_psrv);
}

void CDXUATexture::ApplySRVToCS(int iStartSlot)
{
	ASSERT(m_pDXTex && m_psrv);

	DXRenderer::Instance()->GetDeviceContext()->CSSetShaderResources(iStartSlot, 1, &m_psrv);
}

void CDXUATexture::ApplyUAVToCS(int iStartSlot, int iSlotNum, int iStartMip)
{
	ASSERT(m_pDXTex && m_psrv);
	ASSERT(iStartMip + iSlotNum <= m_desc.iMipLevels);

	ID3D11UnorderedAccessView** ppUAVs = m_uavs.data() + iStartMip;
	DXRenderer::Instance()->GetDeviceContext()->CSSetUnorderedAccessViews(iStartSlot, iSlotNum, ppUAVs, NULL);
}

bool CDXUATexture::SaveToFile(const char* szFile, bool bDDS)
{
	if (!m_pDXTex)
	{
		ASSERT(m_pDXTex);
		return false;
	}

	HRESULT hr;
	auint16 szWFile[MAX_PATH];
	a_GB2312ToUTF16(szWFile, szFile, MAX_PATH);

	if (bDDS)
	{
		//	the file generated by SaveDDSTextureToFile have inverted R and B channel
	//	hr = DirectX::SaveDDSTextureToFile(DXRenderer::Instance()->GetDeviceContext(),
	//		m_pDXTex, (LPCWSTR)szWFile);
		hr = DirectX::SaveWICTextureToFile(DXRenderer::Instance()->GetDeviceContext(),
			m_pDXTex, GUID_ContainerFormatDds, (LPCWSTR)szWFile, &GUID_WICPixelFormat32bppBGRA);
	}
	else
	{
		DirectX::ScratchImage image;
		hr = DirectX::CaptureTexture(DXRenderer::Instance()->GetDevice(),
			DXRenderer::Instance()->GetDeviceContext(), m_pDXTex, image);
		if (FAILED(hr))
		{
			a_OutputLog("CDXUATexture::SaveToFile, failed capture texture!");
			return false;
		}

		hr = DirectX::SaveToTGAFile(*(image.GetImage(0, 0, 0)), (LPCWSTR)szWFile);
	}

	if (FAILED(hr))
	{
		a_OutputLog("CDXUATexture::SaveToFile, failed to save texture to file [%s]!", szFile);
		return false;
	}

	return true;
}

