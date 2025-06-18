#include "VTMainWindow.h"
#include <QtWidgets/QApplication>
#include "Global.h"
#include "DXRenderer.h"
#include "AFI.h"
#include "qdir.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	VTMainWindow mainwnd;

	QByteArray strCurPath = QDir::currentPath().toLocal8Bit();
	af_Initialize(strCurPath.data());

	//	Initialize renderer
	if (!DXRenderer::InitInstance((HWND)(mainwnd.centralWidget()->winId())))
	{
		glb_OutputLog("main, failed to initialize renderer !");
		return -1;
	}

	if (!mainwnd.Init())
	{
		glb_OutputLog("main, failed to initialize main window !");
		return -1;
	}

	mainwnd.showMaximized();

	int ret = app.exec();

	mainwnd.Release();
	DXRenderer::ReleaseInstance();

	af_Finalize();

	return ret;
}
