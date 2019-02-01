/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 颜色
\*************************************************************************/
#ifndef __VAYO2D_COLOR_H__
#define __VAYO2D_COLOR_H__

inline void colorClamp(float& x)
{
	if (x < 0.0f) x = 0.0f;
	if (x > 1.0f) x = 1.0f;
}

class Color
{
public:
	float r, g, b, a;

	Color(const float _r, const float _g, const float _b, const float _a)
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}

	Color(const unsigned int col)
	{
		setHWColor(col);
	}

	Color()
	{
		r = g = b = a = 0;
	}

	Color operator-(const Color& c) const
	{
		return Color(r - c.r, g - c.g, b - c.b, a - c.a);
	}

	Color operator+(const Color& c) const
	{
		return Color(r + c.r, g + c.g, b + c.b, a + c.a);
	}

	Color operator*(const Color& c) const
	{
		return Color(r * c.r, g * c.g, b * c.b, a * c.a);
	}

	Color& operator-=(const Color& c)
	{
		r -= c.r;
		g -= c.g;
		b -= c.b;
		a -= c.a;
		return *this;
	}

	Color& operator+=(const Color& c)
	{
		r += c.r;
		g += c.g;
		b += c.b;
		a += c.a;
		return *this;
	}

	bool operator==(const Color& c) const
	{
		return (r == c.r && g == c.g && b == c.b && a == c.a);
	}

	bool operator!=(const Color& c) const
	{
		return (r != c.r || g != c.g || b != c.b || a != c.a);
	}

	Color operator/(const float scalar) const
	{
		return Color(r / scalar, g / scalar, b / scalar, a / scalar);
	}

	Color operator*(const float scalar) const
	{
		return Color(r * scalar, g * scalar, b * scalar, a * scalar);
	}

	Color& operator*=(const float scalar)
	{
		r *= scalar;
		g *= scalar;
		b *= scalar;
		a *= scalar;
		return *this;
	}

	void clamp()
	{
		colorClamp(r);
		colorClamp(g);
		colorClamp(b);
		colorClamp(a);
	}

	void setHWColor(const unsigned int col)
	{
		a = (col >> 24) / 255.0f;
		r = ((col >> 16) & 0xFF) / 255.0f;
		g = ((col >> 8) & 0xFF) / 255.0f;
		b = (col & 0xFF) / 255.0f;
	}

	unsigned int getHWColor() const
	{
		return (unsigned(a * 255.0f) << 24) + (unsigned(r * 255.0f) << 16)
			+ (unsigned(g * 255.0f) << 8) + unsigned(b * 255.0f);
	}
};

inline Color operator*(const float sc, const Color& c)
{
	return c * sc;
}

#endif // __VAYO2D_COLOR_H__
