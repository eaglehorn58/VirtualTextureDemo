/*	Copyright(c) 2020, Andy Do, All Rights Reserved.
	Contact: eaglehorn58@gmail.com, eaglehorn58@163.com
*/

#pragma once

#include <d3d11.h>
#include "AXTypes.h"
#include <vector>

///////////////////////////////////////////////////////////////////////////

class CDXTexture;

///////////////////////////////////////////////////////////////////////////
//	Class CDXUATexture
///////////////////////////////////////////////////////////////////////////

class CDXUATexture
{
public:		//	Types

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

public:		//	Operations

	CDXUATexture();
	virtual ~CDXUATexture();

	//	Create a texture without filling content
	//	iMipLevels: the maximum number of mipmap levels in the texture. use 0 to generate a full set of subtextures
	bool Init(int width, int height, DXGI_FORMAT fmt, bool bCubeMap, int iMipLevels = 0);
	//	Initialize as texture array
	//	szID: id string
	//	iArraySize: texture array size
	//	iMipLevels: the maximum number of mipmap levels in the texture, use 0 to generate a full set of subtextures
	bool InitArray(int width, int height, DXGI_FORMAT fmt, int iArraySize, int iMipLevels = 0);
	//	Release object
	void Release();

	//	Copy content from source texture with same size, format and mipmap level
	//	iDstArrayIndex: destination array index in this texture
	//	pSrc: source texture
	//	iSrcArrayIndex: source array index in source texture
	//	mip: the mip level will be copied. -1 means to copy all mips;
	void CopyFrom(int iDstArrayIndex, CDXTexture* pSrc, int iSrcArrayIndex, int mip = -1);
	//	Copy out content to specified texture
	void CopyOut(int iDstArrayIndex, CDXTexture* pDst, int iSrcArrayIndex, int mip = -1);

	//	Apply texture to shader
	void ApplySRVToPS(int iStartSlot);
	void ApplySRVToCS(int iStartSlot);
	void ApplyUAVToCS(int iStartSlot, int iSlotNum, int iStartMip);

	//	Output texture to file
	bool SaveToFile(const char* szFile, bool bDDS);

	//	Get texture desc
	const TEX_DESC& GetDesc() const { return m_desc; }

	ID3D11Texture2D* GetDXTex() const { return m_pDXTex; }

protected:	//	Attributes

	typedef std::vector<ID3D11UnorderedAccessView*> UAVVector;

	TEX_DESC	m_desc;		//	texture desc

	ID3D11Texture2D*			m_pDXTex;		//	DX texture object
	ID3D11ShaderResourceView*	m_psrv;			//	texture view
	UAVVector					m_uavs;			//	UAVs for all mips
	
protected:	//	Operations

	//	Create empty texture
	//	iArraySize: > 0, texture array size; 0 means cubemap
	bool Create(int width, int height, DXGI_FORMAT fmt, int iArraySize, int iMipLevels);
	//	Create texture view
	bool CreateTextureView();
};


