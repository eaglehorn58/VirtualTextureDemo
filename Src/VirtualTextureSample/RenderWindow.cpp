#include "Global.h"
#include "RenderWindow.h"
#include "VTMainWindow.h"
#include "qmessagebox.h"
#include "qevent.h"
#include "DXRenderer.h"
#include "AXViewport.h"
#include "AXCamera.h"
#include "GameScene.h"
#include "VTextureSystem.h"

CRenderWindow::CRenderWindow(QWidget *parent) :
QWidget(parent),
m_ptLastMouse(0, 0)
{
	m_pMainWnd = dynamic_cast<VTMainWindow*>(parent);

	this->setMouseTracking(true);
}

CRenderWindow::~CRenderWindow()
{

}

void CRenderWindow::resizeEvent(QResizeEvent* pEvent)
{
	QWidget::resizeEvent(pEvent);

	if (!DXRenderer::Instance()->IsInitOk())
		return;

	QSize size = pEvent->size();
	int width = size.width();
	int height = size.height();

	DXRenderer::Instance()->ResizeDevice(width, height);

	AXViewport* pViewport = m_pMainWnd->GetViewport();
	AXCamera* pCamera = m_pMainWnd->GetCamera();
	MyAssert(pViewport && pCamera);

	A3DVIEWPORTPARAM Params;
	Params.X = 0;
	Params.Y = 0;
	Params.Width = width;
	Params.Height = height;
	Params.MinZ = 0.0f;
	Params.MaxZ = 1.0f;

	pViewport->SetParam(&Params);

	//	Adjust camera
	float fRatio = (float)width / height;
	pCamera->SetProjectionParam(pCamera->GetFOV(), pCamera->GetZFront(), pCamera->GetZBack(), fRatio);

	//	Resize game scene
	CGameScene* pScene = m_pMainWnd->GetGameScene();
	if (pScene)
	{
		pScene->OnWindowResize(width, height);
	}

	//	VT handle resize
	CVTextureSystem* pVTSys = m_pMainWnd->GetVTSystem();
	if (pVTSys)
	{
		pVTSys->OnWindowResize(width, height);
	}
}

void CRenderWindow::mousePressEvent(QMouseEvent* pEvent)
{
	QWidget::mousePressEvent(pEvent);

	if (!g_AppConfigs.bCamAutoRun)
	{
		if (pEvent->button() == Qt::RightButton)
		{
			m_ptLastMouse.x = pEvent->x();
			m_ptLastMouse.y = pEvent->y();
		}
	}
}

void CRenderWindow::mouseReleaseEvent(QMouseEvent* pEvent)
{
	QWidget::mouseReleaseEvent(pEvent);
}

void CRenderWindow::mouseMoveEvent(QMouseEvent* pEvent)
{
	QWidget::mouseMoveEvent(pEvent);

	if (!g_AppConfigs.bCamAutoRun)
	{
		AXCamera* pCamera = g_pMainWnd->GetCamera();
		if (!pCamera)
			return;

		int cur_x = pEvent->x();
		int cur_y = pEvent->y();

		if (pEvent->buttons() & Qt::RightButton)
		{
			//	Rotate camera
			float fYaw = (cur_x - m_ptLastMouse.x) * g_AppConfigs.fCamRotateSpeed;
			pCamera->DegDelta(fYaw);
			float fPitch = -(cur_y - m_ptLastMouse.y) * g_AppConfigs.fCamRotateSpeed;
			pCamera->PitchDelta(fPitch);

			m_ptLastMouse.x = cur_x;
			m_ptLastMouse.y = cur_y;
		}
	}
}


