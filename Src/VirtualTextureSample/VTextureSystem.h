//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#pragma once

#include "AXTypes.h"
#include <dxgiformat.h>

///////////////////////////////////////////////////////////////////////////

class CVTexturePageRequest;
class CVTextureStream;
class CVTextureAtlas;

///////////////////////////////////////////////////////////////////////////
//	Class CVTextureSystem
///////////////////////////////////////////////////////////////////////////

class CVTextureSystem
{
public:		//	Types

	friend class CVTextureStream;

	//	original texture
	struct ORIGINAL_TEX
	{
		auint32		idTex;			//	texture id
		float		off_u;			//	u offset in VT
		float		off_v;			//	v offset in VT
		float		scale;			//	size scale
	};

public:		//	Constructors and Destructors

	CVTextureSystem();
	virtual ~CVTextureSystem();

public:		//	Attributes

public:		//	Operations

	//	Initialize
	//	scn_width, scn_height: size of the screen
	//	szFile: VT file name
	bool Init(int scn_width, int scn_height, const char* szFile);
	//	Release
	void Release();

	//	When main window size changed, this function should be called
	void OnWindowResize(int scn_width, int scn_height);

	//	This function should be called before the page id requesting (UAV) rendering step
	void BeforePageIDRequestRendering();
	//	This function should be called after the page id requesting (UAV) rendering step.
	void AfterPageIDRequestRendering();

	//	Get dither offset for requesting page id
	const int GetDitherX() const { return m_iDitherX; }
	const int GetDitherY() const { return m_iDitherY; }

	//	Get original texture info
	const ORIGINAL_TEX& GetOriginalTexInfo(int n) const;
	int GetVTexSize() const { return m_iVTexSize; }
	int GetVPageSize() const { return m_iVPageSize; }
	int GetIndirMapSize() const { return m_iIndirMapSize; }
	int GetPixelBytes() const { return m_iPixelBytes; }
	int GetVPageDataSize() const { return m_iPageDataSize; }
	int GetMipmapCnt() const { return m_iMipmapCnt; }
	int GetOriginalTexNum() const { return m_iOriginalTexNum; }
	DXGI_FORMAT GetVTFormat() const { return m_fmtVT; }
	//	Get page cache texture (physical texture) size
	int GetPageCacheTexSize() const;

	//	Get interfaces
	CVTextureStream* GetVTStream() { return m_pVTStream; }
	CVTexturePageRequest* GetVTPageRequest() { return m_pVTPageRequest;	}
	CVTextureAtlas* GetVTAtlas() { return m_pVTAtlas; }

protected:	//	Attributes

	CVTextureStream*		m_pVTStream;
	CVTexturePageRequest*	m_pVTPageRequest;
	CVTextureAtlas*			m_pVTAtlas;

	ORIGINAL_TEX*	m_aOriTexInfo;
	int				m_iVTexSize;		//	width(height) of vt in pixels
	int				m_iVPageSize;		//	width(height) of virtual page in pixels
	int				m_iIndirMapSize;	//	indirect map size (level 0 mipmap)
	int				m_iPixelBytes;		//	bytes per pixel
	int				m_iMipmapCnt;		//	mipmap count
	int				m_iOriginalTexNum;	//	original texture info
	DXGI_FORMAT		m_fmtVT;			//	format of original textures
	int				m_iPageDataSize;	//	data size of each virtual page

	int				m_iDitherCnt;		//	dither data used in page requesting
	int				m_iDitherX;			//	dither data used in page requesting
	int				m_iDitherY;			//	dither data used in page requesting

protected:	//	Operations

};

