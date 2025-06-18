//	Copyright(c) 2020, Andy Du, All Rights Reserved.
// 	Contact: eaglehorn58@gmail.com


#ifndef _AXFUNCS_H_
#define _AXFUNCS_H_

//#include "ASys.h"
#include "A3DMacros.h"
#include "AXGeometry.h"

//	Fast inverse square root calculation
float a3d_InvSqrt(float v);
//	Length-related functions
inline float a3d_SquareMagnitude(const AXVector3& v) {return v.x * v.x + v.y * v.y + v.z * v.z;}
inline float a3d_Magnitude(const AXVector3& v) {return (float)(sqrt(v.x * v.x + v.y * v.y + v.z * v.z));}
inline float a3d_MagnitudeH(const AXVector3& v) {return (float)(sqrt(v.x * v.x + v.z * v.z));}

inline AXVector3 a3d_Vector423(const AXVector4& v) {return AXVector3(v.x / v.w, v.y / v.w, v.z / v.w);}

//	Returns vector with same direction and unit length
inline AXVector3 a3d_Normalize(const AXVector3& v)
{
	float mag = v.Magnitude();
	if (mag < 1e-12	&& mag > -1e-12)
		return AXVector3(0.0f);
	else
		return v / mag;
}

//	Compress horizontal direction to a byte
unsigned char a3d_CompressDirH(float x, float z);
//	Decompress horizontal direction
AXVector3 a3d_DecompressDirH(unsigned char byDir);
//	Compress direction to two bytes
void a3d_CompressDir(const AXVector3& v, unsigned char& b1, unsigned char& b2);
//	Decompress direction from two bytes
AXVector3 a3d_DecompressDir(unsigned char b1, unsigned char b2);

//	Return min/max vector composed with min/max component of the input 2 vector
AXVector3 a3d_VecMin(const AXVector3& v1, const AXVector3& v2); 
AXVector3 a3d_VecMax(const AXVector3& v1, const AXVector3& v2); 

//	Convert a vector from view coordinates to world coordinates
AXVector3 a3d_ViewToWorld(const AXVector3& vIn, AXMatrix4& matView);

//	Matrix initialize;
AXMatrix3 a3d_IdentityMatrix3();
AXMatrix4 a3d_IdentityMatrix();
AXMatrix4 a3d_ZeroMatrix();
AXMatrix4 a3d_ViewMatrix(const AXVector3& from, const AXVector3& dir, const AXVector3& vecUp, float roll);
AXMatrix4 a3d_LookAtMatrix(const AXVector3& from, const AXVector3& to, const AXVector3& vecUp, float roll);
AXMatrix4 a3d_TransformMatrix(const AXVector3& vecDir, const AXVector3& vecUp, const AXVector3& vecPos);

//	Matrix manipulation;
AXMatrix4 a3d_RotateX(float vRad);
AXMatrix4 a3d_RotateX(const AXMatrix4& mat, float vRad);
AXMatrix4 a3d_RotateY(float vRad);
AXMatrix4 a3d_RotateY(const AXMatrix4& mat, float vRad);
AXMatrix4 a3d_RotateZ(float vRad);
AXMatrix4 a3d_RotateZ(const AXMatrix4& mat, float vRad);
AXMatrix4 a3d_RotateAxis(const AXVector3& vecAxis, float vRad);
AXMatrix4 a3d_RotateAxis(const AXVector3& vecPos, const AXVector3& vecAxis, float vRad);
AXMatrix4 a3d_RotateEuler(float xRad, float yRad, float zRad); // result = RotX * RotY * RotZ

//	Position and vector rotating routines
AXVector3 a3d_RotatePosAroundX(const AXVector3& vPos, float fRad);
AXVector3 a3d_RotatePosAroundY(const AXVector3& vPos, float fRad);
AXVector3 a3d_RotatePosAroundZ(const AXVector3& vPos, float fRad);
AXVector3 a3d_RotatePosAroundAxis(const AXVector3& vPos, const AXVector3& vAxis, float fRad);
AXVector3 a3d_RotatePosAroundLine(const AXVector3& vPos, const AXVector3& vOrigin, const AXVector3& vDir, float fRad);
AXVector3 a3d_RotateVecAroundLine(const AXVector3& v, const AXVector3& vOrigin, const AXVector3& vDir, float fRad);

AXMatrix4 a3d_Scaling(float sx, float sy, float sz);
AXMatrix4 a3d_Scaling(const AXMatrix4& mat, float sx, float sy, float sz);
AXMatrix4 a3d_ScalingRelative(const AXMatrix4& matRoot, float sx, float sy, float sz);
AXMatrix4 a3d_Translate(float x, float y, float z);
AXMatrix4 a3d_ScaleAlongAxis(const AXVector3& vAxis, float fScale);

AXMatrix4 a3d_Transpose(const AXMatrix4& mat);
void a3d_Transpose(const AXMatrix4& matIn, AXMatrix4* pmatOut);

/*
void a3d_DecomposeMatrix(const AXMatrix4& mat, AXVector3& scale, AXQuaternion& orient, AXVector3& pos);

// Decompose affine matrix that might have shear.
// mat = transpose(scaleRotate) * scale * scaleRotate * rotate * sign * translate
// Ken Shoemake and Tom Duff. Matrix Animation and Polar Decomposition.
// Proceedings of Graphics Interface 1992.
void a3d_DecomposeStrechedMatrix(const AXMatrix4& mat, AXVector3& scale, AXQuaternion& rotate, AXVector3& pos, AXQuaternion& scaleRotate, afloat32& sign);

// Decompose affine matrix faster than previous, discard scale and scaleRotate
void a3d_DecomposeStrechedMatrixDiscardScale(const AXMatrix4& mat, AXQuaternion& rotate, AXVector3& pos, afloat32& sign);
*/

// Get the dir and up of a view within the cube map
// 0 ---- right
// 1 ---- left
// 2 ---- top
// 3 ---- bottom
// 4 ---- front
// 5 ---- back
bool a3d_GetCubeMapDirAndUp(int nFaceIndex, AXVector3 * pDir, AXVector3 * pUp);

//Color Value and Color RGB Convertion;
A3DCOLOR a3d_ColorValueToColorRGBA(A3DCOLORVALUE colorValue);
A3DCOLORVALUE a3d_ColorRGBAToColorValue(A3DCOLOR colorRGBA);

AXMatrix4 a3d_MirrorMatrix(const AXVector3 &p, const AXVector3 &n);

//	Note: a3d_InverseTM can only be used on transform matrix. A transform matrix is a
//	translation, rotation or scale matrix, it also can be a matrix combined by these
//	three type matrices. For calcuating an arbitrary matrix's inverse, use a3d_InverseMatrix
AXMatrix4 a3d_InverseTM(const AXMatrix4& mat);
void a3d_InverseTM(const AXMatrix4& matIn, AXMatrix4* pmatOut);
AXMatrix4 a3d_InverseMatrix(const AXMatrix4& mat);
void a3d_InverseMatrix(const AXMatrix4& matIn, AXMatrix4* pmatOut);
AXMatrix4 a3d_InverseAffineMatrix(const AXMatrix4& mat);
void a3d_InverseAffineMatrix(const AXMatrix4& matIn, AXMatrix4* pmatOut);

//  Projection (From Angelica 4)
AXMatrix4 a3d_OrthoMatrix(float fWidth, float fHeight, float fZNear, float fZFar);
AXMatrix4 a3d_OrthoOffCenterMatrix(float fLeft, float fRight, float fBottom, float fTop, float fZNear, float fZFar);
AXMatrix4 a3d_PerspectiveMatrix(float fWidth, float fHeight, float fZNear, float fZFar);
AXMatrix4 a3d_PerspectiveFovMatrix(float fFovY, float fAspect, float fZNear, float fZFar);
AXMatrix4 a3d_PerspectiveOffCenterMatrix(float fLeft, float fRight, float fBottom, float fTop, float fZNear, float fZFar);

AXMatrix4 a3d_OrthoMatrixRH(float fWidth, float fHeight, float fZNear, float fZFar);
AXMatrix4 a3d_OrthoOffCenterMatrixRH(float fLeft, float fRight, float fBottom, float fTop, float fZNear, float fZFar);
AXMatrix4 a3d_PerspectiveMatrixRH(float fWidth, float fHeight, float fZNear, float fZFar);
AXMatrix4 a3d_PerspectiveFovMatrixRH(float fFovY, float fAspect, float fZNear, float fZFar);
AXMatrix4 a3d_PerspectiveOffCenterMatrixRH(float fLeft, float fRight, float fBottom, float fTop, float fZNear, float fZFar);


//	AABB operations
void a3d_ClearAABB(AXVector3& vMins, AXVector3& vMaxs);
void a3d_AddVertexToAABB(AXVector3& vMins, AXVector3& vMaxs, AXVector3& vPoint);
void a3d_ExpandAABB(AXVector3& vecMins, AXVector3& vecMaxs, const AXOBB& obb);
void a3d_ExpandAABB(AXVector3& vecMins, AXVector3& vecMaxs, const AXAABB& subAABB);

inline void a3d_BuildAABB(AXAABB* pAABB, const AXOBB& obb) { pAABB->Build(obb); }
inline void a3d_MergeAABB(AXAABB* pAABB, const AXAABB& subAABB) { pAABB->Merge(subAABB); }
inline void a3d_CompleteAABB(AXAABB* pAABB) { pAABB->CompleteCenterExts(); }

//	OBB operations;
inline void a3d_CompleteOBB(AXOBB* pOBB) { pOBB->CompleteExtAxis(); }

AXVector3 a3d_RandDirH();

//	Convert a color from HSV to RGB
//	h, s, v is in [0.0f, 1.0f]
void a3d_HSVToRGB(A3DHSVCOLORVALUE& hsvColor, A3DCOLOR * pRgbColor);
//	Convert a color from RGB to HSV
//	h, s, v is in [0.0f, 1.0f]
void a3d_RGBToHSV(A3DCOLOR rgbColor, A3DHSVCOLORVALUE * pHsvColor);
void a3d_RGBToHSL(float r, float g, float b, float&  h, float& s, float& l);
//	Convert a color from RGB to seperate r, g, b value;
void a3d_DecompressColor(A3DCOLOR rgbColor, unsigned char * pR, unsigned char * pG, unsigned char * pB, unsigned char *pA=NULL);

AXMatrix3 a3d_Matrix3Rotate(float fRad);
AXMatrix3 a3d_Matrix3Rotate(const AXMatrix3& mat, float fRad);
AXMatrix3 a3d_Matrix3Translate(float x, float y);
AXMatrix3 a3d_Matrix3Translate(const AXMatrix3& mat, float x, float y);

// D3DVERTEXELEMENT9 a3d_SetVSDecl(WORD Stream, WORD Offset, BYTE Type, BYTE Method,BYTE Usage, BYTE UsageIndex);
//	Build standard vertex shader declarator from vertex type
/*void a3d_MakeVSDeclarator(A3DVERTEXTYPE VertexType, int iStreamIdx, D3DVERTEXELEMENT9* pDecl);*/
//	Build vertex shader declarator from FVF
/*void a3d_MakeVSDeclarator(auint32 dwFVF, D3DVERTEXELEMENT9* pDecl);*/

//	Multiply a vertex but don't consider translation factors (the fourth row of matrix),
//	this function can be used to transform a normal. 
AXVector3 a3d_VectorMatrix3x3(const AXVector3& v, const AXMatrix4& mat);

// squared minimum distance from point to aabb 
float a3d_DistSquareToAABB(const AXVector3& vPos, const AXAABB& aabb);
AXAABB a3d_TransformAABB(const AXAABB& aabb, const AXMatrix4& mat);

bool a3d_RayIntersectTriangle(const AXVector3& vecStart, const AXVector3& vecDelta, AXVector3 * pTriVerts, float * pvFraction, AXVector3 * pVecPos);


// some new math functions
// added by yangliuqing 2015-04-20

template <typename T>
inline T Sqr(const T& value)
{
	return value * value;
}

inline float Sqrt(float fValue)
{
	return sqrtf(fValue);
}

//	make transform from scaling, rotation and translation
//	AXMatrix4 TransformMatrix(const AXQuaternion& qRotation, const AXVector3& vTranslation, const AXVector3& vScaling);

// linear interpolation
template <typename T>
inline T Lerp(const T& left, const T& right, afloat32 fT)
{
	return left + (right - left) * fT;
}

// fraction between left and right, floating point format only
template <typename T>
inline T Fraction(T fLeft, T fRight, T fValue)
{
	return (fValue - fLeft) / (fRight - fLeft);
}

// end here

////////////////////////////////////////////////////////////////////////////
//
//	Below functions are only remained for the compatibility with old 
//	Angelica applications and games, so don't use them again when developing
//	new applications and games.
//
////////////////////////////////////////////////////////////////////////////

inline float SquareMagnitude(const AXVector3& v) { return a3d_SquareMagnitude(v); }
inline float Magnitude(const AXVector3& v) { return a3d_Magnitude(v); }
inline float MagnitudeH(const AXVector3& v) { return a3d_MagnitudeH(v); }
inline AXVector3 Vector423(const AXVector4& v) { return a3d_Vector423(v); }
inline AXVector3 Normalize(const AXVector3& v) { return a3d_Normalize(v); }
inline AXVector3 VecMin(const AXVector3& v1, const AXVector3& v2) { return a3d_VecMin(v1, v2); }
inline AXVector3 VecMax(const AXVector3& v1, const AXVector3& v2) { return a3d_VecMax(v1, v2); }
inline AXVector3 ViewToWorld(AXVector3& vIn, AXMatrix4& matView) { return a3d_ViewToWorld(vIn, matView); }
inline AXMatrix3 IdentityMatrix3() { return a3d_IdentityMatrix3(); }
inline AXMatrix4 IdentityMatrix() { return a3d_IdentityMatrix(); }
inline AXMatrix4 ZeroMatrix() { return a3d_ZeroMatrix(); }
inline AXMatrix4 ViewMatrix(const AXVector3& from, const AXVector3& dir, const AXVector3& vecUp, float roll) { return a3d_ViewMatrix(from, dir, vecUp, roll); }
inline AXMatrix4 LookAtMatrix(const AXVector3& from, const AXVector3& to, const AXVector3& vecUp, float roll) { return a3d_LookAtMatrix(from, to, vecUp, roll); }
inline AXMatrix4 TransformMatrix(const AXVector3& vecDir, const AXVector3& vecUp, const AXVector3& vecPos) { return a3d_TransformMatrix(vecDir, vecUp, vecPos); }
inline AXMatrix4 RotateX(float vRad) { return a3d_RotateX(vRad); }
inline AXMatrix4 RotateX(const AXMatrix4& mat, float vRad) { return a3d_RotateX(mat, vRad); }
inline AXMatrix4 RotateY(float vRad) { return a3d_RotateY(vRad); }
inline AXMatrix4 RotateY(const AXMatrix4& mat, float vRad) { return a3d_RotateY(mat, vRad); }
inline AXMatrix4 RotateZ(float vRad) { return a3d_RotateZ(vRad); }
inline AXMatrix4 RotateZ(const AXMatrix4& mat, float vRad) { return a3d_RotateZ(mat, vRad); }
inline AXMatrix4 RotateAxis(const AXVector3& vecAxis, float vRad) { return a3d_RotateAxis(vecAxis, vRad); }
inline AXMatrix4 RotateAxis(const AXVector3& vecPos, const AXVector3& vecAxis, float vRad) { return a3d_RotateAxis(vecPos, vecAxis, vRad); }
inline AXMatrix4 RotateEuler(const AXVector3& eulerAngle) { return a3d_RotateEuler(eulerAngle.x, eulerAngle.y, eulerAngle.z); }
inline AXMatrix4 Scaling(float sx, float sy, float sz) { return a3d_Scaling(sx, sy, sz); }
inline AXMatrix4 Scaling(const AXMatrix4& mat, float sx, float sy, float sz) { return a3d_Scaling(mat, sx, sy, sz); }
inline AXMatrix4 Translate(float x, float y, float z) { return a3d_Translate(x, y, z); }
inline AXMatrix4 Transpose(const AXMatrix4& mat) { return a3d_Transpose(mat); }
inline void Transpose(const AXMatrix4& matIn, AXMatrix4* pmatOut) { a3d_Transpose(matIn, pmatOut); }
inline bool GetCubeMapDirAndUp(int nFaceIndex, AXVector3 * pDir, AXVector3 * pUp) { return a3d_GetCubeMapDirAndUp(nFaceIndex, pDir, pUp); }
inline A3DCOLOR ColorValueToColorRGBA(A3DCOLORVALUE colorValue) { return a3d_ColorValueToColorRGBA(colorValue); }
inline A3DCOLORVALUE ColorRGBAToColorValue(A3DCOLOR colorRGBA) { return a3d_ColorRGBAToColorValue(colorRGBA); }
inline AXMatrix4 MirrorMatrix(const AXVector3 &p, const AXVector3 &n) { return a3d_MirrorMatrix(p, n); }
inline AXMatrix4 InverseTM(const AXMatrix4& mat) { return a3d_InverseTM(mat); }
inline void InverseTM(const AXMatrix4& matIn, AXMatrix4* pmatOut) { a3d_InverseTM(matIn, pmatOut); }
inline void ClearAABB(AXVector3& vMins, AXVector3& vMaxs) { a3d_ClearAABB(vMins, vMaxs); }
inline void AddVertexToAABB(AXVector3& vMins, AXVector3& vMaxs, AXVector3& vPoint) { a3d_AddVertexToAABB(vMins, vMaxs, vPoint); }
inline void ExpandAABB(AXVector3& vecMins, AXVector3& vecMaxs, const AXOBB& obb) { a3d_ExpandAABB(vecMins, vecMaxs, obb); }
inline void ExpandAABB(AXVector3& vecMins, AXVector3& vecMaxs, const AXAABB& subAABB) { a3d_ExpandAABB(vecMins, vecMaxs, subAABB); }
inline void ExpandAABB(AXAABB* pAABB, const AXOBB& obb) { a3d_BuildAABB(pAABB, obb); }
inline void ExpandAABB(AXAABB* pAABB, const AXAABB& subAABB) { a3d_MergeAABB(pAABB, subAABB); }
inline void CompleteAABB(AXAABB* pAABB) { a3d_CompleteAABB(pAABB); }
inline void CompleteOBB(AXOBB* pOBB) { a3d_CompleteOBB(pOBB); }
inline AXVector3 RandDirH() { return a3d_RandDirH(); }
inline void HSVToRGB(A3DHSVCOLORVALUE& hsvColor, A3DCOLOR * pRgbColor) { a3d_HSVToRGB(hsvColor, pRgbColor); }
inline void RGBToHSV(A3DCOLOR rgbColor, A3DHSVCOLORVALUE * pHsvColor) { a3d_RGBToHSV(rgbColor, pHsvColor); }
inline void DecompressColor(A3DCOLOR rgbColor, unsigned char * pR, unsigned char * pG, unsigned char * pB, unsigned char *pA=NULL) { a3d_DecompressColor(rgbColor, pR, pG, pB, pA); }

#endif	//	_AXFUNCS_H_
