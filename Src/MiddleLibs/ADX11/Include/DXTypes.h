//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com

#pragma once

#include <d3d11.h>

///////////////////////////////////////////////////////////////////////////

namespace DX
{
	//	CPU access flags
	enum
	{
		CPU_NO_ACCESS = 0,
		CPU_READ = 0x01,		//	staging readable texture
		CPU_WRITE = 0x02,		//	staging writable texture
		CPU_DYNAMIC = 0x04,		//	dynamic texture
	};

	//	buffer type
	enum
	{
		BUFFER_UNKNOWN = 0,
		BUFFER_STRUCTURED,		//	structured buffer
		BUFFER_RAW,				//	raw buffer
	};
}

