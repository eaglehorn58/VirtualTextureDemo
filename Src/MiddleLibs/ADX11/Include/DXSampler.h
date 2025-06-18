//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com

#ifndef _DXSAMPLER_H_
#define _DXSAMPLER_H_

#include <d3d11.h>
#include "AXTypes.h"

///////////////////////////////////////////////////////////////////////////

class CDXSampler
{
public:		//	Types

	//	Filter
	enum FILTER
	{
		FILTER_POINT = 0,
		FILTER_LINEAR,
		FILTER_ANISOTROPIC,
	};

	//	Address
	enum ADDRESS
	{
		ADDR_WRAP,
		ADDR_MIRROR,
		ADDR_CLAMP,
		ADDR_BORDER,
	};

	struct SAMPLER_STATE
	{
		D3D11_SAMPLER_DESC	sd;

		SAMPLER_STATE();
	};

public:		//	Constructors and Destructors

	CDXSampler();
	CDXSampler(const SAMPLER_STATE& s);
	CDXSampler(FILTER filter, ADDRESS addr, int iAnisotropy=1);
	virtual ~CDXSampler();

public:		//	Attributes

public:		//	Operations

	//	Apply sampler to device
	//	slot: sampler slot
	void ApplyToPS(int slot);
	void ApplyToVS(int slot);
	void ApplyToCS(int slot);

protected:	//	Attributes

	ID3D11SamplerState*		m_pSampler;	//	DX sampler

protected:	//	Operations
	
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_DXSAMPLER_H_

