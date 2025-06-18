//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#include "Global.h"
#include "VTextureGenerator.h"
#include "DXComputeShader.h"
#include "DXShaderMate.h"
#include "DXUATexture.h"
#include "DXTexture.h"
#include "DXTextureMan.h"
#include "DXSampler.h"
#include "DXRenderer.h"
#include "A3DMacros.h"
#include "AFI.h"

//	group thread number for adding border to VT
#define THREAD_CNT_ADD_BORDER	16

///////////////////////////////////////////////////////////////////////////

struct TEMP_RES
{
	CDXTexture*		pSrcVT;
	CDXUATexture*	pBorderedVT;

	TEMP_RES()
	{
		pSrcVT = nullptr;
		pBorderedVT = nullptr;
	}

	~TEMP_RES()
	{
		CDXTextureMan* pTexMan = DXRenderer::Instance()->GetTextureMan();

		if (pSrcVT)
		{
			pTexMan->ReleaseTexture(pSrcVT);
		}

		A3DRELEASE(pBorderedVT);
	}
};

///////////////////////////////////////////////////////////////////////////
//	Class CCSMateAddBorder
///////////////////////////////////////////////////////////////////////////

class CCSMateAddBorder : public CDXComputeShaderMate
{
public:		//	Types

	struct CS_CONST
	{
		int		iVTSize;			//	VT texture size (mip 0 level)
		int		iVPageSize;			//	VPage size
		int		iBorderSize;		//	border size
		float	padding[1];
	};

public:		//	Attributes

	CS_CONST	data;		//	Constant data

public:		//	Operations

	CCSMateAddBorder()
	{
		memset(&data, 0, sizeof(data));
	}

	virtual int GetConstBufNum() override { return 1; }
	virtual int GetConstBufLen(int slot) override { return sizeof(data); }
	virtual const void* GetConstBufData(int slot) override { return &data; }
};

///////////////////////////////////////////////////////////////////////////
//	Implement of CVTextureGenerator
///////////////////////////////////////////////////////////////////////////

CVTextureGenerator::CVTextureGenerator() :
m_pPtSampler(nullptr),
m_pCSAddBorder(nullptr),
m_pcsmateAddBorder(nullptr)
{
	m_pcsmateAddBorder = new CCSMateAddBorder;
}

CVTextureGenerator::~CVTextureGenerator()
{
	A3DSAFEDELETE(m_pcsmateAddBorder);
}

bool CVTextureGenerator::Init()
{
	m_pCSAddBorder = new CDXComputeShader;
	m_pCSAddBorder->Init("Shaders/GenBorderedVT_cs.hlsl", "cs_main", m_pcsmateAddBorder);

	//	point wrap sampler
	m_pPtSampler = new CDXSampler(CDXSampler::FILTER_POINT, CDXSampler::ADDR_WRAP);

	return true;
}

void CVTextureGenerator::Release()
{
	A3DSAFEDELETE(m_pPtSampler);
	A3DRELEASE(m_pCSAddBorder);
}

void test(int dtid_x)
{
	const int g_iVTSize = 4096;
	const int g_iVPageSize = 128;
	const int g_iBorderSize = 2;


	int page = dtid_x / g_iVPageSize;
	int vGridInPage = dtid_x % g_iVPageSize;
	float uv_off = (page * g_iVPageSize - g_iBorderSize) / (float)g_iVTSize;

	float fPageSizeWithBorder = g_iVPageSize + g_iBorderSize * 2;
	float vGridOff = ((float)vGridInPage / g_iVPageSize) * fPageSizeWithBorder;

	float uv0 = floor(vGridOff);
	float uv3 = uv0 + 1.0f;
	float vFrac = ((uv3 / fPageSizeWithBorder) - (vGridInPage / (float)g_iVPageSize)) * g_iVPageSize;

	uv0 = uv_off + uv0 / (float)g_iVTSize;
//	uv1 = uv_off + (uv1 * fPageSizeWithBorder) / (float)g_iVTSize;
//	uv2 = uv_off + (uv2 * fPageSizeWithBorder) / (float)g_iVTSize;
	uv3 = uv_off + uv3 / (float)g_iVTSize;

	int a = 0;
}

bool CVTextureGenerator::ProcessVT(const char* szFile, int iVPageSize, int iBorderSize)
{
//	test(1);

	CDXTextureMan* pTexMan = DXRenderer::Instance()->GetTextureMan();

	TEMP_RES temp;

	//	load source VT
	temp.pSrcVT = pTexMan->LoadTextureFromFile(szFile, 1);
	if (!temp.pSrcVT)
	{
		glb_OutputLog("CVTextureGenerator::ProcessVT, failed to init load texture [%s]", szFile);
		return false;
	}

	const CDXTexture::TEX_DESC& desc_vt = temp.pSrcVT->GetDesc();
	MyAssert(desc_vt.width == desc_vt.height);

	//	Create bordered VT
	temp.pBorderedVT = new CDXUATexture;
	if (!temp.pBorderedVT->Init(desc_vt.width, desc_vt.height, desc_vt.fmt, false, 1))
	{
		glb_OutputLog("CVTextureGenerator::ProcessVT, failed to create bordered VT");
		return false;
	}

	//	Run compute shader to fill border for VT
	temp.pSrcVT->ApplyToCS(0);
	temp.pBorderedVT->ApplyUAVToCS(0, 1, 0);
	m_pPtSampler->ApplyToCS(0);

	int group_dim_x = desc_vt.width / THREAD_CNT_ADD_BORDER;
	int group_dim_y = desc_vt.height / THREAD_CNT_ADD_BORDER;

	m_pcsmateAddBorder->data.iVTSize = desc_vt.width;
	m_pcsmateAddBorder->data.iVPageSize = iVPageSize;
	m_pcsmateAddBorder->data.iBorderSize = iBorderSize;
	m_pCSAddBorder->UpdateParams(m_pcsmateAddBorder, -1);

	m_pCSAddBorder->Apply();
	m_pCSAddBorder->Dispatch(group_dim_x, group_dim_y, 1);

	DXRenderer::Instance()->ClearCS_SRVs(0, 1);
	DXRenderer::Instance()->ClearCS_UAVs(0, 1);

	//	save new texture to file
	char szPath[MAX_PATH], szTitle[MAX_PATH];
	af_GetFilePath(szFile, szPath, MAX_PATH);
	af_GetFileTitle(szFile, szTitle, MAX_PATH);

	char szNewFile[MAX_PATH];
	sprintf(szNewFile, "%s/B_%s", szPath, szTitle);
	temp.pBorderedVT->SaveToFile(szNewFile, true);

	return true;
}

