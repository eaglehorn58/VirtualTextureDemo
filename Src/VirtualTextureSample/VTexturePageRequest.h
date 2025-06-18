//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#pragma once

#include "AXTypes.h"
#include "VTextureTypes.h"
#include <unordered_set>
#include <vector>

///////////////////////////////////////////////////////////////////////////

class CVTextureSystem;
class CDXUABuffer;
class CDXBuffer;

///////////////////////////////////////////////////////////////////////////
//	Class CVTexturePageRequest
///////////////////////////////////////////////////////////////////////////

class CVTexturePageRequest
{
public:		//	Types

	friend class CVTextureSystem;

public:		//	Constructors and Destructors

	CVTexturePageRequest();
	virtual ~CVTexturePageRequest();

public:		//	Attributes

public:		//	Operations

	//	Initialize
	//	scn_width, scn_height: size of the screen
	bool Init(CVTextureSystem* pVTSys, int scn_width, int scn_height);
	//	Release
	void Release();

	//	Get number of requested pages
	int GetRequestedPageCount() const { return (int)m_aSortedIDs.size(); }
	//	Get one of requested page id
	auint32 GetRequestedPage(int n) const { return m_aSortedIDs[n]; }
	//	Throw away specified number the request pages in front of m_aSortedIDs
	void ThrowAwayRequestedPageIDs(int count);

	//	get interfaces
	CDXUABuffer* GetUABuffer() { return m_pUABuf; }

protected:	//	Attributes

	CVTextureSystem*	m_pVTSys;
	CDXUABuffer*		m_pUABuf;		//	UA buffer used to collect page ids from pixel shader
	CDXBuffer*			m_pStageBuf;	//	Stage buffer used to transfer data to UA buffer
	auint32*			m_pTempIDBuf;	//	temporary page id buffer

	std::unordered_set<auint32>	m_setUniqueIDs;		//	hash table used to remove repeated page ids.
	std::vector<auint32>		m_aSortedIDs;		//	sorted page ids

	int		m_iPageBufWid;	//	page buffer width
	int		m_iPageBufHei;	//	page buffer height

protected:	//	Operations
	
	//	Release buffers that are associated with page requesting
	void ReleaseBuffers();
	//	Resize page request buffer
	//	scn_width, scn_height: new size of the screen
	bool ResizeBuffers(int scn_width, int scn_height);
	//	Clear UA buffer content
	void ClearUABuffer();
	//	This function fetch requested page ids from GPU (UA buffer) to CPU (m_pPageIDBuf)
	void BuildRequestedPageList();
};

