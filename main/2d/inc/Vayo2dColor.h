/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 颜色
\*************************************************************************/
#ifndef __VAYO2D_COLOR_H__
#define __VAYO2D_COLOR_H__

#include "Vayo2dSupport.h"
NS_VAYO2D_BEGIN

inline void colorClamp(float& x)
{
	if (x < 0.0f) x = 0.0f;
	if (x > 1.0f) x = 1.0f;
}

class Color
{
public:
	float _r, _g, _b, _a;

	Color(const float r, const float g, const float b, const float a = 1.0f)
	{
		_r = r;
		_g = g;
		_b = b;
		_a = a;
	}

	Color(const unsigned int col)
	{
		setHWColor(col);
	}

	Color()
	{
		_r = _g = _b = _a = 0;
	}

	Color operator-(const Color& c) const
	{
		return Color(_r - c._r, _g - c._g, _b - c._b, _a - c._a);
	}

	Color operator+(const Color& c) const
	{
		return Color(_r + c._r, _g + c._g, _b + c._b, _a + c._a);
	}

	Color operator*(const Color& c) const
	{
		return Color(_r * c._r, _g * c._g, _b * c._b, _a * c._a);
	}

	Color& operator-=(const Color& c)
	{
		_r -= c._r;
		_g -= c._g;
		_b -= c._b;
		_a -= c._a;
		return *this;
	}

	Color& operator+=(const Color& c)
	{
		_r += c._r;
		_g += c._g;
		_b += c._b;
		_a += c._a;
		return *this;
	}

	bool operator==(const Color& c) const
	{
		return (_r == c._r && _g == c._g && _b == c._b && _a == c._a);
	}

	bool operator!=(const Color& c) const
	{
		return (_r != c._r || _g != c._g || _b != c._b || _a != c._a);
	}

	Color operator/(const float scalar) const
	{
		return Color(_r / scalar, _g / scalar, _b / scalar, _a / scalar);
	}

	Color operator*(const float scalar) const
	{
		return Color(_r * scalar, _g * scalar, _b * scalar, _a * scalar);
	}

	Color& operator*=(const float scalar)
	{
		_r *= scalar;
		_g *= scalar;
		_b *= scalar;
		_a *= scalar;
		return *this;
	}

	void clamp()
	{
		colorClamp(_r);
		colorClamp(_g);
		colorClamp(_b);
		colorClamp(_a);
	}

	void setHWColor(const unsigned int col)
	{
		_a = (col >> 24) / 255.0f;
		_r = ((col >> 16) & 0xFF) / 255.0f;
		_g = ((col >> 8) & 0xFF) / 255.0f;
		_b = (col & 0xFF) / 255.0f;
	}

	unsigned int getHWColor() const
	{
		return (unsigned(_a * 255.0f) << 24) + (unsigned(_r * 255.0f) << 16)
			+ (unsigned(_g * 255.0f) << 8) + unsigned(_b * 255.0f);
	}
};

inline Color operator*(const float sc, const Color& c)
{
	return c * sc;
}

NS_VAYO2D_END

#endif // __VAYO2D_COLOR_H__
