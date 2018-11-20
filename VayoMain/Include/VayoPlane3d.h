/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 authored by 朱加浩
* 三维平面
\*************************************************************************/
#ifndef __VAYO_PLANE3D_H__
#define __VAYO_PLANE3D_H__

#include "VayoVector3d.h"
NS_VAYO_BEGIN

enum EIntersectionRelation
{
	EIR_FRONT = 0,
	EIR_BACK,
	EIR_PLANAR,
	EIR_SPANNING,
	EIR_CLIPPED
};

template<typename T>
class Plane3d
{
public:
	Plane3d(): _normal(0,1,0) { recalculateD(Vector3d<T>(0,0,0)); }
	Plane3d(const Vector3d<T>& MPoint, const Vector3d<T>& normal) : _normal(normal) { recalculateD(MPoint); }
	Plane3d(T px, T py, T pz, T nx, T ny, T nz) : _normal(nx, ny, nz) { recalculateD(Vector3d<T>(px, py, pz)); }
	Plane3d(const Plane3d<T>& other) :_distance(other._distance), _normal(other._normal) {}
	Plane3d(const Vector3d<T>& point1, const Vector3d<T>& point2, const Vector3d<T>& point3) { setPlane(point1, point2, point3); }

	// operators

	inline bool operator==(const Plane3d<T>& other) const { return (equals(_distance, other._distance) && _normal==other._normal); }
	inline bool operator!=(const Plane3d<T>& other) const { return (!equals(_distance, other._distance) || _normal!=other._normal); }

	// functions

	void setPlane(const Vector3d<T>& point, const Vector3d<T>& nvector)
	{
		_normal = nvector;
		_normal.normalize();
		recalculateD(point);
	}

	void setPlane(const Vector3d<T>& nvect, T d)
	{
		_normal = nvect;
		_distance = d;
	}

	void setPlane(const Vector3d<T>& point1, const Vector3d<T>& point2, const Vector3d<T>& point3)
	{
		_normal = (point2 - point1).crossProduct(point3 - point1);
		_normal.normalize();

		recalculateD(point1);
	}

	// Returns an intersection with a 3d line.
	// param lineVect: Vector of the line to intersect with.
	// param linePoint: Point of the line to intersect with.
	// param outIntersection: Place to store the intersection point, if there is one.
	// return: Returns true if there was an intersection, false if there was not.
	bool getIntersectionWithLine(const Vector3d<T>& linePoint, const Vector3d<T>& lineVect,
		Vector3d<T>& outIntersection) const
	{
		T t2 = _normal.dotProduct(lineVect);

		if (equals(t2, 0.0f))
			return false;

		T t = -(_normal.dotProduct(linePoint) + _distance) / t2;			
		outIntersection = linePoint + (lineVect * t);
		return true;
	}

	// Get percentage of line between two points where an intersection with this plane happens.
	/** Only useful if known that there is an intersection.
	\param linePoint1 Point1 of the line to intersect with.
	\param linePoint2 Point2 of the line to intersect with.
	\return Where on a line between two points an intersection with this plane happened.
	For example, 0.5 is returned if the intersection happened exactly in the middle of the two points.
	*/
	float getKnownIntersectionWithLine(const Vector3d<T>& linePoint1,
		const Vector3d<T>& linePoint2) const
	{
		Vector3d<T> vect = linePoint2 - linePoint1;
		T t2 = (float)_normal.dotProduct(vect);
		return (float)-((_normal.dotProduct(linePoint1) + _distance) / t2);
	}

	// Returns an intersection with a 3d line, limited between two 3d points.
	// param linePoint1: Point 1 of the line.
	// param linePoint2: Point 2 of the line.
	// param outIntersection: Place to store the intersection point, if there is one.
	// return: Returns true if there was an intersection, false if there was not.
	bool getIntersectionWithLimitedLine(const Vector3d<T>& linePoint1, 
		const Vector3d<T>& linePoint2, Vector3d<T>& outIntersection) const
	{
		return (getIntersectionWithLine(linePoint1, linePoint2 - linePoint1, outIntersection) &&
			outIntersection.isBetweenPoints(linePoint1, linePoint2));
	}

	// Classifies the relation of a point to this plane.
	// param point: Point to classify its relation.
	// return: Returns ISREL3D_FRONT if the point is in front of the plane,
	// ISREL3D_BACK if the point is behind of the plane, and
	// ISREL3D_PLANAR if the point is within the plane.
	EIntersectionRelation classifyPointRelation(const Vector3d<T>& point) const
	{
		float d = _normal.dotProduct(point) + _distance;

		if (d < -ROUNDING_ERROR_FLOAT)
			return EIR_BACK;

		if (d > ROUNDING_ERROR_FLOAT)
			return EIR_FRONT;

		return EIR_PLANAR;
	}

	// Recalculates the distance from origin by applying
	// a new member point to the plane.
	void recalculateD(const Vector3d<T>& MPoint)
	{
		_distance = - MPoint.dotProduct(_normal);
	}

	// return: Returns a member point of the plane.
	Vector3d<T> getMemberPoint() const
	{
		return _normal * -_distance;
	}

	// Tests if there is a intersection between this plane and another
	// return: Returns true if there is a intersection.
	bool existsInterSection(const Plane3d<T>& other) const
	{
		Vector3d<T> cross = other._normal.crossProduct(_normal);
		return cross.getLength() > 1e-08f;
	}

	// Intersects this plane with another.
	// return: Returns true if there is a intersection, false if not.
	bool getIntersectionWithPlane(const Plane3d<T>& other, Vector3d<T>& outLinePoint,
		Vector3d<T>& outLineVect) const
	{
		float fn00 = _normal.getLength();
		float fn01 = _normal.dotProduct(other._normal);
		float fn11 = other._normal.getLength();
		float det = fn00*fn11 - fn01*fn01;

		if (fabsf(det) < 1e-08f)
			return false;

		det = 1.0 / det;
		float fc0 = (fn11*-_distance + fn01*other._distance) * det;
		float fc1 = (fn00*-other._distance + fn01*_distance) * det;

		outLineVect = _normal.crossProduct(other._normal);
		outLinePoint = _normal*(T)fc0 + other._normal*(T)fc1;
		return true;
	}

	// Returns the intersection point with two other planes if there is one.
	bool getIntersectionWithPlanes(const Plane3d<T>& o1, 
		const Plane3d<T>& o2, Vector3d<T>& outPoint) const
	{
		Vector3d<T> linePoint, lineVect;
		if (getIntersectionWithPlane(o1, linePoint, lineVect))
			return o2.getIntersectionWithLine(linePoint, lineVect, outPoint);

		return false;
	}

	// Returns if the plane is front of backfacing. Note that this only
	// works if the normal is Normalized.
	// param lookDirection: Look direction.
	// return: Returns true if the plane is front facing, which mean it would
	// be visible, and false if it is backfacing.
	bool isFrontFacing(const Vector3d<T>& lookDirection) const
	{
		return _normal.dotProduct(lookDirection) <= 0.0f;
	}

	// Returns the distance to a point. Note that this only
	// works if the normal is Normalized.
	T getDistanceTo(const Vector3d<T>& point) const
	{
		return point.dotProduct(_normal) + _distance;
	}

	// member variables

	T           _distance;      // distance from origin
	Vector3d<T> _normal;        // normal vector
};

typedef Plane3d<float>  Plane3df;
typedef Plane3d<int>    Plane3di;

NS_VAYO_END

#endif // __VAYO_PLANE3D_H__
