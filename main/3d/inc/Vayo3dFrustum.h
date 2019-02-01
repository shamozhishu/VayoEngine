/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 平截头体
\*************************************************************************/
#ifndef __VAYO3D_FRUSTUM_H__
#define __VAYO3D_FRUSTUM_H__

#include "VayoCommon.h"
#include "math/VayoMatrix4x4.h"
NS_VAYO3D_BEGIN

class _Vayo3dExport Frustum
{
public:
	enum EViewPlanes
	{
		EVP_FAR_PLANE = 0,
		EVP_NEAR_PLANE,
		EVP_LEFT_PLANE,
		EVP_RIGHT_PLANE,
		EVP_BOTTOM_PLANE,
		EVP_TOP_PLANE,
		EVP_PLANE_COUNT
	};

	enum EFrustumTransformation
	{
		EFT_VIEW,
		EFT_PROJECTION,
		EFT_COUNT
	};

	Frustum() {}
	Frustum(const Frustum& other);
	Frustum(const Matrix4x4& mat);
	void             setFrom(const Matrix4x4& mat);
	void             transform(const Matrix4x4& mat);
	Vector3df        getFarLeftUp() const;
	Vector3df        getFarLeftDown() const;
	Vector3df        getFarRightUp() const;
	Vector3df        getFarRightDown() const;
	Vector3df        getNearLeftUp() const;
	Vector3df        getNearLeftDown() const;
	Vector3df        getNearRightUp() const;
	Vector3df        getNearRightDown() const;
	const Aabbox3df& getBoundingBox() const;
	void             recalculateBoundingBox();
	Matrix4x4&       getTransform(EFrustumTransformation state);
	const Matrix4x4& getTransform(EFrustumTransformation state) const;
	bool             clipLine(Line3df& line) const;

public:
	Vector3df _cameraPosition;
	Plane3df  _planes[EVP_PLANE_COUNT];
	Aabbox3df _boundingBox;
private:
	Matrix4x4 _matrices[EFT_COUNT];
};

NS_VAYO3D_END

#endif // __VAYO3D_FRUSTUM_H__
