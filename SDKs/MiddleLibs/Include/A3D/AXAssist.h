/*
 * FILE: AXAssist.h
 *
 * DESCRIPTION: Assist routines
 *
 * CREATED BY: duyuxin, 2003/6/6
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AXASSIST_H_
#define _AXASSIST_H_

#include "ATypes.h"
//	#include "AString.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define ARAND_MAX	RAND_MAX

#define min2(a, b) (((a) > (b)) ? (b) : (a))
#define min3(a, b, c) (min2(min2((a), (b)), (c)))
#define max2(a, b) (((a) > (b)) ? (a) : (b))
#define max3(a, b, c) (max2(max2((a), (b)), (c)))
#define min4(a, b, c, d) (min2(min2((a), (b)), min2((c), (d))))
#define max4(a, b, c, d) (max2(max2((a), (b)), max2((c), (d))))

//#ifndef max
//#define max max2
//#endif

//#ifndef min
//#define min min2
//#endif

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

//	Make ID from string
auint32 a_MakeIDFromString(const char* szStr);
auint32 a_MakeIDFromLowString(const char* szStr);
auint32 a_MakeIDFromFileName(const char* szFile);

void a_CRC32_InitChecksum(auint32& uCrcvalue);
void a_CRC32_Update(auint32& uCrcvalue, const auint8 data);
void a_CRC32_UpdateChecksum(auint32 &uCrcvalue, const void* pData, aint32 uLength);
void a_CRC32_FinishChecksum(auint32& uCrcvalue);
auint32 a_CRC32_BlockChecksum(const void* pData, aint32 length);

bool a_GetStringAfter(const char* szBuffer, const char* szTag, char* szResult);

//	Random number generator
bool a_InitRandom();
int a_Random();
float a_Random(float fMin, float fMax);
int a_Random(int iMin, int iMax);

//  export current memory log to file
void a_ExportMemLog(const char* szPath);

bool a_Equals(afloat32 a, afloat32 b, afloat32 eps);
bool a_Equals(afloat64 a, afloat64 b, afloat64 eps);

inline abool a_IsPowerOfTwo(auint32 x)
{
	return (x && !(x & (x - 1)));
}

inline auint32 a_CeilPowerOfTwo(auint32 x)
{
	--x;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	++x;
	return x;
}

inline auint32 a_FloorPowerOfTwo( auint32 x ) 
{
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	x++;
	return x >> 1;
}

//	Compress / Uncompress
//	int a_zlib_Compress(abyte* dest, auint64* destLen, const abyte* source, auint64 sourceLen, int level=1);
//	int a_zlib_Uncompress(abyte* dest, auint64* destLen, const abyte* source, auint64 sourceLen);

//	Get Milli-second
auint32 a_GetMilliSecond();
//	Get micro-second
auint64 a_GetMicroSecond();
//	Convert gb2132 to utf-16
int a_GB2312ToUTF16(auint16* szDest, const achar* szSrc, int nDestBufferMaxSize);
//	Convert utf-16 to gb2132
int a_UTF16ToGB2312(char* szDest, const auint16* szSrc, int nDestBufferMaxSize);

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////
template <class T>
inline afloat32 a_Magnitude2D(T fX1, T fY1, T fX2, T fY2)
{
	return sqrtf((fX1-fX2)*(fX1-fX2) + (fY1-fY2)*(fY1-fY2));
}

template <class T>
inline T a_DotProduct(T fX1, T fY1, T fX2, T fY2)
{
	return fX1 * fX2 + fY1 * fY2;
}

template <class T>
inline afloat32 a_Normalize2D(T& x, T& y)
{
	afloat32 len = sqrtf(x*x + y*y);
	if (len > 0.0f)
	{
		x /= len;
		y /= len;
	}
	return len;
}

template <class T>
inline void a_Swap(T& lhs, T& rhs)
{
	T tmp;
	tmp = lhs;
	lhs = rhs;
	rhs = tmp;
}

template <class T>
inline const T& a_Min(const T& x, const T& y)
{
	return y < x ? y : x;
}

template <class T>
inline const T& a_Max(const T& x, const T& y)
{
	return y < x ? x : y;
}

template <class T>
inline const T& a_Min(const T& x, const T& y, const T& z)
{
	return a_Min(a_Min(x, y), z);
}

template <class T>
inline const T& a_Max(const T& x, const T& y, const T& z)
{
	return a_Max(a_Max(x, y), z);
}

template <class T>
inline void a_ClampRoof(T& x, const T& max)
{
	if (x > max) x = max;
}

template <class T>
inline void a_ClampFloor(T& x, const T& min)
{
	if (x < min) x = min;
}

template <class T>
inline T ax_Clamp(const T& x, const T& min, const T& max)
{
	if (x < min)
	{
		return min;
	}

	if (x > max)
	{
		return max;
	}

	return x;
}

template <class T>
inline void a_Clamp(T& x, const T& min, const T& max)
{
	if (x < min) x = min;
	if (x > max) x = max;
}

// new rand algorithm
static int randSeed = 0;

inline void	a_Srand_( unsigned seed ) {
	randSeed = seed;
}

inline int	a_Rand_( void ) {
	randSeed = (69069 * randSeed + 1);
	return randSeed & 0x7fff;
}

inline float a_Random_0_to_1() {
	union {
		auint32 d;
		float f;
	} u;
	u.d = (((auint32)a_Rand_() & 0x7fff) << 8) | 0x3f800000;
	return u.f - 1.0f;
}

inline float a_Random_minus1_to_1() {
	union {
		auint32 d;
		float f;
	} u;
	u.d = (((auint32)a_Rand_() & 0x7fff) << 8) | 0x40000000;
	return u.f - 3.0f;
}

inline int	a_RandInt( int lower, int upper)
{
	return lower + (int)((upper + 1 - lower) * a_Random_0_to_1());
}

inline float a_RandFloat( float lower, float upper)
{
	return lower + (upper - lower) * a_Random_0_to_1();
}

//-------------------------------------------------------------
//FastRand
inline int	a_FastRand(int *seed) 
{
	*seed = int(6364136223846793005 * *seed + 1442695040888963407);
	return *seed;
}

inline float a_FastRandFloat(int *seed) 
{
	return (a_FastRand(seed) & 0xffff) / (float)0x10000;
}

inline float a_FastRandFloat(int *seed, float lower, float upper)
{
	return lower + (upper - lower) * a_FastRandFloat(seed);
}

#endif	//	_AXASSIST_H_
