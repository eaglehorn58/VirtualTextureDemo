//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com

#include "Global.h"
#include "qpushbutton.h"
#include "qboxlayout.h"
#include "FormContainer.h"
#include "FormBase.h"
#include "A3DMacros.h"

CFromContainer::CFromContainer(QWidget* parent, const char* szTitle, CFormBase* pFrom) :
QWidget(parent),
m_pForm(pFrom)
{
	this->setContentsMargins(0, 0, 0, 0);
	this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	//	Create layout
	QVBoxLayout* pLayout = new QVBoxLayout(this);
	pLayout->setContentsMargins(0, 0, 0, 0);
	pLayout->setSizeConstraint(QLayout::SetFixedSize);

	//	Create title bar
	QPushButton* pTitleBtn = new QPushButton(this);
	pTitleBtn->setText(szTitle);
	glb_SetWidgetBkColor(pTitleBtn, A3DCOLORRGB(191, 232, 242));
//	pTitleBtn->setStyleSheet("background-color: rgb()");
	pLayout->addWidget(pTitleBtn);

	//	Set central widget
	pFrom->setParent(this);
	pLayout->addWidget(pFrom);
}

void CFromContainer::UpdateUI()
{
	if (m_pForm)
		m_pForm->UpdateUI();
}