/*
 * FILE: DXRenderTarget.h
 *
 * CREATED BY: duyuxin, 2018/10/17
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _DXRENDERTARGET_H_
#define _DXRENDERTARGET_H_

#include <d3d11.h>
#include "AXTypes.h"

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

class CDXUABuffer;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	interface IDXRTResource
//
///////////////////////////////////////////////////////////////////////////

class IDXRTResource
{
public :

	enum
	{
		MAX_SIM_RT_NUM = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT,
	};

	friend class DXRenderer;

protected:

	//	Apply to device
	//	uClearFlags: combinations of DXRenderer::CLEAR_xxx
	virtual void Apply(auint32 uClearFlags) = 0;
};

///////////////////////////////////////////////////////////////////////////
//
//	Class CDXRenderTarget
//
///////////////////////////////////////////////////////////////////////////

class CDXRenderTarget : public IDXRTResource
{
public:		//	Types

	friend class DXRenderer;
	friend class CDXRTSuite;

	//	color render target
	struct COLOR_RT
	{
		ID3D11Texture2D*			pRT;
		ID3D11RenderTargetView*		pRTView;

		DXGI_FORMAT		fmt;		//	format
		A3DCOLORVALUE	color;		//	clear color
	};

public:

	CDXRenderTarget();
	virtual ~CDXRenderTarget();

public:		//	Operations

	//	Initialize object
	//	width, height: size of render targets
	//	iNumRT: number of simultaneous render targets.
	//	aFmts: color buffer format array contains iNumRT formats
	//	fmtDepth: depth buffer format. DXGI_FORMAT_UNKNOWN means don't bind depth buffer
	bool Init(int width, int height, int iNumRT, DXGI_FORMAT* aFmts, DXGI_FORMAT fmtDepth);
	//	Release object
	void Release();

	//	Resize render target
	bool Resize(int width, int height);

	//	Set clear color
	void SetClearColor(int slot, const A3DCOLORVALUE& color);
	//	Set clear depth value
	void SetClearDepth(float val) { m_fClrDepth = val; }
	//	Set clear stencil value
	void SetClearStencil(abyte val) { m_uClrStencil = val; }

	//	Get size of render target
	int GetWidth() const { return m_iWidth; }
	int GetHeight() const { return m_iHeight; }
	//	Get number of color targets
	int GetColorRTNum() const { return m_iNumRT; }
	//	Get color render target
	const COLOR_RT& GetColorRT(int slot);

	//	Get interfaces
	ID3D11DepthStencilView* GetDepthView() { return m_pDepthView; }

protected:	//	Attributes

	int			m_iWidth;		//	width of render target
	int			m_iHeight;		//	height of render target
	int			m_iNumRT;		//	Number of render targets
	DXGI_FORMAT	m_fmtDepth;		//	depth format
	float		m_fClrDepth;	//	clear depth value
	abyte		m_uClrStencil;	//	clear stencil value

	COLOR_RT					m_aColorRTs[MAX_SIM_RT_NUM];	//	color targets
	ID3D11Texture2D*			m_pDepthBuf;					//	depth buffer
	ID3D11DepthStencilView*		m_pDepthView;					//	depth buffer view
	
protected:	//	Operations

	//	Create a color target
	bool CreateColorTarget(int slot);
	//	Create depth target
	bool CreateDepthBuffer();
	//	Release render target objects and views
	void ReleaseObjectsAndViews();
	//	Apply to device as render targets
	//	uClearFlags: combinations of DXRenderer::CLEAR_xxx
	virtual void Apply(auint32 uClearFlags) override;
	//	Clear render targets
	void ClearTargets(auint32 uClearFlags);
};

///////////////////////////////////////////////////////////////////////////
//
//	Class CDXRTCombo
//
///////////////////////////////////////////////////////////////////////////

class CDXRTSuite : public IDXRTResource
{
public:		//	Types

	friend class DXRenderer;

public:

	CDXRTSuite(CDXRenderTarget* pRT, CDXUABuffer** aUABufs, int iUABufNum);
	virtual ~CDXRTSuite();

public:		//	Operations

protected:	//	Attributes

	CDXRenderTarget*	m_pRT;
	CDXUABuffer*		m_aUABufs[MAX_SIM_RT_NUM];
	int					m_iUABufNum;

protected:	//	Operations

	//	Apply to device as render targets
	//	uClearFlags: combinations of DXRenderer::CLEAR_xxx
	virtual void Apply(auint32 uClearFlags) override;
};


#endif	//	_DXRENDERTARGET_H_

