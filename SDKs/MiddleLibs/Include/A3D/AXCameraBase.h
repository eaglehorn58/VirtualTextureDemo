/*
 * FILE: AXCameraBase.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/6/12
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AX_CAMERA_BASE_H_
#define _AX_CAMERA_BASE_H_

#include "AXTypes.h"
#include "AXFrustum.h"
#include "A3DMacros.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

enum AXCameraType
{
	ACT_INVALID = -1,
	ACT_PERSPECTIVE,
	ACT_ORTHOGONAL,
	ACT_PERSPECTIVE_OFFCENTER
};

struct AXScreenBasedMirrorParam
{
	afloat32	fMirrorRadius;
	afloat32	fMinNearClip;

	AXScreenBasedMirrorParam()
		: fMirrorRadius(1.0f), fMinNearClip(1.0f) {}
};
struct AXMirrorParam
{
	AXVector3					vMirrorOrg;
	AXVector3					vMirrorNormal;
	AXScreenBasedMirrorParam	SBParam;

	AXMirrorParam()
		: vMirrorOrg(0, 0, 0), vMirrorNormal(0, 1, 0) {}
};


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class AXCameraBase
//	
///////////////////////////////////////////////////////////////////////////

//class AXCameraBase/*	: public A3DObject*/ : public AMemBase
class AXCameraBase
{
public:		//	Types

public:		//	Constructor and Destructor

	AXCameraBase();
	virtual ~AXCameraBase();

public:		//	Attributes

public:		//	Operations

	//	Initlaize object
	bool Init(float vFront, float vBack);

	//	Tick every frame
	void BeginRender(aint32 nDeltaTime, abool bClearPreMatrix = afalse);
	void EndRender();

	//	Release object
	virtual void Release() {}

	//	Is this a perspective camera ?
	virtual bool IsPerspective() const { return ((GetType() == ACT_PERSPECTIVE) || (GetType() == ACT_PERSPECTIVE_OFFCENTER)); }
	//	Is this a orthogonal camera ?
	virtual bool IsOrthogonal() const { return (GetType() == ACT_ORTHOGONAL); }

	virtual AXCameraType GetType() const { return ACT_INVALID; }

	//	Set camera's direction and up
	void SetDirAndUp(const AXVector3& vecDir, const AXVector3& vecUp);

	//	Camera moving routines
	bool MoveFront(float vDistance);
	bool MoveBack(float vDistance);
	bool MoveLeft(float vDistance);
	bool MoveRight(float vDistance);
	bool Move(const AXVector3& vecDelta);

	//	Camera rotating routines
	bool TurnAroundAxis(const AXVector3& vecPos, const AXVector3& vecAxis, float vDeltaRad);
	bool DegDelta(float vDelta);
	bool PitchDelta(float vDelta);
	bool SetDeg(float vDeg);
	bool SetPitch(float vPitch);

	//	Please don't call this function only once per tick, or maybe for several tick,
	//	so we can save some cpu usage;
	bool UpdateEar();
	
	//	Check whether aabb is in view frustum
	virtual bool AABBInViewFrustum(const AXAABB& aabb);
	virtual bool AABBInViewFrustum(const AXVector3& vMins, const AXVector3& vMaxs);
	//	Check whether sphere is in view frustum
	virtual bool SphereInViewFrustum(const AXVector3& vCenter, float fRadius);
	//	Check wether point is in view frustum
	virtual bool PointInViewFrustum(const AXVector3& vPos);

	//	Transform position from world space to cuboid coordinates space
	virtual bool Transform(const AXVector3& vecIn, AXVector3& vecOut);
	//	Transform position from coboid coordinates to world space
	virtual bool InvTransform(const AXVector3& vecIn, AXVector3& vecOut);

	//	Get camera's position
	const AXVector3& GetPos() const;

	//	Set mirror camera
	virtual void SetMirror(const AXCameraBase* pOrgCamera, const AXMirrorParam& MirrorParam) { ASSERT(afalse); }

	bool SetViewTM(const AXMatrix4& matView);
	bool SetProjectionTM(const AXMatrix4& matProjection);
	
	//	Set / Get Z front and back distance
	bool SetZFrontAndBack(float fFront, float fBack, bool bUpdateProjMat=true);
	float GetZFront() const { return m_vFront; }
	float GetZBack() const { return m_vBack; }

	//	UpdateViewTM should be called after set pos; but we should think
	//	carefully about when to call it;
	void SetPos(const AXVector3& vecPos) { m_vecPos = vecPos; UpdateViewTM(); }
	const AXMatrix4& GetViewTM() const { return m_matViewTM; }
	const AXMatrix4& GetProjectionTM() const { return m_matProjectTM; }
	const AXMatrix4& GetVPTM() const { return m_matVPTM; }
	const AXMatrix4& GetPreViewTM() const { return m_preMatViewTM; }
	const AXMatrix4& GetPreProjectionTM() const { return m_preMatProjectTM; }
	const AXMatrix4& GetPreVPTM() const { return m_preMatVPTM; }
	const AXVector3& GetDir() const { return m_vecDir; }
	const AXVector3& GetDirH() const { return m_vecDirH; }
	const AXVector3& GetUp() const { return m_vecUp; }
	const AXVector3& GetRight() const { return m_vecRight; }
	AXVector3 GetRightH() const { return -1.0f * m_vecLeftH; }
	const AXVector3& GetLeft() const { return m_vecLeft; }
	const AXVector3& GetLeftH() const { return m_vecLeftH; }
	float GetDeg() const { return m_vDeg; }
	float GetPitch() const { return m_vPitch; }

	//	Set / Get post project TM
	const AXMatrix4& GetPostProjectTM() const { return m_matPostProjectTM; }
	void SetPostProjectTM(const AXMatrix4& matPostProjectTM) { m_matPostProjectTM = matPostProjectTM; UpdateProjectTM(); }

	//	Get view frustum in world space
	AXFrustum* GetWorldFrustum() { return &m_WorldFrustum; }
	AXFrustum* GetViewFrustum()  { return &m_ViewFrustum; }

	// mirror information
	inline bool IsMirrored() const { return m_bMirrored; }

	virtual int CalcFrustumCorners(AXVector3* aCorners, bool bInViewSpace, float fNear, float fFar) { assert(0); return 0; }

protected:	//	Attributes

	AXVector3			m_vecPos;
	AXVector3			m_vecDir;
	AXVector3			m_vecUp;
	AXVector3			m_vecLeft;
	AXVector3			m_vecRight;
	AXVector3			m_vecDirH;
	AXVector3			m_vecLeftH;

	AXMatrix4			m_matViewTM;
	AXMatrix4			m_matProjectTM;
	AXMatrix4			m_matVPTM;
	AXMatrix4			m_matPostProjectTM;	// Transform matrix used for post project process

	abool				m_bFirstFrame;
	AXMatrix4			m_preMatViewTM;
	AXMatrix4			m_preMatProjectTM;
	AXMatrix4			m_preMatVPTM;

	AXMatrix4			m_matInvProjectTM;
	AXMatrix4			m_matInvVPTM;

	float				m_vDeg;
	float				m_vPitch;

	float				m_vFront;
	float				m_vBack;

	//	Frustum in view coordinates and world coordinates
	AXFrustum			m_ViewFrustum;
	AXFrustum			m_WorldFrustum;

	bool				m_bMirrored;

protected:	//	Operations

	//	Update view transform matrix
	bool UpdateViewTM();
	//	Update camera's direction and up
	bool UpdateDirAndUp();

	//	Update project TM
	virtual bool UpdateProjectTM() { assert(0); return false; }
	//	Create view frustum
	virtual bool CreateViewFrustum() { assert(0); return false; }
	//	Update world frustum
	virtual bool UpdateWorldFrustum() { assert(0); return false; }
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_A3DCAMERABASE_H_
