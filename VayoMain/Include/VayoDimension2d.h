/*************************************************************************\
* ÍûÓÑÒýÇæ1.0
* Copyright (c) 2018-2018 author by Öì¼ÓºÆ
* ¶þÎ¬³ß´ç
\*************************************************************************/
#ifndef __VAYO_DIMENSION2D_H__
#define __VAYO_DIMENSION2D_H__

#include "VayoMath.h"
NS_VAYO_BEGIN

template<typename T>
class Dimension2d
{
public:
	Dimension2d()
		: _width(0), _height(0) {}

	Dimension2d(T width, T height)
		: _width(width), _height(height) {}

	Dimension2d(const Dimension2d<T>& other)
		: _width(other._width), _height(other._height) {}

	Dimension2d<T> operator+(const Dimension2d<T>& other) const
	{
		return Dimension2d<T>(_width + other._width, _height + other._height);
	}

	Dimension2d<T>& operator+=(const Dimension2d<T>& other)
	{
		_width += other._width;
		_height += other._height;
		return *this;
	}

	Dimension2d<T> operator-(const Dimension2d<T>& other) const
	{
		return Dimension2d<T>(_width - other._width, _height - other._height);
	}

	Dimension2d<T>& operator-=(const Dimension2d<T>& other)
	{
		_width -= other._width;
		_height -= other._height;
		return *this;
	}

	Dimension2d<T> operator*(const T& scale) const
	{
		return Dimension2d<T>(_width*scale, _height*scale);
	}

	Dimension2d<T>& operator*=(const T& scale)
	{
		_width *= scale;
		_height *= scale;
		return *this;
	}

	Dimension2d<T> operator/(const T& scale) const
	{
		return Dimension2d<T>(_width / scale, _height / scale);
	}

	Dimension2d<T>& operator/=(const T& scale)
	{
		_width /= scale;
		_height /= scale;
		return *this;
	}

	bool operator==(const Dimension2d<T>& other) const
	{
		return equals(_width, other._width) && equals(_height, other._height);
	}


	bool operator!=(const Dimension2d<T>& other) const
	{
		return !equals(_width, other._width) || !equals(_height, other._height);
	}

	const Dimension2d<T>& operator=(const Dimension2d<T>& other) 
	{
		_width = other._width;
		_height = other._height;
		return *this;
	}

	Dimension2d<T>& set(const T& width, const T& height)
	{
		_width = width;
		_height = height;
		return *this;
	}

	T _width, _height;
};

typedef Dimension2d<float>        Dimension2df;
typedef Dimension2d<int>          Dimension2di;
typedef Dimension2d<unsigned int> Dimension2du;

NS_VAYO_END

#endif // __VAYO_DIMENSION2D_H__
