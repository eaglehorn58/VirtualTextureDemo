//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#ifndef _AXMATRIX_H_
#define _AXMATRIX_H_

#include "AXVector.h"
//#include "ASys.h"
//#include "AMemBase.h"
#ifdef ARM_NEON
#include "NE10.h"
#endif

///////////////////////////////////////////////////////////////////////////

class AXMatrix3 //: public AMemBase
{
public:		//	Types

	//	Construct flag
	enum CONSTRUCT
	{
		IDENTITY = 0,	//	Construct a identity matrix
		CLEARED,		//	Construct a cleared matrix
	};

public:		//	Constructions and Destructions

	AXMatrix3()
    {
#ifdef ARM_NEON
        InitNeon();
#endif
    }

	AXMatrix3(CONSTRUCT c)
	{
		NEON_STATISTICS_TICK
#ifdef ARM_NEON        
        InitNeon();
        if (c == IDENTITY)
        {
            ne10_identitymat_3x3f((ne10_mat3x3f_t*)m, 1);
        }
        else if (c == CLEARED)
        {
            ne10_setc_float((ne10_float32_t*)m, 0.0f, 9);
        }
#else
		if (c == IDENTITY)
		{
			_12 = _13 = 0.0f;
			_21 = _23 = 0.0f;
			_31 = _32 = 0.0f;
			_11 = _22 = _33 = 1.0f;
		}
		else if (c == CLEARED)
		{
			_11 = _12 = _13 = 0.0f;
			_21 = _22 = _23 = 0.0f;
			_31 = _32 = _33 = 0.0f;
		}
#endif
	}
	
	AXMatrix3(const float arr[3][3])
	{
#ifdef ARM_NEON
		InitNeon();
#endif
		memcpy(m,arr,9*sizeof(float));
	}

	AXMatrix3(const AXMatrix3& rkMatrix)
	{
		memcpy( m, rkMatrix.m, 9 * sizeof(float));
	}

	AXMatrix3(float fEntry00, float fEntry01, float fEntry02,
			   float fEntry10, float fEntry11, float fEntry12,
			   float fEntry20, float fEntry21, float fEntry22)
	{
#ifdef ARM_NEON          
        InitNeon();
#endif
		m[0][0] = fEntry00;
		m[0][1] = fEntry01;
		m[0][2] = fEntry02;
		m[1][0] = fEntry10;
		m[1][1] = fEntry11;
		m[1][2] = fEntry12;
		m[2][0] = fEntry20;
		m[2][1] = fEntry21;
		m[2][2] = fEntry22;
	}

public:		//	Attributes

	union
	{
		struct
		{
			float _11, _12, _13;
			float _21, _22, _23;
			float _31, _32, _33;
		};

		float m[3][3];
	};

public:		//	Operations

	AXVector3 GetRow(int i) const { return AXVector3(m[i][0], m[i][1], m[i][2]); }
	AXVector3 GetCol(int i) const { return AXVector3(m[0][i], m[1][i], m[2][i]); }

	//	* operator
	friend AXVector3 operator * (const AXVector3& v, const AXMatrix3& mat);
	friend AXVector3 operator * (const AXMatrix3& mat, const AXVector3& v);
	friend AXMatrix3 operator * (const AXMatrix3& mat1, const AXMatrix3& mat2);

	//	*= operator
	AXMatrix3& operator *= (const AXMatrix3& mat)
	{
		*this = *this * mat;
		return *this;
	}

	void Transpose() 
	{ 
		float t; 
		t = m[0][1]; m[0][1] = m[1][0]; m[1][0] = t; 
		t = m[0][2]; m[0][2] = m[2][0];	m[2][0] = t;
		t = m[1][2]; m[1][2] = m[2][1]; m[2][1] = t;
	}

	//	Clear all elements to zero
	void Clear();
	//	Set matrix to identity matrix
	void Identity();

	//	Build matrix to be translation and rotation matrix
	void Translate(float x, float y);
	void Rotate(float fRad);

    void InverseTM();

};

///////////////////////////////////////////////////////////////////////////
//
//	class AXMatrix4
//
///////////////////////////////////////////////////////////////////////////

class AXMatrix4 //: public AMemBase
{
public:		//	Types

	//	Construct flag
	enum CONSTRUCT
	{
		IDENTITY = 0,	//	Construct a identity matrix
	};

public:		//	Constructions and Destructions

	AXMatrix4()
    {
#ifdef ARM_NEON
        InitNeon();
#endif
    }

	AXMatrix4(float* v)
	{
#ifdef ARM_NEON
        InitNeon();
#endif
		for (int i=0; i < 4; i++)
		{
			for (int j=0; j < 4; j++)
				m[i][j] = v[i * 4 + j];
		}
	}

	AXMatrix4(const AXMatrix4& mat)
	{
		for (int i=0; i < 4; i++)
		{
			for (int j=0; j < 4; j++)
				m[i][j] = mat.m[i][j];
		}
	}
	
	AXMatrix4(CONSTRUCT c)
	{
		NEON_STATISTICS_TICK
#ifdef ARM_NEON        
        InitNeon();
        ne10_identitymat_4x4f((ne10_mat4x4f_t*)m, 1);
#else
		_12 = _13 = _14 = 0.0f;
		_21 = _23 = _24 = 0.0f;
		_31 = _32 = _34 = 0.0f;
		_41 = _42 = _43 = 0.0f;
		_11 = _22 = _33 = _44 = 1.0f;
#endif
	}
	
	AXMatrix4(	float fEntry00, float fEntry01, float fEntry02, float fEntry03,
				float fEntry10, float fEntry11, float fEntry12, float fEntry13,
				float fEntry20, float fEntry21, float fEntry22, float fEntry23,
				float fEntry30, float fEntry31, float fEntry32, float fEntry33)
	{
#ifdef ARM_NEON
        InitNeon();
#endif
		m[0][0] = fEntry00;
		m[0][1] = fEntry01;
		m[0][2] = fEntry02;
		m[0][3] = fEntry03;
		
		m[1][0] = fEntry10;
		m[1][1] = fEntry11;
		m[1][2] = fEntry12;
		m[1][3] = fEntry13;
		
		m[2][0] = fEntry20;
		m[2][1] = fEntry21;
		m[2][2] = fEntry22;
		m[2][3] = fEntry23;
		
		m[3][0] = fEntry30;
		m[3][1] = fEntry31;
		m[3][2] = fEntry32;
		m[3][3] = fEntry33;
	}

public:		//	Attributes

	union
	{
		struct
		{
			float	_11, _12, _13, _14;
			float	_21, _22, _23, _24;
			float	_31, _32, _33, _34;
			float	_41, _42, _43, _44;
		};

		float m[4][4];
    };

public:		//	Operaitons

	//	Get row and column
	AXVector3 GetRow(int i) const { return AXVector3(m[i][0], m[i][1], m[i][2]); }
	AXVector3 GetCol(int i) const { return AXVector3(m[0][i], m[1][i], m[2][i]); }
	//	Set row and column
	void SetRow(int i, const AXVector3& v) { m[i][0]=v.x; m[i][1]=v.y; m[i][2]=v.z; }
	void SetCol(int i, const AXVector3& v) { m[0][i]=v.x; m[1][i]=v.y; m[2][i]=v.z; }

	//	* operator
	// 特别注意 vector3 * matrx4 在以前Angelica系列引擎中被当作D3DXVec3TransformCoord 处理
	// 现在当作向量和4维矩阵相乘
	friend AXVector3 operator * (const AXVector3& v, const AXMatrix4& mat);
	friend AXVector3 operator * (const AXMatrix4& mat, const AXVector3& v);

	friend AXVector4 operator * (const AXVector4& v, const AXMatrix4& mat);
	friend AXVector4 operator * (const AXMatrix4& mat, const AXVector4& v);
	friend AXMatrix4 operator * (const AXMatrix4& mat1, const AXMatrix4& mat2);

	//	Scale matrix
	friend AXMatrix4 operator * (const AXMatrix4& mat, float s);
	friend AXMatrix4 operator * (float s, const AXMatrix4& mat) { return mat * s; }
	friend AXMatrix4 operator / (const AXMatrix4& mat, float s) { return mat * (1.0f / s); }

	//	*= operator
	AXMatrix4& operator *= (const AXMatrix4& mat)
	{
		*this = *this * mat;
		return *this;
	}
	AXMatrix4& operator *= (float s);
	//	/= operator
	AXMatrix4& operator /= (float s) { return *this *= (1.0f / s); }
	
	//	== operator
	friend bool operator == (const AXMatrix4& mat1, const AXMatrix4& mat2)
	{
		for (int i=0; i < 4; i++)
		{
			for (int j=0; j < 4; j++)
			{
				if (mat1.m[i][j] != mat2.m[i][j])
					return false;
			}
		}
		return true;
	}

	//	!= operator
	friend bool operator != (const AXMatrix4& mat1, const AXMatrix4& mat2)
	{ 
		for (int i=0; i < 4; i++)
		{
			for (int j=0; j < 4; j++)
			{
				if (mat1.m[i][j] != mat2.m[i][j])
					return true;
			}
		}
		return false;
	}

	//	+ operator
	friend AXMatrix4 operator + (const AXMatrix4& mat1, const AXMatrix4& mat2);
	//	- operator
	friend AXMatrix4 operator - (const AXMatrix4& mat1, const AXMatrix4& mat2);
	//	+= operator
	AXMatrix4& operator += (const AXMatrix4& mat);
	//	-= operator
	AXMatrix4& operator -= (const AXMatrix4& mat);

	//	Clear all elements to zero
	void Clear();
	//	Set matrix to identity matrix
	void Identity();
	//	Transpose matrix
	void Transpose();
	//	Get transpose matrix of this matrix
	AXMatrix4 GetTranspose() const;
	//	Inverse matrix
	void Inverse() { *this = GetInverse(); }
	//	Get inverse matrix of this matrix
	AXMatrix4 GetInverse() const;
	//	Inverse matrix (used only by transform matrix)
	void InverseTM() { *this = GetInverseTM(); }
	//	Get inverse matrix (used only by transform matrix)
	AXMatrix4 GetInverseTM() const;
	//	Get determinant of this matrix
	float Determinant() const;

	//	Build matrix to be translation and rotation matrix
	void Translate(float x, float y, float z);
	void RotateX(float fRad);
	void RotateY(float fRad);
	void RotateZ(float fRad);
	void RotateAxis(const AXVector3& v, float fRad);
	void RotateAxis(const AXVector3& vPos, const AXVector3& vAxis, float fRad);
	void Scale(float sx, float sy, float sz);

    //	Set/Get rotation part of transform matrix
    void SetRotatePart(const AXMatrix3& mat);
    AXMatrix3 GetRotatePart() const;
    //	Set/Get translate part of transform matrix
    void SetTransPart(const AXVector3& vTrans) { SetRow(3, vTrans); }
    AXVector3 GetTransPart() const { return GetRow(3); };
	AXVector3 TransformCoord(const AXVector3& v) const;
	void TransformCoord(const AXVector3& inV, AXVector3& outV) const;

protected:	//	Attributes

	//	Calcualte determinant of a 3x3 matrix
	float Det3x3(float a11, float a12, float a13, float a21, float a22, float a23,
				 float a31, float a32, float a33) const
	{
		return a11 * a22 * a33 + a21 * a32 * a13 + a31 * a12 * a23 -
			   a13 * a22 * a31 - a23 * a32 * a11 - a33 * a12 * a21;
	}

protected:	//	Operations
	
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////

//	Clear all elements to zero
A_FORCEINLINE void AXMatrix4::Clear()
{
	NEON_STATISTICS_TICK
#ifdef ARM_NEON
		ne10_setc_float((ne10_float32_t*)m, 0.0f, 16);
#else
		_11 = _12 = _13 = _14 = 0.0f;
	_21 = _22 = _23 = _24 = 0.0f;
	_31 = _32 = _33 = _34 = 0.0f;
	_41 = _42 = _43 = _44 = 0.0f;
#endif
}

//	Set matrix to identity matrix
A_FORCEINLINE void AXMatrix4::Identity()
{
	NEON_STATISTICS_TICK
#ifdef ARM_NEON
		ne10_identitymat_4x4f((ne10_mat4x4f_t*)m, 1);
#else
		_12 = _13 = _14 = 0.0f;
	_21 = _23 = _24 = 0.0f;
	_31 = _32 = _34 = 0.0f;
	_41 = _42 = _43 = 0.0f;
	_11 = _22 = _33 = _44 = 1.0f;
#endif
}

A_FORCEINLINE AXMatrix4 operator * (const AXMatrix4& mat1, const AXMatrix4& mat2)
{
	NEON_STATISTICS_TICK
	AXMatrix4 matRet;
	matRet.Clear();
#ifdef ARM_NEON
	ne10_mulmat_4x4f((ne10_mat4x4f_t*)matRet.m, (ne10_mat4x4f_t*)mat2.m, (ne10_mat4x4f_t*)mat1.m, 1);
#else
	for (int i=0; i < 4; i++)
	{
		for (int j=0; j < 4; j++)
		{
			for (int k=0; k < 4; k++)
				matRet.m[i][j] += mat1.m[i][k] * mat2.m[k][j];
		}
	}
#endif
	//	D3DXMatrixMultiply((D3DXMATRIX*)&matRet, (const D3DXMATRIX*)&mat1, (const D3DXMATRIX*)&mat2);

	return matRet;
}

#endif	//	_AXMATRIX_H_
