//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com

#ifndef _AX_CAMERA_H_
#define _AX_CAMERA_H_

#include "AXCameraBase.h"

class AXCamera	: public AXCameraBase
{
public:		//	Types

public:		//	Constructor and Destructor

	AXCamera();
	virtual ~AXCamera() {}

public:		//	Attributes

public:		//	Operations

	//	Initlaize object
	
	bool Init(float vFov=DEG2RAD(65.0f), float vFront=1.0f, float vBack=2000.0f, float vRatio=1.3333333f);
	
	//	Release object
	virtual void Release();

	//	Is this a perspective camera ?
	//virtual bool IsPerspective() const { return true; }
	//	Is this a orthogonal camera ?
	//virtual bool IsOrthogonal() const { return false; }

	virtual AXCameraType GetType() const override { return ACT_PERSPECTIVE; }

	//	Apply camera to device
	virtual bool Active();
	//	Update function is called in Active()
	bool Update();

	//	Set / Get FOV
	void SetFOV(float vFOV) { m_vFOV = vFOV; UpdateProjectTM(); }
	float GetFOV() const { return m_vFOV; }
	void SetFOVSet(float vFOVSet) { m_vFOVSet = vFOVSet; }
	float GetRatio() const { return m_vRatio; }

	//	Set / Get projection parameters
	bool SetProjectionParam(float fFOV, float fFront, float fBack, float fRatio);
	void GetProjectionParam(float* pvFOV, float* pvFront, float* pvBack, float* pvRatio) const
	{ 
		*pvFOV	 = m_vFOV;
		*pvFront = m_vFront; 
		*pvBack	 = m_vBack; 
		*pvRatio = m_vRatio;
	}

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

	virtual void SetMirror(const AXCameraBase* pOrgCamera, const AXMirrorParam& MirrorParam) override;

protected:	//	Attributes

	float	m_vFOV;
	float	m_vFOVSet;
	float	m_vRatio;

protected:	//	Operations

	//	Update project TM
	virtual bool UpdateProjectTM();
	//	Create view frustum
	virtual bool CreateViewFrustum();
	//	Update world frustum
	virtual bool UpdateWorldFrustum();
};


class AXCameraOffCenter : public AXCameraBase
{
public:
	AXCameraOffCenter();
	virtual			~AXCameraOffCenter() {}

	abool			Init(afloat32 fLeft = -1.0f, afloat32 fRight = 1.0f, afloat32 fBottom = -1.0f, afloat32 fTop = 1.0f, afloat32 fFront = 1.0f, afloat32 fBack = 2000.0f);
	virtual void	Release();

	//	Is this a perspective camera ?
	//virtual abool	IsPerspective() const override { return atrue; }
	//	Is this a orthogonal camera ?
	//virtual abool	IsOrthogonal() const override { return afalse; }

	virtual AXCameraType GetType() const override { return ACT_PERSPECTIVE_OFFCENTER; }

	abool			SetProjectionParam(afloat32 fLeft, afloat32 fRight, afloat32 fBottom, afloat32 fTop, afloat32 fFront, afloat32 fBack);

	virtual void	SetMirror(const AXCameraBase* pOrgCamera, const AXMirrorParam& MirrorParam) override;

	//	NOTE!! 未实现
	virtual aint32	CalcFrustumCorners(AXVector3* aCorners, bool bInViewSpace, float fNear, float fFar) override { return 8; }

protected:
	//	Update project TM
	virtual bool	UpdateProjectTM() override;
	//	Create view frustum
	virtual bool	CreateViewFrustum() override;
	//	Update world frustum
	virtual bool	UpdateWorldFrustum() override;

protected:
	afloat32		m_fLeft;
	afloat32		m_fRight;
	afloat32		m_fBottom;
	afloat32		m_fTop;
};

#endif
