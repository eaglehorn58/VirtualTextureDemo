//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com

#pragma once

#include "AXTypes.h"
#include "DXShaderMate.h"
#include "VTextureTypes.h"
#include "AList.h"
#include <unordered_map>
#include <vector>
#include <d3d11.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////

class CVTextureSystem;
class CDXTexture;
class AXViewport;
class CDXRenderTarget;
class CDXRenderState; 
class CDXSampler;
class CVTextureAtlasMesh;

///////////////////////////////////////////////////////////////////////////
//	Class CVTextureAtlas
///////////////////////////////////////////////////////////////////////////

class CVTextureAtlas
{
public:		//	Types

	enum
	{
		PAGE_CACHE_TEX_SIZE = 2048,		//	page cache texture size
	};

	struct CACHE_STUB
	{
		auint32			idPage;			//	page id
		auint32			idCache;		//	page cache id
		ALISTPOSITION	posAccessList;	//	position in access list
	};

	typedef std::unordered_map<auint32, CACHE_STUB> CacheStubMap;

public:		//	Constructors and Destructors

	CVTextureAtlas();
	virtual ~CVTextureAtlas();

public:		//	Attributes

	//	Initialize
	bool Init(CVTextureSystem* pVTSys);
	//	Release
	void Release();

	//	Load newly requested pages from stream
	void LoadRequestedPages();
	//	Update vt atlas with the new pages that were loaded by LoadRequestedPages(),
	//	this function should be called during rendering.
	void RenderAtlas();

	//	get interfaces
	CDXTexture* GetVPageCacheTexture() { return m_pTexPageCache; }
	CDXTexture* GetIndirMapTexture() { return m_pTexIndir; }
	CDXSampler*	GetVTSampler() { return m_pVTSampler; }
	CDXSampler*	GetVTIndirSampler() { return m_pVTIndirSampler; }

public:		//	Operations

protected:	//	Attributes

	CVTextureSystem*		m_pVTSys;
	AXViewport*				m_pTopView;			//	Top viewport covers editing area
	CDXRenderTarget*		m_pPageCacheRT;		//	page cache render target
	CDXTexture*				m_pTexPageCache;	//	page cache texture
	CDXTexture*				m_pTexIndir;		//	indirect texture
	CDXTexture*				m_pTexIndirStage;	//	indirect texture in staging memory
	CDXTexture*				m_pTArrayNewPages;	//	texture array of newly loaded pages
	abyte*					m_pPageDataBuf;		//	temporary buffer for loading page data 
	CDXRenderState*			m_pFillPageCacheRS;	//	Render state for filling page cache
	CDXSampler*				m_pPtClampSamp;		//	point/clamp sampler
	CDXSampler*				m_pVTSampler;		//	sampler for VT texture
	CDXSampler*				m_pVTIndirSampler;	//	sampler for VT indirect texture
	CVTextureAtlasMesh*		m_pAtlasMesh;

	bool		m_bAtlasInit;			//	true, atlas content has been initialized
	int			m_iMaxPageCacheCnt;		//	maximum number of page can be hold in cache
	auint32		m_idDefPage;			//	default page id
	auint32		m_idDefCache;			//	default cache id

	CacheStubMap			m_mapCachedPage;	//	current cached pages
	std::vector<CACHE_STUB>	m_aChangedEntries;	//	changed indirect entries in current frame
	AList<auint32>			m_listAccess;		//	access list

protected:	//	Operations

	//	Initialize some content
	bool InitializeContent();
	//	Add a new loaded page to cache
	void AddPageToCache(const VTexture::PageID& page);
	//	Refresh indirect map texture
	void RefreshIndirectMap();
	//	Refresh indirect map texture of specified mip level
	void RefreshIndirectMapMipLevel(int iStartIndex, int iNumEntry, int mip);
};

