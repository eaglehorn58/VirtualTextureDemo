//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#ifndef _AX_ORTHOCAMERA_H_
#define _AX_ORTHOCAMERA_H_

#include "AXCameraBase.h"

///////////////////////////////////////////////////////////////////////////

class AXOrthoCamera : public AXCameraBase
{
public:		//	Types

public:		//	Constructor and Destructor

	AXOrthoCamera();
	virtual ~AXOrthoCamera() {}

public:		//	Attributes

public:		//	Operations

	//	Initialize camera object
	bool Init(/*A3DDevice* pA3DDevice, */float l, float r, float b, float t, float zn, float zf);
	//	Release object
	virtual void Release();

	//	Is this a perspective camera ?
	//virtual bool IsPerspective() { return false; }
	//	Is this a orthogonal camera ?
	//virtual bool IsOrthogonal() { return true; }

	virtual AXCameraType GetType() const override { return ACT_ORTHOGONAL; }

	//	Set / Get projection parameters
	bool SetProjectionParam(float l, float r, float b, float t, float zn, float zf);
	float GetLeft() { return m_fLeft; }
	float GetRight() { return m_fRight; }
	float GetTop() { return m_fTop; }
	float GetBottom() { return m_fBottom; }

	/*	Calculate view frustum's 8 corners' position

		Return number of corners;

		aCorners (out): array used to receive corners position, you can pass NULL to get how
				many corners by return value. 
				corner index is defined as below:
				0 - 3: near plane corners, left-bottom, left-top, right-top, right-bottom
				4 - 7: far plane corners, left-bottom, left-top, right-top, right-bottom
		bInViewSpace: true, positions are in view space; false, in world space
		fNear, fFar: near and far distance.
	*/
	virtual int CalcFrustumCorners(AXVector3* aCorners, bool bInViewSpace, float fNear, float fFar);
protected:	//	Attributes
	
	float	m_fLeft;
	float	m_fRight;
	float	m_fTop;
	float	m_fBottom;

protected:	//	Operations

	//	Update project TM
	virtual bool UpdateProjectTM();
	//	Create view frustum
	virtual bool CreateViewFrustum();
	//	Update world frustum
	virtual bool UpdateWorldFrustum();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_AX_ORTHOCAMERA_H_
