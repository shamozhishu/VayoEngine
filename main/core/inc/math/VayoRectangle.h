/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 矩形
\*************************************************************************/
#ifndef __VAYO_RECTANGLE_H__
#define __VAYO_RECTANGLE_H__

#include "VayoVector2d.h"
#include "VayoDimension2d.h"

namespace Vayo {

	template<typename T>
	class Rectangle
	{
	public:
		Rectangle()
			: _upperLeftCorner(0, 0), _lowerRightCorner(0, 0) {}


		Rectangle(T x, T y, T x2, T y2)
			: _upperLeftCorner(x, y), _lowerRightCorner(x2, y2) {}


		Rectangle(const Position2d<T>& upperLeft, const Position2d<T>& lowerRight)
			: _upperLeftCorner(upperLeft), _lowerRightCorner(lowerRight) {}

		Rectangle(const Rectangle<T>& other)
			: _upperLeftCorner(other._upperLeftCorner), _lowerRightCorner(other._lowerRightCorner) {}

		Rectangle(const Position2d<T>& pos, const Dimension2d<T>& size)
			: _upperLeftCorner(pos), _lowerRightCorner(pos._x + size._width, pos._y + size._height) {}


		Rectangle<T> operator+(const Position2d<T>& pos) const
		{
			Rectangle<T> ret(*this);
			ret._upperLeftCorner += pos;
			ret._lowerRightCorner += pos;
			return ret;
		}

		const Rectangle<T>& operator+=(const Position2d<T>& pos)
		{
			_upperLeftCorner += pos;
			_lowerRightCorner += pos;
			return *this;
		}

		bool operator==(const Rectangle<T>& other) const
		{
			return _upperLeftCorner == other._upperLeftCorner && _lowerRightCorner == other._lowerRightCorner;
		}


		bool operator!=(const Rectangle<T>& other) const
		{
			return _upperLeftCorner != other._upperLeftCorner || _lowerRightCorner != other._lowerRightCorner;
		}

		const Rectangle<T>& operator=(const Rectangle<T>& other)
		{
			_upperLeftCorner = other._upperLeftCorner;
			_lowerRightCorner = other._lowerRightCorner;
			return *this;
		}

		bool isPointInside(const Position2d<T>& pos) const
		{
			return _upperLeftCorner._x <= pos._x && _upperLeftCorner._y <= pos._y &&
				_lowerRightCorner._x >= pos._x && _lowerRightCorner._y >= pos._y;
		}

		bool isRectCollided(const Rectangle<T>& other) const
		{
			return (_lowerRightCorner._y > other._upperLeftCorner._y && _upperLeftCorner._y < other._lowerRightCorner._y &&
				_lowerRightCorner._x > other._upperLeftCorner._x && _upperLeftCorner._x < other._lowerRightCorner._x);
		}

		void clipAgainst(const Rectangle<T>& other)
		{
			if (other._lowerRightCorner._x < _lowerRightCorner._x)
				_lowerRightCorner._x = other._lowerRightCorner._x;
			if (other._lowerRightCorner._y < _lowerRightCorner._y)
				_lowerRightCorner._y = other._lowerRightCorner._y;

			if (other._upperLeftCorner._x > _upperLeftCorner._x)
				_upperLeftCorner._x = other._upperLeftCorner._x;
			if (other._upperLeftCorner._y > _upperLeftCorner._y)
				_upperLeftCorner._y = other._upperLeftCorner._y;
		}

		T getWidth() const
		{
			return _lowerRightCorner._x - _upperLeftCorner._x;
		}

		T getHeight() const
		{
			return _lowerRightCorner._y - _upperLeftCorner._y;
		}

		void repair()
		{
			if (_lowerRightCorner._x < _upperLeftCorner._x)
			{
				int t = _lowerRightCorner._x;
				_lowerRightCorner._x = _upperLeftCorner._x;
				_upperLeftCorner._x = t;
			}

			if (_lowerRightCorner._y < _upperLeftCorner._y)
			{
				int t = _lowerRightCorner._y;
				_lowerRightCorner._y = _upperLeftCorner._y;
				_upperLeftCorner._y = t;
			}
		}

		bool isValid() const
		{
			return ((_lowerRightCorner._x >= _upperLeftCorner._x) &&
				(_lowerRightCorner._y >= _upperLeftCorner._y));
		}

		Position2d<T> getPos() const
		{
			return _upperLeftCorner;
		}

		Position2d<T> getCenter() const
		{
			return Position2d<T>(
				(_upperLeftCorner._x + _lowerRightCorner._x) / 2,
				(_upperLeftCorner._y + _lowerRightCorner._y) / 2);
		}

		Dimension2d<T> getSize() const
		{
			return Dimension2d<T>(getWidth(), getHeight());
		}

		Position2d<T> _upperLeftCorner;
		Position2d<T> _lowerRightCorner;
	};

	typedef Rectangle<float> Rectf;
	typedef Rectangle<int>   Recti;

}

#endif // __VAYO_RECTANGLE_H__
