//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com

#include "Global.h"
#include "FormViewVPage.h"
#include "VTMainWindow.h"
#include "VTextureSystem.h"
#include "VPageViewer.h"
#include "qvalidator.h"

CFormViewVPage::CFormViewVPage() :
CFormBase(NULL)
{
	ui.setupUi(this);

	this->setMinimumSize(this->size());

//	ui.editU->setValidator(new QDoubleValidator(0.0f, 1.0f, 10, this));
//	ui.editV->setValidator(new QDoubleValidator(0.0f, 1.0f, 10, this));
	ui.editU->setValidator(new QIntValidator(0, 4096, this));
	ui.editV->setValidator(new QIntValidator(0, 4096, this));
	ui.slideMipmap->setMinimum(0);
	ui.slideMipmap->setMaximum(0);
	ui.editMipmap->setText(QString::number(0));
	ui.checkShowVPage->setChecked(false);
	ui.editPageSize->setText(QString::number(0));
	ui.checkShowVPageCache->setChecked(false);

	connect(ui.btnChooseVPage, SIGNAL(released()), this, SLOT(OnChooseVPage()));
	connect(ui.checkShowVPage, SIGNAL(clicked(bool)), this, SLOT(OnCheckShowVPage(bool)));
	connect(ui.slideMipmap, SIGNAL(valueChanged(int)), this, SLOT(OnMipmapChanged(int)));
	connect(ui.checkShowVPageCache, SIGNAL(clicked(bool)), this, SLOT(OnCheckShowVPageCache(bool)));
}

CFormViewVPage::~CFormViewVPage()
{
}

void CFormViewVPage::UpdateUI()
{
	CVTextureSystem* pVTSys = g_pMainWnd->GetVTSystem();
	CVPageViewer* pVPageViewer = g_pMainWnd->GetVPageViewer();

	if (pVTSys && pVPageViewer)
	{
		const CVPageViewer::POSITION& pos = pVPageViewer->GetPos();
		ui.editU->setText(QString::number(pos.x));
		ui.editV->setText(QString::number(pos.y));

		ui.slideMipmap->setMinimum(0);
		ui.slideMipmap->setMaximum(pVTSys->GetMipmapCnt() - 1);
		ui.slideMipmap->setPageStep(1);
		ui.slideMipmap->setSliderPosition(pVPageViewer->GetMipmap());
		ui.editMipmap->setText(QString::number(pVPageViewer->GetMipmap()));
		ui.checkShowVPage->setChecked(g_AppConfigs.bShowPageViewer);
		ui.editPageSize->setText(QString::number(pVTSys->GetVPageSize()));
	}
	else
	{
		ui.editU->setText(QString::number(0.0f));
		ui.editV->setText(QString::number(1.0f));

		ui.slideMipmap->setMinimum(0);
		ui.slideMipmap->setMaximum(0);
		ui.slideMipmap->setPageStep(1);
		ui.slideMipmap->setSliderPosition(0);
		ui.editMipmap->setText(QString::number(0));
		ui.checkShowVPage->setChecked(false);
		ui.editPageSize->setText(QString::number(0));
	}
}

void CFormViewVPage::OnChooseVPage()
{
	CVPageViewer* pVPageViewer = g_pMainWnd->GetVPageViewer();
	if (!pVPageViewer)
		return;

//	float u = ui.editU->text().toFloat();
//	float v = ui.editV->text().toFloat();
	int x = ui.editU->text().toInt();
	int y = ui.editV->text().toInt();
	int mipmap = ui.slideMipmap->sliderPosition();
	pVPageViewer->SetPage(x, y, mipmap);
}

void CFormViewVPage::OnCheckShowVPage(bool bChecked)
{
	g_AppConfigs.bShowPageViewer = bChecked;
}

void CFormViewVPage::OnMipmapChanged(int value)
{
	int mipmap = ui.slideMipmap->sliderPosition();
	ui.editMipmap->setText(QString::number(mipmap));
}

void CFormViewVPage::OnCheckShowVPageCache(bool bChecked)
{
	g_AppConfigs.bShowVPageCache = bChecked;
}

