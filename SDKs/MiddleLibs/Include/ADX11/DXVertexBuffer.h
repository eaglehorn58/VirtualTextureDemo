/*
 * FILE: DXVertexBuffer.h
 *
 * CREATED BY: duyuxin, 2018/10/17
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _DXVERTEXBUFFER_H_
#define _DXVERTEXBUFFER_H_

#include <d3d11.h>
#include "ATypes.h"

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


///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CDXVertexBuffer
//
///////////////////////////////////////////////////////////////////////////

class CDXVertexBuffer
{
public:		//	Types

	//	Flags
	enum
	{
		FLAG_STATIC		= 0,		//	Static buffer
		FLAG_DYNAMIC	= 1,		//	Dynamic buffer
	};

public:		//	Constructors and Destructors

	CDXVertexBuffer();
	virtual ~CDXVertexBuffer();

public:		//	Attributes

public:		//	Operations

	//	Initialize
	//	iVertSize: bytes of one vertex
	//	iNumVert: number of vertex
	//	uFlags: FLAG_xxx flags
	//	pInitData: initialize data buffer, can be NULL
	bool Init(int iVertSize, int iNumVert, auint32 uFlags, const void* pInitData);
	//	Release
	void Release();

	//	Lock buffer
	void* Lock();
	//	Unlock buffer
	void Unlock();

	//	Apply buffer to device
	bool Apply();

	int GetVertSize() const { return m_iVertSize; }
	int	GetVertNum() const { return m_iNumVert; }
	
protected:	//	Attributes

	ID3D11Buffer*	m_pBuf;			//	DX buffer
	int				m_iVertSize;	//	bytes of each vertex
	int				m_iNumVert;		//	number of vertex

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Class CDXIndexBuffer
//
///////////////////////////////////////////////////////////////////////////

class CDXIndexBuffer
{
public:		//	Types

	//	Flags
	enum
	{
		FLAG_STATIC = 0,		//	Static buffer
		FLAG_DYNAMIC = 1,		//	Dynamic buffer
	};

public:		//	Constructors and Destructors

	CDXIndexBuffer();
	virtual ~CDXIndexBuffer();

public:		//	Attributes

public:		//	Operations

	//	Initialize
	//	iIndexSize: bytes of each index. 2 or 4
	//	iNumIndex: number of index
	//	uFlags: FLAG_xxx flags
	//	pInitData: initialize data buffer, can be NULL
	bool Init(int iIndexSize, int iNumIndex, auint32 uFlags, const void* pInitData, D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//	Release
	void Release();

	//	Lock buffer
	void* Lock();
	//	Unlock buffer
	void Unlock();

	//	Apply buffer to device
	bool Apply();

	int GetIndexSize() const { return m_iIndexSize; }
	int	GetIndexNum() const { return m_iNumIndex; }

protected:	//	Attributes

	ID3D11Buffer*	m_pBuf;			//	DX buffer
	int				m_iIndexSize;	//	bytes of each index
	int				m_iNumIndex;	//	number of index

	D3D_PRIMITIVE_TOPOLOGY	m_Topology;

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_DXVERTEXBUFFER_H_

