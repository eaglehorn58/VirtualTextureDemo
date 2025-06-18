//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#include "Global.h"
#include "TestMesh.h"
#include "DXVertexBuffer.h"
#include "DXVertexShader.h"
#include "DXPixelShader.h"
#include "DXRenderer.h"
#include "DXTexture.h"
#include "DXSampler.h"
#include "A3DMacros.h"
#include "AXCameraBase.h"
#include "VTMainWindow.h"
#include "VTextureSystem.h"
#include "VTextureAtlas.h"

///////////////////////////////////////////////////////////////////////////

//	convert local uv to VT uv
static AXVector2 _uv_Local2VT(AXVector2 vLocal, const CVTextureSystem::ORIGINAL_TEX& TexInfo)
{
//	vLocal = vLocal * 0.9999f + AXVector2(0.0001f, 0.0001f);
	AXVector2 uvVT = vLocal * TexInfo.scale + AXVector2(TexInfo.off_u, TexInfo.off_v);
	return uvVT;
}

///////////////////////////////////////////////////////////////////////////

CVSMateMeshDef::CVSMateMeshDef()
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
	m_layout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	//	m_layout[1].InputSlot = 0;
	m_layout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	m_layout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement of CPSMateMeshDef
//
///////////////////////////////////////////////////////////////////////////

CPSMateMeshDef::CPSMateMeshDef()
{
	memset(&data, 0, sizeof(data));
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement of CPSMateMeshVT
//
///////////////////////////////////////////////////////////////////////////

CPSMateMeshVT::CPSMateMeshVT()
{
	memset(&data, 0, sizeof(data));
}

void CPSMateMeshVT::FillCB()
{
	CVTextureSystem* pVTSys = g_pMainWnd->GetVTSystem();

	data.iVTSize = pVTSys->GetVTexSize();
	data.iVPageSize = pVTSys->GetVPageSize();
	data.iVPageCntInRow = data.iVTSize / data.iVPageSize;
	data.iMaxMipmap = pVTSys->GetMipmapCnt() - 1;
	data.iPageIDDownscale = 8;
	data.iPageIDBufWid = DXRenderer::Instance()->GetScreenWidth() / 8;
	data.vPageIDDither[0] = pVTSys->GetDitherX();
	data.vPageIDDither[1] = pVTSys->GetDitherY();
	data.iCacheTexSize = pVTSys->GetPageCacheTexSize();
	data.iBorderSize = VTexture::PAGE_BORDER_SIZE;
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement of CTestMesh
//
///////////////////////////////////////////////////////////////////////////

CTestMesh::CTestMesh() :
m_pvb(NULL),
m_pib(NULL),
m_pvsDef(nullptr),
m_ppsDef(nullptr),
m_ppsVT(nullptr)
{
}

CTestMesh::~CTestMesh()
{
}

bool CTestMesh::Init()
{
	m_pvsDef = new CDXVertexShader;
	m_pvsDef->Init("Shaders/MeshDefault_vs.hlsl", "vs_main", &m_vsmateDef);

	m_ppsDef = new CDXPixelShader;
	m_ppsDef->Init("Shaders/MeshDefault_ps.hlsl", "ps_main", &m_psmateDef);

	m_ppsVT = new CDXPixelShader;
	m_ppsVT->Init("Shaders/MeshVT_ps.hlsl", "ps_main", &m_psmateVT);

	if (!m_pvsDef || !m_ppsDef || !m_ppsVT)
	{
		MyAssert(0);
		Release();
		glb_OutputLog("CTestMesh::Init, Failed to create shaders!");
		return false;
	}

	//	Build a box
	float fScale = 50.0f;

	//	Create vertex buffer
	AXVector3 aPos[8];
	aPos[0].Set(-fScale, fScale, fScale);
	aPos[1].Set(fScale, fScale, fScale);
	aPos[2].Set(fScale, fScale, -fScale);
	aPos[3].Set(-fScale, fScale, -fScale);
	aPos[4].Set(-fScale, -fScale, fScale);
	aPos[5].Set(fScale, -fScale, fScale);
	aPos[6].Set(fScale, -fScale, -fScale);
	aPos[7].Set(-fScale, -fScale, -fScale);

	VERTEX_TYPE aVerts[24];
	const float fOne = 0.99999f;

	{
		//	Face 0
		const CVTextureSystem::ORIGINAL_TEX& TexInfo = g_pMainWnd->GetVTSystem()->GetOriginalTexInfo(0);
		aVerts[0].vPos = aPos[0];
		aVerts[0].uv = _uv_Local2VT(AXVector2(0.0f, 0.0f), TexInfo);
		aVerts[1].vPos = aPos[1];
		aVerts[1].uv = _uv_Local2VT(AXVector2(fOne, 0.0f), TexInfo);
		aVerts[2].vPos = aPos[2];
		aVerts[2].uv = _uv_Local2VT(AXVector2(fOne, fOne), TexInfo);
		aVerts[3].vPos = aPos[3];
		aVerts[3].uv = _uv_Local2VT(AXVector2(0.0f, fOne), TexInfo);
	}

	{
		//	Face 1
		const CVTextureSystem::ORIGINAL_TEX& TexInfo = g_pMainWnd->GetVTSystem()->GetOriginalTexInfo(1);
		aVerts[4].vPos = aPos[2];
		aVerts[4].uv = _uv_Local2VT(AXVector2(0.0f, 0.0f), TexInfo);
		aVerts[5].vPos = aPos[1];
		aVerts[5].uv = _uv_Local2VT(AXVector2(fOne, 0.0f), TexInfo);
		aVerts[6].vPos = aPos[5];
		aVerts[6].uv = _uv_Local2VT(AXVector2(fOne, fOne), TexInfo);
		aVerts[7].vPos = aPos[6];
		aVerts[7].uv = _uv_Local2VT(AXVector2(0.0f, fOne), TexInfo);
	}

	{
		//	Face 2
		const CVTextureSystem::ORIGINAL_TEX& TexInfo = g_pMainWnd->GetVTSystem()->GetOriginalTexInfo(2);
		aVerts[8].vPos = aPos[3];
		aVerts[8].uv = _uv_Local2VT(AXVector2(0.0f, 0.0f), TexInfo);
		aVerts[9].vPos = aPos[2];
		aVerts[9].uv = _uv_Local2VT(AXVector2(fOne, 0.0f), TexInfo);
		aVerts[10].vPos = aPos[6];
		aVerts[10].uv = _uv_Local2VT(AXVector2(fOne, fOne), TexInfo);
		aVerts[11].vPos = aPos[7];
		aVerts[11].uv = _uv_Local2VT(AXVector2(0.0f, fOne), TexInfo);
	}

	{
		//	Face 3
		const CVTextureSystem::ORIGINAL_TEX& TexInfo = g_pMainWnd->GetVTSystem()->GetOriginalTexInfo(3);
		aVerts[12].vPos = aPos[0];
		aVerts[12].uv = _uv_Local2VT(AXVector2(0.0f, 0.0f), TexInfo);
		aVerts[13].vPos = aPos[3];
		aVerts[13].uv = _uv_Local2VT(AXVector2(fOne, 0.0f), TexInfo);
		aVerts[14].vPos = aPos[7];
		aVerts[14].uv = _uv_Local2VT(AXVector2(fOne, fOne), TexInfo);
		aVerts[15].vPos = aPos[4];
		aVerts[15].uv = _uv_Local2VT(AXVector2(0.0f, fOne), TexInfo);
	}

	{
		//	Face 4
		const CVTextureSystem::ORIGINAL_TEX& TexInfo = g_pMainWnd->GetVTSystem()->GetOriginalTexInfo(4);
		aVerts[16].vPos = aPos[1];
		aVerts[16].uv = _uv_Local2VT(AXVector2(0.0f, 0.0f), TexInfo);
		aVerts[17].vPos = aPos[0];
		aVerts[17].uv = _uv_Local2VT(AXVector2(fOne, 0.0f), TexInfo);
		aVerts[18].vPos = aPos[4];
		aVerts[18].uv = _uv_Local2VT(AXVector2(fOne, fOne), TexInfo);
		aVerts[19].vPos = aPos[5];
		aVerts[19].uv = _uv_Local2VT(AXVector2(0.0f, fOne), TexInfo);
	}

	{
		//	Face 5
		const CVTextureSystem::ORIGINAL_TEX& TexInfo = g_pMainWnd->GetVTSystem()->GetOriginalTexInfo(5);
		aVerts[20].vPos = aPos[7];
		aVerts[20].uv = _uv_Local2VT(AXVector2(0.0f, 0.0f), TexInfo);
		aVerts[21].vPos = aPos[6];
		aVerts[21].uv = _uv_Local2VT(AXVector2(fOne, 0.0f), TexInfo);
		aVerts[22].vPos = aPos[5];
		aVerts[22].uv = _uv_Local2VT(AXVector2(fOne, fOne), TexInfo);
		aVerts[23].vPos = aPos[4];
		aVerts[23].uv = _uv_Local2VT(AXVector2(0.0f, fOne), TexInfo);
	}

	m_pvb = new CDXVertexBuffer;
	if (!m_pvb->Init(sizeof(VERTEX_TYPE), 24, CDXVertexBuffer::FLAG_STATIC, aVerts))
	{
		Release();
		glb_OutputLog("CTestMesh::Init, Failed to create vertex buffer !");
		return false;
	}

	//	Create index buffer
	const auint16 aBaseIdx[6] = { 0, 1, 2, 0, 2, 3 };
	auint16 aIndices[36];
	int index = 0;
	for (int i = 0; i < 6; i++)
	{
		auint16 start = i * 4;
		for (int j = 0; j < 6; j++)
		{
			aIndices[index++] = start + aBaseIdx[j];
		}
	}

	m_pib = new CDXIndexBuffer;
	if (!m_pib->Init(sizeof(auint16), 36, CDXIndexBuffer::FLAG_STATIC, aIndices))
	{
		Release();
		glb_OutputLog("CTestMesh::Init, Failed to create index buffer !");
		return false;
	}

	return true;
}

void CTestMesh::Release()
{
	A3DRELEASE(m_pvb);
	A3DRELEASE(m_pib);
	A3DRELEASE(m_pvsDef);
	A3DRELEASE(m_ppsDef);
	A3DRELEASE(m_ppsVT);
}

bool CTestMesh::RenderZPrePass(AXCameraBase* pCamera)
{
	//	Update vertex shader constants
	m_vsmateDef.data.matWVP = pCamera->GetVPTM();
	m_vsmateDef.data.matWVP.Transpose();		//	This is necessary if a matrix will be passed to shader
	m_pvsDef->UpdateParams(&m_vsmateDef, -1);

	//	Update pixel shader constants
	m_psmateDef.data.color.Set(1.0f, 0.0f, 0.0f, 1.0f);
	m_ppsDef->UpdateParams(&m_psmateDef, -1);

	m_pvsDef->Apply();
	DXRenderer::Instance()->ClearPixelShader();

	m_pvb->Apply();
	m_pib->Apply();

	ID3D11DeviceContext* pDevContext = DXRenderer::Instance()->GetDeviceContext();
	pDevContext->DrawIndexed(m_pib->GetIndexNum(), 0, 0);

	return true;
}

bool CTestMesh::Render(AXCameraBase* pCamera)
{
	//	Update vertex shader constants
	m_vsmateDef.data.matWVP = pCamera->GetVPTM();
	m_vsmateDef.data.matWVP.Transpose();		//	This is necessary if a matrix will be passed to shader
	m_pvsDef->UpdateParams(&m_vsmateDef, -1);
	m_pvsDef->Apply();

	//	Update pixel shader constants
	m_psmateVT.FillCB();
	m_ppsVT->UpdateParams(&m_psmateVT, -1);
	m_ppsVT->Apply();

	//	Apply VT textures
	CVTextureAtlas* pVTAtlas = g_pMainWnd->GetVTSystem()->GetVTAtlas();
	pVTAtlas->GetVPageCacheTexture()->ApplyToPS(0);
	pVTAtlas->GetIndirMapTexture()->ApplyToPS(1);
	pVTAtlas->GetVTSampler()->ApplyToPS(0);
	pVTAtlas->GetVTIndirSampler()->ApplyToPS(1);

	m_pvb->Apply();
	m_pib->Apply();

	ID3D11DeviceContext* pDevContext = DXRenderer::Instance()->GetDeviceContext();
	pDevContext->DrawIndexed(m_pib->GetIndexNum(), 0, 0);

	return true;
}

