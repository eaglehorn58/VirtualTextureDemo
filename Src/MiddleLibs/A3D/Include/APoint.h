//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#ifndef _APOINT_H_
#define _APOINT_H_

#include "ATypes.h"

#pragma warning (disable: 4786)

///////////////////////////////////////////////////////////////////////////

template <class T>
class APoint
{
public:		//	Types

public:		//	Constructions and Destructions

	APoint() {}
	APoint(const T& _x, const T& _y) { x = _x; y = _y; }
	APoint(const APoint& pt) { x = pt.x; y = pt.y; }
	
public:		//	Attributes

	T	x, y;

public:		//	Operaitons

	//	Offset point
	void Offset(const T& ox, const T& oy) { x += ox; y += oy; }
	//	Set point
	void Set(const T& _x, const T& _y) { x = _x; y = _y; }

	//	== and != operator
	friend bool operator != (const APoint& pt1, const APoint& pt2) { return pt1.x != pt2.x || pt1.y != pt2.y; }
	friend bool operator == (const APoint& pt1, const APoint& pt2) { return pt1.x == pt2.x && pt1.y == pt2.y; }

	//	+ and - operator
	friend APoint operator + (const APoint& pt1, const APoint& pt2) { return APoint(pt1.x+pt2.x, pt1.y+pt2.y); }
	friend APoint operator - (const APoint& pt1, const APoint& pt2) { return APoint(pt1.x-pt2.x, pt1.y-pt2.y); }

	APoint operator + () const { return *this; }
	APoint operator - () const { return APoint(-x, -y); }

	//	= operator
	const APoint& operator = (const APoint& pt) { x = pt.x; y = pt.y; return *this; }

	//	+= and -= operator
	const APoint& operator += (const APoint& pt) { x += pt.x; y += pt.y; return *this; }
	const APoint& operator -= (const APoint& pt) { x -= pt.x; y -= pt.y; return *this; }

protected:	//	Attributes

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Predefined type
//
///////////////////////////////////////////////////////////////////////////

typedef APoint<int>		APointI;
typedef APoint<float>	APointF;

struct APointHashFunc
{
	inline auint32 operator()(const APointI& p) const { return p.x * 10000 + p.y; }
	inline auint32 operator()(const APointF& p) const { return (auint32)(p.x * 1000.f + p.y); }
};

#endif	//	_APOINT_H_
