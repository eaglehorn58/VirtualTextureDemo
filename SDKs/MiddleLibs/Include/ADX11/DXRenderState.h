/*
* FILE: DXRenderState.h
*
* CREATED BY: duyuxin, 2018/10/17
*
* HISTORY:
*
* Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
*/

#ifndef _DXRENDERSTATE_H_
#define _DXRENDERSTATE_H_

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


///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CDXRenderState
//
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