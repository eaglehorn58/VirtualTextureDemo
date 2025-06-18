//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com

#include "Global.h"
#include "FormCamera.h"
#include "A3DMacros.h"
#include "VTMainWindow.h"

#define MAX_ROT_SPEED_RANGE		1000
#define MAX_ROT_SPEED			0.5f
#define MAX_MOV_SPEED_RANGE		1000
#define MAX_MOV_SPEED			1000.0f

CFormCamera::CFormCamera() :
CFormBase(NULL)
{
	ui.setupUi(this);

	this->setMinimumSize(this->size());

	//	Setup rotate speed slide
	GLB_SLIDER_INIT(ui.slideRotSpeed, g_AppConfigs.fCamRotateSpeed, MAX_ROT_SPEED, MAX_ROT_SPEED_RANGE);
	//	Setup move speed slide
	GLB_SLIDER_INIT(ui.slideMoveSpeed, g_AppConfigs.fCamMoveSpeed, MAX_MOV_SPEED, MAX_MOV_SPEED_RANGE);

	connect(ui.slideRotSpeed, SIGNAL(valueChanged(int)), this, SLOT(OnRotateSpeedChanged(int)));
	connect(ui.slideMoveSpeed, SIGNAL(valueChanged(int)), this, SLOT(OnMoveSpeedChanged(int)));
	connect(ui.btnAutoRun, SIGNAL(clicked(bool)), this, SLOT(OnBtnAutoRun(bool)));
}

CFormCamera::~CFormCamera()
{
}

void CFormCamera::OnRotateSpeedChanged(int value)
{
	float f = ((float)value / MAX_ROT_SPEED_RANGE) * MAX_ROT_SPEED;
	g_AppConfigs.fCamRotateSpeed = f;
}

void CFormCamera::OnMoveSpeedChanged(int value)
{
	float f = ((float)value / MAX_MOV_SPEED_RANGE) * MAX_MOV_SPEED;
	g_AppConfigs.fCamMoveSpeed = f;
}

void CFormCamera::OnBtnAutoRun(bool bChecked)
{
	g_AppConfigs.bCamAutoRun = bChecked;
	g_pMainWnd->StartCameraAutoRun(bChecked);
}

