//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#include "ALog.h"

///////////////////////////////////////////////////////////////////////////

void a_OutputLog(const char* szMsg, ...)
{
	char szBuf[2048];

	va_list vaList;
	va_start(vaList, szMsg);
	vsnprintf(szBuf, sizeof(szBuf), szMsg, vaList);
	va_end(vaList);

	strcat(szBuf, "\n");

	OutputDebugString(szBuf);
}



