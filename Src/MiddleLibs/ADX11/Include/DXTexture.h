//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#ifndef _DXTEXTURE_H_
#define _DXTEXTURE_H_

#include <d3d11.h>
#include "AXTypes.h"
#include "AString.h"

class CDXRenderTarget;

class CDXTexture
{
public:		//	Types

	friend class CDXTextureMan;

	//	CPU access flags
	enum
	{
		CPU_NO_ACCESS = 0,
		CPU_READ = 0x01,		//	staging readable texture
		CPU_WRITE = 0x02,		//	staging writable texture
		CPU_DYNAMIC = 0x04,		//	dynamic texture
	};

	//	Mipmap info
	struct MIP_INFO
	{
		int		iWidth;
		int		iHeight;
		int		iRowPitch;
		int		iSlicePitch;
	};

	//	Texture desc
	struct TEX_DESC
	{
		int				width;			//	texture width
		int				height;			//	texture height
		DXGI_FORMAT		fmt;			//	texture format
		int				iMipLevels;		//	number of mipmap levels
		int				iArraySize;		//	array size
		bool			bCubeMap;		//	true, this is a cube map

		MIP_INFO*		aMips;			//	Mip info

		TEX_DESC();
		~TEX_DESC();
	};

	//	Update info used by Update()
	struct UPDATE_INFO
	{
		void*	pPixels;		//	Pixel data
		int		iArrayIdx;		//	Index in array, [0, 5] for cubemap
		int		iMipLevel;		//	Mip level index

		UPDATE_INFO();
	};

	//	Lock structure used by Map method
	struct LOCK_INFO
	{
		void*	pData;
		auint32	uRowPitch;
	};

public:		//	Operations

public:		//	Operations

	//	Make texture id from file name
	static auint32 MakeID(const char* szFile);
	//	Normalize texture file name
	static void NormalizeFileName(const char* szFile, char* szOutFile);
	static void NormalizeFileName(const char* szFile, AString& strOutFile);
	//	Create a noise texture for Grand Terrain
	static void CreateNoiseMap(const wchar_t* szFile);

	//	Update content for texture, this function can't be used on textures created with CPU_DYNAMIC flag
	//	aInfos: update info array
	//	iNumInfo: count of info stored in aInfos
	bool Update(UPDATE_INFO* aInfos, int iNumInfo);
	//	Copy content from source texture with same size, format and mipmap level
	//	iDstArrayIndex: destination array index in this texture
	//	pSrc: source texture
	//	iSrcArrayIndex: source array index in source texture
	//	mip: the mip level will be copied. -1 means to copy all mips;
	void CopyFrom(int iDstArrayIndex, CDXTexture* pSrc, int iSrcArrayIndex, int mip=-1);

	//	Apply texture to shader
	void ApplyToPS(int iStartSlot);
	void ApplyToCS(int iStartSlot);

	//	Lock texture for CPU read/write
	//	This function can be used for texture created with CPU_DYNAMIC flag
	bool Lock(int iArrayIndex, int iMipLevel, LOCK_INFO& info);
	//	Unlock texture
	void Unlock(int iArrayIndex, int iMipLevel);

	//	Output texture to file
	//	bDDS: true, save as dds, otherwise as tga
	bool SaveToFile(const char* szFile, bool bDDS);

	//	Get file id
	auint32 GetID() const { return m_id; }
	//	Get texture ID string. If texture was created from a disk file, 
	//	the string is file name relative to work dir
	const char* GetFileName() const { return m_strFile; }
	//	Get texture desc
	const TEX_DESC& GetDesc() const { return m_desc; }

	ID3D11Texture2D* GetDXTex() const { return m_pDXTex; }

protected:

	//	All CDXTexture object should be created and released by CDXTextureMan
	CDXTexture();
	virtual ~CDXTexture();

	//	Create a texture without filling content
	//	szID: id string
	//	iMipLevels: the maximum number of mipmap levels in the texture. use 0 to generate a full set of subtextures
	//	uCPUAccess: CPU access flags. CDXTexture::CPU_xxx
	bool Init(const char* szID, int width, int height, DXGI_FORMAT fmt, bool bCubeMap, int iMipLevels = 0, auint32 uCPUAccess = 0);
	//	Initialize as texture array
	//	szID: id string
	//	iArraySize: texture array size
	//	iMipLevels: the maximum number of mipmap levels in the texture, use 0 to generate a full set of subtextures
	bool InitArray(const char* szID, int width, int height, DXGI_FORMAT fmt, int iArraySize, int iMipLevels = 0);
	//	Initialize texture from file
	//	iMipLevels: the maximum number of mipmap levels in the texture, 0 means to use the number stored in file
	bool InitFromFile(const char* szFile, int iMipLevels = 0);
	//	Initialize texture from a color render target
	//	iRTIndex: color target index
	bool InitFromRT(const char* szID, CDXRenderTarget* pRT, int iRTIndex);
	//	Initialize as a very small pure color texture
	bool Init(const char* szID, A3DCOLOR color);
	//	Release object
	void Release();

protected:	//	Attributes

	AString			m_strFile;		//	texture ID string. If texture was created from a disk file, the string contains file name relative to work dir
	auint32			m_id;			//	texture id
	TEX_DESC		m_desc;			//	texture desc
	bool			m_bFromRT;		//	true, this texture is created from render target
	auint32			m_uCPUAccess;	//	CPU access flags

	ID3D11Texture2D*			m_pDXTex;		//	DX texture object
	ID3D11ShaderResourceView*	m_psrv;			//	texture view
	
protected:	//	Operations

	//	Create empty texture
	//	iArraySize: > 0, texture array size; 0 means cubemap
	bool Create(int width, int height, DXGI_FORMAT fmt, int iArraySize, int iMipLevels);
	//	Load from DDS file
	bool LoadFromDDSFile(const char* szFile, int iMipLevels);
	//	Create texture view
	bool CreateTextureView();
};

#endif	//	_DXTEXTURE_H_

