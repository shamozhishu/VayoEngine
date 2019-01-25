/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 轨迹球
\*************************************************************************/
#ifndef __VAYO3D_ARCBALL_H__
#define __VAYO3D_ARCBALL_H__

#include "Vayo3dSupport.h"
#include "math/VayoVector2d.h"
#include "math/VayoMatrix4x4.h"
NS_VAYO3D_BEGIN

class _Vayo3dExport Arcball
{
public:
	Arcball(float w, float h);
	~Arcball();
	void setBounds(float w, float h);
	void updateScale(float xscale, float yscale, float zscale);
	void updateRotate(float xscreen, float yscreen, bool isClick);
	void updateTranslate(float xpos, float ypos, float zpos);

private:
	void click(const Position2df& mousePos);
	void drag(const Position2df& mousePos, Matrix4x4& newRot);
	void mapToSphere(const Position2df& mousePos, Vector3df& newVec) const;

public:
	Matrix4x4    _transform;         // 最终变换
	Matrix4x4    _thisRot;           // 这次旋转
	Matrix4x4    _lastRot;           // 上次旋转
	Matrix4x4    _deltaRot;          // 旋转增量

private:
	Vector3df    _startVec;          // 保存鼠标点击的坐标
	Vector3df    _endVec;            // 保存鼠标拖动的坐标
	Position2df  _mousePos;          // 当前的鼠标位置
	float        _adjustWidth;       // 宽度的调整因子
	float        _adjustHeight;      // 高度的调整因子
	bool         _isDragging;        // 是否拖动
	bool         _isClicked;         // 是否点击鼠标
};

NS_VAYO3D_END

#endif // __VAYO3D_ARCBALL_H__
