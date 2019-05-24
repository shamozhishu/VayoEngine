#include "Vayo3dArcball.h"

NS_VAYO3D_BEGIN

// 轨迹球参数:
// 直径                   2.0f
// 半径                   1.0f
// 半径平方               1.0f
void Arcball::mapToSphere(const Position2df& mousePos, Vector3df& newVec) const
{
	Position2df tmpPt;

	// 把宽高映射到[-1~1]区间
	tmpPt._x = (mousePos._x * _adjustWidth) - 1.0f;
	tmpPt._y = 1.0f - (mousePos._y * _adjustHeight);

	float length = tmpPt.getLengthSQ();

	if (length > 1.0f) // 如果映射在球外
	{
		float norm = 1.0f / sqrtf(length);
		newVec._x = tmpPt._x / norm;
		newVec._y = tmpPt._y / norm;
		newVec._z = 0.0f;
	}
	else // 如果映射在球内
	{
		newVec._x = tmpPt._x;
		newVec._y = tmpPt._y;
		newVec._z = sqrtf(1.0f - length);
	}

	newVec.normalize();
}

Arcball::Arcball(float w, float h)
	: _isClicked(false)
	, _isDragging(false)
{
	setBounds(w, h);
}

Arcball::~Arcball()
{
}

void Arcball::reset()
{
	_thisRot.makeIdentity();
	_lastRot.makeIdentity();
	_deltaRot.makeIdentity();
	_finalRot.makeIdentity();
	_finalPos.makeIdentity();
	_finalZoom.makeIdentity();
}

void Arcball::setBounds(float w, float h)
{
	_adjustWidth = 1.0f / ((w - 1.0f) * 0.5f);
	_adjustHeight = 1.0f / ((h - 1.0f) * 0.5f);
}

void Arcball::updateRotate(float xscreen, float yscreen, bool isClick)
{
	_isClicked = isClick;
	_mousePos.set(xscreen, yscreen);

	if (!_isDragging && _isClicked)
	{
		_isDragging = true;
		_lastRot = _thisRot;
		this->click(_mousePos);
	}
	else if (_isDragging)
	{
		if (_isClicked)
		{
			this->drag(_mousePos, _deltaRot);
			_thisRot = _deltaRot;
			_thisRot *= _lastRot;
			Vector3df curPos = _finalRot.getTranslation();
			_finalRot = _thisRot;
			_finalRot.setTranslation(curPos);
		}
		else
		{
			_isDragging = false;
		}
	}
}

void Arcball::updateRotate(float xrot, float yrot, float zrot)
{
	_thisRot.setRotationDegrees(Vector3df(xrot, yrot, zrot));
	Vector3df curPos = _finalRot.getTranslation();
	_finalRot = _lastRot = _thisRot;
	_finalRot.setTranslation(curPos);
}

void Arcball::updateScale(float xscale, float yscale, float zscale)
{
	Matrix4x4 zoomMat;
	zoomMat.setScale(Vector3df(xscale, yscale, zscale));
	_finalZoom *= zoomMat;
}

void Arcball::updateTranslate(float xpos, float ypos, float zpos)
{
	_finalPos.setTranslation(_finalPos.getTranslation() + Vector3df(xpos, ypos, zpos));
}

Matrix4x4 Arcball::getFinalTransform()
{
	if (_finalZoom == IdentityMat4)
		return _finalPos * _finalRot;
	return _finalZoom * _finalPos * _finalRot;
}

void Arcball::click(const Position2df& mousePos)
{
	mapToSphere(mousePos, _startVec);
}

void Arcball::drag(const Position2df& mousePos, Matrix4x4& newRot)
{
	mapToSphere(mousePos, _endVec);
	Vector3df rotAxis = _startVec.crossProduct(_endVec);

	if (isZero(rotAxis.getLength()))
	{
		newRot = Matrix4x4();
	}
	else
	{
		float angle = (float)acos(_startVec.dotProduct(_endVec));
		newRot.setRotationRadians(angle, rotAxis.normalize());
	}
}

NS_VAYO3D_END
