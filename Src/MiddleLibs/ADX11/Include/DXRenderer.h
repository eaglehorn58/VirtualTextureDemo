//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#ifndef _DXRENDERER_H_
#define _DXRENDERER_H_

#include <Windows.h>
#include <d3d11.h>
#include "vector.h"

///////////////////////////////////////////////////////////////////////////

class AXViewport;
class CDXTextureMan;
class CDXRenderState;
class IDXRTResource;

///////////////////////////////////////////////////////////////////////////

class DXRenderer
{
public:		//	Types

	//	Render targets clear flags
	enum
	{
		CLEAR_COLOR = 0x01,
		CLEAR_DEPTH = 0x02,
		CLEAR_STENCIL = 0x04,
	};

public:		//	Attributes

public:		//	Operations

	//	Initialize instance
	static bool InitInstance(HWND hWnd);
	//	Release instance
	static void ReleaseInstance();
	//	Get instance
	static DXRenderer* Instance();

	//	Resize device
	bool ResizeDevice(int width, int height);
	//	Push viewport
	void PushViewport(AXViewport* pViewport);
	//	Pop viewport
	void PopViewport(AXViewport* pViewport);
	//	Get current viewport
	AXViewport* GetCurViewport();

	//	Begin render
	bool BeginRender();
	//	End render
	void EndRender();

	//	Clear vertex shader
	void ClearVertexShader();
	//	Clear pixel shader
	void ClearPixelShader();

	//	Set wire frame mode
	void SetWireFrameMode(bool bWireFrame);
	//	Is in wire frame mode ?
	bool IsInWireFrameMode() const { return m_bWireFrame; }

	//	Push render target
	//	pRTRes: render target resources
	//	uClearFlags: combinations of DXRenderer::CLEAR_xxx
	void PushRenderTarget(IDXRTResource* pRTRes, auint32 uClearFlags);
	//	Pop render target
	void PopRenderTarget(IDXRTResource* pRTRes);

	//	clear (unbind) compute shader SRVs
	void ClearCS_SRVs(int iStartSlot, int iSlotNum);
	//	clear (unbind) compute shader UAVs
	void ClearCS_UAVs(int iStartSlot, int iSlotNum);

	//	Has renderer been initilaized successfully ?
	bool IsInitOk() const { return m_bInitOk; }
	//	Get interfaces
	ID3D11Device* GetDevice() { return m_pDevice; }
	ID3D11DeviceContext* GetDeviceContext() { return m_pDevContext; }
	CDXTextureMan* GetTextureMan() { return m_pTexMan; }
	ID3D11Texture2D* GetBackBuffer() { return m_pBackBuf; }
	ID3D11RenderTargetView* GetBackBufferView() { return m_pBackBufView; }
	ID3D11DepthStencilView* GetDepthView() { return m_pDepthView; }
	//	Get current screen size
	int GetScreenWidth() const { return m_iScnWidth; }
	int GetScreenHeight() const { return m_iScnHeight; }

private:	//	Constructors and Destructors

	DXRenderer();
	virtual ~DXRenderer();

	bool Init(HWND hWnd);
	void Release();

protected:	//	Attributes

	HWND		m_hWnd;
	bool		m_bInitOk;		//	true, renderer has been initilaized successfully !
	bool		m_bWireFrame;	//	true, is in wire frame mode

	int			m_iScnWidth;	//	current screen width
	int			m_iScnHeight;	//	current screen height

	ID3D11Device*				m_pDevice;
	ID3D11DeviceContext*		m_pDevContext;
	IDXGISwapChain*				m_pSwapChain;
	ID3D11Texture2D*			m_pBackBuf;
	ID3D11RenderTargetView* 	m_pBackBufView;
	ID3D11Texture2D*			m_pDepthBuf;
	ID3D11DepthStencilView* 	m_pDepthView;

	CDXTextureMan*		m_pTexMan;		//	Texture manager

	abase::vector<AXViewport*>		m_ViewStack;	//	viewport stack
	abase::vector<IDXRTResource*>	m_RTStack;		//	render target stack

protected:	//	Operations
	
	//	Initialize device
	bool InitDevice();
	//	Create back buffer and depth-stencil buffer
	bool CreateBackBuffers(int width, int height);
	//	Apply viewport to device
	void ApplyViewport(AXViewport* pViewport);
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_DXRENDERER_H_

