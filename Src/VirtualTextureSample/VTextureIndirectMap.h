/*	Copyright(c) 2020, Andy Do, All Rights Reserved.
	Contact: eaglehorn58@gmail.com, eaglehorn58@163.com
*/

//	The CVTextureIndirMap class try to use a quadtree to manager indirect map entries,
//	so that when a new page is added, all its children can point to the new page cache instead
//	of just point to the default page.
//	The class hasn't been finished, because updating the quadtree frequently may cause too much
//	time, but only a little rendering quality can be improved in the very short period of time
//	when a page's cache being loaded in.

#pragma once

#include "AXTypes.h"
#include "VTextureTypes.h"
#include "AList.h"
#include <vector>
#include <d3d11.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////

class CVTextureSystem;
class CDXTexture;

///////////////////////////////////////////////////////////////////////////
//	Class CVTextureIndirMap
///////////////////////////////////////////////////////////////////////////

class CVTextureIndirMap
{
public:		//	Types

	//	page cache status
	enum
	{
		PAGE_NOT_LOADED = 0,		//	page hasn't been loaded
		PAGE_LOADED,				//	page has been loaded
	};

	//	indirect entry
	struct INDIR_ENTRY
	{
		auint32			idPage;		//	id of page
		auint32			idCache;	//	id of page cache
		int				state;		//	cache state
		ALISTPOSITION	posAccess;	//	position in access list

		//	quadtree info
		int		iParent;		//	parent index
		int		children[4];	//	children
	};

	//	mip level of indirect map
	struct MIPMAP
	{
		int		iWidth;			//	mipmap width/height
		int		iStart;			//	start position of this mip's entries in buffer

		MIPMAP(int width, int start)
		{
			iWidth = width;
			iStart = start;
		}
	};

public:		//	Constructors and Destructors

	CVTextureIndirMap();
	virtual ~CVTextureIndirMap();

public:		//	Attributes

	//	Initialize
	bool Init(CVTextureSystem* pVTSys);
	//	Release
	void Release();

	//	Query spcified page's cache state
	//	bUpdateAccess: true, this query will update page's access state.
	//	return page's cache state CVTextureIndirMap::PAGE_xxx
	int QueryEntryState(const VTexture::PageID& page, bool bUpdateAccess);
	//	after a page has been added to cache, this function should be called to update indirect map
	//	cache_x, cache_y: new page cache's position at cache texture. if either one < 0, it means
	//	to replace the longest un-accessd page cache instead of add a new one
	void AddPage(const VTexture::PageID& page, int cache_x, int cache_y);

	//	get interfaces
	CDXTexture* GetIndirMapTexture() { return m_pTexIndir; }

public:		//	Operations

protected:	//	Attributes

	CVTextureSystem*		m_pVTSys;
	CDXTexture*				m_pTexIndir;		//	indirect texture
	CDXTexture*				m_pTexIndirStage;	//	indirect texture in staging memory

	INDIR_ENTRY*			m_pEntryBuf;		//	entry buffer
	int						m_iRootIndex;		//	index of the root entry of the root tree
	std::vector<MIPMAP>		m_aMips;			//	each mip level's start position in m_EntrieBuf
	APtrList<int>			m_listAccess;		//	access list

	int			m_iEntryNum;	//	total entry count
	auint32		m_idDefCache;	//	default cache id

protected:	//	Operations

	//	initialize entries
	void InitEntries();
	//	initialize entry quadtree
	void InitEntryTree_r(int index, int x, int y, int mip);
	//	Get entry's index in buffer
	int GetEntryIndex(int x, int y, int mip);
	//	remove the longest un-accessd page cache
	void RemoveOldestPageCache();
	//	Re-direct specified entry's (including it's children) cache position,
	//	if its mip level greater than the one passed in.
	//	iEntryIndex: index of the entry will be checked
	//	idCache: new cache positon
	//	mip: mip level of new cache position
	//	idInvalid: invalid cache position, if the will be checked entry point to
	//		this position, it must use the new cache position specified by idEntry.
	void RedirectEntry(int iEntryIndex, auint32 idCache, int mip, auint32 idInvalid);
	//	Commit changes caused by AddPage redirect texture
	void CommitChanges();
	void CommitChangesMipLevel(int mip);
};

