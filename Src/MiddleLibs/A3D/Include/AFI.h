//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#ifndef _AFI_H_
#define _AFI_H_

#include "AString.h"
#include "ATypes.h"

//	************************** Attention Please! *******************************
//
//	To Use File Module properly, you must first call af_Initialize() to set the
//  correct directories, and at last you should make a call to af_Finalize()
//
//  ****************************************************************************

bool af_Initialize(const char* pszBaseDir);
bool af_Finalize();
void af_SetBaseDir(const char* pszBaseDir);

// BaseDir is the working directory we load resource files from, and it's read-only.
//
// Under iOS system:
//
// These four directories are independent, and have different access authorities.
// We write files into Document directory or Library directory which could be synchronized automatically by system.
// We write temporary files like preprocessed .glsl files to TempDir.
//
// Under Windows system:
//
// BaseDir, DocumentDir and LibraryDir are same, they all pointed to the current working directory of the program.
// And the TempDir is a sub-dir under current working directory named Temp.

const char* af_GetBaseDir();
//const char* af_GetDocumentDir();
//const char* af_GetLibraryDir();
//const char* af_GetTempDir();

void af_GetRelativePathNoBase(const char* szFullpath, const char* szParentPath, char* szRelativepath);
void af_GetRelativePathNoBase_2(const char* szFullpath, const char* szParentPath, char* szRelativepath, unsigned int bufSize);
void af_GetRelativePathNoBase(const char* szFullpath, const char* szParentPath, AString& strRelativePath);
void af_GetFullPathNoBase(char* szFullpath, const char* szBaseDir, const char* szFilename);
void af_GetFullPathNoBase_2(char* szFullpath, unsigned int bufSize, const char* szBaseDir, const char* szFilename);
void af_GetFullPathNoBase(AString& strFullpath, const char* szBaseDir, const char* szFilename);

void af_GetFullPath(char* szFullPath, const char* szFolderName, const char* szFileName);
void af_GetFullPath(char* szFullPath, const char* szFileName);
void af_GetFullPath(AString& strFullPath, const char* szFolderName, const char* szFileName);
void af_GetFullPath(AString& strFullPath, const char* szFileName);
void af_GetFullPathWithUpdate(AString& strFullPath, const char* szFileName,bool bNoCheckFileExist = false);
void af_GetFullPathWithDocument(AString& strFullPath, const char* szFileName,bool bNoCheckFileExist = false);


void af_GetRelativePath(const char* szFullPath, const char* szFolderName, char* szRelativePath);
void af_GetRelativePath(const char* szFullPath, char* szRelativePath);
void af_GetRelativePath(const char* szFullPath, AString& strRelativePath);
void af_GetRelativePath(const char* szFullPath, const char* szFolderName, AString& strRelativePath);
void af_GetTextureFileName(const achar* szTextureFileName, AString& strTextureFileName);

//	Get the file's title in the filename string;
//	Note: lpszFile and lpszTitle should be different buffer;
bool af_GetFileTitle(const char* lpszFile, char* lpszTitle, unsigned short cbBuf);
bool af_GetFileTitle(const char* lpszFile, AString& strTitle);

//	Get the file's path in the filename string;
//	Note: lpszFile and lpszPath should be different buffer;
bool af_GetFilePath(const char* lpszFile, char* lpszPath, unsigned short cbBuf);
bool af_GetFilePath(const char* lpszFile, AString& strPath);

//	Check file extension
bool af_CheckFileExt(const char* szFileName, const char* szExt, int iExtLen=-1, int iFileNameLen=-1);
//	Change file extension, szNewExt should contain it's own '.' before new extension, for example, '.bmp'
bool af_ChangeFileExt(char* szFileNameBuf, int iBufLen, const char* szNewExt);
bool af_ChangeFileExt(AString& strFileName, const char* szNewExt);

//	Check if file exist
bool afx_IsFileExist(const char * szFileName);

void af_RemoveExtName(AString& strFileName);

bool af_ContainFilePath(const char* szFileName);

//	Normalize file name, this function convert file name to lower case and change '/' to '\'
void af_NormalizeFileName(char* szFileName);
void af_NormalizeFileName(const char* szSrcFileName, char* szDstFileName);

#endif

