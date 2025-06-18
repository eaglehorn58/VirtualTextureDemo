//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


//	TODO: 
//	- if a page is removed from cache, set it's reindirect entry to a parent entry
//	- texture filter
//	- create multiple UAV to receive page ids from GPU
//	- multithread streaming page data

#include "Global.h"
#include "VTextureSystem.h"
#include "VTextureStream.h"
#include "VTexturePageRequest.h"
#include "VTextureAtlas.h"
#include "A3DMacros.h"

///////////////////////////////////////////////////////////////////////////
//	Implement of CVTextureSystem
///////////////////////////////////////////////////////////////////////////

CVTextureSystem::CVTextureSystem() :
m_pVTStream(nullptr),
m_pVTPageRequest(nullptr),
m_pVTAtlas(nullptr),
m_aOriTexInfo(nullptr),
m_iVTexSize(0),
m_iVPageSize(0),
m_iIndirMapSize(0),
m_iPixelBytes(0),
m_iMipmapCnt(0),
m_iOriginalTexNum(0),
m_fmtVT(DXGI_FORMAT_UNKNOWN),
m_iPageDataSize(0),
m_iDitherCnt(0),
m_iDitherX(0),
m_iDitherY(0)
{
}

CVTextureSystem::~CVTextureSystem()
{
}

bool CVTextureSystem::Init(int scn_width, int scn_height, const char* szFile)
{
	m_pVTStream = new CVTextureStream;
	if (!m_pVTStream->Open(this, szFile))
	{
		glb_OutputLog("CVTextureSystem::Init, failed to open vt file [%s]", szFile);
		return false;
	}

	m_pVTPageRequest = new CVTexturePageRequest;
	if (!m_pVTPageRequest->Init(this, scn_width, scn_height))
	{
		glb_OutputLog("CVTextureSystem::Init, failed to init CVTexturePageRequest");
		return false;
	}

	m_pVTAtlas = new CVTextureAtlas;
	if (!m_pVTAtlas->Init(this))
	{
		glb_OutputLog("CVTextureSystem::Init, failed to init CVTextureAtlas");
		return false;
	}

	return true;
}

void CVTextureSystem::Release()
{
	if (m_aOriTexInfo)
	{
		delete[] m_aOriTexInfo;
		m_aOriTexInfo = nullptr;
	}

	A3DRELEASE(m_pVTAtlas);
	A3DRELEASE(m_pVTPageRequest);
	A3DRELEASE(m_pVTStream);
}

const CVTextureSystem::ORIGINAL_TEX& CVTextureSystem::GetOriginalTexInfo(int n) const
{
	MyAssert(m_aOriTexInfo);
	MyAssert(n >= 0 && n < m_iOriginalTexNum);
	return m_aOriTexInfo[n];
}

void CVTextureSystem::OnWindowResize(int scn_width, int scn_height)
{
	if (m_pVTPageRequest)
	{
		m_pVTPageRequest->ResizeBuffers(scn_width, scn_height);
	}
}

int CVTextureSystem::GetPageCacheTexSize() const
{
	return CVTextureAtlas::PAGE_CACHE_TEX_SIZE;
}


void CVTextureSystem::BeforePageIDRequestRendering()
{
	//	clear UAV buffer
	m_pVTPageRequest->ClearUABuffer();
}

void CVTextureSystem::AfterPageIDRequestRendering()
{
	//	Build list of pages requested in this rendering
	m_pVTPageRequest->BuildRequestedPageList();

	//	set a new dither offset
	const int iTotalCount = VTexture::PAGE_UAV_DOWNSCALE * VTexture::PAGE_UAV_DOWNSCALE;
	if (++m_iDitherCnt >= iTotalCount)
	{
		m_iDitherCnt = 0;
	}

	m_iDitherX = m_iDitherCnt % VTexture::PAGE_UAV_DOWNSCALE;
	m_iDitherY = m_iDitherCnt / VTexture::PAGE_UAV_DOWNSCALE;
}



