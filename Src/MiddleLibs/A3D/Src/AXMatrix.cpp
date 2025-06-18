//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com

#include "AXMatrix.h"

///////////////////////////////////////////////////////////////////////////

//	Clear all elements to zero
void AXMatrix3::Clear()
{
	NEON_STATISTICS_TICK
#ifdef ARM_NEON
    ne10_setc_float((ne10_float32_t*)m, 0.0f, 9);
#else
	_11 = _12 = _13 = 0.0f;
	_21 = _22 = _23 = 0.0f;
	_31 = _32 = _33 = 0.0f;
#endif
}

//	Set matrix to identity matrix
void AXMatrix3::Identity()
{
	NEON_STATISTICS_TICK
#ifdef ARM_NEON
    ne10_identitymat_3x3f((ne10_mat3x3f_t*)m, 1);
#else
	_12 = _13 = 0.0f;
	_21 = _23 = 0.0f;
	_31 = _32 = 0.0f;
	_11 = _22 = _33 = 1.0f;
#endif
}

//	* operator
AXVector3 operator * (const AXVector3& v, const AXMatrix3& mat)
{
	NEON_STATISTICS_TICK
#ifdef ARM_NEON
	AXVector3 vDst;
	ne10_mulcmatvec_cm3x3f_v3f((ne10_vec3f_t*)vDst.m, (const ne10_mat3x3f_t*)mat.m, (ne10_vec3f_t*)v.m, 1);
	return vDst;
#else
	return AXVector3(v.x * mat._11 + v.y * mat._21 + v.z * mat._31,
					  v.x * mat._12 + v.y * mat._22 + v.z * mat._32,
					  v.x * mat._13 + v.y * mat._23 + v.z * mat._33);    
#endif
}

AXVector3 operator * (const AXMatrix3& mat, const AXVector3& v)
{
	return AXVector3(v.x * mat._11 + v.y * mat._12 + v.z * mat._13,
					  v.x * mat._21 + v.y * mat._22 + v.z * mat._23,
					  v.x * mat._31 + v.y * mat._32 + v.z * mat._33);

}

AXMatrix3 operator * (const AXMatrix3& mat1, const AXMatrix3& mat2)
{
	NEON_STATISTICS_TICK
	AXMatrix3 matRet(AXMatrix3::CLEARED);
#ifdef ARM_NEON
    ne10_mulmat_3x3f((ne10_mat3x3f_t*)matRet.m, (ne10_mat3x3f_t*)mat2.m, (ne10_mat3x3f_t*)mat1.m, 1);
#else
    
	for (int i=0; i < 3; i++)
	{
		for (int j=0; j < 3; j++)
		{
			for (int k=0; k < 3; k++)
				matRet.m[i][j] += mat1.m[i][k] * mat2.m[k][j];
		}
	}
#endif
	return matRet;
}


void AXMatrix3::Translate(float x, float y)
{
	Identity();
	_31 = x; 
	_32 = y;
}

void AXMatrix3::Rotate(float fRad)
{
	Identity();
	m[1][1] = m[0][0] = (float)cos(fRad);
	m[0][1] = (float)sin(fRad);
	m[1][0] = -m[0][1];
}

void AXMatrix3::InverseTM()
{
	NEON_STATISTICS_TICK
    float matInverse[3][3];
#ifdef ARM_NEON
    ne10_invmat_3x3f((ne10_mat3x3f_t*)matInverse, (ne10_mat3x3f_t*)m, 1);
    ne10_addc_float((ne10_float32_t*)m, (ne10_float32_t*)matInverse, 0.0f, 9);
#else

    matInverse[0][0] = m[1][1]*m[2][2] -
        m[1][2]*m[2][1];
    matInverse[0][1] = m[0][2]*m[2][1] -
        m[0][1]*m[2][2];
    matInverse[0][2] = m[0][1]*m[1][2] -
        m[0][2]*m[1][1];
    matInverse[1][0] = m[1][2]*m[2][0] -
        m[1][0]*m[2][2];
    matInverse[1][1] = m[0][0]*m[2][2] -
        m[0][2]*m[2][0];
    matInverse[1][2] = m[0][2]*m[1][0] -
        m[0][0]*m[1][2];
    matInverse[2][0] = m[1][0]*m[2][1] -
        m[1][1]*m[2][0];
    matInverse[2][1] = m[0][1]*m[2][0] -
        m[0][0]*m[2][1];
    matInverse[2][2] = m[0][0]*m[1][1] -
        m[0][1]*m[1][0];

    float fDet =
        m[0][0] * matInverse[0][0] +
        m[0][1] * matInverse[1][0] +
        m[0][2] * matInverse[2][0];

    float fInvDet = 1.0f / fDet;
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
            m[iRow][iCol] = matInverse[iRow][iCol] * fInvDet;
    }
#endif
}
///////////////////////////////////////////////////////////////////////////
//
//	Implement AXMatrix4
//
///////////////////////////////////////////////////////////////////////////

//	Vector3 * matrix4
AXVector3 operator * (const AXVector3& v, const AXMatrix4& mat)
{
	AXVector3 vRet(v.x * mat._11 + v.y * mat._21 + v.z * mat._31,
					v.x * mat._12 + v.y * mat._22 + v.z * mat._32,
					v.x * mat._13 + v.y * mat._23 + v.z * mat._33);
	return vRet;
}

//	Matrix4 * vector3
AXVector3 operator * (const AXMatrix4& mat, const AXVector3& v)
{
	AXVector3 vRet(v.x * mat._11 + v.y * mat._12 + v.z * mat._13,
					v.x * mat._21 + v.y * mat._22 + v.z * mat._23,
					v.x * mat._31 + v.y * mat._32 + v.z * mat._33);
	return vRet;
}

AXVector3 AXMatrix4::TransformCoord(const AXVector3& v) const
{
		AXVector3 vRet(v.x * _11 + v.y * _21 + v.z * _31 + _41,
						v.x * _12 + v.y * _22 + v.z * _32 + _42,
						v.x * _13 + v.y * _23 + v.z * _33 + _43);
		float w = v.x * _14 + v.y * _24 + v.z * _34 + _44;
		if (fabsf(w) < -1e-6)
		{
			return AXVector3(0.0f, 0.0f, 0.0f);
		}
		return vRet *= 1.0f / w;
}


void AXMatrix4::TransformCoord(const AXVector3& inV, AXVector3& outV) const
{
	outV.x = inV.x * _11 + inV.y * _21 + inV.z * _31 + _41;
	outV.y = inV.x * _12 + inV.y * _22 + inV.z * _32 + _42;
	outV.z = inV.x * _13 + inV.y * _23 + inV.z * _33 + _43;
	float w = inV.x * _14 + inV.y * _24 + inV.z * _34 + _44;
	if (fabsf(w) < -1e-6)
	{
		outV.x = 0.0f;
		outV.y = 0.0f;
		outV.z = 0.0f;
	}
	w = 1.0f / w;
	outV.x *= w;
	outV.y *= w;
	outV.z *= w;
}

//	Matrix4 * vector4
AXVector4 operator * (const AXVector4& v, const AXMatrix4& mat)
{
	NEON_STATISTICS_TICK
#ifdef ARM_NEON
	AXVector4 vDst;
	ne10_mulcmatvec_cm4x4f_v4f((ne10_vec4f_t*)vDst.m, (const ne10_mat4x4f_t*)mat.m, (ne10_vec4f_t*)v.m, 1);
	return vDst;
#else
	return AXVector4(v.x * mat._11 + v.y * mat._21 + v.z * mat._31 + v.w * mat._41,
					  v.x * mat._12 + v.y * mat._22 + v.z * mat._32 + v.w * mat._42,
					  v.x * mat._13 + v.y * mat._23 + v.z * mat._33 + v.w * mat._43,
					  v.x * mat._14 + v.y * mat._24 + v.z * mat._34 + v.w * mat._44);
#endif
}

AXVector4 operator * (const AXMatrix4& mat, const AXVector4& v)
{
	return AXVector4(v.x * mat._11 + v.y * mat._12 + v.z * mat._13 + v.w * mat._14,
					  v.x * mat._21 + v.y * mat._22 + v.z * mat._23 + v.w * mat._24,
					  v.x * mat._31 + v.y * mat._32 + v.z * mat._33 + v.w * mat._34,
					  v.x * mat._41 + v.y * mat._42 + v.z * mat._43 + v.w * mat._44);
}


//	Scale matrix4
AXMatrix4 operator * (const AXMatrix4& mat, float s)
{
	NEON_STATISTICS_TICK
	AXMatrix4 matRet;
#ifdef ARM_NEON
    ne10_mulc_float((ne10_float32_t*)matRet.m, (ne10_float32_t*)mat.m, s, 16);
#else
	for (int i=0; i < 4; i++)
	{
		for (int j=0; j < 4; j++)
			matRet.m[i][j] = mat.m[i][j] * s;
	}
#endif
	return matRet;
}

AXMatrix4& AXMatrix4::operator *= (float s)
{
	NEON_STATISTICS_TICK
#ifdef ARM_NEON
    ne10_mulc_float((ne10_float32_t*)m, (ne10_float32_t*)m, s, 16);
#else
	for (int i=0; i < 4; i++)
	{
		for (int j=0; j < 4; j++)
			m[i][j] *= s;
	}
#endif
	return *this;
}

AXMatrix4 operator + (const AXMatrix4& mat1, const AXMatrix4& mat2)
{
	NEON_STATISTICS_TICK
	AXMatrix4 matRet;
#ifdef ARM_NEON
    ne10_add_float((ne10_float32_t*)matRet.m, (ne10_float32_t*)mat1.m, (ne10_float32_t*)mat2.m, 16);
#else
    for (int i=0; i < 4; i++)
	{
		for (int j=0; j < 4; j++)
			matRet.m[i][j] = mat1.m[i][j] + mat2.m[i][j];
	}
#endif
	return matRet;
}

//	- operator
AXMatrix4 operator - (const AXMatrix4& mat1, const AXMatrix4& mat2)
{
	NEON_STATISTICS_TICK
	AXMatrix4 matRet;
#ifdef ARM_NEON
    ne10_sub_float((ne10_float32_t*)matRet.m, (ne10_float32_t*)mat1.m, (ne10_float32_t*)mat2.m, 16);
#else
	for (int i=0; i < 4; i++)
	{
		for (int j=0; j < 4; j++)
			matRet.m[i][j] = mat1.m[i][j] - mat2.m[i][j];
	}
#endif
	return matRet;
}

//	+= operator
AXMatrix4& AXMatrix4::operator += (const AXMatrix4& mat)
{
	NEON_STATISTICS_TICK
#ifdef ARM_NEON
    ne10_add_float((ne10_float32_t*)m, (ne10_float32_t*)m, (ne10_float32_t*)mat.m, 16);
#else
	for (int i=0; i < 4; i++)
	{
		for (int j=0; j < 4; j++)
			m[i][j] += mat.m[i][j];
	}
#endif
	return *this;
}

//	-= operator
AXMatrix4& AXMatrix4::operator -= (const AXMatrix4& mat)
{
	NEON_STATISTICS_TICK
#ifdef ARM_NEON
    ne10_sub_float((ne10_float32_t*)m, (ne10_float32_t*)m, (ne10_float32_t*)mat.m, 16);
#else
	for (int i=0; i < 4; i++)
	{
		for (int j=0; j < 4; j++)
			m[i][j] -= mat.m[i][j];
	}
#endif
	return *this;
}


//	Transpose matrix
void AXMatrix4::Transpose()
{
	float t;
	t = _12; _12 = _21; _21 = t;
	t = _13; _13 = _31; _31 = t;
	t = _14; _14 = _41; _41 = t;
	t = _23; _23 = _32; _32 = t;
	t = _24; _24 = _42; _42 = t;
	t = _34; _34 = _43; _43 = t;
}

//	Get transpose matrix of this matrix
AXMatrix4 AXMatrix4::GetTranspose() const
{
	NEON_STATISTICS_TICK
	AXMatrix4 matRet;
    
#ifdef ARM_NEON
    ne10_transmat_4x4f((ne10_mat4x4f_t*)matRet.m, (ne10_mat4x4f_t*)m, 1);
#else
	for (int i=0; i < 4; i++)
	{
		for (int j=0; j < 4; j++)
			matRet.m[i][j] = m[j][i];
	}
#endif

	return matRet;
}

//	Set translation matrix
void AXMatrix4::Translate(float x, float y, float z)
{
	Identity();
	_41 = x; 
	_42 = y;
	_43 = z;
}

//	fRad rotate radian
void AXMatrix4::RotateX(float fRad)
{
	Identity();
	m[2][2] = m[1][1] = (float)cos(fRad);
	m[1][2] = (float)sin(fRad);
	m[2][1] = -m[1][2];
}

//	fRad rotate radian
void AXMatrix4::RotateY(float fRad)
{
	Identity();
	m[2][2] = m[0][0] = (float)cos(fRad);
	m[2][0] = (float)sin(fRad);
	m[0][2] = -m[2][0];
}

//	fRad rotate radian
void AXMatrix4::RotateZ(float fRad)
{
	Identity();
	m[1][1] = m[0][0] = (float)cos(fRad);
	m[0][1] = (float)sin(fRad);
	m[1][0] = -m[0][1];
}

/*	fRad rotate radian

	v: normalized axis
	fRad: rotation radian
*/
void AXMatrix4::RotateAxis(const AXVector3& v, float fRad)
{
	float fCos = (float)cos(fRad);
	float fSin = (float)sin(fRad);

	_11 = (v.m[0] * v.m[0]) * (1.0f - fCos) + fCos;
	_21 = (v.m[0] * v.m[1]) * (1.0f - fCos) - (v.m[2] * fSin);
	_31 = (v.m[0] * v.m[2]) * (1.0f - fCos) + (v.m[1] * fSin);

	_12 = (v.m[1] * v.m[0]) * (1.0f - fCos) + (v.m[2] * fSin);
	_22 = (v.m[1] * v.m[1]) * (1.0f - fCos) + fCos;
	_32	= (v.m[1] * v.m[2]) * (1.0f - fCos) - (v.m[0] * fSin);

	_13	= (v.m[2] * v.m[0]) * (1.0f - fCos) - (v.m[1] * fSin);
	_23	= (v.m[2] * v.m[1]) * (1.0f - fCos) + (v.m[0] * fSin);
	_33 = (v.m[2] * v.m[2]) * (1.0f - fCos) + fCos;

	_14 = _24 = _34 = 0.0f;
	_41 = _42 = _43 = 0.0f;
	_44 = 1.0f;

//	D3DXMatrixRotationAxis((D3DXMATRIX*)this, (D3DXVECTOR3*)&v, fRad);
}

/*	Rotate camera round an axis

	vPos: start position of rotate axis
	vAxis: rotate axis
	fRad: rotate radian
*/
void AXMatrix4::RotateAxis(const AXVector3& vPos, const AXVector3& vAxis, float fRad)
{
	Translate(-vPos.x, -vPos.y, -vPos.z);

	AXMatrix4 mat;
	mat.RotateAxis(vAxis, fRad);
	*this *= mat;
	mat.Translate(vPos.x, vPos.y, vPos.z);
	*this *= mat;
}

//	Get determinant of this matrix
float AXMatrix4::Determinant() const
{
	NEON_STATISTICS_TICK
	float fDet;
    
#ifdef ARM_NEON
    ne10_detmat_4x4f((ne10_float32_t*)&fDet, (ne10_mat4x4f_t*)m, 1);
#else
	//fDet  = _11 * _22 * _33 * _44;
	//fDet += _21 * _32 * _43 * _14;
	//fDet += _31 * _42 * _13 * _24;
	//fDet += _41 * _12 * _23 * _34;
	//fDet -= _41 * _32 * _23 * _14;
	//fDet -= _42 * _33 * _24 * _11;
	//fDet -= _43 * _34 * _21 * _12;
	//fDet -= _44 * _31 * _22 * _13;
	//	按照第一行展开
	fDet =	_11 * Det3x3(_22, _23, _24, _32, _33, _34, _42, _43, _44) -
			_12 * Det3x3(_21, _23, _24, _31, _33, _34, _41, _43, _44) +
			_13 * Det3x3(_21, _22, _24, _31, _32, _34, _41, _42, _44) -
			_14 * Det3x3(_21, _22, _23, _31, _32, _33, _41, _42, _43);
#endif
    
	return fDet;
}

//	Get inverse matrix of this matrix
AXMatrix4 AXMatrix4::GetInverse() const
{
	//2.2中注释掉的实现
	/*
	float fDet = 1.0f / Determinant();

	AXMatrix4 mat;
	
    mat._11 = fDet * Det3x3(_22, _23, _24, _32, _33, _34, _42, _43, _44);
    mat._12 = fDet * Det3x3(_12, _13, _14, _32, _33, _34, _42, _43, _44);
    mat._13 = fDet * Det3x3(_12, _13, _14, _22, _23, _24, _42, _43, _44);
    mat._14 = fDet * Det3x3(_12, _13, _14, _22, _23, _24, _32, _33, _34);

    mat._21 = fDet * Det3x3(_21, _23, _24, _31, _33, _34, _41, _43, _44);
    mat._22 = fDet * Det3x3(_11, _13, _14, _31, _33, _34, _41, _43, _44);
    mat._23 = fDet * Det3x3(_11, _13, _14, _21, _23, _24, _41, _43, _44);
    mat._24 = fDet * Det3x3(_11, _13, _14, _21, _23, _24, _31, _33, _34);

    mat._31 = fDet * Det3x3(_21, _22, _24, _31, _32, _34, _41, _42, _44);
    mat._32 = fDet * Det3x3(_11, _12, _14, _31, _32, _34, _41, _42, _44);
    mat._33 = fDet * Det3x3(_11, _12, _14, _21, _22, _24, _41, _42, _44);
    mat._34 = fDet * Det3x3(_11, _12, _14, _21, _22, _24, _31, _32, _34);

	mat._41 = fDet * Det3x3(_21, _22, _23, _31, _32, _33, _41, _42, _43);
    mat._42 = fDet * Det3x3(_11, _12, _13, _31, _32, _33, _41, _42, _43);
    mat._43 = fDet * Det3x3(_11, _12, _13, _21, _22, _23, _41, _42, _43);
    mat._44 = fDet * Det3x3(_11, _12, _13, _21, _22, _23, _31, _32, _33);
	*/
	NEON_STATISTICS_TICK
	//Angelica 4 中的实现
	AXMatrix4 mResult;
#ifdef ARM_NEON
    ne10_invmat_4x4f((ne10_mat4x4f_t*)mResult.m, (ne10_mat4x4f_t*)m, 1);
#else
	float fDet = Determinant();

	mResult._11 = Det3x3(_22, _23, _24, _32, _33, _34, _42, _43, _44);
	mResult._12 = -Det3x3(_12, _13, _14, _32, _33, _34, _42, _43, _44);
	mResult._13 = Det3x3(_12, _13, _14, _22, _23, _24, _42, _43, _44);
	mResult._14 = -Det3x3(_12, _13, _14, _22, _23, _24, _32, _33, _34);

	mResult._21 = -Det3x3(_21, _23, _24, _31, _33, _34, _41, _43, _44);
	mResult._22 = Det3x3(_11, _13, _14, _31, _33, _34, _41, _43, _44);
	mResult._23 = -Det3x3(_11, _13, _14, _21, _23, _24, _41, _43, _44);
	mResult._24 = Det3x3(_11, _13, _14, _21, _23, _24, _31, _33, _34);

	mResult._31 = Det3x3(_21, _22, _24, _31, _32, _34, _41, _42, _44);
	mResult._32 = -Det3x3(_11, _12, _14, _31, _32, _34, _41, _42, _44);
	mResult._33 = Det3x3(_11, _12, _14, _21, _22, _24, _41, _42, _44);
	mResult._34 = -Det3x3(_11, _12, _14, _21, _22, _24, _31, _32, _34);

	mResult._41 = -Det3x3(_21, _22, _23, _31, _32, _33, _41, _42, _43);
	mResult._42 = Det3x3(_11, _12, _13, _31, _32, _33, _41, _42, _43);
	mResult._43 = -Det3x3(_11, _12, _13, _21, _22, _23, _41, _42, _43);
	mResult._44 = Det3x3(_11, _12, _13, _21, _22, _23, _31, _32, _33);

	mResult /= fDet;    
#endif
	return mResult;

	//return mat;
}

//	Get inverse matrix (used only by transform matrix)
AXMatrix4 AXMatrix4::GetInverseTM() const
{
	float fDet = 1.0f / Det3x3(_11, _12, _13, _21, _22, _23, _31, _32, _33);

	AXMatrix4 mat;
	
    mat._11 = fDet * Det3x3(_22, _23, _24, _32, _33, _34, _42, _43, _44);
    mat._12 = -fDet * Det3x3(_12, _13, _14, _32, _33, _34, _42, _43, _44);
    mat._13 = fDet * Det3x3(_12, _13, _14, _22, _23, _24, _42, _43, _44);
    mat._14 = -fDet * Det3x3(_12, _13, _14, _22, _23, _24, _32, _33, _34);

    mat._21 = -fDet * Det3x3(_21, _23, _24, _31, _33, _34, _41, _43, _44);
    mat._22 = fDet * Det3x3(_11, _13, _14, _31, _33, _34, _41, _43, _44);
    mat._23 = -fDet * Det3x3(_11, _13, _14, _21, _23, _24, _41, _43, _44);
    mat._24 = fDet * Det3x3(_11, _13, _14, _21, _23, _24, _31, _33, _34);

    mat._31 = fDet * Det3x3(_21, _22, _24, _31, _32, _34, _41, _42, _44);
    mat._32 = -fDet * Det3x3(_11, _12, _14, _31, _32, _34, _41, _42, _44);
    mat._33 = fDet * Det3x3(_11, _12, _14, _21, _22, _24, _41, _42, _44);
    mat._34 = -fDet * Det3x3(_11, _12, _14, _21, _22, _24, _31, _32, _34);

	mat._41 = -fDet * Det3x3(_21, _22, _23, _31, _32, _33, _41, _42, _43);
    mat._42 = fDet * Det3x3(_11, _12, _13, _31, _32, _33, _41, _42, _43);
    mat._43 = -fDet * Det3x3(_11, _12, _13, _21, _22, _23, _41, _42, _43);
    mat._44 = fDet * Det3x3(_11, _12, _13, _21, _22, _23, _31, _32, _33);

	return mat;
}

//	Build scale matrix
void AXMatrix4::Scale(float sx, float sy, float sz)
{
	Clear();
    _11 = sx;
    _22 = sy;
    _33 = sz;
    _44 = 1.0f;
}

//	Set rotation part of transform matrix
void AXMatrix4::SetRotatePart(const AXMatrix3& mat)
{
    for (int i=0; i < 3; i++)
    {
        for (int j=0; j < 3; j++)
            m[i][j] = mat.m[i][j];
    }
}

//	Get rotation part of transform matrix
AXMatrix3 AXMatrix4::GetRotatePart() const
{
    AXMatrix3 ret;

    for (int i=0; i < 3; i++)
    {
        for (int j=0; j < 3; j++)
            ret.m[i][j] = m[i][j];
    }

    return ret;
}