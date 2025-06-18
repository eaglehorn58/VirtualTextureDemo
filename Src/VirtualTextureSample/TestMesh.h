//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#ifndef _TESTMESH_H_
#define _TESTMESH_H_

#include "DXShaderMate.h"
#include "AXVector.h"
#include "AXMatrix.h"
#include "AXTypes.h"

///////////////////////////////////////////////////////////////////////////

class CDXVertexBuffer;
class CDXIndexBuffer;
class CDXVertexShader;
class CDXPixelShader;
class AXCameraBase;

///////////////////////////////////////////////////////////////////////////

class CVSMateMeshDef : public CDXVertexShaderMate
{
public:		//	Types

	struct VS_CONST
	{
		AXMatrix4	matWVP;		//	world * view * projection
	};

public:		//	Attributes

	VS_CONST	data;		//	Constant data

public:		//	Operations

	CVSMateMeshDef();

	virtual int GetInputElemDescNum() override { return sizeof(m_layout) / sizeof(D3D11_INPUT_ELEMENT_DESC); }
	virtual const D3D11_INPUT_ELEMENT_DESC* GetInputElemDesc() override { return m_layout; }
	virtual int GetConstBufNum() override { return 1; }
	virtual int GetConstBufLen(int slot) override { return sizeof(data); }
	virtual const void* GetConstBufData(int slot) override { return &data; }

protected:	//	Attributes

	D3D11_INPUT_ELEMENT_DESC	m_layout[2];

protected:	//	Operations

};

class CPSMateMeshDef : public CDXPixelShaderMate
{
public:		//	Types

	struct PS_CONST
	{
		A3DCOLORVALUE	color;		//	Mesh color
	};

public:		//	Attributes

	PS_CONST	data;		//	Constant data

public:		//	Operations

	CPSMateMeshDef();

	virtual int GetConstBufNum() override { return 1; }
	virtual int GetConstBufLen(int slot) override { return sizeof(data); }
	virtual const void* GetConstBufData(int slot) override { return &data; }
};

class CPSMateMeshVT : public CDXPixelShaderMate
{
public:		//	Types

	struct PS_CONST
	{
		int		iVTSize;			//	VT texture size (mip 0 level)
		int		iVPageSize;			//	Virtual page size
		int		iVPageCntInRow;		//	Virtual page count in each row
		int		iMaxMipmap;			//	maximum mipmap level
		int		iPageIDDownscale;	//	downscale for gathering page ids
		int		iPageIDBufWid;		//	page id buffer width
		int		vPageIDDither[2];	//	dither for gathering page ids
		int		iCacheTexSize;		//	page cache texture (physical texture) size
		int		iBorderSize;		//	border size on each side
		float	padding[2];
	};

public:		//	Attributes

	PS_CONST	data;		//	Constant data

	//	fill constant buffer
	void FillCB();

public:		//	Operations

	CPSMateMeshVT();

	virtual int GetConstBufNum() override { return 1; }
	virtual int GetConstBufLen(int slot) override { return sizeof(data); }
	virtual const void* GetConstBufData(int slot) override { return &data; }
};

///////////////////////////////////////////////////////////////////////////
//
//	Class CTestMesh
//
///////////////////////////////////////////////////////////////////////////

class CTestMesh
{
public:		//	Types

	//	Vertex type, keep corresponding with layout in CVSMateTestMesh
	struct VERTEX_TYPE
	{
		AXVector3	vPos;	//	Position
		AXVector2	uv;		//	uv
	};

public:		//	Constructors and Destructors

	CTestMesh();
	virtual ~CTestMesh();

public:		//	Attributes

public:		//	Operations

	//	Intialize object
	bool Init();
	//	Release object
	void Release();

	//	Render
	bool RenderZPrePass(AXCameraBase* pCamera);
	bool Render(AXCameraBase* pCamera);

protected:	//	Attributes

	CDXVertexBuffer*	m_pvb;			//	Vertex buffer
	CDXIndexBuffer*		m_pib;			//	Index buffer

	CDXVertexShader*	m_pvsDef;		//	default vertex shader
	CDXPixelShader*		m_ppsDef;		//	default pixel shader
	CDXPixelShader*		m_ppsVT;		//	VT pixel shader
	CVSMateMeshDef		m_vsmateDef;
	CPSMateMeshDef		m_psmateDef;
	CPSMateMeshVT		m_psmateVT;

protected:	//	Operations
	
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_TESTMESH_H_

