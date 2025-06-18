//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#ifndef _SCREENQUAD_H_
#define _SCREENQUAD_H_

#include "ATypes.h"
#include "AXVector.h"
#include "DXShaderMate.h"
#include "ARect.h"

///////////////////////////////////////////////////////////////////////////

class CDXVertexShader;
class CDXPixelShader;
class CDXTexture;
class CDXUATexture;
class CDXSampler;
class CDXRenderState;

///////////////////////////////////////////////////////////////////////////

class CVSMateScreenQuad : public CDXVertexShaderMate
{
public:		//	Types

	struct VS_CONST
	{
		AXVector4	vRect;			//	x, y, z, w: left, top, right, bottom
		float		fScnWid;		//	screen width
		float		fScnHei;		//	screen height
		float		padding[2];	
	};

public:		//	Attributes

	VS_CONST	data;		//	Constant data

public:		//	Operations

	CVSMateScreenQuad()
	{
		memset(&data, 0, sizeof(data));
	}

	virtual int GetInputElemDescNum() override { return 0; }
	virtual const D3D11_INPUT_ELEMENT_DESC* GetInputElemDesc() override { return NULL; }
	virtual int GetConstBufNum() override { return 1; }
	virtual int GetConstBufLen(int slot) override { return sizeof(data); }
	virtual const void* GetConstBufData(int slot) override { return &data; }

protected:	//	Attributes

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Class CScreenQuad
//
///////////////////////////////////////////////////////////////////////////

class CScreenQuad
{
public:		//	Types

	//	Draw parameters
	struct DRAW_PARAMS
	{
		CDXTexture*		pTex;	//	specify a texture to quad
		CDXRenderState*	pRS;	//	specify a render state to quad

		DRAW_PARAMS() : pTex(NULL), pRS(NULL) {}
	};

public:		//	Constructors and Destructors

	CScreenQuad();
	virtual ~CScreenQuad();

public:		//	Attributes

public:		//	Operations

	//	Draw quad on screen.
	//	this funciton only apply vertex shader, leave all other	render states un-touched
	bool Render();

	//	set/get full screen flag
	void SetFullScreen(bool bFull) { m_bFullScreen = bFull; }
	bool GetFullScreen() const { return m_bFullScreen; }
	//	set/get quad size
	void SetQuad(int left, int top, int right, int bottom) { m_rcQuad.SetRect(left, top, right, bottom); }
	const ARectI& GetQuad() const { return m_rcQuad; }

protected:	//	Attributes

	bool		m_bFullScreen;			//	full-screen flag
	ARectI		m_rcQuad;				//	Quad size

	CDXVertexShader*	m_pvsFull;		//	draw full-screen quad
	CDXVertexShader*	m_pvsQuad;		//	draw quad with m_rcQuad size
	CVSMateScreenQuad	m_vsMate;

protected:	//	Operations
	
	//	Apply vertex shader
	void ApplyVS();
};

///////////////////////////////////////////////////////////////////////////
//
//	Class CTexScreenQuad
//
///////////////////////////////////////////////////////////////////////////

class CTexScreenQuad : public CScreenQuad
{
public:		//	Types

public:		//	Constructors and Destructors

	CTexScreenQuad();
	virtual ~CTexScreenQuad();

public:		//	Attributes

public:		//	Operations

	//	Draw quad on screen, with specified texture.
	//	the function apply internal VS, PS and render states to device
	bool Render(CDXTexture* pTex);
	bool Render(CDXUATexture* pTex);

protected:	//	Attributes

	CDXRenderState*		m_pRS;			//	internal render state
	CDXSampler*			m_pSampler;		//	internal sampler for internal PS
	CDXPixelShader*		m_pps;			//	internal PS

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_SCREENQUADMESH_H_

