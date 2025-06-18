//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#include "AFI.h"
//#include "ALog.h"
//#include "AXFilePackage.h"
//#include "AXFilePackMan.h"

///////////////////////////////////////////////////////////////////////////

char	g_szBaseDir[MAX_PATH] = "";

///////////////////////////////////////////////////////////////////////////

inline void af_RemoveLastDirSlash(char* pszDir)
{
    // Get rid of last '\\'
    const int nLength = (const int)strlen(pszDir);
	if( pszDir[0] && (pszDir[nLength - 1] == '\\' || pszDir[nLength - 1] == '/') )
		pszDir[nLength - 1] = '\0';
}

bool af_Initialize(const char* pszBaseDir)
{
	strncpy(g_szBaseDir, pszBaseDir, MAX_PATH);
    af_RemoveLastDirSlash(g_szBaseDir);
	return true;
}

void af_SetBaseDir(const char* pszBaseDir)
{
	strncpy(g_szBaseDir, pszBaseDir, MAX_PATH);
	af_RemoveLastDirSlash(g_szBaseDir);
}

const char* af_GetBaseDir()
{
	return g_szBaseDir;
}

bool af_Finalize()
{
	g_szBaseDir[0] = 0;
	return true;
}

//	NOTE!! by zhoushiheng, buffer overflow risk
void af_GetRelativePathNoBase(const char* szFullpath, const char* szParentPath, char* szRelativepath)
{
	const char* p1 = szParentPath;
	const char* p2 = szFullpath;
	
	while( *p1 && *p2 && // Not null
		(
		(*p1 == *p2) || // Character is identical
		(*p1 >= 'A' && *p1 <= 'Z' && *p1 + 0x20 == *p2) || (*p2 >= 'A' && *p2 <= 'Z' && *p1 == *p2 + 0x20) || // Compare English character without regard to case.
		(*p1 == '\\' && (*p2 == '/' || *p2 == '\\')) || (*p1 == '/' && (*p2 == '/' || *p2 == '\\'))		// Both are / or \;
		)
		)
	{
		++p1;
		++p2;
	}

	if( *p1 ) // not found;
	{
		strcpy(szRelativepath, szFullpath);
		return;
	}

	if( (*p2 == '\\') || (*p2 == '/') )
		p2 ++;

	strcpy(szRelativepath, p2);
}

void af_GetRelativePathNoBase_2(const char* szFullpath, const char* szParentPath, char* szRelativepath, unsigned int bufSize)
{
	const char* p1 = szParentPath;
	const char* p2 = szFullpath;

	while (*p1 && *p2 && // Not null
		(
		(*p1 == *p2) || // Character is identical
		(*p1 >= 'A' && *p1 <= 'Z' && *p1 + 0x20 == *p2) || (*p2 >= 'A' && *p2 <= 'Z' && *p1 == *p2 + 0x20) || // Compare English character without regard to case.
		(*p1 == '\\' && (*p2 == '/' || *p2 == '\\')) || (*p1 == '/' && (*p2 == '/' || *p2 == '\\'))		// Both are / or \;
		)
		)
	{
		++p1;
		++p2;
	}

	if (*p1) // not found;
	{
		//af_StrCopySafe(szRelativepath, bufSize, szFullpath);
		strncpy(szRelativepath, szFullpath, bufSize);
		return;
	}

	if ((*p2 == '\\') || (*p2 == '/'))
		p2++;

	//af_StrCopySafe(szRelativepath, bufSize, p2);
	strncpy(szRelativepath, p2, bufSize);
}

void af_GetRelativePathNoBase(const char* szFullpath, const char* szParentPath, AString& strRelativePath)
{
	char szRelativePath[MAX_PATH];
	af_GetRelativePathNoBase_2(szFullpath, szParentPath, szRelativePath, sizeof(szRelativePath));
	strRelativePath = szRelativePath;
}

//	NOTE!! by zhoushiheng, buffer overflow risk
void af_GetFullPathNoBase(char* szFullpath, const char* szBaseDir, const char* szFilename)
{
	szFullpath[0] = '\0';

	int nStrLenName = (int)strlen(szFilename);
	if( nStrLenName == 0 )
		return;

	//See if it is a absolute path;
	if( nStrLenName > 3 )
	{
		if ((szFilename[1] == ':' && (szFilename[2] == '\\' || szFilename[2] == '/'))
			|| (szFilename[0] == '\\' && szFilename[1] == '\\')
			|| (szFilename[0] == '/' && szFilename[1] == '/'))
		{
			strcpy(szFullpath, szFilename);
			return;
		}
	}
	else
	{
		if( nStrLenName > 1 && ( szFilename[0] == '\\' || szFilename[0] == '/' ))
			return;
	}

	const char* pszRealfile = szFilename;
	// Get rid of prefix .\, so to make a clean relative file path
	if( nStrLenName > 2 && szFilename[0] == '.' && ( szFilename[1] == '\\' || szFilename[1] == '/' ))
		pszRealfile = szFilename + 2;
	
	if( szBaseDir[0] == '\0' )
		strcpy(szFullpath, pszRealfile);
	else if( (szBaseDir[strlen(szBaseDir) - 1] == '\\') || (szBaseDir[strlen(szBaseDir) - 1] == '/'))
		sprintf(szFullpath, "%s%s", szBaseDir, pszRealfile);
	else
		sprintf(szFullpath, "%s/%s", szBaseDir, pszRealfile);
	return;
}

void af_GetFullPathNoBase_2(char* szFullpath, unsigned int bufSize, const char* szBaseDir, const char* szFilename)
{
	szFullpath[0] = '\0';

	int nStrLenName = (int)strlen(szFilename);
	if (nStrLenName == 0)
		return;

	//See if it is a absolute path;
	if (nStrLenName > 3)
	{
		if ((szFilename[1] == ':' && (szFilename[2] == '\\' || szFilename[2] == '/'))
			|| (szFilename[0] == '\\' && szFilename[1] == '\\')
			|| (szFilename[0] == '/' && szFilename[1] == '/'))
		{
			//af_StrCopySafe(szFullpath, bufSize, szFilename);
			strncpy(szFullpath, szFilename, bufSize);
			return;
		}
	}
	else
	{
		if (nStrLenName > 1 && (szFilename[0] == '\\' || szFilename[0] == '/'))
			return;
	}

	const char* pszRealfile = szFilename;
	// Get rid of prefix .\, so to make a clean relative file path
	if (nStrLenName > 2 && szFilename[0] == '.' && (szFilename[1] == '\\' || szFilename[1] == '/'))
		pszRealfile = szFilename + 2;

	if (szBaseDir[0] == '\0')
	{
		//af_StrCopySafe(szFullpath, bufSize, pszRealfile);
		strncpy(szFullpath, pszRealfile, bufSize);
	}
	else if ((szBaseDir[strlen(szBaseDir) - 1] == '\\') || (szBaseDir[strlen(szBaseDir) - 1] == '/'))
	{
		//af_StrPrintfSafe(szFullpath, bufSize, "%s%s", szBaseDir, pszRealfile);
		_snprintf(szFullpath, bufSize, "%s%s", szBaseDir, pszRealfile);
	}
	else
	{
		//af_StrPrintfSafe(szFullpath, bufSize, "%s/%s", szBaseDir, pszRealfile);
		_snprintf(szFullpath, bufSize, "%s/%s", szBaseDir, pszRealfile);
	}
	return;
}

void af_GetFullPathNoBase(AString& strFullpath, const char* szBaseDir, const char* szFilename)
{
	char szFullPath[MAX_PATH];
	af_GetFullPathNoBase_2(szFullPath, sizeof(szFullPath), szBaseDir, szFilename);
	strFullpath = szFullPath;
}

//	NOTE!! by zhoushiheng, buffer overflow risk
void af_GetFullPath(char* szFullPath, const char* szFolderName, const char* szFileName)
{
	char szBaseDir[MAX_PATH];
	//af_StrPrintfSafe(szBaseDir, sizeof(szBaseDir), "%s/%s", g_szBaseDir, szFolderName);
	_snprintf(szBaseDir, sizeof(szBaseDir), "%s/%s", g_szBaseDir, szFolderName);
	af_GetFullPathNoBase(szFullPath, szBaseDir, szFileName);
}

//	NOTE!! by zhoushiheng, buffer overflow risk
void af_GetFullPath(char* szFullPath, const char* szFileName)
{
	af_GetFullPathNoBase(szFullPath, g_szBaseDir, szFileName);
}

void af_GetFullPath(AString& strFullPath, const char* szFolderName, const char* szFileName)
{
	char szBaseDir[MAX_PATH];
	//af_StrPrintfSafe(szBaseDir, sizeof(szBaseDir), "%s/%s", g_szBaseDir, szFolderName);
	_snprintf(szBaseDir, sizeof(szBaseDir), "%s/%s", g_szBaseDir, szFolderName);
	af_GetFullPathNoBase(strFullPath, szBaseDir, szFileName);
}

void af_GetFullPath(AString& strFullPath, const char* szFileName)
{
	af_GetFullPathNoBase(strFullPath, g_szBaseDir, szFileName);
}

void af_GetFullPathWithUpdate(AString& strFullPath, const char* szFileName, bool bNoCheckFileExist)
{
	AString strfilename = szFileName;
	strfilename.MakeLower();
	af_GetFullPathNoBase(strFullPath, g_szBaseDir, (const char*)strfilename);
}

void af_GetFullPathWithDocument(AString& strFullPath, const char* szFileName, bool bNoCheckFileExist)
{
	AString strfilename = szFileName;
	strfilename.MakeLower();
	af_GetFullPathNoBase(strFullPath, g_szBaseDir, (const char*)strfilename);
}

//	NOTE!! by zhoushiheng, buffer overflow risk
void af_GetRelativePath(const char* szFullPath, const char* szFolderName, char* szRelativePath)
{
	char szBaseDir[MAX_PATH];
	//af_StrPrintfSafe(szBaseDir, sizeof(szBaseDir), "%s/%s", g_szBaseDir, szFolderName);
	_snprintf(szBaseDir, sizeof(szBaseDir), "%s/%s", g_szBaseDir, szFolderName);
	af_GetRelativePathNoBase(szFullPath, szBaseDir, szRelativePath);
}

//	NOTE!! by zhoushiheng, buffer overflow risk
void af_GetRelativePath(const char* szFullPath, char* szRelativePath)
{
	af_GetRelativePathNoBase(szFullPath, g_szBaseDir, szRelativePath);
}

void af_GetRelativePath(const char* szFullPath, AString& strRelativePath)
{
	af_GetRelativePathNoBase(szFullPath, g_szBaseDir, strRelativePath);
}

void af_GetRelativePath(const char* szFullPath, const char* szFolderName, AString& strRelativePath)
{
	char szBaseDir[MAX_PATH];
	//af_StrPrintfSafe(szBaseDir, sizeof(szBaseDir), "%s/%s", g_szBaseDir, szFolderName);
	_snprintf(szBaseDir, sizeof(szBaseDir), "%s/%s", g_szBaseDir, szFolderName);
	af_GetRelativePathNoBase(szFullPath, szBaseDir, strRelativePath);
}

void af_GetTextureFileName(const achar* szTextureFileName, AString& strTextureFileName)
{
	if (!szTextureFileName || !szTextureFileName[0])
	{
		strTextureFileName = "";
		return;
	}

	strTextureFileName = szTextureFileName;
	aint32 nPos = strTextureFileName.Find('.');
	if (nPos >= 0)
	{
		strTextureFileName = strTextureFileName.Left(nPos);
	}
	nPos = strTextureFileName.Find('|');
	if (nPos >= 0)
	{
		strTextureFileName = strTextureFileName.Left(nPos);
	}
}


bool af_GetFileTitle(const char* lpszFile, char* lpszTitle, unsigned short cbBuf)
{
	if (!lpszFile || !lpszTitle)
		return false;

	lpszTitle[0] = '\0';
	if (lpszFile[0] == '\0')
		return true;

	const char* pszTemp = lpszFile + strlen(lpszFile);

	--pszTemp;
    if ('\\' == (* pszTemp) || '/' == (* pszTemp)) return false;
    while (true) {
        if ('\\' == (* pszTemp) || '/' == (* pszTemp))
		{
            ++pszTemp;
            break;
        }
        if (pszTemp == lpszFile) break;
        --pszTemp;
    }
    //af_StrCopySafe(lpszTitle, cbBuf, pszTemp);
	strncpy(lpszTitle, pszTemp, cbBuf);
    return true;
}

bool af_GetFileTitle(const char* lpszFile, AString& strTitle)
{
	char szTitle[MAX_PATH];
	bool bRet = af_GetFileTitle(lpszFile, szTitle, sizeof(szTitle));
	if (bRet)
		strTitle = szTitle;

	return bRet;
}

bool af_GetFilePath(const char* lpszFile, char* lpszPath, unsigned short cbBuf)
{
	if (!lpszFile || !lpszPath)
		return false;

	lpszPath[0]= '\0';
	if (lpszFile[0] == '\0')
		return true;

	//af_StrCopySafe(lpszPath, cbBuf, lpszFile);
	strncpy(lpszPath, lpszFile, cbBuf);
    char* pszTemp = (char *)lpszPath + strlen(lpszPath);

    --pszTemp;
    while (true) {
        if ('\\' == (* pszTemp) || '/' == (* pszTemp)) 
		{
            break;
        }
        if (pszTemp == lpszPath) break;
        --pszTemp;
    }
    *pszTemp = '\0';
    return true;
}

bool af_GetFilePath(const char* lpszFile, AString& strPath)
{
	char szPath[MAX_PATH];
	bool bRet = af_GetFilePath(lpszFile, szPath, sizeof(szPath));
	if (bRet)
		strPath = szPath;

	return bRet;
}

//	Check file extension
bool af_CheckFileExt(const char* szFileName, const char* szExt, int iExtLen/* -1 */, int iFileNameLen/* -1 */)
{
	ASSERT(szFileName && szExt);

	if (iFileNameLen < 0)
		iFileNameLen = (int)strlen(szFileName);

	if (iExtLen < 0)
		iExtLen = (int)strlen(szExt);

	const char* p1 = szFileName + iFileNameLen - 1;
	const char* p2 = szExt + iExtLen - 1;

	bool bMatch = true;

	while (p2 >= szExt && p1 >= szFileName)
	{
		if (*p1 != *p2 && !(*p1 >= 'A' && *p1 <= 'Z' && *p2 == *p1 + 32) &&
			!(*p1 >= 'a' && *p1 <= 'z' && *p2 == *p1 - 32))
		{
			bMatch = false;
			break;
		}

		p1--;
		p2--;
	}

	return bMatch;
}

//	Change file extension
bool af_ChangeFileExt(char* szFileNameBuf, int iBufLen, const char* szNewExt)
{
	char szFile[MAX_PATH];
	//af_StrCopySafe(szFile, sizeof(szFile), szFileNameBuf);
	strncpy(szFile, szFileNameBuf, sizeof(szFile));

	char* pTemp = strrchr(szFile, '.');
	if (pTemp)
	{
		//af_StrCopySafe(pTemp, szFile + sizeof(szFile) - pTemp, szNewExt);
		strncpy(pTemp, szNewExt, szFile + sizeof(szFile) - pTemp);
	}
	else
	{
		strncat(szFile, szNewExt, sizeof(szFile) - strlen(szFile));
	}

	int iLen = (int)strlen(szFile);
	if (iLen >= iBufLen)
	{
		ASSERT(iLen < iBufLen);
		return false;
	}

	//af_StrCopySafe(szFileNameBuf, iBufLen, szFile);
	strncpy(szFileNameBuf, szFile, iBufLen);
	return true;
}

bool af_ChangeFileExt(AString& strFileName, const char* szNewExt)
{
	char szFile[MAX_PATH];
	//af_StrCopySafe(szFile, sizeof(szFile), strFileName);
	strncpy(szFile, strFileName, sizeof(szFile));

	char* pTemp = strrchr(szFile, '.');
	if (pTemp)
	{
		//af_StrCopySafe(pTemp, szFile + sizeof(szFile) - pTemp, szNewExt);
		strncpy(pTemp, szNewExt, szFile + sizeof(szFile) - pTemp);
	}
	else
	{
		strncat(szFile, szNewExt, sizeof(szFile) - strlen(szFile));
	}

	strFileName = szFile;
	return true;
}

// Check if a specified file exist,	gb2312 encode
bool afx_IsFileExist(const char * szFileName)
{
	AString strRelativePath;
	af_GetRelativePath(szFileName, strRelativePath);

/*	AXFilePackBase *pPackage = g_AXUpdateFilePackMan.GetFilePck(strRelativePath);
	if (pPackage && pPackage->IsFileExist(strRelativePath))
		return true;

	// we must supply a relative path to GetFilePck function
	pPackage = g_AXFilePackMan.GetFilePck(strRelativePath);
	if (pPackage && pPackage->IsFileExist(strRelativePath))
		return true;
*/

	// not found in package, so test if exist on the disk, here we must use full path
	char	szFullPath[MAX_PATH];

	AString filename = strRelativePath;
	filename.Replace('\\', '/');
	filename.MakeLower();
	af_GetFullPathNoBase_2(szFullPath, sizeof(szFullPath), af_GetBaseDir(), filename);

	if (INVALID_FILE_ATTRIBUTES != GetFileAttributes(szFullPath))
		return true;

	return false;
}

void af_RemoveExtName(AString& strFileName)
{
	int iPos = strFileName.ReverseFind('.');
	if (iPos >= 0)
	{
		strFileName = strFileName.Left(iPos);
	}
}

bool af_ContainFilePath( const char* szFileName )
{
    return strchr(szFileName, '\\') || strchr(szFileName, '/');
}

void af_NormalizeFileName(char* szFileName)
{
	if (szFileName)
	{
		char* p = szFileName;
		while (*p)
		{
			if (*p >= 'A' && *p <= 'Z')
				*p += 32;
			else if (*p == '\\')
				*p = '/';
			p++;
		}
	}
}

void af_NormalizeFileName(const char* szSrcFileName, char* szDstFileName)
{
	if (!szSrcFileName || !szDstFileName)
	{
		ASSERT(szSrcFileName && szDstFileName);
		return;
	}

	const char* ps = szSrcFileName;
	char* pd = szDstFileName;

	while (*ps)
	{
		char ch = *ps++;
		if (ch >= 'A' && ch <= 'Z')
			*pd = ch + 32;
		else if (ch == '\\')
			*pd = '/';
		else
			*pd = ch;

		pd++;
	}

	*pd = '\0';
}