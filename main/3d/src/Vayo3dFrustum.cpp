#include "Vayo3dFrustum.h"

NS_VAYO3D_BEGIN

void Frustum::setFrom(const Matrix4x4& mat)
{
	// left clipping plane
	_planes[EVP_LEFT_PLANE]._normal._x = mat[3] + mat[0];
	_planes[EVP_LEFT_PLANE]._normal._y = mat[7] + mat[4];
	_planes[EVP_LEFT_PLANE]._normal._z = mat[11] + mat[8];
	_planes[EVP_LEFT_PLANE]._distance = mat[15] + mat[12];

	// right clipping plane
	_planes[EVP_RIGHT_PLANE]._normal._x = mat[3] - mat[0];
	_planes[EVP_RIGHT_PLANE]._normal._y = mat[7] - mat[4];
	_planes[EVP_RIGHT_PLANE]._normal._z = mat[11] - mat[8];
	_planes[EVP_RIGHT_PLANE]._distance = mat[15] - mat[12];

	// top clipping plane
	_planes[EVP_TOP_PLANE]._normal._x = mat[3] - mat[1];
	_planes[EVP_TOP_PLANE]._normal._y = mat[7] - mat[5];
	_planes[EVP_TOP_PLANE]._normal._z = mat[11] - mat[9];
	_planes[EVP_TOP_PLANE]._distance = mat[15] - mat[13];

	// bottom clipping plane
	_planes[EVP_BOTTOM_PLANE]._normal._x = mat[3] + mat[1];
	_planes[EVP_BOTTOM_PLANE]._normal._y = mat[7] + mat[5];
	_planes[EVP_BOTTOM_PLANE]._normal._z = mat[11] + mat[9];
	_planes[EVP_BOTTOM_PLANE]._distance = mat[15] + mat[13];

	// far clipping plane
	_planes[EVP_FAR_PLANE]._normal._x = mat[3] - mat[2];
	_planes[EVP_FAR_PLANE]._normal._y = mat[7] - mat[6];
	_planes[EVP_FAR_PLANE]._normal._z = mat[11] - mat[10];
	_planes[EVP_FAR_PLANE]._distance = mat[15] - mat[14];

	// near clipping plane
	_planes[EVP_NEAR_PLANE]._normal._x = mat[2];
	_planes[EVP_NEAR_PLANE]._normal._y = mat[6];
	_planes[EVP_NEAR_PLANE]._normal._z = mat[10];
	_planes[EVP_NEAR_PLANE]._distance = mat[14];

	// normalize normals
	for (unsigned int i = 0; i != EVP_PLANE_COUNT; ++i)
	{
		const float len = -1.f / sqrtf(_planes[i]._normal.getLengthSQ());
		_planes[i]._normal *= len;
		_planes[i]._distance *= len;
	}

	// make bounding box
	recalculateBoundingBox();
}

Frustum::Frustum(const Frustum& other)
{
	_cameraPosition = other._cameraPosition;
	_boundingBox = other._boundingBox;

	unsigned int i;
	for (i = 0; i < EVP_PLANE_COUNT; ++i)
		_planes[i] = other._planes[i];

	for (i = 0; i < EFT_COUNT; ++i)
		_matrices[i] = other._matrices[i];
}

Frustum::Frustum(const Matrix4x4& mat)
{
	setFrom(mat);
}

void Frustum::transform(const Matrix4x4& mat)
{
	for (unsigned int i = 0; i < EVP_PLANE_COUNT; ++i)
		mat.transformPlane(_planes[i]);
	mat.transformVect(_cameraPosition);
	recalculateBoundingBox();
}

Vector3df Frustum::getFarLeftUp() const
{
	Vector3df p;
	_planes[EVP_FAR_PLANE].getIntersectionWithPlanes(
		_planes[EVP_TOP_PLANE],
		_planes[EVP_LEFT_PLANE], p);
	return p;
}

Vector3df Frustum::getFarLeftDown() const
{
	Vector3df p;
	_planes[EVP_FAR_PLANE].getIntersectionWithPlanes(
		_planes[EVP_BOTTOM_PLANE],
		_planes[EVP_LEFT_PLANE], p);
	return p;
}

Vector3df Frustum::getFarRightUp() const
{
	Vector3df p;
	_planes[EVP_FAR_PLANE].getIntersectionWithPlanes(
		_planes[EVP_TOP_PLANE],
		_planes[EVP_RIGHT_PLANE], p);
	return p;
}

Vector3df Frustum::getFarRightDown() const
{
	Vector3df p;
	_planes[EVP_FAR_PLANE].getIntersectionWithPlanes(
		_planes[EVP_BOTTOM_PLANE],
		_planes[EVP_RIGHT_PLANE], p);
	return p;
}

Vector3df Frustum::getNearLeftUp() const
{
	Vector3df p;
	_planes[EVP_NEAR_PLANE].getIntersectionWithPlanes(
		_planes[EVP_TOP_PLANE],
		_planes[EVP_LEFT_PLANE], p);
	return p;
}

Vector3df Frustum::getNearLeftDown() const
{
	Vector3df p;
	_planes[EVP_NEAR_PLANE].getIntersectionWithPlanes(
		_planes[EVP_BOTTOM_PLANE],
		_planes[EVP_LEFT_PLANE], p);
	return p;
}

Vector3df Frustum::getNearRightUp() const
{
	Vector3df p;
	_planes[EVP_NEAR_PLANE].getIntersectionWithPlanes(
		_planes[EVP_TOP_PLANE],
		_planes[EVP_RIGHT_PLANE], p);
	return p;
}

Vector3df Frustum::getNearRightDown() const
{
	Vector3df p;
	_planes[EVP_NEAR_PLANE].getIntersectionWithPlanes(
		_planes[EVP_BOTTOM_PLANE],
		_planes[EVP_RIGHT_PLANE], p);
	return p;
}

const Aabbox3df& Frustum::getBoundingBox() const
{
	return _boundingBox;
}

void Frustum::recalculateBoundingBox()
{
	_boundingBox.reset(_cameraPosition);
	_boundingBox.addInternalPoint(getFarLeftUp());
	_boundingBox.addInternalPoint(getFarRightUp());
	_boundingBox.addInternalPoint(getFarLeftDown());
	_boundingBox.addInternalPoint(getFarRightDown());
}

Matrix4x4& Frustum::getTransform(EFrustumTransformation state)
{
	VAYO_ASSERT((state == EFT_VIEW || state == EFT_PROJECTION) && L"Index out of bounds(Frustum::getTransform).");
	return _matrices[state];
}

const Matrix4x4& Frustum::getTransform(EFrustumTransformation state) const
{
	VAYO_ASSERT((state == EFT_VIEW || state == EFT_PROJECTION) && L"Index out of bounds(Frustum::getTransform).");
	return _matrices[state];
}

bool Frustum::clipLine(Line3df& line) const
{
	bool wasClipped = false;
	for (unsigned int i = 0; i < EVP_PLANE_COUNT; ++i)
	{
		if (_planes[i].classifyPointRelation(line._start) == EIR_FRONT)
		{
			line._start = line._start.getInterpolated(line._end,
				_planes[i].getKnownIntersectionWithLine(line._start, line._end));
			wasClipped = true;
		}
		if (_planes[i].classifyPointRelation(line._end) == EIR_FRONT)
		{
			line._end = line._start.getInterpolated(line._end,
				_planes[i].getKnownIntersectionWithLine(line._start, line._end));
			wasClipped = true;
		}
	}
	return wasClipped;
}

NS_VAYO3D_END
