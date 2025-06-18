/*
 * FILE: DXVertexShader.h
 *
 * CREATED BY: duyuxin, 2018/10/17
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _DXVERTEXSHADER_H_
#define _DXVERTEXSHADER_H_

#include <d3d11.h>

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

class CDXVertexShaderMate;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CDXVertexShader
//
///////////////////////////////////////////////////////////////////////////

class CDXVertexShader
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

	CDXVertexShader();
	virtual ~CDXVertexShader();

public:		//	Attributes

public:		//	Operations

	//	Initialize
	//	szFile: vertex shader file
	//	szFunc: name of the shader-entry point function
	//	pMate: a mate object that match this shader
	bool Init(const char* szFile, const char* szFunc, CDXVertexShaderMate* pMate);
	//	Release
	void Release();

	//	Update shader parameters from mate to buffer, and commit buffer to device context.
	//	slot: index of constant slot whose data will be updated to shader. -1 means to update all slots
	bool UpdateParams(CDXVertexShaderMate* pMate, int slot);
	//	Apply shader to device
	bool Apply();

	//	Get constant buffer number
	int GetCBufNum() const { return m_iCBufNum; }

protected:	//	Attributes

	ID3D11VertexShader*		m_pShader;				//	DX vertex shader
	ID3D11InputLayout*		m_pLayout;				//	DX layout

	CONST_BUF	m_aCBufs[MAX_CB_NUM];	//	Constant buffers
	int			m_iCBufNum;				//	Constant buffer number

protected:	//	Operations

	//	Check constant buffer size
	bool CheckConstBufferSize(const void* pShaderBuf, int iShaderBufSize, CDXVertexShaderMate* pMate);
	//	Update constant buffer from mate
	//	Note: this function only copy data from mate to buffer, but not commit to device context !
	bool UpdateConstBuffer(CDXVertexShaderMate* pMate, int slot);
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_DXVERTEXSHADER_H_

