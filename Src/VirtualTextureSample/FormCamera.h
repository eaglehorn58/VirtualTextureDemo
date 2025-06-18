//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com

#ifndef _FORMCAMERA_H_
#define _FORMCAMERA_H_

#include "FormBase.h"
#include "ui_FormCamera.h"

class CFormCamera : public CFormBase
{
	Q_OBJECT

public:

public:

	CFormCamera();
	virtual ~CFormCamera();

public:

protected:

	Ui::FormCamera	ui;

protected:

protected slots:

	void OnRotateSpeedChanged(int value);
	void OnMoveSpeedChanged(int value);
	void OnBtnAutoRun(bool bChecked);
};


#endif	//	_FROMCAMERA_H_