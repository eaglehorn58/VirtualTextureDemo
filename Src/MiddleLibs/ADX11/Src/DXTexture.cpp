//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com

#pragma warning(disable:4005)

#include "DXRenderer.h"
#include "DXTexture.h"
#include "DXRenderTarget.h"
#include "AFileWrapper.h"
#include "AXAssist.h"
#include "AFI.h"
#include "ALog.h"
#include "A3DMacros.h"
#include "DirectXTex.h"
#include "ScreenGrab.h"
#include "AMemory.h"
#include <wincodec.h>

///////////////////////////////////////////////////////////////////////////

CDXTexture::TEX_DESC::TEX_DESC() :
width(0),
height(0),
fmt(DXGI_FORMAT_UNKNOWN),
iMipLevels(0),
iArraySize(1),
bCubeMap(false),
aMips(NULL)
{
}

CDXTexture::TEX_DESC::~TEX_DESC()
{
	if (aMips)
	{
		a_free(aMips);
		aMips = NULL;
	}
}

CDXTexture::UPDATE_INFO::UPDATE_INFO() :
pPixels(NULL),
iArrayIdx(0),
iMipLevel(0)
{
}

///////////////////////////////////////////////////////////////////////////

auint32 CDXTexture::MakeID(const char* szFile)
{
	ASSERT(szFile && szFile[0]);
	return a_MakeIDFromLowString(szFile);
}

void CDXTexture::NormalizeFileName(const char* szFile, char* szOutFile)
{
	af_GetRelativePath(szFile, szOutFile);
	af_NormalizeFileName(szOutFile);
}

void CDXTexture::NormalizeFileName(const char* szFile, AString& strOutFile)
{
	char szOut[MAX_PATH];
	CDXTexture::NormalizeFileName(szFile, szOut);
	strOutFile = szOut;
}

void CDXTexture::CreateNoiseMap(const wchar_t* szFile)
{
	const int width = 256;
	abyte buf[width * width];
	for (int i = 0; i < width * width; i++)
	{
		buf[i] = a_Random(0, 255);
	}

	DirectX::Image image;
	image.format = DXGI_FORMAT_R8_UNORM;
	image.width = width;
	image.height = width;
	image.rowPitch = width;
	image.slicePitch = width * width;
	image.pixels = buf;
	DirectX::SaveToDDSFile(image, 0, szFile);
}

CDXTexture::CDXTexture() :
m_id(0),
m_pDXTex(NULL),
m_psrv(NULL),
m_bFromRT(false),
m_uCPUAccess(0)
{
}

CDXTexture::~CDXTexture()
{

}

void CDXTexture::Release()
{
	A3DINTERFACERELEASE(m_psrv);

	if (!m_bFromRT)
	{
		A3DINTERFACERELEASE(m_pDXTex);
	}
}

bool CDXTexture::Init(const char* szID, int width, int height, DXGI_FORMAT fmt, bool bCubeMap,
	int iMipLevels, auint32 uCPUAccess)
{
	if (!szID || !szID[0])
		return false;

	//	dynamic texture can only have 1 level mipmap
	if ((uCPUAccess & CPU_DYNAMIC) && iMipLevels != 1)
	{
		a_OutputLog("CDXTexture::Init, dynamic texture can only have 1 level mipmap [%s] !", szID);
		return false;
	}

	m_strFile = szID;
	m_id = MakeID(szID);
	m_uCPUAccess = uCPUAccess;

	//	Create texture
	if (!Create(width, height, fmt, bCubeMap ? 0 : 1, iMipLevels))
	{
		a_OutputLog("CDXTexture::Init, failed to call Create() [%s] !", szID);
		return false;
	}

	return true;
}

bool CDXTexture::InitArray(const char* szID, int width, int height, DXGI_FORMAT fmt, int iArraySize, int iMipLevels)
{
	if (!szID || !szID[0])
		return false;

	ASSERT(iArraySize >= 1);

	m_strFile = szID;
	m_id = MakeID(szID);

	//	Create texture
	if (!Create(width, height, fmt, iArraySize, iMipLevels))
	{
		a_OutputLog("CDXTexture::InitArray, failed to call Create() !");
		return false;
	}

	return true;
}

bool CDXTexture::InitFromFile(const char* szFile, int iMipLevels)
{
	if (!szFile || !szFile[0])
		return false;

	NormalizeFileName(szFile, m_strFile);
	m_id = MakeID(m_strFile);

	if (af_CheckFileExt(szFile, ".dds"))
	{
		//	Load from DDS file
		if (!LoadFromDDSFile(szFile, iMipLevels))
		{
			a_OutputLog("CDXTexture::InitFromFile, failed to call LoadFromDDSFile() [%s] !", szFile);
			return false;
		}
	}
	else
	{
		ASSERT(0 && "un-supported file type");
		return false;
	}

	return true;
}

bool CDXTexture::LoadFromDDSFile(const char* szFile, int iMipLevels)
{
	auint16 szWFile[MAX_PATH];
	a_GB2312ToUTF16(szWFile, szFile, MAX_PATH);

	//	Load dds file
	DirectX::TexMetadata metadata;
	DirectX::ScratchImage image;
	HRESULT hr = DirectX::LoadFromDDSFile((LPCWSTR)szWFile, DirectX::DDS_FLAGS_FORCE_RGB, &metadata, image);
	if (FAILED(hr))
	{
		a_OutputLog("CDXTexture::LoadFromDDSFile, failed to call DirectX::LoadFromDDSFile() [%s] !", szFile);
		return false;
	}

	int iRealMipLevels = (int)metadata.mipLevels;
	if (iMipLevels > 0)
		iRealMipLevels = a_Min(iMipLevels, (int)metadata.mipLevels);
	
	int iArraySize = (int)metadata.arraySize;
	if (metadata.IsCubemap())
	{
		ASSERT(metadata.arraySize == 6);
		iArraySize = 0;
	}

	//	Create texture
	if (!Create(metadata.width, metadata.height, metadata.format, iArraySize, iRealMipLevels))
	{
		a_OutputLog("CDXTexture::LoadFromDDSFile, failed to call Create() [%s] !", szFile);
		return false;
	}

	//	Fill image content
	int iFaceNum = metadata.IsCubemap() ? 6 : 1;
	for (int nFace = 0; nFace < iFaceNum; ++nFace)
	{
		for (int nMipmap = 0; nMipmap < iRealMipLevels; ++nMipmap)
		{
			const DirectX::Image* pImage = image.GetImage(nMipmap, nFace, 0);

			ASSERT((int)pImage->rowPitch == m_desc.aMips[nMipmap].iRowPitch);
			ASSERT((int)pImage->slicePitch == m_desc.aMips[nMipmap].iSlicePitch);

			auint32 sr_idx = D3D11CalcSubresource(nMipmap, nFace, iRealMipLevels);
			DXRenderer::Instance()->GetDeviceContext()->UpdateSubresource(m_pDXTex, sr_idx, NULL, pImage->pixels, pImage->rowPitch, 0);
		}
	}

	return true;
}

bool CDXTexture::Create(int width, int height, DXGI_FORMAT fmt, int iArraySize, int iMipLevels)
{
	//	Is cubemap ?
	bool bCubeMap = false;
	if (iArraySize <= 0)
	{
		bCubeMap = true;
		iArraySize = 6;
	}

	//	Note: D3D11_USAGE_DYNAMIC texture have many limitations, for example they can only
	//	have 1 array size and 1 level mipmap.
	if (m_uCPUAccess & CPU_DYNAMIC)
	{
		if (iMipLevels != 1 || iArraySize != 1)
		{
			a_OutputLog("CDXTexture::Create, dynamic texture can only have 1 array size and 1 level mipmap !");
			return false;
		}
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

	//	Note: D3D11_USAGE_STAGING texture can be only used as resource like a relay station, 
	//	and can not be bound to the rendering pipeline
	//	see detail: http://www.programmersought.com/article/3963513072/
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	if (m_uCPUAccess & CPU_DYNAMIC)
	{
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags |= D3D11_CPU_ACCESS_WRITE;
	}
	else if (m_uCPUAccess)
	{
		desc.Usage = D3D11_USAGE_STAGING;
		desc.BindFlags = 0;

		if (m_uCPUAccess & CPU_READ) desc.CPUAccessFlags |= D3D11_CPU_ACCESS_READ;
		if (m_uCPUAccess & CPU_WRITE) desc.CPUAccessFlags |= D3D11_CPU_ACCESS_WRITE;
	}

	//	Create texture
	HRESULT hr = DXRenderer::Instance()->GetDevice()->CreateTexture2D(&desc, NULL, &m_pDXTex);
	if (FAILED(hr))
	{
		a_OutputLog("CDXTexture::Create, failed to call CreateTexture2D !");
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
		a_OutputLog("CDXTexture::Create, failed to call CreateTextureView() !");
		return false;
	}

	return true;
}

bool CDXTexture::CreateTextureView()
{
	//	Staging texture can't create shader resource view, so return directly
	if ((m_uCPUAccess & CPU_READ) || (m_uCPUAccess & CPU_WRITE))
		return true;

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
		a_OutputLog("CDXTexture::CreateTextureView, failed to call CreateShaderResourceView() !");
		return false;
	}

	return true;
}

bool CDXTexture::InitFromRT(const char* szID, CDXRenderTarget* pRT, int iRTIndex)
{
	if (!szID || !szID[0])
		return false;

	m_strFile = szID;
	m_id = MakeID(szID);
	m_bFromRT = true;

	const CDXRenderTarget::COLOR_RT& rt = pRT->GetColorRT(iRTIndex);
	if (!rt.pRT || !rt.pRTView)
	{
		ASSERT(rt.pRT && rt.pRTView);
		return false;
	}

	m_pDXTex = rt.pRT;

	//	Build texture description
	m_desc.width = pRT->GetWidth();
	m_desc.height = pRT->GetHeight();
	m_desc.fmt = rt.fmt;
	m_desc.iMipLevels = 1;
	m_desc.iArraySize = 1;
	m_desc.bCubeMap = false;
	m_desc.aMips = (MIP_INFO*)a_malloc(sizeof(MIP_INFO));

	size_t row_pitch = 0, slice_pitch = 0;
	DirectX::ComputePitch(m_desc.fmt, m_desc.width, m_desc.height, row_pitch, slice_pitch);
	m_desc.aMips[0].iRowPitch = (int)row_pitch;
	m_desc.aMips[0].iSlicePitch = (int)slice_pitch;

	//	Create texture view
	if (!CreateTextureView())
	{
		a_OutputLog("CDXTexture::InitFromRT, failed to call CreateTextureView() !");
		return false;
	}

	return true;
}

bool CDXTexture::Init(const char* szID, A3DCOLOR color)
{
	if (!szID || !szID[0])
		return false;

	m_strFile = szID;
	m_id = MakeID(szID);

	//	Create texture
	const int width = 4;
	if (!Create(width, width, DXGI_FORMAT_R8G8B8A8_UNORM, 1, 1))
	{
		a_OutputLog("CDXTexture::Init, init as pure color texture. failed to call Create() !");
		return false;
	}

	//	fill color
	const int iMaxBufSize = 128;
	abyte buf[iMaxBufSize];
	ASSERT(m_desc.aMips[0].iSlicePitch <= iMaxBufSize);

	for (int r = 0; r < width; r++)
	{
		auint32* pLine = (auint32*)&buf[r * m_desc.aMips[0].iRowPitch];
		for (int c = 0; c < width; c++)
			pLine[c] = color;
	}

	UPDATE_INFO ui;
	ui.iArrayIdx = 0;
	ui.iMipLevel = 0;
	ui.pPixels = buf;
	Update(&ui, 1);

	return true;
}

bool CDXTexture::Update(UPDATE_INFO* aInfos, int iNumInfo)
{
	if (!m_pDXTex)
	{
		ASSERT(m_pDXTex);
		return false;
	}

	//	according to UpdateSubresource's doc, dynamic texture cann't use UpdateSubresource
	//	to update its content, use Lock() instead.
	if (m_uCPUAccess & CPU_DYNAMIC)
	{
		ASSERT(0 && "Dynamic texture can't be updated, use Lock instead.");
		return false;
	}

	for (int i = 0; i < iNumInfo; i++)
	{
		const UPDATE_INFO& info = aInfos[i];

		ASSERT(info.iMipLevel >= 0 && info.iMipLevel < m_desc.iMipLevels);
		int row_pitch = m_desc.aMips[info.iMipLevel].iRowPitch;

		auint32 sr_idx = D3D11CalcSubresource(info.iMipLevel, info.iArrayIdx, m_desc.iMipLevels);
		DXRenderer::Instance()->GetDeviceContext()->UpdateSubresource(m_pDXTex, sr_idx, NULL, info.pPixels, row_pitch, 0);
	}
	
	return true;
}

void CDXTexture::CopyFrom(int iDstArrayIndex, CDXTexture* pSrc, int iSrcArrayIndex, int mip/* -1 */)
{
	ASSERT(m_pDXTex && pSrc->m_pDXTex);

	const CDXTexture::TEX_DESC& src_desc = pSrc->GetDesc();

	//	Check if data format is match
	ASSERT(src_desc.iMipLevels == m_desc.iMipLevels);
	ASSERT(src_desc.fmt == m_desc.fmt);
	ASSERT(src_desc.width == m_desc.width && src_desc.height == m_desc.height);

	ID3D11DeviceContext* pDevContext = DXRenderer::Instance()->GetDeviceContext();

	if (mip < 0)
	{
		//	copy all mips
		//for (int i = 0; i < src_desc.iMipLevels; i++)
		//{
		//	UINT riSrc = D3D11CalcSubresource(i, iSrcArrayIndex, src_desc.iMipLevels);
		//	UINT riDst = D3D11CalcSubresource(i, iDstArrayIndex, m_desc.iMipLevels);
		//	pDevContext->CopySubresourceRegion(m_pDXTex, riDst, 0, 0, 0, pSrc->m_pDXTex, riSrc, NULL);
		//}
		pDevContext->CopyResource(m_pDXTex, pSrc->m_pDXTex);
	}
	else
	{
		//	only copy specified mip level
		ASSERT(mip < m_desc.iMipLevels && mip < src_desc.iMipLevels);
		UINT riSrc = D3D11CalcSubresource(mip, iSrcArrayIndex, src_desc.iMipLevels);
		UINT riDst = D3D11CalcSubresource(mip, iDstArrayIndex, m_desc.iMipLevels);
		pDevContext->CopySubresourceRegion(m_pDXTex, riDst, 0, 0, 0, pSrc->m_pDXTex, riSrc, NULL);
	}
}

void CDXTexture::ApplyToPS(int iStartSlot)
{
	ASSERT(!(m_uCPUAccess & CPU_READ) && !(m_uCPUAccess & CPU_WRITE));
	ASSERT(m_pDXTex && m_psrv);

	DXRenderer::Instance()->GetDeviceContext()->PSSetShaderResources(iStartSlot, 1, &m_psrv);
}

void CDXTexture::ApplyToCS(int iStartSlot)
{
	ASSERT(!(m_uCPUAccess & CPU_READ) && !(m_uCPUAccess & CPU_WRITE));
	ASSERT(m_pDXTex && m_psrv);

	DXRenderer::Instance()->GetDeviceContext()->CSSetShaderResources(iStartSlot, 1, &m_psrv);
}

bool CDXTexture::Lock(int iArrayIndex, int iMipLevel, LOCK_INFO& info)
{
	if (!m_pDXTex)
	{
		ASSERT(m_pDXTex);
		return false;
	}

	if (!m_uCPUAccess)
	{
		ASSERT(m_uCPUAccess);
		return false;
	}

	D3D11_MAP d3d_map;
	if (m_uCPUAccess & CPU_DYNAMIC)
		d3d_map = D3D11_MAP_WRITE_DISCARD;
	else if ((m_uCPUAccess & CPU_READ) && (m_uCPUAccess & CPU_WRITE))
		d3d_map = D3D11_MAP_READ_WRITE;
	else if (m_uCPUAccess & CPU_READ)
		d3d_map = D3D11_MAP_READ;
	else
		d3d_map = D3D11_MAP_WRITE;

	UINT sr_idx = D3D11CalcSubresource(iMipLevel, iArrayIndex, m_desc.iMipLevels);
	D3D11_MAPPED_SUBRESOURCE mapped;
	HRESULT hr = DXRenderer::Instance()->GetDeviceContext()->Map(m_pDXTex, sr_idx, d3d_map, 0, &mapped);
	if (FAILED(hr))
	{
		a_OutputLog("CDXTexture::Map, failed to map texture !");
		return false;
	}

	info.pData = mapped.pData;
	info.uRowPitch = mapped.RowPitch;

	return true;
}

void CDXTexture::Unlock(int iArrayIndex, int iMipLevel)
{
	if (!m_pDXTex)
	{
		ASSERT(m_pDXTex);
		return;
	}

	if (!m_uCPUAccess)
	{
		ASSERT(m_uCPUAccess);
		return;
	}

	UINT sr_idx = D3D11CalcSubresource(iMipLevel, iArrayIndex, m_desc.iMipLevels);
	DXRenderer::Instance()->GetDeviceContext()->Unmap(m_pDXTex, sr_idx);
}

bool CDXTexture::SaveToFile(const char* szFile, bool bDDS)
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
			a_OutputLog("CDXTexture::SaveToFile, failed capture texture!");
			return false;
		}

		hr = DirectX::SaveToTGAFile(*(image.GetImage(0, 0, 0)), (LPCWSTR)szWFile);
	}

	if (FAILED(hr))
	{
		a_OutputLog("CDXTexture::SaveToFile, failed to save texture to file [%s]!", szFile);
		return false;
	}

	return true;
}

