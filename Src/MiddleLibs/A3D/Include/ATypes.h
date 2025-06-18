//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com

#ifndef _A_TYPES_H_
#define _A_TYPES_H_

//	#include "APlatform.h"

#define A_PLATFORM_WIN_DESKTOP 1
#define A_PLATFORM_64 1

#if A_PLATFORM_WIN_DESKTOP
	#include "AWinTypes.h"
#elif A_PLATFORM_ANDROID
    #include "AAndroidTypes.h"
#elif A_PLATFORM_LINUX
	#include "ALinuxTypes.h"
#elif A_PLATFORM_XOS
    #include "AXOSTypes.h"
#elif A_PLATFORM_WINRT
    #include "AWinRTTypes.h"
#endif

#define A_MIN_BYTE		0x00
#define A_MAX_BYTE		0xff
#define A_MIN_CHAR		0x80
#define A_MAX_CHAR		0x7f
#define A_MIN_UINT16	0x0000U
#define A_MAX_UINT16	0xffffU
#define A_MIN_UINT32	0x00000000U
#define A_MAX_UINT32	0xffffffffU
#define A_MIN_UINT64	0x0000000000000000ULL
#define A_MAX_UINT64	0xffffffffffffffffULL
#define A_MIN_INT16		0x8000
#define A_MAX_INT16		0x7fff
#define A_MIN_INT32		0x80000000
#define A_MAX_INT32		0x7fffffff
#define A_MIN_INT64		0x8000000000000000LL
#define A_MAX_INT64		0x7fffffffffffffffLL
#define A_MAX_FLT32		3.402823466e+38F
#define A_MIN_FLT32		1.175494351e-38F
#define A_EPSILON_FLT32 1.192092896e-07F
#define A_MAX_FLT64		1.7976931348623158e+308
#define A_MIN_FLT64		2.2250738585072014e-308
#define A_EPSILON_FLT64 2.2204460492503131e-016
#define A_FLOAT_NEAR_ZERO	0.001F

#define A_INDEX_NONE	 -1

/*
#ifdef      MAX_PATH
#undef      MAX_PATH
#endif
#define     MAX_PATH        512		//260

#define ABit( num )		( 1 << ( num ) )
#define AOffsetOf(s,m)   (auint32)&reinterpret_cast<const volatile abyte&>((((s *)0)->m))
#define AL(str)			L##str
#define A_SAFEDELETE(x) if ((x) != NULL) { delete x; x = NULL; }
#define A_SAFEDELETE_ARRAY(x) if ((x) != NULL) { delete[] x; x = NULL; }
#define A_SAFEFREE(x) if ((x) != NULL) { a_free(x); x = NULL; }

#ifdef A_PLATFORM_64
#define A_VOID_INT32( x ) ((aint32)((auint64)(x) & 0xffffffff))
#define A_VOID_UINT32( x ) ((auint32)((auint64)(x) & 0xffffffff))
#else
#define A_VOID_INT32( x ) ((aint32)x)
#define A_VOID_UINT32( x ) ((auint32)x)
#endif

#define A_SAFERELEASE(p) \
{ \
	if (p) \
	{ \
		(p)->Release(); \
		delete (p); \
		(p) = NULL; \
	} \
}

#define A_SAFERELEASE_INTERFACE(p) \
{ \
	if (p) \
	{ \
	(p)->Release(); \
	(p) = NULL; \
	} \
}

//	Text encoding.
enum ATextEncoding
{
	ATextEncodingUnknown = 0,	//	Unkown 
	ATextEncodingSystem,		//	The current Windows or Linux Code-Page. This is only for system-API calls, user should NOT save files or net-transfer the result buffer.
	ATextEncodingUTF8,
	ATextEncodingGB2312,
	ATextEncodingGB18030,
	ATextEncodingBig5,
	ATextEncodingNUM,			//	Number of supported encoding
};

// MessageBox enum
enum AMessageStyle
{
	AMessageStyle_OK = 0,		 
	AMessageStyle_OK_CANCLE,		
	AMessageStyle_YES_NO,	
};

enum AMessageReturn
{
	AMessageReturn_OK = 0,		 
	AMessageReturn_CANCLE,		
	AMessageReturn_NO,
	AMessageReturn_YES,
	AMessageReturn_OHTER
};
*/

#ifndef ASSERT
#define ASSERT	assert
#endif	//	ASSERT

#if defined(_DEBUG) || defined(DEBUG)
#else
#undef ASSERT
#define ASSERT(x) 
#endif

/*
//	Disable copy constructor and operator = 
#define DISABLE_COPY_AND_ASSIGNMENT(ClassName)	\
	private:\
	ClassName(const ClassName&);\
	ClassName& operator = (const ClassName&);

//	Time structure
struct ATIME
{
	aint32	year;		//	year since 1900
	aint32	month;		//	[0, 11]
	aint32	day;		//	[1, 31]
	aint32	hour;		//	[0, 23]
	aint32	minute;		//	[0, 59]
	aint32	second;		//	[0, 59]
	aint32	wday;		//	day of week, [0, 6]
};
*/

#endif //	_A_TYPES_H_