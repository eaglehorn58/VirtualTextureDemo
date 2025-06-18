//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#include "Global.h"
#include "VTextureSystem.h"
#include "VTexturePageRequest.h"
#include "DirectXTex.h"
#include "A3DMacros.h"
#include "DXBuffer.h"

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//	Implement of CVTexturePageRequest
///////////////////////////////////////////////////////////////////////////

CVTexturePageRequest::CVTexturePageRequest() :
m_pVTSys(nullptr),
m_pUABuf(nullptr),
m_pStageBuf(nullptr),
m_pTempIDBuf(nullptr),
m_iPageBufWid(0),
m_iPageBufHei(0)
{
}

CVTexturePageRequest::~CVTexturePageRequest()
{
}

bool CVTexturePageRequest::Init(CVTextureSystem* pVTSys, int scn_width, int scn_height)
{
	m_pVTSys = pVTSys;

	if (scn_width && scn_height)
	{
		if (!ResizeBuffers(scn_width, scn_height))
		{
			glb_OutputLog("CVTexturePageRequest::Init, failed to call Resize() !");
			return false;
		}
	}

	return true;
}

void CVTexturePageRequest::Release()
{
	ReleaseBuffers();
}

void CVTexturePageRequest::ReleaseBuffers()
{
	if (m_pTempIDBuf)
	{
		delete[] m_pTempIDBuf;
		m_pTempIDBuf = nullptr;
	}

	A3DRELEASE(m_pUABuf);
	A3DRELEASE(m_pStageBuf);

	m_setUniqueIDs.clear();
	m_aSortedIDs.clear();
}

bool CVTexturePageRequest::ResizeBuffers(int scn_width, int scn_height)
{
	MyAssert(scn_width && scn_height);

	int width = scn_width / VTexture::PAGE_UAV_DOWNSCALE;
	int height = scn_height / VTexture::PAGE_UAV_DOWNSCALE;

	if (width == m_iPageBufWid && height == m_iPageBufHei)
		return true;
	
	int count = width * height;

	m_pUABuf = new CDXUABuffer;
	if (!m_pUABuf->Init(count, sizeof(auint32), true))
	{
		glb_OutputLog("CVTexturePageRequest::ResizeBuffers, failed to create UA buffer!");
		return false;
	}

	m_pStageBuf = new CDXBuffer;
	if (!m_pStageBuf->Init(count, sizeof(auint32), nullptr, CDXBuffer::CPU_READ | CDXBuffer::CPU_WRITE))
	{
		glb_OutputLog("CVTexturePageRequest::ResizeBuffers, failed to create buffer!");
		return false;
	}

	m_pTempIDBuf = new auint32[count];

	m_setUniqueIDs.reserve(count / 10);
	m_aSortedIDs.reserve(count / 10);

	m_iPageBufWid = width;
	m_iPageBufHei = height;

	return true;
}

void CVTexturePageRequest::ClearUABuffer()
{
	if (m_pUABuf)
	{
		//	clear stage buffer
		//	TODO: lock buffer only with cpu_write flag
		void* pData = m_pStageBuf->Lock();
		memset(pData, 0, m_pStageBuf->GetElemNum() * m_pStageBuf->GetElemSize());
		m_pStageBuf->Unlock();
		m_pUABuf->Update(m_pStageBuf);
	}
}

void CVTexturePageRequest::BuildRequestedPageList()
{
	//	Transfer data from UAV to temporary buffer
	m_pUABuf->CopyOut(m_pStageBuf);
	//	TODO: lock buffer only with cpu_read flag
	void* pData = m_pStageBuf->Lock();
	memcpy(m_pTempIDBuf, pData, m_pStageBuf->GetElemNum() * m_pStageBuf->GetElemSize());
	m_pStageBuf->Unlock();

	int iNumTotal = m_pStageBuf->GetElemNum();
	int iNewPageCnt = 0;

	for (int i = 0; i < iNumTotal; )
	{
		auint32 id = m_pTempIDBuf[i];

		if (!id)
		{
			//	id == 0 isn't a valid page request
			m_pTempIDBuf[i] = m_pTempIDBuf[iNumTotal - 1];
			iNumTotal--;
		}
		else if (m_setUniqueIDs.find(id) == m_setUniqueIDs.end())
		{
			m_setUniqueIDs.insert(id);
			iNewPageCnt++;
			i++;
		}
		else
		{
			m_pTempIDBuf[i] = m_pTempIDBuf[iNumTotal - 1];
			iNumTotal--;
		}
	}

	for (int i = 0; i < iNewPageCnt; i++)
	{
		m_aSortedIDs.push_back(m_pTempIDBuf[i]);
	}

	//	sort page ids
	auto idCompLambda = [](const auint32& id1, const auint32& id2) -> bool
	{
		int mip1 = VTexture::PageID::GetMip(id1);
		int mip2 = VTexture::PageID::GetMip(id2);
		return mip1 > mip2;
	};

	std::sort(m_aSortedIDs.begin(), m_aSortedIDs.end(), idCompLambda);
}

void CVTexturePageRequest::ThrowAwayRequestedPageIDs(int count)
{
	if (count >= (int)m_aSortedIDs.size())
	{
		m_aSortedIDs.clear();
		m_setUniqueIDs.clear();
	}
	else
	{
		for (int i = 0; i < count; i++)
		{
			m_setUniqueIDs.erase(m_aSortedIDs[i]);
		}

		m_aSortedIDs.erase(m_aSortedIDs.begin(), m_aSortedIDs.begin() + count);
	}
}


