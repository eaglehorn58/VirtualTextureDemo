/*	Copyright(c) 2020, Andy Do, All Rights Reserved.
 	Contact: eaglehorn58@gmail.com, eaglehorn58@163.com
*/

#include "Global.h"
#include "VTextureIndirMap.h"
#include "VTextureSystem.h"
#include "A3DMacros.h"
#include "DXTexture.h"
#include "DXTextureMan.h"
#include "DXRenderer.h"


///////////////////////////////////////////////////////////////////////////
//	Implement of CVTextureIndirMap::INDIR_MIPMAP
///////////////////////////////////////////////////////////////////////////

CVTextureIndirMap::INDIR_MIPMAP::INDIR_MIPMAP(int width)
{
	iWdith = width;

	int count = width * width;
	aEntries = new INDIR_ENTRY[count];
	memset(aEntries, 0, sizeof(INDIR_ENTRY) * count);
}

//	get speicified entry
CVTextureIndirMap::INDIR_ENTRY& CVTextureIndirMap::INDIR_MIPMAP::GetEntry(int x, int y)
{
	ASSERT(x >= 0 && x < width && y >= 0 && y < width);
	return aEntries[y * iWidth + x];
}

///////////////////////////////////////////////////////////////////////////
//	Implement of CVTextureIndirMap
///////////////////////////////////////////////////////////////////////////

CVTextureIndirMap::CVTextureIndirMap() :
m_pVTSys(nullptr),
m_pTexIndir(nullptr),
m_pTexIndirStage(nullptr),
m_pEntryBuf(nullptr),
m_iEntryNum(0),
m_idDefCache(VTexture::CacheID::INVALID)
{
}

CVTextureIndirMap::~CVTextureIndirMap()
{
}

bool CVTextureIndirMap::Init(CVTextureSystem* pVTSys)
{
	m_pVTSys = pVTSys;

	//	default entry id
	const int iMipCount = m_pVTSys->GetMipmapCnt();
	m_idDefCache = VTexture::CacheID::MakeID(0, 0, iMipCount - 1, 0);

	//	initialize entry tree
	InitEntries();

	//	create indirect texture
	//	note: under dx11 the formats like DXGI_FORMAT_R32_UINT/DXGI_FORMAT_R8G8B8A8_UINT are not required
	//	to support shader sampling, this means that you may create texture in these formats, but you can't sample
	//	them in shader, so we use _UNORM formats here.
	//	See dx doc for detail:
	//	https://docs.microsoft.com/zh-cn/windows/win32/direct3ddxgi/format-support-for-direct3d-11-0-feature-level-hardware
	CDXTextureMan* pTexMan = DXRenderer::Instance()->GetTextureMan();

	m_pTexIndir = pTexMan->CreateTexture("VTIndirTex", pVTSys->GetIndirMapSize(), pVTSys->GetIndirMapSize(), 
		DXGI_FORMAT_R8G8B8A8_UNORM, false, iMipCount, 0);
	ASSERT(m_pTexIndir);

	m_pTexIndirStage = pTexMan->CreateTexture("VTIndirTexStage", pVTSys->GetIndirMapSize(), pVTSys->GetIndirMapSize(),
		DXGI_FORMAT_R8G8B8A8_UNORM, false, iMipCount, CDXTexture::CPU_WRITE);
	ASSERT(m_pTexIndirStage);

	//	Fill indirect map to default value
	const CDXTexture::TEX_DESC& desc = m_pTexIndirStage->GetDesc();
	for (int i = 0; i < desc.iMipLevels; i++)
	{
		const CDXTexture::MIP_INFO& mip = desc.aMips[i];

		//	fill stage texture
		CDXTexture::LOCK_INFO lock;
		m_pTexIndirStage->Lock(0, i, lock);

		int iHeight = mip.iHeight;
		for (int r = 0; r < iHeight; r++)
		{
			//	note: DON'T use mip.iRowPitch, use lock.uRowPitch instead
			auint32* pDst = (auint32*)((abyte*)lock.pData + lock.uRowPitch * r);

			for (int c = 0; c < mip.iWidth; c++)
			{
				*pDst++ = m_idDefEntry;
			}
		}

		m_pTexIndirStage->Unlock(0, i);
	}

	//	Copy to main indirect map
	m_pTexIndir->CopyFrom(0, m_pTexIndirStage, 0);

	return true;
}

void CVTextureIndirMap::Release()
{
	m_listAccess.RemoveAll();

	CDXTextureMan* pTexMan = DXRenderer::Instance()->GetTextureMan();

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

	if (m_pEntryBuf)
	{
		delete[] m_pEntryBuf;
		m_pEntryBuf = nullptr;
	}
}

void CVTextureIndirMap::InitEntries()
{
	const int iMipCount = m_pVTSys->GetMipmapCnt();
	int iWidth = m_pVTSys->GetIndirMapSize();

	//	initialize mipmap info
	m_iEntryNum = iWidth * iWidth;
	int iStart = 0;
	for (int i = iMipCount; i > 0; i--)
	{
		MIPMAP mip(iWidth, iStart);
		m_aMips.push_back(mip);

		iStart += m_iEntryNum;
		iWidth >>= 1;
		m_iEntryNum += iWidth * iWidth;
	}

	m_pEntryBuf = new INDIR_ENTRY[m_iEntryNum];
	memset(m_pEntryBuf, 0, sizeof(INDIR_ENTRY) * count);

	//	Initialize entries, and build parent & children links
	m_iRootIndex = m_aMips[iMipCount - 1].iStart;

	INDIR_ENTRY& root = m_pEntryBuf[m_iRootIndex];
	root.iParent = -1;
	InitEntryTree_r(m_iRootIndex, 0, 0, iMipCount - 1);
}

void CVTextureIndirMap::InitEntryTree_r(int index, int x, int y, int mip)
{
	INDIR_ENTRY& entry = m_pEntryBuf[index];

	entry.idPage = VTexture::PageID::MakeID(x, y, mip, 0);
	entry.idCache = m_idDefCache;
	entry.state = PAGE_NOT_LOADED;

	if (mip == 0)
	{
		//	this is a leaf
		entry.children[0] = -1;
		entry.children[1] = -1;
		entry.children[2] = -1;
		entry.children[3] = -1;
		return;
	}
	else
	{
		const int off_x = { 0, 1, 0, 1 };
		const int off_y = { 0, 0, 1, 1 };

		//	continue to children
		for (int i = 0; i < 4; i++)
		{
			int x1 = x * 2 + off_x[i];
			int y1 = y * 2 + off_y[i];

			int iChildIndex = GetEntryIndex(x1, y1, mip - 1);
			entry.children[i] = iChildIndex;

			INDIR_ENTRY& child = m_pEntryBuf[child];
			child.iParent = index;

			InitEntryTree_r(entry.children[i], x1, y1, mip - 1);
		}
	}
}

int CVTextureIndirMap::GetEntryIndex(int x, int y, int mip)
{
	const MIPMAP& MipInfo = m_aMips[mip];
	return MipInfo.iStart + MipInfo.iWidth * y + x;
}

int CVTextureIndirMap::QueryEntryState(const VTexture::PageID& page, bool bUpdateAccess)
{
	int index = GetEntryIndex(page.x, page.y, page.mip);
	INDIR_ENTRY& entry = m_pEntryBuf[index];

	if (bUpdateAccess)
	{
		//	update its position in access list
		m_listAccess.RemoveAt(entry.posAccess);
		entry.posAccess = m_listAccess.InsertAfter(m_listAccess.GetHeadPosition(), index);
	}

	return entry.state;
}

void CVTextureIndirMap::AddPage(const VTexture::PageID& page, int cache_x, int cache_y)
{
	//	get page
	int index = GetEntryIndex(page.x, page.y, page.mip);
	INDIR_ENTRY& newEntry = m_pEntryBuf[index];

	if (cache_x < 0 || cache_y < 0)
	{
		//	cache has been full, we shall replace the longest un-used one in the cache,
		//	the longest un-used cache is at the tail of access list, but at first, we
		//	need recored its position on cache texture.
		int iRemveIndex = m_listAccess.GetTail();
		auint32 idRemoveEntry = m_pEntryBuf[iRemveIndex].idCache;
		//	remove the page cache
		RemoveOldestPageCache();

		//	Place new page at the position that was just recorded
		VTexture::CacheID entry(idRemoveEntry);
		newEntry.idPage = page.id;
		newEntry.idCache = VTexture::CacheID::MakeID(entry.off_x, entry.off_y, page.mip, 0);
		newEntry.state = PAGE_LOADED;
		newEntry.posAccess = m_listAccess.InsertAfter(m_listAccess.GetHeadPosition(), index);
	}
	else
	{
		//	add a new cache
		newEntry.idPage = page.id;
		newEntry.idCache = VTexture::CacheID::MakeID(cache_x, cache_y, page.mip, 0);
		newEntry.state = PAGE_LOADED;
		newEntry.posAccess = m_listAccess.InsertAfter(m_listAccess.GetHeadPosition(), index);
	}

	//	redirect children's cache position if they pointed to a cache with higher mip level
	for (int i = 0; i < 4; i++)
	{
		int child = newEntry.children[i];
		if (child < 0)
			break;

		RedirectEntry(child, newEntry.idCache, page.mip, VTexture::CacheID::INVALID);
	}
}

void CVTextureIndirMap::RemoveOldestPageCache()
{
	//	the longest un-used cache is at the tail of access list
	int index = m_listAccess.RemoveTail();
	INDIR_ENTRY& entry = m_pEntryBuf[index];
	auint32 idRemovedCache = entry.idCache;

	entry.state = PAGE_NOT_LOADED;
	entry.posAccess = nullptr;

	//	the root page (default page) shouldn't be removed.
	ASSERT(entry.iParent >= 0);
	//	point to parent page's cache
	entry.idCache = m_pEntryBuf[entry.iParent].idCache;

	//	redirect children's cache position if they are pointing to the removed cache
	int mip = VTexture::CacheID::GetMip(entry.idCache);
	for (int i = 0; i < 4; i++)
	{
		int child = entry.children[i];
		if (child < 0)
			break;

		RedirectEntry(child, entry.idCache, mip, idRemovedCache);
	}
}

void CVTextureIndirMap::RedirectEntry(int iEntryIndex, auint32 idCache, int mip, auint32 idInvalid)
{
	ASSERT(iEntryIndex >= 0 && iEntryIndex < m_iEntryNum);

	INDIR_ENTRY& entry = m_pEntryBuf[iEntryIndex];
	if (entry.idCache == idInvalid || VTexture::CacheID::GetMip(entry.idCache) > mip)
	{
		entry.idCache = idCache;
	}

	//	handle all children
	for (int i = 0; i < 4; i++)
	{
		int child = entry.children[i];
		if (child < 0)
			break;

		RedirectEntry(child, idCache, mip, idInvalid);
	}
}

