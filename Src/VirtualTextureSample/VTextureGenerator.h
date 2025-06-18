//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#pragma once

#include "ATypes.h"
#include "DXShaderMate.h"

///////////////////////////////////////////////////////////////////////////

class CDXSampler;
class CDXComputeShader;
class CCSMateAddBorder;

///////////////////////////////////////////////////////////////////////////
//	Class CVTextureGenerator
///////////////////////////////////////////////////////////////////////////

class CVTextureGenerator
{
public:		//	Types

public:		//	Attributes

public:		//	Operations

	CVTextureGenerator();
	virtual ~CVTextureGenerator();

	//	initialize 
	bool Init();
	//	release
	void Release();

	//	Process VT
	bool ProcessVT(const char* szFile, int iVPageSize, int iBorderSize);

protected:	//	Attributes

	CDXSampler*		m_pPtSampler;		//	point-wrap sampler

	CDXComputeShader*	m_pCSAddBorder;
	CCSMateAddBorder*	m_pcsmateAddBorder;

protected:	//	Operations

};

