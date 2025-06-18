//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#include "DXVertexBuffer.h"
#include "DXRenderer.h"
#include "ALog.h"
#include "A3DMacros.h"

///////////////////////////////////////////////////////////////////////////

CDXVertexBuffer::CDXVertexBuffer() :
m_pBuf(NULL),
m_iVertSize(0),
m_iNumVert(0)
{
}

CDXVertexBuffer::~CDXVertexBuffer()
{
}

bool CDXVertexBuffer::Init(int iVertSize, int iNumVert, auint32 uFlags, const void* pInitData)
{
	ASSERT(iVertSize && iNumVert);

	D3D11_BUFFER_DESC descBuf;
	memset(&descBuf, 0, sizeof(descBuf));

	if (uFlags & FLAG_DYNAMIC)
	{
		descBuf.Usage = D3D11_USAGE_DYNAMIC;
		descBuf.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		descBuf.Usage = D3D11_USAGE_DEFAULT;
		descBuf.CPUAccessFlags = 0;
	}

	descBuf.ByteWidth = iVertSize * iNumVert;
	descBuf.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//	descBuf.MiscFlags = 0;
	//	descBuf.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA srData;
	srData.pSysMem = pInitData;
	srData.SysMemPitch = 0;
	srData.SysMemSlicePitch = 0;

	HRESULT	result = DXRenderer::Instance()->GetDevice()->CreateBuffer(&descBuf, pInitData ? &srData : NULL, &m_pBuf);
	if (FAILED(result))
	{
		a_OutputLog("CDXVertexBuffer::Init, failed to call CreateBuffer() !");
		return false;
	}

	m_iVertSize = iVertSize;
	m_iNumVert = iNumVert;

	return true;
}

void CDXVertexBuffer::Release()
{
	A3DINTERFACERELEASE(m_pBuf);

	m_iVertSize = 0;
	m_iNumVert = 0;
}

void* CDXVertexBuffer::Lock()
{
	if (!m_pBuf)
	{
		ASSERT(m_pBuf);
		return NULL;
	}

	ID3D11DeviceContext* pDevContext = DXRenderer::Instance()->GetDeviceContext();
	ASSERT(pDevContext);

	D3D11_MAPPED_SUBRESOURCE MappedRes;
	HRESULT result = pDevContext->Map(m_pBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedRes);
	if (FAILED(result))
		return false;
	
	return MappedRes.pData;
}

void CDXVertexBuffer::Unlock()
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

bool CDXVertexBuffer::Apply()
{
	ASSERT(m_pBuf);

	ID3D11DeviceContext* pDevContext = DXRenderer::Instance()->GetDeviceContext();
	ASSERT(pDevContext);

	UINT uStride = m_iVertSize;
	UINT uOffset = 0;
	pDevContext->IASetVertexBuffers(0, 1, &m_pBuf, &uStride, &uOffset);

	return true;
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement of CDXIndexBuffer
//
///////////////////////////////////////////////////////////////////////////

CDXIndexBuffer::CDXIndexBuffer() :
m_pBuf(NULL),
m_iIndexSize(0),
m_iNumIndex(0),
m_Topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
{
}

CDXIndexBuffer::~CDXIndexBuffer()
{
}

bool CDXIndexBuffer::Init(int iIndexSize, int iNumIndex, auint32 uFlags, const void* pInitData,
	D3D_PRIMITIVE_TOPOLOGY topology)
{
	ASSERT(iIndexSize && iNumIndex);

	D3D11_BUFFER_DESC descBuf;
	memset(&descBuf, 0, sizeof(descBuf));

	if (uFlags & FLAG_DYNAMIC)
	{
		descBuf.Usage = D3D11_USAGE_DYNAMIC;
		descBuf.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		descBuf.Usage = D3D11_USAGE_DEFAULT;
		descBuf.CPUAccessFlags = 0;
	}

	descBuf.ByteWidth = iIndexSize * iNumIndex;
	descBuf.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//	descBuf.MiscFlags = 0;
	//	descBuf.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA srData;
	srData.pSysMem = pInitData;
	srData.SysMemPitch = 0;
	srData.SysMemSlicePitch = 0;

	HRESULT	result = DXRenderer::Instance()->GetDevice()->CreateBuffer(&descBuf, pInitData ? &srData : NULL, &m_pBuf);
	if (FAILED(result))
	{
		a_OutputLog("CDXIndexBuffer::Init, failed to call CreateBuffer() !");
		return false;
	}

	m_iIndexSize = iIndexSize;
	m_iNumIndex = iNumIndex;
	m_Topology = topology;

	return true;
}

void CDXIndexBuffer::Release()
{
	A3DINTERFACERELEASE(m_pBuf);

	m_iIndexSize = 0;
	m_iNumIndex = 0;
}

void* CDXIndexBuffer::Lock()
{
	if (!m_pBuf)
	{
		ASSERT(m_pBuf);
		return NULL;
	}

	ID3D11DeviceContext* pDevContext = DXRenderer::Instance()->GetDeviceContext();
	ASSERT(pDevContext);

	D3D11_MAPPED_SUBRESOURCE MappedRes;
	HRESULT result = pDevContext->Map(m_pBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedRes);
	if (FAILED(result))
		return false;

	return MappedRes.pData;
}

void CDXIndexBuffer::Unlock()
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

bool CDXIndexBuffer::Apply()
{
	ASSERT(m_pBuf);

	ID3D11DeviceContext* pDevContext = DXRenderer::Instance()->GetDeviceContext();
	ASSERT(pDevContext);

	if (m_iIndexSize == 4)
		pDevContext->IASetIndexBuffer(m_pBuf, DXGI_FORMAT_R32_UINT, 0);
	else if (m_iIndexSize == 2)
		pDevContext->IASetIndexBuffer(m_pBuf, DXGI_FORMAT_R16_UINT, 0);
	else
	{
		ASSERT(0);
		return false;
	}

	pDevContext->IASetPrimitiveTopology(m_Topology);

	return true;
}

