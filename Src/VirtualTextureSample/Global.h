//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#pragma once

#include "AXTypes.h"
#include "AXVector.h"

///////////////////////////////////////////////////////////////////////////

//	standard assert is disabled by Qt
#ifdef _DEBUG
	#define MyAssert(expression) { if (!(expression)) __debugbreak(); }
#else
	#define MyAssert(expression)
#endif

#define	MAX_VIEW_DIST	8000.0f

//	interial texture id
#define INTERNAL_TEX_WHITE	"internal_pure_white_tex"

///////////////////////////////////////////////////////////////////////////

class QWidget;

//	App configs
struct APP_CONFIGS
{
	float		fCamRotateSpeed;	//	Camera rotate speed
	float		fCamMoveSpeed;		//	Camera move speed
	bool		bCamAutoRun;		//	Camera auto run

	float		fSunHeight;			//	Sun height, degree [0, 90]
	float		fSunAngle;			//	Sun angle, degree [0.360]
	A3DCOLOR	colSun;				//	Sun color
	float		fSunPower;			//	Sun power
	A3DCOLOR	colAmbient;			//	Ambient color
	float		fAmbPower;			//	Ambient power

	bool		bShowPageViewer;	//	true, show page viewer
	bool		bShowVPageCache;	//	true, show VPage cache

	APP_CONFIGS();
};

extern APP_CONFIGS	g_AppConfigs;

///////////////////////////////////////////////////////////////////////////

//	Output log
void glb_OutputLog(const char* szMsg, ...);
//	Output message box
void glb_MessageBox(const char* szTitle, const char* szMsg, ...);
//	Check if a directory has existed
bool glb_IsDirExist(const char* szDir);
//	Create a directory
bool glb_CreateDirectory(const char* szDir);
//	Get Milli-second
auint32 glb_GetMilliSecond();
//	Get micro-second
auint64 glb_GetMicroSecond();
//	Convert gb2132 to utf-16
int glb_GB2312ToUTF16(auint16* szDest, const achar* szSrc, int nDestBufferMaxSize);
//	Convert utf-16 to gb2132
int glb_UTF16ToGB2312(char* szDest, const auint16* szSrc, int nDestBufferMaxSize);
//	Get sun direction
AXVector3 glb_GetSunDir();
//	Check if specified number is a 2^n number
bool glb_Is2Power(auint32 number);
//	Set backgroud color for a Qt widget
void glb_SetWidgetBkColor(QWidget* pWidget, A3DCOLOR color);
//	Convert from terrain slope [0, 90] to normal.y value
float glb_SlopeToNormalY(float slope);
//	Convert from normal.y to terrain slope [0, 90]
float glb_NormalYToSlope(float y);
