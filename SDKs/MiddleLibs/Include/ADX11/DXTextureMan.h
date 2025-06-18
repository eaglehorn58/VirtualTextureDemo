/*
 * FILE: DXTextureMan.h
 *
 * DESCRIPTION: Class that manage all textures in A3D Engine
 *
 * CREATED BY: Hedi, 2001/11/23
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _DXTEXTUREMAN_H_
#define _DXTEXTUREMAN_H_

#include <d3d11.h>
#include "AXTypes.h"
#include "hashtab.h"

class CDXTexture;
class CDXRenderTarget;
class ASysThreadMutex;

class CDXTextureMan
{
public:		//	Types

	struct TEXTURESLOT
	{
		char			szFile[MAX_PATH];
		CDXTexture*		pTexture;
		int				iRefCount;

		TEXTURESLOT();
	};

	typedef abase::hashtab<TEXTURESLOT*, auint32, abase::_hash_function> TextureTable;

public:

	CDXTextureMan();
	virtual ~CDXTextureMan();

public:

public:

	//	Initialize object
	bool Init();
	//	Release object
	void Release();

	//	Create a empty content texture
	//	szID: a unique string used to identify texture
	//	iMipLevels: the maximum number of mipmap levels in the texture. use 0 to generate a full set of subtextures
	//	uCPUAccess: CPU access flags. CDXTexture::CPU_xxx
	CDXTexture* CreateTexture(const char* szID, int width, int height, DXGI_FORMAT fmt, bool bCubeMap, int iMipLevels = 0, auint32 uCPUAccess = 0);
	//	iArraySize: texture array size
	CDXTexture* CreateTextureArray(const char* szID, int width, int height, DXGI_FORMAT fmt, int iArraySize, int iMipLevels = 0);
	//	Load texture from file
	//	iMipLevels: the maximum number of mipmap levels in the texture, 0 means to use the number stored in file
	CDXTexture* LoadTextureFromFile(const char* szFile, int iMipLevels = 0);
	//	Create texture from a render target
	CDXTexture* CreateTexture(const char* szID, CDXRenderTarget* pRT, int iRTIndex);
	//	Create small texture with prue color
	CDXTexture* CreateTexture(const char* szID, A3DCOLOR color);
	//	Clone a texture object
	CDXTexture* CloneTexture(CDXTexture* pSrcTex);
	//	Release texture
	void ReleaseTexture(CDXTexture* pTexture);

	//	Get texture number
	int GetTextureCount() const;

protected:

	ASysThreadMutex*	m_csTexTab;		//	Texture table lock
	TextureTable		m_tabTex;		//	Texture table

protected:

	//	Create a new texture slot and push it to table
	void CreateTextureSlot(CDXTexture* pTex);
	//	Search a texture with speicfied file name
	//	bAddRef: true, add texture reference counter if it exists
	TEXTURESLOT* FindTexture(const char* szFilename, bool bAddRef = false);
	//	Release all textures
	void ReleaseAllTextures();
};


#endif	//	_DXTEXTUREMAN_H_
