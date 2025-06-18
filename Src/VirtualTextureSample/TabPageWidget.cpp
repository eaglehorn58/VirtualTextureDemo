//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#include "Global.h"
#include "qboxlayout.h"
#include "qpushbutton.h"
#include "qscrollbar.h"
#include "TabPageWidget.h"
#include "FormContainer.h"

CTabPageWidget::CTabPageWidget(QWidget* parent) :
QScrollArea(parent)
{
	this->setContentsMargins(0, 0, 0, 0);
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	this->setMinimumWidth(450 + 20);// sizeVSBar.height());

	QWidget* pContainer = new QWidget(this);
	pContainer->setContentsMargins(0, 0, 0, 0);

	m_pLayout = new QVBoxLayout(pContainer);
	m_pLayout->setContentsMargins(0, 0, 0, 0);
	m_pLayout->setSizeConstraint(QLayout::SetFixedSize);

	pContainer->setLayout(m_pLayout);

	this->setWidgetResizable(true);		//	Adjust size with parent
	this->setWidget(pContainer);
}

bool CTabPageWidget::AddContainer(const char* szTitle, CFormBase* pFormBase)
{
	CFromContainer* pFormContainer = new CFromContainer(this, szTitle, pFormBase);
	m_pLayout->addWidget(pFormContainer);
	m_aForms.push_back(pFormContainer);
	return true;
}

void CTabPageWidget::UpdateUI()
{
	int iNumForm = (int)m_aForms.size();
	for (int i = 0; i < iNumForm; i++)
	{
		CFromContainer* pFormContainer = m_aForms[i];
		pFormContainer->UpdateUI();
	}
}

