#include "Vayo2dWatcher.h"
#include "Vayo2dLayerManager.h"
#include "Vayo2dRenderer.h"
#include "Vayo2dPivot.h"
#include "Vayo2dLayer.h"

NS_VAYO2D_BEGIN

void Watcher::setNeedRefresh(bool isRefresh)
{
	_needRefresh = isRefresh;
}

Watcher::Watcher(const wstring& name, LayerManager* oriLayerMgr)
	: Body(name, oriLayerMgr)
	, TouchDelegate(oriLayerMgr->getName())
	, KeypadDelegate(oriLayerMgr->getName())
	, _aspect(1.0f)
	, _rotation(0.0f)
	, _needRefresh(true)
{
	Device* pDev = Pivot::getSingleton().getActiveDevice();
	if (pDev)
	{
		Dimension2di size = pDev->getScreenSize();
		_window = _viewport = Rectf(Position2df::Origin, Dimension2df(size._width, size._height));
		_aspect = _window.getWidth() / _viewport.getWidth();
	}
}

Watcher::~Watcher()
{
}

void Watcher::refresh()
{
	Matrix3x3 mat;
	Layer* pParent = getParentLayer();
	if (pParent)
		mat = _view * pParent->getAbsTransform() * _affector;
	else
		mat = _view * _affector;

	if (_needRefresh)
	{
		_needRefresh = false;
		float scaleX = (_viewport._lowerRightCorner._x - _viewport._upperLeftCorner._x) / (_window._lowerRightCorner._x - _window._upperLeftCorner._x);
		float scaleY = (_viewport._upperLeftCorner._y - _viewport._lowerRightCorner._y) / (_window._upperLeftCorner._y - _window._lowerRightCorner._y);
		float translateX = _viewport._upperLeftCorner._x - scaleX * _window._upperLeftCorner._x;
		float translateY = _viewport._lowerRightCorner._y - scaleY * _window._lowerRightCorner._y;
		_orthoProj.makeIdentity();
		_orthoProj.setScale(Vector2df(scaleX, scaleY));
		_orthoProj.setTranslation(Vector2df(translateX, translateY));
	}

	Pivot::getSingleton().getActiveRenderer()->setTransform(ETK_VIEW, mat);
	Pivot::getSingleton().getActiveRenderer()->setTransform(ETK_PROJECTION, _orthoProj);
}

void Watcher::moveTo(const Vector2df& pos)
{
	_view.setTranslation(pos);
}

void Watcher::moveBy(const Vector2df& pos)
{
	Vector2df curpos = _view.getTranslation();
	curpos += pos;
	_view.setTranslation(curpos);
}

void Watcher::scaleTo(const Vector2df& scale)
{
	_view.setScale(scale);
}

void Watcher::scaleBy(const Vector2df& scale)
{
	Vector2df curscale = _view.getScale();
	curscale += scale;
	_view.setScale(curscale);
}

void Watcher::rotateTo(float rot)
{
	_rotation = rot;
	_view.setRotationDegrees(_rotation);
}

void Watcher::rotateBy(float rot)
{
	_rotation += rot;
	_view.setRotationDegrees(_rotation);
}

const Matrix3x3& Watcher::getView() const
{
	return _view;
}

void Watcher::setWindow(Rectf window)
{
	_window = window;
	_aspect = _window.getWidth() / _viewport.getWidth();
	_needRefresh = true;
}

void Watcher::setViewport(Rectf viewport)
{
	_viewport = viewport;
	_aspect = _window.getWidth() / _viewport.getWidth();
	_needRefresh = true;
}

void Watcher::setViewAffector(const Matrix3x3& affector)
{
	_affector = affector;
}

const Matrix3x3& Watcher::getViewAffector() const
{
	return _affector;
}

//////////////////////////////////////////////////////////////////////////
Reflex<BirdEyeWatcher, const wstring&, LayerManager*> BirdEyeWatcher::_dynReflex;
BirdEyeWatcher::BirdEyeWatcher(const wstring& name, LayerManager* oriLayerMgr)
	: Watcher(name, oriLayerMgr)
	, _zoomSpeed(20.0f)
	, _enableMoved(false)
	, _zoomByMousePt(false)
{
}

BirdEyeWatcher::~BirdEyeWatcher()
{
}

void BirdEyeWatcher::update(float dt)
{
	if (Pivot::getSingleton().getCurLayerMgr()->getActiveWatcher() == this)
		refresh();
}

void BirdEyeWatcher::setZoomSpeed(float speed)
{
	_zoomSpeed = speed;
}

bool BirdEyeWatcher::touchBegan(const Touch& touch, EMouseKeys key)
{
	if (key == EMK_RIGHT)
		_enableMoved = true;
	return _enableMoved;
}

void BirdEyeWatcher::touchMoved(const Touch& touch, EMouseKeys key)
{
	if (_enableMoved)
	{
		Position2di pos = touch.getDelta();
		_window += Position2df(-pos._x, -pos._y) * _aspect;
		setWindow(_window);
	}
}

void BirdEyeWatcher::touchEnded(const Touch& touch, EMouseKeys key)
{
	if (key == EMK_RIGHT)
		_enableMoved = false;
}

bool BirdEyeWatcher::touchWheel(const Touch& touch, float wheel)
{
	Device* pDev = Pivot::getSingleton().getActiveDevice();
	if (pDev)
	{
		Dimension2df screenSize(pDev->getScreenSize()._width, pDev->getScreenSize()._height);
		Position2df cecterPos = Position2df(screenSize._width * 0.5f, screenSize._height * 0.5f);
		float aspect = _window.getHeight() / _window.getWidth();
		float w = _viewport.getWidth() + wheel * _zoomSpeed;
		if (w <= 0.0f)
			w = _viewport.getWidth();
		float h = w * aspect;
		float halfW = w * 0.5f;
		float halfH = h * 0.5f;
		setViewport(Rectf(cecterPos._x - halfW, cecterPos._y - halfH, cecterPos._x + halfW, cecterPos._y + halfH));
		return true;
	}
	return false;
}

bool BirdEyeWatcher::keyClicked(const tagKeyInput& keyInput)
{
	if (keyInput.Control && keyInput.PressedDown)
		_zoomByMousePt = true;
	else
		_zoomByMousePt = false;
	return false;
}

NS_VAYO2D_END
