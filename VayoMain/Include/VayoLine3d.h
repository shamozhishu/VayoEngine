/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 三维直线
\*************************************************************************/
#ifndef __VAYO_LINE3D_H__
#define __VAYO_LINE3D_H__

#include "VayoVector3d.h"
NS_VAYO_BEGIN

// 3D line between two points with intersection methods.
template<typename T>
class Line3d
{
public:
	Line3d() : _start(0,0,0), _end(1,1,1) {}
	Line3d(T xa, T ya, T za, T xb, T yb, T zb) : _start(xa, ya, za), _end(xb, yb, zb) {}
	Line3d(const Vector3d<T>& start, const Vector3d<T>& end) : _start(start), _end(end) {}

	// operators

	Line3d<T> operator+(const Vector3d<T>& point) const { return Line3d<T>(_start + point, _end + point); }
	Line3d<T>& operator+=(const Vector3d<T>& point) { _start += point; _end += point; return *this; }

	Line3d<T> operator-(const Vector3d<T>& point) const { return Line3d<T>(_start - point, _end - point); }
	Line3d<T>& operator-=(const Vector3d<T>& point) { _start -= point; _end -= point; return *this; }

	bool operator==(const Line3d<T>& other) const
	{ return (_start==other._start && _end==other._end) || (_end==other._start && _start==other._end);}
	bool operator!=(const Line3d<T>& other) const
	{ return !((_start==other._start && _end==other._end) || (_end==other._start && _start==other._end));}

	// functions
	// Set this line to a new line going through the two points.
	void setLine(const T& xa, const T& ya, const T& za, const T& xb, const T& yb, const T& zb)
	{_start.set(xa, ya, za); _end.set(xb, yb, zb);}
	// Set this line to a new line going through the two points.
	void setLine(const Vector3d<T>& start, const Vector3d<T>& end)
	{_start.set(start); _end.set(end);}
	// Set this line to new line given as parameter.
	void setLine(const Line3d<T>& line)
	{_start.set(line._start); _end.set(line._end);}

	// Get length of line
	/** \return Length of line. */
	T getLength() const { return _start.getDistanceFrom(_end); }

	// Get squared length of line
	/** \return Squared length of line. */
	T getLengthSQ() const { return _start.getDistanceFromSQ(_end); }

	// Get middle of line
	/** \return Center of line. */
	Vector3d<T> getMiddle() const
	{
		return (_start + _end)/(T)2;
	}

	// Get vector of line
	/** \return vector of line. */
	Vector3d<T> getVector() const
	{
		return _end - _start;
	}

	// Check if the given point is between start and end of the line.
	/** Assumes that the point is already somewhere on the line.
	\param point The point to test.
	\return True if point is on the line between start and end, else false.
	*/
	bool isPointBetweenStartAndEnd(const Vector3d<T>& point) const
	{
		return point.isBetweenPoints(_start, _end);
	}

	// Get the closest point on this line to a point
	/** \param point The point to compare to.
	\return The nearest point which is part of the line. */
	Vector3d<T> getClosestPoint(const Vector3d<T>& point) const
	{
		Vector3d<T> c = point - _start;
		Vector3d<T> v = _end - _start;
		T d = (T)v.getLength();
		v /= d;
		T t = v.dotProduct(c);

		if (t < (T)0.0)
			return _start;
		if (t > d)
			return _end;

		v *= t;
		return _start + v;
	}

	// Check if the line intersects with a shpere
	/** \param sorigin: Origin of the shpere.
	\param sradius: Radius of the sphere.
	\param outdistance: The distance to the first intersection point.
	\return True if there is an intersection.
	If there is one, the distance to the first intersection point
	is stored in outdistance. */
	bool getIntersectionWithSphere(Vector3d<T> sorigin, T sradius, float& outdistance) const
	{
		const Vector3d<T> q = sorigin - _start;
		T c = q.getLength();
		T v = q.dotProduct(getVector().normalize());
		T d = sradius * sradius - (c*c - v*v);

		if (d < 0.0)
			return false;

		outdistance = v - sqrtf(d);
		return true;
	}

	// member variables

	// Start point of line
	Vector3d<T> _start;
	// End point of line
	Vector3d<T> _end;
};

typedef Line3d<float> Line3df;
typedef Line3d<int>   Line3di;

NS_VAYO_END

#endif // __VAYO_LINE3D_H__
