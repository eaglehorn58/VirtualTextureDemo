//-------------------------------------------------------------------------------------------------
//FileName:AWinTypes.h
//Created by liyi 2012,11,19
//-------------------------------------------------------------------------------------------------
#ifndef _A_WIN_TYPES_H_
#define _A_WIN_TYPES_H_

#ifdef _DEBUG
#define A_DEBUG
#endif

#include <windows.h>
#include <DbgHelp.h>
#include <Shlwapi.h>

#include <tchar.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <assert.h>
#include <malloc.h> // for _alloca
#include <stdio.h>
#include <time.h>
#include <stdarg.h>	
#include <io.h>
#include <string.h>
#include <new>
#include <sys/stat.h>
#include <wchar.h>

#pragma warning(disable : 4251)
#pragma warning(disable : 4355) // this used in base initializer list                                  
#pragma warning(disable : 4996)

//-------------------------------------------------------
//	base types
//-------------------------------------------------------

typedef unsigned __int8		auint8;		
typedef unsigned __int16	auint16;		
typedef unsigned __int32	auint32;
typedef unsigned __int64	auint64;
		
typedef	signed __int8		aint8;		
typedef signed __int16		aint16;		
typedef signed __int32 		aint32;		
typedef signed __int64		aint64;		

typedef float				afloat32;		
typedef double				afloat64;	

typedef char				achar;
typedef unsigned __int8		abyte;
typedef bool				abool;

#define atrue				true
#define afalse				false

#ifdef A_PLATFORM_64
typedef aint64				aptrint;
typedef auint64				auptrint;
typedef aint64				along;
typedef auint64				aulong;
#else
typedef aint32				aptrint;		
typedef auint32				auptrint;	
typedef aint32				along;
typedef auint32				aulong;
#endif

//	File handle
typedef HANDLE				AFILEHANDLE;
#define AMEM			: public AMemBase

#define A_FORCEINLINE	__forceinline
#define A_INLINE		inline
#define A_ALIGN(n)		__declspec(align(n))

#define Assert			ASSERT


template <typename T>
inline T AAlign(const T ptr, aint32 nAlignment)
{
	return (T)(((aptrint)ptr + nAlignment - 1) & ~(nAlignment - 1));
}

#ifdef A_PLATFORM_64
#define AAlloca16( x )	((void *)((((aint64)_alloca( (x)+15 )) + 15) & ~15))
#else
#define AAlloca16( x )	((void *)((((aint32)_alloca( (x)+15 )) + 15) & ~15))
#endif


#define A_DLL_EXPORT	__declspec(dllexport)
#define A_DLL_IMPORT	__declspec(dllimport)

#define A_DLL_LOCAL

#define A_STDCALL		__stdcall

#endif	//	_A_WIN_TYPES_H_