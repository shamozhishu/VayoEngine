/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 视图备忘录
\*************************************************************************/
#ifndef __VAYO_VIEW_MEMENTO_H__
#define __VAYO_VIEW_MEMENTO_H__

#include "VayoSharedPtr.h"
#include "VayoMatrix4x4.h"
#include "VayoNode.h"
NS_VAYO_BEGIN

// 视图备忘录基类
class _VayoExport ViewMemento
{
public:
	virtual ~ViewMemento() {}
	virtual void storage(Camera* camera);
	virtual void recover(Camera* camera);

protected:
	Vector3df _position;
	Vector3df _right;
	Vector3df _up;
	Vector3df _look;
	Matrix4x4 _affector;
};

// 第一人称摄像机视图备忘录
class _VayoExport ViewMementoFPS : public ViewMemento
{
public:
	void storage(Camera* camera);
	void recover(Camera* camera);

protected:
	void setSpatialInfo(Node* pParent);
	void getSpatialInfo(Node* pParent);

protected:
	float _moveSpeed;
	vector<SpatialInfo> _placement;
};

// 轨道摄像机视图备忘录
class _VayoExport ViewMementoOrbit : public ViewMementoFPS
{
public:
	void storage(Camera* camera);
	void recover(Camera* camera);

protected:
	float     _zoomSpeed;
	Matrix4x4 _thisRot;
	Matrix4x4 _lastRot;
	Matrix4x4 _transform;
};

// 鹰眼摄像机视图备忘录
class _VayoExport ViewMementoEagleEye : public ViewMementoFPS
{
public:
	void storage(Camera* camera);
	void recover(Camera* camera);

protected:
	float     _zoomSpeed;
	float     _zoomFactor;
	Matrix4x4 _transform;
};

NS_VAYO_END

#endif // __VAYO_VIEW_MEMENTO_H__
