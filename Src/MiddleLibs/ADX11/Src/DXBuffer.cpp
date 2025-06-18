//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#include "DXBuffer.h"
#include "DXRenderer.h"
#include "ALog.h"
#include "A3DMacros.h"

///////////////////////////////////////////////////////////////////////////

CDXBuffer::CDXBuffer() :
m_pBuf(nullptr),
m_psrv(nullptr),
m_iBufType(DX::BUFFER_UNKNOWN),
m_iElemNum(0),
m_iElemSize(0),
m_uCPUAccess(CPU_NO_ACCESS)
{
}

CDXBuffer::~CDXBuffer()
{
}

bool CDXBuffer::Init(int iElemCnt, int iElemSize, const void* pInitData, auint32 uCPUAccess)
{
	m_iBufType = DX::BUFFER_STRUCTURED;
	m_uCPUAccess = uCPUAccess;

	if (!Create(iElemCnt, iElemSize, pInitData))
		return false;
	
	return true;
}

bool CDXBuffer::InitRaw(int iElemCnt, const void* pInitData, auint32 uCPUAccess)
{
	m_iBufType = DX::BUFFER_RAW;
	m_uCPUAccess = uCPUAccess;

	if (!Create(iElemCnt, sizeof (auint32), pInitData))
		return false;

	return true;
}

bool CDXBuffer::Create(int iElemCnt, int iElemSize, const void* pInitData)
{
	m_iElemNum = iElemCnt;
	m_iElemSize = iElemSize;

	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));

	desc.ByteWidth = iElemSize * iElemCnt;
	desc.StructureByteStride = iElemSize;

	if (m_iBufType == DX::BUFFER_STRUCTURED)
	{
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	}
	else if (m_iBufType == DX::BUFFER_RAW)
	{
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
	}

	if (m_uCPUAccess)
	{
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
			desc.MiscFlags = 0;		//	disable SRV view

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

	D3D11_SUBRESOURCE_DATA srData;
	srData.pSysMem = pInitData;
	srData.SysMemPitch = 0;
	srData.SysMemSlicePitch = 0;

	HRESULT	result = DXRenderer::Instance()->GetDevice()->CreateBuffer(&desc, pInitData ? &srData : NULL, &m_pBuf);
	if (FAILED(result))
	{
		a_OutputLog("CDXBuffer::Create, failed to call CreateBuffer()!");
		return false;
	}

	//	Create shader resource view
	if (!CreateSRV())
	{
		a_OutputLog("CDXBuffer::Create, failed to call CreateSRV()!");
		return false;
	}

	return true;
}

bool CDXBuffer::CreateSRV()
{
	//	Staging texture can't create shader resource view, so return directly
	if ((m_uCPUAccess & CPU_READ) || (m_uCPUAccess & CPU_WRITE))
		return true;

	//	Create shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
	memset(&srv_desc, 0, sizeof(srv_desc));

	if (m_iBufType == DX::BUFFER_STRUCTURED)
	{
		srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srv_desc.Format = DXGI_FORMAT_UNKNOWN;
		srv_desc.Buffer.NumElements = m_iElemNum;
	}
	else if (m_iBufType == DX::BUFFER_RAW)
	{
		srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
		srv_desc.Format = DXGI_FORMAT_R32_TYPELESS;
		srv_desc.BufferEx.NumElements = m_iElemNum;
		srv_desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
	}

	HRESULT hr = DXRenderer::Instance()->GetDevice()->CreateShaderResourceView(m_pBuf, &srv_desc, &m_psrv);
	if (FAILED(hr))
	{
		a_OutputLog("CDXBuffer::CreateSRV, failed to call CreateShaderResourceView()!");
		return false;
	}

	return true;
}

void CDXBuffer::Release()
{
	A3DINTERFACERELEASE(m_psrv);
	A3DINTERFACERELEASE(m_pBuf);
}

bool CDXBuffer::Update(const void* pSrc)
{
	if (!m_pBuf || !pSrc)
	{
		ASSERT(m_pBuf && pSrc);
		return false;
	}

	//	according to UpdateSubresource's doc, dynamic texture cann't use UpdateSubresource
	//	to update its content, use Lock() instead.
	if (m_uCPUAccess & CPU_DYNAMIC)
	{
		ASSERT(0 && "Dynamic texture can't be updated, use Lock instead.");
		return false;
	}

	int iRowPitch = m_iElemSize * m_iElemNum;
	DXRenderer::Instance()->GetDeviceContext()->UpdateSubresource(m_pBuf, 0, NULL, pSrc, iRowPitch, 0);

	return true;
}

void* CDXBuffer::Lock()
{
	if (!m_pBuf)
	{
		ASSERT(m_pBuf);
		return nullptr;
	}

	if (!m_uCPUAccess)
	{
		ASSERT(m_uCPUAccess);
		return nullptr;
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

	ID3D11DeviceContext* pDevContext = DXRenderer::Instance()->GetDeviceContext();
	ASSERT(pDevContext);

	D3D11_MAPPED_SUBRESOURCE MappedRes;
	HRESULT result = pDevContext->Map(m_pBuf, 0, d3d_map, 0, &MappedRes);
	if (FAILED(result))
		return false;
	
	return MappedRes.pData;
}

void CDXBuffer::Unlock()
{
	if (!m_pBuf)
	{
		ASSERT(m_pBuf);
		return;
	}

	ID3D11DeviceContext* pDevContext = DXRenderer::Instance()->GetDeviceContext();
	ASSERT(pDevContext);

	pDevContext->Unmap(m_pBuf, 0);
}

void CDXBuffer::ApplyToPS(int iSlot)
{
	ASSERT(m_pBuf && m_psrv);
	ASSERT(!(m_uCPUAccess & CPU_READ) && !(m_uCPUAccess & CPU_WRITE));

	DXRenderer::Instance()->GetDeviceContext()->PSSetShaderResources(iSlot, 1, &m_psrv);
}

void CDXBuffer::ApplyToCS(int iSlot)
{
	ASSERT(m_pBuf && m_psrv);
	ASSERT(!(m_uCPUAccess & CPU_READ) && !(m_uCPUAccess & CPU_WRITE));

	DXRenderer::Instance()->GetDeviceContext()->CSSetShaderResources(iSlot, 1, &m_psrv);
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement of CDXUABuffer
//
///////////////////////////////////////////////////////////////////////////

CDXUABuffer::CDXUABuffer() :
m_pBuf(nullptr),
m_psrv(nullptr),
m_puav(nullptr),
m_iBufType(DX::BUFFER_UNKNOWN),
m_iElemNum(0),
m_iElemSize(0)
{
}

CDXUABuffer::~CDXUABuffer()
{
}

bool CDXUABuffer::Init(int iElemCnt, int iElemSize)
{
	m_iBufType = DX::BUFFER_STRUCTURED;

	if (!Create(iElemCnt, iElemSize))
		return false;

	return true;
}

bool CDXUABuffer::InitRaw(int iElemCnt)
{
	m_iBufType = DX::BUFFER_RAW;

	if (!Create(iElemCnt, sizeof(auint32)))
		return false;

	return true;
}

bool CDXUABuffer::Create(int iElemCnt, int iElemSize)
{
	m_iElemNum = iElemCnt;
	m_iElemSize = iElemSize;

	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));

	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = iElemSize * iElemCnt;
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;;
	desc.StructureByteStride = iElemSize;
//	desc.CPUAccessFlags == 0;

	if (m_iBufType == DX::BUFFER_STRUCTURED)
	{
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	}
	else if (m_iBufType == DX::BUFFER_RAW)
	{
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
	}

	HRESULT	hr = DXRenderer::Instance()->GetDevice()->CreateBuffer(&desc, nullptr, &m_pBuf);
	if (FAILED(hr))
	{
		a_OutputLog("CDXUABuffer::Create, failed to call CreateBuffer() !");
		return false;
	}

	//	create shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
	memset(&srv_desc, 0, sizeof(srv_desc));

	if (m_iBufType == DX::BUFFER_STRUCTURED)
	{
		srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srv_desc.Format = DXGI_FORMAT_UNKNOWN;
		srv_desc.Buffer.NumElements = m_iElemNum;
	}
	else if (m_iBufType == DX::BUFFER_RAW)
	{
		srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
		srv_desc.Format = DXGI_FORMAT_R32_TYPELESS;
		srv_desc.BufferEx.NumElements = m_iElemNum;
		srv_desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
	}

	hr = DXRenderer::Instance()->GetDevice()->CreateShaderResourceView(m_pBuf, &srv_desc, &m_psrv);
	if (FAILED(hr))
	{
		a_OutputLog("CDXUABuffer::Create, failed to call CreateShaderResourceView()!");
		return false;
	}

	//	create unordered access view
	D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc;
	memset(&uav_desc, 0, sizeof(uav_desc));
	uav_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uav_desc.Buffer.NumElements = m_iElemNum;

	if (m_iBufType == DX::BUFFER_STRUCTURED)
	{
		uav_desc.Format = DXGI_FORMAT_UNKNOWN;
	}
	else if (m_iBufType == DX::BUFFER_RAW)
	{
		uav_desc.Format = DXGI_FORMAT_R32_TYPELESS;
		uav_desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
	}

	hr = DXRenderer::Instance()->GetDevice()->CreateUnorderedAccessView(m_pBuf, &uav_desc, &m_puav);
	if (FAILED(hr))
	{
		a_OutputLog("CDXUABuffer::Create, failed to call CreateUnorderedAccessView()!");
		return false;
	}

	return true;
}

void CDXUABuffer::Release()
{
	A3DINTERFACERELEASE(m_psrv);
	A3DINTERFACERELEASE(m_puav);
	A3DINTERFACERELEASE(m_pBuf);
}

bool CDXUABuffer::CopyFrom(CDXBuffer* pSrcBuf)
{
	if (!m_pBuf || !pSrcBuf)
		return false;

	if (m_iElemNum != pSrcBuf->GetElemNum() || m_iElemSize != pSrcBuf->GetElemSize())
	{
		ASSERT(0 && "buffer not match!");
		return false;
	}

	ID3D11DeviceContext* pDevContext = DXRenderer::Instance()->GetDeviceContext();
	pDevContext->CopyResource(m_pBuf, pSrcBuf->GetBuf());

	return true;
}

bool CDXUABuffer::CopyFromRegion(int iDstOffset, CDXBuffer* pSrcBuf, int iSrcOffset, int iBytes)
{
	if (!m_pBuf || !pSrcBuf)
		return false;

	int iDstLength = m_iElemNum * m_iElemSize;
	int iSrcLength = pSrcBuf->GetElemNum() * pSrcBuf->GetElemSize();
	ASSERT(iDstOffset >= 0 && iDstOffset + iBytes <= iDstLength);
	ASSERT(iSrcOffset >= 0 && iSrcOffset + iBytes <= iSrcLength);

	D3D11_BOX box;
	box.left = iSrcOffset;
	box.right = iSrcOffset + iBytes;
	box.top = 0;
	box.bottom = 1;
	box.front = 0;
	box.back = 1;

	ID3D11DeviceContext* pDevContext = DXRenderer::Instance()->GetDeviceContext();
	pDevContext->CopySubresourceRegion(m_pBuf, 0, iDstOffset, 0, 0, pSrcBuf->GetBuf(), 0, &box);

	return true;
}

bool CDXUABuffer::CopyOut(CDXBuffer* pDstBuf)
{
	if (!m_pBuf || !pDstBuf)
		return false;

	if (m_iElemNum != pDstBuf->GetElemNum() || m_iElemSize != pDstBuf->GetElemSize())
	{
		ASSERT(0 && "buffer not match!");
		return false;
	}

	ID3D11DeviceContext* pDevContext = DXRenderer::Instance()->GetDeviceContext();
	pDevContext->CopyResource(pDstBuf->GetBuf(), m_pBuf);

	return true;
}

bool CDXUABuffer::CopyOutRegion(CDXBuffer* pDstBuf, int iDstOffset, int iSrcOffset, int iBytes)
{
	if (!m_pBuf || !pDstBuf)
		return false;

	int iDstLength = pDstBuf->GetElemNum() * pDstBuf->GetElemSize();
	int iSrcLength = m_iElemNum * m_iElemSize;
	ASSERT(iDstOffset >= 0 && iDstOffset + iBytes <= iDstLength);
	ASSERT(iSrcOffset >= 0 && iSrcOffset + iBytes <= iSrcLength);

	D3D11_BOX box;
	box.left = iSrcOffset;
	box.right = iSrcOffset + iBytes;
	box.top = 0;
	box.bottom = 1;
	box.front = 0;
	box.back = 1;

	ID3D11DeviceContext* pDevContext = DXRenderer::Instance()->GetDeviceContext();
	pDevContext->CopySubresourceRegion(pDstBuf->GetBuf(), 0, iDstOffset, 0, 0, m_pBuf, 0, &box);

	return true;
}

void CDXUABuffer::ApplySRVToPS(int iSlot)
{
	ASSERT(m_pBuf && m_psrv);

	DXRenderer::Instance()->GetDeviceContext()->PSSetShaderResources(iSlot, 1, &m_psrv);
}

void CDXUABuffer::ApplySRVToCS(int iSlot)
{
	ASSERT(m_pBuf && m_psrv);

	DXRenderer::Instance()->GetDeviceContext()->CSSetShaderResources(iSlot, 1, &m_psrv);
}

void CDXUABuffer::ApplyUAVToCS(int iSlot)
{
	ASSERT(m_pBuf && m_puav);

	ID3D11DeviceContext* pDevContext = DXRenderer::Instance()->GetDeviceContext();
	pDevContext->CSSetUnorderedAccessViews(iSlot, 1, &m_puav, nullptr);
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement of CDXUABufferSuit
//
///////////////////////////////////////////////////////////////////////////

CDXUABufferSuit::CDXUABufferSuit() :
m_pUAVBuf(nullptr),
m_pStageBuf(nullptr),
m_pMemBuf(nullptr),
m_iBufType(DX::BUFFER_UNKNOWN),
m_iElemNum(0),
m_iElemSize(0)
{
}

CDXUABufferSuit::~CDXUABufferSuit()
{
}

bool CDXUABufferSuit::Init(int iElemCnt, int iElemSize)
{
	m_iBufType = DX::BUFFER_STRUCTURED;

	if (!Create(iElemCnt, iElemSize))
		return false;

	return true;
}

bool CDXUABufferSuit::InitRaw(int iElemCnt)
{
	m_iBufType = DX::BUFFER_RAW;

	if (!Create(iElemCnt, sizeof(auint32)))
		return false;

	return true;
}

bool CDXUABufferSuit::Create(int iElemCnt, int iElemSize)
{
	m_iElemNum = iElemCnt;
	m_iElemSize = iElemSize;

	m_pUAVBuf = new CDXUABuffer;
	m_pStageBuf = new CDXBuffer;

	if (m_iBufType == DX::BUFFER_STRUCTURED)
	{
		if (!m_pUAVBuf->Init(iElemCnt, iElemSize))
			return false;

		if (!m_pStageBuf->Init(iElemCnt, iElemSize, nullptr, DX::CPU_WRITE | DX::CPU_READ))
			return false;
	}
	else if (m_iBufType = DX::BUFFER_RAW)
	{
		if (!m_pUAVBuf->InitRaw(iElemCnt))
			return false;

		if (!m_pStageBuf->InitRaw(iElemCnt, nullptr, DX::CPU_WRITE | DX::CPU_READ))
			return false;
	}
	else
	{
		return false;
	}

	m_pMemBuf = new abyte[iElemCnt * iElemSize];
	if (!m_pMemBuf)
		return false;

	return true;
}

void CDXUABufferSuit::Release()
{
	if (m_pMemBuf)
	{
		delete[] m_pMemBuf;
		m_pMemBuf = nullptr;
	}

	A3DRELEASE(m_pUAVBuf);
	A3DRELEASE(m_pStageBuf);
}

void CDXUABufferSuit::Update(int iStart, int iBytes)
{
	ASSERT(m_pUAVBuf && m_pStageBuf);

	if (iStart < 0)
	{
		//	TODO: lock buffer only with cpu_write flag
		void* pData = m_pStageBuf->Lock();
		memcpy(pData, m_pMemBuf, m_iElemNum * m_iElemSize);
		m_pStageBuf->Unlock();

		m_pUAVBuf->CopyFrom(m_pStageBuf);
	}
	else
	{
		ASSERT(iStart >= 0 && iStart + iBytes <= m_iElemNum * m_iElemSize);

		//	TODO: lock buffer only with cpu_write flag
		abyte* pData = (abyte*)m_pStageBuf->Lock();
		memcpy(pData + iStart, m_pMemBuf + iStart, iBytes);
		m_pStageBuf->Unlock();

		m_pUAVBuf->CopyFromRegion(iStart, m_pStageBuf, iStart, iBytes);
	}
}

void CDXUABufferSuit::Extract(int iStart, int iBytes)
{
	ASSERT(m_pUAVBuf && m_pStageBuf);

	if (iStart < 0)
	{
		m_pUAVBuf->CopyOut(m_pStageBuf);

		//	TODO: lock buffer only with cpu_read flag
		void* pData = m_pStageBuf->Lock();
		memcpy(m_pMemBuf, pData, m_iElemNum * m_iElemSize);
		m_pStageBuf->Unlock();
	}
	else
	{
		ASSERT(iStart >= 0 && iStart + iBytes <= m_iElemNum * m_iElemSize);

		m_pUAVBuf->CopyOutRegion(m_pStageBuf, iStart, iStart, iBytes);

		//	TODO: lock buffer only with cpu_read flag
		abyte* pData = (abyte*)m_pStageBuf->Lock();
		memcpy(m_pMemBuf + iStart, pData + iStart, iBytes);
		m_pStageBuf->Unlock();
	}
}

void CDXUABufferSuit::ApplySRVToPS(int iSlot)
{
	m_pUAVBuf->ApplySRVToPS(iSlot);
}

void CDXUABufferSuit::ApplySRVToCS(int iSlot)
{
	m_pUAVBuf->ApplySRVToPS(iSlot);
}

void CDXUABufferSuit::ApplyUAVToCS(int iSlot)
{
	m_pUAVBuf->ApplyUAVToCS(iSlot);
}

