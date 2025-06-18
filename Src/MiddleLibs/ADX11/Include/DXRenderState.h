//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#ifndef _DXRENDERSTATE_H_
#define _DXRENDERSTATE_H_

#include <d3d11.h>
#include "AXTypes.h"

///////////////////////////////////////////////////////////////////////////

class CDXRenderState
{
public:     //  Types

    struct BLEND
    {
        D3D11_BLEND_DESC d;
        BLEND();
    };

    struct DEPTH
    {
        D3D11_DEPTH_STENCIL_DESC d;
        DEPTH();
    };

    struct RASTERIZE
    {
        D3D11_RASTERIZER_DESC d;
        RASTERIZE();
	};

public:

	CDXRenderState();
	CDXRenderState(const BLEND* blend, const DEPTH* depth, const RASTERIZE* rast, bool bWireFrameEnable);
    ~CDXRenderState();

public:

    A3DCOLORVALUE   m_BlendFactor;
    UINT            m_SampleMask;
    UINT            m_StencilRef;

public:

	//	Apply render state to device
	void Apply();

protected:

    ID3D11BlendState*           m_pBlendState;
    ID3D11DepthStencilState*    m_pDepthState;
    ID3D11RasterizerState*      m_pRastState;
    ID3D11RasterizerState*      m_pRastStateWF;      //  for wireframe

    bool        m_bWireFrameEnable;

protected:

};

#endif	//	_DXRENDERSTATE_H_