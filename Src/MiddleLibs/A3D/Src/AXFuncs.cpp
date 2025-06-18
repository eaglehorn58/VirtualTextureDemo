//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


//#include "ASys.h"
#include "AXTypes.h"
#include "A3DMacros.h"
#include "AXFuncs.h"
#include "AXAssist.h"

static bool _KeepOrthogonal(AXMatrix4 mat)
{
	float vDot;
	float vNormal;
	float error = 1e-5f;

	AXVector3 x = AXVector3(mat._11, mat._12, mat._13);
	AXVector3 y = AXVector3(mat._21, mat._22, mat._23);
	AXVector3 z = AXVector3(mat._31, mat._32, mat._33);

	vNormal = x.Magnitude();
	if (fabs(fabs(vNormal) - 1.0f) > error)
		return false;

	vNormal = y.Magnitude();
	if (fabs(fabs(vNormal) - 1.0f) > error)
		return false;

	vNormal = y.Magnitude();
	if (fabs(fabs(vNormal) - 1.0f) > error)
		return false;

	vDot = DotProduct(x, y);
	if (fabs(vDot) > error)
		return false;

	vDot = DotProduct(y, z);
	if (fabs(vDot) > error)
		return false;

	vDot = DotProduct(x, z);
	if (fabs(vDot) > error)
		return false;
	
	vDot = DotProduct(CrossProduct(x, y), z);
	if (fabs(fabs(vDot) - 1.0f) > error)
		return false;

	return true;
}

//	Inverse sqrt using Newton approximation
float a3d_InvSqrt(float v)
{
	float vhalf = 0.5f * v;
	int i = *(int*)&v;
	i = 0x5f3759df - (i >> 1);
	v = *(float*)&i;
	v = v * (1.5f - vhalf * v * v);
	return v;
}

// Return min/max vector composed with min/max component of the input 2 vector
AXVector3 a3d_VecMin(const AXVector3& v1, const AXVector3& v2)
{
	return AXVector3(min2(v1.x, v2.x), min2(v1.y, v2.y), min2(v1.z, v2.z));
}

AXVector3 a3d_VecMax(const AXVector3& v1, const AXVector3& v2)
{
	return AXVector3(max2(v1.x, v2.x), max2(v1.y, v2.y), max2(v1.z, v2.z));
}

//	Convert a vector from view coordinates to world coordinates
AXVector3 a3d_ViewToWorld(const AXVector3& vIn, AXMatrix4& matView)
{
	AXVector3 vOut;
	vOut.x = vIn.x * matView._11 + vIn.y * matView._12 + vIn.z * matView._13;
	vOut.y = vIn.x * matView._21 + vIn.y * matView._22 + vIn.z * matView._23;
	vOut.z = vIn.x * matView._31 + vIn.y * matView._32 + vIn.z * matView._33;
	return vOut;
}

AXMatrix3 a3d_IdentityMatrix3()
{
	AXMatrix3 result;
	memset(&result, 0, sizeof(result));
	result._11 = result._22 = result._33 = 1.0f;
	return result;
}

AXMatrix4 a3d_IdentityMatrix()
{
	AXMatrix4 result;
	memset(&result, 0, sizeof(result));
	result._11 = result._22 = result._33 = result._44 = 1.0f;
	return result;
}

AXMatrix4 a3d_ZeroMatrix()
{
	AXMatrix4 result;
	memset(&result, 0, sizeof(result));
	return result;
}

AXMatrix4 a3d_ViewMatrix(const AXVector3& from, const AXVector3& dir, 
					  const AXVector3& vecUp, float roll)
{
	AXMatrix4 view = a3d_IdentityMatrix();
	AXVector3 up, right, view_dir;

	view_dir = Normalize(dir);
	right = CrossProduct(vecUp, dir);
	right = Normalize(right);
	up = CrossProduct(dir, right);
	up = Normalize(up);
	
	view.m[0][0] = right.x;
	view.m[1][0] = right.y;
	view.m[2][0] = right.z;
	view.m[0][1] = up.x;
	view.m[1][1] = up.y;
	view.m[2][1] = up.z;
	view.m[0][2] = view_dir.x;
	view.m[1][2] = view_dir.y;
	view.m[2][2] = view_dir.z;
	
	view.m[3][0] = -DotProduct(right, from);
	view.m[3][1] = -DotProduct(up, from);
	view.m[3][2] = -DotProduct(view_dir, from);

	// Set roll
	if (roll != 0.0f)
		view = a3d_RotateZ(-roll) * view;
	
	return view;
} 

AXMatrix4 a3d_LookAtMatrix(const AXVector3& vEye, const AXVector3& vAt, const AXVector3& vUp, float roll)
{
	AXMatrix4 m;
	//D3DXMatrixLookAtLH((D3DXMATRIX*)&mat, (D3DXVECTOR3*)&from, (D3DXVECTOR3*)&to, (D3DXVECTOR3*)&vecUp);
	//From Angelica 4
	AXVector3 vZAxis = vAt - vEye;
	AXVector3 vXAxis = CrossProduct(vUp, vZAxis);
	AXVector3 vYAxis = CrossProduct(vZAxis, vXAxis);
	vZAxis.Normalize();
	vXAxis.Normalize();
	vYAxis.Normalize();
	m._11 = vXAxis.x; m._12 = vYAxis.x; m._13 = vZAxis.x; m._14 = 0.0f;
	m._21 = vXAxis.y; m._22 = vYAxis.y; m._23 = vZAxis.y; m._24 = 0.0f;
	m._31 = vXAxis.z; m._32 = vYAxis.z; m._33 = vZAxis.z; m._34 = 0.0f;
	m._41 = -DotProduct(vXAxis, vEye);
	m._42 = -DotProduct(vYAxis, vEye);
	m._43 = -DotProduct(vZAxis, vEye);
	m._44 = 1.0f;


	//	Set roll
	if (roll != 0.0f)
		m = a3d_RotateZ(-roll) * m;

	return m;
}

AXMatrix4 a3d_Transpose(const AXMatrix4& tm)
{
	AXMatrix4 matT;
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<4; j++)
			matT.m[i][j] = tm.m[j][i];
	}

	return matT;
}

void a3d_Transpose(const AXMatrix4& matIn, AXMatrix4* pmatOut)
{
	for (int i=0; i < 4; i++)
	{
		for (int j=0; j < 4; j++)
			pmatOut->m[i][j] = matIn.m[j][i];
	}
}
/*
void a3d_DecomposeMatrix(const AXMatrix4& mat, AXVector3& scale, AXQuaternion& orient, AXVector3& pos)
{
	AXVector3	vecX, vecY, vecZ;
	
	vecX = mat.GetRow(0);
	vecY = mat.GetRow(1);
	vecZ = mat.GetRow(2);

	scale.x = Magnitude(vecX);
	scale.y = Magnitude(vecY);
	scale.z = Magnitude(vecZ);

	pos  = mat.GetRow(3);

	AXMatrix4 matOrient = IdentityMatrix();
	matOrient.SetRow(0, vecX / scale.x);
	matOrient.SetRow(1, vecY / scale.y);
	matOrient.SetRow(2, vecZ / scale.z);

	orient.ConvertFromMatrix(matOrient);
}
*/
/*
AXMatrix4 InverseTM(AXMatrix4 tm)
{
	//We should check if tm is a unified transform matrix;
	//that is to say: each row's magnitude must be 1.0f;
	AXMatrix4 matInv = a3d_IdentityMatrix();

	//X axis;
	matInv._11 = tm._11;
	matInv._21 = tm._12;
	matInv._31 = tm._13;

	//Y axis;
	matInv._12 = tm._21;
	matInv._22 = tm._22;
	matInv._32 = tm._23;

	//Z axis;
	matInv._13 = tm._31;
	matInv._23 = tm._32;
	matInv._33 = tm._33;

	//Offset;
	matInv._41 = -(tm._41 * tm._11 + tm._42 * tm._12 + tm._43 * tm._13);
	matInv._42 = -(tm._41 * tm._21 + tm._42 * tm._22 + tm._43 * tm._23);
	matInv._43 = -(tm._41 * tm._31 + tm._42 * tm._32 + tm._43 * tm._33);

	return matInv;
}*/

static float _Det(float a11, float a12, float a13, 
				float a21, float a22, float a23,
				float a31, float a32, float a33)
{
	return a11 * a22 * a33 + a21 * a32 * a13 + a31 * a12 * a23 -
		a13 * a22 * a31 - a23 * a32 * a11 - a33 * a12 * a21;
}

AXMatrix4 a3d_InverseTM(const AXMatrix4& mat)
{
	AXMatrix4 ret;
	a3d_InverseTM(mat, &ret);
	return ret;
}

void a3d_InverseTM(const AXMatrix4& matIn, AXMatrix4* pmatOut)
{
	float vDet;

	vDet = 1.0f / _Det(matIn._11, matIn._12, matIn._13,
					   matIn._21, matIn._22, matIn._23,
					   matIn._31, matIn._32, matIn._33);

	pmatOut->_11 = vDet * _Det(matIn._22, matIn._23, matIn._24,
		matIn._32, matIn._33, matIn._34,
		matIn._42, matIn._43, matIn._44);
	pmatOut->_12 = -vDet * _Det(matIn._12, matIn._13, matIn._14,
		matIn._32, matIn._33, matIn._34,
		matIn._42, matIn._43, matIn._44);
	pmatOut->_13 = vDet * _Det(matIn._12, matIn._13, matIn._14,
		matIn._22, matIn._23, matIn._24,
		matIn._42, matIn._43, matIn._44);
	pmatOut->_14 = -vDet * _Det(matIn._12, matIn._13, matIn._14,
		matIn._22, matIn._23, matIn._24,
		matIn._32, matIn._33, matIn._34);

	pmatOut->_21 = -vDet * _Det(matIn._21, matIn._23, matIn._24,
		matIn._31, matIn._33, matIn._34,
		matIn._41, matIn._43, matIn._44);
	pmatOut->_22 = vDet * _Det(matIn._11, matIn._13, matIn._14,
		matIn._31, matIn._33, matIn._34,
		matIn._41, matIn._43, matIn._44);
	pmatOut->_23 = -vDet * _Det(matIn._11, matIn._13, matIn._14,
		matIn._21, matIn._23, matIn._24,
		matIn._41, matIn._43, matIn._44);
	pmatOut->_24 = vDet * _Det(matIn._11, matIn._13, matIn._14,
		matIn._21, matIn._23, matIn._24,
		matIn._31, matIn._33, matIn._34);

	pmatOut->_31 = vDet * _Det(matIn._21, matIn._22, matIn._24,
		matIn._31, matIn._32, matIn._34,
		matIn._41, matIn._42, matIn._44);
	pmatOut->_32 = -vDet * _Det(matIn._11, matIn._12, matIn._14,
		matIn._31, matIn._32, matIn._34,
		matIn._41, matIn._42, matIn._44);
	pmatOut->_33 = vDet * _Det(matIn._11, matIn._12, matIn._14,
		matIn._21, matIn._22, matIn._24,
		matIn._41, matIn._42, matIn._44);
	pmatOut->_34 = -vDet * _Det(matIn._11, matIn._12, matIn._14,
		matIn._21, matIn._22, matIn._24,
		matIn._31, matIn._32, matIn._34);

	pmatOut->_41 = -vDet * _Det(matIn._21, matIn._22, matIn._23,
		matIn._31, matIn._32, matIn._33,
		matIn._41, matIn._42, matIn._43);
	pmatOut->_42 = vDet * _Det(matIn._11, matIn._12, matIn._13,
		matIn._31, matIn._32, matIn._33,
		matIn._41, matIn._42, matIn._43);
	pmatOut->_43 = -vDet * _Det(matIn._11, matIn._12, matIn._13,
		matIn._21, matIn._22, matIn._23,
		matIn._41, matIn._42, matIn._43);
	pmatOut->_44 = vDet * _Det(matIn._11, matIn._12, matIn._13,
		matIn._21, matIn._22, matIn._23,
		matIn._31, matIn._32, matIn._33);
}

AXMatrix4 a3d_InverseAffineMatrix(const AXMatrix4& mat)
{
	AXMatrix4 matInv;
	a3d_InverseAffineMatrix(mat, &matInv);
	return matInv;
}

AXMatrix4 a3d_InverseMatrix(const AXMatrix4& mat)
{
	AXMatrix4 matInv;
	a3d_InverseMatrix(mat, &matInv);
	return matInv;
}

inline afloat32 a3d_Determinant3(
	afloat32 _m11, afloat32 _m12, afloat32 _m13,
	afloat32 _m21, afloat32 _m22, afloat32 _m23,
	afloat32 _m31, afloat32 _m32, afloat32 _m33)
{
	return 
		+ _m11 * (_m22 * _m33 - _m23 * _m32)
		+ _m12 * (_m23 * _m31 - _m21 * _m33)
		+ _m13 * (_m21 * _m32 - _m22 * _m31);
}


afloat32 a3d_DeterminantMatrix(const AXMatrix4& matIn)
{
	return
		- matIn._14 * a3d_Determinant3(matIn._21, matIn._22, matIn._23, matIn._31, matIn._32, matIn._33, matIn._41, matIn._42, matIn._43)
		+ matIn._24 * a3d_Determinant3(matIn._11, matIn._12, matIn._13, matIn._31, matIn._32, matIn._33, matIn._41, matIn._42, matIn._43)
		- matIn._34 * a3d_Determinant3(matIn._11, matIn._12, matIn._13, matIn._21, matIn._22, matIn._23, matIn._41, matIn._42, matIn._43)
		+ matIn._44 * a3d_Determinant3(matIn._11, matIn._12, matIn._13, matIn._21, matIn._22, matIn._23, matIn._31, matIn._32, matIn._33);

}

void a3d_InverseAffineMatrix(const AXMatrix4& matIn, AXMatrix4* pmatOut)
{
	pmatOut->_11 = matIn._11; pmatOut->_12 = matIn._21; pmatOut->_13 = matIn._31, pmatOut->_14 = 0.0f;
	pmatOut->_21 = matIn._12; pmatOut->_22 = matIn._22; pmatOut->_23 = matIn._32, pmatOut->_24 = 0.0f;
	pmatOut->_31 = matIn._13; pmatOut->_32 = matIn._23; pmatOut->_33 = matIn._33, pmatOut->_34 = 0.0f;

	pmatOut->_41 = -matIn._11 * matIn._41 - matIn._12 * matIn._42 - matIn._13 * matIn._43;
	pmatOut->_42 = -matIn._21 * matIn._41 - matIn._22 * matIn._42 - matIn._23 * matIn._43;
	pmatOut->_43 = -matIn._31 * matIn._41 - matIn._32 * matIn._42 - matIn._33 * matIn._43;
	pmatOut->_44 = 1.0f;
}

void a3d_InverseMatrix(const AXMatrix4& matIn, AXMatrix4* pmatOut)
{

	//D3DXMatrixInverse((D3DXMATRIX*) pmatOut, NULL, (D3DXMATRIX*) &matIn);
	// From Angelica 4

	afloat32 fDet = a3d_DeterminantMatrix(matIn);
	AXMatrix4 mResult;

	mResult._11 =  a3d_Determinant3(matIn._22, matIn._23, matIn._24, matIn._32, matIn._33, matIn._34, matIn._42, matIn._43, matIn._44);
	mResult._12 = -a3d_Determinant3(matIn._12, matIn._13, matIn._14, matIn._32, matIn._33, matIn._34, matIn._42, matIn._43, matIn._44);
	mResult._13 =  a3d_Determinant3(matIn._12, matIn._13, matIn._14, matIn._22, matIn._23, matIn._24, matIn._42, matIn._43, matIn._44);
	mResult._14 = -a3d_Determinant3(matIn._12, matIn._13, matIn._14, matIn._22, matIn._23, matIn._24, matIn._32, matIn._33, matIn._34);

	mResult._21 = -a3d_Determinant3(matIn._21, matIn._23, matIn._24, matIn._31, matIn._33, matIn._34, matIn._41, matIn._43, matIn._44);
	mResult._22 =  a3d_Determinant3(matIn._11, matIn._13, matIn._14, matIn._31, matIn._33, matIn._34, matIn._41, matIn._43, matIn._44);
	mResult._23 = -a3d_Determinant3(matIn._11, matIn._13, matIn._14, matIn._21, matIn._23, matIn._24, matIn._41, matIn._43, matIn._44);
	mResult._24 =  a3d_Determinant3(matIn._11, matIn._13, matIn._14, matIn._21, matIn._23, matIn._24, matIn._31, matIn._33, matIn._34);

	mResult._31 =  a3d_Determinant3(matIn._21, matIn._22, matIn._24, matIn._31, matIn._32, matIn._34, matIn._41, matIn._42, matIn._44);
	mResult._32 = -a3d_Determinant3(matIn._11, matIn._12, matIn._14, matIn._31, matIn._32, matIn._34, matIn._41, matIn._42, matIn._44);
	mResult._33 =  a3d_Determinant3(matIn._11, matIn._12, matIn._14, matIn._21, matIn._22, matIn._24, matIn._41, matIn._42, matIn._44);
	mResult._34 = -a3d_Determinant3(matIn._11, matIn._12, matIn._14, matIn._21, matIn._22, matIn._24, matIn._31, matIn._32, matIn._34);

	mResult._41 = -a3d_Determinant3(matIn._21, matIn._22, matIn._23, matIn._31, matIn._32, matIn._33, matIn._41, matIn._42, matIn._43);
	mResult._42 =  a3d_Determinant3(matIn._11, matIn._12, matIn._13, matIn._31, matIn._32, matIn._33, matIn._41, matIn._42, matIn._43);
	mResult._43 = -a3d_Determinant3(matIn._11, matIn._12, matIn._13, matIn._21, matIn._22, matIn._23, matIn._41, matIn._42, matIn._43);
	mResult._44 =  a3d_Determinant3(matIn._11, matIn._12, matIn._13, matIn._21, matIn._22, matIn._23, matIn._31, matIn._32, matIn._33);

	mResult /= fDet;
	*pmatOut = mResult;
}

AXMatrix4 a3d_TransformMatrix(const AXVector3& vecDir, const AXVector3& vecUp, const AXVector3& vecPos)
{
	AXMatrix4   mat;
	AXVector3   vecXAxis, vecYAxis, vecZAxis;

	vecZAxis = Normalize(vecDir);
	vecYAxis = Normalize(vecUp);
	vecXAxis = Normalize(CrossProduct(vecYAxis, vecZAxis));

	memset(&mat, 0, sizeof(mat));
	mat.m[0][0] = vecXAxis.x;
	mat.m[0][1] = vecXAxis.y;
	mat.m[0][2] = vecXAxis.z;

	mat.m[1][0] = vecYAxis.x;
	mat.m[1][1] = vecYAxis.y;
	mat.m[1][2] = vecYAxis.z;

	mat.m[2][0] = vecZAxis.x;
	mat.m[2][1] = vecZAxis.y;
	mat.m[2][2] = vecZAxis.z;

	mat.m[3][0] = vecPos.x;
	mat.m[3][1] = vecPos.y;
	mat.m[3][2] = vecPos.z;
	mat.m[3][3] = 1.0f;

	return mat;
}


AXMatrix4 a3d_RotateX(float vRad)
{
	AXMatrix4 ret = a3d_IdentityMatrix();
	ret.m[2][2] = ret.m[1][1] = (float) cos(vRad);
	ret.m[1][2] = (float) sin(vRad);
	ret.m[2][1] = (float) -ret.m[1][2];

	//	_KeepOrthogonal(ret);
	return ret;
}

AXMatrix4 a3d_RotateX(const AXMatrix4& mat, float vRad)
{
	return mat * a3d_RotateX(vRad);
}

AXMatrix4 a3d_RotateY(float vRad)
{
	AXMatrix4 ret = a3d_IdentityMatrix();
	ret.m[2][2] = ret.m[0][0] = (float) cos(vRad);
	ret.m[2][0] = (float) sin(vRad);
	ret.m[0][2] = -ret.m[2][0];
	//	_KeepOrthogonal(ret);
	return ret;
}

AXMatrix4 a3d_RotateY(const AXMatrix4& mat, float vRad)
{
	return mat * a3d_RotateY(vRad);
}

AXMatrix4 a3d_RotateZ(float vRad)
{
	AXMatrix4 ret = a3d_IdentityMatrix();
	ret.m[1][1] = ret.m[0][0] = (float) cos(vRad);
	ret.m[0][1] = (float) sin(vRad);
	ret.m[1][0] = -ret.m[0][1];
	//	_KeepOrthogonal(ret);
	return ret;
}

AXMatrix4 a3d_RotateZ(const AXMatrix4& mat, float vRad)
{
	return mat * a3d_RotateZ(vRad);
}

AXMatrix4 a3d_RotateAxis(const AXVector3& vRotAxis, float vRad)
{
	/*
		The derivation of this algorithm can be seen in rotation.pdf in my 文档 directory.
		The basic idea is to divide the original vector to two part: paralell to axis and 
		perpendicular to axis, then only perpendicular part can be affected by this rotation.
		Now divide the rotation part onto the main axis on the rotation plane then it will
		be clear to see what composes the rotated vector. Finally we can get the transform
		matrix from a set of 3 equations.
	*/
	AXVector3 vecAxis = Normalize(vRotAxis);

	AXMatrix4 ret;
	float xx, xy, xz, yy, yz, zz, cosine, sine, one_cs, xsine, ysine, zsine;

	xx = vecAxis.x * vecAxis.x;
	xy = vecAxis.x * vecAxis.y;
	xz = vecAxis.x * vecAxis.z;
	yy = vecAxis.y * vecAxis.y;
	yz = vecAxis.y * vecAxis.z;
	zz = vecAxis.z * vecAxis.z;

	cosine = (float)cos(vRad);
	sine = (float)sin(vRad);
	one_cs = 1.0f - cosine;

	xsine = vecAxis.x * sine;
	ysine = vecAxis.y * sine;
	zsine = vecAxis.z * sine;
	
	ret._11 = xx + cosine * (1.0f - xx);
	ret._12 = xy * one_cs + zsine;
	ret._13 = xz * one_cs - ysine;
	ret._14 = 0.0f;

	ret._21 = xy * one_cs - zsine;
	ret._22 = yy + cosine * (1.0f - yy);
	ret._23 = yz * one_cs + xsine;
	ret._24 = 0.0f;

	ret._31 = xz * one_cs + ysine;
	ret._32 = yz * one_cs - xsine;
	ret._33 = zz + cosine * (1.0f - zz);
	ret._34 = 0.0f;

	ret._41 = 0.0f;
	ret._42 = 0.0f;
	ret._43 = 0.0f;
	ret._44 = 1.0f;
	
	//D3DXMatrixRotationAxis((D3DXMATRIX *)&ret, (D3DXVECTOR3*)&vecAxis, vRad);
	return ret;
}

AXMatrix4 a3d_RotateAxis(const AXVector3& vecPos, const AXVector3& vecAxis, float vRad)
{
	AXMatrix4 ret = a3d_Translate(-vecPos.x, -vecPos.y, -vecPos.z);
	ret = ret * a3d_RotateAxis(vecAxis, vRad);
	ret = ret * a3d_Translate(vecPos.x, vecPos.y, vecPos.z);

	return ret;
}

//	Rotate a position around axis X
AXVector3 a3d_RotatePosAroundX(const AXVector3& vPos, float fRad)
{
	AXMatrix4 mat;
	mat.RotateX(fRad);
	return mat.TransformCoord(vPos);
}

//	Rotate a position around axis Y
AXVector3 a3d_RotatePosAroundY(const AXVector3& vPos, float fRad)
{
	AXMatrix4 mat;
	mat.RotateY(fRad);
	return mat.TransformCoord(vPos);
}

//	Rotate a position around axis Z
AXVector3 a3d_RotatePosAroundZ(const AXVector3& vPos, float fRad)
{
	AXMatrix4 mat;
	mat.RotateZ(fRad);
	return mat.TransformCoord(vPos);
}

/*	Rotate a position around arbitrary axis

	Return result position.

	vPos: position will do rotate
	vAxis: normalized axis
	fRad: rotation radian
*/
AXVector3 a3d_RotatePosAroundAxis(const AXVector3& vPos, const AXVector3& vAxis, float fRad)
{
	AXMatrix4 mat;
	mat.RotateAxis(vAxis, fRad);
	return mat.TransformCoord(vPos);
}

/*	Rotate a position around a line
	
	Return result position.

	vPos: position will do rotate
	vOrigin: point on line
	vDir: normalized line's direction
	fRad: rotation radian
*/
AXVector3 a3d_RotatePosAroundLine(const AXVector3& vPos, const AXVector3& vOrigin, const AXVector3& vDir, float fRad)
{
	AXMatrix4 mat;
	mat.RotateAxis(vOrigin, vDir, fRad);
	return mat.TransformCoord(vPos);
}

/*	Rotate a position around a line
	
	Return result vector.

	v: vector will do rotate
	vOrigin: point on line
	vDir: normalized line's direction
	fRad: rotation radian
*/
AXVector3 a3d_RotateVecAroundLine(const AXVector3& v, const AXVector3& vOrigin, const AXVector3& vDir, float fRad)
{
	AXMatrix4 mat;
	mat.RotateAxis(vOrigin, vDir, fRad);
	return mat.TransformCoord(v) - mat.GetRow(3);
}

AXMatrix4 a3d_Scaling(float sx, float sy, float sz)
{
	AXMatrix4 ret = ZeroMatrix();
	ret.m[0][0] = sx;
	ret.m[1][1] = sy;
	ret.m[2][2] = sz;
	ret.m[3][3] = 1.0f;
	return ret;
}

AXMatrix4 a3d_Scaling(const AXMatrix4& mat, float sx, float sy, float sz)
{
	AXMatrix4 ret = mat;
	ret.m[0][0] *= sx;
	ret.m[0][1] *= sx;
	ret.m[0][2] *= sx;
	ret.m[0][3] *= sx;

	ret.m[1][0] *= sy;
	ret.m[1][1] *= sy;
	ret.m[1][2] *= sy;
	ret.m[1][3] *= sy;

	ret.m[2][0] *= sz;
	ret.m[2][1] *= sz;
	ret.m[2][2] *= sz;
	ret.m[2][3] *= sz;
	return ret;
}

AXMatrix4 a3d_ScalingRelative(const AXMatrix4& matRoot, float sx, float sy, float sz)
{
	AXMatrix4 matScale; 
	matScale = InverseTM(matRoot) * a3d_Scaling(sx, sy, sz) * matRoot;

	return matScale;
}

AXMatrix4 a3d_Translate(float x, float y, float z)
{
	AXMatrix4 ret = a3d_IdentityMatrix();

	ret._41 = x;
	ret._42 = y;
	ret._43 = z;

	return ret;
}

AXMatrix4 a3d_ScaleAlongAxis(const AXVector3& vAxis, float fScale)
{
/*	AXMatrix4 mat1, mat2;

	mat1.Clear();
	mat1._11 = mat1._22 = mat1._33 = 1.0f;
	mat1._14 = vAxis.x;
	mat1._24 = vAxis.y;
	mat1._34 = vAxis.z;

	mat2.Clear();
	mat2._11 = mat2._22 = mat2._33 = 1.0f;
	mat2._41 = (fScale - 1.0f) * vAxis.x;
	mat2._42 = (fScale - 1.0f) * vAxis.y;
	mat2._43 = (fScale - 1.0f) * vAxis.z;

	mat = mat1 * mat2;
	mat._14 = mat._24 = mat._34 = 0.0f;
	mat._44 = 1.0f;
*/
	AXMatrix4 mat(AXMatrix4::IDENTITY);

	float s = fScale - 1.0f;
	float f1 = s * vAxis.x * vAxis.y;
	float f2 = s * vAxis.y * vAxis.z;
	float f3 = s * vAxis.x * vAxis.z;

	mat._11 = 1 + s * vAxis.x * vAxis.x;
	mat._22 = 1 + s * vAxis.y * vAxis.y;
	mat._33 = 1 + s * vAxis.z * vAxis.z;
	mat._12 = mat._21 = f1;
	mat._13 = mat._31 = f3;
	mat._23 = mat._32 = f2;

	return mat;
}

//	Multiply a vertex but don't consider translation factors (the fourth row of matrix),
//	this function can be used to transform a normal. 
AXVector3 a3d_VectorMatrix3x3(const AXVector3& v, const AXMatrix4& mat)
{
	return AXVector3(v.x * mat._11 + v.y * mat._21 + v.z * mat._31,
					  v.x * mat._12 + v.y * mat._22 + v.z * mat._32,
					  v.x * mat._13 + v.y * mat._23 + v.z * mat._33);
}

A3DCOLOR a3d_ColorValueToColorRGBA(A3DCOLORVALUE colorValue)
{
	int r, g, b, a;

	a = (int)(colorValue.a * 255);
	r = (int)(colorValue.r * 255);
	g = (int)(colorValue.g * 255);
	b = (int)(colorValue.b * 255);

	if( a > 255 ) a = 255;
	if( r > 255 ) r = 255;
	if( g > 255 ) g = 255;
	if( b > 255 ) b = 255;
	return A3DCOLORRGBA(r, g, b, a);
}

A3DCOLORVALUE a3d_ColorRGBAToColorValue(A3DCOLOR colorRGBA)
{
	unsigned char r, g, b, a;
	static float fTemp = 1.0f / 255.0f;
						
	a = A3DCOLOR_GETALPHA(colorRGBA);
	r = A3DCOLOR_GETRED(colorRGBA);
	g = A3DCOLOR_GETGREEN(colorRGBA);
	b = A3DCOLOR_GETBLUE(colorRGBA);

	return A3DCOLORVALUE(r * fTemp, g * fTemp, b * fTemp, a * fTemp);
}

AXVector3 a3d_RandDirH()
{
	AXVector3 vecDirH;

	float vRad = (rand() % 10000) / 10000.0f * 2.0f * A3D_PI;
	
	vecDirH.x = (float) cos(vRad);
	vecDirH.y = 0.0f;
	vecDirH.z = (float) sin(vRad);

	return vecDirH;
}

AXMatrix4 a3d_MirrorMatrix(const AXVector3& p, // IN: point on the plane
							const AXVector3& n) // IN: plane perpendicular direction
{
	AXMatrix4 ret;

	//V' = V - 2((V - P)*N)N)
	float dot = p.x*n.x + p.y*n.y + p.z*n.z;
	
	ret.m[0][0] = 1 - 2*n.x*n.x;
	ret.m[1][0] = - 2*n.x*n.y;
	ret.m[2][0] = - 2*n.x*n.z;
	ret.m[3][0] = 2*dot*n.x;

	ret.m[0][1] = - 2*n.y*n.x;
	ret.m[1][1] = 1 - 2*n.y*n.y;
	ret.m[2][1] = - 2*n.y*n.z;
	ret.m[3][1] = 2*dot*n.y;

	ret.m[0][2] = - 2*n.z*n.x;
	ret.m[1][2] = - 2*n.z*n.y;
	ret.m[2][2] = 1 - 2*n.z*n.z;
	ret.m[3][2] = 2*dot*n.z;

	ret.m[0][3] = 0;
	ret.m[1][3] = 0;
	ret.m[2][3] = 0;
	ret.m[3][3] = 1;

	return ret;
}

//	Clear AABB
void a3d_ClearAABB(AXVector3& vMins, AXVector3& vMaxs)
{
	vMins.Set(999999.0f, 999999.0f, 999999.0f);
	vMaxs.Set(-999999.0f, -999999.0f, -999999.0f);
}

//Use a vertex point to expand an AABB data;
void a3d_AddVertexToAABB(AXVector3& vMins, AXVector3& vMaxs, AXVector3& vPoint)
{
	for (int i=0; i < 3; i++)
	{
		if (vPoint.m[i] < vMins.m[i])
			vMins.m[i] = vPoint.m[i];
	
		if (vPoint.m[i] > vMaxs.m[i])
			vMaxs.m[i] = vPoint.m[i];
	}
}

//Use a OBB data to expand an AABB data;
void a3d_ExpandAABB(AXVector3& vecMins, AXVector3& vecMaxs, const AXOBB& obb)
{
	AXVector3 v[8];

	//Up 4 corner;
	v[0] = obb.Center + obb.ExtY - obb.ExtX + obb.ExtZ;
	v[1] = v[0] + obb.ExtX + obb.ExtX;	//	+ obb.ExtX * 2.0f;
	v[2] = v[1] - obb.ExtZ - obb.ExtZ;	//	+ obb.ExtZ * (-2.0f);
	v[3] = v[2] - obb.ExtX - obb.ExtX;	//	+ obb.ExtX * (-2.0f);

	//Down 4 corner;
	v[4] = obb.Center - obb.ExtY - obb.ExtX + obb.ExtZ;
	v[5] = v[4] + obb.ExtX + obb.ExtX;	//	+ obb.ExtX * 2.0f;
	v[6] = v[5] - obb.ExtZ - obb.ExtZ;	//	+ obb.ExtZ * (-2.0f);
	v[7] = v[6] - obb.ExtX - obb.ExtX;	//	+ obb.ExtX * (-2.0f);

	for(int i=0; i<3; i++)
	{
		for(int j=0; j<8; j++)
		{
			if( vecMins.m[i] > v[j].m[i] )
				vecMins.m[i] = v[j].m[i];

			if( vecMaxs.m[i] < v[j].m[i] )
				vecMaxs.m[i] = v[j].m[i];
		}
	}
}

//Use a sub AABB data to expand an AABB data;
void a3d_ExpandAABB(AXVector3& vecMins, AXVector3& vecMaxs, const AXAABB& subAABB)
{
	for (int i=0; i<3; i++)
	{
		if (vecMins.m[i] > subAABB.Mins.m[i])
			vecMins.m[i] = subAABB.Mins.m[i];
		if (vecMaxs.m[i] < subAABB.Maxs.m[i])
			vecMaxs.m[i] = subAABB.Maxs.m[i];
	}
}

void a3d_HSVToRGB(A3DHSVCOLORVALUE& hsvColor, A3DCOLOR * pRgbColor)
{
	int r=0, g=0, b=0, a=0;

	float h = hsvColor.h;
	float s = hsvColor.s;
	float v = hsvColor.v * 255.0f + 0.5f;

	a = (int)(hsvColor.a * 255.0f);

	if( s == 0 )
		r = g = b = (int)v;
	else
	{
		if( h == 1.0f )
			h = 0.0f;

		h *= 6.0f;
		int i = int(floor(h));
		float f = h - i;
		float aa = v * (1 - s);
		float bb = v * (1 - s * f);
		float cc = v * (1 - s * (1 - f));

		switch(i)
		{
		case 0: r = (int)v;	 g = (int)cc; b = (int)aa; break;
		case 1: r = (int)bb; g = (int)v;  b = (int)aa; break;
		case 2: r = (int)aa; g = (int)v;  b = (int)cc; break;
		case 3:	r = (int)aa; g = (int)bb; b = (int)v;  break;
		case 4: r = (int)cc; g = (int)aa; b = (int)v;  break;
		case 5: r = (int)v;  g = (int)aa; b = (int)bb; break;
		}
	}

	*pRgbColor = A3DCOLORRGBA(r, g, b, a);
}

void a3d_RGBToHSV(A3DCOLOR rgbColor, A3DHSVCOLORVALUE * pHsvColor)
{
	float h = 0.0f;

	float r = A3DCOLOR_GETRED  (rgbColor) / 255.0f;
	float g = A3DCOLOR_GETGREEN(rgbColor) / 255.0f;
	float b = A3DCOLOR_GETBLUE (rgbColor) / 255.0f;
	float a = A3DCOLOR_GETALPHA(rgbColor) / 255.0f;

	float vMax = max2(r, max2(g, b)), vMin = min2(r, min2(g, b));
	float vDelta = vMax - vMin;

	pHsvColor->v = vMax;
	pHsvColor->a = a;

	if( vMax != 0.0f )
		pHsvColor->s = vDelta / vMax;
	else
		pHsvColor->s = 0.0f;

	if( pHsvColor->s == 0.0f ) 
		h = 0.0f;
	else
	{
		if( r == vMax )
			h = (g - b) / vDelta;
		else if( g == vMax)
			h = 2.0f + (b - r) / vDelta;
		else if( b == vMax )
			h = 4.0f + (r - g) / vDelta;
		
		h *= 60.0f;
		if( h < 0.0f ) h += 360.0f;
		h /= 360.0f;
	}
	pHsvColor->h = h;
}

void a3d_RGBToHSL(float r, float g, float b, float&  h, float& s, float& l)
{
	h = 0.0f;

	float vMax = max2(r, max2(g, b)), vMin = min2(r, min2(g, b));
	float vDelta = vMax - vMin;

	l = (vMax + vMin) / 2.0f;

	if( l == 0.0f || vDelta == 0.0f )
		s = 0.0f;
	else if( l <= 0.5f )
		s = vDelta / (2.0f * l);
	else
		s = vDelta / (2.0f - 2.0f*l);


	if( s == 0.0f ) 
		h = 0.0f;
	else
	{
		if( r == vMax )
			h = (g - b) / vDelta;
		else if( g == vMax)
			h = 2.0f + (b - r) / vDelta;
		else if( b == vMax )
			h = 4.0f + (r - g) / vDelta;

		h *= 60.0f;
		if( h < 0.0f ) h += 360.0f;
		h /= 360.0f;
	}
}

void a3d_DecompressColor(A3DCOLOR rgbColor, unsigned char * pR, unsigned char * pG, unsigned char * pB, unsigned char * pA)
{
	if( pR )
		*pR = A3DCOLOR_GETRED(rgbColor);
	if( pG )
		*pG = A3DCOLOR_GETGREEN(rgbColor);
	if( pB )
		*pB = A3DCOLOR_GETBLUE(rgbColor);
	if( pA )
		*pA = A3DCOLOR_GETALPHA(rgbColor);
}

// Get the dir and up of a view within the cube map
// 0 ---- right
// 1 ---- left
// 2 ---- top
// 3 ---- bottom
// 4 ---- front
// 5 ---- back
bool a3d_GetCubeMapDirAndUp(int nFaceIndex, AXVector3 * pDir, AXVector3 * pUp)
{
	switch(nFaceIndex)
	{
	case 0: // Right
		*pDir	= AXVector3(1.0f, 0.0f, 0.0f);
		*pUp	= AXVector3(0.0f, 1.0f, 0.0f);
		break;
	case 1: // Left
		*pDir	= AXVector3(-1.0f, 0.0f, 0.0f);
		*pUp	= AXVector3(0.0f, 1.0f, 0.0f);
		break;
	case 2: // Top
		*pDir	= AXVector3(0.0f, 1.0f, 0.0f);
		*pUp	= AXVector3(0.0f, 0.0f, -1.0f);
		break;
	case 3: // Bottom
		*pDir	= AXVector3(0.0f, -1.0f, 0.0f);
		*pUp	= AXVector3(0.0f, 0.0f, 1.0f);
		break;
	case 4: // Front
		*pDir	= AXVector3(0.0f, 0.0f, 1.0f);
		*pUp	= AXVector3(0.0f, 1.0f, 0.0f);
		break;
	case 5: // Back
		*pDir	= AXVector3(0.0f, 0.0f, -1.0f);
		*pUp	= AXVector3(0.0f, 1.0f, 0.0f);
		break;
	default: // Error
		*pDir	= AXVector3(0.0f);
		*pUp	= AXVector3(0.0f);
		return false;
	}
	return true;
}

// D3DVERTEXELEMENT9 a3d_SetVSDecl(WORD Stream, WORD Offset, BYTE Type, BYTE Method,BYTE Usage, BYTE UsageIndex)
// {
// 	D3DVERTEXELEMENT9 Element;
// 	Element.Stream = Stream;
// 	Element.Offset = Offset;
// 	Element.Type = Type;
// 	Element.Method = Method;
// 	Element.Usage = Usage;
// 	Element.UsageIndex = UsageIndex;
// 	return Element;
// }

//	Build standard vertex shader declarator from vertex type
// void a3d_MakeVSDeclarator(A3DVERTEXTYPE VertexType, int iStreamIdx, D3DVERTEXELEMENT9* pDecl)
// {
// 	int iCnt = 0;
// 	switch (VertexType)
// 	{
// 	case A3DVT_VERTEX:
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0);
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0);
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0);
// 			break;
// 	case A3DVT_VERTEXTAN:
// 		{
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0);
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0);
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0);
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 32, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0);
// 		}
// 		break;
// 	case A3DVT_LVERTEX:	
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0);
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0);
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 1);
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 20, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0);
// 			break;
// 	case A3DVT_TLVERTEX:	
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0);
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0);
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 1);
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0);
// 			break;
// 	case A3DVT_IBLVERTEX:
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0);
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0);
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 1);
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 20, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0);
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1);
// 			break;
// 	case A3DVT_BVERTEX1:
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0);
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 12, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0);
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0);
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 20, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0);
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 32, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0);
// 			break;
// 	case A3DVT_BVERTEX2:
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0);
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0);
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0);
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0);
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 36, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0);
// 			break;
// 	case A3DVT_BVERTEX3:
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0);
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0);
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0);
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 28, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0);
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 40, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0);
// 			break;
// 	case A3DVT_BVERTEX3TAN:
// 		{
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0);
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0);
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0);
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 28, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0);
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 40, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0);
// 			pDecl[iCnt++] = a3d_SetVSDecl(iStreamIdx, 48, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0);
// 		}
// 		break;
// 	default:
// 		ASSERT(0);
// 	}
// 
// 	pDecl[iCnt++] = a3d_SetVSDecl(0xFF,0,D3DDECLTYPE_UNUSED,0,0,0);//D3DDECL_END();
// 	return;
// }
// 
// 
// 
// //	Build vertex shader declarator from FVF
// void a3d_MakeVSDeclarator(auint32 dwFVF, D3DVERTEXELEMENT9* pDecl)
// {
// 	D3DXDeclaratorFromFVF(dwFVF, pDecl);
// }

AXMatrix3 a3d_Matrix3Rotate(float fRad)
{
	AXMatrix3 mat;
	mat.Rotate(fRad);
	return mat;
}

AXMatrix3 a3d_Matrix3Rotate(const AXMatrix3& mat, float fRad)
{
	return mat * a3d_Matrix3Rotate(fRad);
}

AXMatrix3 a3d_Matrix3Translate(float x, float y)
{
	AXMatrix3 mat;
	mat.Translate(x, y);
	return mat;
}

AXMatrix3 a3d_Matrix3Translate(const AXMatrix3& mat, float x, float y)
{
	return mat * a3d_Matrix3Translate(x, y);
}

float a3d_DistSquareToAABB(const AXVector3& vPos, const AXAABB& aabb)
{
	float d = 0;
	for (int i = 0; i < 3; i++)
	{
		if (vPos.m[i] < aabb.Mins.m[i])
		{
			float s = vPos.m[i] - aabb.Mins.m[i];
			d += s * s;
		}
		else if (vPos.m[i] > aabb.Maxs.m[i])
		{
			float s = vPos.m[i] - aabb.Maxs.m[i];
			d += s * s;
		}
	}
	return d;
}

AXMatrix4 a3d_OrthoMatrix(float fWidth, float fHeight, float fZNear, float fZFar)
{
	AXMatrix4 m;
	m._11 = 2.0f / fWidth;
	m._22 = 2.0f / fHeight;
	m._33 = 1.0f / (fZFar - fZNear);
	m._43 = -fZNear * m._33;
	m._44 = 1.0f;
	m._12 = m._13 = m._14 = 0.0f;
	m._21 = m._23 = m._24 = 0.0f;
	m._31 = m._32 = m._34 = 0.0f;
	m._41 = m._42 = 0.0f;
	return m;
}

AXMatrix4 a3d_PerspectiveMatrix(float fWidth, float fHeight, float fZNear, float fZFar)
{
	AXMatrix4 m;
	float f2ZN = 2.0f * fZNear;
	m._11 = f2ZN / fWidth;
	m._22 = f2ZN / fHeight;
	m._33 = fZFar / (fZFar - fZNear);
	m._43 = -fZNear * m._33;
	m._34 = 1.0f;
	m._12 = m._13 = m._14 = 0.0f;
	m._21 = m._23 = m._24 = 0.0f;
	m._31 = m._32 = 0.0f;
	m._41 = m._42 = m._44 = 0.0f;
	return m;
}

AXMatrix4 a3d_PerspectiveFovMatrix(float fFovY, float fAspect, float fZNear, float fZFar)
{
	AXMatrix4 m;
	float fYScale = 1.0f / tanf(fFovY / 2.0f);
	float fXScale = fYScale / fAspect;
	m._11 = fXScale;
	m._22 = fYScale;
	m._33 = fZFar / (fZFar - fZNear);
	m._43 = -fZNear * m._33;
	m._34 = 1.0f;
	m._12 = m._13 = m._14 = 0.0f;
	m._21 = m._23 = m._24 = 0.0f;
	m._31 = m._32 = 0.0f;
	m._41 = m._42 = m._44 = 0.0f;
	return m;
}

AXMatrix4 a3d_OrthoOffCenterMatrix(float fLeft, float fRight, float fBottom, float fTop, float fZNear, float fZFar)
{
	AXMatrix4 m;
	float fWidth = fRight - fLeft;
	float fHeight = fTop - fBottom;
	m._11 = 2.0f / fWidth;
	m._22 = 2.0f / fHeight;
	m._33 = 1.0f / (fZFar - fZNear);
	m._43 = -fZNear * m._33;
	m._44 = 1.0f;
	m._12 = m._13 = m._14 = 0.0f;
	m._21 = m._23 = m._24 = 0.0f;
	m._31 = m._32 = m._34 = 0.0f;
	m._41 = -(fRight + fLeft) / fWidth;
	m._42 = -(fTop + fBottom) / fHeight;
	return m;
}

AXMatrix4 a3d_PerspectiveOffCenterMatrix(float fLeft, float fRight, float fBottom, float fTop, float fZNear, float fZFar)
{
	AXMatrix4 m;
	float fWidth = fRight - fLeft;
	float fHeight = fTop - fBottom;
	float f2ZN = 2.0f * fZNear;
	m._11 = f2ZN / fWidth;
	m._22 = f2ZN / fHeight;
	m._33 = fZFar / (fZFar - fZNear);
	m._43 = -fZNear * m._33;
	m._34 = 1.0f;
	m._12 = m._13 = m._14 = 0.0f;
	m._21 = m._23 = m._24 = 0.0f;
	m._31 = -(fRight + fLeft) / fWidth;
	m._32 = -(fTop + fBottom) / fHeight;
	m._41 = m._42 = m._44 = 0.0f;
	return m;
}

AXMatrix4 a3d_OrthoMatrixRH(float fWidth, float fHeight, float fZNear, float fZFar)
{
	AXMatrix4 m;
	m._11 = 2.0f / fWidth;
	m._22 = 2.0f / fHeight;
	m._33 = 1.0f / (fZNear - fZFar); //
	m._43 = fZNear * m._33; //
	m._44 = 1.0f;
	m._12 = m._13 = m._14 = 0.0f;
	m._21 = m._23 = m._24 = 0.0f;
	m._31 = m._32 = m._34 = 0.0f;
	m._41 = m._42 = 0.0f;
	return m;
}

AXMatrix4 a3d_PerspectiveMatrixRH(float fWidth, float fHeight, float fZNear, float fZFar)
{
	AXMatrix4 m;
	float f2ZN = 2.0f * fZNear;
	m._11 = f2ZN / fWidth;
	m._22 = f2ZN / fHeight;
	m._33 = fZFar / (fZNear - fZFar); //
	m._43 = fZNear * m._33; // 
	m._34 = -1.0f; //
	m._12 = m._13 = m._14 = 0.0f;
	m._21 = m._23 = m._24 = 0.0f;
	m._31 = m._32 = 0.0f;
	m._41 = m._42 = m._44 = 0.0f;
	return m;
}

AXMatrix4 a3d_PerspectiveFovMatrixRH(float fFovY, float fAspect, float fZNear, float fZFar)
{
	AXMatrix4 m;
	float fYScale = 1.0f / tanf(fFovY / 2.0f);
	float fXScale = fYScale / fAspect;
	m._11 = fXScale;
	m._22 = fYScale;
	m._33 = fZFar / (fZNear - fZFar); //
	m._43 = fZNear * m._33; // 
	m._34 = -1.0f; //
	m._12 = m._13 = m._14 = 0.0f;
	m._21 = m._23 = m._24 = 0.0f;
	m._31 = m._32 = 0.0f;
	m._41 = m._42 = m._44 = 0.0f;
	return m;
}

AXMatrix4 a3d_OrthoOffCenterMatrixRH(float fLeft, float fRight, float fBottom, float fTop, float fZNear, float fZFar)
{
	AXMatrix4 m;
	float fWidth = fRight - fLeft;
	float fHeight = fTop - fBottom;
	m._11 = 2.0f / fWidth;
	m._22 = 2.0f / fHeight;
	m._33 = 1.0f / (fZNear - fZFar); //
	m._43 = fZNear * m._33; //
	m._44 = 1.0f;
	m._12 = m._13 = m._14 = 0.0f;
	m._21 = m._23 = m._24 = 0.0f;
	m._31 = m._32 = m._34 = 0.0f;
	m._41 = -(fRight + fLeft) / fWidth;
	m._42 = -(fTop + fBottom) / fHeight;
	return m;
}

AXMatrix4 a3d_PerspectiveOffCenterMatrixRH(float fLeft, float fRight, float fBottom, float fTop, float fZNear, float fZFar)
{
	AXMatrix4 m;
	float fWidth = fRight - fLeft;
	float fHeight = fTop - fBottom;
	float f2ZN = 2.0f * fZNear;
	m._11 = f2ZN / fWidth;
	m._22 = f2ZN / fHeight;
	m._33 = fZFar / (fZNear - fZFar); //
	m._43 = fZNear * m._33; //
	m._34 = -1.0f; //
	m._12 = m._13 = m._14 = 0.0f;
	m._21 = m._23 = m._24 = 0.0f;
	m._31 = (fRight + fLeft) / fWidth; //
	m._32 = (fTop + fBottom) / fHeight; //
	m._41 = m._42 = m._44 = 0.0f;
	return m;
}

AXAABB a3d_TransformAABB(const AXAABB& aabb, const AXMatrix4& mat)
{
	AXAABB aabbNew;
	aabbNew.Clear();
	AXVector3 verts[8];
	aabb.GetVertices(verts, NULL, false);

	for(int j=0; j<8; j++)
	{
		verts[j] = mat.TransformCoord(verts[j]);
		aabbNew.AddVertex(verts[j]);
	}
	aabbNew.CompleteCenterExts();

	return aabbNew;
}

bool a3d_RayIntersectTriangle(const AXVector3& vecStart, const AXVector3& vecDelta, AXVector3 * pTriVerts, float * pvFraction, AXVector3 * pVecPos)
{
	float t, u, v;
	AXVector3 v0, v1, v2;

	static const float ERR_T = 1e-6f;

	v0 = pTriVerts[0];
	v1 = pTriVerts[1];
	v2 = pTriVerts[2];

	// Find vectors for two edges sharing vert0
	AXVector3 edge1 = v1 - v0;
	AXVector3 edge2 = v2 - v0;

	// Begin calculating determinant - also used to calculate U parameter
	AXVector3 pvec;
	pvec = CrossProduct(vecDelta, edge2);

	// If determinant is near zero, ray lies in plane of triangle
	float det = DotProduct(edge1, pvec);
	if( det < ERR_T )
		return false;

	// Calculate distance from vert0 to ray origin
	AXVector3 tvec = vecStart - v0;

	// Calculate U parameter and test bounds
	u = DotProduct(tvec, pvec);
	if( u < -ERR_T || u > det + ERR_T )
		return false;

	// Prepare to test V parameter
	AXVector3 qvec;
	qvec = CrossProduct(tvec, edge1);

	// Calculate V parameter and test bounds
	v = DotProduct(vecDelta, qvec);
	if( v < -ERR_T || u + v > det + ERR_T)
		return false;

	// Calculate t, scale parameters, ray intersects triangle
	t = DotProduct(edge2, qvec);
	float fInvDet = 1.0f / det;
	t *= fInvDet;
	u *= fInvDet;
	v *= fInvDet;

	//If the line is short for touch that triangle;
	if( t < 0.0f || t > 1.0f )
		return false;

	* pvFraction = t;// * 0.99f;
	* pVecPos = vecStart + vecDelta * (* pvFraction);
	return true;
}
/*
AXMatrix4 TransformMatrix(const AXQuaternion& qRotation, const AXVector3& vTranslation, const AXVector3& vScaling)
{
	AXMatrix4 result;
	qRotation.ConvertToMatrix(result);

	result.m[0][0] *= vScaling.x;
	result.m[0][1] *= vScaling.x;
	result.m[0][2] *= vScaling.x;
	result.m[0][3] *= vScaling.x;

	result.m[1][0] *= vScaling.y;
	result.m[1][1] *= vScaling.y;
	result.m[1][2] *= vScaling.y;
	result.m[1][3] *= vScaling.y;

	result.m[2][0] *= vScaling.z;
	result.m[2][1] *= vScaling.z;
	result.m[2][2] *= vScaling.z;
	result.m[2][3] *= vScaling.z;

	result.m[3][0] = vTranslation.x;
	result.m[3][1] = vTranslation.y;
	result.m[3][2] = vTranslation.z;
	result.m[3][3] = 1.f;

	return result;
}
*/
AXMatrix4 a3d_RotateEuler(float xRad, float yRad, float zRad)
{
	float cx, cy, cz, sx, sy, sz;
	cx = cosf(xRad);
	cy = cosf(yRad);
	cz = cosf(zRad);
	sx = sinf(xRad);
	sy = sinf(yRad);
	sz = sinf(zRad);

	return AXMatrix4(cy * cz,                  cy * sz,                -sy,     0,
					 -cx * sz + sx * sy * cz,   cx * cz + sx * sy * sz, sx * cy, 0,
					  sx * sz + cx * sy * cz,  -sx * cz + cx * sy * sz, cx * cy, 0,
					  0,   0,   0,   1);

}

namespace
{

#pragma warning ( disable : 4244 )  // conversion from 'double' to 'float', possible loss of data.

    /**** Decompose.h - Basic declarations ****/
    typedef struct { float x, y, z, w; } Quatern; /* Quaternernion */
    enum QuaternPart { X, Y, Z, W };
    typedef Quatern HVect; /* Homogeneous 3D vector */
    typedef float HMatrix[4][4]; /* Right-handed, for column vectors */
    typedef struct {
        HVect t;  /* Translation components */
        Quatern  q;  /* Essential rotation   */
        Quatern  u;  /* Stretch rotation   */
        HVect k;  /* Stretch factors    */
        float f;  /* Sign of determinant    */
    } SAffineParts;


    float polar_decomp(HMatrix M, HMatrix Q, HMatrix S);
    HVect spect_decomp(HMatrix S, HMatrix U);
    Quatern snuggle(Quatern q, HVect *k);

    /******* Matrix Preliminaries *******/

    /** Fill out 3x3 matrix to 4x4 **/
#define mat_pad(A) (A[W][X]=A[X][W]=A[W][Y]=A[Y][W]=A[W][Z]=A[Z][W]=0,A[W][W]=1)

    /** Copy nxn matrix A to C using "gets" for assignment **/
#define mat_copy(C,gets,A,n) {int i,j; for(i=0;i<n;i++) for(j=0;j<n;j++)\
    C[i][j] gets (A[i][j]);}

    /** Copy transpose of nxn matrix A to C using "gets" for assignment **/
#define mat_tpose(AT,gets,A,n) {int i,j; for(i=0;i<n;i++) for(j=0;j<n;j++)\
    AT[i][j] gets (A[j][i]);}

    /** Assign nxn matrix C the element-wise combination of A and B using "op" **/
#define mat_binop(C,gets,A,op,B,n) {int i,j; for(i=0;i<n;i++) for(j=0;j<n;j++)\
    C[i][j] gets (A[i][j]) op (B[i][j]);}

    /** Multiply the upper left 3x3 parts of A and B to get AB **/
    static  void mat_mult(HMatrix A, HMatrix B, HMatrix AB)
    {
        int i, j;
        for (i = 0; i<3; i++) for (j = 0; j<3; j++)
            AB[i][j] = A[i][0] * B[0][j] + A[i][1] * B[1][j] + A[i][2] * B[2][j];
    }

    /** Return dot product of length 3 vectors va and vb **/
    static  float vdot(float *va, float *vb)
    {
        return (va[0] * vb[0] + va[1] * vb[1] + va[2] * vb[2]);
    }

    /** Set v to cross product of length 3 vectors va and vb **/
    static  void vcross(float *va, float *vb, float *v)
    {
        v[0] = va[1] * vb[2] - va[2] * vb[1];
        v[1] = va[2] * vb[0] - va[0] * vb[2];
        v[2] = va[0] * vb[1] - va[1] * vb[0];
    }

    /** Set MadjT to transpose of inverse of M times determinant of M **/
    static  void adjoint_transpose(HMatrix M, HMatrix MadjT)
    {
        vcross(M[1], M[2], MadjT[0]);
        vcross(M[2], M[0], MadjT[1]);
        vcross(M[0], M[1], MadjT[2]);
    }

    /******* Quaternernion Preliminaries *******/

    /* Construct a (possibly non-unit) Quaternernion from real components. */
    static  Quatern Qt_(float x, float y, float z, float w)
    {
        Quatern qq;
        qq.x = x; qq.y = y; qq.z = z; qq.w = w;
        return (qq);
    }

    /* Return conjugate of Quaternernion. */
    static  Quatern Qt_Conj(Quatern q)
    {
        Quatern qq;
        qq.x = -q.x; qq.y = -q.y; qq.z = -q.z; qq.w = q.w;
        return (qq);
    }

    /* Return Quaternernion product qL * qR.  Note: order is important!
    * To combine rotations, use the product Mul(qSecond, qFirst),
    * which gives the effect of rotating by qFirst then qSecond. */
    static  Quatern Qt_Mul(Quatern qL, Quatern qR)
    {
        Quatern qq;
        qq.w = qL.w*qR.w - qL.x*qR.x - qL.y*qR.y - qL.z*qR.z;
        qq.x = qL.w*qR.x + qL.x*qR.w + qL.y*qR.z - qL.z*qR.y;
        qq.y = qL.w*qR.y + qL.y*qR.w + qL.z*qR.x - qL.x*qR.z;
        qq.z = qL.w*qR.z + qL.z*qR.w + qL.x*qR.y - qL.y*qR.x;
        return (qq);
    }

    /* Return product of Quaternernion q by scalar w. */
    static  Quatern Qt_Scale(Quatern q, float w)
    {
        Quatern qq;
        qq.w = q.w*w; qq.x = q.x*w; qq.y = q.y*w; qq.z = q.z*w;
        return (qq);
    }

    /* Construct a unit Quaternernion from rotation matrix.  Assumes matrix is
    * used to multiply column vector on the left: vnew = mat vold.  Works
    * correctly for right-handed coordinate system and right-handed rotations.
    * Translation and perspective components ignored. */
    static  Quatern Qt_FromMatrix(HMatrix mat)
    {
        /* This algorithm avoids near-zero divides by looking for a large component
        * - first w, then x, y, or z.  When the trace is greater than zero,
        * |w| is greater than 1/2, which is as small as a largest component can be.
        * Otherwise, the largest diagonal entry corresponds to the largest of |x|,
        * |y|, or |z|, one of which must be larger than |w|, and at least 1/2. */
        Quatern qu;
        register double tr, s;

        tr = mat[X][X] + mat[Y][Y] + mat[Z][Z];
        if (tr >= 0.0) {
            s = sqrt(tr + mat[W][W]);
            qu.w = s*0.5;
            s = 0.5 / s;
            qu.x = (mat[Z][Y] - mat[Y][Z]) * s;
            qu.y = (mat[X][Z] - mat[Z][X]) * s;
            qu.z = (mat[Y][X] - mat[X][Y]) * s;
        }
        else {
            int h = X;
            if (mat[Y][Y] > mat[X][X]) h = Y;
            if (mat[Z][Z] > mat[h][h]) h = Z;
            switch (h) {
#define caseMacro(i,j,k,I,J,K) \
      case I:\
    s = sqrt( (mat[I][I] - (mat[J][J]+mat[K][K])) + mat[W][W] );\
    qu.i = s*0.5;\
    s = 0.5 / s;\
    qu.j = (mat[I][J] + mat[J][I]) * s;\
    qu.k = (mat[K][I] + mat[I][K]) * s;\
    qu.w = (mat[K][J] - mat[J][K]) * s;\
    break
                caseMacro(x, y, z, X, Y, Z);
                caseMacro(y, z, x, Y, Z, X);
                caseMacro(z, x, y, Z, X, Y);
            }
        }
        if (mat[W][W] != 1.0) qu = Qt_Scale(qu, 1.0f / sqrt(mat[W][W]));
        return (qu);
    }
    /******* Decomp Auxiliaries *******/

    static HMatrix mat_id = { { 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 1, 0 }, { 0, 0, 0, 1 } };

    /** Compute either the 1 or infinity norm of M, depending on tpose **/
    static  float mat_norm(HMatrix M, int tpose)
    {
        int i;
        float sum, max;
        max = 0.0;
        for (i = 0; i<3; i++) {
            if (tpose) sum = fabs(M[0][i]) + fabs(M[1][i]) + fabs(M[2][i]);
            else     sum = fabs(M[i][0]) + fabs(M[i][1]) + fabs(M[i][2]);
            if (max<sum) max = sum;
        }
        return max;
    }

    static  float norm_inf(HMatrix M) { return mat_norm(M, 0); }
    static  float norm_one(HMatrix M) { return mat_norm(M, 1); }

    /** Return index of column of M containing maximum abs entry, or -1 if M=0 **/
    static  int find_max_col(HMatrix M)
    {
        float abs, max;
        int i, j, col;
        max = 0.0; col = -1;
        for (i = 0; i<3; i++) for (j = 0; j<3; j++) {
            abs = M[i][j]; if (abs<0.0) abs = -abs;
            if (abs>max) { max = abs; col = j; }
        }
        return col;
    }

    /** Setup u for Household reflection to zero all v components but first **/
    static  void make_reflector(float *v, float *u)
    {
        float s = sqrt(vdot(v, v));
        u[0] = v[0]; u[1] = v[1];
        u[2] = v[2] + ((v[2]<0.0) ? -s : s);
        s = sqrt(2.0 / vdot(u, u));
        u[0] = u[0] * s; u[1] = u[1] * s; u[2] = u[2] * s;
    }

    /** Apply Householder reflection represented by u to column vectors of M **/
    static  void reflect_cols(HMatrix M, float *u)
    {
        int i, j;
        for (i = 0; i<3; i++) {
            float s = u[0] * M[0][i] + u[1] * M[1][i] + u[2] * M[2][i];
            for (j = 0; j<3; j++) M[j][i] -= u[j] * s;
        }
    }
    /** Apply Householder reflection represented by u to row vectors of M **/
    static  void reflect_rows(HMatrix M, float *u)
    {
        int i, j;
        for (i = 0; i<3; i++) {
            float s = vdot(u, M[i]);
            for (j = 0; j<3; j++) M[i][j] -= u[j] * s;
        }
    }

    /** Find orthogonal factor Q of rank 1 (or less) M **/
    static  void do_rank1(HMatrix M, HMatrix Q)
    {
        float v1[3], v2[3], s;
        int col;
        mat_copy(Q, = , mat_id, 4);
        /* If rank(M) is 1, we should find a non-zero column in M */
        col = find_max_col(M);
        if (col<0) return; /* Rank is 0 */
        v1[0] = M[0][col]; v1[1] = M[1][col]; v1[2] = M[2][col];
        make_reflector(v1, v1); reflect_cols(M, v1);
        v2[0] = M[2][0]; v2[1] = M[2][1]; v2[2] = M[2][2];
        make_reflector(v2, v2); reflect_rows(M, v2);
        s = M[2][2];
        if (s<0.0) Q[2][2] = -1.0;
        reflect_cols(Q, v1); reflect_rows(Q, v2);
    }

    /** Find orthogonal factor Q of rank 2 (or less) M using adjoint transpose **/
    static  void do_rank2(HMatrix M, HMatrix MadjT, HMatrix Q)
    {
        float v1[3], v2[3];
        float w, x, y, z, c, s, d;
        int col;
        /* If rank(M) is 2, we should find a non-zero column in MadjT */
        col = find_max_col(MadjT);
        if (col<0) { do_rank1(M, Q); return; } /* Rank<2 */
        v1[0] = MadjT[0][col]; v1[1] = MadjT[1][col]; v1[2] = MadjT[2][col];
        make_reflector(v1, v1); reflect_cols(M, v1);
        vcross(M[0], M[1], v2);
        make_reflector(v2, v2); reflect_rows(M, v2);
        w = M[0][0]; x = M[0][1]; y = M[1][0]; z = M[1][1];
        if (w*z>x*y) {
            c = z + w; s = y - x; d = sqrt(c*c + s*s); c = c / d; s = s / d;
            Q[0][0] = Q[1][1] = c; Q[0][1] = -(Q[1][0] = s);
        }
        else {
            c = z - w; s = y + x; d = sqrt(c*c + s*s); c = c / d; s = s / d;
            Q[0][0] = -(Q[1][1] = c); Q[0][1] = Q[1][0] = s;
        }
        Q[0][2] = Q[2][0] = Q[1][2] = Q[2][1] = 0.0; Q[2][2] = 1.0;
        reflect_cols(Q, v1); reflect_rows(Q, v2);
    }


    /******* Polar Decomposition *******/

    /* Polar Decomposition of 3x3 matrix in 4x4,
    * M = QS.  See Nicholas Higham and Robert S. Schreiber,
    * Fast Polar Decomposition of An Arbitrary Matrix,
    * Technical Report 88-942, October 1988,
    * Department of Computer Science, Cornell University.
    */
    float polar_decomp(HMatrix M, HMatrix Q, HMatrix S)
    {
#define TOL 1.0e-6
        HMatrix Mk, MadjTk, Ek;
        float det, M_one, M_inf, MadjT_one, MadjT_inf, E_one, gamma, g1, g2;
        int i, j;
        mat_tpose(Mk, = , M, 3);
        M_one = norm_one(Mk);  M_inf = norm_inf(Mk);
        do {
            adjoint_transpose(Mk, MadjTk);
            det = vdot(Mk[0], MadjTk[0]);
            if (det == 0.0) { do_rank2(Mk, MadjTk, Mk); break; }
            MadjT_one = norm_one(MadjTk); MadjT_inf = norm_inf(MadjTk);
            gamma = sqrt(sqrt((MadjT_one*MadjT_inf) / (M_one*M_inf)) / fabs(det));
            g1 = gamma*0.5;
            g2 = 0.5 / (gamma*det);
            mat_copy(Ek, = , Mk, 3);
            mat_binop(Mk, = , g1*Mk, +, g2*MadjTk, 3);
            mat_copy(Ek, -= , Mk, 3);
            E_one = norm_one(Ek);
            M_one = norm_one(Mk);  M_inf = norm_inf(Mk);
        } while (E_one>(M_one*TOL));
        mat_tpose(Q, = , Mk, 3); mat_pad(Q);
        mat_mult(Mk, M, S);  mat_pad(S);
        for (i = 0; i<3; i++) for (j = i; j<3; j++)
            S[i][j] = S[j][i] = 0.5*(S[i][j] + S[j][i]);
        return (det);
    }


    /******* Spectral Decomposition *******/

    /* Compute the spectral decomposition of symmetric positive semi-definite S.
    * Returns rotation in U and scale factors in result, so that if K is a diagonal
    * matrix of the scale factors, then S = U K (U transpose). Uses Jacobi method.
    * See Gene H. Golub and Charles F. Van Loan. Matrix Computations. Hopkins 1983.
    */
    HVect spect_decomp(HMatrix S, HMatrix U)
    {
        HVect kv;
        double Diag[3], OffD[3]; /* OffD is off-diag (by omitted index) */
        double g, h, fabsh, fabsOffDi, t, theta, c, s, tau, ta, OffDq, a, b;
        static char nxt[] = { Y, Z, X };
        int sweep, i, j;
        mat_copy(U, = , mat_id, 4);
        Diag[X] = S[X][X]; Diag[Y] = S[Y][Y]; Diag[Z] = S[Z][Z];
        OffD[X] = S[Y][Z]; OffD[Y] = S[Z][X]; OffD[Z] = S[X][Y];
        for (sweep = 20; sweep>0; sweep--) {
            float sm = fabs(OffD[X]) + fabs(OffD[Y]) + fabs(OffD[Z]);
            if (sm == 0.0) break;
            for (i = Z; i >= X; i--) {
                int p = nxt[i]; int q = nxt[p];
                fabsOffDi = fabs(OffD[i]);
                g = 100.0*fabsOffDi;
                if (fabsOffDi>0.0) {
                    h = Diag[q] - Diag[p];
                    fabsh = fabs(h);
                    if (fabsh + g == fabsh) {
                        t = OffD[i] / h;
                    }
                    else {
                        theta = 0.5*h / OffD[i];
                        t = 1.0 / (fabs(theta) + sqrt(theta*theta + 1.0));
                        if (theta<0.0) t = -t;
                    }
                    c = 1.0 / sqrt(t*t + 1.0); s = t*c;
                    tau = s / (c + 1.0);
                    ta = t*OffD[i]; OffD[i] = 0.0;
                    Diag[p] -= ta; Diag[q] += ta;
                    OffDq = OffD[q];
                    OffD[q] -= s*(OffD[p] + tau*OffD[q]);
                    OffD[p] += s*(OffDq - tau*OffD[p]);
                    for (j = Z; j >= X; j--) {
                        a = U[j][p]; b = U[j][q];
                        U[j][p] -= s*(b + tau*a);
                        U[j][q] += s*(a - tau*b);
                    }
                }
            }
        }
        kv.x = Diag[X]; kv.y = Diag[Y]; kv.z = Diag[Z]; kv.w = 1.0;
        return (kv);
    }

    /******* Spectral Axis Adjustment *******/

    /* Given a unit Quaternernion, q, and a scale vector, k, find a unit Quaternernion, p,
    * which permutes the axes and turns freely in the plane of duplicate scale
    * factors, such that q p has the largest possible w component, i.e. the
    * smallest possible angle. Permutes k's components to go with q p instead of q.
    * See Ken Shoemake and Tom Duff. Matrix Animation and Polar Decomposition.
    * Proceedings of Graphics Interface 1992. Details on p. 262-263.
    */
    Quatern snuggle(Quatern q, HVect *k)
    {
#define SQRTHALF (0.7071067811865475244f)
#define sgn(n,v)    ((n)?-(v):(v))
#define swap(a,i,j) {a[3]=a[i]; a[i]=a[j]; a[j]=a[3];}
#define cycle(a,p)  if (p) {a[3]=a[0]; a[0]=a[1]; a[1]=a[2]; a[2]=a[3];}\
                else   {a[3]=a[2]; a[2]=a[1]; a[1]=a[0]; a[0]=a[3];}
        Quatern p;
        float ka[4];
        int i, turn = -1;
        ka[X] = k->x; ka[Y] = k->y; ka[Z] = k->z;
        if (ka[X] == ka[Y]) { if (ka[X] == ka[Z]) turn = W; else turn = Z; }
        else { if (ka[X] == ka[Z]) turn = Y; else if (ka[Y] == ka[Z]) turn = X; }
        if (turn >= 0) {
            Quatern qtoz, qp;
            unsigned neg[3], win;
            double mag[3], t;
            static Quatern qxtoz = { 0, SQRTHALF, 0, SQRTHALF };
            static Quatern qytoz = { SQRTHALF, 0, 0, SQRTHALF };
            static Quatern qppmm = { 0.5, 0.5, -0.5, -0.5 };
            static Quatern qpppp = { 0.5, 0.5, 0.5, 0.5 };
            static Quatern qmpmm = { -0.5, 0.5, -0.5, -0.5 };
            static Quatern qpppm = { 0.5, 0.5, 0.5, -0.5 };
            static Quatern q0001 = { 0.0, 0.0, 0.0, 1.0 };
            static Quatern q1000 = { 1.0, 0.0, 0.0, 0.0 };
            switch (turn) {
            default: return (Qt_Conj(q));
            case X: q = Qt_Mul(q, qtoz = qxtoz); swap(ka, X, Z); break;
            case Y: q = Qt_Mul(q, qtoz = qytoz); swap(ka, Y, Z); break;
            case Z: qtoz = q0001; break;
            }
            q = Qt_Conj(q);
            mag[0] = (double)q.z*q.z + (double)q.w*q.w - 0.5;
            mag[1] = (double)q.x*q.z - (double)q.y*q.w;
            mag[2] = (double)q.y*q.z + (double)q.x*q.w;
            for (i = 0; i<3; i++) if (neg[i] = (mag[i]<0.0)) mag[i] = -mag[i];
            if (mag[0]>mag[1]) { if (mag[0]>mag[2]) win = 0; else win = 2; }
            else       { if (mag[1]>mag[2]) win = 1; else win = 2; }
            switch (win) {
            case 0: if (neg[0]) p = q1000; else p = q0001; break;
            case 1: if (neg[1]) p = qppmm; else p = qpppp; cycle(ka, 0); break;
            case 2: if (neg[2]) p = qmpmm; else p = qpppm; cycle(ka, 1); break;
            }
            qp = Qt_Mul(q, p);
            t = sqrt(mag[win] + 0.5);
            p = Qt_Mul(p, Qt_(0.0, 0.0, -qp.z / t, qp.w / t));
            p = Qt_Mul(qtoz, Qt_Conj(p));
        }
        else {
            float qa[4], pa[4];
            unsigned lo, hi, neg[4], par = 0;
            double all, big, two;
            qa[0] = q.x; qa[1] = q.y; qa[2] = q.z; qa[3] = q.w;
            for (i = 0; i<4; i++) {
                pa[i] = 0.0;
                if (neg[i] = (qa[i]<0.0)) qa[i] = -qa[i];
                par ^= neg[i];
            }
            /* Find two largest components, indices in hi and lo */
            if (qa[0]>qa[1]) lo = 0; else lo = 1;
            if (qa[2]>qa[3]) hi = 2; else hi = 3;
            if (qa[lo]>qa[hi]) {
                if (qa[lo ^ 1]>qa[hi]) { hi = lo; lo ^= 1; }
                else { hi ^= lo; lo ^= hi; hi ^= lo; }
            }
            else { if (qa[hi ^ 1]>qa[lo]) lo = hi ^ 1; }
            all = (qa[0] + qa[1] + qa[2] + qa[3])*0.5;
            two = (qa[hi] + qa[lo])*SQRTHALF;
            big = qa[hi];
            if (all>two) {
                if (all>big) {/*all*/
                    {int i; for (i = 0; i<4; i++) pa[i] = sgn(neg[i], 0.5); }
                    cycle(ka, par)
                }
                else {/*big*/ pa[hi] = sgn(neg[hi], 1.0); }
            }
            else {
                if (two>big) {/*two*/
                    pa[hi] = sgn(neg[hi], SQRTHALF); pa[lo] = sgn(neg[lo], SQRTHALF);
                    if (lo>hi) { hi ^= lo; lo ^= hi; hi ^= lo; }
                    if (hi == W) { hi = "\001\002\000"[lo]; lo = 3 - hi - lo; }
                    swap(ka, hi, lo)
                }
                else {/*big*/ pa[hi] = sgn(neg[hi], 1.0); }
            }
            p.x = -pa[0]; p.y = -pa[1]; p.z = -pa[2]; p.w = pa[3];
        }
        k->x = ka[X]; k->y = ka[Y]; k->z = ka[Z];
        return (p);
    }


    /******* Decompose Affine Matrix *******/

    /* Decompose 4x4 affine matrix A as TFRUK(U transpose), where t contains the
    * translation components, q contains the rotation R, u contains U, k contains
    * scale factors, and f contains the sign of the determinant.
    * Assumes A transforms column vectors in right-handed coordinates.
    * See Ken Shoemake and Tom Duff. Matrix Animation and Polar Decomposition.
    * Proceedings of Graphics Interface 1992.
    */
    static  void decomp_affine(HMatrix A, SAffineParts *parts)
    {
        HMatrix Q, S, U;
        Quatern p;
        float det;
        parts->t = Qt_(A[X][W], A[Y][W], A[Z][W], 0);
        det = polar_decomp(A, Q, S);
        if (det<0.0) {
            mat_copy(Q, = , -Q, 3);
            parts->f = -1;
        }
        else parts->f = 1;
        parts->q = Qt_FromMatrix(Q);
        parts->k = spect_decomp(S, U);
        parts->u = Qt_FromMatrix(U);
        p = snuggle(parts->u, &parts->k);
        parts->u = Qt_Mul(parts->u, p);
    }

    static  void decomp_affine_discard_scale(HMatrix A, SAffineParts *parts)
    {
        HMatrix Q, S;
        float det;

        parts->t = Qt_(A[X][W], A[Y][W], A[Z][W], 0);
        det = polar_decomp(A, Q, S);

        if (det<0.0) {
            mat_copy(Q, = , -Q, 3);
            parts->f = -1;
        }
        else parts->f = 1;

        parts->q = Qt_FromMatrix(Q);
//         parts->k = spect_decomp(S, U);
//         parts->u = Qt_FromMatrix(U);
    }

}

/*
void a3d_DecomposeStrechedMatrix(const AXMatrix4& mat, AXVector3& scale, AXQuaternion& rotate, AXVector3& pos, AXQuaternion& scaleRotate, afloat32& sign)
{
    SAffineParts parts;

    AXMatrix4 tm44(mat);
    HMatrix &H = *((HMatrix*)&tm44); // Treat HMatrix as a Matrix44.
    for (int i = 0; i < 4; ++i)
    {
        for (int j = i + 1; j < 4; ++j)
        {
            float a = tm44.m[i][j];
            tm44.m[i][j] = tm44.m[j][i];
            tm44.m[j][i] = a;
        }
    }

    decomp_affine(H, &parts);

    rotate = AXQuaternion(parts.q.w, parts.q.x, parts.q.y, parts.q.z);
    scaleRotate = AXQuaternion(parts.u.w, parts.u.x, parts.u.y, parts.u.z);
    pos = AXVector3(parts.t.x, parts.t.y, parts.t.z);
    scale = AXVector3(parts.k.x, parts.k.y, parts.k.z);
    sign = parts.f;
}

void a3d_DecomposeStrechedMatrixDiscardScale(const AXMatrix4& mat, AXQuaternion& rotate, AXVector3& pos, afloat32& sign)
{
    SAffineParts parts;

    AXMatrix4 tm44(mat);
    HMatrix &H = *((HMatrix*)&tm44); // Treat HMatrix as a Matrix44.
    for (int i = 0; i < 4; ++i)
    {
        for (int j = i + 1; j < 4; ++j)
        {
            float a = tm44.m[i][j];
            tm44.m[i][j] = tm44.m[j][i];
            tm44.m[j][i] = a;
        }
    }

    decomp_affine_discard_scale(H, &parts);

    rotate = AXQuaternion(parts.q.w, parts.q.x, parts.q.y, parts.q.z);
    pos = AXVector3(parts.t.x, parts.t.y, parts.t.z);
    sign = parts.f;
}

*/