/*
 * FILE: AXVector.h
 *
 * DESCRIPTION: Vector class
 *
 * CREATED BY: duyuxin, 2003/6/5
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AXVECTOR_H_
#define _AXVECTOR_H_

#include "AXAssist.h"

#ifdef ARM_NEON
#include "NE10.h"
#endif

/*
#ifdef ENABLE_ARM_NEON_STASTICS
#include "AStats.h"

//////////////////////////////////////////////////////////////////////////////////
//ANeonStatGroup
class ANeonStatGroup : public AStatGroup
{
public:
	ANeonStatGroup();
	~ANeonStatGroup();

public:
	AStatCounter m_neonFuncionCalledCount;
};

extern ANeonStatGroup* g_pANeonStats;
#endif
*/
//#ifdef ENABLE_ARM_NEON_STASTICS
//#define NEON_STATISTICS_TICK g_pANeonStats->m_neonFuncionCalledCount.m_Value += 1;
//#else
#define NEON_STATISTICS_TICK
//#endif

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////

#ifdef ARM_NEON
void InitNeon();
#endif

///////////////////////////////////////////////////////////////////////////
//
//	class AXVector3
//
///////////////////////////////////////////////////////////////////////////

class AXVector3 //: public AMemBase
{
public:		//	Types

public:		//	Constructions and Destructions

	AXVector3()
    {
#ifdef ARM_NEON
        InitNeon();
#endif
    }
	explicit AXVector3(float m) : x(m), y(m), z(m)
    {        
#ifdef ARM_NEON
        InitNeon();
#endif
    }
    
	AXVector3(float x, float y, float z) : x(x), y(y), z(z)
    {        
#ifdef ARM_NEON
        InitNeon();
#endif
    }
    
	AXVector3(const AXVector3& v) : x(v.x), y(v.y), z(v.z){ }

public:		//	Attributes

	union
	{
		struct
		{
			float x, y, z;
		};

		float m[3];
	};

public:		//	Operaitons

	//	+ operator
	friend AXVector3 operator + (const AXVector3& v1, const AXVector3& v2)
	{
		NEON_STATISTICS_TICK
#ifdef ARM_NEON
		AXVector3 vDst;
		ne10_add_float((ne10_float32_t*)vDst.m, (ne10_float32_t*)v1.m, (ne10_float32_t*)v2.m, 3);
		return vDst;
#else
        return AXVector3(v1.x+v2.x, v1.y+v2.y, v1.z+v2.z);
#endif	
	}
    
	//	- operator
	friend AXVector3 operator - (const AXVector3& v1, const AXVector3& v2)
    {
		NEON_STATISTICS_TICK
#ifdef ARM_NEON
		AXVector3 vDst;
        ne10_sub_float((ne10_float32_t*)vDst.m, (ne10_float32_t*)v1.m, (ne10_float32_t*)v2.m, 3);
		return vDst;
#else
        return AXVector3(v1.x-v2.x, v1.y-v2.y, v1.z-v2.z);
#endif
    }

	//	* operator
	friend AXVector3 operator * (const AXVector3& v, float f)
    {
		NEON_STATISTICS_TICK
#ifdef ARM_NEON
		AXVector3 vDst;
        ne10_mulc_float((ne10_float32_t*)vDst.m, (ne10_float32_t*)v.m, f, 3);
		return vDst;
#else
        return AXVector3(v.x*f, v.y*f, v.z*f);
#endif
    }
    
	friend AXVector3 operator * (float f, const AXVector3& v)
    {
		NEON_STATISTICS_TICK
#ifdef ARM_NEON
		AXVector3 vDst;
		ne10_mulc_float((ne10_float32_t*)vDst.m, (ne10_float32_t*)v.m, f, 3);
		return vDst;
#else
        return AXVector3(v.x*f, v.y*f, v.z*f);
#endif
    }
    
	friend AXVector3 operator * (const AXVector3& v, int f) { return v * (float)f; }
	friend AXVector3 operator * (int f, const AXVector3& v) { return v * (float)f; }
    friend AXVector3 operator * (const AXVector3& v1, const AXVector3& v2)
    {
		NEON_STATISTICS_TICK
#ifdef ARM_NEON
		AXVector3 vDst;
        ne10_mul_float((ne10_float32_t*)vDst.m, (ne10_float32_t*)v1.m, (ne10_float32_t*)v2.m, 3);
		return vDst;
#else
		return AXVector3(v1.x*v2.x, v1.y*v2.y, v1.z*v2.z);
#endif
    }

	//	/ operator
	friend AXVector3 operator / (const AXVector3& v, float f)
    {
		NEON_STATISTICS_TICK
        f = 1.0f / f;
#ifdef ARM_NEON
		AXVector3 vDst;
        ne10_mulc_float((ne10_float32_t*)vDst.m, (ne10_float32_t*)v.m, f, 3);
		return vDst;
#else
        return AXVector3(v.x*f, v.y*f, v.z*f);
#endif
    }
    
	friend AXVector3 operator / (const AXVector3& v, int f) { return v / (float)f; }
	
    friend AXVector3 operator / (const AXVector3& v1, const AXVector3& v2)
    {
		NEON_STATISTICS_TICK
#ifdef ARM_NEON
		AXVector3 vDst;
        ne10_div_float((ne10_float32_t*)vDst.m, (ne10_float32_t*)v1.m, (ne10_float32_t*)v2.m, 3);
		return vDst;
#else
        return AXVector3(v1.x/v2.x, v1.y/v2.y, v1.z/v2.z);
#endif
    }

	//	Dot product
	friend float DotProduct(const AXVector3& v1, const AXVector3& v2);

	//	Cross product
	friend AXVector3 CrossProduct(const AXVector3& v1, const AXVector3& v2)
	{
		NEON_STATISTICS_TICK
#ifdef ARM_NEON
		AXVector3 vDst;
        ne10_cross_vec3f((ne10_vec3f_t*)vDst.m, (ne10_vec3f_t*)v1.m, (ne10_vec3f_t*)v2.m, 1);\
		return vDst;
#else
		return AXVector3(v1.y * v2.z - v1.z * v2.y,
						  v1.z * v2.x - v1.x * v2.z,
						  v1.x * v2.y - v1.y * v2.x);
#endif
	}

	void CrossProduct(const AXVector3& v1, const AXVector3& v2)
	{
		NEON_STATISTICS_TICK
#ifdef ARM_NEON
        ne10_cross_vec3f((ne10_vec3f_t*)m, (ne10_vec3f_t*)v1.m, (ne10_vec3f_t*)v2.m, 1);
#else
		x = v1.y * v2.z - v1.z * v2.y;
		y = v1.z * v2.x - v1.x * v2.z;
		z = v1.x * v2.y - v1.y * v2.x;
#endif
	}

	//	== operator
	friend bool operator == (const AXVector3& v1, const AXVector3& v2) { return v1.x==v2.x && v1.y==v2.y && v1.z==v2.z; }
	//	!= operator
	friend bool operator != (const AXVector3& v1, const AXVector3& v2) { return v1.x!=v2.x || v1.y!=v2.y || v1.z!=v2.z; }

	//	+= operator
	const AXVector3& operator += (const AXVector3& v)
    {
		NEON_STATISTICS_TICK
#ifdef ARM_NEON
        ne10_add_float((ne10_float32_t*)m, (ne10_float32_t*)m, (ne10_float32_t*)v.m, 3);
#else
        x += v.x; y += v.y; z += v.z;
#endif
        return *this;
    }
    
	//	-= operator
	const AXVector3& operator -= (const AXVector3& v)
    {
		NEON_STATISTICS_TICK
#ifdef ARM_NEON
        ne10_sub_float((ne10_float32_t*)m, (ne10_float32_t*)m, (ne10_float32_t*)v.m, 3);
#else
        x -= v.x; y -= v.y; z -= v.z;
#endif
        return *this;
    }
    
	//	*= operator
	const AXVector3& operator *= (float f)
    {
		NEON_STATISTICS_TICK
#ifdef ARM_NEON
        ne10_mulc_float((ne10_float32_t*)m, (ne10_float32_t*)m, f, 3);
#else
        x *= f; y *= f; z *= f;
#endif
        return *this;
    }

	const AXVector3& operator *= (const AXVector3& v)
	{
		NEON_STATISTICS_TICK
#ifdef ARM_NEON
		ne10_mul_float((ne10_float32_t*) m, (ne10_float32_t*) m, (ne10_float32_t*) v.m, 3);
#else
		x *= v.x; y *= v.y; z *= v.z;
		return *this;
#endif
	}
    
	//	/= operator
	const AXVector3& operator /= (float f)
    {
		NEON_STATISTICS_TICK
        f = 1.0f / f;        
#ifdef ARM_NEON
        ne10_mulc_float((ne10_float32_t*)m, (ne10_float32_t*)m, f, 3);
#else
        x *= f; y *= f; z *= f;
#endif
        return *this;
    }
    
	//	= operator
	const AXVector3& operator = (const AXVector3& v)
    {
		NEON_STATISTICS_TICK
#ifdef ARM_NEON
        ne10_setc_vec3f((ne10_vec3f_t*)m, (const ne10_vec3f_t*)v.m, 1);
#else
        x = v.x; y = v.y; z = v.z;
#endif
        return *this;
    }

	AXVector3 operator + () const { return *this; }
	AXVector3 operator - () const { return AXVector3(-x,-y,-z); }

	void Set(float _x, float _y, float _z) { x = _x; y = _y; z = _z; }
	void Clear()
    {
		NEON_STATISTICS_TICK
#ifdef ARM_NEON
        ne10_setc_float((ne10_float32_t*)m, 0.0f, 3);
#else
        x = y = z = 0.0f;
#endif
    }
	bool IsZero() const { return x == 0.0f && y == 0.0f && z == 0.0f; }
	bool IsEquals(const AXVector3 & v, afloat32 epsilon) const
	{
		return a_Equals(x, v.x, epsilon) && a_Equals(y, v.y, epsilon) && a_Equals(z, v.z, epsilon);
	}
	
	//	Magnitude
	float Magnitude() const
    {
        return (float)(sqrt(x * x + y * y + z * z));
    }
    
	float MagnitudeH() const { return (float)(sqrt(x * x + z * z)); }
    
	float SquaredMagnitude() const
    {
		NEON_STATISTICS_TICK
		float fRet;
#ifdef ARM_NEON
        ne10_dot_vec3f(&fRet, (ne10_vec3f_t*)m, (ne10_vec3f_t*)m, 1);
#else
        fRet = x * x + y * y + z * z;
#endif
		return fRet;
    }
    
	float SquaredMagnitudeH() const { return x * x + z * z; }

	//	Normalize
	float Normalize()
	{
		float fMag = Magnitude();
		if (fMag < 1e-6	&& fMag > -1e-6)
		{
			Clear();
			fMag = 0.0f;
		}
		else
		{
			NEON_STATISTICS_TICK
			float f = 1.0f / fMag;
#ifdef ARM_NEON
            ne10_mulc_float((ne10_float32_t*)m, (ne10_float32_t*)m, f, 3);
#else
			x = x * f;
			y = y * f;
			z = z * f;
#endif
		}

		return fMag;
	}

	//	Get normalize
	friend float Normalize(const AXVector3& vIn, AXVector3& vOut)
	{
		float fMag = vIn.Magnitude();
		if (fMag < 1e-6	&& fMag > -1e-6)
		{
			vOut.Clear();
			fMag = 0.0f;
		}
		else
		{
			float f = 1.0f / fMag;
			vOut = vIn * f;
		}

		return fMag;
	}

	// LERP method.
	friend AXVector3 LERPVec(const AXVector3& v1, const AXVector3& v2, float fraction)
	{
		return v1 * (1.0f - fraction) + v2 * fraction;
	}

	//	Get minimum number
	float MinMember() const
	{ 
		if (x < y)
			return x < z ? x : z;
		else
			return y < z ? y : z;
	}

	//	Get maximum member
	float MaxMember() const
	{ 
		if (x > y)
			return x > z ? x : z;
		else
			return y > z ? y : z;
	}
	
	//	Snap vector
	void Snap()
	{
		for (int i=0; i < 3; i++)
		{
			if (m[i] > 1.0f - 1e-5f)
			{
				Clear();
				m[i] = 1.0f;
				break;
			}
			else if (m[i] < -1.0f + 1e-5f)
			{
				Clear();
				m[i] = -1.0f;
				break;
			}
		}
	}

protected:	//	Attributes

protected:	//	Operations
	
};

inline float DotProduct(const AXVector3& v1, const AXVector3& v2)
{
	NEON_STATISTICS_TICK
	float fRet;
#ifdef ARM_NEON
    ne10_dot_vec3f(&fRet, (ne10_vec3f_t*)v1.m, (ne10_vec3f_t*)v2.m, 1);
#else
    fRet = v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
#endif

	return fRet;
}

///////////////////////////////////////////////////////////////////////////
//
//	class AXVector4
//
///////////////////////////////////////////////////////////////////////////

class AXVector4 //: public AMemBase
{
public:		//	Attributes

	union
	{
		struct
		{
			float x, y, z, w;
		};

		float m[4];
	};

public:		//	Constructors and Destructors

	AXVector4()
    {
#ifdef ARM_NEON
        InitNeon();
#endif
    }
	AXVector4(float _x, float _y, float _z, float _w)
    {
#ifdef ARM_NEON
        InitNeon();
#endif
        x=_x; y=_y; z=_z; w=_w;
    }
	explicit AXVector4(float f)
    {
		NEON_STATISTICS_TICK
#ifdef ARM_NEON
		InitNeon();
        ne10_setc_float((ne10_float32_t*)m, f, 4);
#else
        x = y = z = w = f;
#endif
    }
    
	AXVector4(const float f[4])
    {
		NEON_STATISTICS_TICK
#ifdef ARM_NEON
		InitNeon();
        ne10_setc_vec4f((ne10_vec4f_t*)m, (const ne10_vec4f_t*)f, 1);
#else
        x=f[0]; y=f[1]; z=f[2]; w=f[3];
#endif
    }
    
	AXVector4(const AXVector4& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}
	AXVector4(const AXVector3& v) : x(v.x), y(v.y), z(v.z), w(1.0f) {}

public:		//	Operations

	//	= operator
	const AXVector4& operator = (const AXVector4& v)
    {
		NEON_STATISTICS_TICK
#ifdef ARM_NEON
        ne10_setc_vec4f((ne10_vec4f_t*)m, (const ne10_vec4f_t*)v.m, 1);
#else
        x = v.x; y = v.y; z = v.z; w = v.w;
#endif
        return *this;
    }
    
	const AXVector4& operator = (const AXVector3& v)
    {
		NEON_STATISTICS_TICK
#ifdef ARM_NEON
        ne10_setc_vec3f((ne10_vec3f_t*)m, (const ne10_vec3f_t*)v.m, 1);
        w = 1.0f;
#else
        x = v.x; y = v.y; z = v.z; w = 1.0f;
#endif
        return *this;
    }

	void Set(float _x, float _y, float _z, float _w) { x = _x; y = _y; z = _z; w = _w; }
    
	void Clear()
    {
		NEON_STATISTICS_TICK
#ifdef ARM_NEON
        ne10_setc_float((ne10_float32_t*)m, 0.0f, 4);
#else
        x = y = z = w = 0.0f;
#endif
    }
    
	bool IsEquals(const AXVector4 & v, afloat32 epsilon) const
	{
		return a_Equals(x, v.x, epsilon) && a_Equals(y, v.y, epsilon) && a_Equals(z, v.z, epsilon) && a_Equals(w, v.w, epsilon);
	}

	const AXVector4& operator += (const AXVector4& v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
	const AXVector4& operator *= (afloat32 f) { x *= f; y *= f; z *= f; w *= f; return *this; }

	friend AXVector4 operator * (const AXVector4& v, afloat32 f) { return AXVector4(v.x * f, v.y * f, v.z * f, v.w * f); }

	friend AXVector4 operator * (afloat32 f, const AXVector4& v) { return v * f; }

	friend float DotProduct(const AXVector4& v1, const AXVector4& v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w; }
};

///////////////////////////////////////////////////////////////////////////
//
//	class AXVector2
//
///////////////////////////////////////////////////////////////////////////

class AXVector2 //: public AMemBase
{
public:		//	Types

public:		//	Constructions and Destructions

	AXVector2()
    {
#ifdef ARM_NEON
        InitNeon();
#endif
    }
    
	explicit AXVector2(float m) : x(m), y(m)
    {
#ifdef ARM_NEON
        InitNeon();
#endif
    }
	AXVector2(float x, float y) : x(x), y(y)
    {
#ifdef ARM_NEON
        InitNeon();
#endif
    }
	AXVector2(const AXVector2& v) : x(v.x), y(v.y) {}

public:		//	Attributes

	union
	{
		struct
		{
			float x, y;
		};

		float m[2];
	};

public:		//	Operaitons

	//	+ operator
	friend AXVector2 operator + (const AXVector2& v1, const AXVector2& v2)
    {
		NEON_STATISTICS_TICK
#ifdef ARM_NEON
		AXVector2 vDst;
        ne10_add_float((ne10_float32_t*)vDst.m, (ne10_float32_t*)v1.m, (ne10_float32_t*)v2.m, 2);
		return vDst;
#else
        return AXVector2(v1.x+v2.x, v1.y+v2.y);
#endif
    }
	//	- operator
	friend AXVector2 operator - (const AXVector2& v1, const AXVector2& v2)
    {
		NEON_STATISTICS_TICK
#ifdef ARM_NEON
		AXVector2 vDst;
        ne10_sub_float((ne10_float32_t*)vDst.m, (ne10_float32_t*)v1.m, (ne10_float32_t*)v2.m, 2);
		return vDst;
#else
        return AXVector2(v1.x-v2.x, v1.y-v2.y);
#endif
    }

	//	* operator
	friend AXVector2 operator * (const AXVector2& v, float f)
    {
		NEON_STATISTICS_TICK
#ifdef ARM_NEON
		AXVector2 vDst;
        ne10_mulc_float((ne10_float32_t*)vDst.m, (ne10_float32_t*)v.m, f, 2);
		return vDst;
#else
        return AXVector2(v.x*f, v.y*f);
#endif
    }
    
	friend AXVector2 operator * (float f, const AXVector2& v)
    {
		NEON_STATISTICS_TICK
#ifdef ARM_NEON
		AXVector2 vDst;
        ne10_mulc_float((ne10_float32_t*)vDst.m, (ne10_float32_t*)v.m, f, 2);
		return vDst;
#else
        return AXVector2(v.x*f, v.y*f);
#endif
    }
    
    
	friend AXVector2 operator * (const AXVector2& v, int f) { return v * (float)f; }
    
	friend AXVector2 operator * (int f, const AXVector2& v) { return v * (float)f; }
    
	friend AXVector2 operator * (const AXVector2& v1, const AXVector2& v2)
    {
		NEON_STATISTICS_TICK
#ifdef ARM_NEON
		AXVector2 vDst;
        ne10_mul_float((ne10_float32_t*)vDst.m, (ne10_float32_t*)v1.m, (ne10_float32_t*)v2.m, 2);
		return vDst;
#else
        return AXVector2(v1.x*v2.x, v1.y*v2.y);
#endif
    }

	//	/ operator
	friend AXVector2 operator / (const AXVector2& v, float f)
    {
		NEON_STATISTICS_TICK
        f = 1.0f / f;
#ifdef ARM_NEON
		AXVector2 vDst;
        ne10_mulc_float((ne10_float32_t*)vDst.m, (ne10_float32_t*)v.m, f, 2);
		return vDst;
#else
		return AXVector2(v.x*f, v.y*f);
#endif
    }
    
	friend AXVector2 operator / (const AXVector2& v, int f) { return v / (float)f; }
	friend AXVector2 operator / (const AXVector2& v1, const AXVector2& v2)
    {
#ifdef ARM_NEON
		AXVector2 vDst;
        ne10_div_float((ne10_float32_t*)vDst.m, (ne10_float32_t*)v1.m, (ne10_float32_t*)v2.m, 2);
		return vDst;
#else
        return AXVector2(v1.x/v2.x, v1.y/v2.y);
#endif
    }

	//	Dot product
	friend float DotProduct(const AXVector2& v1, const AXVector2& v2);

	//	== operator
	friend bool operator == (const AXVector2& v1, const AXVector2& v2) { return v1.x==v2.x && v1.y==v2.y; }
	//	!= operator
	friend bool operator != (const AXVector2& v1, const AXVector2& v2) { return v1.x!=v2.x || v1.y!=v2.y; }

	//	+= operator
	const AXVector2& operator += (const AXVector2& v)
    {
		NEON_STATISTICS_TICK
#ifdef ARM_NEON
        ne10_add_float((ne10_float32_t*)m, (ne10_float32_t*)m, (ne10_float32_t*)v.m, 2);
#else
        x += v.x; y += v.y;
#endif
        return *this;
    }
    
	//	-= operator
	const AXVector2& operator -= (const AXVector2& v)
    {
		NEON_STATISTICS_TICK
#ifdef ARM_NEON
        ne10_sub_float((ne10_float32_t*)m, (ne10_float32_t*)m, (ne10_float32_t*)v.m, 2);
#else
        x -= v.x; y -= v.y;
#endif
        return *this;
    }
    
	//	*= operator
	const AXVector2& operator *= (float f)
    {
		NEON_STATISTICS_TICK
#ifdef ARM_NEON
        ne10_mulc_float((ne10_float32_t*)m, (ne10_float32_t*)m, f, 2);
#else
        x *= f; y *= f;
#endif
        return *this;
    }
    
	//	/= operator
	const AXVector2& operator /= (float f)
    {
		NEON_STATISTICS_TICK
        f = 1.0f / f;
#ifdef ARM_NEON
        ne10_mulc_float((ne10_float32_t*)m, (ne10_float32_t*)m, f, 2);
#else
        x *= f; y *= f;
#endif
        return *this;
    }
    
	//	= operator
	const AXVector2& operator = (const AXVector2& v)
    {
		NEON_STATISTICS_TICK
#ifdef ARM_NEON
        ne10_setc_vec2f((ne10_vec2f_t*)m, (const ne10_vec2f_t*)v.m, 1);
#else
        x = v.x; y = v.y;
#endif
        return *this;
    }

	AXVector2 operator + () const { return *this; }
	AXVector2 operator - () const { return AXVector2(-x,-y); }

	void Set(float _x, float _y) { x = _x; y = _y; }
    
	void Clear()
    {
		NEON_STATISTICS_TICK
#ifdef ARM_NEON
        ne10_setc_float((ne10_float32_t*)m, 0.0f, 2);
#else
        x = y = 0.0f;
#endif
    }
    
	bool IsZero() const { return x == 0.0f && y == 0.0f; }
	bool IsEquals(const AXVector2 & v, afloat32 epsilon) const
	{
		return a_Equals(x, v.x, epsilon) && a_Equals(y, v.y, epsilon);
	}

	//	Magnitude
	float Magnitude() const
    {
		NEON_STATISTICS_TICK
		float fRet;
#ifdef ARM_NEON       
        ne10_dot_vec2f(&fRet, (ne10_vec2f_t*)m, (ne10_vec2f_t*)m, 1);
        fRet = (float)(sqrt(fRet));
#else
        fRet = (float)(sqrt(x * x + y * y));
#endif
		return fRet;
    }
    
	float SquaredMagnitude() const
    {
		NEON_STATISTICS_TICK
		float fRet;
#ifdef ARM_NEON
        ne10_dot_vec2f(&fRet, (ne10_vec2f_t*)m, (ne10_vec2f_t*)m, 1);
#else
        fRet = x * x + y * y;
#endif
		return fRet;
    }

	//	Normalize
	float Normalize()
	{
		float fMag = Magnitude();
		if (fMag < 1e-6	&& fMag > -1e-6)
		{
			Clear();
			fMag = 0.0f;
		}
		else
		{
			NEON_STATISTICS_TICK
			float f = 1.0f / fMag;
#ifdef ARM_NEON
            ne10_mulc_float((ne10_float32_t*)m, (ne10_float32_t*)m, f, 2);
#else
			x = x * f;
			y = y * f;
#endif
		}

		return fMag;
	}

	//	Get normalize
	friend float Normalize(const AXVector2& vIn, AXVector2& vOut)
	{
		float fMag = vIn.Magnitude();
		if (fMag < 1e-6	&& fMag > -1e-6)
		{
			vOut.Clear();
			fMag = 0.0f;
		}
		else
		{
			float f = 1.0f / fMag;
			vOut = vIn * f;
		}

		return fMag;
	}

	// LERP method.
	friend AXVector2 LERPVec(const AXVector2& v1, const AXVector2& v2, float fraction)
	{
		return v1 * (1.0f - fraction) + v2 * fraction;
	}

	//	Get minimum number
	float MinMember() const
	{ 
		if (x < y)
			return x;
		else
			return y;
	}

	//	Get maximum member
	float MaxMember() const
	{ 
		if (x > y)
			return x;
		else
			return y;
	}

	//	Snap vector
	void Snap()
	{
		for (int i=0; i < 2; i++)
		{
			if (m[i] > 1.0f - 1e-5f)
			{
				Clear();
				m[i] = 1.0f;
				break;
			}
			else if (m[i] < -1.0f + 1e-5f)
			{
				Clear();
				m[i] = -1.0f;
				break;
			}
		}
	}

protected:	//	Attributes

protected:	//	Operations

};

inline float DotProduct(const AXVector2& v1, const AXVector2& v2)
{
	NEON_STATISTICS_TICK
	float fRet;
#ifdef ARM_NEON
    ne10_dot_vec2f(&fRet, (ne10_vec2f_t*)v1.m, (ne10_vec2f_t*)v2.m, 1);
#else
    fRet = v1.x*v2.x + v1.y*v2.y;
#endif
	return fRet;
}

// in case ambiguous call, treat as vector3
inline float DotProduct(float v1, float v2) { return v1 * v2; }





///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_AXVECTOR_H_
