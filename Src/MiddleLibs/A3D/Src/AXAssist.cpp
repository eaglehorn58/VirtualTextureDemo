﻿//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#include <atlstr.h>
#include "ATypes.h"
#include "AXAssist.h"
//#include "AMemory.h"
//#include "ZLib.h"

///////////////////////////////////////////////////////////////////////////

#define CRC32_POLY	0x04c11db7
#define CRC32_INIT_VALUE	0xffffffffL
#define CRC32_XOR_VALUE		0xffffffffL

///////////////////////////////////////////////////////////////////////////

//static int l_iCharCodePage	= 936;	//	Default is chinese

static const auint32 l_aCRC32Table[] =
{
	0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
	0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
	0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
	0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
	0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
	0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
	0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
	0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
	0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
	0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
	0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
	0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
	0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
	0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
	0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
	0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
	0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
	0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
	0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
	0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
	0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
	0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
	0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
	0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
	0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
	0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
	0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
	0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
	0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
	0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
	0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
	0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
	0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
	0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
	0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
	0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
	0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
	0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
	0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
	0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
	0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
	0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
	0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
	0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
	0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
	0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
	0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
	0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
	0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
	0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
	0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
	0x2d02ef8dL
};

///////////////////////////////////////////////////////////////////////////

/*	Create a ID base on specified string

	Return created ID.

	szStr: specified string.
*/
auint32 a_MakeIDFromString(const char* szStr)
{
	ASSERT(szStr);

	auint32 c = 0xffffffff; /* shift register contents */
	const unsigned char* p = (const unsigned char*)szStr;

	while (*p)
	{
		c = l_aCRC32Table[((int)c ^ (*p++)) & 0xff] ^ (c >> 8);
	};

	return c ^ 0xffffffffL;
}

/*	Create a ID base on specified string's lowercase version

	Return created ID.

	szStr: specified string.
*/
auint32 a_MakeIDFromLowString(const char* szStr)
{
	ASSERT(szStr);

	auint32 c = 0xffffffff; /* shift register contents */
	const unsigned char* p = (const unsigned char*)szStr;

	while (*p)
	{
		unsigned char ch = *p++;
		if (ch >= 'A' && ch <= 'Z')
			ch += 32;

		c = l_aCRC32Table[((int)c ^ (ch)) & 0xff] ^ (c >> 8);
	}

	return c ^ 0xffffffffL;
}

auint32 a_MakeIDFromFileName(const char* szFile)
{
	ASSERT(szFile);

	auint32 c = 0xffffffff; /* shift register contents */
	const unsigned char* p = (const unsigned char*)szFile;

	while (*p)
	{
		unsigned char ch = *p++;
		if (ch >= 'A' && ch <= 'Z')
			ch += 32;
		else if (ch == '/')
			ch = '\\';

		c = l_aCRC32Table[((int)c ^ (ch)) & 0xff] ^ (c >> 8);
	}

	return c ^ 0xffffffffL;
}

// CRC operations
void a_CRC32_InitChecksum(auint32& uCrcvalue)
{
    uCrcvalue = CRC32_INIT_VALUE;
}

void a_CRC32_Update(auint32& uCrcvalue, const auint8 data)
{
    uCrcvalue = l_aCRC32Table[ ( uCrcvalue ^ data ) & 0xff ] ^ ( uCrcvalue >> 8 );
}

void a_CRC32_UpdateChecksum(auint32 &uCrcvalue, const void* pData, aint32 uLength)
{
    auint32 uCrc;
    const auint8 *pBuf = (const auint8*) pData;

    uCrc = uCrcvalue;
    while( uLength-- ) 
    {
        uCrc = l_aCRC32Table[ ( uCrc ^ ( *pBuf++ ) ) & 0xff ] ^ ( uCrc >> 8 );
    }
    uCrcvalue = uCrc;
}

void a_CRC32_FinishChecksum(auint32& uCrcvalue)
{
    uCrcvalue ^= CRC32_XOR_VALUE;
}

auint32 a_CRC32_BlockChecksum(const void* pData, aint32 length)
{
    auint32 uCrc;

    a_CRC32_InitChecksum(uCrc);
    a_CRC32_UpdateChecksum(uCrc, pData, length);
    a_CRC32_FinishChecksum(uCrc);
    return uCrc;
}

/*	Get string after specified tag string
 
	szBuffer: buffer will be searched.
	szTag: tag string.
	szResult: buffer used to store result string.
*/
bool a_GetStringAfter(const char* szBuffer, const char* szTag, char* szResult)
{
	szResult[0] = '\0';

	const char* pch = strstr(szBuffer, szTag);
	if (pch != szBuffer)
		return false;

	pch += strlen(szTag);
	strcpy(szResult, pch);
	
	return true;
}

static auint32 l_dwMultiplier = 16807;    // (7 ** 5)
static auint32 l_dwModuler = 2147483647;  // (2 ** 31) - 1	
static auint32 l_dwSeed = 765983;
static double l_invModuler = 1.0 / 2147483647;

//	Initalize random number generator
bool a_InitRandom()
{
	l_dwSeed = (unsigned)time(NULL) + 1;
	srand((unsigned)time(NULL));
	return true;
}

//	Generator a random number
int a_Random()
{
	auint32 dwRandUniform = (auint32)(((aint64)l_dwMultiplier * l_dwSeed) % (aint64)l_dwModuler);
	l_dwSeed = dwRandUniform;
	return dwRandUniform;
	//return rand();
}

/*	Get a random float value in [fMin, fMax].

	Return random float value.

	fMin, fMax: Range of random value
*/
float a_Random(float fMin, float fMax)
{
	float fDelta = fMax - fMin;
	float fRand	 = (float)(a_Random() * l_invModuler);
	return fMin + fRand * fDelta;
}

/*	Get a random int value in [iMin, iMax].

	Return random float value.

	fMin, fMax: Range of random value
*/
int a_Random(int iMin, int iMax)
{
	int iDelta	= iMax - iMin;
	int iRand	= rand() % (iDelta + 1);
	return iRand + iMin;
}

bool a_Equals(afloat32 a, afloat32 b, afloat32 eps)
{
	const afloat32 diff = fabs(a - b);
	return (diff < eps)? true : false;
}

bool a_Equals(afloat64 a, afloat64 b, afloat64 eps)
{
	const afloat64 diff = fabs(a - b);
	return (diff < eps)? true : false;
}

/*
void* _zlib_MemAlloc(void* opaque, unsigned int items, unsigned int size)
{
	return a_malloctemp(size * items);
}

void _zlib_MemFree(void* opaque, void* ptr)
{
	a_freetemp(ptr);
}

int a_zlib_Compress(abyte* dest, auint64* destLen, const abyte* source, auint64 sourceLen, int level)
{
	z_stream stream;
	int err;

	stream.next_in = (Bytef*)source;
	stream.avail_in = (uInt)sourceLen;
#ifdef MAXSEG_64K
	// Check for source > 64K on 16-bit machine:
	if ((auint64)stream.avail_in != sourceLen) return Z_BUF_ERROR;
#endif
	stream.next_out = dest;
	stream.avail_out = (uInt)*destLen;
	if ((auint64)stream.avail_out != *destLen) return Z_BUF_ERROR;

	stream.zalloc = &_zlib_MemAlloc;
	stream.zfree = &_zlib_MemFree;
	stream.opaque = (voidpf)0;

	err = deflateInit(&stream, level);
	if (err != Z_OK) return err;

	err = deflate(&stream, Z_FINISH);
	if (err != Z_STREAM_END) {
		deflateEnd(&stream);
		return err == Z_OK ? Z_BUF_ERROR : err;
	}
	*destLen = stream.total_out;

	err = deflateEnd(&stream);
	return err;
}

int a_zlib_Uncompress(abyte* dest, auint64* destLen, const abyte* source, auint64 sourceLen)
{
	z_stream stream;
	int err;

	stream.next_in = (Bytef*)source;
	stream.avail_in = (uInt)sourceLen;
	//	Check for source > 64K on 16-bit machine:
	if ((auint64)stream.avail_in != sourceLen) return Z_BUF_ERROR;

	stream.next_out = dest;
	stream.avail_out = (uInt)*destLen;
	if ((auint64)stream.avail_out != *destLen) return Z_BUF_ERROR;

	stream.zalloc = &_zlib_MemAlloc;
	stream.zfree = &_zlib_MemFree;

	err = inflateInit(&stream);
	if (err != Z_OK) return err;

	err = inflate(&stream, Z_FINISH);
	if (err != Z_STREAM_END) {
		inflateEnd(&stream);
		return err == Z_OK ? Z_BUF_ERROR : err;
	}
	*destLen = stream.total_out;

	err = inflateEnd(&stream);
	return err;
}
*/

auint32 a_GetMilliSecond()
{
	return (auint32)(a_GetMicroSecond() / 1000);
}

auint64 a_GetMicroSecond()
{
	static LARGE_INTEGER liFrequency;
	static abool bFirstTime = atrue;

	if (bFirstTime)
	{
		bFirstTime = false;
		::QueryPerformanceFrequency(&liFrequency);
	}

	LARGE_INTEGER liCounter;
	::QueryPerformanceCounter(&liCounter);

	auint64 uSecond = liCounter.QuadPart / liFrequency.QuadPart;
	auint64 uRemainder = liCounter.QuadPart % liFrequency.QuadPart;

	return uSecond * 1000000 + uRemainder * 1000000 / liFrequency.QuadPart;
}

int a_GB2312ToUTF16(auint16* szDest, const achar* szSrc, int nDestBufferMaxSize)
{
	return MultiByteToWideChar(CP_ACP, 0, szSrc, -1, (wchar_t*)szDest, nDestBufferMaxSize);
}

int a_UTF16ToGB2312(char* szDest, const auint16* szSrc, int nDestBufferMaxSize)
{
	return WideCharToMultiByte(936, 0, (const wchar_t*)szSrc, -1, szDest, nDestBufferMaxSize, NULL, NULL);
}

