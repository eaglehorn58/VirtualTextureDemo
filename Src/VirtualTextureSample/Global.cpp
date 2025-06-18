//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#include <atlstr.h>
#include "Global.h"
#include "qmessagebox.h"
#include "A3DMacros.h"
#include "AXFuncs.h"
#include "qwidget.h"

///////////////////////////////////////////////////////////////////////////

APP_CONFIGS	g_AppConfigs;

APP_CONFIGS::APP_CONFIGS() :
fCamRotateSpeed(0.1f),
fCamMoveSpeed(30.0f),
bCamAutoRun(false),
fSunHeight(45.0f),
fSunAngle(0.0f),
colSun(A3DCOLORRGB(180, 180, 180)),
fSunPower(1.0f),
colAmbient(A3DCOLORRGB(80, 80, 80)),
fAmbPower(1.0f),
bShowPageViewer(false),
bShowVPageCache(false)
{
}

///////////////////////////////////////////////////////////////////////////

void glb_OutputLog(const char* szMsg, ...)
{
	char szBuf[2048];

	va_list vaList;
	va_start(vaList, szMsg);
	vsnprintf(szBuf, sizeof(szBuf), szMsg, vaList);
	va_end(vaList);

	strcat(szBuf, "\n");

	OutputDebugString(szBuf);
}

void glb_MessageBox(const char* szTitle, const char* szMsg, ...)
{
	char szBuf[1024];

	va_list vaList;
	va_start(vaList, szMsg);
	vsnprintf(szBuf, sizeof(szBuf), szMsg, vaList);
	va_end(vaList);

	QMessageBox::information(nullptr, szTitle, szBuf);
}

bool glb_IsDirExist(const char* szDir)
{
	DWORD dwAttrib = GetFileAttributes(szDir);
	return (INVALID_FILE_ATTRIBUTES != dwAttrib) && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

bool glb_CreateDirectory(const char* szDir)
{
	CString strDir = szDir;
	strDir.Replace('\\', '/');

	int iLen = strDir.GetLength();
	if (iLen <= 3)
		return true;

	//	Clear the last '/'
	if (strDir.GetAt(iLen - 1) == '/')
		strDir.SetAt(iLen - 1, '\0');

	//	Save current directory
	char szCurDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szCurDir);

	CString strTemp;

	int iPos = strDir.Find('/', 0);
	while (iPos > 0)
	{
		strTemp = strDir.Mid(0, iPos);
		CreateDirectory(strTemp, NULL);
		iPos = strDir.Find('/', iPos + 1);
	}

	CreateDirectory(szDir, NULL);

	//	Restore current directory
	SetCurrentDirectory(szCurDir);

	return true;
}

auint32 glb_GetMilliSecond()
{
	return (auint32)(glb_GetMicroSecond() / 1000);
}

auint64 glb_GetMicroSecond()
{
	static LARGE_INTEGER liFrequency;
	static abool bFirstTime = atrue;

	if (bFirstTime)
	{
		bFirstTime = false;
		::QueryPerformanceFrequency(&liFrequency);
	}

	LARGE_INTEGER liCounter;
	::QueryPerformanceCounter(&liCounter);

	auint64 uSecond = liCounter.QuadPart / liFrequency.QuadPart;
	auint64 uRemainder = liCounter.QuadPart % liFrequency.QuadPart;

	return uSecond * 1000000 + uRemainder * 1000000 / liFrequency.QuadPart;
}

int glb_GB2312ToUTF16(auint16* szDest, const achar* szSrc, int nDestBufferMaxSize)
{
	return MultiByteToWideChar(CP_ACP, 0, szSrc, -1, (wchar_t*)szDest, nDestBufferMaxSize);
}

int glb_UTF16ToGB2312(char* szDest, const auint16* szSrc, int nDestBufferMaxSize)
{
	return WideCharToMultiByte(936, 0, (const wchar_t*)szSrc, -1, szDest, nDestBufferMaxSize, NULL, NULL);
}

AXVector3 glb_GetSunDir()
{
	float fLongitude = DEG2RAD(g_AppConfigs.fSunAngle);
	float fLatitude = DEG2RAD(g_AppConfigs.fSunHeight);

	float len = cosf(fLatitude);

	AXVector3 pos;
	pos.x = len * sinf(fLongitude);
	pos.y = sinf(fLatitude);
	pos.z = len * cosf(fLongitude);

	return a3d_Normalize(-pos);
}

bool glb_Is2Power(auint32 number)
{
	return ((number & (number - 1)) == 0);
}

void glb_SetWidgetBkColor(QWidget* pWidget, A3DCOLOR color)
{
	int r = A3DCOLOR_GETRED(color);
	int g = A3DCOLOR_GETGREEN(color);
	int b = A3DCOLOR_GETBLUE(color);

	char sz[200];
	sprintf(sz, "background-color: rgb(%d, %d, %d)", r, g, b);
	pWidget->setStyleSheet(sz);
}

float glb_SlopeToNormalY(float slope)
{
	return sinf(DEG2RAD(90.0f - slope));
}

float glb_NormalYToSlope(float y)
{
	return 90.0f - RAD2DEG(asinf(y));
}

