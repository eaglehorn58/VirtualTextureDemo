//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#ifndef _ARECT_H_
#define _ARECT_H_

#include "APoint.h"
#include "AXAssist.h"

#pragma warning (disable: 4786)

///////////////////////////////////////////////////////////////////////////

template <class T>
class ATRect
{
public:		//	Types

public:		//	Constructions and Destructions

	ATRect() {}
	ATRect(const T& _left, const T& _top, const T& _right, const T& _bottom) { left = _left; top = _top; right = _right; bottom = _bottom; }
	ATRect(const ATRect& rc) { left = rc.left; top = rc.top; right = rc.right; bottom = rc.bottom; }
	
public:		//	Attributes

	T	left, top, right, bottom;

public:		//	Operaitons

	//	== and != operator
	friend bool operator != (const ATRect& rc1, const ATRect& rc2) { return rc1.left != rc2.left || rc1.top != rc2.top || rc1.right != rc2.right || rc1.bottom != rc2.bottom; }
	friend bool operator == (const ATRect& rc1, const ATRect& rc2) { return rc1.left == rc2.left && rc1.top == rc2.top && rc1.right == rc2.right && rc1.bottom == rc2.bottom; }

	//	+ and - operator
	friend ATRect operator + (const ATRect& rc1, const ATRect& rc2) { return ATRect(rc1.left+rc2.left, rc1.top+rc2.top, rc1.right+rc2.right, rc1.bottom+rc2.bottom); }
	friend ATRect operator - (const ATRect& rc1, const ATRect& rc2) { return ATRect(rc1.left-rc2.left, rc1.top-rc2.top, rc1.right-rc2.right, rc1.bottom-rc2.bottom); }
	friend ATRect operator + (const ATRect& rc1, const APoint<T>& pt) { return ATRect(rc1.left+pt.x, rc1.top+pt.y, rc1.right+pt.x, rc1.bottom+pt.y); }
	friend ATRect operator - (const ATRect& rc1, const APoint<T>& pt) { return ATRect(rc1.left-pt.x, rc1.top-pt.y, rc1.right-pt.x, rc1.bottom-pt.y); }

	//	&= and |= operator
	const ATRect& operator &= (const ATRect& rc) { *this = *this & rc; return *this; }
	const ATRect& operator |= (const ATRect& rc) { *this = *this | rc; return *this; }

	ATRect operator + () const { return *this; }
	ATRect operator - () const { return ATRect(-left, -top, -right, -bottom); }

	//	= operator
	const ATRect& operator = (const ATRect& rc) { left = rc.left; top = rc.top; right = rc.right; bottom = rc.bottom; return *this; }

	//	+= and -= operator
	const ATRect& operator += (const ATRect& rc) { left += rc.left; top += rc.top; right += rc.right; bottom += rc.bottom; return *this; }
	const ATRect& operator -= (const ATRect& rc) { left -= rc.left; top -= rc.top; right -= rc.right; bottom -= rc.bottom; return *this; }
	const ATRect& operator += (const APoint<T>& pt) { left += pt.x; top += pt.y; right += pt.x; bottom += pt.y; return *this; }
	const ATRect& operator -= (const APoint<T>& pt) { left -= pt.x; top -= pt.y; right -= pt.x; bottom -= pt.y; return *this; }

	//	Get width of rectangle
	A_FORCEINLINE T Width() const { return right - left; }
	//	Get height of rectangle
	A_FORCEINLINE T Height() const { return bottom - top; }
	//	Get center point of rectangle
	A_FORCEINLINE APoint<T> CenterPoint() const { return APoint<T>((left+right) / 2, (top+bottom) / 2); }
	//	Set rectangle value
	A_FORCEINLINE void SetRect(const T& _left, const T& _top, const T& _right, const T& _bottom) { left=_left; top=_top; right=_right; bottom=_bottom; }
	
	//	Point in rectangle
	A_FORCEINLINE bool PtInRect(const T& x, const T& y) const { return (x >= left && x < right && y >= top && y < bottom) ? true : false; }
	A_FORCEINLINE bool PtInRect(const APoint<T>& pt) const { return PtInRect(pt.x, pt.y); }

	//	Normalize rectangle. Note: The following CRect member functions require 
	//	normalized rectangles in order to work properly: Height, Width, Size, 
	//	IsEmpty, PtInRect, SetUnion, SetIntersect, operator ==, operator !=, 
	//	operator |, operator |=, operator &, and operator &=
	void Normalize();

	//	All members are 0 ?
	A_FORCEINLINE bool IsRectNull() const { return (left == 0 && top == 0 && right == 0 && bottom == 0); }
	//	Rectangle is empty ?
	A_FORCEINLINE bool IsEmpty() const { return (Width() == 0 || Height() == 0); }
	//	Set all members to 0
	A_FORCEINLINE void Clear() { left = top = right = bottom = 0; }
	//	Deflate rectangle
	A_FORCEINLINE void Deflate(const T& x, const T& y) { left += x; top += y; right -= x; bottom -= y; }
	A_FORCEINLINE void Deflate(const ATRect<T>& rc) { left += rc.left; top += rc.top; right -= rc.right; bottom -= rc.bottom; }
	A_FORCEINLINE void Deflate(const T& l, const T& t, const T& r, const T& b) { left += l; top += t; right -= r; bottom -= b; }
	//	Inflate rectangle
	A_FORCEINLINE void Inflate(const T& x, const T& y) { left -= x; top -= y; right += x; bottom += y; }
	A_FORCEINLINE void Inflate(const ATRect& rc) { left -= rc.left; top -= rc.top; right += rc.right; bottom += rc.bottom; }
	A_FORCEINLINE void Inflate(const T& l, const T& t, const T& r, const T& b) { left -= l; top -= t; right += r; bottom += b; }
	//	Offset rectangle
	A_FORCEINLINE void Offset(const T& x, const T& y) { left += x; top += y; right += x; bottom += y; }
	A_FORCEINLINE void Offset(const APoint<T>& pt) { *this += pt; }
	//	Set rectangle as union result
	A_FORCEINLINE void SetUnion(const ATRect<T>& rc1, const ATRect<T>& rc2) { *this = rc1 | rc2; }
	//	Set rectangle as intersect result
	A_FORCEINLINE void SetIntersect(const ATRect<T>& rc1, const ATRect<T>& rc2) { *this = rc1 & rc2; }

	void ClipAgainst(const ATRect<T>& other);

protected:	//	Attributes

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////

typedef ATRect<int>		ARectI;
typedef ATRect<float>	ARectF;

///////////////////////////////////////////////////////////////////////////

//	operator & calculate the intersection of two rectangles, both rc1 and rc2 
//	need to be normlaized in order the result
template <class T>
ATRect<T> operator & (const ATRect<T>& rc1, const ATRect<T>& rc2)
{
	if (rc1.IsEmpty() || rc2.IsEmpty())
		return ATRect<T>(0, 0, 0, 0);

	if (rc1.left >= rc2.right || rc2.left >= rc1.right ||
		rc1.top >= rc2.bottom || rc2.top >= rc1.bottom)
		return ATRect<T>(0, 0, 0, 0);

	return ATRect<T>(rc1.left > rc2.left ? rc1.left : rc2.left,
				  rc1.top > rc2.top ? rc1.top : rc2.top,
				  rc1.right < rc2.right ? rc1.right : rc2.right,
				  rc1.bottom < rc2.bottom ? rc1.bottom : rc2.bottom);
}

//	operator | calculate the union of two rectangles, both rc1 and rc2 
//	need to be normlaized in order the result
template <class T>
ATRect<T> operator | (const ATRect<T>& rc1, const ATRect<T>& rc2)
{
	if (rc1.IsEmpty())
		return rc2;

	if (rc2.IsEmpty())
		return rc1;
	
	return ATRect<T>(rc1.left < rc2.left ? rc1.left : rc2.left,
				  rc1.top < rc2.top ? rc1.top : rc2.top,
				  rc1.right > rc2.right ? rc1.right : rc2.right,
				  rc1.bottom > rc2.bottom ? rc1.bottom : rc2.bottom);
}

//	Normalize rectangle, set both the height and width are positive
template <class T>
void ATRect<T>::Normalize()
{
	if (left > right)
		a_Swap(left, right); 
	
	if (top > bottom) 
		a_Swap(top, bottom);
}


template <class T>
void ATRect<T>::ClipAgainst( const ATRect<T>& other )
{
	if (other.right < right)
		right = other.right;
	if (other.bottom < bottom)
		bottom = other.bottom;

	if (other.left > left)
		left = other.left;
	if (other.top > top)
		top = other.top;

	// correct possible invalid rect resulting from clipping
	if (top > bottom)
		top = bottom;
	if (left > right)
		left = right;
}

#endif	//	_ARECT_H_
