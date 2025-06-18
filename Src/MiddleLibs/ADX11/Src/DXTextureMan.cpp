//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com

#pragma warning(disable:4005)

#include "DXTextureMan.h"
#include "DXTexture.h"
#include "AXCSWrapper.h"
#include "AThreadFactory.h"
#include "ALog.h"
#include "A3DMacros.h"

///////////////////////////////////////////////////////////////////////////

CDXTextureMan::TEXTURESLOT::TEXTURESLOT() :
pTexture(NULL),
iRefCount(1)
{
	szFile[0] = '\0';
}

///////////////////////////////////////////////////////////////////////////

CDXTextureMan::CDXTextureMan() :
m_tabTex(512)
{
	m_csTexTab = AThreadFactory::CreateThreadMutex();
}

CDXTextureMan::~CDXTextureMan()
{
	A3DINTERFACERELEASE(m_csTexTab);
}

bool CDXTextureMan::Init()
{
	return true;
}

void CDXTextureMan::Release()
{
	ReleaseAllTextures();
}

void CDXTextureMan::ReleaseAllTextures()
{
	AXCSWrapper csw(m_csTexTab);

	//	Clear mesh cache
	TextureTable::iterator it = m_tabTex.begin();
	for (; it != m_tabTex.end(); ++it)
	{
		TEXTURESLOT* pSlot = *it.value();
		A3DRELEASE(pSlot->pTexture);
		delete pSlot;
	}

	m_tabTex.clear();
}

CDXTextureMan::TEXTURESLOT* CDXTextureMan::FindTexture(const char* szFilename, bool bAddRef)
{
	ASSERT(szFilename && szFilename[0]);
	auint32 id = CDXTexture::MakeID(szFilename);

	AXCSWrapper csw(m_csTexTab);

	TextureTable::pair_type pair = m_tabTex.get(id);
	if (pair.second)
	{
		TEXTURESLOT* pSlot = *pair.first;

		//	file name should match
		ASSERT(!stricmp(pSlot->szFile, szFilename));

		if (bAddRef)
		{
			pSlot->iRefCount++;
		}

		return pSlot;
	}

	return NULL;
}

void CDXTextureMan::ReleaseTexture(CDXTexture* pTexture)
{
	if (!pTexture)
		return;

	AXCSWrapper csw(m_csTexTab);

	TextureTable::iterator it = m_tabTex.find(pTexture->GetID());
	if (it != m_tabTex.end())
	{
		TEXTURESLOT* pSlot = *it.value();

		ASSERT(pSlot->pTexture == pTexture);

		pSlot->iRefCount--;
		if (!pSlot->iRefCount)
		{
			//	Release texture
			A3DRELEASE(pSlot->pTexture);
			delete pSlot;
			m_tabTex.erase(it);
		}
	}
	else
	{
		ASSERT(0 && "CDXTextureMan::ReleaseTexture, can't find texture");
	}
}

CDXTexture* CDXTextureMan::CreateTexture(const char* szID, int width, int height, DXGI_FORMAT fmt,
	bool bCubeMap, int iMipLevels, auint32 uCPUAccess)
{
	if (!szID || !szID[0])
		return NULL;

	AXCSWrapper csw(m_csTexTab);

	TEXTURESLOT* pSlot = FindTexture(szID, true);
	if (pSlot)
		return pSlot->pTexture;

	CDXTexture* pTexture = new CDXTexture;
	if (!pTexture->Init(szID, width, height, fmt, bCubeMap, iMipLevels, uCPUAccess))
	{
		a_OutputLog("CDXTextureMan::CreateTexture, failed to create texture [%s] !", szID);
		return false;
	}

	//	Push to table
	CreateTextureSlot(pTexture);

	return pTexture;
}

CDXTexture* CDXTextureMan::CreateTextureArray(const char* szID, int width, int height, DXGI_FORMAT fmt,
	int iArraySize, int iMipLevels)
{
	if (!szID || !szID[0])
		return NULL;

	AXCSWrapper csw(m_csTexTab);

	TEXTURESLOT* pSlot = FindTexture(szID, true);
	if (pSlot)
		return pSlot->pTexture;

	CDXTexture* pTexture = new CDXTexture;
	if (!pTexture->InitArray(szID, width, height, fmt, iArraySize, iMipLevels))
	{
		a_OutputLog("CDXTextureMan::CreateTextureArray, failed to create texture array [%s] !", szID);
		return false;
	}

	//	Push to table
	CreateTextureSlot(pTexture);

	return pTexture;
}

CDXTexture* CDXTextureMan::LoadTextureFromFile(const char* szFile, int iMipLevels)
{
	if (!szFile || !szFile[0])
		return NULL;

	char szTexFile[MAX_PATH];
	CDXTexture::NormalizeFileName(szFile, szTexFile);

	AXCSWrapper csw(m_csTexTab);

	TEXTURESLOT* pSlot = FindTexture(szTexFile, true);
	if (pSlot)
		return pSlot->pTexture;

	CDXTexture* pTexture = new CDXTexture;
	if (!pTexture->InitFromFile(szFile, iMipLevels))
	{
		a_OutputLog("CDXTextureMan::LoadTextureFromFile(), failed to load texture [%s] !", szFile);
		return false;
	}

	//	Push to table
	CreateTextureSlot(pTexture);

	return pTexture;
}

CDXTexture* CDXTextureMan::CreateTexture(const char* szID, CDXRenderTarget* pRT, int iRTIndex)
{
	if (!szID || !szID[0])
		return NULL;

	AXCSWrapper csw(m_csTexTab);

	TEXTURESLOT* pSlot = FindTexture(szID, true);
	if (pSlot)
		return pSlot->pTexture;

	CDXTexture* pTexture = new CDXTexture;
	if (!pTexture->InitFromRT(szID, pRT, iRTIndex))
	{
		a_OutputLog("CDXTextureMan::CreateTexture, failed to create texture [%s] !", szID);
		return false;
	}

	//	Push to table
	CreateTextureSlot(pTexture);

	return pTexture;
}

CDXTexture* CDXTextureMan::CreateTexture(const char* szID, A3DCOLOR color)
{
	if (!szID || !szID[0])
		return NULL;

	AXCSWrapper csw(m_csTexTab);

	TEXTURESLOT* pSlot = FindTexture(szID, true);
	if (pSlot)
		return pSlot->pTexture;

	CDXTexture* pTexture = new CDXTexture;
	if (!pTexture->Init(szID, color))
	{
		a_OutputLog("CDXTextureMan::CreateTexture, failed to create texture [%s] !", szID);
		return false;
	}

	//	Push to table
	CreateTextureSlot(pTexture);

	return pTexture;
}

void CDXTextureMan::CreateTextureSlot(CDXTexture* pTex)
{
	TEXTURESLOT* pSlot = new TEXTURESLOT;
	strcpy(pSlot->szFile, pTex->GetFileName());
	pSlot->pTexture = pTex;
	m_tabTex.put(pTex->GetID(), pSlot);
}

CDXTexture* CDXTextureMan::CloneTexture(CDXTexture* pSrcTex)
{
	ASSERT(pSrcTex);

	AXCSWrapper csw(m_csTexTab);

	TextureTable::iterator it = m_tabTex.find(pSrcTex->GetID());
	if (it != m_tabTex.end())
	{
		TEXTURESLOT* pSlot = *it.value();
		ASSERT(pSlot->pTexture == pSrcTex);
		pSlot->iRefCount++;
		return pSlot->pTexture;
	}
	else
	{
		//	This shouldn't happen
		ASSERT(0 && "Isolated texture object !");
		return NULL;
	}
}

int CDXTextureMan::GetTextureCount() const
{
	return (int)m_tabTex.size();
}
