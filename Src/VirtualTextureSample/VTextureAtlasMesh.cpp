//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#include "Global.h"
#include "VTextureSystem.h"
#include "VTextureAtlasMesh.h"
#include "VTextureAtlas.h"
#include "A3DMacros.h"
#include "DXPixelShader.h"
#include "DXVertexBuffer.h"
#include "DXVertexShader.h"
#include "DXRenderer.h"
#include "DXShaderMate.h"

///////////////////////////////////////////////////////////////////////////
//	Class CVSMateDrawVPage
///////////////////////////////////////////////////////////////////////////

class CVSMateVTAtlas : public CDXVertexShaderMate
{
public:		//	Types

	struct VS_CONST
	{
		float	aOffsetX[VTexture::MAX_VPAGE_LOADED];	//	offset of page in cache texture
		float	aOffsetY[VTexture::MAX_VPAGE_LOADED];
		float	fPageDivRTSize;		//	VPage size / render target size
		float	padding[3];
	};

public:		//	Attributes

	VS_CONST	data;		//	Constant data

public:		//	Operations

	CVSMateVTAtlas()
	{
		memset(&data, 0, sizeof(data));
		memset(&m_layout, 0, sizeof(m_layout));

		m_layout[0].SemanticName = "POSITION";
		m_layout[0].SemanticIndex = 0;
		m_layout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		//	m_layout[0].InputSlot = 0;
		m_layout[0].AlignedByteOffset = 0;
		m_layout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		//	m_layout[0].InstanceDataStepRate = 0;

		m_layout[1].SemanticName = "TEXCOORD";
		m_layout[1].SemanticIndex = 0;
		m_layout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		//	m_layout[1].InputSlot = 0;
		m_layout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		m_layout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	}

	virtual int GetInputElemDescNum() override { return sizeof(m_layout) / sizeof(D3D11_INPUT_ELEMENT_DESC); }
	virtual const D3D11_INPUT_ELEMENT_DESC* GetInputElemDesc() override { return m_layout; }
	virtual int GetConstBufNum() override { return 1; }
	virtual int GetConstBufLen(int slot) override { return sizeof(data); }
	virtual const void* GetConstBufData(int slot) override { return &data; }

protected:	//	Attributes

	D3D11_INPUT_ELEMENT_DESC	m_layout[2];

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	Class CPSMateVTAtlas
///////////////////////////////////////////////////////////////////////////

class CPSMateVTAtlas : public CDXPixelShaderMate
{
public:		//	Types

	struct PS_CONST
	{
		//	A3DCOLORVALUE	color;		//	Mesh color
	};

public:		//	Attributes

	PS_CONST	data;		//	Constant data

public:		//	Operations

	CPSMateVTAtlas()
	{
		memset(&data, 0, sizeof(data));
	}

	virtual int GetConstBufNum() override { return 0; }
	virtual int GetConstBufLen(int slot) override { return sizeof(data); }
	virtual const void* GetConstBufData(int slot) override { return &data; }
};

///////////////////////////////////////////////////////////////////////////
//	Implement of CVTextureAtlasMesh
///////////////////////////////////////////////////////////////////////////

CVTextureAtlasMesh::CVTextureAtlasMesh(CVTextureSystem* pVTSys) :
m_pVTSys(pVTSys)
{
	m_vsmateAltas = new CVSMateVTAtlas;
	m_psmateAltas = new CPSMateVTAtlas;

	m_pvsAltas = new CDXVertexShader;
	m_pvsAltas->Init("Shaders/VTextureAtlas_vs.hlsl", "vs_main", m_vsmateAltas);

	m_ppsAltas = new CDXPixelShader;
	m_ppsAltas->Init("Shaders/VTextureAtlas_ps.hlsl", "ps_main", m_psmateAltas);

	//	Initlaize vertex buffer
	//	TODO: use instanced rendering ?
	VERTEX_TYPE aVerts[VTexture::MAX_VPAGE_LOADED * 6];
	int pageIdx = 0;
	int vertIdx = 0;
	for (int i = 0; i < VTexture::MAX_VPAGE_LOADED; i++)
	{
		aVerts[vertIdx++].vPos.Set(0.0f, 0.0f, (float)pageIdx);
		aVerts[vertIdx++].vPos.Set(1.0f, 0.0f, (float)pageIdx);
		aVerts[vertIdx++].vPos.Set(1.0f, 1.0f, (float)pageIdx);

		aVerts[vertIdx++].vPos.Set(0.0f, 0.0f, (float)pageIdx);
		aVerts[vertIdx++].vPos.Set(1.0f, 1.0f, (float)pageIdx);
		aVerts[vertIdx++].vPos.Set(0.0f, 1.0f, (float)pageIdx);

		pageIdx++;
	}

	m_pvb = new CDXVertexBuffer;
	m_pvb->Init(sizeof(VERTEX_TYPE), VTexture::MAX_VPAGE_LOADED * 6, CDXVertexBuffer::FLAG_STATIC, aVerts);
}

CVTextureAtlasMesh::~CVTextureAtlasMesh()
{
	A3DRELEASE(m_pvb);
	A3DRELEASE(m_pvsAltas);
	A3DRELEASE(m_ppsAltas);
	A3DSAFEDELETE(m_vsmateAltas);
	A3DSAFEDELETE(m_psmateAltas);
}

void CVTextureAtlasMesh::Render(const auint32* aCacheIDs, int iNumCache, bool bSkipDefPage)
{
	if (!iNumCache)
		return;

	MyAssert(iNumCache <= VTexture::MAX_VPAGE_LOADED);

	m_vsmateAltas->data.fPageDivRTSize = (float)(m_pVTSys->GetVPageSize()) / CVTextureAtlas::PAGE_CACHE_TEX_SIZE;

	int dstIndex = bSkipDefPage ? 1 : 0;
	for (int i = 0; i < iNumCache; i++)
	{
		VTexture::CacheID entry = aCacheIDs[i];
		m_vsmateAltas->data.aOffsetX[dstIndex] = (float)entry.off_x;
		m_vsmateAltas->data.aOffsetY[dstIndex] = (float)entry.off_y;
		dstIndex++;
	}

	m_pvsAltas->UpdateParams(m_vsmateAltas, -1);
	m_pvsAltas->Apply();

	m_ppsAltas->UpdateParams(m_psmateAltas, -1);
	m_ppsAltas->Apply();

	m_pvb->Apply();

	ID3D11DeviceContext* pDevContext = DXRenderer::Instance()->GetDeviceContext();
	pDevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDevContext->Draw(iNumCache * 6, bSkipDefPage ? 6 : 0);
}


