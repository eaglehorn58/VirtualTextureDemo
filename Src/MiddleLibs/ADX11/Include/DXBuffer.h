//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#pragma once

#include <d3d11.h>
#include "ATypes.h"
#include "DXTypes.h"

///////////////////////////////////////////////////////////////////////////

class CDXBuffer
{
public:		//	Types

	//	CPU access flags
	enum
	{
		CPU_NO_ACCESS = 0,
		CPU_READ = 0x01,		//	staging readable texture
		CPU_WRITE = 0x02,		//	staging writable texture
		CPU_DYNAMIC = 0x04,		//	dynamic texture
	};

public:		//	Constructors and Destructors

	CDXBuffer();
	virtual ~CDXBuffer();

public:		//	Attributes

public:		//	Operations

	//	Initialize as structured buffer
	bool Init(int iElemCnt, int iElemSize, const void* pInitData, auint32 uCPUAccess);
	//	Initialize as raw buffer
	bool InitRaw(int iElemCnt, const void* pInitData, auint32 uCPUAccess);
	//	Release
	void Release();

	//	Update content for buffer, this function can't be used on buffers created with CPU_DYNAMIC flag
	bool Update(const void* pSrc);
	//	Lock buffer for CPU read/write
	//	This function can be used for buffer created with CPU_DYNAMIC flag
	void* Lock();
	//	Unlock texture
	void Unlock();

	//	Apply SRV to shader
	void ApplyToPS(int iSlot);
	void ApplyToCS(int iSlot);

	ID3D11Buffer* GetBuf() const { return m_pBuf; }
	int GetElemNum() const { return m_iElemNum; }
	int GetElemSize() const { return m_iElemSize; }

	//	Get interface
	ID3D11ShaderResourceView* GetSRV() { return m_psrv; }

protected:	//	Attributes

	ID3D11Buffer*				m_pBuf;		//	DX buffer
	ID3D11ShaderResourceView*	m_psrv;		//	Shader resource view

	int			m_iBufType;		//	buffer type
	int			m_iElemNum;		//	element count
	int			m_iElemSize;	//	element size
	auint32		m_uCPUAccess;	//	CPU access flags

protected:	//	Operations

	//	Create buffer
	bool Create(int iElemCnt, int iElemSize, const void* pInitData);
	//	Create shader resource view
	bool CreateSRV();
};

///////////////////////////////////////////////////////////////////////////
//
//	Class CDXUABuffer
//
///////////////////////////////////////////////////////////////////////////

class CDXUABuffer
{
public:		//	Types

public:		//	Constructors and Destructors

	CDXUABuffer();
	virtual ~CDXUABuffer();

public:		//	Attributes

public:		//	Operations

	//	Initialize as structured buffer
	bool Init(int iElemCnt, int iElemSize);
	//	Initialize as raw buffer
	//	iElemCnt: count of uint
	bool InitRaw(int iElemCnt);
	//	Release
	void Release();

	//	Update buffer
	//	iDstOffset: offset in bytes in dest buffer
	//	iSrcOffset: offset in bytes in source buffer
	//	iBytes: count of bytes
	bool CopyFrom(CDXBuffer* pSrcBuf);
	bool CopyFromRegion(int iDstOffset, CDXBuffer* pSrcBuf, int iSrcOffset, int iBytes);
	//	Copy out buffer content
	//	iStart: start position in buffer in bytes
	//	iBytes: count of bytes
	bool CopyOut(CDXBuffer* pDstBuf);
	bool CopyOutRegion(CDXBuffer* pDstBuf, int iDstOffset, int iSrcOffset, int iBytes);

	//	Apply to shader
	void ApplySRVToPS(int iSlot);
	void ApplySRVToCS(int iSlot);
	void ApplyUAVToCS(int iSlot);

	int GetElemNum() const { return m_iElemNum; }
	int GetElemSize() const { return m_iElemSize; }

	//	Get interface
	ID3D11ShaderResourceView* GetSRV() { return m_psrv; }
	ID3D11UnorderedAccessView* GetUAV() { return m_puav; }

protected:	//	Attributes

	ID3D11Buffer*				m_pBuf;		//	DX buffer
	ID3D11ShaderResourceView*	m_psrv;		//	Shader resource view
	ID3D11UnorderedAccessView*	m_puav;		//	Unordered access view

	int		m_iBufType;		//	buffer type
	int		m_iElemNum;		//	element count
	int		m_iElemSize;	//	element size

protected:	//	Operations

	//	Create buffer
	bool Create(int iElemCnt, int iElemSize);
};

///////////////////////////////////////////////////////////////////////////
//
//	Class CDXUABufferSuit
//
///////////////////////////////////////////////////////////////////////////

class CDXUABufferSuit
{
public:		//	Types

public:		//	Constructors and Destructors

	CDXUABufferSuit();
	virtual ~CDXUABufferSuit();

public:		//	Attributes

public:		//	Operations

	//	Initialize as structured buffer
	bool Init(int iElemCnt, int iElemSize);
	//	Initialize as raw buffer
	//	iElemCnt: count of uint
	bool InitRaw(int iElemCnt);
	//	Release
	void Release();

	//	Update cpu buffer content to uav buffer
	//	iStart: start position in buffer in bytes, -1 means to submit whole buffer
	//	iBytes: count of bytes to be submitted
	void Update(int iStart, int iBytes);
	//	Extract uav buffer content to cpu buffer, -1 means to extract whole buffer
	//	iStart: start position in buffer in bytes
	//	iBytes: count of bytes to be extracted
	void Extract(int iStart, int iBytes);

	//	Apply UAV to CS
	void ApplySRVToPS(int iSlot);
	void ApplySRVToCS(int iSlot);
	void ApplyUAVToCS(int iSlot);

	//	get element info
	int GetElemNum() const { return m_iElemNum; }
	int GetElemSize() const { return m_iElemSize; }
	//	Get cpu buffer
	abyte* GetCPUBuf() { return m_pMemBuf; }

	//	Get interface
	ID3D11ShaderResourceView* GetSRV() { return m_pUAVBuf->GetSRV(); }
	ID3D11UnorderedAccessView* GetUAV() { return m_pUAVBuf->GetUAV(); }

protected:	//	Attributes

	CDXUABuffer*	m_pUAVBuf;		//	uav buffer
	CDXBuffer*		m_pStageBuf;	//	stage buffer
	abyte*			m_pMemBuf;		//	cpu buffer

	int		m_iBufType;		//	buffer type
	int		m_iElemNum;		//	element count
	int		m_iElemSize;	//	element size

protected:	//	Operations

	//	Create buffer
	bool Create(int iElemCnt, int iElemSize);
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


