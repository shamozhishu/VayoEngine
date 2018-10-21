#include "VayoArcball.h"

NS_VAYO_BEGIN

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
	: _isDragging(false)
	, _isClicked(false)
{
	setBounds(w, h);
}

Arcball::~Arcball()
{
}

void Arcball::setBounds(float w, float h)
{
	_adjustWidth = 1.0f / ((w - 1.0f) * 0.5f);
	_adjustHeight = 1.0f / ((h - 1.0f) * 0.5f);
}

void Arcball::updateScale(float xscale, float yscale, float zscale)
{
	_transform.setScale(Vector3df(xscale, yscale, zscale));
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
			Vector3df curPos = _transform.getTranslation();
			_transform = _thisRot;
			_transform.setTranslation(curPos);
		}
		else
		{
			_isDragging = false;
		}
	}
}

void Arcball::updateTranslate(float xpos, float ypos, float zpos)
{
	Vector3df vec(_adjustWidth * xpos, _adjustHeight * ypos, _adjustHeight * zpos);
	_transform.setTranslation(_transform.getTranslation() + vec);
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

NS_VAYO_END
