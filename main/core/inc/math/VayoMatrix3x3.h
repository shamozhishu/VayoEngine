/*************************************************************************\
* Õ˚”—“˝«Ê1.0
* Copyright (c) 2018-2019 authored by ÷Ïº”∫∆
* 3x3æÿ’Û
\*************************************************************************/
#ifndef __VAYO_MATRIX3X3_H__
#define __VAYO_MATRIX3X3_H__

#include "VayoVector2d.h"
#include "VayoRectangle.h"

namespace Vayo {

	class Matrix3x3
	{
	public:
		Matrix3x3();

		float& operator()(int row, int col) { return _m[col * 3 + row]; }
		const float& operator()(int row, int col) const { return _m[col * 3 + row]; }
		float& operator[](unsigned int index) { return _m[index]; }
		const float& operator[](unsigned int index) const { return _m[index]; }
		Matrix3x3& operator=(const Matrix3x3 &other);
		bool operator==(const Matrix3x3 &other) const;
		bool operator!=(const Matrix3x3 &other) const;
		Matrix3x3& operator*=(const Matrix3x3& other);
		Matrix3x3 operator*(const Matrix3x3& other) const;

		void makeIdentity();

		Vector2df getTranslation() const;
		void setTranslation(const Vector2df& translation);
		void setInverseTranslation(const Vector2df& translation);

		void setRotationDegrees(float angle);
		void setRotationRadians(float angle);

		Vector2df getScale() const;
		void setScale(const Vector2df& scale);

		void transformVect(Vector2df& vect) const;
		void transformVect(const Vector2df& in, Vector2df& out) const;
		void translateVect(Vector2df& vect) const;
		void inverseTranslateVect(Vector2df& vect) const;
		void rotateVect(Vector2df& vect) const;
		void inverseRotateVect(Vector2df& vect) const;

		void transformArea(Rectf& area) const;

		float determinant() const;
		bool  makeInverse();
		bool  getInverse(Matrix3x3& out) const;

		Matrix3x3& adjugate();
		Matrix3x3  adjugate() const;
		Matrix3x3& transpose();
		Matrix3x3  transpose() const;
		Matrix3x3  interpolate(Matrix3x3& b, float time);

		float* pointer() { return _m; }
		const float* pointer() const { return _m; }

	private:
		float _m[9];
	};

	static const Matrix3x3 IdentityMat3;

	inline Matrix3x3::Matrix3x3()
	{
		makeIdentity();
	}

	inline Matrix3x3& Matrix3x3::operator=(const Matrix3x3 &other)
	{
		for (int i = 0; i < 9; ++i)
			_m[i] = other._m[i];

		return *this;
	}

	inline bool Matrix3x3::operator==(const Matrix3x3 &other) const
	{
		for (int i = 0; i < 9; ++i)
			if (!equals(_m[i], other._m[i]))
				return false;

		return true;
	}

	inline bool Matrix3x3::operator!=(const Matrix3x3 &other) const
	{
		return !(*this == other);
	}

	inline Matrix3x3& Matrix3x3::operator*=(const Matrix3x3& other)
	{
		float newMatrix[9];
		const float *m1 = _m, *m2 = other._m;

		newMatrix[0] = m1[0] * m2[0] + m1[3] * m2[1] + m1[6] * m2[2];
		newMatrix[1] = m1[1] * m2[0] + m1[4] * m2[1] + m1[7] * m2[2];
		newMatrix[2] = m1[2] * m2[0] + m1[5] * m2[1] + m1[8] * m2[2];

		newMatrix[3] = m1[0] * m2[3] + m1[3] * m2[4] + m1[6] * m2[5];
		newMatrix[4] = m1[1] * m2[3] + m1[4] * m2[4] + m1[7] * m2[5];
		newMatrix[5] = m1[2] * m2[3] + m1[5] * m2[4] + m1[8] * m2[5];

		newMatrix[6] = m1[0] * m2[6] + m1[3] * m2[7] + m1[6] * m2[8];
		newMatrix[7] = m1[1] * m2[6] + m1[4] * m2[7] + m1[7] * m2[8];
		newMatrix[8] = m1[2] * m2[6] + m1[5] * m2[7] + m1[8] * m2[8];

		for (int i = 0; i < 9; ++i)
			_m[i] = newMatrix[i];

		return *this;
	}

	inline Matrix3x3 Matrix3x3::operator*(const Matrix3x3& other) const
	{
		Matrix3x3 tmtrx;
		const float *m1 = _m, *m2 = other._m;
		float *m3 = tmtrx._m;

		m3[0] = m1[0] * m2[0] + m1[3] * m2[1] + m1[6] * m2[2];
		m3[1] = m1[1] * m2[0] + m1[4] * m2[1] + m1[7] * m2[2];
		m3[2] = m1[2] * m2[0] + m1[5] * m2[1] + m1[8] * m2[2];

		m3[3] = m1[0] * m2[3] + m1[3] * m2[4] + m1[6] * m2[5];
		m3[4] = m1[1] * m2[3] + m1[4] * m2[4] + m1[7] * m2[5];
		m3[5] = m1[2] * m2[3] + m1[5] * m2[4] + m1[8] * m2[5];

		m3[6] = m1[0] * m2[6] + m1[3] * m2[7] + m1[6] * m2[8];
		m3[7] = m1[1] * m2[6] + m1[4] * m2[7] + m1[7] * m2[8];
		m3[8] = m1[2] * m2[6] + m1[5] * m2[7] + m1[8] * m2[8];

		return tmtrx;
	}

	inline void Matrix3x3::makeIdentity()
	{
		for (int i = 0; i < 9; ++i)
			_m[i] = 0.0f;
		_m[0] = _m[4] = _m[8] = 1;
	}

	inline Vector2df Matrix3x3::getTranslation() const
	{
		return Vector2df(_m[6], _m[7]);
	}

	inline void Matrix3x3::setTranslation(const Vector2df& translation)
	{
		_m[6] = translation._x;
		_m[7] = translation._y;
	}

	inline void Matrix3x3::setInverseTranslation(const Vector2df& translation)
	{
		_m[6] = -translation._x;
		_m[7] = -translation._y;
	}

	inline void Matrix3x3::setRotationDegrees(float angle)
	{
		setRotationRadians(VayoDegToRad(angle));
	}

	inline void Matrix3x3::setRotationRadians(float angle)
	{
		_m[0] = cosf(angle);
		_m[1] = sinf(angle);
		_m[3] = -sinf(angle);
		_m[4] = cosf(angle);
	}

	inline Vector2df Matrix3x3::getScale() const
	{
		if (isZero(_m[1]) && isZero(_m[3]))
			return Vector2df(_m[0], _m[4]);

		return Vector2df(sqrtf(_m[0] * _m[0] + _m[1] * _m[1]), sqrtf(_m[3] * _m[3] + _m[4] * _m[4]));
	}

	inline void Matrix3x3::setScale(const Vector2df& scale)
	{
		_m[0] = scale._x;
		_m[4] = scale._y;
	}

	inline void Matrix3x3::transformVect(Vector2df& vect) const
	{
		float vector[2];

		vector[0] = vect._x*_m[0] + vect._y*_m[3] + _m[6];
		vector[1] = vect._x*_m[1] + vect._y*_m[4] + _m[7];

		vect._x = vector[0];
		vect._y = vector[1];
	}

	inline void Matrix3x3::transformVect(const Vector2df& in, Vector2df& out) const
	{
		out._x = in._x*_m[0] + in._y*_m[3] + _m[6];
		out._y = in._x*_m[1] + in._y*_m[4] + _m[7];
	}

	inline void Matrix3x3::translateVect(Vector2df& vect) const
	{
		vect._x += _m[6];
		vect._y += _m[7];
	}

	inline void Matrix3x3::inverseTranslateVect(Vector2df& vect) const
	{
		vect._x -= _m[6];
		vect._y -= _m[7];
	}

	inline void Matrix3x3::rotateVect(Vector2df& vect) const
	{
		Vector2df tmp = vect;
		vect._x = tmp._x*_m[0] + tmp._y*_m[3];
		vect._y = tmp._x*_m[1] + tmp._y*_m[4];
	}

	inline void Matrix3x3::inverseRotateVect(Vector2df& vect) const
	{
		Vector2df tmp = vect;
		vect._x = tmp._x*_m[0] + tmp._y*_m[1];
		vect._y = tmp._x*_m[3] + tmp._y*_m[4];
	}

	inline void Matrix3x3::transformArea(Rectf& area) const
	{

	}

	inline float Matrix3x3::determinant() const
	{
		float output;
		output = _m[0] * _m[4] * _m[8] + _m[1] * _m[5] * _m[6] + _m[2] * _m[3] * _m[7];
		output -= _m[2] * _m[4] * _m[6] + _m[0] * _m[5] * _m[7] + _m[1] * _m[3] * _m[8];
		return output;
	}

	inline bool Matrix3x3::makeInverse()
	{
		Matrix3x3 temp;

		if (getInverse(temp))
		{
			*this = temp;
			return true;
		}

		return false;
	}

	inline bool Matrix3x3::getInverse(Matrix3x3& out) const
	{
		float detInv = determinant();
		if (isZero(detInv))
			return false;

		detInv = (float)(1.0 / (double)detInv);
		Matrix3x3 mat;
		mat = adjugate();

		for (int i = 0; i < 9; ++i)
			out._m[i] = mat[i] * detInv;

		return true;
	}

	inline Matrix3x3& Matrix3x3::adjugate()
	{
		Matrix3x3 temp = *this;
		_m[0] = temp._m[4] * temp._m[8] - temp._m[5] * temp._m[7];
		_m[1] = temp._m[2] * temp._m[7] - temp._m[1] * temp._m[8];
		_m[2] = temp._m[1] * temp._m[5] - temp._m[2] * temp._m[4];
		_m[3] = temp._m[5] * temp._m[6] - temp._m[3] * temp._m[8];
		_m[4] = temp._m[0] * temp._m[8] - temp._m[2] * temp._m[6];
		_m[5] = temp._m[2] * temp._m[3] - temp._m[0] * temp._m[5];
		_m[6] = temp._m[3] * temp._m[7] - temp._m[4] * temp._m[6];
		_m[7] = temp._m[1] * temp._m[6] - temp._m[0] * temp._m[7];
		_m[8] = temp._m[0] * temp._m[4] - temp._m[1] * temp._m[3];
		return *this;
	}

	inline Matrix3x3 Matrix3x3::adjugate() const
	{
		Matrix3x3 temp;
		temp._m[0] = _m[4] * _m[8] - _m[5] * _m[7];
		temp._m[1] = _m[2] * _m[7] - _m[1] * _m[8];
		temp._m[2] = _m[1] * _m[5] - _m[2] * _m[4];
		temp._m[3] = _m[5] * _m[6] - _m[3] * _m[8];
		temp._m[4] = _m[0] * _m[8] - _m[2] * _m[6];
		temp._m[5] = _m[2] * _m[3] - _m[0] * _m[5];
		temp._m[6] = _m[3] * _m[7] - _m[4] * _m[6];
		temp._m[7] = _m[1] * _m[6] - _m[0] * _m[7];
		temp._m[8] = _m[0] * _m[4] - _m[1] * _m[3];
		return temp;
	}

	inline Matrix3x3& Matrix3x3::transpose()
	{
		Matrix3x3 temp = *this;
		int z, x;
		for (z = 0; z < 3; ++z) {
			for (x = 0; x < 3; ++x) {
				_m[(z * 3) + x] = temp._m[(x * 3) + z];
			}
		}

		return *this;
	}

	inline Matrix3x3 Matrix3x3::transpose() const
	{
		Matrix3x3 temp;
		int z, x;
		for (z = 0; z < 3; ++z) {
			for (x = 0; x < 3; ++x) {
				temp._m[(z * 3) + x] = _m[(x * 3) + z];
			}
		}

		return temp;
	}

	inline Matrix3x3 Matrix3x3::interpolate(Matrix3x3& b, float time)
	{
		float ntime = 1.0f - time;
		Matrix3x3 mat;
		for (int i = 0; i < 9; ++i)
			mat._m[i] = _m[i] * ntime + b._m[i] * time;

		return mat;
	}

}

#endif // __VAYO_MATRIX3X3_H__
