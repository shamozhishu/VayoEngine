#include "Vayo2dWatcher.h"
#include "Vayo2dLayerManager.h"
#include "Vayo2dRenderer.h"
#include "Vayo2dPivot.h"
#include "Vayo2dLayer.h"

NS_VAYO2D_BEGIN

Reflex<Watcher, const wstring&, LayerManager*> Watcher::_dynReflex;
Watcher::Watcher(const wstring& name, LayerManager* oriLayerMgr)
	: Body(name, oriLayerMgr)
	, TouchDelegate(oriLayerMgr->getName())
	, KeypadDelegate(oriLayerMgr->getName())
	, _rotation(0.0f)
	, _zoomFactor(1.0f)
	, _needRefresh(true)
	, _enableMoved(false)
{
}

Watcher::~Watcher()
{
}

void Watcher::refresh()
{
	if (_needRefresh)
	{
		_needRefresh = false;

		Matrix3x3 mat;
		Layer* pParent = getParentLayer();
		if (pParent)
			mat = _view * pParent->getAbsTransform() * _affector;
		else
			mat = _view * _affector;

		Pivot::getSingleton().getActiveRenderer()->setTransform(ETK_VIEW, mat);
	}
}

void Watcher::update(float dt)
{
	if (Pivot::getSingleton().getCurLayerMgr()->getActiveWatcher() == this)
		refresh();
}

void Watcher::setNeedRefresh(bool isRefresh)
{
	_needRefresh = isRefresh;
}

void Watcher::moveTo(const Vector2df& pos)
{
	_view.setTranslation(pos);
	_needRefresh = true;
}

void Watcher::moveBy(const Vector2df& pos)
{
	Vector2df curpos = _view.getTranslation();
	curpos += pos;
	_view.setTranslation(curpos);
	_needRefresh = true;
}

void Watcher::scaleTo(const Vector2df& scale)
{
	_view.setScale(scale);
	_needRefresh = true;
}

void Watcher::scaleBy(const Vector2df& scale)
{
	Vector2df curscale = _view.getScale();
	curscale += scale;
	_view.setScale(curscale);
	_needRefresh = true;
}

void Watcher::rotateTo(float rot)
{
	_rotation = rot;
	_view.setRotationDegrees(_rotation);
	_needRefresh = true;
}

void Watcher::rotateBy(float rot)
{
	_rotation += rot;
	_view.setRotationDegrees(_rotation);
	_needRefresh = true;
}

const Matrix3x3& Watcher::getView() const
{
	return _view;
}

void Watcher::setViewAffector(const Matrix3x3& affector)
{
	_affector = affector;
}

const Matrix3x3& Watcher::getViewAffector() const
{
	return _affector;
}

bool Watcher::touchBegan(const Touch& touch, EMouseKeys key)
{
	if (key == EMK_RIGHT)
		_enableMoved = true;
	return _enableMoved;
}

void Watcher::touchMoved(const Touch& touch, EMouseKeys key)
{
	if (_enableMoved)
	{
		Position2di pos = touch.getDelta();
		moveBy(Vector2df(pos._x, pos._y));
	}
}

void Watcher::touchEnded(const Touch& touch, EMouseKeys key)
{
	if (key == EMK_RIGHT)
		_enableMoved = false;
}

bool Watcher::touchWheel(const Touch& touch, float wheel)
{
	float oldzoom = _zoomFactor;
	_zoomFactor += wheel;
	if (_zoomFactor <= 0.0f)
		_zoomFactor = 0.0f;

	scaleTo(Vector2df(_zoomFactor, _zoomFactor));
	return true;
}

bool Watcher::keyClicked(const tagKeyInput& keyInput)
{
	return false;
}

NS_VAYO2D_END
