/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 轨迹球
\*************************************************************************/
#ifndef __VAYO3D_ARCBALL_H__
#define __VAYO3D_ARCBALL_H__

#include "Vayo3dSupport.h"
#include "VayoVector2d.h"
#include "VayoMatrix4x4.h"
NS_VAYO3D_BEGIN

class _Vayo3dExport Arcball
{
public:
	Matrix4x4 _thisRot;
	Matrix4x4 _lastRot;
	Matrix4x4 _deltaRot;
	Matrix4x4 _finalRot;
	Matrix4x4 _finalPos;
	Matrix4x4 _finalZoom;

public:
	Arcball(float w, float h);
	~Arcball();
	void reset();
	void setBounds(float w, float h);
	void updateRotate(float xscreen, float yscreen, bool isClick);
	void updateRotate(float xrot, float yrot, float zrot);
	void updateScale(float xscale, float yscale, float zscale);
	void updateTranslate(float xpos, float ypos, float zpos);
	Matrix4x4 getFinalTransform();

private:
	void click(const Position2df& mousePos);
	void drag(const Position2df& mousePos, Matrix4x4& newRot);
	void mapToSphere(const Position2df& mousePos, Vector3df& newVec) const;

private:
	bool         _isClicked;    // 是否点击鼠标
	bool         _isDragging;   // 是否拖动鼠标
	float        _adjustWidth;  // 宽度的调整因子
	float        _adjustHeight; // 高度的调整因子
	Position2df  _mousePos;     // 当前的鼠标位置
	Vector3df    _startVec;     // 保存鼠标点击的坐标
	Vector3df    _endVec;       // 保存鼠标拖动的坐标
};

NS_VAYO3D_END

#endif // __VAYO3D_ARCBALL_H__
