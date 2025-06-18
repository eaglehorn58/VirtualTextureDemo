//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#include "DXRenderer.h"
#include "AXTypes.h"
#include "ALog.h"
#include "A3DMacros.h"
#include "AXViewport.h"
#include "DXTextureMan.h"
#include "DXRenderTarget.h"
#include "DXRenderState.h"
#include <d3dcommon.h>

///////////////////////////////////////////////////////////////////////////

DXRenderer* g_pRendererInst = nullptr;

///////////////////////////////////////////////////////////////////////////

DXRenderer::DXRenderer() :
m_hWnd(NULL),
m_bInitOk(false),
m_bWireFrame(false),
m_iScnWidth(0),
m_iScnHeight(0),
m_pDevice(NULL),
m_pDevContext(NULL),
m_pSwapChain(NULL),
m_pBackBuf(NULL),
m_pBackBufView(NULL),
m_pDepthBuf(NULL),
m_pDepthView(NULL),
m_pTexMan(NULL)
{
}

DXRenderer::~DXRenderer()
{
}

bool DXRenderer::InitInstance(HWND hWnd)
{
	if (g_pRendererInst)
		return true;

	DXRenderer* pInst = new DXRenderer();
	if (!pInst->Init(hWnd))
	{
		return false;
	}

	g_pRendererInst = pInst;

	return true;
}

void DXRenderer::ReleaseInstance()
{
	if (g_pRendererInst)
	{
		g_pRendererInst->Release();
		delete g_pRendererInst;
		g_pRendererInst = nullptr;
	}
}

DXRenderer* DXRenderer::Instance()
{
	return g_pRendererInst;
}

bool DXRenderer::Init(HWND hWnd)
{
	m_hWnd = hWnd;

	//	Initialize device
	if (!InitDevice())
	{
		a_OutputLog("DXRenderer::Init, failed to call InitDevice() !");
		return false;
	}

	//	Create texture manager
	m_pTexMan = new CDXTextureMan;
	if (!m_pTexMan || !m_pTexMan->Init())
	{
		a_OutputLog("DXRenderer::Init, failed to create texture manager !");
		return false;
	}

	m_bInitOk = true;

	return true;
}

void DXRenderer::Release()
{
	ASSERT(!m_RTStack.size());
	ASSERT(!m_ViewStack.size());

	A3DRELEASE(m_pTexMan);

	if (m_pDevContext)
	{
		m_pDevContext->ClearState();
	}

	A3DINTERFACERELEASE(m_pDepthView);
	A3DINTERFACERELEASE(m_pDepthBuf);
	A3DINTERFACERELEASE(m_pBackBufView);
	A3DINTERFACERELEASE(m_pBackBuf);
	A3DINTERFACERELEASE(m_pSwapChain);
	A3DINTERFACERELEASE(m_pDevContext);

	//	report d3d debug info
	if (m_pDevice)
	{
		ID3D11Debug* pD3DDebug = NULL;
		m_pDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<VOID**>(&pD3DDebug));
		if (pD3DDebug)
		{
			pD3DDebug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY);
			pD3DDebug->Release();
			pD3DDebug = NULL;
		}
	}

	A3DINTERFACERELEASE(m_pDevice);

	m_bInitOk = false;
}

bool DXRenderer::InitDevice()
{
	HRESULT hResult;

	//	Get window's size
	RECT rect;
	GetClientRect(m_hWnd, &rect);
	m_iScnWidth = rect.right - rect.left;
	m_iScnHeight = rect.bottom - rect.top;

	//	Setup swap chain properties
	DXGI_SWAP_CHAIN_DESC descSwapChain;
	memset(&descSwapChain, 0, sizeof(descSwapChain));

	descSwapChain.BufferCount = 1;
	descSwapChain.BufferDesc.Width = m_iScnWidth;
	descSwapChain.BufferDesc.Height = m_iScnHeight;
	descSwapChain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//	descSwapChain.BufferDesc.RefreshRate.Numerator = 60;
//	descSwapChain.BufferDesc.RefreshRate.Denominator = 1;
//	descSwapChain.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
//	descSwapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	descSwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	descSwapChain.OutputWindow = m_hWnd;
	descSwapChain.SampleDesc.Count = 1;
	descSwapChain.SampleDesc.Quality = 0;
	descSwapChain.Windowed = true;
	descSwapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	descSwapChain.Flags = D3D11_CREATE_DEVICE_DEBUG;

	D3D_FEATURE_LEVEL FeatureLevel = D3D_FEATURE_LEVEL_11_0;

	hResult = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&descSwapChain,
		&m_pSwapChain,
		&m_pDevice,
		&FeatureLevel,
		&m_pDevContext);

	if (FAILED(hResult))
	{
		a_OutputLog("DXRenderer::InitDevice, failed to call D3D11CreateDeviceAndSwapChain !");
		return false;
	}

	if (FeatureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		Release();
		a_OutputLog("DXRenderer::InitDevice, DX11 feature isn't supported !");
		return false;
	}

	//	Create back buffers
	if (!CreateBackBuffers(m_iScnWidth, m_iScnHeight))
	{
		Release();
		a_OutputLog("DXRenderer::InitDevice, failed to call CreateBackBuffers() !");
		return false;
	}

	return true;
}

bool DXRenderer::CreateBackBuffers(int width, int height)
{
	//	Get back buffer
	HRESULT hResult = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&m_pBackBuf);
	if (FAILED(hResult))
	{
		a_OutputLog("DXRenderer::CreateBackBuffers, failed to get back buffer !");
		return false;
	}

	//	Create render target view
	m_pDevice->CreateRenderTargetView(m_pBackBuf, NULL, &m_pBackBufView);

	//	Create depth-stencil buffer
	D3D11_TEXTURE2D_DESC descDepth;
	memset(&descDepth, 0, sizeof(descDepth));

	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	//	descDepth.CPUAccessFlags = 0;
	//	descDepth.MiscFlags = 0;

	hResult = m_pDevice->CreateTexture2D(&descDepth, NULL, &m_pDepthBuf);
	if (FAILED(hResult))
	{
		a_OutputLog("DXRenderer::CreateBackBuffers, failed to create depth-stencil buffer !");
		return false;
	}

	//	Create depth-stencil view
	m_pDevice->CreateDepthStencilView(m_pDepthBuf, NULL, &m_pDepthView);

	//	Set render target
	m_pDevContext->OMSetRenderTargets(1, &m_pBackBufView, m_pDepthView);

	return true;
}

bool DXRenderer::ResizeDevice(int width, int height)
{
	if (!m_bInitOk)
		return true;

	ASSERT(width > 0 && height > 0);

	//	Clear states
	m_pDevContext->ClearState();

	//	Release current buffer and views
	A3DINTERFACERELEASE(m_pDepthView);
	A3DINTERFACERELEASE(m_pDepthBuf);
	A3DINTERFACERELEASE(m_pBackBufView);
	A3DINTERFACERELEASE(m_pBackBuf);

	m_iScnWidth = width;
	m_iScnHeight = height;

	//	Resize the swap chain.
	HRESULT hResult = m_pSwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
	if (FAILED(hResult))
	{
		a_OutputLog("DXRenderer::ResizeDevice, Failed to resize swapchain buffer !");
		ASSERT(0);
		return false;
	}

	//	Re-create back buffers
	if (!CreateBackBuffers(width, height))
	{
		a_OutputLog("DXRenderer::ResizeDevice, failed to call CreateBackBuffers() !");
		return false;
	}

	//	Clear render state
	m_RTStack.clear();
	m_ViewStack.clear();

	return true;
}

void DXRenderer::PushViewport(AXViewport* pViewport)
{
	ASSERT(pViewport);
	ApplyViewport(pViewport);
	m_ViewStack.push_back(pViewport);
}

void DXRenderer::PopViewport(AXViewport* pViewport)
{
	ASSERT(pViewport && m_ViewStack.size());
	ASSERT(pViewport == m_ViewStack[m_ViewStack.size() - 1]);
	m_ViewStack.pop_back();

	if (m_ViewStack.size())
	{
		//	Restore previous viewport
		AXViewport* pPreViewport = m_ViewStack[m_ViewStack.size() - 1];
		ApplyViewport(pPreViewport);
	}
}

AXViewport* DXRenderer::GetCurViewport()
{
	if (m_ViewStack.size())
		return m_ViewStack[m_ViewStack.size() - 1];
	else
		return NULL;
}

void DXRenderer::ApplyViewport(AXViewport* pViewport)
{
	if (!m_pDevContext)
		return;

	const A3DVIEWPORTPARAM* pParams = pViewport->GetParam();

	D3D11_VIEWPORT view;
	view.Width = pParams->Width;
	view.Height = pParams->Height;
	view.MinDepth = pParams->MinZ;
	view.MaxDepth = pParams->MaxZ;
	view.TopLeftX = pParams->X;
	view.TopLeftY = pParams->Y;

	m_pDevContext->RSSetViewports(1, &view);
}

bool DXRenderer::BeginRender()
{
	//	all user render targets should have been popped
	ASSERT(!m_RTStack.size());

	//	Clear back buffer
	A3DCOLORVALUE color(0.0f, 0.125f, 0.3f, 1.0f);
	m_pDevContext->ClearRenderTargetView(m_pBackBufView, color.c);
	m_pDevContext->ClearDepthStencilView(m_pDepthView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	return true;
}

void DXRenderer::EndRender()
{
	m_pSwapChain->Present(0, 0);
}

void DXRenderer::ClearVertexShader()
{
	m_pDevContext->VSSetShader(NULL, NULL, 0);
}

void DXRenderer::ClearPixelShader()
{
	m_pDevContext->PSSetShader(NULL, NULL, 0);
}

void DXRenderer::SetWireFrameMode(bool bWireFrame)
{
	m_bWireFrame = bWireFrame;
}

void DXRenderer::PushRenderTarget(IDXRTResource* pRT, auint32 uClearFlags)
{
	ASSERT(pRT);
	pRT->Apply(uClearFlags);
	m_RTStack.push_back(pRT);
}

void DXRenderer::PopRenderTarget(IDXRTResource* pRT)
{
	ASSERT(pRT && m_RTStack.size());
	ASSERT(pRT == m_RTStack[m_RTStack.size() - 1]);
	m_RTStack.pop_back();

	if (m_RTStack.size())
	{
		//	Restore previous render targets
		IDXRTResource* pPreRTRes = m_RTStack[m_RTStack.size() - 1];
		pPreRTRes->Apply(0);
	}
	else
	{
		//	Restore default render targets
		m_pDevContext->OMSetRenderTargets(1, &m_pBackBufView, m_pDepthView);
	}
}

void DXRenderer::ClearCS_SRVs(int iStartSlot, int iSlotNum)
{
	ID3D11ShaderResourceView* pSRV = nullptr;

	for (int i = 0; i < iSlotNum; i++)
	{
		m_pDevContext->CSSetShaderResources(iStartSlot + i, 1, &pSRV);
	}
}

void DXRenderer::ClearCS_UAVs(int iStartSlot, int iSlotNum)
{
	ID3D11UnorderedAccessView* pUAV = nullptr;

	for (int i = 0; i < iSlotNum; i++)
	{
		m_pDevContext->CSSetUnorderedAccessViews(iStartSlot + i, 1, &pUAV, nullptr);
	}
}

