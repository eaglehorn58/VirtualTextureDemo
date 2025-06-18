//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com

#ifndef _FORMVIEWVPAGE_H_
#define _FORMVIEWVPAGE_H_

#include "FormBase.h"
#include "ui_FormViewVPage.h"

class CFormViewVPage : public CFormBase
{
	Q_OBJECT

public:

public:

	CFormViewVPage();
	virtual ~CFormViewVPage();

public:

	//	Update UI
	virtual void UpdateUI() override;

protected:

	Ui_FormViewVPage	ui;

protected:

protected slots:

	void OnChooseVPage();
	void OnCheckShowVPage(bool bChecked);
	void OnMipmapChanged(int value);
	void OnCheckShowVPageCache(bool bChecked);
};


#endif	//	_FORMVIEWVPAGE_H_