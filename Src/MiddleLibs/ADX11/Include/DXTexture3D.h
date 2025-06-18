//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#pragma once

#include <d3d11.h>
#include "AXTypes.h"
#include <vector>

///////////////////////////////////////////////////////////////////////////

class CDXTexture;

///////////////////////////////////////////////////////////////////////////

class CDXTexture3D
{
public:		//	Types

	//	cpu access flags
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
		int		iDepth;
		int		iRowPitch;
		int		iSlicePitch;
	};

	//	Texture desc
	struct TEX_DESC
	{
		int				width;			//	texture width
		int				height;			//	texture height
		int				depth;			//	texture depth
		DXGI_FORMAT		fmt;			//	texture format
		int				iMipLevels;		//	number of mipmap levels

		MIP_INFO*		aMips;			//	Mip info

		TEX_DESC();
		~TEX_DESC();
	};

public:		//	Operations

	CDXTexture3D();
	virtual ~CDXTexture3D();

	//	iMipLevels: the maximum number of mipmap levels in the texture. use 0 to generate a full set of subtextures
	bool Init(int width, int height, int depth, DXGI_FORMAT fmt, int iMipLevels = 0, auint32 uCPUAccess = 0);
	//	initialize as UAV texture
	bool InitUAV(int width, int height, int depth, DXGI_FORMAT fmt, int iMipLevels = 0);
	//	Release object
	void Release();

	//	Copy content from source texture with same size, format and mipmap level
	//	pSrc: source texture
	//	mip: the mip level will be copied. -1 means to copy all mips;
	void CopyFrom(CDXTexture3D* pSrc, int mip = -1);
	//	Copy out content to specified texture
	void CopyOut(CDXTexture3D* pDst, int mip = -1);

	//	Apply texture to shader
	void ApplySRVToPS(int iStartSlot);
	void ApplySRVToCS(int iStartSlot);
	void ApplyUAVToCS(int iStartSlot, int iSlotNum, int iStartMip);

	//	Get texture desc
	const TEX_DESC& GetDesc() const { return m_desc; }

	ID3D11Texture3D* GetDXTex() const { return m_pDXTex; }
	ID3D11UnorderedAccessView* GetUAV(int n) { return m_uavs[n]; }

protected:	//	Attributes

	typedef std::vector<ID3D11UnorderedAccessView*> UAVVector;

	TEX_DESC	m_desc;			//	texture desc
	auint32		m_uCPUAccess;	//	CPU access flags
	bool		m_bUAV;			//	UAV flag

	ID3D11Texture3D*			m_pDXTex;		//	DX texture object
	ID3D11ShaderResourceView*	m_psrv;			//	texture view
	UAVVector					m_uavs;			//	UAVs for all mips
	
protected:	//	Operations

	//	Create texture
	bool Create(int width, int height, int depth, DXGI_FORMAT fmt, int iMipLevels);
	//	Create texture view
	bool CreateTextureView();
};


