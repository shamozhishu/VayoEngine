/*************************************************************************\
* ��������1.0
* Copyright (c) 2018-2019 authored by ��Ӻ�
* �켣��
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
	Matrix4x4    _transform;         // ���ձ任
	Matrix4x4    _thisRot;           // �����ת
	Matrix4x4    _lastRot;           // �ϴ���ת
	Matrix4x4    _deltaRot;          // ��ת����

private:
	Vector3df    _startVec;          // ���������������
	Vector3df    _endVec;            // ��������϶�������
	Position2df  _mousePos;          // ��ǰ�����λ��
	float        _adjustWidth;       // ��ȵĵ�������
	float        _adjustHeight;      // �߶ȵĵ�������
	bool         _isDragging;        // �Ƿ��϶�
	bool         _isClicked;         // �Ƿ������
};

NS_VAYO3D_END

#endif // __VAYO3D_ARCBALL_H__
