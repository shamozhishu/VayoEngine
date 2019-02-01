/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 轴对齐绑定盒
\*************************************************************************/
#ifndef __VAYO_AABBOX3D_H__
#define __VAYO_AABBOX3D_H__

#include "VayoLine3d.h"
#include "VayoPlane3d.h"

namespace Vayo {

	// Axis aligned bounding box in 3d dimensional space.
	/** Has some useful methods used with occlusion culling or clipping.
	*/
	template<typename T>
	class Aabbox3d
	{
	public:
		Aabbox3d() : _minEdge(upperLimit<T>(), upperLimit<T>(), upperLimit<T>()),
			_maxEdge(-upperLimit<T>(), -upperLimit<T>(), -upperLimit<T>()) {}
		Aabbox3d(const Vector3d<T>& min, const Vector3d<T>& max) : _minEdge(min), _maxEdge(max) {}
		Aabbox3d(const Vector3d<T>& init) : _minEdge(init), _maxEdge(init) {}
		Aabbox3d(T minx, T miny, T minz, T maxx, T maxy, T maxz) : _minEdge(minx, miny, minz), _maxEdge(maxx, maxy, maxz) {}

		// operators
		// Equality operator
		/** \param other box to compare with.
		\return True if both boxes are equal, else false. */
		inline bool operator==(const Aabbox3d<T>& other) const { return (_minEdge == other._minEdge && other._maxEdge == _maxEdge); }
		// Inequality operator
		/** \param other box to compare with.
		\return True if both boxes are different, else false. */
		inline bool operator!=(const Aabbox3d<T>& other) const { return !(_minEdge == other._minEdge && other._maxEdge == _maxEdge); }

		// functions

		// Resets the bounding box to a one-point box.
		/** \param x _x coord of the point.
		\param y _y coord of the point.
		\param z _z coord of the point. */
		void reset(T x, T y, T z)
		{
			_maxEdge.set(x, y, z);
			_minEdge = _maxEdge;
		}

		// Resets the bounding box.
		/** \param initValue New box to set this one to. */
		void reset(const Aabbox3d<T>& initValue)
		{
			*this = initValue;
		}

		// Resets the bounding box to a one-point box.
		/** \param initValue New point. */
		void reset(const Vector3d<T>& initValue)
		{
			_maxEdge = initValue;
			_minEdge = initValue;
		}

		// Adds a point to the bounding box
		/** The box grows bigger, if point was outside of the box.
		\param p: Point to add into the box. */
		void addInternalPoint(const Vector3d<T>& p)
		{
			addInternalPoint(p._x, p._y, p._z);
		}

		// Adds another bounding box
		/** The box grows bigger, if the new box was outside of the box.
		\param b: Other bounding box to add into this box. */
		void addInternalBox(const Aabbox3d<T>& b)
		{
			if (!b.isEmpty())
			{
				addInternalPoint(b._maxEdge);
				addInternalPoint(b._minEdge);
			}
		}

		// Adds a point to the bounding box
		/** The box grows bigger, if point is outside of the box.
		\param x _x coordinate of the point to add to this box.
		\param y _y coordinate of the point to add to this box.
		\param z _z coordinate of the point to add to this box. */
		void addInternalPoint(T x, T y, T z)
		{
			if (x > _maxEdge._x) _maxEdge._x = x;
			if (y > _maxEdge._y) _maxEdge._y = y;
			if (z > _maxEdge._z) _maxEdge._z = z;

			if (x < _minEdge._x) _minEdge._x = x;
			if (y < _minEdge._y) _minEdge._y = y;
			if (z < _minEdge._z) _minEdge._z = z;
		}

		// Get center of the bounding box
		/** \return Center of the bounding box. */
		Vector3d<T> getCenter() const
		{
			return (_minEdge + _maxEdge) / 2;
		}

		// Get extent of the box (maximal distance of two points in the box)
		/** \return Extent of the bounding box. */
		Vector3d<T> getExtent() const
		{
			return _maxEdge - _minEdge;
		}

		// Check if the box is empty.
		/** This means that there is no space between the min and max edge.
		\return True if box is empty, else false. */
		bool isEmpty() const
		{
			return _minEdge >= _maxEdge;
		}

		void setEmpty()
		{
			T maxVal = upperLimit<T>();
			T minVal = -upperLimit<T>();
			_minEdge.set(maxVal, maxVal, maxVal);
			_maxEdge.set(minVal, minVal, minVal);
		}

		// Get the volume enclosed by the box in cubed units
		T getVolume() const
		{
			const Vector3d<T> e = getExtent();
			return e._x * e._y * e._z;
		}

		// Get the surface area of the box in squared units
		T getArea() const
		{
			const Vector3d<T> e = getExtent();
			return 2 * (e._x*e._y + e._x*e._z + e._y*e._z);
		}

		// Stores all 8 edges of the box into an array
		/** \param edges: Pointer to array of 8 edges. */
		void getEdges(Vector3d<T> *edges) const
		{
			const Vector3d<T> middle = getCenter();
			const Vector3d<T> diag = middle - _maxEdge;

			/*
			Edges are stored in this way:
			Hey, am I an ascii artist, or what? :) niko.
				   /3--------/7
				  / |       / |
				 /  |      /  |
				1---------5   |
				|  /2- - -|- -6
				| /       |  /
				|/        | /
				0---------4/
			*/

			edges[0].set(middle._x + diag._x, middle._y + diag._y, middle._z + diag._z);
			edges[1].set(middle._x + diag._x, middle._y - diag._y, middle._z + diag._z);
			edges[2].set(middle._x + diag._x, middle._y + diag._y, middle._z - diag._z);
			edges[3].set(middle._x + diag._x, middle._y - diag._y, middle._z - diag._z);
			edges[4].set(middle._x - diag._x, middle._y + diag._y, middle._z + diag._z);
			edges[5].set(middle._x - diag._x, middle._y - diag._y, middle._z + diag._z);
			edges[6].set(middle._x - diag._x, middle._y + diag._y, middle._z - diag._z);
			edges[7].set(middle._x - diag._x, middle._y - diag._y, middle._z - diag._z);
		}

		// Repairs the box.
		/** Necessary if for example MinEdge and MaxEdge are swapped. */
		void repair()
		{
			T t;

			if (_minEdge._x > _maxEdge._x)
			{
				t = _minEdge._x; _minEdge._x = _maxEdge._x; _maxEdge._x = t;
			}
			if (_minEdge._y > _maxEdge._y)
			{
				t = _minEdge._y; _minEdge._y = _maxEdge._y; _maxEdge._y = t;
			}
			if (_minEdge._z > _maxEdge._z)
			{
				t = _minEdge._z; _minEdge._z = _maxEdge._z; _maxEdge._z = t;
			}
		}

		// Calculates a new interpolated bounding box.
		/** d=0 returns other, d=1 returns this, all other values blend between
		the two boxes.
		\param other Other box to interpolate between
		\param d Value between 0.0f and 1.0f.
		\return Interpolated box. */
		Aabbox3d<T> getInterpolated(const Aabbox3d<T>& other, float d) const
		{
			float inv = 1.0f - d;
			return Aabbox3d<T>((other._minEdge*inv) + (_minEdge*d),
				(other._maxEdge*inv) + (_maxEdge*d));
		}

		// Determines if a point is within this box.
		/** Border is included (IS part of the box)!
		\param p: Point to check.
		\return True if the point is within the box and false if not */
		bool isPointInside(const Vector3d<T>& p) const
		{
			return (p._x >= _minEdge._x && p._x <= _maxEdge._x &&
				p._y >= _minEdge._y && p._y <= _maxEdge._y &&
				p._z >= _minEdge._z && p._z <= _maxEdge._z);
		}

		// Determines if a point is within this box and not its borders.
		/** Border is excluded (NOT part of the box)!
		\param p: Point to check.
		\return True if the point is within the box and false if not. */
		bool isPointTotalInside(const Vector3d<T>& p) const
		{
			return (p._x > _minEdge._x && p._x < _maxEdge._x &&
				p._y > _minEdge._y && p._y < _maxEdge._y &&
				p._z > _minEdge._z && p._z < _maxEdge._z);
		}

		// Check if this box is completely inside the 'other' box.
		/** \param other: Other box to check against.
		\return True if this box is completly inside the other box,
		otherwise false. */
		bool isFullInside(const Aabbox3d<T>& other) const
		{
			return (_minEdge._x >= other._minEdge._x && _minEdge._y >= other._minEdge._y && _minEdge._z >= other._minEdge._z &&
				_maxEdge._x <= other._maxEdge._x && _maxEdge._y <= other._maxEdge._y && _maxEdge._z <= other._maxEdge._z);
		}

		// Determines if the axis-aligned box intersects with another axis-aligned box.
		/** \param other: Other box to check a intersection with.
		\return True if there is an intersection with the other box,
		otherwise false. */
		bool intersectsWithBox(const Aabbox3d<T>& other) const
		{
			return (_minEdge._x <= other._maxEdge._x && _minEdge._y <= other._maxEdge._y && _minEdge._z <= other._maxEdge._z &&
				_maxEdge._x >= other._minEdge._x && _maxEdge._y >= other._minEdge._y && _maxEdge._z >= other._minEdge._z);
		}

		// Tests if the box intersects with a line
		/** \param line: Line to test intersection with.
		\return True if there is an intersection , else false. */
		bool intersectsWithLine(const Line3d<T>& line) const
		{
			return intersectsWithLine(line.getMiddle(), line.getVector().normalize(),
				(T)(line.getLength() * 0.5));
		}

		// Tests if the box intersects with a line
		/** \param linemiddle Center of the line.
		\param linevect Vector of the line.
		\param halflength Half length of the line.
		\return True if there is an intersection, else false. */
		bool intersectsWithLine(const Vector3d<T>& linemiddle,
			const Vector3d<T>& linevect, T halflength) const
		{
			const Vector3d<T> e = getExtent() * (T)0.5;
			const Vector3d<T> t = getCenter() - linemiddle;

			if ((fabs(t._x) > e._x + halflength * fabs(linevect._x)) ||
				(fabs(t._y) > e._y + halflength * fabs(linevect._y)) ||
				(fabs(t._z) > e._z + halflength * fabs(linevect._z)))
				return false;

			T r = e._y * (T)fabs(linevect._z) + e._z * (T)fabs(linevect._y);
			if (fabs(t._y*linevect._z - t._z*linevect._y) > r)
				return false;

			r = e._x * (T)fabs(linevect._z) + e._z * (T)fabs(linevect._x);
			if (fabs(t._z*linevect._x - t._x*linevect._z) > r)
				return false;

			r = e._x * (T)fabs(linevect._y) + e._y * (T)fabs(linevect._x);
			if (fabs(t._x*linevect._y - t._y*linevect._x) > r)
				return false;

			return true;
		}

		// Tests if the box intersects with a ray.
		bool intersectsWithRay(const Vector3d<T>& pos, const Vector3d<T>& dir) const
		{
			double ox = pos._x; double oy = pos._y; double oz = pos._z;
			double dx = dir._x; double dy = dir._y; double dz = dir._z;
			double tx_min, ty_min, tz_min;
			double tx_max, ty_max, tz_max;

			if (abs(dx) < 0.000001f)
			{
				// 若射线方向矢量的x轴分量为0且原点不在盒体内
				if (ox < _maxEdge._x || ox > _minEdge._x)
					return false;
			}
			else
			{
				if (dx >= 0)
				{
					tx_min = (_minEdge._x - ox) / dx;
					tx_max = (_maxEdge._x - ox) / dx;
				}
				else
				{
					tx_min = (_maxEdge._x - ox) / dx;
					tx_max = (_minEdge._x - ox) / dx;
				}
			}

			if (abs(dy) < 0.000001f)
			{
				// 若射线方向矢量的x轴分量为0且原点不在盒体内
				if (oy < _maxEdge._y || oy > _minEdge._y)
					return false;
			}
			else
			{
				if (dy >= 0)
				{
					ty_min = (_minEdge._y - oy) / dy;
					ty_max = (_maxEdge._y - oy) / dy;
				}
				else
				{
					ty_min = (_maxEdge._y - oy) / dy;
					ty_max = (_minEdge._y - oy) / dy;
				}
			}

			if (abs(dz) < 0.000001f)
			{
				// 若射线方向矢量的x轴分量为0且原点不在盒体内
				if (oz < _maxEdge._z || oz > _minEdge._z)
					return false;
			}
			else
			{
				if (dz >= 0)
				{
					tz_min = (_minEdge._z - oz) / dz;
					tz_max = (_maxEdge._z - oz) / dz;
				}
				else
				{
					tz_min = (_maxEdge._z - oz) / dz;
					tz_max = (_minEdge._z - oz) / dz;
				}
			}

			double t0, t1;

			// 光线进入平面处（最靠近的平面）的最大t值
			t0 = MAX(tz_min, MAX(tx_min, ty_min));

			// 光线离开平面处（最远离的平面）的最小t值
			t1 = MIN(tz_max, MIN(tx_max, ty_max));

			return t0 < t1;
		}

		// Classifies a relation with a plane.
		/** \param plane Plane to classify relation to.
		\return Returns ISREL3D_FRONT if the box is in front of the plane,
		ISREL3D_BACK if the box is behind the plane, and
		ISREL3D_CLIPPED if it is on both sides of the plane. */
		EIntersectionRelation classifyPlaneRelation(const Plane3d<T>& plane) const
		{
			Vector3d<T> nearPoint(_maxEdge);
			Vector3d<T> farPoint(_minEdge);

			if (plane._normal._x > (T)0)
			{
				nearPoint._x = _minEdge._x;
				farPoint._x = _maxEdge._x;
			}

			if (plane._normal._y > (T)0)
			{
				nearPoint._y = _minEdge._y;
				farPoint._y = _maxEdge._y;
			}

			if (plane._normal._z > (T)0)
			{
				nearPoint._z = _minEdge._z;
				farPoint._z = _maxEdge._z;
			}

			if (plane._normal.dotProduct(nearPoint) + plane._distance > (T)0)
				return EIR_FRONT;

			if (plane._normal.dotProduct(farPoint) + plane._distance > (T)0)
				return EIR_CLIPPED;

			return EIR_BACK;
		}

		// The near edge
		Vector3d<T> _minEdge;

		// The far edge
		Vector3d<T> _maxEdge;
	};

	typedef Aabbox3d<float> Aabbox3df;
	typedef Aabbox3d<int>   Aabbox3di;

}

#endif // __VAYO_AABBOX3D_H__
