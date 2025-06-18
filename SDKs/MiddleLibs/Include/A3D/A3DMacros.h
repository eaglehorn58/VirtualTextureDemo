/*
 * FILE: A3DMacros.h
 *
 * DESCRIPTION: Fundermental macros for Angelica 3D Engine
 *
 * CREATED BY: Hedi, 2001/7/27
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DMACROS_H_
#define _A3DMACROS_H_

#include "AXTypes.h"

#define A3D_PI		3.1415926535f
#define A3D_2PI		6.2831853072f
#define A3D_PI_2	1.5707963268f
#define A3D_PI_4	0.7853981634f


//Warning: you must supply byte values as r, g and b or the result may be undetermined
#define A3DCOLORRGB(r, g, b) ((A3DCOLOR)(0xff000000 | ((b) << 16) | ((g) << 8) | (r)))					
//Warning: you must supply byte values as r, g, b and a, or the result may be undetermined	
#define A3DCOLORRGBA(r, g, b, a) ((A3DCOLOR)(((a) << 24) | ((b) << 16) | ((g) << 8) | (r)))

#define A3DCOLOR_GETRED(color) ((unsigned char)(((color) & 0x000000ff)))
#define A3DCOLOR_GETGREEN(color) ((unsigned char)(((color) & 0x0000ff00) >> 8))
#define A3DCOLOR_GETBLUE(color) ((unsigned char)(((color) & 0x00ff0000) >> 16))
#define A3DCOLOR_GETALPHA(color) ((unsigned char)(((color) & 0xff000000) >> 24))

#define A3DCOLOR_SETRED(color, r) { color = (((color) & 0xffffff00) | ((r) & 0xff)); }
#define A3DCOLOR_SETGREEN(color, g) { color = (((color) & 0xffff00ff) | (((g) & 0xff)) << 8); }
#define A3DCOLOR_SETBLUE(color, b) { color = (((color) & 0xff00ffff) | (((b) & 0xff)) << 16); }
#define A3DCOLOR_SETALPHA(color, a) { color = (((color) & 0x00ffffff) | (((a) & 0xff)) << 24); }

//	Convert a DWORD formatted in 0xaarrggbb to current system's A3DCOLOR
//	The way to use this macro is often like: A3DCOLOR color = A3DCOLOR_FROM_DWORD_ARGB(0xff00358a);
#define A3DCOLOR_FROM_DWORD_ARGB(dw) (((dw) & 0xff00ff00) | (((dw) & 0x00ff0000) >> 16) | (((dw) & 0x000000ff) << 16))

//	Convert color from Angelica2.1 format to current system's format when load it from file
#define ANGELICA21_LOAD_A3DCOLOR(color) { color = A3DCOLOR_FROM_DWORD_ARGB(color); }
//	Convert color from current system's format to Angelica2.1 format when save it to file
#define ANGELICA21_SAVE_A3DCOLOR(color) ANGELICA21_LOAD_A3DCOLOR(color)

#define DEG2RAD(deg) ((deg) * A3D_PI / 180.0f)
#define RAD2DEG(rad) ((rad) * 180.0f / A3D_PI)

#define FLOATISZERO(x)	((x) > -1e-7f && (x) < 1e-7f)

//	Release A3D objects
#define A3DRELEASE(p) \
{ \
	if (p) \
	{ \
		(p)->Release(); \
		delete (p); \
		(p) = NULL; \
	} \
}

//	Release A3D interfaces
#define A3DINTERFACERELEASE(p) \
{ \
	if (p) \
	{ \
	(p)->Release(); \
	(p) = NULL; \
	} \
}

//	Safe delete object.
#define A3DSAFEDELETE(p) \
{ \
	if (p) \
	{ \
		delete p; \
		p = NULL; \
	} \
}

//	Safe delete object array.
#define A3DSAFEDELETEARRAY(p) \
{ \
	if (p) \
	{ \
		delete [] p; \
		p = NULL; \
	} \
}

#define MEMBER_OFFSET(_CLS, _MEMBER)   ((size_t)(&(((_CLS*)0)->_MEMBER)))

#endif

