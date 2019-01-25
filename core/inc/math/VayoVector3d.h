/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 三维向量
\*************************************************************************/
#ifndef __VAYO_VECTOR3D_H__
#define __VAYO_VECTOR3D_H__

#include "VayoMath.h"

namespace Vayo {

	template<typename T>
	class Vector3d
	{
	public:
		Vector3d() : _x(0), _y(0), _z(0) {}
		Vector3d(T x, T y, T z) : _x(x), _y(y), _z(z) {}
		Vector3d(const Vector3d<T>& other) :_x(other._x), _y(other._y), _z(other._z) {}

		// operators

		Vector3d<T>& operator=(const Vector3d<T>& other) { _x = other._x; _y = other._y; _z = other._z; return *this; }

		Vector3d<T> operator+(const Vector3d<T>& other) const { return Vector3d<T>(_x + other._x, _y + other._y, _z + other._z); }
		Vector3d<T>& operator+=(const Vector3d<T>& other) { _x += other._x; _y += other._y; _z += other._z; return *this; }

		Vector3d<T> operator-(const Vector3d<T>& other) const { return Vector3d<T>(_x - other._x, _y - other._y, _z - other._z); }
		Vector3d<T>& operator-=(const Vector3d<T>& other) { _x -= other._x; _y -= other._y; _z -= other._z; return *this; }

		Vector3d<T> operator*(const Vector3d<T>& other) const { return Vector3d<T>(_x * other._x, _y * other._y, _z * other._z); }
		Vector3d<T>& operator*=(const Vector3d<T>& other) { _x *= other._x; _y *= other._y; _z *= other._z; return *this; }
		Vector3d<T> operator*(const T v) const { return Vector3d<T>(_x * v, _y * v, _z * v); }
		Vector3d<T>& operator*=(const T v) { _x *= v; _y *= v; _z *= v; return *this; }

		Vector3d<T> operator/(const Vector3d<T>& other) const { return Vector3d<T>(_x / other._x, _y / other._y, _z / other._z); }
		Vector3d<T>& operator/=(const Vector3d<T>& other) { _x /= other._x; _y /= other._y; _z /= other._z; return *this; }
		Vector3d<T> operator/(const T v) const { T i = (T)1.0 / v; return Vector3d<T>(_x * i, _y * i, _z * i); }
		Vector3d<T>& operator/=(const T v) { T i = (T)1.0 / v; _x *= i; _y *= i; _z *= i; return *this; }

		bool operator<=(const Vector3d<T>&other) const { return _x <= other._x && _y <= other._y && _z <= other._z; }
		bool operator>=(const Vector3d<T>&other) const { return _x >= other._x && _y >= other._y && _z >= other._z; }

		bool operator==(const Vector3d<T>& other) const { return equals(_x, other._x) && equals(_y, other._y) && equals(_z, other._z); }
		bool operator!=(const Vector3d<T>& other) const { return !equals(_x, other._x) || !equals(_y, other._y) || !equals(_z, other._z); }

		inline friend wstringstream& operator<<(wstringstream& ss, const Vector3d<T>& in)
		{
			ss << in._x << L" " << in._y << L" " << in._z << L" ";
			return ss;
		}

		inline friend wstringstream& operator >> (wstringstream& ss, Vector3d<T>& out)
		{
			ss >> out._x >> out._y >> out._z;
			return ss;
		}

		// functions

		void set(const T x, const T y, const T z) { _x = x; _y = y; _z = z; }
		void set(const Vector3d<T>& p) { _x = p._x; _y = p._y; _z = p._z; }

		float getLength() const { return sqrtf(_x*_x + _y*_y + _z*_z); }

		float getLengthSQ() const { return _x*_x + _y*_y + _z*_z; }

		T dotProduct(const Vector3d<T>& other) const
		{
			return _x*other._x + _y*other._y + _z*other._z;
		}

		float getDistanceFrom(const Vector3d<T>& other) const
		{
			float vx = _x - other._x; float vy = _y - other._y; float vz = _z - other._z;
			return sqrtf(vx*vx + vy*vy + vz*vz);
		}

		float getDistanceFromSQ(const Vector3d<T>& other) const
		{
			float vx = _x - other._x; float vy = _y - other._y; float vz = _z - other._z;
			return (vx*vx + vy*vy + vz*vz);
		}

		Vector3d<T> crossProduct(const Vector3d<T>& p) const
		{
			return Vector3d<T>(_y * p._z - _z * p._y, _z * p._x - _x * p._z, _x * p._y - _y * p._x);
		}

		bool isBetweenPoints(const Vector3d<T>& begin, const Vector3d<T>& end) const
		{
			float f = (float)(end - begin).getLengthSQ();
			return (float)getDistanceFromSQ(begin) < f &&
				(float)getDistanceFromSQ(end) < f;
		}

		Vector3d<T>& normalize()
		{
			T inv = (T)1.0 / (T)getLength();
			_x *= inv;
			_y *= inv;
			_z *= inv;
			return *this;
		}

		void setLength(T newlength)
		{
			normalize();
			*this *= newlength;
		}

		void invert()
		{
			_x *= -1.0f;
			_y *= -1.0f;
			_z *= -1.0f;
		}

		void rotateXZBy(double degrees, const Vector3d<T>& center)
		{
			degrees = m3dDegToRad(degrees);
			T cs = (T)cos(degrees);
			T sn = (T)sin(degrees);
			_x -= center._x;
			_z -= center._z;
			set(_x*cs - _z*sn, _y, _x*sn + _z*cs);
			_x += center._x;
			_z += center._z;
		}

		void rotateXYBy(double degrees, const Vector3d<T>& center)
		{
			degrees = m3dDegToRad(degrees);
			T cs = (T)cos(degrees);
			T sn = (T)sin(degrees);
			_x -= center._x;
			_y -= center._y;
			set(_x*cs - _y*sn, _x*sn + _y*cs, _z);
			_x += center._x;
			_y += center._y;
		}

		void rotateYZBy(double degrees, const Vector3d<T>& center)
		{
			degrees = m3dDegToRad(degrees);
			T cs = (T)cos(degrees);
			T sn = (T)sin(degrees);
			_z -= center._z;
			_y -= center._y;
			set(_x, _y*cs - _z*sn, _y*sn + _z*cs);
			_z += center._z;
			_y += center._y;
		}

		Vector3d<T> getInterpolated(const Vector3d<T>& other, float d) const
		{
			float inv = 1.0f - d;
			return Vector3d<T>(other._x*inv + _x*d, other._y*inv + _y*d, other._z*inv + _z*d);
		}

		// member variables

		T _x, _y, _z;

		static const Vector3d<T> XAxis;
		static const Vector3d<T> YAxis;
		static const Vector3d<T> ZAxis;
		static const Vector3d<T> Origin;
	};

	template<typename T>
	const Vector3d<T> Vector3d<T>::XAxis(1, 0, 0);
	template<typename T>
	const Vector3d<T> Vector3d<T>::YAxis(0, 1, 0);
	template<typename T>
	const Vector3d<T> Vector3d<T>::ZAxis(0, 0, 1);
	template<typename T>
	const Vector3d<T> Vector3d<T>::Origin(0, 0, 0);

#define     Position3d        Vector3d
	typedef Vector3d<float>   Vector3df;
	typedef Vector3d<int>     Vector3di;
	typedef Position3d<float> Position3df;
	typedef Position3d<int>   Position3di;

}

#endif // __VAYO_VECTOR3D_H__
