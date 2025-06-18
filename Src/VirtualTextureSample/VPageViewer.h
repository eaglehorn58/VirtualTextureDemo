//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#ifndef _VPAGEVIEWER_H_
#define _VPAGEVIEWER_H_

#include "AXTypes.h"
#include <dxgiformat.h>

///////////////////////////////////////////////////////////////////////////

class CDXTexture;
class CVTextureSystem;

///////////////////////////////////////////////////////////////////////////

class CVPageViewer
{
public:		//	Types

	struct POSITION
	{
		union
		{
			float	u;
			int		x;
		};

		union
		{
			float	v;
			int		y;
		};
	};

public:		//	Constructors and Destructors

	CVPageViewer();
	virtual ~CVPageViewer();

public:		//	Attributes

public:		//	Operations

	//	initialize
	//	iVPageWidth: width(height) of virtual page
	bool Init(DXGI_FORMAT fmt, int iVPageWidth, int iPixelBytes);
	//	release
	void Release();

	//	Set page
	void SetPageByUV(float u, float v, int mipmap);
	void SetPage(int x, int y, int mipmap);
	//	Fill VPage content
	bool FillVPage(CVTextureSystem* pVTSys);

	//	Is page changed ?
	bool IsPageChanged() const { return m_bPageChanged; }
	//	get page
	const POSITION& GetPos() const { return m_pos; }
	int GetMipmap() const { return m_mipmap; }

	//	get page texture
	CDXTexture* GetPageTexture() { return m_pVPageTex; }

protected:	//	Attributes

	CDXTexture*		m_pVPageTex;
	abyte*			m_pPageBuf;

	int			m_iVPageWidth;
	int			m_iPixelBytes;
	POSITION	m_pos;
	int			m_mipmap;
	bool		m_bPageChanged;

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_VPAGEVIEWER_H_

