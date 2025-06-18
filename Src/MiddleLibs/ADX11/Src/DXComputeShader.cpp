//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#include "DXComputeShader.h"
#include "DXShaderMate.h"
#include "DXInclude.h"
#include "DXRenderer.h"
#include "ALog.h"
#include "AFileWrapper.h"
#include "A3DMacros.h"
#include <d3dcompiler.h>
#include <d3d11shader.h>

///////////////////////////////////////////////////////////////////////////

CDXComputeShader::CDXComputeShader() :
m_pShader(NULL),
m_iCBufNum(0)
{
	memset(m_aCBufs, 0, sizeof(m_aCBufs));
}

CDXComputeShader::~CDXComputeShader()
{
}

bool CDXComputeShader::Init(const char* szFile, const char* szFunc, CDXComputeShaderMate* pMate)
{
	//	Open shader file
	AFileWrapper file(szFile, "rb");
	if (!file.GetFile())
	{
		a_OutputLog("CDXComputeShader::Init, failed to open file [%s] !", szFile);
		return false;
	}

	//	Get file length
	int iFileLen = (int)file.GetLength();

	//	Load file input memory
	void* pCodeBuf = malloc(iFileLen);
	if (!pCodeBuf)
	{
		a_OutputLog("CDXComputeShader::Init, not enough memory [%d] for loading shader [%s] !", iFileLen, szFile);
		return false;
	}

	file.Read(pCodeBuf, iFileLen);
	file.Close();

	//	include parser
	CDXInclude inc(szFile);

	//	Compile shader
	ID3D10Blob* pErrorMsg = NULL;
	ID3D10Blob* pShaderBuf = NULL;
	HRESULT result = D3DCompile(pCodeBuf, iFileLen, NULL, NULL, &inc, szFunc, "cs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_ENABLE_STRICTNESS, 0, &pShaderBuf, &pErrorMsg);

	free(pCodeBuf);
	pCodeBuf = NULL;

	if (FAILED(result))
	{
		if (pErrorMsg)
		{
			//	Output error message
			const char* pszError = (const char*)(pErrorMsg->GetBufferPointer());
			int iErrLen = (int)pErrorMsg->GetBufferSize();

			const int iMsgLen = 640;
			char sz[iMsgLen + 1];

			int iCopyLen = (iErrLen < iMsgLen) ? iErrLen : iMsgLen;
			strncpy(sz, pszError, iCopyLen);
			sz[iCopyLen] = '\0';

			a_OutputLog("CDXComputeShader::Init, compile error: %s !", sz);

			pErrorMsg->Release();
			pErrorMsg = NULL;
		}
		else
		{
			a_OutputLog("CDXComputeShader::Init, failed to compile shader [%s] !", szFile);
		}

		return false;
	}

	ID3D11Device* pDevice = DXRenderer::Instance()->GetDevice();

	//	Create DX shader object
	result = pDevice->CreateComputeShader(pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, &m_pShader);
	if (FAILED(result))
	{
		pShaderBuf->Release();
		a_OutputLog("CDXComputeShader::Init, failed to call CreateComputeShader() for shader [%s] !", szFile);
		return false;
	}

#ifdef _DEBUG

	if (pMate && pMate->GetConstBufNum())
	{
		if (!CheckConstBufferSize(pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), pMate))
		{
			pShaderBuf->Release();
			ASSERT(0 && "compute shader constant buffer size dismatch !");
			a_OutputLog("CDXComputeShader::Init, shader constant buffer size dismatch [%s] !", szFile);
			return false;
		}
	}

#endif

	pShaderBuf->Release();
	pShaderBuf = NULL;

	if (pMate && pMate->GetConstBufNum())
	{
		//	Create constant buffers
		m_iCBufNum = pMate->GetConstBufNum();
		ASSERT(m_iCBufNum >= 0 && m_iCBufNum < MAX_CB_NUM);

		for (int i = 0; i < m_iCBufNum; i++)
		{
			CONST_BUF& cb = m_aCBufs[i];

			//	constant buffer length must be in multiples of 16
			cb.iCBLen = pMate->GetConstBufLen(i);
			ASSERT((cb.iCBLen & 0xf) == 0);

			D3D11_BUFFER_DESC descConstBuf;
			memset(&descConstBuf, 0, sizeof(descConstBuf));

			descConstBuf.Usage = D3D11_USAGE_DYNAMIC;
			descConstBuf.ByteWidth = cb.iCBLen;
			descConstBuf.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			descConstBuf.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			//	descConstBuf.MiscFlags = 0;
			//	descConstBuf.StructureByteStride = 0;

			result = pDevice->CreateBuffer(&descConstBuf, NULL, &cb.pCBuf);
			if (FAILED(result))
			{
				Release();
				a_OutputLog("CDXComputeShader::Init, failed to call CreateBuffer() for shader [%s] !", szFile);
				return false;
			}
		}
	}

	return true;
}

void CDXComputeShader::Release()
{
	if (m_iCBufNum)
	{
		for (int i = 0; i < m_iCBufNum; i++)
		{
			CONST_BUF& cb = m_aCBufs[i];
			A3DINTERFACERELEASE(cb.pCBuf);
			cb.iCBLen = 0;
		}

		m_iCBufNum = 0;
	}

	A3DINTERFACERELEASE(m_pShader);
}

bool CDXComputeShader::CheckConstBufferSize(const void* pShaderBuf, int iShaderBufSize, CDXComputeShaderMate* pMate)
{
	ID3D11ShaderReflection* pShaderRef = NULL;
	HRESULT result = D3DReflect(pShaderBuf, iShaderBufSize, IID_ID3D11ShaderReflection, (void**)&pShaderRef);
	if (FAILED(result))
	{
		ASSERT(0);
		return false;
	}

	D3D11_SHADER_DESC descShader;
	pShaderRef->GetDesc(&descShader);

	int iConstBufCnt = 0;

	for (int i = 0; i < (int)descShader.ConstantBuffers; i++)
	{
		//	ID3D11ShaderReflectionConstantBuffer isn't COM interface, so not release it
		ID3D11ShaderReflectionConstantBuffer* pCBRef = pShaderRef->GetConstantBufferByIndex(i);

		D3D11_SHADER_BUFFER_DESC descCB;
		pCBRef->GetDesc(&descCB);

		//	Only treat scalar constants buffer
		if (descCB.Type != D3D_CT_CBUFFER && descCB.Type != D3D11_CT_CBUFFER)
			continue;

		if (iConstBufCnt >= pMate->GetConstBufNum() ||
			descCB.Size != pMate->GetConstBufLen(iConstBufCnt))
		{
			pShaderRef->Release();
			int iSize = pMate->GetConstBufLen(iConstBufCnt);
			ASSERT(iConstBufCnt < pMate->GetConstBufNum() && descCB.Size == iSize);
			return false;
		}

		iConstBufCnt++;
	}

	if (iConstBufCnt < pMate->GetConstBufNum())
	{
		pShaderRef->Release();
		int count = pMate->GetConstBufNum();
		ASSERT(iConstBufCnt == count);
		return false;
	}

	pShaderRef->Release();
	pShaderRef = NULL;

	return true;
}

bool CDXComputeShader::UpdateParams(CDXComputeShaderMate* pMate, int slot)
{
	ASSERT(pMate);

	ID3D11DeviceContext* pDevContext = DXRenderer::Instance()->GetDeviceContext();
	ASSERT(pDevContext);

	if (slot >= 0)
	{
		ASSERT(slot < m_iCBufNum);

		//	Update constant buffer
		UpdateConstBuffer(pMate, slot);
		//	Commit buffer to device
		pDevContext->CSSetConstantBuffers(slot, 1, &m_aCBufs[slot].pCBuf);
	}
	else
	{
		ASSERT(m_iCBufNum == pMate->GetConstBufNum());

		ID3D11Buffer* aBufs[MAX_CB_NUM];

		for (int i = 0; i < m_iCBufNum; i++)
		{
			//	Update constant buffer
			UpdateConstBuffer(pMate, i);

			aBufs[i] = m_aCBufs[i].pCBuf;
		}

		//	Commit buffer to device
		pDevContext->CSSetConstantBuffers(0, m_iCBufNum, aBufs);
	}

	return true;
}

bool CDXComputeShader::UpdateConstBuffer(CDXComputeShaderMate* pMate, int slot)
{
	CONST_BUF& cb = m_aCBufs[slot];

	int iCBufLen = pMate->GetConstBufLen(slot);
	if (iCBufLen != cb.iCBLen)
	{
		ASSERT(0 && "Constant buffer length mis-match !");
		return false;
	}

	if (!iCBufLen)
		return true;

	ASSERT(cb.pCBuf);

	ID3D11DeviceContext* pDevContext = DXRenderer::Instance()->GetDeviceContext();

	//	Lock constant buffer
	D3D11_MAPPED_SUBRESOURCE MappedRes;
	HRESULT result = pDevContext->Map(cb.pCBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedRes);
	if (FAILED(result))
		return false;

	memcpy(MappedRes.pData, pMate->GetConstBufData(slot), iCBufLen);

	pDevContext->Unmap(cb.pCBuf, 0);

	return true;
}

bool CDXComputeShader::Apply()
{
	ASSERT(m_pShader);

	ID3D11DeviceContext* pDevContext = DXRenderer::Instance()->GetDeviceContext();
	ASSERT(pDevContext);

	pDevContext->CSSetShader(m_pShader, NULL, 0);

	return true;
}

bool CDXComputeShader::Dispatch(int group_x, int group_y, int group_z)
{
	ASSERT(m_pShader);

	ID3D11DeviceContext* pDevContext = DXRenderer::Instance()->GetDeviceContext();
	ASSERT(pDevContext);

	pDevContext->Dispatch(group_x, group_y, group_z);

	return true;
}
