//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com

#include "Global.h"
#include "VTextureAtlas.h"
#include "VTextureSystem.h"
#include "VTextureStream.h"
#include "VTexturePageRequest.h"
#include "VTextureAtlasMesh.h"
#include "A3DMacros.h"
#include "AXViewport.h"
#include "DXTexture.h"
#include "DXTextureMan.h"
#include "DXRenderState.h"
#include "DXRenderer.h"
#include "DXRenderTarget.h"
#include "DXSampler.h"
#include <algorithm>

///////////////////////////////////////////////////////////////////////////
//	Implement of CVTextureAtlas
///////////////////////////////////////////////////////////////////////////

CVTextureAtlas::CVTextureAtlas() :
m_pVTSys(nullptr),
m_pTopView(nullptr),
m_pPageCacheRT(nullptr),
m_pTexPageCache(nullptr),
m_pTexIndir(nullptr),
m_pTexIndirStage(nullptr),
m_pTArrayNewPages(nullptr),
m_pPageDataBuf(nullptr),
m_pFillPageCacheRS(nullptr),
m_pPtClampSamp(nullptr),
m_pVTSampler(nullptr),
m_pVTIndirSampler(nullptr),
m_pAtlasMesh(nullptr),
m_bAtlasInit(false),
m_iMaxPageCacheCnt(0),
m_idDefPage(VTexture::PageID::INVALID),
m_idDefCache(VTexture::CacheID::INVALID)
{
}

CVTextureAtlas::~CVTextureAtlas()
{
}

bool CVTextureAtlas::Init(CVTextureSystem* pVTSys)
{
	m_pVTSys = pVTSys;

	m_iMaxPageCacheCnt = PAGE_CACHE_TEX_SIZE / pVTSys->GetVPageSize();
	m_iMaxPageCacheCnt *= m_iMaxPageCacheCnt;

	m_mapCachedPage.reserve(m_iMaxPageCacheCnt);
	m_aChangedEntries.reserve(VTexture::MAX_VPAGE_LOADED * 2);

	CDXTextureMan* pTexMan = DXRenderer::Instance()->GetTextureMan();

	//	create page cache render target
	DXGI_FORMAT fmt = pVTSys->GetVTFormat();
	m_pPageCacheRT = new CDXRenderTarget;
	if (!m_pPageCacheRT->Init(PAGE_CACHE_TEX_SIZE, PAGE_CACHE_TEX_SIZE, 1, &fmt, DXGI_FORMAT_UNKNOWN))
	{
		glb_OutputLog("CVTextureAtlas::Init, Failed to page cache render target!");
		return false;
	}

	m_pPageCacheRT->SetClearColor(0, A3DCOLORVALUE(1.0f, 0.5f, 1.0f, 1.0f));

	//	create VT page cache texture
	m_pTexPageCache = pTexMan->CreateTexture("VTPageCacheTex", m_pPageCacheRT, 0);
	MyAssert(m_pTexPageCache);

	//	create indirect map
	//	note: under dx11 the formats like DXGI_FORMAT_R32_UINT/DXGI_FORMAT_R8G8B8A8_UINT are not required
	//	to support shader sampling, this means that you may create texture in these formats, but you can't sample
	//	them in shader, so we use _UNORM formats here.
	//	See dx doc for detail:
	//	https://docs.microsoft.com/zh-cn/windows/win32/direct3ddxgi/format-support-for-direct3d-11-0-feature-level-hardware
	m_pTexIndir = pTexMan->CreateTexture("VTIndirTex", pVTSys->GetIndirMapSize(), pVTSys->GetIndirMapSize(), 
		DXGI_FORMAT_R8G8B8A8_UNORM, false, pVTSys->GetMipmapCnt(), 0);
	MyAssert(m_pTexIndir);

	m_pTexIndirStage = pTexMan->CreateTexture("VTIndirTexStage", pVTSys->GetIndirMapSize(), pVTSys->GetIndirMapSize(),
		DXGI_FORMAT_R8G8B8A8_UNORM, false, pVTSys->GetMipmapCnt(), CDXTexture::CPU_WRITE);
	MyAssert(m_pTexIndirStage);

	//	create default page
	m_pTArrayNewPages = pTexMan->CreateTextureArray("VTNewPageTexArray", pVTSys->GetVPageSize(), pVTSys->GetVPageSize(),
		pVTSys->GetVTFormat(), VTexture::MAX_VPAGE_LOADED, 1);
	MyAssert(m_pTArrayNewPages);

	//	temporary buffer
	m_pPageDataBuf = new abyte[pVTSys->GetVPageDataSize()];

	//	Altas mesh
	m_pAtlasMesh = new CVTextureAtlasMesh(pVTSys);
	MyAssert(m_pAtlasMesh);

	//	Render states
	CDXRenderState::DEPTH ds;
	ds.d.DepthEnable = FALSE;
	ds.d.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	m_pFillPageCacheRS = new CDXRenderState(NULL, &ds, NULL, false);

	//	samplers
	m_pPtClampSamp = new CDXSampler(CDXSampler::FILTER_POINT, CDXSampler::ADDR_CLAMP);
//	m_pVTSampler = new CDXSampler(CDXSampler::FILTER_POINT, CDXSampler::ADDR_CLAMP);
	m_pVTSampler = new CDXSampler(CDXSampler::FILTER_LINEAR, CDXSampler::ADDR_CLAMP);
	m_pVTIndirSampler = new CDXSampler(CDXSampler::FILTER_POINT, CDXSampler::ADDR_CLAMP);

	//	create viewport
	A3DVIEWPORTPARAM viewParams;
	viewParams.X = 0;
	viewParams.Y = 0;
	viewParams.Width = PAGE_CACHE_TEX_SIZE;
	viewParams.Height = PAGE_CACHE_TEX_SIZE;
	viewParams.MinZ = 0.0f;
	viewParams.MaxZ = 1.0f;
	m_pTopView = new AXViewport;
	m_pTopView->Init(&viewParams);

	//	Initialize some content
	InitializeContent();

	return true;
}

void CVTextureAtlas::Release()
{
	m_mapCachedPage.clear();
	m_aChangedEntries.clear();
	m_listAccess.RemoveAll();

	CDXTextureMan* pTexMan = DXRenderer::Instance()->GetTextureMan();

	if (m_pPageDataBuf)
	{
		delete[] m_pPageDataBuf;
		m_pPageDataBuf = nullptr;
	}

	if (m_pTArrayNewPages)
	{
		pTexMan->ReleaseTexture(m_pTArrayNewPages);
		m_pTArrayNewPages = nullptr;
	}

	if (m_pTexIndir)
	{
		pTexMan->ReleaseTexture(m_pTexIndir);
		m_pTexIndir = nullptr;
	}

	if (m_pTexIndirStage)
	{
		pTexMan->ReleaseTexture(m_pTexIndirStage);
		m_pTexIndirStage = nullptr;
	}

	if (m_pTexPageCache)
	{
		pTexMan->ReleaseTexture(m_pTexPageCache);
		m_pTexPageCache = nullptr;
	}

	A3DRELEASE(m_pTopView);
	A3DSAFEDELETE(m_pAtlasMesh);
	A3DRELEASE(m_pPageCacheRT);
	A3DSAFEDELETE(m_pFillPageCacheRS);
	A3DSAFEDELETE(m_pPtClampSamp);
}

bool CVTextureAtlas::InitializeContent()
{
	//	load default page content and fill it into m_pTArrayNewPages at index 0
	CVTextureStream* pVTStream = m_pVTSys->GetVTStream();
	pVTStream->ReadVPage(0, 0, m_pVTSys->GetMipmapCnt() - 1, m_pPageDataBuf);

	CDXTexture::UPDATE_INFO ui;
	ui.iArrayIdx = 0;
	ui.iMipLevel = 0;
	ui.pPixels = m_pPageDataBuf;
	m_pTArrayNewPages->Update(&ui, 1);

	//	default page and cache id
	m_idDefPage = VTexture::PageID::MakeID(0, 0, m_pVTSys->GetMipmapCnt() - 1, 0);
	m_idDefCache = VTexture::CacheID::MakeID(0, 0, m_pVTSys->GetMipmapCnt() - 1, 0);

	//	Record default page's stub
	CACHE_STUB stub;
	stub.idPage = m_idDefPage;
	stub.idCache = m_idDefCache;
	stub.posAccessList = m_listAccess.AddHead(stub.idPage);
	m_mapCachedPage[stub.idPage] = stub;
	m_aChangedEntries.push_back(stub);

	//	Fill indirect map to default value
	const CDXTexture::TEX_DESC& desc = m_pTexIndirStage->GetDesc();
	for (int i = 0; i < desc.iMipLevels; i++)
	{
		const CDXTexture::MIP_INFO& mip = desc.aMips[i];

		CDXTexture::LOCK_INFO lock;
		m_pTexIndirStage->Lock(0, i, lock);

		int iHeight = mip.iHeight;

		for (int r = 0; r < iHeight; r++)
		{
			//	note: DON'T use mip.iRowPitch, use lock.uRowPitch instead
			auint32* pDst = (auint32*)((abyte*)lock.pData + lock.uRowPitch * r);

			for (int c = 0; c < mip.iWidth; c++)
			{
				*pDst++ = m_idDefCache;
			}
		}

		m_pTexIndirStage->Unlock(0, i);
	}

	//	Copy to main indirect map
	m_pTexIndir->CopyFrom(0, m_pTexIndirStage, 0);

	return true;
}

void CVTextureAtlas::LoadRequestedPages()
{
	if (m_bAtlasInit)
	{
		//	m_aChangedEntries should be cleared every frame
		MyAssert(!m_aChangedEntries.size());
	}

	CVTextureStream* pVTStream = m_pVTSys->GetVTStream();
	CVTexturePageRequest* pVTPageReq = m_pVTSys->GetVTPageRequest();

	int iReqPageCnt = pVTPageReq->GetRequestedPageCount();
	if (iReqPageCnt)
	{
		int iPageLoadedCnt = 0;

		int i;
		for (i = 0; i < iReqPageCnt; i++)
		{
			auint32 idPage = pVTPageReq->GetRequestedPage(i);

			//	don't consider debug info in id
			idPage = VTexture::PageID::ClearDebugComponent(idPage);
			//	default page is always cached, akip it.
			if (idPage == m_idDefPage)
				continue;

			VTexture::PageID page(idPage);

			//	check if this page has been existed in cache.
			auto iter = m_mapCachedPage.find(page.id);
			if (iter != m_mapCachedPage.end())
			{
				//	page has been existed, update its position in access list
				CACHE_STUB& stub = (*iter).second;
				m_listAccess.RemoveAt(stub.posAccessList);
				stub.posAccessList = m_listAccess.InsertAfter(m_listAccess.GetHeadPosition(), page.id);
				continue;	
			}

			//	if we have loaded enough pages in this frame, break out.
			//	+1: don't forget the default page always hold one place in texture array (m_pTArrayNewPages)
			if (iPageLoadedCnt + 1 >= VTexture::MAX_VPAGE_LOADED)
			{
				break;
			}

			//	load page data from stream
			MyAssert(page.mip < m_pVTSys->GetMipmapCnt());
			pVTStream->ReadVPage(page.x, page.y, page.mip, m_pPageDataBuf);

			//	Fill page content into texture
			CDXTexture::UPDATE_INFO ui;
			ui.iArrayIdx = iPageLoadedCnt + 1;	//	the position of index 0 was hold by the default page
			ui.iMipLevel = 0;
			ui.pPixels = m_pPageDataBuf;
			m_pTArrayNewPages->Update(&ui, 1);

			//	Add page to cache
			AddPageToCache(page);

			iPageLoadedCnt++;
		}

		//	Throw away the request pages that we have loaded
		if (i > 0)
		{
			pVTPageReq->ThrowAwayRequestedPageIDs(i);
		}
	}
}

void CVTextureAtlas::AddPageToCache(const VTexture::PageID& page)
{
	if ((int)m_mapCachedPage.size() >= m_iMaxPageCacheCnt)
	{
		//	page cache has been full, we shall replace the longest un-used one in the cache.
		//	the longest un-used cache is at the tail of access list.
		auint32 idPageToRemove = m_listAccess.RemoveTail();
		MyAssert(m_listAccess.GetCount() > 0);
		auto iter = m_mapCachedPage.find(idPageToRemove);
		MyAssert(iter != m_mapCachedPage.end());

		//	record cache position for new loaded page at first, then make the entry point to
		//	the default page cache.
		CACHE_STUB stub = (*iter).second;
		VTexture::CacheID entry(stub.idCache);
		//	shouldn't replace the default page
		MyAssert(entry.off_x != 0 || entry.off_y != 0);
		stub.idCache = m_idDefCache;
		stub.posAccessList = nullptr;
		m_aChangedEntries.push_back(stub);

		//	remove the stub
		m_mapCachedPage.erase(iter);

		//	add new page at the position of old one's in cache map
		stub.idPage = page.id;
		stub.idCache = VTexture::CacheID::MakeID(entry.off_x, entry.off_y, page.mip, 0);
		stub.posAccessList = m_listAccess.InsertAfter(m_listAccess.GetHeadPosition(), (auint32)page.id);

		m_mapCachedPage[page.id] = stub;
		m_aChangedEntries.push_back(stub);
	}
	else
	{
		//	add a new page to cache
		//	find a new position in cache texture
		const int iPageInRow = PAGE_CACHE_TEX_SIZE / m_pVTSys->GetVPageSize();
		int index = (int)m_mapCachedPage.size();
		int x = index % iPageInRow;
		int y = index / iPageInRow;

		//	shouldn't replace the default page
		MyAssert(x != 0 || y != 0);

		CACHE_STUB stub;
		stub.idPage = page.id;
		stub.idCache = VTexture::CacheID::MakeID(x, y, page.mip, 0);
		stub.posAccessList = m_listAccess.InsertAfter(m_listAccess.GetHeadPosition(), (auint32)page.id);

		m_mapCachedPage[page.id] = stub;
		m_aChangedEntries.push_back(stub);
	}
}

void CVTextureAtlas::RenderAtlas()
{
	if (!m_pPageCacheRT)
		return;

	CVTexturePageRequest* pVTPageReq = m_pVTSys->GetVTPageRequest();

	if (m_aChangedEntries.size())
	{
		//	collect postions on cache texture that will be updated
		auint32 aUpdatedCaches[VTexture::MAX_VPAGE_LOADED];
		int iUpdatedCacheCnt = 0;

		for (int i = 0; i < (int)m_aChangedEntries.size(); i++)
		{
			const CACHE_STUB& stub = m_aChangedEntries[i];

			//	default page is updated on the first frame, after then stub.idCache == m_idDefCache
			//	means that cache is removed (replaced by other page cache).
			if (!m_bAtlasInit || stub.idCache != m_idDefCache)
			{
				aUpdatedCaches[iUpdatedCacheCnt++] = stub.idCache;
			}
		}

		MyAssert(iUpdatedCacheCnt < VTexture::MAX_VPAGE_LOADED);

		//	Refresh indirect map
		RefreshIndirectMap();

		//	Draw new pages to page cahce
		auint32 uClearFlags = m_bAtlasInit ? 0 : DXRenderer::CLEAR_COLOR;
		DXRenderer::Instance()->PushRenderTarget(m_pPageCacheRT, uClearFlags);
		DXRenderer::Instance()->PushViewport(m_pTopView);

		m_pPtClampSamp->ApplyToPS(0);
		m_pFillPageCacheRS->Apply();
		m_pTArrayNewPages->ApplyToPS(0);

	//	if (!m_bAtlasInit)	//	test code...
			m_pAtlasMesh->Render(aUpdatedCaches, iUpdatedCacheCnt, m_bAtlasInit == true);

		DXRenderer::Instance()->PopViewport(m_pTopView);
		DXRenderer::Instance()->PopRenderTarget(m_pPageCacheRT);

		m_bAtlasInit = true;

		//	all new loaded pages have been handled
		m_aChangedEntries.clear();
	}
}

void CVTextureAtlas::RefreshIndirectMap()
{
	if (!m_aChangedEntries.size())
		return;

	//	sort stubs in m_aChangedEntries by page's mip,
	//	so we can update all entries of the same mip level in one batch
	auto StubCompLambda = [](const CACHE_STUB& stub1, const CACHE_STUB& stub2) -> bool
	{
		int mip1 = VTexture::PageID::GetMip(stub1.idPage);
		int mip2 = VTexture::PageID::GetMip(stub2.idPage);
		return mip1 < mip2;
	};

	std::sort(m_aChangedEntries.begin(), m_aChangedEntries.end(), StubCompLambda);

	//	update indirect texture
	VTexture::PageID page0(m_aChangedEntries[0].idPage);
	int cur_mip = page0.mip;
	int start = 0;
	int iNumEntry = (int)m_aChangedEntries.size();

	for (int i = 1; i < iNumEntry; i++)
	{
		int mip = VTexture::PageID::GetMip(m_aChangedEntries[i].idPage);
		if (cur_mip != mip)
		{
			RefreshIndirectMapMipLevel(start, i - start, cur_mip);
			cur_mip = mip;
			start = i;
		}
	}

	//	the last batch
	RefreshIndirectMapMipLevel(start, iNumEntry - start, cur_mip);
}

void CVTextureAtlas::RefreshIndirectMapMipLevel(int iStartIndex, int iNumEntry, int mip)
{
	//	Fill indirect map to default value
	const CDXTexture::TEX_DESC& desc = m_pTexIndirStage->GetDesc();
	const CDXTexture::MIP_INFO& mip_info = desc.aMips[mip];

	CDXTexture::LOCK_INFO lock;
	m_pTexIndirStage->Lock(0, mip, lock);

	abyte* pData = (abyte*)lock.pData;

	for (int i = 0; i < iNumEntry; i++)
	{
		const CACHE_STUB& stub = m_aChangedEntries[iStartIndex + i];
		VTexture::PageID page(stub.idPage);

		MyAssert(page.x < mip_info.iWidth && page.y < mip_info.iHeight);

		//	note: DON'T use mip.iRowPitch, use lock.uRowPitch instead
		auint32* pPixel = (auint32*)(pData + lock.uRowPitch * page.y + page.x * sizeof(auint32));
		*pPixel = stub.idCache;
	}

	m_pTexIndirStage->Unlock(0, mip);

	//	Copy to main indirect map
	m_pTexIndir->CopyFrom(0, m_pTexIndirStage, 0, mip);
}

