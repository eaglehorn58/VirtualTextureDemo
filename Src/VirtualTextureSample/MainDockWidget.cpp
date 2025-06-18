//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#include "Global.h"
#include "qtabwidget.h"
#include "MainDockWidget.h"
#include "TabPageWidget.h"
#include "FormCamera.h"
#include "FormViewVPage.h"

//	Page names
static const char* _PageNames[CMainDockWidget::NUM_PAGE] = 
{
	"General",
};

CMainDockWidget::CMainDockWidget(QWidget *parent, Qt::WindowFlags flags) :
QDockWidget(parent, flags)
{
	this->setWindowTitle("Tool Bar");
	this->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

	m_pTab = new QTabWidget(this);

	//	Create "General" page
	{
		CTabPageWidget* pPage = new CTabPageWidget(m_pTab);
		m_aPages[PAGE_GENERAL] = pPage;

		{
			CFormCamera* pForm = new CFormCamera;
			pPage->AddContainer("Camera", pForm);
		}

		{
			CFormViewVPage* pForm = new CFormViewVPage;
			pPage->AddContainer("View Virtual Page", pForm);
		}
	}

	//	Add pages to table widget
	for (int i = 0; i < NUM_PAGE; i++)
	{
		m_pTab->addTab(m_aPages[i], _PageNames[i]);
	}

	this->setWidget(m_pTab);

	connect(m_pTab, SIGNAL(currentChanged(int)), this, SLOT(OnCurrentPageChanged(int)));
}

void CMainDockWidget::UpdateUI(bool bAllPages)
{
	if (bAllPages)
	{
		//	Force to update all pages
		for (int i = 0; i < NUM_PAGE; i++)
		{
			m_aPages[i]->UpdateUI();
		}
	}
	else
	{
		//	Only update current active page
		int index = m_pTab->currentIndex();
		if (index >= 0 && index < NUM_PAGE)
		{
			m_aPages[index]->UpdateUI();
		}
	}
}

void CMainDockWidget::OnCurrentPageChanged(int index)
{
	//	Update current page's content
	UpdateUI(index);
}

