//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


//#include "A3DPI.h"
#include "AXFuncs.h"
#include "AXAssist.h"

///////////////////////////////////////////////////////////////////////////

// namespace A3D
// {
// 	const AXVector3	g_vAxisX(1.0f, 0.0f, 0.0f);
// 	const AXVector3	g_vAxisY(0.0f, 1.0f, 0.0f);
// 	const AXVector3	g_vAxisZ(0.0f, 0.0f, 1.0f);
// 	const AXVector3	g_vOrigin(0.0f);
// 	const AXMatrix4	g_matIdentity(AXMatrix4::IDENTITY);
// }

A3DCOLORVALUE::A3DCOLORVALUE(A3DCOLOR Color)
{
	static float fTemp = 1.0f / 255.0f;
	a = A3DCOLOR_GETALPHA(Color) * fTemp;
	r = A3DCOLOR_GETRED(Color) * fTemp;
	g = A3DCOLOR_GETGREEN(Color) * fTemp;
	b = A3DCOLOR_GETBLUE(Color) * fTemp;
}

const A3DCOLORVALUE& A3DCOLORVALUE::operator = (A3DCOLOR Color)
{
	static float fTemp = 1.0f / 255.0f;
	a = A3DCOLOR_GETALPHA(Color) * fTemp;
	r = A3DCOLOR_GETRED(Color) * fTemp;
	g = A3DCOLOR_GETGREEN(Color) * fTemp;
	b = A3DCOLOR_GETBLUE(Color) * fTemp;
	return *this;
}

//	Convert color value to A3DCOLOR

A3DCOLOR A3DCOLORVALUE::ToRGBAColor() const
{
	int _a = (int)(a * 255);
	int _r = (int)(r * 255);
	int _g = (int)(g * 255);
	int _b = (int)(b * 255);
	return A3DCOLORRGBA(min2(_r, 255), min2(_g, 255), min2(_b, 255), min2(_a, 255));
}



