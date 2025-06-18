//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com

#pragma once

#include <d3d11.h>

///////////////////////////////////////////////////////////////////////////

class CDXComputeShaderMate;

///////////////////////////////////////////////////////////////////////////

class CDXComputeShader
{
public:		//	Types

	enum
	{
		MAX_CB_NUM = D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT,
	};

	struct CONST_BUF
	{
		ID3D11Buffer*	pCBuf;		//	DX constant buffer
		int				iCBLen;		//	Constant buffer length
	};

public:		//	Constructors and Destructors

	CDXComputeShader();
	virtual ~CDXComputeShader();

public:		//	Attributes

public:		//	Operations

	//	Initialize
	//	szFile: compute shader file
	//	szFunc: name of the shader-entry point function
	//	pMate: a mate object that match this shader
	bool Init(const char* szFile, const char* szFunc, CDXComputeShaderMate* pMate);
	//	Release
	void Release();

	//	Update shader parameters from mate to buffer, and commit buffer to device context.
	//	slot: index of constant slot whose data will be updated to shader. -1 means to update all slots
	bool UpdateParams(CDXComputeShaderMate* pMate, int slot);
	//	Apply shader to device
	bool Apply();
	//	Dispatch compute shader
	bool Dispatch(int group_x, int group_y, int group_z);

protected:	//	Attributes

	ID3D11ComputeShader*	m_pShader;		//	DX computer shader

	CONST_BUF	m_aCBufs[MAX_CB_NUM];	//	Constant buffers
	int			m_iCBufNum;				//	Constant buffer number

protected:	//	Operations

	//	Check constant buffer size
	bool CheckConstBufferSize(const void* pShaderBuf, int iShaderBufSize, CDXComputeShaderMate* pMate);
	//	Update constant buffer from mate
	//	Note: this function only copy data from mate to buffer, but not commit to device context !
	bool UpdateConstBuffer(CDXComputeShaderMate* pMate, int slot);
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////



