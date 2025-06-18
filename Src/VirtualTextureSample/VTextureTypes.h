//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com

#pragma once

#include "AXTypes.h"

///////////////////////////////////////////////////////////////////////////

namespace VTexture
{
	//	Hightmap file type
	enum
	{
		MAX_VPAGE_LOADED = 32,		//	Maximum number of VPage loaded each frame
		PAGE_BORDER_SIZE = 2,		//	broder pixels on each side
		PAGE_UAV_DOWNSCALE = 8,		//	page requesting UAV buffer downscale factor
	};

	///////////////////////////////////////////////////////////////////////////
	//	Class PageID
	///////////////////////////////////////////////////////////////////////////

	class PageID
	{
	public:

		enum
		{
			INVALID = 0xffffffff,
		};

		PageID(auint32 _id)
		{
			id = _id;
			x = (int)((id & 0xfff00000) >> 20);
			y = (int)((id & 0x000fff00) >> 8);
			mip = (int)((id & 0x000000f0) >> 4);
			debug = (int)(id & 0x0000000f);
		}

	public:

		static auint32 MakeID(int x, int y, int mip, int debug)
		{
			auint32 id = (auint32)(x << 20) | (auint32)(y << 8) | (auint32)(mip << 4) | (auint32)debug;
			return id;
		}

		//	get components
		static int GetX(auint32 _id) { return (int)((_id & 0xfff00000) >> 20); }
		static int GetY(auint32 _id) { return (int)((_id & 0x000fff00) >> 8); }
		static int GetMip(auint32 _id) { return (int)((_id & 0x000000f0) >> 4); }
		static int GetDebug(auint32 _id) { return (int)(_id & 0x0000000f); }

		//	clear debug component in a id
		static auint32 ClearDebugComponent(auint32 _id)
		{
			return _id & 0xfffffff0;
		}

		auint32	id;		//	page id
		int		x;		//	page's x position (in pages, not in texels) on mip level VT, 
		int		y;		//	page's y position (in pages, not in texels) on mip level VT
		int		mip;	//	requested mip
		int		debug;
	};

	///////////////////////////////////////////////////////////////////////////
	//	Class CacheID
	///////////////////////////////////////////////////////////////////////////

	class CacheID
	{
	public:

		enum
		{
			INVALID = 0xffffffff,
		};

		CacheID(auint32 _id)
		{
			id = _id;
			off_x = (int)((id & 0xfff00000) >> 20);
			off_y = (int)((id & 0x000fff00) >> 8);
			mip = (int)((id & 0x000000f0) >> 4);
			debug = (int)(id & 0x0000000f);
		}

	public:

		static auint32 MakeID(int off_x, int off_y, int mip, int debug)
		{
			auint32 id = (auint32)(off_x << 20) | (auint32)(off_y << 8) | (auint32)(mip << 4) | (auint32)debug;
			return id;
		}

		//	get components
		static int GetOffX(auint32 _id) { return (int)((_id & 0xfff00000) >> 20); }
		static int GetOffY(auint32 _id) { return (int)((_id & 0x000fff00) >> 8); }
		static int GetMip(auint32 _id) { return (int)((_id & 0x000000f0) >> 4); }
		static int GetDebug(auint32 _id) { return (int)(_id & 0x0000000f); }

		//	clear debug component in a id
		static auint32 ClearDebugComponent(auint32 _id)
		{
			return _id & 0xfffffff0;
		}

		auint32	id;			//	page cache id
		int		off_x;		//	x offset (in pages, not in texels) on page cache texture
		int		off_y;		//	y offset (in pages, not in texels) on page cache texture
		int		mip;		//	real mip of this page cache (may not same as the requested mip)
		int		debug;
	};
};


