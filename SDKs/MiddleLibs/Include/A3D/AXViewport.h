/*
 * FILE: AXViewport.h
 *
 * DESCRIPTION: Class standing for a viewport
 *
 * CREATED BY: Hedi, 2001/7/27
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _AX_VIEWPORT_H_
#define _AX_VIEWPORT_H_

#include "AXTypes.h"

class AXCameraBase;

class AXViewport //: public AMemBase
{
public:
	AXViewport();
	~AXViewport();

	bool Init(A3DVIEWPORTPARAM * pParam);
	
	bool Release();

	// Set Viewport's parameters;
	bool SetParam(A3DVIEWPORTPARAM * pParam);

	// Transform a world space point to screen coordinates, vecOut is a screen point;
	bool Transform(const AXVector3& vecIn, AXVector3& vecOut);
	// Transform a screen point to world space coordinates, vecIn is a screen point;
	bool InvTransform(const AXVector3& vecIn, AXVector3& vecOut);

	bool SetCamera(AXCameraBase* pA3DCamera);
	
	inline A3DVIEWPORTPARAM * GetParam() { return &m_ViewportParam; }
	inline AXCameraBase* GetCamera() { return m_pA3DCamera; }
	inline const AXMatrix4 GetViewScale() { return m_matViewScale; }

	inline const A3DCOLORVALUE& GetClearColor() { return m_ClearColor;}
	inline void			SetClearColor(const A3DCOLORVALUE& clearColor) { m_ClearColor = clearColor;}

private:
	A3DVIEWPORTPARAM	m_ViewportParam;
	AXCameraBase*		m_pA3DCamera;

	AXMatrix4			m_matViewScale; //FIXME!! perhaps need to be deleted.
	AXMatrix4			m_matInvViewScale;

	A3DCOLORVALUE		m_ClearColor;
};

#endif