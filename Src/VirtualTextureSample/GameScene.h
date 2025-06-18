//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#ifndef _GAMESCENE_H_
#define _GAMESCENE_H_

#include "ATypes.h"
#include "AXVector.h"
#include "DXShaderMate.h"
#include "ARect.h"

///////////////////////////////////////////////////////////////////////////

class CDXRenderState;
class AXCameraBase;
class CTestMesh;
class CDXRenderTarget;

///////////////////////////////////////////////////////////////////////////

class CGameScene
{
public:		//	Types

public:		//	Constructors and Destructors

	CGameScene();
	virtual ~CGameScene();

public:		//	Attributes

	//	Initialize
	bool Init();
	//	Release
	void Release();

public:		//	Operations

	//	On window resize
	void OnWindowResize(int width, int height);

	//	Tick
	void Tick(float time);
	//	Render
	bool Render(AXCameraBase* pCamera);

protected:	//	Attributes

	CDXRenderTarget*	m_pRT;				//	Render target
	CTestMesh*			m_pTestMesh;

	CDXRenderState*		m_prsZPrePass;		//	Z pre pass render states
	CDXRenderState*		m_prsLight;			//	Light pass render states

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_GAMESCENE_H_

