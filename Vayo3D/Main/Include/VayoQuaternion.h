/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 四元数
\*************************************************************************/
#ifndef __VAYO_QUATERNION_H__
#define __VAYO_QUATERNION_H__

#include "VayoMatrix4x4.h"
NS_VAYO_BEGIN

// Quaternion class.
class _VayoExport Quaternion
{
public:
	// Default Constructor
	Quaternion();

	// Constructor
	Quaternion(float x, float y, float z, float w);

	// Constructor which converts euler angles to a quaternion
	Quaternion(float x, float y, float z);

	// Constructor which converts a matrix to a quaternion
	Quaternion(const Matrix4x4& mat);

	// equal operator
	bool operator==(const Quaternion& other) const;

	// assignment operator
	Quaternion& operator=(const Quaternion& other);

	// matrix assignment operator
	Quaternion& operator=(const Matrix4x4& other);

	// add operator
	Quaternion operator+(const Quaternion& other) const;

	// multiplication operator
	Quaternion operator*(const Quaternion& other) const;

	// multiplication operator
	Quaternion operator*(float s) const;

	// multiplication operator
	Quaternion& operator*=(const Quaternion& other);

	// calculates the dot product
	float getDotProduct(const Quaternion& other) const;

	// sets new quaternion 
	void set(float x, float y, float z, float w);

	// sets new quaternion based on euler angles
	void set(float x, float y, float z);

	// normalizes the quaternion
	Quaternion& normalize();

	// Creates a matrix from this quaternion
	Matrix4x4 getMatrix() const;

	// Inverts this quaternion
	void makeInverse();

	// Interpolates the quaternion between to quaternions based on time
	Quaternion slerp(Quaternion q1, Quaternion q2, float time);

	// Create quaternion from rotation angle and rotation axis.
	/** Axis must be unit length.
	The quaternion representing the rotation is
	q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k).
	\param angle Rotation Angle in radians.
	\param axis Rotation axis. */
	Quaternion& fromAngleAxis(float angle, const Vector3df& axis);

	// Fills an angle (radians) around an axis (unit vector)
	void toAngleAxis(float& angle, Vector3df& axis) const;

	float _x, _y, _z, _w;
};

// Default Constructor
inline Quaternion::Quaternion()
	: _x(0.0f), _y(0.0f), _z(0.0f), _w(1.0f)
{
}

// Constructor
inline Quaternion::Quaternion(float x, float y, float z, float w)
	: _x(x), _y(y), _z(z), _w(w)
{
}

// Constructor which converts euler angles to a quaternion
inline Quaternion::Quaternion(float x, float y, float z)
{
	set(x, y, z);
}

// Constructor which converts a matrix to a quaternion
inline Quaternion::Quaternion(const Matrix4x4& mat)
{
	(*this) = mat;
}

// equal operator
inline bool Quaternion::operator==(const Quaternion& other) const
{
	if (_x != other._x)
		return false;
	if (_y != other._y)
		return false;
	if (_z != other._z)
		return false;
	if (_w != other._w)
		return false;

	return true;
}

// assignment operator
inline Quaternion& Quaternion::operator=(const Quaternion& other)
{
	_x = other._x;
	_y = other._y;
	_z = other._z;
	_w = other._w;
	return *this;
}

// matrix assignment operator
inline Quaternion& Quaternion::operator=(const Matrix4x4& m)
{
	float diag = m(0, 0) + m(1, 1) + m(2, 2) + 1;
	float scale = 0.0f;

	if (diag > 0.0f)
	{
		scale = sqrtf(diag) * 2.0f; // get scale from diagonal

		// TODO: speed this up
		_x = (m(1, 2) - m(2, 1)) / scale;
		_y = (m(2, 0) - m(0, 2)) / scale;
		_z = (m(0, 1) - m(1, 0)) / scale;
		_w = 0.25f * scale;
	}
	else
	{
		if (m(0, 0) > m(1, 1) && m(0, 0) > m(2, 2))
		{
			// 1st element of diag is greatest value
			// find scale according to 1st element, and double it
			scale = sqrtf(1.0f + m(0, 0) - m(1, 1) - m(2, 2)) * 2.0f;

			// TODO: speed this up
			_x = 0.25f * scale;
			_y = (m(0, 1) + m(1, 0)) / scale;
			_z = (m(2, 0) + m(0, 2)) / scale;
			_w = (m(1, 2) - m(2, 1)) / scale;
		}
		else if (m(1, 1) > m(2, 2))
		{
			// 2nd element of diag is greatest value
			// find scale according to 2nd element, and double it
			scale = sqrtf(1.0f + m(1, 1) - m(0, 0) - m(2, 2)) * 2.0f;

			// TODO: speed this up
			_x = (m(0, 1) + m(1, 0)) / scale;
			_y = 0.25f * scale;
			_z = (m(1, 2) + m(2, 1)) / scale;
			_w = (m(2, 0) - m(0, 2)) / scale;
		}
		else
		{
			// 3rd element of diag is greatest value
			// find scale according to 3rd element, and double it
			scale = sqrtf(1.0f + m(2, 2) - m(0, 0) - m(1, 1)) * 2.0f;

			// TODO: speed this up
			_x = (m(2, 0) + m(0, 2)) / scale;
			_y = (m(1, 2) + m(2, 1)) / scale;
			_z = 0.25f * scale;
			_w = (m(0, 1) - m(1, 0)) / scale;
		}
	}

	normalize();
	return *this;
}

// multiplication operator
inline Quaternion Quaternion::operator*(const Quaternion& other) const
{
	Quaternion tmp;

	tmp._w = (other._w * _w) - (other._x * _x) - (other._y * _y) - (other._z * _z);
	tmp._x = (other._w * _x) - (other._x * _w) - (other._y * _z) - (other._z * _y);
	tmp._y = (other._w * _y) - (other._y * _w) - (other._z * _x) - (other._x * _z);
	tmp._z = (other._w * _z) - (other._z * _w) - (other._x * _y) - (other._y * _x);

	return tmp;
}

// multiplication operator
inline Quaternion Quaternion::operator*(float s) const
{
	return Quaternion(s*_x, s*_y, s*_z, s*_w);
}

// multiplication operator
inline Quaternion& Quaternion::operator*=(const Quaternion& other)
{
	*this = other * (*this);
	return *this;
}

// add operator
inline Quaternion Quaternion::operator+(const Quaternion& b) const
{
	return Quaternion(_x + b._x, _y + b._y, _z + b._z, _w + b._w);
}

// Creates a matrix from this quaternion
inline Matrix4x4 Quaternion::getMatrix() const
{
	Matrix4x4 m;

	m(0, 0) = 1.0f - 2.0f*_y*_y - 2.0f*_z*_z;
	m(1, 0) = 2.0f*_x*_y + 2.0f*_z*_w;
	m(2, 0) = 2.0f*_x*_z - 2.0f*_y*_w;
	m(3, 0) = 0.0f;

	m(0, 1) = 2.0f*_x*_y - 2.0f*_z*_w;
	m(1, 1) = 1.0f - 2.0f*_x*_x - 2.0f*_z*_z;
	m(2, 1) = 2.0f*_z*_y + 2.0f*_x*_w;
	m(3, 1) = 0.0f;

	m(0, 2) = 2.0f*_x*_z + 2.0f*_y*_w;
	m(1, 2) = 2.0f*_z*_y - 2.0f*_x*_w;
	m(2, 2) = 1.0f - 2.0f*_x*_x - 2.0f*_y*_y;
	m(3, 2) = 0.0f;

	m(0, 3) = 0.0f;
	m(1, 3) = 0.0f;
	m(2, 3) = 0.0f;
	m(3, 3) = 1.0f;

	return m;
}

// Inverts this quaternion
inline void Quaternion::makeInverse()
{
	_x = -_x; _y = -_y; _z = -_z;
}

// calculates the dot product
inline float Quaternion::getDotProduct(const Quaternion& q2) const
{
	return (_x * q2._x) + (_y * q2._y) + (_z * q2._z) + (_w * q2._w);
}

// sets new quaternion 
inline void Quaternion::set(float x, float y, float z, float w)
{
	_x = x;
	_y = y;
	_z = z;
	_w = w;
}

// sets new quaternion based on euler angles
inline void Quaternion::set(float x, float y, float z)
{
	float angle;

	angle = x * 0.5f;
	float sr = (float)sin(angle);
	float cr = (float)cos(angle);

	angle = y * 0.5f;
	float sp = (float)sin(angle);
	float cp = (float)cos(angle);

	angle = z * 0.5f;
	float sy = (float)sin(angle);
	float cy = (float)cos(angle);

	float cpcy = cp * cy;
	float spcy = sp * cy;
	float cpsy = cp * sy;
	float spsy = cp * sy;

	_x = sr * cpcy - cr * spsy;
	_y = cr * spcy + sr * cpsy;
	_z = cr * cpsy - sr * spcy;
	_w = cr * cpcy + sr * spsy;

	normalize();
}

// normalizes the quaternion
inline Quaternion& Quaternion::normalize()
{
	float n = _x*_x + _y*_y + _z*_z + _w*_w;

	if (n == 1)
		return *this;

	n = 1.0f / sqrtf(n);
	_x *= n;
	_y *= n;
	_z *= n;
	_w *= n;

	return *this;
}

// Interpolates the quaternion between to quaternions based on time
inline Quaternion Quaternion::slerp(Quaternion q1, Quaternion q2, float time)
{
	if (q1 == q2 || time == 0)
		return q1;

	float angle = q1.getDotProduct(q2);

	if (angle < 0.0f)
	{
		angle *= -1.0f;
		q2 = Quaternion(-q2._w, -q2._x, -q2._y, -q2._z);
	}

	float invs = 1.0f - time;
	float s = time;

	if (1.0f - angle > 0.0000001f)
	{
		float theta = (float)acos(angle);
		float sinTheta = (float)sin(theta);

		invs = (float)sin(invs * theta) / sinTheta;
		s = (float)sin((time * theta)) / sinTheta;
	}

	*this = (q1*invs) + (q2*s);
	return *this;
}

// axis must be unit length, angle in radians
inline Quaternion& Quaternion::fromAngleAxis(float angle, const Vector3df& axis)
{
	const float fHalfAngle = 0.5f*angle;
	const float fSin = sinf(fHalfAngle);
	_w = cosf(fHalfAngle);
	_x = fSin*axis._x;
	_y = fSin*axis._y;
	_z = fSin*axis._z;
	return *this;
}

inline void Quaternion::toAngleAxis(float& angle, Vector3df& axis) const
{
	const float scale = sqrtf(_x*_x + _y*_y + _z*_z);

	if (isZero(scale) || _w > 1.0f || _w < -1.0f)
	{
		angle = 0.0f;
		axis._x = 0.0f;
		axis._y = 1.0f;
		axis._z = 0.0f;
	}
	else
	{
		const float invscale = 1.0f / scale;
		angle = 2.0f * acosf(_w);
		axis._x = _x * invscale;
		axis._y = _y * invscale;
		axis._z = _z * invscale;
	}
}

NS_VAYO_END

#endif // __VAYO_QUATERNION_H__
