//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#ifndef _FORMCONTAINER_H_
#define _FORMCONTAINER_H_

#include "qwidget.h"

class CFormBase;

class CFromContainer : public QWidget
{
	Q_OBJECT

public:

public:

	CFromContainer(QWidget* parent, const char* szTitle, CFormBase* pFrom);

public:

	//	Update UI
	void UpdateUI();

private:

	CFormBase*	m_pForm;

protected slots:

protected:

protected:

};

#endif // _FORMCONTAINER_H_
