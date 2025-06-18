//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#pragma once

#include "AXTypes.h"

///////////////////////////////////////////////////////////////////////////

class AFileWrapper;
class CVTextureSystem;

///////////////////////////////////////////////////////////////////////////
//	Class CVTextureStream
///////////////////////////////////////////////////////////////////////////

class CVTextureStream
{
public:		//	Types

	//	mipmap info in vt file
	struct MIP_DATA
	{
		auint64		offset;			//	mipmap data offset from start of file
		auint64		uDataSize;		//	data size
		int			iVPageCnt;		//	each mipmap contain iVPageCnt * iVPageCnt virtual pages
	};

public:		//	Constructors and Destructors

	CVTextureStream();
	virtual ~CVTextureStream();

public:		//	Attributes

public:		//	Operations

	//	Generate a virtual texture file
	//	szDDSFile: R8G8B8A8 un-compressed dds file
	static bool GenVirtualTexture(const char* szDDSFile, int iVTexSize, int iVPageSize, int iIndirTexSize, int iOriTexSize);

	//	Open vt file
	bool Open(CVTextureSystem* pVTSys, const char* szFile);
	//	Reelase
	void Release();

	//	Note: this function should only be called for debug.
	//	Read virtual page's content by UV on virtual texture
	//	u, v: uv on vt texture
	//	mipmap: mipmap index
	//	pBuf: buffer size should m_iPageDataSize
	bool ReadVPageByUV(float u, float v, int mipmap, abyte* pBuf);
	//	Read virtual page's content by page's position on virtual texture
	//	x, y: page position (in pages, not in vexels) on VT
	//	mipmap: mipmap index
	//	pBuf: buffer size should m_iPageDataSize
	bool ReadVPage(int x, int y, int mipmap, abyte* pBuf);

protected:	//	Attributes

	CVTextureSystem*	m_pVTSys;
	AFileWrapper*		m_pFile;

	int			m_iMipmapCnt;
	MIP_DATA*	m_aMipInfos;

protected:	//	Operations
	
};



