/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 二维直线
\*************************************************************************/
#ifndef __VAYO_LINE2D_H__
#define __VAYO_LINE2D_H__

#include "VayoVector2d.h"

namespace Vayo {

	// 2D line between two points with intersection methods.
	template<typename T>
	class Line2d
	{
	public:
		// Default constructor for line going from (0,0) to (1,1).
		Line2d() : _start(0, 0), _end(1, 1) {}
		// Constructor for line between the two points.
		Line2d(T xa, T ya, T xb, T yb) : _start(xa, ya), _end(xb, yb) {}
		// Constructor for line between the two points given as vectors.
		Line2d(const Vector2d<T>& start, const Vector2d<T>& end) : _start(start), _end(end) {}
		// Copy constructor.
		Line2d(const Line2d<T>& other) : _start(other._start), _end(other._end) {}

		// operators

		Line2d<T> operator+(const Vector2d<T>& point) const { return Line2d<T>(_start + point, _end + point); }
		Line2d<T>& operator+=(const Vector2d<T>& point) { _start += point; _end += point; return *this; }

		Line2d<T> operator-(const Vector2d<T>& point) const { return Line2d<T>(_start - point, _end - point); }
		Line2d<T>& operator-=(const Vector2d<T>& point) { _start -= point; _end -= point; return *this; }

		bool operator==(const Line2d<T>& other) const
		{
			return (_start == other._start && _end == other._end) || (_end == other._start && _start == other._end);
		}
		bool operator!=(const Line2d<T>& other) const
		{
			return !((_start == other._start && _end == other._end) || (_end == other._start && _start == other._end));
		}

		// functions
		// Set this line to new line going through the two points.
		void setLine(const T& xa, const T& ya, const T& xb, const T& yb) { _start.set(xa, ya); _end.set(xb, yb); }
		// Set this line to new line going through the two points.
		void setLine(const Vector2d<T>& nstart, const Vector2d<T>& nend) { _start.set(nstart); _end.set(nend); }
		// Set this line to new line given as parameter.
		void setLine(const Line2d<T>& line) { _start.set(line._start); _end.set(line._end); }

		// Get length of line
		/** \return Length of the line. */
		T getLength() const { return _start.getDistanceFrom(_end); }

		// Get squared length of the line
		/** \return Squared length of line. */
		T getLengthSQ() const { return _start.getDistanceFromSQ(_end); }

		// Get middle of the line
		/** \return center of the line. */
		Vector2d<T> getMiddle() const
		{
			return (_start + _end) / (T)2;
		}

		// Get the vector of the line.
		/** \return The vector of the line. */
		Vector2d<T> getVector() const { return Vector2d<T>(_end._x - _start._x, _end._y - _start._y); }

		// Tests if this line intersects with another line.
		/** \param l: Other line to test intersection with.
		\param checkOnlySegments: Default is to check intersection between the begin and endpoints.
		When set to false the function will check for the first intersection point when extending the lines.
		\param out: If there is an intersection, the location of the
		intersection will be stored in this vector.
		\return True if there is an intersection, false if not. */
		bool intersectWith(const Line2d<T>& l, Vector2d<T>& out, bool checkOnlySegments = true) const
		{
			// Uses the method given at:
			const float commonDenominator = (float)(l._end._y - l._start._y)*(_end._x - _start._x) -
				(l._end._x - l._start._x)*(_end._y - _start._y);

			const float numeratorA = (float)(l._end._x - l._start._x)*(_start._y - l._start._y) -
				(l._end._y - l._start._y)*(_start._x - l._start._x);

			const float numeratorB = (float)(_end._x - _start._x)*(_start._y - l._start._y) -
				(_end._y - _start._y)*(_start._x - l._start._x);

			if (equals(commonDenominator, 0.0f))
			{
				// The lines are either coincident or parallel
				// if both numerators are 0, the lines are coincident
				if (equals(numeratorA, 0.0f) && equals(numeratorB, 0.0f))
				{
					// Try and find a common endpoint
					if (l._start == _start || l._end == _start)
						out = _start;
					else if (l._end == _end || l._start == _end)
						out = _end;
					// now check if the two segments are disjunct
					else if (l._start._x > _start._x && l._end._x > _start._x && l._start._x > _end._x && l._end._x > _end._x)
						return false;
					else if (l._start._y > _start._y && l._end._y > _start._y && l._start._y > _end._y && l._end._y > _end._y)
						return false;
					else if (l._start._x < _start._x && l._end._x < _start._x && l._start._x < _end._x && l._end._x < _end._x)
						return false;
					else if (l._start._y < _start._y && l._end._y < _start._y && l._start._y < _end._y && l._end._y < _end._y)
						return false;
					// else the lines are overlapping to some extent
					else
					{
						// find the points which are not contributing to the
						// common part
						Vector2d<T> maxp;
						Vector2d<T> minp;
						if ((_start._x > l._start._x && _start._x > l._end._x && _start._x > _end._x) || (_start._y > l._start._y && _start._y > l._end._y && _start._y > _end._y))
							maxp = _start;
						else if ((_end._x > l._start._x && _end._x > l._end._x && _end._x > _start._x) || (_end._y > l._start._y && _end._y > l._end._y && _end._y > _start._y))
							maxp = _end;
						else if ((l._start._x > _start._x && l._start._x > l._end._x && l._start._x > _end._x) || (l._start._y > _start._y && l._start._y > l._end._y && l._start._y > _end._y))
							maxp = l._start;
						else
							maxp = l._end;
						if (maxp != _start && ((_start._x < l._start._x && _start._x < l._end._x && _start._x < _end._x) || (_start._y < l._start._y && _start._y < l._end._y && _start._y < _end._y)))
							minp = _start;
						else if (maxp != _end && ((_end._x < l._start._x && _end._x < l._end._x && _end._x < _start._x) || (_end._y < l._start._y && _end._y < l._end._y && _end._y < _start._y)))
							minp = _end;
						else if (maxp != l._start && ((l._start._x < _start._x && l._start._x < l._end._x && l._start._x < _end._x) || (l._start._y < _start._y && l._start._y < l._end._y && l._start._y < _end._y)))
							minp = l._start;
						else
							minp = l._end;

						// one line is contained in the other. Pick the center
						// of the remaining points, which overlap for sure
						out = Vector2d<T>();
						if (_start != maxp && _start != minp)
							out += _start;
						if (_end != maxp && _end != minp)
							out += _end;
						if (l._start != maxp && l._start != minp)
							out += l._start;
						if (l._end != maxp && l._end != minp)
							out += l._end;
						out._x = (T)(out._x / 2);
						out._y = (T)(out._y / 2);
					}

					return true; // coincident
				}

				return false; // parallel
			}

			// Get the point of intersection on this line, checking that
			// it is within the line segment.
			const float uA = numeratorA / commonDenominator;
			if (checkOnlySegments && (uA < 0.f || uA > 1.f))
				return false; // Outside the line segment

			const float uB = numeratorB / commonDenominator;
			if (checkOnlySegments && (uB < 0.f || uB > 1.f))
				return false; // Outside the line segment

			// Calculate the intersection point.
			out._x = (T)(_start._x + uA * (_end._x - _start._x));
			out._y = (T)(_start._y + uA * (_end._y - _start._y));
			return true;
		}

		// Get unit vector of the line.
		/** \return Unit vector of this line. */
		Vector2d<T> getUnitVector() const
		{
			T len = (T)(1.0 / getLength());
			return Vector2d<T>((_end._x - _start._x) * len, (_end._y - _start._y) * len);
		}

		// Get angle between this line and given line.
		/** \param l Other line for test.
		\return Angle in degrees. */
		double getAngleWith(const Line2d<T>& l) const
		{
			Vector2d<T> vect = getVector();
			Vector2d<T> vect2 = l.getVector();
			return vect.getAngleWith(vect2);
		}

		// Tells us if the given point lies to the left, right, or on the line.
		/** \return 0 if the point is on the line
		<0 if to the left, or >0 if to the right. */
		T getPointOrientation(const Vector2d<T>& point) const
		{
			return ((_end._x - _start._x) * (point._y - _start._y) -
				(point._x - _start._x) * (_end._y - _start._y));
		}

		// Check if the given point is a member of the line
		/** \return True if point is between start and end, else false. */
		bool isPointOnLine(const Vector2d<T>& point) const
		{
			T d = getPointOrientation(point);
			return (d == 0 && point.isBetweenPoints(_start, _end));
		}

		// Check if the given point is between start and end of the line.
		/** Assumes that the point is already somewhere on the line. */
		bool isPointBetweenStartAndEnd(const Vector2d<T>& point) const
		{
			return point.isBetweenPoints(_start, _end);
		}

		// Get the closest point on this line to a point
		/** \param checkOnlySegments: Default (true) is to return a point on the line-segment (between begin and end) of the line.
		When set to false the function will check for the first the closest point on the the line even when outside the segment. */
		Vector2d<T> getClosestPoint(const Vector2d<T>& point, bool checkOnlySegments = true) const
		{
			Vector2d<double> c((double)(point._x - _start._x), (double)(point._y - _start._y));
			Vector2d<double> v((double)(_end._x - _start._x), (double)(_end._y - _start._y));
			double d = v.getLength();
			if (d == 0)	// can't tell much when the line is just a single point
				return _start;
			v /= d;
			double t = v.dotProduct(c);

			if (checkOnlySegments)
			{
				if (t < 0) return Vector2d<T>((T)_start._x, (T)_start._y);
				if (t > d) return Vector2d<T>((T)_end._x, (T)_end._y);
			}

			v *= t;
			return Vector2d<T>((T)(_start._x + v._x), (T)(_start._y + v._y));
		}

		// Start point of the line.
		Vector2d<T> _start;
		// End point of the line.
		Vector2d<T> _end;
	};

	// partial specialization to optimize <float> lines (avoiding casts)
	template<>
	inline Vector2df Line2d<float>::getClosestPoint(const Vector2df& point, bool checkOnlySegments) const
	{
		Vector2df c = point - _start;
		Vector2df v = _end - _start;
		float d = (float)v.getLength();
		if (d == 0)	// can't tell much when the line is just a single point
			return _start;
		v /= d;
		float t = v.dotProduct(c);

		if (checkOnlySegments)
		{
			if (t < 0) return _start;
			if (t > d) return _end;
		}

		v *= t;
		return _start + v;
	}

	// Typedef for an float line.
	typedef Line2d<float> Line2df;
	// Typedef for an integer line.
	typedef Line2d<int> Line2di;

}

#endif // __VAYO_LINE2D_H__
