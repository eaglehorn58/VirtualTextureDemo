#include "Global.h"
#include "qevent.h"
#include "VTMainWindow.h"
#include "AXViewport.h"
#include "AXCamera.h"
#include "A3DMacros.h"
#include "axfuncs.h"
#include "RenderWindow.h"
#include "MainDockWidget.h"
#include "DXRenderer.h"
#include "TestMesh.h"
#include "DlgGenVirtualTexture.h"
#include "VTextureSystem.h"
#include "VPageViewer.h"
#include "VTextureAtlas.h"
#include "ScreenQuad.h"
#include "GameScene.h"
#include "qfiledialog.h"

VTMainWindow*	g_pMainWnd = NULL;

VTMainWindow::VTMainWindow(QWidget *parent) :
QMainWindow(parent),
m_pRenderWnd(nullptr),
m_pMainDock(nullptr),
m_pScene(nullptr),
m_pViewport(nullptr),
m_pCamera(nullptr),
m_pVTSys(nullptr),
m_pVPageViewer(nullptr),
m_pTexQuad(nullptr),
m_uCamMoveFlag(0),
m_bAllInitDone(false)
{
	MyAssert(!g_pMainWnd);
	g_pMainWnd = this;

	ui.setupUi(this);

	m_pRenderWnd = new CRenderWindow(this);
	m_pRenderWnd->setMinimumSize(100, 100);
	m_pRenderWnd->show();
	setCentralWidget(m_pRenderWnd);

	//	Setup UI
	SetupMenuBar();
	SetupDockWidgets();
}

VTMainWindow::~VTMainWindow()
{
}

void VTMainWindow::SetupMenuBar()
{

	//	------------------------------------------
	//	File menu
	//	------------------------------------------
	QMenu* pMenuFile = menuBar()->addMenu(tr("&File"));

	QAction* pAction = new QAction(this);
	m_aMenuActs[MENUACT_GEN_VT] = pAction;
	pAction->setText(QString::fromLocal8Bit("Generate Virtual Texture ..."));
	pMenuFile->addAction(pAction);
	connect(pAction, SIGNAL(triggered()), this, SLOT(OnGenerateVirtualTexture()));

	pAction = new QAction(this);
	m_aMenuActs[NENUACT_OPEN_VT] = pAction;
	pAction->setText(QString::fromLocal8Bit("Open Virtual Texture ..."));
	pMenuFile->addAction(pAction);
	connect(pAction, SIGNAL(triggered()), this, SLOT(OnOpenVirtualTexture()));

	//	------------------------------------------
	//	Edit menu
	//	------------------------------------------
/*	QMenu* pMenuEdit = menuBar()->addMenu(tr("&Edit"));

	pAction = new QAction(this);
	m_aMenuActs[MENUACT_MATERIAL_EDITOR] = pAction;
	pAction->setText(QString::fromLocal8Bit("Material Editor ..."));
	pAction->setCheckable(true);
	pMenuEdit->addAction(pAction);
	connect(pAction, SIGNAL(triggered()), this, SLOT(OnMaterialEditor()));

	pAction = new QAction(this);
	m_aMenuActs[MENUACT_MATERIAL_LIB] = pAction;
	pAction->setText(QString::fromLocal8Bit("Material Library ..."));
	pMenuEdit->addAction(pAction);
	connect(pAction, SIGNAL(triggered()), this, SLOT(OnMaterialLib()));

	//	------------------------------------------
	//	Render menu
	//	------------------------------------------
	QMenu* pMenuRender = menuBar()->addMenu(tr("&Render"));

	{
		//	Render mode submenu
		QMenu* pSubMenu = pMenuRender->addMenu(tr("Render Mode"));
		QActionGroup* pGroup = new QActionGroup(this);
		pGroup->setExclusive(true);

		pAction = new QAction(this);
		m_aMenuActs[MENUACT_RM_DEFAULT] = pAction;
		pAction->setText(QString::fromLocal8Bit("Default"));
		pAction->setCheckable(true);
		pAction->setChecked(true);
		pGroup->addAction(pAction);
		pSubMenu->addAction(pAction);

		pAction = new QAction(this);
		m_aMenuActs[MENUACT_RM_LOD] = pAction;
		pAction->setText(QString::fromLocal8Bit("View LOD"));
		pAction->setCheckable(true);
		pGroup->addAction(pAction);
		pSubMenu->addAction(pAction);

		pAction = new QAction(this);
		m_aMenuActs[MENUACT_RM_LIGHTONLY] = pAction;
		pAction->setText(QString::fromLocal8Bit("Light Only"));
		pAction->setCheckable(true);
		pGroup->addAction(pAction);
		pSubMenu->addAction(pAction);

		connect(pGroup, SIGNAL(triggered(QAction*)), this, SLOT(OnRenderMode(QAction*)));
	}

	pMenuRender->addSeparator();

	pAction = new QAction(this);
	m_aMenuActs[MENUACT_WIREFRAME] = pAction;
	pAction->setText(QString::fromLocal8Bit("Wire frame"));
	pAction->setCheckable(true);
	pMenuRender->addAction(pAction);
	connect(pAction, SIGNAL(triggered()), this, SLOT(OnWireFrame()));

	//	Update main menu states
	UpdateMainMenu();
*/
}

void VTMainWindow::SetupDockWidgets()
{
	int flags = 0;
	m_pMainDock = new CMainDockWidget(this, Qt::WindowFlags(flags));
	addDockWidget(Qt::RightDockWidgetArea, m_pMainDock);
}

bool VTMainWindow::Init()
{
	QWidget* pWidget = centralWidget();
	QSize size = pWidget->size();
	int width = size.width();
	int height = size.height();

	//	Create viewport
	A3DVIEWPORTPARAM viewParams;
	viewParams.X = 0;
	viewParams.Y = 0;
	viewParams.Width = width;
	viewParams.Height = height;
	viewParams.MinZ = 0.0f;
	viewParams.MaxZ = 1.0f;

	m_pViewport = new AXViewport;
	m_pViewport->Init(&viewParams);

	//	Create camera
	m_pCamera = new AXCamera;
	m_pCamera->Init(DEG2RAD(65.0f), 0.2f, MAX_VIEW_DIST, (float)width / height);

	m_pViewport->SetCamera(m_pCamera);

	//	Set camera's initial pose
	m_pCamera->SetPos(AXVector3(0.0f, 0.0f, -150.0f));
	m_pCamera->SetDirAndUp(AXVector3(0.0f, 0.0f, 1.0f), AXVector3(0.0f, 1.0f, 0.0f));

	//	create screen quad
	m_pTexQuad = new CTexScreenQuad();
	m_pTexQuad->SetFullScreen(false);

	//	Start timer after all initalization work done
	m_Timer.start(3, this);

	return true;
}

void VTMainWindow::Release()
{
	ReleaseVirtualTexture();

	A3DSAFEDELETE(m_pTexQuad);

	A3DRELEASE(m_pCamera);
	A3DRELEASE(m_pViewport);
	A3DRELEASE(m_pScene);
}

void VTMainWindow::UpdateMainDock(bool bAllPages)
{
	if (m_pMainDock)
		m_pMainDock->UpdateUI(bAllPages);
}

void VTMainWindow::timerEvent(QTimerEvent* pEvent)
{
	if (pEvent->timerId() != m_Timer.timerId())
		return;

	if (!DXRenderer::Instance()->IsInitOk())
		return;

	static auint32 nLastTime = 0;
	auint32 nCurTime = glb_GetMilliSecond();
	auint32 nTickTime = nCurTime - nLastTime;
	if (nTickTime <= 0 || nLastTime == 0)
	{
		nLastTime = glb_GetMilliSecond();
		return;
	}

	a_ClampRoof(nTickTime, 200U);
	afloat32 t = nTickTime * 0.001f;

	//	Tick
	Tick(t);
	//	Render
	Render();

	nLastTime = nCurTime;
}

void VTMainWindow::closeEvent(QCloseEvent* pEvent)
{
	m_Timer.stop();

	QMainWindow::closeEvent(pEvent);
}

void VTMainWindow::keyPressEvent(QKeyEvent* pEvent)
{
	QMainWindow::keyPressEvent(pEvent);

	if (!g_AppConfigs.bCamAutoRun)
	{
		auint32 uFlags = m_uCamMoveFlag;

		if (pEvent->key() == Qt::Key_W)
			uFlags |= CAM_MOVE_FORWARD;
		else if (pEvent->key() == Qt::Key_S)
			uFlags |= CAM_MOVE_BACK;
		else if (pEvent->key() == Qt::Key_A)
			uFlags |= CAM_MOVE_LEFT;
		else if (pEvent->key() == Qt::Key_D)
			uFlags |= CAM_MOVE_RIGHT;
		else if (pEvent->key() == Qt::Key_Q)
			uFlags |= CAM_MOVE_UP;
		else if (pEvent->key() == Qt::Key_Z)
			uFlags |= CAM_MOVE_DOWN;

		m_uCamMoveFlag = uFlags;
	}
}

void VTMainWindow::keyReleaseEvent(QKeyEvent* pEvent)
{
	QMainWindow::keyReleaseEvent(pEvent);

	if (!g_AppConfigs.bCamAutoRun)
	{
		auint32 uFlags = m_uCamMoveFlag;

		if (pEvent->key() == Qt::Key_W)
			uFlags &= ~CAM_MOVE_FORWARD;
		else if (pEvent->key() == Qt::Key_S)
			uFlags &= ~CAM_MOVE_BACK;
		else if (pEvent->key() == Qt::Key_A)
			uFlags &= ~CAM_MOVE_LEFT;
		else if (pEvent->key() == Qt::Key_D)
			uFlags &= ~CAM_MOVE_RIGHT;
		else if (pEvent->key() == Qt::Key_Q)
			uFlags &= ~CAM_MOVE_UP;
		else if (pEvent->key() == Qt::Key_Z)
			uFlags &= ~CAM_MOVE_DOWN;

		m_uCamMoveFlag = uFlags;
	}
}

void VTMainWindow::Tick(float time)
{
	if (!m_bAllInitDone)
		return;

	if (g_AppConfigs.bCamAutoRun)
	{
		AutoRunCamera(time);
	}
	else if (m_uCamMoveFlag)
	{
		//	Move camera
		MoveCamera(time);
	}

	if (m_bAllInitDone)
	{
		//	Render scene
		if (m_pScene)
		{
			m_pScene->Tick(time);
		}
	}

	if (m_pVPageViewer)
	{
		if (m_pVPageViewer->IsPageChanged())
		{
			//	refill virtual page in main thread
			m_pVPageViewer->FillVPage(m_pVTSys);
		}
	}
}

void VTMainWindow::Render()
{
	DXRenderer::Instance()->BeginRender();

	//	Push main viewport
	DXRenderer::Instance()->PushViewport(m_pViewport);

	if (m_bAllInitDone)
	{
		//	Render scene
		if (m_pScene)
		{
			m_pScene->Render(m_pCamera);
		}

		//	draw page viewer
		if (g_AppConfigs.bShowPageViewer)
		{
			int iPageSize = m_pVTSys->GetVPageSize();
			m_pTexQuad->SetQuad(10, 10, 10 + iPageSize, 10 + iPageSize);
			m_pTexQuad->Render(m_pVPageViewer->GetPageTexture());
		}

		//	draw VPage cache texture
		if (g_AppConfigs.bShowVPageCache)
		{
			int x = DXRenderer::Instance()->GetScreenWidth() - 512 - 10;
			int y = DXRenderer::Instance()->GetScreenHeight() - 512 - 10;
			m_pTexQuad->SetQuad(x, y, x + 512, y + 512);
			m_pTexQuad->Render(m_pVTSys->GetVTAtlas()->GetVPageCacheTexture());
		}
	}

	//	Pop main viewport
	DXRenderer::Instance()->PopViewport(m_pViewport);

	DXRenderer::Instance()->EndRender();
}

void VTMainWindow::MoveCamera(float time)
{
	AXVector3 vDirH = m_pCamera->GetDirH();
	AXVector3 vRightH = m_pCamera->GetRightH();
	AXVector3 vDelta(0.0f);

	if (m_uCamMoveFlag & CAM_MOVE_FORWARD)
		vDelta = vDirH;
	else if (m_uCamMoveFlag & CAM_MOVE_BACK)
		vDelta = -vDirH;

	if (m_uCamMoveFlag & CAM_MOVE_LEFT)
		vDelta = vDelta - vRightH;
	else if (m_uCamMoveFlag & CAM_MOVE_RIGHT)
		vDelta = vDelta + vRightH;

	if (m_uCamMoveFlag & CAM_MOVE_UP)
		vDelta = vDelta + AXVector3(0.0f, 1.0f, 0.0f);
	else if (m_uCamMoveFlag & CAM_MOVE_DOWN)
		vDelta = vDelta - AXVector3(0.0f, 1.0f, 0.0f);

	vDelta.Normalize();
	vDelta *= g_AppConfigs.fCamMoveSpeed * time;

	m_pCamera->Move(vDelta);
}

void VTMainWindow::StartCameraAutoRun(bool bStart)
{
	if (bStart)
	{
		//	reset camera's position
		m_pCamera->SetPos(AXVector3(0.0f, 0.0f, -150.0f));
		m_pCamera->SetDirAndUp(AXVector3(0.0f, 0.0f, 1.0f), AXVector3(0.0f, 1.0f, 0.0f));
	}
}

void VTMainWindow::AutoRunCamera(float time)
{
	if (!g_AppConfigs.bCamAutoRun)
		return;

	static float fTimeAccu = 0;
	float fRotate = g_AppConfigs.fCamRotateSpeed * 0.2f * fTimeAccu;
	float fSin1 = sinf(fTimeAccu / 3.0f * A3D_PI);
	float fSin2 = sinf(fTimeAccu / 4.0f * A3D_PI);
	fTimeAccu += time;

	AXVector3 vStartPos(0.0f, 0.0f, -150.0f);
	AXVector3 vPos = a3d_RotatePosAroundY(vStartPos, RAD2DEG(fRotate));
	vPos.y = fSin1 * 150.0f;

	AXVector3 vDir(-vPos);
	vDir.Normalize();
	vPos = vPos + vDir * (fSin2 * 60.0f);

	m_pCamera->SetPos(vPos);

	AXMatrix4 matView = a3d_LookAtMatrix(vPos, AXVector3(0.0f), AXVector3(0.0f, 1.0f, 0.0f), 0.0f);
	m_pCamera->SetViewTM(matView);
}

void VTMainWindow::OnGenerateVirtualTexture()
{
	CDlgGenVirtualTexture dlg(this);
	if (dlg.exec() != QDialog::Accepted)
		return;
}

void VTMainWindow::OnOpenVirtualTexture()
{
	QString str = QFileDialog::getOpenFileName(this, tr("Load Texture File"), "", "DDS Files (*.vt)");
	if (!str.length())
		return;

	QByteArray strBytes = str.toLocal8Bit();

	//	Release current virtual texture
	ReleaseVirtualTexture();

	int scn_width = DXRenderer::Instance()->GetScreenWidth();
	int scn_height = DXRenderer::Instance()->GetScreenHeight();

	//	open new vt file
	m_pVTSys = new CVTextureSystem();
	if (!m_pVTSys->Init(scn_width, scn_height, strBytes.data()))
	{
		glb_MessageBox("Error", "Failed to open file!");
		ReleaseVirtualTexture();
		return;
	}

	//	Create virtual page viewer
	m_pVPageViewer = new CVPageViewer;
	if (!m_pVPageViewer->Init(m_pVTSys->GetVTFormat(), m_pVTSys->GetVPageSize(), m_pVTSys->GetPixelBytes()))
	{
		glb_MessageBox("Error", "Failed to create virtual page viewer!");
		ReleaseVirtualTexture();
		return;
	}

	//	Create game scene
	m_pScene = new CGameScene;
	if (!m_pScene || !m_pScene->Init())
	{
		glb_OutputLog("VTMainWindow::Init, failed to game scene!");
		return;
	}

	m_pMainDock->UpdateUI(true);

	m_bAllInitDone = true;
}

void VTMainWindow::ReleaseVirtualTexture()
{
	A3DRELEASE(m_pVPageViewer);
	A3DRELEASE(m_pVTSys);

	m_pMainDock->UpdateUI(true);
}

