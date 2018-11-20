/*************************************************************************\
* Õ˚”—“˝«Ê1.0
* Copyright (c) 2018-2018 authored by ÷Ïº”∫∆
* 4x4æÿ’Û
\*************************************************************************/
#ifndef __VAYO_MATRIX4X4_H__
#define __VAYO_MATRIX4X4_H__

#include "VayoAabbox3d.h"
NS_VAYO_BEGIN

// 4x4 matrix. Mostly used as transformation matrix for 3d calculations.
class _VayoExport Matrix4x4
{
public:
	Matrix4x4();

	// Simple operator for directly accessing every element of the matrix.
	float& operator()(int row, int col) { return _m[col * 4 + row]; }

	// Simple operator for directly accessing every element of the matrix.
	const float& operator()(int row, int col) const {  return _m[col * 4 + row]; }

	// Simple operator for linearly accessing every element of the matrix.
	float& operator[](unsigned int index) { return _m[index]; }

	// Simple operator for linearly accessing every element of the matrix.
	const float& operator[](unsigned int index) const { return _m[index]; }

	// Sets this matrix equal to the other matrix.
	Matrix4x4& operator=(const Matrix4x4 &other);

	// Returns true if other matrix is equal to this matrix.
	bool operator==(const Matrix4x4 &other) const;

	// Returns true if other matrix is not equal to this matrix.
	bool operator!=(const Matrix4x4 &other) const;

	// Multiply by another matrix.
	Matrix4x4& operator*=(const Matrix4x4& other);

	// Multiply by another matrix.
	Matrix4x4 operator*(const Matrix4x4& other) const;

	// Set matrix to identity.
	void makeIdentity();

	// Set the translation of the current matrix. Will erase any previous values.
	void setTranslation(const Vector3df& translation);

	// Gets the current translation
	Vector3df getTranslation() const;

	// Set the inverse translation of the current matrix. Will erase any previous values.
	void setInverseTranslation(const Vector3df& translation);

	// Make a rotation matrix from Euler angles. The 4th row and column are unmodified.
	void setRotationRadians(const Vector3df& rotation);
	void setRotationRadians(float angle, const Vector3df& axis);

	// Make a rotation matrix from Euler angles. The 4th row and column are unmodified.
	void setRotationDegrees(const Vector3df& rotation);
	void setRotationDegrees(float angle, const Vector3df& axis);

	// Returns the rotation, as set by setRotation(). This code was orginally written by by Chev.
	Vector3df getRotationDegrees() const;

	// Make an inverted rotation matrix from Euler angles. The 4th row and column are unmodified.
	void setInverseRotationRadians(const Vector3df& rotation);

	// Make an inverted rotation matrix from Euler angles. The 4th row and column are unmodified.
	void setInverseRotationDegrees(const Vector3df& rotation);

	// Set Scale
	void setScale(const Vector3df& scale);

	// Get Scale
	Vector3df getScale() const;

	// Translate a vector by the inverse of the translation part of this matrix.
	void inverseTranslateVect(Vector3df& vect) const;

	// Rotate a vector by the inverse of the rotation part of this matrix.
	void inverseRotateVect(Vector3df& vect) const;

	// Rotate a vector by the rotation part of this matrix.
	void rotateVect(Vector3df& vect) const;

	// Transforms the vector by this matrix
	void transformVect(Vector3df& vect) const;

	// Transforms input vector by this matrix and stores result in output vector
	void transformVect(const Vector3df& in, Vector3df& out) const;

	// Translate a vector by the translation part of this matrix.
	void translateVect(Vector3df& vect) const;

	// Transforms a plane by this matrix
	void transformPlane(Plane3d<float> &plane) const;

	// Transforms a plane by this matrix
	void transformPlane(const Plane3d<float> &in, Plane3d<float> &out) const;

	// Transforms a axis aligned bounding box
	void transformBox(Aabbox3d<float> &box) const;

	// Multiplies this matrix by a 1x4 matrix
	void multiplyWith1x4Matrix(float* matrix) const;

	// Calculates inverse of matrix. Slow.
	// \return Returns false if there is no inverse matrix.
	bool makeInverse();

	// returns the inversed matrix of this one
	// \param Target, where result matrix is written to.
	// \return Returns false if there is no inverse matrix.
	bool getInverse(Matrix4x4& out) const;

	// Builds a right-handed perspective projection matrix based on a field of view
	void buildProjectionMatrixPerspectiveFovRH(float fieldOfViewRadians, float aspectRatio, float zNear, float zFar);

	// Builds a left-handed perspective projection matrix based on a field of view
	void buildProjectionMatrixPerspectiveFovLH(float fieldOfViewRadians, float aspectRatio, float zNear, float zFar);

	// Builds a right-handed perspective projection matrix.
	void buildProjectionMatrixPerspectiveRH(float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar);

	// Builds a left-handed perspective projection matrix.
	void buildProjectionMatrixPerspectiveLH(float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar);

	// Builds a right-handed orthogonal projection matrix.
	void buildProjectionMatrixOrthoRH(float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar);

	// Builds a left-handed orthogonal projection matrix.
	void buildProjectionMatrixOrthoLH(float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar);

	// Builds a right-handed look-at matrix.
	void buildCameraLookAtMatrixRH(const Vector3df& position, const Vector3df& target, const Vector3df& upVector);

	// Builds a left-handed look-at matrix.
	void buildCameraLookAtMatrixLH(const Vector3df& position, const Vector3df& target, const Vector3df& upVector);

	// Builds a matrix that flattens geometry into a plane.
	// \param light: light source
	// \param plane: plane into which the geometry if flattened into
	// \param point: value between 0 and 1, describing the light source.
	// If this is 1, it is a point light, if it is 0, it is a directional light.
	void buildShadowMatrix(Vector3df light, Plane3df plane, float point=1.0f);

	// creates a new matrix as interpolated matrix from to other ones.
	// \param b: other matrix to interpolate with
	// \param time: Must be a value between 0 and 1.
	Matrix4x4 interpolate(Matrix4x4& b, float time);

	float* pointer() { return _m; }
	const float* pointer() const { return _m; }

private:
	// Matrix data, stored in column-major order
	float _m[16];
};

static const Matrix4x4 IdentityMatrix;

inline Matrix4x4::Matrix4x4()
{
	makeIdentity();
}

// multiply by another matrix
inline Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& other)
{
	float newMatrix[16];
	const float *m1 = _m, *m2 = other._m;

	newMatrix[0] = m1[0]*m2[0] + m1[4]*m2[1] + m1[8]*m2[2] + m1[12]*m2[3];
	newMatrix[1] = m1[1]*m2[0] + m1[5]*m2[1] + m1[9]*m2[2] + m1[13]*m2[3];
	newMatrix[2] = m1[2]*m2[0] + m1[6]*m2[1] + m1[10]*m2[2] + m1[14]*m2[3];
	newMatrix[3] = m1[3]*m2[0] + m1[7]*m2[1] + m1[11]*m2[2] + m1[15]*m2[3];
		
	newMatrix[4] = m1[0]*m2[4] + m1[4]*m2[5] + m1[8]*m2[6] + m1[12]*m2[7];
	newMatrix[5] = m1[1]*m2[4] + m1[5]*m2[5] + m1[9]*m2[6] + m1[13]*m2[7];
	newMatrix[6] = m1[2]*m2[4] + m1[6]*m2[5] + m1[10]*m2[6] + m1[14]*m2[7];
	newMatrix[7] = m1[3]*m2[4] + m1[7]*m2[5] + m1[11]*m2[6] + m1[15]*m2[7];
		
	newMatrix[8] = m1[0]*m2[8] + m1[4]*m2[9] + m1[8]*m2[10] + m1[12]*m2[11];
	newMatrix[9] = m1[1]*m2[8] + m1[5]*m2[9] + m1[9]*m2[10] + m1[13]*m2[11];
	newMatrix[10] = m1[2]*m2[8] + m1[6]*m2[9] + m1[10]*m2[10] + m1[14]*m2[11];
	newMatrix[11] = m1[3]*m2[8] + m1[7]*m2[10] + m1[11]*m2[10] + m1[15]*m2[11];
		
	newMatrix[12] = m1[0]*m2[12] + m1[4]*m2[13] + m1[8]*m2[14] + m1[12]*m2[15];
	newMatrix[13] = m1[1]*m2[12] + m1[5]*m2[13] + m1[9]*m2[14] + m1[13]*m2[15];
	newMatrix[14] = m1[2]*m2[12] + m1[6]*m2[13] + m1[10]*m2[14] + m1[14]*m2[15];
	newMatrix[15] = m1[3]*m2[12] + m1[7]*m2[13] + m1[11]*m2[14] + m1[15]*m2[15];
		
	for (int i=0; i<16; ++i)
		_m[i] = newMatrix[i];

	return *this;
}

// multiply by another matrix
inline Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other) const
{
	Matrix4x4 tmtrx;
	const float *m1 = _m, *m2 = other._m;
	float *m3 = tmtrx._m;

	m3[0] = m1[0]*m2[0] + m1[4]*m2[1] + m1[8]*m2[2] + m1[12]*m2[3];
	m3[1] = m1[1]*m2[0] + m1[5]*m2[1] + m1[9]*m2[2] + m1[13]*m2[3];
	m3[2] = m1[2]*m2[0] + m1[6]*m2[1] + m1[10]*m2[2] + m1[14]*m2[3];
	m3[3] = m1[3]*m2[0] + m1[7]*m2[1] + m1[11]*m2[2] + m1[15]*m2[3];
		
	m3[4] = m1[0]*m2[4] + m1[4]*m2[5] + m1[8]*m2[6] + m1[12]*m2[7];
	m3[5] = m1[1]*m2[4] + m1[5]*m2[5] + m1[9]*m2[6] + m1[13]*m2[7];
	m3[6] = m1[2]*m2[4] + m1[6]*m2[5] + m1[10]*m2[6] + m1[14]*m2[7];
	m3[7] = m1[3]*m2[4] + m1[7]*m2[5] + m1[11]*m2[6] + m1[15]*m2[7];
		
	m3[8] = m1[0]*m2[8] + m1[4]*m2[9] + m1[8]*m2[10] + m1[12]*m2[11];
	m3[9] = m1[1]*m2[8] + m1[5]*m2[9] + m1[9]*m2[10] + m1[13]*m2[11];
	m3[10] = m1[2]*m2[8] + m1[6]*m2[9] + m1[10]*m2[10] + m1[14]*m2[11];
	m3[11] = m1[3]*m2[8] + m1[7]*m2[10] + m1[11]*m2[10] + m1[15]*m2[11];
		
	m3[12] = m1[0]*m2[12] + m1[4]*m2[13] + m1[8]*m2[14] + m1[12]*m2[15];
	m3[13] = m1[1]*m2[12] + m1[5]*m2[13] + m1[9]*m2[14] + m1[13]*m2[15];
	m3[14] = m1[2]*m2[12] + m1[6]*m2[13] + m1[10]*m2[14] + m1[14]*m2[15];
	m3[15] = m1[3]*m2[12] + m1[7]*m2[13] + m1[11]*m2[14] + m1[15]*m2[15];

	return tmtrx;
}

inline Vector3df Matrix4x4::getTranslation() const
{
	return Vector3df(_m[12], _m[13], _m[14]);
}

inline void Matrix4x4::setTranslation(const Vector3df& translation)
{
	_m[12] = translation._x;
	_m[13] = translation._y;
	_m[14] = translation._z;
}

inline void Matrix4x4::setInverseTranslation(const Vector3df& translation)
{
	_m[12] = -translation._x;
	_m[13] = -translation._y;
	_m[14] = -translation._z;
}

inline void Matrix4x4::setScale(const Vector3df& scale)
{
	_m[0] = scale._x;
	_m[5] = scale._y;
	_m[10] = scale._z;
}

inline Vector3df Matrix4x4::getScale() const
{
	return Vector3df(_m[0], _m[5], _m[10]);
}

inline void Matrix4x4::setRotationDegrees(const Vector3df& rotation)
{
	setRotationRadians( rotation * (float)3.1415926535897932384626433832795 / 180.0 );
}

inline void Matrix4x4::setRotationDegrees(float angle, const Vector3df& axis)
{
	setRotationRadians(m3dDegToRad(angle), axis);
}

inline void Matrix4x4::setInverseRotationDegrees(const Vector3df& rotation)
{
	setInverseRotationRadians( rotation * (float)3.1415926535897932384626433832795 / 180.0 );
}

inline void Matrix4x4::setRotationRadians(const Vector3df& rotation)
{
	double cr = cos( rotation._x );
	double sr = sin( rotation._x );
	double cp = cos( rotation._y );
	double sp = sin( rotation._y );
	double cy = cos( rotation._z );
	double sy = sin( rotation._z );

	_m[0] = (float)( cp*cy );
	_m[1] = (float)( cp*sy );
	_m[2] = (float)( -sp );

	double srsp = sr*sp;
	double crsp = cr*sp;

	_m[4] = (float)( srsp*cy-cr*sy );
	_m[5] = (float)( srsp*sy+cr*cy );
	_m[6] = (float)( sr*cp );

	_m[8] = (float)( crsp*cy+sr*sy );
	_m[9] = (float)( crsp*sy-sr*cy );
	_m[10] = (float)( cr*cp );
}

inline void Matrix4x4::setRotationRadians(float angle, const Vector3df& axis)
{
	float x = axis._x;
	float y = axis._y;
	float z = axis._z;
	float mag, s, c;
	float xx, yy, zz, xy, yz, zx, xs, ys, zs, one_c;

	s = float(sin(angle));
	c = float(cos(angle));

	mag = float(sqrt( x*x + y*y + z*z ));

	// Identity matrix
	if (mag == 0.0f) {
		makeIdentity();
		return;
	}

	// Rotation matrix is normalized
	x /= mag;
	y /= mag;
	z /= mag;

	xx = x * x;
	yy = y * y;
	zz = z * z;
	xy = x * y;
	yz = y * z;
	zx = z * x;
	xs = x * s;
	ys = y * s;
	zs = z * s;
	one_c = 1.0f - c;

	(*this)(0,0) = (one_c * xx) + c;
	(*this)(0,1) = (one_c * xy) - zs;
	(*this)(0,2) = (one_c * zx) + ys;

	(*this)(1,0) = (one_c * xy) + zs;
	(*this)(1,1) = (one_c * yy) + c;
	(*this)(1,2) = (one_c * yz) - xs;

	(*this)(2,0) = (one_c * zx) - ys;
	(*this)(2,1) = (one_c * yz) + xs;
	(*this)(2,2) = (one_c * zz) + c;
}

// Returns a rotation that is equivalent to that set by setRotationDegrees().
/** This code was sent in by Chev.  Note that it does not necessarily return
the *same* Euler angles as those set by setRotationDegrees(), but the rotation will
be equivalent, i.e. will have the same result when used to rotate a vector or node. */
inline Vector3df Matrix4x4::getRotationDegrees() const
{
	const Matrix4x4& mat = *this;
	Vector3df scale = getScale();
	// we need to check for negative scale on to axes, which would bring up wrong results
	if (scale._y < 0 && scale._z < 0)
	{
		scale._y = -scale._y;
		scale._z = -scale._z;
	}
	else if (scale._x < 0 && scale._z < 0)
	{
		scale._x = -scale._x;
		scale._z = -scale._z;
	}
	else if (scale._x < 0 && scale._y < 0)
	{
		scale._x = -scale._x;
		scale._y = -scale._y;
	}
	const Vector3df invScale(1.0f / scale._x, 1.0f / scale._y, 1.0f / scale._z);

	float y = -asin(clamp_(mat[2] * invScale._x, -1.0f, 1.0f));
	const float c = cos(y);
	y = m3dRadToDeg(y);

	float rotx, roty, x, z;

	if (!isZero(c))
	{
		const float invC = 1.0f / c;
		rotx = mat[10] * invC * invScale._z;
		roty = mat[6] * invC * invScale._y;
		x = m3dRadToDeg(atan2(roty, rotx));
		rotx = mat[0] * invC * invScale._x;
		roty = mat[1] * invC * invScale._x;
		z = m3dRadToDeg(atan2(roty, rotx));
	}
	else
	{
		x = 0.0;
		rotx = mat[5] * invScale._y;
		roty = -mat[4] * invScale._y;
		z = m3dRadToDeg(atan2(roty, rotx));
	}

	// fix values that get below zero
	if (x < 0.0) x += 360.0f;
	if (y < 0.0) y += 360.0f;
	if (z < 0.0) z += 360.0f;

	return Vector3df(x, y, z);
}

inline void Matrix4x4::setInverseRotationRadians(const Vector3df& rotation)
{
	double cr = cos( rotation._x );
	double sr = sin( rotation._x );
	double cp = cos( rotation._y );
	double sp = sin( rotation._y );
	double cy = cos( rotation._z );
	double sy = sin( rotation._z );

	_m[0] = (float)( cp*cy );
	_m[4] = (float)( cp*sy );
	_m[8] = (float)( -sp );

	double srsp = sr*sp;
	double crsp = cr*sp;

	_m[1] = (float)( srsp*cy-cr*sy );
	_m[5] = (float)( srsp*sy+cr*cy );
	_m[9] = (float)( sr*cp );

	_m[2] = (float)( crsp*cy+sr*sy );
	_m[6] = (float)( crsp*sy-sr*cy );
	_m[10] = (float)( cr*cp );
}

inline void Matrix4x4::makeIdentity()
{
	for (int i=0; i<16; ++i)
		_m[i] = 0.0f;
	_m[0] = _m[5] = _m[10] = _m[15] = 1;
}

inline void Matrix4x4::rotateVect(Vector3df& vect) const
{
	Vector3df tmp = vect;
	vect._x = -(tmp._x*_m[0] + tmp._y*_m[1] + tmp._z*_m[2]);
	vect._y = -(tmp._x*_m[4] + tmp._y*_m[5] + tmp._z*_m[6]);
	vect._z = -(tmp._x*_m[8] + tmp._y*_m[9] + tmp._z*_m[10]);
}

inline void Matrix4x4::inverseRotateVect(Vector3df& vect) const
{
	Vector3df tmp = vect;
	vect._x = tmp._x*_m[0] + tmp._y*_m[1] + tmp._z*_m[2];
	vect._y = tmp._x*_m[4] + tmp._y*_m[5] + tmp._z*_m[6];
	vect._z = tmp._x*_m[8] + tmp._y*_m[9] + tmp._z*_m[10];
}

inline void Matrix4x4::transformVect(Vector3df& vect) const
{
	float vector[3];

	vector[0] = vect._x*_m[0] + vect._y*_m[4] + vect._z*_m[8] + _m[12];
	vector[1] = vect._x*_m[1] + vect._y*_m[5] + vect._z*_m[9] + _m[13];
	vector[2] = vect._x*_m[2] + vect._y*_m[6] + vect._z*_m[10] + _m[14];

	vect._x = vector[0];
	vect._y = vector[1];
	vect._z = vector[2];
}

inline void Matrix4x4::transformVect(const Vector3df& in, Vector3df& out) const
{
	out._x = in._x*_m[0] + in._y*_m[4] + in._z*_m[8] + _m[12];
	out._y = in._x*_m[1] + in._y*_m[5] + in._z*_m[9] + _m[13];
	out._z = in._x*_m[2] + in._y*_m[6] + in._z*_m[10] + _m[14];
}

// Transforms a plane by this matrix
inline void Matrix4x4::transformPlane(Plane3d<float> &plane) const
{
	Vector3df member;
	transformVect(plane.getMemberPoint(), member);

	Vector3df origin(0,0,0);
	transformVect(plane._normal);
	transformVect(origin);

	plane._normal -= origin;
	plane._distance = - member.dotProduct(plane._normal);
}

// Transforms a plane by this matrix
inline void Matrix4x4::transformPlane(const Plane3d<float> &in, Plane3d<float> &out) const
{
	out = in;
    transformPlane( out );
}

// Transforms a axis aligned bounding box
inline void Matrix4x4::transformBox(Aabbox3d<float> &box) const
{
	const float Amin[3] = { box._minEdge._x, box._minEdge._y, box._minEdge._z };
	const float Amax[3] = { box._maxEdge._x, box._maxEdge._y, box._maxEdge._z };

	float Bmin[3];
	float Bmax[3];

	Bmin[0] = Bmax[0] = _m[12];
	Bmin[1] = Bmax[1] = _m[13];
	Bmin[2] = Bmax[2] = _m[14];

	const Matrix4x4 &m = *this;

	for (unsigned int i = 0; i < 3; ++i)
	{
		for (unsigned int j = 0; j < 3; ++j)
		{
			const float a = m(i, j) * Amin[j];
			const float b = m(i, j) * Amax[j];

			if (a < b)
			{
				Bmin[i] += a;
				Bmax[i] += b;
			}
			else
			{
				Bmin[i] += b;
				Bmax[i] += a;
			}
		}
	}

	box._minEdge._x = Bmin[0];
	box._minEdge._y = Bmin[1];
	box._minEdge._z = Bmin[2];

	box._maxEdge._x = Bmax[0];
	box._maxEdge._y = Bmax[1];
	box._maxEdge._z = Bmax[2];
}

// Multiplies this matrix by a 1x4 matrix
inline void Matrix4x4::multiplyWith1x4Matrix(float* matrix) const
{
	/*
	0  1  2  3
	4  5  6  7
	8  9  10 11
	12 13 14 15
	*/

	float mat[4];
	mat[0] = matrix[0];
	mat[1] = matrix[1];
	mat[2] = matrix[2];
	mat[3] = matrix[3];

	matrix[0] = _m[0]*mat[0] + _m[4]*mat[1] + _m[8]*mat[2] + _m[12]*mat[3];
	matrix[1] = _m[1]*mat[0] + _m[5]*mat[1] + _m[9]*mat[2] + _m[13]*mat[3];
	matrix[2] = _m[2]*mat[0] + _m[6]*mat[1] + _m[10]*mat[2] + _m[14]*mat[3];
	matrix[3] = _m[3]*mat[0] + _m[7]*mat[1] + _m[11]*mat[2] + _m[15]*mat[3];
}

inline void Matrix4x4::inverseTranslateVect(Vector3df& vect) const
{
	vect._x = vect._x-_m[12];
	vect._y = vect._y-_m[13];
	vect._z = vect._z-_m[14];
}

inline void Matrix4x4::translateVect(Vector3df& vect) const
{
	vect._x = vect._x+_m[12];
	vect._y = vect._y+_m[13];
	vect._z = vect._z+_m[14];
}

inline bool Matrix4x4::getInverse(Matrix4x4& out) const
{
	/// Calculates the inverse of this Matrix 
	/// The inverse is calculated using Cramers rule.
	/// If no inverse exists then 'false' is returned.

	const Matrix4x4 &m = *this;

	float d = (m(0, 0) * m(1, 1) - m(1, 0) * m(0, 1)) * (m(2, 2) * m(3, 3) - m(3, 2) * m(2, 3))	- (m(0, 0) * m(2, 1) - m(2, 0) * m(0, 1)) * (m(1, 2) * m(3, 3) - m(3, 2) * m(1, 3))
			+ (m(0, 0) * m(3, 1) - m(3, 0) * m(0, 1)) * (m(1, 2) * m(2, 3) - m(2, 2) * m(1, 3))	+ (m(1, 0) * m(2, 1) - m(2, 0) * m(1, 1)) * (m(0, 2) * m(3, 3) - m(3, 2) * m(0, 3))
			- (m(1, 0) * m(3, 1) - m(3, 0) * m(1, 1)) * (m(0, 2) * m(2, 3) - m(2, 2) * m(0, 3))	+ (m(2, 0) * m(3, 1) - m(3, 0) * m(2, 1)) * (m(0, 2) * m(1, 3) - m(1, 2) * m(0, 3));
		
	if (d == 0.f)
		return false;

	d = 1.f / d;

	out(0, 0) = d * (m(1, 1) * (m(2, 2) * m(3, 3) - m(3, 2) * m(2, 3)) + m(2, 1) * (m(3, 2) * m(1, 3) - m(1, 2) * m(3, 3)) + m(3, 1) * (m(1, 2) * m(2, 3) - m(2, 2) * m(1, 3)));
	out(1, 0) = d * (m(1, 2) * (m(2, 0) * m(3, 3) - m(3, 0) * m(2, 3)) + m(2, 2) * (m(3, 0) * m(1, 3) - m(1, 0) * m(3, 3)) + m(3, 2) * (m(1, 0) * m(2, 3) - m(2, 0) * m(1, 3)));
	out(2, 0) = d * (m(1, 3) * (m(2, 0) * m(3, 1) - m(3, 0) * m(2, 1)) + m(2, 3) * (m(3, 0) * m(1, 1) - m(1, 0) * m(3, 1)) + m(3, 3) * (m(1, 0) * m(2, 1) - m(2, 0) * m(1, 1)));
	out(3, 0) = d * (m(1, 0) * (m(3, 1) * m(2, 2) - m(2, 1) * m(3, 2)) + m(2, 0) * (m(1, 1) * m(3, 2) - m(3, 1) * m(1, 2)) + m(3, 0) * (m(2, 1) * m(1, 2) - m(1, 1) * m(2, 2)));
	out(0, 1) = d * (m(2, 1) * (m(0, 2) * m(3, 3) - m(3, 2) * m(0, 3)) + m(3, 1) * (m(2, 2) * m(0, 3) - m(0, 2) * m(2, 3)) + m(0, 1) * (m(3, 2) * m(2, 3) - m(2, 2) * m(3, 3)));
	out(1, 1) = d * (m(2, 2) * (m(0, 0) * m(3, 3) - m(3, 0) * m(0, 3)) + m(3, 2) * (m(2, 0) * m(0, 3) - m(0, 0) * m(2, 3)) + m(0, 2) * (m(3, 0) * m(2, 3) - m(2, 0) * m(3, 3)));
	out(2, 1) = d * (m(2, 3) * (m(0, 0) * m(3, 1) - m(3, 0) * m(0, 1)) + m(3, 3) * (m(2, 0) * m(0, 1) - m(0, 0) * m(2, 1)) + m(0, 3) * (m(3, 0) * m(2, 1) - m(2, 0) * m(3, 1)));
	out(3, 1) = d * (m(2, 0) * (m(3, 1) * m(0, 2) - m(0, 1) * m(3, 2)) + m(3, 0) * (m(0, 1) * m(2, 2) - m(2, 1) * m(0, 2)) + m(0, 0) * (m(2, 1) * m(3, 2) - m(3, 1) * m(2, 2)));
	out(0, 2) = d * (m(3, 1) * (m(0, 2) * m(1, 3) - m(1, 2) * m(0, 3)) + m(0, 1) * (m(1, 2) * m(3, 3) - m(3, 2) * m(1, 3)) + m(1, 1) * (m(3, 2) * m(0, 3) - m(0, 2) * m(3, 3)));
	out(1, 2) = d * (m(3, 2) * (m(0, 0) * m(1, 3) - m(1, 0) * m(0, 3)) + m(0, 2) * (m(1, 0) * m(3, 3) - m(3, 0) * m(1, 3)) + m(1, 2) * (m(3, 0) * m(0, 3) - m(0, 0) * m(3, 3)));
	out(2, 2) = d * (m(3, 3) * (m(0, 0) * m(1, 1) - m(1, 0) * m(0, 1)) + m(0, 3) * (m(1, 0) * m(3, 1) - m(3, 0) * m(1, 1)) + m(1, 3) * (m(3, 0) * m(0, 1) - m(0, 0) * m(3, 1)));
	out(3, 2) = d * (m(3, 0) * (m(1, 1) * m(0, 2) - m(0, 1) * m(1, 2)) + m(0, 0) * (m(3, 1) * m(1, 2) - m(1, 1) * m(3, 2)) + m(1, 0) * (m(0, 1) * m(3, 2) - m(3, 1) * m(0, 2)));
	out(0, 3) = d * (m(0, 1) * (m(2, 2) * m(1, 3) - m(1, 2) * m(2, 3)) + m(1, 1) * (m(0, 2) * m(2, 3) - m(2, 2) * m(0, 3)) + m(2, 1) * (m(1, 2) * m(0, 3) - m(0, 2) * m(1, 3)));
	out(1, 3) = d * (m(0, 2) * (m(2, 0) * m(1, 3) - m(1, 0) * m(2, 3)) + m(1, 2) * (m(0, 0) * m(2, 3) - m(2, 0) * m(0, 3)) + m(2, 2) * (m(1, 0) * m(0, 3) - m(0, 0) * m(1, 3)));
	out(2, 3) = d * (m(0, 3) * (m(2, 0) * m(1, 1) - m(1, 0) * m(2, 1)) + m(1, 3) * (m(0, 0) * m(2, 1) - m(2, 0) * m(0, 1)) + m(2, 3) * (m(1, 0) * m(0, 1) - m(0, 0) * m(1, 1)));
	out(3, 3) = d * (m(0, 0) * (m(1, 1) * m(2, 2) - m(2, 1) * m(1, 2)) + m(1, 0) * (m(2, 1) * m(0, 2) - m(0, 1) * m(2, 2)) + m(2, 0) * (m(0, 1) * m(1, 2) - m(1, 1) * m(0, 2)));

	return true;
}

inline bool Matrix4x4::makeInverse()
{
	Matrix4x4 temp;

	if (getInverse(temp))
	{
		*this = temp;
		return true;
	}

	return false;
}

inline Matrix4x4& Matrix4x4::operator=(const Matrix4x4 &other)
{
	for (int i = 0; i < 16; ++i)
		_m[i] = other._m[i];

	return *this;
}

inline bool Matrix4x4::operator==(const Matrix4x4 &other) const
{
	for (int i = 0; i < 16; ++i)
		if (!equals(_m[i], other._m[i]))
			return false;

	return true;
}

inline bool Matrix4x4::operator!=(const Matrix4x4 &other) const
{
	return !(*this == other);
}

// Builds a right-handed perspective projection matrix based on a field of view
inline void Matrix4x4::buildProjectionMatrixPerspectiveFovRH(float fieldOfViewRadians, float aspectRatio, float zNear, float zFar)
{
	float top = zNear * tanf(0.5f*fieldOfViewRadians);
	float bottom = -top;
	float right = aspectRatio * top;
	float left = -right;

	(*this)(0,0) = 2*zNear/(right-left);
	(*this)(1,0) = 0;
	(*this)(2,0) = 0;
	(*this)(3,0) = 0;

	(*this)(0,1) = 0;
	(*this)(1,1) = 2*zNear/(top-bottom);
	(*this)(2,1) = 0;
	(*this)(3,1) = 0;

	(*this)(0,2) = (right+left)/(right-left);
	(*this)(1,2) = (top+bottom)/(top-bottom);
	(*this)(2,2) = (zNear+zFar)/(zNear-zFar);
	(*this)(3,2) = -1;

	(*this)(0,3) = 0;
	(*this)(1,3) = 0;
	(*this)(2,3) = 2*zNear*zFar/(zNear-zFar);
	(*this)(3,3) = 0;
}

// Builds a left-handed perspective projection matrix based on a field of view
inline void Matrix4x4::buildProjectionMatrixPerspectiveFovLH(float fieldOfViewRadians, float aspectRatio, float zNear, float zFar)
{
	float h = (float)(cos(fieldOfViewRadians/2) / sin(fieldOfViewRadians/2));
	float w = h / aspectRatio;

	(*this)(0,0) = 2*zNear/w;
	(*this)(1,0) = 0;
	(*this)(2,0) = 0;
	(*this)(3,0) = 0;

	(*this)(0,1) = 0;
	(*this)(1,1) = 2*zNear/h;
	(*this)(2,1) = 0;
	(*this)(3,1) = 0;

	(*this)(0,2) = 0;
	(*this)(1,2) = 0;
	(*this)(2,2) = zFar/(zFar-zNear);
	(*this)(3,2) = 1;

	(*this)(0,3) = 0;
	(*this)(1,3) = 0;
	(*this)(2,3) = zNear*zFar/(zNear-zFar);
	(*this)(3,3) = 0;
}

// Builds a right-handed perspective projection matrix.
inline void Matrix4x4::buildProjectionMatrixPerspectiveRH(float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar)
{
	(*this)(0,0) = 2*zNear/widthOfViewVolume;
	(*this)(1,0) = 0;
	(*this)(2,0) = 0;
	(*this)(3,0) = 0;

	(*this)(0,1) = 0;
	(*this)(1,1) = 2*zNear/heightOfViewVolume;
	(*this)(2,1) = 0;
	(*this)(3,1) = 0;

	(*this)(0,2) = 0;
	(*this)(1,2) = 0;
	(*this)(2,2) = zFar/(zNear-zFar);
	(*this)(3,2) = -1;

	(*this)(0,3) = 0;
	(*this)(1,3) = 0;
	(*this)(2,3) = zNear*zFar/(zNear-zFar);
	(*this)(3,3) = 0;
}

// Builds a left-handed perspective projection matrix.
inline void Matrix4x4::buildProjectionMatrixPerspectiveLH(float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar)
{
	(*this)(0,0) = 2*zNear/widthOfViewVolume;
	(*this)(1,0) = 0;
	(*this)(2,0) = 0;
	(*this)(3,0) = 0;

	(*this)(0,1) = 0;
	(*this)(1,1) = 2*zNear/heightOfViewVolume;
	(*this)(2,1) = 0;
	(*this)(3,1) = 0;

	(*this)(0,2) = 0;
	(*this)(1,2) = 0;
	(*this)(2,2) = zFar/(zFar-zNear);
	(*this)(3,2) = 1;

	(*this)(0,3) = 0;
	(*this)(1,3) = 0;
	(*this)(2,3) = zNear*zFar/(zNear-zFar);
	(*this)(3,3) = 0;
}

// Builds a right-handed orthogonal projection matrix.
inline void Matrix4x4::buildProjectionMatrixOrthoRH(float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar)
{
	float top = heightOfViewVolume / 2.0f;
	float bottom = -top;
	float right = widthOfViewVolume / 2.0f;
	float left = -right;

	(*this)(0, 0) = 2 / (right - left);
	(*this)(1, 0) = 0;
	(*this)(2, 0) = 0;
	(*this)(3, 0) = 0;

	(*this)(0, 1) = 0;
	(*this)(1, 1) = 2 / (top - bottom);
	(*this)(2, 1) = 0;
	(*this)(3, 1) = 0;

	(*this)(0, 2) = 0;
	(*this)(1, 2) = 0;
	(*this)(2, 2) = 2 / (zNear - zFar);
	(*this)(3, 2) = 0;

	(*this)(0, 3) = (left + right) / (left - right);
	(*this)(1, 3) = (bottom + top) / (bottom - top);
	(*this)(2, 3) = (zNear + zFar) / (zNear - zFar);
	(*this)(3, 3) = 1;
}

// Builds a left-handed orthogonal projection matrix.
inline void Matrix4x4::buildProjectionMatrixOrthoLH(float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar)
{
	(*this)(0, 0) = 2 / widthOfViewVolume;
	(*this)(1, 0) = 0;
	(*this)(2, 0) = 0;
	(*this)(3, 0) = 0;

	(*this)(0, 1) = 0;
	(*this)(1, 1) = 2 / heightOfViewVolume;
	(*this)(2, 1) = 0;
	(*this)(3, 1) = 0;

	(*this)(0, 2) = 0;
	(*this)(1, 2) = 0;
	(*this)(2, 2) = 1 / (zNear - zFar);
	(*this)(3, 2) = 0;

	(*this)(0, 3) = 0;
	(*this)(1, 3) = 0;
	(*this)(2, 3) = zNear / (zNear - zFar);
	(*this)(3, 3) = 1;
}

// Builds a right-handed look-at matrix.
inline void Matrix4x4::buildCameraLookAtMatrixRH(const Vector3df& position,
	const Vector3df& target,
	const Vector3df& upVector)
{
	Vector3df zaxis = position - target;
	zaxis.normalize();

	Vector3df xaxis = upVector.crossProduct(zaxis);
	xaxis.normalize();

	Vector3df yaxis = zaxis.crossProduct(xaxis);

	(*this)(0, 0) = xaxis._x;
	(*this)(1, 0) = yaxis._x;
	(*this)(2, 0) = zaxis._x;
	(*this)(3, 0) = 0;

	(*this)(0, 1) = xaxis._y;
	(*this)(1, 1) = yaxis._y;
	(*this)(2, 1) = zaxis._y;
	(*this)(3, 1) = 0;

	(*this)(0, 2) = xaxis._z;
	(*this)(1, 2) = yaxis._z;
	(*this)(2, 2) = zaxis._z;
	(*this)(3, 2) = 0;

	(*this)(0, 3) = -xaxis.dotProduct(position);
	(*this)(1, 3) = -yaxis.dotProduct(position);
	(*this)(2, 3) = -zaxis.dotProduct(position);
	(*this)(3, 3) = 1.0f;
}

// Builds a left-handed look-at matrix.
inline void Matrix4x4::buildCameraLookAtMatrixLH(const Vector3df& position,
	const Vector3df& target,
	const Vector3df& upVector)
{
	Vector3df zaxis = target - position;
	zaxis.normalize();

	Vector3df xaxis = upVector.crossProduct(zaxis);
	xaxis.normalize();

	Vector3df yaxis = zaxis.crossProduct(xaxis);

	(*this)(0, 0) = xaxis._x;
	(*this)(1, 0) = yaxis._x;
	(*this)(2, 0) = zaxis._x;
	(*this)(3, 0) = 0;

	(*this)(0, 1) = xaxis._y;
	(*this)(1, 1) = yaxis._y;
	(*this)(2, 1) = zaxis._y;
	(*this)(3, 1) = 0;

	(*this)(0, 2) = xaxis._z;
	(*this)(1, 2) = yaxis._z;
	(*this)(2, 2) = zaxis._z;
	(*this)(3, 2) = 0;

	(*this)(0, 3) = -xaxis.dotProduct(position);
	(*this)(1, 3) = -yaxis.dotProduct(position);
	(*this)(2, 3) = -zaxis.dotProduct(position);
	(*this)(3, 3) = 1.0f;
}

// Builds a matrix that flattens geometry into a plane.
inline void Matrix4x4::buildShadowMatrix(Vector3df light, Plane3df plane, float point)
{
	plane._normal.normalize();
	float d = plane._normal.dotProduct(light);

	(*this)(0,0) = plane._normal._x * light._x + d;
	(*this)(1,0) = plane._normal._x * light._y;
	(*this)(2,0) = plane._normal._x * light._z;
	(*this)(3,0) = plane._normal._x * point;

	(*this)(0,1) = plane._normal._y * light._x;
	(*this)(1,1) = plane._normal._y * light._y + d;
	(*this)(2,1) = plane._normal._y * light._z;
	(*this)(3,1) = plane._normal._y * point;

	(*this)(0,2) = plane._normal._z * light._x;
	(*this)(1,2) = plane._normal._z * light._y;
	(*this)(2,2) = plane._normal._z * light._z + d;
	(*this)(3,2) = plane._normal._z * point;

	(*this)(0,3) = plane._distance * light._x + d;
	(*this)(1,3) = plane._distance * light._y;
	(*this)(2,3) = plane._distance * light._z;
	(*this)(3,3) = plane._distance * point;
}

// creates a new matrix as interpolated matrix from to other ones.
// \param time: Must be a value between 0 and 1.
inline Matrix4x4 Matrix4x4::interpolate(Matrix4x4& b, float time)
{
	float ntime = 1.0f - time;
	Matrix4x4 mat;
	for (int i=0; i<16; ++i)
		mat._m[i] = _m[i]*ntime + b._m[i]*time;

	return mat;
}

NS_VAYO_END

#endif // __VAYO_MATRIX4X4_H__
