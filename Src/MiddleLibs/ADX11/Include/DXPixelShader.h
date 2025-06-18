//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#ifndef _DXPIXELSHADER_H_
#define _DXPIXELSHADER_H_

#include <d3d11.h>

///////////////////////////////////////////////////////////////////////////

class CDXPixelShaderMate;

///////////////////////////////////////////////////////////////////////////

class CDXPixelShader
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

	CDXPixelShader();
	virtual ~CDXPixelShader();

public:		//	Attributes

public:		//	Operations

	//	Initialize
	//	szFile: pixel shader file
	//	szFunc: name of the shader-entry point function
	//	pMate: a mate object that match this shader
	bool Init(const char* szFile, const char* szFunc, CDXPixelShaderMate* pMate);
	//	Release
	void Release();

	//	Update shader parameters from mate to buffer, and commit buffer to device context.
	//	slot: index of constant slot whose data will be updated to shader. -1 means to update all slots
	bool UpdateParams(CDXPixelShaderMate* pMate, int slot);
	//	Apply shader to device
	bool Apply();

protected:	//	Attributes

	ID3D11PixelShader*		m_pShader;			//	DX pixel shader

	CONST_BUF	m_aCBufs[MAX_CB_NUM];	//	Constant buffers
	int			m_iCBufNum;				//	Constant buffer number

protected:	//	Operations

	//	Check constant buffer size
	bool CheckConstBufferSize(const void* pShaderBuf, int iShaderBufSize, CDXPixelShaderMate* pMate);
	//	Update constant buffer from mate
	//	Note: this function only copy data from mate to buffer, but not commit to device context !
	bool UpdateConstBuffer(CDXPixelShaderMate* pMate, int slot);
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_DXPIXELSHADER_H_

