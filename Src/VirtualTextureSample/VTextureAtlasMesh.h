//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#pragma once

#include "AXTypes.h"
#include "VTextureTypes.h"

///////////////////////////////////////////////////////////////////////////

class CVTextureSystem;
class CDXVertexShader;
class CDXPixelShader;
class CDXVertexBuffer;
class CVSMateVTAtlas;
class CPSMateVTAtlas;

///////////////////////////////////////////////////////////////////////////
//	Class CVTextureAtlasMesh
///////////////////////////////////////////////////////////////////////////

class CVTextureAtlasMesh
{
public:		//	Types

	//	Vertex type
	struct VERTEX_TYPE
	{
		AXVector3	vPos;	//	xy: uv; z: page index
	};

public:		//	Constructors and Destructors

	CVTextureAtlasMesh(CVTextureSystem* pVTSys);
	virtual ~CVTextureAtlasMesh();

public:		//	Attributes

public:		//	Operations

	//	Render mesh
	//	aCacheIDs: id of cache that needs updated
	//	iNumCache: number of page cache id in aCacheIDs
	//	bSkipDefPage: true, don't set default page
	void Render(const auint32* aCacheIDs, int iNumCache, bool bSkipDefPage);

protected:	//	Attributes

	CVTextureSystem*	m_pVTSys;
	CDXVertexBuffer*	m_pvb;			//	Vertex buffer
	CDXVertexShader*	m_pvsAltas;		//	vertex shader
	CDXPixelShader*		m_ppsAltas;		//	pixel shader
	CVSMateVTAtlas*		m_vsmateAltas;
	CPSMateVTAtlas*		m_psmateAltas;

protected:	//	Operations

};

