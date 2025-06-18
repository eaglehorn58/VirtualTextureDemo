//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#include "DXInclude.h"
#include "AFI.h"
#include "AFileWrapper.h"
#include "ALog.h"
#include "AMemory.h"

///////////////////////////////////////////////////////////////////////////

CDXInclude::CDXInclude(const char* szFile)
{
	//	Get base path without the last '/';
	af_NormalizeFileName(szFile, m_szBasePath);
	char* pTemp = strrchr(m_szBasePath, '/');
	ASSERT(pTemp);
	*pTemp = '\0';
}

CDXInclude::~CDXInclude()
{
	ASSERT(0 == m_aFileNodes.size());
}

int CDXInclude::FindFileNode(const void* pData)
{
	for (int i = 0; i < (int)m_aFileNodes.size(); i++)
	{
		if (m_aFileNodes[i]->pData == pData)
			return i;
	}

	return -1;
}

CDXInclude::FILE_NODE* CDXInclude::ParseFileName(const char* pFileName, FILE_NODE* pParentNode)
{
	char szCurFile[MAX_PATH];
	if (pParentNode)
	{
		//	Get parent file's path
		strcpy(szCurFile, pParentNode->szFile);
		char* pTemp = strrchr(szCurFile, '/');
		ASSERT(pTemp);
		*pTemp = '\0';
	}
	else
		strncpy(szCurFile, m_szBasePath, MAX_PATH);

	const char* pCur = pFileName;
	int iLen = static_cast<int>(strlen(pFileName));
	int iCur = 0;

	//	handle '.' and '..'
	while (1)
	{
		if (iLen - iCur > 2)
		{
			if (pCur[0] == '.' && pCur[1] == '/')
			{
				iCur += 2;
				pCur += 2;
			}
			else if (iLen - iCur > 3)
			{
				if (pCur[0] == '.' && pCur[1] == '.' && pCur[2] == '/')
				{
					iCur += 3;
					pCur += 3;

					//	Return to up level folder
					char* pTemp = strrchr(szCurFile, '/');
					if (!pTemp)
					{
						ASSERT(pTemp);
						return NULL;
					}
					else
						*pTemp = '\0';
				}
				else
					break;
			}
			else
				break;
		}
		else
			break;
	}

	//	Build current file name
	strcat(szCurFile, "/");
	strcat(szCurFile, pCur);

	//	Create a new file node
	FILE_NODE* pNewNode = (FILE_NODE*)a_malloc(sizeof (FILE_NODE));
	pNewNode->pData = NULL;
	strncpy(pNewNode->szFile, szCurFile, MAX_PATH);
	m_aFileNodes.push_back(pNewNode);

	return pNewNode;
}

HRESULT __stdcall CDXInclude::Open(THIS_ D3D10_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData,
	LPCVOID *ppData, UINT *pBytes)
{
	FILE_NODE* pParentNode = NULL;
	if (pParentData)
	{
		int iIndex = FindFileNode(pParentData);
		ASSERT(iIndex >= 0);
		pParentNode = m_aFileNodes[iIndex];
	}

	//	normalize file name
	char szFile[MAX_PATH];
	af_NormalizeFileName(pFileName, szFile);

	//	Parse file name
	FILE_NODE* pNode = ParseFileName(szFile, pParentNode);
	if (!pNode)
	{
		ASSERT(pNode);
		return E_FAIL;
	}

	auint32 dwDataLen = 0;
	void* pData = LoadFileContent(pNode->szFile, dwDataLen);
	if (!pData)
		return E_FAIL;

	*ppData = pData;
	*pBytes = dwDataLen;

	pNode->pData = pData;

	return S_OK;
}

HRESULT __stdcall CDXInclude::Close(THIS_ LPCVOID pData)
{
	//	Remove path node
	int iIndex = FindFileNode(pData);
	ASSERT(iIndex >= 0);

	FILE_NODE* pNode = m_aFileNodes[iIndex];
	a_free(pNode);
	m_aFileNodes.erase_noorder(m_aFileNodes.begin() + iIndex);
	a_free((void*)pData);

	return S_OK;
}

void* CDXInclude::LoadFileContent(const char* szFile, auint32& dwDataSize)
{
	AFileWrapper file(szFile, "rb");
	if (!file.GetFile())
	{
		a_OutputLog("CDXInclude::LoadFileContent, failed to open file [%s] !", szFile);
		return NULL;
	}

	dwDataSize = (auint32)file.GetLength();
	void* pData = a_malloc(dwDataSize);
	if (!pData)
		return NULL;

	file.Read(pData, dwDataSize);

	return pData;
}

