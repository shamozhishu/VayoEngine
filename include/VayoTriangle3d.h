/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 author by 朱加浩
* 三角形
\*************************************************************************/
#ifndef __VAYO_TRIANGLE3D_H__
#define __VAYO_TRIANGLE3D_H__

#include "VayoAabbox3d.h"
NS_VAYO_BEGIN

// 3d triangle template class for doing collision detection and other things.
template <typename T>
class Triangle3d
{
public:
	Triangle3d() {}
	Triangle3d(Vector3d<T> v1, Vector3d<T> v2, Vector3d<T> v3) : _pointA(v1), _pointB(v2), _pointC(v3) {}

	// Equality operator
	bool operator==(const Triangle3d<T>& other) const
	{
		return other._pointA==_pointA && other._pointB==_pointB && other._pointC==_pointC;
	}

	// Inequality operator
	bool operator!=(const Triangle3d<T>& other) const
	{
		return !(*this==other);
	}

	// Determines if the triangle is totally inside a bounding box.
	/** \param box Box to check.
	\return True if triangle is within the box, otherwise false. */
	bool isTotalInsideBox(const Aabbox3d<T>& box) const
	{
		return (box.isPointInside(_pointA) &&
			box.isPointInside(_pointB) &&
			box.isPointInside(_pointC));
	}

	// Determines if the triangle is totally outside a bounding box.
	/** \param box Box to check.
	\return True if triangle is outside the box, otherwise false. */
	bool isTotalOutsideBox(const Aabbox3d<T>& box) const
	{
		return ((_pointA._x > box._maxEdge._x && _pointB._x > box._maxEdge._x && _pointC._x > box._maxEdge._x) ||

			(_pointA._y > box._maxEdge._y && _pointB._y > box._maxEdge._y && _pointC._y > box._maxEdge._y) ||
			(_pointA._z > box._maxEdge._z && _pointB._z > box._maxEdge._z && _pointC._z > box._maxEdge._z) ||
			(_pointA._x < box._minEdge._x && _pointB._x < box._minEdge._x && _pointC._x < box._minEdge._x) ||
			(_pointA._y < box._minEdge._y && _pointB._y < box._minEdge._y && _pointC._y < box._minEdge._y) ||
			(_pointA._z < box._minEdge._z && _pointB._z < box._minEdge._z && _pointC._z < box._minEdge._z));
	}

	// Get the closest point on a triangle to a point on the same plane.
	/** \param p Point which must be on the same plane as the triangle.
	\return The closest point of the triangle */
	Vector3d<T> closestPointOnTriangle(const Vector3d<T>& p) const
	{
		const Vector3d<T> rab = Line3d<T>(_pointA, _pointB).getClosestPoint(p);
		const Vector3d<T> rbc = Line3d<T>(_pointB, _pointC).getClosestPoint(p);
		const Vector3d<T> rca = Line3d<T>(_pointC, _pointA).getClosestPoint(p);

		const T d1 = rab.getDistanceFrom(p);
		const T d2 = rbc.getDistanceFrom(p);
		const T d3 = rca.getDistanceFrom(p);

		if (d1 < d2)
			return d1 < d3 ? rab : rca;

		return d2 < d3 ? rbc : rca;
	}

	// Check if a point is inside the triangle (border-points count also as inside)
	/*
	\param p Point to test. Assumes that this point is already
	on the plane of the triangle.
	\return True if the point is inside the triangle, otherwise false. */
	bool isPointInside(const Vector3d<T>& p) const
	{
		Vector3d<double> af64((double)_pointA._x, (double)_pointA._y, (double)_pointA._z);
		Vector3d<double> bf64((double)_pointB._x, (double)_pointB._y, (double)_pointB._z);
		Vector3d<double> cf64((double)_pointC._x, (double)_pointC._y, (double)_pointC._z);
		Vector3d<double> pf64((double)p._x, (double)p._y, (double)p._z);
		return (isOnSameSide(pf64, af64, bf64, cf64) &&
 			isOnSameSide(pf64, bf64, af64, cf64) &&
 			isOnSameSide(pf64, cf64, af64, bf64));
	}

	// Check if a point is inside the triangle (border-points count also as inside)
	/** This method uses a barycentric coordinate system.
	It is faster than isPointInside but is more susceptible to floating point rounding
	errors. This will especially be noticable when the FPU is in single precision mode
	(which is for example set on default by Direct3D).
	\param p Point to test. Assumes that this point is already
	on the plane of the triangle.
	\return True if point is inside the triangle, otherwise false. */
	bool isPointInsideFast(const Vector3d<T>& p) const
	{
		const Vector3d<T> a = _pointC - _pointA;
		const Vector3d<T> b = _pointB - _pointA;
		const Vector3d<T> c = p - _pointA;

		const double dotAA = a.dotProduct( a);
		const double dotAB = a.dotProduct( b);
		const double dotAC = a.dotProduct( c);
		const double dotBB = b.dotProduct( b);
		const double dotBC = b.dotProduct( c);

		// get coordinates in barycentric coordinate system
		const double invDenom =  1/(dotAA * dotBB - dotAB * dotAB);
		const double u = (dotBB * dotAC - dotAB * dotBC) * invDenom;
		const double v = (dotAA * dotBC - dotAB * dotAC ) * invDenom;

		// We count border-points as inside to keep downward compatibility.
		// Rounding-error also needed for some test-cases.
		return (u > -ROUNDING_ERROR_DOUBLE) && (v >= 0) && (u + v < 1+ROUNDING_ERROR_DOUBLE);
	}


	// Get an intersection with a 3d line.
	/** \param line Line to intersect with.
	\param outIntersection Place to store the intersection point, if there is one.
	\return True if there was an intersection, false if not. */
	bool getIntersectionWithLimitedLine(const Line3d<T>& line,
		Vector3d<T>& outIntersection) const
	{
		return getIntersectionWithLine(line.start,
			line.getVector(), outIntersection) &&
			outIntersection.isBetweenPoints(line.start, line.end);
	}


	// Get an intersection with a 3d line.
	/** Please note that also points are returned as intersection which
	are on the line, but not between the start and end point of the line.
	If you want the returned point be between start and end
	use getIntersectionWithLimitedLine().
	\param linePoint Point of the line to intersect with.
	\param lineVect Vector of the line to intersect with.
	\param outIntersection Place to store the intersection point, if there is one.
	\return True if there was an intersection, false if there was not. */
	bool getIntersectionWithLine(const Vector3d<T>& linePoint,
		const Vector3d<T>& lineVect, Vector3d<T>& outIntersection) const
	{
		if (getIntersectionOfPlaneWithLine(linePoint, lineVect, outIntersection))
			return isPointInside(outIntersection);

		return false;
	}


	// Calculates the intersection between a 3d line and the plane the triangle is on.
	/** \param lineVect Vector of the line to intersect with.
	\param linePoint Point of the line to intersect with.
	\param outIntersection Place to store the intersection point, if there is one.
	\return True if there was an intersection, else false. */
	bool getIntersectionOfPlaneWithLine(const Vector3d<T>& linePoint,
		const Vector3d<T>& lineVect, Vector3d<T>& outIntersection) const
	{
		// Work with double to get more precise results (makes enough difference to be worth the casts).
		const Vector3d<double> linePointf64(linePoint._x, linePoint._y, linePoint._z);
		const Vector3d<double> lineVectf64(lineVect._x, lineVect._y, lineVect._z);
		Vector3d<double> outIntersectionf64;

		Triangle3d<float> trianglef64(Vector3d<double>((double)_pointA._x, (double)_pointA._y, (double)_pointA._z)
									,Vector3d<double>((double)_pointB._x, (double)_pointB._y, (double)_pointB._z)
									, Vector3d<double>((double)_pointC._x, (double)_pointC._y, (double)_pointC._z));
		const Vector3d<double> normalf64 = trianglef64.getNormal().normalize();
		double t2;

		if ( isZero( t2 = normalf64.dotProduct(lineVectf64) ) )
			return false;

		double d = trianglef64._pointA.dotProduct(normalf64);
		double t = -(normalf64.dotProduct(linePointf64) - d) / t2;
		outIntersectionf64 = linePointf64 + (lineVectf64 * t);

		outIntersection._x = (T)outIntersectionf64._x;
		outIntersection._y = (T)outIntersectionf64._y;
		outIntersection._z = (T)outIntersectionf64._z;
		return true;
	}


	// Get the normal of the triangle.
	/** Please note: The normal is not always normalized. */
	Vector3d<T> getNormal() const
	{
		return (_pointB - _pointA).crossProduct(_pointC - _pointA);
	}

	// Test if the triangle would be front or backfacing from any point.
	/** Thus, this method assumes a camera position from which the
	triangle is definitely visible when looking at the given direction.
	Do not use this method with points as it will give wrong results!
	\param lookDirection Look direction.
	\return True if the plane is front facing and false if it is backfacing. */
	bool isFrontFacing(const Vector3d<T>& lookDirection) const
	{
		const Vector3d<T> n = getNormal().normalize();
		const float d = (float)n.dotProduct(lookDirection);
		return d <= 0.0f;
	}

	// Get the plane of this triangle.
	Plane3d<T> getPlane() const
	{
		return Plane3d<T>(_pointA, _pointB, _pointC);
	}

	// Get the area of the triangle
	T getArea() const
	{
		return (_pointB - _pointA).crossProduct(_pointC - _pointA).getLength() * 0.5f;
	}

	// sets the triangle's points
	void set(const Vector3d<T>& a, const Vector3d<T>& b, const Vector3d<T>& c)
	{
		_pointA = a;
		_pointB = b;
		_pointC = c;
	}

	// the three points of the triangle
	Vector3d<T> _pointA;
	Vector3d<T> _pointB;
	Vector3d<T> _pointC;

private:
	// Using double instead of <T> to avoid integer overflows when T=int (maybe also less floating point troubles).
	bool isOnSameSide(const Vector3d<double>& p1, const Vector3d<double>& p2,
		const Vector3d<double>& a, const Vector3d<double>& b) const
	{
		Vector3d<double> bminusa = b - a;
		Vector3d<double> cp1 = bminusa.crossProduct(p1 - a);
		Vector3d<double> cp2 = bminusa.crossProduct(p2 - a);
		double res = cp1.dotProduct(cp2);
		if ( res < 0 )
		{
			// This catches some floating point troubles.
			// Unfortunately slightly expensive and we don't really know the best epsilon for isZero.
			Vector3d<double> cp1 = bminusa.normalize().crossProduct((p1 - a).normalize());
			if (isZero(cp1._x) && isZero(cp1._y) && isZero(cp1._z))
			{
				res = 0.f;
			}
		}
		return (res >= 0.0f);
	}
};

typedef Triangle3d<float> Triangle3df;
typedef Triangle3d<int>   Triangle3di;

NS_VAYO_END

#endif // __VAYO_TRIANGLE3D_H__
