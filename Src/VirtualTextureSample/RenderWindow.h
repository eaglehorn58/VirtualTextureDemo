#ifndef _RENDERWINDOW_H_
#define _RENDERWINDOW_H_

#include "qwidget.h"
#include "APoint.h"

class VTMainWindow;

class CRenderWindow : public QWidget
{
	Q_OBJECT

public:

public:

	CRenderWindow(QWidget *parent = 0);
	~CRenderWindow();

public:

private:

	VTMainWindow*	m_pMainWnd;		//	Parent main window
	APointI			m_ptLastMouse;	//	Last position of mouse

protected slots:

protected:

	void resizeEvent(QResizeEvent* pEvent) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent* pEvent) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent* pEvent) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent* pEvent) Q_DECL_OVERRIDE;

protected:

	//	

	//	Tick
	//	time: time in seconds
//	void Tick(float time);
	//	Move camera
//	void MoveCamera(float time);
};

#endif // _RENDERWINDOW_H_
