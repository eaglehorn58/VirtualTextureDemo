//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#include "Global.h"
#include "VPageViewer.h"
#include "DXTexture.h"
#include "DXTextureMan.h"
#include "DXRenderer.h"
#include "A3DMacros.h"
#include "VTextureStream.h"
#include "VTextureSystem.h"

///////////////////////////////////////////////////////////////////////////

CVPageViewer::CVPageViewer() :
m_pVPageTex(nullptr),
m_pPageBuf(nullptr),
m_iVPageWidth(0),
m_mipmap(0),
m_bPageChanged(false)
{
	memset(&m_pos, 0, sizeof(m_pos));
}

CVPageViewer::~CVPageViewer()
{
}

bool CVPageViewer::Init(DXGI_FORMAT fmt, int iVPageWidth, int iPixelBytes)
{
	//	create page texture
	CDXTextureMan* pTexMan = DXRenderer::Instance()->GetTextureMan();
	m_pVPageTex = pTexMan->CreateTexture("CVPageViewerTexture", iVPageWidth, iVPageWidth, fmt,
				false, 1, CDXTexture::CPU_DYNAMIC);
	if (!m_pVPageTex)
	{
		return false;
	}

	m_iVPageWidth = iVPageWidth;
	m_iPixelBytes = iPixelBytes;

	//	create page data buffer
	m_pPageBuf = new abyte[m_iVPageWidth * m_iVPageWidth * m_iPixelBytes];
	if (!m_pPageBuf)
		return false;

	return true;
}

void CVPageViewer::Release()
{
	if (m_pPageBuf)
	{
		delete[] m_pPageBuf;
		m_pPageBuf = nullptr;
	}

	if (m_pVPageTex)
	{
		DXRenderer::Instance()->GetTextureMan()->ReleaseTexture(m_pVPageTex);
		m_pVPageTex = nullptr;
	}
}

void CVPageViewer::SetPageByUV(float u, float v, int mipmap)
{
	m_pos.u = u;
	m_pos.v = v;
	m_mipmap = mipmap;
	m_bPageChanged = true;
}

void CVPageViewer::SetPage(int x, int y, int mipmap)
{
	m_pos.x = x;
	m_pos.y = y;
	m_mipmap = mipmap;
	m_bPageChanged = true;
}

bool CVPageViewer::FillVPage(CVTextureSystem* pVTSys)
{
	CVTextureStream* pVTFile = pVTSys->GetVTStream();
	if (!pVTFile || !m_pVPageTex)
		return false;

	//	read data form stream
	MyAssert(m_mipmap >= 0 && m_mipmap < pVTSys->GetMipmapCnt());
//	pVTFile->ReadVPageByUV(m_u, m_v, m_mipmap, m_pPageBuf);
	pVTFile->ReadVPage(m_pos.x, m_pos.y, m_mipmap, m_pPageBuf);

	CDXTexture::LOCK_INFO lock;
	if (!m_pVPageTex->Lock(0, 0, lock))
		return false;

	const int bpp = 4;
	const int iLineBytes = m_iVPageWidth * m_iPixelBytes;
	abyte* pDst = (abyte*)lock.pData;
	const abyte* pSrc = m_pPageBuf;

	for (int i = 0; i < m_iVPageWidth; i++)
	{
		memcpy(pDst, pSrc, iLineBytes);
		pSrc += iLineBytes;
		pDst += lock.uRowPitch;
	}

	m_pVPageTex->Unlock(0, 0);

	return true;
}

