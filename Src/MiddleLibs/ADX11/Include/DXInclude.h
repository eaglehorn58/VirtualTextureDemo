//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#ifndef _DXINCLUDE_H_
#define _DXINCLUDE_H_

#include "ATypes.h"
#include "vector.h"
#include <d3d11.h>

///////////////////////////////////////////////////////////////////////////

class CDXInclude : public ID3D10Include
{
public:		//	Types

	//	File node
	struct FILE_NODE
	{
		char		szFile[MAX_PATH];
		const void*	pData;
	};

public:		//	Constructors and Destructors

	CDXInclude(const char* szFile);
	~CDXInclude();

public:		//	Attributes

public:		//	Operations

	STDMETHOD(Open)(THIS_ D3D10_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes);
	STDMETHOD(Close)(THIS_ LPCVOID pData);

protected:	//	Attributes

	char	m_szBasePath[MAX_PATH];		//	Base path

	abase::vector<FILE_NODE*>	m_aFileNodes;		//	File nodes

protected:	//	Operations

	//	Parse file name
	FILE_NODE* ParseFileName(const char* pFileName, FILE_NODE* pParentNode);
	//	Find a file node
	int FindFileNode(const void* pData);
	//	Load file content
	void* LoadFileContent(const char* szFile, auint32& dwDataSize);
};
	
///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_DXINCLUDE_H_

