//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#ifndef _TABPAGEWIDGET_H_
#define _TABPAGEWIDGET_H_

#include "qscrollarea.h"
#include "vector.h"

class CFromContainer;
class CFormBase;
class QVBoxLayout;

class CTabPageWidget : public QScrollArea
{
	Q_OBJECT

public:

public:

	CTabPageWidget(QWidget* parent);

public:

	//	Add a form container
	bool AddContainer(const char* szTitle, CFormBase* pFormBase);
	//	Update UI
	void UpdateUI();

private:

	QVBoxLayout*	m_pLayout;

	abase::vector<CFromContainer*>	m_aForms;	//	Forms

protected slots:

protected:

protected:

};

#endif // _TABPAGEWIDGET_H_
