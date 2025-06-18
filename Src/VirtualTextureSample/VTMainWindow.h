#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_VTMainWindow.h"
#include "qbasictimer.h"
#include "AXTypes.h"

class QMenu;
class QAction;
class AXViewport;
class AXCamera;
class CProject;
class CRenderWindow;
class CMainDockWidget;
class CGameScene;
class CVTextureSystem;
class CVPageViewer;
class CTexScreenQuad;

class VTMainWindow : public QMainWindow
{
	Q_OBJECT

public:

	//	Menu actions
	enum
	{
		MENUACT_GEN_VT = 0,
		NENUACT_OPEN_VT,
		NUM_MENUACT,
	};

	//	Camera move flags
	enum
	{
		CAM_MOVE_FORWARD = 0x01,
		CAM_MOVE_BACK = 0x02,
		CAM_MOVE_LEFT = 0x04,
		CAM_MOVE_RIGHT = 0x08,
		CAM_MOVE_UP = 0x10,
		CAM_MOVE_DOWN = 0x20,
	};

public:
	
	VTMainWindow(QWidget *parent = Q_NULLPTR);
	~VTMainWindow();

public:

	//	Do initialing work
	bool Init();
	//	Do release work
	void Release();

	//	Update main dock UI
	//	bAllPages: true, force to update all pages; otherwise false means only update current active page
	void UpdateMainDock(bool bAllPages = false);
	//	start/end camera auto run
	void StartCameraAutoRun(bool bStart);

	//	Get interface
	CRenderWindow* GetRenderWnd() { return m_pRenderWnd; }
	AXViewport* GetViewport() { return m_pViewport; }
	AXCamera* GetCamera() { return m_pCamera; }
	CGameScene* GetGameScene() { return m_pScene; }
	CVTextureSystem* GetVTSystem() { return m_pVTSys; }
	CVPageViewer* GetVPageViewer() { return m_pVPageViewer; }

private:

	Ui::VTMainWindowClass ui;

	//	Menu items
	QAction*			m_aMenuActs[NUM_MENUACT];

	QBasicTimer			m_Timer;
	CRenderWindow*		m_pRenderWnd;		//	central render window
	CMainDockWidget*	m_pMainDock;		//	main dock widget

	CGameScene*			m_pScene;
	AXViewport*			m_pViewport;
	AXCamera*			m_pCamera;
	CVTextureSystem*	m_pVTSys;			//	virtual texture system

	CTexScreenQuad*		m_pTexQuad;
	CVPageViewer*		m_pVPageViewer;		//	virtual page viewer

	auint32				m_uCamMoveFlag;
	bool				m_bAllInitDone;		//	true, all init work has been done

protected slots:

	void OnGenerateVirtualTexture();
	void OnOpenVirtualTexture();

protected:

	void timerEvent(QTimerEvent* pEvent) Q_DECL_OVERRIDE;
	void closeEvent(QCloseEvent* pEvent) Q_DECL_OVERRIDE;
	void keyPressEvent(QKeyEvent* pEvent) Q_DECL_OVERRIDE;
	void keyReleaseEvent(QKeyEvent* pEvent) Q_DECL_OVERRIDE;

protected:

	//	Setup main menu bar
	void SetupMenuBar();
	//	Setup dock widgets
	void SetupDockWidgets();
	//	Tick
	//	time: time in seconds
	void Tick(float time);
	//	Render
	void Render();
	//	Move camera
	void MoveCamera(float time);
	//	Auto run camera
	void AutoRunCamera(float time);
	//	Release current virtual texture
	void ReleaseVirtualTexture();
};

extern VTMainWindow*	g_pMainWnd;
