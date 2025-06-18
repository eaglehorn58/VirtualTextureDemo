/*
 * FILE: AXTypes.h
 *
 * DESCRIPTION: Fundermental data types for Angelica 3D Engine
 *
 * CREATED BY: Hedi, 2001/7/27
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _AXTYPES_H_
#define _AXTYPES_H_

//	#include "ASys.h"
#include "ATypes.h"
#include "AXVector.h"
#include "AXMatrix.h"
//	#include "AXQuaternion.h"
#include "ARect.h"
//	#include "AMemBase.h"

typedef aint32		A3DRESULT;	//	Return code data type;
typedef auint32		A3DCOLOR;	//	Color

typedef APointI		A3DPOINT2;
typedef ARectI		A3DRECT;
typedef auint32		HA3DFONT;

#define MAX_TEX_LAYERS		8
#define MAX_SAMPLE_LAYERS	16
#define VS_SAMPLE_LAYERS_NUM 4
#define MIN_VS_SAMPLE_LAYERS D3DVERTEXTEXTURESAMPLER0
#define MAX_VS_SAMPLE_LAYERS D3DVERTEXTEXTURESAMPLER3
#define MAX_CLIP_PLANES		6	//	D3DMAXUSERCLIPPLANES = 32, but 6 is enough for us ?

//	Color value
class A3DCOLORVALUE
{
public:		//	Constructors and Destructors

	A3DCOLORVALUE() {}
	A3DCOLORVALUE(float _r, float _g, float _b, float _a) { r=_r; g=_g; b=_b; a=_a; }
	A3DCOLORVALUE(float c) { r=c; g=c; b=c; a=c; }
	A3DCOLORVALUE(const A3DCOLORVALUE& v) { r=v.r; g=v.g; b=v.b; a=v.a; }
	A3DCOLORVALUE(A3DCOLOR Color);

public:		//	Attributes

	union
	{
		struct
		{
			float r, g, b, a;
		};

		float c[4];
	};

public:		//	Operations

	//	Operator *
	friend A3DCOLORVALUE operator * (const A3DCOLORVALUE& v, float s) { return A3DCOLORVALUE(v.r * s, v.g * s, v.b * s, v.a * s); }
	friend A3DCOLORVALUE operator * (float s, const A3DCOLORVALUE& v) { return A3DCOLORVALUE(v.r * s, v.g * s, v.b * s, v.a * s); }
	friend A3DCOLORVALUE operator * (const A3DCOLORVALUE& v1, const A3DCOLORVALUE& v2) { return A3DCOLORVALUE(v1.r*v2.r, v1.g*v2.g, v1.b*v2.b, v1.a*v2.a); }
	//	Operator + and -
	friend A3DCOLORVALUE operator + (const A3DCOLORVALUE& v1, const A3DCOLORVALUE& v2) { return A3DCOLORVALUE(v1.r+v2.r, v1.g+v2.g, v1.b+v2.b, v1.a+v2.a); }
	friend A3DCOLORVALUE operator - (const A3DCOLORVALUE& v1, const A3DCOLORVALUE& v2) { return A3DCOLORVALUE(v1.r-v2.r, v1.g-v2.g, v1.b-v2.b, v1.a-v2.a); }
	//	Operator != and ==
	friend bool operator != (const A3DCOLORVALUE& v1, const A3DCOLORVALUE& v2) { return (v1.r!=v2.r || v1.g!=v2.g || v1.b!=v2.b || v1.a!=v2.a); }
	friend bool operator == (const A3DCOLORVALUE& v1, const A3DCOLORVALUE& v2) { return (v1.r==v2.r && v1.g==v2.g && v1.b==v2.b && v1.a==v2.a); }
	//	Operator *=
	const A3DCOLORVALUE& operator *= (float s) { r*=s; g*=s; b*=s; a*=s; return *this; }
	const A3DCOLORVALUE& operator *= (const A3DCOLORVALUE& v) { r*=v.r; g*=v.g; b*=v.b; a*=v.a; return *this; }
	//	Operator += and -=
	const A3DCOLORVALUE& operator += (const A3DCOLORVALUE& v) { r+=v.r; g+=v.g; b+=v.b; a+=v.a; return *this; }
	const A3DCOLORVALUE& operator -= (const A3DCOLORVALUE& v) { r-=v.r; g-=v.g; b-=v.b; a-=v.a; return *this; }
	//	Operator =
	const A3DCOLORVALUE& operator = (const A3DCOLORVALUE& v) { r=v.r; g=v.g; b=v.b; a=v.a; return *this; }
	const A3DCOLORVALUE& operator = (A3DCOLOR Color);

	//	Set value
	void Set(float _r, float _g, float _b, float _a) { r=_r; g=_g; b=_b; a=_a; }

	//	Clamp values
	void ClampRoof() { if (r>1.0f) r=1.0f; if (g>1.0f) g=1.0f; if (b>1.0f) b=1.0f; if (a>1.0f) a=1.0f; }
	void ClampFloor() { if (r<0.0f) r=0.0f; if (g<0.0f) g=0.0f; if (b<0.0f) b=0.0f; if (a<0.0f) a=0.0f; }
	void Clamp() 
	{
		if (r > 1.0f) r = 1.0f; else if (r < 0.0f) r = 0.0f;
		if (g > 1.0f) g = 1.0f; else if (g < 0.0f) g = 0.0f;
		if (b > 1.0f) b = 1.0f; else if (b < 0.0f) b = 0.0f;
		if (a > 1.0f) a = 1.0f; else if (a < 0.0f) a = 0.0f;
	}

	//	Convert to A3DCOLOR
	A3DCOLOR ToRGBAColor() const;
};

struct A3DHSVCOLORVALUE
{
	float	h;
	float	s;
	float	v;
	float	a;
public:
	A3DHSVCOLORVALUE() {}
	inline A3DHSVCOLORVALUE(float _h, float _s, float _v, float _a) { h = _h; s = _s; v = _v; a = _a; }
	inline A3DHSVCOLORVALUE(float c) {h = c; s = c; v = c; a = c;	}
};

//Viewport Parameters;
struct A3DVIEWPORTPARAM
{	
    auint32	X;
    auint32	Y;
    auint32	Width;
    auint32	Height;
    float	MinZ;
    float	MaxZ;
};

struct A3DMATERIALPARAM
{
	A3DCOLORVALUE   Diffuse;
	A3DCOLORVALUE   Ambient;
	A3DCOLORVALUE   Specular;
	A3DCOLORVALUE   Emissive;
	float           Power;
};

#endif	//	_AXTYPES_H_
